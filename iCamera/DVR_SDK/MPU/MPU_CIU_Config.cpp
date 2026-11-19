
/*!
 *\file	MPU_CIU_Config_Net.cpp
 *\brief	MPU模块，远程配置，网络部分
 *\author	林云
 *\date	2009/8/6

功能：
-#	远程配置，网络部分

说明：
-#	本文件使用新的网络库实现
 */

#include "MPU_CIU_global.h"

/*
* @brief
*	获取设备配置信息回调接口
* @param dwUserID
*	用户ID,有效用户ID由1开始,0表示SDK操作配置信息
* @param dwMajorType
*	配置信息主类型
* @param dwMinorType
*	配置信息子类型
* @param pBuffer
*	配置信息缓冲区地址
* @param dwBufLen
*	配置信息缓冲区长度
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
static int CIU_GetConfigure(eProtocolID eProtocol, DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen)
{
	int nRet = IDVR_OK;

#if 0
	//判断用户权限
	if (0 == CIU_CheckRemoteUser(dwUserID, REMOTE_SETTINGS))
	{
		MPU_ERR("no config right\n");
		return IDVR_CIU_NO_RIGHT;
	}
#endif
//跟tutk的潜规则
#if 1
	if(((DWORD)-2 == dwUserID))
	{
		TutkGetAdminUserID(&dwUserID);	
	}
#endif

	MPU_DBG("ID %d, Major %d, Minor %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType);

	MPU_CHECK_POINTER_PARAMETER(pBuffer);
	MPU_CHECK_POINTER_PARAMETER(pdwBufLen);
	switch (dwMajorType)
	{
		case HY_DVR_GET_NETCFG:	//获取网络配置信息
			nRet = CIU_GetConfigure_Net(dwUserID, dwMajorType, dwMinorType, pBuffer, pdwBufLen);
			break;

		case HY_DVR_GET_NETCFG_EX://获取网络配置信息(wifi和Email时间间隔相关配置)
			nRet = CIU_GetConfigure_Net_EX(dwUserID, dwMajorType, dwMinorType, pBuffer, pdwBufLen);
			break;

		case HY_DVR_GET_NETSERVERCFG:	//获取网络服务器配置信息
			nRet = CIU_GetConfigure_Server(dwUserID, dwMajorType, dwMinorType, pBuffer, pdwBufLen);
			break;
		case HY_DVR_GET_PICCFG:
			nRet = CIU_GetConfigure_Pic(dwUserID, dwMajorType, dwMinorType, pBuffer, pdwBufLen);
			break;
        /* 设置编解码配置 */
		case HY_DVR_GET_COMPRESSCFG:
			nRet = CIU_GetConfigure_Compress(dwUserID, dwMajorType, dwMinorType, pBuffer, pdwBufLen);
			break;

        /* 获取编码参数(V2版)(fc 2016年9月13日 15:12:22) */
        case HY_DVR_GET_COMPRESSCFG_V2:
			nRet = CIU_GetConfigure_Compress_V2(dwUserID, dwMajorType, dwMinorType, pBuffer, pdwBufLen);
			break;

		case HY_DVR_GET_RECORDCFG:
			nRet = CIU_GetConfigure_Record(dwUserID, dwMajorType, dwMinorType, pBuffer, pdwBufLen);
			break;

		case HY_DVR_GET_SYSTIME:
			nRet = CIU_GetConfigure_Time(dwUserID, dwMajorType, dwMinorType, pBuffer, pdwBufLen);
			break;

		case HY_DVR_GET_PTZCFG:
			nRet = CIU_GetConfigure_Ptz(dwUserID, dwMajorType, dwMinorType, pBuffer, pdwBufLen);
			break;

		case HY_DVR_GET_SERIALCFG:
			nRet = CIU_GetConfigure_Serial(dwUserID, dwMajorType, dwMinorType, pBuffer, pdwBufLen);
			break;

		case HY_DVR_GET_ALARMCFG:
			nRet = CIU_GetConfigure_Alarm(dwUserID, dwMajorType, dwMinorType, pBuffer, pdwBufLen);
			break;
        
        /* 添加抓图配置信息(新添) */
        case HY_DVR_GET_ALARMCFG_EX:
			nRet = CIU_GetConfigure_AlarmEx(dwUserID, dwMajorType, dwMinorType, pBuffer, pdwBufLen);
			break;

        /* 添加抓图配置信息(新添) ，支持真实分辨率，而不是分辨率宏*/
        case HY_DVR_GET_ALARMCFG_EX_V2:
			nRet = CIU_GetConfigure_AlarmExV2(dwUserID, dwMajorType, dwMinorType, pBuffer, pdwBufLen);
			break;

        /* 获取用户自定义透传的信息 */
        case HY_DVR_GET_DEVICE_CUSTOM_FUCNTION:
            nRet = CIU_GetConfigure_DeviceCustomFunction(dwUserID, dwMajorType, dwMinorType, pBuffer, pdwBufLen);
            break;

		/* 获取录像文件列表属性(获取当前总共文件记录有多少条和每页有多少条) */
        case HY_DVR_GET_RECORD_FILE_LIST_ATTRIBUTE:
            nRet = CIU_GetConfigure_RecordFileListAttr(dwUserID, dwMajorType, dwMinorType, pBuffer, pdwBufLen);
            break;

        /* 获取录像文件当前显示的是哪一页 */
        case HY_DVR_GET_RECORD_FILE_CURRENT_PAGE:
            nRet = CIU_GetConfigure_RecordFileCurrentPage(dwUserID, dwMajorType, dwMinorType, pBuffer, pdwBufLen);
            break;

        /* 获取录像文件列表 */
        case HY_DVR_GET_RECORD_FILE_LIST:
            nRet = CIU_GetConfigure_RecordFileList(dwUserID, dwMajorType, dwMinorType, pBuffer, pdwBufLen);
            break;

        /* 获取抓图文件列表属性(获取当前总共文件记录有多少条和每页有多少条) */
        case HY_DVR_GET_SNAPSHOT_FILE_LIST_ATTRIBUTE:
            nRet = CIU_GetConfigure_SnapshotFileListAttr(dwUserID, dwMajorType, dwMinorType, pBuffer, pdwBufLen);
            break;

        /* 获取抓图文件当前显示的是哪一页 */
        case HY_DVR_GET_SNAPSHOT_FILE_CURRENT_PAGE:
            nRet = CIU_GetConfigure_SnapshotFileCurrentPage(dwUserID, dwMajorType, dwMinorType, pBuffer, pdwBufLen);
            break;

        /* 抓图文件列表 */
        case HY_DVR_GET_SNAPSHOT_FILE_LIST:
            nRet = CIU_GetConfigure_SnapshotFileList(dwUserID, dwMajorType, dwMinorType, pBuffer, pdwBufLen);
            break;

		/* 获取定时抓图配置 */
        case HY_DVR_GET_TIMED_CAPTURE_CFG:
            nRet = CIU_GetConfigure_TimedCaptureCfg(dwUserID, dwMajorType, dwMinorType, pBuffer, pdwBufLen);
            break;
		
		case HY_DVR_GET_VIDEOOUTCFG:
			nRet = CIU_GetConfigure_VideoOut(dwUserID, dwMajorType, dwMinorType, pBuffer, pdwBufLen);
			break;

		case HY_DVR_GET_USERCFG:
			nRet = CIU_GetConfigure_User(dwUserID, dwMajorType, dwMinorType, pBuffer, pdwBufLen);
			break;

		case HY_DVR_GET_DEVICEINFO:
			nRet = CIU_GetConfigure_DevInfo(dwUserID, dwMajorType, dwMinorType, pBuffer, pdwBufLen);
			break;

		case HY_DVR_GET_DEVICECFG:
			nRet = CIU_GetConfigure_DevConfig(dwUserID, dwMajorType, dwMinorType, pBuffer, pdwBufLen);
			break;

		case HY_DVR_GET_STORAGEINFO:
			nRet = CIU_GetConfigure_Storage(dwUserID, dwMajorType, dwMinorType, pBuffer, pdwBufLen);
			break;

		case HY_DVR_GET_DEVICESTATE:
			nRet = CIU_GetConfigure_DevState(dwUserID, dwMajorType, dwMinorType, pBuffer, pdwBufLen);
			break;

		case HY_DVR_GET_DEVICEMAINTENANCE:
			nRet = CIU_GetConfigure_DevMaintenance(dwUserID, dwMajorType, dwMinorType, pBuffer, pdwBufLen);
			break;
#if 0
		case HY_DVR_GET_DEVICECUSTOM:
			nRet = CIU_GetConfigure_DevCustom(dwUserID, dwMajorType, dwMinorType, pBuffer, pdwBufLen);
			break;
#endif
		case HY_DVR_GET_HXHT:
			nRet = CIU_GetConfigure_Hxht(dwUserID, dwMajorType, dwMinorType, pBuffer, pdwBufLen);
			break;

		case HY_DVR_GET_PROTOCOL:
			nRet = CIU_GetConfigure_Protocol(dwUserID, dwMajorType, dwMinorType, pBuffer, pdwBufLen);
			break;

		case HY_DVR_GET_NXP8850ISP:
			nRet = CIU_GetConfigure_ISP(dwUserID, dwMajorType, dwMinorType, pBuffer, pdwBufLen);
			break;
			
		/* 获取IPC图像配置信息 */	
		case HY_DVR_IPC_GET_ISPCFG:
			nRet = CIU_GetConfigure_IPC_ISP(dwUserID, dwMajorType, dwMinorType, pBuffer, pdwBufLen);
			break;
		/* 获取IRCUT相关配置信息 */
        case HY_DVR_COM_GET_ISPCFG_EX:
			nRet = CIU_GetConfigure_IR_CUT(dwUserID, dwMajorType, dwMinorType, pBuffer, pdwBufLen);
			break;
		case HY_DVR_GET_TONE_ISP:
			nRet = CIU_GetConfigure_tone_ISP(dwUserID, dwMajorType, dwMinorType, pBuffer, pdwBufLen);
			break;
		/* 获取ROI相关配置信息 */
        case HY_DVR_GET_ROI_CFG:      //  155
			nRet = CIU_GetConfigure_ROI(dwUserID, dwMajorType, dwMinorType, pBuffer, pdwBufLen);
			break;
		case HY_DVR_GET_DEVICEINFO_EX: //157
			nRet = CIU_GetConfigure_SOFT_POWER(dwUserID, dwMajorType, dwMinorType, pBuffer, pdwBufLen);
			break;

        /* NVR获取ISP相关参数 */
        case HY_DVR_NVR_GET_ISPCFG:
            nRet = CIU_GetConfigure_NVR_ISP(dwUserID, dwMajorType, dwMinorType, pBuffer, pdwBufLen);
            break;

        /* 获取配置文件中缓冲区的相关参数 */
        case HY_DVR_GET_BUFFER_SIZE:
            nRet = CIU_GetConfigure_BUF_SIZE(dwUserID, dwMajorType, dwMinorType, pBuffer, pdwBufLen);
            break;

#ifdef __USE_RTMP__
		//获取RTMP码流推送协议配置	
		case HY_DVR_GET_RTMP_PUSH_CONFIG: 
			nRet = CIU_GetConfigure_RTMP_PUSH(dwUserID, dwMajorType, dwMinorType, pBuffer, pdwBufLen);	
			break;
#endif

		case HY_DVR_GET_DEVICE_CAPACITY_FILE_NAME:
            nRet = CIU_GetDeviceCapacity(dwUserID, dwMajorType, dwMinorType, pBuffer, pdwBufLen);
            break;

		case HY_DVR_GET_DEVICE_CAPACITY_FILE_NAME_V2:
			nRet = CIU_GetDeviceCapacity_V2(dwUserID, dwMajorType, dwMinorType, pBuffer, pdwBufLen);
			break;
        #if 0
		case HY_DVR_GET_EXTENT_OSD:
			nRet = CIU_GetExtentOSD(dwUserID, dwMajorType, dwMinorType, pBuffer, pdwBufLen);	
			break;
		#endif
		case HY_DVR_GET_PICTURE_BY_BUFFER:
			nRet = CIU_GetThumbnail(dwUserID, dwMajorType, dwMinorType, pBuffer, pdwBufLen);
			break;
			
        //获取授权信息 (HY_DVR_AUTH_INFO)
        case HY_DVR_GET_AUTH_INFO:
            nRet = CIU_GetConfigure_AUTH_INFO(dwUserID, dwMajorType, dwMinorType, pBuffer, pdwBufLen);  
            break;

        case HY_DVR_GET_DEVICE_ERR_INFO:
            nRet = CIU_GetDeviceErrorInfo(dwUserID, dwMajorType, dwMinorType, pBuffer, pdwBufLen);
            break;

		//获取声音报警的高级配置
		case HY_DVR_GET_AUDIO_ALARM_CONFIG	: 
			nRet = CIU_GetConfigure_AUDIO_ALARM_ADVANCE_CONFIG(dwUserID, dwMajorType, dwMinorType, pBuffer, pdwBufLen);	
			break;

        case HY_DVR_GET_CONNECT_RESOURCE_INFO: 
            nRet = CIU_GetConfigure_Connect_Resource(dwUserID, dwMajorType, dwMinorType, pBuffer, pdwBufLen);
            break;

		//设置人脸抓拍配置
		case HY_DVR_GET_FACE_SNAP: 
			nRet = CIU_GetConfigure_FaceCap(dwUserID, dwMajorType, dwMinorType, pBuffer, pdwBufLen);	
			break;

		case HY_DVR_GET_OSD_V2:
			nRet = CIU_GetOSDV2(dwUserID, dwMajorType, dwMinorType, pBuffer, pdwBufLen);
			break;

#if defined (__NET_PROTOCOL_CONTROL__)	
		case HY_DVR_GET_THIRD_PARTY_PROTOCOL_CONFIG:
			nRet = CIU_GetConfigureOnOff_Protocol(dwUserID, dwMajorType, dwMinorType, pBuffer, pdwBufLen);
			break;
#endif
		case HY_DVR_GET_STREAM_TRANSPORT_TYPE:
			nRet = CIU_GetConfigure_RealStreamBuffer(dwUserID, dwMajorType, dwMinorType, pBuffer, pdwBufLen);
			break;

#if defined (__IS_SUPPORT_P6SMESSAGE__)
		case HY_DVR_P6S_MP_GET_SEVER_PARAM:
			nRet = CIU_GetConfigure_P6S_MP_SERVER_PARAM(dwUserID, dwMajorType, dwMinorType, pBuffer, pdwBufLen);
			break;

		//获取错误码信息
		case HY_DVR_GET_ERROR_CODE_INFO:
			nRet = CIU_GetConfigure_ERROR_CODE(dwUserID, dwMajorType, dwMinorType, pBuffer, pdwBufLen);
			break;

		//获取服务器必选参数
		case HY_DVR_GET_ERROR_CODE_SERVER_INFO:
			nRet = CIU_GetConfigure_ERROR_CODE_SERVER_INFO(dwUserID, dwMajorType, dwMinorType, pBuffer, pdwBufLen);
			break;
#endif
		case HY_DVR_GET_VOICE_BROADCAST_LANGUAGE:
			nRet = CIU_GetConfigure_VoiceBroadcastLanguage(dwUserID, dwMajorType, dwMinorType, pBuffer, pdwBufLen);
			break;
#ifdef __MODULE_4G_SUPPORT__	
		case HY_DVR_GET_4G_CARD_SELECT_CFG:
			nRet = CIU_GetConfigure_4GCardSelect(dwUserID, dwMajorType, dwMinorType, pBuffer, pdwBufLen);
			break;
		case HY_DVR_GET_4G_CARD_CFG:
			nRet = CIU_GetConfigure_4GCardInfo(dwUserID, dwMajorType, dwMinorType, pBuffer, pdwBufLen);
			break;
		case HY_DVR_GET_SIM_CARD_INFO:
			nRet = CIU_GetConfigure_SimIndexCardInfo(dwUserID, dwMajorType, dwMinorType, pBuffer, pdwBufLen);
			break;
        case HY_DVR_FL_POST_INIT_PARAM:
			nRet = CIU_GetConfigure_FL(dwUserID, dwMajorType, dwMinorType, pBuffer, pdwBufLen);
			break;
#endif
        //获取设备对讲模式
        case HY_DVR_GET_DEVICE_INTERCOM_MODE:
            nRet = CIU_GetConfigure_DeviceIntercomMode(dwUserID, dwMajorType, dwMinorType, pBuffer, pdwBufLen);
            break;
#ifdef __SUPPORT_P2P_MAXCLIENT__
        case HY_DVR_GET_STREAM_OUT_OF_RANGE:
            nRet = CIU_GetConfigure_StreamOutOfRange(dwUserID, dwMajorType, dwMinorType, pBuffer, pdwBufLen);
            break;
#endif
		/* 获取设备一键夜灯信息 */	
		case HY_DVR_GET_NIGHT_LED_INFO:
			nRet = CIU_GetConfigure_NIGHT_LED_INFO(dwUserID, dwMajorType, dwMinorType, pBuffer, pdwBufLen);
			break;

        //获取设备睡眠信息
        case HY_DVR_GET_DEVICE_SLEEP_INFO:
            nRet = CIU_GetConfigure_DEVICE_SLEEP_INFO(dwUserID, dwMajorType, dwMinorType, pBuffer, pdwBufLen);
            break;
        case HY_DVR_GET_DEVICE_SLEEP_CONTROL:
            nRet = CIU_GetConfigure_DEVICE_SLEEP_CONTROL(dwUserID, dwMajorType, dwMinorType, pBuffer, pdwBufLen);
            break;

		case HY_DVR_GET_CLOUD_STORAGE:
			nRet = CIU_GetConfigure_CloudStorage(dwUserID, dwMajorType, dwMinorType, pBuffer, pdwBufLen);
			break;

#ifdef __IS_SUPPORT_CLOUDWORLD__
		case HY_DVR_GET_CLOUD_STORAGE_MODE:
			nRet = CIU_GetConfigure_CloudStorageMode(dwUserID, dwMajorType, dwMinorType, pBuffer, pdwBufLen);
			break;
		//帧率同步问题
		case HY_DVR_GET_CURRENT_FRAME_PARAM:
			nRet = CIU_GetConfigure_CurrentFrame_Param(dwUserID, dwMajorType, dwMinorType, pBuffer, pdwBufLen);
			break;
#endif
        case HY_DVR_GET_DEVICE_DIGITAL_MULTIPLE:
            nRet = CIU_GetConfigure_DeviceDragDigitalMultiple(dwUserID, dwMajorType, dwMinorType, pBuffer, pdwBufLen);
            break;

        //声音侦测
		case HY_DVR_GET_CRY_SCREAM_DETECT:
			nRet = CIU_GetConfigure_CryDetect(dwUserID, dwMajorType, dwMinorType, pBuffer, pdwBufLen);
			break;

#if defined (__IS_SUPPORT_REMINDER_SCHEDULE__)
		// 获取定时提醒参数
		case HY_DVR_GET_TIME_REMIND:
			nRet = CIU_GetRemindSchedule(dwUserID, dwMajorType, dwMinorType, pBuffer, pdwBufLen);
			break;
#endif

#if defined (__IS_SUPPORT_LCD_MANAGER__)
		// 获取屏幕管理参数
		case HY_DVR_GET_SCREEN_MANAGEMENT_CFG:
			nRet = CIU_GetConfigure_ScreenManagent(dwUserID, dwMajorType, dwMinorType, pBuffer, pdwBufLen);
			break;
#endif
		case HY_DVR_GET_TCPRELAY_INFO:
			nRet = CIU_GetConfigure_TcpRelay(dwUserID, dwMajorType, dwMinorType, pBuffer, pdwBufLen);
			break;
		default:
			{
				MPU_DBG("unsupported dwMajorType = %lu\n", dwMajorType);
				nRet = IDVR_CIU_UNSUPPORTED_MAJORTYPE;
			}
			break;
	}

	MPU_DBG("ID %d, Major %d, Minor %d, return %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType, nRet);
	return nRet;
}

/*
* @brief
*	设置设备配置信息回调接口
* @param dwUserID 
*	用户ID,有效用户ID由1开始,0表示SDK操作配置信息
* @param dwMajorType
*	配置信息主类型
* @param dwMinorType
*	配置信息子类型
* @param pBuffer
*	配置信息缓冲区地址
* @param dwBufLen
*	配置信息缓冲区长度
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
static int CIU_SetConfigure(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, const BYTE * pBuffer, DWORD dwBufLen)
{
	int nRet = IDVR_OK;
    int nNeedReboot = 0;
    
//跟tutk的潜规则
#if 1
	if(((DWORD)-2 == dwUserID))
	{
		TutkGetAdminUserID(&dwUserID);	
	}
#endif

	//先判断用户权限
	if (0 == CIU_CheckRemoteUser(dwUserID, REMOTE_SETTINGS))
	{
		MPU_ERR("no config right, ID %d, Major %d, Minor %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType);
		return IDVR_CIU_NO_RIGHT;
	}

	MPU_DBG("ID %d, Major %d, Minor %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType);
	//判断指针是否为空
	MPU_CHECK_POINTER_PARAMETER(pBuffer);

	switch (dwMajorType)
	{
		case HY_DVR_SET_NETCFG:	//设置网络配置信息
			nRet = CIU_SetConfigure_Net(dwUserID, dwMajorType, dwMinorType, pBuffer, dwBufLen);
			break;

		case HY_DVR_SET_NETSERVERCFG:	//设置网络服务器配置信息
			nRet = CIU_SetConfigure_Server(dwUserID, dwMajorType, dwMinorType, pBuffer, dwBufLen);
			break;
			
        case HY_DVR_SET_NETCFG_EX://设置网络配置信息(wifi和Email时间间隔相关配置)
            nRet = CIU_SetConfigure_Net_EX(dwUserID, dwMajorType, dwMinorType, pBuffer, dwBufLen);
			break;

		case HY_DVR_SET_PICCFG:
			nRet = CIU_SetConfigure_Pic(dwUserID, dwMajorType, dwMinorType, pBuffer, dwBufLen);
			break;

		case HY_DVR_SET_COMPRESSCFG:
			nRet = CIU_SetConfigure_Compress(dwUserID, dwMajorType, dwMinorType, pBuffer, dwBufLen);
			break;

        /* 设置编码参数(V2版)(fc 2016年9月13日 15:12:27) */
        case HY_DVR_SET_COMPRESSCFG_V2:
			nRet = CIU_SetConfigure_Compress_V2(dwUserID, dwMajorType, dwMinorType, pBuffer, dwBufLen, &nNeedReboot);
			break;
			
		case HY_DVR_SET_RECORDCFG:
			nRet = CIU_SetConfigure_Record(dwUserID, dwMajorType, dwMinorType, pBuffer, dwBufLen);
			break;

		case HY_DVR_SET_SYSTIME:
			nRet = CIU_SetConfigure_Time(dwUserID, dwMajorType, dwMinorType, pBuffer, dwBufLen);
			break;

		case HY_DVR_SET_PTZCFG:
			nRet = CIU_SetConfigure_Ptz(dwUserID, dwMajorType, dwMinorType, pBuffer, dwBufLen);
			break;

		case HY_DVR_SET_SERIALCFG:
			nRet = CIU_SetConfigure_Serial(dwUserID, dwMajorType, dwMinorType, pBuffer, dwBufLen);
			break;

		case HY_DVR_SET_ALARMCFG:
			nRet = CIU_SetConfigure_Alarm(dwUserID, dwMajorType, dwMinorType, pBuffer, dwBufLen);
			break;
        /* 设置用户自定义透传的信息 */
        case HY_DVR_SET_DEVICE_CUSTOM_FUNCTION:
            nRet = CIU_SetConfigure_DeviceCustomFunction(dwUserID, dwMajorType, dwMinorType, pBuffer, dwBufLen);
            break;

		/* 设置录像文件列表属性(设置每一页显示多少条记录) */
        case HY_DVR_SET_RECORD_FILE_LIST_ATTRIBUTE:
            nRet = CIU_SetConfigure_RecordFileListAttr(dwUserID, dwMajorType, dwMinorType, pBuffer, dwBufLen);
            break;

        /* 设置录像文件当前页(设置当前需要显示哪一页) */
        case HY_DVR_SET_RECORD_FILE_CURRENT_PAGE:
            nRet = CIU_SetConfigure_RecordFileCurrentPage(dwUserID, dwMajorType, dwMinorType, pBuffer, dwBufLen);
            break;

        /* 设置录像文件列表 */
        case HY_DVR_SET_RECORD_FILE_LIST:
            nRet = CIU_SetConfigure_RecordFileList(dwUserID, dwMajorType, dwMinorType, pBuffer, dwBufLen);
            break;

        /* 设置抓图文件列表属性(设置每一页显示多少条记录) */
        case HY_DVR_SET_SNAPSHOT_FILE_LIST_ATTRIBUTE:
            nRet = CIU_SetConfigure_SnapshotFileListAttr(dwUserID, dwMajorType, dwMinorType, pBuffer, dwBufLen);
            break;

        /* 设置抓图文件当前页(设置当前需要显示哪一页) */
        case HY_DVR_SET_SNAPSHOT_FILE_CURRENT_PAGE:
            nRet = CIU_SetConfigure_SnapshotFileCurrentPage(dwUserID, dwMajorType, dwMinorType, pBuffer, dwBufLen);
            break;

        /* 设置抓图文件列表 */
        case HY_DVR_SET_SNAPSHOT_FILE_LIST:
            nRet = CIU_SetConfigure_SnapshotFileList(dwUserID, dwMajorType, dwMinorType, pBuffer, dwBufLen);
            break;

		/* 设置定时抓图配置 */
        case HY_DVR_SET_TIMED_CAPTURE_CFG:
            nRet = CIU_SetConfigure_TimedCaptureCfg(dwUserID, dwMajorType, dwMinorType, pBuffer, dwBufLen);
            break;

		case HY_DVR_SET_VIDEOOUTCFG:
			nRet = CIU_SetConfigure_VideoOut(dwUserID, dwMajorType, dwMinorType, pBuffer, dwBufLen);
			break;

		case HY_DVR_SET_USERCFG:
			nRet = CIU_SetConfigure_User(dwUserID, dwMajorType, dwMinorType, pBuffer, dwBufLen);
			break;

		case HY_DVR_SET_DEVICEINFO:
			nRet = CIU_SetConfigure_DevInfo(dwUserID, dwMajorType, dwMinorType, pBuffer, dwBufLen);
			break;

		case HY_DVR_SET_DEVICECFG:
			nRet = CIU_SetConfigure_DevConfig(dwUserID, dwMajorType, dwMinorType, pBuffer, dwBufLen);
			break;

		case HY_DVR_SET_STORAGEINFO:
			nRet = CIU_SetConfigure_Storage(dwUserID, dwMajorType, dwMinorType, pBuffer, dwBufLen);
			break;
        /* 设置自动维护模式 */
		case HY_DVR_SET_DEVICEMAINTENANCE:
			nRet = CIU_SetConfigure_DevMaintenance(dwUserID, dwMajorType, dwMinorType, pBuffer, dwBufLen);
			break;
#if 0
		case HY_DVR_SET_DEVICECUSTOM:
			nRet = CIU_SetConfigure_DevCustom(dwUserID, dwMajorType, dwMinorType, pBuffer, dwBufLen);
			break;
#endif
		case HY_DVR_SET_HXHT:
			nRet = CIU_SetConfigure_Hxht(dwUserID, dwMajorType, dwMinorType, pBuffer, dwBufLen);
			break;

		case HY_DVR_SET_NXP8850ISP:
			nRet = CIU_SetConfigure_ISP(dwUserID, dwMajorType, dwMinorType, pBuffer, dwBufLen);
			break;
		/* 图像配置(设置图像参数) */	
		case HY_DVR_IPC_SET_ISPCFG:
			nRet = CIU_SetConfigure_IPC_ISP(dwUserID, dwMajorType, dwMinorType, pBuffer, dwBufLen);
			break;
        /* 设置IRCUT阈值和切换时间 */
        case HY_DVR_COM_SET_ISPCFG_EX:
            nRet = CIU_SetConfigure_IR_CUT(dwUserID, dwMajorType, dwMinorType, pBuffer, dwBufLen);
            break;
        /* 设置gamma调节 */
		case HY_DVR_SET_TONE_ISP:
			nRet = CIU_SetConfigure_tone_ISP(dwUserID, dwMajorType, dwMinorType, pBuffer, dwBufLen);
			break;
        /* 设置ROI相关信息 */
        case HY_DVR_SET_ROI_CFG:  // 156
            nRet = CIU_SetConfigure_ROI(dwUserID, dwMajorType, dwMinorType, pBuffer, dwBufLen);
            break;
            
        /* NVR设置ISP相关参数 */
        case HY_DVR_NVR_SET_ISPCFG:
            nRet = CIU_SetConfigure_NVR_ISP(dwUserID, dwMajorType, dwMinorType, pBuffer, dwBufLen);
            break; 

#ifdef __USE_RTMP__
		//获取RTMP码流推送协议配置	
		case HY_DVR_SET_RTMP_PUSH_CONFIG: 
			nRet = CIU_SetConfigure_RTMP_PUSH(dwUserID, dwMajorType, dwMinorType, pBuffer, dwBufLen);	
			break;
#endif

        //设置设备对讲模式
        case HY_DVR_SET_DEVICE_INTERCOM_MODE:
            nRet = CIU_SetConfigure_DeviceIntercomMode(dwUserID, dwMajorType, dwMinorType, pBuffer, dwBufLen);
            break;
        #if 0
		case HY_DVR_SET_EXTENT_OSD:
			nRet = CIU_SetExtentOSD(dwUserID, dwMajorType, dwMinorType, pBuffer, dwBufLen); 
			break;
		#endif
        //设置授权信息 (HY_DVR_AUTH_INFO)
        case HY_DVR_SET_AUTH_INFO:
            nRet = CIU_SetConfigure_AUTH_INFO(dwUserID, dwMajorType, dwMinorType, pBuffer, dwBufLen);   
            break;

		//设置声音报警的高级配置
		case HY_DVR_SET_AUDIO_ALARM_CONFIG	: 
			nRet = CIU_SetConfigure_AUDIO_ALARM_ADVANCE_CONFIG(dwUserID, dwMajorType, dwMinorType, pBuffer, dwBufLen);	
			break;

		//设置人脸抓拍配置
		case HY_DVR_SET_FACE_SNAP: 
			nRet = CIU_SetConfigure_FaceCap(dwUserID, dwMajorType, dwMinorType, pBuffer, dwBufLen);
			break;

		case HY_DVR_SET_OSD_V2:
			nRet = CIU_SetOSDV2(dwUserID, dwMajorType, dwMinorType, pBuffer, dwBufLen);
			break;

		case HY_DVR_SET_MESSAGE_NOTIFY:
			nRet = CIU_SetMessageNotify(dwUserID, dwMajorType, dwMinorType, pBuffer, dwBufLen);
			break;

		#if defined (__NET_PROTOCOL_CONTROL__)	
		case HY_DVR_SET_THIRD_PARTY_PROTOCOL_CONFIG:
			nRet = CIU_SetConfigureOnOff_Protocol(dwUserID, dwMajorType, dwMinorType, pBuffer, dwBufLen, &nNeedReboot);
			break;
		#endif

		#if defined (__IS_SUPPORT_P6SMESSAGE__)
		case HY_DVR_P6S_MP_SET_SEVER_PARAM:
			nRet = CIU_SetConfigure_P6S_MP_SERVER_PARAM(dwUserID, dwMajorType, dwMinorType, pBuffer, dwBufLen);
			break;
		#endif

		case HY_DVR_SET_P6SMSG_POLICY:
			nRet = CIU_SetConfigure_P6smsgPolicy(dwUserID, dwMajorType, dwMinorType, pBuffer, dwBufLen);
			break;
        
        #if defined (__USE_APPLY_DID_MANUAL__)
        case HY_DVR_SET_APPLY_DID_MANUL://设置手动申请DID url信息
            nRet = CIU_SetConfigure_Net_DID(dwUserID, dwMajorType, dwMinorType, pBuffer, dwBufLen);
			break;
        #endif
		case HY_DVR_SET_VOICE_BROADCAST_LANGUAGE:
			nRet = CIU_SetConfigure_VoiceBroadcastLanguage(dwUserID, dwMajorType, dwMinorType, pBuffer, dwBufLen);
			break;
	#ifdef __MODULE_4G_SUPPORT__	
		case HY_DVR_SET_4G_CARD_SELECT_CFG:
			nRet = CIU_SetConfigure_4GCardSelect(dwUserID, dwMajorType, dwMinorType, pBuffer, dwBufLen);
			break;
    #endif
		/* 设置设备一键夜灯信息 */	
		case HY_DVR_SET_NIGHT_LED_INFO:
			nRet = CIU_SetConfigure_NIGHT_LED_INFO(dwUserID, dwMajorType, dwMinorType, pBuffer, dwBufLen);
			break;

        case HY_DVR_SET_DEVICE_SLEEP_INFO:
            nRet = CIU_SetConfigure_DEVICE_SLEEP_INFO(dwUserID, dwMajorType, dwMinorType, pBuffer, dwBufLen);
            break;
        case HY_DVR_SET_DEVICE_SLEEP_CONTROL:
            nRet = CIU_SetConfigure_DEVICE_SLEEP_CONTROL(dwUserID, dwMajorType, dwMinorType, pBuffer, dwBufLen);
            break;

		case HY_DVR_SET_CLOUD_STORAGE:
			nRet = CIU_SetConfigure_CloudStorage(dwUserID, dwMajorType, dwMinorType, pBuffer, dwBufLen);
			break;
#ifdef __IS_SUPPORT_CLOUDWORLD__
		case HY_DVR_SET_CLOUD_STORAGE_MODE:
			nRet = CIU_SetConfigure_CloudStorageMode(dwUserID, dwMajorType, dwMinorType, pBuffer, dwBufLen);
			break;
#endif
        case HY_DVR_SET_DEVICE_DIGITAL_MULTIPLE:
            nRet = CIU_SetConfigure_DeviceDragDigitalMultiple(dwUserID, dwMajorType, dwMinorType, pBuffer, dwBufLen);
            break;
        //声音侦测
		case HY_DVR_SET_CRY_SCREAM_DETECT:
			nRet = CIU_SetConfigure_CryDetect(dwUserID, dwMajorType, dwMinorType, pBuffer, dwBufLen);
			break;
    #if defined (__IS_SUPPORT_4G_CLOUD_DOMAIN__)
        case HY_DVR_SET_ZWHOST_API:
            nRet = CIU_SetConfigure_AddDomain(dwUserID, dwMajorType, dwMinorType, pBuffer, dwBufLen);
            break;
    #endif
#ifdef __ONE_CLICK_CALL__
		case HY_DVR_SET_DEVICE_ONE_BUTTON_CALL_CFG:
			nRet = CIU_SetConfigure_DeviceOneButtonCallCfg(dwUserID, dwMajorType, dwMinorType, pBuffer, dwBufLen);
			break;
#endif
        case HY_DVR_SET_P2P_COMMAND:
            nRet = CIU_SetConfigure_P2pCommand(dwUserID, dwMajorType, dwMinorType, pBuffer, dwBufLen);
            break;

#if defined (__IS_SUPPORT_REMINDER_SCHEDULE__)
        // 设置定时提醒参数
		case HY_DVR_SET_TIME_REMIND:
			nRet = CIU_SetRemindSchedule(dwUserID, dwMajorType, dwMinorType, pBuffer, dwBufLen);
			break;
#endif
#if defined (__IS_SUPPORT_LCD_MANAGER__)
            // 设置屏幕管理参数
            case HY_DVR_SET_SCREEN_MANAGEMENT_CFG:
                nRet = CIU_SetConfigure_ScreenManagent(dwUserID, dwMajorType, dwMinorType, pBuffer, dwBufLen);
                break;
#endif
		case HY_DVR_SET_TCPRELAY_INFO:
			nRet = CIU_SetConfigure_TcpRelay(dwUserID, dwMajorType, dwMinorType, pBuffer, dwBufLen);
			break;
		default:
			{
				MPU_ERR("unsupported dwMajorType = %lu\n", dwMajorType);
				nRet = IDVR_CIU_UNSUPPORTED_MAJORTYPE;
			}
			break;
	}
	//所有不需要保存配置的命令
	if ((IDVR_OK == nRet) && (NETCFG_NOTIFY_TUTK_STATE != (int) dwMinorType) 
		&& (NETCFG_NOTIFY_ULUCO_P2P_STATE != (int) dwMinorType)
		&& (NETCFG_NOTIFY_ULUCO_P2P_UID != (int) dwMinorType)
		&& (NETCFG_WHO_PROBE_CONF != (int) dwMinorType))//TUTK保存的是单独的配置文件
	{
		MPU_PAR_SaveConfig();
		MPU_DBG("************************Save Config*********************\n");
	}

    if (nNeedReboot == 1)
    {
    	MPU_PAR_SaveConfigReal();
		PAR_ForbiddenSaveFile();    
        MPU_INFO("\n...编码参数修改，需要重启...\n");
		RebootLinux(REBOOT_CIU_ENC_CHANGE);
    }

	MPU_DBG("ID %d, Major %d, Minor %d, return %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType, nRet);
	return nRet;
}

//注册远程配置回调
void CIU_RegisterConfig()
{
	int nSDKRet;

	ConfigureInterface ci;

	ci.ifGetConfigure = CIU_GetConfigure;
	ci.ifSetConfigure = CIU_SetConfigure;

	nSDKRet = HieServer_ConfigureIf(&ci);
	if (0 != nSDKRet)
	{
		MPU_ERR("call HieServer_ConfigureIf() fail, return %d\n", nSDKRet);
		MPU_CHK(0 == nSDKRet);
	}
	else
	{
		//MPU_DBG("ok\n");
	}
}

//取消注册远程配置回调
void CIU_UnRegisterConfig()
{
	int nSDKRet;

	ConfigureInterface ci;

	ci.ifGetConfigure = NULL;
	ci.ifSetConfigure = NULL;

	nSDKRet = HieServer_ConfigureIf(&ci);
	if (0 != nSDKRet)
	{
		MPU_ERR("call HieServer_ConfigureIf() fail, return %d\n", nSDKRet);
		MPU_CHK(0 == nSDKRet);
	}
	else
	{
		//MPU_DBG("ok\n");
	}
}

