
/*!
 *\file		MPU_PPPoE_API.h
 *\brief	MPU模块，PPPoE参数管理相关接口定义
 *\author	林云
 *\date		2009/03/20

功能：
-#	PPPoE参数配置（使能、用户名、密码）
-#	PPPoE网络参数获取（IP地址、子网掩码、网关地址）
-#	手动拨号、挂断
-#	PPPoE状态获取
 */


#ifndef _INC_MPU_PPPoE_API_H_
#define _INC_MPU_PPPoE_API_H_


#include "MSG.h"


//!\brief   获取PPPoE是否启用
//!\return  TRUE表示启用，FALSE表示禁用
BOOL MPU_PPPoE_GetEnable();

//!\brief   设置PPPoE是否启用
//!\param   bEnable     TRUE表示启用，FALSE表示禁用
//!\return  0表示成功，其它为错误码
int MPU_PPPoE_SetEnable(BOOL bEnable);

//!\brief   获取用户名
//!\param   pszUserName     返回用户名
//!\return  本接口拷贝用户名到pszUserName指向的buf，并返回pszUserName
char *MPU_PPPoE_GetUserName(char *pszUserName);

//!\brief   设置用户名
//!\param   pszUserName     用户名
//!\return  0表示成功，其它为错误码
int MPU_PPPoE_SetUserName(const char *pszUserName);

//!\brief   获取密码
//!\param   pszPassword     返回密码
//!\return  本接口拷贝密码到pszPassword指向的buf，并返回pszPassword
char *MPU_PPPoE_GetPassword(char *pszPassword);

//!\brief   设置密码
//!\param   pszPassword     密码
//!\return  0表示成功，其它为错误码
int MPU_PPPoE_SetPassword(const char *pszPassword);

//!\brief   启动PPPoE连接
//!\return  返回0表示开始连接，其它为错误码
//!         PPPoE连接是一个异步过程，
//!         上层需调用MPU_PPPoE_IsConnected获取状态
int MPU_PPPoE_Connect();

//!\brief   挂断PPPoE连接
//!\return  0表示开始挂断成功，其它为错误码
//!         PPPoE连接是一个异步过程，
//!         上层需调用MPU_PPPoE_IsConnected获取状态
int MPU_PPPoE_Disconnect();

//!\brief   获取PPPoE连接状态
//!\return  0表示没有连接，1表示正在连接，2表示连接成功，3表示正在挂断
int MPU_PPPoE_IsConnected();

//!\brief   获取IP地址
//!\param   pszIPAddress    返回IP地址（例 172.1.10.2）
//!\return  本接口拷贝IP地址到pszIPAddress指向的buf，并返回pszIPAddress
char *MPU_PPPoE_GetIPAddress(char *pszIPAddress);

//!\brief   获取子网掩码地址
//!\param   pszSubnetMask   返回子网掩码（例 255.255.0.0）
//!\return  本接口拷贝子网掩码到pszSubnetMask指向的buf，并返回pszSubnetMask
char *MPU_PPPoE_GetSubnetMask(char *pszSubnetMask);

//!\brief   获取网关地址
//!\param   pszGateway      返回网关地址（例 172.1.10.1）
//!\return  本接口拷贝网关地址到pszGateway指向的buf，并返回pszGateway
char *MPU_PPPoE_GetGateway(char *pszGateway);

//!\brief   设置PPPoE状态通知回调
//!\param   pCallback           回调函数指针
//!\return  0表示成功，其它为错误码
int MPU_PPPoE_SetNotifyCallback(MSG_CALLBACK pCallback);

//!\brief   设置DNS地址
//!\param   char * PrimaryDNS, char * SecondaryDNS  主备DNS地址
//!         PrimaryDNS!=NULL,SecondaryDNS可以为空
//!\return  0成功，其它失败
//##ModelId=49BB403B030E
int MPU_PPPoE_SetDNS(char *PrimaryDNS, char *SecondaryDNS);

int MPU_PPPoE_SetAutoObtainDNS(unsigned long autoObtainDNS);

int MPU_PPPoE_GetAutoObtainDNS(unsigned long *autoObtainDNS);

//!\brief   获取DNS地址
//!\param   char * PrimaryDNS, char * SecondaryDNS      返回DNS地址 
//!         PrimaryDNS!=NULL,SecondaryDNS可以为空
//!\return  0成功，其它失败
//##ModelId=49BB403B030E
int MPU_PPPoE_GetDNS(char *PrimaryDNS, char *SecondaryDNS);


#endif							/* _INC_MPU_PPPoE_API_H_ */
