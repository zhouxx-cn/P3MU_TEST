
/*!
 *\file	MPU_Email_API.cpp
 *\brief	MPU模块，Email服务器相关接口实现
 *\author	林云
 *\date	2009/03/20

功能：
-#	邮件服务器参数配置（域名、用户名、密码）
-#	发送邮件

说明：
-#	邮件为异步发送
 */



#ifndef DEBUG_NOT_USE_NET_LIB	//调试时，不调用NET库

#include "NET/IEmail.h"			//EMail管理接口
#include "MPU_global.h"

/**************************************************************************
 *                                                     局部宏定义                                                              *
 **************************************************************************/

//调用EMail管理器执行动作
#define MPU_EML_CALL(x)									\
do														\
{														\
	int nRet;												\
														\
	IEmail *pMgr = IEmail::Instance();							\
														\
	if (NULL != pMgr)										\
	{													\
		nRet = pMgr->x;									\
		if (IDVR_OK != nRet)								\
		{												\
			MPU_ERR("%s, nRet = %d\n", #x, nRet);			\
		}												\
	}													\
	else													\
	{													\
		MPU_ERR("Get IEmail instance fail!\n");				\
		MPU_CHK(NULL != pMgr);							\
		nRet = IDVR_MPU_EML_NO_MANAGER_INSTANCE;			\
	}													\
														\
	return nRet;											\
}														\
while (0)


//调用EMail管理器获取参数
#define MPU_EML_GET(DataType, DefaultValue, Function)		\
do														\
{														\
	DataType retValue;										\
														\
	IEmail *pMgr = IEmail::Instance();							\
														\
	if (NULL != pMgr)										\
	{													\
		retValue = pMgr->Function;							\
	}													\
	else													\
	{													\
		MPU_ERR("Get IEmail instance fail!\n");				\
		MPU_CHK(NULL != pMgr);							\
		retValue = DefaultValue;							\
	}													\
														\
	return retValue;										\
}														\
while (0)


#else							/* #ifndef DEBUG_NOT_USE_NET_LIB */


//调用EMail管理器执行动作
#define MPU_EML_CALL(x)									\
do														\
{														\
	return IDVR_OK;										\
}														\
while (0)

//调用EMail管理器获取参数
#define MPU_EML_GET(DataType, DefaultValue, Function)		\
do														\
{														\
	return DefaultValue;									\
}														\
while (0)


#endif							/* #ifndef DEBUG_NOT_USE_NET_LIB */


/**************************************************************************
 *                                                          模块内部接口                                                   *
 **************************************************************************/

//!\brief   初始化EMail
//!\return  0表示成功，其它为错误码
int EML_Init()
{
	int nRet = IDVR_OK;
	
#if defined (__USE_EMAIL__)
#ifndef DEBUG_NOT_USE_NET_LIB	//调试时，不调用NET库

	IEmail *pMgr = IEmail::Instance();

	if (NULL == pMgr)
	{
		MPU_ERR("Get IEmail instance fail!\n");
		MPU_CHK(NULL != pMgr);
		nRet = IDVR_MPU_EML_NO_MANAGER_INSTANCE;
	}

#endif							/* #ifndef DEBUG_NOT_USE_NET_LIB */
#endif

	return nRet;
}

//!\brief   逆初始化EMail
//!\return  0表示成功，其它为错误码
int EML_Uninit()
{
	
#if defined (__USE_EMAIL__)
#ifndef DEBUG_NOT_USE_NET_LIB	//调试时，不调用NET库

	IEmail::Release();

#endif							/* #ifndef DEBUG_NOT_USE_NET_LIB */
#endif

	return IDVR_OK;
}


/**************************************************************************
 *                                                                全局接口                                                         *
 **************************************************************************/



//!\brief       获取是否启用SMTP服务器
//!\return  1表示启用，0表示不启用
int MPU_EML_GetEnable()
{
#if defined (__USE_EMAIL__)
	MPU_EML_GET(int, 0, EML_GetEnable());
#else
	return 0;
#endif
}

//!\brief       设置是否启用SMTP服务器
//!\param   bEnable 1表示启用，0表示禁用
//!\return  0表示成功，其它为错误码
int MPU_EML_SetEnable(int bEnable)
{
#if defined (__USE_EMAIL__)
	MPU_EML_CALL(EML_SetEnable(bEnable));
#else
		return 0;
#endif
}

//!\brief       获取SMTP服务器地址(域名或者IP地址)
//!\param   pszAddress  返回SMTP服务器地址
//!\param   nBufLen     pszAddress指向的buf大小
//!\return  0表示成功，其它为错误码
int MPU_EML_GetSmtpAddress(char *pszAddress, int nBufLen)
{
#if defined (__USE_EMAIL__)
	MPU_EML_CALL(EML_GetSmtpAddress(pszAddress, nBufLen));
#else
		return 0;
#endif
}

//!\brief       设置SMTP服务器地址(域名或者IP地址)
//!\param   pszAddress  SMTP服务器地址
//!\return  0表示成功，其它为错误码
int MPU_EML_SetSmtpAddress( char *pszAddress)
{
#if defined (__USE_EMAIL__)
	MPU_EML_CALL(EML_SetSmtpAddress(pszAddress));
#else
		return 0;
#endif
}

//!\brief       获取SMTP服务器端口
//!\return  返回端口号
unsigned short MPU_EML_GetSmtpPort()
{
#if defined (__USE_EMAIL__)
	MPU_EML_GET(unsigned short, 25, EML_GetSmtpPort());
#else
		return 0;
#endif
}

//!\brief       设置SMTP服务器端口
//!\param   wPort   SMTP服务器端口
//!\return  0表示成功，其它为错误码
int MPU_EML_SetSmtpPort(unsigned short wPort)
{
#if defined (__USE_EMAIL__)
	MPU_EML_CALL(EML_SetSmtpPort(wPort));
#else
		return 0;
#endif
}

//!\brief       获取身份验证方式
//!\return  0表示none，1表示login，2表示plain
int MPU_EML_GetSmtpAuth()
{
#if defined (__USE_EMAIL__)
	MPU_EML_GET(int, 1, EML_GetSmtpAuth());
#else
		return 0;
#endif
}

//!\brief       设置身份验证方式
//!\param   nAuth   0表示none，1表示login
//!\return  0表示成功，其它为错误码
int MPU_EML_SetSmtpAuth(int nAuth)
{
#if defined (__USE_EMAIL__)
	MPU_EML_CALL(EML_SetSmtpAuth(nAuth));
#else
		return 0;
#endif
}

//!\brief   获取安全验证方式
//!\return  0表示none，1表示TLS，2表示SSL，3表示STARTTLS，4表示未设置 其它为错误码
int MPU_EML_GetSecuritytMode()
{
#if defined (__USE_EMAIL__)
	MPU_EML_GET(int, 1, EML_GetSecuritytMode());
#else
		return 0;
#endif
}

//!\brief   设置安全验证方式
//!\param   SecMode 0表示none，1表示TLS，2表示SSL，3表示STARTTLS，4表示未设置
//!\return  0表示成功，其它为错误码
int MPU_EML_SetSecuritytMode(int SecMode)
{
#if defined (__USE_EMAIL__)
	MPU_EML_CALL(EML_SetSecuritytMode(SecMode));
#else
		return 0;
#endif
}

//!\brief       获取是否启用附加图片
//!\return  1表示启用，0表示禁用
int MPU_EML_GetPic_att()
{
#if defined (__USE_EMAIL__)
	MPU_EML_GET(int, 0, EML_GetPic_att());
#else
		return 0;
#endif
}

//!\brief       设置是否启用附加图片
//!\param   bEnable 1表示启用，0表示禁用
//!\return  0表示成功，其它为错误码
int MPU_EML_SetPic_att(int bEnable)
{
#if defined (__USE_EMAIL__)
	MPU_EML_CALL(EML_SetPic_att(bEnable));
#else
		return 0;
#endif
}

//!\brief       获取发件人Email
//!\param   pszAddress  返回发件人Email
//!\param   nBufLen     pszEmail指向的buf大小
//!\return  0表示成功，其它为错误码
int MPU_EML_GetAddresserEmail(char *pszEmail, int nBufLen)
{
#if defined (__USE_EMAIL__)
	MPU_EML_CALL(EML_GetAddresserEmail(pszEmail, nBufLen));
#else
		return 0;
#endif
}

//!\brief       设置发件人Email
//!\param   pszEmail    发件人Email
//!\return  0表示成功，其它为错误码
int MPU_EML_SetAddresserEmail( char *pszEmail)
{
#if defined (__USE_EMAIL__)
	MPU_EML_CALL(EML_SetAddresserEmail(pszEmail));
#else
		return 0;
#endif
}

//!\brief       获取发件人Email账户密码
//!\param   pszPassword 返回发件人Email账户密码
//!\param   nBufLen     pszPassword指向的buf大小
//!\return  0表示成功，其它为错误码
int MPU_EML_GetAddresserPassword(char *pszPassword, int nBufLen)
{
#if defined (__USE_EMAIL__)
	MPU_EML_CALL(EML_GetAddresserPassword(pszPassword, nBufLen));
#else
		return 0;
#endif
}

//!\brief       设置发件人Email账户密码
//!\param   pszPassword 发件人Email账户密码
//!\return  0表示成功，其它为错误码
int MPU_EML_SetAddresserPassword( char *pszPassword)
{
#if defined (__USE_EMAIL__)
	MPU_EML_CALL(EML_SetAddresserPassword(pszPassword));
#else
		return 0;
#endif
}

//!\brief       获取收件人Email
//!\param   pszAddress  返回收件人Email
//!\param   nBufLen     pszEmail指向的buf大小
//!\return  0表示成功，其它为错误码
int MPU_EML_GetRecvAddresser(char *pszEmail, int nBufLen, int index)
{
#if defined (__USE_EMAIL__)
	MPU_EML_CALL(EML_GetRecvAddresser(pszEmail, nBufLen, index));
#else
		return 0;
#endif
}

//!\brief       设置收件人Email
//!\param   pszEmail    收件人Email
//!\return  0表示成功，其它为错误码
int MPU_EML_SetRecvAddresser(const char *pszEmail, int index)
{
#if defined (__USE_EMAIL__)
	MPU_EML_CALL(EML_SetRecvAddresser(pszEmail, index));
#else
		return 0;
#endif
}

//!\brief   获取发送Email的时间间隔
//!\param   nTimeInterval 发送Email的时间间隔(秒)
//!\return  0表示成功，其它为错误码
int MPU_EML_GetSendTimeInterval(unsigned int *nTimeInterval)
{
#if defined (__USE_EMAIL__)
	MPU_EML_CALL(EML_GetEmailTimeInterval(nTimeInterval));
#else
		return 0;
#endif
}

//!\brief   设置发送Email的时间间隔
//!\param   nTimeInterval 发送Email的时间间隔(秒)
//!\return  0表示成功，其它为错误码
int MPU_EML_SetSendTimeInterval(unsigned int nTimeInterval)
{
#if defined (__USE_EMAIL__)
    MPU_EML_CALL(EML_SetEmailTimeInterval(nTimeInterval));
#else
		return 0;
#endif
}

//!\brief   获取发送Email的主题 
//!\param   pTopic 		获取的主题内容
//!\param   nBufLen     主题大小
//!\return  0表示成功，其它为错误码
int MPU_EML_GetSubject( char *pTopic,int nBufLen)
{
#if defined (__USE_EMAIL__)
	MPU_EML_CALL(EML_GetSendSubject(pTopic,nBufLen));
#else
		return 0;
#endif
}

//!\brief   设置发送Email的主题 16_12_23 add
//!\param   pTopic 		设置的主题地址
//!\return  0表示成功，其它为错误码
int MPU_EML_SetSubject(const char *pTopic)
{
#if defined (__USE_EMAIL__)
	MPU_EML_CALL(EML_SetSendSubject(pTopic));
#else
		return 0;
#endif

}
//!\brief   获取发送Email的内容
//!\param   pBody 		设置的邮件主体
//!\param   nBufLen     主体大小
//!\return  0表示成功，其它为错误码
int MPU_EML_GetBody(char *pBody,int nBufLen)
{
#if defined (__USE_EMAIL__)
	MPU_EML_CALL(EML_GetSendBody(pBody,nBufLen));
#else
		return 0;
#endif
}

//!\brief   设置发送Email的内容
//!\param   pBody 		设置的邮件主体
//!\return  0表示成功，其它为错误码
int MPU_EML_SetBody(const char *pBody)
{
#if defined (__USE_EMAIL__)
	MPU_EML_CALL(EML_SetSendBody(pBody));
#else
		return 0;
#endif
}



int MPU_EML_GetEmailTimeIntervalIsArrival(unsigned int *bTimeIsArrival)
{
#if defined (__USE_EMAIL__)
    MPU_EML_CALL(EML_GetEmailTimeIntervalIsArrival(bTimeIsArrival));
#else
		return 0;
#endif
}
//!\brief           判断配置是否齐全
//!\return  0表示成功，其它为错误码
int MPU_EML_IsReady()
{
#if defined (__USE_EMAIL__)
	MPU_EML_GET(int, 0, EML_IsReady());
#else
		return 0;
#endif
}

//!\brief       检查Email是否合法
//!\param   pszEmail        检查Email是否合法
//!\return  0表示成功，其它为错误码
int MPU_EML_CheckEmail( char *pszEmail)
{
#if defined (__USE_EMAIL__)
	MPU_EML_CALL(EML_CheckEmail(pszEmail));
#else
		return 0;
#endif
}



//!\brief     发送Email列表
//!\param   pszAddresseeList    收件人Email 列表
//!\param   nAddresseeNum       收件人个数(最多支持15个用户)
//!\param   pszTopic        主题（256 chars）
//!\param   pszContext      内容（不限制大小）
//!\param   pszAffix        附件（18个文件，路径名总长不超过80 x 18 chars?//!\param   bTestEML        是否为测试邮件
//!\return  0表示成功，其它为错误码
int MPU_EML_SendList( char *pszAddresseeList[], int nAddresseeNum,  char *pszTopic,  char *pszContext,  char *pszAffix, int bTestEML)
{
#if defined (__USE_EMAIL__)
	MPU_EML_CALL(EML_SendList((const char **)pszAddresseeList, nAddresseeNum, pszTopic, pszContext, pszAffix, bTestEML));
#else
		return 0;
#endif
}

int MPU_EML_IsBusy()
{
#if defined (__USE_EMAIL__)
	MPU_EML_CALL(EML_IsBusy());
#else
		return 0;
#endif
}

//!\brief   获取测试邮件发送状态
//!\param   pStatus 测试邮件发送状态?//!\return  0表示成功，其它为错误码
int MPU_EML_GetTestSendStatus(int *pStatus)
{
#if defined (__USE_EMAIL__)
    MPU_EML_CALL(EML_GetTestEmailSendStatus(pStatus));
#else
		return 0;
#endif
}


int MPU_EML_GetTeststatus(int *pStatus)
{
#if defined (__USE_EMAIL__)
    MPU_EML_CALL(EML_GetTestEmailStatus(pStatus));
#else
		return 0;
#endif
}


/**************************************************************************
 *                                                     注销局部宏定义                                                  *
 **************************************************************************/

#undef MPU_EML_CALL				//调用EMail管理器执行动作
#undef MPU_EML_GET				//调用EMail管理器获取参数值
