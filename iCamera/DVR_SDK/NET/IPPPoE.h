// Copyright (C) 1991 - 1999 Rational Software Corporation

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_IPPPOE_49B5FFFD0088_INCLUDED
#define _INC_IPPPOE_49B5FFFD0088_INCLUDED
#include "MSG.h"
#include <pthread.h>
#include "global_macro.h"


#include "NET_global.h"



class CConfig;

//!\brief   Net模块     PPPoE设置接口类
//##ModelId=49BB403B02E9
class IPPPoE
{
  public:
	//!\brief   获取唯一实例
	//!\return  实例指针
	//##ModelId=49BDB4DC0163
	static IPPPoE *GetInstance();

	//!\brief   销毁实例
	//##ModelId=49BDB7CE0149
	static void Release();

	//!\brief 初始化
	int Init();

	//!\brief 释放资源
	int UnInit();


	//!\brief   获取PPPoE是否启用
	//!\return  true表示启用，false表示禁用
	//##ModelId=49BB403B02F5
	int PPPoE_GetEnable();

	//!\brief   设置PPPoE是否启用
	//!\param   bEnable     true表示启用，false表示禁用
	//!\return  0表示成功，其它为错误码
	//##ModelId=49BB403B02F6
	int PPPoE_SetEnable(int bEnable);

	//!\brief   获取用户名
	//!\param   pszUserName     返回用户名
	//!\return  本接口拷贝用户名到pszUserName指向的buf，并返回pszUserName
	//##ModelId=49BB403B02FD
	char *PPPoE_GetUserName(char *pszUserName);

	//!\brief   设置用户名
	//!\param   pszUserName     用户名
	//!\return  0表示成功，其它为错误码
	//##ModelId=49BB403B02FF
	int PPPoE_SetUserName(const char *pszUserName);

	//!\brief   获取密码
	//!\param   pszPassword     返回密码
	//!\return  本接口拷贝密码到pszPassword指向的buf，并返回pszPassword
	//##ModelId=49BB403B0301
	char *PPPoE_GetPassword(char *pszPassword);

	//!\brief   设置密码
	//!\param   pszPassword     密码
	//!\return  0表示成功，其它为错误码
	//##ModelId=49BB403B0303
	int PPPoE_SetPassword(const char *pszPassword);

	//!\brief   启动PPPoE连接
	//!\return  返回0表示开始连接，其它为错误码
	//!         PPPoE连接是一个异步过程，
	//!         上层需调用PPPoE_IsConnected获取状态
	//##ModelId=49BB403B0307
	int PPPoE_Connect();

	//!\brief   挂断PPPoE连接
	//!\return  0表示开始挂断成功，其它为错误码
	//!         PPPoE连接是一个异步过程，
	//!         上层需调用PPPoE_IsConnected获取状态
	//##ModelId=49BB403B0308
	int PPPoE_Disconnect();

	//!\brief   获取PPPoE连接状态
	//!\return  0表示没有连接，1表示正在连接，2表示连接成功，3表示正在挂断
	//##ModelId=49BB403B0309
	int PPPoE_IsConnected();

	//!\brief   获取IP地址
	//!\param   pszIPAddress    返回IP地址（例 172.1.10.2）
	//!\return  本接口拷贝IP地址到pszIPAddress指向的buf，并返回pszIPAddress
	//##ModelId=49BB403B030A
	char *PPPoE_GetIPAddress(char *pszIPAddress);

	//!\brief   获取子网掩码地址
	//!\param   pszSubnetMask   返回子网掩码（例 255.255.0.0）
	//!\return  本接口拷贝子网掩码到pszSubnetMask指向的buf，并返回pszSubnetMask
	//##ModelId=49BB403B030C
	char *PPPoE_GetSubnetMask(char *pszSubnetMask);

	//!\brief   获取网关地址
	//!\param   pszGateway      返回网关地址（例 172.1.10.1）
	//!\return  本接口拷贝网关地址到pszGateway指向的buf，并返回pszGateway
	//##ModelId=49BB403B030E
	char *PPPoE_GetGateway(char *pszGateway);

	//!\brief   设置DNS地址
	//!\param   char * PrimaryDNS, char * SecondaryDNS  主备DNS地址
	//!         PrimaryDNS!=NULL,SecondaryDNS可以为空
	//!\return  0成功，其它失败
	//##ModelId=49BB403B030E
	int PPPoE_SetDNS(char *PrimaryDNS, char *SecondaryDNS);


	//!\brief   设置PPPoE状态通知回调
	//!\param   pCallback           回调函数指针
	//!\return  0表示成功，其它为错误码
	//##ModelId=49BDC7D1019D
	int PPPoE_SetNotifyCallback(MSG_CALLBACK pCallback);
  public:
	//////////////////////////////////////////////////////////////////////////
	//以下接口，一般外部不要调用

	int PPPoE_SetAutoObtainDNS(unsigned long autoObtainDNS);

	int PPPoE_GetAutoObtainDNS(unsigned long *autoObtainDNS);
	
	//!\brief  重新激活 
	//!\return 0表示成功,其它为错误码
	int PPPoE_ReActive();

	//!brief 检测网络状态
	//!\return <-1-错误;-1-不检测;0-正常;1-断开;
	int PPPoE_CheckStatus();



  protected:
	//##ModelId=49BB403B02F3
	  IPPPoE();

	//##ModelId=49BB403B02F4
	 ~IPPPoE();

	//!\brief   网络状态通知回调函数
	//!\param   .nMsg-MSG_PPPOE_LOCAL_NAME
	//          .nParam1 （0表示Adsl断线，1表示Adsl连通）
	//!\return  0表示成功，其它为错误码
	//##ModelId=49BDC7D101E4
	MSG_CALLBACK m_pNotifyCallback;

  private:
	//##ModelId=49BDB4DC0132
	static IPPPoE *m_pInstance;
	//PPPOE使能
	int m_bIPPPOE_Enable;
	//连接操作标志,0-无操作,1-将连接
	int m_bConnOpt;
	//断开操作标志,0-无操作,1-将断开
	int m_bDisconnOpt;
	//连通状态
	int m_bConnected;
	//保存上次状态
	int m_bConnected_old;
	//用户名
	char m_sUserName[PPPOE_USERNAME_LEN];
	//密码
	char m_sPassword[PPPOE_PASSWORD_LEN];

	//主DNS地址
	char m_sPrimaryDNS[20];

	//第二DNS地址
	char m_sSecondaryDNS[20];

	//PPPOE-IP地址
	char m_sPPPOE_IP[20];
	//状态通知回调
	MSG_CALLBACK m_FxnCallBack;
	//配置文件
	CConfig *m_pCConfig;
	//状态检测线程
	pthread_t m_hCheckThread;
	//状态检测线程是否退出
	int m_bCheck_exit;

  	unsigned long m_autoObtainDNS;

  private:
	//导入缺省值
	int LoadParam();

	//!\brief 连接状态
	//!\return 1-连接，0断开
	int CheckConnect();

	//!\brief 检查IP是否有效
	//!\param ipaddr 待检查IP地址
	//!\return 0无效,1有效
	int IsValidIP(const char *ipaddr);



};

#endif							/* _INC_IPPPOE_49B5FFFD0088_INCLUDED */
