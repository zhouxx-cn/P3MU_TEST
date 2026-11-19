/**
********************************************************************************
 * @Copyright: 2022 RUISION CO.,LTD. All Rights Reserved.
 * @FilePath: \xrsframework\include\misp\isp_tuning_struct.h
 * @Author: HL.LIU
 * @Version: 
 * @Date: 2022-09-09 14:06:20
 * @Descripttion: 
********************************************************************************
*/
#pragma once

typedef unsigned char UINT8;
typedef unsigned short UINT16;
typedef unsigned int UINT32;
typedef signed int SINT32;
typedef float FLOAT32;

/* 白平衡模式 */
typedef enum
{
    ISP_TUNE_WB_MANUAL = 0, /* 手动模式 */
    ISP_TUNE_WB_AUTO,       /* 自动模式 */
} ISP_TUNE_AWB_MODE_E;

/* 白平衡增益 */
typedef struct
{
    UINT16 Rgain; /* Rgain：0-100，默认50 */
    UINT16 Bgain; /* Bgain：0-100，默认50 */

} ISP_TUNE_AWB_GAIN_S, *PISP_TUNE_AWB_GAIN_S;

/* 白平衡参数 */
typedef struct
{
    ISP_TUNE_AWB_MODE_E AwbMode;   /* 白平衡模式 */
    ISP_TUNE_AWB_GAIN_S AwbCustom; /* 手动白平衡的增益 */
} ISP_TUNE_AWB_PARAM_S, *PISP_TUNE_AWB_PARAM_S;

/* 曝光模式 */
typedef enum
{
    ISP_TUNE_AE_MANUAL = 0, /* 手动模式 */
    ISP_TUNE_AE_AUTO,       /* 自动模式 */
} ISP_TUNE_AE_MODE_E;

/* 防闪烁模式 */
typedef enum
{
    ISP_TUNE_FLICKER_OFF = 0, /* 关闭 */
    ISP_TUNE_FLICKER_50HZ,    /* 50HZ防闪烁 */
    ISP_TUNE_FLICKER_60HZ,    /* 60HZ防闪烁 */
} ISP_TUNE_FLICKER_MODE_E;

/* 曝光参数 */
typedef struct
{
    UINT32 StartX; /* 归一化值，0-10000，起点坐标X */
    UINT32 StartY; /* 归一化值，0-10000，起点坐标Y */
    UINT32 Width;  /* 归一化值，0-10000，宽度 */
    UINT32 Height; /* 归一化值，0-10000，高度 */
} ISP_TUNE_RECT_ANGLE_S, *PISP_TUNE_RECT_ANGLE_S;

typedef struct
{
    ISP_TUNE_FLICKER_MODE_E FlickerMode; /* 防闪烁模式，默认OFF*/
    ISP_TUNE_AE_MODE_E AeMode;           /* 曝光模式，默认自动 */
    FLOAT32 ExposureTime;                /* 曝光时间, 范围0.01-40.00ms */
    UINT8 ExposureGain;                  /* 曝光增益, 范围0-100， 默认50 */
    UINT8 ExposureSpeed;                 /* 曝光速度 范围0-100，默认50 */
    UINT8 ExposureLuma;                  /* 曝光亮度，范围0-100，默认50 */
    ISP_TUNE_RECT_ANGLE_S ExposureRect;  /* 曝光区域，{0, 0, 0, 0} | {0, 0, 10000, 10000}为全局*/
    UINT32 Resvd[8];                      /* 预留位 */
} ISP_TUNE_AE_PARAM_S, *PISP_TUNE_AE_PARAM_S;

/* 图像调节 */
typedef struct
{
    UINT8 Hue;        /* 色度，范围0-100，默认50 */
    UINT8 Saturation; /* 饱和度，范围0-100，默认50 */
    UINT8 Luma;       /* 亮度，范围0-100，默认50 */
    UINT8 Contrast;   /* 对比度，范围0-100，默认50 */
    UINT8 Sharpen;    /* 锐度，范围0-100，默认50 */
} ISP_TUNE_IMAGE_STYLE_PARAM_S, *PISP_TUNE_IMAGE_STYLE_PARAM_S;

/* 背光模式 */
typedef enum
{
    ISP_TUNE_EXT_AE_OFF = 0, /* 关闭 */
    ISP_TUNE_EXT_AE_BLC,     /* 背光补偿 */
    ISP_TUNE_EXT_AE_HLC,     /* 强光抑制 */
    ISP_TUNE_EXT_AE_HDR,     /* 宽动态 */
} ISP_TUNE_EXT_AE_MODE_E;

/* 曝光扩展参数 */
typedef struct
{
    ISP_TUNE_EXT_AE_MODE_E ExtAeMode; /* 曝光扩展模式 */
    UINT8 Level;                      /* 强度等级。范围0-100，默认50 */
} ISP_TUNE_EXT_AE_PARAM_S, *PISP_TUNE_EXT_AE_PARAM_S;

typedef enum
{
    ISP_TUNE_FALSE = 0,
    ISP_TUNE_TRUE = 1,
} ISP_TUNE_BOOL_E;

/* 降噪参数 */
typedef struct
{
    ISP_TUNE_BOOL_E Nr2dEnable; /* nr2d（空域降噪）的使能，0-关闭，1-开启，默认开启 */
    ISP_TUNE_BOOL_E Nr3dEnable; /* nr3d（时域降噪的使能，0-关闭，1-开启，默认开启 */
    UINT8 Nr2dLevel;            /* nr2d（空域降噪）的等级，范围0-100，默认50 */
    UINT8 Nr3dLevel;            /* nr3d（空域降噪）的等级，范围0-100，默认50 */
} ISP_TUNE_NR_PARAM_S, *PISP_TUNE_NR_PARAM_S;

/*日夜切换模式*/
typedef enum
{
    ISP_TUNE_DN_AUTO = 0,           //自动
    ISP_TUNE_DN_COLOR,              //彩色
    ISP_TUNE_DN_WHITE_BLACK,        //黑白
    ISP_TUNE_DN_COLOR_NIGHT_VISION, //彩色夜视
    ISP_TUNE_DN_IR_NIGHT_VISION,    //红外夜视
    ISP_TUNE_DN_BUTT,               
} ISP_TUNE_COLOR_BLACK_MODE_E;

/* 灯光模式 */
typedef enum
{
    ISP_TUNE_LAMP_AUTO = 0, /* 灯光自动模式 */
    ISP_TUNE_LAMP_MANUAL,   /* 灯光手动模式 */
    ISP_TUNE_LAMP_OFF,      /* 灯光关闭 */
} ISP_TUNE_LAMP_MODE_E;

/* 灯光类型 */
typedef enum
{
    ISP_TUNE_LAMP_IR = 0,     /* 红外灯 */
    ISP_TUNE_LAMP_WHITELIGHT, /* 白光灯 */
} ISP_TUNE_LAMP_TYPE_E;

/* 灯光参数 */
typedef struct
{
    ISP_TUNE_LAMP_MODE_E LampMode; /* 灯光模式 */
    ISP_TUNE_LAMP_TYPE_E LampType; /* 灯光类型 */
    UINT8 WhiteLightLevel;         /* 白光灯的等级，范围0-100，默认50 */
    UINT8 IRLightLevel;            /* 红外灯的等级，范围0-100，默认50 */
} ISP_TUNE_LAMP_PARAM_S, *PISP_TUNE_LAMP_PARAM_S;

/* 日夜切换/黑白彩色切换 */
typedef struct
{
    ISP_TUNE_COLOR_BLACK_MODE_E ColorBlackMode; /* 黑白彩色切换的模式 */
    UINT8 SensitivityLevel;                     /* 黑白彩色切换的灵敏度的等级。范围0-100，默认50 */
    UINT8 DelayTimeLevel;                       /* icr切换延时等级 范围0-100， 默认50 */
} ISP_TUNE_COLOR_BLACK_PARAM_S, *PISP_TUNE_COLOR_BLACK_PARAM_S;

/* 透雾模式 */
typedef enum
{
    ISP_TUNE_DEHAZE_OFF = 0, /* 透雾关闭 */
    ISP_TUNE_DEHAZE_ON,      /* 透雾开启 */
} ISP_TUNE_DEHAZE_MODE_E;

/* 透雾参数 */
typedef struct
{
    ISP_TUNE_DEHAZE_MODE_E DehazeMode; /* 透雾模式 */
    UINT8 DehazeLevel;                 /* 透雾的等级。范围是0-100，默认50 */
} ISP_TUNE_DEHAZE_PARAM_S, *PISP_TUNE_DEHAZE_PARAM_S;

typedef enum
{
    ISP_TUNE_ALG_ICR_DAY = 0, /* 日片 */
    ISP_TUNE_ALG_ICR_NIGHT,   /* 夜片 */
    ISP_TUNE_ALG_ICR_BUFF,
} ISP_TUNE_ALG_ICR_MODE_E;

/* ICR算法参数 */
typedef struct
{
    FLOAT32 R_G;          /* R分量比例 : R/G */
    FLOAT32 B_G;          /* B分量比例 : B/G */
    FLOAT32 IcrAlgLight;  /* icr算法实时亮度 */
    FLOAT32 IcrAlgCutThd; /* ict算法切换亮度阈值 */
    UINT8   IcrPredict;      /* icr预测状态 */
    UINT32 Resvd[4];       /* 预留位 */
} ISP_TUNE_ALG_ICR_PARAM_S, *PISP_TUNE_ALG_ICR_PARAM_S;

typedef struct
{
    UINT8 RealICR;                     /* ICR实际滤光片状态 0：彩色 1：黑白 */
    UINT32 AGain;                      /* 曝光增益AGain */
    UINT32 DGain;                      /* 曝光增益DGain */
    UINT32 ISPGain;                    /* 曝光增益ISPGain */
    UINT16 AeSht;                      /* 曝光时间 */
    UINT16 Pris;                       /* 光圈值 */
    UINT32 Exposure;                   /* 曝光值 */
    UINT32 WdrType;                    /* 当前宽动态类型,0:线性 1：真实宽动态 */
    ISP_TUNE_ALG_ICR_PARAM_S IcrParam; /* icr算法运行参数 */
    UINT32 Resvd[4];                   /* 预留位 */
} ISP_TUNE_ALG_INFOR_S, *PISP_TUNE_ALG_INFOR_S;

/* 驱动回调函数 */
typedef SINT32 (*pdn_cvt_callback)(UINT8 type);                /* 滤片控制回调接口 */
typedef SINT32 (*plamp_ctl_callback)(UINT8 type, UINT8 level); /* 灯光控制回调接口 */

/* 回调参数 */
typedef struct
{
    pdn_cvt_callback DayNightCvt_cb; /* ICR切换回调函数 */
    plamp_ctl_callback LampCtl_cb;   /* 白光灯和红外灯控制接口 */
} ISP_TUNE_DRV_CONTROL_CALLBACK_S, *PISP_TUNE_DRV_CONTROL_CALLBACK_S;