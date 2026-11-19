
/*!
 *\file	 MPU_CIU_Config_Net.cpp
 *\brief	MPU模块，远程配置，网络部分
 *\author	林云
 *\date	2009/8/6

功能：
-#	远程配置，网络部分

说明：
-#	本文件使用新的网络库实现
 */



#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <semaphore.h>
#include <errno.h>
#include "base64Lib.h"
#include "zwsdk.h"
#include "MPU_CIU_global.h"
#include "MPU_UPNP_API.h"
#include "MPU_Net_API.h"


#define SAVE_IP_FIRST

#if defined (__USE_FTP__)
//#include "ftp_process.h"
#include "ftpClient.h"
#endif

//异步设置网络参数
static void *network_config_thread(void *param);

static pthread_mutex_t netWorkThreadLock;
static pthread_mutex_t netThreadIDLock;

static long int netThreadWorkingID = 1;//表示该处理的线程编号
static long int netThreadReadyID = 1;//表示线程的编号

static BOOL bNeedUpdateRouter = FALSE;

static int GetEmailCfg_1(LPHY_DVR_NET_EML_1 pcfg);

static int SetEmailCfg_1(LPHY_DVR_NET_EML_1 pcfg);

#define DBG_MICRO
#ifdef DBG_MICRO
#define FUNC_BEGIN     printf("[%s:%d] ====== Into Function %s ======\n", __FILE__, __LINE__, __FUNCTION__);
#define FUNC_ENDIT(x)  printf("[%s:%d] ====== End of Function %s ret = %d======\n", __FILE__, __LINE__, __FUNCTION__, x);
#else
#define FUNC_BEGIN
#define FUNC_ENDIT(x)
#endif

//异步设置参数
typedef struct tag_network_config
{
	DWORD m_dwUserID;
	DWORD m_dwMajorType;
	DWORD m_dwMinorType;
	BYTE *m_pBuffer;
	DWORD m_dwBufLen;
	long int threadId;

	  tag_network_config()
	{
		MPU_DBG("enter\n");

		m_dwUserID = (DWORD) - 1;
		m_dwMajorType = 0;
		m_dwMinorType = 0;
		m_pBuffer = NULL;
		m_dwBufLen = 0;

		MPU_DBG("ok\n");
	}

	 ~tag_network_config()
	{
		MPU_DBG("enter\n");

		if (NULL != m_pBuffer)
		{
			delete[]m_pBuffer;
			m_pBuffer = NULL;

			MPU_DBG("delete\n");
		}

		MPU_DBG("ok\n");
	}

	int set(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, const BYTE * pBuffer, DWORD dwBufLen)
	{
		int nRet = IDVR_OK, nSDKRet;

		pthread_attr_t attr;

		pthread_t thrd_id;		//线程ID

		MPU_DBG("enter\n");
		if (NULL == pBuffer)
		{
			MPU_ERR("NULL == pBuffer\n");
			return IDVR_ERROR_PARAMETER_INVALID_POINTER;
		}

		if (0 == dwBufLen)
		{
			MPU_ERR("0 == dwBufLen\n");
			return IDVR_ERROR_PARAMETER_OVERFLOW;
		}

		m_pBuffer = new unsigned char[dwBufLen];

		if (NULL == m_pBuffer)
		{
			MPU_ERR("NULL == m_pBuffer\n");
			return IDVR_ERROR_ALLOC;
		}

		m_dwUserID = dwUserID;
		m_dwMajorType = dwMajorType;
		m_dwMinorType = dwMinorType;

		memcpy(m_pBuffer, pBuffer, dwBufLen);
		m_dwBufLen = dwBufLen;

		//初始话线程属性结构体
		nSDKRet = pthread_attr_init(&attr);
		if (0 != nSDKRet)
		{
			MPU_ERR("pthread_attr_init() return %d\n", nSDKRet);
			MPU_CHK(0 == nSDKRet);
		}

		//第二个参数可选为分离线程
		nSDKRet = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);//分离线程返回后自己会释放内存
		if (0 != nSDKRet)
		{
			MPU_ERR("pthread_attr_setdetachstate( PTHREAD_CREATE_DETACHED ) return %d\n", nSDKRet);
			MPU_CHK(0 == nSDKRet);
		}

		pthread_mutex_lock(&netThreadIDLock);
		threadId = netThreadReadyID++;
		pthread_mutex_unlock(&netThreadIDLock);

		//创建分离线程
		nSDKRet = pthread_create(&thrd_id, &attr, network_config_thread, (void *) this);
		if (0 != nSDKRet)
		{
			MPU_ERR("pthread_create( network_config_thread ) DETACHED return %d\n", nSDKRet);
			MPU_CHK(0 == nSDKRet);

			nRet = IDVR_CIU_CALLSDK_FAIL;
		}

		nSDKRet = pthread_attr_destroy(&attr);
		if (0 != nSDKRet)
		{
			MPU_ERR("pthread_attr_destroy() return %d\n", nSDKRet);
			MPU_CHK(0 == nSDKRet);
		}

		MPU_DBG("return %d\n", nRet);
		return nRet;
	}
}

NETWORK_CONFIG, *LPNETWORK_CONFIG;


static BOOL g_bNeedReset = FALSE;
static unsigned int iuPnPStatus = 0xF;
static char szExAddress[DVR_MAX_IP_STR_LEN] = "0.0.0.0";
static int  b_GetCfg = 0;
static int b_Upnp_Enable = 0;

static int restartRtsp()
{
printf("-------------restartRtsp-------\n");
	int nSDKRet = 0;
	nSDKRet = HieServer_DetachProtocol( 3 );
	if(nSDKRet)
	{
		MPU_ERR("HieServer_DetachProtocol() return %d\n", nSDKRet);
		MPU_CHK(0 == nSDKRet);
	}

	nSDKRet = HieServer_LoadProtocol( 3 );
	if(nSDKRet)
	{
		MPU_ERR("HieServer_LoadProtocol() return %d\n", nSDKRet);
		MPU_CHK(0 == nSDKRet);
	}
	printf("-------------restartRtsp-----return --\n");

	return 0;
}

static int SetTutkUIDCfg(LPZW_SYS_NET_CFG pcfg)
{
	int nSDKRet = IDVR_OK;
	TutkCfg stuTutkCfg = {0};

	//! 本结构体是否有效，=0表示无效
	if (0 == pcfg->bValid)
	{
		MPU_DBG("0 == pcfg->bValid\n");
		return IDVR_CIU_STRUCT_UNVALID;
	}
	//MPU_DBG("--set-bTutkEnable %d-----acTutkUID %s------\n", pcfg->bTutkEnable, pcfg->acTutkUID);
	memset(&stuTutkCfg, 0, sizeof(stuTutkCfg));
	stuTutkCfg.bEnable = pcfg->bTutkEnable;
	strncpy(stuTutkCfg.szTutkUID, pcfg->acTutkUID, sizeof(stuTutkCfg.szTutkUID) - 1);
	strncpy(stuTutkCfg.szPushID, pcfg->strPushNum, sizeof(stuTutkCfg.szPushID) - 1);
	//nSDKRet = MPU_NET_SetTutkUID(&stuTutkCfg, 0);
	MPU_CHK(0 == nSDKRet);

	return (0 == nSDKRet) ? IDVR_OK : IDVR_CIU_CALLSDK_FAIL;
}


static int GetTutkUIDCfg(LPZW_SYS_NET_CFG pcfg)
{
	int nSDKRet = IDVR_OK;
	TutkCfg stuTutkCfg = {0};
	
	pcfg->bValid = 1;
	MPU_NET_GetTutkUID(&stuTutkCfg, &pcfg->dwTutkStatus);
	pcfg->bTutkEnable = stuTutkCfg.bEnable;
    #if defined (__USE_APPLY_DID_MANUAL__)
	pcfg->iErrCode = stuTutkCfg.iErrCode;
	pcfg->iDidGetStatus = stuTutkCfg.iStatus;
    #endif
	strncpy(pcfg->acTutkUID, stuTutkCfg.szTutkUID, sizeof(pcfg->acTutkUID) - 1);
	pcfg->acTutkUID[sizeof(pcfg->acTutkUID)-1] = '\0';
	strncpy(pcfg->strPushNum, stuTutkCfg.szPushID, sizeof(pcfg->strPushNum) - 1);
	pcfg->strPushNum[sizeof(pcfg->strPushNum)-1] = '\0';
	//MPU_DBG("--get-bTutkEnable %d-----acTutkUID %s---Status %d---\n", pcfg->bTutkEnable, pcfg->acTutkUID, pcfg->dwTutkStatus);

	return (0 == nSDKRet) ? IDVR_OK : IDVR_CIU_CALLSDK_FAIL;
}


static int SetTutkStatus(int *pTutkStatus)
{
	int nSDKRet = IDVR_OK;

	nSDKRet = MPU_NET_SetTutkStatus(pTutkStatus);
	
	//MPU_DBG("--set-bTutk status %d---------\n", *pTutkStatus);

	return (0 == nSDKRet) ? IDVR_OK : IDVR_CIU_CALLSDK_FAIL;
}

/* 获取ULK相关状态 */
static int GetUlkUIDCfg(LPZW_SYS_NET_CFG pcfg)
{
	int nSDKRet = IDVR_OK;

	pcfg->bValid = 1;

	MPU_NET_GetUlkUID(&pcfg->bUlucoP2PEnable, pcfg->acUlucoP2PDID, &pcfg->dwUlucoP2PStatus);
	
	//MPU_DBG("--get-bUlkEnable %d-----acUlkUID %s---Status %ld---\n", pcfg->bUlucoP2PEnable, pcfg->acUlucoP2PDID, pcfg->dwUlucoP2PStatus);

	return (0 == nSDKRet) ? IDVR_OK : IDVR_CIU_CALLSDK_FAIL;
}

/* 设置ULK的UID(从上层获取) */
static int SetUlkUIDCfg(LPZW_SYS_NET_CFG pcfg)
{
	int nSDKRet = IDVR_OK;

	if (0 == pcfg->bValid)
	{
		MPU_DBG("0 == pcfg->bValid\n");
		return IDVR_CIU_STRUCT_UNVALID;
	}

    char oldEnable = 0;
    int  oldUlkStatus = 0;
    char oldUlkUID[MAX_UID] = {0};
    
	MPU_NET_GetUlkUID(&oldEnable, oldUlkUID, &oldUlkStatus);

    if ((oldEnable==pcfg->bUlucoP2PEnable) && (0==strncmp(pcfg->acUlucoP2PDID, oldUlkUID, sizeof(pcfg->acUlucoP2PDID))))
    {
        printf("ulk uid param no change!\n");
        return 0;
    }
    
	MPU_NET_SetUlkUID(pcfg->bUlucoP2PEnable, pcfg->acUlucoP2PDID);
	
//	printf("--set-bUlkEnable %d-----acUlkUID %s---\n", pcfg->bUlucoP2PEnable, pcfg->acUlucoP2PDID);

	return (0 == nSDKRet) ? IDVR_OK : IDVR_CIU_CALLSDK_FAIL;
}

static int SetIPAdjustCfg(LPZW_SYS_NET_CFG pcfg)
{
	int nSDKRet = IDVR_OK;

	if (0 == pcfg->bValid)
	{
		MPU_DBG("0 == pcfg->bValid\n");
		return IDVR_CIU_STRUCT_UNVALID;
	}

    int oldEnable = 0;
    MPU_NET_GetIPAddressAdjustEnable(&oldEnable);
    if (oldEnable == pcfg->bEnableAutoGenIP)
    {
        printf("IPAddressAdjust Enable no change\n");
        return 0;
    }

	MPU_NET_SetIPAddressAdjustEnable(pcfg->bEnableAutoGenIP);

	return (0 == nSDKRet) ? IDVR_OK : IDVR_CIU_CALLSDK_FAIL;
}

static int GetIPAdjustCfg(LPZW_SYS_NET_CFG pcfg)
{
	int nSDKRet = IDVR_OK;
	int enable = 0;

	pcfg->bValid = 1;

	MPU_NET_GetIPAddressAdjustEnable(&enable);
	pcfg->bEnableAutoGenIP = enable;
	
	return (0 == nSDKRet) ? IDVR_OK : IDVR_CIU_CALLSDK_FAIL;
}

/* 设置ULK的状态 */
static int SetUlkStatus(int *pUlkkStatus)
{
	int nSDKRet = IDVR_OK;

	nSDKRet = MPU_NET_SetUlkStatus(pUlkkStatus);
	
//	printf("--set-bULK status %d---------\n", *pUlkkStatus);

	return (0 == nSDKRet) ? IDVR_OK : IDVR_CIU_CALLSDK_FAIL;
}

#if defined (__AUTH_V2__)
static int SetConnectInfo(HY_P2P_CONNECT_INFO *pcfg)
{
    int nSDKRet = IDVR_OK;

    //设置连接串信息
    P2P_CONNECT_INFO nP2pConnectInfo = {0};
    int i = 0;
    memcpy(nP2pConnectInfo.szPlatformName, pcfg->szPlatformName, sizeof(nP2pConnectInfo.szPlatformName));
    memcpy(nP2pConnectInfo.szConnectString, pcfg->szConnectString, sizeof(nP2pConnectInfo.szConnectString));
    memcpy(nP2pConnectInfo.szAuthWord, pcfg->szAuthWord, sizeof(nP2pConnectInfo.szAuthWord));

    for(i = 0 ; i < 8; i++)
    {
        memcpy(nP2pConnectInfo.struDomain[i].szDomain, pcfg->struDomain[i].szDomain, sizeof(nP2pConnectInfo.struDomain[i].szDomain));
        memcpy(nP2pConnectInfo.struDomain[i].szIP, pcfg->struDomain[i].szIP, sizeof(nP2pConnectInfo.struDomain[i].szIP));
    }

    //nSDKRet = MPU_NET_SetConnectString(&nP2pConnectInfo, NULL);

    return (0 == nSDKRet) ? IDVR_OK : IDVR_CIU_CALLSDK_FAIL;
}
#endif

/* 设置ULK的UID */
static int SetUlkUid(char *pUlkUid)
{
    int nSDKRet = IDVR_OK;

	printf("[%s:%d] pUlkUid = %s\n", __FUNCTION__, __LINE__, pUlkUid);

    char oldEnable = 0;
    int  oldUlkStatus = 0;
    char oldUlkUID[MAX_UID] = {0};
    
	MPU_NET_GetUlkUID(&oldEnable, oldUlkUID, &oldUlkStatus);

    if (0==strncmp(pUlkUid, oldUlkUID, sizeof(oldUlkUID)))
    {
        printf("ulk uid no change!\n");
        return 0;
    }

	nSDKRet = MPU_NET_SetULKUID(pUlkUid);

	return (0 == nSDKRet) ? IDVR_OK : IDVR_CIU_CALLSDK_FAIL;
}

//获取DDNS配置信息,需要在外部对pcfg清零
static int GetDdnsCfg(LPHY_DVR_DDNS_CONF pcfg)
{
	int nRet = IDVR_OK, i, nCurrent;
	//! 本结构体是否有效，=0表示无效
	pcfg->bValid = 1;

	//支持的DDNS协议总数(只读)
	pcfg->nDdnsProtocolCount = MPU_DDNS_GetProtocolCount();
	if (pcfg->nDdnsProtocolCount > DVR_MAX_DDNSPRO_NUM)
	{
		pcfg->nDdnsProtocolCount = DVR_MAX_DDNSPRO_NUM;
	}

	/*是否启用DDNS*/
	pcfg->bEnable = MPU_DDNS_GetEnable();
	MPU_DBG("-----GetDdnsCfg------bEnable=%d----\n",pcfg->bEnable);
	/*当前使用的DDNS协议ID */
	nCurrent = MPU_DDNS_GetCurrentProtocol();

	//当前系统支持的协议列表
	for (i = 0; i < pcfg->nDdnsProtocolCount; ++i)
	{
		LPHY_DVR_DDNS pDdns = &pcfg->struDdnsProtocol[i];

		//本结构体是否有效，=0表示无效
		pDdns->bValid = 1;

		if (i == nCurrent)
		{
			/*当前选中的服务器索引*/
			pDdns->bDdnsEnable = true;
			/*当前DDNS状态 (只读) */
			pDdns->nDdnsState = MPU_DDNS_GetState();
		}else
		{
			/*当前选中的服务器索引*/
			pDdns->bDdnsEnable = false;	
		}

		/*协议名称 */
		MPU_DDNS_GetProtocolName(i, pDdns->szProtocolName);

		/*注册用户名 */
		MPU_DDNS_GetUserName(i, pDdns->szUserName);

		/*注册用户密码 */
		MPU_DDNS_GetPassword(i, pDdns->szUserPassword);

		/*注册用户域名 */
		MPU_DDNS_GetDomainName(i, pDdns->szDomainName, sizeof(pDdns->szDomainName));

		/*自动注册时间 */
		pDdns->nAutoRegIntervals = MPU_DDNS_GetAutoUpdateTime(i);
	}

	return nRet;
}

//获取DHCP配置信息,需要在外部对pcfg清零
static int GetDhcpCfg(LPHY_DVR_DHCP_CONF pcfg)
{
	int nRet = IDVR_OK;
	//! 本结构体是否有效，=0表示无效
	pcfg->bValid = 1;

	/* 0-不启用Dhcp,1-启用Dhcp */
	pcfg->bDhcpEnable = MPU_NET_GetDhcpEnableEx();

	return nRet;
}

//获取DHCP配置信息,需要在外部对pcfg清零
static int GetDhcpCfg_Local(LPZW_SYS_NET_CFG pcfg)
{
	int nRet = IDVR_OK;

	/* 0-不启用Dhcp,1-启用Dhcp */
	pcfg->dwEnableDHCP = MPU_NET_GetDhcpEnableEx();

	return nRet;
}
//获取DHCP状态信息,需要在外部对pcfg清零
static int GetDhcpState(LPHY_DVR_DHCP_STATE pcfg)
{
	//目前系统不支持获取Dhcp状态
	pcfg->bValid = 0;

//  MPU_DBG("unsupported get dhcp state\n");

	return IDVR_CIU_STRUCT_UNSUPPORTED;
}

//获取Eth配置信息,需要在外部对pcfg清零
static int GetEthIF(LPHY_DVR_ETH_IF pcfg)
{
	int nRet = IDVR_OK;
	
	//! 本结构体是否有效，=0表示无效
	pcfg->bValid = 1;

	//本地网络适配器信息
	/* DVR IP地址 */
	MPU_NET_GetIPAddressEx(pcfg->szIPAddress);
	/* DVR 子网掩码 */
	MPU_NET_GetSubnetMask(pcfg->szSubnetMask);
	/* 网关地址 */
	MPU_NET_GetGateway(pcfg->szGateway);
	/* 只读：服务器的物理地址 */
	MPU_NET_GetMacAddress(pcfg->szMacAddress);
	return nRet;
}
//获取Eth配置信息,需要在外部对pcfg清零
static int GetEthIF_Local(LPZW_SYS_NET_CFG pcfg)
{
	int nRet = IDVR_OK;

	//! 本结构体是否有效，=0表示无效
	pcfg->bValid = 1;

	//本地网络适配器信息
	char szNetCfg[20]="";
	struct in_addr sInaddr;
	
	/* DVR IP地址 */
	MPU_NET_GetIPAddressEx(szNetCfg);
	inet_aton(szNetCfg,&sInaddr);
	pcfg->dwIPAddress = sInaddr.s_addr;
	
	/* DVR 子网掩码 */
	MPU_NET_GetSubnetMask(szNetCfg);
	inet_aton(szNetCfg,&sInaddr);
	pcfg->dwSubnetMask = sInaddr.s_addr;

	/* 网关地址 */
	MPU_NET_GetGateway(szNetCfg);
	inet_aton(szNetCfg,&sInaddr);
	pcfg->dwGateway = sInaddr.s_addr;

	/* 只读：服务器的物理地址 */
	MPU_NET_GetMacAddress(pcfg->szMacAddress);
	
	pcfg->dwNetworkCardType = eNetworkCardType_100M;

	return nRet;
}

//获取PPPoE状态信息,需要在外部对pcfg清零
static int GetPPPoEIF(LPHY_DVR_PPPOE_IF pcfg)
{
	int nRet = IDVR_OK;

	//! 本结构体是否有效，=0表示无效
	pcfg->bValid = 1;

	if (MPU_PPPoE_GetEnable())
	{
		/*PPPoE 连接状态(只读) */
		//0表示没有连接，1表示正在连接，2表示连接成功，3表示正在挂断
		pcfg->nPPPoEState = MPU_PPPoE_IsConnected();

		if (2 == pcfg->nPPPoEState)
		{
			/*PPPoE IP地址(只读) */
			MPU_PPPoE_GetIPAddress(pcfg->szPPPoEIP);

			/*PPPoE 子网掩码(只读) */
			MPU_PPPoE_GetSubnetMask(pcfg->szPPPoESubnetMask);

			/*PPPoE 网关(只读) */
			MPU_PPPoE_GetGateway(pcfg->szPPPoEGateway);
		}
		else
		{
//          MPU_DBG("pppoe not connected, nPPPoEState = %d\n", pcfg->nPPPoEState);
		}
	}
	else
	{
		//禁用时，参数都无效，填0
//      MPU_DBG("pppoe not enable\n");
	}

	return nRet;
}

//获取PPPoE配置信息,需要在外部对pcfg清零
static int GetPPPoECfg(LPHY_DVR_PPPOE_CONF pcfg)
{
	int nRet = IDVR_OK;
	//! 本结构体是否有效，=0表示无效
	pcfg->bValid = 1;

	/* 0-不启用PPPoE,1-启用 PPPoE */
	pcfg->bPPPoEEnable = MPU_PPPoE_GetEnable();
	/* PPPoE用户名 */
	MPU_PPPoE_GetUserName(pcfg->szPPPoEUser);

	/* PPPoE密码 */
	MPU_PPPoE_GetPassword(pcfg->szPPPoEPassword);

	return nRet;
}

//获取DNS配置信息,需要在外部对pcfg清零
static int GetDnsCfg(LPHY_DVR_DNS_CONF pcfg)
{
	int nRet = IDVR_OK;

	//! 本结构体是否有效，=0表示无效
	pcfg->bValid = 1;

	/*主DNS服务器 */
	/*备用DNS */
	MPU_ETH0_GetDNS(pcfg->szPrimaryDNS, pcfg->szSecondaryDNS);

	return nRet;
}
//获取DNS配置信息,需要在外部对pcfg清零
static int GetDnsCfg_Local(LPZW_SYS_NET_CFG pcfg)
{
	int nRet = IDVR_OK;

	//! 本结构体是否有效，=0表示无效
	pcfg->bValid = 1;

#if 0
	if (MPU_NET_GetDhcpEnableEx())
		pcfg->bAutoObtainDNS = TRUE;
	else
		pcfg->bAutoObtainDNS = FALSE;
#else
		MPU_PPPoE_GetAutoObtainDNS(&pcfg->bAutoObtainDNS);
#endif

	char szDNSCfg_1[20]="";
	char szDNSCfg_2[20]="";
	struct in_addr sInaddr;
	
	/*主DNS服务器 */
	/*备用DNS */
	MPU_ETH0_GetDNS(szDNSCfg_1, szDNSCfg_2);
	inet_aton(szDNSCfg_1,&sInaddr);
	pcfg->dwPrimaryDNS =sInaddr.s_addr;
	inet_aton(szDNSCfg_2,&sInaddr);
	pcfg->dwSecondaryDNS = sInaddr.s_addr;

	return nRet;
}
//获取http配置信息,需要在外部对pcfg清零
static int GetHttpCfg(LPZW_SYS_NET_CFG pcfg)
{
	int nRet = IDVR_OK;

	//! 本结构体是否有效，=0表示无效
	pcfg->bValid = 1;

//	pcfg->bEnable = 1;			//启用或禁用http服务

	/* Http端口 */
	pcfg->dwHTTPPort = MPU_NET_GetHttpPort();

	return nRet;
}

//获取本地监听端口配置信息,需要在外部对pcfg清零
static int GetListenPortCfg(LPZW_SYS_NET_CFG pcfg)
{
	int nRet = IDVR_OK;

	//! 本结构体是否有效，=0表示无效
	pcfg->bValid = 1;

	/*本地命令端口 */
	pcfg->dwCommandPort = MPU_CIU_GetCommandPort();

	/*本地媒体端口 */
	pcfg->dwMediaPort = MPU_CIU_GetMediaPort();

	/*本地rtp端口 */
	pcfg->dwRtpPort = MPU_CIU_GetRtpPort();

	return nRet;
}

//获取浪潮手机配置信息,需要在外部对pcfg清零
static int GetTideMobileCfg(LPZW_SYS_NET_CFG pcfg)
{
	int nRet = IDVR_OK;

	//! 本结构体是否有效，=0表示无效
	pcfg->bValid = 1;

	/*手机端口 */
	pcfg->dwMobilePort = MPU_CIU_GetMobilePort();

	return nRet;
}

//获取手机配置信息,需要在外部对pcfg清零
static int GetMobileCfg(LPZW_SYS_NET_CFG pcfg)
{
	int nRet = IDVR_OK;

	//! 本结构体是否有效，=0表示无效
	pcfg->bValid = 1;

	/*浪潮手机配置 */
	GetTideMobileCfg(pcfg);

	return nRet;
}

//获取自动发现端口配置信息,需要在外部对pcfg清零
static int GetAutoDetectCfg(LPZW_SYS_NET_CFG pcfg)
{
	int nRet = IDVR_OK;

	//! 本结构体是否有效，=0表示无效
	pcfg->bValid = 1;

	/*本地命令端口 */
	pcfg->wAutoDetectPort = MPU_CIU_GetAutoDetectPort();

	return nRet;
}

static int GetHKPrivateCfg(LPZW_SYS_NET_CFG pcfg)
{
	int nRet = IDVR_OK;

	//! 本结构体是否有效，=0表示无效
	pcfg->bValid = 1;

	/* HK Private端口 */
    pcfg->HKCommandPortFieldValid = 1;
	pcfg->HKCommandPort = MPU_CIU_GetHKPrivatePort();
    //pcfg->HKCommandPort = MPU_NET_GetHKPrivatePort();

	return nRet;    
}

//樊聪2016年6月28日
static void * GetExIP(void * pAddress)
{
    PRT_TID();

    while(1)
    {
        if (b_Upnp_Enable == 0)
        {
            sleep(2);
            continue;
        }
    	//BEGINRUN_THREAD;
        DEFINE_THREAD_NAME(__FUNCTION__);

    	char sAddress[DVR_MAX_IP_STR_LEN] = "";
    	int nRet = MPU_UPNP_QUERY_EXTERNAL_IP(sAddress, DVR_MAX_IP_STR_LEN);
    	if(nRet != 0)
    	{
    		MPU_ERR("MPU_UPNP_QUERY_EXTERNAL_IP Error!iRet =%d\n",nRet);
    	}
    	memcpy(szExAddress,sAddress,DVR_MAX_IP_STR_LEN);
        sleep(2);
    }
	//ENDRUN_THREAD;
	return NULL;
}

static WORD iPortArray0[8];
static pthread_mutex_t sync_mutex_uPnP = PTHREAD_MUTEX_INITIALIZER;//2同步互斥锁

static void * GetuPnPStatus(void * param)
{
	//BEGINRUN_THREAD;
	unsigned int iStatus;
	WORD iPortMap[4];
	WORD iPortList[4];
	int i;

    DEFINE_THREAD_NAME(__FUNCTION__);
	
	for(i = 0; i < 4; i++)
	{
		iPortMap[i] = iPortArray0[i];
		iPortList[i] = iPortArray0[i+4];
	}
	int nRet = MPU_UPNP_GET_MAPSTATE(iPortMap, iPortList, &iStatus);
	if(nRet != 0)
	{
		MPU_ERR("MPU_UPNP_GET_MAPSTATE Error!nRet =%d\n",nRet);
	}
	iuPnPStatus = iStatus;
	pthread_mutex_unlock(&sync_mutex_uPnP);
	//ENDRUN_THREAD;
	return NULL;
}

static int GetuPnPCfg(LPHY_DVR_UPNP_CONF pcfg)
{
	int nRet = IDVR_OK;
	WORD iPortList[4];
	WORD iPortMap[4];
	int i;
	int iPortChange = 0;
	//! 本结构体是否有效，=0表示无效
	pcfg->bValid = 1;
	
	pcfg->nLocalHttpPort    = iPortMap[0] = MPU_NET_GetHttpPort();
	pcfg->nLocalCommandPort = iPortMap[1] = MPU_CIU_GetCommandPort();
	pcfg->nLocalMediaPort   = iPortMap[2] = MPU_CIU_GetMediaPort();
	pcfg->nLocalMobilePort  = iPortMap[3] = MPU_CIU_GetRtpPort(); //MPU_CIU_GetMobilePort();
	
	pcfg->bEnable = MPU_UPNP_GetEnable();
	
	b_Upnp_Enable = pcfg->bEnable;
	
	nRet = MPU_UPNP_QUERY_MAPLiST(iPortList);
	if(nRet != 0)
	{
		MPU_ERR("MPU_UPNP_QUERY_MAPLiST Error!iRet =%d\n",nRet);
	}
	if(0 == iPortList[0])
	{
		iPortList[0] = pcfg->nLocalHttpPort;
	}
	pcfg->nExportHttpPort = iPortList[0];
	
	if(0 == iPortList[1])
	{
		iPortList[1] = pcfg->nLocalCommandPort;
	}
	pcfg->nExportCommandPort = iPortList[1];

	if(0 == iPortList[2])
	{
		iPortList[2] = pcfg->nLocalMediaPort;
	}
	pcfg->nExportMediaPort = iPortList[2];
	
	if(0 == iPortList[3])
	{
		iPortList[3] = pcfg->nLocalMobilePort;
	}
	pcfg->nExportMobilePort = iPortList[3];

	if(iPortArray0[4] != iPortList[0] || iPortArray0[5] != iPortList[1] || iPortArray0[6] != iPortList[2] || iPortArray0[7] != iPortList[3])
	{
		iPortChange = 1;
	}
	
	for(i = 0; i < 4; i++)
	{
		iPortArray0[i] = iPortMap[i];
		iPortArray0[i+4] = iPortList[i];
	}
	if(0 == b_GetCfg)
	{	
		pthread_t thread_t;
	    int iRet = pthread_create(&thread_t, NULL, GetExIP, NULL);	
		if (iRet !=0)
	    {
	        MPU_ERR("[%s]pthread_create failed (%d)\n",__FUNCTION__, iRet);
	    }
		else
		{
			pthread_detach(thread_t);
			b_GetCfg = 1;
		}
	}
	memcpy(pcfg->szWanAddress, szExAddress, sizeof(szExAddress));
	
	if(pcfg->bEnable != 0 && 1 == iPortChange)
	{
		if (EBUSY == pthread_mutex_trylock(&sync_mutex_uPnP))
		{	
			//return EBUSY;
		}
		else
		{
			pthread_t thread_t0;
			nRet = pthread_create(&thread_t0, NULL, GetuPnPStatus, 0);	
			if (nRet !=0)
		    {
		        MPU_ERR("[%s]pthread_create   failed (%d)\n",__FUNCTION__,nRet);
		    }
			else
			{
				pthread_detach(thread_t0);
			}
		}
	}
	pcfg->nHttpPortState = iuPnPStatus&1;
	pcfg->nCommandPortState = (iuPnPStatus>>1)&1;
	pcfg->nMediaPortState = (iuPnPStatus>>2)&1;
	pcfg->nMobilePortState = (iuPnPStatus>>3)&1;
	
	//MPU_UPNP_ADD_PORTMAP(iPortMap,iPortList);
	return nRet;
}

//获取设备转发能力
static int GetDeviceRelyEnable(LPZW_SYS_NET_CFG pcfg)
{
	if (NULL == pcfg)
	{
		return -1;
	}
	
	int nRet = IDVR_OK;
	
#ifdef __MODULE_4G_SUPPORT__
    int bSupport4G = 0;
    ZWSys_Get4GSupport(&bSupport4G);
    if (bSupport4G)
    {
        pcfg->dwDeviceRelyEnable = 0;
    }
    else
    {
        pcfg->dwDeviceRelyEnable = 1;
    }
#else
    pcfg->dwDeviceRelyEnable = 1;
#endif
	
	return nRet;
}

static int SetPushAiParam(LPZW_SYS_NET_CFG pcfg)
{
	int nRet = IDVR_OK;

	//! 本结构体是否有效，=0表示无效
	if (0 == pcfg->bValid)
	{
		MPU_DBG("0 == pcfg->bValid\n");
		return IDVR_CIU_STRUCT_UNVALID;
	}

    nRet = MPU_NET_SetPushAiParam(pcfg->strShangyunPushSubKey);

    return nRet;
}

static int GetPushAiParam(LPZW_SYS_NET_CFG pcfg)
{
	int nRet = IDVR_OK;
    if (MPU_NET_IsSupportSetSubkeyByDID())
    {
    	pcfg->nIsEnablePushSubKey = 1; // 表示当前是否显示推送SubKey配置界面
        //nRet = MPU_NET_GetPushAiParam(pcfg->strShangyunPushSubKey);        
#if defined (__USE_P2P_PUSH__)
		nRet = ZWP2PPUSHVNClient_GetSubkey(pcfg->strShangyunPushSubKey, sizeof(pcfg->strShangyunPushSubKey));
#endif
    }
    else
    {
    	pcfg->nIsEnablePushSubKey = 0; // 表示当前是否显示推送SubKey配置界面
    }

    return nRet;
}

//获取WLAN 网络配置信息,需要在外部对pcfg清零
static int WLAN_GetEthIF(LPHY_DVR_ETH_IF pcfg)
{
	//MPU_DBG("...MPU_CIU_config_net.cpp....WLAN_GetEthIF()...begin...\n");
	int nRet = IDVR_OK;

#if defined (__IS_SUPPORT_WLAN__)
	if (2 == MPU_WLAN_GetWiFiModuleEnable())
	{
		pcfg->bValid = 0;
		return nRet;
	}

	//! 本结构体是否有效，=0表示无效
	pcfg->bValid = 1;

	//本地网络适配器信息

	/* DVR IP地址 */
	MPU_WLAN_GetIPAddress(pcfg->szIPAddress);
	/* DVR 子网掩码 */
	MPU_WLAN_GetSubnetMask(pcfg->szSubnetMask);

	/* 网关地址 */
	MPU_WLAN_GetGateway(pcfg->szGateway,sizeof(pcfg->szGateway));

	/* 只读：服务器的物理地址 */
	MPU_WLAN_GetMacAddress(pcfg->szMacAddress);
#endif
	return nRet;
}

//获取DHCP配置信息,需要在外部对pcfg清零
static int WLAN_GetDhcpCfg(LPHY_DVR_DHCP_CONF pcfg)
{
	int nRet = IDVR_OK;
	MPU_DBG("\n\n...MPU_CIU_config_net.cpp....WLAN_GetDhcpCfg()...begin...\n");
#if defined (__IS_SUPPORT_WLAN__)

	if (2 == MPU_WLAN_GetWiFiModuleEnable())
	{
		pcfg->bValid = 0;
		return nRet;
	}

	//! 本结构体是否有效，=0表示无效
	pcfg->bValid = 1;

	/* 0-不启用Dhcp,1-启用Dhcp */
	pcfg->bDhcpEnable = MPU_WLAN_GetDhcpEnable();
#endif
	return nRet;
}

//获取DHCP状态信息,需要在外部对pcfg清零
static int WLAN_GetDhcpStateCfg(LPHY_DVR_DHCP_STATE pcfg)
{
	int nRet = IDVR_OK;
	MPU_DBG("\n\n...MPU_CIU_config_net.cpp....WLAN_GetDhcpStateCfg()...begin...\n");
#if defined (__IS_SUPPORT_WLAN__)
	if (2 == MPU_WLAN_GetWiFiModuleEnable())
	{
		pcfg->bValid = 0;
		return nRet;
	}

	//! 本结构体是否有效，=0表示无效
	pcfg->bValid = 1;


	pcfg->nDhcpState = MPU_WLAN_GetDhcpState();
#endif
	return nRet;
}

//获取DNS配置信息,需要在外部对pcfg清零
static int WLAN_GetDnsCfg(LPHY_DVR_DNS_CONF pcfg)
{
	int nRet = IDVR_OK;
	MPU_DBG("\n\n...MPU_CIU_config_net.cpp....WLAN_GetDnsCfg()...begin...\n");
#if defined (__IS_SUPPORT_WLAN__)
	if (2 == MPU_WLAN_GetWiFiModuleEnable())
	{
		pcfg->bValid = 0;
		return nRet;
	}

	//! 本结构体是否有效，=0表示无效
	pcfg->bValid = 1;

	/* 主DNS服务器 */
	/* 备用DNS */
	MPU_WLAN_GetDNS(pcfg->szPrimaryDNS, pcfg->szSecondaryDNS, sizeof(pcfg->szSecondaryDNS));
#endif
	return nRet;
}

static int WLAN_GetConnectionCfg(LP_HY_DVR_WLAN_CONNECTION pcfg)
{
	int nRet = IDVR_OK;
	
#if defined (__IS_SUPPORT_WLAN__)
	int nAuthModeMap = 0;
	MPU_DBG("\n\n...MPU_CIU_config_net.cpp....WLAN_GetConnectionCfg()...begin...\n");

	if (2 == MPU_WLAN_GetWiFiModuleEnable())
	{
		pcfg->bValid = 0;
		return nRet;
	}

	WLAN_CONNECTION conn;

	nRet = MPU_WLAN_GetConnection(&conn);
	if (0 != nRet)
	{
		return nRet;
	}

	//! 本结构体是否有效，=0表示无效
	pcfg->bValid = 1;

    /* 
      底层处理的认证模式:
      0-OPEN,1-SHARED,2-WPAPSK,3-WPA2PSK,4-WPANONE,5-WPA,6-WPA2
      上层传入的认证模式:
      0-OPEN,1-SHARED,2-WEPAUTO,3-WPAPSK,4-WPA2PSK,5-WPANONE,6-WPA,7-WPA2,8-NONE
      出现错位，需要映射
    */
    switch(conn.nAuthMode)
    {
        case 0: nAuthModeMap=0; break;
        case 1: nAuthModeMap=1; break;
        case 2: nAuthModeMap=3; break;
        case 3: nAuthModeMap=4; break;
        case 4: nAuthModeMap=5; break;
        case 5: nAuthModeMap=6; break;
        case 6: nAuthModeMap=7; break;
        default: printf("Not Support AuthMode %d\n", conn.nAuthMode);return IDVR_CIU_PARAM_OVERFLOW;
    }

	pcfg->bConnect = conn.bConnect;
	pcfg->nNetworkType = conn.nNetworkType;
	pcfg->nAuthMode = nAuthModeMap;
	pcfg->nEncrypType = conn.nEncrypType;
	pcfg->struKeyCfg.bValid = conn.struKeyCfg.bValid;
	pcfg->struKeyCfg.nDefKeyID = conn.struKeyCfg.nDefKeyID;
	pcfg->struKeyCfg.Key1.bValid = conn.struKeyCfg.Key1.bValid;
	strncpy(pcfg->struKeyCfg.Key1.szKey, conn.struKeyCfg.Key1.szKey, DVR_MAX_WIFI_KEY_LEN - 1);
	pcfg->struKeyCfg.Key2.bValid = conn.struKeyCfg.Key2.bValid;
	strncpy(pcfg->struKeyCfg.Key2.szKey, conn.struKeyCfg.Key2.szKey, DVR_MAX_WIFI_KEY_LEN - 1);
	pcfg->struKeyCfg.Key3.bValid = conn.struKeyCfg.Key3.bValid;
	strncpy(pcfg->struKeyCfg.Key3.szKey, conn.struKeyCfg.Key3.szKey, DVR_MAX_WIFI_KEY_LEN - 1);
	pcfg->struKeyCfg.Key4.bValid = conn.struKeyCfg.Key4.bValid;
	strncpy(pcfg->struKeyCfg.Key4.szKey, conn.struKeyCfg.Key4.szKey, DVR_MAX_WIFI_KEY_LEN - 1);
	pcfg->struKeyCfg.WPAKey.bValid = conn.struKeyCfg.WPAKey.bValid;
    pcfg->struKeyCfg.WPAKey.nKeyType = 1; // 字符串形式(固定)
	strncpy(pcfg->struKeyCfg.WPAKey.szKey, conn.struKeyCfg.WPAKey.szKey, DVR_MAX_WIFI_KEY_LEN - 1);
	strncpy(pcfg->szMacAddress, conn.szMacAddress, DVR_MAX_MAC_STR_LEN - 1);
	strncpy(pcfg->szSSID, conn.szSSID, DVR_MAX_SSID_LEN - 1);
#endif
	return nRet;
}

#if defined (__IS_SUPPORT_WLAN__)
static int WLAN_GetConnectionCfgV2(LP_HY_DVR_WLAN_CONNECTION_V2 pcfg)
{
	HY_DVR_WLAN_CONNECTION pcfg_old;
	int nRet = IDVR_OK;

	memset(&pcfg_old,0,sizeof(HY_DVR_WLAN_CONNECTION));

	nRet = WLAN_GetConnectionCfg(&pcfg_old);

	pcfg->bValid = pcfg_old.bValid;
	pcfg->bConnect = pcfg_old.bConnect;
	memcpy(&(pcfg->szMacAddress[0]),&(pcfg_old.szMacAddress[0]),DVR_MAX_MAC_STR_LEN);
	pcfg->nNetworkType = pcfg_old.nNetworkType;
	pcfg->nAuthMode = pcfg_old.nAuthMode;
	pcfg->nEncrypType = pcfg_old.nEncrypType;
	memcpy(&(pcfg->struKeyCfg),&(pcfg_old.struKeyCfg),sizeof(HY_DVR_WLAN_KEY_CFG));
	base64Encode((const unsigned char *)&(pcfg_old.szSSID[0]),&(pcfg->szSSID[0]),strlen(&(pcfg_old.szSSID[0])));	

	return nRet;
}
#endif

static int WLAN_GetAccessPointsCfg(LP_HY_DVR_WLAN_ACCESSPOINT_CFG pcfg)
{
	int nRet = IDVR_OK;	
	
	MPU_DBG("\n\n...MPU_CIU_config_net.cpp....WLAN_GetAccessPointsCfg()...begin...\n");
#if defined (__IS_SUPPORT_WLAN__)
	if (2 == MPU_WLAN_GetWiFiModuleEnable())
	{
		pcfg->bValid = 0;
		return nRet;
	}

	//! 本结构体是否有效，=0表示无效
	pcfg->bValid = 1;

	WLAN_ACCESSPOINT AccessPoints[WLAN_MAX_AP_NUM];

    int nAuthModeMap = 0;
	int i = 0;
	int count = 0;

	MPU_WLAN_GetAccessPoints(AccessPoints, &count);

    count = count > DVR_MAX_AP_NUM ? DVR_MAX_AP_NUM : count;


	for (i = 0; i < count; i++)
	{
        /* 
          底层处理的认证模式:
          0-OPEN,1-SHARED,2-WPAPSK,3-WPA2PSK,4-WPANONE,5-WPA,6-WPA2
          上层传入的认证模式:
          0-OPEN,1-SHARED,2-WEPAUTO,3-WPAPSK,4-WPA2PSK,5-WPANONE,6-WPA,7-WPA2,8-NONE
          出现错位，需要映射
        */
        switch(AccessPoints[i].nAuthentication)
        {
            case 0: nAuthModeMap=0; break;
            case 1: nAuthModeMap=1; break;
            case 2: nAuthModeMap=3; break;
            case 3: nAuthModeMap=4; break;
            case 4: nAuthModeMap=5; break;
            case 5: nAuthModeMap=6; break;
            case 6: nAuthModeMap=7; break;
            default: printf("Not Support AuthMode %d\n", AccessPoints[i].nAuthentication);return IDVR_CIU_PARAM_OVERFLOW;
        }

		pcfg->struAccessPoints[i].bValid = 1;
		pcfg->struAccessPoints[i].nAuthentication = nAuthModeMap;
		pcfg->struAccessPoints[i].nEncrypType = AccessPoints[i].nEncrypType;
		pcfg->struAccessPoints[i].nSignal = AccessPoints[i].nQuality;
		pcfg->struAccessPoints[i].nWirelessMode = AccessPoints[i].nWirelessMode;
		strncpy(pcfg->struAccessPoints[i].szMacAddress, AccessPoints[i].szMacAddress, DVR_MAX_MAC_STR_LEN - 1);
		strncpy(pcfg->struAccessPoints[i].szSSID, AccessPoints[i].szSSID, DVR_MAX_SSID_LEN - 1);
	}
#endif
	return nRet;
}
#if defined (__IS_SUPPORT_WLAN__)
static int WLAN_GetAccessPointsCfgV2(LP_HY_DVR_WLAN_ACCESSPOINT_CFG_V2 pcfg)
{
	int nRet = IDVR_OK;	
  	
	MPU_DBG("\n\n...MPU_CIU_config_net.cpp....WLAN_GetAccessPointsCfg()...begin...\n");

	if (2 == MPU_WLAN_GetWiFiModuleEnable())
	{
		pcfg->bValid = 0;
		return nRet;
	}

	//! 本结构体是否有效，=0表示无效
	pcfg->bValid = 1;

	WLAN_ACCESSPOINT AccessPoints[WLAN_MAX_AP_NUM];

    int nAuthModeMap = 0;
	int i = 0;
	int count = 0;

	MPU_WLAN_GetAccessPoints(AccessPoints, &count);

    // count = count > DVR_MAX_AP_NUM_V2 ? DVR_MAX_AP_NUM_V2 : count;
    count = count > 20 ? 20 : count; // 根据需求，限制为20个(和海芯一样)
	
	for (i = 0; i < count; i++)
	{
        /* 
          底层处理的认证模式:
          0-OPEN,1-SHARED,2-WPAPSK,3-WPA2PSK,4-WPANONE,5-WPA,6-WPA2
          上层传入的认证模式:
          0-OPEN,1-SHARED,2-WEPAUTO,3-WPAPSK,4-WPA2PSK,5-WPANONE,6-WPA,7-WPA2,8-NONE
          出现错位，需要映射
        */
        switch(AccessPoints[i].nAuthentication)
        {
            case 0: nAuthModeMap=0; break;
            case 1: nAuthModeMap=1; break;
            case 2: nAuthModeMap=3; break;
            case 3: nAuthModeMap=4; break;
            case 4: nAuthModeMap=5; break;
            case 5: nAuthModeMap=6; break;
            case 6: nAuthModeMap=7; break;
            default: printf("Not Support AuthMode %d\n", AccessPoints[i].nAuthentication);return IDVR_CIU_PARAM_OVERFLOW;
        }
		pcfg->struAccessPoints[i].bValid = 1;
		pcfg->struAccessPoints[i].nAuthentication = nAuthModeMap;
		pcfg->struAccessPoints[i].nEncrypType = AccessPoints[i].nEncrypType;
		pcfg->struAccessPoints[i].nSignal = AccessPoints[i].nQuality;
		pcfg->struAccessPoints[i].nWirelessMode = AccessPoints[i].nWirelessMode;
		strncpy(pcfg->struAccessPoints[i].szMacAddress, AccessPoints[i].szMacAddress, DVR_MAX_MAC_STR_LEN - 1);
		strncpy(pcfg->struAccessPoints[i].szSSID, AccessPoints[i].szSSID, DVR_MAX_SSID_LEN - 1);
	}

	
	return nRet;
}

static int WLAN_GetAccessPointsCfgV2_1(LP_HY_DVR_WLAN_ACCESSPOINT_CFG_SSID_INFO_V2 pcfg)
{
	int nRet = IDVR_OK;	
	
	MPU_DBG("\n\n...MPU_CIU_config_net.cpp....WLAN_GetAccessPointsCfg()...begin...\n");

	if (2 == MPU_WLAN_GetWiFiModuleEnable())
	{
		pcfg->bValid = 0;
		return nRet;
	}

	//! 本结构体是否有效，=0表示无效
	pcfg->bValid = 1;

	WLAN_ACCESSPOINT AccessPoints[WLAN_MAX_AP_NUM];

    int nAuthModeMap = 0;
	int i = 0;
	int count = 0;

	MPU_WLAN_GetAccessPoints(AccessPoints, &count);

    // count = count > DVR_MAX_AP_NUM_V2 ? DVR_MAX_AP_NUM_V2 : count;
    count = count > 20 ? 20 : count; // 根据需求，限制为20个(和海芯一样)
	
	for (i = 0; i < count; i++)
	{
        /* 
          底层处理的认证模式:
          0-OPEN,1-SHARED,2-WPAPSK,3-WPA2PSK,4-WPANONE,5-WPA,6-WPA2
          上层传入的认证模式:
          0-OPEN,1-SHARED,2-WEPAUTO,3-WPAPSK,4-WPA2PSK,5-WPANONE,6-WPA,7-WPA2,8-NONE
          出现错位，需要映射
        */
        switch(AccessPoints[i].nAuthentication)
        {
            case 0: nAuthModeMap=0; break;
            case 1: nAuthModeMap=1; break;
            case 2: nAuthModeMap=3; break;
            case 3: nAuthModeMap=4; break;
            case 4: nAuthModeMap=5; break;
            case 5: nAuthModeMap=6; break;
            case 6: nAuthModeMap=7; break;
            default: printf("Not Support AuthMode %d\n", AccessPoints[i].nAuthentication);return IDVR_CIU_PARAM_OVERFLOW;
        }
		pcfg->struAccessPoints[i].bValid = 1;
		pcfg->struAccessPoints[i].nAuthentication = nAuthModeMap;
		pcfg->struAccessPoints[i].nEncrypType = AccessPoints[i].nEncrypType;
		pcfg->struAccessPoints[i].nSignal = AccessPoints[i].nQuality;
		pcfg->struAccessPoints[i].nWirelessMode = AccessPoints[i].nWirelessMode;
		strncpy(pcfg->struAccessPoints[i].szMacAddress, AccessPoints[i].szMacAddress, DVR_MAX_MAC_STR_LEN - 1);
		strncpy(pcfg->struAccessPoints[i].szSSID, AccessPoints[i].szSSID, DVR_MAX_SSID_LEN - 1);
	}
	
	return nRet;
}

static int WLAN_GetAccessConnectInfoEx(LPHY_DVR_NETCFG_WLAN_CONNECTION_INFO_EX pcfg)
{
    int nRet = IDVR_OK;	

    MPU_DBG("\n\n...MPU_CIU_config_net.cpp....WLAN_GetAccessPointsCfg()...begin...\n");

    if (2 == MPU_WLAN_GetWiFiModuleEnable())
    {
        pcfg->iValid = 0;
        return nRet;
    }

    //! 本结构体是否有效，=0表示无效
    pcfg->iValid = 1;

    WLAN_ACCESSSIGNALINFO SignalInfo = {0};

    MPU_WLAN_GetAccessSignalInfo(&SignalInfo);
    
    pcfg->iWifiSignalStrength = SignalInfo.nQuality;
    strncpy(pcfg->acWifiName, SignalInfo.szSSID, DVR_MAX_SSID_LEN - 1);
   
    return nRet;
}


#endif

//设置Eth配置信息
static int WLAN_SetEthIF(LPHY_DVR_ETH_IF pcfg)
{
	int nRet = IDVR_OK;
#if defined (__IS_SUPPORT_WLAN__)
	MPU_DBG("\n\n...MPU_CIU_config_net.cpp....WLAN_SetEthIF()...begin...\n");
	//! 本结构体是否有效，=0表示无效
	if (0 == pcfg->bValid)
	{
		MPU_DBG("0 == pcfg->bValid\n");
		return IDVR_CIU_STRUCT_UNVALID;
	}

	if (2 == MPU_WLAN_GetWiFiModuleEnable())
	{
		return nRet;
	}

	char szOldAddress[DVR_MAX_IP_STR_LEN] = {0};
	char szOldNetmask[DVR_MAX_MAC_STR_LEN] = {0};
	char szOldGateway[DVR_MAX_IP_STR_LEN] = {0};
	
	//本地网络适配器信息
	MPU_WLAN_GetIPAddress(szOldAddress);
	/* 子网掩码 */
    MPU_WLAN_GetSubnetMask(szOldNetmask);

	/* 网关地址 */
    MPU_WLAN_GetGateway(szOldGateway,sizeof(szOldGateway));

    /* 设置参数之前，需要先判断参数是否改变 */
	if ((0==strncmp(szOldAddress, pcfg->szIPAddress,  DVR_MAX_IP_STR_LEN))
	 && (0==strncmp(szOldNetmask, pcfg->szSubnetMask, DVR_MAX_MAC_STR_LEN))
	 && (0==strncmp(szOldGateway, pcfg->szGateway,    DVR_MAX_IP_STR_LEN)))
	{
	    printf("wlan0 network not change!\n");
	    return 0;
    }	


	/* DVR IP地址 */
	MPU_WLAN_SetIPAddress(pcfg->szIPAddress);
	
	/* DVR 子网掩码 */
	MPU_WLAN_SetSubnetMask(pcfg->szSubnetMask);

	/* 网关地址 */
	MPU_WLAN_SetGateway(pcfg->szGateway);
#endif
	return nRet;
}

static int GetIfNeedAdjustIpAddress(LPHY_DVR_WHOPROBE_CONF pcfg, int *same)
{
	int nRet = IDVR_OK;
	
	//MPU_INFO("SetIpByProbe().bValid:%d..ip:%s \n", pcfg->bValid, pcfg->szIPAddress);
	//! 本结构体是否有效，=0表示无效
	if (0 == pcfg->bValid)
	{
		MPU_DBG("0 == pcfg->bValid\n");
		return IDVR_CIU_STRUCT_UNVALID;
	}

	nRet = MPU_NET_IPAddressIfSameWithNvr(pcfg->szIPAddress, same);

	return nRet;
}

static int SetIpByProbe(LPHY_DVR_WHOPROBE_CONF pcfg)
{
	int nRet = IDVR_OK;
	
	MPU_INFO("SetIpByProbe().bValid:%d..ip:%s \n", pcfg->bValid, pcfg->szIPAddress);
	//! 本结构体是否有效，=0表示无效
	if (0 == pcfg->bValid)
	{
		MPU_DBG("0 == pcfg->bValid\n");
		return IDVR_CIU_STRUCT_UNVALID;
	}

	MPU_NET_SetIPByProbe(pcfg->szIPAddress);

	return nRet;
}

// 获取FTP相关参数
static int GetFtpUserParam(LPHY_DVR_FTP_CONF_V2 pcfg)
{
	int nRet = IDVR_OK;

#if defined (__USE_FTP__)
	
    FTP_user_param ftpusrparam;

    if (NULL == pcfg)
    {
		MPU_DBG("NULL == pcfg\n");
		return IDVR_CIU_STRUCT_UNVALID;
    }

	//! 本结构体是否有效，=0表示无效
	pcfg->bValid = 1;

    //! 获取ftp主机连接状态 
    pcfg->iFtpState = MPU_NET_GetFtpParam();
    if (pcfg->iFtpState == 1) 
    {
        pcfg->iFtpState = 2;
    }else if (pcfg->iFtpState == 0){
        pcfg->iFtpState = 0;
    }else{
        MPU_DBG("status = %d out of [0,1] not support!\n");        
        pcfg->iFtpState = 0;
    }

	memset(&ftpusrparam, 0, sizeof(ftpusrparam));
	
	MPU_NET_GetFtpParam(&ftpusrparam);
	
	pcfg->bEnable  = ftpusrparam.ftpenable;
	pcfg->wFtpPort = ftpusrparam.port;
	pcfg->nFtpMode = ftpusrparam.nFtpMode;
	strncpy(pcfg->szIPAddress, ftpusrparam.host, sizeof(pcfg->szIPAddress) - 1);
	strncpy(pcfg->szUserName, ftpusrparam.user, sizeof(pcfg->szUserName) - 1);
	strncpy(pcfg->szUserPassword, ftpusrparam.pass, sizeof(pcfg->szUserPassword) - 1);
	strncpy(pcfg->cFtpRemoteServerStoragePath, ftpusrparam.rmPath, sizeof(pcfg->cFtpRemoteServerStoragePath) - 1);

    switch(ftpusrparam.transt_type)
    {
        case SEND_NONE: pcfg->nTransferContentTypeMask = 0x0; break;
        case SEND_PICTURE: pcfg->nTransferContentTypeMask = 0x1; break;
        case SEND_RECORDS: pcfg->nTransferContentTypeMask = 0x2; break;
        case SEND_ALL: pcfg->nTransferContentTypeMask = 0x3; break;
        default:  MPU_DBG("invail TypeMask=%d\n", ftpusrparam.transt_type); pcfg->nTransferContentTypeMask = 0x0; break;
    }

#endif

	return nRet;
}

// 设置FTP相关参数
static int SetFtpUserParam(LPHY_DVR_FTP_CONF_V2 pcfg)
{
	int nRet = IDVR_OK;
	
#if defined (__USE_FTP__)
	
    FTP_user_param ftpusrparam;
    
    if (NULL == pcfg)
    {
		MPU_DBG("NULL == pcfg\n");
		return IDVR_CIU_STRUCT_UNVALID;
    }

	//! 本结构体是否有效，=0表示无效
	if (0 == pcfg->bValid)
	{
		MPU_DBG("0 == pcfg->bValid\n");
		return IDVR_CIU_STRUCT_UNVALID;
	}

	memset(&ftpusrparam, 0, sizeof(ftpusrparam));
	
	ftpusrparam.ftpenable = pcfg->bEnable;
	ftpusrparam.port = pcfg->wFtpPort;
	ftpusrparam.nFtpMode = pcfg->nFtpMode;	//设置ftp模式
	strncpy(ftpusrparam.host, pcfg->szIPAddress, sizeof(pcfg->szIPAddress) - 1);
	strncpy(ftpusrparam.user, pcfg->szUserName, sizeof(pcfg->szUserName) - 1);
	strncpy(ftpusrparam.pass, pcfg->szUserPassword, sizeof(pcfg->szUserPassword) - 1);
	if((strlen(pcfg->cFtpRemoteServerStoragePath)+1)<=200)
	{
		strncpy(ftpusrparam.rmPath, pcfg->cFtpRemoteServerStoragePath, sizeof(ftpusrparam.rmPath) - 1);//设置远程路径
	}
	else
	{
		printf("pcfg->cFtpRemoteServerStoragePath is too long\n");
	}
    
    switch(pcfg->nTransferContentTypeMask)
    {
        case 0x0: ftpusrparam.transt_type = SEND_NONE; break;
        case 0x1: ftpusrparam.transt_type = SEND_PICTURE; break;
        case 0x2: ftpusrparam.transt_type = SEND_RECORDS; break;
        case 0x3: ftpusrparam.transt_type = SEND_ALL; break;
        default:  MPU_DBG("invail TypeMask=%d\n", pcfg->nTransferContentTypeMask); ftpusrparam.transt_type = SEND_NONE; break;
    }
	MPU_NET_SetFtpParam(&ftpusrparam);
#endif

    return nRet;
}

/* 获取当前用的网卡类型 */
static int GetCurrNetCardType(LPHY_DVR_WHICH_NETCARD_USED pcfg)
{
    int nRet = IDVR_OK;

    int nCurrNetCardType = 0;
     
    if (NULL == pcfg)
    {
        MPU_DBG("NULL == pcfg\n");
        return IDVR_CIU_STRUCT_UNVALID;
    }

    //! 本结构体是否有效，=0表示无效
    pcfg->bValid = 1;

	MPU_NET_GetCurrNetCardType(&nCurrNetCardType);

#ifdef __MODULE_4G_SUPPORT__
		if (2 == nCurrNetCardType)
		{
			//改为4G类型网卡
			nCurrNetCardType = 5;
		}
#endif 

    pcfg->dwCardIndex = nCurrNetCardType;
#if defined (__IS_SUPPORT_WLAN__)
	//获取当前wifi模式(0:none 1:sta 2:ap)
	int nCurrWifiMode = 0;
	MPU_NET_GetCurrWifiMode(&nCurrWifiMode);
	pcfg->dwAPMode = nCurrWifiMode;
#endif
    return nRet;
}

#ifdef __MODULE_4G_SUPPORT__

/* 获取当前SIM卡的只读信息 */
static int GetCurrSimCardInfo(HY_DVR_4G_NETCARD_INFO *pcfg)
{
	int nRet = IDVR_OK;
    int ucWorkMode = 0;

    pcfg->bValid = 1;

    //获取卡模式
    MPU_Net_GetUcWorkMode(&ucWorkMode);

    nRet = MPU_NET_GetSimCardInfo(pcfg->szIMSI, 
                                    pcfg->szIMEI, 
                                    pcfg->szACT, 
                                    pcfg->szOperator, 
                                    pcfg->szBand,
                                    pcfg->szICCID,
                                    pcfg->ac4GModuleVersion);
    if (-1 == nRet)
    {    
		MPU_ERR("Fail to called MPU_NET_GetSimCardInfo\n");
		return nRet;
    }

    pcfg->iCardWorkMode = ucWorkMode;

    nRet = MPU_Net_Get4GModuleInfo(pcfg->ac4GModuleVersion,&(pcfg->i4GSignalStrength));
    if (-1 == nRet)
    {    
		MPU_ERR("Fail to called MPU_Net_Get4GModuleInfo\n");
		return nRet;
    }   

	return nRet;
}


static int SetCurrSimCardInfo(HY_DVR_4G_NETCARD_INFO *pcfg)
{
	int nRet = IDVR_OK;

	//! 本结构体是否有效，=0表示无效
	if (0 == pcfg->bValid)
	{
		MPU_DBG("0 == pcfg->bValid\n");
		return IDVR_CIU_STRUCT_UNVALID;
	}

	MPU_NET_SetSimCardType(&pcfg->bIsExternalCard);

	MPU_DBG("SetCurrSimCardInfo isExternalCard = %d\n",pcfg->bIsExternalCard);
	
	return nRet;
}

/* 获取当前SIM卡的可配置信息 */
static int GetCurrSimCardCfg(HY_DVR_4G_NETCARD_CONFIG *pcfg)
{
	int nRet = IDVR_OK;

    pcfg->bValid = 1;

	pcfg->nAPNMode = 0;
	strncpy(pcfg->szAPNServer, "UNKNOWN",sizeof(pcfg->szAPNServer)-1);
	strncpy(pcfg->szAPNUserName, "UNKNOWN",sizeof(pcfg->szAPNUserName)-1);
	strncpy(pcfg->szAPNPassword, "UNKNOWN",sizeof(pcfg->szAPNPassword)-1);
	//pcfg->nRegisterStatus = 0;
	pcfg->nEnableModWander = 0;
	strncpy(pcfg->strIPAddress, "192.168.100.100",sizeof(pcfg->strIPAddress));
	strncpy(pcfg->strGateway, "192.168.100.1",sizeof(pcfg->strGateway)-1);

	nRet = MPU_NET_GPRS_GetSimCardStatus(&pcfg->nRegisterStatus);
	if (-1 == nRet)
	{
	    MPU_ERR("Fail to called MPU_NET_GPRS_GetSimCardStatus\n");
        return nRet;
	}

    nRet = MPU_Net_GetGPRSModuleSignalQuality(&pcfg->nSignalVal);
    if (-1 == nRet)
    {    
		MPU_ERR("Fail to called MPU_Net_GetGPRSModuleSignalQuality\n");
        return nRet;
    }

	return nRet;
}

/* 设置当前SIM卡的可配置信息 */
static int SetCurrSimCardCfg(HY_DVR_4G_NETCARD_CONFIG *pcfg)
{
	int nRet = IDVR_OK;
    
	//! 本结构体是否有效，=0表示无效
	if (0 == pcfg->bValid)
	{
		MPU_DBG("0 == pcfg->bValid\n");
		return IDVR_CIU_STRUCT_UNVALID;
	}

	return nRet;
}

//设置卡选择配置 
static int Set4GCardSelectCfg(HY_DVR_4G_CARD_SELECT_CFG_INFO *pcfg)
{
	int nRet = 0;
	
	//本结构体是否有效，=0表示无效
	if (0 == pcfg->nValid)
	{
		MPU_DBG("0 == pcfg->bValid\n");
		return IDVR_CIU_STRUCT_UNVALID;
	}
	
	nRet = MPU_Net_Set4GControlConfig(pcfg);
	return nRet;
}

//获取选择配置 
static int Get4GCardSelectCfg(HY_DVR_4G_CARD_SELECT_CFG_INFO *pcfg)
{
	int nRet = 0;
	
	nRet = MPU_Net_Get4GControlConfig(pcfg);

	return nRet;
}

static int Get4GCardSelectResult(HY_DVR_4G_CARD_SELECT_RESULT_INFO *pcfg)
{
	int nRet = 0;
	
	nRet = MPU_Net_Get4GSwitchResult(pcfg);

	return nRet;
}


//获取4G卡信息 
static int Get4GCardInfo(HY_DVR_4G_CARD_CFG_LIST *pcfg)
{
	int nRet = 0;
	int i = 0;

	pcfg->nValid = 1;

    char    sOper[64] = {0};
    char    sImei[16] = {0};
    char    sIccid[32] = {0};
    char sz4GModuleVersion[128] = {0};
    int nIsInUse;
    int nSignalStrength;

    for (i = 0; i < 2; i++)
    {
        if (i == 0)
        {
            pcfg->st4GCardCfg[i].nCardType = 1; //外置卡
        }

        nRet = MPU_Net_GetSimIndexCardInfo(i, &nIsInUse, sOper, &nSignalStrength, sIccid, sImei, sz4GModuleVersion);
        if (nRet == 0)
        {
            if (strlen(sIccid) != 0)
            {
                pcfg->st4GCardCfg[i].nCardStatus = 1;
            }

            pcfg->st4GCardCfg[i].nValid = 1;
            memcpy(pcfg->st4GCardCfg[i].cICCID, sIccid, sizeof(sIccid));
            memcpy(pcfg->st4GCardCfg[i].szOperator, sOper, sizeof(sOper));	
			pcfg->st4GCardCfg[i].isDirect = MPU_Net_Module4G_GetCardBindMode();
        }

        MPU_DBG("i[%d] nValid[%d] nCardType[%d] cICCID[%s] nCardStatus[%d] szOperator[%s]\n",     \
                i, pcfg->st4GCardCfg[i].nValid, pcfg->st4GCardCfg[i].nCardType, pcfg->st4GCardCfg[i].cICCID,    \
                pcfg->st4GCardCfg[i].nCardStatus, pcfg->st4GCardCfg[i].szOperator);
    }

	return nRet;
}


//获取双sim卡信息 
static int GetSimIndexCardInfo(HY_DVR_SIM_CARD_INFO_LIST *pcfg)
{
    int nRet = 0;
    int i = 0;
    int ucWorkMode = 0;
    int iIsCloudCardDev = 0;
    int iSimNum = 0;

    pcfg->bValid = 1;

	//获取卡模式
    MPU_Net_GetUcWorkMode(&ucWorkMode);

    //获取是否是云卡设备
    iIsCloudCardDev = MPU_Net_GetIsCloudCardDev();

    //获取SIM卡数量
    iSimNum = Get4GRealSimCardCount();

    for (i = 0; i < 2; i++)
    {
        if (i >= 1)
        {
            if (iIsCloudCardDev || iSimNum < 2)
            {
                pcfg->stSIMCardInfo[i].bValid = 0;
				printf("iIsCloudCardDev = %d iSimNum = %d\n",iIsCloudCardDev,iSimNum);
                continue;
            }
        }

        pcfg->stSIMCardInfo[i].bValid = 1;

        if (i == 0)
        {
            pcfg->stSIMCardInfo[i].nCardType = 1; //外置卡
        }

        nRet = MPU_Net_GetSimIndexCardInfo(i, &pcfg->stSIMCardInfo[i].nIsInUse, pcfg->stSIMCardInfo[i].szOperator,      \
                                            &pcfg->stSIMCardInfo[i].nSignalStrength, pcfg->stSIMCardInfo[i].szICCID,    \
                                            pcfg->stSIMCardInfo[i].szIMEI, pcfg->stSIMCardInfo[i].sz4GModuleVersion);

		pcfg->stSIMCardInfo[i].iCardWorkMode = ucWorkMode;
		pcfg->stSIMCardInfo[i].isDirect = MPU_Net_Module4G_GetCardBindMode();
		MPU_DBG("pcfg->bValid = %d\n",pcfg->bValid);

		MPU_DBG("i:%d bValid:%d nCardType:%d nIsInUse:%d szOperator:%s nSignalStrength:%d szICCID:%d szIMEI:%s sz4GModuleVersion:%s\n",
			i,pcfg->stSIMCardInfo[i].bValid,pcfg->stSIMCardInfo[i].nCardType,pcfg->stSIMCardInfo[i].nIsInUse,pcfg->stSIMCardInfo[i].szOperator,
			pcfg->stSIMCardInfo[i].nSignalStrength,pcfg->stSIMCardInfo[i].szICCID,
			pcfg->stSIMCardInfo[i].szIMEI,pcfg->stSIMCardInfo[i].sz4GModuleVersion);

    }

    return nRet;
}


static int GetFlAppid(LPHY_DVR_FL_APPID pcfg)
{
	int nRet = IDVR_OK;
	//! 本结构体是否有效，=0表示无效
	pcfg->nValid = 1;


	MPU_Net_GPRS_FL_Appid(pcfg->strAppid);

	return nRet;
}


static int GetFlAppkey(LPHY_DVR_FL_APPKEY pcfg)
{
	int nRet = IDVR_OK;
	//! 本结构体是否有效，=0表示无效
	pcfg->nValid = 1;

	MPU_Net_GPRS_FL_Appkey(pcfg->strAppkey);

	return nRet;
}

//获取北纬cardtype
static int GetNorthLatiCardType(LPHY_DVR_FL_BW_CARDTYPE pcfg)
{
	int nRet = IDVR_OK;
	//! 本结构体是否有效，=0表示无效
	pcfg->nValid = 1;

	MPU_NET_GPRS_GetNorthLatiCardType(pcfg->strCardtype,sizeof(pcfg->strCardtype));

	return nRet;
}



//获取断开与转发服务器连接判定条件，包括是否是金睿卡，允许连接时长。
static int GetFlNetworkInfo(LPHY_DVR_FL_NETWORK_INFO pcfg)
{
	int nRet = IDVR_OK;
	//! 本结构体是否有效，=0表示无效
	pcfg->nValid = 1;

	MPU_Net_GPRS_FL_SimCardType(&pcfg->nFlMode);

	MPU_Net_GPRS_FL_AllowTime(&pcfg->nConnectInterVal);

	return nRet;
}

#endif



//设置DHCP配置信息
static int WLAN_SetDhcpCfg(const LPHY_DVR_DHCP_CONF pcfg)
{
	int nRet = IDVR_OK;
	MPU_DBG("\n\n...MPU_CIU_config_net.cpp....WLAN_SetDhcpCfg()...begin...\n");
#if defined (__IS_SUPPORT_WLAN__)
	//! 本结构体是否有效，=0表示无效
	if (0 == pcfg->bValid)
	{
		MPU_DBG("0 == pcfg->bValid\n");
		return IDVR_CIU_STRUCT_UNVALID;
	}

	if (2 == MPU_WLAN_GetWiFiModuleEnable())
	{
		return nRet;
	}

	if (MPU_WLAN_GetDhcpEnable() != pcfg->bDhcpEnable)
	{
		/* 0-不启用Dhcp,1-启用Dhcp */
		MPU_WLAN_SetDhcpEnable(pcfg->bDhcpEnable);		
        bNeedUpdateRouter = TRUE;
	}
#endif
	return nRet;
}

static int WLAN_SetConnectionCfg(const LP_HY_DVR_WLAN_CONNECTION pcfg)
{
	int nRet = IDVR_OK;
	//! 本结构体是否有效，=0表示无效
	if (NULL == pcfg ||  0 == pcfg->bValid)
	{
		MPU_DBG("0 == pcfg->bValid\n");
		return IDVR_CIU_STRUCT_UNVALID;
	}
#if defined (__IS_SUPPORT_WLAN__)
	int nAuthModeMap = 0;
	if (2 == MPU_WLAN_GetWiFiModuleEnable())
	{
		return nRet;
	}

	WLAN_CONNECTION conn;

    /* 上层传入的认证模式:
       0-OPEN,1-SHARED,2-WEPAUTO,3-WPAPSK,4-WPA2PSK,5-WPANONE,6-WPA,7-WPA2,8-NONE
       底层处理的认证模式:
       0-OPEN,1-SHARED,2-WPAPSK,3-WPA2PSK,4-WPANONE,5-WPA,6-WPA2
       出现错位，需要映射
    */
    switch(pcfg->nAuthMode)
    {
        case 0: nAuthModeMap=0; break;
        case 1: nAuthModeMap=1; break;
        case 2: nAuthModeMap=3; break;
        case 3: nAuthModeMap=2; break;
        case 4: nAuthModeMap=3; break;
        case 5: nAuthModeMap=4; break;
        case 6: nAuthModeMap=5; break;
        case 7: nAuthModeMap=6; break;
        case 8: nAuthModeMap=4; break;
        default: printf("Not Support AuthMode %d\n", pcfg->nAuthMode);return IDVR_CIU_PARAM_OVERFLOW;
    }

	conn.nNetworkType = pcfg->nNetworkType;
	conn.nEncrypType = pcfg->nEncrypType;
	conn.nAuthMode = nAuthModeMap;
	conn.struKeyCfg.bValid = pcfg->struKeyCfg.bValid;
	conn.struKeyCfg.nDefKeyID = pcfg->struKeyCfg.nDefKeyID;
	conn.struKeyCfg.Key1.bValid = pcfg->struKeyCfg.Key1.bValid;
	strncpy(conn.struKeyCfg.Key1.szKey, pcfg->struKeyCfg.Key1.szKey, WLAN_MAX_KEY_LEN - 1);
	conn.struKeyCfg.Key2.bValid = pcfg->struKeyCfg.Key2.bValid;
	strncpy(conn.struKeyCfg.Key2.szKey, pcfg->struKeyCfg.Key2.szKey, WLAN_MAX_KEY_LEN - 1);
	conn.struKeyCfg.Key3.bValid = pcfg->struKeyCfg.Key3.bValid;
	strncpy(conn.struKeyCfg.Key3.szKey, pcfg->struKeyCfg.Key3.szKey, WLAN_MAX_KEY_LEN - 1);
	conn.struKeyCfg.Key4.bValid = pcfg->struKeyCfg.Key4.bValid;
	strncpy(conn.struKeyCfg.Key4.szKey, pcfg->struKeyCfg.Key4.szKey, WLAN_MAX_KEY_LEN - 1);
	conn.struKeyCfg.WPAKey.bValid = pcfg->struKeyCfg.WPAKey.bValid;
	strncpy(conn.struKeyCfg.WPAKey.szKey, pcfg->struKeyCfg.WPAKey.szKey, WLAN_MAX_KEY_LEN - 1);
	strncpy(conn.szMacAddress, pcfg->szMacAddress, WLAN_MAC_ADDRESS_LEN - 1);
	strncpy(conn.szSSID, pcfg->szSSID, WLAN_MAX_SSID_LEN - 1);

	nRet = MPU_WLAN_SetConnection(&conn);
#endif
	return nRet;
}

#if defined (__IS_SUPPORT_WLAN__)
static int WLAN_SetConnectionCfgEx(LP_HY_DVR_WLAN_CONNECTION pcfg)
{
	int nRet = IDVR_OK;

    int nAuthModeMap = 0;

	//! 本结构体是否有效，=0表示无效
	if (0 == pcfg->bValid)
	{
		MPU_DBG("0 == pcfg->bValid\n");
		return IDVR_CIU_STRUCT_UNVALID;
	}

	if (2 == MPU_WLAN_GetWiFiModuleEnable())
	{
		return nRet;
	}

	WLAN_CONNECTION conn;

    /* 上层传入的认证模式:
       0-OPEN,1-SHARED,2-WEPAUTO,3-WPAPSK,4-WPA2PSK,5-WPANONE,6-WPA,7-WPA2,8-NONE
       底层处理的认证模式:
       0-OPEN,1-SHARED,2-WPAPSK,3-WPA2PSK,4-WPANONE,5-WPA,6-WPA2
       出现错位，需要映射
    */
    switch(pcfg->nAuthMode)
    {
        case 0: nAuthModeMap=0; break;
        case 1: nAuthModeMap=1; break;
        case 2: nAuthModeMap=3; break;
        case 3: nAuthModeMap=2; break;
        case 4: nAuthModeMap=3; break;
        case 5: nAuthModeMap=4; break;
        case 6: nAuthModeMap=5; break;
        case 7: nAuthModeMap=6; break;
        case 8: nAuthModeMap=4; break;
        default: printf("Not Support AuthMode %d\n", pcfg->nAuthMode);return IDVR_CIU_PARAM_OVERFLOW;
    }

	conn.nNetworkType = pcfg->nNetworkType;
	conn.nEncrypType = pcfg->nEncrypType;
	conn.nAuthMode = nAuthModeMap;
	conn.struKeyCfg.bValid = pcfg->struKeyCfg.bValid;
	conn.struKeyCfg.nDefKeyID = pcfg->struKeyCfg.nDefKeyID;
	conn.struKeyCfg.Key1.bValid = pcfg->struKeyCfg.Key1.bValid;
	strncpy(conn.struKeyCfg.Key1.szKey, pcfg->struKeyCfg.Key1.szKey, WLAN_MAX_KEY_LEN - 1);
	conn.struKeyCfg.Key2.bValid = pcfg->struKeyCfg.Key2.bValid;
	strncpy(conn.struKeyCfg.Key2.szKey, pcfg->struKeyCfg.Key2.szKey, WLAN_MAX_KEY_LEN - 1);
	conn.struKeyCfg.Key3.bValid = pcfg->struKeyCfg.Key3.bValid;
	strncpy(conn.struKeyCfg.Key3.szKey, pcfg->struKeyCfg.Key3.szKey, WLAN_MAX_KEY_LEN - 1);
	conn.struKeyCfg.Key4.bValid = pcfg->struKeyCfg.Key4.bValid;
	strncpy(conn.struKeyCfg.Key4.szKey, pcfg->struKeyCfg.Key4.szKey, WLAN_MAX_KEY_LEN - 1);
	conn.struKeyCfg.WPAKey.bValid = pcfg->struKeyCfg.WPAKey.bValid;
	strncpy(conn.struKeyCfg.WPAKey.szKey, pcfg->struKeyCfg.WPAKey.szKey, WLAN_MAX_KEY_LEN - 1);
	strncpy(conn.szMacAddress, pcfg->szMacAddress, WLAN_MAC_ADDRESS_LEN - 1);
	strncpy(conn.szSSID, pcfg->szSSID, WLAN_MAX_SSID_LEN - 1);

	nRet = MPU_WLAN_SetConnectionEx(&conn);
	return nRet;
}

static int WLAN_SetConnectionCfgExV2(LP_HY_DVR_WLAN_CONNECTION_V2 pcfg)
{
	HY_DVR_WLAN_CONNECTION pcfg_old;
	int nRet = IDVR_OK;
	
	memset(&pcfg_old,0,sizeof(HY_DVR_WLAN_CONNECTION));
	pcfg_old.bValid = pcfg->bValid;
	pcfg_old.bConnect = pcfg->bConnect;
	memcpy(&(pcfg_old.szMacAddress[0]),&(pcfg->szMacAddress[0]),DVR_MAX_MAC_STR_LEN);
	pcfg_old.nNetworkType = pcfg->nNetworkType;
	pcfg_old.nAuthMode = pcfg->nAuthMode;
	pcfg_old.nEncrypType = pcfg->nEncrypType;
	memcpy(&(pcfg_old.struKeyCfg),&(pcfg->struKeyCfg),sizeof(HY_DVR_WLAN_KEY_CFG));
	base64Decode((const char *)&(pcfg->szSSID[0]),(unsigned char *)&(pcfg_old.szSSID[0]));		

	nRet = WLAN_SetConnectionCfgEx(&pcfg_old);
	return nRet;
}

static int WLAN_SetConnectionCfgV2(LP_HY_DVR_WLAN_CONNECTION_V2 pcfg)
{
	HY_DVR_WLAN_CONNECTION pcfg_old;
	int nRet = IDVR_OK;
	memset(&pcfg_old,0,sizeof(HY_DVR_WLAN_CONNECTION));
	pcfg_old.bValid = pcfg->bValid;
	pcfg_old.bConnect = pcfg->bConnect;
	memcpy(&(pcfg_old.szMacAddress[0]),&(pcfg->szMacAddress[0]),DVR_MAX_MAC_STR_LEN);
	pcfg_old.nNetworkType = pcfg->nNetworkType;
	pcfg_old.nAuthMode = pcfg->nAuthMode;
	pcfg_old.nEncrypType = pcfg->nEncrypType;
	memcpy(&(pcfg_old.struKeyCfg),&(pcfg->struKeyCfg),sizeof(HY_DVR_WLAN_KEY_CFG));
	base64Decode((const char *)&(pcfg->szSSID[0]),(unsigned char *)&(pcfg_old.szSSID[0]));

	nRet = WLAN_SetConnectionCfg(&pcfg_old);
	return nRet;
}

static int WLAN_SetTestConnectionCfg(LP_HY_DVR_WLAN_CONNECTION pcfg)
{
	int nRet = IDVR_OK;
	int nAuthModeMap = 0;
	//! 本结构体是否有效，=0表示无效
	if (0 == pcfg->bValid)
	{
		MPU_DBG("0 == pcfg->bValid\n");
		return IDVR_CIU_STRUCT_UNVALID;
	}
	
    FUNC_BEGIN;
MPU_DBG("Set ===============================\n\
        pcfg->bConnect = %d,\n\
        pcfg->bValid = %d,\n\
        pcfg->nAuthMode = %d,\n\
        pcfg->nEncrypType = %d,\n\
        pcfg->nNetworkType = %d,\n\
        pcfg->struKeyCfg.bValid = %d,\n\
        pcfg->struKeyCfg.nDefKeyID = %d,\n\
        pcfg->struKeyCfg.Key1.bValid = %d,\n\
        pcfg->struKeyCfg.Key1.nKeyType = %d,\n\
        pcfg->struKeyCfg.Key1.szKey = %s,\n\
        pcfg->struKeyCfg.Key2.bValid = %d,\n\
        pcfg->struKeyCfg.Key2.nKeyType = %d,\n\
        pcfg->struKeyCfg.Key2.szKey = %s,\n\
        pcfg->struKeyCfg.Key3.bValid = %d,\n\
        pcfg->struKeyCfg.Key3.nKeyType = %d,\n\
        pcfg->struKeyCfg.Key3.szKey = %s,\n\
        pcfg->struKeyCfg.Key4.bValid = %d,\n\
        pcfg->struKeyCfg.Key4.nKeyType = %d,\n\
        pcfg->struKeyCfg.Key4.szKey = %s,\n\
        pcfg->struKeyCfg.WPAKey.bValid = %d,\n\
        pcfg->struKeyCfg.WPAKey.nKeyType = %d,\n\
        pcfg->struKeyCfg.WPAKey.szKey = %s,\n\
        pcfg->szMacAddress = %s,\n\
        pcfg->szSSID = %s\n",
    pcfg->bConnect,
    pcfg->bValid,
    pcfg->nAuthMode,
    pcfg->nEncrypType,
    pcfg->nNetworkType,
    pcfg->struKeyCfg.bValid,    
    pcfg->struKeyCfg.nDefKeyID,
    pcfg->struKeyCfg.Key1.bValid,
    pcfg->struKeyCfg.Key1.nKeyType,
    pcfg->struKeyCfg.Key1.szKey,
    pcfg->struKeyCfg.Key2.bValid,
    pcfg->struKeyCfg.Key2.nKeyType,
    pcfg->struKeyCfg.Key2.szKey,
    pcfg->struKeyCfg.Key3.bValid,
    pcfg->struKeyCfg.Key3.nKeyType,
    pcfg->struKeyCfg.Key3.szKey,
    pcfg->struKeyCfg.Key4.bValid,
    pcfg->struKeyCfg.Key4.nKeyType,
    pcfg->struKeyCfg.Key4.szKey,
    pcfg->struKeyCfg.WPAKey.bValid,
    pcfg->struKeyCfg.WPAKey.nKeyType,
    pcfg->struKeyCfg.WPAKey.szKey,
    pcfg->szMacAddress,
    pcfg->szSSID);

	if (2 == MPU_WLAN_GetWiFiModuleEnable())
	{
		return nRet;
	}

	WLAN_CONNECTION conn;

    /* 上层传入的认证模式:
       0-OPEN,1-SHARED,2-WEPAUTO,3-WPAPSK,4-WPA2PSK,5-WPANONE,6-WPA,7-WPA2,8-NONE
       底层处理的认证模式:
       0-OPEN,1-SHARED,2-WPAPSK,3-WPA2PSK,4-WPANONE,5-WPA,6-WPA2
       出现错位，需要映射
    */
    switch(pcfg->nAuthMode)
    {
        case 0: nAuthModeMap=0; break;
        case 1: nAuthModeMap=1; break;
        case 2: nAuthModeMap=3; break;
        case 3: nAuthModeMap=2; break;
        case 4: nAuthModeMap=3; break;
        case 5: nAuthModeMap=4; break;
        case 6: nAuthModeMap=5; break;
        case 7: nAuthModeMap=6; break;
        case 8: nAuthModeMap=4; break;
        default: printf("Not Support AuthMode %d\n", pcfg->nAuthMode);return IDVR_CIU_PARAM_OVERFLOW;
    }

	conn.nNetworkType = pcfg->nNetworkType;
	conn.nEncrypType = pcfg->nEncrypType;
	conn.nAuthMode = nAuthModeMap;
	conn.struKeyCfg.bValid = pcfg->struKeyCfg.bValid;
	conn.struKeyCfg.nDefKeyID = pcfg->struKeyCfg.nDefKeyID;
	conn.struKeyCfg.Key1.bValid = pcfg->struKeyCfg.Key1.bValid;
	strncpy(conn.struKeyCfg.Key1.szKey, pcfg->struKeyCfg.Key1.szKey, WLAN_MAX_KEY_LEN - 1);
	conn.struKeyCfg.Key2.bValid = pcfg->struKeyCfg.Key2.bValid;
	strncpy(conn.struKeyCfg.Key2.szKey, pcfg->struKeyCfg.Key2.szKey, WLAN_MAX_KEY_LEN - 1);
	conn.struKeyCfg.Key3.bValid = pcfg->struKeyCfg.Key3.bValid;
	strncpy(conn.struKeyCfg.Key3.szKey, pcfg->struKeyCfg.Key3.szKey, WLAN_MAX_KEY_LEN - 1);
	conn.struKeyCfg.Key4.bValid = pcfg->struKeyCfg.Key4.bValid;
	strncpy(conn.struKeyCfg.Key4.szKey, pcfg->struKeyCfg.Key4.szKey, WLAN_MAX_KEY_LEN - 1);
	conn.struKeyCfg.WPAKey.bValid = pcfg->struKeyCfg.WPAKey.bValid;
	strncpy(conn.struKeyCfg.WPAKey.szKey, pcfg->struKeyCfg.WPAKey.szKey, WLAN_MAX_KEY_LEN - 1);
	strncpy(conn.szMacAddress, pcfg->szMacAddress, WLAN_MAC_ADDRESS_LEN - 1);
	strncpy(conn.szSSID, pcfg->szSSID, WLAN_MAX_SSID_LEN - 1);

	nRet = MPU_WLAN_SetTestConnection(&conn);
	
    FUNC_ENDIT(nRet);
	return nRet;
}

static int WLAN_GetTestConnStatus(LPHY_DVR_WLAN_STATE_FOR_TEST pcfg)
{
	int nRet = IDVR_OK;

	if (2 == MPU_WLAN_GetWiFiModuleEnable())
	{
		pcfg->bValid = 0;
		return nRet;
	}

	//! 本结构体是否有效，=0表示无效
	pcfg->bValid = 1;

    pcfg->bWifiStatus = MPU_WLAN_GetWifiTestStatus();

    if (-1 == pcfg->bWifiStatus)
    {
        nRet = -1;
    }

    return nRet;
}
#endif

static int WLAN_SetStateCfg(const LPHY_DVR_WLAN_STATE pcfg)
{
	int nRet = IDVR_OK;

	//! 本结构体是否有效，=0表示无效
	if (0 == pcfg->bValid)
	{
		MPU_DBG("0 == pcfg->bValid\n");
		return IDVR_CIU_STRUCT_UNVALID;
	}
#if defined (__IS_SUPPORT_WLAN__)	
    if (pcfg->bEnable >= 2)
    {
        return IDVR_CIU_PARAM_OVERFLOW;
    }

	if (2 == MPU_WLAN_GetWiFiModuleEnable())
	{
		return nRet;
	}
	if(pcfg->bEnable != 1)
	{
		printf("[H4W] ____%s %d___pcfg->bEnable = %d \n", __FUNCTION__, __LINE__, pcfg->bEnable);
	}
	nRet = MPU_WLAN_SetEnable(pcfg->bEnable);
#endif
	return nRet;
}


static int WLAN_GetStateCfg(LPHY_DVR_WLAN_STATE pcfg)
{
	int nRet = IDVR_OK;
	MPU_DBG("\n\n...MPU_CIU_config_net.cpp....WLAN_GetStateCfg()...begin...\n");
#if defined (__IS_SUPPORT_WLAN__)

	if (2 == MPU_WLAN_GetWiFiModuleEnable())
	{
		pcfg->bValid = 0;
		return nRet;
	}

	//! 本结构体是否有效，=0表示无效
	pcfg->bValid = 1;

	pcfg->bEnable = MPU_WLAN_GetWiFiModuleEnable();

    MPU_DBG("pcfg->bEnable = %d\n", pcfg->bEnable);
#endif
	return nRet;
}


//设置DNS配置信息
static int WLAN_SetDnsCfg(LPHY_DVR_DNS_CONF pcfg)
{
	int nRet = IDVR_OK;
#if defined (__IS_SUPPORT_WLAN__)
	int Eth0DhcpSuccTime = 0;
	int Wlan0DhcpSuccTime = 0;

	//! 本结构体是否有效，=0表示无效
	if (0 == pcfg->bValid)
	{
		MPU_DBG("0 == pcfg->bValid\n");
		return IDVR_CIU_STRUCT_UNVALID;
	}

	if (2 == MPU_WLAN_GetWiFiModuleEnable())
	{
		return nRet;
	}
	/* 主DNS服务器 */
	char szOldPrimaryDNS[32] = {0};
	/* 备用DNS */
	char szOldSecondaryDNS[32] = {0};
    MPU_WLAN_GetDNS(szOldPrimaryDNS, szOldSecondaryDNS, sizeof(szOldSecondaryDNS));

    if ((0 == strncmp(szOldPrimaryDNS, pcfg->szPrimaryDNS, DVR_MAX_IP_STR_LEN))
      &&(0 == strncmp(szOldSecondaryDNS, pcfg->szSecondaryDNS, DVR_MAX_IP_STR_LEN)))
    {
        printf("wlan0 dns not change!\n");
        return 0;
    }

	 MPU_NETCARD_GetDhcpSuccTime(&Eth0DhcpSuccTime, &Wlan0DhcpSuccTime);
	if (Wlan0DhcpSuccTime >=1 && Wlan0DhcpSuccTime <= PROTOCOL_SETDNS_WAIT_DHCPSUCC_INTERVAL)
	{
		printf("-----------------[dnstest] wlan0 DNS interval, forbidden set dns !------------ \n");
	}
	else
	{
		MPU_WLAN_SetDNS(pcfg->szPrimaryDNS, pcfg->szSecondaryDNS);
	}

//  MPU_DBG("Primary      DNS: %s", pcfg->szPrimaryDNS);
//  MPU_DBG("Secondary DNS: %s", pcfg->szSecondaryDNS);
#endif
	return nRet;
}

//获取网络配置信息,需要在外部对pcfg清零
static int GetNetCfg(LPHY_DVR_NET_CFG pcfg)
{
	int nRet = IDVR_OK;

	//! 本结构体是否有效，=0表示无效
	pcfg->bValid = 1;

	//GetDhcpCfg(&pcfg->struDhcp_Conf);	//Dhcp配置信息

	//GetDhcpState(&pcfg->struDhcp_State);	//Dhcp状态信息

	//GetEthIF(&pcfg->struEth);	//Eth网络信息

	GetPPPoECfg(&pcfg->struPPPoE_Conf);	//PPPoE配置

	GetPPPoEIF(&pcfg->struPPPoE_IF);	//PPPoE网络信息
	
	//GetDnsCfg(&pcfg->struDNS);	//DNS配置信息

	GetDdnsCfg(&pcfg->struDDNS);	//DDNS配置信息

	GetDhcpCfg_Local(&pcfg->struBasicNetCfg);	//Dhcp配置信息

	GetEthIF_Local(&pcfg->struBasicNetCfg);	//Eth网络信息

	GetDnsCfg_Local(&pcfg->struBasicNetCfg);	//DNS配置信息
	
	GetHttpCfg(&pcfg->struBasicNetCfg);	//http配置

	GetListenPortCfg(&pcfg->struBasicNetCfg);	//服务端口配置信息

	GetMobileCfg(&pcfg->struBasicNetCfg);	//手机端口配置信息

	GetAutoDetectCfg(&pcfg->struBasicNetCfg);	//自动发现端口配置信息

    GetHKPrivateCfg(&pcfg->struBasicNetCfg);    //HK协议配置信息

	GetEmailCfg_1(&pcfg->struEmlCfg);//邮件服务配置
	
	WLAN_GetEthIF(&pcfg->struWIFICommon);

	WLAN_GetDhcpCfg(&pcfg->struWIFIDhcp_Conf);

	WLAN_GetDhcpStateCfg(&pcfg->struWIFIDhcp_State);

	WLAN_GetDnsCfg(&pcfg->struWIFIDNS);

	WLAN_GetStateCfg(&pcfg->struWIFIState);

	WLAN_GetConnectionCfg(&pcfg->struWIFIConnectCfg);

    //printf("pcfg->struWIFIConnectCfg.bConnect=%d\n", pcfg->struWIFIConnectCfg.bConnect);

	//WLAN_GetAccessPointsCfg(&pcfg->struWIFIAccessPoint);
	pcfg->struWIFIAccessPoint.bValid = 0;

	GetTutkUIDCfg(&pcfg->struBasicNetCfg);
	GetUlkUIDCfg(&pcfg->struBasicNetCfg);
	GetIPAdjustCfg(&pcfg->struBasicNetCfg);

	GetuPnPCfg(&pcfg->struUPNPConf); 
	GetDeviceRelyEnable(&pcfg->struBasicNetCfg);
	GetPushAiParam(&pcfg->struBasicNetCfg);

	return nRet;
}

static int SetP6smsgPolicy(HY_DVR_P6SMSG_POLICY_CONFIG * pcfg)
{
	if (NULL == pcfg)
	{
		MPU_ERR("NULL pointer\n");
		return -1;
	}
	int ret = -1;
	ret = MPU_NET_SetP6SMSGServerStatics(pcfg->iPushMode,pcfg->iPushStatus);
	return ret;
}

int CIU_SetConfigure_P6smsgPolicy(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, const BYTE * pBuffer, DWORD  dwBufLen)
{
	int nRet = IDVR_OK;

	switch (dwMinorType)
	{
        case ZWHOST_API_ADD_DOMAIN_FOR_GET_ALIAS:
        {
            HY_DVR_P6SMSG_POLICY_CONFIG * pcfg = (HY_DVR_P6SMSG_POLICY_CONFIG *) pBuffer; 
            
            //判断buf空间是否足够
            if (dwBufLen < sizeof(HY_DVR_P6SMSG_POLICY_CONFIG))
            {
                nRet = IDVR_CIU_BUF_SMALL;
                MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_P6SMSG_POLICY_CONFIG), dwBufLen);
                break;
            } 
			
            nRet = SetP6smsgPolicy(pcfg);
        }
        break;

        default:
        {
            MPU_ERR("unsupported dwMinorType = %lu\n", dwMinorType);
            nRet = IDVR_CIU_UNSUPPORTED_MINORTYPE;
        }
        break;
    }
           
    return nRet;
}

/* 获取拓展的网络参数(WIFI和Email时间间隔等新添加的网络参数) */
static int GetNetCfgEX(LPHY_DVR_NET_CFG_EX pcfg)
{
	int nRet = IDVR_OK;
	int is_supportWifi = 0;
	//! 本结构体是否有效，=0表示无效
	pcfg->bValid = 1;
#if defined (__IS_SUPPORT_WLAN__)
    /* 如果USB wifi存在，则说明该IPC支持USB wifi；否则不支持 */
	ZWSys_GetWifiExist(&is_supportWifi);
#endif

    if (is_supportWifi)
	    pcfg->dwIsSupportWifi = 1;
	else
	    pcfg->dwIsSupportWifi = 0;
	    
	pcfg->dwNetworkWifiDNSEnable = 0;//暂时固定
	pcfg->dwNetworkProtocolSupport = 1;	 /*!<设备是否支持网络协议掩码，0，表示不支持，1，表示支持 */
    
	MPU_DBG("GetNetCfgEX-----dwIsSupportWifi=%d\n", pcfg->dwIsSupportWifi);
#if defined (__IS_SUPPORT_WLAN__)
    int nCurrWifiMode = 0;
	int nCurrNetCardType = 0;
    MPU_NET_GetCurrNetCardType(&nCurrNetCardType);
	switch (nCurrNetCardType)
	{
		default:
		case eNETCARD_WIRED_1:
		case eNETCARD_WIRED_2:
		{
			pcfg->dwNetworkCardSelect = 0;
			break;
		}
		case eNETCARD_WIFI:
		case eNETCARD_3G:
		case eNETCARD_4G:
		case eNETCARD_5G:
		{
			pcfg->dwNetworkCardSelect = 1;
			break;
		}
	}
	//获取当前wifi模式(0:none 1:sta 2:ap)
	MPU_NET_GetCurrWifiMode(&nCurrWifiMode);
	pcfg->dwWifiMode = nCurrWifiMode;

    // 如果当前是无线网卡模式，则需要判断当前是AP模式还是STA模式
    if (pcfg->dwNetworkCardSelect)
    {
        switch(nCurrWifiMode)
        {
            case 0:  // NONE
            {
                pcfg->dwIsSupportNetCardName = 0;
                break;
            }
            case 1:  // STA
            case 2:  // AP
            {
                pcfg->dwIsSupportNetCardName = 1;
                if (2 == nCurrWifiMode)
                {
                    strncpy(pcfg->strNetworkCardName, "wlan0", sizeof(pcfg->strNetworkCardName) - 1);
                }
                else
                {
                    strncpy(pcfg->strNetworkCardName, "wlan0", sizeof(pcfg->strNetworkCardName) - 1);
                }
                break;
            }            
        }
    }
	
	MPU_DBG("GetNetCfgEX-----dwIsSupportWifi=%d\n", pcfg->dwIsSupportWifi);
#endif
    HieServer_GetProtocolLoadState(pcfg->dwNetworkProtocolMask); /*!<设备支持的网络协议 */
	MPU_NET_GetP2PDownloadURL(pcfg->cAppDownloadURL);	

#if defined (__USE_EMAIL__)
    /* 获取收件人邮件发送的时间间隔 */
    nRet =  MPU_EML_GetSendTimeInterval(&pcfg->struEmlCfgEx.nEMLTimeInterval);
	MPU_CHK(0 == nRet);

	/*获取主题*/
	nRet = MPU_EML_GetSubject(pcfg->struEmlCfgEx.strEmailTopic, NET_MAX_SUBJECT_LEN);
	//获取内容
	nRet = MPU_EML_GetBody(pcfg->struEmlCfgEx.strEmailMessage, NET_MAX_EMAIL_BODY_LEN);
	
	MPU_DBG("GetNetCfgEX-----pcfg->struEmlCfgEx.nEMLTimeInterval=%d\n", pcfg->struEmlCfgEx.nEMLTimeInterval);
#endif

	return nRet;
}

//获取中心管理服务器配置信息,需要在外部对pcfg清零
static int GetCmsCfg(LPHY_DVR_NET_CMS pcfg)
{
	int nRet = IDVR_OK;

	//! 本结构体是否有效，=0表示无效
	pcfg->bValid = 1;

	/*0-不启用管理服务器, 1-启用 */
	pcfg->bCmsEnable = MPU_NMS_GetEnable();

	/*远程管理主机地址 */
	MPU_NMS_GetServerIP(pcfg->szManageHostAddress, sizeof(pcfg->szManageHostAddress));

	/*远程管理主机端口 */
	pcfg->wManageHostPort = MPU_NMS_GetServerPort();

	/*注册间隔时间秒 */
	pcfg->nAutoRegIntervals = MPU_NMS_GetAutoRegisterTime();

	return nRet;
}

//获取中心管理服务器状态,需要在外部对pcfg清零
static int GetCmsState(LPHY_DVR_CMS_STATE pcfg)
{
	//目前系统不支持获取cms状态
	pcfg->bValid = 0;

//  MPU_DBG("unsupported get cms state\n");

	return IDVR_CIU_STRUCT_UNSUPPORTED;
}


//获取报警管理服务器配置信息,需要在外部对pcfg清零
static int GetAmsCfg(LPHY_DVR_NET_AMS pcfg)
{
	int nRet = IDVR_OK;

	//! 本结构体是否有效，=0表示无效
	pcfg->bValid = 1;

	/*0-不启用报警服务器, 1-启用 */
	pcfg->bAmsEnable = MPU_AMS_GetEnable();

	/*报警服务器 */
	MPU_AMS_GetServerIP(pcfg->szAlarmHostAddress);

	/*报警服务器端口 */
	pcfg->wAlarmHostPort = MPU_AMS_GetServerPort();

	return nRet;
}

//获取Ntp服务器配置信息,需要在外部对pcfg清零
static int GetNtpCfg(LPHY_DVR_NET_NTP pcfg)
{
	int nRet = IDVR_OK;

	//! 本结构体是否有效，=0表示无效
	pcfg->bValid = 1;

	/*0-不启用, 1-启用 */
	pcfg->bNtpEnable = MPU_NTP_GetEnable();

	/*Ntp服务器 */
	MPU_NTP_GetServerDomainName(pcfg->szNtpHostAddress);

	//目前系统不支持配置
	/*Ntp服务器，端口 */
	//pcfg->wNtpHostPort;

	/*Ntp服务器同步间隔 */
	pcfg->nAutoRegIntervals = MPU_NTP_GetAutoUpdateTime();

	return nRet;
}

#if 0
//获取Email服务器配置信息,需要在外部对pcfg清零
static int GetEmailCfg(LPHY_DVR_NET_EML pcfg)
{
	int nRet = IDVR_OK;
MPU_DBG("-2-MPU_CIU_Config_Net.cpp----GetEmailCfg()--begin---");
	//! 本结构体是否有效，=0表示无效
	pcfg->bValid = 1;

	/*0-不启用, 1-启用 */
	pcfg->bEmlEnable = MPU_EML_GetEnable();

	/*smtp服务器地址 */
	MPU_EML_GetSmtpAddress(pcfg->szSmtpServer, DVR_MAX_DOMAINNAME_LEN);

	/*端口 */
	pcfg->wSmtpPort = MPU_EML_GetSmtpPort();

	/*发件人邮箱地址 */
	MPU_EML_GetAddresserEmail(pcfg->szSenderAddress, DVR_MAX_EMAIL_LEN);

	/*smtp服务器密码 */
	MPU_EML_GetAddresserPassword(pcfg->szSmtpPassword, DVR_MAX_PASSWORD_LEN);

	return nRet;
}
#endif
//获取Email服务器配置信息,需要在外部对pcfg清零
static int GetEmailCfg_1(LPHY_DVR_NET_EML_1 pcfg)
{
	int nRet = IDVR_OK;
	MPU_DBG("-2-MPU_CIU_Config_Net.cpp----GetEmailCfg_1()--begin---\n");
	//! 本结构体是否有效，=0表示无效
	pcfg->bValid = 1;

	/*0-不启用, 1-启用 */
	pcfg->bEmlEnable = MPU_EML_GetEnable();

	/*smtp服务器地址 */
	MPU_EML_GetSmtpAddress(pcfg->szSmtpServer, DVR_MAX_DOMAINNAME_LEN);

	/*端口 */
	pcfg->wSmtpPort = MPU_EML_GetSmtpPort();

	/*验证方式(0为NONE, 1为LOGIN, 2为PLAIN) */
	pcfg->nSmtpAuth = MPU_EML_GetSmtpAuth();
#if 1
	/*是否抓图(0为FALSE 1为TRUE) */
	pcfg->nWithSnapshot = MPU_EML_GetPic_att();
#endif

	/*是否使用安全登陆(0为off, 1为on) */
	pcfg->nSmtpTLS = MPU_EML_GetSecuritytMode();

	/*发件人邮箱地址 */
	MPU_EML_GetAddresserEmail(pcfg->szSenderEmail, DVR_MAX_EMAIL_LEN);

	/*smtp服务器密码 */
	MPU_EML_GetAddresserPassword(pcfg->szSenderPassword, DVR_MAX_PASSWORD_LEN);
#if 1
	/*获取收件人地址*/
	MPU_EML_GetRecvAddresser(pcfg->szReceiverAddress_1,DVR_MAX_EMAIL_LEN,0);
	MPU_EML_GetRecvAddresser(pcfg->szReceiverAddress_2,DVR_MAX_EMAIL_LEN,1);
	MPU_EML_GetRecvAddresser(pcfg->szReceiverAddress_3,DVR_MAX_EMAIL_LEN,2);
#endif


	return nRet;
}

//获取中心管理服务器信息对应的协议（和CMS是绑定的）,需要在外部对pcfg清零
static int GetCmsProtocol(LPHY_DVR_CMS_FOR_PROTOCOL pcfg)
{
	int nRet = IDVR_OK;

	//! 本结构体是否有效，=0表示无效
	pcfg->bValid = 1;

	if (1 == MPU_NMS_GetProtocol())
	{
		pcfg->dwCMSProtocol = PROTOCOL_HXHT;
	}
	else
	{
		pcfg->dwCMSProtocol = PROTOCOL_PRIVATE;
	}

	return nRet;
}



//获取网络管理服务器配置信息,需要在外部对pcfg清零
static int GetNetManager(LPHY_DVR_NET_MANAGER pcfg)
{
	int nRet = IDVR_OK;

	//! 本结构体是否有效，=0表示无效
	pcfg->bValid = 1;

	GetCmsCfg(&pcfg->struCMS);	//中心管理服务器信息    

	GetAmsCfg(&pcfg->struAMS);	//报警服务器信息    

	//GetNtpCfg(&pcfg->struNTP);	//NTP服务器信息 

	//GetEmailCfg(&pcfg->struEML);	//Email服务器信息  

	GetCmsProtocol(&pcfg->struCMSForProtocol);	//中心管理服务器存储注册信息

	GetCmsState(&pcfg->struCmsState);	//中心管理服务器连接状态

	//GetEmailCfg_1(&pcfg->struEML_1);	//Email服务器信息  1.1

	return nRet;
}

//获取NAT状态信息,需要在外部对pcfg清零
static int GetNatStatus(LPHY_DVR_NATSTATUS pcfg)
{
	//! 本结构体是否有效，=0表示无效
	pcfg->bValid = 1;

	/*!< 是否在NAT后面，0表示不在后面，1表示在NAT后面 */
	pcfg->dwIsNAT = MPU_HXHT_GetNAT();

	return IDVR_OK;
}

//获取语音呼叫参数,需要在外部对pcfg清零
static int GetVoiceParam(LPHY_DVR_VOICEPARAM pcfg)
{
	//! 本结构体是否有效，=0表示无效
	pcfg->bValid = 1;

	/*!< 支持的语音编码格式，可以有多个，以逗号隔开；G711U.HX G711A.HX G722.HX */
	snprintf(pcfg->szAudioEncoder, sizeof(pcfg->szAudioEncoder), "G711U.HX");

	/*!< 通道数 */
	pcfg->dwChannel = 1;

	/*!< 采样深度 */
	pcfg->dwBitsPerSample = 16;

	/*!< 采样率 */
	pcfg->dwSamplesPerSec = 8000;

	return IDVR_OK;
}

//获取中心管理服务器存储注册信息,需要在外部对pcfg清零
static int GetCmsStorage(LPHY_DVR_NET_CMS_STOREAGE pcfg)
{
	//! 本结构体是否有效，=0表示无效
	pcfg->bValid = 1;

	/*远程管理主机地址，长度考虑域名 */
	MPU_HXHT_GetStorageServerIP(pcfg->szManageHostAddress, sizeof (pcfg->szManageHostAddress));

	/*远程管理主机端口 */
	pcfg->wManageHostPort = MPU_HXHT_GetStorageServerPort();

	return IDVR_OK;
}

//获取前端设备ID,需要在外部对pcfg清零
static int GetDeviceID(LPHY_DVR_DEVICE_ID pcfg)
{
	//! 本结构体是否有效，=0表示无效
	pcfg->bValid = 1;

	/* 前端设备ID */
	MPU_HXHT_GetDeviceID(pcfg->szDeviceID, sizeof (pcfg->szDeviceID));

	return IDVR_OK;
}

//获取互信互通配置信息,需要在外部对pcfg清零
static int GetHxhtCfg(LPHY_DVR_HXHT_CFG pcfg)
{
	//! 本结构体是否有效，=0表示无效
	pcfg->bValid = 1;

	GetCmsStorage(&pcfg->struCmsStorage);	//中心管理服务器存储注册信息

	GetDeviceID(&pcfg->struDeviceID);	//前端设备ID

	GetNatStatus(&pcfg->struNATStatus);	//NAT状态

	GetVoiceParam(&pcfg->struVoiceParam);	//语音呼叫参数

	return IDVR_OK;
}

#if defined (__NET_PROTOCOL_CONTROL__)
/* 获取第三方协议开关 */
static int GetProtocolOnOff(LPHY_DVR_THIRD_PROTOCOL_MANAGE pcfg)
{
	int nRet = IDVR_OK;
	CConfig *pCfgFile = NULL;
    
	//! 本结构体是否有效，=0表示无效
	pcfg->nValid = 1;
    
    //获取配置文件管理器实例指针
    pCfgFile = CConfig::GetInstance();
    
    if (NULL != pCfgFile)
    {
        pcfg->nJstpProtocol = pCfgFile->GetValue("NetProtocolOnOff", "ProtocolJstp", 0);//海康协议
    }    

	return nRet;

}
#endif

/* 设置拓展的网络参数(WIFI和Email时间间隔等新添加的网络参数) */
static int SetNetCfgEX(LPHY_DVR_NET_CFG_EX pcfg)
{
	int nRet = IDVR_OK;

	//! 本结构体是否有效，=0表示无效
	if (0 == pcfg->bValid)
	{
		MPU_DBG("0 == pcfg->bValid\n");
		return IDVR_CIU_STRUCT_UNVALID;
	}
	 printf("Set Email Time Interval = %d\n", pcfg->struEmlCfgEx.nEMLTimeInterval);
    /* 获取收件人邮件发送的时间间隔 */
    	nRet = MPU_EML_SetSendTimeInterval(pcfg->struEmlCfgEx.nEMLTimeInterval);
	MPU_CHK(0 == nRet);
	//设置邮件主题
	printf("\n\n*****MPU set data pcfg->struEmlCfgEx.strEmailTopic:%s\n",pcfg->struEmlCfgEx.strEmailTopic);
	nRet = MPU_EML_SetSubject(pcfg->struEmlCfgEx.strEmailTopic);
	MPU_CHK(0 == nRet);
	//设置邮件内容
	printf("\n\n*****MPU set data pcfg->struEmlCfgEx.strEmailMessage:%s\n",pcfg->struEmlCfgEx.strEmailMessage);
	nRet = MPU_EML_SetBody(pcfg->struEmlCfgEx.strEmailMessage);
	MPU_CHK(0 == nRet);

	
//	printf("SetNetCfgEX-----pcfg->struEmlCfgEx.nEMLTimeInterval=%d\n", pcfg->struEmlCfgEx.nEMLTimeInterval);

	return nRet;
}

static int SetMessageNotify(HY_DVR_MESSAGE_NOTIFY *pcfg)
{
	int nRet = 0;
	bool bEnable = false;
	//本结构体是否有效，=0表示无效
	if (0 == pcfg->nValid)
	{
		MPU_DBG("0 == pcfg->bValid\n");
		return IDVR_CIU_STRUCT_UNVALID;
	}
	
	bEnable = pcfg->nMessageType == 0 ? true : false;
//和产品讨论P4需要屏蔽叮声，避免app重复下发命令一直触发
#if !defined(__PRODUCT_P4__) && !defined(__PRODUCT_P4U__)
    nRet = ZWSys_SetSpeechContent(SPEECH_SYS_QRCODE_SCAN_OVER);
#endif
    if(nRet != 0)
    {
        //循环查询，等待播放结束
        int i = 0;
        while (1)
        {
            SelectSleepMSec(100);
            if (0 == ZWSys_GetSpeechPlayingStatus())
            {
                break;
            }

            if (++i > (SPEECH_PLAY_TIME_OUT_SEC*10))
            {
                i = 0;
                break;
            }
        }
    }

    #if defined (__USE_P2P_PUSH__)
	nRet = ZWP2PPUSHVNClient_TriggerPushEnable(bEnable, 0);

	MPU_CHK(0 == nRet);
	#endif

	return nRet;
}

//设置DDNS配置信息
static int SetDdnsCfg(LPHY_DVR_DDNS_CONF pcfg)
{
	int nSDKRet, i, bEnable = FALSE, nCurrent = -1;

	//! 本结构体是否有效，=0表示无效
	if (0 == pcfg->bValid)
	{
		MPU_DBG("0 == pcfg->bValid\n");
		return IDVR_CIU_STRUCT_UNVALID;
	}
	MPU_DBG("----SetDdnsCfg--bEnable=%d--nDdnsProtocolCount=%d-----\n",pcfg->bEnable,pcfg->nDdnsProtocolCount);
	/*设置是否启用DDNS*/
	nSDKRet = MPU_DDNS_SetEnable(pcfg->bEnable?true:false);
	MPU_CHK(0 == nSDKRet);
	//当前系统支持的协议列表
	for (i = 0; i < pcfg->nDdnsProtocolCount; ++i)
	{
		LPHY_DVR_DDNS pDdns = &pcfg->struDdnsProtocol[i];

		if (pDdns->bDdnsEnable)
		{
			bEnable = true;	
			nCurrent = i;
		}
		else
		{
			continue;
		}
		MPU_DBG("--i=%d--nDdnsState=%d--szProtocolName=%s--szUserName=%s--szUserPassword=%s-" \
			"-szDomainName=%s--wRegisterPort=%d--nAutoRegIntervals=%d-------\n",
			i,pDdns->nDdnsState,pDdns->szProtocolName,pDdns->szUserName,pDdns->szUserPassword,
			pDdns->szDomainName,pDdns->wRegisterPort,pDdns->nAutoRegIntervals);
		/*注册用户名 */
		nSDKRet = MPU_DDNS_SetUserName(i, pDdns->szUserName);
		MPU_CHK(0 == nSDKRet);

		/*注册用户密码 */
		nSDKRet = MPU_DDNS_SetPassword(i, pDdns->szUserPassword);
		MPU_CHK(0 == nSDKRet);

		/*注册用户域名 */
		nSDKRet = MPU_DDNS_SetDomainName(i, pDdns->szDomainName);
		MPU_CHK(0 == nSDKRet);

		/*自动注册时间 */
		nSDKRet = MPU_DDNS_SetAutoUpdateTime(i, pDdns->nAutoRegIntervals);
		MPU_CHK(0 == nSDKRet);
	}

	/*当前使用的DDNS协议ID */
	if (nCurrent >= 0)
	{
		nSDKRet = MPU_DDNS_SetCurrentProtocol(nCurrent);
		MPU_CHK(0 == nSDKRet);
	}

	if (bEnable)
	{
		nSDKRet = MPU_DDNS_Start();
		MPU_CHK(0 == nSDKRet);
	}
	else
	{
		nSDKRet = MPU_DDNS_Stop();
		MPU_CHK(0 == nSDKRet);
	}

	return IDVR_OK;
}

//设置DHCP配置信息
static int SetDhcpCfg(LPHY_DVR_DHCP_CONF pcfg)
{
	int nRet = IDVR_OK;

	//! 本结构体是否有效，=0表示无效
	if (0 == pcfg->bValid)
	{
		MPU_DBG("0 == pcfg->bValid\n");
		return IDVR_CIU_STRUCT_UNVALID;
	}

	if (MPU_NET_GetDhcpEnable() != pcfg->bDhcpEnable)
	{
		/* 0-不启用Dhcp,1-启用Dhcp */
		MPU_NET_SetDhcpEnable(pcfg->bDhcpEnable);

		//g_bNeedReset = TRUE;
	}

	return nRet;
}

//设置DHCP配置信息
static int SetDhcpCfg_Local(LPZW_SYS_NET_CFG pcfg)
{
	int nRet = IDVR_OK;

	//! 本结构体是否有效，=0表示无效
	if (0 == pcfg->bValid)
	{
		MPU_DBG("0 == pcfg->bValid\n");
		return IDVR_CIU_STRUCT_UNVALID;
	}

	if (MPU_NET_GetDhcpEnable() != (int)pcfg->dwEnableDHCP)
	{
		/* 0-不启用Dhcp,1-启用Dhcp */
		nRet = MPU_NET_SetDhcpEnable(pcfg->dwEnableDHCP);
		
		bNeedUpdateRouter = TRUE;
		//g_bNeedReset = TRUE;
	}

	return nRet;
}

//设置Eth配置信息
static int SetEthIF(LPHY_DVR_ETH_IF pcfg)
{
	int nRet = IDVR_OK;
    
	char szOldAddress[DVR_MAX_IP_STR_LEN] = {0};

	//! 本结构体是否有效，=0表示无效
	if (0 == pcfg->bValid)
	{
		return IDVR_CIU_STRUCT_UNVALID;
	}

    /* 修改IP之前，如果DHCP使能，则需要关闭DHCP */
    if (TRUE == MPU_NET_GetDhcpEnable())
    {
        MPU_NET_SetDhcpEnable(FALSE);
    }
    
	//本地网络适配器信息
	MPU_NET_GetIPAddress(szOldAddress);
	if (0 != strncmp(szOldAddress, pcfg->szIPAddress, DVR_MAX_IP_STR_LEN))
	{
		/* DVR IP地址 */
		MPU_NET_SetIPAddress(pcfg->szIPAddress);
	}

	/* DVR 子网掩码 */
	MPU_NET_SetSubnetMask(pcfg->szSubnetMask);

	/* 网关地址 */
	MPU_NET_SetGateway(pcfg->szGateway);

    //HieServer_RefreshEvent(USEREVENT_ALARM_NOTICE, ALERT_IPCHANGED, 0, 0);
    //HieServer_RefreshEventEx(USEREVENT_ALARM_NOTICE, 0, 
	//	ALERT_IPCHANGED, 0, 0, NULL, 0);

	return nRet;
}

//设置本地Eth配置信息
static int SetEthIF_Local(LPZW_SYS_NET_CFG pcfg)
{
	int nRet = IDVR_OK;
	char szOldAddress[DVR_MAX_IP_STR_LEN] = {0};
	char szOldNetmask[DVR_MAX_MAC_STR_LEN] = {0};
	char szOldGateway[DVR_MAX_IP_STR_LEN] = {0};

	char ipaddr[20]  = {0};
	char netmask[20] = {0};
	char gateway[20] = {0};
	struct in_addr sInaddr_1;
	char str[32] = { 0 };
	//! 本结构体是否有效，=0表示无效

	if (0 == pcfg->bValid)
	{
		MPU_DBG("0 == pcfg->bValid\n");
		return IDVR_CIU_STRUCT_UNVALID;
	}
	/* IP地址 */
	sInaddr_1.s_addr = pcfg->dwIPAddress;
	strncpy(ipaddr, inet_ntop(AF_INET, &sInaddr_1, str, sizeof(str)), sizeof(ipaddr) - 1);
	MPU_NET_GetIPAddress(szOldAddress);
	
	/* 子网掩码 */
	sInaddr_1.s_addr = pcfg->dwSubnetMask;
	strncpy(netmask, inet_ntop(AF_INET, &sInaddr_1, str, sizeof(str)), sizeof(netmask) - 1);
    MPU_NET_GetSubnetMask(szOldNetmask);
    
	/* 网关地址 */
	sInaddr_1.s_addr = pcfg->dwGateway;
	strncpy(gateway, inet_ntop(AF_INET, &sInaddr_1, str, sizeof(str)), sizeof(gateway) - 1);
    MPU_NET_GetGateway(szOldGateway);
	// printf("[ %s %d ]ip = %s | mask = %s | way = %s \n", __FILE__, __LINE__, ipaddr, netmask, gateway);
    /* 设置参数之前，需要先判断参数是否改变 */
	if ((0==strncmp(szOldAddress, ipaddr,  DVR_MAX_IP_STR_LEN))
	 && (0==strncmp(szOldNetmask, netmask, DVR_MAX_MAC_STR_LEN))
	 && (0==strncmp(szOldGateway, gateway, DVR_MAX_IP_STR_LEN)))
	{
	    printf("Local network not change!\n");
	    return 0;
    }

	#if 0
    /* 修改IP之前，如果DHCP使能，则需要关闭DHCP */
    if (TRUE == MPU_NET_GetDhcpEnable())
    {
        MPU_NET_SetDhcpEnable(FALSE);
    }
	#endif

	MPU_NET_SetIPAddress(ipaddr);
    MPU_NET_SetSubnetMask(netmask);
    MPU_NET_SetGateway(gateway);

    //HieServer_RefreshEvent(USEREVENT_ALARM_NOTICE, ALERT_IPCHANGED, 0, 0);
    //HieServer_RefreshEventEx(USEREVENT_ALARM_NOTICE, 0, 
	//	ALERT_IPCHANGED, 0, 0, NULL, 0);
	
    bNeedUpdateRouter = TRUE;

	/* 只读：服务器的物理地址 */

//  MPU_DBG("IP            %s\n", pcfg->szIPAddress);
//  MPU_DBG("netmask %s\n", pcfg->szSubnetMask);
//  MPU_DBG("gateway %s\n", pcfg->szGateway);

	return nRet;
}

//设置PPPoE配置信息
static int SetPPPoECfg(LPHY_DVR_PPPOE_CONF pcfg)
{
	int nRet = IDVR_OK;

	//! 本结构体是否有效，=0表示无效
	if (0 == pcfg->bValid)
	{
		MPU_DBG("0 == pcfg->bValid\n");
		return IDVR_CIU_STRUCT_UNVALID;
	}

	char oldUserName[64] = {0};	
	char oldPassword[64] = {0};
	
	/* PPPoE用户名 */
    MPU_PPPoE_GetUserName(oldUserName);
	/* PPPoE密码 */
    MPU_PPPoE_GetPassword(oldPassword);
    
	if ((0 == strncmp(oldUserName, pcfg->szPPPoEUser, 64))
      &&(0 == strncmp(oldPassword, pcfg->szPPPoEPassword, 64) && (pcfg->bPPPoEEnable == MPU_PPPoE_GetEnable()))
	)
	{
	    printf("pppoe param no change!\n");
	    return 0;
    }	

	/* PPPoE用户名 */
	MPU_PPPoE_SetUserName(pcfg->szPPPoEUser);

	/* PPPoE密码 */
	MPU_PPPoE_SetPassword(pcfg->szPPPoEPassword);

	if (pcfg->bPPPoEEnable != MPU_PPPoE_GetEnable())
	{
		/* 0-不启用PPPoE,1-启用 PPPoE */
		MPU_PPPoE_SetEnable(pcfg->bPPPoEEnable);

		if (0 == pcfg->bPPPoEEnable)
		{
			//禁用以后，断开连接
			MPU_PPPoE_Disconnect();
		}
		else
		{
			//启动以后，开始连接
			MPU_PPPoE_Connect();
		}
	}

	return nRet;
}

//设置DNS配置信息
static int SetDnsCfg(LPHY_DVR_DNS_CONF pcfg)
{
	int nRet = IDVR_OK;

	//! 本结构体是否有效，=0表示无效
	if (0 == pcfg->bValid)
	{
		MPU_DBG("0 == pcfg->bValid\n");
		return IDVR_CIU_STRUCT_UNVALID;
	}

	/*主DNS服务器 */
	char szOldPrimaryDNS[DVR_MAX_IP_STR_LEN] = {0};
	/*备用DNS */
	char szOldSecondaryDNS[DVR_MAX_IP_STR_LEN] = {0};
	
    MPU_ETH0_GetDNS(szOldPrimaryDNS, szOldSecondaryDNS);

    if ((0==strncmp(szOldPrimaryDNS, pcfg->szPrimaryDNS, DVR_MAX_IP_STR_LEN))
      &&(0==strncmp(szOldSecondaryDNS, pcfg->szSecondaryDNS, DVR_MAX_IP_STR_LEN)))
	{
        printf("dns param no change!\n");
        return 0;
	}
	/*主DNS服务器 */
	/*备用DNS */
	MPU_ETH0_SetDNS(pcfg->szPrimaryDNS, pcfg->szSecondaryDNS);

//  MPU_DBG("Primary      DNS: %s", pcfg->szPrimaryDNS);
//  MPU_DBG("Secondary DNS: %s", pcfg->szSecondaryDNS);
	return nRet;
}
//设置DNS配置信息
static int SetDnsCfg_Local(LPZW_SYS_NET_CFG pcfg)
{
	int nRet = IDVR_OK;
	int Eth0DhcpSuccTime = 0;
	int Wlan0DhcpSuccTime = 0;

	//! 本结构体是否有效，=0表示无效
	if (0 == pcfg->bValid)
	{
		MPU_DBG("0 == pcfg->bValid\n");
		return IDVR_CIU_STRUCT_UNVALID;
	}
	
	MPU_PPPoE_SetAutoObtainDNS(pcfg->bAutoObtainDNS);
	//pcfg->bAutoObtainDNS = TRUE;
	char sPrimaryDNS[20] = {0};
	char sSecondaryDNS[20] = {0};
	char sPrimaryDNS_old[20] = {0};
	char sSecondaryDNS_old[20] = {0};
	char str[32] = { 0 };
	struct in_addr sInaddr_1;
	sInaddr_1.s_addr = pcfg->dwPrimaryDNS;
	strncpy(sPrimaryDNS, inet_ntop(AF_INET, &sInaddr_1, str, sizeof(str)), sizeof(sPrimaryDNS) - 1);
	sInaddr_1.s_addr = pcfg->dwSecondaryDNS;
	strncpy(sSecondaryDNS, inet_ntop(AF_INET, &sInaddr_1, str, sizeof(str)), sizeof(sSecondaryDNS) - 1);

    MPU_ETH0_GetDNS(sPrimaryDNS_old, sSecondaryDNS_old);

	/* 主DNS服务器和备用DNS */
    if ((0==strncmp(sPrimaryDNS, sPrimaryDNS_old, 20)) 
     && (0==strncmp(sSecondaryDNS, sSecondaryDNS_old, 20)))
    {
        printf("dns param no change!\n");
        return 0;
    }
    
    // dhcp获取dns成功后一段时间内，不使用协议下发的dns
	MPU_NETCARD_GetDhcpSuccTime(&Eth0DhcpSuccTime, &Wlan0DhcpSuccTime);
	if (Eth0DhcpSuccTime >0 && Eth0DhcpSuccTime <= PROTOCOL_SETDNS_WAIT_DHCPSUCC_INTERVAL)
	{
		printf("-----------------[dnstest] Dhcp Succ interval, forbidden set dns !------------ \n");
	}
	else
	{
		MPU_ETH0_SetDNS(sPrimaryDNS, sSecondaryDNS);
	}
     
	return nRet;
}
//设置http配置信息
static int SetHttpCfg(LPZW_SYS_NET_CFG pcfg, int *needReboot)
{
	int nRet = IDVR_OK;

	//! 本结构体是否有效，=0表示无效
	if (0 == pcfg->bValid)
	{
		MPU_DBG("0 == pcfg->bValid\n");
		return IDVR_CIU_STRUCT_UNVALID;
	}

	if(0 == pcfg->dwHTTPPort)
	{
		MPU_ERR("http port %d is invalid!\n", pcfg->dwHTTPPort);
		return -1;
	}


	/* Http端口 */
	*needReboot = MPU_NET_SetHttpPort(pcfg->dwHTTPPort);
	

	return nRet;
}

//设置本地监听端口配置信息
static int SetListenPortCfg(LPZW_SYS_NET_CFG pcfg, int *needReboot)
{
	int nRet = IDVR_OK, nSDKRet;

	//! 本结构体是否有效，=0表示无效
	if (0 == pcfg->bValid)
	{
		MPU_DBG("0 == pcfg->bValid\n");
		return IDVR_CIU_STRUCT_UNVALID;
	}

	if (MPU_CIU_GetCommandPort() != pcfg->dwCommandPort)
	{
		/*本地命令端口 */
		nSDKRet = MPU_CIU_SetCommandPort(pcfg->dwCommandPort);
		MPU_CHK(0 == nSDKRet);

		MPU_DBG("new command port: %d\n", pcfg->dwCommandPort);
		g_bNeedReset = TRUE;
	}

	if (MPU_CIU_GetMediaPort() != pcfg->dwMediaPort)
	{
		/*本地媒体端口 */
		nSDKRet = MPU_CIU_SetMediaPort(pcfg->dwMediaPort);
		MPU_CHK(0 == nSDKRet);

		MPU_DBG("new media port: %d\n", pcfg->dwMediaPort);
		g_bNeedReset = TRUE;
	}

	/*本地rtp端口 */
	nSDKRet = MPU_CIU_SetRtpPort(pcfg->dwRtpPort, needReboot);
	MPU_CHK(0 == nSDKRet);

	return nRet;
}

//手机配置信息
static int SetTideMobileCfg(LPZW_SYS_NET_CFG pcfg)
{
	int nRet = IDVR_OK, nSDKRet;

	//! 本结构体是否有效，=0表示无效
	if (0 == pcfg->bValid)
	{
		MPU_DBG("0 == pcfg->bValid\n");
		return IDVR_CIU_STRUCT_UNVALID;
	}

	if (MPU_CIU_GetMobilePort() != pcfg->dwMobilePort)
	{
		/*手机端口 */
		nSDKRet = MPU_CIU_SetMobilePort(pcfg->dwMobilePort);
		MPU_CHK(0 == nSDKRet);

		MPU_DBG("new mobile port: %d\n", pcfg->dwMobilePort);
		g_bNeedReset = TRUE;
	}

	return nRet;
}

//手机配置信息
static int SetMobileCfg(LPZW_SYS_NET_CFG pcfg)
{
	int nRet = IDVR_OK, nSDKRet;

	//! 本结构体是否有效，=0表示无效
	if (0 == pcfg->bValid)
	{
		MPU_DBG("0 == pcfg->bValid\n");
		return IDVR_CIU_STRUCT_UNVALID;
	}

	nSDKRet = SetTideMobileCfg(pcfg);	//浪潮手机配置信息
	CIU_ERR(nSDKRet, nRet);

	return nRet;
}

//设置自动发现端口配置信息
static int SetAutoDetectCfg(LPZW_SYS_NET_CFG pcfg)
{
	int nSDKRet=0;

	//! 本结构体是否有效，=0表示无效
	if (0 == pcfg->bValid)
	{
		MPU_DBG("0 == pcfg->bValid\n");
		return IDVR_CIU_STRUCT_UNVALID;
	}

    if (pcfg->wAutoDetectPort==MPU_CIU_GetAutoDetectPort())
    {
        printf("Auto Detect Port no change!\n");
        return 0;
    }
	/*自动发现端口 */
	nSDKRet = MPU_CIU_SetAutoDetectPort(pcfg->wAutoDetectPort);
	MPU_CHK(0 == nSDKRet);

	return (0 == nSDKRet) ? IDVR_OK : IDVR_CIU_CALLSDK_FAIL;
}

static int SetHKPrivateCfg(LPZW_SYS_NET_CFG pcfg)
{
	int nRet = IDVR_OK;

	//! 本结构体是否有效，=0表示无效
	if (0 == pcfg->bValid || 0 == pcfg->HKCommandPortFieldValid)
	{
		MPU_DBG("0 == pcfg->bValid or 0 == pcfg->HKCommandPortFieldValid\n");
		return IDVR_CIU_STRUCT_UNVALID;
	}

    fprintf(stderr, "set HK Port = %d\n",pcfg->HKCommandPort);
    
    if (pcfg->HKCommandPort==MPU_CIU_GetHKPrivatePort())
    {
        printf("HK Private Port no change!\n");
        return 0;
    }

	nRet = MPU_CIU_SetHKPrivatePort(pcfg->HKCommandPort);
    //nRet = MPU_NET_SetHKPrivatePort(pcfg->HKCommandPort)
	return nRet;
}

static WORD iPortArray[8] = {0};
static void * uPnPMap(void * pParam)
{
	//BEGINRUN_THREAD;
	WORD iPortMap[4] = {0};
	WORD iPortList[4] = {0};
	int i;

    DEFINE_THREAD_NAME(__FUNCTION__);
	
	for(i = 0; i < 4; i++)
	{
		iPortMap[i] = iPortArray[i];
		iPortList[i] = iPortArray[i+4];
	}
	
	MPU_UPNP_ADD_PORTMAP(iPortMap, iPortList);
	//ENDRUN_THREAD;
	return NULL;
}

static int SetuPnPCfg(LPHY_DVR_UPNP_CONF pcfg)
{
	int nRet = IDVR_OK,iRet = IDVR_OK;
	WORD iPortList[4] = {0};
	WORD iPortMap[4] = {0};
	WORD iPortListOld[4] = {0};
	int i;

	//! 本结构体是否有效，=0表示无效
	if (0 == pcfg->bValid)
	{
		MPU_DBG("0 == pcfg->bValid\n");
		return IDVR_CIU_STRUCT_UNVALID;
	}
	
    FUNC_BEGIN;

	if (pcfg->bEnable == MPU_UPNP_GetEnable())
	{
		printf("upnp config no change!\n");
		return 0;
	}

	iPortMap[0] = pcfg->nLocalHttpPort;
	iPortMap[1] = pcfg->nLocalCommandPort;
	iPortMap[2] = pcfg->nLocalMediaPort;
	iPortMap[3] = pcfg->nLocalMobilePort;

	iPortList[0] = pcfg->nExportHttpPort;
	iPortList[1] = pcfg->nExportCommandPort;
	iPortList[2] = pcfg->nExportMediaPort;
	iPortList[3] = pcfg->nExportMobilePort;
	
	MPU_UPNP_QUERY_MAPLiST(iPortListOld);

	for(i = 0; i < 4; i++)
	{
		iPortArray[i]   = iPortMap[i];
		iPortArray[i+4] = iPortList[i];
	}
	
	iRet = MPU_UPNP_SetEnable(pcfg->bEnable);
	if(iRet != 0)
	{
		MPU_ERR("MPU_UPNP_SetEnable Error! iRet =%d\n",iRet);
	}
	
	b_Upnp_Enable = pcfg->bEnable;
/*	
	if(1 == pcfg->bEnable && 
		(  iPortListOld[0] != iPortList[0] || iPortListOld[1] != iPortList[1]
		|| iPortListOld[2] != iPortList[2] || iPortListOld[3] != iPortList[3]))
*/		
	if(1 == pcfg->bEnable)
	{
		pthread_t thread_t;
	    iRet = pthread_create(&thread_t, NULL, uPnPMap, 0);	
		if (iRet !=0)
	    {
	        MPU_ERR("[%s]pthread_create failed (%d)\n",__FUNCTION__,iRet);
	    }
		else
		{
			pthread_detach(thread_t);
		}
	}

    FUNC_ENDIT(nRet);

	return IDVR_OK;
}

//设置网络配置信息
static BOOL bNeedReboot = FALSE;
static int SetNetCfg(LPHY_DVR_NET_CFG pcfg)
{
	int nRet, nSDKRet;
	int needRebootHttp=0, needRebootRtsp=0;

    struct timeval tv;
    
	//! 本结构体是否有效，=0表示无效
	if (0 == pcfg->bValid)
	{
		MPU_DBG("0 == pcfg->bValid\n");
		return IDVR_CIU_STRUCT_UNVALID;
	}
	gettimeofday(&tv, NULL);	
#if defined (__MUSL__)
	printf("START===%s===%d====%lld===\n",__FUNCTION__, __LINE__, tv.tv_sec);
#else
	printf("START===%s===%d====%d===\n",__FUNCTION__, __LINE__, tv.tv_sec);
#endif
	//默认返回设置成功
	nRet = IDVR_OK;

	// 优先写dns , 防止覆盖 后续dhcp 获取到的
	nSDKRet = SetDnsCfg_Local(&pcfg->struBasicNetCfg);	//DNS配置信息
	CIU_ERR(nSDKRet, nRet);
	
	nSDKRet = WLAN_SetDnsCfg(&pcfg->struWIFIDNS);
	CIU_ERR(nSDKRet, nRet);

	nSDKRet = SetPPPoECfg(&pcfg->struPPPoE_Conf);	//PPPoE配置
	CIU_ERR(nSDKRet, nRet);

	nSDKRet = SetDdnsCfg(&pcfg->struDDNS);	//DDNS配置信息
	CIU_ERR(nSDKRet, nRet);

	nSDKRet = SetDhcpCfg_Local(&pcfg->struBasicNetCfg);	//Dhcp配置信息
	CIU_ERR(nSDKRet, nRet);

	nSDKRet = SetEthIF_Local(&pcfg->struBasicNetCfg);	//Eth网络信息
	CIU_ERR(nSDKRet, nRet);

	nSDKRet = SetHttpCfg(&pcfg->struBasicNetCfg, &needRebootHttp);	//http配置
	CIU_ERR(nSDKRet, nRet);

	nSDKRet = SetListenPortCfg(&pcfg->struBasicNetCfg, &needRebootRtsp);	//服务端口配置信息
	CIU_ERR(nSDKRet, nRet);

	nSDKRet = SetMobileCfg(&pcfg->struBasicNetCfg);	//手机端口配置信息
	CIU_ERR(nSDKRet, nRet);

	nSDKRet = SetAutoDetectCfg(&pcfg->struBasicNetCfg);	//自动发现端口配置信息
	CIU_ERR(nSDKRet, nRet);

    nSDKRet = SetHKPrivateCfg(&pcfg->struBasicNetCfg); //HK协议端口配置信息
    CIU_ERR(nSDKRet, nRet);

	nSDKRet = WLAN_SetStateCfg(&pcfg->struWIFIState);
	CIU_ERR(nSDKRet, nRet);

	nSDKRet = WLAN_SetEthIF(&pcfg->struWIFICommon);
	CIU_ERR(nSDKRet, nRet);

	nSDKRet = WLAN_SetDhcpCfg(&pcfg->struWIFIDhcp_Conf);
	CIU_ERR(nSDKRet, nRet);

	nSDKRet = WLAN_SetConnectionCfg(&pcfg->struWIFIConnectCfg);
	CIU_ERR(nSDKRet, nRet);
	
	nSDKRet  = SetuPnPCfg(&pcfg->struUPNPConf); //UPNP配置  // ===small struct=== OK
	CIU_ERR(nSDKRet, nRet);

	nSDKRet  = SetEmailCfg_1(&pcfg->struEmlCfg); //邮件服务配置
	CIU_ERR(nSDKRet, nRet);

	nSDKRet  = SetTutkUIDCfg(&pcfg->struBasicNetCfg);
	CIU_ERR(nSDKRet, nRet);

    nSDKRet = SetUlkUIDCfg(&pcfg->struBasicNetCfg);
	CIU_ERR(nSDKRet, nRet);

    nSDKRet = SetIPAdjustCfg(&pcfg->struBasicNetCfg);
	CIU_ERR(nSDKRet, nRet);

	nSDKRet = SetPushAiParam(&pcfg->struBasicNetCfg);
	CIU_ERR(nSDKRet, nRet);

	if(1 == needRebootRtsp)
	{
		restartRtsp();
	}

    if (bNeedUpdateRouter)
    {
        MPU_NET_NeedUpdateRouter();
        bNeedUpdateRouter = FALSE;
    }

	if(1 == needRebootHttp)
	{
		bNeedReboot = TRUE;
	}
  
//  gettimeofday(&tv, NULL);    
//  printf("END===%s===%d====%d===\n",__FUNCTION__, __LINE__, tv.tv_sec);

	return nRet;
}

//设置中心管理服务器配置信息
static int SetCmsCfg(LPHY_DVR_NET_CMS pcfg)
{
	int nRet = IDVR_OK;

	//! 本结构体是否有效，=0表示无效
	if (0 == pcfg->bValid)
	{
		MPU_DBG("0 == pcfg->bValid\n");
		return IDVR_CIU_STRUCT_UNVALID;
	}

	/*0-不启用管理服务器, 1-启用 */
	if (pcfg->bCmsEnable != MPU_NMS_GetEnable())
	{
	    MPU_NMS_SetEnable(pcfg->bCmsEnable);
    }
    
	/*远程管理主机地址 */
	char szOldServerIP[DVR_MAX_DOMAINNAME_LEN] = {0};
	MPU_NMS_GetServerIP(szOldServerIP, sizeof(szOldServerIP));
	if (strncmp(pcfg->szManageHostAddress, szOldServerIP, DVR_MAX_DOMAINNAME_LEN))
	{
	    MPU_NMS_SetServerIP(pcfg->szManageHostAddress);
    }
    
	/*远程管理主机端口 */
    if (pcfg->wManageHostPort != MPU_NMS_GetServerPort())
	{
        MPU_NMS_SetServerPort(pcfg->wManageHostPort);
    }

	/*注册间隔时间秒 */
    if (pcfg->nAutoRegIntervals != MPU_NMS_GetAutoRegisterTime())
	{
        MPU_NMS_SetAutoRegisterTime(pcfg->nAutoRegIntervals);
    }

	return nRet;
}

//设置报警管理服务器配置信息
static int SetAmsCfg(LPHY_DVR_NET_AMS pcfg)
{
	int nRet = IDVR_OK;

	//! 本结构体是否有效，=0表示无效
	if (0 == pcfg->bValid)
	{
		MPU_DBG("0 == pcfg->bValid\n");
		return IDVR_CIU_STRUCT_UNVALID;
	}

	/*0-不启用报警服务器, 1-启用 */
	MPU_AMS_SetEnable(pcfg->bAmsEnable);

	/*报警服务器 */
	MPU_AMS_SetServerIP(pcfg->szAlarmHostAddress);

	/*报警服务器端口 */
	MPU_AMS_SetServerPort(pcfg->wAlarmHostPort);

	return nRet;
}

//设置Ntp服务器配置信息
static int SetNtpCfg(LPHY_DVR_NET_NTP pcfg)
{
	int nRet = IDVR_OK;

	//! 本结构体是否有效，=0表示无效
	if (0 == pcfg->bValid)
	{
		MPU_DBG("0 == pcfg->bValid\n");
		return IDVR_CIU_STRUCT_UNVALID;
	}

	/*0-不启用, 1-启用 */
	MPU_NTP_SetEnable(pcfg->bNtpEnable);

	/*Ntp服务器 */
	MPU_NTP_SetServerDomainName(pcfg->szNtpHostAddress);

	//目前系统不支持配置
	/*Ntp服务器，端口 */
	//pcfg->wNtpHostPort;

	/*Ntp服务器同步间隔 */
	MPU_NTP_SetAutoUpdateTime(pcfg->nAutoRegIntervals);

	return nRet;
}
#if 0
//设置Email服务器配置信息
static int SetEmailCfg(LPHY_DVR_NET_EML pcfg)
{
	int nRet = IDVR_OK;
	MPU_DBG("--MPU_CIU_Config_Net.cpp----SetEmailCfg()--begin---");
	//! 本结构体是否有效，=0表示无效
	if (0 == pcfg->bValid)
	{
		MPU_DBG("0 == pcfg->bValid\n");
		return IDVR_CIU_STRUCT_UNVALID;
	}

	/*0-不启用, 1-启用 */
	MPU_EML_SetEnable(pcfg->bEmlEnable);

	/*smtp服务器地址 */
	MPU_DBG("--MPU_CIU_Config_Net.cpp----SetEmailCfg()---smtp服务器地址=%s----",pcfg->szSmtpServer);
	MPU_EML_SetSmtpAddress(pcfg->szSmtpServer);

	/*端口 */
	MPU_DBG("--MPU_CIU_Config_Net.cpp----SetEmailCfg()---端口=%s----",pcfg->wSmtpPort);
	MPU_EML_SetSmtpPort(pcfg->wSmtpPort);

	/*发件人邮箱地址 */
	MPU_DBG("--MPU_CIU_Config_Net.cpp----SetEmailCfg()---发件人邮箱地址=%s----",pcfg->szSenderAddress);
	MPU_EML_SetAddresserEmail(pcfg->szSenderAddress);

	/*smtp服务器密码 */
	MPU_DBG("--MPU_CIU_Config_Net.cpp----SetEmailCfg()---smtp服务器密码=%s----",pcfg->szSmtpPassword);
	MPU_EML_SetAddresserPassword(pcfg->szSmtpPassword);

	return nRet;
}
#endif
//设置Email服务器配置信息1.1
static int SetEmailCfg_1(LPHY_DVR_NET_EML_1 pcfg)
{
	int nRet = IDVR_OK;

	//! 本结构体是否有效，=0表示无效
	if (0 == pcfg->bValid)
	{
		MPU_DBG("0 == pcfg->bValid\n");
		return IDVR_CIU_STRUCT_UNVALID;
	}

	MPU_DBG("\
		pcfg->bEmlEnable=%d\n\
		pcfg->szReceiverAddress_1=%s\n\
		pcfg->szReceiverAddress_2=%s\n\
		pcfg->szReceiverAddress_3=%s\n\
		\n\n",
		pcfg->bEmlEnable,
		pcfg->szReceiverAddress_1,
		pcfg->szReceiverAddress_2,
		pcfg->szReceiverAddress_3);
	/*0-不启用, 1-启用 */
	MPU_EML_SetEnable(pcfg->bEmlEnable);

	/*smtp服务器地址 */
	MPU_EML_SetSmtpAddress(pcfg->szSmtpServer);

	/*端口 */
	MPU_EML_SetSmtpPort(pcfg->wSmtpPort);

	/*验证方式(0为NONE, 1为LOGIN) */
	MPU_EML_SetSmtpAuth(pcfg->nSmtpAuth);

	/*是否使用安全登陆(0为off, 1为on) */
	MPU_EML_SetSecuritytMode(pcfg->nSmtpTLS);
	MPU_DBG("--SetEmailCfg_1--nSmtpTLS=%d----------\n",pcfg->nSmtpTLS);
#if 1
	/*是否抓图(0为FALSE 1为TRUE) */
	MPU_EML_SetPic_att(pcfg->nWithSnapshot);
	printf("--SetEmailCfg_1--nWithSnapshot=%d----------\n", pcfg->nWithSnapshot);
#endif

	/*发件人邮箱地址 */
	MPU_EML_SetAddresserEmail(pcfg->szSenderEmail);

	/*发件人邮箱密码 */
	MPU_EML_SetAddresserPassword(pcfg->szSenderPassword);
#if 1
	/*收件人邮箱地址*/
	MPU_EML_SetRecvAddresser(pcfg->szReceiverAddress_1,0);
	MPU_EML_SetRecvAddresser(pcfg->szReceiverAddress_2,1);
	MPU_EML_SetRecvAddresser(pcfg->szReceiverAddress_3,2);
#endif

    /*发件人发送邮箱时间间隔*/
//  MPU_EML_SetSendTimeInterval(pcfg->szSendEMLTimeInterval);
    
	return nRet;
}

//設置中心管理服务器信息对应的协议（和CMS是绑定的）
static int SetCmsProtocol(LPHY_DVR_CMS_FOR_PROTOCOL pcfg)
{
	//! 本结构体是否有效，=0表示无效
	if (0 == pcfg->bValid)
	{
		MPU_DBG("0 == pcfg->bValid\n");
		return IDVR_CIU_STRUCT_UNVALID;
	}

	return MPU_NMS_SetProtocol((PROTOCOL_HXHT == pcfg->dwCMSProtocol) ? 1 : 0);
}

//设置网络管理服务器配置信息
static int SetNetManager(LPHY_DVR_NET_MANAGER pcfg)
{
	int nRet, nSDKRet;

	//! 本结构体是否有效，=0表示无效
	if (0 == pcfg->bValid)
	{
		MPU_DBG("0 == pcfg->bValid\n");
		return IDVR_CIU_STRUCT_UNVALID;
	}

	//默认返回设置成功
	nRet = IDVR_OK;

	nSDKRet = SetCmsCfg(&pcfg->struCMS);	//中心管理服务器信息    
	CIU_ERR(nSDKRet, nRet);

	nSDKRet = SetCmsProtocol(&pcfg->struCMSForProtocol);	//中心管理服务器信息对应的协议（和CMS是绑定的）
	CIU_ERR(nSDKRet, nRet);

	nSDKRet = SetAmsCfg(&pcfg->struAMS);	//报警服务器信息    
	CIU_ERR(nSDKRet, nRet);
#if 0
	nSDKRet = SetNtpCfg(&pcfg->struNTP);	//NTP服务器信息 
	CIU_ERR(nSDKRet, nRet);

	nSDKRet = SetEmailCfg(&pcfg->struEML);	//Email服务器信息  
	CIU_ERR(nSDKRet, nRet);

	nSDKRet = SetEmailCfg_1(&pcfg->struEML_1);	//Email服务器信息  1.1
	CIU_ERR(nSDKRet, nRet);
#endif
	return nRet;
}

//设置NAT状态
static int SetNatStatus(LPHY_DVR_NATSTATUS pcfg)
{
	int nRet = IDVR_OK, nSDKRet;

	//! 本结构体是否有效，=0表示无效
	if (0 == pcfg->bValid)
	{
		MPU_DBG("0 == pcfg->bValid\n");
		return IDVR_CIU_STRUCT_UNVALID;
	}

	nSDKRet = MPU_HXHT_SetNAT((int) pcfg->dwIsNAT);
	if (IDVR_OK != nSDKRet)
	{
		MPU_ERR("set nat status (%lu) return %d\n", pcfg->dwIsNAT, nSDKRet);
		nRet = IDVR_CIU_CALLSDK_FAIL;
	}

	return nRet;
}

//设置前端设备ID
static int SetDeviceID(LPHY_DVR_DEVICE_ID pcfg)
{
	int nRet = IDVR_OK, nSDKRet;

	//! 本结构体是否有效，=0表示无效
	if (0 == pcfg->bValid)
	{
		MPU_DBG("0 == pcfg->bValid\n");
		return IDVR_CIU_STRUCT_UNVALID;
	}

	nSDKRet = MPU_HXHT_SetDeviceID(pcfg->szDeviceID);
	if (IDVR_OK != nSDKRet)
	{
		MPU_ERR("set device id (%s) return %d\n", pcfg->szDeviceID, nSDKRet);
		nRet = IDVR_CIU_CALLSDK_FAIL;
	}

	return nRet;
}

//设置中心管理服务器存储注册信息
static int SetCmsStorage(LPHY_DVR_NET_CMS_STOREAGE pcfg)
{
	int nRet = IDVR_OK, nSDKRet;
	MPU_DBG("\n\n....Mpu_ciu_config_Net.cpp.....int SetCmsStorage()..//设置中心管理服务器存储注册信息...\n");
	//! 本结构体是否有效，=0表示无效
	if (0 == pcfg->bValid)
	{
		MPU_DBG("0 == pcfg->bValid\n");
		return IDVR_CIU_STRUCT_UNVALID;
	}

	nSDKRet = MPU_HXHT_SetStorageServerIP(pcfg->szManageHostAddress);
	if (IDVR_OK != nSDKRet)
	{
		MPU_ERR("set storage server ip (%s) return %d\n", pcfg->szManageHostAddress, nSDKRet);
		nRet = IDVR_CIU_CALLSDK_FAIL;
	}

	nSDKRet = MPU_HXHT_SetStorageServerPort(pcfg->wManageHostPort);
	if (IDVR_OK != nSDKRet)
	{
		MPU_ERR("set storage server port (%u) return %d\n", pcfg->wManageHostPort, nSDKRet);
		nRet = IDVR_CIU_CALLSDK_FAIL;
	}

	return nRet;
}

//设置互信互通配置信息
static int SetHxhtCfg(LPHY_DVR_HXHT_CFG pcfg)
{
	int nRet, nSDKRet;
	MPU_DBG("\n....Mpu_ciu_config_Net.cpp.....int SetHxhtCfg().....\n");
	//! 本结构体是否有效，=0表示无效
	if (0 == pcfg->bValid)
	{
		MPU_DBG("0 == pcfg->bValid\n");
		return IDVR_CIU_STRUCT_UNVALID;
	}

	//默认返回设置成功
	nRet = IDVR_OK;

	nSDKRet = SetCmsStorage(&pcfg->struCmsStorage);	//中心管理服务器存储注册信息
	CIU_ERR(nSDKRet, nRet);

	nSDKRet = SetDeviceID(&pcfg->struDeviceID);	//前端设备ID
	CIU_ERR(nSDKRet, nRet);

	nSDKRet = SetNatStatus(&pcfg->struNATStatus);	//NAT状态
	CIU_ERR(nSDKRet, nRet);

	return nRet;
}

#ifdef SAVE_IP_FIRST
static int saveIPfirst(const char* ip, int *ipChanged)
{
	MPU_NET_SetIPAddressEx(ip, ipChanged);
	return 0;
}

static int saveDhcpFirst(const unsigned long dhcpEnable, int *dhcpChanged)
{
	MPU_NET_SetDhcpEnableEx(dhcpEnable, dhcpChanged);
	return 0;
}

static int saveSubMaskFirst(const char* mask, int *subMaskChanged)
{
	MPU_NET_SetSubnetMaskEx(mask, subMaskChanged);
	return 0;
}
#endif

#if defined (__NET_PROTOCOL_CONTROL__)
//设置第三方协议开关
static int SetProtocolOnOff(HY_DVR_THIRD_PROTOCOL_MANAGE *pcfg, int *nNeedReboot)
{
	int ret = 0;
    int nJstp = 0;
	CConfig *pCfgFile = NULL;
    
	///////////////////////////////////////////////////////////////////////////////
	//本结构体是否有效，=0表示无效
	if (0 == pcfg->nValid)
	{
		MPU_ERR("pcfg->bValid = %d \n", pcfg->nValid);
		return IDVR_CIU_STRUCT_UNVALID;
	}

    //获取配置文件管理器实例指针
    pCfgFile = CConfig::GetInstance();

    if (pcfg->nJstpProtocol < 0 || pcfg->nJstpProtocol > 1)
    {
        MPU_DBG("pcfg->nJstpProtocol must be in [0,1]\n");
        return IDVR_CIU_PARAM_OVERFLOW;
    }
    
    if (NULL != pCfgFile)
    {
        //海康协议
        nJstp = pCfgFile->GetValue("NetProtocolOnOff", "ProtocolJstp", 0);
        if (pcfg->nJstpProtocol != nJstp)
        {
            pCfgFile->SetValue("NetProtocolOnOff", "ProtocolJstp", pcfg->nJstpProtocol);
            mysystem("reboot -d 3 &");
        }
    }
    
	return ret;
}
#endif

static int SetConfigure_Net(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, const BYTE * pBuffer, DWORD dwBufLen)
{
	int nRet = IDVR_OK;
	int nSDKRet = IDVR_OK;
    
	printf("ID %d, Major %d, Minor %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType);

	//默认不需要重启网络服务
	g_bNeedReset = FALSE;

	switch (dwMinorType)
	{
			//设置网络适配器配置信息
		case NETCFG_ALL:
			{
				LPHY_DVR_NET_CFG pcfg = (LPHY_DVR_NET_CFG) pBuffer;

				//判断buf空间是否足够
				if (dwBufLen < NET_CFG_LEN)
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", NET_CFG_LEN, dwBufLen);
					break;
				}

				nRet = SetNetCfg(pcfg);
			}
			break;

			/*Dhcp配置信息 */
		case NETCFG_DHCP_CONF:
			{
				LPHY_DVR_DHCP_CONF pcfg = (LPHY_DVR_DHCP_CONF) pBuffer;

				//判断buf空间是否足够
				if (dwBufLen < sizeof (HY_DVR_DHCP_CONF))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof (HY_DVR_DHCP_CONF), dwBufLen);
					break;
				}

				nRet = SetDhcpCfg(pcfg);
			}
			break;

			/*Dhcp状态信息          */
		case NETCFG_DHCP_STATE:
		case NETCFG_WLAN_DHCP_STATE:
			{
				//LPHY_DVR_DHCP_STATE pcfg = (LPHY_DVR_DHCP_STATE) pBuffer;
				//判断buf空间是否足够
				if (dwBufLen < sizeof (HY_DVR_DHCP_STATE))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof (HY_DVR_DHCP_STATE), dwBufLen);
					break;
				}

				MPU_ERR("dhcp state is read only\n");
				nRet = IDVR_CIU_STRUCT_READONLY;
			}
			break;

			/*Eth网络信息               */
		case NETCFG_ETH_IF:
			{
				LPHY_DVR_ETH_IF pcfg = (LPHY_DVR_ETH_IF) pBuffer;
				//判断buf空间是否足够
				if (dwBufLen < sizeof (HY_DVR_ETH_IF))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof (HY_DVR_ETH_IF), dwBufLen);
					break;
				}

				nRet = SetEthIF(pcfg);
			}
			break;

			/*PPPoE配置                 */
		case NETCFG_PPPOE_CONF:
			{
				LPHY_DVR_PPPOE_CONF pcfg = (LPHY_DVR_PPPOE_CONF) pBuffer;
				//判断buf空间是否足够
				if (dwBufLen < sizeof (HY_DVR_PPPOE_CONF))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof (HY_DVR_PPPOE_CONF), dwBufLen);
					break;
				}

				nRet = SetPPPoECfg(pcfg);
			}
			break;

			/*PPPoE网络信息         */
		case NETCFG_PPPOE_IF:
			{
				//LPHY_DVR_PPPOE_IF pcfg = (LPHY_DVR_PPPOE_IF) pBuffer;
				//判断buf空间是否足够
				if (dwBufLen < sizeof (HY_DVR_PPPOE_IF))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof (HY_DVR_PPPOE_IF), dwBufLen);
					break;
				}

				MPU_ERR("pppoe net info is read only\n");
				nRet = IDVR_CIU_STRUCT_READONLY;
			}
			break;

			/*DNS配置                 */
		case NETCFG_DNS_CONF:
			{
				LPHY_DVR_DNS_CONF pcfg = (LPHY_DVR_DNS_CONF) pBuffer;
				//判断buf空间是否足够
				if (dwBufLen < sizeof (HY_DVR_DNS_CONF))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof (HY_DVR_DNS_CONF), dwBufLen);
					break;
				}

				nRet = SetDnsCfg(pcfg);
			}
			break;

			/*DDNS配置                 */
		case NETCFG_DDNS_CONF:
			{
				LPHY_DVR_DDNS_CONF pcfg = (LPHY_DVR_DDNS_CONF) pBuffer;
				//判断buf空间是否足够
				if (dwBufLen < sizeof (HY_DVR_DDNS_CONF))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof (HY_DVR_DDNS_CONF), dwBufLen);
					break;
				}

				nRet = SetDdnsCfg(pcfg);
			}
			break;
#if 0
			/*Http配置                 */
		case NETCFG_HTTP_CONF:
			{
				LPHY_DVR_HTTP_CONF pcfg = (LPHY_DVR_HTTP_CONF) pBuffer;
				//判断buf空间是否足够
				if (dwBufLen < sizeof (HY_DVR_HTTP_CONF))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof (HY_DVR_HTTP_CONF), dwBufLen);
					break;
				}

				nRet = SetHttpCfg(pcfg);
			}
			break;

			/*服务端口配置信息  */
		case NETCFG_LISTENPORT_CONF:
			{
				LPHY_DVR_LISTENPORT_CONF pcfg = (LPHY_DVR_LISTENPORT_CONF) pBuffer;
				//判断buf空间是否足够
				if (dwBufLen < LISTENPORT_CONF_LEN)
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", LISTENPORT_CONF_LEN, dwBufLen);
					break;
				}

				nRet = SetListenPortCfg(pcfg);
			}
			break;

			/*手机端口配置信息  */
		case NETCFG_MOBILE_CONF:
			{
				LPHY_DVR_MOBILE_CONF pcfg = (LPHY_DVR_MOBILE_CONF) pBuffer;
				//判断buf空间是否足够
				if (dwBufLen < MOBILE_CONF_LEN)
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", MOBILE_CONF_LEN, dwBufLen);
					break;
				}

				nRet = SetMobileCfg(pcfg);
			}
			break;

			/*自动发现端口配置信息  */
		case NETCFG_AUTODETECT_CONF:
			{
				LPHY_DVR_AUTODETECT_CONF pcfg = (LPHY_DVR_AUTODETECT_CONF) pBuffer;
				//判断buf空间是否足够
				if (dwBufLen < AUTODETECT_CONF_LEN)
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", AUTODETECT_CONF_LEN, dwBufLen);
					break;
				}

				nRet = SetAutoDetectCfg(pcfg);
			}
			break;
#endif
			/*ZW_SYS_NET_CFG  配置信息  */
		case NETCFG_BASIC_CONF:
			{
				LPZW_SYS_NET_CFG pcfg = (LPZW_SYS_NET_CFG) pBuffer;
				int needRebootHttp=0,needRebootRtsp=0;
				//判断buf空间是否足够
				if (dwBufLen < ZW_NET_CFG_LEN)
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", ZW_NET_CFG_LEN, dwBufLen);
					break;
				}

				nSDKRet = SetDnsCfg_Local(pcfg);
                CIU_ERR(nSDKRet, nRet);

				//Onvif过审，同时设置dhcp失能和IP时，先操作ip，再操作dhcp
				if ((TRUE == MPU_NET_GetDhcpEnable()) && (0 == pcfg->dwEnableDHCP))
				{
					nSDKRet = SetEthIF_Local(pcfg);
					CIU_ERR(nSDKRet, nRet);
				}
				else
				{
					nSDKRet = SetDhcpCfg_Local(pcfg);
					CIU_ERR(nSDKRet, nRet);
				}
                
				nSDKRet = SetHttpCfg(pcfg,&needRebootHttp);
                CIU_ERR(nSDKRet, nRet);
                
				nSDKRet = SetListenPortCfg(pcfg, &needRebootRtsp);
                CIU_ERR(nSDKRet, nRet);
                
				nSDKRet = SetMobileCfg(pcfg);
                CIU_ERR(nSDKRet, nRet);

				if ((TRUE == MPU_NET_GetDhcpEnable()) && (0 == pcfg->dwEnableDHCP))
				{
					nSDKRet = SetDhcpCfg_Local(pcfg);
					CIU_ERR(nSDKRet, nRet);
				}
				else
				{
					nSDKRet = SetEthIF_Local(pcfg);
					CIU_ERR(nSDKRet, nRet);					
				}	
                
				nSDKRet = SetAutoDetectCfg(pcfg);
                CIU_ERR(nSDKRet, nRet);
                
				nSDKRet = SetTutkUIDCfg(pcfg);
                CIU_ERR(nSDKRet, nRet);
                
				nSDKRet = SetUlkUIDCfg(pcfg);
                CIU_ERR(nSDKRet, nRet);
                
				nSDKRet = SetHKPrivateCfg(pcfg);
                CIU_ERR(nSDKRet, nRet);
				
				if(1 == needRebootRtsp)
					restartRtsp();

				if((1 == needRebootHttp))
				{
					MPU_PAR_SaveConfigReal();
					PAR_ForbiddenSaveFile();				
					//确保保存后，再重启
					mysystem("sync");
					sleep(3);
					mysystem("reboot -d 3 &");
				}	
			}
			break;
		/* EML配置信息1.1 */
		case NETCFG_EMAIL_CONF:
			{
				LPHY_DVR_NET_EML_1 pcfg = (LPHY_DVR_NET_EML_1) pBuffer;

				//判断buf空间是否足够
				if (dwBufLen < sizeof (HY_DVR_NET_EML_1))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof (HY_DVR_NET_EML_1), dwBufLen);
					break;
				}

				nRet = SetEmailCfg_1(pcfg);
			}
			break;
		case NETCFG_WLAN_IF:
			{
				LPHY_DVR_ETH_IF pcfg = (LPHY_DVR_ETH_IF) pBuffer;
				//判断buf空间是否足够
				if (dwBufLen < sizeof (HY_DVR_ETH_IF))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof (HY_DVR_ETH_IF), dwBufLen);
					break;
				}

				nRet = WLAN_SetEthIF(pcfg);
			}
			break;
		case NETCFG_WLAN_DHCP_CONF:
			{
				LPHY_DVR_DHCP_CONF pcfg = (LPHY_DVR_DHCP_CONF) pBuffer;
				//判断buf空间是否足够
				if (dwBufLen < sizeof (HY_DVR_DHCP_CONF))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof (HY_DVR_DHCP_CONF), dwBufLen);
					break;
				}

				nRet = WLAN_SetDhcpCfg(pcfg);
			}
			break;
			/*DNS配置                 */
		case NETCFG_WLAN_DNS_CONF:
			{
				LPHY_DVR_DNS_CONF pcfg = (LPHY_DVR_DNS_CONF) pBuffer;
				//判断buf空间是否足够
				if (dwBufLen < sizeof (HY_DVR_DNS_CONF))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof (HY_DVR_DNS_CONF), dwBufLen);
					break;
				}

				nRet = WLAN_SetDnsCfg(pcfg);
			}
			break;
		case NETCFG_WLAN_ACCESSPOINTS:
			//READ ONLY
			break;
		case NETCFG_WLAN_CONNECTION:
			{
				LP_HY_DVR_WLAN_CONNECTION pcfg = (LP_HY_DVR_WLAN_CONNECTION) pBuffer;
				//判断buf空间是否足够
				if (dwBufLen < sizeof (HY_DVR_WLAN_CONNECTION))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof (HY_DVR_DHCP_CONF), dwBufLen);
					break;
				}

				nRet = WLAN_SetConnectionCfg(pcfg);
			}
			break;
		case NETCFG_WLAN_CONNECTION_INFO_V2:
			{
#if defined (__IS_SUPPORT_WLAN__)
				LP_HY_DVR_WLAN_CONNECTION_V2 pcfg = (LP_HY_DVR_WLAN_CONNECTION_V2) pBuffer;
				//判断buf空间是否足够
				if (dwBufLen < sizeof (HY_DVR_WLAN_CONNECTION_V2))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof (HY_DVR_WLAN_CONNECTION_V2), dwBufLen);
					break;
				}

				nRet = WLAN_SetConnectionCfgV2(pcfg);
#endif
			}
			break;
		case NETCFG_WLAN_STATE:
			{
				LPHY_DVR_WLAN_STATE pcfg = (LPHY_DVR_WLAN_STATE) pBuffer;
				//判断buf空间是否足够
				if (dwBufLen < sizeof (LPHY_DVR_WLAN_STATE))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof (LPHY_DVR_WLAN_STATE), dwBufLen);
					break;
				}
				printf("[H4W] ____%s %d___pcfg->bEnable = %d \n", __FUNCTION__, __LINE__, pcfg->bEnable);
				nRet = WLAN_SetStateCfg(pcfg);
				break;
			}
		case NETCFG_UPNP_CONF:
			{
				break;
			}
		case NETCFG_WHO_PROBE_CONF:
			{
				LPHY_DVR_WHOPROBE_CONF pcfg = (LPHY_DVR_WHOPROBE_CONF) pBuffer;
				//判断buf空间是否足够
				if (dwBufLen < sizeof (HY_DVR_WHOPROBE_CONF))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof (HY_DVR_WHOPROBE_CONF), dwBufLen);
					break;
				}
				dwBufLen = sizeof (HY_DVR_WHOPROBE_CONF);
				nRet = SetIpByProbe(pcfg);	
				break;
			}
		
        /* 设置FTP参数 */
		case NETCFG_FTP_CONF_V2:
			{
				LPHY_DVR_FTP_CONF_V2 pcfg = (LPHY_DVR_FTP_CONF_V2) pBuffer;
				//判断buf空间是否足够
				if (dwBufLen < sizeof (HY_DVR_FTP_CONF_V2))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof (HY_DVR_FTP_CONF_V2), dwBufLen);
					break;
				}
				dwBufLen = sizeof (HY_DVR_FTP_CONF_V2);

                printf("\n[Set]pcfg->bEnable:%d,\
pcfg->iFtpState:%d,\
pcfg->wFtpPort:%d,\
pcfg->szIPAddress:%s,\
pcfg->szUserName:%s,\
pcfg->szUserPassword:%s,\
pcfg->nTransferContentTypeMask:%d\
pcfg->nFtpMode=%d\
pcfg->cFtpRemoteServerStoragePath=%s\n\n",
                    pcfg->bEnable, 
                    pcfg->iFtpState, 
                    pcfg->wFtpPort, 
                    pcfg->szIPAddress,
                    pcfg->szUserName, 
                    pcfg->szUserPassword,
                    pcfg->nTransferContentTypeMask,
                    pcfg->nFtpMode,
                    pcfg->cFtpRemoteServerStoragePath);
				
				nRet = SetFtpUserParam(pcfg);				
				break;
			}
			
        case NETCFG_WIFI_CONNECT_PARAM_CHECK:
			{
#if defined (__IS_SUPPORT_WLAN__)
				LP_HY_DVR_WLAN_CONNECTION pcfg = (LP_HY_DVR_WLAN_CONNECTION) pBuffer;
				//判断buf空间是否足够
				if (dwBufLen != sizeof (HY_DVR_WLAN_CONNECTION))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof (HY_DVR_DHCP_CONF), dwBufLen);
					break;
				}

				nRet = WLAN_SetTestConnectionCfg(pcfg);
#endif
			}
			break;
        
		default:
			{
				MPU_ERR("unsupported dwMinorType = %lu\n", dwMinorType);
				nRet = IDVR_CIU_UNSUPPORTED_MINORTYPE;
			}
			break;
	}

	if (IDVR_OK == nRet)
	{
		//记录日志，设置网络参数
		MPU_DBG("\n....MPU_CIU_Config_NET.cpp......SetConfigure_Net().......\n");
		CIU_OPL_AddLog(dwUserID, EDVR_EVENT_SETTING, EDVR_EVENT_SETTING_DETAIL_NETWORK_SETTING, 0, 0);
		
		
		//设置完成以后，重启网络服务
		MPU_DBG("\n....MPU_CIU_Config_NET.cpp......SetConfigure_Net().........\n");
		if (!bNeedReboot)
    	{
			MPU_PAR_SaveConfigReal();
		}

	}
    if (bNeedReboot == TRUE)
    {
    	MPU_PAR_SaveConfigReal();
		PAR_ForbiddenSaveFile();    
        MPU_INFO("\n...网络参数修改，需要重启...\n");
        bNeedReboot = FALSE;
		//确保保存后，再重启
		mysystem("sync");
		sleep(3);
        mysystem("reboot -d 3 &");
    }

	if (g_bNeedReset)
	{
		//需要重启网络服务
		MPU_INFO("\n....MPU_CIU_Config_NET.cpp......SetConfigure_Net().... 需要重启网络服务.......\n");
		MPU_INFO("call CIU_ResetServer(1)\n");
		CIU_ResetServer(1);
	}

	MPU_DBG("ID %d, Major %d, Minor %d, return %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType, nRet);
	return nRet;
}

int CIU_GetConfigure_Net(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen)
{
	int nRet = IDVR_OK;

	//MPU_INFO("ID %d, Major %d, Minor %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType);
	switch (dwMinorType)
	{
			//获取网络适配器配置信息
		case NETCFG_ALL:
			{
				LPHY_DVR_NET_CFG  pcfg = (LPHY_DVR_NET_CFG) pBuffer;

				//判断buf空间是否足够
				if (*pdwBufLen < NET_CFG_LEN)
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", NET_CFG_LEN, *pdwBufLen);
					break;
				}

				memset(pcfg, 0, NET_CFG_LEN);
				*pdwBufLen = NET_CFG_LEN;

				nRet = GetNetCfg(pcfg);

 //             printf("pcfg->struWIFIConnectCfg.nEncrypType = %d\n", pcfg->struWIFIConnectCfg.nEncrypType);
				
			}
			break;

			//获取DHCP配置信息
		case NETCFG_DHCP_CONF:
			{
				LPHY_DVR_DHCP_CONF pcfg = (LPHY_DVR_DHCP_CONF) pBuffer;

				//判断buf空间是否足够
				if (*pdwBufLen < sizeof (HY_DVR_DHCP_CONF))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof (HY_DVR_DHCP_CONF), *pdwBufLen);
					break;
				}

				memset(pcfg, 0, sizeof (HY_DVR_DHCP_CONF));
				*pdwBufLen = sizeof (HY_DVR_DHCP_CONF);

				nRet = GetDhcpCfg(pcfg);
			}
			break;

			//获取DHCP状态信息
		case NETCFG_DHCP_STATE:
		case NETCFG_WLAN_DHCP_STATE:
			{
				LPHY_DVR_DHCP_STATE pcfg = (LPHY_DVR_DHCP_STATE) pBuffer;

				//判断buf空间是否足够
				if (*pdwBufLen < sizeof (HY_DVR_DHCP_STATE))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof (HY_DVR_DHCP_STATE), *pdwBufLen);
					break;
				}

				memset(pcfg, 0, sizeof (HY_DVR_DHCP_STATE));
				*pdwBufLen = sizeof (HY_DVR_DHCP_STATE);

				nRet = GetDhcpState(pcfg);
			}
			break;

			//获取eth0配置信息
		case NETCFG_ETH_IF:
			{
				LPHY_DVR_ETH_IF pcfg = (LPHY_DVR_ETH_IF) pBuffer;

				//判断buf空间是否足够
				if (*pdwBufLen < sizeof (HY_DVR_ETH_IF))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof (HY_DVR_ETH_IF), *pdwBufLen);
					break;
				}

				memset(pcfg, 0, sizeof (HY_DVR_ETH_IF));
				*pdwBufLen = sizeof (HY_DVR_ETH_IF);

				nRet = GetEthIF(pcfg);
			}
			break;

			//获取PPPoE配置信息
		case NETCFG_PPPOE_CONF:
			{
				LPHY_DVR_PPPOE_CONF pcfg = (LPHY_DVR_PPPOE_CONF) pBuffer;

				//判断buf空间是否足够
				if (*pdwBufLen < sizeof (HY_DVR_PPPOE_CONF))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof (HY_DVR_PPPOE_CONF), *pdwBufLen);
					break;
				}

				memset(pcfg, 0, sizeof (HY_DVR_PPPOE_CONF));
				*pdwBufLen = sizeof (HY_DVR_PPPOE_CONF);

				nRet = GetPPPoECfg(pcfg);
			}
			break;

			//获取PPPoE网络信息
		case NETCFG_PPPOE_IF:
			{
				LPHY_DVR_PPPOE_IF pcfg = (LPHY_DVR_PPPOE_IF) pBuffer;

				//判断buf空间是否足够
				if (*pdwBufLen < sizeof (HY_DVR_PPPOE_IF))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof (HY_DVR_PPPOE_IF), *pdwBufLen);
					break;
				}

				memset(pcfg, 0, sizeof (HY_DVR_PPPOE_IF));
				*pdwBufLen = sizeof (HY_DVR_PPPOE_IF);

				nRet = GetPPPoEIF(pcfg);
			}
			break;

			//获取DNS配置信息
		case NETCFG_DNS_CONF:
			{
				LPHY_DVR_DNS_CONF pcfg = (LPHY_DVR_DNS_CONF) pBuffer;

				//判断buf空间是否足够
				if (*pdwBufLen < sizeof (HY_DVR_DNS_CONF))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof (HY_DVR_DNS_CONF), *pdwBufLen);
					break;
				}

				memset(pcfg, 0, sizeof (HY_DVR_DNS_CONF));
				*pdwBufLen = sizeof (HY_DVR_DNS_CONF);

				nRet = GetDnsCfg(pcfg);
			}
			break;

			//获取DDNS配置信息
		case NETCFG_DDNS_CONF:
			{
				LPHY_DVR_DDNS_CONF pcfg = (LPHY_DVR_DDNS_CONF) pBuffer;

				//判断buf空间是否足够
				if (*pdwBufLen < sizeof (HY_DVR_DDNS_CONF))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof (HY_DVR_DDNS_CONF), *pdwBufLen);
					break;
				}

				memset(pcfg, 0, sizeof (HY_DVR_DDNS_CONF));
				*pdwBufLen = sizeof (HY_DVR_DDNS_CONF);

				nRet = GetDdnsCfg(pcfg);
			}
			break;
#if 0
			//获取HTTP配置信息
		case NETCFG_HTTP_CONF:
			{
				LPHY_DVR_HTTP_CONF pcfg = (LPHY_DVR_HTTP_CONF) pBuffer;

				//判断buf空间是否足够
				if (*pdwBufLen < sizeof (HY_DVR_HTTP_CONF))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof (HY_DVR_HTTP_CONF), *pdwBufLen);
					break;
				}

				memset(pcfg, 0, sizeof (HY_DVR_HTTP_CONF));
				*pdwBufLen = sizeof (HY_DVR_HTTP_CONF);

				nRet = GetHttpCfg(pcfg);
			}
			break;

			/*服务端口配置信息*/
		case NETCFG_LISTENPORT_CONF:
			{
				LPHY_DVR_LISTENPORT_CONF pcfg = (LPHY_DVR_LISTENPORT_CONF) pBuffer;

				//判断buf空间是否足够
				if (*pdwBufLen < LISTENPORT_CONF_LEN)
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", LISTENPORT_CONF_LEN, *pdwBufLen);
					break;
				}

				memset(pcfg, 0, LISTENPORT_CONF_LEN);
				*pdwBufLen = LISTENPORT_CONF_LEN;

				nRet = GetListenPortCfg(pcfg);
			}
			break;

			/*手机端口配置信息  */
		case NETCFG_MOBILE_CONF:
			{
				LPHY_DVR_MOBILE_CONF pcfg = (LPHY_DVR_MOBILE_CONF) pBuffer;

				//判断buf空间是否足够
				if (*pdwBufLen < MOBILE_CONF_LEN)
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", MOBILE_CONF_LEN, *pdwBufLen);
					break;
				}

				memset(pcfg, 0, MOBILE_CONF_LEN);
				*pdwBufLen = MOBILE_CONF_LEN;

				nRet = GetMobileCfg(pcfg);
			}
			break;

			/*自动发现端口配置信息  */
		case NETCFG_AUTODETECT_CONF:
			{
				LPHY_DVR_AUTODETECT_CONF pcfg = (LPHY_DVR_AUTODETECT_CONF) pBuffer;

				//判断buf空间是否足够
				if (*pdwBufLen < AUTODETECT_CONF_LEN)
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", AUTODETECT_CONF_LEN, *pdwBufLen);
					break;
				}

				memset(pcfg, 0, AUTODETECT_CONF_LEN);
				*pdwBufLen = AUTODETECT_CONF_LEN;

				nRet = GetAutoDetectCfg(pcfg);
			}
			break;
#endif
		case NETCFG_BASIC_CONF:
			{
				LPZW_SYS_NET_CFG pcfg = (LPZW_SYS_NET_CFG) pBuffer;

				//判断buf空间是否足够
				if (*pdwBufLen < ZW_NET_CFG_LEN)
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", ZW_NET_CFG_LEN, *pdwBufLen);
					break;
				}
				memset(pcfg, 0, sizeof(ZW_SYS_NET_CFG));
				pcfg->bValid = 1;
				nRet = GetDhcpCfg_Local(pcfg);
				nRet = GetHttpCfg(pcfg);
				nRet = GetListenPortCfg(pcfg);
				nRet = GetMobileCfg(pcfg);
				nRet = GetEthIF_Local(pcfg);
				nRet = GetDnsCfg_Local(pcfg);
				nRet = GetAutoDetectCfg(pcfg);
				nRet = GetTutkUIDCfg(pcfg);
				nRet = GetUlkUIDCfg(pcfg);
				nRet = GetHKPrivateCfg(pcfg);
				nRet = GetDeviceRelyEnable(pcfg);
				nRet = GetPushAiParam(pcfg);
				//MPU_DBG("GetTutkUIDCfg return %d\n", nRet);
			}
			break;
			/* smtp配置信息1.1 */
		case NETCFG_EMAIL_CONF:
			{
				LPHY_DVR_NET_EML_1 pcfg = (LPHY_DVR_NET_EML_1) pBuffer;

				//判断buf空间是否足够
				if (*pdwBufLen < sizeof (HY_DVR_NET_EML_1))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof (HY_DVR_NET_EML_1), *pdwBufLen);
					break;
				}

				memset(pcfg, 0, sizeof (HY_DVR_NET_EML_1));
				*pdwBufLen = sizeof (HY_DVR_NET_EML_1);

				nRet = GetEmailCfg_1(pcfg);
			}
			break;
		case NETCFG_WLAN_IF:
			{
				LPHY_DVR_ETH_IF pcfg = (LPHY_DVR_ETH_IF) pBuffer;

				//判断buf空间是否足够
				if (*pdwBufLen < NET_CFG_LEN)
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", NET_CFG_LEN, *pdwBufLen);
					break;
				}

				memset(pcfg, 0, NET_CFG_LEN);
				*pdwBufLen = NET_CFG_LEN;

				nRet = WLAN_GetEthIF(pcfg);
			}
			break;
			
		case NETCFG_WLAN_DHCP_CONF:
			{
				LPHY_DVR_DHCP_CONF pcfg = (LPHY_DVR_DHCP_CONF) pBuffer;

				//判断buf空间是否足够
				if (*pdwBufLen < sizeof (HY_DVR_DHCP_CONF))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof (HY_DVR_DHCP_CONF), *pdwBufLen);
					break;
				}

				memset(pcfg, 0, sizeof (HY_DVR_DHCP_CONF));
				*pdwBufLen = sizeof (HY_DVR_DHCP_CONF);

				nRet = WLAN_GetDhcpCfg(pcfg);
			}
			break;
		case NETCFG_WLAN_DNS_CONF:
			{
				LPHY_DVR_DNS_CONF pcfg = (LPHY_DVR_DNS_CONF) pBuffer;

				//判断buf空间是否足够
				if (*pdwBufLen < sizeof (HY_DVR_DNS_CONF))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof (HY_DVR_DNS_CONF), *pdwBufLen);
					break;
				}

				memset(pcfg, 0, sizeof (HY_DVR_DNS_CONF));
				*pdwBufLen = sizeof (HY_DVR_DNS_CONF);

				nRet = WLAN_GetDnsCfg(pcfg);
			}
			break;
			
		case NETCFG_WLAN_ACCESSPOINTS:
			{
				LP_HY_DVR_WLAN_ACCESSPOINT_CFG pcfg = (LP_HY_DVR_WLAN_ACCESSPOINT_CFG) pBuffer;

				//判断buf空间是否足够
				if (*pdwBufLen < sizeof (HY_DVR_WLAN_ACCESSPOINT_CFG))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof (HY_DVR_WLAN_ACCESSPOINT_CFG), *pdwBufLen);
					break;
				}

				memset(pcfg, 0, sizeof (HY_DVR_WLAN_ACCESSPOINT_CFG));
				*pdwBufLen = sizeof (HY_DVR_WLAN_ACCESSPOINT_CFG);

				nRet = WLAN_GetAccessPointsCfg(pcfg);
			}
			break;
#if defined (__IS_SUPPORT_WLAN__)
        case NETCFG_WLAN_ACCESSPOINTS_V2:
            {
				LP_HY_DVR_WLAN_ACCESSPOINT_CFG_V2 pcfg = (LP_HY_DVR_WLAN_ACCESSPOINT_CFG_V2) pBuffer;

				//判断buf空间是否足够
				if (*pdwBufLen < sizeof (HY_DVR_WLAN_ACCESSPOINT_CFG_V2))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof (HY_DVR_WLAN_ACCESSPOINT_CFG_V2), *pdwBufLen);
					break;
				}
				
				memset(pcfg, 0, sizeof (HY_DVR_WLAN_ACCESSPOINT_CFG_V2));
				*pdwBufLen = sizeof (HY_DVR_WLAN_ACCESSPOINT_CFG_V2);
				
				nRet = WLAN_GetAccessPointsCfgV2(pcfg);
			}
            break;

		case NETCFG_WLAN_ACCESSPOINTS_INFO_V2:
			{
				LP_HY_DVR_WLAN_ACCESSPOINT_CFG_SSID_INFO_V2 pcfg = (LP_HY_DVR_WLAN_ACCESSPOINT_CFG_SSID_INFO_V2) pBuffer;

				//判断buf空间是否足够
				if (*pdwBufLen < sizeof (HY_DVR_WLAN_ACCESSPOINT_CFG_SSID_INFO_V2))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof (HY_DVR_WLAN_ACCESSPOINT_CFG_SSID_INFO_V2), *pdwBufLen);
					break;
				}
				
				memset(pcfg, 0, sizeof (HY_DVR_WLAN_ACCESSPOINT_CFG_SSID_INFO_V2));
				*pdwBufLen = sizeof (HY_DVR_WLAN_ACCESSPOINT_CFG_SSID_INFO_V2);
				
				nRet = WLAN_GetAccessPointsCfgV2_1(pcfg);
			}
			break;

			case NETCFG_WLAN_CONNECTION_INFO_EX:
            {
                LPHY_DVR_NETCFG_WLAN_CONNECTION_INFO_EX pcfg = (LPHY_DVR_NETCFG_WLAN_CONNECTION_INFO_EX) pBuffer;
                
                //判断buf空间是否足够
                if ( *pdwBufLen < sizeof (HY_DVR_NETCFG_WLAN_CONNECTION_INFO_EX))
                {
                    nRet = IDVR_CIU_BUF_SMALL;
                    MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof (HY_DVR_NETCFG_WLAN_CONNECTION_INFO_EX), *pdwBufLen);
                    break;
                }
                
                memset(pcfg, 0, sizeof (HY_DVR_NETCFG_WLAN_CONNECTION_INFO_EX));
                *pdwBufLen = sizeof (HY_DVR_NETCFG_WLAN_CONNECTION_INFO_EX);
                
                nRet = WLAN_GetAccessConnectInfoEx(pcfg);
            }
            break;
#endif
		case NETCFG_WLAN_CONNECTION:
			{
				LP_HY_DVR_WLAN_CONNECTION pcfg = (LP_HY_DVR_WLAN_CONNECTION) pBuffer;

				//判断buf空间是否足够
				if (*pdwBufLen < sizeof (HY_DVR_WLAN_CONNECTION))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof (HY_DVR_WLAN_CONNECTION), *pdwBufLen);
					break;
				}

				memset(pcfg, 0, sizeof (HY_DVR_WLAN_CONNECTION));
				*pdwBufLen = sizeof (HY_DVR_WLAN_CONNECTION);

				nRet = WLAN_GetConnectionCfg(pcfg);
			}
			break;
#if defined (__IS_SUPPORT_WLAN__)
		case NETCFG_WLAN_CONNECTION_INFO_V2:
			{
				LP_HY_DVR_WLAN_CONNECTION_V2 pcfg = (LP_HY_DVR_WLAN_CONNECTION_V2) pBuffer;

				//判断buf空间是否足够
				if (*pdwBufLen < sizeof (HY_DVR_WLAN_CONNECTION_V2))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof (HY_DVR_WLAN_CONNECTION_V2), *pdwBufLen);
					break;
				}

				memset(pcfg, 0, sizeof (HY_DVR_WLAN_CONNECTION_V2));
				*pdwBufLen = sizeof (HY_DVR_WLAN_CONNECTION_V2);

				nRet = WLAN_GetConnectionCfgV2(pcfg);
			}
			break;
#endif

		case NETCFG_WLAN_STATE:
			{
				LPHY_DVR_WLAN_STATE pcfg = (LPHY_DVR_WLAN_STATE) pBuffer;

				//判断buf空间是否足够
				if (*pdwBufLen < sizeof (HY_DVR_WLAN_STATE))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof (HY_DVR_WLAN_STATE), *pdwBufLen);
					break;
				}

				memset(pcfg, 0, sizeof (HY_DVR_WLAN_STATE));
				*pdwBufLen = sizeof (HY_DVR_WLAN_STATE);

				nRet = WLAN_GetStateCfg(pcfg);
			}
			break;
			
		case NETCFG_UPNP_CONF:
			{
				LPHY_DVR_UPNP_CONF pcfg = (LPHY_DVR_UPNP_CONF) pBuffer;

				//判断buf空间是否足够
				if (*pdwBufLen < sizeof (HY_DVR_UPNP_CONF))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof (HY_DVR_UPNP_CONF), *pdwBufLen);
					break;
			    }

				memset(pcfg, 0, sizeof (HY_DVR_UPNP_CONF));
				*pdwBufLen = sizeof (HY_DVR_UPNP_CONF);

				//nRet = GetUpnpChCfg(pcfg);
			}
			break;

        /* 获取FTP参数 */
        case NETCFG_FTP_CONF_V2:
            {
				LPHY_DVR_FTP_CONF_V2 pcfg = (LPHY_DVR_FTP_CONF_V2) pBuffer;

				//判断buf空间是否足够
				if (*pdwBufLen < sizeof (HY_DVR_FTP_CONF_V2))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof (HY_DVR_FTP_CONF_V2), *pdwBufLen);
					break;
			    }

				memset(pcfg, 0, sizeof (HY_DVR_FTP_CONF_V2));
				*pdwBufLen = sizeof (HY_DVR_FTP_CONF_V2);

				nRet = GetFtpUserParam(pcfg);

                printf("[Get]pcfg->bEnable:%d,\n\
                        pcfg->iFtpState:%d,\n\
                        pcfg->wFtpPort:%d,\n\
                        pcfg->szIPAddress:%s,\n\
                        pcfg->szUserName:%s,\n\
                        pcfg->szUserPassword:%s,\n\
                        pcfg->nTransferContentTypeMask:%d\n\
                        pcfg->nFtpMode=%d\n\
                        pcfg->cFtpRemoteServerStoragePath=%s\n\n",
                        pcfg->bEnable, 
                        pcfg->iFtpState, 
                        pcfg->wFtpPort, 
                        pcfg->szIPAddress,
                        pcfg->szUserName, 
                        pcfg->szUserPassword,
                        pcfg->nTransferContentTypeMask,
                        pcfg->nFtpMode,
                        pcfg->cFtpRemoteServerStoragePath);
            }
            break;

            case NETCFG_TUTK_CODEC_LEVEL_CONF:
            {
                /* 暂不支持 */
            }
            break;            

            case NETCFG_WHICH_NETCARD_USED:
            {
                LPHY_DVR_WHICH_NETCARD_USED pcfg = (LPHY_DVR_WHICH_NETCARD_USED) pBuffer;
                //判断buf空间是否足够
                if (*pdwBufLen < sizeof (HY_DVR_WHICH_NETCARD_USED))
                {
                    nRet = IDVR_CIU_BUF_SMALL;
                    MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof (HY_DVR_WHICH_NETCARD_USED), *pdwBufLen);
                    break;
                }

                memset(pcfg, 0, sizeof (HY_DVR_WHICH_NETCARD_USED));

                nRet = GetCurrNetCardType(pcfg);

//              printf("GetCurrNetCardType=>pcfg->dwCardIndex=%d\n", pcfg->dwCardIndex);
            }
            break;

        case NETCFG_WIFI_CONNECT_PARAM_CHECK_STATUS:
            {
#if defined (__IS_SUPPORT_WLAN__)
                LPHY_DVR_WLAN_STATE_FOR_TEST pcfg = (LPHY_DVR_WLAN_STATE_FOR_TEST) pBuffer;
                //判断buf空间是否足够
                if (*pdwBufLen < sizeof (HY_DVR_WLAN_STATE_FOR_TEST))
                {
                    nRet = IDVR_CIU_BUF_SMALL;
                    MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof (HY_DVR_WLAN_STATE_FOR_TEST), *pdwBufLen);
                    break;
                }

                memset(pcfg, 0, sizeof (HY_DVR_WLAN_STATE_FOR_TEST));

                MPU_DBG("---Start to called WLAN_GetTestConnStatus---\n");

                nRet = WLAN_GetTestConnStatus(pcfg);

                MPU_DBG("WLAN_GetTestConnStatus=>pcfg->bWifiStatus=%d\n", pcfg->bWifiStatus);
#endif
            }
            break;
#ifdef __MODULE_4G_SUPPORT__        
		/* 4G SIM卡的信息(只读) */
		case NETCFG_4G_NETCARD_INFO:
			{
				HY_DVR_4G_NETCARD_INFO *pcfg = (HY_DVR_4G_NETCARD_INFO *) pBuffer;
				//判断buf空间是否足够
				if (*pdwBufLen < sizeof (HY_DVR_4G_NETCARD_INFO))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof (HY_DVR_4G_NETCARD_INFO), *pdwBufLen);
					break;
				}

				memset(pcfg, 0, sizeof(HY_DVR_4G_NETCARD_INFO));
				nRet = GetCurrSimCardInfo(pcfg);
MPU_DBG("[Get] pcfg->bValid = %d,\n\
pcfg->szIMSI = %s,\n\
pcfg->szIMEI = %s,\n\
pcfg->szACT = %s,\n\
pcfg->szOperator = %s,\n\
pcfg->szBand = %s,\n\
pcfg->szICCID = %s\n",
pcfg->bValid,
pcfg->szIMSI,
pcfg->szIMEI,
pcfg->szACT,
pcfg->szOperator,
pcfg->szBand,
pcfg->szICCID);
			}
			break;

		/* 4G SIM卡当前配置信息(读写) */
		case NETCFG_4G_NETCARD_CFG:
			{
				HY_DVR_4G_NETCARD_CONFIG *pcfg = (HY_DVR_4G_NETCARD_CONFIG *) pBuffer;
				//判断buf空间是否足够
				if (*pdwBufLen < sizeof (HY_DVR_4G_NETCARD_CONFIG))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof (HY_DVR_4G_NETCARD_CONFIG), *pdwBufLen);
					break;
				}

				memset(pcfg, 0, sizeof(HY_DVR_4G_NETCARD_CONFIG));
				nRet = GetCurrSimCardCfg(pcfg);

MPU_DBG("[Get] bValid = %d, \
szAPNServer = %s, \
szAPNUserName = %s, \
szAPNPassword = %s, \
nRegisterStatus = %d\
nSignalVal = %d, \
nEnableModWander = %d, \
nSignalVal = %d\n",
pcfg->bValid,
pcfg->szAPNServer,
pcfg->szAPNUserName,
pcfg->szAPNPassword,
pcfg->nRegisterStatus,
pcfg->nSignalVal,
pcfg->nEnableModWander,
pcfg->nSignalVal);
			}
			break;
#endif   

		default:
			{
				MPU_ERR("unsupported dwMinorType = %lu\n", dwMinorType);
				nRet = IDVR_CIU_UNSUPPORTED_MINORTYPE;
			}
			break;
	}

	//MPU_INFO("ID %d, Major %d, Minor %d, return %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType, nRet);
	return nRet;
}

/* 获取拓展的网络参数(WIFI和Email时间间隔等新添加的网络参数) */
int CIU_GetConfigure_Net_EX(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen)
{
	int nRet = IDVR_OK;

	MPU_DBG("ID %d, Major %d, Minor %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType);
	switch (dwMinorType)
	{
			//获取网络适配器配置信息
		case NETCFGEX_ALL:
			{
				LPHY_DVR_NET_CFG_EX pcfg = (LPHY_DVR_NET_CFG_EX) pBuffer;

				//判断buf空间是否足够
				if (*pdwBufLen < NET_CFG_EX_LEN)
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", NET_CFG_EX_LEN, *pdwBufLen);
					break;
				}

				memset(pcfg, 0, NET_CFG_EX_LEN);
				*pdwBufLen = NET_CFG_EX_LEN;

				nRet = GetNetCfgEX(pcfg);
				MPU_DBG("=====================pcfg->dwIsSupportWifi=%d==================\n", pcfg->dwIsSupportWifi);
				MPU_DBG("===============pcfg->struEmlCfgEx.nEMLTimeInterval=%d==========\n", pcfg->struEmlCfgEx.nEMLTimeInterval);
			}
			break;

		default:
			{
				MPU_ERR("unsupported dwMinorType = %lu\n", dwMinorType);
				nRet = IDVR_CIU_UNSUPPORTED_MINORTYPE;
			}
			break;
	}

	MPU_DBG("ID %d, Major %d, Minor %d, return %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType, nRet);
	return nRet;
}

int CIU_SetMessageNotify(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, const BYTE * pBuffer, DWORD  dwBufLen)
{
	int nRet = IDVR_OK;

	MPU_DBG("ID %d, Major %d, Minor %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType);
	
	switch (dwMinorType)
	{
		case MESSAGE_NOTIFY_ALL:
			{
				///////////////////////////////////////////////////////////////////////////////
				//判断buf空间是否足够
				if (dwBufLen < sizeof(HY_DVR_MESSAGE_NOTIFY))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_MESSAGE_NOTIFY), dwBufLen);
					break;
				}

				HY_DVR_MESSAGE_NOTIFY *pcfg = (HY_DVR_MESSAGE_NOTIFY *) pBuffer;
				nRet = SetMessageNotify(pcfg);
			}
			break;

		default:
			{
				MPU_ERR("Unsupported dwMinorType = %lu\n", dwMinorType);
				nRet = IDVR_CIU_UNSUPPORTED_MINORTYPE;
			}
			break;
	}

	MPU_DBG("ID %d, Major %d, Minor %d, return %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType, nRet);
	return nRet;
}

/* 设置拓展的网络参数(WIFI和Email时间间隔等新添加的网络参数) */
int CIU_SetConfigure_Net_EX(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, const BYTE * pBuffer, DWORD dwBufLen)
{
	int nRet = IDVR_OK;

	MPU_DBG("ID %d, Major %d, Minor %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType);
	switch (dwMinorType)
	{
			//获取网络适配器配置信息
		case NETCFGEX_ALL:
			{
				LPHY_DVR_NET_CFG_EX  pcfg = (LPHY_DVR_NET_CFG_EX) pBuffer;

				//判断buf空间是否足够
				if (dwBufLen < sizeof(HY_DVR_NET_CFG_EX))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_NET_CFG_EX), dwBufLen);
					break;
				}

				nRet = SetNetCfgEX(pcfg);
			}
			break;

		default:
			{
				MPU_ERR("unsupported dwMinorType = %lu\n", dwMinorType);
				nRet = IDVR_CIU_UNSUPPORTED_MINORTYPE;
			}
			break;
	}

	MPU_DBG("ID %d, Major %d, Minor %d, return %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType, nRet);
	return nRet;
}


int CIU_SetConfigure_Net(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, const BYTE * pBuffer, DWORD dwBufLen)
{
	int nRet = IDVR_OK;
	int *ptutkStatus;
	int *pulkStatus;
	char *pUlkUid;
    int same=1;
	int byPass = 0;

	MPU_DBG("ID %d, Major %d, Minor %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType);
	switch (dwMinorType)
	{
			//设置网络适配器配置信息
		case NETCFG_ALL:
			//判断buf空间是否足够
			if (dwBufLen < NET_CFG_LEN)
			{
				nRet = IDVR_CIU_BUF_SMALL;
				MPU_ERR("buffer is too small, need %d, real %lu\n", NET_CFG_LEN, dwBufLen);
			}
#ifdef SAVE_IP_FIRST
			else
			{
				LPHY_DVR_NET_CFG pcfg = (LPHY_DVR_NET_CFG) pBuffer;

				if(pcfg->bValid && pcfg->struBasicNetCfg.bValid)
				{
					struct in_addr sInaddr_1;
					char ipaddr[DVR_MAX_IP_STR_LEN] = {0};
					int ipChanged = 0;
					int dhcpChanged = 0;
					int subMaskChanged = 0;
					char str[32] = { 0 };
					sInaddr_1.s_addr = pcfg->struBasicNetCfg.dwIPAddress;
					memset(ipaddr, 0, sizeof(ipaddr));
					strncpy(ipaddr, inet_ntop(AF_INET, &sInaddr_1, str, sizeof(str)), sizeof(ipaddr) - 1);

					saveIPfirst(ipaddr, &ipChanged);
					saveDhcpFirst(pcfg->struBasicNetCfg.dwEnableDHCP, &dhcpChanged);
					/* DVR 子网掩码 */
					sInaddr_1.s_addr = pcfg->struBasicNetCfg.dwSubnetMask;
					memset(ipaddr, 0, sizeof(ipaddr));
					strncpy(ipaddr, inet_ntop(AF_INET, &sInaddr_1, str, sizeof(str)), sizeof(ipaddr) - 1);
					saveSubMaskFirst(ipaddr, &subMaskChanged);
					if(ipChanged || dhcpChanged || subMaskChanged)
					{
						byPass = 0;
					}
					else
					{
						byPass = 1;
					}
				}
			}
#endif

			break;

			/*Dhcp配置信息 */
		case NETCFG_DHCP_CONF:
			//判断buf空间是否足够
			if (dwBufLen < sizeof (HY_DVR_DHCP_CONF))
			{
				nRet = IDVR_CIU_BUF_SMALL;
				MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof (HY_DVR_DHCP_CONF), dwBufLen);
			}
#ifdef SAVE_IP_FIRST
			else
			{
				LPHY_DVR_DHCP_CONF pcfg = (LPHY_DVR_DHCP_CONF) pBuffer;
				
				if(pcfg->bValid)
				{
					int dhcpChanged = 0;
					saveDhcpFirst(pcfg->bDhcpEnable, &dhcpChanged);
					if(dhcpChanged)
					{
						byPass = 0;
					}
					else
					{
						byPass = 1;
					}
				}
			}
#endif

			break;

			/*Dhcp状态信息          */
		case NETCFG_DHCP_STATE:
			//判断buf空间是否足够
			if (dwBufLen < sizeof (HY_DVR_DHCP_STATE))
			{
				nRet = IDVR_CIU_BUF_SMALL;
				MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof (HY_DVR_DHCP_STATE), dwBufLen);
			}
			else
			{
				nRet = IDVR_CIU_STRUCT_READONLY;
				MPU_ERR("dhcp state is read only\n");
			}
			break;

			/*Eth网络信息               */
		case NETCFG_ETH_IF:
			//判断buf空间是否足够
			if (dwBufLen < sizeof (HY_DVR_ETH_IF))
			{
				nRet = IDVR_CIU_BUF_SMALL;
				MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof (HY_DVR_ETH_IF), dwBufLen);
			}
#ifdef SAVE_IP_FIRST
			else
			{
				LPHY_DVR_ETH_IF pcfg = (LPHY_DVR_ETH_IF) pBuffer;
				if(pcfg->bValid)
				{
					int ipChanged = 0;
					saveIPfirst(pcfg->szIPAddress, &ipChanged);
					if(ipChanged)
					{
						byPass = 0;
					}
					else
					{
						byPass = 1;
					}
				}
			}
#endif

			break;
			
			/*PPPoE配置                 */
		case NETCFG_PPPOE_CONF:
			//判断buf空间是否足够
			if (dwBufLen < sizeof (HY_DVR_PPPOE_CONF))
			{
				nRet = IDVR_CIU_BUF_SMALL;
				MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof (HY_DVR_PPPOE_CONF), dwBufLen);
			}
			break;

			/*PPPoE网络信息         */
		case NETCFG_PPPOE_IF:
			//判断buf空间是否足够
			if (dwBufLen < sizeof (HY_DVR_PPPOE_IF))
			{
				nRet = IDVR_CIU_BUF_SMALL;
				MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof (HY_DVR_PPPOE_IF), dwBufLen);
			}
			else
			{
				nRet = IDVR_CIU_STRUCT_READONLY;
				MPU_ERR("pppoe net info is read only\n");
			}
			break;

			/*DNS配置                 */
		case NETCFG_DNS_CONF:
			//判断buf空间是否足够
			if (dwBufLen < sizeof (HY_DVR_DNS_CONF))
			{
				nRet = IDVR_CIU_BUF_SMALL;
				MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof (HY_DVR_DNS_CONF), dwBufLen);
			}
			break;

			/*DDNS配置                 */
		case NETCFG_DDNS_CONF:
			//判断buf空间是否足够
			if (dwBufLen < sizeof (HY_DVR_DDNS_CONF))
			{
				nRet = IDVR_CIU_BUF_SMALL;
				MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof (HY_DVR_DDNS_CONF), dwBufLen);
			}
			break;

#if 0
			/*Http配置                 */
		case NETCFG_HTTP_CONF:
			//判断buf空间是否足够
			if (dwBufLen < sizeof (ZW_SYS_NET_CFG))
			{
				nRet = IDVR_CIU_BUF_SMALL;
				MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof (ZW_SYS_NET_CFG), dwBufLen);
			}
			break;

			/*服务端口配置信息  */
		case NETCFG_LISTENPORT_CONF:
			//判断buf空间是否足够
			if (dwBufLen < ZW_NET_CFG_LEN)
			{
				nRet = IDVR_CIU_BUF_SMALL;
				MPU_ERR("buffer is too small, need %d, real %lu\n", ZW_NET_CFG_LEN, dwBufLen);
			}
			break;

			/*手机端口配置信息  */
		case NETCFG_MOBILE_CONF:
			//判断buf空间是否足够
			if (dwBufLen < ZW_NET_CFG_LEN)
			{
				nRet = IDVR_CIU_BUF_SMALL;
				MPU_ERR("buffer is too small, need %d, real %lu\n", ZW_NET_CFG_LEN, dwBufLen);
			}
			break;

			/*自动端口配置信息  */
		case NETCFG_AUTODETECT_CONF:
			//判断buf空间是否足够
			if (dwBufLen < ZW_NET_CFG_LEN)
			{
				nRet = IDVR_CIU_BUF_SMALL;
				MPU_ERR("buffer is too small, need %d, real %lu\n", ZW_NET_CFG_LEN, dwBufLen);
			}
			break;
#endif
		case NETCFG_BASIC_CONF:
			{
				//判断buf空间是否足够
				if (dwBufLen < ZW_NET_CFG_LEN)
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", ZW_NET_CFG_LEN, dwBufLen);
				} 
#ifdef SAVE_IP_FIRST
				else
				{
					LPZW_SYS_NET_CFG pcfg = (LPZW_SYS_NET_CFG) pBuffer;
					if(pcfg->bValid)
					{
						struct in_addr sInaddr_1;
						char ipaddr[DVR_MAX_IP_STR_LEN] = {0};
						char str[32] = { 0 };
						sInaddr_1.s_addr = pcfg->dwIPAddress;
						memset(ipaddr, 0, sizeof(ipaddr));
						strncpy(ipaddr, inet_ntop(AF_INET, &sInaddr_1, str, sizeof(str)), sizeof(ipaddr) - 1);
						int ipChanged = 0;
						int dhcpChanged = 0;
						int subMaskChanged = 0;
						saveIPfirst(ipaddr, &ipChanged);
						saveDhcpFirst(pcfg->dwEnableDHCP, &dhcpChanged);
						/* DVR 子网掩码 */
						sInaddr_1.s_addr = pcfg->dwSubnetMask;
						memset(ipaddr, 0, sizeof(ipaddr));
						strncpy(ipaddr, inet_ntop(AF_INET, &sInaddr_1, str, sizeof(str)), sizeof(ipaddr) - 1);
						saveSubMaskFirst(ipaddr, &subMaskChanged);
						if(ipChanged || dhcpChanged || subMaskChanged)
						{
							byPass = 0;
						}
						else
						{
							byPass = 1;
						}
					}
				}
#endif
				break;

			}
			/* EML配置信息1.1(废弃不用) */
		case NETCFG_EMAIL_CONF:
			{
				//判断buf空间是否足够
				if (dwBufLen < sizeof(HY_DVR_NET_EML_1))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_NET_EML_1), dwBufLen);
				}
				break;
			}
		case NETCFG_WLAN_IF:
			//判断buf空间是否足够
			if (dwBufLen < sizeof (HY_DVR_ETH_IF))
			{
				nRet = IDVR_CIU_BUF_SMALL;
				MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof (HY_DVR_ETH_IF), dwBufLen);
			}
			break;
		case NETCFG_WLAN_DHCP_CONF:
			//判断buf空间是否足够
			if (dwBufLen < sizeof (HY_DVR_DHCP_CONF))
			{
				nRet = IDVR_CIU_BUF_SMALL;
				MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof (HY_DVR_DHCP_CONF), dwBufLen);
			}
			break;
		case NETCFG_WLAN_DHCP_STATE:
			//判断buf空间是否足够
			if (dwBufLen < sizeof (HY_DVR_DHCP_STATE))
			{
				nRet = IDVR_CIU_BUF_SMALL;
				MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof (HY_DVR_DHCP_STATE), dwBufLen);
			}
			else
			{
				nRet = IDVR_CIU_STRUCT_READONLY;
				MPU_ERR("dhcp state is read only\n");
			}
			break;
			/*DNS配置                 */
		case NETCFG_WLAN_DNS_CONF:
			//判断buf空间是否足够
			if (dwBufLen < sizeof (HY_DVR_DNS_CONF))
			{
				nRet = IDVR_CIU_BUF_SMALL;
				MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof (HY_DVR_DNS_CONF), dwBufLen);
			}
			break;
		case NETCFG_WLAN_CONNECTION:
			//判断buf空间是否足够
			if (dwBufLen < sizeof (HY_DVR_WLAN_CONNECTION))
			{
				nRet = IDVR_CIU_BUF_SMALL;
				MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof (HY_DVR_WLAN_CONNECTION), dwBufLen);
			}
		#if defined (__IS_SUPPORT_WLAN__)
			WLAN_SetConnectionCfgEx((LP_HY_DVR_WLAN_CONNECTION) pBuffer);
		#endif
			break;
#if defined (__IS_SUPPORT_WLAN__)		
		case NETCFG_WLAN_CONNECTION_INFO_V2:
			//判断buf空间是否足够
			if (dwBufLen < sizeof (HY_DVR_WLAN_CONNECTION_V2))
			{
				nRet = IDVR_CIU_BUF_SMALL;
				MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof (HY_DVR_WLAN_CONNECTION_V2), dwBufLen);
			}
			WLAN_SetConnectionCfgExV2((LP_HY_DVR_WLAN_CONNECTION_V2) pBuffer);
		
			break;
		case NETCFG_WIFI_CONNECT_PARAM_CHECK:
			//判断buf空间是否足够
			if (dwBufLen < sizeof (HY_DVR_WLAN_CONNECTION))
			{
				nRet = IDVR_CIU_BUF_SMALL;
				MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof (HY_DVR_DHCP_CONF), dwBufLen);
			}
			break;
#endif

		case NETCFG_UPNP_CONF:
			//判断buf空间是否足够
			if (dwBufLen < sizeof (HY_DVR_UPNP_CONF))
			{
				nRet = IDVR_CIU_BUF_SMALL;
				MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof (HY_DVR_UPNP_CONF), dwBufLen);
			}

			break;
		case NETCFG_WHO_PROBE_CONF:
			//判断buf空间是否足够
			if (dwBufLen < sizeof (HY_DVR_WHOPROBE_CONF))
			{
				nRet = IDVR_CIU_BUF_SMALL;
				MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof (HY_DVR_WHOPROBE_CONF), dwBufLen);
			}
			//判断IP地址是否出于同网段，避免无谓的系统开销
			nRet = GetIfNeedAdjustIpAddress((LPHY_DVR_WHOPROBE_CONF)pBuffer, &same);
			if((0 == nRet) && (0 == same))//可以自适应
			{
				MPU_DBG("Need do ip adjust!\n");
			}
			else
			{
				return 0;
			}
				
			break;
		case NETCFG_NOTIFY_TUTK_STATE:
			//判断buf空间是否足够
			if (dwBufLen < sizeof (int))
			{
				nRet = IDVR_CIU_BUF_SMALL;
				MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof (HY_DVR_UPNP_CONF), dwBufLen);
			}
			ptutkStatus = (int *)pBuffer;
			nRet = SetTutkStatus(ptutkStatus);

			break;
		/* ULK状态 */
		case NETCFG_NOTIFY_ULUCO_P2P_STATE:   //0x1233: 4659
			//判断buf空间是否足够
			if (dwBufLen < sizeof (int))
			{
				nRet = IDVR_CIU_BUF_SMALL;
				MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof (HY_DVR_UPNP_CONF), dwBufLen);
			}
			pulkStatus = (int *)pBuffer;
			nRet = SetUlkStatus(pulkStatus);
		
			break;
		case NETCFG_NOTIFY_ULUCO_P2P_UID:	 //0x1232: 4658
			pUlkUid = (char *)pBuffer;
			nRet = SetUlkUid(pUlkUid);
		
//			printf("[%s:%d]ULK UID = %s\n",  __FUNCTION__, __LINE__, pUlkUid);
			
			break;

		case NETCFG_FTP_CONF_V2:
            //判断buf空间是否足够
            if (dwBufLen < sizeof (LPHY_DVR_FTP_CONF_V2))
            {
                nRet = IDVR_CIU_BUF_SMALL;
                MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof (LPHY_DVR_FTP_CONF_V2), dwBufLen);
            }
            break;
			
#if defined (__AUTH_V2__)
		case NETCFG_P2P_CONNECT_INFO:
			//判断buf空间是否足够
			if (dwBufLen < sizeof(HY_P2P_CONNECT_INFO))
			{
				nRet = IDVR_CIU_BUF_SMALL;
				MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_P2P_CONNECT_INFO), dwBufLen);
			}
			else
			{
				HY_P2P_CONNECT_INFO *pcfg = (HY_P2P_CONNECT_INFO *) pBuffer;
				if(pcfg->nValid)
				{
					SetConnectInfo(pcfg);
				}
			}
#endif
#ifdef __MODULE_4G_SUPPORT__
		case NETCFG_4G_NETCARD_INFO:
			{
				HY_DVR_4G_NETCARD_INFO *pcfg = (HY_DVR_4G_NETCARD_INFO *) pBuffer;
				//判断buf空间是否足够
				if (dwBufLen != sizeof (HY_DVR_4G_NETCARD_INFO))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof (HY_DVR_4G_NETCARD_INFO), dwBufLen);
					break;
				}
				dwBufLen = sizeof (HY_DVR_4G_NETCARD_INFO);

				nRet = SetCurrSimCardInfo(pcfg);
			}
			break;

		/* 4G SIM卡当前配置信息(读写) */
		case NETCFG_4G_NETCARD_CFG:
			{
				HY_DVR_4G_NETCARD_CONFIG *pcfg = (HY_DVR_4G_NETCARD_CONFIG *) pBuffer;
				//判断buf空间是否足够
				if (dwBufLen != sizeof (HY_DVR_4G_NETCARD_CONFIG))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof (HY_DVR_4G_NETCARD_CONFIG), dwBufLen);
					break;
				}
				dwBufLen = sizeof (HY_DVR_4G_NETCARD_CONFIG);

				nRet = SetCurrSimCardCfg(pcfg);
			}
			break;
#endif

		default:
			{
				nRet = IDVR_CIU_UNSUPPORTED_MINORTYPE;
				MPU_ERR("unsupported dwMinorType = %lu\n", dwMinorType);
			}
			break;
	}

	if((NETCFG_NOTIFY_TUTK_STATE == dwMinorType) || (NETCFG_NOTIFY_ULUCO_P2P_STATE == dwMinorType)
		 || (NETCFG_NOTIFY_ULUCO_P2P_UID == dwMinorType))
		return nRet;

#ifdef SAVE_IP_FIRST	
		//除了改IP,MASK,DHCP的其它网络参数直接设置
		if(byPass)
		{
			MPU_DBG("**********************Set net bypass!\n");
			nRet = SetConfigure_Net(dwUserID, dwMajorType, dwMinorType, pBuffer, dwBufLen);
			MPU_DBG("ID %d, Major %d, Minor %d, return %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType, nRet);
			return nRet;
		}
#endif
	if (IDVR_OK == nRet)
	{
		//需要异步执行命令
		LPNETWORK_CONFIG pcfg = new NETWORK_CONFIG;

		if (NULL != pcfg)
		{
            if (NETCFG_WIFI_CONNECT_PARAM_CHECK == dwMinorType)
            {
                nRet = SetConfigure_Net(dwUserID, dwMajorType, dwMinorType, pBuffer, dwBufLen);
            }
		    else
		    {
    			nRet = pcfg->set(dwUserID, dwMajorType, dwMinorType, pBuffer, dwBufLen);
            }
			if (IDVR_OK != nRet)
			{
				MPU_ERR("pcfg->set() fail, return %d\n", nRet);
				delete pcfg;
				pcfg = NULL;
			}
		}
		else
		{
			nRet = IDVR_ERROR_ALLOC;
			MPU_ERR("new NETWORK_CONFIG fail\n");
		}
	}

	MPU_DBG("ID %d, Major %d, Minor %d, return %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType, nRet);
	return nRet;
}

//!\brief   远程配置，获取网络服务器参数
int CIU_GetConfigure_Server(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen)
{
	int nRet = IDVR_OK;

	MPU_DBG("ID %d, Major %d, Minor %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType);
	switch (dwMinorType)
	{
			//获取网络管理服务器信息
		case NETSERVERCFG_ALL:
			{
				LPHY_DVR_NET_MANAGER pcfg = (LPHY_DVR_NET_MANAGER) pBuffer;

				//判断buf空间是否足够
				if (*pdwBufLen < NET_MANAGER_LEN)
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", NET_MANAGER_LEN, *pdwBufLen);
					break;
				}

				memset(pcfg, 0, NET_MANAGER_LEN);
				*pdwBufLen = NET_MANAGER_LEN;

				nRet = GetNetManager(pcfg);
			}
			break;

			/*CMS配置信息               */
		case NETSERVERCFG_CMS_CONF:
			{
				LPHY_DVR_NET_CMS pcfg = (LPHY_DVR_NET_CMS) pBuffer;

				//判断buf空间是否足够
				if (*pdwBufLen < sizeof (HY_DVR_NET_CMS))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof (HY_DVR_NET_CMS), *pdwBufLen);
					break;
				}

				memset(pcfg, 0, sizeof (HY_DVR_NET_CMS));
				*pdwBufLen = sizeof (HY_DVR_NET_CMS);

				nRet = GetCmsCfg(pcfg);
			}
			break;

			/*CMS状态信息               */
		case NETSERVERCFG_CMS_STATE:
			{
				LPHY_DVR_CMS_STATE pcfg = (LPHY_DVR_CMS_STATE) pBuffer;

				//判断buf空间是否足够
				if (*pdwBufLen < sizeof (HY_DVR_CMS_STATE))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof (HY_DVR_CMS_STATE), *pdwBufLen);
					break;
				}

				memset(pcfg, 0, sizeof (HY_DVR_CMS_STATE));
				*pdwBufLen = sizeof (HY_DVR_CMS_STATE);

				nRet = GetCmsState(pcfg);
			}
			break;

			/*AMS配置信息               */
		case NETSERVERCFG_AMS_CONF:
			{
				LPHY_DVR_NET_AMS pcfg = (LPHY_DVR_NET_AMS) pBuffer;

				//判断buf空间是否足够
				if (*pdwBufLen < sizeof (HY_DVR_NET_AMS))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof (HY_DVR_NET_AMS), *pdwBufLen);
					break;
				}

				memset(pcfg, 0, sizeof (HY_DVR_NET_AMS));
				*pdwBufLen = sizeof (HY_DVR_NET_AMS);

				nRet = GetAmsCfg(pcfg);
			}
			break;

			/*NTP配置信息               */
		case NETSERVERCFG_NTP_CONF:
			{
				LPHY_DVR_NET_NTP pcfg = (LPHY_DVR_NET_NTP) pBuffer;

				//判断buf空间是否足够
				if (*pdwBufLen < sizeof (HY_DVR_NET_NTP))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof (HY_DVR_NET_NTP), *pdwBufLen);
					break;
				}

				memset(pcfg, 0, sizeof (HY_DVR_NET_NTP));
				*pdwBufLen = sizeof (HY_DVR_NET_NTP);

				nRet = GetNtpCfg(pcfg);
			}
			break;
#if 0
			/*smtp配置信息               */
		case NETSERVERCFG_EML_CONF:
			{
				LPHY_DVR_NET_EML pcfg = (LPHY_DVR_NET_EML) pBuffer;

				//判断buf空间是否足够
				if (*pdwBufLen < sizeof (HY_DVR_NET_EML))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof (HY_DVR_NET_EML), *pdwBufLen);
					break;
				}

				memset(pcfg, 0, sizeof (HY_DVR_NET_EML));
				*pdwBufLen = sizeof (HY_DVR_NET_EML);

				nRet = GetEmailCfg(pcfg);
			}
			break;

			/* smtp配置信息1.1		 */
		case NETSERVERCFG_EML_1_CONF:
			{
				LPHY_DVR_NET_EML_1 pcfg = (LPHY_DVR_NET_EML_1) pBuffer;

				//判断buf空间是否足够
				if (*pdwBufLen < sizeof (HY_DVR_NET_EML_1))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof (HY_DVR_NET_EML_1), *pdwBufLen);
					break;
				}

				memset(pcfg, 0, sizeof (HY_DVR_NET_EML_1));
				*pdwBufLen = sizeof (HY_DVR_NET_EML_1);

				nRet = GetEmailCfg_1(pcfg);
			}
			break;
#endif
			/*CMS协议信息 */
		case NETSERVERCFG_CMS_PROTOCOL:
			{
				LPHY_DVR_CMS_FOR_PROTOCOL pcfg = (LPHY_DVR_CMS_FOR_PROTOCOL) pBuffer;

				//判断buf空间是否足够
				if (*pdwBufLen < sizeof (HY_DVR_CMS_FOR_PROTOCOL))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof (HY_DVR_CMS_FOR_PROTOCOL), *pdwBufLen);
					break;
				}

				memset(pcfg, 0, sizeof (HY_DVR_CMS_FOR_PROTOCOL));
				*pdwBufLen = sizeof (HY_DVR_CMS_FOR_PROTOCOL);

				nRet = GetCmsProtocol(pcfg);
			}
			break;

		default	:
			{
				MPU_ERR("unsupported dwMinorType = %lu\n", dwMinorType);
				nRet = IDVR_CIU_UNSUPPORTED_MINORTYPE;
			}
			break;
	}

	MPU_DBG("ID %d, Major %d, Minor %d, return %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType, nRet);
	return nRet;

}

//!\brief   远程配置，设置网络服务器参数
int CIU_SetConfigure_Server(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, const BYTE * pBuffer, DWORD dwBufLen)
{
	int nRet = IDVR_OK;

	MPU_DBG("ID %d, Major %d, Minor %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType);
	switch (dwMinorType)
	{
			//设置网络管理服务器信息
		case NETSERVERCFG_ALL:
			{
				LPHY_DVR_NET_MANAGER pcfg = (LPHY_DVR_NET_MANAGER) pBuffer;

				//判断buf空间是否足够
				if (dwBufLen < NET_MANAGER_LEN)
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", NET_MANAGER_LEN, dwBufLen);
					break;
				}

				nRet = SetNetManager(pcfg);
			}
			break;

			/*CMS配置信息               */
		case NETSERVERCFG_CMS_CONF:
			{
				LPHY_DVR_NET_CMS pcfg = (LPHY_DVR_NET_CMS) pBuffer;

				//判断buf空间是否足够
				if (dwBufLen < sizeof (HY_DVR_NET_CMS))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof (HY_DVR_NET_CMS), dwBufLen);
					break;
				}

				nRet = SetCmsCfg(pcfg);
			}
			break;

			/*CMS状态信息               */
		case NETSERVERCFG_CMS_STATE:
			{
				//LPHY_DVR_CMS_STATE pcfg = (LPHY_DVR_CMS_STATE) pBuffer;

				//判断buf空间是否足够
				if (dwBufLen < sizeof (HY_DVR_CMS_STATE))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof (HY_DVR_CMS_STATE), dwBufLen);
					break;
				}

				MPU_ERR("center manager server state is read only\n");
				nRet = IDVR_CIU_STRUCT_READONLY;
			}
			break;

			/*AMS配置信息               */
		case NETSERVERCFG_AMS_CONF:
			{
				LPHY_DVR_NET_AMS pcfg = (LPHY_DVR_NET_AMS) pBuffer;

				//判断buf空间是否足够
				if (dwBufLen < sizeof (HY_DVR_NET_AMS))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof (HY_DVR_NET_AMS), dwBufLen);
					break;
				}

				nRet = SetAmsCfg(pcfg);
			}
			break;

			/*NTP配置信息               */
		case NETSERVERCFG_NTP_CONF:
			{
				LPHY_DVR_NET_NTP pcfg = (LPHY_DVR_NET_NTP) pBuffer;

				//判断buf空间是否足够
				if (dwBufLen < sizeof (HY_DVR_NET_NTP))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof (HY_DVR_NET_NTP), dwBufLen);
					break;
				}

				nRet = SetNtpCfg(pcfg);
			}
			break;
#if 0
			/*EML配置信息               */
		case NETSERVERCFG_EML_CONF:
			{
				LPHY_DVR_NET_EML pcfg = (LPHY_DVR_NET_EML) pBuffer;

				//判断buf空间是否足够
				if (dwBufLen < sizeof (HY_DVR_NET_EML))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof (HY_DVR_NET_EML), dwBufLen);
					break;
				}

				nRet = SetEmailCfg(pcfg);
			}
			break;

			/* EML配置信息1.1 */
		case NETSERVERCFG_EML_1_CONF:
			{
				LPHY_DVR_NET_EML_1 pcfg = (LPHY_DVR_NET_EML_1) pBuffer;

				//判断buf空间是否足够
				if (dwBufLen < sizeof (HY_DVR_NET_EML_1))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof (HY_DVR_NET_EML_1), dwBufLen);
					break;
				}

				nRet = SetEmailCfg_1(pcfg);
			}
			break;
#endif
			/*CMS协议信息 */
		case NETSERVERCFG_CMS_PROTOCOL:
			{
				LPHY_DVR_CMS_FOR_PROTOCOL pcfg = (LPHY_DVR_CMS_FOR_PROTOCOL) pBuffer;

				//判断buf空间是否足够
				if (dwBufLen < sizeof (HY_DVR_CMS_FOR_PROTOCOL))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof (HY_DVR_CMS_FOR_PROTOCOL), dwBufLen);
					break;
				}

				nRet = SetCmsProtocol(pcfg);
			}
			break;

		default:
			{
				nRet = IDVR_CIU_UNSUPPORTED_MINORTYPE;
				MPU_ERR("unsupported dwMinorType = %lu\n", dwMinorType);
			}
			break;
	}

	if (IDVR_OK == nRet)
	{
		//记录日志，设置服务器参数
		CIU_OPL_AddLog(dwUserID, EDVR_EVENT_SETTING, EDVR_EVENT_SETTING_DETAIL_SERVER_SETTING, 0, 0);
	}

	MPU_DBG("ID %d, Major %d, Minor %d, return %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType, nRet);
	return nRet;
}

//!\brief   远程配置，获取互信互通參數
int CIU_GetConfigure_Hxht(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen)
{
	int nRet = IDVR_OK;

	MPU_DBG("ID %d, Major %d, Minor %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType);

	switch (dwMinorType)
	{
			/* 全部HXHT配置 */
		case HXHT_ALL:
			{
				LPHY_DVR_HXHT_CFG pcfg = (LPHY_DVR_HXHT_CFG) pBuffer;

				//判断buf空间是否足够
				if (*pdwBufLen < sizeof (HY_DVR_HXHT_CFG))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof (HY_DVR_HXHT_CFG), *pdwBufLen);
					break;
				}

				memset(pcfg, 0, sizeof (HY_DVR_HXHT_CFG));
				*pdwBufLen = sizeof (HY_DVR_HXHT_CFG);

				nRet = GetHxhtCfg(pcfg);
			}
			break;

			/* NAT状态配置    */
		case HXHT_NATSTATUS:
			{
				LPHY_DVR_NATSTATUS pcfg = (LPHY_DVR_NATSTATUS) pBuffer;

				//判断buf空间是否足够
				if (*pdwBufLen < sizeof (HY_DVR_NATSTATUS))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof (HY_DVR_NATSTATUS), *pdwBufLen);
					break;
				}

				memset(pcfg, 0, sizeof (HY_DVR_NATSTATUS));
				*pdwBufLen = sizeof (HY_DVR_NATSTATUS);

				nRet = GetNatStatus(pcfg);
			}
			break;

			/* 语音呼叫配置 */
		case HXHT_VOICEPARAM:
			{
				LPHY_DVR_VOICEPARAM pcfg = (LPHY_DVR_VOICEPARAM) pBuffer;

				//判断buf空间是否足够
				if (*pdwBufLen < sizeof (HY_DVR_VOICEPARAM))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof (HY_DVR_VOICEPARAM), *pdwBufLen);
					break;
				}

				memset(pcfg, 0, sizeof (HY_DVR_VOICEPARAM));
				*pdwBufLen = sizeof (HY_DVR_VOICEPARAM);

				nRet = GetVoiceParam(pcfg);
			}
			break;

			/*CMS存储注册配置信息       */
		case HXHT_CMS_STORAGE:
			{
				LPHY_DVR_NET_CMS_STOREAGE pcfg = (LPHY_DVR_NET_CMS_STOREAGE) pBuffer;

				//判断buf空间是否足够
				if (*pdwBufLen < sizeof (HY_DVR_NET_CMS_STOREAGE))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof (HY_DVR_NET_CMS_STOREAGE), *pdwBufLen);
					break;
				}

				memset(pcfg, 0, sizeof (HY_DVR_NET_CMS_STOREAGE));
				*pdwBufLen = sizeof (HY_DVR_NET_CMS_STOREAGE);

				nRet = GetCmsStorage(pcfg);
			}
			break;

			/* 前端设备ID */
		case HXHT_DEVICE_ID:
			{
				LPHY_DVR_DEVICE_ID pcfg = (LPHY_DVR_DEVICE_ID) pBuffer;

				//判断buf空间是否足够
				if (*pdwBufLen < sizeof (HY_DVR_DEVICE_ID))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof (HY_DVR_DEVICE_ID), *pdwBufLen);
					break;
				}

				memset(pcfg, 0, sizeof (HY_DVR_DEVICE_ID));
				*pdwBufLen = sizeof (HY_DVR_DEVICE_ID);

				nRet = GetDeviceID(pcfg);
			}
			break;

		default:
			{
				MPU_ERR("unsupported dwMinorType = %lu\n", dwMinorType);
				nRet = IDVR_CIU_UNSUPPORTED_MINORTYPE;
			}
			break;
	}

	MPU_DBG("ID %d, Major %d, Minor %d, return %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType, nRet);
	return nRet;
}

//!\brief   远程配置，设置互信互通參數
int CIU_SetConfigure_Hxht(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, const BYTE * pBuffer, DWORD dwBufLen)
{
	int nRet = IDVR_OK;

	MPU_DBG("ID %d, Major %d, Minor %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType);

	switch (dwMinorType)
	{
			/* 全部HXHT配置 */
		case HXHT_ALL:
			{
				LPHY_DVR_HXHT_CFG pcfg = (LPHY_DVR_HXHT_CFG) pBuffer;

				//判断buf空间是否足够
				if (dwBufLen < sizeof (HY_DVR_HXHT_CFG))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof (HY_DVR_HXHT_CFG), dwBufLen);
					break;
				}

				nRet = SetHxhtCfg(pcfg);
			}
			break;

			/* NAT状态配置    */
		case HXHT_NATSTATUS:
			{
				LPHY_DVR_NATSTATUS pcfg = (LPHY_DVR_NATSTATUS) pBuffer;

				//判断buf空间是否足够
				if (dwBufLen < sizeof (HY_DVR_NATSTATUS))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof (HY_DVR_NATSTATUS), dwBufLen);
					break;
				}

				nRet = SetNatStatus(pcfg);
			}
			break;

			/* 语音呼叫配置 */
		case HXHT_VOICEPARAM:
			{
				//只读参数
				nRet = IDVR_CIU_STRUCT_READONLY;
			}
			break;

			/*CMS存储注册配置信息       */
		case HXHT_CMS_STORAGE:
			{
				LPHY_DVR_NET_CMS_STOREAGE pcfg = (LPHY_DVR_NET_CMS_STOREAGE) pBuffer;

				//判断buf空间是否足够
				if (dwBufLen < sizeof (HY_DVR_NET_CMS_STOREAGE))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof (HY_DVR_NET_CMS_STOREAGE), dwBufLen);
					break;
				}

				nRet = SetCmsStorage(pcfg);
			}
			break;

			/* 前端设备ID */
		case HXHT_DEVICE_ID:
			{
				LPHY_DVR_DEVICE_ID pcfg = (LPHY_DVR_DEVICE_ID) pBuffer;

				//判断buf空间是否足够
				if (dwBufLen < sizeof (HY_DVR_DEVICE_ID))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof (HY_DVR_DEVICE_ID), dwBufLen);
					break;
				}

				nRet = SetDeviceID(pcfg);
			}
			break;

		default:
			{
				nRet = IDVR_CIU_UNSUPPORTED_MINORTYPE;
				MPU_ERR("unsupported dwMinorType = %lu\n", dwMinorType);
			}
			break;
	}

	MPU_DBG("ID %d, Major %d, Minor %d, return %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType, nRet);
	return nRet;
}

//!\brief   远程配置，获取中心管理服务器通讯协议
int CIU_GetConfigure_Protocol(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen)
{
	int nRet = IDVR_OK;

	static int bhxht = -1;

	MPU_DBG("ID %d, Major %d, Minor %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType);

	switch (dwMinorType)
	{
			//获取中心管理服务器通讯协议
		case PROTOCOL_ALL:
			{
				LPHY_DVR_PROTOCOL_NAME pcfg = (LPHY_DVR_PROTOCOL_NAME) pBuffer;

				//判断buf空间是否足够
				if (*pdwBufLen < sizeof (HY_DVR_PROTOCOL_NAME))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof (HY_DVR_PROTOCOL_NAME), *pdwBufLen);
					break;
				}

				if (bhxht < 0)
				{
					bhxht = 0;

					//读取互信互通配置
					FILE *fp = fopen(NMS_CONFIG_FILE, "r");

					if (NULL != fp)
					{
						char fbuf[32] = { 0 };
						fread(fbuf, sizeof(char), 1, fp);
						sscanf(fbuf, "%d", &bhxht);
						// fscanf(fp, "%d", &bhxht);
						fclose(fp);
					}
				}

				memset(pcfg, 0, sizeof (HY_DVR_PROTOCOL_NAME));
				*pdwBufLen = sizeof (HY_DVR_PROTOCOL_NAME);

				//! 本结构体是否有效，=0表示无效
				pcfg->bValid = 1;

				/*!< 协议名称：协议是按位设置和读取的，例如只有手机协议时，该值为1；
				   只有互信互通协议时，该值为2；两个协议都有时，该值为3。
				   （私有协议是必须加载的，不在其中） */
#ifdef EGUI_FOR_ICAMERA_NXP8850_BASED_DVR
				if (1 == bhxht)
				{
					pcfg->dwProtocolName = PROTOCOL_PRIVATE | PROTOCOL_HXHT | PROTOCOL_TIDEMOBILE;
				}
				else if (2 == bhxht)
				{
					pcfg->dwProtocolName = PROTOCOL_PRIVATE | PROTOCOL_RTSP | PROTOCOL_TIDEMOBILE;
				}
				else
				{
					pcfg->dwProtocolName = PROTOCOL_PRIVATE | PROTOCOL_TIDEMOBILE;
				}
#else

				if (bhxht)
				{
					pcfg->dwProtocolName = PROTOCOL_PRIVATE | PROTOCOL_TIDEMOBILE | PROTOCOL_HXHT;
				}
				else
				{
					pcfg->dwProtocolName = PROTOCOL_PRIVATE | PROTOCOL_TIDEMOBILE;
				}
#endif
			}
			break;

		default:
			{
				MPU_ERR("unsupported dwMinorType = %lu\n", dwMinorType);
				nRet = IDVR_CIU_UNSUPPORTED_MINORTYPE;
			}
			break;
	}

	MPU_DBG("ID %d, Major %d, Minor %d, return %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType, nRet);
	return nRet;
}
/* 网络修改标记，由于网络修改了，这时候设备重启了，设置值并未完成,此处做一个简单的同步 */
int NetChFlag = 0;

static void *network_config_thread(void *param)
{
    PRT_TID();

	BEGINRUN_THREAD;
	//MPU_DBG("enter\n");

	LPNETWORK_CONFIG pcfg = (LPNETWORK_CONFIG) param;

	if (NULL != pcfg)
	{
		//休眠3秒
		sleep(1);
		int id,count=0;;
		while(1)
		{
			pthread_mutex_lock(&netWorkThreadLock);
			id = netThreadWorkingID;
			pthread_mutex_unlock(&netWorkThreadLock);
			if((pcfg->threadId <= id) || (count > 750))
				break;
			count ++;
			usleep(40000);
		}
		printf("------------network_config_thread--------threadId=%ld\n", pcfg->threadId);

		NetChFlag++; 
		/* 真正设置网络参数函数接口 */
		SetConfigure_Net(pcfg->m_dwUserID, pcfg->m_dwMajorType, pcfg->m_dwMinorType, pcfg->m_pBuffer, pcfg->m_dwBufLen);
		NetChFlag--;

		delete pcfg;

		pcfg = NULL;
	}
	else
	{
		MPU_ERR("NULL == pcfg\n");
	}

	pthread_mutex_lock(&netWorkThreadLock);
	netThreadWorkingID ++;
	pthread_mutex_unlock(&netWorkThreadLock);

	//MPU_DBG("ok\n");
	ENDRUN_THREAD;
	return 0;
}

#if defined (__NET_PROTOCOL_CONTROL__)
int CIU_SetConfigureOnOff_Protocol(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, const BYTE * pBuffer, DWORD  dwBufLen, int *nNeedReboot)
{
	int nRet = IDVR_OK;
	MPU_INFO("ID %d, Major %d, Minor %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType);
	switch (dwMinorType)
	{
		//设置第三方协议开关
		case THIRD_PARTY_PROTOCOL_CONFIG:
			{
				///////////////////////////////////////////////////////////////////////////////
				//判断buf空间是否足够
				if (dwBufLen < sizeof(HY_DVR_THIRD_PROTOCOL_MANAGE))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_THIRD_PROTOCOL_MANAGE), dwBufLen);
					break;
				}

				HY_DVR_THIRD_PROTOCOL_MANAGE *pcfg = (HY_DVR_THIRD_PROTOCOL_MANAGE *) pBuffer;
				nRet = SetProtocolOnOff(pcfg, nNeedReboot);
			}
			break;
        
		default:
			{
				MPU_ERR("Unsupported dwMinorType = %lu\n", dwMinorType);
				nRet = IDVR_CIU_UNSUPPORTED_MINORTYPE;
			}
			break;
	}

	MPU_INFO("ID %d, Major %d, Minor %d, return %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType, nRet);
	return nRet;
}

/* 获取第三方协议开关 */
int CIU_GetConfigureOnOff_Protocol(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen)
{
	int nRet = IDVR_OK;

	MPU_DBG("ID %d, Major %d, Minor %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType);
	switch (dwMinorType)
	{
			//获取第三方协议开关
		case THIRD_PARTY_PROTOCOL_CONFIG:
			{
				LPHY_DVR_THIRD_PROTOCOL_MANAGE pcfg = (LPHY_DVR_THIRD_PROTOCOL_MANAGE) pBuffer;

				//判断buf空间是否足够
				if (*pdwBufLen < sizeof(HY_DVR_THIRD_PROTOCOL_MANAGE))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_THIRD_PROTOCOL_MANAGE), *pdwBufLen);
					break;
				}

				memset(pcfg, 0, sizeof(HY_DVR_THIRD_PROTOCOL_MANAGE));
				*pdwBufLen = sizeof(HY_DVR_THIRD_PROTOCOL_MANAGE);

				nRet = GetProtocolOnOff(pcfg);
			}
			break;

		default:
			{
				MPU_ERR("unsupported dwMinorType = %lu\n", dwMinorType);
				nRet = IDVR_CIU_UNSUPPORTED_MINORTYPE;
			}
			break;
	}

	MPU_DBG("ID %d, Major %d, Minor %d, return %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType, nRet);
	return nRet;
}
#endif

#if defined (__USE_APPLY_DID_MANUAL__)
static int SetApplyDIDRequestUrl(HY_DVR_APPLY_DID_MANUL_CFG *pcfg)
{
	int nSDKRet = IDVR_OK;

	//! 本结构体是否有效，=0表示无效
	pcfg->nValid = 1;
	if (0 == pcfg->nValid || NULL == pcfg->acRequestUrl)
	{
		MPU_DBG("0 == pcfg->nValid\n");
		return IDVR_CIU_STRUCT_UNVALID;
	}
	MPU_DBG("*** --SetApplyDIDRequestUrl --url %s------\n", pcfg->acRequestUrl);
	
	nSDKRet = MPU_NET_SetApplyDIDRequestUrl(pcfg->acRequestUrl);
	MPU_CHK(0 == nSDKRet);

	return (0 == nSDKRet) ? IDVR_OK : IDVR_CIU_CALLSDK_FAIL;
}

int CIU_SetConfigure_Net_DID(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, const BYTE * pBuffer, DWORD dwBufLen)
{
	int nRet = IDVR_OK;

	switch (dwMinorType)
	{
        case HY_DVR_APPLY_DID_MANUL:
        {
            HY_DVR_APPLY_DID_MANUL_CFG *pcfg = (HY_DVR_APPLY_DID_MANUL_CFG *)pBuffer;
            
            //判断buf空间是否足够
            if (dwBufLen < sizeof(HY_DVR_APPLY_DID_MANUL_CFG))
            {
                nRet = IDVR_CIU_BUF_SMALL;
                MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_APPLY_DID_MANUL_CFG), dwBufLen);
                break;
            } 

            nRet = SetApplyDIDRequestUrl(pcfg);
        }
        break;
        
        default:
        {
            MPU_ERR("unsupported dwMinorType = %lu\n", dwMinorType);
            nRet = IDVR_CIU_UNSUPPORTED_MINORTYPE;
        }
        break;
    }
           
    return nRet;
}
#endif

#if defined (__IS_SUPPORT_P6SMESSAGE__)
int GetMpServerParamCfg(HY_DVR_MP_SERVER_URL_INFO *pcfg)
{
	int nRet = 0;

	pcfg->nValid = 1;
	nRet = MPU_NET_GetMpServerParamCfg(pcfg->strServerIpAndPort, pcfg->strUrlPathTimeCheck, pcfg->strUrlPathEventPolicy, pcfg->strUrlPathEventPost, pcfg->strUrlPathSetXmlStatus, pcfg->strSrvMaintenance);

	return nRet;
}

int CIU_GetConfigure_P6S_MP_SERVER_PARAM(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen)
{
	int nRet = IDVR_OK;

	MPU_DBG("ID %d, Major %d, Minor %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType);

	switch (dwMinorType)
	{
		case HY_DVR_MP_SERVER_URL:
		{
			HY_DVR_MP_SERVER_URL_INFO *pcfg = (HY_DVR_MP_SERVER_URL_INFO*) pBuffer;

			//判断buf空间是否足够
			if (*pdwBufLen < sizeof(HY_DVR_MP_SERVER_URL_INFO))
			{
				nRet = IDVR_CIU_BUF_SMALL;
				MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_MP_SERVER_URL_INFO), *pdwBufLen);
				break;
			}

			memset(pcfg, 0, sizeof(HY_DVR_MP_SERVER_URL_INFO));
			*pdwBufLen = sizeof(HY_DVR_MP_SERVER_URL_INFO);

			nRet = GetMpServerParamCfg(pcfg);
			break;
		}
		default:
		{
			MPU_ERR("unsupported dwMinorType = %lu\n", dwMinorType);
			nRet = IDVR_CIU_UNSUPPORTED_MINORTYPE;
			break;
		}
	}

	MPU_DBG("ID %d, Major %d, Minor %d, return %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType, nRet);
	return nRet;
}

int SetMpServerParamCfg(HY_DVR_MP_SERVER_URL_INFO *pcfg)
{
	int nRet = 0;
	
	//本结构体是否有效，=0表示无效
	if (0 == pcfg->nValid)
	{
		MPU_DBG("0 == pcfg->bValid\n");
		return IDVR_CIU_STRUCT_UNVALID;
	}
	
	nRet = MPU_NET_SetMpServerParamCfg(pcfg->strServerIpAndPort, pcfg->strUrlPathTimeCheck, pcfg->strUrlPathEventPolicy, pcfg->strUrlPathEventPost, pcfg->strUrlPathSetXmlStatus, pcfg->strSrvMaintenance);
	return nRet;
}

int CIU_SetConfigure_P6S_MP_SERVER_PARAM(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, const BYTE * pBuffer, DWORD dwBufLen)
{
	int nRet = IDVR_OK;

	MPU_DBG("ID %d, Major %d, Minor %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType);

	switch (dwMinorType)
	{
		case HY_DVR_MP_SERVER_URL:
		{
			HY_DVR_MP_SERVER_URL_INFO *pcfg = (HY_DVR_MP_SERVER_URL_INFO*) pBuffer;

			//判断buf空间是否足够
			if (dwBufLen < sizeof(HY_DVR_MP_SERVER_URL_INFO))
			{
				nRet = IDVR_CIU_BUF_SMALL;
				MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_MP_SERVER_URL_INFO), dwBufLen);
				break;
			}

			nRet = SetMpServerParamCfg(pcfg);
			break;
		}
		default:
		{
			MPU_ERR("unsupported dwMinorType = %lu\n", dwMinorType);
			nRet = IDVR_CIU_UNSUPPORTED_MINORTYPE;
			break;
		}
	}

	MPU_DBG("ID %d, Major %d, Minor %d, return %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType, nRet);
	return nRet;
}
#endif

#ifdef __MODULE_4G_SUPPORT__

//!\brief  设置卡选择配置 
int CIU_SetConfigure_4GCardSelect(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, const BYTE * pBuffer, DWORD dwBufLen)
{
	int nRet = IDVR_OK;

	MPU_DBG("ID %d, Major %d, Minor %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType);

	switch (dwMinorType)
	{
		case HY_DVR_4G_CARD_SELECT_CFG:
		{
			HY_DVR_4G_CARD_SELECT_CFG_INFO *pcfg = (HY_DVR_4G_CARD_SELECT_CFG_INFO*)pBuffer;
			
			if (dwBufLen < sizeof(HY_DVR_4G_CARD_SELECT_CFG_INFO))
			{
				nRet = IDVR_CIU_BUF_SMALL;
				MPU_ERR("buffer is too small, need %d, real %lu\n", 
							sizeof(HY_DVR_4G_CARD_SELECT_CFG_INFO), dwBufLen);
			}

			nRet = Set4GCardSelectCfg(pcfg);
			
			break;
		}
		default:
		{
			MPU_ERR ("unsupported dw minor type(%d)\n", dwMinorType);
			nRet = IDVR_CIU_UNSUPPORTED_MINORTYPE;
			break;
		}
	}

	return nRet;
}


//!\brief   远程配置，获取选择配置 
int CIU_GetConfigure_4GCardSelect(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen)
{
	int nRet = IDVR_OK;

	MPU_DBG("ID %d, Major %d, Minor %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType);

	switch (dwMinorType)
	{
			/*获取设备显示信息 */
		case HY_DVR_4G_CARD_SELECT_CFG:
		{
			HY_DVR_4G_CARD_SELECT_CFG_INFO *pcfg = (HY_DVR_4G_CARD_SELECT_CFG_INFO*) pBuffer;

			//判断buf空间是否足够
			if (*pdwBufLen < sizeof(HY_DVR_4G_CARD_SELECT_CFG_INFO))
			{
				nRet = IDVR_CIU_BUF_SMALL;
				MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_4G_CARD_SELECT_CFG_INFO), *pdwBufLen);
				break;
			}

			memset(pcfg, 0, sizeof(HY_DVR_4G_CARD_SELECT_CFG_INFO));
			*pdwBufLen = sizeof(HY_DVR_4G_CARD_SELECT_CFG_INFO);

			nRet = Get4GCardSelectCfg(pcfg);
			
			break;
		}
		case HY_DVR_4G_CARD_SELECT_RESULT:
		{
			HY_DVR_4G_CARD_SELECT_RESULT_INFO *pcfg = (HY_DVR_4G_CARD_SELECT_RESULT_INFO*) pBuffer;

			//判断buf空间是否足够
			if (*pdwBufLen < sizeof(HY_DVR_4G_CARD_SELECT_RESULT_INFO))
			{
				nRet = IDVR_CIU_BUF_SMALL;
				MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_4G_CARD_SELECT_RESULT_INFO), *pdwBufLen);
				break;
			}

			memset(pcfg, 0, sizeof(HY_DVR_4G_CARD_SELECT_RESULT_INFO));
			*pdwBufLen = sizeof(HY_DVR_4G_CARD_SELECT_RESULT_INFO);

			nRet = Get4GCardSelectResult(pcfg);
			break;
		}
		default:
		{
			MPU_ERR("unsupported dwMinorType = %lu\n", dwMinorType);
			nRet = IDVR_CIU_UNSUPPORTED_MINORTYPE;
			break;
		}
	}

	MPU_DBG("ID %d, Major %d, Minor %d, return %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType, nRet);
	return nRet;
}


//!\brief	远程配置，获取4G卡信息 
int CIU_GetConfigure_4GCardInfo(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen)
{
	int nRet = IDVR_OK;

	MPU_DBG("ID %d, Major %d, Minor %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType);

	switch (dwMinorType)
	{
		case HY_DVR_4G_CARD_CFG:
		{
			HY_DVR_4G_CARD_CFG_LIST *pcfg = (HY_DVR_4G_CARD_CFG_LIST*) pBuffer;

			//判断buf空间是否足够
			if (*pdwBufLen < sizeof(HY_DVR_4G_CARD_CFG_LIST))
			{
				nRet = IDVR_CIU_BUF_SMALL;
				MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_POWER_CONFIG_INFO), *pdwBufLen);
				break;
			}

			memset(pcfg, 0, sizeof(HY_DVR_4G_CARD_CFG_LIST));
			*pdwBufLen = sizeof(HY_DVR_4G_CARD_CFG_LIST);

			nRet = Get4GCardInfo(pcfg);
			
			break;
		}
		default:
		{
			MPU_ERR("unsupported dwMinorType = %lu\n", dwMinorType);
			nRet = IDVR_CIU_UNSUPPORTED_MINORTYPE;
			break;
		}
			
	}


	MPU_DBG("ID %d, Major %d, Minor %d, return %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType, nRet);
	return nRet;
}


//!\brief	远程配置，获取双sim卡信息 
int CIU_GetConfigure_SimIndexCardInfo(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen)
{
	int nRet = IDVR_OK;

	MPU_DBG("ID %d, Major %d, Minor %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType);

	switch (dwMinorType)
	{
		case SIM_CARD_INFO:
		{
			HY_DVR_SIM_CARD_INFO_LIST *pcfg = (HY_DVR_SIM_CARD_INFO_LIST*) pBuffer;

			//判断buf空间是否足够
			if (*pdwBufLen < sizeof(HY_DVR_SIM_CARD_INFO_LIST))
			{
				nRet = IDVR_CIU_BUF_SMALL;
				MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_SIM_CARD_INFO_LIST), *pdwBufLen);
				break;
			}

			memset(pcfg, 0, sizeof(HY_DVR_SIM_CARD_INFO_LIST));
			*pdwBufLen = sizeof(HY_DVR_SIM_CARD_INFO_LIST);

			nRet = GetSimIndexCardInfo(pcfg);
			
			break;
		}
		default:
		{
			MPU_ERR("unsupported dwMinorType = %lu\n", dwMinorType);
			nRet = IDVR_CIU_UNSUPPORTED_MINORTYPE;
			break;
		}
			
	}


	MPU_DBG("ID %d, Major %d, Minor %d, return %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType, nRet);
	return nRet;
}



int CIU_GetConfigure_FL(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen)
{
	int nRet = IDVR_OK;

	
	switch (dwMinorType)
	{
			//获取网络适配器配置信息
		case FL_INIT_POST_APPID:
			{
				LPHY_DVR_FL_APPID pcfg = (LPHY_DVR_FL_APPID) pBuffer;

				//判断buf空间是否足够
				if (*pdwBufLen < sizeof (HY_DVR_FL_APPID))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof (HY_DVR_FL_APPID), *pdwBufLen);
					break;
				}

				memset(pcfg, 0, sizeof (HY_DVR_FL_APPID));
				*pdwBufLen = sizeof (HY_DVR_FL_APPID);

				nRet = GetFlAppid(pcfg);
			}
			break;
     	case FL_INIT_POST_APPKEY:
     		{
				LPHY_DVR_FL_APPKEY pcfg = (LPHY_DVR_FL_APPKEY) pBuffer;

				//判断buf空间是否足够
				if (*pdwBufLen < sizeof (HY_DVR_FL_APPKEY))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof (HY_DVR_FL_APPKEY), *pdwBufLen);
					break;
				}

				memset(pcfg, 0, sizeof (HY_DVR_FL_APPKEY));
				*pdwBufLen = sizeof (HY_DVR_FL_APPKEY);

				nRet = GetFlAppkey(pcfg);
			}
			break;
		case FL_NETWORK_INFO:
     		{
				LPHY_DVR_FL_NETWORK_INFO pcfg = (LPHY_DVR_FL_NETWORK_INFO) pBuffer;

				//判断buf空间是否足够
				if (*pdwBufLen < sizeof (HY_DVR_FL_NETWORK_INFO))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof (HY_DVR_FL_NETWORK_INFO), *pdwBufLen);
					break;
				}

				memset(pcfg, 0, sizeof (HY_DVR_FL_NETWORK_INFO));
				*pdwBufLen = sizeof (HY_DVR_FL_NETWORK_INFO);

				nRet = GetFlNetworkInfo(pcfg);
			}
			break;		
		case FL_INIT_BW_CARDTYPE:
     		{
				LPHY_DVR_FL_BW_CARDTYPE pcfg = (LPHY_DVR_FL_BW_CARDTYPE) pBuffer;

				//判断buf空间是否足够
				if (*pdwBufLen < sizeof (HY_DVR_FL_BW_CARDTYPE))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof (HY_DVR_FL_BW_CARDTYPE), *pdwBufLen);
					break;
				}

				memset(pcfg, 0, sizeof (HY_DVR_FL_BW_CARDTYPE));
				*pdwBufLen = sizeof (HY_DVR_FL_BW_CARDTYPE);

				nRet = GetNorthLatiCardType(pcfg);
			}
			break;		
		 default:
            {
                nRet = IDVR_CIU_UNSUPPORTED_MINORTYPE;
                MPU_ERR("unsupported dwMinorType = %lu\n", dwMinorType);
            }
            break;
	}

	return nRet;
}

#endif

//TCP转发是否开启
static int GetTcpRelayCfg(HY_DVR_TCPRELAY_ENABLE *pcfg)
{
    int nRet = IDVR_OK;
    int bEnable = 0;

    ///////////////////////////////////////////////////////////////////////////////
    //本结构体是否有效，=0表示无效
    pcfg->iValid = 1;
    //默认返回设置成功
    nRet = MPU_Net_TcpRelay_GetEnable(&bEnable);
	pcfg->iEnable=bEnable;
    return nRet;
}

//设置TCP转发
static int SetTcpRelayCfg(HY_DVR_TCPRELAY_ENABLE *pcfg)
{
    int nRet = IDVR_OK;
    ///////////////////////////////////////////////////////////////////////////////
    //本结构体是否有效，=0表示无效
    //默认返回设置成功
    nRet = MPU_Net_TcpRelay_SetEnable(&pcfg->iEnable);

    return nRet;
}

int CIU_GetConfigure_TcpRelay(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen)
{
	int nRet = IDVR_OK;
	
	switch (dwMinorType)
	{
		case TCPRELAY_ENABLE:
        {
			///////////////////////////////////////////////////////////////////////////////
			//判断buf空间是否足够
			if (*pdwBufLen < sizeof(HY_DVR_TCPRELAY_ENABLE))
			{
				nRet = IDVR_CIU_BUF_SMALL;
				MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_TCPRELAY_ENABLE), *pdwBufLen);
				break;
			}

			HY_DVR_TCPRELAY_ENABLE *pcfg = (HY_DVR_TCPRELAY_ENABLE*) pBuffer;

			///////////////////////////////////////////////////////////////////////////////
			//初始化输出缓冲区
			memset(pcfg, 0, sizeof(HY_DVR_TCPRELAY_ENABLE));
			*pdwBufLen = sizeof(HY_DVR_TCPRELAY_ENABLE);

			nRet = GetTcpRelayCfg(pcfg);
		}
        break;

        default:
        {
            MPU_ERR("unsupported dwMinorType = %lu\n", dwMinorType);
            nRet = IDVR_CIU_UNSUPPORTED_MINORTYPE;
        }
        break;
    }
           
    return nRet;
}
int CIU_SetConfigure_TcpRelay(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, const BYTE * pBuffer, DWORD  dwBufLen)
{
	int nRet = IDVR_OK;
	printf("CIU_SetConfigure_TcpRelay\n");
	switch (dwMinorType)
	{
		case TCPRELAY_ENABLE:
        {
			///////////////////////////////////////////////////////////////////////////////
			HY_DVR_TCPRELAY_ENABLE *pcfg = (HY_DVR_TCPRELAY_ENABLE*) pBuffer;
			//判断buf空间是否足够
			if (dwBufLen < sizeof(HY_DVR_TCPRELAY_ENABLE))
			{
				nRet = IDVR_CIU_BUF_SMALL;
				MPU_ERR("buffer is too small, need %d, real %lu\n", 
							sizeof(HY_DVR_TCPRELAY_ENABLE), dwBufLen);
			}
            nRet = SetTcpRelayCfg(pcfg);
		}
        break;

        default:
        {
            MPU_ERR("unsupported dwMinorType = %lu\n", dwMinorType);
            nRet = IDVR_CIU_UNSUPPORTED_MINORTYPE;
        }
        break;
    }
           
    return nRet;
}


#if defined (__IS_SUPPORT_4G_CLOUD_DOMAIN__)
static int SetAddDomainForGetAlias(HY_DVR_ZWHOST_API_ADD_DOMAIN_FOR_GET_ALIAS * pcfg)
{
	if (NULL == pcfg)
	{
		printf("NULL pointer\n");
		return -1;
	}
	int ret = -1;

	printf("pcfg->nValid：%d cDomain: %s\n", pcfg->nValid, pcfg->cDomain);
	if (1 == pcfg->nValid && strlen(pcfg->cDomain))
	{
		ret = MPU_NET_SetAddDomainForGetAlias(&pcfg->cDomain[0]);
	}

	return ret;
}

static int SetAddDomainList2Local(HY_DVR_ZWHOST_API_ADD_DOMAIN_2_LOCAL *pcfg)
{
	if (NULL == pcfg)
	{
		printf("NULL pointer\n");
		return -1;
	}
	int ret = 0;
	int count = 0;
	int totalsize = sizeof(pcfg->stDomainList) / sizeof(pcfg->stDomainList[0]);
	NET_DOMAIN_INFO_LIST DomainInfoList;
	memset(&DomainInfoList, 0, sizeof(DomainInfoList));
	printf("pcfg->nValid：%d totalsize: %d \n", pcfg->nValid, totalsize);
	
	for (unsigned int i = 0; i < sizeof(pcfg->stDomainList)/sizeof(pcfg->stDomainList[0]); ++i)
	{
		if (1 == pcfg->stDomainList[i].nValid)
		{
			printf("pcfg->stDomainList[%d].Domain: %s\n", i, pcfg->stDomainList[i].cDomain);
			memcpy(&DomainInfoList.stDomainInfoList[i].cDomain, &pcfg->stDomainList[i].cDomain, sizeof(DomainInfoList.stDomainInfoList[i].cDomain));
			memcpy(&DomainInfoList.stDomainInfoList[i].cIPv4, &pcfg->stDomainList[i].cIP, sizeof(DomainInfoList.stDomainInfoList[i].cIPv4));
			count++;
		}
		else
		{
			printf("pcfg->stDomainList[%d] inValid \n", i);
		}
	}

	printf("useful count: %d\n", count);
	
	// 转化成中间层需要的结构体
	DomainInfoList.nCount = count; 
	for (int i = 0; i < count; ++i)
	{
		printf("final DomainList[%d].Domain: %s ipv4: %s \n", i, DomainInfoList.stDomainInfoList[i].cDomain, DomainInfoList.stDomainInfoList[i].cIPv4);
	}
	ret = MPU_NET_SetAddDomainList2Local(DomainInfoList);

	return ret;
}

// 添加域名
int CIU_SetConfigure_AddDomain(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, const BYTE * pBuffer, DWORD  dwBufLen)
{
	int nRet = IDVR_OK;

	switch (dwMinorType)
	{
        case ZWHOST_API_ADD_DOMAIN_FOR_GET_ALIAS:
        {
            HY_DVR_ZWHOST_API_ADD_DOMAIN_FOR_GET_ALIAS * pcfg = (HY_DVR_ZWHOST_API_ADD_DOMAIN_FOR_GET_ALIAS *) pBuffer; 
            
            //判断buf空间是否足够
            if (dwBufLen < sizeof(HY_DVR_ZWHOST_API_ADD_DOMAIN_FOR_GET_ALIAS))
            {
                nRet = IDVR_CIU_BUF_SMALL;
                MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_ZWHOST_API_ADD_DOMAIN_FOR_GET_ALIAS), dwBufLen);
                break;
            } 

            nRet = SetAddDomainForGetAlias(pcfg);
        }
        break;
        case ZWHOST_API_ADD_DOMAIN_LIST_2_LOCAL:
        {
            HY_DVR_ZWHOST_API_ADD_DOMAIN_2_LOCAL * pcfg = (HY_DVR_ZWHOST_API_ADD_DOMAIN_2_LOCAL *) pBuffer; 
            
            //判断buf空间是否足够
            if (dwBufLen < sizeof(HY_DVR_ZWHOST_API_ADD_DOMAIN_2_LOCAL))
            {
                nRet = IDVR_CIU_BUF_SMALL;
                MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_ZWHOST_API_ADD_DOMAIN_2_LOCAL), dwBufLen);
                break;
            } 

            nRet = SetAddDomainList2Local(pcfg);
        }
        break;
        default:
        {
            MPU_ERR("unsupported dwMinorType = %lu\n", dwMinorType);
            nRet = IDVR_CIU_UNSUPPORTED_MINORTYPE;
        }
        break;
    }

    return nRet;
}
#endif


