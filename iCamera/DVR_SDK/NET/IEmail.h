// Copyright (C) 1991 - 1999 Rational Software Corporation

#ifndef _INC_IEMAIL_49B6325C0261_INCLUDED
#define _INC_IEMAIL_49B6325C0261_INCLUDED

#include <pthread.h>
#include <semaphore.h>
#include "CSmtp.h"
#include "global_macro.h"


//!\brief       Net模块     Email服务器接口类
class IEmail
{
  public:
	//!\brief       获取唯一实例
	//!\return  实例指针
	static IEmail *Instance();

	//!\brief       销毁实例
	static void Release();

	//!\brief       获取是否启用SMTP服务器
	//!\return  1表示启用，0表示不启用
	int EML_GetEnable();

	//!\brief       设置是否启用SMTP服务器
	//!\param   bEnable 1表示启用，0表示禁用
	//!\return  0表示成功，其它为错误码
	int EML_SetEnable(int bEnable);

	//!\brief       获取SMTP服务器地址(域名或者IP地址)
	//!\param   pszAddress  返回SMTP服务器地址
	//!\param   nBufLen     pszAddress指向的buf大小
	//!\return  0表示成功，其它为错误码
	int EML_GetSmtpAddress(char *pszAddress, int nBufLen);

	//!\brief       设置SMTP服务器地址(域名或者IP地址)
	//!\param   pszAddress  SMTP服务器地址
	//!\return  0表示成功，其它为错误码
	int EML_SetSmtpAddress(const char *pszAddress);

	//!\brief       获取SMTP服务器端口
	//!\return  返回端口号
	unsigned short EML_GetSmtpPort();

	//!\brief       设置SMTP服务器端口
	//!\param   wPort   SMTP服务器端口
	//!\return  0表示成功，其它为错误码
	int EML_SetSmtpPort(unsigned short wPort);

	//!\brief       获取身份验证方式
	//!\return  0表示none，1表示login
	int EML_GetSmtpAuth();

	//!\brief       设置身份验证方式
	//!\param   nAuth   0表示none，1表示login，2表示plain
	//!\return  0表示成功，其它为错误码
	int EML_SetSmtpAuth(int nAuth);

	//!\brief   设置安全验证方式
	//!\param   SecMode 0表示none，1表示TLS，2表示SSL，3表示STARTTLS，4表示未设置
	//!\return  0表示成功，其它为错误码
	int EML_SetSecuritytMode(int SecMode);

	//!\brief   获取安全验证方式
	//!\return  0表示none，1表示TLS，2表示SSL，3表示STARTTLS，4表示未设置 其它为错误码
	int EML_GetSecuritytMode();

	//!\brief       获取是否启用附加图片
	//!\return  1表示启用，0表示禁用
	int EML_GetPic_att();

	//!\brief       设置是否启用附加图片
	//!\param   bEnable 1表示启用，0表示禁用
	//!\return  0表示成功，其它为错误码
	int EML_SetPic_att(int bEnable);
	
	//!\brief       获取发件人Email
	//!\param   pszAddress  返回发件人Email
	//!\param   nBufLen pszEmail指向的buf大小
	//!\return  0表示成功，其它为错误码
	int EML_GetAddresserEmail(char *pszEmail, int nBufLen);

	//!\brief       设置发件人Email
	//!\param   pszEmail    发件人Email
	//!\return  0表示成功，其它为错误码
	int EML_SetAddresserEmail(const char *pszEmail);

	//!\brief       获取发件人Email账户密码
	//!\param   pszPassword 返回发件人Email账户密码
	//!\param   nBufLen     pszPassword指向的buf大小
	//!\return  0表示成功，其它为错误码
	int EML_GetAddresserPassword(char *pszPassword, int nBufLen);

	//!\brief       设置发件人Email账户密码
	//!\param   pszPassword 发件人Email账户密码
	//!\return  0表示成功，其它为错误码
	int EML_SetAddresserPassword(const char *pszPassword);

    //!\brief       获取收件人Email
    //!\param   pszAddress  返回收件人Email
    //!\param   nBufLen pszEmail指向的buf大小
    //!\return  0表示成功，其它为错误码
    int EML_GetRecvAddresser(char *pszEmail, int nBufLen, int index);
    
    //!\brief       设置收件人Email
    //!\param   pszEmail    收件人Email
    //!\return  0表示成功，其它为错误码
    int EML_SetRecvAddresser(const char *pszEmail, int index);

	//!\brief       检查Email是否合法
	//!\param   pszEmail        检查Email是否合法
	//!\return  0表示成功，其它为错误码
	int EML_CheckEmail(const char *pszEmail);

	//!\brief           判断配置是否齐全
	//!\return  0表示成功，其它为错误码
	int EML_IsReady();

	//!\brief       发送Email列表
	//!\param   pszAddressee    收件人Email 列表
	//!\param   nAddresseeNum   收件人个数(最多支持15个用户)
	//!\param   pszTopic        主题（256 chars）
	//!\param   pszContext      内容（不限制大小）
	//!\param   pszAffix        附件（18个文件，路径名总长不超过100 x 18 chars?
	//!\param   bTestEML        是否为测试邮件
	//!\return  0表示成功，其它为错误码
	int EML_SendList(const char *pszAddresseeList[], int nAddresseeNum, const char *pszTopic = NULL, 
	                         const char *pszContext = NULL, const char *pszAffix = NULL, int bTestEML = 0);

	int EML_IsBusy();
	
    //!\brief   设置发送Email的时间间隔
    //!\param   nTimeInterval 发送Email的时间间隔(秒)
    //!\return  0表示成功，其它为错误码
    int EML_SetEmailTimeInterval(unsigned int nTimeInterval);
    
    //!\brief   获取发送Email的时间间隔
    //!\param   nTimeInterval 发送Email的时间间隔(秒)
    //!\return  0表示成功，其它为错误码
    int EML_GetEmailTimeInterval(unsigned int *nTimeInterval);

	//!\brief   获取发件人Email用户名
    //!\param   pszUserName 返回发件人Email用户名
    //!\param   nBufLen pszUserName指向的buf大小
    //!\return  0表示成功，其它为错误码
    int EML_GetSenderUserName(char *pszUserName, int nBufLen);
    
    //!\brief   设置发件人Email用户名
    //!\param   pszUserName 发件人Email用户名
    //!\return  0表示成功，其它为错误码
    int EML_SetSenderUserName(char *pszUserName);
    
    //!\brief   获取抄送人Email
    //!\param   pszEmail  返回抄送人Email
    //!\param   nBufLen pszEmail指向的buf大小
    //!\return  0表示成功，其它为错误码
    int EML_GetCCAddresser(char *pszEmail, int nBufLen);
    
    //!\brief   设置抄送人Email
    //!\param   pszEmail    抄送人Email
    //!\return  0表示成功，其它为错误码
    int EML_SetCCAddresser(const char *pszEmail);
    
    //!\brief   获取抄送人Email
    //!\param   pszEmail  返回抄送人Email
    //!\param   nBufLen pszEmail指向的buf大小
    //!\return  0表示成功，其它为错误码
    int EML_GetBCCAddresser(char *pszEmail, int nBufLen, int index);
    
    //!\brief   设置抄送人Email
    //!\param   pszEmail   收件人Email
    //!\return  0表示成功，其它为错误码
    int EML_SetBCCAddresser(const char *pszEmail);
    
    //!\brief   获取发送主题
    //!\param   pszSendSubject 返回发送主题
    //!\param   nBufLen pszSendSubject指向的buf大小
    //!\return  0表示成功，其它为错误码
    int EML_GetSendSubject(char *pszSendSubject, int nBufLen);
    
    //!\brief   设置发送主题
    //!\param   pszSendSubject 发件人Email主题
    //!\return  0表示成功，其它为错误码
    int EML_SetSendSubject(const char *pszSendSubject);
    
    //!\brief   获取发送Email正文
    //!\param   pszSendBody 返回发送Email正文
    //!\param   nBufLen pszSendBody指向的buf大小
    //!\return  0表示成功，其它为错误码
    int EML_GetSendBody(char *pszSendBody, int nBufLen);
    
    //!\brief   设置发送Email正文
    //!\param   pszSendBody Email正文
    //!\return  0表示成功，其它为错误码
    int EML_SetSendBody(const char *pszSendBody);
    
    //!\brief   获取发送Email的时间是否到
    //!\param   bTimeArrival 发送Email的时间是否到 0未到；1到了
    //!\return  0表示成功，其它为错误码
    int EML_GetEmailTimeIntervalIsArrival(unsigned int *bTimeArrival);
    
    //!\brief   获取测试邮件发送状态
    //!\param   pStatus 测试邮件发送状态
    //!\return  0表示成功，其它为错误码
    int EML_GetTestEmailSendStatus(int * pStatus);

	//!\brief  邮件发送线程处理函数
	void SendEmailThreadProcess();
	
    //!breif  时间计数器线程处理函数
    void TimerCountThreadProcess();

	
	int EML_GetTestEmailStatus(int *pStatus);

#ifdef USE_MEMORY_POOL
	//##ModelId=49B634E70241
	  IEmail();

	//##ModelId=49B634E70242
	 ~IEmail();
#else
  protected:
    //##ModelId=49B634E70241
  	  IEmail();
  
    //##ModelId=49B634E70242
     ~IEmail();
#endif

  private:
	//##ModelId=49BDD35B03A5
	static IEmail *instance;

	//!\brief   互斥锁
	pthread_mutex_t m_Mutex;

	//!\brief   互斥锁
	pthread_mutex_t m_status_Mutex;

	//线程ID
	pthread_t m_thrdID;

	//计时器线程ID
	pthread_t m_TimethrdID;
	
	//!\brief   退出发送线程信号
	sem_t m_semExitThread;

	//!\brief   唤醒发送    线程信号
	sem_t m_semWakeThread;

	//!\brief   邮件队列首编号
	volatile int m_nFirstMailIndex;

	//!\brief  邮件队列尾巴编号
	volatile int m_nLastMailIndex;

	//!\brief  邮件临时编号
	volatile int m_nTempMailIndex;

	//是否启用Email功能,0为禁用，1为启用
	int m_bEnable;

	//Smtp服务器地址(域名或者IP)
	char m_szSmtpAddress[NET_MAX_DOMAINNAME_LEN];

	//Smtp服务器身份验证方式,0为不验证，1为login
	int m_nSmtpAuth;

	//Smtp服务器端口(默认为25)
	int m_nSmtpPort;

	//0表示none，1表示SSL，2表示STL，3表示STARTTLS
	int m_nSmtpSecurityMode;

	//SMTP_SECURITY_TYPE m_eSecurityType;

	//是否启用附加图片，0为禁用，1为启用
	int m_bPic_att;
	
	//发件人Email
	char m_szAddresserEmail[NET_MAX_EMAIL_LEN];

	//发件人Email帐户密码
	char m_szAddresserPassword[NET_MAX_PASSWORD_LEN];

	//收件人Email帐户
	char m_sRecvAddresser[3][NET_MAX_PASSWORD_LEN];

	//Smtp发送方用户名
    char m_szSenderUserName[NET_MAX_USER_NAME_LEN];

    //Smtp抄送人CC
	char m_szCCAddresserEmail[NET_MAX_EMAIL_LEN];
    
    //Smtp密送人BCC
	char m_szBCCAddresserEmail[NET_MAX_EMAIL_LEN];

	//主题
	char m_sSubjectTopic[NET_MAX_SUBJECT_LEN];

	//信息正文
	char m_szEmailBody[NET_MAX_EMAIL_BODY_LEN];

	//发送邮件时间间隔
	unsigned int m_nSmtpSendTimeInterval;

	//发送Email状态
	int m_nTestEmailSendStatus;
	int m_bTestEmailOverFlag;

	int m_bSendEmailIsBusy;
	
    //当前发送邮件是否为测试邮件(0:触发邮件 1:测试邮件)
    int m_bTestEmail;

	//计数器
	unsigned int m_TimeCount;

	//上次发送Email的时间
	unsigned int m_LastSendEMLTime;

	//编码句柄
	void * m_hEnc;

    //最大支持5个附件
    char m_sAttraAffix[5][NET_MAX_EMAIL_LEN];

    //当前附件个数
    unsigned int m_nCurrAtrraCount;

	int m_nTestEmailstaue;

    //Email接口
    CSmtp *m_pCSmtp;

    //利用SMTP发送Email接口
    int SendSmtpEmail(const char *pEmailBodyFile);

	//!\brief       发送邮件
	//!\param   pszAddresseeList    收件人Email 列表
	//!\param   nAddresseeNum       收件人个数(最多15个)
	//!\param   pszContext      内容（不限制大小）
	//!\param   pszAffix        附件（18个文件，路径名总长不超过100 x 18 chars
	//!\param   bTestEml        是否为测试邮件
	//!\return  0表示成功，其它为错误码
	int SendMail(const char *pszAddresseeList[], int nAddresseeNum, const char *pszTopic = NULL, 
	                 const char *pszContext = NULL, const char *pszAffix = NULL, int bTestEml = 0);

	//!\brief   获取测试邮件发送状态
	//!\return  0表示成功，其它为错误码
	int GetTestEmailSendStatus(void);
};

#endif							/* _INC_IEMAIL_49B6325C0261_INCLUDED */
