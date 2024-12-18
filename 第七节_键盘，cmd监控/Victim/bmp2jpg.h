#pragma once
#include <iostream>
#include <stdio.h>
// 加入jpeg压缩库头文件
extern "C" {
#include "jpeg-9f/jpeglib.h"
#include "jpeg-9f/jmorecfg.h"
#include "jpeg-9f/jconfig.h"
}

//#pragma comment(lib,"jpeg.lib")
using namespace std;







#pragma pack(1)        //两字节对齐，否则bmp_fileheader会占16Byte
struct bmp_fileheader
{
    unsigned short    bfType;        //若不对齐，这个会占4Byte
    unsigned long    bfSize;
    unsigned short    bfReverved1;
    unsigned short    bfReverved2;
    unsigned long    bfOffBits;
};

struct bmp_infoheader
{
    unsigned long    biSize;
    unsigned long    biWidth;
    unsigned long    biHeight;
    unsigned short    biPlanes;
    unsigned short    biBitCount;
    unsigned long    biCompression;
    unsigned long    biSizeImage;
    unsigned long    biXPelsPerMeter;
    unsigned long    biYpelsPerMeter;
    unsigned long    biClrUsed;
    unsigned long    biClrImportant;
};



// 声明全局变量（加上 extern）
extern FILE* input_file;
extern FILE* output_file;

extern struct bmp_fileheader bfh;
extern struct bmp_infoheader bih;

extern unsigned char* src_buffer;
extern unsigned char* dst_buffer;

// 声明函数（加上 extern）
extern unsigned long bmp_to_jpg(unsigned char* bmp, unsigned char* jpg);
extern unsigned long synthese_jpeg(unsigned char* jpg, const bmp_infoheader& bih);
extern void read_bmp_data(unsigned char* bmp, const bmp_fileheader& bfh, const bmp_infoheader& bih);
extern void read_bmp_header(unsigned char* bmp, bmp_fileheader& bfh, bmp_infoheader& bih);