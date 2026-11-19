#ifndef __FONT_HZK16_H__
#define __FONT_HZK16_H__

#include "lcd_common.h"

#ifdef ENABLE_LCD_HZK16
extern void UniToStr(const uint16_t *uni, uint8_t *str);

extern int findStrInHZK16(unsigned short *value, char *buf);

int GetPixel_FromHZK16Bitmap(const char *bitmap, int x, int y, int width);

int Calc_UnicodeWidth(const uint16_t *unicode, uint8_t len);
#endif /* ENABLE_LCD_HZK16 */

#endif /* __FONT_HZK16_H__ */
