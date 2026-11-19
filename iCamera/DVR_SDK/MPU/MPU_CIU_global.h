
/*!
 *\file		MPU_CIU_global.h
 *\brief	MPU模块,CIU部分内部使用全局宏、结构、变量、函数定义
 *\author	林云
 *\date		2009/03/19
 */

#ifndef _INC_MPU_CIU_GLOBAL_H_
#define _INC_MPU_CIU_GLOBAL_H_

#pragma once


//全局头文件
#include "MPU_global.h"			//导出接口
#include "PUB/macro_pthread.h"
#include "PUB/tagLog.h"
#include "common_pub.h"

//其它工程头文件


/**************************************************************************
 *                        常量                                            *
 **************************************************************************/


/**************************************************************************
 *                          宏定义                                         *
 **************************************************************************/
//#define CIU_USE_OLD_LIB

#ifdef CIU_USE_OLD_LIB
#include "net_remote.h"
#include "hieserver.h"
#else
#include "newhie/HieServer.h"	//网络SDK
#include "newhie/HieServer_Common.h"	//网络SDK
#endif
#include "MPU_CIU_Preview.h"
#include "MPU_CIU_LogQuery.h"
#include "MPU_CIU_Login.h"
#include "zwsdk.h"

//CIU重启网络服务线程改成JOINABLE
#define CIU_RESET_THREAD_CREATE_JOINABLE

#define FUNCTIONTYPE "FunctionType=1\n"//用于区分产测工具下发的数据是通用配置还是详细配置，如果FunctionType=1则表示详细配置，否则是通用配置

//当用户ID为-1时，具有一切权限
#define VALUE_INVALUE       0xFFFFFFFF

#define CIU_ERR(nSDKRet, nRet)											\
do																	\
{																	\
	if ((IDVR_CIU_STRUCT_UNVALID != (nSDKRet)) && (IDVR_OK != (nSDKRet)))	\
	{																\
		MPU_ERR("return %d\n", (nSDKRet));								\
		nRet = (nSDKRet);												\
	}																\
}																	\
while (0)


/**************************************************************************
 *                          模块内部接口                                  *
 **************************************************************************/

//!\brief   更新端口配置
void CIU_UpdatePortConfig();

//!\brief    工厂测试用 开启语音对讲
void CIU_StartLocalVoice(DWORD dwChannel);

//!\brief   工厂 测试用 停止语音对讲
void CIU_StopLocalVoice(DWORD dwChannel);

//!\brief   初始化远程预览
void CIU_RegisterPreview();

#ifdef __USE_CIRCLEQUEUE__
//!\brief   初始化远程预览
int CIU_GetMjpegStreamCircleQueueInterface(LPPullRealStreamInterface pMjpegStreamInterface);
#endif

void CIU_SetTalkCallBack(Talk_Callback pStopTalkCallBack);

//!\brief   初始化远程下载
void CIU_RegisterDownload();

//!\brief   初始化远程升级
void CIU_RegisterUpload();

//!\brief   设置远程升级回调函数
void CIU_SetRemoteUploadCallBack(Upload_Callback pRemoteUploadCallBack);

//!\brief   停止远程升级
int CIU_StopRemoteUpdate();

// 远程通知开启ftp升级
int CIU_FtpUpdateStart(LPSystemUpdateParam ptFtpUpdateParam);

//!\brief   初始化远程用户登陆
void CIU_RegisterLogin();

//!\brief  获取用户在线信息
int CIU_IsOnline(HANDLE hUser);

//!\brief  获取用户信息
int CIU_GetUserInfo(DWORD dwUserID, HANDLE * hUser, const char **ppcUserName, DWORD * pdwTime, const char **ppcIpAddr, eProtocolID * peProtocol);

int CIU_GetUserInfo(DWORD dwUserID, HANDLE * hUser, const char **ppcUserName, const char **ppcTime, const char **ppcIpAddr);

int CIU_GetUserInfo(DWORD dwUserID, const char **ppcUserName, unsigned long *pdwIpAddr);

//!\brief  将字符IP转换为数字
int CIU_TransIpFromStrToDw(char *pcIpAddr, DWORD * pdwIpaddr);


//!\brief       判断是否同一用户
//!\return  0表示不是，1表示是
int CIU_IsUser(HANDLE hUser, const char *pszUserName);

int CIU_IsUser(DWORD dwUserID, HANDLE hUser);

int CIU_IsUser(DWORD dwUserID, const char *pszUserName);

int TutkGetAdminUserID(DWORD *pdwUserID);

//!\brief       添加远程操作日志
//!\param   dwUserID                用户ID
//!\param   nLogType                    日志类型
//!\param   nDetail             日志详细信息
//!\param   nAlertChannel           产生日志信息的通道号
//!\param   dwRecordChannelFlag 联动录像的通道标记
//!\return  0添加成功 -1表示失败
int CIU_OPL_AddLog(DWORD dwUserID, int nLogType, int nDetail, int nAlertChannel, unsigned long dwRecordChannelFlag);

//!\brief       判断是否具有权限
//!\return  0表示无权限，1表示有权限
int CIU_CheckRemoteUser(DWORD dwUserID, REMOTE_ACTION eAction);

//!\brief       判断是否具有权限
//!\return  0表示无权限，1表示有权限
int CIU_CheckRemoteUser(DWORD dwUserID, CHANNEL_ACTION eAction, int nCh);

//!\brief       判断是否具有写权限
//!\return  0表示无权限，1表示有权限
int CIU_CheckRemoteWriteUser(DWORD dwUserID, REMOTE_ACTION_V1 eAction);

//!\brief       判断是否具有读权限
//!\return  0表示无权限，1表示有权限
int CIU_CheckRemoteReadUser(DWORD dwUserID, REMOTE_ACTION_V1 eAction);

//!\brief  获取是否启用IP 权限管理
//!\param   
//!\return  0 不启用，1启用,其它错误
int CIU_GetIsIPRightEnable();

//!\brief  设置是否启用IP 权限管理
//!\param    0 不启用，1启用
//!\return 
int CIU_SetIsIPRightEnable(int iIpRightEnable);

//!\brief  获取被屏蔽的ip
//!\param   ptIPDisable ip数组的地址
//!\return  被屏蔽的ip的个数
int CIU_GetIPEnableList(DWORD * pdwIPEnableList);

//!\brief  添加许可IP
//!\param   szIPAddress ip
//!\return  0成功
int CIU_ChangeIPEnable(DWORD dwIpAddr, int iEnable);

//!\brief  清除所有IP
//!\return  0成功
int CIU_ClearIPEnableList();

//!\brief  获取用户查询参数
//!\param   dwUserID 用户ID
//!\return  PT_REMOTEQUEERY 查询参数的句柄
PT_REMOTEQUEERY CIU_GetLogQueryParam(DWORD dwUserID);

//!\brief   网络服务重启
void CIU_ResetServer_Logout();

//!\brief   注册远程配置回调函数
void CIU_RegisterConfig();

//!\brief   取消注册远程配置回调函数
void CIU_UnRegisterConfig();

//!\brief   远程配置，获取网络参数
int CIU_GetConfigure_Net(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen);

int CIU_GetConfigure_Net_EX(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen);

//!\brief   远程配置，设置网络参数
int CIU_SetConfigure_Net(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, const BYTE * pBuffer, DWORD dwBufLen);

//!\brief   远程配置，设置网络拓展参数(WIFI和Email时间间隔)
int CIU_SetConfigure_Net_EX(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, const BYTE * pBuffer, DWORD dwBufLen);

#if defined (__NET_PROTOCOL_CONTROL__)
//!\brief   远程配置，设置第三方协议开关
int CIU_SetConfigureOnOff_Protocol(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, const BYTE * pBuffer, DWORD dwBufLen, int *nNeedReboot);

int CIU_GetConfigureOnOff_Protocol(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen);
#endif

//!\brief   远程配置，设置网络参数
int CIU_SetConfigure_Net_DID(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, const BYTE * pBuffer, DWORD dwBufLen);

//!\brief   远程配置，获取网络服务器参数
int CIU_GetConfigure_Server(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen);

//!\brief   远程配置，设置网络服务器参数
int CIU_SetConfigure_Server(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, const BYTE * pBuffer, DWORD dwBufLen);

//!\brief   远程配置，获取视频参数
int CIU_GetConfigure_Pic(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen);

//!\brief   远程配置，设置视频参数
int CIU_SetConfigure_Pic(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, const BYTE * pBuffer, DWORD dwBufLen);

//!\brief   远程配置，获取编码参数
int CIU_GetConfigure_Compress(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen);

//!\brief   远程配置，获取编码参数
int CIU_GetConfigure_Compress_V2(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen);

//!\brief   远程配置，设置编码参数
int CIU_SetConfigure_Compress_V2(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, const BYTE * pBuffer, DWORD dwBufLen, int *nNeedReboot);


//!\brief   远程配置，设置编码参数
int CIU_SetConfigure_Compress(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, const BYTE * pBuffer, DWORD dwBufLen);

//!\brief   远程配置，获取录像参数
int CIU_GetConfigure_Record(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen);

//!\brief   远程配置，设置录像参数
int CIU_SetConfigure_Record(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, const BYTE * pBuffer, DWORD dwBufLen);

/* 获取HTTP Event事件触发相关信息 */
int CIU_GetConfigure_HttpEvent(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen);

/* 设置HTTP Event事件触发相关信息 */
int CIU_SetConfigure_HttpEvent(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, const BYTE * pBuffer, DWORD dwBufLen);

/* 获取录像文件列表属性(获取当前总共文件记录有多少条和每页有多少条) */
int CIU_GetConfigure_RecordFileListAttr(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen);

/* 设置录像文件列表属性(设置每一页显示多少条记录) */
int CIU_SetConfigure_RecordFileListAttr(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, const BYTE * pBuffer, DWORD dwBufLen);

/* 获取录像文件当前显示的是哪一页 */
int CIU_GetConfigure_RecordFileCurrentPage(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen);

/* 设置录像文件当前页(设置当前需要显示哪一页) */
int CIU_SetConfigure_RecordFileCurrentPage(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, const BYTE * pBuffer, DWORD dwBufLen);

/* 获取录像文件列表 */
int CIU_GetConfigure_RecordFileList(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen);

/* 设置录像文件列表(无效) */
int CIU_SetConfigure_RecordFileList(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, const BYTE * pBuffer, DWORD dwBufLen);

/* 获取抓图文件列表属性(获取当前总共文件记录有多少条和每页有多少条) */
int CIU_GetConfigure_SnapshotFileListAttr(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen);

/* 设置抓图文件列表属性(设置每一页显示多少条记录) */
int CIU_SetConfigure_SnapshotFileListAttr(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, const BYTE * pBuffer, DWORD dwBufLen);

/* 获取抓图文件当前显示的是哪一页 */
int CIU_GetConfigure_SnapshotFileCurrentPage(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen);

/* 设置抓图文件当前页(设置当前需要显示哪一页) */
int CIU_SetConfigure_SnapshotFileCurrentPage(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, const BYTE * pBuffer, DWORD dwBufLen);

/* 获取抓图文件列表 */
int CIU_GetConfigure_SnapshotFileList(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen);

/* 设置抓图文件列表(无效) */
int CIU_SetConfigure_SnapshotFileList(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, const BYTE * pBuffer, DWORD dwBufLen);

/* 获取定时抓图配置 */
int CIU_GetConfigure_TimedCaptureCfg(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen);

/* 设置定时抓图配置 */
int CIU_SetConfigure_TimedCaptureCfg(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, const BYTE * pBuffer, DWORD dwBufLen);

//!\brief   远程配置，获取系统时间
int CIU_GetConfigure_Time(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen);

//!\brief   远程配置，设置系统时间
int CIU_SetConfigure_Time(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, const BYTE * pBuffer, DWORD dwBufLen);

//!\brief   远程配置，获取云台参数
int CIU_GetConfigure_Ptz(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen);

//!\brief   远程配置，设置云台参数
int CIU_SetConfigure_Ptz(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, const BYTE * pBuffer, DWORD dwBufLen);

//!\brief   远程配置，获取串口参数
int CIU_GetConfigure_Serial(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen);

//!\brief   远程配置，设置串口参数
int CIU_SetConfigure_Serial(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, const BYTE * pBuffer, DWORD dwBufLen);

//!\brief   远程配置，获取报警输入、输出及异常参数
int CIU_GetConfigure_Alarm(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen);

//!\brief   远程配置，获取报警输入、输出及异常参数
int CIU_GetConfigure_AlarmEx(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen);

int CIU_GetConfigure_AlarmExV2(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen);

//!\brief   远程配置，设置报警输入、输出及异常参数
int CIU_SetConfigure_Alarm(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, const BYTE * pBuffer, DWORD dwBufLen);

//!\brief   远程配置，获取显示输出参数
int CIU_GetConfigure_VideoOut(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen);

//!\brief   远程配置，设置显示输出参数
int CIU_SetConfigure_VideoOut(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, const BYTE * pBuffer, DWORD dwBufLen);

//!\brief   远程配置，用户管理信息
int CIU_GetConfigure_User(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen);

//!\brief   远程配置，用户管理信息
int CIU_SetConfigure_User(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, const BYTE * pBuffer, DWORD dwBufLen);

//!\brief   远程配置，获取系统信息
int CIU_GetConfigure_DevInfo(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen);

//!\brief   远程配置，设置系统信息
int CIU_SetConfigure_DevInfo(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, const BYTE * pBuffer, DWORD dwBufLen);

//!\brief   远程配置，获取系统配置
int CIU_GetConfigure_DevConfig(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen);

//!\brief   远程配置，设置系统配置
int CIU_SetConfigure_DevConfig(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, const BYTE * pBuffer, DWORD dwBufLen);

//!\brief   远程配置，获取存储管理信息
int CIU_GetConfigure_Storage(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen);

//!\brief   远程配置，设置存储管理信息
int CIU_SetConfigure_Storage(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, const BYTE * pBuffer, DWORD dwBufLen);

//!\brief   远程配置，获取系统状态
int CIU_GetConfigure_DevState(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen);

//!\brief   远程配置，获取设备自动维护配置信息
int CIU_GetConfigure_DevMaintenance(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen);

//!\brief   远程配置，设置设备自动维护配置信息
int CIU_SetConfigure_DevMaintenance(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, const BYTE * pBuffer, DWORD dwBufLen);

//!\brief   远程配置，获取设备功能定制配置信息
int CIU_GetConfigure_DevCustom(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen);

//!\brief   远程配置，设置设备功能定制配置信息

//!\brief   获取buf信息
int CIU_GetConfigure_BUF_SIZE(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD* dwBufLen);

//!\brief   获取P2P连接能力
int CIU_GetConfigure_Connect_Resource(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen);

int CIU_GetDeviceCapacity(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen);

int CIU_GetDeviceCapacity_V2(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen);

//!\brief   远程配置，获取互信互通參數
int CIU_GetConfigure_Hxht(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen);

//!\brief   远程配置，设置互信互通參數
int CIU_SetConfigure_Hxht(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, const BYTE * pBuffer, DWORD dwBufLen);

//!\brief   远程配置，设置ISP参数
int CIU_GetConfigure_ISP(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen);

//!\brief   远程配置，获取ISP参数
int CIU_GetConfigure_IPC_ISP(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen);

//!\brief   NVR远程配置，获取ISP参数
int CIU_GetConfigure_NVR_ISP(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen);

//!\brief   远程配置，设置ISP参数
int CIU_SetConfigure_ISP(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, const BYTE * pBuffer, DWORD dwBufLen);

//!\brief   NVR远程配置，设置ISP参数
int CIU_SetConfigure_NVR_ISP(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, const BYTE * pBuffer, DWORD dwBufLen);

//!\brief   远程配置，设置ISP参数
int CIU_SetConfigure_IPC_ISP(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, const BYTE * pBuffer, DWORD dwBufLen);

//!\breif   获取IRCUT阈值
int CIU_GetConfigure_IR_CUT(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen);

int CIU_GetConfigure_tone_ISP(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen);

int CIU_SetConfigure_tone_ISP(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, const BYTE * pBuffer, DWORD dwBufLen);


//!\brief   设置IRCUT阈值 
int CIU_SetConfigure_IR_CUT(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, const BYTE * pBuffer, DWORD dwBufLen);

//!\breif   获取ROI信息
int CIU_GetConfigure_ROI(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, const BYTE * pBuffer, DWORD * pdwBufLen);

//!\brief   设置ROI信息
int CIU_SetConfigure_ROI(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, const BYTE * pBuffer, DWORD dwBufLen);

//!\brief   获取软件能力信息
int CIU_GetConfigure_SOFT_POWER(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, const BYTE * pBuffer, DWORD * pdwBufLen);

//!\brief   远程配置，获取中心管理服务器通讯协议
int CIU_GetConfigure_Protocol(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen);

int CIU_SetConfigure_RTMP_PUSH(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, const BYTE * pBuffer, DWORD  dwBufLen);
int CIU_GetConfigure_RTMP_PUSH(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen);

int CIU_GetConfigure_DeviceCustomFunction(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen);

int CIU_SetConfigure_DeviceCustomFunction(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, const BYTE * pBuffer, DWORD dwBufLen);

//获取通道当前缩略图
int CIU_GetThumbnail(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen);

int CIU_GetConfigure_AUTH_INFO(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen);

int CIU_SetConfigure_AUTH_INFO(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, const BYTE * pBuffer, DWORD  dwBufLen);

//获取CIU模块错误信息
int CIU_GetDeviceErrorInfo(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen);

int CIU_GetExtentOSD(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen);

int CIU_SetExtentOSD(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, const BYTE * pBuffer, DWORD  dwBufLen);

int CIU_SetConfigure_PRODUCE_CUSTOM_INFO(char *pCustomFile, char *pDstFile);

int CIU_Update_RealStream_Connect_INFO(int nCh, int StreamType, int action);

int CIU_GetOSDV2(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen);

int CIU_SetOSDV2(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, const BYTE * pBuffer, DWORD  dwBufLen);

int CIU_SetMessageNotify(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, const BYTE * pBuffer, DWORD  dwBufLen);

//!\brief       初始化透明通道
//!\return  0表示成功，其它为错误码
int CIU_TransparentChanne_Init();

//!\brief 初始化远程回放
int CIU_Playback_init();

//!\brief 初始化远程图片管理
int CIU_PictureManager_init();

//!\brief 初始化远程命令
int CIU_RemoteCommand_init();

//!\brief   重启网络服务
//!\param   nSecond 延迟时间
int CIU_ResetServer(int nSecond);

//!\brief       设置远程控制回调函数
//!\param   RemoteControl_Callback 回调函数
//!\param   unsigned long dwUserID 用户ID
//!\param   unsigned long dwOperation 1-重启,2-关机,3-待机
//!\return  0表示设置成功
int CIU_SetRemoteControlCallback(RemoteControl_Callback pRemoteControlCallBack);

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
int CIU_NotifyRemoteControl(unsigned long dwUserID, eRemoteControlNotify eOperation, unsigned long dwParam1 = 0);

int CIU_GetDataSize(DWORD dwChanMask, DWORD dwDiskGroupMask, DWORD dwRecTypeMask, DWORD tStartTime, DWORD tEndTime, unsigned long long *dwSize);

void CIU_SetStandardChangeCallback(int (*pStandardChangeCallBack) (int newStandard));

int CIU_SetConfigure_AUDIO_ALARM_ADVANCE_CONFIG(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, const BYTE * pBuffer, DWORD dwBufLen);

int CIU_GetConfigure_AUDIO_ALARM_ADVANCE_CONFIG(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen);

int CIU_SetConfigure_FaceCap(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, const BYTE * pBuffer, DWORD dwBufLen);

int CIU_GetConfigure_FaceCap(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen);

int CIU_SetConfigure_FaceRecoRuleList(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, const BYTE * pBuffer, DWORD dwBufLen);

int CIU_GetConfigure_FaceRecoRuleList(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen);

//获取流传输方式
int CIU_GetConfigure_RealStreamBuffer(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen);

int CIU_SetConfigure_P6smsgPolicy(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, const BYTE * pBuffer, DWORD  dwBufLen);

int CIU_GetConfigure_VoiceBroadcastLanguage(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen);

int CIU_SetConfigure_VoiceBroadcastLanguage(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, const BYTE * pBuffer, DWORD  dwBufLen);

#ifdef __SUPPORT_P2P_MAXCLIENT__
int CIU_GetConfigure_StreamOutOfRange(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen);
#endif

#ifdef __MODULE_4G_SUPPORT__

//!\brief  设置卡选择配置 
int CIU_SetConfigure_4GCardSelect(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, const BYTE * pBuffer, DWORD dwBufLen);

//!\brief   远程配置，获取选择配置 
int CIU_GetConfigure_4GCardSelect(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen);

//!\brief	远程配置，获取4G卡信息 
int CIU_GetConfigure_4GCardInfo(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen);

//!\brief	远程配置，获取双sim卡信息 
int CIU_GetConfigure_SimIndexCardInfo(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen);


int CIU_GetConfigure_FL(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen);

#endif

#if defined (__IS_SUPPORT_P6SMESSAGE__)
int CIU_GetConfigure_ERROR_CODE(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen);

int CIU_GetConfigure_ERROR_CODE_SERVER_INFO(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen);

int CIU_SetConfigure_P6S_MP_SERVER_PARAM(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, const BYTE * pBuffer, DWORD dwBufLen);

int CIU_GetConfigure_P6S_MP_SERVER_PARAM(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen);
#endif

int CIU_GetConfigure_DeviceIntercomMode(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, const BYTE * pBuffer, DWORD * dwBufLen);
int CIU_SetConfigure_DeviceIntercomMode(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, const BYTE * pBuffer, DWORD dwBufLen);

int CIU_GetConfigure_NIGHT_LED_INFO(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen);
int CIU_SetConfigure_NIGHT_LED_INFO(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, const BYTE * pBuffer, DWORD pdwBufLen);

int CIU_GetConfigure_DEVICE_SLEEP_INFO(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen);
int CIU_SetConfigure_DEVICE_SLEEP_INFO(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, const BYTE * pBuffer, DWORD dwBufLen);
int CIU_GetConfigure_DEVICE_SLEEP_CONTROL(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen);
int CIU_SetConfigure_DEVICE_SLEEP_CONTROL(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, const BYTE * pBuffer, DWORD dwBufLen);

#ifdef __IS_SUPPORT_CLOUDWORLD__
/* 获取设备云存储 模式*/
int CIU_GetConfigure_CloudStorageMode(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen);

int CIU_GetConfigure_DeviceDragDigitalMultiple(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen);
int CIU_SetConfigure_DeviceDragDigitalMultiple(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, const BYTE * pBuffer, DWORD dwBufLen);


/* 设置设备云存储 模式*/
int CIU_SetConfigure_CloudStorageMode(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, const BYTE * pBuffer, DWORD dwBufLen);
/* 设置帧同步 */
int CIU_GetConfigure_CurrentFrame_Param(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen);
#endif

/* 获取设备云存储 信息*/
int CIU_GetConfigure_CloudStorage(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen);
/* 设置设备云存储 信息*/
int CIU_SetConfigure_CloudStorage(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, const BYTE * pBuffer, DWORD dwBufLen);


int CIU_SetConfigure_CryDetect(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, const BYTE * pBuffer, DWORD  dwBufLen);
int CIU_GetConfigure_CryDetect(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen);

int CIU_GetConfigure_TcpRelay(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen);
int CIU_SetConfigure_TcpRelay(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, const BYTE *  pBuffer, DWORD pdwBufLen);

#ifdef __ONE_CLICK_CALL__
int CIU_SetConfigure_DeviceOneButtonCallCfg(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, const BYTE * pBuffer, DWORD dwBufLen);
#endif

#if defined (__IS_SUPPORT_4G_CLOUD_DOMAIN__)
int CIU_SetConfigure_AddDomain(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, const BYTE * pBuffer, DWORD  dwBufLen);
#endif

int CIU_SetConfigure_P2pCommand(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, const BYTE * pBuffer, DWORD dwBufLen);

#if defined (__IS_SUPPORT_REMINDER_SCHEDULE__)
int CIU_GetRemindSchedule(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen);
int CIU_SetRemindSchedule(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, const BYTE * pBuffer, DWORD pdwBufLen);
#endif

#if defined (__IS_SUPPORT_LCD_MANAGER__)
int CIU_GetConfigure_ScreenManagent(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen);
int CIU_SetConfigure_ScreenManagent(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, const BYTE * pBuffer, DWORD pdwBufLen);
#endif

extern "C" int mysystem(const char *cmdstring);

#endif							//_INC_MPU_CIU_GLOBAL_H_
