
/*!
 *\file		MPU_Net_API.h
 *\brief	MPU模块，本地网络管理相关接口定义
 *\author	林云
 *\date		2009/03/20

功能：
-#	静态IP地址、子网掩码、网关地址设置
-#	支持DHCP服务，手动更新网络参数
-#	HTTP端口设置
-#	mac地址读取
 */

#ifndef _INC_MPU_NET_API_H_
#define _INC_MPU_NET_API_H_


#include "CBAlert.h"
#include "wifi_api.h"

#if defined (__USE_FTP__)
//#include "NET/ftp_process.h"
#include "ftpClient.h"
#endif
#include "NET/INet.h"			//本地网络配置接口
#include "newhie/ZWP2PPUSHVNClient.h"
#include "newhie/HieServer.h"

typedef int (*CBNetInit) ();

//!\brief   获取是否启用DHCP
//!\return  TRUE表示启用，FALSE表示不启用
BOOL MPU_NET_GetDhcpEnable();

//!\brief   获取是否启用DHCP，如果有将要修改为的DHCP使能则返回该值（还未生效），否则返回当前真实的DHCP使能
//!\return  TRUE表示启用，FALSE表示不启用
BOOL MPU_NET_GetDhcpEnableEx();

//!\brief   设置是否启用DHCP
//!\param   bEnable     TRUE表示启用，FALSE表示不启用
//!\return  0表示成功，其它为错误码
int MPU_NET_SetDhcpEnable(BOOL bEnable);

int MPU_NET_SetDhcpEnableEx(BOOL bEnable, int *dhcpChanged);

//!\brief   手动更新DHCP
//!\return  0表示成功，其它为错误码
int MPU_NET_RenewDhcp();

//!\brief   获取IP地址
//!\param   pszIPAddress    返回IP地址（例 172.1.10.2）
//!\return  本接口拷贝IP地址到pszIPAddress指向的buf，并返回pszIPAddress
char *MPU_NET_GetIPAddress(char *pszIPAddress);

char *MPU_NET_GetIPAddressEx(char *pszIPAddress);

//!\brief   设置IP地址
//!\param   pszIPAddress    IP地址（例 172.1.10.2）
//!\return  0表示成功，其它为错误码
int MPU_NET_SetIPAddress(const char *pszIPAddress);

int MPU_NET_SetIPAddressEx(const char *pszIPAddress, int *ipChanged);

int MPU_NET_IPAddressIfSameWithNvr(const char *pszIPAddress, int *same);

int MPU_NET_SetIPAddressAdjustEnable(int enable);

int MPU_NET_GetIPAddressAdjustEnable(int *enable);

int MPU_NET_SetIPByProbe(const char *pszIPAddress);

//!\brief   获取子网掩码地址
//!\param   pszSubnetMask   返回子网掩码（例 255.255.0.0）
//!\return  本接口拷贝子网掩码到pszSubnetMask指向的buf，并返回pszSubnetMask
char *MPU_NET_GetSubnetMask(char *pszSubnetMask);

//!\brief   设置子网掩码地址
//!\param   pszSubnetMask   子网掩码（例 255.255.0.0）
//!\return  0表示成功，其它为错误码
int MPU_NET_SetSubnetMask(const char *pszSubnetMask);

int MPU_NET_SetSubnetMaskEx(const char *pszSubnetMask, int *subMaskChanged);

//!\brief   获取网关地址
//!\param   pszGateway      返回网关地址（例 172.1.10.1）
//!\return  本接口拷贝网关地址到pszGateway指向的buf，并返回pszGateway
char *MPU_NET_GetGateway(char *pszGateway);

//!\brief   设置网关地址
//!\param   pszGateway      网关地址（例 172.1.10.1）
//!\return  0表示成功，其它为错误码
int MPU_NET_SetGateway(const char *pszGateway);

//!\brief   获取HTTP Server端口
//!\return  端口号（例：80）
unsigned short MPU_NET_GetHttpPort();

//!\brief   设置HTTP Server端口
//!\param   usPort          端口号（例：80）
//!\return  0表示成功，其它为错误码
int MPU_NET_SetHttpPort(unsigned short usPort);

//!\brief   获取command端口
//!\return  端口号（例：6060）
unsigned short MPU_NET_GetCommandPort();

//!\brief   设置command端口
//!\param   usPort          端口号（例：6060）
//!\return  0表示成功，其它为错误码
int MPU_NET_SetCommandPort(unsigned short usPort);

//!\brief   获取media端口
//!\return  端口号（例：6066）
unsigned short MPU_NET_GetMediaPort();

//!\brief   设置media端口
//!\param   usPort          端口号（例：6066）
//!\return  0表示成功，其它为错误码
int MPU_NET_SetMediaPort(unsigned short usPort);

//!\brief   获取rtsp端口
//!\return  端口号（例：554）
unsigned short MPU_NET_GetRtspPort();

//!\brief   设置rtsp端口
//!\param   usPort          端口号（例：554）
//!\return  0表示成功，其它为错误码
int MPU_NET_SetRtspPort(unsigned short usPort);

//!\brief   获取mobile端口
//!\return  端口号（例：10000）
unsigned short MPU_NET_GetMobilePort();

//!\brief   设置mobile端口
//!\param   usPort          端口号（例：10000）
//!\return  0表示成功，其它为错误码
int MPU_NET_SetMobilePort(unsigned short usPort);

//!\brief   获取auto detect端口
//!\return  端口号（例：5555）
unsigned short MPU_NET_GetAutoDetectPort();

//!\brief   设置auto detect端口
//!\param   usPort          端口号（例：5555）
//!\return  0表示成功，其它为错误码
int MPU_NET_SetAutoDetectPort(unsigned short usPort);

unsigned short MPU_NET_GetHKPrivatePort();

int MPU_NET_SetHKPrivatePort(unsigned short usPort);

//!\brief   获取Mac地址
//!\param   pszMacAddress   返回Mac地址（例 00-1A-4D-2B-91-64）
//!\return  本接口拷贝Mac地址到pszMacAddress指向的buf，并返回pszMacAddress
char *MPU_NET_GetMacAddress(char *pszMacAddress);

//!\brief       设置IP冲突联动动作
//!\param   nActionType 类型
//!     ACTION_BEEP                     声音提示
//!      ACTION_SENDTOSERVER        上传报警服务器
//!      ACTION_EMAIL                       联动Email
//!      ACTION_SMS                     联动短信   
//!      ACTION_ALARMOUT                报警输出
//!\param   bEnable 如果是报警输出则为端口号掩位,其他表示使能
int MPU_NET_SetIPAction(int nActionType, int bEnable);

//!\brief       获取IP冲突联动动作
//!\param nActionType 类型
//  !      ACTION_BEEP 声音提示
//  !      ACTION_SENDTOSERVER 上传报警服务器
//  !      ACTION_EMAIL 联动Email
//  !      ACTION_SMS 联动短信   
//  !      ACTION_ALARMOUT 报警输出
//!\return 返回是否设置，如果是报警输出则返回端口号掩位
int MPU_NET_GetIPAction(int nActionType);

#if defined (__USE_FTP__)
int MPU_NET_GetFtpTransferType(int *p_transfer_type);

//!\brief      设置FTP相关参数
//!\param      pFTP_user_param 类型
int MPU_NET_SetFtpParam(pFTP_user_param pFtpUserCfg);

//!\brief      获取FTP相关参数
//!\param      pFTP_user_param 类型
int MPU_NET_GetFtpParam(pFTP_user_param pFtpUserCfg);

//!\brief      获取FTP主机状态
//!\param      void 类型
//!\return     0未连接，1连接成功
int MPU_NET_GetFtpParam();

#endif

//!\brief      获取当前哪一个网卡有效
//!\param      pCurrNetCardType 类型
int MPU_NET_GetCurrNetCardType(int * pCurrNetCardType);


//!\brief       设置IP冲突(IP更新)通知回调
//!\param   pCallback           回调函数指针
//!\param   int nAlarmType    报警类型
//!\return  0表示成功，其它为错误码
//pAlertInfo.AlertType = 网络状态类型;
//pAlertInfo.nChannel = 0;
//pAlertInfo.dwReserved = (0表示IP冲突，1表示IP更新)
//pActionInfo.tActionType = 报警类型,ACTION_BEEP,ACTION_SENDTOSERVER,ACTION_EMAIL,ACTION_SMS,ACTION_ALARMOUT
//pActionInfo.dwReserved = <IP冲突: 冲突主机MAC地址指针,6字节>
//pActionInfo.nChannel = 报警输出时表示端口号
int MPU_NET_SetIPCallBack(int nAlarmType, CBAlert pCallback);

//!\brief       设置网线断联动动作
//!\param   nActionType 类型
//!     ACTION_BEEP                     声音提示
//!      ACTION_SENDTOSERVER        上传报警服务器
//!      ACTION_EMAIL                       联动Email
//!      ACTION_SMS                     联动短信   
//!      ACTION_ALARMOUT                报警输出
//!\param   bEnable 如果是报警输出则为端口号掩位,其他表示使能
int MPU_NET_SetPHYAction(int nActionType, int bEnable);

//!\brief       获取网线断联动动作
//!\param nActionType 类型
//  !      ACTION_BEEP 声音提示
//  !      ACTION_SENDTOSERVER 上传报警服务器
//  !      ACTION_EMAIL 联动Email
//  !      ACTION_SMS 联动短信   
//  !      ACTION_ALARMOUT 报警输出
//!\return 返回是否设置，如果是报警输出则返回端口号掩位
int MPU_NET_GetPHYAction(int nActionType);

//!\brief       设置网线断(或重连)通知回调
//!\param   pCallback           回调函数指针
//!\param   int nAlarmType    报警类型
//!\return  0表示成功，其它为错误码
//pAlertInfo.AlertType = 网络状态类型;
//pAlertInfo.nChannel = 0;
//pAlertInfo.dwReserved = (0表示网线断，1网线重连)
//pActionInfo.tActionType = 报警类型,ACTION_BEEP,ACTION_SENDTOSERVER,ACTION_EMAIL,ACTION_SMS,ACTION_ALARMOUT
//pActionInfo.dwReserved = 0
//pActionInfo.nChannel = 报警输出时表示端口号
int MPU_NET_SetPHYCallBack(int nAlarmType, CBAlert pCallback);

//!\brief 获取网络状态
//!\return  <-1-错误;0-正常;1-断开;2-未知状态；3-IP冲突;4-IP更新
int MPU_NET_GetStatus();

//!\brief   获取是否启用DHCP
//!\return  TRUE表示启用，FALSE表示不启用
BOOL MPU_WLAN_GetDhcpEnable();

//!\brief   设置是否启用DHCP
//!\param   bEnable     TRUE表示启用，FALSE表示不启用
//!\return  0表示成功，其它为错误码
int MPU_WLAN_SetDhcpEnable(BOOL bEnable);

//!\brief   手动更新DHCP
//!\return  0表示成功，其它为错误码
int MPU_WLAN_RenewDhcp();

//!\brief   获取IP地址
//!\param   pszIPAddress    返回IP地址（例 172.1.10.2）
//!\return  本接口拷贝IP地址到pszIPAddress指向的buf，并返回pszIPAddress
char *MPU_WLAN_GetIPAddress(char *pszIPAddress);

//app 推送用
int MPU_NET_SetP6SMSGServerStatics(int PushMode,int PushStatus);

//获取网卡信息给产测打印二维码使用
int MPU_Net_GetNetCardInfo(NetCardInfo *NetworkInfo);

//!\brief   设置IP地址
//!\param   pszIPAddress    IP地址（例 172.1.10.2）
//!\return  0表示成功，其它为错误码
int MPU_WLAN_SetIPAddress(const char *pszIPAddress);

//!\brief   获取子网掩码地址
//!\param   pszSubnetMask   返回子网掩码（例 255.255.0.0）
//!\return  本接口拷贝子网掩码到pszSubnetMask指向的buf，并返回pszSubnetMask
char *MPU_WLAN_GetSubnetMask(char *pszSubnetMask);

//!\brief   设置子网掩码地址
//!\param   pszSubnetMask   子网掩码（例 255.255.0.0）
//!\return  0表示成功，其它为错误码
int MPU_WLAN_SetSubnetMask(const char *pszSubnetMask);

//!\brief   获取网关地址
//!\param   pszGateway      返回网关地址（例 172.1.10.1）
//!\return  本接口拷贝网关地址到pszGateway指向的buf，并返回pszGateway
char *MPU_WLAN_GetGateway(char *pszGateway, int GatewaySize);

//!\brief   设置网关地址
//!\param   pszGateway      网关地址（例 172.1.10.1）
//!\return  0表示成功，其它为错误码
int MPU_WLAN_SetGateway(const char *pszGateway);

//!\brief   获取Mac地址
//!\param   pszMacAddress   返回Mac地址（例 00-1A-4D-2B-91-64）
//!\return  本接口拷贝Mac地址到pszMacAddress指向的buf，并返回pszMacAddress
char *MPU_WLAN_GetMacAddress(char *pszMacAddress);

//!\brief   获取无线热点列表
//!\param   pAccessPoints   热点数组
//!\param   pnAPCount 热点个数
//!\return  本接口拷贝热点列表到pAccessPoints指向的buf，成功返回0，其他为错误码
int MPU_WLAN_GetAccessPoints(WLAN_ACCESSPOINT pAccessPoints[WLAN_MAX_AP_NUM], int *pnAPCount);

//!\brief   获取当前连接wifi信号强度
//!\param   pSignalInfo   热点信号值
//!\return  本接口返回热点信号值，成功返回0，其他为错误码
int MPU_WLAN_GetAccessSignalInfo(LPWLAN_ACCESSSIGNALINFO pSignalInfo);
int MPU_WLAN_GetAccessSignalQuality(int *nQuality);

//!\brief   获取无线连接参数
//!\param   连接参数
//!\return  本接口拷贝连接到pConnection指向的buf，成功返回0，其他为错误码
int MPU_WLAN_GetConnection(LPWLAN_CONNECTION pConnection);

//!\brief 设置无线连接参数
//!\param 连接参数
//!\return 设置无线连接参数 0表示成功，其他表示错误码
int MPU_WLAN_SetConnection(LPWLAN_CONNECTION pConnection);

//!\brief 获取网络状态
//!\return  <-1-错误;0-正常;1-断开;2-未知状态；3-IP冲突;4-IP更新
int MPU_WLAN_GetStatus();

//!\brief   设置DNS地址
//!\param   char * PrimaryDNS, char * SecondaryDNS  主备DNS地址
//!         PrimaryDNS!=NULL,SecondaryDNS可以为空
//!\return  0成功，其它失败
//##ModelId=49BB403B030E
int MPU_WLAN_SetDNS(char *PrimaryDNS, char *SecondaryDNS);

//!\brief   获取DNS地址
//!\param   char * PrimaryDNS, char * SecondaryDNS      返回DNS地址 
//!         PrimaryDNS!=NULL,SecondaryDNS可以为空
//!\return  0成功，其它失败
//##ModelId=49BB403B030E
int MPU_WLAN_GetDNS(char *PrimaryDNS, char *SecondaryDNS, int SecondarySize);

//!\brief   获取WLAN_DHCP状态
//!\return  未连接0 正在获取1 获取失败,继续获取2 获取成功3

int MPU_WLAN_GetDhcpState();


int MPU_WLAN_SetEnable(int bEnable);

int MPU_WLAN_GetWiFiModuleEnable();

int MPU_WLAN_SetConnectionEx(LPWLAN_CONNECTION pConnection);

int MPU_WLAN_SetTestConnection(LPWLAN_CONNECTION pConnection);

int MPU_WLAN_GetWifiTestStatus();

int MPU_WLAN_IsConfigureNetIng();

int MPU_WLAN_SaveQrConfigureWlanSessionID(char *pszData);

int MPU_WLAN_GetQrConfigureWlanSessionID(char *pszData, int nBufLen);

int MPU_WLAN_GetWifiLinkStatus(int *pnStatus);

int MPU_NET_SetWlanScanStatus(int *pWlanScanStatus);

int MPU_WLAN_SetClientAddrInfo(unsigned int u32Ip, unsigned int u32Port);

int MPU_NET_SetWifiMode(unsigned int mode, int force);

int MPU_Net_CheckIfNetAccessToInternet(int *pConnStatus);

int MPU_NET_GetCurrWifiMode(int *pCurrWifiMode);

// #endif

int MPU_NET_SetTutkUID(TutkCfg *pTutkCfg, int pSetMethod);

int NET_SetP6SMSGServerStatics(int PushMode,int PushStatus);

#if defined (__AUTH_V2__)
int MPU_NET_SetConnectString(P2P_CONNECT_INFO *pConnectString, const unsigned char *pszAuthLicense);
#endif

char *MPU_NET_GetTutkUID(TutkCfg *pTutkCfg, unsigned long *pTutkStatus);

#if defined (__AUTH_V2__)
int MPU_NET_GetConnectString(P2P_CONNECT_INFO *pConnectString);
#endif

int MPU_NET_SetApplyDIDRequestUrl(char *pRequestUrl);

int MPU_NET_SetTutkStatus(int *pTutkStatus);

int MPU_NET_GetTutkStatus(int *pTutkStatus);

int MPU_NET_SetUlkUIDEnable(int bEnable);

int MPU_NET_GetUlkUIDEnable();

int MPU_NET_SetUlkUID(char bEnable, const char *pUlkUID);

int MPU_NET_GetP2PDownloadURL(char *pDownloadURL);


char *MPU_NET_GetUlkUID(char *bEnable, char *pUlkUID, int *pUlkStatus);
	
int MPU_NET_SetUlkStatus(int *pUlkStatus);

int MPU_NET_SetULKUID(char *pUlkUid);

int MPU_NET_NeedUpdateRouter(void);

/* 获取当前用的DID是否支持Subkey */
int MPU_NET_IsSupportSetSubkeyByDID();

/* 设置PushAi参数 */
int MPU_NET_SetPushAiParam(const char *szPushAiParam);

/* 获取PushAi参数 */
int MPU_NET_GetPushAiParam(char *szPushAiParam);

/*推送事件 */
int MPU_Net_ZwP2pPushEvent(ZWP2P_UINT32 nEventType, 
	ZWP2P_UINT32 nChannel, LPZWP2P_PUSH_EVENT_INFO pEventInfo);

/* 检测当前网络是否可以连接互联网(*pConnStatus=1表示可以联通互联网) */
int MPU_Net_CheckIfNetAccessToInternet(int *pConnStatus);
							//_INC_MPU_NET_API_H_
#if defined (__IS_SUPPORT_P6SMESSAGE__)
//获取云消息URL
int MPU_NET_GetMpServerParamCfg(char *ServerIpAndPort, char *UrlPathTimeCheck, char *UrlPathEventPolicy, char *UrlPathEventPost, char *UrlPathSetXmlStatus, char *SrvMaintenance);

//获取云消息URL
int MPU_NET_SetMpServerParamCfg(char *ServerIpAndPort, char *UrlPathTimeCheck, char *UrlPathEventPolicy, char *UrlPathEventPost, char *UrlPathSetXmlStatus, char *SrvMaintenance);
#endif

#ifdef __MODULE_4G_SUPPORT__
/*设置开始执行产测流程*/
int MPU_Net_SetProductTest(int isTest);

int MPU_NET_SetConnServerStatus(int ConnServer);

/*获取4G测试数据*/
int MPU_Net_Get4GProductTestData(Module4G_ProductTest_Data_t *pData);

/*设置4G模块配置*/
int MPU_Net_Set4GControlConfig(HY_DVR_4G_CARD_SELECT_CFG_INFO *pcfg);

/*获取4G模块配置*/
int MPU_Net_Get4GControlConfig(HY_DVR_4G_CARD_SELECT_CFG_INFO *pcfg);

/*获取4G切卡结果*/
int MPU_Net_Get4GSwitchResult(HY_DVR_4G_CARD_SELECT_RESULT_INFO *pcfg);


/*获取4G卡信息*/
int MPU_Net_Get4GCardInfo(int nSim,int *nCardType,char *Iccid,int *nCardStatus,char *szOperator);

/* 获取4G模组信息 */
int MPU_Net_Get4GModuleInfo(char *Version,int *iSignal4G);

/* 获取当前是否是P2P产测模式 */
int MPU_Net_IsP2PProductTestMode();


/* 获取当前4G工作模式 */
int MPU_Net_Module4G_GetCardBindMode();


//获取云卡工作模式
int MPU_Net_GetUcWorkMode(int *piUcWorkMode);

//获取是否是云卡设备
int MPU_Net_GetIsCloudCardDev(void);

/*获取双sim卡信息*/
int MPU_Net_GetSimIndexCardInfo(int nSim, int* nIsInUse, char *szOperator, int *nSignalStrength,
            char *szICCID, char *szIMEI, char *sz4GModuleVersion);

/* 获取当前SIM卡的信息 */
int MPU_NET_GetSimCardInfo(char *imsi, char *imei, char *act, char *oper, char *band, char *iccid,char *version);

/* 获取当前模块的信号强度 */
int MPU_Net_GetGPRSModuleSignalQuality(int *signal_quality);

/* 检查Sim卡的状态0:成功 1:SIM卡锁住 2:失败 */
int MPU_NET_GPRS_GetSimCardStatus(int * sim_card_status);

/* 检查Sim卡的状态0:成功 1:失败 */
int MPU_NET_GPRS_GetSimCardStatus1(int * sim_card_status);

/* 检测当前GPRS模块是否可以连接互联网 */
int MPU_Net_CheckIfGPRSAccessToInernet(int *gprs_status);

/* 获取金睿平台的id，key*/
int MPU_Net_GPRS_FL_Appid(char *szFlAppid);

int MPU_Net_GPRS_FL_Appkey(char *szFlAppkey);

//获取北纬平台appid
int MPU_NET_GPRS_GetNorthLatiAppid(char  * gprs_appid);

//获取北纬平台key
int MPU_NET_GPRS_GetNorthLatiAppkey(char	* gprs_appkey);

//获取北纬平台cardtype
int MPU_NET_GPRS_GetNorthLatiCardType(char  * gprs_cardtype,int len);

/*获取卡类型*/
int MPU_Net_GPRS_FL_SimCardType(int *nType);

/*获取允许联网时间*/
int MPU_Net_GPRS_FL_AllowTime(int *nTime);

//设置卡类型
int MPU_NET_SetSimCardType(int  * simcard_type);

/*设置4g扫码状态*/
int MPU_Net_SetScanEndStatus(int scanend);

/*设置userId*/
int MPU_Net_SetUserId(char* userid);

/*设置服务器地址*/
int MPU_Net_SetServerAddress(char* serveraddress);

/*设置随机值*/
int MPU_Net_SetRandomNumber(char* randomnumber);
#endif

#ifdef __IS_SUPPORT_AIC_BLUETOOTH__
//开启蓝牙
int MPU_WLAN_BluetoothOpen();

//关闭蓝牙
int MPU_WLAN_BluetoothClose();

//设置应用层蓝牙收数据回调接口
int MPU_NET_SetAppBlueToothRecvCallback(AppRecvProcessCallback pFunc);

int MPU_WLAN_BluetoothProTestStatus(int *status);
#endif

//设置AuthP2p信息
//返回值：0-成功       其他-错误码
int MPU_Net_SetAuthP2pInfo(Net_AuthP2pInfo *ptAuthP2pInfo);

//获取AuthP2p信息
//返回值：0-成功       其他-错误码
int MPU_NET_GetAuthP2pInfo(Net_AuthP2pInfo *ptAuthP2pInfo);

int  MPU_Net_GetAuthDomainSuffixExist(int *status);

int  MPU_Net_GetTutkAndDomainSuffix(char *Tutk, int TutkLen, char *Domainsuffix, int DomainLen);

int  MPU_Net_GetAuthDomainSuffixExist_Test(int *status);

int  MPU_Net_GetTutkAndDomainSuffix_Test(char *Tutk, int TutkLen, char *Domainsuffix, int DomainLen);


int MPU_ETH0_GetDNS(char *PrimaryDNS, char *SecondaryDNS);

int MPU_ETH0_SetDNS(char *PrimaryDNS, char *SecondaryDNS);

int MPU_NETCARD_GetDhcpSuccTime(int	*nEth0DhcpSuccTime, int *nWlan0DhcpSuccTime);

int MPU_NET_SetAuthInfoPushDomainSuffix(P2P_TRANSPARENT_INFO_PUSHDOMAIN *pPushDomainParam);

int MPU_NET_GetAuthInfoPushDomainSuffix(P2P_TRANSPARENT_INFO_PUSHDOMAIN *pPushDomainParam);

int  MPU_Net_ClearAuthDomainSuffix();

#if defined (__IS_SUPPORT_4G_CLOUD_DOMAIN__)
int MPU_NET_AuthInfoTransparentV2(TransParent_INFO *pTransParentInfo);

int MPU_NET_SetAddDomainForGetAlias(char *pDomain);

int MPU_NET_SetAddDomainList2Local(const NET_DOMAIN_INFO_LIST DomainList);
#endif

/* 获取当前TCP转发是否开启 */
int MPU_Net_TcpRelay_GetEnable(int *bEnable);

/* 设置TCP转发 */
int MPU_Net_TcpRelay_SetEnable(int *bEnable);

int MPU_Net_GetWiFiUSBWifiDev();

#endif
