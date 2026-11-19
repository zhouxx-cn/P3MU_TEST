/**
********************************************************************************
* @Copyright: 2022 RUISION CO.,LTD. All Rights Reserved.
* @FilePath: \configv:\filesystem\xrsframework\source\modules\ISP\visp.h
* @Author: HL.LIU
* @Version:
* @Date: 2022-09-24 11:43:01
* @Descripttion:
********************************************************************************
*/

#ifndef _VISP_H_
#define _VISP_H_

#ifdef __cplusplus
extern "C" {
#endif  //@__cplusplus
#include <linux/limits.h>

#include "vcomm.h"
#include "xrsdef.h"
#include "xrsstd.h"

// #define ISP_TUNING_API_INCLUDE

typedef enum { VISP_WDR_LEVEL_1 = 0, VISP_WDR_LEVEL_2, VISP_WDR_LEVEL_3, VISP_WDR_LEVEL_END } VISP_WDR_LEVEL_E;

/*
屏1 : boe，京东方
屏2 : auo，友达
...
*/
typedef enum { VISP_SCREEN_MODE_BOE = 0, VISP_SCREEN_MODE_AUO, VISP_SCREEN_MODE_MAX } VISP_SCREEN_MODE_E;

typedef struct {
    SINT16 bright;     /* 有效范围 : VIDEO_MIN_BRIGHT ~ VIDEO_MAX_BRIGHT */
    SINT16 Saturation; /* 有效范围 : VIDEO_MIN_SATURATION ~ VIDEO_MAX_SATURATION */
    SINT16 Contrast;   /* 有效范围 : VIDEO_MIN_CONTRAST ~ VIDEO_MAX_CONTRAST */
    SINT16 Hue;        /* 有效范围 : VIDEO_MIN_HUE ~ VIDEO_MAX_HUE */
    VISP_WDR_LEVEL_E WdrLevel;
    XRS_BOOL bFaceAe;              /* 是否开启faceAE功能 */
    VISP_SCREEN_MODE_E ScreenMode; /* 量产屏幕序号 */
} VISP_IMAGE_PARAM_S, *PVISP_IMAGE_PARAM_S;
#define VISP_IMAGE_PARAM_S_LEN sizeof(VISP_IMAGE_PARAM_S)

typedef enum { VISP_LOWLIGHT_UNRELIABLE = 0, VISP_LOWLIGHT_TRUE, VISP_LOWLIGHT_FALSE } VISP_LOWLIGHT_STATUS_E;

typedef enum {
    VISP_LOWLIGHT_SHAKE_FALSE = 0,
    VISP_LOWLIGHT_SHAKE_TRUE,
    VISP_LOWLIGHT_SHAKE_BUTT
} VISP_LOWLIGHT_SHAKE_E;

typedef enum { VISP_PIPE_INDEX_RGB = 0, VISP_PIPE_INDEX_IR, VISP_PIPE_INDEX_BUTT } VISP_PIPE_INDEX_E;

/* 白平衡模式 */
typedef enum {
    ISP_TUNE_WB_MANUAL = 0, /* 手动模式 */
    ISP_TUNE_WB_AUTO,       /* 自动模式 */
} ISP_TUNE_AWB_MODE_E;

/* 白平衡增益 */
typedef struct {
    UINT16 Rgain; /* Rgain：0-100，默认50 */
    UINT16 Bgain; /* Bgain：0-100，默认50 */

} ISP_TUNE_AWB_GAIN_S, *PISP_TUNE_AWB_GAIN_S;

/* 白平衡参数 */
typedef struct {
    ISP_TUNE_AWB_MODE_E AwbMode;   /* 白平衡模式 */
    ISP_TUNE_AWB_GAIN_S AwbCustom; /* 手动白平衡的增益 */
} ISP_TUNE_AWB_PARAM_S, *PISP_TUNE_AWB_PARAM_S;

/* 曝光模式 */
typedef enum {
    ISP_TUNE_AE_MANUAL = 0, /* 手动模式 */
    ISP_TUNE_AE_AUTO,       /* 自动模式 */
} ISP_TUNE_AE_MODE_E;

/* 防闪烁模式 */
typedef enum {
    ISP_TUNE_FLICKER_OFF = 0, /* 关闭 */
    ISP_TUNE_FLICKER_50HZ,    /* 50HZ防闪烁 */
    ISP_TUNE_FLICKER_60HZ,    /* 60HZ防闪烁 */
} ISP_TUNE_FLICKER_MODE_E;

/* 防闪烁模式 */
typedef enum {
    ISP_TUNE_FLICKER_STRENGTH_OFF = 0, /* 关闭 */
    ISP_TUNE_FLICKER_STRENGTH_LOW,     /* 弱闪烁 */
    ISP_TUNE_FLICKER_STRENGTH_HIGH,    /* 强闪烁，现在曝光时间，室外容易过曝 */
} ISP_TUNE_FLICKER_STRENGTH_E;

/* 曝光参数 */
typedef struct {
    UINT32 StartX; /* 归一化值，0-10000，起点坐标X */
    UINT32 StartY; /* 归一化值，0-10000，起点坐标Y */
    UINT32 Width;  /* 归一化值，0-10000，宽度 */
    UINT32 Height; /* 归一化值，0-10000，高度 */
} ISP_TUNE_RECT_ANGLE_S, *PISP_TUNE_RECT_ANGLE_S;

typedef struct {
    ISP_TUNE_FLICKER_MODE_E FlickerMode;         /* 防闪烁模式，默认OFF*/
    ISP_TUNE_AE_MODE_E AeMode;                   /* 曝光模式，默认自动 */
    FLOAT32 ExposureTime;                        /* 曝光时间, 范围0.01-40.00ms */
    UINT8 ExposureGain;                          /* 曝光增益, 范围0-100， 默认50 */
    UINT8 ExposureSpeed;                         /* 曝光速度 范围0-100，默认50 */
    UINT8 ExposureLuma;                          /* 曝光亮度，范围0-100，默认50 */
    ISP_TUNE_RECT_ANGLE_S ExposureRect;          /* 曝光区域，{0, 0, 0, 0} | {0, 0, 10000, 10000}为全局*/
    ISP_TUNE_FLICKER_STRENGTH_E FlickerStrength; /* 防闪烁强度，默认OFF, 曝光模式为自动时生效*/
    FLOAT32 MaxExposureTime;                     /* 最大曝光时间,单位 ms, 曝光模式为自动时生效*/
    FLOAT32 MinExposureTime;                     /* 最小曝光时间,单位 ms, 曝光模式为自动时生效 */
    UINT32 MaxSysGain;                           /* 最大sysgain, 配置为0不改动 */
    UINT32 MaxIspDGain;                          /* 最大ispdgain, 配置为0不改动 */
    UINT32 Resvd[4];                             /* 预留位 */
} ISP_TUNE_AE_PARAM_S, *PISP_TUNE_AE_PARAM_S;

typedef struct {
    ISP_TUNE_AE_MODE_E AeMode; /* 曝光模式，默认自动 */
    UINT32 ExposureTime;       /* 曝光时间 */
    UINT32 AGain;              /* 曝光增益, AGAIN */
    UINT32 DGain;              /* 曝光增益, DGAIN */
    UINT32 IspDGain;           /* 曝光增益, ISPDGAIN */
    UINT8 Resvd[44];           /* 预留位 */
} ISP_TUNE_AE_PARAM_EX_S, *PISP_TUNE_AE_PARAM_EX_S;
ASSERT_SIZEOF_TYPEDEF_STRUCT(ISP_TUNE_AE_PARAM_EX_S, 64);

/* 图像调节 */
typedef struct {
    UINT8 Hue;        /* 色度，范围0-100，默认50 */
    UINT8 Saturation; /* 饱和度，范围0-100，默认50 */
    UINT8 Luma;       /* 亮度，范围0-100，默认50 */
    UINT8 Contrast;   /* 对比度，范围0-100，默认50 */
    UINT8 Sharpen;    /* 锐度，范围0-100，默认50 */
} ISP_TUNE_IMAGE_STYLE_PARAM_S, *PISP_TUNE_IMAGE_STYLE_PARAM_S;

/* 背光模式 */
typedef enum {
    ISP_TUNE_EXT_AE_OFF = 0, /* 关闭 */
    ISP_TUNE_EXT_AE_BLC,     /* 背光补偿 */
    ISP_TUNE_EXT_AE_HLC,     /* 强光抑制 */
    ISP_TUNE_EXT_AE_HDR,     /* 宽动态 */
} ISP_TUNE_EXT_AE_MODE_E;

/* 曝光扩展参数 */
typedef struct {
    ISP_TUNE_EXT_AE_MODE_E ExtAeMode; /* 曝光扩展模式 */
    UINT8 Level;                      /* 强度等级。范围0-100，默认50 */
} ISP_TUNE_EXT_AE_PARAM_S, *PISP_TUNE_EXT_AE_PARAM_S;

typedef enum {
    ISP_TUNE_FALSE = 0,
    ISP_TUNE_TRUE = 1,
} ISP_TUNE_BOOL_E;

/* 降噪参数 */
typedef struct {
    ISP_TUNE_BOOL_E Nr2dEnable; /* nr2d（空域降噪）的使能，0-关闭，1-开启，默认开启 */
    ISP_TUNE_BOOL_E Nr3dEnable; /* nr3d（时域降噪的使能，0-关闭，1-开启，默认开启 */
    UINT8 Nr2dLevel;            /* nr2d（空域降噪）的等级，范围0-100，默认50 */
    UINT8 Nr3dLevel;            /* nr3d（空域降噪）的等级，范围0-100，默认50 */
} ISP_TUNE_NR_PARAM_S, *PISP_TUNE_NR_PARAM_S;

/*日夜切换模式*/
typedef enum {
    ISP_TUNE_DN_AUTO = 0,            // 自动
    ISP_TUNE_DN_COLOR,               // 彩色
    ISP_TUNE_DN_WHITE_BLACK,         // 黑白
    ISP_TUNE_DN_COLOR_NIGHT_VISION,  // 彩色夜视
    ISP_TUNE_DN_IR_NIGHT_VISION,     // 红外夜视
    ISP_TUNE_DN_BUTT,
} ISP_TUNE_COLOR_BLACK_MODE_E;

/* 灯光模式 */
typedef enum {
    ISP_TUNE_LAMP_AUTO = 0, /* 灯光自动模式 */
    ISP_TUNE_LAMP_MANUAL,   /* 灯光手动模式 */
    ISP_TUNE_LAMP_OFF,      /* 灯光关闭 */
} ISP_TUNE_LAMP_MODE_E;

/* 灯光类型 */
typedef enum {
    ISP_TUNE_LAMP_IR = 0,     /* 红外灯 */
    ISP_TUNE_LAMP_WHITELIGHT, /* 白光灯 */
} ISP_TUNE_LAMP_TYPE_E;

/* 灯光参数 */
typedef struct {
    ISP_TUNE_LAMP_MODE_E LampMode; /* 灯光模式 */
    ISP_TUNE_LAMP_TYPE_E LampType; /* 灯光类型 */
    UINT8 WhiteLightLevel;         /* 白光灯的等级，范围0-100，默认50 */
    UINT8 IRLightLevel;            /* 红外灯的等级，范围0-100，默认50 */
} ISP_TUNE_LAMP_PARAM_S, *PISP_TUNE_LAMP_PARAM_S;

/* 日夜切换/黑白彩色切换 */
typedef struct {
    ISP_TUNE_COLOR_BLACK_MODE_E ColorBlackMode; /* 黑白彩色切换的模式 */
    UINT8 SensitivityLevel;                     /* 黑白彩色切换的灵敏度的等级。范围0-100，默认50 */
    UINT8 DelayTimeLevel;                       /* icr切换延时等级 范围0-100， 默认50 */
} ISP_TUNE_COLOR_BLACK_PARAM_S, *PISP_TUNE_COLOR_BLACK_PARAM_S;

/* 透雾模式 */
typedef enum {
    ISP_TUNE_DEHAZE_OFF = 0, /* 透雾关闭 */
    ISP_TUNE_DEHAZE_ON,      /* 透雾开启 */
} ISP_TUNE_DEHAZE_MODE_E;

/* 透雾参数 */
typedef struct {
    ISP_TUNE_DEHAZE_MODE_E DehazeMode; /* 透雾模式 */
    UINT8 DehazeLevel;                 /* 透雾的等级。范围是0-100，默认50 */
} ISP_TUNE_DEHAZE_PARAM_S, *PISP_TUNE_DEHAZE_PARAM_S;

typedef enum {
    ISP_TUNE_ALG_ICR_DAY = 0, /* 日片 */
    ISP_TUNE_ALG_ICR_NIGHT,   /* 夜片 */
    ISP_TUNE_ALG_ICR_BUFF,
} ISP_TUNE_ALG_ICR_MODE_E;

/* ICR算法参数 */
typedef struct {
    FLOAT32 R_G;          /* R分量比例 : R/G */
    FLOAT32 B_G;          /* B分量比例 : B/G */
    FLOAT32 IcrAlgLight;  /* icr算法实时亮度 */
    FLOAT32 IcrAlgCutThd; /* ict算法切换亮度阈值 */
    UINT8 IcrPredict;     /* icr预测状态 */
    UINT32 Resvd[4];      /* 预留位 */
} ISP_TUNE_ALG_ICR_PARAM_S, *PISP_TUNE_ALG_ICR_PARAM_S;

typedef struct {
    UINT8 RealICR;                     /* ICR实际滤光片状态 0：彩色 1：黑白 */
    UINT32 AGain;                      /* 曝光增益AGain */
    UINT32 DGain;                      /* 曝光增益DGain */
    UINT32 ISPGain;                    /* 曝光增益ISPGain */
    UINT32 AeSht;                      /* 曝光时间 */
    UINT32 Pris;                       /* 光圈值 */
    UINT32 Exposure;                   /* 曝光值 */
    UINT32 WdrType;                    /* 当前宽动态类型,0:线性 1：真实宽动态 */
    ISP_TUNE_ALG_ICR_PARAM_S IcrParam; /* icr算法运行参数 */
    UINT32 Target;                     /* 目标亮度 */
    UINT32 Stable;                     /* AE收敛状态 */
    UINT32 ISO;                        /* ISO */
    UINT32 Resvd[2];                   /* 预留位 */
} ISP_TUNE_ALG_INFOR_S, *PISP_TUNE_ALG_INFOR_S;

typedef struct {
    UINT16 RGain;
    UINT16 GrGain;
    UINT16 GbGain;
    UINT16 BGain;
    UINT16 Satu;
    UINT32 Resvd[4]; /* 预留位 */
} ISP_TUNE_AWB_STATIS_INFO_S, *PISP_TUNE_AWB_STATIS_INFO_S;

#define ISP_TUNE_AWB_ZONE_NUM (32 * 32)
typedef struct {
    UINT16 GlobalR;
    UINT16 GlobalG;
    UINT16 GlobalB;
    UINT16 ZoneAvgR[ISP_TUNE_AWB_ZONE_NUM];
    UINT16 ZoneAvgG[ISP_TUNE_AWB_ZONE_NUM];
    UINT16 ZoneAvgB[ISP_TUNE_AWB_ZONE_NUM];
} ISP_TUNE_AWB_ZONE_STATIS_INFO_S, *PISP_TUNE_AWB_ZONE_STATIS_INFO_S;

typedef struct {
    UINT32 IspOsdSize; /**< osd占用大小：开启直通模式时需要设置。非直通模式下，此参数不生效*/
    UINT32 IpuOsdSize; /**< osd占用大小*/
    UINT32 MjpegSize;  /**< mjpeg编码流占用大小*/
    UINT32 SnrDefMode; /**< sensor输出帧率模式(0-3),映射最大帧率：0->30Fps;1-25fps;2->20fps;3-15fps*/
    UINT32 Fs2MemSize; /**< 算法流framesource申请内存总大小(depth * width * height * 3 /
                          2)，其中宽和高要求16对齐，与MJPEG流buf共享，及设置次通道内存池大小*/
    UINT32 WdrEnable;  /**< WDR使能开关：0-disable；1-enable*/
    UINT32 Fs1MemSize; /**< 次码流framesource申请内存总大小(depth * width * height * 3 /
                          2)，宽和高要求16对齐，用来设置次通道内存池大小*/
    UINT32 Fs0MemSize; /**< 主码流framesource申请内存总大小(MaxWdith * memline * 3 /
                          2),其中MaxWdith需256向上对齐，用来设置次通道内存池大小*/
    UINT32
    Enc0StreamSize; /**< 主码流stream
                       buf大小(单位字节)，按最大分辨率来设置：5MP和4MP均建议768KB；4MP时设置为512KB，需项目实测是否有编码内存越界问题*/
    UINT32
    Enc1StreamSize;  /**< 子码流stream
                        buf大小(单位字节)，按最大分辨率及sensor规格来设置：D1分辨率下，若sensor规格为5MP，建议设置为512KB,减少需要项目实测验证；
                                                             若sensor规格为4MP，建议设置为128KB*/
    UINT32 Resvd[3]; /* 预留位 */
} ISP_JZ_PLAT_CFG_S, *PISP_JZ_PLAT_CFG_S;

typedef struct {
    union {
        ISP_JZ_PLAT_CFG_S JzPlat; /*!< 君正平台配置*/
    };
    union {
        UINT8 Resvd[128]; /*!< 预留位*/
    };
} ISP_PLAT_CFG_S, *PISP_PLAT_CFG_S;

typedef struct {
    UINT8 weight[15][17]; /**< 各区域权重信息 [0 ~ 15]*/
} ISP_AF_WEIGHT_S;

typedef struct {
    UINT8 weight[15][17]; /**< 各区域权重信息 [0 ~ 15]*/
} ISP_AE_WEIGHT_S;

typedef struct {
    UINT32 enable;            /**< AE权重使能*/
    ISP_AE_WEIGHT_S aeWeight; /**< 各区域权重信息 [0 ~ 15]*/
    UINT16 HistRatio;         /* 感兴趣区域权重 */
    UINT8 HistOffset;         /* 感兴趣区域影响程度 */
} ISP_AE_WEIGHT_ATTR_S;

typedef struct {
    UINT32 enable;      /**目标亮度表使能*/
    UINT8 compensation; /**目标亮度*/
} ISP_ATLIST_S;

typedef struct {
    UINT32 Fps;         /* 当前场景图像参数推荐的慢快门帧率 */
    UINT32 Gop;         /* 当前场景图像参数推荐的I帧间隔 */
    FLOAT32 AeTime;     /* 微秒 */
    UINT32 MaxSysGain;  /* 总增益最大值 */
    UINT32 MaxIspDGain; /* ISP数字增益最大值 */
} ISP_SCENE_FPS_S;

/* 驱动回调函数 */
typedef SINT32 (*pdn_cvt_callback)(UINT8 type);                /* 滤片控制回调接口 */
typedef SINT32 (*plamp_ctl_callback)(UINT8 type, UINT8 level); /* 灯光控制回调接口 */

/* 回调参数 */
typedef struct {
    pdn_cvt_callback DayNightCvt_cb; /* ICR切换回调函数 */
    plamp_ctl_callback LampCtl_cb;   /* 白光灯和红外灯控制接口 */
} ISP_TUNE_DRV_CONTROL_CALLBACK_S, *PISP_TUNE_DRV_CONTROL_CALLBACK_S;

typedef enum {
    VISP_AE_ALG_RK,
    VISP_AE_ALG_HISI,
    VISP_AE_ALG_MEGAE,
    VISP_AE_ALG_BUTT,
} VISP_AE_ALG_E;

#define VISP_LIGHT_LEVEL_MIN 0
#define VISP_LIGHT_LEVEL_MAx 100

typedef enum {
    VISP_ERROR_START = COMP_ERROR_START, /* 用户模块起始错误号 */
    VISP_ERROR_REINIT,                   /* 重复初始化 */
    VISP_ERROR_NOINIT,                   /* ISP未初始化 */
    VISP_ERROR_MODULEOPEN,               /* ISP模块打开错误 */
    VISP_ERROR_MODULECLOSE,              /* ISP模块关闭错误 */
    VISP_ERROR_SYSTEMINIT,               /* ISP系统初始化错误 */
    VISP_ERROR_INTERNAL,                 /* ISP内部错误，未细分的错误类型 */
    VISP_ERROR_SETSCENEMODE,             /* 设置黑白彩色错误 */
    VISP_ERROR_SETSENSORFRM,             /* 设置sensor帧率错误 */
    VISP_ERROR_PATHTOOLONG,              /* 路径太长错误 */
    VISP_ERROR_LOADBIN,                  /* 加载ISP图像bin文件错误 */
    VISP_ERROR_HVFLIP,                   /* 设置镜像错误 */
    VISP_ERROR_SETDEHAZE,                /* 设置去雾错误  */
    VISP_ERROR_SETNR,                    /* 设置降噪错误  */
    VISP_ERROR_SETBLC,                   /* 设置背光错误  */
    VISP_ERROR_SETHLC,                   /* 设置强光抑制错误  */
    VISP_ERROR_AE = 100,                 /* AE功能错误 */
    VISP_ERROR_AWB = 110,                /* AWB功能错误 */
    VISP_ERROR_AF = 120,                 /* AF功能错误 */
    VISP_ERROR_WDR = 130,                /* WDR功能错误 */
    VISP_ERROR_SCENE_PARAM = 200,        /* 图像参数错误 */
} VISP_ERROR_E;
#define VISP_ERRNO(err) ERRNO(err, COMP_VIDEOISP)

#define VISP_ERR(str, args...) TRACE_ERR(COMP_VIDEOISP, str, ##args)
#define VISP_WARN(str, args...) TRACE_WARN(COMP_VIDEOISP, str, ##args)
#define VISP_FLOW(str, args...) TRACE_FLOW(COMP_VIDEOISP, str, ##args)
#define VISP_INFO(str, args...) TRACE_INFO(COMP_VIDEOISP, str, ##args)
#define VISP_DBG0(str, args...) TRACE_DBG0(COMP_VIDEOISP, str, ##args)

/*
    bRgbLowLight :  rgb路检测到当前是否低照环境
    bIrLowLight  :  ir路检测到当前是否低照环境
*/
typedef SINT32 (*Visp_LowLight_Callback)(VISP_LOWLIGHT_STATUS_E bRgbLowLight, VISP_LOWLIGHT_STATUS_E bIrLowLight);

/* 灯光控制回调接口 */
typedef SINT32 (*Visp_LightControl_Callback)(VISP_PIPE_INDEX_E Idx, SINT32 Luminance);

typedef struct {
    Visp_LowLight_Callback LowLight_CB;               /* 低照环境回调函数 */
    Visp_LightControl_Callback WhiteLight_Control_CB; /* 白光灯和红外灯控制接口 */
} VISP_CONTROL_CALLBACK_S;

#define VISP_SOFTWARE_VERSION_LEN 32
#define VISP_LUA_FILE_VERSION_LEN 64
typedef struct {
    char Software_Version[VISP_SOFTWARE_VERSION_LEN];
    char LuaFIle_Version[VISP_LUA_FILE_VERSION_LEN];
} VISP_VERSION_INFO_S, *PVISP_VERSION_INFO_S;
#define VISP_VERSION_INFO_S_LEN sizeof(VISP_VERSION_INFO_S);

typedef struct {
    UINT16 SnsNum;                                   /* sensor数目 */
    VISP_SENSOR_CONF_S SnsConf[MAX_VISP_SENSOR_NUM]; /* sensor配置信息 */
    char SceneDir[VIDEO_FILENAME_MAX_LEN];           /* 场景配置文件路径 */
} VISP_CONF_S;

typedef struct {
    UINT16 Width;      /* 通道图像宽度 */
    UINT16 Height;     /* 通道图像高度 */
    XRS_BOOL EnAi3DNR; /* 使能AI3DNR 610暂不支持*/
    XRS_BOOL EnAiBNR;  /* 使能AIBNR */
} VISP_AIISP_ATTR_S;

/******************************************************************************
 * 函数功能: 查询ISP模块的软件版本号和LUA文件的版本号
 * 输入参数: PVISP_VERSION_INFO_S: 版本信息
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Visp_Get_VersionInfo(PVISP_VERSION_INFO_S pVersion);

/******************************************************************************
 * 函数功能: 打开模块,分配系统资源,模块初始化
 * 输入参数: MaxChn: 当前视频最大输入数
 *           ChipConf: 芯片配置信息
 *           LowLight_Cb: 模块检测到低照场景后回调主控
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Visp_Open(SINT32 MaxChn, VIDEO_CHIP_CONF_S ChipConf, VISP_CONTROL_CALLBACK_S ControlCb);

/******************************************************************************
 * 函数功能: 关闭模块,释放系统资源
 * 输入参数:
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Visp_Close(SINT32 handle);

/******************************************************************************
 * 函数功能: 打开模块,分配系统资源,模块初始化
 * 输入参数: MaxChn: 当前视频最大输入数
 *           Conf: ISP配置信息
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Visp_Start(SINT32 handle, VISP_CONF_S Conf);

/******************************************************************************
 * 函数功能: 停止ISP
 * 输入参数:
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Visp_Stop(SINT32 handle);

/******************************************************************************
 * 函数功能: 复位sensor
 * 输入参数: handle: 无效
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Visp_Reset_Sensor(SINT32 handle);

/*
以下接口暂未使用
*/
/******************************************************************************
 * 函数功能: 主控向模块更新白光灯的设置等级
 * 输入参数: LightLevel: 白光灯设置等级， ?[[VISP_LIGHT_LEVEL_MIN, VISP_LIGHT_LEVEL_MAX]
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Visp_Update_LightLevel(UINT32 LightLevel);

/******************************************************************************
 * 函数功能: 更新检测到的人脸目标情况
 * 输入参数: Idx: sensor序号
 *           TargetNum : 人脸目标数目
 *           pObjList  : 人脸状态列表
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Visp_Report_TargetObject(SINT32 handle, VISP_PIPE_INDEX_E Idx, SINT32 TargetNum,
                                PVISP_TARGET_OBJ_INFO_S pObjList);

/******************************************************************************
 * 函数功能: 设置ISP模块的图像参数
 * 输入参数: pImageParam: 图像参数
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Visp_Set_ImageParam(SINT32 handle, PVISP_IMAGE_PARAM_S pImageParam);

/******************************************************************************
 * 函数功能: 获取ISP模块的图像参数
 * 输入参数: pImageParam: 图像参数
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Visp_Get_ImageParam(SINT32 handle, PVISP_IMAGE_PARAM_S pImageParam);

/******************************************************************************
 * 函数功能: 设置双路标定参数
 * 输入参数: 双路标定参数
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Visp_Update_CalibrationParam(SINT32 handle, float LeftRight_K, float LeftRight_B, float TopBottom_K,
                                    float TopBottom_B);
SINT32 Visp_Update_DetectParam(SINT32 handle, SINT32 ViPipe, PVISP_OBJ_DETECT_PARAM_S pDetectParam);
SINT32 Visp_IoCtrl(SINT32 handle, SINT32 Cmd, SINT32 Chn, SINT32 *pParam, SINT32 ParamLen);

/******************************************************************************
 * 函数功能: 设置AE ALG
 * 输入参数: AE ALG 类型
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Visp_SetAE_ALG(VISP_AE_ALG_E Ae_Alg);

/******************************************************************************
 * 函数功能: 获取 AE ALG
 * 输入参数: 无
 * 输出参数: 无
 * 返回值  : AE ALG 类型
 *****************************************************************************/
VISP_AE_ALG_E Visp_GetAE_ALG();

/******************************************************************************
 * 函数功能: isp初始化
 * 输入参数: path: isp配置文件路径
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Visp_TuneManagerInit(const char *path);

/******************************************************************************
 * 函数功能: isp退出
 * 输入参数: 无
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Visp_TuneManagerRelease(void);

/******************************************************************************
 * 函数功能: icr及lamp驱动回调函数注册
 * 输入参数: controlCb: 回调函数
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Visp_TuneIcrCallbackRegist(ISP_TUNE_DRV_CONTROL_CALLBACK_S controlCb);

/******************************************************************************
 * 函数功能: 设置图像风格，如图像亮度，对比度，锐度等
 * 输入参数: channel: 通道号; pImageStyleParam: 参数结构体指针
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Visp_Set_TuneImageStyle(SINT32 channel, PISP_TUNE_IMAGE_STYLE_PARAM_S pImageStyleParam);

/******************************************************************************
 * 函数功能: 设置白平衡参数
 * 输入参数: channel: 通道号; pAwbParam: 参数结构体指针
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Visp_Set_TuneAutoWhiteBalance(SINT32 channel, PISP_TUNE_AWB_PARAM_S pAwbParam);

/******************************************************************************
 * 函数功能: 设置曝光参数
 * 输入参数: channel: 通道号; pAeParam: 参数结构体指针
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Visp_Set_TuneAutoExposure(SINT32 channel, PISP_TUNE_AE_PARAM_S pAeParam);

/******************************************************************************
 * 函数功能: 设置曝光参数，按实际生效值
 * 输入参数: channel: 通道号; pAeParam: 参数结构体指针
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Visp_Set_TuneExposureEx(SINT32 channel, PISP_TUNE_AE_PARAM_EX_S pAeParam);

/******************************************************************************
 * 函数功能: 设置背光补偿、宽动态、强光抑制相关参数
 * 输入参数: channel: 通道号; pExtAeParam: 参数结构体指针
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Visp_Set_TuneExtendAutoExposure(SINT32 channel, PISP_TUNE_EXT_AE_PARAM_S pExtAeParam);

/******************************************************************************
 * 函数功能: 设置降噪相关参数
 * 输入参数: channel: 通道号; pNrParam: 参数结构体指针
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Visp_Set_TuneNoiseReduction(SINT32 channel, PISP_TUNE_NR_PARAM_S pNrParam);

/******************************************************************************
 * 函数功能: 设置透雾相关参数
 * 输入参数: channel: 通道号; pDehazeParam: 参数结构体指针
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Visp_Set_TuneDehaze(SINT32 channel, PISP_TUNE_DEHAZE_PARAM_S pDehazeParam);

/******************************************************************************
 * 函数功能: 设置日夜切换相关参数
 * 输入参数: channel: 通道号; pColorBlackParam: 参数结构体指针
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Visp_Set_TuneColorBlackTransform(SINT32 channel, PISP_TUNE_COLOR_BLACK_PARAM_S pColorBlackParam);

/******************************************************************************
 * 函数功能: 设置灯光相关参数
 * 输入参数: channel: 通道号; pLampParam: 参数结构体指针
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Visp_Set_TuneLamp(SINT32 channel, PISP_TUNE_LAMP_PARAM_S pLampParam);

/******************************************************************************
 * 函数功能: 获取ISP算法相关参数
 * 输入参数: channel: 通道号; pParam: 参数结构体指针
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Visp_Get_TuneCurrInfo(SINT32 channel, PISP_TUNE_ALG_INFOR_S pParam);

/******************************************************************************
 * 函数功能: 获取AWB统计信息
 * 输入参数: channel: 通道号; pParam: 参数结构体指针
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Visp_Get_AWBStatisInfo(SINT32 channel, PISP_TUNE_AWB_STATIS_INFO_S pParam);

/******************************************************************************
 * 函数功能: 获取AWB统计信息（分块）
 * 输入参数: channel: 通道号; pParam: 参数结构体指针
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Visp_Get_AWBZoneStatisInfo(SINT32 channel, PISP_TUNE_AWB_ZONE_STATIS_INFO_S pParam);

/******************************************************************************
 * 函数功能: 设置图像调优配置参数文件
 * 输入参数: channel: 通道号; pFileName: 配置文件路径名称，< 64字节
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Visp_Set_TuneConfigFile(SINT32 channel, const char *pFileName);

/******************************************************************************
 * 函数功能: 设置图像场景
 * 输入参数: channel: 通道号; Sence: 图像场景
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Visp_Set_SwitchScene(SINT32 channel, VISP_SCENE_MODE_E Sence);

/******************************************************************************
 * 函数功能: 设置sensor的翻转镜像
 * 输入参数: channel: 通道号; HVFlipMode: 镜像模式
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Visp_Set_TuneHVFLIP(SINT32 channel, XRS_HVFLIP_E HVFlipMode);

/******************************************************************************
 * 函数功能: 设置平台配置信息，需在调用Visp_Open接口之前使用
 * 输入参数: channel: 通道号; allocInfo: 分配内存信息：暂包含osd和mjpeg
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Visp_Set_PlatCfgInfo(SINT32 channel, PISP_PLAT_CFG_S pPlatCfg);

SINT32 Visp_Set_WdrEnable(SINT32 channel, int WdrEnable);

/******************************************************************************
 * 函数功能: 设置AF权重配置
 * 输入参数: channel: 通道号; pAfWeight: 权重信息
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Visp_Set_AFWeight(SINT32 channel, ISP_AF_WEIGHT_S *pAfWeight);

/******************************************************************************
 * 函数功能: 获取AF权重信息
 * 输入参数: channel: 通道号;
 * 输出参数: pAfWeight: 权重信息
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Visp_Get_AFWeight(SINT32 channel, ISP_AF_WEIGHT_S *pAfWeight);

/******************************************************************************
 * 函数功能: 获取AF统计值
 * 输入参数: channel: 通道号;
 * 输出参数: pAFMetrics：平台返回的统计值
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Visp_Get_AFMetrics(SINT32 channel, UINT32 *pAFMetrics);

/******************************************************************************
 * 函数功能: 获取sensor寄存器值
 * 输入参数: channel: 通道号; addr: reg地址
 * 输出参数: pValue：reg值
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Visp_Get_SensorRegister(SINT32 channel, UINT32 addr, UINT32 *pValue);

/******************************************************************************
 * 函数功能: 设置sensor寄存器值
 * 输入参数: channel: 通道号; addr: reg地址； value：reg值
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Visp_Set_SensorRegister(SINT32 channel, UINT32 addr, UINT32 value);

/******************************************************************************
 * 函数功能: 设置AE权重配置
 * 输入参数: channel: 通道号;
 * 输出参数: pAEWeight: 权重信息
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Visp_Set_AEWeight(SINT32 channel, ISP_AE_WEIGHT_ATTR_S *pAEWeight);

/******************************************************************************
 * 函数功能: 获取AE权重信息
 * 输入参数: channel: 通道号; pAEWeight: 权重信息
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Visp_Get_AEWeight(SINT32 channel, ISP_AE_WEIGHT_ATTR_S *pAEWeight);

/******************************************************************************
 * 函数功能: 设置目标亮度信息
 * 输入参数: channel: 通道号; pAtList: 目标亮度信息
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Visp_Set_AtList(SINT32 channel, ISP_ATLIST_S *pAtList);

/******************************************************************************
 * 函数功能: 获取目标亮度信息
 * 输入参数: channel: 通道号;
 * 输出参数: pAtList: 目标亮度信息
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Visp_Get_AtList(SINT32 channel, ISP_ATLIST_S *pAtList);

/******************************************************************************
 * 函数功能: 设置中心点放大倍数
 * 输入参数: channel: 通道号; widthT: 对比原图宽得到缩放比例；heightT：对比原图高得到缩放比例
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Visp_Set_CenterZoomTimes(SINT32 channel, FLOAT64 *widthT, FLOAT64 *heightT);

/******************************************************************************
 * 函数功能: 获取中心点放大倍数
 * 输入参数: channel: 通道号;
 * 输出参数: widthT: 对比原图宽得到缩放比例；heightT：对比原图高得到缩放比例
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Visp_Get_CenterZoomTimes(SINT32 channel, FLOAT64 *widthT, FLOAT64 *heightT);

SINT32 Visp_Start_SceneAuto(SINT32 handle, const char *dir, VISP_SCENE_MODE_E mode);
SINT32 Visp_Pause_SceneAuto(SINT32 handle, XRS_BOOL pause);
SINT32 Visp_Stop_SceneAuto(SINT32 handle);
SINT32 Visp_Get_SceneFps(SINT32 channel, ISP_SCENE_FPS_S *SceneFps);
/******************************************************************************
 * 函数功能: 设置图像场景
 * 输入参数: channel: 通道号; mode: 图像场景
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Visp_Switch_SceneMode(SINT32 channel, VISP_SCENE_MODE_E mode);

SINT32 Visp_Start_TuneServe(SINT32 handle);
SINT32 Visp_Stop_TuneServe(SINT32 handle);

SINT32 Visp_Start_AiIsp(SINT32 handle, SINT32 Chn, VISP_AIISP_ATTR_S *pAiIspAttr);
SINT32 Visp_Stop_AiIsp(SINT32 handle, SINT32 Chn);

#ifdef __cplusplus
}
#endif  //@__cplusplus
#endif  /* _VISP_H_ */
