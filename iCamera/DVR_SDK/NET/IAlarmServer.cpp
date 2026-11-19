// Copyright (C) 1991 - 1999 Rational Software Corporation

//#include "stdafx.h"
#include "IAlarmServer.h"

#include "NET_global.h"
#include "CConfig.h"


//##ModelId=49B634E702DF
IAlarmServer::IAlarmServer()
{
	//获取配置实例
	m_pCConfig = CConfig::GetInstance();
	if (NULL == m_pCConfig)
	{
		NET_ERR("[IAlarmServer:%s]NULL==m_pCConfig\n", __FUNCTION__);

		return;
	}
}

//##ModelId=49B634E702E0
IAlarmServer::~IAlarmServer()
{
	// TODO: Add your specialized code here.
}


//!\brief 设置模式 一般/高级
//!\param nMode 0表示一般模式,1表示高级模式
//!\return 0表示设置成功,-1表示失败
//##ModelId=49B5E6CC0128
int IAlarmServer::AMS_SetMode(int nMode)
{
	m_pCConfig->SetValue(IAlarmServer_CfgSection, "Mode", nMode != 0);
	return 0;
}

//!\brief 获取模式 
//!\return 0表示一般模式，1表示高级模式,-1表示失败
//##ModelId=49B5E6CC0128
int IAlarmServer::AMS_GetMode()
{
	return m_pCConfig->GetValue(IAlarmServer_CfgSection, "Mode", 0);
}

//##ModelId=49B634E702E1
int IAlarmServer::AMS_GetEnable()
{
	return m_pCConfig->GetValue(IAlarmServer_CfgSection, "Enable", 0);
}

//##ModelId=49B634E702E2
int IAlarmServer::AMS_SetEnable(int bEnable)
{
	m_pCConfig->SetValue(IAlarmServer_CfgSection, "Enable", (bEnable != 0));
	return 0;
}

//##ModelId=49B634E702E3
char *IAlarmServer::AMS_GetServerIP(char *pszServerIP)
{
	if (NULL == pszServerIP)
	{
		NET_ERR("[%s] NULL == pszServerIP \n", __FUNCTION__);
		return NULL;
	}
	m_pCConfig->GetValue(IAlarmServer_CfgSection, "ServerIP", "", pszServerIP, AMS_MAX_DOMAINNAME_LEN);
	return pszServerIP;
}

//##ModelId=49B634E702E4
int IAlarmServer::AMS_SetServerIP(const char *pszServerIP)
{
	if (NULL == pszServerIP)
	{
		NET_ERR("[%s] NULL == pszServerIP \n", __FUNCTION__);
		return -1;
	}
	m_pCConfig->SetValue(IAlarmServer_CfgSection, "ServerIP", pszServerIP);
	return 0;
}

//##ModelId=49B634E702E5
unsigned short IAlarmServer::AMS_GetServerPort()
{

	return m_pCConfig->GetValue(IAlarmServer_CfgSection, "ServerPort", 0);
}

//##ModelId=49B634E702E6
int IAlarmServer::AMS_SetServerPort(unsigned short usPort)
{
	m_pCConfig->SetValue(IAlarmServer_CfgSection, "ServerPort", usPort);
	return 0;
}



//##ModelId=49BDD3510242
IAlarmServer *IAlarmServer::instance = 0;

//##ModelId=49BDD3510269
IAlarmServer *IAlarmServer::Instance()
{
	if (instance == 0)
	{
		instance = new IAlarmServer();
	}
	return instance;
}

//##ModelId=49BDF36D006A
void IAlarmServer::Release()
{
	if (instance != 0)
	{
		delete instance;

		instance = 0;
	}
}
