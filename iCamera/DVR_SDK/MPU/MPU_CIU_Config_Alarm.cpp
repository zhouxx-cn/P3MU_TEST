
/*!
 *\file	 MPU_CIU_Config_Alarm.cpp
 *\brief	MPU模块，远程配置，报警参数
 *\author	林云
 *\date	2009/8/17

功能：
-#	报警输入
-#	报警输出
-#	异常处理

说明：
-#	本文件使用新的网络库实现
 */

#include "MPU_CIU_global.h"
#include "PUB/lattice2bmp24.h"
extern int GetDevCustom_Alarm(LPHY_DVR_WORKMODE pcfg);

//extern int SetDevCustom_Alarm(LPHY_DVR_WORKMODE pcfg);
//设置报警输入事件联动处理配置信息

//获取报警灯光配置信息，需要在外部对pcfg清零
static int GetAlarmLightCfg(int nCh, LPHY_DVR_LIGHT_WARNING pcfg)
{
    int nRet = IDVR_OK;
    return nRet;
}

//设置报警灯光通道配置信息
static int SetAlarmLightCfg(int nCh, LPHY_DVR_LIGHT_WARNING pcfg)
{
    int nRet = IDVR_OK;
    return nRet;
}

static int GetCapturePicCfgV2(int nCh, LPHY_DVR_ALARM_CAPTURE_PIC_V2 pcfg)
{
	//! 本结构体是否有效，=0表示无效
	pcfg->bValid = 1;
    int nResolution = 0;
    
	//联动快照张数
	//联动快照间隔时间

	pcfg->nStreamTypeValid = 0;
	MPU_AVL_Pic_GetCaptureCfg(nCh, &pcfg->nCapPicCount, &pcfg->nCapPicIntervalTime,
		&nResolution, (int *)&pcfg->dwResolutionWidth, (int *)&pcfg->dwResolutionHeight, (int *)&pcfg->dwPictureQuality);

	pcfg->nStreamTypeValid = 1;
	// 支持子码流
	pcfg->nSupportStreamType = 2;
	MPU_AVL_Pic_GetCaptureStreamType(nCh, &pcfg->nStreamType);

	return IDVR_OK;
}

static int GetOverlayInfo(int nCh, LPHY_DVR_ALARM_OVERLAY_INFO pcfg)
{
	//! 本结构体是否有效，=0表示无效
	pcfg->nValid = 1;

	MPU_AVL_GetHumanOverlayInfo(nCh, &pcfg->cMainStreamPeopleOverlay, &pcfg->cSubStreamPeopleOverlay);
	MPU_AVL_GetDetectAreaOverlayInfo(nCh, &pcfg->nMainStreamOverlay, &pcfg->nSubStreamOverlay);
	return IDVR_OK;
}

//设置联动快照配置信息
static int SetCapturePicCfgV2(int nCh,LPHY_DVR_ALARM_CAPTURE_PIC_V2 pcfg)
{
	int nSDKRet;
    int nResolution = 0;
	///////////////////////////////////////////////////////////////////////////////
	//本结构体是否有效，=0表示无效
	if (0 == pcfg->bValid)
	{
		MPU_DBG("0 == pcfg->bValid\n");
		return IDVR_CIU_STRUCT_UNVALID;
	}

	if (1 == pcfg->nStreamTypeValid)
	{
		MPU_DBG("--SetCapturePicCfgV2--dwPictureQuality=%d,nStreamType=%d--\n", pcfg->dwPictureQuality, pcfg->nStreamType);
		nSDKRet = MPU_AVL_Pic_SetCaptureCfgV2(nCh, (int)pcfg->dwPictureQuality, pcfg->nStreamType);
		MPU_CHK(0 == nSDKRet);
	}
	else
	{
		MPU_DBG("--SetCapturePicCfg--nCapPicCount=%d--nCapPicIntervalTime=%d--dwResolutionWidth=%d--dwResolutionHeight=%d--\n",
		pcfg->nCapPicCount,pcfg->nCapPicIntervalTime,pcfg->dwResolutionWidth,pcfg->dwResolutionHeight,pcfg->dwPictureQuality);
		
		//联动快照张数
		//联动快照间隔时间
		nSDKRet = MPU_AVL_Pic_SetCaptureCfg(nCh,pcfg->nCapPicCount, pcfg->nCapPicIntervalTime, nResolution, pcfg->dwResolutionWidth, pcfg->dwResolutionHeight, (int)pcfg->dwPictureQuality);
		MPU_CHK(0 == nSDKRet);
	}

	return (0 == nSDKRet) ? IDVR_OK : IDVR_CIU_CALLSDK_FAIL;
}

static int SetOverlayInfo(int nCh, LPHY_DVR_ALARM_OVERLAY_INFO pcfg)
{
	///////////////////////////////////////////////////////////////////////////////
	//本结构体是否有效，=0表示无效
	if (0 == pcfg->nValid)
	{
		MPU_DBG("0 == pcfg->nValid\n");
		return IDVR_CIU_STRUCT_UNVALID;
	}

	MPU_AVL_SetHumanOverlayInfo(nCh, pcfg->cMainStreamPeopleOverlay, pcfg->cSubStreamPeopleOverlay);
	MPU_AVL_SetDetectAreaOverlayInfo(nCh, pcfg->nMainStreamOverlay, pcfg->nSubStreamOverlay);
	return IDVR_OK;
}

#if 0
//获取各通道OSD，需外部将缓冲区清零
static int GetAlarmIn_Alias(int nCh, LPHY_DVR_ALIAS_CFG pcfg)
{
	int nRet;

	char szSection[30];

	char szDefaultName[30];

	snprintf(szSection, sizeof(szSection), SECTION_DIP_ALIAS, nCh);

	snprintf(szDefaultName, sizeof(szDefaultName), DEFNAME_DIP_ALIAS, nCh + 1);

	nRet = PUB_GetAlias(szSection, szDefaultName, pcfg);

	return nRet;
}

//设置各通道OSD
static int SetAlarmIn_Alias(int nCh, LPHY_DVR_ALIAS_CFG pcfg)
{
	int nRet = IDVR_OK, nSDKRet;

	char szSection[30];

	char szFileName[64];

	snprintf(szSection, sizeof(szSection), SECTION_DIP_ALIAS, nCh);

	//本结构体是否有效，=0表示无效
	if (0 == pcfg->bValid)
	{
		MPU_DBG("0 == pcfg->bValid, nCh = %d\n", nCh);
		return IDVR_CIU_STRUCT_UNVALID;
	}

	if ((((pcfg->nWidth + 7) / 8) * pcfg->nHeight) > DVR_MAX_LATTICE_BUFSIZE)
	{
		MPU_ERR("%s: nWidth %d, nHeight %d, is overflow\n", szSection, pcfg->nWidth, pcfg->nHeight);
		return IDVR_CIU_PARAM_OVERFLOW;
	}

	if (0 == strlen((const char *)pcfg->strName))
	{
		MPU_ERR("%s: is NULL\n", szSection);
		return IDVR_CIU_PARAM_OVERFLOW;
	}

	MPU_INFO("%s: [%s] nWidth %d, nHeight %d\n", szSection, pcfg->strName, pcfg->nWidth, pcfg->nHeight);

	//保存点阵到位图
	snprintf(szFileName, sizeof(szFileName), IDVR_DIP_ALIAS, nCh + 1);

	nSDKRet = lattice_saveas_bmp24(pcfg->nWidth, pcfg->nHeight, pcfg->byLattice, szFileName, 0);
	if (0 == nSDKRet)
	{
		nSDKRet = PUB_SetAlias(szSection, pcfg);
		MPU_CHK(0 == nSDKRet);

		//通知GUI报警输入通道别名变更
		CIU_NotifyRemoteControl(0, eRemoteSetAlarmInAlias, nCh);
	}
	else
	{
		MPU_ERR("%s: change lattice to bmp fail, return %d\n", szSection, nSDKRet);
		nRet = IDVR_CIU_CALLSDK_FAIL;
	}

	return nRet;
}

//获取各通道OSD，需外部将缓冲区清零
static int GetAlarmOut_Alias(int nCh, LPHY_DVR_ALIAS_CFG pcfg)
{
	int nRet;

	char szSection[30];

	char szDefaultName[30];

	snprintf(szSection, sizeof(szSection), SECTION_DOP_ALIAS, nCh);

	snprintf(szDefaultName, sizeof(szDefaultName), DEFNAME_DOP_ALIAS, nCh + 1);

	nRet = PUB_GetAlias(szSection, szDefaultName, pcfg);

	return nRet;
}

//设置各通道OSD
static int SetAlarmOut_Alias(int nCh, LPHY_DVR_ALIAS_CFG pcfg)
{
	int nRet = IDVR_OK, nSDKRet;

	char szSection[30];

	char szFileName[64];

	snprintf(szSection, sizeof(szSection), SECTION_DOP_ALIAS, nCh);

	//本结构体是否有效，=0表示无效
	if (0 == pcfg->bValid)
	{
		MPU_DBG("0 == pcfg->bValid, nCh = %d\n", nCh);
		return IDVR_CIU_STRUCT_UNVALID;
	}

	if ((((pcfg->nWidth + 7) / 8) * pcfg->nHeight) > DVR_MAX_LATTICE_BUFSIZE)
	{
		MPU_ERR("%s: nWidth %d, nHeight %d, is overflow\n", szSection, pcfg->nWidth, pcfg->nHeight);
		return IDVR_CIU_PARAM_OVERFLOW;
	}

	if (0 == strlen((const char *)pcfg->strName))
	{
		MPU_ERR("%s: is NULL\n", szSection);
		return IDVR_CIU_PARAM_OVERFLOW;
	}

	MPU_INFO("%s: [%s] nWidth %d, nHeight %d\n", szSection, pcfg->strName, pcfg->nWidth, pcfg->nHeight);

	//保存点阵到位图
	snprintf(szFileName, sizeof(szFileName), IDVR_DOP_ALIAS, nCh + 1);

	nSDKRet = lattice_saveas_bmp24(pcfg->nWidth, pcfg->nHeight, pcfg->byLattice, szFileName, 0);
	if (0 == nSDKRet)
	{
		nSDKRet = PUB_SetAlias(szSection, pcfg);
		MPU_CHK(0 == nSDKRet);

		//通知GUI报警输出通道别名变更
		CIU_NotifyRemoteControl(0, eRemoteSetAlarmOutAlias, nCh);
	}
	else
	{
		MPU_ERR("%s: change lattice to bmp fail, return %d\n", szSection, nSDKRet);
		nRet = IDVR_CIU_CALLSDK_FAIL;
	}

	return nRet;
}
#endif

//获取报警、异常处理配置拓展信息，需要在外部对pcfg清零
static int GetAlarmCfgEx(LPHY_DVR_ALARM_CFG_EX pcfg)
{
	int nRet = IDVR_OK;

	pcfg->bValid = 1;
	pcfg->struSnapshotPicCapability.bValid = 1;
	pcfg->struSnapshotPicCapability.nResolutionCap =  DVR_CAP_RESOLUTION_CIF;

	return nRet;
}

//获取报警、异常处理配置拓展信息，需要在外部对pcfg清零
static int GetAlarmCfgExV2(LPHY_DVR_ALARM_CFG_EX_V2 pcfg)
{
	int nRet = IDVR_OK;
	int nBoardID;
    ZWSys_GetBoardInfo(&nBoardID);

	pcfg->bValid = 1;
	pcfg->struSnapshotPicCapabilityV2.bValid = 1;
	pcfg->struSnapshotPicCapabilityV2.struResolutionInfo.nResolutionWidth = 352 ;
	pcfg->struSnapshotPicCapabilityV2.struResolutionInfo.nResolutionHeight = 288 ;
	pcfg->struSnapshotPicCapabilityV2.pNext = NULL;


	return nRet;
}

//获取区域布防时间，需要在外部对pcfg清零
static int GetSchedule_AreaDefense(int nCh, LPHY_DVR_POLYGON_SCHEDULE pcfg)
{
	int nRet = IDVR_OK, nDay, i, num;

	MotionSchedule sched;

	//! 本结构体是否有效，=0表示无效
	pcfg->nValid = 1;

	///////////////////////////////////////////////////////////////////////////////
	//移动侦测报警布防计划时间表
	/* 0表示不布防，1表示布防 */
	MPU_AVL_AreaDefense_GetSchedule(nCh, &sched);

	num = DVR_MAX_TIMESEGMENT / HY_DVR_MOTIONSEG_DAY;

	//时间表需要扩展
	for (nDay = 0; nDay < DVR_MAX_DAYS; ++nDay)
	{
		for (i = 0; i < DVR_MAX_TIMESEGMENT; ++i)
		{
			pcfg->szSchedule[nDay][i] = sched.dwTimeSeg[nDay][i / num];
		}
	}

	//! 状态,0为禁用(全天布防),1为启用,2为不支持
	pcfg->nEnable = sched.bytState;
	return nRet;
}

//设置区域布防时间
static int SetSchedule_AreaDefense(int nCh, LPHY_DVR_POLYGON_SCHEDULE pcfg)
{
	int nRet = IDVR_OK, nDay, i, num;

	MotionSchedule sched;

	///////////////////////////////////////////////////////////////////////////////
	//本结构体是否有效，=0表示无效
	if (0 == pcfg->nValid)
	{
		MPU_DBG("0 == pcfg->bValid, nCh = %d\n", nCh);
		return IDVR_CIU_STRUCT_UNVALID;
	}
	MPU_DBG("------byState %d-------\n", pcfg->nEnable);
	num = DVR_MAX_TIMESEGMENT / HY_DVR_MOTIONSEG_DAY;

	//时间表需要扩展
	for (nDay = 0; nDay < DVR_MAX_DAYS; ++nDay)
	{
		for (i = 0; i < HY_DVR_ALARMOUTSEG_DAY; ++i)
		{
			sched.dwTimeSeg[nDay][i] = pcfg->szSchedule[nDay][i * num];
			//MPU_DBG("%d ", pcfg->bySetType[nDay][i * num]);
		}
	}

	//! 状态,0为禁用(全天布防),1为启用,2为不支持
	sched.bytState = pcfg->nEnable;

	//设置视频丢失布防时间计划
	MPU_AVL_AreaDefense_SetSchedule(nCh, &sched);

	return nRet;
}

//获取声音报警布防时间，需要在外部对pcfg清零
static int GetSchedule_AudioAlarm(int nCh, LPHY_DVR_POLYGON_SCHEDULE pcfg)
{
	int nRet = IDVR_OK, nDay, i, num;

	AudioAlarmSchedule sched;

	//! 本结构体是否有效，=0表示无效
	pcfg->nValid = 1;

	///////////////////////////////////////////////////////////////////////////////
	//声音报警布防计划时间表
	/* 0表示不布防，1表示布防 */
	MPU_AVL_AudioAlarm_GetSchedule(nCh, &sched);

	num = DVR_MAX_TIMESEGMENT / HY_DVR_MOTIONSEG_DAY;

	//时间表需要扩展
	for (nDay = 0; nDay < DVR_MAX_DAYS; ++nDay)
	{
		for (i = 0; i < DVR_MAX_TIMESEGMENT; ++i)
		{
			pcfg->szSchedule[nDay][i] = sched.dwTimeSeg[nDay][i / num];
		}
	}

	//! 状态,0为禁用(全天布防),1为启用,2为不支持
	pcfg->nEnable = sched.bytState;

	//! 状态,0为夜晚布防,1为白天布防,2为不支持
	pcfg->nDayMode = sched.nDayMode;
	
	return nRet;
}

//设置声音报警布防时间
static int SetSchedule_AudioAlarm(int nCh, LPHY_DVR_POLYGON_SCHEDULE pcfg)
{
	int nRet = IDVR_OK, nDay, i, num;

	AudioAlarmSchedule sched;

	///////////////////////////////////////////////////////////////////////////////
	//本结构体是否有效，=0表示无效
	if (0 == pcfg->nValid)
	{
		MPU_DBG("0 == pcfg->bValid, nCh = %d\n", nCh);
		return IDVR_CIU_STRUCT_UNVALID;
	}
	MPU_DBG("------byState %d-------\n", pcfg->nEnable);
	num = DVR_MAX_TIMESEGMENT / HY_DVR_MOTIONSEG_DAY;

	//时间表需要扩展
	for (nDay = 0; nDay < DVR_MAX_DAYS; ++nDay)
	{
		for (i = 0; i < HY_DVR_ALARMOUTSEG_DAY; ++i)
		{
			sched.dwTimeSeg[nDay][i] = pcfg->szSchedule[nDay][i * num];
			//MPU_DBG("%d ", pcfg->bySetType[nDay][i * num]);
		}
	}

	//! 状态,0为禁用(全天布防),1为启用,2为不支持
	sched.bytState = pcfg->nEnable;

	//! 状态,0为夜晚布防,1为白天布防,2为不支持
	sched.nDayMode = pcfg->nDayMode;

	//设置声音报警布防时间计划
	MPU_AVL_AudioAlarm_SetSchedule(nCh, &sched);

	return nRet;
}

//获取区域布防联动参数，需外部将缓冲区清零
static int GetHandleConfig_AreaDefense(int nCh, LPHY_DVR_HANDLEEXCEPTION pHandle)
{
	int bEnable, i, nCount, nSDKRet;
	int nPtzAction, nPtzIndex;
	
	//本结构体是否有效，=0表示无效
	pHandle->bValid = 1;

	//联动屏幕预览
	bEnable = MPU_AVL_AreaDefense_IsEnableAction(nCh, ACTION_SCREEN);
	if (bEnable > 0)
	{
		pHandle->dwHandleType |= DVR_ALARM_TRIGGER_PREVIEW;	//联动屏幕提示
		pHandle->nMonitorMask = bEnable;
	}

	bEnable = MPU_AVL_AreaDefense_IsEnableAction(nCh, ACTION_BEEP);
	if (bEnable > 0)
	{
		pHandle->dwHandleType |= DVR_ALARM_TRIGGER_AUDIOOUT;	//联动声音告警
	}

	bEnable = MPU_AVL_AreaDefense_IsEnableAction(nCh, ACTION_SENDTOSERVER);
	if (bEnable > 0)
	{
		pHandle->dwHandleType |= DVR_ALARM_TRIGGER_NOTIFYAMS;	//联动上传报警中心
	}
	
	bEnable = MPU_AVL_AreaDefense_IsEnableAction(nCh, ACTION_EMAIL);
	if (bEnable > 0)
	{
		pHandle->dwHandleType |= DVR_ALARM_TRIGGER_EMAIL;	//联动Email通知
	}
	
	bEnable = MPU_AVL_AreaDefense_IsEnableAction(nCh, ACTION_PTZ);
	if (bEnable > 0)
	{
		pHandle->dwHandleType |= DVR_ALARM_TRIGGER_PTZ;	//联动ptz
	}

	bEnable = MPU_AVL_AreaDefense_IsEnableAction(nCh, ACTION_FTP);
	if (bEnable > 0)
	{
		pHandle->dwHandleType |= DVR_ALARM_TRIGGER_FTP;	//联动ftp
	}

	bEnable = MPU_AVL_AreaDefense_IsEnableAction(nCh, ACTION_PUSH);
	if (bEnable > 0)
	{
		pHandle->dwHandleType |= DVR_ALARM_TRIGGER_PUSH; //联动推送
	}

	bEnable = MPU_AVL_AreaDefense_IsEnableAction(nCh, ACTION_LIGHTALARM);
	if (bEnable > 0)
	{
		pHandle->dwHandleType |= DVR_ALARM_TRIGGER_LIGHTALARM; //联动灯光报警
	}
	
	//获取视频输入通道总数
	nCount = MAX_ENC_CHANNEL_NUM;
	if (nCount > DVR_MAX_PTZ_NUM)
	{
		MPU_ERR("remote config vi lost %d, real %d\n", DVR_MAX_PTZ_NUM, nCount);
		nCount = DVR_MAX_PTZ_NUM;
	}
	for (i = 0; i < nCount; ++i)
	{
		//PTZ动作
		nSDKRet = MPU_AVL_AreaDefense_GetPtzParam(nCh, i, &nPtzAction, &nPtzIndex);
		pHandle->szPtzIndex[i] = nPtzIndex;
		pHandle->szPtzAction[i] = nPtzAction;

		MPU_CHK(0 == nSDKRet);
	}
	
	bEnable = MPU_AVL_AreaDefense_IsEnableAction(nCh, ACTION_SNAPSHOT);
	if (bEnable > 0)
	{
		pHandle->dwHandleType |= DVR_ALARM_TRIGGER_SNAPSHOT;	//联动快照
	}
	//联动快照通道标志
	nSDKRet = MPU_AVL_AreaDefense_GetSnapshotParam(nCh, &pHandle->nSnapshotChannelMask);
	MPU_CHK(0 == nSDKRet);
	
	bEnable = MPU_AVL_AreaDefense_IsEnableAction(nCh, ACTION_RECORD);
	if (bEnable > 0)
	{
		pHandle->dwHandleType |= DVR_ALARM_TRIGGER_RECORD;	//联动录像
	}
	//联动录像通道标志
	nSDKRet = MPU_AVL_AreaDefense_GetRecordParam(nCh, &pHandle->nRecordChannelMask);
	MPU_CHK(0 == nSDKRet);
	
	bEnable = MPU_AVL_AreaDefense_IsEnableAction(nCh, ACTION_ALARMOUT);
	if (bEnable > 0)
	{
		pHandle->dwHandleType |= DVR_ALARM_TRIGGER_ALARMOUT;	//联动报警输出
	}
	//联动报警输出通道标志
	nSDKRet = MPU_AVL_AreaDefense_GetDopParam(nCh, &pHandle->nAlarmOutChannelMask);
	MPU_CHK(0 == nSDKRet);

	return IDVR_OK;
}

//设置区域布防联动参数
static int SetHandleConfig_AreaDefense(int nCh, LPHY_DVR_HANDLEEXCEPTION pHandle)
{
	int bEnable, i, nCount, nSDKRet;

	bEnable = (pHandle->dwHandleType & DVR_ALARM_TRIGGER_EMAIL) ? TRUE : FALSE;
	//本结构体是否有效，=0表示无效
	if (0 == pHandle->bValid)
	{
		MPU_DBG("0 == pHandle->bValid, nCh = %d\n", nCh);
		return IDVR_CIU_STRUCT_UNVALID;
	}
	MPU_DBG("----SetHandleConfig_AreaDefense--------dwHandleType=%x----------\n",pHandle->dwHandleType);
	
	//联动屏幕提示
	bEnable = (pHandle->dwHandleType & DVR_ALARM_TRIGGER_PREVIEW) ? TRUE : FALSE;
	nSDKRet = MPU_AVL_AreaDefense_EnableAction(nCh, ACTION_SCREEN, bEnable/*pHandle->nMonitorMask*/);
	MPU_CHK(0 == nSDKRet);

	//联动声音告警
	bEnable = (pHandle->dwHandleType & DVR_ALARM_TRIGGER_AUDIOOUT) ? TRUE : FALSE;
	nSDKRet = MPU_AVL_AreaDefense_EnableAction(nCh, ACTION_BEEP, bEnable);
	MPU_CHK(0 == nSDKRet);
	
	//联动上传报警中心
	bEnable = (pHandle->dwHandleType & DVR_ALARM_TRIGGER_NOTIFYAMS) ? TRUE : FALSE;
	nSDKRet = MPU_AVL_AreaDefense_EnableAction(nCh, ACTION_SENDTOSERVER, bEnable);
	MPU_CHK(0 == nSDKRet);

	//联动Email通知
	bEnable = (pHandle->dwHandleType & DVR_ALARM_TRIGGER_EMAIL) ? TRUE : FALSE;
	nSDKRet = MPU_AVL_AreaDefense_EnableAction(nCh, ACTION_EMAIL, bEnable);
	MPU_CHK(0 == nSDKRet);
	
	//联动Ptz
	bEnable = (pHandle->dwHandleType & DVR_ALARM_TRIGGER_PTZ) ? TRUE : FALSE;
	nSDKRet = MPU_AVL_AreaDefense_EnableAction(nCh, ACTION_PTZ, bEnable);
	MPU_CHK(0 == nSDKRet);

	//联动ftp
	bEnable = (pHandle->dwHandleType & DVR_ALARM_TRIGGER_FTP) ? TRUE : FALSE;
	nSDKRet = MPU_AVL_AreaDefense_EnableAction(nCh, ACTION_FTP, bEnable);
	MPU_CHK(0 == nSDKRet);

	//联动推送
	bEnable = (pHandle->dwHandleType & DVR_ALARM_TRIGGER_PUSH) ? TRUE : FALSE;
	nSDKRet = MPU_AVL_AreaDefense_EnableAction(nCh, ACTION_PUSH, bEnable);
	MPU_CHK(0 == nSDKRet);

	//获取视频输入通道总数
	nCount = MAX_ENC_CHANNEL_NUM;
	if (nCount > DVR_MAX_PTZ_NUM)
	{
		MPU_ERR("remote config vi lost %d, real %d\n", DVR_MAX_PTZ_NUM, nCount);
		nCount = DVR_MAX_PTZ_NUM;
	}
	for (i = 0; i < nCount; ++i)
	{
		//PTZ动作
		nSDKRet = MPU_AVL_AreaDefense_SetPtzParam(nCh, i, pHandle->szPtzAction[i], pHandle->szPtzIndex[i]);
		MPU_CHK(0 == nSDKRet);
	}

	//联动拍照
	bEnable = (pHandle->dwHandleType & DVR_ALARM_TRIGGER_SNAPSHOT) ? TRUE : FALSE;
	nSDKRet = MPU_AVL_AreaDefense_EnableAction(nCh, ACTION_SNAPSHOT, bEnable);
	MPU_CHK(0 == nSDKRet);

	//联动拍照通道标志
	nSDKRet = MPU_AVL_AreaDefense_SetSnapshotParam(nCh, pHandle->nSnapshotChannelMask);
	MPU_CHK(0 == nSDKRet);

	//联动录像
	bEnable = (pHandle->dwHandleType & DVR_ALARM_TRIGGER_RECORD) ? TRUE : FALSE;
	nSDKRet = MPU_AVL_AreaDefense_EnableAction(nCh, ACTION_RECORD, bEnable);
	MPU_CHK(0 == nSDKRet);

	//联动录像通道标志
	nSDKRet = MPU_AVL_AreaDefense_SetRecordParam(nCh, pHandle->nRecordChannelMask);
	MPU_CHK(0 == nSDKRet);
	
	//联动报警输出
	bEnable = (pHandle->dwHandleType & DVR_ALARM_TRIGGER_ALARMOUT) ? TRUE : FALSE;
	nSDKRet = MPU_AVL_AreaDefense_EnableAction(nCh, ACTION_ALARMOUT, bEnable);
	MPU_CHK(0 == nSDKRet);

	//联动灯光报警
	bEnable = (pHandle->dwHandleType & DVR_ALARM_TRIGGER_LIGHTALARM) ? TRUE : FALSE;
	nSDKRet = MPU_AVL_AreaDefense_EnableAction(nCh, ACTION_LIGHTALARM, bEnable);
	MPU_CHK(0 == nSDKRet);

	//联动报警输出通道标志
	nSDKRet = MPU_AVL_AreaDefense_SetDopParam(nCh, pHandle->nAlarmOutChannelMask);
	MPU_CHK(0 == nSDKRet);

	return IDVR_OK;
}

static int GetAlarm_Polygon(LPHY_DVR_POLYGON_CONFIG pcfg)
{
	int i = 0;
	struct tagPolygon region = {0};

	pcfg->nValid = 1;
	MPU_AVL_GetPolygonRegionCfg(0, &region);
	pcfg->nValid = 1;
	pcfg->nPolygonLineNum = region.size;
	for (i = 0; i < region.size; i++)
	{
		pcfg->struPolygon[i].bValid = 1;
		pcfg->struPolygon[i].nX = region.point[i].x;
		pcfg->struPolygon[i].nY = region.point[i].y;
	}
	pcfg->nPolygonLineDirection = 2;

	// 获取移动侦测使能
	pcfg->cUseForMotionDetect = MPU_AVL_AreaDefense_Motion_IsEnable(0);
	// 获取人形侦测使能
	pcfg->cUseForPeopleDetect = MPU_AVL_AreaDefense_HumanShape_IsEnable(0);
	// 获取人脸侦测使能
	pcfg->cUseForFaceDetect = MPU_AVL_AreaDefense_FaceCap_IsEnable(0);

	// 联动方式获取
	GetHandleConfig_AreaDefense(0, &pcfg->struHandleException);

	return IDVR_OK;
}

static int SetAlarm_Polygon(DWORD dwUserID, LPHY_DVR_POLYGON_CONFIG pcfg)
{
	if (NULL == pcfg)
	{
		return IDVR_CIU_STRUCT_UNVALID;
	}

	int nRet = 0;
	if (pcfg->nValid)
	{
		// 检测图形边的个数
		if ((pcfg->nPolygonLineNum < 3) || (pcfg->nPolygonLineNum > 8))
		{
			return IDVR_CIU_STRUCT_UNVALID;
		}
		struct tagPolygon region = {0};

		region.bValid = 1;
		region.size = pcfg->nPolygonLineNum;
		
		for (int i = 0; i < region.size; i++)
		{
			region.point[i].x = pcfg->struPolygon[i].nX;
			region.point[i].y = pcfg->struPolygon[i].nY;
		}

		int rc = MPU_AVL_SetPolygonRegionCfg(0, &region);
		if (0 != rc)
		{
			return nRet;
		}
		// 1.设置使能
		MPU_AVL_AreaDefense_Motion_Enable(0, pcfg->cUseForMotionDetect);
		MPU_AVL_AreaDefense_HumanShape_Enable(0, pcfg->cUseForPeopleDetect);
		MPU_AVL_AreaDefense_FaceCap_Enable(0, pcfg->cUseForFaceDetect);

		SetHandleConfig_AreaDefense(0, &pcfg->struHandleException);

#if defined (__IS_SUPPORT_LONGAWAYHOME__)
        int nLongAwayEn = 0; 
        MPU_AVL_HumanShape_GetLongAwayEnable(0, &nLongAwayEn);
        /* 关人形，需要强制联动关闭长久离家 */
        if (!pcfg->cUseForPeopleDetect && nLongAwayEn)
        {
            MPU_AVL_HumanShape_SetLongAwayEnable(0, 0);
        }
#endif
	}

	return nRet;
}

static HY_DVR_ASSISSTANT_LINE_CONFIG g_line_config = {0};
static HY_DVR_ASSISSTANT_LINE_SCHEDULE g_line_schedule = {0};

int CIU_GetConfigure_Alarm(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen)
{
	int nRet = IDVR_OK;

	MPU_DBG("ID %d, Major %d, Minor %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType);

	switch (dwMinorType)
	{
			/*报警配置 */
    case ALARMCFG_ALL:
        break;

    case ALARMCFG_ALL_EX:
        break;

    case ALARMCFG_IMGCAPTURE_CONF_V2:
    {
        ///////////////////////////////////////////////////////////////////////////////
        //判断buf空间是否足够
        if (*pdwBufLen < sizeof(HY_DVR_ALARM_CAPTURE_PIC_V2))
        {
            nRet = IDVR_CIU_BUF_SMALL;
            MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_ALARM_CAPTURE_PIC_V2), *pdwBufLen);
            break;
        }

        LPHY_DVR_ALARM_CAPTURE_PIC_V2 pcfg = (LPHY_DVR_ALARM_CAPTURE_PIC_V2)pBuffer;

        ///////////////////////////////////////////////////////////////////////////////
        //初始化输出缓冲区
        memset(pcfg, 0, sizeof(HY_DVR_ALARM_CAPTURE_PIC_V2));
        *pdwBufLen = sizeof(HY_DVR_ALARM_CAPTURE_PIC_V2);
        nRet = GetCapturePicCfgV2(0, pcfg);
    }
    break;

    case ALARMCFG_ALARM_OVERLAY_INFO:
    {
        ///////////////////////////////////////////////////////////////////////////////
        //判断buf空间是否足够
        if (*pdwBufLen < sizeof(HY_DVR_ALARM_OVERLAY_INFO))
        {
            nRet = IDVR_CIU_BUF_SMALL;
            MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_ALARM_OVERLAY_INFO), *pdwBufLen);
            break;
        }

        LPHY_DVR_ALARM_OVERLAY_INFO pcfg = (LPHY_DVR_ALARM_OVERLAY_INFO)pBuffer;

        ///////////////////////////////////////////////////////////////////////////////
        //初始化输出缓冲区
        memset(pcfg, 0, sizeof(HY_DVR_ALARM_OVERLAY_INFO));
        *pdwBufLen = sizeof(HY_DVR_ALARM_OVERLAY_INFO);
        nRet = GetOverlayInfo(0, pcfg);
    }
            break;
        
        case ALARMCFG_ALARM_LIGHT_CONFIG:
    		{
    			//判断buf空间是否足够
                if (*pdwBufLen < sizeof(HY_DVR_LIGHT_WARNING))
                {
                    nRet = IDVR_CIU_BUF_SMALL;
                    MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_LIGHT_WARNING), *pdwBufLen);
                    break;
                }
                
                LPHY_DVR_LIGHT_WARNING pcfg = (LPHY_DVR_LIGHT_WARNING)pBuffer;
                
                ///////////////////////////////////////////////////////////////////////////////
                //初始化输出缓冲区
                memset(pcfg, 0, sizeof(HY_DVR_LIGHT_WARNING));
                *pdwBufLen = sizeof(HY_DVR_LIGHT_WARNING);
                nRet = GetAlarmLightCfg(0, pcfg);
    		}
		    break;

		case ALARMCFG_ALARM_POLYGON_AREA_INFO:
			{
                //判断buf空间是否足够
                if (*pdwBufLen < sizeof(HY_DVR_POLYGON_CONFIG))
                {
                    nRet = IDVR_CIU_BUF_SMALL;
                    MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_POLYGON_CONFIG), *pdwBufLen);
                    break;
                }
                
                LPHY_DVR_POLYGON_CONFIG pcfg = (LPHY_DVR_POLYGON_CONFIG)pBuffer;
                
                ///////////////////////////////////////////////////////////////////////////////
                //初始化输出缓冲区
                memset(pcfg, 0, sizeof(HY_DVR_POLYGON_CONFIG));
                *pdwBufLen = sizeof(HY_DVR_POLYGON_CONFIG);

				int nSDKRet = 0;
				nSDKRet = GetAlarm_Polygon(pcfg);
				CIU_ERR(nSDKRet, nRet);
			}
			break;

		case ALARMCFG_ALARM_POLYGON_SCHEDULE:
			{
				//判断buf空间是否足够
				if (*pdwBufLen < sizeof(HY_DVR_POLYGON_SCHEDULE))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_POLYGON_SCHEDULE), *pdwBufLen);
					break;
				}
				
				LPHY_DVR_POLYGON_SCHEDULE pcfg = (LPHY_DVR_POLYGON_SCHEDULE)pBuffer;
				
				///////////////////////////////////////////////////////////////////////////////
				//初始化输出缓冲区
				memset(pcfg, 0, sizeof(HY_DVR_POLYGON_SCHEDULE));
				*pdwBufLen = sizeof(HY_DVR_POLYGON_SCHEDULE);

				int nSDKRet = 0;
				GetSchedule_AreaDefense(0, pcfg);
				CIU_ERR(nSDKRet, nRet);
			}
			break;

		case ALARMCFG_ALARM_ASSISSTANT_LINE_INFO:
			{
                //判断buf空间是否足够
                if (*pdwBufLen < sizeof(HY_DVR_ASSISSTANT_LINE_CONFIG))
                {
                    nRet = IDVR_CIU_BUF_SMALL;
                    MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_ASSISSTANT_LINE_CONFIG), *pdwBufLen);
                    break;
                }
                
                LPHY_DVR_ASSISSTANT_LINE_CONFIG pcfg = (LPHY_DVR_ASSISSTANT_LINE_CONFIG)pBuffer;
                
                ///////////////////////////////////////////////////////////////////////////////
                //初始化输出缓冲区
                memset(pcfg, 0, sizeof(HY_DVR_ASSISSTANT_LINE_CONFIG));
                *pdwBufLen = sizeof(HY_DVR_ASSISSTANT_LINE_CONFIG);

				mempcpy(pcfg, &g_line_config, sizeof(HY_DVR_ASSISSTANT_LINE_CONFIG));

                pcfg->nValid = 1;
				pcfg->struAssisstantLine.bValid = 1;
				// printf("HGH-TEST[%s %d] b-x[%d] b-y[%d] e-x[%d] e-y[%d]\n", __FUNCTION__, __LINE__,
				// 						pcfg->struAssisstantLine.nBeginX, pcfg->struAssisstantLine.nBeginY,
				// 						pcfg->struAssisstantLine.nEndX, pcfg->struAssisstantLine.nEndY);
                int nSDKRet = 0;
				// nSDKRet = GetPicChannel_HumanShape_Polygon(0, pcfg);
				CIU_ERR(nSDKRet, nRet);
			}
			break;

		case ALARMCFG_ALARM_ASSISSTANT_LINE_SCHEDULE:
			{
                //判断buf空间是否足够
                if (*pdwBufLen < sizeof(HY_DVR_ASSISSTANT_LINE_SCHEDULE))
                {
                    nRet = IDVR_CIU_BUF_SMALL;
                    MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_ASSISSTANT_LINE_SCHEDULE), *pdwBufLen);
                    break;
                }
                
                LPHY_DVR_ASSISSTANT_LINE_SCHEDULE pcfg = (LPHY_DVR_ASSISSTANT_LINE_SCHEDULE)pBuffer;
                
                ///////////////////////////////////////////////////////////////////////////////
                //初始化输出缓冲区
                memset(pcfg, 0, sizeof(HY_DVR_ASSISSTANT_LINE_SCHEDULE));
                *pdwBufLen = sizeof(HY_DVR_ASSISSTANT_LINE_SCHEDULE);

				mempcpy(pcfg, &g_line_schedule, sizeof(HY_DVR_ASSISSTANT_LINE_SCHEDULE));

                pcfg->nValid = 1;


				// printf("HGH-TEST[%s %d] Valid[%d] nEnable[%d]\n", __FUNCTION__, __LINE__, pcfg->nValid, pcfg->nEnable);
				// for (int j = 0; j < DVR_MAX_DAYS; j++)
				// {
				// 	printf("HGH-TEST[%s %d] [%d]", __FUNCTION__, __LINE__, j);
				// 	for (int k = 0; k < DVR_MAX_TIMESEGMENT; k++)
				// 	{
				// 		printf("[%d] ", pcfg->szSchedule[j][k]);
				// 	}
				// 	printf("\n");
				// }

                int nSDKRet = 0;
				// nSDKRet = GetPicChannel_HumanShape_Polygon(0, pcfg);
				CIU_ERR(nSDKRet, nRet);
			}
			break;
		case ALARMCFG_ALARM_VOICE_SCHEDULE:
			{
				//判断buf空间是否足够
				if (*pdwBufLen < sizeof(HY_DVR_POLYGON_SCHEDULE))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_POLYGON_SCHEDULE), *pdwBufLen);
					break;
				}
				
				LPHY_DVR_POLYGON_SCHEDULE pcfg = (LPHY_DVR_POLYGON_SCHEDULE)pBuffer;
				
				///////////////////////////////////////////////////////////////////////////////
				//初始化输出缓冲区
				memset(pcfg, 0, sizeof(HY_DVR_POLYGON_SCHEDULE));
				*pdwBufLen = sizeof(HY_DVR_POLYGON_SCHEDULE);
		
				int nSDKRet = 0;
				GetSchedule_AudioAlarm(0, pcfg);
				CIU_ERR(nSDKRet, nRet);
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

int CIU_GetConfigure_AlarmEx(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen)
{
	int nRet = IDVR_OK;

	MPU_DBG("ID %d, Major %d, Minor %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType);

	switch (dwMinorType)
	{
			/*报警配置 */
		case ALARMCFG_ALL:
			{
				///////////////////////////////////////////////////////////////////////////////
				//判断buf空间是否足够
				if (*pdwBufLen < sizeof(HY_DVR_ALARM_CFG_EX))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_ALARM_CFG_EX), *pdwBufLen);
					break;
				}

				LPHY_DVR_ALARM_CFG_EX pcfg = (LPHY_DVR_ALARM_CFG_EX) pBuffer;

				///////////////////////////////////////////////////////////////////////////////
				//初始化输出缓冲区
				memset(pcfg, 0, sizeof(HY_DVR_ALARM_CFG_EX));
				*pdwBufLen = sizeof(HY_DVR_ALARM_CFG_EX);

				nRet = GetAlarmCfgEx(pcfg);
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

int CIU_GetConfigure_AlarmExV2(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen)
{
	int nRet = IDVR_OK;

	MPU_DBG("ID %d, Major %d, Minor %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType);

	switch (dwMinorType)
	{
			/*报警配置 */
		case ALARMCFG_EX_V2_ALL:
			{
				///////////////////////////////////////////////////////////////////////////////
				//判断buf空间是否足够
				if (*pdwBufLen < sizeof(HY_DVR_ALARM_CFG_EX_V2))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_ALARM_CFG_EX_V2), *pdwBufLen);
					break;
				}

				LPHY_DVR_ALARM_CFG_EX_V2 pcfg = (LPHY_DVR_ALARM_CFG_EX_V2) pBuffer;

				///////////////////////////////////////////////////////////////////////////////
				//初始化输出缓冲区
				memset(pcfg, 0, sizeof(HY_DVR_ALARM_CFG_EX_V2));
				*pdwBufLen = sizeof(HY_DVR_ALARM_CFG_EX_V2);

				nRet = GetAlarmCfgExV2(pcfg);
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

int CIU_SetConfigure_Alarm(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, const BYTE * pBuffer, DWORD dwBufLen)
{
	int nRet = IDVR_OK;

	MPU_DBG("ID %d, Major %d, Minor %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType);


	switch (dwMinorType)
	{
			/*报警配置 */
    case ALARMCFG_ALL:
        break;

    case ALARMCFG_ALL_EX:
        break;

    case ALARMCFG_IMGCAPTURE_CONF_V2:
    {
        ///////////////////////////////////////////////////////////////////////////////
        //判断buf空间是否足够
        if (dwBufLen < sizeof(HY_DVR_ALARM_CAPTURE_PIC_V2))
        {
            nRet = IDVR_CIU_BUF_SMALL;
            MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_ALARM_CAPTURE_PIC_V2), dwBufLen);
            break;
        }

        LPHY_DVR_ALARM_CAPTURE_PIC_V2 pcfg = (LPHY_DVR_ALARM_CAPTURE_PIC_V2)pBuffer;

        nRet = SetCapturePicCfgV2(0, pcfg);
    }
    break;

    case ALARMCFG_ALARM_OVERLAY_INFO:
    {
        ///////////////////////////////////////////////////////////////////////////////
        //判断buf空间是否足够
        if (dwBufLen < sizeof(HY_DVR_ALARM_OVERLAY_INFO))
        {
            nRet = IDVR_CIU_BUF_SMALL;
            MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_ALARM_OVERLAY_INFO), dwBufLen);
            break;
        }

        LPHY_DVR_ALARM_OVERLAY_INFO pcfg = (LPHY_DVR_ALARM_OVERLAY_INFO)pBuffer;

        nRet = SetOverlayInfo(0, pcfg);
    }
    break;

    case ALARMCFG_ALARM_LIGHT_CONFIG:
    {
        //判断buf空间是否足够
        if (dwBufLen < sizeof(HY_DVR_LIGHT_WARNING))
        {
            nRet = IDVR_CIU_BUF_SMALL;
            MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_LIGHT_WARNING), dwBufLen);
            break;
        }

        LPHY_DVR_LIGHT_WARNING pcfg = (LPHY_DVR_LIGHT_WARNING)pBuffer;

        nRet = SetAlarmLightCfg(0, pcfg);
    }
    break;

    case ALARMCFG_ALARM_POLYGON_AREA_INFO:
    {
        ///////////////////////////////////////////////////////////////////////////////
        //判断buf空间是否足够
        if (dwBufLen < sizeof(HY_DVR_POLYGON_CONFIG))
        {
            nRet = IDVR_CIU_BUF_SMALL;
            MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_POLYGON_CONFIG), dwBufLen);
            break;
        }

        LPHY_DVR_POLYGON_CONFIG pcfg = (LPHY_DVR_POLYGON_CONFIG)pBuffer;
        if (0 == pcfg->nValid)
        {
            MPU_DBG("0 == pcfg->nValid\n");
            nRet = IDVR_CIU_STRUCT_UNVALID;
        }
        else
        {
            int nSDKRet = 0;
            nSDKRet = SetAlarm_Polygon(dwUserID, pcfg);
            CIU_ERR(nSDKRet, nRet);
        }
    }
    break;

    case ALARMCFG_ALARM_POLYGON_SCHEDULE:
    {
        ///////////////////////////////////////////////////////////////////////////////
        //判断buf空间是否足够
        if (dwBufLen < sizeof(HY_DVR_POLYGON_SCHEDULE))
        {
            nRet = IDVR_CIU_BUF_SMALL;
            MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_POLYGON_SCHEDULE), dwBufLen);
            break;
        }

        LPHY_DVR_POLYGON_SCHEDULE pcfg = (LPHY_DVR_POLYGON_SCHEDULE)pBuffer;
        if (0 == pcfg->nValid)
        {
            MPU_DBG("0 == pcfg->nValid\n");
            nRet = IDVR_CIU_STRUCT_UNVALID;
        }
        else
        {
            int nSDKRet = 0;
            SetSchedule_AreaDefense(0, pcfg);
            CIU_ERR(nSDKRet, nRet);
        }
    }
    break;

    case ALARMCFG_ALARM_ASSISSTANT_LINE_INFO:
    {
        ///////////////////////////////////////////////////////////////////////////////
        //判断buf空间是否足够
        if (dwBufLen < sizeof(HY_DVR_ASSISSTANT_LINE_CONFIG))
        {
            nRet = IDVR_CIU_BUF_SMALL;
            MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_ASSISSTANT_LINE_CONFIG), dwBufLen);
            break;
        }

        LPHY_DVR_ASSISSTANT_LINE_CONFIG pcfg = (LPHY_DVR_ASSISSTANT_LINE_CONFIG)pBuffer;
        if (0 == pcfg->nValid)
        {
            MPU_DBG("0 == pcfg->nValid\n");
            nRet = IDVR_CIU_STRUCT_UNVALID;
        }
        else
        {
            int nSDKRet = 0;
            // printf("HGH-TEST[%s %d] b-x[%d] b-y[%d] e-x[%d] e-y[%d]\n", __FUNCTION__, __LINE__,
            // 						pcfg->struAssisstantLine.nBeginX, pcfg->struAssisstantLine.nBeginY,
            // 						pcfg->struAssisstantLine.nEndX, pcfg->struAssisstantLine.nEndY);
            mempcpy(&g_line_config, pcfg, sizeof(HY_DVR_ASSISSTANT_LINE_CONFIG));

            CIU_ERR(nSDKRet, nRet);
        }
    }
    break;

    case ALARMCFG_ALARM_ASSISSTANT_LINE_SCHEDULE:
    {
        ///////////////////////////////////////////////////////////////////////////////
        //判断buf空间是否足够
        if (dwBufLen < sizeof(HY_DVR_ASSISSTANT_LINE_SCHEDULE))
        {
            nRet = IDVR_CIU_BUF_SMALL;
            MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_ASSISSTANT_LINE_SCHEDULE), dwBufLen);
            break;
        }

        LPHY_DVR_ASSISSTANT_LINE_SCHEDULE pcfg = (LPHY_DVR_ASSISSTANT_LINE_SCHEDULE)pBuffer;
        if (0 == pcfg->nValid)
        {
            MPU_DBG("0 == pcfg->nValid\n");
            nRet = IDVR_CIU_STRUCT_UNVALID;
        }
        else
        {
            int nSDKRet = 0;

            mempcpy(&g_line_schedule, pcfg, sizeof(HY_DVR_ASSISSTANT_LINE_SCHEDULE));

            CIU_ERR(nSDKRet, nRet);
        }
    }
    break;
    case ALARMCFG_ALARM_VOICE_SCHEDULE:
    {
        ///////////////////////////////////////////////////////////////////////////////
        //判断buf空间是否足够
        if (dwBufLen < sizeof(HY_DVR_POLYGON_SCHEDULE))
        {
            nRet = IDVR_CIU_BUF_SMALL;
            MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_POLYGON_SCHEDULE), dwBufLen);
            break;
        }

        LPHY_DVR_POLYGON_SCHEDULE pcfg = (LPHY_DVR_POLYGON_SCHEDULE)pBuffer;
        if (0 == pcfg->nValid)
        {
            MPU_DBG("0 == pcfg->nValid\n");
            nRet = IDVR_CIU_STRUCT_UNVALID;
        }
        else
        {
            int nSDKRet = 0;
            SetSchedule_AudioAlarm(0, pcfg);
            CIU_ERR(nSDKRet, nRet);
        }
    }
    break;
    case ALARMCFG_ALARM_SIMULATE_DIGITAL_INPUT:
    {
        ///////////////////////////////////////////////////////////////////////////////
        //判断buf空间是否足够
        if (dwBufLen < sizeof(HY_DVR_SIMULATE_DIGITAL_INPUT))
        {
            nRet = IDVR_CIU_BUF_SMALL;
            MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_SIMULATE_DIGITAL_INPUT), dwBufLen);
            break;
        }

        LPHY_DVR_SIMULATE_DIGITAL_INPUT pcfg = (LPHY_DVR_SIMULATE_DIGITAL_INPUT)pBuffer;
        if (0 == pcfg->nValid)
        {
            MPU_DBG("0 == pcfg->nValid\n");
            nRet = IDVR_CIU_STRUCT_UNVALID;
        }
        else
        {
            HieServer_RefreshEvent(USEREVENT_ALARM_NOTICE, ALERT_ALARMIN, 0, pcfg->nState);
        }
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
