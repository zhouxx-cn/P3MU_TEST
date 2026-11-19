
/*!
 *\file		MPU_Email_API.h
 *\brief		MPU模块，Email服务器相关接口定义
 *\author		林云
 *\date		2009/03/20

功能：
-#	邮件服务器参数配置（域名、用户名、密码）
-#	发送邮件

说明：
-#	邮件为异步发送
 */


#ifndef _INC_EMAIL_API_H_
#define _INC_EMAIL_API_H_




//!\brief       获取是否启用SMTP服务器
//!\return  1表示启用，0表示不启用
int MPU_EML_GetEnable();

//!\brief       设置是否启用SMTP服务器
//!\param   bEnable 1表示启用，0表示禁用
//!\return  0表示成功，其它为错误码
int MPU_EML_SetEnable(int bEnable);

//!\brief       获取SMTP服务器地址(域名或者IP地址)
//!\param   pszAddress  返回SMTP服务器地址
//!\param   nBufLen     pszAddress指向的buf大小(256字节)
//!\return  0表示成功，其它为错误码
int MPU_EML_GetSmtpAddress(char *pszAddress, int nBufLen);

//!\brief       设置SMTP服务器地址(域名或者IP地址)
//!\param   pszAddress  SMTP服务器地址
//!\return  0表示成功，其它为错误码
int MPU_EML_SetSmtpAddress( char *pszAddress);

//!\brief       获取SMTP服务器端口
//!\return  返回端口号
unsigned short MPU_EML_GetSmtpPort();

//!\brief       设置SMTP服务器端口
//!\param   wPort   SMTP服务器端口
//!\return  0表示成功，其它为错误码
int MPU_EML_SetSmtpPort(unsigned short wPort);

//!\brief       获取身份验证方式
//!\return  0表示none，1表示login，2表示plain
int MPU_EML_GetSmtpAuth();

//!\brief       设置身份验证方式
//!\param   nAuth   0表示none，1表示login
//!\return  0表示成功，其它为错误码
int MPU_EML_SetSmtpAuth(int nAuth);

//!\brief   获取安全验证方式
//!\return  0表示none，1表示TLS，2表示SSL，3表示STARTTLS，4表示未设置 其它为错误码
int MPU_EML_GetSecuritytMode();

//!\brief   设置安全验证方式
//!\param   SecMode 0表示none，1表示TLS，2表示SSL，3表示STARTTLS，4表示未设置
//!\return  0表示成功，其它为错误码
int MPU_EML_SetSecuritytMode(int SecMode);

//!\brief       获取是否启用附加图片
//!\return  1表示启用，0表示禁用
int MPU_EML_GetPic_att();

//!\brief       设置是否启用附加图片
//!\param   bEnable 1表示启用，0表示禁用
//!\return  0表示成功，其它为错误码
int MPU_EML_SetPic_att(int bEnable);

//!\brief       获取发件人Email
//!\param   pszAddress  返回发件人Email
//!\param   nBufLen     pszEmail指向的buf大小(256字节)
//!\return  0表示成功，其它为错误码
int MPU_EML_GetAddresserEmail(char *pszEmail, int nBufLen);

//!\brief       设置发件人Email
//!\param   pszEmail    发件人Email
//!\return  0表示成功，其它为错误码
int MPU_EML_SetAddresserEmail( char *pszEmail);

//!\brief       获取发件人Email账户密码
//!\param   pszPassword 返回发件人Email账户密码
//!\param   nBufLen     pszPassword指向的buf大小(64字节)
//!\return  0表示成功，其它为错误码
int MPU_EML_GetAddresserPassword(char *pszPassword, int nBufLen);

//!\brief       设置发件人Email账户密码
//!\param   pszPassword 发件人Email账户密码
//!\return  0表示成功，其它为错误码
int MPU_EML_SetAddresserPassword( char *pszPassword);

//!\brief       获取收件人Email
//!\param   pszAddress  返回收件人Email
//!\param   nBufLen     pszEmail指向的buf大小
//!\return  0表示成功，其它为错误码
int MPU_EML_GetRecvAddresser(char *pszEmail, int nBufLen, int index);

//!\brief       设置收件人Email
//!\param   pszEmail    收件人Email
//!\return  0表示成功，其它为错误码
int MPU_EML_SetRecvAddresser(const char *pszEmail, int index);

//!\brief   获取发送Email的时间间隔
//!\param   nTimeInterval 发送Email的时间间隔(秒)
//!\return  0表示成功，其它为错误码
int MPU_EML_GetSendTimeInterval(unsigned int *nTimeInterval);

//!\brief   设置发送Email的时间间隔
//!\param   nTimeInterval 发送Email的时间间隔(秒)
//!\return  0表示成功，其它为错误码
int MPU_EML_SetSendTimeInterval(unsigned int nTimeInterval);

//!\brief   获取发送Email的主题 
//!\param   pTopic 		获取的主题内容
//!\param   nBufLen     主题大小
//!\return  0表示成功，其它为错误码
int MPU_EML_GetSubject(char *pTopic,int nBufLen);

//!\brief   设置发送Email的主题 16_12_23 add
//!\param   pTopic 		设置的主题内容
//!\param   nBufLen     主题大小
//!\return  0表示成功，其它为错误码
int MPU_EML_SetSubject(const char *pTopic);

//!\brief   获取发送Email的内容
//!\param   pBody 		设置的邮件主体
//!\param   nBufLen     主体大小
//!\return  0表示成功，其它为错误码
int MPU_EML_GetBody(char *pBody,int nBufLen);

//!\brief   设置发送Email的内容
//!\param   pBody 		设置的邮件主体
//!\param   nBufLen     主体大小
//!\return  0表示成功，其它为错误码
int MPU_EML_SetBody(const char *pBody);

int MPU_EML_GetEmailTimeIntervalIsArrival(unsigned int *bTimeIsArrival);


//!\brief           判断配置是否齐全
//!\return  0表示成功，其它为错误码
int MPU_EML_IsReady();

//!\brief       检查Email是否合法
//!\param   pszEmail        检查Email是否合法
//!\return  0表示成功，其它为错误码
int MPU_EML_CheckEmail( char *pszEmail);


//!\brief       发送Email列表
//!\param   pszAddresseeList    收件人Email 列表
//!\param   nAddresseeNum   收件人个数(最多支持15个用户)
//!\param   pszTopic        主题（256 chars）
//!\param   pszContext      内容（不限制大小）
//!\param   pszAffix        附件（18个文件，路径名总长不超过80 x 18 chars）
//!\return  0表示成功，其它为错误码
int MPU_EML_SendList( char *pszAddresseeList[], int nAddresseeNum,  char *pszTopic = NULL,  char *pszContext =
					 NULL,  char *pszAffix = NULL, int bTestEML = 0);

int MPU_EML_IsBusy();

//!\brief   获取测试邮件发送状态
//!\param   pStatus 测试邮件发送状态
//!\return  0表示成功，其它为错误码
int MPU_EML_GetTestSendStatus(int *pStatus);

int MPU_EML_GetTeststatus(int *pStatus);

#endif							/* _INC_EMAIL_API_H_ */
