#ifndef __FONT_CUSTOM_H__
#define __FONT_CUSTOM_H__

#include "lcd_common.h"

typedef struct
{
    const uint16_t *Charset;    //字库映射表（Unicode）
    const uint8_t  *FontData;   //字库地址
    uint16_t     CharWith;      //字符宽度
    uint16_t     CharHeight;    //字符高度
    uint16_t     CharSize;      //字符占用空间大小
    uint16_t     CharCount;     //字符数量    
    uint16_t     CharPixelSize; //字符实际显示大小
} CustomBitmapInfo_st;

//全局声明
extern  CustomBitmapInfo_st WIFI_IMP_FontLibInfo;
extern const uint16_t WIFI_DISCONNECT_RGB565Data[];

/**
 * @brief 从字符位图数据中获取指定坐标的像素值（紧凑位图，高位在前）
 * @param data      字符数据起始地址
 * @param x         列坐标 (0 ~ width-1)
 * @param y         行坐标 (0 ~ height-1)
 * @param width     字符宽度
 * @return 1（前景）或 0（背景）
 */
int GetPixel_FromCustomBitmap(const uint8_t *data, int x, int y, int width);

#ifdef ENABLE_LCD_CUSTOM_BITMAP
/**
 * @brief 获取字符在字库映射表中的索引
 * @param charset 字库映射表（Unicode数组）
 * @param unicode 要查找的Unicode值
 * @param count 映射表中字符数量
 * @return 索引，-1 表示未找到
 */
int FindCharIndex(const uint16_t *charset, uint16_t unicode, uint16_t count);

/**
 * @brief 根据Unicode值获取对应的字库信息
 * @param unicode Unicode值
 * @return 对应的字库信息指针，NULL表示未找到
 */
CustomBitmapInfo_st* GetCustomFontByUnicode(uint16_t unicode);

/**
 * @brief 计算Unicode字符串宽度，自动选择不同字库
 * @param unicode Unicode数组
 * @return 字符串宽度
 */
int Calc_StringWidth_Auto_Unicode(const uint16_t *unicode);

/**
 * @brief 计算Unicode字符串高度，自动选择不同字库
 * @param unicode Unicode数组
 * @return 字符串高度
 */
int Calc_StringHeight_Auto_Unicode(const uint16_t *unicode);
#endif

#endif
