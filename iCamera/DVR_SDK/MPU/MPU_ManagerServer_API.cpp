
/*!
 *\file		MPU_ManagerServer_API.cpp
 *\brief	MPU模块，中心管理服务器相关接口实现
 *\author	林云
 *\date		2009/03/20

功能：
-#	中心服务器参数管理（是否启用、IP地址、端口、自动注册时间间隔）
 */


#include "MPU_global.h"


#ifndef DEBUG_NOT_USE_NET_LIB	//调试时，不调用NET库

#include "NET/IManagerServer.h"	//管理服务器接口


/**************************************************************************
 *                                                     局部宏定义                                                              *
 **************************************************************************/

//调用中心管理服务器管理器执行动作
#define MPU_NMS_CALL(x)									\
do														\
{														\
	int nRet;												\
														\
	IManagerServer *pMgr = IManagerServer::Instance();			\
														\
	if (NULL != pMgr)										\
	{													\
		nRet = pMgr->x;									\
		if (IDVR_OK != nRet)								\
		{												\
			MPU_ERR("%s, nRet = %d\n", #x, nRet);			\
		}												\
	}													\
	else													\
	{													\
		MPU_ERR("Get IManagerServer instance fail!\n");			\
		MPU_CHK(NULL != pMgr);							\
		nRet = IDVR_MPU_NMS_NO_MANAGER_INSTANCE;			\
	}													\
														\
	return nRet;											\
}														\
while (0)


//调用中心管理服务器管理器获取参数
#define MPU_NMS_GET(DataType, DefaultValue, Function)		\
do														\
{														\
	DataType retValue;										\
														\
	IManagerServer *pMgr = IManagerServer::Instance();			\
														\
	if (NULL != pMgr)										\
	{													\
		retValue = pMgr->Function;							\
	}													\
	else													\
	{													\
		MPU_ERR("Get IManagerServer instance fail!\n");			\
		MPU_CHK(NULL != pMgr);							\
		retValue = DefaultValue;							\
	}													\
														\
	return retValue;										\
}														\
while (0)


#else							/* #ifndef DEBUG_NOT_USE_NET_LIB */


//调用中心管理服务器管理器执行动作
#define MPU_NMS_CALL(x)									\
do														\
{														\
	return IDVR_OK;										\
}														\
while (0)

//调用中心管理服务器管理器获取参数
#define MPU_NMS_GET(DataType, DefaultValue, Function)		\
do														\
{														\
	return DefaultValue;									\
}														\
while (0)


#endif							/* #ifndef DEBUG_NOT_USE_NET_LIB */



/**************************************************************************
 *                            模块内部接口                                *
 **************************************************************************/

//!\brief   初始化中心管理服务器
//!\return  0表示成功，其它为错误码
int NMS_Init()
{
	int nRet = IDVR_OK;

#ifndef DEBUG_NOT_USE_NET_LIB	//调试时，不调用NET库

	IManagerServer *pMgr = IManagerServer::Instance();

	if (NULL != pMgr)
	{
		//if (pMgr->NMS_GetMode())
		if (pMgr->NMS_GetEnable())
		{
			int nSDKRet = HieServer_DeviceRegisterStart();

			if (0 == nSDKRet)
			{
				MPU_DBG("start nms\n");
			}
			else
			{
				MPU_ERR("HieServer_DeviceRegisterStart() fail, return %d\n", nSDKRet);
			}
		}
	}
	else
	{
		MPU_ERR("Get IManagerServer instance fail!\n");
		MPU_CHK(NULL != pMgr);
		nRet = IDVR_MPU_NMS_NO_MANAGER_INSTANCE;
	}

#endif							/* #ifndef DEBUG_NOT_USE_NET_LIB */

	return nRet;
}

//!\brief   逆初始化中心管理服务器
//!\return  0表示成功，其它为错误码
int NMS_Uninit()
{
	int nSDKRet;

	nSDKRet = HieServer_DeviceRegisterStop();
	if (0 != nSDKRet)
	{
		MPU_ERR("HieServer_DeviceRegisterStop() fail, return %d\n", nSDKRet);
	}

#ifndef DEBUG_NOT_USE_NET_LIB	//调试时，不调用NET库

	IManagerServer::Release();

#endif							/* #ifndef DEBUG_NOT_USE_NET_LIB */

	return IDVR_OK;
}

//!\brief       重启动NMS注册服务
//!\return  1表示启动，0表示没有启动
int NMS_Reset()
{
	int nSDKRet;

	int nWorkMode = MPU_NMS_GetWorkMode();
	int bNMSEnable = MPU_NMS_GetEnable();

	nSDKRet = HieServer_DeviceRegisterStop();
	if (0 != nSDKRet)
	{
		MPU_ERR("HieServer_DeviceRegisterStop() fail, return %d\n", nSDKRet);
	}

	if (nWorkMode && bNMSEnable)
	{
		nSDKRet = HieServer_DeviceRegisterStart();
		if (0 == nSDKRet)
		{
			MPU_DBG("restart nms\n");
			return 1;
		}
		else
		{
			MPU_ERR("HieServer_DeviceRegisterStart() fail, return %d\n", nSDKRet);
		}
	}

	MPU_DBG("stop nms\n");
	return 0;
}



/**************************************************************************
 *                            全局接口                                    *
 **************************************************************************/

//!\brief      获取中心管理服务器的工作模式
//!\return   0为普通工作模式，1为高级工作模式，失败为-1
int MPU_NMS_GetWorkMode()
{
#if 0
	int nWorkMode = 0;			//默认工作模式为普通模式

	//从配置文件初始化
	CConfig *pConfig = CConfig::GetInstance();

	MPU_CHK(NULL != pConfig);

	if (NULL != pConfig)
	{
		nWorkMode = pConfig->GetValue("NMS", "WorkMode", 0);
	}

	return nWorkMode;
#else
	MPU_NMS_GET(int, 0, NMS_GetMode());
#endif
}

//!\brief       设置中心管理服务器
//!\param   nWorkMode   0为普通工作模式，1为高级工作模式，失败为-1
//!\return  成功为0，失败为-1
static int NMS_SetWorkMode(int nWorkMode)
{
#if 0
	int nRet;

	//从配置文件初始化
	CConfig *pConfig = CConfig::GetInstance();

	MPU_CHK(NULL != pConfig);

	if (NULL != pConfig)
	{
		nRet = pConfig->SetValue("NMS", "WorkMode", nWorkMode);
	}
	else
	{
		nRet = IDVR_MPU_NO_CCONFIG_INSTANCE;
	}

	return nRet;
#else
	MPU_NMS_CALL(NMS_SetMode(nWorkMode));
#endif
}

//!\brief       设置中心管理服务器
//!\param   nWorkMode   0为普通工作模式，1为高级工作模式，失败为-1
//!\return  成功为0，失败为-1
int MPU_NMS_SetWorkMode(int nWorkMode)
{
	int nRet = IDVR_OK;

	if (MPU_NMS_GetWorkMode() != nWorkMode)
	{
		nRet = NMS_SetWorkMode(nWorkMode);
		if (0 == nRet)
		{
			NMS_Reset();
		}
	}

	return nRet;
}

//!\brief   获取是否启用中心管理服务器
//!\return  TRUE表示启用，FALSE表示不启用
BOOL MPU_NMS_GetEnable()
{
	MPU_NMS_GET(BOOL, FALSE, NMS_GetEnable());
}

//!\brief   设置是否启用中心管理服务器
//!\param   bEnable     TRUE表示启用，FALSE表示不启用
//!\return  0表示成功，其它为错误码
static int NMS_SetEnable(BOOL bEnable)
{
	MPU_NMS_CALL(NMS_SetEnable(bEnable));
}

//!\brief   设置是否启用中心管理服务器
//!\param   bEnable     TRUE表示启用，FALSE表示不启用
//!\return  0表示成功，其它为错误码
int MPU_NMS_SetEnable(BOOL bEnable)
{
	int nRet = IDVR_OK;

	//9000设备不能配置该项目
	//bEnable = TRUE;

	if (MPU_NMS_GetEnable() != bEnable)
	{
		nRet = NMS_SetEnable(bEnable);
		if (0 == nRet)
		{
			NMS_Reset();
		}
	}

	return nRet;
}

//!\brief   获取中心管理服务器IP地址
//!\param   pszServerIP 返回中心管理服务器IP地址
//!\return  本接口拷贝中心管理服务器IP地址到pszServerIP指向的buf，并返回pszServerIP
char *MPU_NMS_GetServerIP(char *pszServerIP, int serverIpSize)
{
	MPU_NMS_GET(char *, pszServerIP, NMS_GetServerIP(pszServerIP, serverIpSize));
}

//!\brief   设置中心管理服务器IP地址
//!\param   pszServerIP 中心管理服务器IP地址
//!\return  0表示成功，其它为错误码
static int NMS_SetServerIP(const char *pszServerIP)
{
	MPU_NMS_CALL(NMS_SetServerIP(pszServerIP));
}

//!\brief   设置中心管理服务器IP地址
//!\param   pszServerIP 中心管理服务器IP地址
//!\return  0表示成功，其它为错误码
int MPU_NMS_SetServerIP(const char *pszServerIP)
{
	int nRet = IDVR_OK;
	char szServerAddress[256] = { 0 };

	MPU_NMS_GetServerIP(szServerAddress, sizeof(szServerAddress));

	if (0 != strncmp(pszServerIP, szServerAddress, 256))
	{
		nRet = NMS_SetServerIP(pszServerIP);
		if (0 == nRet)
		{
			NMS_Reset();
		}
	}

	return nRet;
}

//!\brief   获取中心管理服务器端口
//!\return  端口号(0-65535)
unsigned short MPU_NMS_GetServerPort()
{
	MPU_NMS_GET(unsigned short, 0, NMS_GetServerPort());
}

//!\brief   设置中心管理服务器端口
//!\param   usPort  端口号(0-65535)
//!\return  0表示成功，其它为错误码
static int NMS_SetServerPort(unsigned short usPort)
{
	MPU_NMS_CALL(NMS_SetServerPort(usPort));
}

//!\brief   设置中心管理服务器端口
//!\param   usPort  端口号(0-65535)
//!\return  0表示成功，其它为错误码
int MPU_NMS_SetServerPort(unsigned short usPort)
{
	int nRet = IDVR_OK;

	if (MPU_NMS_GetServerPort() != usPort)
	{
		nRet = NMS_SetServerPort(usPort);
		if (0 == nRet)
		{
			NMS_Reset();
		}
	}

	return nRet;
}

//!\brief   获取自动注册中心服务器间隔时间
//!\return  间隔时间，单位为秒
int MPU_NMS_GetAutoRegisterTime()
{
	MPU_NMS_GET(int, 0, NMS_GetAutoRegisterTime());
}

//!\brief   设置自动注册中心服务器间隔时间
//!\param   nIntervalSecond     间隔时间，单位为秒
//!\return  0表示成功，其它为错误码
static int NMS_SetAutoRegisterTime(int nIntervalSecond)
{
	MPU_NMS_CALL(NMS_SetAutoRegisterTime(nIntervalSecond));
}

//!\brief   设置自动注册中心服务器间隔时间
//!\param   nIntervalSecond     间隔时间，单位为秒
//!\return  0表示成功，其它为错误码
int MPU_NMS_SetAutoRegisterTime(int nIntervalSecond)
{
	int nRet = IDVR_OK;

	if (MPU_NMS_GetAutoRegisterTime() != nIntervalSecond)
	{
		nRet = NMS_SetAutoRegisterTime(nIntervalSecond);
		if (0 == nRet)
		{
			NMS_Reset();
		}
	}

	return nRet;
}

//!\brief   获取通讯协议
//!\return  0表示禁用，1表示互信互通
int MPU_NMS_GetProtocol()
{
	MPU_NMS_GET(int, 0, NMS_GetProtocol());
}

//!\brief   设置通讯协议
//!\param   nProtocol       0表示禁用，1表示互信互通
//!\return  0表示不NAT背后，1表示在NAT背后
static int NMS_SetProtocol(int nProtocol)
{
	MPU_NMS_CALL(NMS_SetProtocol(nProtocol));
}

//!\brief   设置通讯协议
//!\param   nProtocol       0表示禁用，1表示互信互通
//!\return  0表示不NAT背后，1表示在NAT背后
int MPU_NMS_SetProtocol(int nProtocol)
{
	int nRet = IDVR_OK;

	if (MPU_NMS_GetProtocol() != nProtocol)
	{
		nRet = NMS_SetProtocol(nProtocol);
		if (0 == nRet)
		{
			NMS_Reset();
		}
	}

	return nRet;
}

//!\brief   获取互信互通是否有NAT服务
//!\return  0表示禁用，1表示互信互通
int MPU_HXHT_GetNAT()
{
	MPU_NMS_GET(int, 0, HXHT_GetNAT());
}

//!\brief   设置互信互通是否有NAT服务
//!\param   bEnable     0表示不NAT背后，1表示在NAT背后
//!\return  0表示成功，其它为错误码
static int HXHT_SetNAT(int bEnable)
{
	MPU_NMS_CALL(HXHT_SetNAT(bEnable));
}

//!\brief   设置互信互通是否有NAT服务
//!\param   bEnable     0表示不NAT背后，1表示在NAT背后
//!\return  0表示成功，其它为错误码
int MPU_HXHT_SetNAT(int bEnable)
{
	int nRet = IDVR_OK;

	if (MPU_HXHT_GetNAT() != bEnable)
	{
		nRet = HXHT_SetNAT(bEnable);
		if (0 == nRet)
		{
			if (MPU_NMS_GetProtocol() == 1)
			{
				NMS_Reset();
			}
		}
	}

	return nRet;
}

//!\brief   获取互信互通存储IP地址
//!\param   pszServerIP 返回中心管理服务器IP地址
//!\param   nBufSize    pszServerIP指向的buf大小
//!\return  本接口拷贝中心管理服务器IP地址到pszServerIP指向的buf，并返回pszServerIP
char *MPU_HXHT_GetStorageServerIP(char *pszServerIP, int nBufSize)
{
	MPU_NMS_GET(char *, pszServerIP, HXHT_GetStorageServerIP(pszServerIP, nBufSize));
}

//!\brief   设置互信互通存储IP地址
//!\param   pszServerIP 中心管理服务器IP地址
//!\return  0表示成功，其它为错误码
static int HXHT_SetStorageServerIP(const char *pszServerIP)
{
	MPU_NMS_CALL(HXHT_SetStorageServerIP(pszServerIP));
}

//!\brief   设置互信互通存储IP地址
//!\param   pszServerIP 中心管理服务器IP地址
//!\return  0表示成功，其它为错误码
int MPU_HXHT_SetStorageServerIP(const char *pszServerIP)
{
	int nRet = IDVR_OK;
	char szServerAddress[DVR_MAX_DOMAINNAME_LEN] = { 0 };

	MPU_HXHT_GetStorageServerIP(szServerAddress, sizeof (szServerAddress));

	if (0 != strncmp(pszServerIP, szServerAddress, sizeof (szServerAddress)))
	{
		nRet = HXHT_SetStorageServerIP(pszServerIP);
		if (0 == nRet)
		{
			if (MPU_NMS_GetProtocol() == 1)
			{
				NMS_Reset();
			}
		}
	}

	return nRet;
}

//!\brief   获取互信互通存储端口
//!\return  端口号(0-65535)
unsigned short MPU_HXHT_GetStorageServerPort()
{
	MPU_NMS_GET(unsigned short, 6004, HXHT_GetStorageServerPort());
}

//!\brief   设置互信互通存储服务器端口
//!\param   usPort  端口号(0-65535)
//!\return  0表示成功，其它为错误码
static int HXHT_SetStorageServerPort(unsigned short usPort)
{
	MPU_NMS_CALL(HXHT_SetStorageServerPort(usPort));
}

//!\brief   设置互信互通存储服务器端口
//!\param   usPort  端口号(0-65535)
//!\return  0表示成功，其它为错误码
int MPU_HXHT_SetStorageServerPort(unsigned short usPort)
{
	int nRet = IDVR_OK;

	if (MPU_HXHT_GetStorageServerPort() != usPort)
	{
		nRet = HXHT_SetStorageServerPort(usPort);
		if (0 == nRet)
		{
			if (MPU_NMS_GetProtocol() == 1)
			{
				NMS_Reset();
			}
		}
	}

	return nRet;
}

//!\brief   获取互信互通前端设备ID
//!\param   pszDeviceID 返回前端设备ID
//!\param   nBufSize    pszDeviceID指向的buf大小
//!\return  本接口拷贝前端设备ID到pszDeviceID指向的buf，并返回pszDeviceID
char *MPU_HXHT_GetDeviceID(char *pszDeviceID, int nBufSize)
{
	MPU_NMS_GET(char *, pszDeviceID, HXHT_GetDeviceID(pszDeviceID, nBufSize));
}

//!\brief   设置互信互通前端设备ID
//!\param   pszDeviceID 前端设备ID
//!\return  0表示成功，其它为错误码
static int HXHT_SetDeviceID(const char *pszDeviceID)
{
	MPU_NMS_CALL(HXHT_SetDeviceID(pszDeviceID));
}

//!\brief   设置互信互通前端设备ID
//!\param   pszDeviceID 前端设备ID
//!\return  0表示成功，其它为错误码
int MPU_HXHT_SetDeviceID(const char *pszDeviceID)
{
	int nRet = IDVR_OK;
	char szDeviceID[DVR_COMMON_STRING_LEN] = { 0 };

	MPU_HXHT_GetDeviceID(szDeviceID, sizeof (szDeviceID));

	if (0 != strncmp(pszDeviceID, szDeviceID, sizeof (szDeviceID)))
	{
		nRet = HXHT_SetDeviceID(pszDeviceID);
		if (0 == nRet)
		{
			if (MPU_NMS_GetProtocol() == 1)
			{
				NMS_Reset();
			}
		}
	}

	return nRet;
}


/**************************************************************************
 *                            注销局部宏定义                              *
 **************************************************************************/

#undef MPU_NMS_CALL				//调用中心管理服务器管理器执行动作
#undef MPU_NMS_GET				//调用中心管理服务器管理器获取参数值
