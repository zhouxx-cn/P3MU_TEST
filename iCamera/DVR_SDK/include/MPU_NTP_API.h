
/*!
 *\file		MPU_NTP_API.h
 *\brief		MPU模块，NTP服务器管理相关接口定义
 *\author	林云
 *\date		2009/03/20

功能：
-#	NTP服务器域名、端口、自动更新时间等参数配置管理
-#	创建定时器，自动与NTP服务器同步
 */

#ifndef _INC_MPU_NTP_API_H_
#define _INC_MPU_NTP_API_H_

//!\brief      获取NTP服务器的工作模式
//!\return   0为普通工作模式，1为高级工作模式，失败为-1
int MPU_NTP_GetWorkMode();

//!\brief       设置NTP服务器
//!\param   nWorkMode   0为普通工作模式，1为高级工作模式，失败为-1
//!\return  成功为0，失败为-1
int MPU_NTP_SetWorkMode(int nWorkMode);

//!\brief   获取是否启用NTP服务器
//!\return  TRUE表示启用，FALSE表示不启用
BOOL MPU_NTP_GetEnable();

//!\brief   设置是否启用NTP服务器
//!\param   bEnable     TRUE表示启用，FALSE表示不启用
//!\return  0表示成功，其它为错误码
int MPU_NTP_SetEnable(BOOL bEnable);

//!\brief   获取NTP服务器域名
//!\param   pszDomainName   返回NTP服务器域名
//!\return  本接口拷贝NTP服务器域名到pszDomainName指向的buf，并返回pszDomainName
char *MPU_NTP_GetServerDomainName(char *pszDomainName);

//!\brief   设置NTP服务器域名
//!\param   pszDomainName   NTP服务器域名
//!\return  0表示成功，其它为错误码
int MPU_NTP_SetServerDomainName(const char *pszDomainName);

//!\brief   获取同步NTP服务器间隔时间
//!\return  间隔时间，单位为秒
int MPU_NTP_GetAutoUpdateTime();

//!\brief   设置同步NTP服务器间隔时间
//!\param   nIntervalSecond     间隔时间，单位为秒
//!\return  0表示成功，其它为错误码
int MPU_NTP_SetAutoUpdateTime(int nIntervalSecond);

#endif							//_INC_MPU_NTP_API_H_
