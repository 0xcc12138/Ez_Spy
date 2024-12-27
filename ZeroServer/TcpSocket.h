#pragma once
#ifndef TCPSOCKET_H
#define TCPSOCKET_H
#pragma execution_character_set("utf-8")
#include <QObject>
#include <QTcpSocket>
#include <QHostAddress>

class TcpSocket : public QObject
{
    Q_OBJECT
public:
    // ��ʼ��socket
    // @sock: ��sock�ӵ������mSock��˽�б�����
    explicit TcpSocket(QTcpSocket* sock, QObject* parent = 0);

    // ��ȡsocket
    QTcpSocket* socket() {
        return mSock;
    }

    // ��ȡ������
    QByteArray* buffer() {
        return &mBuf;
    }

    // �Ͽ��Ϳͻ�֮�������
    void close();

    // ��������
    void write(QByteArray data);

private:
    QTcpSocket* mSock;  // �ͻ�
    QByteArray mBuf;    // ���ݻ��������ӿͻ�����յ������ݶ����ȷ�������

signals:
    // ���������ݼ��뵽mBuf��ͷ�������źţ��õ�����������֪����
    // Ȼ���ڶ��µ�����������Ӧ�Ĵ���
    void newData();

    // ���ͻ��Ͽ��Ƿ�����ź�
    void disconnected();

public slots:
    void readReady();
};

#endif // TCPSOCKET_H