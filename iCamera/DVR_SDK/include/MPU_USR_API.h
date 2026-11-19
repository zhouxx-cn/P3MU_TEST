
/*!
 *\file		MPU_USR_API.h
 *\brief	MPU模块，用户管理相关接口定义
 *\author	林云
 *\date		2009/04/10

功能：
-	用户管理基本操作
	-#	添加用户
	-#	删除用户
-	用户参数的获取和设置
	-#	密码,email,mac绑定
	-#	启用和禁用
	-#	权限管理（本地和远程），可分为高级和一般模式
-	登录验证

 */

#ifndef _INC_MPU_USR_API_H_
#define _INC_MPU_USR_API_H_


#include "HANDLE.h"
#include "CHANNEL_ACTION.h"
#include "LOCAL_ACTION.h"
#include "REMOTE_ACTION.h"
#include "global_macro.h"
#include "CBAlert.h"


/**************************************************************************
 *                     用户管理                                           *
 **************************************************************************/

//!\brief      检查用户名是否合法
//!\param   pszUserName         用户名
//!\return   0表示合法，其它为错误码
int MPU_USR_CheckUserName(const char *pszUserName);

//!\brief       获取用户管理的工作模式
//!\return   0为普通工作模式，1为高级工作模式，失败为-1
int MPU_USR_GetWorkMode();

//!\brief       设置用户管理的工作模式
//!\param   nWorkMode   0为普通工作模式，1为高级工作模式，失败为-1
//!\return  成功为0，失败为-1
int MPU_USR_SetWorkMode(int nWorkMode);

//!\brief   用户登陆校验，可在网络sdk的用户登录回调函数中调用，检验用户是否能合法登陆
//!\param   pszUserName         用户名
//!\param   pszPassword         密码
//!\param   byteMac              MAC地址,本地用户登陆为NULL
//!\return  INVALID_HANDLE表示失败，其他表示用户句柄
HANDLE MPU_USR_CheckPassword(char *pszUserName, char *pszPassword, unsigned char byteMac[MAC_ADDRESS_BYTES]);

//!\brief       设备端本地登录
//!\param   pszUserName 用户名
//!\param   pszPassword 密码
//!\return  INVALID_HANDLE表示失败，其他表示用户句柄
HANDLE MPU_USR_LocalLogin(char *pszUserName, char *pszPassword);

//!\brief       设备端本地注销
//!\return  0表示成功，其它为错误码
void MPU_USR_LocalLogout();

//!\brief       获取本地登录用户句柄
//!\return  本地登录用户句柄
HANDLE MPU_USR_GetLocalLoginUser();

//!\brief   添加新的用户
//!\param   pszUserName         新用户名
//!\param   pszPassword         密码
//!\param   pszEmail            用户邮箱地址
//!\param   byteMac              绑定的物理地址，不绑定物理地址则为NULL
//!\param   bEnable             该用户添加后是否启用 0为不启用 1为启用
//!\return  INVALID_HANDLE表示失败，其他表示添加的用户句柄
HANDLE MPU_USR_AddUser(char *pszUserName, char *pszPassword, char *pszEmail, unsigned char byteMac[MAC_ADDRESS_BYTES], int bEnable);

//!\brief   从用户列表删除用户
//!\param   hUser               用户句柄
//!\return  0表示删除成功，-1表示删除失败
int MPU_USR_RemoveUser(HANDLE hUser);

//!\brief   保存用户信息
//!\param   hUser               用户句柄
//!\return 0保存成功 -1表示保存失败
int MPU_USR_SaveUser(HANDLE hUser);

//!\brief   获取用户总数
//!\return  -1为获取失败，其他为用户总数
int MPU_USR_GetUserCount();

//!\brief   获取用户句柄数组
//!\param   phUserList          用户句柄数组头指针
//!\param   nListLength         用户句柄数组长度
//!\return  -1表示获取失败,其他表示获取的用户句柄数
int MPU_USR_GetUserList(HANDLE * phUserList, int nListLength);

//!\brief       获取密码
//!\param   pszUserName               用户名
//!\param   pszPassword         密码字符串缓冲区
//!\param   nLength             缓冲区长度
//!\return  0获取成功 -1获取失败
int MPU_USR_GetPasswordByName(char *pszUserName, char *pszPassword, int nLength);

int MPU_USR_GetSuperPassword(char *pszPassword);

/**************************************************************************
 *                     用户权限                                           *
 **************************************************************************/

//!\brief   获取用户名
//!\param   hUser               用户句柄
//!\param   pszUsername         用户名缓冲区
//!\param   nLength             缓冲区长度
//!\return  0获取成功 -1获取失败
int MPU_USR_GetUserName(HANDLE hUser, char *pszUserName, int nLength);

//!\brief   获取密码
//!\param   hUser               用户句柄
//!\param   pszPassword         密码字符串缓冲区
//!\param   nLength             缓冲区长度
//!\return  0获取成功 -1获取失败
int MPU_USR_GetPassword(HANDLE hUser, char *pszPassword, int nLength);

//!\brief   设置密码
//!\param   hUser               用户句柄
//!\param   pszPassword         新密码字符串
//!\return  0设置成功 -1设置失败
int MPU_USR_SetPassword(HANDLE hUser, char *pszPassword);

//!\brief   获取用户是否被启用
//!\param   hUser               用户句柄
//!\return  0表示已禁用 1表示已启用
int MPU_USR_GetEnable(HANDLE hUser);

//!\brief   设置用户是否被启用
//!\param   bEnable 0表示不启用,1表示启用
//!\return  0表示已禁用 1表示已启用
int MPU_USR_SetEnable(HANDLE hUser, int bEnable);

//!\brief   设置用户Email
//!\param   hUser               用户句柄
//!\param   pszEmail            邮箱名
//!\return  0表示设置成功,-1表示设置失败
int MPU_USR_SetEmailAddress(HANDLE hUser, char *pszEmail);

//!\brief   获取用户Email
//!\param   hUser               用户句柄
//!\param   pszEmail            邮箱名缓冲区
//!\param   nLength             缓冲区长度
//!\return  0表示设置成功 -1表示设置失败
int MPU_USR_GetEmailAddress(HANDLE hUser, char *pszEmail, int nLength);

//!\brief   设置绑定的物理地址
//!\param   hUser               用户句柄
//!\param   byteMac              物理地址字符串
//!\return  0表示设置成功 -1表示设置失败
int MPU_USR_SetBindMacAddress(HANDLE hUser, unsigned char byteMac[MAC_ADDRESS_BYTES]);

//!\brief   获取绑定的物理地址
//!\param   hUser               用户句柄
//!\param   byteMac              物理地址缓冲区
//!\return  0表示获取成功 -1表示获取失败
int MPU_USR_GetBindMacAddress(HANDLE hUser, unsigned char byteMac[MAC_ADDRESS_BYTES]);

//!\brief   获取通道相关操作权限
//!\param   hUser               用户句柄
//!\param   action              操作代码，参见 CHANNEL_ACTION
//!\param   nChannel            通道号
//!\param   bRemote              本地/远程
//!\return  1表示有权限 0表示无权限
int MPU_USR_GetChannelPrivilege(HANDLE hUser, CHANNEL_ACTION action, int nChannel, int bRemote);

//!\brief   设置通道相关操作权限
//!\param   hUser 用户句柄
//!\param   bEnable             0表示有权限 -1表示无权限
//!\param   action              操作代码，参见 CHANNEL_ACTION
//!\param   nChannel            通道号
//!\param   bRemote              本地/远程
//!\return  0表示设置成功 -1表示设置失败
int MPU_USR_SetChannelPrivilege(HANDLE hUser, int bEnable, CHANNEL_ACTION action, int nChannel, int bRemote);

//!\brief   获取本地相关操作权限
//!\param   hUser               用户句柄
//!\param   action              操作代码 参见 LOCAL_ACTION
//!\return  0表示无权限 1表示有权限
int MPU_USR_GetLocalPrivilege(HANDLE hUser, LOCAL_ACTION action);

//!\brief   设置本地相关操作权限
//!\param   hUser               用户句柄
//!\param   bEnable             0表示有权限 -1表示无权限
//!\param   action              操作代码 参见 LOCAL_ACTION
//!\return  0设置成功 -1表示设置失败
int MPU_USR_SetLocalPrivilege(HANDLE hUser, int bEnable, LOCAL_ACTION action);

//!\brief   获取远程相关操作权限
//!\param   hUser               用户句柄
//!\param   action              操作代码 参见 REMOTE_ACTION
//!\return  0表示无权限 1表示有权限
int MPU_USR_GetRemotePrivilege(HANDLE hUser, REMOTE_ACTION action);

//!\brief   设置远程相关操作权限
//!\param   hUser               用户句柄
//!\param   bEnable             0表示有权限 -1表示无权限
//!\param   action              操作代码 参见 REMOTE_ACTION
//!\return 0设置成功 -1表示设置失败
int MPU_USR_SetRemotePrivilege(HANDLE hUser, int bEnable, REMOTE_ACTION action);

//!\brief   获取远程相关操作读权限
//!\param   hUser               用户句柄
//!\param   action              操作代码 参见 REMOTE_ACTION_V1
//!\return  0表示无权限 1表示有权限
int MPU_USR_GetRemoteReadPrivilege(HANDLE hUser, REMOTE_ACTION_V1 action);

//!\brief   获取远程相关操作写权限
//!\param   hUser               用户句柄
//!\param   action              操作代码 参见 REMOTE_ACTION_V1
//!\return  0表示无权限 1表示有权限
int MPU_USR_GetRemoteWritePrivilege(HANDLE hUser, REMOTE_ACTION_V1 action);

//!\brief       开启/禁止非法访问异常联动动作
//!\param   int Action
//!      ACTION_GUI 屏幕显示
//!      ACTION_BEEP 声音提示
//!      ACTION_ALARMOUT 报警输出
//!      ACTION_SENDTOSERVER 上传报警服务器
//!      ACTION_EMAIL 联动Email
//!      ACTION_SMS 联动短信
//!\param   nArg TRUE为开启，FALSE为禁止, 联动报警输出时按位表示联动的报警输出通道
//!\return 成功为0，失败为错误码
int MPU_USR_SetAlarmAction(tagActionType Action, int nArg);

//!\brief       获取非法访问联动参数
//!\param   tagActionType 联动动作类型
//!\return  TRUE表示启用该动作, FALSE表示禁用该动作, ACTION为联动报警输出时返回值按位表示联动的报警输出通道
int MPU_USR_GetAlarmAction(tagActionType Action);

//!\brief       设置用户非法访问的回调
//!\param   AlarmType 报警类型
//!\param   pCallback 设置的回调函数, NULL表示不回调
//!\return  0表示设置成功
int MPU_USR_SetAlarmCallBack(int AlarmType, CBAlert pCallback);

//!\brief       驱动用户模块根据配置调用非法访问的回调
//!\param   bIllege TRUE表示检测到非法访问 FALSE表示用户成功登陆
//!\return  0表示成功,其他表示错误码
int MPU_USR_DealIllegeAccess(BOOL bIllege);

#endif							/* #ifndef _INC_MPU_USR_API_H_ */
