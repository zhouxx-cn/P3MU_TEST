
/*!
 *\file		MPU_Net_API.cpp
 *\brief	MPU模块，本地网络管理相关接口实现
 *\author	林云
 *\date		2009/03/20

功能：
-#	静态IP地址、子网掩码、网关地址设置
-#	支持DHCP服务，手动更新网络参数
-#	HTTP端口设置
-#	mac地址读取
 */

#include "MPU_global.h"
#include "MPU_CIU_global.h"
#include "mysystem.h"
#include "../PAR/CPubFunc.h"
#ifndef DEBUG_NOT_USE_NET_LIB	//调试时，不调用NET库

#include "NET/INet.h"			//本地网络配置接口
#include "newhie/HieServer.h"

#if defined (__USE_FTP__)
#include "ftpClient.h"
//#include "NET/ftp_process.h"
#endif

/**************************************************************************
 *                            局部宏定义                                  *
 **************************************************************************/


//调用网络管理器执行动作
#define MPU_NET_CALL(x)															\
do																				\
{																				\
	int nRet;																		\
																				\
	INet *pMgr = INet::GetInstance();													\
																				\
	if (NULL != pMgr)																\
	{																			\
		nRet = pMgr->x;															\
		if (IDVR_OK != nRet)														\
		{																		\
			MPU_ERR("%s, nRet = %d\n", #x, nRet);									\
		}																		\
	}																			\
	else																			\
	{																			\
		MPU_ERR("Get INet instance fail!\n");											\
		MPU_CHK(NULL != pMgr);													\
		nRet = IDVR_MPU_NET_NO_MANAGER_INSTANCE;									\
	}																			\
																				\
	return nRet;																	\
}																				\
while (0)


//调用网络管理器获取参数
#define MPU_NET_GET(DataType, DefaultValue, Function)									\
do																				\
{																				\
	DataType retValue;																\
																				\
	INet *pMgr = INet::GetInstance();													\
																				\
	if (NULL != pMgr)																\
	{																			\
		retValue = pMgr->Function;													\
	}																			\
	else																			\
	{																			\
		MPU_ERR("Get INet instance fail!\n");											\
		MPU_CHK(NULL != pMgr);													\
		retValue = DefaultValue;													\
	}																			\
																				\
	return retValue;																\
}																				\
while (0)


#else							/* #ifndef DEBUG_NOT_USE_NET_LIB */


//调用网络管理器执行动作
#define MPU_NET_CALL(x)															\
do																				\
{																				\
	return IDVR_OK;																\
}																				\
while (0)

//调用网络管理器获取参数
#define MPU_NET_GET(DataType, DefaultValue, Function)									\
do																				\
{																				\
	return DefaultValue;															\
}																				\
while (0)


#endif


/* 设备能力支持 */
static int bSupportULK      = 1;
static int bSupportTUTK     = 1;
static int bSupportIPAdjust = 1;

/**************************************************************************
 *                            模块内部接口                                *
 **************************************************************************/

//!\brief   初始化本地网络
//!\return  0表示成功，其它为错误码
int NET_Init()
{
	int nRet = IDVR_OK;
    char value[64];

#ifndef DEBUG_NOT_USE_NET_LIB	//调试时，不调用NET库

	INet *pMgr = INet::GetInstance();

	if (NULL == pMgr)
	{
		MPU_ERR("Get INet instance fail!\n");
		MPU_CHK(NULL != pMgr);
		nRet = IDVR_MPU_NET_NO_MANAGER_INSTANCE;
	}
	else
	{
		MPU_INFO("out, return %d\n", nRet);
	}
	
    /* 从用户配置文件里面获取用户是否需要支持该功能 */
    memset(value, 0, sizeof(value));
    if (0 == GetIniSectionItem(CUSTOM_CONFIG, "ULK", "bSupportULK", value, sizeof(value)))
	{
		bSupportULK = atoi(value);
	}

    memset(value, 0, sizeof(value));
    if (0 == GetIniSectionItem(CUSTOM_CONFIG, "TUTK", "bSupportTUTK", value, sizeof(value)))
	{
		bSupportTUTK = atoi(value);
	}

    memset(value, 0, sizeof(value));
    if (0 == GetIniSectionItem(CUSTOM_CONFIG, "IPADJUST", "bSupportIPAdjust", value, sizeof(value)))
    {
    	bSupportIPAdjust = atoi(value);
	}
            
#endif							/* #ifndef DEBUG_NOT_USE_NET_LIB */

	return nRet;
}

//!\brief   逆初始化本地网络
//!\return  0表示成功，其它为错误码
int NET_Uninit()
{
#ifndef DEBUG_NOT_USE_NET_LIB	//调试时，不调用NET库

	INet::Release();

#endif							/* #ifndef DEBUG_NOT_USE_NET_LIB */

	return IDVR_OK;
}

//!\brief   恢复出厂IP地址
void NET_Resume()
{
	MPU_DBG("\n\n..mpu_net_api.cpp...NET_Resume()..//!\brief   恢复出厂IP地址...\n");
#ifdef USE_ICAMERA
	//获取新的配置文件读取实例
	CConfig *pDefaultCfg = CConfig::NewInstance();
	int needReboot=0;

    char ipaddr[256];
    char netmask[256];
    char gateway[256];

    memset(ipaddr, 0, sizeof(ipaddr));
    memset(netmask, 0, sizeof(netmask));
    memset(gateway, 0, sizeof(gateway));

	//从缺省配置文件中读取
	int nRet = pDefaultCfg->LoadFileRO(IDVR_DEFAULT_CONFIG_FILE_PATH);

	if (0 == nRet)
	{
		int enable, port;

		enable = pDefaultCfg->GetValue("INet_Cfg", "DHCP_Enable", 0);
		MPU_NET_SetDhcpEnable(enable);

		pDefaultCfg->GetValue("INet_Cfg", "LocalIP", "192.168.1.11", ipaddr, sizeof (ipaddr));
		MPU_NET_SetIPAddress(ipaddr);
		
		pDefaultCfg->GetValue("INet_Cfg", "LocalSubnetMask", "255.255.255.0", netmask, sizeof (netmask));
		MPU_NET_SetSubnetMask(netmask);

		pDefaultCfg->GetValue("INet_Cfg", "LocalGateway", "192.168.1.1", gateway, sizeof (gateway));
		MPU_NET_SetGateway(gateway);
		//http端口最后修改，先改http端口后改ip会导致onvif崩溃引起程序崩溃
		port = pDefaultCfg->GetValue("INet_Cfg", "HttpPort", 80);
		needReboot = MPU_NET_SetHttpPort((unsigned short) port);
		
		port = pDefaultCfg->GetValue("CIU", "CommandPort", 6060);
		nRet = MPU_NET_SetCommandPort(port);
		
		port = pDefaultCfg->GetValue("CIU", "MediaPort", 6066);
		nRet = MPU_NET_SetMediaPort(port);
		
		port = pDefaultCfg->GetValue("CIU", "RtpPort", 554);
		nRet = MPU_NET_SetRtspPort(port);
		if(0 == nRet)
			needReboot = 1;
	
		port = pDefaultCfg->GetValue("CIU", "AutoDetectPort", 5555);
		nRet = MPU_NET_SetAutoDetectPort(port);

		port = pDefaultCfg->GetValue("CIU", "MobilePort", 10000);
		nRet = MPU_NET_SetMobilePort(port);

		//for HK
		port = pDefaultCfg->GetValue("CIU","HKPrivatePort",8000);
		nRet = MPU_NET_SetHKPrivatePort(port);
	}
	else
	{
		strncpy(ipaddr, "192.168.1.11", sizeof(ipaddr) - 1);
		strncpy(netmask, "255.255.255.0", sizeof(netmask) - 1);
		strncpy(gateway, "192.168.1.1", sizeof(gateway) - 1);
		
		MPU_NET_SetDhcpEnable(0);

		MPU_NET_SetIPAddress(ipaddr);

		MPU_NET_SetSubnetMask(netmask);

		MPU_NET_SetGateway(gateway);

		needReboot = MPU_NET_SetHttpPort(80);
		
		nRet = MPU_NET_SetCommandPort(6060);
		
		nRet = MPU_NET_SetMediaPort(6066);
		
		nRet = MPU_NET_SetRtspPort(554);
		if(0 == nRet)
			needReboot = 1;
		
		nRet = MPU_NET_SetAutoDetectPort(5555);

		nRet = MPU_NET_SetMobilePort(10000);

		nRet = MPU_NET_SetHKPrivatePort(8000);
	}
	
    //HieServer_RefreshEvent(USEREVENT_ALARM_NOTICE, ALERT_IPCHANGED, 0, 0);
    //result = HieServer_RefreshEventEx(USEREVENT_ALARM_NOTICE, 0, 
	//	ALERT_TYPE_IPCHANGED, 0, 0, NULL, 0);

	CConfig::ReleaseInstance(pDefaultCfg);

	MPU_PAR_SaveConfigReal();
	if (1 == needReboot)
	{
		PAR_ForbiddenSaveFile();
        sleep(2);
		RebootLinux(REBOOT_RESUME);
	}

#endif
}

/**************************************************************************
 *                            全局接口                                    *
 **************************************************************************/

//!\brief   获取是否启用DHCP
//!\return  TRUE表示启用，FALSE表示不启用
BOOL MPU_NET_GetDhcpEnable()
{
	MPU_NET_GET(BOOL, FALSE, NET_GetDhcpEnable());
}

//!\brief   获取是否启用DHCP，如果有将要修改为的DHCP使能则返回该值（还未生效），否则返回当前真实的DHCP使能；用于返回值给客户端
//!\return  TRUE表示启用，FALSE表示不启用
BOOL MPU_NET_GetDhcpEnableEx()
{
	MPU_NET_GET(BOOL, FALSE, NET_GetDhcpEnableEx());
}

//!\brief   设置是否启用DHCP
//!\param   bEnable     TRUE表示启用，FALSE表示不启用
//!\return  0表示成功，其它为错误码
int MPU_NET_SetDhcpEnable(BOOL bEnable)
{
	MPU_NET_CALL(NET_SetDhcpEnable(bEnable));
}

int MPU_NET_SetDhcpEnableEx(BOOL bEnable, int *dhcpChanged)
{
	MPU_NET_CALL(NET_SetDhcpEnableEx(bEnable, dhcpChanged));
}

//!\brief   手动更新DHCP
//!\return  0表示成功，其它为错误码
int MPU_NET_RenewDhcp()
{
	MPU_NET_CALL(NET_RenewDhcp());
}

//!\brief   获取IP地址
//!\param   pszIPAddress    返回IP地址（例 172.1.10.2）
//!\return  本接口拷贝IP地址到pszIPAddress指向的buf，并返回pszIPAddress
char *MPU_NET_GetIPAddress(char *pszIPAddress)
{
	MPU_NET_GET(char *, pszIPAddress, NET_GetIPAddress(pszIPAddress));
}

//!\brief   获取IP地址，如果有将要修改为的IP地址则返回该地址（还未生效），否则返回真实起效的IP地址
//!\param   pszIPAddress    返回IP地址（例 172.1.10.2）
//!\return  本接口拷贝IP地址到pszIPAddress指向的buf，并返回pszIPAddress
//!\brief   获取IP地址，如果有将要修改为的IP地址则返回该地址（还未生效），否则返回真实起效的IP地址；用于返回值给客户端
//!\param   pszIPAddress    返回IP地址（例 172.1.10.2）
//!\return  本接口拷贝IP地址到pszIPAddress指向的buf，并返回pszIPAddress
char *MPU_NET_GetIPAddressEx(char *pszIPAddress)
{
	MPU_NET_GET(char *, pszIPAddress, NET_GetIPAddressEx(pszIPAddress));
}

//!\brief   设置IP地址
//!\param   pszIPAddress    IP地址（例 172.1.10.2）
//!\return  0表示成功，其它为错误码
int MPU_NET_SetIPAddress(const char *pszIPAddress)
{
	MPU_NET_CALL(NET_SetIPAddress(pszIPAddress));
}

int MPU_NET_SetIPAddressEx(const char *pszIPAddress, int *ipChanged)
{
	MPU_NET_CALL(NET_SetIPAddressEx(pszIPAddress, ipChanged));
}

int MPU_NET_SetP6SMSGServerStatics(int PushMode,int PushStatus)
{
	MPU_NET_CALL(NET_SetP6SMSGServerStatics(PushMode,PushStatus));
}

//获取网卡信息给产测打印二维码使用
int MPU_Net_GetNetCardInfo(NetCardInfo *NetworkInfo)
{
	MPU_NET_CALL(Net_GetNetCardInfo(NetworkInfo));
}

//!\brief	设置IP地址
//!\param	pszIPAddress	IP地址（例 172.1.10.2）
//!\return	0表示成功，其它为错误码
int MPU_NET_IPAddressIfSameWithNvr(const char *pszIPAddress, int *same)
{
	MPU_NET_CALL(NET_IPAddressIfSameWithNvr(pszIPAddress, same));
}

int MPU_NET_SetIPAddressAdjustEnable(int enable)
{
	MPU_NET_CALL(NET_SetIPAddressAdjustEnable(enable));
}

int MPU_NET_GetIPAddressAdjustEnable(int *enable)
{
	MPU_NET_CALL(NET_GetIPAddressAdjustEnable(enable));
}

//!\brief   设置IP地址
//!\param   pszIPAddress    IP地址（例 172.1.10.2）
//!\return  0表示成功，其它为错误码
int MPU_NET_SetIPByProbe(const char *pszIPAddress)
{
	MPU_NET_CALL(NET_SetIPAddressByNVR(pszIPAddress));
}

//!\brief   获取子网掩码地址
//!\param   pszSubnetMask   返回子网掩码（例 255.255.0.0）
//!\return  本接口拷贝子网掩码到pszSubnetMask指向的buf，并返回pszSubnetMask
char *MPU_NET_GetSubnetMask(char *pszSubnetMask)
{
	MPU_NET_GET(char *, pszSubnetMask, NET_GetSubnetMask(pszSubnetMask));
}

int MPU_NET_SetSubnetMaskEx(const char *pszSubnetMask, int *subMaskChanged)
{
	MPU_NET_CALL(NET_SetSubnetMaskEx(pszSubnetMask, subMaskChanged));
}

//!\brief   设置子网掩码地址
//!\param   pszSubnetMask   子网掩码（例 255.255.0.0）
//!\return  0表示成功，其它为错误码
int MPU_NET_SetSubnetMask(const char *pszSubnetMask)
{
	MPU_NET_CALL(NET_SetSubnetMask(pszSubnetMask));
}

//!\brief   获取网关地址
//!\param   pszGateway      返回网关地址（例 172.1.10.1）
//!\return  本接口拷贝网关地址到pszGateway指向的buf，并返回pszGateway
char *MPU_NET_GetGateway(char *pszGateway)
{
	MPU_NET_GET(char *, pszGateway, NET_GetGateway(pszGateway));
}

//!\brief   设置网关地址
//!\param   pszGateway      网关地址（例 172.1.10.1）
//!\return  0表示成功，其它为错误码
int MPU_NET_SetGateway(const char *pszGateway)
{
	MPU_NET_CALL(NET_SetGateway(pszGateway));
}

//!\brief   获取HTTP Server端口
//!\return  端口号（例：80）
unsigned short MPU_NET_GetHttpPort()
{
	MPU_NET_GET(unsigned short, ((unsigned short) 80), NET_GetHttpPort());
}

//!\brief   设置HTTP Server端口
//!\param   usPort          端口号（例：80）
//!\return  0表示成功，其它为错误码
int MPU_NET_SetHttpPort(unsigned short usPort)
{
	MPU_NET_CALL(NET_SetHttpPort(usPort));
}

//!\brief   获取command端口
//!\return  端口号（例：6060）
unsigned short MPU_NET_GetCommandPort()
{
	MPU_NET_GET(unsigned short, ((unsigned short) 6060), NET_GetCommandPort());
}

//!\brief   设置command端口
//!\param   usPort          端口号（例：6060）
//!\return  0表示成功，其它为错误码
int MPU_NET_SetCommandPort(unsigned short usPort)
{
	MPU_NET_CALL(NET_SetCommandPort(usPort));
}

//!\brief   获取media端口
//!\return  端口号（例：6066）
unsigned short MPU_NET_GetMediaPort()
{
	MPU_NET_GET(unsigned short, ((unsigned short) 6066), NET_GetMediaPort());
}

//!\brief   设置media端口
//!\param   usPort          端口号（例：6066）
//!\return  0表示成功，其它为错误码
int MPU_NET_SetMediaPort(unsigned short usPort)
{
	MPU_NET_CALL(NET_SetMediaPort(usPort));
}

//!\brief   获取rtsp端口
//!\return  端口号（例：554）
unsigned short MPU_NET_GetRtspPort()
{
	MPU_NET_GET(unsigned short, ((unsigned short) 554), NET_GetRtspPort());
}

//!\brief   设置rtsp端口
//!\param   usPort          端口号（例：554）
//!\return  0表示成功，其它为错误码
int MPU_NET_SetRtspPort(unsigned short usPort)
{
	MPU_NET_CALL(NET_SetRtspPort(usPort));
}

//!\brief   获取mobile端口
//!\return  端口号（例：10000）
unsigned short MPU_NET_GetMobilePort()
{
	MPU_NET_GET(unsigned short, ((unsigned short) 10000), NET_GetMobilePort());
}

//!\brief   设置mobile端口
//!\param   usPort          端口号（例：10000）
//!\return  0表示成功，其它为错误码
int MPU_NET_SetMobilePort(unsigned short usPort)
{
	MPU_NET_CALL(NET_SetMobilePort(usPort));
}

//!\brief   获取auto detect端口
//!\return  端口号（例：5555）
unsigned short MPU_NET_GetAutoDetectPort()
{
	MPU_NET_GET(unsigned short, ((unsigned short) 5555), NET_GetAutoDetectPort());
}

//!\brief   设置auto detect端口
//!\param   usPort          端口号（例：5555）
//!\return  0表示成功，其它为错误码
int MPU_NET_SetAutoDetectPort(unsigned short usPort)
{
	MPU_NET_CALL(NET_SetAutoDetectPort(usPort));
}

//!\brief   获取HK Private端口
//!\return  端口号（例：8000）
unsigned short MPU_NET_GetHKPrivatePort()
{
	MPU_NET_GET(unsigned short, ((unsigned short) 8000), NET_GetHKPrivatePort());
}

//!\brief   设置HK Private端口
//!\param   usPort          端口号（例：8000）
//!\return  0表示成功，其它为错误码
int MPU_NET_SetHKPrivatePort(unsigned short usPort)
{
	MPU_NET_CALL(NET_SetHKPrivatePort(usPort));
}

//!\brief   获取Mac地址
//!\param   pszMacAddress   返回Mac地址（例 00-1A-4D-2B-91-64）
//!\return  本接口拷贝Mac地址到pszMacAddress指向的buf，并返回pszMacAddress
char *MPU_NET_GetMacAddress(char *pszMacAddress)
{
	MPU_NET_GET(char *, pszMacAddress, NET_GetMacAddress(pszMacAddress));
}

//!\brief       设置IP冲突联动动作
//!\param   nActionType 类型
//!     ACTION_BEEP                     声音提示
//!      ACTION_SENDTOSERVER        上传报警服务器
//!      ACTION_EMAIL                       联动Email
//!      ACTION_SMS                     联动短信   
//!      ACTION_ALARMOUT                报警输出
//!\param   bEnable 如果是报警输出则为端口号掩位,其他表示使能
int MPU_NET_SetIPAction(int nActionType, int bEnable)
{
	MPU_NET_CALL(NET_SetIPAction(nActionType, bEnable));
}

//!\brief       获取IP冲突联动动作
//!\param nActionType 类型
//  !      ACTION_BEEP 声音提示
//  !      ACTION_SENDTOSERVER 上传报警服务器
//  !      ACTION_EMAIL 联动Email
//  !      ACTION_SMS 联动短信   
//  !      ACTION_ALARMOUT 报警输出
//!\return 返回是否设置，如果是报警输出则返回端口号掩位
int MPU_NET_GetIPAction(int nActionType)
{
	MPU_NET_GET(int, 0, NET_GetIPAction(nActionType));
}

#if defined (__USE_FTP__)

/*--------------------------------------------------------------------------*/
//!\brief      获取FTP传输类型
//!\param      p_transfer_type 传输类型(0:不传 1:图片 2:录像 3:ALL )
//!\return     0表示成功，其它为错误码
int MPU_NET_GetFtpTransferType(int *p_transfer_type)
{
	MPU_NET_GET(int, 0, NET_GetFtpTransferType(p_transfer_type));
}

//!\brief      设置FTP相关参数
//!\param      pFTP_user_param 类型
int MPU_NET_SetFtpParam(pFTP_user_param pFtpUserCfg)
{
	MPU_NET_CALL(NET_SetFtpParam(pFtpUserCfg));
}

//!\brief      获取FTP相关参数
//!\param      pFTP_user_param 类型
int MPU_NET_GetFtpParam(pFTP_user_param pFtpUserCfg)
{
	MPU_NET_GET(int, 0, NET_GetFtpParam(pFtpUserCfg));
}

//!\brief      获取FTP主机状态
//!\param      void 类型
//!\return     0未连接，1连接成功
int MPU_NET_GetFtpParam()
{
	MPU_NET_GET(int, 0, NET_GetFtpConnStatus());
}
/*--------------------------------------------------------------------------*/

#endif

//!\brief      获取当前哪一个网卡有效
//!\param      pCurrNetCardType 类型
int MPU_NET_GetCurrNetCardType(int *pCurrNetCardType)
{
	MPU_NET_GET(int, 1, NET_GetCurrNetCardType(pCurrNetCardType));
}

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
int MPU_NET_SetIPCallBack(int nAlarmType, CBAlert pCallback)
{
	if (NULL == g_pMpuInfo)
	{
		return IDVR_ERROR_INVALID_POINTER;
	}

	if (ALERT_TYPE_IPCONFLICT == nAlarmType)
	{
		g_pMpuInfo->m_pCallback_IpConflict = pCallback;
		MPU_NET_CALL(NET_SetIPCallBack(nAlarmType, callback_AlarmAction));
	}
	else
	{
		MPU_ERR("ALERT_TYPE_IPCONFLICT != nAlarmType\n");
		MPU_CHK(ALERT_TYPE_IPCONFLICT == nAlarmType);
		return IDVR_ERROR_PARAMETER;
	}
}

//!\brief       设置网线断联动动作
//!\param   nActionType 类型
//!     ACTION_BEEP                     声音提示
//!      ACTION_SENDTOSERVER        上传报警服务器
//!      ACTION_EMAIL                       联动Email
//!      ACTION_SMS                     联动短信   
//!      ACTION_ALARMOUT                报警输出
//!\param   bEnable 如果是报警输出则为端口号掩位,其他表示使能
int MPU_NET_SetPHYAction(int nActionType, int bEnable)
{
	MPU_NET_CALL(NET_SetPHYAction(nActionType, bEnable));
}

//!\brief       获取网线断联动动作
//!\param nActionType 类型
//  !      ACTION_BEEP 声音提示
//  !      ACTION_SENDTOSERVER 上传报警服务器
//  !      ACTION_EMAIL 联动Email
//  !      ACTION_SMS 联动短信   
//  !      ACTION_ALARMOUT 报警输出
//!\return 返回是否设置，如果是报警输出则返回端口号掩位
int MPU_NET_GetPHYAction(int nActionType)
{
	MPU_NET_GET(int, 0, NET_GetPHYAction(nActionType));
}

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
int MPU_NET_SetPHYCallBack(int nAlarmType, CBAlert pCallback)
{
	if (NULL == g_pMpuInfo)
	{
		return IDVR_ERROR_INVALID_POINTER;
	}

	if (ALERT_TYPE_RETICLEDISCONNECT == nAlarmType)
	{
		g_pMpuInfo->m_pCallback_NetworkDisconnect = pCallback;
		MPU_NET_CALL(NET_SetPHYCallBack(nAlarmType, callback_AlarmAction));
	}
	else
	{
		MPU_ERR("ALERT_TYPE_RETICLEDISCONNECT != nAlarmType\n");
		MPU_CHK(ALERT_TYPE_RETICLEDISCONNECT == nAlarmType);
		return IDVR_ERROR_PARAMETER;
	}
}

//!\brief 获取网络状态
//!\return  <-1-错误;0-正常;1-断开;2-未知状态；3-IP冲突;4-IP更新
int MPU_NET_GetStatus()
{
	MPU_NET_GET(int, 0, NET_GetStatus());
}


int MPU_ETH0_GetDNS(char *PrimaryDNS, char *SecondaryDNS)
{
	MPU_NET_GET(int, -1, ETH0_GetDNS(PrimaryDNS, SecondaryDNS));
}

int MPU_NETCARD_GetDhcpSuccTime(int	      	   *nEth0DhcpSuccTime, int *nWlan0DhcpSuccTime)
{
	MPU_NET_GET(int, -1, NETCARD_GetDhcpSuccTime(nEth0DhcpSuccTime, nWlan0DhcpSuccTime));
}

int MPU_ETH0_SetDNS(char *PrimaryDNS, char *SecondaryDNS)
{
	MPU_NET_CALL(ETH0_SetDNS(PrimaryDNS, SecondaryDNS));

}


#if defined (__IS_SUPPORT_WLAN__)
//!\brief   获取是否启用DHCP
//!\return  TRUE表示启用，FALSE表示不启用
BOOL MPU_WLAN_GetDhcpEnable()
{
	MPU_NET_GET(BOOL, TRUE, WLAN_GetDhcpEnable());
}

//!\brief   设置是否启用DHCP
//!\param   bEnable     TRUE表示启用，FALSE表示不启用
//!\return  0表示成功，其它为错误码
int MPU_WLAN_SetDhcpEnable(BOOL bEnable)
{
	MPU_NET_CALL(WLAN_SetDhcpEnable(bEnable));
}

//!\brief   手动更新DHCP
//!\return  0表示成功，其它为错误码
int MPU_WLAN_RenewDhcp()
{
	MPU_NET_CALL(WLAN_RenewDhcp());
}

//!\brief   获取IP地址
//!\param   pszIPAddress    返回IP地址（例 172.1.10.2）
//!\return  本接口拷贝IP地址到pszIPAddress指向的buf，并返回pszIPAddress
char *MPU_WLAN_GetIPAddress(char *pszIPAddress)
{
	MPU_NET_GET(char *, pszIPAddress, WLAN_GetIPAddress(pszIPAddress));
}

//!\brief   设置IP地址
//!\param   pszIPAddress    IP地址（例 172.1.10.2）
//!\return  0表示成功，其它为错误码
int MPU_WLAN_SetIPAddress(const char *pszIPAddress)
{
	MPU_NET_CALL(WLAN_SetIPAddress(pszIPAddress));
}

//!\brief   获取子网掩码地址
//!\param   pszSubnetMask   返回子网掩码（例 255.255.0.0）
//!\return  本接口拷贝子网掩码到pszSubnetMask指向的buf，并返回pszSubnetMask
char *MPU_WLAN_GetSubnetMask(char *pszSubnetMask)
{
	MPU_NET_GET(char *, pszSubnetMask, WLAN_GetSubnetMask(pszSubnetMask));
}

//!\brief   设置子网掩码地址
//!\param   pszSubnetMask   子网掩码（例 255.255.0.0）
//!\return  0表示成功，其它为错误码
int MPU_WLAN_SetSubnetMask(const char *pszSubnetMask)
{
	MPU_NET_CALL(WLAN_SetSubnetMask(pszSubnetMask));
}

//!\brief   获取网关地址
//!\param   pszGateway      返回网关地址（例 172.1.10.1）
//!\return  本接口拷贝网关地址到pszGateway指向的buf，并返回pszGateway
char *MPU_WLAN_GetGateway(char *pszGateway,int GatewaySize)
{
	MPU_NET_GET(char *, pszGateway, WLAN_GetGateway(pszGateway,GatewaySize));
}

//!\brief   设置网关地址
//!\param   pszGateway      网关地址（例 172.1.10.1）
//!\return  0表示成功，其它为错误码
int MPU_WLAN_SetGateway(const char *pszGateway)
{
	MPU_NET_CALL(WLAN_SetGateway(pszGateway));
}

//!\brief   获取Mac地址
//!\param   pszMacAddress   返回Mac地址（例 00-1A-4D-2B-91-64）
//!\return  本接口拷贝Mac地址到pszMacAddress指向的buf，并返回pszMacAddress
char *MPU_WLAN_GetMacAddress(char *pszMacAddress)
{
	MPU_NET_GET(char *, pszMacAddress, WLAN_GetMacAddress(pszMacAddress));
}

//!\brief   设置DNS地址
//!\param   char * PrimaryDNS, char * SecondaryDNS  主备DNS地址
//!         PrimaryDNS!=NULL,SecondaryDNS可以为空
//!\return  0成功，其它失败
//##ModelId=49BB403B030E
int MPU_WLAN_SetDNS(char *PrimaryDNS, char *SecondaryDNS)
{
	MPU_NET_CALL(WLAN_SetDNS(PrimaryDNS, SecondaryDNS));

}

//!\brief   获取DNS地址
//!\param   char * PrimaryDNS, char * SecondaryDNS      返回DNS地址 
//!         PrimaryDNS!=NULL,SecondaryDNS可以为空
//!\return  0成功，其它失败
//##ModelId=49BB403B030E
int MPU_WLAN_GetDNS(char *PrimaryDNS, char *SecondaryDNS, int SecondarySize)
{
	MPU_NET_GET(int, -1, WLAN_GetDNS(PrimaryDNS, SecondaryDNS, SecondarySize));
}


//!\brief   获取WLAN_DHCP状态
//!\return  /*0-未连接,1-获取成功,2-正在获取,3-获取失败,继续获取*/

int MPU_WLAN_GetDhcpState()
{
	MPU_NET_GET(int, -1, WLAN_GetDhcpState());

}


//!\brief   获取无线热点列表
//!\param   pAccessPoints   热点数组
//!\param   pnAPCount 热点个数
//!\return  本接口拷贝热点列表到pAccessPoints指向的buf，成功返回0，其他为错误码
int MPU_WLAN_GetAccessPoints(WLAN_ACCESSPOINT pAccessPoints[WLAN_MAX_AP_NUM], int *pnAPCount)
{
	MPU_NET_GET(int, 0, WLAN_GetAccessPoints(pAccessPoints, pnAPCount));
}

//!\brief   获取无线连接参数
//!\param   连接参数
//!\return  本接口拷贝连接到pConnection指向的buf，成功返回0，其他为错误码
int MPU_WLAN_GetConnection(LPWLAN_CONNECTION pConnection)
{
	MPU_NET_GET(int, 0, WLAN_GetConnection(pConnection));
}

int MPU_WLAN_SetConnection(LPWLAN_CONNECTION pConnection)
{
	MPU_NET_CALL(WLAN_SetConnection(pConnection));
}

int MPU_WLAN_GetWiFiModuleEnable()
{
	MPU_NET_GET(int, 0, WLAN_GetWiFiModuleEnable());
}

int MPU_WLAN_SetEnable(int bEnable)
{
	MPU_NET_CALL(WLAN_SetEnable(bEnable));
}

//!\brief 获取网络状态
//!\return  <-1-错误;0-正常;1-断开;2-未知状态；3-IP冲突;4-IP更新
int MPU_WLAN_GetStatus()
{
	MPU_NET_GET(int, 0, WLAN_GetStatus());
}

int MPU_WLAN_SetConnectionEx(LPWLAN_CONNECTION pConnection)
{
	MPU_NET_CALL(WLAN_SetConnectionEx(pConnection));
}

int MPU_WLAN_SetTestConnection(LPWLAN_CONNECTION pConnection)
{
	MPU_NET_CALL(WLAN_SendTestConfigure(pConnection));
}

/* 获取测试wifi的状态 return -1:必须先调用设置接口 0:断开  1:连上 */
int MPU_WLAN_GetWifiTestStatus()
{
	MPU_NET_GET(int, 0, WLAN_GetWifiTestStatus());
}
//是否正在进行wifi配置
int MPU_WLAN_IsConfigureNetIng()
{
    MPU_NET_GET(int, 0, WLAN_IsConfigureNetIng());
}

//保存二维码配网交互配对信息
int MPU_WLAN_SaveQrConfigureWlanSessionID(char *pszData)
{
	MPU_NET_CALL(WLAN_SaveQrConfigureWlanSessionID(pszData));
}

//获取二维码配网交互配对信息
int MPU_WLAN_GetQrConfigureWlanSessionID(char *pszData, int nBufLen)
{
	MPU_NET_GET(int, 0, WLAN_GetQrConfigureWlanSessionID(pszData, nBufLen));
}

//获取wifi配网状态:0-等待配网 1-连接wifi中 2-wifi连接成功(wifi连接失败，重新返回等待配网)
int MPU_WLAN_GetWifiLinkStatus(int *pnStatus)
{
	MPU_NET_GET(int, 0, WLAN_GetWifiLinkStatus(pnStatus));
}

int MPU_NET_SetWlanScanStatus(int *pWlanScanStatus)
{
	MPU_NET_GET(int, 0, NET_SetWlanScanStatue(pWlanScanStatus));
}


//设置配网客户端地址信息
int MPU_WLAN_SetClientAddrInfo(unsigned int u32Ip, unsigned int u32Port)
{
    MPU_NET_CALL(WLAN_SetClientAddrInfo(u32Ip, u32Port));
}

/* 获取当前wifi是STA模式还是AP模式*/
int MPU_NET_GetCurrWifiMode(int *pCurrWifiMode)
{
	MPU_NET_GET(int, 0, NET_GetCurrWifiMode(pCurrWifiMode));
}

int MPU_NET_SetWifiMode(unsigned int mode, int force)
{
	MPU_NET_CALL(NET_SetWifiMode(mode, force));
}

#endif

/* 获取当前TCP转发是否开启 */
int MPU_Net_TcpRelay_GetEnable(int *bEnable)
{
	MPU_NET_GET(int, 0, TcpRelay_GetEnable(bEnable));
}

/* 获取当前TCP转发是否开启 */
int MPU_Net_TcpRelay_SetEnable(int *bEnable)
{
	MPU_NET_CALL(TcpRelay_SetEnable(bEnable));
}

int MPU_NET_SetTutkUIDEnable(int bEnable)
{
	MPU_NET_CALL(NET_SetTutkUIDEnable(bEnable));
}

int MPU_NET_GetTutkUIDEnable()
{
	MPU_NET_GET(int, 0, NET_GetTutkUIDEnable());
}

int MPU_NET_SetTutkUID(TutkCfg *pTutkCfg, int pSetMethod)
{
	MPU_NET_CALL(NET_SetTutkUID(pTutkCfg, pSetMethod));
}

#if defined (__AUTH_V2__)
int MPU_NET_SetConnectString(P2P_CONNECT_INFO *pConnectString, const unsigned char *pszAuthLicense)
{
	MPU_NET_CALL(NET_SetConnectString(pConnectString, pszAuthLicense));
}
#endif

char *MPU_NET_GetTutkUID(TutkCfg *pTutkCfg, unsigned long *pTutkStatus)
{
	MPU_NET_GET(char *, NULL, NET_GetTutkUID(pTutkCfg, pTutkStatus));
}

#if defined (__AUTH_V2__)
int MPU_NET_GetConnectString(P2P_CONNECT_INFO *pConnectString)
{
	MPU_NET_GET(int, 0, NET_GetConnectString(pConnectString));
}
#endif

int MPU_NET_GetTutkStatus(int *pTutkStatus)
{
	MPU_NET_GET(int, 0, NET_GetTutkStatus(pTutkStatus));
}

int MPU_NET_SetTutkStatus(int *pTutkStatus)
{
	MPU_NET_GET(int, 0, NET_SetTutkStatus(pTutkStatus));
}

#if defined (__USE_APPLY_DID_MANUAL__)
int MPU_NET_SetApplyDIDRequestUrl(char *pRequestUrl)
{
	MPU_NET_CALL(NET_SetApplyDIDRequestUrl(pRequestUrl));
}
#endif

//===================================================================================
int MPU_NET_GetUlkUIDEnable()
{
	MPU_NET_GET(int, 0, NET_GetUlkUIDEnable());
}

int MPU_NET_SetUlkUID(char bEnable, const char *pUlkUID)
{
	MPU_NET_CALL(NET_SetUlkUID(bEnable, pUlkUID));
}

int MPU_NET_NeedUpdateRouter(void)
{
	MPU_NET_CALL(NET_NeedUpdateRouter());
}

char *MPU_NET_GetUlkUID(char *bEnable, char *pUlkUID, int *pUlkStatus)
{
	MPU_NET_GET(char *, pUlkUID, NET_GetUlkUID(bEnable, pUlkUID, pUlkStatus));
}
	
int MPU_NET_SetUlkStatus(int *pUlkStatus)
{
	MPU_NET_CALL(NET_SetUlkStatus(pUlkStatus));
}

int MPU_NET_SetULKUID(char *pUlkUid)
{
	MPU_NET_CALL(NET_SetUUID(pUlkUid));
}

int MPU_NET_GetP2PDownloadURL(char *pDownloadURL)
{
	MPU_NET_GET(int, 0, NET_GetP2PDownloadURL(pDownloadURL));
}

/*推送事件 */
int MPU_Net_ZwP2pPushEvent(ZWP2P_UINT32 nEventType, 
	ZWP2P_UINT32 nChannel, LPZWP2P_PUSH_EVENT_INFO pEventInfo)
{
	MPU_NET_CALL(Net_ZwP2pPushEvent(nEventType, nChannel, pEventInfo));
}

/* 获取当前用的DID是否支持Subkey */
int MPU_NET_IsSupportSetSubkeyByDID()
{
    MPU_NET_GET(int, 0, Net_IsSupportSetSubkeyByDID());
}

/* 设置PushAi参数 */
int MPU_NET_SetPushAiParam(const char *szPushAiParam)
{
    MPU_NET_CALL(Net_SetPushAiParam(szPushAiParam));
}

/* 获取PushAi参数 */
int MPU_NET_GetPushAiParam(char *szPushAiParam)
{
    MPU_NET_GET(int, 0, Net_GetPushAiParam(szPushAiParam));
}

/* 检测当前网络是否可以连接互联网(*pConnStatus=1表示可以联通互联网) */
int MPU_Net_CheckIfNetAccessToInternet(int *pConnStatus)
{
    MPU_NET_GET(int, 0, Net_CheckIfNetAccessToInternet(pConnStatus));
}

#if defined (__IS_SUPPORT_P6SMESSAGE__)
//获取云消息URL
int MPU_NET_GetMpServerParamCfg(char *ServerIpAndPort, char *UrlPathTimeCheck, char *UrlPathEventPolicy, char *UrlPathEventPost, char *UrlPathSetXmlStatus, char *SrvMaintenance)
{
	MPU_NET_GET(int , 0 , NET_GetMpServerParamCfg(ServerIpAndPort, UrlPathTimeCheck, UrlPathEventPolicy, UrlPathEventPost, UrlPathSetXmlStatus, SrvMaintenance));
}

//设置云消息URL
int MPU_NET_SetMpServerParamCfg(char *ServerIpAndPort, char *UrlPathTimeCheck, char *UrlPathEventPolicy, char *UrlPathEventPost, char *UrlPathSetXmlStatus, char *SrvMaintenance)
{
	MPU_NET_CALL(NET_SetMpServerParamCfg(ServerIpAndPort, UrlPathTimeCheck, UrlPathEventPolicy, UrlPathEventPost, UrlPathSetXmlStatus, SrvMaintenance));
}
#endif

#ifdef __MODULE_4G_SUPPORT__
int MPU_NET_SetConnServerStatus(int ConnServer)
{
	MPU_NET_CALL(SetConnServerStatus(ConnServer));
}

/*设置开始执行产测流程*/
int MPU_Net_SetProductTest(int isTest)
{
	MPU_NET_CALL(Net_SetProductTest(isTest));
}

/*获取4G测试数据*/
int MPU_Net_Get4GProductTestData(Module4G_ProductTest_Data_t *pData)
{
	MPU_NET_GET(int, 0, Net_Get4GProductTestData(pData));
}


/*设置4G模块配置*/
int MPU_Net_Set4GControlConfig(HY_DVR_4G_CARD_SELECT_CFG_INFO *pcfg)
{
	MPU_NET_CALL(Net_Set4GControlConfig(pcfg));
}

/*获取4G模块配置*/
int MPU_Net_Get4GControlConfig(HY_DVR_4G_CARD_SELECT_CFG_INFO *pcfg)
{
	MPU_NET_GET(int, 0, Net_Get4GControlConfig(pcfg));
}

/*获取4G切卡结果*/
int MPU_Net_Get4GSwitchResult(HY_DVR_4G_CARD_SELECT_RESULT_INFO *pcfg)
{
	MPU_NET_GET(int, 0, Net_Get4GSwitchResult(pcfg));
}


/* 获取4G模组信息 */
int MPU_Net_Get4GModuleInfo(char *Version,int *iSignal4G)
{
	MPU_NET_GET(int, 0, Net_Get4GModuleInfo(Version,iSignal4G));
}


/* 获取当前是否是P2P产测模式 */
int MPU_Net_IsP2PProductTestMode()
{
	MPU_NET_GET(int, 0, Net_IsP2PProductTestMode());
}

/* 获取当前4G工作模式 */
int MPU_Net_Module4G_GetCardBindMode()
{
	MPU_NET_GET(int, 0, Module4G_GetCardBindMode());
}


/* 获取当前SIM卡的信息 */
int MPU_NET_GetSimCardInfo(char *imsi, char *imei, char *act, char *oper, char *band, char *iccid,char *version)
{
    MPU_NET_GET(int, 0, Net_GetSimCardInfo(imsi, imei, act, oper, band, iccid,version));
}

/* 获取当前模块的信号强度 */
int MPU_Net_GetGPRSModuleSignalQuality(int *signal_quality)
{
    MPU_NET_GET(int, 0, Net_GetGPRSModuleSignalQuality(signal_quality));
}

/* 检查Sim卡的状态0:成功 1:SIM卡锁住 2:失败 */
int MPU_NET_GPRS_GetSimCardStatus(int * sim_card_status)
{
    MPU_NET_GET(int, 0, NET_GPRS_GetSimCardStatus(sim_card_status));
}

/* 检查Sim卡的状态0:成功 1:失败 */
int MPU_NET_GPRS_GetSimCardStatus1(int * sim_card_status)
{
    MPU_NET_GET(int, 0, NET_GPRS_GetSimCardStatus1(sim_card_status));
}


/* 检测当前GPRS模块是否可以连接互联网 */
int MPU_Net_CheckIfGPRSAccessToInernet(int *gprs_status)
{
	MPU_NET_GET(int, 0, Net_CheckIfGPRSAccessToInernet(gprs_status));
}


/*金睿平台appid获取*/
int MPU_Net_GPRS_FL_Appid(char *szFlAppid)
{
	MPU_NET_CALL(NET_GPRS_GetFlAppid(szFlAppid));
}


/*金睿平台appkey获取*/
int MPU_Net_GPRS_FL_Appkey(char *szFlAppkey)
{
	MPU_NET_CALL(NET_GPRS_GetFlAppkey(szFlAppkey));
}


//获取北纬平台appid
int MPU_NET_GPRS_GetNorthLatiAppid(char  * gprs_appid)
{
	MPU_NET_CALL(NET_GPRS_GetNorthLatiAppid(gprs_appid));
}

//获取北纬平台key
int MPU_NET_GPRS_GetNorthLatiAppkey(char	* gprs_appkey)
{
	MPU_NET_CALL(NET_GPRS_GetNorthLatiAppkey(gprs_appkey));
}

//获取北纬平台cardtype
int MPU_NET_GPRS_GetNorthLatiCardType(char  * gprs_cardtype,int len)
{
	MPU_NET_CALL(NET_GPRS_GetNorthLatiCardType(gprs_cardtype,len));
}


/*获取卡类型*/
int MPU_Net_GPRS_FL_SimCardType(int *nType)
{
	MPU_NET_CALL(Net_GPRS_FL_SimCardType(nType));
}


/*获取允许联网时间*/
int MPU_Net_GPRS_FL_AllowTime(int *nTime)
{
	MPU_NET_CALL(Net_GPRS_FL_AllowTime(nTime));
}

//设置卡类型
int MPU_NET_SetSimCardType(int  * simcard_type)
{
	MPU_NET_CALL(NET_SetSimCardType(simcard_type));
}

//获取云卡工作模式
int MPU_Net_GetUcWorkMode(int *piUcWorkMode)
{
    MPU_NET_GET(int ,0, Net_GetUcWorkMode(piUcWorkMode));
}

//获取是否是云卡设备
int MPU_Net_GetIsCloudCardDev(void)
{
    MPU_NET_GET(int ,0, Net_GetIsCloudCardDev());
}

/*获取双sim卡信息*/
int MPU_Net_GetSimIndexCardInfo(int nSim, int* nIsInUse, char *szOperator, int *nSignalStrength,
			char *szICCID, char *szIMEI, char *sz4GModuleVersion)
{
	MPU_NET_GET(int, 0, Net_GetSimIndexCardInfo(nSim, nIsInUse, szOperator, nSignalStrength, szICCID, szIMEI, sz4GModuleVersion));
}


int MPU_Net_SetScanEndStatus(int scanend)
{
    MPU_NET_CALL(SetScanEndStatus(scanend));
}

int MPU_Net_SetUserId(char* userid)
{
    MPU_NET_CALL(SetUserId(userid));
}

int MPU_Net_SetServerAddress(char* serveraddress)
{
    MPU_NET_CALL(SetServerAddress(serveraddress));
}

int MPU_Net_SetRandomNumber(char* randomnumber)
{
    MPU_NET_CALL(SetRandomNumber(randomnumber));
}
#endif

int MPU_NET_SetAuthInfoPushDomainSuffix(P2P_TRANSPARENT_INFO_PUSHDOMAIN *pPushDomainParam)
{
	MPU_NET_CALL(NET_SetAuthInfoPushDomainSuffix(pPushDomainParam));
}

int  MPU_NET_GetAuthInfoPushDomainSuffix(P2P_TRANSPARENT_INFO_PUSHDOMAIN *pPushDomainParam)
{
	MPU_NET_CALL(NET_GetAuthInfoPushDomainSuffix(pPushDomainParam));
}

int  MPU_Net_ClearAuthDomainSuffix()
{
	MPU_NET_CALL(Net_ClearAuthDomainSuffix());
}

int  MPU_Net_GetAuthDomainSuffixExist(int *status)
{
	MPU_NET_CALL(Net_GetAuthDomainSuffixExist(status));
} 

int  MPU_Net_GetTutkAndDomainSuffix(char *Tutk, int TutkLen, char *Domainsuffix, int DomainLen)
{
	MPU_NET_CALL(Net_GetTutkAndDomainSuffix(Tutk, TutkLen, Domainsuffix, DomainLen));
}

int  MPU_Net_GetAuthDomainSuffixExist_Test(int *status)
{
	MPU_NET_CALL(Net_GetAuthDomainSuffixExist_Test(status));
} 

int  MPU_Net_GetTutkAndDomainSuffix_Test(char *Tutk, int TutkLen, char *Domainsuffix, int DomainLen)
{
	MPU_NET_CALL(Net_GetTutkAndDomainSuffix_Test(Tutk, TutkLen, Domainsuffix, DomainLen));
}


#if defined (__IS_SUPPORT_4G_CLOUD_DOMAIN__)
int MPU_NET_AuthInfoTransparentV2(TransParent_INFO *pTransParentInfo)
{
	MPU_NET_CALL(NET_SetDomainListManagerServer(pTransParentInfo));
}

int MPU_NET_SetAddDomainForGetAlias(char *pDomain)
{
	MPU_NET_CALL(NET_AddDomainForGetAlias(pDomain));
}

int MPU_NET_SetAddDomainList2Local(const NET_DOMAIN_INFO_LIST DomainList)
{
	MPU_NET_CALL(NET_SetAddDomainList2Local(DomainList));
}
#endif

//!\brief   获取当前连接wifi信号强度
//!\param   pSignal   热点信号值
//!\return  本接口返回热点信号值，成功返回0，其他为错误码
int MPU_WLAN_GetAccessSignalInfo(LPWLAN_ACCESSSIGNALINFO pSignalInfo)
{
	MPU_NET_GET(int, 0, WLAN_GetAccessSignalInfo(pSignalInfo));
}

int MPU_WLAN_GetAccessSignalQuality(int *nQuality)
{
    MPU_NET_GET(int, 0, WLAN_GetAccessSignalQuality(nQuality));
}

#ifdef __IS_SUPPORT_AIC_BLUETOOTH__

//设置应用层蓝牙收数据回调接口
int MPU_NET_SetAppBlueToothRecvCallback(AppRecvProcessCallback pFunc)
{
	MPU_NET_CALL(NET_SetAppBlueToothRecvCallback(pFunc));
}

int MPU_WLAN_BluetoothProTestStatus(int *status)
{
	MPU_NET_CALL(getBlueTestStatus(status));
}
#endif

//设置AuthP2p信息
//返回值：0-成功       其他-错误码
int MPU_Net_SetAuthP2pInfo(Net_AuthP2pInfo *ptAuthP2pInfo)
{
    MPU_NET_CALL(NET_SetAuthP2pInfo(ptAuthP2pInfo));
}

//获取AuthP2p信息
//返回值：0-成功       其他-错误码
int MPU_NET_GetAuthP2pInfo(Net_AuthP2pInfo *ptAuthP2pInfo)
{
	MPU_NET_GET(int, 0, NET_GetAuthP2pInfo(ptAuthP2pInfo));
}

/*
//获取wifi模组信息
//返回值：
typedef enum{
    WIFI_MODULE_NONE,
    WIFI_MODULE_AIC_8800DL,
    WIFI_MODULE_HI_H383U,
    WIFI_MODULE_ATBM_6132U,
    WIFI_MODULE_AIC_8800DL_2_4G,
}eWifiModuleType;
*/

int MPU_Net_GetWiFiUSBWifiDev()
{
	MPU_NET_GET(int, 0, NET_GetWiFiUSBWifiDev());
}



/**************************************************************************
 *                            注销局部宏定义                              *
 **************************************************************************/
#undef MPU_NET_CALL				//调用网络管理器执行动作
#undef MPU_NET_GET				//调用网络管理器获取参数值

