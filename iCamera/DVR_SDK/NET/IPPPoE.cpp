// Copyright (C) 1991 - 1999 Rational Software Corporation

//#include <iostream>
#include <net/if.h>
#include <sys/ioctl.h>
#include <net/route.h>
#include <linux/sockios.h>
#include <netdb.h>
#include <netinet/if_ether.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <arpa/nameser.h>
#include <resolv.h>

#include "CConfig.h"
#include "ip.h"
#include "adsl.h"
#ifdef USE_HOSTS_PARSER
#include "newhie/ZWHosts.h"
#endif
#include "IPPPoE.h"

/* 消除本文件中的类型转换警告 */
#pragma GCC diagnostic ignored "-Wcast-qual"

#define NIPQUAD(addr) \
    ((unsigned char *)&addr)[0], \
    ((unsigned char *)&addr)[1], \
    ((unsigned char *)&addr)[2], \
    ((unsigned char *)&addr)[3]

//缺省值
static const int IPPPOE_Enable_Default_i = 0;
static const int PPPOE_IPQUADSIZ = (16);

//线程函数
static void *Fnx_CheckThread(void *data);

//##ModelId=49BDB4DC0132
IPPPoE *IPPPoE::m_pInstance = 0;

//##ModelId=49BB403B02F3
IPPPoE::IPPPoE()
{
	m_sUserName[0] = 0;
	m_sPassword[0] = 0;
	m_sPPPOE_IP[0] = 0;
	m_bIPPPOE_Enable = 0;
	m_bConnected = 0;
	m_FxnCallBack = NULL;
	m_hCheckThread = (pthread_t) NULL;
	m_bCheck_exit = 0;
	m_bConnected_old = -1;
	m_pCConfig = NULL;

	m_bConnOpt = 0;
	m_bDisconnOpt = 0;
    m_autoObtainDNS = 0;
}

//##ModelId=49BB403B02F4
IPPPoE::~IPPPoE()
{
	UnInit();
}

//!\brief 初始化
int IPPPoE::Init()
{
	int ret;

	//检测是否已经初始化
	if (NULL != (void *) m_hCheckThread)
	{
		//  TRACE_ERR("[IPPPoE:%s]inited\n",__FUNCTION__);
		return 0;
	}

	//获取配置实例
	m_pCConfig = CConfig::GetInstance();
	if (NULL == m_pCConfig)
	{
		NET_ERR("[IPPPoE:%s] NULL==m_pCConfig\n", __FUNCTION__);
		UnInit();
		return -1;
	}

	m_autoObtainDNS = m_pCConfig->GetValue("INet_Cfg", "DHCP_Enable", 1);

	m_bConnOpt = 0;
	m_bDisconnOpt = 0;
	//导入配置
	LoadParam();

	m_bConnected_old = -1;

	m_bCheck_exit = 0;

	//创建注册检测线程
	ret = pthread_create(&m_hCheckThread, NULL, Fnx_CheckThread, this);
	if (ret < 0)
	{
		NET_ERR("[IPPPoE:%s]Create thread failed\n", __FUNCTION__);
		UnInit();
		return -1;
	}

	return 0;
}

//!\brief 释放资源
int IPPPoE::UnInit()
{
	//结束注册检测线程
	m_bCheck_exit = 1;
	if (0 != m_hCheckThread)
	{
		pthread_cancel(m_hCheckThread);
		pthread_join(m_hCheckThread, NULL);
		m_hCheckThread = 0;
	}
	m_FxnCallBack = NULL;
	m_bIPPPOE_Enable = 0;
	m_bConnected_old = -1;
	return 0;

}

int IPPPoE::LoadParam()
{
	int result;

    NET_DBG("\n...IPPPoE.cpp....LoadParam()..........\n");
	//使能
	m_bIPPPOE_Enable = m_pCConfig->GetValue(IPPPOE_CfgSection, IPPPOE_Enable_key, 0);
	
    NET_DBG("\n..1..IPPPoE.cpp....LoadParam()....m_sUserName=%s....m_sPassword=%s..\n",m_sUserName,m_sPassword);
	//用户名与密码
	m_pCConfig->GetValue(IPPPOE_CfgSection, IPPPOE_UserName_key, IPPPOE_UserName_Default_s, m_sUserName, PPPOE_USERNAME_LEN);
	m_pCConfig->GetValue(IPPPOE_CfgSection, IPPPOE_Password_key, IPPPOE_Password_Default_s, m_sPassword, PPPOE_PASSWORD_LEN);
	NET_DBG("\n..2..IPPPoE.cpp....LoadParam()....m_sUserName=%s....m_sPassword=%s..\n",m_sUserName,m_sPassword);
	//设置
	result = EDVR_ADSL_SetLoginInfo(m_sUserName, m_sPassword);
	if (result)
	{
		NET_ERR("[IPPPoE:%s]SetLoginInfo failed!\n", __FUNCTION__);
	}

	return 0;
}

//##ModelId=49BB403B02F5
int IPPPoE::PPPoE_GetEnable()
{
	return m_bIPPPOE_Enable;
}


//##ModelId=49BB403B02F6
int IPPPoE::PPPoE_SetEnable(int bEnable)
{
	if ((m_bIPPPOE_Enable == 1) && (bEnable == 0))
	{
		m_bConnected = 3;
		//断开操作
		NET_WARN("[IPPPoE:%s]pppoe-stop ......\n", __FUNCTION__);
		EDVR_ADSL_Disconnect();
		//清除IP
		memset(m_sPPPOE_IP, 0, 16);
	}

	m_bIPPPOE_Enable = bEnable;
	if (m_pCConfig->SetValue(IPPPOE_CfgSection, IPPPOE_Enable_key, m_bIPPPOE_Enable) < 0)
	{
		NET_ERR("[IPPPoE:%s]SetValue failed\n", __FUNCTION__);
		return -1;
	}
	return 0;
}

//##ModelId=49BB403B02FD
char *IPPPoE::PPPoE_GetUserName(char *pszUserName)
{
	if (NULL == pszUserName)
	{
		NET_ERR("[IPPPoE:%s]NULL==pszUserName\n", __FUNCTION__);
		return NULL;
	}
	strncpy(pszUserName, m_sUserName, PPPOE_USERNAME_LEN - 1);
	return pszUserName;
}

//##ModelId=49BB403B02FF
int IPPPoE::PPPoE_SetUserName(const char *pszUserName)
{
	int result;

	if (NULL == pszUserName)
	{
		NET_ERR("[IPPPoE:%s]NULL==pszUserName\n", __FUNCTION__);
		return -1;
	}
	strncpy(m_sUserName, pszUserName, PPPOE_USERNAME_LEN - 1);
	m_pCConfig->SetValue(IPPPOE_CfgSection, IPPPOE_UserName_key, m_sUserName);
	result = EDVR_ADSL_SetLoginInfo(m_sUserName, m_sPassword);
	if (result)
	{
		NET_ERR("[IPPPoE:%s]EDVR_ADSL_SetLoginInfo return %d\n", __FUNCTION__, result);
		return -1;
	}

	return 0;

}

//##ModelId=49BB403B0301
char *IPPPoE::PPPoE_GetPassword(char *pszPassword)
{
	if (NULL == pszPassword)
	{
		NET_ERR("[IPPPoE:%s]NULL==pszPassword\n", __FUNCTION__);
		return NULL;
	}

	strncpy(pszPassword, m_sPassword, PPPOE_PASSWORD_LEN - 1);
	return pszPassword;
}

//##ModelId=49BB403B0303
int IPPPoE::PPPoE_SetPassword(const char *pszPassword)
{
	int result;

	if (NULL == pszPassword)
	{
		NET_ERR("[IPPPoE:%s]NULL==pszPassword\n", __FUNCTION__);
		return -1;
	}
	strncpy(m_sPassword, pszPassword, PPPOE_PASSWORD_LEN - 1);
	m_pCConfig->SetValue(IPPPOE_CfgSection, IPPPOE_Password_key, m_sPassword);
	result = EDVR_ADSL_SetLoginInfo(m_sUserName, m_sPassword);
	if (result)
	{
		NET_ERR("[IPPPoE:%s]EDVR_ADSL_SetLoginInfo return %d\n", __FUNCTION__, result);
		return -1;
	}

	return 0;
}

//##ModelId=49BB403B0307
int IPPPoE::PPPoE_Connect()
{
//  int ret;

	if (!m_bIPPPOE_Enable)
	{
		NET_ERR("[IPPPoE:%s]m_bIPPPOE_Enable=0\n", __FUNCTION__);
		return -1;
	}
	if (m_bConnected != 0)
	{
		NET_ERR("[IPPPoE:%s]status=%d\n", __FUNCTION__, m_bConnected);
		return -1;
	}

	//置连接
	m_bConnOpt = 1;

	return 0;
}

//##ModelId=49BB403B0308
int IPPPoE::PPPoE_Disconnect()
{
//  int ret;

	//已经是断开情况下，不用再断开
	if (m_bConnected == 3)
	{
		NET_WARN("[IPPPoE:%s] m_bConnected=%d..\n", __FUNCTION__, m_bConnected);
		return 0;
	}

	//置断开
	m_bDisconnOpt = 1;
	m_bConnOpt = 0;
	return 0;
}

//##ModelId=49BB403B0309
int IPPPoE::PPPoE_IsConnected()
{
	return m_bConnected;
}

//!\brief 连接状态
//!\return 1-连接，0断开
int IPPPoE::CheckConnect()
{
//  int bconn;
	int result;

	FILE *fp;

	char ip[20];

	char buf[100];

	char *pstr, cc;

//  int len;
	int i, j;

	int bget;
	if (!m_bIPPPOE_Enable)
	{
		return 0;
	}
	result = mysystem("pppoe-status >/var/pppoe_status");
	if (result)
	{
		NET_WARN("[IPPPoE:%s]pppoe-status return %d\n", __FUNCTION__, result);
		return 0;
	}
	fp = fopen("/var/pppoe_status", "r");
	if (NULL == fp)
	{
		NET_WARN("[IPPPoE:%s]open file failed\n", __FUNCTION__);
		return 0;
	}

	pstr = fgets(buf, 100, fp);
	if (pstr == NULL)
	{
		NET_WARN("[IPPPoE:%s]pstr == NULL\n", __FUNCTION__);
		fclose(fp);
		return 0;
	}
	pstr = fgets(buf, 100, fp);
	if (pstr == NULL)
	{
		NET_WARN("[IPPPoE:%s]pstr == NULL\n", __FUNCTION__);
		fclose(fp);
		return 0;
	}
	pstr = fgets(buf, 100, fp);
	if (pstr == NULL)
	{
		NET_WARN("[IPPPoE:%s]pstr == NULL\n", __FUNCTION__);
		fclose(fp);
		return 0;
	}
	fclose(fp);
	i = 0;
	bget = 0;
	j = 0;
	while (1)
	{
		cc = buf[i++];
		if (cc == '\0')
		{
			break;
		}
		if (cc == 'P' && buf[i] == '-')
		{
			bget = 1;
			i += 5;
			continue;
		}
		if (bget && cc == ' ')
		{
			break;
		}
		if (bget)
		{
			ip[j++] = cc;
		}
	}
	ip[j] = 0;//将Buf【i+5】之后到' '结束的字符串保存到IP[]中
	if ((in_addr_t) - 1 == inet_addr(ip))
	{
		NET_WARN("[IPPPoE:%s]invalid ip =%s\n", __FUNCTION__, ip);
		return 0;
	}
	snprintf(buf, sizeof(buf), "ping -q -c 1 %s > NULL", ip);//-q为开始结束时候打印重要信息，之后的 EMSG msg;中获取相关信息

	result = mysystem(buf);
	if (!result)
	{
		return 1;
	}
	result = mysystem(buf);
	if (!result)
	{
		return 1;
	}
	result = mysystem(buf);
	if (!result)
	{
		return 1;
	}
	return 0;

}

//##ModelId=49BB403B030A
char *IPPPoE::PPPoE_GetIPAddress(char *pszIPAddress)
{
	int ret;

	if (NULL == pszIPAddress)
	{
		NET_ERR("[IPPPoE:%s]NULL==pszIPAddress\n", __FUNCTION__);
		return NULL;
	}
	ret = get_if_address((char *) IFNAME_PPPOE, pszIPAddress, 20, GET_ADDRESS);
	if (ret)
	{
		NET_ERR("[IPPPoE:%s]GetIP failed(%d)\n", __FUNCTION__, ret);
		return NULL;
	}
	return pszIPAddress;
}

//##ModelId=49BB403B030C
char *IPPPoE::PPPoE_GetSubnetMask(char *pszSubnetMask)
{
	int ret;

	if (NULL == pszSubnetMask)
	{
		NET_ERR("[IPPPoE:%s]NULL==pszSubnetMask\n", __FUNCTION__);
		return NULL;
	}
	ret = get_if_address((char *) IFNAME_PPPOE, pszSubnetMask, 20, GET_NETMASK);
	if (ret)
	{
		NET_ERR("[IPPPoE:%s]Getsubnetmask failed(%d)\n", __FUNCTION__, ret);
		return NULL;
	}
	return pszSubnetMask;
}

//##ModelId=49BB403B030E
char *IPPPoE::PPPoE_GetGateway(char *pszGateway)
{
	int ret;

	if (NULL == pszGateway)
	{
		NET_ERR("[IPPPoE:%s]NULL==pszGateway\n", __FUNCTION__);
		return NULL;
	}

	ret = get_if_address((char *) IFNAME_PPPOE, pszGateway, 20, GET_GATEWAY);
	if (ret)
	{
		NET_ERR("[IPPPoE:%s]Get Gateway failed(%d)\n", __FUNCTION__, ret);
		return NULL;
	}
	return pszGateway;
}
//将一个点分十进制的IP转换成一个长整数型数
int IPPPoE::IsValidIP(const char *ipaddr)
{
	unsigned int ip;

	ip = inet_addr(ipaddr);
	if ((unsigned int) -1 == ip || 0 == ip)
	{
		return 0;
	}
	return 1;
}

//!\brief   设置DNS地址
//!\param   char * PrimaryDNS, char * SecondaryDNS  主备DNS地址
//!\return  0成功，其它失败
//##ModelId=49BB403B030E
int IPPPoE::PPPoE_SetDNS(char *PrimaryDNS, char *SecondaryDNS)
{
	// 不做处理, dns 设置都放在inet.cpp 文件里面
	return 0;

	int result;

	if (NULL == PrimaryDNS)
	{
		NET_ERR("NULL == PrimaryDNS\n");
		return -1;
	}
	if (!IsValidIP(PrimaryDNS))
	{
		NET_ERR("invalid PrimaryDNS = %s\n", PrimaryDNS);
		return -2;
	}
	if (NULL != SecondaryDNS && (!IsValidIP(SecondaryDNS)))
	{
		NET_ERR("invalid SecondaryDNS = %s\n", SecondaryDNS);
		return -3;
	}

	result = EDVR_ADSL_SetDNS(PrimaryDNS, SecondaryDNS);
	if (0 != result)
	{
		NET_ERR("EDVR_ADSL_SetDNS failed(%d) PrimaryDNS = %s SecondaryDNS = %s\n", result, PrimaryDNS, SecondaryDNS);
		return -4;
	}

    //设备更新网络后，如果路由器的网段不一致，会引起域名解析失效    
    res_init();

#ifdef USE_HOSTS_PARSER
    result = Hosts_Update();
    if (result)
    {
        NET_ERR("Fail to update the hosts, errno = %d\n", result);
    }
#endif

	/*DNS改变后调用res_init 防止域名解析失败*/
	res_init();
	return 0;
}

//该字段对我们客户端没有实际意义，只是为了提高onvif test工具通过率将该字段保存起来
int IPPPoE::PPPoE_SetAutoObtainDNS(unsigned long autoObtainDNS)
{
	m_autoObtainDNS = autoObtainDNS;

	return 0;
}

int IPPPoE::PPPoE_GetAutoObtainDNS(unsigned long *autoObtainDNS)
{
	*autoObtainDNS = m_autoObtainDNS;

	return 0;
}

int IPPPoE::PPPoE_ReActive()
{
	//断开连接，如果有的话
	PPPoE_Disconnect();
	//重连接
	return PPPoE_Connect();
}

static char StrConnStatus[][20] = { "Disconnect", "Connecting", "Connected", "Disconnecting" };

//##ModelId=49BDC7D1019D
int IPPPoE::PPPoE_SetNotifyCallback(MSG_CALLBACK pCallback)
{
	//不管是否为NULL，均认为是有效的
	m_FxnCallBack = pCallback;
	return 0;
}

int IPPPoE::PPPoE_CheckStatus()
{
	int bconn;

	int ret;
	if (m_bCheck_exit)//m_bCheck_exit!=0 状态线程退出
	{
		if (m_bConnected == 2 || m_bConnected == 1)
		{
			NET_WARN("[IPPPoE:%s]stop pppoe\n", __FUNCTION__);
			EDVR_ADSL_Disconnect();//mysystem("pppoe-stop > /dev/null")
		}
		NET_WARN("[IPPPoE:%s]stop check status \n", __FUNCTION__);
		return -1;
	}

	//如果需要断开，且状态为连接或者是正在连接时，则断开
	if (m_bDisconnOpt)
	{
	    NET_DBG("\n...IPPPoE.cpp....PPPoE_CheckStatus().......如果需要断开，且状态为连接或者是正在连接时，则断开.....\n");
		//置状态为 正在断开
		m_bConnected = 3;
		//断开操作
		NET_WARN("[IPPPoE:%s]pppoe-stop ......\n", __FUNCTION__);
		EDVR_ADSL_Disconnect();
		//清除IP
		memset(m_sPPPOE_IP, 0, 16);
		NET_WARN("[IPPPoE:%s]pppoe-stop done......\n", __FUNCTION__);
	}

	//如果需要连接，且状态为断开或者是正在断开时，则连接
	if (m_bConnOpt && (m_bConnected == 3 || m_bConnected == 0))
	{
	    NET_DBG("\n...IPPPoE.cpp....PPPoE_CheckStatus()...begin.....连接操作......\n");
		//置状态为  正在连接
		m_bConnected = 1;
		//连接操作
		NET_WARN("[IPPPoE:%s]pppoe-start ......\n", __FUNCTION__);
		ret = EDVR_ADSL_Connect();
		if (!ret)
		{						//成功
			m_bConnected = 2;
		}
		else
		{
			m_bConnected = 0;

		}

		//清除IP
		memset(m_sPPPOE_IP, 0, 16);
		NET_WARN("[IPPPoE:%s]pppoe-start done.....\n", __FUNCTION__);
	}

	m_bConnOpt = 0;
	m_bDisconnOpt = 0;

	if (m_bIPPPOE_Enable)
	{
#if 0
		//如果能获得正确IP，则可认为是连接的
		if (0 == EDVR_ADSL_GetADSLStatus(m_sPPPOE_IP))
		{
			if (strcmp(m_sPPPOE_IP, "") != 0)
			{
				if (m_bConnected == 1 || m_bConnected == 0)	//正在连接或者没有连接，则现在可以认为是成功了
				{
					m_bConnected = 2;	//连接成功
				}
			}
			else if (m_bConnected == 2 || m_bConnected == 3)	//前一状态是连接的或者正在挂断，现在认为是断开了
			{
				m_bConnected = 0;
			}

		}
		else if (m_bConnected == 2 || m_bConnected == 3)	//前一状态是连接的或者正在挂断，现在认为是断开了
		{
			m_bConnected = 0;
		}
#else
		if (CheckConnect())
		{
			if (m_bConnected == 1 || m_bConnected == 0)	//正在连接或者没有连接，则现在可以认为是成功了
			{
				m_bConnected = 2;	//连接成功
			}

		}
		else if (m_bConnected == 2 || m_bConnected == 3)	//前一状态是连接的或者正在挂断，现在认为是断开了
		{
			m_bConnected = 0;
		}
#endif
	}
	else
	{							//如果没有使能,自然可认为是断开的,如果状态不为断开，则断开
		if (m_bConnected == 2 || m_bConnected == 1)
		{
			PPPoE_Disconnect();
		}
	}

	bconn = (m_bConnected == 2);

	//如果状态改变，则回调通知
	if (m_bConnected_old != bconn)
	{
		NET_WARN("[IPPPoE:%s]callback %s\n", __FUNCTION__, StrConnStatus[m_bConnected]);

		m_bConnected_old = bconn;
		if (NULL != m_FxnCallBack)
		{
			EMSG msg;

			msg.nMsg = MSG_PPPOE_LOCAL_NAME;
			msg.nParam1 = bconn;
			m_FxnCallBack(&msg, this);
		}
	}
	//如果是断开且使能，则重连
	if (m_bConnected == 0 && m_bIPPPOE_Enable)
	{
		NET_WARN("[IPPPoE:%s]Reconnect....\n", __FUNCTION__);
		PPPoE_ReActive();
	}
	return m_bConnected;
}

static void *Fnx_CheckThread(void *lParam)
{
    PRT_TID_NAME("IPPPoECheckThread");

	int ret = 0;

	IPPPoE *pPppoe = (IPPPoE *) lParam;

	if (NULL == pPppoe)
	{
		NET_ERR("[IPPPoE:%s]NULL==pnet\n", __FUNCTION__);
		return NULL;
	}
	while (1)
	{

		ret = pPppoe->PPPoE_CheckStatus();

		if (ret == -1)			//线程退出
		{
			NET_WARN("[IPPPoE:%s]thread exit...\n", __FUNCTION__);
			break;
		}

		sleep(1);				//一秒检测一次
	}
	return NULL;
}


//##ModelId=49BDB4DC0163
IPPPoE *IPPPoE::GetInstance()
{
	if (m_pInstance == 0)
	{
		m_pInstance = new IPPPoE;
	}
	if (NULL != m_pInstance)
	{
		m_pInstance->Init();
	}
	return m_pInstance;//IPPPoE的函数指针
}

//##ModelId=49BDB7CE0149
void IPPPoE::Release()
{
	if (m_pInstance != 0)
	{
		delete m_pInstance;

		m_pInstance = 0;
	}
}
