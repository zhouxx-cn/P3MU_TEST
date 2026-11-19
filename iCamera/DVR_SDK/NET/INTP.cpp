// Copyright (C) 1991 - 1999 Rational Software Corporation

//#include "stdafx.h"
#include "INTP.h"
#include<sys/time.h>
#include <signal.h>

#include "NET_global.h"
#include "CConfig.h"
// #include "sys_banned.h"
#include "global_gather.h"

static int m_bUpdateFlag;

static void *Fnx_CheckThread(void *lParam);
static void ntp_Updata_callback(int value);

//##ModelId=49B634E7020F
INTP::INTP()
{
	m_ntpUpdateThread = 0;
	m_bUpdateFlag = 0;
	//获取配置实例
	m_pCConfig = CConfig::GetInstance();
	if (NULL == m_pCConfig)
	{
		NET_ERR("[INTP:%s]NULL==m_pCConfig\n", __FUNCTION__);

		return;
	}
}

//!\brief 初始化
int INTP::Init()
{
	int ret;
printf("------INTP::Init()------\n");
	//检测是否已经初始化
	if (NULL != (void *) m_ntpUpdateThread)
	{
		//  NET_ERR("[IDDNS:%s]inited\n",__FUNCTION__);
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
	
	NTP_Start();

	m_bCheck_exit = 0;
	//创建注册检测线程
	ret = pthread_create(&m_ntpUpdateThread, NULL, Fnx_CheckThread, this);
	if (ret < 0)
	{
		NET_ERR("[INTP:%s]Create thread failed\n", __FUNCTION__);
		UnInit();
		return -1;
	}

	return 0;
}

//!\brief 释放资源
int INTP::UnInit()
{
	//结束注册检测线程
	m_bCheck_exit = 1;
	NET_INFO("[INTP:%s] exit...\n", __FUNCTION__);

	if (0 != m_ntpUpdateThread)
	{
		NET_DBG("[INTP:%s] cancel...\n", __FUNCTION__);
		pthread_cancel(m_ntpUpdateThread);
		NET_DBG("[INTP:%s] wait exit...\n", __FUNCTION__);
		pthread_join(m_ntpUpdateThread, NULL);
		NET_DBG("[INTP:%s] ok...\n", __FUNCTION__);
		m_ntpUpdateThread = 0;
	}

	NET_INFO("[INTP:%s] ok all...\n", __FUNCTION__);	
	return 0;
}


//##ModelId=49B634E70210
INTP::~INTP()
{
	// TODO: Add your specialized code here.
}

int INTP::LoadParam()
{
	m_bMode = NTP_GetMode();
	m_bNTPEnable = NTP_GetEnable();
	NTP_GetServerDomainName(m_bDomainName);
	m_bIntervalSec = NTP_GetAutoUpdateTime();

	return 0;
}

//!\brief 设置模式 一般/高级
//!\param nMode 0表示一般模式,1表示高级模式
//!\return 0表示设置成功,-1表示失败
//##ModelId=49B5E6CC0128
int INTP::NTP_SetMode(int nMode)
{
	m_pCConfig->SetValue(INTP_CfgSection, "Mode", nMode != 0);
	return 0;
}

//!\brief 获取模式 
//!\return 0表示一般模式，1表示高级模式,-1表示失败
//##ModelId=49B5E6CC0128
int INTP::NTP_GetMode()
{
	return m_pCConfig->GetValue(INTP_CfgSection, "Mode", 0);
}

//##ModelId=49B634E70211
int INTP::NTP_GetEnable()
{
	return m_pCConfig->GetValue(INTP_CfgSection, "Enable", 0);
}

//##ModelId=49B634E70212
int INTP::NTP_SetEnable(int bEnable)
{
	m_pCConfig->SetValue(INTP_CfgSection, "Enable", (bEnable != 0));
	//未使能
	if (m_bNTPEnable)
	{
		//正在启动
		m_iNTP_status = NTP_STATE_CONNECTING;
		m_bUpdateFlag = 1;

		if(m_bIntervalSec)
		{
			struct itimerval st_new = { {m_bIntervalSec, 0}, {m_bIntervalSec, 0} }, st_old;
			
			signal(SIGALRM, &ntp_Updata_callback);
			setitimer(ITIMER_REAL, &st_new, &st_old);
		}
	}
	else
		{
			if(m_bIntervalSec)
			{
				struct itimerval st_new = { {0, 0}, {0, 0} }, st_old;
				
				signal(SIGALRM, &ntp_Updata_callback);
				setitimer(ITIMER_REAL, &st_new, &st_old);
			}
			m_bUpdateFlag = 0;
			
			//正在停止
			m_iNTP_status = NTP_STATE_STOP;

		}
	
	return 0;
}

//##ModelId=49B634E70213
char *INTP::NTP_GetServerDomainName(char *pszDomainName)
{
	if (NULL == pszDomainName)
	{
		NET_ERR("[%s] NULL == pszDomainName \n", __FUNCTION__);
		return NULL;
	}
	m_pCConfig->GetValue(INTP_CfgSection, "DomainName", "", pszDomainName, NTP_MAX_DOMAINNAME_LEN);
	return pszDomainName;
}

//时间服务器
//time-a.nist.gov 
//time-b.nist.gov
//rdate.darkorb.net
//202.106.196.19
//time-b.timefreq.bldrdoc.gov
//##ModelId=49B634E70214
//210.72.145.44(中国国家授时中心服务器IP地址)
//ntp.sjtu.edu.cn (上海交通大学网络中心NTP服务器地址）
//ntp.fudan.edu.cn (复旦) （国内用户推荐使用此服务器） 
//timekeeper.isi.edu 
//subitaneous.cpsc.ucalgary.ca 
//usno.pa-x.dec.com 
//time.twc.weather.com 
//swisstime.ethz.ch 
//ntp0.fau.de 
//ntp3.fau.de 
//time-nw.nist.gov 
//nist1-sj.glassey.com

int INTP::NTP_SetServerDomainName(const char *pszDomainName)
{
	if (NULL == pszDomainName)
	{
		NET_ERR("[%s] NULL == pszDomainName \n", __FUNCTION__);
		return -1;
	}
	m_pCConfig->SetValue(INTP_CfgSection, "DomainName", pszDomainName);
	return 0;
}

//##ModelId=49B634E70215
int INTP::NTP_GetAutoUpdateTime()
{
	int nIntervalSecond;
	
	nIntervalSecond = m_pCConfig->GetValue(INTP_CfgSection, "IntervalSec", 1);
	if(nIntervalSecond * 60 < NTP_UPDATE_INTERVAL_DECOND_MIN)
	{
		NTP_SetAutoUpdateTime(NTP_UPDATE_INTERVAL_DECOND_MIN);
		return NTP_UPDATE_INTERVAL_DECOND_MIN;
	}
	
	return nIntervalSecond;
}

//##ModelId=49B634E70216
int INTP::NTP_SetAutoUpdateTime(int nIntervalSecond)
{
	if(nIntervalSecond * 60 < NTP_UPDATE_INTERVAL_DECOND_MIN)
		m_pCConfig->SetValue(INTP_CfgSection, "IntervalSec", NTP_UPDATE_INTERVAL_DECOND_MIN);
	else
		m_pCConfig->SetValue(INTP_CfgSection, "IntervalSec", nIntervalSecond);

	return 0;
}

int INTP::NTP_Start()
{
	//未使能
	if (!m_bNTPEnable)
	{
		return 0;
	}
	
	//正在启动
	m_iNTP_status = NTP_STATE_CONNECTING;

	m_bStart = 1;
	m_bUpdateFlag = 1;

	if(m_bIntervalSec)
	{
		struct itimerval st_new = { {m_bIntervalSec, 0}, {m_bIntervalSec, 0} }, st_old;
		
		signal(SIGALRM, &ntp_Updata_callback);
		setitimer(ITIMER_REAL, &st_new, &st_old);
	}

	return 0;
}

//##ModelId=49B634E7026F
int INTP::NTP_Stop()
{
	if(m_bStart && m_bIntervalSec)
	{
		struct itimerval st_new = { {0, 0}, {0, 0} }, st_old;
		
		signal(SIGALRM, &ntp_Updata_callback);
		setitimer(ITIMER_REAL, &st_new, &st_old);
	}

	m_bStart = 0;
	m_bStop = 1;
	m_bUpdateFlag = 0;

	//正在停止
	m_iNTP_status = NTP_STATE_STOP;
	return 0;
}

int INTP::NTP_CheckStatus()
{	
	if (m_bCheck_exit)
	{
		NET_ERR("[INTP:%s]stop check status \n", __FUNCTION__);
		return -1;
	}

	if (!m_bNTPEnable)
	{							//未使能
		m_iNTP_status = NTP_STATE_STOP;
		return 0;
	}

	return 0;					//m_iDDNS_status;
}

//##ModelId=49BDD33C009D
INTP *INTP::instance = 0;

//##ModelId=49BDD33C00CE
INTP *INTP::Instance()
{
	if (instance == 0)
	{
		instance = new INTP();
	}
	return instance;
}

//##ModelId=49BDE50E018C
void INTP::Release()
{
	if (instance != 0)
	{
		delete instance;

		instance = 0;
	}
}

static void *Fnx_CheckThread(void *lParam)
{
    PRT_TID();

	int ret = 0;

	INTP *pNTP = (INTP *) lParam;

	if (NULL == pNTP)
	{
		NET_ERR("[INTP:%s]NULL==pnet\n", __FUNCTION__);
		return NULL;
	}

	while (1)
	{

		ret = pNTP->NTP_CheckStatus();

		if (ret == -1)			//线程退出
		{
			NET_ERR("[IDDNS:%s]thread exit...\n", __FUNCTION__);
			break;
		}

		sleep(1);				//一秒检测一次
	}
	return NULL;
}


void ntp_Updata_callback(int value)
{
	m_bUpdateFlag = 1;
	signal(SIGALRM, &ntp_Updata_callback);
}

