// Copyright (C) 1991 - 1999 Rational Software Corporation

//#include "stdafx.h"
//#include <iostream>

#include "IManagerServer.h"
#include "NET_global.h"
#include "CConfig.h"
// #include "sys_banned.h"
#include "global_gather.h"

static const int NMS_Mode_default = 0;

static const int NMS_Enable_default = 0;

static const int NMS_port_default = 3031;

static const int NMS_RegisterTime_default = 5;	//秒

static const int NMS_Protocol_default = 0;	//私有协议

//初始化静态实例
IManagerServer *IManagerServer::instance = NULL;


//##ModelId=49B634E70232
IManagerServer::IManagerServer()
{
	//获取配置实例
	m_pCConfig = CConfig::GetInstance();

	if (NULL == m_pCConfig)
	{
		NET_ERR("NULL == m_pCConfig\n");

		snprintf(m_szServerIP, sizeof(m_szServerIP), NMS_IP_default);
		m_uiServerPort = NMS_port_default;
		m_nProtocol = NMS_Protocol_default;
		m_nMode = NMS_Mode_default;
		m_bEnable = NMS_Enable_default;
	}
	else
	{
		//读IP
		m_pCConfig->GetValue(NMS_CfgSection, NMS_IP_key, NMS_IP_default, m_szServerIP, NMS_MAX_DOMAINNAME_LEN);

		//读端口
		m_uiServerPort = m_pCConfig->GetValue(NMS_CfgSection, NMS_Port_key, NMS_port_default);

		//读注册时间
		m_uiRegisterTime = m_pCConfig->GetValue(NMS_CfgSection, NMS_RegisterTime_key, NMS_RegisterTime_default);

		//读通讯协议
		m_nProtocol = m_pCConfig->GetValue(NMS_CfgSection, NMS_Protocol_key, NMS_Protocol_default);

		//读工作模式
		m_nMode = m_pCConfig->GetValue(NMS_CfgSection, NMS_Mode_key, NMS_Mode_default);

		//读使能,放在最后，以使在其它配置未读取时出现操作异常
		m_bEnable = m_pCConfig->GetValue(NMS_CfgSection, NMS_Enable_key, NMS_Enable_default);
	}
}

//##ModelId=49B634E70235
IManagerServer::~IManagerServer()
{
}

//!\brief 获取模式 
//!\return 0表示一般模式，1表示高级模式,-1表示失败
//##ModelId=49B5E6CC0128
int IManagerServer::NMS_GetMode()
{
	return m_nMode;
}

//!\brief 设置模式 一般/高级
//!\param nMode 0表示一般模式,1表示高级模式
//!\return 0表示设置成功,-1表示失败
//##ModelId=49B5E6CC0128
int IManagerServer::NMS_SetMode(int nMode)
{
	m_nMode = nMode;

	return m_pCConfig->SetValue(NMS_CfgSection, NMS_Mode_key, nMode != 0);
}

//##ModelId=49B634E70236
int IManagerServer::NMS_GetEnable()
{
	return m_bEnable;
}

//##ModelId=49B634E70237
int IManagerServer::NMS_SetEnable(int bEnable)
{
	m_bEnable = bEnable;

	//写配置
	return m_pCConfig->SetValue(NMS_CfgSection, NMS_Enable_key, bEnable);
}

//##ModelId=49B634E70238
char *IManagerServer::NMS_GetServerIP(char *pszServerIP, int serverIpSize)
{
	if (NULL == pszServerIP)
	{
		NET_ERR("NULL == pszServerIP\n");
		return NULL;
	}

	strncpy(pszServerIP, m_szServerIP, serverIpSize - 1);
	return pszServerIP;
}

//##ModelId=49B634E70239
int IManagerServer::NMS_SetServerIP(const char *pszServerIP)
{
	if (NULL == pszServerIP)
	{
		NET_ERR("NULL == pszServerIP\n");
		return -1;
	}

	strncpy(m_szServerIP, pszServerIP, NMS_MAX_DOMAINNAME_LEN - 1);
	m_szServerIP[NMS_MAX_DOMAINNAME_LEN - 1] = 0;

	//写配置
	return m_pCConfig->SetValue(NMS_CfgSection, NMS_IP_key, m_szServerIP);
}

//##ModelId=49B634E7023A
unsigned short IManagerServer::NMS_GetServerPort()
{
	return m_uiServerPort;
}

//##ModelId=49B634E7023B
int IManagerServer::NMS_SetServerPort(unsigned short usPort)
{
	m_uiServerPort = usPort;

	//写配置
	return m_pCConfig->SetValue(NMS_CfgSection, NMS_Port_key, usPort);
}

//##ModelId=49B634E7023C
int IManagerServer::NMS_GetAutoRegisterTime()
{
	return m_uiRegisterTime;
}

//##ModelId=49B634E7023F
int IManagerServer::NMS_SetAutoRegisterTime(int nIntervalSecond)
{
	m_uiRegisterTime = nIntervalSecond;

	//写配置
	return m_pCConfig->SetValue(NMS_CfgSection, NMS_RegisterTime_key, nIntervalSecond);
}

//!\brief   获取通讯协议
//!\return  0表示禁用，1表示互信互通
int IManagerServer::NMS_GetProtocol()
{
	return m_nProtocol;
}

//!\brief   设置通讯协议
//!\param   nProtocol       0表示禁用，1表示互信互通
//!\return  0表示不NAT背后，1表示在NAT背后
int IManagerServer::NMS_SetProtocol(int nProtocol)
{
	m_nProtocol = nProtocol;

	//写配置
	return m_pCConfig->SetValue(NMS_CfgSection, NMS_Protocol_key, nProtocol);
}

//!\brief   获取互信互通是否有NAT服务
//!\return  0表示禁用，1表示互信互通
int IManagerServer::HXHT_GetNAT()
{
	return m_pCConfig->GetValue(HXHT_CfgSection, "NAT", 0);
}

//!\brief   设置互信互通是否有NAT服务
//!\param   bEnable     0表示不NAT背后，1表示在NAT背后
//!\return  0表示成功，其它为错误码
int IManagerServer::HXHT_SetNAT(int bEnable)
{
	return m_pCConfig->SetValue(HXHT_CfgSection, "NAT", bEnable);
}

//!\brief   获取互信互通前端设备ID
//!\param   pszDeviceID 返回前端设备ID
//!\param   nBufSize    pszDeviceID指向的buf大小
//!\return  本接口拷贝前端设备ID到pszDeviceID指向的buf，并返回pszDeviceID
char *IManagerServer::HXHT_GetDeviceID(char *pszDeviceID, int nBufSize)
{
	m_pCConfig->GetValue(HXHT_CfgSection, "DeviceID", "", pszDeviceID, nBufSize);
	return pszDeviceID;
}

//!\brief   设置互信互通前端设备ID
//!\param   pszDeviceID 前端设备ID
//!\return  0表示成功，其它为错误码
int IManagerServer::HXHT_SetDeviceID(const char *pszDeviceID)
{
	return m_pCConfig->SetValue(HXHT_CfgSection, "DeviceID", pszDeviceID);
}

//!\brief   获取互信互通存储IP地址
//!\param   pszServerIP 返回中心管理服务器IP地址
//!\param   nBufSize    pszServerIP指向的buf大小
//!\return  本接口拷贝中心管理服务器IP地址到pszServerIP指向的buf，并返回pszServerIP
char *IManagerServer::HXHT_GetStorageServerIP(char *pszServerIP, int nBufSize)
{
	m_pCConfig->GetValue(HXHT_CfgSection, "StorageServerIP", "192.168.0.30", pszServerIP, nBufSize);
	return pszServerIP;
}

//!\brief   设置互信互通存储IP地址
//!\param   pszServerIP 中心管理服务器IP地址
//!\return  0表示成功，其它为错误码
int IManagerServer::HXHT_SetStorageServerIP(const char *pszServerIP)
{
	return m_pCConfig->SetValue(HXHT_CfgSection, "StorageServerIP", pszServerIP);
}

//!\brief   获取互信互通存储端口
//!\return  端口号(0-65535)
unsigned short IManagerServer::HXHT_GetStorageServerPort()
{
	return m_pCConfig->GetValue(HXHT_CfgSection, "StorageServerPort", 6004);
}

//!\brief   设置互信互通存储服务器端口
//!\param   usPort  端口号(0-65535)
//!\return  0表示成功，其它为错误码
int IManagerServer::HXHT_SetStorageServerPort(unsigned short usPort)
{
	int nPort = usPort;

	return m_pCConfig->SetValue(HXHT_CfgSection, "StorageServerPort", nPort);
}

//##ModelId=49BDD333031A
IManagerServer *IManagerServer::Instance()
{
	if (instance == 0)
	{
		instance = new IManagerServer();
	}
	return instance;
}

//##ModelId=49BDEB7B0029
void IManagerServer::Release()
{
	if (instance != 0)
	{
		delete instance;

		instance = 0;
	}
}
