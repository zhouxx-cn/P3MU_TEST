/*
 * @Author: Sun Yuhui
 * @Date: 2022-07-13 10:52:15
 * @LastEditTime: 2022-08-12 16:48:15
 * @LastEditors: Sun Yuhui
 * @Description:
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

#include "lightctrl.h"
#include "Common.h"
#include "EncoderLib.h"

#define XLOG_ERR(format...)            LOG_E(LOG_MODULE_LIGHT, format)
#define XLOG_PURE_ERR(format...)       LOG_E(LOG_MODULE_LIGHT, format)
#define XLOG_WARN(format...)           LOG_W(LOG_MODULE_LIGHT, format)
#define XLOG_INFO(format...)           LOG_I(LOG_MODULE_LIGHT, format)
#define XLOG_DEBUG(format...)          LOG_D(LOG_MODULE_LIGHT, format)

/*
 * 代码维护注意
 *   1、代码中使用灯板属性而不是灯板ID进行差异化处理‘
 *   2、内部参数定义统一，PWM（范围0-100，100最亮），增益（1024为1倍，不同平台回调中处理）
 *   3、灯板控制及IR状态获取使用包装好的接口，可以忽略灯板的高低电平差异(_WhiteLightCtl, _RedLightCtl, _HwIR_WithDir)
 */

/*
 * TODO-LIST:
 *   1、如果有需要在切换灯板或者关灯等指定操作后，进行得操作，可以后续添加回调接口；
 *   2、日志模块完善，可以分模块控制打印等级，更为通用
 */

#define SOFT_IR_STABLE_D2N_TIMES	 (2)
#define SOFT_IR_STABLE_N2D_TIMES	 (15)
#define PRODUCT_SOFT_IR_STABLE_TIMES (1)
#define PRODUCT_HARD_IR_STABLE_TIMES (3)
#define LIGHT_DBG_CHECK_DELAY_MS    (10 * 1000)

typedef enum IntelligentNightState
{
    INTELLIGENT_LIGHT_STATE_DAY = 0,
    INTELLIGENT_LIGHT_STATE_NIGHT,
    INTELLIGENT_LIGHT_STATE_TRIG,
} INTELLIGENT_LIGHT_STATE_E;

static char* gINStStr[] = {"DAY", "NIGHT", "TRIG"};

static LIGHT_BOARD_S gLightBoards[] = {
    /*灯板ID,						        有红灯, 有白灯, 红灯自主控制, 光敏检测类型, 光敏检测触发电平, 亮灯触发电平,     PWM频率KHZ  */
    {LIGHT_BOARD_NONE}, // NONE
    {LIGHT_BOARD_INFRARED_HIR_HIGH,         TRUE,  FALSE, TRUE , IR_DETECT_HARDWARE, TRIG_LEVEL_HIGH, TRIG_LEVEL_HIGH, 20},
    {LIGHT_BOARD_INFRARED_HIR_LOW,          TRUE,  FALSE, TRUE , IR_DETECT_HARDWARE, TRIG_LEVEL_HIGH, TRIG_LEVEL_LOW,  20},
    {LIGHT_BOARD_INFRARED_SIR_HIGH,         TRUE,  FALSE, TRUE , IR_DETECT_SOFTWARE, TRIG_LEVEL_HIGH, TRIG_LEVEL_HIGH, 20},
    {LIGHT_BOARD_INFRARED_SIR_LOW,          TRUE,  FALSE, TRUE , IR_DETECT_SOFTWARE, TRIG_LEVEL_HIGH, TRIG_LEVEL_LOW,  20},

    {LIGHT_BOARD_INFINITY_WL_HIR_HIGH,      FALSE, TRUE,  FALSE, IR_DETECT_HARDWARE, TRIG_LEVEL_HIGH, TRIG_LEVEL_HIGH, 20},
    {LIGHT_BOARD_INFINITY_WL_HIR_LOW,       FALSE, TRUE,  FALSE, IR_DETECT_HARDWARE, TRIG_LEVEL_HIGH, TRIG_LEVEL_LOW,  20},
    {LIGHT_BOARD_INFINITY_WL_SIR_HIGH,      FALSE, TRUE,  FALSE, IR_DETECT_SOFTWARE, TRIG_LEVEL_HIGH, TRIG_LEVEL_HIGH, 20},
    {LIGHT_BOARD_INFINITY_WL_SIR_LOW,       FALSE, TRUE,  FALSE, IR_DETECT_SOFTWARE, TRIG_LEVEL_HIGH, TRIG_LEVEL_LOW,  20},

    {LIGHT_BOARD_INFINITY_RL_HIR_HIGH,      TRUE,  FALSE, FALSE, IR_DETECT_HARDWARE, TRIG_LEVEL_HIGH, TRIG_LEVEL_HIGH, 20},
    {LIGHT_BOARD_INFINITY_RL_HIR_LOW,       TRUE,  FALSE, FALSE, IR_DETECT_HARDWARE, TRIG_LEVEL_HIGH, TRIG_LEVEL_LOW,  20},
    {LIGHT_BOARD_INFINITY_RL_SIR_HIGH,      TRUE,  FALSE, FALSE, IR_DETECT_SOFTWARE, TRIG_LEVEL_HIGH, TRIG_LEVEL_HIGH, 20},
    {LIGHT_BOARD_INFINITY_RL_SIR_LOW,       TRUE,  FALSE, FALSE, IR_DETECT_SOFTWARE, TRIG_LEVEL_HIGH, TRIG_LEVEL_LOW,  20},

    {LIGHT_BOARD_DOUBLE_LIGHT_HIR_HIGH,     TRUE,  TRUE,  FALSE, IR_DETECT_HARDWARE, TRIG_LEVEL_HIGH, TRIG_LEVEL_HIGH, 20},
    {LIGHT_BOARD_DOUBLE_LIGHT_HIR_LOW,      TRUE,  TRUE,  FALSE, IR_DETECT_HARDWARE, TRIG_LEVEL_HIGH, TRIG_LEVEL_LOW,  20},
    {LIGHT_BOARD_DOUBLE_LIGHT_SIR_HIGH,     TRUE,  TRUE,  FALSE, IR_DETECT_SOFTWARE, TRIG_LEVEL_HIGH, TRIG_LEVEL_HIGH, 20},
    {LIGHT_BOARD_DOUBLE_LIGHT_SIR_LOW,      TRUE,  TRUE,  FALSE, IR_DETECT_SOFTWARE, TRIG_LEVEL_HIGH, TRIG_LEVEL_LOW,  20},
};

/* END - 灯板模式上下文定义 */
static int get_hour()
{
    struct tm tnow_r;
    time_t CurTime = 0;

    time(&CurTime);
    localtime_r(&CurTime, &tnow_r);

    return tnow_r.tm_hour;
}

static BOOL __check_ctx(LIGHT_CTX_S* lightCtx)
{
    if (!lightCtx)
    {
        printf("[LIGHT] Context CAN-NOT be NULL!\n");
        return FALSE;
    }

    if (LIGHT_BOARD_NONE == lightCtx->lightBoradID || lightCtx->lightBoradID > LIGHT_BOARD_NUM - 1)
    {
        printf("[LIGHT] Invalid Light Board ID(%d)!\n", lightCtx->lightBoradID);
        return FALSE;
    }

    if (!lightCtx->GetHardwareIRStatus || !lightCtx->IspCtrl || !lightCtx->WhiteLightCtrl
        || !lightCtx->RedLightCtrl || !lightCtx->GetAIStatus || !lightCtx->GetISPInfo)
    {
        printf("[LIGHT] Ctrl Funcs CAN-NOT be NULL!\n");
        return FALSE;
    }

    return TRUE;
}

void handleFrequentConvert(LIGHT_CTX_S* lightCtx, unsigned int checkTimeMs, UINT32 ev)
{
    static int conversion = 0;                  //连续日夜切换次数
    static unsigned int nowtime_cnt = 0;        //当前时间计数
    static unsigned int lasttime_cnt = 0;       //上次时间计数，判断是否处在2分钟内
    static LIGHTCTL_IRCUT_MODE_E LastDayNightStutas = LIGHTCTL_IRCUT_DAY;     //上次的日夜状态

    static int threshold = 0;
    static UINT32 last_ev = 0;                  //进入强制夜晚时的EV值
    static UINT32 t_count = 0;          //进入夜晚模式后，等待EV值平稳时的计数
    static UINT32 c_count = 0;          //进入强制夜晚后，当前EV值连续变化次数

    static int nPtzMovingPauseTimer = 0;        //云台转动后重新等待时间

    nowtime_cnt += checkTimeMs;

#if 1
    //云台转动期间暂停防频切检测
    if (LIGHT_PTZ_MOVING == lightCtx->ptzStatus)
    {
        nPtzMovingPauseTimer = 10000; //大约10S后还原
    }
    if (nPtzMovingPauseTimer > 0)
    {
        nPtzMovingPauseTimer -= checkTimeMs;
        if (nPtzMovingPauseTimer <= 0 && lightCtx->isForceNightStutas == TRUE)
        {
            t_count = 0;
            c_count = 0;
            lightCtx->isForceNightStutas = FALSE;
        }
        return;
    }
#endif

    do {
        if (lightCtx->isForceNightStutas == TRUE)     //已经进入强制夜晚模式
        {
            if (t_count < 7000)    //需等待7s左右，EV值进入平稳状态
            {
                t_count += checkTimeMs;
                if (t_count >= 7000)
                {
                    last_ev = ev;   //记录进入强制夜晚模式后的EV值
                    if (last_ev <= 5000) {           //反射面积越大,图像越亮，EV值变化越小
                        threshold = 300;
                    }
                    else if (last_ev <= 10000) {
                        threshold = 400;
                    }
                    else if (last_ev <= 20000) {
                        threshold = 1000;
                    }
                    else if (last_ev <= 40000) {
                        threshold = 5000;
                    }
                    else if (last_ev <= 70000) {
                        threshold = 7000;
                    }
                    else if (last_ev <= 90000) {
                        threshold = 10000;
                    }
                    else {
                        threshold = 15000;
                    }
                }
                break;
            }

            if ((ev - last_ev) >= threshold)     //EV值变化超过阈值
            {
                c_count += checkTimeMs;
            }
            else
            {
                c_count = 0;
            }

            t_count += checkTimeMs;
            if (c_count >= 7000 && t_count >= (7000))   //此时退出强制夜晚模式
            {
                t_count = 0; c_count = 0; last_ev= 0;
                lightCtx->isForceNightStutas = FALSE;
                printf("[%s:%d] this ev is over %d, exit force black mode!\n", __FILE__, __LINE__, threshold);
            }
            break;
        }

        if (nowtime_cnt - lasttime_cnt <= 60*2*1000)     //两分钟内判断是否发生频切
        {
            if (lightCtx->eDayNightStatus != LastDayNightStutas)
            {
                conversion++;
            }

            if (conversion >= 4 && LIGHTCTL_IRCUT_NIGHT == lightCtx->eDayNightStatus)     //如果频切超过4次以上，进入强制夜晚模式
            {
                conversion = 0;t_count = 0; c_count = 0; last_ev= 0;
                lightCtx->isForceNightStutas = TRUE;
                printf("[%s:%d]Lights is switching to frequent, forced to black mode!\n", __FILE__, __LINE__);
            }
        }
        else
        {
            conversion = 0;
            lasttime_cnt = nowtime_cnt;
        }
    } while(0);

    LastDayNightStutas = lightCtx->eDayNightStatus;
}

static int _WhiteLightCtl_WithDir(LIGHT_CTX_S* lightCtx, UINT32 duty)
{
    LIGHT_BOARD_S* lightBoard = NULL;

    if (NULL == lightCtx) return -1;

    lightBoard = &gLightBoards[lightCtx->lightBoradID];

    if (TRIG_LEVEL_HIGH == lightBoard->eLightTrigLevel)
    {
        lightCtx->WhiteLightCtrl(LIGHT_OP_MODE_PWM, duty, lightBoard->PWMFreqKHz);
        lightCtx->whiteLightBrightness = duty;
    }
    else
    {
        lightCtx->WhiteLightCtrl(LIGHT_OP_MODE_PWM, PWM_MAX_DUTY - duty, lightBoard->PWMFreqKHz);
        lightCtx->whiteLightBrightness = PWM_MAX_DUTY - duty;
    }

    return 0;
}

/**
 * @description: 白光灯控制
 * @param {LIGHT_CTX_S} *lightCtx 灯光控制上下文
 * @param {UINT32} duty 占空比
 * @return {int} 0：成功 <0：失败
 */
static int _WhiteLightCtl(LIGHT_CTX_S* lightCtx, UINT32 duty)
{
    // 灯光报警开启时，智能双光优先级高，其他模式优先级比灯光报警低
    // 之前灯光已经打开，可以继续操作
    if (lightCtx->m_NightLedFlag == NIGNTLED_ON)
    {
        XLOG_DEBUG("[LIGHT] Light Alarm Trig, Just Return...\n");
        return 0;
    }

    // 当前白光灯开关状态
    lightCtx->isWhiteLightOpen = !!duty;

    // 当前不支持PWM调光，为避免出问题
    if (duty != 0)
    {
        duty = PWM_MAX_DUTY;
    }

    XLOG_DEBUG("duty(%d) isWhiteLightOpen(%d)\n", duty, lightCtx->isWhiteLightOpen);

    return _WhiteLightCtl_WithDir(lightCtx, duty);
}

// 灯光报警控制不影响白光灯状态，闪烁不好控制，灯板实际不用这个状态，只用于控制灯板与灯光报警逻辑
static int _WhiteLightCtl_Alarm(LIGHT_CTX_S* lightCtx, UINT32 duty)
{
    return _WhiteLightCtl_WithDir(lightCtx, duty);
}

static int _RedLightCtl_WithDir(LIGHT_CTX_S* lightCtx, UINT32 duty)
{
    LIGHT_BOARD_S* lightBoard = NULL;

    if (NULL == lightCtx)
        return -1;

    lightBoard = &gLightBoards[lightCtx->lightBoradID];

    if (TRIG_LEVEL_HIGH == lightBoard->eLightTrigLevel)
    {
        lightCtx->RedLightCtrl(LIGHT_OP_MODE_PWM, duty, lightBoard->PWMFreqKHz);
        lightCtx->redLightBrightness = duty;
    }
    else
    {
        lightCtx->RedLightCtrl(LIGHT_OP_MODE_PWM, PWM_MAX_DUTY - duty, lightBoard->PWMFreqKHz);
        lightCtx->redLightBrightness = PWM_MAX_DUTY - duty;
    }

    return 0;
}

static int _RedLightCtl(LIGHT_CTX_S* lightCtx, UINT32 duty)
{
    lightCtx->isRedLightOpen = !!duty;

    // 当前不支持PWM调光，为避免出问题
    if (duty != 0)
    {
        duty = PWM_MAX_DUTY;
    }

    return _RedLightCtl_WithDir(lightCtx, duty);
}

static IR_STATUS_E _HwIR_WithDir(LIGHT_CTX_S* lightCtx)
{
    LIGHT_BOARD_S* lightBoard = NULL;
    IR_STATUS_E trig = IR_STATUS_DAY;

    if (NULL == lightCtx)
        return -1;

    lightBoard = &gLightBoards[lightCtx->lightBoradID];

    if (TRIG_LEVEL_HIGH == lightBoard->eIRTrigLevel)
    {
        lightCtx->GetHardwareIRStatus(&trig);
    }
    else
    {
        lightCtx->GetHardwareIRStatus(&trig);
        trig = !trig;
    }

    return trig;
}

static IR_STATUS_E _SfIR_WithDir(LIGHT_CTX_S* lightCtx, IR_ISP_INFO_S *pLightIrInfo, LIGHTCTL_IRCUT_MODE_E CurState)
{
    IR_STATUS_E IRStatus = IR_STATUS_DAY;
#if	defined(__PRODUCT_P4__)||defined(__PRODUCT_P4U__)
    UINT32 N2DEVFactor = 520000;
    UINT32 D2NEVFactor = 3650000;
#else
	UINT32 N2DEVFactor = 215000;
    UINT32 D2NEVFactor = 1800000;
#endif

    static IR_STATUS_E LastIRStatus = IR_STATUS_DAY;
    static UINT32 LastEvValue = 0;
    static UINT32 InvalidCnt = 0;

    /* 晚上当画面亮度发生突变时，延迟3次开始检测 */
    if ((LIGHTCTL_IRCUT_NIGHT == CurState) && (LastEvValue - pLightIrInfo->ev) > 50000)
    {
        InvalidCnt = 3;
    }
    LastEvValue = pLightIrInfo->ev;

    if (InvalidCnt > 0)
    {
        InvalidCnt--;
        printf("[%s][%d] InvalidCnt[%d]\n", __FUNCTION__, __LINE__, InvalidCnt);
        return LastIRStatus;
    }

    if (LIGHTCTL_IRCUT_DAY == CurState)
    {
        /* 白切黑检测 */
        if ((pLightIrInfo->gain > lightCtx->D2NFactor) && (pLightIrInfo->ev > D2NEVFactor))
        {
            IRStatus = IR_STATUS_NIGHT;
        }
        else
        {
            IRStatus = IR_STATUS_DAY;
        }
    }
    else
    {
        /* 黑切白检测 */
        if ((pLightIrInfo->gain < lightCtx->N2DFactor) && (pLightIrInfo->ev < N2DEVFactor))
        {
            IRStatus = IR_STATUS_DAY;
        }
        else
        {
            IRStatus = IR_STATUS_NIGHT;
        }
    }

    LastIRStatus = IRStatus;
    return IRStatus;
}

static BOOL _IsLightAlarmOn2Off(LIGHT_CTX_S* lightCtx)
{
    if (TRUE == lightCtx->lastLightAlarmStReal && FALSE == lightCtx->lightAlarmStReal)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/**
 * @description: 软光敏灯板强切白天（时间段6-9时，每隔半小时切换下彩色，切到夜晚的时间大于等于IR_DELAY_TIME_AFTER_SWITCH_TO_NIGHT）
 * @param {LIGHT_CTX_S} *lightCtx 灯板控制上下文
 * @param {UINT64} nightStart 切到夜晚的开始时间
 * @return {BOOL} TRUE: 满足强切条件 FALSE: 不满足强切条件
 */
static BOOL _EveryHourForceToDay(LIGHT_CTX_S* lightCtx, UINT64 nightStart)
{
    BOOL bRedrayForceToDay = FALSE;

    if (LIGHTCTL_IRCUT_NIGHT == lightCtx->eDayNightStatus && (get_ms() - nightStart >= IR_DELAY_TIME_AFTER_SWITCH_TO_NIGHT)) //刚切为晚上，保持一段时间再进行整点判断
    {
        int iCurTimeHour = 0;
        int iCurTimemin = 0;
        int iCurTimesec = 0;
        struct tm tnow_r;
        time_t CurTime = 0;

        time(&CurTime);
        localtime_r(&CurTime, &tnow_r);

        iCurTimeHour = tnow_r.tm_hour;
        iCurTimemin = tnow_r.tm_min;
        iCurTimesec = tnow_r.tm_sec;

        if ((iCurTimeHour >= 6 && iCurTimeHour <= 9)  && ((0 == iCurTimemin % 30) && (0 == iCurTimesec)))
        {
            if (9 == iCurTimeHour && 30 == iCurTimemin)
            {
                bRedrayForceToDay = FALSE;
            }
            else
            {
                bRedrayForceToDay = TRUE;
            }
            // fprintf(stderr, "[%s][%d] ********iRedrayForceSwitchHour=[%d] force redray to day*****\r\n", __FUNCTION__, __LINE__, iRedrayForceSwitchHour);
            printf("[%s][%d] ********iRedrayForceSwitchHour=[%d],Min=[%d] force redray to day*****\r\n", __FUNCTION__, __LINE__, iCurTimeHour, iCurTimemin);
        }
    }
    else
    {
        bRedrayForceToDay = FALSE;
    }

    return bRedrayForceToDay;
}

/**
 * @description: 灯光报警初始化
 * @param {LIGHT_CTX_S*} lightCtx 灯光控制上下文
 * @return {int} 0:成功 <0:失败
 */
int LightCtl_Init(LIGHT_CTX_S* lightCtx)
{
    XLOG_DEBUG("[LIGHTCTL] Init -->>\n");
    return 0;
}

/**
 * @description: 时间控制模式
 * @param {LIGHT_CTX_S} *lightCtx 灯板控制上下文
 * @param {BOOL} reset 是否重置
 * @return {int} 0：成功 <0：失败
 */
int LightCtl_Time(LIGHT_CTX_S* lightCtx, BOOL reset)
{
    static SINT8 lastTimeFlag = -1;

    time_t curTime;
    struct tm curr_tm;
    UINT32 curSec = 0, startSec = 0, stopSec = 0;
    SINT8 timeFlag = 0;

    if (reset)
    {
        _WhiteLightCtl(lightCtx, 0);
        _RedLightCtl(lightCtx, 0);
        lastTimeFlag = -1;
    }

    //获取当前时间段信息
    curTime = time(NULL);
    localtime_r(&curTime, &curr_tm);
    curSec = (curr_tm.tm_hour * 3600 + curr_tm.tm_min * 60 + curr_tm.tm_sec);
    startSec = lightCtx->ctlTime.startSec;
    stopSec = lightCtx->ctlTime.stopSec;

    if (startSec > stopSec)
    {
        if (((curSec >= startSec) && (curr_tm.tm_hour <= 23)) || (curSec <= stopSec))
            timeFlag = 1;
        else
            timeFlag = 0;
    }
    else
    {
        if ((curSec >= startSec) && (curSec <= stopSec))
            timeFlag = 1;
        else
            timeFlag = 0;
    }

    //时间进入定时范围或超过定时范围，或刚切换成定时模式
    if ((lastTimeFlag != timeFlag) || reset)
    {
        if (1 == timeFlag)
        {
            lightCtx->IspCtrl(LIGHTCTL_IRCUT_DAY);

            _WhiteLightCtl(lightCtx, 0);
            _RedLightCtl(lightCtx, 0);
        }
        else
        {
            lightCtx->IspCtrl(LIGHTCTL_IRCUT_NIGHT);

            _WhiteLightCtl(lightCtx, 0);
            _RedLightCtl(lightCtx, PWM_MAX_DUTY);
        }
    }

    lastTimeFlag = timeFlag;
    lightCtx->lightBoardCheckDelayMs = LIGHT_BOARD_CHECK_DELAY_MS;

    return 0;
}

/**
 * @description: 固定日夜模式，切换模式得时候直接在配置接口切换了日夜，函数中只需要切换灯板
 * @param {LIGHT_CTX_S} *lightCtx 灯板控制上下文
 * @param {BOOL} reset 是否重置
 * @return {int} 0：成功 <0：失败
 */
int LightCtl_FixDayNight(LIGHT_CTX_S* lightCtx, BOOL reset)
{
    if (LIGHTCTL_MODE_DAY == lightCtx->ctrlMode)
    {
        _WhiteLightCtl(lightCtx, 0);
        _RedLightCtl(lightCtx, 0);
    }
    else if (LIGHTCTL_MODE_NIGHT == lightCtx->ctrlMode)
    {
        _WhiteLightCtl(lightCtx, 0);
        _RedLightCtl(lightCtx, PWM_MAX_DUTY);
    }

    lightCtx->lightBoardCheckDelayMs = LIGHT_BOARD_CHECK_DELAY_MS;

    return 0;
}

/**
 * @description: 普通红外，自动主动模式
 * @param {LIGHT_CTX_S} *lightCtx 灯板控制上下文
 * @param {BOOL} reset 是否重置
 * @return {int} 0：成功 <0：失败
 */
int LightCtl_Initiative(LIGHT_CTX_S* lightCtx, BOOL reset)
{
    static unsigned int N2DCnt = 0;
    static unsigned int D2NCnt = 0;
    static int iAgainDark = 0;
    static int iAgainBright = 0;

    IR_ISP_INFO_S LightIrInfo = {0};
    IR_STATUS_E IRStatus = IR_STATUS_DAY;

    if (reset)
    {
        D2NCnt = 0;
        N2DCnt = 0;

        lightCtx->IspCtrl(LIGHTCTL_IRCUT_DAY);
        _WhiteLightCtl(lightCtx, 0); //关白灯
        _RedLightCtl(lightCtx, 0);   //关红灯
        return 0;
    }

    //查询AGAIN
    lightCtx->GetISPInfo(&LightIrInfo);
    XLOG_DEBUG("[LIGHT] th(%d) sw(%d) gain(%d) D2NFactor(%d) N2DFactor(%d)\n", lightCtx->initiativeModeTh, lightCtx->dayNightSwitchTime, LightIrInfo.gain, lightCtx->D2NFactor, lightCtx->N2DFactor);

    if (LightIrInfo.gain >= lightCtx->D2NFactor)
    {
        iAgainDark++;
    }
    else
    {
        iAgainDark = 0;
    }

    if (LightIrInfo.gain <= lightCtx->N2DFactor)
    {
        iAgainBright++;
    }
    else
    {
        iAgainBright = 0;
    }

    if (iAgainDark > lightCtx->dayNightSwitchTime)
        iAgainDark = lightCtx->dayNightSwitchTime + 1;
    if (iAgainBright > lightCtx->dayNightSwitchTime)
        iAgainBright = lightCtx->dayNightSwitchTime + 1;

    //查询光敏的状态
    IRStatus = _HwIR_WithDir(lightCtx);
    if (LIGHTCTL_IRCUT_DAY == lightCtx->eDayNightStatus) /* 如果原来为白天 */
    {
        if (IR_STATUS_NIGHT == IRStatus)
            D2NCnt++;   /* 持续的秒数计数器加一 */
        else               /* 未达到pChContext->m_DayNightSwitchTime都清零 */
            D2NCnt = 0; /* 计数器清零 */
    }
    else /* 如果原来为晚上 */
    {
        if (IR_STATUS_DAY == IRStatus)
            N2DCnt++;
        else
            N2DCnt = 0;
    }

    if (D2NCnt > lightCtx->dayNightSwitchTime)
    {
        D2NCnt = lightCtx->dayNightSwitchTime + 1;
    }

    if (N2DCnt > lightCtx->dayNightSwitchTime)
    {
        N2DCnt = lightCtx->dayNightSwitchTime + 1;
    }

    XLOG_DEBUG("[LIGHT] adc(%d) iAgainDark(%d) iAgainBright(%d) N2DCnt(%d) D2NCnt(%d)\n", IRStatus, iAgainDark, iAgainBright, N2DCnt, D2NCnt);

    //往黑夜时，要满足again和光敏两个条件才会切入晚上
    if ((iAgainDark >= lightCtx->dayNightSwitchTime) && (D2NCnt >= lightCtx->dayNightSwitchTime)) /* 稳定状态持续时间到达，可以切换了，并将计数器清零 */
    {
        lightCtx->IspCtrl(LIGHTCTL_IRCUT_NIGHT);

        _WhiteLightCtl(lightCtx, 0);
        _RedLightCtl(lightCtx, PWM_MAX_DUTY);

        D2NCnt = 0; /* 计数器清零 */
    }

    //往白天时，只需要满足光敏即切入白天
    if (N2DCnt >= lightCtx->dayNightSwitchTime) /* 稳定状态持续时间到达，可以切换了，并将计数器清零 */
    {
        lightCtx->IspCtrl(LIGHTCTL_IRCUT_DAY);

        _WhiteLightCtl(lightCtx, 0);
        _RedLightCtl(lightCtx, 0);

        N2DCnt = 0; /*计数器清零*/
    }

    lightCtx->lightBoardCheckDelayMs = LIGHT_BOARD_CHECK_DELAY_MS;

    return 0;
}

/**
 * @description: 普通红外，自动被动模式
 * @param {LIGHT_CTX_S} *lightCtx 灯板控制上下文
 * @param {BOOL} reset 是否重置
 * @return {int} 0：成功 <0：失败
 */
int LightCtl_Passivity(LIGHT_CTX_S* lightCtx, BOOL reset)
{
    static UINT32 N2DCnt = 0;
    static UINT32 D2NCnt = 0;
    IR_STATUS_E IRStatus = 0;

    if (reset)
    {
        D2NCnt = 0;
        N2DCnt = 0;

        lightCtx->IspCtrl(LIGHTCTL_IRCUT_DAY);
        _WhiteLightCtl(lightCtx, 0); //关白灯
        _RedLightCtl(lightCtx, 0);   //关红灯
        return 0;
    }

    if (lightCtx->usePtzCtrl)
    {
        if (LIGHTCTL_IRCUT_NIGHT == lightCtx->ptzCtrlStatus) // night
        {
            IRStatus = IR_STATUS_NIGHT;
        }
        else // day
        {
            IRStatus = IR_STATUS_DAY;
        }
    }
    else
    {
        //查询光敏的状态
        IRStatus = _HwIR_WithDir(lightCtx);
    }

    if (LIGHTCTL_IRCUT_DAY == lightCtx->eDayNightStatus) /* 如果原来为白天，即adc_status == 0 */
    {
        if (IR_STATUS_NIGHT == IRStatus)
            D2NCnt++;   /* 持续的秒数计数器加一 */
        else               /* 未达到pChContext->m_DayNightSwitchTime都清零 */
            D2NCnt = 0; /* 计数器清零 */
    }
    else /* 如果原来为晚上 */
    {
        if (IR_STATUS_DAY == IRStatus)
            N2DCnt++;
        else
            N2DCnt = 0;
    }

    XLOG_DEBUG("[LIGHT] IR(%s) D2NCnt(%d) N2DCnt(%d)\n", (IR_STATUS_NIGHT == IRStatus) ? "NIGHT" : "DAY", D2NCnt, N2DCnt);

    if (D2NCnt >= ((1 == HY_Enc_GetProductMode()) ? PRODUCT_HARD_IR_STABLE_TIMES : lightCtx->dayNightSwitchTime)) /* 稳定状态持续时间到达，可以切换了，并将计数器清零 */
    {
        lightCtx->IspCtrl(LIGHTCTL_IRCUT_NIGHT);

        _WhiteLightCtl(lightCtx, 0); //关白灯
        _RedLightCtl(lightCtx, PWM_MAX_DUTY); //开红灯

        D2NCnt = 0; /* 计数器清零 */
    }
    else if (N2DCnt >= ((1 == HY_Enc_GetProductMode()) ? PRODUCT_HARD_IR_STABLE_TIMES : lightCtx->dayNightSwitchTime)) /* 稳定状态持续时间到达，可以切换了，并将计数器清零 */
    {
        lightCtx->IspCtrl(LIGHTCTL_IRCUT_DAY);

        _WhiteLightCtl(lightCtx, 0); //关白灯
        _RedLightCtl(lightCtx, 0); //开红灯

        N2DCnt = 0; /*计数器清零*/
    }

    lightCtx->lightBoardCheckDelayMs = LIGHT_BOARD_CHECK_DELAY_MS;

    return 0;
}

/**
 * @description: 普通红外，无光敏（软光敏）
 * @param {LIGHT_CTX_S} *lightCtx 灯板控制上下文
 * @param {BOOL} reset 是否重置
 * @return {int} 0：成功 <0：失败
 */
int LightCtl_NoLDRNormalRedray(LIGHT_CTX_S* lightCtx, BOOL reset)
{
    // LOCAL
    IR_ISP_INFO_S LightIrInfo = {0};
    IR_STATUS_E IRStatus = -1;
    BOOL bRedrayForceToDay = FALSE;
    static UINT64 nightStartTime = 0;
	static UINT32 DayStatusCount = 0;
	static UINT32 NightStatusCount = 0;

    if (reset)
    {
        DayStatusCount = 0;
        NightStatusCount = 0;

        lightCtx->IspCtrl(LIGHTCTL_IRCUT_DAY);
        _WhiteLightCtl(lightCtx, 0); //关白灯
        _RedLightCtl(lightCtx, 0);   //关红灯
        return 0;
    }

    /* 获取统计信息 */
    lightCtx->GetISPInfo(&LightIrInfo);

    /* 软光敏整点强制切换至白天 */
    bRedrayForceToDay = _EveryHourForceToDay(lightCtx, nightStartTime);

    /* 防频切机制 */
    handleFrequentConvert(lightCtx, LIGHT_BOARD_CHECK_DELAY_MS/1000, LightIrInfo.ev);

    if (TRUE == bRedrayForceToDay)
    {
        IRStatus = IR_STATUS_DAY;
    }
    else if (TRUE == lightCtx->isForceNightStutas)
    {
        IRStatus = IR_STATUS_NIGHT;
    }
    else
    {
        /* 软光敏检测 */
        IRStatus = _SfIR_WithDir(lightCtx, &LightIrInfo, lightCtx->eDayNightStatus);
    }

#if 0
    if (access("/mnt/mtd/force_day", F_OK) == 0)
    {
        IRStatus = IR_STATUS_DAY;
    }
    else if (access("/mnt/mtd/force_night", F_OK) == 0)
    {
        IRStatus = IR_STATUS_NIGHT;
    }
#else
    XLOG_DEBUG("ForceToDay[%d] ForceNight[%d] gain[%d] ev[%d] D2N[%d] N2D[%d] D_NCnt[%d-%d] CurState[%d] NextState[%d]\n",
        bRedrayForceToDay, lightCtx->isForceNightStutas, LightIrInfo.gain, LightIrInfo.ev, lightCtx->D2NFactor,
        lightCtx->N2DFactor,DayStatusCount, NightStatusCount, lightCtx->eDayNightStatus, !IRStatus);
#endif

    if (IR_STATUS_DAY == IRStatus && LIGHTCTL_IRCUT_NIGHT == lightCtx->eDayNightStatus)
    {
        DayStatusCount++;
        NightStatusCount = 0;

        if (bRedrayForceToDay || DayStatusCount >= ((1 == HY_Enc_GetProductMode()) ? PRODUCT_SOFT_IR_STABLE_TIMES : SOFT_IR_STABLE_N2D_TIMES))
        {
            lightCtx->IspCtrl(LIGHTCTL_IRCUT_DAY);
            _WhiteLightCtl(lightCtx, 0);
            _RedLightCtl(lightCtx, 0);
        }
    }
    else if (IR_STATUS_NIGHT == IRStatus && LIGHTCTL_IRCUT_DAY == lightCtx->eDayNightStatus && lightCtx->m_NightLedFlag != NIGNTLED_ON)
    {
        NightStatusCount++;
        DayStatusCount = 0;

        if (NightStatusCount >= ((1 == HY_Enc_GetProductMode()) ? PRODUCT_SOFT_IR_STABLE_TIMES : SOFT_IR_STABLE_D2N_TIMES))
        {
            nightStartTime = get_ms();
            lightCtx->IspCtrl(LIGHTCTL_IRCUT_NIGHT);
            _WhiteLightCtl(lightCtx, 0);
            _RedLightCtl(lightCtx, PWM_MAX_DUTY);
        }
    }
    else
    {
        NightStatusCount = 0;
        DayStatusCount = 0;
    }

    lightCtx->lightBoardCheckDelayMs = LIGHT_BOARD_CHECK_DELAY_MS;
    return 0;
}

/**
 * @description: 全彩白光，获取软光敏状态附带增益阈值判断
 * @param {LIGHT_CTX_S} *lightCtx 灯板控制上下文
 * @return {int} 0：成功 <0：失败
 */
static IR_STATUS_E _ColorNight_GetSoftIRWithGain(LIGHT_CTX_S* lightCtx, BOOL rst)
{
    IR_ISP_INFO_S LightIrInfo = {0};

    lightCtx->GetISPInfo(&LightIrInfo);

    if (LightIrInfo.gain >= lightCtx->D2NFactor)
    {
        return IR_STATUS_NIGHT;
    }

    return IR_STATUS_DAY;
}

int LightCtl_ColorNight(LIGHT_CTX_S* lightCtx, BOOL reset)
{
    static SINT32 sAdcCheckCycle = 0;
    static UINT32 sDarkNum = 0;

    LIGHT_BOARD_S* lightBoard = NULL;
    IR_STATUS_E IRStatus = IR_STATUS_DAY;

    lightBoard = &gLightBoards[lightCtx->lightBoradID];

    if (LIGHTCTL_IRCUT_NIGHT == lightCtx->eDayNightStatus)
    {
        //切换到彩色
        lightCtx->IspCtrl(LIGHTCTL_IRCUT_DAY);
    }

    if (reset)
    {
        sAdcCheckCycle = 0;
        sDarkNum = 0;
    }

    if (TRUE == lightCtx->lightAlarmStReal)
    {
        sAdcCheckCycle = 0;
        sDarkNum = 0;
        lightCtx->lightBoardCheckDelayMs = LIGHT_BOARD_CHECK_DELAY_MS;
        return 0;
    }

    if (sAdcCheckCycle <= 0)
    {
        //获取亮度值 ，先关灯
        if (lightBoard->hasRedLight)
        {
            _RedLightCtl(lightCtx, 0);
        }

        if (lightBoard->hasWhiteLight)
        {
            _WhiteLightCtl(lightCtx, 0);
        }

        // 延时一段时间，灯带采集稳定
        usleep(100 * 1000);

        if (IR_DETECT_SOFTWARE == lightBoard->eIRDetType)
        {
            IRStatus = _ColorNight_GetSoftIRWithGain(lightCtx, reset);
        }
        else
        {
            IRStatus = _HwIR_WithDir(lightCtx);
        }

        //计算亮度变化后持续的次数
        if (IR_STATUS_NIGHT == IRStatus)
        {
            sDarkNum++;
        }
        else
        {
            sDarkNum = 0;
        }

        XLOG_DEBUG("[LIGHT] Check IRStatus(%d) sDarkNum(%d)\n", IRStatus, sDarkNum);

        if (sDarkNum >= lightCtx->dayNightSwitchTime) /* 稳定状态持续时间到达，可以切换了，并将计数器清零 */
        {
            if (lightBoard->hasRedLight)
            {
                _RedLightCtl(lightCtx, 0);
            }

            _WhiteLightCtl(lightCtx, PWM_MAX_DUTY);

            lightCtx->colorNightMode = 1;

            //重置检测周期
            sAdcCheckCycle = ADC_CHECK_CYCLE_FOR_COLOR_NIGHT; // 10分钟检测一次
            sDarkNum = 0; // 重置计数
        }
        else if (0 == sDarkNum)
        {
            if (lightBoard->hasRedLight)
            {
                _RedLightCtl(lightCtx, 0);
            }

            _WhiteLightCtl(lightCtx, 0);

            lightCtx->colorNightMode = 0;

            //重置检测周期
            sAdcCheckCycle = 0;
        }
    }
    else
    {
        sAdcCheckCycle -= LIGHT_BOARD_CHECK_DELAY_MS;
        XLOG_DEBUG("[LIGHT] sAdcCheckCycle(%d)\n", sAdcCheckCycle);
    }

    //开启白光灯，灯板上检测是白天时是不会起效白光灯的
    // setColorNightLed(pSysInfo->m_BoardInfo, 1);
    //如果是夜晚

    lightCtx->lightBoardCheckDelayMs = LIGHT_BOARD_CHECK_DELAY_MS;

    return 0;
}

int LightCtl_ColorNightManual(LIGHT_CTX_S* lightCtx, BOOL reset)
{
    static SINT8 lastTimeFlag = -1;

    time_t curTime;
    struct tm curr_tm;
    UINT32 curSec = 0, startSec = 0, stopSec = 0;
    SINT8 timeFlag = 0;
    LIGHT_BOARD_S* lightBoard = NULL;

    lightBoard = &gLightBoards[lightCtx->lightBoradID];

    if (reset)
    {
        lastTimeFlag = -1;
    }

    if (LIGHTCTL_IRCUT_NIGHT == lightCtx->eDayNightStatus)
    {
        //切换到彩色
        lightCtx->IspCtrl(LIGHTCTL_IRCUT_DAY);
    }

    //获取当前时间段信息
    curTime = time(NULL);
    localtime_r(&curTime, &curr_tm);
    curSec = (curr_tm.tm_hour * 3600 + curr_tm.tm_min * 60 + curr_tm.tm_sec);
    startSec = lightCtx->ctlTime.startSec;
    stopSec = lightCtx->ctlTime.stopSec;

    if (startSec > stopSec)
    {
        if (((curSec >= startSec) && (curr_tm.tm_hour <= 23)) || (curSec <= stopSec))
            timeFlag = 1;
        else
            timeFlag = 0;
    }
    else
    {
        if ((curSec >= startSec) && (curSec <= stopSec))
            timeFlag = 1;
        else
            timeFlag = 0;
    }

    XLOG_DEBUG("[LIGHTCTL] cur(%d) start(%d) stop(%d) timeFlag(%d) lastTimeFlag(%d)\n", \
        curSec, startSec, stopSec, timeFlag, lastTimeFlag);

    //时间进入定时范围或超过定时范围，或刚切换成自定义控制
    if (lastTimeFlag != timeFlag || _IsLightAlarmOn2Off(lightCtx))
    {
        if (1 == timeFlag)
        {
            //定时范围内
            if (lightBoard->hasRedLight)
            {
                _RedLightCtl(lightCtx, 0);
            }

            _WhiteLightCtl(lightCtx, PWM_MAX_DUTY);
            lightCtx->colorNightMode = 1;
        }
        else
        {
            //定时范围外
            if (lightBoard->hasRedLight)
            {
                _RedLightCtl(lightCtx, 0);
            }

            _WhiteLightCtl(lightCtx, 0);
            lightCtx->colorNightMode = 0;
        }
    }

    lastTimeFlag = timeFlag;
    lightCtx->lightBoardCheckDelayMs = LIGHT_BOARD_CHECK_DELAY_MS;

    return 0;
}

int LightCtl_BlackMode_SetParam(UINT32 nIsForce2Day)
{
	return 0;
}

int LightCtl_BlackMode_GetParam(UINT32 *nVarWhiteGainFlag, UINT32 *nWhiteLightSelfCheck, UINT32 *nRedLightSelfCheck)
{
	return 0;
}

int lightCtl_IntelligentNight(LIGHT_CTX_S* lightCtx, int reset)
{
    // CONTEXT
    static INTELLIGENT_LIGHT_STATE_E sNextState;
    static INTELLIGENT_LIGHT_STATE_E sLastState;
    static INTELLIGENT_LIGHT_STATE_E sCurState;
    static BOOL sAIStatus = FALSE;
    static BOOL sLastAIStatus = FALSE;
    static UINT32 sDelayTime = 0;
	static UINT32 sForceExitWriteTime = 0;
    static BOOL sFirstTrig = TRUE; // 是否为第一次触发
    static UINT64 sTrigStartMs = 0; // 触发开始时间
    static UINT64 sNightStartMs = 0; // 进入夜晚的开始时间
    static UINT32 sStableNum = 0;

    // LOCAL
    LIGHT_BOARD_S* lightBoard = &gLightBoards[lightCtx->lightBoradID];
    UINT64 curTimeMs = 0;
    UINT32 curHour = 0;
    BOOL force2Day = FALSE;
    IR_STATUS_E IRStatus = IR_STATUS_DAY;
    IR_ISP_INFO_S LightIrInfo = {0};

    if (reset)
    {
        sLastState = sNextState = sCurState = INTELLIGENT_LIGHT_STATE_DAY;
        lightCtx->IspCtrl(LIGHTCTL_IRCUT_DAY);
        _WhiteLightCtl(lightCtx, 0);
        _RedLightCtl(lightCtx, 0);
        lightCtx->IspCtrl(LIGHTCTL_IRCUT_DAY);
        sAIStatus = FALSE;
        sLastAIStatus = FALSE;
        sDelayTime = 0;
        lightCtx->GetAIStatus(sLastAIStatus, &sAIStatus, TRUE); // 清空AI检测状态
    }

    if (INTELLIGENT_LIGHT_STATE_DAY == sCurState)
    {
        if (sCurState != sLastState)
        {
            lightCtx->IspCtrl(LIGHTCTL_IRCUT_DAY);

            _WhiteLightCtl(lightCtx, 0);
            _RedLightCtl(lightCtx, 0);

            sStableNum = 0;
        }

        if (IR_DETECT_HARDWARE == lightBoard->eIRDetType)
        {
            IRStatus = _HwIR_WithDir(lightCtx);
        }
        else
        {
            /* 获取统计信息 */
            lightCtx->GetISPInfo(&LightIrInfo);

            // 软光敏频切处理
            handleFrequentConvert(lightCtx, COLOR_NIGHT_SLEEP_TIME/1000, LightIrInfo.ev);
            if (lightCtx->isForceNightStutas == TRUE)
            {
                IRStatus = IR_STATUS_NIGHT;
            }
            else
            {
                /* 软光敏检测 */
                IRStatus = _SfIR_WithDir(lightCtx, &LightIrInfo, LIGHTCTL_IRCUT_DAY);
            }
        }

        sStableNum = (IR_STATUS_NIGHT == IRStatus) ? sStableNum + 1 : 0;

        lightCtx->GetAIStatus(sLastAIStatus, &sAIStatus, 0);

        XLOG_DEBUG("[LIGHT] IRStatus(%d) NUM(%d) sAIStatus(%d)\n", IRStatus, sStableNum, sAIStatus);

        if (sStableNum * COLOR_NIGHT_SLEEP_TIME >= COLOR_DAY_TO_NIGHT_SWITCH_TIME)
        {
            XLOG_INFO("[LIGHT] Switch to Night...\n");
            sNextState = INTELLIGENT_LIGHT_STATE_NIGHT;
        }
    }
    else if (INTELLIGENT_LIGHT_STATE_NIGHT == sCurState)
    {
        if (sCurState != sLastState)
        {
            lightCtx->IspCtrl(LIGHTCTL_IRCUT_NIGHT);
            _WhiteLightCtl(lightCtx, 0);
            _RedLightCtl(lightCtx, PWM_MAX_DUTY);
            sStableNum = 0;
            sLastAIStatus = FALSE;
            lightCtx->GetAIStatus(sLastAIStatus, &sAIStatus, TRUE); // 清空AI检测状态,切换图像可能导致误检
            sNightStartMs = get_ms();
            lightCtx->intelligentWhiteStatue = 0;
        }

        if (IR_DETECT_HARDWARE == lightBoard->eIRDetType)
        {
            IRStatus = _HwIR_WithDir(lightCtx);
        }
        else
        {
            // 软光敏整点强切
            force2Day = _EveryHourForceToDay(lightCtx, sNightStartMs);

			/* 获取统计信息 */
            lightCtx->GetISPInfo(&LightIrInfo);

            // 软光敏频切处理
            handleFrequentConvert(lightCtx, COLOR_NIGHT_SLEEP_TIME/1000, LightIrInfo.ev);
            if (lightCtx->isForceNightStutas == TRUE)
            {
                IRStatus = IR_STATUS_NIGHT;
            }
            else
            {
                /* 软光敏检测 */
                IRStatus = _SfIR_WithDir(lightCtx, &LightIrInfo, LIGHTCTL_IRCUT_NIGHT);
            }
        }

        sStableNum = (IR_STATUS_DAY == IRStatus) ? sStableNum + 1 : 0;

        XLOG_DEBUG("[LIGHT] IRStatus(%d) NUM(%d)\n", IRStatus, sStableNum);

        if (force2Day || sStableNum * COLOR_NIGHT_SLEEP_TIME >= COLOR_NIGHT_TO_DAY_SWITCH_TIME)
        {
            XLOG_DEBUG("[LIGHT] Switch to DAY...\n");
            sNextState = INTELLIGENT_LIGHT_STATE_DAY;
			if (lightCtx->isForceNightStutas)
			{
				lightCtx->isForceNightStutas = FALSE;
			}
			XLOG_DEBUG("force day,quit ForceNightStutas %d",lightCtx->isForceNightStutas);
        }
        else
        {
            if (get_ms() - sNightStartMs >= AI_CHECK_DELAY_MS)
            {
                lightCtx->GetAIStatus(sLastAIStatus, &sAIStatus, 0);
                if (sAIStatus)
                {
                    XLOG_DEBUG("[LIGHT] Switch to TRIG...\n");
                    _WhiteLightCtl(lightCtx, PWM_MAX_DUTY);
                    _RedLightCtl(lightCtx, 0);
                    lightCtx->IspCtrl(LIGHTCTL_IRCUT_DAY);
                    sNextState = INTELLIGENT_LIGHT_STATE_TRIG;
                    sDelayTime = 0;
                    lightCtx->intelligentWhiteStatue = 1;
					lightCtx->isForceNightStutas = FALSE;
					XLOG_DEBUG("quit ForceNightStutas %d\n",lightCtx->isForceNightStutas);
                }
            }
            else
            {
                lightCtx->GetAIStatus(sLastAIStatus, &sAIStatus, TRUE); // 清空AI检测状态
                XLOG_DEBUG("[LIGHT] clear AI and Deley(%lld) to Check AI!\n", AI_CHECK_DELAY_MS - (get_ms() - sNightStartMs));
            }
        }
    }
    else // INTELLIGENT_LIGHT_STATE_TRIG
    {
        lightCtx->GetAIStatus(sLastAIStatus, &sAIStatus, 0);
        if (sAIStatus)
        {
            if (sFirstTrig)
            {
                sFirstTrig = FALSE;
                sTrigStartMs = get_ms();
                sDelayTime = 0;
            }
            else
            {
                curTimeMs = get_ms();
                curHour = get_hour();

                if ((curHour >= 6 && curHour <= 9) && (curTimeMs - sTrigStartMs >= 1000 * 60 * 60))
                {
                    force2Day = TRUE; //判定为白天
                    sForceExitWriteTime = 0;
                    //printf("[%s][%d] ********iRedrayForceSwitchHour=[%d],Min=[%d] force redray to day*****\r\n", __FUNCTION__, __LINE__, curHour, curMin);
                }
                else
                {
                    sDelayTime = 0;
                }
            }
        }

        sDelayTime += COLOR_NIGHT_SLEEP_TIME;
		sForceExitWriteTime += COLOR_NIGHT_SLEEP_TIME;
        if (sDelayTime >= lightCtx->colorNightContinueTimeMs) //计数时间达到阈值
        {
            XLOG_DEBUG("[LIGHT] AI Trig Stop, Switch to Night...\n");
			sForceExitWriteTime = 0;
            sFirstTrig = TRUE;

            if (!force2Day) // 夜晚
            {
                sNextState = INTELLIGENT_LIGHT_STATE_NIGHT;
            }
            else
            {
                sNextState = INTELLIGENT_LIGHT_STATE_DAY;
            }
            sLastAIStatus = FALSE;
            force2Day = FALSE;
        }

		if(sForceExitWriteTime >= COLOR_NIGHT_SLEEP_TIME * 60 * 10)
		{
			sFirstTrig = TRUE;
			sForceExitWriteTime = 0;
			sNextState = INTELLIGENT_LIGHT_STATE_DAY;
		}
    }

    sLastState = sCurState;
    sCurState = sNextState;

    XLOG_DEBUG("[LIGHT] Last(%s) Cur(%s) Next(%s)\n", gINStStr[sLastState], gINStStr[sCurState], gINStStr[sNextState]);

    lightCtx->lightBoardCheckDelayMs = COLOR_NIGHT_SLEEP_TIME;

    return 0;
}

/**
 * @description: 供产测工具辅助控制测试灯板使用，辅助控制白灯
 * @param {LIGHT_CTX_S*} lightCtx 灯板控制上下文
 * @param {UINT32} duty 占空比
 * @return {int} 0:成功 <0:失败
 */
int LightCtl_AsstCtlWhiteLight(LIGHT_CTX_S* lightCtx, UINT32 duty)
{
    LIGHT_BOARD_S* lightBoard = NULL;

    if (!__check_ctx(lightCtx))
    {
        return -1;
    }

    lightBoard = &gLightBoards[lightCtx->lightBoradID];

    if (!lightBoard->hasWhiteLight)
    {
        XLOG_ERR("Board(%d) has no White-Light.\n", lightCtx->lightBoradID);
        return -1;
    }

    return _WhiteLightCtl(lightCtx, duty);
}

/**
 * @description: 供产测工具辅助控制测试灯板使用，辅助控制红灯
 * @param {LIGHT_CTX_S*} lightCtx 灯板控制上下文
 * @param {UINT32} duty 占空比
 * @return {int} 0:成功 <0:失败
 */
int LightCtl_AsstCtlRedLight(LIGHT_CTX_S* lightCtx, UINT32 duty)
{
    LIGHT_BOARD_S* lightBoard = NULL;

    if (!__check_ctx(lightCtx))
    {
        return -1;
    }

    lightBoard = &gLightBoards[lightCtx->lightBoradID];

    if (!lightBoard->hasRedLight)
    {
        XLOG_ERR("Board(%d) has no Red-Light.\n", lightCtx->lightBoradID);
        return -1;
    }

    return _RedLightCtl(lightCtx, duty);
}

/**
 * @description: 灯光报警控制
 * @param {LIGHT_CTX_S*} lightCtx 灯光控制上下文
 * @return {int} 0:成功 <0:失败
 */
int LightCtl_LightAlarm(LIGHT_CTX_S* lightCtx)
{
	static UINT32 s_nStayTime = 0;         /* 记录持续时间US */
    static BOOL nLastStatus = FALSE;       /* 记录灯光报警时白光灯状态 */

    if (gLightBoards[lightCtx->lightBoradID].hasWhiteLight == FALSE)
    {
        lightCtx->lightAlarmStReal = FALSE;
        return 0;
    }

    BOOL nCurStatus = lightCtx->lightAlarmStReal;

#if 0
    XLOG_DEBUG("[LIGHT] Day(%d) WhiteOpen(%d) hasWhiteLight(%d) Alarm(%d)\n",
        lightCtx->eDayNightStatus, lightCtx->isWhiteLightOpen, gLightBoards[lightCtx->lightBoradID].hasWhiteLight, lightCtx->lightAlarmEn);
#endif

    /* 白光灯闪烁报警持续时间判定 */
    if (nCurStatus == TRUE)
    {
        if (s_nStayTime > lightCtx->lightAlarmStayTime*1000*1000)
		{
			lightCtx->lightAlarmStReal = FALSE;
			s_nStayTime = 0;
		}
        else
        {
    		s_nStayTime += LIGHT_ALARM_CHECK_DELAY_MS;
        }
    }

    if (lightCtx->m_NightLedFlag != NIGNTLED_ON)
    {
        if (nCurStatus == TRUE) //开灯
        {
            if (LIGHT_ALARM_MODE_OPEN == lightCtx->lightAlarmMode) //常亮模式
            {
                _WhiteLightCtl_Alarm(lightCtx, 100);
                nLastStatus = TRUE;
            }
            else if (LIGHT_ALARM_MODE_FLICK == lightCtx->lightAlarmMode) //闪烁模式
            {
                if (FALSE == nLastStatus)
                {
                    _WhiteLightCtl_Alarm(lightCtx, 100); //开白灯
                    nLastStatus = TRUE;
                }
                else if (TRUE == nLastStatus)
                {
                    _WhiteLightCtl_Alarm(lightCtx, 0); //关白灯
                    nLastStatus = FALSE;
                }
            }
        }
        else //关灯
        {
            if (nLastStatus == TRUE)
            {
                _WhiteLightCtl_Alarm(lightCtx, 0); //关白灯
                nLastStatus = FALSE;
            }
        }
    }

    lightCtx->lightAlarmCheckDelayMs = LIGHT_ALARM_CHECK_DELAY_MS;

    return 0;
}

int LightCtl_LightBoard(LIGHT_CTX_S* lightCtx)
{
    // 上下文定义
    static LIGHTCTL_MODE_E eLastCtlMode = LIGHTCTL_MODE_PASSIVITY;
    static LIGHT_WORKMODE_E eLastWorkMode = LIGHT_WORKMODE_AUTO;
    static TRIG_LEVEL_E eLastTrigLevel = TRIG_LEVEL_HIGH;
    static int iSleepModeFlag = 0;
    // 局部变量
    BOOL reset = FALSE;
    LIGHT_BOARD_S* lightBoard = NULL;

    if (!__check_ctx(lightCtx))
    {
        return -1;
    }

    lightBoard = &gLightBoards[lightCtx->lightBoradID];

    if (1 == lightCtx->isSleepMode)
    {
        if (0 == iSleepModeFlag)
        {
            _WhiteLightCtl(lightCtx, 0);
            _RedLightCtl(lightCtx, 0);
            iSleepModeFlag = 1;
            lightCtx->colorNightMode = 0;
            lightCtx->intelligentWhiteStatue = 0;
        }
        return 0;
    }
    else
    {
        if (1 == iSleepModeFlag)
        {
            reset = TRUE;
            iSleepModeFlag = 0;
        }
    }

    if (eLastCtlMode != lightCtx->ctrlMode || eLastWorkMode != lightCtx->eWorkMode || eLastTrigLevel != lightBoard->eLightTrigLevel)
    {
        reset = TRUE;
        eLastCtlMode = lightCtx->ctrlMode;
        eLastWorkMode = lightCtx->eWorkMode;
        eLastTrigLevel = lightBoard->eLightTrigLevel;
        lightCtx->colorNightMode = 0;
        lightCtx->intelligentWhiteStatue = 0;
    }

    XLOG_DEBUG("[LIGHTCTL] CtrlMode(%d) WorkMode(%d) BoardID(%d)\n", lightCtx->ctrlMode, lightCtx->eWorkMode, lightCtx->lightBoradID);

    switch (lightCtx->ctrlMode)
    {
    case LIGHTCTL_MODE_PASSIVITY:
        if (IR_DETECT_HARDWARE == lightBoard->eIRDetType)
        {
            LightCtl_Passivity(lightCtx, reset);
        }
        else
        {
            LightCtl_NoLDRNormalRedray(lightCtx, reset);
        }
        break;

    case LIGHTCTL_MODE_DAY:
    case LIGHTCTL_MODE_NIGHT:
        LightCtl_FixDayNight(lightCtx, reset);
        break;

    case LIGHTCTL_MODE_TIME:
        LightCtl_Time(lightCtx, reset);
        break;

    case LIGHTCTL_MODE_INITIATIVE:
        if (IR_DETECT_HARDWARE == lightBoard->eIRDetType)
        {
            LightCtl_Initiative(lightCtx, reset);
        }
        else
        {
            LightCtl_NoLDRNormalRedray(lightCtx, reset);
        }
        break;

    case LIGHTCTL_MODE_COLOR_NIGHT:
        if (LIGHT_WORKMODE_TIME == lightCtx->eWorkMode)
        {
            LightCtl_ColorNightManual(lightCtx, reset);
        }
        else // LIGHT_WORKMODE_AUTO
        {
            LightCtl_ColorNight(lightCtx, reset);
        }
        break;

    case LIGHTCTL_MODE_INTELLIGENT_NIGHT:
        lightCtl_IntelligentNight(lightCtx, reset);
        break;

    default:
        lightCtx->lightBoardCheckDelayMs = LIGHT_BOARD_CHECK_DELAY_MS;
        break;
    }

    return 0;
}

/**
 * @description: 灯光控制主逻辑
 * @param {LIGHT_CTX_S*} lightCtx 灯光控制上下文
 * @return  {int} 0:成功 <0:失败
 */
int LightCtl_Progress(LIGHT_CTX_S* lightCtx)
{
    static UINT64  sLastCheckDbgMs = 0;

    if (get_ms() - sLastCheckDbgMs >= LIGHT_DBG_CHECK_DELAY_MS)
    {
        sLastCheckDbgMs = get_ms();
    }

    if (lightCtx->lightAlarmCheckDelayMs <= 0)
    {
        LightCtl_LightAlarm(lightCtx);
    }
    else
    {
        lightCtx->lightAlarmCheckDelayMs -= LIGHTCTL_PROCESS_SLEEP_MS;
    }

    if (lightCtx->lightBoardCheckDelayMs <= 0)
    {
        LightCtl_LightBoard(lightCtx);

        // 灯光报警状态同步，用于灯板从灯光报警状态中恢复
        lightCtx->lastLightAlarmStReal = lightCtx->lightAlarmStReal;
    }
    else
    {
        lightCtx->lightBoardCheckDelayMs -= LIGHTCTL_PROCESS_SLEEP_MS;
    }

    usleep(LIGHTCTL_PROCESS_SLEEP_MS);

    return 0;
}

