// Copyright (C) 1991 - 1999 Rational Software Corporation

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_IDDNS_49B607C000D5_INCLUDED
#define _INC_IDDNS_49B607C000D5_INCLUDED

#include <pthread.h>
#include <sys/socket.h>
#include "global_macro.h"
#include "NET_global.h"

class CConfig;

//!\brief   Net模块     DDNS设置接口类
//##ModelId=49B634E70253
class IDDNS
{
  public:



	//!\brief   获取唯一实例
	//!\return  实例指针
	//##ModelId=49BDB4E601B7
	static IDDNS *GetInstance();

	//!\brief   销毁实例
	//##ModelId=49BDB7CE01D5
	static void Release();


	int SetNetCard(int num);
	//!\brief 初始化
	int Init();

	//!\brief 释放资源
	int UnInit();

	//!\brief   获取支持的协议总数
	//!\return  协议总数
	//##ModelId=49B634E7025F
	int DDNS_GetProtocolCount();

	//!\brief   获取当前选择的协议编号
	//!\return  协议编号[0 - (count-1)],-1表示无协议
	//##ModelId=49B634E70261
	int DDNS_GetCurrentProtocol();

	//!\brief   设置当前协议编号
	//!\param   nIndex      协议编号[0 - (count-1)]
	//!\return  0表示成功，其它为错误码
	//##ModelId=49B634E70262
	int DDNS_SetCurrentProtocol(int nIndex);

	//!\brief   获取是否启用DDNS
	//!\return  true表示启用，false表示禁用
	//##ModelId=49B634E70263
	int DDNS_GetEnable();

	//!\brief   设置是否启用DDNS
	//!\param   bEnable     true表示启用，false表示禁用
	//!\return  0表示成功，其它为错误码
	//##ModelId=49B634E70264
	int DDNS_SetEnable(int bEnable);

	//!\brief   获取DDNS协议名称
	//!\param   nIndex          协议编号[0 - (count-1)]
	//!\param   ProtocolName    返回协议名称
	//!\return  本接口拷贝DDNS协议名称到pszProtocolName指向的buf，并返回pszProtocolName
	//##ModelId=49BDD28802CE
	char *DDNS_GetProtocolName(int nIndex, char *pszProtocolName);

	//!\brief   获取DDNS用户名
	//!\param   pszUserName 返回用户名
	//!\return  
	//本接口拷贝DDNS用户名到pszUserName指向的buf，并返回pszUse
	//rName
	//##ModelId=49B634E70265
	char *DDNS_GetUserName(int nIndex, char *pszUserName);

	//!\brief   设置DDNS用户名
	//!\param   pszUserName 用户名
	//!\return  0表示成功，其它为错误码
	//##ModelId=49B634E70267
	int DDNS_SetUserName(int nIndex, const char *pszUserName);

	//!\brief   获取DDNS密码
	//!\param   pszPassword 返回密码
	//!\return  
	//本接口拷贝DDNS密码到pszPassword指向的buf，并返回pszPassw
	//ord
	//##ModelId=49B634E70268
	char *DDNS_GetPassword(int nIndex, char *pszPassword);

	//!\brief   设置DDNS密码
	//!\param   pszPassword 密码
	//!\return  0表示成功，其它为错误码
	//##ModelId=49B634E70269
	int DDNS_SetPassword(int nIndex, const char *pszPassword);

	//!\brief   获取DDNS域名
	//!\param   pszDomainName   返回域名
	//!\return  
	//本接口拷贝DDNS域名到pszDomainName指向的buf，并返回pszDom
	//ainName
	//##ModelId=49B634E7026A
	char *DDNS_GetDomainName(int nIndex, char *pszDomainName, int DomainNameSize);

	//!\brief   设置DDNS域名
	//!\param   pszDomainName   域名
	//!\return  0表示成功，其它为错误码
	//##ModelId=49B634E7026B
	int DDNS_SetDomainName(int nIndex, const char *pszDomainName);

	//!\brief   获取DDNS保活时间
	//!\return  间隔时间，单位为秒
	//##ModelId=49B634E7026C
	int DDNS_GetAutoUpdateTime(int nIndex);

	//!\brief   获取DDNS保活时间
	//!\param   nIntervalSecond     间隔时间，单位为秒
	//!\return  0表示成功，其它为错误码
	//##ModelId=49B634E7026D
	int DDNS_SetAutoUpdateTime(int nIndex, int nIntervalSecond);

	//!\brief   启动DDNS
	//!\return  0表示开始启动，其它为错误码
	//!         启动DDNS是一个异步过程，上层需调用DDNS_GetState获取状态
	//##ModelId=49B634E7026E
	int DDNS_Start();

	//!\brief   停止DDNS
	//!\return  0表示开始停止，其它为错误码
	//!         启动DDNS是一个异步过程，上层需调用DDNS_GetState获取状态
	//##ModelId=49B634E7026F
	int DDNS_Stop();

	//!\brief   获取DDNS状态
	//!\return  0表示已经停止，1表示正在启动，2表示已经启动，3表示正在停止
	//##ModelId=49B634E70270
	int DDNS_GetState();

	//!brief 更新状态,内部使用,外部要获得状态调用DDNS_GetState()
	//!\return <-1-错误;-1-不检测;0表示已经停止，1表示正在启动，2表示已经启动，3表示正在停止

	int DDNS_CheckStatus();

	//!\brief   设置DDNS服务器名称
	//!\return  0成功,其他失败
	int SetDIYDDNSServer(const char *szName);

	//!\brief   获取DDNS服务器名称
	//!\return  0成功,其他失败
	int GetDIYDDNSServer(char *szName, const int len);
	char * DDNS_ReGetNetDevName();
	int DDNS_GetNetDevName(void);
	int DDNS_SetDevName(int num);
  protected:
	//##ModelId=49B634E7025D
	  IDDNS();

	//##ModelId=49B634E7025E
	 ~IDDNS();

public:
	
	static char m_NetDevName[16];
  private:
	//##ModelId=49BDB4E6017C
	static IDDNS *m_Instance;
	//使能
	int m_bDDNSEnable;
	//DDNS个数
	int m_iProtocolCount;
	//当前使用的协议索引
	int m_iIndex;

	//开始标志
	int m_bStart;

	//停止标志
	int m_bStop;

	//协议有效
	int m_bProtoValid;
#if 0
	//协议名
	char m_sProtoName[PROTOCOLNAMELEN];

	//服务器主机名
	char m_sSvrName[DOMAINNAMELEN];
#endif
	//DIY域名服务器域名
	char m_sServerName[DOMAINNAMELEN];

	//域名
	char m_sDomainName[DOMAINNAMELEN];

	//用户名
	char m_sUserName[DDNS_USERNAME_LEN];

	//密码
	char m_sPassword[DDNS_PASSWORD_LEN];


	//上次注册IP
	char m_sIp[32];

	//上次注册设备
	char m_sDev[32];


	//保活时间
	unsigned int m_uiUpdateTime;

	//命令缓冲
	char m_sCmdString[1024];

	//DDNS状态 0表示已经停止，1表示正在启动，2表示已经启动，3表示正在停止
	int m_iDDNS_status;

	//时间计数,为0时将做更新
	unsigned int m_iTimeCnt;

	//检测外网IP是否改变的时间（默认为60秒）
	unsigned int m_iTimeCheckIp;

	//配置文件
	CConfig *m_pCConfig;

	//更新线程
	pthread_t m_hCheckThread;

	//更新线程是否退出
	int m_bCheck_exit;



  private:
	//!\brief 导入缺省值
	int LoadParam();

	//!\brief 连接到指定主机/端口
	//!\return 0成功，其它错误
	int do_connect();


	int ResolveHost(const char *szinHostname, char *szoutIP, int outIPSize);



};

#endif							/* _INC_IDDNS_49B607C000D5_INCLUDED */
