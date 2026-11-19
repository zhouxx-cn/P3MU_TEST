#ifndef __FONT_SCALE_H__
#define __FONT_SCALE_H__

#include "lcd_common.h"

/* 缩放算法 */
typedef enum {
    SCALE_NEAREST = 0,      /* 最近邻（最快） */
    SCALE_BILINEAR,         /* 双线性（平滑） */
    SCALE_AREA_AVG,         /* 区域平均（缩小专用） */
} ScaleAlgorithm_t;

/**
 * @brief 2位黑白点阵源位图绘制到RGB565帧缓冲（支持抗锯齿缩放）
 * @param srcBitmap  2位黑白点阵源数据 
 * @param BitmapType 点阵类型，0表示HZK16，1表示自定义点阵
 * @param srcWidth,srcHeight  源图尺寸
 * @param dstBuffer      RGB565帧缓冲
 * @param textColor      前景色
 * @param backColor      背景色，COLOR_TRANSPARENT表示透明
 * @param algorithm      缩放算法
 */
void BitmapScale_Draw(const char *srcBitmap, int BitmapType, int srcWidth, int srcHeight,
                      uint16_t *dstBuffer, int dstBufWidth, int dstBufHeight,
                      int dstX, int dstY, int dstWidth, int dstHeight,
                      uint16_t textColor, uint16_t backColor,
                      ScaleAlgorithm_t algorithm);

/**
 * @brief 8位灰度源位图绘制到RGB565帧缓冲（支持抗锯齿缩放）
 * @param srcGrayBitmap  8位灰度源数据（0=透明/背景, 255=完全不透明/前景，中间值=半透明）
 * @param srcWidth,srcHeight  源图尺寸
 * @param dstBuffer      RGB565帧缓冲
 * @param textColor      前景色（对应灰度255）
 * @param backColor      背景色（对应灰度0），COLOR_TRANSPARENT表示透明
 * @param algorithm      缩放算法
 */
void BitmapScale_DrawGray(const uint8_t *srcGrayBitmap, 
                          int srcWidth, int srcHeight,
                          uint16_t *dstBuffer, int dstBufWidth, int dstBufHeight,
                          int dstX, int dstY, int dstWidth, int dstHeight,
                          uint16_t textColor, uint16_t backColor,
                          ScaleAlgorithm_t algorithm);


#endif
