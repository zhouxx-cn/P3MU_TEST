#ifndef __VECTOR_FONT_STB_H__
#define __VECTOR_FONT_STB_H__

#include "lcd_common.h"

#ifdef ENABLE_LCD_VECTOR_FONT

/* 矢量字体默认参考像素高度 */
#define DEFAULT_REFERENCE_PIXEL_H (100)

/**
 * @brief 初始化 stb_truetype 字体引擎
 * @return 0 成功，-1 失败
 */
int STB_Font_Init(void);

/**
 * @brief 反初始化 stb_truetype 字体引擎，释放内部资源
 */
void STB_Font_Deinit(void);

/**
 * @brief 计算 Unicode 字符串的尺寸和基线偏移
 * @param TextUnicode  以 0 结尾的 Unicode 字符串数组
 * @param pixel_h      渲染目标高度（像素）
 * @param out_width    输出：字符串总宽度（像素，不含额外间距）
 * @param out_height   输出：字符串实际像素高度（最高点到最低点）
 * @param out_baseline_y 输出：从字符串顶部到基线的距离（正值）
 */
int STB_ComputeStringSize(const uint16_t *TextUnicode, int pixel_h,
                           int* out_width, int* out_height, int* out_baseline_y);

/**
 * @brief 绘制单个 Unicode 字符（基于基线对齐）
 * @param dst           目标 RGB565 缓冲区
 * @param dst_w         缓冲区宽度（像素）
 * @param dst_h         缓冲区高度（像素）
 * @param x             字符原点的 X 坐标（字符最左侧）
 * @param y             字符原点的 Y 坐标（**注意：此参数未使用，保留以兼容**）
 * @param unicode       要绘制的 Unicode 字符
 * @param pixel_h       渲染目标高度（像素）
 * @param fg_color      前景色（RGB565）
 * @param bg_color      背景色（COLOR_TRANSPARENT 表示透明）
 * @param baseline_y    基线在屏幕上的 Y 坐标
 * @return 字符的实际前进量（advance + letter_spacing），失败返回 -1
 */
int STB_DrawChar(uint16_t* dst, int dst_w, int dst_h,
                 int x, int y, uint32_t unicode, int pixel_h,
                 uint16_t fg_color, uint16_t bg_color, int baseline_y);

#endif /* ENABLE_LCD_VECTOR_FONT */

#endif /* __VECTOR_FONT_STB_H__ */

