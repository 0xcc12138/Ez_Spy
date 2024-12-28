#pragma once
#ifndef TCPSOCKET_H
#define TCPSOCKET_H
#pragma execution_character_set("utf-8")
#include <winsock2.h>
#include <iostream>
#include <string>
#include <cstdio>

class TcpSocket
{
public:
    TcpSocket();

    // ����תIP
    static std::string fromDomainToIP(std::string domain);

    // ���ӣ��Ͽ������ͣ�����
    bool connectTo(std::string domain, int port);
    void dissconnect();
    bool sendData(const char* data, int size);
    int recvData(char* data, int size);

    // �ж��Ƿ������ӵ�״̬
    bool isConnected() {
        return (int)mSock != SOCKET_ERROR;
    }

private:
    SOCKET mSock;           // socket
    struct sockaddr_in mAddr;   // ��������ַ
    
public:
    int mPort;              // �˿�
    std::string mIp;         // ip

};

#endif // TCPSOCKET_H
