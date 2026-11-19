// Copyright (C) 1991 - 1999 Rational Software Corporation

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_INET_49B634E70217_INCLUDED
#define _INC_INET_49B634E70217_INCLUDED
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>

#include "global_macro.h"
#include "tagAlertInfo.h"
#include "tagActionInfo.h"
#include "tagNetInfo.h"
#include "wifi_api.h"

#if defined (__USE_FTP__)
//#include "ftp_process.h"
#include "ftpClient.h"
#endif
#include "newhie/ZWP2PPUSHVNClient.h"
#include "newhie/HieServer.h"

#ifdef USE_HOSTS_PARSER
#include "newhie/ZWHosts.h"
#include "newhie/ZWHostsCommon.h"
#endif

#ifdef __MODULE_4G_SUPPORT__
#include "../../modules/GPRS/include/4g_mode.h"
#endif

#ifdef __IS_SUPPORT_AIC_BLUETOOTH__
#include "XRSBLE_L2.h"
#endif

const int MSG_NET_LOCAL_NAME = 0x5000;

typedef enum{
    ADD_WIFI_ROUTE,
    DEL_WIFI_ROUTE,
    ADD_NET_ROUTE,
    DEL_NET_ROUTE,
    ADD_WIFI_SPECIAL,
    ADD_NET_SPECIAL
}eROUTESETTING;
typedef enum{
    WIFI_MODULE_NONE,
    WIFI_MODULE_AIC_8800DL,
    WIFI_MODULE_HI_H383U,
    WIFI_MODULE_ATBM_6132U,
    WIFI_MODULE_AIC_8800DL_2_4G,
}eWifiModuleType;


typedef struct tagNetCardInfo
{
    int nBluetoothIsExist;  // 0不带蓝牙  1带蓝牙
    int nSparklinkIsExist;  // 0不带星闪  1带星闪
    int nNetworkType;       // BIT0 有线，BIT1 WIFI，BIT2 4G
    int nWIFIband;          // 0 单频，1双频  ，纯有线和纯4G可以在这里设置为0
    char WIFIspeed1[8];     // 2.4G，5G，6G 默认为空
    char WIFIspeed2[8];     // 2.4G，5G，6G  双频填写，单频不填写。默认为空
}NetCardInfo;

typedef enum{
    btProTestTrue,
    btProTestfalse
}eBTPROTEST;

/* wifi配网模式 */
#if defined (__IS_SUPPORT_WLAN__)
typedef enum
{
	eWifiCfgMode_Normal = 0,	//常规
	eWifiCfgMode_SdCard = 1,	//SD卡配网
}eWifiCfgMode;

typedef enum
{
	noToolRun,
	STAToolRun,
	APToolRun,
}eWifiToolStatus;
#endif

typedef enum{
    btGeneral,
    btProTest
}BlueMode;

typedef struct stTutkCfg
{
    int bEnable;
    char szTutkUID[MAX_DID_LENGTH];
    char szPushID[MAX_PUSH_ID_LENGTH];
    int iErrCode;
    int iStatus;
}TutkCfg;


//auth p2p信息
typedef struct
{
    //P2P信息有效
    char bValidP2pInfo;
    //是否支持P2PHDR，0-不支持，1-支持
    int iP2PHDR;
}Net_AuthP2pInfo;


typedef struct dns_thread_param
{
    pthread_t *pThrID;       // 线程ID指针
    int count;               // 计数标记 不为0表示更新
    char primaryDNS[32];     // 主DNS
    char secondaryDNS[32];   // 备用DNS
    char gateway[32];        // 网关
}DNSThrParam;//DNS检测线程结构体


//关于DHCP功能脚本，见本文档末端

//!\brief   Net模块     本地网卡设置接口类
//##ModelId=49BB426A02BB
class CConfig;

class INet
{
  public:
	//!\brief   获取唯一实例
	//!\return  实例指针
	//##ModelId=49BDB4D20317
	static INet *GetInstance();
	//!\brief   销毁实例
	//##ModelId=49BDB7CE0199
	static void Release();

	//!\breif  防止XP系统不能重连 
	int NET_arp_update_neighbors();
	
	//!\brief   初始化
	//!\return 0-成功;其它为错误码
	int Init();

	char * GetDevName(int num, char *DevName);
	//!\brief   释放资源
	//!\return 0-成功;其它为错误码
	int UnInit();

	int NET_GetDhcpEnableEx();

	//!\brief   获取是否启用DHCP
	//!\return  true表示启用，false表示不启用
	//##ModelId=49BB426A02CF
	int NET_GetDhcpEnable();

	int NET_SetDhcpEnableEx(int bEnable, int *dhcpChanged);

	//!\brief   设置是否启用DHCP
	//!\param   bEnable     true表示启用，false表示不启用
	//!\return  0表示成功，其它为错误码
	//##ModelId=49BB426A02D0
	int NET_SetDhcpEnable(int bEnable);

	//!\brief 兼容不同的路由器在从新获取DHCP切换ip
	//!\param pNetCard 网卡类型 command 输出组合的命令
	//!\return 合法0,不合法其他值
	int DhcpIPFixed(const char *pNetCard,char *command, int size);

    int enableDhcpOperation(int enable, const char *pNetCard);

	//!\brief   手动更新DHCP
	//!\return  0表示成功，其它为错误码
	//##ModelId=49BDBBB00207
	int NET_RenewDhcp();

	//!\brief   获取IP地址
	//!\param   pszIPAddress    返回IP地址（例 172.1.10.2）
	//!\return  本接口拷贝IP地址到pszIPAddress指向的buf，并返回pszIPAddress
	//##ModelId=49BB426A02D2
	char *NET_GetIPAddress(char *pszIPAddress,int addressSize = MAX_IP_STR_LEN);

	char *NET_GetIPAddressEx(char *pszIPAddress,int addressSize = ADDRESS_DEFAULT_SIZE);

	//!\brief   设置IP地址
	//!\param   pszIPAddress    IP地址（例 172.1.10.2）
	//!\return  0表示成功，其它为错误码,需重激活才生效
	//##ModelId=49BB426A02D4
	int NET_SetIPAddress(const char *pszIPAddress);

	//只保存ip地址
	int NET_SetIPAddressEx(const char *pszIPAddress, int *ipChanged);

	char *NET_GetSubnetMaskEx(char *pszSubnetMask,int SubnetMaskSize = ADDRESS_DEFAULT_SIZE);

	//!\brief   获取子网掩码地址
	//!\param   pszSubnetMask   返回子网掩码（例 255.255.0.0）
	//!\return  本接口拷贝子网掩码到pszSubnetMask指向的buf，并返回pszSubnetMask
	//##ModelId=49BB426A02D6
	char *NET_GetSubnetMask(char *pszSubnetMask, int MaskSize = ADDRESS_DEFAULT_SIZE);

	int NET_SetSubnetMaskEx(const char *pszSubnetMask, int *subMaskChanged);

	//!\brief   设置子网掩码地址
	//!\param   pszSubnetMask   子网掩码（例 255.255.0.0）
	//!\return  0表示成功，其它为错误码,需重激活才生效
	//##ModelId=49BB426A02D8
	int NET_SetSubnetMask(const char *pszSubnetMask);

	//!\brief   获取网关地址
	//!\param   pszGateway      返回网关地址（例 172.1.10.1）
	//!\return  本接口拷贝网关地址到pszGateway指向的buf，并返回pszGateway
	//##ModelId=49BB426A02DA
	char *NET_GetGateway(char *pszGateway, int gateWaySize = ADDRESS_DEFAULT_SIZE);

	//!\brief   设置网关地址
	//!\param   pszGateway      网关地址（例 172.1.10.1）
	//!\return  0表示成功，其它为错误码,需重激活才生效
	//##ModelId=49BB426A02DC
	int NET_SetGateway(const char *pszGateway);	

	//!\brief   获取HTTP Server端口
	//!\return  端口号（例：80）,0表示失败
	//##ModelId=49BB426A02DE
	unsigned int NET_GetHttpPort();

	//!\brief   设置HTTP Server端口
	//!\param   usPort          端口号（例：80）
	//!\return  0表示成功，其它为错误码;设置成功即生效
	//##ModelId=49BB426A02DF
	int NET_SetHttpPort(unsigned int usPort);

	int NET_SetCommandPort(unsigned int usPort);
	
	unsigned int NET_GetCommandPort();
	
	int NET_SetMediaPort(unsigned int usPort);
	
	unsigned int NET_GetMediaPort();
	
	int NET_SetRtspPort(unsigned int usPort);
	
	unsigned int NET_GetRtspPort();
	
	int NET_SetAutoDetectPort(unsigned int usPort);
	
	unsigned int NET_GetAutoDetectPort();
	
	int NET_SetMobilePort(unsigned int usPort);
	
	unsigned int NET_GetMobilePort();

	int NET_SetHKPrivatePort(unsigned int usPort);

	unsigned int NET_GetHKPrivatePort();

	//!\brief   获取Mac地址
	//!\param   pszMacAddress   返回Mac地址（例 00-1A-4D-2B-91-64）
	//!\return  本接口拷贝Mac地址到pszMacAddress指向的buf，并返回pszMacAddress
	//##ModelId=49BB426A02E1
	char *NET_GetMacAddress(char *pszMacAddress, int macSize = ADDRESS_DEFAULT_SIZE);

	//!\brief 设置联动动作
	//!\param nType 类型
	//  !      ACTION_BEEP 声音提示
	//  !      ACTION_SENDTOSERVER 上传报警服务器
	//  !      ACTION_EMAIL 联动Email
	//  !      ACTION_SMS 联动短信   
	//  !      ACTION_ALARMOUT 报警输出
	//!\param int bEnable 如果是报警输出则为端口号掩位,其他表示使能
	int NET_SetIPAction(int nType, int bEnable);

	//!\brief 获取联动动作
	//!\param nType 类型
	//  !      ACTION_BEEP 声音提示
	//  !      ACTION_SENDTOSERVER 上传报警服务器
	//  !      ACTION_EMAIL 联动Email
	//  !      ACTION_SMS 联动短信   
	//  !      ACTION_ALARMOUT 报警输出
	//!\return 返回是否设置，如果是报警输出则返回端口号掩位
	int NET_GetIPAction(int nType);

	//!\brief 设置联动动作
	//!\param nType 类型
	//  !      ACTION_BEEP 声音提示
	//  !      ACTION_SENDTOSERVER 上传报警服务器
	//  !      ACTION_EMAIL 联动Email
	//  !      ACTION_SMS 联动短信   
	//  !      ACTION_ALARMOUT 报警输出
	//!\param int bEnable 如果是报警输出则为端口号掩位,其他表示使能
	int NET_SetPHYAction(int nType, int bEnable);

	//!\brief 获取联动动作
	//!\param nType 类型
	//  !      ACTION_BEEP 声音提示
	//  !      ACTION_SENDTOSERVER 上传报警服务器
	//  !      ACTION_EMAIL 联动Email
	//  !      ACTION_SMS 联动短信   
	//  !      ACTION_ALARMOUT 报警输出
	//!\return 返回是否设置，如果是报警输出则返回端口号掩位
	int NET_GetPHYAction(int nType);

	//!\brief 获取网络状态
	//!\return  <-1-错误;0-正常;1-断开;2-未知状态；3-IP冲突;4-IP更新
	int NET_GetStatus();
	
	int NET_SetP6SMSGServerStatics(int PushMode,int PushStatus);

	int NET_SetTutkUIDEnable(int bEnable);

	int NET_GetTutkUIDEnable();

	int NET_SetTutkUID(TutkCfg *pTutkCfg, int pSetMethod);

	char *NET_GetTutkUID(TutkCfg *pTutkCfg, unsigned long *pTutkStatus);

#if defined (__AUTH_V2__)
	int NET_SetConnectString(P2P_CONNECT_INFO *pConnectString, const unsigned char *pszAuthLicense);
	
	int NET_GetConnectString(P2P_CONNECT_INFO *pConnectString);
#endif

    int NET_SetAuthInfoPushDomainSuffix(P2P_TRANSPARENT_INFO_PUSHDOMAIN *pPushDomainParam);

    int NET_GetAuthInfoPushDomainSuffix(P2P_TRANSPARENT_INFO_PUSHDOMAIN *pPushDomainParam);

	int NET_SetConnectStringOnly(HY_P2P_CONNECT_INFO pConnectString);

	int NET_SetTutkStatus(int *pTutkStatus);

    int NET_GetTutkStatus(int *pTutkStatus);

    //!\breif ULK相关配置接口
    //!\return 0-OK,其他-错误
	int NET_SetUlkUIDEnable(int bEnable);

	int NET_GetUlkUIDEnable();

	int NET_SetUlkUID(char bEnable, const char *pUlkUID);

    int NET_SetUUID(char *pUlkUID);

	char *NET_GetUlkUID(char *bEnable, char *pUlkUID, int *UlkStatus);

	int NET_GetP2PDownloadURL(char *pDownloadURL);
		
	int NET_SetUlkStatus(int *pUlkStatus);

	int NET_SetApplyDIDRequestUrl(char *pRequestUrl);

    /* 检测当前网络是否可以连接互联网(conn_status=1表示可以联通互联网) */
    int Net_CheckIfNetAccessToInternet(int *conn_status);

	int SetIpFromSN(int *dat);

	int SetIpRand(const char *hostIp);

	int NET_IfIPCConnected(int *connected);

	int NET_ifChangeNet(const char *pszIPAddress);

	int NET_SetIPAddressByNVR(const char *pszIPAddress);

	int NET_IPAddressIfSameWithNvr(const char *pszIPAddress, int *same);

	int NET_SetIPAddressAdjustEnable(int enable);
	
	int NET_GetIPAddressAdjustEnable(int *enable);

#if defined (__USE_FTP__)
    /* 设置用户的ftp参数 */
    int NET_SetFtpParam(const pFTP_user_param pFtpUsrParam);

    /* 获取用户设置的ftp参数 */
    int NET_GetFtpParam(pFTP_user_param pFtpUsrParam);

    /* 获取当前的FTP主机连接状态(成功return 1；失败 return 0;) */
   	int NET_GetFtpConnStatus();

	int NET_GetFtpTransferType(int *p_transfer_type);

	int NET_SetMotionCallBack(int (*CallBack) (int));
#endif

    /* 获取当前哪一个网卡有效 */
    int NET_GetCurrNetCardType(int *pCurrNetCardType);

	void NET_InitAuthP2pInfo();

	int NET_NeedUpdateRouter(void);
	/* 获取当前用的DID是否支持Subkey */
    int Net_IsSupportSetSubkeyByDID(void);

    /* 设置推送AI参数 */
    int Net_SetPushAiParam(const char *pPushSubKey);

    /* 获取推送AI参数 */
    int Net_GetPushAiParam(char *pPushSubKey);

#if defined (__IS_SUPPORT_P6SMESSAGE__)
	// 获取云消息URL
	int NET_GetMpServerParamCfg(char *ServerIpAndPort, char *UrlPathTimeCheck, char *UrlPathEventPolicy, char *UrlPathEventPost, char *UrlPathSetXmlStatus, char *SrvMaintenance);

	// 设置云消息URL
	int NET_SetMpServerParamCfg(char *ServerIpAndPort, char *UrlPathTimeCheck, char *UrlPathEventPolicy, char *UrlPathEventPost, char *UrlPathSetXmlStatus, char *SrvMaintenance);
#endif

#ifdef __MODULE_4G_SUPPORT__
	/*设置开始执行产测流程*/
	int Net_SetProductTest(int isTest);

	/*获取4G测试数据*/
	int Net_Get4GProductTestData(Module4G_ProductTest_Data_t *pData);

	/*设置4G模块配置*/
	int Net_Set4GControlConfig(HY_DVR_4G_CARD_SELECT_CFG_INFO *pcfg);

	/*获取4G模块配置*/
	int Net_Get4GControlConfig(HY_DVR_4G_CARD_SELECT_CFG_INFO *pcfg);

	/*获取4G切卡结果*/
	int Net_Get4GSwitchResult(HY_DVR_4G_CARD_SELECT_RESULT_INFO *pcfg);


	/*获取4G卡信息*/
	int Net_Get4GCardInfo(int nSim,int *nCardType,char *Iccid,int *nCardStatus,char *szOperator);

	/* 获取4G模组信息 */
	int Net_Get4GModuleInfo(char *Version,int *iSignal4G);

	//获取uid
	int NET_P2P_ProductTestGetUID(char *pUid);

	//获取pcmac
	int NET_P2P_ProductTestGetPcMac(char *pMac);

	/* 获取当前是否是p2p产测模式 */
	int Net_IsP2PProductTestMode(void);
	
	/*是否需要连接产测服务器*/
	int SetConnServerStatus(int);

    /*设置扫描完成状态*/
    int SetScanEndStatus(int scanend);

    /*设置userid*/
    int SetUserId(char *userid);

    /*设置上传服务器地址*/
    int SetServerAddress(char *serveraddress);

    /*设置随机值*/
    int SetRandomNumber(char *randomnumber);
#endif 


	//!\brief   设置网络状态通知回调
	//!\param   pCallback           回调函数指针
	//!\param int AlarmType    报警类型
	//!\return  0表示成功，其它为错误码
	//pAlertInfo.AlertType = IP冲突类型;
	//pAlertInfo.nChannel = 0;
	//pAlertInfo.dwReserved = (0表示IP冲突,1表示IP更新)

	//pActionInfo.tActionType = 报警类型,ACTION_BEEP,ACTION_SENDTOSERVER,ACTION_EMAIL,ACTION_SMS,ACTION_ALARMOUT
	//pActionInfo.dwReserved = <IP冲突: 冲突主机MAC地址指针,6字节>
	//pActionInfo.nChannel = 报警输出时表示端口号

	static int NET_SetIPCallBack(int AlarmType, int (*CallBack) (struct tagAlertInfo * pAlertInfo, struct tagActionInfo * pActionInfo));

	//!\brief   设置网络状态通知回调
	//!\param   pCallback           回调函数指针
	//!\param int AlarmType    报警类型
	//!\return  0表示成功，其它为错误码
	//pAlertInfo.AlertType = 物理断开类型;
	//pAlertInfo.nChannel = 0;
	//pAlertInfo.dwReserved = (0表示网线断，1表示网线重连)

	//pActionInfo.tActionType = 报警类型,ACTION_BEEP,ACTION_SENDTOSERVER,ACTION_EMAIL,ACTION_SMS,ACTION_ALARMOUT
	//pActionInfo.dwReserved = 0
	//pActionInfo.nChannel = 报警输出时表示端口号

	static int NET_SetPHYCallBack(int AlarmType, int (*CallBack) (struct tagAlertInfo * pAlertInfo, struct tagActionInfo * pActionInfo));

  public:
	//////////////////////////////////////////////////////////////////////////
	//以下外部一般不要调用

	//!\brief 重新激活配置
	//!\return 0成功，-1失败
	int NET_ReActive();

	//!brief 检测网络状态
	//!\return <-1-错误;-1-不检测;0-正常;1-断开;2-未知状态；3-IP冲突;4-IP更新
	int NET_CheckStatus();

    //!\brief 检查设备P2P状态是否异常
    //!\return 正常返回0，异常返回1
    int checkP2PStatusAbnormal(); 

	int NET_SetAuthP2pInfo(Net_AuthP2pInfo *ptAuthP2pInfo);

	int NET_GetAuthP2pInfo(Net_AuthP2pInfo *ptAuthP2pInfo);
	int NET_GetWiFiModule();
	int NET_GetWiFiUSBWifiDev();

	int TcpRelay_GetEnable(int *bEnable);
	int TcpRelay_SetEnable(int *bEnable);

	//申请DID
	char m_RequestUrl[MAX_REQUEST_URL_LEN];
    int m_RequestUrlPort;
	int m_iApplyFromSFlag;
	int m_iApplyStatus;//当前did的获取状态(0，ok；1，正在获取DID；2,网络异常；3，服务器拒绝；4，失败)
	int m_iApplyErrCode;//服务器错误码
    int m_TutkUID_Enable;
    
  protected:
	//##ModelId=49BB426A02C5
	  INet();

	//##ModelId=49BB426A02C6
	 ~INet();

  private:
	//##ModelId=49BDB4D202C8
	static INet *m_instance;	//!< 唯一实例指针

	//是否使能DHCP
	int m_bDHCP_Enable;

	//是否使能DHCP
	int m_bDHCP_connect_forbid;

	//是否开启了IP自适应
	int m_bIP_Adjust_Enbale;

    int m_ifRouteDeleted;

	//是否使能IP冲突
	bool m_iPconf_Enable;

	//本地IP地址(0-静态;1-DHCP)
	char m_sLocalIP[MAX_IP_STR_LEN];

	//本地网关地址
	char m_sGateWay[MAX_IP_STR_LEN];

	//子网掩码
	char m_sSubNetMask[MAX_IP_STR_LEN];

	//MAC地址
	char m_sMAC_addr[MAX_MAC_STR_LENGTH];

	//冲突MAC地址
	char m_sOtherMAC_addr[MAX_MAC_STR_LENGTH];

	//本地IP地址,用于检查IP是否更新
	char m_sLocalIP_old[MAX_IP_STR_LEN];

	char m_sTutkUID[MAX_UID];
	int m_supportSetDid;

	// ULK配置接口
	int m_UlkUID_Enable;
	char m_sUlkUID[MAX_UID];
	int m_iUlkStatus;
	
	//HTTP端口
	unsigned int m_uiHttpPort;
	unsigned int m_uiCommandPort;
	unsigned int m_uiMediaPort;
	unsigned int m_uiRtspPort;
	unsigned int m_uiMobilePort;
	unsigned int m_uiAutoDetectPort;
    //for HK
	unsigned int m_uiHKPrivatePort;

	char m_P2PDownloadURL[256];	

#if defined (__USE_FTP__)

    //FTP相关参数
    FTP_user_param m_FtpUsrInfo;
    unsigned int   m_FtpConnStatus;

#endif

	char  m_szPushSubKey[64];	 // 推送2.0的设备的DID号
	
	int m_bIsSuportSubKey;		 // 当前DID是否支持SubKey

	//录像联动类型
   	int m_nLinkageType;//0:ftp 1:sd 2:samba
	
    //当前有效的的网卡(0:unknown 1:eth0 2:eth1 3:wlan 4:3G 5:4G 6:5G)
    int m_nCurrNetCardType;

	//设置IP锁
	pthread_mutex_t m_setIPLock;

    //Adjust DHCP锁
    pthread_mutex_t m_IpDhcpLock;

	//设置文件锁
	pthread_mutex_t m_FileLock;

	pthread_mutex_t m_HTMLLock;

	pthread_mutex_t m_OperateRouteLock;

	pthread_mutex_t m_SetWifiConnectLock;

	pthread_mutex_t m_OperateDhcpLock;

	//报警类型
	static int m_IPAlarmType;

	//报警类型
	static int m_PHYAlarmType;
	
	//!\brief   网络状态通知回调函数指针
	//pAlertInfo.AlertType = IP冲突类型;
	//pAlertInfo.nChannel = 0;
	//pAlertInfo.dwReserved = (0表示IP冲突,1表示IP更新)

	//pActionInfo.tActionType = 报警类型,ACTION_BEEP,ACTION_SENDTOSERVER,ACTION_EMAIL,ACTION_SMS,ACTION_ALARMOUT
	//pActionInfo.dwReserved = <IP冲突: 冲突主机MAC地址指针,6字节>
	//pActionInfo.nChannel = 报警输出时表示端口号
	//!\return  0表示成功，其它为错误码
	//##ModelId=49BDD2880225
	//状态通知回调
	static int (*m_FxnCallBack_IP) (struct tagAlertInfo * pAlertInfo, struct tagActionInfo * pActionInfo);
	//!\brief   网络状态通知回调函数指针
	//pAlertInfo.AlertType = 物理断开类型;
	//pAlertInfo.nChannel = 0;
	//pAlertInfo.dwReserved = (0表示网线断，1表示网线重连)

	//pActionInfo.tActionType = 报警类型,ACTION_BEEP,ACTION_SENDTOSERVER,ACTION_EMAIL,ACTION_SMS,ACTION_ALARMOUT
	//pActionInfo.dwReserved = 0
	//pActionInfo.nChannel = 报警输出时表示端口号
	//!\return  0表示成功，其它为错误码
	//##ModelId=49BDD2880225
	//状态通知回调
	static int (*m_FxnCallBack_PHY) (struct tagAlertInfo * pAlertInfo, struct tagActionInfo * pActionInfo);

	//动作类型
	//声音
	int m_bBeep[2];
	//报警中心
	int m_bSTS[2];
	//Email
	int m_bEmail[2];
	//短信
	int m_bSMS[2];
	//报警输出
	int m_bDOP[2];
	//屏幕提示
	int m_bGUI[2];
	//录像
	int m_bRecord[2];
	
	int m_bPush[2];
	int m_bLightAlarm[2];

	//配置文件
	CConfig *m_pCConfig;

	//网络状态
	int m_iNetStatus;

	//前一网络状态
	//  int  m_iNetStatus_last;

	//DHCP是否失败
	int m_iNetDHCPFailed;

	//DHCP是否失败
	int m_iNetDHCPFailedTimes;

	//
	int m_iNetDHCPEnableShadow;

	//push初始化线程
	pthread_t m_hPushInitThread;

	//状态检测线程
	pthread_t m_hCheckThread;

	//IP自适应检测线程
	pthread_t m_IPAdjustCheckThread;

	//IP自适应处理线程
	pthread_t m_IPAdjustProcessThread;

    //UPNP检测线程
	pthread_t m_UpnpCheckThread;

    //路由操作线程
    pthread_t m_OperateRouterThread;
    //wifi重连线程
	#if defined (__IS_SUPPORT_WLAN__)
    pthread_t m_WlanReconnectThread;
	pthread_t m_WlanToolDetectionThread;
	pthread_t m_SendWifiTestCommandThread;
	pthread_t m_wifiApStaStatusCheckThread;
	#endif

	#ifdef __IS_SUPPORT_AIC_BLUETOOTH__
	pthread_t m_btProTestFunc;
	#endif
	
    //FTP操作线程
    pthread_t m_FtpOperateThread;

	//申请DID线程
	pthread_t m_ApplyDID;

    //当用户点击测试wifi之后发送post消息
    sem_t m_semWakeThread;

    //测试wifi正在进行
    int m_bWifiTestIsBusy;

    //wifi测试的参数是否已经设置了
    int m_bWifiTestHasSet;

	//状态检测线程是否退出
	int m_bCheck_exit;

    //推送模块是否初始化
	int m_bZWPushInit;

	//ARP包缓冲
	unsigned char *m_pArpPacket;

	//ARP包缓冲大小
	int m_iArpPacketSize;

	//ARP应答接受缓冲
	char *m_pArpReplyBuff;

	//ARP应答接受缓冲大小
	int m_iArpReplyBuffSize;

	int m_wifiIfExist;

    //是否下发域名后缀
    int m_acDomainSuffixExist;

    // 授权时下发的域名后缀
    char m_acDomainSuffix[64];
	
    //是否下发域名后缀_产测使用
    int m_acDomainSuffixExist_Test;

    // 授权时下发的域名后缀_产测使用
    char m_acDomainSuffix_Test[64];

	// 是否开启TCP转发功能
	int TcpRelayEnable;

	//是否YL号段
	int m_YLDid;
public:
	//探测资源锁
	int firstSameIP;
	pthread_mutex_t m_detectResourceLock;
	pthread_mutex_t m_writeLock;
	char detectQueue[DETECT_MAX_NUM][IP_MAX_LEN];
	int detectNum;

private:
	//导入缺省值
	int LoadParam();

    //恢复出厂网络配置,配合升级
    void Update_Recover(); 


	int IsIpConflict(int checkEnable);
	int ArpSend(int m_sock, const char *ifname, unsigned char srcHWAddr[6], unsigned int destIp);
	int IsResponseFromOthers(const char *buff, const char *localHWAddr);
	//!\brief 测试端口是否有效
	//!\param port 指定测试端口
	//!\return 0表示无效，1有效
	int TestPort(unsigned int port);

	//!\brief 检查IP是否合法
	//!\param ipaddr IP地址
	//!\return 合法1,不合法0
	int IsValidIP(const char *ipaddr);

	int checkMask(const char *pszSubnetMask);

	//!\brief 检查GW是否合法
	//!\param ipaddr IP地址
	//!\return 合法1,不合法0
	int IsValidGW(const char *ipaddr);

	//!\brief 检查IP最后位是否为255或者0
	//!\param ipaddr IP地址
	//!\return 合法1,不合法0
    int IsValidIpAddr(const char *ipaddr);

	//!\brief得到命令为CmdName的进程号
	//!\param char *CmdName为进程的命令号
	//!\return 正确返回进程号，错误返回－1
	int Get_UID(const char *CmdName);

    //!\brief 加载授权信息
    int Net_LoadAuthPushParam();

public:
	int WLAN_LoadParam();
	int WLAN_SetConnection(LPWLAN_CONNECTION pConnection, int force=0);
	int ETH0_GetDNS(char *PrimaryDNS, char *SecondaryDNS);

	int ETH0_SetDNS(char *PrimaryDNS, char *SecondaryDNS);

	int NETCARD_GetDhcpSuccTime(int	*nEth0DhcpSuccTime, int *nWlan0DhcpSuccTime);

	int WriteDNSFile(const char *pDNS, const char *pSNS, const char *pGateWay);

	//!brief 路由表操作线程
    void* OperateRouterThreadProcess();
#if defined (__IS_SUPPORT_WLAN__)
	/* 设备重连线程 */
    void* WlanReconnectThreadProcess();
	/* wifi小工具检测线程 */
	void* WlanToolDetectionThreadProcess();
    //监控wifi测试恢复配置线程
    void* SendWifiTestCommandThreadProcess(void);

public:
	int UdpPushDid();

	int switchNetCardOperate(int isWire);

  	int WLAN_SetConnectionEx(LPWLAN_CONNECTION pConnection);

	int WLAN_SendTestConfigure(LPWLAN_CONNECTION pConnection);

	int WLAN_GetWifiTestStatus();

	int WLAN_IsConfigureNetIng();

	int WLAN_SaveQrConfigureWlanSessionID(char *pszData);

	int WLAN_GetQrConfigureWlanSessionID(char *pszData, int nBufLen);

	//设置wifi配网状态:0-等待配网 1-连接wifi中 2-wifi连接成功(wifi连接失败，重新返回等待配网)
	int WLAN_SetWifiLinkStatus(int nNewStatus);

	int WLAN_GetWifiLinkStatus(int *pnStatus);

	int NET_SetWlanScanStatue(int *pWlanScanStatus);

	int NET_GetWlanScanStatue();

	int WLAN_SetClientAddrInfo(unsigned int u32Ip, unsigned int u32Port);

	/* 获取当前wifi是STA模式还是AP模式*/
	int NET_GetCurrWifiMode(int *pCurrWifiMode);

    /* 获取当前配网模式是常规配网模式还是SD卡配网模式 */
    int NET_GetCurrWifiCfgMode(int *pCurrWifiCfgMode);

	int WLAN_SetCurToolMode(eWifiToolStatus status);
#endif
	int WLAN_GetDhcpEnable();

	int checkIPAddr(const char *pszIPAddress);
	
	int WLAN_SetDhcpEnable(int bEnable);

	int WLAN_RenewDhcp();
	//!\brief   wpa_cli 关闭再开启
	//!\reason 由于AIC8800模块，在关闭DHCP后，修改IP，设备就ping不通了，厂家也没有提供很好的处理方案；
	//!\reason	经过命令配网测试，通过用 wpa_cli disable_network 0 和 wpa_cli enable_network 0 的方式，可以规避这个问题
	//!\return  成功返回0，其他为错误码
	int WLAN_ReNewWpaCli();

	char *WLAN_GetIPAddress(char *pszIPAddress, int addressSize = ADDRESS_DEFAULT_SIZE);

	int WLAN_SetIPAddress(const char *pszIPAddress);

	char *WLAN_GetSubnetMask(char *pszSubnetMask, int SubnetMaskSize = ADDRESS_DEFAULT_SIZE);

	int WLAN_SetSubnetMask(const char *pszSubnetMask);

	char *WLAN_GetGateway(char *pszGateway, int GatewaySize = ADDRESS_DEFAULT_SIZE);

	int WLAN_SetGateway(const char *pszGateway);

	char *WLAN_GetMacAddress(char *pszMacAddress,int MacAddressSize = ADDRESS_DEFAULT_SIZE);

	int WLAN_GetDNS(char *PrimaryDNS, char *SecondaryDNS, int DnsSize);

	int WLAN_SetDNS(char *PrimaryDNS, char *SecondaryDNS);

	int WLAN_GetAccessPoints(WLAN_ACCESSPOINT pAccessPoints[WLAN_MAX_AP_NUM], int *pnAPCount);

	int WLAN_GetAccessSignalInfo(LPWLAN_ACCESSSIGNALINFO signal_info);

    int WLAN_GetAccessSignalQuality(int *nQuality);

	int WLAN_GetConnection(LPWLAN_CONNECTION pConnection);
#if defined (__IS_SUPPORT_WLAN__)
	int WLAN_SaveWifiKey(char *key);

	int WLAN_GetWifiKey(char *key);

	int CfgWirelessWhenWireChanged(int isWire);
	
	int WLAN_setFromApToStaConnectionCfgFlag(int cfgOverFlag);
#endif
	int WLAN_GetWiFiModuleEnable();

	int WLAN_SetEnable(int bEnable);

	int WLAN_GetStatus();

	int WLAN_GetDhcpState();

	int NET_SetWifiMode(unsigned int mode, int force);

    int Net_ZwP2pPushEvent(ZWP2P_UINT32 nEventType, ZWP2P_UINT32 nChannel, LPZWP2P_PUSH_EVENT_INFO pEventInfo);
        
    void Net_SetZwP2pPushInit(int isInited);

	//!\brief 获取网卡信息，提供给产测打印二维码使用
	int Net_GetNetCardInfo(NetCardInfo *NetworkInfo);   

#ifdef __MODULE_4G_SUPPORT__
	char *NET_4G_GetTutkUID(TutkCfg *pTutkCfg);

	//初始化P2P产测
	void NET_P2P_ProductTestInit();

    //初始化4G绑定流程
    void NET_P2P_4GBindInit();

	//P2P产测模式下
	void NET_P2P_ProductTest_Resolution_Init();
	
	//初始化4G相关数据
	void NET_4GModule_Init();

	/* 4G状态检测线程 */
	void* Module4G_CheckStatusThreadProcess();


	/* 网络路由表处理线程 */
	void* Operate4GModuleRouterThreadProcess();

	/* 获取4G工作模式 */
	int Module4G_GetCardBindMode();

	/* 获取产测状态 */
	int Module4G_GetProductState();

	/*加载P2P协议*/
	void Module4G_LoadTutkProtocol(int nEth0Status,int nTime);

    //获取当前云卡模式
    int Net_GetUcWorkMode(int *piUcWorkMode);

    //获取是否是云卡设备
    int Net_GetIsCloudCardDev(void);

	/* 获取当前SIM卡的信息 */
	int Net_GetSimCardInfo(char *imsi, 
									 char *imei, 
									 char *act, 
									 char *oper, 
									 char *band,
									 char *iccid,
									 char *sVersion);

	/* 获取当前模块的信号强度 */
	int Net_GetGPRSModuleSignalQuality(int *signal_quality);

	/* 检查Sim卡的状态0:成功 1:SIM卡锁住 2:失败 */
	int NET_GPRS_GetSimCardStatus(int * sim_card_status);
		/* 检查Sim卡的状态0:成功 1:失败 */
	int NET_GPRS_GetSimCardStatus1(int * sim_card_status);
	
	/* 检测当前GPRS模块是否可以连接互联网 */
	int Net_CheckIfGPRSAccessToInernet(int *gprs_status);

	/*获取金睿平台的id，key*/
	int NET_GPRS_GetFlAppkey(char  * gprs_appkey);

	int NET_GPRS_GetFlAppid(char  * gprs_appid);

	//获取北纬平台appid
	int NET_GPRS_GetNorthLatiAppid(char  * gprs_appid);

	//获取北纬平台key
	int NET_GPRS_GetNorthLatiAppkey(char	* gprs_appkey);

	//获取北纬平台cardtype
	int NET_GPRS_GetNorthLatiCardType(char	* gprs_cardtype,int len);


	/*获取卡类型*/
	int Net_GPRS_FL_SimCardType(int *nType);

	/*获取允许联网时间*/
	int Net_GPRS_FL_AllowTime(int *nTime);

	//设置卡类型
	int NET_SetSimCardType(int	* simcard_type);

	/* 检测卡类型计时器线程 */
	void* CheckCardTypeTimerThreadProcess(void);  

	/* 检测是否是北纬卡 */
	int Module4G_CheckBWCardType(int reset);

	/* 检测是否是云仓库卡 */
	int Module4G_CheckClouldWareHouseCardType(int reset);

	/* 获取金睿4G解绑状态 */
	int Module4G_GetCardBindState(int reset);

	/* 根据是否是金睿卡、解绑状态、判定是否允许使用4G联网 */
	void Modlue4G_AllowHandler(int reset);

	/* 根据是否是产测模式、运营模式、判定是否允许使用有线联网 */
	void Modlue4G_Eth0AllowHandler(int nTime);
	
	/* 获取双sim卡信息 */
	int Net_GetSimIndexCardInfo(int nSim, int* nIsInUse, char *szOperator, int *nSignalStrength,
			char *szICCID, char *szIMEI, char *sz4GModuleVersion);

#endif

	//开启蓝牙
	int WLAN_BluetoothOpen();
	
	//关闭蓝牙
	int WLAN_BluetoothClose();

    int BluetoothIsOpen();
	//获取蓝牙开启时间
	int Get_BluetoothStartTime();
	
#ifdef __IS_SUPPORT_AIC_BLUETOOTH__


	//蓝牙数据接收
	static int BlueToothRecvData(XRSBLE_L2CmdData *pCmdData);
	
	//设置应用层蓝牙收数据回调接口
	int NET_SetAppBlueToothRecvCallback(AppRecvProcessCallback pFunc);
	
	int getBlueMode();
	
	int setBlueTestStatus(int status);
	
	int getBlueTestStatus(int *status);
	// 标记该设备是产测设备的client 
	int WLAN_setBluetoothProTestClient(int status);
	// 开始搜索其他的蓝牙设备
	int WLAN_BluetoothScanDev();
	// 开始连接指定mac的蓝牙设备
	int WLAN_BluetoothMacConnectDev(uint8_t *macAddr,uint8_t type = 0);
	// 断开其他蓝牙的连接
	int WLAN_BluetoothDisconnectDev();
	
	int WLAN_BluetoothConnectStatus();

#endif

    //!\brief清空授权的域名后缀标志
    int Net_ClearAuthDomainSuffix();

    int Net_GetAuthDomainSuffixExist(int *isExist);

    int Net_SetAuthDomainSuffixExist(int isExist);

    int Net_GetTutkAndDomainSuffix(char *Tutk, int TutkLen, char *Domainsuffix, int DomainLen);

    int Net_GetAuthDomainSuffixExist_Test(int *isExist);

	int Net_GetTutkAndDomainSuffix_Test(char *Tutk, int TutkLen, char *Domainsuffix, int DomainLen);

#if defined (__IS_SUPPORT_4G_CLOUD_DOMAIN__)
    int NET_AddDomainForGetAlias(char *pDomain);

    int NET_SetAddDomainList2Local(const NET_DOMAIN_INFO_LIST DomainList);

    int NET_SetDomainListManagerServer(TransParent_INFO *pTransParentInfo);
#endif

#ifdef USE_HOSTS_PARSER
	/*
	********************************************************************************
	* @函数功能: 通过zwhost接口计算域名解析时间
	* @作者	 : liushun
	* @param[in] *pDNS 主DNS
	* @param[in] *pSNS	次DNS
	* @param[in] *pGateWay	网关
	* @param[out] *plist	输出排序好的dns列表
	* @返回值  : 0表示成功, 否则表示错误码 
	* @其他	 : 
	********************************************************************************
	*/
	int NET_UseZwhostCalcDomain(const char *pDNS, const char *pSNS, const char *pGateWay, ZWHost_DNS_SERVER_INFO_LIST *plist);
#endif

public:

	int WLAN_ReActive();
#if defined (__IS_SUPPORT_WLAN__)
	int WLAN_CheckStatus();
	int WLAN_GetWifiMode();
	int WLAN_getWLANWPAKey(char *szSSID,int *authMode, int *encryptype);
#endif
    CConfig * GetConfig();

private:
	int IsWlanIpConflict();

    int WLAN_ReadConnCfgFromSD();

	int WLAN_ReadConnCfg(CConfig * pConfig);

	int WLAN_SaveConnCfg(CConfig * pConfig);
public:
    unsigned long m_iTutkStatus;
#ifdef __MODULE_4G_SUPPORT__
	int m_iConnServer;
    int m_b4gScanEnd;
    char m_szUserId[64];
    char m_szServerAddress[128];
    char m_szRandomNumber[16];
#endif 
	Net_AuthP2pInfo   m_AuthP2pInfo;
	int m_eCurrUSBWifiDev;


private:
	int m_bWlan_Enable;
	eWifiMode m_wifiMode;
#if defined (__IS_SUPPORT_WLAN__)
	// 扫描的wifi信息缓存
	WLAN_ACCESSPOINT m_AccessPoints[WLAN_MAX_AP_NUM];
	//wifi配网模式(常规/SD卡配网)
	eWifiCfgMode m_wifiCfgMode;
    eWifiCfgMode m_wifiSdCfgMode;
	int m_wifiFakeSTAModeEnable;
	unsigned int m_wifiToStaFlag;
	unsigned int m_wifiApToStaCfgOver;//ap切到sta模式的配置完成标志，通知连接超时开始计数
	unsigned int m_changeWifiConnectFlag;
	int m_bIsConfigureNetIng;  //当前是否正在进行配网
	char m_szSessionID[8];
	pthread_mutex_t m_SetWifiModeLock;
	eWifiToolStatus m_curToolStatus;
#endif
	//是否使能DHCP
	int m_bWlan_DHCP_Enable;
    int m_bWlan_ifRouteDeleted;
    int m_bIsAddWifiRoute;
    int m_bIsAddNetRoute;
	//本地IP地址(0-静态;1-DHCP)
	char m_sWlan_LocalIP[MAX_IP_STR_LEN];
	//本地网关地址
	char m_sWlan_GateWay[MAX_IP_STR_LEN];
	//子网掩码
	char m_sWlan_SubNetMask[MAX_IP_STR_LEN];
	//MAC地址
	char m_sWlan_MAC_addr[MAX_MAC_STR_LENGTH];
	//冲突MAC地址
	char m_sWlan_OtherMAC_addr[MAX_MAC_STR_LENGTH];
	//本地IP地址,用于检查IP是否更新
	char m_sWlan_LocalIP_old[MAX_IP_STR_LEN];
	//网络状态
	int m_iWlan_Status;
	//wifi的DHCP状态  /*0-未连接,1-获取成功,2-正在获取,3-获取失败,继续获取*/
	int m_iWlan_DHCP_Status;
	WLAN_CONNECTION m_tWlan_Connection;
	char m_sWlanPrimaryDNS[MAX_IP_STR_LEN];
	char m_sWlanSecondaryDNS[MAX_IP_STR_LEN];

	char m_sPrimaryDNS[20]; // eth0 dns
	
	char m_sSecondaryDNS[20];
	
	//DHCP是否失败
	int m_iWlan_DHCPFailed;
#if defined (__IS_SUPPORT_WLAN__)
	//0:UDP推送结束 1:需要UDP推送2:UDP推送准备就绪
	char m_nUdpPushStatus;
	unsigned int m_u32ClientIp;
	unsigned int m_u32ClientPort;
	//UDP推送计数
	int m_nUdpPushCnt;
	//UDP推送线程
	pthread_t m_hUdpPushThread;
	pthread_mutex_t m_muUdpPushLock;

	int m_nWifiLinkStatus;
	int m_iWlanScanStatus;
	//wifi的热点配置将要改变的标记
	int m_wifiConnChange;
#endif

#ifdef __MODULE_4G_SUPPORT__
	int m_nAllowUseEth0;		//0-不允许使用有线		  1-允许使用有线
	int m_nProductState;		//0-正常状态	  1-产测状态
	int m_nSimCardBindState;	//0-正常模式	   1-运营模式
	int m_IsJinRuiSimCard;		//0-非金睿卡	   1-金睿卡 2-北纬卡
	int m_IsBindState;			//0-解绑状态	   1-绑定状态
	int m_IsAddwhite;			//0-未加白 1-已加白
	int m_CardTypeTmp_Clould; //0-非云仓库卡		用于是否是云仓库卡设置
	int m_CardTypeTmp_BW;		//0-非北纬卡	 用于是否是北纬卡设置
	int m_CardTypeTmp;			//0-非金睿卡	 用于是否是金睿卡设置
	int m_PrevEth0LinkStatus;	//记录上一次有线连接状态
	int m_CheckEth0Count;		//连续变换状态次数
	
	int m_FlowCustomType;		//运营客户类型
	int m_P2pProductMode;		//p2p产测标识, 1-p2p产测模式
	char m_PcMac[64];			//pcmac,用于p2p产测

	pthread_t m_4GModule_ProcessThread;  //检测GPRS状态线程
	pthread_t m_GPRS_CheckStatusThread;
	pthread_t m_CheckCardTypeTimerThread;

	//金睿平台的appid，appkey
	char m_szAppid[64];
	char m_szAppkey[256];

	//北纬平台的appid，appkey，cardtype
	char m_szNorthLatiAppid[64];
	char m_szNorthLatiAppkey[256];
	int m_NorthLatiCardType;

	unsigned int m_nCheckCardTypeCounter;
	unsigned int m_judgehalfhour; //0-----30分钟后需要重新检测卡的状态
	unsigned int m_judgehalfcount; //卡状态检测失败，该值就记一次数，到了12就重启

	//记录4g模组初始化接口的初始值，检测卡状态失败时需要给变量赋初始值以重新检测
	int m_RecordBindState;			
	int m_RecordCardTypeTmp_Clould; 
	int m_RecordCardTypeTmp_BW; 	
	int m_RecordCardTypeTmp;	
#endif
	//蓝牙是否开启
	int m_bIsBlueToothOpen;

#ifdef __IS_SUPPORT_AIC_BLUETOOTH__
	int m_blueProTestStatus;
	int m_isProTestClient;
	BlueMode m_blueMode;
	//蓝牙数据接收回调
	static AppRecvProcessCallback m_cbBlueToothRecvDataCb;
#endif

};

#endif							/* _INC_INET_49B634E70217_INCLUDED */

/*!
* 实现DHCP动态获取IP地址的功能
*
*      本功能依赖于busybox里的udhcp工具，使用的版本V1.2.2。为正常使用
*  这个工具，需要在设备端建立/usr/share/udhcpc/文件夹。并在这个目录下放
*  一个名为default.script脚本文件,内容附后。
*
*      注意：为了能在网络正常和不正常情况均能正常工作，需要了解udhcp的
*  用法，如使用udhcp命令不带任何参数则在正常的情况能正常获取动态IP地址。
*  但拔掉网线则会出现不会返回的情况。通过查看其用法，有一个参数-n表示
*  若不能获取一个IP地址的租期将立即返回。在本功能的实现中就采取立即返回
*  的方式。
*
*\author hdd
*\date	  2008/09/06
*\version 0.1
*/

/*!
*  以下是default.script的脚本源码
**********************************************************************

#!/bin/sh

# udhcpc script edited by Tim Riker <Tim@Rikers.org>

[ -z "$1" ] && echo "Error: should be called from udhcpc" && exit 1

RESOLV_CONF="/etc/resolv.conf"
[ -n "$broadcast" ] && BROADCAST="broadcast $broadcast"
[ -n "$subnet" ] && NETMASK="netmask $subnet"

case "$1" in
deconfig)
/sbin/ifconfig $interface 0.0.0.0
;;

renew|bound)
/sbin/ifconfig $interface $ip $BROADCAST $NETMASK

if [ -n "$router" ] ; then
echo "deleting routers"
while route del default gw 0.0.0.0 dev $interface ; do
:
done

for i in $router ; do
route add default gw $i dev $interface
done
fi

echo -n > $RESOLV_CONF
[ -n "$domain" ] && echo search $domain >> $RESOLV_CONF
for i in $dns ; do
echo adding dns $i
echo nameserver $i >> $RESOLV_CONF
done
;;
esac

exit 0
**********************************************************************
*/

