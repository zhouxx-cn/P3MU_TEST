#ifndef __ZW_HTTP_CLIENT_COMMON_H
#define __ZW_HTTP_CLIENT_COMMON_H

/**************************************************************************
*                           系统环境		      							  *
************************************************************************* */
#if defined(_WIN32)||defined(WIN32)
#define SYSTEM_WIN32
#elif defined(_LINUX)||defined(LINUX)
#define SYSTEM_LINUX
#else
#error "undefined system!"
#endif


/**************************************************************************
*                           与系统相关的通用数据类型							  *
************************************************************************* */
#if defined(SYSTEM_WIN32)
#define ZWHTTPCLIENT_CALLBACK
#elif defined(SYSTEM_LINUX)
#define ZWHTTPCLIENT_CALLBACK					__stdcall
#else
#define ZWHTTPCLIENT_CALLBACK
#endif

typedef enum
{
    HTTP_METHOD_BEGIN  =  0,
    HTTP_POST,
    HTTP_GET,
    HTTP_PUT,
    HTTP_METHOD_END,
}eZWHTTP_METHOD;

typedef enum
{
    CMD_TYPE_BEGIN  =  0,
    CMD_TYPE_4G,
    CMD_TYPE_END,
}eZWHTTP_CMD_TYPE;

typedef struct tagServerInfo
{
    bool isHttps;
    char serverAddr[128];
    int nPort;
    char queryParam[256];
    int connectTimeOutMs;
    int waitResponseTimeOutMs;
}ServerInfo, *LPServerInfo;

typedef struct tagTransDataServerInfo
{
    bool isHttps;
    char serverUrl[256]; // 完整的url，例如：http://auth-test.p6sai.com:8091/mock/34/cloud/notify-open
    int connectTimeOutMs;
    int waitResponseTimeOutMs;
    char reserver[256];
}TransDataServerInfo, *LPTransDataServerInfo;

typedef struct tag4G_DEVICE_P2P
{
    char did[64];
    char pcMac[64];
    char deviceMac[64];
    int deviceType;
    char firmwareVersion[64];
    char sn[64];
    char reserver[64];
}HTTP_4G_DEVICE_P2P,*LP4G_DEVICE_P2P;

#endif