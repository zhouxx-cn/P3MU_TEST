
/*!
 *\file	 MPU_CIU_Config_Pic.cpp
 *\brief	MPU模块，远程配置，视频设置
 *\author	林云
 *\date	2009/8/13

功能：
-#	远程配置，视频设置

说明：
-#	本文件使用新的网络库实现
 */

#include "MPU_CIU_global.h"
#include "PUB/lattice2bmp24.h"	//点阵转位图
#include "PUB/common_pub.h"
#include "tagPtzCode.h"

static int bSupportNormalWhite = 0;
static int bSupportColorNight = 0; 
static int bSupportIntelligentNight = 0;	
static int bSupportVariableInfrared = 0;	
static int bSupportVariableWhite = 0;	
static int bSupportFaceNoExposure = 0; 
static int bSupportLicensePlate = 0;	
static int VariableInfraredValueFixed = 0; 
static int VariableWhiteFixed = 0; 
static int bDayNightModeFixd = 0;    //夜视模式是否固定

//兼容新旧灯板能力(规避不支持的模式设置)
static int currentDayNightMode = 0; //当前设置的模式
//普通红外
static int bSptNormIRPassive = 0; //自动被动
static int bSptNormIRActive = 0; //自动主动
static int bSptNormIRDay = 0; //白天
static int bSptNormIRNight = 0; //晚上
static int bSptNormIRplan = 0; //定时
//全彩白
static int bSptColorNightAuto = 0; //全彩白自动
//无极红
static int bSptVaryIRAuto = 0; //无极红自动
//无极白
static int bSptVaryWitAuto = 0; //无极白自动
static int bSptVaryWitManuPlan = 0; //无极白手动下定时
static int bSptVaryWitManuThresh = 0; //无极白手动下阈值调节

#if defined (__IS_SUPPORT_APP_SHARE__)
// 用于避免频繁获取
static int g_bSupportDeviceShared = -1;
static int g_bSupportSelfUnbind = -1;
#endif

extern int GetDevCustom_Pic(LPHY_DVR_WORKMODE pcfg);

//extern int SetDevCustom_Pic(LPHY_DVR_WORKMODE pcfg);

//获取移动侦测联动参数，需外部将缓冲区清零
static int GetHandleConfig_Motion(int nCh, LPHY_DVR_HANDLEEXCEPTION pHandle)
{
	int bEnable, i, nCount, nSDKRet;
	int nPtzAction, nPtzIndex;
	
	//本结构体是否有效，=0表示无效
	pHandle->bValid = 1;

	//联动屏幕预览
	bEnable = MPU_AVL_IsEnableAction(nCh,ALERT_TYPE_MOTION, ACTION_SCREEN);
	if (bEnable > 0)
	{
		pHandle->dwHandleType |= DVR_ALARM_TRIGGER_PREVIEW;	//联动屏幕提示
		pHandle->nMonitorMask = bEnable;
	}

	bEnable = MPU_AVL_IsEnableAction(nCh, ALERT_TYPE_MOTION, ACTION_BEEP);
	if (bEnable > 0)
	{
		pHandle->dwHandleType |= DVR_ALARM_TRIGGER_AUDIOOUT;	//联动声音告警
	}

	bEnable = MPU_AVL_IsEnableAction(nCh, ALERT_TYPE_MOTION, ACTION_SENDTOSERVER);
	if (bEnable > 0)
	{
		pHandle->dwHandleType |= DVR_ALARM_TRIGGER_NOTIFYAMS;	//联动上传报警中心
	}


	bEnable = MPU_AVL_IsEnableAction(nCh, ALERT_TYPE_MOTION, ACTION_EMAIL);
	if (bEnable > 0)
	{
		pHandle->dwHandleType |= DVR_ALARM_TRIGGER_EMAIL;	//联动Email通知
	}


	bEnable = MPU_AVL_IsEnableAction(nCh, ALERT_TYPE_MOTION, ACTION_PTZ);
	if (bEnable > 0)
	{
		pHandle->dwHandleType |= DVR_ALARM_TRIGGER_PTZ;	//联动ptz
	}

	bEnable = MPU_AVL_IsEnableAction(nCh, ALERT_TYPE_MOTION, ACTION_FTP);
	if (bEnable > 0)
	{
		pHandle->dwHandleType |= DVR_ALARM_TRIGGER_FTP;	//联动ftp
	}

	bEnable = MPU_AVL_IsEnableAction(nCh, ALERT_TYPE_MOTION, ACTION_PUSH);
	if (bEnable > 0)
	{
		pHandle->dwHandleType |= DVR_ALARM_TRIGGER_PUSH; //联动推送
	}

	bEnable = MPU_AVL_IsEnableAction(nCh, ALERT_TYPE_MOTION, ACTION_LIGHTALARM);
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
		nSDKRet = MPU_AVL_GetPtzParam(nCh, ALERT_TYPE_MOTION, i, &nPtzAction, &nPtzIndex);
		pHandle->szPtzIndex[i] = nPtzIndex;
		pHandle->szPtzAction[i] = nPtzAction;

		MPU_CHK(0 == nSDKRet);
	}


	bEnable = MPU_AVL_IsEnableAction(nCh, ALERT_TYPE_MOTION, ACTION_SNAPSHOT);
	if (bEnable > 0)
	{
		pHandle->dwHandleType |= DVR_ALARM_TRIGGER_SNAPSHOT;	//联动快照
	}
	//联动快照通道标志
	nSDKRet = MPU_AVL_GetSnapshotParam(nCh, ALERT_TYPE_MOTION, &pHandle->nSnapshotChannelMask);
	MPU_CHK(0 == nSDKRet);


	bEnable = MPU_AVL_IsEnableAction(nCh, ALERT_TYPE_MOTION, ACTION_RECORD);
	if (bEnable > 0)
	{
		pHandle->dwHandleType |= DVR_ALARM_TRIGGER_RECORD;	//联动录像
	}
	//联动录像通道标志
	nSDKRet = MPU_AVL_GetRecordParam(nCh, ALERT_TYPE_MOTION, &pHandle->nRecordChannelMask);
	MPU_CHK(0 == nSDKRet);


	bEnable = MPU_AVL_IsEnableAction(nCh, ALERT_TYPE_MOTION, ACTION_ALARMOUT);
	if (bEnable > 0)
	{
		pHandle->dwHandleType |= DVR_ALARM_TRIGGER_ALARMOUT;	//联动报警输出
	}
	//联动报警输出通道标志
	nSDKRet = MPU_AVL_GetDopParam(nCh, ALERT_TYPE_MOTION, &pHandle->nAlarmOutChannelMask);
	MPU_CHK(0 == nSDKRet);

	return IDVR_OK;
}

//设置移动侦测联动参数
static int SetHandleConfig_Motion(int nCh, LPHY_DVR_HANDLEEXCEPTION pHandle)
{
	int bEnable, i, nCount, nSDKRet;

	//本结构体是否有效，=0表示无效
	if (0 == pHandle->bValid)
	{
		MPU_DBG("0 == pHandle->bValid, nCh = %d\n", nCh);
		return IDVR_CIU_STRUCT_UNVALID;
	}
	MPU_DBG("----SetHandleConfig_Motion--------dwHandleType=%x----------\n",pHandle->dwHandleType);
	//联动屏幕提示
	bEnable = (pHandle->dwHandleType & DVR_ALARM_TRIGGER_PREVIEW) ? TRUE : FALSE;
	nSDKRet = MPU_AVL_EnableAction(nCh, ALERT_TYPE_MOTION, ACTION_SCREEN, bEnable/*pHandle->nMonitorMask*/);
	MPU_CHK(0 == nSDKRet);

	//联动声音告警
	bEnable = (pHandle->dwHandleType & DVR_ALARM_TRIGGER_AUDIOOUT) ? TRUE : FALSE;
	nSDKRet = MPU_AVL_EnableAction(nCh, ALERT_TYPE_MOTION, ACTION_BEEP, bEnable);
	MPU_CHK(0 == nSDKRet);


	//联动上传报警中心
	bEnable = (pHandle->dwHandleType & DVR_ALARM_TRIGGER_NOTIFYAMS) ? TRUE : FALSE;
	nSDKRet = MPU_AVL_EnableAction(nCh, ALERT_TYPE_MOTION, ACTION_SENDTOSERVER, bEnable);
	MPU_CHK(0 == nSDKRet);


	//联动Email通知
	bEnable = (pHandle->dwHandleType & DVR_ALARM_TRIGGER_EMAIL) ? TRUE : FALSE;
	nSDKRet = MPU_AVL_EnableAction(nCh, ALERT_TYPE_MOTION, ACTION_EMAIL, bEnable);
	MPU_CHK(0 == nSDKRet);


	//联动Ptz
	bEnable = (pHandle->dwHandleType & DVR_ALARM_TRIGGER_PTZ) ? TRUE : FALSE;
	nSDKRet = MPU_AVL_EnableAction(nCh, ALERT_TYPE_MOTION, ACTION_PTZ, bEnable);
	MPU_CHK(0 == nSDKRet);

	//联动ftp
	bEnable = (pHandle->dwHandleType & DVR_ALARM_TRIGGER_FTP) ? TRUE : FALSE;
	nSDKRet = MPU_AVL_EnableAction(nCh, ALERT_TYPE_MOTION, ACTION_FTP, bEnable);
	MPU_CHK(0 == nSDKRet);

	//联动推送
	bEnable = (pHandle->dwHandleType & DVR_ALARM_TRIGGER_PUSH) ? TRUE : FALSE;
	nSDKRet = MPU_AVL_EnableAction(nCh, ALERT_TYPE_MOTION, ACTION_PUSH, bEnable);
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
		nSDKRet = MPU_AVL_SetPtzParam(nCh, ALERT_TYPE_MOTION, i, pHandle->szPtzAction[i], pHandle->szPtzIndex[i]);
		MPU_CHK(0 == nSDKRet);
	}


	//联动拍照
	bEnable = (pHandle->dwHandleType & DVR_ALARM_TRIGGER_SNAPSHOT) ? TRUE : FALSE;
	nSDKRet = MPU_AVL_EnableAction(nCh, ALERT_TYPE_MOTION, ACTION_SNAPSHOT, bEnable);
	MPU_CHK(0 == nSDKRet);

	//联动拍照通道标志
	nSDKRet = MPU_AVL_SetSnapshotParam(nCh, ALERT_TYPE_MOTION, pHandle->nSnapshotChannelMask);
	MPU_CHK(0 == nSDKRet);


	//联动录像
	bEnable = (pHandle->dwHandleType & DVR_ALARM_TRIGGER_RECORD) ? TRUE : FALSE;
	nSDKRet = MPU_AVL_EnableAction(nCh, ALERT_TYPE_MOTION, ACTION_RECORD, bEnable);
	MPU_CHK(0 == nSDKRet);

	//联动录像通道标志
	nSDKRet = MPU_AVL_SetRecordParam(nCh, ALERT_TYPE_MOTION, pHandle->nRecordChannelMask);
	MPU_CHK(0 == nSDKRet);

	//联动灯光报警
	bEnable = (pHandle->dwHandleType & DVR_ALARM_TRIGGER_LIGHTALARM) ? TRUE : FALSE;
	nSDKRet = MPU_AVL_EnableAction(nCh, ALERT_TYPE_MOTION, ACTION_LIGHTALARM, bEnable);
	MPU_CHK(0 == nSDKRet);

	//联动报警输出
	bEnable = (pHandle->dwHandleType & DVR_ALARM_TRIGGER_ALARMOUT) ? TRUE : FALSE;
	nSDKRet = MPU_AVL_EnableAction(nCh, ALERT_TYPE_MOTION, ACTION_ALARMOUT, bEnable);
	MPU_CHK(0 == nSDKRet);

	//联动报警输出通道标志
	nSDKRet = MPU_AVL_SetDopParam(nCh, ALERT_TYPE_MOTION, pHandle->nAlarmOutChannelMask);
	MPU_CHK(0 == nSDKRet);

	return IDVR_OK;
}

//获取移动侦测联动参数，需外部将缓冲区清零
static int GetHandleConfig_HumanShape(int nCh, LPHY_DVR_HANDLEEXCEPTION pHandle)
{
	int bEnable, i, nCount, nSDKRet;
	int nPtzAction, nPtzIndex;
	
	//本结构体是否有效，=0表示无效
	pHandle->bValid = 1;

	//联动屏幕预览
	bEnable = MPU_AVL_IsEnableAction(nCh, ALERT_TYPE_HUMANSHAPE, ACTION_SCREEN);
	if (bEnable > 0)
	{
		pHandle->dwHandleType |= DVR_ALARM_TRIGGER_PREVIEW;	//联动屏幕提示
		pHandle->nMonitorMask = bEnable;
	}

	bEnable = MPU_AVL_IsEnableAction(nCh, ALERT_TYPE_HUMANSHAPE, ACTION_BEEP);
	if (bEnable > 0)
	{
		pHandle->dwHandleType |= DVR_ALARM_TRIGGER_AUDIOOUT;	//联动声音告警
	}

	bEnable = MPU_AVL_IsEnableAction(nCh, ALERT_TYPE_HUMANSHAPE, ACTION_SENDTOSERVER);
	if (bEnable > 0)
	{
		pHandle->dwHandleType |= DVR_ALARM_TRIGGER_NOTIFYAMS;	//联动上传报警中心
	}
	
	bEnable = MPU_AVL_IsEnableAction(nCh, ALERT_TYPE_HUMANSHAPE, ACTION_EMAIL);
	if (bEnable > 0)
	{
		pHandle->dwHandleType |= DVR_ALARM_TRIGGER_EMAIL;	//联动Email通知
	}
	
	bEnable = MPU_AVL_IsEnableAction(nCh, ALERT_TYPE_HUMANSHAPE, ACTION_PTZ);
	if (bEnable > 0)
	{
		pHandle->dwHandleType |= DVR_ALARM_TRIGGER_PTZ;	//联动ptz
	}

	bEnable = MPU_AVL_IsEnableAction(nCh, ALERT_TYPE_HUMANSHAPE, ACTION_FTP);
	if (bEnable > 0)
	{
		pHandle->dwHandleType |= DVR_ALARM_TRIGGER_FTP;	//联动ftp
	}

	bEnable = MPU_AVL_IsEnableAction(nCh, ALERT_TYPE_HUMANSHAPE, ACTION_PUSH);
	if (bEnable > 0)
	{
		pHandle->dwHandleType |= DVR_ALARM_TRIGGER_PUSH; //联动推送
	}

	bEnable = MPU_AVL_IsEnableAction(nCh, ALERT_TYPE_HUMANSHAPE, ACTION_LIGHTALARM);
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
		nSDKRet = MPU_AVL_GetPtzParam(nCh, ALERT_TYPE_HUMANSHAPE, i, &nPtzAction, &nPtzIndex);
		pHandle->szPtzIndex[i] = nPtzIndex;
		pHandle->szPtzAction[i] = nPtzAction;

		MPU_CHK(0 == nSDKRet);
	}
	
	bEnable = MPU_AVL_IsEnableAction(nCh, ALERT_TYPE_HUMANSHAPE, ACTION_SNAPSHOT);
	if (bEnable > 0)
	{
		pHandle->dwHandleType |= DVR_ALARM_TRIGGER_SNAPSHOT;	//联动快照
	}
	//联动快照通道标志
	nSDKRet = MPU_AVL_GetSnapshotParam(nCh, ALERT_TYPE_HUMANSHAPE, &pHandle->nSnapshotChannelMask);
	MPU_CHK(0 == nSDKRet);
	
	bEnable = MPU_AVL_IsEnableAction(nCh, ALERT_TYPE_HUMANSHAPE, ACTION_RECORD);
	if (bEnable > 0)
	{
		pHandle->dwHandleType |= DVR_ALARM_TRIGGER_RECORD;	//联动录像
	}
	//联动录像通道标志
	nSDKRet = MPU_AVL_GetRecordParam(nCh, ALERT_TYPE_HUMANSHAPE, &pHandle->nRecordChannelMask);
	MPU_CHK(0 == nSDKRet);
	
	bEnable = MPU_AVL_IsEnableAction(nCh, ALERT_TYPE_HUMANSHAPE, ACTION_ALARMOUT);
	if (bEnable > 0)
	{
		pHandle->dwHandleType |= DVR_ALARM_TRIGGER_ALARMOUT;	//联动报警输出
	}
	//联动报警输出通道标志
	nSDKRet = MPU_AVL_GetDopParam(nCh, ALERT_TYPE_HUMANSHAPE, &pHandle->nAlarmOutChannelMask);
	MPU_CHK(0 == nSDKRet);

	return IDVR_OK;
}

//设置移动侦测联动参数
static int SetHandleConfig_HumanShape(int nCh, LPHY_DVR_HANDLEEXCEPTION pHandle)
{
	int bEnable, i, nCount, nSDKRet;

	//本结构体是否有效，=0表示无效
	if (0 == pHandle->bValid)
	{
		MPU_DBG("0 == pHandle->bValid, nCh = %d\n", nCh);
		return IDVR_CIU_STRUCT_UNVALID;
	}
	MPU_DBG("----SetHandleConfig_HumanShape--------dwHandleType=%x----------\n",pHandle->dwHandleType);
	//联动屏幕提示
	bEnable = (pHandle->dwHandleType & DVR_ALARM_TRIGGER_PREVIEW) ? TRUE : FALSE;
	nSDKRet = MPU_AVL_EnableAction(nCh, ALERT_TYPE_HUMANSHAPE, ACTION_SCREEN, bEnable/*pHandle->nMonitorMask*/);
	MPU_CHK(0 == nSDKRet);

	//联动声音告警
	bEnable = (pHandle->dwHandleType & DVR_ALARM_TRIGGER_AUDIOOUT) ? TRUE : FALSE;
	nSDKRet = MPU_AVL_EnableAction(nCh, ALERT_TYPE_HUMANSHAPE, ACTION_BEEP, bEnable);
	MPU_CHK(0 == nSDKRet);
	
	//联动上传报警中心
	bEnable = (pHandle->dwHandleType & DVR_ALARM_TRIGGER_NOTIFYAMS) ? TRUE : FALSE;
	nSDKRet = MPU_AVL_EnableAction(nCh, ALERT_TYPE_HUMANSHAPE, ACTION_SENDTOSERVER, bEnable);
	MPU_CHK(0 == nSDKRet);

	//联动Email通知
	bEnable = (pHandle->dwHandleType & DVR_ALARM_TRIGGER_EMAIL) ? TRUE : FALSE;
	nSDKRet = MPU_AVL_EnableAction(nCh, ALERT_TYPE_HUMANSHAPE, ACTION_EMAIL, bEnable);
	MPU_CHK(0 == nSDKRet);
	
	//联动Ptz
	bEnable = (pHandle->dwHandleType & DVR_ALARM_TRIGGER_PTZ) ? TRUE : FALSE;
	nSDKRet = MPU_AVL_EnableAction(nCh, ALERT_TYPE_HUMANSHAPE, ACTION_PTZ, bEnable);
	MPU_CHK(0 == nSDKRet);

	//联动ftp
	bEnable = (pHandle->dwHandleType & DVR_ALARM_TRIGGER_FTP) ? TRUE : FALSE;
	nSDKRet = MPU_AVL_EnableAction(nCh, ALERT_TYPE_HUMANSHAPE, ACTION_FTP, bEnable);
	MPU_CHK(0 == nSDKRet);

	//联动推送
	bEnable = (pHandle->dwHandleType & DVR_ALARM_TRIGGER_PUSH) ? TRUE : FALSE;
	nSDKRet = MPU_AVL_EnableAction(nCh, ALERT_TYPE_HUMANSHAPE, ACTION_PUSH, bEnable);
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
		nSDKRet = MPU_AVL_SetPtzParam(nCh, ALERT_TYPE_HUMANSHAPE, i, pHandle->szPtzAction[i], pHandle->szPtzIndex[i]);
		MPU_CHK(0 == nSDKRet);
	}

	//联动拍照
	bEnable = (pHandle->dwHandleType & DVR_ALARM_TRIGGER_SNAPSHOT) ? TRUE : FALSE;
	nSDKRet = MPU_AVL_EnableAction(nCh, ALERT_TYPE_HUMANSHAPE, ACTION_SNAPSHOT, bEnable);
	MPU_CHK(0 == nSDKRet);

	//联动拍照通道标志
	nSDKRet = MPU_AVL_SetSnapshotParam(nCh, ALERT_TYPE_HUMANSHAPE, pHandle->nSnapshotChannelMask);
	MPU_CHK(0 == nSDKRet);

	//联动录像
	bEnable = (pHandle->dwHandleType & DVR_ALARM_TRIGGER_RECORD) ? TRUE : FALSE;
	nSDKRet = MPU_AVL_EnableAction(nCh, ALERT_TYPE_HUMANSHAPE, ACTION_RECORD, bEnable);
	MPU_CHK(0 == nSDKRet);

	//联动录像通道标志
	nSDKRet = MPU_AVL_SetRecordParam(nCh, ALERT_TYPE_HUMANSHAPE, pHandle->nRecordChannelMask);
	MPU_CHK(0 == nSDKRet);
	
	//联动报警输出
	bEnable = (pHandle->dwHandleType & DVR_ALARM_TRIGGER_ALARMOUT) ? TRUE : FALSE;
	nSDKRet = MPU_AVL_EnableAction(nCh, ALERT_TYPE_HUMANSHAPE, ACTION_ALARMOUT, bEnable);
	MPU_CHK(0 == nSDKRet);

	//联动灯光报警
	bEnable = (pHandle->dwHandleType & DVR_ALARM_TRIGGER_LIGHTALARM) ? TRUE : FALSE;
	nSDKRet = MPU_AVL_EnableAction(nCh, ALERT_TYPE_HUMANSHAPE, ACTION_LIGHTALARM, bEnable);
	MPU_CHK(0 == nSDKRet);

	//联动报警输出通道标志
	nSDKRet = MPU_AVL_SetDopParam(nCh, ALERT_TYPE_HUMANSHAPE, pHandle->nAlarmOutChannelMask);
	MPU_CHK(0 == nSDKRet);

	return IDVR_OK;
}

//获取移动侦测布防时间，需要在外部对pcfg清零
static int GetSchedule_Motion(int nCh, LPHY_DVR_SCHEDTIME pcfg)
{
	int nRet = IDVR_OK, nDay, i, num;

	MotionSchedule sched;

	//! 本结构体是否有效，=0表示无效
	pcfg->bValid = 1;

	///////////////////////////////////////////////////////////////////////////////
	//移动侦测报警布防计划时间表
	/* 0表示不布防，1表示布防 */
	MPU_AVL_Motion_GetSchedule(nCh, &sched);

	num = DVR_MAX_TIMESEGMENT / HY_DVR_MOTIONSEG_DAY;

	//时间表需要扩展
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

//设置移动侦测布防时间
static int SetSchedule_Motion(int nCh, LPHY_DVR_SCHEDTIME pcfg)
{
	int nRet = IDVR_OK, nDay, i, num;
	MotionSchedule sched;

	///////////////////////////////////////////////////////////////////////////////
	//本结构体是否有效，=0表示无效
	if (0 == pcfg->bValid)
	{
		MPU_DBG("0 == pcfg->bValid, nCh = %d\n", nCh);
		return IDVR_CIU_STRUCT_UNVALID;
	}
	MPU_DBG("------byState %d-------\n", pcfg->byState);
	num = DVR_MAX_TIMESEGMENT / HY_DVR_MOTIONSEG_DAY;

	//时间表需要扩展
	for (nDay = 0; nDay < DVR_MAX_DAYS; ++nDay)
	{
		for (i = 0; i < HY_DVR_ALARMOUTSEG_DAY; ++i)
		{
			sched.dwTimeSeg[nDay][i] = pcfg->bySetType[nDay][i * num];
			//MPU_DBG("%d ", pcfg->bySetType[nDay][i * num]);
		}
		//MPU_DBG("\n");
	}

	//! 状态,0为禁用(全天布防),1为启用,2为不支持
	sched.bytState = pcfg->byState;
	sched.bytReserve[0] = pcfg->byReserve[0];
	sched.bytReserve[1] = pcfg->byReserve[1];
	sched.bytReserve[2] = pcfg->byReserve[2];

	//设置视频丢失布防时间计划
	MPU_AVL_Motion_SetSchedule(nCh, &sched);

	return nRet;
}

//获取移动侦测布防时间，需要在外部对pcfg清零
static int GetSchedule_HumanShape(int nCh, LPHY_DVR_SCHEDTIME pcfg)
{
	int nRet = IDVR_OK, nDay, i, num;
	MotionSchedule sched;

	//! 本结构体是否有效，=0表示无效
	pcfg->bValid = 1;

	///////////////////////////////////////////////////////////////////////////////
	//移动侦测报警布防计划时间表
	/* 0表示不布防，1表示布防 */
	MPU_AVL_HumanShape_GetSchedule(nCh, &sched);

	num = DVR_MAX_TIMESEGMENT / HY_DVR_MOTIONSEG_DAY;

	//时间表需要扩展
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

//获取各通道OSD，需外部将缓冲区清零
static int GetPicChannel_Osd(int nCh, LPHY_DVR_OSD_CFG pcfg)
{
	int nRet = IDVR_OK, nSDKRet;

	///////////////////////////////////////////////////////////////////////////////
	//本结构体是否有效，=0表示无效
	pcfg->bValid = 1;

	///////////////////////////////////////////////////////////////////////////////
	//显示通道名 
	/* 0为通道时间均不显示、1为显示通道，2、为显示时间、3为两者均显示 */
	nSDKRet = MPU_AVL_OSD_GetType(nCh, &pcfg->nShowOsd);
	MPU_CHK(0 == nSDKRet);

	//osd的坐标
	nSDKRet = MPU_AVL_OSD_GetPosition(nCh, 0, &pcfg->nTimeTopLeftX, &pcfg->nTimeTopLeftY);
	MPU_CHK(0 == nSDKRet);

	nSDKRet = MPU_AVL_OSD_GetPosition(nCh, 1, &pcfg->nNameTopLeftX, &pcfg->nNameTopLeftY);
	MPU_CHK(0 == nSDKRet);

	return nRet;
}

//设置各通道OSD
static int SetPicChannel_Osd(int nCh, LPHY_DVR_OSD_CFG pcfg)
{
	int nRet = IDVR_OK, nSDKRet;

	///////////////////////////////////////////////////////////////////////////////
	//本结构体是否有效，=0表示无效
	if (0 == pcfg->bValid)
	{
		MPU_DBG("0 == pcfg->bValid, nCh = %d\n", nCh);
		return IDVR_CIU_STRUCT_UNVALID;
	}

	///////////////////////////////////////////////////////////////////////////////
	//显示通道名 
	/* 0为通道时间均不显示、1为显示通道，2、为显示时间、3为两者均显示 */
	nSDKRet = MPU_AVL_OSD_SetType(nCh, pcfg->nShowOsd);
	if (0 != nSDKRet)
	{
		MPU_ERR("set channel %d osd tpye %d fail, return %d\n", nCh, pcfg->nShowOsd, nSDKRet);
		nRet = IDVR_CIU_CALLSDK_FAIL;
	}
	MPU_DBG("nTimeTopLeftX=%d nTimeTopLeftY=%d nNameTopLeftX=%d nNameTopLeftY=%d\n", 
		pcfg->nTimeTopLeftX, pcfg->nTimeTopLeftY, pcfg->nNameTopLeftX, pcfg->nNameTopLeftY);
	///////////////////////////////////////////////////////////////////////////////
	//显示坐标
	/* 0为通道时间均不设置、1为设置通道，2、为设置时间、3为两者均设置 */
	nSDKRet = MPU_AVL_OSD_SetPositionAll(nCh, pcfg->nTimeTopLeftX, pcfg->nTimeTopLeftY, pcfg->nNameTopLeftX, pcfg->nNameTopLeftY, 3);
	MPU_CHK(0 == nSDKRet);

	if (0 == nSDKRet)
	{
		//通知GUI通道别名变更
		nSDKRet = CIU_NotifyRemoteControl(0, eRemoteMoveVideoInAlias, nCh);
		MPU_CHK(0 == nSDKRet);
	}
	else
	{
		MPU_ERR("set channel %d osd position fail, return %d\n", nCh, nSDKRet);
		nRet = IDVR_CIU_CALLSDK_FAIL;
	}

	return nRet;
}
#define NO_USE_BITMAP  //lhl add

#if 0
//获取各通道OSD，需外部将缓冲区清零
static int GetPicChannel_Alias(int nCh, LPHY_DVR_ALIAS_CFG pcfg)
{
	int nRet;

	char szSection[30];

	char szDefaultName[30];

	snprintf(szSection, sizeof(szSection), SECTION_OSD_ALIAS, nCh);

	snprintf(szDefaultName, sizeof(szDefaultName), DEFNAME_OSD_ALIAS, nCh + 1);

	nRet = PUB_GetAlias(szSection, szDefaultName, pcfg);

	return nRet;
}


//设置各通道OSD
static int SetPicChannel_Alias(int nCh, LPHY_DVR_ALIAS_CFG pcfg)
{
	int nRet, nSDKRet;

	char szSection[30];

	char szFileName[64];

	char szBakFileName[64];

	int tmp=0;

	snprintf(szSection, sizeof(szSection), SECTION_OSD_ALIAS, nCh);

	///////////////////////////////////////////////////////////////////////////////
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

	if (0 == strlen(pcfg->strName))
	{
		MPU_ERR("%s: is NULL\n", szSection);
		return IDVR_CIU_PARAM_OVERFLOW;
	}

	MPU_INFO("%s: [%s] nWidth %d, nHeight %d\n", szSection, pcfg->strName, pcfg->nWidth, pcfg->nHeight);

	//需要保存的位图文件名
	snprintf(szFileName, sizeof(szFileName), IDVR_OSD_ALIAS, nCh + 1);

	//备份位图文件名
	snprintf(szBakFileName, sizeof(szBakFileName),  "%s.bak", szFileName);

	//先备份原有位图
	rename(szFileName, szBakFileName);

	//生成新的位图
	nSDKRet = lattice_saveas_bmp24(pcfg->nWidth, pcfg->nHeight, pcfg->byLattice, szFileName, 1);
	if (0 == nSDKRet)
	{
		#ifdef NO_USE_BITMAP//用freetype库的话要先将osd配置到文件
		//保存到配置文件
		nSDKRet = PUB_SetAlias(szSection, pcfg);
		MPU_CHK(0 == nSDKRet);
		#endif
		//设置点阵到系统sdk
		MPU_DBG("\n....设置点阵到系统sdk......mpu_ciu_config_pic.cpp......SetPicChannel_Alias..szFileName=%s..\n",szFileName);
		nSDKRet = MPU_AVL_OSD_SetCharBitMap(nCh, szFileName, pcfg->byLattice,pcfg->struFontInfo.nFontSize, pcfg->nWidth, pcfg->nHeight);
		if (0 == nSDKRet)
		{
			#ifndef NO_USE_BITMAP
			//保存到配置文件
			nSDKRet = PUB_SetAlias(szSection, pcfg);
			MPU_CHK(0 == nSDKRet);
			#endif
			//通知GUI通道别名变更
			CIU_NotifyRemoteControl(0, eRemoteSetVideoInAlias, nCh);

			nRet = IDVR_OK;

			//设置成功，删除备份位图
			remove(szBakFileName);
		}
		else
		{
			MPU_ERR("%s: set bmp to dsp fail, return %d\n", szSection, nSDKRet);
			nRet = IDVR_CIU_CALLSDK_FAIL;

			//设置失败，恢复备份位图
			rename(szBakFileName, szFileName);

			//恢复位图到DSP
			nSDKRet = MPU_AVL_OSD_SetCharBitMap(nCh, szFileName, pcfg->byLattice,pcfg->struFontInfo.nFontSize, pcfg->nWidth, pcfg->nHeight);
			if (0 != nSDKRet)
			{
				MPU_ERR("%s: restore bmp to dsp fail, return %d\n", szSection, nSDKRet);
			}
		}
	}
	else
	{
		MPU_ERR("%s: change lattice to bmp fail, return %d\n", szSection, nSDKRet);
		nRet = IDVR_CIU_CALLSDK_FAIL;

		//设置失败，恢复备份位图
		rename(szBakFileName, szFileName);
	}

	return nRet;
}
#endif
//获取各通道OSD，需外部将缓冲区清零
static int GetPicChannel_Alias_1(int nCh, LPHY_DVR_ALIAS_CFG_1 pcfg)
{
	int nRet;

	char szSection[30] = {0};

	char szDefaultName[30] = {0};

	snprintf(szSection, sizeof(szSection), SECTION_OSD_ALIAS, nCh);

	snprintf(szDefaultName, sizeof(szDefaultName), DEFNAME_OSD_ALIAS, nCh + 1);

	nRet = MPU_PUB_GetAlias(szSection, szDefaultName, pcfg);

	return nRet;
}

// 设置各个通道的编码类型H264或者H265
static int SetAdvanceCodecParam(int nCh, HY_DVR_ADVANCED_CHANNEL_CFG *pcfg)
{
	int nRet = IDVR_OK;

    AdvanceEncCfg stuAdvEncCfg;
    
	///////////////////////////////////////////////////////////////////////////////
	//本结构体是否有效，=0表示无效
	if (0 == pcfg->bValid)
	{
		MPU_DBG("0 == pcfg->bValid, nCh = %d\n", nCh);
		return IDVR_CIU_STRUCT_UNVALID;
	}
 
 	MPU_DBG("Set pcfg->nVideoCodecModeNormal[nCh]=%d , pcfg->nVideoCodecModeEvent[nCh]=%d , pcfg->nVideoCodecModeNet[nCh]=%d, pcfg->nVideoCodecModeVGANet[nCh]=%d\n",
		 pcfg->nVideoCodecModeNormal[nCh],pcfg->nVideoCodecModeEvent[nCh], pcfg->nVideoCodecModeNet[nCh], pcfg->nVideoCodecModeVGANet[nCh]);

	if(pcfg->nVideoCodecModeNormal[nCh] == 0 || pcfg->nVideoCodecModeEvent[nCh] == 0 || pcfg->nVideoCodecModeNet[nCh] == 0 || pcfg->nVideoCodecModeVGANet[nCh] == 0)
	{
		printf("no suport H264");
		return IDVR_CIU_STRUCT_UNSUPPORTED;
	}
 
    stuAdvEncCfg.nVideoCodecModeNormal[nCh] = pcfg->nVideoCodecModeNormal[nCh];
    stuAdvEncCfg.nVideoCodecModeEvent[nCh]  = pcfg->nVideoCodecModeEvent[nCh];
    stuAdvEncCfg.nVideoCodecModeNet[nCh]    = pcfg->nVideoCodecModeNet[nCh];
    stuAdvEncCfg.nVideoCodecModeVGANet[nCh] = pcfg->nVideoCodecModeVGANet[nCh];

    memcpy(&stuAdvEncCfg.struShelter[nCh], &pcfg->struShelter[nCh], sizeof(VIDEO_SHELTER));
    //stuAdvEncCfg.struShelter[nCh]           = pcfg->struShelter[nCh];

    /* 设置普通模式、事件模式下的高级编码参数和视频遮挡相关的参数 */
    nRet = MPU_AVL_SetAdvanceEncParam(nCh, stuAdvEncCfg.nVideoCodecModeNormal[nCh], stuAdvEncCfg.nVideoCodecModeEvent[nCh], &stuAdvEncCfg.struShelter[nCh]);
    MPU_CHK(0 == nRet);

	//Onvif过审需要子码流支持 MJPEG
#if 0
	//ODM设置MJPEG会崩溃，所以当子码流或者第二子码流得编码格式为MJPEG则返回
	if (2 == stuAdvEncCfg.nVideoCodecModeNet[nCh] || 2 == stuAdvEncCfg.nVideoCodecModeVGANet[nCh])
	{
		return nRet;
	}
#endif
	
    /* 设置子码流，高清子码流下的高级编码参数 */
    nRet = MPU_AVL_SetAuxHDAdvEncParam(nCh, stuAdvEncCfg.nVideoCodecModeNet[nCh], stuAdvEncCfg.nVideoCodecModeVGANet[nCh]);
    MPU_CHK(0 == nRet);
    
	return nRet;
}

// 获取各个通道的编码类型H264或者H265
static int GetAdvanceCodecParam(int nCh, HY_DVR_ADVANCED_CHANNEL_CFG *pcfg)
{
	int nRet = IDVR_OK;
    AdvanceEncCfg stuAdvEncCfg;
    
    pcfg->bValid = 1;

    nRet = MPU_AVL_GetAdvanceEncParam(nCh, &(stuAdvEncCfg.nVideoCodecModeNormal[nCh]), &(stuAdvEncCfg.nVideoCodecModeEvent[nCh]), &(stuAdvEncCfg.struShelter[nCh]));
    if (nRet)
    {
        MPU_ERR("Fail to called MPU_AVL_GetAdvanceEncParam, return %#x\n", nRet);
        return nRet;
    }
    nRet = MPU_AVL_GetAuxHDAdvEncParam(nCh, &stuAdvEncCfg.nVideoCodecModeNet[nCh], &stuAdvEncCfg.nVideoCodecModeVGANet[nCh]);
    if (nRet)
    {
        MPU_ERR("Fail to called MPU_AVL_GetAuxHDAdvEncParam, return %#x\n", nRet);
        return nRet;
    }

    pcfg->nVideoCodecModeNormal[nCh] = stuAdvEncCfg.nVideoCodecModeNormal[nCh];
    pcfg->nVideoCodecModeEvent[nCh] = stuAdvEncCfg.nVideoCodecModeEvent[nCh];

    memcpy(&pcfg->struShelter[nCh], &stuAdvEncCfg.struShelter[nCh], sizeof(VIDEO_SHELTER));
    
    //pcfg->struShelter[nCh] = stuAdvEncCfg.struShelter[nCh];
    
    pcfg->nVideoCodecModeNet[nCh] = stuAdvEncCfg.nVideoCodecModeNet[nCh];
    pcfg->nVideoCodecModeVGANet[nCh] = stuAdvEncCfg.nVideoCodecModeVGANet[nCh];

    MPU_DBG("Get pcfg->nVideoCodecModeNormal[0]=%d, pcfg->nVideoCodecModeNet[0]=%d, pcfg->nVideoCodecModeVGANet[0]=%d\n",
             pcfg->nVideoCodecModeNormal[0], pcfg->nVideoCodecModeNet[0], pcfg->nVideoCodecModeVGANet[0]);
    
	return nRet;
}

static int GetMotionSenstiveLevelCount(int nCh, LPHY_DVR_MOTION_EX_LIST pcfg)
{
	int nSDKRet = 0;
	int nDay = 0;
	int i = 0;
	int num = 0;
	AudioAlarmSchedule sched = {0};
	
	if(0 == nCh)
	{
		pcfg->struMotionEx[nCh].bValid = 1;
		pcfg->struMotionEx[nCh].nMotionSenstiveCap = 
			HY_DVR_MOTION_SENSTIVE_HIGHEST | HY_DVR_MOTION_SENSTIVE_HIGH | HY_DVR_MOTION_SENSTIVE_MIDDLE
			| HY_DVR_MOTION_SENSTIVE_LOW | HY_DVR_MOTION_SENSTIVE_LOWEST;

		pcfg->struMotionEx[nCh].nPedestrianfilteringValid = 1;
		pcfg->struMotionEx[nCh].nEnablePedestrianfiltering = MPU_AVL_PedestrianFiltering_IsEnable(nCh);
	}
	else
	{
		pcfg->struMotionEx[nCh].bValid = 0;
	}

	//获取移动侦测声音报警布防计划时间表
	nSDKRet = MPU_AVL_Motion_GetAudioAlarmSchedule(nCh, &sched);
	if (nSDKRet)
	{
		MPU_ERR("MPU_AVL_Motion_GetAudioAlarmSchedule failed, return %d\n", nSDKRet);
	}
	
	pcfg->struMotionEx[nCh].stVoiceAlarmSchedTime.bValid = 1;
	
	num = DVR_MAX_TIMESEGMENT / HY_DVR_MOTIONSEG_DAY;

	//时间表需要扩展
	for (nDay = 0; nDay < DVR_MAX_DAYS; ++nDay)
	{
		for (i = 0; i < HY_DVR_ALARMOUTSEG_DAY; ++i)
		{
			pcfg->struMotionEx[nCh].stVoiceAlarmSchedTime.bySetType[nDay][i * num] = sched.dwTimeSeg[nDay][i];
		}
	}

	//获取移动侦测声音报警配置
	nSDKRet = MPU_AVL_Motion_GetAudioAlarmCfg(nCh, &pcfg->struMotionEx[nCh].iVoiceType, &pcfg->struMotionEx[nCh].iBroadcastCount);
	if (nSDKRet)
	{
		MPU_ERR("MPU_AVL_Motion_GetAudioAlarmCfg failed, return %d\n", nSDKRet);
	}

	//! 状态,0为禁用(全天布防),1为启用,2为不支持
	pcfg->struMotionEx[nCh].stVoiceAlarmSchedTime.byState = sched.bytState;

	//! 状态,0为夜晚布防,1为白天布防,2为不支持
	pcfg->struMotionEx[nCh].iDayMode = sched.nDayMode;

	MPU_DBG("[%s] VoiceType=%d, BroadcastCount=%d, SchedTime.bValid=%d, byState=%d, iDayMode=%d\n", __FUNCTION__, \
		pcfg->struMotionEx[nCh].iVoiceType, pcfg->struMotionEx[nCh].iBroadcastCount, \
		pcfg->struMotionEx[nCh].stVoiceAlarmSchedTime.byState, pcfg->struMotionEx[nCh].iDayMode);

	return 0;
}

//设置各通道OSD
static int SetPicChannel_Alias_1(int nCh, LPHY_DVR_ALIAS_CFG_1 pcfg)
{
	int nRet = 0, nSDKRet = 0;
	char szSection[30] = {0};
	char szFileName[64] = {0};
	char szBakFileName[72] = {0};
	snprintf(szSection, sizeof(szSection), SECTION_OSD_ALIAS, nCh);

	///////////////////////////////////////////////////////////////////////////////
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

	if (0 == pcfg->strName[0])
	{
		MPU_ERR("%s: is NULL\n", szSection);
		return IDVR_CIU_PARAM_OVERFLOW;
	}

	MPU_DBG("%s: nWidth %d, nHeight %d\n", szSection, pcfg->nWidth, pcfg->nHeight);

	//需要保存的位图文件名
	snprintf(szFileName, sizeof(szFileName), IDVR_OSD_ALIAS, nCh + 1);

	//备份位图文件名
	snprintf(szBakFileName, sizeof(szBakFileName), "%s.bak", szFileName);

	//先备份原有位图
	rename(szFileName, szBakFileName);

	//生成新的位图
	nSDKRet = lattice_saveas_bmp24(pcfg->nWidth, pcfg->nHeight, pcfg->byLattice, szFileName, 1);
	if (0 == nSDKRet)
	{
		#ifdef NO_USE_BITMAP//用freetype库的话要先将osd配置到文件
		nSDKRet = MPU_PUB_SetAlias(szSection, pcfg);
		MPU_CHK(0 == nSDKRet);
		#endif
		//设置点阵到系统sdk
		nSDKRet = MPU_AVL_OSD_SetCharBitMap(nCh, szFileName, pcfg->byLattice,pcfg->struFontInfo.nFontSize, pcfg->nWidth, pcfg->nHeight);
		if (0 == nSDKRet)
		{
			#ifndef NO_USE_BITMAP
			//保存到配置文件
			nSDKRet = MPU_PUB_SetAlias(szSection, pcfg);
			MPU_CHK(0 == nSDKRet);
			#endif
			//通知GUI通道别名变更
			CIU_NotifyRemoteControl(0, eRemoteSetVideoInAlias, nCh);

			nRet = IDVR_OK;

			//设置成功，删除备份位图
			remove(szBakFileName);
		}
		else
		{
			MPU_ERR("%s: set bmp to dsp fail, return %d\n", szSection, nSDKRet);
			nRet = IDVR_CIU_CALLSDK_FAIL;

			//设置失败，恢复备份位图
			rename(szBakFileName, szFileName);

			//恢复位图到DSP
			nSDKRet = MPU_AVL_OSD_SetCharBitMap(nCh, szFileName, pcfg->byLattice,pcfg->struFontInfo.nFontSize, pcfg->nWidth, pcfg->nHeight);
			if (0 != nSDKRet)
			{
				MPU_ERR("%s: restore bmp to dsp fail, return %d\n", szSection, nSDKRet);
			}
		}
	}
	else
	{
		MPU_ERR("%s: change lattice to bmp fail, return %d\n", szSection, nSDKRet);
		nRet = IDVR_CIU_CALLSDK_FAIL;

		//设置失败，恢复备份位图
		rename(szBakFileName, szFileName);
	}

	return nRet;
}

//获取各通道各时间段图像参数，需外部将缓冲区清零
static int GetPicChannel_ChromaSec(int nCh, int nTimeSec, LPHY_DVR_CHROMA_SEC pcfg)
{
	int nRet = IDVR_OK;

	//MPU_DBG("nCh = %d, nTimeSec = %d\n", nCh, nTimeSec);  

	///////////////////////////////////////////////////////////////////////////////
	//本结构体是否有效，=0表示无效
	pcfg->bValid = 1;

	/*启用时间片 */
	pcfg->bTimeSegEnable = 1;

	//亮度、色度、对比度、饱和度
	pcfg->nBrightness = MPU_AVL_Pic_GetPicParam(nCh, nTimeSec, 0);
	pcfg->nHue = MPU_AVL_Pic_GetPicParam(nCh, nTimeSec, 1);
	pcfg->nContrast = MPU_AVL_Pic_GetPicParam(nCh, nTimeSec, 2);
	pcfg->nSaturation = MPU_AVL_Pic_GetPicParam(nCh, nTimeSec, 3);

	//时间段开始和结束时间
	MPU_AVL_Pic_GetTimeCfg(nCh, nTimeSec, &pcfg->nStartTime, &pcfg->nStopTime);

	return nRet;
}

//设置各通道各时间段图像参数
static int SetPicChannel_ChromaSec(int nCh, int nTimeSec, LPHY_DVR_CHROMA_SEC pcfg)
{
	int nRet = IDVR_OK, nSDKRet;

	///////////////////////////////////////////////////////////////////////////////
	//本结构体是否有效，=0表示无效
	if (0 == pcfg->bValid)
	{
		MPU_DBG("0 == pcfg->bValid, nCh = %d, nTimeSec = %d\n", nCh, nTimeSec);
		return IDVR_CIU_STRUCT_UNVALID;
	}

	if (pcfg->bTimeSegEnable)
	{
		//亮度、色度、对比度、饱和度
		nSDKRet = MPU_AVL_Pic_SetPicParam(nCh, nTimeSec, 0, pcfg->nBrightness);
		MPU_CHK(0 == nSDKRet);

		nSDKRet = MPU_AVL_Pic_SetPicParam(nCh, nTimeSec, 1, pcfg->nHue);
		MPU_CHK(0 == nSDKRet);

		nSDKRet = MPU_AVL_Pic_SetPicParam(nCh, nTimeSec, 2, pcfg->nContrast);
		MPU_CHK(0 == nSDKRet);

		nSDKRet = MPU_AVL_Pic_SetPicParam(nCh, nTimeSec, 3, pcfg->nSaturation);
		MPU_CHK(0 == nSDKRet);

		//时间段开始和结束时间
		nSDKRet = MPU_AVL_Pic_SetTimeCfg(nCh, nTimeSec, pcfg->nStartTime, pcfg->nStopTime);
		MPU_CHK(0 == nSDKRet);

		MPU_DBG("nCh = %d, nTimeSec = %d, nBrightness = %d, nStartTime= %d, nStopTime = %d\n", nCh, nTimeSec, pcfg->nBrightness, pcfg->nStartTime,
				pcfg->nStopTime);
	}

	return nRet;
}

//获取各通道图像参数，需外部将缓冲区清零
static int GetPicChannel_Chroma(int nCh, LPHY_DVR_CHROMA pcfg)
{
	int nRet = IDVR_OK, i;

	///////////////////////////////////////////////////////////////////////////////
	//本结构体是否有效，=0表示无效
	pcfg->bValid = 1;

	//IDVR只支持4个时间端
	for (i = 0; i < 4; ++i)
	{
		GetPicChannel_ChromaSec(nCh, i, &pcfg->struChromaSec[i]);
	}

	return nRet;
}

//设置各通道图像参数
static int SetPicChannel_Chroma(DWORD dwUserID, int nCh, LPHY_DVR_CHROMA pcfg)
{
	int nRet, nSDKRet, i, bChange = FALSE;

	///////////////////////////////////////////////////////////////////////////////
	//本结构体是否有效，=0表示无效
	if (0 == pcfg->bValid)
	{
		MPU_DBG("0 == pcfg->bValid, nCh = %d\n", nCh);
		return IDVR_CIU_STRUCT_UNVALID;
	}

	//默认返回设置成功
	nRet = IDVR_OK;

	//IDVR只支持4个时间端
	for (i = 0; i < 4; ++i)
	{
		nSDKRet = SetPicChannel_ChromaSec(nCh, i, &pcfg->struChromaSec[i]);
		CIU_ERR(nSDKRet, nRet);

		if (IDVR_OK == nSDKRet)
		{
			bChange = TRUE;
		}
	}

	if (bChange)
	{
		//记录日志，设置视频参数
		CIU_OPL_AddLog(dwUserID, EDVR_EVENT_SETTING, EDVR_EVENT_SETTING_DETAIL_PICTURE_SETTING, nCh, 0);
	}

	return nRet;
}

//获取各通道Motion，需外部将缓冲区清零
static int GetPicChannel_Motion(int nCh, LPHY_DVR_MOTION pcfg)
{
	int nRet = IDVR_OK, i;

	int nCount;

	unsigned char byMagicBlock[16 * 12];

	///////////////////////////////////////////////////////////////////////////////
	//本结构体是否有效，=0表示无效
	pcfg->bValid = 1;

	/* 是否处理移动侦测 */
	pcfg->bEnable = MPU_AVL_Motion_IsEnable(nCh);

	/*移动侦测灵敏度, 0 - 5,越高越灵敏,0xff关闭 */
	pcfg->nSenstive = MPU_AVL_Motion_GetDetectPrecision(nCh);

	/*侦测区域,共有22*18个小宏块,为1表示改宏块是移动侦测区域,0-表示不是 */
	pcfg->nXScope = 16;
	pcfg->nYScope = 12;
	nCount = MPU_AVL_Motion_GetEnableMagicBlock(nCh, byMagicBlock, 16 * 12);
	for (i = 0; i < nCount; ++i)
	{
		int y = byMagicBlock[i] / 16;

		int x = byMagicBlock[i] % 16;

		pcfg->byMotionScope[y][x] = 1;
	}

	/* 处理方式 */
	GetHandleConfig_Motion(nCh, &pcfg->struHandleType);

	/* 0表示不布防，1表示布防 */
	GetSchedule_Motion(nCh, &pcfg->struSchedTime);

	return nRet;
}

//设置各通道Motion
static int SetPicChannel_Motion(DWORD dwUserID, int nCh, LPHY_DVR_MOTION pcfg)
{
	int nRet, nSDKRet, x, y, nCount, bChange = FALSE;

    int bEnable   = 0;
    int nSenstive = 0;
	int old_count = 0;
	unsigned char          byMotionScope[16 * 12] = {0};	
	HY_DVR_SCHEDTIME       struSchedTime;
 
    printf("===SetPicChannel_Motion: dwUserID=%lu nCh = %d===\n", dwUserID, nCh);

	unsigned char byMagicBlock[16 * 12] = {0};	

	///////////////////////////////////////////////////////////////////////////////
	//本结构体是否有效，=0表示无效
	if (0 == pcfg->bValid)
	{
		MPU_DBG("0 == pcfg->bValid, nCh = %d\n", nCh);
		return IDVR_CIU_STRUCT_UNVALID;
	}

	//默认返回设置成功
	nRet = IDVR_OK;
    MPU_DBG("--SetPicChannel_Motion--nCh=%d--bEnable=%d--nSenstive=%d--nXScope=%d--nYScope=%d--\n",nCh,pcfg->bEnable,
	pcfg->nSenstive, pcfg->nXScope, pcfg->nYScope);

	/* 是否处理移动侦测 */
    bEnable = MPU_AVL_Motion_IsEnable(nCh);
	if (bEnable != pcfg->bEnable)
	{
    	nSDKRet = MPU_AVL_Motion_Enable(nCh, pcfg->bEnable);
    	if (0 == nSDKRet)
    	{
    		bChange = TRUE;
    	}
    	else
    	{
    		MPU_ERR("Set Ch %d motion enable %d fail, return %d\n", nCh, pcfg->bEnable, nSDKRet);
    		nRet = IDVR_CIU_CALLSDK_FAIL;
    	}
	}
	else
	{
        printf("===[Motion] bEnable no changed!===\n");
	}

	/*移动侦测灵敏度, 0 - 5,越高越灵敏,0xff关闭 */
    nSenstive = MPU_AVL_Motion_GetDetectPrecision(nCh);
	if (nSenstive != pcfg->nSenstive)
	{
    	nSDKRet = MPU_AVL_Motion_SetDetectPrecision(nCh, pcfg->nSenstive);
    	if (0 == nSDKRet)
    	{
    		bChange = TRUE;
    	}
    	else
    	{
    		MPU_ERR("Set Ch %d motion precision %d fail, return %d\n", nCh, pcfg->nSenstive, nSDKRet);
    		nRet = IDVR_CIU_CALLSDK_FAIL;
    	}
    }
    else
    {
        printf("===[Micro_Motion] nSenstive no changed!===\n");
    }

#if 1
	/*侦测区域,共有16*12个小宏块,为1表示改宏块是移动侦测区域,0-表示不是 */
	if (pcfg->nXScope > 16)
	{
		MPU_ERR("nXScope = %d is overflow\n", pcfg->nXScope);
		pcfg->nXScope = 16;
	}
	if (pcfg->nYScope > 12)
	{
		MPU_ERR("nYScope = %d is overflow\n", pcfg->nYScope);
		pcfg->nYScope = 12;
	}
	nCount = 0;

	for (y = 0; y < pcfg->nYScope; ++y)
	{
		for (x = 0; x < pcfg->nXScope; ++x)
		{
			if (pcfg->byMotionScope[y][x]) // 如果该区域被标记了，是移动侦测区域
			{
			    // 将序列号放到数组中，数组中存放的是已经标记了的宏块的序列号
				byMagicBlock[nCount++] = y * pcfg->nXScope + x;  
			}
		}
	}

	//设置范围
	old_count = MPU_AVL_Motion_GetEnableMagicBlock(nCh, byMotionScope, 16 * 12);
	if (old_count!=nCount || strncmp((const char*)byMotionScope, (const char*)byMagicBlock, 16*12))
	{
		bChange = TRUE;
	}
	else
	{
		printf("===[Micro_Motion] byMagicBlock no changed!===\n");
	}

	nSDKRet = MPU_AVL_Motion_EnableMagicBlock(nCh, byMagicBlock, nCount);
	if (nSDKRet)
	{
		MPU_ERR("Set Ch %d motion magic blocks fail, return %d\n", nCh, nSDKRet);
		nRet = IDVR_CIU_CALLSDK_FAIL;
	}
#endif
	
	/* 0表示不布防，1表示布防 */
    memset(&struSchedTime, 0, sizeof(HY_DVR_SCHEDTIME));
    GetSchedule_Motion(nCh, &struSchedTime);
    if (memcmp(&pcfg->struSchedTime, &struSchedTime, sizeof(HY_DVR_SCHEDTIME)))
    {
        nSDKRet = SetSchedule_Motion(nCh, &pcfg->struSchedTime);
        CIU_ERR(nSDKRet, nRet);
        if (0 == nSDKRet)
        {
            bChange = TRUE;
        }
    }
    else
    {
        printf("===[Micro_Motion] struSchedTime no changed!===\n");
    }
    
	if (bChange)
	{
		//记录日志，设置视频参数
		CIU_OPL_AddLog(dwUserID, EDVR_EVENT_SETTING, EDVR_EVENT_SETTING_DETAIL_MOTION_SETTING, nCh, 0);
	}	
	else
	{
        printf("===[Micro_Motion] all param no changed!===\n");
	}

	return nRet;
}

//设置各通道人形配置
static int SetPicChannel_HumanShape(DWORD dwUserID, int nCh, LPHY_DVR_PEOPLE_DETECT pcfg)
{
	int nRet, nSDKRet, bChange = FALSE;
    static int bMotionEnable = 0, bHumanEnable = 0, bLastTrackEnable = 0;
	HY_DVR_HANDLEEXCEPTION struHandleType;

    printf("===SetPicChannel_HumanShape: dwUserID=%lu nCh = %d===\n", dwUserID, nCh);
    
	///////////////////////////////////////////////////////////////////////////////
	//本结构体是否有效，=0表示无效
	if (0 == pcfg->bValid)
	{
		MPU_DBG("0 == pcfg->bValid, nCh = %d\n", nCh);
		return IDVR_CIU_STRUCT_UNVALID;
	}

	//默认返回设置成功
	nRet = IDVR_OK;
    MPU_DBG("--SetPicChannel_HumanShape--nCh=%d--bEnable=%d--nSenstive=%d------\n",nCh,pcfg->bEnable,pcfg->nSenstive);

    /*1.当打开人形跟踪开关时，默认打开人形侦测,关闭移动侦测功能
      2.当关闭人形跟踪开关时，恢复打开人形跟踪开关时人形侦测以及移动侦测的开关状态。
    */
    if(1 == pcfg->nValidTrack && 1 == pcfg->nEnableTrack && 0 == bLastTrackEnable)
    {
        pcfg->bEnable = 1;
        bLastTrackEnable = 1;
        bMotionEnable = MPU_AVL_Motion_IsEnable(nCh);
        bHumanEnable = MPU_AVL_HumanShape_IsEnable(nCh);
        //printf("[%s:%d] open TrackSwitch bMotinEnable %d, bHumanEnable %d\n", __FILE__, __LINE__, bMotionEnable, bHumanEnable);
        MPU_AVL_AreaDefense_Motion_Enable( nCh, 0);
    }
    else if (1 == pcfg->nValidTrack && 0 == pcfg->nEnableTrack && 1 == bLastTrackEnable)
    {
        pcfg->bEnable = bHumanEnable;
        bLastTrackEnable = 0;
        //printf("[%s:%d] close TrackSwitch bMotinEnable %d, bHumanEnable %d\n", __FILE__, __LINE__, bMotionEnable, bHumanEnable);
        MPU_AVL_AreaDefense_Motion_Enable( nCh, bMotionEnable);
    }

	/* 是否处理人形检测 */
	nSDKRet = MPU_AVL_HumanShape_Enable(nCh, pcfg->bEnable);
	if (0 == nSDKRet)
	{
		bChange = TRUE;
	}
	else
	{
		MPU_ERR("Set Ch %d human enable %d fail, return %d\n", nCh, pcfg->bEnable, nSDKRet);
		nRet = IDVR_CIU_CALLSDK_FAIL;
	}

	/* 主子码流叠加人形框处理 */
	nSDKRet = MPU_AVL_SetHumanOverlayInfo(nCh, pcfg->cMainStreamOverlay, pcfg->cSubStreamOverlay);
	if (0 == nSDKRet)
	{
		bChange = TRUE;
	}
	else
	{
		MPU_ERR("Set Ch %d human enable %d fail, return %d\n", nCh, pcfg->bEnable, nSDKRet);
		nRet = IDVR_CIU_CALLSDK_FAIL;
	}

	/*灵敏度, 0 - 5,越高越灵敏,0xff关闭 */
	nSDKRet = MPU_AVL_HumanShape_SetDetectPrecision(nCh, pcfg->nSenstive);
	if (0 == nSDKRet)
	{
		bChange = TRUE;
	}
	else
	{
		MPU_ERR("Set Ch %d human precision %d fail, return %d\n", nCh, pcfg->nSenstive, nSDKRet);
		nRet = IDVR_CIU_CALLSDK_FAIL;
	}

    /* 处理方式 */
    memset(&struHandleType, 0, sizeof(HY_DVR_HANDLEEXCEPTION));
    GetHandleConfig_HumanShape(nCh, &struHandleType);
    if (memcmp(&struHandleType, &pcfg->struHandleType, sizeof(HY_DVR_HANDLEEXCEPTION)))
    {
    	nSDKRet = SetHandleConfig_HumanShape(nCh, &pcfg->struHandleType);
    	CIU_ERR(nSDKRet, nRet);
		nSDKRet = SetHandleConfig_Motion(nCh, &pcfg->struHandleType);//这里处理方式同步给移动侦测
    	CIU_ERR(nSDKRet, nRet);
    	if (0 == nSDKRet)
    	{
    		bChange = TRUE;
    	}
    }
    else
    {
        printf("===[CGI_HumanShape] struHandleType no changed!===\n");
    }

	if (bChange)
	{
		//记录日志，设置视频参数
		//CIU_OPL_AddLog(dwUserID, EDVR_EVENT_SETTING, EDVR_EVENT_SETTING_DETAIL_MOTION_SETTING, nCh, 0);
	}
	else
	{
        printf("===[CGI_HumanShape] all param no changed!===\n");
	}

	return nRet;
}

//获取各通道人形配置
static int GetPicChannel_HumanShape(int nCh, LPHY_DVR_PEOPLE_DETECT pcfg)
{
	int nRet = IDVR_OK, i = 0;
	int nCount = 0;
	MotionRegion stuMotionRegion[DVR_MAX_MOTION_REGION_NUM] = {0};
	
	///////////////////////////////////////////////////////////////////////////////
	//本结构体是否有效，=0表示无效
	pcfg->bValid = 1;

	/* 是否处理移动侦测 */
	pcfg->bEnable = MPU_AVL_HumanShape_IsEnable(nCh);

	/* 主子码流叠加人形框处理 */
	MPU_AVL_GetHumanOverlayInfo(nCh, &pcfg->cMainStreamOverlay, &pcfg->cSubStreamOverlay);

	/*移动侦测灵敏度, 0 - 5,越高越灵敏,0xff关闭 */
	pcfg->nSenstive = MPU_AVL_HumanShape_GetDetectPrecision(nCh);
	
	/*侦测区域 */
	nCount = MPU_AVL_HumanShape_GetRegionCfg(nCh, stuMotionRegion, DVR_MAX_MOTION_REGION_NUM);
	if (nCount > DVR_MAX_MOTION_REGION_NUM)
	{
		nCount = DVR_MAX_MOTION_REGION_NUM;
		MPU_ERR("remote config CGI motion region %d, real %d\n", DVR_MAX_MOTION_REGION_NUM, nCount);
	}
	
	/* 获取移动侦测的区域 */
	for (i=0; i<DVR_MAX_MOTION_REGION_NUM; i++)
	{
		pcfg->struPeopDetectRegion[i].bValid = stuMotionRegion[i].bValid;
		pcfg->struPeopDetectRegion[i].nMotionAreaTopLeftX = stuMotionRegion[i].nMotionAreaTopLeftX;
		pcfg->struPeopDetectRegion[i].nMotionAreaTopLeftY = stuMotionRegion[i].nMotionAreaTopLeftY;
		pcfg->struPeopDetectRegion[i].nMotionAreaBottomRightX = stuMotionRegion[i].nMotionAreaBottomRightX;
		pcfg->struPeopDetectRegion[i].nMotionAreaBottomRightY = stuMotionRegion[i].nMotionAreaBottomRightY;
/*
		printf("Get[%d]:Valid=%d, TopLeftX=%d, TopLeftX=%d, BottomRightX=%d, BottomRightY=%d\n",
				i, pcfg->struMotionRegion[i].bValid, 
				pcfg->struPeopDetectRegion[i].nMotionAreaTopLeftX, 
				pcfg->struPeopDetectRegion[i].nMotionAreaTopLeftY,
				pcfg->struPeopDetectRegion[i].nMotionAreaBottomRightX, 
				pcfg->struPeopDetectRegion[i].nMotionAreaBottomRightY);
*/
	}
	
	/* 处理方式 */
	GetHandleConfig_HumanShape(nCh, &pcfg->struHandleType);

	/* 0表示不布防，1表示布防 */
	GetSchedule_HumanShape(nCh, &pcfg->struSchedTime);

    /*人形跟踪使能*/
    pcfg->nValidTrack = 1;

    return nRet;
}

static int SetPicChannel_Motion_Ext(int nCh, LPHY_DVR_MOTION_EX_LIST pcfg)
{
	int nRet, nSDKRet;
	
    int bEnable = 0;
	int nDay = 0;
	int i = 0;
	int num = 0;
	AudioAlarmSchedule sched = {0};

    printf("===SetPicChannel_Motion_Ext: nCh = %d===\n", nCh);

	///////////////////////////////////////////////////////////////////////////////
	//本结构体是否有效，=0表示无效
	if (0 == pcfg->bValid)
	{
		MPU_DBG("0 == pcfg->bValid, nCh = %d\n", nCh);
		return IDVR_CIU_STRUCT_UNVALID;
	}

	MPU_DBG("[%s] VoiceType=%d, BroadcastCount=%d, SchedTime.bValid=%d, byState=%d, iDayMode=%d\n", __FUNCTION__, \
		pcfg->struMotionEx[nCh].iVoiceType, pcfg->struMotionEx[nCh].iBroadcastCount, \
		pcfg->struMotionEx[nCh].stVoiceAlarmSchedTime.byState, pcfg->struMotionEx[nCh].iDayMode);

	//默认返回设置成功
	nRet = IDVR_OK;
    MPU_DBG("--SetPicChannel_Motion--nCh=%d----nEnablePedestrianfiltering=%d------\n",nCh,pcfg->struMotionEx[nCh].nEnablePedestrianfiltering);

    bEnable = MPU_AVL_PedestrianFiltering_IsEnable(nCh);
	if (bEnable != pcfg->struMotionEx[nCh].nEnablePedestrianfiltering)
	{
    	nSDKRet = MPU_AVL_PedestrianFiltering_Enable(nCh, pcfg->struMotionEx[nCh].nEnablePedestrianfiltering);
    	if (0 == nSDKRet)
    	{
    	}
    	else
    	{
    		MPU_ERR("Set Ch %d motion enable %d fail, return %d\n", nCh, pcfg->struMotionEx[nCh].nEnablePedestrianfiltering, nSDKRet);
    		nRet = IDVR_CIU_CALLSDK_FAIL;
    	}
	}

	num = DVR_MAX_TIMESEGMENT / HY_DVR_MOTIONSEG_DAY;

	//时间表需要扩展
	for (nDay = 0; nDay < DVR_MAX_DAYS; ++nDay)
	{
		for (i = 0; i < HY_DVR_ALARMOUTSEG_DAY; ++i)
		{
			sched.dwTimeSeg[nDay][i] = pcfg->struMotionEx[nCh].stVoiceAlarmSchedTime.bySetType[nDay][i * num];
		}
	}

	//! 状态,0为禁用(全天布防),1为启用,2为不支持
	sched.bytState = pcfg->struMotionEx[nCh].stVoiceAlarmSchedTime.byState;

	//! 状态,0为夜晚布防,1为白天布防,2为不支持
	sched.nDayMode = pcfg->struMotionEx[nCh].iDayMode;

	//设置移动侦测的声音报警布防时间计划
	nSDKRet = MPU_AVL_Motion_SetAudioAlarmSchedule(nCh, &sched);
	if (nSDKRet)
	{
		MPU_ERR("MPU_AVL_Motion_SetAudioAlarmSchedule failed, return %d\n", nSDKRet);
		nRet = IDVR_CIU_CALLSDK_FAIL;
	}
	
	//设置移动侦测声音报警配置
	nSDKRet = MPU_AVL_Motion_SetAudioAlarmCfg(nCh, pcfg->struMotionEx[nCh].iVoiceType, pcfg->struMotionEx[nCh].iBroadcastCount);
	if (nSDKRet)
	{
		MPU_ERR("MPU_AVL_Motion_SetAudioAlarmCfg failed, return %d\n", nSDKRet);
		nRet = IDVR_CIU_CALLSDK_FAIL;
	}

	return nRet;
}

//获取气体检测OSD，需外部将缓冲区清零
static int GetPicChannel_AirOsd(int nCh, LPHY_DVR_OSD_CFG_EX_LIST pcfg)
{
	return IDVR_OK;
}

static int SetPicChannel_AirOsd(int nCh, LPHY_DVR_OSD_CFG_EX_LIST pcfg)
{    
	return IDVR_OK;
}

//获取图像配置，需外部将缓冲区清零
static int GetPicCfg(LPHY_DVR_PIC_CFG pcfg)
{
	int nRet = IDVR_OK, nCh, nCount;

	///////////////////////////////////////////////////////////////////////////////
	//本结构体是否有效，=0表示无效
	pcfg->bValid = 1;
	//MPU_DBG("----GetPicCfg--ChannelCount %d--!\n", MAX_ENC_CHANNEL_NUM);
	///////////////////////////////////////////////////////////////////////////////
	//获取工作模式
	GetDevCustom_Pic(&pcfg->struWorkMode);


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
		GetPicChannel_Osd(nCh, &pcfg->struOsd[nCh]);

		//通道名
//		GetPicChannel_Alias(nCh, &pcfg->struChName[nCh]);

		//图像参数
		GetPicChannel_Chroma(nCh, &pcfg->struChroma[nCh]);

		//移动侦测 
		GetPicChannel_Motion(nCh, &pcfg->struMotion[nCh]);

		//通道名(unicode)
		GetPicChannel_Alias_1(nCh, &pcfg->struChName_1[nCh]);
	}
	//MPU_DBG("----GetPicCfg--return %d--!\n",nRet);

	return nRet;
}

//设置图像配置
static int SetPicCfg(DWORD dwUserID, LPHY_DVR_PIC_CFG pcfg)
{
	int nRet = IDVR_OK, nCh, nCount, nSDKRet;

	///////////////////////////////////////////////////////////////////////////////
	//本结构体是否有效，=0表示无效
	if (0 == pcfg->bValid)
	{
		MPU_DBG("0 == pcfg->bValid\n");
		return IDVR_CIU_STRUCT_UNVALID;
	}

	///////////////////////////////////////////////////////////////////////////////
	//设置工作模式
	//SetDevCustom_Pic(&pcfg->struWorkMode);


	///////////////////////////////////////////////////////////////////////////////
	//获取录像通道总数
	nCount = MAX_ENC_CHANNEL_NUM;

	//设置各通道图像参数
	for (nCh = 0; nCh < nCount; ++nCh)
	{
		//显示OSD
		nSDKRet = SetPicChannel_Osd(nCh, &pcfg->struOsd[nCh]);
		CIU_ERR(nSDKRet, nRet);

		//通道名
//		nSDKRet = SetPicChannel_Alias(nCh, &pcfg->struChName[nCh]);
//		CIU_ERR(nSDKRet, nRet);

		//图像参数
		nSDKRet = SetPicChannel_Chroma(dwUserID, nCh, &pcfg->struChroma[nCh]);
		CIU_ERR(nSDKRet, nRet);

		//移动侦测 
		nSDKRet = SetPicChannel_Motion(dwUserID, nCh, &pcfg->struMotion[nCh]);
		CIU_ERR(nSDKRet, nRet);

		//通道名(unicode)
		nSDKRet = SetPicChannel_Alias_1(nCh, &pcfg->struChName_1[nCh]);
		CIU_ERR(nSDKRet, nRet);
	}

	return nRet;
}

int CIU_GetConfigure_Pic(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen)
{
	int nRet = IDVR_OK;

	MPU_DBG("ID %d, Major %d, Minor %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType);
	//printf("CIU_GetConfigure_Pic ID %d, Major %d, Minor %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType);

	switch (dwMinorType)
	{
			//设置图像参数
		case PICCFG_ALL:
			{
				///////////////////////////////////////////////////////////////////////////////
				//判断buf空间是否足够
				if (*pdwBufLen < PIC_CFG_LEN)
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", PIC_CFG_LEN, *pdwBufLen);
					break;
				}

				LPHY_DVR_PIC_CFG pcfg = (LPHY_DVR_PIC_CFG) pBuffer;

				///////////////////////////////////////////////////////////////////////////////
				//初始化输出缓冲区
				memset(pcfg, 0, PIC_CFG_LEN);
				*pdwBufLen = PIC_CFG_LEN;

				nRet = GetPicCfg(pcfg);
			}
			break;

			//通道名
		case PICCFG_CHNAME_CONF:
			{
				///////////////////////////////////////////////////////////////////////////////
				//判断buf空间是否足够
				if (*pdwBufLen < PIC_CFG_LEN)
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", PIC_CFG_LEN, *pdwBufLen);
					break;
				}

				LPHY_DVR_PIC_CFG pcfg = (LPHY_DVR_PIC_CFG) pBuffer;

				///////////////////////////////////////////////////////////////////////////////
				//初始化输出缓冲区
				memset(pcfg, 0, PIC_CFG_LEN);
				*pdwBufLen = PIC_CFG_LEN;

				pcfg->bValid = 1;

				int nCount;

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
				/*for (nCh = 0; nCh < nCount; ++nCh)
				{
					//通道名
					GetPicChannel_Alias(nCh, &pcfg->struChName[nCh]);
				}*/
			}
			break;

			//通道名(unicode)
		case PICCFG_CHNAME_UNICODE_CONF:
			{
				///////////////////////////////////////////////////////////////////////////////
				//判断buf空间是否足够
				if (*pdwBufLen < PIC_CFG_LEN)
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", PIC_CFG_LEN, *pdwBufLen);
					break;
				}

				LPHY_DVR_PIC_CFG pcfg = (LPHY_DVR_PIC_CFG) pBuffer;

				///////////////////////////////////////////////////////////////////////////////
				//初始化输出缓冲区
				memset(pcfg, 0, PIC_CFG_LEN);
				*pdwBufLen = PIC_CFG_LEN;

				pcfg->bValid = 1;

				int nCh, nCount;

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
					//通道名(unicode)
					GetPicChannel_Alias_1(nCh, &pcfg->struChName_1[nCh]);
				}
			}
			break;
			
        // 移动侦测坐标版
        case  PICCFG_MOTION_ACTION_CONF:			
		case PICCFG_MOTION_V2_CONF:
            {
				#if 0
                ///////////////////////////////////////////////////////////////////////////////
                //判断buf空间是否足够
                if (*pdwBufLen < sizeof(HY_DVR_MOTION_V2_LIST))
                {
                    nRet = IDVR_CIU_BUF_SMALL;
                    MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_MOTION_V2_LIST), *pdwBufLen);
                    break;
                }
                
                LPHY_DVR_MOTION_V2_LIST pcfg = (LPHY_DVR_MOTION_V2_LIST)pBuffer;
                
                ///////////////////////////////////////////////////////////////////////////////
                //初始化输出缓冲区
                memset(pcfg, 0, sizeof(HY_DVR_MOTION_V2_LIST));
                *pdwBufLen = sizeof(HY_DVR_MOTION_V2_LIST);
                
                pcfg->bValid = 1;
                GetPicChannel_Motion_V2(0, &(pcfg->struMotionV2[0]));
				#endif
            }
            break;

		case PICCFG_ADVANCED_CONF:
			{
				///////////////////////////////////////////////////////////////////////////////
				//判断buf空间是否足够
				if (*pdwBufLen < sizeof(HY_DVR_ADVANCED_CHANNEL_CFG))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_ADVANCED_CHANNEL_CFG), *pdwBufLen);
					break;
				}

				HY_DVR_ADVANCED_CHANNEL_CFG *pcfg = (HY_DVR_ADVANCED_CHANNEL_CFG *) pBuffer;

				//初始化输出缓冲区
				memset(pcfg, 0, sizeof(HY_DVR_ADVANCED_CHANNEL_CFG));
				*pdwBufLen = sizeof(HY_DVR_ADVANCED_CHANNEL_CFG);

				pcfg->bValid = 1;
					
				int nCh, nCount, nSDKRet;

				nCount = MAX_ENC_CHANNEL_NUM;
				if (nCount > DVR_MAX_VIDEOIN_NUM)
				{
					MPU_ERR("remote config picture %d, real %d\n", DVR_MAX_VIDEOIN_NUM, nCount);
					nCount = DVR_MAX_VIDEOIN_NUM;
				}
				//设置各个通道的高级编码参数(H.264或者H.265)
				for (nCh = 0; nCh < nCount; ++nCh)
				{
					nSDKRet = GetAdvanceCodecParam(nCh, pcfg);
					CIU_ERR(nSDKRet, nRet);
				}
			}
			break;
			
		case PICCFG_MOTION_EX_CONF:
			{
                ///////////////////////////////////////////////////////////////////////////////
                //判断buf空间是否足够
                if (*pdwBufLen < sizeof(HY_DVR_MOTION_EX_LIST))
                {
                    nRet = IDVR_CIU_BUF_SMALL;
                    MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_MOTION_EX_LIST), *pdwBufLen);
                    break;
                }
                
                LPHY_DVR_MOTION_EX_LIST pcfg = (LPHY_DVR_MOTION_EX_LIST)pBuffer;
                
                ///////////////////////////////////////////////////////////////////////////////
                //初始化输出缓冲区
                memset(pcfg, 0, sizeof(HY_DVR_MOTION_EX_LIST));
                *pdwBufLen = sizeof(HY_DVR_MOTION_EX_LIST);
                
                pcfg->bValid = 1;
                int nCh = 0, nSDKRet = 0;
				//获取各个通道移动侦测的灵敏度等级个数
				for (nCh = 0; nCh < DVR_MAX_VIDEOIN_NUM; ++nCh)
				{
					nSDKRet = GetMotionSenstiveLevelCount(nCh, pcfg);
					CIU_ERR(nSDKRet, nRet);
				}
            }
			break;
			
		case PICCFG_PEOPLE_DETECT_CFG:
			{
				///////////////////////////////////////////////////////////////////////////////
                //判断buf空间是否足够
                if (*pdwBufLen < sizeof(HY_DVR_PEOPLE_DETECT_CFG))
                {
                    nRet = IDVR_CIU_BUF_SMALL;
                    MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_PEOPLE_DETECT_CFG), *pdwBufLen);
                    break;
                }
                
                LPHY_DVR_PEOPLE_DETECT_CFG pcfg = (LPHY_DVR_PEOPLE_DETECT_CFG)pBuffer;
                
                ///////////////////////////////////////////////////////////////////////////////
                //初始化输出缓冲区
                memset(pcfg, 0, sizeof(HY_DVR_PEOPLE_DETECT_CFG));
                *pdwBufLen = sizeof(HY_DVR_PEOPLE_DETECT_CFG);
                
                pcfg->bValid = 1;
                int nSDKRet = 0;
				nSDKRet = GetPicChannel_HumanShape(0, &(pcfg->struPeopleDetect[0]));
				CIU_ERR(nSDKRet, nRet);
			}
            break;
            case  PICCFG_OSD_CONF_EX:
                {
                ///////////////////////////////////////////////////////////////////////////////
                //判断buf空间是否足够
                if (*pdwBufLen < sizeof(HY_DVR_OSD_CFG_EX_LIST))
                {
                    nRet = IDVR_CIU_BUF_SMALL;
                    MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_OSD_CFG_EX_LIST), *pdwBufLen);
                    break;
                }
                
                LPHY_DVR_OSD_CFG_EX_LIST pcfg = (LPHY_DVR_OSD_CFG_EX_LIST)pBuffer;
                
                ///////////////////////////////////////////////////////////////////////////////
                //初始化输出缓冲区
                memset(pcfg, 0, sizeof(HY_DVR_OSD_CFG_EX_LIST));
                *pdwBufLen = sizeof(HY_DVR_OSD_CFG_EX_LIST);
                nRet = GetPicChannel_AirOsd(0, pcfg);
            }
			break;
			
#if defined (__IS_SUPPORT_LONGAWAYHOME__) 
			case PICCFG_LONG_AWAY_HOME_DETECT_CFG:
			{
				///////////////////////////////////////////////////////////////////////////////
                //判断buf空间是否足够
                if (*pdwBufLen < sizeof(HY_DVR_LONG_AWAY_HOME_DETECT_CFG))
                {
                    nRet = IDVR_CIU_BUF_SMALL;
                    MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_LONG_AWAY_HOME_DETECT_CFG), *pdwBufLen);
                    break;
                }
                
                LPHY_DVR_LONG_AWAY_HOME_DETECT_CFG pcfg = (LPHY_DVR_LONG_AWAY_HOME_DETECT_CFG)pBuffer;
                
                ///////////////////////////////////////////////////////////////////////////////
                //初始化输出缓冲区
                memset(pcfg, 0, sizeof(HY_DVR_LONG_AWAY_HOME_DETECT_CFG));
                *pdwBufLen = sizeof(HY_DVR_LONG_AWAY_HOME_DETECT_CFG);

                pcfg->iValid = 1;
                MPU_AVL_HumanShape_GetLongAwayEnable(0, &pcfg->iEnable);
                MPU_AVL_HumanShape_GetLongAwayDuration(0, &pcfg->iAwayHomeAlarmDuration);
			}
			break;
			
#endif
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

int CIU_SetConfigure_Pic(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, const BYTE * pBuffer, DWORD dwBufLen)
{
	int nRet = IDVR_OK;

	MPU_DBG("ID %d, Major %d, Minor %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType);

	//printf("CIU_SetConfigure_Pic ID %d, Major %d, Minor %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType);

	switch (dwMinorType)
	{
			//设置图像参数
		case PICCFG_ALL:
			{
				///////////////////////////////////////////////////////////////////////////////
				//判断buf空间是否足够
				if (dwBufLen < PIC_CFG_LEN)
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", PIC_CFG_LEN, dwBufLen);
					break;
				}

				LPHY_DVR_PIC_CFG pcfg = (LPHY_DVR_PIC_CFG) pBuffer;

				nRet = SetPicCfg(dwUserID, pcfg);
			}
			break;

			//通道名
		case PICCFG_CHNAME_CONF:
			{
				///////////////////////////////////////////////////////////////////////////////
				//判断buf空间是否足够
				if (dwBufLen < PIC_CFG_LEN)
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", PIC_CFG_LEN, dwBufLen);
					break;
				}

				LPHY_DVR_PIC_CFG pcfg = (LPHY_DVR_PIC_CFG) pBuffer;

				if (0 == pcfg->bValid)
				{
					MPU_DBG("0 == pcfg->bValid\n");
					nRet = IDVR_CIU_STRUCT_UNVALID;
				}
				else
				{
					int nCount;

					nCount = MAX_ENC_CHANNEL_NUM;
					if (nCount > DVR_MAX_VIDEOIN_NUM)
					{
						MPU_ERR("remote config picture %d, real %d\n", DVR_MAX_VIDEOIN_NUM, nCount);
						nCount = DVR_MAX_VIDEOIN_NUM;
					}
					//设置各通道图像参数
					/*for (nCh = 0; nCh < nCount; ++nCh)
					{
						//通道名
						nSDKRet = SetPicChannel_Alias(nCh, &pcfg->struChName[nCh]);
						CIU_ERR(nSDKRet, nRet);
					}*/
				}
			}
			break;

			//通道名(unicode)
		case PICCFG_CHNAME_UNICODE_CONF:
			{
				///////////////////////////////////////////////////////////////////////////////
				//判断buf空间是否足够
				if (dwBufLen < PIC_CFG_LEN)
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", PIC_CFG_LEN, dwBufLen);
					break;
				}

				LPHY_DVR_PIC_CFG pcfg = (LPHY_DVR_PIC_CFG) pBuffer;

				if (0 == pcfg->bValid)
				{
					MPU_DBG("0 == pcfg->bValid\n");
					nRet = IDVR_CIU_STRUCT_UNVALID;
				}
				else
				{
					int nCh, nCount, nSDKRet;

					nCount = MAX_ENC_CHANNEL_NUM;
					if (nCount > DVR_MAX_VIDEOIN_NUM)
					{
						MPU_ERR("remote config picture %d, real %d\n", DVR_MAX_VIDEOIN_NUM, nCount);
						nCount = DVR_MAX_VIDEOIN_NUM;
					}
					//设置各通道图像参数
					for (nCh = 0; nCh < nCount; ++nCh)
					{
						//通道名(unicode)
						nSDKRet = SetPicChannel_Alias_1(nCh, &pcfg->struChName_1[nCh]);
						CIU_ERR(nSDKRet, nRet);
					}
				}
			}
			break;

        // 移动侦测坐标版
        case PICCFG_MOTION_ACTION_CONF:			
		case PICCFG_MOTION_V2_CONF:
            {
				#if 0
				///////////////////////////////////////////////////////////////////////////////
				//判断buf空间是否足够
				if (dwBufLen < sizeof(HY_DVR_MOTION_V2_LIST))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_MOTION_V2_LIST), dwBufLen);
					break;
				}

				LPHY_DVR_MOTION_V2_LIST pcfg = (LPHY_DVR_MOTION_V2_LIST) pBuffer;

				if (0 == pcfg->bValid)
				{
					MPU_DBG("0 == pcfg->bValid\n");
					nRet = IDVR_CIU_STRUCT_UNVALID;
				}
				else
				{
					int nSDKRet;
                    //移动侦测(坐标)
                    nSDKRet = SetPicChannel_Motion_V2(dwUserID, 0, &(pcfg->struMotionV2[0]));
                    CIU_ERR(nSDKRet, nRet);
				}  
				#endif
            }
            break;

        case PICCFG_ADVANCED_CONF:
            {
				///////////////////////////////////////////////////////////////////////////////
				//判断buf空间是否足够
				if (dwBufLen < sizeof(HY_DVR_ADVANCED_CHANNEL_CFG))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_ADVANCED_CHANNEL_CFG), dwBufLen);
					break;
				}

				HY_DVR_ADVANCED_CHANNEL_CFG *pcfg = (HY_DVR_ADVANCED_CHANNEL_CFG *) pBuffer;

				if (0 == pcfg->bValid)
				{
					MPU_DBG("0 == pcfg->bValid\n");
					nRet = IDVR_CIU_STRUCT_UNVALID;
				}
				else
				{
					int nCh, nCount, nSDKRet;

					nCount = MAX_ENC_CHANNEL_NUM;
					if (nCount > DVR_MAX_VIDEOIN_NUM)
					{
						MPU_ERR("remote config picture %d, real %d\n", DVR_MAX_VIDEOIN_NUM, nCount);
						nCount = DVR_MAX_VIDEOIN_NUM;
					}
					//设置各个通道的高级编码参数(H.264或者H.265)
					for (nCh = 0; nCh < nCount; ++nCh)
					{
						nSDKRet = SetAdvanceCodecParam(nCh, pcfg);
						CIU_ERR(nSDKRet, nRet);
					}
				}
            }
            break;

		case PICCFG_MOTION_EX_CONF:
			{
                ///////////////////////////////////////////////////////////////////////////////
                //判断buf空间是否足够
                if (dwBufLen < sizeof(HY_DVR_MOTION_EX_LIST))
                {
                    nRet = IDVR_CIU_BUF_SMALL;
                    MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_MOTION_EX_LIST), dwBufLen);
                    break;
                }
                
				LPHY_DVR_MOTION_EX_LIST pcfg = (LPHY_DVR_MOTION_EX_LIST) pBuffer;

				if (0 == pcfg->bValid)
				{
					MPU_DBG("0 == pcfg->bValid\n");
					nRet = IDVR_CIU_STRUCT_UNVALID;
				}
				else
				{
					SetPicChannel_Motion_Ext(0, pcfg);
				}
            }
			break;
			
		case PICCFG_PEOPLE_DETECT_CFG:
			{
				///////////////////////////////////////////////////////////////////////////////
				//判断buf空间是否足够
				if (dwBufLen < sizeof(HY_DVR_PEOPLE_DETECT_CFG))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_PEOPLE_DETECT_CFG), dwBufLen);
					break;
				}

				LPHY_DVR_PEOPLE_DETECT_CFG pcfg = (LPHY_DVR_PEOPLE_DETECT_CFG) pBuffer;
				if (0 == pcfg->bValid)
				{
					MPU_DBG("0 == pcfg->bValid\n");
					nRet = IDVR_CIU_STRUCT_UNVALID;
				}
				else
				{
					int nSDKRet;
                    //移动侦测(坐标)
                    nSDKRet = SetPicChannel_HumanShape(dwUserID, 0, &(pcfg->struPeopleDetect[0]));
                    CIU_ERR(nSDKRet, nRet);
				}
            }
            break;
            case PICCFG_OSD_CONF_EX:
            {
                ///////////////////////////////////////////////////////////////////////////////
                //判断buf空间是否足够
                if (dwBufLen < sizeof(HY_DVR_OSD_CFG_EX_LIST))
                {
                    nRet = IDVR_CIU_BUF_SMALL;
                    MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_OSD_CFG_EX_LIST), dwBufLen);
                    break;
                }
                
                LPHY_DVR_OSD_CFG_EX_LIST pcfg = (LPHY_DVR_OSD_CFG_EX_LIST) pBuffer;
                
                nRet = SetPicChannel_AirOsd(0, pcfg); 
            }
            break;
#if defined (__IS_SUPPORT_LONGAWAYHOME__) 
	   case PICCFG_LONG_AWAY_HOME_DETECT_CFG:
	        {
	            ///////////////////////////////////////////////////////////////////////////////
	            //判断buf空间是否足够
	            if (dwBufLen < sizeof(HY_DVR_LONG_AWAY_HOME_DETECT_CFG))
	            {
	                nRet = IDVR_CIU_BUF_SMALL;
	                MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_LONG_AWAY_HOME_DETECT_CFG), dwBufLen);
	                break;
	            }
	            
	            LPHY_DVR_LONG_AWAY_HOME_DETECT_CFG pcfg = (LPHY_DVR_LONG_AWAY_HOME_DETECT_CFG) pBuffer;
				if (0 == pcfg->iValid)
				{
					MPU_DBG("0 == pcfg->iValid\n");
					nRet = IDVR_CIU_STRUCT_UNVALID;
				}
				else
				{
				    /* 开长久离家，需要强制联动开启人形侦测 */
                    if (pcfg->iEnable && !(MPU_AVL_HumanShape_IsEnable(0)))
                    {
                        MPU_AVL_AreaDefense_HumanShape_Enable(0, 1);
                        MPU_AVL_AreaDefense_Motion_Enable(0, 0);
                    }
                    MPU_AVL_HumanShape_SetLongAwayEnable(0, pcfg->iEnable);
                    MPU_AVL_HumanShape_SetLongAwayDuration(0, pcfg->iAwayHomeAlarmDuration);
				}
	        }
	        break;
#endif	

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

static int GetOnvifCustomMedia(LPHY_DVR_ONVIF_CUSTOMIZED pcfg)
{
	int nRet = IDVR_OK;	
	int OnvifCustom = 1;
	int nSDKRet = IDVR_OK;
	
	pcfg->nValid = 1;
    //获取新的配置文件读取实例
    CConfig *pCfg = CConfig::NewInstance();

	nSDKRet = pCfg->LoadFileRO(CUSTOM_INFO_FILE);
	if (0 == nSDKRet)
	{
		OnvifCustom = pCfg->GetValue("Onvif", "isSupportMedia2", 1);
	}

	//支持media2的，车载设备是连接不上IPC的流，不支持media2可以连接IPC的流，默认情况下是走media2
	pcfg->isSupportMedia2 = OnvifCustom;
    CConfig::ReleaseInstance(pCfg);

	return nRet;
}

int CIU_SetConfigure_VideoShelter(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, const BYTE * pBuffer, DWORD  dwBufLen)
{
	int nRet = IDVR_OK;

	switch (dwMinorType)
	{
        default:
        {
            MPU_ERR("unsupported dwMinorType = %lu\n", dwMinorType);
            nRet = IDVR_CIU_UNSUPPORTED_MINORTYPE;
        }
        break;
    }
           
    return nRet;
}

int CIU_GetConfigure_VideoShelter(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen)
{
	int nRet = IDVR_OK;

	switch (dwMinorType)
	{        
        default:
        {
            MPU_ERR("unsupported dwMinorType = %lu\n", dwMinorType);
            nRet = IDVR_CIU_UNSUPPORTED_MINORTYPE;
        }
        break;
    }
           
    return nRet;
}

//获取ISP配置基本参数
static int GetIspCommon(BoardSensorType_t SensorType, LPHY_DVR_ISP_COMMON pcfg)
{
	int nRet = IDVR_OK;

	//获取配置文件管理器实例指针
	CConfig *pCfg = CConfig::GetInstance();

	if (NULL != pCfg)
	{
		char szSection[30];

		snprintf(szSection, sizeof(szSection), "ISP_S%d", SensorType);

		///////////////////////////////////////////////////////////////////////////////
		//本结构体是否有效，=0表示无效
		pcfg->bValid = 1;

		/*
		   参数模板配置 :
		   意义： 通过环境条件的不同进行不同参数脚本的调节
		   0    普通 
		   1    室内 
		   2 室内低亮度 
		   3 室外
		   4 出入口
		   5 自定义
//set
		   安霸无效参数，待定
		 */
		pcfg->nWorkMode = pCfg->GetValue(szSection, "WorkMode", 5);

		/*
		   图像颜色 （Video Color）
		   AMBARRELLA 	0 自动 1 黑白 2 彩色  
		 */
		pcfg->nVideoColor = pCfg->GetValue(szSection, "VideoColor", 0);

		/*
		   图像方向 (Video orientation)
		   0    正常
		   1    翻转      
		   2    镜像
		   3    翻转+ 镜像
		 */
		pcfg->nVideoOrientation = pCfg->GetValue(szSection, "VideoOrientation", 0);

		/*
		   	前端输入频率
			0    50Hz
			1    60Hz
		*/
		 
		pcfg->nInputFrequency = pCfg->GetValue(szSection, "InputFrequency", 0);
	}
	else
	{
		//读取配置文件失败
		MPU_ERR("no CConfig instance\n");
		MPU_CHK(NULL != pCfg);
	}

	return nRet;
}
#if 0
//获取ISP配置信息AE Window，需外部将缓冲区清零 ambarella 无效
static int GetIspAEWindow(BoardSensorType_t SensorType, int nTemplate, int nWnd, LPHY_DVR_AEWINDOW_CFG pcfg)
{
	int nRet = IDVR_OK;

	//获取配置文件管理器实例指针
	CConfig *pCfg = CConfig::GetInstance();

	if (NULL != pCfg)
	{
		char szSection[30];

		snprintf(szSection, sizeof(szSection), "ISP_S%d_T%d_AE%d", SensorType, nTemplate, nWnd);

		///////////////////////////////////////////////////////////////////////////////
		//本结构体是否有效，=0表示无效
		pcfg->bValid = 1;

		//开关(on /off)
		pcfg->bOpen = pCfg->GetValue(szSection, "Open", 0);

		//X (0-4095) 需要识别到最大的宽度，如果是最大720p,就是1280
		pcfg->nX = pCfg->GetValue(szSection, "X", 0);

		//Y (0-4095) 需要识别最大有效像素高度
		pcfg->nY = pCfg->GetValue(szSection, "Y", 0);

		//width (1-4095) 需要识别到最大的宽度
		pcfg->nWidth = pCfg->GetValue(szSection, "Width", 160);

		//height(1-4095) 需要识别最大有效像素高度
		pcfg->nHeight = pCfg->GetValue(szSection, "Height", 120);

		//Weighting (1X 到 8X可选) 参数0-7
		pcfg->nWeighting = pCfg->GetValue(szSection, "Weighting", 0);
	}
	else
	{
		//读取配置文件失败
		MPU_ERR("no CConfig instance\n");
		MPU_CHK(NULL != pCfg);
	}

	return nRet;
}
#endif
//获取ISP配置信息白平衡，需外部将缓冲区清零
static int GetIspWhiteBalance(BoardSensorType_t SensorType, int nTemplate, LPHY_DVR_WHITEBALANCE_CFG pcfg)
{
	int nRet = IDVR_OK;

	//获取配置文件管理器实例指针
	CConfig *pCfg = CConfig::GetInstance();

	if (NULL != pCfg)
	{
		char szSection[30];

		snprintf(szSection, sizeof(szSection), "ISP_S%d_T%d", SensorType, nTemplate);

		///////////////////////////////////////////////////////////////////////////////
		//本结构体是否有效，=0表示无效
		pcfg->bValid = 1;

		/*
	AMBARELLA 0 自动 1 白炽灯 2 D4000 3 D5000 4 晴天 5 阴天 6 闪光 7 冷白荧光灯 8 荧光灯2 9 水中 10 关闭

		   0 自动
		   1 手动： 手动 需要修改 RED Gain (0-255),Blue Gain(0-255)
		   2 多云	
		   3 太阳
		   4 阴天
		   5 冷白荧光灯
		 */
		pcfg->bWorkMode = pCfg->GetValue(szSection, "WhiteBalanceMode", 0);

		for (int nWorkMode = 0; nWorkMode < DVR_MAX_ISP_WHITEBALANCE; ++nWorkMode)
		{
			snprintf(szSection, sizeof(szSection), "ISP_S%d_T%d_WB%d", SensorType, nTemplate, nWorkMode);
		}
	}
	else
	{
		//读取配置文件失败
		MPU_ERR("no CConfig instance\n");
		MPU_CHK(NULL != pCfg);
	}

	return nRet;
}

//获取ISP配置信息模版参数，需外部将缓冲区清零
static int GetIspTemplate(BoardSensorType_t SensorType, int nTemplate, LPHY_DVR_ISP_TEMPLATE pcfg)
{
	int nRet = IDVR_OK;

	//获取配置文件管理器实例指针
	CConfig *pCfg = CConfig::GetInstance();

	if (NULL != pCfg)
	{
		char szSection[30];

		pcfg->bValid = 1;		//! 本结构体是否有效，=0表示无效

		snprintf(szSection, sizeof(szSection), "ISP_S%d_T%d", SensorType, nTemplate);


		pcfg->struAEMode.bValid = 1;
		
		pcfg->struAEMode.nMode = pCfg->GetValue("ISP_S_AE", "Mode", 2);
	
		/* 
		   AE Window 设置,分成6个Window,分别是 1 -6 ambarella 无效
		 
		for (nWnd = 0; nWnd < DVR_MAX_ISP_AEWINDOW; ++nWnd)
		{
			GetIspAEWindow(SensorType, nTemplate, nWnd, &pcfg->struAEWindow[nWnd]);
		}*/


		/*
		   白平衡 (White Balance)
		 */
		GetIspWhiteBalance(SensorType, nTemplate, &pcfg->struWhiteBalance);


		/*
		   曝光参数
		 */
		pcfg->struExposal.bValid = 1;
		/*
		   镜头光圈：(Lens Type)
		   关闭自动自动  开启启动光圈   手动光圈
		 */
		pcfg->struExposal.bLensType = pCfg->GetValue(szSection, "LensType", 0);
		/*
		   目标亮度  (Target Lumiance)  0-255
		 */
		pcfg->struExposal.nTargetLumiance = pCfg->GetValue(szSection, "TargetLumiance", 100);
		/*
		   偏移 (Offset)    0-255
		 */
		pcfg->struExposal.nOffset = pCfg->GetValue(szSection, "Offset", 20);

		/*
		   快门速度  (Shutter Speed)
		   最小速度，0～1/2
		   最大速度，1/120～1
		   速度可选0,1/120,1/60,1/30,1/15,1/8,1/4,1/2,1 
		 */
		pcfg->struExposal.nMinShutterSpeed = pCfg->GetValue(szSection, "MinShutterSpeed", 120);
		pcfg->struExposal.nMaxShutterSpeed = pCfg->GetValue(szSection, "MaxShutterSpeed", 30);
		/*
		   增益系数 (Gain)
		   最小增益 1x ~ 16x 
		   最大增益 2x ~ 32x
		 */
		pcfg->struExposal.nMinAutoGain = pCfg->GetValue(szSection, "MinAutoGain", 1);
		pcfg->struExposal.nMaxAutoGain = pCfg->GetValue(szSection, "MaxAutoGain", 6);


		/*
		   数字降噪 （De-Noise）
		 */
		pcfg->struDeNoise.bValid = 1;	//! 本结构体是否有效，=0表示无效
		/*
		   0 关闭
		   1 开启
			  Y  Strength AMBARELLA 背光补偿  0 关 1 开 
		 Cb strength  AMBARELLA 数字宽动态  0 关 1 自动 2 手动1级 3 手动2级 4 手动3级 5 手动4级
		 Cr strength  无效
		 */
		pcfg->struDeNoise.bWorkMode = pCfg->GetValue(szSection, "DeNoiseMode", 0);
		pcfg->struDeNoise.nYStrength = pCfg->GetValue(szSection, "YStrength", 10);
		pcfg->struDeNoise.nCbStrength = pCfg->GetValue(szSection, "CbStrength", 10);
		pcfg->struDeNoise.nCrStrength = pCfg->GetValue(szSection, "CrStrength", 10);
	}

	return nRet;
}

//获取ISP配置，需外部将缓冲区清零
static int GetIspCfg(LPHY_DVR_NXP8850ISP_CFG pcfg)
{
	int nRet = IDVR_OK, nTemplate;

	BoardSensorType_t SensorType;

	///////////////////////////////////////////////////////////////////////////////
	//本结构体是否有效，=0表示无效
	pcfg->bValid = 1;

	//获取 Sensor类型
	ZWSys_GetSensorType(&SensorType);

	//SensorType=BOARD_SENSOR_NORM5150;
	///////////////////////////////////////////////////////////////////////////////
	//获取基本参数
	GetIspCommon(SensorType, &pcfg->struCommon);

	//设置各模版参数
	for (nTemplate = 0; nTemplate < DVR_MAX_ISP_TEMPLATE; ++nTemplate)
	{
		GetIspTemplate(SensorType, nTemplate, &pcfg->struTemplate[nTemplate]);
	}

	return nRet;
}

//设置ISP配置基本参数
static int SetIspCommon(BoardSensorType_t SensorType, LPHY_DVR_ISP_COMMON pcfg)
{
	int nRet = IDVR_OK;

	///////////////////////////////////////////////////////////////////////////////
	//本结构体是否有效，=0表示无效
	if (0 == pcfg->bValid)
	{
		MPU_DBG("0 == pcfg->bValid\n");
		return IDVR_CIU_STRUCT_UNVALID;
	}

	//获取配置文件管理器实例指针
	CConfig *pCfg = CConfig::GetInstance();

	if (NULL != pCfg)
	{
		char szSection[30];

		snprintf(szSection, sizeof(szSection), "ISP_S%d", SensorType);

		/*
		   参数模板配置 :
		   意义： 通过环境条件的不同进行不同参数脚本的调节
			AMBARELLA 0 自动 1 白炽灯 2 D4000 3 D5000 4 晴天 5 阴天 6 闪光 7 冷白荧光灯 8 荧光灯2 9 水中 10 关闭
		   0    普通 
		   1    室内 
		   2 室内低亮度 
		   3 室外
		   4 出入口
		   5 自定义
		 */
		pCfg->SetValue(szSection, "WorkMode", pcfg->nWorkMode);

		/*
		   图像颜色 （Video Color）
		   AMBARRELLA 	0 自动 1 黑白 2 彩色  
		 */
        if (0 == bDayNightModeFixd)
        {
			nRet = pCfg->SetValue(szSection, "VideoColor", pcfg->nVideoColor);
			if (nRet == 0 && pCfg->m_bReadOnlyCfgFlag == 1)
			{
				pcfg->nVideoColor = pCfg->GetValue(szSection, "VideoColor", 0);
			}
        }

		/*
		   图像方向 (Video orientation)
		   0    正常
		   1    翻转      
		   2    镜像
		   3    翻转+ 镜像[顺时针转180]
		 */
		pCfg->SetValue(szSection, "VideoOrientation", pcfg->nVideoOrientation);

	/*
	前端输入频率
	0    50Hz
	1    60Hz
	*/
		pCfg->SetValue(szSection, "InputFrequency", pcfg->nInputFrequency);
	}
	else
	{
		//读取配置文件失败
		MPU_ERR("no CConfig instance\n");
		MPU_CHK(NULL != pCfg);

		nRet = IDVR_ERROR_CONFIGFILE;
	}

	return nRet;
}

static int SetDayNightMode_Ex(void *hChannelHandle, DAYNIGHTMODE_T mode, LP_IrcutTimeModeInfo info)
{
	int nRet = IDVR_OK;

	nRet = ZWSys_EncSetDayNightMode(hChannelHandle, mode, info);

	return nRet;
}


//设置各通道ISP配置信息白平衡，需外部将缓冲区清零
static int SetIspWhiteBalance(BoardSensorType_t SensorType, int nTemplate, LPHY_DVR_WHITEBALANCE_CFG pcfg)
{
	int nRet = IDVR_OK;

	char szSection[30];

	///////////////////////////////////////////////////////////////////////////////
	//本结构体是否有效，=0表示无效
	if (0 == pcfg->bValid)
	{
		MPU_DBG("0 == pcfg->bValid, SensorType = %d, nTemplate = %d\n", SensorType, nTemplate);
		return IDVR_CIU_STRUCT_UNVALID;
	}

	//获取配置文件管理器实例指针
	CConfig *pCfg = CConfig::GetInstance();

	if (NULL != pCfg)
	{

	/*
		AMBARELLA 0 自动 1 白炽灯 2 D4000 3 D5000 4 晴天 5 阴天 6 闪光 7 冷白荧光灯 8 荧光灯2 9 水中 10 关闭
		int bWorkMode;
	*/

		snprintf(szSection, sizeof(szSection), "ISP_S%d_T%d", SensorType, nTemplate);

		pCfg->SetValue(szSection, "WhiteBalanceMode", pcfg->bWorkMode);

		for (int nWorkMode = 0; nWorkMode < DVR_MAX_ISP_WHITEBALANCE; ++nWorkMode)
		{
			snprintf(szSection, sizeof(szSection), "ISP_S%d_T%d_WB%d", SensorType, nTemplate, nWorkMode);
		}
	}
	else
	{
		//读取配置文件失败
		MPU_ERR("no CConfig instance\n");
		MPU_CHK(NULL != pCfg);

		nRet = IDVR_ERROR_CONFIGFILE;
	}

	return nRet;
}

//设置ISP模版参数
static int SetIspTemplate(BoardSensorType_t SensorType, int nTemplate, LPHY_DVR_ISP_TEMPLATE pcfg)
{
	int nRet = IDVR_OK, nSDKRet;

	char szSection[30];

	//获取配置文件管理器实例指针
	CConfig *pCfg = CConfig::GetInstance();


	///////////////////////////////////////////////////////////////////////////////
	//本结构体是否有效，=0表示无效
	if (0 == pcfg->bValid)
	{
		MPU_DBG("0 == pcfg->bValid, nTemplate = %d\n", nTemplate);
		return IDVR_CIU_STRUCT_UNVALID;
	}


	if ( pcfg->struAEMode.bValid )
		pCfg->SetValue("ISP_S_AE", "Mode", pcfg->struAEMode.nMode);

	#if 0 
	   //AE Window 设置,分成6个Window,分别是 1 -6  ambarella 无效
	 
	for (nWnd = 0; nWnd < DVR_MAX_ISP_AEWINDOW; ++nWnd)
	{
		nSDKRet = SetIspAEWindow(SensorType, nTemplate, nWnd, &pcfg->struAEWindow[nWnd]);
		CIU_ERR(nSDKRet, nRet);
	}
	#endif


	/*
	   白平衡 (White Balance)
	 */
	nSDKRet = SetIspWhiteBalance(SensorType, nTemplate, &pcfg->struWhiteBalance);
	CIU_ERR(nSDKRet, nRet);


	if (NULL != pCfg)
	{
		snprintf(szSection, sizeof(szSection), "ISP_S%d_T%d", SensorType, nTemplate);

		/*
		   曝光参数
		 */
		if (pcfg->struExposal.bValid)
		{
			/*
			   镜头光圈：(Lens Type)
			   		AMBARELLA 0 手动光圈  1 自动光圈
			 */
			pCfg->SetValue(szSection, "LensType", pcfg->struExposal.bLensType);
			/*
			   目标亮度  (Target Lumiance)  
			AMBARELLA  曝光值 0-F1.6 1-F2.2 2-F3.2 3-F4.4 5-F6.4 6-F8.8 7 F12 8-F17 9-F24 10-F34  

			 */
			pCfg->SetValue(szSection, "TargetLumiance", pcfg->struExposal.nTargetLumiance);
			/*
			  // 偏移 (Offset) 
			视频颜色模式 0 PC 1 TV
			 */
			pCfg->SetValue(szSection, "Offset", pcfg->struExposal.nOffset);
			/*
			   快门速度  (Shutter Speed)
			   最小速度，	AMBARELLA 0 自动 1/4 1/8 ........1/10000 
			   最大速度，	AMBARELLA 0 自动 1/4 1/8 ........1/10000 
			 */
			pCfg->SetValue(szSection, "MinShutterSpeed", pcfg->struExposal.nMinShutterSpeed);
			pCfg->SetValue(szSection, "MaxShutterSpeed", pcfg->struExposal.nMaxShutterSpeed);
			/*
			   增益系数 (Gain) ambarella 默认值为6
			   最小增益 1x ~ 16x 无效
			   最大增益 2x ~ 32x
			 */
			pCfg->SetValue(szSection, "MinAutoGain", pcfg->struExposal.nMinAutoGain);
			pCfg->SetValue(szSection, "MaxAutoGain", pcfg->struExposal.nMaxAutoGain);
		}


		/*
		   数字降噪 （De-Noise）
		 */
		if (pcfg->struDeNoise.bValid)	//! 本结构体是否有效，=0表示无效
		{
			/*
			   0    关闭
			   1    开启： 
			  Y  Strength AMBARELLA 背光补偿  0 关 1 开 
			 Cb strength  AMBARELLA 数字宽动态  0 关 1 自动 2 手动1级 3 手动2级 4 手动3级 5 手动4级
			 Cr strength  无效
			 */
			pCfg->SetValue(szSection, "DeNoiseMode", pcfg->struDeNoise.bWorkMode);
			if (0 != pcfg->struDeNoise.nYStrength)
				pcfg->struDeNoise.nYStrength = 1;
			else
				pcfg->struDeNoise.nYStrength = 0;
			pCfg->SetValue(szSection, "YStrength", pcfg->struDeNoise.nYStrength);
			pCfg->SetValue(szSection, "CbStrength", pcfg->struDeNoise.nCbStrength);
			pCfg->SetValue(szSection, "CrStrength", pcfg->struDeNoise.nCrStrength);
		}
	}
	else
	{
		//读取配置文件失败
		MPU_ERR("no CConfig instance\n");
		MPU_CHK(NULL != pCfg);

		nRet = IDVR_ERROR_CONFIGFILE;
	}

	return nRet;
}

//设置ISP配置
static int SetIspCfg(LPHY_DVR_NXP8850ISP_CFG pcfg)
{
	int nRet = IDVR_OK, nSDKRet;

	BoardSensorType_t SensorType;

	///////////////////////////////////////////////////////////////////////////////
	//本结构体是否有效，=0表示无效
	if (0 == pcfg->bValid)
	{
		MPU_DBG("0 == pcfg->bValid\n");
		return IDVR_CIU_STRUCT_UNVALID;
	}

	//获取 Sensor类型
	ZWSys_GetSensorType(&SensorType);
	///////////////////////////////////////////////////////////////////////////////
	//设置基本参数

	nSDKRet = SetIspCommon(SensorType, &pcfg->struCommon);
	CIU_ERR(nSDKRet, nRet);

#if 1
	//只设置用户自定义
	nSDKRet = SetIspTemplate(SensorType, 5, &pcfg->struTemplate[5]);
	CIU_ERR(nSDKRet, nRet);
#else
	//设置各模版参数
	for (nTemplate = 0; nTemplate < DVR_MAX_ISP_TEMPLATE; ++nTemplate)
	{
		nSDKRet = SetIspTemplate(SensorType, nTemplate, &pcfg->struTemplate[nTemplate]);
		CIU_ERR(nSDKRet, nRet);
	}
#endif

	ISP_Init();

	return nRet;
}

static int SetIpcColorCfg(ZW_IPC_COLOR * pcfg)
{
	int nRet = IDVR_OK, nSDKRet=IDVR_OK,i;
	BoardSensorType_t SensorType;

	//本结构体是否有效，=0表示无效
	if (0 == pcfg->bValid)
	{
		MPU_DBG("0 == pcfg->bValid\n");
		return IDVR_CIU_STRUCT_UNVALID;
	}
	printf("SetIpcColorCfg--nAutoChangeBright=%d--nBright=%d--nContrast=%d--nSaturation=%d--nChroma=%d--nSharpness=%d----\n",
		pcfg->nAutoChangeBright, pcfg->nBright, pcfg->nContrast, pcfg->nSaturation, pcfg->nChroma, pcfg->nSharpness);

	//获取 Sensor类型
	ZWSys_GetSensorType(&SensorType);
	//获取配置文件管理器实例指针
	CConfig *pCfgFile = CConfig::GetInstance();

	if (NULL != pCfgFile)
	{
		char szSection[30];
		void *hChannelHandle=ZWSys_EncChannelOpen(0);
		
		snprintf(szSection, sizeof(szSection), "ISP_S%d", SensorType);
		
		if(pcfg->nAutoChangeBright != pCfgFile->GetValue(szSection, "AutoChangeBright", 1))
			nSDKRet = ZWSys_EncSetAutoChangeBright(hChannelHandle, (pcfg->nAutoChangeBright == 0) ? 0 : 1);
		if (0 != nSDKRet)
		{ 
			MPU_ERR("%s: set Dc Iris fail, return %d\n", szSection, nSDKRet);
			nRet = IDVR_ERROR_HISDK;
		}
		pCfgFile->SetValue(szSection, "AutoChangeBright", pcfg->nAutoChangeBright);
#if 0
		pCfgFile->SetValue(szSection, "Bright", pcfg->nBright);
		pCfgFile->SetValue(szSection, "Contrast", pcfg->nContrast);
		pCfgFile->SetValue(szSection, "Saturation", pcfg->nSaturation);
		pCfgFile->SetValue(szSection, "Chorma", pcfg->nChroma);
		pCfgFile->SetValue(szSection, "Sharpness", pcfg->nSharpness);
		//nSDKRet = ZWSys_EncSetVideoPara(hChannelHandle, pcfg->nBright, pcfg->nContrast, pcfg->nSaturation, pcfg->nChroma, pcfg->nSharpness);
		if (0 != nSDKRet)
		{ 
			MPU_ERR("%s: set Dc Iris fail, return %d\n", szSection, nSDKRet);
			nRet = IDVR_ERROR_HISDK;
		}
#endif
		for(i=0; i<4; i++)
		{
			if((pcfg->nBright > 1) || (pcfg->nContrast > 1) || (pcfg->nSaturation > 1) || (pcfg->nSharpness > 1))
			{
				//亮度、色度、对比度、饱和度
				nSDKRet = MPU_AVL_Pic_SetPicParam(0, i, 0, pcfg->nBright);/*!< 亮度,范围[0,100] */
				MPU_CHK(0 == nSDKRet);
				
				nSDKRet = MPU_AVL_Pic_SetPicParam(0, i, 1, pcfg->nContrast);/*!< 对比度,范围[0,100] */
				MPU_CHK(0 == nSDKRet);
				
				nSDKRet = MPU_AVL_Pic_SetPicParam(0, i, 2, pcfg->nSaturation);/*!< 饱和度,范围[0,100] */
				MPU_CHK(0 == nSDKRet);
                
				//onvif过审，去掉限制，但对0值需要特殊处理，和协议交互是0，设备给sdk是1
				nSDKRet = MPU_AVL_Pic_SetPicParam(0, i, 4, pcfg->nSharpness);/*!< 锐度,范围[0,100] */
				MPU_CHK(0 == nSDKRet);
			}
			nSDKRet = MPU_AVL_Pic_SetPicParam(0, i, 3, pcfg->nChroma);/*!< 色度,范围[0,100] */
			MPU_CHK(0 == nSDKRet);		
			//时间段开始和结束时间
			//nSDKRet = MPU_AVL_Pic_SetTimeCfg(nCh, nTimeSec, pcfg->nStartTime, pcfg->nStopTime);
			//MPU_CHK(0 == nSDKRet);
		}

	}
	return nRet;
}

static int GetIpcColorCfg(ZW_IPC_COLOR * pcfg)
{
	int nRet = IDVR_OK;
	BoardSensorType_t SensorType;
	
	CConfig *pCfgFile = CConfig::GetInstance();
	if(NULL != pCfgFile)
	{
		char szSection[30];

		pcfg->bValid = 1;

		//获取 Sensor类型
		ZWSys_GetSensorType(&SensorType);
		snprintf(szSection, sizeof(szSection), "ISP_S%d", SensorType);
		pcfg->nAutoChangeBright = pCfgFile->GetValue(szSection, "AutoChangeBright", 1);
#if 0
		pcfg->nBright = pCfgFile->GetValue(szSection, "Bright", 50);
		pcfg->nContrast = pCfgFile->GetValue(szSection, "Contrast", 50);
		pcfg->nSaturation = pCfgFile->GetValue(szSection, "Saturation", 50);
		pcfg->nChroma = pCfgFile->GetValue(szSection, "Chorma", 50);
		pcfg->nSharpness = pCfgFile->GetValue(szSection, "Sharpness", 50);
#endif
		
		//亮度、色度、对比度、饱和度
		pcfg->nBright = MPU_AVL_Pic_GetPicParam(0, 0, 0);
		pcfg->nContrast = MPU_AVL_Pic_GetPicParam(0, 0, 1);
		pcfg->nSaturation = MPU_AVL_Pic_GetPicParam(0, 0, 2);
		pcfg->nChroma = MPU_AVL_Pic_GetPicParam(0, 0, 3);
		pcfg->nSharpness = MPU_AVL_Pic_GetPicParam(0, 0, 4);
		
		//时间段开始和结束时间
		//MPU_AVL_Pic_GetTimeCfg(nCh, nTimeSec, &pcfg->nStartTime, &pcfg->nStopTime);
	}
	else
	{
		//读取配置文件失败
		MPU_ERR("no CConfig instance\n");
		MPU_CHK(NULL != pCfgFile);
	}
	MPU_DBG("--GetIpcColorCfg--nAutoChangeBright=%d--nBright=%d--nContrast=%d--nSaturation=%d--nChroma=%d--nSharpness=%d----\n",
		pcfg->nAutoChangeBright, pcfg->nBright, pcfg->nContrast, pcfg->nSaturation, pcfg->nChroma, pcfg->nSharpness);
	return nRet;
}

static int SetIpcExposureCfg(ZW_IPC_EXPOSURE * pcfg)
{
	int nSDKRet = IDVR_OK, nValue;
	BoardSensorType_t SensorType;

	//本结构体是否有效，=0表示无效
	if (0 == pcfg->bValid)
	{
		MPU_DBG("0 == pcfg->bValid\n");
		return IDVR_CIU_STRUCT_UNVALID;
	}
	//获取 Sensor类型
	ZWSys_GetSensorType(&SensorType);
	printf("nIrisType = %d nExposureDelay = %d nGain = %d \n",pcfg->nIrisType,pcfg->nExposureDelay,pcfg->nGain);
	//获取配置文件管理器实例指针
	CConfig *pCfgFile = CConfig::GetInstance();

	if (NULL != pCfgFile)
	{
		char szSection[30];
		AE_T sensor_cfg;
		void *hChannelHandle=ZWSys_EncChannelOpen(0);
		
		if (hChannelHandle == NULL)
			MPU_ERR("ChannelOpen fail, !!!!!!!!!\n");
		
		snprintf(szSection, sizeof(szSection), "ISP_S%d_T%d", SensorType, USER_CUSTOM_TEMPLATE);
		pCfgFile->SetValue(szSection, "LensType", pcfg->nIrisType);
		pCfgFile->SetValue(szSection, "MaxShutterSpeed", pcfg->nExposureDelay);
		pCfgFile->SetValue(szSection, "MaxAutoGain", pcfg->nGain);

		nSDKRet = ZWSys_SetDcIris(hChannelHandle, pcfg->nIrisType);
		if (0 != nSDKRet)
		{ 
			MPU_ERR("%s: set Dc Iris fail, return %d\n", szSection, nSDKRet);
		}
		
		sensor_cfg.MaxShutterTime = pcfg->nExposureDelay;
		sensor_cfg.MinShutterTime = 10000;
		sensor_cfg.sensorGain = pcfg->nGain;
		snprintf(szSection, sizeof(szSection), "ISP_S%d", SensorType);
		nValue = pCfgFile->GetValue(szSection, "InputFrequency", 0);
		if (0 == nValue)
		{
			sensor_cfg.antiFlicker = CMOS_FREQUENCY_50HZ;
		}
		else if (1 == nValue)
		{
			sensor_cfg.antiFlicker = CMOS_FREQUENCY_60HZ;	
		}
		else if (2 == nValue)
		{
			sensor_cfg.antiFlicker = CMOS_FREQUENCY_CLOSE;
		}

		nSDKRet = ZWSys_SetAEParam(hChannelHandle,&sensor_cfg);
		if (0 != nSDKRet)
		{
			MPU_ERR("%s: ZWSys_SetAEParam fail, return %d\n", szSection, nSDKRet);
		}
	}
	return 0;	
}

static int GetIpcExposureCfg(ZW_IPC_EXPOSURE * pcfg)
{
	int nRet = IDVR_OK;
	BoardSensorType_t SensorType;
	
	CConfig *pCfgFile = CConfig::GetInstance();
	if(NULL != pCfgFile)
	{
		char szSection[30];

		pcfg->bValid = 1;

		//获取 Sensor类型
		ZWSys_GetSensorType(&SensorType);
		snprintf(szSection, sizeof(szSection), "ISP_S%d_T%d", SensorType, USER_CUSTOM_TEMPLATE);
		pcfg->nIrisType = pCfgFile->GetValue(szSection, "LensType", 1);
		pcfg->nExposureDelay = pCfgFile->GetValue(szSection, "MaxShutterSpeed", 30);
		pcfg->nGain = pCfgFile->GetValue(szSection, "MaxAutoGain", 0);
	}
	else
	{
		//读取配置文件失败
		MPU_ERR("no CConfig instance\n");
		MPU_CHK(NULL != pCfgFile);
	}
	
	MPU_DBG("--GetIpcExposureCfg--nIrisType=%d--nExposureDelay=%d--nGain=%d---\n",
		pcfg->nIrisType,pcfg->nExposureDelay,pcfg->nGain);
	return nRet;
}

static int SetIpcWhiteBlanceCfg(ZW_IPC_WHITE_BLANCE * pcfg)
{
	int nSDKRet;
	BoardSensorType_t SensorType;

	//本结构体是否有效，=0表示无效
	if (0 == pcfg->bValid)
	{
		MPU_DBG("0 == pcfg->bValid\n");
		return IDVR_CIU_STRUCT_UNVALID;
	}
	printf("SetIpcWhiteBlanceCfg--nWhiteBlanceMode=%d--nGainRed=%d--nGainBlue=%d-----\n",
		pcfg->nWhiteBlanceMode, pcfg->nGainRed, pcfg->nGainBlue);

	//获取 Sensor类型
	ZWSys_GetSensorType(&SensorType);
	//获取配置文件管理器实例指针
	CConfig *pCfgFile = CConfig::GetInstance();

	if (NULL != pCfgFile)
	{
		char szSection[30];
		void *hChannelHandle=ZWSys_EncChannelOpen(0);
		WHITEBALANCE_T WhiteBalance;

		if (hChannelHandle == NULL)
			MPU_ERR("ChannelOpen fail, !!!!!!!!!\n");
		snprintf(szSection, sizeof(szSection), "ISP_S%d_T%d", SensorType, USER_CUSTOM_TEMPLATE);
		pCfgFile->SetValue(szSection, "WhiteBalanceMode", pcfg->nWhiteBlanceMode);

		if(4 == pcfg->nWhiteBlanceMode)
		{
			snprintf(szSection, sizeof(szSection), "ISP_S%d_T%d_WB%d", SensorType, USER_CUSTOM_TEMPLATE, 5);
			pCfgFile->SetValue(szSection, "RedGain", pcfg->nGainRed);
			pCfgFile->SetValue(szSection, "BlueGain", pcfg->nGainBlue);
		}
		if((5 == pcfg->nWhiteBlanceMode))// || (4 == pcfg->nWhiteBlanceMode)
			WhiteBalance.WBEnable = 0;
		else
			WhiteBalance.WBEnable = 1;
		WhiteBalance.WBWorkMode = pcfg->nWhiteBlanceMode;
		WhiteBalance.RedGain= pcfg->nGainRed;
		WhiteBalance.BlueGain= pcfg->nGainBlue;
		nSDKRet = ZWSys_SetWhiteBalance(hChannelHandle,&WhiteBalance);
		if (0 != nSDKRet)
		{
			MPU_ERR("%s: set white balance  to dsp fail, return %d\n", szSection, nSDKRet);
		}
	}
	return 0;
}

static int GetIpcWhiteBlanceCfg(ZW_IPC_WHITE_BLANCE * pcfg)
{
	int nRet = IDVR_OK;
	BoardSensorType_t SensorType;
	
	CConfig *pCfgFile = CConfig::GetInstance();
	if(NULL != pCfgFile)
	{
		char szSection[30];

		pcfg->bValid = 1;

		//获取 Sensor类型
		ZWSys_GetSensorType(&SensorType);
		snprintf(szSection, sizeof(szSection), "ISP_S%d_T%d", SensorType, USER_CUSTOM_TEMPLATE);
		pcfg->nWhiteBlanceMode = pCfgFile->GetValue(szSection, "WhiteBalanceMode", 6);
		snprintf(szSection, sizeof(szSection), "ISP_S%d_T%d_WB%d", SensorType, USER_CUSTOM_TEMPLATE, 5);
		pcfg->nGainRed = pCfgFile->GetValue(szSection, "RedGain", 50);
		pcfg->nGainBlue = pCfgFile->GetValue(szSection, "BlueGain", 50);
	}
	else
	{
		//读取配置文件失败
		MPU_ERR("no CConfig instance\n");
		MPU_CHK(NULL != pCfgFile);
	}
	
	MPU_DBG("--GetIpcWhiteBlanceCfg--nWhiteBlanceMode=%d--nGainRed=%d--nGainBlue=%d-----\n",
		pcfg->nWhiteBlanceMode, pcfg->nGainRed, pcfg->nGainBlue);
	return nRet;
}

static int SetIpcDayNightCfg(ZW_IPC_DAY_NIGHT * pcfg)
{
	int nRet = IDVR_OK, nSDKRet;
	BoardSensorType_t SensorType;
	int DayNightMode = 0;

	//本结构体是否有效，=0表示无效
	if (0 == pcfg->bValid)
	{
		MPU_DBG("0 == pcfg->bValid\n");
		return IDVR_CIU_STRUCT_UNVALID;
	}
	printf("SetIpcDayNightCfg--nMode=%d--Start=%d:%d:%d--stop=%d:%d:%d------\n",
		pcfg->nMode, pcfg->nStartHour, pcfg->nStartMin, pcfg->nStartSec,
		pcfg->nStopHour, pcfg->nStopMin, pcfg->nStopSec);

#if defined (__IR_CUT_CONTROL_IMPROVE__)
    //普通红外各模式
	if ((0 == bSptNormIRPassive) && (0 == pcfg->nMode))
	{
		MPU_DBG("--------SetIpcDayNightCfg Not support Passive--------\n");
		return IDVR_ERROR_UNSUPPORT;
	}
	if ((0 == bSptNormIRDay) && (1 == pcfg->nMode))
	{
		MPU_DBG("--------SetIpcDayNightCfg Not support Day--------\n");
		return IDVR_ERROR_UNSUPPORT;
	}
	if ((0 == bSptNormIRNight) && (2 == pcfg->nMode))
	{
		MPU_DBG("--------SetIpcDayNightCfg Not support Night--------\n");
		return IDVR_ERROR_UNSUPPORT;
	}
	if ((0 == bSptNormIRplan) && (3 == pcfg->nMode))
	{
		MPU_DBG("--------SetIpcDayNightCfg Not support time--------\n");
		return IDVR_ERROR_UNSUPPORT;
	}
	if ((0 == bSptNormIRActive) && (4 == pcfg->nMode))
	{
		MPU_DBG("--------SetIpcDayNightCfg Not support Initiative--------\n");
		return IDVR_ERROR_UNSUPPORT;
	}
#endif

	if ((0 == bSupportColorNight) && (5 == pcfg->nMode))
	{
		MPU_DBG("--------SetIpcDayNightCfg Not support ColorNight--------\n");
		return IDVR_ERROR_UNSUPPORT;
	}

	if ((0 == bSupportIntelligentNight) && (6 == pcfg->nMode))
	{
		MPU_DBG("--------SetIpcDayNightCfg Not support IntelligentNight--------\n");
		return IDVR_ERROR_UNSUPPORT;
	}

	if ((0 == bSupportVariableInfrared) && (7 == pcfg->nMode))
	{
		MPU_DBG("--------SetIpcDayNightCfg Not support VariableInfrared--------\n");
		return IDVR_ERROR_UNSUPPORT;
	}

	if ((0 == bSupportVariableWhite) && (8 == pcfg->nMode))
	{
		MPU_DBG("--------SetIpcDayNightCfg Not support VariableWhite--------\n");
		return IDVR_ERROR_UNSUPPORT;
	}

	if ((0 == bSupportNormalWhite) && (9 == pcfg->nMode))
	{
		MPU_DBG("--------SetIpcDayNightCfg Not support NormalWhite--------\n");
		return IDVR_ERROR_UNSUPPORT;
	}

    currentDayNightMode = pcfg->nMode;

	//获取 Sensor类型
	ZWSys_GetSensorType(&SensorType);
	//获取配置文件管理器实例指针
	CConfig *pCfgFile = CConfig::GetInstance();

	if (NULL != pCfgFile)
	{
		char szSection[30];
		void *hChannelHandle=ZWSys_EncChannelOpen(0);
		IrcutTimeModeInfo info;

		snprintf(szSection, sizeof(szSection), "ISP_S%d", SensorType);
		if (hChannelHandle == NULL)
			MPU_ERR("ChannelOpen fail, !!!!!!!!!\n");

        //夜视模式为固定模式，不使用上层传递的值
        if (bDayNightModeFixd)
        {
            DayNightMode = pCfgFile->GetValue(szSection, "VideoColor", 0);
        }
        else
        {
            DayNightMode = pcfg->nMode;
			nRet = pCfgFile->SetValue(szSection, "VideoColor", DayNightMode);
			if (nRet == 0 && pCfgFile->m_bReadOnlyCfgFlag == 1)
			{
				DayNightMode = pCfgFile->GetValue(szSection, "VideoColor", 0);
			}
        }

		if (3 == pcfg->nMode)
		{
			pCfgFile->SetValue(szSection, "DayNightStartHour", pcfg->nStartHour);
			pCfgFile->SetValue(szSection, "DayNightStartMin", pcfg->nStartMin);
			pCfgFile->SetValue(szSection, "DayNightStartSec", pcfg->nStartSec);
			pCfgFile->SetValue(szSection, "DayNightStopHour", pcfg->nStopHour);
			pCfgFile->SetValue(szSection, "DayNightStopMin", pcfg->nStopMin);
			pCfgFile->SetValue(szSection, "DayNightStopSec", pcfg->nStopSec);
			info.nStartHour = pcfg->nStartHour;
			info.nStartMin = pcfg->nStartMin;
			info.nStartSec = pcfg->nStartSec;
			info.nStopHour = pcfg->nStopHour;
			info.nStopMin = pcfg->nStopMin;
			info.nStopSec = pcfg->nStopSec;
		}

		nSDKRet = SetDayNightMode_Ex(hChannelHandle, (DAYNIGHTMODE_T)DayNightMode, &info);
		if (0 != nSDKRet)
		{
			MPU_ERR("%s: set video color to dsp fail, return %d\n", szSection, nSDKRet);
			nRet = IDVR_ERROR_HISDK;
		}	
	}

	return nRet;
}

static int GetIpcDayNightCfg(ZW_IPC_DAY_NIGHT * pcfg)
{
	int nRet = IDVR_OK;
	BoardSensorType_t SensorType;
	
	CConfig *pCfgFile = CConfig::GetInstance();
	if(NULL != pCfgFile)
	{
		char szSection[30];

		pcfg->bValid = 1;
		//获取 Sensor类型
		ZWSys_GetSensorType(&SensorType);
		snprintf(szSection, sizeof(szSection), "ISP_S%d", SensorType);
		pcfg->nMode = pCfgFile->GetValue(szSection, "VideoColor", 0);
		pcfg->nStartHour = pCfgFile->GetValue(szSection, "DayNightStartHour", 19);
		pcfg->nStartMin = pCfgFile->GetValue(szSection, "DayNightStartMin", 0);
		pcfg->nStartSec = pCfgFile->GetValue(szSection, "DayNightStartSec", 0);
		pcfg->nStopHour = pCfgFile->GetValue(szSection, "DayNightStopHour", 7);
		pcfg->nStopMin = pCfgFile->GetValue(szSection, "DayNightStopMin", 0);
		pcfg->nStopSec = pCfgFile->GetValue(szSection, "DayNightStopSec", 0);
	}
	else
	{
		//读取配置文件失败
		MPU_ERR("no CConfig instance\n");
		MPU_CHK(NULL != pCfgFile);
	}
	MPU_DBG("--GetIpcDayNightCfg--nMode=%d--Start=%d:%d:%d--stop=%d:%d:%d------\n",
		pcfg->nMode, pcfg->nStartHour, pcfg->nStartMin, pcfg->nStartSec,
		pcfg->nStopHour, pcfg->nStopMin, pcfg->nStopSec);

	return nRet;
}

/* 设置IrCut相关信息 */  
static int SetIpcIrCutCfg(ZW_COM_ISP_EX * pcfg)
{
	int nRet = IDVR_OK, nSDKRet; 
	int PassivityTimeGetFromCfg;
	int InitactiveTimeGetFromCfg, InitactiveSensitivity;
	int sceneMode;
	int EnableIRcutReverse;
	int colorNightLastTime = 30;
	int nInfraredThreshold = 30;
	int nWhiteThreshold = 30;
	int nImageMode = 0;
	int nDefogStrength = 0;	
	int InfraredWorkMode = 0;
	int InfraredBrightness = 0;
    int nColorWorkMode = 0;
	int nVarWhiteControlMode = 0;
	int nVarWhiteWorkMode = 0;
    int nVarWhiteBrightness = 20;
    int nVarWhiteLightOnThreshold = 50;
    int nVarWhiteLightOffThreshold = 10;        
	int nNormalWhiteLightOffLimit = 25;
    int nNormalWhiteLightOnLimit = 50;
    #if defined (__DOUBLE_LIGHT_NO_LDR_CUSTOM__)
    int nColorNightThreshold = 0;
    int nIntelligentThreshold = 0;
    #endif	
	int nAntiFlashStrength = 0;	
    
	MPU_DBG("pcfg->IpcIspExConfig[0].nInitactiveSwitchTime=%d\n", pcfg->IpcIspExConfig[0].nInitactiveSwitchTime);
	MPU_DBG("pcfg->IpcIspExConfig[0].nPassivitySwitchTime=%d\n", pcfg->IpcIspExConfig[0].nPassivitySwitchTime);
		
	//本结构体是否有效，=0表示无效
	if (!(pcfg->bValid) || !(pcfg->IpcIspExConfig[0].bValid))
	{
		MPU_DBG("0 == bValid\n");
		return IDVR_CIU_STRUCT_UNVALID;
	}
	/* IRCUT转换时间范围是[3, 15]秒 */
	if ( pcfg->IpcIspExConfig[0].nInitactiveSwitchTime < 3 || pcfg->IpcIspExConfig[0].nInitactiveSwitchTime > 15
	  || pcfg->IpcIspExConfig[0].nPassivitySwitchTime  < 3 || pcfg->IpcIspExConfig[0].nPassivitySwitchTime	> 15)  
	{
		MPU_DBG("SwitchTime must be in [3, 15]\n");
		return IDVR_CIU_PARAM_OVERFLOW;
	}
	
	MPU_DBG("--SetIrCutCfg--nPassivityDayNightSensitivity=%d--nPassivitySwitchTime=%d--nInitactiveDayNightSensitivity=%d--nInitactiveSwitchTime=%d--\n", 
			 pcfg->IpcIspExConfig[0].nPassivityDayNightSensitivity, 
			 pcfg->IpcIspExConfig[0].nPassivitySwitchTime, 
			 pcfg->IpcIspExConfig[0].nInitactiveDayNightSensitivity, 
			 pcfg->IpcIspExConfig[0].nInitactiveSwitchTime);

#if defined (__IR_CUT_CONTROL_IMPROVE__)
    //全彩白
    if ((5 == currentDayNightMode) && (0 == bSptColorNightAuto) && (WORKMODE_AUTO == pcfg->IpcIspExConfig[0].nColorWorkMode))
    {
        return IDVR_ERROR_UNSUPPORT;
    }
    //无极红
    if ((7 == currentDayNightMode) && (0 == bSptVaryIRAuto) && (WORKMODE_AUTO == pcfg->IpcIspExConfig[0].nVarInfraredWorkMode))
    {
        return IDVR_ERROR_UNSUPPORT;
    }
    //无极白 自动、手动下定时、手动下阈值控制
    if (8 == currentDayNightMode)
    {
        if ((0 == bSptVaryWitAuto) && (WORKMODE_AUTO == pcfg->IpcIspExConfig[0].nVarWhiteControlMode))
        {
            return IDVR_ERROR_UNSUPPORT;
        }
        if ((0 == bSptVaryWitManuPlan) && (0 == pcfg->IpcIspExConfig[0].nVarWhiteWorkMode))
        {
            return IDVR_ERROR_UNSUPPORT;
        }
        if ((0 == bSptVaryWitManuThresh) && (1 == pcfg->IpcIspExConfig[0].nVarWhiteWorkMode))
        {
            return IDVR_ERROR_UNSUPPORT;
        }
    }
#endif

	//获取配置文件管理器实例指针
	CConfig *pCfgFile = CConfig::GetInstance();

	if (NULL != pCfgFile)
	{
		char szSection[50];
		void *hChannelHandle = ZWSys_EncChannelOpen(0);
		
		snprintf(szSection, sizeof(szSection), "IR_CUT_cfg");
		/* 从配置文件获取自动(被动模式)相关信息 */
		PassivityTimeGetFromCfg  = pCfgFile->GetValue(szSection, "nPassivitySwitchTime", 5); 
		/* 从配置文件获取自动(主动模式)相关信息 */
		InitactiveSensitivity	 = pCfgFile->GetValue(szSection, "nInitactiveDayNightSensitivity", 50); 
		InitactiveTimeGetFromCfg = pCfgFile->GetValue(szSection, "nInitactiveSwitchTime", 5);
		/*获取IRCUT是否反向*/
		EnableIRcutReverse =  pCfgFile->GetValue(szSection, "IRcutReverse", 0);
		/*获取智能全彩的延时时间*/
		colorNightLastTime = pCfgFile->GetValue(szSection, "colorNightLastTime", 30);
		/*获取可变红外光的阀值*/
		nInfraredThreshold = pCfgFile->GetValue(szSection, "InfraredThreshold", 30);
		/*获取可变白光的阀值*/
		nWhiteThreshold = pCfgFile->GetValue(szSection, "WhiteThreshold", 30);
		/*获取图像模式*/
		nImageMode = pCfgFile->GetValue(szSection, "ImageMode", 30);

		#if defined (__IR_CUT_AUTO_MANUAL__)
		/*获取全彩白光的控制模式*/
		nColorWorkMode = pCfgFile->GetValue(szSection, "nColorWorkMode", 1);

		/*获取普通白光开灯阀值*/
		nNormalWhiteLightOnLimit = pCfgFile->GetValue(szSection, "NormalWhiteLightOnLimit", 50);
		/*获取普通白光关灯阀值*/
		nNormalWhiteLightOffLimit = pCfgFile->GetValue(szSection, "NormalWhiteLightOffLimit", 25);

		/*获取无极白光相关*/
        nVarWhiteControlMode = pCfgFile->GetValue(szSection, "nVarWhiteControlMode", 1);
        nVarWhiteWorkMode = pCfgFile->GetValue(szSection, "nVarWhiteWorkMode", 0);
        nVarWhiteBrightness = pCfgFile->GetValue(szSection, "nVarWhiteBrightness", 20);
        nVarWhiteLightOnThreshold = pCfgFile->GetValue(szSection, "nVarWhiteLightOnThreshold", 50);
        nVarWhiteLightOffThreshold = pCfgFile->GetValue(szSection, "nVarWhiteLightOffThreshold", 10);
		#endif

        #if defined (__DOUBLE_LIGHT_NO_LDR_CUSTOM__)
        nColorNightThreshold = pCfgFile->GetValue(szSection, "nColorNightThreshold", 50);
        nIntelligentThreshold = pCfgFile->GetValue(szSection, "nIntelligentThreshold", 50);
        #endif
        
		/*获取场景模式*/
		sceneMode = pCfgFile->GetValue(szSection, "nSceneMode", 3);
		
		/*获取无极红外工作模式*/
		InfraredWorkMode = pCfgFile->GetValue(szSection, "InfraredWorkMode", 1);
		
		/*获取无极红外手动模式亮度*/
		InfraredBrightness = pCfgFile->GetValue(szSection, "InfraredBrightness", 100);

		/* 如果从配置文件中获取到的信息与IE插件传入的信息不一致，则写入配置文件并更新 */
		if (PassivityTimeGetFromCfg != pcfg->IpcIspExConfig[0].nPassivitySwitchTime)
        {
            /* 被动模式的ircut切换时间 */
            nRet = pCfgFile->SetValue(szSection, "nPassivitySwitchTime", pcfg->IpcIspExConfig[0].nPassivitySwitchTime);
			if (nRet == 0 && pCfgFile->m_bReadOnlyCfgFlag == 1)
			{
				pcfg->IpcIspExConfig[0].nPassivitySwitchTime = pCfgFile->GetValue(szSection, "nPassivitySwitchTime", 0);
			}
            nSDKRet = ZWSys_EncSetDayNightSwitchTime(hChannelHandle, pcfg->IpcIspExConfig[0].nPassivitySwitchTime);
            MPU_CHK(0 == nSDKRet);
        }
        if (InitactiveSensitivity != pcfg->IpcIspExConfig[0].nInitactiveDayNightSensitivity)
        {
            /* 主动模式的ircut灵敏度 */
            nRet = pCfgFile->SetValue(szSection, "nInitactiveDayNightSensitivity", pcfg->IpcIspExConfig[0].nInitactiveDayNightSensitivity);
			if (nRet == 0 && pCfgFile->m_bReadOnlyCfgFlag == 1)
			{
				pcfg->IpcIspExConfig[0].nInitactiveDayNightSensitivity = pCfgFile->GetValue(szSection, "nInitactiveDayNightSensitivity", 0);
			}
            nSDKRet = ZWSys_EncSetIrcutInitiativeModeSensity(hChannelHandle,  pcfg->IpcIspExConfig[0].nInitactiveDayNightSensitivity);
            MPU_CHK(0 == nSDKRet);

			/* 被动模式的ircut灵敏度 */
			nRet = pCfgFile->SetValue(szSection, "nPassivityDayNightSensitivity", pcfg->IpcIspExConfig[0].nInitactiveDayNightSensitivity);
			if (nRet == 0 && pCfgFile->m_bReadOnlyCfgFlag == 1)
			{
				pcfg->IpcIspExConfig[0].nInitactiveDayNightSensitivity = pCfgFile->GetValue(szSection, "nPassivityDayNightSensitivity", 0);
			}
			nSDKRet = ZWSys_EncSetIrCut(hChannelHandle,  pcfg->IpcIspExConfig[0].nInitactiveDayNightSensitivity);
			MPU_CHK(0 == nSDKRet);
		}
		
		//fprintf(stderr, "[%s][%d] ********zxr nInitactiveSwitchTime=[%d]*****\r\n",__FUNCTION__,__LINE__, pcfg->IpcIspExConfig[0].nInitactiveSwitchTime);
        if (InitactiveTimeGetFromCfg != pcfg->IpcIspExConfig[0].nInitactiveSwitchTime)
        {
            /* 主动模式的ircut切换时间 */
            nRet = pCfgFile->SetValue(szSection, "nInitactiveSwitchTime", pcfg->IpcIspExConfig[0].nInitactiveSwitchTime);
			if (nRet == 0 && pCfgFile->m_bReadOnlyCfgFlag == 1)
			{
				pcfg->IpcIspExConfig[0].nInitactiveSwitchTime = pCfgFile->GetValue(szSection, "nInitactiveSwitchTime", 0);
			}
            nSDKRet = ZWSys_EncSetDayNightInitiativeSwitchTime(hChannelHandle, pcfg->IpcIspExConfig[0].nInitactiveSwitchTime);
            MPU_CHK(0 == nSDKRet);
        } 
		if (sceneMode != pcfg->IpcIspExConfig[0].nSceneMode)
        {
            nRet = pCfgFile->SetValue(szSection, "nSceneMode", pcfg->IpcIspExConfig[0].nSceneMode);
			if (nRet == 0 && pCfgFile->m_bReadOnlyCfgFlag == 1)
			{
				pcfg->IpcIspExConfig[0].nSceneMode = pCfgFile->GetValue(szSection, "nSceneMode", 0);
			}
            nSDKRet = ZWSys_SetColorType(hChannelHandle, (color_type_E)(pcfg->IpcIspExConfig[0].nSceneMode));
            MPU_CHK(0 == nSDKRet);
        }
		if(EnableIRcutReverse != pcfg->IpcIspExConfig[0].nEnableIRcutReverse)
		{
			nRet = pCfgFile->SetValue(szSection, "IRcutReverse", pcfg->IpcIspExConfig[0].nEnableIRcutReverse);
			if (nRet == 0 && pCfgFile->m_bReadOnlyCfgFlag == 1)
			{
				pcfg->IpcIspExConfig[0].nEnableIRcutReverse = pCfgFile->GetValue(szSection, "IRcutReverse", 0);
			}
			nSDKRet = ZWSys_EncSetIrcutDirection(hChannelHandle, pcfg->IpcIspExConfig[0].nEnableIRcutReverse);
			MPU_CHK(0 == nSDKRet);
		}
		if(colorNightLastTime != pcfg->IpcIspExConfig[0].nColorNightLastTime)
		{
			nRet = pCfgFile->SetValue(szSection, "colorNightLastTime", pcfg->IpcIspExConfig[0].nColorNightLastTime);
			if (nRet == 0 && pCfgFile->m_bReadOnlyCfgFlag == 1)
			{
				pcfg->IpcIspExConfig[0].nColorNightLastTime = pCfgFile->GetValue(szSection, "colorNightLastTime", 0);
			}
			nSDKRet = ZWSys_SetColorNightCfg(hChannelHandle, pcfg->IpcIspExConfig[0].nColorNightLastTime);
			MPU_CHK(0 == nSDKRet);
		}

		if(nInfraredThreshold != pcfg->IpcIspExConfig[0].nVariableInfraredThreshold && (0==VariableInfraredValueFixed))
		{
			nRet = pCfgFile->SetValue(szSection, "InfraredThreshold", pcfg->IpcIspExConfig[0].nVariableInfraredThreshold);
			if (nRet == 0 && pCfgFile->m_bReadOnlyCfgFlag == 1)
			{
				pcfg->IpcIspExConfig[0].nVariableInfraredThreshold = pCfgFile->GetValue(szSection, "InfraredThreshold", 0);
			}
			nSDKRet = ZWSys_SetInfraredThreshold(hChannelHandle, pcfg->IpcIspExConfig[0].nVariableInfraredThreshold);
			MPU_CHK(0 == nSDKRet);
		}
		if((nWhiteThreshold != pcfg->IpcIspExConfig[0].nVariableWhiteThreshold) && (0==VariableWhiteFixed))
		{
			nRet = pCfgFile->SetValue(szSection, "WhiteThreshold", pcfg->IpcIspExConfig[0].nVariableWhiteThreshold);
			if (nRet == 0 && pCfgFile->m_bReadOnlyCfgFlag == 1)
			{
				pcfg->IpcIspExConfig[0].nVariableWhiteThreshold = pCfgFile->GetValue(szSection, "WhiteThreshold", 0);
			}
			nSDKRet = ZWSys_SetWhiteLightThreshold(hChannelHandle, pcfg->IpcIspExConfig[0].nVariableWhiteThreshold);
			MPU_CHK(0 == nSDKRet);
		}

		if((InfraredWorkMode != pcfg->IpcIspExConfig[0].nVarInfraredWorkMode) && (1 == bSupportVariableInfrared))
		{
			nRet = pCfgFile->SetValue(szSection, "InfraredWorkMode", pcfg->IpcIspExConfig[0].nVarInfraredWorkMode);
			if (nRet == 0 && pCfgFile->m_bReadOnlyCfgFlag == 1)
			{
				pcfg->IpcIspExConfig[0].nVarInfraredWorkMode = pCfgFile->GetValue(szSection, "InfraredWorkMode", 0);
			}	
			nSDKRet = ZWSys_SetInfraredWorkMode(hChannelHandle, pcfg->IpcIspExConfig[0].nVarInfraredWorkMode);
			MPU_CHK(0 == nSDKRet);
		}

		if((InfraredBrightness != pcfg->IpcIspExConfig[0].nVarInfraredBrightness) && (1 == bSupportVariableInfrared))
		{
			nRet = pCfgFile->SetValue(szSection, "InfraredBrightness", pcfg->IpcIspExConfig[0].nVarInfraredBrightness);
			if (nRet == 0 && pCfgFile->m_bReadOnlyCfgFlag == 1)
			{
				pcfg->IpcIspExConfig[0].nVarInfraredBrightness = pCfgFile->GetValue(szSection, "InfraredBrightness", 0);
			}
			nSDKRet = ZWSys_SetInfraredBrightness(hChannelHandle, pcfg->IpcIspExConfig[0].nVarInfraredBrightness);
			MPU_CHK(0 == nSDKRet);
		}
		
		if(nImageMode != pcfg->IpcIspExConfig[0].nImageMode)
		{
			if (0 == bSupportFaceNoExposure && 1==pcfg->IpcIspExConfig[0].nImageMode)
			{
				return IDVR_ERROR_UNSUPPORT;
			}

			if (0 == bSupportLicensePlate && 2==pcfg->IpcIspExConfig[0].nImageMode)
			{
				return IDVR_ERROR_UNSUPPORT;
			}			

			nRet = pCfgFile->SetValue(szSection, "ImageMode", pcfg->IpcIspExConfig[0].nImageMode);
			if (nRet == 0 && pCfgFile->m_bReadOnlyCfgFlag == 1)
			{
				pcfg->IpcIspExConfig[0].nImageMode = pCfgFile->GetValue(szSection, "ImageMode", 0);
			}
			nSDKRet = ZWSys_SetImageMode(hChannelHandle, pcfg->IpcIspExConfig[0].nImageMode);
			MPU_CHK(0 == nSDKRet);
		}
		#if defined (__IR_CUT_AUTO_MANUAL__)
		IrcutNightVisionTimeInfo info;
	
		/*全彩白*/
		if(WORKMODE_CUSTOM == pcfg->IpcIspExConfig[0].nColorWorkMode)
		{
			pCfgFile->SetValue(szSection, "ColorNightStartHour", pcfg->IpcIspExConfig[0].struColorWorkModeCustomStartTime.nHour);
			pCfgFile->SetValue(szSection, "ColorNightStartMin", pcfg->IpcIspExConfig[0].struColorWorkModeCustomStartTime.nMinute);
			pCfgFile->SetValue(szSection, "ColorNightStartSec", pcfg->IpcIspExConfig[0].struColorWorkModeCustomStartTime.nSecond);
			pCfgFile->SetValue(szSection, "ColorNightStopHour", pcfg->IpcIspExConfig[0].struColorWorkModeCustomStopTime.nHour);
			pCfgFile->SetValue(szSection, "ColorNightStopMin", pcfg->IpcIspExConfig[0].struColorWorkModeCustomStopTime.nMinute);
			pCfgFile->SetValue(szSection, "ColorNightStopSec", pcfg->IpcIspExConfig[0].struColorWorkModeCustomStopTime.nSecond);
	
			info.nStartHour = pcfg->IpcIspExConfig[0].struColorWorkModeCustomStartTime.nHour;
			info.nStartMin = pcfg->IpcIspExConfig[0].struColorWorkModeCustomStartTime.nMinute;
			info.nStartSec = pcfg->IpcIspExConfig[0].struColorWorkModeCustomStartTime.nSecond;
			
			info.nStopHour = pcfg->IpcIspExConfig[0].struColorWorkModeCustomStopTime.nHour;
			info.nStopMin = pcfg->IpcIspExConfig[0].struColorWorkModeCustomStopTime.nMinute;
			info.nStopSec = pcfg->IpcIspExConfig[0].struColorWorkModeCustomStopTime.nSecond; 
		}
		else if(WORKMODE_AUTO == pcfg->IpcIspExConfig[0].nColorWorkMode)
		{
			info.nStartHour = pCfgFile->GetValue(szSection, "ColorNightStartHour", 18);
			info.nStartMin = pCfgFile->GetValue(szSection, "ColorNightStartMin", 0);
			info.nStartSec = pCfgFile->GetValue(szSection, "ColorNightStartSec", 0);
			
			info.nStopHour = pCfgFile->GetValue(szSection, "ColorNightStopHour", 6);
			info.nStopMin = pCfgFile->GetValue(szSection, "ColorNightStopMin", 0);
			info.nStopSec = pCfgFile->GetValue(szSection, "ColorNightStopSec", 0); 
		}
		if(nColorWorkMode != pcfg->IpcIspExConfig[0].nColorWorkMode || WORKMODE_CUSTOM == pcfg->IpcIspExConfig[0].nColorWorkMode)
		{
			pCfgFile->SetValue(szSection, "nColorWorkMode", pcfg->IpcIspExConfig[0].nColorWorkMode);
			nSDKRet = ZWSys_EncSetColorNightMode(hChannelHandle, (NIGHTVISIONWORKMODE_T)pcfg->IpcIspExConfig[0].nColorWorkMode, &info);
			MPU_CHK(0 == nSDKRet);
		}

		if(nNormalWhiteLightOffLimit != pcfg->IpcIspExConfig[0].nNormalWhiteLightOffLimit || nNormalWhiteLightOnLimit != pcfg->IpcIspExConfig[0].nNormalWhiteLightOnLimit)
		{
			pCfgFile->SetValue(szSection, "NormalWhiteLightOffLimit", pcfg->IpcIspExConfig[0].nNormalWhiteLightOffLimit);
			pCfgFile->SetValue(szSection, "NormalWhiteLightOnLimit", pcfg->IpcIspExConfig[0].nNormalWhiteLightOnLimit);
			nSDKRet = ZWSys_SetNormalWhiteLightThreshold(hChannelHandle, pcfg->IpcIspExConfig[0].nNormalWhiteLightOnLimit, pcfg->IpcIspExConfig[0].nNormalWhiteLightOffLimit);
			MPU_CHK(0 == nSDKRet);
		}

        /*无极白*/
        if(WORKMODE_CUSTOM == pcfg->IpcIspExConfig[0].nVarWhiteControlMode)
        {
            pCfgFile->SetValue(szSection, "VarWhiteStartHour", pcfg->IpcIspExConfig[0].struVarWhiteModeStartTime.nHour);
			pCfgFile->SetValue(szSection, "VarWhiteStartMin", pcfg->IpcIspExConfig[0].struVarWhiteModeStartTime.nMinute);
			pCfgFile->SetValue(szSection, "VarWhiteStartSec", pcfg->IpcIspExConfig[0].struVarWhiteModeStartTime.nSecond);
			pCfgFile->SetValue(szSection, "VarWhiteStopHour", pcfg->IpcIspExConfig[0].struVarWhiteModeStopTime.nHour);
			pCfgFile->SetValue(szSection, "VarWhiteStopMin", pcfg->IpcIspExConfig[0].struVarWhiteModeStopTime.nMinute);
			pCfgFile->SetValue(szSection, "VarWhiteStopSec", pcfg->IpcIspExConfig[0].struVarWhiteModeStopTime.nSecond);

            info.nStartHour = pcfg->IpcIspExConfig[0].struVarWhiteModeStartTime.nHour;
            info.nStartMin = pcfg->IpcIspExConfig[0].struVarWhiteModeStartTime.nMinute;
            info.nStartSec = pcfg->IpcIspExConfig[0].struVarWhiteModeStartTime.nSecond;
            
            info.nStopHour = pcfg->IpcIspExConfig[0].struVarWhiteModeStopTime.nHour;
            info.nStopMin = pcfg->IpcIspExConfig[0].struVarWhiteModeStopTime.nMinute;
            info.nStopSec = pcfg->IpcIspExConfig[0].struVarWhiteModeStopTime.nSecond; 
        }
        else if(WORKMODE_AUTO == pcfg->IpcIspExConfig[0].nVarWhiteControlMode)
        {
            info.nStartHour = pCfgFile->GetValue(szSection, "VarWhiteStartHour", 18);
            info.nStartMin = pCfgFile->GetValue(szSection, "VarWhiteStartMin", 0);
            info.nStartSec = pCfgFile->GetValue(szSection, "VarWhiteStartSec", 0);
            
            info.nStopHour = pCfgFile->GetValue(szSection, "VarWhiteStopHour", 23);
            info.nStopMin = pCfgFile->GetValue(szSection, "VarWhiteStopMin", 0);
            info.nStopSec = pCfgFile->GetValue(szSection, "VarWhiteStopSec", 0); 
        }
#if 0
		if(nVarWhiteControlMode != pcfg->IpcIspExConfig[0].nVarWhiteControlMode || WORKMODE_CUSTOM == pcfg->IpcIspExConfig[0].nVarWhiteControlMode)
		{
#endif
		    if(nVarWhiteControlMode != pcfg->IpcIspExConfig[0].nVarWhiteControlMode)
	        {      
				nRet = pCfgFile->SetValue(szSection, "nVarWhiteControlMode", pcfg->IpcIspExConfig[0].nVarWhiteControlMode);
				if (nRet == 0 && pCfgFile->m_bReadOnlyCfgFlag == 1)
				{
					pcfg->IpcIspExConfig[0].nVarWhiteControlMode = pCfgFile->GetValue(szSection, "nVarWhiteControlMode", 0);
				}
				nVarWhiteControlMode = pcfg->IpcIspExConfig[0].nVarWhiteControlMode;
	        }

	        if(nVarWhiteWorkMode != pcfg->IpcIspExConfig[0].nVarWhiteWorkMode)
	        {
				nRet = pCfgFile->SetValue(szSection, "nVarWhiteWorkMode", pcfg->IpcIspExConfig[0].nVarWhiteWorkMode);
				if (nRet == 0 && pCfgFile->m_bReadOnlyCfgFlag == 1)
				{
					pcfg->IpcIspExConfig[0].nVarWhiteWorkMode = pCfgFile->GetValue(szSection, "nVarWhiteWorkMode", 0);
				}
				nVarWhiteWorkMode = pcfg->IpcIspExConfig[0].nVarWhiteWorkMode;
	        }

	        if(nVarWhiteBrightness != pcfg->IpcIspExConfig[0].nVarWhiteBrightness)
	        {
				nRet = pCfgFile->SetValue(szSection, "nVarWhiteBrightness", pcfg->IpcIspExConfig[0].nVarWhiteBrightness);
				if (nRet == 0 && pCfgFile->m_bReadOnlyCfgFlag == 1)
				{
					pcfg->IpcIspExConfig[0].nVarWhiteBrightness = pCfgFile->GetValue(szSection, "nVarWhiteBrightness", 0);
				}
				nVarWhiteBrightness = pcfg->IpcIspExConfig[0].nVarWhiteBrightness;
	        }

	        if(nVarWhiteLightOnThreshold != pcfg->IpcIspExConfig[0].nVarWhiteLightOnThreshold)
	        {
	            pCfgFile->SetValue(szSection, "nVarWhiteLightOnThreshold", pcfg->IpcIspExConfig[0].nVarWhiteLightOnThreshold);
	            nVarWhiteLightOnThreshold = pcfg->IpcIspExConfig[0].nVarWhiteLightOnThreshold;
	        }

	        if(nVarWhiteLightOffThreshold != pcfg->IpcIspExConfig[0].nVarWhiteLightOffThreshold)
	        {
	            pCfgFile->SetValue(szSection, "nVarWhiteLightOffThreshold", pcfg->IpcIspExConfig[0].nVarWhiteLightOffThreshold);
	            nVarWhiteLightOffThreshold = pcfg->IpcIspExConfig[0].nVarWhiteLightOffThreshold;
	        }
	        
			nSDKRet = ZWSys_EncSetVarWhiteMode(hChannelHandle, (NIGHTVISIONWORKMODE_T)nVarWhiteControlMode, (NIGHTVISIONCUSTOMMODE_T)nVarWhiteWorkMode, nVarWhiteBrightness);
			MPU_CHK(0 == nSDKRet);

	        nSDKRet = ZWSys_EncSetVarWhiteModeInfo(hChannelHandle, &info, nVarWhiteLightOnThreshold, nVarWhiteLightOffThreshold);
	        MPU_CHK(0 == nSDKRet);
	#if 0
		}
	#endif
		#endif

		#if defined (__DOUBLE_LIGHT_NO_LDR_CUSTOM__)
        if(nColorNightThreshold != pcfg->IpcIspExConfig[0].nColorNightThreshold)
        {
            pCfgFile->SetValue(szSection, "nColorNightThreshold", pcfg->IpcIspExConfig[0].nColorNightThreshold);
            nColorNightThreshold = pcfg->IpcIspExConfig[0].nColorNightThreshold;
        }

        if(nIntelligentThreshold != pcfg->IpcIspExConfig[0].nIntelligentThreshold)
        {
            pCfgFile->SetValue(szSection, "nIntelligentThreshold", pcfg->IpcIspExConfig[0].nIntelligentThreshold);
            nIntelligentThreshold = pcfg->IpcIspExConfig[0].nIntelligentThreshold;
        }
        
        nSDKRet = ZWSys_SetColorNightThreshold(hChannelHandle, nColorNightThreshold);
        MPU_CHK(0 == nSDKRet);

        nSDKRet = ZWSys_SetIntelligentThreshold(hChannelHandle, nIntelligentThreshold);
        MPU_CHK(0 == nSDKRet);
		#endif
        
        BoardSensorType_t SensorType;
        ZWSys_GetSensorType(&SensorType);
        snprintf(szSection, sizeof(szSection), "ISP_S%d", SensorType);
        /*去雾功能相关*/
		nDefogStrength    = pCfgFile->GetValue(szSection, "DefogStrength", 50);
		if(nDefogStrength != pcfg->IpcIspExConfig[0].nDeFrogStrengthValue)
		{
			pCfgFile->SetValue(szSection, "DefogStrength", pcfg->IpcIspExConfig[0].nDeFrogStrengthValue);
			nSDKRet = ZWSys_SetDefogStrength(hChannelHandle, pcfg->IpcIspExConfig[0].nDeFrogStrengthValue);
			MPU_CHK(0 == nSDKRet);
		}        

        /*抗闪强度*/
		nAntiFlashStrength    = pCfgFile->GetValue(szSection, "AntiFlashStrength", 50);
		if(nAntiFlashStrength != pcfg->IpcIspExConfig[0].iAntiFlashStrength)
		{
			pCfgFile->SetValue(szSection, "AntiFlashStrength", pcfg->IpcIspExConfig[0].iAntiFlashStrength);
			nSDKRet = ZWSys_SetAntiFlickerStrength(hChannelHandle, pcfg->IpcIspExConfig[0].iAntiFlashStrength);
			MPU_CHK(0 == nSDKRet);
		}  
	}
	return nRet;
}

/* 获取IrCut相关信息 */
static int GetIpcIrCutCfg(ZW_COM_ISP_EX * pcfg)
{
	int nRet = IDVR_OK;
	BoardSensorType_t SensorType;

	ZWSys_GetSensorType(&SensorType);	
	CConfig *pCfgFile = CConfig::GetInstance();
	if(NULL != pCfgFile)
	{
		char szSection[50] = {0};

		pcfg->bValid = 1;
		pcfg->IpcIspExConfig[0].bValid = 1;
		pcfg->IpcIspExConfig[0].nIRCutModeEnable = 1;
		pcfg->IpcIspExConfig[0].nSceneEnable = 1;//支持场景模式设置
		pcfg->IpcIspExConfig[0].nSupportIRcutReverse = 1;
		pcfg->IpcIspExConfig[0].nSupportDeFrog		   = 1;
		pcfg->IpcIspExConfig[0].nDeFrog		   = 1;

		snprintf(szSection, sizeof(szSection), "%s", "IR_CUT_cfg"); 	  
		/* 自动(主动模式) */
		pcfg->IpcIspExConfig[0].nInitactiveDayNightSensitivity = pCfgFile->GetValue(szSection, "nInitactiveDayNightSensitivity", 50);		 
		pcfg->IpcIspExConfig[0].nInitactiveSwitchTime		   = pCfgFile->GetValue(szSection, "nInitactiveSwitchTime", 5);   

		/* 自动(被动模式) */
		pcfg->IpcIspExConfig[0].nPassivityDayNightSensitivity  = pcfg->IpcIspExConfig[0].nInitactiveDayNightSensitivity;		
		pcfg->IpcIspExConfig[0].nPassivitySwitchTime		   = pCfgFile->GetValue(szSection, "nPassivitySwitchTime", 5); 

		/*获取图像场景模式*/
		pcfg->IpcIspExConfig[0].nSceneMode					   = pCfgFile->GetValue(szSection, "nSceneMode", 3);
		/*获取IRCUT是否反向*/
		pcfg->IpcIspExConfig[0].nEnableIRcutReverse 		   = pCfgFile->GetValue(szSection, "IRcutReverse", 0);
		/*获取智能全彩的延时时间*/
		pcfg->IpcIspExConfig[0].nColorNightLastTime 		   = pCfgFile->GetValue(szSection, "colorNightLastTime", 30);
		/*获取可变红外光的阀值*/	
		pcfg->IpcIspExConfig[0].nVariableInfraredThreshold		   = pCfgFile->GetValue(szSection, "InfraredThreshold", 30);
	
		/*获取可变白光的阀值*/		
		pcfg->IpcIspExConfig[0].nVariableWhiteThreshold 		   = pCfgFile->GetValue(szSection, "WhiteThreshold", 30);

		/*获取可变红外光工作模式*/	
		pcfg->IpcIspExConfig[0].nVarInfraredWorkMode		   = pCfgFile->GetValue(szSection, "InfraredWorkMode", 1);

		/*获取可变红外光亮度*/	
		pcfg->IpcIspExConfig[0].nVarInfraredBrightness		   = pCfgFile->GetValue(szSection, "InfraredBrightness", 100);

		/*获取图像模式*/	
		pcfg->IpcIspExConfig[0].nImageMode		   = pCfgFile->GetValue(szSection, "ImageMode", 0);
		#if defined (__IR_CUT_AUTO_MANUAL__)
		/*全彩白控制模式相关*/
		pcfg->IpcIspExConfig[0].nIsSupportColorOtherConfig = 1;
		pcfg->IpcIspExConfig[0].nColorWorkMode		   = pCfgFile->GetValue(szSection, "nColorWorkMode", 1);
	
		/*暂时写死 */
		pcfg->IpcIspExConfig[0].struColorWorkModeCustomStartTime.nYear			= 2019;
		pcfg->IpcIspExConfig[0].struColorWorkModeCustomStartTime.nMonth 		= 11;
		pcfg->IpcIspExConfig[0].struColorWorkModeCustomStartTime.nDay			= 11;
		pcfg->IpcIspExConfig[0].struColorWorkModeCustomStopTime.nYear			= 2019;
		pcfg->IpcIspExConfig[0].struColorWorkModeCustomStopTime.nMonth			= 11;
		pcfg->IpcIspExConfig[0].struColorWorkModeCustomStopTime.nDay			= 11;
		
		pcfg->IpcIspExConfig[0].struColorWorkModeCustomStartTime.nHour			= pCfgFile->GetValue(szSection, "ColorNightStartHour", 18);
		pcfg->IpcIspExConfig[0].struColorWorkModeCustomStartTime.nMinute		= pCfgFile->GetValue(szSection, "ColorNightStartMin", 0);
		pcfg->IpcIspExConfig[0].struColorWorkModeCustomStartTime.nSecond		= pCfgFile->GetValue(szSection, "ColorNightStartSec", 0);		 
		pcfg->IpcIspExConfig[0].struColorWorkModeCustomStopTime.nHour			= pCfgFile->GetValue(szSection, "ColorNightStopHour", 6);
		pcfg->IpcIspExConfig[0].struColorWorkModeCustomStopTime.nMinute 		= pCfgFile->GetValue(szSection, "ColorNightStopMin", 0);
		pcfg->IpcIspExConfig[0].struColorWorkModeCustomStopTime.nSecond 		= pCfgFile->GetValue(szSection, "ColorNightStopSec", 0);

		/*获取普通白光的开灯、关灯阀值*/		
        pcfg->IpcIspExConfig[0].nNormalWhiteLightOnLimit         = pCfgFile->GetValue(szSection, "NormalWhiteLightOnLimit", 50);
        pcfg->IpcIspExConfig[0].nNormalWhiteLightOffLimit         = pCfgFile->GetValue(szSection, "NormalWhiteLightOffLimit", 25);

		/*无极白控制模式相关*/
        pcfg->IpcIspExConfig[0].nIsSupportVarWhiteColorOtherConfig = 1;
        pcfg->IpcIspExConfig[0].nVarWhiteControlMode		   = pCfgFile->GetValue(szSection, "nVarWhiteControlMode", 1);
        pcfg->IpcIspExConfig[0].nVarWhiteWorkMode		   = pCfgFile->GetValue(szSection, "nVarWhiteWorkMode", 0);

        pcfg->IpcIspExConfig[0].nVarWhiteBrightness		   = pCfgFile->GetValue(szSection, "nVarWhiteBrightness", 20);
        pcfg->IpcIspExConfig[0].nVarWhiteLightOnThreshold		   = pCfgFile->GetValue(szSection, "nVarWhiteLightOnThreshold", 50);
        pcfg->IpcIspExConfig[0].nVarWhiteLightOffThreshold		   = pCfgFile->GetValue(szSection, "nVarWhiteLightOffThreshold", 10);

        /*暂时写死 */
        pcfg->IpcIspExConfig[0].struVarWhiteModeStartTime.nYear          = 2019;
        pcfg->IpcIspExConfig[0].struVarWhiteModeStartTime.nMonth         = 11;
        pcfg->IpcIspExConfig[0].struVarWhiteModeStartTime.nDay           = 11;
        pcfg->IpcIspExConfig[0].struVarWhiteModeStopTime.nYear           = 2019;
        pcfg->IpcIspExConfig[0].struVarWhiteModeStopTime.nMonth          = 11;
        pcfg->IpcIspExConfig[0].struVarWhiteModeStopTime.nDay            = 11;
        
        pcfg->IpcIspExConfig[0].struVarWhiteModeStartTime.nHour		    = pCfgFile->GetValue(szSection, "VarWhiteStartHour", 18);
        pcfg->IpcIspExConfig[0].struVarWhiteModeStartTime.nMinute	    = pCfgFile->GetValue(szSection, "VarWhiteStartMin", 0);
        pcfg->IpcIspExConfig[0].struVarWhiteModeStartTime.nSecond		= pCfgFile->GetValue(szSection, "VarWhiteStartSec", 0);        
        pcfg->IpcIspExConfig[0].struVarWhiteModeStopTime.nHour		    = pCfgFile->GetValue(szSection, "VarWhiteStopHour", 23);
        pcfg->IpcIspExConfig[0].struVarWhiteModeStopTime.nMinute		    = pCfgFile->GetValue(szSection, "VarWhiteStopMin", 0);
        pcfg->IpcIspExConfig[0].struVarWhiteModeStopTime.nSecond		    = pCfgFile->GetValue(szSection, "VarWhiteStopSec", 0);
		#endif

        #if defined (__DOUBLE_LIGHT_NO_LDR_CUSTOM__)
        pcfg->IpcIspExConfig[0].nColorNightThreshold		   = pCfgFile->GetValue(szSection, "nColorNightThreshold", 50);
        pcfg->IpcIspExConfig[0].nIntelligentThreshold		   = pCfgFile->GetValue(szSection, "nIntelligentThreshold", 50);
        #endif
        
        snprintf(szSection, sizeof(szSection), "ISP_S%d", SensorType);
        /*去雾功能相关*/
		pcfg->IpcIspExConfig[0].nDeFrogStrengthValue    = pCfgFile->GetValue(szSection, "DefogStrength", 50);

        /*抗闪强度*/
		pcfg->IpcIspExConfig[0].iAntiFlashStrength    = pCfgFile->GetValue(szSection, "AntiFlashStrength", 50);        
	}
	else
	{	
		//读取配置文件失败
		MPU_ERR("no CConfig instance\n");
		MPU_CHK(NULL != pCfgFile);
	}
	MPU_DBG("--GetIrCutCfg--nPassivityDayNightSensitivity=%d--nPassivitySwitchTime=%d \
	--nInitactiveDayNightSensitivity=%d--nInitactiveSwitchTime=%d-- \
	nColorNightLastTime=%d--nVariableInfraredThreshold=%d--nVariableWhiteThreshold=%d--nImageMode=%d--\n", 
			 pcfg->IpcIspExConfig[0].nPassivityDayNightSensitivity, 
			 pcfg->IpcIspExConfig[0].nPassivitySwitchTime, 
			 pcfg->IpcIspExConfig[0].nInitactiveDayNightSensitivity, 
			 pcfg->IpcIspExConfig[0].nInitactiveSwitchTime,
			 pcfg->IpcIspExConfig[0].nColorNightLastTime,
			 pcfg->IpcIspExConfig[0].nVariableInfraredThreshold,
			 pcfg->IpcIspExConfig[0].nVariableWhiteThreshold,
			 pcfg->IpcIspExConfig[0].nImageMode);
	
	#if defined (__IR_CUT_AUTO_MANUAL__)
	MPU_DBG("--GetIrCutCfg--nNormalWhiteLightOnLimit=%d--nNormalWhiteLightOffLimit=%d\n",
    pcfg->IpcIspExConfig[0].nNormalWhiteLightOnLimit,
    pcfg->IpcIspExConfig[0].nNormalWhiteLightOffLimit);
	#endif

	return nRet;
}

/* 设置两个流ROI相关信息 */
int SetIpcRoiCfg(LPHY_DVR_STREAM_ROI_CFG pcfg)
{
    int nRet = IDVR_OK;
    return nRet; 
}

/* 获取全部流ROI相关信息，需外部将缓冲区清零 */
int GetIpcRoiCfg(LPHY_DVR_STREAM_ROI_CFG pcfg)
{
	int nRet = IDVR_OK;
	return nRet;
}

static int GetSoftPower(LPHY_DVR_DEVICE_INFO_EX pcfg)
{
	int nRet = IDVR_OK, nSDKRet;

	DEVICEVERSION dv = {0};

	pcfg->bValid = 1;

    /* 如果wifi存在，则smartavc不能设置 */
	if (!access("/.wifi_if_exist", 0))
        pcfg->nEnableSmartAVC[0] = 0;
    else
    	pcfg->nEnableSmartAVC[0] = 0;
    unsigned int nAuthState = 0;
    unsigned char szAuthCode[256] = {0};
    ZWSys_GetAuthInfo(szAuthCode, &nAuthState);

    pcfg->cAuthState = nAuthState;
	pcfg->cUseSliceToSend = false;
	pcfg->iP6SSocketSendBuffer = 0;//系统默认值
	pcfg->iP6SSocketReceiveBuffer = 0;//系统默认值

#if defined (__IS_SUPPORT_APP_SHARE__)
    int rc = -1, cnt = 0;
	// 根据DeviceCap.xml的能力来决定支持设备分享能力
	if (-1 == g_bSupportDeviceShared)
	{
        //最多重试3次
        while(-1 == rc && cnt < 3)
        {
            rc = GetCapFromDeviceCapXML_Bool("Device:FunctionList:DeviceShared", false);
            cnt++;
            usleep(100*1000);
        }
		if (rc == -1)
		{
			OPEN_LOG_E("Get DeviceShared Failed\n");
			/*如果多次尝试获取能力集失败,则直接使用配置中的值*/
			CConfig *pAbilityCfg = NULL;
			pAbilityCfg = CConfig::NewInstance();
			if (0 == pAbilityCfg->LoadFileRO(CUSTOM_ABILITY_FILE))
			{
				if (pAbilityCfg->CheckKeyExist("FunctionSupport", "IsSupportDevShared"))
				{
					rc = pAbilityCfg->GetValue("FunctionSupport", "IsSupportDevShared", 0);
					OPEN_LOG_D("Use Ability Value DeviceShared = %d\n", rc);
				}
				else
				{
					OPEN_LOG_E("Ability DeviceShared Not Access\n");
				}
			}
			CConfig::ReleaseInstance(pAbilityCfg);
			pAbilityCfg = NULL;
		}
		g_bSupportDeviceShared = (rc == 1) ? 1 : 0;
	}
	pcfg->bDeviceShared = g_bSupportDeviceShared;	//支持设备分享
	// 根据DeviceCap.xml的能力来决定支持自助解绑能力
	if (-1 == g_bSupportSelfUnbind)
	{
        rc = -1;
		cnt = 0;
        //最多重试3次
        while(-1 == rc && cnt < 3)
        {
            rc = GetCapFromDeviceCapXML_Bool("Device:FunctionList:DeviceShareAndSelfUnbind", false);
            cnt++;
            usleep(100*1000);
        }
        g_bSupportSelfUnbind = (rc == 1) ? 1 : 0;

	}
	pcfg->bDeviceAutoUnbind = g_bSupportSelfUnbind;
#endif

#ifdef __MODULE_4G_SUPPORT__
    int bSupport4G = 0;
    ZWSys_Get4GSupportV2(&bSupport4G);
    if (bSupport4G)
    {
    	if (MPU_Net_IsP2PProductTestMode())
    	{
    		//4G P2P产测模式下，不支持分享
    		pcfg->bDeviceShared = 0;
    	}
    }
#endif

#if 1
	//DeviceUUID
	{
		char pUuidBuf[64] = {0};
		int iUuidLen = sizeof(pUuidBuf);
		nRet = GetDeviceUUID(pUuidBuf, iUuidLen);
        if (0 == nRet)
        {
		    memcpy(pcfg->szChipID, pUuidBuf, iUuidLen);
        }
	}
#endif
    //GB28181
    pcfg->cPositionCapability = 0;
    pcfg->cDownloadSpeed = 0;
    pcfg->cEncode = 1;
    pcfg->cReason = 0;

#if defined (__USE_STRONG_PW__)
	//探测用
    pcfg->nIsSupportStrongPassword = 1;
#endif
	//产测工具探测
	pcfg->nIsSupportProductTestToolV2 = 1;

	pcfg->nIsSupportTheSecondDeviceVersion = 1;
	//获取设备版本信息
	nSDKRet = MPU_PUB_GetDeviceVersion(&dv,0);
	MPU_CHK(0 == nSDKRet);
	memcpy(pcfg->cTheSecondDeviceVersion, dv.m_szSoftwareVersionPure, sizeof(pcfg->cTheSecondDeviceVersion));

    // 所有设备这个字段都要置1（协议才会 根据能力大小动态申请内存）
	pcfg->nIsGetCapFileSize = 1;

	return nRet;
}

static int SetIpcBasicCfg(ZW_IPC_BASIC * pcfg)
{
	int nRet = IDVR_OK, nSDKRet;
	BoardSensorType_t SensorType;
	int BoardInfo = 0;

	//本结构体是否有效，=0表示无效
	if (0 == pcfg->bValid)
	{
		MPU_DBG("0 == pcfg->bValid\n");
		return IDVR_CIU_STRUCT_UNVALID;
	}
	printf("SetIpcBasicCfg--nAntiFlickMode=%d--n3DDeNoise=%d--nBackLight=%d--nWideDynamicRange=%d--nMirrorMode=%d---\n",
		pcfg->nAntiFlickMode, pcfg->n3DDeNoise, pcfg->nBackLight, 
		pcfg->nWideDynamicRange, pcfg->nMirrorMode);
	//获取 Sensor类型
	ZWSys_GetSensorType(&SensorType);
    // 获取版型
    ZWSys_GetBoardInfo(&BoardInfo);
	//获取配置文件管理器实例指针
	CConfig *pCfgFile = CConfig::GetInstance();

	if (NULL != pCfgFile)
	{
		char szSection[30];
		char szMirrorModeSection[30] = {0};
		AE_T sensor_cfg;
		void *hChannelHandle=ZWSys_EncChannelOpen(0);

		snprintf(szSection, sizeof(szSection), "ISP_S%d", SensorType);
        snprintf(szMirrorModeSection, sizeof(szMirrorModeSection), "BoardID-%x", BoardInfo);
		if (hChannelHandle == NULL)
			MPU_ERR("ChannelOpen fail, !!!!!!!!!\n");

		if(pcfg->nMirrorMode != pCfgFile->GetValue(szMirrorModeSection, "VideoOrientation", 0))
		{
			pCfgFile->SetValue(szMirrorModeSection, "VideoOrientation", pcfg->nMirrorMode);
			FLIP_ROTATE_MODE_E nVideoOrientation;

			if (0 == pcfg->nMirrorMode)//正常
			{
				nVideoOrientation = FR_NORMAL;
			}
			else if (1 == pcfg->nMirrorMode)//镜像
			{
				nVideoOrientation = FR_HORI_FLIP;
			}
			else if (2 == pcfg->nMirrorMode)//翻转+ 镜像
			{
				nVideoOrientation = FR_VERT_FLIP;
			}
			else if (3 == pcfg->nMirrorMode)//翻转
			{
				nVideoOrientation = FR_ROTATE_180;
			}
			else
			{
				nVideoOrientation = FR_NORMAL;
			}
			nSDKRet = ZWSys_SetFlipRotateMode(hChannelHandle,nVideoOrientation);
			if (0 != nSDKRet)
			{
				MPU_ERR("%s: set video orientation  to dsp fail, return %d\n", szSection, nSDKRet);
				nRet = IDVR_ERROR_HISDK;
			}
		}
		
		if(pcfg->nAntiFlickMode != pCfgFile->GetValue(szSection, "InputFrequency", 0))
		{
			if (0 == pcfg->nAntiFlickMode)
			{
				sensor_cfg.antiFlicker = CMOS_FREQUENCY_50HZ;
			}
			else if (1 == pcfg->nAntiFlickMode)
			{
				sensor_cfg.antiFlicker = CMOS_FREQUENCY_60HZ;
			}
			else if (2 == pcfg->nAntiFlickMode)
			{
				sensor_cfg.antiFlicker = CMOS_FREQUENCY_CLOSE;
			}
			pCfgFile->SetValue(szSection, "InputFrequency", pcfg->nAntiFlickMode);

			sensor_cfg.MinShutterTime = 10000;
			snprintf(szSection, sizeof(szSection), "ISP_S%d_T%d", SensorType, USER_CUSTOM_TEMPLATE);
			sensor_cfg.MaxShutterTime = pCfgFile->GetValue(szSection, "MaxShutterSpeed", 30);
			sensor_cfg.sensorGain = pCfgFile->GetValue(szSection, "MaxAutoGain", 36);
			nSDKRet = ZWSys_SetAEParam(hChannelHandle,&sensor_cfg);
			if (0 != nSDKRet)
			{
				MPU_ERR("%s: ZWSys_SetAEParam fail, return %d\n", szSection, nSDKRet);
				nRet = IDVR_ERROR_HISDK;
			}

		}
		snprintf(szSection, sizeof(szSection), "ISP_S%d_T%d", SensorType, USER_CUSTOM_TEMPLATE);
		
		if(pcfg->n3DDeNoise != pCfgFile->GetValue(szSection, "DeNoiseMode", 0))
		{
			pCfgFile->SetValue(szSection, "DeNoiseMode", pcfg->n3DDeNoise);
			nSDKRet = ZWSys_SetMCTFStrength(hChannelHandle, pcfg->n3DDeNoise);
			if (0 != nSDKRet)
			{
				MPU_ERR("%s: ZWSys_SetMCTFStrength fail, return %d\n", szSection, nSDKRet);
				nRet = IDVR_ERROR_HISDK;
			}
		}

		if(pcfg->nBackLight != pCfgFile->GetValue(szSection, "YStrength", 0))
		{
			pCfgFile->SetValue(szSection, "YStrength", pcfg->nBackLight);
			nSDKRet = ZWSys_EncSetBacklightMode(hChannelHandle, pcfg->nBackLight);
			if (0 != nSDKRet)
			{
				MPU_ERR("%s: ZWSys_EncSetBacklightMode fail, return %d\n", szSection, nSDKRet);
				nRet = IDVR_ERROR_HISDK;
			}
		}

		if(pcfg->nWideDynamicRange != pCfgFile->GetValue(szSection, "CbStrength", 0))
		{
			pCfgFile->SetValue(szSection, "CbStrength", pcfg->nWideDynamicRange);
			nSDKRet = ZWSys_SetLocalExpMode(hChannelHandle,pcfg->nWideDynamicRange);
			if (0 != nSDKRet)
			{
				MPU_ERR("%s: ZWSys_SetLocalExpMode fail, return %d\n", szSection, nSDKRet);
				nRet = IDVR_ERROR_HISDK;
			}
		}
		
		pCfgFile->SaveFile();
		
	}
	return nRet;
}

static int GetIpcBasicCfg(ZW_IPC_BASIC * pcfg)
{
	int nRet = IDVR_OK;
	BoardSensorType_t SensorType;
    int BoardInfo = 0;
	
	CConfig *pCfgFile = CConfig::GetInstance();
	if(NULL != pCfgFile)
	{
		char szSection[30] = {0};
        char boardIDSection[24] = {0};

		pcfg->bValid = 1;

		//获取 Sensor类型
		ZWSys_GetSensorType(&SensorType);
		// 获取版型
		ZWSys_GetBoardInfo(&BoardInfo);
		
		snprintf(szSection, sizeof(szSection), "ISP_S%d", SensorType);
		
        // 镜像翻转需要区分版型
        snprintf(boardIDSection, sizeof(boardIDSection), "BoardID-%x", BoardInfo);
		pcfg->nMirrorMode = pCfgFile->GetValue(boardIDSection, "VideoOrientation", 0);

		pcfg->nAntiFlickMode = pCfgFile->GetValue(szSection, "InputFrequency", 1);
		snprintf(szSection, sizeof(szSection), "ISP_S%d_T%d", SensorType, USER_CUSTOM_TEMPLATE);
		pcfg->n3DDeNoise = pCfgFile->GetValue(szSection, "DeNoiseMode", 0);
		pcfg->nBackLight = pCfgFile->GetValue(szSection, "YStrength", 0);
		pcfg->nWideDynamicRange = pCfgFile->GetValue(szSection, "CbStrength", 0);
	}
	else
	{
		//读取配置文件失败
		MPU_ERR("no CConfig instance\n");
		MPU_CHK(NULL != pCfgFile);
	}
	MPU_DBG("--GetIpcBasicCfg--nAntiFlickMode=%d--n3DDeNoise=%d--nBackLight=%d--nWideDynamicRange=%d--nMirrorMode=%d---\n",
		pcfg->nAntiFlickMode, pcfg->n3DDeNoise, pcfg->nBackLight, 
		pcfg->nWideDynamicRange, pcfg->nMirrorMode);
	return nRet;
}

static int SetIpcIspCfg(ZW_IPC_ISP_CONFIG * pcfg)
{
	int nRet = IDVR_OK;
	///////////////////////////////////////////////////////////////////////////////
	//本结构体是否有效，=0表示无效
	if (0 == pcfg->bValid)
	{
		MPU_DBG("0 == pcfg->bValid\n");
		return IDVR_CIU_STRUCT_UNVALID;
	}

	SetIpcColorCfg(&pcfg->struColor);
	SetIpcExposureCfg(&pcfg->struExposure);
	SetIpcWhiteBlanceCfg(&pcfg->struWhiteBlance);
	SetIpcDayNightCfg(&pcfg->struDayAndNight);
	SetIpcBasicCfg(&pcfg->struBasic);

	return nRet;
}

static int GetIpcIspCfg(ZW_IPC_ISP_CONFIG * pcfg)
{
	int nRet = IDVR_OK;

	///////////////////////////////////////////////////////////////////////////////
	pcfg->bValid = 1;

	GetIpcColorCfg(&pcfg->struColor);
	GetIpcExposureCfg(&pcfg->struExposure);
	GetIpcWhiteBlanceCfg(&pcfg->struWhiteBlance);
	GetIpcDayNightCfg(&pcfg->struDayAndNight); 
	GetIpcBasicCfg(&pcfg->struBasic);

	return nRet;
}

static int setIpcRGB2YUV(LP_ZW_IPC_TONE_RGB2YUV pcfg)
{
	int nRet = IDVR_OK;
	IPC_TONE_RGB2YUV dataArray;

	//本结构体是否有效，=0表示无效
	if (0 == pcfg->bValid)
	{
		MPU_DBG("0 == pcfg->bValid\n");
		return IDVR_CIU_STRUCT_UNVALID;
	}
	dataArray.matrix_values[0] = pcfg->matrix_values[0];
	dataArray.matrix_values[1] = pcfg->matrix_values[1];
	dataArray.matrix_values[2] = pcfg->matrix_values[2];
	dataArray.matrix_values[3] = pcfg->matrix_values[3];
	dataArray.matrix_values[4] = pcfg->matrix_values[4];
	dataArray.matrix_values[5] = pcfg->matrix_values[5];
	dataArray.matrix_values[6] = pcfg->matrix_values[6];
	dataArray.matrix_values[7] = pcfg->matrix_values[7];
	dataArray.matrix_values[8] = pcfg->matrix_values[8];
	dataArray.y_offset = pcfg->y_offset;
	dataArray.u_offset = pcfg->u_offset;
	dataArray.v_offset = pcfg->v_offset;

	void *hChannelHandle=ZWSys_EncChannelOpen(0);
	
	nRet = ZWSys_SetRGB2YUVMatrix(hChannelHandle, 0, &dataArray);
	
	return nRet;
}

static int SetIpcRgb2yuvDefault()
{
	int nRet = IDVR_OK;
	IPC_TONE_RGB2YUV dataArray;

	void *hChannelHandle=ZWSys_EncChannelOpen(0);
	
	nRet = ZWSys_SetRGB2YUVMatrix(hChannelHandle, 1, &dataArray);
	
	return nRet;
}


static int getIpcRGB2YUV(LP_ZW_IPC_TONE_RGB2YUV pcfg)
{
	int nRet = IDVR_OK;
	IPC_TONE_RGB2YUV dataArray;

	pcfg->bValid = 1;

	void *hChannelHandle=ZWSys_EncChannelOpen(0);
	
	nRet = ZWSys_GetRGB2YUVMatrix(hChannelHandle, &dataArray);
	
	pcfg->matrix_values[0] = dataArray.matrix_values[0];
	pcfg->matrix_values[1] = dataArray.matrix_values[1];
	pcfg->matrix_values[2] = dataArray.matrix_values[2];
	pcfg->matrix_values[3] = dataArray.matrix_values[3];
	pcfg->matrix_values[4] = dataArray.matrix_values[4];
	pcfg->matrix_values[5] = dataArray.matrix_values[5];
	pcfg->matrix_values[6] = dataArray.matrix_values[6];
	pcfg->matrix_values[7] = dataArray.matrix_values[7];
	pcfg->matrix_values[8] = dataArray.matrix_values[8];
	pcfg->y_offset = dataArray.y_offset;
	pcfg->u_offset = dataArray.u_offset;
	pcfg->v_offset = dataArray.v_offset;
	
	return nRet;
}

static int SetIpcToneIsp(LP_ZW_IPC_TONE_ISP pcfg)
{
	int nRet = IDVR_OK;

	//本结构体是否有效，=0表示无效
	if (0 == pcfg->bValid)
	{
		MPU_DBG("0 == pcfg->bValid\n");
		return IDVR_CIU_STRUCT_UNVALID;
	}

	setIpcRGB2YUV(&pcfg->struRGB2YUV);

	return nRet;
}

/*
*isRgb2yuvDefault:0只获取参数，1恢复默认参数并获取
*/
static int GetIpcToneIsp(LP_ZW_IPC_TONE_ISP pcfg)
{
	int nRet = IDVR_OK;

	pcfg->bValid = 1;

	getIpcRGB2YUV(&pcfg->struRGB2YUV);

	return nRet;
}

static int SetIpcDefaultCfg()
{
	int i,nRet = IDVR_OK, nSDKRet;

	BoardSensorType_t SensorType;
	
	//获取配置文件管理器实例指针
	CConfig *pCfgFile = CConfig::GetInstance();

	MPU_DBG("SET ISP DEFAULT CONFIG!\n");

	char szSection[50];
	AE_T sensor_cfg;

	//获取 Sensor类型
	ZWSys_GetSensorType(&SensorType);
	
	sensor_cfg.antiFlicker = CMOS_FREQUENCY_60HZ;
	sensor_cfg.MinShutterTime = 10000;
	sensor_cfg.MaxShutterTime = 30;
	sensor_cfg.sensorGain = 0;

	WHITEBALANCE_T WhiteBalance;
	WhiteBalance.WBEnable = 1;
	WhiteBalance.WBWorkMode = 6;
	WhiteBalance.RedGain = 50;
	WhiteBalance.BlueGain = 50;
	int dayNightStartHour=7;
	int dayNightStartMin=0;
	int dayNightStartSec=0;
	int dayNightStopHour=19;
	int dayNightStopMin=0;
	int dayNightStopSec=0;

	int BoardInfo;
	char boardIDSection[24];

	int AutoChangeBright = 1;
	int DcIris = 1;
	int MCTFStrength = 0;
	int BackLight = 0;
	int WideDynamicRange = 0;
	int DayNightMode = 0;
	FLIP_ROTATE_MODE_E MirrorMode = FR_NORMAL;
	int nChroma_Scale = 1;//0 for tv, 1 for pc

    int PassivitySensitivity     = 50;               /* 被动模式灵敏度 */
    int PassivityTimeGetFromCfg  = 5;                /* 主动模式切换时间 */
    int InitactiveSensitivity    = 50;               /* 被动模式灵敏度 */
    int InitactiveTimeGetFromCfg = 5;                /* 被动模式切换时间 */
	int sceneModeDefault = 3;
	int colorNightLastTime       = 30;
	int nInfraredThreshold = 30;
	int InfraredWorkMode = 0;
	int InfraredBrightness = 90;
	int nWhiteThreshold = 30;
	int nNormalWhiteLightOnLimit = 50;
	int nNormalWhiteLightOffLimit = 25;   
	int nImageMode = 0;
	int nDefogStrength = 50;
	unsigned int Bright;
	unsigned int Contrast;
	unsigned int Saturation;
	unsigned int Chorma;
	unsigned int Sharpness;
	int nAntiFlashStrength = 50;
	char temp[100];

    int nColorWorkMode;
	int colorNightStartHour=18;
	int colorNightStartMin=0;
	int colorNightStartSec=0;
	int colorNightStopHour=23;
	int colorNightStopMin=0;
	int colorNightStopSec=0;

	int nVarWhiteControlMode= 1;
    int nVarWhiteWorkMode = 0;
    int nVarWhiteBrightness = 20;
    int nVarWhiteLightOnThreshold = 50;
    int nVarWhiteLightOffThreshold = 10;  
	int VarWhiteStartHour=18;
	int VarWhiteStartMin=0;
	int VarWhiteStartSec=0;
	int VarWhiteStopHour=23;
	int VarWhiteStopMin=0;
	int VarWhiteStopSec=0;
#if defined (__DOUBLE_LIGHT_NO_LDR_CUSTOM__)
    int nColorNightThreshold = 0;
    int nIntelligentThreshold = 0;
#endif
	void *hChannelHandle=ZWSys_EncChannelOpen(0);
 
	//获取新的配置文件读取实例
	CConfig *pDefaultCfg = CConfig::NewInstance();
	IrcutTimeModeInfo info;

	#if defined (__IR_CUT_AUTO_MANUAL__)
	IrcutNightVisionTimeInfo timeInfo;
	IrcutNightVisionTimeInfo whitetimeInfo;
	#endif

	snprintf(szSection, sizeof(szSection), "ISP_S%d", SensorType);
	
	if(NULL != pDefaultCfg)
	{
		//从缺省配置文件中读取
		pDefaultCfg->LoadFileRO(IDVR_DEFAULT_CONFIG_FILE_PATH);

		AutoChangeBright = pDefaultCfg->GetValue(szSection, "AutoChangeBright", 1);
		sensor_cfg.antiFlicker = (CmosFrequencyType_t)(pDefaultCfg->GetValue(szSection, "InputFrequency", 1));		
		nDefogStrength    = pDefaultCfg->GetValue(szSection, "DefogStrength", 50);
		nAntiFlashStrength    = pDefaultCfg->GetValue(szSection, "AntiFlashStrength", 50);
		dayNightStartHour = pDefaultCfg->GetValue(szSection, "DayNightStartHour", 7);
		dayNightStartMin = pDefaultCfg->GetValue(szSection, "DayNightStartMin", 0);
		dayNightStartSec = pDefaultCfg->GetValue(szSection, "DayNightStartSec", 0);
		dayNightStopHour = pDefaultCfg->GetValue(szSection, "DayNightStopHour", 19);
		dayNightStopMin = pDefaultCfg->GetValue(szSection, "DayNightStopMin", 0);
		dayNightStopSec = pDefaultCfg->GetValue(szSection, "DayNightStopSec", 0);
		/* 翻转需要根据版型来 */
		// MirrorMode = (FLIP_ROTATE_MODE_E)(pDefaultCfg->GetValue(szSection, "VideoOrientation", 0));

	}

	//根据版型来修改默认配置参数
	ZWSys_GetBoardInfo(&BoardInfo);
	memset(boardIDSection, 0, sizeof(boardIDSection));
	snprintf(boardIDSection, sizeof(boardIDSection), "BoardID-%x", BoardInfo);

    DayNightMode = pDefaultCfg->GetValue(boardIDSection, "VideoColor", 0);
	InitactiveSensitivity	 = pDefaultCfg->GetValue(boardIDSection, "nInitactiveDayNightSensitivity", 50); 	
	PassivitySensitivity	= InitactiveSensitivity;
	nInfraredThreshold	 = pDefaultCfg->GetValue(boardIDSection, "InfraredThreshold", 50); 
	InfraredWorkMode	 = pDefaultCfg->GetValue(boardIDSection, "InfraredWorkMode", 1); 
	InfraredBrightness	 = pDefaultCfg->GetValue(boardIDSection, "InfraredBrightness", 100); 
	nWhiteThreshold	 = pDefaultCfg->GetValue(boardIDSection, "WhiteThreshold", 50); 
	nImageMode = pDefaultCfg->GetValue(boardIDSection, "ImageMode", 0);
	colorNightLastTime = pDefaultCfg->GetValue(boardIDSection, "colorNightLastTime", 30);
	nDefogStrength    = pDefaultCfg->GetValue(boardIDSection, "DefogStrength", nDefogStrength);
	MirrorMode = (FLIP_ROTATE_MODE_E)(pDefaultCfg->GetValue(boardIDSection, "VideoOrientation", 0));
	#if defined (__IR_CUT_AUTO_MANUAL__)
    nVarWhiteLightOnThreshold = pDefaultCfg->GetValue(boardIDSection, "nVarWhiteLightOnThreshold", 50);
    nVarWhiteLightOffThreshold = pDefaultCfg->GetValue(boardIDSection, "nVarWhiteLightOffThreshold", 10);
    #endif
	
	snprintf(szSection, sizeof(szSection), "ISP_S%d", SensorType);
	pCfgFile->SetValue(szSection, "AutoChangeBright", AutoChangeBright);
	pCfgFile->SetValue(szSection, "InputFrequency", sensor_cfg.antiFlicker);
	if (DayNightMode >= 0)
	{
		nRet = pCfgFile->SetValue(szSection, "VideoColor", DayNightMode);
		if (nRet == 0 && pCfgFile->m_bReadOnlyCfgFlag == 1)
		{
			DayNightMode = pCfgFile->GetValue(szSection, "VideoColor", 0);
		}
	}
	pCfgFile->SetValue(szSection, "DefogStrength", nDefogStrength);
	pCfgFile->SetValue(szSection, "AntiFlashStrength", nAntiFlashStrength);
	pCfgFile->SetValue(szSection, "DayNightStartHour", dayNightStartHour);
	pCfgFile->SetValue(szSection, "DayNightStartMin", dayNightStartMin);
	pCfgFile->SetValue(szSection, "DayNightStartSec", dayNightStartSec);
	pCfgFile->SetValue(szSection, "DayNightStopHour", dayNightStopHour);
	pCfgFile->SetValue(szSection, "DayNightStopMin", dayNightStopMin);
	pCfgFile->SetValue(szSection, "DayNightStopSec", dayNightStopSec);
	pCfgFile->SetValue(boardIDSection, "VideoOrientation", MirrorMode);

	snprintf(szSection, sizeof(szSection), "%s", "IR_CUT_cfg");
	InitactiveTimeGetFromCfg = pDefaultCfg->GetValue(szSection, "nInitactiveSwitchTime", 5);
	sceneModeDefault		 = pDefaultCfg->GetValue(szSection, "nSceneMode", 3);
	PassivityTimeGetFromCfg  = pDefaultCfg->GetValue(szSection, "nPassivitySwitchTime", 5);

#if defined (__IR_CUT_AUTO_MANUAL__)
	nNormalWhiteLightOnLimit	 = pDefaultCfg->GetValue(boardIDSection, "NormalWhiteLightOnLimit", 50); 
	nNormalWhiteLightOffLimit	 = pDefaultCfg->GetValue(boardIDSection, "NormalWhiteLightOffLimit", 25); 

	pCfgFile->SetValue(szSection, "NormalWhiteLightOnLimit", nNormalWhiteLightOnLimit);
	pCfgFile->SetValue(szSection, "NormalWhiteLightOffLimit", nNormalWhiteLightOffLimit);

	nColorWorkMode			 = pDefaultCfg->GetValue(boardIDSection, "nColorWorkMode", 1);
	colorNightStartHour = pDefaultCfg->GetValue(boardIDSection, "ColorNightStartHour", 18);
	colorNightStartMin = pDefaultCfg->GetValue(boardIDSection, "ColorNightStartMin", 0);
	colorNightStartSec = pDefaultCfg->GetValue(boardIDSection, "ColorNightStartSec", 0);
	colorNightStopHour = pDefaultCfg->GetValue(boardIDSection, "ColorNightStopHour", 6);
	colorNightStopMin = pDefaultCfg->GetValue(boardIDSection, "ColorNightStopMin", 0);
	colorNightStopSec = pDefaultCfg->GetValue(boardIDSection, "ColorNightStopSec", 0);

	pCfgFile->SetValue(szSection, "nColorWorkMode", nColorWorkMode); 
	pCfgFile->SetValue(szSection, "ColorNightStartHour", colorNightStartHour); 
	pCfgFile->SetValue(szSection, "ColorNightStartMin", colorNightStartMin); 
	pCfgFile->SetValue(szSection, "ColorNightStartSec", colorNightStartSec); 
	pCfgFile->SetValue(szSection, "ColorNightStopHour", colorNightStopHour);
	pCfgFile->SetValue(szSection, "ColorNightStopMin", colorNightStopMin);
	pCfgFile->SetValue(szSection, "ColorNightStopSec", colorNightStopSec);

	nVarWhiteControlMode           = pDefaultCfg->GetValue(boardIDSection, "nVarWhiteControlMode", 1);
    nVarWhiteWorkMode = pDefaultCfg->GetValue(boardIDSection, "nVarWhiteWorkMode", 0);
    nVarWhiteBrightness = pDefaultCfg->GetValue(boardIDSection, "nVarWhiteBrightness", 20);
    nVarWhiteLightOnThreshold = pDefaultCfg->GetValue(boardIDSection, "nVarWhiteLightOnThreshold", 50);
	nVarWhiteLightOffThreshold = pDefaultCfg->GetValue(boardIDSection, "nVarWhiteLightOffThreshold", 10);    
    VarWhiteStartHour = pDefaultCfg->GetValue(boardIDSection, "VarWhiteStartHour", 18);
    VarWhiteStartMin = pDefaultCfg->GetValue(boardIDSection, "VarWhiteStartMin", 0);
    VarWhiteStartSec = pDefaultCfg->GetValue(boardIDSection, "VarWhiteStartSec", 0);
    VarWhiteStopHour = pDefaultCfg->GetValue(boardIDSection, "VarWhiteStopHour", 23);
    VarWhiteStopMin = pDefaultCfg->GetValue(boardIDSection, "VarWhiteStopMin", 0);
    VarWhiteStopSec = pDefaultCfg->GetValue(boardIDSection, "VarWhiteStopSec", 0);

	pCfgFile->SetValue(szSection, "nVarWhiteControlMode", nVarWhiteControlMode); 
    pCfgFile->SetValue(szSection, "nVarWhiteWorkMode", nVarWhiteWorkMode);
    pCfgFile->SetValue(szSection, "nVarWhiteBrightness", nVarWhiteBrightness);
    pCfgFile->SetValue(szSection, "nVarWhiteLightOnThreshold", nVarWhiteLightOnThreshold);
	pCfgFile->SetValue(szSection, "nVarWhiteLightOffThreshold", nVarWhiteLightOffThreshold);     
    pCfgFile->SetValue(szSection, "VarWhiteStartHour", VarWhiteStartHour); 
    pCfgFile->SetValue(szSection, "VarWhiteStartMin", VarWhiteStartMin); 
    pCfgFile->SetValue(szSection, "VarWhiteStartSec", VarWhiteStartSec); 
	pCfgFile->SetValue(szSection, "VarWhiteStopHour", VarWhiteStopHour);
	pCfgFile->SetValue(szSection, "VarWhiteStopMin", VarWhiteStopMin);
	pCfgFile->SetValue(szSection, "VarWhiteStopSec", VarWhiteStopSec);
#endif

#if defined (__DOUBLE_LIGHT_NO_LDR_CUSTOM__)
	nColorNightThreshold = pDefaultCfg->GetValue(boardIDSection, "nColorNightThreshold", 50);
	nIntelligentThreshold = pDefaultCfg->GetValue(boardIDSection, "nIntelligentThreshold", 50);

    pCfgFile->SetValue(szSection, "nColorNightThreshold", nColorNightThreshold);
    pCfgFile->SetValue(szSection, "nIntelligentThreshold", nIntelligentThreshold);
#endif
	nRet = pCfgFile->SetValue(szSection, "nInitactiveDayNightSensitivity", InitactiveSensitivity); 
	if (nRet == 0 && pCfgFile->m_bReadOnlyCfgFlag == 1)
	{
		InitactiveSensitivity = pCfgFile->GetValue(szSection, "nInitactiveDayNightSensitivity", 0); 
	}
    nRet = pCfgFile->SetValue(szSection, "nPassivityDayNightSensitivity", PassivitySensitivity); 
	if (nRet == 0 && pCfgFile->m_bReadOnlyCfgFlag == 1)
	{
		PassivitySensitivity = pCfgFile->GetValue(szSection, "nPassivityDayNightSensitivity", 0);
	}
    nRet = pCfgFile->SetValue(szSection, "nPassivitySwitchTime", PassivityTimeGetFromCfg); 
	if (nRet == 0 && pCfgFile->m_bReadOnlyCfgFlag == 1)
	{
		PassivityTimeGetFromCfg = pCfgFile->GetValue(szSection, "nPassivitySwitchTime", 0);
	}
    nRet = pCfgFile->SetValue(szSection, "nInitactiveSwitchTime", InitactiveTimeGetFromCfg); 
	if (nRet == 0 && pCfgFile->m_bReadOnlyCfgFlag == 1)
	{
		InitactiveTimeGetFromCfg = pCfgFile->GetValue(szSection, "nInitactiveSwitchTime", 0);
	}
	nRet = pCfgFile->SetValue(szSection, "nSceneMode", sceneModeDefault);
	if (nRet == 0 && pCfgFile->m_bReadOnlyCfgFlag == 1)
	{
		sceneModeDefault = pCfgFile->GetValue(szSection, "nSceneMode", 0);
	}
	nRet = pCfgFile->SetValue(szSection, "colorNightLastTime", colorNightLastTime);
	if (nRet == 0 && pCfgFile->m_bReadOnlyCfgFlag == 1)
	{
		colorNightLastTime = pCfgFile->GetValue(szSection, "colorNightLastTime", 0);
	}
	nRet = pCfgFile->SetValue(szSection, "InfraredThreshold", nInfraredThreshold);
	if (nRet == 0 && pCfgFile->m_bReadOnlyCfgFlag == 1)
	{
		nInfraredThreshold = pCfgFile->GetValue(szSection, "InfraredThreshold", 0);
	}	
	nRet = pCfgFile->SetValue(szSection, "InfraredWorkMode", InfraredWorkMode);
	if (nRet == 0 && pCfgFile->m_bReadOnlyCfgFlag == 1)
	{
		InfraredWorkMode = pCfgFile->GetValue(szSection, "InfraredWorkMode", 0);
	}
	nRet = pCfgFile->SetValue(szSection, "InfraredBrightness", InfraredBrightness);
	if (nRet == 0 && pCfgFile->m_bReadOnlyCfgFlag == 1)
	{
		InfraredBrightness = pCfgFile->GetValue(szSection, "InfraredBrightness", 0);
	}
	nRet = pCfgFile->SetValue(szSection, "WhiteThreshold", nWhiteThreshold);
	if (nRet == 0 && pCfgFile->m_bReadOnlyCfgFlag == 1)
	{
		nWhiteThreshold = pCfgFile->GetValue(szSection, "WhiteThreshold", 0);
	}
	nRet = pCfgFile->SetValue(szSection, "ImageMode", nImageMode);
	if (nRet == 0 && pCfgFile->m_bReadOnlyCfgFlag == 1)
	{
		nImageMode = pCfgFile->GetValue(szSection, "ImageMode", 0);
	}

	info.nStartHour = dayNightStartHour;
	info.nStartMin= dayNightStartMin;
	info.nStartSec= dayNightStartSec;
	info.nStopHour= dayNightStopHour;
	info.nStopMin= dayNightStopMin;
	info.nStopSec= dayNightStopSec;
#if defined (__IR_CUT_AUTO_MANUAL__)
	timeInfo.nStartHour = colorNightStartHour;
	timeInfo.nStartMin= colorNightStartMin;
	timeInfo.nStartSec= colorNightStartSec;
	timeInfo.nStopHour= colorNightStopHour;
	timeInfo.nStopMin= colorNightStopMin;
	timeInfo.nStopSec= colorNightStopSec;

	whitetimeInfo.nStartHour = VarWhiteStartHour;
	whitetimeInfo.nStartMin= VarWhiteStartMin;
	whitetimeInfo.nStartSec= VarWhiteStartSec;
	whitetimeInfo.nStopHour= VarWhiteStopHour;
	whitetimeInfo.nStopMin= VarWhiteStopMin;
	whitetimeInfo.nStopSec= VarWhiteStopSec;
#endif
	snprintf(szSection, sizeof(szSection), "ISP_S%d_T%d", SensorType, USER_CUSTOM_TEMPLATE);
	
	if(NULL != pDefaultCfg)
	{
		sensor_cfg.MaxShutterTime = pDefaultCfg->GetValue(szSection, "MaxShutterSpeed", 30);
		sensor_cfg.sensorGain = pDefaultCfg->GetValue(szSection, "MaxAutoGain", 0);
		DcIris = pDefaultCfg->GetValue(szSection, "LensType", 1);
		WhiteBalance.WBWorkMode = pDefaultCfg->GetValue(szSection, "WhiteBalanceMode", 6);
		MCTFStrength = pDefaultCfg->GetValue(szSection, "DeNoiseMode", 0);
		BackLight = pDefaultCfg->GetValue(szSection, "YStrength", 0);
		WideDynamicRange = pDefaultCfg->GetValue(szSection, "CbStrength", 0);
		nChroma_Scale = pDefaultCfg->GetValue(szSection, "Offset", 1);
	}	
	pCfgFile->SetValue(szSection, "MaxShutterSpeed", sensor_cfg.MaxShutterTime);
	pCfgFile->SetValue(szSection, "MaxAutoGain", sensor_cfg.sensorGain);
	pCfgFile->SetValue(szSection, "LensType", DcIris);
	pCfgFile->SetValue(szSection, "WhiteBalanceMode", WhiteBalance.WBWorkMode);
	pCfgFile->SetValue(szSection, "DeNoiseMode", MCTFStrength);
	pCfgFile->SetValue(szSection, "YStrength", BackLight);
	pCfgFile->SetValue(szSection, "CbStrength", WideDynamicRange);
	pCfgFile->SetValue(szSection, "Offset", nChroma_Scale);

	snprintf(szSection, sizeof(szSection), "ISP_S%d_T%d_WB%d", SensorType, USER_CUSTOM_TEMPLATE, 5);
	
	if(NULL != pDefaultCfg)
	{
		WhiteBalance.RedGain = pDefaultCfg->GetValue(szSection, "RedGain", 50);
		WhiteBalance.BlueGain = pDefaultCfg->GetValue(szSection, "BlueGain", 50);
	}
	pCfgFile->SetValue(szSection, "RedGain", WhiteBalance.RedGain);
	pCfgFile->SetValue(szSection, "BlueGain", WhiteBalance.BlueGain);

	
	nSDKRet = ZWSys_EncSetAutoChangeBright(hChannelHandle, AutoChangeBright);
	if (0 != nSDKRet)
	{ 
		MPU_ERR("%s: set Dc Iris fail, return %d\n", szSection, nSDKRet);
		nRet = IDVR_ERROR_HISDK;
	}

	for(i=0; i<1; i++)
	{
        snprintf(szSection, sizeof(szSection), "OpticsCfg%d", SensorType);
		pDefaultCfg->GetValue(szSection, "OpticsValueCh00_Seg0", "50,50,50,50,50", temp, sizeof(temp));
		sscanf(temp, "%d,%d,%d,%d,%d", 
			&Bright, &Contrast, &Saturation, &Chorma, &Sharpness);
		//亮度、色度、对比度、饱和度
		nSDKRet = MPU_AVL_Pic_SetPicParam(0, i, 0, Bright);
		MPU_CHK(0 == nSDKRet);
		
		nSDKRet = MPU_AVL_Pic_SetPicParam(0, i, 1, Contrast);
		MPU_CHK(0 == nSDKRet);
		
		nSDKRet = MPU_AVL_Pic_SetPicParam(0, i, 2, Saturation);
		MPU_CHK(0 == nSDKRet);
		
		nSDKRet = MPU_AVL_Pic_SetPicParam(0, i, 3, Chorma);
		MPU_CHK(0 == nSDKRet);

		//onvif过审，去掉限制，但对0值需要特殊处理，和协议交互是0，设备给sdk是1
        if (0 == Sharpness)
        {
		    nSDKRet = MPU_AVL_Pic_SetPicParam(0, i, 4, 1);
        }
        else
        {
		    nSDKRet = MPU_AVL_Pic_SetPicParam(0, i, 4, Sharpness);
        }
		MPU_CHK(0 == nSDKRet);
		//时间段开始和结束时间
		//nSDKRet = MPU_AVL_Pic_SetTimeCfg(nCh, nTimeSec, pcfg->nStartTime, pcfg->nStopTime);
		//MPU_CHK(0 == nSDKRet);
	}

    nSDKRet = ZWSys_EncSetIrCut(hChannelHandle,  PassivitySensitivity);
    MPU_CHK(0 == nSDKRet);

    nSDKRet = ZWSys_EncSetDayNightSwitchTime(hChannelHandle, PassivityTimeGetFromCfg);
    MPU_CHK(0 == nSDKRet);

    nSDKRet = ZWSys_EncSetIrcutInitiativeModeSensity(hChannelHandle,  InitactiveSensitivity);
    MPU_CHK(0 == nSDKRet);

    nSDKRet = ZWSys_EncSetDayNightInitiativeSwitchTime(hChannelHandle, InitactiveTimeGetFromCfg);
    MPU_CHK(0 == nSDKRet);

	nSDKRet = ZWSys_SetColorNightCfg(hChannelHandle, colorNightLastTime);
    MPU_CHK(0 == nSDKRet);

	nSDKRet = ZWSys_SetInfraredThreshold(hChannelHandle, nInfraredThreshold);
    MPU_CHK(0 == nSDKRet);

	nSDKRet = ZWSys_SetNormalWhiteLightThreshold(hChannelHandle, nNormalWhiteLightOnLimit, nNormalWhiteLightOffLimit);
    MPU_CHK(0 == nSDKRet);
	  
	nSDKRet = ZWSys_SetInfraredWorkMode(hChannelHandle, InfraredWorkMode);
    MPU_CHK(0 == nSDKRet);
	
	nSDKRet = ZWSys_SetInfraredBrightness(hChannelHandle, InfraredBrightness);
    MPU_CHK(0 == nSDKRet);

	nSDKRet = ZWSys_SetWhiteLightThreshold(hChannelHandle, nWhiteThreshold);
    MPU_CHK(0 == nSDKRet);

	nSDKRet = ZWSys_SetImageMode(hChannelHandle, nImageMode);
    MPU_CHK(0 == nSDKRet);

	nSDKRet = ZWSys_SetDefogStrength(hChannelHandle, nDefogStrength);
    MPU_CHK(0 == nSDKRet);

	nSDKRet = ZWSys_SetAntiFlickerStrength(hChannelHandle, nAntiFlashStrength);
    MPU_CHK(0 == nSDKRet);

    nSDKRet = ZWSys_SetColorType(hChannelHandle, (color_type_E)sceneModeDefault);
    MPU_CHK(0 == nSDKRet);
	/////////////////////////////////////////////////////////////////////////////////////////////////////

	nSDKRet = ZWSys_SetAEParam(hChannelHandle,&sensor_cfg);
	if (0 != nSDKRet)
	{
		MPU_ERR("%s: ZWSys_SetAEParam fail, return %d\n", szSection, nSDKRet);
		nRet = IDVR_ERROR_HISDK;
	}

	nSDKRet = ZWSys_SetDcIris(hChannelHandle, DcIris);
	if (0 != nSDKRet)
	{ 
		MPU_ERR("%s: set Dc Iris fail, return %d\n", szSection, nSDKRet);
		nRet = IDVR_ERROR_HISDK;
	}

	nSDKRet = ZWSys_SetWhiteBalance(hChannelHandle,&WhiteBalance);
	if (0 != nSDKRet)
	{
		MPU_ERR("%s: set white balance	to dsp fail, return %d\n", szSection, nSDKRet);
		nRet = IDVR_ERROR_HISDK;
	}

	
	nSDKRet = ZWSys_SetMCTFStrength(hChannelHandle, MCTFStrength);
	if (0 != nSDKRet)
	{
		MPU_ERR("%s: ZWSys_SetMCTFStrength fail, return %d\n", szSection, nSDKRet);
		nRet = IDVR_ERROR_HISDK;
	}

	nSDKRet = ZWSys_EncSetBacklightMode(hChannelHandle, BackLight);
	if (0 != nSDKRet)
	{
		MPU_ERR("%s: ZWSys_EncSetBacklightMode fail, return %d\n", szSection, nSDKRet);
		nRet = IDVR_ERROR_HISDK;
	}

	nSDKRet = ZWSys_SetLocalExpMode(hChannelHandle,WideDynamicRange);
	if (0 != nSDKRet)
	{
		MPU_ERR("%s: ZWSys_SetLocalExpMode fail, return %d\n", szSection, nSDKRet);
		nRet = IDVR_ERROR_HISDK;
	}

	nSDKRet = SetDayNightMode_Ex(hChannelHandle, (DAYNIGHTMODE_T)DayNightMode, &info);
	if (0 != nSDKRet)
	{
		MPU_ERR("%s: set video color to dsp fail, return %d\n", szSection, nSDKRet);
		nRet = IDVR_ERROR_HISDK;
	}

#if defined (__IR_CUT_AUTO_MANUAL__)
	nSDKRet = ZWSys_EncSetColorNightMode(hChannelHandle, (NIGHTVISIONWORKMODE_T)nColorWorkMode, &timeInfo);
	if (0 != nSDKRet)
	{
		MPU_ERR("%s: set video color night fail, return %d\n", szSection, nSDKRet);
		nRet = IDVR_ERROR_HISDK;
	}

	nSDKRet = ZWSys_EncSetVarWhiteMode(hChannelHandle, (NIGHTVISIONWORKMODE_T)nVarWhiteControlMode, (NIGHTVISIONCUSTOMMODE_T)nVarWhiteWorkMode, nVarWhiteBrightness);
	if (0 != nSDKRet)
	{
		MPU_ERR("%s: set video var white fail, return %d\n", szSection, nSDKRet);
		nRet = IDVR_ERROR_HISDK;
	}
	
	nSDKRet = ZWSys_EncSetVarWhiteModeInfo(hChannelHandle, &whitetimeInfo, nVarWhiteLightOnThreshold, nVarWhiteLightOffThreshold);
	if (0 != nSDKRet)
	{
		MPU_ERR("%s: set video var white fail, return %d\n", szSection, nSDKRet);
		nRet = IDVR_ERROR_HISDK;
	}
#endif

#if defined (__DOUBLE_LIGHT_NO_LDR_CUSTOM__)
    nSDKRet = ZWSys_SetColorNightThreshold(hChannelHandle, nColorNightThreshold);
    if (0 != nSDKRet)
    {
        MPU_ERR("%s: ZWSys_SetColorNightThreshold fail, return %d\n", szSection, nSDKRet);
        nRet = IDVR_ERROR_HISDK;
    }
    
    nSDKRet = ZWSys_SetIntelligentThreshold(hChannelHandle, nIntelligentThreshold);
    if (0 != nSDKRet)
    {
        MPU_ERR("%s: ZWSys_SetIntelligentThreshold fail, return %d\n", szSection, nSDKRet);
        nRet = IDVR_ERROR_HISDK;
    }
#endif

	FLIP_ROTATE_MODE_E nVideoOrientation;
	
	if (0 == MirrorMode)//正常
	{
		nVideoOrientation = FR_NORMAL;
	}
	else if (3 == MirrorMode)//翻转
	{
		nVideoOrientation = FR_ROTATE_180;
	}
	else if (1 == MirrorMode)//镜像
	{
		nVideoOrientation = FR_HORI_FLIP;
	}
	else if (2 == MirrorMode)//翻转+ 镜像
	{
		nVideoOrientation = FR_VERT_FLIP;
	}
	else
	{
		nVideoOrientation = FR_NORMAL;
	}

	nSDKRet = ZWSys_SetFlipRotateMode(hChannelHandle, nVideoOrientation);
	if (0 != nSDKRet)
	{
		MPU_ERR("%s: set video orientation  to dsp fail, return %d\n", szSection, nSDKRet);
		nRet = IDVR_ERROR_HISDK;
	}

	nSDKRet = ZWSys_SetChromaScale(hChannelHandle, (CHROMA_SCALE_E)nChroma_Scale);
	if (0 != nSDKRet)
	{
		MPU_ERR("%s: set video orientation  to dsp fail, return %d\n", szSection, nSDKRet);
		nRet = IDVR_ERROR_HISDK;
	}
	
	if(pDefaultCfg != NULL)
		CConfig::ReleaseInstance(pDefaultCfg);

	return nRet;
}

static int GetNightLedInfo(HY_DVR_NIGHT_LED_INFO_CFG *pcfg)
{
	int nRet = IDVR_OK;

	CConfig *pCfgFile = CConfig::GetInstance();
	if(NULL != pCfgFile)
	{
		pcfg->iValid = 1;
        pcfg->bEnable = 0;
		pcfg->bDuration = pCfgFile->GetValue("NightLed", "Duration", 15);
	}
	else
	{
		//读取配置文件失败
		MPU_ERR("no CConfig instance\n");
		MPU_CHK(NULL != pCfgFile);
	}

	printf("Get NightLed Info: bDuration = %d \n", pcfg->bDuration);
	return nRet;
}

static int SetNightLedInfo(const HY_DVR_NIGHT_LED_INFO_CFG *pcfg)
{
	int nRet = IDVR_OK;
    int nSDKRet = IDVR_OK;
    CConfig *pCfgFile = NULL;

	//本结构体是否有效，=0表示无效
	if (0 == pcfg->iValid)
	{
		MPU_DBG("0 == pcfg->bValid\n");
		return IDVR_CIU_STRUCT_UNVALID;
	}
	MPU_DBG("SetNightLedInfo: bDuration = %d , bEnable = [%d] \n", pcfg->bDuration, pcfg->bEnable);

    if (0 == pcfg->bEnable)
    {
        nSDKRet = ZWSys_EncSetNightLedMode(NIGNTLED_STOP);
        if (0 != nSDKRet)
        {
            MPU_ERR("set night led info to dsp fail\n");
            nRet = IDVR_ERROR_HISDK;
        }

    	if ((pcfg->bDuration < 1) || (pcfg->bDuration > 60))
    	{
    		MPU_DBG("--------SetNightLedInfo Not support Duration < 1s or > 60s --------\n");
    		return IDVR_ERROR_UNSUPPORT;
    	}

        nSDKRet = ZWSys_EncSetNightLedDuration(pcfg->bDuration);
        if (0 == nSDKRet)
        {
        	//获取配置文件管理器实例指针
        	pCfgFile = CConfig::GetInstance();
        	if (NULL != pCfgFile)
        	{
        		pCfgFile->SetValue("NightLed", "Duration", pcfg->bDuration);
        	}
        }
    }
    else
    {
        nSDKRet = ZWSys_EncSetNightLedMode(NIGNTLED_START);
        if (0 != nSDKRet)
        {
            MPU_ERR("set night led info to dsp fail\n");
            nRet = IDVR_ERROR_HISDK;
        }

        if ((pcfg->bDuration < 1) || (pcfg->bDuration > 60))
    	{
    		MPU_DBG("--------SetNightLedInfo Not support Duration < 1s or > 60s --------\n");
    		return IDVR_ERROR_UNSUPPORT;
    	}

        nSDKRet = ZWSys_EncSetNightLedDuration(pcfg->bDuration);
        if (0 == nSDKRet)
        {
        	//获取配置文件管理器实例指针
        	pCfgFile = CConfig::GetInstance();
        	if (NULL != pCfgFile)
        	{
        		pCfgFile->SetValue("NightLed", "Duration", pcfg->bDuration);
        	}
        }

        //暂停声光报警
        //ZWSys_SetPauseAudioAlarm();
    }

    return nRet;
}

int CIU_GetConfigure_ISP(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen)
{
	int nRet = IDVR_OK;

	MPU_DBG("ID %d, Major %d, Minor %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType);

	switch (dwMinorType)
	{
			//设置图像参数
		case NXP8850ISP_ALL:
			{
				///////////////////////////////////////////////////////////////////////////////
				//判断buf空间是否足够
				if (*pdwBufLen < NXP8850ISP_CFG_LEN)
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", NXP8850ISP_CFG_LEN, *pdwBufLen);
					break;
				}

				LPHY_DVR_NXP8850ISP_CFG pcfg = (LPHY_DVR_NXP8850ISP_CFG) pBuffer;

				///////////////////////////////////////////////////////////////////////////////
				//初始化输出缓冲区
				memset(pcfg, 0, NXP8850ISP_CFG_LEN);
				*pdwBufLen = NXP8850ISP_CFG_LEN;

				nRet = GetIspCfg(pcfg);
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

int  CIU_GetConfigure_IPC_ISP(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen)
{
	int nRet = IDVR_OK;

	MPU_DBG("ID %d, Major %d, Minor %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType);

	switch (dwMinorType)
	{
			//设置图像参数
		case IPC_ISPCFG_ALL:
			{
				///////////////////////////////////////////////////////////////////////////////
				//判断buf空间是否足够
				if (*pdwBufLen < sizeof(ZW_IPC_ISP_CONFIG))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(ZW_IPC_ISP_CONFIG), *pdwBufLen);
					break;
				}

				ZW_IPC_ISP_CONFIG *pcfg = (ZW_IPC_ISP_CONFIG *) pBuffer;

				///////////////////////////////////////////////////////////////////////////////
				//初始化输出缓冲区
				memset(pcfg, 0, sizeof(ZW_IPC_ISP_CONFIG));
				*pdwBufLen = sizeof(ZW_IPC_ISP_CONFIG);

				nRet = GetIpcIspCfg(pcfg);
			}
			break;
		//颜色配置
		case IPC_ISPCFG_COLOR:
			{
				if (*pdwBufLen < sizeof(ZW_IPC_COLOR))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(ZW_IPC_COLOR), *pdwBufLen);
					break;
				}
				ZW_IPC_COLOR *pcfg = (ZW_IPC_COLOR *) pBuffer;
				nRet = GetIpcColorCfg(pcfg);
			}
			break;
		//曝光配置
		case IPC_ISPCFG_EXPOSURE:
			{
				if (*pdwBufLen < sizeof(ZW_IPC_EXPOSURE))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(ZW_IPC_EXPOSURE), *pdwBufLen);
					break;
				}
				ZW_IPC_EXPOSURE *pcfg = (ZW_IPC_EXPOSURE *) pBuffer;
				nRet = GetIpcExposureCfg(pcfg);
			}
			break;
		// 白平衡配置
		case IPC_ISPCFG_WHITEBLANCE:
			{
				if (*pdwBufLen < sizeof(ZW_IPC_WHITE_BLANCE))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(ZW_IPC_WHITE_BLANCE), *pdwBufLen);
					break;
				}
				ZW_IPC_WHITE_BLANCE *pcfg = (ZW_IPC_WHITE_BLANCE *) pBuffer;
				nRet = GetIpcWhiteBlanceCfg(pcfg);
			}
			break;
		// 日夜切换配置
		case IPC_ISPCFG_DAY_NIGHT:
			{
				if (*pdwBufLen < sizeof(ZW_IPC_DAY_NIGHT))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(ZW_IPC_DAY_NIGHT), *pdwBufLen);
					break;
				}
				ZW_IPC_DAY_NIGHT * pcfg = (ZW_IPC_DAY_NIGHT *) pBuffer;
				nRet = GetIpcDayNightCfg(pcfg);
			}
			break;
		//基本参数配置
		case IPC_ISPCFG_BASIC:
			{
				if (*pdwBufLen < sizeof(ZW_IPC_BASIC))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(ZW_IPC_BASIC), *pdwBufLen);
					break;
				}
				ZW_IPC_BASIC * pcfg = (ZW_IPC_BASIC *) pBuffer;
				nRet = GetIpcBasicCfg(pcfg);
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

int CIU_SetConfigure_ISP(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, const BYTE * pBuffer, DWORD dwBufLen)
{
	int nRet = IDVR_OK;

	MPU_DBG("ID %d, Major %d, Minor %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType);

	switch (dwMinorType)
	{
			//设置图像参数
		case NXP8850ISP_ALL:
			{
				///////////////////////////////////////////////////////////////////////////////
				//判断buf空间是否足够
				if (dwBufLen < NXP8850ISP_CFG_LEN)
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", NXP8850ISP_CFG_LEN, dwBufLen);
					break;
				}

				LPHY_DVR_NXP8850ISP_CFG pcfg = (LPHY_DVR_NXP8850ISP_CFG) pBuffer;

				nRet = SetIspCfg(pcfg);
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

int CIU_GetConfigure_IR_CUT(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen)
{
    int nRet = IDVR_OK;

	MPU_DBG("ID %d, Major %d, Minor %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType);

	switch (dwMinorType)
	{
	    //获取IRCUT相关信息(灵敏度和切换时间)
		case COM_ISPCFG_EX_ALL:
			{ 				
			    //判断buf空间是否足够 
				if (*pdwBufLen < sizeof(ZW_COM_ISP_EX))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(ZW_COM_ISP_EX), *pdwBufLen);
					break;
				}
				ZW_COM_ISP_EX * pcfg = (ZW_COM_ISP_EX *) pBuffer;
				nRet = GetIpcIrCutCfg(pcfg); 
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

int CIU_SetConfigure_IR_CUT(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, const BYTE * pBuffer, DWORD dwBufLen)
{
    int nRet = IDVR_OK;

	MPU_DBG("ID %d, Major %d, Minor %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType);

	switch (dwMinorType)
	{ 
	 	//设置IrCut相关信息(灵敏度和切换时间)
		case COM_ISPCFG_EX_ALL:
		    {
              if (dwBufLen < sizeof(ZW_COM_ISP_EX))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(ZW_COM_ISP_EX), dwBufLen);
					break;
				}
				ZW_COM_ISP_EX * pcfg = (ZW_COM_ISP_EX *) pBuffer;
				nRet = SetIpcIrCutCfg(pcfg);
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

int CIU_GetConfigure_ROI(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, const BYTE * pBuffer, DWORD * pdwBufLen)
{
    int nRet = IDVR_OK;

	MPU_DBG("ID %d, Major %d, Minor %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType);

	switch (dwMinorType)
	{
        //获取ROI相关信息
		case ROI_CFG_ALL:
			{ 				
			    //判断buf空间是否足够 
				if (*pdwBufLen < sizeof(HY_DVR_STREAM_ROI_CFG))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_STREAM_ROI_CFG), *pdwBufLen);
					break;
				}
				LPHY_DVR_STREAM_ROI_CFG pcfg = (LPHY_DVR_STREAM_ROI_CFG) pBuffer;

                //初始化输出缓冲区
				memset(pcfg, 0, sizeof(HY_DVR_STREAM_ROI_CFG));
				*pdwBufLen = sizeof(HY_DVR_STREAM_ROI_CFG);
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

int CIU_GetConfigure_SOFT_POWER(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, const BYTE * pBuffer, DWORD * pdwBufLen)
{
    int nRet = IDVR_OK;

	MPU_DBG("ID %d, Major %d, Minor %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType);

	switch (dwMinorType)
	{
        //获取ROI相关信息
		case DEVICEINFO_EX_ALL:
			{ 				
			    //判断buf空间是否足够 
				if (*pdwBufLen < sizeof(HY_DVR_DEVICE_INFO_EX))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_DEVICE_INFO_EX), *pdwBufLen);
					break;
				}
				LPHY_DVR_DEVICE_INFO_EX pcfg = (LPHY_DVR_DEVICE_INFO_EX) pBuffer;

                //初始化输出缓冲区
				memset(pcfg, 0, sizeof(HY_DVR_DEVICE_INFO_EX));
				*pdwBufLen = sizeof(HY_DVR_DEVICE_INFO_EX);
#if defined (__IS_SUPPORT_WLAN__)
				nRet = MPU_WLAN_GetQrConfigureWlanSessionID(pcfg->bytSessionID, sizeof(pcfg->bytSessionID));
				if (0 == nRet)
				{
					pcfg->bValidSessionID = 1;
				}
#endif
				nRet = GetSoftPower(pcfg);
			}
			break;
		//获取定制信息
		case ONVIF_CUSTOMIZED_INFO:
			{
				//判断buf空间是否足够 
				if (*pdwBufLen < sizeof(HY_DVR_ONVIF_CUSTOMIZED))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_ONVIF_CUSTOMIZED), *pdwBufLen);
					break;
				}
				LPHY_DVR_ONVIF_CUSTOMIZED pcfg = (LPHY_DVR_ONVIF_CUSTOMIZED) pBuffer;
		
				//初始化输出缓冲区
				memset(pcfg, 0, sizeof(HY_DVR_ONVIF_CUSTOMIZED));
				*pdwBufLen = sizeof(HY_DVR_ONVIF_CUSTOMIZED);
				
				nRet = GetOnvifCustomMedia(pcfg);
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

int CIU_SetConfigure_ROI(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, const BYTE * pBuffer, DWORD dwBufLen)
{
    int nRet = IDVR_OK;

	MPU_DBG("ID %d, Major %d, Minor %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType);

	switch (dwMinorType)
	{ 
	    //设置ROI配置
		case ROI_CFG_ALL:
		    {
              if (dwBufLen < sizeof(HY_DVR_STREAM_ROI_CFG))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_STREAM_ROI_CFG), dwBufLen);
					break;
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
	MPU_DBG("ID %d, Major %d, Minor %d, return %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType, nRet);
	return nRet;

}

int CIU_GetConfigure_tone_ISP(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen)
{
    int nRet = IDVR_OK;

	MPU_DBG("ID %d, Major %d, Minor %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType);

	switch (dwMinorType)
	{
		//获取TONE_ISP
		case TONE_ISP_ALL:
			{ 				
			    //判断buf空间是否足够 
				if (*pdwBufLen < sizeof(ZW_IPC_TONE_ISP))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(ZW_IPC_TONE_ISP), *pdwBufLen);
					break;
				}
				LP_ZW_IPC_TONE_ISP pcfg = (LP_ZW_IPC_TONE_ISP) pBuffer;
				nRet = GetIpcToneIsp(pcfg); 
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

int CIU_SetConfigure_tone_ISP(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, const BYTE * pBuffer, DWORD dwBufLen)
{
    int nRet = IDVR_OK;

	MPU_DBG("ID %d, Major %d, Minor %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType);

	switch (dwMinorType)
	{ 
	 	//TONE ISP设置
		case TONE_ISP_ALL:
		    {
              if (dwBufLen < sizeof(ZW_IPC_TONE_ISP))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(ZW_IPC_TONE_ISP), dwBufLen);
					break;
				}
				LP_ZW_IPC_TONE_ISP pcfg = (LP_ZW_IPC_TONE_ISP) pBuffer;
				nRet = SetIpcToneIsp(pcfg);
		    }  
		    break;
			
		case TONE_ISP_RGB2YUV_DEFAULT:
			{
				nRet = SetIpcRgb2yuvDefault();
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


int CIU_GetConfigure_NVR_ISP(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen)
{
	int nRet = IDVR_OK;

	MPU_INFO("ID %d, Major %d, Minor %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType);

	switch (dwMinorType)
	{
			//设置图像参数
		case NVR_ISPCFG_ALL:
			{
				///////////////////////////////////////////////////////////////////////////////
				//判断buf空间是否足够
				if (*pdwBufLen < sizeof(ZW_NVR_ISP_CONFIG))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(ZW_NVR_ISP_CONFIG), *pdwBufLen);
					break;
				}

				ZW_NVR_ISP_CONFIG *pcfg = (ZW_NVR_ISP_CONFIG *) pBuffer;

				///////////////////////////////////////////////////////////////////////////////
				//初始化输出缓冲区
				memset(pcfg, 0, sizeof(ZW_NVR_ISP_CONFIG));
				*pdwBufLen = sizeof(ZW_NVR_ISP_CONFIG);
				
                pcfg->bValid = 1;
				nRet = GetIpcIspCfg(&(pcfg->IpcIspConfig[0]));
			}
			break;
		//颜色配置
		case NVR_ISPCFG_COLOR:
			{
				if (*pdwBufLen < sizeof(ZW_NVR_COLOR))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(ZW_NVR_COLOR), *pdwBufLen);
					break;
				}
				ZW_NVR_COLOR* pcfg = (ZW_NVR_COLOR *) pBuffer;
				pcfg->bValid = 1;
				nRet = GetIpcColorCfg(&(pcfg->struColor[0]));
			}
			break;
		//曝光配置
		case NVR_ISPCFG_EXPOSURE:
			{
				if (*pdwBufLen < sizeof(ZW_NVR_EXPOSURE))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(ZW_NVR_EXPOSURE), *pdwBufLen);
					break;
				}
				ZW_NVR_EXPOSURE* pcfg = (ZW_NVR_EXPOSURE*) pBuffer;
				pcfg->bValid = 1;
				nRet = GetIpcExposureCfg(&(pcfg->struExposure[0]));
			}
			break;
		// 白平衡配置
		case NVR_ISPCFG_WHITEBLANCE:
			{
				if (*pdwBufLen < sizeof(ZW_NVR_WHITE_BLANCE))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(ZW_NVR_WHITE_BLANCE), *pdwBufLen);
					break;
				}
				ZW_NVR_WHITE_BLANCE* pcfg = (ZW_NVR_WHITE_BLANCE*) pBuffer;
				pcfg->bValid = 1;
				nRet = GetIpcWhiteBlanceCfg(&(pcfg->struWhiteBlance[0]));
			}
			break;
		// 日夜切换配置
		case NVR_ISPCFG_DAY_NIGHT:
			{
				if (*pdwBufLen < sizeof(ZW_NVR_DAY_NIGHT))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(ZW_NVR_DAY_NIGHT), *pdwBufLen);
					break;
				}
				ZW_NVR_DAY_NIGHT* pcfg = (ZW_NVR_DAY_NIGHT*) pBuffer;
				pcfg->bValid = 1;
				nRet = GetIpcDayNightCfg(&(pcfg->struDayAndNight[0]));
			}
			break;
		//基本参数配置
		case NVR_ISPCFG_BASIC:
			{
				if (*pdwBufLen < sizeof(ZW_NVR_BASIC))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(ZW_NVR_BASIC), *pdwBufLen);
					break;
				}
				ZW_NVR_BASIC* pcfg = (ZW_NVR_BASIC*) pBuffer;
				pcfg->bValid = 1;
				nRet = GetIpcBasicCfg(&(pcfg->struBasic[0]));
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


int CIU_SetConfigure_IPC_ISP(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, const BYTE * pBuffer, DWORD dwBufLen)
{
	int nRet = IDVR_OK;

	MPU_DBG("ID %d, Major %d, Minor %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType);

	switch (dwMinorType)
	{
		//设置图像参数
		case IPC_ISPCFG_ALL:
			{
				///////////////////////////////////////////////////////////////////////////////
				//判断buf空间是否足够
				if (dwBufLen < sizeof(ZW_IPC_ISP_CONFIG))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(ZW_IPC_ISP_CONFIG), dwBufLen);
					break;
				}

				ZW_IPC_ISP_CONFIG * pcfg = (ZW_IPC_ISP_CONFIG *) pBuffer;
				nRet = SetIpcIspCfg(pcfg);
			}
			break;
		//颜色配置
		case IPC_ISPCFG_COLOR:
			{
				if (dwBufLen < sizeof(ZW_IPC_COLOR))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(ZW_IPC_COLOR), dwBufLen);
					break;
				}
				ZW_IPC_COLOR * pcfg = (ZW_IPC_COLOR *) pBuffer;
				nRet = SetIpcColorCfg(pcfg);
			}
			break;
		//曝光配置
		case IPC_ISPCFG_EXPOSURE:
			{
				if (dwBufLen < sizeof(ZW_IPC_EXPOSURE))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(ZW_IPC_EXPOSURE), dwBufLen);
					break;
				}
				ZW_IPC_EXPOSURE * pcfg = (ZW_IPC_EXPOSURE *) pBuffer;
				
				nRet = SetIpcExposureCfg(pcfg);
			}
			break;
		// 白平衡配置
		case IPC_ISPCFG_WHITEBLANCE:
			{
				if (dwBufLen < sizeof(ZW_IPC_WHITE_BLANCE))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(ZW_IPC_WHITE_BLANCE), dwBufLen);
					break;
				}
				ZW_IPC_WHITE_BLANCE * pcfg = (ZW_IPC_WHITE_BLANCE *) pBuffer;
				nRet = SetIpcWhiteBlanceCfg(pcfg);
			}
			break;
		// 日夜切换配置
		case IPC_ISPCFG_DAY_NIGHT:
			{
				if (dwBufLen < sizeof(ZW_IPC_DAY_NIGHT))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(ZW_IPC_DAY_NIGHT), dwBufLen);
					break;
				}
				ZW_IPC_DAY_NIGHT * pcfg = (ZW_IPC_DAY_NIGHT *) pBuffer;
				nRet = SetIpcDayNightCfg(pcfg);
			}
			break; 		
		//基本参数配置
		case IPC_ISPCFG_BASIC:
			{
				if (dwBufLen < sizeof(ZW_IPC_BASIC))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(ZW_IPC_BASIC), dwBufLen);
					break;
				}
				ZW_IPC_BASIC * pcfg = (ZW_IPC_BASIC *) pBuffer;
				nRet = SetIpcBasicCfg(pcfg);
			}
			break;
		//恢复默认
		case IPC_ISPCFG_DEFAULT:
			{ 
				nRet = SetIpcDefaultCfg();
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


int CIU_SetConfigure_NVR_ISP(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, const BYTE * pBuffer, DWORD dwBufLen)
{
	int nRet = IDVR_OK;

	MPU_INFO("ID %d, Major %d, Minor %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType);

	switch (dwMinorType)
	{
		//设置图像参数(无效)
		case NVR_ISPCFG_ALL:
			{
				///////////////////////////////////////////////////////////////////////////////
				//判断buf空间是否足够
				if (dwBufLen < sizeof(ZW_NVR_ISP_CONFIG))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(ZW_NVR_ISP_CONFIG), dwBufLen);
					break;
				}

				ZW_NVR_ISP_CONFIG* pcfg = (ZW_NVR_ISP_CONFIG*) pBuffer;
                if (0 == pcfg->bValid)
                {
                    MPU_DBG("0 == pcfg->bValid\n");
                    break;
                }
			
				nRet = SetIpcIspCfg(&(pcfg->IpcIspConfig[0]));
			}
			break;
		//颜色配置(图像基本参数)
		case NVR_ISPCFG_COLOR:
			{
				if (dwBufLen < sizeof(ZW_NVR_COLOR))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(ZW_NVR_COLOR), dwBufLen);
					break;
				}
				ZW_NVR_COLOR* pcfg = (ZW_NVR_COLOR*) pBuffer;
				 if (0 == pcfg->bValid)
                {
                    MPU_DBG("0 == pcfg->bValid\n");
                    break;
                }
				printf("AutoChangeBright = %d  nBright = %d nContrast = %d nSaturation = %d nChroma = %d  nSharpness = %d \n",
				pcfg->struColor[0].nAutoChangeBright,pcfg->struColor[0].nBright,
				pcfg->struColor[0].nContrast,pcfg->struColor[0].nSaturation,
				pcfg->struColor[0].nChroma,pcfg->struColor[0].nSharpness);
				nRet = SetIpcColorCfg(&(pcfg->struColor[0]));
			}
			break;
		//曝光配置
		case NVR_ISPCFG_EXPOSURE:
			{
				if (dwBufLen < sizeof(ZW_NVR_EXPOSURE))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(ZW_NVR_EXPOSURE), dwBufLen);
					break;
				}
				ZW_NVR_EXPOSURE* pcfg = (ZW_NVR_EXPOSURE*) pBuffer;
				 if (0 == pcfg->bValid)
                {
                    MPU_DBG("0 == pcfg->bValid\n");
                    break;
                }
				nRet = SetIpcExposureCfg(&(pcfg->struExposure[0]));
			}
			break;
		// 白平衡配置
		case NVR_ISPCFG_WHITEBLANCE:
			{
				if (dwBufLen < sizeof(ZW_NVR_WHITE_BLANCE))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(ZW_NVR_WHITE_BLANCE), dwBufLen);
					break;
				}
				ZW_NVR_WHITE_BLANCE* pcfg = (ZW_NVR_WHITE_BLANCE*) pBuffer;
				 if (0 == pcfg->bValid)
                {
                    MPU_DBG("0 == pcfg->bValid\n");
                    break;
                }
				nRet = SetIpcWhiteBlanceCfg(&(pcfg->struWhiteBlance[0]));
			}
			break;
		// 日夜切换配置
		case NVR_ISPCFG_DAY_NIGHT:
			{
				if (dwBufLen < sizeof(ZW_NVR_DAY_NIGHT))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(ZW_NVR_DAY_NIGHT), dwBufLen);
					break;
				}
				ZW_NVR_DAY_NIGHT* pcfg = (ZW_NVR_DAY_NIGHT*) pBuffer;
				 if (0 == pcfg->bValid)
                {
                    MPU_DBG("0 == pcfg->bValid\n");
                    break;
                }
				nRet = SetIpcDayNightCfg(&(pcfg->struDayAndNight[0]));
			}
			break;
		//基本参数配置
		case NVR_ISPCFG_BASIC:
			{
				if (dwBufLen < sizeof(ZW_NVR_BASIC))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(ZW_NVR_BASIC), dwBufLen);
					break;
				}
				ZW_NVR_BASIC* pcfg = (ZW_NVR_BASIC*) pBuffer;
				 if (0 == pcfg->bValid)
                {
                    MPU_DBG("0 == pcfg->bValid\n");
                    break;
                }
				nRet = SetIpcBasicCfg(&(pcfg->struBasic[0]));
			}
			break;
		//恢复默认
		case NVR_ISPCFG_DEFAULT:
			{
				nRet = SetIpcDefaultCfg();
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

int ISP_Init()
{
	int nRet = IDVR_OK, nSDKRet;

	BoardSensorType_t SensorType;
	
	//获取配置文件管理器实例指针
	CConfig *pCfgFile = CConfig::GetInstance();

	MPU_DBG("\n#######################-------Set ISP_begin!!!!!!------#######################\n");

	char szSection[50];
	AE_T sensor_cfg;

	//获取 Sensor类型
	ZWSys_GetSensorType(&SensorType);
	
	sensor_cfg.antiFlicker = CMOS_FREQUENCY_60HZ;
	sensor_cfg.MinShutterTime = 10000;
	sensor_cfg.MaxShutterTime = 30;
	sensor_cfg.sensorGain = 0;

	WHITEBALANCE_T WhiteBalance;
	WhiteBalance.WBEnable = 1;
	WhiteBalance.WBWorkMode = 6;
	WhiteBalance.RedGain = 50;
	WhiteBalance.BlueGain = 50;
	int AutoChangeBright = 1;
	int AudioInVol = 0;
	int AudioOutVol = 0;
	int DcIris = 1;
	int MCTFStrength = 0;
	int BackLight = 0;
	int WideDynamicRange = 0;
	int DayNightMode = 0;

	IrcutTimeModeInfo info;
#if defined (__IR_CUT_AUTO_MANUAL__)
	IrcutNightVisionTimeInfo timeInfo;
	IrcutNightVisionTimeInfo whitetimeInfo;
#endif

    int PassivitySensitivity     = 50; 
    int PassivityTimeGetFromCfg  = 5; 
    int InitactiveSensitivity    = 50;
    int InitactiveTimeGetFromCfg = 5;
	int	ImageMode = 0;
	int sceneMode = 3;
	int EnableIRcutReverse = 0;
	int nInfraredThreshold = 30;
	int InfraredWorkMode = 0;
	int InfraredBrightness = 90;
	int nWhiteThreshold = 30;
	int nNormalWhiteLightOnLimit = 50;
	int nNormalWhiteLightOffLimit = 25;
	int nImageMode = 0;
    int nDefogStrength = 50;
    int nColorWorkMode = 0;
	int nVarWhiteControlMode = 0;
	int nVarWhiteWorkMode = 0;
    int nVarWhiteBrightness = 20;
    int nVarWhiteLightOnThreshold = 50;
    int nVarWhiteLightOffThreshold = 10;
    int nAntiFlashStrength = 50;

#if defined (__DOUBLE_LIGHT_NO_LDR_CUSTOM__)
	int nColorNightThreshold = 0;
	int nIntelligentThreshold = 0;
#endif
#if defined (__IR_CUT_CONTROL_IMPROVE__)
		int bSptVaryWitManuStdCm = 0;
		int bSptVaryWitManuHsabCm = 0;
#endif

	FLIP_ROTATE_MODE_E MirrorMode = FR_NORMAL;
	void *hChannelHandle=ZWSys_EncChannelOpen(0);

	int BoardInfo;
	char boardIDSection[24];
	int iIrCutDefault=0;
	int InfraredThreshold = 0;
	int WhiteThreshold = 0;

	//获取定制文件读取实例
	CConfig *pCfg = CConfig::NewInstance();
	nSDKRet = pCfg->LoadFileRO(CUSTOM_INFO_FILE);
	if (0 == nSDKRet)
	{
		bSupportColorNight = pCfg->GetValue("ImageCfg", "bSupportColorNight", 0);
        bSupportIntelligentNight = pCfg->GetValue("ImageCfg", "bSupportIntelligentNight", 0);
        bSupportVariableInfrared = pCfg->GetValue("ImageCfg", "bSupportVariableInfrared", 0);
        bSupportVariableWhite = pCfg->GetValue("ImageCfg", "bSupportVariableWhite", 0);
        bSupportFaceNoExposure = pCfg->GetValue("ImageCfg", "bSupportFaceNoExposure", 0);
        bSupportLicensePlate = pCfg->GetValue("ImageCfg", "bSupportLicensePlate", 0);
        VariableInfraredValueFixed = pCfg->GetValue("ImageCfg", "VariableInfraredValueFixed", 0);
        VariableWhiteFixed = pCfg->GetValue("ImageCfg", "VariableWhiteFixed", 0);        
        bDayNightModeFixd = pCfg->GetValue("ImageCfg", "bDayNightModeFixd", 0);
        bSupportNormalWhite = pCfg->GetValue("ImageCfg", "bSupportNormalWhite", 0);
	}
	CConfig::ReleaseInstance(pCfg);

#if defined (__IR_CUT_CONTROL_IMPROVE__)
	pCfg = CConfig::NewInstance();
	nSDKRet = pCfg->LoadFile(CUSTOM_ABILITY_FILE);
	if (0 == nSDKRet)
	{
        // 普通红外模式
		bSptNormIRPassive = pCfg->GetValue("ImageCfg", "bSptNormIRPassive", 0);
        bSptNormIRActive = pCfg->GetValue("ImageCfg", "bSptNormIRActive", 0);
        bSptNormIRDay = pCfg->GetValue("ImageCfg", "bSptNormIRDay", 0);
        bSptNormIRNight = pCfg->GetValue("ImageCfg", "bSptNormIRNight", 0);
        bSptNormIRplan = pCfg->GetValue("ImageCfg", "bSptNormIRplan", 0);
        // 无极红外模式
        bSptVaryIRAuto = pCfg->GetValue("ImageCfg", "bSptVaryIRAuto", 0);
        // 无极白光模式
        bSptVaryWitAuto = pCfg->GetValue("ImageCfg", "bSptVaryWitAuto", 0);
        bSptVaryWitManuPlan = pCfg->GetValue("ImageCfg", "bSptVaryWitManuPlan", 0);
        bSptVaryWitManuStdCm = pCfg->GetValue("ImageCfg", "bSptVaryWitManuStd", 0);
        bSptVaryWitManuHsabCm = pCfg->GetValue("ImageCfg", "bSptVaryWitManuHsab", 0);
        bSptVaryWitManuThresh = bSptVaryWitManuStdCm | bSptVaryWitManuHsabCm;
        // 全彩白光/智能全彩
        bSptColorNightAuto = pCfg->GetValue("ImageCfg", "bSptColorNightAuto", 0);
	}
	CConfig::ReleaseInstance(pCfg);
#endif
    int IntercomMode = 0;
    IntercomMode = pCfgFile->GetValue("IntercomMode", "AppIntercomMode", 1);
    nSDKRet = ZWSys_EncSetIntercomMode(IntercomMode);
	if (0 != nSDKRet)
	{
		MPU_ERR("%s: set Intercom Mode to dsp fail, return %d\n", szSection, nSDKRet);
		nRet = IDVR_ERROR_HISDK;
	}

#ifdef ACTIVE_CODE_SUPPORT
	int bActiveSuccess = 0;
	int nActiveStutusCode = 0;

    /* 首先从SDK里面获取当前设备的激活状态码 */
    ZWSys_GetActiveStatusCode(&nActiveStutusCode);

    bActiveSuccess = nActiveStutusCode & (1 << DOUBLE_LIGHT_SUPPORT);
#endif

    /* 如果授权了双光源则强制打开该能力 */
    if(bActiveSuccess)
    {
        bSupportColorNight = 1;
        bSupportIntelligentNight = 1;
    }	

	ZWSys_GetBoardInfo(&BoardInfo);
	memset(boardIDSection, 0, sizeof(boardIDSection));
	snprintf(boardIDSection, sizeof(boardIDSection), "BoardID-%x", BoardInfo);
    
	//根据版型来修改默认配置参数
	iIrCutDefault = pCfgFile->GetValue(boardIDSection, "IrCutDefault", 0);
	if(1 == iIrCutDefault)
	{
		pCfgFile->SetValue(boardIDSection, "IrCutDefault", 0);
		InitactiveSensitivity	 = pCfgFile->GetValue(boardIDSection, "nInitactiveDayNightSensitivity", 50); 			
		InfraredThreshold	 = pCfgFile->GetValue(boardIDSection, "InfraredThreshold", 50); 
		WhiteThreshold	 = pCfgFile->GetValue(boardIDSection, "WhiteThreshold", 50); 
		ImageMode	 = pCfgFile->GetValue(boardIDSection, "ImageMode", 0); 		        
		DayNightMode	 = pCfgFile->GetValue(boardIDSection, "VideoColor", -1); 
		InfraredWorkMode = pCfgFile->GetValue(boardIDSection, "InfraredWorkMode", 1); 
		InfraredBrightness = pCfgFile->GetValue(boardIDSection, "InfraredBrightness", 100);

		#if defined (__IR_CUT_AUTO_MANUAL__)
		nColorWorkMode = pCfgFile->GetValue(boardIDSection, "nColorWorkMode", 1);
		timeInfo.nStartHour = pCfgFile->GetValue(boardIDSection, "ColorNightStartHour", 18);
		timeInfo.nStartMin = pCfgFile->GetValue(boardIDSection, "ColorNightStartMin", 0);
		timeInfo.nStartSec= pCfgFile->GetValue(boardIDSection, "ColorNightStartSec", 0);
		timeInfo.nStopHour = pCfgFile->GetValue(boardIDSection, "ColorNightStopHour", 6);
		timeInfo.nStopMin= pCfgFile->GetValue(boardIDSection, "ColorNightStopMin", 0);
		timeInfo.nStopSec= pCfgFile->GetValue(boardIDSection, "ColorNightStopSec", 0);

		/*获取无极白光相关*/
        nVarWhiteControlMode = pCfgFile->GetValue(boardIDSection, "nVarWhiteControlMode", 1);
        nVarWhiteWorkMode = pCfgFile->GetValue(boardIDSection, "nVarWhiteWorkMode", 0);
        nVarWhiteBrightness = pCfgFile->GetValue(boardIDSection, "nVarWhiteBrightness", 20);
        nVarWhiteLightOnThreshold = pCfgFile->GetValue(boardIDSection, "nVarWhiteLightOnThreshold", 50);
        nVarWhiteLightOffThreshold = pCfgFile->GetValue(boardIDSection, "nVarWhiteLightOffThreshold", 10);

		/*获取普通白光开灯阀值*/
		nNormalWhiteLightOnLimit = pCfgFile->GetValue(boardIDSection, "NormalWhiteLightOnLimit", 50);
		/*获取普通白光关灯阀值*/
		nNormalWhiteLightOffLimit = pCfgFile->GetValue(boardIDSection, "NormalWhiteLightOffLimit", 25);
		#endif

        #if defined (__DOUBLE_LIGHT_NO_LDR_CUSTOM__)
        nColorNightThreshold = pCfgFile->GetValue(boardIDSection, "nColorNightThreshold", 50);
        nIntelligentThreshold = pCfgFile->GetValue(boardIDSection, "nIntelligentThreshold", 50);
        #endif

        if (DayNightMode >= 0)
        {
            snprintf(szSection, sizeof(szSection), "ISP_S%d", SensorType);		
			pCfgFile->SetValue(szSection, "VideoColor", DayNightMode); 
		}

		snprintf(szSection, sizeof(szSection), "IR_CUT_cfg");
		nRet = pCfgFile->SetValue(szSection, "nPassivityDayNightSensitivity", InitactiveSensitivity); 
		if (nRet == 0 && pCfgFile->m_bReadOnlyCfgFlag == 1)
		{
			InitactiveSensitivity = pCfgFile->GetValue(szSection, "nPassivityDayNightSensitivity", 0);
		}
		nRet = pCfgFile->SetValue(szSection, "nInitactiveDayNightSensitivity", InitactiveSensitivity); 
		if (nRet == 0 && pCfgFile->m_bReadOnlyCfgFlag == 1)
		{
			InitactiveSensitivity = pCfgFile->GetValue(szSection, "nInitactiveDayNightSensitivity", 0);
		}
		nRet = pCfgFile->SetValue(szSection, "InfraredThreshold", InfraredThreshold); 
		if (nRet == 0 && pCfgFile->m_bReadOnlyCfgFlag == 1)
		{
			InfraredThreshold = pCfgFile->GetValue(szSection, "InfraredThreshold", 0); 
		}
		nRet = pCfgFile->SetValue(szSection, "WhiteThreshold", WhiteThreshold); 
		if (nRet == 0 && pCfgFile->m_bReadOnlyCfgFlag == 1)
		{
			WhiteThreshold = pCfgFile->GetValue(szSection, "WhiteThreshold", 0); 
		}
		nRet = pCfgFile->SetValue(szSection, "ImageMode", ImageMode); 
		if (nRet == 0 && pCfgFile->m_bReadOnlyCfgFlag == 1)
		{
			ImageMode = pCfgFile->GetValue(szSection, "ImageMode", 0); 
		}

		#if defined (__IR_CUT_AUTO_MANUAL__)
        /*获取无极白光相关*/
		nRet = pCfgFile->SetValue(szSection, "nVarWhiteControlMode", nVarWhiteControlMode);		
		if (nRet == 0 && pCfgFile->m_bReadOnlyCfgFlag == 1)
		{
			nVarWhiteControlMode = pCfgFile->GetValue(szSection, "nVarWhiteControlMode", 1);
		}
		
		nRet = pCfgFile->SetValue(szSection, "nVarWhiteWorkMode", nVarWhiteWorkMode);		
		if (nRet == 0 && pCfgFile->m_bReadOnlyCfgFlag == 1)
		{
			nVarWhiteWorkMode = pCfgFile->GetValue(szSection, "nVarWhiteWorkMode", 0);
		}
		
		nRet = pCfgFile->SetValue(szSection, "nVarWhiteBrightness", nVarWhiteBrightness);		
		if (nRet == 0 && pCfgFile->m_bReadOnlyCfgFlag == 1)
		{
			nVarWhiteBrightness = pCfgFile->GetValue(szSection, "nVarWhiteBrightness", 20);
		}
		
		nRet = pCfgFile->SetValue(szSection, "nVarWhiteLightOnThreshold", nVarWhiteLightOnThreshold);		
		if (nRet == 0 && pCfgFile->m_bReadOnlyCfgFlag == 1)
		{
			nVarWhiteLightOnThreshold = pCfgFile->GetValue(szSection, "nVarWhiteLightOnThreshold", 50);
		}
		
		nRet = pCfgFile->SetValue(szSection, "nVarWhiteLightOffThreshold", nVarWhiteLightOffThreshold);		
		if (nRet == 0 && pCfgFile->m_bReadOnlyCfgFlag == 1)
		{
			nVarWhiteLightOffThreshold = pCfgFile->GetValue(szSection, "nVarWhiteLightOffThreshold", 10);
		}

		nRet = pCfgFile->SetValue(szSection, "NormalWhiteLightOnLimit", nNormalWhiteLightOnLimit);		
		if (nRet == 0 && pCfgFile->m_bReadOnlyCfgFlag == 1)
		{
			nNormalWhiteLightOnLimit = pCfgFile->GetValue(szSection, "NormalWhiteLightOnLimit", 50);
		}
		
		nRet = pCfgFile->SetValue(szSection, "NormalWhiteLightOffLimit", nNormalWhiteLightOffLimit);		
		if (nRet == 0 && pCfgFile->m_bReadOnlyCfgFlag == 1)
		{
			nNormalWhiteLightOffLimit = pCfgFile->GetValue(szSection, "NormalWhiteLightOffLimit", 25);
		}
        #endif

        #if defined (__IR_CUT_CONTROL_IMPROVE__)
        nRet = pCfgFile->SetValue(szSection, "nColorNightThreshold", nColorNightThreshold);       
        if (nRet == 0 && pCfgFile->m_bReadOnlyCfgFlag == 1)
        {
            nColorNightThreshold = pCfgFile->GetValue(szSection, "nColorNightThreshold", 50);
        }
        
        nRet = pCfgFile->SetValue(szSection, "nIntelligentThreshold", nIntelligentThreshold);       
        if (nRet == 0 && pCfgFile->m_bReadOnlyCfgFlag == 1)
        {
            nIntelligentThreshold = pCfgFile->GetValue(szSection, "nIntelligentThreshold", 50);
        }
        #endif
	}

	//当定制夜视模式为固定模式时，为保证以前的版本升级以后到配置设置的模式，就每次都从板型定制的字段读取夜视模式的值
    if (bDayNightModeFixd)
    {
        CConfig *pDefaultCfg = CConfig::NewInstance();        
        if(NULL != pDefaultCfg)
        {
            //从缺省配置文件中读取
            pDefaultCfg->LoadFileRO(IDVR_DEFAULT_CONFIG_FILE_PATH);
            //从板型定制文件里面读取
            DayNightMode = pDefaultCfg->GetValue(boardIDSection, "VideoColor", -1);     
            if (DayNightMode >= 0)
            {
                snprintf(szSection, sizeof(szSection), "ISP_S%d", SensorType);    
				pCfgFile->SetValue(szSection, "VideoColor", DayNightMode); 
            } 
            
            CConfig::ReleaseInstance(pDefaultCfg);
        }     
    }

    //保存一下配置
    if (iIrCutDefault || bDayNightModeFixd)
    {
        pCfgFile->SaveFile();
    }

	snprintf(szSection, sizeof(szSection),"%s", "AudioIn");
	AudioInVol = pCfgFile->GetValue(szSection, "AudioVol", 80);

	snprintf(szSection, sizeof(szSection),"%s", "AudioOut");
	AudioOutVol = pCfgFile->GetValue(szSection, "AudioVol", 80);

	snprintf(szSection, sizeof(szSection), "ISP_S%d", SensorType);
	AutoChangeBright = pCfgFile->GetValue(szSection, "AutoChangeBright", 1);
	sensor_cfg.antiFlicker = (CmosFrequencyType_t)(pCfgFile->GetValue(szSection, "InputFrequency", 1));

	DayNightMode = pCfgFile->GetValue(szSection, "VideoColor", 0);
    nDefogStrength    = pCfgFile->GetValue(szSection, "DefogStrength", 50);
    nAntiFlashStrength    = pCfgFile->GetValue(szSection, "AntiFlashStrength", 50);
	info.nStartHour = pCfgFile->GetValue(szSection, "DayNightStartHour", 7);
	info.nStartMin = pCfgFile->GetValue(szSection, "DayNightStartMin", 0);
	info.nStartSec= pCfgFile->GetValue(szSection, "DayNightStartSec", 0);
	info.nStopHour = pCfgFile->GetValue(szSection, "DayNightStopHour", 19);
	info.nStopMin= pCfgFile->GetValue(szSection, "DayNightStopMin", 0);
	info.nStopSec= pCfgFile->GetValue(szSection, "DayNightStopSec", 0);

	MirrorMode = (FLIP_ROTATE_MODE_E)(pCfgFile->GetValue(boardIDSection, "VideoOrientation", 0));

	snprintf(szSection, sizeof(szSection), "ISP_S%d_T%d", SensorType, USER_CUSTOM_TEMPLATE);
	
	sensor_cfg.MaxShutterTime = pCfgFile->GetValue(szSection, "MaxShutterSpeed", 30);
	sensor_cfg.sensorGain = pCfgFile->GetValue(szSection, "MaxAutoGain", 0);

	DcIris = pCfgFile->GetValue(szSection, "LensType", 1);

	WhiteBalance.WBWorkMode = pCfgFile->GetValue(szSection, "WhiteBalanceMode", 6);

	MCTFStrength = pCfgFile->GetValue(szSection, "DeNoiseMode", 0);

	BackLight = pCfgFile->GetValue(szSection, "YStrength", 0);

	WideDynamicRange = pCfgFile->GetValue(szSection, "CbStrength", 0);

	snprintf(szSection, sizeof(szSection), "ISP_S%d_T%d_WB%d", SensorType, USER_CUSTOM_TEMPLATE, 5);
	WhiteBalance.RedGain = pCfgFile->GetValue(szSection, "RedGain", 50);
	WhiteBalance.BlueGain = pCfgFile->GetValue(szSection, "BlueGain", 50);
	
	nSDKRet = ZWSys_EncSetAutoChangeBright(hChannelHandle, AutoChangeBright);
	if (0 != nSDKRet)
	{ 
		MPU_ERR("%s: set Dc Iris fail, return %d\n", szSection, nSDKRet);
		nRet = IDVR_ERROR_HISDK;
	}

	/* 关于IrCut初始化配置(系统启动时直接从配置文件中读取信息设置下去) */
	//////////////////////////////////////////////////////////////////////////////////
    snprintf(szSection, sizeof(szSection), "IR_CUT_cfg");
    PassivitySensitivity     = pCfgFile->GetValue(szSection, "nInitactiveDayNightSensitivity", 50); 
    PassivityTimeGetFromCfg  = pCfgFile->GetValue(szSection, "nPassivitySwitchTime", 5); 
    InitactiveSensitivity    = pCfgFile->GetValue(szSection, "nInitactiveDayNightSensitivity", 50); 
    InitactiveTimeGetFromCfg = pCfgFile->GetValue(szSection, "nInitactiveSwitchTime", 5); 
	sceneMode		         = pCfgFile->GetValue(szSection, "nSceneMode", 3);
	nInfraredThreshold       = pCfgFile->GetValue(szSection, "InfraredThreshold", 30);
	InfraredWorkMode       = pCfgFile->GetValue(szSection, "InfraredWorkMode", 1);
	InfraredBrightness       = pCfgFile->GetValue(szSection, "InfraredBrightness", 100);
	nWhiteThreshold       = pCfgFile->GetValue(szSection, "WhiteThreshold", 30);
	nImageMode       = pCfgFile->GetValue(szSection, "ImageMode", 0);
	EnableIRcutReverse =  pCfgFile->GetValue(szSection, "IRcutReverse", 0);

#if defined (__IR_CUT_AUTO_MANUAL__)
	nNormalWhiteLightOnLimit	   = pCfgFile->GetValue(szSection, "NormalWhiteLightOnLimit", 50);
	nNormalWhiteLightOffLimit		= pCfgFile->GetValue(szSection, "NormalWhiteLightOffLimit", 25);

	nColorWorkMode = pCfgFile->GetValue(szSection, "nColorWorkMode", 1);
	timeInfo.nStartHour = pCfgFile->GetValue(szSection, "ColorNightStartHour", 18);
	timeInfo.nStartMin = pCfgFile->GetValue(szSection, "ColorNightStartMin", 0);
	timeInfo.nStartSec= pCfgFile->GetValue(szSection, "ColorNightStartSec", 0);
	timeInfo.nStopHour = pCfgFile->GetValue(szSection, "ColorNightStopHour", 6);
	timeInfo.nStopMin= pCfgFile->GetValue(szSection, "ColorNightStopMin", 0);
	timeInfo.nStopSec= pCfgFile->GetValue(szSection, "ColorNightStopSec", 0);

	nVarWhiteControlMode = pCfgFile->GetValue(szSection, "nVarWhiteControlMode", 1);
    nVarWhiteWorkMode = pCfgFile->GetValue(szSection, "nVarWhiteWorkMode", 0);
    nVarWhiteBrightness = pCfgFile->GetValue(szSection, "nVarWhiteBrightness", 20);
    nVarWhiteLightOnThreshold = pCfgFile->GetValue(szSection, "nVarWhiteLightOnThreshold", 50);
	nVarWhiteLightOffThreshold = pCfgFile->GetValue(szSection, "nVarWhiteLightOffThreshold", 10);
	whitetimeInfo.nStartHour = pCfgFile->GetValue(szSection, "VarWhiteStartHour", 18);
	whitetimeInfo.nStartMin = pCfgFile->GetValue(szSection, "VarWhiteStartMin", 0);
	whitetimeInfo.nStartSec= pCfgFile->GetValue(szSection, "VarWhiteStartSec", 0);
	whitetimeInfo.nStopHour = pCfgFile->GetValue(szSection, "VarWhiteStopHour", 23);
	whitetimeInfo.nStopMin= pCfgFile->GetValue(szSection, "VarWhiteStopMin", 0);
	whitetimeInfo.nStopSec= pCfgFile->GetValue(szSection, "VarWhiteStopSec", 0);
#endif

#if defined (__DOUBLE_LIGHT_NO_LDR_CUSTOM__)
	nColorNightThreshold = pCfgFile->GetValue(szSection, "nColorNightThreshold", 50);
	nIntelligentThreshold = pCfgFile->GetValue(szSection, "nIntelligentThreshold", 50);
#endif
	printf("Start -------------------------------ISP \n");
	nSDKRet = ZWSys_EncSetIrcutDirection(hChannelHandle, EnableIRcutReverse);
	MPU_CHK(0 == nSDKRet);
	
    nSDKRet = ZWSys_EncSetIrCut(hChannelHandle,  PassivitySensitivity);
    MPU_CHK(0 == nSDKRet);

    nSDKRet = ZWSys_EncSetDayNightSwitchTime(hChannelHandle, PassivityTimeGetFromCfg);
    MPU_CHK(0 == nSDKRet);

    nSDKRet = ZWSys_EncSetIrcutInitiativeModeSensity(hChannelHandle,  InitactiveSensitivity);
    MPU_CHK(0 == nSDKRet);

    nSDKRet = ZWSys_EncSetDayNightInitiativeSwitchTime(hChannelHandle, InitactiveTimeGetFromCfg);
    MPU_CHK(0 == nSDKRet);

	//nSDKRet = ZWSys_SetColorNightCfg(hChannelHandle, colorNightLastTime );
    //MPU_CHK(0 == nSDKRet);

	nSDKRet = ZWSys_SetInfraredThreshold(hChannelHandle, nInfraredThreshold);
    MPU_CHK(0 == nSDKRet);

	nSDKRet = ZWSys_SetInfraredWorkMode(hChannelHandle, InfraredWorkMode);
    MPU_CHK(0 == nSDKRet);
	
	nSDKRet = ZWSys_SetInfraredBrightness(hChannelHandle, InfraredBrightness);
    MPU_CHK(0 == nSDKRet);

	nSDKRet = ZWSys_SetWhiteLightThreshold(hChannelHandle, nWhiteThreshold);
    MPU_CHK(0 == nSDKRet);

	nSDKRet = ZWSys_SetImageMode(hChannelHandle, nImageMode);
    MPU_CHK(0 == nSDKRet);

	nSDKRet = ZWSys_SetDefogStrength(hChannelHandle, nDefogStrength);
    MPU_CHK(0 == nSDKRet);
    
	nSDKRet = ZWSys_SetAntiFlickerStrength(hChannelHandle, nAntiFlashStrength);
    MPU_CHK(0 == nSDKRet);

	nSDKRet = ZWSys_SetColorType(hChannelHandle, (color_type_E)sceneMode);
    MPU_CHK(0 == nSDKRet);
    //////////////////////////////////////////////////////////////////////////////////

	nSDKRet = ZWSys_SetAudioInVol(hChannelHandle, AudioInVol);
    MPU_CHK(0 == nSDKRet);

	nSDKRet = ZWSys_SetAudioOutVol(hChannelHandle, AudioOutVol, 1);
	MPU_CHK(0 == nSDKRet);
#if 0
	for(i=0; i<4; i++)
	{
		//亮度、色度、对比度、饱和度
		nSDKRet = MPU_AVL_Pic_SetPicParam(0, i, 0, 50);
		MPU_CHK(0 == nSDKRet);
		
		nSDKRet = MPU_AVL_Pic_SetPicParam(0, i, 1, 50);
		MPU_CHK(0 == nSDKRet);
		
		nSDKRet = MPU_AVL_Pic_SetPicParam(0, i, 2, 50);
		MPU_CHK(0 == nSDKRet);
		
		nSDKRet = MPU_AVL_Pic_SetPicParam(0, i, 3, 50);
		MPU_CHK(0 == nSDKRet);

		nSDKRet = MPU_AVL_Pic_SetPicParam(0, i, 4, 50);
		MPU_CHK(0 == nSDKRet);
		//时间段开始和结束时间
		//nSDKRet = MPU_AVL_Pic_SetTimeCfg(nCh, nTimeSec, pcfg->nStartTime, pcfg->nStopTime);
		//MPU_CHK(0 == nSDKRet);
	}
#endif
	nSDKRet = ZWSys_SetAEParam(hChannelHandle,&sensor_cfg);
	if (0 != nSDKRet)
	{
		MPU_ERR("%s: ZWSys_SetAEParam fail, return %d\n", szSection, nSDKRet);
		nRet = IDVR_ERROR_HISDK;
	}

	nSDKRet = ZWSys_SetDcIris(hChannelHandle, DcIris);
	if (0 != nSDKRet)
	{ 
		MPU_ERR("%s: set Dc Iris fail, return %d\n", szSection, nSDKRet);
		nRet = IDVR_ERROR_HISDK;
	}

	if(WB_MODE_AUTO == WhiteBalance.WBWorkMode)// || (4 == pcfg->nWhiteBlanceMode)
		WhiteBalance.WBEnable = 0;
	else
		WhiteBalance.WBEnable = 1;
	nSDKRet = ZWSys_SetWhiteBalance(hChannelHandle,&WhiteBalance);
	if (0 != nSDKRet)
	{
		MPU_ERR("%s: set white balance	to dsp fail, return %d\n", szSection, nSDKRet);
		nRet = IDVR_ERROR_HISDK;
	}

	nSDKRet = ZWSys_SetMCTFStrength(hChannelHandle, MCTFStrength);
	if (0 != nSDKRet)
	{
		MPU_ERR("%s: ZWSys_SetMCTFStrength fail, return %d\n", szSection, nSDKRet);
		nRet = IDVR_ERROR_HISDK;
	}

	nSDKRet = ZWSys_EncSetBacklightMode(hChannelHandle, BackLight);
	if (0 != nSDKRet)
	{
		MPU_ERR("%s: ZWSys_EncSetBacklightMode fail, return %d\n", szSection, nSDKRet);
		nRet = IDVR_ERROR_HISDK;
	}

	nSDKRet = ZWSys_SetLocalExpMode(hChannelHandle,WideDynamicRange);
	if (0 != nSDKRet)
	{
		MPU_ERR("%s: ZWSys_SetLocalExpMode fail, return %d\n", szSection, nSDKRet);
		nRet = IDVR_ERROR_HISDK;
	}

	nSDKRet = SetDayNightMode_Ex(hChannelHandle, (DAYNIGHTMODE_T)DayNightMode, &info);
	if (0 != nSDKRet)
	{
		MPU_ERR("%s: set video color to dsp fail, return %d\n", szSection, nSDKRet);
		nRet = IDVR_ERROR_HISDK;
	}

#if defined (__IR_CUT_AUTO_MANUAL__)
	nSDKRet = ZWSys_SetNormalWhiteLightThreshold(hChannelHandle, nNormalWhiteLightOnLimit, nNormalWhiteLightOffLimit);
	if (0 != nSDKRet)
	{
		MPU_ERR("%s: ZWSys_SetNormalWhiteLightThreshold fail, return %d\n", szSection, nSDKRet);
		nRet = IDVR_ERROR_HISDK;
	}

	nSDKRet = ZWSys_EncSetColorNightMode(hChannelHandle, (NIGHTVISIONWORKMODE_T)nColorWorkMode, &timeInfo);
	if (0 != nSDKRet)
	{
		MPU_ERR("%s: ZWSys_EncSetColorNightMode fail, return %d\n", szSection, nSDKRet);
		nRet = IDVR_ERROR_HISDK;
	}

	nSDKRet = ZWSys_EncSetVarWhiteMode(hChannelHandle, (NIGHTVISIONWORKMODE_T)nVarWhiteControlMode, (NIGHTVISIONCUSTOMMODE_T)nVarWhiteWorkMode, nVarWhiteBrightness);
	if (0 != nSDKRet)
	{
		MPU_ERR("%s: ZWSys_EncSetVarWhiteMode fail, return %d\n", szSection, nSDKRet);
		nRet = IDVR_ERROR_HISDK;
	}
	
    nSDKRet = ZWSys_EncSetVarWhiteModeInfo(hChannelHandle, &whitetimeInfo, nVarWhiteLightOnThreshold, nVarWhiteLightOffThreshold);
	if (0 != nSDKRet)
	{
		MPU_ERR("%s: ZWSys_EncSetVarWhiteModeInfo fail, return %d\n", szSection, nSDKRet);
		nRet = IDVR_ERROR_HISDK;
	}
#endif

#if defined (__DOUBLE_LIGHT_NO_LDR_CUSTOM__)
    nSDKRet = ZWSys_SetColorNightThreshold(hChannelHandle, nColorNightThreshold);
    if (0 != nSDKRet)
    {
        MPU_ERR("%s: ZWSys_SetColorNightThreshold fail, return %d\n", szSection, nSDKRet);
        nRet = IDVR_ERROR_HISDK;
    }
    
    nSDKRet = ZWSys_SetIntelligentThreshold(hChannelHandle, nIntelligentThreshold);
    if (0 != nSDKRet)
    {
        MPU_ERR("%s: ZWSys_SetIntelligentThreshold fail, return %d\n", szSection, nSDKRet);
        nRet = IDVR_ERROR_HISDK;
    }
#endif

	if (0 == MirrorMode)//正常
	{
		MirrorMode = FR_NORMAL;
	}
	else if (3 == MirrorMode)//翻转
	{
		MirrorMode = FR_ROTATE_180;
	}
	else if (1 == MirrorMode)//镜像
	{
		MirrorMode = FR_HORI_FLIP;
	}
	else if (2 == MirrorMode)//翻转+ 镜像
	{
		MirrorMode = FR_VERT_FLIP;
	}
	else
	{
		MirrorMode = FR_NORMAL;
	}
	nSDKRet = ZWSys_SetFlipRotateMode(hChannelHandle, MirrorMode);
	if (0 != nSDKRet)
	{
		MPU_ERR("%s: set video orientation	to dsp fail, return %d\n", szSection, nSDKRet);
		nRet = IDVR_ERROR_HISDK;
	}
	MPU_DBG("\n#######################-------Set ISP_end!!!!!!------#######################\n");
	printf("End -------------------------------ISP \n");
	
return nRet;
}

//设备对讲模式
static int GetDeviceIntercomMode(HY_DVR_DEVICE_INTERCOM_MODE *pcfg)
{
	int nRet = IDVR_OK;
	CConfig *pCfgFile = CConfig::GetInstance();

	if (NULL != pCfgFile)
	{
		pcfg->iValid = 1;
        pcfg->iEnable = 0;
		pcfg->iIntercomMode = pCfgFile->GetValue("IntercomMode", "AppIntercomMode", 1);//默认半双工
        ZWSys_EncGetIntercomMode(&pcfg->iIntercomMode);
	}
	else
	{
		//读取配置文件失败
		MPU_ERR("no CConfig instance\n");
		MPU_CHK(NULL != pCfgFile);
	}

	return nRet;
}

static int SetDeviceIntercomMode(const HY_DVR_DEVICE_INTERCOM_MODE *pcfg)
{
	int nRet = IDVR_OK;
    CConfig *pCfgFile = NULL;

	//本结构体是否有效，=0表示无效
	if (0 == pcfg->iValid)
	{
		MPU_DBG("0 == pcfg->bValid\n");
		return IDVR_CIU_STRUCT_UNVALID;
	}

	if (pcfg->iIntercomMode > 1 || pcfg->iIntercomMode < 0)
	{
		MPU_DBG("IntercomMode [0|1]\n");
		return IDVR_CIU_PARAM_OVERFLOW;
	}

	//获取配置文件管理器实例指针
	pCfgFile = CConfig::GetInstance();
	if (NULL != pCfgFile)
	{
		pCfgFile->SetValue("IntercomMode", "AppIntercomMode", pcfg->iIntercomMode);
		ZWSys_EncSetIntercomMode(pcfg->iIntercomMode);
	}
    else
	{
		//读取配置文件失败
		MPU_ERR("no CConfig instance\n");
		MPU_CHK(NULL != pCfgFile);
	}

    return nRet;
}

int CIU_GetConfigure_DeviceIntercomMode(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, const BYTE * pBuffer, DWORD * dwBufLen)
{
	int nRet = IDVR_OK;
	MPU_DBG("ID %d, Major %d, Minor %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType);
	switch (dwMinorType)
	{
		case DEVICE_INTERCOM_MODE:
			{
                //判断buf空间是否足够
				if (*dwBufLen < sizeof(HY_DVR_DEVICE_INTERCOM_MODE))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_DEVICE_INTERCOM_MODE), *dwBufLen);
					break;
				}
				HY_DVR_DEVICE_INTERCOM_MODE *pcfg = (HY_DVR_DEVICE_INTERCOM_MODE *) pBuffer;
                //初始化输出缓冲区
				memset(pcfg, 0, sizeof(HY_DVR_DEVICE_INTERCOM_MODE));
				nRet = GetDeviceIntercomMode(pcfg);
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

int CIU_SetConfigure_DeviceIntercomMode(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, const BYTE * pBuffer, DWORD dwBufLen)
{
	int nRet = IDVR_OK;
	MPU_DBG("ID %d, Major %d, Minor %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType);

	switch (dwMinorType)
	{
		case DEVICE_INTERCOM_MODE:
			{
                //判断buf空间是否足够
				if (dwBufLen < sizeof(HY_DVR_DEVICE_INTERCOM_MODE))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_DEVICE_INTERCOM_MODE), dwBufLen);
					break;
				}
				HY_DVR_DEVICE_INTERCOM_MODE *pcfg = (HY_DVR_DEVICE_INTERCOM_MODE *) pBuffer;
				nRet = SetDeviceIntercomMode(pcfg);
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


#ifdef __SUPPORT_P2P_MAXCLIENT__
static int GetStreamOutMode(HY_DVR_STREAM_OUT_CFG *pcfg)
{
    int nRet = IDVR_OK;
    pcfg->nValid = 1;
    pcfg->mode = 1; //默认支持踢掉最大预览数量不显示预览满提示
    return nRet;
}


int CIU_GetConfigure_StreamOutOfRange(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen)
{

	int nRet = IDVR_OK;
	MPU_INFO("ID %d, Major %d, Minor %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType);

	switch (dwMinorType)
	{
		case HY_DVR_GET_STREAM_OUT_MODE:
			{
				///////////////////////////////////////////////////////////////////////////////
				//判断buf空间是否足够
				if (*pdwBufLen < sizeof(HY_DVR_STREAM_OUT_CFG))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_STREAM_OUT_CFG), *pdwBufLen);
					break;
				}
				HY_DVR_STREAM_OUT_CFG *pcfg = (HY_DVR_STREAM_OUT_CFG *) pBuffer;
				memset(pcfg, 0, sizeof(HY_DVR_STREAM_OUT_CFG));
				nRet = GetStreamOutMode(pcfg);
			}
			break;

		default:
			{
				MPU_ERR("Unsupported dwMinorType = %lu\n", dwMinorType);
				nRet = IDVR_CIU_UNSUPPORTED_MINORTYPE;
			}
			break;

	}

	MPU_INFO("ID %d, Major %d, Minor %d, return %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType, nRet);
	return nRet;
}
#endif

int  CIU_GetConfigure_NIGHT_LED_INFO(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen)
{
	int nRet = IDVR_OK;
	MPU_DBG("ID %d, Major %d, Minor %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType);
	switch (dwMinorType)
	{
		// 获取一键夜灯信息
		case NIGHT_LED_CFG_ALL:
			{
                //判断buf空间是否足够
				if (*pdwBufLen < sizeof(HY_DVR_NIGHT_LED_INFO_CFG))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_NIGHT_LED_INFO_CFG), *pdwBufLen);
					break;
				}
				HY_DVR_NIGHT_LED_INFO_CFG *pcfg = (HY_DVR_NIGHT_LED_INFO_CFG *) pBuffer;
                //初始化输出缓冲区
				memset(pcfg, 0, sizeof(HY_DVR_NIGHT_LED_INFO_CFG));
				nRet = GetNightLedInfo(pcfg);
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

int CIU_SetConfigure_NIGHT_LED_INFO(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, const BYTE * pBuffer, DWORD dwBufLen)
{
	int nRet = IDVR_OK;
	MPU_DBG("ID %d, Major %d, Minor %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType);

	switch (dwMinorType)
	{
		// 设置一键夜灯信息
		case NIGHT_LED_CFG_ALL:
			{
                //判断buf空间是否足够
				if (dwBufLen < sizeof(HY_DVR_NIGHT_LED_INFO_CFG))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_NIGHT_LED_INFO_CFG), dwBufLen);
					break;
				}
				HY_DVR_NIGHT_LED_INFO_CFG *pcfg = (HY_DVR_NIGHT_LED_INFO_CFG *) pBuffer;
				nRet = SetNightLedInfo(pcfg);
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

//声音检测
static int GetCryDetectCfg(HY_DVR_CRY_SCREAM_DETECT_CFG *pcfg)
{
	int nRet = IDVR_OK, nSDKRet = FALSE;
	int nCh = 0;
	int bEnable = 0;
	int nSenstive = 0;

	///////////////////////////////////////////////////////////////////////////////
	//本结构体是否有效，=0表示无效
	pcfg->iValid = 1;

	//默认返回设置成功
    nSDKRet = MPU_AVL_SoundDetect_GetCfg( nCh, &bEnable, &nSenstive);
	if (nSDKRet)
	{
		MPU_ERR("Set Ch %d target magic blocks fail, return %d\n", nCh, nSDKRet);
		nRet = IDVR_CIU_CALLSDK_FAIL;
	}
	else
	{
        switch (nSenstive)
        {
            case 1:
            {
                pcfg->iSenstive = 30;
            }
            break;
        
            case 2:
            {
                pcfg->iSenstive = 60;
            }
            break;
        
            case 3:
            {
                pcfg->iSenstive = 90;
            }
            break;
        
            default:
            {
                pcfg->iSenstive = 60;
            }
            break;
        }
		pcfg->iEnable = bEnable;
	}

	return nRet;
}

static int SetCryDetectCfg(HY_DVR_CRY_SCREAM_DETECT_CFG *pcfg)
{
	int nRet = IDVR_OK, nSDKRet = 0;
	int nCh = 0;
    int nSenstive = 0;
	///////////////////////////////////////////////////////////////////////////////
	//本结构体是否有效，=0表示无效
	if (0 == pcfg->iValid)
	{
		MPU_DBG("0 == pcfg->nValid\n");
		return IDVR_CIU_STRUCT_UNVALID;
	}

	switch (pcfg->iSenstive)
	{
        case 30:
        {
            nSenstive = 1;
        }
        break;

        case 60:
        {
            nSenstive = 2;
        }
        break;

        case 90:
        {
            nSenstive = 3;
        }
        break;

        default:
        {
            nSenstive = 2;
        }
        break;
    }

    nSDKRet = MPU_AVL_SoundDetect_SetCfg( nCh, pcfg->iEnable, nSenstive);
	if (nSDKRet)
	{
		MPU_ERR("Set Ch %d target magic blocks fail, return %d\n", nCh, nSDKRet);
		nRet = IDVR_CIU_CALLSDK_FAIL;
	}

	return nRet;
}

int CIU_SetConfigure_CryDetect(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, const BYTE * pBuffer, DWORD  dwBufLen)
{
	int nRet = IDVR_OK;

	switch (dwMinorType)
	{
        case CRY_SCREAM_DETECT_CFG:
        {
            HY_DVR_CRY_SCREAM_DETECT_CFG * pcfg = (HY_DVR_CRY_SCREAM_DETECT_CFG *) pBuffer;
            
            //判断buf空间是否足够
            if (dwBufLen < sizeof(HY_DVR_CRY_SCREAM_DETECT_CFG))
            {
                nRet = IDVR_CIU_BUF_SMALL;
                MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_CRY_SCREAM_DETECT_CFG), dwBufLen);
                break;
            } 

            nRet = SetCryDetectCfg(pcfg);
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

int CIU_GetConfigure_CryDetect(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen)
{
	int nRet = IDVR_OK;

	switch (dwMinorType)
	{
		case CRY_SCREAM_DETECT_CFG:
        {
			///////////////////////////////////////////////////////////////////////////////
			//判断buf空间是否足够
			if (*pdwBufLen < sizeof(HY_DVR_CRY_SCREAM_DETECT_CFG))
			{
				nRet = IDVR_CIU_BUF_SMALL;
				MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_CRY_SCREAM_DETECT_CFG), *pdwBufLen);
				break;
			}

			LPHY_DVR_CRY_SCREAM_DETECT_CFG pcfg = (LPHY_DVR_CRY_SCREAM_DETECT_CFG) pBuffer;

			///////////////////////////////////////////////////////////////////////////////
			//初始化输出缓冲区
			memset(pcfg, 0, sizeof(HY_DVR_CRY_SCREAM_DETECT_CFG));
			*pdwBufLen = sizeof(HY_DVR_CRY_SCREAM_DETECT_CFG);

			nRet = GetCryDetectCfg(pcfg);
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
#ifdef __ONE_CLICK_CALL__
//一键呼叫接听协议对接
static int SetDeviceOneButtonCallCfg(const HY_DVR_DEVICE_ONE_BUTTON_CALL_CFG *pcfg)
{
     int nRet = IDVR_OK;    

    //本结构体是否有效，=0表示无效
    if (0 == pcfg->iValid)
    {
        MPU_DBG("0 == pcfg->bValid\n");
        return IDVR_CIU_STRUCT_UNVALID;
    }

    if (pcfg->iOneButtonCall > 1 || pcfg->iOneButtonCall < 0)
    {
        MPU_DBG("ioneButtonCall [0|1]\n");
        return IDVR_CIU_PARAM_OVERFLOW;
    }

    if (1 == pcfg->iOneButtonCall)//接听
    {
        CIU_CALL_SetRecvCallState(1);
    }
    else//挂断
    {
        CIU_CALL_SetRecvCallState(2);
    }

    return nRet;
}

int CIU_SetConfigure_DeviceOneButtonCallCfg(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, const BYTE * pBuffer, DWORD dwBufLen)
{
    int nRet = IDVR_OK;
    MPU_DBG("ID %d, Major %d, Minor %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType);

    switch (dwMinorType)
    {
        case DEVICE_ONE_BUTTON_CALL_CFG:
            {
                //判断buf空间是否足够
                if (dwBufLen < sizeof(HY_DVR_DEVICE_ONE_BUTTON_CALL_CFG))
                {
                    nRet = IDVR_CIU_BUF_SMALL;
                    MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_DEVICE_ONE_BUTTON_CALL_CFG), dwBufLen);
                    break;
                }
                HY_DVR_DEVICE_ONE_BUTTON_CALL_CFG *pcfg = (HY_DVR_DEVICE_ONE_BUTTON_CALL_CFG *) pBuffer;
                nRet = SetDeviceOneButtonCallCfg(pcfg);
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


