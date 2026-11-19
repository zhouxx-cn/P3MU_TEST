// Copyright (C) 1991 - 1999 Rational Software Corporation


#include <iostream>
#include <netdb.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
//#include "hie_dnsupdate.h"
#include "IDDNS.h"
#include "CConfig.h"
#include "adsl.h"
#include "ip.h"
#include "INet.h"
// #include "sys_banned.h"
#include "global_gather.h"

typedef struct _DDNS_INFO_
{
	char strProtocolName[128];
	char strServerName[256];
} DDNS_INFO;

#define ISDIYDDNSNAME(name)         ( 0 == strcmp(DIYDDNSNAME,name))
char IDDNS::m_NetDevName[16];

static DDNS_INFO m_ddns_info[MAX_DDNS_PROTO];

#if 0
static DDNS_INFO m_ddns_info[] = {
	{"noip","dynupdate.no-ip.com"},	
	{"qdns", "members.3322.org"},
	{"qdns-static", "members.3322.org(static)"},
	{"dyndns", "members.dyndns.org"},
	{"dyndns-static", "members.dyndns.org(static)"},
	{"tzo", "cgi.tzo.com"},
	{"easydns", "members.easydns.com"},
	{"justlinux", "www.justlinux.com"},
	{"ods", "update.ods.org"},
	{"zoneedit", "www.zoneedit.com"},
	{"hn", "www.hn.org"},
	{"dhs", "members.dhs.org"},
	{"dyns", "www.dyns.cx"},
	{"vantech", "www.vantechdns.net"},
	/*{"higheasy.cn", DIYDDNSNAME},*/
};
#endif

//线程函数
static void *Fnx_CheckThread(void *data);

//##ModelId=49BDB4E6017C
IDDNS *IDDNS::m_Instance = 0;

//##ModelId=49B634E7025D
IDDNS::IDDNS()
{
	m_bDDNSEnable = 0;
	m_iProtocolCount = 0;
	m_iIndex = 0;
	m_iDDNS_status = DDNS_STATE_STOP;
	m_hCheckThread = 0;
	m_bCheck_exit = 1;
	m_pCConfig = NULL;
	m_iTimeCnt = 0;
	m_iTimeCheckIp = CHECKEXTERNIPTIME;
#if 0
	m_sProtoName[0] = 0;
	m_sSvrName[0] = 0;
#endif
	m_sDomainName[0] = 0;
	m_sUserName[0] = 0;
	m_sPassword[0] = 0;
	strncpy(m_NetDevName, "eth0", sizeof(m_NetDevName) - 1);
}

//!\brief 初始化
int IDDNS::Init()
{
	int ret;

	//检测是否已经初始化
	if (NULL != (void *) m_hCheckThread)
	{
		return 0;
	}

	//获取配置实例
	m_pCConfig = CConfig::GetInstance();
	if (NULL == m_pCConfig)
	{
		NET_ERR("[IDDNS:%s]NULL==m_pCConfig\n", __FUNCTION__);
		UnInit();
		return -1;
	}

	//导入配置
	LoadParam();

	DDNS_Start();

	m_iTimeCnt = 0;
	m_iTimeCheckIp = CHECKEXTERNIPTIME;
	m_bCheck_exit = 0;
	//创建注册检测线程
	ret = pthread_create(&m_hCheckThread, NULL, Fnx_CheckThread, this);
	if (ret < 0)
	{
		NET_ERR("[IDDNS:%s]Create thread failed\n", __FUNCTION__);
		UnInit();
		return -1;
	}

	return 0;
}

//##ModelId=49B634E7025E
IDDNS::~IDDNS()
{
	UnInit();
}

//!\brief 释放资源
int IDDNS::UnInit()
{
	//结束注册检测线程
	m_bCheck_exit = 1;
	NET_INFO("[IDDNS:%s] exit...\n", __FUNCTION__);

	if (0 != m_hCheckThread)
	{
		NET_DBG("[IDDNS:%s] cancel...\n", __FUNCTION__);
		pthread_cancel(m_hCheckThread);
		NET_DBG("[IDDNS:%s] wait exit...\n", __FUNCTION__);
		pthread_join(m_hCheckThread, NULL);
		NET_DBG("[IDDNS:%s] ok...\n", __FUNCTION__);
		m_hCheckThread = 0;
	}

	NET_INFO("[IDDNS:%s] ok all...\n", __FUNCTION__);
	return 0;
}

//!\brief   设置DDNS服务器名称
//!\return  0成功,其他失败
int IDDNS::SetDIYDDNSServer(const char *szName)
{
	if (strlen(szName) >= DOMAINNAMELEN)
	{
		return -1;
	}
	snprintf(m_sServerName, sizeof(m_sServerName), "%s", szName);
	m_pCConfig->SetValue(DDNS_CfgSection, "DIYDDNSServer", m_sServerName);

	DDNS_Stop();
	DDNS_Start();
	return 0;
}

//!\brief   获取DDNS服务器名称
//!\return  0成功,其他失败
int IDDNS::GetDIYDDNSServer(char *szName, const int len)
{

	if (strlen(m_sServerName) >= (size_t) len)
	{
		return -1;
	}

	snprintf(szName, len, "%s", m_sServerName);
	return 0;
}

int IDDNS::LoadParam()
{
	int i = 0;
	FILE *pFile = 0;
	char acBuf[256] = {0};
	char *ptrDelim = 0;
	char *substr_ptr;
	
	//使能值
	m_bDDNSEnable = DDNS_GetEnable();
	DDNS_ReGetNetDevName();
	//获得协议总数
	for (i=0;i<MAX_DDNS_PROTO;i++)
	{
		memset(&m_ddns_info[i], 0, sizeof(DDNS_INFO));
	}

	pFile = fopen(DDNS_SERVER_CONFIG_FILE, "r");
	if (NULL == pFile)
	{
		fprintf(stderr, "Fail to open %s: %s\n", DDNS_SERVER_CONFIG_FILE, strerror(errno));
		return -1;
	}
	
	/* 从配置文件逐行获取ddns服务器，放在数组里面 */
	m_iProtocolCount = 0;
	while(!feof(pFile))
	{
		memset(acBuf, 0, sizeof(acBuf));
		fgets(acBuf, sizeof(acBuf), pFile);

		if (strlen(acBuf) <= 1)
			continue;

		ptrDelim = strchr(acBuf, ',');
		if (0 == ptrDelim)
			continue;

		substr_ptr= strstr(acBuf, "\r\n");
		if (NULL != substr_ptr)
		{
			*substr_ptr = '\0';
		}

		if (acBuf[strlen(acBuf)-1] == '\r' || acBuf[strlen(acBuf)-1] == '\n')
			acBuf[strlen(acBuf)-1] = '\0';

		if (strstr(acBuf, "vantech"))
			continue;

		snprintf(m_ddns_info[m_iProtocolCount].strProtocolName, ptrDelim-acBuf+1, "%s", acBuf);
		snprintf(m_ddns_info[m_iProtocolCount].strServerName, sizeof(m_ddns_info[m_iProtocolCount].strServerName), "%s", ptrDelim+1);
		//fprintf(stderr, "[%s][%d] strProtocolName=[%s] strServerName=[%s]\r\n",__FUNCTION__,__LINE__, m_ddns_info[m_iProtocolCount].strProtocolName,m_ddns_info[m_iProtocolCount].strServerName);
		m_iProtocolCount++;
	}

	fclose(pFile);
	pFile = 0;
#if 0
	m_iProtocolCount = m_pCConfig->GetValue(DDNS_CfgSection, "ProtocolCount", 0);
	if (m_iProtocolCount <= 0)
	{							// 配置无协议 
		m_iProtocolCount = 0;
		return 0;
	}
	//当前服务器主机名
	snprintf(key, sizeof(key), "ServerName_%02d", m_iIndex);
	m_pCConfig->GetValue(DDNS_CfgSection, key, "", m_sSvrName, DOMAINNAMELEN);

	//当前协议名
	DDNS_GetProtocolName(m_iIndex, m_sProtoName);
#else
	//m_iProtocolCount = sizeof (m_ddns_info) / sizeof (DDNS_INFO);

#endif
	//当前索引
	m_iIndex = DDNS_GetCurrentProtocol();

	//检查索引是否有效
	if (m_iIndex < 0 || m_iIndex >= m_iProtocolCount)
	{
		m_iIndex = 0;
	}



	//当前域名
	DDNS_GetDomainName(m_iIndex, m_sDomainName, sizeof(m_sDomainName));

	//当前用户名
	DDNS_GetUserName(m_iIndex, m_sUserName);

	//当前密码
	DDNS_GetPassword(m_iIndex, m_sPassword);

	//当前保活时间
	m_uiUpdateTime = DDNS_GetAutoUpdateTime(m_iIndex);

	m_bProtoValid = 1;

	m_pCConfig->GetValue(DDNS_CfgSection, "DIYDDNSServer", "www.higheasy.cn", m_sServerName, sizeof (m_sServerName));

	return 0;
}



//##ModelId=49B634E7025F
int IDDNS::DDNS_GetProtocolCount()
{
	return m_iProtocolCount;
}

//##ModelId=49BDD28802CE
char *IDDNS::DDNS_GetProtocolName(int nIndex, char *pszProtocolName)
{
	if (nIndex < 0 || nIndex >= m_iProtocolCount)
	{
		NET_ERR("[IDDNS:%s]nIndex=%d MAXCont=%d\n", __FUNCTION__, nIndex, m_iProtocolCount);
		return NULL;
	}
	if (NULL == pszProtocolName)
	{
		NET_ERR("[IDDNS:%s]NULL==pszProtocolName\n", __FUNCTION__);
		return NULL;
	}
#if 0
	snprintf(key, sizeof(key), "ProtocolName_%02d", nIndex);
	m_pCConfig->GetValue(DDNS_CfgSection, key, "", pszProtocolName, PROTOCOLNAMELEN);

#else
	strncpy(pszProtocolName, m_ddns_info[nIndex].strServerName, sizeof(m_ddns_info[nIndex].strServerName) - 1);
#endif
	return pszProtocolName;
}

//##ModelId=49B634E70261
int IDDNS::DDNS_GetCurrentProtocol()
{
	if (m_iProtocolCount == 0)
	{
		NET_ERR("[IDDNS:%s] MAXCont=0\n", __FUNCTION__);
		return -1;
	}
	return m_pCConfig->GetValue(DDNS_CfgSection, "CurrProtoIndex", 0);;
}

//##ModelId=49B634E70262
int IDDNS::DDNS_SetCurrentProtocol(int nIndex)
{
//  char key[30];

	if (nIndex < 0 || nIndex >= m_iProtocolCount)
	{
		NET_ERR("[IDDNS:%s]nIndex=%d MAXCont=%d\n", __FUNCTION__, nIndex, m_iProtocolCount);
		return -1;
	}
	if (m_iIndex == nIndex)
	{
		return 0;
	}
	m_iIndex = nIndex;


	//保存配置
	m_pCConfig->SetValue(DDNS_CfgSection, "CurrProtoIndex", m_iIndex);
	//先停止更新
	DDNS_Stop();
	//////////////////////////////////////////////////////////////////////////
	//导入新值
#if 0
	//当前协议名
	DDNS_GetProtocolName(m_iIndex, m_sProtoName);


	//当前服务器主机名
	snprintf(key, sizeof(key), "ServerName_%02d", m_iIndex);
	m_pCConfig->GetValue(DDNS_CfgSection, key, "", m_sSvrName, DOMAINNAMELEN);

#endif
	//域名
	DDNS_GetDomainName(m_iIndex, m_sDomainName, sizeof(m_sDomainName));

	//用户名
	DDNS_GetUserName(m_iIndex, m_sUserName);

	//密码
	DDNS_GetPassword(m_iIndex, m_sPassword);

	//更新时间
	m_uiUpdateTime = DDNS_GetAutoUpdateTime(m_iIndex);


	//新值导完后，开启更新
	DDNS_Start();
	return 0;
}

//##ModelId=49B634E70263
int IDDNS::DDNS_GetEnable()
{
	return m_pCConfig->GetValue(DDNS_CfgSection, DDNS_Enable_key, 0);
}

//##ModelId=49B634E70264
int IDDNS::DDNS_SetEnable(int bEnable)
{
	m_bDDNSEnable = bEnable;
	
	if (m_pCConfig->SetValue(DDNS_CfgSection, DDNS_Enable_key, m_bDDNSEnable) < 0)
	{
		NET_ERR("[IDDNS:%s]SetValue failed\n", __FUNCTION__);
		return -1;
	}
	return 0;
}

//##ModelId=49B634E70265
char *IDDNS::DDNS_GetUserName(int nIndex, char *pszUserName)
{
	char key[100];

	if (nIndex < 0 || nIndex >= m_iProtocolCount)
	{
		NET_ERR("[IDDNS:%s]nIndex=%d MAXCont=%d\n", __FUNCTION__, nIndex, m_iProtocolCount);
		return NULL;
	}
	if (NULL == pszUserName)
	{
		NET_ERR("[IDDNS:%s]NULL==pszUserName\n", __FUNCTION__);
		return NULL;
	}
	snprintf(key, sizeof(key), "UserName_%02d", nIndex);
	m_pCConfig->GetValue(DDNS_CfgSection, key, "", pszUserName, DDNS_USERNAME_LEN);

	return pszUserName;
}

//##ModelId=49B634E70267
int IDDNS::DDNS_SetUserName(int nIndex, const char *pszUserName)
{
	char key[100];
	if (nIndex < 0 || nIndex >= m_iProtocolCount)
	{
		NET_ERR("[IDDNS:%s]nIndex=%d MAXCont=%d\n", __FUNCTION__, nIndex, m_iProtocolCount);
		return -1;
	}
	if (NULL == pszUserName)
	{
		NET_ERR("[IDDNS:%s]NULL==pszUserName\n", __FUNCTION__);
		return -1;
	}
	snprintf(key, sizeof(key), "UserName_%02d", nIndex);
	if (m_pCConfig->SetValue(DDNS_CfgSection, key, pszUserName) < 0)
	{
		NET_ERR("[IDDNS:%s]SetValue failed\n", __FUNCTION__);
		return -1;
	}
	//判断是否为当前协议,如是，则要更新
	if (nIndex == m_iIndex)
	{
		DDNS_Stop();
		strncpy(m_sUserName, pszUserName, DDNS_USERNAME_LEN - 1);
		DDNS_Start();
	}
	return 0;
}

//##ModelId=49B634E70268
char *IDDNS::DDNS_GetPassword(int nIndex, char *pszPassword)
{
	char key[100] = {0};
	if (nIndex < 0 || nIndex >= m_iProtocolCount)
	{
		NET_ERR("[IDDNS:%s]nIndex=%d MAXCont=%d\n", __FUNCTION__, nIndex, m_iProtocolCount);
		return NULL;
	}
	if (NULL == pszPassword)
	{
		NET_ERR("[IDDNS:%s]NULL==pszPassword\n", __FUNCTION__);
		return NULL;
	}
	snprintf(key, sizeof(key), "Password_%02d", nIndex);
	m_pCConfig->GetValue(DDNS_CfgSection, key, "", pszPassword, DDNS_PASSWORD_LEN);
	return pszPassword;
}

//##ModelId=49B634E70269
int IDDNS::DDNS_SetPassword(int nIndex, const char *pszPassword)
{
	char key[100];
	if (nIndex < 0 || nIndex >= m_iProtocolCount)
	{
		NET_ERR("[IDDNS:%s]nIndex=%d MAXCont=%d\n", __FUNCTION__, nIndex, m_iProtocolCount);
		return -1;
	}
	if (NULL == pszPassword)
	{
		NET_ERR("[IDDNS:%s]NULL==pszPassword\n", __FUNCTION__);
		return -1;
	}
	snprintf(key, sizeof(key), "Password_%02d", nIndex);
	if (m_pCConfig->SetValue(DDNS_CfgSection, key, pszPassword) < 0)
	{
		NET_ERR("[IDDNS:%s]SetValue failed\n", __FUNCTION__);
		return -1;
	}
	//判断是否为当前协议,如是，则要更新
	if (nIndex == m_iIndex)
	{
		DDNS_Stop();
		strncpy(m_sPassword, pszPassword, DDNS_PASSWORD_LEN - 1);
		DDNS_Start();
	}
	return 0;
}

//##ModelId=49B634E7026A
char *IDDNS::DDNS_GetDomainName(int nIndex, char *pszDomainName, int DomainNameSize)
{
	char key[DOMAINNAMELEN + 1];

	if (nIndex < 0 || nIndex >= m_iProtocolCount)
	{
		NET_ERR("[IDDNS:%s]nIndex=%d MAXCont=%d\n", __FUNCTION__, nIndex, m_iProtocolCount);
		return NULL;
	}
	if (NULL == pszDomainName)
	{
		NET_ERR("[IDDNS:%s]NULL==pszDomainName\n", __FUNCTION__);
		return NULL;
	}

	if (!ISDIYDDNSNAME(m_ddns_info[nIndex].strServerName))
	{
		snprintf(key, sizeof(key), "DoMainName_%02d", nIndex);
		m_pCConfig->GetValue(DDNS_CfgSection, key, "", pszDomainName, DOMAINNAMELEN);
	}
	else
	{
		if (m_sDomainName[0] == 0)
		{
			NET_WARN("current protol is diyddns not get domainname\n");
		}
		else
		{
			snprintf(pszDomainName, DomainNameSize, "%s", m_sDomainName);
			NET_DBG("pszDomainName = %s\n", pszDomainName);
		}
	}

	return pszDomainName;
}

//##ModelId=49B634E7026B
int IDDNS::DDNS_SetDomainName(int nIndex, const char *pszDomainName)
{
	char key[100];

	if (nIndex < 0 || nIndex >= m_iProtocolCount)
	{
		NET_ERR("[IDDNS:%s]nIndex=%d MAXCont=%d\n", __FUNCTION__, nIndex, m_iProtocolCount);
		return -1;
	}
	if (NULL == pszDomainName)
	{
		NET_ERR("[IDDNS:%s]NULL==pszDomainName\n", __FUNCTION__);
		return -1;
	}
	snprintf(key, sizeof(key), "DoMainName_%02d", nIndex);
	if (m_pCConfig->SetValue(DDNS_CfgSection, key, pszDomainName) < 0)
	{
		NET_ERR("[IDDNS:%s]SetValue failed\n", __FUNCTION__);
		return -1;
	}


	if (ISDIYDDNSNAME(m_ddns_info[nIndex].strServerName))
	{
		NET_ERR("[IDDNS:%s] Protocol %s Is not need Set DomainName\n", __FUNCTION__, m_ddns_info[nIndex].strServerName);
	}

	//判断是否为当前协议,如是，则要更新
	if (nIndex == m_iIndex)
	{
		DDNS_Stop();
		if (!ISDIYDDNSNAME(m_ddns_info[nIndex].strServerName))
		{
			strncpy(m_sDomainName, pszDomainName, DOMAINNAMELEN - 1);
		}
		DDNS_Start();
	}
	return 0;
}

//##ModelId=49B634E7026C
int IDDNS::DDNS_GetAutoUpdateTime(int nIndex)
{
	char key[100] = {0};
	if (nIndex < 0 || nIndex >= m_iProtocolCount)
	{
		NET_ERR("[IDDNS:%s]nIndex=%d MAXCont=%d\n", __FUNCTION__, nIndex, m_iProtocolCount);
		return -1;
	}
	snprintf(key, sizeof(key), "UpdateTime_%02d", nIndex);

	return m_pCConfig->GetValue(DDNS_CfgSection, key, 30 * 60);	//缺省为30分钟
}

//##ModelId=49B634E7026D
int IDDNS::DDNS_SetAutoUpdateTime(int nIndex, int nIntervalSecond)
{
	char key[30];
	if (nIndex < 0 || nIndex >= m_iProtocolCount)
	{
		NET_ERR("[IDDNS:%s]nIndex=%d MAXCont=%d\n", __FUNCTION__, nIndex, m_iProtocolCount);
		return -1;
	}
	if (nIntervalSecond <= 5)	//不能小于5秒
	{
		NET_ERR("[IDDNS:%s]nIndex=%d nIntervalSecond=%d\n", __FUNCTION__, nIndex, nIntervalSecond);
		return -1;
	}
	snprintf(key, sizeof(key), "UpdateTime_%02d", nIndex);
	if (m_pCConfig->SetValue(DDNS_CfgSection, key, nIntervalSecond) < 0)
	{
		NET_ERR("[IDDNS:%s]SetValue failed\n", __FUNCTION__);
		return -1;
	}
	//判断是否为当前协议,如是，则要更新
	if (nIndex == m_iIndex)
	{
		DDNS_Stop();
		m_uiUpdateTime = nIntervalSecond;
		DDNS_Start();
	}
	return 0;
}

//##ModelId=49B634E7026E
int IDDNS::DDNS_Start()
{
	//未使能
	if (!m_bDDNSEnable)
	{
		return 0;
	}
	//正在启动
	m_iDDNS_status = DDNS_STATE_CONNECTING;

	m_bStart = 1;

	return 0;
}

//##ModelId=49B634E7026F
int IDDNS::DDNS_Stop()
{
	m_bStart = 0;
	m_bStop = 1;

	if (ISDIYDDNSNAME(m_ddns_info[m_iIndex].strServerName) && m_iDDNS_status == DDNS_STATE_CONNECTED)
	{
		NET_DBG("Call hie_dnsupdate_once(1)\n");
		m_iDDNS_status = DDNS_STATE_STOP;
		//hie_dnsupdate_once(1);
	}

	//正在停止
	m_iDDNS_status = DDNS_STATE_STOP;
	return 0;
}

//##ModelId=49B634E70270
int IDDNS::DDNS_GetState()
{
	return m_iDDNS_status;
}

static int get_current_extern_ip(char *pExternIP,int ipSize);
int IDDNS::DDNS_CheckStatus()
{
	int status;

	if (m_bCheck_exit)
	{
		NET_ERR("[IDDNS:%s]stop check status \n", __FUNCTION__);
		return -1;
	}
	if (!m_bDDNSEnable)
	{							//未使能

		m_iDDNS_status = DDNS_STATE_STOP;

		return 0;
	}

	if (m_iProtocolCount <= 0)
	{							//没有协议
		m_iDDNS_status = DDNS_STATE_STOP;
		return 0;
	}

	//开始做更新
	if (m_iTimeCnt == 0 || m_bStart)
	{							//更新内容
		m_bStart = 0;
		m_iDDNS_status = DDNS_STATE_CONNECTING;

		//连接
		NET_DBG("[DDNS] start update...\n");
		status = do_connect();
		NET_DBG("[DDNS] start update  end   ...next time=%d...\n", m_uiUpdateTime);
		//初始化下一次更新时间
		m_iTimeCnt = m_uiUpdateTime;
		m_iTimeCheckIp = CHECKEXTERNIPTIME;
	
		if (status == DDNS_STATE_CONNECTED)
		{
			m_iDDNS_status = DDNS_STATE_CONNECTED;
		}
		else
		{
			m_iDDNS_status = DDNS_STATE_CONNECT_FAILED;
			// m_bStart = 1;//失败，重试
			m_iTimeCnt = 10;	//10重试一次
		}
	}
	else if (m_iTimeCnt > 0)
	{
		m_iTimeCnt--;
		m_iTimeCheckIp--;

		if(m_iTimeCheckIp <= 0)
		{
			char ip[32] = {0};
			int nRet = 0;
			
			nRet = get_current_extern_ip((char *) ip,sizeof(ip));
			if ((0 == nRet) && (ip[0] != '\0') && (0 != strcmp(ip, m_sIp)))
			{
				NET_DBG("[DDNS] ExternIp change---exterIp=%s,currentIp=%s\n",ip, m_sIp);
				m_iTimeCnt = 0;
			}
			m_iTimeCheckIp = CHECKEXTERNIPTIME;
		}
	}

	//更新完成

	return 0;					//m_iDDNS_status;
}

static void *Fnx_CheckThread(void *lParam)
{
    PRT_TID_NAME("IDDNSCheckThread");

	int ret = 0;

	IDDNS *pDDNS = (IDDNS *) lParam;

	if (NULL == pDDNS)
	{
		NET_ERR("[IDDNS:%s]NULL==pnet\n", __FUNCTION__);
		return NULL;
	}
	while (1)
	{

		ret = pDDNS->DDNS_CheckStatus();

		if (ret == -1)			//线程退出
		{
			NET_ERR("[IDDNS:%s]thread exit...\n", __FUNCTION__);
			break;
		}

		sleep(1);				//一秒检测一次
	}
	return NULL;
}


int IDDNS::ResolveHost(const char *szinHostname, char *szoutIP, int outIPSize)
{
	struct hostent *hp;

	char **pptr;
	char str[32] = { 0 };
	struct sockaddr_in m_cliaddr;

//  int len = sizeof (m_cliaddr);

	memset(&m_cliaddr, 0, sizeof (m_cliaddr));

	hp = gethostbyname(szinHostname);
	if (hp == 0)
	{
		char str[256];

		herror(str);
		NET_ERR("[%s]err:%s\n", __FUNCTION__, str);

		return -1;
	}

	pptr = hp->h_addr_list;
	for (pptr = hp->h_addr_list; *pptr != NULL; pptr++)
	{
		memcpy(&m_cliaddr.sin_addr, *pptr, hp->h_length);

		if (szoutIP != NULL)
		{
			strncpy(szoutIP, inet_ntop(AF_INET, &m_cliaddr.sin_addr, str, sizeof(str)), outIPSize - 1);
		}

		return m_cliaddr.sin_addr.s_addr;
	}
	return 0;
}

extern "C" int getexternip(char *ip, int iplen);
extern "C" int ip_host_cmp(char *ip,char *hostname);
static int get_current_extern_ip(char *pExternIP,int ipSize)
{
	int result = EDVR_ADSL_GetADSLStatus(pExternIP);
	if (0 == result)
	{
		return 0;
	}
	
	result = getexternip(pExternIP, 16);

	if (0 == result)
	{
		return 0;
	}
	
#if 0
	/* 之前是当获取公网IP失败时，则去获取设备本身IP去进行绑定，暂没必要；故去掉 */
	result = get_if_address(IDDNS::m_NetDevName,pExternIP, ipSize, GET_ADDRESS);
	if (0 == result)
	{
		return 0;
	}
#endif

	return -1;
}

int IDDNS::do_connect()
{
	int result = 0;
	char sznetmode[16] = {0};
	char ip[20] = {0};

	result = EDVR_ADSL_GetADSLStatus(ip);
	if (0 == result)
	{
		snprintf(sznetmode, sizeof(sznetmode), "ppp0");
	}
	else
	{
		snprintf(sznetmode, sizeof(sznetmode), m_NetDevName);

		result = get_if_address((char *) sznetmode, (char *) ip, sizeof(ip), GET_ADDRESS);
		if (result)
		{
			NET_ERR("[IDDNS:%s]Get ipaddress failed (%d) \n", __FUNCTION__, result);
			return DDNS_STATE_CONNECT_FAILED;
		}
	}

	if (ISDIYDDNSNAME(m_ddns_info[m_iIndex].strServerName))
	{
		int ret = 0;

		char mac[256] = {0};

		int tv_sec = m_uiUpdateTime;
		get_current_extern_ip((char *) ip,sizeof(ip));

		result = get_if_address((char *)m_NetDevName, (char *) mac, sizeof(mac), GET_MAC);
		NET_DBG("m_sServerName = %s,mac = %s,ip = %s,tv_sec = %d\n", m_sServerName, mac, ip, tv_sec);

		if (ret == 0)
		{
			snprintf(m_sDomainName, sizeof(m_sDomainName), "%s", mac);
			snprintf(m_sDev, sizeof(m_sDev), "%s", sznetmode);
			snprintf(m_sIp, sizeof(m_sDev), "%s", ip);
			return DDNS_STATE_CONNECTED;
		}
		else
		{
			m_sDomainName[0] = 0;
			return DDNS_STATE_CONNECT_FAILED;
		}
	}

	if (m_sDomainName[0] == 0)
	{
		NET_WARN("[IDDNS:%s] DomainName is empty", __FUNCTION__);
		return DDNS_STATE_CONNECT_FAILED;
	}

	if (m_sUserName[0] == 0)
	{
		NET_WARN("[IDDNS:%s] UserName is empty", __FUNCTION__);
		return DDNS_STATE_CONNECT_FAILED;
	}
	if (m_sPassword[0] == 0)
	{
		NET_WARN("[IDDNS:%s] Password is empty", __FUNCTION__);
		return DDNS_STATE_CONNECT_FAILED;
	}

	memset(ip, 0, sizeof (ip));
	if (0 == get_current_extern_ip(ip,sizeof(ip)))
	{
		snprintf(m_sCmdString, sizeof(m_sCmdString), "/mnt/app/bin/ez-ipupdate -i %s -h %s -S %s -s %s -u %s:%s -t 10 -a %s",
				sznetmode, m_sDomainName, m_ddns_info[m_iIndex].strProtocolName, m_ddns_info[m_iIndex].strServerName, m_sUserName, m_sPassword, ip);
	}
	else
	{
		return DDNS_STATE_CONNECT_FAILED;//若获取外网IP失败，则返回
	}

	NET_DBG("m_sCmdString = %s\n", m_sCmdString);

	result = mysystem(m_sCmdString);
	if (result)
	{
		NET_WARN("[IDDNS:%s]mysystem<%s> end return: %d\n", __FUNCTION__, m_sCmdString, result);
		//  return DDNS_STATE_CONNECT_FAILED;
	}

	FILE *fw;
	char c = 0;

	fw = fopen("/tmp/.ez_ipresult", "r");
	if (fw != NULL)
	{
		fread(&c, 1, 1, fw);
		fclose(fw);
	}
	if (c == '1')
	{						//成功
		NET_WARN("[IDDNS:%s] success\n", __FUNCTION__);

		// get_current_extern_ip((char *) ip,sizeof(ip));
		snprintf(m_sDev, sizeof(m_sDev), "%s", sznetmode);
		snprintf(m_sIp, sizeof(m_sIp), "%s", ip);

		return DDNS_STATE_CONNECTED;
	}
	else
	{						//失败
		NET_WARN("[IDDNS:%s]failed\n", __FUNCTION__);
		return DDNS_STATE_CONNECT_FAILED;
	}
}

//##ModelId=49BDB4E601B7
IDDNS *IDDNS::GetInstance()
{
	if (m_Instance == 0)
	{
		m_Instance = new IDDNS();
		if (m_Instance != NULL)
		{
			m_Instance->Init();
		}
	}

	return m_Instance;
}

//##ModelId=49BDB7CE01D5
void IDDNS::Release()
{
	if (m_Instance != 0)
	{
		delete m_Instance;

		m_Instance = 0;
	}
}
int IDDNS::DDNS_SetDevName(int num)
{
	return 0;
}
char * IDDNS::DDNS_ReGetNetDevName()
{
	return m_pCConfig->GetValue(DDNS_CfgSection ,"NetDevName", "eth0",m_NetDevName,sizeof(m_NetDevName));
}
int IDDNS::DDNS_GetNetDevName(void)
{
	return 0;
}
