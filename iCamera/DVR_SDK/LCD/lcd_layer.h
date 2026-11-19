#ifndef __LCD_LAYER_H__
#define __LCD_LAYER_H__

#include "lcd_common.h"

/* ==================== 常量定义 ==================== */
/* 最大显示字符个数 */
#define OSD_TEXT_UNICODE_MAX_LENGTH     (64)

/* 缩放算法阈值 */
#define SCALE_THRESHOLD_SMALLER         (100)       /* 小于此值使用AREA_AVG */

/* ==================== 枚举定义 ==================== */
/* 图层类型定义 */
typedef enum {
    LAYER_TYPE_BACKGROUND = 0,      /* 背景层（图片）- 最底层 */
    LAYER_TYPE_TEXT_1,              /* 文本1-时间 */
    LAYER_TYPE_TEXT_2,              /* 文本2-日期 */
    LAYER_TYPE_IMG_1,               /* 图片1 */
    LAYER_TYPE_IMG_2,               /* 图片2 */
    LAYER_TYPE_WIFI,                /* WiFi图标 */
    LAYER_TYPE_MAX
} LayerType_t;

/* OSD类型定义 - 与LayerType_t部分对应 */
typedef enum {
    LAYER_OSD_TEXT_1 = 0,           /* 文本1-时间 OSD层 */
    LAYER_OSD_TEXT_2,               /* 文本2-日期 OSD层 */
    LAYER_OSD_WIFI,                 /* WiFi层 */
    LAYER_OSD_MAX
} LayerOsd_t;

/* 图层状态 */
typedef enum {
    LAYER_STATE_INVALID = 0,        /* 无效/未使用 */
    LAYER_STATE_VALID,              /* 有效但未变更 */
    LAYER_STATE_DIRTY,              /* 需要刷新 */
} LayerState_t;

/* OSD文本水平对齐方式 */
typedef enum {
    OSD_ALIGN_LEFT = 0,
    OSD_ALIGN_CENTER,
    OSD_ALIGN_RIGHT,
} OSD_Align_t;

/* WiFi信号强度等级 */
typedef enum {
    WIFI_SIGNAL_STRONG = 0,         /* >= 80% */
    WIFI_SIGNAL_MEDIUM,             /* >= 60% */
    WIFI_SIGNAL_WEAK,               /* < 60% */
    WIFI_SIGNAL_MAX
} WiFiSignalLevel_t;

/**
 * @brief 更新帧缓冲指定区域，并将该区域刷新到 LCD
 * @param startX   区域左上角 X 坐标
 * @param startY   区域左上角 Y 坐标
 * @param width    区域宽度
 * @param height   区域高度
 * @param pData    要写入帧缓冲的数据（RGB565 格式，按行连续排列，每像素 2 字节）
 * @param DataSize data 的字节数（应为 width * height * 2）
 */
typedef int (*REFRESH_CALLBACK)(uint16_t startX, uint16_t startY, uint16_t width, uint16_t height, uint8_t *pData, unsigned int DataSize);

/* ==================== 结构体定义 ==================== */
/* 图层结构 */
typedef struct {
    LayerState_t    state;          /* 图层状态 */
    uint16_t        x;              /* 起始X坐标 */
    uint16_t        y;              /* 起始Y坐标 */
    uint16_t        width;          /* 宽度 */
    uint16_t        height;         /* 高度 */
    const uint8_t   *data;          /* 图层数据（RGB565格式） */
    uint32_t        dataSize;       /* 数据大小（字节） */
} LayerInfo_t;

/* OSD配置结构 */
typedef struct {
    uint16_t        x;              /* 显示位置X */
    uint16_t        y;              /* 显示位置Y */
    uint16_t        osdPixelWidth;  /* OSD实际像素宽度 */
    uint16_t        osdPixelHeight; /* OSD实际像素高度 */
    OSD_Align_t     align;          /* 对齐方式 */
    uint16_t        textColor;      /* 文字颜色 (RGB565) */
    uint16_t        backColor;      /* 背景颜色 (COLOR_TRANSPARENT=透明) */
    uint16_t        scale;          /* 缩放比例 (100为原始大小) */
    uint8_t         enable;         /* 使能标志 */
    uint8_t         changed;        /* 内容是否变化 */
    uint8_t         forceUpdate;    /* 强制更新标志 */
    uint8_t         TextStr[OSD_TEXT_UNICODE_MAX_LENGTH];      /* UTF-8字符串 */
    uint16_t        TextUnicode[OSD_TEXT_UNICODE_MAX_LENGTH];/* Unicode数组 */
    uint8_t         TextLen;        /* 字符数量 */
} OSD_Config_t;

/* 用户自定义配置结构 */
typedef struct {
    uint8_t           wifiStatus;           /* WiFi状态（0正常，其他异常） */
    WiFiSignalLevel_t wifiSignalLevel;      /* WiFi信号强度等级（0-100） */
} Custom_Config_t;

/* 图层管理器 */
typedef struct {
    LayerInfo_t     layers[LAYER_TYPE_MAX];     /* 图层数组 */
    uint8_t         layerCount;                 /* 有效图层数 */
    uint8_t         *compositeBuffer;           /* 合成缓冲区,指向fb */
    uint16_t        screenWidth;                /* 屏幕宽度 */
    uint16_t        screenHeight;               /* 屏幕高度 */
    
    Custom_Config_t customcfg;                  /* 用户配置 */
    REFRESH_CALLBACK pCallback;                 /* LCD硬件刷新接口回调 */

    /* 脏矩形管理 */
    uint16_t        dirtyX, dirtyY;             /* 脏矩形左上角 */
    uint16_t        dirtyW, dirtyH;             /* 脏矩形宽高 */
    uint8_t         hasDirtyRect;               /* 是否有脏矩形标记 */
    
    /* OSD管理 */
    OSD_Config_t    LayerOsdCfg[LAYER_OSD_MAX]; /* OSD配置数组 */
} LayerManager_t;

/* 图片资源描述结构 - 用于外部传入图片数据 */
typedef struct {
    uint8_t         *data;          /* RGB565数据指针 */
    uint16_t        x;              /* 图片左上角坐标x */
    uint16_t        y;              /* 图片左上角坐标y */
    uint16_t        width;          /* 图片宽度 */
    uint16_t        height;         /* 图片高度 */
    uint32_t        dataSize;       /* 数据大小 */
} ImageResource_t;

/* 文本资源描述结构 - 用于外部传入文本数据 */
typedef struct {
    const char *    TextStr;        /* 文本内容字符串 */
    uint16_t        x;              /* 文本左上角坐标x */
    uint16_t        y;              /* 文本左上角坐标y */
    uint16_t        scale;          /* 缩放比例 (100为原始大小) */    
    OSD_Align_t     align;          /* 水平对齐方式 */
} TextResource_t;

/* ==================== API声明 ==================== */

/**
 * @brief 初始化图层管理器
 * @param manager 图层管理器指针
 * @param screenWidth 屏幕宽度
 * @param screenHeight 屏幕高度
 * @return 0成功，-1失败
 */
int Layer_Init(LayerManager_t *manager, uint16_t screenWidth, uint16_t screenHeight);

/**
 * @brief 反初始化图层管理器 
 * @param manager 图层管理器指针
 * @return 0成功
 */
int Layer_UnInit(LayerManager_t *manager);

/**
 * @brief 设置图层管理器刷新LCD接口回调
 * @param manager 图层管理器指针 
 * @param pCallback LCD接口回调指针
 * @return 0成功
 */
int Layer_SetRefreshCallback(LayerManager_t *manager, REFRESH_CALLBACK pCallback);

/**
 * @brief 更新背景图层
 * @param manager 图层管理器
 * @param image 图片数据信息
 * @return 1有变化，0无变化
 */ 
int Layer_UpdateBackground(LayerManager_t *manager, const ImageResource_t *image);

/**
 * @brief 合成所有图层到framebuffer
 * @param manager 图层管理器
 * @return 0成功
 */
int Layer_Composite(LayerManager_t *manager);

/**
 * @brief 刷新脏矩形区域到LCD
 * @param manager 图层管理器
 * @return 0成功，-1失败
 */
int Layer_Refresh(LayerManager_t *manager);

/**
 * @brief 清除所有OSD (关闭使能并清空状态)
 * @param manager 图层管理器
 */
void Layer_ClearAll(LayerManager_t *manager);

/**
 * @brief 设置任意图片图层（背景、图片1、图片2等）
 * @param manager       图层管理器
 * @param imagegroup    图片资源数组（包含数据、位置、尺寸）
 * @param imagegrouplen 图片资源数组（包含数据、位置、尺寸）
 * @return 1=有变化，0=无变化
 */
int Layer_SetImageLayer(LayerManager_t *manager, const ImageResource_t *imagegroup, int imagegrouplen);

/**
 * @brief 设置任意文本OSD（支持位置、缩放、对齐、颜色、使能）
 * @param manager       图层管理器
 * @param textgroup     文本资源数组（包含数据、位置、尺寸）
 * @param textgrouplen  文本资源数组（包含数据、位置、尺寸）
 * @return 1=有变化，0=无变化
 */
int Layer_SetTextLayer(LayerManager_t *manager, const TextResource_t *textgroup, int textgrouplen);

/**
 * @brief 更新WiFi图标OSD
 * @param manager   图层管理器 
 * @param textgroup 文本资源数组（包含位置、尺寸）
 * @param nStatus  连接状态（0-其他）
 * @param nQuality 信号质量（0-100）
 * @return 1有变化，0无变化
 */
int Layer_SetWifiLayer(LayerManager_t *manager, const TextResource_t *textgroup, int nStatus, int nQuality);
#endif /* __LCD_LAYER_H__ */

