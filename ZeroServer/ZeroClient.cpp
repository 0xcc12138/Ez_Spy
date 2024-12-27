#include "zeroclient.h"

ZeroClient::ZeroClient(QTcpSocket* sock, QObject* parent) :
    QObject(parent), mId(-1)
{
    // ����socket
    mSock = new TcpSocket(sock, this);
    connect(mSock, SIGNAL(newData()), this, SLOT(newData()));
    connect(mSock, SIGNAL(disconnected()), this, SLOT(disconnected()));

    // ���ü�ʱ�����жϿͻ��Ƿ���룬���û�ͶϿ�����
    // ������������10���ӣ�������ģ�������ô���ö�����
    mLoginTimeout = new QTimer(this);
    connect(mLoginTimeout, SIGNAL(timeout()), this, SLOT(clientLoginTimeout()));
    mLoginTimeout->start(10 * 1000);
}

void ZeroClient::closeAndDelete()
{
    // �����Ϣ
    qDebug() << mSock->socket()->peerAddress().toString() << ":"
        << mSock->socket()->peerPort() << " �Ѿ��Ͽ������";

    mSock->close();
    deleteLater();
}

void ZeroClient::processCommand(QByteArray& cmd, QByteArray& args)
{
    cmd = cmd.toUpper().trimmed();
    QHash<QByteArray, QByteArray> hashArgs = parseArgs(args);

    // ����ָ��
    if (cmd == CmdLogin && mId == -1) {
        doLogin(hashArgs);
        return;
    }
}

QHash<QByteArray, QByteArray> ZeroClient::parseArgs(QByteArray& args)
{
    QList<QByteArray> listArgs = args.split(CmdSplit[0]);

    // �ֽ������Ȼ����������ϣ��
    QHash<QByteArray, QByteArray> hashArgs;
    for (int i = 0; i < listArgs.length() - 1; i += 2) {
        hashArgs.insert(listArgs[i].toUpper().trimmed(),
            listArgs[i + 1].trimmed());
    }

    return hashArgs;
}

void ZeroClient::doLogin(QHash<QByteArray, QByteArray>& args)
{
    // �����¼�ź�
    QString userName = args["USER_NAME"];
    QString system = args["SYSTEM"];
    QString ip = mSock->socket()->peerAddress().toString();
    int port = mSock->socket()->peerPort();
    emit login(this, userName, ip, port, system);

    // �����Ϣ
    qDebug() << ip << ":" << port << " �Ѿ���������";
}

void ZeroClient::clientLoginTimeout()
{
    if (mId == -1) {
        closeAndDelete();
    }
}

void ZeroClient::disconnected()
{
    if (mId >= 0) {
        emit logout(mId);
    }

    closeAndDelete();
}

void ZeroClient::newData()
{
    // ��socket���ȡ������
    QByteArray* buf = mSock->buffer();

    int endIndex;
    while ((endIndex = buf->indexOf(CmdEnd)) > -1) {  //����ڷ�����������û�ҵ�/r/n�Ļ���ֱ�ӷ���
        // ��ȡһ��ָ��
        QByteArray data = buf->mid(0, endIndex);
        buf->remove(0, endIndex + CmdEnd.length());

        // ��ȡָ��Ͳ���
        QByteArray cmd, args;
        int argIndex = data.indexOf(CmdSplit);
        if (argIndex == -1) {
            cmd = data;
        }
        else {
            cmd = data.mid(0, argIndex);
            args = data.mid(argIndex + CmdSplit.length(), data.length());
        }

        // ����ָ��
        processCommand(cmd, args);
    }
}