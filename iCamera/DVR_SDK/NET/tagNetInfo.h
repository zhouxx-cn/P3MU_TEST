#ifndef __TAG_NET_INFO_H__
#define __TAG_NET_INFO_H__

#pragma once

/* wifi模式 */
typedef enum
{
    NO_WIFI_DEV   = 0,
    WLAN_STA_MODE = 1,
    SOFT_AP_MODE  = 2,
    UNKNOWN_MODE  = 3,
}eWifiMode;

/* 发送HTTP的类型 */
typedef enum {
    HTTP_EVENT_SEND    = 0,
    MOTION_HTTP_SEND   = 1,
    SEND_TYPE_NUM
}E_HTTP_SEND_TPYE;

/* Http Server配置信息 */
typedef struct tagHttpServerInfo{
    int  bHttpServerEnable;         // Http Server使能
    char szHttpServerName[64];      // Http Server名字
    char szHttpServerHostURL[256];  // Http Server Host URL
    char szHttpServerUserName[64];  // Http Server用户名
    char szHttpServerPassword[64];  // Http Server用户名
}stuHttpServerInfo;

/* Event配置信息 */
typedef struct tagEventCfgInfo{
    int  nPostRecordTime;           // 触发的预录时间
    int  nPostAlarmInterval;        // 发送报警的时间间隔
    char szHttpServerMessage[256];  // Http Server发送消息
}stuEventCfgInfo;


typedef struct tagDOMAIN_INFO
{
	char szDomain[64];
	char szIP[16];
}DOMAIN_INFO;


typedef struct tagP2P_CONNECT_INFO
{
	char szPlatformName[16];
	char szConnectString[256];
	char szAuthWord[32];
	DOMAIN_INFO struDomain[8];
}P2P_CONNECT_INFO;

typedef struct tagP2P_TRANSPARENT_INFO_PUSHDOMAIN
{
    char acDomainSuffix[64];                       /* 域名后缀*/
}P2P_TRANSPARENT_INFO_PUSHDOMAIN;

typedef struct tagDOMAIN_INFOV2
{
	char cDomain[64];
	char cIPv4[16];
}NET_DOMAIN_INFO;


typedef struct tagDOMAIN_INFO_LIST
{
	int nCount;			//域名和IP 信息实际个数
	NET_DOMAIN_INFO stDomainInfoList[32];  //域名信息列表
}NET_DOMAIN_INFO_LIST;



typedef struct tagTransParent_INFO
{
	char cDomainServer[128];//域名列表管理服务器域名
	char acDomainSuffix[64]; //域名后缀
	int port;	//服务器端口号
	
}TransParent_INFO;


#endif // end of __TAG_NET_INFO_H__
