#include <stdlib.h>
#include <sys/stat.h>

#include "EncoderLib.h"
#include "Hi_MPP.h"
#include "Common.h"
#include "xtrace.h"
#include "opencore-amrnb/interf_enc.h"
#include "Zoom.h"
#include "Xrs_Lcd.h"

/* 全局变量 */
static int g_nCurSlowFrame = 0;     //当前慢快门帧率
static int g_nProductTest  = 0;     //工厂测试模式 0：普通模式， 1：工厂测试模式
static time_t NewSYSTime = 0;
static CChannelContext g_EncChannelContext;     //编码通道参数信息
static CTalkingChannel_t g_TalkingChannel;      //语音对讲通道信息

static int  g_SecretPinStatus = 1;              //0表示按下复位键 1表示松开复位键

#ifdef __ONE_CLICK_CALL__
// 一键呼叫按键状态位掩码定义
#define CALL_PIN_STATUS_CALL    (1 << 0)  // bit0: 呼叫按键
#define CALL_PIN_STATUS_HANGUP  (1 << 1)  // bit1: 挂断按键

static unsigned int  g_CallPinStatus = 0;
static unsigned int  g_TestCallPinStatus = 0;   //产测测试一键呼叫使用变量
#endif

/* 消除本文件中的snprintf截断警告 */
#pragma GCC diagnostic ignored "-Wformat-truncation"

//比较Osd区域是否有重叠，交叉返回1，无则返回0
int CompareModifyOsdRegion(CChannelContext *pChannelContext, ENCODE_STREAM_E eStm, int height, ENCODE_OSD_TYPE enOsdType, RECTH *rect, int index)
{
    return 0;
}

void* AudioInThread( void * lpParameter)
{
    PRT_TID_NAME("AudioInThread");
    ZW_DEBUG("%s Thread PID = %d !!!!!!!!!!!!\n", __FUNCTION__, getpid());
#if 1
    ThreadInfo_t        *pParent = (ThreadInfo_t *)lpParameter;
    SystemInfo_t *pSysInfo = NULL;
    CChannelContext *pChCtx = NULL;
    CTalkingChannel_t *pTalkingCh = NULL;

    HY_Res_GetSystemInfo(&pSysInfo);
    HY_Enc_GetTalkingChInfo(&pTalkingCh);
    HY_Enc_GetEncChInfo(&pChCtx);

    while (1)
    {
        if (pParent->m_Exit == 1)
        {
            break;
        }

        if (TRUE != pChCtx->m_bRecord)
        {
            HY_Res_SDK_Usleep(5000);
            continue;
        }

        XTRACE_MUTEX_LOCK(&pSysInfo->m_AudioInLock);

        // 开始将音频数据写入环形缓冲区
        pSysInfo->m_bAduioInStart = TRUE;

        if (0 == Hi_MPP_ReadAudioStream(pChCtx->pPlatformChannelhandle, &pChCtx->m_AudioStreamInfo, &pTalkingCh->m_TalkingIn, &pChCtx->m_audioSyncFlag))
        {
            XTRACE_MUTEX_LOCK(&pSysInfo->m_StreamCallbackLock);
            pChCtx->bAudioStreamOK = TRUE;
            pSysInfo->m_pStreamReadCallback(0);
            pChCtx->bAudioStreamOK = FALSE;
            XTRACE_MUTEX_UNLOCK(&pSysInfo->m_StreamCallbackLock);

            if ((pTalkingCh->m_bStart == TRUE) && (pSysInfo->m_pTalkingCallBack != NULL))
            {
                XTRACE_MUTEX_LOCK(&pSysInfo->m_StreamCallbackLock);
                pChCtx->bAudioStreamOK = TRUE;
                pSysInfo->m_pTalkingCallBack(0);
                pChCtx->bAudioStreamOK = FALSE;
                XTRACE_MUTEX_UNLOCK(&pSysInfo->m_StreamCallbackLock);
            }
        }

        XTRACE_MUTEX_UNLOCK(&pSysInfo->m_AudioInLock);

        HY_Res_SDK_Usleep(10*1000);
    }
#endif
    ZW_DEBUG("AudioInThread Exit \n");
    return NULL;
}

/* 读出音频数据到上层 */
void* AudioDecThread( void * lpParameter)
{
    PRT_TID_NAME("AudioDecThread");

    ThreadInfo_t        *pParent = (ThreadInfo_t *)lpParameter;
    SystemInfo_t        *pSysInfo = NULL;
    CTalkingChannel_t   *pTalkingCh = NULL;
    CChannelContext     *pChCtx = NULL;
    int                 sleepTime = 40000;
    int                 frameSize = 320;
    int                 nSpeakerMode = 0;
    int                 nSpeakerLastMode = 0;
    int                 bSpeechEnd = 0;
    char                *pAudioBuf = NULL;

    ZW_DEBUG("%s Audio Dec Thread PID = %d !!!!!!!!!!!!\n", __FUNCTION__, getpid());

    HY_Res_GetSystemInfo(&pSysInfo);
    HY_Enc_GetTalkingChInfo(&pTalkingCh);
    HY_Enc_GetEncChInfo(&pChCtx);

    while (1)
    {
        if (pParent->m_Exit == 1)
        {
            break;
        }

        XTRACE_MUTEX_LOCK(&pTalkingCh->m_Lock);
        nSpeakerMode = HY_Speaker_CalcCurrMode(pSysInfo);
        /* 如果语音对讲输出的数据缓冲区满一帧音频数据，则将数据发送到上层 */
        //开启了对讲，或触发了语音
        if (nSpeakerMode)
        {
            sleepTime = 10000;
            // 正在播报
            if (SPEECH_ENABLE == nSpeakerMode)
            {
                frameSize = pSysInfo->m_SpeechAudioFrameSize;
            }
            else // 对讲
            {
                frameSize = pSysInfo->m_AudioFrameSize;
            }

            if (pTalkingCh->m_TalkingOut.WritePos >= frameSize)
            {
                while (pTalkingCh->m_TalkingOut.WritePos > 0)
                {
                    if (SPEECH_ENABLE == nSpeakerMode)
                    {
                        if (pChCtx->m_SpeechPlayEnd == 1)
                        {
                            ZW_INFO("Speech End Flag = %d\n", pChCtx->m_SpeechPlayEnd);
                            bSpeechEnd = AUDIO_DATA_TYPE_FILE_END;
                        }
                        else
                        {
                            bSpeechEnd = AUDIO_DATA_TYPE_VALID;
                        }
                    }

                    if (pTalkingCh->m_TalkingOut.WritePos < frameSize)
                    {
                        pTalkingCh->m_TalkingOut.WritePos = 0;
                        break;
                    }

                    /* 发送音频净数据到ADEV设备 */
                    pAudioBuf = pTalkingCh->m_TalkingOut.pBuf + pTalkingCh->m_TalkingOut.ReadPos * frameSize;
                    if (SPEECH_ENABLE == nSpeakerMode)
                    {
                        Hi_MPP_SendFrameToAdev(pChCtx->pPlatformChannelhandle, pAudioBuf, frameSize, bSpeechEnd, pSysInfo->m_SpeechMode);
                    }
                    else
                    {
                        Hi_MPP_SendFrameToAdev(pChCtx->pPlatformChannelhandle, pAudioBuf, frameSize, AUDIO_DATA_TYPE_VALID, pSysInfo->m_TalkingMode);
                    }
                    pTalkingCh->m_TalkingOut.ReadPos += 1;   // 向前移动一帧数据(320),输出帧的个数加一
                    pTalkingCh->m_TalkingOut.WritePos -= frameSize;  // 输出帧缓冲池减少一个帧的大小
                }
                pTalkingCh->m_TalkingOut.WritePos = 0;
                pTalkingCh->m_TalkingOut.ReadPos  = 0;
            }
        }
        else
        {
            if (TALK_ENABLE == nSpeakerLastMode)
            {
                Hi_MPP_SendFrameToAdev(pChCtx->pPlatformChannelhandle, pAudioBuf, 0, AUDIO_DATA_TYPE_INVALID, pSysInfo->m_TalkingMode);
            }
        }
        nSpeakerLastMode = nSpeakerMode;

        XTRACE_MUTEX_UNLOCK(&pTalkingCh->m_Lock);
        HY_Res_SDK_Usleep(sleepTime);
    }

    return NULL;
}

//蓝屏帧线程
void* BuleScreenThread(void * lpParameter)
{
    PRT_TID_NAME("BuleScreenThread");

    int nSleepCnt = 0;
    stBuleScreenThreadInfo  *pParent = (stBuleScreenThreadInfo *)lpParameter;
    CChannelContext         *pChnCtx  = NULL;
    HY_Enc_GetEncChInfo(&pChnCtx);
    int index = 0;
    int count = 0;
    int nSleepUs = 0;
    int MaxSns = HY_Res_GetMaxSensorCnt();

    nSleepUs = 100000 / (MaxSns);

    while (1)
    {
        if (NULL != pParent->cbStreamReadCallback)
        {
            if (MaxSns == index)
            {
                index = 0;
            }
            pChnCtx->bUpgradeStreamOK[index] = TRUE;
            pParent->cbStreamReadCallback(0);
            pChnCtx->bUpgradeStreamOK[index] = FALSE;
        }

        //sleep 200ms(5fps)
        for (nSleepCnt = 0; nSleepCnt < MaxSns; nSleepCnt++)
        {
            if (pParent->bExit)
            {
                goto THREAD_EXIT;
                break;
            }
            usleep(nSleepUs);
        }
        count++;
        if (count % MaxSns == 0)
        {
            index++;
        }
    }

THREAD_EXIT:
    ZW_DEBUG("BuleScreenThread Exit\n");
    return NULL;
}

#ifdef __MODULE_4G_SUPPORT__
int AdjustSwitchPicFormatAndEncPram(SystemInfo_t *pSysInfo, CChannelContext        *pChCtx)
{
    static ADJUST_ENCPRAM_STATUS nLastEncPramStatus = STATUS_NORMAL;

    int s32Ret = 0;
    unsigned short int PicWidth, PicHeight;

    if (pSysInfo->m_NicStatus == eXRS_NETCARD_WIRED_1)
    {
        pSysInfo->nCurAdustEncPramStatus = STATUS_NORMAL;
    }
    else if (access(PRODUCTTEST_FILE, F_OK) == 0)
    {
        pSysInfo->nCurAdustEncPramStatus = STATUS_PRODUCT_TEST;
    }
    else if ((pSysInfo->m_4GTelecomOperatorType == eChinaMobile) && (pSysInfo->m_NicStatus == eXRS_NETCARD_4G))
    {
        pSysInfo->nCurAdustEncPramStatus = STATUS_CHINAMOBILE;
    }
    else if (access(P2P_SEND_DATA_FAIL_FLAG, F_OK) == 0)
    {
        pSysInfo->nCurAdustEncPramStatus = STATUS_SEND_DATA_FAIL;
    }
    else
    {
        pSysInfo->nCurAdustEncPramStatus = STATUS_NORMAL;
    }

    /* 方便测试代码 */
    if (access("/mnt/mtd/AdustEncPramStatusTest", F_OK) == 0)
    {
        int valuetest = 0;
        if (get_integer_from_file("/mnt/mtd/AdustEncPramStatusTest", &valuetest) == 0)
        {
            pSysInfo->nCurAdustEncPramStatus = valuetest;
        }
    }

    if (pSysInfo->nCurAdustEncPramStatus == nLastEncPramStatus)
    {
        return 0;
    }
    ZW_PRINTF("EncPramStatus [%d]-->[%d]\n", nLastEncPramStatus, pSysInfo->nCurAdustEncPramStatus);
    nLastEncPramStatus = pSysInfo->nCurAdustEncPramStatus;

    switch (pSysInfo->nCurAdustEncPramStatus)
    {
        case STATUS_PRODUCT_TEST:
        case STATUS_CHINAMOBILE:
        case STATUS_SEND_DATA_FAIL:
            PicWidth = VSIZE_720P_WIDTH;
            PicHeight = VSIZE_720P_HEIGHT;
            break;
        case STATUS_NORMAL:
        default:
            PicWidth =  VSIZE_1080P_WIDTH;
            PicHeight = VSIZE_1080P_HEIGHT;

            break;
    }

    /* 1、修改分辨率 */
    if (pChCtx->m_PictureRes[FIRST_STREAM].m_PicWidth != PicWidth || pChCtx->m_PictureRes[FIRST_STREAM].m_PicHeight != PicHeight)
    {
        /* 1、修改分辨率 */
        /* 此处修改分辨率有双逻辑是因为G84上发现 大分辨切小分辨率不用重置SDK，小切大则需要，否则会失败 */
        if (PicWidth == VSIZE_720P_WIDTH)
        {
        	UserSetParamFlag_t m_UserSetParam = {0};
            // 停止用户设置
    		HY_Enc_StopUserSet(pChCtx, FIRST_STREAM, &m_UserSetParam);
            XTRACE_MUTEX_LOCK(&pChCtx->m_SensorLock);
            XTRACE_MUTEX_LOCK(&pChCtx->m_EncLock);
            XTRACE_MUTEX_LOCK(&pSysInfo->m_SysOSDLock);
            XTRACE_MUTEX_LOCK(&pChCtx->m_InitVPSSLock);
            XTRACE_MUTEX_LOCK(&pChCtx->m_CheckLumLock);

			s32Ret = Hi_MPP_ClearDrawLineInfo(1);

            pChCtx->m_PictureRes[FIRST_STREAM].m_PicWidth = PicWidth;
            pChCtx->m_PictureRes[FIRST_STREAM].m_PicHeight = PicHeight;
            pChCtx->m_EncAttr.PictureRes.m_PicWidth = PicWidth;
            pChCtx->m_EncAttr.PictureRes.m_PicHeight = PicHeight;

            /*更新用户设置的分辨率*/
            Hi_MPP_UpdatePictureFormat(pChCtx->pPlatformChannelhandle,
                &pChCtx->m_EncAttr.PictureRes,  &pChCtx->m_EncAttrAux.PictureRes,
                pChCtx->m_bSupportFourthStream, &pChCtx->m_EncAttrFourth.PictureRes);

            s32Ret = Hi_MPP_SwitchPicFormat(pChCtx->pPlatformChannelhandle, FIRST_STREAM);
            if (s32Ret < 0)
            {
                ZW_ERROR("Main Fail to called HY_Enc_SwitchPicFormat! ret = %#x\n", s32Ret);
            }

            XTRACE_MUTEX_UNLOCK(&pChCtx->m_CheckLumLock);
            XTRACE_MUTEX_UNLOCK(&pChCtx->m_InitVPSSLock);
            XTRACE_MUTEX_UNLOCK(&pSysInfo->m_SysOSDLock);
            XTRACE_MUTEX_UNLOCK(&pChCtx->m_EncLock);
            XTRACE_MUTEX_UNLOCK(&pChCtx->m_SensorLock);
			// 启用用户设置
    		HY_Enc_StartUserSet(pChCtx, FIRST_STREAM, &m_UserSetParam);
        }
        else
        {
            XTRACE_MUTEX_LOCK(&pSysInfo->m_SysOSDLock);
            pChCtx->m_PictureRes[FIRST_STREAM].m_PicWidth = PicWidth;
            pChCtx->m_PictureRes[FIRST_STREAM].m_PicHeight = PicHeight;
            pChCtx->m_EncAttr.PictureRes.m_PicWidth = PicWidth;
            pChCtx->m_EncAttr.PictureRes.m_PicHeight = PicHeight;
            XTRACE_MUTEX_LOCK(&pSysInfo->m_WriteFileLock);
            s32Ret = HY_Enc_SwitchPicFormat(pChCtx, FIRST_STREAM);
            if (s32Ret < 0)
            {
                ZW_ERROR("Main Fail to called HY_Enc_SwitchPicFormat! ret = %#x\n", s32Ret);
            }

            XTRACE_MUTEX_UNLOCK(&pSysInfo->m_WriteFileLock);
            XTRACE_MUTEX_UNLOCK(&pSysInfo->m_SysOSDLock);
        }

        // 如果SmartP开启了，则切换分辨率后强制下I帧
        if (pChCtx->m_EncAttr.EncType == LtH265PlusVideo)
        {
            Hi_MPP_ForceIFrame(pChCtx, -1);
        }
    }

    /* 2、修改码流参数 */
    XTRACE_MUTEX_LOCK(&pChCtx->m_VencLock);
    Hi_MPP_SetEncPara(pChCtx->pPlatformChannelhandle, &pChCtx->m_EncAttr, FIRST_STREAM, 1, pChCtx->m_iSlowFrameMode);
    Hi_MPP_SetEncPara(pChCtx->pPlatformChannelhandle, &pChCtx->m_EncAttrAux, SECOND_STREAM, 1, pChCtx->m_iSlowFrameMode);
    XTRACE_MUTEX_UNLOCK(&pChCtx->m_VencLock);

    return s32Ret;
}
#endif

void *UpdateOSDThread(void *lpParameter)
{
    PRT_TID_NAME("UpdateOSDThread");
    ZW_DEBUG("Update OSD Thread PID = %d !!!!!!!!!!!!\n", getpid());

    ThreadInfo_t        *pParent = NULL;
    SystemInfo_t        *pSysInfo = NULL;
    CChannelContext        *pChCtx = NULL;
    CPlatformChannelContext *pPlatCtx = NULL;

    if (NULL == lpParameter)
    {
        ZW_ERROR("lpParameter CANNOT be NULL\n");
        return NULL;
    }

    pParent = (ThreadInfo_t *)lpParameter;

    time_t CurTime = 0, LastTime = 0;
    int bNeedRestartOsd = 0;

    int s32Ret = 0;
    int i = 0;
    int MaxSns = HY_Res_GetMaxSensorCnt();

    time(&CurTime);
    time(&LastTime);

    HY_Res_GetSystemInfo(&pSysInfo);
    HY_Enc_GetEncChInfo(&pChCtx);
    pPlatCtx = (CPlatformChannelContext    *)pChCtx->pPlatformChannelhandle;

    //等待IRCUT切换两次操作结束
    while (1)
    {
        if (pParent->m_Exit == 1)
        {
            break;
        }

        usleep(500 * 1000);
        XTRACE_MUTEX_LOCK(&pChCtx->m_IrcutLock);
        if (pChCtx->m_IrcutSwitchFlag == 1)
        {
            XTRACE_MUTEX_UNLOCK(&pChCtx->m_IrcutLock);
            break;
        }
        else
            XTRACE_MUTEX_UNLOCK(&pChCtx->m_IrcutLock);
    }

    while (1)
    {
        if (pParent->m_Exit == 1)
        {
            break;
        }

        /* 刷新时间OSD信息 */
        time(&CurTime);
        if (CurTime - LastTime)
        {
            XTRACE_MUTEX_LOCK(&pSysInfo->m_SysOSDLock);
            XTRACE_MUTEX_LOCK(&pChCtx->m_OSDLock);

            //刷新时间OSD信息
            if (pChCtx->m_bEnableTimeOSD)
            {
                for (i = 0; i < MAX_STREAM_COUNT; i++)
                {
                    if (i != FIRST_STREAM && i != SECOND_STREAM && i != FOURTH_STREAM)
                    {
                        continue;
                    }

#if defined __ISP_IMAGE_DEBUG__
                    if (i != FIRST_STREAM)
                    {
                        continue;
                    }
#endif

                    for (unsigned int j = 0; j < MaxSns; j++)
                    {
                        if (0 == (s32Ret = Hi_MPP_GetTimeBitmap(pPlatCtx, &CurTime, j, i)))
                        {
                            Hi_MPP_SetOSD(pPlatCtx, j, i, TIME_TYPE, pChCtx->m_bEnableTimeOSD);
                        }
                        else
                        {
                            if (-2 == s32Ret)
                            {//获取底层地址失败，重置osd
                                bNeedRestartOsd = 1;
                                break;
                            }
                        }
                    }
                }
            }

            // 刷新文本信息，如果不支持OSDV2则无需刷新
            if (0 == bNeedRestartOsd)
            {
                if (pChCtx->m_bEnableTextOSD && pChCtx->m_bCreatTextOSDRegion)
                {
                    s32Ret = Hi_MPP_GetOSDV2Bitmap(pPlatCtx);
                    if (-2 == s32Ret)
                    {//获取底层地址失败，重置osd
                        bNeedRestartOsd = 1;
                        ZW_ERROR("!!!!Hi_MPP_GetOSDV2Bitmap return faild\n");
                        XTRACE_MUTEX_UNLOCK(&pChCtx->m_OSDLock);
                        XTRACE_MUTEX_UNLOCK(&pSysInfo->m_SysOSDLock);
                        break;
                    }
                }
            }

            if (bNeedRestartOsd)
            {
                HY_Enc_RestartOsd(pChCtx);
                bNeedRestartOsd = 0;
            }

            XTRACE_MUTEX_UNLOCK(&pChCtx->m_OSDLock);
            XTRACE_MUTEX_UNLOCK(&pSysInfo->m_SysOSDLock);
            LastTime = CurTime;
        }

        #ifdef __MODULE_4G_SUPPORT__
        AdjustSwitchPicFormatAndEncPram(pSysInfo, pChCtx);
        #endif

        HY_Res_SDK_Usleep(100 * 1000);
    }

    ZW_DEBUG("UpdateOSDThread Exit \n");
    return NULL;
}

//二维码扫描功能使能
int HY_EnableQrScan(int bEnable)
{
    SystemInfo_t *pSysInfo = NULL;
    HY_Res_GetSystemInfo(&pSysInfo);

    if (bEnable != pSysInfo->m_bIsQrScanEnable)
    {
        pSysInfo->m_bIsQrScanEnable = bEnable;
    }

    return 0;
}

// 调整夜晚下人形检测阈值，暂只做黑光模式。
int AdjustNightAlgoThreshold(UINT32 nVarWhiteGainFlag)
{
    CChannelContext     *pChCtx = NULL;
    CPlatformChannelContext *pPlatCtx = NULL;

    HY_Enc_GetEncChInfo(&pChCtx);
    pPlatCtx = (CPlatformChannelContext *)pChCtx->pPlatformChannelhandle;

    static double algoThr = ALGO_THRESHOLD_NORMAL;
    static double algoThrLast = ALGO_THRESHOLD_NORMAL;

    static int recoverCount = 0;

    // 黑光模式：无极白手动阈值，且白灯亮度配置为0
    if ((IRCUT_INTELLIGENT_WHITELIGHT == pChCtx->DayNightMode) && (WORKMODE_CUSTOM == pChCtx->VarWhiteMode) && (CUSTOMMODE_THR == pChCtx->VarWhiteCustomMode))
    {
        // 只在(夜晚)且(没有人形触发亮灯时)做检测，调高灵敏度
        if (nVarWhiteGainFlag && ((pChCtx->m_stLightCtx.whiteLightBrightness == pChCtx->VarWhiteCustomBright) && (0 == pChCtx->VarWhiteCustomBright)))
        {
            if (g_nCurSlowFrame < ALGO_CHANGE_THR_FRM)
            {
                // 调更灵敏
                algoThr = ALGO_THRESHOLD_NIGHT;
            }
            else
            {
                // 忽略人形触发亮灯后导致的帧率变高
                recoverCount ++;
                if (recoverCount > 250)
                {
                    // 还原
                    algoThr = ALGO_THRESHOLD_NORMAL;
                    recoverCount = 0;
                }
            }
        }
        else if (!nVarWhiteGainFlag || ((pChCtx->m_stLightCtx.whiteLightBrightness == pChCtx->VarWhiteCustomBright) && (0 != pChCtx->VarWhiteCustomBright)))
        {
            // 还原
            algoThr = ALGO_THRESHOLD_NORMAL;
            recoverCount = 0;
        }
    }
    else
    {
        algoThr = ALGO_THRESHOLD_NORMAL;
        recoverCount = 0;
    }

    if (algoThrLast != algoThr)
    {
        //生效
        pChCtx->m_AlgoThr = algoThr;
        Hi_MPP_LoadAlgoParame(pPlatCtx);

        algoThrLast = algoThr;
    }

    return 0;
}

static int GetNightLedFlagChange()
{
    CChannelContext *pChContext = NULL;
    HY_Enc_GetEncChInfo(&pChContext);

    if (NIGNTLED_START == pChContext->m_NightLedFlag)
    {
        pChContext->m_NightLedFlag = NIGNTLED_ON;
        return NIGNTLED_START;
    }
    else if (NIGNTLED_ON == pChContext->m_NightLedFlag)
    {
        return NIGNTLED_ON;
    }
    else if (NIGNTLED_STOP == pChContext->m_NightLedFlag)
    {
        pChContext->m_NightLedFlag = NIGNTLED_NULL;
        return NIGNTLED_STOP;
    }

    return 0;
}

#if 1
int LightCtl_OneKeyNigntLedProcess(CChannelContext *pChannelContext)
{
    int ret = 0;

    static int NightLedTime = 0;
    static int iDayNightStutasWhenNightledStart = -1; //-1:无效 1:白天 0:夜晚

    ret = GetNightLedFlagChange();

    if (pChannelContext->DayNightMode == IRCUT_COLOR_NIGHT && pChannelContext->m_stLightCtx.isWhiteLightOpen)
    {
        return 0;
    }

    if (ret == NIGNTLED_START)
    {
        NightLedTime = ((pChannelContext->m_NightLedTime * 10)) + 1;
        if (-1 == iDayNightStutasWhenNightledStart)
        {
            iDayNightStutasWhenNightledStart = pChannelContext->m_DayNightStutas;
        }

        /* 打开白光灯，关闭红外灯 */
        if (pChannelContext->m_IrPin3Default == 0)
        {
            Hi_MPP_SetWhiteLight(100,20);
            Hi_MPP_SetRedLight(0,20);
        }
        else
        {
            Hi_MPP_SetWhiteLight(0,20);
            Hi_MPP_SetRedLight(0,20);
        }

        usleep(50*1000);

        /* 切换到彩色 */
        XTRACE_MUTEX_LOCK(&pChannelContext->m_SensorLock);
        HY_Enc_SetDayNightMode(pChannelContext, 1);
        XTRACE_MUTEX_UNLOCK(&pChannelContext->m_SensorLock);
    }
    else if (ret == NIGNTLED_STOP && ( NightLedTime > 1 ))
    {
        NightLedTime = 1;
    }

    if (NightLedTime > 0)
    {
        #if 0
        /* 中间异常熄灯需要重新开启 */
        if (GetPin3Status() == 0)
        {
            if (pChannelContext->m_IrPin3Default == 0)
            {
                Hi_MPP_SetWhiteLight(1,20);
                Hi_MPP_SetRedLight(0,20);
            }
            else
            {
                Hi_MPP_SetWhiteLight(0,20);
                Hi_MPP_SetRedLight(1,20);
            }
        }
        #endif

        NightLedTime--;

        /* 结束夜灯,恢复之前双光源检测状态 */
        if (NightLedTime == 0)
        {
            if (ISDAY == iDayNightStutasWhenNightledStart /*&& (1 != g_Colornight_nightmode && (2 != colorNight_status && 4 != colorNight_status))*/)
            {
                if (pChannelContext->m_IrPin3Default == 0)
                {
                    Hi_MPP_SetWhiteLight(0,20);
                    Hi_MPP_SetRedLight(0,20);
                }
                else
                {
                    Hi_MPP_SetWhiteLight(100,20);
                    Hi_MPP_SetRedLight(100,20);
                }
            }
            else if (ISNIGHT == iDayNightStutasWhenNightledStart)
            {
                if (pChannelContext->m_IrPin3Default == 0)
                {
                    Hi_MPP_SetWhiteLight(0,20);
                    Hi_MPP_SetRedLight(100,20);
                }
                else
                {
                    Hi_MPP_SetWhiteLight(100,20);
                    Hi_MPP_SetRedLight(0,20);
                }

                XTRACE_MUTEX_LOCK(&pChannelContext->m_SensorLock);
                HY_Enc_SetDayNightMode(pChannelContext, 0);
                XTRACE_MUTEX_UNLOCK(&pChannelContext->m_SensorLock);
            }

            //usleep(100 * 1000);
            //HY_Enc_SetRecoveryAudioAlarm();
            iDayNightStutasWhenNightledStart = -1;
            pChannelContext->m_NightLedFlag = NIGNTLED_STOP;
        }
    }

    return NightLedTime;
}
#endif

void* CheckDayAndNightThread(void * lpParameter)
{
    PRT_TID_NAME("CheckDayAndNightThread");
    ZW_DEBUG("%s Thread PID = %d !!!!!!!!!!!!\n", __FUNCTION__, getpid());

    ThreadInfo_t            *pParent = (ThreadInfo_t *)lpParameter;
    SystemInfo_t            *pSysInfo = NULL;
    CChannelContext         *pChCtx = NULL;
    //CPlatformChannelContext *pPlatCtx = NULL;

    HY_Enc_GetEncChInfo(&pChCtx);

    //pPlatCtx = (CPlatformChannelContext *)pChCtx->pPlatformChannelhandle;

    /* 获取系统状态信息 */
    HY_Res_GetSystemInfo(&pSysInfo);

    SINT32 RtVal = 0;
    static int TuneServe = 1;
    static int AutoSceneFlag = 0;
    DAYNIGHT_T  bLastDayNightStutas = ISDAY;
    DAYNIGHT_T  curDayNightStutas = ISDAY;
    VISP_SCENE_MODE_E sceneMode = VISP_SCENE_MODE_NORMAL;
#if 0
    UINT32 nVarWhiteGainFlag = 0;//是否是夜晚 无极白手动阈值
    UINT32 nWhiteLightSelfCheck = 1;//无极白是否自检
    UINT32 nRedLightSelfCheck = 1;//无极红是否自检
    UINT64 sNightStartMs = 0; // 切换夜晚效果的开始时间

    ISP_AE_WEIGHT_ATTR_S AEweightBak = {0};
    ISP_ATLIST_S AtListAttrBak = {0};

    //先获取默认值，备份以恢复用
    Visp_Get_AEWeight(0, &AEweightBak);
#endif

    while (1)
    {
        if (1 == pParent->m_Exit)
        {
            break;
        }

        // 开放图像调试
        if (0 == access(P2PUDAnyFile, F_OK) && TuneServe)
        {
            TuneServe = 0;
            RtVal = Visp_Start_TuneServe(0);
            ZW_INFO("\n ***Visp_Start_TuneServe RtVal=(%#x)\n", RtVal);
        }

        // 控制 autoScene
        if (0 == access(AutoSceneStopFlag, F_OK) && (0 == AutoSceneFlag))
        {
            // 停止 autuScene
            Visp_Pause_SceneAuto(0, 1);
            AutoSceneFlag = 1;
        }
        else if (0 != access(AutoSceneStopFlag, F_OK) && AutoSceneFlag)
        {
            // 继续 autuScene
            Visp_Pause_SceneAuto(0, 0);
            AutoSceneFlag = 0;
        }

        XTRACE_MUTEX_LOCK(&pChCtx->m_DayNightLock);
        curDayNightStutas = pChCtx->m_DayNightStutas;
        XTRACE_MUTEX_UNLOCK(&pChCtx->m_DayNightLock);

        // 图像参数切换 [两份图像：彩色 黑白]
        if (curDayNightStutas != bLastDayNightStutas)
        {
            if (ISNIGHT == curDayNightStutas)
            {
                sceneMode = VISP_SCENE_MODE_BLIGHT;
                //sNightStartMs = get_ms();
            }
            else
            {
                sceneMode = VISP_SCENE_MODE_NORMAL;
            }

            XTRACE_MUTEX_LOCK(&pChCtx->m_SensorLock);
            XTRACE_MUTEX_LOCK(&pChCtx->m_EncLock);

            do {
                RtVal = Visp_Switch_SceneMode(0, sceneMode);
                if (RtVal < 0)
                {
                    ZW_ERROR("Visp_Switch_SceneMode failed\n");
                    break;
                }

                /*加载效果文件后需要延时, 否则用户参数不生效, 画面会变成紫色*/
                usleep(30 * 1000);

                RtVal = Hi_MPP_ResetIspParam();
                if (RtVal < 0)
                {
                    ZW_ERROR("Hi_MPP_ResetIspParam failed\n");
                    break;
                }

            } while(0);

            XTRACE_MUTEX_UNLOCK(&pChCtx->m_EncLock);
            XTRACE_MUTEX_UNLOCK(&pChCtx->m_SensorLock);

            if (ISNIGHT == curDayNightStutas)
            {
                //避免夜晚图像还没加载完毕先把ircut切了，导致图像先变红再恢复正常
                usleep(100*1000);
                XTRACE_MUTEX_LOCK(&pChCtx->m_IrcutLock);
                ZW_INFO("===Now set night mode!===\n");
                pChCtx->m_IrcutState = 2;//night
                pChCtx->m_IrcutStateHis = pChCtx->m_IrcutState;
                XTRACE_MUTEX_UNLOCK(&pChCtx->m_IrcutLock);
            }

            bLastDayNightStutas = curDayNightStutas;
        }

        // 用户设置图像重置
        if (pChCtx->ResetUserPic)
        {
            XTRACE_MUTEX_LOCK(&pChCtx->m_SensorLock);
            XTRACE_MUTEX_LOCK(&pChCtx->m_EncLock);
            RtVal = Hi_MPP_ResetIspParam();
            XTRACE_MUTEX_UNLOCK(&pChCtx->m_EncLock);
            XTRACE_MUTEX_UNLOCK(&pChCtx->m_SensorLock);
            if (RtVal < 0)
            {
                ZW_ERROR("Hi_MPP_ResetIspParam failed\n");
            }
            else
            {
                pChCtx->ResetUserPic = FALSE;
            }
        }

        usleep(10 * 1000);
    }

    return NULL;
}

static void _GetISPInfo(IR_ISP_INFO_S *LightIrInfo)
{
    Hi_MPP_GetCurrDayNightStatus(LightIrInfo);
}

static void _GetCurSlowFrame(UINT32 *slowFrame)
{
    *slowFrame = g_nCurSlowFrame;
}

static void _SetWhiteLight(LIGHT_OP_MODE_E mode, UINT32 duty, UINT32 freqKHz)
{
    Hi_MPP_SetWhiteLight(duty, freqKHz);
}

static void _SetRedLight(LIGHT_OP_MODE_E mode, UINT32 duty, UINT32 freqKHz)
{
    Hi_MPP_SetRedLight(duty, freqKHz);
}

static void _GetHwIRStatus(IR_STATUS_E *irStatus)
{
    UINT32 status = 0;

    Hi_MPP_GetIRStateLevel(&status);
    *irStatus = status?IR_STATUS_DAY:IR_STATUS_NIGHT;
}

static void _ISPCtrl(LIGHTCTL_IRCUT_MODE_E mode)
{
    CChannelContext *pChCtx = NULL;
    HY_Enc_GetEncChInfo(&pChCtx);

    //ZW_DEBUG("[LIGHT] ISP Set to %s\n", mode == LIGHTCTL_IRCUT_DAY ? "DAY" : "NIGHT");

    XTRACE_MUTEX_LOCK(&pChCtx->m_SensorLock);
    HY_Enc_SetDayNightMode(pChCtx, (LIGHTCTL_IRCUT_DAY == mode)?ISDAY:ISNIGHT);
    XTRACE_MUTEX_UNLOCK(&pChCtx->m_SensorLock);
}

static void _GetAIStatus(BOOL lastTrig, BOOL *trig, BOOL rst)
{
    CChannelContext *pChCtx = NULL;

    HY_Enc_GetEncChInfo(&pChCtx);

    if (rst)
    {
        pChCtx->m_bHumanStateIsValue = FALSE;
        pChCtx->m_bMotionStateIsValue = FALSE;
    }

    *trig = lastTrig;

    if (pChCtx->m_bHumanStateIsValue)
    {
        pChCtx->m_bHumanStateIsValue = FALSE;
        *trig = pChCtx->m_bHumanStateEx;
    }
    else if (pChCtx->m_bMotionStateIsValue)
    {
        pChCtx->m_bMotionStateIsValue = FALSE;
        *trig = pChCtx->m_bMotionStateEx;
    }

    return;
}

static LIGHT_BOARD_TYPE_E _GetLightBoardType(UINT32 type, TRIG_LEVEL_E trigLevel)
{
    LIGHT_BOARD_TYPE_E eLightType = LIGHT_BOARD_NONE;

    if (1 == type) // 硬光敏普通红外
    {
        if (TRIG_LEVEL_HIGH == trigLevel)
        {
            eLightType = LIGHT_BOARD_INFRARED_HIR_HIGH;
        }
        else
        {
            eLightType = LIGHT_BOARD_INFRARED_HIR_LOW;
        }
    }
    else if (3 == type) // 无极白光
    {
        if (TRIG_LEVEL_HIGH == trigLevel)
        {
            eLightType = LIGHT_BOARD_INFINITY_WL_SIR_HIGH;
        }
        else
        {
            eLightType = LIGHT_BOARD_INFINITY_WL_SIR_LOW;
        }
    }
    else if (4 == type) // 软光敏红外
    {
        if (TRIG_LEVEL_HIGH == trigLevel)
        {
            eLightType = LIGHT_BOARD_INFRARED_SIR_HIGH;
        }
        else
        {
            eLightType = LIGHT_BOARD_INFRARED_SIR_LOW;
        }
    }
    else if (6 == type) // 硬光敏双光
    {
        if (TRIG_LEVEL_HIGH == trigLevel)
        {
            eLightType = LIGHT_BOARD_DOUBLE_LIGHT_HIR_HIGH;
        }
        else
        {
            eLightType = LIGHT_BOARD_DOUBLE_LIGHT_HIR_LOW;
        }
    }
    else if (7 == type) // 软光敏双光
    {
        if (TRIG_LEVEL_HIGH == trigLevel)
        {
            eLightType = LIGHT_BOARD_DOUBLE_LIGHT_SIR_HIGH;
        }
        else
        {
            eLightType = LIGHT_BOARD_DOUBLE_LIGHT_SIR_LOW;
        }
    }

    return eLightType;
}

static int _LightCtl_Init(LIGHT_CTX_S *lightCtx)
{
    CChannelContext *pChCtx = NULL;
    SystemInfo_t *pSysInfo = NULL;

    if (!lightCtx)
    {
        return -1;
    }

    HY_Enc_GetEncChInfo(&pChCtx);
    HY_Res_GetSystemInfo(&pSysInfo);

    lightCtx->BoardInfo = pSysInfo->m_BoardInfo;


    lightCtx->eSnrType = SENSOR_TYPE_Hi_JXF38P;
#if    defined(__PRODUCT_P4__)||defined(__PRODUCT_P4U__)
    lightCtx->D2NFactor = 16800000;    //彩色 1.6ux
    lightCtx->N2DFactor = 277000;      //黑白 4-6lux
#else
    lightCtx->D2NFactor = 1300000;    //彩色 1.6ux
    lightCtx->N2DFactor = 46000;      //黑白 4-6lux
#endif

    lightCtx->GetISPInfo = _GetISPInfo;
    lightCtx->IspCtrl = _ISPCtrl;
    lightCtx->WhiteLightCtrl = _SetWhiteLight;
    lightCtx->RedLightCtrl = _SetRedLight;
    lightCtx->GetHardwareIRStatus = _GetHwIRStatus;
    lightCtx->GetAIStatus = _GetAIStatus;
    lightCtx->lightBoradID = _GetLightBoardType(pSysInfo->m_LightBoardTypeID, pChCtx->m_IrPin3Default?TRIG_LEVEL_LOW:TRIG_LEVEL_HIGH);
    lightCtx->GetSlowFrame = _GetCurSlowFrame;

    LightCtl_Init(lightCtx);

    return 0;
}

static int _LightCtl_Update(LIGHT_CTX_S *lightCtx)
{
    CChannelContext *pChCtx = NULL;
    SystemInfo_t *pSysInfo = NULL;

    if (!lightCtx)
    {
        return -1;
    }

    HY_Enc_GetEncChInfo(&pChCtx);
    HY_Res_GetSystemInfo(&pSysInfo);

    //现产测工具定制灯板支持即时生效、不用重启;
    lightCtx->lightBoradID = _GetLightBoardType(pSysInfo->m_LightBoardTypeID, pChCtx->m_IrPin3Default?TRIG_LEVEL_LOW:TRIG_LEVEL_HIGH);

    lightCtx->ctrlMode = (LIGHTCTL_MODE_E)pChCtx->DayNightMode;
    if (LIGHTCTL_MODE_INTELLIGENT_WHITELIGHT == lightCtx->ctrlMode)
    {
        if (WORKMODE_CUSTOM == pChCtx->VarWhiteMode)
        {
            lightCtx->eWorkMode = (pChCtx->VarWhiteCustomMode == CUSTOMMODE_TIME) ? LIGHT_WORKMODE_TIME : LIGHT_WORKMODE_THR;
        }
        else
        {
            lightCtx->eWorkMode = LIGHT_WORKMODE_AUTO;
        }
    }
    else if (LIGHTCTL_MODE_COLOR_NIGHT == lightCtx->ctrlMode)
    {
        lightCtx->eWorkMode = (pChCtx->ColorNightMode == WORKMODE_AUTO) ? LIGHT_WORKMODE_AUTO : LIGHT_WORKMODE_TIME;
    }
    else if (LIGHTCTL_MODE_INTELLIGENT_REDRAY == lightCtx->ctrlMode)
    {
        lightCtx->eWorkMode = (pChCtx->m_InfraredWorkMode == WORKMODE_AUTO) ? LIGHT_WORKMODE_AUTO : LIGHT_WORKMODE_THR;
    }

    lightCtx->dayNightSwitchTime = pChCtx->m_DayNightSwitchTime;
    lightCtx->initiativeModeTh = pChCtx->m_InitiativeModeSensity;
    lightCtx->colorNightContinueTimeMs = pChCtx->m_colorNightLastTime;
    lightCtx->m_NightLedFlag = pChCtx->m_NightLedFlag;

    // TODO:可否改为内部管理，外部使用接口获取状态？
    lightCtx->eDayNightStatus = pChCtx->m_DayNightStutas == ISDAY ? LIGHTCTL_IRCUT_DAY : LIGHTCTL_IRCUT_NIGHT;

    lightCtx->usePtzCtrl = pChCtx->m_PtzCtrlIrcut;
    lightCtx->ptzCtrlStatus = pChCtx->m_PtzSetDayOrNight ? LIGHTCTL_IRCUT_NIGHT : LIGHTCTL_IRCUT_DAY;
    lightCtx->ptzStatus = ((pChCtx->m_enPtzStatus == ePtzStatus_Stop) ? LIGHT_PTZ_STOP : LIGHT_PTZ_MOVING);

    // 灯光报警
    lightCtx->lightAlarmMode = LIGHT_ALARM_MODE_FLICK;

    // 时间得配置会被多个模式复用
    if (LIGHTCTL_MODE_INTELLIGENT_WHITELIGHT == lightCtx->ctrlMode)
    {
        lightCtx->ctlTime.startSec = pChCtx->m_ircutVarWhiteModeInfo.nStartHour * 3600 \
                                    + pChCtx->m_ircutVarWhiteModeInfo.nStartMin * 60 \
                                    + pChCtx->m_ircutVarWhiteModeInfo.nStartSec;
        lightCtx->ctlTime.stopSec = pChCtx->m_ircutVarWhiteModeInfo.nStopHour * 3600 \
                                    + pChCtx->m_ircutVarWhiteModeInfo.nStopMin * 60 \
                                    + pChCtx->m_ircutVarWhiteModeInfo.nStopSec;
    }
    else if (LIGHTCTL_MODE_TIME == lightCtx->ctrlMode)
    {
        lightCtx->ctlTime.startSec = pChCtx->m_ircutTimeModeInfo.nStartHour * 3600 \
                                    + pChCtx->m_ircutTimeModeInfo.nStartMin * 60 \
                                    + pChCtx->m_ircutTimeModeInfo.nStartSec;
        lightCtx->ctlTime.stopSec = pChCtx->m_ircutTimeModeInfo.nStopHour * 3600 \
                                    + pChCtx->m_ircutTimeModeInfo.nStopMin * 60 \
                                    + pChCtx->m_ircutTimeModeInfo.nStopSec;
    }
    else if (LIGHTCTL_MODE_COLOR_NIGHT == lightCtx->ctrlMode)
    {
        lightCtx->ctlTime.startSec = pChCtx->m_ircutColorNightModeInfo.nStartHour * 3600 \
                                    + pChCtx->m_ircutColorNightModeInfo.nStartMin * 60 \
                                    + pChCtx->m_ircutColorNightModeInfo.nStartSec;
        lightCtx->ctlTime.stopSec = pChCtx->m_ircutColorNightModeInfo.nStopHour * 3600 \
                                    + pChCtx->m_ircutColorNightModeInfo.nStopMin * 60 \
                                    + pChCtx->m_ircutColorNightModeInfo.nStopSec;
    }

    lightCtx->isSleepMode = pSysInfo->m_iDormancyState;

    return 0;
}

/**
 * @description: 初始化灯板控制模块
 * @return {int} 0: 成功 <0: 失败
 */
int HY_LightCtlInit()
{
    CChannelContext* pChCtx = NULL;

    HY_Enc_GetEncChInfo(&pChCtx);

    _LightCtl_Init(&pChCtx->m_stLightCtx);

    _LightCtl_Update(&pChCtx->m_stLightCtx);

    return 0;
}


#if defined (__IS_SUPPORT_ADUJUST_SENSOR_FRAME__)

#if 0
static VENC_CHNTYPE_E __KS_VENC_CHN_TYPE(DEV_SUPPORT_SENSOR SensorIndex, ENCODE_STREAM_E eStm)
{
    VENC_CHNTYPE_E eChnType = VENC_CHNTYPE_BUTT;

    if (FIRST_SENSOR == SensorIndex)
    {
        switch (eStm)
        {
        case FIRST_STREAM:
            eChnType = VENC_CHNTYPE_MAIN;
            break;
        case SECOND_STREAM:
            eChnType = VENC_CHNTYPE_MINOR;
            break;
        default:
            ZW_ERROR("NOW ONLY SUPPORT Main and Sub Stream [%d]\n", SensorIndex);
            break;
        }
    }
    else if (SEC_SENSOR == SensorIndex)
    {
        switch (eStm)
        {
        case FIRST_STREAM:
            eChnType = VENC_CHNTYPE_MAIN_SEC;
            break;
        case SECOND_STREAM:
            eChnType = VENC_CHNTYPE_MINOR_SEC;
            break;
        default:
            ZW_ERROR("NOW ONLY SUPPORT Main and Sub Stream [%d]\n", SensorIndex);
            break;
        }
    }

    return eChnType;
}
#endif

#if defined (__PRODUCT_P4__)||defined(__PRODUCT_P4U__)
#define SENSOR_SLOW_FRAME_RATE 10
#else
#define SENSOR_SLOW_FRAME_RATE 8
#endif
static int _LightCtl_Frame(SystemInfo_t *pInfo, CChannelContext *pChCtx, LIGHT_CTX_S* lightCtx)
{
    int i = 0;
    int ret = 0;
#if 0
    int maxFrameRate = 10;
    SINT32 SrcFrmRate = 0;
    static int SensorFrame = 10;//sensor帧率
    static int LastSensorFrame = 10;//上一次sensor帧率

    VENC_CHNTYPE_E eChnType = 0;
    VENC_ENCODEPARAM_S EncParam = {0};
    CPlatformChannelContext *pPlatCtx = (CPlatformChannelContext *)pChCtx->pPlatformChannelhandle;
    CmosFrequencyType_t AEFlicker = CMOS_FREQUENCY_50HZ;//抗闪模式
#endif

    if (access("/mnt/mtd/en_frame", F_OK) == 0)
    {
        ZW_INFO("ctrlMode = %d, eDayNightStatus = %d, m_VIChnFrmRate = %d, Frequ_HZ = %d\n",\
            lightCtx->ctrlMode, lightCtx->eDayNightStatus, pInfo->m_VIChnFrmRate[0], pInfo->m_Frequ_HZ);
    }

    switch (lightCtx->ctrlMode)
    {
        case LIGHTCTL_MODE_PASSIVITY:
        case LIGHTCTL_MODE_DAY:
        case LIGHTCTL_MODE_NIGHT:
        case LIGHTCTL_MODE_TIME:
        case LIGHTCTL_MODE_INITIATIVE:
        case LIGHTCTL_MODE_INTELLIGENT_REDRAY:
        case LIGHTCTL_MODE_INTELLIGENT_WHITELIGHT:
        case LIGHTCTL_MODE_NORMAL_WHITELIGHT:
        {
            if (LIGHTCTL_IRCUT_NIGHT == lightCtx->eDayNightStatus)
            {
                if (0 == lightCtx->isSensorSlowFPSMode)
                {
                    for (i = 0; i < SUPPORT_MAX_SENSOR_COUNT; i++)
                    {
                        Hi_MPP_ChangeSensorFps(0, i, (FLOAT32)SENSOR_SLOW_FRAME_RATE);
                    }
//                    SensorFrame = SENSOR_SLOW_FRAME_RATE;
                    lightCtx->isSensorSlowFPSMode = 1;
                }
            }
            else
            {
                if (1 == lightCtx->isSensorSlowFPSMode)
                {
                    for (i = 0; i < SUPPORT_MAX_SENSOR_COUNT; i++)
                    {
#if defined(__IS_SUPPORT_12FPS__)
                        if(pInfo->m_Frequ_HZ == CMOS_FREQUENCY_50HZ || pInfo->m_Frequ_HZ ==CMOS_FREQUENCY_CLOSE)
                        {
                            Hi_MPP_ChangeSensorFps(0, i, 12.5);
                        }
                        else
                        {
                            Hi_MPP_ChangeSensorFps(0, i, (FLOAT32)pInfo->m_VIChnFrmRate[i]);
                        }
#else
                        Hi_MPP_ChangeSensorFps(0, i, (FLOAT32)pInfo->m_VIChnFrmRate[i]);
#endif
                    }
//                    SensorFrame = pInfo->m_VIChnFrmRate[0];
                    lightCtx->isSensorSlowFPSMode = 0;
                }
            }
        }
        break;
        case LIGHTCTL_MODE_COLOR_NIGHT:
        {
            if (1 == lightCtx->colorNightMode)
            {
                if (0 == lightCtx->isSensorSlowFPSMode)
                {
                    for (i = 0; i < SUPPORT_MAX_SENSOR_COUNT; i++)
                    {
                        Hi_MPP_ChangeSensorFps(0, i, (FLOAT32)SENSOR_SLOW_FRAME_RATE);
                    }
//                    SensorFrame = SENSOR_SLOW_FRAME_RATE;
                    lightCtx->isSensorSlowFPSMode = 1;
                }
            }
            else
            {
                if (1 == lightCtx->isSensorSlowFPSMode)
                {
                    for (i = 0; i < SUPPORT_MAX_SENSOR_COUNT; i++)
                    {
#if defined(__IS_SUPPORT_12FPS__)
                        if(pInfo->m_Frequ_HZ == CMOS_FREQUENCY_50HZ || pInfo->m_Frequ_HZ ==CMOS_FREQUENCY_CLOSE)
                        {
                            Hi_MPP_ChangeSensorFps(0, i, 12.5);
                        }
                        else
                        {
                            Hi_MPP_ChangeSensorFps(0, i, (FLOAT32)pInfo->m_VIChnFrmRate[i]);
                        }
#else
                        Hi_MPP_ChangeSensorFps(0, i, (FLOAT32)pInfo->m_VIChnFrmRate[i]);
#endif
                    }
//                    SensorFrame = pInfo->m_VIChnFrmRate[0];
                    lightCtx->isSensorSlowFPSMode = 0;
                }
            }
        }
        break;
        case LIGHTCTL_MODE_INTELLIGENT_NIGHT:
        {
            if (LIGHTCTL_IRCUT_NIGHT == lightCtx->eDayNightStatus || 1 == lightCtx->intelligentWhiteStatue)
            {
                if (0 == lightCtx->isSensorSlowFPSMode)
                {
                    for (i = 0; i < SUPPORT_MAX_SENSOR_COUNT; i++)
                    {
                        Hi_MPP_ChangeSensorFps(0, i, (FLOAT32)SENSOR_SLOW_FRAME_RATE);
                    }
//                    SensorFrame = SENSOR_SLOW_FRAME_RATE;
                    lightCtx->isSensorSlowFPSMode = 1;
                }
            }
            else
            {
                if (1 == lightCtx->isSensorSlowFPSMode)
                {
                    for (i = 0; i < SUPPORT_MAX_SENSOR_COUNT; i++)
                    {
#if defined(__IS_SUPPORT_12FPS__)
                        if(pInfo->m_Frequ_HZ == CMOS_FREQUENCY_50HZ || pInfo->m_Frequ_HZ ==CMOS_FREQUENCY_CLOSE)
                        {
                            Hi_MPP_ChangeSensorFps(0, i, 12.5);
                        }
                        else
                        {
                            Hi_MPP_ChangeSensorFps(0, i, (FLOAT32)pInfo->m_VIChnFrmRate[i]);
                        }
#else
                        Hi_MPP_ChangeSensorFps(0, i, (FLOAT32)pInfo->m_VIChnFrmRate[i]);
#endif
                    }
//                    SensorFrame = pInfo->m_VIChnFrmRate[0];
                    lightCtx->isSensorSlowFPSMode = 0;
                }
            }
        }
        break;
        default:
        {

        }
        break;
    }

#if 0
    if (SensorFrame != LastSensorFrame)
    {
        g_nCurSensorFrame = SensorFrame;
        LastSensorFrame = SensorFrame;

        //下面暂时注释掉，如果不加编码降帧机制需要把这部分代码打开
        ZW_DEBUG("m_EncAttr.FrameRate=%d, m_EncAttrAux.FrameRate=%d\n", pChCtx->m_EncAttr.FrameRate, pChCtx->m_EncAttrAux.FrameRate);
        for (int i = 0; i < SUPPORT_MAX_SENSOR_COUNT; i++)
        {
            eChnType = __KS_VENC_CHN_TYPE(i, FIRST_STREAM);
            ret = Venc_Chn_Get_EncodeParam(0, eChnType, pPlatCtx->m_MppEncChNum[i][FIRST_STREAM], &EncParam, &SrcFrmRate);
            if(!ret)
            {
                //sensor帧率比用户配置的要小，则调整目标帧率
                if(SensorFrame < pChCtx->m_EncAttr.FrameRate)
                {
                    EncParam.FrameRate = SensorFrame;
                }
                else
                {
                    EncParam.FrameRate = pChCtx->m_EncAttr.FrameRate;
                }
                EncParam.FrameRate = EncParam.FrameRate > maxFrameRate ? maxFrameRate : EncParam.FrameRate;
                Hi_MPP_GetGOPValueByNetType(EncParam.FrameRate, &EncParam.KeyInterval);
                Venc_Chn_Set_EncodeParam(0, eChnType, pPlatCtx->m_MppEncChNum[i][FIRST_STREAM], EncParam, SrcFrmRate);
            }
        }

        for (int i = 0; i < SUPPORT_MAX_SENSOR_COUNT; i++)
        {
            eChnType = __KS_VENC_CHN_TYPE(i, SECOND_STREAM);
            ret = Venc_Chn_Get_EncodeParam(0, eChnType, pPlatCtx->m_MppEncChNum[i][SECOND_STREAM], &EncParam, &SrcFrmRate);
            if(!ret)
            {
                //sensor帧率比用户配置的要小，则调整目标帧率
                if(SensorFrame < pChCtx->m_EncAttrAux.FrameRate)
                {
                    EncParam.FrameRate = SensorFrame;
                }
                else
                {
                    EncParam.FrameRate = pChCtx->m_EncAttrAux.FrameRate;
                }
                EncParam.FrameRate = EncParam.FrameRate > maxFrameRate ? maxFrameRate : EncParam.FrameRate;
                Hi_MPP_GetGOPValueByNetType(EncParam.FrameRate, &EncParam.KeyInterval);
                Venc_Chn_Set_EncodeParam(0, eChnType, pPlatCtx->m_MppEncChNum[i][SECOND_STREAM], EncParam, SrcFrmRate);
            }
        }
    }

    if (CMOS_FREQUENCY_50HZ == AEFlicker)
    {
        if (0 == lightCtx->isSensorSlowFPSMode)
        {
            for (i = 0; i < SUPPORT_MAX_SENSOR_COUNT; i++)
            {
                Hi_MPP_ChangeSensorFps(0, i, SENSOR_SLOW_FRAME_RATE);
            }
            lightCtx->isSensorSlowFPSMode = 1;
        }
    }
    else
    {
        if (1 == lightCtx->isSensorSlowFPSMode)
        {
            for (i = 0; i < SUPPORT_MAX_SENSOR_COUNT; i++)
            {
                Hi_MPP_ChangeSensorFps(0, i, pInfo->m_VIChnFrmRate[i]);
            }
            lightCtx->isSensorSlowFPSMode = 0;
        }
    }
#endif
    return ret;
}
#endif

void *CheckBrightnessThread(void *lpParameter)
{
    PRT_TID_NAME("CheckBrightnessThread");
    ZW_DEBUG("%s Thread PID = %d !!!!!!!!!!!!\n", __FUNCTION__, getpid());

    ThreadInfo_t *pParent = (ThreadInfo_t *)lpParameter;
    CChannelContext *pChCtx = NULL;
    LIGHT_CTX_S* lightCtx = NULL;
#if defined (__IS_SUPPORT_ADUJUST_SENSOR_FRAME__)
    SystemInfo_t *pSysInfo = NULL;
    HY_Res_GetSystemInfo(&pSysInfo);
#endif
    HY_Enc_GetEncChInfo(&pChCtx);

    lightCtx = &pChCtx->m_stLightCtx;

    //等待IRCUT切换两次操作结束
    while (1)
    {
        if (pParent->m_Exit == 1)
        {
            break;
        }

        usleep(500 * 1000);
        XTRACE_MUTEX_LOCK(&pChCtx->m_IrcutLock);
        if (pChCtx->m_IrcutSwitchFlag == 1)
        {
            XTRACE_MUTEX_UNLOCK(&pChCtx->m_IrcutLock);
            break;
        }

        XTRACE_MUTEX_UNLOCK(&pChCtx->m_IrcutLock);
    }

    while (1)
    {
        if (pParent->m_Exit == 1)
        {
            break;
        }

        //如果正在进行老化测试或者其它的辅助控制测试，则直接continue
        if (0 == access(SD_AGING_TEST_LIST_FILE_NAME, F_OK) || pChCtx->m_nAssistantControlTest > 0)
        {
            usleep(BRIGHTNESS_CHECK_FREQ);
            continue;
        }
        LightCtl_OneKeyNigntLedProcess(pChCtx);

        // 更新一些配置信息
        _LightCtl_Update(lightCtx);

        // 内部延时控制节奏
        LightCtl_Progress(lightCtx);

        // 夜晚降帧机制(当前项目最大帧率10帧)
#if defined (__IS_SUPPORT_ADUJUST_SENSOR_FRAME__)
        _LightCtl_Frame(pSysInfo, pChCtx, lightCtx);
#endif
        // usleep(100 * 1000);
    }

    return NULL;
}

void *HY_Switch_Ircut_Twice(void *arg)
{
    PRT_TID_NAME("Switch_Ircut_Twice");
    pthread_detach(pthread_self());

    CChannelContext *pChannelContext = (CChannelContext *)arg;

        /*
    * ircutState = 0: 关闭ircut
    * ircutState = 1: 当前为白天模式，要设置为夜间模式:   白--->夜
    * ircutState = 2: 当前为夜间模式，要设置为白天模式:   夜--->白
    */

    sleep(2);

    XTRACE_MUTEX_LOCK(&pChannelContext->m_IrcutLock);

    if (1 == pChannelContext->m_DayNightStutas)  //day
    {
        if(0 == pChannelContext->m_IrcutDeriction)
        {
            Hi_MPP_SetIRCutMode(IRCUT_MODE_NIGHT);
        }
        else
        {
            Hi_MPP_SetIRCutMode(IRCUT_MODE_DAY);
        }

        sleep(5);

        if(0 == pChannelContext->m_IrcutDeriction)
        {
            Hi_MPP_SetIRCutMode(IRCUT_MODE_DAY);
        }
        else
        {
            Hi_MPP_SetIRCutMode(IRCUT_MODE_NIGHT);
        }
    }
    else //night
    {
        if (0 == pChannelContext->m_IrcutDeriction)
        {
            Hi_MPP_SetIRCutMode(IRCUT_MODE_DAY);
        }
        else
        {
            Hi_MPP_SetIRCutMode(IRCUT_MODE_NIGHT);
        }

        sleep(5);

        if(0 == pChannelContext->m_IrcutDeriction)
        {
            Hi_MPP_SetIRCutMode(IRCUT_MODE_NIGHT);
        }
        else
        {
            Hi_MPP_SetIRCutMode(IRCUT_MODE_DAY);
        }
    }

    pChannelContext->m_IrcutSwitchFlag = 1;

    XTRACE_MUTEX_UNLOCK(&pChannelContext->m_IrcutLock);

    return NULL;
}

// 定义复位按钮操作
typedef enum reset_key_action
{
    RESET_KEY_ACTION_NONE = 0,        // 无效动作
    RESET_KEY_ACTION_SW_LIGHT,        // 切换灯板
    RESET_KEY_ACTION_TRANSITION,    // 过度态，无实际动作
    RESET_KEY_ACTION_RECOVER_SYS,    // 系统复位
} RESET_KEY_ACTION_E;

#define SLEEPUNIT_TIME (200*1000)
#define SYS_RESET_TIME (3*1000*1000)

/* 检测IP复位引脚状态线程 */
void *CheckSecretPinStatusThread(void *lpParameter)
{
    PRT_TID_NAME("CheckSecretPinStatusThread");
    ZW_DEBUG("%s Thread PID = %d !!!!!!!!!!!!\n", __FUNCTION__, getpid());

    ThreadInfo_t *pParent = NULL;
    CChannelContext     *pChannelContext = NULL;

    int resetSuccss = 0;
    unsigned int plushTime = 0;         //按键按下的时间
    KEY_STATUS_E eKeySt = KEY_RELE;     //实时按键状态
    RESET_KEY_ACTION_E eAction = RESET_KEY_ACTION_NONE;
    HY_Enc_GetEncChInfo(&pChannelContext);

#ifdef __ONE_CLICK_CALL__
    SystemInfo_t *pSysInfo = NULL;
    HY_Res_GetSystemInfo(&pSysInfo);
    KEY_STATUS_E eCallKeySt = KEY_RELE;       // 呼叫按键状态
    KEY_STATUS_E eHangupKeySt = KEY_RELE;     // 挂断按键状态
#endif

    pParent = (ThreadInfo_t *)lpParameter;

    while (1)
    {
        if (pParent->m_Exit == 1)
        {
            break;
        }

        usleep(SLEEPUNIT_TIME); //200ms

#ifdef __ONE_CLICK_CALL__
        //一键呼叫复用功能
        if(pSysInfo->m_bSupportAlternateResetButton == 1)
        {
            Hi_MPP_GetCallKeyStatus(&eCallKeySt, &eHangupKeySt);
            Hi_MPP_GetRestKeyStatus(&eKeySt);
            if (eCallKeySt == KEY_PUSH || eHangupKeySt == KEY_PUSH)
            {
                eKeySt = KEY_PUSH;
            }
        }
        else
        {
            Hi_MPP_GetRestKeyStatus(&eKeySt);
        }
#else
        Hi_MPP_GetRestKeyStatus(&eKeySt);
#endif
        if (KEY_PUSH == eKeySt)
        {
            g_SecretPinStatus = 0;
            plushTime += SLEEPUNIT_TIME;
        }
        else
        {
            g_SecretPinStatus = 1;
            resetSuccss = 0;
            plushTime = 0;
            eAction = RESET_KEY_ACTION_NONE;
        }

        switch (eAction)
        {
        case RESET_KEY_ACTION_NONE:
            //ZW_INFO("You has push key...\n");
            if ((plushTime > 0) && (plushTime < SYS_RESET_TIME))
            {
                eAction = RESET_KEY_ACTION_TRANSITION;
            }
            break;

        case RESET_KEY_ACTION_TRANSITION:
            //ZW_INFO("You has push key greater than %d seconds and less than %d seconds...\n", switchLightBoardTimeMax / 1000000, cfgResetTime / 1000000);
            if (plushTime >= SYS_RESET_TIME)
            {
                eAction = RESET_KEY_ACTION_RECOVER_SYS;
            }
            break;

        case RESET_KEY_ACTION_RECOVER_SYS:

            if (resetSuccss == 0)
            {
                if (pChannelContext->m_GetKeyStatusCallBack)
                {
                    ZW_INFO("KEYCLICK_RESUME\n");
                    pChannelContext->m_GetKeyStatusCallBack(KEYCLICK_RESUME);
                }
                resetSuccss = 1;
                eAction = RESET_KEY_ACTION_NONE;
            }
            break;

        default:
            break;
        }
    }

    return NULL;
}

#if defined __ONE_CLICK_CALL__
// 定义呼叫按键状态
typedef enum call_key_action
{
    CALL_KEY_ACTION_NONE = 0,           // 初始/空闲状态
    CALL_KEY_ACTION_CALL_SUCCESSFUL,    // 呼叫按键触发成功
    CALL_KEY_ACTION_HANGUP_SUCCESSFUL,  // 挂断按键触发成功
    CALL_KEY_ACTION_ALL_SUCCESSFUL,     // 呼叫和挂断按键同时触发成功
    CALL_KEY_ACTION_WAIT_RELEASE,       // 等待按键松开状态
} CALL_KEY_ACTION_E;

void* CheckKeytocallPinStatusThread(void * lpParameter)
{
    PRT_TID_NAME("CheckKeytocallPinStatusThread");
    ThreadInfo_t *pParent = (ThreadInfo_t *)lpParameter;

    SystemInfo_t *pSysInfo = NULL;
    HY_Res_GetSystemInfo(&pSysInfo);

    unsigned int cfgCallTime = 40 * 1000;//单位us
    unsigned int sleepUnit = 20 * 1000;//单位us
    CALL_KEY_ACTION_E eAction = CALL_KEY_ACTION_NONE;
    KEY_STATUS_E eCallKeySt = KEY_RELE;       // 呼叫按键状态
    KEY_STATUS_E eHangupKeySt = KEY_RELE;     // 挂断按键状态
    int callSuccess = 0;                      // 呼叫触发标志
    int hangupSuccess = 0;                    // 挂断触发标志
    unsigned int callPlushTime = 0;           // 呼叫按键按下的累计时间
    unsigned int hangupPlushTime = 0;         // 挂断按键按下的累计时间

    if (0 == pSysInfo->m_bSupportKeyToCall)
    {
        g_CallPinStatus = 0;
        return 0;
    }

    if (1 == pSysInfo->m_bSupportAlternateResetButton)
    {
        g_CallPinStatus = 0;
        return 0;
    }

    while (1)
    {
        if (pParent->m_Exit == 1 || 1 == pSysInfo->m_bSupportAlternateResetButton)
        {
            if (pParent->m_Exit == 1)
                pParent->m_Exit = 0;
            break;
        }

        usleep(sleepUnit); //20ms

        // 检测按键从松开到按下，才开始累计时间
        Hi_MPP_GetCallKeyStatus(&eCallKeySt, &eHangupKeySt);

        // 处理呼叫按键
        if (KEY_PUSH == eCallKeySt)
        {
            if (0 == callSuccess)
            {
                callPlushTime += sleepUnit;
            }
        }
        else
        {
            g_CallPinStatus &= ~CALL_PIN_STATUS_CALL;
            callSuccess = 0;
            callPlushTime = 0;
        }

        // 处理挂断按键
        if (KEY_PUSH == eHangupKeySt)
        {
            if (0 == hangupSuccess)
            {
                hangupPlushTime += sleepUnit;
            }
        }
        else
        {
            g_CallPinStatus &= ~CALL_PIN_STATUS_HANGUP;
            hangupSuccess = 0;
            hangupPlushTime = 0;
        }

        // 按键状态机处理
        switch (eAction)
        {
            case CALL_KEY_ACTION_NONE: // 检查呼叫和挂断按键是否触发
            if (callPlushTime >= cfgCallTime && hangupPlushTime < cfgCallTime)
            {
                eAction = CALL_KEY_ACTION_CALL_SUCCESSFUL;
            }
            else if (callPlushTime < cfgCallTime && hangupPlushTime >= cfgCallTime)
            {
                eAction = CALL_KEY_ACTION_HANGUP_SUCCESSFUL;
            }
            else if (callPlushTime >= cfgCallTime && hangupPlushTime >= cfgCallTime)
            {
                eAction = CALL_KEY_ACTION_ALL_SUCCESSFUL;
            }
            break;

            case CALL_KEY_ACTION_CALL_SUCCESSFUL:   // 呼叫按键触发成功
            if (callSuccess == 0)
            {
                g_CallPinStatus |= CALL_PIN_STATUS_CALL;  // 设置呼叫按键位(bit0)
                g_TestCallPinStatus |= CALL_PIN_STATUS_CALL;
                callSuccess = 1;
                eAction = CALL_KEY_ACTION_WAIT_RELEASE;
            }
            break;

            case CALL_KEY_ACTION_HANGUP_SUCCESSFUL:     // 挂断按键触发成功
            if (hangupSuccess == 0)
            {
                g_CallPinStatus |= CALL_PIN_STATUS_HANGUP;  // 设置挂断按键位(bit1)
                g_TestCallPinStatus |= CALL_PIN_STATUS_HANGUP;
                hangupSuccess = 1;
                eAction = CALL_KEY_ACTION_WAIT_RELEASE;
            }
            break;

            case CALL_KEY_ACTION_ALL_SUCCESSFUL:    // 呼叫和挂断按键同时触发成功
            if (callSuccess == 0)
            {
                g_CallPinStatus |= CALL_PIN_STATUS_CALL;  // 设置呼叫按键位(bit0)
                g_TestCallPinStatus |= CALL_PIN_STATUS_CALL;
                callSuccess = 1;
            }
            if (hangupSuccess == 0)
            {
                g_CallPinStatus |= CALL_PIN_STATUS_HANGUP;  // 设置挂断按键位(bit1)
                g_TestCallPinStatus |= CALL_PIN_STATUS_HANGUP;
                hangupSuccess = 1;
            }
            eAction = CALL_KEY_ACTION_WAIT_RELEASE;
            break;

            case CALL_KEY_ACTION_WAIT_RELEASE:      // 等待按键松开状态
            if (KEY_RELE == eCallKeySt && KEY_RELE == eHangupKeySt)
            {
                eAction = CALL_KEY_ACTION_NONE;
            }
            break;

            default:
            break;
        }
    }
    return NULL;
}
#endif

/* IRCUT切换线程 */
void* IRCUTSwitchThread(void * lpParameter)
{
    PRT_TID_NAME("IRCUTSwitchThread");
    ZW_DEBUG("%s Thread PID = %d !!!!!!!!!!!!\n", __FUNCTION__, getpid());

    SINT32 ircutState = 0;
    CChannelContext *pChCtx = NULL;
    ThreadInfo_t    *pParent = NULL;

    if (NULL == lpParameter)
    {
        ZW_ERROR("lpParameter CANNOT be NULL\n");
        return NULL;
    }

    pParent = (ThreadInfo_t *)lpParameter;

    HY_Enc_GetEncChInfo(&pChCtx);

    while(1)
    {
        if (pParent->m_Exit == 1)
        {
            break;
        }

        XTRACE_MUTEX_LOCK(&pChCtx->m_IrcutLock);
        /* 获取ircut当前状态 =>
        * ircutState = 0: 关闭ircut
        * ircutState = 1: 当前为白天模式，要设置为夜间模式:   白--->夜
        * ircutState = 2: 当前为夜间模式，要设置为白天模式:   夜--->白
        */
        ircutState = pChCtx->m_IrcutState;
        pChCtx->m_IrcutState = 0;

        /* 接线:
        * GPIOF_1 => DN_F1 => BIN
        * GPIOF_2 => DN_F2 => AIN
        */
        if(IRCUT_STATUS_NIGHT == ircutState)  /* 白天到晚上方向切换 */
        {
            ZW_INFO("-----set ircut night mode-----\n");
            if(1 == pChCtx->m_IrcutDeriction)
            {
                Hi_MPP_SetIRCutMode(IRCUT_MODE_DAY);
                Hi_MPP_SwitchScreenColor(ISNIGHT);
            }
            else
            {
                Hi_MPP_SetIRCutMode(IRCUT_MODE_NIGHT);
                Hi_MPP_SwitchScreenColor(ISNIGHT);
            }
        }
        else if(IRCUT_STATUS_DAY == ircutState) /* 晚上到白天方向切换 */
        {
            ZW_INFO("-----set ircut day mode-----\n");
            if(1 == pChCtx->m_IrcutDeriction)
            {
                Hi_MPP_SetIRCutMode(IRCUT_MODE_NIGHT);
                Hi_MPP_SwitchScreenColor(ISDAY);
            }
            else
            {
                Hi_MPP_SetIRCutMode(IRCUT_MODE_DAY);
                Hi_MPP_SwitchScreenColor(ISDAY);
            }
        }

        XTRACE_MUTEX_UNLOCK(&pChCtx->m_IrcutLock);

        usleep(250 * 1000);
    }

    return NULL;
}

/* 获取编码通道信息句柄 */
int    HY_Enc_GetEncChInfo(CChannelContext **pInfo)
{
    *pInfo = &g_EncChannelContext;
    return 0;
}

int    HY_Enc_InitEncChInfo()
{
    memset(&g_EncChannelContext, 0, sizeof(CChannelContext));
    return 0;
}

int HY_Enc_InitTalkingChInfo()
{
    memset(&g_TalkingChannel, 0, sizeof(CTalkingChannel_t));
    return 0;
}

/* 获取语音对讲通道信息句柄 */
int    HY_Enc_GetTalkingChInfo(CTalkingChannel_t **pInfo)
{
    *pInfo = &g_TalkingChannel;
    return 0;
}

/* 适配编码资源 函数没啥作用*/
int    HY_Enc_InitEncCap()
{
    SystemInfo_t *pSysInfo = NULL;
    HY_Res_GetSystemInfo(&pSysInfo);

    //总资源
    int m_MainStreamCap = (MAIN_STREAM_WIDTH * MAIN_STREAM_HEIGHT * MAX_MAIN_FPS);
    int m_AuxStreamCap = (AUX_STREAM_WIDTH * AUX_STREAM_HEIGHT * MAX_AUX_FPS);
    int m_FourthStreamCap = (MAIN_STREAM_WIDTH * MAIN_STREAM_HEIGHT * FOURTH_STREAM_FPS);

    pSysInfo->m_TotalEncCap    = m_MainStreamCap + m_AuxStreamCap + m_FourthStreamCap;

    return 0;
}


int HY_Enc_RestartOsd(CChannelContext *pChannelContext)
{
    CChannelContext *pChCtx = NULL;

    if (NULL == pChannelContext)
    {
        ZW_ERROR("pChannelContext CANNOT be NULL\n");
        return -1;
    }

    pChCtx = pChannelContext;

    ZW_DEBUG("HY_Enc_RestartOsd Start\n");

    if (pChCtx->m_bEnableTimeOSD)
    {
         HY_Enc_StopOsd(pChCtx, TIME_TYPE, MAX_STREAM_COUNT);
    }

     if (pChCtx->m_bEnableTextOSD)
     {
         HY_Enc_StopOsd(pChCtx, TEXT_TYPE, MAX_STREAM_COUNT);
     }

#ifdef __DIGITAL_ZOOM_DRAG_SUPPORT__
        HY_Enc_StopOsd(pChCtx, ZOOM_TYPE, MAX_STREAM_COUNT);
#endif

    if (pChCtx->m_bEnableTimeOSD)
    {
         HY_Enc_StartOsd(pChannelContext, TIME_TYPE, MAX_STREAM_COUNT);
    }

     if (pChCtx->m_bEnableTextOSD)
     {
         HY_Enc_StartOsd(pChannelContext, TEXT_TYPE, MAX_STREAM_COUNT);
     }

#ifdef __DIGITAL_ZOOM_DRAG_SUPPORT__
        HY_Enc_StartOsd(pChannelContext, ZOOM_TYPE, MAX_STREAM_COUNT);
#endif

    ZW_DEBUG("HY_Enc_RestartOsd End\n");

    return 0;
}

int HY_Enc_StopUserSet(CChannelContext *pChannelContext, ENCODE_STREAM_E eStream, UserSetParamFlag_t *m_UserSetParam)
{
    CChannelContext *pChCtx = NULL;

    if (NULL == pChannelContext)
    {
        ZW_ERROR("pChannelContext CANNOT be NULL\n");
        return -1;
    }

    pChCtx = pChannelContext;

#ifdef __DIGITAL_ZOOM_DRAG_SUPPORT__
            Zoom_StopDragZoom();
#endif

    /*停止MD*/
    XTRACE_MUTEX_LOCK(&pChCtx->m_MDLock);
    if (pChCtx->m_bStartMD)
    {
        HY_Enc_MD_Stop(pChCtx);
        m_UserSetParam->restartMdFlag = TRUE;
    }
    XTRACE_MUTEX_UNLOCK(&pChCtx->m_MDLock);

    // 如果OSD开启则停止OSD
    XTRACE_MUTEX_LOCK(&pChCtx->m_OSDLock);
    if (pChCtx->m_bEnableTimeOSD)
    {
         HY_Enc_StopOsd(pChCtx, TIME_TYPE, eStream);
         m_UserSetParam->restartTimeOsdFlag = TRUE;
    }

     if (pChCtx->m_bEnableTextOSD)
     {
         HY_Enc_StopOsd(pChCtx, TEXT_TYPE, eStream);
         m_UserSetParam->restartTextOsdFlag = TRUE;
     }

#ifdef __DIGITAL_ZOOM_DRAG_SUPPORT__
    if(pChCtx->m_DragUseOsd)
    {
        HY_Enc_StopOsd(pChCtx, ZOOM_TYPE, eStream);
        m_UserSetParam->restartZoomOsdFlag = TRUE;
    }
#endif

    XTRACE_MUTEX_UNLOCK(&pChCtx->m_OSDLock);

     // 清除画框画线信息
	Hi_MPP_ClearDrawLineInfo(1);

    return 0;
}

int HY_Enc_StartUserSet(CChannelContext *pChannelContext, ENCODE_STREAM_E eStream, UserSetParamFlag_t *m_UserSetParam)
{
    CChannelContext *pChCtx = NULL;

    if (NULL == pChannelContext)
    {
        ZW_ERROR("pChannelContext CANNOT be NULL\n");
        return -1;
    }

    pChCtx = pChannelContext;

    /*调整MD资源*/
    XTRACE_MUTEX_LOCK(&pChCtx->m_MDLock);
    if (m_UserSetParam->restartMdFlag)
    {
        HY_Enc_MD_Start(pChCtx);
    }
    XTRACE_MUTEX_UNLOCK(&pChCtx->m_MDLock);

    /* 调整OSD资源 */
    XTRACE_MUTEX_LOCK(&pChannelContext->m_OSDLock);
    if(m_UserSetParam->restartTimeOsdFlag)
    {
         HY_Enc_StartOsd(pChannelContext, TIME_TYPE, eStream);
    }
     if(m_UserSetParam->restartTextOsdFlag)
     {
         HY_Enc_StartOsd(pChannelContext, TEXT_TYPE, eStream);
     }
#ifdef __DIGITAL_ZOOM_DRAG_SUPPORT__
        if(m_UserSetParam->restartZoomOsdFlag)
        {
            HY_Enc_StartOsd(pChannelContext, ZOOM_TYPE, eStream);
        }
#endif

        XTRACE_MUTEX_UNLOCK(&pChannelContext->m_OSDLock);

#ifdef __DIGITAL_ZOOM_DRAG_SUPPORT__
        Zoom_StartDragZoom();
#endif

    return 0;
}

int HY_Enc_AdjustEncCap(void)
{
    int MaxFrameRate  = 0;
    int MaxFrameRateAux = 0;
    CChannelContext *pChCtx = NULL;
    HY_Enc_GetEncChInfo(&pChCtx);

    //主码流
    if(pChCtx->m_EncAttr.PictureRes.m_PicWidth == 3840 && pChCtx->m_EncAttr.PictureRes.m_PicHeight == 2160) //800W
    {
        MaxFrameRate = MAX_FPS_8M;
    }
    else if(pChCtx->m_EncAttr.PictureRes.m_PicWidth == 2880 && pChCtx->m_EncAttr.PictureRes.m_PicHeight == 1616) // 500W 16:9
    {
        MaxFrameRate = MAX_FPS_5M;
    }
    else if(pChCtx->m_EncAttr.PictureRes.m_PicWidth == 2560 && pChCtx->m_EncAttr.PictureRes.m_PicHeight == 1920) // 500W
    {
        MaxFrameRate = MAX_FPS_5M;
    }
    else if (pChCtx->m_EncAttr.PictureRes.m_PicWidth == 2560 && pChCtx->m_EncAttr.PictureRes.m_PicHeight == 1440)
    {
        MaxFrameRate = MAX_FPS_4M;
    }
    else if(pChCtx->m_EncAttr.PictureRes.m_PicWidth > 1920 && pChCtx->m_EncAttr.PictureRes.m_PicWidth <= 2304)
    {
        MaxFrameRate = MAX_FPS_QXGA;
    }
    else if(pChCtx->m_EncAttr.PictureRes.m_PicWidth > 1280 && pChCtx->m_EncAttr.PictureRes.m_PicWidth <= 1920)
    {
        MaxFrameRate = MAX_FPS_HD1080P;
    }
    else if(pChCtx->m_EncAttr.PictureRes.m_PicWidth > 704 && pChCtx->m_EncAttr.PictureRes.m_PicWidth <= 1280)
    {
        MaxFrameRate = MAX_FPS_HD720P;
    }
    else if(pChCtx->m_EncAttr.PictureRes.m_PicWidth > 576 && pChCtx->m_EncAttr.PictureRes.m_PicWidth <= 704)
    {
        MaxFrameRate = MAX_FPS_512P;
    }
    else
    {
        MaxFrameRate = MAX_FPS_HD1080P;
    }

    //子码流
    if(pChCtx->m_EncAttrAux.PictureRes.m_PicWidth < 704 && pChCtx->m_EncAttrAux.PictureRes.m_PicHeight < 576)
    {
        MaxFrameRateAux = MAX_FPS_360P;
    }
    else if(pChCtx->m_EncAttrAux.PictureRes.m_PicWidth == 704 && pChCtx->m_EncAttrAux.PictureRes.m_PicHeight == 576)
    {
        MaxFrameRateAux = MAX_FPS_D1;
    }
    else if(pChCtx->m_EncAttrAux.PictureRes.m_PicWidth == 800 && pChCtx->m_EncAttrAux.PictureRes.m_PicHeight == 448)
    {
        MaxFrameRateAux = MAX_FPS_D1;
    }
    else if(pChCtx->m_EncAttrAux.PictureRes.m_PicWidth == 1280 && pChCtx->m_EncAttrAux.PictureRes.m_PicHeight == 720)
    {
        MaxFrameRateAux = AUX_MAX_FPS_HD720P;
    }
    else
    {
        MaxFrameRateAux = MAX_FPS_D1;
    }

    XTRACE_MUTEX_LOCK(&pChCtx->m_VencLock);
    /* 主码流 */
    if (pChCtx->m_bRecord)
    {
        if (pChCtx->m_EncAttr.FrameRate > MaxFrameRate)
        {
            pChCtx->m_EncAttr.FrameRate = MaxFrameRate;
        }
        Hi_MPP_SetEncPara(pChCtx->pPlatformChannelhandle, &pChCtx->m_EncAttr, FIRST_STREAM, 0, pChCtx->m_iSlowFrameMode);
    }

    /* 子码流 */
    if (pChCtx->m_bRecordAux)
    {
        if (pChCtx->m_EncAttrAux.FrameRate > MaxFrameRateAux)
        {
            pChCtx->m_EncAttrAux.FrameRate = MaxFrameRateAux;
        }
        Hi_MPP_SetEncPara(pChCtx->pPlatformChannelhandle, &pChCtx->m_EncAttrAux, SECOND_STREAM, 0, pChCtx->m_iSlowFrameMode);
    }
#if 0
    /* 抓图码流 */
    if (pChCtx->m_bRecordFourth)
    {
        if (pChCtx->m_EncAttrFourth.FrameRate > CAPTURE_STREAM_MAX_FPS)
        {
            pChCtx->m_EncAttrFourth.FrameRate = CAPTURE_STREAM_MAX_FPS;
        }
        Hi_MPP_SetEncPara(pChCtx->pPlatformChannelhandle, &pChCtx->m_EncAttrFourth, FOURTH_STREAM, pChCtx->m_FourthEncType, 0, eCODE_RATE_USR_SET, pChCtx->m_iSlowFrameMode);
    }
#endif
    XTRACE_MUTEX_UNLOCK(&pChCtx->m_VencLock);

    return 0;
}

int HY_Enc_SetFrameRate(void *hChannelHandle, ENCODE_STREAM_E chn_type,int frmRate, int slowframemode)
{
    int nRet = 0;
    int    MaxFrameRate = 0;
    SystemInfo_t *pSysInfo = NULL;

    HY_Res_GetSystemInfo(&pSysInfo);
    MaxFrameRate = (pSysInfo->m_Frequ_HZ==CMOS_FREQUENCY_60HZ) ? 30 : 25;

    if (hChannelHandle == NULL)
    {
        ZW_ERROR("frame hChannelHandle is NULL !!!\n");
        return -1;
    }
    if (frmRate <= 0 || frmRate > MaxFrameRate)
    {
        ZW_ERROR("frame input error chn_type=%d frmRate=%d!!!\n", chn_type, frmRate);
        return -1;
    }

    nRet = Hi_MPP_SetVencFrameRateV2(hChannelHandle, chn_type, frmRate, slowframemode);
    if (nRet)
    {
        ZW_ERROR("set frame error !!!\n");
        return -1;
    }

    return 0;
}

int HY_Enc_PlatformInit()
{
    SINT32 s32Ret = 0;

    SystemInfo_t *pSysInfo = NULL;
    CChannelContext *pChCtx = NULL;
    CPlatformChannelContext *pPlatCtx = NULL;

    HY_Res_GetSystemInfo(&pSysInfo);
    HY_Enc_GetEncChInfo(&pChCtx);
#ifdef __SUPPORT_WIFI_FUNC__
    HY_EnableQrScan(0);
#endif
    Hi_MPP_initHandle(&pChCtx->pPlatformChannelhandle);

    pPlatCtx = (CPlatformChannelContext *)pChCtx->pPlatformChannelhandle;//平台相关的参数信息

    Hi_MPP_SetUserParam(pChCtx->pPlatformChannelhandle,
                        pSysInfo->m_BoardInfo,
                        &pChCtx->m_EncAttr,
                        &pChCtx->m_EncAttrAux,
                        pChCtx->m_bSupportFourthStream,
                        &pChCtx->m_EncAttrFourth);

    // MDbg_Init(PRN_MANNER_TERM, NULL, COMPSTR);

    /*编码模块初始化*/
    s32Ret = Hi_MPP_InitVi(pChCtx->pPlatformChannelhandle);
    DBG_ASSERT(s32Ret == 0, s32Ret, "Called Hi_MPP_InitVi fail");

    s32Ret = Hi_MPP_InitIsp(pChCtx->pPlatformChannelhandle);
    DBG_ASSERT(s32Ret == 0, s32Ret, "Called Hi_MPP_InitIsp fail");

    s32Ret = Hi_MPP_InitOsd(pChCtx->pPlatformChannelhandle);
    DBG_ASSERT(s32Ret == 0, s32Ret, "Called Hi_MPP_InitOsd fail");

    s32Ret = Hi_MPP_InitVda(pChCtx->pPlatformChannelhandle);
    DBG_ASSERT(s32Ret == 0, s32Ret, "Called Hi_MPP_InitVda fail");

    s32Ret = Hi_MPP_InitVpssChn(pChCtx->pPlatformChannelhandle);
    DBG_ASSERT(s32Ret == 0, s32Ret, "Called Hi_MPP_InitVpssChn fail");

    s32Ret = Hi_MPP_InitVpss(pChCtx->pPlatformChannelhandle);
    DBG_ASSERT(s32Ret == 0, s32Ret, "Called Hi_MPP_InitVpss fail");

    s32Ret = Hi_MPP_InitVenc(pChCtx->pPlatformChannelhandle);
    DBG_ASSERT(s32Ret == 0, s32Ret, "Called Hi_MPP_InitVenc fail");

    s32Ret = Hi_MPP_InitEncChn(pChCtx->pPlatformChannelhandle);
    DBG_ASSERT(s32Ret == 0, s32Ret, "Called Hi_MPP_InitEncChn fail");

    pPlatCtx->m_bEncoderIsStart = TRUE;
    pPlatCtx->m_bHandInit = TRUE;
    pPlatCtx->m_bISPInitIsOver = TRUE;

    s32Ret = Hi_MPP_InitVdaChn(pChCtx->pPlatformChannelhandle);
    DBG_ASSERT(s32Ret == 0, s32Ret, "Called Hi_MPP_InitVdaChn fail");

    s32Ret = Hi_MPP_StartOSD(pChCtx->pPlatformChannelhandle);
    DBG_ASSERT(s32Ret == 0, s32Ret, "Called Hi_MPP_StartOSD fail");

    s32Ret = Hi_MPP_AudioInit(pChCtx->pPlatformChannelhandle);
    DBG_ASSERT(s32Ret == 0, s32Ret, "Called Hi_MPP_AudioInit fail");
#if USE_ALGO
    s32Ret = Hi_MPP_AlgoInit(pChCtx->pPlatformChannelhandle);
#endif

#if defined(__IS_SUPPORT_LCD_MANAGER__)
    s32Ret = Xrs_Lcd_StartLCD(pChCtx->pPlatformChannelhandle);

    s32Ret = Xrs_Lcd_StartVdec(pChCtx->pPlatformChannelhandle);
#endif

    // 最后起SceneAuto
#if defined (__PRODUCT_P4__)||defined(__PRODUCT_P4U__)
    s32Ret = Visp_Start_SceneAuto(0, HISI_SCENEAUTO_PATH_JXK06A, 0);
#elif defined(__PRODUCT_G85T__) || defined(__PRODUCT_W85T__)
    s32Ret = Visp_Start_SceneAuto(0, HISI_SCENEAUTO_PATH_F38PA, 0);
#elif defined(__PRODUCT_G43TA__) || defined(__PRODUCT_G84F__)||defined (__PRODUCT_W84F__) || defined (__PRODUCT_G63F__)|| defined (__PRODUCT_W63F__)
    s32Ret = Visp_Start_SceneAuto(0, HISI_SCENEAUTO_PATH_GC20C3, 0);
#else
    s32Ret = Visp_Start_SceneAuto(0, HISI_SCENEAUTO_PATH, 0);
#endif

    if (s32Ret < 0)
    {
        ZW_ERROR("Visp_Start_SceneAuto failed\n");
        Visp_Stop_SceneAuto(0);
        return -1;
    }
    return 0;
}

/* 适配编码资源逆初始化 */
int    HY_Enc_DeInitEncCap()
{
    SystemInfo_t *pSysInfo = NULL;
    HY_Res_GetSystemInfo(&pSysInfo);

    pSysInfo->m_TotalEncCap    = 0;

    return 0;
}

int HY_Enc_UnInitAISDK()
{
    #if USE_ALGO
     Hi_MPP_AlgounInit();
    #endif
    return 0;
}

int HY_Enc_PlatformDeInit()
{
    CChannelContext *pChCtx = NULL;
    UserSetParamFlag_t m_UserSetParam = {0};
    HY_Enc_GetEncChInfo(&pChCtx);

    /* 停止及销毁OSD,MD,MOSAIC*/
    HY_Enc_StopUserSet(pChCtx, MAX_STREAM_COUNT, &m_UserSetParam);

    Hi_MPP_SetHumanDetectDisEnable(pChCtx->pPlatformChannelhandle);

    // Hi_MPP_DeInitOsd();

    /* 停止编码 */
    XTRACE_MUTEX_LOCK(&pChCtx->m_VencLock);

    pChCtx->m_bRecord = FALSE;
    pChCtx->m_bRecordAux = FALSE;
    pChCtx->m_bRecordFourth = FALSE;

    /* 反初始化音频*/
    Hi_MPP_AudioUninit(pChCtx->pPlatformChannelhandle);

    /* 反初始化编码各个模块*/
    Hi_MPP_DeInit();

    XTRACE_MUTEX_UNLOCK(&pChCtx->m_VencLock);

    Hi_MPP_HalModDeInit();

    /* 销毁通道句柄*/
    Hi_MPP_DeinitHandle((void **)(&pChCtx->pPlatformChannelhandle));

    return 0;
}

static void *SYS_TimeSyncThread(void * pSDKInfoContext)
{
    PRT_TID_NAME("TimeSyncThread");

    int nSDKRet = 0;
    char tbuf[64] = { 0 };
    //pthread_detach(pthread_self());
    mysystem("hwclock -w");

    //如果系统时间跟RTC时间不同（DST开启了就会不同），则重新设置系统时间
    if(NewSYSTime)
    {
        nSDKRet = stime(&NewSYSTime);
        if (0 != nSDKRet)
        {
            ctime_r(&NewSYSTime,tbuf);
            ZW_ERROR("call stime() fail, tNewTime %llu, return %d, mktime %s \n", NewSYSTime, nSDKRet, tbuf);
        }
    }
    pthread_exit(NULL);
    return NULL;
}

//此接口意义已变为设置RTC硬件时间，硬件时间跟linux系统时间不同，linux时间 = RTC时间 +- DST时间；
int HY_Enc_SetCurrentTime(int tNewTime)
{
    pthread_t timeSyncThread;

    NewSYSTime = tNewTime;

    //hwclock命令有塞好几十秒的可能
    pthread_create(&timeSyncThread, NULL, SYS_TimeSyncThread, NULL);
    pthread_join(timeSyncThread,NULL);

    return 0;
}

/* 编码通道句柄检查 */
int HY_Enc_IsEncHandleOk(void * hChannelHandle)
{
    SystemInfo_t    *pSystmeInfo;

    HY_Res_GetSystemInfo(&pSystmeInfo);

    if (&g_EncChannelContext == hChannelHandle)
        return TRUE;

    ZW_ERROR("hChannelHandle = 0x%x Err !!!!!\n", (int)hChannelHandle);

    return FALSE;
}

int HY_Enc_StartEnc(CChannelContext *pChannelContext, int stream)
{
    return 0;
}

int HY_Enc_StopEnc(CChannelContext *pChannelContext, int stream)
{
    return 0;
}

int HY_Enc_ForceIFrame(CChannelContext *pChannelContext, int nSensorIndex)
{
    int s32Ret = 0;

    s32Ret = Hi_MPP_ForceIFrame(pChannelContext->pPlatformChannelhandle, nSensorIndex);
    DBG_ASSERT(s32Ret==0, s32Ret, "Fail to call Hi_MPP_ForceIFrame");

    return s32Ret;
}

int HY_Enc_SwitchPicFormat(CChannelContext *pChannelContext, int EncChannel)
{
    int s32Ret = 0;
    SystemInfo_t *pSysInfo = NULL;
    CChannelContext *pChCtx = NULL;
    CTalkingChannel_t *pTalkingCh = NULL;
    UserSetParamFlag_t m_UserSetParam = {0};
    CPlatformChannelContext *pPlatCtx = NULL;

    if (NULL == pChannelContext)
    {
        ZW_ERROR("pChannelContext CANNOT be NULL\n");
        return -1;
    }

    pChCtx = pChannelContext;
    pPlatCtx = pChCtx->pPlatformChannelhandle;

    HY_Res_GetSystemInfo(&pSysInfo);
    HY_Enc_GetTalkingChInfo(&pTalkingCh);

    if (FOURTH_STREAM == EncChannel && !pChCtx->m_bRecordFourth)
    {
        ZW_INFO("Stream %d not enable!\n", EncChannel);
        return 0;
    }
    else if (SECOND_STREAM == EncChannel && !pChCtx->m_bRecordAux)
    {
        ZW_INFO("Stream %d not enable!\n", EncChannel);
        return 0;
    }
    else if (FIRST_STREAM == EncChannel && !pChCtx->m_bRecord)
    {
        ZW_INFO("Stream %d not enable!\n", EncChannel);
        return 0;
    }

    if (SameAsMainStream == pChCtx->m_CaptureType)
    {
        pChCtx->m_EncAttrFourth.PictureRes = pChCtx->m_EncAttr.PictureRes;
        pChCtx->m_PictureRes[FOURTH_STREAM].m_PicHeight = pChCtx->m_EncAttr.PictureRes.m_PicHeight;
        pChCtx->m_PictureRes[FOURTH_STREAM].m_PicWidth = pChCtx->m_EncAttr.PictureRes.m_PicWidth;
    }
    else if (SameAsAuxStream == pChCtx->m_CaptureType)
    {
        pChCtx->m_EncAttrFourth.PictureRes = pChCtx->m_EncAttrAux.PictureRes;
        pChCtx->m_PictureRes[FOURTH_STREAM].m_PicHeight = pChCtx->m_EncAttrAux.PictureRes.m_PicHeight;
        pChCtx->m_PictureRes[FOURTH_STREAM].m_PicWidth = pChCtx->m_EncAttrAux.PictureRes.m_PicWidth;
    }

    /*初始化用户设置的编码参数*/
    Hi_MPP_InitUserEncodeParam(pPlatCtx, &pChCtx->m_EncAttr, &pChCtx->m_EncAttrAux,
                            pChCtx->m_bSupportFourthStream, &pChCtx->m_EncAttrFourth);

    /*更新用户设置的分辨率*/
    Hi_MPP_UpdatePictureFormat(pChCtx->pPlatformChannelhandle,
        &pChCtx->m_EncAttr.PictureRes,  &pChCtx->m_EncAttrAux.PictureRes,
        pChCtx->m_bSupportFourthStream, &pChCtx->m_EncAttrFourth.PictureRes);

    //切分辨率，整个的SDK重置
    ZW_ERROR("OPEN Restar Enc !!!!!!\n");
    // 停止用户设置
    HY_Enc_StopUserSet(pChCtx, MAX_STREAM_COUNT, &m_UserSetParam);

    XTRACE_MUTEX_LOCK(&pChCtx->m_InitVPSSLock);
    XTRACE_MUTEX_LOCK(&pChCtx->m_CheckLumLock);
    ZW_DEBUG("Change Venc Lock!!!!!!\n");
    do {
        pChCtx->m_bChangeCodePara = true;
        s32Ret = Hi_MPP_RestartEnc(pChCtx->pPlatformChannelhandle);
        if (0 != s32Ret)
        {
            pChCtx->m_bChangeCodePara = false;
            ZW_DEBUG("Hi_MPP_RestartEnc failed, s32Ret = %#x.\n", s32Ret);
            break;
        }
        pChCtx->m_bChangeCodePara = false;
    } while(0);
    ZW_DEBUG("Change Venc UnLock!!!!!!\n");
    XTRACE_MUTEX_UNLOCK(&pChCtx->m_CheckLumLock);
    XTRACE_MUTEX_UNLOCK(&pChCtx->m_InitVPSSLock);

    if (0 != s32Ret)
    {
        return -1;
    }

    HY_Enc_StartUserSet(pChCtx, MAX_STREAM_COUNT, &m_UserSetParam);
    ZW_ERROR("Close Restar Enc !!!!!!\n");

    //重置用户图像配置
    pChCtx->ResetUserPic = TRUE;

    ZW_DEBUG("Picture Format Switch Success! EncChannel=[%d]\n", EncChannel);
    return 0;
}

int HY_Enc_ReadOneFrame(CChannelContext *pChannelContext,
                             char **pFirstDataBuf,
                               int *pFirstLength,
                               int *pFrameType,
                               int CallBackType)
{
    CChannelContext *pChCtx = pChannelContext;

    *pFrameType = 0;
    if (CallBackType == STREAM_CALLBACK_MAIN)          // 主码流
    {
        if (0 != Hi_MPP_ReadStream(pChCtx->pPlatformChannelhandle, pFirstDataBuf, pFirstLength, &pChCtx->m_RealTime, &pChCtx->m_RealTime_USec,
                                   pFrameType, pChCtx->m_EncType, FIRST_STREAM, FIRST_SENSOR))
        {
            return -1;
        }
    }
    else if (CallBackType == STREAM_CALLBACK_SUB)    // 子码流
    {
        if(0 != Hi_MPP_ReadStream(pChCtx->pPlatformChannelhandle, pFirstDataBuf, pFirstLength, &pChCtx->m_RealTime, &pChCtx->m_RealTime_USec,
                                   pFrameType, pChCtx->m_AuxEncType, SECOND_STREAM, FIRST_SENSOR))
        {
            return -1;
        }
        if(*pFrameType == PktIFrames)
            *pFrameType = PktQCIFIFrames;
        else if(*pFrameType == PktPFrames)
            *pFrameType = PktQCIFPFrames;
    }
#if (SYSTEM_SUPPORT_CHANNEL >= 2)
    else if (CallBackType == STREAM_CALLBACK_MAIN_SEC)     // 第二sensor 主码流
    {
        if(0 != Hi_MPP_ReadStream(pChCtx->pPlatformChannelhandle, pFirstDataBuf, pFirstLength, &pChCtx->m_RealTime, &pChCtx->m_RealTime_USec,
                                   pFrameType, pChCtx->m_EncType, FIRST_STREAM, SEC_SENSOR))
        {
            return -1;
        }
        if (*pFrameType == PktIFrames)
            *pFrameType = PktIFramesForS1;
        else if (*pFrameType == PktPFrames)
            *pFrameType = PktPFramesForS1;
    }
    else if (CallBackType == STREAM_CALLBACK_SUB_SEC)    // 第二sensor 子码流
    {
        if(0 != Hi_MPP_ReadStream(pChCtx->pPlatformChannelhandle, pFirstDataBuf, pFirstLength, &pChCtx->m_RealTime, &pChCtx->m_RealTime_USec,
                                   pFrameType, pChCtx->m_AuxEncType, SECOND_STREAM, SEC_SENSOR))
        {
            return -1;
        }
        if(*pFrameType == PktIFrames)
            *pFrameType = PktQCIFIFramesForS1;
        else if(*pFrameType == PktPFrames)
            *pFrameType = PktQCIFPFramesForS1;
    }
    else if (CallBackType == STREAM_CALLBACK_MJPEG_SEC)   // 第二sensor 四码流
    {
        if(0 != Hi_MPP_ReadStream(pChCtx->pPlatformChannelhandle, pFirstDataBuf, pFirstLength, &pChCtx->m_RealTime, &pChCtx->m_RealTime_USec,
                                   pFrameType, pChCtx->m_FourthEncType, FOURTH_STREAM, SEC_SENSOR))
        {
            return -1;
        }
        if(*pFrameType == PktIFrames)
            *pFrameType = PktFourthIFramesForS1;
        else if(*pFrameType == PktPFrames)
            *pFrameType = PktFourthPFramesForS1;
    }
#endif
#if (SYSTEM_SUPPORT_CHANNEL >= 3)
    else if (CallBackType == STREAM_CALLBACK_MAIN_THIRD)     // 第三sensor 主码流
    {
        if(0 != Hi_MPP_ReadStream(pChCtx->pPlatformChannelhandle, pFirstDataBuf, pFirstLength, &pChCtx->m_RealTime, &pChCtx->m_RealTime_USec,
                                   pFrameType, pChCtx->m_EncType, FIRST_STREAM, THIRD_SENSOR))
        {
            return -1;
        }
        if (*pFrameType == PktIFrames)
            *pFrameType = PktIFramesForS2;
        else if (*pFrameType == PktPFrames)
            *pFrameType = PktPFramesForS2;
    }
    else if (CallBackType == STREAM_CALLBACK_SUB_THIRD)    // 第三sensor 子码流
    {
        if(0 != Hi_MPP_ReadStream(pChCtx->pPlatformChannelhandle, pFirstDataBuf, pFirstLength, &pChCtx->m_RealTime, &pChCtx->m_RealTime_USec,
                                   pFrameType, pChCtx->m_AuxEncType, SECOND_STREAM, THIRD_SENSOR))
        {
            return -1;
        }
        if(*pFrameType == PktIFrames)
            *pFrameType = PktQCIFIFramesForS2;
        else if(*pFrameType == PktPFrames)
            *pFrameType = PktQCIFPFramesForS2;
    }
    else if (CallBackType == STREAM_CALLBACK_MJPEG_THIRD)   // 第三sensor 四码流
    {
        if(0 != Hi_MPP_ReadStream(pChCtx->pPlatformChannelhandle, pFirstDataBuf, pFirstLength, &pChCtx->m_RealTime, &pChCtx->m_RealTime_USec,
                                   pFrameType, pChCtx->m_FourthEncType, FOURTH_STREAM, THIRD_SENSOR))
        {
            return -1;
        }
        if(*pFrameType == PktIFrames)
            *pFrameType = PktFourthIFramesForS2;
        else if(*pFrameType == PktPFrames)
            *pFrameType = PktFourthPFramesForS2;
    }

#endif
#if (SYSTEM_SUPPORT_CHANNEL >= 4)
    else if (CallBackType == STREAM_CALLBACK_MAIN_FOURTH)     // 第四sensor 主码流
    {
        if(0 != Hi_MPP_ReadStream(pChCtx->pPlatformChannelhandle, pFirstDataBuf, pFirstLength, &pChCtx->m_RealTime, &pChCtx->m_RealTime_USec,
                                   pFrameType, pChCtx->m_EncType, FIRST_STREAM, FOURTH_SENSOR))
        {
            return -1;
        }
        if (*pFrameType == PktIFrames)
            *pFrameType = PktIFramesForS3;
        else if (*pFrameType == PktPFrames)
            *pFrameType = PktPFramesForS3;
    }
    else if (CallBackType == STREAM_CALLBACK_SUB_FOURTH)    // 第四sensor 子码流
    {
        if(0 != Hi_MPP_ReadStream(pChCtx->pPlatformChannelhandle, pFirstDataBuf, pFirstLength, &pChCtx->m_RealTime, &pChCtx->m_RealTime_USec,
                                   pFrameType, pChCtx->m_AuxEncType, SECOND_STREAM, FOURTH_SENSOR))
        {
            return -1;
        }
        if(*pFrameType == PktIFrames)
            *pFrameType = PktQCIFIFramesForS3;
        else if(*pFrameType == PktPFrames)
            *pFrameType = PktQCIFPFramesForS3;
    }
    else if (CallBackType == STREAM_CALLBACK_MJPEG_FOURTH)   // 第四sensor 四码流
    {
        if(0 != Hi_MPP_ReadStream(pChCtx->pPlatformChannelhandle, pFirstDataBuf, pFirstLength, &pChCtx->m_RealTime, &pChCtx->m_RealTime_USec,
                                   pFrameType, pChCtx->m_FourthEncType, FOURTH_STREAM, FOURTH_SENSOR))
        {
            return -1;
        }
        if(*pFrameType == PktIFrames)
            *pFrameType = PktFourthIFramesForS3;
        else if(*pFrameType == PktPFrames)
            *pFrameType = PktFourthPFramesForS3;
    }
#endif
    else if (CallBackType == STREAM_CALLBACK_MJPEG)   // 四码流
    {
        if(0 != Hi_MPP_ReadStream(pChCtx->pPlatformChannelhandle, pFirstDataBuf, pFirstLength, &pChCtx->m_RealTime, &pChCtx->m_RealTime_USec,
                                   pFrameType, pChCtx->m_FourthEncType, FOURTH_STREAM, FIRST_SENSOR))
        {
            return -1;
        }
        if(*pFrameType == PktIFrames)
            *pFrameType = PktFourthIFrames;
        else if(*pFrameType == PktPFrames)
            *pFrameType = PktFourthPFrames;
    }
    else if (CallBackType == STREAM_CALLBACK_AUDIO)  // 音频流
    {
        PH264DATAPACKETHEADER pHead = (PH264DATAPACKETHEADER)pChCtx->m_AudioStreamInfo.pBuf;
        pChCtx->m_RealTime = pHead->u32RealTimeSec;
        pChCtx->m_RealTime_USec = pHead->u32RealTimeUSec;

        *pFirstDataBuf = pChCtx->m_AudioStreamInfo.pBuf;
        *pFirstLength = pChCtx->m_AudioStreamInfo.WritePos;
        pChCtx->m_AudioStreamInfo.WritePos = 0;
        *pFrameType = PktAudioFrames;
    }
    else if (CallBackType == STREAM_CALLBACK_MOTION
        ||CallBackType == STREAM_CALLBACK_MOTION_SEC
        ||CallBackType == STREAM_CALLBACK_MOTION_THIRD
        ||CallBackType == STREAM_CALLBACK_MOTION_FOURTH)  // 移动侦测
    {
        *pFirstDataBuf = pChCtx->pUsrBlockState;
        *pFirstLength = 192;
        *pFrameType = PktMotionDetection;
    }
    else if (CallBackType == STREAM_CALLBACK_HUMANSHAPE
        ||CallBackType == STREAM_CALLBACK_HUMANSHAPE_SEC
        ||CallBackType == STREAM_CALLBACK_HUMANSHAPE_THIRD
        ||CallBackType == STREAM_CALLBACK_HUMANSHAPE_FOURTH)  // 人形检测
    {
        *pFrameType = PktIAFrames;
        if (0 != Hi_MPP_ReadStream(pChCtx->pPlatformChannelhandle, pFirstDataBuf,
            pFirstLength, &pChCtx->m_RealTime, &pChCtx->m_RealTime_USec,
            pFrameType, pChCtx->m_FourthEncType, FOURTH_STREAM, FIRST_SENSOR))
        {
            *pFrameType = PktError;
            return -1;
        }

        *pFrameType = PktIAFrames;
    }
    else
    {
        ZW_ERROR("CallBackType = %d Error\n", CallBackType);
        return -1;
    }

    //ZW_DEBUG("CallBackType=%d, encType=%d-%d-%d, byFrameType=%d\n",
    //    CallBackType, pChCtx->m_EncType, pChCtx->m_AuxEncType, pChCtx->m_ThirdEncType, *pFrameType);
    return 0;
}


#ifdef __DIGITAL_ZOOM_DRAG_SUPPORT__
int HY_Enc_SetZoomOSDBitMap(CChannelContext *pChCtx, ENCODE_STREAM_E eStmTyp, BOOL bEn)
{
    RECTH OSDRect;                     //显示区域
    UINT32 FontSzWidth = 0, FontSzHeight = 0;   // 放大后的字符宽高，英文
    UINT32 ScaleWidth = 0, ScaleHeight = 0;
    XRS_StreamAttr *pstStmAttr = NULL;
    CPlatformChannelContext *pPlatCtx = (CPlatformChannelContext *)pChCtx->pPlatformChannelhandle;
    int spaceTop = 0, spaceLeft = 0;
    int StreamWidth = 0, StreamHeight = 0;
    ENCODE_STREAM_E eStm = FIRST_STREAM;
    BOOL bSingle = FALSE;
    VOSD_OVERYLAY_S PVosdOverLay[SUPPORT_MAX_SENSOR_COUNT] = {0};
    int ret = 0;
    char                ptmp;
    unsigned char        bit;
    const unsigned char *pixel;
    unsigned char         *pDst2;
    const unsigned char    *pSrc;
    int                    stride;
    int pixelCount = 0;
    int offset_byte = 0;
    int offset_bit = 0;
    int                    i, k;
    int                    MallocSize;

    ZoomOSD_t        *pZoomOsdInfo = NULL;
    int BlockNum = 0;

    if (NULL == pChCtx)
    {
        ZW_ERROR("pChannelContext CANNOT be NULL\n");
        return -1;
    }

    /* 变倍 OSD 初始化 */
    pZoomOsdInfo = &pChCtx->m_ZoomOsdInfo;
    pZoomOsdInfo->FontWidth   = OSD_ZOOM_UNICODE_W;
    pZoomOsdInfo->FontHeight  = OSD_ZOOM_UNICODE_H;
    pZoomOsdInfo->nFontNum    = OSD_ZOOM_MAX_LENGTH;
    pZoomOsdInfo->rect.left   = 176;
    pZoomOsdInfo->rect.top    = 250;
    pZoomOsdInfo->rect.right  = 176 + OSD_ZOOM_PIC_WIDTH;
    pZoomOsdInfo->rect.bottom = 250 + OSD_ZOOM_PIC_HEIGHT;
    BlockNum = pZoomOsdInfo->nFontNum;

    /* 重新分配15个关键字点阵内存,白色、黑色各一套，到时候直接贴图 */
    // TODO:实际使用只有白色一套，可以去掉黑色关键字的内存占用
    HIEASY_FREE(pZoomOsdInfo->pWhiteFontInfo);
    MallocSize = pZoomOsdInfo->FontWidth * pZoomOsdInfo->FontHeight * 13 / BYTE_PIXELS;

    pZoomOsdInfo->pWhiteFontInfo = (char *)malloc(MallocSize);
    if(NULL == pZoomOsdInfo->pWhiteFontInfo)
    {
        ZW_ERROR("pFontInfo malloc fail, w = %d, h = %d\n", pZoomOsdInfo->FontWidth, pZoomOsdInfo->FontHeight);
                            //释放接口锁
        return ERR_MALLOC_ERROR;
    }

    memset(pZoomOsdInfo->pWhiteFontInfo, 0, MallocSize);
    /* 转换海思需要的点阵 */
    pSrc = DEFAULT_ZOOMOSD_PIXEL.pixel_buf;
    pDst2    = (unsigned char *)pZoomOsdInfo->pWhiteFontInfo;
    stride = (pZoomOsdInfo->FontWidth + 7) >> 3;
    for (i = 0; i < 13 * pZoomOsdInfo->FontHeight; i++)
    {
        pixel = pSrc + i * stride;
        ptmp = *pixel;
        bit = 0;
        for (k = 0; k < pZoomOsdInfo->FontWidth; k++)
        {
            if(bit == 0)
            {
                ptmp = *pixel ++;
                bit = 0x80;
            }

            offset_byte = pixelCount / BYTE_PIXELS;
            offset_bit = (pixelCount % BYTE_PIXELS) * PIXEL_BITS;
            if ((ptmp & bit) != 0)
            {
                pDst2[offset_byte] |= (COLOR_WITHE << offset_bit);
            }

            pixelCount ++;
            bit = (bit >> 1) & 0xFF;
        }
    }

    ZW_DEBUG("[OSD][TIME] BlockNum = %d\n", BlockNum);
    // 修复因为区块放大比例小于分辨率比例导致底部留空的问题
    spaceTop = (pZoomOsdInfo->rect.top < 0) ? 0 : pZoomOsdInfo->rect.top;
    spaceLeft = (pZoomOsdInfo->rect.left < 0) ? 0 : pZoomOsdInfo->rect.left;


    // 如果流类型为MAX_STREAM_COUNT则设置所有通道，否则设置指定通道
    bSingle = (MAX_STREAM_COUNT != eStmTyp);
    for (eStm = (bSingle ? eStmTyp : 0); eStm < (bSingle ? eStmTyp + 1 : eStmTyp); eStm++)
    {
#if defined __ISP_IMAGE_DEBUG__
        if (eStm != FIRST_STREAM)
        {
            continue;
        }
#endif
        int j = 0;
        pstStmAttr = &pPlatCtx->m_stStreamAttr[FIRST_SENSOR][eStm];

        if (!pstStmAttr->bEnable || !pstStmAttr->bUseOSD)
            continue;

        if(bEn)//使能才获取内存地址
        {
            StreamWidth = pChCtx->m_PictureRes[eStm].m_PicWidth;
            StreamHeight = pChCtx->m_PictureRes[eStm].m_PicHeight;

            GetOSDFontSizeByRes(StreamWidth, StreamHeight, (int *)&ScaleWidth, (int *)&ScaleHeight);
            FontSzWidth = pZoomOsdInfo->FontWidth*ScaleWidth;
            FontSzHeight = pZoomOsdInfo->FontHeight * ScaleHeight;

            ZW_INFO("[OSD][TIME] Ch Width: %d, Height = %d, FontSzWidth = %d, FontSzHeight = %d\n",
                 pChCtx->m_PictureRes[eStm].m_PicWidth, pChCtx->m_PictureRes[eStm].m_PicHeight, FontSzWidth, FontSzHeight);

            /* 更新各个通道的时间点阵，默认按照白色开始启动叠加 */
            for (j = 0; j < BlockNum; j++)
            {
                pChCtx->BlockAscValue[eStm][j] = -1;
            }

            pChCtx->ZoomBlockNum[eStm] = BlockNum;
            pPlatCtx->ZoomBitmapWidth[eStm] = BlockNum * FontSzWidth ;
            pPlatCtx->ZoomBitmapHeight[eStm] = FontSzHeight;

            OSDRect.left = spaceLeft * StreamWidth / 352;
            if(OSDRect.left < (1 * 8 * ScaleWidth))
            {
                OSDRect.left = 1 * 8 * ScaleWidth;
            }
            else if((OSDRect.left + pPlatCtx->ZoomBitmapWidth[eStm]) > (StreamWidth * 9 /10))
            {
                //右边框的位置大于总宽度的90%，直接贴边(留一个字符)
                OSDRect.left = StreamWidth - pPlatCtx->ZoomBitmapWidth[eStm] - 1 * 8 * ScaleWidth;
            }
            OSDRect.right = OSDRect.left + pPlatCtx->ZoomBitmapWidth[eStm];

            OSDRect.top = spaceTop * StreamHeight / 288;
            if(OSDRect.top < (1 * 8 * ScaleHeight))
            {
                //上面预留一个字符位置
                OSDRect.top = 1 * 8 * ScaleHeight;
            }
            else if((OSDRect.top + pPlatCtx->ZoomBitmapHeight[eStm]) > StreamHeight)
            {
                //下面预留一个字符位置
                OSDRect.top = StreamHeight - pPlatCtx->ZoomBitmapHeight[eStm] - 1 * 8 * ScaleHeight;
            }
            OSDRect.bottom = OSDRect.top + pPlatCtx->ZoomBitmapHeight[eStm];

            memcpy(&pChCtx->m_ZoomOSDRect[eStm], &OSDRect, sizeof(RECTH));

            for (unsigned int i = 0; i < SUPPORT_MAX_SENSOR_COUNT; i++)
            {
                /* 转换海思需要的点阵 */
                PVosdOverLay[i].Rect.x = OSDRect.left;
                PVosdOverLay[i].Rect.y = OSDRect.top;
                PVosdOverLay[i].Rect.w = OSDRect.right-OSDRect.left;
                PVosdOverLay[i].Rect.h = OSDRect.bottom-OSDRect.top;
                ZW_DEBUG("PVosdOverLay.Rect.x(%d) PVosdOverLay.Rect.y(%d) PVosdOverLay.Rect.w(%d) PVosdOverLay.Rect.h(%d)\n",PVosdOverLay[i].Rect.x,PVosdOverLay[i].Rect.y,PVosdOverLay[i].Rect.w,PVosdOverLay[i].Rect.h);

                #if 1
                //OSD设置流程 删除上一次-->创建-->刷新
                //Hi_MPP_SetOSD(pPlatCtx, i, eStm, ZOOM_TYPE, 0);//创建之前先删除上一次句柄
                ret = Hi_MPP_SetBitMapAttrToGetAddrFromPlat(pPlatCtx, i, eStm, ZOOM_TYPE, &(PVosdOverLay[FIRST_SENSOR]), 1);
                if(ret)
                {
                 // pChCtx->m_bCreatTextOSDRegion = FALSE; //这里给false可能会出现子码流申请地址失败导致主码流的叠加刷新不了
                 ZW_ERROR("Hi_MPP_SetBitMapAttrToGetAddrFromPlat fail(%d)\n", ret);
                 continue;
                }
                pPlatCtx->m_stStreamAttr[i][eStm].pZoomBitmap = PVosdOverLay[i].pData;
                #endif
                Hi_MPP_GetZoomBitmap(pPlatCtx, i, eStm);
            }
        }

#if 0
            // 更新bitmap
            for (unsigned int i = 0; i < SUPPORT_MAX_SENSOR_COUNT; i++)
            {
                Hi_MPP_SetOSD(pPlatCtx, i, eStm, ZOOM_TYPE, bEn);
            }

#else
            Hi_MPP_SetOSD(pPlatCtx, 0, eStm, ZOOM_TYPE, bEn);

#endif

    }

    return 0;
}
#endif

/**
 * @description: 设置时间OSD的ARGB1555点阵信息
 * @param {CChannelContext} *pChCtx 通道句柄
 * @param {SysTimeOSD_t} *pSysTime 时间OSD配置
 * @param {STREAM_TYPE_E} eStmTyp 流类型，MAX_STREAM_COUNT指所有通道
 * @param {BOOL} bEn 是否使能显示
 * @return {SINT32} 0-成功，非0-失败
 */
int HY_Enc_SetTimeOSDBitMap(CChannelContext *pChCtx, SysTimeOSD_t *pSysTime, ENCODE_STREAM_E eStmTyp, BOOL bEn)
{
    int BlockNum;
    RECTH OSDRect;                       //显示区域
    time_t CurTime;
    UINT32 FontSzWidth = 0, FontSzHeight = 0;    // 放大后的字符宽高，英文
    int ScaleWidth = 0, ScaleHeight = 0;
    XRS_StreamAttr *pstStmAttr = NULL;
    CPlatformChannelContext *pPlatCtx = (CPlatformChannelContext *)pChCtx->pPlatformChannelhandle;
    int spaceTop = 0, spaceLeft = 0;
    int StreamWidth = 0, StreamHeight = 0;
    ENCODE_STREAM_E eStm = FIRST_STREAM;
    BOOL bSingle = FALSE;
    VOSD_OVERYLAY_S PVosdOverLay[SUPPORT_MAX_SENSOR_COUNT] = {0};
    int ret = 0;
    int MaxSns = HY_Res_GetMaxSensorCnt();

    if(NULL == pSysTime->pWhiteFontInfo)
    {
        return -1;
    }

    if (pSysTime->bHour24) //显示格式：2008-06-06 09:28:32        19个宏块
        BlockNum = 19;
    else //显示格式：2008-06-06 AM 09:28:32        22个宏块
        BlockNum = 22;

    ZW_DEBUG("[OSD][TIME] BlockNum = %d\n", BlockNum);

    // 修复因为区块放大比例小于分辨率比例导致底部留空的问题
    spaceTop = (pSysTime->rect.top < 0) ? 0 : pSysTime->rect.top;
    spaceLeft = (pSysTime->rect.left < 0) ? 0 : pSysTime->rect.left;

    // 如果流类型为MAX_STREAM_COUNT则设置所有通道，否则设置指定通道
    bSingle = (MAX_STREAM_COUNT != eStmTyp);
    for (eStm = (bSingle ? eStmTyp : 0); eStm < (bSingle ? eStmTyp + 1 : eStmTyp); eStm++)
    {
        if (eStm != FIRST_STREAM && eStm != SECOND_STREAM && eStm != FOURTH_STREAM)
        {
            continue;
        }

#if defined __ISP_IMAGE_DEBUG__
        if (eStm != FIRST_STREAM)
        {
            continue;
        }
#endif
        int j = 0;
        pstStmAttr = &pPlatCtx->m_stStreamAttr[FIRST_SENSOR][eStm];

        if (!pstStmAttr->bEnable || !pstStmAttr->bUseOSD)
            continue;

        if(bEn)//使能才获取内存地址
        {
            StreamWidth = pChCtx->m_PictureRes[eStm].m_PicWidth;
            StreamHeight = pChCtx->m_PictureRes[eStm].m_PicHeight;

            GetOSDFontSizeByRes(StreamWidth, StreamHeight, &ScaleWidth, &ScaleHeight);
            FontSzWidth = pSysTime->FontWidth*ScaleWidth;
            FontSzHeight = pSysTime->FontHeight * ScaleHeight;

            ZW_INFO("[OSD][TIME] Ch Width: %d, Height = %d, FontSzWidth = %d, FontSzHeight = %d\n",
                    pChCtx->m_PictureRes[eStm].m_PicWidth, pChCtx->m_PictureRes[eStm].m_PicHeight, FontSzWidth, FontSzHeight);

            /* 更新各个通道的时间点阵，默认按照白色开始启动叠加 */
            for (j = 0; j < BlockNum; j++)
            {
                pChCtx->BlockAscValue[eStm][j] = -1;
            }

            pChCtx->TimeBlockNum[eStm] = BlockNum;
            pPlatCtx->TimeBitmapWidth[eStm] = BlockNum * FontSzWidth ;
            pPlatCtx->TimeBitmapHeight[eStm] = FontSzHeight;

            OSDRect.left = spaceLeft * StreamWidth / 352;
            if(OSDRect.left < (1 * 8 * ScaleWidth))
            {
                OSDRect.left = 1 * 8 * ScaleWidth;
            }
            else if((OSDRect.left + pPlatCtx->TimeBitmapWidth[eStm]) > (StreamWidth * 9 /10))
            {
                //右边框的位置大于总宽度的90%，直接贴边(留一个字符)
                OSDRect.left = StreamWidth - pPlatCtx->TimeBitmapWidth[eStm] - 1 * 8 * ScaleWidth;
            }
            OSDRect.right = OSDRect.left + pPlatCtx->TimeBitmapWidth[eStm];

            OSDRect.top = spaceTop * StreamHeight / 288;
            if(OSDRect.top < (1 * 8 * ScaleHeight))
            {
                //上面预留一个字符位置
                OSDRect.top = 1 * 8 * ScaleHeight;
            }
            else if((OSDRect.top + pPlatCtx->TimeBitmapHeight[eStm]) > StreamHeight)
            {
                //下面预留一个字符位置
                OSDRect.top = StreamHeight - pPlatCtx->TimeBitmapHeight[eStm] - 1 * 8 * ScaleHeight;
            }
            OSDRect.bottom = OSDRect.top + pPlatCtx->TimeBitmapHeight[eStm];

            memcpy(&pChCtx->m_TimeOSDRect[eStm], &OSDRect, sizeof(RECTH));
            for (unsigned int i = 0; i < MaxSns; i++)
            {
                /* 转换海思需要的点阵 */
                PVosdOverLay[i].Rect.x = OSDRect.left;
                PVosdOverLay[i].Rect.y = OSDRect.top;
                PVosdOverLay[i].Rect.w = OSDRect.right-OSDRect.left;
                PVosdOverLay[i].Rect.h = OSDRect.bottom-OSDRect.top;

                //OSD设置流程 删除上一次-->创建-->刷新
                Hi_MPP_SetOSD(pPlatCtx, i, eStm, TIME_TYPE, 0);//创建之前先删除上一次句柄

                ZW_DEBUG("PVosdOverLay.Rect.x(%d) PVosdOverLay.Rect.y(%d) PVosdOverLay.Rect.w(%d) PVosdOverLay.Rect.h(%d)\n",PVosdOverLay[i].Rect.x,PVosdOverLay[i].Rect.y,PVosdOverLay[i].Rect.w,PVosdOverLay[i].Rect.h);

                ret = Hi_MPP_SetBitMapAttrToGetAddrFromPlat(pPlatCtx, i, eStm, TIME_TYPE, &(PVosdOverLay[i]), 1);
                if(ret)
                {
                    // pChCtx->m_bCreatTextOSDRegion = FALSE; //这里给false可能会出现子码流申请地址失败导致主码流的叠加刷新不了
                    ZW_ERROR("Hi_MPP_SetBitMapAttrToGetAddrFromPlat fail(%d)\n", ret);
                    continue;
                }
                pPlatCtx->m_stStreamAttr[i][eStm].pTimeBitmap = PVosdOverLay[i].pData;

                time(&CurTime);
                Hi_MPP_GetTimeBitmap(pPlatCtx, &CurTime, i, eStm);
            }
        }

        // 更新bitmap
        for (unsigned int i = 0; i < MaxSns; i++)
        {
            Hi_MPP_SetOSD(pPlatCtx, i, eStm, TIME_TYPE, bEn);
        }
    }

    return 0;
}

/**
 * @description: 设置文本OSD ARGB1555点阵信息
 * @param {CChannelContext} *pChCtx 通道句柄
 * @param {STREAM_TYPE_E} eStmTyp 流类型，MAX_STREAM_COUNT指所有通道
 * @param {BOOL} bEn 是否使能显示
 * @param {int} index 0-文本OSD1, 1-文本OSD2
 * @return {SINT32} 0-成功，非0-失败
 */
SINT32 HY_Enc_SetTextOSDBitMap(CChannelContext *pChCtx, ENCODE_STREAM_E eStmTyp, BOOL bEn)
{
    unsigned char *pDst2;
    char *pSrc;
    unsigned char ptmp, bit;
    char *pixel;
    int TotalStride, TotalWidth, TotalHeight;
    int i, j, m, n;
    RECTH OSDRect;          //显示区域
    RECTH OSDRect1,OSDRect2;
    int spaceTop = 0, spaceLeft = 0;
    XRS_StreamAttr *pstStmAttr = NULL;
    CPlatformChannelContext *pPlatCtx = NULL;
    ENCODE_STREAM_E eStm = FIRST_STREAM;
    int ScaleWidth = 0, ScaleHeight = 0;    // 根据不同分辨率缩放之后的大小
    int StmWidth = 0, StmHeight = 0;    // 当前通道的分辨率
    BOOL bSingle = FALSE;
    VOSD_OVERYLAY_S PVosdOverLay[SUPPORT_MAX_SENSOR_COUNT] = {0};
    int ret = 0;
    int offset_byte = 0;
    int offset_bit = 0;
    int stride = 0;
    int line_index = 0;
    int bitmap_index = 0;
    int MaxSns = HY_Res_GetMaxSensorCnt();

    if (NULL == pChCtx)
        return -1;

    pPlatCtx = pChCtx->pPlatformChannelhandle;

    TotalWidth = pChCtx->TextOSDRect_Src.right - pChCtx->TextOSDRect_Src.left;
    TotalHeight = pChCtx->TextOSDRect_Src.bottom - pChCtx->TextOSDRect_Src.top;
    TotalStride = (TotalWidth + 7) >> 3;

    TotalWidth = (TotalWidth + 0xF) & 0xFFF0;
    // TotalHeight = (TotalHeight + 0xF) & 0xFFF0;

    //ZW_DEBUG("[OSD][TEXT[%d]]TotalWidth:%d,TotalHeight:%d, pChCtx->m_PictureRes: w=%d h=%d\n", index,
    //         TotalWidth, TotalHeight, pChCtx->m_PictureRes[FIRST_STREAM].m_PicWidth, pChCtx->m_PictureRes[FIRST_STREAM].m_PicHeight);
    for (int nSenIndex = 0; nSenIndex < MaxSns; nSenIndex++)
    {
        // 如果流类型为MAX_STREAM_COUNT则设置所有通道，否则设置指定通道
        bSingle = (MAX_STREAM_COUNT != eStmTyp);
        for (eStm = (bSingle ? eStmTyp : 0); eStm < (bSingle ? eStmTyp + 1 : eStmTyp); eStm++)
        {
            if (eStm != FIRST_STREAM && eStm != SECOND_STREAM && eStm != FOURTH_STREAM)
            {
                continue;
            }

#if defined __ISP_IMAGE_DEBUG__
            if (eStm != FIRST_STREAM)
            {
                continue;
            }
#endif
            pstStmAttr = &pPlatCtx->m_stStreamAttr[nSenIndex][eStm];

            ZW_DEBUG("pstStmAttr->bEnable(%d)  pstStmAttr->bUseOSD(%d) bEn(%d) eStm(%d)\n",pstStmAttr->bEnable,pstStmAttr->bUseOSD,bEn,eStm);

            // 通道不存在或者不启用OSD则直接退出
            if (!pstStmAttr->bEnable || !pstStmAttr->bUseOSD)
            {
                continue;
            }

            // 使能开启才获取内存地址
            if (bEn)
            {
                StmWidth = pChCtx->m_PictureRes[eStm].m_PicWidth;
                StmHeight = pChCtx->m_PictureRes[eStm].m_PicHeight;

                // 获取当前分辨率对应的字体大小
                GetOSDFontSizeByRes(StmWidth, StmHeight, &ScaleWidth, &ScaleHeight);

                //ZW_DEBUG("StmWidth(%d) StmHeight(%d) ScaleWidth(%d) ScaleHeight(%d)\n",StmWidth,StmHeight, ScaleWidth, ScaleHeight);
                // 修复因为区块放大比例小于分辨率比例导致底部留空的问题
                spaceTop = (pChCtx->TextOSDRect_Src.top < 0) ? 0 : pChCtx->TextOSDRect_Src.top;
                spaceLeft = (pChCtx->TextOSDRect_Src.left < 0) ? 0 : pChCtx->TextOSDRect_Src.left;

                pChCtx->TextBitmapWidth[eStm] = TotalWidth * ScaleWidth;
                pChCtx->TextBitmapHeight[eStm] = TotalHeight * ScaleHeight;

                OSDRect.left = spaceLeft * StmWidth / 352;
                if(OSDRect.left < 1 * 8 * ScaleWidth)
                {
                    //左边预留一个字符位置
                    OSDRect.left = 1 * 8 * ScaleWidth;
                }
                else if((OSDRect.left + pChCtx->TextBitmapWidth[eStm]) > (StmWidth * 9 /10))
                {
                    //大于总宽度的90%，直接贴边(留一个字符的位置)
                    OSDRect.left = StmWidth - pChCtx->TextBitmapWidth[eStm] - 1 * 8 * ScaleWidth;
                }
                OSDRect.left = ((OSDRect.left + 0xf) & 0xfff0);
                OSDRect.right = OSDRect.left + pChCtx->TextBitmapWidth[eStm];

                OSDRect.top = spaceTop * StmHeight / 288;
                // OSDRect.top = ((OSDRect.top+0xf)&0xfff0); //y不需要对齐,否则可能会把OSD截掉一点
                if(OSDRect.top < 1 * 8 * ScaleHeight)
                {
                    //上面预留一个字符位置
                    OSDRect.top = 1 * 8 * ScaleHeight;
                }
                else if(OSDRect.top + pChCtx->TextBitmapHeight[eStm] > StmHeight)
                {
                    OSDRect.top = StmHeight - pChCtx->TextBitmapHeight[eStm] - 1 * 8 * ScaleHeight;
                }
                OSDRect.bottom = OSDRect.top + pChCtx->TextBitmapHeight[eStm];

                //ZW_DEBUG("eStm(%d) OSDRect.left(%d) OSDRect.right(%d) OSDRect.top(%d) OSDRect.bottom(%d)\n",eStm,OSDRect.left,OSDRect.right,OSDRect.top,OSDRect.bottom);
                if (pChCtx->m_bEnableTimeOSD)
                {
                    memcpy(&OSDRect1, &OSDRect, sizeof(RECTH));
                    CompareModifyOsdRegion(pChCtx, eStm, StmHeight, TEXT_TYPE, &OSDRect, 0);
                    memcpy(&OSDRect2, &OSDRect, sizeof(RECTH));
                }
                memcpy(&pChCtx->m_TextOSDRect[eStm], &OSDRect, sizeof(RECTH));

                /* 转换海思需要的点阵 */
                PVosdOverLay[nSenIndex].Rect.x = OSDRect.left;
                PVosdOverLay[nSenIndex].Rect.y = OSDRect.top;
                PVosdOverLay[nSenIndex].Rect.w = OSDRect.right-OSDRect.left;
                PVosdOverLay[nSenIndex].Rect.h = OSDRect.bottom-OSDRect.top;

                ZW_DEBUG("PVosdOverLay.Rect.x(%d) PVosdOverLay.Rect.y(%d) PVosdOverLay.Rect.w(%d) PVosdOverLay.Rect.h(%d)\n",PVosdOverLay[nSenIndex].Rect.x,PVosdOverLay[nSenIndex].Rect.y,PVosdOverLay[nSenIndex].Rect.w,PVosdOverLay[nSenIndex].Rect.h);
                //OSD设置流程 删除上一次-->创建-->刷新
                Hi_MPP_SetOSD(pPlatCtx, nSenIndex, eStm, TEXT_TYPE, 0);//获取之前先删除上一次句柄

                ret = Hi_MPP_SetBitMapAttrToGetAddrFromPlat(pPlatCtx, nSenIndex, eStm, TEXT_TYPE, &(PVosdOverLay[nSenIndex]), 1);
                if (ret)
                {
                    // pChCtx->m_bCreatTextOSDRegion = FALSE; //这里给false可能会出现子码流申请地址失败导致主码流的叠加刷新不了
                    ZW_ERROR("Hi_MPP_SetBitMapAttrToGetAddrFromPlat fail(%d)\n", ret);
                    continue;
                }
                stride = PVosdOverLay[nSenIndex].Color;
                pDst2 = (unsigned char *)PVosdOverLay[nSenIndex].pData;
                pPlatCtx->m_stStreamAttr[nSenIndex][eStm].pTextBitmap = PVosdOverLay[nSenIndex].pData;
                pSrc = (char *)pChCtx->pTextOSDBit_Src;

                //ZW_DEBUG("pDst2(%p) pSrc(%p)\n",pDst2,pSrc);
                for (i = 0; i < TotalHeight; i++)
                {
                    pixel = pSrc + i * TotalStride;
                    ptmp = *pixel;
                    bit = 0;
                    for (j = 0; j < TotalWidth; j++)
                    {
                        if (bit == 0)
                        {
                            if (j >= (TotalStride << 3))
                            {
                                ptmp = 0;
                            }
                            else
                            {
                                ptmp = *pixel++;
                            }
                            bit = 0x80;
                        }

                        // 像素放大，列
                        for (m = 0; m < ScaleHeight; m++)
                        {
                            // 行
                            for (n = 0; n < ScaleWidth; n++)
                            {
                                bitmap_index = (TotalWidth * ScaleWidth * (i * ScaleHeight + m) + j * ScaleWidth + n);
                                line_index = bitmap_index / (TotalWidth * ScaleWidth);
                                offset_byte = line_index * stride + (bitmap_index % (TotalWidth * ScaleWidth)) / BYTE_PIXELS;
                                offset_bit = (bitmap_index % BYTE_PIXELS) * PIXEL_BITS;

                                if ((ptmp & bit) != 0) //一个点放大为16个点
                                {
                                    pDst2[offset_byte] |= (COLOR_WITHE << offset_bit); // 白色
                                }
                                else
                                {
                                    pDst2[offset_byte] &= ~(BYTE_ROUND << offset_bit);
                                }
                            }
                        }

                        bit = (bit >> 1) & 0xFF;
                    }
                }

                font_stroke(TotalWidth*ScaleWidth, TotalHeight*ScaleHeight, pDst2, COLOR_WITHE, COLOR_BLACK, 1,stride);

                //OSD刷新线程标志，使能并且内存获取成功才置为TRUE
                pChCtx->m_bCreatTextOSDRegion = TRUE;
            }
            else
            {
                pChCtx->m_bCreatTextOSDRegion = FALSE;
            }

            Hi_MPP_SetOSD(pPlatCtx, nSenIndex, eStm, TEXT_TYPE, bEn);
        }
    }

    return 0;
}

int HY_Enc_StartOsd(CChannelContext *pChannelContext, ENCODE_OSD_TYPE eOsdType, ENCODE_STREAM_E eStream)
{
    switch (eOsdType)
    {
        case TEXT_TYPE:
            HY_Enc_SetTextOSDBitMap(pChannelContext, eStream, TRUE);
            break;
        case TIME_TYPE:
            HY_Enc_SetTimeOSDBitMap(pChannelContext, &pChannelContext->m_TimeOSDInfo, eStream, TRUE);
            break;
#ifdef __DIGITAL_ZOOM_DRAG_SUPPORT__
        case ZOOM_TYPE:
            HY_Enc_SetZoomOSDBitMap(pChannelContext, eStream, TRUE);
            break;
#endif
        default:
            ZW_ERROR("[OSD] Unsupport Osd Type: %d\n", eOsdType);
            break;
    }

    return 0;
}

int HY_Enc_StopOsd(CChannelContext *pChannelContext, ENCODE_OSD_TYPE eOsdType, ENCODE_STREAM_E eStream)
{
    switch (eOsdType)
    {
        case TEXT_TYPE:
            HY_Enc_SetTextOSDBitMap(pChannelContext, eStream, FALSE);
            break;
        case TIME_TYPE:
            HY_Enc_SetTimeOSDBitMap(pChannelContext, &pChannelContext->m_TimeOSDInfo, eStream, FALSE);
            break;
#ifdef __DIGITAL_ZOOM_DRAG_SUPPORT__
        case ZOOM_TYPE:
            HY_Enc_SetZoomOSDBitMap(pChannelContext, eStream, FALSE);
            break;
#endif
        default:
            ZW_ERROR("[OSD] Unsupport Osd Type: %d\n", eOsdType);
            break;
    }

    return 0;
}

int HY_Enc_MD_Start(CChannelContext *pChannelContext)
{
    int ret = 0;

    ret = Hi_MPP_StartMD(pChannelContext->pPlatformChannelhandle);

    return ret;
}

int HY_Enc_MD_Stop(CChannelContext *pChannelContext)
{
    int ret = 0;

    ret = Hi_MPP_StopMD(pChannelContext->pPlatformChannelhandle);

    return ret;
}

int HY_Enc_MD_SetPrecision(CChannelContext *pChannelContext)
{
    return Hi_MPP_MDSetPrecision(pChannelContext->pPlatformChannelhandle, pChannelContext->MotionFps, pChannelContext->MotionGrade);
}

int HY_Enc_SetVideoPara(CChannelContext *pChannelContext)
{
    int s32Ret = 0;

    s32Ret = Hi_MPP_SetImgPara(pChannelContext->pPlatformChannelhandle, pChannelContext->Bright, pChannelContext->Contrast,
                    pChannelContext->Hue, pChannelContext->Saturation, pChannelContext->Sharpness);
    if(s32Ret != 0)
    {
        ZW_ERROR("Hi_MPP_SetImgPara fail !\n");
        return -1;
    }

    return 0;
}

int HY_Enc_SetAEParam(CChannelContext *pChannelContext, AE_T *aeParam, int ae_mode)
{
    if (pChannelContext->m_ae_mode == ae_mode
        && pChannelContext->AEParam.antiFlicker == aeParam->antiFlicker
        && pChannelContext->AEParam.MinShutterTime == aeParam->MinShutterTime
        && pChannelContext->AEParam.MaxShutterTime == aeParam->MaxShutterTime
        && pChannelContext->AEParam.sensorGain == aeParam->sensorGain)
    {
        return 0;
    }

    if(Hi_MPP_SetAEParam(pChannelContext->pPlatformChannelhandle, aeParam->antiFlicker,
        aeParam->MinShutterTime, aeParam->MaxShutterTime, aeParam->sensorGain, ae_mode) < 0)
    {
        ZW_ERROR("Hi_MPP_SetAEParam fail !\n");
        return -1;
    }
    else
    {
        pChannelContext->m_ae_mode = ae_mode;
        memcpy(&pChannelContext->AEParam, aeParam, sizeof(AE_T));
    }

    return 0;
}

int HY_Enc_SetAntiFlickerStrengthParam(CChannelContext *pChannelContext, int strength)
{
    if (strength == pChannelContext->m_antiFlickerStrength)
    {
        return 0;
    }

    if(Hi_MPP_SetAntiFlickerStrengthParam(pChannelContext->pPlatformChannelhandle, strength) < 0)
    {
        ZW_ERROR("Hi_MPP_SetAntiFlickerStrengthParam fail !\n");
        return -1;
    }
    else
    {
        pChannelContext->m_antiFlickerStrength = strength;
    }

    return 0;
}

int HY_Enc_SetDefogStrengthParam(CChannelContext *pChannelContext, int strength)
{
    if (Hi_MPP_SetDefogStrengthParam(pChannelContext->pPlatformChannelhandle, strength) < 0)
    {
        ZW_ERROR("Hi_MPP_SetDefogStrengthParam fail !\n");
        return -1;
    }

    pChannelContext->FogStrength = strength;

    return 0;
}

int HY_Enc_SetAudioInVol(CChannelContext *pChannelContext, int vol, int IsClearCache)
{
    return Hi_MPP_SetAudioInVol(pChannelContext->pPlatformChannelhandle, vol, IsClearCache, 0);
}

int HY_Enc_SetAudioOutVol(CChannelContext *pChannelContext, int vol, int IsUpdateVoiceCache, int m_IntercomMode)
{
    return Hi_MPP_SetAudioOutVol(pChannelContext->pPlatformChannelhandle, vol, IsUpdateVoiceCache, m_IntercomMode);
}

int HY_Enc_SetAudioMappingVol(CChannelContext *pChannelContext, int vol, int Type)
{
    return Hi_MPP_SetAudioMappingVol(pChannelContext->pPlatformChannelhandle, vol, Type);
}



int HY_Enc_SetAudioCodec(CChannelContext *pChCtx)
{
    int ret = 0;
    SystemInfo_t *pSysInfo = NULL;
    CTalkingChannel_t *pTalkingCh = NULL;
    CPlatformChannelContext *pPlatCtx = NULL;

    if (NULL == pChCtx)
    {
        ZW_ERROR("pChCtx CANNOT be NULL\n");
        return -1;
    }

    pPlatCtx = pChCtx->pPlatformChannelhandle;

    HY_Res_GetSystemInfo(&pSysInfo);
    HY_Enc_GetTalkingChInfo(&pTalkingCh);

    XTRACE_MUTEX_LOCK(&pTalkingCh->m_Lock);
    XTRACE_MUTEX_LOCK(&pSysInfo->m_AudioInLock);

    ret = Hi_MPP_ChangeAudioEnc(pPlatCtx);
    if (ret < 0)
    {
        ZW_ERROR("Hi_MPP_ChangeAudioEnc Failed\n");
    }

    XTRACE_MUTEX_UNLOCK(&pSysInfo->m_AudioInLock);
    XTRACE_MUTEX_UNLOCK(&pTalkingCh->m_Lock);

    return ret;
}

int HY_Enc_SetBacklightMode(CChannelContext *pChannelContext,  int enable)
{
    if(Hi_MPP_EnableBacklightMode(pChannelContext->pPlatformChannelhandle, enable) < 0)
    {
        return -1;
    }

    return 0;
}

int HY_Enc_SetWhiteBalance(CChannelContext *pChannelContext, WHITEBALANCE_T *WhiteBalance)
{
    if(Hi_MPP_SetWhiteBalance(pChannelContext->pPlatformChannelhandle, WhiteBalance->WBEnable, WhiteBalance->WBWorkMode, WhiteBalance->RedGain, WhiteBalance->BlueGain) < 0)
    {
        ZW_ERROR("AMB_SetWhiteBalanceMode err !\n");
        return -1;
    }
    else
    {
        pChannelContext->WhiteBalance.WBWorkMode = WhiteBalance->WBWorkMode;
        pChannelContext->WhiteBalance.WBEnable   = WhiteBalance->WBEnable;
        pChannelContext->WhiteBalance.RedGain    = WhiteBalance->RedGain;
        pChannelContext->WhiteBalance.BlueGain   = WhiteBalance->BlueGain;
    }

    return 0;
}

int HY_Enc_SetMCTFStrength(CChannelContext *pChannelContext, int MCTFStrength)
{
    if(Hi_MPP_SetMCTFStrength(pChannelContext->pPlatformChannelhandle, MCTFStrength) != 0)
    {
        return -1;
    }
    else
    {
        pChannelContext->MCTFStrength = MCTFStrength;
    }

    return 0;
}

int HY_Enc_SetDayNightMode(CChannelContext *pChannelContext, int enable)
{
    if ((0 != enable) && (1 != enable))
    {
        ZW_ERROR("SYS_SetDayNightMode faile!,enable=%d\n", enable);
        return -1;
    }

#if 0
    if (IRCUT_COLOR_NIGHT == pChannelContext->DayNightMode && pChannelContext->m_nAssistantControlTest <= 0)
    {
        XTRACE_MUTEX_LOCK(&pChannelContext->m_IrcutLock);
        pChannelContext->m_IrcutState = 1;//day
        pChannelContext->m_IrcutStateHis = pChannelContext->m_IrcutState;
        XTRACE_MUTEX_UNLOCK(&pChannelContext->m_IrcutLock);
        return 0;
    }
#endif

    if ( Hi_MPP_SimulateColor2Grey(pChannelContext->pPlatformChannelhandle, enable) < 0)
    {
        return -1;
    }
    else
    {
        if (enable != pChannelContext->m_DayNightStutas)
        {
            if (IRCUT_DAY == pChannelContext->DayNightMode)
            {
                XTRACE_MUTEX_LOCK(&pChannelContext->m_DayNightLock);
                pChannelContext->m_DayNightStutas = 1;
                XTRACE_MUTEX_UNLOCK(&pChannelContext->m_DayNightLock);
            }
            else if (IRCUT_NIGHT == pChannelContext->DayNightMode)
            {
                XTRACE_MUTEX_LOCK(&pChannelContext->m_DayNightLock);
                pChannelContext->m_DayNightStutas = 0;
                XTRACE_MUTEX_UNLOCK(&pChannelContext->m_DayNightLock);
            }
        }

        if ((1 == enable) && (0 == pChannelContext->m_DayNightStutas))
        {
            XTRACE_MUTEX_LOCK(&pChannelContext->m_IrcutLock);
            ZW_INFO("===Now set day mode!===\n");
            pChannelContext->m_IrcutState = 1;//day
            pChannelContext->m_IrcutStateHis = pChannelContext->m_IrcutState;
            XTRACE_MUTEX_UNLOCK(&pChannelContext->m_IrcutLock);

            XTRACE_MUTEX_LOCK(&pChannelContext->m_DayNightLock);
            pChannelContext->m_DayNightStutas  = 1;
            XTRACE_MUTEX_UNLOCK(&pChannelContext->m_DayNightLock);
        }
        else
        {
            if((0 == enable) && (1 == pChannelContext->m_DayNightStutas))
            {
                //现在要切黑白、彩色效果，为了避免切换时，画面会红一下，移到其他daynight线程去。
            #if 0
                XTRACE_MUTEX_LOCK(&pChannelContext->m_IrcutLock);
                ZW_INFO("===Now set night mode!===\n");
                pChannelContext->m_IrcutState = 2;//night
                pChannelContext->m_IrcutStateHis = pChannelContext->m_IrcutState;
                XTRACE_MUTEX_UNLOCK(&pChannelContext->m_IrcutLock);
            #endif
                XTRACE_MUTEX_LOCK(&pChannelContext->m_DayNightLock);
                pChannelContext->m_DayNightStutas = 0;
                XTRACE_MUTEX_UNLOCK(&pChannelContext->m_DayNightLock);
            }
        }
    }

    return 0;
}

int HY_Enc_GetIRCutCheckStatus(int *pIRCutCheckStatus)
{
    CChannelContext *pChCtx = NULL;
    HY_Enc_GetEncChInfo(&pChCtx);
    if(pChCtx == NULL)
    {
        ZW_ERROR("HY_Enc_GetIRCutCheckStatus faile!\n");
        return -1;
    }
    *pIRCutCheckStatus = pChCtx->m_IrcutSwitchFlag;
    return 0;
}

int HY_Enc_SetLocalImageMode(CChannelContext *pChannelContext, int Mode)
{
    if(Hi_MPP_SetLocalImageMode(pChannelContext->pPlatformChannelhandle, Mode) != 0)
    {
        return -1;
    }
    else
    {
        pChannelContext->m_LocalImageMode = Mode;
    }

    return 0;
}

int HY_Enc_SetLocalExpMode(CChannelContext *pChannelContext, int ExpMode)
{
    if(Hi_MPP_SetLocalExpMode(pChannelContext->pPlatformChannelhandle, ExpMode) != 0)
    {
        return -1;
    }
    else
    {
        pChannelContext->m_WDRLevel = ExpMode;
    }

    return 0;
}

int HY_Enc_SetDcIris(CChannelContext *pChannelContext, BOOL enable)
{
    return 0;
}

int HY_Enc_SetLocalIrcutMode(CChannelContext *pChannelContext, int Mode)
{
    if(Hi_MPP_SetLocalIrcutMode(pChannelContext->pPlatformChannelhandle, Mode) != 0)
        return -1;

    return 0;
}

int HY_Enc_SetFlipRotateMode(CChannelContext *pChannelContext, FLIP_ROTATE_MODE_E flipRotate)
{
    if(Hi_MPP_SetFlipRotateMode(pChannelContext->pPlatformChannelhandle, flipRotate) != 0)
    {
        return -1;
    }
    else
    {
        pChannelContext->flipRotate = flipRotate;
    }

    return 0;
}

int HY_GetSecretPinStatus(int *pResetStatus)
{
    *pResetStatus = g_SecretPinStatus;
    return 0;
}

#ifdef __ONE_CLICK_CALL__
int HY_GetCallPinStatus(int *pCallStatus, int *pHangupStatus)
{
    *pCallStatus = (g_CallPinStatus & CALL_PIN_STATUS_CALL) ? 1 : 0;
    *pHangupStatus = (g_CallPinStatus & CALL_PIN_STATUS_HANGUP) ? 1 : 0;

    g_CallPinStatus = 0;
    return 0;
}

int HY_GetCallPinStatus_Test(int *pCallStatus, int *pHangupStatus)
{
    *pCallStatus = (g_TestCallPinStatus & CALL_PIN_STATUS_CALL) ? 1 : 0;
    *pHangupStatus = (g_TestCallPinStatus & CALL_PIN_STATUS_HANGUP) ? 1 : 0;

    return 0;
}

int HY_SetCallPinStatus_Test(int pCallStatus, int pHangupStatus)
{
    if (pCallStatus) {
        g_TestCallPinStatus |= CALL_PIN_STATUS_CALL;
    } else {
        g_TestCallPinStatus &= ~CALL_PIN_STATUS_CALL;
    }

    if (pHangupStatus) {
        g_TestCallPinStatus |= CALL_PIN_STATUS_HANGUP;
    } else {
        g_TestCallPinStatus &= ~CALL_PIN_STATUS_HANGUP;
    }

    return 0;
}
#endif

int HY_Enc_SetAlarmOutSupport(int nSupport)
{
    SystemInfo_t *pSysInfo = NULL;
    HY_Res_GetSystemInfo(&pSysInfo);

    if (pSysInfo->m_bSupportAlarmOut != nSupport)
    {
        pSysInfo->m_bSupportAlarmOut = !(!nSupport);
    }

    return 0;
}

int HY_Enc_SetAlarmOutDefaultStatus(int nStatus)
{
    CChannelContext *pChannelContext = NULL;
    HY_Enc_GetEncChInfo(&pChannelContext);

    if (pChannelContext->m_AlarmOutDefault != nStatus)
    {
        pChannelContext->m_AlarmOutDefault = !(!nStatus);
    }

    return 0;
}

int HY_Enc_SetAlarmOut(long nPort, int bOpen)
{
    UINT32 RetVal = 0;
    SystemInfo_t *pSysInfo = NULL;
    HY_Res_GetSystemInfo(&pSysInfo);

    if (pSysInfo->m_bSupportAlarmOut == 0)
    {
        return 0;
    }

    RetVal = Hi_MPP_SetAlarmOut(nPort, bOpen);
    if (RetVal < 0)
    {
        ZW_ERROR("Hi_MPP_SetAlarmOut Failed\n");
        return -1;
    }

    return 0;
}

int HY_Enc_SetLightAlarm(int nMode, int bOpen)
{
    CChannelContext *pChannelContext = NULL;

    HY_Enc_GetEncChInfo(&pChannelContext);

    pChannelContext->m_nLightAlarmMode = nMode;
    pChannelContext->m_bLightAlarmIsOpen = bOpen;

    return 0;
}

int HY_Enc_GetAlarmIn(int *pAlarmIndata)
{
    UINT32 RetVal = 0;
    UINT32 data = 0;

    *pAlarmIndata = 0;
    RetVal = Hi_MPP_GetAlarmIn(&data);
    if (RetVal < 0)
    {
        ZW_ERROR("Hi_MPP_GetAlarmIn Failed\n");
        return -1;
    }

    //若支持多路则需要修改逻辑
    if (data)
    {
        *pAlarmIndata = 1;
    }

     return 0;
}

/* 设置授权信息，成功返回授权错误码，失败返回-1 */
int HY_Res_SetAuthInfo(const unsigned char *pszAuthLicense)
{
    int ret = -1;
    int ActionCodeOk = 0;
    stuShmAuthInfo auth_info;
    SystemInfo_t  *pSysInfo = NULL;

    memset(&auth_info, 0, sizeof(auth_info));

    HY_Res_GetSystemInfo(&pSysInfo);

#ifdef CHECK_AUTHEN
    /* 进行授权校验 */
    ret = DeEncryptLicense(pszAuthLicense, &auth_info);

#ifdef __MODULE_4G_SUPPORT__
    int bSupport4G = 0;
    ZWSys_Get4GSupportV2(&bSupport4G);
    if (bSupport4G)
    {
        if (AUTH_SUCCESS == ret)
        {
            ZW_ERROR("4Gmodule type :%d \n", pSysInfo->m_4Gmoduletype);
#if 0
            if (0 == ZWSys_CheckDidIsSupport((char *)auth_info.struCloudInfo.szDid))
            {
                ZW_ERROR("unsupport Auth failed did = %s \n", auth_info.struCloudInfo.szDid);
                return AUTH_CUSTOM_CODE_INVALID;
            }

            //    模组类型为EG800Q, 只能授权普通did, 特殊did号段会被禁止
            if (QUECTEL_EG800 == pSysInfo->m_4Gmoduletype || ZHONGYUN_ZX800SG == pSysInfo->m_4Gmoduletype)
            {
                // 特殊号段返回授权失败
                if (1 == ZWSys_CheckDidIsFlowOpreate((char *)auth_info.struCloudInfo.szDid))
                {
                    ZW_ERROR("EG800Q moudule Auth failed did = %s \n", auth_info.struCloudInfo.szDid);
                    return AUTH_CUSTOM_CODE_INVALID;
                }
            }
#ifdef __SUPPORT_COMMON_DID_OPREATE__
            //授权普通DID，带激活运营模式的，返回授权失败
            if (0 == ZWSys_CheckDidIsFlowOpreate((char *)auth_info.struCloudInfo.szDid))
            {
                if ((0xffffffff != auth_info.dwFuncion) && (0x0 != auth_info.dwFuncion))
                {
                    if (auth_info.dwFuncion & CAPABILITY_EXTERNSIMCARD) //激活运营模式
                    {
                        ZW_ERROR("Auth failed did = %s ActionCode = %lu\n",auth_info.struCloudInfo.szDid,auth_info.dwFuncion);
                        return AUTH_CUSTOM_CODE_INVALID;
                    }
                }
            }
#endif
            //授权运营DID，不带激活运营模式的，返回授权失败
            if (1 == ZWSys_CheckDidIsFlowOpreate((char *)auth_info.struCloudInfo.szDid))
            {
                if (!(auth_info.dwFuncion & CAPABILITY_EXTERNSIMCARD)) //未激活运营模式
                {
                    ZW_ERROR("Auth failed did = %s ActionCode = %lu\n",auth_info.struCloudInfo.szDid,auth_info.dwFuncion);
                    return AUTH_CUSTOM_CODE_INVALID;
                }
            }
#endif
        //云卡设备不能授权带激活运营模式的DID
        if(pSysInfo->m_CloudCardmode)
        {
            if ((0xffffffff != auth_info.dwFuncion) && (0x0 != auth_info.dwFuncion))
            {
                if (auth_info.dwFuncion & CAPABILITY_EXTERNSIMCARD) //激活运营模式
                {
                    ZW_ERROR("Auth failed did = %s ActionCode = %lu\n",auth_info.struCloudInfo.szDid,auth_info.dwFuncion);
                    return AUTH_CUSTOM_CODE_INVALID;
                }
            }
        }

        }
    }
#endif

    /* 授权校验成功，则需要将相关信息写入配置和环境变量，变更DID，序列号和版型 */
    if (AUTH_SUCCESS == ret)
    {
        /* 设置授权码到配置和环境变量 */
        SetLicenseAuthorize(&auth_info);

        /* 变更授权码 */
        memset(pSysInfo->m_szAuthLicense, 0, sizeof(pSysInfo->m_szAuthLicense));
        strncpy((char *)pSysInfo->m_szAuthLicense, (char *)auth_info.szAuthCode, sizeof(pSysInfo->m_szAuthLicense));

        /* 变更DID号码 */
        FILE *fp = NULL;
        fp = fopen(AUTH_DID_NUM_FLAG, "w");
        if (NULL == fp)
        {
            SDKPRINT("Fail to fopen %s\n", AUTH_DID_NUM_FLAG);
        }
        else
        {
            fprintf(fp, "%s", auth_info.struCloudInfo.szDid);
            fflush(fp);
            fclose(fp);
            fp = NULL;
        }

        /* 变更序列号 */
        memset(pSysInfo->m_szSerialNum, 0, sizeof(pSysInfo->m_szSerialNum));
        strncpy((char *)pSysInfo->m_szSerialNum, (char *)auth_info.szSerialNum, sizeof(pSysInfo->m_szSerialNum));

        /* 变更版型 */
        pSysInfo->m_BoardInfo = auth_info.dwBoardID;

        /* 变更MAC地址(下次启动起效) */
        strncpy(pSysInfo->m_szMacAddr, (char *)auth_info.szMac, MAC_ADDR_LEN);

        /* 变更授权状态 */
        pSysInfo->m_bAuthEncryptOK = TRUE;

        /* 变更AF激活码 */
        if ((0xffffffff != auth_info.dwFuncion) && (0x0 != auth_info.dwFuncion))
        {
            if (0 == (auth_info.dwFuncion & CAPABILITY_AF))    //不支持
            {
                ActionCodeOk &= ~(1 << AF_SUPPORT);
                SDKPRINT("Don't Support AF!\n");
            }
            else
            {
                /* 当加密授权解密成功，则表明获取到了激活码 */
                ActionCodeOk |= (1 << AF_SUPPORT);
                SDKPRINT("AtionCode is right!\n");
            }
#ifdef __MODULE_4G_SUPPORT__
            if (0 == (auth_info.dwFuncion & CAPABILITY_EXTERNSIMCARD)) //不支持
            {
                 ActionCodeOk &= ~(CAPABILITY_EXTERNSIMCARD);
                 SDKPRINT("Don't Support EXTERNSIMCARD!\n");

                 if (1 == ZWSys_CheckDidIsFlowOpreate((char *)auth_info.struCloudInfo.szDid))
                 {
                     //IOTDBB 4g运营号段,为激活运营模式，视频显示未激活
                     //MP4Sys_SetNoActiveOperModeOSDDisplay();
                 }
            }
            else
            {
                 /* 当加密授权解密成功，则表明获取到了激活码 */
                 ActionCodeOk |= CAPABILITY_EXTERNSIMCARD;
                 SDKPRINT("AtionCode is right!\n");
            }
#endif

            pSysInfo->m_nFuncActiveStatusCode = ActionCodeOk;
        }

        ZW_INFO("-----------------------------------------------------------------------\n");
        ZW_INFO("auth_info.dwAuthVerifyErrno = %ld\n", auth_info.dwAuthVerifyErrno);
        ZW_INFO("auth_info.dCustomID = %#x\n", auth_info.dCustomID);
        ZW_INFO("auth_info.dwBoardID = %#lx\n", auth_info.dwBoardID);
        ZW_INFO("auth_info.dwAtime = %ld\n", auth_info.dwAtime);
        ZW_INFO("auth_info.dwFuncion = %ld\n", auth_info.dwFuncion);
        ZW_INFO("auth_info.szAuthCode = %s\n", auth_info.szAuthCode);

        int i = 0;
        ZW_INFO("auth_info.szMac = ");
        for (i=0; i<6; i++)
        {
            ZW_INFO("%#x ", auth_info.szMac[i]);
        }
        ZW_INFO("auth_info.szSerialNum = %s\n", auth_info.szSerialNum);
        ZW_INFO("auth_info.struCloudInfo.szType = %d\n", auth_info.struCloudInfo.szType);
        ZW_INFO("auth_info.struCloudInfo.szLen = %d\n", auth_info.struCloudInfo.szLen);
        ZW_INFO("auth_info.struCloudInfo.szDid = %s\n", auth_info.struCloudInfo.szDid);
        ZW_INFO("-----------------------------------------------------------------------\n");
    }
    else
    {
        pSysInfo->m_bAuthEncryptOK = FALSE;
    }
#endif
    return ret;
}

/* 获取授权码和授权状态，成功返回0，失败返回-1 */
int HY_Res_GetAuthInfo(unsigned char *pszAuthLicense, unsigned int *pAuthState)
{
    SystemInfo_t *pSysInfo = NULL;

    HY_Res_GetSystemInfo(&pSysInfo);

    strncpy((char *)pszAuthLicense, pSysInfo->m_szAuthLicense, LICENSE_CODE_MAX_LEN);

    if (TRUE == pSysInfo->m_bAuthEncryptOK)
    {
        *pAuthState = 1;
    }
    else
    {
        *pAuthState = 0;
    }

    return 0;
}

int HY_Enc_SetHumanTrackEn(CChannelContext *pChannelContext, int bEnable)
{
    XTRACE_MUTEX_LOCK(&pChannelContext->m_IVPLock);
    int nRet = Hi_MPP_SetHumanTrackEn(pChannelContext->pPlatformChannelhandle, bEnable);
    XTRACE_MUTEX_UNLOCK(&pChannelContext->m_IVPLock);
    return nRet;
}

int HY_Enc_SetHumanDetectEnable(CChannelContext *pChannelContext, int bEnable)
{
    XTRACE_MUTEX_LOCK(&pChannelContext->m_IVPLock);
    int nRet = Hi_MPP_SetHumanDetectEnable(pChannelContext->pPlatformChannelhandle, bEnable);
    XTRACE_MUTEX_UNLOCK(&pChannelContext->m_IVPLock);
    return nRet;
}

int HY_Enc_SetHumanShapeCfgV2(CChannelContext *pChannelContext)
{
    XTRACE_MUTEX_LOCK(&pChannelContext->m_IVPLock);
    int nRet = Hi_MPP_SetIVPCfgV2(pChannelContext->pPlatformChannelhandle, &pChannelContext->m_stHumanShapeCfgV2);
    XTRACE_MUTEX_UNLOCK(&pChannelContext->m_IVPLock);

    return nRet;
}

char speechLanguageDir[eSpeechLanguage_Num][16]=
{
    "didi",        //滴滴
    "chinese",     //中
    "english",     //英
    "vietnamese",  //越南语
    "indonesian",  //印尼语
    "spanish",     //西班牙语
    "portuguese",  //葡萄牙语
    "korean",      //韩语
    "thai",        //泰语
    "japanese",    //日语
    "german",      //德语
    "italian",     //意大利语
    "turkish",     //土耳其语
    "polish",      //波兰语
    "french",      //法语
    "czech",       //捷克语
    "greek",       //希腊语
    "persian",     //波斯语
    "hebrew",      //希伯来
    "arabic",      //阿拉伯
    "russian",     //俄语
    "other",       //自定义
#ifdef __SUPPORT_PRODUCT_TEST_CUSTOM_SPEECH_FILES__
    ""             //忽略
#endif
};

#ifdef __MODULE_4G_SUPPORT__
static int GetErrcodeValueSpeechContent(const int speechtype, int *speechcontentbuf, int speechcontentcnt)
{
    //平台错误码判断
    if (speechtype < 1 || speechtype > (SPEECH_4G_CARDTYPE_ERRORCODE6-SPEECH_4G_CARDTYPE_ERROR))
    {
        ZW_ERROR("invalid speechtype :%d \n", speechtype);
        return -1;
    }

    if (!speechcontentbuf)
    {
        ZW_ERROR("NULL pointer \n");
        return -1;
    }

    ZW_ERROR("speechtype :%d  \n", speechtype);

    speechcontentbuf[0] = SPEECH_4G_CARDTYPE_ERROR;
    speechcontentbuf[1] = SPEECH_4G_SIGNAL_0+speechtype;

    for (int num = 0; num < speechcontentcnt; num++)
    {
        ZW_ERROR("speechcontentbuf[%d] :%d \n", num, speechcontentbuf[num]);
    }
    return 0;
}


// 获取信号质量对应的播报内容
static int GetSigalValueSpeechContent(const int signalvalue, int *speechcontentbuf, int speechcontentcnt)
{
    if (signalvalue < 0 || signalvalue > 99)
    {
        ZW_ERROR("invalid signal :%d \n", signalvalue);
        return -1;
    }

    if (!speechcontentbuf)
    {
        ZW_ERROR("NULL pointer \n");
        return -1;
    }

    //获取十位数
    int shang = signalvalue / 10;
    //获取个位数
    int yushu = signalvalue % 10;

    ZW_ERROR("signalvalue :%d shang :%d yushu :%d  \n", signalvalue, shang, yushu);

    speechcontentbuf[0] = SPEECH_4G_SIGNAL_QUALITY;

    // 10~19播报比较特殊，最多播报两个数字
    if (10 == signalvalue)
    {
        speechcontentbuf[1] =SPEECH_4G_SIGNAL_10;
    }
    else if(10 < signalvalue && signalvalue <20)
    {
        speechcontentbuf[1] =SPEECH_4G_SIGNAL_10;
        speechcontentbuf[2] =(SPEECH_4G_SIGNAL_0+yushu);
    }
    else if (shang < 1)
    {
        // 小于10
        speechcontentbuf[1] =(SPEECH_4G_SIGNAL_0+yushu);
    }
    else if (0 < yushu)
    {
        // 不被10整除，如39
        speechcontentbuf[1] = (SPEECH_4G_SIGNAL_0+shang);
        speechcontentbuf[2] = (SPEECH_4G_SIGNAL_10);
        speechcontentbuf[3] = (SPEECH_4G_SIGNAL_0+yushu);
    }
    else if (0 == yushu)
    {
        // 被10整除，如40
        speechcontentbuf[1] =(SPEECH_4G_SIGNAL_0+shang);
        speechcontentbuf[2] =(SPEECH_4G_SIGNAL_10);
    }

    for (int num = 0; num < speechcontentcnt; num++)
    {
        ZW_DEBUG("speechcontentbuf[%d] :%d \n", num, speechcontentbuf[num]);
    }
    return 0;
}
#endif

int HY_Enc_SetAlarmLightStayTime(int nEnable, int nStayTime)
{
    LIGHT_CTX_S* lightCtx = NULL;
    CChannelContext *pChContext = NULL;

#ifdef NO_SET_AUDIO
    return 0;
#endif

    HY_Enc_GetEncChInfo(&pChContext);
    lightCtx = &pChContext->m_stLightCtx;

    if (nStayTime <= 0 || nStayTime > 60)
    {
        return -1;
    }

    lightCtx->lightAlarmEn = nEnable;
    lightCtx->lightAlarmStayTime = nStayTime;
    return 0;
}

int HY_Enc_SetAlarmLightStart()
{
    CChannelContext *pChCtx = NULL;
    LIGHT_CTX_S* lightCtx = NULL;

#ifdef NO_SET_AUDIO
    return 0;
#endif

    HY_Enc_GetEncChInfo(&pChCtx);

    /* 声光报警能力、打断声光报警              */
    if (0 == pChCtx->m_whiteLightFlickeredEnable || 1 == pChCtx->m_recvPauseAudioLightAlarm)
    {
        return 0;
    }

    /* 夜晚或者白光灯开启（全彩或者智能模式触发报警）时不触发灯光报警 */
    if (pChCtx->m_DayNightStutas == ISNIGHT)
    {
        return 0;
    }

    lightCtx = &pChCtx->m_stLightCtx;

    /* 声光报警使能关闭 */
    if (0 == lightCtx->lightAlarmEn)
    {
        return 0;
    }

    /* 白天且灯板没有触发白光时才能使用这个功能 */
    if (LIGHTCTL_IRCUT_NIGHT == lightCtx->eDayNightStatus || TRUE == lightCtx->isWhiteLightOpen)
    {
        return 0;
    }

    //启用移动侦测时，过滤掉切换画面模式导致的移动侦测误触，防止灯光报警影响 双光源和引起下一次移动侦测误触
#if 0
    if (1 == pChCtx->m_bStartMD)
    {
        return 0;
    }
#endif

    lightCtx->lightAlarmStReal = TRUE;
    return 0;
}

/*
1.如果当前正在播放报警声，又触发了报警，则不再新添报警播报事件，但需重置播报延时
2.如果报警事件列表里已有报警事件，又触发了报警，则不再新添报警事件
*/
int HY_Enc_SetSpeechContent(SpeechType_T speech)
{
    CChannelContext        *pChannelContext = NULL;
    int writePoint = 0;
#ifdef __SUPPORT_PRODUCT_TEST_CUSTOM_SPEECH_FILES__
    int nSpeechLanguage = eSpeechLanguage_None;
#endif

    HY_Enc_GetEncChInfo(&pChannelContext);

    XTRACE_MUTEX_LOCK(&pChannelContext->m_SpeechLock);

    //如果满了
    if (MAX_SPEECH_CACHE_NUM == pChannelContext->m_SpeechNum)
    {
        ZW_ERROR("Speech num == %d\n", MAX_SPEECH_CACHE_NUM);
        XTRACE_MUTEX_UNLOCK(&pChannelContext->m_SpeechLock);
        return -1;
    }

#ifdef __MODULE_4G_SUPPORT__

    if (SPEECH_4G_SIGNAL_QUALITY <= speech && SPEECH_4G_SIGNAL_MAXNUM >= speech && pChannelContext->m_enSpeechLanguage != eSpeechLanguage_Chinese)
    {
        ZW_ERROR("not chinese, return !\n");
        pthread_mutex_unlock(&pChannelContext->m_SpeechLock);
        return -1;
    }

    if (SPEECH_4G_CARDTYPE_ERROR <= speech && SPEECH_4G_CARDTYPE_ERRORCODE6 >= speech && pChannelContext->m_enSpeechLanguage != eSpeechLanguage_Chinese)
    {
        ZW_ERROR("not chinese, return !\n");
        pthread_mutex_unlock(&pChannelContext->m_SpeechLock);
        return -1;
    }
#endif

    // 如果当前处理升级准备阶段,过滤掉升级以外的语音播报
    if (0 == access("/tmp/.update_create", F_OK))
    {
        SpeechType_T white_lists[] = {SPEECH_UPGRADE_START, SPEECH_UPGRADE_END_REBOOT, SPEECH_UPGRADE_FAIL, SPEECH_UPGRADE_FAIL_ERROR_VERSION};
        int idx = 0;
        char is_speech_in_whitelist  = 0;
        int list_size = sizeof(white_lists)/sizeof(white_lists[0]);
        for (idx = 0; idx < list_size; idx++)
        {
            if (speech == white_lists[idx])
            {
                is_speech_in_whitelist = 1;
                break;
            }
        }
        if (!is_speech_in_whitelist)
        {
            pthread_mutex_unlock(&pChannelContext->m_SpeechLock);
            return 0;
        }
    }

    // 在22（晚上10点）到次日6点之间，不能播报升级语音。
    if (SPEECH_UPGRADE_START == speech || SPEECH_NET_CONNECT_SUCCESS == speech)
    {
        time_t now = time(NULL);
        struct tm tm_now;
        localtime_r(&now, &tm_now);

        if (tm_now.tm_hour >= 22 || tm_now.tm_hour < 6)
        {
            ZW_INFO("Voice broadcasts are not allowed from 10:00 PM to 6:00 AM the next morning.\n");
            pthread_mutex_unlock(&pChannelContext->m_SpeechLock);
            return 0;
        }
    }

    //复位播报语音优先级最高，可以打断其他播报
    if (SPEECH_SYS_RESET == speech)
    {
        pChannelContext->m_BoardcastSpeechFirst = 1;
        pChannelContext->m_SpeechWrite = 0;
        ZW_DEBUG("Reset voice broadcast priority is highest\n");
    }

    if (((SPEECH_SYS_STARTING == speech)
        || (SPEECH_SYS_START_OVER == speech)
        || (SPEECH_SYS_QRCODE_SCAN_OVER == speech)
        || (SPEECH_SYS_RESET_SUCCESS == speech)
        || (SPEECH_WIFI_IN_AP_STATUS == speech)
        || (SPEECH_WIFI_CONNECTING == speech)
        || (SPEECH_WIFI_CONNECT_SUCCESS == speech)
        || (SPEECH_WIFI_CONNECT_FAIL == speech)
        || (SPEECH_WIFI_CONNECT_FAIL_PWD_WRONG == speech)
        || (SPEECH_WIFI_AP_BUILD == speech)
        || (SPEECH_WIFI_CONNECT_FAIL_PLEASE_AGAIN == speech)
        || (SPEECH_WIFI_PLEASE_CONNECT == speech)
        || (SPEECH_NET_CONNECT_SUCCESS == speech)
        || (SPEECH_AGING_TEST == speech))
        && ((SPEECH_ALARM_HUMAN == pChannelContext->m_SpeechNow)
        || (SPEECH_ALARM_MOTION == pChannelContext->m_SpeechNow)))
    {
        pChannelContext->m_BoardcastSpeechFirst = 1;
        pChannelContext->m_SpeechWrite = 0;
    }

    if (SPEECH_ALARM_HUMAN == speech) //人形优先于移动
    {
        if ((SPEECH_ALARM_HUMAN == pChannelContext->m_SpeechNow))
        {
            pChannelContext->m_SpeechAlarmDelayClear = 1;
            ZW_DEBUG("Clear delay time!\n");
            XTRACE_MUTEX_UNLOCK(&pChannelContext->m_SpeechLock);
            return 0;
        }
        else
        {
            if (pChannelContext->m_SpeechNum > 0)
            {
                if (SPEECH_ALARM_HUMAN == pChannelContext->m_SpeechType[pChannelContext->m_SpeechWrite - 1])
                {
                    ZW_DEBUG("SPEECH_ALARM_HUMAN drop!\n");
                    XTRACE_MUTEX_UNLOCK(&pChannelContext->m_SpeechLock);
                    return 0;
                }
            }
        }
    }
    else if (SPEECH_ALARM_MOTION == speech)
    {
        if (SPEECH_ALARM_HUMAN == pChannelContext->m_SpeechNow)
        {
            ZW_DEBUG("human alarm ing!\n");
            XTRACE_MUTEX_UNLOCK(&pChannelContext->m_SpeechLock);
            return 0;
        }
        else if (SPEECH_ALARM_MOTION == pChannelContext->m_SpeechNow)
        {
            pChannelContext->m_SpeechAlarmDelayClear = 1;
            ZW_DEBUG("Clear delay time!\n");
            XTRACE_MUTEX_UNLOCK(&pChannelContext->m_SpeechLock);
            return 0;
        }
        else
        {
            if (pChannelContext->m_SpeechNum > 0)
            {
                if (SPEECH_ALARM_MOTION == pChannelContext->m_SpeechType[pChannelContext->m_SpeechWrite - 1])
                {
                    ZW_DEBUG("SPEECH_ALARM_MOTION drop!\n");
                    XTRACE_MUTEX_UNLOCK(&pChannelContext->m_SpeechLock);
                    return 0;
                }
            }
        }
    }

    //当前是报警语音
    if (SPEECH_ALARM_MOTION == speech
        || SPEECH_ALARM_HUMAN == speech
        || (SPEECH_WIFI_IN_AP_STATUS == speech)
        || (SPEECH_WIFI_CONNECTING == speech)
        || (SPEECH_WIFI_CONNECT_SUCCESS == speech)
        || (SPEECH_WIFI_CONNECT_FAIL == speech)
        || (SPEECH_WIFI_CONNECT_FAIL_PWD_WRONG == speech)
        || (SPEECH_WIFI_AP_BUILD == speech)
        || (SPEECH_WIFI_CONNECT_FAIL_PLEASE_AGAIN == speech)
        || (SPEECH_WIFI_PLEASE_CONNECT == speech)
        || (SPEECH_NET_CONNECT_SUCCESS == speech))
    {
        /*当前设置进去的播报和当前播报的语音类型相同，则直接重新播报，不加入队列中
         *防止报警播报时间长而触发的频率高了后面停止触发了还是播报很久 */
        if(speech)
        {
            if (speech == pChannelContext->m_SpeechNow)
            {
                pChannelContext->m_SpeechAlarmDelayClear = 1;
                ZW_DEBUG("Clear delay time!\n");
                XTRACE_MUTEX_UNLOCK(&pChannelContext->m_SpeechLock);
                return 0;
            }
            else if (speech != pChannelContext->m_SpeechNow && 0 != pChannelContext->m_SpeechNow)
            {
                // 当前播报完后立马播报下一个类型
                pChannelContext->m_SpeechAlarmDelayClear = 2;
                ZW_DEBUG("End Current Speech!\n");
            }
        }
    }

    if (MAX_SPEECH_CACHE_NUM == pChannelContext->m_SpeechWrite)
    {
        pChannelContext->m_SpeechWrite = 0;
    }

#ifdef __MODULE_4G_SUPPORT__
    if (SPEECH_4G_SIGNAL_0 <= speech && SPEECH_4G_SIGNAL_MAXNUM >= speech)
    {
        int speechcontentbuf[4] = {0};
        // 获取播报内容
        int ret = GetSigalValueSpeechContent((speech-SPEECH_4G_SIGNAL_0), speechcontentbuf, sizeof(speechcontentbuf) / sizeof(speechcontentbuf[0]));
        // 原子操作--逐个写入队列,不允许中间插入其他语音
        if (0 == ret)
        {
            for (int num = 0; num < sizeof(speechcontentbuf) / sizeof(speechcontentbuf[0]); num++)
            {
                if (speechcontentbuf[num])
                {
                    ZW_ERROR("signal content :%d\n", speechcontentbuf[num]);
                    if (MAX_SPEECH_CACHE_NUM == pChannelContext->m_SpeechWrite)
                    {
                        pChannelContext->m_SpeechWrite = 0;
                    }
                    writePoint = pChannelContext->m_SpeechWrite;
                    pChannelContext->m_SpeechType[writePoint] = speechcontentbuf[num];
                    ZW_ERROR("pChannelContext->m_SpeechType[%d] :%d \n", writePoint, pChannelContext->m_SpeechType[writePoint]);
                    switch (speechcontentbuf[num])
                    {
                        case SPEECH_4G_SIGNAL_QUALITY:
                             snprintf(pChannelContext->m_SpeechFileName[writePoint],
                                 MAX_SPEECH_FILE_SIZE, "%s%s/4gsignal%s", pChannelContext->m_SpeechFilePath,
                                 speechLanguageDir[nSpeechLanguage], SPEECH_PREFIX);
                             break;
                        case SPEECH_4G_SIGNAL_0:
                             snprintf(pChannelContext->m_SpeechFileName[writePoint],
                                 MAX_SPEECH_FILE_SIZE, "%s%s/signal0%s", pChannelContext->m_SpeechFilePath,
                                 speechLanguageDir[nSpeechLanguage], SPEECH_PREFIX);
                             break;
                        case SPEECH_4G_SIGNAL_1:
                             snprintf(pChannelContext->m_SpeechFileName[writePoint],
                                 MAX_SPEECH_FILE_SIZE, "%s%s/signal1%s", pChannelContext->m_SpeechFilePath,
                                 speechLanguageDir[nSpeechLanguage], SPEECH_PREFIX);
                             break;
                        case SPEECH_4G_SIGNAL_2:
                             snprintf(pChannelContext->m_SpeechFileName[writePoint],
                                 MAX_SPEECH_FILE_SIZE, "%s%s/signal2%s", pChannelContext->m_SpeechFilePath,
                                 speechLanguageDir[nSpeechLanguage], SPEECH_PREFIX);
                             break;
                        case SPEECH_4G_SIGNAL_3:
                             snprintf(pChannelContext->m_SpeechFileName[writePoint],
                                 MAX_SPEECH_FILE_SIZE, "%s%s/signal3%s", pChannelContext->m_SpeechFilePath,
                                 speechLanguageDir[nSpeechLanguage], SPEECH_PREFIX);
                             break;
                        case SPEECH_4G_SIGNAL_4:
                             snprintf(pChannelContext->m_SpeechFileName[writePoint],
                                 MAX_SPEECH_FILE_SIZE, "%s%s/signal4%s", pChannelContext->m_SpeechFilePath,
                                 speechLanguageDir[nSpeechLanguage], SPEECH_PREFIX);
                             break;
                        case SPEECH_4G_SIGNAL_5:
                             snprintf(pChannelContext->m_SpeechFileName[writePoint],
                                 MAX_SPEECH_FILE_SIZE, "%s%s/signal5%s", pChannelContext->m_SpeechFilePath,
                                 speechLanguageDir[nSpeechLanguage], SPEECH_PREFIX);
                             break;
                        case SPEECH_4G_SIGNAL_6:
                             snprintf(pChannelContext->m_SpeechFileName[writePoint],
                                 MAX_SPEECH_FILE_SIZE, "%s%s/signal6%s", pChannelContext->m_SpeechFilePath,
                                 speechLanguageDir[nSpeechLanguage], SPEECH_PREFIX);
                             break;
                        case SPEECH_4G_SIGNAL_7:
                             snprintf(pChannelContext->m_SpeechFileName[writePoint],
                                 MAX_SPEECH_FILE_SIZE, "%s%s/signal7%s", pChannelContext->m_SpeechFilePath,
                                 speechLanguageDir[nSpeechLanguage], SPEECH_PREFIX);
                             break;
                        case SPEECH_4G_SIGNAL_8:
                             snprintf(pChannelContext->m_SpeechFileName[writePoint],
                                 MAX_SPEECH_FILE_SIZE, "%s%s/signal8%s", pChannelContext->m_SpeechFilePath,
                                 speechLanguageDir[nSpeechLanguage], SPEECH_PREFIX);
                             break;
                        case SPEECH_4G_SIGNAL_9:
                             snprintf(pChannelContext->m_SpeechFileName[writePoint],
                                 MAX_SPEECH_FILE_SIZE, "%s%s/signal9%s", pChannelContext->m_SpeechFilePath,
                                 speechLanguageDir[nSpeechLanguage], SPEECH_PREFIX);
                             break;
                        case SPEECH_4G_SIGNAL_10:
                             snprintf(pChannelContext->m_SpeechFileName[writePoint],
                                 MAX_SPEECH_FILE_SIZE, "%s%s/signal10%s", pChannelContext->m_SpeechFilePath,
                                 speechLanguageDir[nSpeechLanguage], SPEECH_PREFIX);
                             break;
                        default:
                            break;
                    }
                    pChannelContext->m_SpeechNum++;
                    pChannelContext->m_SpeechWrite++;
                }
            }
        }
    }
    // 中文才播放 4G卡类型对应的错误码
    else if (SPEECH_4G_CARDTYPE_ERROR <= speech && SPEECH_4G_CARDTYPE_ERRORCODE6 >= speech)
    {
        int speechcontentbuf[2] = {0};
        // 获取播报内容
        int ret = GetErrcodeValueSpeechContent((speech-SPEECH_4G_CARDTYPE_ERROR), speechcontentbuf, sizeof(speechcontentbuf) / sizeof(speechcontentbuf[0]));
        // 原子操作--逐个写入队列,不允许中间插入其他语音
        if (0 == ret)
        {
            for (int num = 0; num < sizeof(speechcontentbuf) / sizeof(speechcontentbuf[0]); num++)
            {
                if (speechcontentbuf[num])
                {
                    ZW_ERROR("errcode content :%d\n", speechcontentbuf[num]);
                    if (MAX_SPEECH_CACHE_NUM == pChannelContext->m_SpeechWrite)
                    {
                        pChannelContext->m_SpeechWrite = 0;
                    }
                    writePoint = pChannelContext->m_SpeechWrite;
                    pChannelContext->m_SpeechType[writePoint] = speechcontentbuf[num];
                    ZW_ERROR("pChannelContext->m_SpeechType[%d] :%d \n", writePoint, pChannelContext->m_SpeechType[writePoint]);
                    switch (speechcontentbuf[num])
                    {
                        case SPEECH_4G_CARDTYPE_ERROR:
                             snprintf(pChannelContext->m_SpeechFileName[writePoint],
                                 MAX_SPEECH_FILE_SIZE, "%s%s/4gcardtypeerrcode%s", pChannelContext->m_SpeechFilePath,
                                 speechLanguageDir[nSpeechLanguage], SPEECH_PREFIX);
                             break;
                        case SPEECH_4G_SIGNAL_1:
                             snprintf(pChannelContext->m_SpeechFileName[writePoint],
                                 MAX_SPEECH_FILE_SIZE, "%s%s/signal1%s", pChannelContext->m_SpeechFilePath,
                                 speechLanguageDir[nSpeechLanguage], SPEECH_PREFIX);
                             break;
                        case SPEECH_4G_SIGNAL_2:
                             snprintf(pChannelContext->m_SpeechFileName[writePoint],
                                 MAX_SPEECH_FILE_SIZE, "%s%s/signal2%s", pChannelContext->m_SpeechFilePath,
                                 speechLanguageDir[nSpeechLanguage], SPEECH_PREFIX);
                             break;
                        case SPEECH_4G_SIGNAL_3:
                             snprintf(pChannelContext->m_SpeechFileName[writePoint],
                                 MAX_SPEECH_FILE_SIZE, "%s%s/signal3%s", pChannelContext->m_SpeechFilePath,
                                 speechLanguageDir[nSpeechLanguage], SPEECH_PREFIX);
                             break;
                        case SPEECH_4G_SIGNAL_4:
                             snprintf(pChannelContext->m_SpeechFileName[writePoint],
                                 MAX_SPEECH_FILE_SIZE, "%s%s/signal4%s", pChannelContext->m_SpeechFilePath,
                                 speechLanguageDir[nSpeechLanguage], SPEECH_PREFIX);
                             break;
                        case SPEECH_4G_SIGNAL_5:
                             snprintf(pChannelContext->m_SpeechFileName[writePoint],
                                 MAX_SPEECH_FILE_SIZE, "%s%s/signal5%s", pChannelContext->m_SpeechFilePath,
                                 speechLanguageDir[nSpeechLanguage], SPEECH_PREFIX);
                             break;
                        case SPEECH_4G_SIGNAL_6:
                             snprintf(pChannelContext->m_SpeechFileName[writePoint],
                                 MAX_SPEECH_FILE_SIZE, "%s%s/signal6%s", pChannelContext->m_SpeechFilePath,
                                 speechLanguageDir[nSpeechLanguage], SPEECH_PREFIX);
                             break;
                        default:
                            break;
                    }
                    pChannelContext->m_SpeechNum++;
                    pChannelContext->m_SpeechWrite++;
                }
            }
        }
    }
    else
#endif
    {
        writePoint = pChannelContext->m_SpeechWrite;
        pChannelContext->m_SpeechNum++;
        ZW_INFO("m_SpeechNum=%d, m_SpeechWrite=%d, m_SpeechRead=%d\n",
            pChannelContext->m_SpeechNum, pChannelContext->m_SpeechWrite,
            pChannelContext->m_SpeechRead);
        pChannelContext->m_SpeechWrite++;
        pChannelContext->m_SpeechType[writePoint] = speech;
    }

    switch (speech)
    {
        /*WIFI相关*/
        case SPEECH_WIFI_IN_AP_STATUS:
            snprintf(pChannelContext->m_SpeechFileName[writePoint], MAX_SPEECH_FILE_SIZE, "%s%s/wifiApMode%s", pChannelContext->m_SpeechFilePath, speechLanguageDir[nSpeechLanguage], SPEECH_PREFIX);
            break;
        case SPEECH_WIFI_CONNECTING:
            snprintf(pChannelContext->m_SpeechFileName[writePoint], MAX_SPEECH_FILE_SIZE, "%s%s/wifiConning%s", pChannelContext->m_SpeechFilePath, speechLanguageDir[nSpeechLanguage], SPEECH_PREFIX);
            break;
        case SPEECH_WIFI_CONNECT_SUCCESS:
            snprintf(pChannelContext->m_SpeechFileName[writePoint], MAX_SPEECH_FILE_SIZE, "%s%s/wifiConnSucc%s", pChannelContext->m_SpeechFilePath, speechLanguageDir[nSpeechLanguage], SPEECH_PREFIX);
            break;
        case SPEECH_WIFI_CONNECT_FAIL:
            snprintf(pChannelContext->m_SpeechFileName[writePoint], MAX_SPEECH_FILE_SIZE, "%s%s/wifiConnFail%s", pChannelContext->m_SpeechFilePath, speechLanguageDir[nSpeechLanguage], SPEECH_PREFIX);
            break;
        case SPEECH_WIFI_CONNECT_FAIL_PWD_WRONG:
            snprintf(pChannelContext->m_SpeechFileName[writePoint], MAX_SPEECH_FILE_SIZE, "%s%s/wifiConnFailPWDErr%s", pChannelContext->m_SpeechFilePath, speechLanguageDir[nSpeechLanguage], SPEECH_PREFIX);
            break;
        case SPEECH_WIFI_AP_BUILD:
            snprintf(pChannelContext->m_SpeechFileName[writePoint], MAX_SPEECH_FILE_SIZE, "%s%s/wifiAPBuild%s", pChannelContext->m_SpeechFilePath, speechLanguageDir[nSpeechLanguage], SPEECH_PREFIX);
            break;
        case SPEECH_WIFI_CONNECT_FAIL_PLEASE_AGAIN:
            snprintf(pChannelContext->m_SpeechFileName[writePoint], MAX_SPEECH_FILE_SIZE, "%s%s/wifiConnFailPleaseAgain%s", pChannelContext->m_SpeechFilePath, speechLanguageDir[nSpeechLanguage], SPEECH_PREFIX);
            break;
        case SPEECH_WIFI_PLEASE_CONNECT:
            snprintf(pChannelContext->m_SpeechFileName[writePoint], MAX_SPEECH_FILE_SIZE, "%s%s/PleaseConnWifi%s", pChannelContext->m_SpeechFilePath, speechLanguageDir[nSpeechLanguage], SPEECH_PREFIX);
            break;
        case SPEECH_NET_CONNECT_SUCCESS:
            snprintf(pChannelContext->m_SpeechFileName[writePoint], MAX_SPEECH_FILE_SIZE, "%s%s/netConnSucc%s", pChannelContext->m_SpeechFilePath, speechLanguageDir[nSpeechLanguage], SPEECH_PREFIX);
            break;

        /*报警相关*/
        case SPEECH_ALARM_HUMAN:
            if (1 == pChannelContext->m_AlarmSpeechCfg.alarmSpeechType)
            {
                snprintf(pChannelContext->m_SpeechFileName[writePoint], MAX_SPEECH_FILE_SIZE, "%s/motionAlarm%s", SPEECH_FILE_OTHER_PATH, SPEECH_PREFIX);
            }
            else if (2 == pChannelContext->m_AlarmSpeechCfg.alarmSpeechType)
            {
                snprintf(pChannelContext->m_SpeechFileName[writePoint], MAX_SPEECH_FILE_SIZE, "%s/motionDogBarking%s", SPEECH_FILE_OTHER_PATH, SPEECH_PREFIX);
            }
            else
            {
                if (!access(SPEECH_MOTION_ALARM_CUSTOM_FILE, F_OK))
                {
                    snprintf(pChannelContext->m_SpeechFileName[writePoint], MAX_SPEECH_FILE_SIZE, "%s", SPEECH_MOTION_ALARM_CUSTOM_FILE);
                }
                else
                {
                    snprintf(pChannelContext->m_SpeechFileName[writePoint], MAX_SPEECH_FILE_SIZE, "%s/motionDogBarking%s", SPEECH_FILE_OTHER_PATH, SPEECH_PREFIX);
                }
            }
            break;
        case SPEECH_ALARM_MOTION:
            if (1 == pChannelContext->m_AlarmSpeechCfg.alarmSpeechType)
            {
                snprintf(pChannelContext->m_SpeechFileName[writePoint], MAX_SPEECH_FILE_SIZE, "%s/motionAlarm%s", SPEECH_FILE_OTHER_PATH, SPEECH_PREFIX);
            }
            else if (2 == pChannelContext->m_AlarmSpeechCfg.alarmSpeechType)
            {
                snprintf(pChannelContext->m_SpeechFileName[writePoint], MAX_SPEECH_FILE_SIZE, "%s/motionDogBarking%s", SPEECH_FILE_OTHER_PATH, SPEECH_PREFIX);
            }
            else
            {
                if (!access(SPEECH_MOTION_ALARM_CUSTOM_FILE, F_OK))
                {
                    snprintf(pChannelContext->m_SpeechFileName[writePoint], MAX_SPEECH_FILE_SIZE, "%s", SPEECH_MOTION_ALARM_CUSTOM_FILE);
                }
                else
                {
                    snprintf(pChannelContext->m_SpeechFileName[writePoint], MAX_SPEECH_FILE_SIZE, "%s/motionAlarm%s", SPEECH_FILE_OTHER_PATH, SPEECH_PREFIX);
                }
            }
            break;

        /*定时提醒相关*/
        case SPEECH_REMINDER_SCHEDULE_1:
        case SPEECH_REMINDER_SCHEDULE_2:
        case SPEECH_REMINDER_SCHEDULE_3:
        case SPEECH_REMINDER_SCHEDULE_TEST:
            {
                eSpeechRemindType curRemindType = eSpeechType_Remind_1;
                SpeechRemindTypeMap(speech, &curRemindType);

                if (1 == pChannelContext->m_RemindSpeechCfg[curRemindType].remindSpeechType)
                {
                    snprintf(pChannelContext->m_SpeechFileName[writePoint], MAX_SPEECH_FILE_SIZE, "%s/remind_canon%s", SPEECH_FILE_OTHER_PATH, SPEECH_PREFIX);
                }
                else if (2 == pChannelContext->m_RemindSpeechCfg[curRemindType].remindSpeechType)
                {
                    snprintf(pChannelContext->m_SpeechFileName[writePoint], MAX_SPEECH_FILE_SIZE, "%s/remind_elisie%s", SPEECH_FILE_OTHER_PATH, SPEECH_PREFIX);
                }
                else if (3 == pChannelContext->m_RemindSpeechCfg[curRemindType].remindSpeechType)
                {
                    snprintf(pChannelContext->m_SpeechFileName[writePoint], MAX_SPEECH_FILE_SIZE, "%s/remind_ring%s", SPEECH_FILE_OTHER_PATH, SPEECH_PREFIX);
                }
                else if (4 == pChannelContext->m_RemindSpeechCfg[curRemindType].remindSpeechType)
                {
                    snprintf(pChannelContext->m_SpeechFileName[writePoint], MAX_SPEECH_FILE_SIZE, "%s/remind_clock%s", SPEECH_FILE_OTHER_PATH, SPEECH_PREFIX);
                }
                else if (5 == pChannelContext->m_RemindSpeechCfg[curRemindType].remindSpeechType)
                {
                    snprintf(pChannelContext->m_SpeechFileName[writePoint], MAX_SPEECH_FILE_SIZE, "%s/remind_bird%s", SPEECH_FILE_OTHER_PATH, SPEECH_PREFIX);
                }
                else if (0 == pChannelContext->m_RemindSpeechCfg[curRemindType].remindSpeechType && (!access(SPEECH_REMIND_SCHEDULE_CUSTOM_FILE1, F_OK)))
                {
                    /* 自定义提示语音1 */
                    snprintf(pChannelContext->m_SpeechFileName[writePoint], MAX_SPEECH_FILE_SIZE, "%s", SPEECH_REMIND_SCHEDULE_CUSTOM_FILE1);
                }
                else if (6 == pChannelContext->m_RemindSpeechCfg[curRemindType].remindSpeechType && (!access(SPEECH_REMIND_SCHEDULE_CUSTOM_FILE2, F_OK)))
                {
                    /* 自定义提示语音2 */
                    snprintf(pChannelContext->m_SpeechFileName[writePoint], MAX_SPEECH_FILE_SIZE, "%s", SPEECH_REMIND_SCHEDULE_CUSTOM_FILE2);
                }
                else if (7 == pChannelContext->m_RemindSpeechCfg[curRemindType].remindSpeechType && (!access(SPEECH_REMIND_SCHEDULE_CUSTOM_FILE3, F_OK)))
                {
                    /* 自定义提示语音3 */
                    snprintf(pChannelContext->m_SpeechFileName[writePoint], MAX_SPEECH_FILE_SIZE, "%s", SPEECH_REMIND_SCHEDULE_CUSTOM_FILE3);
                }
                else
                {
                    snprintf(pChannelContext->m_SpeechFileName[writePoint], MAX_SPEECH_FILE_SIZE, "%s/remind_canon%s", SPEECH_FILE_OTHER_PATH, SPEECH_PREFIX);
                }
            }
            break;

        /*升级相关*/
        case SPEECH_UPGRADE_START:
            snprintf(pChannelContext->m_SpeechFileName[writePoint], MAX_SPEECH_FILE_SIZE, "%s%s/upgradeStart%s", pChannelContext->m_SpeechFilePath, speechLanguageDir[nSpeechLanguage], SPEECH_PREFIX);
            break;

        case SPEECH_UPGRADE_END_REBOOT:
            snprintf(pChannelContext->m_SpeechFileName[writePoint], MAX_SPEECH_FILE_SIZE, "%s%s/upgradeOver%s", pChannelContext->m_SpeechFilePath, speechLanguageDir[nSpeechLanguage], SPEECH_PREFIX);
            break;

        case SPEECH_UPGRADE_FAIL:
            snprintf(pChannelContext->m_SpeechFileName[writePoint], MAX_SPEECH_FILE_SIZE, "%s%s/upgradeFail%s", pChannelContext->m_SpeechFilePath, speechLanguageDir[nSpeechLanguage], SPEECH_PREFIX);
            break;

        case SPEECH_UPGRADE_FAIL_ERROR_VERSION:
            snprintf(pChannelContext->m_SpeechFileName[writePoint], MAX_SPEECH_FILE_SIZE, "%s%s/upgradeFileErr%s", pChannelContext->m_SpeechFilePath, speechLanguageDir[nSpeechLanguage], SPEECH_PREFIX);
            break;

        /*系统相关*/
        case SPEECH_SYS_RESET:
            snprintf(pChannelContext->m_SpeechFileName[writePoint], MAX_SPEECH_FILE_SIZE, "%s%s/systemReset%s", pChannelContext->m_SpeechFilePath, speechLanguageDir[nSpeechLanguage], SPEECH_PREFIX);
            break;

        case SPEECH_SYS_STARTING:
            snprintf(pChannelContext->m_SpeechFileName[writePoint], MAX_SPEECH_FILE_SIZE, "%s%s/systemStarting%s", pChannelContext->m_SpeechFilePath, speechLanguageDir[nSpeechLanguage], SPEECH_PREFIX);
            break;

        case SPEECH_SYS_START_OVER:
            snprintf(pChannelContext->m_SpeechFileName[writePoint], MAX_SPEECH_FILE_SIZE, "%s%s/systemStartOver%s", pChannelContext->m_SpeechFilePath, speechLanguageDir[nSpeechLanguage], SPEECH_PREFIX);
            break;

        case SPEECH_SYS_QRCODE_SCAN_OVER:
            snprintf(pChannelContext->m_SpeechFileName[writePoint],MAX_SPEECH_FILE_SIZE, "%s/qrCodeScanOver%s", SPEECH_FILE_OTHER_PATH, SPEECH_PREFIX);
            break;

        case SPEECH_SYS_RESET_SUCCESS:
            snprintf(pChannelContext->m_SpeechFileName[writePoint],MAX_SPEECH_FILE_SIZE, "%s%s/systemResetSucc%s",pChannelContext->m_SpeechFilePath, speechLanguageDir[nSpeechLanguage], SPEECH_PREFIX);
            break;

        case SPEECH_FACTORY_AUDIO_TEST:
            snprintf(pChannelContext->m_SpeechFileName[writePoint], MAX_SPEECH_FILE_SIZE, "%s/motionAlarm%s", SPEECH_FILE_OTHER_PATH, SPEECH_PREFIX);
            break;

    #ifdef __ONE_CLICK_CALL__

        case SPEECH_KEY2CALL_START:
            snprintf(pChannelContext->m_SpeechFileName[writePoint], MAX_SPEECH_FILE_SIZE, "%s%s/key2call_start%s", pChannelContext->m_SpeechFilePath, speechLanguageDir[nSpeechLanguage], SPEECH_PREFIX);
            break;

        case SPEECH_KEY2CALL_OFFLINE:
            snprintf(pChannelContext->m_SpeechFileName[writePoint], MAX_SPEECH_FILE_SIZE, "%s%s/key2call_offline%s", pChannelContext->m_SpeechFilePath, speechLanguageDir[nSpeechLanguage], SPEECH_PREFIX);
            break;

        case SPEECH_KEY2CALL_DU:
            snprintf(pChannelContext->m_SpeechFileName[writePoint], MAX_SPEECH_FILE_SIZE, "%s/key2call_du%s", SPEECH_FILE_OTHER_PATH, SPEECH_PREFIX);
            break;

        case SPEECH_KEY2CALL_NO_ANSWER:
            snprintf(pChannelContext->m_SpeechFileName[writePoint], MAX_SPEECH_FILE_SIZE, "%s%s/key2call_noAnswer%s", pChannelContext->m_SpeechFilePath, speechLanguageDir[nSpeechLanguage], SPEECH_PREFIX);
            break;
    #endif

    #ifdef __AGINGTEST__
        case SPEECH_AGING_TEST:
        snprintf(pChannelContext->m_SpeechFileName[writePoint], MAX_SPEECH_FILE_SIZE,"%s", SD_AGING_TEST_AUDIO_FILE_NAME);
            break;
    #endif
    #ifdef __MODULE_4G_SUPPORT__
        case SPEECH_4G_SYS_RESET: //系统复位
            snprintf(pChannelContext->m_SpeechFileName[writePoint], MAX_SPEECH_FILE_SIZE, "%s%s/4GsystemReset%s", pChannelContext->m_SpeechFilePath, speechLanguageDir[nSpeechLanguage], SPEECH_PREFIX);
            break;
        case SPEECH_4G_SYS_RESET_SUCCESS: //系统复位
            snprintf(pChannelContext->m_SpeechFileName[writePoint], MAX_SPEECH_FILE_SIZE, "%s%s/4GsystemResetSucc%s", pChannelContext->m_SpeechFilePath, speechLanguageDir[nSpeechLanguage], SPEECH_PREFIX);
            break;
        case SPEECH_4G_SYS_STARTING: //系统启动中
            snprintf(pChannelContext->m_SpeechFileName[writePoint], MAX_SPEECH_FILE_SIZE, "%s%s/4GsystemStarting%s", pChannelContext->m_SpeechFilePath, speechLanguageDir[nSpeechLanguage], SPEECH_PREFIX);
            break;
        case SPEECH_4G_SYS_START_OVER: //系统启动成功
            snprintf(pChannelContext->m_SpeechFileName[writePoint], MAX_SPEECH_FILE_SIZE, "%s%s/4GsystemStartOver%s", pChannelContext->m_SpeechFilePath, speechLanguageDir[nSpeechLanguage], SPEECH_PREFIX);
            break;
        case SPEECH_4G_USE_WIRED: //使用有线网络
            snprintf(pChannelContext->m_SpeechFileName[writePoint], MAX_SPEECH_FILE_SIZE, "%s%s/4GUseWired%s", pChannelContext->m_SpeechFilePath, speechLanguageDir[nSpeechLanguage], SPEECH_PREFIX);
            break;
        case SPEECH_4G_CONNECT_SUCCESS: //网络连接成功
            snprintf(pChannelContext->m_SpeechFileName[writePoint], MAX_SPEECH_FILE_SIZE, "%s%s/4GConnectSuccess%s", pChannelContext->m_SpeechFilePath, speechLanguageDir[nSpeechLanguage], SPEECH_PREFIX);
            break;
        case SPEECH_4G_CONNECT_FAILED: //网络连接失败
            snprintf(pChannelContext->m_SpeechFileName[writePoint], MAX_SPEECH_FILE_SIZE, "%s%s/4GConnectFailed%s", pChannelContext->m_SpeechFilePath, speechLanguageDir[nSpeechLanguage], SPEECH_PREFIX);
            break;
        case SPEECH_4G_MODULE_CHECK_SUCCESS: //4G模块检测成功
            snprintf(pChannelContext->m_SpeechFileName[writePoint], MAX_SPEECH_FILE_SIZE, "%s%s/4GModuleCheckSuccess%s", pChannelContext->m_SpeechFilePath, speechLanguageDir[nSpeechLanguage], SPEECH_PREFIX);
            break;
        case SPEECH_4G_MODULE_CHECK_FAILED: //4G模块检测成功
            snprintf(pChannelContext->m_SpeechFileName[writePoint], MAX_SPEECH_FILE_SIZE, "%s%s/4GModuleCheckFailed%s", pChannelContext->m_SpeechFilePath, speechLanguageDir[nSpeechLanguage], SPEECH_PREFIX);
            break;
        case SPEECH_4G_SIM_CHECK_SUCCESS: //SIM卡检测成功
            snprintf(pChannelContext->m_SpeechFileName[writePoint], MAX_SPEECH_FILE_SIZE, "%s%s/4GSimCheckSuccess%s", pChannelContext->m_SpeechFilePath, speechLanguageDir[nSpeechLanguage], SPEECH_PREFIX);
            break;
        case SPEECH_4G_SIM_CHECK_FAILED: //SIM卡检测失败
            snprintf(pChannelContext->m_SpeechFileName[writePoint], MAX_SPEECH_FILE_SIZE, "%s%s/4GSimCheckFailed%s", pChannelContext->m_SpeechFilePath, speechLanguageDir[nSpeechLanguage], SPEECH_PREFIX);
            break;
        case SPEECH_4G_SIM_NOT_ORIGINAL:
            snprintf(pChannelContext->m_SpeechFileName[writePoint], MAX_SPEECH_FILE_SIZE, "%s%s/4GSimNotOriginal%s", pChannelContext->m_SpeechFilePath, speechLanguageDir[nSpeechLanguage], SPEECH_PREFIX);
            break;
        case SPEECH_4G_CONNECT_FAILED_CHECK_SIMCARD:
            snprintf(pChannelContext->m_SpeechFileName[writePoint], MAX_SPEECH_FILE_SIZE, "%s%s/4GConnFailCheckSimcard%s", pChannelContext->m_SpeechFilePath, speechLanguageDir[nSpeechLanguage], SPEECH_PREFIX);
            break;
        case SPEECH_4G_CONNECTING_PLEASE_WAIT:
            snprintf(pChannelContext->m_SpeechFileName[writePoint], MAX_SPEECH_FILE_SIZE, "%s%s/4GCregWaitC%s", pChannelContext->m_SpeechFilePath, speechLanguageDir[nSpeechLanguage], SPEECH_PREFIX);
            break;
        case SPEECH_4G_INSUFFICIENT_BALANCE:
            snprintf(pChannelContext->m_SpeechFileName[writePoint], MAX_SPEECH_FILE_SIZE, "%s%s/4GInsufficientBalance%s",pChannelContext->m_SpeechFilePath, speechLanguageDir[nSpeechLanguage], SPEECH_PREFIX);
            break;
    #endif
        default:
            break;
    }

    XTRACE_MUTEX_UNLOCK(&pChannelContext->m_SpeechLock);
    ZW_DEBUG("---speech_voice----type=%d, language=[%d---%d], filename=%s\n", speech, pChannelContext->m_enSpeechLanguage, nSpeechLanguage, pChannelContext->m_SpeechFileName[writePoint]);
    return 0;
}

/* 设置报警播放配置 */
int HY_Enc_SetAlarmPlayCfg(eSpeechLanguage speech_language)
{
    CChannelContext *pChannelContext = NULL;
    HY_Enc_GetEncChInfo(&pChannelContext);

    pChannelContext->m_enSpeechLanguage = speech_language;

    return 0;
}

int HY_Enc_SwitchAOMode(int mode)
{
    CChannelContext        *pChannelContext = NULL;
    HY_Enc_GetEncChInfo(&pChannelContext);

    return Hi_MPP_SwitchAOMode(pChannelContext->pPlatformChannelhandle, mode);
}

int HY_Enc_SetAudioCustomVoice(CChannelContext *hChannelHandle, int customtype, char *pBuff, int len)
{
    int ret = 0;
    FILE *pFd = NULL;
    int writeLen = 0;
    void* amr_encoder = NULL;
    char* filepath = NULL;

    /* customtype：0：报警语音 1：定时提醒语音1 2：定时提醒语音2 3：定时提醒语音3 */
    switch (customtype)
    {
        case 0: filepath = SPEECH_MOTION_ALARM_CUSTOM_FILE_TEMP; break;
        case 1: filepath = SPEECH_REMIND_SCHEDULE_CUSTOM_FILE1_TMP; break;
        case 2: filepath = SPEECH_REMIND_SCHEDULE_CUSTOM_FILE2_TMP; break;
        case 3: filepath = SPEECH_REMIND_SCHEDULE_CUSTOM_FILE3_TMP; break;
        default: ZW_ERROR("Parameter error: [%d]\n", customtype); return -1;
    }

    //将音频数据先写入内存中，由使用该文件的地方拷贝到配置分区中
    pthread_mutex_lock(&hChannelHandle->m_SpeechCustomAMRFileLock);

    pFd = fopen(filepath, "wb");
    if (NULL == pFd)
    {
        ZW_ERROR("Open file %s fail!\n", filepath);
        ret = -1;
        goto end;
    }

    // 写入AMR文件头
    const char amr_header[] = "#!AMR\n";
    writeLen = fwrite(amr_header, 1, sizeof(amr_header) - 1, pFd);
    if (writeLen != sizeof(amr_header) - 1) {
        ZW_ERROR("fwrite amr_header failed! [%d]\n", writeLen);
        ret = -1;
        goto end;
    }

    // 初始化AMR编码器
    amr_encoder = Encoder_Interface_init(0); // 0=禁用DTX
    if (!amr_encoder) {
        ZW_ERROR("Encoder_Interface_init failed!\n");
        ret = -1;
        goto end;
    }

    // 设置目标比特率 (MR59=5.9kbps)
    enum Mode target_mode = MR59;

    // 处理缓冲区
    unsigned char *g711_ptr = (unsigned char *)pBuff;
    short pcm_buffer[G711U_FRAME_SIZE];
    unsigned char amr_frame[AMRNB_FRAME_SIZE];
    int processed = 0;
    int frame_count = 0;

    while (processed < len)
    {
        int bytes_to_process = (len - processed) > G711U_FRAME_SIZE ? G711U_FRAME_SIZE : (len - processed);

        // 转换G711U到PCM
        G711U_DECODE_PCM(pcm_buffer, &(g711_ptr[processed]), bytes_to_process);

        // 如果读取不足一帧，填充静音
        if (bytes_to_process < G711U_FRAME_SIZE) {
            memset(pcm_buffer + bytes_to_process, 0, (G711U_FRAME_SIZE - bytes_to_process) * sizeof(short));
        }

        // AMR编码
        int frame_size = Encoder_Interface_Encode(amr_encoder, target_mode, pcm_buffer, amr_frame, 0);

        if (frame_size <= 0) {
            ZW_ERROR("Encoder_Interface_Encode Failed!, [%d] [%d]\n", frame_size, frame_count);
            frame_count++;
            continue; // 继续处理下一帧
        }

        // 写入AMR帧
        writeLen = fwrite(amr_frame, 1, frame_size, pFd);
        if (writeLen != frame_size) {
            ZW_ERROR("fwrite failed! [%d] [%d-%d]\n", frame_count, writeLen, frame_size);
            ret = -1;
            goto end;
        }

        processed += bytes_to_process;
        frame_count++;
    }

end:
    if (amr_encoder != NULL) {
        Encoder_Interface_exit(amr_encoder);
    }

    if (pFd != NULL) {
        fclose(pFd);
    }

    pthread_mutex_unlock(&hChannelHandle->m_SpeechCustomAMRFileLock);
    return ret;
}

int HY_Enc_GetBitrateInfo(CChannelContext *pChannelContext, int dataArr[], int arrlen)
{
    return Hi_MPP_GetBitrateInfo(pChannelContext->pPlatformChannelhandle, dataArr, arrlen);
}

int HY_Enc_RegisterKeyStautsCallBack(KEY_STATUS_CALLBACK getKeyStateCallback)
{
    CChannelContext *pChannelContext = NULL;

    HY_Enc_GetEncChInfo(&pChannelContext);
    pChannelContext->m_GetKeyStatusCallBack = getKeyStateCallback;

    return 0;
}
#ifdef __SUPPORT_WIFI_FUNC__
int HY_Res_SetLinkStatus(int status)
{
    int ret = 0;
    //linkstatus = !status;

    return ret;
}

int HY_Res_SetWifiStatusLed(WIFI_STATUS_T wifiStatus)
{
    return 0;
}
#endif

int HY_Res_SetCurrNetCardType(XRS_NETCARD_TYPE_E status)
{
    SystemInfo_t *pSysInfo = NULL;

    HY_Res_GetSystemInfo(&pSysInfo);
    pSysInfo->m_NicStatus = status;

    return 0;
}

void* DrawLineThread(void * lpParameter)
{
    PRT_TID_NAME("DrawLineThread");
    ZW_DEBUG("%s Thread PID = %d !!!!!!!!!!!!\n", __FUNCTION__, getpid());

    CChannelContext *pChnCtx = NULL;
    SystemInfo_t *pSysInfo = NULL;
    unsigned long long newTimeMs = get_ms();
    unsigned long long lastTimeMs = newTimeMs;
    int callCyclMs = 40;
    ThreadInfo_t *pParent = (ThreadInfo_t *)lpParameter;

       HY_Enc_GetEncChInfo(&pChnCtx);

    HY_Res_GetSystemInfo(&pSysInfo);

    ZW_INFO(" Hi_MPP_DrawLineThread------------------------------------------------\n");

    while (1)
    {
        if (pParent->m_Exit == 1)
        {
            break;
        }

        XTRACE_MUTEX_LOCK(&pChnCtx->m_EncLock);
        XTRACE_MUTEX_LOCK(&pSysInfo->m_SysOSDLock);

        //计算函数调用的时间间隔
        newTimeMs = get_ms();
        callCyclMs = newTimeMs - lastTimeMs;
        lastTimeMs = newTimeMs;
        callCyclMs = callCyclMs > 1000 ? 1000 : (callCyclMs < 0 ? 40 : callCyclMs);

        //处理人形检测结果及布防区域
        Hi_MPP_ReadyDrawLineInfo(pChnCtx->pPlatformChannelhandle, pChnCtx->m_HumanAreaOverlayEnable, 0, 0, callCyclMs);

        XTRACE_MUTEX_UNLOCK(&pSysInfo->m_SysOSDLock);
        XTRACE_MUTEX_UNLOCK(&pChnCtx->m_EncLock);
        usleep(40000);
    }

	//清空及销毁画框区域
	Hi_MPP_ClearDrawLineInfo(1);

    ZW_DEBUG("DrawLineThread Exit \n");

    return NULL;

}

//-----------------------------------------------------------------------------
//    说明：        设备外设控制
//    输入参数：AssistantCtrlInfo    pCtrlInfo    控制信息
//    返回:        成功为 0 , 其它为定义的错误号；
//-----------------------------------------------------------------------------
int HY_AssistantControl(AssistantCtrlInfo *pCtrlInfo)
{
    int s32Ret = 0;
    SystemInfo_t *pSysInfo = NULL;
    CChannelContext* pChCtx = NULL;
    LIGHT_CTX_S* lightCtx = NULL;

    if (NULL == pCtrlInfo)
    {
        ZW_ERROR("AssistantControl param invalid\n");
        return -1;
    }

    HY_Res_GetSystemInfo(&pSysInfo);
    HY_Enc_GetEncChInfo(&pChCtx);

    lightCtx = &pChCtx->m_stLightCtx;

    if (1 == pCtrlInfo->nCtrlStatus)
    {
        pChCtx->m_nAssistantControlTest |= 1 << pCtrlInfo->enAssisstantType;
    }

    // 更新灯板控制上下文
    _LightCtl_Update(lightCtx);

    switch (pCtrlInfo->enAssisstantType)
    {
        case PDEV_WHITE_LED:    //白光灯
        {
            s32Ret = LightCtl_AsstCtlWhiteLight(lightCtx, pCtrlInfo->nCtrlStatus ? PWM_MAX_DUTY : 0);
            if (s32Ret)
            {
                ZW_ERROR("LightCtl_AsstCtlWhiteLight Failed!\n");
            }

            break;
        }
        case PDEV_INFRARED_LED:    //红外灯
        {
            //以下灯板类型支持主板控制红灯
            s32Ret = LightCtl_AsstCtlRedLight(lightCtx, pCtrlInfo->nCtrlStatus ? PWM_MAX_DUTY : 0);
            if (s32Ret)
            {
                ZW_ERROR("LightCtl_AsstCtlRedLight Failed!\n");
            }

            if(0 == pCtrlInfo->nCtrlStatus)
            {
                lightCtx->IspCtrl(LIGHTCTL_IRCUT_DAY);
            }
            else
            {
                lightCtx->IspCtrl(LIGHTCTL_IRCUT_NIGHT);
            }

            break;
        }
        case PDEV_RB_WARN_LED:    //红蓝爆闪灯(暂不支持)
        {
            if (pCtrlInfo->nCtrlStatus)
            {
                //pChContext->m_nRedBlueStatus = 1;
            }
            else
            {
                //pChContext->m_nRedBlueStatus = 0;
            }
            break;
        }
        default:
        {
            ZW_ERROR("Unsupport dev type:%d\n", pCtrlInfo->enAssisstantType);
            break;
        }
    }

    if (0 == pCtrlInfo->nCtrlStatus)
    {
        pChCtx->m_nAssistantControlTest &= ~(1 << pCtrlInfo->enAssisstantType);
    }
    return s32Ret;
}

// 说明: 根据优先级计算扬声器模式
// 参数: nSpeechTalkingMode 已经打开的模式,bit0 表示播报 bit1表示对讲
// 返回: 扬声器模式 0=>关闭 SPEECH_ENABLE=>播报模式 TALK_ENABLE=>对讲模式
static int HY_Speaker_CalcMode(SystemInfo_t *pSysInfo, int nSpeechTalkingMode)
{
    // 对讲优先
    if (1/*1 == pSysInfo->m_SpeakerPriority*/)
    {
        if (nSpeechTalkingMode & TALK_ENABLE)
        {
            return TALK_ENABLE;
        }
        else if (nSpeechTalkingMode & SPEECH_ENABLE)
        {
            return SPEECH_ENABLE;
        }
    }
    // 播报优先
    // (0 == pSysInfo->m_SpeakerPriority)
    else
    {
        if (nSpeechTalkingMode & SPEECH_ENABLE)
        {
            return SPEECH_ENABLE;
        }
        else if (nSpeechTalkingMode & TALK_ENABLE)
        {
            return TALK_ENABLE;
        }
    }

    return 0;
}

int HY_Speaker_CalcCurrMode(SystemInfo_t *pSysInfo)
{
    return HY_Speaker_CalcMode(pSysInfo, pSysInfo->m_SpeechTalkingMode);
}

int HY_Speaker_CalcNextMode(SystemInfo_t *pSysInfo, int nMode, int bEnable)
{
    int nCurrMode = pSysInfo->m_SpeechTalkingMode;

    if (bEnable)
    {
        nCurrMode |= nMode;
    }
    else
    {
        nCurrMode &= (~nMode);
    }

    return HY_Speaker_CalcMode(pSysInfo, nCurrMode);
}

int HY_Speaker_ChangeMode(SystemInfo_t *pSysInfo, int nMode, int bEnable)
{
    int nNextMode = 0;
    int nCurrMode = 0;

    nCurrMode = HY_Speaker_CalcCurrMode(pSysInfo);
    nNextMode = HY_Speaker_CalcNextMode(pSysInfo, nMode, bEnable);

    if (bEnable)
    {
        pSysInfo->m_SpeechTalkingMode |= nMode;
    }
    else
    {
        pSysInfo->m_SpeechTalkingMode &= (~nMode);
    }

    if (nCurrMode == nNextMode)
    {
        return 0;
    }
    // 打开对讲
    else if (nNextMode == TALK_ENABLE)
    {
        // 音频小板关闭状态
        if (0 == nCurrMode)
        {
            //音频小板使能
            Hi_MPP_SetSpeakerEn(1);
        }
        HY_Enc_SwitchAOMode(0);//将音频输出数据类型切为G711
        //音频小板使能
        Hi_MPP_SetSpeakerEn(1);
    }
    else if (nNextMode == SPEECH_ENABLE)
    {
        // 音频小板关闭状态
        if (0 == nCurrMode)
        {
            //音频小板使能
            Hi_MPP_SetSpeakerEn(1);
        }
        if(2 == pSysInfo->m_SpeechMode)//如果是pcm
        {
            HY_Enc_SwitchAOMode(1);//将音频输出数据类型切为PCM
        }
        else if (1 == pSysInfo->m_SpeechMode)
        {
            HY_Enc_SwitchAOMode(0);//将音频输出数据类型切为G711u
        }
    }
    // 关闭音频
    else if (0 == nNextMode)
    {
        //音频小板禁用
        Hi_MPP_SetSpeakerEn(0);
    }

    return 0;
}

/*******硬件接口封装***********/
/**
 * @description: 初始化KS HalMod模块
 * @return {int} 正常返回0，异常返回<0
 */
SINT32 HY_HalModInit()
{
    return Hi_MPP_HalModInit();
}

/**
 * @description: 反初始化KS HalMod
 * @return {SINT32} 正常返回0，异常返回<0
 */
SINT32 HY_HalModDeInit()
{
    return Hi_MPP_HalModDeInit();
}

/**
 * @description: 获取设备得唯一CHIP ID
 * @param {SINT8} *buffer 缓冲区
 * @param {SINT32} len 缓冲区长度
 * @return {SINT32} 0-成功， 非0-失败
 */
SINT32 HY_GetDevChipID(SINT8 *buffer, SINT32 len)
{
    return Hi_MPP_GetDevChipID(buffer, len);
}

/**
 * @description: 设置系统启动状态标志
 * @param {int} Stat 启动状态标记
 * @return {int} 正常返回0，异常返回<0
 */
SINT32 HY_SetSysStartTag(int Stat)
{
    return Hi_MPP_SetSysStartTag(Stat);
}

int HY_Enc_GetRealKeyFrameInterval(int nChannel,int *nFrameRateInterval,int *nKeyFrameInterval)
{
    if (Hi_MPP_GetRealKeyFrameInterval(nChannel,nFrameRateInterval,nKeyFrameInterval))
    {
        ZW_ERROR("HY_Enc_GetRealKeyFrameInterval fail !\n");
        return -1;
    }

    return 0;
}

//设置休眠状态 0为唤醒状态，1为休眠状态
int HY_Enc_SetDormancyState(int iState)
{
    FILE *file = NULL;
    struct stat buf = {0};
    SystemInfo_t *pSysInfo = NULL;

    HY_Res_GetSystemInfo(&pSysInfo);

    /* 初始化or反初始化休眠视频帧buf */
    do {
        if (iState == 0)
        {
            HIEASY_FREE(pSysInfo->pDormancyIFrameBuffer);
            HIEASY_FREE(pSysInfo->pDormancyPFrameBuffer);
            pSysInfo->m_DormancyIFrameBufferSize = 0;
            pSysInfo->m_DormancyPFrameBufferSize = 0;
        }
        else
        {
            /* 初始化pDormancyIFrameBuffer，读取休眠视频I帧信息 */
            if (stat("/mnt/app/bin/SleepScreenIFrame.bin", &buf) == -1)
            {
                ZW_ERROR("stat /mnt/app/bin/SleepScreenIFrame.bin failed!!!!\n");
                break;
            }
            if (NULL == pSysInfo->pDormancyIFrameBuffer)
            {
                pSysInfo->m_DormancyIFrameBufferSize = buf.st_size;
                pSysInfo->pDormancyIFrameBuffer = (unsigned char *)malloc(pSysInfo->m_DormancyIFrameBufferSize);
                if (NULL == pSysInfo->pDormancyIFrameBuffer)
                {
                    ZW_ERROR("pDormancyIFrameBuffer malloc failed!\n");
                    break;
                }

                file = fopen("/mnt/app/bin/SleepScreenIFrame.bin","rb");
                if (file != NULL )
                {
                    fread(pSysInfo->pDormancyIFrameBuffer, 1, pSysInfo->m_DormancyIFrameBufferSize, file);
                    fclose(file);
                    file = NULL;
                }
            }

            /* 初始化pDormancyPFrameBuffer，读取休眠视频P帧信息 */
            if (stat("/mnt/app/bin/SleepScreenPFrame.bin", &buf) == -1)
            {
                ZW_ERROR("stat /mnt/app/bin/SleepScreenPFrame.bin failed!!!!\n");
                break;
            }
            if (NULL == pSysInfo->pDormancyPFrameBuffer)
            {
                pSysInfo->m_DormancyPFrameBufferSize = buf.st_size;
                pSysInfo->pDormancyPFrameBuffer = (unsigned char *)malloc(pSysInfo->m_DormancyPFrameBufferSize);
                if (NULL == pSysInfo->pDormancyPFrameBuffer)
                {
                    ZW_ERROR("pDormancyPFrameBuffer malloc failed!\n");
                    break;
                }

                file = fopen("/mnt/app/bin/SleepScreenPFrame.bin","rb");
                if (file != NULL )
                {
                    fread(pSysInfo->pDormancyPFrameBuffer, 1, pSysInfo->m_DormancyPFrameBufferSize, file);
                    fclose(file);
                    file = NULL;
                }
            }
        }
    } while(0);

    /* 静音or取消静音 */
#if 0
    CChannelContext *pChannelContext = NULL;
    HY_Enc_GetEncChInfo(&pChannelContext);
    CPlatformChannelContext *pChContext = (CPlatformChannelContext *)pChannelContext->pPlatformChannelhandle;
    if (iState) {
        Hi_MPP_SwitchAudioInModeParam(pChContext, AUDIO_DORMANCY_MODE);
        Hi_MPP_SwitchAudioOutModeParam(pChContext, AUDIO_DORMANCY_MODE);
    } else {
        Hi_MPP_SwitchAudioInModeParam(pChContext, AUDIO_MONITORING_MODE);
        Hi_MPP_SwitchAudioOutModeParam(pChContext, AUDIO_MONITORING_MODE);
    }
#endif
    /* 目前这个休眠状态是会不断检查的，因此不做锁 */
    pSysInfo->m_iDormancyState = iState;

    return 0;
}

/*
切换低帧模式的线程：线程外控制低帧模式状态，线程内部计时回到低帧模式。1.打开直播 2.触发报警 恢复主码流帧率
线程外状态：
1.打开直播 -> 需要一直处于正常帧率模式
2.触发报警 -> 恢复正常帧率模式3分钟，3分钟后自动降帧。如果中途重复触发报警，重置3分钟计时
3.关闭直播 -> 转为触发报警模式，仍保持3分钟的正常帧率模式，3分钟后自动降帧
设置编码帧率时，只判断当前低帧模式 ?= FORCESLOW
*/
void *SlowFrameModeSwitchThread(void * lpParameter)
{
    PRT_TID_NAME("SlowFrameModeSwitchThread");
    ZW_DEBUG("%s Thread PID = %d !!!!!!!!!!!!\n", __FUNCTION__, getpid());

    ThreadInfo_t    *pParent = (ThreadInfo_t *)lpParameter;

    int SwitchCnt = 0;
    int FrameRateMain = 12;
    int FrameRateAux = 12;
    int LastSlowFrameMode = 0;
    int CurSlowFrameMode = 0;
#if defined (__IS_SUPPORT_ADUJUST_SENSOR_FRAME__)
#if defined(__IS_SUPPORT_12FPS__)
    unsigned int LastSensorFrame = 12;
    unsigned int CurSensorFrame = 12;
#else
    unsigned int LastSensorFrame = 10;
    unsigned int CurSensorFrame = 10;
#endif

#endif
    CChannelContext *pChannelContext = NULL;
    HY_Enc_GetEncChInfo(&pChannelContext);
    LastSlowFrameMode = SLOWFRAME_SCENE_NONE;

    while(1)
    {
        if (pParent->m_Exit == 1)
        {
            break;
        }

        usleep(500*1000);

        CurSlowFrameMode = pChannelContext->m_iSlowFrameMode;
#if defined (__IS_SUPPORT_ADUJUST_SENSOR_FRAME__)
        //获取当前sensor帧率
        GetCurSensorFrame(&CurSensorFrame);

        if (LastSlowFrameMode != CurSlowFrameMode || LastSensorFrame != CurSensorFrame)
#else
        if (LastSlowFrameMode != CurSlowFrameMode)
#endif
        {
#if defined (__IS_SUPPORT_ADUJUST_SENSOR_FRAME__)
            FrameRateMain = pChannelContext->m_EncAttr.FrameRate > CurSensorFrame ? CurSensorFrame : pChannelContext->m_EncAttr.FrameRate;
            FrameRateAux = pChannelContext->m_EncAttrAux.FrameRate > CurSensorFrame ? CurSensorFrame : pChannelContext->m_EncAttrAux.FrameRate;
#else
            FrameRateMain = pChannelContext->m_EncAttr.FrameRate;
            FrameRateAux = pChannelContext->m_EncAttrAux.FrameRate;
#endif
            HY_Enc_SetFrameRate(pChannelContext->pPlatformChannelhandle,(ENCODE_STREAM_E)0, FrameRateMain, CurSlowFrameMode);
            HY_Enc_SetFrameRate(pChannelContext->pPlatformChannelhandle,(ENCODE_STREAM_E)1, FrameRateAux, CurSlowFrameMode);
#if defined (__IS_SUPPORT_ADUJUST_SENSOR_FRAME__)
            if (LastSensorFrame != CurSensorFrame)
            {
                LastSensorFrame = CurSensorFrame;
            }
            if (LastSlowFrameMode != CurSlowFrameMode)
            {
                LastSlowFrameMode = CurSlowFrameMode;
            }
#else
            LastSlowFrameMode = CurSlowFrameMode;
#endif
            if (CurSlowFrameMode == SLOWFRAME_SCENE_NONE)
            {
                SwitchCnt = SLOWFRAME_SWITCH_INTERVAL;
            }
        }

        //重复触发报警时重置计时间隔
        if (pChannelContext->m_iSlowFrameResetFlag == 1)
        {
            SwitchCnt = SLOWFRAME_SWITCH_INTERVAL;
            pChannelContext->m_iSlowFrameResetFlag = 0;
        }

        //低帧模式关闭计时
        if (CurSlowFrameMode == SLOWFRAME_SCENE_FORCESLOW)
        {
            SwitchCnt = 0;
        }

        //开流模式始终使能计时间隔 = MAX
        if (pChannelContext->m_isRuRemotepreview >= 1)
        {
            SwitchCnt = SLOWFRAME_SWITCH_INTERVAL;
        }

        if (SwitchCnt > 0)
        {
            SwitchCnt--;
        }
        else
        {
            XTRACE_MUTEX_LOCK(&pChannelContext->m_SlowFrameLock);
             pChannelContext->m_iSlowFrameMode = SLOWFRAME_SCENE_FORCESLOW;
            XTRACE_MUTEX_UNLOCK(&pChannelContext->m_SlowFrameLock);
        }
    }

    return NULL;
}

int HY_Enc_GetProductMode()
{
    return g_nProductTest;
}

int HY_Enc_SetProductMode(int nProductMode)
{
    g_nProductTest = nProductMode;

    return 0;
}

int HY_Enc_SetAudioDetectCfg(CChannelContext *pChannelContext, LpAudioDetectCfg pCfg)
{
    return Hi_MPP_SetAudioDetectCfg(pChannelContext->pPlatformChannelhandle, pCfg);
}

/* 根据不同的抗闪模式设置VI和VPSS的帧率(CLOSE||50HZ:12.5fps； 60HZ:12fps) */
int HY_ChangeSensorFrameRate(CChannelContext *pChannelContext, CmosFrequencyType_t antiFlickerNew)
{

    ZW_DEBUG("IN _HY_ChangeSensorFrameRate -->>\n");
    SystemInfo_t *pSysInfo = NULL;
    CChannelContext *pChCtx = pChannelContext;

    int i = 0, RetVal = 0;
    FLOAT32 ViFps = 0.0;

    HY_Res_GetSystemInfo(&pSysInfo);
    if(CMOS_FREQUENCY_50HZ == antiFlickerNew || CMOS_FREQUENCY_CLOSE == antiFlickerNew)//从60hz切换到50hz
    {
        ViFps = 12.5;
    }
    else
    {
        ViFps = 12.0;
    }

    ZW_DEBUG("[CMOS:%d] Change ISP to %f fps\n", antiFlickerNew, ViFps);

     XTRACE_MUTEX_LOCK(&pChCtx->m_VencLock);
    /*25 帧和 30 帧切换时先不检测人形,防止引起误报*/
    XTRACE_MUTEX_LOCK(&pChCtx->m_InitVPSSLock);

    for (i = 0; i < SUPPORT_MAX_SENSOR_COUNT; i++)
    {
        RetVal = Hi_MPP_ChangeSensorFps(0, i, ViFps);
        if (RetVal < 0)
        {
            ZW_ERROR("_HY_Enc_SetSensorFps Set Sensor %d Failed RetVal = %d\n", i, RetVal);
            break;
        }
    }

    XTRACE_MUTEX_UNLOCK(&pChCtx->m_InitVPSSLock);
    XTRACE_MUTEX_UNLOCK(&pChCtx->m_VencLock);

    if (RetVal < 0)
    {
        return -1;
    }

    return RetVal;
}

int HY_Enc_SwitchAecMode(int AecMode)
{
    CChannelContext *pChannelContext = NULL;
    HY_Enc_GetEncChInfo(&pChannelContext);

#if defined (__PRODUCT_P3MU__)
    int IntercomMode = 0;   /* P3MU固定为全双工对讲，防止通话时半双工mic静音 */
#else
    int IntercomMode = pChannelContext->m_IntercomMode;
#endif

    Hi_MPP_SwitchAecMode(pChannelContext->pPlatformChannelhandle, AecMode, IntercomMode);
    return 0;
}

#ifdef __ONE_CLICK_CALL__
int HY_Enc_SetKey2callhanupState(int iState)
{
    CChannelContext *pChannelContext = NULL;
    HY_Enc_GetEncChInfo(&pChannelContext);

    if (iState != 1 && iState != 2)
    {
        return 0;
    }

    pthread_mutex_lock(&pChannelContext->m_SpeechLock);

    if ( pChannelContext->m_SpeechNow == SPEECH_KEY2CALL_DU )
    {
        pChannelContext->m_recvkey2callhanup = iState;
    }

    pthread_mutex_unlock(&pChannelContext->m_SpeechLock);
    return 0;
}

int HY_Enc_SetPauseAudioAlarm()
{
    CChannelContext *pChannelContext = NULL;
    HY_Enc_GetEncChInfo(&pChannelContext);

    pChannelContext->m_recvPauseAudioLightAlarm = 1;
    return 0;
}

int HY_Enc_SetAlternateResetButton(int enable)
{
    if(enable < 0 || enable > 1)
    {
        ZW_ERROR("enable is error!\n");
        return -1;
    }
    SystemInfo_t *pSysInfo = NULL;
    HY_Res_GetSystemInfo(&pSysInfo);
    pSysInfo->m_bSupportAlternateResetButton = enable;
    return 0;
}

int HY_Enc_SetRecoveryAudioAlarm()
{
    CChannelContext *pChannelContext = NULL;
    HY_Enc_GetEncChInfo(&pChannelContext);

    pChannelContext->m_recvPauseAudioLightAlarm = 0;
    return 0;
}

int HY_Enc_SetKeyToCall(int enable)
{
    if (enable < 0 || enable > 1)
    {
        ZW_ERROR("enable is NULL!\n");
        return -1;
    }

#if defined (__PRODUCT_P3MU__)
    return 0;   /* P3MU默认支持一键呼叫，不需要定制 */
#endif

    SystemInfo_t *pSysInfo = NULL;
    HY_Res_GetSystemInfo(&pSysInfo);
    if (pSysInfo->m_bSupportKeyToCall != enable)
    {
        pSysInfo->m_bSupportKeyToCall= enable;
    }
    else
    {
        ZW_ERROR("pSysInfo->m_bSupportKeyToCall equal[%d] enable[%d] ,set fail!\n",pSysInfo->m_bSupportKeyToCall,enable);
        return -1;
    }

    if (1 == pSysInfo->m_bSupportKeyToCall)
    {
        mysystem("/mnt/app/etc/gpio_ctrl.sh GPIO_CTRL 0 1");
        //一键呼叫gpio检测
        HY_Res_CreatThread(&pSysInfo->m_KeytocallThread, 0, CheckKeytocallPinStatusThread);
    }
    else if(0 == pSysInfo->m_bSupportKeyToCall)
    {
        mysystem("/mnt/app/etc/gpio_ctrl.sh GPIO_CTRL 0 0");
        HY_Res_DestroyThread(&pSysInfo->m_KeytocallThread);
    }

    ZW_INFO("SetKeyToCall:%d\n", pSysInfo->m_bSupportKeyToCall);
    return 0;
}

int HY_Enc_SetKeyToCallHardwareType(int type)
{
    /* 只有P3MU需要定制一键呼叫按键连接方式 */
#if defined (__PRODUCT_P3MU__)
    SystemInfo_t *pSysInfo = NULL;
    HY_Res_GetSystemInfo(&pSysInfo);

    if (0 == pSysInfo->m_bSupportKeyToCall)
    {
        ZW_PRINTF("Not SupportKeyToCall!\n");
        return 0;
    }

    if (0 == type)
    {
        /* GPIO方式，双GPIO分别控制呼叫和挂断响应 */
        mysystem("/mnt/app/etc/SetCallType.sh 0");
    }
    else
    {
        /* ADC方式，单ADC采样控制呼叫和挂断响应 */
        mysystem("/mnt/app/etc/SetCallType.sh 1");
    }
#endif

    ZW_INFO("SetKeyToCallHardwareType:%d\n", type);
    return 0;
}
#endif

int HY_Enc_GetWiFiInfo(void *lib_wifiInfo)
{
    if (lib_wifiInfo == NULL)
    {
        ZW_ERROR("input para error\n");
        return -1;
    }

    if (Hi_MPP_GetWiFiInfo(lib_wifiInfo))
    {
        ZW_ERROR("=============HY_Enc_GetWiFiInfo get wifi info error===========\n");
        return -1;
    }

    return 0;
}

SINT32 HY_Res_P2PNotifySendDataFail(const char * pBuffer, UINT32 dwBufLen)
{
    static unsigned int LastRealTimeSec = 0;

    SystemInfo_t *pSysInfo = NULL;
    HY_Res_GetSystemInfo(&pSysInfo);

    PH264DATAPACKETHEADER pFrameHead = (PH264DATAPACKETHEADER)pBuffer;
    ZW_PRINTF("uFrameNo[%u] Type[%u--%u] Time[%lu--%lu] Chn[%u] Len[%u] Cnt[%u]\n", \
        pFrameHead->uFrameNo, pFrameHead->byFrameType, pFrameHead->byLoadType, \
        pFrameHead->u32RealTimeSec, pFrameHead->u32RealTimeUSec, pFrameHead->uReserved, \
        pFrameHead->uOverloadLen, pSysInfo->m_P2PSendDataFailCnt);

    if (access(P2P_SEND_DATA_FAIL_FLAG, F_OK) == 0)
    {
        return 0;
    }

    if (pSysInfo->m_NicStatus == eXRS_NETCARD_WIRED_1)
    {
        ZW_PRINTF("Is Wired, Not Deal... \n");
        return 0;
    }

    /* 上次回调通知发送失败小于15S，则计数++ */
    if ((pFrameHead->u32RealTimeSec - LastRealTimeSec) <= 15 || LastRealTimeSec == 0)
    {
        pSysInfo->m_P2PSendDataFailCnt++;
    }
    else
    {
        pSysInfo->m_P2PSendDataFailCnt = 0;
    }

    LastRealTimeSec = pFrameHead->u32RealTimeSec;

    if (pSysInfo->m_P2PSendDataFailCnt >= 5)
    {
        // 创建P2P发送失败标志文件
        int fd = open(P2P_SEND_DATA_FAIL_FLAG, O_RDWR | O_CREAT | O_TRUNC);
        if (fd > 0)
        {
            close(fd); fd = -1;
        }
    }

    return 0;
}

#if defined(__IS_SUPPORT_LCD_MANAGER__)
int HY_Enc_LCDRefreshFrame(char *pData, unsigned int DataSize)
{
    return Xrs_Lcd_RefreshFrame(pData, DataSize);
}

int HY_Enc_LCDSetBackLightLevel(int level)
{
    return Xrs_Lcd_SetBackLightLevel(level);
}

int HY_Enc_LCDCreateVdecChn(int nType)
{
    return Xrs_Lcd_CreateVdecChn(nType);
}

int HY_Enc_LCDSendStreamToChn(int nType, char *pData, unsigned int DataSize)
{
    return Xrs_Lcd_SendStreamToChn(nType, pData, DataSize);
}

int HY_Enc_LCDDestoryVdecChn(int nType)
{
    return Xrs_Lcd_DestoryVdecChn(nType);
}
#endif

#if defined(__DIGITAL_ZOOM_DRAG_SUPPORT__)
int DoDragAutoZoom(CChannelContext *pChContext, int* changeRatio)
{
    int ret = 0;
    VPSS_CENTER_ZOOM_ATTR_S zoomAttr = {0};
    double tmpRadio = 0.0f;
    static int nFirst = 1;
    static int nStopFirst = 1;
    struct timespec CurTime = {0};
    SystemInfo_t        *pSysInfo = NULL;
    unsigned long CurTimeMs = 0;

    //倍数变量
    double newRatio = DRAG_ZOOM_MIN_RATIO;
    double zoomNewRatio = DRAG_ZOOM_MIN_RATIO;
    double zoomCurRatio = DRAG_ZOOM_MIN_RATIO;

    int eOpreator = 0;
    int nNeedUpdateRadio = 0;

    // 变倍指令
    DRAG_ZOOM_CMD nZoomDragCmdEx = DRAG_CMD_ZOOM_STOP;

    HY_Res_GetSystemInfo(&pSysInfo);

    zoomNewRatio = pChContext->m_ZoomTrueRatio;
    nZoomDragCmdEx = pChContext->m_ZoomDragCmd;

    eOpreator = pChContext->m_ZoomOpreate;
    zoomCurRatio = pChContext->m_ZoomCurRatio;

#if defined(__IS_SUPPORT_LONG_SHORT_FOCUS_ZOOM__)
        int needSensorSwitch = 0;
        int targetSensor = pChContext->m_CurrentSensor;
#endif

    if (DRAG_CLIENT_RADIO == eOpreator || DRAG_CRUISE_RADIO == eOpreator || DRAG_TRACK_RADIO == eOpreator)
    {//变倍命令
        if (fabs(zoomCurRatio - zoomNewRatio) > 0.01)
        {
            nFirst = 1;
            nNeedUpdateRadio = 1;
            if (zoomCurRatio < zoomNewRatio)
            {
                newRatio = zoomCurRatio + DRAG_ZOOM_RATIO_STEP;
                if (newRatio > pChContext->m_ZoomUserMaxRatio)
                {
                    newRatio = pChContext->m_ZoomUserMaxRatio;
                }
            }
            else
            {
                newRatio = zoomCurRatio - DRAG_ZOOM_RATIO_STEP;
                if (newRatio < DRAG_ZOOM_MIN_RATIO)
                {
                    newRatio = DRAG_ZOOM_MIN_RATIO;
                }
            }
        #if defined(__IS_SUPPORT_LONG_SHORT_FOCUS_ZOOM__)
            Zoom_SwitchFocusTiming(zoomCurRatio, newRatio, &needSensorSwitch, &targetSensor);
        #endif
        }
        else
        {
            if (1 == nFirst)
            {//记录此次命令完成时间
                nFirst = 0;
                clock_gettime(CLOCK_MONOTONIC, &CurTime);
                pChContext->m_UserChangeRatioTime = CurTime.tv_sec * 1000 + CurTime.tv_nsec / 1000000;
            }
            else
            {
                clock_gettime(CLOCK_MONOTONIC, &CurTime);
                CurTimeMs = CurTime.tv_sec * 1000 + CurTime.tv_nsec / 1000000;
                if ((CurTimeMs - pChContext->m_UserChangeRatioTime) >= DRAG_CMD_WAIT_TIME)
                {
                    pChContext->m_ZoomOpreate = DRAG_NONE;
                    pChContext->m_UserChangeRatioTime = 0;
                    nFirst = 1;
                }
            }
        }

        pChContext->m_ZoomDragCmdStop = 0;
    }
    else if (DRAG_CLIENT_ZOOM == eOpreator)
    {//放大、缩小
        if (DRAG_CMD_ZOOMIN == nZoomDragCmdEx || DRAG_CMD_ZOOMOUT == nZoomDragCmdEx)
        {
            if (pChContext->m_ZoomDragCmdStop && pChContext->m_ZoomExecFlag)
            {//下发了停止变倍，且已经执行过
                if (1 == nStopFirst)
                {//记录此次命令完成时间
                    nStopFirst = 0;
                    clock_gettime(CLOCK_MONOTONIC, &CurTime);
                    pChContext->m_UserChangeRatioTime = CurTime.tv_sec * 1000 + CurTime.tv_nsec / 1000000;
                }
                else
                {
                    clock_gettime(CLOCK_MONOTONIC, &CurTime);
                    CurTimeMs = CurTime.tv_sec * 1000 + CurTime.tv_nsec / 1000000;
                    if ((CurTimeMs - pChContext->m_UserChangeRatioTime) >= DRAG_CMD_WAIT_TIME)
                    {
                        ZW_DEBUG("DIffTIme(%lu)\n",(CurTimeMs - pChContext->m_UserChangeRatioTime));
                        pChContext->m_ZoomDragCmdStop = 0;
                        pChContext->m_ZoomExecFlag = 0;
                        pChContext->m_ZoomOpreate = DRAG_NONE;
                        pChContext->m_UserChangeRatioTime = 0;
                        nStopFirst = 1;
                        pChContext->m_ClickFirst = 0;
                    }
                }
            }
            else
            {
                //计算倍数
                newRatio = Zoom_AnalyseDragCmdEx(nZoomDragCmdEx, zoomCurRatio, pChContext->m_ZoomUserMaxRatio);
                if (newRatio != zoomCurRatio)
                {
                    nFirst = 1;
                    nNeedUpdateRadio = 1;
                #if defined(__IS_SUPPORT_LONG_SHORT_FOCUS_ZOOM__)
                    Zoom_SwitchFocusTiming(zoomCurRatio, newRatio, &needSensorSwitch, &targetSensor);
                #endif
                }
                else
                {
                    if (1 == nFirst)
                    {//记录此次命令完成时间
                        nFirst = 0;
                        clock_gettime(CLOCK_MONOTONIC, &CurTime);
                        pChContext->m_UserChangeRatioTime = CurTime.tv_sec * 1000 + CurTime.tv_nsec / 1000000;
                    }
                    else
                    {
                        clock_gettime(CLOCK_MONOTONIC, &CurTime);
                        CurTimeMs = CurTime.tv_sec * 1000 + CurTime.tv_nsec / 1000000;
                        if ((CurTimeMs - pChContext->m_UserChangeRatioTime) >= DRAG_CMD_WAIT_TIME)
                        {
                            ZW_DEBUG("DIffTIme(%lu)\n",(CurTimeMs - pChContext->m_UserChangeRatioTime));
                            pChContext->m_ZoomDragCmdStop = 0;
                            pChContext->m_ZoomOpreate = DRAG_NONE;
                            pChContext->m_UserChangeRatioTime = 0;
                            nFirst = 1;
                            pChContext->m_ClickFirst = 0;
                        }
                    }
                }

                pChContext->m_ZoomExecFlag = 1;
                nStopFirst = 1;
            }

            if (nNeedUpdateRadio)
            {
                if (0 == pChContext->m_ClickFirst)
                {
                    //慢点，适配单击一次，变化一次。
                    usleep(150000);
                    pChContext->m_ClickFirst = 1;
                }
                else
                {
                    usleep(30000);
                }
            }
        }
    }
    else
    {
        pChContext->m_ZoomDragCmdStop = 0;
        nStopFirst = 1;
        nFirst = 1;
    }
    if (nNeedUpdateRadio)
    {
        #if defined(__IS_SUPPORT_LONG_SHORT_FOCUS_ZOOM__)
        ret = Zoom_SwicthFocusOpt(needSensorSwitch, targetSensor, changeRatio);
        if (0 != ret)
        {
            ZW_ERROR("SwicthFocusOpt failed! needSensorSwitch(%d) targetSensor(%d) HeightRatio(%d)\n",needSensorSwitch,targetSensor,*changeRatio);
            return -1;
        }
        #endif
        //更新倍数
        pChContext->m_ZoomCurRatio = newRatio;
        Zoom_RatioConverseAdapt(newRatio, &tmpRadio);

        //计算设置到基线的倍数
        tmpRadio = sqrt(tmpRadio);
        zoomAttr.Enable = 1;
        zoomAttr.WidthRatio = 1.0f / tmpRadio;
        zoomAttr.HeightRatio = 1.0f / tmpRadio;


        ret = Vpss_Grp_Set_CenterZoom(0,0,zoomAttr);
        if (0 != ret)
        {
            ZW_ERROR("Vpss_Grp_Set_CenterZoom failed! ret = %d  Enable(%d) WidthRatio(%f) HeightRatio(%f)\n",ret,zoomAttr.Enable,zoomAttr.WidthRatio,zoomAttr.HeightRatio);
        }

        pChContext->m_ZoomRealValue = newRatio;

        //osd叠加倍数处理
        XTRACE_MUTEX_LOCK(&pChContext->m_OSDLock);
        pChContext->m_DragUseOsd = 1;
        HY_Enc_SetZoomOSDBitMap(pChContext, MAX_STREAM_COUNT, 1);
        XTRACE_MUTEX_UNLOCK(&pChContext->m_OSDLock);
    }
    *changeRatio = nNeedUpdateRadio;

    return 0;
}

void *TrackerZoomWorkThread(void *lpParameter)
{
    ThreadInfo_t *pParent = (ThreadInfo_t *)lpParameter;

    unsigned long long Time1 = 0;
    unsigned long long Time2 = 0;
    unsigned long long OsdShowTime = 0;
    unsigned int nUseTime = 0;
    unsigned int nSleepTime = 0;
    int times = 0;
    int changeRatio = 0;
    bool bUpdateOsd = false;
    SystemInfo_t *pSysInfo = NULL;
    HY_Res_GetSystemInfo(&pSysInfo);
    CChannelContext *pChContext = NULL;
    HY_Enc_GetEncChInfo(&pChContext);
    if (pChContext == NULL || pSysInfo == NULL)
    {
        ZW_ERROR("Param Err!\n");
        return NULL;
    }

    if ((0.001f > pChContext->m_ZoomUserMaxRatio) && (-0.001f < pChContext->m_ZoomUserMaxRatio))
    {
        pChContext->m_ZoomUserMaxRatio = DRAG_ZOOM_DEFAULT_RATIO;
    }

    sleep(5);

    while (1)
    {
        times = (int)(pChContext->m_ZoomUserMaxRatio - DRAG_ZOOM_MIN_RATIO) / 0.1;
        nSleepTime = DRAG_ZOOM_MAX_TIME / times;

        if (pParent->m_Exit == 1)
        {
            break;
        }

        Time1 = get_ms();
        XTRACE_MUTEX_LOCK(&pChContext->m_DragZoomLock);
        if (pChContext->m_DragRunFlag)
        {
            DoDragAutoZoom(pChContext,&changeRatio);
        }
        XTRACE_MUTEX_UNLOCK(&pChContext->m_DragZoomLock);
        if(changeRatio)
        {
            OsdShowTime = get_ms();
            bUpdateOsd = true;
        }
        Time2 = get_ms();

        if(Time2 - OsdShowTime > 3000 && bUpdateOsd)
        {
            HY_Enc_SetZoomOSDBitMap(pChContext, MAX_STREAM_COUNT, FALSE);
            bUpdateOsd = false;
        }

        nUseTime = Time2 - Time1;
        if (nSleepTime > nUseTime)
        {
            nSleepTime = nSleepTime - nUseTime;
        }

        usleep(nSleepTime);
    }
    return NULL;
}

#endif


