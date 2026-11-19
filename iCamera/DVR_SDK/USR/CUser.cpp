/*!
 *\file		CUser.cpp
 *\brief	USR模块 CUsr模型类函数定义
 *\author	yuk@higheasyrd.com
 *\date		2009/04/14

功能：
-# 	支持用户基本信息和权限的获取
-#  支持用户信息和权限的修改
-#  支持用户的启用和禁用

*/

#include <string>
#include "idvr_errno.h"
#include "USR/CUserManager.h"
#include "CUser.h"
#include "USRTrace.h"
//#include "hisdk.h"
#include "zwsdk.h"
// #include "sys_banned.h"
#include "global_gather.h"
/**************************************************************************
 *                     CUser函数实现                                       *
 **************************************************************************/

//##ModelId=499CCB4301E7
CUser::CUser():m_nUserId(-1), m_bEnable(0)
{
	// TODO: Add your specialized code here.
	memset(m_szUsername, 0, USERNAME_BUF_LEN);
	memset(m_szPassword, 0, PASSWORD_BUF_LEN);
	memset(m_szEmail, 0, EMAIL_BUF_LEN);
	memset(m_szMacAddress, 0, MAC_ADDRESS_BYTES);
	m_dwPreview[0] = 0;
	m_dwPreview[1] = 0;
	m_dwPlayback[0] = 0;
	m_dwPlayback[1] = 0;
	m_dwPtzCtrl[0] = 0;
	m_dwPtzCtrl[1] = 0;
	m_dwManualRecord[0] = 0;
	m_dwManualRecord[1] = 0;
	m_dwLocal = 0;
	m_dwRemote = 0;
}

//##ModelId=499CCB480342
CUser::~CUser()
{
	// TODO: Add your specialized code here.
}

//!\brief 创建一个新用户
//!\param strUsername 用户名
//!\brief strPassword 密码
//!\return NULL表示创建失败, 其他表示用户指针
//##ModelId=49BDB96C02BF
CUser * CUser::GetInstance(const char * strUsername, char * strPassword)
{
	if ((strUsername == NULL)
		|| (0 == strlen(strUsername))
		|| (strlen(strUsername) > MAX_USERNAME_LEN) 
		|| (strlen(strPassword) > MAX_PASSWORD_LEN))
	{
		USR_ERROR("ERR: param error, strUserName=%s, strPassword=%s"
			, strUsername, strPassword);
		return NULL;
	}
	
	CUser *pUser = new CUser();
	if (pUser != NULL)
	{
		strncpy(pUser->m_szUsername, strUsername, sizeof(pUser->m_szUsername) - 1);
		strncpy(pUser->m_szPassword, strPassword, sizeof(pUser->m_szPassword) - 1);
	}
	
	// TODO: Add your specialized code here.
	return (CUser *)pUser;
}

//!\brief 获取用户名
//!\param strUserName 存储用户名的缓冲区
//!\param nLength 缓冲区长度 
//!\return 0表示获取成功 其他为错误码
//!\note 缓冲区长度小于用户名字符串长度时返回参数错误
//##ModelId=499BC0270074
int CUser::GetUserName(char *strUsername, int nLength) const
{
	//校验参数合法性
	//如果缓冲区长度小于用户名字符串长度返回参数错误
	//避免获得用户名不正确
	if ((NULL == strUsername) || (nLength < (int)MAX_USERNAME_LEN))
	{
		//参数非法
		USR_ERROR("para error, strPassword=%s, nLength=%d\n"
				, strUsername
				, nLength);
		return IDVR_ERROR_PARAMETER;
	}

	strncpy(strUsername, m_szUsername,nLength - 1);
	return 0;
}

//!\brief 获取用户密码
//!\param strPassword 存储用户密码的缓冲区
//!\param nLength 缓冲区长度 
//!\return 0表示获取成功 其他为错误码
//!\note 缓冲区长度小于用户密码字符串长度时返回参数错误
//##ModelId=499BC0510088
int CUser::GetPassword(char *strPassword, int nLength) const
{
	//校验参数合法性
	//如果缓冲区长度小于密码字符串长度返回参数错误
	//避免获得用户名不正确
	if ((NULL == strPassword) || (nLength < (int)MAX_PASSWORD_LEN))
	{
		//参数非法
		USR_DBG("para error, strPassword=%s, nLength=%d\n"
				, strPassword
				, nLength);
		return IDVR_ERROR_PARAMETER;
	}
	
	strncpy(strPassword, m_szPassword,nLength - 1);

	//成功返回
	return 0;
}

//!\brief 设置用户密码
//!\param strPassword 新用户密码字符串
//!\return 0表示设置成功 其他为错误码
//!\note 字符串strPassword为空时,清除用户密码
//##ModelId=499BC06B00A4
int CUser::SetPassword(const char *strPassword)
{
	//设置的密码长度大于最大密码长度时返回错误码
	if ((strPassword != NULL) && (strlen(strPassword) > MAX_PASSWORD_LEN))
	{
		USR_ERROR("[%s]ERR:para error, strPassword=%s", __FUNCTION__, strPassword);
		return IDVR_ERROR_PARAMETER;
	}

	if (NULL == strPassword)
	{
		//清空密码
		memset(m_szPassword, 0, PASSWORD_BUF_LEN);
	}
	else
	{
		//设置新的密码
		strncpy(m_szPassword, strPassword,sizeof(m_szPassword) - 1);
	}
	return 0;
}

//!\brief 获取用户是否被禁用
//!\return 0表示被禁用 1表示已启用
//##ModelId=499BC18302FF
int CUser::GetEnable() const
{
	// TODO: Add your specialized code here.
	return (int)m_bEnable;
}

//!\brief 设置用户是否被禁用
//!\param 0表示禁用 大于0表示启用
//##ModelId=499BC1A000D0
int CUser::SetEnable(int bEnable)
{
	if (bEnable < 0)
	{
		USR_ERROR("[%s]ERR: para error, bEnable=%d", __FUNCTION__, bEnable);
		return IDVR_ERROR_PARAMETER;
	}
	else if (bEnable > 0)
	{
		m_bEnable = 1;
	}
	else
	{
		m_bEnable = 0;
	}
	return 0;
}

//!\brief 设置用户Email
//!\param strEmail email字符串
//!\return 0表示设置成功 其他表示错误码
//!\note NULL表示不设置Email
//##ModelId=499CCA5801B7
int CUser::SetEmailAddress(char *strEmail)
{
	if ((strEmail != NULL) && (strlen(strEmail) > MAX_EMAIL_LEN))
	{
		USR_ERROR("[%s]ERR:para error, strEmail=%s", __FUNCTION__, strEmail);
		return IDVR_ERROR_PARAMETER;
	}

	if (NULL == strEmail)
	{
		memset(m_szEmail, 0, EMAIL_BUF_LEN);
	}
	else
	{
		strncpy(m_szEmail, strEmail, sizeof(m_szEmail) - 1);
	}
	return 0;
}

//!\brief 获取用户email
//!\param strEmail 存储用户email的缓冲区
//!\param nLength 缓冲区长度 
//!\return 0表示获取成功 其他为错误码
//!\note 缓冲区长度小于用户email字符串长度时返回参数错误
//##ModelId=499CCA6B01DC
int CUser::GetEmailAddress(char *strEmail, int nLength) const
{
	//校验参数合法性
	//如果缓冲区长度小于密码字符串长度返回参数错误
	//避免获得用户名不正确
	if ((NULL == strEmail) || (nLength < (int)MAX_EMAIL_LEN))
	{
		//参数非法
		USR_DBG("para error, strEmail=%s, nLength=%d\n"
				, strEmail
				, nLength);
		return IDVR_ERROR_PARAMETER;
	}

	strncpy(strEmail, m_szEmail, nLength - 1);
	// TODO: Add your specialized code here.
	return 0;
}

//!\brief 设置用户绑定的物理地址
//!\param byteMac MAC地址
//!\return 0表示设置成功 其他表示错误码
//!\note NULL表示不设置绑定的物理地址
//##ModelId=499CCA8300BE
int CUser::SetBindMacAddress(unsigned char byteMac[MAC_ADDRESS_BYTES])
{
	if (byteMac != NULL)
	{
		memcpy(m_szMacAddress, byteMac, MAC_ADDRESS_BYTES);
	}
	else
	{
		memset(m_szMacAddress, 0, MAC_ADDRESS_BYTES);
	}
	return 0;
}

//!\brief 获取用户绑定的物理地址
//!\param byteMac 存储用户绑定物理地址的缓冲区
//!\return 0表示获取成功 其他为错误码
//##ModelId=499CCA9401F9
int CUser::GetBindMacAddress(unsigned char byteMac[MAC_ADDRESS_BYTES]) const
{
	if (NULL == byteMac)
	{
		USR_ERROR("[%s]ERR: para error, byteMac=%p", __FUNCTION__, byteMac);
		return IDVR_ERROR_PARAMETER_INVALID_POINTER;
	}
	else
	{
		memcpy(byteMac, m_szMacAddress, MAC_ADDRESS_BYTES);
	}
	// TODO: Add your specialized code here.
	return 0;
}

//!\brief 获取用户与通道相关的权限
//!\param action 通道相关的权限
//!		CHANNEL_PREVIEW 预览
//!		CHANNEL_PLAYBACK 回放
//!		CHANNEL_PTZCTRL 云台控制
//!		CHANNEL_MANUALRECORD 手动录像
//!\param nChannel 通道号,从0开始
//!\param bRemote 0表示本地权限 1表示远程权限
//!\return 0表示无权限 1为有权限 其他为错误码
//##ModelId=49ACF31B005D
int CUser::GetChannelPrivilege(CHANNEL_ACTION action, int nChannel, int bRemote) const
{
	unsigned long nResult = 0;
	CUserManager *pManager = CUserManager::USR_GetInstance();
	if (NULL == pManager)
	{
		USR_ERROR("Get Manager Instance Failed\n");
		return 0;
	}

	const static int nChannelCount = pManager->USR_GetChanNum();
	const static unsigned long dwChMask = ((1 << nChannelCount) - 1);
	if ((nChannel < 0) || (nChannel >= nChannelCount)
		|| (bRemote < 0) || (bRemote > 1))
	{
		USR_ERROR("[%s]ERR: param error, action = %d, nChannel = %d, bRemote = %d",
			__FUNCTION__, action, nChannel, bRemote);
		return IDVR_ERROR_PARAMETER;
	}

	//高级模式按位获取通道权限
	if (pManager->USR_GetWorkMode())
	{
		switch (action)
		{
		case CHANNEL_PREVIEW:
			nResult = (m_dwPreview[bRemote] >> nChannel) & 1;
			break;
		case CHANNEL_PLAYBACK:
			nResult = (m_dwPlayback[bRemote] >> nChannel) & 1;
			break;
		case CHANNEL_PTZCTRL:
			nResult = (m_dwPtzCtrl[bRemote] >> nChannel) & 1;
			break;
		case CHANNEL_MANUALRECORD:
			nResult = (m_dwManualRecord[bRemote] >> nChannel) & 1;
			break;
		default:
			USR_ERROR("[%s]ERR: param error, action = %d, nChannel = %d, bRemote = %d",
				__FUNCTION__, action, nChannel, bRemote);
			return 0;
			break;
		}
	}
	//普通模式，则只有所有通道有权限才能执行相应动作
	else
	{
		switch (action)
		{
		case CHANNEL_PREVIEW:
			nResult = ((m_dwPreview[bRemote] & dwChMask) == dwChMask);
			break;
		case CHANNEL_PLAYBACK:
			nResult = ((m_dwPlayback[bRemote] & dwChMask) == dwChMask);
			break;
		case CHANNEL_PTZCTRL:
			nResult = ((m_dwPtzCtrl[bRemote] & dwChMask) == dwChMask);
			break;
		case CHANNEL_MANUALRECORD:
			nResult = ((m_dwManualRecord[bRemote] & dwChMask) == dwChMask);
			break;
		default:
			USR_ERROR("[%s]ERR: param error, action = %d, nChannel = %d, bRemote = %d",
				__FUNCTION__, action, nChannel, bRemote);
			return 0;
			break;
		}
	}

	// TODO: Add your specialized code here.
	return (int)nResult;
}

//!\brief 设置通道相关操作权限
//!\param bEnable 1表示有权限 0表示无权限
//!\param action 操作代码
//!		CHANNEL_PREVIEW 预览
//!		CHANNEL_PLAYBACK 回放
//!		CHANNEL_PTZCTRL 云台控制
//!		CHANNEL_MANUALRECORD 手动录像
//!\param nChannel 通道号,从0开始
//!\param bRemote 0表示本地, 1表示远程
//!\return 0表示设置成功 -1表示设置失败
//##ModelId=49ACF31B0196
int CUser::SetChannelPrivilege(int bEnable, CHANNEL_ACTION action, int nChannel, int bRemote)
{
	CUserManager *pManager = CUserManager::USR_GetInstance();
	if (NULL == pManager)
	{
		USR_ERROR("Get Manager Instance Failed\n");
		return 0;
	}

	const int nChannelCount = pManager->USR_GetChanNum();
	if ((bEnable < 0) || (bEnable > 1) 
		|| (nChannel < 0) || (nChannel >= nChannelCount)
		|| (bRemote < 0) || (bRemote > 1))
	{
		USR_ERROR("[%s]ERR: param error, bEnable=%d, action = %d, nChannel = %d, bRemote = %d",
			__FUNCTION__, bEnable, action, nChannel, bRemote);
		return IDVR_ERROR_PARAMETER;
	}

	USR_DBG("bEnable=%d, action=%d, nChannel=%d, bRemote=%d\n", bEnable, action, nChannel, bRemote);
	switch (action)
	{
	case CHANNEL_PREVIEW:
		USR_DBG("m_dwPreview[%d] = %lu, (~(1 << nChannel) = %u, (bEnable << nChannel)=%u\n"
			, bRemote, m_dwPreview[bRemote], ~(1 << nChannel), (bEnable << nChannel));
		m_dwPreview[bRemote] = ((m_dwPreview[bRemote] & (~(1 << nChannel))) | (bEnable << nChannel));
		USR_DBG("m_dwPreview[%d] = %lu\n", bRemote, m_dwPreview[bRemote]);
		break;
	case CHANNEL_PLAYBACK:
		USR_DBG("m_dwPlayback[%d] = %lu\n", bRemote, m_dwPlayback[bRemote]);
		m_dwPlayback[bRemote] = ((m_dwPlayback[bRemote] & (~(1 << nChannel))) | (bEnable << nChannel));
		USR_DBG("m_dwPlayback[%d] = %lu\n", bRemote, m_dwPlayback[bRemote]);
		break;
	case CHANNEL_PTZCTRL:
		USR_DBG("m_dwPtzCtrl[%d] = %lu\n", bRemote, m_dwPtzCtrl[bRemote]);
		m_dwPtzCtrl[bRemote] = ((m_dwPtzCtrl[bRemote] & (~(1 << nChannel))) | (bEnable << nChannel));
		USR_DBG("m_dwPtzCtrl[%d] = %lu\n", bRemote, m_dwPtzCtrl[bRemote]);
		break;
	case CHANNEL_MANUALRECORD:
		USR_DBG("m_dwManualRecord[%d] = %lu\n", bRemote, m_dwManualRecord[bRemote]);
		m_dwManualRecord[bRemote] = ((m_dwManualRecord[bRemote] & (~(1 << nChannel))) | (bEnable << nChannel));
		USR_DBG("m_dwManualRecord[%d] = %lu\n", bRemote, m_dwManualRecord[bRemote]);
		break;
	default:
		USR_ERROR("[%s]ERR: param error, bEnable=%d, action = %d, nChannel = %d, bRemote = %d",
			__FUNCTION__, bEnable, action, nChannel, bRemote);
		return IDVR_ERROR_PARAMETER;
		break;
	}
	// TODO: Add your specialized code here.
	return 0;
}

//!\brief 获取本地相关操作权限
//!\param action 操作代码
//!		LOCAL_SETTINGS 系统设置
//!		LOCAL_HALT 关闭系统
//!		LOCAL_EMAIL发送邮件通知
//!		LOCAL_SMS发送短信通知
//!\return 0表示无权限 1表示有权限
//##ModelId=49ACF31B032C
int CUser::GetLocalPrivilege(LOCAL_ACTION action) const
{
	// TODO: Add your specialized code here.
	int nResult = 0;
	switch (action)
	{
	case LOCAL_SETTINGS:
	case LOCAL_HALT:
	case LOCAL_EMAIL:
	case LOCAL_SMS:
	case LOCAL_FILE:
	case LOCAL_BACKUP:
		nResult = (m_dwLocal >> action) & 1;
		break;
	default:
		USR_ERROR("[%s]ERR: param error, action = %d",
			__FUNCTION__, action);
		return 0;
		break;
	}
	return (int)nResult;
}


//!\brief 设置本地相关操作权限
//!\brief bEnable 1表示启用权限 0表示禁用该权限
//!\param action 操作代码
//!		LOCAL_SETTINGS 系统设置
//!		LOCAL_HALT 关闭系统
//!		LOCAL_EMAIL发送邮件通知
//!		LOCAL_SMS发送短信通知
//!\return 0表示无权限 1表示有权限
//##ModelId=49ACF31B032C//##ModelId=49ACF31B03B9
int CUser::SetLocalPrivilege(int bEnable, LOCAL_ACTION action)
{
	// TODO: Add your specialized code here.
	if ((bEnable < 0) || (bEnable > 1))
	{
		USR_ERROR("[%s]ERR: param error, bEnable=%d, action = %d",
			__FUNCTION__, bEnable, action);
		return IDVR_ERROR_PARAMETER;
	}
	switch (action)
	{
	case LOCAL_SETTINGS:
	case LOCAL_HALT:
	case LOCAL_EMAIL:
	case LOCAL_SMS:
	case LOCAL_FILE:
	case LOCAL_BACKUP:
		m_dwLocal = ((m_dwLocal & (~(1 << action))) | (bEnable << action));
		break;
	default:
		USR_ERROR("[%s]ERR: param error, bEnable=%d, action = %d",
			__FUNCTION__, bEnable, action);
		return IDVR_ERROR_PARAMETER;
		break;
	}
	return (int)0;
}

//!\brief 获取远程相关操作权限
//!\param action 操作代码
//!		REMOTE_SETTINGS 系统设置
//!		REMOTE_HALT 关闭系统
//!		REMOTE_VOICE 语音对讲
//!		REMOTE_COM 透明通道
//!		REMOTE_ALARMOUT 报警输出
//!		REMOTE_UPDATE 远程升级
//!\return 0表示无权限 1表示有权限
//##ModelId=49ACF31C00AB
int CUser::GetRemotePrivilege(REMOTE_ACTION action) const
{
	// TODO: Add your specialized code here.
	int nResult = 0;
	switch (action)
	{
	case REMOTE_SETTINGS:
	case REMOTE_HALT:
	case REMOTE_VOICE:
	case REMOTE_COM:
	case REMOTE_ALARMOUT:
	case REMOTE_UPDATE:
		nResult = (m_dwRemote >> action) & 1;
		break;
	default:
		USR_ERROR("[%s]ERR: param error, action = %d",
			__FUNCTION__, action);
		return 0;
		break;
	}
	return (int)nResult;
}

//!\brief 设置远程相关操作权限
//!\param bEnable 1表示启用权限 0表示取消权限
//!\param action 操作代码
//!		REMOTE_SETTINGS 系统设置
//!		REMOTE_HALT 关闭系统
//!		REMOTE_VOICE 语音对讲
//!		REMOTE_COM 透明通道
//!		REMOTE_ALARMOUT 报警输出
//!		REMOTE_UPDATE 远程升级
//!\return 0表示设置成功 其他表示错误码
//##ModelId=49ACF31C0138
int CUser::SetRemotePrivilege(int bEnable, REMOTE_ACTION action)
{
	// TODO: Add your specialized code here.
	if ((bEnable < 0) || (bEnable > 1))
	{
		USR_ERROR("[%s]ERR: param error, bEnable=%d, action = %d",
			__FUNCTION__, bEnable, action);
		return IDVR_ERROR_PARAMETER;
	}
	switch (action)
	{
	case REMOTE_SETTINGS:
	case REMOTE_HALT:
	case REMOTE_VOICE:
	case REMOTE_COM:
	case REMOTE_ALARMOUT:
	case REMOTE_UPDATE:
		m_dwRemote = ((m_dwRemote & (~(1 << action))) | (bEnable << action));
		break;
	default:
		USR_ERROR("[%s]ERR: param error, bEnable=%d, action = %d",
			__FUNCTION__, bEnable, action);
		return IDVR_ERROR_PARAMETER;
		break;
	}
	return (int)0;
}

//!\brief 获取远程相关操作读权限V1
//!\param action 操作代码
//!		REMOTE_VOICE_INTERCOM 系统设置
//!		REMOTE_SERIAL_COM 关闭系统
//!		REMOTE_FACE 人脸配置
//!		REMOTE_TEMP 体温配置
//!		REMOTE_MASK 口罩配置
//!		REMOTE_PRIVACY 隐私配置
//!		REMOTE_OEPNDOOR 开门配置
//!		REMOTE_IMAGE 影像配置
//!		REMOTE_BASIC_NETWORK 基础网络设置
//!		REMOTE_ADVANCE_NETWORK 高级网络设置
//!		REMOTE_CODE 编码设置
//!		REMOTE_ALARM 报警设置
//!		REMOTE_BASIC_SYSTEM 基本系统设置
//!		REMOTE_ADVANCE_SYSTEM 高级系统设置
//!		REMOTE_UPDATE 系统升级
//!\return 0表示无权限 1表示有权限
int CUser::GetRemoteReadPrivilege(REMOTE_ACTION_V1 action) const
{
	// TODO: Add your specialized code here.
	int nResult = 0;
	switch (action)
	{
        case REMOTE_VOICE_INTERCOM:
        case REMOTE_SERIAL_COM:
        case REMOTE_FACE:
        case REMOTE_TEMP:
        case REMOTE_MASK:
        case REMOTE_PRIVACY:
        case REMOTE_OEPNDOOR:
        case REMOTE_IMAGE:
        case REMOTE_BASIC_NETWORK:
        case REMOTE_ADVANCE_NETWORK:
        case REMOTE_CODE:
        case REMOTE_ALARM:
        case REMOTE_BASIC_SYSTEM:
        case REMOTE_ADVANCE_SYSTEM:
        case REMOTE_SYSTEM_UPDATE:
    		nResult = (m_dwRemoteR >> action) & 1;
    		break;
    	default:
    		USR_ERROR("[%s]ERR: param error, action = %d",
    			__FUNCTION__, action);
    		return 0;
    		break;
	}
	return (int)nResult;
}

//!\brief 获取远程相关操作写权限V1
//!\param action 操作代码
//!		REMOTE_VOICE_INTERCOM 系统设置
//!		REMOTE_SERIAL_COM 关闭系统
//!		REMOTE_FACE 人脸配置
//!		REMOTE_TEMP 体温配置
//!		REMOTE_MASK 口罩配置
//!		REMOTE_PRIVACY 隐私配置
//!		REMOTE_OEPNDOOR 开门配置
//!		REMOTE_IMAGE 影像配置
//!		REMOTE_BASIC_NETWORK 基础网络设置
//!		REMOTE_ADVANCE_NETWORK 高级网络设置
//!		REMOTE_CODE 编码设置
//!		REMOTE_ALARM 报警设置
//!		REMOTE_BASIC_SYSTEM 基本系统设置
//!		REMOTE_ADVANCE_SYSTEM 高级系统设置
//!		REMOTE_UPDATE 系统升级
//!\return 0表示无权限 1表示有权限
int CUser::GetRemoteWritePrivilege(REMOTE_ACTION_V1 action) const
{
	// TODO: Add your specialized code here.
	int nResult = 0;
	switch (action)
	{
        case REMOTE_VOICE_INTERCOM:
        case REMOTE_SERIAL_COM:
        case REMOTE_FACE:
        case REMOTE_TEMP:
        case REMOTE_MASK:
        case REMOTE_PRIVACY:
        case REMOTE_OEPNDOOR:
        case REMOTE_IMAGE:
        case REMOTE_BASIC_NETWORK:
        case REMOTE_ADVANCE_NETWORK:
        case REMOTE_CODE:
        case REMOTE_ALARM:
        case REMOTE_BASIC_SYSTEM:
        case REMOTE_ADVANCE_SYSTEM:
        case REMOTE_SYSTEM_UPDATE:
    		nResult = (m_dwRemoteW >> action) & 1;
    		break;
    	default:
    		USR_ERROR("[%s]ERR: param error, action = %d",
    			__FUNCTION__, action);
    		return 0;
    		break;
	}
	return (int)nResult;
}

//!\brief 设置远程相关操作读权限V1
//!\param action 操作代码
//!\param bEnable 1表示启用权限 0表示取消权限
//!		REMOTE_VOICE_INTERCOM 系统设置
//!		REMOTE_SERIAL_COM 关闭系统
//!		REMOTE_FACE 人脸配置
//!		REMOTE_TEMP 体温配置
//!		REMOTE_MASK 口罩配置
//!		REMOTE_PRIVACY 隐私配置
//!		REMOTE_OEPNDOOR 开门配置
//!		REMOTE_IMAGE 影像配置
//!		REMOTE_BASIC_NETWORK 基础网络设置
//!		REMOTE_ADVANCE_NETWORK 高级网络设置
//!		REMOTE_CODE 编码设置
//!		REMOTE_ALARM 报警设置
//!		REMOTE_BASIC_SYSTEM 基本系统设置
//!		REMOTE_ADVANCE_SYSTEM 高级系统设置
//!		REMOTE_UPDATE 系统升级
//!\return 0表示设置成功 其他表示错误码
int CUser::SetRemoteReadPrivilege(int bEnable, REMOTE_ACTION_V1 action)
{
	// TODO: Add your specialized code here.
	if ((bEnable < 0) || (bEnable > 1))
	{
		USR_ERROR("[%s]ERR: param error, bEnable=%d, action = %d",
			__FUNCTION__, bEnable, action);
		return IDVR_ERROR_PARAMETER;
	}
	switch (action)
	{
        case REMOTE_VOICE_INTERCOM:
        case REMOTE_SERIAL_COM:
        case REMOTE_FACE:
        case REMOTE_TEMP:
        case REMOTE_MASK:
        case REMOTE_PRIVACY:
        case REMOTE_OEPNDOOR:
        case REMOTE_IMAGE:
        case REMOTE_BASIC_NETWORK:
        case REMOTE_ADVANCE_NETWORK:
        case REMOTE_CODE:
        case REMOTE_ALARM:
        case REMOTE_BASIC_SYSTEM:
        case REMOTE_ADVANCE_SYSTEM:
        case REMOTE_SYSTEM_UPDATE:
    		m_dwRemoteR = ((m_dwRemoteR & (~(1 << action))) | (bEnable << action));
    		break;
    	default:
    		USR_ERROR("[%s]ERR: param error, bEnable=%d, action = %d",
    			__FUNCTION__, bEnable, action);
    		return IDVR_ERROR_PARAMETER;
    		break;
	}
	return (int)0;
}

//!\brief 设置远程相关操作写权限V1
//!\param action 操作代码
//!\param bEnable 1表示启用权限 0表示取消权限
//!		REMOTE_VOICE_INTERCOM 系统设置
//!		REMOTE_SERIAL_COM 关闭系统
//!		REMOTE_FACE 人脸配置
//!		REMOTE_TEMP 体温配置
//!		REMOTE_MASK 口罩配置
//!		REMOTE_PRIVACY 隐私配置
//!		REMOTE_OEPNDOOR 开门配置
//!		REMOTE_IMAGE 影像配置
//!		REMOTE_BASIC_NETWORK 基础网络设置
//!		REMOTE_ADVANCE_NETWORK 高级网络设置
//!		REMOTE_CODE 编码设置
//!		REMOTE_ALARM 报警设置
//!		REMOTE_BASIC_SYSTEM 基本系统设置
//!		REMOTE_ADVANCE_SYSTEM 高级系统设置
//!		REMOTE_UPDATE 系统升级
//!\return 0表示设置成功 其他表示错误码
int CUser::SetRemoteWritePrivilege(int bEnable, REMOTE_ACTION_V1 action)
{
	// TODO: Add your specialized code here.
	if ((bEnable < 0) || (bEnable > 1))
	{
		USR_ERROR("[%s]ERR: param error, bEnable=%d, action = %d",
			__FUNCTION__, bEnable, action);
		return IDVR_ERROR_PARAMETER;
	}
	switch (action)
	{
        case REMOTE_VOICE_INTERCOM:
        case REMOTE_SERIAL_COM:
        case REMOTE_FACE:
        case REMOTE_TEMP:
        case REMOTE_MASK:
        case REMOTE_PRIVACY:
        case REMOTE_OEPNDOOR:
        case REMOTE_IMAGE:
        case REMOTE_BASIC_NETWORK:
        case REMOTE_ADVANCE_NETWORK:
        case REMOTE_CODE:
        case REMOTE_ALARM:
        case REMOTE_BASIC_SYSTEM:
        case REMOTE_ADVANCE_SYSTEM:
        case REMOTE_SYSTEM_UPDATE:
    		m_dwRemoteW = ((m_dwRemoteW & (~(1 << action))) | (bEnable << action));
    		break;
    	default:
    		USR_ERROR("[%s]ERR: param error, bEnable=%d, action = %d",
    			__FUNCTION__, bEnable, action);
    		return IDVR_ERROR_PARAMETER;
    		break;
	}
	return (int)0;
}

void CUser::Print()
{
	USR_DBG("ID = %d, Name = %s, Pwd = %s, Email = %s, Enable = %d, Pv = {%lu, %lu}, Pb = {%lu, %lu}, Pc = {%lu, %lu}, Mr = {%lu, %lu}, Local = %lu, Remote = %lu\n", 
		m_nUserId, m_szUsername, m_szPassword, m_szEmail, m_bEnable, m_dwPreview[0], m_dwPreview[1], 
		m_dwPlayback[0], m_dwPlayback[1], m_dwPtzCtrl[0], m_dwPtzCtrl[1], 
		m_dwManualRecord[0], m_dwManualRecord[1], m_dwLocal, m_dwRemote);
}

//!\brief 重载赋值操作符
CUser &CUser::operator=(const CUser &objUser)
{
	if (this != &objUser)
	{
		m_nUserId = objUser.m_nUserId;
		memset(m_szUsername, 0, USERNAME_BUF_LEN);
		strncpy(m_szUsername, objUser.m_szUsername, sizeof(m_szUsername) - 1);
		
		memset(m_szPassword, 0, PASSWORD_BUF_LEN);
		strncpy(m_szPassword, objUser.m_szPassword, sizeof(m_szPassword) - 1);

		memset(m_szEmail, 0, EMAIL_BUF_LEN);
		strncpy(m_szEmail, objUser.m_szEmail, sizeof(m_szEmail) - 1);

		memcpy(m_szMacAddress, objUser.m_szMacAddress, MAC_ADDRESS_BYTES);

		m_bEnable = objUser.m_bEnable;
		m_dwPreview[0] = objUser.m_dwPreview[0];
		m_dwPreview[1] = objUser.m_dwPreview[1];
		m_dwPlayback[0] = objUser.m_dwPlayback[0];
		m_dwPlayback[1] = objUser.m_dwPlayback[1];
		m_dwPtzCtrl[0] = objUser.m_dwPtzCtrl[0];
		m_dwPtzCtrl[1] = objUser.m_dwPtzCtrl[1];
		m_dwManualRecord[0] = objUser.m_dwManualRecord[0];
		m_dwManualRecord[1] = objUser.m_dwManualRecord[1];
		m_dwLocal = objUser.m_dwLocal;
		m_dwRemote = objUser.m_dwRemote;
	}
	return *this; 
}


