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
        std::cout << "�Ѿ�����ͬ������" << std::endl;
        return -1;
    }


    // ��ʼ��Windows socket���ܣ�Ҫ��Windowsʹ����������ʼ�����
    WSAData wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData)) {
        std::cout << "��ʼ��WSAʧ��\n" << std::endl;
        return -1;
    }





    // ��ѭ��������ὲ
    while (1) {
        // ��ѭ��
        ZeroClient client;
        while (1) {
            // ����Ͽ��ˣ���һ���Զ�����
            client.connectTo("10.194.4.200", 18000);
            Sleep(100);
        }

        Sleep(1000);
    }


    // ���������ͷ�WSA
    WSACleanup();


    return 0;
}