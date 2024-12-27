#pragma once

#ifndef ZEROSERVER_H
#define ZEROSERVER_H
#pragma execution_character_set("utf-8")
#include <QObject>
#include "TcpServer.h"
#include "ZeroClient.h"
#include <QHash>

class ZeroServer : public QObject
{
    Q_OBJECT
public:
    explicit ZeroServer(QObject* parent = 0);

    // ������ֹͣ�����
    void start(int port);
    void stop();

    // ��id����ȡZeroClient
    ZeroClient* client(int id) {
        return mClients[id];
    }

private:
    TcpServer* mServer;         // Tcp�����
    QHash<int, ZeroClient*> mClients;  // ��ID��������Ӧ�Ŀͻ�

    // �����µ�id
    int generateId();

signals:
    // �ͻ������ǳ�����Ҫ�Ǹ��ߴ��ڿؼ�
    void clientLogin(int id, QString userName,
        QString ip, int port, QString system);
    void clientLogout(int id);

public slots:
    // �¿ͻ�����
    void newConnection(QTcpSocket* sock);

    // �ͻ�����
    void login(ZeroClient*, QString userName,
        QString ip, int port, QString system);

    // �ͻ��ǳ�
    void logout(int id);
};

#endif // ZEROSERVER_H