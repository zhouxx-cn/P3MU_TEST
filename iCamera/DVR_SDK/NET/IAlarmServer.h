// Copyright (C) 1991 - 1999 Rational Software Corporation

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_IALARMSERVER_49B6310F0398_INCLUDED
#define _INC_IALARMSERVER_49B6310F0398_INCLUDED

class CConfig;

//!\brief   Net模块     报警管理服务器设置接口类
//##ModelId=49B634E702D8
class IAlarmServer
{
  public:
	//!\brief   获取唯一实例
	//!\return  实例指针
	//##ModelId=49BDD3510269
	static IAlarmServer *Instance();

	//!\brief   销毁实例
	//##ModelId=49BDF36D006A
	static void Release();

	//!\brief 设置模式 一般/高级
	//!\param nMode 0表示一般模式,1表示高级模式
	//!\return 0表示设置成功,-1表示失败
	//##ModelId=49B5E6CC0128
	int AMS_SetMode(int nMode);

	//!\brief 获取模式 
	//!\return 0表示一般模式，1表示高级模式,-1表示失败
	//##ModelId=49B5E6CC0128
	int AMS_GetMode();

	//!\brief   获取是否启用报警管理服务器
	//!\return  true表示启用，false表示不启用
	//##ModelId=49B634E702E1
	int AMS_GetEnable();

	//!\brief   设置是否启用报警管理服务器
	//!\param   bEnable     true表示启用，false表示不启用
	//!\return  0表示成功，其它为错误码
	//##ModelId=49B634E702E2
	int AMS_SetEnable(int bEnable);

	//!\brief   获取报警管理服务器IP地址
	//!\param   pszServerIP 返回报警管理服务器IP地址
	//!\return  本接口拷贝报警管理服务器IP地址到pszServerIP指向的buf，并返回pszServerIP
	//##ModelId=49B634E702E3
	char *AMS_GetServerIP(char *pszServerIP);

	//!\brief   设置报警管理服务器IP地址
	//!\param   pszServerIP 报警管理服务器IP地址
	//!\return  0表示成功，其它为错误码
	//##ModelId=49B634E702E4
	int AMS_SetServerIP(const char *pszServerIP);

	//!\brief   获取报警管理服务器端口
	//!\return  端口号(0-65535)
	//##ModelId=49B634E702E5
	unsigned short AMS_GetServerPort();

	//!\brief   设置报警管理服务器端口
	//!\param   usPort  端口号(0-65535)
	//!\return  0表示成功，其它为错误码
	//##ModelId=49B634E702E6
	int AMS_SetServerPort(unsigned short usPort);

  protected:
	//##ModelId=49B634E702DF
	  IAlarmServer();

	//##ModelId=49B634E702E0
	 ~IAlarmServer();

  private:
	//##ModelId=49BDD3510242
	static IAlarmServer *instance;

	//配置文件
	CConfig *m_pCConfig;
};

#endif							/* _INC_IALARMSERVER_49B6310F0398_INCLUDED */
