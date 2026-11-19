#ifndef __LIGHT_CTRL__
#define __LIGHT_CTRL__

#include "defs.h"

#define LIGHTCTL_DEBUG                  1
#define LIGHTCTL_PROCESS_SLEEP_MS       (100 * 1000)
#define LIGHT_BOARD_CHECK_DELAY_MS      (1 * 1000 * 1000)
#define LIGHT_ALARM_CHECK_DELAY_MS      (500 * 1000)
#define ADC_CHECK_CYCLE_FOR_COLOR_NIGHT (10 * 60 * 1000 * 1000)
#define COLOR_NIGHT_SLEEP_TIME          (1 * 1000 * 1000)           //睡眠时间
#define COLOR_DAY_TO_NIGHT_SWITCH_TIME  (4 * 1000 * 1000)           // IR检测时间,白天切晚上
#define COLOR_NIGHT_TO_DAY_SWITCH_TIME  (15 * 1000 * 1000)          // IR检测时间,晚上切白天

#define IR_DELAY_TIME_AFTER_SWITCH_TO_NIGHT (10 * 1000) //切换到夜晚，延时稳定后，再检测环境，避免马上又切为白天

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MID(a, min, max) (MAX(MIN((a), (max)), (min)))

#define PWM_MAX_DUTY                    100

// 新的灯板定制，使用自定义的灯板ID，按现有定制属性列举所有可能的类型，外部再映射
typedef enum LightBoardType
{
    LIGHT_BOARD_NONE = 0,
    LIGHT_BOARD_INFRARED_HIR_HIGH,              // 普通红外灯板，硬光敏，高电平亮灯
    LIGHT_BOARD_INFRARED_HIR_LOW,               // 普通红外灯板，硬光敏，低电平亮灯
    LIGHT_BOARD_INFRARED_SIR_HIGH,              // 普通红外灯板，软光敏，高电平亮灯
    LIGHT_BOARD_INFRARED_SIR_LOW,               // 普通红外灯板，软光敏，低电平亮灯

    LIGHT_BOARD_INFINITY_WL_HIR_HIGH,           // 无极白光灯板，硬光敏，高电平亮灯
    LIGHT_BOARD_INFINITY_WL_HIR_LOW,            // 无极白光灯板，硬光敏，低电平亮灯
    LIGHT_BOARD_INFINITY_WL_SIR_HIGH,           // 无极白光灯板，软光敏，高电平亮灯
    LIGHT_BOARD_INFINITY_WL_SIR_LOW,            // 无极白光灯板，软光敏，低电平亮灯

    LIGHT_BOARD_INFINITY_RL_HIR_HIGH,           // 无极红光灯板，软光敏，高电平亮灯
    LIGHT_BOARD_INFINITY_RL_HIR_LOW,            // 无极红光灯板，软光敏，高电平亮灯
    LIGHT_BOARD_INFINITY_RL_SIR_HIGH,           // 无极红光灯板，软光敏，高电平亮灯
    LIGHT_BOARD_INFINITY_RL_SIR_LOW,            // 无极红光灯板，软光敏，高电平亮灯

    LIGHT_BOARD_DOUBLE_LIGHT_HIR_HIGH,             // 双光灯板，硬光敏，高电平亮灯
    LIGHT_BOARD_DOUBLE_LIGHT_HIR_LOW,              // 双光灯板，硬光敏，低电平亮灯
    LIGHT_BOARD_DOUBLE_LIGHT_SIR_HIGH,             // 双光灯板，软光敏，高电平亮灯
    LIGHT_BOARD_DOUBLE_LIGHT_SIR_LOW,              // 双光灯板，软光敏，低电平亮灯

    LIGHT_BOARD_NUM,
} LIGHT_BOARD_TYPE_E;

typedef enum SensorType
{
    SENSOR_TYPE_Hi_JXF38P = 0,
} SENSOR_TYPE_E;

typedef enum IRStatus
{
    IR_STATUS_NIGHT = 0, // 检测为夜晚
    IR_STATUS_DAY        // 检测为白天
} IR_STATUS_E;

typedef enum LightCtrlMode
{
    LIGHTCTL_MODE_PASSIVITY = 0,              //自动(被动模式)
    LIGHTCTL_MODE_DAY = 1,                    //白天
    LIGHTCTL_MODE_NIGHT = 2,                  //夜晚
    LIGHTCTL_MODE_TIME = 3,                   //定时
    LIGHTCTL_MODE_INITIATIVE = 4,             //自动(主动模式)
    LIGHTCTL_MODE_COLOR_NIGHT = 5,            //夜视全彩
    LIGHTCTL_MODE_INTELLIGENT_NIGHT = 6,      //智能全彩
    LIGHTCTL_MODE_INTELLIGENT_REDRAY = 7,     //智能红外(强光抑制)
    LIGHTCTL_MODE_INTELLIGENT_WHITELIGHT = 8, //智能白光PWM
    LIGHTCTL_MODE_NORMAL_WHITELIGHT = 9       //普通白光
} LIGHTCTL_MODE_E;

// 灯板没有硬光敏则为软光敏
typedef enum IRDetectType
{
    IR_DETECT_HARDWARE = 0, /* 硬光敏 */
    IR_DETECT_SOFTWARE,     /* 软光敏 */
} IR_DETECT_TYPE_E;

typedef enum TrigLevel
{
    TRIG_LEVEL_LOW = 0,
    TRIG_LEVEL_HIGH,
} TRIG_LEVEL_E;

typedef struct LightBoard
{
    UINT8 id;                     /* 灯板ID */
    BOOL hasRedLight;             /* 有红灯 */
    BOOL hasWhiteLight;           /* 有白灯*/
    BOOL redLightSelfCtl;         /* 红灯自主控制 */
    IR_DETECT_TYPE_E eIRDetType;  /* 光敏检测类型 */
    TRIG_LEVEL_E eIRTrigLevel;    /* 光敏检测触发电平 */
    TRIG_LEVEL_E eLightTrigLevel; /* 亮灯触发电平 */
    UINT32 PWMFreqKHz;            /* PWM频率 */
} LIGHT_BOARD_S;

typedef enum LightOpMode
{
    LIGHT_OP_MODE_GPIO = 0,
    LIGHT_OP_MODE_PWM,
} LIGHT_OP_MODE_E;

typedef enum LightCtrlGpioSt
{
    GPIO_ST_LOW = 0,
    GPIO_ST_HIGH,
} LIGHTCTL_GPIO_ST_E;

typedef enum LightCtrlIRMode
{
    LIGHTCTL_IRCUT_DAY = 0,
    LIGHTCTL_IRCUT_NIGHT,
} LIGHTCTL_IRCUT_MODE_E;

typedef enum InfinityLightWorkMode
{
    LIGHT_WORKMODE_AUTO = 0,    /* 自动模式 */
    LIGHT_WORKMODE_TIME,        /* 时间控制 */
    LIGHT_WORKMODE_THR,         /* 手动阈值 */
} LIGHT_WORKMODE_E;

typedef struct ctrltime
{
    UINT32 startSec;    /* 开始时间 */
    UINT32 stopSec;     /* 结束时间 */
} CTRL_TIME_S;

typedef enum LightAlarmMode
{
    LIGHT_ALARM_MODE_OPEN = 0,  /* 常开 */
    LIGHT_ALARM_MODE_FLICK,     /* 闪烁 */
} LIGHT_ALARM_MODE_E;

/* PTZ状态 */
typedef enum LightPtzStatus
{
    LIGHT_PTZ_STOP = 0,    // PTZ静止状态
    LIGHT_PTZ_MOVING,      // PTZ运动状态
}eLightPtzStatus;

typedef struct LightIspInfo
{
    UINT32 gain;
    UINT32 luma;
    UINT32 ev;
    UINT16 rggain;
    UINT16 bggain;
}IR_ISP_INFO_S;

typedef void (*LightCtrl_SetLight)(LIGHT_OP_MODE_E mode, UINT32 duty, UINT32 freqKHz);
typedef void (*LightCtrl_GetIRStatus)(IR_STATUS_E *status);
typedef void (*LightCtrl_ISPCtrl)(LIGHTCTL_IRCUT_MODE_E mode);
typedef void (*LightCtrl_GetISPInfo)(IR_ISP_INFO_S *LightIrInfo);
typedef void (*LightCtrl_GetAIStatus)(BOOL lastTrig, BOOL *trig, BOOL rst);
typedef void (*LightCtrl_IntelligentLightTrigCb)();
typedef void (*LightCtrl_GetLightParam)(UINT8 *useFlag, UINT32 *lightBrightness,int reset);
typedef void (*LightCtrl_GetSlowFrame)(UINT32 *slowFrame);

typedef struct LightContext
{
    LIGHTCTL_MODE_E ctrlMode;              /* 灯板模式 */
    LIGHT_WORKMODE_E eWorkMode;            /* 灯板工作模式，自动，自定义，子模式 */
    UINT8 initiativeModeTh;                /* 普通红外，自动主动模式阈值 外部更新 */
    UINT8 intelligentWhiteStatue;          /* 标记是否为智能双光人形触发白光灯 */
    UINT32 colorNightContinueTimeMs;       /* 夜视全彩持续时间 */
    UINT8 colorNightMode;                  /* 标记是否在全彩白天晚上 */
    UINT8 dayNightSwitchTime;              /* 日夜切换时间(S) */
    LIGHTCTL_IRCUT_MODE_E eDayNightStatus; /* 当前日夜状态，外部更新 */
    CTRL_TIME_S ctlTime;                   /* 时间控制模式的开始、结束时间，多个模式共用 */
    BOOL usePtzCtrl;                       /* PTZ控制日夜切换 未测 */
    LIGHTCTL_IRCUT_MODE_E ptzCtrlStatus;   /* 当前PTZ控制日夜状态 */
    BOOL isSleepMode;                      /* 当前设备是否休眠*/
    BOOL isSensorSlowFPSMode;              /* 当前设备是否为Sensor低帧率模式*/
    BOOL isForceNightStutas;               /* 当前设备是否为强制夜晚模式 */
    eLightPtzStatus  ptzStatus;            /* PTZ状态 */

    // 灯光报警
    BOOL lightAlarmEn;                     /* 灯光报警是否开启 */
    BOOL lastLightAlarmStReal;             /* 灯光报警上次实际生效状态 */
    BOOL lightAlarmStReal;                 /* 灯光报警实际生效状态 */
    UINT32 lightAlarmStayTime;             /* 灯光报警生效时长 */
    LIGHT_ALARM_MODE_E lightAlarmMode;     /* 灯光报警模式 */
    UINT8 m_NightLedFlag;

    // 状态统计信息
    BOOL isWhiteLightOpen;         /* 白光灯状态 */
    BOOL isRedLightOpen;           /* 白光灯状态 */

    UINT8 whiteLightBrightness;    /* 白光灯亮度(实际亮度值) */
	UINT8 redLightBrightness;      /* 红光灯亮度(实际亮度值) */

    UINT32 lightAlarmCheckDelayMs; /* 灯光报警检测频率控制 */
    UINT32 lightBoardCheckDelayMs; /* 灯板检测频率控制 */

    /* 以下参数一般初始化设置后不再改变 */
    int BoardInfo;                             /*版型 */
    SENSOR_TYPE_E eSnrType;                    /* Sensor */
    UINT32 D2NFactor;                          /* 白切黑阈值 */
    UINT32 N2DFactor;                          /* 黑切白阈值 */
    LIGHT_BOARD_TYPE_E lightBoradID;           /* 灯板ID */
    LightCtrl_SetLight WhiteLightCtrl;         /* 白光灯控制 */
    LightCtrl_SetLight RedLightCtrl;           /* 红光灯控制 */
    LightCtrl_GetIRStatus GetHardwareIRStatus; /* 硬光敏状态 */
    LightCtrl_ISPCtrl IspCtrl;                 /* 日夜切换 */
    LightCtrl_GetISPInfo GetISPInfo;           /* 获取当前的环境信息 */
    LightCtrl_GetAIStatus GetAIStatus;         /* AI触发状态，智能双光使用 */
    LightCtrl_GetSlowFrame GetSlowFrame;
} LIGHT_CTX_S;

/**
 * @description: 灯光报警初始化
 * @param {LIGHT_CTX_S*} lightCtx 灯光控制上下文
 * @return {int} 0:成功 <0:失败
 */
int LightCtl_Init(LIGHT_CTX_S* lightCtx);

int LightCtl_Progress(LIGHT_CTX_S *lightCtx);

/**
 * @description: 获取灯板状态参数，黑光逻辑用
 * @paramn nVarWhiteGainFlag 当前无极白阈值是否为夜晚 1是；0不是
 * @return {int} 0:成功 <0:失败
 */
int LightCtl_BlackMode_GetParam(UINT32 *nVarWhiteGainFlag, UINT32 *nWhiteLightSelfCheck, UINT32 *nRedLightSelfCheck);

/**
 * @description: 设置灯板状态参数，黑光逻辑用
 * @paramn nIsForce2Day 强制出夜晚 1是；0不是
 * @return {int} 0:成功 <0:失败
 */
int LightCtl_BlackMode_SetParam(UINT32 nIsForce2Day);

/**
 * @description: 供产测工具辅助控制测试灯板使用，辅助控制白灯
 * @param {LIGHT_CTX_S*} lightCtx 灯板控制上下文
 * @param {UINT32} duty 占空比
 * @return {int} 0:成功 <0:失败
 */
int LightCtl_AsstCtlWhiteLight(LIGHT_CTX_S* lightCtx, UINT32 duty);

/**
 * @description: 供产测工具辅助控制测试灯板使用，辅助控制红灯
 * @param {LIGHT_CTX_S*} lightCtx 灯板控制上下文
 * @param {UINT32} duty 占空比
 * @return {int} 0:成功 <0:失败
 */
int LightCtl_AsstCtlRedLight(LIGHT_CTX_S* lightCtx, UINT32 duty);

#endif // __LIGHT_CTRL__
