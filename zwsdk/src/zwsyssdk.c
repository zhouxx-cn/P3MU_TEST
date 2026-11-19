/*!
 *\file   Zwsyssdk.c
 *\brief  系统SDK,编码函数、系统控制函数接口
 *\author caoy
 *\date	  2012/08/16
 *\version 0.1
 */
#include <stdlib.h>

#include "hisdk.h"
#include "Init.h"
#include "Common.h"

//#define NO_USE_VBR
//#define NO_USE_TALKING
//#define NO_CHANGE_IMAGE_DEFAULT_CONFIG
//#define NO_CHANGE_ENCODE

//-----------------------------------------------------------------------------
//	说明:	初始化系统,该函数必须在应用软件启动时首先调用；
//  具体流程如下：
//	<1>		完成系统SDK需要的编码通道句柄与解码通道句柄各种初始化工作。
//	<2>		需要配置相应的视频源
//  <3>		初始化安霸的SDK模块
//  <4>		初始化视频捕获、编码、视频输出模块 
//  <5>		开辟所需要的线程。
//	返回:	成功返回SDK_SUCESS，否则返回错误码
//-----------------------------------------------------------------------------
int     ZWSys_Init(int mode)
{
	int nRet = SDK_SUCESS;

	nRet = MP4Sys_InitDSPs();

	return nRet;
}

int     ZWSys_DeInit()
{
	int nRet = SDK_SUCESS;
	
	nRet = MP4Sys_DeInitDSPs();
	
	return nRet;
}

int ZWSys_GetPresetConfig(ZWSDK_PresetConfig *pPresetConfig)
{
    int nRet = SDK_SUCESS;

    nRet = HY_Res_GetPresetConfig(pPresetConfig);

    return nRet;
}

int ZWSys_SetPresetConfig(ZWSDK_PresetConfig PresetConfig)
{
    int nRet = SDK_SUCESS;

    nRet = HY_Res_SetPresetConfig(PresetConfig);

    return nRet;
}

//-----------------------------------------------------------------------------
//进入/退出升级模式(反初始化SDK，并启动蓝屏帧)
//unsigned int *pu32PhyAddr     输出供升级使用的内存物理地址
//void **ppVirtAddr      输出供升级使用的内存虚拟地址
//unsigned int *puMmzAllocSize      输出供升级使用的内存大小
//退出升级模式时后面几个参数可以为NULL
//-----------------------------------------------------------------------------
int     ZWSys_SwitchToUpgradeMode(int bInto, unsigned int *pu32PhyAddr, void **ppVirtAddr, unsigned int *puMmzAllocSize)
{
	int nRet = SDK_SUCESS;
	
	nRet = MP4Sys_SwitchToUpgradeMode(bInto, pu32PhyAddr, ppVirtAddr, puMmzAllocSize);
	
	return nRet;
}

int     ZWSys_GetBoardInfo(int *pBoardInfo )
{
	int nRet = SDK_SUCESS;
	if (NULL == pBoardInfo)
	{
		ZW_ERROR("INPUT Point are NULL\n");
		nRet = ERR_SDK_INVALID_ARGUMENT;
	}
	else
	{
		*pBoardInfo = MP4Sys_GetBoardInfo();
	}
	
	return nRet;
}

int ZWSys_GetStreamCount(void *hChannelHandle, int *stream)
{
	if (NULL == stream)
	{
		ZW_ERROR("INPUT Point are NULL\n");
		return ERR_SDK_INVALID_ARGUMENT;
	}

	int count=1;
	
	count |= (1<<1);
	*stream = count;

	return 0;
}

int     ZWSys_GetTUTKUID(char *puid)
{
	int nRet = SDK_SUCESS;
	if (NULL == puid)
	{
		ZW_ERROR("INPUT Point are NULL\n");
		return ERR_SDK_INVALID_ARGUMENT;
	}
	nRet = MP4Sys_GetTUTKUID(puid);

	return nRet;
}

int     ZWSys_GetSerialNum(unsigned char * pSerialNum)
{
	if (NULL == pSerialNum)
	{
		ZW_ERROR("INPUT Point are NULL\n");
		return ERR_SDK_INVALID_ARGUMENT;
	}

	MP4Sys_GetSerialNum(pSerialNum);

	return SDK_SUCESS;
}

int     ZWSys_GetResetPinStatus(int *pResetStatus)
{
	int nRet = SDK_SUCESS;
	
	if (NULL == pResetStatus)
	{
		ZW_ERROR("INPUT Point are NULL\n");
		return ERR_SDK_INVALID_ARGUMENT;
	}	
	nRet = MP4Sys_GetSecretPinStatus(pResetStatus);
	
	return nRet;
}

#ifdef __ONE_CLICK_CALL__
int ZWSys_SetPauseAudioAlarm()
{
    int nRet = SDK_SUCESS;

    nRet = MP4Sys_SetPauseAudioAlarm();

    return nRet;
}

int ZWSys_SetAlternateResetButton(int enable)
{
    int nRet = SDK_SUCESS;

    nRet = MP4Sys_SetAlternateResetButton(enable);

    return nRet;
}

int ZWSys_SetKeyToCall(int enable)
{
    int nRet = SDK_SUCESS;
    nRet = MP4Sys_SetKeyToCall(enable);

    return nRet;
}

int ZWSys_SetKeyToCallHardwareType(int type)
{
    return MP4Sys_SetKeyToCallHardwareType(type);
}

int ZWSys_GetCallPinStatus(int *pCallStatus, int *pHangupStatus)
{
    if (NULL == pCallStatus || NULL == pHangupStatus)
    {
        return ERR_SDK_INVALID_ARGUMENT;
    }

    return MP4Sys_GetCallPinStatus(pCallStatus, pHangupStatus);
}

int ZWSys_GetCallPinStatus_Test(int *pCallStatus, int *pHangupStatus)
{
    if (NULL == pCallStatus || NULL == pHangupStatus)
    {
        return ERR_SDK_INVALID_ARGUMENT;
    }

    return MP4Sys_GetCallPinStatus_Test(pCallStatus, pHangupStatus);
}

int ZWSys_SetCallPinStatus_Test(int pCallStatus, int pHangupStatus)
{
    return MP4Sys_SetCallPinStatus_Test(pCallStatus, pHangupStatus);
}
#endif

int     ZWSys_StartSysStatusLed(void *hChannelHandle, int freq)
{
	CChannelContext    *pChannelContext = (CChannelContext*)hChannelHandle;
	
	ZW_DEBUG("=============into ZWSys_StartSysStatusLed===========\n");

    MP4Sys_StartSysStatusLed(pChannelContext, freq);
	pChannelContext->m_SysStarted = 1;

	return SDK_SUCESS;
}

int     ZWSys_Set485WriteOrRead(int rw)
{
    return SDK_SUCESS;
}

int     ZWSys_GetTotalEncChannels( )
{
	int	TotalNum = 0;
	
	TotalNum = MP4Sys_GetTotalEncChannels();
	
	return TotalNum;
}

void*   ZWSys_EncChannelOpen(int ChannelNum)
{
	CChannelContext	*pChannelContext;

	pChannelContext = MP4Sys_EncChannelOpen(ChannelNum);
	
	return (void *)pChannelContext;
}

int     ZWSys_EncChannelClose(void *hChannelHandle)
{
	MP4Sys_EncChannelClose(hChannelHandle);
	
	return SDK_SUCESS;
}

int     ZWSys_EncSetCurrentTime( )
{
	int nRet = SDK_SUCESS;
	
	nRet = MP4Sys_EncSetCurrentTime();

	return nRet;
}

int     ZWSys_EncSetCurrentTimeEx(int tNewTime)
{
	int nRet = SDK_SUCESS;
	
	nRet = MP4Sys_EncSetCurrentTimeEx(tNewTime);

	return nRet;
}

int 	ZWSys_EncRegisterStreamReadCallback(STREAM_READ_CALLBACK StreamReadCallback)
{
	ZW_DEBUG("=============into ZWSys_EncRegisterStreamReadCallback===========\n");

	// 参数检查 
	int nRet = SDK_SUCESS;
	
	nRet = MP4Sys_EncRegisterStreamReadCallback(StreamReadCallback);
	
	return nRet;
}

int 	ZWSys_EncStartVideoCapture(void *hChannelHandle)
{
	ZW_DEBUG("=============into ZWSys_EncStartVideoCapture===========\n");

	int nRet = SDK_SUCESS;
	
	nRet = MP4Sys_EncStartVideoCapture(hChannelHandle);

	return nRet;
}

//stop stream and MD
int 	ZWSys_EncStopVideoCapture(void *hChannelHandle)
{
	int nRet = SDK_SUCESS;
	
	ZW_DEBUG("=============into ZWSys_EncStopVideoCapture===========\n");

	nRet = MP4Sys_EncStopVideoCapture(hChannelHandle);
	
	return nRet;
}

int 	ZWSys_EncReadStreamData(void *hChannelHandle, char **pDataBuf, 
	int *Length, int *FrameType, unsigned int *RealTime_Sec, unsigned int *RealTime_USec, int *nSensorChn)
{
	int nRet = SDK_SUCESS;
	
	int nStreamType = STREAM_CALLBACK_MAIN;

	//1 need get callback type	
	nRet = MP4Sys_EncReadStreamData(hChannelHandle,pDataBuf,Length,FrameType,RealTime_Sec,RealTime_USec,&nStreamType);
#if (SYSTEM_SUPPORT_CHANNEL <=1)
		*nSensorChn = FIRST_SENSOR;
#endif
#if (SYSTEM_SUPPORT_CHANNEL >= 2)
			if (nStreamType >= STREAM_CALLBACK_MAIN_SEC && nStreamType <= STREAM_CALLBACK_SNAP_SEC)
			{
				*nSensorChn = SEC_SENSOR;
			}
			else if (nStreamType == STREAM_CALLBACK_HUMANSHAPE_SEC 
				||nStreamType == STREAM_CALLBACK_MOTION_SEC
				|| nStreamType == STREAM_CALLBACK_MJPEG_SEC)
			{
				*nSensorChn = SEC_SENSOR;
			}
#endif
#if (SYSTEM_SUPPORT_CHANNEL >= 3)
			else if (nStreamType >= STREAM_CALLBACK_MAIN_THIRD && nStreamType <= STREAM_CALLBACK_SNAP_THIRD)
			{
				*nSensorChn = THIRD_SENSOR;
			}
			 else if (nStreamType == STREAM_CALLBACK_HUMANSHAPE_THIRD
				||nStreamType == STREAM_CALLBACK_MOTION_THIRD
				||nStreamType == STREAM_CALLBACK_MJPEG_THIRD)
			{
				*nSensorChn = THIRD_SENSOR;
			}
#endif
#if (SYSTEM_SUPPORT_CHANNEL >= 4)
			else if (nStreamType >= STREAM_CALLBACK_MAIN_FOURTH && nStreamType <= STREAM_CALLBACK_SNAP_FOURTH)
			{
				*nSensorChn = FOURTH_SENSOR;
			}
			else if (nStreamType == STREAM_CALLBACK_HUMANSHAPE_FOURTH 
				||nStreamType == STREAM_CALLBACK_MOTION_FOURTH
				||nStreamType == STREAM_CALLBACK_MJPEG_FOURTH)
			{
				*nSensorChn = FOURTH_SENSOR;
			}
#endif
#if (SYSTEM_SUPPORT_CHANNEL >= 2)
			else
			{
				*nSensorChn = FIRST_SENSOR;
			}
#endif
	return nRet;
}

int 	ZWSys_EncForceIFrame(void *hChannelHandle, int nSensorIndex)
{
	int nRet = SDK_SUCESS;

	ZW_DEBUG("=============into ZWSys_EncForceIFrame nSensorIndex[%d]===========\n",nSensorIndex);
	nRet = MP4Sys_EncCaptureIFrame(hChannelHandle, nSensorIndex);
	
	return nRet;
}

int 	ZWSys_EncSetStreamType(void *hChannelHandle, unsigned short Type)
{
	int nRet = SDK_SUCESS;

	ZW_DEBUG("=============into ZWSys_EncSetStreamType===========\n");
	nRet = MP4Sys_EncSetStreamType(hChannelHandle, Type);
	
    return nRet;
}
int 	ZWSys_EncGetStreamType(void *hChannelHandle, unsigned short *StreamType)
{
	int nRet = SDK_SUCESS;
	
	ZW_DEBUG("=============into ZWSys_EncGetStreamType===========\n");
	
	nRet = MP4Sys_EncGetStreamType(hChannelHandle,StreamType);
	
	return nRet;
}

//Bit rate, Frame rate, GOP length, IDR intervals, force IDR, Intra refresh MB rows for infinite GOP,
//QP range for H.264 codec, ROI 这是参数是可以运行中改变的
//bAux: 0主码流 ，1高清子码流，2MJPEG流，3子码流
int 	ZWSys_EncSetCodecPara(void *hChannelHandle, int bAux, CODEC_PARA_S *pCodecPara, EncodeType_t encType)
{
	int nRet= SDK_SUCESS;
	
	nRet = MP4Sys_EncSetCodecPara(hChannelHandle,bAux,pCodecPara,encType);

	return nRet;
}

int 	ZWSys_EncSetCodecParaExt(void *hChannelHandle, int stream, CODEC_PARA_S_EXT *pCodecPara)
{
	int nRet= SDK_SUCESS;
	
	nRet = MP4Sys_EncSetCodecParaExt(hChannelHandle,stream,pCodecPara);
	
	return nRet;
}

int		ZWSys_EncPTZSetIrcutPassivity(void *hChannelHandle, int dayNight)
{
	int nRet = SDK_SUCESS;

	ZW_DEBUG("=============into ZWSys_EncPTZSetIrcutPassivity===========\n");

	nRet = MP4Sys_EncPTZSetIrcutPassivity(hChannelHandle, dayNight);

	return	nRet;
}

/* 更新PTZ状态*/
int ZWSys_UpdatePtzStatus(void *hChannelHandle, ePtzStatus enStatus)
{
    int nRet = 0;
    nRet = MP4Sys_UpdatePtzStatus(hChannelHandle, enStatus);
    return nRet;
}

int ZWSys_SetCalibrationStatue(int nStatue)
{
    int ret = SDK_SUCESS;
    ZW_DEBUG("===into ZWSys_SetCalibrationStatue===Open %d\n", nStatue);
    //ret = MP4Sys_SetCalibrationStatue(nStatue);
    return ret;
}

/*跟踪是否回到了看守位*/
int		ZWSys_PtzForTrackBacktoPreset(void *hChannelHandle, ePtz2PresetStatus toPresetStat)
{
	int nRet = 0;
	
    ZW_DEBUG("===into ZWSys_PtzForTrackBacktoPreset===toPresetStat %d\n", toPresetStat);
	nRet = MP4Sys_PtzForTrackBacktoPreset(hChannelHandle, toPresetStat);
	
	return nRet;
}

/***
 * @description: 获取当前得日夜状态
 * @param {void} *hChannelHandle 通道句柄
 * @param {int} isDay 白天1，夜晚0
 * @return {int} 0:成功 <0:失败
 */
int		ZWsys_EncGetDayNightStatus(void *hChannelHandle, int *isDay)
{
	//ZW_DEBUG("=============into ZWsys_EncGetDayNightStatus===========\n");

	return MP4Sys_EncGetDayNightStatus(hChannelHandle, isDay);
}

int     ZWSys_SetColorNightCfg(void *hChannelHandle, int lastTime)
{
	int nRet = SDK_SUCESS;

	//ZW_DEBUG("=============into ZWSys_SetColorNightCfg===========\n");
	nRet = MP4Sys_SetColorNightCfg(hChannelHandle, lastTime);

	return nRet;
}

int ZWSys_EncSetNightLedMode(NightLedCommand_T NightLedFlag)
{
	int nRet = SDK_SUCESS;

	ZW_DEBUG("=============into ZWSys_EncSetNightLedMode===========\n");

	nRet = MP4Sys_EncSetNightLedMode(NightLedFlag);

	return	nRet;
}

int ZWSys_EncSetNightLedDuration(int LedDuration)
{
	int nRet = SDK_SUCESS;

	ZW_DEBUG("=============into ZWSys_EncSetNightLedDuration===========\n");

	nRet = MP4Sys_EncSetNightLedDuration(LedDuration);

	return	nRet;
}


int ZWSys_SetColorNightThreshold(void *hChannelHandle, int threshold)
{
	ZW_DEBUG("=============into ZWSys_SetColorNightThreshold===========\n");

	return MP4Sys_SetColorNightThreshold(hChannelHandle, threshold);
}

int ZWSys_SetIntelligentThreshold(void *hChannelHandle, int threshold)
{
	ZW_DEBUG("=============into ZWSys_SetIntelligentThreshold===========\n");

	return MP4Sys_SetIntelligentThreshold(hChannelHandle, threshold);
}

int ZWSys_SetInfraredThreshold(void *hChannelHandle, int threshold)
{
	ZW_DEBUG("=============into ZWSys_SetInfraredThreshold===========\n");

	return MP4Sys_SetInfraredThreshold(hChannelHandle, threshold);
}

int ZWSys_SetInfraredWorkMode(void *hChannelHandle, int mode)
{
	ZW_DEBUG("=============into ZWSys_SetInfraredThreshold===========\n");

	return MP4Sys_SetInfraredWorkMode(hChannelHandle, mode);
}

int ZWSys_SetInfraredBrightness(void *hChannelHandle, int Brightness)
{
	ZW_DEBUG("=============into ZWSys_SetInfraredThreshold===========\n");

	return MP4Sys_SetInfraredBrightness(hChannelHandle, Brightness);
}

int ZWSys_SetWhiteLightThreshold(void *hChannelHandle, int threshold)
{
	ZW_DEBUG("=============into ZWSys_SetWhiteLightThreshold===========\n");
	
	return MP4Sys_SetWhiteLightThreshold(hChannelHandle, threshold);
}

int ZWSys_SetNormalWhiteLightThreshold(void *hChannelHandle, int C2Blkthreshold, int Blk2Cthreshold)
{
	ZW_DEBUG("=============into ZWSys_SetNormalWhiteLightThreshold===========\n");
	
	return MP4Sys_SetNormalWhiteLightThreshold(hChannelHandle, C2Blkthreshold, Blk2Cthreshold);
}

int ZWSys_SetImageMode(void *hChannelHandle, int mode)
{
	ZW_DEBUG("=============into ZWSys_SetImageMode===========mode=[%d]\n", mode);
	
	return MP4Sys_SetImageMode(hChannelHandle, mode);
}

int ZWSys_SetDefogStrength(void *hChannelHandle, int strength)
{
	ZW_DEBUG("=============into ZWSys_SetDefogStrength===========strength=[%d]\n", strength);
	
	return MP4Sys_SetDefogStrength(hChannelHandle, strength);
}

int ZWSys_SetAntiFlickerStrength(void *hChannelHandle, int strength)
{
	ZW_DEBUG("=============into ZWSys_SetAntiFlashStrength===========strength=[%d]\n", strength);
	
	return MP4Sys_SetAntiFlickerStrength(hChannelHandle, strength);
}

int ZWSys_EncGetCodecPara(void *hChannelHandle, int bAux, CODEC_PARA_S * pCodecPara)
{
    return MP4Sys_EncGetCodecPara(hChannelHandle, bAux, pCodecPara);
}

int 	ZWSys_EncStartAuxCodec(void *hChannelHandle)
{
	int nRet = SDK_SUCESS;
	
	ZW_DEBUG("=============into ZWSys_EncStartAuxCodec===========\n");

 	nRet = MP4Sys_EncStartAuxCodec(hChannelHandle);

	return nRet;
}
	
int 	ZWSys_EncStopAuxCodec(void *hChannelHandle)
{
	int nRet = SDK_SUCESS;

	ZW_DEBUG("=============into ZWSys_EncStopAuxCodec===========\n");
	nRet = MP4Sys_EncStopAuxCodec(hChannelHandle);

	return nRet;
}

int     ZWSys_EncSetOsdText(void *hChannelHandle, TextOSDInfo_t *pOSDTextInfo)
{
    ZW_DEBUG("=============into ZWSys_EncSetOsdText===========\n");

    int    nRet = SDK_SUCESS;    
    SystemInfo_t  *pSysInfo = NULL;

    HY_Res_GetSystemInfo(&pSysInfo);

    /* 授权成功才允许设置OSD，否则一直显示"未授权"字符 */
    if (TRUE == pSysInfo->m_bAuthEncryptOK)
    {
        nRet = MP4Sys_EncSetOsdText(hChannelHandle,pOSDTextInfo);
    }
    else
    {
        nRet = MP4Sys_SetNoAuthInfoOSDDisplay();
    }

    return nRet;
}

int ZWSys_EncSetOsdTextEx(void *hChannelHandle, TextOSDInfoEx_t *pOSDTextInfoEx)
{
	ZW_DEBUG("=============into ZWSys_EncSetOsdTextEx===========\n");

	int	nRet =SDK_SUCESS;
    SystemInfo_t  *pSysInfo = NULL;

    HY_Res_GetSystemInfo(&pSysInfo);
    
    /* 授权成功才允许设置OSD，否则一直显示"未授权"字符 */
    if (TRUE == pSysInfo->m_bAuthEncryptOK)
    {
        nRet = MP4Sys_EncSetOsdTextEx(hChannelHandle, pOSDTextInfoEx);
    }
    else
    {
        nRet = MP4Sys_SetNoAuthInfoOSDDisplay();
    }

	return nRet;
}

int ZWSys_EncSetOsdV2Text(void *hChannelHandle, TextOSDV2Info_t *pOSDTextV2Info, int index)
{
	ZW_DEBUG("=============into ZWSys_EncSetOsdV2Text, index = %d ===========\n", index);

    if (index != 0)
    {
        return 0;
    }

    int	nRet = SDK_SUCESS;
    SystemInfo_t *pSysInfo = NULL;

    HY_Res_GetSystemInfo(&pSysInfo);
    
    /* 授权成功才允许设置OSD，否则一直显示"未授权"字符 */
    if (TRUE == pSysInfo->m_bAuthEncryptOK)
    {
        nRet = MP4Sys_EncSetOsdV2Text(hChannelHandle, pOSDTextV2Info);
    }
    else
    {
        nRet = MP4Sys_SetNoAuthInfoOSDDisplay();
    }

	return nRet;
}

int 	ZWSys_EncSetOsdTime(void *hChannelHandle, TimeOSDInfo_t *pOSDTimeInfo)
{
    int	nRet = SDK_SUCESS;
	CChannelContext 	*pChCtx = (CChannelContext*)hChannelHandle;
	ZW_DEBUG("=============into ZWSys_EncSetOsdTime=====bHour24 %d======\n", pOSDTimeInfo->bHour24);

	nRet = MP4Sys_EncSetOsdTime(pChCtx, pOSDTimeInfo);

	return nRet;
}

int 	ZWSys_EncSetOsd(void *hChannelHandle, int EnableText1, int EnableTime, int EnableText)
{
 	int nRet = SDK_SUCESS;

	ZW_DEBUG("=============into ZWSys_EncSetOsd===========\n");

	nRet = MP4Sys_EncSetOsd(hChannelHandle,EnableTime,EnableText);
	
    return nRet;
}

//人形跟踪使能开关
int 	ZWSys_EncSetHumanTrackEn(void *hChannelHandle, int bEnable)
{
	int nRet = SDK_SUCESS;

	ZW_DEBUG("=============into ZWSys_EncSetHumanTrackEn===========\n");
	nRet = MP4Sys_EncSetHumanTrackEn(hChannelHandle, bEnable);

	return nRet;
}

//人形检测使能开关
int 	ZWSys_EncSetPedestrianFiltering(void *hChannelHandle, int bEnable)
{
	int nRet = SDK_SUCESS;

	ZW_DEBUG("=============into ZWSys_EncSetPedestrianFiltering===========\n");
	nRet = MP4Sys_EncSetHumanDetectEnable(hChannelHandle, bEnable);

	return nRet;
}

int ZWSys_EncSetHumanShapeCfgV2(void *hChannelHandle, LpHumanShapeCfgV2 pCfg)
{
	int nRet = SDK_SUCESS;
	ZW_DEBUG("=============into ZWSys_EncSetHumanShapeCfgV2===========\n");
	nRet = MP4Sys_EncSetHumanShapeCfgV2(hChannelHandle, pCfg);
	
	return nRet;
}

//用于是否画框
int 	ZWSys_EncSetDetectSchedule(void *hChannelHandle, int enable)
{
	int nRet = SDK_SUCESS;
	
	ZW_DEBUG("=============into ZWSys_EncSetDetectSchedule===========\n");
	nRet = MP4Sys_EncSetDetectSchedule(hChannelHandle, enable);
	
	return nRet;
}

//人形布防配置
int 	ZWSys_EncSetHumanShapeSchedule(void *hChannelHandle, LpAiScehdule Schedule)
{
	int nRet = SDK_SUCESS;
	
	ZW_DEBUG("=============into ZWSys_EncSetHumanShapeSchedule===========\n");
	nRet = MP4Sys_EncSetHumanShapeSchedule(hChannelHandle, Schedule);
	
	return nRet;
}

//人形叠加信息处理
int 	ZWSys_SetHumanOverlayInfo(void *hChannelHandle, char nMainStreamHumanOverlay, char nSubStreamHumanOverlay)
{
	int nRet = SDK_SUCESS;
	
	ZW_DEBUG("=============into ZWSys_SetHumanOverlayInfo===========\n");
	nRet = MP4Sys_SetHumanOverlayInfo(hChannelHandle, nMainStreamHumanOverlay, nSubStreamHumanOverlay);
	
	return nRet;
}

int 	ZWSys_EncSetupMotionDetection( void *hChannelHandle, MotionDetectGridToi_t *pMotionDetectToiInfo)
{
	int nRet = SDK_SUCESS;
	
	ZW_DEBUG("=============into ZWSys_EncSetupMotionDetection==nBlockNum %d==threshold %d==sensitivity %d=====\n",
		pMotionDetectToiInfo->nBlockNum,pMotionDetectToiInfo->threshold,pMotionDetectToiInfo->sensitivity);

	//1 note
	nRet = MP4Sys_EncSetupMotionDetection(hChannelHandle, pMotionDetectToiInfo->listOfAreas);
	MP4Sys_EncAdjustMotionDetectPrecision(hChannelHandle, pMotionDetectToiInfo->sensitivity, 4);

	return nRet;
}

// 移动侦测配置
int 	ZWSys_EncSetMotionSchedule(void *hChannelHandle, LpAiScehdule Schedule)
{
	int nRet = SDK_SUCESS;
	
	ZW_DEBUG("=============into ZWSys_EncSetMotionSchedule===========\n");
	nRet = MP4Sys_EncSetMotionSchedule(hChannelHandle, Schedule);
	
	return nRet;
}

int 	ZWSys_EncStartMotionDetection(void *hChannelHandle)
{
	int nRet = SDK_SUCESS;

	ZW_DEBUG("=============into ZWSys_EncStartMotionDetection===========\n");

	nRet = MP4Sys_EncStartMotionDetection(hChannelHandle);

	return nRet;
}


//清除用户的设置，下次需要重新初始化
int 	ZWSys_EncStopMotionDetection(void *hChannelHandle)
{
	int nRet = SDK_SUCESS;

	ZW_DEBUG("=============into ZWSys_EncStopMotionDetection===========\n");
	nRet = MP4Sys_EncStopMotionDetection(hChannelHandle);

	return nRet;
}
int 	ZWSys_EncAdjustMotionDetectPrecision(void *hChannelHandle, int iGrade, int iMotionDetectFps)
{

	int nRet = SDK_SUCESS;
	ZW_DEBUG("=============into ZWSys_EncAdjustMotionDetectPrecision===========\n");
	return 0;	//don't need
	
	nRet = MP4Sys_EncAdjustMotionDetectPrecision(hChannelHandle,iGrade,iMotionDetectFps);

	return nRet;
}

int 	ZWSys_AlarmOutSupport(int nSupport)
{
	return MP4Sys_AlarmOutSupport(nSupport);
}

int 	ZWSys_AlarmOutDefaultStatus(int nStatus)
{
	return MP4Sys_AlarmOutDefaultStatus(nStatus);
}

int 	ZWSys_AlarmOut (long nPort, int bOpen)
{
	int nRet = SDK_SUCESS;

	ZW_DEBUG("===into ZWSys_AlarmOut==Port %ld==Open %d\n", nPort, bOpen);
	nRet = MP4Sys_AlarmOut(nPort,bOpen);

	return nRet;
}

int 	ZWSys_LightAlarm(int nMode, int bOpen)
{
	int nRet = SDK_SUCESS;

	//ZW_DEBUG("===into ZWSys_LightAlarm==Mode %d==Open %d\n", nMode, bOpen);
	nRet = MP4Sys_LightAlarm(nMode, bOpen);

	return nRet;
}

int 	ZWSys_AlarmIn(int *pAlarmIndata )
{
	int nRet = SDK_SUCESS;
	
	//MP4Sys_AlarmIn(pAlarmIndata);

	return nRet;
}

int 	ZWSys_SetAudioChannel(AudioChannelType_t channel)
{
	int nRet = SDK_SUCESS;
	
	ZW_DEBUG("=============into ZWSys_SetAudioChannel===========\n");
	nRet = MP4Sys_SetAudioInMode(channel);
	
	return nRet;
}

int 	ZWSys_SetAudioCodec(AudioEncodeMode_t ACodec)
{
	int nRet = SDK_SUCESS;
	ZW_DEBUG("=============into ZWSys_SetAudioCodec===========\n");
	nRet = MP4Sys_SetAudioCodec(ACodec);
	
	return nRet;
}

int 	ZWSys_GetAudioEncodeMode(AudioEncodeMode_t *audioEncodeMode)
{
	SystemInfo_t		*pSysInfo;
	ZW_DEBUG("=============into ZWSys_GetAudioEncodeMode===========\n");

	// 接口加锁 
	if (HY_Res_SDK_Lock() != 0)
	{
		ZW_ERROR("%s : SDK Not Init\n", __FUNCTION__);
		return ERR_SDK_NOINIT;
	}

	HY_Res_GetSystemInfo(&pSysInfo);

	if(pSysInfo->m_TotalAudioEncChannel <= 0)
	{
		ZW_ERROR("%s : m_TotalAudioEncChannel = %d\n", __FUNCTION__, pSysInfo->m_TotalAudioEncChannel);
		HY_Res_SDK_UnLock();
		return ERR_SDK_NOT_SUPPORT;
	}

	*audioEncodeMode = eCodecG711U;

	HY_Res_SDK_UnLock();						  //释放接口锁
	
	return SDK_SUCESS;
}

//-----------------------------------------------------------------------------
//	说明：		设置语音对讲模式(支持互信互通所添加的接口)
//	输入参数：	Mode		见AudioEncodeMode_t
//	返回：		成功为 0， 其它为定义的错误号；
//-----------------------------------------------------------------------------
int 	ZWSys_SetTalkingMode(int Mode)
{
	int nRet =SDK_SUCESS;
	
	ZW_DEBUG("=============into ZWSys_SetTalkingMode===========\n");
	nRet = MP4Sys_SetTalkingMode(Mode);
	
	return nRet;
}

//启动对讲
int 	ZWSys_SetTalkingEnable(int bEnable)
{
	int nRet = SDK_SUCESS;
	
	ZW_DEBUG("=============into ZWSys_SetTalkingEnable====bEnable %d=======\n",bEnable);
	nRet = MP4Sys_SetTalkingEnable(bEnable);

	return nRet;
}

//数据传出
int 	ZWSys_ReadTalkingData(char *DataBuf, unsigned int* BufSize)
{
    int nRet = SDK_SUCESS;
	
//	ZW_DEBUG("=============into ZWSys_ReadTalkingData===========\n");
	nRet = MP4Sys_ReadTalkingData(DataBuf,BufSize);

    return nRet;
}

//数据传入
int 	ZWSys_SetTalkingData(char *DataBuf, unsigned int DataSize)
{
	int nRet = SDK_SUCESS;
	//ZW_DEBUG("=============into ZWSys_SetTalkingData===========\n");
	nRet = MP4Sys_SetTalkingData(DataBuf,DataSize);

    return nRet;
}

int 	ZWSys_RegisterTalkingReadCallback(TALKING_READ_CALLBACK TalkingReadCallback)
{
    int nRet = SDK_SUCESS;

	ZW_DEBUG("=============into ZWSys_RegisterTalkingReadCallback===========\n");
	nRet = MP4Sys_RegisterTalkingReadCallback(TalkingReadCallback);

    return nRet;
}

int 	ZWSys_SetSerialDataCallBack(SERIALDATA_READ_CALLBACK SerailDataReadCallBack)
{
	int nRet = SDK_SUCESS;
	
	nRet = MP4Sys_SetSerialDataCallBack(SerailDataReadCallBack);
		
	return nRet;
}

int 	ZWSys_SetSerialDataWriteCallBack(SERIALDATA_WRITE_CALLBACK SerailDataWriteCallBack)
{
	int nRet = SDK_SUCESS;

	//nRet = MP4Sys_SetSerialDataWriteCallBack(SerailDataWriteCallBack);
	
	return nRet;
}

int ZWSys_SetSerialCfg(PTZParam_t *pParam)
{
	return SDK_SUCESS;
}

int ZWSys_GetSensorType(BoardSensorType_t *SensorType)
{ 
	int nRet = SDK_SUCESS;

	//ZW_DEBUG("=============into ZWSys_GetSensorType===========\n");
	nRet = MP4Sys_GetSensorType(SensorType);

    return nRet;
}

int ZWSys_GetCVBSOutStandard(VideoOutStandard_t *VideoStandard)
{
    *VideoStandard = StandardPAL;
     return 0;
}

//需进入空闲状态设置
int ZWSys_SetCVBSOutStandard(void *hChannelHandle, VideoOutStandard_t VideoStandard)
{
	return 0;
}

int 	ZWSys_EncSetVideoPara(void *hChannelHandle, int Brightness, 
		int Contrast, int Saturation, int Hue, int Sharpness)
{
	int nRet = SDK_SUCESS;

	ZW_DEBUG("=============into ZWSys_EncSetVideoPara===========\n");
	nRet = MP4Sys_EncSetVideoPara(hChannelHandle,Brightness,Contrast,Saturation,Hue,Sharpness);

	return  nRet;
}

int ZWSys_EncSetAutoChangeBright(void *hChannelHandle,  int enable)
{
	return	SDK_SUCESS;
}

int		ZWSys_EncSetBacklightMode(void *hChannelHandle,  int enable)
{
	int nRet = SDK_SUCESS;
	ZW_DEBUG("=============into ZWSys_EncSetBacklightMode===========\n");
	nRet = MP4Sys_EncSetBacklightMode(hChannelHandle,enable);

	return	nRet;
}

int ZWSys_EncSetIrcutDirection(void *hChannelHandle,  int mode)
{
	int nRet = SDK_SUCESS;

	ZW_DEBUG("=============into ZWSys_EncSetIrcutMode===========\n");

	nRet = MP4Sys_EncSetIrcutDirection(hChannelHandle,mode);

	return	nRet;
}

/* 设置IRcut切换时间*/
int		ZWSys_EncSetDayNightSwitchTime(void *hChannelHandle,  int time)
{
	int nRet = SDK_SUCESS;
	nRet = MP4Sys_EncSetDayNightSwitchTime(hChannelHandle,time);

	return	nRet;
}

int		ZWSys_EncSetDayNightInitiativeSwitchTime(void *hChannelHandle,  int time)
{
	int nRet = SDK_SUCESS;
	nRet = MP4Sys_EncSetDayNightInitiativeSwitchTime(hChannelHandle,time);

	return	nRet;
}

int		ZWSys_EncSetDayNightMode(void *hChannelHandle, DAYNIGHTMODE_T mode, LP_IrcutTimeModeInfo pIrcutTimeModeInfo)
{
	int nRet = SDK_SUCESS;

	ZW_DEBUG("=============into ZWSys_EncSetDayNightMode===========\n");

	nRet = MP4Sys_EncSetDayNightMode(hChannelHandle,mode,pIrcutTimeModeInfo);

	return	nRet;
}

int     ZWSys_EncSetVarWhiteMode(void *hChannelHandle, NIGHTVISIONWORKMODE_T mode, NIGHTVISIONCUSTOMMODE_T manualMode, int bright)
{
	int nRet = SDK_SUCESS;

	ZW_DEBUG("=============into ZWSys_EncSetVarWhiteMode===========\n");

	nRet = MP4Sys_EncSetVarWhiteMode(hChannelHandle, mode, manualMode, bright);

	return	nRet;
}

int     ZWSys_EncSetVarWhiteModeInfo(void *hChannelHandle, LP_IrcutNightVisionTimeInfo pVarWhiteModeInfo, int nightThr, int dayThr)
{
	int nRet = SDK_SUCESS;

	ZW_DEBUG("=============into ZWSys_EncSetVarWhiteModeInfo===========\n");

	nRet = MP4Sys_EncSetVarWhiteModeInfo(hChannelHandle, pVarWhiteModeInfo, nightThr, dayThr);

	return	nRet;
}

int ZWSys_EncSetColorNightMode(void *hChannelHandle, NIGHTVISIONWORKMODE_T mode, LP_IrcutNightVisionTimeInfo pColorNightModeInfo)
{
	int nRet = SDK_SUCESS;

	ZW_DEBUG("=============into ZWSys_EncSetColorNightMode===========\n");

	nRet = MP4Sys_EncSetColorNightMode(hChannelHandle, mode, pColorNightModeInfo);

	return	nRet;

}

//设置ircut被动模式的灵敏度
int	ZWSys_EncSetIrCut(void *hChannelHandle,  int nDayNightSensity)
{
	int nRet = SDK_SUCESS;

	ZW_DEBUG("=============into ZWSys_EncSetIrCut=====nDayNightSensity %d======\n", nDayNightSensity);

	nRet = MP4Sys_EncSetIrCut(hChannelHandle,nDayNightSensity);

	return	nRet;
}

/*设置日夜自动(主动模式)的灵敏度*/
int	ZWSys_EncSetIrcutInitiativeModeSensity(void *hChannelHandle,  int Sensity)
{
	int nRet = SDK_SUCESS;

	ZW_DEBUG("=============into ZWSys_EncSetIrcutInitiativeModeSensity=====Sensity %d======\n", Sensity);

	nRet = MP4Sys_EncSetIrcutInitiativeModeSensity(hChannelHandle,Sensity);

	return	nRet;
}
// 获取ircut 自检状态
int ZWSys_GetIrcutCheckStatus(int *ircutCheckStatus)
{
	if(ircutCheckStatus == NULL)
	{
		ZW_ERROR("=============ZWSys_GetIrcutCheckStatus ircutCheckStatus null===========\n");
		return -1;
	}
	int nRet = 0;
	nRet = HY_Enc_GetIRCutCheckStatus(ircutCheckStatus);
	if(nRet)
	{
		ZW_ERROR("=============ZWSys_GetIrcutCheckStatus HY_Enc_GetIRCutCheckStatus fail===========\n");
		return -1;
	}
	return 0;
}
//img_awb_set_cali_diff_thr控制白平衡是否启动cali_awb.c
int   ZWSys_SetWhiteBalance(void *hChannelHandle, WHITEBALANCE_T *pWhiteBalance)
{
	int nRet = SDK_SUCESS;
	
	ZW_DEBUG("=============into ZWSys_SetWhiteBalance===========\n");
	nRet = MP4Sys_SetWhiteBalance(hChannelHandle,pWhiteBalance);

	return nRet;
}

int ZWSys_SetAEParam(void *hChannelHandle, AE_T *pAeParam)
{
 	int nRet = SDK_SUCESS;
 	
	ZW_DEBUG("=============into ZWSys_SetAEParam===========\n");
	nRet = MP4Sys_SetAEParam(hChannelHandle,pAeParam);

	return nRet;
}

int ZWSys_SetAudioInVol(void *hChannelHandle, int vol)
{
	int nRet = SDK_SUCESS;
	
	ZW_DEBUG("=============into ZWSys_SetAudioInVol===========\n");
	nRet = MP4Sys_SetAudioInVol(hChannelHandle,vol);

	return nRet;
}

int ZWSys_SetAudioOutVol(void *hChannelHandle, int vol, int IsUpdateVoiceCache)
{
	int nRet = SDK_SUCESS;
	
	ZW_DEBUG("=============into ZWSys_SetAudioOutVol===========\n");
	nRet = MP4Sys_SetAudioOutVol(hChannelHandle, vol, IsUpdateVoiceCache, 0);

	return nRet;
}

int ZWSys_SetAudioMappingVol(void *hChannelHandle, int vol, int Type)
{
	int nRet = SDK_SUCESS;
	
	ZW_DEBUG("=============into ZWSys_SetAudioOutVol===========\n");
	nRet = MP4Sys_SetAudioMappingVol(hChannelHandle, vol, Type);

	return nRet;
}


int ZWSys_GetRealKeyFrameInterval(int nChannel,int *nFrameRateInterval, int *nKeyFrameInterval)
{ 
	int nRet =SDK_SUCESS;

	//ZW_DEBUG("=============into ZWSys_GetSensorType===========\n");
	nRet = MP4Sys_GetRealKeyFrameInterval(nChannel, nFrameRateInterval, nKeyFrameInterval);
	return nRet;
}

int ZWSys_EncGetIntercomMode( int *iIntercomMode )
{
	int nRet = SDK_SUCESS;

	ZW_DEBUG("=============into ZWSys_EncGetIntercomMode===========\n");

	nRet = MP4Sys_EncGetIntercomMode( iIntercomMode);

	return	nRet;
}

int ZWSys_EncSetIntercomMode( int iIntercomMode )
{
	int nRet = SDK_SUCESS;

	ZW_DEBUG("=============into ZWSys_EncSetIntercomMode===========\n");

	nRet = MP4Sys_EncSetIntercomMode( iIntercomMode);

	return	nRet;
}

//wdr 数字宽动态
int ZWSys_SetLocalExpMode(void *hChannelHandle, int ExpMode)
{
	int nRet = SDK_SUCESS;
	
	ZW_DEBUG("=============into ZWSys_SetLocalExpMode===========\n");
	nRet = MP4Sys_SetLocalExpMode(hChannelHandle,ExpMode);

	return nRet;
}

// 3D降噪
int ZWSys_SetMCTFStrength(void *hChannelHandle, int MCTFStrength)
{
	int nRet = SDK_SUCESS;
	
	ZW_DEBUG("=============into ZWSys_SetMCTFStrength===========\n");
	nRet = MP4Sys_SetMCTFStrength(hChannelHandle,MCTFStrength);

	return nRet;
}

int ZWSys_SetLensOrientation(unsigned int nLensOrientation)
{
	int ret = 0;
    ret = MP4Sys_SetLensOrientation(nLensOrientation);
    return ret;
}

int ZWSys_SetFlipRotateMode(void *hChannelHandle, FLIP_ROTATE_MODE_E flipRotate)
{
	int nRet = SDK_SUCESS;
	
	ZW_DEBUG("=============into ZWSys_SetFlipRotateMode======flipRotate %d=====\n", flipRotate);
	nRet = MP4Sys_SetFlipRotateMode(hChannelHandle,flipRotate);

	return nRet;
}

//pc tv
int ZWSys_SetChromaScale(void *hChannelHandle, CHROMA_SCALE_E chromaScale)
{
	int nRet = SDK_SUCESS;
	
	ZW_DEBUG("=============into ZWSys_SetChromaScale===========\n");

	nRet = MP4Sys_SetChromaScale(hChannelHandle,chromaScale);

	return nRet;
}

int ZWSys_SetColorType(void *hChannelHandle, color_type_E type)
{
	ZW_DEBUG("=============into ZWSys_SetColorType==type:%d=========\n", type);

	return	SDK_SUCESS;
}

#ifdef __AGINGTEST__
int ZWSys_SetForceNightStatus(void *hChannelHandle, int enable)
{
	int nRet = SDK_SUCESS;

	ZW_DEBUG("=============into ZWSys_SetForceNightStatus===========\n");

	nRet = MP4Sys_SetForceNightStatus(hChannelHandle, enable);

	return	nRet;
}
#endif

int ZWSys_SetDcIris(void *hChannelHandle, int enable)
{
	int nRet = SDK_SUCESS;
	
	ZW_DEBUG("=============into ZWSys_SetDcIris===========\n");

	nRet = MP4Sys_SetDcIris(hChannelHandle,enable);

	return nRet;
}

int ZWSys_SetDcIrisDuty(void *hChannelHandle, int duty)
{
	int nRet = SDK_SUCESS;
	
	ZW_DEBUG("=============into ZWSys_SetDcIrisDuty===========\n");
	nRet = MP4Sys_SetDcIrisDuty(hChannelHandle,duty);

	return nRet;
}

int ZWSys_SetRGB2YUVMatrix(void *hChannelHandle, int isDefault, LP_IPC_TONE_RGB2YUV dataArray)
{
	return 0;
}

int ZWSys_GetWifiExist(int *exist)
{
    if(NULL == exist)
    {
        ZW_ERROR("param error\n");
        return ERR_INVALID_ARGUMENT;
    }

    if (HY_Res_SDK_Check() != 0)
    {
        ZW_ERROR("SDK Not Init\n");
        return ERR_SDK_NOINIT;
    }

    SystemInfo_t *pSysInfo = NULL;

	HY_Res_GetSystemInfo(&pSysInfo);

    *exist = pSysInfo->m_bSupportWifi;

    return SDK_SUCESS;
}

int ZWSys_GetRGB2YUVMatrix(void *hChannelHandle, LP_IPC_TONE_RGB2YUV dataArray)
{
	return 0;
}

int ZWSys_SetTuneToneCurve(void *hChannelHandle, int isDefault, const char *toneCurveFile)
{
	return 0;
}

int ZWSys_EnableDspMemToUpgrade(void *hChannelHandle, int *dspAddr, int *length, int bEnable)
{
	ZW_DEBUG("=============into ZWSys_EnableDspMemToUpgrade===========\n");
	return 0;
}

int ZWSys_EncSetSnapPara(void *hChannelHandle, int nFormat, int nQuality)
{
	int nRet = SDK_SUCESS;
	
	ZW_DEBUG("=============into ZWSys_EncSetSnapPara===========\n");
	nRet = MP4Sys_EncSetSnapPara(hChannelHandle,(ResolutionParam_t *)nFormat, nQuality);

	return nRet;
}

int ZWSys_EncSetSnapParaV2(void *hChannelHandle, int nQuality, char nStreamType)
{
	int nRet = SDK_SUCESS;
	
	ZW_DEBUG("=============into ZWSys_EncSetSnapParaV2===========\n");

	nRet = MP4Sys_EncSetSnapParaV2(hChannelHandle, nQuality, nStreamType);

	return nRet;
}

/* 直接从加密设备里面读取版型，不需要初始化SDK */
int     ZWSys_GetBoardTypeNoSDK(char *pInfo)
{
	int nRet = SDK_SUCESS;
	
	ZW_DEBUG("=============into ZWSys_GetBoardTypeNoSDK===========\n");
	nRet = MP4Sys_GetBoardTypeNoSDK(pInfo);

	return nRet;
}

/* 设置授权信息，成功返回授权错误码，失败返回-1 */
int ZWSys_SetAuthInfo(const unsigned char *pszAuthLicense)
{
    return MP4Sys_SetAuthInfo(pszAuthLicense);
}

/* 获取授权码和授权状态，成功返回0，失败返回-1 */
int ZWSys_GetAuthInfo(unsigned char *pszAuthLicense, unsigned int *pAuthState)
{
    return MP4Sys_GetAuthInfo(pszAuthLicense, pAuthState);
}

/* 移除(取消)授权，直接返回REMOVE_AUTH_SUCCESS */
int ZWSys_RemoveAuthLicense(void)
{
    return MP4Sys_RemoveAuthLicense();
}

/* 设置激活状态码(AF和双光源) */
int ZWSys_SetActiveStatusCode(int nActiveStatusCode)
{
	int ret = 0;    

	ret = MP4Sys_SetActiveStatusCode(nActiveStatusCode);
    
	return ret;
}

/* 获取激活状态码(AF和双光源) */
int ZWSys_GetActiveStatusCode(int *pbActiveStatusCode)
{
	int ret = 0;

	ret = MP4Sys_GetActiveStatusCode(pbActiveStatusCode);

	return ret;
}

int ZWSys_SetAlarmLightStayTime(int nEnable, int nStayTime)
{
	int nRet = SDK_SUCESS;
	
	ZW_DEBUG("===into ZWSys_SetAlarmLightStayTime===nStayTime %d\n", nStayTime);
	nRet = MP4Sys_SetAlarmLightStayTime(nEnable, nStayTime);

	return nRet;

}

int ZWSys_SetAlarmLightStart()
{
	int nRet = SDK_SUCESS;
	
	ZW_DEBUG("===into ZWSys_SetAlarmLightStart===Open\n");
	nRet = MP4Sys_SetAlarmLightStart();

	return nRet;
}

int ZWSys_SetSpeechContent(SpeechType_T speech)
{
	return MP4Sys_SetSpeechContent(speech);
}

int ZWSys_SetSpeechLanType(eSpeechLanguage speech_language)
{
    return MP4Sys_SetSpeechLanType(speech_language);
}

int ZWSys_LightBoardType(int nLightType)
{
    return MP4Sys_LightBoardType(nLightType);
}

int ZWSys_SetAlarmSpeechCfg(void *hChannelHandle, SpeechAlarmInfo_t *speechCfg)
{
	return MP4Sys_SetAlarmSpeechCfg(hChannelHandle, speechCfg);
}

int ZWSys_SetRemindSpeechCfg(void *hChannelHandle, int TypeIdx, SpeechRemindInfo_t *speechCfg)
{
	return MP4Sys_SetRemindSpeechCfg(hChannelHandle, TypeIdx, speechCfg);
}

int ZWSys_SetRemindHangupState(int iState)
{
    return MP4Sys_SetRemindHangupState(iState);
}

int ZWSys_SetAudioCustomVoice(void *hChannelHandle, int customtype, char *pBuff, int len)
{
	return MP4Sys_SetAudioCustomVoice(hChannelHandle, customtype, pBuff, len);
}

int ZWSys_GetBitrateInfo(void *hChannelHandle, int dataArr[], int arrlen)
{
	return MP4Sys_GetBitrateInfo(hChannelHandle, dataArr, arrlen);
}

/* 设置按键状态获取回调函数 */
int ZWSys_RegisterKeyStautsCallBack(KEY_STATUS_CALLBACK getKeyStateCallback)
{
    return MP4Sys_RegisterKeyStautsCallBack(getKeyStateCallback);
}

#ifdef __SUPPORT_WIFI_FUNC__
//0 - 连接 ,1-断开
int ZWSys_SetLinkStatus(int status)
{
    int ret = 0;
    ret = MP4Sys_SetLinkStatus(status);

    return ret;
}

int ZWSys_SetWifiStatusLed(WIFI_STATUS_T wifiStatus)
{
    int nRet = SDK_SUCESS;

    nRet = MP4Sys_SetWifiStatusLed(wifiStatus);

    return nRet;
}
#endif

int ZWSys_EnableQrScan(int bEnable)
{
    int nRet = SDK_SUCESS;
    // ZW_DEBUG("=============into ZWSys_EnableQrScan===========\n");
    nRet = MP4Sys_EnableQrScan(bEnable);
    return nRet;
}

int ZWSys_RegisterQrScanCallback(QR_SCAN_CALLBACK pCallback)
{
    int nRet = SDK_SUCESS;
    ZW_DEBUG("=============into ZWSys_RegisterQrScanCallback===========\n");
    nRet = MP4Sys_RegisterQrScanCallback(pCallback);
    return nRet;
}

int ZWSys_RegisterMotoSavePosCallback(Moto_SavePos_CALLBACK pCallback)
{
    int nRet = SDK_SUCESS;
    ZW_DEBUG("=============into ZWSys_RegisterMotoSavePosCallback===========\n");
    nRet = MP4Sys_RegisterMotoSavePosCallback(pCallback);
    return nRet;
}


/*
	status : 当前网络是无线还是有线还是异常状态 0:网络异常 1：无线状态 2：有线状态
*/
int ZWSys_SetCurrNetCardType(XRS_NETCARD_TYPE_E status)
{
	if (status < eXRS_NETCARD_WIRED_Unknown || status > eXRS_NETCARD_5G)
	{
		ZW_ERROR("ZWSys_GetWlanNetStatus fail\n");
		return -1;
	}

	if (status)
	{
    	MP4Sys_SetCurrNetCardType(status);
	}
	else
    {
		ZW_ERROR("CurrNetCardType is unknown!\n");
		return -1;
	}

	return 0;
}


int ZWSys_GetSpeechPlayingStatus(void)
{
	return MP4Sys_GetSpeechPlayingStatus();
}

//-----------------------------------------------------------------------------
//	说明：		设备外设控制
//	输入参数：AssistantCtrlInfo	pCtrlInfo	控制信息
//	返回:		成功为 0 , 其它为定义的错误号；
//-----------------------------------------------------------------------------
int ZWSys_AssistantControl(AssistantCtrlInfo *pCtrlInfo)
{
    int nRet = SDK_SUCESS;
    ZW_DEBUG("=============into ZWSys_AssistantControl===========\n");
    nRet = MP4Sys_AssistantControl(pCtrlInfo);
    return nRet;
}

int ZWSys_GetAdcStatus(int *nAdcStatus)
{
	int nRet = SDK_SUCESS;

    nRet = MP4Sys_GetAdcStatus(nAdcStatus);

    return nRet;
}

int ZWSys_GetUniqueID(char *uniqueid, int len)
{
    return MP4Sys_GetUniqueID(uniqueid, len);
}

/**
 * @description: 设置系统启动状态标志
 * @param {int} Stat 启动状态标记
 * @return {int} 正常返回0，异常返回<0
 */
int ZWSys_SetSysStartTag(int Stat)
{
	return MP4Sys_SetSysStartTag(Stat);
}

/**
 * @description: 初始化KS HalMod模块
 * @return {int} 正常返回0，异常返回<0
 */
int ZWSys_HalModInit()
{
	return MP4Sys_HalModInit();
}

/**
 * @description: 反初始化KS HalMod
 * @return {int} 正常返回0，异常返回<0
 */
int ZWSys_HalModDeInit()
{
	return MP4Sys_HalModDeInit();
}

int ZWSys_SetDormancyState(int iState)
{
	int nRet = SDK_SUCESS;

	nRet = MP4Sys_SetDormancyState(iState);
	
	return nRet;
}

int ZWSys_EncSetSlowFrameMode( int iSlowFrameMode)
{
	int nRet = SDK_SUCESS;

	//ZW_DEBUG("=============into ZWSys_EncSetSlowFrameMode===========\n");

	nRet = MP4Sys_EncSetSlowFrameMode(iSlowFrameMode);

	return	nRet;
}

//设置灯板高低开模式
int ZWSys_SetLightOpenStatus(int Status)
{
    int ret = 0;

    ret = MP4Sys_SetLightOpenStatus(Status);

    return ret;
}

int ZWSys_SetProductMode(int nProductMode)
{
    int nRet = SDK_SUCESS;

    nRet = MP4Sys_SetProductMode(nProductMode);

    return nRet;
}

//设置声音侦测配置
int ZWSys_SetAudioDetectCfg(void *hChannelHandle, LpAudioDetectCfg pCfg)
{
    int nRet = SDK_SUCESS;
    
    ZW_DEBUG("=============into %s===========\n", __FUNCTION__);
    nRet = MP4Sys_SetAudioDetectCfg(hChannelHandle, pCfg);

    return nRet;
}

//注册声音侦测报警回调
int ZWSys_RegisterAudioDetectCallback(AudioDetect_CALLBACK pCallback, void *UserData)
{
    int nRet = SDK_SUCESS;
    
    ZW_DEBUG("=============into %s===========\n", __FUNCTION__);
    nRet = MP4Sys_RegisterAudioDetectCallback(pCallback, UserData);

    return nRet;
}

int ZWSys_SwitchAecMode(int AecMode)
{
	int nRet = SDK_SUCESS;

	nRet = MP4Sys_SwitchAecMode(AecMode);

    return nRet;
}

int ZWSys_SysInitSensorCnt()
{
    return MP4Sys_SysInitSensorCnt();
}

int ZWSys_GetMaxSensorCnt()
{
    return MP4Sys_GetMaxSensorCnt();
}

int ZWSys_GetSensorConfCorrect(int CheckCnt)
{
    return MP4Sys_GetSensorConfCorrect(CheckCnt);
}

int ZWSys_SetRecoveryAudioAlarm()
{
    int nRet = SDK_SUCESS;

    nRet = MP4Sys_SetRecoveryAudioAlarm();

    return nRet;
}

//人形跟踪回调
int ZWSys_HumanTrackerCallBack(HumanTracker_CALLBACK pFn_HumanTracker)
{
	int nRet = SDK_SUCESS;

	nRet = MP4Sys_HumanTrackerCallBack(pFn_HumanTracker);

	return	nRet;
}

int ZWSys_GetWiFiInfo(DEV_WIFI_INFO_S *wifiInfo)
{
    if (wifiInfo == NULL)
    {
        ZW_ERROR("input para error\n");
        return -1;
    }
    
    if (MP4Sys_GetWiFiInfo(wifiInfo))
    {
        ZW_ERROR("=============ZWSys_GetWiFiInfo get wifi info error===========\n");
        return -1;
    }
    return 0;
}

int ZWSys_P2PNotifySendDataFail(const char * pBuffer, unsigned int dwBufLen)
{
    return MP4Sys_P2PNotifySendDataFail(pBuffer, dwBufLen);
}

#ifdef __MODULE_4G_SUPPORT__
/* 设置4G模组的类型 */
int ZWSys_SetModuleType(e4GModuleType moduletype)
{
    return MP4Sys_SetModuleType(moduletype);
}

/* 设置云卡的支持使能 */
int ZWSys_SetCloudCardmode(int CloudCardmode)
{
    return MP4Sys_SetCloudCardmode(CloudCardmode);
}



//设置4G运营商的类型
int ZWSys_Set4GTelecomOperatorType(e4GTelecomOperatorType telecomOperatorType)
{
    return MP4Sys_Set4GTelecomOperatorType(telecomOperatorType);
}

int ZWSys_GetModuleType(e4GModuleType *moduletype)
{
    return MP4Sys_GetModuleType(moduletype);
}

//复位4G模组
int ZWSys_4GModuleReset()
{
	return MP4Sys_4GModuleReset();
}

//判断当前是否是运营号段 返回值 1：是运营号段  0：非运营号段 -1:异常
int ZWSys_CheckDidIsFlowOpreate(char *pDid)
{
	int i = 0;
	//流量运营平台
	char *pFlowOpreatePlatform[] = 
	{
		FORWARD_4G_NUMBER,
		FORWARD_4G_NUMBER1,
		FORWARD_4G_NUMBER2,
		FORWARD_4G_NUMBER3,
		FORWARD_4G_NUMBER4
	};

	if (NULL == pDid)
	{
		return -1;
	}

	for (i = 0;i < sizeof(pFlowOpreatePlatform) / sizeof(char *);i++)
	{
		if(0 == strncmp(pDid,pFlowOpreatePlatform[i],strlen(pFlowOpreatePlatform[i])))
		{
			//有一个匹配即可
			return 1;
		}
	}

	return 0;
}


//判断当前是否支持的号段 返回值 1：支持  0：不支持 -1:异常
int ZWSys_CheckDidIsSupport(char *pDid)
{
	int i = 0;
	//流量运营平台
	char *pFlowOpreatePlatform[] = 
	{
	    FORWARD_4G_NUMBER,
		FORWARD_4G_NUMBER1,
		FORWARD_4G_NUMBER2,
		FORWARD_4G_NUMBER3,
		FORWARD_4G_NUMBER4,
        SUPPORT_4G_IOTF,
        SUPPORT_4G_YLTB,
        SUPPORT_4G_IOTH
	};

	if (NULL == pDid)
	{
		return -1;
	}

	for (i = 0;i < sizeof(pFlowOpreatePlatform) / sizeof(char *);i++)
	{
		if(0 == strncmp(pDid,pFlowOpreatePlatform[i],strlen(pFlowOpreatePlatform[i])))
		{
			//有一个匹配即可
			return 1;
		}
	}

	return 0;
}



//根据号段获取客户类型 nCustomer: 客户类型
int ZWSys_DidForCustomerType(char *pDid,int *nCustomer)
{
	int custom = CUSTOMTYPE_NOTFLOW;

	if (NULL == pDid)
	{
		return -1;
	}

	if (0 == strncmp(pDid,FORWARD_4G_NUMBER,strlen(FORWARD_4G_NUMBER)))
	{
		custom = CUSTOMTYPE_XINRUISHI;
	}
	else if (0 == strncmp(pDid,FORWARD_4G_NUMBER1,strlen(FORWARD_4G_NUMBER1)))
	{
		custom = CUSTOMTYPE_SUNYIYANG;
	}
	else if (0 == strncmp(pDid,FORWARD_4G_NUMBER2,strlen(FORWARD_4G_NUMBER2)))
	{
		custom = CUSTOMTYPE_YUNRUI;
	}
	else if (0 == strncmp(pDid,FORWARD_4G_NUMBER3,strlen(FORWARD_4G_NUMBER3)))
	{
		custom = CUSTOMTYPE_LONGTU;
	}
	else if (0 == strncmp(pDid,FORWARD_4G_NUMBER4,strlen(FORWARD_4G_NUMBER4)))
	{
		custom = CUSTOMTYPE_XINRUISHI;
	}
#if defined(__SUPPORT_COMMON_DID_OPREATE__)
	else
	{
		custom = CUSTOMTYPE_XINRUISHI_NORMAL;
	}
#endif

	*nCustomer = custom;

	return 0;
}

// 设置了IOTDBB 号段的DID，未激活运营模式，显示 “未激活”（GB2312）
int ZWSys_SetNoActiveOperModeOSDDisplay(void)
{
	return 0;
}

//当前网卡是否是4G
int ZWSys_Get4GSupport(int *bSupport)
{
    if (NULL == bSupport)
    {
        ZW_ERROR("param error\n");
        return ERR_INVALID_ARGUMENT;
    }

    *bSupport = 1;

    return SDK_SUCESS;
}

//当前网卡是否是4G,不管是否识别到wifi
int ZWSys_Get4GSupportV2(int *bSupport)
{
    if (NULL == bSupport)
    {
        ZW_ERROR("param error\n");
        return ERR_INVALID_ARGUMENT;
    }

    *bSupport = 1;
    return SDK_SUCESS;
}

//获取是否支持绑定后播报语音
int ZWSys_GetBroadCastEnable(int *nEnable)
{
	return MP4Sys_GetBroadCastEnable(nEnable);
}

#endif


#if defined(__DIGITAL_ZOOM_DRAG_SUPPORT__)
int ZWSys_SetDragZoomRatio(double ratio,int eOpreator)
{
    int nRet = SDK_SUCESS;
    nRet = MP4Sys_SetDragZoomRatio(ratio,eOpreator);
    return nRet;
}

int ZWSys_GetDragZoomRatio(double *ratio)
{
    return MP4Sys_GetDragZoomRatio(ratio);
}

#if defined(__IS_SUPPORT_LONG_SHORT_FOCUS_ZOOM__)
int ZWSys_GetEachFocusZoom(double *shortZoom, double *middleZoom, double *longZoom)
{
    return MP4Sys_GetEachFocusZoom(shortZoom, middleZoom, longZoom);
}
#endif

int ZWSys_SetDragZoomCmd(DRAG_ZOOM_CMD nCmd,int eOpreator)
{
    int nRet = SDK_SUCESS;
    nRet = MP4Sys_SetDragZoomCmd(nCmd,eOpreator);
    return nRet;
}

#if defined __IS_SUPPORT_LONG_SHORT_FOCUS_ZOOM__
int ZWSys_SetZoomLens(int nLens)
{
    int nRet = SDK_SUCESS;
    nRet = MP4Sys_SetZoomLens(nLens);
    return nRet;
}

int ZWSys_GetCurrentSensor()
{
    int CurrentSensor = MP4Sys_GetCurrentSensor();
    return CurrentSensor;
}
#endif

double ZWSys_GetCurRatio()
{
	double CurRatio = 0;
	CurRatio = MP4Sys_GetCurRatio();
	return CurRatio;
}

int ZWSys_GetDragZoomChgStatus(int *status)
{
    int nRet = SDK_SUCESS;
    nRet = MP4Sys_GetDragZoomChgStatus(status);
    return nRet;
}

int ZWSys_SetDragZoomScaleMagnification(double ratio)
{
    int nRet = SDK_SUCESS;
    nRet = MP4Sys_SetDragZoomScaleMagnification(ratio);
    return nRet;
}

int ZWSys_GetDragZoomRange(double *min,double *max)
{
    int nRet = SDK_SUCESS;
    nRet = MP4Sys_GetDragZoomRange(min,max);
    return nRet;
}


//获取实际最大倍数
int ZWSys_GetRealMaxRatio(double *ratio)
{
	CChannelContext 	*pChannelContext = NULL;
	HY_Enc_GetEncChInfo(&pChannelContext);
#if defined(__PRODUCT_G85T__) || defined(__PRODUCT_W85T__)
	*ratio = 27;
#elif defined(__PRODUCT_G86T__)
	*ratio = 54;
#elif  defined(__PRODUCT_G43TA__)
	*ratio = 12;
#else
	*ratio = DRAG_ZOOM_MAX_RATIO;
#endif

    return SDK_SUCESS;
}

#ifdef __ONE_CLICK_CALL__
int ZWSys_SetKey2callhanupState(int iState)
{
    int nRet = SDK_SUCESS;

    nRet = MP4Sys_SetKey2callhanupState(iState);

    return nRet;
}

int ZWSys_GetKey2callDuSpeechStatus(int *status)
{
    int nRet = SDK_SUCESS;

    nRet = MP4Sys_GetKey2callDuSpeechStatus(status);

    return nRet;
}
#endif

//获取送检时真实倍数
int ZWSys_GetCheckRealRatio(double *curRatio)
{
	SystemInfo_t *pSysInfo = NULL;
	HY_Res_GetSystemInfo(&pSysInfo);

	if (NULL == curRatio)
	{
		ZW_ERROR("curRatio is NULL\n");
		return -1;
	}

	*curRatio = pSysInfo->m_CurRatio;

    return SDK_SUCESS;
}
#endif

#if defined(__IS_SUPPORT_LCD_MANAGER__)
int ZWSys_LCDRefreshFrame(char *pData, unsigned int DataSize)
{
    return MP4Sys_LCDRefreshFrame(pData, DataSize);
}

int ZWSys_LCDSetBackLightLevel(int level)
{
    return MP4Sys_LCDSetBackLightLevel(level);
}

int ZWSys_LCDCreateVdecChn(int nType)
{
    return MP4Sys_LCDCreateVdecChn(nType);
}

int ZWSys_LCDSendStreamToChn(int nType, char *pData, unsigned int DataSize)
{
    return MP4Sys_LCDSendStreamToChn(nType, pData, DataSize);
}

int ZWSys_LCDDestoryVdecChn(int nType)
{
    return MP4Sys_LCDDestoryVdecChn(nType);
}
#endif

//SD卡下电
int ZWSys_DownSdPower()
{
	return MP4Sys_DownSdPower();
}

/* 设置左右球/枪的sensor通道交换支持标志 */
int ZWSys_SetSptSwapLRPTZChannel(int bEnable)
{
    int nRet = SDK_SUCESS;
    nRet = MP4Sys_SetSptSwapLRPTZChannel(bEnable);

    return nRet;
}

/* 获取左右球/枪的sensor通道交换支持标志 */
int ZWSys_GetSptSwapLRPTZChannel(int *pbEnable)
{
    int nRet = SDK_SUCESS;
    nRet = MP4Sys_GetSptSwapLRPTZChannel(pbEnable);

    return nRet;
}


