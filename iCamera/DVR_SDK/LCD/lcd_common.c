#include "lcd_common.h"
#include "tjpgd.h"

/**
 * @brief 将 UTF-8 字符串转换为 Unicode 码点数组（UTF-16/UCS-2）
 * @param utf8_str  输入 UTF-8 字符串（以空字符结尾）
 * @param unicode   输出 Unicode 码点数组（uint16_t）
 * @param max_len   输出数组的最大容量（元素个数）
 * @return 成功返回转换的字符数，失败返回 -1
 */
int utf8_to_unicode_array(const uint8_t *utf8_str, uint16_t *unicode, size_t max_len)
{
    if (!utf8_str || !unicode || max_len == 0) {
        return -1;
    }

    const uint8_t *p = utf8_str;
    size_t count = 0;

    while (*p && count < max_len) {
        uint32_t code = 0;
        int bytes = 0;

        // 根据首字节判断 UTF-8 编码长度
        if ((*p & 0x80) == 0) {
            // ASCII 字符
            code = *p;
            bytes = 1;
        } else if ((*p & 0xE0) == 0xC0) {
            // 2 字节 UTF-8
            if ((p[1] & 0xC0) != 0x80) goto invalid;
            code = ((*p & 0x1F) << 6) | (p[1] & 0x3F);
            bytes = 2;
        } else if ((*p & 0xF0) == 0xE0) {
            // 3 字节 UTF-8（中文等）
            if ((p[1] & 0xC0) != 0x80 || (p[2] & 0xC0) != 0x80) goto invalid;
            code = ((*p & 0x0F) << 12) | ((p[1] & 0x3F) << 6) | (p[2] & 0x3F);
            bytes = 3;
        } else if ((*p & 0xF8) == 0xF0) {
            // 4 字节 UTF-8（超出 BMP，如 emoji）
            // 这里简单用替换字符 U+FFFD，因为 uint16_t 无法存储
            code = 0xFFFD;  // Unicode 替换字符
            bytes = 4;
            // 跳过后续字节（需要验证有效性）
            if ((p[1] & 0xC0) != 0x80 || (p[2] & 0xC0) != 0x80 || (p[3] & 0xC0) != 0x80) {
                goto invalid;
            }
        } else {
            goto invalid;
        }

        // 对于 BMP 内的字符直接存储，否则存储替换字符
        if (code > 0xFFFF) {
            code = 0xFFFD;
        }

        unicode[count] = (uint16_t)code;
        count++;
        p += bytes;
        continue;

    invalid:
        // 无效 UTF-8 序列，使用替换字符 U+FFFD
        unicode[count] = 0xFFFD;
        count++;
        p++;  // 跳过当前字节，尝试恢复同步
    }

    return (int)count;
}

// 混合函数（RGB565，小端模式）
uint16_t blend_rgb565(uint16_t fg, uint16_t bg, uint8_t alpha)
{
    uint8_t fg_r = (fg >> 11) & 0x1F;
    uint8_t fg_g = (fg >> 5)  & 0x3F;
    uint8_t fg_b = fg & 0x1F;
    uint8_t bg_r = (bg >> 11) & 0x1F;
    uint8_t bg_g = (bg >> 5)  & 0x3F;
    uint8_t bg_b = bg & 0x1F;

    uint32_t r = ( (uint32_t)fg_r * alpha + (uint32_t)bg_r * (255 - alpha) + 127 ) / 255;
    uint32_t g = ( (uint32_t)fg_g * alpha + (uint32_t)bg_g * (255 - alpha) + 127 ) / 255;
    uint32_t b = ( (uint32_t)fg_b * alpha + (uint32_t)bg_b * (255 - alpha) + 127 ) / 255;

    if (r > 31) r = 31;
    if (g > 63) g = 63;
    if (b > 31) b = 31;

    return (r << 11) | (g << 5) | b;
}

/* 用户定义设备标识 */
typedef struct
{
    FILE *fp;      /* 用于输入函数的文件指针 */
    char *fbuf;    /* 用于输出函数的帧缓冲区的指针 */
    unsigned int wfbuf;    /* 帧缓冲区的图像宽度[像素] */
} IODEV;

static uint16_t in_func(JDEC *jd, uint8_t *buff, uint16_t nbyte)
{
    IODEV *dev = (IODEV *)jd->device;  /* Device identifier for the session (5th argument of jd_prepare function) */    

    if (buff)
    {
        /* 从输入流读取一字节 */
        return (unsigned int)fread(buff, 1, nbyte, dev->fp);
    }
    else
    {
        /* 从输入流移除一字节 */
        return fseek(dev->fp, nbyte, SEEK_CUR) ? 0 : nbyte;
    }
}

static uint16_t out_func(JDEC *jd, void *bitmap, JRECT *rect)
{
#if 1
    /* 小端模式存储 */
    IODEV *dev = (IODEV *)jd->device;
    unsigned char *src = (unsigned char *)bitmap;
    char *dst;
    unsigned int y, bws, bwd;

    /* 计算目标位置（考虑缩放后的坐标） */
    dst = dev->fbuf + 2 * (rect->top * dev->wfbuf + rect->left);
    bws = 2 * (rect->right - rect->left + 1);   /* 源宽度（字节） */
    bwd = 2 * dev->wfbuf;                         /* 目标行宽（字节） */

    /* 逐行拷贝 */
    for (y = rect->top; y <= rect->bottom; y++) {
        memcpy(dst, src, bws);
        src += bws;
        dst += bwd;
    }
#else
    /* 转换为大端模式存储 */
    IODEV *dev = (IODEV *)jd->device;
    uint16_t *src = (uint16_t *)bitmap;
    char *dst;
    uint16_t x, y;
    uint16_t width = rect->right - rect->left + 1;
    uint16_t height = rect->bottom - rect->top + 1;

    /* 计算目标起始位置 */
    dst = dev->fbuf + 2 * (rect->top * dev->wfbuf / (1 << JD_SCALE) + rect->left);

    /* 逐行处理，转换为RGB565大端 */
    for (y = 0; y < height; y++) {
        char *row_dst = dst + y * 2 * dev->wfbuf;

        for (x = 0; x < width; x++) {
            uint16_t pixel = src[y * width + x];  /* 小端RGB565 */

            /* 转换为大端：交换高低字节 */
            row_dst[2 * x]     = (pixel >> 8) & 0xFF;  /* 高字节 */
            row_dst[2 * x + 1] = pixel & 0xFF;         /* 低字节 */
        }
    }
#endif
    return 1;    /* 继续解码 */
}

int DecodeJpegToRgb565(const char* path, char **outbuf, uint32_t *outbufsize, uint32_t *width, uint32_t *height)
{
    void *work;       /* 指向解码工作区域 */
    JDEC jdec;        /* 解码对象 */
    JRESULT res;      /* TJpgDec API的返回值 */
    IODEV devid;      /* 用户定义设备标识 */

    devid.fp = fopen(path, "rb");
    if (devid.fp == NULL) {
        return -1;
    }

    /* 分配一个用于TJpgDec的工作区域 */
    work = malloc(3100);
    if (work == NULL) {
        fclose(devid.fp);
        return -1;
    }

    /* 准备解码 */
    res = jd_prepare(&jdec, in_func, work, 3100, &devid);
    if (res != JDR_OK)
    {
        LCD_ERR("Failed to prepare: rc = %d\n", res);
        goto DEND;
    }

    /* 准备好解码。图像信息有效 */
    LCD_INF("Image dimensions: %u by %u. %u bytes used.\n", jdec.width, jdec.height, 3100 - jdec.sz_pool);

    devid.fbuf = malloc(2*jdec.width*jdec.height); /* 输出图像的帧缓冲区(RGB565配置) */
    if (devid.fbuf == NULL) {
         LCD_ERR("Failed to allocate frame buffer: %u bytes\n", 2*jdec.width*jdec.height);
         res = -2;
         goto DEND;
    }
    devid.wfbuf = jdec.width;

    res = jd_decomp(&jdec, out_func, JD_SCALE);
    if (res != JDR_OK) {
        LCD_ERR("Failed to decompress: rc=%d\n", res);
        goto DEND;
    }

    *width = jdec.width;
    *height = jdec.height;
    *outbuf = devid.fbuf;
    *outbufsize = 2*jdec.width*jdec.height;
    devid.fbuf = NULL;  /* 防止DEND释放 */

DEND:
    if (work) {
        free(work);
    }
    if (devid.fp) {
        fclose(devid.fp);
    }
    if (devid.fbuf != NULL) {
        free(devid.fbuf);
    }
    return res;
}

/**
 * @brief 判断数据是否为JPEG格式，如果是则返回宽高
 */
int GetJpegDimensions(const char* imageData, uint32_t dataLen, uint32_t* outWidth, uint32_t* outHeight)
{
    if (imageData == NULL || dataLen == 0 || outWidth == NULL || outHeight == NULL) {
        return -1;  // Invalid input
    }

    // 1. Check JPEG magic number (SOI marker: 0xFF 0xD8)
    if (dataLen < 2 || (unsigned char)imageData[0] != 0xFF || (unsigned char)imageData[1] != 0xD8) {
        return -2;  // Not a JPEG file
    }

    // 2. Parse JPEG markers to find SOF0 (0xC0) or SOF2 (0xC2)
    for (uint32_t i = 2; i + 9 < dataLen; ) {
        if ((unsigned char)imageData[i] == 0xFF) {
            unsigned char marker = (unsigned char)imageData[i + 1];

            // Skip padding bytes (0xFF followed by 0xFF)
            if (marker == 0xFF) {
                i++;
                continue;
            }

            // SOF0 (Baseline DCT) or SOF2 (Progressive DCT)
            // Format: 0xFF marker [2-byte length] [1-byte precision] [2-byte height] [2-byte width] ...
            if (marker == 0xC0 || marker == 0xC2) {
                if (i + 9 < dataLen) {
                    *outHeight = ((unsigned char)imageData[i + 5] << 8) | (unsigned char)imageData[i + 6];
                    *outWidth  = ((unsigned char)imageData[i + 7] << 8) | (unsigned char)imageData[i + 8];
                    return 0;  // Success
                }
                break;
            }

            // Skip other marker segments
            // Markers without length: 0x00, 0x01, and 0xD0~0xD9
            if (marker != 0x00 && marker != 0x01 && (marker < 0xD0 || marker > 0xD9)) {
                if (i + 3 < dataLen) {
                    uint16_t segmentLen = ((unsigned char)imageData[i + 2] << 8) | (unsigned char)imageData[i + 3];
                    i += 2 + segmentLen;
                    continue;
                }
            }
        }
        i++;
    }

    return -3;  // Cannot parse JPEG dimensions
}
