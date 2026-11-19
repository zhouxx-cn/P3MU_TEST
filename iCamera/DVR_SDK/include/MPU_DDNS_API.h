
/*!
 *\file		MPU_DDNS_API.h
 *\brief	MPU模块，DDNS服务相关接口定义
 *\author	林云
 *\date		2009/03/20

功能：
-#	DDNS服务能力选择（支持多种协议）
-#	DDNS服务配置（选择协议、使能、用户名、密码、域名）
-#	手动更新，自动保活
 */


#ifndef _INC_MPU_DDNS_API_H_
#define _INC_MPU_DDNS_API_H_


//!\brief   获取支持的协议总数
//!\return  协议总数
int MPU_DDNS_GetProtocolCount();

//!\brief   获取当前选择的协议编号
//!\return  协议编号[0 - (count-1)]
int MPU_DDNS_GetCurrentProtocol();

//!\brief   设置当前协议编号
//!\param   nIndex      协议编号[0 - (count-1)]
//!\return  0表示成功，其它为错误码
int MPU_DDNS_SetCurrentProtocol(int nIndex);

//!\brief   获取是否启用DDNS
//!\return  true表示启用，false表示禁用
bool MPU_DDNS_GetEnable();

//!\brief   设置是否启用DDNS
//!\param   bEnable     true表示启用，false表示禁用
//!\return  0表示成功，其它为错误码
int MPU_DDNS_SetEnable(bool bEnable);

//!\brief   获取DDNS协议名称
//!\param   nIndex          协议编号[0 - (count-1)]
//!\param   ProtocolName    返回协议名称
//!\return  本接口拷贝DDNS协议名称到pszProtocolName指向的buf，并返回pszProtocolName
char *MPU_DDNS_GetProtocolName(int nIndex, char *pszProtocolName);

//!\brief   获取DDNS用户名
//!\param   pszUserName 返回用户名
//!\return  本接口拷贝DDNS用户名到pszUserName指向的buf，并返回pszUserName
char *MPU_DDNS_GetUserName(int nIndex, char *pszUserName);

//!\brief   设置DDNS用户名
//!\param   pszUserName 用户名
//!\return  0表示成功，其它为错误码
int MPU_DDNS_SetUserName(int nIndex, const char *pszUserName);

//!\brief   获取DDNS密码
//!\param   pszPassword 返回密码
//!\return  本接口拷贝DDNS密码到pszPassword指向的buf，并返回pszPassword
char *MPU_DDNS_GetPassword(int nIndex, char *pszPassword);

//!\brief   设置DDNS密码
//!\param   pszPassword 密码
//!\return  0表示成功，其它为错误码
int MPU_DDNS_SetPassword(int nIndex, const char *pszPassword);

//!\brief   获取DDNS域名
//!\param   pszDomainName   返回域名
//!\return  
//本接口拷贝DDNS域名到pszDomainName指向的buf，并返回pszDom
//ainName
char *MPU_DDNS_GetDomainName(int nIndex, char *pszDomainName, int DomainNameSize);

//!\brief   设置DDNS域名
//!\param   pszDomainName   域名
//!\return  0表示成功，其它为错误码
int MPU_DDNS_SetDomainName(int nIndex, const char *pszDomainName);

//!\brief   获取DDNS保活时间
//!\return  间隔时间，单位为秒
int MPU_DDNS_GetAutoUpdateTime(int nIndex);

//!\brief   获取DDNS保活时间
//!\param   nIntervalSecond     间隔时间，单位为秒
//!\return  0表示成功，其它为错误码
int MPU_DDNS_SetAutoUpdateTime(int nIndex, int nIntervalSecond);

//!\brief   启动DDNS
//!\return  0表示开始启动，其它为错误码
//!         启动DDNS是一个异步过程，上层需调用MPU_DDNS_GetState获取状态
int MPU_DDNS_Start();

//!\brief   停止DDNS
//!\return  0表示开始停止，其它为错误码
//!         启动DDNS是一个异步过程，上层需调用MPU_DDNS_GetState获取状态
int MPU_DDNS_Stop();

//!\brief   获取DDNS状态
//!\return  0表示已经停止，1表示正在启动，2表示已经启动，3表示正在停止
int MPU_DDNS_GetState();


//!\brief   设置DDNS服务器名称
//!\return  0成功，1失败
int MPU_DDNS_SetDIYDDNSServer(char *pszUserName);

//!\brief   获取DDNS服务器名称
//!\param   pszUserName 返回服务器名称
//!\return  0成功其他失败
int MPU_DDNS_GetDIYDDNSServer(char *pszUserName, const int len);

#endif							/* _INC_MPU_DDNS_API_H_ */
