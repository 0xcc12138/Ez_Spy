
#ifndef FILESPY_H
#define FILESPY_H

#include "TcpSocket.h"
#include <windows.h>
#include <iostream>
#include <map>
#include <vector>

class FileSpy
{
public:
    FileSpy();

    // �������ͻ��˷��͵�ָ��(���������Ҫ��Ҳ���������Լ���ָ��)
    const std::string CmdGetDirFiles = "GET_DIRS_FILES";   // ��ȡ·���µ������ļ�����·����
    const std::string CmdDownloadFile = "DOWNLOAD_FILE";   // ����˴ӿͻ�Ҳ�����ļ�
    const std::string CmdUploadFile = "UPLOAD_FILE";       // ������ϴ��ļ����ͻ���
    const std::string CmdDeleteFile = "DELETE_FILE";       // ������ڿͻ���ɾ���ļ�

    // �ͻ��������˷��͵�ָ��(���������Ҫ��Ҳ���������Լ���ָ��)
    const std::string CmdSendDrives = "SEND_DRIVES";        // �����̷�
    const std::string CmdSendDirs = "SEND_DIRS";            // ����·���µ�����·����
    const std::string CmdSendFiles = "SEND_FILES";          // ����·���µ������ļ���
    const std::string CmdDeleteFileSuccess = "DELETE_SUCCESS";  // �ɹ�ɾ���ļ�
    const std::string CmdDeleteFileFailed = "DELETE_FAILED";    // ɾ���ļ�ʧ��

    // �ָ���źͽ������ţ������ȡ�ļ��������ļ�����:FILES<�ָ��>FILEA<�ļ��ָ��>FILEB<�ļ��ָ��>FILEC<��������>
    const std::string CmdSplit = ";";
    const std::string CmdEnd = "\r\n";
    const std::string CmdFileSplit = "|";

    // ��������ں���
    static void startByNewThread(std::string domain, int port);
    static DWORD WINAPI fileSpyThreadProc(LPVOID args);
    static void startFileSpy(std::string domain, int port);

    // �����������
    static void addDataToBuffer(TcpSocket* sock, std::string& buf, char* data, int size);
    static std::map<std::string, std::string> parseArgs(std::string& data);
    static void processCmd(TcpSocket* sock, std::string& cmd, std::string& data);

    // �������
    static void doGetDirFiles(TcpSocket* sock, std::map<std::string, std::string>& args);
    static void doDownloadFile(TcpSocket* sock, std::map<std::string, std::string>& args);
    static void doUploadFile(TcpSocket* sock, std::map<std::string, std::string>& args);
    static void doDeleteFile(TcpSocket* sock, std::map<std::string, std::string>& args);

    // ��ȡ�����̷�
    static std::vector<std::string> getDrives();
    // ��ȡ·���µ�����·��
    static std::vector<std::string> getDirs(std::string dir);
    // ��ȡ·���µ������ļ�
    static std::vector<std::string> getFiles(std::string dir);

    // �����ļ����ݰ�ͷ
    typedef struct {
        char fileName[256];
        unsigned int len;
    } FileHeader;

    // �����ļ���ں���
    static void startSendFileByNewThread(std::string filePath, std::string domain, int port);
    static DWORD WINAPI sendFileThreadProc(LPVOID args);
    static void startSendFile(std::string filePath, std::string domain, int port);

    // �����ļ���ں���
    static void startRecvFileByNewThread(std::string filePath, std::string domain, int port);
    static DWORD WINAPI recvFileThreadProc(LPVOID args);
    static void startRecvFile(std::string filePath, std::string domain, int port);
};

#endif // FILESPY_H