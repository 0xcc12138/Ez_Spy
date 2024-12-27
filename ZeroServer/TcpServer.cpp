#include "tcpserver.h"

TcpServer::TcpServer(QObject* parent) : QObject(parent)
{
    mServer = new QTcpServer(this);
    connect(mServer, SIGNAL(newConnection()), this, SLOT(newConnection()));
}

void TcpServer::start(int port)
{
    if (!mServer->isListening()) {
        if (mServer->listen(QHostAddress::AnyIPv4, port)) {
            qDebug() << "����˼����ɹ�";
        }
        else {
            qDebug() << "����˼���ʧ�ܣ�" << mServer->errorString();
        }
    }
}

void TcpServer::stop()
{
    if (mServer->isListening()) {
        mServer->close();
    }
}

void TcpServer::newConnection()
{
    while (mServer->hasPendingConnections()) {
        // ��ȡ������
        QTcpSocket* sock = mServer->nextPendingConnection();
        // �����������ź��õ��÷���������֪��
        emit newConnection(sock);
    }
}