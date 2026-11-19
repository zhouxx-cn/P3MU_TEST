#include <math.h>

#include "font_scale.h"
#ifdef ENABLE_LCD_HZK16
#include "font_hzk16.h"
#endif
#include "font_custom.h"

static int BitmapScale_GetPixel(const char *bitmap, int type, int x, int y, int width)
{
    if (type == 0)
    {    
#ifdef ENABLE_LCD_HZK16
        return GetPixel_FromHZK16Bitmap(bitmap, x, y, width);
#endif
    }
    else
    {
        return GetPixel_FromCustomBitmap((const uint8_t *)bitmap, x, y, width);
    }

    return 0;
}

/**
 * @brief 最近邻插值（最快）
 */
static int nearest_interp(const char *srcBitmap, int BitmapType,
                          int srcWidth, int srcHeight,
                          int dstX, int dstY, int dstW, int dstH)
{
    int srcX = (dstX * srcWidth) / dstW;
    int srcY = (dstY * srcHeight) / dstH;
    return BitmapScale_GetPixel(srcBitmap, BitmapType, srcX, srcY, srcWidth);
}

#if 0
/**
* @brief 双线性插值（简化版，适合放大）
*/
 static int bilinear_interp(const char *srcData, int type, int srcW, int srcH,
                                float dstX, float dstY, int dstW, int dstH)
 {
     float srcX = dstX * srcW / (float)dstW;
     float srcY = dstY * srcH / (float)dstH;
     
     int x0 = (int)srcX;
     int y0 = (int)srcY;
     int x1 = x0 + 1;
     int y1 = y0 + 1;
     
     if (x1 >= srcW) x1 = srcW - 1;
     if (y1 >= srcH) y1 = srcH - 1;
     
     float fx = srcX - x0;
     float fy = srcY - y0;
     
     // 获取四个邻域像素
     int p00 = BitmapScale_GetPixel(srcData, type, x0, y0, srcW);
     int p01 = BitmapScale_GetPixel(srcData, type, x0, y1, srcW);
     int p10 = BitmapScale_GetPixel(srcData, type, x1, y0, srcW);
     int p11 = BitmapScale_GetPixel(srcData, type, x1, y1, srcW);
     
     // 双线性插值
     float value = (1-fx)*(1-fy)*p00 + (1-fx)*fy*p01 + 
                   fx*(1-fy)*p10 + fx*fy*p11;
 
     return (value >= 0.5f) ? 1 : 0;
}

/**
 * @brief 区域平均降采样（用于缩小）
 */
static int area_average(const char *srcBitmap, int type,
                        int srcWidth, int srcHeight,
                        int dstX, int dstY, int dstW, int dstH)
{
    /* 计算源区域 */
    int srcX0 = (dstX * srcWidth) / dstW;
    int srcY0 = (dstY * srcHeight) / dstH;
    int srcX1 = ((dstX + 1) * srcWidth + dstW - 1) / dstW;
    int srcY1 = ((dstY + 1) * srcHeight + dstH - 1) / dstH;
    
    if (srcX1 > srcWidth) srcX1 = srcWidth;
    if (srcY1 > srcHeight) srcY1 = srcHeight;
    
    /* 统计前景像素 */
    int total = 0, foreground = 0;
    for (int y = srcY0; y < srcY1; y++) {
        for (int x = srcX0; x < srcX1; x++) {
            total++;
            if (BitmapScale_GetPixel(srcBitmap, type, x, y, srcWidth)) {
                foreground++;
            }
        }
    }
    
    /* 40%阈值，保留细线 */
    return (foreground * 10 > total * 4) ? 1 : 0;
}

void BitmapScale_Draw(const char *srcBitmap, int BitmapType, int srcWidth, int srcHeight,
                      uint16_t *dstBuffer, int dstBufWidth, int dstBufHeight,
                      int dstX, int dstY, int dstWidth, int dstHeight,
                      uint16_t textColor, uint16_t backColor,
                      ScaleAlgorithm_t algorithm)
{
    if (srcBitmap == NULL || dstBuffer == NULL) return;

    /* 边界裁剪 */
    int startX = (dstX < 0) ? 0 : dstX;
    int startY = (dstY < 0) ? 0 : dstY;
    int endX = (dstX + dstWidth > dstBufWidth) ? dstBufWidth : dstX + dstWidth;
    int endY = (dstY + dstHeight > dstBufHeight) ? dstBufHeight : dstY + dstHeight;
    int relX, relY, pixel;
    char *p;

    if ((algorithm == SCALE_AREA_AVG) && !(dstWidth < srcWidth || dstHeight < srcHeight))
    {    
        /* 放大时回退到双线性 */
        algorithm = SCALE_BILINEAR;
        LCD_ERR("algorithm changed!!");
    }

    /* 逐像素缩放绘制 */
    for (int y = startY; y < endY; y++)
    {
        for (int x = startX; x < endX; x++)
        {
            /* 计算在目标区域中的相对坐标 */
            relX = x - dstX;
            relY = y - dstY;

            /* 选择算法 */
            switch (algorithm) {
                case SCALE_AREA_AVG:
                    pixel = area_average(srcBitmap, BitmapType, srcWidth, srcHeight,
                                        relX, relY, dstWidth, dstHeight);
                    break;

                case SCALE_BILINEAR:
                    pixel = bilinear_interp(srcBitmap, BitmapType, srcWidth, srcHeight,
                                           relX, relY, dstWidth, dstHeight);
                    break;                

                case SCALE_NEAREST:
                default:
                    pixel = nearest_interp(srcBitmap, BitmapType, srcWidth, srcHeight,
                                          relX, relY, dstWidth, dstHeight);
                    break;
            }

            /* 绘制到帧缓冲区 */
            p = (char *)&(dstBuffer[y * dstBufWidth + x]);
            if (pixel) {
                *p = textColor >> 8;
                *(p+1) = textColor;
            } else if (backColor != COLOR_TRANSPARENT) {
                *p = backColor >> 8;
                *(p+1) = backColor;
            }
        }
    }
}
#else
/**
* @brief 双线性插值（简化版，适合放大）
*/
static uint8_t bilinear_interp_gray(const char *srcData, int type, int srcW, int srcH,
                                            float dstX, float dstY, int dstW, int dstH)
{
    float srcX = dstX * srcW / (float)dstW;
    float srcY = dstY * srcH / (float)dstH;

    int x0 = (int)srcX;
    int y0 = (int)srcY;
    int x1 = x0 + 1;
    int y1 = y0 + 1;

    if (x1 >= srcW) x1 = srcW - 1;
    if (y1 >= srcH) y1 = srcH - 1;

    float fx = srcX - x0;
    float fy = srcY - y0;

    int p00 = BitmapScale_GetPixel(srcData, type, x0, y0, srcW);
    int p01 = BitmapScale_GetPixel(srcData, type, x0, y1, srcW);
    int p10 = BitmapScale_GetPixel(srcData, type, x1, y0, srcW);
    int p11 = BitmapScale_GetPixel(srcData, type, x1, y1, srcW);

    // 双线性插值，得到0~1之间的浮点值
    float value = (1-fx)*(1-fy)*p00 + (1-fx)*fy*p01 +
                  fx*(1-fy)*p10 + fx*fy*p11;

    // 转换为0~255灰度
    return (uint8_t)(value * 255.0f + 0.5f);
}

/**
 * @brief 区域平均降采样（用于缩小）
 */
static uint8_t area_average_gray(const char *srcBitmap, int type, int srcWidth, int srcHeight,
                                        int dstX, int dstY, int dstW, int dstH)
{
    // 计算源区域
    int srcX0 = (dstX * srcWidth) / dstW;
    int srcY0 = (dstY * srcHeight) / dstH;
    int srcX1 = ((dstX + 1) * srcWidth + dstW - 1) / dstW;
    int srcY1 = ((dstY + 1) * srcHeight + dstH - 1) / dstH;

    if (srcX1 > srcWidth) srcX1 = srcWidth;
    if (srcY1 > srcHeight) srcY1 = srcHeight;

    // 统计前景像素
    int total = 0, foreground = 0;
    for (int y = srcY0; y < srcY1; y++) {
        for (int x = srcX0; x < srcX1; x++) {
            total++;
            if (BitmapScale_GetPixel(srcBitmap, type, x, y, srcWidth)) {
                foreground++;
            }
        }
    }

    // 返回比例灰度值
    return (uint8_t)((foreground * 255) / total);
}

void BitmapScale_Draw(const char *srcBitmap, int BitmapType, int srcWidth, int srcHeight,
                      uint16_t *dstBuffer, int dstBufWidth, int dstBufHeight,
                      int dstX, int dstY, int dstWidth, int dstHeight,
                      uint16_t textColor, uint16_t backColor,
                      ScaleAlgorithm_t algorithm)
{
    if (srcBitmap == NULL || dstBuffer == NULL) return;

    // 边界裁剪
    int startX = (dstX < 0) ? 0 : dstX;
    int startY = (dstY < 0) ? 0 : dstY;
    int endX = (dstX + dstWidth > dstBufWidth) ? dstBufWidth : dstX + dstWidth;
    int endY = (dstY + dstHeight > dstBufHeight) ? dstBufHeight : dstY + dstHeight;
    int relX, relY, pixel;

    // 如果目标尺寸大于源尺寸，区域平均不适合放大，回退到双线性
    if ((algorithm == SCALE_AREA_AVG) && (dstWidth > srcWidth || dstHeight > srcHeight)) {
        algorithm = SCALE_BILINEAR;
    }

    // 逐像素绘制
    for (int y = startY; y < endY; y++)
    {
        for (int x = startX; x < endX; x++)
        {
            relX = x - dstX;
            relY = y - dstY;

            if (algorithm == SCALE_NEAREST)
            {
                // 最近邻：直接二值判断
                pixel = nearest_interp(srcBitmap, BitmapType, srcWidth, srcHeight,
                                           relX, relY, dstWidth, dstHeight);

                if (pixel) {
                    dstBuffer[y * dstBufWidth + x] = textColor;
                } else if (backColor != COLOR_TRANSPARENT) {
                    dstBuffer[y * dstBufWidth + x] = backColor;
                }
            }
            else
            {
                // 双线性或区域平均：获取灰度值并混合
                uint8_t gray;
                if (algorithm == SCALE_BILINEAR) {
                    gray = bilinear_interp_gray(srcBitmap, BitmapType, srcWidth, srcHeight,
                                                relX, relY, dstWidth, dstHeight);
                } else { // SCALE_AREA_AVG
                    gray = area_average_gray(srcBitmap, BitmapType, srcWidth, srcHeight,
                                             relX, relY, dstWidth, dstHeight);
                }

                if (gray < 80) { /* 透明度阈值，可调整 */
                    if (backColor != COLOR_TRANSPARENT) {
                        dstBuffer[y * dstBufWidth + x] = backColor;
                    }
                } else if (gray == 255) {
                    dstBuffer[y * dstBufWidth + x] = textColor;
                }
                else {
                    uint16_t bpiexl = blend_rgb565(textColor, dstBuffer[y * dstBufWidth + x], gray);
                    dstBuffer[y * dstBufWidth + x] = bpiexl;
                }
            }
        }
    }
}

void BitmapScale_DrawGray(const uint8_t *srcGrayBitmap, 
                          int srcWidth, int srcHeight,
                          uint16_t *dstBuffer, int dstBufWidth, int dstBufHeight,
                          int dstX, int dstY, int dstWidth, int dstHeight,
                          uint16_t textColor, uint16_t backColor,
                          ScaleAlgorithm_t algorithm)
{
    if (srcGrayBitmap == NULL || dstBuffer == NULL) return;

    /* 边界裁剪 */
    int startX = (dstX < 0) ? 0 : dstX;
    int startY = (dstY < 0) ? 0 : dstY;
    int endX = (dstX + dstWidth > dstBufWidth) ? dstBufWidth : dstX + dstWidth;
    int endY = (dstY + dstHeight > dstBufHeight) ? dstBufHeight : dstY + dstHeight;
    int relX, relY;

    // 如果目标尺寸大于源尺寸，区域平均不适合放大，回退到双线性
    if ((algorithm == SCALE_AREA_AVG) && (dstWidth > srcWidth || dstHeight > srcHeight)) {
        algorithm = SCALE_BILINEAR;
    }

    for (int y = startY; y < endY; y++)
    {
        for (int x = startX; x < endX; x++)
        {
            relX = x - dstX;
            relY = y - dstY;
            uint8_t gray;

            if (algorithm == SCALE_NEAREST)
            {
                // 最近邻：直接采样
                int srcX = (relX * srcWidth) / dstWidth;
                int srcY = (relY * srcHeight) / dstHeight;
                gray = srcGrayBitmap[srcY * srcWidth + srcX];
            }
            else if (algorithm == SCALE_BILINEAR)
            {
                // 双线性插值（浮点版本，适合放大）
                float srcXf = relX * srcWidth / (float)dstWidth;
                float srcYf = relY * srcHeight / (float)dstHeight;

                int x0 = (int)srcXf;
                int y0 = (int)srcYf;
                int x1 = (x0 + 1 < srcWidth) ? x0 + 1 : x0;
                int y1 = (y0 + 1 < srcHeight) ? y0 + 1 : y0;
                
                float fx = srcXf - x0;
                float fy = srcYf - y0;
                
                uint8_t p00 = srcGrayBitmap[y0 * srcWidth + x0];
                uint8_t p01 = srcGrayBitmap[y1 * srcWidth + x0];
                uint8_t p10 = srcGrayBitmap[y0 * srcWidth + x1];
                uint8_t p11 = srcGrayBitmap[y1 * srcWidth + x1];
                
                float val = (1-fx)*(1-fy)*p00 + (1-fx)*fy*p01 + 
                            fx*(1-fy)*p10 + fx*fy*p11;
                gray = (uint8_t)(val + 0.5f);
            }
            else // SCALE_AREA_AVG
            {
                // 区域平均（适合缩小）
                int srcX0 = (relX * srcWidth) / dstWidth;
                int srcY0 = (relY * srcHeight) / dstHeight;
                int srcX1 = ((relX + 1) * srcWidth + dstWidth - 1) / dstWidth;
                int srcY1 = ((relY + 1) * srcHeight + dstHeight - 1) / dstHeight;
                
                if (srcX1 > srcWidth) srcX1 = srcWidth;
                if (srcY1 > srcHeight) srcY1 = srcHeight;
                
                int total = 0, sum = 0;
                for (int sy = srcY0; sy < srcY1; sy++) {
                    for (int sx = srcX0; sx < srcX1; sx++) {
                        total++;
                        sum += srcGrayBitmap[sy * srcWidth + sx];
                    }
                }
                gray = total ? (uint8_t)(sum / total) : 0;
            }

            /* 绘制到帧缓冲：使用灰度作为Alpha混合 */
            if (gray < 80) { /* 透明度阈值，可调整 */
                if (backColor != COLOR_TRANSPARENT) {
                    dstBuffer[y * dstBufWidth + x] = backColor;
                }
            }
            else if (gray == 255) {
                dstBuffer[y * dstBufWidth + x] = textColor;
            }
            else {
                // 灰度作为Alpha值混合前景色和背景色
                uint16_t blended = blend_rgb565(textColor, dstBuffer[y * dstBufWidth + x], gray);
                dstBuffer[y * dstBufWidth + x] = blended;
            }
        }
    }
}
#endif
