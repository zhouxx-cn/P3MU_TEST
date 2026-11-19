
/*!
 *\file		MPU_CIU_API.h
 *\brief	MPU模块，管理远程访问相关接口定义
 *\author	林云
 *\date		2009/03/20

功能：
-#	远程服务端口配置
-#	获取在线网络客户端数量、信息（用户、IP地址、物理地址等）
-#	管理网络客户连接（断开连接、禁止IP登录
-#	远程升级（显示进度、取消升级）
-#	响应远程重启、关机命令

说明：
-#	本文件只定义设备本地GUI需要干预的相关部分。
-#	对应网络客户端访问设备，主要功能如远程预览、远程回放、远程参数配置等，由底层自动完成。
 */

#ifndef _INC_MPU_CIU_API_H_
#define _INC_MPU_CIU_API_H_

#include "tagRemoteClientInfo.h"
#include "MPU/MPU_CIU_Login.h"
#include "MPU/MPU_CIU_Preview.h"
#include "MPU/MPU_CIU_Upload.h"
#include "HANDLE.h"


/*!
*  @enum		eRemoteControlNotify
*  @brief	远程设备控制
*/
enum eRemoteControlNotify
{
	eRemoteControlBegin = 0,	/*!< 起始值                                 */
	eRemoteReboot,				/*!< 重启动                                 */
	eRemoteHalt,				/*!< 关机                                   */
	eRemoteStandby,				/*!< 待机                                   */
	eRemoteSetDefault,			/*!< 恢复系统默认                */
	eRemoteSendTestEmail,		/*!< 发送测试邮件                */
	eRemoteUpdateConfig = 10,	/*!< 导入参数                           */
	eRemoteSetVideoInAlias,		/*!< 预览通道别名变更   */
	eRemoteSetAlarmInAlias,		/*!< 报警输入通道别名变更   */
	eRemoteSetAlarmOutAlias,	/*!< 报警输出通道别名变更   */
	eRemoteSetDevAlias,			/*!< 设备通道别名变更   */
	eRemoteSetUserRight,		/*!< 用户权限变更   */
	eRemoteRemoveUser,			/*!< 删除用户 */
	eRemoteClearAlarm,			/*!< 报警清除 */
	eRemoteSetTimeFormat,		/*!< 变更语言，日期时间格式 */
	eRemoteMoveVideoInAlias,	/*!< 变更预览OSD坐标 */
	eRemoteSetStorageMode,		/*!< 变更高级存储模式,参数表示新的模式 */
	eRemoteSetHCResMode,		/*!< 变更系统资源配置,参数1表示需要重启 */
	eRemoteSendTestFtp =27,
	eRemoteRestoreFactory,      /*!< 恢复出厂设置28                           */
};

// 远程控制的回调函数定义
//!param    unsigned long dwUserID      用户ID
//!param    unsigned long dwOperation   
//  10-导入参数
//  11-视频预览通道别名变更
//  12-报警输入通道别名变更
//  13-报警输出通道别名变更
//  14-设备别名变更
//  15-用户权限变更
//  16- 删除用户  
//  17-  报警清除 
//  18-  变更日期时间格式 
//  19- 变更预览OSD坐标 
typedef int (*RemoteControl_Callback) (unsigned long dwUserID, eRemoteControlNotify eOperation, unsigned long dwParam1);

//!\brief   获取数据端口
//!\return  端口号
unsigned short MPU_CIU_GetMediaPort();

//!\brief   设置数据端口
//!\param   nPort       端口号
//!\return  0表示成功，其它为错误码
int MPU_CIU_SetMediaPort(unsigned short nPort);

//!\brief   获取命令端口
//!\return  端口号
unsigned short MPU_CIU_GetCommandPort();

//!\brief   设置命令端口
//!\param   nPort       端口号
//!\return  0表示成功，其它为错误码
int MPU_CIU_SetCommandPort(unsigned short nPort);

//!\brief   获取rtp端口
//!\return  端口号
unsigned short MPU_CIU_GetRtpPort();

//!\brief   设置rtp端口
//!\param   nPort       端口号
//!\return  0表示成功，其它为错误码
int MPU_CIU_SetRtpPort(unsigned short nPort, int *needReboot);

//!\brief   获取mobile端口
//!\return  端口号
unsigned short MPU_CIU_GetMobilePort();

//!\brief       设置Mobile端口
//!\param   nPort       端口号
//!\return  0表示成功，其它为错误码
int MPU_CIU_SetMobilePort(unsigned short nPort);

//!\brief   获取AutoDetect端口
//!\return  端口号
unsigned short MPU_CIU_GetAutoDetectPort();

//!\brief       设置AutoDetect端口
//!\param   nPort       端口号
//!\return  0表示成功，其它为错误码
int MPU_CIU_SetAutoDetectPort(unsigned short nPort);

//!\brief       获取连接的客户端数量
//!\return  客户端数量
int MPU_CIU_GetClientNum();

//!\brief   获取HK Private端口
//!\return  端口号
unsigned short MPU_CIU_GetHKPrivatePort();

//!\brief       设置HK Private端口
//!\param   nPort       端口号
//!\return  0表示成功，其它为错误码
int MPU_CIU_SetHKPrivatePort(unsigned short nPort);

//!\brief       获取指定的用户是否在线
//!\return  0 不在线，1在线
int MPU_CIU_IsOnline(HANDLE hUser);

//!\brief       获取连接的客户端信息
//!\param   pClientInfo   客户端信息
//!\return  0表示成功，其它为错误码
int MPU_CIU_GetClientInfo(PREMOTECLIENTINFO pClientInfo);

//!\brief       获取连接的客户端信息
//!\param  dwUserID   用户ID
//!\param  HANDLE *hUser 用户句柄地址
//!\param pcUerName   用户名
//!\param pcIPAddress   ip 地址
//!\param pcLoginTime  登陆时间
//!\return  0表示成功，其它为错误码
int MPU_CIU_GetRemoteByID(unsigned long dwUserID, HANDLE * phUser, char *pcUerName, int userNameSize, char *pcIPAddress,  int ipAddrSize, char *pcLoginTime, int loginTimeSize);

//!\brief       判断 远程是否具有权限
//!\param dwUserID   远程用户ID
//!\param eAction   操作
//!\return  0表示无权限，1表示有权限
int MPU_CIU_CheckRemoteUser(unsigned long dwUserID, REMOTE_ACTION eAction);

//!\brief       判断 远程是否具有权限
//!\param dwUserID   远程用户ID
//!\param eAction   操作
//!\param nCh   通道
//!\return  0表示无权限，1表示有权限
int MPU_CIU_CheckRemoteUser(unsigned long dwUserID, CHANNEL_ACTION eAction, int nCh);

//!\brief       断开指定客户端
//!\param   nClientID   客户端ID
//!\return  0表示成功，其它为错误码
int MPU_CIU_DisconnectClient(int nClientID);

//!\brief  获取是否启用IP 权限管理
//!\param   
//!\return  0 不启用，1启用
int MPU_CIU_GetIPRightEnable();

//!\brief  设置是否启用IP 权限管理
//!\param    0 不启用，1启用
//!\return 
int MPU_CIU_SetIPRightEnable(int iIpRightEnable);

//!\brief  获取许可ip
//!\param   pdwIPEnableList ip数组的地址
//!\return  许可的ip的个数
int MPU_CIU_GetIPEnableList(unsigned long *pdwIPEnableList);

//!\brief  添加删除许可IP
//!\param   dwIpAddr ip地址
//!\param   iEnable 0删除， 1 添加
//!\return  0成功
int MPU_CIU_ChangeIPEnable(unsigned long dwIpAddr, int iEnable);

//!\brief  清除所有IP
//!\return  0成功
int MPU_CIU_ClearIPEnableList();

//!\brief       取消远程升级
//!\return  0表示成功，其它为错误码
int MPU_CIU_CancelUpdate();

//!\brief       设置回调函数    远程升级
//!\return  0表示成功，其它为错误码
int MPU_CIU_SetUpdateNotifyCallback(Upload_Callback pRemoteUploadCallBack);

//!\brief       设置语音对讲结束的回调函数
//!\param   pStopTalkCallBack 回调函数
//!\return  0表示设置成功
int MPU_CIU_SetTalkCallback(Talk_Callback pStopTalkCallBack);

//!\brief       设置远程控制回调函数
//!\param   RemoteControl_Callback 回调函数
//!\return  0表示设置成功
int MPU_CIU_SetRemoteControlCallback(RemoteControl_Callback pRemoteControlCallBack);

//!\brief       重启网络服务,为异步处理
//!\param   nSecond 延迟时间
//!\return  0表示设置成功
int MPU_CIU_ResetService(int nSecond);

//!\brief    工厂测试用 开启语音对讲
void MPU_CIU_StartLocalVoice(DWORD dwChannel);


//!\brief   工厂 测试用 停止语音对讲
void MPU_CIU_StopLocalVoice(DWORD dwChannel);

void MPU_CIU_SetStandardChangeCallback(int (*pStandardChangeCallBack) (int newStandard));

int  MPU_AVL_SetTrigRecordOverCallBack(int (*CallBack)(unsigned char *));

//获取云台配置信息，需外部将pcfg清零
LPHY_DVR_PTZ_ELCTRONIC MPU_CIU_GetPtzEletronicConfig();

#endif							//_INC_MPU_CIU_API_H_
