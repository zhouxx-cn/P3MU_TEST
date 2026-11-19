#ifndef __COMMON_H__
#define __COMMON_H__

#include <stdio.h>
#include "LogManager.h"

#define LCD_ERR(format...)            LOG_E(LOG_MODULE_LCD, format)
#define LCD_WRAN(format...)           LOG_W(LOG_MODULE_LCD, format)
#define LCD_INF(format...)            LOG_I(LOG_MODULE_LCD, format)
#define LCD_DBG(format...)            LOG_D(LOG_MODULE_LCD, format)

typedef unsigned char  uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int   uint32_t;

/* 常用颜色RGB565 */
#define     COLOUR_RED      (0XF800)
#define     COLOUR_GREEN    (0x07E0)
#define     COLOUR_BLUE     (0X001F)
#define     COLOUR_WHITE    (0xFFFF)
#define     COLOUR_YELLOW   (0xFFE0)
#define     COLOUR_CYAN     (0x07FF)
#define     COLOUR_BLACK    (0x0000)

/* 特殊颜色值定义 */
#define COLOR_TRANSPARENT               (0x1234)    /* 透明色标记 */

// 字体引擎类型宏（只能选择一个）
#define FONT_ENGINE_CUSTOM_BITMAP   0   //自定义点阵字库(font_custom.c)
#define FONT_ENGINE_HZK16           1   //HZK16 点阵字库(hzk16)
#define FONT_ENGINE_VECTOR          2   //矢量字体(stb_truetype)

// 选择当前使用的字体引擎
#define CURRENT_FONT_ENGINE FONT_ENGINE_VECTOR

// 根据引擎自动定义启用宏
#if CURRENT_FONT_ENGINE == FONT_ENGINE_CUSTOM_BITMAP
#define ENABLE_LCD_CUSTOM_BITMAP
#elif CURRENT_FONT_ENGINE == FONT_ENGINE_HZK16
#define ENABLE_LCD_HZK16
#elif CURRENT_FONT_ENGINE == FONT_ENGINE_VECTOR
#define ENABLE_LCD_VECTOR_FONT
#endif

/**
 * @brief 将 UTF-8 字符串转换为 Unicode 码点数组（UTF-16/UCS-2）
 * @param utf8_str  输入 UTF-8 字符串（以空字符结尾）
 * @param unicode   输出 Unicode 码点数组（uint16_t）
 * @param max_len   输出数组的最大容量（元素个数）
 * @return 成功返回转换的字符数，失败返回 -1
 */
int utf8_to_unicode_array(const uint8_t *utf8_str, uint16_t *unicode, size_t max_len);

// 混合函数（RGB565，小端模式）
uint16_t blend_rgb565(uint16_t fg, uint16_t bg, uint8_t alpha);

/**
 * @brief 将JPEG文件解码为RGB565格式数据
 * 
 * @param path          [in]  JPEG文件路径
 * @param outbuf        [out] 输出RGB565数据缓冲区指针（函数内部分配，调用者负责释放）
 * @param outbufsize    [out] 输出RGB565数据缓冲区大小
 * @param width         [out] 输出图像宽度（像素）
 * @param height        [out] 输出图像高度（像素）
 * 
 * @return  0: 成功 其他失败，详情见函数内部解释
 * 
 * @note 
 * 1. outbuf由函数内部malloc分配，调用者必须在使用后free释放
 * 2. 输出格式为RGB565小端（R5G6B5），每像素2字节
 * 3. 缓冲区大小 = (*width) * (*height) * 2 字节
 * 
 */
int DecodeJpegToRgb565(const char* path, char **outbuf, uint32_t *outbufsize, uint32_t *width, uint32_t *height);

// @brief 判断数据是否为JPEG格式，如果是则返回宽高
// Return codes:
//  0: Success, JPEG format valid, width/height filled
// -1: Invalid input (null pointer or empty data)
// -2: Not a JPEG file (missing SOI marker)
// -3: Cannot parse JPEG dimensions (no SOF0/SOF2 marker found)
int GetJpegDimensions(const char* imageData, uint32_t dataLen, uint32_t* outWidth, uint32_t* outHeight);

#endif
