#include "tcpsocket.h"

TcpSocket::TcpSocket(QTcpSocket* sock, QObject* parent) :
    QObject(parent), mSock(sock)
{
    mSock->setParent(this);
    connect(mSock, SIGNAL(readyRead()), this, SLOT(readReady()));
    connect(mSock, SIGNAL(disconnected()), this, SIGNAL(disconnected()));

    // �����Ϣ
    qDebug() << mSock->peerAddress().toString() << ":" << mSock->peerPort() << " �������Ϸ����";
}

void TcpSocket::close()
{
    mSock->close();
}

void TcpSocket::write(QByteArray data)
{
    mSock->write(data);

    if (!mSock->waitForBytesWritten(3000)) {
        // �������ݳ�ʱ
        close();
        emit disconnected();

        // �����Ϣ
        qDebug() << mSock->peerAddress().toString() << ":" << mSock->peerPort()
            << " д��ʧ�ܣ�" << mSock->errorString();
    }
}

void TcpSocket::readReady()
{
    mBuf.append(mSock->readAll());
    emit newData();
}