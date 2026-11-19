/*!
* @file
*  ZWHosts.h 
* @brief
*  域名解析缓存管理模块
* @author
*  zw
* @date
*  2019年02月23日
* @version
*  1.0.0
*/

#ifndef ZW_HOSTS_H

#define ZW_HOSTS_H

#ifdef ZWHOSTS_EXPORTS

#define ZWHOSTS_API			extern "C"__declspec(dllexport)
#ifndef CALLSTACK
#define CALLSTACK			__stdcall
#endif

#elif defined(__cplusplus)

#ifdef _WIN32

#define ZWHOSTS_API			extern "C"__declspec(dllimport)
#ifndef CALLSTACK
#define CALLSTACK			__stdcall
#endif

#else

#define ZWHOSTS_API			extern "C" __attribute ((visibility("default")))
#ifndef CALLSTACK
#define CALLSTACK
#endif

#endif

#else

#define ZWHOSTS_API
#define CALLSTACK

#endif

#include <stdlib.h>

#include "ZWHostsCommon.h"

/*
配置文件为Ini格式，内容如下
[domain_0]
name=www.163.com
[domain_1]
name=www.sina.com.cn
[domain_2]
name=www.sohu.com
[timer]
interval=60
#interval单位为分钟
*/

/*!
* @brief
*	模块初始化接口
* @param config
*	配置文件路径（该配置文件为ZWHosts模块必须依赖的文件）
* @param hosts
*	系统域名缓存文件路径（根据系统内核的不同有：/etc/hosts等，具体请确认内核参数）
* @param resolve
*	系统DNS配置文件路径（例如：/etc/resolv.conf）
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	模块初始化接口，使用功能接口前，请先初始化模块
* @note
*	接口类型: 阻塞式
*/
ZWHOSTS_API int Hosts_Initialize(IN const char *config, IN const char *hosts, IN const char *resolve);

/*!
* @brief
*	模块配置更新接口
* @param list
*	域名信息列表
* @param param
*	域名解析的参数
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	使用功能接口前，请先初始化模块
* @note
*	接口类型: 阻塞式
*/
ZWHOSTS_API int Hosts_UpdateDomainList(IN const ZWHost_DOMAIN_INFO_LIST *list);

/*!
* @brief
*	模块释放接口
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	模块资源释放接口，应用退出时请释放模块资源，无需处理返回值
* @note
*	接口类型: 阻塞式
*/
ZWHOSTS_API int Hosts_Release(void);

/*!
* @brief
*	域名解析缓存更新接口
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	建议在网络接口及路由等网络配置变化时调用，该接口会更新域名解析缓存
* @note
*	接口类型: 阻塞式
*/
ZWHOSTS_API int Hosts_Update(void);

/*!
* @brief
*	设置域名解析缓存更新回调接口
*	仅在服务器域名有更新时回调
* @param cb
*	更新回调
* @param userdata
*	用户数据
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
ZWHOSTS_API int Hosts_SetUpdateCB(IN CB_HostsUpdate cb, IN void *userdata);

#ifndef XRS_OPERATOR
/*!
* @brief
*	设置域名列表管理服务器相关参数
* @param param
*	域名管理服务器的域名以及设备DID
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
ZWHOSTS_API int Hosts_SetDomainListManagerServer(ZWHost_DomainListManagerServerParam *param);

/*!
* @brief
*	获取域名的别名
* @param pDomain
*	需要获取别名的域名
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
ZWHOSTS_API int Hosts_AddDomainForGetAlias(char* pDomain);
#endif

/*!
* @brief
*	域名解析
* @param param
*	需要解析的域名
* @param param
*	域名解析的ip
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式,写hosts文件
*/
ZWHOSTS_API int Hosts_GetHostByDomain_Synchronize(char* pDomain, char *pHost);

/*!
* @brief
*	域名解析(采用多线程的方式，支持设置超时时间)
* @param list
*	需要解析的域名信息列表
* @param param
*	域名解析的参数
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式,写hosts文件
*/
ZWHOSTS_API int Hosts_GetHostByDomain_SynchronizeEx(IN ZWHost_DOMAIN_INFO_LIST* list, IN const ZWHOST_PARSE_PARAM * param);

/*!
* @brief
*	域名解析耗时(采用多线程的方式，支持设置超时时间, 最多支持8个DNS服务器)，AOV设备使用该接口时一定要确保该函数退出之后进入AOV，该接口内部不做AOV逻辑处理
* @param pDomain
*	要解析的域名
* @param list
*	DNS服务器列表(注意list下的nValidDNSServerCount字段的取值必须为有效的dns个数)
* @param timeout
*	单个域名解析的超时时间(单位是毫秒)
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式（线程安全）
*/
ZWHOSTS_API int Hosts_MultiThread_GetDomainResolutionTime(IN const char* pDomain, IN ZWHost_DNS_SERVER_INFO_LIST* list, int timeout);

/*!
* @brief
*	并发域名解析(采用多线程的方式，支持设置超时时间, 最多支持8个DNS服务器, 任意一个dns解析域名成功直接返回)，AOV设备使用该接口时，当为线程常驻模式时，内部已经处理了AOV的逻辑，当为非常驻模式时，需要确保该函数退出之后再进入AOV模式
* @param pDomain
*	要解析的域名
* @param nMode;												  	
*   多线程模式,0.创建新线程解析完成之后线程销毁;1.使用常驻线程（第一次域名解析时创建线程，创建完成之后常驻最多常驻8个线程，每个线程维护不同的dns服务器）
* @param list
*	DNS服务器列表
* @param timeout
*	单个域名解析的超时时间（单位是毫秒）
* @param pHost
*	域名解析的ip
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式（线程安全）
*/
ZWHOSTS_API int Hosts_MultiThread_GetHostByDomain(IN const char* pDomain, int nMode,IN ZWHost_DNS_SERVER_LIST* list, OUT char* pHost,IN int timeout); 

#endif