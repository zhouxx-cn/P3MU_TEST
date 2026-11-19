/* 编码声明: UTF-8 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "lcd_layer.h"
#include "font_scale.h"

#ifdef ENABLE_LCD_HZK16
#include "font_hzk16.h"
#endif

#include "font_custom.h"

#ifdef ENABLE_LCD_VECTOR_FONT
#include "vector_font_stb.h"
#endif

/* ==================== 内部函数声明 ==================== */

static LayerType_t OsdTypeConvertToLayer(LayerOsd_t osd);
static void Layer_MarkDirty(LayerManager_t *manager, LayerType_t type);
static int Layer_SetOSD(LayerManager_t *manager, LayerOsd_t OsdType, const char *text);
static int Layer_SetInfo(LayerManager_t *manager, LayerType_t type, const uint8_t *imageData,
                         uint16_t x, uint16_t y, uint16_t width, uint16_t height);
static int Layer_ClrInfo(LayerManager_t *manager, LayerType_t type);
static void Layer_OSDInit(LayerManager_t *manager);

/* 绘制函数 */
#ifdef ENABLE_LCD_HZK16
static int Draw_HZK16BitmapToBuffer(uint16_t *buffer, uint16_t bufWidth, uint16_t bufHeight, 
                                    OSD_Config_t *cfg);
#endif

#ifdef ENABLE_LCD_CUSTOM_BITMAP
static int Draw_CustomBitmapToBuffer(uint16_t *buffer, uint16_t bufWidth, uint16_t bufHeight, 
                                     OSD_Config_t *cfg);
#endif

#ifdef ENABLE_LCD_VECTOR_FONT
static int Draw_StbVectorTextToBuffer(uint16_t *buffer, uint16_t bufWidth, uint16_t bufHeight, 
                                      OSD_Config_t *cfg);
#endif

static int Draw_CustomShapeBitmapToBuffer(uint16_t *buffer, uint16_t bufWidth, uint16_t bufHeight, 
                                          OSD_Config_t *cfg, CustomBitmapInfo_st *bitmapInfo, 
                                          uint8_t customValue);

/* ==================== 类型转换 ==================== */

static LayerType_t OsdTypeConvertToLayer(LayerOsd_t osd)
{
    static const LayerType_t OSD_TO_LAYER_MAP[LAYER_OSD_MAX] = {
        [LAYER_OSD_TEXT_1] = LAYER_TYPE_TEXT_1,
        [LAYER_OSD_TEXT_2] = LAYER_TYPE_TEXT_2,
        [LAYER_OSD_WIFI] = LAYER_TYPE_WIFI
    };
    
    if (osd >= LAYER_OSD_MAX) {
        return LAYER_TYPE_MAX;
    }
    return OSD_TO_LAYER_MAP[osd];
}

/* ==================== 绘制实现 ==================== */

#ifdef ENABLE_LCD_HZK16
/**
 * @brief 使用HZK16字库绘制文本到缓冲区
 */
static int Draw_HZK16BitmapToBuffer(uint16_t *buffer, uint16_t bufWidth, uint16_t bufHeight, 
                                    OSD_Config_t *cfg)
{
    if (buffer == NULL || cfg == NULL) {
        LCD_ERR("Invalid parameter: buffer=%p, cfg=%p\n", buffer, cfg);
        return 0;
    }

    /* 计算原始点阵宽高 */
    uint16_t origWidth = Calc_UnicodeWidth(cfg->TextUnicode, cfg->TextLen);
    const uint16_t origHeight = 16; /* HZK16固定16x16 */

    /* 计算缩放后尺寸（四舍五入） */
    uint16_t scaledWidth = (origWidth * cfg->scale + 50) / 100;
    uint16_t scaledHeight = (origHeight * cfg->scale + 50) / 100;

    cfg->osdPixelWidth = scaledWidth;
    cfg->osdPixelHeight = scaledHeight;

    /* 计算水平对齐后的起始X坐标 */
    if (cfg->align == OSD_ALIGN_CENTER) {
        cfg->x = (scaledWidth < bufWidth) ? ((bufWidth - scaledWidth) / 2) : 0;
    } else if (cfg->align == OSD_ALIGN_RIGHT) {
        cfg->x = (scaledWidth < bufWidth) ? (bufWidth - scaledWidth - cfg->x) : 0;
    }

    /* Unicode转GB2312 */
    uint16_t gb2312Buf[OSD_TEXT_UNICODE_MAX_LENGTH * 2] = {0};
    UniToStr(cfg->TextUnicode, (uint8_t *)gb2312Buf);

    /* 分配点阵缓冲区 */
    int bitmapSize = (scaledHeight * scaledWidth) >> 3;
    if (bitmapSize <= 0) {
        LCD_ERR("Invalid bitmap size: %d\n", bitmapSize);
        return 0;
    }
    
    char *bitmap = (char *)malloc(bitmapSize);
    if (bitmap == NULL) {
        LCD_ERR("malloc bitmap failed, size=%d\n", bitmapSize);
        return 0;
    }

    /* 读取字库点阵 */
    if (findStrInHZK16(gb2312Buf, bitmap) != 0) {
        LCD_ERR("findStrInHZK16 failed\n");
        free(bitmap);
        return 0;
    }

    /* 选择缩放算法 */
    ScaleAlgorithm_t algo = (cfg->scale < SCALE_THRESHOLD_SMALLER) ? 
                            SCALE_AREA_AVG : SCALE_BILINEAR;

    /* 执行缩放绘制 */
    BitmapScale_Draw(bitmap, 0, origWidth, origHeight,
                     buffer, bufWidth, bufHeight, cfg->x, cfg->y, 
                     scaledWidth, scaledHeight,
                     cfg->textColor, cfg->backColor, algo);

    free(bitmap);
    return scaledWidth;
}
#endif /* ENABLE_LCD_HZK16 */

#ifdef ENABLE_LCD_CUSTOM_BITMAP
/**
 * @brief 使用自定义位图字体绘制文本
 */
static int Draw_CustomBitmapToBuffer(uint16_t *buffer, uint16_t bufWidth, uint16_t bufHeight, 
                                     OSD_Config_t *cfg)
{
    if (buffer == NULL || cfg == NULL) {
        LCD_ERR("Invalid parameter\n");
        return 0;
    }

    /* 计算总尺寸 */
    uint16_t totalWidth = Calc_StringWidth_Auto_Unicode(cfg->TextUnicode);
    uint16_t totalHeight = Calc_StringHeight_Auto_Unicode(cfg->TextUnicode);

    cfg->osdPixelWidth = (totalWidth * cfg->scale + 50) / 100;
    cfg->osdPixelHeight = (totalHeight * cfg->scale + 50) / 100;

    /* 水平对齐计算 */
    if (cfg->align == OSD_ALIGN_CENTER) {
        cfg->x = (cfg->osdPixelWidth < bufWidth) ? ((bufWidth - cfg->osdPixelWidth) / 2) : 0;
    } else if (cfg->align == OSD_ALIGN_RIGHT) {
        cfg->x = (cfg->osdPixelWidth < bufWidth) ? (bufWidth - cfg->osdPixelWidth - cfg->x) : 0;
    }

    int currentX = cfg->x;
    int drawnWidth = 0;

    /* 逐字符绘制 */
    for (int i = 0; cfg->TextUnicode[i] != 0; i++) {
        uint16_t code = cfg->TextUnicode[i];
        CustomBitmapInfo_st *font = GetCustomFontByUnicode(code);
        if (font == NULL) {
            LCD_WRAN("No font for unicode: %hu\n", code);
            continue;
        }

        int idx = FindCharIndex(font->Charset, code, font->CharCount);
        if (idx < 0) {
            LCD_WRAN("Char not found in font: %hu\n", code);
            continue;
        }

        const uint8_t *charData = font->FontData + idx * font->CharSize;
        int dstWidth = ((font->CharWith * cfg->scale) + 50) / 100;
        int dstHeight = ((font->CharHeight * cfg->scale) + 50) / 100;

        BitmapScale_Draw((const char *)charData, 1, font->CharWith, font->CharHeight,
                         buffer, bufWidth, bufHeight, currentX, cfg->y, 
                         dstWidth, dstHeight,
                         cfg->textColor, cfg->backColor, SCALE_BILINEAR);

        currentX += dstWidth;
        drawnWidth += dstWidth;
    }

    return drawnWidth;
}
#endif /* ENABLE_LCD_CUSTOM_BITMAP */

/**
 * @brief 绘制自定义形状位图（如WiFi图标）
 */
static int Draw_CustomShapeBitmapToBuffer(uint16_t *buffer, uint16_t bufWidth, uint16_t bufHeight,
                                          OSD_Config_t *cfg, CustomBitmapInfo_st *bitmapInfo,
                                          uint8_t customValue)
{
    if (buffer == NULL || cfg == NULL || bitmapInfo == NULL) {
        LCD_ERR("Invalid parameter\n");
        return 0;
    }

    /* 计算缩放后尺寸 */
    cfg->osdPixelWidth = (bitmapInfo->CharWith * cfg->scale + 50) / 100;
    cfg->osdPixelHeight = (bitmapInfo->CharHeight * cfg->scale + 50) / 100;

    /* 水平对齐 */
    if (cfg->align == OSD_ALIGN_CENTER) {
        cfg->x = (cfg->osdPixelWidth < bufWidth) ? ((bufWidth - cfg->osdPixelWidth) / 2) : 0;
    } else if (cfg->align == OSD_ALIGN_RIGHT) {
        cfg->x = (cfg->osdPixelWidth < bufWidth) ? (bufWidth - cfg->osdPixelWidth - cfg->x) : 0;
    }

    /* 获取对应索引的位图数据 */
    if (customValue >= bitmapInfo->CharCount) {
        LCD_ERR("customValue out of range: %u >= %u\n", customValue, bitmapInfo->CharCount);
        customValue = 0;
    }

    const uint8_t *charData = &(bitmapInfo->FontData[bitmapInfo->CharSize * customValue]);

    /* 选择算法并绘制 */
    ScaleAlgorithm_t algo = (cfg->scale < SCALE_THRESHOLD_SMALLER) ? 
                            SCALE_AREA_AVG : SCALE_BILINEAR;

    BitmapScale_DrawGray((const uint8_t *)charData, bitmapInfo->CharWith, bitmapInfo->CharHeight,
                          buffer, bufWidth, bufHeight, cfg->x, cfg->y,
                          cfg->osdPixelWidth, cfg->osdPixelHeight,
                          cfg->textColor, cfg->backColor, algo);

    return cfg->osdPixelWidth;
}

#ifdef ENABLE_LCD_VECTOR_FONT
/**
 * @brief 使用STB矢量字体绘制文本
 */
static int Draw_StbVectorTextToBuffer(uint16_t *buffer, uint16_t bufWidth, uint16_t bufHeight,
                                      OSD_Config_t *cfg)
{
    if (buffer == NULL || cfg == NULL) {
        LCD_ERR("Invalid parameter\n");
        return 0;
    }

    /* 计算目标像素高度 */
    int pixelHeight = ((DEFAULT_REFERENCE_PIXEL_H * cfg->scale + 50) / 100);
    const int letterSpacing = 0; /* 字符间距，可配置 */

    /* 计算字符串总尺寸 */
    int totalWidth = 0, totalHeight = 0, baselineY = 0;
    if (STB_ComputeStringSize(cfg->TextUnicode, pixelHeight, &totalWidth, &totalHeight, &baselineY) != 0) {
        LCD_ERR("STB_ComputeStringSize failed\n");
        return 0;
    }

    cfg->osdPixelWidth = totalWidth + letterSpacing * (cfg->TextLen - 1);
    cfg->osdPixelHeight = totalHeight;

    /* 水平对齐 */
    if (cfg->align == OSD_ALIGN_CENTER) {
        cfg->x = (cfg->osdPixelWidth < bufWidth) ? ((bufWidth - cfg->osdPixelWidth) / 2) : 0;
    } else if (cfg->align == OSD_ALIGN_RIGHT) {
        cfg->x = (cfg->osdPixelWidth < bufWidth) ? (bufWidth - cfg->osdPixelWidth - cfg->x) : 0;
    }

    baselineY += cfg->y;
    int currentX = cfg->x;
    int drawnWidth = 0;

    /* 逐字符绘制 */
    for (int i = 0; cfg->TextUnicode[i] != 0; i++) {
        uint16_t code = cfg->TextUnicode[i];
        
        int charWidth = STB_DrawChar(buffer, bufWidth, bufHeight,
                                     currentX, cfg->y, code, pixelHeight,
                                     cfg->textColor, cfg->backColor, baselineY);
        
        if (charWidth < 0) {
            LCD_WRAN("STB_DrawChar failed for code: %hu\n", code);
            continue;
        }

        currentX += (charWidth + letterSpacing);
        drawnWidth += (charWidth + letterSpacing);
    }

    LCD_DBG("Vector text: totalW=%d, totalH=%d, drawnW=%d, pixelH=%d\n",
            totalWidth, totalHeight, drawnWidth, pixelHeight);
    
    return drawnWidth;
}
#endif /* ENABLE_LCD_VECTOR_FONT */

/* ==================== 图层管理核心 ==================== */

/**
 * @brief 设置OSD文本内容
 * @return 1内容变化，0无变化
 */
static int Layer_SetOSD(LayerManager_t *manager, LayerOsd_t OsdType, const char *text)
{
    if (manager == NULL || text == NULL || OsdType >= LAYER_OSD_MAX) {
        LCD_ERR("Invalid parameter\n");
        return 0;
    }

    OSD_Config_t *osd = &manager->LayerOsdCfg[OsdType];
    size_t textLen = strlen(text);

    /* 检查内容是否变化 */
    if (strncmp((char *)osd->TextStr, text, textLen) == 0 && osd->TextStr[textLen] == '\0') {
        return 0; /* 无变化 */
    }

    /* 拷贝字符串 */
    strncpy((char *)osd->TextStr, text, OSD_TEXT_UNICODE_MAX_LENGTH - 1);
    osd->TextStr[OSD_TEXT_UNICODE_MAX_LENGTH - 1] = '\0';

    /* 转换为Unicode数组 */
    int count = utf8_to_unicode_array((const uint8_t *)text, osd->TextUnicode, 
                                     sizeof(osd->TextUnicode) / sizeof(osd->TextUnicode[0]));
    if (count > 0) {
        osd->TextUnicode[count] = 0; /* 结束符 */
        osd->TextLen = count;
    } else {
        osd->TextLen = 0;
    }

    osd->changed = 1;
    LCD_INF("[SetOSD] type=%d, text=%s, len=%zu, unicodeCount=%d\n", 
            OsdType, text, textLen, count);

    return 1;
}

/**
 * @brief 标记图层区域为脏（需要刷新）
 */
static void Layer_MarkDirty(LayerManager_t *manager, LayerType_t type)
{
    if (manager == NULL || type >= LAYER_TYPE_MAX) {
        return;
    }

    LayerInfo_t *layer = &manager->layers[type];
    if (layer->state == LAYER_STATE_INVALID) {
        return;
    }

    layer->state = LAYER_STATE_DIRTY;

    /* 扩展脏矩形包含该图层区域 */
    uint16_t layerX2 = layer->x + layer->width;
    uint16_t layerY2 = layer->y + layer->height;

    if (!manager->hasDirtyRect) {
        /* 第一个脏区域 */
        manager->dirtyX = layer->x;
        manager->dirtyY = layer->y;
        manager->dirtyW = layer->width;
        manager->dirtyH = layer->height;
        manager->hasDirtyRect = 1;
    } else {
        /* 合并矩形：取并集 */
        uint16_t dirtyX2 = manager->dirtyX + manager->dirtyW;
        uint16_t dirtyY2 = manager->dirtyY + manager->dirtyH;

        uint16_t newX = (manager->dirtyX < layer->x) ? manager->dirtyX : layer->x;
        uint16_t newY = (manager->dirtyY < layer->y) ? manager->dirtyY : layer->y;
        uint16_t newX2 = (dirtyX2 > layerX2) ? dirtyX2 : layerX2;
        uint16_t newY2 = (dirtyY2 > layerY2) ? dirtyY2 : layerY2;

        manager->dirtyX = newX;
        manager->dirtyY = newY;
        manager->dirtyW = newX2 - newX;
        manager->dirtyH = newY2 - newY;
    }
}

/**
 * @brief 设置图层信息（位置、尺寸、数据）
 * @return 1成功，0失败
 */
static int Layer_SetInfo(LayerManager_t *manager, LayerType_t type, const uint8_t *imageData,
                         uint16_t x, uint16_t y, uint16_t width, uint16_t height)
{
    if (manager == NULL || type >= LAYER_TYPE_MAX) {
        LCD_ERR("Invalid parameter\n");
        return 0;
    }

    LayerInfo_t *layer = &manager->layers[type];
    uint16_t newX = x, newY = y, newW = width, newH = height;

    /* 与已有有效区域合并（取并集） */
    if (layer->state != LAYER_STATE_INVALID) {
        uint16_t oldX2 = layer->x + layer->width;
        uint16_t oldY2 = layer->y + layer->height;
        uint16_t newX2 = x + width;
        uint16_t newY2 = y + height;

        newX = (layer->x < x) ? layer->x : x;
        newY = (layer->y < y) ? layer->y : y;
        newW = ((oldX2 > newX2) ? oldX2 : newX2) - newX;
        newH = ((oldY2 > newY2) ? oldY2 : newY2) - newY;
    }

    /* 边界裁剪 */
    if (newX >= manager->screenWidth || newY >= manager->screenHeight) {
        LCD_WRAN("Layer position out of screen: (%u,%u)\n", newX, newY);
        return 0;
    }

    uint16_t maxW = manager->screenWidth - newX;
    uint16_t maxH = manager->screenHeight - newY;

    if (newW > maxW) newW = maxW;
    if (newH > maxH) newH = maxH;

    /* 更新图层信息 */
    layer->x = newX;
    layer->y = newY;
    layer->width = newW;
    layer->height = newH;

    if (imageData != NULL) {
        layer->data = imageData;
        layer->dataSize = (uint32_t)newW * newH * 2; /* RGB565 = 2 bytes/pixel */
    }

    layer->state = LAYER_STATE_VALID;

    LCD_DBG("[SetInfo] type=%d, pos=(%u,%u), size=%ux%u\n", type, newX, newY, newW, newH);
    return 1;
}

/**
 * @brief 清除图层信息
 */
static int Layer_ClrInfo(LayerManager_t *manager, LayerType_t type)
{
    if (manager == NULL || type >= LAYER_TYPE_MAX) {
        return -1;
    }

    LayerInfo_t *layer = &manager->layers[type];
    
    memset(layer, 0, sizeof(LayerInfo_t));
    layer->state = LAYER_STATE_INVALID;

    LCD_DBG("[ClrInfo] type=%d\n", type);
    return 0;
}

/**
 * @brief 初始化所有OSD配置
 */
static void Layer_OSDInit(LayerManager_t *manager)
{
    if (manager == NULL) {
        return;
    }

    for (int i = 0; i < LAYER_OSD_MAX; i++) {
        manager->LayerOsdCfg[i].x = 0;
        manager->LayerOsdCfg[i].y = 0;
        manager->LayerOsdCfg[i].osdPixelWidth = 0;
        manager->LayerOsdCfg[i].osdPixelHeight = 0;
        manager->LayerOsdCfg[i].scale = 100;
        manager->LayerOsdCfg[i].align = OSD_ALIGN_CENTER;
        manager->LayerOsdCfg[i].textColor = COLOUR_WHITE;
        manager->LayerOsdCfg[i].backColor = COLOR_TRANSPARENT;
        manager->LayerOsdCfg[i].enable = 0;
        manager->LayerOsdCfg[i].changed = 0;
        manager->LayerOsdCfg[i].forceUpdate = 0;
        manager->LayerOsdCfg[i].TextLen = 0;
        manager->LayerOsdCfg[i].TextStr[0] = '\0';
        manager->LayerOsdCfg[i].TextUnicode[0] = 0;
    }
}

/* ==================== 对外API实现 ==================== */
int Layer_Init(LayerManager_t *manager, uint16_t screenWidth, uint16_t screenHeight)
{
    if (manager == NULL || screenWidth == 0 || screenHeight == 0) {
        LCD_ERR("Invalid parameter: manager=%p, size=%ux%u\n", manager, screenWidth, screenHeight);
        return -1;
    }

    /* 初始化管理器结构 */
    memset(manager, 0, sizeof(LayerManager_t));
    manager->screenWidth = screenWidth;
    manager->screenHeight = screenHeight;

    /* 分配全局帧缓冲（仅分配一次） */
    manager->compositeBuffer = (uint8_t *)malloc(screenWidth * screenHeight * 2);
    if (manager->compositeBuffer == NULL) {
        LCD_ERR("malloc framebuffer failed, size=%u\n", screenWidth * screenHeight * 2);
        return -1;
    }
    memset(manager->compositeBuffer, 0, screenWidth * screenHeight * 2);

    /* 初始化所有图层为无效状态 */
    for (int i = 0; i < LAYER_TYPE_MAX; i++) {
        manager->layers[i].state = LAYER_STATE_INVALID;
    }

#ifdef ENABLE_LCD_VECTOR_FONT
    STB_Font_Init();
#endif

    Layer_OSDInit(manager);
    
    LCD_INF("Layer_Init success: %ux%u\n", screenWidth, screenHeight);
    return 0;
}

int Layer_UnInit(LayerManager_t *manager)
{
    if (manager == NULL) {
        LCD_ERR("Invalid parameter\n");
        return -1;
    }

#ifdef ENABLE_LCD_VECTOR_FONT
    STB_Font_Deinit();
#endif

    if (manager->compositeBuffer != NULL) {
        free(manager->compositeBuffer);
        manager->compositeBuffer = NULL;
    }

    LCD_INF("Layer_UnInit success\n");
    return 0;
}

int Layer_SetRefreshCallback(LayerManager_t *manager, REFRESH_CALLBACK pCallback)
{
    if (manager == NULL || pCallback == NULL) {
        LCD_ERR("Invalid manager\n");
        return -1;
    }

    manager->pCallback = pCallback;

    return 0;
}

int Layer_Composite(LayerManager_t *manager)
{
    if (manager == NULL || manager->compositeBuffer == NULL) {
        LCD_ERR("Invalid manager\n");
        return -1;
    }

    uint16_t *fb = (uint16_t *)manager->compositeBuffer;
    uint16_t screenW = manager->screenWidth;
    uint16_t screenH = manager->screenHeight;

    /* 1. 绘制背景层 */
    LayerInfo_t *bgLayer = &manager->layers[LAYER_TYPE_BACKGROUND];
    if (bgLayer->state != LAYER_STATE_INVALID && bgLayer->data != NULL) {
        const uint16_t *bgData = (const uint16_t *)bgLayer->data;

        for (uint16_t y = 0; y < bgLayer->height && y < screenH; y++) {
            for (uint16_t x = 0; x < bgLayer->width && x < screenW; x++) {
                fb[y * screenW + x] = bgData[y * bgLayer->width + x];
            }
        }
    }

    /* 2. 绘制其他图片层（IMG_1, IMG_2等） */
    for (int i = LAYER_TYPE_BACKGROUND + 1; i < LAYER_TYPE_MAX; i++) {
        LayerInfo_t *layer = &manager->layers[i];
        
        if (layer->state != LAYER_STATE_INVALID && layer->data != NULL) {
            const uint16_t *srcData = (const uint16_t *)layer->data;
            
            for (uint16_t y = 0; y < layer->height && (y + layer->y) < screenH; y++) {
                for (uint16_t x = 0; x < layer->width && (x + layer->x) < screenW; x++) {
                    uint16_t dstX = layer->x + x;
                    uint16_t dstY = layer->y + y;
                    fb[dstY * screenW + dstX] = srcData[y * layer->width + x];
                }
            }

            Layer_MarkDirty(manager, (LayerType_t)i);
            LCD_DBG("[Composite] Image layer %d at (%u,%u) size %ux%u\n", 
                    i, layer->x, layer->y, layer->width, layer->height);
        }
    }

    /* 3. 绘制OSD层 */
    for (int i = 0; i < LAYER_OSD_MAX; i++) {
        LayerType_t layerType = OsdTypeConvertToLayer((LayerOsd_t)i);
        if (layerType == LAYER_TYPE_MAX) {
            continue;
        }

        OSD_Config_t *osd = &manager->LayerOsdCfg[i];
#if 0
        if (!osd->enable || (!osd->changed && !osd->forceUpdate)) {
            continue; /* 未使能或无变化 */
        }
#else
        if (!osd->enable) {
            continue; /* 未使能, 基线使用全屏刷新，只判断使能 */
        }
#endif

        /* 绘制OSD到帧缓冲 */
        if (i == LAYER_OSD_WIFI) {
            /* WiFi图标使用自定义形状位图 */
            Draw_CustomShapeBitmapToBuffer(fb, screenW, screenH, osd, &WIFI_IMP_FontLibInfo, (uint8_t)manager->customcfg.wifiSignalLevel);

            /* 叠加WiFi状态异常提示 */
            if (manager->customcfg.wifiStatus)
            {
                uint16_t x, y, dstX, dstY;
                for (y = 0; y < 16; y++) {
                    for (x = 0; x < 16; x++) {
                        dstX = osd->x + 25 + x;
                        dstY = osd->y + 15 + y;
                        if (WIFI_DISCONNECT_RGB565Data[y * 16 + x] != COLOR_TRANSPARENT) {
                            fb[dstY * screenW + dstX] = WIFI_DISCONNECT_RGB565Data[y * 16 + x];
                        }
                    }
                }
            }
        } else {
            /* 文本使用配置的字体引擎 */
#if (CURRENT_FONT_ENGINE == FONT_ENGINE_CUSTOM_BITMAP)
            Draw_CustomBitmapToBuffer(fb, screenW, screenH, osd);
#elif (CURRENT_FONT_ENGINE == FONT_ENGINE_HZK16)
            Draw_HZK16BitmapToBuffer(fb, screenW, screenH, osd);
#elif (CURRENT_FONT_ENGINE == FONT_ENGINE_VECTOR)
            Draw_StbVectorTextToBuffer(fb, screenW, screenH, osd);
#else
            LCD_WRAN("[Composite] Unknown font engine: %d\n", CURRENT_FONT_ENGINE);
#endif
        }

        /* 更新图层信息 */
        Layer_SetInfo(manager, layerType, NULL, osd->x, osd->y, 
                      osd->osdPixelWidth, osd->osdPixelHeight);

        /* 标记脏区域 */
        Layer_MarkDirty(manager, layerType);

        /* 清除变化标记 */
        osd->changed = 0;
        osd->forceUpdate = 0;

        LCD_DBG("[Composite] OSD %d at (%u,%u) size %ux%u\n", 
                i, osd->x, osd->y, osd->osdPixelWidth, osd->osdPixelHeight);
    }

    return 0;
}

int Layer_Refresh(LayerManager_t *manager)
{
    if (manager == NULL || !manager->hasDirtyRect) {
        return 0; /* 无需刷新 */
    }

#if 0
    /* 边界检查与裁剪 */
    uint16_t x = manager->dirtyX;
    uint16_t y = manager->dirtyY;
    uint16_t w = manager->dirtyW;
    uint16_t h = manager->dirtyH;

    if (x >= manager->screenWidth || y >= manager->screenHeight) {
        LCD_ERR("Dirty rect out of bounds: (%u,%u)\n", x, y);
        manager->hasDirtyRect = 0;
        return -1;
    }

    if (x + w > manager->screenWidth) {
        w = manager->screenWidth - x;
    }
    if (y + h > manager->screenHeight) {
        h = manager->screenHeight - y;
    }

    if (w == 0 || h == 0) {
        LCD_WRAN("Empty dirty rect\n");
        manager->hasDirtyRect = 0;
        return 0;
    }

    /* 分配临时缓冲区 */
    uint32_t dataSize = (uint32_t)w * h * 2;
    uint8_t *tempBuffer = (uint8_t *)malloc(dataSize);
    if (tempBuffer == NULL) {
        LCD_ERR("malloc temp buffer failed, size=%u\n", dataSize);
        return -1;
    }

    /* 从帧缓冲提取脏区域数据 */
    for (uint16_t row = 0; row < h; row++) {
        uint8_t *src = manager->compositeBuffer + ((y + row) * manager->screenWidth + x) * 2;
        uint8_t *dst = tempBuffer + row * w * 2;
        memcpy(dst, src, w * 2);
    }

    /* 更新脏区域到LCD硬件 */
    manager->pCallback(x, y, w, h, tempBuffer, dataSize);

    /* 清理 */
    manager->hasDirtyRect = 0;
    free(tempBuffer);
#else
    uint16_t x = 0;
    uint16_t y = 0;
    uint16_t w = manager->screenWidth;
    uint16_t h = manager->screenHeight;
    uint32_t dataSize = (uint32_t)(w*h*2);

    if (manager->pCallback) {
        /* 更新全屏到LCD硬件，基线当前只支持全屏刷新 */
        manager->pCallback(x, y, w, h, manager->compositeBuffer, dataSize);
    }
#endif

    //LCD_INF("[Refresh] area=(%u,%u,%u,%u), size=%u\n", x, y, w, h, dataSize);
    return 0;
}

void Layer_ClearAll(LayerManager_t *manager)
{
    if (manager == NULL) {
        return;
    }

    /* 关闭所有OSD使能 */
    for (int i = 0; i < LAYER_OSD_MAX; i++) {
        manager->LayerOsdCfg[i].enable = 0;
        manager->LayerOsdCfg[i].changed = 0;
        manager->LayerOsdCfg[i].forceUpdate = 0;
    }

    /* 清除所有非背景图层 */
    for (int i = LAYER_TYPE_BACKGROUND + 1; i < LAYER_TYPE_MAX; i++) {
        if (manager->layers[i].state != LAYER_STATE_INVALID) {
            Layer_ClrInfo(manager, (LayerType_t)i);
        }
    }

    //LCD_INF("Layer_ClearAll\n");
}

int Layer_UpdateBackground(LayerManager_t *manager, const ImageResource_t *image)
{
    if (manager == NULL) {
        LCD_ERR("Invalid manager\n");
        return 0;
    }

    /* 更新背景层信息 */
    int result = Layer_SetInfo(manager, LAYER_TYPE_BACKGROUND, image->data, image->x, image->y, image->width, image->height);

    if (result) {
        /* 全屏标记为脏 */
        manager->dirtyX = 0;
        manager->dirtyY = 0;
        manager->dirtyW = image->width;
        manager->dirtyH = image->height;
        manager->hasDirtyRect = 1;
    }

    return result;
}

int Layer_SetImageLayer(LayerManager_t *manager, const ImageResource_t *imagegroup, int imagegrouplen)
{
    if (manager == NULL || imagegroup == NULL || imagegrouplen <= 0) {
        LCD_ERR("Invalid parameter: imagegrouplen=%d\n", imagegrouplen);
        return 0;
    }

    int result = 0;
    LayerType_t type;

    for (int i = 0; i < imagegrouplen; i++)
    {
        switch (i) {
            case 0:     type = LAYER_TYPE_IMG_1;    break;
            case 1:     type = LAYER_TYPE_IMG_2;    break;
            default:    
                LCD_ERR("It's already the biggest!! %d\n", imagegrouplen);
                return result;
        }

        if (imagegroup[i].data != NULL)
        {
            result |= Layer_SetInfo(manager, type, imagegroup[i].data,
                                    imagegroup[i].x, imagegroup[i].y,
                                    imagegroup[i].width, imagegroup[i].height);
        }
    }

    return result;
}

int Layer_SetTextLayer(LayerManager_t *manager, const TextResource_t *textgroup, int textgrouplen)
{
    if (manager == NULL || textgroup == NULL || textgrouplen <= 0) {
        LCD_ERR("Invalid parameter: imagegrouplen=%d\n", textgrouplen);
        return 0;
    }

    int result = 0;
    LayerOsd_t type;

    for (int i = 0; i < textgrouplen; i++)
    {
        switch (i) {
            case 0:     type = LAYER_OSD_TEXT_1;    break;
            case 1:     type = LAYER_OSD_TEXT_2;    break;
            default:    
                LCD_ERR("It's already the biggest!! %d\n", textgrouplen);
                return result;
        }

        OSD_Config_t *osd = &manager->LayerOsdCfg[type];

        if (textgroup[i].TextStr != NULL)
        {
            result |= Layer_SetOSD(manager, type, textgroup[i].TextStr);
            if (!osd->enable) {
                osd->x = textgroup[i].x;
                osd->y = textgroup[i].y;
                osd->scale = textgroup[i].scale;
                osd->align = textgroup[i].align;
                osd->enable = 1;
                osd->forceUpdate = 1;
            }
        }
    }

    return result;
}

int Layer_SetWifiLayer(LayerManager_t *manager, const TextResource_t *textgroup, int nStatus, int nQuality)
{
    if (manager == NULL || textgroup == NULL) {
        LCD_ERR("Invalid parameter\n");
        return 0;
    }

    int result = 0;

    OSD_Config_t *wifiOsd = &manager->LayerOsdCfg[LAYER_OSD_WIFI];
    if (!wifiOsd->enable) {
        wifiOsd->x = textgroup->x;
        wifiOsd->y = textgroup->y;
        wifiOsd->scale = textgroup->scale;
        wifiOsd->align = textgroup->align;
        wifiOsd->enable = 1;
        wifiOsd->forceUpdate = 1;
        result = 1; /* 首次使能，有变化 */
    }

    WiFiSignalLevel_t newLevel;
    if (nQuality >= 80) {
        newLevel = WIFI_SIGNAL_STRONG;
    } else if (nQuality >= 60) {
        newLevel = WIFI_SIGNAL_MEDIUM;
    } else {
        newLevel = WIFI_SIGNAL_WEAK;
    }

    if (manager->customcfg.wifiSignalLevel != newLevel)
    {
        result = 1;
        manager->customcfg.wifiSignalLevel = newLevel;
        LCD_INF("WiFi signal set to: %d - %d%%\n", newLevel, nQuality);
    }

    if (manager->customcfg.wifiStatus != nStatus)
    {
        result = 1;
        manager->customcfg.wifiStatus = nStatus;
        LCD_INF("WiFi status set to: %d\n", nStatus);
    }

    return result; /* 已在使能状态 */
}
