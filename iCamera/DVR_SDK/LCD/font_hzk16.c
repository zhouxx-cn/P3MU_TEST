#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "font_hzk16.h"

#ifdef ENABLE_LCD_HZK16
 /*** @brief 从合并位图读取像素 ***/
int GetPixel_FromHZK16Bitmap(const char *bitmap, int x, int y, int width)
{
    if (x < 0 || y < 0) return 0;
    int byteIdx = y * ((width + 7) / 8) + (x / 8);
    int bitIdx = 7 - (x % 8);
    return (bitmap[byteIdx] >> bitIdx) & 0x01;
}

int Calc_UnicodeWidth(const uint16_t *unicode, uint8_t len)
{
    if (unicode == NULL || len == 0) return 0;

    int width = 0;
    for (int i = 0; i < len; i++) {
        if (unicode[i] < 0x80) {
            width += 8;   // ASCII: 8像素宽
        } else {
            width += 16;  // 汉字: 16像素宽
        }
    }

    return width;
}
#endif
