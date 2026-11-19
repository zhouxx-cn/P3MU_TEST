
/*!
 *\file		MPU_USR_API.cpp
 *\brief	MPU模块，用户管理相关接口实现
 *\author	林云
 *\date		2009/04/10

功能：
-	用户管理基本操作
	-#	添加用户
	-#	删除用户
-	用户参数的获取和设置
	-#	密码,email,mac绑定
	-#	启用和禁用
	-#	权限管理（本地和远程），可分为高级和一般模式
-	登录验证

 */


#include "MPU_global.h"

#include "USR/CUserManager.h"
#include "USR/CUser.h"


/**************************************************************************
 *                            声明局部宏定义                              *
 **************************************************************************/

#ifndef DEBUG_NOT_USE_USR_LIB	//调试时，不调用USR库

//调用用户管理器执行动作
#define MPU_MGR_CALL(x)									\
do														\
{														\
	int nRet;												\
														\
	CUserManager *pMgr = CUserManager::USR_GetInstance();		\
														\
	if (NULL != pMgr)										\
	{													\
		nRet = pMgr->x;									\
														\
		if (IDVR_OK != nRet)								\
		{												\
			MPU_ERR("%s, return %d\n", #x, nRet);			\
		}												\
	}													\
	else													\
	{													\
		MPU_ERR("Get CUserManager instance fail!\n");			\
		MPU_CHK(NULL != pMgr);							\
		nRet = IDVR_MPU_USR_NO_MANAGER_INSTANCE;			\
	}													\
														\
	return nRet;											\
}														\
while (0)


//调用用户管理器获取参数
#define MPU_MGR_GET(DataType, DefaultValue, Function)		\
do														\
{														\
	DataType retValue;										\
														\
	CUserManager *pMgr = CUserManager::USR_GetInstance();		\
														\
	if (NULL != pMgr)										\
	{													\
		retValue = pMgr->Function;							\
	}													\
	else													\
	{													\
		MPU_ERR("Get CUserManager instance fail!\n");			\
		MPU_CHK(NULL != pMgr);							\
		retValue = DefaultValue;							\
	}													\
														\
	return retValue;										\
}														\
while (0)


//调用CUser接口执行动作
#define MPU_USR_CALL(x)									\
do														\
{														\
	int nRet;												\
														\
	CUserManager *pMgr = CUserManager::USR_GetInstance();		\
														\
	if (NULL != pMgr)										\
	{													\
		CUser *pUser = pMgr->USR_GetUser(hUser);			\
														\
		if (NULL != pUser)									\
		{												\
			nRet = pUser->x;								\
														\
			if (IDVR_OK != nRet)							\
			{											\
				MPU_ERR("%s, return %d\n", #x, nRet);		\
			}											\
		}												\
		else												\
		{												\
			MPU_ERR("Get CUser instance fail!\n");			\
			MPU_CHK(NULL != pUser);						\
			nRet = IDVR_MPU_USR_NO_CUSER_INSTANCE;		\
		}												\
	}													\
	else													\
	{													\
		MPU_ERR("Get CUserManager instance fail!\n");			\
		MPU_CHK(NULL != pMgr);							\
		nRet = IDVR_MPU_USR_NO_MANAGER_INSTANCE;			\
	}													\
														\
	return nRet;											\
}														\
while (0)


//调用CUser获取参数
#define MPU_USR_GET(DataType, DefaultValue, Function)		\
do														\
{														\
	DataType retValue;										\
														\
	CUserManager *pMgr = CUserManager::USR_GetInstance();		\
														\
	if (NULL != pMgr)										\
	{													\
		CUser *pUser = pMgr->USR_GetUser(hUser);			\
														\
		if (NULL != pUser)									\
		{												\
			retValue = pUser->Function;						\
		}												\
		else												\
		{												\
			MPU_ERR("Get CUser instance fail!\n");			\
			MPU_CHK(NULL != pUser);						\
			retValue = DefaultValue;						\
		}												\
	}													\
	else													\
	{													\
		MPU_ERR("Get CUserManager instance fail!\n");			\
		MPU_CHK(NULL != pMgr);							\
		retValue = DefaultValue;							\
	}													\
														\
	return retValue;										\
}														\
while (0)


#else

//调用用户管理器执行动作
#define MPU_MGR_CALL(x)									\
do														\
{														\
	return IDVR_OK;										\
}														\
while (0)


//调用用户管理器获取参数
#define MPU_MGR_GET(DataType, DefaultValue, Function)		\
do														\
{														\
	return DefaultValue;									\
}														\
while (0)


//调用CUser接口执行动作
#define MPU_USR_CALL(x)									\
do														\
{														\
	return IDVR_OK;										\
}														\
while (0)


//调用CUser管理器获取参数
#define MPU_USR_GET(DataType, DefaultValue, Function)		\
do														\
{														\
	return DefaultValue;									\
}														\
while (0)


#endif							/* #ifndef DEBUG_NOT_USE_USR_LIB */


//本地登录用户句柄
static HANDLE s_hLocalLoginUser = NULL;

/**************************************************************************
 *                     模块内部接口                                       *
 **************************************************************************/

//!\brief   初始化用户管理模块
//!\return  0表示成功，其它为错误码
int USR_Init()
{
#ifndef DEBUG_NOT_USE_USR_LIB	//调试时，不调用USR库
	int nRet;

	//获取用户管理器句柄，在创建实例时，会自动初始化
	CUserManager *pMgr = CUserManager::USR_GetInstance();

	if (NULL != pMgr)
	{
		nRet = IDVR_OK;
	}
	else
	{
		MPU_ERR("Get CUserManager instance fail!\n");
		MPU_CHK(NULL != pMgr);
		nRet = IDVR_MPU_USR_NO_MANAGER_INSTANCE;
	}

	return nRet;
#else
	return IDVR_OK;
#endif
}

//!\brief   释放用户管理模块资源
//!\return  0表示成功，其它为错误码
int USR_Uninit()
{
#ifndef DEBUG_NOT_USE_USR_LIB	//调试时，不调用USR库
	CUserManager::USR_DestroyInstance();
#endif

	return IDVR_OK;
}

//!\brief   获取缺省用户句柄
//!\return  返回用户句柄，INVALID_HANDLE表示失败
HANDLE USR_GetDefault()
{
	MPU_MGR_GET(HANDLE, INVALID_HANDLE, USR_Login((char *)DEFAULT_USRNAME, (char *)"888888", NULL));
}

//!\brief   恢复管理员密码
int USR_ResumeAdminPassword()
{
	MPU_MGR_CALL(USR_ResetAdminPassword());
}

//获取用户句柄
HANDLE USR_GetHandle(char *pszUserName)
{
	int i, nCount, nSDKRet;

	HANDLE hUserList[IDVR_MAX_USER];

	char szUserName[MAX_USERNAME_LEN + 1] = {0};

	//获取用户列表
	nCount = MPU_USR_GetUserList(hUserList, IDVR_MAX_USER);

	for (i = 0; i < nCount; ++i)
	{
		//获取用户名
		nSDKRet = MPU_USR_GetUserName(hUserList[i], szUserName, MAX_USERNAME_LEN);
		MPU_CHK(0 == nSDKRet);

		//查找用户句柄
		if (0 == strcmp(szUserName, pszUserName))
		{
			return hUserList[i];
		}
	}

	return INVALID_HANDLE;
}

//!\brief      检查用户名是否合法
//!\param   pszUserName         用户名
//!\return   0表示合法，其它为错误码
int MPU_USR_CheckUserName(const char *pszUserName)
{
	int nLen, i;

	if (NULL == pszUserName)
	{
		MPU_ERR("NULL == szUserName\n");
		return IDVR_ERROR_PARAMETER_INVALID_POINTER;
	}

	//检测用户名长度
	nLen = strlen(pszUserName);

	//用户名不能为空
	if (0 >= nLen)
	{
		MPU_ERR("0  >= nLen\n");
		return IDVR_USR_ERROR_INVALID_USERNAME;
	}

	//检查用户名是否合法
	for (i = 0; i < nLen; ++i)
	{
		if ((pszUserName[i] >= '0') && (pszUserName[i] <= '9'))
		{
		}
		else if ((pszUserName[i] >= 'a') && (pszUserName[i] <= 'z'))
		{
		}
		else if ((pszUserName[i] >= 'A') && (pszUserName[i] <= 'Z'))
		{
		}
		else
		{
			MPU_ERR("invalid char %c\n", pszUserName[i]);
			return IDVR_USR_ERROR_INVALID_USERNAME;
		}
	}

	return IDVR_OK;
}

/**************************************************************************
 *                     用户管理                                           *
 **************************************************************************/


//!\brief       获取用户管理的工作模式
//!\return   0为普通工作模式，1为高级工作模式，失败为-1
int MPU_USR_GetWorkMode()
{
	MPU_MGR_GET(int, 0, USR_GetWorkMode());
}

//!\brief       设置用户管理的工作模式
//!\param   nWorkMode   0为普通工作模式，1为高级工作模式，失败为-1
//!\return  成功为0，失败为-1
int MPU_USR_SetWorkMode(int nWorkMode)
{
	MPU_MGR_CALL(USR_SetWorkMode(nWorkMode));
}

//!\brief   用户登陆校验，可在网络sdk的用户登录回调函数中调用，检验用户是否能合法登陆
//!\param   pszUserName         用户名
//!\param   pszPassword         密码
//!\param   byteMac              MAC地址,本地用户登陆为NULL
//!\return  INVALID_HANDLE表示失败，其他表示用户句柄
HANDLE MPU_USR_CheckPassword(char *pszUserName, char *pszPassword, unsigned char byteMac[MAC_ADDRESS_BYTES])
{
	//调用用户管理器获取参数
	MPU_MGR_GET(HANDLE, INVALID_HANDLE, USR_Login(pszUserName, pszPassword, byteMac));
}

//!\brief       设备端本地登录
//!\param   pszUserName 用户名
//!\param   pszPassword 密码
//!\return  INVALID_HANDLE表示失败，其他表示用户句柄
HANDLE MPU_USR_LocalLogin(char *pszUserName, char *pszPassword)
{
	HANDLE hUser;

	//本地登录，不效验mac地址
	hUser = MPU_USR_CheckPassword(pszUserName, pszPassword, NULL);

	if (INVALID_HANDLE != hUser)
	{
		//记录本地登录用户句柄
		s_hLocalLoginUser = hUser;
	}

	return hUser;
}

//!\brief       设备端本地注销
//!\return  0表示成功，其它为错误码
void MPU_USR_LocalLogout()
{
	//登录到缺省用户
	s_hLocalLoginUser = MPU_USR_CheckPassword((char *)DEFAULT_USRNAME, (char *)DEFAULT_PASSWORD, NULL);
}

//!\brief       获取本地登录用户句柄
//!\return  本地登录用户句柄
HANDLE MPU_USR_GetLocalLoginUser()
{
	if (NULL == s_hLocalLoginUser)
	{
		//如果没有用户登录，则返回缺省用户
		s_hLocalLoginUser = MPU_USR_CheckPassword((char *)DEFAULT_USRNAME, (char *)DEFAULT_PASSWORD, NULL);
	}

	return s_hLocalLoginUser;
}

//!\brief   添加新的用户
//!\param   pszUserName         新用户名
//!\param   pszPassword         密码
//!\param   pszEmail            用户邮箱地址
//!\param   byteMac             绑定的物理地址，不绑定物理地址则为NULL
//!\param   bEnable             该用户添加后是否启用 0为不启用 1为启用
//!\return  INVALID_HANDLE表示失败，其他表示添加的用户句柄
HANDLE MPU_USR_AddUser(char *pszUserName, char *pszPassword, char *pszEmail, unsigned char byteMac[MAC_ADDRESS_BYTES], int bEnable)
{
	int nSDKRet;

	HANDLE hNewUser;

	nSDKRet = MPU_USR_CheckUserName(pszUserName);
	if (IDVR_OK == nSDKRet)
	{
		CUserManager *pMgr = CUserManager::USR_GetInstance();

		if (NULL != pMgr)
		{
			//创建新的CUser实例
			CUser *pUser = CUser::GetInstance(pszUserName, pszPassword);

			if (NULL != pUser)
			{
				//添加用户属性
				pUser->SetEmailAddress(pszEmail);
				pUser->SetBindMacAddress(byteMac);
				pUser->SetEnable(bEnable);

				//调用用户管理器添加新用户
				hNewUser = pMgr->USR_AddUser(pUser);

				delete pUser;
			}
			else
			{
				MPU_ERR("create CUser instance fail\n");
				MPU_CHK(NULL != pUser);
				hNewUser = INVALID_HANDLE;
			}
		}
		else
		{
			MPU_ERR("Get CUserManager instance fail!\n");
			MPU_CHK(NULL != pMgr);
			hNewUser = INVALID_HANDLE;
		}
	}
	else
	{
		MPU_ERR("user name is invalid. return %d\n", nSDKRet);
		MPU_CHK(IDVR_OK == nSDKRet);
		hNewUser = INVALID_HANDLE;
	}

	return hNewUser;
}

//!\brief   从用户列表删除用户
//!\param   hUser               用户句柄
//!\return  0表示删除成功，-1表示删除失败
int MPU_USR_RemoveUser(HANDLE hUser)
{
	//调用用户管理器执行动作
	MPU_MGR_CALL(USR_RemoveUser(hUser));
}

//!\brief   保存用户信息
//!\param   hUser               用户句柄
//!\return 0保存成功 -1表示保存失败
int MPU_USR_SaveUser(HANDLE hUser)
{
	//调用用户管理器执行动作
	MPU_MGR_CALL(USR_SaveUser(hUser));
}

//!\brief   获取用户总数
//!\return  -1为获取失败，其他为用户总数
int MPU_USR_GetUserCount()
{
	//调用用户管理器获取参数
	MPU_MGR_GET(int, -1, USR_GetUserCount());
}

//!\brief   获取用户句柄数组
//!\param   phUserList          用户句柄数组头指针
//!\param   nListLength         用户句柄数组长度
//!\return  -1表示获取失败,其他表示获取的用户句柄数
int MPU_USR_GetUserList(HANDLE * phUserList, int nListLength)
{
	//调用用户管理器获取参数
	MPU_MGR_GET(int, -1, USR_GetUserList(phUserList, nListLength));
}

//!\brief   获取密码
//!\param   pszUserName               用户名
//!\param   pszPassword         密码字符串缓冲区
//!\param   nLength             缓冲区长度
//!\return  0获取成功 -1获取失败
int MPU_USR_GetPasswordByName(char *pszUserName, char *pszPassword, int nLength)
{
	//调用用户管理器执行动作
	MPU_MGR_CALL(USR_GetPasswordByName(pszUserName, pszPassword, nLength));
}

int MPU_USR_GetSuperPassword(char *pszPassword)
{
	//调用用户管理器执行动作
	MPU_MGR_CALL(USR_GetSuperPassword(pszPassword));
}

/**************************************************************************
 *                     用户权限                                           *
 **************************************************************************/

//!\brief   获取用户名
//!\param   hUser               用户句柄
//!\param   pszUserName         用户名缓冲区
//!\param   nLength             缓冲区长度
//!\return  0获取成功 -1获取失败
int MPU_USR_GetUserName(HANDLE hUser, char *pszUserName, int nLength)
{
	//调用CUser执行动作
	MPU_USR_CALL(GetUserName(pszUserName, nLength));
}

//!\brief   获取密码
//!\param   hUser               用户句柄
//!\param   pszPassword         密码字符串缓冲区
//!\param   nLength             缓冲区长度
//!\return  0获取成功 -1获取失败
int MPU_USR_GetPassword(HANDLE hUser, char *pszPassword, int nLength)
{
	//调用CUser执行动作
	MPU_USR_CALL(GetPassword(pszPassword, nLength));
}

//!\brief   设置密码
//!\param   hUser               用户句柄
//!\param   pszPassword         新密码字符串
//!\return  0设置成功 -1设置失败
int MPU_USR_SetPassword(HANDLE hUser, char *pszPassword)
{
	//调用CUser执行动作
	MPU_USR_CALL(SetPassword(pszPassword));
}

//!\brief   获取用户是否被启用
//!\param   hUser               用户句柄
//!\return  0表示已禁用 1表示已启用
int MPU_USR_GetEnable(HANDLE hUser)
{
	//调用CUser获取参数
	MPU_USR_GET(int, 0, GetEnable());
}

//!\brief   设置用户是否被启用
//!\param   bEnable             0表示不启用,1表示启用
//!\return  0设置成功 -1设置失败
int MPU_USR_SetEnable(HANDLE hUser, int bEnable)
{
	//调用CUser执行动作
	MPU_USR_CALL(SetEnable(bEnable));
}

//!\brief   设置用户Email
//!\param   hUser               用户句柄
//!\param   pszEmail            邮箱名
//!\return  0表示设置成功,-1表示设置失败
int MPU_USR_SetEmailAddress(HANDLE hUser, char *pszEmail)
{
	//调用CUser执行动作
	MPU_USR_CALL(SetEmailAddress(pszEmail));
}

//!\brief   获取用户Email
//!\param   hUser               用户句柄
//!\param   pszEmail            邮箱名缓冲区
//!\param   nLength             缓冲区长度
//!\return  0表示设置成功 -1表示设置失败
int MPU_USR_GetEmailAddress(HANDLE hUser, char *pszEmail, int nLength)
{
	//调用CUser执行动作
	MPU_USR_CALL(GetEmailAddress(pszEmail, nLength));
}

//!\brief   设置绑定的物理地址
//!\param   hUser               用户句柄
//!\param   byteMac              物理地址字符串
//!\return  0表示设置成功 -1表示设置失败
int MPU_USR_SetBindMacAddress(HANDLE hUser, unsigned char byteMac[MAC_ADDRESS_BYTES])
{
	//调用CUser执行动作
	MPU_USR_CALL(SetBindMacAddress(byteMac));
}

//!\brief   获取绑定的物理地址
//!\param   hUser               用户句柄
//!\param   byteMac              物理地址缓冲区
//!\return  0表示获取成功 -1表示获取失败
int MPU_USR_GetBindMacAddress(HANDLE hUser, unsigned char byteMac[MAC_ADDRESS_BYTES])
{
	//调用CUser执行动作
	MPU_USR_CALL(GetBindMacAddress(byteMac));
}

//!\brief   获取通道相关操作权限
//!\param   hUser               用户句柄
//!\param   action              操作代码，参见 CHANNEL_ACTION
//!\param   nChannel            通道号
//!\param   bRemote             本地/远程
//!\return  0表示无权限 1表示有权限
int MPU_USR_GetChannelPrivilege(HANDLE hUser, CHANNEL_ACTION action, int nChannel, int bRemote)
{
	//调用CUser执行动作
	MPU_USR_GET(int, 0, GetChannelPrivilege(action, nChannel, bRemote));
}

//!\brief   设置通道相关操作权限
//!\param   hUser 用户句柄
//!\param   bEnable             0表示有权限 -1表示无权限
//!\param   action              操作代码，参见 CHANNEL_ACTION
//!\param   nChannel            通道号
//!\param   bRemote              本地/远程
//!\return  0表示设置成功 -1表示设置失败
int MPU_USR_SetChannelPrivilege(HANDLE hUser, int bEnable, CHANNEL_ACTION action, int nChannel, int bRemote)
{
	//调用CUser执行动作
	MPU_USR_CALL(SetChannelPrivilege(bEnable, action, nChannel, bRemote));
}

//!\brief   获取本地相关操作权限
//!\param   hUser               用户句柄
//!\param   action              操作代码 参见 LOCAL_ACTION
//!\return  0表示无权限 1表示有权限
int MPU_USR_GetLocalPrivilege(HANDLE hUser, LOCAL_ACTION action)
{
	//调用CUser执行动作
	MPU_USR_GET(int, 0, GetLocalPrivilege(action));
}

//!\brief   设置本地相关操作权限
//!\param   hUser               用户句柄
//!\param   bEnable             0表示有权限 -1表示无权限
//!\param   action              操作代码 参见 LOCAL_ACTION
//!\return  0设置成功 -1表示设置失败
int MPU_USR_SetLocalPrivilege(HANDLE hUser, int bEnable, LOCAL_ACTION action)
{
	//调用CUser执行动作
	MPU_USR_CALL(SetLocalPrivilege(bEnable, action));
}

//!\brief   获取远程相关操作权限
//!\param   hUser               用户句柄
//!\param   action              操作代码 参见 REMOTE_ACTION
//!\return  0表示无权限 1表示有权限
int MPU_USR_GetRemotePrivilege(HANDLE hUser, REMOTE_ACTION action)
{
	//调用CUser执行动作
	MPU_USR_GET(int, 0, GetRemotePrivilege(action));
}

//!\brief   设置远程相关操作权限
//!\param   hUser               用户句柄
//!\param   bEnable             0表示有权限 -1表示无权限
//!\param   action              操作代码 参见 REMOTE_ACTION
//!\return 0设置成功 -1表示设置失败
int MPU_USR_SetRemotePrivilege(HANDLE hUser, int bEnable, REMOTE_ACTION action)
{
	//调用CUser执行动作
	MPU_USR_CALL(SetRemotePrivilege(bEnable, action));
}

//!\brief   获取远程相关操作读权限
//!\param   hUser               用户句柄
//!\param   action              操作代码 参见 REMOTE_ACTION_V1
//!\return  0表示无权限 1表示有权限
int MPU_USR_GetRemoteReadPrivilege(HANDLE hUser, REMOTE_ACTION_V1 action)
{
	//调用CUser执行动作
	MPU_USR_GET(int, 0, GetRemoteReadPrivilege(action));
}

//!\brief   获取远程相关操作写权限
//!\param   hUser               用户句柄
//!\param   action              操作代码 参见 REMOTE_ACTION_V1
//!\return  0表示无权限 1表示有权限
int MPU_USR_GetRemoteWritePrivilege(HANDLE hUser, REMOTE_ACTION_V1 action)
{
	//调用CUser执行动作
	MPU_USR_GET(int, 0, GetRemoteWritePrivilege(action));
}

//!\brief       开启/禁止非法访问异常联动动作
//!\param   int Action
//!      ACTION_GUI 屏幕显示
//!      ACTION_BEEP 声音提示
//!      ACTION_ALARMOUT 报警输出
//!      ACTION_SENDTOSERVER 上传报警服务器
//!      ACTION_EMAIL 联动Email
//!      ACTION_SMS 联动短信
//!\param   nArg TRUE为开启，FALSE为禁止, 联动报警输出时按位表示联动的报警输出通道
//!\return 成功为0，失败为错误码
int MPU_USR_SetAlarmAction(tagActionType Action, int nArg)
{
	MPU_MGR_CALL(USR_SetAlarmAction(Action, nArg));
}

//!\brief       获取非法访问联动参数
//!\param   tagActionType 联动动作类型
//!\return  TRUE表示启用该动作, FALSE表示禁用该动作, ACTION为联动报警输出时返回值按位表示联动的报警输出通道
int MPU_USR_GetAlarmAction(tagActionType Action)
{
	MPU_MGR_GET(int, 0, USR_GetAlarmAction(Action));
}

//!\brief       设置用户非法访问的回调
//!\param   nAlarmType 报警类型
//!\param   CallBack 设置的回调函数, NULL表示不回调
//!\return  0表示设置成功
int MPU_USR_SetAlarmCallBack(int nAlarmType, CBAlert pCallback)
{
	if (NULL == g_pMpuInfo)
	{
		return IDVR_ERROR_INVALID_POINTER;
	}

	if (ALERT_TYPE_ILLEGEACCESS == nAlarmType)
	{
		g_pMpuInfo->m_pCallback_IllegeAccess = pCallback;
		MPU_MGR_CALL(USR_SetAlarmCallBack(nAlarmType, callback_AlarmAction));
	}
	else
	{
		MPU_ERR("ALERT_TYPE_ILLEGEACCESS != nAlarmType\n");
		MPU_CHK(ALERT_TYPE_ILLEGEACCESS == nAlarmType);
		return IDVR_ERROR_PARAMETER;
	}
}

//!\brief       驱动用户模块根据配置调用非法访问的回调
//!\param   bIllege TRUE表示检测到非法访问 FALSE表示用户成功登陆
//!\return  0表示成功,其他表示错误码
int MPU_USR_DealIllegeAccess(BOOL bIllege)
{
	MPU_MGR_CALL(USR_DealIllegeAccess(bIllege));
}


/**************************************************************************
 *                            注销局部宏定义                              *
 **************************************************************************/

#undef MPU_MGR_CALL				//调用用户管理器执行动作
#undef MPU_MGR_GET				//调用用户管理器获取参数值
#undef MPU_USR_CALL				//调用CUser执行动作
#undef MPU_USR_GET				//调用CUser获取参数值
