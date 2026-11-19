// Copyright (C) 1991 - 1999 Rational Software Corporation
#if defined __USE_EMAIL__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/ipc.h>   
#include <sys/msg.h>   
#include <errno.h>   

#include "IEmail.h"
#include "NET_global.h"
#include "zwsdk.h"
#include "CConfig.h"

#include "msg_net.h"

//配置



#define EML_LOCK(x)		do {/* NET_DBG("lock\n");   */XTRACE_MUTEX_LOCK(x); } while (0)
#define EML_UNLOCK(x)	do {/* NET_DBG("unlock\n"); */XTRACE_MUTEX_UNLOCK(x); } while (0)

//!\brief   邮件发送线程，回调函数
//!\param   pContext    上下文指针
//!\return  0表示成功，其它为错误码
static void *callback_SendEmailThread(void *pContext);

//!\brief   计时器线程
//!\param   pContext    上下文指针
//!\return  0表示成功，其它为错误码
static void *TimeCntThread(void * pContext);

/* 字符串分割函数 */
#if 0
static int StringSplit(const char *pString, const char *pSeparator, char s[][256])
{
    if (NULL==pString || NULL==pSeparator)
    {
        printf("NULL POINTER EXCEPTION!\n");
        return -1;
    }
    
    char *tokens[100];
    char *saveptr = NULL;
#ifdef USE_MEMORY_POOL
	char *tmp = MEM_STRDUP(pString);
#else
	char *tmp = strdup(pString); // 此处使用了strdup，分配了多余的内存，故在后面需要释放该内存
#endif
    char *p = tmp;

    int i = 0;
    int j = 0;

    while((tokens[i]=strtok_r(tmp, pSeparator, &saveptr)) != NULL)
    {
        i ++;
        tmp = NULL;
    }

    for (j=0; j<i; j++)
    {
        strncpy(s[j], tokens[j], strlen(tokens[j]) - 1);
    }

    /* 释放strdup分配的内存 */
#ifdef USE_MEMORY_POOL
	MEM_FREE(p);
#else
	free(p);
#endif
    p = NULL;

    return i;
}
#endif

IEmail *IEmail::instance = 0;

IEmail::IEmail()
{
	int nSDKRet;
	int i = 0;

	NET_INFO("enter\n");
	m_bEnable = 1;			//默认启用Email功能
	memset(m_szSmtpAddress, 0, sizeof(m_szSmtpAddress));
	m_nSmtpPort = 25;		//默认Smtp端口为25
	m_nSmtpAuth = 1;		//默认需要身份验证
	m_nSmtpSecurityMode = 1;
	m_nSmtpSendTimeInterval = MINEMAILTIMEINTERVAL; //默认Smtp发送时间间隔为60秒
	m_nTestEmailstaue = 0;
	memset(m_szAddresserEmail, 0, sizeof(m_szAddresserEmail));
	memset(m_szAddresserPassword, 0, sizeof(m_szAddresserPassword));
	memset(m_sRecvAddresser, 0, 3 * NET_MAX_PASSWORD_LEN);
	memset(m_szSenderUserName, 0, sizeof(m_szSenderUserName));
	memset(m_szCCAddresserEmail, 0, sizeof(m_szCCAddresserEmail));
	memset(m_szBCCAddresserEmail, 0, sizeof(m_szBCCAddresserEmail));
	memset(m_sSubjectTopic, 0, sizeof(m_sSubjectTopic));
	memset(m_szEmailBody, 0, sizeof(m_szEmailBody));

    for (i=0;i<5;i++)
    {
        memset(m_sAttraAffix[i], 0 , sizeof(m_sAttraAffix[i]));
    }

	m_pCSmtp = CSmtp::GetInstance();
	if(m_pCSmtp == NULL)
	{
		NET_ERR("get m_pCSmtp's point error!!!\n");	//fan-2014-06-19
	}

	//从配置文件初始化
	CConfig *pConfig = CConfig::GetInstance();
	if (NULL != pConfig)
	{
		m_bEnable = pConfig->GetValue(NET_EMAIL_CFG, "Enable", 0);
		m_nSmtpSendTimeInterval = pConfig->GetValue(NET_EMAIL_CFG, "SendEMLTimeInterval", MINEMAILTIMEINTERVAL);

		if (m_nSmtpSendTimeInterval < MINEMAILTIMEINTERVAL)
    	{
        		m_nSmtpSendTimeInterval = MINEMAILTIMEINTERVAL;
    	}

		pConfig->GetValue(NET_EMAIL_CFG, "SmtpAddress", "", m_szSmtpAddress, NET_MAX_DOMAINNAME_LEN);
		m_nSmtpPort = pConfig->GetValue(NET_EMAIL_CFG, "SmtpPort", 25);
		m_nSmtpAuth = pConfig->GetValue(NET_EMAIL_CFG, "SmtpAuth", 1);
		m_nSmtpSecurityMode = pConfig->GetValue(NET_EMAIL_CFG, "SmtpSecurityMode", 1);
		m_bPic_att = pConfig->GetValue(NET_EMAIL_CFG, "Pic_att", 1);

		pConfig->GetValue(NET_EMAIL_CFG, "AddresserEmail", "", m_szAddresserEmail, NET_MAX_EMAIL_LEN);
		pConfig->GetValue(NET_EMAIL_CFG, "AddresserPassword", "", m_szAddresserPassword, NET_MAX_PASSWORD_LEN);
		pConfig->GetValue(NET_EMAIL_CFG, "RecvAddresser1", "", m_sRecvAddresser[0], NET_MAX_PASSWORD_LEN);
		pConfig->GetValue(NET_EMAIL_CFG, "RecvAddresser2", "", m_sRecvAddresser[1], NET_MAX_PASSWORD_LEN);
		pConfig->GetValue(NET_EMAIL_CFG, "RecvAddresser3", "", m_sRecvAddresser[2], NET_MAX_PASSWORD_LEN);

		pConfig->GetValue(NET_EMAIL_CFG, "SenderUserName", "", m_szSenderUserName, NET_MAX_USER_NAME_LEN);
        pConfig->GetValue(NET_EMAIL_CFG, "CCAddresserEmail", "", m_szCCAddresserEmail, NET_MAX_EMAIL_LEN);
        pConfig->GetValue(NET_EMAIL_CFG, "BCCAddresserEmail", "", m_szBCCAddresserEmail, NET_MAX_EMAIL_LEN);
        pConfig->GetValue(NET_EMAIL_CFG, "SubjectTopic", "", m_sSubjectTopic, NET_MAX_SUBJECT_LEN);
        pConfig->GetValue(NET_EMAIL_CFG, "SendBody", "", m_szEmailBody, NET_MAX_EMAIL_BODY_LEN);
			
	}
	else
	{
		//没有配置文件，也可以工作
		NET_ERR("no CConfig instance\n");
		NET_CHK(NULL != pConfig);
	}

    //计时器时间初始为0
    m_TimeCount = 0;

	//上次发送Email的时间
    m_LastSendEMLTime = 0;

	m_bSendEmailIsBusy = 0;

    //默认当前附件个数为0  
    m_nCurrAtrraCount = 0;

    //获取编码句柄
	m_hEnc = ZWSys_EncChannelOpen(0);

	//删除以前的正文文件
	mysystem("rm -f /tmp/app/mail_*");

	//发送Email状态
    m_nTestEmailSendStatus = -1;
	m_bTestEmailOverFlag=0;

    //当前发送邮件是否为测试邮件(0:触发邮件 1:测试邮件)
    m_bTestEmail = 0;

	//当前邮件编号
	m_nFirstMailIndex = 0;
	m_nLastMailIndex = 0;
	m_nTempMailIndex = 0;

	//创建互斥锁
	nSDKRet = XTRACE_MUTEX_INIT(&m_Mutex, NULL);
	if (0 != nSDKRet)
	{
		NET_ERR("XTRACE_MUTEX_INIT( m_Mutex ) return %d\n", nSDKRet);
		NET_CHK(0 == nSDKRet);
	}

	nSDKRet = XTRACE_MUTEX_INIT(&m_status_Mutex, NULL);
	if (0 != nSDKRet)
	{
		NET_ERR("XTRACE_MUTEX_INIT( m_status_Mutex ) return %d\n", nSDKRet);
		NET_CHK(0 == nSDKRet);
	}

	//创建浏览线程同步资源，初始化信号量
	nSDKRet = sem_init(&m_semExitThread, 0, 0);
	if (0 != nSDKRet)
	{
		NET_ERR("sem_init( m_semExitThread ) return %d\n", nSDKRet);
		NET_CHK(0 == nSDKRet);
	}

	nSDKRet = sem_init(&m_semWakeThread, 0, 0);
	if (0 != nSDKRet)
	{
		NET_ERR("sem_init( m_semWakeThread ) return %d\n", nSDKRet);
		NET_CHK(0 == nSDKRet);
	}

	//创建浏览线程
	nSDKRet = pthread_create(&m_thrdID, NULL, callback_SendEmailThread, this);
	if (0 != nSDKRet)
	{
		NET_ERR("pthread_create( m_thrdID ) return %d\n", nSDKRet);
		NET_CHK(0 == nSDKRet);
	}

    //创建计数器线程
	nSDKRet = pthread_create(&m_TimethrdID, NULL, TimeCntThread, this);
	if (0 != nSDKRet)
	{
		NET_ERR("pthread_create( m_TimethrdID ) return %d\n", nSDKRet);
		NET_CHK(0 == nSDKRet);
	}

	NET_INFO("out");
}

IEmail::~IEmail()
{
	int nSDKRet;

	NET_INFO("enter\n");

	//激活并退出邮件发送线程
	sem_post(&m_semExitThread);
	sem_post(&m_semWakeThread);

	//等待邮件发送线程退出完毕
	nSDKRet = pthread_join(m_thrdID, NULL);
	if (0 != nSDKRet)
	{
		NET_ERR("pthread_join( m_thrdID ) return %d\n", nSDKRet);
		NET_CHK(0 == nSDKRet);
	}

	//等待计数器线程退出完毕
	nSDKRet = pthread_join(m_TimethrdID, NULL);
	if (0 != nSDKRet)
	{
		NET_ERR("pthread_join( m_TimethrdID ) return %d\n", nSDKRet);
		NET_CHK(0 == nSDKRet);
	}

	//销毁邮件发送线程同步资源
	nSDKRet = sem_destroy(&m_semExitThread);
	if (0 != nSDKRet)
	{
		NET_ERR("sem_destroy( m_semExitThread ) return %d\n", nSDKRet);
		NET_CHK(0 == nSDKRet);
	}

	nSDKRet = sem_destroy(&m_semWakeThread);
	if (0 != nSDKRet)
	{
		NET_ERR("sem_destroy( m_semWakeThread ) return %d\n", nSDKRet);
		NET_CHK(0 == nSDKRet);
	}

	//销毁互斥锁
	nSDKRet = XTRACE_MUTEX_DESTORY(&m_Mutex);
	if (0 != nSDKRet)
	{
		NET_ERR("XTRACE_MUTEX_DESTORY( m_Mutex ) return %d\n", nSDKRet);
		NET_CHK(0 == nSDKRet);
	}

	//销毁互斥锁
	nSDKRet = XTRACE_MUTEX_DESTORY(&m_status_Mutex);
	if (0 != nSDKRet)
	{
		NET_ERR("XTRACE_MUTEX_DESTORY( m_status_Mutex ) return %d\n", nSDKRet);
		NET_CHK(0 == nSDKRet);
	}

	NET_INFO("out\n");
}

//!\brief       获取唯一实例
//!\return  实例指针
IEmail *IEmail::Instance()
{
	if (instance == 0)
	{
#ifdef USE_MEMORY_POOL
		instance = CommonKit::TCreateObject<IEmail>::CreateObject(__FILE__, __FUNCTION__, __LINE__);
#else
		instance = new IEmail();
#endif
	}

	return instance;
}

//!\brief       销毁实例
void IEmail::Release()
{
	if (instance != 0)
	{
#ifdef USE_MEMORY_POOL
		CommonKit::TDestroyObject<IEmail>::DestroyObject(instance);
#else
		delete instance;
#endif
		instance = 0;
	}
}


//!\brief       获取是否启用SMTP服务器
//!\return  1表示启用，0表示不启用
int IEmail::EML_GetEnable()
{
	return m_bEnable;
}

//!\brief       设置是否启用SMTP服务器
//!\param   bEnable 1表示启用，0表示禁用
//!\return  0表示成功，其它为错误码
int IEmail::EML_SetEnable(int bEnable)
{
	EML_LOCK(&m_Mutex);

	if (0 == bEnable)
	{
		m_bEnable = 0;
	}
	else
	{
		m_bEnable = 1;
	}

	//保存到配置文件
	CConfig *pConfig = CConfig::GetInstance();

	if (NULL != pConfig)
	{
		pConfig->SetValue(NET_EMAIL_CFG, "Enable", m_bEnable);
	}

	EML_UNLOCK(&m_Mutex);
	return 0;
}

//!\brief       获取SMTP服务器地址(域名或者IP地址)
//!\param   pszAddress  返回SMTP服务器地址
//!\param   nBufLen pszAddress指向的buf大小
//!\return  0表示成功，其它为错误码
int IEmail::EML_GetSmtpAddress(char *pszAddress, int nBufLen)
{
	if (NULL == pszAddress)
	{
		NET_ERR("NULL == pszAddress\n");
		return -1;
	}

	if (nBufLen <= 0)
	{
		NET_ERR("nBufLen(%d) <= 0\n", nBufLen);
		return -2;
	}

	if (nBufLen < NET_MAX_DOMAINNAME_LEN)
	{
		NET_INFO("buffer is small, nBufLen = %d, need %d\n", nBufLen, NET_MAX_DOMAINNAME_LEN);
	}

	EML_LOCK(&m_Mutex);

	strncpy(pszAddress, m_szSmtpAddress, nBufLen - 1);
	pszAddress[nBufLen - 1] = '\0';

	EML_UNLOCK(&m_Mutex);
	return 0;
}

//!\brief       设置SMTP服务器地址(域名或者IP地址)
//!\param   pszAddress  SMTP服务器地址
//!\return  0表示成功，其它为错误码
int IEmail::EML_SetSmtpAddress(const char *pszAddress)
{
	EML_LOCK(&m_Mutex);
//printf("\n---IEmail.cpp-----EML_SetSmtpAddress()---设置SMTP服务器地址-----pszAddress=%s-----\n",pszAddress);
	if (NULL == pszAddress)
	{
		if ('\0' == m_szSmtpAddress[0])
		{
			//没有变更
			EML_UNLOCK(&m_Mutex);
			return 0;
		}

		//输入NULL，则清除地址
		m_szSmtpAddress[0] = '\0';
	}
	else
	{
		int nBufLen = strlen(pszAddress);

		if (nBufLen <= 0)
		{
			if ('\0' == m_szSmtpAddress[0])
			{
				//没有变更
				EML_UNLOCK(&m_Mutex);
				return 0;
			}

			//输入空字符串则清除地址
			m_szSmtpAddress[0] = '\0';
		}
		else if (nBufLen >= NET_MAX_DOMAINNAME_LEN)
		{
			//输入的地址太长，无法保存
			NET_ERR("buffer is big, nBufLen = %d, need %d\n", nBufLen, NET_MAX_DOMAINNAME_LEN);

			EML_UNLOCK(&m_Mutex);
			return -1;
		}
		else if (0 == strncmp(m_szSmtpAddress, pszAddress, NET_MAX_DOMAINNAME_LEN - 1))
		{
			//没有变更
			EML_UNLOCK(&m_Mutex);
			return 0;
		}
		else
		{
			//保存
			snprintf(m_szSmtpAddress,sizeof(m_szSmtpAddress),"%s",pszAddress);
		}
	}

	//保存到配置文件
	CConfig *pConfig = CConfig::GetInstance();

	if (NULL != pConfig)
	{
		pConfig->SetValue(NET_EMAIL_CFG, "SmtpAddress", m_szSmtpAddress);
	}

	EML_UNLOCK(&m_Mutex);
	return 0;
}

//!\brief       获取SMTP服务器端口
//!\return  返回端口号
unsigned short IEmail::EML_GetSmtpPort()
{    
	return m_nSmtpPort;
}

//!\brief       设置SMTP服务器端口
//!\param   wPort   SMTP服务器端口
//!\return  0表示成功，其它为错误码
int IEmail::EML_SetSmtpPort(unsigned short wPort)
{
	EML_LOCK(&m_Mutex);
//printf("\n---IEmail.cpp-----EML_SetSmtpPort()----设置SMTP服务器端口----wPort=%d-----\n",wPort);
	if (m_nSmtpPort == wPort)
	{
		//端口没有改变
		EML_UNLOCK(&m_Mutex);
		return 0;
	}

	m_nSmtpPort = wPort;

	//保存到配置文件
	CConfig *pConfig = CConfig::GetInstance();

	if (NULL != pConfig)
	{
		pConfig->SetValue(NET_EMAIL_CFG, "SmtpPort", m_nSmtpPort);
	}

	EML_UNLOCK(&m_Mutex);
	return 0;
}

//!\brief       获取身份验证方式
//!\return  0表示none，1表示login
int IEmail::EML_GetSmtpAuth()
{
	return m_nSmtpAuth;
}

//!\brief       设置身份验证方式
//!\param   nAuth   0表示none，1表示login，2表示plain
//!\return  0表示成功，其它为错误码
int IEmail::EML_SetSmtpAuth(int nAuth)
{
	EML_LOCK(&m_Mutex);

	if ((nAuth < 0) || (nAuth > 2))
	{
		//默认值
		nAuth = 1;
	}

	if (m_nSmtpAuth == nAuth)
	{
		//验证方式没有改变
		EML_UNLOCK(&m_Mutex);
		return 0;
	}

	m_nSmtpAuth = nAuth;

	//保存到配置文件
	CConfig *pConfig = CConfig::GetInstance();

	if (NULL != pConfig)
	{
		pConfig->SetValue(NET_EMAIL_CFG, "SmtpAuth", m_nSmtpAuth);
	}

	EML_UNLOCK(&m_Mutex);
	return 0;
}

//!\brief   获取安全验证方式
//!\return  0表示none，1表示TLS，2表示SSL，3表示STARTTLS，4表示未设置 其它为错误码
int IEmail::EML_GetSecuritytMode()
{
	return m_nSmtpSecurityMode;
}

//!\brief   设置安全验证方式
//!\param   SecMode 0表示none，1表示TLS，2表示SSL，3表示STARTTLS，4表示未设置
//!\return  0表示成功，其它为错误码
int IEmail::EML_SetSecuritytMode(int SecMode)
{
    if (SecMode<0 || SecMode>3)
    {
        NET_ERR("Security is %d out of [0, 3]\n", SecMode);
        return -1;
    }

    EML_LOCK(&m_Mutex);
    
//printf("\n---IEmail.cpp-----EML_SetSecuritytMode()----设置SMTP安全模式----SecMode=%d-----\n",SecMode);
    if (m_nSmtpSecurityMode == SecMode)
    {
        //模式没有改变
        EML_UNLOCK(&m_Mutex);
        return 0;
    }

    m_nSmtpSecurityMode = SecMode;

    

    //保存到配置文件
    CConfig *pConfig = CConfig::GetInstance();
    if (NULL != pConfig)
    {
        pConfig->SetValue(NET_EMAIL_CFG, "SmtpSecurityMode", m_nSmtpSecurityMode);
    }

    EML_UNLOCK(&m_Mutex);

    return 0;
}

//!\brief       获取是否启用附加图片
//!\return  1表示启用，0表示禁用
int IEmail::EML_GetPic_att()
{
	return m_bPic_att;
}

//!\brief       设置是否启用附加图片
//!\param   bEnable 1表示启用，0表示禁用
//!\return  0表示成功，其它为错误码
int IEmail::EML_SetPic_att(int bEnable)
{
	EML_LOCK(&m_Mutex);

	if (0 != bEnable)
	{
		bEnable = 1;
	}

	if (m_bPic_att == bEnable)
	{
		EML_UNLOCK(&m_Mutex);
		return 0;
	}

	m_bPic_att = bEnable;


	//保存到配置文件
	CConfig *pConfig = CConfig::GetInstance();

	if (NULL != pConfig)
	{
		pConfig->SetValue(NET_EMAIL_CFG, "Pic_att", m_bPic_att);
	}
    
	EML_UNLOCK(&m_Mutex);
	return 0;
}

//!\brief       获取发件人Email
//!\param   pszAddress  返回发件人Email
//!\param   nBufLen pszEmail指向的buf大小
//!\return  0表示成功，其它为错误码
int IEmail::EML_GetAddresserEmail(char *pszEmail, int nBufLen)
{
	if (NULL == pszEmail)
	{
		NET_ERR("NULL == pszEmail\n");
		return -1;
	}

	if (nBufLen <= 0)
	{
		NET_ERR("nBufLen(%d) <= 0\n", nBufLen);
		return -2;
	}

	if (nBufLen < NET_MAX_EMAIL_LEN)
	{
		NET_INFO("buffer is small, nBufLen = %d, need %d\n", nBufLen, NET_MAX_EMAIL_LEN);
	}

	EML_LOCK(&m_Mutex);
	strncpy(pszEmail, m_szAddresserEmail, nBufLen - 1);
	pszEmail[nBufLen - 1] = '\0';

	EML_UNLOCK(&m_Mutex);
	return 0;
}

//!\brief       设置发件人Email
//!\param   pszEmail    发件人Email
//!\return  0表示成功，其它为错误码
int IEmail::EML_SetAddresserEmail(const char *pszEmail)
{
	EML_LOCK(&m_Mutex);

	if (NULL == pszEmail)
	{
		if ('\0' == m_szAddresserEmail[0])
		{
			//没有变更
			EML_UNLOCK(&m_Mutex);
			return 0;
		}

		//输入NULL，清除设置
		m_szAddresserEmail[0] = '\0';
	}
	else
	{
		int nBufLen = strlen(pszEmail);
	//printf("\n--IEmail.cpp---EML_SetAddresserEmail()---pszEmail=%s---\n",pszEmail);
		if (nBufLen <= 0)
		{
			if ('\0' == m_szAddresserEmail[0])
			{
				//没有变更
				EML_UNLOCK(&m_Mutex);
				return 0;
			}

			//输入空串，清除设置
			m_szAddresserEmail[0] = '\0';
		}
		else if (0 != EML_CheckEmail(pszEmail))
		{
			//输入的邮件不合法
			NET_ERR("invalid email: %s\n", pszEmail);

			EML_UNLOCK(&m_Mutex);
			return -1;
		}
		else if (0 == strncmp(m_szAddresserEmail, pszEmail, NET_MAX_EMAIL_LEN - 1))
		{
			//没有变更
			EML_UNLOCK(&m_Mutex);
			return 0;
		}
		else
		{
			//保存发件人
			snprintf(m_szAddresserEmail,sizeof(m_szAddresserEmail),"%s", pszEmail);
		}
	}

	//保存到配置文件
	CConfig *pConfig = CConfig::GetInstance();

	if (NULL != pConfig)
	{
		pConfig->SetValue(NET_EMAIL_CFG, "AddresserEmail", m_szAddresserEmail);
	}

	EML_UNLOCK(&m_Mutex);
	return 0;
}

//!\brief   获取发送Email的时间间隔
//!\param   nTimeInterval 发送Email的时间间隔(秒)
//!\return  0表示成功，其它为错误码
int IEmail::EML_GetEmailTimeInterval(unsigned int *nTimeInterval)
{
    *nTimeInterval = m_nSmtpSendTimeInterval;

    return 0;
}

//!\brief   设置发送Email的时间间隔
//!\param   nTimeInterval 发送Email的时间间隔(秒)
//!\return  0表示成功，其它为错误码
int IEmail::EML_SetEmailTimeInterval(unsigned int nTimeInterval)
{
	EML_LOCK(&m_Mutex);
	
    if (nTimeInterval < 0)
    {
        EML_UNLOCK(&m_Mutex);
        return -1;
    }

    if (nTimeInterval < MINEMAILTIMEINTERVAL)
    {
        nTimeInterval = MINEMAILTIMEINTERVAL;
    }

    if (m_nSmtpSendTimeInterval == nTimeInterval)
    {
        EML_UNLOCK(&m_Mutex);
        return 0;
    }
    
    m_nSmtpSendTimeInterval = nTimeInterval;

	//保存到配置文件
	CConfig *pConfig = CConfig::GetInstance();
	if (NULL != pConfig)
	{
		pConfig->SetValue(NET_EMAIL_CFG, "SendEMLTimeInterval", m_nSmtpSendTimeInterval);
	}
	
    EML_UNLOCK(&m_Mutex);

    return 0;
}

//!\brief   获取发件人Email用户名
//!\param   pszUserName 返回发件人Email用户名
//!\param   nBufLen pszUserName指向的buf大小
//!\return  0表示成功，其它为错误码
int IEmail::EML_GetSenderUserName(char *pszUserName, int nBufLen)
{
	if (NULL == pszUserName)
	{
		NET_ERR("NULL == pszUserName\n");
		return -1;
	}

	if (nBufLen <= 0)
	{
		NET_ERR("nBufLen(%d) <= 0\n", nBufLen);
		return -2;
	}

	if (nBufLen < NET_MAX_USER_NAME_LEN)
	{
		NET_INFO("buffer is small, nBufLen = %d, need %d\n", nBufLen, NET_MAX_USER_NAME_LEN);
	}

	EML_LOCK(&m_Mutex);

	strncpy(pszUserName, m_szSenderUserName, nBufLen - 1);
	pszUserName[nBufLen - 1] = '\0';

	EML_UNLOCK(&m_Mutex);
	return 0;
}

//!\brief   设置发件人Email用户名
//!\param   pszUserName 发件人Email用户名
//!\return  0表示成功，其它为错误码
int IEmail::EML_SetSenderUserName(char *pszUserName)
{
	EML_LOCK(&m_Mutex);

	if (NULL == pszUserName)
	{
		if ('\0' == m_szSenderUserName[0])
		{
			//没有变更
			EML_UNLOCK(&m_Mutex);
			return 0;
		}

		//输入NULL，清除设置
		m_szSenderUserName[0] = '\0';
	}
	else
	{
		int nBufLen = strlen(pszUserName);

		if (nBufLen <= 0)
		{
			if ('\0' == m_szSenderUserName[0])
			{
				//没有变更
				EML_UNLOCK(&m_Mutex);
				return 0;
			}

			//输入空串，清除设置
			m_szSenderUserName[0] = '\0';
		}
		else if (nBufLen >= NET_MAX_USER_NAME_LEN)
		{
			//输入太长，无法保存
			NET_ERR("buffer is big, nBufLen = %d, need %d\n", nBufLen, NET_MAX_USER_NAME_LEN);
			EML_UNLOCK(&m_Mutex);
			return -1;
		}
		else if (0 == strncmp(m_szSenderUserName, pszUserName, NET_MAX_USER_NAME_LEN - 1))
		{
			//没有变更
			EML_UNLOCK(&m_Mutex);
			return 0;
		}
		else
		{
			//保存
			snprintf(m_szSenderUserName, sizeof(m_szSenderUserName), "%s", pszUserName);
		}
	}

	//保存到配置文件
	CConfig *pConfig = CConfig::GetInstance();
	if (NULL != pConfig)
	{
		pConfig->SetValue(NET_EMAIL_CFG, "SenderUserName", m_szSenderUserName);
	}

	EML_UNLOCK(&m_Mutex);
	return 0;
}

//!\brief       获取发件人Email账户密码
//!\param   pszPassword 返回发件人Email账户密码
//!\param   nBufLen pszPassword指向的buf大小
//!\return  0表示成功，其它为错误码
int IEmail::EML_GetAddresserPassword(char *pszPassword, int nBufLen)
{
	if (NULL == pszPassword)
	{
		NET_ERR("NULL == pszPassword\n");
		return -1;
	}

	if (nBufLen <= 0)
	{
		NET_ERR("nBufLen(%d) <= 0\n", nBufLen);
		return -2;
	}

	if (nBufLen < NET_MAX_PASSWORD_LEN)
	{
		NET_INFO("buffer is small, nBufLen = %d, need %d\n", nBufLen, NET_MAX_PASSWORD_LEN);
	}

	EML_LOCK(&m_Mutex);

	strncpy(pszPassword, m_szAddresserPassword, nBufLen - 1);
	pszPassword[nBufLen - 1] = '\0';

	EML_UNLOCK(&m_Mutex);
	return 0;
}


//!\brief       设置发件人Email账户密码
//!\param   pszPassword 发件人Email账户密码
//!\return  0表示成功，其它为错误码
int IEmail::EML_SetAddresserPassword(const char *pszPassword)
{
	EML_LOCK(&m_Mutex);

	if (NULL == pszPassword)
	{
		if ('\0' == m_szAddresserPassword[0])
		{
			//没有变更
			EML_UNLOCK(&m_Mutex);
			return 0;
		}

		//输入NULL，清除设置
		m_szAddresserPassword[0] = '\0';
	}
	else
	{
		int nBufLen = strlen(pszPassword);

		if (nBufLen <= 0)
		{
			if ('\0' == m_szAddresserPassword[0])
			{
				//没有变更
				EML_UNLOCK(&m_Mutex);
				return 0;
			}

			//输入空串，清除设置
			m_szAddresserPassword[0] = '\0';
		}
		else if (nBufLen >= NET_MAX_PASSWORD_LEN)
		{
			//输入的密码太长，无法保存
			NET_ERR("buffer is big, nBufLen = %d, need %d\n", nBufLen, NET_MAX_PASSWORD_LEN);

			EML_UNLOCK(&m_Mutex);
			return -1;
		}
		else if (0 == strncmp(m_szAddresserPassword, pszPassword, NET_MAX_PASSWORD_LEN - 1))
		{
			//没有变更
			EML_UNLOCK(&m_Mutex);
			return 0;
		}
		else
		{
			//保存密码
			snprintf(m_szAddresserPassword,sizeof(m_szAddresserPassword),"%s", pszPassword);
		}
	}


	//保存到配置文件
	CConfig *pConfig = CConfig::GetInstance();

	if (NULL != pConfig)
	{
		pConfig->SetValue(NET_EMAIL_CFG, "AddresserPassword", m_szAddresserPassword);
	}

	EML_UNLOCK(&m_Mutex);
	return 0;
}

//!\brief       获取收件人Email
//!\param   pszAddress  返回收件人Email地址
//!\param   nBufLen pszEmail指向的buf大小
//!\param   index 第几个收件地址
//!\return  0表示成功，其它为错误码
int IEmail::EML_GetRecvAddresser(char *pszEmail, int nBufLen, int index)
{
	if (NULL == pszEmail)
	{
		NET_ERR("NULL == pszEmail\n");
		return -1;
	}

	if (nBufLen <= 0)
	{
		NET_ERR("nBufLen(%d) <= 0\n", nBufLen);
		return -2;
	}

    if (index < 0 || index >= 3)
    {
		NET_ERR("index(%d) error\n", index);
		return -2;
    }
	if (nBufLen < NET_MAX_EMAIL_LEN)
	{
		NET_INFO("buffer is small, nBufLen = %d, need %d\n", nBufLen, NET_MAX_EMAIL_LEN);
	}

	EML_LOCK(&m_Mutex);

	strncpy(pszEmail, m_sRecvAddresser[index], nBufLen - 1);
	EML_UNLOCK(&m_Mutex);
	return 0;
}

//!\brief       设置收件人Email
//!\param   pszEmail    收件人Email
//!\return  0表示成功，其它为错误码
int IEmail::EML_SetRecvAddresser(const char *pszEmail, int index)
{
	EML_LOCK(&m_Mutex);

    if (index < 0 || index >= 3)
    {
		NET_ERR("index(%d) error\n", index);
		return -2;
    }
	if (NULL == pszEmail)
	{
		if ('\0' == m_sRecvAddresser[index][0])
		{
			//没有变更
			EML_UNLOCK(&m_Mutex);
			return 0;
		}
		//输入NULL，清除设置
		m_sRecvAddresser[index][0] = '\0';
	}
	else
	{
		int nBufLen = strlen(pszEmail);

		if (nBufLen <= 0)
		{
			if ('\0' == m_sRecvAddresser[index][0])
			{
				//没有变更
				EML_UNLOCK(&m_Mutex);
				return 0;
			}
			//输入空串，清除设置
			m_sRecvAddresser[index][0] = '\0';
		}
		else if (0 != EML_CheckEmail(pszEmail))
		{
			//输入的邮件不合法
			NET_ERR("invalid email: %s\n", pszEmail);

			EML_UNLOCK(&m_Mutex);
			return -1;
		}
		else if (0 == strncmp(m_sRecvAddresser[index], pszEmail, NET_MAX_EMAIL_LEN - 1))
		{
            //没有变更
			EML_UNLOCK(&m_Mutex);
			return 0;
		}
		else
		{
            //保存发件人
			strncpy(m_sRecvAddresser[index], pszEmail, nBufLen - 1);
			m_sRecvAddresser[index][nBufLen] = '\0';
		}
	}

	//保存到配置文件
	CConfig *pConfig = CConfig::GetInstance();

	if (NULL != pConfig)
	{
	    if (0 == index)
	    {
    		pConfig->SetValue(NET_EMAIL_CFG, "RecvAddresser1", m_sRecvAddresser[index]);
	    }
	    else if (1 == index)
	    {
            pConfig->SetValue(NET_EMAIL_CFG, "RecvAddresser2", m_sRecvAddresser[index]);
	    }
	    else if (2 == index)
	    {
            pConfig->SetValue(NET_EMAIL_CFG, "RecvAddresser3", pszEmail);
	    }
	}
	EML_UNLOCK(&m_Mutex);
	return 0;

}

//!\brief   获取抄送人Email
//!\param   pszEmail  返回抄送人Email
//!\param   nBufLen pszEmail指向的buf大小
//!\return  0表示成功，其它为错误码
int IEmail::EML_GetCCAddresser(char *pszEmail, int nBufLen)
{
	if (NULL == pszEmail)
	{
		NET_ERR("NULL == pszEmail\n");
		return -1;
	}

	if (nBufLen <= 0)
	{
		NET_ERR("nBufLen(%d) <= 0\n", nBufLen);
		return -2;
	}

	if (nBufLen < NET_MAX_EMAIL_LEN)
	{
		NET_INFO("buffer is small, nBufLen = %d, need %d\n", nBufLen, NET_MAX_EMAIL_LEN);
	}

	EML_LOCK(&m_Mutex);

	strncpy(pszEmail, m_szCCAddresserEmail, nBufLen - 1);
	EML_UNLOCK(&m_Mutex);
	
	return 0;
}

//!\brief   设置抄送人Email
//!\param   pszEmail    抄送人Email
//!\return  0表示成功，其它为错误码
int IEmail::EML_SetCCAddresser(const char *pszEmail)
{
	if (NULL == pszEmail)
	{
		NET_ERR("NULL == pszEmail\n");
		return -1;
	}

	EML_LOCK(&m_Mutex);
	
	if (NULL == pszEmail)
	{
		if ('\0' == m_szCCAddresserEmail[0])
		{
			//没有变更
			EML_UNLOCK(&m_Mutex);
			return 0;
		}
		//输入NULL，清除设置
		m_szCCAddresserEmail[0] = '\0';
	}
	else
	{
        if (0 != EML_CheckEmail(pszEmail))
        {
            //输入的邮件不合法
            NET_ERR("invalid email: %s\n", pszEmail);
            EML_UNLOCK(&m_Mutex);
            return -1;
        }
        else if (0 == strncmp(m_szCCAddresserEmail, pszEmail, NET_MAX_EMAIL_LEN - 1))
    	{
            //没有变更
    		EML_UNLOCK(&m_Mutex);
    		return 0;
    	}
    	else
    	{
            //保存
    		strncpy(m_szCCAddresserEmail, pszEmail, strlen(pszEmail) - 1);
    		m_szCCAddresserEmail[strlen(pszEmail)] = '\0';
    	}
    }
	//保存到配置文件
	CConfig *pConfig = CConfig::GetInstance();

	if (NULL != pConfig)
	{
		pConfig->SetValue(NET_EMAIL_CFG, "CCAddresserEmail", m_szCCAddresserEmail);
	}
	EML_UNLOCK(&m_Mutex);
	
	return 0;
}

//!\brief   获取抄送人Email
//!\param   pszEmail  返回抄送人Email
//!\param   nBufLen pszEmail指向的buf大小
//!\return  0表示成功，其它为错误码
int IEmail::EML_GetBCCAddresser(char *pszEmail, int nBufLen, int index)
{
	if (NULL == pszEmail)
	{
		NET_ERR("NULL == pszEmail\n");
		return -1;
	}

	if (nBufLen <= 0)
	{
		NET_ERR("nBufLen(%d) <= 0\n", nBufLen);
		return -2;
	}

	if (nBufLen < NET_MAX_EMAIL_LEN)
	{
		NET_INFO("buffer is small, nBufLen = %d, need %d\n", nBufLen, NET_MAX_EMAIL_LEN);
	}

	EML_LOCK(&m_Mutex);

	strncpy(pszEmail, m_szBCCAddresserEmail, nBufLen - 1);
	EML_UNLOCK(&m_Mutex);
	
	return 0;
}

//!\brief   设置抄送人Email
//!\param   pszEmail    抄送人Email
//!\return  0表示成功，其它为错误码
int IEmail::EML_SetBCCAddresser(const char *pszEmail)
{
	if (NULL == pszEmail)
	{
		NET_ERR("NULL == pszEmail\n");
		return -1;
	}

	EML_LOCK(&m_Mutex);
	if (NULL == pszEmail)
	{
		if ('\0' == m_szBCCAddresserEmail[0])
		{
			//没有变更
			EML_UNLOCK(&m_Mutex);
			return 0;
		}
		//输入NULL，清除设置
		m_szBCCAddresserEmail[0] = '\0';
	}
    else
    {
        if (0 != EML_CheckEmail(pszEmail))
        {
            //输入的邮件不合法
            NET_ERR("invalid email: %s\n", pszEmail);
            EML_UNLOCK(&m_Mutex);
            return -1;
        }
        else if (0 == strncmp(m_szBCCAddresserEmail, pszEmail, NET_MAX_EMAIL_LEN - 1))
    	{
            //没有变更
    		EML_UNLOCK(&m_Mutex);
    		return 0;
    	}
    	else
    	{
            //保存
    		strncpy(m_szBCCAddresserEmail, pszEmail, strlen(pszEmail) - 1);
    		m_szBCCAddresserEmail[strlen(pszEmail)] = '\0';
    	}
    }
    
	//保存到配置文件
	CConfig *pConfig = CConfig::GetInstance();

	if (NULL != pConfig)
	{
		pConfig->SetValue(NET_EMAIL_CFG, "BCCAddresserEmail", m_szBCCAddresserEmail);
	}
	EML_UNLOCK(&m_Mutex);
	
	return 0;
}

//!\brief   获取发送主题
//!\param   pszSendSubject 返回发送主题
//!\param   nBufLen pszSendSubject指向的buf大小
//!\return  0表示成功，其它为错误码
int IEmail::EML_GetSendSubject(char *pszSendSubject, int nBufLen)
{
	if (NULL == pszSendSubject)
	{
		NET_ERR("NULL == pszSendSubject\n");
		return -1;
	}

	if (nBufLen <= 0)
	{
		NET_ERR("nBufLen(%d) <= 0\n", nBufLen);
		return -2;
	}

	if (nBufLen < NET_MAX_SUBJECT_LEN)
	{
		NET_INFO("buffer is small, nBufLen = %d, need %d\n", nBufLen, NET_MAX_SUBJECT_LEN);
	}

	EML_LOCK(&m_Mutex);

	strncpy(pszSendSubject, m_sSubjectTopic, nBufLen - 1);
	pszSendSubject[nBufLen - 1] = '\0';

	EML_UNLOCK(&m_Mutex);
	return 0;
}

//!\brief   设置发送主题
//!\param   pszSendSubject 发送主题
//!\return  0表示成功，其它为错误码
int IEmail::EML_SetSendSubject(const char *pszSendSubject)
{
	EML_LOCK(&m_Mutex);

	if (NULL == pszSendSubject)
	{
		if ('\0' == m_sSubjectTopic[0])
		{
			//没有变更
			EML_UNLOCK(&m_Mutex);
			return 0;
		}

		//输入NULL，清除设置
		m_sSubjectTopic[0] = '\0';
	}
	else
	{
		int nBufLen = strlen(pszSendSubject);

		if (nBufLen <= 0)
		{
			if ('\0' == m_sSubjectTopic[0])
			{
				//没有变更
				EML_UNLOCK(&m_Mutex);
				return 0;
			}

			//输入空串，清除设置
			m_sSubjectTopic[0] = '\0';
		}
		else if (nBufLen >= NET_MAX_SUBJECT_LEN)
		{
			//输入太长，无法保存
			NET_ERR("buffer is big, nBufLen = %d, need %d\n", nBufLen, NET_MAX_SUBJECT_LEN);
			EML_UNLOCK(&m_Mutex);
			return -1;
		}
		else if (0 == strncmp(m_sSubjectTopic, pszSendSubject, NET_MAX_SUBJECT_LEN - 1))
		{
			//没有变更
			EML_UNLOCK(&m_Mutex);
			return 0;
		}
		else
		{
			//保存	
			snprintf(m_sSubjectTopic,sizeof(m_sSubjectTopic),"%s", pszSendSubject);
			printf("\n\n*****m_sSubjectTopic changed to=%s\n",m_sSubjectTopic);
		}
	}

	//保存到配置文件
	CConfig *pConfig = CConfig::GetInstance();
	if (NULL != pConfig)
	{
		pConfig->SetValue(NET_EMAIL_CFG, "SubjectTopic", m_sSubjectTopic);
	}

	EML_UNLOCK(&m_Mutex);
	return 0;
}

//!\brief   获取发送Email正文
//!\param   pszSendBody 返回发送Email正文
//!\param   nBufLen pszSendBody指向的buf大小
//!\return  0表示成功，其它为错误码
int IEmail::EML_GetSendBody(char *pszSendBody, int nBufLen)
{
	if (NULL == pszSendBody)
	{
		NET_ERR("NULL == pszSendBody\n");
		return -1;
	}

	if (nBufLen <= 0)
	{
		NET_ERR("nBufLen(%d) <= 0\n", nBufLen);
		return -2;
	}

	if (nBufLen < NET_MAX_EMAIL_BODY_LEN)
	{
		NET_INFO("buffer is small, nBufLen = %d, need %d\n", nBufLen, NET_MAX_EMAIL_BODY_LEN);
	}
	EML_LOCK(&m_Mutex);

	strncpy(pszSendBody, m_szEmailBody, nBufLen - 1);
	pszSendBody[nBufLen - 1] = '\0';

	EML_UNLOCK(&m_Mutex);
	return 0;
}

//!\brief   设置发送Email正文
//!\param   pszSendBody Email正文
//!\return  0表示成功，其它为错误码
int IEmail::EML_SetSendBody(const char *pszSendBody)
{
	EML_LOCK(&m_Mutex);

	if (NULL == pszSendBody)
	{
		if ('\0' == m_szEmailBody[0])
		{
			//没有变更
			EML_UNLOCK(&m_Mutex);
			return 0;
		}

		//输入NULL，清除设置
		m_sSubjectTopic[0] = '\0';
	}
	else
	{
		int nBufLen = strlen(pszSendBody);

		if (nBufLen <= 0)
		{
			if ('\0' == m_sSubjectTopic[0])
			{
				//没有变更
				EML_UNLOCK(&m_Mutex);
				return 0;
			}

			//输入空串，清除设置
			m_szEmailBody[0] = '\0';
		}
		else if (nBufLen >= NET_MAX_EMAIL_BODY_LEN)
		{
			//输入太长，无法保存
			NET_ERR("buffer is big, nBufLen = %d, need %d\n", nBufLen, NET_MAX_EMAIL_BODY_LEN);
			EML_UNLOCK(&m_Mutex);
			return -1;
		}
		else if (0 == strncmp(m_szEmailBody, pszSendBody, NET_MAX_EMAIL_BODY_LEN - 1))
		{
			//没有变更
			EML_UNLOCK(&m_Mutex);
			return 0;
		}
		else
		{
			//保存
			snprintf(m_szEmailBody,sizeof(m_szEmailBody),"%s",pszSendBody);
			printf("\n\n*****m_szEmailBody changed to=%s\n",m_szEmailBody);
		}
	}

	//保存到配置文件
	CConfig *pConfig = CConfig::GetInstance();
	if (NULL != pConfig)
	{
		pConfig->SetValue(NET_EMAIL_CFG, "SendBody", m_szEmailBody);
	}

	EML_UNLOCK(&m_Mutex);
	return 0;
}


//!\brief   获取发送Email的时间是否到
//!\param   bTimeArrival 发送Email的时间是否到 0未到；1到了
//!\return  0表示成功，其它为错误码
int IEmail::EML_GetEmailTimeIntervalIsArrival(unsigned int *bTimeArrival)
{
    EML_LOCK(&m_status_Mutex);         //加锁

    // 判断两次发送邮件的时间是否超过指定时间间隔，如果超过或者是第一次发送，则正常发送邮件，否则挂起等待时间到达
    //printf("-----m_TimeCount:%d, m_LastSendEMLTime:%d, m_nSmtpSendTimeInterval:%d-----\n", m_TimeCount, m_LastSendEMLTime, m_nSmtpSendTimeInterval);
    if (m_TimeCount - m_LastSendEMLTime >= m_nSmtpSendTimeInterval)
    //if (m_TimeCount - m_LastSendEMLTime >= 15)
    {
        *bTimeArrival = 1;
    }
    else
    {
        *bTimeArrival = 0;
    }

    EML_UNLOCK(&m_status_Mutex);         //解锁

    return 0;
}


//!\brief   获取测试邮件发送状态
//!\param   pStatus 测试邮件发送状态
//!\return  0表示成功，其它为错误码
int IEmail::EML_GetTestEmailSendStatus(int *pStatus)
{
    if (NULL == pStatus)
    {
		NET_ERR("NULL == pStatus\n");
		return -1;
    }
	NET_DBG("FUNCTION IN\n\n");
	printf(" \n\n***EML_GetTestEmailSendStatus\n\n");
	int i = 0;	
    int status = 0;

	while (i++ <= 30)
	{
		SelectSleepSec(1);
		status = GetTestEmailSendStatus();
		if (0 != status)
		{
			printf(" \n\n***EmailSendStatus != 0\n\n");
			if(m_bTestEmailOverFlag==1)//如果测试已经结束，提前返回
			{
				break;
			}

			continue;
		}
		else if(0 == status)
		{
			printf("\n\n***EmailSendStatus = 0\n\n");
			break;
		}	
	}

    switch(status)
    {
        case 0:   *pStatus = 0; break;
        case 200: 
        case 201: 
        case 202: 
        case 203: 
        case 204: 
        case 205: 
        case 209: *pStatus = IDVR_NET_EMAIL_INPUT_PARAM_EMPTY;      break;
        case 206: *pStatus = IDVR_NET_EMAIL_PASSWD_ERROR;           break;
        case 208: *pStatus = IDVR_NET_EMAIL_SERVER_ERROR;           break;
        case 400: *pStatus = IDVR_NET_EMAIL_CONNECTION_CLOSED;      break;
        case 401: *pStatus = IDVR_NET_EMAIL_SERVER_NOT_READY;       break;
        case 402: *pStatus = IDVR_NET_EMAIL_SERVER_NOT_RESPONDING;  break;
        case 404: *pStatus = IDVR_NET_EMAIL_ATTACHMENT_NOT_EXIST;   break;
        case 406: *pStatus = IDVR_NET_EMAIL_BAD_LOGIN_PASS;         break;
        case 414: *pStatus = IDVR_NET_EMAIL_SSL_PROBLEM;            break;
        case 416: *pStatus = IDVR_NET_EMAIL_STARTTLS_NOT_SUPPORTED; break;
        case 417: *pStatus = IDVR_NET_EMAIL_LOGIN_NOT_SUPPORTED;    break;
        default:  *pStatus = IDVR_NET_EMAIL_OTHER_ERROR;            break; 
    }

    return 0;
}

//!\brief       检查Email是否合法
//!\param   pszEmail        检查Email是否合法
//!\return  0表示成功，其它为错误码
int IEmail::EML_CheckEmail(const char *pszEmail)
{
	int nLen, n, i;

	const char *pszRaw;

	const char *s;

	bool b;

	//指针不能为NULL
	if (NULL == pszEmail)
	{
		NET_ERR("NULL == pszEmail\n");
		return -1;
	}

	//检测长度(最短email "1@1.1" 长度为5)
//printf("\n--IEmail.cpp-------EML_CheckEmail()-------pszEmail=%s----\n",pszEmail);
	nLen = strlen(pszEmail);
	//printf("\n--IEmail.cpp-------EML_CheckEmail()-------nLen=%d----\n",nLen);
	if (nLen < 5)
	{
		NET_ERR("\"%s\", too short, len = %d\n", pszEmail, nLen);
		return -2;
	}

	//必须小于缓冲区大小
	if (nLen >= NET_MAX_EMAIL_LEN)
	{
		NET_ERR("\"%s\", too long, len = %d\n", pszEmail, nLen);
		return -3;
	}
//printf("\n--IEmail.cpp-------EML_CheckEmail()----1----\n");
	//检查有没有输入别名，查找字符<
	s = strchr(pszEmail, '<');
	if (NULL == s)
	{
		//不包含别名，则为纯Email
		pszRaw = pszEmail;
	}
	else if (s == pszEmail)
	{
		//字符<在最前面，等于没有别名
		NET_ERR("\"%s\", < at first\n", pszEmail);
		return -4;
	}
	else if (pszEmail[nLen - 1] != '>')
	{
		//最后一个字符必须为>
		NET_ERR("\"%s\", last char not >\n", pszEmail);
		return -5;
	}
	else
	{
		//包含合法别名，提取存Email

		n = s - pszEmail;		//别名的长度
		nLen -= n + 2;			//实际email长度

		if (nLen < 5)
		{
			NET_ERR("\"%s\", raw email too short, len = %d\n", pszEmail, nLen);
			return -6;
		}

		//纯Email位置
		pszRaw = s + 1;
	}
//printf("\n--IEmail.cpp-------EML_CheckEmail()----2----\n");
	//查找字符@
	s = strchr(pszRaw, '@');
	if (NULL == s)
	{
		NET_ERR("\"%s\", no @\n", pszEmail);
		return -7;
	}

	//判断@的位置是否合法(前面至少有1个字符，后面至少有3个字符)
	n = s - pszRaw;
	if ((n < 1) || ((nLen - n) <= 3))
	{
		NET_ERR("\"%s\", @ pos = %d, len = %d\n", pszEmail, n, nLen);
		return -8;
	}
//printf("\n--IEmail.cpp-------EML_CheckEmail()----3----\n");
	//检查Email账户名是否合法
	for (i = 0; i < n; ++i)
	{
		if ((pszRaw[i] >= '0') && (pszRaw[i] <= '9'))
		{
		}
		else if ((pszRaw[i] >= 'a') && (pszRaw[i] <= 'z'))
		{
		}
		else if ((pszRaw[i] >= 'A') && (pszRaw[i] <= 'Z'))
		{
		}
		else if ((pszRaw[i] == '_') || (pszRaw[i] == '-') || (pszRaw[i] == '.'))
		{
			if ((i == 0) || (i == (n - 1)))
			{
				NET_ERR("\"%s\", invalid first or last char %c\n", pszEmail, pszRaw[i]);
				return -9;
			}
		}
		else
		{
			NET_ERR("\"%s\", invalid char %c\n", pszEmail, pszRaw[i]);
			return -10;
		}
	}


	//获取域名
	++s;

//printf("\n--IEmail.cpp-------EML_CheckEmail()----4----\n");
	//检查Email域名是否合法

	//至少要有一个.
	if (NULL == strchr(s, '.'))
	{
		NET_ERR("\"%s\", no .\n", pszEmail);
		return -11;
	}
//printf("\n--IEmail.cpp-------EML_CheckEmail()----5----\n");
	//控制第一个字符不能为.
	b = true;
	for (i = n + 1; i < nLen; ++i)
	{
		if ((pszRaw[i] >= '0') && (pszRaw[i] <= '9'))
		{
			b = false;
		}
		else if ((pszRaw[i] >= 'a') && (pszRaw[i] <= 'z'))
		{
			b = false;
		}
		else if ((pszRaw[i] >= 'A') && (pszRaw[i] <= 'Z'))
		{
			b = false;
		}
		else if (pszRaw[i] == '.')
		{
			if (b)
			{
				NET_ERR("\"%s\", domain has ..\n", pszEmail);
				return -12;
			}
			else
			{
				b = true;
			}
		}
		else
		{
			NET_ERR("\"%s\", invalid char %c\n", pszEmail, pszRaw[i]);
			return -13;
		}
	}
//printf("\n--IEmail.cpp-------EML_CheckEmail()----6----\n");
	//最后一个不能为
	if (b)
	{
		NET_ERR("\"%s\", last char is .\n", pszEmail);
		return -14;
	}
	//printf("\n--IEmail.cpp-------EML_CheckEmail()-------pszEmail=%s----\n",pszEmail);
//printf("\n--IEmail.cpp-------EML_CheckEmail()----end----\n");
	return 0;
}

//!\brief           判断配置是否齐全
//!\return  0表示成功，其它为错误码
int IEmail::EML_IsReady()
{
	EML_LOCK(&m_Mutex);
//printf("\n-------EML_IsReady------\n");

	//没有启动SMTP服务器配置
	if (0 == m_bEnable)
	{
		NET_ERR("not enable smtp server\n");

		EML_UNLOCK(&m_Mutex);
		return -2;
	}

	//没有设置SMTP服务器地址
	if (strlen(m_szSmtpAddress) <= 0)
	{
		NET_ERR("not set smtp server address\n");

		EML_UNLOCK(&m_Mutex);
		return -3;
	}

	//没有设置发件人Email
	//printf("\n--IEmail.cpp---EML_IsReady()--设置发件人EMail--m_szAddresserEmail=%s---\n",m_szAddresserEmail);
	if (0 != EML_CheckEmail(m_szAddresserEmail))
	{
		NET_ERR("invilad addresser email\n");

		EML_UNLOCK(&m_Mutex);
		return -4;
	}

	EML_UNLOCK(&m_Mutex);
	return 0;
}

int IEmail::EML_IsBusy()
{
	return m_bSendEmailIsBusy;
}

//!\brief       发送Email列表
//!\param   pszAddressee    收件人Email 列表
//!\param   nAddresseeNum   收件人个数(最多支持15个用户)
//!\param   pszTopic        主题（256 chars）
//!\param   pszContext      内容（不限制大小）
//!\param   pszAffix        附件（18个文件，路径名总长不超过100 x 18 chars?//!\param   bTestEML        是否为测试邮件
//!\return  0表示成功，其它为错误码
int IEmail::EML_SendList(const char *pszAddresseeList[], int nAddresseeNum, 	 const char *pszTopic, 
								 const char *pszContext, const char *pszAffix,int bTestEML)
{
	const char *pszList[IDVR_MAX_USER];
	int i, count;
	if(bTestEML)
	{
		m_nTestEmailstaue = 1;
	}
	//发件人数量错误
	if (nAddresseeNum <= 0)
	{
		NET_ERR("nAddresseeNum(%d) <= 0\n", nAddresseeNum);
		if(bTestEML)
		{
			m_nTestEmailstaue = 0;
		}
		return -1;
	}

	if (NULL == pszAddresseeList)
	{
		NET_ERR("NULL == pszAddresseeList\n");
		if(bTestEML)
		{
			m_nTestEmailstaue = 0;
		}
		return -2;
	}

	//检查是否允许发送
	if (0 != EML_IsReady())
	{
		NET_ERR("config not ready\n");
		if(bTestEML)
		{
			m_nTestEmailstaue = 0;
		}
		return -3;
	}

	//最多支持15个用户
	if ((unsigned int) nAddresseeNum > IDVR_MAX_USER)
	{
		nAddresseeNum = IDVR_MAX_USER;
	}
	count = 0;
	for (i = 0; i < nAddresseeNum; ++i)
	{
        printf("\n--IEmail.cpp-------EML_SendList()-------pszAddresseeList=%s----\n",pszAddresseeList[i]);
		if (0 == EML_CheckEmail(pszAddresseeList[i]))
		{
			pszList[count] = pszAddresseeList[i];
			++count;
		}
	}

	if (count <= 0)
	{
		NET_ERR("no valid addressee email\n");
		if(bTestEML)
		{
			m_nTestEmailstaue = 0;
		}
		return -4;
	}

	EML_LOCK(&m_Mutex);
	
	//发送邮件
	if (0 != SendMail(pszList, count, pszTopic, pszContext, pszAffix, bTestEML))
	{
		NET_ERR("send %d email fail\n", count);

		EML_UNLOCK(&m_Mutex);
		if(bTestEML)
		{
			m_nTestEmailstaue = 0;
		}
		return -5;
	}

	EML_UNLOCK(&m_Mutex);
	if(bTestEML)
	{
		m_nTestEmailstaue = 0;
	}
	return 0;
}

//!\brief   获取测试邮件发送状态
//!\return  0表示成功，其它为错误码
int IEmail::GetTestEmailSendStatus(void)
{
    return m_nTestEmailSendStatus;
}


int IEmail::EML_GetTestEmailStatus(int *pStatus)
{
	if (NULL == pStatus)
    {
		NET_ERR("NULL == pStatus\n");
		return -1;
    }
	*pStatus = m_nTestEmailstaue;
	return 0;
}


//!\brief   实际发送邮件
//!\return  0表示成功，其它为错误码
int IEmail::SendSmtpEmail(const char *pEmailBodyFile)
{
    if (NULL == pEmailBodyFile)
    {
        printf("[%s-%d] pSubjectTopicFile==NULL\n", __FILE__, __LINE__);
        return -1;
    }

	int iRet = 0;
	unsigned int i = 0;
	SMTP_SECURITY_TYPE SecurityType;
	char *pSendNameEndFlag = NULL;
	char szStrTmp[256] = {0};
	int nRecvAddrCount = 0;

	//检查是否允许发送
	if (0 != EML_IsReady())
	{
		NET_ERR("config not ready\n");
		return -3;
	}
	//检查收件人邮箱合法性
	nRecvAddrCount = 0;
	for (i=0; i<3; i++)
    {
		if (0 == EML_CheckEmail(m_sRecvAddresser[i]))
		{
			nRecvAddrCount++;
		}
    }
	if (nRecvAddrCount <= 0)
	{
		NET_ERR("no valid addressee email\n");
		return -4;
	}
	
    switch(m_nSmtpSecurityMode)
    {
        case 0:SecurityType = NO_SECURITY; break;
        case 1:SecurityType = USE_SSL;     break;
        case 2:SecurityType = USE_TLS;     break;
        case 3:SecurityType = USE_TLS;     break;
        case 4:SecurityType = DO_NOT_SET;  break;
		default :SecurityType = DO_NOT_SET;  break;
    }

    try
    {
    	m_pCSmtp->SetCharSet("UTF-8");	//避免发件人及收件人为乱码的问题
        m_pCSmtp->SetSMTPServer(m_szSmtpAddress, m_nSmtpPort, m_nSmtpAuth);
        m_pCSmtp->SetSecurityType(SecurityType);
        m_pCSmtp->SetLogin(m_szAddresserEmail);
        m_pCSmtp->SetPassword(m_szAddresserPassword);
		memset(szStrTmp, 0, sizeof(szStrTmp));
		memcpy(szStrTmp, m_szAddresserEmail, sizeof(szStrTmp));
		pSendNameEndFlag = strstr(m_szAddresserEmail, "@");
		if (NULL != pSendNameEndFlag)
		{
			szStrTmp[(int)(pSendNameEndFlag-m_szAddresserEmail)] = '\0';
		}
        m_pCSmtp->SetSenderName(szStrTmp/*"jingzhuFan1992"*/);
        m_pCSmtp->SetSenderMail(m_szAddresserEmail);
		m_pCSmtp->SetReplyTo(m_szAddresserEmail);
        m_pCSmtp->SetSubject(m_sSubjectTopic);

        /* 清空(清空收件人,密送人,抄送人,附件和消息) */
        m_pCSmtp->ClearMessage();

        for (i=0; i<3; i++)
        {
            if ('\0' != m_sRecvAddresser[i][0])
            {
                m_pCSmtp->AddRecipient(m_sRecvAddresser[i]);
            }
        }

        iRet = m_pCSmtp->AddMsgFile(pEmailBodyFile);

        m_pCSmtp->SetXPriority(XPRIORITY_NORMAL);    
        m_pCSmtp->SetXMailer("The Bat! (v3.02) Professional");
		
		if (m_bPic_att)
		{
			for (i=0; i<m_nCurrAtrraCount; i++)
	        {
	            m_pCSmtp->AddAttachment(m_sAttraAffix[i]);
	        }
		}
		
		printf("\
|------------------------------Message Info------------------------------|\n\
=> SmtpAddress = %s\n\
=> SmtpPort = %d\n\
=> SmtpAuth = %d\n\
=> SecurityType = %d\n\
=> AddresserEmail = %s\n\
=> AddresserPassword = %s\n\
=> RecvAddresser[0] = %s\n\
=> RecvAddresser[1] = %s\n\
=> RecvAddresser[2] = %s\n\
=> m_nCurrAtrraCount= %d\n\
=> AttraAffix[0] = %s\n\
=> AttraAffix[1] = %s\n\
=> AttraAffix[2] = %s\n\
=> Am_sSubjectTopic = %s\n\
=> EmailBodyFile = %s\n\
|------------------------------------------------------------------------|\n",
			m_szSmtpAddress,
			m_nSmtpPort,
			m_nSmtpAuth,
			m_nSmtpSecurityMode,
			m_szAddresserEmail,
			m_szAddresserPassword,
			m_sRecvAddresser[0],
			m_sRecvAddresser[1],
			m_sRecvAddresser[2],
			m_nCurrAtrraCount,
			m_sAttraAffix[0],
			m_sAttraAffix[1],
			m_sAttraAffix[2],
			m_sSubjectTopic,
			pEmailBodyFile);

		iRet = m_pCSmtp->Send();
    }
    catch(ECSmtp e)
    {
        iRet = e.GetErrorNum();
        //SMTP_ERR("\n\n*****Send Fail: %s(%d)\n\n", e.GetErrorText().c_str(), iRet);
		fprintf(stderr, "\n\n*****Send Fail: %s(%d)\n\n", e.GetErrorText().c_str(), iRet);
	}

    if (0 == iRet)
    {
        //SMTP_OK("\n\n*****Email was sent successfully\n\n");		
		fprintf(stderr, "\n\n*****Email was sent successfully\n\n");
    }
    
    return iRet;
}

//!\brief   邮件发送线程处理函数
void IEmail::SendEmailThreadProcess()
{
	char szMail[30] = {0};
    char command[128] = {0};
    unsigned int i = 0;

	//判断邮件发送线程是否需要退出
	while (0 != sem_trywait(&m_semExitThread))
	{
		//等待唤醒
		sem_wait(&m_semWakeThread);
		EML_LOCK(&m_Mutex);		//加锁

		m_bSendEmailIsBusy = 1;

        while (m_nLastMailIndex > m_nFirstMailIndex)
		{		
			snprintf(szMail, sizeof (szMail) - 1, "/tmp/app/mail_%x", m_nTempMailIndex);
            szMail[sizeof (szMail) - 1] = 0;
            printf("\n\n*** test email body file:%s\n\n",szMail);
			EML_UNLOCK(&m_Mutex);	    //解锁
			
			if (m_bTestEmail)
            {
				//SMTP_OK("---------------Begin send Test Email---------------\n");          
			  	fprintf(stderr, "---------------Begin send Test Email---------------\n\n");
				m_nTestEmailSendStatus=SendSmtpEmail(szMail);
				m_bTestEmailOverFlag=1;
			}
            else
            {
				fprintf(stderr, "\n\n*****Email was sent successfully\n\n");
                //SMTP_OK("\n\n*****Email was sent successfully\n");
                SendSmtpEmail(szMail);
            }
			//删除正文文件            
            snprintf(command,sizeof(command), "rm -f %s", szMail);
            mysystem(command);

			EML_LOCK(&m_Mutex);	        //加锁

			//邮件计数
			++m_nFirstMailIndex;

			//邮件队列已空
			if (m_nFirstMailIndex == m_nLastMailIndex)
			{
				m_nFirstMailIndex = 0;
				m_nLastMailIndex = 0;
				m_nTempMailIndex = 0;
			}
			
			//发送完毕之后需要删除原来拷贝的图片
            if (0 != m_nCurrAtrraCount)
            {
				for (i=0; i<m_nCurrAtrraCount; i++)
				{
                	snprintf(command,sizeof(command), "rm -f %s", m_sAttraAffix[i]);
                	mysystem(command);
					//printf("command: %s\n", command);
                    memset(m_sAttraAffix[i], 0, sizeof(m_sAttraAffix[i]));
            	}
				m_nCurrAtrraCount = 0;
            }
		}

		m_bSendEmailIsBusy = 0;

		EML_UNLOCK(&m_Mutex);	//解锁
	}
}

//!\brief   发送邮件
//!\param   pszAddresseeList    收件人Email 列表
//!\param   nAddresseeNum       收件人个数(最多15个)
//!\param   pszTopic        主题（256 chars）
//!\param   pszContext      内容（不限制大小）
//!\param   pszAffix        附件（18个文件，路径名总长不超过80 x 18 chars）
//!\param   bTestEML        是否为测试邮件
//!\return  0表示成功，其它为错误码
int IEmail::SendMail(const char *pszAddresseeList[], int nAddresseeNum, const char *pszTopic, 
						  const char *pszContext, const char *pszAffix, int bTestEML)
{
    unsigned int bTimeIsArrival = 0;
	int nContextLen = 0;
	int sem_value  = -1;
	char szMail[30] = {0};
    int  i = 0;
    
    m_bTestEmail = (bTestEML==1) ? 1 : 0;

	printf("\n--IEmail.cpp-------SendMail()-----------\n\n");
    // 如果是触发的邮件，则需要添加时间间隔功能
    if (m_bTestEmail == 0)
    {
        /* 获取时间间隔是否达到 */        
        EML_GetEmailTimeIntervalIsArrival(&bTimeIsArrival);
        if (bTimeIsArrival)
		{
            EML_LOCK(&m_status_Mutex);         //加锁
            m_LastSendEMLTime = m_TimeCount;   //调用该接口表明有移动侦测(目前只有移动侦测会联动邮件)，需要记录当前时间
            EML_UNLOCK(&m_status_Mutex);       //解锁
        }
        else
        {
			// SMTP_OK("Time not arrived! please wait...\n");           
			fprintf(stderr, "Time not arrived! please wait...");
            return -3;
        }
    }
	else	//如果是测试邮件，邮件状态复位
	{
		m_nTestEmailSendStatus=-1;
		m_bTestEmailOverFlag=0;
	}
	
	//邮件计数
	if ((m_nLastMailIndex - m_nFirstMailIndex) >= NET_EMAIL_QUEUE_LEN)
	{
		NET_ERR("email queue is full\n");
		return -3;
	}

	//获取正文大小
	if (NULL == pszContext)
	{
		nContextLen = 0;
	}
	else
	{
		nContextLen = strlen(pszContext);
	}

	if (nContextLen > 0)
	{	
		//生成邮件文件名
		snprintf(szMail, sizeof (szMail) - 1, "/tmp/app/mail_%x", m_nLastMailIndex);
		szMail[sizeof (szMail) - 1] = 0;
		printf("\n\n***szMail=%s\n\n",szMail);
		//创建正文文件
		FILE *fp_context = fopen(szMail, "wb");
		if (NULL == fp_context)
		{
			NET_ERR("create %s fail\n", szMail);

			return -2;
		}
		fwrite(pszContext, 1, nContextLen, fp_context);
		fclose(fp_context);
		fp_context = NULL;
	}
	else
	{
		szMail[0] = 0;
	}
	
	//填写附件
	if (pszAffix && m_bPic_att)
	{
        if (strlen(pszAffix) > 1)  // 还有一个空格
        {            
            //获取各文件绝对地址
    		sscanf(pszAffix, "%s %s", m_sAttraAffix[0],m_sAttraAffix[1]);
    		for (i=0;i<5;i++)
    		{
    		    if (strlen(m_sAttraAffix[i]) > 0)
    		    {
    		        m_nCurrAtrraCount++;
    		    }
    		}
    		printf("\n***m_sAttraAffix[0]=%s,m_sAttraAffix[1]=%s\n",m_sAttraAffix[0],m_sAttraAffix[1]);
        }
        else
        {
            printf("%s not exist!\n", pszAffix);
        }
	}
	else
	{
		m_nCurrAtrraCount=0;
	}
	
	//邮件计数
	m_nTempMailIndex = m_nLastMailIndex;
	++m_nLastMailIndex;
	
	sem_getvalue(&m_semWakeThread, &sem_value);
	if (sem_value == 0)
	{
		printf("\n\n***notify send email thread action\n\n");
		sem_post(&m_semWakeThread);
	}
	
	return 0;
}

//!\brief 时间计数器线程处理函数
void IEmail::TimerCountThreadProcess()
{	
    NET_DBG(">>>>>>>>>> enter, pid = %d\n", getpid());
    BEGINRUN_THREAD;
    
    while(1)
    {
        m_TimeCount++;
        if (m_TimeCount == 3600) // 计时器上限为1小时
        {
            m_TimeCount = 0;
        }
        SelectSleepSec(1);
    }
   
    NET_DBG(">>>>>>>>>> exit, pid = %d\n", getpid());
    ENDRUN_THREAD;

	return;
}


//!\brief   邮件发送线程，回调函数
//!\param   pContext    上下文指针
//!\return  0表示成功，其它为错误码
static void *callback_SendEmailThread(void *pContext)
{
    PRT_TID();

	NET_DBG(">>>>>>>>>> enter, pid = %d\n", getpid());
	BEGINRUN_THREAD;

	//获取当前流播放器实例
	IEmail *pEmail = (IEmail *) pContext;

	if (NULL != pEmail)
	{
		pEmail->SendEmailThreadProcess();
	}
	else
	{
		NET_ERR("get IEmail instance fail\n");
		NET_CHK(NULL != pEmail);
	}

	NET_DBG(">>>>>>>>>> exit, pid = %d\n", getpid());
	ENDRUN_THREAD;

	return NULL;
}

/* 时间计数器线程(1秒计时一次，上限为1小时) */
void* TimeCntThread(void * pContext)
{
    PRT_TID();

	NET_DBG(">>>>>>>>>> enter, pid = %d\n", getpid());
	BEGINRUN_THREAD;

	//获取当前流播放器实例
	IEmail *pEmail = (IEmail *) pContext;

	if (NULL != pEmail)
	{
       pEmail->TimerCountThreadProcess();
	}
	else
	{
		NET_ERR("get IEmail instance fail\n");
		NET_CHK(NULL != pEmail);
	}

	NET_DBG(">>>>>>>>>> exit, pid = %d\n", getpid());
	ENDRUN_THREAD;

	return NULL;
}
#endif

