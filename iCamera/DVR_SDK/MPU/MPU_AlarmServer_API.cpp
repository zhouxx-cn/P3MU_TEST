
/*!
 *\file		MPU_AlarmServer_API.cpp
 *\brief	MPU模块，报警服务器相关接口实现
 *\author	林云
 *\date		2009/03/20

功能：
-#	报警服务器参数配置（使能、IP地址、端口）
 */

#include "MPU_global.h"


#ifndef DEBUG_NOT_USE_NET_LIB	//调试时，不调用NET库

#include "NET/IAlarmServer.h"	//报警服务器管理接口


/**************************************************************************
 *                            局部宏定义                                  *
 **************************************************************************/


//调用报警服务器管理器执行动作
#define MPU_AMS_CALL(x)									\
do														\
{														\
	int nRet;												\
														\
	IAlarmServer *pMgr = IAlarmServer::Instance();				\
														\
	if (NULL != pMgr)										\
	{													\
		nRet = pMgr->x;									\
														\
		if (IDVR_OK != nRet)								\
		{												\
			MPU_ERR("%s, nRet = %d\n", #x, nRet);			\
		}												\
	}													\
	else													\
	{													\
		MPU_ERR("Get IAlarmServer instance fail!\n");			\
		MPU_CHK(NULL != pMgr);							\
		nRet = IDVR_MPU_AMS_NO_MANAGER_INSTANCE;			\
	}													\
														\
	return nRet;											\
}														\
while (0)


//调用报警服务器管理器获取参数
#define MPU_AMS_GET(DataType, DefaultValue, Function)		\
do														\
{														\
	DataType retValue;										\
														\
	IAlarmServer *pMgr = IAlarmServer::Instance();				\
														\
	if (NULL != pMgr)										\
	{													\
		retValue = pMgr->Function;							\
	}													\
	else													\
	{													\
		MPU_ERR("Get IAlarmServer instance fail!\n");			\
		MPU_CHK(NULL != pMgr);							\
		retValue = DefaultValue;							\
	}													\
														\
	return retValue;										\
}														\
while (0)


#else							/* #ifndef DEBUG_NOT_USE_NET_LIB */


//调用报警服务器管理器执行动作
#define MPU_AMS_CALL(x)									\
do														\
{														\
	return IDVR_OK;										\
}														\
while (0)

//调用报警服务器管理器获取参数
#define MPU_AMS_GET(DataType, DefaultValue, Function)		\
do														\
{														\
	return DefaultValue;									\
}														\
while (0)


#endif							/* #ifndef DEBUG_NOT_USE_NET_LIB */



/**************************************************************************
 *                            模块内部接口                                *
 **************************************************************************/

//!\brief   初始化报警服务器
//!\return  0表示成功，其它为错误码
int AMS_Init()
{
	int nRet = IDVR_OK;

#ifndef DEBUG_NOT_USE_NET_LIB	//调试时，不调用NET库

	IAlarmServer *pMgr = IAlarmServer::Instance();

	if (NULL == pMgr)
	{
		MPU_ERR("Get IAlarmServer instance fail!\n");
		MPU_CHK(NULL != pMgr);
		nRet = IDVR_MPU_AMS_NO_MANAGER_INSTANCE;
	}

#endif							/* #ifndef DEBUG_NOT_USE_NET_LIB */

	return nRet;
}

//!\brief   逆初始化报警服务器
//!\return  0表示成功，其它为错误码
int AMS_Uninit()
{
#ifndef DEBUG_NOT_USE_NET_LIB	//调试时，不调用NET库

	IAlarmServer::Release();

#endif							/* #ifndef DEBUG_NOT_USE_NET_LIB */

	return IDVR_OK;
}


/**************************************************************************
 *                            全局接口                                    *
 **************************************************************************/

//!\brief      获取报警服务器的工作模式
//!\return   0为普通工作模式，1为高级工作模式，失败为-1
int MPU_AMS_GetWorkMode()
{
#if 0
	int nWorkMode = 0;			//默认工作模式为普通模式

	//从配置文件初始化
	CConfig *pConfig = CConfig::GetInstance();

	MPU_CHK(NULL != pConfig);

	if (NULL != pConfig)
	{
		nWorkMode = pConfig->GetValue("AMS", "WorkMode", 0);
	}

	return nWorkMode;
#else
	MPU_AMS_GET(int, 0, AMS_GetMode());
#endif
}

//!\brief       设置报警服务器
//!\param   nWorkMode   0为普通工作模式，1为高级工作模式，失败为-1
//!\return  成功为0，失败为-1
int MPU_AMS_SetWorkMode(int nWorkMode)
{
#if 0
	int nRet;

	//从配置文件初始化
	CConfig *pConfig = CConfig::GetInstance();

	MPU_CHK(NULL != pConfig);

	if (NULL != pConfig)
	{
		nRet = pConfig->SetValue("AMS", "WorkMode", nWorkMode);
	}
	else
	{
		nRet = IDVR_MPU_NO_CCONFIG_INSTANCE;
	}

	return nRet;
#else
	MPU_AMS_CALL(AMS_SetMode(nWorkMode));
#endif
}

//!\fn      BOOL MPU_AMS_GetEnable()
//!\brief   获取是否启用报警管理服务器
//!\return  TRUE表示启用，FALSE表示不启用
BOOL MPU_AMS_GetEnable()
{
	MPU_AMS_GET(BOOL, FALSE, AMS_GetEnable());
}

//!\fn      int MPU_AMS_SetEnable(BOOL bEnable)
//!\brief   设置是否启用报警管理服务器
//!\param   bEnable     TRUE表示启用，FALSE表示不启用
//!\return  0表示成功，其它为错误码
int MPU_AMS_SetEnable(BOOL bEnable)
{
	MPU_AMS_CALL(AMS_SetEnable(bEnable));
}

//!\fn      char *MPU_AMS_GetServerIP(char *pszServerIP)
//!\brief   获取报警管理服务器IP地址
//!\param   pszServerIP 返回报警管理服务器IP地址，buf大小不得小于16字节
//!\return  本接口拷贝报警管理服务器IP地址到pszServerIP指向的buf，并返回pszServerIP
char *MPU_AMS_GetServerIP(char *pszServerIP)
{
	MPU_AMS_GET(char *, pszServerIP, AMS_GetServerIP(pszServerIP));
}

//!\fn      int MPU_AMS_SetServerIP(const char *pszServerIP)
//!\brief   设置报警管理服务器IP地址
//!\param   pszServerIP 报警管理服务器IP地址
//!\return  0表示成功，其它为错误码
int MPU_AMS_SetServerIP(const char *pszServerIP)
{
	MPU_AMS_CALL(AMS_SetServerIP(pszServerIP));
}

//!\fn      unsigned short MPU_AMS_GetServerPort()
//!\brief   获取报警管理服务器端口
//!\return  端口号(0-65535)
unsigned short MPU_AMS_GetServerPort()
{
	MPU_AMS_GET(unsigned short, 5000, AMS_GetServerPort());
}

//!\fn      int MPU_AMS_SetServerPort(unsigned short usPort)
//!\brief   设置报警管理服务器端口
//!\param   usPort  端口号(0-65535)
//!\return  0表示成功，其它为错误码
int MPU_AMS_SetServerPort(unsigned short usPort)
{
	MPU_AMS_CALL(AMS_SetServerPort(usPort));
}


/**************************************************************************
 *                            注销局部宏定义                              *
 **************************************************************************/

#undef MPU_AMS_CALL				//调用报警服务器管理器执行动作
#undef MPU_AMS_GET				//调用报警服务器管理器获取参数值
