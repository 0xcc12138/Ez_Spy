#pragma once
#ifndef TCPSERVER_H
#define TCPSERVER_H
#pragma execution_character_set("utf-8")
#include <QObject>
#include <QTcpServer>

class TcpServer : public QObject
{
    Q_OBJECT
public:
    explicit TcpServer(QObject* parent = 0);

    // ���������
    // @port: �����Ķ˿�
    void start(int port);
    void stop();

    // ���ط�����
    QTcpServer* server() {
        return mServer;
    }

private:
    QTcpServer* mServer;  // �ڹ��캯�����ʼ��

signals:
    // ���µ����ӽ���ʱ���͵��ź�
    // @sock: �µ�����
    void newConnection(QTcpSocket* sock);

public slots:
    // ���д�mServer�н��յ������Ӻ󣬻�ȡ�����ӵ�socket��Ȼ����
    // ����newConnection�ź�
    void newConnection();
};

#endif // TCPSERVER_H