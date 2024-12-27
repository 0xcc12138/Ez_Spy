#include "bmp2jpg.h"
unsigned char* src_buffer;
unsigned char* dst_buffer;

void read_bmp_header(unsigned char* bmp, bmp_fileheader& bfh, bmp_infoheader& bih)
{
    memcpy(&bfh, bmp, sizeof(bmp_fileheader));
    memcpy(&bih, bmp + sizeof(bmp_fileheader), sizeof(bmp_infoheader));
}

void read_bmp_data(unsigned char* bmp, const bmp_fileheader& bfh, const bmp_infoheader& bih)
{
    unsigned long width = bih.biWidth;
    unsigned long height = bih.biHeight;
    unsigned short depth = static_cast<unsigned short>(bih.biBitCount / 8);

    src_buffer = new unsigned char[width * height * depth];
    dst_buffer = new unsigned char[width * height * depth];

    memcpy(src_buffer, bmp + bfh.bfOffBits, width * height * depth);

    unsigned char* src_point = src_buffer + width * depth * (height - 1);
    unsigned char* dst_point = dst_buffer + width * depth * (height - 1);

    for (unsigned long i = 0; i < height; i++)
    {
        for (unsigned long j = 0; j < width * depth; j += depth)
        {
            if (depth == 1) // ����Ҷ�ͼ
            {
                dst_point[j] = src_point[j];
            }

            if (depth == 3) // �����ɫͼ
            {
                dst_point[j + 2] = src_point[j + 0];
                dst_point[j + 1] = src_point[j + 1];
                dst_point[j + 0] = src_point[j + 2];
            }
        }
        dst_point -= width * depth;
        src_point -= width * depth;
    }
}

unsigned long synthese_jpeg(unsigned char* jpg, const bmp_infoheader& bih)
{
    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;
    JSAMPARRAY buffer;

    unsigned long width = bih.biWidth;
    unsigned long height = bih.biHeight;
    unsigned short depth = static_cast<unsigned short>(bih.biBitCount / 8);
    unsigned char* point;

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);

    unsigned char* mem_buffer = nullptr;
    size_t mem_size = 0;
    jpeg_mem_dest(&cinfo, &mem_buffer, &mem_size);

    cinfo.image_width = width;
    cinfo.image_height = height;
    cinfo.input_components = depth;
    cinfo.in_color_space = (depth == 1) ? JCS_GRAYSCALE : JCS_RGB;

    jpeg_set_defaults(&cinfo);
    jpeg_set_quality(&cinfo, 20, TRUE);
    jpeg_start_compress(&cinfo, TRUE);

    // ����ÿ���ֽ�������ȷ����4�ı���
    unsigned long row_stride = width * depth;
    unsigned long aligned_row_stride = (row_stride + 3) & ~3;  // ���϶��뵽4�ı���

    buffer = (*cinfo.mem->alloc_sarray)
        ((j_common_ptr)&cinfo, JPOOL_IMAGE, aligned_row_stride, 1);

    // ���ͼ�����ݣ�ȷ��ÿ�а�4�ֽڶ���
    point = dst_buffer + row_stride * (height - 1);  // �����һ�п�ʼ
    while (cinfo.next_scanline < height)
    {
        // ����һ��ͼ�����ݵ����������������4�ֽڶ���
        memcpy(*buffer, point, row_stride);
        memset(*buffer + row_stride, 0, aligned_row_stride - row_stride);  // ���ʣ����ֽ�

        jpeg_write_scanlines(&cinfo, buffer, 1);
        point -= row_stride;  // ����һ��
    }

    jpeg_finish_compress(&cinfo);
    memcpy(jpg, mem_buffer, mem_size);
    jpeg_destroy_compress(&cinfo);
    free(mem_buffer);

    return mem_size;
}








unsigned long bmp_to_jpg(unsigned char* bmp, unsigned char* jpg)
{
    bmp_fileheader bfh;
    bmp_infoheader bih;

    read_bmp_header(bmp, bfh, bih);
    read_bmp_data(bmp, bfh, bih);

    unsigned long jpg_len = synthese_jpeg(jpg, bih);

    delete[] src_buffer;
    delete[] dst_buffer;

    return jpg_len;
}
