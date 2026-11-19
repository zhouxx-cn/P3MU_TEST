// Copyright (C) 1991 - 1999 Rational Software Corporation

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_INTP_49B631A703B5_INCLUDED
#define _INC_INTP_49B631A703B5_INCLUDED

#include <pthread.h>
#include <sys/socket.h>
#include "global_constant.h"

class CConfig;

//!\brief   Net模块     NTP服务器设置接口类
//##ModelId=49B634E7020E
class INTP
{

  public:
	//!\brief   获取唯一实例
	//!\return  实例指针
	//##ModelId=49BDD33C00CE
	static INTP *Instance();

	//!\brief   销毁实例
	//##ModelId=49BDE50E018C
	static void Release();

	int Init();
	int UnInit();

	int LoadParam();

	//!\brief 设置模式 一般/高级
	//!\param nMode 0表示一般模式,1表示高级模式
	//!\return 0表示设置成功,-1表示失败
	//##ModelId=49B5E6CC0128
	int NTP_SetMode(int nMode);

	//!\brief 获取模式 
	//!\return 0表示一般模式，1表示高级模式,-1表示失败
	//##ModelId=49B5E6CC0128
	int NTP_GetMode();

	//!\brief   获取是否启用NTP服务器
	//!\return  true表示启用，false表示不启用
	//##ModelId=49B634E70211
	int NTP_GetEnable();

	//!\brief   设置是否启用NTP服务器
	//!\param   bEnable     true表示启用，false表示不启用
	//!\return  0表示成功，其它为错误码
	//##ModelId=49B634E70212
	int NTP_SetEnable(int bEnable);

	//!\brief   获取NTP服务器域名
	//!\param   pszDomainName   返回NTP服务器域名
	//!\return  本接口拷贝NTP服务器域名到pszDomainName指向的buf，并返回pszDomainName
	//##ModelId=49B634E70213
	char *NTP_GetServerDomainName(char *pszDomainName);

	//!\brief   设置NTP服务器域名
	//!\param   pszDomainName   NTP服务器域名
	//!\return  0表示成功，其它为错误码
	//##ModelId=49B634E70214
	int NTP_SetServerDomainName(const char *pszDomainName);

	//!\brief   获取同步NTP服务器间隔时间
	//!\return  间隔时间，单位为秒
	//##ModelId=49B634E70215
	int NTP_GetAutoUpdateTime();

	//!\brief   设置同步NTP服务器间隔时间
	//!\param   nIntervalSecond     间隔时间，单位为秒
	//!\return  0表示成功，其它为错误码
	//##ModelId=49B634E70216
	int NTP_SetAutoUpdateTime(int nIntervalSecond);
	
	int NTP_Start();

	int NTP_Stop();

  	int NTP_CheckStatus();

  protected:
	//##ModelId=49B634E7020F
	  INTP();

	//##ModelId=49B634E70210
	 ~INTP();

  private:
	//##ModelId=49BDD33C009D
	static INTP *instance;
	
	//使能
	int m_bNTPEnable;

	//NTP状态 0表示已经停止，1表示正在启动，2表示已经启动，3表示正在停止
	int m_iNTP_status;

	//开始标志
	int m_bStart;

	//停止标志
	int m_bStop;

	//模式
	int m_bMode;

	//时间服务器
	char m_bDomainName[NTP_MAX_DOMAINNAME_LEN];

	//更新时间间隔
	int m_bIntervalSec;


	//命令缓冲区
	char m_sCmdString[300];
	
	//ntp时间更新线程
	pthread_t m_ntpUpdateThread;
	
	//更新线程是否退出
	int m_bCheck_exit;


	//配置文件
	CConfig *m_pCConfig;
};

#endif							/* _INC_INTP_49B631A703B5_INCLUDED */
