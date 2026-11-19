/*!
* @file
*  ZWHostsCommon.h
* \brief
*  域名解析缓存接口结构与类型定义
* @author
*  zw
* @date
*  2019年02月23日
* @version
*  1.0.0
*/

#ifndef ZWHOSTS_COMMON_H
#define ZWHOSTS_COMMON_H

//////////////////////////////////////////////////////////////////////////////
//宏定义
/////////////////////////////////////////////////////////////////////////////
//参数属性
/** \brief 入参																						*/
#ifndef IN
#define IN
#endif

/** \brief 出参																						*/
#ifndef OUT
#define OUT
#endif

/** \brief 入出参																					*/
#ifndef IN_OUT
#define IN_OUT
#endif

/** \brief 回调接口																					*/
#ifndef CALLBACK
#if defined(_WIN32)
#define CALLBACK			__stdcall
#else
#define CALLBACK					
#endif
#endif

//////////////////////////////////////////////////////////////////////////////
//枚举
/////////////////////////////////////////////////////////////////////////////
/*!
*  \enum	eHostsError
*  \brief	错误码
*/
typedef enum eHostsError
{
	eHostsNone 					= 0,					/*!< 成功							*/
	eHostsFailed				= 1,					/*!< 接口调用失败							*/
	eHostsParameter,									/*!< 输入参数错误							*/
	eHostsMemory,										/*!< 系统内存错误							*/
	eHostsHandle,										/*!< 系统句柄错误							*/
	eHostsThread,										/*!< 系统线程错误							*/
}eHostsError;


/*!
*  \enum	eDeviceRunMode
*  \brief	错误码
*/
typedef enum eDeviceRunMode
{
	eDeviceRunMode_ConstantElectricity				= 0,		/*!< 常电模式						*/
	eDeviceRunMode_lowPower,									/*!< 低功耗							*/
	eDeviceRunMode_PowerSaving,									/*!< 省电							*/
	eDeviceRunMode_AOV,											/*!< AOV							*/

}eDeviceRunMode;
///////////////////////////////////////////////////////////////////////////
//结构
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//回调接口
//////////////////////////////////////////////////////////////////////////
/*!
*  \brief
*	域名地址更新回调
*	每次回调一个更新的服务器信息
* @param domain
*	服务器域名
* @param ip
*	服务器地址
* @param userdata
*	用户数据
* @return
*	返回0
* @note
*	接口类型: 阻塞式
*/
typedef int(CALLBACK *CB_HostsUpdate)(IN const char *domain, IN const char *ip, IN void *userdata);


typedef struct tagZWHost_DOMAIN_INFO
{
	char cDomain[64];
	char cIPv4[16];
	int  nParseStatus;   //0.表示成功，非0表示失败
}ZWHost_DOMAIN_INFO;

typedef struct tagZWHost_DOMAIN_INFO_LIST
{
	int nCount;			//域名和IP 信息实际个数
	ZWHost_DOMAIN_INFO stDomainInfoList[32];  //域名信息列表
}ZWHost_DOMAIN_INFO_LIST;

typedef struct tagZWHOST_PARSE_PARAM
{
	int nParseThreadCount;  //解析线程个数，大于0且小于等于域名个数
	int nParseTimeout;    //单个域名解析的超时时间

	char cReserve[256];
}ZWHOST_PARSE_PARAM;

typedef struct tagZWHost_DomainListManagerServerParam
{
	char cDID[64];//设备DID
	char cDomainServer[128];//域名列表管理服务器域名
	int port;	//服务器端口号
	char cReserve[256];
}ZWHost_DomainListManagerServerParam;


typedef struct tagZWHost_DNS_SERVER_INFO
{
	char cDNSServer[256];//域名服务器
	char cIPv4[16];		// 域名对应的ip
	int  nParseTime;   // 域名解析耗时
	int  nParseStatus;   //0.表示成功，非0表示失败
}ZWHost_DNS_SERVER_INFO;

#define MAX_DNS_SERVER 8
typedef struct tagZWHost_DNS_SERVER_INFO_LIST
{
	ZWHost_DNS_SERVER_INFO stDNSServerInfoList[MAX_DNS_SERVER];  //DNS 服务器 解析信息列表
	int nValidDNSServerCount;	//有效DNS服务器个数
	char cReserve[128];
}ZWHost_DNS_SERVER_INFO_LIST;

typedef struct tagZWHost_DNS_SERVER
{
	char cDNSServer[256]; // 域名服务器

	char cReserve[128];
} ZWHost_DNS_SERVER;

typedef struct tagZWHost_DNS_SERVER_LIST
{
	ZWHost_DNS_SERVER stDNSServerList[MAX_DNS_SERVER];	// DNS服务器列表
	int nValidDNSServerCount;	//有效DNS服务器个数
	

	char cReserve[128];
} ZWHost_DNS_SERVER_LIST;

#endif

