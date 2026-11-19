
/*!
*\file		MPU_CIU_RemoteCommand.cpp
*\brief	MPU模块，远程命令的实现
*\author	周庆丰
*\date		2009/08/24

功能：
-#	
-#	远程命令

说明：
-#	 
*/


#include "newhie/HieServer_Common.h"
#ifdef __SUPPORT_SD_CARD__
#include "STR/CDPManager.h"
#endif
#include "SnapShot/CSnapshotManager.h"
// #include "AVL/CChannelManager.h"
// #include "AVL/enumRecordMode.h"
// #include "PTZ/CPtzManager.h"
// #include "PTZ/CPtzAdjuster.h"
// #include "STR/CDiskManager.h"
// #include "STR/CDiskGroupManager.h"
// #include "AVL/enumRecType.h"
#include <semaphore.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/sysinfo.h>
#include <sys/stat.h>
#include "boottime.h"
#include "mysystem.h"
//#include "hisdk.h"
#include "zwsdk.h"
#include "MPU_CIU_global.h"
#include "pkg_interface.h"
#include "common_pub.h"
// #include "sys_banned.h"
#include "STR/CDiskManager.h"
#include "STR/CDiskGroupManager.h"
#include "AVL/enumRecType.h"
#include "AVL/CChannelManager.h"
#include "AVL/enumRecordMode.h"
#include "PTZ/CPtzManager.h"
#include "PTZ/CPtzAdjuster.h"
#ifdef __DIGITAL_ZOOM_DRAG_SUPPORT__
#include "CZoomManager.h"
#endif

#define ERR_COMMAND_OK      ((int)eQueryOK)	/*!< 操作成功 */
#define ERR_COMMAND_FAILED      ((int)eQueryFailed)	/*!<操作失败 */
#define ERR_COMMAND_BUSY      ((int)eQueryBusy)	/*!< 操作忙 */
#define ERR_COMMAND_FINISHED      ((int)eQueryFinished)	/*!< 操作结束 */



static pthread_t m_hCreateThread=0;	//异步工作线程句柄
static sem_t m_sem_create;		//异步工作线程信号

typedef struct _GET_DATA_SIZE_T_
{

	//操作类型
	DWORD dwOperationType;		// 0- 无操作,1-硬盘管理,2-历史流数据大小获取
	//硬盘管理
	DWORD dwDoDiskManageReady;	//是否准备好
	DWORD dwDiskManageUserID;
	eDiskGroupOperation eDiskManageOperationType;
	DWORD dwDiskManageParam[3];	//参数
	//获取历史流数据大小
	DWORD dwDataSizeReady;		//是否准备好
	DWORD dwDataSizeUserID;
	DWORD dwDataSizeDiskGroup;	//盘组
	DWORD dwDataSizeChannelBits;
	DWORD dwDataSizeStreamType;
	time_t tDataSizeStartTime;
	time_t tDataSizeStopTime;
	DWORD dwDataSizeDataSize;
	DWORD dwDataSizeUserData;
} GET_DATA_SIZE_T;

//远程分组管理配置
static GET_DATA_SIZE_T m_DiskMan_DataSize;

//远程操作回调函数
static RemoteControl_Callback m_RemoteControl_Callback_Fxn = NULL;


//远程操作异步处理线程
static void *Fnx_CommandCreateThread(void *lParam);

//!\brief      通知界面
//!\param   unsigned long dwUserID 用户ID
//!\param   unsigned long dwOperation 1-重启,2-关机,3-待机，4-回复系统默认
//  10-导入参数
//  11-视频预览通道别名变更
//  12-报警输入通道别名变更
//  13-报警输出通道别名变更
//  14-设备别名变更
//  15-用户权限变更
//!\return  0表示设置成功
int CIU_NotifyRemoteControl(unsigned long dwUserID, eRemoteControlNotify eOperation, unsigned long dwParam1)
{
	if (NULL != m_RemoteControl_Callback_Fxn)
	{
		MPU_DBG("call m_RemoteControl_Callback_Fxn\n");
		int nSDKRet = (*m_RemoteControl_Callback_Fxn) (dwUserID, eOperation, dwParam1);

		if (0 == nSDKRet)
		{
			return ERR_COMMAND_OK;
		}
		MPU_ERR("callback err\n");
	}
	MPU_ERR("callback is null\n");
	return ERR_COMMAND_FAILED;
}

//!\brief       设置远程控制回调函数
//!\param   RemoteControl_Callback 回调函数
//!\param   unsigned long dwUserID 用户ID
//!\param   unsigned long dwOperation 1-重启,2-关机,3-待机
//!\return  0表示设置成功
int CIU_SetRemoteControlCallback(RemoteControl_Callback pRemoteControlCallBack)
{
	m_RemoteControl_Callback_Fxn = pRemoteControlCallBack;
	return 0;
}

static int HY_DVR_DataExistCheck(DWORD dwUserID, DWORD dwChannel, DWORD dwMajorType, DWORD dwMinorType, char *szYearMonth, DWORD * dwResult)
{
	int iYear, iMonth, iDay;

	int DataMark = 0;

	int result;
#ifdef __SUPPORT_SD_CARD__
	CDPManager *m_pCDPmgr;
#endif

	if (szYearMonth == NULL || dwResult == NULL)
	{
		MPU_ERR("szYearMonth == NULL ||dwResult == NULL\n");
		return ERR_COMMAND_FAILED;
	}

	//时间
	sscanf(szYearMonth, "%04d%02d", &iYear, &iMonth);
	switch (dwMajorType)
	{
		case 1:				//图片查询,dwChannel为通道号
			{
				return ERR_COMMAND_FAILED;
				break;
			}
		case 0:				//历史流查询,dwChannel为盘组
			{
			#ifndef DEBUG_NOT_USE_STR_LIB
				m_pCDPmgr = CDPManager::GetInstance();
				if (m_pCDPmgr == NULL)
				{
					MPU_ERR("CDPManager::GetInstance() failed\n");
					return ERR_COMMAND_FAILED;
				}
				DataMark = 0;
				for (iDay = 1; iDay < 32; iDay++)
				{
				#ifdef USE_SD_PKG
					ZW_PkgSd_IsFileExist(iYear, iMonth, iDay, &result);
					result = (result == 0) ? 1 : 0;
				#else
					result = m_pCDPmgr->IsFileExist(dwChannel, iYear, iMonth, iDay);
				#endif
					if (result)
					{
						continue;
					}
					DataMark |= (1 << iDay);
				}
				*dwResult = (DataMark >> 1);
			#else
				return ERR_COMMAND_FAILED;
			#endif
				break;
			}
		default:
			{
				MPU_ERR("unsupported majortype = %d\n", (int) dwMajorType);
				*dwResult = 0;
				return ERR_COMMAND_FAILED;
				break;
			}
	}


	return ERR_COMMAND_OK;
}

static void *restoreFactory(void *parm)
{
    PRT_TID();
	pthread_detach(pthread_self());

	sleep(1);
	MPU_INFO("System start restore factory!!!\n");

	USR_ResumeAdminPassword();
	PAR_ForbiddenSaveFile();
	PAR_NET_Resume(__FUNCTION__);
	PAR_UnForbiddenSaveFile();

	pthread_exit(NULL);
}

#ifdef __IS_SUPPORT_LONG_SHORT_FOCUS_ZOOM__
static double matchProductRatio(int opt)
{
	double result = 1.0;

    double min_multiple = 1.0, max_multiple = 12.0;
    double shortZoom = 1.0, middleZoom = 1.0, longZoom = 1.0;
    ZWSys_GetEachFocusZoom(&shortZoom, &middleZoom, &longZoom);
    ZWSys_GetDragZoomRange(&min_multiple, &max_multiple);
   
#if defined(__PRODUCT_G85T__) || defined(__PRODUCT_W85T__)||defined(__PRODUCT_G43TA__)
    if(opt == 1) result = shortZoom;
    else if(opt == 2) result = middleZoom;
    else if(opt == 3 || opt == 10) result = longZoom;
#elif defined(__PRODUCT_G86T__)
    if(opt == 1) result = shortZoom;
    else if(opt == 2) result = middleZoom;
    else if(opt == 3) result = longZoom;
    else if(opt == 10) result = max_multiple;
#else
    result = 1.0;
#endif
    
    return result;
}
#endif

//远程重启
static int HY_DVR_RemoteReboot(DWORD dwUserID, eRemoteDeviceControl eOperationCode)
{ 
	MPU_DBG("<ID %d, CMD %d>\n", (int) dwUserID, (int) eOperationCode);
/**/
	//获取通道管理器句柄
	void *hChannelHandle = ZWSys_EncChannelOpen(0);
    if (eDeviceDetectBadPoint == eOperationCode)
    {
        return 0;
    }
	else if (eDeviceGammaDefault == eOperationCode)
	{
	    if(ZWSys_SetTuneToneCurve(hChannelHandle, 1, NULL))
	    {
	        MPU_ERR("Called ZWSys_SetTuneToneCurve error!\n");
	        return IDVR_CIU_CALLSDK_FAIL;
	    }
	    return 0;
	}
	else if (eDeviceRestoreFactory == eOperationCode)
	{
		pthread_t m_Thread;
		//异步处理，让网络回应
		pthread_create(&m_Thread, NULL, restoreFactory, NULL);
		return 0;
	}
	else if (eDeviceTryListenAlarmAudio == eOperationCode)
	{
        MPU_AVL_TryListenAlarmAudio(0, 1, 2, 5, 80, 5);

		return 0;
	}
	
	if((DWORD)-3 == dwUserID)//跟恢复密码工具软件的潜规则
	{
		if(eDeviceAdminDefaultPassword == eOperationCode)//功能跟密码复位一样
		{
			MPU_SYS_Led(IPRESET_HZ);
			USR_ResumeAdminPassword();
			//NET_Resume();
			char command_UserFile[256] = {0};
			memset(command_UserFile, 0, sizeof(command_UserFile));
			snprintf(command_UserFile, sizeof(command_UserFile), "cp -v %s %s", UserFile_BAK, UserFile);
			mysystem(command_UserFile); // resolv.conf(需要清空)
			/* 给boa发送消息告知已经有文件修改了 */
    		mysystem(SendMSGToBoa);

			MPU_SYS_Buzzer(BUZZER500);
			sleep(3);
			MPU_SYS_Led(NORMAL_HZ);
		}     
	}
	else
	{
		//检验远程用户权限
		if (!CIU_CheckRemoteUser(dwUserID, REMOTE_HALT))
		{
			MPU_ERR("<ID %d, CMD %d> no right\n", (int) dwUserID, (int) eOperationCode);
			return IDVR_CIU_NO_RIGHT;
		}
		MPU_DBG("----HY_DVR_RemoteReboot--have right \n");

		if(eOperationCode <= eDeviceSendTestEmail)
		{
			//通知GUI处理重启
			return CIU_NotifyRemoteControl(dwUserID, (eRemoteControlNotify) eOperationCode, 0);
		}
	}  
	return 0;
}

/* return 0:触发成功 -8:触发失败 -14:有事件正在处理 */
static int HY_DVR_TriggerRecordEvent(DWORD dwUserID, DWORD dwChannel, DWORD dwRecordType, DWORD dwUserData)
{    
#if 0
    int ret = 0;
	CChannelManager *pcm;

    int bEnable = 0;

    struct timeval now_tv;
    
	//检验参数有效性
	if (dwChannel < 0 || dwChannel >= DVR_MAX_VIDEOIN_NUM)
	{
		MPU_ERR("invalid channel %d\n", (int) dwChannel);
		return ERR_COMMAND_FAILED;
	}

	//获取通道管理器句柄
	pcm = CChannelManager::GetInstanceHandle();
	if (pcm == NULL)
	{
		MPU_ERR("CChannelManager::GetInstanceHandle() failed\n");
		return ERR_COMMAND_FAILED;
	}

	ret = pcm->HttpTriggerRecord(0);
	MPU_NET_TrigSendHttpUrlInfo(0);

	return ret;
#endif
	return 0;
}


//远程强制I帧
static int HY_DVR_RemoteForceIFrame(DWORD dwUserID, DWORD dwChannel)
{
	int result;

	CChannelManager *pcm;

	//检验参数有效性
	if (dwChannel < 0 || dwChannel >= DVR_MAX_VIDEOIN_NUM)
	{
		MPU_ERR("invalid channel %d\n", (int) dwChannel);
		return ERR_COMMAND_FAILED;
	}

	//获取通道管理器句柄
	pcm = CChannelManager::GetInstanceHandle();
	if (pcm == NULL)
	{
		MPU_ERR("CChannelManager::GetInstanceHandle() failed\n");
		return ERR_COMMAND_FAILED;
	}

	//强制I帧
    if (dwChannel == 0)
    {
        result = pcm->EncCaptureIFrame(dwChannel, 0);
    }
    else if (dwChannel == 1)
    {
    	dwChannel = 0;
    	result = pcm->EncCaptureIFrame(dwChannel, 1);
    }
    else if (dwChannel == 2)
    {
    	dwChannel = 0;
    	result = pcm->EncCaptureIFrame(dwChannel, 2);
    }
    else if (dwChannel == 3)
    {
    	dwChannel = 0;
    	result = pcm->EncCaptureIFrame(dwChannel, 3);
    }

	if (result)
	{
		return ERR_COMMAND_FAILED;
	}

	return ERR_COMMAND_OK;
}

//远程清除报警
static int HY_DVR_RemoteAlarmClear(DWORD dwUserID, DWORD dwChannel, DWORD dwAlarmType)
{
	MPU_DBG("<ID %d, Channel %d, AlarmType %d>\n", (int) dwUserID, (int) dwChannel, (int) dwAlarmType);

	//只有Admin用户允许清除报警
	if (!CIU_IsUser(dwUserID, ADMIN_USRNAME))
	{
		MPU_ERR("<ID %d, Channel %d, AlarmType %d> no right\n", (int) dwUserID, (int) dwChannel, (int) dwAlarmType);
		return IDVR_CIU_NO_RIGHT;
	}

	//通知界面清除报警
	return CIU_NotifyRemoteControl(dwUserID, eRemoteClearAlarm, 0);
}

//远程抓图
static int HY_DVR_RemoteImageCapture(DWORD dwUserID, DWORD dwChannel, eImageFormat dwImageFormat, LPImageFileInfo pImageFileInfo)
{
	int result;

	CChannelManager *pcm;

	if (dwChannel < 0 || dwChannel >= DVR_MAX_VIDEOIN_NUM)
	{
		MPU_ERR("invalid channel %lu\n", dwChannel);
		return ERR_COMMAND_FAILED;
	}

    /* 当前抓图暂时只支持JPEG抓图 */
    if (dwImageFormat != eJpgFormat)
    {
		MPU_ERR("Only Support Jpeg Picture Format! Not Support Picture Format %d\n", dwImageFormat);
		return ERR_COMMAND_FAILED;
    }

	pcm = CChannelManager::GetInstanceHandle();
	if (pcm == NULL)
	{
		MPU_ERR("CChannelManager::GetInstanceHandle() failed\n");
		return ERR_COMMAND_FAILED;
	}

	result = pcm->Pic_Capture(dwChannel, 0, 0, 0);
	if (result)
	{
		MPU_ERR("ch = %d Pic_Capture failed(%d)\n", (int) dwChannel, result);
		return ERR_COMMAND_FAILED;
	}
#if 0
	//不提供信息
	pImageFileInfo->m_dwFileSize = 0;
	pImageFileInfo->m_sCreateTime[0] = 0;
	pImageFileInfo->m_sFileName[0] = 0;
#else
	struct stat buf;

	strncpy(pImageFileInfo->m_sFileName,REMOTE_CAPTURE_IMAGE_FILE_NAME,sizeof(pImageFileInfo->m_sFileName) - 1);
	if (stat((const char *)pImageFileInfo->m_sFileName, &buf) != -1)
	{
		MPU_DBG("File %s size = %d\n", pImageFileInfo->m_sFileName, (int) buf.st_size);
	}
	pImageFileInfo->m_dwFileSize = buf.st_size;
	pImageFileInfo->m_sCreateTime[0] = 0;
#endif
	return ERR_COMMAND_OK;
}

static int HY_DVR_PtzControl(DWORD dwUserID, DWORD  dwChannel, ePTZControlCmdCode eCommandCode, DWORD dwParam1, DWORD dwParam2, DWORD dwParam3,
							 DWORD dwParam4)
{
	int result;
	printf("[H4W] HY_DVR_PtzControl eCommandCode = %d \n", eCommandCode);
	CPtzManager *pPtz;

	DWORD dwIP = 0;

	char szIP[20];

	char szName[20] = "";

	MPU_DBG("PTZ ..uid = %d ch = %d,cmd = %d,wp1 = %d ,wp2 = %d ,wp3 = %d, wp4 = %d\n",
		 (int) dwUserID, (int) dwChannel, (int) eCommandCode, (int) dwParam1, (int) dwParam2, (int) dwParam3, (int) dwParam4);

	if (dwParam4 == 0xFFFFFFFE)
	{//2 sd卡
		int  Num=0;
		int ret = 1;
		MPU_STR_Disk_GetNum(&Num);
		if (Num == 1)
		{
			ret = MPU_STR_Disk_GetDiskStatus(0);			
		}

		MPU_DBG("SD ret = %d\n",(ret != 0 && ret != 1));
		
		return ((ret != 0 && ret != 1));
	}
	else if (dwParam4 == 0xFFFFFFFD)
	{//2log
		mysystem("touch /tmp/app/clearlog");
		return 0;
	}
	else if (dwParam4 == 0xFFFFFFFC)
	{//2ircut
		// MP4Sys_SetIR_CUT(1);
		 return 0;
	}
	else if (dwParam4 == 0xFFFFFFFB)
	{//2ircut
		// MP4Sys_SetIR_CUT(0);
		 return 0;
	}


	if (!MPU_CIU_CheckRemoteUser(dwUserID, CHANNEL_PTZCTRL, dwChannel))
	{
		MPU_ERR("HY_DVR_PtzControl uid = %d ch = %d no right\n", (int) dwUserID, (int) dwChannel);
		return IDVR_CIU_NO_RIGHT;
	}
	pPtz = CPtzManager::PTZ_GetInstance();
	if (NULL == pPtz)
	{
		MPU_ERR("CPtzManager::PTZ_GetInstance() failed \n");
		return ERR_COMMAND_FAILED;
	}


	result = MPU_CIU_GetRemoteByID(dwUserID, NULL, szName, sizeof(szName), szIP, sizeof(szIP), NULL, 0);
	if (!result)
	{
		dwIP = inet_addr(szIP);
#ifdef IDVR_LIL_ENDIAN
		dwIP = iDvrArchSwap32(dwIP);
#endif

	}

	{
		#if defined (__IS_SUPPORT_PTZ_BUILTIN__) && defined (__IS_SUPPORT_PTZ_BUILTIN_HUMAN_TRACK__)
		//内置云台，通过NVR操作时，先设置下速度
		if (ePTZControlCodeUp == eCommandCode || ePTZControlCodeDown == eCommandCode
			|| ePTZControlCodeLeft == eCommandCode || ePTZControlCodeRight == eCommandCode)
		{
			if (MPU_PTZ_GetIsSupportMoto() && dwParam2 > 0)
			{
				MPU_PTZ_SetSpeed(dwParam2,0);
			}
		}
		#endif

		MPU_ERR("PTZ model\n");
		switch (eCommandCode)
		{
			case ePTZControlCodeAllOff:	//    0    －  关闭(或断开)所有开关    (Param1: 1-开,0-停止, Param2: 无效, Param3: 无效, Param4: 无效)    
			{
				int bOn = (dwParam1) & 1;	//当前状态

				result = pPtz->PTZ_AuxCtrl(TTY_AUX_POWER, bOn, dwChannel);
				result |= pPtz->PTZ_AuxCtrl(TTY_AUX_LIGHT, bOn, dwChannel);
				result |= pPtz->PTZ_AuxCtrl(TTY_AUX_WIPER, bOn, dwChannel);
				result |= pPtz->PTZ_AuxCtrl(TTY_AUX_HEATER, bOn, dwChannel);
				if (result)
				{
					MPU_DBG
						("[%d]PTZ_AuxCtrl<cmd =%d,p %d,%d,%d,%d> failed (%d)\n",
						 (int) dwChannel, (int) eCommandCode, (int) dwParam1, (int) dwParam2, (int) dwParam3, (int) dwParam4, result);
					//  return ERR_COMMAND_FAILED;
				}


				MPU_OPL_AddLog(EDVR_EVENT_OPT, EDVR_EVENT_OPT_DETAIL_PTZ_CTL, 0, 0, dwIP, szName);

				break;
			}
			case ePTZControlCodeCameraPower:	//    1    －  接通摄像机电源      (Param1: 1-开始/0-停止, Param2: 无效, Param3: 无效, Param4: 无效)   
			{

				result = pPtz->PTZ_AuxCtrl(TTY_AUX_POWER, dwParam1, dwChannel);

				if (result)
				{
					MPU_ERR
						("[%d]PTZ_AuxCtrl<cmd =%d,p %d,%d,%d,%d> failed (%d)\n",
						 (int) dwChannel, (int) eCommandCode, (int) dwParam1, (int) dwParam2, (int) dwParam3, (int) dwParam4, result);
					return ERR_COMMAND_FAILED;
				}
				MPU_OPL_AddLog(EDVR_EVENT_OPT, EDVR_EVENT_OPT_DETAIL_PTZ_CTL, 0, 0, dwIP, szName);
				break;
			}
			case ePTZControlCodeLightPower:	//    2    －  接通灯光电源            (Param1: 1-开始/0-停止, Param2: 无效, Param3: 无效, Param4: 无效)   
			{
				result = pPtz->PTZ_AuxCtrl(TTY_AUX_LIGHT, dwParam1, dwChannel);

				if (result)
				{
					MPU_ERR
						("[%d]PTZ_AuxCtrl<cmd =%d,p %d,%d,%d,%d> failed (%d)\n",
						 (int) dwChannel, (int) eCommandCode, (int) dwParam1, (int) dwParam2, (int) dwParam3, (int) dwParam4, result);
					return ERR_COMMAND_FAILED;
				}
				MPU_OPL_AddLog(EDVR_EVENT_OPT, EDVR_EVENT_OPT_DETAIL_PTZ_CTL, 0, 0, dwIP, szName);
				break;
			}
			case ePTZControlCodeWiper:	//    3    －  接通雨刷开关            (Param1: 1-开始/0-停止, Param2: 无效, Param3: 无效, Param4: 无效)   
			{
				result = pPtz->PTZ_AuxCtrl(TTY_AUX_WIPER, dwParam1, dwChannel);

				if (result)
				{
					MPU_ERR
						("[%d]PTZ_AuxCtrl<cmd =%d,p %d,%d,%d,%d> failed (%d)\n",
						 (int) dwChannel, (int) eCommandCode, (int) dwParam1, (int) dwParam2, (int) dwParam3, (int) dwParam4, result);
					return ERR_COMMAND_FAILED;
				}
				MPU_OPL_AddLog(EDVR_EVENT_OPT, EDVR_EVENT_OPT_DETAIL_PTZ_CTL, 0, 0, dwIP, szName);
				break;
			}
			case ePTZControlCodeFans:	//    4    －  接通风扇开关            (Param1: 1-开始/0-停止, Param2: 无效, Param3: 无效, Param4: 无效)   
			{

				break;
			}
			case ePTZControlCodeHeater:	//    5    －  接通加热器开关      (Param1: 1-开始/0-停止, Param2: 无效, Param3: 无效, Param4: 无效)   
			{
				result = pPtz->PTZ_AuxCtrl(TTY_AUX_HEATER, dwParam1, dwChannel);

				if (result)
				{
					MPU_ERR
						("[%d]PTZ_AuxCtrl<cmd =%d,p %d,%d,%d,%d> failed (%d)\n",
						 (int) dwChannel, (int) eCommandCode, (int) dwParam1, (int) dwParam2, (int) dwParam3, (int) dwParam4, result);
					return ERR_COMMAND_FAILED;
				}
				MPU_OPL_AddLog(EDVR_EVENT_OPT, EDVR_EVENT_OPT_DETAIL_PTZ_CTL, 0, 0, dwIP, szName);
				break;
			}
			case ePTZControlCodeAuxEquiment:	//    6    －  接通辅助设备开关        (Param1: 1-开始/0-停止, Param2: 无效, Param3: 无效, Param4: 无效)   
			{

				break;
			}

			case ePTZControlCodeStopContinue:	//     10  －  停止所有连续量(镜头,云台)动作   (Param1: 无效, Param2: 无效, Param3: 无效, Param4: 无效)    
			{
				result = pPtz->PTZ_NormalCtrl(TTY_PTZ_STOP, dwParam2, dwChannel);

				if (result)
				{
					MPU_ERR
						("[%d]PTZ_NormalCtrl<cmd =%d,p %d,%d,%d,%d> failed (%d)\n",
						 (int) dwChannel, (int) eCommandCode, (int) dwParam1, (int) dwParam2, (int) dwParam3, (int) dwParam4, result);
					return ERR_COMMAND_FAILED;
				}
				MPU_OPL_AddLog(EDVR_EVENT_OPT, EDVR_EVENT_OPT_DETAIL_PTZ_CTL, 0, 0, dwIP, szName);
				break;

			}
			case ePTZControlCodeZoomIn:	//     11  －  焦距变大(倍率变大)      (Param1: 1-开始/0-停止, Param2: 速度 [0-10,0表示默认速度,1-10表示速度级别], Param3: 无效, Param4: 无效) 
			{
				if (dwParam1)
				{
					#ifdef __DIGITAL_ZOOM_DRAG_SUPPORT__
                    CZoomManager * czm = CZoomManager::GetInstance();
					#if defined(__IS_SUPPORT_LONG_SHORT_FOCUS_ZOOM__)
						if (0 == access(PRODUCTTEST_FILE, F_OK) || pPtz->PTZ_GetProductTestMode())
						{
							result = czm->SetRatio(matchProductRatio(dwParam4), DRAG_CLIENT_RADIO);
						}
						else
						{
							result = czm->SetDragZoomCmd(DRAG_CMD_ZOOMIN, DRAG_CLIENT_ZOOM);
						}
					#else
                        result = czm->SetDragZoomCmd(DRAG_CMD_ZOOMIN, DRAG_CLIENT_ZOOM);
                    #endif

                    #else
                    result = pPtz->PTZ_NormalCtrl(TTY_PTZ_ZOOMIN, dwParam2, dwChannel);
                    #endif
				}
				else
				{
					#ifdef __DIGITAL_ZOOM_DRAG_SUPPORT__
                    CZoomManager * czm = CZoomManager::GetInstance();
                    result = czm ->SetDragZoomCmd(DRAG_CMD_ZOOM_STOP, DRAG_CLIENT_ZOOM);
					#else
					result = pPtz->PTZ_NormalCtrl(TTY_PTZ_ZOOM_STOP, dwParam2, dwChannel);
					#endif
				}
				if (result)
				{
					MPU_ERR
						("[%d]PTZ_NormalCtrl<cmd =%d,p %d,%d,%d,%d> failed (%d)\n",
						 (int) dwChannel, (int) eCommandCode, (int) dwParam1, (int) dwParam2, (int) dwParam3, (int) dwParam4, result);
					return ERR_COMMAND_FAILED;
				}
				MPU_OPL_AddLog(EDVR_EVENT_OPT, EDVR_EVENT_OPT_DETAIL_PTZ_CTL, 0, 0, dwIP, szName);
				break;
			}
			case ePTZControlCodeZoomOut:	//     12  －  焦距变小(倍率变小)      (Param1: 1-开始/0-停止, Param2: 速度 [0-10,0表示默认速度,1-10表示速度级别], Param3: 无效, Param4: 无效) 
			{
				if (dwParam1)
				{
					#ifdef __DIGITAL_ZOOM_DRAG_SUPPORT__
                    CZoomManager * czm = CZoomManager::GetInstance();
					#if defined(__IS_SUPPORT_LONG_SHORT_FOCUS_ZOOM__)
						if (0 == access(PRODUCTTEST_FILE, F_OK) || pPtz->PTZ_GetProductTestMode())
						{
							result = czm->SetRatio(matchProductRatio(dwParam4), DRAG_CLIENT_RADIO);
						}
						else
						{
							result = czm->SetDragZoomCmd(DRAG_CMD_ZOOMOUT, DRAG_CLIENT_ZOOM);
						}
					#else
                        result = czm->SetDragZoomCmd(DRAG_CMD_ZOOMOUT, DRAG_CLIENT_ZOOM);
                    #endif

					#else
					result = pPtz->PTZ_NormalCtrl(TTY_PTZ_ZOOMOUT, dwParam2, dwChannel);
					#endif
				}
				else
				{
					#ifdef __DIGITAL_ZOOM_DRAG_SUPPORT__
                    CZoomManager * czm = CZoomManager::GetInstance();
                    result = czm ->SetDragZoomCmd(DRAG_CMD_ZOOM_STOP, DRAG_CLIENT_ZOOM);
					#else
					result = pPtz->PTZ_NormalCtrl(TTY_PTZ_ZOOM_STOP, dwParam2, dwChannel);
					#endif
				}
				if (result)
				{
					MPU_ERR
						("[%d]PTZ_NormalCtrl<cmd =%d,p %d,%d,%d,%d> failed (%d)\n",
						 (int) dwChannel, (int) eCommandCode, (int) dwParam1, (int) dwParam2, (int) dwParam3, (int) dwParam4, result);
					return ERR_COMMAND_FAILED;
				}
				MPU_OPL_AddLog(EDVR_EVENT_OPT, EDVR_EVENT_OPT_DETAIL_PTZ_CTL, 0, 0, dwIP, szName);
				break;
			}
			case ePTZControlCodeFocusNear:	//     13  －  焦点前调                (Param1: 1-开始/0-停止, Param2: 速度 [0-10,0表示默认速度,1-10表示速度级别], Param3: 无效, Param4: 无效) 
			{

				if (dwParam1)
				{
					result = pPtz->PTZ_NormalCtrl(TTY_PTZ_FOCUS_NEAR, dwParam2, dwChannel);
				}
				else
				{
					result = pPtz->PTZ_NormalCtrl(TTY_PTZ_FOCUS_STOP, dwParam2, dwChannel);
				}
				if (result)
				{
					MPU_ERR
						("[%d]PTZ_NormalCtrl<cmd =%d,p %d,%d,%d,%d> failed (%d)\n",
						 (int) dwChannel, (int) eCommandCode, (int) dwParam1, (int) dwParam2, (int) dwParam3, (int) dwParam4, result);
					return ERR_COMMAND_FAILED;
				}
				MPU_OPL_AddLog(EDVR_EVENT_OPT, EDVR_EVENT_OPT_DETAIL_PTZ_CTL, 0, 0, dwIP, szName);

				break;
			}
			case ePTZControlCodeFocusFar:	//     14  －  焦点后调                (Param1: 1-开始/0-停止, Param2: 速度 [0-10,0表示默认速度,1-10表示速度级别], Param3: 无效, Param4: 无效) 
			{
				if (dwParam1)
				{
					result = pPtz->PTZ_NormalCtrl(TTY_PTZ_FOCUS_FAR, dwParam2, dwChannel);
				}
				else
				{
					result = pPtz->PTZ_NormalCtrl(TTY_PTZ_FOCUS_STOP, dwParam2, dwChannel);
				}
				if (result)
				{
					MPU_ERR
						("[%d]PTZ_NormalCtrl<cmd =%d,p %d,%d,%d,%d> failed (%d)\n",
						 (int) dwChannel, (int) eCommandCode, (int) dwParam1, (int) dwParam2, (int) dwParam3, (int) dwParam4, result);
					return ERR_COMMAND_FAILED;
				}
				MPU_OPL_AddLog(EDVR_EVENT_OPT, EDVR_EVENT_OPT_DETAIL_PTZ_CTL, 0, 0, dwIP, szName);
				break;
			}
			case ePTZControlCodeApertureUp:	//     15  －  光圈扩大                (Param1: 1-开始/0-停止, Param2: 速度 [0-10,0表示默认速度,1-10表示速度级别], Param3: 无效, Param4: 无效) 
			{
				if (dwParam1)
				{
					result = pPtz->PTZ_NormalCtrl(TTY_PTZ_IRIS_LARGE, dwParam2, dwChannel);
				}
				else
				{
					result = pPtz->PTZ_NormalCtrl(TTY_PTZ_IRIS_STOP, dwParam2, dwChannel);
				}
				if (result)
				{
					MPU_ERR
						("[%d]PTZ_NormalCtrl<cmd =%d,p %d,%d,%d,%d> failed (%d)\n",
						 (int) dwChannel, (int) eCommandCode, (int) dwParam1, (int) dwParam2, (int) dwParam3, (int) dwParam4, result);
					return ERR_COMMAND_FAILED;
				}
				MPU_OPL_AddLog(EDVR_EVENT_OPT, EDVR_EVENT_OPT_DETAIL_PTZ_CTL, 0, 0, dwIP, szName);
				break;
			}
			case ePTZControlCodeApertureDown:	//     16  －  光圈缩小                (Param1: 1-开始/0-停止, Param2: 速度 [0-10,0表示默认速度,1-10表示速度级别], Param3: 无效, Param4: 无效) 
			{
				if (dwParam1)
				{
					result = pPtz->PTZ_NormalCtrl(TTY_PTZ_IRIS_SMALL, dwParam2, dwChannel);
				}
				else
				{
					result = pPtz->PTZ_NormalCtrl(TTY_PTZ_IRIS_STOP, dwParam2, dwChannel);
				}
				if (result)
				{
					MPU_ERR
						("[%d]PTZ_NormalCtrl<cmd =%d,p %d,%d,%d,%d> failed (%d)\n",
						 (int) dwChannel, (int) eCommandCode, (int) dwParam1, (int) dwParam2, (int) dwParam3, (int) dwParam4, result);
					return ERR_COMMAND_FAILED;
				}
				MPU_OPL_AddLog(EDVR_EVENT_OPT, EDVR_EVENT_OPT_DETAIL_PTZ_CTL, 0, 0, dwIP, szName);
				break;
			}
			case ePTZControlCodeAutoZoom:	//     17  －  开自动焦距(自动倍率)    (Param1: 1-开始/0-停止, Param2: 无效, Param3: 无效, Param4: 无效)   
			{
				break;
			}
			case ePTZControlCodeAutoFocus:	//     18  －  开自动调焦          (Param1: 1-开始/0-停止, Param2: 无效, Param3: 无效, Param4: 无效)   
			{
				break;
			}
			case ePTZControlCodeAutoAperture:	//     19  －  开自动光圈          (Param1: 1-开始/0-停止, Param2: 无效, Param3: 无效, Param4: 无效)   
			{
				break;
			}
			case ePTZControlCodeUp:	//     21  －  云台上仰                (Param1: 1-开始/0-停止, Param2: 速度 [0-10,0表示默认速度,1-10表示速度级别], Param3: 1表示电子PTZ,0表示传统PTZ, Param4: 无效) 
			{
				if (dwParam1)
				{
				#if !defined (__IS_SUPPORT_PTZ_BUILTIN__) || !defined (__IS_SUPPORT_PTZ_BUILTIN_HUMAN_TRACK__)
					if (3 == nMirrorMode || 2 == nMirrorMode) //如果是翻转状态或者中心对称状态，上下需要颠倒
					{
						result = pPtz->PTZ_NormalCtrl(TTY_PTZ_DOWN, dwParam2, dwChannel);
					}
					else
				#endif
					{
						result = pPtz->PTZ_NormalCtrl(TTY_PTZ_UP, dwParam2, dwChannel);
					}
				}
				else
				{
				#if !defined (__IS_SUPPORT_PTZ_BUILTIN__) || !defined (__IS_SUPPORT_PTZ_BUILTIN_HUMAN_TRACK__)
					if (3 == nMirrorMode || 2 == nMirrorMode)//如果是翻转状态或者中心对称状态，上下需要颠倒
					{
						result = pPtz->PTZ_NormalCtrl(TTY_PTZ_DOWN_STOP, dwParam2, dwChannel);
					}
					else
				#endif
					{
						result = pPtz->PTZ_NormalCtrl(TTY_PTZ_UP_STOP, dwParam2, dwChannel);
					}
				}
				if (result)
				{
					MPU_ERR("[%d]PTZ_NormalCtrl<cmd =%d,p %d,%d,%d,%d> failed (%d)\n",
						 (int) dwChannel, (int) eCommandCode, (int) dwParam1, (int) dwParam2, (int) dwParam3, (int) dwParam4, result);
					return ERR_COMMAND_FAILED;
				}
				MPU_OPL_AddLog(EDVR_EVENT_OPT, EDVR_EVENT_OPT_DETAIL_PTZ_CTL, 0, 0, dwIP, szName);
				break;
			}
			case ePTZControlCodeDown:	//     22  －  云台下俯                (Param1: 1-开始/0-停止, Param2: 速度 [0-10,0表示默认速度,1-10表示速度级别], Param3: 无效, Param4: 无效) 
			{
				if (dwParam1)
				{
				#if !defined (__IS_SUPPORT_PTZ_BUILTIN__) || !defined (__IS_SUPPORT_PTZ_BUILTIN_HUMAN_TRACK__)
					if (3 == nMirrorMode || 2 == nMirrorMode)//如果是翻转状态或者中心对称状态，上下需要颠倒
					{
						result = pPtz->PTZ_NormalCtrl(TTY_PTZ_UP, dwParam2, dwChannel);
					}
					else
				#endif
					{		
						result = pPtz->PTZ_NormalCtrl(TTY_PTZ_DOWN, dwParam2, dwChannel);
					}
				}
				else
				{
				#if !defined (__IS_SUPPORT_PTZ_BUILTIN__) || !defined (__IS_SUPPORT_PTZ_BUILTIN_HUMAN_TRACK__)
					if (3 == nMirrorMode || 2 == nMirrorMode)//如果是翻转状态或者中心对称状态，上下需要颠倒
					{
						result = pPtz->PTZ_NormalCtrl(TTY_PTZ_UP_STOP, dwParam2, dwChannel);
					}
					else
				#endif
					{		
						result = pPtz->PTZ_NormalCtrl(TTY_PTZ_DOWN_STOP, dwParam2, dwChannel);
					}
				}
				if (result)
				{
					MPU_ERR("[%d]PTZ_NormalCtrl<cmd =%d,p %d,%d,%d,%d> failed (%d)\n",
						 (int) dwChannel, (int) eCommandCode, (int) dwParam1, (int) dwParam2, (int) dwParam3, (int) dwParam4, result);
					return ERR_COMMAND_FAILED;
				}
				MPU_OPL_AddLog(EDVR_EVENT_OPT, EDVR_EVENT_OPT_DETAIL_PTZ_CTL, 0, 0, dwIP, szName);
				break;
			}
			case ePTZControlCodeLeft:	//     23  －  云台左转                (Param1: 1-开始/0-停止, Param2: 速度 [0-10,0表示默认速度,1-10表示速度级别], Param3: 无效, Param4: 无效) 
			{
				if (dwParam1)
				{
				#if !defined (__IS_SUPPORT_PTZ_BUILTIN__) || !defined (__IS_SUPPORT_PTZ_BUILTIN_HUMAN_TRACK__)
					if (3 == nMirrorMode || 1 == nMirrorMode)//如果是镜像状态或者中心对称状态，左右需要颠倒
					{
						result = pPtz->PTZ_NormalCtrl(TTY_PTZ_RIGHT, dwParam2, dwChannel);
					}
					else
				#endif
					{		
						result = pPtz->PTZ_NormalCtrl(TTY_PTZ_LEFT, dwParam2, dwChannel);
					}
				}
				else
				{
				#if !defined (__IS_SUPPORT_PTZ_BUILTIN__) || !defined (__IS_SUPPORT_PTZ_BUILTIN_HUMAN_TRACK__)
					if (3 == nMirrorMode || 1 == nMirrorMode)//如果是镜像状态或者中心对称状态，左右需要颠倒
					{
						result = pPtz->PTZ_NormalCtrl(TTY_PTZ_RIGHT_STOP, dwParam2, dwChannel);
					}
					else
				#endif
					{		
						result = pPtz->PTZ_NormalCtrl(TTY_PTZ_LEFT_STOP, dwParam2, dwChannel);
					}
				}
				if (result)
				{
					MPU_ERR("[%d]PTZ_NormalCtrl<cmd =%d,p %d,%d,%d,%d> failed (%d)\n",
						 (int) dwChannel, (int) eCommandCode, (int) dwParam1, (int) dwParam2, (int) dwParam3, (int) dwParam4, result);
					return ERR_COMMAND_FAILED;
				}
				MPU_OPL_AddLog(EDVR_EVENT_OPT, EDVR_EVENT_OPT_DETAIL_PTZ_CTL, 0, 0, dwIP, szName);
				break;
			}
			case ePTZControlCodeRight:	//     24  －  云台右转                (Param1: 1-开始/0-停止, Param2: 速度 [0-10,0表示默认速度,1-10表示速度级别], Param3: 无效, Param4: 无效) 
			{
				if (dwParam1)
				{
				#if !defined (__IS_SUPPORT_PTZ_BUILTIN__) || !defined (__IS_SUPPORT_PTZ_BUILTIN_HUMAN_TRACK__)
					if (3 == nMirrorMode || 1 == nMirrorMode)//如果是镜像状态或者中心对称状态，左右需要颠倒
					{
						result = pPtz->PTZ_NormalCtrl(TTY_PTZ_LEFT, dwParam2, dwChannel);
					}
					else
				#endif
					{		
						result = pPtz->PTZ_NormalCtrl(TTY_PTZ_RIGHT, dwParam2, dwChannel);
					}
				}
				else
				{
				#if !defined (__IS_SUPPORT_PTZ_BUILTIN__) || !defined (__IS_SUPPORT_PTZ_BUILTIN_HUMAN_TRACK__)
					if (3 == nMirrorMode || 1 == nMirrorMode)//如果是镜像状态或者中心对称状态，左右需要颠倒
					{
						result = pPtz->PTZ_NormalCtrl(TTY_PTZ_LEFT_STOP, dwParam2, dwChannel);
					}
					else
				#endif
					{		
						result = pPtz->PTZ_NormalCtrl(TTY_PTZ_RIGHT_STOP, dwParam2, dwChannel);
					}
				}
				if (result)
				{
					MPU_ERR("[%d]PTZ_NormalCtrl<cmd =%d,p %d,%d,%d,%d> failed (%d)\n",
						 (int) dwChannel, (int) eCommandCode, (int) dwParam1, (int) dwParam2, (int) dwParam3, (int) dwParam4, result);
					return ERR_COMMAND_FAILED;
				}
				MPU_OPL_AddLog(EDVR_EVENT_OPT, EDVR_EVENT_OPT_DETAIL_PTZ_CTL, 0, 0, dwIP, szName);
				break;
			}
			case ePTZControlCodeUpLeft:	//     25  －  云台上仰和左转      (Param1: 1-开始/0-停止, Param2: 速度 [0-10,0表示默认速度,1-10表示速度级别], Param3: 无效, Param4: 无效) 
			{
				if (dwParam1)
				{
					result = pPtz->PTZ_NormalCtrl(TTY_PTZ_LEFTUP, dwParam2, dwChannel);
				}
				else
				{
					result = pPtz->PTZ_NormalCtrl(TTY_PTZ_LEFTUP_STOP, dwParam2, dwChannel);
				}
				if (result)
				{
					MPU_ERR("[%d]PTZ_NormalCtrl<cmd =%d,p %d,%d,%d,%d> failed (%d)\n",
						 (int) dwChannel, (int) eCommandCode, (int) dwParam1, (int) dwParam2, (int) dwParam3, (int) dwParam4, result);
					return ERR_COMMAND_FAILED;
				}
				MPU_OPL_AddLog(EDVR_EVENT_OPT, EDVR_EVENT_OPT_DETAIL_PTZ_CTL, 0, 0, dwIP, szName);
				break;
			}
			case ePTZControlCodeUpRight:	//     26  －  云台上仰和右转      (Param1: 1-开始/0-停止, Param2: 速度 [0-10,0表示默认速度,1-10表示速度级别], Param3: 无效, Param4: 无效) 
			{
				if (dwParam1)
				{
					result = pPtz->PTZ_NormalCtrl(TTY_PTZ_RIGHTUP, dwParam2, dwChannel);
				}
				else
				{
					result = pPtz->PTZ_NormalCtrl(TTY_PTZ_RIGHTUP_STOP, dwParam2, dwChannel);
				}
				if (result)
				{
					MPU_ERR("[%d]PTZ_NormalCtrl<cmd =%d,p %d,%d,%d,%d> failed (%d)\n",
						 (int) dwChannel, (int) eCommandCode, (int) dwParam1, (int) dwParam2, (int) dwParam3, (int) dwParam4, result);
					return ERR_COMMAND_FAILED;
				}
				MPU_OPL_AddLog(EDVR_EVENT_OPT, EDVR_EVENT_OPT_DETAIL_PTZ_CTL, 0, 0, dwIP, szName);
				break;
			}
			case ePTZControlCodeDownLeft:	//     27  －  云台下俯和左转      (Param1: 1-开始/0-停止, Param2: 速度 [0-10,0表示默认速度,1-10表示速度级别], Param3: 无效, Param4: 无效) 
			{
				if (dwParam1)
				{
					result = pPtz->PTZ_NormalCtrl(TTY_PTZ_LEFTDOWN, dwParam2, dwChannel);
				}
				else
				{
					result = pPtz->PTZ_NormalCtrl(TTY_PTZ_LEFTDOWN_STOP, dwParam2, dwChannel);
				}
				if (result)
				{
					MPU_ERR("[%d]PTZ_NormalCtrl<cmd =%d,p %d,%d,%d,%d> failed (%d)\n",
						 (int) dwChannel, (int) eCommandCode, (int) dwParam1, (int) dwParam2, (int) dwParam3, (int) dwParam4, result);
					return ERR_COMMAND_FAILED;
				}
				MPU_OPL_AddLog(EDVR_EVENT_OPT, EDVR_EVENT_OPT_DETAIL_PTZ_CTL, 0, 0, dwIP, szName);
				break;
			}
			case ePTZControlCodeDownRight:	//     28  －  云台下俯和右转      (Param1: 1-开始/0-停止, Param2: 速度 [0-10,0表示默认速度,1-10表示速度级别], Param3: 无效, Param4: 无效) 
			{
				if (dwParam1)
				{
					result = pPtz->PTZ_NormalCtrl(TTY_PTZ_RIGHTDOWN, dwParam2, dwChannel);
				}
				else
				{
					result = pPtz->PTZ_NormalCtrl(TTY_PTZ_RIGHTDOWN_STOP, dwParam2, dwChannel);
				}
				if (result)
				{
					MPU_ERR("[%d]PTZ_NormalCtrl<cmd =%d,p %d,%d,%d,%d> failed (%d)\n",
						 (int) dwChannel, (int) eCommandCode, (int) dwParam1, (int) dwParam2, (int) dwParam3, (int) dwParam4, result);
					return ERR_COMMAND_FAILED;
				}
				MPU_OPL_AddLog(EDVR_EVENT_OPT, EDVR_EVENT_OPT_DETAIL_PTZ_CTL, 0, 0, dwIP, szName);
				break;
			}
			case ePTZControlCodeAutoLeftRight:	//     29  －  云台左右自动扫描        (Param1: 1-开始/0-停止, Param2: 速度 [0-10,0表示默认速度,1-10表示速度级别], Param3: 无效, Param4: 无效) 
			{

				result = pPtz->PTZ_AutoCtrl(TTY_PTZ_AUTO, 0, 0, 0, dwParam2, 0, dwParam1, dwChannel);
				if (result)
				{
					MPU_ERR("[%d]PTZ_AutoCtrl<cmd =%d,p %d,%d,%d,%d> failed (%d)\n",
							(int) dwChannel, (int) eCommandCode, (int) dwParam1, (int) dwParam2, (int) dwParam3, (int) dwParam4, result);
					return ERR_COMMAND_FAILED;
				}
				MPU_OPL_AddLog(EDVR_EVENT_OPT, EDVR_EVENT_OPT_DETAIL_PTZ_CTL, 0, 0, dwIP, szName);
				break;
			}

			case ePTZControlCodePresetSet:	//     40  －  设置预置点          (Param1: 预置点序号[>=0], Param2: 无效, Param3: 无效, Param4: 无效) 
			{
				result = pPtz->PTZ_PresetCtrl(TTY_PTZ_PRESETSET, dwParam1, 0, 0, 0, dwChannel);
				if (result)
				{
					MPU_ERR("[%d]PTZ_PresetCtrl<cmd =%d,p %d,%d,%d,%d> failed (%d)\n",
						 (int) dwChannel, (int) eCommandCode, (int) dwParam1, (int) dwParam2, (int) dwParam3, (int) dwParam4, result);
					return ERR_COMMAND_FAILED;
				}
				MPU_OPL_AddLog(EDVR_EVENT_OPT, EDVR_EVENT_OPT_DETAIL_PTZ_CTL, 0, 0, dwIP, szName);
				break;
			}
			case ePTZControlCodePresetClear:	//     41  －  清除预置点          (Param1: 预置点序号[>=0], Param2: 无效, Param3: 无效, Param4: 无效) 
			{
				result = pPtz->PTZ_PresetCtrl(TTY_PTZ_PRESETMOVE, dwParam1, 0, 0, 0, dwChannel);
				if (result)
				{
					MPU_ERR("[%d]PTZ_PresetCtrl<cmd =%d,p %d,%d,%d,%d> failed (%d)\n",
						 (int) dwChannel, (int) eCommandCode, (int) dwParam1, (int) dwParam2, (int) dwParam3, (int) dwParam4, result);
					return ERR_COMMAND_FAILED;
				}
				MPU_OPL_AddLog(EDVR_EVENT_OPT, EDVR_EVENT_OPT_DETAIL_PTZ_CTL, 0, 0, dwIP, szName);
				break;
			}
			case ePTZControlCodePresetCall:	//     42  －  转到预置点          (Param1: 预置点序号[>=0], Param2: 无效, Param3: 无效, Param4: 无效) 
			{
				result = pPtz->PTZ_PresetCtrl(TTY_PTZ_PRESETCALL, dwParam1, 0, 0, 0, dwChannel);
				if (result)
				{
					MPU_ERR("[%d]PTZ_PresetCtrl<cmd =%d,p %d,%d,%d,%d> failed (%d)\n",
						 (int) dwChannel, (int) eCommandCode, (int) dwParam1, (int) dwParam2, (int) dwParam3, (int) dwParam4, result);
					return ERR_COMMAND_FAILED;
				}
				MPU_OPL_AddLog(EDVR_EVENT_OPT, EDVR_EVENT_OPT_DETAIL_PTZ_CTL, 0, 0, dwIP, szName);
				break;
			}

			case ePTZControlCodeCuriserSetStart:	//     51  －  启动巡航记忆            (Param1: 巡航路线号[>=0], Param2: 无效, Param3: 无效, Param4: 无效) 
			{
				result = pPtz->PTZ_PresetCtrl(TTY_PTZ_ROUTESET, 0, dwParam1, 0, 0, dwChannel);
				if (result)
				{
					MPU_ERR("[%d]PTZ_PresetCtrl<cmd =%d,p %d,%d,%d,%d> failed (%d)\n",
						 (int) dwChannel, (int) eCommandCode, (int) dwParam1, (int) dwParam2, (int) dwParam3, (int) dwParam4, result);
					return ERR_COMMAND_FAILED;
				}
				MPU_OPL_AddLog(EDVR_EVENT_OPT, EDVR_EVENT_OPT_DETAIL_PTZ_CTL, 0, 0, dwIP, szName);

				break;
			}
			case ePTZControlCodeCuriserSetStop:	//     52  －  关闭巡航记忆            (Param1: 巡航路线号[>=0], Param2: 无效, Param3: 无效, Param4: 无效) 
			{
				result = pPtz->PTZ_PresetCtrl(TTY_PTZ_ROUTEEND, 0, dwParam1, 0, 0, dwChannel);
				if (result)
				{
					MPU_ERR("[%d]PTZ_PresetCtrl<cmd =%d,p %d,%d,%d,%d> failed (%d)\n",
						 (int) dwChannel, (int) eCommandCode, (int) dwParam1, (int) dwParam2, (int) dwParam3, (int) dwParam4, result);
					return ERR_COMMAND_FAILED;
				}
				MPU_OPL_AddLog(EDVR_EVENT_OPT, EDVR_EVENT_OPT_DETAIL_PTZ_CTL, 0, 0, dwIP, szName);
				break;
			}
			case ePTZControlCodeCuriserAddPreset:	//     53  －  将预置点加入巡航序列    (Param1: 巡航路线号[>=0], Param2: 预置点序号[>=0], Param3: 停顿时间[秒,>=0], Param4: 巡航速度[1-10])    
			{
				result = pPtz->PTZ_PresetCtrl(TTY_PTZ_ROUTEADDPOINT, dwParam2, dwParam1, dwParam4, dwParam3, dwChannel);
				if (result)
				{
					MPU_ERR("[%d]PTZ_PresetCtrl<cmd =%d,p %d,%d,%d,%d> failed (%d)\n",
						 (int) dwChannel, (int) eCommandCode, (int) dwParam1, (int) dwParam2, (int) dwParam3, (int) dwParam4, result);
					return ERR_COMMAND_FAILED;
				}
				MPU_OPL_AddLog(EDVR_EVENT_OPT, EDVR_EVENT_OPT_DETAIL_PTZ_CTL, 0, 0, dwIP, szName);
				break;
			}
			case ePTZControlCodeCuriserRunStart:	//     54  －  开始巡航                (Param1: 巡航路线号[>=0], Param2: 无效, Param3: 无效, Param4: 无效) 
			{
			#if defined (__IS_SUPPORT_PTZ_BUILTIN__) && defined (__IS_SUPPORT_PTZ_BUILTIN_HUMAN_TRACK__)
				CPtzAdjuster *pPtzAdj;
				pPtzAdj = CPtzAdjuster::GetInstance();
				result = pPtzAdj->EnablePTZCruise(1);
				if (result)
				{
					MPU_ERR("[%d]EnablePTZCruise<cmd =%d,p %d,%d,%d,%d> failed (%d)\n",
							(int) dwChannel, (int) eCommandCode, (int) dwParam1, (int) dwParam2, (int) dwParam3, (int) dwParam4, result);
					return ERR_COMMAND_FAILED;
				}			
			#else				
				result = pPtz->PTZ_AutoCtrl(TTY_PTZ_SETROUTE, 0, 0, dwParam1, 0, 0, 1, dwChannel);
				if (result)
				{
					MPU_ERR("[%d]PTZ_AutoCtrl<cmd =%d,p %d,%d,%d,%d> failed (%d)\n",
							(int) dwChannel, (int) eCommandCode, (int) dwParam1, (int) dwParam2, (int) dwParam3, (int) dwParam4, result);
					return ERR_COMMAND_FAILED;
				}
			#endif
				MPU_OPL_AddLog(EDVR_EVENT_OPT, EDVR_EVENT_OPT_DETAIL_PTZ_CTL, 0, 0, dwIP, szName);
				break;
			}
			case ePTZControlCodeCuriserRunStop:	//     55  －  停止巡航                (Param1: 巡航路线号[>=0], Param2: 无效, Param3: 无效, Param4: 无效) 
			{
			#if defined (__IS_SUPPORT_PTZ_BUILTIN__) && defined (__IS_SUPPORT_PTZ_BUILTIN_HUMAN_TRACK__)
				CPtzAdjuster *pPtzAdj;
				pPtzAdj = CPtzAdjuster::GetInstance();
				result = pPtzAdj->EnablePTZCruise(0);
				if (result)
				{
					MPU_ERR("[%d]EnablePTZCruise<cmd =%d,p %d,%d,%d,%d> failed (%d)\n",
							(int) dwChannel, (int) eCommandCode, (int) dwParam1, (int) dwParam2, (int) dwParam3, (int) dwParam4, result);
					return ERR_COMMAND_FAILED;
				}
			#else				
				result = pPtz->PTZ_AutoCtrl(TTY_PTZ_SETROUTE, 0, 0, dwParam1, 0, 0, 0, dwChannel);
				if (result)
				{
					MPU_ERR("[%d]PTZ_AutoCtrl<cmd =%d,p %d,%d,%d,%d> failed (%d)\n",
							(int) dwChannel, (int) eCommandCode, (int) dwParam1, (int) dwParam2, (int) dwParam3, (int) dwParam4, result);
					return ERR_COMMAND_FAILED;
				}
			#endif
				MPU_OPL_AddLog(EDVR_EVENT_OPT, EDVR_EVENT_OPT_DETAIL_PTZ_CTL, 0, 0, dwIP, szName);
				break;
			}

			case ePTZControlCodeTraceSetStart:	//     61  －  启动轨迹记忆            (Param1: 轨迹号[>=0], Param2: 无效, Param3: 无效, Param4: 无效) 
			{
				result = pPtz->PTZ_PresetCtrl(TTY_PTZ_TRACKSET, 0, dwParam1, 0, 0, dwChannel);
				if (result)
				{
					MPU_ERR
						("[%d]PTZ_PresetCtrl<cmd =%d,p %d,%d,%d,%d> failed (%d)\n",
						 (int) dwChannel, (int) eCommandCode, (int) dwParam1, (int) dwParam2, (int) dwParam3, (int) dwParam4, result);
					return ERR_COMMAND_FAILED;
				}
				MPU_OPL_AddLog(EDVR_EVENT_OPT, EDVR_EVENT_OPT_DETAIL_PTZ_CTL, 0, 0, dwIP, szName);
				break;
			}
			case ePTZControlCodeTraceSetStop:	//     62  －  关闭轨迹记忆            (Param1: 轨迹号[>=0], Param2: 无效, Param3: 无效, Param4: 无效) 
			{
				result = pPtz->PTZ_PresetCtrl(TTY_PTZ_TRACKEND, 0, dwParam1, 0, 0, dwChannel);
				if (result)
				{
					MPU_ERR
						("[%d]PTZ_PresetCtrl<cmd =%d,p %d,%d,%d,%d> failed (%d)\n",
						 (int) dwChannel, (int) eCommandCode, (int) dwParam1, (int) dwParam2, (int) dwParam3, (int) dwParam4, result);
					return ERR_COMMAND_FAILED;
				}
				MPU_OPL_AddLog(EDVR_EVENT_OPT, EDVR_EVENT_OPT_DETAIL_PTZ_CTL, 0, 0, dwIP, szName);
				break;
			}
			case ePTZControlCodeTraceRunStart:	//     63  －  开始轨迹                (Param1: 轨迹号[>=0], Param2: 无效, Param3: 无效, Param4: 无效) 
			{
				result = pPtz->PTZ_AutoCtrl(TTY_PTZ_TRACKCALL, 0, 0, dwParam1, 0, 0, 1, dwChannel);
				if (result)
				{
					MPU_ERR("[%d]PTZ_AutoCtrl<cmd =%d,p %d,%d,%d,%d> failed (%d)\n",
							(int) dwChannel, (int) eCommandCode, (int) dwParam1, (int) dwParam2, (int) dwParam3, (int) dwParam4, result);
					return ERR_COMMAND_FAILED;
				}
				MPU_OPL_AddLog(EDVR_EVENT_OPT, EDVR_EVENT_OPT_DETAIL_PTZ_CTL, 0, 0, dwIP, szName);
				break;
			}
			case ePTZControlCodeTraceRunStop:	//     64  －  停止轨迹                (Param1: 轨迹号[>=0], Param2: 无效, Param3: 无效, Param4: 无效) 
			{
				result = pPtz->PTZ_AutoCtrl(TTY_PTZ_TRACKCALL, 0, 0, dwParam1, 0, 0, 0, dwChannel);
				if (result)
				{
					MPU_ERR("[%d]PTZ_AutoCtrl<cmd =%d,p %d,%d,%d,%d> failed (%d)\n",
							(int) dwChannel, (int) eCommandCode, (int) dwParam1, (int) dwParam2, (int) dwParam3, (int) dwParam4, result);
					return ERR_COMMAND_FAILED;
				}
				MPU_OPL_AddLog(EDVR_EVENT_OPT, EDVR_EVENT_OPT_DETAIL_PTZ_CTL, 0, 0, dwIP, szName);
				break;
			}

		#if defined (__IS_SUPPORT_PTZ_BUILTIN__) && defined (__IS_SUPPORT_PTZ_BUILTIN_HUMAN_TRACK__)
			case ePTZControlCodeTrackRunStart:	//     68  －  开始跟踪 
			{
				printf("ePTZControlCodeTrackRunStart\n");
				CPtzAdjuster *pPtzAdj;
				pPtzAdj = CPtzAdjuster::GetInstance();
				result = pPtzAdj->EnablePTZTrack(1);
				if (result)
				{
					MPU_ERR("[%d]EnablePTZTrack<cmd =%d,p %d,%d,%d,%d> failed (%d)\n",
							(int) dwChannel, (int) eCommandCode, (int) dwParam1, (int) dwParam2, (int) dwParam3, (int) dwParam4, result);
					return ERR_COMMAND_FAILED;
				}
				MPU_OPL_AddLog(EDVR_EVENT_OPT, EDVR_EVENT_OPT_DETAIL_PTZ_CTL, 0, 0, dwIP, szName);
				break;
			}

			case ePTZControlCodeTrackRunStop:	//     69  －  停止跟踪
			{
				printf("ePTZControlCodeTrackRunStop\n");
				CPtzAdjuster *pPtzAdj;
				pPtzAdj = CPtzAdjuster::GetInstance();
				result = pPtzAdj->EnablePTZTrack(0);
				if (result)
				{
					MPU_ERR("[%d]EnablePTZTrack<cmd =%d,p %d,%d,%d,%d> failed (%d)\n",
							(int) dwChannel, (int) eCommandCode, (int) dwParam1, (int) dwParam2, (int) dwParam3, (int) dwParam4, result);
					return ERR_COMMAND_FAILED;
				}
				MPU_OPL_AddLog(EDVR_EVENT_OPT, EDVR_EVENT_OPT_DETAIL_PTZ_CTL, 0, 0, dwIP, szName);
				break;
			}
		#endif
		
			case ePTZControlCodeSystemReset:	//     99  －  系统复位                (Param1: 无效, Param2: 无效, Param3: 无效, Param4: 无效)    
			{
				break;
			}
			case ePTZControlCodeElctronicReset:	//     100  －  电子PTZ复位                (Param1: 无效, Param2: 无效, Param3: 无效, Param4: 无效)    
			{
				MPU_PTZ_ResetAllEletronicProp(dwChannel);
				break;
			}
			#if defined (__IS_SUPPORT_PTZ_BUILTIN__) && defined (__IS_SUPPORT_PTZ_BUILTIN_HUMAN_TRACK__)
			case ePTZControlCodeCalibration:
			{
				result = pPtz->PTZ_MotoPosCalibration(0);
				if (result)
				{
					MPU_ERR("[%d]PTZ_PresetCtrl<cmd =%d,p %d,%d,%d,%d> failed (%d)\n",
						 (int) dwChannel, (int) eCommandCode, (int) dwParam1, (int) dwParam2, (int) dwParam3, (int) dwParam4, result);
					return ERR_COMMAND_FAILED;
				}
				break;
			}
			#endif
            case ePTZControlCodeWatchCareGoto:  //     160,         /* － 转到看守位(Param1: 看守位序号[>=0], Param2: 无效, Param3: 无效, Param4: 无效);  
            {
                result = pPtz->PTZ_PresetCtrl(TTY_PTZ_WATCHCAREGOTO, dwParam1, 0, 0, 0, dwChannel);
                if (result)
                {
                    MPU_ERR("[%d]PTZ_PresetCtrl<cmd =%d,p %d,%d,%d,%d> failed (%d)\n",
                         (int) dwChannel, (int) eCommandCode, (int) dwParam1, (int) dwParam2, (int) dwParam3, (int) dwParam4, result);
                    return ERR_COMMAND_FAILED;
                }
                break;
            }
			default:
			{
				MPU_ERR("invalid PTZ command <%d>\n", (int) eCommandCode);
				return ERR_COMMAND_FAILED;
				break;
			}

		}
	}
	return ERR_COMMAND_OK;
}

//远程锁定文件
static int HY_DVR_RemoteFileLock(DWORD dwUserID, char *szFileName, eFileLockOperationCode eOperationCode)
{
	int result = 0;

	MPU_DBG("< ID %d, CMD %d, %s>\n", (int) dwUserID, (int) eOperationCode, szFileName);

	if (szFileName == NULL)
	{
		MPU_ERR("szFileName == NULL \n");
		return ERR_COMMAND_FAILED;
	}

	if (eOperationCode == eLockFile)
	{
		result = MPU_STR_SnapShotM_Lock(szFileName);
	}
	else if (eOperationCode == eUnlockFile)
	{
		result = MPU_STR_SnapShotM_Unlock(szFileName);
	}

	if (result)
	{
		MPU_ERR("MPU_STR_SnapShotM_Lock/Unlock failed(%d)\n", result);
		return ERR_COMMAND_FAILED;
	}

	return ERR_COMMAND_OK;
}

//远程录像控制
static int HY_DVR_RemoteRecordControl(DWORD dwUserID, DWORD dwChannel, DWORD dwRecordType)
{
	int result;

	MPU_DBG("uid = %d ch = %d type = %d\n", (int) dwUserID, (int) dwChannel, (int) dwRecordType);

	if (!CIU_CheckRemoteUser(dwUserID, CHANNEL_MANUALRECORD, dwChannel))
	{
		MPU_ERR("uid = %d ch = %d no right\n", (int) dwUserID, (int) dwChannel);
		return IDVR_CIU_NO_RIGHT;
	}

	result = MPU_AVL_Record_StartMode(dwChannel, (enumRecordMode) dwRecordType);
	if (result)
	{
		return ERR_COMMAND_FAILED;
	}

	//记录日志，远程录像控制
	CIU_OPL_AddLog(dwUserID, EDVR_EVENT_RECORD, (EDVR_EVENT_RECORD_DETAIL) dwRecordType, 0, 0);

	return ERR_COMMAND_OK;
}

//远程报警输出控制
static int HY_DVR_RemoteAlarmoutControl(DWORD dwUserID, DWORD dwChannel, DWORD dwSwitch)
{
	int nSDKRet;

	MPU_DBG("<ID %d, CMD %d, SWITCH %d>\n", (int) dwUserID, (int) dwChannel, (int) dwSwitch);

	if (!MPU_CIU_CheckRemoteUser(dwUserID, REMOTE_ALARMOUT))
	{
		MPU_DBG("<ID %d, CMD %d, SWITCH %d> no right\n", (int) dwUserID, (int) dwChannel, (int) dwSwitch);
		return IDVR_CIU_NO_RIGHT;
	}

	nSDKRet = MPU_DOP_SetAlarmOutState(dwChannel, dwSwitch);
	if (0 != nSDKRet)
	{
		return ERR_COMMAND_FAILED;
	}

    HieServer_RefreshEvent(USEREVENT_ALARM_NOTICE, ALERT_ALARMOUT, 0, dwSwitch);

	return ERR_COMMAND_OK;
}

//远程获取报警输出状态
static int HY_DVR_RemoteAlarmoutStateGet(DWORD dwUserID, DWORD * dwChanNum, DWORD * dwStateBits)
{
	int nChNum;

	int nCh;

	DWORD dwState;

	if ((NULL == dwChanNum) || (NULL == dwStateBits))
	{
		MPU_ERR("<ID %d, pointer is null\n", (int) dwUserID);
		return ERR_COMMAND_FAILED;
	}

	if (!CIU_CheckRemoteUser(dwUserID, REMOTE_ALARMOUT))
	{
		MPU_ERR("<ID %d> no right\n", (int) dwUserID);
		return IDVR_CIU_NO_RIGHT;
	}

	nChNum = MPU_DOP_GetPortNum();
	if (nChNum < 0)
	{
		nChNum = 0;
	}
	else if (nChNum > 32)
	{
		nChNum = 32;
	}

	dwState = 0;
	for (nCh = 0; nCh < nChNum; ++nCh)
	{
		if (MPU_DOP_GetAlarmOutState(nCh))
		{
			SET_BIT(dwState, nCh);
		}
	}

	*dwChanNum = nChNum;
	*dwStateBits = dwState;

	MPU_DBG("<ID %d, ChanNum %d, State 0x%08X>\n", (int) dwUserID, nChNum, (int) dwState);

	return ERR_COMMAND_OK;
}


static int HY_DVR_DiskGroupManage(DWORD dwUserID, eDiskGroupOperation eOperationCode, DWORD dwParam1, DWORD dwParam2, DWORD dwParam3)
{
	MPU_DBG("< %d,%d,%d,%d,%d>\n", (int) dwUserID, (int) eOperationCode, (int) dwParam1, (int) dwParam2, (int) dwParam3);

	if (!MPU_CIU_CheckRemoteUser(dwUserID, REMOTE_SETTINGS))
	{
		MPU_ERR("uid = %d  no right\n", (int) dwUserID);
		return IDVR_CIU_NO_RIGHT;
	}
	if (!m_DiskMan_DataSize.dwDoDiskManageReady)
	{
		MPU_ERR("not ready,busy,wait for a while \n");
		return ERR_COMMAND_BUSY;
	}
	m_DiskMan_DataSize.dwDoDiskManageReady = 0;
	if (eOperationCode == eUnMountDisk ||
		eOperationCode == eCreateNewPartition || eOperationCode == eDeletePartition || eOperationCode == eFormatPartition)
	{
#if 0
		Local_VerifyMark = MPU_STR_Disk_GetVerifyMark((int) dwParam1);
		//验证码
		if (Local_VerifyMark != dwParam3)
		{
			m_DiskMan_DataSize.dwDoDiskManageReady = 1;
			MPU_ERR("The verify mark invalid,need refresh disk info 0x%08x --> 0x%08x\n", (int) dwParam3, (int) Local_VerifyMark);
			return ERR_COMMAND_FAILED;
		}
#endif

	}

	m_DiskMan_DataSize.dwDiskManageUserID = dwUserID;
	m_DiskMan_DataSize.eDiskManageOperationType = eOperationCode;
	m_DiskMan_DataSize.dwDiskManageParam[0] = dwParam1;
	m_DiskMan_DataSize.dwDiskManageParam[1] = dwParam2;
	m_DiskMan_DataSize.dwDiskManageParam[2] = dwParam3;
	m_DiskMan_DataSize.dwOperationType = 1;	//硬盘管理
	sem_post(&m_sem_create);
	return ERR_COMMAND_OK;
}

static int Do_DiskGroupManage(DWORD dwUserID, eDiskGroupOperation eOperationCode, DWORD dwParam1, DWORD dwParam2, DWORD dwParam3)
{
	int result = 0;

	int nCh, nCount;

	DWORD Local_VerifyMark = 0;

	DWORD dwIP = 0;

	char szIP[20];

	char szName[20] = "";

	printf("< %d,%d,%d,%d,%d>\n", (int) dwUserID, (int) eOperationCode, (int) dwParam1, (int) dwParam2, (int) dwParam3);
	result = MPU_CIU_GetRemoteByID(dwUserID, NULL, szName, sizeof(szName), szIP, sizeof(szIP), NULL, 0);
	if (!result)
	{
		dwIP = inet_addr(szIP);
#ifdef IDVR_LIL_ENDIAN
		dwIP = iDvrArchSwap32(dwIP);
#endif
	}
	result = 0;
	switch (eOperationCode)
	{
		case eUnMountDisk:		//                        /*!< 卸载分区                               */
			{					/*!
								   dwParam1 <=> 硬盘索引;
								   dwParam2 <=> 0;
								   dwParam3 <=> VerifyMark(验证码),用于标识当前操作是否基于正确的配置。
								 */
				int busNo=0;

				Local_VerifyMark = MPU_STR_Disk_GetVerifyMark((int) dwParam1);

				//验证码
				if (Local_VerifyMark != dwParam3)
				{
					MPU_ERR("The verify mark invalid,need refresh disk info 0x%08x --> 0x%08x\n", (int) dwParam3, (int) Local_VerifyMark);
					return -1;
				}
				result = MPU_STR_Disk_GetBusNumber((int) dwParam1, &busNo);
				if (result)
				{
					MPU_ERR("MPU_STR_Disk_GetBusNumber < %d> failed<%d>\n", (int) dwParam1, result);
					return -1;
				}
				result = MPU_STR_Disk_UnMount((int) busNo);
				if (result)
				{
					MPU_ERR("MPU_STR_Disk_UnMount < %d> failed<%d>\n", (int) dwParam1, result);
					return -1;
				}
				MPU_OPL_AddLog(EDVR_EVENT_STORAGE, EDVR_EVENT_STORAGE_DETAIL_UMOUNT_DISK, 0, 0, dwIP, szName);
				break;
			}
		case eCreateNewPartition:	//             /*!< 建立新分区                             */
			{					/*!
								   dwParam1 <=> 硬盘索引;
								   dwParam2 <=> 分区大小（MB）；
								   dwParam3 <=> VerifyMark(验证码),用于标识当前操作是否基于正确的配置。

								 */
				Local_VerifyMark = MPU_STR_Disk_GetVerifyMark((int) dwParam1);

				//验证码
				if (Local_VerifyMark != dwParam3)
				{
					MPU_ERR("The verify mark invalid,need refresh disk info 0x%08x --> 0x%08x\n", (int) dwParam3, (int) Local_VerifyMark);
					return -1;
				}

				result = MPU_STR_Disk_CreatePartition((int) dwParam1, dwParam2);
				if (result)
				{
					MPU_ERR("CreatePartition < %d,%d> failed<%d>\n", (int) dwParam1, (int) dwParam2, result);
					return -1;
				}
				MPU_OPL_AddLog(EDVR_EVENT_STORAGE, EDVR_EVENT_STORAGE_DETAIL_ADD_PARTITION, 0, 0, dwIP, szName);
				break;
			}
		case eDeletePartition:	//                    /*!< 删除分区                               */
			{					/*!
								   dwParam1 <=> 硬盘索引;
								   dwParam2 <=> 分区索引；
								   dwParam3 <=> VerifyMark(验证码),用于标识当前操作是否基于正确的配置。

								 */
				Local_VerifyMark = MPU_STR_Disk_GetVerifyMark((int) dwParam1);

				//验证码
				if (Local_VerifyMark != dwParam3)
				{
					MPU_ERR("The verify mark invalid,need refresh disk info 0x%08x --> 0x%08x\n", (int) dwParam3, (int) Local_VerifyMark);
					return -1;
				}

				result = MPU_STR_Disk_DestroyPartition((int) dwParam1, (int) dwParam2);
				if (result)
				{
					MPU_ERR("DestroyPartition < %d,%d> failed<%d>\n", (int) dwParam1, (int) dwParam2, result);
					return -1;
				}
				MPU_OPL_AddLog(EDVR_EVENT_STORAGE, EDVR_EVENT_STORAGE_DETAIL_REMOVE_PARTITION, 0, 0, dwIP, szName);
				break;
			}
		case eFormatPartition:	//                    /*!< 格式化分区                             */
			{					/*!
								   dwParam1 <=> 硬盘索引;
								   dwParam2 <=> 分区索引；
								   dwParam3 <=> VerifyMark(验证码),用于标识当前操作是否基于正确的配置。

								 */
#if 0
				Local_VerifyMark = MPU_STR_Disk_GetVerifyMark((int) dwParam1);

				//验证码
				if (Local_VerifyMark != dwParam3)
				{
					MPU_ERR("The verify mark invalid,need refresh disk info 0x%08x --> 0x%08x\n", (int) dwParam3, (int) Local_VerifyMark);
					return -1;
				}
#endif
				nCount = MAX_ENC_CHANNEL_NUM;

				//操作前暂停录像
				for (nCh = 0; nCh < nCount; nCh++)
				{
					MPU_AVL_Record_Pause(nCh, 1);
				}
printf("----------1----------\n");

				result = MPU_STR_Disk_FormatPartition((int) dwParam1, (int) dwParam2);

                printf("----------MPU_STR_Disk_FormatPartition => result = %d----------\n", result);

				//操作后恢复录像
				for (nCh = 0; nCh < nCount; nCh++)
				{
					MPU_AVL_Record_Pause(nCh, 0);
				}
				if (result)
				{
					MPU_ERR("FormatPartition < %d,%d> failed<%d>\n", (int) dwParam1, (int) dwParam2, result);
					return -1;
				}
				MPU_OPL_AddLog(EDVR_EVENT_STORAGE, EDVR_EVENT_STORAGE_DETAIL_FORMAT_PARTITION, 0, 0, dwIP, szName);
				break;
			}
		case eDiskGroupKeepTime:	//                  /*!< 分组保持数据时间                       */
			{					/*!
								   dwParam1 <=> 盘组类型; eDiskGroupType
								   dwParam2 <=> 数据保持时间(S)；
								   dwParam3 <=> 0
								 */


				result = MPU_STR_DiskGroup_DataKeepTime((int) dwParam1, (int) dwParam2);
				if (result)
				{
					MPU_ERR("Set_DataKeepTime < %d,%d> failed<%d>\n", (int) dwParam1, (int) dwParam2, result);
					return -1;
				}
				MPU_OPL_AddLog(EDVR_EVENT_STORAGE, EDVR_EVENT_STORAGE_DETAIL_MANAGE_DISKGROUP, 0, 0, dwIP, szName);
				break;
			}
		case eDiskGroupBindChannel:	//               /*!< 分组绑定通道                           */
			{					/*!
								   dwParam1 <=>盘组类型; eDiskGroupType
								   dwParam2 <=> 录像类型；eRecordType
								   dwParam3 <=> 通道:
								   bit[0~31] <=> 通道掩码,即每个bit位标识每一通道,共支持32通道.
								   其位0表示删除,1表示添加.

								 */

				nCount = MAX_ENC_CHANNEL_NUM;

				for (nCh = 0; nCh < nCount; nCh++)
				{
					//操作前暂停录像

					MPU_AVL_Record_Pause(nCh, 1);

					switch (dwParam1)
					{
						case TypeDiskGroupAlarm_RC:
							{
								if (dwParam3 & (1 << nCh))
								{
									result = MPU_STR_DiskGroup_BindDataCH(1, nCh, RECORD_TYPE_ALARM);
									if (result)
									{
										MPU_ERR("type %d< %d,%d,%d -> %d>  failed %d\n", (int) eOperationCode, (int) dwParam1, (int) dwParam2,
												(int) dwParam3, nCh, result);
										result = -1;
									}
								}
								else
								{
									result = MPU_STR_DiskGroup_UnBindDataCH(1, nCh, RECORD_TYPE_ALARM);
									if (result)
									{
										MPU_ERR("type %d< %d,%d,%d -> %d>  failed %d\n", (int) eOperationCode, (int) dwParam1, (int) dwParam2,
												(int) dwParam3, nCh, result);
										result = -1;
									}
								}
								break;
							}
						case TypeDiskGroupNormal_RC:
							{
#if 0
								if (dwParam3 & (1 << nCh))
								{
									result = MPU_STR_DiskGroup_BindDataCH(0, nCh, RECORD_TYPE_GENERAL);
								}
								else
								{
									result = MPU_STR_DiskGroup_UnBindDataCH(0, nCh, RECORD_TYPE_GENERAL);
								}
#else
								result = -1;
								MPU_ERR("eDiskGroupNormal  Only read< %d,%d,%d -> %d> \n", (int) dwParam1, (int) dwParam2, (int) dwParam3, nCh);
#endif
								break;
							}
						case TypeDiskGroupRedundance_RC:
							{
								if (dwParam3 & (1 << nCh))
								{
									result = MPU_STR_DiskGroup_BindDataCH(2, nCh, RECORD_TYPE_GENERAL);
									result |= MPU_STR_DiskGroup_BindDataCH(2, nCh, RECORD_TYPE_ALARM);
									result |= MPU_STR_DiskGroup_BindDataCH(2, nCh, RECORD_TYPE_MANUAL);
									result |= MPU_STR_DiskGroup_BindDataCH(2, nCh, RECORD_TYPE_MOTION);
									result |= MPU_STR_DiskGroup_BindDataCH(2, nCh, RECORD_TYPE_AI);
								}
								else
								{
									result = MPU_STR_DiskGroup_UnBindDataCH(2, nCh, RECORD_TYPE_GENERAL);
									result |= MPU_STR_DiskGroup_UnBindDataCH(2, nCh, RECORD_TYPE_ALARM);
									result |= MPU_STR_DiskGroup_UnBindDataCH(2, nCh, RECORD_TYPE_MANUAL);
									result |= MPU_STR_DiskGroup_UnBindDataCH(2, nCh, RECORD_TYPE_MOTION);
									result |= MPU_STR_DiskGroup_UnBindDataCH(2, nCh, RECORD_TYPE_AI);
								}
								if (result)
								{
									MPU_ERR("type %d< %d,%d,%d -> %d>  failed %d\n", (int) eOperationCode, (int) dwParam1, (int) dwParam2,
											(int) dwParam3, nCh, result);
									result = -1;
								}
								break;
							}
						case TypeDiskGroupBackup_RC:
							{
#if 0
								if (dwParam3 & (1 << nCh))
								{
									result = MPU_STR_DiskGroup_BindDataCH(3, nCh, RECORD_TYPE_ALARM);
								}
								else
								{
									result = MPU_STR_DiskGroup_UnBindDataCH(3, nCh, RECORD_TYPE_ALARM);
								}
#else
								result = -1;
								MPU_ERR("DiskGroupBackup bind/unbind unsupported< %d,%d,%d -> %d> \n", (int) dwParam1, (int) dwParam2, (int) dwParam3,
										nCh);
#endif
								break;
							}
						default:
							{
								MPU_ERR("eOperationCode =%d < %d,%d,%d -> %d> \n", (int) eOperationCode, (int) dwParam1, (int) dwParam2,
										(int) dwParam3, nCh);
								result = -1;
								break;
							}


					}



					//操作后恢复录像

					MPU_AVL_Record_Pause(nCh, 0);

					if (result)
					{
						return -1;
					}
				}

				MPU_OPL_AddLog(EDVR_EVENT_STORAGE, EDVR_EVENT_STORAGE_DETAIL_MANAGE_DISKGROUP, 0, 0, dwIP, szName);

				break;
			}
		case eDiskGroupAddPartition:	//              /*!< 添加分区至分组                         */
			{					/*!
								   dwParam1 <=> 盘组类型; eDiskGroupType
								   dwParam2 <=> 分区挂载点；
								   dwParam2:四个字节,如sda6,高三字节存sda字符码,低一字节是序号数字 < == > DWORD

								 */

				char partStr[100];

				char *p8 = (char *) &dwParam2;

#if IDVR_BYTEORDER == IDVR_LIL_ENDIAN
				snprintf(partStr, sizeof(partStr), "/dev/%c%c%c%d", p8[0], p8[1], p8[2], p8[3]);
#else
				snprintf(partStr, sizeof(partStr), "/dev/%c%c%c%d", p8[3], p8[2], p8[1], p8[0]);
#endif
				MPU_DBG("Add part <%d,%s>\n", (int) dwParam1, partStr);
				result = MPU_STR_DiskGroup_AddPartition((int) dwParam1, partStr);
				if (result)
				{
					MPU_ERR("AddPartition < %d,0x%08x - %s> failed<%d>\n", (int) dwParam1, (int) dwParam2, partStr, result);
					return -1;
				}
				MPU_OPL_AddLog(EDVR_EVENT_STORAGE, EDVR_EVENT_STORAGE_DETAIL_MANAGE_DISKGROUP, 0, 0, dwIP, szName);
				break;
			}
		case eDiskGroupDelPartition:	//              /*!< 从分组删除分区 */
			{					/*!
								   dwParam1 <=> 盘组类型; eDiskGroupType
								   dwParam2 <=> 分区挂载点；
								   dwParam2:四个字节,如sda6,高三字节存sda字符码,低一字节是序号数字 < == > DWORD

								 */

				char partStr[100];

				char *p8 = (char *) &dwParam2;


#if IDVR_BYTEORDER == IDVR_LIL_ENDIAN
				snprintf(partStr, sizeof(partStr), "/dev/%c%c%c%d", p8[0], p8[1], p8[2], p8[3]);
#else
				snprintf(partStr, sizeof(partStr), "/dev/%c%c%c%d", p8[3], p8[2], p8[1], p8[0]);
#endif
				MPU_DBG("del part <%d,%s>\n", (int) dwParam1, partStr);

				nCount = MAX_ENC_CHANNEL_NUM;

				//操作前暂停录像
				for (nCh = 0; nCh < nCount; nCh++)
				{
					MPU_AVL_Record_Pause(nCh, 1);
				}

				result = MPU_STR_DiskGroup_DelPartition((int) dwParam1, partStr);

				//操作后恢复录像
				for (nCh = 0; nCh < nCount; nCh++)
				{
					MPU_AVL_Record_Pause(nCh, 0);
				}
				if (result)
				{
					MPU_ERR("DelPartition < %d,0x%08x - %s> failed<%d>\n", (int) dwParam1, (int) dwParam2, partStr, result);
					return -1;
				}
				MPU_OPL_AddLog(EDVR_EVENT_STORAGE, EDVR_EVENT_STORAGE_DETAIL_MANAGE_DISKGROUP, 0, 0, dwIP, szName);
				break;
			}
		case eInitializeDisk:	//  /*!< 初始化硬盘                             */
			{					/*!
								   dwParam1 <=> 硬盘索引;
								   dwParam2 <=> 没有使用
								   dwParam3 <=> VerifyMark(验证码),用于标识当前操作是否基于正确的配置。
								 */

				Local_VerifyMark = MPU_STR_Disk_GetVerifyMark((int) dwParam1);

				//验证码
				if (Local_VerifyMark != dwParam3)
				{
					MPU_ERR("The verify mark invalid,need refresh disk info 0x%08x --> 0x%08x\n", (int) dwParam3, (int) Local_VerifyMark);
					return -1;
				}

				nCount = MAX_ENC_CHANNEL_NUM;

				//操作前暂停录像
				for (nCh = 0; nCh < nCount; nCh++)
				{
					MPU_AVL_Record_Pause(nCh, 1);
				}


				result = MPU_STR_Disk_InitDisk((int) dwParam1);

				//操作后恢复录像
				for (nCh = 0; nCh < nCount; nCh++)
				{
					MPU_AVL_Record_Pause(nCh, 0);
				}

				if (result)
				{
					MPU_ERR("InitializeDisk < %d > failed<%d>\n", (int) dwParam1, result);
					return -1;
				}
				MPU_OPL_AddLog(EDVR_EVENT_STORAGE, EDVR_EVENT_STORAGE_DETAIL_INITIALIZE_DISK, 0, 0, dwIP, szName);
				break;
			}
		default:
			{
				MPU_ERR("Unsupported operation <%d>\n", (int) eOperationCode);
				result = -1;
				break;
			}
	}

	return result;
}

//远程硬盘管理异步工作线程
static void *Fnx_CommandCreateThread(void *lParam)
{    
	prctl(PR_SET_NAME,"Fnx_CommandCreateThread");

    PRT_TID();
	int result;

	DWORD dwParam1, dwParam2, dwParam3;
	pthread_detach(pthread_self());
	BEGINRUN_THREAD;
	while (1)
	{
		MPU_DBG("wait....\n");
		sem_wait(&m_sem_create);
		MPU_DBG("wake up....\n");
printf("m_DiskMan_DataSize.dwOperationType = %ld, m_DiskMan_DataSize.eDiskManageOperationType = %d\n",
    m_DiskMan_DataSize.dwOperationType, (int)m_DiskMan_DataSize.eDiskManageOperationType);
		if (m_DiskMan_DataSize.dwOperationType == 1)
		{

			result = Do_DiskGroupManage(m_DiskMan_DataSize.dwDataSizeUserID, (eDiskGroupOperation) m_DiskMan_DataSize.eDiskManageOperationType,
										m_DiskMan_DataSize.dwDiskManageParam[0], m_DiskMan_DataSize.dwDiskManageParam[1],
										m_DiskMan_DataSize.dwDiskManageParam[2]);
			dwParam1 = m_DiskMan_DataSize.eDiskManageOperationType;	//何操作
			dwParam2 = result;	//操作结果
			dwParam3 = m_DiskMan_DataSize.dwDiskManageParam[2];	//验证码
			//通知完成
			result = HieServer_RefreshEvent(USEREVENT_DISKGROUP_MANAGE, dwParam1, dwParam2, dwParam3);
			if (result)
			{
				MPU_ERR("HieServer_RefreshEvent <%d,%d,%d,%d> Failed %d\n", USEREVENT_DISKGROUP_MANAGE, (int) dwParam1, (int) dwParam2,
						(int) dwParam3, result);
			}
			result = HieServer_RefreshEventEx(USEREVENT_DISKGROUP_MANAGE, 0, 
				m_DiskMan_DataSize.eDiskManageOperationType, dwParam2, dwParam3, NULL, 0);
			if (result)
			{
				MPU_ERR("HieServer_RefreshEventEx <%d,%d,%d,%d> Failed %d\n", USEREVENT_DISKGROUP_MANAGE, (int) dwParam1, (int) dwParam2,
						(int) dwParam3, result);
			}
			m_DiskMan_DataSize.dwDoDiskManageReady = 1;
		}
		else if (m_DiskMan_DataSize.dwOperationType == 2)
		{						//获取历史流大小
			unsigned long long dataSize = 0;

			DWORD recType = 0;

			if (m_DiskMan_DataSize.dwDataSizeStreamType & GENERAL_RECORD)
			{
				recType |= 1 << 0;
			}
			if (m_DiskMan_DataSize.dwDataSizeStreamType & MANUAL_RECORD)
			{
				;//recType |= 1 << RECORD_TYPE_MANUAL;
			}
			if (m_DiskMan_DataSize.dwDataSizeStreamType & MOTION_RECORD)
			{
				recType |= 1 << 1;
			}
			if (m_DiskMan_DataSize.dwDataSizeStreamType & ALARM_RECORD)
			{
				recType |= 1 << 2;
			}
			if (m_DiskMan_DataSize.dwDataSizeStreamType & AI_RECORD)
			{
				recType |= 1 << 3;
			}

#ifdef USE_SD_PKG
			result = ZW_PkgSd_GetDataSize(m_DiskMan_DataSize.tDataSizeStartTime, m_DiskMan_DataSize.tDataSizeStopTime, recType, &dataSize);
#else
  			result = CIU_GetDataSize(m_DiskMan_DataSize.dwDataSizeChannelBits, m_DiskMan_DataSize.dwDataSizeDiskGroup, recType,
								m_DiskMan_DataSize.tDataSizeStartTime, m_DiskMan_DataSize.tDataSizeStopTime, &dataSize);
#endif
			if (result)
			{
				MPU_ERR("GetDataSize failed (%d)\n", result);
				dataSize = 0xFFFFFFFF;
			}

			dwParam1 = 1;		//表示为获取数据大小
			dwParam2 = m_DiskMan_DataSize.dwDataSizeUserData;
			dwParam3 = dataSize;
			printf("HieServer_RefreshEvent <%d,%d,%d,%d> \n", USEREVENT_HISTORYSTREAM_NOTICE, (int) dwParam1, (int) dwParam2, (int) dwParam3);
			result = HieServer_RefreshEvent(USEREVENT_HISTORYSTREAM_NOTICE, dwParam1, dwParam2, dwParam3);
			if (result)
			{
				MPU_ERR("HieServer_RefreshEvent <%d,%d,%d,%d> Failed %d\n", USEREVENT_HISTORYSTREAM_NOTICE, (int) dwParam1, (int) dwParam2,
						(int) dwParam3, result);
			}

			result = HieServer_RefreshEventEx(USEREVENT_HISTORYSTREAM_NOTICE, 0, 
				dwParam1, dwParam2, dwParam3, NULL, 0);
			if (result)
			{
				MPU_ERR("HieServer_RefreshEventEx <%d,%d,%d,%d> Failed %d\n", USEREVENT_HISTORYSTREAM_NOTICE, (int) dwParam1, (int) dwParam2,
						(int) dwParam3, result);
			}

			m_DiskMan_DataSize.dwDataSizeReady = 1;
		}

	}

	ENDRUN_THREAD;
	return NULL;
}


static int HY_DVR_GetDataSizeEx(DWORD dwUserID, DWORD dwChannelBits, eDiskGroupType eDskGroupType, DWORD dwStreamType, char *szStartTime,
								char *szEndTime, DWORD dwUserData, DWORD * dwDataSize)
{
	time_t startTime, stopTime;

	int iYear, iMon, iDay, iHour, iMin, iSec;

	struct tm stm;

	int ch = 0;

	printf("HY_DVR_GetDataSizeEx < %d,0x%x,%d,0x%x,%s-%s>\n", (int) dwUserID, (int) dwChannelBits, (int) eDskGroupType, (int) dwStreamType, szStartTime, szEndTime);

	if (eDskGroupType <= eDiskGroupTypeBegin || eDskGroupType >= eDiskGroupTypeEnd)
	{
		MPU_ERR("DiskGroupType = %d\n", (int) eDskGroupType);
		return ERR_COMMAND_FAILED;
	}
	for (ch = 0; ch < 16 && (dwChannelBits & (1 << ch)); ch++)
	{
		if (!MPU_CIU_CheckRemoteUser(dwUserID, CHANNEL_PLAYBACK, ch))
		{
			MPU_ERR("HY_DVR_HistoryQueryCreate uid = %d ch = %d no right\n", (int) dwUserID, ch);
			return IDVR_CIU_NO_RIGHT;
		}
	}


	if (!m_DiskMan_DataSize.dwDataSizeReady)
	{
		MPU_DBG("busy..\n");
		return ERR_COMMAND_BUSY;
	}
	m_DiskMan_DataSize.dwDataSizeReady = 0;
	sscanf(szStartTime, "%04d%02d%02d%02d%02d%02d", &iYear, &iMon, &iDay, &iHour, &iMin, &iSec);
	stm.tm_year = iYear - 1900;
	stm.tm_mon = iMon - 1;
	stm.tm_mday = iDay;
	stm.tm_hour = iHour;
	stm.tm_min = iMin;
	stm.tm_sec = iSec;
	startTime = mktime(&stm);



	sscanf(szEndTime, "%04d%02d%02d%02d%02d%02d", &iYear, &iMon, &iDay, &iHour, &iMin, &iSec);
	stm.tm_year = iYear - 1900;
	stm.tm_mon = iMon - 1;
	stm.tm_mday = iDay;
	stm.tm_hour = iHour;
	stm.tm_min = iMin;
	stm.tm_sec = iSec;
	stopTime = mktime(&stm);


	m_DiskMan_DataSize.tDataSizeStartTime = startTime;
	m_DiskMan_DataSize.tDataSizeStopTime = stopTime;
	m_DiskMan_DataSize.dwDataSizeUserID = dwUserID;
	m_DiskMan_DataSize.dwDataSizeChannelBits = dwChannelBits;
	m_DiskMan_DataSize.dwDataSizeStreamType = dwStreamType;
	m_DiskMan_DataSize.dwDataSizeUserData = dwUserData;
	switch (eDskGroupType)
	{
		case eDiskGroupNormal:
			{
				m_DiskMan_DataSize.dwDataSizeDiskGroup = Size_TypeDiskGroupNormal;
				break;
			}
		case eDiskGroupAlarm:
			{
				m_DiskMan_DataSize.dwDataSizeDiskGroup = Size_TypeDiskGroupNormal;
				break;
			}
		case eDiskGroupRedundance:
			{
				m_DiskMan_DataSize.dwDataSizeDiskGroup = Size_TypeDiskGroupRedundance;
				break;
			}
		case eDiskGroupBackup:
			{
				m_DiskMan_DataSize.dwDataSizeDiskGroup = Size_TypeDiskGroupBackup;
				break;
			}
		default:
			{
				m_DiskMan_DataSize.dwDataSizeDiskGroup = Size_TypeDiskGroupNormal;
				break;
			}
	}


	m_DiskMan_DataSize.dwOperationType = 2;	//获取历史流数据大小
	sem_post(&m_sem_create);

	return ERR_COMMAND_OK;
}

/*
* @brief
*	系统升级通知
* @param dwUserID
*	用户ID
* @param cFTPParam
*	FTP服务器的升级文件参数
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
int HXHT_SystemUpdateNotify(DWORD dwUserID, SystemUpdateParam * pFTPParam)
{
	MPU_DBG("my add: ---------into HXHT_SystemUpdateNotify");

	if (0 == CIU_FtpUpdateStart(pFTPParam))
		return ERR_COMMAND_OK;

	return ERR_COMMAND_FAILED;
}

/*
* @brief
*	远程恢复色度默认参数命令
* @param dwUserID
*	用户ID
* @param dwChannel
*	通道号
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
static int HY_DVR_RemoteSetChromaDefault(DWORD dwUserID, DWORD dwChannel, DWORD dwTimeSection)
{
	MPU_DBG("HY_DVR_RemoteSetChromaDefault CH=%lu, dwSection=%lu\n", dwChannel, dwTimeSection);
	int i = 0;

#if defined(EGUI_FOR_ICAMERA_NXP8850_BASED_DVR)
	int nDefValue[] = { 128, 128, 64, 64 };
#else
	int nDefValue[] = { 128, 128, 46, 64 };
#endif

	// 四个时间段
	for (i = 0; i < 4; i++)
	{
		if (dwTimeSection & 1)
		{
			/* 0 Bright 128
			 * 1 Chroma 128
			 * 2 Contra 46
			 * 3 Satura 64
			 */
			int j = 0;

			for (j = 0; j < 4; j++)
			{
				MPU_AVL_Pic_SetPicParam(dwChannel, i, j, nDefValue[j]);
			}
		}
		dwTimeSection = dwTimeSection >> 1;
	}

	return ERR_COMMAND_OK;
}


//远程获取报警输入状态
static int HY_DVR_RemoteAlarminStateGet(DWORD dwUserID, DWORD * dwChanNum, DWORD * dwStateBits)
{
	int nChNum;
	int nCh;
	DWORD dwState;

	if ((NULL == dwChanNum) || (NULL == dwStateBits))
	{
		MPU_ERR("<ID %d, pointer is null\n", (int) dwUserID);
		return ERR_COMMAND_FAILED;
	}

	nChNum = MPU_DIP_GetPortNum();
	if (nChNum < 0)
	{
		nChNum = 0;
	}
	else if (nChNum > 32)
	{
		nChNum = 32;
	}

	dwState = 0;
	for (nCh = 0; nCh < nChNum; ++nCh)
	{
		if (MPU_DIP_GetState(nCh))
		{
			SET_BIT(dwState, nCh);
		}
	}

	*dwChanNum = nChNum;
	*dwStateBits = dwState;

	MPU_DBG("<ID %d, ChanNum %d, State 0x%08X>\n", (int) dwUserID, nChNum, (int) dwState);

	return ERR_COMMAND_OK;
}

int CIU_RemoteCommand_init()
{
	int result = 0;

	DiskGroupManageInterfaceMultiType dgmi;

	RemoteCommandInterface rci;

	sem_init(&m_sem_create, 0, 0);
	//创建硬盘管理工作线程
	if(m_hCreateThread == 0)
	{
		result = pthread_create(&m_hCreateThread, NULL, Fnx_CommandCreateThread, NULL);
		if (0 != result)
		{
			MPU_ERR("Create thread failed\n");
			return -1;
		}
	}
	memset(&dgmi, 0, sizeof (dgmi));
	memset(&rci, 0, sizeof (rci));

	//注册远程控制命令回调
	rci.ifDataExistCheck = HY_DVR_DataExistCheck;
	rci.ifPtzControl = HY_DVR_PtzControl;
	rci.ifRemoteAlarmClear = HY_DVR_RemoteAlarmClear;
	rci.ifRemoteForceIFrame = HY_DVR_RemoteForceIFrame;
	rci.ifRemoteImageCapture = HY_DVR_RemoteImageCapture;
	rci.ifRemoteDeviceControl = HY_DVR_RemoteReboot;
	rci.ifRemoteFileLock = HY_DVR_RemoteFileLock;
	rci.ifRemoteAlarmoutControl = HY_DVR_RemoteAlarmoutControl;
	rci.ifRemoteRecordControl = HY_DVR_RemoteRecordControl;
	rci.ifRemoteAlarmoutStateGet = HY_DVR_RemoteAlarmoutStateGet;
	rci.ifSystemUpdateNotify = HXHT_SystemUpdateNotify;	/*!< 升级请求接口 */
	rci.ifSetChromaDefault = HY_DVR_RemoteSetChromaDefault;
	rci.ifRemoteAlarminStateGet = HY_DVR_RemoteAlarminStateGet;

    rci.ifTriggerRecordEvent = HY_DVR_TriggerRecordEvent;
	
	HieServer_RemoteCommandIf(&rci);

	//远程硬盘分组管理
	memset(&m_DiskMan_DataSize, 0, sizeof (m_DiskMan_DataSize));
	m_DiskMan_DataSize.dwDataSizeReady = 1;
	m_DiskMan_DataSize.dwDoDiskManageReady = 1;

	//注册远程硬盘分组管理回调
	dgmi.ifDiskGroupManage = HY_DVR_DiskGroupManage;
	dgmi.ifGetDataSize = HY_DVR_GetDataSizeEx;
	HieServer_DiskGroupManageMultiTypeIf(&dgmi);

	return 0;
}

static int BackupCallback(HANDLE hDataPopper, int iChannel, int bIndexFrame, unsigned short wFrameType, time_t FrameTime, unsigned char *pBuf,
						  unsigned long dwSize, unsigned long EventID)
{
	return 0;
};

int CIU_GetDataSize(DWORD dwChanMask, DWORD dwDiskGroupMask, DWORD dwRecTypeMask, DWORD tStartTime, DWORD tEndTime, unsigned long long *dwSize)
{
#ifndef DEBUG_NOT_USE_STR_LIB

	MPU_DBG("CIU_GetDataSize\n");
	CDPManager *pDPManager = CDPManager::GetInstance();

	if (NULL == pDPManager)
	{
		MPU_ERR("Get CDPManage Failed\n");
		return -1;
	}

	void *hDPop = pDPManager->DPM_Create();

	if (NULL == hDPop)
	{
		MPU_ERR("Create CDPop Failed\n");
		return -1;
	}

	int ret = 0;

	ret = pDPManager->DPM_SetAttr(hDPop, dwDiskGroupMask, 0, tStartTime, tEndTime, BackupCallback, 0);
	if (0 != ret)
	{
		MPU_ERR("Set CDPop Failed, dwDiskgroupMask = %d, nRefChan = 0, tStartTime = %ld, tStopTime = %ld, Cb= %p\n", (int) dwDiskGroupMask,
				(time_t) tStartTime, (time_t) tEndTime, BackupCallback);
		pDPManager->DPM_Destroy(hDPop);
		return -1;
	}

	int i = 0;

	for (i = 0; i < MAX_ENC_CHANNEL_NUM; i++)
	{
		if ((dwChanMask >> i) & 1)
		{
			MPU_DBG("BackUp Add Ch %d\n", i);
			pDPManager->DPM_AddCh(hDPop, i, false);
		}
	}

	for (i = 0; i < (int) (sizeof (dwRecTypeMask) * 8); i++)
	{
		if ((dwRecTypeMask >> i) & 1)
		{
			MPU_DBG("BackUp Add Rec Type %d\n", i);
			pDPManager->DPM_AddRecType(hDPop, i);
		}
	}

	*dwSize = pDPManager->DPM_GetSize(hDPop);
	pDPManager->DPM_Destroy(hDPop);
	if ((unsigned long long) -1 == *dwSize)
	{
		MPU_ERR("Get Size Failed\n");
		return -1;
	}

	MPU_DBG("DPM_GetSize() 返回%lluKB\n", *dwSize);
	return 0;
#else
	return -1;
#endif
}
