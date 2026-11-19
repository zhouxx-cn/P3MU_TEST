#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#ifdef __MODULE_4G_SUPPORT__
#include <ctype.h>
#endif

#include "common_pub.h"
#include "Common.h"
#include "Init.h"
#include "xtrace.h"
#include "inifile.h"
#if defined(__IS_SUPPORT_LONG_SHORT_FOCUS_ZOOM__)
#include "Zoom.h"
#endif
#include "opencore-amrnb/interf_dec.h"

//获取人形跟踪结果(最大的一个)
extern int getHumanoidTrackingResult(int *pbHumanShapeState, int *pOx, int *pOy, int *pWidth, int *pHeight);
//获取所有的人形跟踪结果(<=3)
extern int getAllHumanoidTrackingResult(int *number, int *pOx, int *pOy, int *pWidth, int *pHeight);

extern CmosFrequencyType_t AEAntiFlicker;

typedef struct gpio_data_s {		unsigned char dev_address;
unsigned char sub_address;
unsigned char data;}
gpio_data;

typedef struct gpio_datan_s {

    unsigned char dev_address;
    unsigned char num;
    unsigned char data[16];
}gpio_datan;

#ifdef __MODULE_4G_SUPPORT__
extern int PI31415926(char* szActiveCode, unsigned int dwSN, unsigned int dwBoardID, unsigned int* dwCapability);

/*@breif GetIntSerialNum
 *@param
 *@return
 */
static unsigned int GetIntSerialNum(const unsigned char *szSerialNum)
{
    if (NULL == szSerialNum)
    {
        return -1;
    }

    int i = 0;
    unsigned int result = 0;
    char buf[32] = {0};

    /* 以字母(非A~F)开头的字符串 */
    if ((0 == isdigit(szSerialNum[0]))
    && ((szSerialNum[0]>'F') || (szSerialNum[0]>'f')))
    {
        int j = 0;
        int len = strlen((const char *)szSerialNum);
        for (i=len-8; i<=len; i++, j++)
        {
            buf[j] = szSerialNum[i];
        }
    }
    else
    {
        for (i=0; i<8; i++)
        {
            buf[i] = szSerialNum[i];
        }
    }

    /* 转化为十六进制 */
    sscanf(buf, "%x", &result);

    return result;
}


/*@breif ParseActiveCode 从License配置分区里面获取激活码并解析
 *@param
 *@return
 */
static void ParseActiveCode(void)
{
    unsigned int dwCapability = 0;
    int ActiveStatus = 0;
    char nActiveCode[64] = {0};
    SystemInfo_t *pSysInfo = NULL;
    HY_Res_GetSystemInfo(&pSysInfo);

    //获取ActiveCode激活码
    read_profile_string("AuthorizationInfo", "ActiveCode", nActiveCode,sizeof(nActiveCode),"",AUTHORIZATION_CFG);

    //如果获取的激活码正确
    if(strlen(nActiveCode) > 6)
    {
        ActiveStatus = PI31415926(nActiveCode, GetIntSerialNum(pSysInfo->m_szSerialNum), pSysInfo->m_BoardInfo, &dwCapability);
        if (0 == ActiveStatus)
        {
            pSysInfo->m_nFuncActiveStatusCode |= dwCapability;
        }
    }
}

#endif

/* 根据加密芯片信息设置相应参数 */
int Zview_BoardInfo_Init(SystemInfo_t *pSysInfo, char *pInfo)
{
    if (NULL == pSysInfo)
    {
        ZW_ERROR("pSysInfo CAN_NOT be NULL\n");
        return -1;
    }

    if (NULL == pInfo)
    {
        ZW_ERROR("pInfo CAN_NOT be NULL\n");
        return -1;
    }

    int ActionCodeOk = 0;
    stuShmAuthInfo auth_info;
    memset(&auth_info, 0, sizeof(stuShmAuthInfo));

    if (Authorize_GetAuthDevInfo(&auth_info))
    {
        pSysInfo->m_bAuthEncryptOK = FALSE;
    }
    else
    {
        if (AUTH_SUCCESS != auth_info.dwAuthVerifyErrno)
        {
            ZW_DEBUG("Warning: Auth fail! errno: %lu\n", auth_info.dwAuthVerifyErrno);
            pSysInfo->m_bAuthEncryptOK = FALSE;
        }
        else
        {
            pSysInfo->m_bAuthEncryptOK = TRUE;
            pSysInfo->m_BoardInfo = auth_info.dwBoardID;
            strncpy(pSysInfo->m_szMacAddr, (char *)auth_info.szMac, MAC_ADDR_LEN);
            strncpy(pSysInfo->m_szAuthLicense, (char *)auth_info.szAuthCode, LICENSE_CODE_MAX_LEN);
            strncpy(pSysInfo->m_sTutkUID, (char *)auth_info.struCloudInfo.szDid,MAX_DID_NUM_LEN);
            if ((0xffffffff != auth_info.dwFuncion) && (0x0 != auth_info.dwFuncion))
            {
                /* 授权码里面AF激活码判断 */
                if (0 == (auth_info.dwFuncion & CAPABILITY_AF))    //不支持
                {
                    ActionCodeOk &= (~CAPABILITY_AF);
                    ZW_DEBUG("[Auth License] Don't support AF! ActionCodeOk = %d\n", ActionCodeOk);
                }
                else
                {
                    /* 当加密授权解密成功，则表明获取到了AF激活码 */
                    ActionCodeOk |= (CAPABILITY_AF);
                    ZW_DEBUG("[Auth License] AF AtionCode is right! ActionCodeOk = %d\n", ActionCodeOk);
                }

                /* 授权码里面DL激活码判断 */
                if (0 == (auth_info.dwFuncion & CAPABILITY_DL))    //不支持
                {
                    ActionCodeOk &= (~CAPABILITY_DL);
                    ZW_DEBUG("[Auth License] Don't support DL! ActionCodeOk = %d\n", ActionCodeOk);
                }
                else
                {
                    /* 当加密授权解密成功，则表明获取到了DL激活码 */
                    ActionCodeOk |= (CAPABILITY_DL);
                    ZW_DEBUG("[Auth License] DL AtionCode is right! ActionCodeOk = %d\n", ActionCodeOk);
                }

            #ifdef __MODULE_4G_SUPPORT__
                int bSupport4G = 0;
                ZWSys_Get4GSupportV2(&bSupport4G);
                if (bSupport4G)
                {
                        /* 授权码里面运营模式激活码判断 */
                    if (0 == (auth_info.dwFuncion & CAPABILITY_EXTERNSIMCARD)) //不支持
                    {
                        ActionCodeOk &= (~CAPABILITY_EXTERNSIMCARD);
                        ZW_DEBUG(
                            "[Auth License] Don't support EXTERNSIMCARD! ActionCodeOk = %d\n",
                            ActionCodeOk);
                    }
                    else
                    {
                        /* 当加密授权解密成功，则表明获取到了DL激活码 */
                        ActionCodeOk |= (CAPABILITY_EXTERNSIMCARD);
                        ZW_DEBUG("[Auth License] EXTERNSIMCARD AtionCode is right! "
                                 "ActionCodeOk = %d\n",
                            ActionCodeOk);
                    }
                }

           #endif

                pSysInfo->m_nFuncActiveStatusCode = ActionCodeOk;
            }
        }
    }

    if (FALSE == pSysInfo->m_bAuthEncryptOK)
    {
        pSysInfo->m_BoardInfo = GetDefaultBoardID();
        ZW_ERROR("Not authorize! default BoardId: %#x\n", pSysInfo->m_BoardInfo);
    }

#ifdef __MODULE_4G_SUPPORT__
    int bSupport4G = 0;
    ZWSys_Get4GSupportV2(&bSupport4G);
    if (bSupport4G)
    {
        //解析激活码
        ParseActiveCode();
    }
#endif

#ifdef GET_TUTK_UID
    if (TRUE == pSysInfo->m_bAuthEncryptOK)
    {
        strncpy(pSysInfo->pTUTKUID, (char *)auth_info.struCloudInfo.szDid, MAX_DID_NUM_LEN);
    }
    ZW_DEBUG("Get uid is %s\n", pSysInfo->pTUTKUID);
#endif

    ZW_DEBUG("get board id = 0x%x \n", pSysInfo->m_BoardInfo);

    if (TRUE == pSysInfo->m_bAuthEncryptOK)
    {
        strncpy((char *)pSysInfo->m_szSerialNum, (char *)auth_info.szSerialNum, sizeof(auth_info.szSerialNum));
    }
    else
    {
        ShellCmdOut(GET_RANDOM_SERIAL_NUM, (char *)pSysInfo->m_szSerialNum, sizeof(auth_info.szSerialNum));
        // 用shellout命令出来的有空格，去除最后一位空格
        pSysInfo->m_szSerialNum[strlen((char *)pSysInfo->m_szSerialNum) - 1] = '\0';
    }

    pSysInfo->m_TotalAudioEncChannel = 1;
    pSysInfo->m_TotalAudioDecChannel = 1;
    pSysInfo->m_TotalTalkingChannel = 1;
    pSysInfo->m_Frequ_HZ = CMOS_FREQUENCY_50HZ;
    pSysInfo->m_bAudioInited = FALSE;

    /* 从配置文件里面获取相关设备能力 */
    pSysInfo->m_bSupportWifi = read_profile_int("WIFI", "bSupportWIFI", 1, CUSTOM_CONFIG_EX);

    /* 当前版本是否支持音频报警 */
    pSysInfo->m_bSupportAudioAlarm = read_profile_int("Speech", "bSupportAudioAlarm", 1, CUSTOM_CONFIG_EX);

    /* 当前版本是否支持语音播报 */
    pSysInfo->m_bSupportAudioBroadcast = read_profile_int("Speech", "bSupportAudioBroadcast", 1, CUSTOM_CONFIG_EX);

    /* 获取当前定制的灯板类型 */
    pSysInfo->m_LightBoardTypeID = read_profile_int("SystemFunction", "LightBoardType", NO_LDR_2PIN_LIGHT_BOARD, CUSTOM_CONFIG_FILE);

    /* 是否支持sensor交换 */
    pSysInfo->m_bSptSwapLRViewChannel = read_profile_int("FunctionSupport", "IsSupportSwapLRViewChannel", 0, CUSTOM_ABILITY_INI_FILE);

    // 镜头安装方向，由低到高每个字节，代表镜头安装时是否反向，1为反向
    pSysInfo->m_LensOrientation = (unsigned int)read_profile_int("SystemFunction", "LensOrientation", 0, CUSTOM_CONFIG_FILE);

    //网卡状态
    pSysInfo->m_NicStatus = eXRS_NETCARD_WIRED_Unknown;

    pSysInfo->m_SpeechMode = 2;//对讲的音频格式为PCM
    pSysInfo->m_SpeechAudioFrameSize = 320;

    //是否支持低帧率模式
    pSysInfo->m_bSupportCloudIPCSlowFrame = read_profile_int("FunctionSupport", "IsSupportCloudIPCSlowFrame", 0, CUSTOM_ABILITY_INI_FILE);

    //是否支持Zoom变倍
    pSysInfo->m_bSupportZoom = read_profile_int("FunctionSupport", "IsSupportTrackZoom", 0, CUSTOM_ABILITY_INI_FILE);

#ifdef __ONE_CLICK_CALL__
    //是否支持一键呼叫
    pSysInfo->m_bSupportKeyToCall = read_profile_int("FunctionSupport", "IsSupportKeyToCall", 1, CUSTOM_ABILITY_INI_FILE);
#endif

#ifdef __MODULE_4G_SUPPORT__
    /* 是否支持APP绑定下播报语音 */
    pSysInfo->m_BroadCastEnable = read_profile_int("Device", "BroadCastEnable", 0, CUSTOM_CONFIG_FILE);

    pSysInfo->m_bSupportWifi = 0;
    pSysInfo->m_4Gmoduletype = UNKNOW_MODULE;
    pSysInfo->m_4GTelecomOperatorType = eUnknowOperator;
#endif

    //是否支持报警输出
    pSysInfo->m_bSupportAlarmOut = read_profile_int("FunctionSupport", "IsSupportAlarmOut", 1, CUSTOM_ABILITY_INI_FILE);

    //是否支持云台
    pSysInfo->m_bSupportMotor = read_profile_int("FunctionSupport", "IsSupportMotor", 1, CUSTOM_ABILITY_INI_FILE);

    ZW_DEBUG("IP Camera is support WIFI? %s\n",  pSysInfo->m_bSupportWifi?"yes":"no");
    ZW_DEBUG("IP Camera is support SDCard? %s\n", pSysInfo->m_bSupportSDCard?"yes":"no");
    ZW_DEBUG("IP Camera is support AudioAlarm? %s\n", pSysInfo->m_bSupportAudioAlarm?"yes":"no");
    ZW_DEBUG("IP Camera is support AudioBroadcast? %s\n", pSysInfo->m_bSupportAudioBroadcast?"yes":"no");
    ZW_DEBUG("IP Camera is support IsSupportCloudIPCSlowFrame? %s\n", pSysInfo->m_bSupportCloudIPCSlowFrame?"yes":"no");
    ZW_DEBUG("IP Camera LightBoardTypeID is %d\n", pSysInfo->m_LightBoardTypeID);
    ZW_DEBUG("IP Camera is support AlarmOut? %s\n", pSysInfo->m_bSupportAlarmOut?"yes":"no");
#ifdef __ONE_CLICK_CALL__
    ZW_DEBUG("IP Camera is support IsSupportKeyToCall? %s\n", pSysInfo->m_bSupportKeyToCall?"yes":"no");
#endif

    int MaxSns = HY_Res_GetMaxSensorCnt();
    for (int i = 0; i < MaxSns; i++)
    {
        pSysInfo->m_VIChnWidth[i] = SENSOR_REAL_WIDTH;
        pSysInfo->m_VIChnHeight[i] = SENSOR_REAL_HEIGHT;
        pSysInfo->m_VIChnFrmRate[i] = MAX_FPS;
    }

    return 0;
}

/* 全局资源初始化 */
int Zview_Res_Init(char *pInfo)
{
    if (NULL == pInfo)
    {
        ZW_ERROR("pInfo CAN-NOT be NULL!\n");
        return -1;
    }

    SystemInfo_t    *pSysInfo = NULL;
    CChannelContext *pChCtx = NULL;
    CTalkingChannel_t *pTalkingCh = NULL;			//语音对讲通道

    /* 系统信息 */
    HY_Res_GetSystemInfo(&pSysInfo);
    memset((char *)pSysInfo, 0, sizeof(SystemInfo_t));

    Zview_BoardInfo_Init(pSysInfo, pInfo);

    HY_Res_WR_Lock(&pSysInfo->m_SysTemConfigLock);
    XTRACE_MUTEX_INIT(&pSysInfo->m_SysOSDLock, NULL);
    XTRACE_MUTEX_INIT(&pSysInfo->m_WriteFileLock, NULL);
    XTRACE_MUTEX_INIT(&pSysInfo->m_StreamCallbackLock, NULL);
    XTRACE_MUTEX_INIT(&pSysInfo->m_SecretResetLock, NULL);
    XTRACE_MUTEX_INIT(&pSysInfo->m_AudioInLock, NULL);
    XTRACE_MUTEX_INIT(&pSysInfo->m_muAudioDetectCallbackLock, NULL);

    HY_Res_WR_UnLock(&pSysInfo->m_SysTemConfigLock);

    /* 编码通道参数信息 */
    HY_Enc_GetEncChInfo(&pChCtx);
    memset((char *)pChCtx, 0, sizeof(CChannelContext));
    XTRACE_MUTEX_INIT(&pChCtx->m_IrcutLock, NULL);
    XTRACE_MUTEX_INIT(&pChCtx->m_DayNightLock, NULL);
    XTRACE_MUTEX_INIT(&pChCtx->m_SensorLock, NULL);
    XTRACE_MUTEX_INIT(&pChCtx->m_CheckLumLock, NULL);
    XTRACE_MUTEX_INIT(&pChCtx->m_VencLock, NULL);
    XTRACE_MUTEX_INIT(&pChCtx->m_EncLock, NULL);
    XTRACE_MUTEX_INIT(&pChCtx->m_MDLock, NULL);
    XTRACE_MUTEX_INIT(&pChCtx->m_HumanShapeLock, NULL);
    XTRACE_MUTEX_INIT(&pChCtx->m_IVPLock, NULL);
    XTRACE_MUTEX_INIT(&pChCtx->m_OSDLock, NULL);
    XTRACE_MUTEX_INIT(&pChCtx->m_InitVPSSLock, NULL);
    XTRACE_MUTEX_INIT(&pChCtx->m_AudioLock, NULL);
    XTRACE_MUTEX_INIT(&pChCtx->m_SpeechLock, NULL);
    XTRACE_MUTEX_INIT(&pChCtx->m_SpeechCustomAMRFileLock, NULL);
    XTRACE_MUTEX_INIT(&pChCtx->m_SlowFrameLock, NULL);
#ifdef __DIGITAL_ZOOM_DRAG_SUPPORT__
    XTRACE_MUTEX_INIT(&pChCtx->m_ZoomValueLock, NULL);
    XTRACE_MUTEX_INIT(&pChCtx->m_DragZoomLock, NULL);
#endif
    /* 语音对讲通道 */
    HY_Enc_GetTalkingChInfo(&pTalkingCh);
    memset((char *)pTalkingCh, 0, sizeof(CTalkingChannel_t));
    XTRACE_MUTEX_INIT(&pTalkingCh->m_Lock, NULL);

    if(1 == pSysInfo->m_bSupportCloudIPCSlowFrame)
    {
        pChCtx->m_iSlowFrameMode = SLOWFRAME_SCENE_FORCESLOW;
        pChCtx->m_iSlowFrameResetFlag = 0;
    }
    else
    {
        pChCtx->m_iSlowFrameMode = SLOWFRAME_SCENE_NONE;
        pChCtx->m_iSlowFrameResetFlag = -1;
    }
#ifdef __ONE_CLICK_CALL__
    // 支持一键呼叫才能支持备用复位按键
    if (pSysInfo->m_bSupportKeyToCall == 0)
    {
        pSysInfo->m_bSupportAlternateResetButton = 0;
    }
    else
    {
        pSysInfo->m_bSupportAlternateResetButton = read_profile_int("FunctionSupport", "IsSupportAlternateResetButton", 0, CUSTOM_ABILITY_INI_FILE);
    }
#endif

    pChCtx->m_DayNightStutas = ISDAY; //默认天亮

    return 0;
}

/* 全局资源反初始化 */
int Zview_Res_DeInit()
{
    SystemInfo_t 			*pSysInfo;
    CChannelContext			*pEncChannelContext;
    CTalkingChannel_t		*pTalkingCh;			//语音对讲通道

    /* 系统信息 */
    HY_Res_GetSystemInfo(&pSysInfo);

    HIEASY_FREE(pSysInfo->pDormancyIFrameBuffer);
    HIEASY_FREE(pSysInfo->pDormancyPFrameBuffer);
    pSysInfo->m_DormancyIFrameBufferSize = 0;
    pSysInfo->m_DormancyPFrameBufferSize = 0;
    XTRACE_MUTEX_DESTORY(&pSysInfo->m_SysOSDLock);
    XTRACE_MUTEX_DESTORY(&pSysInfo->m_WriteFileLock);
    XTRACE_MUTEX_DESTORY(&pSysInfo->m_SecretResetLock);
    XTRACE_MUTEX_DESTORY(&pSysInfo->m_AudioInLock);
    XTRACE_MUTEX_DESTORY(&pSysInfo->m_StreamCallbackLock);
    XTRACE_MUTEX_DESTORY(&pSysInfo->m_muAudioDetectCallbackLock);
    HY_Res_RWLock_destory(&pSysInfo->m_SysTemConfigLock);
    memset((char *)pSysInfo, 0, sizeof(SystemInfo_t));

    /* 编码通道参数信息 */
    HY_Enc_GetEncChInfo(&pEncChannelContext);

    HIEASY_FREE(pEncChannelContext->pTextOSDBit_Src);		//释放软件设置的原始文本OSD点阵(非实时D1需要)

    XTRACE_MUTEX_DESTORY(&pEncChannelContext->m_IrcutLock);
    XTRACE_MUTEX_DESTORY(&pEncChannelContext->m_DayNightLock);
    XTRACE_MUTEX_DESTORY(&pEncChannelContext->m_SensorLock);
    XTRACE_MUTEX_DESTORY(&pEncChannelContext->m_CheckLumLock);
    XTRACE_MUTEX_DESTORY(&pEncChannelContext->m_VencLock);
    XTRACE_MUTEX_DESTORY(&pEncChannelContext->m_EncLock);
    XTRACE_MUTEX_DESTORY(&pEncChannelContext->m_HumanShapeLock);
    XTRACE_MUTEX_DESTORY(&pEncChannelContext->m_IVPLock);
    XTRACE_MUTEX_DESTORY(&pEncChannelContext->m_MDLock);
    XTRACE_MUTEX_DESTORY(&pEncChannelContext->m_OSDLock);
    XTRACE_MUTEX_DESTORY(&pEncChannelContext->m_InitVPSSLock);
    XTRACE_MUTEX_DESTORY(&pEncChannelContext->m_AudioLock);
    XTRACE_MUTEX_DESTORY(&pEncChannelContext->m_SpeechLock);
    XTRACE_MUTEX_DESTORY(&pEncChannelContext->m_SpeechCustomAMRFileLock);
    XTRACE_MUTEX_DESTORY(&pEncChannelContext->m_SlowFrameLock);
#ifdef __DIGITAL_ZOOM_DRAG_SUPPORT__
    XTRACE_MUTEX_DESTORY(&pEncChannelContext->m_ZoomValueLock);
    XTRACE_MUTEX_DESTORY(&pEncChannelContext->m_DragZoomLock);
#endif

    memset((char *)pEncChannelContext, 0, sizeof(CChannelContext));

    /* 语音对讲通道 */
    HY_Enc_GetTalkingChInfo(&pTalkingCh);
    HIEASY_FREE(pTalkingCh->m_TalkingIn.pBuf);
    HIEASY_FREE(pTalkingCh->m_TalkingOut.pBuf);
    XTRACE_MUTEX_DESTORY(&pTalkingCh->m_Lock);
    memset((char *)pTalkingCh, 0, sizeof(CTalkingChannel_t));

    return 0;
}

int Zview_SDK_Init()
{
    SystemInfo_t *pSysInfo = NULL;
    HY_Res_GetSystemInfo(&pSysInfo);

#ifndef __USE_CIRCLEQUEUE__
    // 分配编码的buffer 大小
    pSysInfo->m_VideoBufferSize = ENC_MAIN_STREAM_BUF_SIZE;            // > 1280*720  编码之后一帧图像的数据缓存

    pSysInfo->pVideoBuffer = (char *)malloc(pSysInfo->m_VideoBufferSize); // 主要用于接收从片码流
    if(NULL == pSysInfo->pVideoBuffer)
    {
        pSysInfo->m_VideoBufferSize = ENC_MAIN_STREAM_BUF_SIZE;
        pSysInfo->pVideoBuffer = (char *)malloc(pSysInfo->m_VideoBufferSize);
        DBG_ASSERT(pSysInfo->pVideoBuffer != NULL, -1,  "pSysInfo->pVideoBuffer==NULL");
    }
#endif
    //jpeg编码的图片缓冲区
    pSysInfo->m_MJPEGBufferSize = MJPEG_FRAME_BUFFER_SIZE;			   // > 1280*720  编码之后一帧图像的数据缓存
    pSysInfo->pMJPEGBuffer = (char *)malloc(pSysInfo->m_MJPEGBufferSize); // 主要用于接收从片码流
    if(NULL == pSysInfo->pMJPEGBuffer)
    {
        DBG_ASSERT(pSysInfo->pMJPEGBuffer != NULL, -1,  "pSysInfo->pMJPEGBuffer==NULL");
    }

    pSysInfo->m_AudioFrameSize = 320;
    pSysInfo->m_TalkingMode = eCodecBegin;
    pSysInfo->m_AudioInMode = MIC_IN;
    pSysInfo->m_SpeechTalkingMode = 0;
    pSysInfo->m_pAmrToPcmDecoder = NULL;
    /* 初始化amr解码器 */
    pSysInfo->m_pAmrToPcmDecoder = Decoder_Interface_init();
    if(pSysInfo->m_pAmrToPcmDecoder == NULL)
    {
        ZW_ERROR("Zview_Decoder_Interface_init() error!\n");
        return -1;
    }

    pSysInfo->pDormancyIFrameBuffer = NULL;
    pSysInfo->pDormancyPFrameBuffer = NULL;
    pSysInfo->m_DormancyIFrameBufferSize = 0;
    pSysInfo->m_DormancyPFrameBufferSize = 0;
    /* 抽象的资源管理 */
    HY_Enc_InitEncCap();

    /* 编码通道信息初始化(H.265修改) */
    if (0 != Zview_EncChInfo_Init(pSysInfo))
    {
        ZW_ERROR("Zview_EncChInfo_Init Fail\n");
        return -1;
    }

    /* 语音对讲通道初始化 */
    if (0 != Zview_TalkingChInfo_Init(pSysInfo))
    {
        ZW_ERROR("Zview_TalkingChInfo_Init Fail\n");
        return -1;
    }

    if (0 != HY_HalModInit())
    {
        ZW_ERROR("HY_HalModInit Fail\n");
        return -1;
    }

#ifdef __MODULE_4G_SUPPORT__
    int bSupport4G = 0;
    ZWSys_Get4GSupport(&bSupport4G);
    if (bSupport4G)
    {
        MP4Sys_4GModuleReset();
    }
#endif

    //报警输出默认拉低
    Hi_MPP_SetAlarmOut(0, 0);

    //音频小板禁用
    Hi_MPP_SetSpeakerEn(0);

    // 灯板模块初始化
    HY_LightCtlInit();

    HY_GetDevChipID((SINT8 *)pSysInfo->m_sUniqueID, UNIQUEID_BUF_LEN);

    /*SDK 层的编码通道以及系统的初始化*/
    if (0 != HY_Enc_PlatformInit())
    {
        ZW_ERROR("HY_Enc_PlatformInit Fail\n");
        return -1;
    }

    return 0;
}

int Zview_SDK_DeInit()
{
    SystemInfo_t		*pSysInfo = NULL;
    HY_Res_GetSystemInfo(&pSysInfo);

    /*编解码系统逆初始化*/
    HY_Enc_PlatformDeInit();

    /* 语音对讲通道逆初始化 */
    Zview_TalkingChInfo_DeInit();

    /* 编码通道信息逆初始化 */
    Zview_EncChInfo_DeInit();

    /* 关闭amr解码器 */
    Decoder_Interface_exit(pSysInfo->m_pAmrToPcmDecoder);

    /* 抽象资源逆初始化 */
    HY_Enc_DeInitEncCap();

    /*AISDK放到最后反初始化，防止卡主导致编码一直申请buffer引起OOM*/
    HY_Enc_UnInitAISDK();

#ifdef __USE_CIRCLEQUEUE__
    HIEASY_FREE(pSysInfo->pVideoBuffer);
#endif
    HIEASY_FREE(pSysInfo->pDormancyIFrameBuffer);
    HIEASY_FREE(pSysInfo->pDormancyPFrameBuffer);
    pSysInfo->m_DormancyIFrameBufferSize = 0;
    pSysInfo->m_DormancyPFrameBufferSize = 0;

    return 0;
}

/* 编码通道信息初始化 */
int Zview_EncChInfo_Init(SystemInfo_t *pSysInfo)
{
    SINT32 play_index = 0;
    CChannelContext *pChnCtx = NULL;

    if (NULL == pSysInfo)
    {
        ZW_ERROR("pSysInfo CAN NOT be NULL\n");
        return -1;
    }

    HY_Enc_GetEncChInfo(&pChnCtx);

    /* 音频资源信息 */
    pChnCtx->m_AudioChNum 				 = 1;
    pChnCtx->m_AudioStreamInfo.m_PortNum = 0;
    pChnCtx->m_AudioStreamInfo.BufSize   = ENC_AUDIOSTREAM_BUFSIZE;
    pChnCtx->m_AudioStreamInfo.pBuf		 = (char *)malloc(pChnCtx->m_AudioStreamInfo.BufSize);
    if (NULL == pChnCtx->m_AudioStreamInfo.pBuf)
    {
        ZW_ERROR("AudioStream Buf malloc fail !!!!!\n");
        return -1;
    }
    //pChnCtx->m_AudioStreamInfo.BufSize  = 0;
    pChnCtx->m_AudioStreamInfo.WritePos = 0;

    /* 预览相关资源 */
    pChnCtx->Bright		= 50;
    pChnCtx->Contrast	= 50;
    pChnCtx->Saturation	= 50;
    pChnCtx->Hue		= 50;

    pChnCtx->m_CaptureType = SameAsAuxStream;

    /* 编码相关资源 */
    pChnCtx->m_PictureRes[FIRST_STREAM].m_PicWidth = MAIN_STREAM_WIDTH_MAX;
    pChnCtx->m_PictureRes[FIRST_STREAM].m_PicHeight = MAIN_STREAM_HEIGHT_MAX;
    pChnCtx->m_PictureRes[SECOND_STREAM].m_PicWidth =  AUX_STREAM_WIDTH_MAX;
    pChnCtx->m_PictureRes[SECOND_STREAM].m_PicHeight = AUX_STREAM_HEIGHT_MAX;

    if (SameAsAuxStream == pChnCtx->m_CaptureType)
    {
        pChnCtx->m_PictureRes[FOURTH_STREAM].m_PicWidth = pChnCtx->m_PictureRes[SECOND_STREAM].m_PicWidth;
        pChnCtx->m_PictureRes[FOURTH_STREAM].m_PicHeight = pChnCtx->m_PictureRes[SECOND_STREAM].m_PicHeight;
    }
    else if (SameAsMainStream == pChnCtx->m_CaptureType)
    {
        pChnCtx->m_PictureRes[FOURTH_STREAM].m_PicWidth = pChnCtx->m_PictureRes[FIRST_STREAM].m_PicWidth;
        pChnCtx->m_PictureRes[FOURTH_STREAM].m_PicHeight = pChnCtx->m_PictureRes[FIRST_STREAM].m_PicHeight;
    }

    pChnCtx->m_bSupportFourthStream = 1;

    pChnCtx->m_EncAttr.PictureRes.m_PicWidth = pChnCtx->m_PictureRes[FIRST_STREAM].m_PicWidth;
    pChnCtx->m_EncAttr.PictureRes.m_PicHeight = pChnCtx->m_PictureRes[FIRST_STREAM].m_PicHeight;
    pChnCtx->m_EncAttr.BitRate = MAIN_STREAM_VBR_MAX_BITRATE;
    pChnCtx->m_EncAttr.FrameRate = MAX_FPS;
    pChnCtx->m_EncAttr.BitRateCtlMode = ENCODE_BITRATE_CTRLMODE_AVBR;
    pChnCtx->m_EncAttr.PicQualityLever = 0;
    pChnCtx->m_EncAttr.EncType = LtH264Video;
    pChnCtx->m_EncAttr.KeyFrameInterval = pChnCtx->m_EncAttr.FrameRate*4;

    pChnCtx->m_EncAttrAux.PictureRes.m_PicWidth = pChnCtx->m_PictureRes[SECOND_STREAM].m_PicWidth;
    pChnCtx->m_EncAttrAux.PictureRes.m_PicHeight = pChnCtx->m_PictureRes[SECOND_STREAM].m_PicHeight;
    pChnCtx->m_EncAttrAux.BitRate = AUX_STREAM_VBR_MAX_BITRATE;
    pChnCtx->m_EncAttrAux.FrameRate = MAX_FPS;

    pChnCtx->m_EncAttrAux.BitRateCtlMode = ENCODE_BITRATE_CTRLMODE_AVBR;
    pChnCtx->m_EncAttrAux.PicQualityLever = 0;
    pChnCtx->m_EncAttrAux.EncType = LtH264Video;
    pChnCtx->m_EncAttrAux.KeyFrameInterval = pChnCtx->m_EncAttrAux.FrameRate*4;

    if (pChnCtx->m_bSupportFourthStream)
    {
        pChnCtx->m_EncAttrFourth.PictureRes.m_PicWidth = pChnCtx->m_PictureRes[FOURTH_STREAM].m_PicWidth;
        pChnCtx->m_EncAttrFourth.PictureRes.m_PicHeight = pChnCtx->m_PictureRes[FOURTH_STREAM].m_PicHeight;
        pChnCtx->m_EncAttrFourth.BitRate = 200; // 码率不够会导致画面异常
        pChnCtx->m_EncAttrFourth.FrameRate = FOURTH_STREAM_FPS;
        pChnCtx->m_EncAttrFourth.BitRateCtlMode = ENCODE_BITRATE_CTRLMODE_VBR;
        if(SameAsAuxStream == pChnCtx->m_CaptureType)
        {
            pChnCtx->m_EncAttrFourth.PicQualityLever = AUX_CAPTURE_QUALITY_MID;
        }
        else if(SameAsMainStream == pChnCtx->m_CaptureType)
        {
            pChnCtx->m_EncAttrFourth.PicQualityLever = MAIN_CAPTURE_QUALITY_MID;
        }

        pChnCtx->m_LastCaptureQuality = pChnCtx->m_EncAttrFourth.PicQualityLever;
        pChnCtx->m_EncAttrFourth.EncType = LtMjpegVideo;
        pChnCtx->m_EncAttrFourth.KeyFrameInterval = pChnCtx->m_EncAttrFourth.FrameRate*4;
    }
    else
    {
        memset(&pChnCtx->m_EncAttrFourth, 0, sizeof(CODEC_PARA_S));
    }

    pChnCtx->m_LastCaptureQuality = pChnCtx->m_EncAttrFourth.PicQualityLever;

    pChnCtx->m_streamType = STREAM_TYPE_AVSYNC;  // 支持音视频，默认为混合流

    pChnCtx->m_EncType       = pChnCtx->m_EncAttr.EncType;
    pChnCtx->m_AuxEncType    = LtH265Video;
    pChnCtx->m_FourthEncType = LtMjpegVideo;

    /* 运动检测相关 */
    pChnCtx->MotionFps			= 4;		//MD间隔
    pChnCtx->MotionGrade		= 1;		//分析SAD时的灵敏度（0 - 高，1 - 中， 2 - 低）
    pChnCtx->m_bStartHuman		= 0;

    ZWSDK_PresetConfig pPresetConfig = {0};
    ZWSys_GetPresetConfig(&pPresetConfig);
    if (pPresetConfig.bLangFlag)
    {
        play_index = pPresetConfig.eLang;
    }
    else
    {
        ZW_ERROR("speech language not set, set to 0\n");
        play_index = 0;
    }

    if((play_index < eSpeechLanguage_Chinese) || (play_index > eSpeechLanguage_Num))
    {
        ZW_ERROR("Custom speech language type %d not in 0~2", play_index);
        play_index = eSpeechLanguage_Chinese;
    }

    HY_Enc_SetAlarmPlayCfg((eSpeechLanguage)play_index);

    /* 获取当前定制的灯板电平 */
    pChnCtx->m_IrPin3Default = read_profile_int("Device", "DLopenStatus", 0, CUSTOM_CONFIG_FILE);

    pChnCtx->m_AlarmOutDefault = read_profile_int("Device", "AlarmOutDefaultStatus", 0, CUSTOM_CONFIG_FILE);

    pChnCtx->m_whiteLightFlickeredEnable = read_profile_int("FunctionSupport", "IsSupportWhiteLightFlickered", 0, CUSTOM_ABILITY_INI_FILE);
#if defined(__IS_SUPPORT_LONG_SHORT_FOCUS_ZOOM__)
    pChnCtx->m_ZoomLens = read_profile_int("ZoomLensCfg", "ZoomLensCombination", 0, CUSTOM_CONFIG_FILE);
#endif
    pChnCtx->m_nAssistantControlTest = 0;

    /* ISP相关初始化 */
    pChnCtx->DayNightMode = IRCUT_PASSIVITY;
    pChnCtx->m_IrcutDeriction = 0;
    pChnCtx->m_IrcutState = 1;//切换成白天
    pChnCtx->m_IrcutStateHis = pChnCtx->m_IrcutState;
    pChnCtx->m_DayNightSwitchTime = 5;//亮度检测默认5秒
    pChnCtx->m_DayNightInitiativeSwitchTime = 5;
    pChnCtx->m_IrcutSwitchFlag = 0;

    pChnCtx->m_colorNightLastTime = 30 * 1000 * 1000;

    pChnCtx->m_InfraredThreshold = 30;
    pChnCtx->m_WhiteLightThreshold = 30;

    pChnCtx->ColorNightMode = WORKMODE_AUTO;

    pChnCtx->VarWhiteMode = WORKMODE_AUTO;
    pChnCtx->VarWhiteCustomMode = CUSTOMMODE_TIME;
    pChnCtx->VarWhiteCustomBright = 20;
    pChnCtx->m_VarWhiteNightThreshold = 50;
    pChnCtx->m_VarWhiteDayThreshold = 10;

    pChnCtx->m_NormalWhiteBlk2CThreshold = 25;
    pChnCtx->m_NormalWhiteC2BlkThreshold = 50;

    pChnCtx->m_InfraredWorkMode = 1;
    pChnCtx->m_InfraredBrightness = 100;

    pChnCtx->m_ImageMode = 0;
    pChnCtx->m_BacklightEnable = 0;

    pChnCtx->Bright = 50;
    pChnCtx->Contrast = 50;
    pChnCtx->Saturation = 50;
    pChnCtx->Sharpness = 50;
    pChnCtx->Hue = 50;
    pChnCtx->m_InitiativeModeSensity = 50;

    pChnCtx->m_bStartMD = TRUE;

    pChnCtx->m_AudioCodec = eCodecG711U;

    pChnCtx->m_AlgoThr = ALGO_THRESHOLD_NORMAL;

    pChnCtx->m_SpeechNum = 0;
    pChnCtx->m_SpeechWrite = 0;
    pChnCtx->m_SpeechRead = 0;
    pChnCtx->m_SpeechNow = 0;
    pChnCtx->m_nSpeechPlayStatus = eSpeechPlayIdle;
    pChnCtx->m_enPtzStatus = ePtzStatus_Stop;

    memset(pChnCtx->m_SpeechFileName, 0, sizeof(pChnCtx->m_SpeechFileName));
    memset(&pChnCtx->m_AlarmSpeechCfg, 0, sizeof(SpeechAlarmInfo_t));
    pChnCtx->m_AlarmSpeechCfg.alarmOutDelay = 5;

    for (int i = 0; i < eSpeechTypeRemindMax; i++) {
        memset(&(pChnCtx->m_RemindSpeechCfg[i]), 0, sizeof(SpeechRemindInfo_t));
    }

    //pChannelContext->m_IRCheckCtr = 0;//默认由IPC检查
    pChnCtx->m_iPtzTrackBacktoPreset = ePtz2Preset_fulfil;

    pChnCtx->m_nAudioInVolNormal = 80;

    pChnCtx->m_FinishingGrade = EXTINGUISH_END;

    pChnCtx->m_HighTempFlag = 0;

#if defined(__DIGITAL_ZOOM_DRAG_SUPPORT__)
    pChnCtx->m_ZoomRealValue = 1.0;
    pChnCtx->m_ZoomDragCmd = DRAG_CMD_ZOOMST;
    pChnCtx->m_ZoomTrueRatio = DRAG_ZOOM_MIN_RATIO;
    pChnCtx->m_ZoomCurRatio = DRAG_ZOOM_MIN_RATIO;
    pChnCtx->m_ZoomChgStatus = 0;
    pChnCtx->m_DragRunFlag = 1;
	pChnCtx->m_FocusCurRatio = 1.0;
#if defined(__IS_SUPPORT_LONG_SHORT_FOCUS_ZOOM__)
    if(pChnCtx->m_ZoomLens == ForAndSix)
    {
        pChnCtx->m_WideMaxRatio = 6.0;
        pChnCtx->m_MiddleMaxRatio = 6.0;
        pChnCtx->m_TeleMaxRatio = 12.0;
        pChnCtx->m_FocusCurRatio = 1.0;
    }
    else
    {
        pChnCtx->m_WideMaxRatio = 6.0;
        pChnCtx->m_MiddleMaxRatio = 12.0;
        pChnCtx->m_TeleMaxRatio = 18.0;
        pChnCtx->m_FocusCurRatio = 1.0;
    }
#endif
#endif

    //初始化确认音频路径
    GetSpeechNamePath(pChnCtx->m_SpeechFilePath);

    return 0;
}

/* 编码通道信息逆初始化 */
int Zview_EncChInfo_DeInit()
{
    CChannelContext		*pChannelContext = NULL;
    HY_Enc_GetEncChInfo(&pChannelContext);

    HIEASY_FREE(pChannelContext->m_AudioStreamInfo.pBuf);
    pChannelContext->m_AudioStreamInfo.BufSize = 0;

    return 0;
}

/* 语音对讲通道初始化 */
int Zview_TalkingChInfo_Init(SystemInfo_t *pSysInfo)
{
    CTalkingChannel_t *pTalkingCh = NULL;

    if (NULL == pSysInfo)
    {
        ZW_ERROR("pSysInfo CAN NOT be NULL\n");
        return -1;
    }

    HY_Enc_GetTalkingChInfo(&pTalkingCh);

    pTalkingCh->m_bStart        = FALSE;

    pTalkingCh->m_TalkingIn.m_PortNum = 0;
    pTalkingCh->m_TalkingIn.BufSize   = 1024*2;
    pTalkingCh->m_TalkingIn.pBuf = (char *)malloc(pTalkingCh->m_TalkingIn.BufSize);
    if (NULL == pTalkingCh->m_TalkingIn.pBuf)
    {
        ZW_ERROR("m_TalkingIn malloc fail !!!!!\n");
        pSysInfo->m_TotalTalkingChannel = 0;
        return -1;
    }
    //pTalkingCh->m_TalkingIn.BufSize  = 0;
    pTalkingCh->m_TalkingIn.WritePos = 0;

    pTalkingCh->m_TalkingOut.m_PortNum = 0;
    pTalkingCh->m_TalkingOut.BufSize = 4*1024;
    pTalkingCh->m_TalkingOut.pBuf = (char *)malloc(pTalkingCh->m_TalkingOut.BufSize);
    if (NULL == pTalkingCh->m_TalkingOut.pBuf)
    {
        ZW_ERROR("m_TalkingOut.pBuf malloc Fail\n");
        pSysInfo->m_TotalTalkingChannel = 0;
        return -1;
    }
    //pTalkingCh->m_TalkingOut.BufSize = 0;
    pTalkingCh->m_TalkingOut.ReadPos  = 0;
    pTalkingCh->m_TalkingOut.WritePos = 0;

    return 0;
}

/* 语音对讲通道逆初始化 */
int Zview_TalkingChInfo_DeInit()
{
    CTalkingChannel_t	*pTalkingCh = NULL;
    HY_Enc_GetTalkingChInfo(&pTalkingCh);

    HIEASY_FREE(pTalkingCh->m_TalkingIn.pBuf);
    pTalkingCh->m_TalkingIn.BufSize = 0;

    HIEASY_FREE(pTalkingCh->m_TalkingOut.pBuf);
    pTalkingCh->m_TalkingOut.BufSize = 0;

    return 0;
}

static void CheckAndSaveAudioCustomFiles(CChannelContext *pChContext)
{
    if (!access(SPEECH_MOTION_ALARM_CUSTOM_FILE_TEMP, 0))
    {
        XTRACE_MUTEX_LOCK(&pChContext->m_SpeechCustomAMRFileLock);
        char command[128] = {0};
        snprintf(command, sizeof(command), "mv %s %s", SPEECH_MOTION_ALARM_CUSTOM_FILE_TEMP, SPEECH_MOTION_ALARM_CUSTOM_FILE);
        mysystem(command);
        XTRACE_MUTEX_UNLOCK(&pChContext->m_SpeechCustomAMRFileLock);
    }

    if (!access(SPEECH_REMIND_SCHEDULE_CUSTOM_FILE1_TMP, 0))
    {
        XTRACE_MUTEX_LOCK(&pChContext->m_SpeechCustomAMRFileLock);
        char command[128] = {0};
        snprintf(command, sizeof(command), "mv %s %s", SPEECH_REMIND_SCHEDULE_CUSTOM_FILE1_TMP, SPEECH_REMIND_SCHEDULE_CUSTOM_FILE1);
        mysystem(command);
        XTRACE_MUTEX_UNLOCK(&pChContext->m_SpeechCustomAMRFileLock);
    }

    if (!access(SPEECH_REMIND_SCHEDULE_CUSTOM_FILE2_TMP, 0))
    {
        XTRACE_MUTEX_LOCK(&pChContext->m_SpeechCustomAMRFileLock);
        char command[128] = {0};
        snprintf(command, sizeof(command), "mv %s %s", SPEECH_REMIND_SCHEDULE_CUSTOM_FILE2_TMP, SPEECH_REMIND_SCHEDULE_CUSTOM_FILE2);
        mysystem(command);
        XTRACE_MUTEX_UNLOCK(&pChContext->m_SpeechCustomAMRFileLock);
    }

    if (!access(SPEECH_REMIND_SCHEDULE_CUSTOM_FILE3_TMP, 0))
    {
        XTRACE_MUTEX_LOCK(&pChContext->m_SpeechCustomAMRFileLock);
        char command[128] = {0};
        snprintf(command, sizeof(command), "mv %s %s", SPEECH_REMIND_SCHEDULE_CUSTOM_FILE3_TMP, SPEECH_REMIND_SCHEDULE_CUSTOM_FILE3);
        mysystem(command);
        XTRACE_MUTEX_UNLOCK(&pChContext->m_SpeechCustomAMRFileLock);
    }
}

extern int g_subSyncFlag[SUPPORT_MAX_SENSOR_COUNT];
void * SYS_TimeThread( void * lpParameter)
{
    PRT_TID_NAME("SYS_TimeThread");
    ZW_DEBUG("%s Thread PID = %d !!!!!!!!!!!!\n", __FUNCTION__, getpid());

    int nSleepCnt = 0;
    CChannelContext *pChContext = NULL;
    ThreadInfo_t	*pParent = (ThreadInfo_t *)lpParameter;
    char		filebak[128] = {0};
    FILE		*file_analysis = NULL;
    char magic[8] = {0};
    char DataBuf[640] = {0};
    unsigned char analysis[32] = {0};
    int openSpeech = 0;
    unsigned int frameCount = 0;
    struct timespec CurTime, LastTime;
    int timeDiff = 0;
    unsigned int timeFlow = 0;
    int ret = 0;
    int datasize = 0;
    //enum Mode dec_mode;
    SystemInfo_t	*pSysInfo = NULL;
    int speechNeed = 0;
    int fileType = 0; //0:amr格式,1:G711格式
    int dec_mode = 0;
    int read_size = 0; 		// 根据AMR文件每帧的头部获取该帧数据大小
    int block_size[] = {12, 13, 15, 17, 19, 20, 26, 31, 5, 6, 5, 5, 0, 0, 0, 0};
    char bIsSpeechPlayFinished = 0;
    int bForcePlayOver = 0;
    int curBroadCastTimes = 0;
    int maxBroadCastTimes = 0;
    SDK_ResInfo_t *pSDK_Info = NULL;
    char bIsSpeechBreak = 0;
    CPlatformChannelContext *pPlatCtx = NULL;
    int alarmSleep = 0;

#ifdef __ONE_CLICK_CALL__
    int SpeechDuCnt = 0;
#endif

    HY_Res_GetSystemInfo(&pSysInfo);
    HY_Enc_GetEncChInfo(&pChContext);
    HY_Res_GetSDKInfo(&pSDK_Info);
    pPlatCtx = (CPlatformChannelContext *)pChContext->pPlatformChannelhandle;

    while (1)
    {
        if (0 != pParent->m_Exit)
        {
            goto THREAD_EXIT;
            break;
        }

        //在这里判断一下退出线程
        if (UpgradeMode == pSDK_Info->m_enSdkModeCur)
        {
            goto THREAD_EXIT;
            break;
        }
        usleep(100000);

        XTRACE_MUTEX_LOCK(&pChContext->m_SpeechLock);

        //获取需要播放的音频文件名，及报警类型
        if ((pChContext->m_SpeechNum > 0) || (1 == pChContext->m_BoardcastSpeechFirst))
        {
             if (MAX_SPEECH_CACHE_NUM == pChContext->m_SpeechRead)
            {
                pChContext->m_SpeechRead = 0;
            }

            if (1 == pChContext->m_BoardcastSpeechFirst)
            {
                pChContext->m_BoardcastSpeechFirst = 0;
                pChContext->m_SpeechRead = 0;
                pChContext->m_SpeechNum = 1;
            }

            strncpy(filebak, pChContext->m_SpeechFileName[pChContext->m_SpeechRead], sizeof(filebak));
            pChContext->m_SpeechNow = pChContext->m_SpeechType[pChContext->m_SpeechRead];
            pChContext->m_SpeechRead++;
            pChContext->m_SpeechNum--;

            curBroadCastTimes = 0;
            speechNeed = 1;
            pChContext->m_nSpeechPlayStatus = eSpeechPlayIdle;
            if (1 == bIsSpeechBreak)
            {
                //条语音被中断这里继续保持播报占用状态
                bIsSpeechBreak = 0;
                pChContext->m_nSpeechPlayStatus = eSpeechPlaying;
            }
            else
            {
                pChContext->m_nSpeechPlayStatus = eSpeechPlayIdle;
            }
        }
        XTRACE_MUTEX_UNLOCK(&pChContext->m_SpeechLock);

        //如果有新设置的报警语音定制文件，则拷贝到配置分区
        CheckAndSaveAudioCustomFiles(pChContext);

        /* 报警语音，但是输出未使能，不播报 */
        if (speechNeed && IS_ALARM_SPEECH_TYPE(pChContext->m_SpeechNow) && pChContext->m_AlarmSpeechCfg.enable == 0)
        {
            speechNeed = 0;
        }

        /* 定时提醒语音，但是输出未使能，不播报 */
        eSpeechRemindType curRemindType = eSpeechType_Remind_1;
        SpeechRemindTypeMap(pChContext->m_SpeechNow, &curRemindType);
        if (speechNeed && IS_REMIND_SPEECH_TYPE(pChContext->m_SpeechNow) && pChContext->m_RemindSpeechCfg[curRemindType].enable == 0)
        {
            speechNeed = 0;
        }

        //声音报警配置只针对报警语音(如果新增了语音报警的类型那这里需要添加)
        if (speechNeed)
        {
            /*此处根据文件后缀名做文件类型判断，目前暂时只区分amr与g711u*/
            if (strstr(filebak, SPEECH_PREFIX) != NULL) {
                fileType = eCodecPCM; //amr转为pcm
            }
            else {
                fileType = eCodecG711U;
            }

            speechNeed = 0;

            ret = MP4Sys_SetSpeechingEnable(1, fileType);
            if (0x102 == ret)//当前已经打开了对讲了
            {
                ZW_DEBUG("talking is starting, need wait for 60s ...\n");
                sleep(1);
                /* 睡眠之后，需要及时清空相关变量 */
                pChContext->m_SpeechNum = 0;
                pChContext->m_SpeechNow = 0;
                pChContext->m_nSpeechPlayStatus = eSpeechPlayIdle;
                ZW_DEBUG("wait time is arrival! start to check speech!\n");
                continue;
            }

            file_analysis = fopen(filebak, "rb");
            if (file_analysis <= 0)
            {
                ZW_INFO("The speech file[%s] is NULL\n", filebak);
                pChContext->m_SpeechNow = 0;
                MP4Sys_SetSpeechingEnable(0, fileType);
                continue;
            }

            pChContext->m_SpeechPlayEnd = 0;

            if (eCodecPCM == fileType) //如果是amr格式转为pcm
            {
                memset(magic, 0, sizeof(magic));
                //校验文件头
                fread(magic, sizeof(char), strlen(AMR_MAGIC_NUMBER), file_analysis);
                if (strncmp(magic, AMR_MAGIC_NUMBER, strlen(AMR_MAGIC_NUMBER)))
                {
                    ZW_INFO("error AMR data!\n");
                    fclose(file_analysis);
                    file_analysis = NULL;
                    pChContext->m_SpeechNow = 0;
                    pChContext->m_nSpeechPlayStatus = eSpeechPlayIdle;
                    MP4Sys_SetSpeechingEnable(0, fileType);
                    continue;
                }
            }
            ZW_INFO("Read SpeechFile [%s].....................\n", filebak);

            pChContext->m_nSpeechPlayStatus = eSpeechPlaying;

#ifdef __ONE_CLICK_CALL__
            if (SPEECH_KEY2CALL_DU == pChContext->m_SpeechNow)
            {
                pChContext->m_key2callDuSpeechStatus = 1;
            }
            SpeechDuCnt = 0;
#endif

            openSpeech = 1;
            frameCount = 0;
            timeFlow = 0;
            timeDiff = 0;
            datasize = 0;
            bIsSpeechPlayFinished = 0;

            if (IS_ALARM_SPEECH_TYPE(pChContext->m_SpeechNow)) {
                alarmSleep = 1;
                maxBroadCastTimes = pChContext->m_AlarmSpeechCfg.broadCastTimes;
                MP4Sys_SetAudioOutVol(pChContext, pChContext->m_AlarmSpeechCfg.broadCastVolume, 0, 2);
            }
            else if (IS_REMIND_SPEECH_TYPE(pChContext->m_SpeechNow)) {
                alarmSleep = 1;
                maxBroadCastTimes = pChContext->m_RemindSpeechCfg[curRemindType].broadCastTimes;
                MP4Sys_SetAudioOutVol(pChContext, pChContext->m_RemindSpeechCfg[curRemindType].broadCastVolume, 0, 2);
            }
            else
            {
                MP4Sys_SetAudioOutVol(pChContext, pPlatCtx->m_AudioOutVol, 0, 0);
            }
            ZW_INFO("m_SpeechNow[%d] NormalVol[%d] AlarmVol[%d] RemindVol[%d]\n", pChContext->m_SpeechNow, \
                    pPlatCtx->m_AudioOutVol, \
                    pChContext->m_AlarmSpeechCfg.broadCastVolume, \
                    pChContext->m_RemindSpeechCfg[curRemindType].broadCastVolume);

            memset(&LastTime, 0, sizeof(struct timeval));
            memset(&CurTime, 0, sizeof(struct timeval));

            //如果有语音播报则进入播报流程
            while (1)
            {
                if (0 != pParent->m_Exit)
                {
                    if (file_analysis)
                    {
                        fclose(file_analysis);
                        file_analysis = NULL;
                    }
                    pChContext->m_nSpeechPlayStatus = eSpeechPlayIdle;
                    break;
                }

                ret = MP4Sys_SetSpeechingEnable(1, fileType);
                if ((0x102 == ret) || (1 == pChContext->m_BoardcastSpeechFirst)) //当前已经打开了对讲、正在切换灯板模式或者语音播报时，进行打断
                {
                    if (file_analysis)
                    {
                        fclose(file_analysis);
                        file_analysis = NULL;
                    }
                    ZW_DEBUG("talking is starting, need wait for 60s ...\n");
                    sleep(1);
                    /* 睡眠之后，需要及时清空相关变量 */
                    if (1 == pChContext->m_BoardcastSpeechFirst)
                    {
                        pChContext->m_SpeechRead = 0;
                        pChContext->m_SpeechNum = 1;
                    }
                    pChContext->m_SpeechNum = 0;
                    pChContext->m_SpeechNow = 0;
                    pChContext->m_nSpeechPlayStatus = eSpeechPlayIdle;
                    ZW_DEBUG("wait time is arrival! start to check speech!\n");
                    break;
                }

                clock_gettime(CLOCK_MONOTONIC, &LastTime);
                if ((LastTime.tv_sec < CurTime.tv_sec))
                {
                    CurTime = LastTime;
                }

                // ZW_INFO("---:%d\n",(LastTime.tv_sec-CurTime.tv_sec) * 1000000+(LastTime.tv_nsec-CurTime.tv_nsec) / 1000 );
                //累计实际睡眠耗时
                if (frameCount)
                {
                    timeFlow += (LastTime.tv_sec - CurTime.tv_sec) * 1000000 + (LastTime.tv_nsec - CurTime.tv_nsec) / 1000;
                }

            ReadAgain:
                bIsSpeechPlayFinished = 0;
                if (eCodecPCM == fileType)//如果是amr格式转为pcm
                {
                    memset(analysis, 0, sizeof(analysis));
                    ret = fread(analysis, sizeof (unsigned char), 1, file_analysis);
                }
                else
                {
                    ret = fread(DataBuf, 1, AUDIO_G711_ONE_FRAME_SIZE, file_analysis);
                    datasize = AUDIO_G711_ONE_FRAME_SIZE;
                }
                if (ret > 0)
                {
                    if (eCodecPCM == fileType) //如果是amr格式
                    {
                        dec_mode = (analysis[0] >> 3) & 0x0F;
                        read_size = block_size[dec_mode];
                        fread(&analysis[1], sizeof (char), read_size, file_analysis );
                        Decoder_Interface_Decode(pSysInfo->m_pAmrToPcmDecoder, analysis, (short *)(DataBuf + datasize), 0);
                        datasize += pSysInfo->m_SpeechAudioFrameSize;
                    }

                    /* 判断是否读取到文件末尾 */
                    unsigned char c;
                    ret = fread(&c, sizeof (unsigned char), 1, file_analysis);
                    if (ret <= 0)
                    {
                        /* 已读到文件末尾 */
                        pChContext->m_SpeechPlayEnd = 1;
                    }
                    else
                    {
                        /* 回退至之前位置 */
                        fseek(file_analysis, -1, SEEK_CUR);
                    }

                    if (pSysInfo->m_SpeechAudioFrameSize == datasize)
                    {
                        // 取一帧音频数据写入缓存区
                        ret = MP4Sys_SetSpeechData(DataBuf, datasize);
                        if ((0x101 == ret) || (0x102 == ret))
                        {
                            ZW_INFO("Set speech data Fail!**********************，ret=0x%x\n", ret);
                            fclose(file_analysis);
                            file_analysis = NULL;
                            speechNeed = 1;
#ifdef __ONE_CLICK_CALL__
                            if (pChContext->m_SpeechNow == SPEECH_KEY2CALL_DU)
                            {
                                speechNeed = 0;
                                pChContext->m_key2callDuSpeechStatus = 0;
                            }
#endif
                            sleep(1);
                            pChContext->m_SpeechNum = 0;
                            pChContext->m_SpeechNow = 0;
                            pChContext->m_nSpeechPlayStatus = eSpeechPlayIdle;
                            break;
                        }
#ifdef __ONE_CLICK_CALL__
                        if (1 == pChContext->m_recvkey2callhanup)
                        {
                            pChContext->m_recvkey2callhanup = 0;
                            if ( pChContext->m_SpeechNow == SPEECH_KEY2CALL_DU)
                            {
                                speechNeed = 0;
                                fclose(file_analysis);
                                file_analysis = NULL;
                                pChContext->m_nSpeechPlayStatus = eSpeechPlayIdle;
                                pChContext->m_key2callDuSpeechStatus = 0;
                                HY_Res_SDK_Usleep(1000000);
                                break;
                            }
                        }

                        if (pChContext->m_recvPauseAudioLightAlarm)
                        {
                            if (IS_ALARM_SPEECH_TYPE(pChContext->m_SpeechNow))
                            {
                                speechNeed = 0;
                                fclose(file_analysis);
                                file_analysis = NULL;
                                MP4Sys_SetAudioOutVol(pChContext, pPlatCtx->m_AudioOutVol, 0, 0);
                                ZW_INFO("Break of Alarm, SetAudioOutVol NormalVol[%d]\n", pPlatCtx->m_AudioOutVol);
                                pChContext->m_nSpeechPlayStatus = eSpeechPlayIdle;
                                HY_Res_SDK_Usleep(1000000);
                                break;
                            }
                        }
#endif
                        if (pChContext->m_recvRemindHangUp)
                        {
                            pChContext->m_recvRemindHangUp = 0;
                            if (IS_REMIND_SPEECH_TYPE(pChContext->m_SpeechNow))
                            {
                                speechNeed = 0;
                                fclose(file_analysis);
                                file_analysis = NULL;
                                MP4Sys_SetAudioOutVol(pChContext, pPlatCtx->m_AudioOutVol, 0, 0);
                                ZW_INFO("Break of Remind, SetAudioOutVol NormalVol[%d]\n", pPlatCtx->m_AudioOutVol);
                                pChContext->m_nSpeechPlayStatus = eSpeechPlayIdle;
                                HY_Res_SDK_Usleep(1000000);
                                break;
                            }
                        }

                        datasize = 0;
                    }
                }
                else //语音完了
                {
                    curBroadCastTimes++;
                    frameCount = 0;   // 语音播放完了，应该把帧数清零，防止播放多次，照成时间戳出错

                    // 以下类型语音结束后不退出，交由后面播放次数判断是否继续
                    if (IS_ALARM_SPEECH_TYPE(pChContext->m_SpeechNow)
                        || IS_REMIND_SPEECH_TYPE(pChContext->m_SpeechNow)
#ifdef __ONE_CLICK_CALL__
                        || SPEECH_KEY2CALL_DU == pChContext->m_SpeechNow
#endif
                    )
                    {
                        if (eCodecPCM == fileType) {
                            fseek(file_analysis, strlen(AMR_MAGIC_NUMBER), SEEK_SET);
                        }
                        else {
                            fseek(file_analysis, 0L, SEEK_SET);
                        }
                        bIsSpeechPlayFinished = 1;  //标志整句已播完
                    }
                    else
                    {
                        fclose(file_analysis);
                        file_analysis = NULL;
                        ZW_INFO("Read over\n");
                        pChContext->m_nSpeechPlayStatus = eSpeechPlayIdle;
                        break;
                    }
#ifdef __ONE_CLICK_CALL__
                    if (2 == pChContext->m_recvkey2callhanup && SPEECH_KEY2CALL_DU != pChContext->m_SpeechNow)
                    {
                        pChContext->m_recvkey2callhanup = 0;
                    }
#endif
                }

                if (0 == bIsSpeechPlayFinished)
                {
                    clock_gettime(CLOCK_MONOTONIC,&CurTime);
                    if ((CurTime.tv_sec < LastTime.tv_sec) || ((CurTime.tv_sec == LastTime.tv_sec) && (CurTime.tv_nsec < LastTime.tv_nsec)))
                    {
                        LastTime = CurTime;
                    }
                    //累计实际解码耗时
                    if (((CurTime.tv_sec - LastTime.tv_sec) * 1000000 + (CurTime.tv_nsec - LastTime.tv_nsec) / 1000) < 0)
                    {
                        // ZW_INFO("###:cur time = %d Sec , %d nsec | last time = %d sec  %d nsec \n",
                        // 		CurTime.tv_sec, CurTime.tv_nsec, LastTime.tv_sec, LastTime.tv_nsec);
                        if (eCodecPCM == fileType)//如果是amr格式转为pcm
                        {
                            timeFlow += 20000;
                        }
                        else
                        {
                            timeFlow += 40000;
                        }
                    }
                    else
                    {
                        timeFlow += (CurTime.tv_sec - LastTime.tv_sec) * 1000000 + (CurTime.tv_nsec - LastTime.tv_nsec) / 1000;
                    }
                    if (!frameCount)  // 送第一帧数据的时间有点多，因为第一帧是开始，刚没有开始播放，所以将实际的第一帧时间重置下，然后第二帧及其后面的数据，应该开始衔接好
                    {
                        if(eCodecPCM == fileType)//如果是amr格式转为pcm
                        {
                            timeFlow = timeFlow > 20000 ? 20000 : timeFlow;
                        }
                        else
                        {
                            timeFlow = timeFlow > 40000 ? 40000 : timeFlow;
                        }

                        if(timeFlow < 0)
                        {
                            timeFlow = 0;
                        }
                    }
                    // ZW_INFO("###:%d\n", (CurTime.tv_sec - LastTime.tv_sec) * 1000000 + (CurTime.tv_nsec - LastTime.tv_nsec) / 1000);
                    //帧数累加，计算出理论时间
                    frameCount++;
                    if (eCodecPCM == fileType)//如果是amr格式转为pcm
                    {
                        timeDiff = frameCount*20000 - timeFlow;//理论总时间-实际流逝时间
                    }
                    else
                    {
                        timeDiff = frameCount*40000 - timeFlow;//理论总时间-实际流逝时间
                    }
                    // ZW_INFO("timeFlow=%d, timeDiff=%d, alarmDelayStartTime=%d, alarmOutDelay=%d\n", timeFlow, timeDiff, alarmDelayStartTime, pChContext->m_AlarmSpeechCfg.alarmOutDelay);
                    if (timeDiff > 0)
                    {
                        HY_Res_SDK_Usleep(timeDiff);
                    }
                    else
                    {
                        HY_Res_SDK_Usleep(1000);
                    }
                }

                //对报警的声音播放次数进行判断，若超过则终止
                if (IS_ALARM_SPEECH_TYPE(pChContext->m_SpeechNow))
                {
                    if (1 == pChContext->m_SpeechAlarmDelayClear)
                    {
                    	if(maxBroadCastTimes == curBroadCastTimes)
                    	{	
                            curBroadCastTimes = 0;//更新计算延时的起始时间点
                        	ZW_INFO("reset curBroadCastTimes to 0 \n");
						}

                        pChContext->m_SpeechAlarmDelayClear = 0;
                    }
                    else if (2 == pChContext->m_SpeechAlarmDelayClear)
                    {
                        // 本轮播报完就立即结束，播报下一个
                        curBroadCastTimes = maxBroadCastTimes;
                        bForcePlayOver = 1;
                        pChContext->m_SpeechAlarmDelayClear = 0;
                    }

                    if (1 == bIsSpeechPlayFinished)
                    {
                        if (bForcePlayOver || (maxBroadCastTimes - curBroadCastTimes) <= 0 || pChContext->m_recvPauseAudioLightAlarm)
                        {
                            bForcePlayOver = 0;
                            fclose(file_analysis);
                            file_analysis = NULL;

                            /* 报警提示音播放完成 */
                            if (alarmSleep == 1) {
                                sleep(1);
                                alarmSleep = 0;
                            }
                            MP4Sys_SetAudioOutVol(pChContext, pPlatCtx->m_AudioOutVol, 0, 0);
                            ZW_INFO("End of Alarm, SetAudioOutVol NormalVol[%d]\n", pPlatCtx->m_AudioOutVol);

                            pChContext->m_nSpeechPlayStatus = eSpeechPlayIdle;
                            break;
                        }
                        else
                        {
                            alarmSleep = 1;
                            pChContext->m_SpeechPlayEnd = 0;
                            ZW_INFO("Continue of Alarm, BroadCastTimes[cur:%d -- max:%d]\n", curBroadCastTimes, maxBroadCastTimes);
                            goto ReadAgain;
                        }
                    }
                }
                //对定时提醒的声音播放次数进行判断，若超过则终止
                else if (IS_REMIND_SPEECH_TYPE(pChContext->m_SpeechNow))
                {
                    if (1 == bIsSpeechPlayFinished)
                    {
                        if ((maxBroadCastTimes - curBroadCastTimes) <= 0 || pChContext->m_recvRemindHangUp)
                        {
                            fclose(file_analysis);
                            file_analysis = NULL;

                            /* 定时提醒提示音播放完成 */
                            if (alarmSleep == 1) {
                                sleep(1);
                                alarmSleep = 0;
                            }
                            MP4Sys_SetAudioOutVol(pChContext, pPlatCtx->m_AudioOutVol, 0, 0);
                            ZW_INFO("End of Remind, SetAudioOutVol NormalVol[%d]\n", pPlatCtx->m_AudioOutVol);

                            pChContext->m_recvRemindHangUp = 0;
                            pChContext->m_nSpeechPlayStatus = eSpeechPlayIdle;
                            break;
                        }
                        else
                        {
                            alarmSleep = 1;
                            pChContext->m_SpeechPlayEnd = 0;
                            ZW_INFO("Continue of Remind, BroadCastTimes[cur:%d -- max:%d]\n", curBroadCastTimes, maxBroadCastTimes);
                            goto ReadAgain;
                        }
                    }
                }
#ifdef __ONE_CLICK_CALL__
                else if (SPEECH_KEY2CALL_DU == pChContext->m_SpeechNow)
                {
                    if (1 == bIsSpeechPlayFinished)
                    {
                        //一键呼叫嘟嘟提示音需要外部打断，满足且整句播放完成才退出。
                        if (0 != pChContext->m_recvkey2callhanup)
                        {
                            fclose(file_analysis);
                            file_analysis = NULL;
                            pChContext->m_nSpeechPlayStatus = eSpeechPlayIdle;
                            pChContext->m_key2callDuSpeechStatus = 0;
                            pChContext->m_recvkey2callhanup = 0;
                            SpeechDuCnt = 0;
                            ZW_INFO("Speech Du delay over!\n");
                            break;
                        }
                        else
                        {
                            // 此处usleep是为了模拟嘟嘟停顿声
                            if (++SpeechDuCnt == 1) {
                                HY_Res_SDK_Usleep(1500000);
                            }
                            else {
                                HY_Res_SDK_Usleep(3000000);
                            }

                            ZW_INFO("Speech Du delay over!\n");
                            pChContext->m_SpeechPlayEnd = 0;
                            goto ReadAgain;
                        }
                    }
                }
#endif
            }

            if (openSpeech)
            {
                HY_Res_SDK_Usleep(350000);
                MP4Sys_SetSpeechingEnable(0, fileType);
                openSpeech = 0;
            }
        }
        pChContext->m_SpeechNow = 0;
#ifdef __ONE_CLICK_CALL__
        pChContext->m_key2callDuSpeechStatus = 0;
#endif
        if (++nSleepCnt >= 10)
        {
            nSleepCnt = 0;
            pChContext->m_audioSyncFlag = 1;
            for (int i = 0; i < SUPPORT_MAX_SENSOR_COUNT; i++)
            {
                g_subSyncFlag[i] = 1;
            }
        }
    }

THREAD_EXIT:
    return NULL;
}

#if defined(__DIGITAL_ZOOM_DRAG_SUPPORT__)
extern void *TrackerZoomWorkThread(void *lpParameter);
#endif

/* 创建SDK需要的线程 */
int Zview_CreatThread()
{
    int RetValue = 0;
    SystemInfo_t *pSysInfo;

    HY_Res_GetSystemInfo(&pSysInfo);

     /* 创建ip复位io口状态检测线程 */
     RetValue = HY_Res_CreatThread(&pSysInfo->m_CheckSecretPinStatusThread, 0, CheckSecretPinStatusThread);
     if (RetValue != 0)
     {
         ZW_ERROR("m_CheckSecretPinStatusThread Creat Fail\n");
         return -1;
     }

    /* 创建OSD刷新线程 */
     RetValue = HY_Res_CreatThread(&pSysInfo->m_OSDUpdateThread, 0, UpdateOSDThread);
     if (RetValue != 0)
     {
         ZW_ERROR("m_OSDUpdateThread Creat Fail\n");
         return -1;
     }

    /* 创建IRCUT切换线程 */
    RetValue = HY_Res_CreatThread(&pSysInfo->m_IRCUTSwitchThread, 0, IRCUTSwitchThread);
    if (RetValue != 0)
    {
        ZW_ERROR("m_IRCUTSwitchThread Creat Fail\n");
        return -1;
    }

    /* 创建亮度检测线程 */
    RetValue = HY_Res_CreatThread(&pSysInfo->m_CheckBrightnessThread, 0, CheckBrightnessThread);
    if (RetValue != 0)
    {
        ZW_ERROR("m_CheckBrightnessThread Creat Fail\n");
        return -1;
    }

 #if (USE_ALGO)
     /* 算法检测线程 */
     RetValue = HY_Res_CreatThread(&pSysInfo->m_HDThread, 30, Hi_MPP_HDThread);
    if (RetValue != 0)
    {
        ZW_ERROR("Hi_MPP_HDThread thread Creat Fail !!!!!!!!!! \n");
        return -1;
    }

     RetValue = HY_Res_CreatThread(&pSysInfo->m_DrawLineThread, 0, DrawLineThread);
     if (RetValue != 0)
     {
         ZW_ERROR("DrawLineThread thread Creat Fail !!!!!!!!!! \n");
         return -1;
     }
#endif

     /* 创建音频解码线程 */
     if (pSysInfo->m_TotalTalkingChannel > 0)
     {
         RetValue = HY_Res_CreatThread(&pSysInfo->m_AuioDecThread, 0, AudioDecThread);
         if (RetValue != 0)
         {
             ZW_ERROR("m_AuioDecThread Creat Fail\n");
             return -1;
         }
     }

     /* 创建音频输入线程 */
     if (pSysInfo->m_TotalTalkingChannel > 0)
     {
         RetValue = HY_Res_CreatThread(&pSysInfo->m_AuioInThread, 0, AudioInThread);
         if (RetValue != 0)
         {
             ZW_ERROR("m_AuioDecThread Creat Fail\n");
             return -1;
         }
     }

     RetValue = HY_Res_CreatThread(&pSysInfo->m_TimerThread, 0, SYS_TimeThread);
     if (RetValue != 0)
     {
         ZW_ERROR("m_TimerThread thread Creat Fail !!!!!!!!!! \n");
         return -1;
     }

    //日夜转换线程，根据白天黑夜状态加载不同的图像参数
    RetValue = HY_Res_CreatThread(&pSysInfo->m_CheckDayAndNightThread, 0, CheckDayAndNightThread);
    if (RetValue != 0)
    {
        ZW_ERROR("m_CheckDayAndNightThread thread Creat Fail !!!!!!!!!! \n");
        return -1;
    }

    if (1 == pSysInfo->m_bSupportCloudIPCSlowFrame)
    {
        RetValue = HY_Res_CreatThread(&pSysInfo->m_SlowFrameModeSwitchThread, 0, SlowFrameModeSwitchThread);
        if (RetValue != 0)
        {
            ZW_ERROR("SlowFrameModeSwitch thread Creat Fail !!!!!!!!!! \n");
            return -1;
        }

    }
#if defined(__DIGITAL_ZOOM_DRAG_SUPPORT__)
    //创建数字变倍线程
    if (1 == pSysInfo->m_bSupportZoom)
    {
        RetValue = HY_Res_CreatThread(&pSysInfo->m_TrackerZoomThread, 0, TrackerZoomWorkThread);
        if (RetValue != 0)
        {
            ZW_ERROR("TrackerZoomWorkThread thread Creat Fail !!!!!!!!!! \n");
            return -1;
        }
    }
#endif

#if defined __ONE_CLICK_CALL__
    //一键呼叫gpio检测
    if(1 == pSysInfo->m_bSupportKeyToCall )
    {
        RetValue = HY_Res_CreatThread(&pSysInfo->m_KeytocallThread, 0, CheckKeytocallPinStatusThread);
        if (RetValue != 0)
        {
           ZW_ERROR("m_AudioDetectThread thread Creat Fail !!!!!!!!!! \n");
           return -1;
        }
    }
#endif

#if !defined __ISP_IMAGE_DEBUG__
    RetValue = HY_Res_CreatThread(&pSysInfo->m_MiscTaskThread, 0, Hi_MPP_MiscTaskThread);
    if (RetValue != 0)
    {
        ZW_ERROR("SlowFrameModeSwitch thread Creat Fail !!!!!!!!!! \n");
        return -1;
    }
#endif

    return 0;
}

/* 退出SDK创建的线程 */
int Zview_ExitThread()
{
    SystemInfo_t *pSysInfo = NULL;
    CChannelContext *pChCtx = NULL;

    HY_Res_GetSystemInfo(&pSysInfo);
    HY_Enc_GetEncChInfo(&pChCtx);

    /* 退出IP复位检测线程 */
    ZW_DEBUG("HY_Res_DestroyThread m_CheckSecretPinStatusThread enter\n");
    HY_Res_DestroyThread(&pSysInfo->m_CheckSecretPinStatusThread);

    /* 退出OSD刷新线程 */
    ZW_DEBUG("HY_Res_DestroyThread m_OSDUpdateThread enter\n");
    HY_Res_DestroyThread(&pSysInfo->m_OSDUpdateThread);
#if defined(__DIGITAL_ZOOM_DRAG_SUPPORT__)
    //销毁变倍线程
    ZW_DEBUG("HY_Res_DestroyThread m_TrackerZoomThread enter\n");
    HY_Res_DestroyThread(&pSysInfo->m_TrackerZoomThread);
#endif

    /* 退出IRCUT切换线程 */
    ZW_DEBUG("HY_Res_DestroyThread m_IRCUTSwitchThread enter\n");
    HY_Res_DestroyThread(&pSysInfo->m_IRCUTSwitchThread);

#if (USE_ALGO)
    ZW_DEBUG("HY_Res_DestroyThread m_DrawLineThread enter\n");
    HY_Res_DestroyThread(&pSysInfo->m_DrawLineThread);

    ZW_DEBUG("HY_Res_DestroyThread m_HDThread enter\n");
    HY_Res_DestroyThread(&pSysInfo->m_HDThread);
#endif

    ZW_DEBUG("HY_Res_DestroyThread m_TimerThread enter\n");
    HY_Res_DestroyThread(&pSysInfo->m_TimerThread);

    /* 退出音频解码线程 */
    ZW_DEBUG("HY_Res_DestroyThread m_AuioDecThread enter\n");
    if (pSysInfo->m_TotalTalkingChannel > 0)
    {
        ZW_DEBUG("HY_Res_DestroyThread m_AuioDecThread enter\n");
        HY_Res_DestroyThread(&pSysInfo->m_AuioDecThread);
    }

    /* 退出音频输入线程 */
    ZW_DEBUG("HY_Res_DestroyThread m_AuioInThread enter\n");
    if (pSysInfo->m_TotalTalkingChannel > 0)
    {
        ZW_DEBUG("HY_Res_DestroyThread m_AuioInThread enter\n");
        HY_Res_DestroyThread(&pSysInfo->m_AuioInThread);
    }

    /* 退出亮度检测线程 */
    ZW_DEBUG("HY_Res_DestroyThread m_CheckBrightnessThread enter\n");
    HY_Res_DestroyThread(&pSysInfo->m_CheckBrightnessThread);

    /* 退出日夜切换线程 */
    ZW_DEBUG("HY_Res_DestroyThread m_CheckDayAndNightThread enter\n");
    HY_Res_DestroyThread(&pSysInfo->m_CheckDayAndNightThread);

    if (1 == pSysInfo->m_bSupportCloudIPCSlowFrame)
    {
        ZW_DEBUG("HY_Res_DestroyThread m_SlowFrameModeSwitchThread enter\n");
        HY_Res_DestroyThread(&pSysInfo->m_SlowFrameModeSwitchThread);
    }

    if (1 == pSysInfo->m_bSupportZoom)
    {
        ZW_DEBUG("HY_Res_DestroyThread m_SlowFrameModeSwitchThread enter\n");
        HY_Res_DestroyThread(&pSysInfo->m_TrackerZoomThread);
    }

#if !defined __ISP_IMAGE_DEBUG__
    ZW_DEBUG("HY_Res_DestroyThread m_MiscTaskThread enter\n");
    HY_Res_DestroyThread(&pSysInfo->m_MiscTaskThread);
#endif

#if defined __ONE_CLICK_CALL__
        /* 退出一键呼叫检测线程 */
        ZW_DEBUG("HY_Res_DestroyThread m_KeytocallThread enter\n");
        HY_Res_DestroyThread(&pSysInfo->m_KeytocallThread);
#endif

    return 0;
}

