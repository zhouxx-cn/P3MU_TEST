#ifndef __ZW_HTTP_CLIENT_API_H__
#define __ZW_HTTP_CLIENT_API_H__

#include "ZW_HTTP_CLIENT_Common.h"
#include "ZW_HTTP_CLIENT_Configure.h"

#ifdef __cplusplus
#define ZW_HTTP_CLIENT_API	extern "C"   
#else
#define ZW_HTTP_CLIENT_API
#endif

#if defined(SYSTEM_WIN32)
#define ZWHTTPCLIENT_CALLBACK
#elif defined(SYSTEM_LINUX)
#define ZWHTTPCLIENT_CALLBACK					__stdcall
#else
#define ZWHTTPCLIENT_CALLBACK
#endif

/*!
* @brief
*	初始化http客戶端
* @param handle
*	输入参数，句柄
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式，同一个线程调用只需要init一次，多线程调用需要多次init
*/
ZW_HTTP_CLIENT_API int ZWHTTPCLIENT_Init(int *handle);

/*!
* @brief
*	去初始化http客戶端
* @param handle
*	输入参数，句柄
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式，与ZWHTTPCLIENT_Init配套使用，不调用会导致内存泄漏
*/
ZW_HTTP_CLIENT_API int ZWHTTPCLIENT_UnInit(int handle);

/*!
* @brief
*	获取http响应
* @param handle
*	输入参数，句柄
* @param serveInfo
*	输入参数，服务器信息
* @param eZWHTTP_METHOD
*	输入参数，http方法
* @param eZWHTTP_CMD_TYPE
*	输入参数，使用的模块，每个模块对应一个请求结构体和响应结构体
* @param reqBuffer
*	输入参数，结构体转的char指针，使用时会根据eZWHTTP_CMD_TYPE进行强转
* @param nBufferLen
*	输入参数，reqBuffer长度
* @param responseBuffer
*	输入参数，结构体转的char指针，使用时会根据eZWHTTP_CMD_TYPE进行强转
* @param nBufferLen
*	输入参数，repBufferLen长度
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
ZW_HTTP_CLIENT_API int ZWHTTPCLIENT_HttpVisitor(int handle, LPServerInfo pserveInfo, eZWHTTP_METHOD method, eZWHTTP_CMD_TYPE cmdType, char *reqBuffer, int nBufferLen, char *responseBuffer, int repBufferLen);

/*!
* @brief
*	获取http透传接口
* @param handle
*	输入参数，句柄
* @param serveInfo
*	输入参数，服务器信息
* @param eZWHTTP_METHOD
*	输入参数，http方法
* @param reqBuffer
*	输入参数，结构体转的char指针，使用时会根据eZWHTTP_CMD_TYPE进行强转
* @param nBufferLen
*	输入参数，reqBuffer长度
* @param responseBuffer
*	输入参数，结构体转的char指针，使用时会根据eZWHTTP_CMD_TYPE进行强转
* @param nBufferLen
*	输入参数，repBufferLen长度
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
ZW_HTTP_CLIENT_API int ZWHTTPCLIENT_HttpTransparentData(int handle, LPTransDataServerInfo pserveInfo, eZWHTTP_METHOD method, char *reqBuffer, int nBufferLen, char *responseBuffer, int repBufferLen);

/*!
* @brief
*	增加头,多个头，多次调用
* @param handle
*	输入参数，句柄
* @param header
*	输入参数，头指针，必现包含完整头信息，例如Content-Type:application/json
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
ZW_HTTP_CLIENT_API int ZWHTTPCLIENT_HttpAddHeader(int handle, char *header);

/*!
* @brief
*	初始化curl
* @return
*	无返回值
* @note
*	接口类型: 阻塞式，同一个进程调用只需要init一次，多线程调用需要一次init
*/
ZW_HTTP_CLIENT_API void ZWHTTPCLIENT_CurlInit();

/*!
* @brief
*	去初始化curl
* @return
*	无返回值
* @note
*	接口类型: 阻塞式，与ZWHTTPCLIENT_CurlInit配套使用，不调用会导致内存泄漏
*/
ZW_HTTP_CLIENT_API void ZWHTTPCLIENT_CurlUnInit();

/*!
* @brief
*	获取curl状态
* @return
*	返回0表示未初始化，1表示已经初始化
* @note
*	接口类型: 非阻塞式
*/
ZW_HTTP_CLIENT_API int ZWHTTPCLIENT_CurlStatus();
#endif