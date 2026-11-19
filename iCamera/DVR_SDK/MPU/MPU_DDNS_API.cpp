
/*!
 *\file		MPU_DDNS_API.cpp
 *\brief	MPU模块，DDNS服务相关接口实现
 *\author	林云
 *\date		2009/03/20

功能：
-#	DDNS服务能力选择（支持多种协议）
-#	DDNS服务配置（选择协议、使能、用户名、密码、域名）
-#	手动更新，自动保活
  */


#include "MPU_global.h"


#ifndef DEBUG_NOT_USE_NET_LIB	//调试时，不调用NET库

#include "NET/IDDNS.h"			//DDNS配置接口

/**************************************************************************
 *                            局部宏定义                                  *
 **************************************************************************/


//调用DDNS管理器执行动作
#define MPU_DDNS_CALL(x)								\
do														\
{														\
	int nRet;												\
														\
	IDDNS *pMgr = IDDNS::GetInstance();						\
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
		MPU_ERR("Get IDDNS instance fail!\n");				\
		MPU_CHK(NULL != pMgr);							\
		nRet = IDVR_MPU_DDNS_NO_MANAGER_INSTANCE;		\
	}													\
														\
	return nRet;											\
}														\
while (0)


//调用DDNS管理器获取参数
#define MPU_DDNS_GET(DataType, DefaultValue, Function)		\
do														\
{														\
	DataType retValue;										\
														\
	IDDNS *pMgr = IDDNS::GetInstance();						\
														\
	if (NULL != pMgr)										\
	{													\
		retValue = pMgr->Function;							\
	}													\
	else													\
	{													\
		MPU_ERR("Get IDDNS instance fail!\n");				\
		MPU_CHK(NULL != pMgr);							\
		retValue = DefaultValue;							\
	}													\
														\
	return retValue;										\
}														\
while (0)


#else							/* #ifndef DEBUG_NOT_USE_NET_LIB */


//调用DDNS管理器执行动作
#define MPU_DDNS_CALL(x)								\
do														\
{														\
	return IDVR_OK;										\
}														\
while (0)

//调用DDNS管理器获取参数
#define MPU_DDNS_GET(DataType, DefaultValue, Function)		\
do														\
{														\
	return DefaultValue;									\
}														\
while (0)


#endif							/* #ifndef DEBUG_NOT_USE_NET_LIB */



/**************************************************************************
 *                            模块内部接口                                *
 **************************************************************************/

//!\brief   初始化DDNS
//!\return  0表示成功，其它为错误码
int DDNS_Init()
{
	int nRet = IDVR_OK;

#ifndef DEBUG_NOT_USE_NET_LIB	//调试时，不调用NET库

	IDDNS *pMgr = IDDNS::GetInstance();

	if (NULL == pMgr)
	{
		MPU_ERR("Get IDDNS instance fail!\n");
		MPU_CHK(NULL != pMgr);
		nRet = IDVR_MPU_DDNS_NO_MANAGER_INSTANCE;
	}
	else
	{
		MPU_INFO("out, return %d\n", nRet);
	}

#endif							/* #ifndef DEBUG_NOT_USE_NET_LIB */

	return nRet;
}

//!\brief   逆初始化DDNS
//!\return  0表示成功，其它为错误码
int DDNS_Uninit()
{
#ifndef DEBUG_NOT_USE_NET_LIB	//调试时，不调用NET库

	IDDNS::Release();

#endif							/* #ifndef DEBUG_NOT_USE_NET_LIB */

	return IDVR_OK;
}


/**************************************************************************
 *                            全局接口                                    *
 **************************************************************************/

//!\brief   获取支持的协议总数
//!\return  协议总数
int MPU_DDNS_GetProtocolCount()
{
	MPU_DDNS_GET(int, 0, DDNS_GetProtocolCount());
}

//!\brief   获取当前选择的协议编号
//!\return  协议编号[0 - (count-1)]
int MPU_DDNS_GetCurrentProtocol()
{
	MPU_DDNS_GET(int, -1, DDNS_GetCurrentProtocol());
}

//!\brief   设置当前协议编号
//!\param   nIndex      协议编号[0 - (count-1)]
//!\return  0表示成功，其它为错误码
int MPU_DDNS_SetCurrentProtocol(int nIndex)
{
	MPU_DDNS_CALL(DDNS_SetCurrentProtocol(nIndex));
}

//!\brief   获取是否启用DDNS
//!\return  true表示启用，false表示禁用
bool MPU_DDNS_GetEnable()
{
	MPU_DDNS_GET(bool, false, DDNS_GetEnable());
}

//!\brief   设置是否启用DDNS
//!\param   bEnable     true表示启用，false表示禁用
//!\return  0表示成功，其它为错误码
int MPU_DDNS_SetEnable(bool bEnable)
{
	MPU_DDNS_CALL(DDNS_SetEnable(bEnable));
}

//!\brief   获取DDNS协议名称
//!\param   nIndex          协议编号[0 - (count-1)]
//!\param   ProtocolName    返回协议名称
//!\return  本接口拷贝DDNS协议名称到pszProtocolName指向的buf，并返回pszProtocolName
char *MPU_DDNS_GetProtocolName(int nIndex, char *pszProtocolName)
{
	MPU_DDNS_GET(char *, pszProtocolName, DDNS_GetProtocolName(nIndex, pszProtocolName));
}

//!\brief   获取DDNS用户名
//!\param   pszUserName 返回用户名
//!\return  本接口拷贝DDNS用户名到pszUserName指向的buf，并返回pszUserName
char *MPU_DDNS_GetUserName(int nIndex, char *pszUserName)
{
	MPU_DDNS_GET(char *, pszUserName, DDNS_GetUserName(nIndex, pszUserName));
}

//!\brief   设置DDNS服务器名称
//!\return  0成功，1失败
int MPU_DDNS_SetDIYDDNSServer(char *pszUserName)
{
	MPU_DDNS_GET(int, -1, SetDIYDDNSServer(pszUserName));
}

//!\brief   获取DDNS服务器名称
//!\param   pszUserName 返回服务器名称
//!\return  0成功其他失败
int MPU_DDNS_GetDIYDDNSServer(char *pszUserName, const int len)
{
	MPU_DDNS_GET(int, -1, GetDIYDDNSServer(pszUserName, len));
}


//!\brief   设置DDNS用户名
//!\param   pszUserName 用户名
//!\return  0表示成功，其它为错误码
int MPU_DDNS_SetUserName(int nIndex, const char *pszUserName)
{
	MPU_DDNS_CALL(DDNS_SetUserName(nIndex, pszUserName));
}

//!\brief   获取DDNS密码
//!\param   pszPassword 返回密码
//!\return  本接口拷贝DDNS密码到pszPassword指向的buf，并返回pszPassword
char *MPU_DDNS_GetPassword(int nIndex, char *pszPassword)
{
	MPU_DDNS_GET(char *, pszPassword, DDNS_GetPassword(nIndex, pszPassword));
}

//!\brief   设置DDNS密码
//!\param   pszPassword 密码
//!\return  0表示成功，其它为错误码
int MPU_DDNS_SetPassword(int nIndex, const char *pszPassword)
{
	MPU_DDNS_CALL(DDNS_SetPassword(nIndex, pszPassword));
}

//!\brief   获取DDNS域名
//!\param   pszDomainName   返回域名
//!\return  
//本接口拷贝DDNS域名到pszDomainName指向的buf，并返回pszDom
//ainName
char *MPU_DDNS_GetDomainName(int nIndex, char *pszDomainName, int DomainNameSize)
{
	MPU_DDNS_GET(char *, pszDomainName, DDNS_GetDomainName(nIndex, pszDomainName, DomainNameSize));
}

//!\brief   设置DDNS域名
//!\param   pszDomainName   域名
//!\return  0表示成功，其它为错误码
int MPU_DDNS_SetDomainName(int nIndex, const char *pszDomainName)
{
	MPU_DDNS_CALL(DDNS_SetDomainName(nIndex, pszDomainName));
}

//!\brief   获取DDNS保活时间
//!\return  间隔时间，单位为秒
int MPU_DDNS_GetAutoUpdateTime(int nIndex)
{
	MPU_DDNS_GET(int, 0, DDNS_GetAutoUpdateTime(nIndex));
}

//!\brief   获取DDNS保活时间
//!\param   nIntervalSecond     间隔时间，单位为秒
//!\return  0表示成功，其它为错误码
int MPU_DDNS_SetAutoUpdateTime(int nIndex, int nIntervalSecond)
{
	MPU_DDNS_CALL(DDNS_SetAutoUpdateTime(nIndex, nIntervalSecond));
}

//!\brief   启动DDNS
//!\return  0表示开始启动，其它为错误码
//!         启动DDNS是一个异步过程，上层需调用MPU_DDNS_GetState获取状态
int MPU_DDNS_Start()
{
	MPU_DDNS_CALL(DDNS_Start());
}

//!\brief   停止DDNS
//!\return  0表示开始停止，其它为错误码
//!         启动DDNS是一个异步过程，上层需调用MPU_DDNS_GetState获取状态
int MPU_DDNS_Stop()
{
	MPU_DDNS_CALL(DDNS_Stop());
}

//!\brief   获取DDNS状态
//!\return  0表示已经停止，1表示正在启动，2表示已经启动，3表示正在停止
int MPU_DDNS_GetState()
{
	MPU_DDNS_GET(int, 0, DDNS_GetState());
}


/**************************************************************************
 *                            注销局部宏定义                              *
 **************************************************************************/

#undef MPU_DDNS_CALL			//调用DDNS管理器执行动作
#undef MPU_DDNS_GET				//调用DDNS管理器获取参数值
