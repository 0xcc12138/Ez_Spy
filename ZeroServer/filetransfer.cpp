#include "filetransfer.h"

FileTransfer::FileTransfer(QObject* parent) : QObject(parent)
{
    // ��ʼ��
    mHeader.len = 0;
}

int FileTransfer::startRecvFileServer(QString userName, QString filePath)
{
    // ���ļ�
    mFile.setFileName(filePath);
    if (!mFile.open(QFile::WriteOnly)) {
        return -1;
    }

    // �����µķ����
    mServer = new TcpServer(this);
    connect(mServer, SIGNAL(newConnection(QTcpSocket*)), this, SLOT(newRecvFileConnection(QTcpSocket*)));

    mServer->start(0);
    if (!mServer->server()->isListening()) {
        qDebug() << "�����ļ����շ����ʧ��";
        deleteLater();
        return -1;
    }

    // ��������������
    mProgress = new QProgressDialog();
    mProgress->setWindowTitle(QString("��%0�ͻ����ص�%1").arg(userName).arg(filePath));
    mProgress->open(this, SLOT(close()));
    mProgress->setMinimumSize(500, mProgress->height());

    return mServer->server()->serverPort();
}

int FileTransfer::startSendFileServer(QString userName, QString filePath)
{
    // ���ļ�
    mFile.setFileName(filePath);
    if (!mFile.open(QFile::ReadOnly)) {
        return -1;
    }

    // �����µķ����
    mServer = new TcpServer(this);
    connect(mServer, SIGNAL(newConnection(QTcpSocket*)), this, SLOT(newSendFileConnection(QTcpSocket*)));

    mServer->start(0);
    if (!mServer->server()->isListening()) {
        qDebug() << "�����ļ����ͷ����ʧ��";
        deleteLater();
        return -1;
    }

    // ��������������
    mProgress = new QProgressDialog();
    mProgress->setWindowTitle(QString("�ϴ��ļ�%0��%1�ͻ�").arg(filePath).arg(userName));
    mProgress->setMinimumSize(500, mProgress->height());
    mProgress->open(this, SLOT(close()));

    return mServer->server()->serverPort();
}

void FileTransfer::close()
{
    // �ͷ�
    mProgress->deleteLater();
    deleteLater();
    mFile.close();
}

void FileTransfer::newRecvFileConnection(QTcpSocket* socket)
{
    // �����ͻ�
    mSock = new TcpSocket(socket, this);
    connect(mSock, SIGNAL(newData()), this, SLOT(recvData()));
    connect(mSock, SIGNAL(disconnected()), this, SLOT(close()));

    // ���ټ����¿ͻ�
    mServer->server()->close();
}

void FileTransfer::newSendFileConnection(QTcpSocket* socket)
{
    // �����ͻ�
    mSock = new TcpSocket(socket, this);
    connect(mSock, SIGNAL(disconnected()), this, SLOT(close()));

    // ��ʼ����
    sendData();

    // ���ټ����¿ͻ�
    mServer->server()->close();
}

void FileTransfer::recvData()
{
    QByteArray* buf = mSock->buffer();
    // ��ȡͷ
    if (mHeader.len == 0) {
        if ((unsigned int)buf->size() >= sizeof(FileHeader)) {
            memcpy(&mHeader, buf->data(), sizeof(FileHeader));
            buf->remove(0, sizeof(FileHeader));

            // ��ǰд������
            _curWritten = 0;

            // ���ý�����
            mProgress->setRange(0, mHeader.len);
        }
    }

    if (mHeader.len > 0 && buf->size() > 0) {
        // ������д������
        _curWritten += buf->size();

        // д���ݵ��ļ���
        mFile.write(buf->data(), buf->size());
        mFile.flush();
        buf->clear();

        // ���½�����
        mProgress->setValue(_curWritten);
    }

    // ����Ѿ���ɾ��˳�
    if (_curWritten >= mHeader.len) {
        close();
    }
}

void FileTransfer::sendData()
{
    // �����ļ�����
    unsigned int len = mFile.size();
    unsigned int pos = 0;
    unsigned int packetSize = 800;

    // ���ý�������Χ
    mProgress->setRange(0, len);

    while (pos < len) {
        int sendSize = pos + packetSize < len ? packetSize : len - pos;
        QByteArray data = mFile.read(sendSize);

        // ����
        mSock->write(data);

        pos += sendSize;

        // ���ý�������ǰλ��
        mProgress->setValue(pos);

    }

    // ���
    close();
}