
/*!
 *\file	 MPU_CIU_Config_Dev.cpp
 *\brief	MPU模块，远程配置，设备信息
 *\author	林云
 *\date	2009/8/17

功能：
-#	设备参数
-#	系统信息

说明：
-#	本文件使用新的网络库实现
 */

#include "MPU_CIU_global.h"
#include "PUB/lattice2bmp24.h"
#include "Authority.h"
#include "common_pub.h"
#include "CProManager.h"
#include "../PUB/PUB_Language.h"
#include "../PAR/CPubFunc.h"
#include "AVL_global.h"
#include "PUB_BoradInfo.h"
#include "MPU_LCD_API.h"

static BOOL g_bSetAuthorize = FALSE;

static int (*CBStandardChange) (int newVideoStandard) = NULL;

static int m_AccessMainStream = 0;      //主sensor主码流预览流数量
static int m_AccessSecondStream = 0;    //主sensor次码流预览流数量
static int m_AccessMainStreamS1 = 0;    //次sensor主码流预览流数量
static int m_AccessSecondStreamS1 = 0;  //次sensor次码流预览流数量
static int m_AccessMainStreamS2 = 0;    //次sensor主码流预览流数量
static int m_AccessSecondStreamS2 = 0;  //次sensor次码流预览流数量
static int m_AccessMainStreamS3 = 0;    //次sensor主码流预览流数量
static int m_AccessSecondStreamS3 = 0;  //次sensor次码流预览流数量

#if defined (__IS_SUPPORT_APP_SHARE__)
static pthread_mutex_t g_scode_mutex = PTHREAD_MUTEX_INITIALIZER; // 2同步互斥锁
#endif

static HY_DVR_ASSISTANT_INTERFACE s_stDevAssistants[MAX_ASSISSTANT_INTERFACE_NUM] = 
{
    {1, 1, eWhiteLight, 0},
    {1, 1, eBrustLight, 0},
    {1, 1, eInfraredLight, 0},
    {0, 1, ePilotLight, 0},
    {0, 1, eErrorAssistantInterfaceType, 0},
    {0, 1, eErrorAssistantInterfaceType, 0},
    {0, 1, eErrorAssistantInterfaceType, 0},
    {0, 1, eErrorAssistantInterfaceType, 0},
};

#ifdef __IS_SUPPORT_CLOUDWORLD__
#define CLOUD_STORAGE_MODE_DOMESTIC             0
#define CLOUD_STORAGE_MODE_FOREIGN              1
#define CLOUD_STORAGE_MODE_UNKNOW  	            2
#endif

void CIU_SetStandardChangeCallback(int (*pStandardChangeCallBack) (int newVideoStandard))
{
	CBStandardChange = pStandardChangeCallBack;
}
static int GetDevAlias(LPHY_DVR_ALIAS_CFG_1 pcfg);

static int SetDevAlias(LPHY_DVR_ALIAS_CFG_1 pcfg);

//获取设备语言
static int GetDevLanguage(HY_DVR_SYSTEM_LANGUAGE_CONFIG *pcfg)
{
	pcfg->nValid = 1;
	eSpeechLanguage eSpLan = eSpeechLanguage_Chinese;
	pcfg->nLanguage = eLanguage_Chinese;

    // 获取当前语音语言
	if(0 == PUB_GetLanguageFromCfg(&eSpLan))
	{
		PUB_changetoHiLanguage(eSpLan,&(pcfg->nLanguage));
	}

	MPU_ERR("pcfg->nLanguage = %d\n",pcfg->nLanguage);
	return 0;
}

//获取设备夏令时配置
static int GetDevDST(tagHY_DVR_DST_INFO *pcfg)
{
	int nRet = 0;
	int DstBias = 0;
	pcfg->bValid = 1;
	pcfg->isInDST = MPU_SYS_IsEnableDST();
	if(pcfg->isInDST == 1)
	{
		DstBias = MPU_SYS_GetDstBias();
		nRet = MPU_SYS_GetDstIsWork();
		if(nRet)
		{
			pcfg->dstOffset = DstBias / 60;
		}
		else
		{
			pcfg->dstOffset = 0;
		}
	}
	else
	{
		pcfg->dstOffset = 0;
	}
	MPU_DBG("pcfg->isInDST = %d,pcfg->dstOffset = %d,DstIsWork:%d\n",pcfg->isInDST,pcfg->dstOffset,nRet);
	return 0;
}


//获取设备能力、版本信息，需外部将pcfg清零
static int GetCustomInfo(LPHY_DVR_CUSTOMER_INFO pcfg)
{
	int nRet = IDVR_OK, nSDKRet;

	nSDKRet = MPU_PUB_GetCustomeFunction(pcfg);
	MPU_CHK(0 == nSDKRet);

	return nRet;
}

static int GetDeviceInfoV2(LPHY_DVR_DEVICE_INFO_V2 pcfg)
{
	pcfg->bValid = 1;
	pcfg->struSerial.nValid = 1;
	pcfg->struSerial.nSerialCustom = 1;			
	ZWSys_GetSerialNum((unsigned char *)pcfg->struSerial.szSerialNum);		

	return 0;	
}

static int SetCustomInfo(LPHY_DVR_CUSTOMER_INFO pcfg)
{
	int nRet = IDVR_OK, nSDKRet;

	nSDKRet = MPU_PUB_SetCustomeFunction(pcfg);
	MPU_CHK(0 == nSDKRet);

	return nRet;
}

//获取安装信息
static int GetDevInstallInfo(ZW_MULTI_CHANNEL_INSTALLTION_INFO *pcfg)
{
	int nRet = IDVR_OK;

	return nRet;
}

//设置安装信息
static int SetDevInstallInfo(ZW_MULTI_CHANNEL_INSTALLTION_INFO *pcfg)
{
    int nRet = IDVR_OK;

    return nRet;
}

//获取设备能力、版本信息，需外部将pcfg清零
static int GetDevInfo(LPHY_DVR_DEVICE_INFO pcfg)
{
	int nRet = IDVR_OK;
	int nSDKRet = IDVR_OK;
	DEVICEVERSION dv = {0};
	DEVICECAP dc = {0};

	int nBoardID = 0;

	static int not_frist=0;
	static HY_DVR_DEVICE_INFO cfginfo = {0}; 	

	//! 本结构体是否有效，=0表示无效
	pcfg->bValid = 1;

    /* 如果设置了授权功能，则需要重新从SDK获取状态(状态不需要重置，授权之后，系统会重启)     */
    if (TRUE == g_bSetAuthorize)
    {
        not_frist = 0;
    }

    /* 避免频繁获取配置 */
    if(1 == not_frist)
    {
        memcpy(pcfg, &cfginfo, sizeof(HY_DVR_DEVICE_INFO));
        return nRet;
    }

	//获取设备版本信息
	nSDKRet = MPU_PUB_GetDeviceVersion(&dv,0);
	MPU_CHK(0 == nSDKRet);

	//pcfg->dwSerialNum = dv.m_dwSerialNum;

	//!\brief	硬件版本
	strncpy(pcfg->szHardwareVersion, dv.m_szHardwareVersion, sizeof (pcfg->szHardwareVersion) - 1);

	//!\brief	软件版本
	strncpy(pcfg->szSoftwareVersion, dv.m_szSoftwareVersion, sizeof (pcfg->szSoftwareVersion) - 1);

	//!\brief	编码版本
	strncpy(pcfg->szDspVersion, dv.m_szDspVersion, sizeof (pcfg->szDspVersion) - 1);

    /* 序列号 */
    //  strncpy(pcfg->szSerialNum, (const char*)dv.m_szSerialNum, sizeof(dv.m_szSerialNum) - 1);
    ZWSys_GetSerialNum((unsigned char *)pcfg->szSerialNum);

	ZWSys_GetBoardInfo(&nBoardID);
	pcfg->dwBoardType = PUB_FindDeviceTypeInfoById(nBoardID); //根据以前代码实际返回的是deviceType
/*
//以前代码返回的实际是devicetype
	switch (nBoardID)
	{
		case 0x216D:
			pcfg->dwBoardType = 0x5184;
			break;
		default:
			pcfg->dwBoardType = 0x5159;
			break;
	}
*/
    /* 针对未授权的设备，设备类型显示为空或者"未授权的设备类型" */
    unsigned int nAuthState = 0;
    unsigned char szAuthCode[256] = {0};
    ZWSys_GetAuthInfo(szAuthCode, &nAuthState);
    if (0 == nAuthState)
    {
        pcfg->dwBoardType = 0xFFFFFFFF;
    }

	//获取设备能力
	nRet = MPU_PUB_GetDeviceCap(&dc);
	MPU_CHK(0 == nRet);

	//设备型谱名称，如HY-2016HC
	strncpy(pcfg->szTypeName, dc.m_szTypeName, sizeof (pcfg->szTypeName) - 1);

	pcfg->byVideoInChannels = dc.m_bytVideoInChannels;	/*视频输入路数 */
	pcfg->byAudioInChannels = dc.m_bytAudioInChannels;	/*音频输入路数 */
	pcfg->byAlarmInChannels = dc.m_bytAlarmInChannels;	/*报警输入路数 */
	pcfg->byAlarmOutChannels = dc.m_bytAlarmOutChannels;	/*报警输出路数 */

	pcfg->byVGAChannels = dc.m_bytVGAChannels;	/*VGA接口个数 */

	pcfg->bySpotOutChannles = dc.m_bytSpotOutChannles;	/*SPOT输出路数 */
	pcfg->byVoiceInChannels = dc.m_bytVoiceInChannels;	/*语音对讲输入路数 */

    if (0 == not_frist)
    {
        memcpy(&cfginfo, pcfg, sizeof(HY_DVR_DEVICE_INFO));
        not_frist = 1;
    }

	return nRet;
}

/* 从SDK获取唯一的序列号，上传给网络层 */
static int GetSeriesNum(char *pSeriesNum,int SerNumSize)
{
    unsigned int dwSerialNum = 0;
    
    ZWSys_GetSerialNum((unsigned char*)pSeriesNum);
    snprintf(pSeriesNum, SerNumSize, "%010u", dwSerialNum);  

    return 0;
}

//获取夏时制配置信息，需外部将pcfg清零
int GetDstCfg(LPHY_DVR_DST pcfg)
{
	int nRet = IDVR_OK, nSDKRet;

	DVR_DST_EX dst;

	//! 本结构体是否有效，=0表示无效
	pcfg->bValid = 1;

	//获取夏时制信息
	nSDKRet = MPU_SYS_GetDST(&dst);
	MPU_CHK(0 == nSDKRet);

	//!\brief   是否启用夏时制
	pcfg->bEnableDST = MPU_SYS_IsEnableDST();

	//开始时间，年无效
	pcfg->struStart.nYear = dst.start.year;
	pcfg->struStart.nMonth = dst.start.mon;
	pcfg->struStart.nDay = dst.start.day;
	pcfg->struStart.nHour = dst.start.hour;
	pcfg->struStart.nMinute = dst.start.min;
	pcfg->struStart.nSecond = dst.start.sec;

	//结束时间，年无效
	pcfg->struEnd.nYear = dst.end.year;
	pcfg->struEnd.nMonth = dst.end.mon;
	pcfg->struEnd.nDay = dst.end.day;
	pcfg->struEnd.nHour = dst.end.hour;
	pcfg->struEnd.nMinute = dst.end.min;
	pcfg->struEnd.nSecond = dst.end.sec;
	pcfg->dwMode = (eDSTTimeFormat)dst.dwMode;

	memcpy(&pcfg->struWeekModeStart,&dst.struWeekModeStart,sizeof(dst.struWeekModeStart));
	memcpy(&pcfg->struWeekModeStop,&dst.struWeekModeStop,sizeof(dst.struWeekModeStop));
	pcfg->struWeekModeStart.bValid =1 ;
	pcfg->struWeekModeStop.bValid =1 ;

	//调整多少时间，以秒为单位
	pcfg->nDSTBias = dst.byDSTBias;

	return nRet;
}
int GetDiskGroup(ZW_RECORD_ADVANCED_CFG *pcfg)
{
	int nRet = IDVR_OK;
    unsigned long keeptime = -1;
	tagDiskGroupInfo iDiskinfo;

	//! 本结构体是否有效，=0表示无效
	pcfg->bValid = 1;
	memset(&iDiskinfo, 0, sizeof(tagDiskGroupInfo));
	nRet = MPU_STR_DiskGroup_GetInfo(0, &iDiskinfo);
	MPU_CHK(0 == nRet);
	keeptime = iDiskinfo.nDataKeepTime;
	if(keeptime == 0x80000000)
	{
		pcfg->bOverwrite = 0;
	}
	else
	{
		pcfg->bOverwrite = 1;
	}

	pcfg->dwAudioInInterface = (eAudioInInterface)MPU_AVL_AudioIn_GetInterface();
	
	MPU_DBG("--GetDiskGroup--bOverwrite=%d--dwAudioInInterface=%d---\n",pcfg->bOverwrite,pcfg->dwAudioInInterface);

    pcfg->eAudioCodec = (eVoiceMode)MPU_AVL_AudioCodec_GetInterface();

    MPU_DBG("--GetDiskGroup--bOverwrite=%d--dwAudioInInterface=%d---\n",pcfg->bOverwrite,pcfg->eAudioCodec); 

/*	nWmode = MPU_STR_DiskGroup_GetWorkMode();
	pcfg->dwESATAWorkMode = (nWmode)?eESTATWorkMode_Snapshot:eESTATWorkMode_RecordOrBackup;
*/
	return nRet;
}

int SetDiskGroup(ZW_RECORD_ADVANCED_CFG *pcfg)
{
	int nRet = IDVR_OK;
    unsigned long keeptime = -1;
	tagDiskGroupInfo iDiskinfo;

	//! 本结构体是否有效，=0表示无效
	if (0 == pcfg->bValid)
	{
		MPU_DBG("0 == pcfg->bValid\n");
		return IDVR_CIU_STRUCT_UNVALID;
	}
	MPU_DBG("--SetDiskGroup--bOverwrite=%d--dwAudioInInterface=%d---\n",pcfg->bOverwrite,pcfg->dwAudioInInterface);
	memset(&iDiskinfo, 0, sizeof(tagDiskGroupInfo));
	nRet = MPU_STR_DiskGroup_GetInfo(0, &iDiskinfo);
	keeptime = iDiskinfo.nDataKeepTime;
	if(pcfg->bOverwrite == 1)
   	{
	   	//是循环写入
	   	if(keeptime == 0x80000000)
	   	{
	   		nRet= MPU_STR_DiskGroup_DataKeepTime(0, 0x7fffffff);
			MPU_CHK(0 == nRet);

		}
   	}
   	else
   	{
   		//不循环写入
	   	if(keeptime != 0x80000000)
	   	{
	   		nRet= MPU_STR_DiskGroup_DataKeepTime(0, 0x80000000);
			MPU_CHK(0 == nRet);

		}
   	}
	
	nRet= MPU_AVL_AudioIn_SetInterface((int)(pcfg->dwAudioInInterface));
	MPU_CHK(0 == nRet);

    nRet= MPU_AVL_AudioCodec_SetInterface((int)(pcfg->eAudioCodec));
	MPU_CHK(0 == nRet);
/*	
	nWmode = (pcfg->dwESATAWorkMode == eESTATWorkMode_Snapshot)?1:0;
	nRet = MPU_STR_DiskGroup_SetWorkMode(nWmode);
	MPU_CHK(0 == nRet);
*/
	return nRet;
}
//设置夏时制配置信息
int SetDstCfg(LPHY_DVR_DST pcfg)
{
	int nRet = IDVR_OK, nSDKRet;

	DVR_DST_EX dst;

	//! 本结构体是否有效，=0表示无效
	if (0 == pcfg->bValid)
	{
		MPU_DBG("0 == pcfg->bValid\n");
		return IDVR_CIU_STRUCT_UNVALID;
	}

	MPU_DBG("--SetDstCfg--bEnableDST=%d--start:%d-%d-%d %d:%d:%d--end:%d-%d-%d %d:%d:%d--nDSTBias=%d--dwMode=%d--" \
		"WeekModeStart:M %d W %d D %d H %d M %d S %d--WeekModeStop:M %d W %d D %d H %d M %d S %d--\n",
		pcfg->bEnableDST,
		pcfg->struStart.nYear,
		pcfg->struStart.nMonth,
		pcfg->struStart.nDay,
		pcfg->struStart.nHour,
		pcfg->struStart.nMinute,
		pcfg->struStart.nSecond,
		pcfg->struEnd.nYear,
		pcfg->struEnd.nMonth,
		pcfg->struEnd.nDay,
		pcfg->struEnd.nHour,
		pcfg->struEnd.nMinute,
		pcfg->struEnd.nSecond,
		pcfg->nDSTBias,
		pcfg->dwMode,
		pcfg->struWeekModeStart.bytStartMonth,
		pcfg->struWeekModeStart.bytWhichWeek,
		pcfg->struWeekModeStart.bytWeekDay,
		pcfg->struWeekModeStart.bytHour,
		pcfg->struWeekModeStart.bytMinute,
		pcfg->struWeekModeStart.bytSecond,
		pcfg->struWeekModeStop.bytStartMonth,
		pcfg->struWeekModeStop.bytWhichWeek,
		pcfg->struWeekModeStop.bytWeekDay,
		pcfg->struWeekModeStop.bytHour,
		pcfg->struWeekModeStop.bytMinute,
		pcfg->struWeekModeStop.bytSecond);
	//!\brief   是否启用夏时制
	//MPU_SYS_EnableDST(pcfg->bEnableDST);

	//开始时间，年无效
	dst.start.year = pcfg->struStart.nYear;
	dst.start.mon = pcfg->struStart.nMonth;
	dst.start.day = pcfg->struStart.nDay;
	dst.start.hour = pcfg->struStart.nHour;
	dst.start.min = pcfg->struStart.nMinute;
	dst.start.sec = pcfg->struStart.nSecond;

	//结束时间，年无效
	dst.end.year = pcfg->struEnd.nYear;
	dst.end.mon = pcfg->struEnd.nMonth;
	dst.end.day = pcfg->struEnd.nDay;
	dst.end.hour = pcfg->struEnd.nHour;
	dst.end.min = pcfg->struEnd.nMinute;
	dst.end.sec = pcfg->struEnd.nSecond;

	//调整多少时间，以秒为单位
	dst.byDSTBias = pcfg->nDSTBias;
	dst.dwMode = (eDSTTimeFormat_EX)pcfg->dwMode;
	memcpy(&dst.struWeekModeStart,&pcfg->struWeekModeStart,sizeof(dst.struWeekModeStart));
	memcpy(&dst.struWeekModeStop,&pcfg->struWeekModeStop,sizeof(dst.struWeekModeStop));

	//设置夏时制信息
	nSDKRet = MPU_SYS_SetDST(&dst);
	MPU_CHK(0 == nSDKRet);

	//!\brief   是否启用夏时制
	nSDKRet = MPU_SYS_EnableDST(pcfg->bEnableDST);
	MPU_CHK(0 == nSDKRet);

	return nRet;
}

//获取设备配置信息DvrID，需外部将pcfg清零
static int GetDevDvrID(ZW_SYSCOMMON_NORMAL_CFG * pcfg)
{
	int nSDKRet = IDVR_OK;
	DEVICEINFO di;
	
	//! 本结构体是否有效，=0表示无效
	pcfg->bValid = 1;

	/* DVR ID,用于遥控器 */
	pcfg->dwDeviceID= PUB_GetDvrID();

	//获取设备配置
	nSDKRet = MPU_PUB_GetDeviceInfo(&di);
	MPU_CHK(0 == nSDKRet);


	//!\brief   自动锁屏时间（单位为秒）
	pcfg->dwAutoLogoutDelayByMinute = di.m_nAutoLockScreenTime;

	//!\brief   语言
	pcfg->dwLanguage= di.m_nMenuLanguage;

	//视频输入制式
	nSDKRet = MPU_AVL_GetStandard((int *)&pcfg->dwVideoOutputStandard);
	MPU_CHK(0 == nSDKRet);

#ifdef ACTIVE_CODE_SUPPORT
        //激活码
        nSDKRet = MPU_AVL_GetActiveCode(pcfg->szActiveCode, sizeof(pcfg->szActiveCode),
            &pcfg->bSupportActiveCodeFlag,
            &pcfg->bActiveStatus,
            &pcfg->uSupportActivedFeatureMask,
            &pcfg->uAllActivedStatusMask);
        MPU_CHK(0 == nSDKRet);
        MPU_DBG("my add: MPU_AVL_GetActiveCode return %d\n",nSDKRet);
#endif

	//视频输出制式
	//pcfg->nVideoOutStandard = pcfg->nVideoInStandard;

	snprintf(pcfg->acDeviceTypeDesc,sizeof(pcfg->acDeviceTypeDesc),"%s", "IPC-XXX");
	//设备别名(unicode)
	GetDevAlias(&pcfg->strDeviceName);

	return nSDKRet;
}

/*设置设备配置信息DvrID*/
static int SetDevDvrID(ZW_SYSCOMMON_NORMAL_CFG * pcfg)
{
	int nSDKRet= IDVR_OK;
	DEVICEINFO di;

	//! 本结构体是否有效，=0表示无效
	if (0 == pcfg->bValid)
	{
		MPU_DBG("0 == pcfg->bValid\n");
		return IDVR_CIU_STRUCT_UNVALID;
	}

	nSDKRet = MPU_PUB_GetDeviceInfo(&di);
	MPU_CHK(0 == nSDKRet);
	MPU_DBG("my add: MPU_PUB_GetDeviceInfo return %d\n",nSDKRet);
	//!\brief   自动锁屏时间（单位为秒）
	di.m_nAutoLockScreenTime = pcfg->dwAutoLogoutDelayByMinute;

	//!\brief   语言
	di.m_nMenuLanguage = pcfg->dwLanguage;

	//设置设备配置
	nSDKRet = MPU_PUB_SetDeviceInfo(&di);
	MPU_CHK(0 == nSDKRet);
	MPU_DBG("my add: MPU_PUB_SetDeviceInfo return %d\n",nSDKRet);

	//视频输入制式
	nSDKRet = MPU_AVL_SetStandard(pcfg->dwVideoOutputStandard);
	MPU_CHK(0 == nSDKRet);
	MPU_DBG("my add: MPU_AVL_SetStandard return %d\n",nSDKRet);

	//设备别名
	nSDKRet = SetDevAlias(&pcfg->strDeviceName);
	MPU_DBG("my add: SetDevAlias return %d\n",nSDKRet);

#ifdef ACTIVE_CODE_SUPPORT
    if (pcfg->bSupportActiveCodeFlag)
    {
        //激活码
        nSDKRet = MPU_AVL_SetActiveCode(pcfg->szActiveCode, pcfg->unAllActivedFunctionMask);
        MPU_CHK(0 == nSDKRet);
        MPU_DBG("my add: MPU_AVL_SetActiveCode return %d\n",nSDKRet);
    }
#endif

	return nSDKRet;
}

static int GetNTP_Cfg(LPHY_DVR_NTP_CFG pcfg)
{
	int nRet = 0;
	//! 本结构体是否有效，=0表示无效
	pcfg->bValid = 1;
#if 1
	/*0-不启用, 1-启用 */
	pcfg->bNtpEnable = MPU_SYS_GetNtpEnable();

	/*Ntp服务器 */
	MPU_SYS_GetNtpServer(pcfg->szNtpHostAddress,sizeof(pcfg->szNtpHostAddress));

	//目前系统不支持配置
	/*Ntp服务器，端口 */
	//pcfg->wNtpHostPort;

	/*Ntp服务器同步间隔 */
	//pcfg->nAutoRegIntervals = MPU_NTP_GetAutoUpdateTime();
#endif
	return nRet;
}
//获取设备配置信息time zone，需外部将pcfg清零
static int GetDevTimeZone(ZW_SYSCOMMON_DATETIME_CFG * pcfg)
{
	int nRet = IDVR_OK;

	//! 本结构体是否有效，=0表示无效
	pcfg->bValid = 1;

	DEVICEINFO di;
	//获取设备配置
	nRet = MPU_PUB_GetDeviceInfo(&di);
	MPU_CHK(0 == nRet);
	//!\brief   日期格式
	pcfg->dwDateFormat = (eZWDateFormat)(di.m_nDateFormat);

	//!\brief   时间格式（12小时制，24小时制）
	pcfg->dwTimeFormat = (eZWTimeFormat)(di.m_nTimeFormat);

	//!\brief   时区时差
	//!\brief   时区时差
	pcfg->dwTimeZoneV2 = MPU_SYS_GetTimeZone();
	pcfg->dwEnableTimeZoneV2 = 1;
	pcfg->dwTimeZone = pcfg->dwTimeZoneV2 / 60;

	//!\brief	同步间隔
	pcfg->nTimeSyncInterval=MPU_SYS_GetNtpInterval();

	//!/brief  NTP
	GetNTP_Cfg(&pcfg->struNtp);
	
	struct tm tm;
	time_t now = time(NULL);
	localtime_r(&now, &tm);
	pcfg->struSysTime.nYear = tm.tm_year + 1900;
	pcfg->struSysTime.nMonth = tm.tm_mon + 1;
	pcfg->struSysTime.nDay = tm.tm_mday;
	pcfg->struSysTime.nHour = tm.tm_hour;
	pcfg->struSysTime.nMinute = tm.tm_min;
	pcfg->struSysTime.nSecond = tm.tm_sec;
	//MPU_DBG("TimeZone %d\n", pcfg->nTimeZone);

	return nRet;
}
static int SetNTP_Cfg(LPHY_DVR_NTP_CFG pcfg)
{
	int nRet = 0;

	//本结构体是否有效，=0表示无效
	if (0 == pcfg->bValid)
	{

		MPU_DBG("0 == pcfg->bValid\n");
		return IDVR_CIU_STRUCT_UNVALID;
	}
#if 1
	MPU_DBG("--SetNTP_Cfg--bNtpEnable=%d--szNtpHostAddress=%s-----\n",pcfg->bNtpEnable,pcfg->szNtpHostAddress);
	/*0-不启用, 1-启用 */
	nRet |= MPU_SYS_SetNtpEnable(pcfg->bNtpEnable);

	/*Ntp服务器 */
	nRet |= MPU_SYS_SetNtpServer(pcfg->szNtpHostAddress);

	//目前系统不支持配置
	/*Ntp服务器，端口 */
	//pcfg->wNtpHostPort;

	/*Ntp服务器同步间隔 */
	//MPU_NTP_SetAutoUpdateTime(pcfg->nAutoRegIntervals);
#endif
	return nRet;
}
/*设置设备配置信息time zone*/
static int SetDevTimeZone(ZW_SYSCOMMON_DATETIME_CFG * pcfg)
{
	int nSDKRet= IDVR_OK, iTimeLayout, nCh, nCount;
	DEVICEINFO di;
	//本结构体是否有效，=0表示无效
	if (0 == pcfg->bValid)
	{
		MPU_DBG("0 == pcfg->bValid\n");
		return IDVR_CIU_STRUCT_UNVALID;
	}
MPU_DBG("SetDevTimeZone: dwDateFormat=%d, dwTimeFormat=%d, dwTimeZone=%d, struNtp: valid=%d, enable=%d, ntpadd=%s\n", 
	pcfg->dwDateFormat, pcfg->dwTimeFormat, pcfg->dwTimeZone, pcfg->struNtp.bValid, pcfg->struNtp.bNtpEnable, pcfg->struNtp.szNtpHostAddress);
	nSDKRet = MPU_PUB_GetDeviceInfo(&di);
	MPU_CHK(0 == nSDKRet);
	
	//!\brief   日期格式
	di.m_nDateFormat = pcfg->dwDateFormat;

	//!\brief   时间格式（12小时制，24小时制）
	di.m_nTimeFormat = pcfg->dwTimeFormat;

	SetNTP_Cfg(&pcfg->struNtp);
	printf("\n\n*******MPU set data:pcfg->nTimeSyncInterval=%d\n\n",pcfg->nTimeSyncInterval);
	MPU_SYS_SetNtpInterval(pcfg->nTimeSyncInterval);//设置同步间隔
	
	
	//设置设备配置
	nSDKRet = MPU_PUB_SetDeviceInfo(&di);
	MPU_CHK(0 == nSDKRet);


	switch (di.m_nDateFormat)
	{
		case 0:
		case 1:
			iTimeLayout = (0 == di.m_nTimeFormat ? 0 : 1);
			break;

		case 2:
		case 3:
			iTimeLayout = (0 == di.m_nTimeFormat ? 2 : 3);
			break;
		case 4:
		case 5:
			iTimeLayout = (0 == di.m_nTimeFormat ? 4 : 5);
			break;

		default:
			iTimeLayout = 0;
			break;
	}

	// 设置编码通道的时间格式
	nCount = MPU_AVL_GetChNum();
	for (nCh = 0; nCh < nCount; ++nCh)
	{
		MPU_AVL_OSD_SetTimeFormat(nCh, iTimeLayout);
	}
//  MPU_DBG("iTimeLayout = %d\n", iTimeLayout);


	//通知GUI日期格式变更
	nSDKRet = CIU_NotifyRemoteControl(0, eRemoteSetTimeFormat, 0);
	MPU_CHK(0 == nSDKRet);
	MPU_DBG("--MPU_SYS_SetTimeZone---dwTimeZone=%d----\n",pcfg->dwTimeZone);
	if(pcfg->dwEnableTimeZoneV2)
	{
		nSDKRet = MPU_SYS_SetTimeZone(pcfg->dwTimeZoneV2);
	}
	else
	{
		nSDKRet = MPU_SYS_SetTimeZone(pcfg->dwTimeZone*60);
	}

	MPU_CHK(0 == nSDKRet);
#if 0	
	struct tm tTime;
	time_t lTime;
	tTime.tm_year = pcfg->struSysTime.nYear - 1900;
	tTime.tm_mon = pcfg->struSysTime.nMonth- 1;
	tTime.tm_mday = pcfg->struSysTime.nDay;
	tTime.tm_hour = pcfg->struSysTime.nHour;
	tTime.tm_min = pcfg->struSysTime.nMinute;
	tTime.tm_sec = pcfg->struSysTime.nSecond;
	lTime = mktime(&tTime);

	MPU_PUB_SetSystemTime(lTime);
	//MPU_DBG("TimeZone %d\n", pcfg->nTimeZone);
#endif
	MPU_DBG("return from SetDevTimeZone\n");
	return nSDKRet;
}

static int GetDevAlias(LPHY_DVR_ALIAS_CFG_1 pcfg)
{
	return MPU_PUB_GetAlias(SECTION_DEV_ALIAS, DEFNAME_DEV_ALIAS, pcfg);
}

//设置设备别名
static int SetDevAlias(LPHY_DVR_ALIAS_CFG_1 pcfg)
{
	int nRet = IDVR_OK, nSDKRet;

	char szSection[30];

	snprintf(szSection, sizeof(szSection), SECTION_DEV_ALIAS);

	///////////////////////////////////////////////////////////////////////////////
	//本结构体是否有效，=0表示无效
	if (0 == pcfg->bValid)
	{
		MPU_DBG("0 == pcfg->bValid\n");
		return IDVR_CIU_STRUCT_UNVALID;
	}

	if ((((pcfg->nWidth + 7) / 8) * pcfg->nHeight) > DVR_MAX_LATTICE_BUFSIZE)
	{
		MPU_ERR("%s: nWidth %d, nHeight %d, is overflow\n", szSection, pcfg->nWidth, pcfg->nHeight);
		return IDVR_CIU_PARAM_OVERFLOW;
	}

	if (0 == pcfg->strName[0])
	{
		MPU_ERR("%s: is NULL\n", szSection);
		return IDVR_CIU_PARAM_OVERFLOW;
	}

	MPU_DBG("%s: nWidth %d, nHeight %d\n", szSection, pcfg->nWidth, pcfg->nHeight);

	//保存点阵到位图
	nSDKRet = lattice_saveas_bmp24(pcfg->nWidth, pcfg->nHeight, pcfg->byLattice, IDVR_DEV_ALIAS, 0);
	if (0 == nSDKRet)
	{
		nSDKRet = MPU_PUB_SetAlias(szSection, pcfg);
		MPU_CHK(0 == nSDKRet);

		//通知GUI设备别名变更
		CIU_NotifyRemoteControl(0, eRemoteSetDevAlias, 0);
	}
	else
	{
		MPU_ERR("%s: change lattice to bmp fail, return %d\n", szSection, nSDKRet);
		nRet = IDVR_CIU_CALLSDK_FAIL;
	}

	return nRet;
}

static int GetDeviceCapacity(HY_DVR_DEVICE_CAP_FILE_NAME *pcfg)
{
	pcfg->nValid = 1;
	strncpy(pcfg->strDeviceCapFileName, CAPACITY_FILE_PATH, sizeof(pcfg->strDeviceCapFileName) - 1);//控制APP功能显示能力

	return 0;
}

static int GetDeviceCapacity_V2(HY_DVR_DEVICE_CAP_FILE_NAME_V2 *pcfg)
{
	pcfg->nValid = 1;
	strncpy(pcfg->strDeviceCapFileName, CUSTOM_DEVICE_CAP_FILE, sizeof(pcfg->strDeviceCapFileName) - 1);//控制网页功能显示的能力

	return 0;
}

static int GetAIDetectDeviceCapacity(HY_DVR_DEVICE_CAP_FILE_NAME_V2 *pcfg)
{
	pcfg->nValid = 1;
	strncpy(pcfg->strDeviceCapFileName, CUSTOM_DEVICE_AI_CAP_FILE, sizeof(pcfg->strDeviceCapFileName) - 1);//控制网页功能显示的能力

	return 0;
}

static int GetDeviceCapacity_V2_2(HY_DVR_DEVICE_CAP_FILE_NAME_V2 *pcfg)
{
	pcfg->nValid = 1;
	strncpy(pcfg->strDeviceCapFileName, PTZ_CAP_FILE_PATH, sizeof(pcfg->strDeviceCapFileName) - 1);//控制网页功能显示的能力

	return 0;
}

static int GetDeviceCapacity_V2_1(HY_DVR_DEVICE_CAP_FILE_NAME_V2 *pcfg)
{
	pcfg->nValid = 1;
	strncpy(pcfg->strDeviceCapFileName, CUSTOM_DEVICE_LIGHT_CAP_FILE, sizeof(pcfg->strDeviceCapFileName) - 1);//控制网页功能显示的能力(灯板)

	return 0;
}

//获取设备缓冲区信息，需外部将pcfg清零
static int GetIpcBufSzCfg(HY_DVR_BUFFER_SIZE *pcfg)
{
    int nRet = IDVR_OK;

    static int not_frist = 0;
    static int CommandBufferLen = 0;
	static int CommandSendBufferLen = 0;
    static int MediaBufferLen = 0;
    static int SubMediaBufferLen = 0;
    static int RealStreamBufferLen = 0;
    static int SubRealStreamBufferLen = 0;
	static int PictureBufferLen = 0;
	static int EventNoticeBufferLen = 0;
	static int CloudStorageSendMediaBufferLen = 0;
	static int CloudStorageAnnulusBufferLen = 0;
    static int CloudStorageSendMediaTimeout = 0;
    static int CloudStorageSendDataRetryCount = 0;
    static int CloudStorageSendIndexTimeout = 0;
    static int CloudStorageOtherMyServerTimeout = 0;
    static int CloudStorageSubSendMediaBufferLen = 0;
    static int CloudStorageSubAnnulusBufferLen = 0;

    //! 本结构体是否有效，=0表示无效
    pcfg->bVaild = 1;

    /* 如果设置了授权功能，则需要重新从SDK获取状态(状态不需要重置，授权之后，系统会重启)     */
    if (TRUE == g_bSetAuthorize)
    {
        not_frist = 0;
    }

    /* 针对设置授权成功，需要重新获取对应的版型 */
    if (1 == not_frist)
    {
        pcfg->nCommandBufferLen = CommandBufferLen;
		pcfg->nCommandSendBufferLen = CommandSendBufferLen;
        pcfg->nMediaBufferLen = MediaBufferLen;
        pcfg->nSubMediaBufferLen = SubMediaBufferLen;
        pcfg->nRealStreamBufferLen = RealStreamBufferLen;
        pcfg->nSubRealStreamBufferLen = SubRealStreamBufferLen;
        pcfg->nPictureBufferLen = PictureBufferLen;
        pcfg->nCloudStorageSubSendMediaBufferLen = CloudStorageSubSendMediaBufferLen;
        pcfg->nCloudStorageSubAnnulusBufferLen = CloudStorageSubAnnulusBufferLen;

        return nRet;
    }
    /* 三/四目设备适当增大尚云发送buffer最大值 */
    if (SYSTEM_SUPPORT_CHANNEL > 2)
    {
		/* 命令缓冲区大小 */
	    CommandBufferLen = 8*1024;
	    /* 命令发送缓冲区大小 */
		CommandSendBufferLen = 16*1024;
	    /* 媒体缓冲区大小(主码流) */
	    MediaBufferLen = 480*1024;
	    /* 媒体缓冲区大小(子码流) */
	    SubMediaBufferLen = 196*1024;
	    /* 实时流缓冲区大小(主码流) */
	    RealStreamBufferLen = 480*1024;
	    /* 实时流缓冲区大小(子码流) */
	    SubRealStreamBufferLen = 196*1024;
		/*缩略图缓冲区大小*/
		PictureBufferLen = MAX_PICTURE_BUF_SIZE;
		EventNoticeBufferLen = (128 + 8)*1024;


	    /*云存储发送缓存区*/
	    CloudStorageSendMediaBufferLen = 480*1024;
	    CloudStorageAnnulusBufferLen = 640*1024;

	    /*云存储媒体发送超时时间和重试次数*/
	    CloudStorageSendMediaTimeout = 8;
	    CloudStorageSendDataRetryCount = 3;
	    /*云存储索引发送超时时间*/
	    CloudStorageSendIndexTimeout = 8;
	    /*云存储服务与我们自己的服务器交互超时时间*/
	    CloudStorageOtherMyServerTimeout = 3;
	    /* 多通道时子码流云存储发送媒体缓冲区大小 */
	    CloudStorageSubSendMediaBufferLen = 196*1024;
	    /* 多通道时子码流云存储环形缓冲区大小 */
	    CloudStorageSubAnnulusBufferLen = 320*1024;

		pcfg->nP2PMaxSenderBufferLens = 1024*1024;
	    pcfg->nCommandBufferLen = CommandBufferLen;
		pcfg->nCommandSendBufferLen = CommandSendBufferLen;
	    pcfg->nMediaBufferLen = MediaBufferLen;
	    pcfg->nSubMediaBufferLen = SubMediaBufferLen;
	    pcfg->nRealStreamBufferLen = RealStreamBufferLen;
	    pcfg->nSubRealStreamBufferLen = SubRealStreamBufferLen;
		pcfg->nPictureBufferLen = PictureBufferLen;
		pcfg->nEventNoticeBufferLen = EventNoticeBufferLen;
    }
	else
	{
		/* 命令缓冲区大小 */
		CommandBufferLen = 8*1024;
		/* 命令发送缓冲区大小 */
		CommandSendBufferLen = 16*1024;
		/* 媒体缓冲区大小(主码流) */
		MediaBufferLen = 320*1024;
		/* 媒体缓冲区大小(子码流) */
		SubMediaBufferLen = 196*1024;
		/* 实时流缓冲区大小(主码流) */
		RealStreamBufferLen = 320*1024;
		/* 实时流缓冲区大小(子码流) */
		SubRealStreamBufferLen = 196*1024;
		/*缩略图缓冲区大小*/
		PictureBufferLen = MAX_PICTURE_BUF_SIZE;
		EventNoticeBufferLen = (128 + 8)*1024;
		
		
		/*云存储发送缓存区*/
		CloudStorageSendMediaBufferLen = 320*1024;
		CloudStorageAnnulusBufferLen = 512*1024;
		
		/*云存储媒体发送超时时间和重试次数*/
		CloudStorageSendMediaTimeout = 8;
		CloudStorageSendDataRetryCount = 3;
		/*云存储索引发送超时时间*/
		CloudStorageSendIndexTimeout = 8;
		/*云存储服务与我们自己的服务器交互超时时间*/
		CloudStorageOtherMyServerTimeout = 3;
		/* 多通道时子码流云存储发送媒体缓冲区大小 */
		CloudStorageSubSendMediaBufferLen = 196*1024;
		/* 多通道时子码流云存储环形缓冲区大小 */
		CloudStorageSubAnnulusBufferLen = 320*1024;
		
		pcfg->nP2PMaxSenderBufferLens = 256*1024;
		pcfg->nCommandBufferLen = CommandBufferLen;
		pcfg->nCommandSendBufferLen = CommandSendBufferLen;
		pcfg->nMediaBufferLen = MediaBufferLen;
		pcfg->nSubMediaBufferLen = SubMediaBufferLen;
		pcfg->nRealStreamBufferLen = RealStreamBufferLen;
		pcfg->nSubRealStreamBufferLen = SubRealStreamBufferLen;
		pcfg->nPictureBufferLen = PictureBufferLen;
		pcfg->nEventNoticeBufferLen = EventNoticeBufferLen;
	}

    pcfg->nCloudStorageSendMediaBufferLen = CloudStorageSendMediaBufferLen;
    pcfg->nCloudStorageAnnulusBufferLen = CloudStorageAnnulusBufferLen;
    pcfg->nCloudStorageSendMediaTimeout = CloudStorageSendMediaTimeout;
    pcfg->nCloudStorageSendDataRetryCount = CloudStorageSendDataRetryCount;
    pcfg->nCloudStorageSendIndexTimeout = CloudStorageSendIndexTimeout;
    pcfg->nCloudStorageOtherMyServerTimeout = CloudStorageOtherMyServerTimeout;
    pcfg->nCloudStorageSubSendMediaBufferLen = CloudStorageSubSendMediaBufferLen;
    pcfg->nCloudStorageSubAnnulusBufferLen = CloudStorageSubAnnulusBufferLen;


    //set flag
    not_frist = 0;

#if 0
    printf("szSection = %s\n",szSection);
    printf("pcfg->nCommandBufferLen = %#x\n",pcfg->nCommandBufferLen);
	printf("pcfg->nCommandSendBufferLen = %#x\n",pcfg->nCommandSendBufferLen);
    printf("pcfg->nMediaBufferLen = %#x\n",pcfg->nMediaBufferLen);
    printf("pcfg->nSubMediaBufferLen = %#x\n",pcfg->nSubMediaBufferLen);
    printf("pcfg->nRealStreamBufferLen = %#x\n",pcfg->nRealStreamBufferLen);
    printf("pcfg->nSubRealStreamBufferLen = %#x\n",pcfg->nSubRealStreamBufferLen);
	printf("pcfg->nPictureBufferLen = %#x\n",pcfg->nPictureBufferLen);
	printf("pcfg->nEventNoticeBufferLen = %#x\n",pcfg->nEventNoticeBufferLen);
#endif

    return nRet;
}

//获取设备缓冲区信息，需外部将pcfg清零
static int GetIpcConnectResource(HY_DVR_CONNECT_RESOURCE_INFO *pcfg)
{
    int nRet = IDVR_OK;

    //! 本结构体是否有效，=0表示无效
    pcfg->bValid = 1;
    pcfg->nMaxClientCount = 4;
    pcfg->nMaxRealStreamCount = 4;
    pcfg->nMaxPlaybackStreamCount = 2;

    return nRet;
}

//获取设备缓冲区信息，需外部将pcfg清零
static int GetIpcRealStreamConnectResource(HY_DVR_CONNECT_STATE *pcfg)
{
    int nRet = IDVR_OK;
    int nMaxSns = 0;

    nMaxSns = ZWSys_GetMaxSensorCnt();

    if (nMaxSns == 1)
    {
        pcfg->m_MaxRealStreamMain = MAX_MAIN_ACCESS_NUM;
        pcfg->m_MaxRealStreamSub = MAX_SECOND_ACCESS_NUM;
        pcfg->m_RealStreamMain = m_AccessMainStream;
        pcfg->m_RealStreamSub = m_AccessSecondStream;
    }
    else if (nMaxSns == 2)
    {
        pcfg->m_MaxRealStreamMain = MAX_MAIN_ACCESS_NUM + MAX_MAIN_ACCESS_NUM_S1;
        pcfg->m_MaxRealStreamSub = MAX_SECOND_ACCESS_NUM + MAX_SECOND_ACCESS_NUM_S1;
        pcfg->m_RealStreamMain = m_AccessMainStream + m_AccessMainStreamS1;
        pcfg->m_RealStreamSub = m_AccessSecondStream + m_AccessSecondStreamS1;
    }
    else if (nMaxSns == 3)
    {
        pcfg->m_MaxRealStreamMain = MAX_MAIN_ACCESS_NUM + MAX_MAIN_ACCESS_NUM_S1 + MAX_MAIN_ACCESS_NUM_S2;
        pcfg->m_MaxRealStreamSub = MAX_SECOND_ACCESS_NUM + MAX_SECOND_ACCESS_NUM_S1 + MAX_SECOND_ACCESS_NUM_S2;
        pcfg->m_RealStreamMain = m_AccessMainStream + m_AccessMainStreamS1 + m_AccessMainStreamS2;
        pcfg->m_RealStreamSub = m_AccessSecondStream + m_AccessSecondStreamS1 + m_AccessSecondStreamS2;
    }
    else if (nMaxSns == 4)
    {
        pcfg->m_MaxRealStreamMain = MAX_MAIN_ACCESS_NUM + MAX_MAIN_ACCESS_NUM_S1 + MAX_MAIN_ACCESS_NUM_S2 + MAX_MAIN_ACCESS_NUM_S3;
        pcfg->m_MaxRealStreamSub = MAX_SECOND_ACCESS_NUM + MAX_SECOND_ACCESS_NUM_S1 + MAX_SECOND_ACCESS_NUM_S2 + MAX_SECOND_ACCESS_NUM_S3;
        pcfg->m_RealStreamMain = m_AccessMainStream + m_AccessMainStreamS1 + m_AccessMainStreamS2 + m_AccessMainStreamS3;
        pcfg->m_RealStreamSub = m_AccessSecondStream + m_AccessSecondStreamS1 + m_AccessSecondStreamS2 + m_AccessSecondStreamS3;
    }
    else
    {
        //! 默认三目
        pcfg->m_MaxRealStreamMain = MAX_MAIN_ACCESS_NUM + MAX_MAIN_ACCESS_NUM_S1 + MAX_MAIN_ACCESS_NUM_S2;
        pcfg->m_MaxRealStreamSub = MAX_SECOND_ACCESS_NUM + MAX_SECOND_ACCESS_NUM_S1 + MAX_SECOND_ACCESS_NUM_S2;
        pcfg->m_RealStreamMain = m_AccessMainStream + m_AccessMainStreamS1 + m_AccessMainStreamS2;
        pcfg->m_RealStreamSub = m_AccessSecondStream + m_AccessSecondStreamS1 + m_AccessSecondStreamS2;
    }

    //! 本结构体是否有效，=0表示无效
    pcfg->nValid = 1;

    return nRet;
}

//获取设备配置信息，需外部将pcfg清零
static int GetDevConfig(LPHY_DVR_DEVICE_CFG pcfg)
{
	int nRet = IDVR_OK;

	//! 本结构体是否有效，=0表示无效
	pcfg->bValid = 1;


	/* DVR ID,用于遥控器 */
	GetDevDvrID(&pcfg->struNormal);

	//获取设备配置gui
	//GetDevGUI(pcfg);

	//!\brief   时区时差 及系统时间
	GetDevTimeZone(&pcfg->strDateTime);

	//夏时制信息
	GetDstCfg(&pcfg->struDST);

	//获取磁盘循环写入信息  ZW_RECORD_ADVANCED_CFG
	GetDiskGroup(&pcfg->struRecord);
	
	//设备别名
	//GetDevAlias(&pcfg->struDvrAlias);

	//获取设备输入输出制式
	//GetDevStandard(&pcfg->struNormal);

	return nRet;
}

/*设置设备配置信息*/
static int SetDevConfig(LPHY_DVR_DEVICE_CFG pcfg)
{
	int nRet, nSDKRet;

	//! 本结构体是否有效，=0表示无效
	if (0 == pcfg->bValid)
	{
		MPU_DBG("0 == pcfg->bValid\n");
		return IDVR_CIU_STRUCT_UNVALID;
	}

	//缺省返回设置失败
	nRet = IDVR_CIU_CALLSDK_FAIL;

	/* DVR ID,用于遥控器 */
	nSDKRet = SetDevDvrID(&pcfg->struNormal);
	if (IDVR_OK == nSDKRet)
	{
		nRet = IDVR_OK;
	}
	else
		MPU_DBG("my add: SetDevDvrID return %d \n",nSDKRet);
#if 0
	//获取设备配置gui
	nSDKRet = SetDevGUI(&pcfg->struGUI);
	if (IDVR_OK == nSDKRet)
	{
		nRet = IDVR_OK;
	}
#endif

	//!\brief   时区时差
	nSDKRet = SetDevTimeZone(&pcfg->strDateTime);
	if (IDVR_OK == nSDKRet)
	{
		nRet = IDVR_OK;
	}
	else
		MPU_DBG("my add: SetDevTimeZone return %d \n",nSDKRet);
	//夏时制信息
	nSDKRet = SetDstCfg(&pcfg->struDST);
	if (IDVR_OK == nSDKRet)
	{
		nRet = IDVR_OK;
	}
	else
		MPU_DBG("my add: SetDstCfg return %d \n",nSDKRet);
	#if 0
	//设备别名
	nSDKRet = SetDevAlias(&pcfg->struDvrAlias);
	if (IDVR_OK == nSDKRet)
	{
		nRet = IDVR_OK;
	}
	#endif

	//设置设备输入输出制式
	/*nSDKRet = SetDevStandard(&pcfg->struStandard);
	if (IDVR_OK == nSDKRet)
	{
		nRet = IDVR_OK;
	}*/

	//设置磁盘循环写入
	nSDKRet = SetDiskGroup(&pcfg->struRecord);
	if (IDVR_OK == nSDKRet)
	{
		nRet = IDVR_OK;
	}
	else
		MPU_DBG("my add: SetDiskGroup return %d \n",nSDKRet);
	//设备别名
	/*nSDKRet = SetDevAlias(&pcfg->struDvrAlias_1);
	if (IDVR_OK == nSDKRet)
	{
		nRet = IDVR_OK;
	}*/

	return nRet;
}

//获取设备显示信息，需外部将pcfg清零
static int GetVoConfig(LPHY_DVR_VO_CFG pcfg)
{
	int nRet = IDVR_OK, nSDKRet;

	//! 本结构体是否有效，=0表示无效
	pcfg->bValid = 1;


	/* 屏幕保护时间 */
	pcfg->nScreenSaveTime = MPU_PRW_GetScreenSaverTime();

	/* 菜单与背景图象对比度 */
	nSDKRet = MPU_PRW_GetEguiMixAlpha(&pcfg->nMenuAlphaValue);
	MPU_CHK(0 == nSDKRet);

	return nRet;
}

/*设置设备显示信息*/
static int SetVoConfig(LPHY_DVR_VO_CFG pcfg)
{
	int nRet = IDVR_OK, nSDKRet;

	//! 本结构体是否有效，=0表示无效
	if (0 == pcfg->bValid)
	{
		MPU_DBG("0 == pcfg->bValid\n");
		return IDVR_CIU_STRUCT_UNVALID;
	}

	/* 屏幕保护时间 */
	nSDKRet = MPU_PRW_SetScreenSaverTime(pcfg->nScreenSaveTime);
	MPU_CHK(0 == nSDKRet);

	/* 菜单与背景图象对比度 */
	nSDKRet = MPU_PRW_SetEguiMixAlpha(pcfg->nMenuAlphaValue);
	MPU_CHK(0 == nSDKRet);

	return nRet;
}

//获取设备各通道状态信息，需外部将pcfg清零
static int GetChannel_State(int nCh, LPHY_DVR_CHANNEL_STATE pcfg)
{
	int nRet = IDVR_OK;

	//! 本结构体是否有效，=0表示无效
	pcfg->bValid = 1;

	//主码流码率
	pcfg->nVideoBitrate = MPU_AVL_Coder_GetBitInfo(nCh);

	//子码流码率
	pcfg->nChildVideoBitrate = MPU_AVL_AuxCoder_GetBitInfo(nCh);

#ifdef USE_HDAUX
	pcfg->nVGAVideoBitrate = MPU_AVL_HDAuxCoder_GetBitInfo(nCh);
	pcfg->bUseVGAVideoStream = MPU_AVL_HDAuxCoder_IsEnable(nCh);
#endif

	//是否启动子码流
	pcfg->bUseAuxVideoStream = MPU_AVL_AuxCoder_IsEnable(nCh);

	//录像状态(分类型)
	pcfg->nRecordType = MPU_AVL_Record_GetState(nCh);

	//是否有移动报警
	//int bMotion;              
	//MPU_INFO("--GetDevState--nVideoBitrate=%d----------\n",pcfg->nVideoBitrate);
//	MPU_DBG("bitrate:%d-%d-%d,enable:%d-%d\n",pcfg->nVideoBitrate,pcfg->nChildVideoBitrate,pcfg->nVGAVideoBitrate,
//		pcfg->bUseAuxVideoStream,pcfg->bUseVGAVideoStream);

	return nRet;
}

//获取设备状态信息，需外部将pcfg清零
static int GetDevState(LPHY_DVR_DEVICE_STATE pcfg)
{
	int nRet = IDVR_OK, nCh, nCount;

	//! 本结构体是否有效，=0表示无效
	pcfg->bValid = 1;

	///////////////////////////////////////////////////////////////////////////////
	//获取录像通道总数
	nCount = MAX_ENC_CHANNEL_NUM;
	if (nCount > DVR_MAX_VIDEOIN_NUM)
	{
		MPU_ERR("remote config picture %d, real %d\n", DVR_MAX_VIDEOIN_NUM, nCount);
		nCount = DVR_MAX_VIDEOIN_NUM;
	}
	///////////////////////////////////////////////////////////////////////////////
	//获取各通道参数配置
	for (nCh = 0; nCh < nCount; ++nCh)
	{
		//显示OSD
		GetChannel_State(nCh, &pcfg->struChannelState[nCh]);
#if 0
		pcfg->struVGABitrate.bValid = 1;
#ifdef USE_HDAUX
		pcfg->struVGABitrate.struVGABitrate[nCh].nVGAVideoBitrate = MPU_AVL_HDAuxCoder_GetBitInfo(nCh);
#endif
		pcfg->struVGABitrate.struVGABitrate[nCh].bValid = 1;
#endif
	}
	
	return nRet;
}

//获取设备系统时间，需外部将pcfg清零
static int GetSystemTime(LPHY_DVR_TIME pcfg)
{
	int nRet = IDVR_OK;

	time_t lTimeNow = time(NULL);	//当前时间

	struct tm tTime;

	localtime_r(&lTimeNow, &tTime);

	pcfg->nYear = tTime.tm_year + 1900;
	pcfg->nMonth = tTime.tm_mon + 1;
	pcfg->nDay = tTime.tm_mday;
	pcfg->nHour = tTime.tm_hour;
	pcfg->nMinute = tTime.tm_min;
	pcfg->nSecond = tTime.tm_sec;

	return nRet;
}

/*设置设备系统时间*/
static int SetSystemTime(LPHY_DVR_TIME pcfg)
{
	int nRet = IDVR_OK, nSDKRet;

	struct tm tTime;

	tTime.tm_year = pcfg->nYear - 1900;
	tTime.tm_mon = pcfg->nMonth - 1;
	tTime.tm_mday = pcfg->nDay;
	tTime.tm_hour = pcfg->nHour;
	tTime.tm_min = pcfg->nMinute;
	tTime.tm_sec = pcfg->nSecond;

	time_t tNewTime = mktime(&tTime);

	nSDKRet = MPU_PUB_SetSystemTime(tNewTime);
	MPU_CHK(0 == nSDKRet);

	if (0 != nSDKRet)
	{
		MPU_ERR("MPU_PUB_SetSystemTime() fail, return %d\n", nSDKRet);
		nRet = IDVR_CIU_CALLSDK_FAIL;
	}

	return nRet;
}

/*获取设备自动维护配置信息(基本配置)，需外部将pcfg清零*/
static int GetDevMaintenance_Common(LPHY_DVR_MAINTENANCE_COMMON pcfg)
{
	int nRet = IDVR_OK, nSDKRet, i;

	tagTime tTime;

	//! 本结构体是否有效，=0表示无效
	pcfg->bValid = 1;

	// 1为启用，0为禁用
	pcfg->bEnable = MPU_SYS_Maintenance_IsEnable();
    MPU_DBG("===pcfg->bEnable = %d===\n", pcfg->bEnable);	

	//维护模式 0为每天、1为每周、2为单次
	pcfg->nMode = MPU_SYS_Maintenance_GetMode();
    MPU_DBG("===pcfg->nMode = %d===\n", pcfg->nMode);	

	//星期模式时，第0位表示星期天,第1位表示星期1,对应位为1表示当天有效
	for (i = 0; i < 7; ++i)
	{
		if (MPU_SYS_Maintenance_IsEnableWeekDay(i))
		{
			SET_BIT(pcfg->nWeekDayMask, i);
		}
	}
	
	//维护时间
	nSDKRet = MPU_SYS_Maintenance_GetMaintenanceTime(&tTime);
	MPU_CHK(0 == nSDKRet);

	//如为单次模式，则年月日时分有效
	//其它只有时分有效
	pcfg->struTime.nYear = tTime.year;
	pcfg->struTime.nMonth = tTime.mon;
	pcfg->struTime.nDay = tTime.day;
	pcfg->struTime.nHour = tTime.hour;
	pcfg->struTime.nMinute = tTime.min;
	pcfg->struTime.nSecond = tTime.sec;

	return nRet;
}

/*设置设备自动维护配置信息(基本配置)*/
static int SetDevMaintenance_Common(LPHY_DVR_MAINTENANCE_COMMON pcfg)
{
	int nRet = IDVR_OK, nSDKRet, i;

	tagTime tTime;

	//! 本结构体是否有效，=0表示无效
	if (0 == pcfg->bValid)
	{
		MPU_DBG("0 == pcfg->bValid\n");
		return IDVR_CIU_STRUCT_UNVALID;
	}

	// 1为启用，0为禁用
	MPU_DBG("===pcfg->bEnable = %d===\n", pcfg->bEnable);	
	nSDKRet = MPU_SYS_Maintenance_Enable(pcfg->bEnable);
	MPU_CHK(0 == nSDKRet);

	//维护模式 0为每天、1为每周、2为单次
	MPU_DBG("===pcfg->nMode   = %d===\n", pcfg->nMode);	
	nSDKRet = MPU_SYS_Maintenance_SetMode(pcfg->nMode);
	MPU_CHK(0 == nSDKRet);

	//维护时间
	if (pcfg->nMode == 0)
	{
		//每天只有时分有效，如每天凌晨00点00分维护
		tTime.year = 0;
		tTime.mon = 0;
		tTime.day = 0;
		tTime.hour = pcfg->struTime.nHour;
		tTime.min = pcfg->struTime.nMinute;
		tTime.sec = 0;
		MPU_DBG("hour = %d, min = %d\n", pcfg->struTime.nHour, pcfg->struTime.nMinute);
	}
	else if (pcfg->nMode == 1)
	{
		//星期模式时，第0位表示星期天,第1位表示星期1,对应位为1表示当天有效
		for (i = 0; i < 7; ++i)
		{
			nSDKRet = MPU_SYS_Maintenance_EnableWeekDay(i, IS_SET_BIT(pcfg->nWeekDayMask, i));
			MPU_CHK(0 == nSDKRet);
		}

		//每周只有时分有效(前面已经配置了星期几)
		tTime.year = 0;
		tTime.mon = 0;
		tTime.day = 0;
		tTime.hour = pcfg->struTime.nHour;
		tTime.min = pcfg->struTime.nMinute;
		tTime.sec = 0;
		MPU_DBG("hour = %d, min = %d\n", pcfg->struTime.nHour, pcfg->struTime.nMinute);
	}
	else if (pcfg->nMode == 2)
	{
		//如为单次模式，则年月日时分有效
		tTime.year = pcfg->struTime.nYear;
		tTime.mon = pcfg->struTime.nMonth;
		tTime.day = pcfg->struTime.nDay;
		tTime.hour = pcfg->struTime.nHour;
		tTime.min = pcfg->struTime.nMinute;
		tTime.sec = 0;
		MPU_DBG("year=%d, mon=%d, day=%d, hour=%d, min=%d\n", pcfg->struTime.nYear, pcfg->struTime.nMonth, 
		        pcfg->struTime.nDay, pcfg->struTime.nHour, pcfg->struTime.nMinute);
	}
	else
	{
		MPU_ERR("unsupported mode = %d\n", pcfg->nMode == 2);
	}

	nSDKRet = MPU_SYS_Maintenance_SetMaintenanceTime(tTime);
	MPU_CHK(0 == nSDKRet);

	return nRet;
}

/*获取设备自动维护配置信息，需外部将pcfg清零*/
static int GetDevMaintenance(LPHY_DVR_DEVICE_MAINTENANCE pcfg)
{
	//! 本结构体是否有效，=0表示无效
	pcfg->bValid = 1;

	GetDevMaintenance_Common(&pcfg->struCommon);

	return IDVR_OK;
}

/*设置设备自动维护配置信息*/
static int SetDevMaintenance(LPHY_DVR_DEVICE_MAINTENANCE pcfg)
{
	int nRet;

	//! 本结构体是否有效，=0表示无效
	if (0 == pcfg->bValid)
	{
		MPU_DBG("0 == pcfg->bValid\n");
		return IDVR_CIU_STRUCT_UNVALID;
	}

	nRet = SetDevMaintenance_Common(&pcfg->struCommon);

	return nRet;
}

int GetDevCustom_User(LPHY_DVR_WORKMODE pcfg)
{
	//! 本结构体是否有效，=0表示无效
	pcfg->bValid = 1;

	pcfg->nWorkMode = MPU_USR_GetWorkMode();

	return IDVR_OK;
}

int GetDevCustom_Alarm(LPHY_DVR_WORKMODE pcfg)
{
    return IDVR_OK;
}

int GetDevCustom_Pic(LPHY_DVR_WORKMODE pcfg)
{
	//! 本结构体是否有效，=0表示无效
	pcfg->bValid = 1;

	pcfg->nWorkMode = MPU_AVL_Pic_GetWorkMode();

	return IDVR_OK;
}

int GetDevCustom_Record(LPHY_DVR_WORKMODE pcfg)
{
	int nSDKRet;

	//! 本结构体是否有效，=0表示无效
	pcfg->bValid = 1;

	nSDKRet = MPU_AVL_Record_GetWorkMode(&pcfg->nWorkMode);
	MPU_CHK(0 == nSDKRet);

	return 0 == nSDKRet ? IDVR_OK : -1;
}

int GetDevCustom_Storage(LPHY_DVR_WORKMODE pcfg)
{
	//! 本结构体是否有效，=0表示无效
	pcfg->bValid = 1;

	pcfg->nWorkMode = MPU_STR_DiskGroup_GetWorkMode();

	return IDVR_OK;
}

int SetDevCustom_User(LPHY_DVR_WORKMODE pcfg)
{
	int nSDKRet;

	//! 本结构体是否有效，=0表示无效
	if (0 == pcfg->bValid)
	{
		MPU_DBG("0 == pcfg->bValid\n");
		return IDVR_CIU_STRUCT_UNVALID;
	}

	nSDKRet = MPU_USR_SetWorkMode(pcfg->nWorkMode);
	MPU_CHK(0 == nSDKRet);

	//通知GUI用户权限变更
	CIU_NotifyRemoteControl(0, eRemoteSetUserRight, 0);

	return (0 == nSDKRet) ? IDVR_OK : IDVR_CIU_CALLSDK_FAIL;
}

int SetDevCustom_Alarm(LPHY_DVR_WORKMODE pcfg)
{
    int nSDKRet;
    return (0 == nSDKRet) ? IDVR_OK : IDVR_CIU_CALLSDK_FAIL;
}

int SetDevCustom_Pic(LPHY_DVR_WORKMODE pcfg)
{
	int nSDKRet;

	//! 本结构体是否有效，=0表示无效
	if (0 == pcfg->bValid)
	{
		MPU_DBG("0 == pcfg->bValid\n");
		return IDVR_CIU_STRUCT_UNVALID;
	}

	nSDKRet = MPU_AVL_Pic_SetWorkMode(pcfg->nWorkMode);
	MPU_CHK(0 == nSDKRet);

	return (0 == nSDKRet) ? IDVR_OK : IDVR_CIU_CALLSDK_FAIL;
}

int SetDevCustom_Record(LPHY_DVR_WORKMODE pcfg)
{
	int nSDKRet;

	//! 本结构体是否有效，=0表示无效
	if (0 == pcfg->bValid)
	{
		MPU_DBG("0 == pcfg->bValid\n");
		return IDVR_CIU_STRUCT_UNVALID;
	}

	nSDKRet = MPU_AVL_Record_SetWorkMode(pcfg->nWorkMode);
	MPU_CHK(0 == nSDKRet);

	return (0 == nSDKRet) ? IDVR_OK : IDVR_CIU_CALLSDK_FAIL;
}

int SetDevCustom_Storage(DWORD dwUserID, LPHY_DVR_WORKMODE pcfg)
{
	int nSDKRet;

	//! 本结构体是否有效，=0表示无效
	if (0 == pcfg->bValid)
	{
		MPU_DBG("0 == pcfg->bValid\n");
		return IDVR_CIU_STRUCT_UNVALID;
	}

	MPU_DBG("storage work mode is %d\n", pcfg->nWorkMode);

	//模式改变则设置
	if (MPU_STR_DiskGroup_GetWorkMode() != pcfg->nWorkMode)
	{
		nSDKRet = MPU_STR_DiskGroup_SetWorkMode(pcfg->nWorkMode);
		MPU_CHK(IDVR_OK == nSDKRet);

		if (IDVR_OK != nSDKRet)
		{
			MPU_ERR("set storage work mode to %d fail, return %d\n", pcfg->nWorkMode, nSDKRet);
			return IDVR_CIU_CALLSDK_FAIL;
		}

		//通知GUI存储管理模式变更
		//高级模式切换到普通模式，需要重启
		CIU_NotifyRemoteControl(dwUserID, eRemoteSetStorageMode, pcfg->nWorkMode);
	}

	return IDVR_OK;
}

static int GetDeviceFunction(char *pBuffer)
{
    if (NULL == pBuffer)
    {
        MPU_ERR("NULL POINTER EXCEPTION!\n");
        return -1;
    }
    
	int nRet = IDVR_OK;
	
    nRet = MPU_PUB_GetDeviceFunction(pBuffer);

	return nRet;
}

static int SetDeviceFunction(const char *pBuffer)
{
    if (NULL == pBuffer)
    {
        MPU_ERR("NULL POINTER EXCEPTION!\n");
        return -1;
    }
    
	int nRet = IDVR_OK;
	
    nRet = MPU_PUB_SetDeviceFunction(pBuffer);

	return nRet;
}

int SetAuthInfoCfg(HY_DVR_AUTH_INFO *pcfg)
{	
	int ret =0;
	char command[128] = {0};

	//本结构体是否有效，=0表示无效
	if (0 == pcfg->bValid)
	{
		MPU_DBG("0 == pcfg->bValid\n");
		return IDVR_CIU_STRUCT_UNVALID;
	}

    /* 传入的授权码为空表示取消授权 */
    if (0 == strlen(pcfg->strAuthInfo))
    {
        ret = ZWSys_RemoveAuthLicense();

	#if defined (__IS_SUPPORT_APP_SHARE__)
		if (REMOVE_AUTH_SUCCESS == ret)
		{
			//取消授权，清除S码
			MPU_SYS_DeviceBind_ClearSCode();
            MPU_Net_ClearAuthDomainSuffix();
		}
	#endif

	#ifdef __MODULE_4G_SUPPORT__
        int bSupport4G = 0;
        ZWSys_Get4GSupportV2(&bSupport4G);
        if (bSupport4G)
        {
    		if (REMOVE_AUTH_SUCCESS == ret)
            {
                char cmd_string[128];

    			//删除解绑标识
    			memset(cmd_string,0,sizeof(cmd_string));
    			snprintf(cmd_string,sizeof(cmd_string)-1,"rm %s -rf",MODULE_4G_BIND_FLAG);
    			mysystem(cmd_string);
    		}
        }
    #endif
    }
    else
    {
        unsigned int nAuthState = 0;
        unsigned char szAuthCode[256] = {0};
        ZWSys_GetAuthInfo(szAuthCode, &nAuthState);

        /* 针对需要授权的设备，需要先取消授权，然后才能再次授权 */
        if (nAuthState)
        {
            printf("IPC has been authorized, please cancel it and retry!\n");
            return AUTH_RE_OPERATE_ERR;
        }
        else
        {
            ret = ZWSys_SetAuthInfo((const unsigned char *)pcfg->strAuthInfo);

        #if defined (__IS_SUPPORT_APP_SHARE__)
            if (AUTH_SUCCESS == ret)
            {
				TutkCfg stuTutkCfg = {0};
				unsigned long tutkStatus = 0;
				MPU_NET_GetTutkUID(&stuTutkCfg, &tutkStatus);

				//从临时文件获取的did，设置下去
				MPU_NET_SetTutkUID(&stuTutkCfg, 0);

				char did[128] = {0};
				if (0 == strlen(stuTutkCfg.szPushID)) 
				{
					snprintf(did, sizeof(did), "%s", stuTutkCfg.szTutkUID);
				}
				else
				{
					snprintf(did, sizeof(did), "%s#%s", stuTutkCfg.szTutkUID, stuTutkCfg.szPushID);
				}
				//SetValueToBootEnv("did_code", did, strlen(did));
                snprintf(command, sizeof(command), "touch %s", SUBKEY_NEED_GENERAL_FLAG);
                mysystem(command);

                MPU_SYS_DeviceBind_CreateSCode(did);
            }
			else
			{
				printf("Auth failed, error code:[%d] !\n",ret);
				return ret;
			}
		#endif
        }
    }

    /* 已经设置了授权功能 */
    g_bSetAuthorize = TRUE;

    printf("Set AuthCode errno: %d [%s]\n", ret, pcfg->strAuthInfo);
	return ret;
}	

#if defined (__AUTH_V2__)
int SetAuthInfoCfgV2(HY_DVR_AUTH_INFO_V2 *pcfg)
{
    int ret =0;
    char command[128] = {0};

    //本结构体是否有效，=0表示无效
    if (0 == pcfg->nValid)
    {
        MPU_DBG("0 == pcfg->nValid\n");
        return IDVR_CIU_STRUCT_UNVALID;
    }

    /* 传入的授权码为空表示取消授权 */
    if (0 == strlen(pcfg->szAuthInfo))
    {
        ret = ZWSys_RemoveAuthLicense();
    #if defined (__IS_SUPPORT_APP_SHARE__)
        if (REMOVE_AUTH_SUCCESS == ret)
        {
            //取消授权，清除S码
            MPU_SYS_DeviceBind_ClearSCode();
            MPU_Net_ClearAuthDomainSuffix();
        }
    #endif

    #ifdef __MODULE_4G_SUPPORT__
        int bSupport4G = 0;
        ZWSys_Get4GSupportV2(&bSupport4G);
        if (bSupport4G)
        {
            if (REMOVE_AUTH_SUCCESS == ret)
            {
                char cmd_string[128];

                //删除解绑标识
                memset(cmd_string,0,sizeof(cmd_string));
                snprintf(cmd_string,sizeof(cmd_string)-1,"rm %s -rf",MODULE_4G_BIND_FLAG);
                mysystem(cmd_string);
            }
        }
    #endif	
    }
    else
    {
        unsigned int nAuthState = 0;
        unsigned char szAuthCode[256] = {0};
        ZWSys_GetAuthInfo(szAuthCode, &nAuthState);

        /* 针对需要授权的设备，需要先取消授权，然后才能再次授权 */
        if (nAuthState)
        {
            printf("IPC has been authorized, please cancel it and retry!\n");
            return AUTH_RE_OPERATE_ERR;
        }
        else
        {
            ret = ZWSys_SetAuthInfo((const unsigned char *)pcfg->szAuthInfo);
            printf("Set AuthCode errno: %d [%s]\n", ret, pcfg->szAuthInfo);
            if (ret != AUTH_SUCCESS)
            {
                return ret;
            }
            else
            {
            #if defined (__IS_SUPPORT_APP_SHARE__)
                TutkCfg stuTutkCfg = {0};
                unsigned long tutkStatus = 0;
                MPU_NET_GetTutkUID(&stuTutkCfg, &tutkStatus);

                //从临时文件获取的did，设置下去
                MPU_NET_SetTutkUID(&stuTutkCfg, 0);

                char did[128] = {0};
                if (0 == strlen(stuTutkCfg.szPushID)) 
                {
                    snprintf(did, sizeof(did), "%s", stuTutkCfg.szTutkUID);
                }
                else
                {
                    snprintf(did, sizeof(did), "%s#%s", stuTutkCfg.szTutkUID, stuTutkCfg.szPushID);
                }
                //SetValueToBootEnv("did_code", did, strlen(did));
                snprintf(command, sizeof(command), "touch %s", SUBKEY_NEED_GENERAL_FLAG);
                mysystem(command);

                MPU_SYS_DeviceBind_CreateSCode(did);
            #endif
            }

        }
    }
    
    /* 已经设置了授权功能 */
    g_bSetAuthorize = TRUE;

    //设置连接串信息
    P2P_CONNECT_INFO nP2pConnectInfo = {0};
    int i = 0;
    memcpy(nP2pConnectInfo.szPlatformName, pcfg->struP2PConnectInfo.szPlatformName, sizeof(nP2pConnectInfo.szPlatformName));
    memcpy(nP2pConnectInfo.szConnectString, pcfg->struP2PConnectInfo.szConnectString, sizeof(nP2pConnectInfo.szConnectString));
    memcpy(nP2pConnectInfo.szAuthWord, pcfg->struP2PConnectInfo.szAuthWord, sizeof(nP2pConnectInfo.szAuthWord));

    for(i = 0 ; i < 8; i++)
    {
        memcpy(nP2pConnectInfo.struDomain[i].szDomain, pcfg->struP2PConnectInfo.struDomain[i].szDomain, sizeof(nP2pConnectInfo.struDomain[i].szDomain));
        memcpy(nP2pConnectInfo.struDomain[i].szIP, pcfg->struP2PConnectInfo.struDomain[i].szIP, sizeof(nP2pConnectInfo.struDomain[i].szIP));
    }
    //设置连接串信息
    ret = MPU_NET_SetConnectString(&nP2pConnectInfo, (const unsigned char *)pcfg->szAuthInfo);
    if (ret != 0)
    {
        printf("Set ConnectString errno: %d [%s]\n", ret, pcfg->struP2PConnectInfo.szConnectString);
        return ret;
    }

    return ret;
}
#endif

int GetAuthInfoCfg(HY_DVR_AUTH_INFO *pcfg)
{
	int nRet = 0;
    unsigned int nAuthState = 0;

    pcfg->bValid = 1;

    nRet = ZWSys_GetAuthInfo((unsigned char *)pcfg->strAuthInfo, &nAuthState);

	return nRet;
}	

#if defined (__AUTH_V2__)
int GetAuthInfoCfgV2(HY_DVR_AUTH_INFO_V2 *pcfg)
{
    int nRet = 0;
    P2P_CONNECT_INFO nP2pConnectInfo = {0};
    unsigned int nAuthState = 0;

    pcfg->nValid = 1;
    
    nRet = ZWSys_GetAuthInfo((unsigned char *)pcfg->szAuthInfo, &nAuthState);
    nRet = MPU_NET_GetConnectString(&nP2pConnectInfo);
    memcpy(pcfg->struP2PConnectInfo.szPlatformName, nP2pConnectInfo.szPlatformName, sizeof(nP2pConnectInfo.szPlatformName));
    memcpy(pcfg->struP2PConnectInfo.szConnectString, nP2pConnectInfo.szConnectString, sizeof(nP2pConnectInfo.szConnectString));
    memcpy(pcfg->struP2PConnectInfo.szAuthWord, nP2pConnectInfo.szAuthWord, sizeof(nP2pConnectInfo.szAuthWord));
    return nRet;
}
#endif

int SetAuthInfoTransparentInfo(HY_DVR_AUTH_INFO_TRANSPARENT_INFO *pcfg)
{
    int ret = 0;

    //本结构体是否有效，=0表示无效
    if (0 == pcfg->nValid || 0 == pcfg->stPushDomainParam.nValid)
    {
        MPU_ERR("0 == pcfg->nValid\n");
        return IDVR_CIU_STRUCT_UNVALID;
    }

    //设置连接串信息
    P2P_TRANSPARENT_INFO_PUSHDOMAIN nP2pPushDomainParam = {0};
    memcpy(nP2pPushDomainParam.acDomainSuffix, pcfg->stPushDomainParam.acDomainSuffix, sizeof(nP2pPushDomainParam.acDomainSuffix));

    ret = MPU_NET_SetAuthInfoPushDomainSuffix(&nP2pPushDomainParam);
    if (ret != 0)
    {
        MPU_ERR("Set SetAuthInfoPushDomainSuffix errno: %d [%s]\n", ret, pcfg->stPushDomainParam.acDomainSuffix);
        return ret;
    }
    return ret;
}

int GetAuthInfoTransparentInfo(HY_DVR_AUTH_INFO_TRANSPARENT_INFO *pcfg)
{
    int nRet = 0;

    P2P_TRANSPARENT_INFO_PUSHDOMAIN nP2pPushDomainParam = {0};
    pcfg->nValid = 1;
    pcfg->stPushDomainParam.nValid = 1;

    nRet = MPU_NET_GetAuthInfoPushDomainSuffix(&nP2pPushDomainParam);

    memcpy(pcfg->stPushDomainParam.acDomainSuffix, nP2pPushDomainParam.acDomainSuffix, sizeof(nP2pPushDomainParam.acDomainSuffix));
    memset(&(pcfg->stManagerServerParam), 0, sizeof(pcfg->stManagerServerParam));
    return nRet;
}

#if defined (__IS_SUPPORT_4G_CLOUD_DOMAIN__)
int SetAuthInfoTransparentV2(HY_DVR_AUTH_INFO_TRANSPARENT_INFO *pcfg)
{
    if (NULL == pcfg)
    {
        MPU_ERR("NULL pointer \n");
        return -1;
    }

    printf("SetAuthInfoTransparentV2 start !\n");

    int nRet = 0;
    TransParent_INFO nTransParentInfo;
    memset(&nTransParentInfo, 0, sizeof(nTransParentInfo));

    printf("valid: %d  stPushDomainParam: %s  stManagerServerParam: %s port: %d \n", pcfg->nValid, pcfg->stPushDomainParam.acDomainSuffix, pcfg->stManagerServerParam.acDomainListManagerServerAdd, pcfg->stManagerServerParam.iDomainListManagerServerPort);
    memcpy(nTransParentInfo.acDomainSuffix, pcfg->stPushDomainParam.acDomainSuffix, sizeof(pcfg->stPushDomainParam.acDomainSuffix));
    memcpy(nTransParentInfo.cDomainServer, pcfg->stManagerServerParam.acDomainListManagerServerAdd, sizeof(nTransParentInfo.cDomainServer));
    nTransParentInfo.port = pcfg->stManagerServerParam.iDomainListManagerServerPort;

    nRet = MPU_NET_AuthInfoTransparentV2(&nTransParentInfo);

    return nRet;
}
#endif

static int GetSchedule_Sleep(LPHY_DVR_SCHEDTIME pcfg)
{
    int nRet = IDVR_OK, nDay, i, num;
    MotionSchedule sched;

    //! 本结构体是否有效，=0表示无效
    pcfg->bValid = 1;

    // 睡眠模式布防计划时间表(0表示不布防，1表示布防) 
    MPU_SYS_AwakeSchedule_GetAwakeScheduleTime(&sched);

    num = DVR_MAX_TIMESEGMENT / HY_DVR_MOTIONSEG_DAY;

    for (nDay = 0; nDay < DVR_MAX_DAYS; ++nDay)
    {
        for (i = 0; i < DVR_MAX_TIMESEGMENT; ++i)
        {
            pcfg->bySetType[nDay][i] = sched.dwTimeSeg[nDay][i / num];
        }
    }

    //! 状态,0为禁用(全天布防),1为启用,2为不支持
    pcfg->byState = sched.bytState;

    pcfg->byReserve[0] = sched.bytReserve[0];
    pcfg->byReserve[1] = sched.bytReserve[1];
    pcfg->byReserve[2] = sched.bytReserve[2];

    return nRet;
}

static int GetDevSleepInfo(LPHY_DVR_DEVICE_SLEEP_INFO_CFG pcfg)
{
    if (NULL == pcfg)
    {
        MPU_ERR("NULL POINTER EXCEPTION!\n");
        return -1;
    }
    
	int nRet = IDVR_OK;

	//本结构体是否有效，=0表示无效
	pcfg->iValid = 1;

    /* 设备当前睡眠状态 */
    pcfg->nSleepStatus = MPU_SYS_AwakeSchedule_GetDevSleepStatus();

    /* 定时睡眠开关 */
    MPU_SYS_AwakeSchedule_GetScheduleEnable(&(pcfg->bEnable));

    /* 睡眠结束时间 */
    MPU_SYS_AwakeSchedule_GetAwakeScheduleEndTime(&(pcfg->stSleepEndTime.iValid), &(pcfg->stSleepEndTime.iLweekday), &(pcfg->stSleepEndTime.iLhour));

    /* 睡眠布防时间 */
    GetSchedule_Sleep(&pcfg->struSchedTime);

	return nRet;
}

static int SetSchedule_Sleep(LPHY_DVR_SCHEDTIME pcfg)
{
    int nRet = IDVR_OK, nDay, i, num;

    MotionSchedule sched;

	//本结构体是否有效，=0表示无效
	if (0 == pcfg->bValid)
	{
		MPU_DBG("0 == pcfg->bValid\n");
		return IDVR_CIU_STRUCT_UNVALID;
	}

	num = DVR_MAX_TIMESEGMENT / HY_DVR_MOTIONSEG_DAY;

	for (nDay = 0; nDay < DVR_MAX_DAYS; ++nDay)
	{
		for (i = 0; i < HY_DVR_ALARMOUTSEG_DAY; ++i)
		{
			sched.dwTimeSeg[nDay][i] = pcfg->bySetType[nDay][i * num];
		}
	}

	sched.bytState = pcfg->byState;

	sched.bytReserve[0] = pcfg->byReserve[0];
	sched.bytReserve[1] = pcfg->byReserve[1];
	sched.bytReserve[2] = pcfg->byReserve[2];

	//设置睡眠布防时间计划
	MPU_SYS_AwakeSchedule_SetAwakeScheduleTime(&sched);

	return nRet;
}

static int SetDevSleepInfo(const LPHY_DVR_DEVICE_SLEEP_INFO_CFG pcfg)
{
    if (NULL == pcfg)
    {
        MPU_ERR("NULL POINTER EXCEPTION!\n");
        return -1;
    }
    
	int nRet = IDVR_OK;
    HY_DVR_SCHEDTIME       struSchedTime;

    //本结构体是否有效，=0表示无效
    if (0 == pcfg->iValid)
	{
		MPU_ERR("0 == pcfg->bValid \n");
		return IDVR_CIU_STRUCT_UNVALID;
	}

    MPU_DBG("--SetDevSleepInfo--nSleepStatus=%d--bEnable=%d--\n",pcfg->nSleepStatus,pcfg->bEnable);

    /* 定时睡眠开关 */
    MPU_SYS_AwakeSchedule_SetScheduleEnable(pcfg->bEnable);

    /* 更新睡眠布防时间 */
    memset(&struSchedTime, 0, sizeof(HY_DVR_SCHEDTIME));
    GetSchedule_Sleep(&struSchedTime);
    if (memcmp(&pcfg->struSchedTime, &struSchedTime, sizeof(HY_DVR_SCHEDTIME)))
    {
        nRet = SetSchedule_Sleep(&pcfg->struSchedTime);
        if (0 != nRet)
        {
            MPU_ERR("[SetDevSleepInfo]:Set SchedTime error!\n");
        }
    }

	return nRet;
}

static int GetDevSleepControl(const LPHY_DVR_DEVICE_SLEEP_CONTROL pcfg)
{
    if (NULL == pcfg)
    {
        MPU_ERR("NULL POINTER EXCEPTION!\n");
        return -1;
    }
    
	int nRet = IDVR_OK;

	//本结构体是否有效，=0表示无效
	pcfg->iValid = 1;

    //立即睡眠开关
    MPU_SYS_AwakeSchedule_GetDorEnable(&pcfg->nSleepControl);

	return nRet;
}

static int SetDevSleepControl(const LPHY_DVR_DEVICE_SLEEP_CONTROL pcfg)
{
    if (NULL == pcfg)
    {
        MPU_ERR("NULL POINTER EXCEPTION!\n");
        return -1;
    }
    
	int nRet = IDVR_OK;
    
    //本结构体是否有效，=0表示无效
    if (0 == pcfg->iValid)
	{
		MPU_ERR("0 == pcfg->bValid \n");
		return IDVR_CIU_STRUCT_UNVALID;
	}

    //立即睡眠开关
    MPU_SYS_AwakeSchedule_DorEnable(pcfg->nSleepControl);

	return nRet;
}

int GetAuthP2pInfo(HY_DVR_AUTH_INFO_P2P_INFO *ptCfg)
{
    int nRet = 0;
 
    Net_AuthP2pInfo tAuthP2pInfo = {0};
    nRet = MPU_NET_GetAuthP2pInfo(&tAuthP2pInfo);
    if(0 == nRet)
    {
        ptCfg->nValid = 1;
        ptCfg->iP2PHDR = tAuthP2pInfo.iP2PHDR;
    }
    else
    {
        ptCfg->nValid = 0;
    }
	MPU_INFO("GetAuthP2pInfo iP2PHDR=%d\n",ptCfg->iP2PHDR);
    return nRet;
}

int SetAuthP2pInfo(HY_DVR_AUTH_INFO_P2P_INFO *ptCfg)
{
    int nRet = 0;

    if (0 == ptCfg->nValid)
	{
		MPU_ERR("0 == pcfg->nValid\n");
		return IDVR_CIU_STRUCT_UNVALID;
	}
	
    Net_AuthP2pInfo tAuthP2pInfo = {0};
	MPU_INFO("SetAuthP2pInfo iP2PHDR=%d\n", ptCfg->iP2PHDR);
    if(1 == ptCfg->iP2PHDR)
    {
        tAuthP2pInfo.iP2PHDR = 1;
    }
    else
    {
        tAuthP2pInfo.iP2PHDR = 0;
    }
    tAuthP2pInfo.bValidP2pInfo = 1;
    nRet = MPU_Net_SetAuthP2pInfo(&tAuthP2pInfo);
    if (nRet != 0)
	{
		MPU_ERR("Set MPU_Net_SetAuthP2pInfo errno: %d \n", nRet);
		return IDVR_CIU_CALLSDK_FAIL;
	}

    return nRet;
}

//获取流传输方式
static int CIU_Config_Dev_RealStreamBufferType(HY_DVR_STREAM_TRANSPORT_TYPE *cfg)
{
	cfg->bValid = 1;

#ifdef __USE_CIRCLEQUEUE__
	cfg->isPullStream = true;
#else
	cfg->isPullStream = false;
#endif
	    cfg->nIsSupportAddAllUser = true;

	MPU_INFO("CIU_Config_Dev_RealStreamBufferType cfg->isPullStream = %d\n", cfg->isPullStream);
	return 0;
}

int CIU_GetConfigure_DevInfo(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen)
{
	int nRet = IDVR_OK;
	MPU_DBG("ID %d, Major %d, Minor %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType);

	switch (dwMinorType)
	{
			/*获取设备信息(版本及能力) */
		case DEVICEINFO_ALL:
			{
				LPHY_DVR_DEVICE_INFO pcfg = (LPHY_DVR_DEVICE_INFO) pBuffer;

				//判断buf空间是否足够
				if (*pdwBufLen < DEVICE_INFO_LEN)
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", DEVICE_INFO_LEN, *pdwBufLen);
					break;
				}

				memset(pcfg, 0, DEVICE_INFO_LEN);
				*pdwBufLen = DEVICE_INFO_LEN;

				nRet = GetDevInfo(pcfg);
			}
			break;

		/* 获取用户自定义信息 */
		case DEVICEINFO_CUSTOMER:
			{
				LPHY_DVR_CUSTOMER_INFO pcfg = (LPHY_DVR_CUSTOMER_INFO) pBuffer;
				//判断buf空间是否足够
				if (*pdwBufLen < sizeof(HY_DVR_CUSTOMER_INFO))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_CUSTOMER_INFO), *pdwBufLen);
					break;
				}
		
				memset(pcfg, 0, sizeof(HY_DVR_CUSTOMER_INFO));
				*pdwBufLen = sizeof(HY_DVR_CUSTOMER_INFO);
		
				nRet = GetCustomInfo(pcfg);
			}
			break;

		case DEVICEINFO_V2:
			{
				LPHY_DVR_DEVICE_INFO_V2 pcfg = (LPHY_DVR_DEVICE_INFO_V2) pBuffer;
				//判断buf空间是否足够
				if (*pdwBufLen < sizeof(HY_DVR_DEVICE_INFO_V2))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_DEVICE_INFO_V2), *pdwBufLen);
					break;
				}
		
				memset(pcfg, 0, sizeof(HY_DVR_DEVICE_INFO_V2));
				*pdwBufLen = sizeof(HY_DVR_DEVICE_INFO_V2);
		
				nRet = GetDeviceInfoV2(pcfg);
			}
			break;
		
		case CLOUD_UPDATE_DEVICE_INFO:
			{
                HY_DVR_CLOUD_UPDATE_BASIC_INFO *pcfg = (HY_DVR_CLOUD_UPDATE_BASIC_INFO *)pBuffer;
    
                //判断buf空间是否足够
                if (*pdwBufLen < sizeof(HY_DVR_CLOUD_UPDATE_BASIC_INFO))
                {
                    nRet = IDVR_CIU_BUF_SMALL;
                    MPU_ERR("buffer is too small, need %d, real %lu\n", 
                        sizeof(HY_DVR_CLOUD_UPDATE_BASIC_INFO), *pdwBufLen);
                    break;
                }
    
                //memset(pcfg, 0, sizeof(UPGRADE_RESULT));
                *pdwBufLen = sizeof(HY_DVR_CLOUD_UPDATE_BASIC_INFO);
                nRet = MPU_CloudUpgrade_GetUpgradeInfo(pcfg);
                if (0 == nRet)  //获取成功
                {
                    pcfg->nValid = 1;
                }
            }
            break;
			
		case DEVICEINFO_SERIAL_NUM:
			{
				char *pcfg = (char *) pBuffer;
				nRet = GetSeriesNum(pcfg,*pdwBufLen);
			}
			break;

		default:
			{
				nRet = IDVR_CIU_UNSUPPORTED_MINORTYPE;
				MPU_ERR("unsupported dwMinorType = %lu\n", dwMinorType);
			}
			break;
	}

	MPU_DBG("ID %d, Major %d, Minor %d, return %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType, nRet);
	return nRet;
}

int CIU_SetConfigure_DevInfo(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, const BYTE * pBuffer, DWORD dwBufLen)
{
	int nRet = IDVR_OK;
	MPU_DBG("ID %d, Major %d, Minor %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType);

	switch (dwMinorType)
	{
			/*设置设备配置信息 */
		case DEVICEINFO_ALL:
			{
				///////////////////////////////////////////////////////////////////////////////
				//判断buf空间是否足够
				if (dwBufLen < DEVICE_INFO_LEN)
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", DEVICE_INFO_LEN, dwBufLen);
					break;
				}

				nRet = IDVR_CIU_STRUCT_READONLY;
				MPU_ERR("device info is read only\n");
			}
			break;
		/*获取用户自定义信息*/
		case DEVICEINFO_CUSTOMER:
			{
				LPHY_DVR_CUSTOMER_INFO pcfg = (LPHY_DVR_CUSTOMER_INFO) pBuffer;
		
				//判断buf空间是否足够
				if (dwBufLen < sizeof(HY_DVR_CUSTOMER_INFO))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_CUSTOMER_INFO), dwBufLen);
					break;
				}
		
				nRet = SetCustomInfo(pcfg);
			}
			break;

		default:
			{
				nRet = IDVR_CIU_UNSUPPORTED_MINORTYPE;
				MPU_ERR("unsupported dwMinorType = %lu\n", dwMinorType);
			}
			break;
	}

	MPU_DBG("ID %d, Major %d, Minor %d, return %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType, nRet);
	return nRet;
}

int CIU_GetConfigure_BUF_SIZE(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen)
{
	int nRet = IDVR_OK;
	MPU_DBG("ID %d, Major %d, Minor %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType);

	switch (dwMinorType)
	{
			//获取BUFFER SIZE的信息
		case BUFFER_SIZE_ALL:
			{
				///////////////////////////////////////////////////////////////////////////////
				//判断buf空间是否足够
				if (*pdwBufLen < sizeof(HY_DVR_BUFFER_SIZE))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_BUFFER_SIZE), *pdwBufLen);
					break;
				}

				HY_DVR_BUFFER_SIZE *pcfg = (HY_DVR_BUFFER_SIZE *) pBuffer;

				///////////////////////////////////////////////////////////////////////////////
				//初始化输出缓冲区
				memset(pcfg, 0, sizeof(HY_DVR_BUFFER_SIZE));
				*pdwBufLen = sizeof(HY_DVR_BUFFER_SIZE);
				
				nRet = GetIpcBufSzCfg(pcfg);
			}
			break;


		default:
			{
				MPU_ERR("unsupported dwMinorType = %lu\n", dwMinorType);
				nRet = IDVR_CIU_UNSUPPORTED_MINORTYPE;
			}
			break;
	}

	MPU_DBG("ID %d, Major %d, Minor %d, return %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType, nRet);
	return nRet;    
}

int CIU_GetConfigure_Connect_Resource(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen)
{
	int nRet = IDVR_OK;
	MPU_DBG("ID %d, Major %d, Minor %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType);

	switch (dwMinorType)
	{
			//获取BUFFER SIZE的信息
		case CONNECT_RESOURCE_INFO_ALL:
			{
				///////////////////////////////////////////////////////////////////////////////
				//判断buf空间是否足够
				if (*pdwBufLen < sizeof(HY_DVR_CONNECT_RESOURCE_INFO))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_CONNECT_RESOURCE_INFO), *pdwBufLen);
					break;
				}

				HY_DVR_CONNECT_RESOURCE_INFO *pcfg = (HY_DVR_CONNECT_RESOURCE_INFO *) pBuffer;

				///////////////////////////////////////////////////////////////////////////////
				//初始化输出缓冲区
				memset(pcfg, 0, sizeof(HY_DVR_CONNECT_RESOURCE_INFO));
				*pdwBufLen = sizeof(HY_DVR_CONNECT_RESOURCE_INFO);
				
				nRet = GetIpcConnectResource(pcfg);
			}
			break;

		default:
			{
				MPU_ERR("unsupported dwMinorType = %lu\n", dwMinorType);
				nRet = IDVR_CIU_UNSUPPORTED_MINORTYPE;
			}
			break;
	}

	MPU_DBG("ID %d, Major %d, Minor %d, return %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType, nRet);
	return nRet;    
}

int CIU_GetConfigure_DevConfig(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen)
{
	int nRet = IDVR_OK;
	MPU_DBG("ID %d, Major %d, Minor %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType);

	switch (dwMinorType)
	{
			/*获取设备配置信息 */
		case DEVICECFG_ALL:
			{
				LPHY_DVR_DEVICE_CFG pcfg = (LPHY_DVR_DEVICE_CFG) pBuffer;

				//判断buf空间是否足够
				if (*pdwBufLen < DEVICE_CFG_LEN)
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", DEVICE_CFG_LEN, *pdwBufLen);
					break;
				}

				memset(pcfg, 0, DEVICE_CFG_LEN);
				*pdwBufLen = DEVICE_CFG_LEN;

				nRet = GetDevConfig(pcfg);
			}
			break;

        //获取升级列表(阻塞式)
        case CLOUD_UPGRADE_FIRMWARE_LIST:
            {
                UPGRADE_FIRMWARE_INFO_LIST *pcfg = (UPGRADE_FIRMWARE_INFO_LIST *)pBuffer;
    
                //判断buf空间是否足够
                if (*pdwBufLen < sizeof(UPGRADE_FIRMWARE_INFO_LIST))
                {
                    nRet = IDVR_CIU_BUF_SMALL;
                    MPU_ERR("buffer is too small, need %d, real %lu\n", 
                        sizeof(UPGRADE_FIRMWARE_INFO_LIST), *pdwBufLen);
                    break;
                }
    
                pcfg->bValid = 0;
                memset(pcfg->struFileList, 0, sizeof(pcfg->struFileList));
                *pdwBufLen = sizeof(UPGRADE_FIRMWARE_INFO_LIST);
                nRet = MPU_CloudUpgrade_GetUpgradeList(pcfg);
                if (0 == nRet)  //获取成功
                {
                    pcfg->bValid = 1;
                }
            }
            break;
    
        //获取升级包下载状态
        case CLOUD_UPGRADE_FILE_DOWNLOAD_STATUS:
            {
                UPGRADE_FILE_DOWNLOAD_STATUS *pcfg = (UPGRADE_FILE_DOWNLOAD_STATUS *)pBuffer;
    
                //判断buf空间是否足够
                if (*pdwBufLen < sizeof(UPGRADE_FILE_DOWNLOAD_STATUS))
                {
                    nRet = IDVR_CIU_BUF_SMALL;
                    MPU_ERR("buffer is too small, need %d, real %lu\n", 
                        sizeof(UPGRADE_FILE_DOWNLOAD_STATUS), *pdwBufLen);
                    break;
                }
    
                //memset(pcfg, 0, sizeof(UPGRADE_FILE_DOWNLOAD_STATUS));
                *pdwBufLen = sizeof(UPGRADE_FILE_DOWNLOAD_STATUS);
                nRet = MPU_CloudUpgrade_GetUpgradeFileDownloadStatus(pcfg);
                if (0 == nRet)  //获取成功
                {
                    pcfg->bValid = 1;
                }
            }
            break;
    
        //获取升级结果
        case CLOUD_UPGRADE_STATUS:
            {
                UPGRADE_RESULT *pcfg = (UPGRADE_RESULT *)pBuffer;
    
                //判断buf空间是否足够
                if (*pdwBufLen < sizeof(UPGRADE_RESULT))
                {
                    nRet = IDVR_CIU_BUF_SMALL;
                    MPU_ERR("buffer is too small, need %d, real %lu\n", 
                        sizeof(UPGRADE_RESULT), *pdwBufLen);
                    break;
                }
    
                //memset(pcfg, 0, sizeof(UPGRADE_RESULT));
                *pdwBufLen = sizeof(UPGRADE_RESULT);
                nRet = MPU_CloudUpgrade_GetUpgradeResult(pcfg);
                if (0 == nRet)  //获取成功
                {
                    pcfg->bValid = 1;
                }
            }
            break;
    
        // 获取设备工作模式
        case DEVICECFG_SYSTEM_WORK_MODE_CONTROL:
            {
                HY_DVR_SYSTEM_WORK_MODE *pcfg = (HY_DVR_SYSTEM_WORK_MODE *)pBuffer;
    
                //判断buf空间是否足够
                if (*pdwBufLen < sizeof(HY_DVR_SYSTEM_WORK_MODE))
                {
                    nRet = IDVR_CIU_BUF_SMALL;
                    MPU_ERR("buffer is too small, need %d, real %lu\n", 
                        sizeof(HY_DVR_SYSTEM_WORK_MODE), *pdwBufLen);
                    break;
                }
    
                *pdwBufLen = sizeof(HY_DVR_SYSTEM_WORK_MODE);
                pcfg->nValid = 1;
                if (0 == access(TEST_WORK_MODE_FLAG, F_OK))
                {
                    pcfg->nSystemWorkMode = eSystemTestMode;
                }
                else
                {
                    pcfg->nSystemWorkMode = eSystemNormalMode;
                }
                
                nRet = 0;
            }
            break;

	#if defined (__IS_SUPPORT_APP_SHARE__)
        // 获取设备S码
        case DEVICECFG_BIND_CONFIG:
            {
				//判断buf空间是否足够
				if (*pdwBufLen < sizeof(HY_DEVICE_BIND_CONFIG))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", 
						sizeof(HY_DEVICE_BIND_CONFIG), *pdwBufLen);
					break;
				}

				*pdwBufLen = sizeof(HY_DEVICE_BIND_CONFIG);

				pthread_mutex_lock(&g_scode_mutex);
				HY_DEVICE_BIND_CONFIG *pcfg = (HY_DEVICE_BIND_CONFIG *)pBuffer;
                pcfg->nValid = 1;
                int codeLen = sizeof(pcfg->strSCode);
                int rc = MPU_SYS_DeviceBind_GetSCodeOfBindState(pcfg->strSCode, &codeLen);
                pcfg->nSCodeValid = (0 == rc)?1:0;
                nRet = 0;
				pthread_mutex_unlock(&g_scode_mutex);
            }
            break;
	#endif
		case PUSH_EVENT_INTERVAL:
			{
				HY_DVR_PUSH_EVENT_INTERVAL *pcfg = (HY_DVR_PUSH_EVENT_INTERVAL *)pBuffer;
				//判断buf空间是否足够
				if (*pdwBufLen < sizeof(HY_DVR_PUSH_EVENT_INTERVAL))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", 
						sizeof(HY_DVR_PUSH_EVENT_INTERVAL), *pdwBufLen);
					break;
				}

				*pdwBufLen = sizeof(HY_DVR_PUSH_EVENT_INTERVAL);
				
                pcfg->nValid = 1;
				pcfg->nChannelID = 0;
			    MPU_AVL_GetPushEventIntvCnt(pcfg->nChannelID, ALERT_TYPE_MOTION, &pcfg->nMotionInterval);
				MPU_AVL_GetPushEventIntvCnt(pcfg->nChannelID, ALERT_TYPE_HUMANSHAPE, &pcfg->nPeopleInterval);
                MPU_AVL_GetPushEventIntvCnt(pcfg->nChannelID, ALERT_TYPE_SOUNDDETECT, &pcfg->nPeopleInterval);
                //MPU_ERR("pcfg->nMotionInterval :%d pcfg->iAreaDetectInterval :%d pcfg->iCrossBorderDetectInterval :%d \n", pcfg->nMotionInterval, pcfg->iAreaDetectInterval, pcfg->iCrossBorderDetectInterval);
            }
            break;
			
		// 获取辅助接口控制开关状态
		case DEVICECFG_ASSISTANT_INTERFACE_CONTROL:
			{
				HY_DVR_ASSISTANT_INTERFACE *pcfg = (HY_DVR_ASSISTANT_INTERFACE *)pBuffer;

				//判断buf空间是否足够
				if (*pdwBufLen < sizeof(HY_DVR_ASSISTANT_INTERFACE))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", 
						sizeof(HY_DVR_ASSISTANT_INTERFACE), *pdwBufLen);
					break;
				}
                *pdwBufLen = sizeof(HY_DVR_ASSISTANT_INTERFACE);
                // TODO[TestTool] 获取当前辅助接口开关状态
                
                pcfg->nValid = 1;
                switch (pcfg->nAssisstantID)
                {
                    case eWhiteLight:
                        {
                            memcpy(pcfg, &s_stDevAssistants[0], sizeof(HY_DVR_ASSISTANT_INTERFACE));
                        }
                        break;
                    case eBrustLight:
                        {
                            memcpy(pcfg, &s_stDevAssistants[1], sizeof(HY_DVR_ASSISTANT_INTERFACE));
                        }
                        break;
                    case eInfraredLight:
                        {
                            memcpy(pcfg, &s_stDevAssistants[2], sizeof(HY_DVR_ASSISTANT_INTERFACE));
                        }
                        break;
                    default:
                        break;
                }
                nRet = 0;
			}
			break;

        //获取安装信息
        case DEVICECFG_INSTALLTION_CONF_LIST:
            {
                ZW_MULTI_CHANNEL_INSTALLTION_INFO *pcfg = (ZW_MULTI_CHANNEL_INSTALLTION_INFO *)pBuffer;
        
                //判断buf空间是否足够
                if (*pdwBufLen < sizeof(ZW_MULTI_CHANNEL_INSTALLTION_INFO))
                {
                    nRet = IDVR_CIU_BUF_SMALL;
                    MPU_ERR("buffer is too small, need %d, real %lu\n", 
                        sizeof(ZW_MULTI_CHANNEL_INSTALLTION_INFO), *pdwBufLen);
                    break;
                }
        
                memset(pcfg, 0, sizeof(ZW_MULTI_CHANNEL_INSTALLTION_INFO));
                *pdwBufLen = sizeof(ZW_MULTI_CHANNEL_INSTALLTION_INFO);
        
                nRet = GetDevInstallInfo(pcfg);
            }
            break;
		case DEVICECFG_LANGUAGE_CONF:
			{
				HY_DVR_SYSTEM_LANGUAGE_CONFIG *pcfg = (HY_DVR_SYSTEM_LANGUAGE_CONFIG *)pBuffer;

				//判断buf空间是否足够
				if (*pdwBufLen < sizeof(HY_DVR_SYSTEM_LANGUAGE_CONFIG))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", 
						sizeof(HY_DVR_SYSTEM_LANGUAGE_CONFIG), *pdwBufLen);
					break;
				}
		
				memset(pcfg, 0, sizeof(HY_DVR_SYSTEM_LANGUAGE_CONFIG));
				*pdwBufLen = sizeof(HY_DVR_SYSTEM_LANGUAGE_CONFIG);
		
				nRet = GetDevLanguage(pcfg);
			}
			break;	
		
		case HY_DVR_DEVICE_DST_INFO:
			{
				tagHY_DVR_DST_INFO *pcfg = (tagHY_DVR_DST_INFO *)pBuffer;

				//判断buf空间是否足够
				if (*pdwBufLen < sizeof(tagHY_DVR_DST_INFO))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", 
						sizeof(tagHY_DVR_DST_INFO), *pdwBufLen);
					break;
				}
		
				memset(pcfg, 0, sizeof(tagHY_DVR_DST_INFO));
				*pdwBufLen = sizeof(tagHY_DVR_DST_INFO);
		
				nRet = GetDevDST(pcfg);
			}
			break;
			
		default:
			{
				nRet = IDVR_CIU_UNSUPPORTED_MINORTYPE;
				MPU_ERR("unsupported dwMinorType = %lu\n", dwMinorType);
			}
			break;
	}

	MPU_DBG("ID %d, Major %d, Minor %d, return %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType, nRet);
	return nRet;
}

int CIU_SetConfigure_DevConfig(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, const BYTE * pBuffer, DWORD dwBufLen)
{
	int nRet = IDVR_OK;
	MPU_DBG("ID %d, Major %d, Minor %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType);

	switch (dwMinorType)
	{
			/*设置设备配置信息 */
		case DEVICECFG_ALL:
			{
				///////////////////////////////////////////////////////////////////////////////
				//判断buf空间是否足够
				if (dwBufLen < DEVICE_CFG_LEN)
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", DEVICE_CFG_LEN, dwBufLen);
					break;
				}

				LPHY_DVR_DEVICE_CFG pcfg = (LPHY_DVR_DEVICE_CFG) pBuffer;

				nRet = SetDevConfig(pcfg);
			}
			break;

        //设置设备升级列表文件服务器配置
        case CLOUD_UPGRADE_FIRMWARE_LIST_LOCATION:
            {
                ///////////////////////////////////////////////////////////////////////////////
                //判断buf空间是否足够
                if (dwBufLen < sizeof(UPGRADE_LIST_SERVER_CFG))
                {
                    nRet = IDVR_CIU_BUF_SMALL;
                    MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(UPGRADE_LIST_SERVER_CFG), dwBufLen);
                    break;
                }
    
                UPGRADE_LIST_SERVER_CFG *pcfg = (UPGRADE_LIST_SERVER_CFG *)pBuffer;
                //! 本结构体是否有效，=0表示无效
                if (0 == pcfg->bValid)
                {
                    MPU_DBG("0 == pcfg->bValid\n");
                    return IDVR_CIU_STRUCT_UNVALID;
                }
                nRet = MPU_CloudUpgrade_SetUpgradeListServerCfg(pcfg);
            }
            break;
    
        //指定并下载升级包(非阻塞式)
        case CLOUD_UPGRADE_FILE_DOWNLOAD_START:
            {
                ///////////////////////////////////////////////////////////////////////////////
                //判断buf空间是否足够
                if (dwBufLen < sizeof(UPGRADE_FILE_DOWNLOAD_START))
                {
                    nRet = IDVR_CIU_BUF_SMALL;
                    MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(UPGRADE_FILE_DOWNLOAD_START), dwBufLen);
                    break;
                }
    
                UPGRADE_FILE_DOWNLOAD_START *pcfg = (UPGRADE_FILE_DOWNLOAD_START *)pBuffer;
                //! 本结构体是否有效，=0表示无效
                if (0 == pcfg->bValid)
                {
                    MPU_DBG("0 == pcfg->bValid\n");
                    return IDVR_CIU_STRUCT_UNVALID;
                }
                nRet = MPU_CloudUpgrade_UpgradeFileDownloadStart(pcfg);
            }
            break;

	#if defined (__IS_SUPPORT_P6SMESSAGE__)
		case CLOUD_UPGRADE_FILE_DOWNLOAD_START_V1:
			{
				///////////////////////////////////////////////////////////////////////////////
				//判断buf空间是否足够
				if (dwBufLen < sizeof(UPGRADE_FILE_DOWNLOAD_START_V1))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(UPGRADE_FILE_DOWNLOAD_START_V1), dwBufLen);
					break;
				}

				UPGRADE_FILE_DOWNLOAD_START_V1 *pcfg = (UPGRADE_FILE_DOWNLOAD_START_V1 *)pBuffer;
				//! 本结构体是否有效，=0表示无效
				if (0 == pcfg->bValid)
				{
					MPU_DBG("0 == pcfg->bValid\n");
					return IDVR_CIU_STRUCT_UNVALID;
				}
				nRet = MPU_CloudUpgrade_UpgradeFileDownloadStart_V1(pcfg);
			}
			break;
	#endif
    
        //开始升级(非阻塞式)
        //真正开始升级前,通过返回值通知APP状态(校验失败/校验通过,开始升级)
        case CLOUD_UPGRADE_START_UPGRADE:
            {
                ///////////////////////////////////////////////////////////////////////////////
                //判断buf空间是否足够
                if (dwBufLen < sizeof(UPGRADE_CONTROL_START))
                {
                    nRet = IDVR_CIU_BUF_SMALL;
                    MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(UPGRADE_CONTROL_START), dwBufLen);
                    break;
                }
    
                UPGRADE_CONTROL_START *pcfg = (UPGRADE_CONTROL_START *)pBuffer;
                //! 本结构体是否有效，=0表示无效
                if (0 == pcfg->bValid)
                {
                    MPU_DBG("0 == pcfg->bValid\n");
                    return IDVR_CIU_STRUCT_UNVALID;
                }
                nRet = MPU_CloudUpgrade_UpgradeStart(pcfg);
            }
            break;
    
        case CLOUD_UPGRADE_FILE_DOWNLOAD_STOP:
            {
                ///////////////////////////////////////////////////////////////////////////////
                //判断buf空间是否足够
                if (dwBufLen < sizeof(UPGRADE_FILE_DOWNLOAD_STOP))
                {
                    nRet = IDVR_CIU_BUF_SMALL;
                    MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(UPGRADE_FILE_DOWNLOAD_STOP), dwBufLen);
                    break;
                }
    
                UPGRADE_FILE_DOWNLOAD_STOP *pcfg = (UPGRADE_FILE_DOWNLOAD_STOP *)pBuffer;
                //! 本结构体是否有效，=0表示无效
                if (0 == pcfg->bValid)
                {
                    MPU_DBG("0 == pcfg->bValid\n");
                    return IDVR_CIU_STRUCT_UNVALID;
                }
                nRet = MPU_CloudUpgrade_UpgradeFileDownloadStop(pcfg);
            }
            break;
    
        // 设置设备当前工作模式
        case DEVICECFG_SYSTEM_WORK_MODE_CONTROL:
            {
                ///////////////////////////////////////////////////////////////////////////////
                //判断buf空间是否足够
                if (dwBufLen < sizeof(HY_DVR_SYSTEM_WORK_MODE))
                {
                    nRet = IDVR_CIU_BUF_SMALL;
                    MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_SYSTEM_WORK_MODE), dwBufLen);
                    break;
                }
    
                HY_DVR_SYSTEM_WORK_MODE *pcfg = (HY_DVR_SYSTEM_WORK_MODE *)pBuffer;
    
                printf("[TestTool][%s %d]Valid[%d] Status[%d]\n", \
                __FUNCTION__, __LINE__, pcfg->nValid, pcfg->nSystemWorkMode);
                int bTestWorkMode = 0;
                if (0 == access(TEST_WORK_MODE_FLAG, F_OK))
                {
                    bTestWorkMode = 1;
                }
                // 设备每次启动默认是正式模式,切换到测试模式后,就不能再切正式模式.
                if ((0 == bTestWorkMode) && (pcfg->nSystemWorkMode == eSystemTestMode))
                {
                    // 测试模式下不保存参数到配置中
                    CConfig *pConfig = CConfig::GetInstance();
                    if (NULL != pConfig)
                    {
                        pConfig->SaveFileReal();
                        pConfig->ForbiddenSaveFile();
                    }
                    char command[256] = {0};
                    snprintf(command, sizeof(command), "touch %s", TEST_WORK_MODE_FLAG);
                    mysystem(command);
                }
    
                nRet = 0;
            }
            break;

		// 设置辅助接口控制开关
		case DEVICECFG_ASSISTANT_INTERFACE_CONTROL:
			{
				///////////////////////////////////////////////////////////////////////////////
				//判断buf空间是否足够
				if (dwBufLen < sizeof(HY_DVR_ASSISTANT_INTERFACE))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_ASSISTANT_INTERFACE), dwBufLen);
					break;
				}

				HY_DVR_ASSISTANT_INTERFACE *pcfg = (HY_DVR_ASSISTANT_INTERFACE *)pBuffer;

                // TODO[TestTool]:设置开关状态
                printf("[TestTool][%s %d]Valid[%d] Channel[%d] Cmd[%d] Status[%d]\n", \
                __FUNCTION__, __LINE__, pcfg->nValid, pcfg->nChannelID, pcfg->nAssisstantID, pcfg->nAssistantInterfaceStatus);

                if (pcfg->nValid)
                {
                    switch (pcfg->nAssisstantID) {
                        case eWhiteLight:
                            {
                                s_stDevAssistants[0].nAssistantInterfaceStatus = pcfg->nAssistantInterfaceStatus;
                                AssistantCtrlInfo stAssitInfo;
                                stAssitInfo.enAssisstantType = PDEV_WHITE_LED;
                                stAssitInfo.nCtrlStatus = s_stDevAssistants[0].nAssistantInterfaceStatus;
                                ZWSys_AssistantControl(&stAssitInfo);
                            }
                            break;
                        case eBrustLight:
                            {
                                s_stDevAssistants[1].nAssistantInterfaceStatus = pcfg->nAssistantInterfaceStatus;
                                AssistantCtrlInfo stAssitInfo;
                                stAssitInfo.enAssisstantType = PDEV_RB_WARN_LED;
                                stAssitInfo.nCtrlStatus = s_stDevAssistants[1].nAssistantInterfaceStatus;
                                ZWSys_AssistantControl(&stAssitInfo);
                            }
                            break;
                        case eInfraredLight:
                            {
                                s_stDevAssistants[2].nAssistantInterfaceStatus = pcfg->nAssistantInterfaceStatus;
                                AssistantCtrlInfo stAssitInfo;
                                stAssitInfo.enAssisstantType = PDEV_INFRARED_LED;
                                stAssitInfo.nCtrlStatus = s_stDevAssistants[2].nAssistantInterfaceStatus;
                                ZWSys_AssistantControl(&stAssitInfo);
                            }
                            break;
                        default:
                            MPU_ERR("UnSupport nAssisstantID[%d]\n", pcfg->nAssisstantID);
                            break;
                    }
                }

                nRet = 0;
			}
			break;

        //设置安装信息
        case DEVICECFG_INSTALLTION_CONF_LIST:
            {
                //判断buf空间是否足够
                if (dwBufLen < sizeof(ZW_MULTI_CHANNEL_INSTALLTION_INFO))
                {
                    nRet = IDVR_CIU_BUF_SMALL;
                    MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(ZW_MULTI_CHANNEL_INSTALLTION_INFO), dwBufLen);
                    break;
                }
        
                ZW_MULTI_CHANNEL_INSTALLTION_INFO *pcfg = (ZW_MULTI_CHANNEL_INSTALLTION_INFO *) pBuffer;
        
                nRet = SetDevInstallInfo(pcfg);
            }
            break;

	#if defined (__IS_SUPPORT_APP_SHARE__)
        // 设置设备S码
		case DEVICECFG_BIND_CONFIG:
			{
				///////////////////////////////////////////////////////////////////////////////
				//判断buf空间是否足够
				if (dwBufLen < sizeof(HY_DEVICE_BIND_CONFIG))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DEVICE_BIND_CONFIG), dwBufLen);
					break;
				}

				pthread_mutex_lock(&g_scode_mutex);
				HY_DEVICE_BIND_CONFIG *pcfg = (HY_DEVICE_BIND_CONFIG *)pBuffer;

                printf("[TestTool][%s %d]Valid[%d] SCode[%s]\n", \
                __FUNCTION__, __LINE__, pcfg->nValid, pcfg->strSCode);

                MPU_SYS_DeviceBind_SetSCode(pcfg->strSCode, strlen(pcfg->strSCode));
				pthread_mutex_unlock(&g_scode_mutex);

                MPU_LCD_SetDevBindConnetStatus(2);
                nRet = 0;
			}
			break;
		case DEVICECFG_SCODE_VALIDATE:
			{
				///////////////////////////////////////////////////////////////////////////////
				//判断buf空间是否足够
				if (dwBufLen < sizeof(HY_DEVICE_BIND_CONFIG))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DEVICE_BIND_CONFIG), dwBufLen);
					break;
				}

				pthread_mutex_lock(&g_scode_mutex);
				HY_DEVICE_BIND_CONFIG *pcfg = (HY_DEVICE_BIND_CONFIG *)pBuffer;

				nRet = IDVR_SYS_BIND_ERR_SCODE;
				if(1 == pcfg->nValid)
				{
					char szSCode[64] = {0};
					int nSCodeLen = sizeof(szSCode);
					int rc = MPU_SYS_DeviceBind_GetSCode(szSCode, &nSCodeLen);
					if ((0 == rc) && (0 == strcmp(pcfg->strSCode, szSCode)))
					{
						nRet = 0;
					}
					else
					{
						memset(szSCode, 0, sizeof(szSCode));
						rc = MPU_SYS_DeviceBind_GetSCodeBak(szSCode, &nSCodeLen);
						if ((0 == rc) && (0 == strcmp(pcfg->strSCode, szSCode)))
						{
							MPU_SYS_DeviceBind_SetSCode(szSCode, strlen(szSCode));
							nRet = 0;
						}
					}

				}
				pthread_mutex_unlock(&g_scode_mutex);
				printf("[CheckSCodeValidate][%s %d]Valid[%d] SCode[%s] nRet[%d]\n", \
						__FUNCTION__, __LINE__, pcfg->nValid, pcfg->strSCode, nRet);
			}
			break;
	#endif
		case PUSH_EVENT_INTERVAL:
			{
				//判断buf空间是否足够
				if (dwBufLen < sizeof(HY_DVR_PUSH_EVENT_INTERVAL))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_PUSH_EVENT_INTERVAL), dwBufLen);
					break;
				}

				HY_DVR_PUSH_EVENT_INTERVAL *pcfg = (HY_DVR_PUSH_EVENT_INTERVAL *)pBuffer;
				//! 本结构体是否有效，=0表示无效
				if (0 == pcfg->nValid)
				{
					MPU_DBG("0 == pcfg->bValid\n");
					return IDVR_CIU_STRUCT_UNVALID;
				}
				MPU_AVL_SetPushEventIntvCnt(pcfg->nChannelID, ALERT_TYPE_MOTION, pcfg->nMotionInterval);
				//MPU_AVL_SetPushEventIntvCnt(pcfg->nChannelID, ALERT_TYPE_HUMANSHAPE, pcfg->nAssistLineInterval);  //人形，和拌线一致，直接用人形进行设置/获取
				MPU_AVL_SetPushEventIntvCnt(pcfg->nChannelID, ALERT_TYPE_HUMANSHAPE, pcfg->nPeopleInterval);
                MPU_AVL_SetPushEventIntvCnt(pcfg->nChannelID, ALERT_TYPE_SOUNDDETECT, pcfg->nPeopleInterval);
            }
            break;

            default:
            {
				nRet = IDVR_CIU_UNSUPPORTED_MINORTYPE;
				MPU_ERR("unsupported dwMinorType = %lu\n", dwMinorType);
			}
			break;
	}

	if (IDVR_OK == nRet)
	{
		//记录日志，设置设备基本参数
		CIU_OPL_AddLog(dwUserID, EDVR_EVENT_SETTING, EDVR_EVENT_SETTING_DETAIL_COMMON_SETTING, 0, 0);
	}

	MPU_DBG("ID %d, Major %d, Minor %d, return %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType, nRet);
	return nRet;
}

//!\brief   远程配置，获取系统时间
int CIU_GetConfigure_Time(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen)
{
	int nRet = IDVR_OK;
	MPU_DBG("ID %d, Major %d, Minor %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType);

	switch (dwMinorType)
	{
			/*获取系统时间 */
		case SYSTIME_ALL:
			{
				LPHY_DVR_TIME pcfg = (LPHY_DVR_TIME) pBuffer;

				//判断buf空间是否足够
				if (*pdwBufLen < TIME_CFG_LEN)
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", TIME_CFG_LEN, *pdwBufLen);
					break;
				}

				memset(pcfg, 0, TIME_CFG_LEN);
				*pdwBufLen = TIME_CFG_LEN;

				nRet = GetSystemTime(pcfg);
			}
			break;

		default:
			{
				nRet = IDVR_CIU_UNSUPPORTED_MINORTYPE;
				MPU_ERR("unsupported dwMinorType = %lu\n", dwMinorType);
			}
			break;
	}

	MPU_DBG("ID %d, Major %d, Minor %d, return %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType, nRet);
	return nRet;

}

//!\brief   远程配置，设置系统时间
int CIU_SetConfigure_Time(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, const BYTE * pBuffer, DWORD dwBufLen)
{
	int nRet = IDVR_OK;
	MPU_DBG("ID %d, Major %d, Minor %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType);

	switch (dwMinorType)
	{
			/*设置系统时间 */
		case SYSTIME_ALL:
			{
				///////////////////////////////////////////////////////////////////////////////
				//判断buf空间是否足够
				if (dwBufLen < TIME_CFG_LEN)
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", TIME_CFG_LEN, dwBufLen);
					break;
				}

				LPHY_DVR_TIME pcfg = (LPHY_DVR_TIME) pBuffer;

				nRet = SetSystemTime(pcfg);
				
			}
			break;

		default:
			{
				nRet = IDVR_CIU_UNSUPPORTED_MINORTYPE;
				MPU_ERR("unsupported dwMinorType = %lu\n", dwMinorType);
			}
			break;
	}

	MPU_DBG("ID %d, Major %d, Minor %d, return %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType, nRet);
	return nRet;
}

//!\brief   远程配置，获取显示输出参数
int CIU_GetConfigure_VideoOut(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen)
{
	int nRet = IDVR_OK;
	MPU_DBG("ID %d, Major %d, Minor %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType);

	switch (dwMinorType)
	{
			/*获取设备显示信息 */
		case VIDEOOUTCFG_ALL:
			{
				LPHY_DVR_VO_CFG pcfg = (LPHY_DVR_VO_CFG) pBuffer;

				//判断buf空间是否足够
				if (*pdwBufLen < VO_CFG_LEN)
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", VO_CFG_LEN, *pdwBufLen);
					break;
				}

				memset(pcfg, 0, VO_CFG_LEN);
				*pdwBufLen = VO_CFG_LEN;

				nRet = GetVoConfig(pcfg);
			}
			break;


		default:
			{
				nRet = IDVR_CIU_UNSUPPORTED_MAJORTYPE;
				MPU_ERR("unsupported dwMinorType = %lu\n", dwMinorType);
			}
			break;
	}

	MPU_DBG("ID %d, Major %d, Minor %d, return %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType, nRet);
	return nRet;
}

//!\brief   远程配置，设置显示输出参数
int CIU_SetConfigure_VideoOut(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, const BYTE * pBuffer, DWORD dwBufLen)
{
	int nRet = IDVR_OK;
	MPU_DBG("ID %d, Major %d, Minor %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType);

	switch (dwMinorType)
	{
			/*设置设备显示信息 */
		case VIDEOOUTCFG_ALL:
			{
				///////////////////////////////////////////////////////////////////////////////
				//判断buf空间是否足够
				if (dwBufLen < VO_CFG_LEN)
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", VO_CFG_LEN, dwBufLen);
					break;
				}

				LPHY_DVR_VO_CFG pcfg = (LPHY_DVR_VO_CFG) pBuffer;

				nRet = SetVoConfig(pcfg);
			}
			break;

		default:
			{
				nRet = IDVR_CIU_UNSUPPORTED_MINORTYPE;
				MPU_ERR("unsupported dwMinorType = %lu\n", dwMinorType);
			}
			break;
	}

	MPU_DBG("ID %d, Major %d, Minor %d, return %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType, nRet);
	return nRet;
}

//!\brief   远程配置，获取系统状态
int CIU_GetConfigure_DevState(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen)
{
	int nRet = IDVR_OK;
	//MPU_DBG("ID %d, Major %d, Minor %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType);

	switch (dwMinorType)
	{
			/*获取设备显示信息 */
		case DEVICESTATE_ALL:
			{
				LPHY_DVR_DEVICE_STATE pcfg = (LPHY_DVR_DEVICE_STATE) pBuffer;

				//判断buf空间是否足够
				if (*pdwBufLen < DEVICE_STATE_LEN)
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", DEVICE_STATE_LEN, *pdwBufLen);
					break;
				}

				memset(pcfg, 0, DEVICE_STATE_LEN);
				*pdwBufLen = DEVICE_STATE_LEN;

				nRet = GetDevState(pcfg);
			}
			break;

		case CONNECT_STATE_ALL:
			{
				HY_DVR_CONNECT_STATE *pcfg = (HY_DVR_CONNECT_STATE *) pBuffer;
				
				//判断buf空间是否足够
				if (*pdwBufLen < sizeof(HY_DVR_CONNECT_STATE))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_CONNECT_STATE), *pdwBufLen);
					break;
				}
				
				memset(pcfg, 0, sizeof(HY_DVR_CONNECT_STATE));
				*pdwBufLen = sizeof(HY_DVR_CONNECT_STATE);
				
				nRet = GetIpcRealStreamConnectResource(pcfg);
			}
			break;

		default:
			{
				nRet = IDVR_CIU_UNSUPPORTED_MINORTYPE;
				MPU_ERR("unsupported dwMinorType = %lu\n", dwMinorType);
			}
			break;
	}


	//MPU_DBG("ID %d, Major %d, Minor %d, return %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType, nRet);
	return nRet;
}

//!\brief   远程配置，获取设备自动维护配置信息
int CIU_GetConfigure_DevMaintenance(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen)
{
	int nRet = IDVR_OK;
	MPU_DBG("ID %d, Major %d, Minor %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType);

	switch (dwMinorType)
	{
			/*获取设备自动维护配置信息 */
		case DEVICEMAINTENANCE_ALL:
			{
				LPHY_DVR_DEVICE_MAINTENANCE pcfg = (LPHY_DVR_DEVICE_MAINTENANCE) pBuffer;

				//判断buf空间是否足够
				if (*pdwBufLen < DEVICE_MAINTENANCE_LEN)
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", DEVICE_MAINTENANCE_LEN, *pdwBufLen);
					break;
				}

				memset(pcfg, 0, DEVICE_MAINTENANCE_LEN);
				*pdwBufLen = DEVICE_MAINTENANCE_LEN;

				nRet = GetDevMaintenance(pcfg);
			}
			break;


		default:
			{
				nRet = IDVR_CIU_UNSUPPORTED_MINORTYPE;
				MPU_ERR("unsupported dwMinorType = %lu\n", dwMinorType);
			}
			break;
	}

	MPU_DBG("ID %d, Major %d, Minor %d, return %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType, nRet);
	return nRet;

}

//!\brief   远程配置，设置设备自动维护配置信息
int CIU_SetConfigure_DevMaintenance(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, const BYTE * pBuffer, DWORD dwBufLen)
{
	int nRet = IDVR_OK;
	MPU_DBG("ID %d, Major %d, Minor %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType);

	switch (dwMinorType)
	{
			/*设置设备自动维护配置信息 */
		case DEVICEMAINTENANCE_ALL:
			{
				///////////////////////////////////////////////////////////////////////////////
				//判断buf空间是否足够
				if (dwBufLen < DEVICE_MAINTENANCE_LEN)
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", DEVICE_MAINTENANCE_LEN, dwBufLen);
					break;
				}

				LPHY_DVR_DEVICE_MAINTENANCE pcfg = (LPHY_DVR_DEVICE_MAINTENANCE) pBuffer;

				nRet = SetDevMaintenance(pcfg);
			}
			break;

		default:
			{
				nRet = IDVR_CIU_UNSUPPORTED_MINORTYPE;
				MPU_ERR("unsupported dwMinorType = %lu\n", dwMinorType);
			}
			break;
	}

	if (IDVR_OK == nRet)
	{
		//记录日志，设置自动维护参数
		CIU_OPL_AddLog(dwUserID, EDVR_EVENT_SETTING, EDVR_EVENT_SETTING_DETAIL_MAINTAIN_SETTING, 0, 0);
	}

	MPU_DBG("ID %d, Major %d, Minor %d, return %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType, nRet);
	return nRet;
}

int CIU_GetDeviceCapacity(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen)
{
	int nRet = IDVR_OK;
	MPU_DBG("ID %d, Major %d, Minor %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType);

	switch (dwMinorType)
	{
			//获取BUFFER SIZE的信息
		case DEVICE_CAPACITY_ALL:
			{
				///////////////////////////////////////////////////////////////////////////////
				//判断buf空间是否足够
				if (*pdwBufLen < sizeof(HY_DVR_DEVICE_CAP_FILE_NAME))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_DEVICE_CAP_FILE_NAME), *pdwBufLen);
					break;
				}

				HY_DVR_DEVICE_CAP_FILE_NAME *pcfg = (HY_DVR_DEVICE_CAP_FILE_NAME *) pBuffer;

				///////////////////////////////////////////////////////////////////////////////
				//初始化输出缓冲区
				memset(pcfg, 0, sizeof(HY_DVR_DEVICE_CAP_FILE_NAME));
				*pdwBufLen = sizeof(HY_DVR_DEVICE_CAP_FILE_NAME);
				
				nRet = GetDeviceCapacity(pcfg);
			}
			break;


		default:
			{
				MPU_ERR("unsupported dwMinorType = %lu\n", dwMinorType);
				nRet = IDVR_CIU_UNSUPPORTED_MINORTYPE;
			}
			break;
	}

	MPU_DBG("ID %d, Major %d, Minor %d, return %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType, nRet);
	return nRet;    
}

int CIU_GetDeviceCapacity_V2(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen)
{
	int nRet = IDVR_OK;
	MPU_DBG("ID %d, Major %d, Minor %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType);

	switch (dwMinorType)
	{
			//获取BUFFER SIZE的信息
		case DEVICE_CAPACITY_V2_ALL:
			{
				///////////////////////////////////////////////////////////////////////////////
				//判断buf空间是否足够
				if (*pdwBufLen < sizeof(HY_DVR_DEVICE_CAP_FILE_NAME_V2))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_DEVICE_CAP_FILE_NAME_V2), *pdwBufLen);
					break;
				}

				HY_DVR_DEVICE_CAP_FILE_NAME_V2 *pcfg = (HY_DVR_DEVICE_CAP_FILE_NAME_V2 *) pBuffer;

				///////////////////////////////////////////////////////////////////////////////
				//初始化输出缓冲区
				memset(pcfg, 0, sizeof(HY_DVR_DEVICE_CAP_FILE_NAME_V2));
				*pdwBufLen = sizeof(HY_DVR_DEVICE_CAP_FILE_NAME_V2);
				
				nRet = GetDeviceCapacity_V2(pcfg);
			}
			break;

		//获取AI信息
		case DEVICE_AI_CAPACITY:
			{
				///////////////////////////////////////////////////////////////////////////////
				//判断buf空间是否足够
				if (*pdwBufLen < sizeof(HY_DVR_DEVICE_CAP_FILE_NAME_V2))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_DEVICE_CAP_FILE_NAME_V2), *pdwBufLen);
					break;
				}
		
				HY_DVR_DEVICE_CAP_FILE_NAME_V2 *pcfg = (HY_DVR_DEVICE_CAP_FILE_NAME_V2 *) pBuffer;
		
				///////////////////////////////////////////////////////////////////////////////
				//初始化输出缓冲区
				memset(pcfg, 0, sizeof(HY_DVR_DEVICE_CAP_FILE_NAME_V2));
				*pdwBufLen = sizeof(HY_DVR_DEVICE_CAP_FILE_NAME_V2);
				
				nRet = GetAIDetectDeviceCapacity(pcfg);
			}
			break;

        //获取灯板的信息
		case DEVICE_LIGHT_CAPACITY:
			{
				///////////////////////////////////////////////////////////////////////////////
				//判断buf空间是否足够
				if (*pdwBufLen < sizeof(HY_DVR_DEVICE_CAP_FILE_NAME_V2))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_DEVICE_CAP_FILE_NAME_V2), *pdwBufLen);
					break;
				}

				HY_DVR_DEVICE_CAP_FILE_NAME_V2 *pcfg = (HY_DVR_DEVICE_CAP_FILE_NAME_V2 *) pBuffer;

				///////////////////////////////////////////////////////////////////////////////
				//初始化输出缓冲区
				memset(pcfg, 0, sizeof(HY_DVR_DEVICE_CAP_FILE_NAME_V2));
				*pdwBufLen = sizeof(HY_DVR_DEVICE_CAP_FILE_NAME_V2);
				
				nRet = GetDeviceCapacity_V2_1(pcfg);
			}
			break;

        case DEVICE_PTZ_CAPACITY:
            {
                ///////////////////////////////////////////////////////////////////////////////
                //判断buf空间是否足够
                if (*pdwBufLen < sizeof(HY_DVR_DEVICE_CAP_FILE_NAME_V2))
                {
                    nRet = IDVR_CIU_BUF_SMALL;
                    MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_DEVICE_CAP_FILE_NAME_V2), *pdwBufLen);
                    break;
                }
        
                HY_DVR_DEVICE_CAP_FILE_NAME_V2 *pcfg = (HY_DVR_DEVICE_CAP_FILE_NAME_V2 *) pBuffer;
        
                ///////////////////////////////////////////////////////////////////////////////
                //初始化输出缓冲区
                memset(pcfg, 0, sizeof(HY_DVR_DEVICE_CAP_FILE_NAME_V2));
                *pdwBufLen = sizeof(HY_DVR_DEVICE_CAP_FILE_NAME_V2);
                
                nRet = GetDeviceCapacity_V2_2(pcfg);
            }
            break;
		default:
			{
				MPU_ERR("unsupported dwMinorType = %lu\n", dwMinorType);
				nRet = IDVR_CIU_UNSUPPORTED_MINORTYPE;
			}
			break;
	}


	MPU_DBG("ID %d, Major %d, Minor %d, return %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType, nRet);
	return nRet;    
}

int CIU_GetConfigure_DeviceCustomFunction(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen)
{
	int nRet = IDVR_OK;

	MPU_DBG("ID %d, Major %d, Minor %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType);

	switch (dwMinorType)
	{
		case DEVICECUSTOM_ALL:
		{
			memset(pBuffer, 0, *pdwBufLen);

			nRet = GetDeviceFunction((char *)pBuffer);
		}
		break;
		case DEVICE_TEST_URL:
		{
			//协议固定pdwBufLen长度为2K与工具定义的buf长度对齐
			memset(pBuffer, 0, *pdwBufLen);

			nRet = GetProductToolCmd((char *)pBuffer,*pdwBufLen);
		}
		break;

    	default:
		{
			nRet = IDVR_CIU_UNSUPPORTED_MINORTYPE;
			MPU_ERR("unsupported dwMinorType = %lu\n", dwMinorType);
		}
		break;
	}

	MPU_DBG("ID %d, Major %d, Minor %d, return %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType, nRet);

	return nRet;
}

int CIU_SetConfigure_DeviceCustomFunction(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, const BYTE * pBuffer, DWORD dwBufLen)
{
    int nRet = IDVR_OK;

	MPU_DBG("ID %d, Major %d, Minor %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType);

	switch (dwMinorType)
	{ 
		case DEVICECUSTOM_ALL:
		{
            nRet = SetDeviceFunction((const char *)pBuffer);
		}  
		 	break;
		case DEVICE_TEST_URL:
		{
			nRet = SetProductToolCmd((char *)pBuffer);
		}
			break;
		
		default:
			{
				MPU_ERR("unsupported dwMinorType = %lu\n", dwMinorType);
				nRet = IDVR_CIU_UNSUPPORTED_MINORTYPE;
			}
			break;
	}
	MPU_DBG("ID %d, Major %d, Minor %d, return %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType, nRet);
	return nRet;
}

#if 0
//!\brief   远程配置，获取设备功能定制配置信息
int CIU_GetConfigure_DevCustom(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen)
{
	int nRet = IDVR_OK;

	MPU_INFO("ID %d, Major %d, Minor %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType);

	switch (dwMinorType)
	{
			/*获取设备自动维护配置信息 */
		case DEVICECUSTOM_ALL:
			{
				LPHY_DVR_DEVICE_CUSTOM pcfg = (LPHY_DVR_DEVICE_CUSTOM) pBuffer;

				//判断buf空间是否足够
				if (*pdwBufLen < DEVICE_CUSTOM_LEN)
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", DEVICE_CUSTOM_LEN, *pdwBufLen);
					break;
				}

				memset(pcfg, 0, DEVICE_CUSTOM_LEN);
				*pdwBufLen = DEVICE_CUSTOM_LEN;

				nRet = GetDevCustom(pcfg);
			}
			break;


		default:
			{
				nRet = IDVR_CIU_UNSUPPORTED_MINORTYPE;
				MPU_ERR("unsupported dwMinorType = %lu\n", dwMinorType);
			}
			break;
	}


	MPU_INFO("ID %d, Major %d, Minor %d, return %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType, nRet);
	return nRet;

}

//!\brief   远程配置，设置设备功能定制配置信息
int CIU_SetConfigure_DevCustom(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, const BYTE * pBuffer, DWORD dwBufLen)
{
	int nRet = IDVR_OK;

	MPU_INFO("ID %d, Major %d, Minor %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType);

	switch (dwMinorType)
	{
			/*设置设备功能定制配置信息 */
		case DEVICECUSTOM_ALL:
			{
				///////////////////////////////////////////////////////////////////////////////
				//判断buf空间是否足够
				if (dwBufLen < DEVICE_CUSTOM_LEN)
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", DEVICE_CUSTOM_LEN, dwBufLen);
					break;
				}

				LPHY_DVR_DEVICE_CUSTOM pcfg = (LPHY_DVR_DEVICE_CUSTOM) pBuffer;

				nRet = SetDevCustom(dwUserID, pcfg);
			}
			break;

		default:
			{
				nRet = IDVR_CIU_UNSUPPORTED_MINORTYPE;
				MPU_ERR("unsupported dwMinorType = %lu\n", dwMinorType);
			}
			break;
	}

	//功能订制，记录日志
	if (IDVR_OK == nRet)
	{
		CIU_OPL_AddLog(dwUserID, EDVR_EVENT_SETTING, EDVR_EVENT_SETTING_DETAIL_CUSTOMIZE_FUNCTION, 0, 0);
	}

	MPU_INFO("ID %d, Major %d, Minor %d, return %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType, nRet);
	return nRet;

}
#endif

/* 获取授权信息 */
int CIU_GetConfigure_AUTH_INFO(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen)
{
	int nRet = IDVR_OK;

	MPU_INFO("ID %d, Major %d, Minor %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType);

	switch (dwMinorType)
	{
			//获取BUFFER SIZE的信息
		case AUTH_INFO_ALL:
			{
				///////////////////////////////////////////////////////////////////////////////
				//判断buf空间是否足够
				if (*pdwBufLen < sizeof(HY_DVR_AUTH_INFO))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_AUTH_INFO), *pdwBufLen);
					break;
				}

				HY_DVR_AUTH_INFO *pcfg = (HY_DVR_AUTH_INFO *) pBuffer;

				///////////////////////////////////////////////////////////////////////////////
				//初始化输出缓冲区
				memset(pcfg, 0, sizeof(HY_DVR_AUTH_INFO));
				*pdwBufLen = sizeof(HY_DVR_AUTH_INFO);
				
				nRet = GetAuthInfoCfg(pcfg);
			}
			break;

#if defined (__AUTH_V2__)
		case AUTH_INFO_ALL_V2:
			{
				///////////////////////////////////////////////////////////////////////////////
				//判断buf空间是否足够
				if (*pdwBufLen < sizeof(HY_DVR_AUTH_INFO_V2))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_AUTH_INFO_V2), *pdwBufLen);
					break;
				}

				HY_DVR_AUTH_INFO_V2 *pcfg = (HY_DVR_AUTH_INFO_V2 *) pBuffer;

				///////////////////////////////////////////////////////////////////////////////
				//初始化输出缓冲区
				memset(pcfg, 0, sizeof(HY_DVR_AUTH_INFO_V2));
				*pdwBufLen = sizeof(HY_DVR_AUTH_INFO_V2);
				
				nRet = GetAuthInfoCfgV2(pcfg);
			}
			break;
#endif
		case AUTH_INFO_TRANSPARENT_INFO:
			{
				///////////////////////////////////////////////////////////////////////////////
				//判断buf空间是否足够
				if (*pdwBufLen < sizeof(HY_DVR_AUTH_INFO_TRANSPARENT_INFO))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_AUTH_INFO_TRANSPARENT_INFO), *pdwBufLen);
					break;
				}

				HY_DVR_AUTH_INFO_TRANSPARENT_INFO *pcfg = (HY_DVR_AUTH_INFO_TRANSPARENT_INFO *) pBuffer;

				///////////////////////////////////////////////////////////////////////////////
				//初始化输出缓冲区
				memset(pcfg, 0, sizeof(HY_DVR_AUTH_INFO_TRANSPARENT_INFO));
				*pdwBufLen = sizeof(HY_DVR_AUTH_INFO_TRANSPARENT_INFO);
				nRet = GetAuthInfoTransparentInfo(pcfg);
			}
			break;

        case AUTH_INFO_P2P_INFO:
            {
                ///////////////////////////////////////////////////////////////////////////////
    			//判断buf空间是否足够
    			if (*pdwBufLen < sizeof(HY_DVR_AUTH_INFO_P2P_INFO))
    			{
    				nRet = IDVR_CIU_BUF_SMALL;
    				MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_AUTH_INFO_P2P_INFO), *pdwBufLen);
    				break;
    			}

    			HY_DVR_AUTH_INFO_P2P_INFO *pcfg = (HY_DVR_AUTH_INFO_P2P_INFO *) pBuffer;

    			///////////////////////////////////////////////////////////////////////////////
    			//初始化输出缓冲区
    			memset(pcfg, 0, sizeof(HY_DVR_AUTH_INFO_P2P_INFO));
    			*pdwBufLen = sizeof(HY_DVR_AUTH_INFO_P2P_INFO);
    			
    			nRet = GetAuthP2pInfo(pcfg);
            }
            break;
       
			
		default:
			{
				MPU_ERR("unsupported dwMinorType = %lu\n", dwMinorType);
				nRet = IDVR_CIU_UNSUPPORTED_MINORTYPE;
			}
			break;
	}


	MPU_INFO("ID %d, Major %d, Minor %d, return %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType, nRet);
	return nRet;    
}

/* 设置授权信息 */
int CIU_SetConfigure_AUTH_INFO(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, const BYTE * pBuffer, DWORD  dwBufLen)
{
	int nRet = IDVR_OK;

	MPU_INFO("ID %d, Major %d, Minor %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType);

	#if 1
	switch (dwMinorType)
	{
			//获取BUFFER SIZE的信息
		case AUTH_INFO_ALL:
			{
				///////////////////////////////////////////////////////////////////////////////
				//判断buf空间是否足够
				if (dwBufLen < sizeof(HY_DVR_AUTH_INFO))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_AUTH_INFO), dwBufLen);
					break;
				}

				HY_DVR_AUTH_INFO *pcfg = (HY_DVR_AUTH_INFO *) pBuffer;

				nRet = SetAuthInfoCfg(pcfg);
			}
			break;

#if defined (__AUTH_V2__)
		case AUTH_INFO_ALL_V2:
			{
				///////////////////////////////////////////////////////////////////////////////
				//判断buf空间是否足够
				if (dwBufLen < sizeof(HY_DVR_AUTH_INFO_V2))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_AUTH_INFO_V2), dwBufLen);
					break;
				}

				HY_DVR_AUTH_INFO_V2 *pcfg = (HY_DVR_AUTH_INFO_V2 *) pBuffer;

				nRet = SetAuthInfoCfgV2(pcfg);
			}
			break;
#endif
        case AUTH_INFO_TRANSPARENT_INFO:
            {
                ///////////////////////////////////////////////////////////////////////////////
                //判断buf空间是否足够
                if (dwBufLen < sizeof(HY_DVR_AUTH_INFO_TRANSPARENT_INFO))
                {
                    nRet = IDVR_CIU_BUF_SMALL;
                    MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_AUTH_INFO_TRANSPARENT_INFO), dwBufLen);
                    break;
                }

                HY_DVR_AUTH_INFO_TRANSPARENT_INFO *pcfg = (HY_DVR_AUTH_INFO_TRANSPARENT_INFO *) pBuffer;
                nRet = SetAuthInfoTransparentInfo(pcfg);
#if defined (__IS_SUPPORT_4G_CLOUD_DOMAIN__)
                nRet = SetAuthInfoTransparentV2(pcfg);
#endif
            }
            break;
		
        case AUTH_INFO_P2P_INFO:
            {
                //判断buf空间是否足够
				if (dwBufLen < sizeof(HY_DVR_AUTH_INFO_P2P_INFO))
				{
				    //printf("\n[FAN-KA][%s][%d]\n",
                    //    __FUNCTION__, __LINE__);
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_AUTH_INFO_P2P_INFO), dwBufLen);
					break;
				}

				HY_DVR_AUTH_INFO_P2P_INFO *pcfg = (HY_DVR_AUTH_INFO_P2P_INFO *) pBuffer;
				//将auth p2p信息设置到NET
				nRet = SetAuthP2pInfo(pcfg);
            }
            break;
			
		default:
			{
				MPU_ERR("Unsupported dwMinorType = %lu\n", dwMinorType);
				nRet = IDVR_CIU_UNSUPPORTED_MINORTYPE;
			}
			break;
	}
	#endif

	MPU_INFO("ID %d, Major %d, Minor %d, return %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType, nRet);
	return nRet;    
}

int CIU_GetDeviceErrorInfo(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen)
{
	int nRet = IDVR_OK;

	MPU_INFO("ID %d, Major %d, Minor %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType);

	switch (dwMinorType)
	{
		case PROTOCOL_INTERACT_ERR_INFO:
		{
			///////////////////////////////////////////////////////////////////////////////
			//判断buf空间是否足够
			if (*pdwBufLen < sizeof(HY_DVR_PROTOCOL_INTERACT_ERROR_INFO))
			{
				nRet = IDVR_CIU_BUF_SMALL;
				MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_PROTOCOL_INTERACT_ERROR_INFO), *pdwBufLen);
				break;
			}

			HY_DVR_PROTOCOL_INTERACT_ERROR_INFO *pcfg = (HY_DVR_PROTOCOL_INTERACT_ERROR_INFO *)pBuffer;

			//初始化输出缓冲区
			pcfg->bValid = 0;
			memset(pcfg->stErrInfo.szErrInfo, 0, sizeof(pcfg->stErrInfo.szErrInfo));
			*pdwBufLen = sizeof(HY_DVR_PROTOCOL_INTERACT_ERROR_INFO);
			
			pcfg->bValid = 1;
            snprintf(pcfg->stErrInfo.szErrInfo,sizeof(pcfg->stErrInfo.szErrInfo) - 1,"Error:%d",pcfg->stErrInfo.nErrCode);
			
			break;
		}

		default:
		{
			MPU_ERR("unsupported dwMinorType = %lu\n", dwMinorType);
			nRet = IDVR_CIU_UNSUPPORTED_MINORTYPE;
			break;
		}
	}
	
	MPU_INFO("ID %d, Major %d, Minor %d, return %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType, nRet);
	return nRet;    
}

int CIU_GetThumbnail(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen)
{
	int nRet = IDVR_OK;

	MPU_INFO("ID %d, Major %d, Minor %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType);

	switch (dwMinorType)
	{
		case PICTURE_BY_BUFFER_ALL:
		{
			//判断buf空间是否足够
			if (*pdwBufLen < sizeof(HY_DVR_PICTURE_BY_BUFFER))
			{
				nRet = IDVR_CIU_BUF_SMALL;
				MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_PICTURE_BY_BUFFER), *pdwBufLen);
				break;
			}

			HY_DVR_PICTURE_BY_BUFFER *pcfg = (HY_DVR_PICTURE_BY_BUFFER *)pBuffer;
			//初始化输出缓冲区
			pcfg->bValid = 0;
			*pdwBufLen = sizeof(HY_DVR_PICTURE_BY_BUFFER);

			//抓图
            printf("Start GetMjpegBuffer, nChannelID %d\n", pcfg->nChannelID);
            nRet = GetMjpegBuffer(0, pcfg->nChannelID, &(pcfg->pBuffer), &(pcfg->nBufferLen), MAX_PICTURE_BUF_SIZE, NULL);

			//抓图超过MAX_FOURTH_PICTURE_BUF_SIZE，则认为失败   
			if (pcfg->nBufferLen > MAX_PICTURE_BUF_SIZE || 0 == pcfg->nBufferLen)
			{
				pcfg->nBufferLen = 0;
				nRet = -1;
			}

			if (0 != nRet)
			{
				MPU_ERR("[%s] GetMjpegBuffer failed nCH[%d] nRet[%d]\n", __FUNCTION__, pcfg->nChannelID, nRet);
			}
			else
			{
				pcfg->bValid = 1;
				nRet = IDVR_OK;
			}

			break;
		}

		default:
		{
			MPU_ERR("unsupported dwMinorType = %lu\n", dwMinorType);
			nRet = IDVR_CIU_UNSUPPORTED_MINORTYPE;
			break;
		}
	}
	
	MPU_INFO("ID %d, Major %d, Minor %d, return %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType, nRet);
	return nRet;    
}

/* 设置生产定制信息 */
int CIU_SetConfigure_PRODUCE_CUSTOM_INFO(char *pCustomFile, char *pDstFile)
{
	int nRet = IDVR_OK;

	return nRet;    
}

//获取流传输方式
int CIU_GetConfigure_RealStreamBuffer(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen)
{
	int nRet = IDVR_OK;

	MPU_DBG("ID %d, Major %d, Minor %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType);

	switch (dwMinorType)
	{
		case STREAM_TRANSPORT_TYPE:
		{
			if (*pdwBufLen < sizeof(HY_DVR_STREAM_TRANSPORT_TYPE))
			{
				nRet = IDVR_CIU_BUF_SMALL;
				MPU_ERR("buffer is too small, need %d, real %lu\n", 
							sizeof(HY_DVR_STREAM_TRANSPORT_TYPE), *pdwBufLen);
				break;
			}
			HY_DVR_STREAM_TRANSPORT_TYPE *pcfg = (HY_DVR_STREAM_TRANSPORT_TYPE*)pBuffer;
			nRet = CIU_Config_Dev_RealStreamBufferType (pcfg);
			*pdwBufLen = sizeof(HY_DVR_STREAM_TRANSPORT_TYPE);

			break;
		}
		default:
		{
			nRet = IDVR_CIU_UNSUPPORTED_MINORTYPE;
			MPU_ERR ("unsupported dw minor type(%d)\n", dwMinorType);
			break;
		}
	}

	return nRet;
}

int CIU_Update_RealStream_Connect_INFO(int nCh, int StreamType, int action)
{
	int nRet = IDVR_OK;
#if (MAX_CH_NUM == 1)	
	printf("begin*************m_AccessMainStream=[%d]\n", m_AccessMainStream);
	printf("begin***********m_AccessSecondStream=[%d]\n", m_AccessSecondStream);
#elif (MAX_CH_NUM == 2)
	printf("begin*************m_AccessMainStream=[%d] S1[%d]\n", m_AccessMainStream, m_AccessMainStreamS1);
	printf("begin***********m_AccessSecondStream=[%d] S1[%d]\n", m_AccessSecondStream, m_AccessSecondStreamS1);
#elif (MAX_CH_NUM == 3)
	printf("begin*************m_AccessMainStream=[%d] S1[%d] S2[%d]\n", m_AccessMainStream, m_AccessMainStreamS1, m_AccessMainStreamS2);
	printf("begin***********m_AccessSecondStream=[%d] S1[%d] S2[%d]\n", m_AccessSecondStream, m_AccessSecondStreamS1, m_AccessSecondStreamS2);
#elif (MAX_CH_NUM == 4)
	printf("begin*************m_AccessMainStream=[%d] S1[%d] S2[%d] S3[%d]\n", m_AccessMainStream, m_AccessMainStreamS1, m_AccessMainStreamS2, m_AccessMainStreamS3);
	printf("begin***********m_AccessSecondStream=[%d] S1[%d] S2[%d] S3[%d]\n", m_AccessSecondStream, m_AccessSecondStreamS1, m_AccessSecondStreamS2, m_AccessSecondStreamS3);
#endif

    if (nCh == 0)
    {
    	if (StreamType == eMediaCodeMain) {
    		if (action == 0){
    			if (m_AccessMainStream < MAX_MAIN_ACCESS_NUM) {
    				m_AccessMainStream++;
    				nRet = 0;
    			} else {
    				nRet = 2;
    			}
     		} else {
    			if (m_AccessMainStream > 0) {
    				m_AccessMainStream--;
    			}
    		}
    	} else if (StreamType == eMediaCodeAssist) {
    		if (action == 0) {
    			if (m_AccessSecondStream < MAX_SECOND_ACCESS_NUM) {
    				m_AccessSecondStream++;
    				nRet = 0;
    			} else {
    				nRet = 2;
    			}
    		} else {
    			if (m_AccessSecondStream > 0) {
    				m_AccessSecondStream--;
    			}
    		}
    	}
    } else if (nCh == 1) {
    	if (StreamType == eMediaCodeMain) {
    		if (action == 0){
    			if (m_AccessMainStreamS1 < MAX_MAIN_ACCESS_NUM_S1) {
    				m_AccessMainStreamS1++;
    				nRet = 0;
    			} else {
    				nRet = 2;
    			}
     		} else {
    			if (m_AccessMainStreamS1 > 0) {
    				m_AccessMainStreamS1--;
    			}
    		}
    	} else if (StreamType == eMediaCodeAssist) {
    		if (action == 0) {
    			if (m_AccessSecondStreamS1 < MAX_SECOND_ACCESS_NUM_S1) {
    				m_AccessSecondStreamS1++;
    				nRet = 0;
    			} else {
    				nRet = 2;
    			}
    		} else {
    			if (m_AccessSecondStreamS1 > 0) {
    				m_AccessSecondStreamS1--;
    			}
    		}
    	}
    }
    else if (nCh == 2) 
    {
    	if (StreamType == eMediaCodeMain) {
    		if (action == 0){
    			if (m_AccessMainStreamS2 < MAX_MAIN_ACCESS_NUM_S2) {
    				m_AccessMainStreamS2++;
    				nRet = 0;
    			} else {
    				nRet = 2;
    			}
     		} else {
    			if (m_AccessMainStreamS2 > 0) {
    				m_AccessMainStreamS2--;
    			}
    		}
    	} else if (StreamType == eMediaCodeAssist) {
    		if (action == 0) {
    			if (m_AccessSecondStreamS2 < MAX_SECOND_ACCESS_NUM_S2) {
    				m_AccessSecondStreamS2++;
    				nRet = 0;
    			} else {
    				nRet = 2;
    			}
    		} else {
    			if (m_AccessSecondStreamS2 > 0) {
    				m_AccessSecondStreamS2--;
    			}
    		}
    	}
    }
    else if (nCh == 3) 
    {
    	if (StreamType == eMediaCodeMain) {
    		if (action == 0){
    			if (m_AccessMainStreamS3 < MAX_MAIN_ACCESS_NUM_S3) {
    				m_AccessMainStreamS3++;
    				nRet = 0;
    			} else {
    				nRet = 2;
    			}
     		} else {
    			if (m_AccessMainStreamS3 > 0) {
    				m_AccessMainStreamS3--;
    			}
    		}
    	} else if (StreamType == eMediaCodeAssist) {
    		if (action == 0) {
    			if (m_AccessSecondStreamS3 < MAX_SECOND_ACCESS_NUM_S3) {
    				m_AccessSecondStreamS3++;
    				nRet = 0;
    			} else {
    				nRet = 2;
    			}
    		} else {
    			if (m_AccessSecondStreamS3 > 0) {
    				m_AccessSecondStreamS3--;
    			}
    		}
    	}
    }
    else
    {
        printf("StreamCapture channel_no err, channel %d\n",nCh);
    }
#if (MAX_CH_NUM == 1)
	printf("end***************m_AccessMainStream=[%d]\n", m_AccessMainStream);
	printf("end*************m_AccessSecondStream=[%d]\n", m_AccessSecondStream);
#elif (MAX_CH_NUM == 2)
	printf("end***************m_AccessMainStream=[%d] S1[%d]\n", m_AccessMainStream, m_AccessMainStreamS1);
	printf("end*************m_AccessSecondStream=[%d] S1[%d]\n", m_AccessSecondStream, m_AccessSecondStreamS1);
#elif (MAX_CH_NUM == 3)
	printf("end***************m_AccessMainStream=[%d] S1[%d] S2[%d]\n", m_AccessMainStream, m_AccessMainStreamS1, m_AccessMainStreamS2);
	printf("end*************m_AccessSecondStream=[%d] S1[%d] S2[%d]\n", m_AccessSecondStream, m_AccessSecondStreamS1, m_AccessSecondStreamS2);
#elif (MAX_CH_NUM == 4)
	printf("end***************m_AccessMainStream=[%d] S1[%d] S2[%d] S3[%d]\n", m_AccessMainStream, m_AccessMainStreamS1, m_AccessMainStreamS2, m_AccessMainStreamS3);
	printf("end*************m_AccessSecondStream=[%d] S1[%d] S2[%d] S3[%d]\n", m_AccessSecondStream, m_AccessSecondStreamS1, m_AccessSecondStreamS2, m_AccessSecondStreamS3);
#endif

	return nRet;
}

#if defined (__IS_SUPPORT_P6SMESSAGE__)
//获取错误码信息
int CIU_GetConfigure_ERROR_CODE(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen)
{
	int nRet = IDVR_OK;

	MPU_DBG("ID %d, Major %d, Minor %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType);

	switch (dwMinorType)
	{
		case PROTOCOL_ZW_CLOUD_STORAGE:
		{
			nRet = HieServer_GetConfig(PROTOCOL_ZW_CLOUD_STORAGE, HY_DVR_GET_CLOUD_STORAGE, CLOUD_STORAGE_BASE_STATUS, (char *)pBuffer, *pdwBufLen);
			break;
		}
		default:
		{
			nRet = IDVR_CIU_UNSUPPORTED_MINORTYPE;
			MPU_ERR("unsupported dwMinorType = %lu\n", dwMinorType);
			break;
		}
	}

	MPU_DBG("ID %d, Major %d, Minor %d, return %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType, nRet);
	return nRet;
}

int GetErrorCodeServerCfg(HY_DVR_MP_POST_PARAM *pcfg)
{
	stuShmAuthInfo stShmAuthInfo = {0};

	CConfig *cCfg = CConfig::GetInstance();

	int nRet = 0;

	unsigned int nAuthState = 0;
    unsigned char szAuthCode[256] = {0};

	static int s_accesssNum = 0;               //同一参数第一次从接口获取值，第二次直接用静态变量的值
	static char s_boardId[64] = {0};  		   //boardid
	static int s_customer = 0;				   // 客户id
    static char s_versionId[64] = {0};         // 客户版本id

	ZWSys_GetAuthInfo(szAuthCode, &nAuthState);
    if (0 == nAuthState)
    {
        return AUTH_NOT_AUTHORIZE;
    }
	else
	{
		//获取设备启动时间
		MPU_SYS_GetDeviceOpenTime(&pcfg->deviceOpenTime);

		if(0 == s_accesssNum)
		{
			// szClientId 优先使用授权码内部的customID
            Authorize_GetAuthDevInfo(&stShmAuthInfo);
			if (AUTH_SUCCESS == stShmAuthInfo.dwAuthVerifyErrno)
			{
				snprintf(pcfg->boardId, sizeof(pcfg->boardId), "%lX", stShmAuthInfo.dwBoardID);//获取boardId
				memcpy(s_boardId, pcfg->boardId, sizeof(pcfg->boardId));
					
				pcfg->customer = stShmAuthInfo.dCustomID;//客户id
				s_customer = stShmAuthInfo.dCustomID;
			}

			//获取客户版本id
			cCfg = CConfig::NewInstance();
			nRet = cCfg->LoadFileRO(CUSTOM_INFO);
			if(0 == nRet)
			{
				cCfg->GetValue(CLOUD_UPGRADE_CUSTOM_SECTION_NAME, 
						CLOUD_UPGRADE_CUSTOM_CUSTOM_NO_KEY_NAME, 
						CLOUD_UPGRADE_CUSTOM_CUSTOM_NO_VALUE_DEF, 
						pcfg->versionId, sizeof(pcfg->versionId));
				memcpy(s_versionId, pcfg->versionId, sizeof(pcfg->versionId));
			}
			CConfig::ReleaseInstance(cCfg);

			s_accesssNum++;
		}
		else
		{
			memcpy(pcfg->boardId, s_boardId, sizeof(s_boardId));
			pcfg->customer = s_customer;
			memcpy(pcfg->versionId, s_versionId, sizeof(s_versionId));
		}
	}

	return nRet;

}

//获取服务器必选参数
int CIU_GetConfigure_ERROR_CODE_SERVER_INFO(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen)
{
	int nRet = IDVR_OK;

	MPU_DBG("ID %d, Major %d, Minor %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType);

	switch (dwMinorType)
	{
		case ERROR_CODE_SERVER_INFO:
		{
			HY_DVR_MP_POST_PARAM *pcfg = (HY_DVR_MP_POST_PARAM*) pBuffer;

			//判断buf空间是否足够
			if (*pdwBufLen < sizeof(HY_DVR_MP_POST_PARAM))
			{
				nRet = IDVR_CIU_BUF_SMALL;
				MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_MP_POST_PARAM), *pdwBufLen);
				break;
			}

			memset(pcfg, 0, sizeof(HY_DVR_MP_POST_PARAM));
			*pdwBufLen = sizeof(HY_DVR_MP_POST_PARAM);

			nRet = GetErrorCodeServerCfg(pcfg);

			break;
		}
		default:
		{
			nRet = IDVR_CIU_UNSUPPORTED_MINORTYPE;
			MPU_ERR("unsupported dwMinorType = %lu\n", dwMinorType);
			break;
		}
	}

	MPU_DBG("ID %d, Major %d, Minor %d, return %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType, nRet);
	return nRet;
}
#endif

int CIU_GetConfigure_VoiceBroadcastLanguage(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen)
{
	int nRet = IDVR_OK;

	MPU_DBG("ID %d, Major %d, Minor %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType);

	switch (dwMinorType)
	{
		case VOICE_BROADCAST_LANGUAGE:
		{
			HY_DVR_VOICE_BROADCAST_LANGUAGE *pcfg = (HY_DVR_VOICE_BROADCAST_LANGUAGE*) pBuffer;

			//判断buf空间是否足够
			if (*pdwBufLen < sizeof(HY_DVR_VOICE_BROADCAST_LANGUAGE))
			{
				nRet = IDVR_CIU_BUF_SMALL;
				MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_VOICE_BROADCAST_LANGUAGE), *pdwBufLen);
				break;
			}

			//memset(pcfg, 0, sizeof(HY_DVR_VOICE_BROADCAST_LANGUAGE));
			*pdwBufLen = sizeof(HY_DVR_VOICE_BROADCAST_LANGUAGE);

			nRet = PUB_GetVoiceLanguage(pcfg);

			break;
		}
		default:
		{
			nRet = IDVR_CIU_UNSUPPORTED_MINORTYPE;
			MPU_ERR("unsupported dwMinorType = %lu\n", dwMinorType);
			break;
		}
	}

	MPU_DBG("ID %d, Major %d, Minor %d, return %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType, nRet);

	return nRet;
}

int CIU_SetConfigure_VoiceBroadcastLanguage(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, const BYTE * pBuffer, DWORD  dwBufLen)
{
	int nRet = IDVR_OK;

	MPU_DBG("[%s %d]dwMajorType=%d\n", __FUNCTION__, __LINE__, dwMajorType);
	switch (dwMinorType)
	{
		case VOICE_BROADCAST_LANGUAGE:
        {
			HY_DVR_VOICE_BROADCAST_LANGUAGE * pcfg = (HY_DVR_VOICE_BROADCAST_LANGUAGE *) pBuffer;
            
            //判断buf空间是否足够
            if (dwBufLen < sizeof(HY_DVR_VOICE_BROADCAST_LANGUAGE))
            {
                nRet = IDVR_CIU_BUF_SMALL;
                MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_VOICE_BROADCAST_LANGUAGE), dwBufLen);
                break;
            }
            nRet = PUB_SetVoiceLanguage(pcfg);
		}
        break;
        
        default:
        {
            MPU_ERR("unsupported dwMinorType = %lu\n", dwMinorType);
            nRet = IDVR_CIU_UNSUPPORTED_MINORTYPE;
        }
        break;
    }
           
    return nRet;
}

int CIU_GetConfigure_DEVICE_SLEEP_INFO(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen)
{
    int nRet = IDVR_OK;
	MPU_DBG("ID %d, Major %d, Minor %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType);

	switch (dwMinorType)
	{
		case SLEEPCFG_ALL:
		{
			if (*pdwBufLen < sizeof(HY_DVR_DEVICE_SLEEP_INFO_CFG))
			{
				nRet = IDVR_CIU_BUF_SMALL;
				MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_DEVICE_SLEEP_INFO_CFG), *pdwBufLen);
				break;
			}
			LPHY_DVR_DEVICE_SLEEP_INFO_CFG pcfg = (LPHY_DVR_DEVICE_SLEEP_INFO_CFG)pBuffer;
            memset(pcfg, 0, sizeof(HY_DVR_DEVICE_SLEEP_INFO_CFG));
            nRet = GetDevSleepInfo(pcfg);
			*pdwBufLen = sizeof(HY_DVR_DEVICE_SLEEP_INFO_CFG);

			break;
		}
		default:
		{
			nRet = IDVR_CIU_UNSUPPORTED_MINORTYPE;
			MPU_ERR ("unsupported dw minor type(%lu)\n", dwMinorType);
			break;
		}
	}

    return nRet;
}

int CIU_SetConfigure_DEVICE_SLEEP_INFO(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, const BYTE * pBuffer, DWORD dwBufLen)
{
	int nRet = IDVR_OK;

	MPU_DBG("ID %d, Major %d, Minor %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType);

	switch (dwMinorType)
	{
        case SLEEPCFG_ALL:
        {
            if (dwBufLen < sizeof(HY_DVR_DEVICE_SLEEP_INFO_CFG))
            {
                nRet = IDVR_CIU_BUF_SMALL;
                MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_DEVICE_SLEEP_INFO_CFG), dwBufLen);
            }

            LPHY_DVR_DEVICE_SLEEP_INFO_CFG pcfg = (LPHY_DVR_DEVICE_SLEEP_INFO_CFG)pBuffer;
            nRet = SetDevSleepInfo(pcfg);
            break;
        }
		default:
		{
			nRet = IDVR_CIU_UNSUPPORTED_MINORTYPE;
			MPU_ERR ("unsupported dw minor type(%lu)\n", dwMinorType);
			break;
		}
	}

	return nRet;
}

int CIU_GetConfigure_DEVICE_SLEEP_CONTROL(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen)
{
    int nRet = IDVR_OK;
	MPU_DBG("ID %d, Major %d, Minor %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType);

	switch (dwMinorType)
	{
		case SLEEP_CONTROL:
		{
			if (*pdwBufLen < sizeof(HY_DVR_DEVICE_SLEEP_CONTROL))
			{
				nRet = IDVR_CIU_BUF_SMALL;
				MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_DEVICE_SLEEP_CONTROL), *pdwBufLen);
				break;
			}
			LPHY_DVR_DEVICE_SLEEP_CONTROL pcfg = (LPHY_DVR_DEVICE_SLEEP_CONTROL)pBuffer;
            memset(pcfg, 0, sizeof(HY_DVR_DEVICE_SLEEP_CONTROL));
            nRet = GetDevSleepControl(pcfg);
			*pdwBufLen = sizeof(HY_DVR_DEVICE_SLEEP_CONTROL);

			break;
		}
		default:
		{
			nRet = IDVR_CIU_UNSUPPORTED_MINORTYPE;
			MPU_ERR ("unsupported dw minor type(%lu)\n", dwMinorType);
			break;
		}
	}

    return nRet;
}

int CIU_SetConfigure_DEVICE_SLEEP_CONTROL(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, const BYTE * pBuffer, DWORD dwBufLen)
{
	int nRet = IDVR_OK;

	MPU_DBG("ID %d, Major %d, Minor %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType);

	switch (dwMinorType)
	{
        case SLEEP_CONTROL:
        {
            if (dwBufLen < sizeof(HY_DVR_DEVICE_SLEEP_CONTROL))
            {
                nRet = IDVR_CIU_BUF_SMALL;
                MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_DEVICE_SLEEP_CONTROL), dwBufLen);
            }

            LPHY_DVR_DEVICE_SLEEP_CONTROL pcfg = (LPHY_DVR_DEVICE_SLEEP_CONTROL)pBuffer;
            nRet = SetDevSleepControl(pcfg);
            break;
        }
		default:
		{
			nRet = IDVR_CIU_UNSUPPORTED_MINORTYPE;
			MPU_ERR ("unsupported dw minor type(%lu)\n", dwMinorType);
			break;
		}
	}

	return nRet;
}

int CIU_SetConfigure_P2pCommand(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, const BYTE * pBuffer, DWORD dwBufLen)
{
    int nRet = IDVR_OK;

    MPU_DBG("ID %d, Major %d, Minor %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType);

    switch (dwMinorType)
    {
        case P2P_BUFFER_FULL:
        {
            if (dwBufLen != sizeof(H264DATAPACKETHEADER))
            {
                nRet = IDVR_CIU_BUF_SMALL;
                MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(H264DATAPACKETHEADER), dwBufLen);
                break;
            }
#if defined (__MODULE_4G_SUPPORT__)
            ZWSys_P2PNotifySendDataFail((char *)pBuffer, dwBufLen);
#endif
            break;
        }

        default:
        {
			nRet = IDVR_CIU_UNSUPPORTED_MINORTYPE;
            MPU_DBG ("unsupported dw minor type(%d)\n", dwMinorType);
            break;
        }
    }

    return nRet;
}



//设置云存储使能信息
int SetCloudStorageInfo(HY_DVR_CLOUD_STORAGE *pcfg)
{
	int nRet = 0;
	
	//本结构体是否有效，=0表示无效
	if (0 == pcfg->nValid)
	{
		MPU_DBG("0 == pcfg->bValid\n");
		return IDVR_CIU_STRUCT_UNVALID;
	}
	
	nRet = MPU_PUB_SetCloudStorageInfo(pcfg);
	return nRet;
}

//获取云存储信息
int GetCloudStorageInfo(HY_DVR_CLOUD_STORAGE *pcfg)
{
	int nRet = 0;

	pcfg->nValid = 1;
	nRet = MPU_PUB_GetCloudStorageInfo(pcfg);

	return nRet;
}

//获取云存储使能信息
int SetCloudStorageBaseCfg(HY_DVR_CLOUD_STROAGE_BASE_CONFIG *pcfg)
{
	int nRet = 0;
	
	//本结构体是否有效，=0表示无效
	if (0 == pcfg->nValid)
	{
		MPU_DBG("0 == pcfg->bValid\n");
		return IDVR_CIU_STRUCT_UNVALID;
	}
	
	nRet = MPU_PUB_SetCloudStorageBaseCfg(pcfg);
	return nRet;
}

//获取云存储使能信息
int GetCloudStorageBaseCfg(HY_DVR_CLOUD_STROAGE_BASE_CONFIG *pcfg)
{
	int nRet = 0;

	pcfg->nValid = 1;
	nRet = MPU_PUB_GetCloudStorageBaseCfg(pcfg);

	return nRet;
}

//获取云存储开通信息
int SetCloudStorageSubscribeStatus(HY_DVR_CLOUD_STROAGE_OPEN_STATUS *pcfg)
{
	int nRet = 0;

	//本结构体是否有效，=0表示无效
	if (0 == pcfg->nValid)
	{
		MPU_DBG("0 == pcfg->bValid\n");
		return IDVR_CIU_STRUCT_UNVALID;
	}
	
	nRet = MPU_PUB_SetCloudStorageSubscribeStatus(pcfg);
	return nRet;
}

//设置云存储录像配置
int SetCloudStorageRecordCfg(HY_DVR_CLOUD_STROAGE_RECORD_PLAN *pcfg)
{
	int nRet = 0;
	
	//本结构体是否有效，=0表示无效
	if (0 == pcfg->nValid)
	{
		MPU_DBG("0 == pcfg->bValid\n");
		return IDVR_CIU_STRUCT_UNVALID;
	}
	
	nRet = MPU_PUB_SetCloudStorageRecordCfg(pcfg);
	return nRet;
}

//获取云存储录像配置
int GetCloudStorageRecordCfg(HY_DVR_CLOUD_STROAGE_RECORD_PLAN *pcfg)
{
	int nRet = 0;

	pcfg->nValid = 1;
	nRet = MPU_PUB_GetCloudStorageRecordCfg(pcfg);

	return nRet;
}

//获取云存储录像配置
int GetCloudStorageRecordCfg_V2(HY_DVR_CLOUD_STROAGE_RECORD_PLAN *pcfg, int nChannelMaxNum)
{
	int nRet = 0;

	pcfg->nValid = 1;
	nRet = MPU_PUB_GetCloudStorageRecordCfg_V2(pcfg, nChannelMaxNum);

	return nRet;
}

//设置云存储录像配置
int SetCloudStorageRecordCfg_V2(HY_DVR_CLOUD_STROAGE_RECORD_PLAN *pcfg, int nChannelMaxNum)
{
	int nRet = 0;
	
	//本结构体是否有效，=0表示无效
	if (0 == pcfg->nValid)
	{
		MPU_DBG("0 == pcfg->bValid\n");
		return IDVR_CIU_STRUCT_UNVALID;
	}
	
	nRet = MPU_PUB_SetCloudStorageRecordCfg_V2(pcfg, nChannelMaxNum);
	return nRet;
}

#ifdef __IS_SUPPORT_CLOUDWORLD__
//云存储配置
int GetCloudStorageLowPowerCfg(HY_DVR_CLOUD_STORAGE_LOW_POWER_CONFIG *ptCfg)
{
    int nRet = 0;

    if(NULL == ptCfg)
    {
        MPU_ERR("input param is error\n");
        return -1;
    }

    ptCfg->nValid = 1;
    ptCfg->deviceMode = 0;  
    ptCfg->isSingleChannelLogic = 1;

    return nRet;
}


//设置云存储模式 国内/国际
int SetCloudStorageMode(HY_DVR_CLOUD_STORAGE_MODE *pcfg)
{
    int nRet = 0;

    if (0 == pcfg->nValid)
    {
        MPU_DBG("0 == pcfg->bValid\n");
        return IDVR_CIU_STRUCT_UNVALID;
    }
    nRet = MPU_PUB_SetCloudModeCfg(pcfg);
    return nRet;
}
//获取云存储模式
int GetCloudStorageMode(HY_DVR_CLOUD_STORAGE_MODE *pcfg)
{
   int nRet = 0;

   pcfg->nValid = 1;
   nRet = MPU_PUB_GetCloudModeCfg(pcfg);

   return nRet;
}
int CIU_GetConfigure_CloudStorageMode(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen)
{
	int nRet = IDVR_OK;
	MPU_DBG("ID %d, Major %d, Minor %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType);
	switch (dwMinorType)
	{
		
		case CLOUD_STORAGE_MODE:
		{
			HY_DVR_CLOUD_STORAGE_MODE *pcfg = (HY_DVR_CLOUD_STORAGE_MODE *)pBuffer;
			//判断buf空间是否足够
			if (*pdwBufLen < sizeof(HY_DVR_CLOUD_STORAGE_MODE))
			{
				nRet = IDVR_CIU_BUF_SMALL;
				MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_CLOUD_STORAGE_MODE), *pdwBufLen);
				break;
			}

			memset(pcfg, 0, sizeof(HY_DVR_CLOUD_STORAGE_MODE));
			*pdwBufLen = sizeof(HY_DVR_CLOUD_STORAGE_MODE);
			nRet = GetCloudStorageMode(pcfg);
			
			
		}
		break;
		
		default:
		{
			nRet = IDVR_CIU_UNSUPPORTED_MINORTYPE;
			MPU_ERR("unsupported dwMinorType = %lu\n", dwMinorType);
		}
		break;
	}
	MPU_DBG("ID %d, Major %d, Minor %d, return %d\n", (int)dwUserID, (int)dwMajorType, (int)dwMinorType, nRet);
	return nRet;
}

int CIU_SetConfigure_CloudStorageMode(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, const BYTE * pBuffer, DWORD dwBufLen)
{
	int nRet = IDVR_OK;
	MPU_DBG("ID %d, Major %d, Minor %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType);
	switch (dwMinorType)
	{
		case CLOUD_STORAGE_MODE:
		{
			if (dwBufLen < sizeof(HY_DVR_CLOUD_STORAGE_MODE))
			{
				nRet = IDVR_CIU_BUF_SMALL;
				MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_CLOUD_STORAGE_MODE), dwBufLen);
				break;
			}

			HY_DVR_CLOUD_STORAGE_MODE *pcfg = (HY_DVR_CLOUD_STORAGE_MODE *)pBuffer;
			nRet = SetCloudStorageMode(pcfg);
		}
		break;

		default:
		{
			nRet = IDVR_CIU_UNSUPPORTED_MINORTYPE;
			MPU_ERR("unsupported dwMinorType = %lu\n", dwMinorType);
		}
		break;
	}
	
	MPU_DBG("ID %d, Major %d, Minor %d, return %d\n", (int)dwUserID, (int)dwMajorType, (int)dwMinorType, nRet);
	return nRet;
}
#endif


int CIU_GetConfigure_CloudStorage(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen)
{
	int nRet = IDVR_OK;
	MPU_DBG("ID %d, Major %d, Minor %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType);

	switch (dwMinorType)
	{
		case CLOUD_STORAGE_INFO:
		{
			HY_DVR_CLOUD_STORAGE *pcfg = (HY_DVR_CLOUD_STORAGE *)pBuffer;
			//判断buf空间是否足够
			if (*pdwBufLen < sizeof(HY_DVR_CLOUD_STORAGE))
			{
				nRet = IDVR_CIU_BUF_SMALL;
				MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_CLOUD_STORAGE), *pdwBufLen);
				break;
			}

			memset(pcfg, 0, sizeof(HY_DVR_CLOUD_STORAGE));
			*pdwBufLen = sizeof(HY_DVR_CLOUD_STORAGE);

			nRet = GetCloudStorageInfo(pcfg);
		}
		break;

		case CLOUD_STORAGE_BASE_CONFIG:
		{
			HY_DVR_CLOUD_STROAGE_BASE_CONFIG *pcfg = (HY_DVR_CLOUD_STROAGE_BASE_CONFIG *)pBuffer;
			//判断buf空间是否足够
			if (*pdwBufLen < sizeof(HY_DVR_CLOUD_STROAGE_BASE_CONFIG))
			{
				nRet = IDVR_CIU_BUF_SMALL;
				MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_CLOUD_STROAGE_BASE_CONFIG), *pdwBufLen);
				break;
			}

			memset(pcfg, 0, sizeof(HY_DVR_CLOUD_STROAGE_BASE_CONFIG));
			*pdwBufLen = sizeof(HY_DVR_CLOUD_STROAGE_BASE_CONFIG);
			nRet = GetCloudStorageBaseCfg(pcfg);
		}
		break;

		case CLOUD_STORAGE_OPEN_STATUS:
		{
			//从协议去获取
			memset(pBuffer, 0, *pdwBufLen);
			nRet = HieServer_GetConfig(PROTOCOL_ZW_CLOUD_STORAGE, dwMajorType, dwMinorType, 
				(char *)pBuffer, *pdwBufLen);
		}
		break;

		case CLOUD_STORAGE_RECORD_PLAN:
		{
			HY_DVR_CLOUD_STROAGE_RECORD_PLAN *pcfg = (HY_DVR_CLOUD_STROAGE_RECORD_PLAN *)pBuffer;
			//判断buf空间是否足够
			if (*pdwBufLen < sizeof(HY_DVR_CLOUD_STROAGE_RECORD_PLAN))
			{
				nRet = IDVR_CIU_BUF_SMALL;
				MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_CLOUD_STROAGE_RECORD_PLAN), *pdwBufLen);
				break;
			}

			memset(pcfg, 0, sizeof(HY_DVR_CLOUD_STROAGE_RECORD_PLAN));
			*pdwBufLen = sizeof(HY_DVR_CLOUD_STROAGE_RECORD_PLAN);

			nRet = GetCloudStorageRecordCfg(pcfg);
		}
		break;

		case CLOUD_STORAGE_RECORD_PLAN_V2:
		{
			HY_DVR_CLOUD_STROAGE_RECORD_PLAN *pcfg = (HY_DVR_CLOUD_STROAGE_RECORD_PLAN *)pBuffer;
			//判断buf空间是否足够
			if (*pdwBufLen < sizeof(HY_DVR_CLOUD_STROAGE_RECORD_PLAN))
			{
				nRet = IDVR_CIU_BUF_SMALL;
				MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_CLOUD_STROAGE_RECORD_PLAN), *pdwBufLen);
				break;
			}
			memset(pBuffer, 0, *pdwBufLen);

			int nChannelMaxNum = (*pdwBufLen) /sizeof(HY_DVR_CLOUD_STROAGE_RECORD_PLAN);
			nRet = GetCloudStorageRecordCfg_V2(pcfg, nChannelMaxNum);
		}
		break;

		case CLOUD_STORAGE_BASE_STATUS:
		case CLOUD_STORAGE_ORDER_INFO:
		case CLOUD_STORAGE_PACKAGE_INFO:
        case CLOUD_STORAGE_BASE_STATUS_V2: // 多通道云存储状态(HY_DVR_CLOUD_STORAGE_BASE_STATUS_V2)
		{
			memset(pBuffer, 0, *pdwBufLen);
			nRet = HieServer_GetConfig(PROTOCOL_ZW_CLOUD_STORAGE, dwMajorType, dwMinorType, (char *)pBuffer, *pdwBufLen);
		}

		break;

		case CLOUD_STORAGE_ORDER_PACKAGE_INFO:
		{
			HY_DVR_CLOUD_STORAGE_BUSINESS_INFO *pcfg = (HY_DVR_CLOUD_STORAGE_BUSINESS_INFO *)pBuffer;
			//判断buf空间是否足够
			if (*pdwBufLen < sizeof(HY_DVR_CLOUD_STORAGE_BUSINESS_INFO))
			{
				nRet = IDVR_CIU_BUF_SMALL;
				MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_CLOUD_STORAGE_BUSINESS_INFO), *pdwBufLen);
				break;
			}

			memset(pcfg, 0, sizeof(HY_DVR_CLOUD_STORAGE_BUSINESS_INFO));
			*pdwBufLen = sizeof(HY_DVR_CLOUD_STORAGE_BUSINESS_INFO);
			nRet = HieServer_GetConfig(PROTOCOL_ZW_CLOUD_STORAGE, dwMajorType, CLOUD_STORAGE_ORDER_INFO, 
				(char *)(&(pcfg->struOrderInfo)), sizeof(pcfg->struOrderInfo));
			if (0 == nRet)
			{
				pcfg->nValid = 1;
			}

			nRet = HieServer_GetConfig(PROTOCOL_ZW_CLOUD_STORAGE, dwMajorType, CLOUD_STORAGE_PACKAGE_INFO, 
				(char *)(&(pcfg->struPackageInfo)), sizeof(pcfg->struPackageInfo));
			if (0 == nRet)
			{
				pcfg->nValid = 1;
			}
			
		}
		break;

		default:
		{
			MPU_ERR("unsupported dwMinorType = %lu\n", dwMinorType);
			nRet = IDVR_CIU_UNSUPPORTED_MINORTYPE;
		}
		break;
	}
	
	MPU_DBG("ID %d, Major %d, Minor %d, return %d\n", (int)dwUserID, (int)dwMajorType, (int)dwMinorType, nRet);
	return nRet;
}

int CIU_SetConfigure_CloudStorage(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, const BYTE * pBuffer, DWORD dwBufLen)
{
	int nRet = IDVR_OK;
	MPU_DBG("ID %d, Major %d, Minor %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType);

	switch (dwMinorType)
	{
		case CLOUD_STORAGE_INFO:
		{
			if (dwBufLen < sizeof(HY_DVR_CLOUD_STORAGE))
			{
				nRet = IDVR_CIU_BUF_SMALL;
				MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_CLOUD_STORAGE), dwBufLen);
				break;
			}

			HY_DVR_CLOUD_STORAGE *pcfg = (HY_DVR_CLOUD_STORAGE *)pBuffer;
			nRet = SetCloudStorageInfo(pcfg);
		}
		break;

		case CLOUD_STORAGE_BASE_CONFIG:
		{
			if (dwBufLen < sizeof(HY_DVR_CLOUD_STROAGE_BASE_CONFIG))
			{
				nRet = IDVR_CIU_BUF_SMALL;
				MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_CLOUD_STROAGE_BASE_CONFIG), dwBufLen);
				break;
			}

			HY_DVR_CLOUD_STROAGE_BASE_CONFIG *pcfg = (HY_DVR_CLOUD_STROAGE_BASE_CONFIG *)pBuffer;
			nRet = SetCloudStorageBaseCfg(pcfg);
		}
		break;

		case CLOUD_STORAGE_OPEN_STATUS:
		{
			if (dwBufLen < sizeof(HY_DVR_CLOUD_STROAGE_OPEN_STATUS))
			{
				nRet = IDVR_CIU_BUF_SMALL;
				MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_CLOUD_STROAGE_OPEN_STATUS), dwBufLen);
				break;
			}

			HY_DVR_CLOUD_STROAGE_OPEN_STATUS *pcfg = (HY_DVR_CLOUD_STROAGE_OPEN_STATUS *)pBuffer;
			nRet = SetCloudStorageSubscribeStatus(pcfg);
		}
		break;

		case CLOUD_STORAGE_RECORD_PLAN:
		{
			if (dwBufLen < sizeof(HY_DVR_CLOUD_STROAGE_RECORD_PLAN))
			{
				nRet = IDVR_CIU_BUF_SMALL;
				MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_CLOUD_STROAGE_RECORD_PLAN), dwBufLen);
				break;
			}

			HY_DVR_CLOUD_STROAGE_RECORD_PLAN *pcfg = (HY_DVR_CLOUD_STROAGE_RECORD_PLAN *)pBuffer;
			nRet = SetCloudStorageRecordCfg(pcfg);

			/* 当前APP云存配置不区分通道 */
			nRet = SetCloudStorageRecordCfg_V2(pcfg, SYSTEM_SUPPORT_CHANNEL);
		}
		break;

		case CLOUD_STORAGE_RECORD_PLAN_V2:
		{
			if (dwBufLen < sizeof(HY_DVR_CLOUD_STROAGE_RECORD_PLAN))
			{
				nRet = IDVR_CIU_BUF_SMALL;
				MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_CLOUD_STROAGE_RECORD_PLAN), dwBufLen);
				break;
			}

            int nChannelMaxNum = dwBufLen /sizeof(HY_DVR_CLOUD_STROAGE_RECORD_PLAN);
			HY_DVR_CLOUD_STROAGE_RECORD_PLAN *pcfg = (HY_DVR_CLOUD_STROAGE_RECORD_PLAN *)pBuffer;
			nRet = SetCloudStorageRecordCfg_V2(pcfg, nChannelMaxNum);
		}
		break;

        case CLOUD_STORAGE_BASE_STATUS_V2:
		{
			if (dwBufLen < sizeof(HY_DVR_CLOUD_STORAGE_BASE_STATUS_V2))
			{
				nRet = IDVR_CIU_BUF_SMALL;
				MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_CLOUD_STORAGE_BASE_STATUS_V2), dwBufLen);
				break;
			}

            HY_DVR_CLOUD_STORAGE_BASE_STATUS_V2 *pcfg = (HY_DVR_CLOUD_STORAGE_BASE_STATUS_V2 *)pBuffer;
            nRet = HieServer_SetConfig(PROTOCOL_ZW_CLOUD_STORAGE, dwMajorType, dwMinorType, (char *)pcfg, dwBufLen);
		}
		break;

		default:
		{
			MPU_ERR("unsupported dwMinorType = %lu\n", dwMinorType);
			nRet = IDVR_CIU_UNSUPPORTED_MINORTYPE;
		}
		break;
	}
	
	MPU_DBG("ID %d, Major %d, Minor %d, return %d\n", (int)dwUserID, (int)dwMajorType, (int)dwMinorType, nRet);
	return nRet;
}


#if defined (__IS_SUPPORT_REMINDER_SCHEDULE__)
static int RemindSchedule_AddReminder(const HY_DVR_TIME_REMIND_CFG *pcfg)
{
	if (0 == pcfg->iValid)
	{
		MPU_DBG("0 == pcfg->bValid\n");
		return IDVR_CIU_STRUCT_UNVALID;
	}
    MPU_INFO("[RemindSchedule_AddReminder] Start!! \n");

	int i, j;    
	tagReminderItem item;

	for (i = 0; i < MAX_TIME_REMIND_NUM; i++)
	{
		if (pcfg->stTimeRemindPlanList[i].iValid == 0)
        {
            continue;
        }
		memset(&item, 0, sizeof(tagReminderItem));
		
		item.id = pcfg->stTimeRemindPlanList[i].iID;
		item.enable = pcfg->stTimeRemindPlanList[i].iEnable;

        for (j = 0; j <= 6; j++) {
            if (pcfg->stTimeRemindPlanList[i].acPeriod[j] == '1')
            {
                if (j == 0) {
                    item.weekMask |= (1 << 6);  // 周日(下标0) -> 位6
                } else {
                    item.weekMask |= (1 << (j - 1));  // 周一(下标1) -> 位0
                }
            }
        }

	    if (sscanf(pcfg->stTimeRemindPlanList[i].acRemindTime, "%*d-%*d-%*dT%d:%d:%*d", &item.hour, &item.minute) == 2) {
	        MPU_INFO("[AddReminder]hour: %d, minute: %d\n", item.hour, item.minute);
	    }
		else {
	        MPU_ERR("[AddReminder] sscanf err  acRemindTime:%s \n",pcfg->stTimeRemindPlanList[i].acRemindTime);
	    }

		memcpy(item.name, pcfg->stTimeRemindPlanList[i].acRemindName, sizeof(item.name));
		item.audioCfg.remindSpeechType = pcfg->stTimeRemindPlanList[i].iVoiceType;
		item.audioCfg.broadCastTimes = pcfg->stTimeRemindPlanList[i].iBroadcastCount;
		item.audioCfg.broadCastVolume = pcfg->stTimeRemindPlanList[i].iBroadcastVolume;

		MPU_INFO("[AddReminder] i:%d, iValid:%d, iEnable:%d, iVoiceType:%d, iID:%d, iBroadcastCount:%d, iBroadcastVolume:%d, weekMask:%d acRemindName:%s, acRemindTime:%s\n", i,
    		pcfg->stTimeRemindPlanList[i].iValid,
    		pcfg->stTimeRemindPlanList[i].iEnable,
    		pcfg->stTimeRemindPlanList[i].iID,
    		pcfg->stTimeRemindPlanList[i].iVoiceType,
    		pcfg->stTimeRemindPlanList[i].iBroadcastCount,
    		pcfg->stTimeRemindPlanList[i].iBroadcastVolume,
    		item.weekMask,
    		pcfg->stTimeRemindPlanList[i].acRemindName,
    		pcfg->stTimeRemindPlanList[i].acRemindTime);

		MPU_SYS_RemindSchedule_AddReminder(&item);
	}

	return 0;
}

static int RemindSchedule_DelReminder(const HY_DVR_TIME_REMIND_CFG *pcfg)
{
	//本结构体是否有效，=0表示无效
	if (0 == pcfg->iValid)
	{
		MPU_DBG("0 == pcfg->bValid\n");
		return IDVR_CIU_STRUCT_UNVALID;
	}

    MPU_INFO("[RemindSchedule_DelReminder] Start!! \n");

	for (int i = 0; i < MAX_TIME_REMIND_NUM; i++)
	{
		if (pcfg->stTimeRemindPlanList[i].iValid == 0)
        {
            continue;
        }

		MPU_SYS_RemindSchedule_DeleteReminder(pcfg->stTimeRemindPlanList[i].iID);
        MPU_INFO("[DelReminder]i:%d iID:%d iValid == 1\n", i, pcfg->stTimeRemindPlanList[i].iID);
	}

	return 0;
}

static int RemindSchedule_ModifyReminder(const HY_DVR_TIME_REMIND_CFG *pcfg)
{
	//本结构体是否有效，=0表示无效
	if (0 == pcfg->iValid)
	{
		MPU_DBG("0 == pcfg->bValid\n");
		return IDVR_CIU_STRUCT_UNVALID;
	}
    MPU_INFO("[RemindSchedule_ModifyReminder] Start!! \n");

	int i, j = 0;
	tagReminderItem item;

	for (i = 0; i < MAX_TIME_REMIND_NUM; i++)
	{
		if (pcfg->stTimeRemindPlanList[i].iValid == 0)
        {
            continue;
        }      
		memset(&item, 0, sizeof(tagReminderItem));
		
		item.id = pcfg->stTimeRemindPlanList[i].iID;
		item.enable = pcfg->stTimeRemindPlanList[i].iEnable;

        for (j = 0; j <= 6; j++) {
            if (pcfg->stTimeRemindPlanList[i].acPeriod[j] == '1')
            {
                if (j == 0) {
                    item.weekMask |= (1 << 6);  // 周日(下标0) -> 位6
                } else {
                    item.weekMask |= (1 << (j - 1));  // 周一(下标1) -> 位0
                }
            }
        }

		if (sscanf(pcfg->stTimeRemindPlanList[i].acRemindTime, "%*d-%*d-%*dT%d:%d:%*d", &item.hour, &item.minute) == 2) {
			MPU_INFO("[ModifyReminder]hour: %d, minute: %d\n", item.hour, item.minute);
		}
		else {
			MPU_ERR("[ModifyReminder] sscanf err  acRemindTime:%s \n",pcfg->stTimeRemindPlanList[i].acRemindTime);
            continue;
		}
		memcpy(item.name, pcfg->stTimeRemindPlanList[i].acRemindName, sizeof(item.name));

		item.audioCfg.remindSpeechType = pcfg->stTimeRemindPlanList[i].iVoiceType;
		item.audioCfg.broadCastTimes = pcfg->stTimeRemindPlanList[i].iBroadcastCount;
		item.audioCfg.broadCastVolume = pcfg->stTimeRemindPlanList[i].iBroadcastVolume;
		
		MPU_INFO("[ModifyReminder]i:%d, iValid:%d ,iEnable:%d, iID:%d iVoiceType:%d, iBroadcastCount:%d, iBroadcastVolume:%d, weekMask:%d acRemindName:%s, acRemindTime:%s\n", i,
    		pcfg->stTimeRemindPlanList[i].iValid,
    		pcfg->stTimeRemindPlanList[i].iEnable,
    		pcfg->stTimeRemindPlanList[i].iID,
    		pcfg->stTimeRemindPlanList[i].iVoiceType,
    		pcfg->stTimeRemindPlanList[i].iBroadcastCount,
    		pcfg->stTimeRemindPlanList[i].iBroadcastVolume,
    		item.weekMask,
    		pcfg->stTimeRemindPlanList[i].acRemindName,
    		pcfg->stTimeRemindPlanList[i].acRemindTime);

		MPU_SYS_RemindSchedule_ModifyReminder(&item);
	}

	return 0;
}

static int RemindSchedule_AddCustomSpeech(const HY_DVR_TIME_REMIND_AUDIO_INFO_LIST *pcfg)
{
	//本结构体是否有效，=0表示无效
	if (0 == pcfg->iValid)
	{
		MPU_ERR("0 == pcfg->bValid\n");
		return IDVR_CIU_STRUCT_UNVALID;
	}
    MPU_INFO("[RemindSchedule_AddCustomSpeech] Start!! \n");

	int i;    
	tagCusSpeechInfo item;

	for (i = 0; i < MAX_TIME_REMIND_AUDIO_NUM; i++)
	{
        if (pcfg->stTimeRemindAudioInfoList[i].iValid == 0)
        {
            continue;
        }

        memset(&item, 0, sizeof(tagCusSpeechInfo));
        item.iID = pcfg->stTimeRemindAudioInfoList[i].iID;
        memcpy(item.iName, pcfg->stTimeRemindAudioInfoList[i].acName, sizeof(item.iName));

        MPU_SYS_RemindSchedule_AddCustomSpeech(&item);

        MPU_INFO("[AddCustomSpeech] iValid:%d, iID:%d, iName:%s\n",
                pcfg->stTimeRemindAudioInfoList[i].iValid,
                pcfg->stTimeRemindAudioInfoList[i].iID,
                pcfg->stTimeRemindAudioInfoList[i].acName);
	}

	return 0;
}

static int RemindSchedule_DelCustomSpeech(const HY_DVR_TIME_REMIND_AUDIO_INFO_LIST *pcfg)
{
	//本结构体是否有效，=0表示无效
	if (0 == pcfg->iValid)
	{
		MPU_ERR("0 == pcfg->bValid\n");
		return IDVR_CIU_STRUCT_UNVALID;
	}
    MPU_INFO("[RemindSchedule_DelCustomSpeech] Start!! \n");

	for (int i = 0; i < MAX_TIME_REMIND_AUDIO_NUM; i++)
	{
		if (pcfg->stTimeRemindAudioInfoList[i].iValid == 0)
        {
            continue;
        }

		MPU_SYS_RemindSchedule_DeleteCustomSpeech(pcfg->stTimeRemindAudioInfoList[i].iID);
        MPU_INFO("[DelCustomSpeech] i:%d, iID:%d, iValid == 1\n", i, pcfg->stTimeRemindAudioInfoList[i].iID);
	}

	return 0;
}

static int RemindSchedule_TestPlaySpeech(const HY_DVR_TIME_REMIND_AUDIO_PLAY_INFO *pcfg)
{
	//本结构体是否有效，=0表示无效
	if (0 == pcfg->iValid)
	{
		MPU_ERR("0 == pcfg->bValid\n");
		return IDVR_CIU_STRUCT_UNVALID;
	}
    MPU_INFO("[RemindSchedule_TestPlaySpeech] Start!! [%d]\n", pcfg->iVoiceType);

	MPU_SYS_RemindSchedule_TestPlaySpeech(pcfg->iVoiceType);
	return 0;
}

int CIU_SetRemindSchedule(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, const BYTE * pBuffer, DWORD dwBufLen)
{
	int nRet = IDVR_OK;
	MPU_DBG("ID %d, Major %d, Minor %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType);

	switch (dwMinorType)
	{
		case ADD_TIME_REMIND_CFG:
			{
				//判断buf空间是否足够
				if (dwBufLen < sizeof(HY_DVR_TIME_REMIND_CFG))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_TIME_REMIND_CFG), dwBufLen);
					break;
				}
				HY_DVR_TIME_REMIND_CFG *pcfg = (HY_DVR_TIME_REMIND_CFG *) pBuffer;
				nRet = RemindSchedule_AddReminder(pcfg);
			}
			break; 
		
		case DELETE_TIME_REMIND_CFG:
			{
				//判断buf空间是否足够
				if (dwBufLen < sizeof(HY_DVR_TIME_REMIND_CFG))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_TIME_REMIND_CFG), dwBufLen);
					break;
				}
				HY_DVR_TIME_REMIND_CFG *pcfg = (HY_DVR_TIME_REMIND_CFG *) pBuffer;
				nRet = RemindSchedule_DelReminder(pcfg);
			}
			break; 

		case MODIFY_TIME_REMIND_CFG:
			{
				//判断buf空间是否足够
				if (dwBufLen < sizeof(HY_DVR_TIME_REMIND_CFG))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_TIME_REMIND_CFG), dwBufLen);
					break;
				}
				HY_DVR_TIME_REMIND_CFG *pcfg = (HY_DVR_TIME_REMIND_CFG *) pBuffer;
				nRet = RemindSchedule_ModifyReminder(pcfg);
			}
			break; 

		case ADD_TIME_REMIND_AUDIO_INFO:
			{
				//判断buf空间是否足够
				if (dwBufLen < sizeof(HY_DVR_TIME_REMIND_AUDIO_INFO_LIST))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_TIME_REMIND_AUDIO_INFO_LIST), dwBufLen);
					break;
				}
				HY_DVR_TIME_REMIND_AUDIO_INFO_LIST *pcfg = (HY_DVR_TIME_REMIND_AUDIO_INFO_LIST *) pBuffer;
				nRet = RemindSchedule_AddCustomSpeech(pcfg);
			}
			break;

        case DELETE_TIME_REMIND_AUDIO_INFO:
            {
                //判断buf空间是否足够
                if (dwBufLen < sizeof(HY_DVR_TIME_REMIND_AUDIO_INFO_LIST))
                {
                    nRet = IDVR_CIU_BUF_SMALL;
                    MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_TIME_REMIND_AUDIO_INFO_LIST), dwBufLen);
                    break;
                }
                HY_DVR_TIME_REMIND_AUDIO_INFO_LIST *pcfg = (HY_DVR_TIME_REMIND_AUDIO_INFO_LIST *) pBuffer;
                nRet = RemindSchedule_DelCustomSpeech(pcfg);
            }
            break;

        case PLAY_TIME_REMIND_AUDIO:
            {
                //判断buf空间是否足够
                if (dwBufLen < sizeof(HY_DVR_TIME_REMIND_AUDIO_PLAY_INFO))
                {
                    nRet = IDVR_CIU_BUF_SMALL;
                    MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_TIME_REMIND_AUDIO_PLAY_INFO), dwBufLen);
                    break;
                }
                HY_DVR_TIME_REMIND_AUDIO_PLAY_INFO *pcfg = (HY_DVR_TIME_REMIND_AUDIO_PLAY_INFO *) pBuffer;
                nRet = RemindSchedule_TestPlaySpeech(pcfg);
            }
            break;

		default:
			{
				MPU_ERR("unsupported dwMinorType = %lu\n", dwMinorType);
				nRet = IDVR_CIU_UNSUPPORTED_MINORTYPE;
			}
			break;

	}
	MPU_DBG("ID %d, Major %d, Minor %d, return %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType, nRet);
	return nRet;
}

static int RemindSchedule_GetReminder(LPHY_DVR_TIME_REMIND_CFG pcfg)
{
	if (NULL == pcfg)
    {
        MPU_ERR("HY_DVR_TIME_REMIND_CFG  pcfg == NULL\n");
        return -1;
    }

	int i = 0;
	tagReminderItem item;
	time_t now = time(NULL);
    struct tm tm_now;
    localtime_r(&now, &tm_now);
	
	int curYear = (tm_now.tm_year + 1900);
	int curMon = (tm_now.tm_mon + 1);
	int curday = tm_now.tm_mday;
	int ReminderMaxCount = MPU_SYS_RemindSchedule_GetMaxReminderCount();

    MPU_INFO("[GetReminder] Start!! [%d]\n", ReminderMaxCount);

    /* 正序循环获取 */
	for (i = 0; i < ReminderMaxCount; i++)
	{
		memset(&item, 0, sizeof(tagReminderItem));
        if (MPU_SYS_RemindSchedule_GetReminder(i+1, &item) != 0) {
            break;
        }

		pcfg->iValid = 1;
		pcfg->stTimeRemindPlanList[i].iValid = 1;
		pcfg->stTimeRemindPlanList[i].iEnable = item.enable;
		pcfg->stTimeRemindPlanList[i].iID = item.id;
		pcfg->stTimeRemindPlanList[i].iVoiceType = item.audioCfg.remindSpeechType;
		pcfg->stTimeRemindPlanList[i].iBroadcastCount = item.audioCfg.broadCastTimes;
		pcfg->stTimeRemindPlanList[i].iBroadcastVolume = item.audioCfg.broadCastVolume;

		memset(pcfg->stTimeRemindPlanList[i].acPeriod, 0, sizeof(pcfg->stTimeRemindPlanList[i].acPeriod));
        for (int j = 0; j <= 6; j++) {
            if (j == 6) { // 位6(周日) -> 下标0
                pcfg->stTimeRemindPlanList[i].acPeriod[0] = (item.weekMask & (1 << j)) ? '1' : '0';
            } else {
                pcfg->stTimeRemindPlanList[i].acPeriod[j + 1] = (item.weekMask & (1 << j)) ? '1' : '0';
            }
        }

		unsigned int len = snprintf(pcfg->stTimeRemindPlanList[i].acRemindTime, sizeof(pcfg->stTimeRemindPlanList[i].acRemindTime), 
             "%04d-%02d-%02dT%02d:%02d:%02d", curYear, curMon, curday, item.hour, item.minute, 0);	

		if (len >= sizeof(pcfg->stTimeRemindPlanList[i].acRemindTime)) 
		{
		    // 缓冲区太小，处理错误
		    MPU_ERR("RemindTime buffer too small, need %d bytes\n", len + 1);
		}

		memcpy(pcfg->stTimeRemindPlanList[i].acRemindName, item.name, sizeof(pcfg->stTimeRemindPlanList[i].acRemindName));
		memcpy(pcfg->stTimeRemindPlanList[i].acVoiceTypeName, item.voicename, sizeof(pcfg->stTimeRemindPlanList[i].acVoiceTypeName));

		MPU_INFO("[GetReminder]i:%d\n",i);
		MPU_INFO("[GetReminder]iValid:%d ,iEnable:%d, iID:%d iVoiceType:%d, iBroadcastCount:%d, iBroadcastVolume:%d, weekMask:%d acRemindName:%s, acRemindTime:%s\n",
		pcfg->stTimeRemindPlanList[i].iValid,
		pcfg->stTimeRemindPlanList[i].iEnable,
		pcfg->stTimeRemindPlanList[i].iID,
		pcfg->stTimeRemindPlanList[i].iVoiceType,
		pcfg->stTimeRemindPlanList[i].iBroadcastCount,
		pcfg->stTimeRemindPlanList[i].iBroadcastVolume,
		item.weekMask,
		pcfg->stTimeRemindPlanList[i].acRemindName,
		pcfg->stTimeRemindPlanList[i].acRemindTime
		);
	}

	return 0;
}

static int RemindSchedule_GetAuidoInfoList(LPHY_DVR_TIME_REMIND_AUDIO_INFO_LIST pcfg)
{
	if (NULL == pcfg)
    {
        MPU_ERR("HY_DVR_TIME_REMIND_CFG  pcfg == NULL\n");
        return -1;
    }
    MPU_INFO("[RemindSchedule_GetAuidoInfoList] Start!!\n");

	int i = 0;
    int index = 0;
    tagCusSpeechInfo item;
	int MaxSpeechCount = MPU_SYS_RemindSchedule_GetMaxCustomSpeechCount();

    /* 正序循环获取 */
	for (i = 0; i < MaxSpeechCount; i++)
    {
        memset(&item, 0, sizeof(tagCusSpeechInfo));
        if (MPU_SYS_RemindSchedule_GetCustomSpeech(i+1, &item) != 0) {
            break;
        }

        if (item.iID == 0) {
            continue;
        }

        if (index >= MAX_TIME_REMIND_AUDIO_NUM) {
            break;
        }

        pcfg->stTimeRemindAudioInfoList[index].iValid = 1;
        pcfg->stTimeRemindAudioInfoList[index].iID = item.iID;
		memcpy(pcfg->stTimeRemindAudioInfoList[index].acName, item.iName, sizeof(pcfg->stTimeRemindAudioInfoList[index].acName));

        MPU_INFO("[GetAuidoInfoList] iValid:%d, iID:%d, iName:%s\n",
                pcfg->stTimeRemindAudioInfoList[index].iValid,
                pcfg->stTimeRemindAudioInfoList[index].iID,
                pcfg->stTimeRemindAudioInfoList[index].acName);

        index++;
    }
    pcfg->iValid = 1;
    pcfg->iCurrentCustomVoiceValidCount = index;

    MPU_INFO("[RemindSchedule_GetAuidoInfoList] End!! ValidCount[%d]\n", pcfg->iCurrentCustomVoiceValidCount);
    return 0;
}

int CIU_GetRemindSchedule(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen)
{
	int nRet = IDVR_OK;
	MPU_DBG("ID %d, Major %d, Minor %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType);
	switch (dwMinorType)
	{
		case QUERY_TIME_REMIND_CFG:
		{
			if (*pdwBufLen < sizeof(HY_DVR_TIME_REMIND_CFG))
			{
				nRet = IDVR_CIU_BUF_SMALL;
				MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_TIME_REMIND_CFG), *pdwBufLen);
				break;
			}

			LPHY_DVR_TIME_REMIND_CFG pcfg = (LPHY_DVR_TIME_REMIND_CFG)pBuffer;
			memset(pcfg, 0, sizeof(HY_DVR_TIME_REMIND_CFG));

			RemindSchedule_GetReminder(pcfg);
			*pdwBufLen = sizeof(HY_DVR_TIME_REMIND_CFG);
		}
		break;

		case QUERY_TIME_REMIND_AUDIO_INFO:
		{
			if (*pdwBufLen < sizeof(HY_DVR_TIME_REMIND_AUDIO_INFO_LIST))
			{
				nRet = IDVR_CIU_BUF_SMALL;
				MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_TIME_REMIND_AUDIO_INFO_LIST), *pdwBufLen);
				break;
			}

			LPHY_DVR_TIME_REMIND_AUDIO_INFO_LIST pcfg = (LPHY_DVR_TIME_REMIND_AUDIO_INFO_LIST)pBuffer;
			memset(pcfg, 0, sizeof(HY_DVR_TIME_REMIND_AUDIO_INFO_LIST));

			RemindSchedule_GetAuidoInfoList(pcfg);
			*pdwBufLen = sizeof(HY_DVR_TIME_REMIND_AUDIO_INFO_LIST);
		}
		break;

		default:
		{
			nRet = IDVR_CIU_UNSUPPORTED_MINORTYPE;
			MPU_ERR("unsupported dwMinorType = %lu\n", dwMinorType);
		}
		break;
	}
	
	MPU_DBG("ID %d, Major %d, Minor %d, return %d\n", (int)dwUserID, (int)dwMajorType, (int)dwMinorType, nRet);
	return nRet;
}

#endif

#if defined (__IS_SUPPORT_LCD_MANAGER__)
//获取屏幕管理配置
static int GetScreenManagentCfg(LPHY_DVR_SCREEN_MANAGEMENT_CFG pcfg)
{
	pcfg->iValid = 1;
	pcfg->iEnable = 1; /* 该参数当前无用 */
    MPU_LCD_GetSleepTimeout((unsigned int*)&pcfg->iScreenCloseTime);
    MPU_LCD_GetBrightness((unsigned int*)&pcfg->iScreenLight);
    MPU_LCD_GetBackgroundImageNumber((unsigned int*)&pcfg->iCurrentScreenThemeType);

    MPU_DBG("GetScreenManagentCfg [%d] [%d] [%d]\n", 
            pcfg->iScreenCloseTime,
            pcfg->iScreenLight,
            pcfg->iCurrentScreenThemeType);

	return 0;
}

//设置屏幕管理配置
static int SetScreenManagentCfg(LPHY_DVR_SCREEN_MANAGEMENT_CFG pcfg)
{
    MPU_DBG("SetScreenManagentCfg [%d] [%d] [%d]\n", 
            pcfg->iScreenCloseTime,
            pcfg->iScreenLight,
            pcfg->iCurrentScreenThemeType);

	//本结构体是否有效，=0表示无效
	if (0 == pcfg->iValid)
	{
		MPU_ERR("0 == pcfg->iValid\n");
		return IDVR_CIU_STRUCT_UNVALID;
	}

    MPU_LCD_SetSleepTimeout((unsigned int)pcfg->iScreenCloseTime);
    MPU_LCD_SetBrightness((unsigned int)pcfg->iScreenLight);
    MPU_LCD_SetBackgroundImageNumber((unsigned int)pcfg->iCurrentScreenThemeType);

	return 0;
}

int CIU_GetConfigure_ScreenManagent(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen)
{
	int nRet = IDVR_OK;
	MPU_DBG("ID %d, Major %d, Minor %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType);
	switch (dwMinorType)
	{
		case SCREEN_MANAGEMENT_CFG:
		{
			if (*pdwBufLen < sizeof(HY_DVR_SCREEN_MANAGEMENT_CFG))
			{
				nRet = IDVR_CIU_BUF_SMALL;
				MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_SCREEN_MANAGEMENT_CFG), *pdwBufLen);
				break;
			}

			LPHY_DVR_SCREEN_MANAGEMENT_CFG pcfg = (LPHY_DVR_SCREEN_MANAGEMENT_CFG)pBuffer;
			GetScreenManagentCfg(pcfg);
			*pdwBufLen = sizeof(HY_DVR_SCREEN_MANAGEMENT_CFG);
		}
		break;

		default:
		{
			nRet = IDVR_CIU_UNSUPPORTED_MINORTYPE;
			MPU_ERR("unsupported dwMinorType = %lu\n", dwMinorType);
		}
		break;
	}

	MPU_DBG("ID %d, Major %d, Minor %d, return %d\n", (int)dwUserID, (int)dwMajorType, (int)dwMinorType, nRet);
	return nRet;
}

int CIU_SetConfigure_ScreenManagent(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, const BYTE * pBuffer, DWORD dwBufLen)
{
	int nRet = IDVR_OK;
	MPU_DBG("ID %d, Major %d, Minor %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType);

	switch (dwMinorType)
	{
		case SCREEN_MANAGEMENT_CFG:
		{
			//判断buf空间是否足够
			if (dwBufLen < sizeof(HY_DVR_SCREEN_MANAGEMENT_CFG))
			{
				nRet = IDVR_CIU_BUF_SMALL;
				MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_SCREEN_MANAGEMENT_CFG), dwBufLen);
				break;
			}
			HY_DVR_SCREEN_MANAGEMENT_CFG *pcfg = (HY_DVR_SCREEN_MANAGEMENT_CFG *)pBuffer;
			nRet = SetScreenManagentCfg(pcfg);
		}
		break;

		default:
		{
			MPU_ERR("unsupported dwMinorType = %lu\n", dwMinorType);
			nRet = IDVR_CIU_UNSUPPORTED_MINORTYPE;
		}
		break;
	}
	MPU_DBG("ID %d, Major %d, Minor %d, return %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType, nRet);
	return nRet;
}
#endif
