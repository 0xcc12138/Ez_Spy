#include "zeroserver.h"

ZeroServer::ZeroServer(QObject* parent) : QObject(parent)
{
    // ��ʼ��������
    mServer = new TcpServer(this);
    connect(mServer, SIGNAL(newConnection(QTcpSocket*)), this, SLOT(newConnection(QTcpSocket*)));
}

void ZeroServer::start(int port)
{
    mServer->start(port);
}

void ZeroServer::stop()
{
    mServer->stop();
}

int ZeroServer::generateId()
{
    const int max = 1 << 30;

    // �����ظ�
    QList<int> existsKeys = mClients.keys();
    for (int i = mClients.size() + 1; i < max; ++i) {
        if (existsKeys.indexOf(i) == -1) {
            return i;
        }
    }

    return -1;
}

void ZeroServer::newConnection(QTcpSocket* sock)
{
    // ����ZeroClient����sock��ӽ�ȥ
    ZeroClient* client = new ZeroClient(sock);
    connect(client, SIGNAL(login(ZeroClient*, QString, QString, int, QString)),this, SLOT(login(ZeroClient*, QString, QString, int, QString)));
    connect(client, SIGNAL(logout(int)), this, SLOT(logout(int)));
}

void ZeroServer::login(ZeroClient* client, QString userName, QString ip, int port, QString system)
{
    // ���ӿͻ�����ϣ��
    int id = generateId();
    mClients.insert(id, client);
    client->setId(id);

    // ��������źŸ����ڿؼ�
    emit clientLogin(id, userName, ip, port, system);
}


void ZeroServer::logout(int id)
{
    // �ӹ�ϣ����ɾ���ͻ�
    mClients.remove(id);

    // ����ǳ��źŸ����ڿؼ�
    emit clientLogout(id);
}
