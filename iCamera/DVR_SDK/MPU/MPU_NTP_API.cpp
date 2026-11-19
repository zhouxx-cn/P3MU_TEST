
/*!
 *\file	MPU_NTP_API.h
 *\brief	MPU模块，NTP服务器管理相关接口定义
 *\author	林云
 *\date	2009/03/20

功能：
-#	NTP服务器域名、端口、自动更新时间等参数配置管理
-#	创建定时器，自动与NTP服务器同步
 */

#include "MPU_global.h"


#ifndef DEBUG_NOT_USE_NET_LIB	//调试时，不调用NET库

#include "NET/INTP.h"			//NTP服务器管理接口


/**************************************************************************
 *                                                           局部宏定义                                                        *
 **************************************************************************/

//调用NTP服务器管理器执行动作
#define MPU_NTP_CALL(x)									\
do														\
{														\
	int nRet;												\
														\
	INTP *pMgr = INTP::Instance();							\
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
		MPU_ERR("Get INTP instance fail!\n");					\
		MPU_CHK(NULL != pMgr);							\
		nRet = IDVR_MPU_NTP_NO_MANAGER_INSTANCE;			\
	}													\
														\
	return nRet;											\
}														\
while (0)


//调用NTP服务器管理器获取参数
#define MPU_NTP_GET(DataType, DefaultValue, Function)		\
do														\
{														\
	DataType retValue;										\
														\
	INTP *pMgr = INTP::Instance();							\
														\
	if (NULL != pMgr)										\
	{													\
		retValue = pMgr->Function;							\
	}													\
	else													\
	{													\
		MPU_ERR("Get INTP instance fail!\n");					\
		MPU_CHK(NULL != pMgr);							\
		retValue = DefaultValue;							\
	}													\
														\
	return retValue;										\
}														\
while (0)


#else							/* #ifndef DEBUG_NOT_USE_NET_LIB */


//调用NTP服务器管理器执行动作
#define MPU_NTP_CALL(x)									\
do														\
{														\
	return IDVR_OK;										\
}														\
while (0)

//调用NTP服务器管理器获取参数
#define MPU_NTP_GET(DataType, DefaultValue, Function)		\
do														\
{														\
	return DefaultValue;									\
}														\
while (0)


#endif							/* #ifndef DEBUG_NOT_USE_NET_LIB */



/**************************************************************************
 *                                                      模块内部接口                                                       *
 **************************************************************************/

//!\brief   初始化NTP服务器
//!\return  0表示成功，其它为错误码
int NTP_Init()
{
    return IDVR_OK;	//目前不再使用,去掉老的ntp代码
	int nRet = IDVR_OK;

#ifndef DEBUG_NOT_USE_NET_LIB	//调试时，不调用NET库

	INTP *pMgr = INTP::Instance();

	if (NULL == pMgr)
	{
		MPU_ERR("Get INTP instance fail!\n");
		MPU_CHK(NULL != pMgr);
		nRet = IDVR_MPU_NTP_NO_MANAGER_INSTANCE;
	}
	else
	{
		pMgr->Init();
		MPU_INFO("out, return %d\n", nRet);
	}

#endif							/* #ifndef DEBUG_NOT_USE_NET_LIB */

	return nRet;
}

//!\brief   逆初始化NTP服务器
//!\return  0表示成功，其它为错误码
int NTP_Uninit()
{
#ifndef DEBUG_NOT_USE_NET_LIB	//调试时，不调用NET库

	INTP::Release();

#endif							/* #ifndef DEBUG_NOT_USE_NET_LIB */

	return IDVR_OK;
}


/**************************************************************************
 *                                                          全局接口                                                               *
 **************************************************************************/

//!\brief      获取NTP服务器的工作模式
//!\return   0为普通工作模式，1为高级工作模式，失败为-1
int MPU_NTP_GetWorkMode()
{
	MPU_NTP_GET(int, 0, NTP_GetMode());
}

//!\brief       设置NTP服务器
//!\param   nWorkMode   0为普通工作模式，1为高级工作模式，失败为-1
//!\return  成功为0，失败为-1
int MPU_NTP_SetWorkMode(int nWorkMode)
{
	MPU_NTP_CALL(NTP_SetMode(nWorkMode));
}

//!\brief   获取是否启用NTP服务器
//!\return  TRUE表示启用，FALSE表示不启用
BOOL MPU_NTP_GetEnable()
{
	MPU_NTP_GET(BOOL, FALSE, NTP_GetEnable());
}

//!\brief   设置是否启用NTP服务器
//!\param   bEnable     TRUE表示启用，FALSE表示不启用
//!\return  0表示成功，其它为错误码
int MPU_NTP_SetEnable(BOOL bEnable)
{
	MPU_NTP_CALL(NTP_SetEnable(bEnable));
}

//!\brief   获取NTP服务器域名
//!\param   pszDomainName   返回NTP服务器域名
//!\return  本接口拷贝NTP服务器域名到pszDomainName指向的buf，并返回pszDomainName
char *MPU_NTP_GetServerDomainName(char *pszDomainName)
{
	MPU_NTP_GET(char *, pszDomainName, NTP_GetServerDomainName(pszDomainName));
}

//!\brief   设置NTP服务器域名
//!\param   pszDomainName   NTP服务器域名
//!\return  0表示成功，其它为错误码
int MPU_NTP_SetServerDomainName(const char *pszDomainName)
{
	MPU_NTP_CALL(NTP_SetServerDomainName(pszDomainName));
}

//!\brief   获取同步NTP服务器间隔时间
//!\return  间隔时间，单位为秒
int MPU_NTP_GetAutoUpdateTime()
{
	MPU_NTP_GET(int, 0, NTP_GetAutoUpdateTime());
}

//!\brief   设置同步NTP服务器间隔时间
//!\param   nIntervalSecond     间隔时间，单位为秒
//!\return  0表示成功，其它为错误码
int MPU_NTP_SetAutoUpdateTime(int nIntervalSecond)
{
	MPU_NTP_CALL(NTP_SetAutoUpdateTime(nIntervalSecond));
}


/**************************************************************************
 *                                          注销局部宏定义                                                             *
 **************************************************************************/

#undef MPU_NTP_CALL				//调用NTP服务器管理器执行动作
#undef MPU_NTP_GET				//调用NTP服务器管理器获取参数值
