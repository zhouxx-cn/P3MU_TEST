// Copyright (C) 1991 - 1999 Rational Software Corporation

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_IMANAGERSERVER_49B62EBF00D0_INCLUDED
#define _INC_IMANAGERSERVER_49B62EBF00D0_INCLUDED
#include <pthread.h>
#include "global_macro.h"

#include "CConfig.h"
#include "MSG.h"


#define NMS_MAX_DOMAINNAME_LEN 			(256)

class CConfig;

//!\brief   Net模块     中心管理服务器设置接口类
//##ModelId=49B634E70231
class IManagerServer
{
  public:
	//!\brief   获取唯一实例
	//!\return  实例指针
	//##ModelId=49BDD333031A
	static IManagerServer *Instance();

	//!\brief   销毁实例
	//##ModelId=49BDEB7B0029
	static void Release();

	//!\brief 设置模式 一般/高级
	//!\param nMode 0表示一般模式,1表示高级模式
	//!\return 0表示设置成功,-1表示失败
	//##ModelId=49B5E6CC0128
	int NMS_SetMode(int nMode);

	//!\brief 获取模式 
	//!\return 0表示一般模式，1表示高级模式,-1表示失败
	//##ModelId=49B5E6CC0128
	int NMS_GetMode();

	//!\brief   获取是否启用中心管理服务器
	//!\return  true表示启用，false表示不启用
	//##ModelId=49B634E70236
	int NMS_GetEnable();

	//!\brief   设置是否启用中心管理服务器
	//!\param   bEnable     true表示启用，false表示不启用
	//!\return  0表示成功，其它为错误码
	//##ModelId=49B634E70237
	int NMS_SetEnable(int bEnable);

	//!\brief   获取中心管理服务器IP地址
	//!\param   pszServerIP 返回中心管理服务器IP地址
	//!\return  本接口拷贝中心管理服务器IP地址到pszServerIP指向的buf，并返回pszServerIP
	//##ModelId=49B634E70238
	char *NMS_GetServerIP(char *pszServerIP, int serverIpSize);

	//!\brief   设置中心管理服务器IP地址
	//!\param   pszServerIP 中心管理服务器IP地址
	//!\return  0表示成功，其它为错误码
	//##ModelId=49B634E70239
	int NMS_SetServerIP(const char *pszServerIP);

	//!\brief   获取中心管理服务器端口
	//!\return  端口号(0-65535)
	//##ModelId=49B634E7023A
	unsigned short NMS_GetServerPort();

	//!\brief   设置中心管理服务器端口
	//!\param   usPort  端口号(0-65535)
	//!\return  0表示成功，其它为错误码
	//##ModelId=49B634E7023B
	int NMS_SetServerPort(unsigned short usPort);

	//!\brief   获取自动注册中心服务器间隔时间
	//!\return  间隔时间，单位为秒
	//##ModelId=49B634E7023C
	int NMS_GetAutoRegisterTime();

	//!\brief   设置自动注册中心服务器间隔时间
	//!\param   nIntervalSecond     间隔时间，单位为秒
	//!\return  0表示成功，其它为错误码
	//##ModelId=49B634E7023F
	int NMS_SetAutoRegisterTime(int nIntervalSecond);

	//!\brief   获取通讯协议
	//!\return  0表示禁用，1表示互信互通
	int NMS_GetProtocol();

	//!\brief   设置通讯协议
	//!\param   nProtocol       0表示禁用，1表示互信互通
	//!\return  0表示不NAT背后，1表示在NAT背后
	int NMS_SetProtocol(int nProtocol);

	//!\brief   获取互信互通是否有NAT服务
	//!\return  0表示禁用，1表示互信互通
	int HXHT_GetNAT();

	//!\brief   设置互信互通是否有NAT服务
	//!\param   bEnable     0表示不NAT背后，1表示在NAT背后
	//!\return  0表示成功，其它为错误码
	int HXHT_SetNAT(int bEnable);

	//!\brief   获取互信互通前端设备ID
	//!\param   pszDeviceID 返回前端设备ID
	//!\param   nBufSize    pszDeviceID指向的buf大小
	//!\return  本接口拷贝前端设备ID到pszDeviceID指向的buf，并返回pszDeviceID
	char *HXHT_GetDeviceID(char *pszDeviceID, int nBufSize);

	//!\brief   设置互信互通前端设备ID
	//!\param   pszDeviceID 前端设备ID
	//!\return  0表示成功，其它为错误码
	int HXHT_SetDeviceID(const char *pszDeviceID);

	//!\brief   获取互信互通存储IP地址
	//!\param   pszServerIP 返回中心管理服务器IP地址
	//!\param   nBufSize    pszServerIP指向的buf大小
	//!\return  本接口拷贝中心管理服务器IP地址到pszServerIP指向的buf，并返回pszServerIP
	char *HXHT_GetStorageServerIP(char *pszServerIP, int nBufSize);

	//!\brief   设置互信互通存储IP地址
	//!\param   pszServerIP 中心管理服务器IP地址
	//!\return  0表示成功，其它为错误码
	int HXHT_SetStorageServerIP(const char *pszServerIP);

	//!\brief   获取互信互通存储端口
	//!\return  端口号(0-65535)
	unsigned short HXHT_GetStorageServerPort();

	//!\brief   设置互信互通存储服务器端口
	//!\param   usPort  端口号(0-65535)
	//!\return  0表示成功，其它为错误码
	int HXHT_SetStorageServerPort(unsigned short usPort);

  protected:
	//##ModelId=49B634E70232
	  IManagerServer();

	//##ModelId=49B634E70235
	 ~IManagerServer();

  private:
	//##ModelId=49BDD33302E9
	static IManagerServer *instance;

	//工作模式
	int m_nMode;

	//是否启用中心管理服务器
	int m_bEnable;

	//中心管理IP或者域名
	char m_szServerIP[NMS_MAX_DOMAINNAME_LEN];

	//中心管理端口
	unsigned int m_uiServerPort;

	//自动注册时间间隔
	time_t m_uiRegisterTime;

	//通讯协议，0为私有协议，1为互信互通
	int m_nProtocol;

	//配置文件
	CConfig *m_pCConfig;
};

#endif							/* _INC_IMANAGERSERVER_49B62EBF00D0_INCLUDED */
