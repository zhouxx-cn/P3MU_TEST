
/*!
*\file		MPU_CIU_login.cpp
*\brief	MPU模块，远程登陆的实现
*\author	ｌｂｊ
*\date		2009/06/15

 功能：
 -#	
 -#	远程登陆
 
  说明：
  -#	
*/


#include "MPU_CIU_global.h"
#include "MPU_CIU_LogQuery.h"
#include "tagLog.h"
#include "MPU_CIU_Login.h"

/* 消除本文件中的snprintf截断警告 */
#pragma GCC diagnostic ignored "-Wformat-truncation"

typedef struct tagLoginUser
{
	DWORD dwTime;
	HANDLE hUser;
	eProtocolID eProtocol;
	char acUserName[USERNAME_LEN + 2];
	char acTime[30];
	char acUserIPAddress[30];
	T_REMOTEQUEERY tLogQuery;
}
T_LOGINUSER, *PT_LOGINUSER;		// 已登陆的用户


static T_LOGINUSER g_atLoginUser[LOGINUSER_COUNT];

//!\brief  通过用户名获取用户密码
//!\param   pUserName    用户名
//!\param   pUserPass    用户密码
//!\return  0表示设置成功,-1表示设置失败
int UserGetPassword(eProtocolID eProtocol, const char *pUserName, char *pUserPass)
{
	return MPU_USR_GetPasswordByName((char *) pUserName, pUserPass, USERPASS_LEN);
}

//!\brief  检查登陆ip的disable时间
//!\param   pcIpAddr    ip地址如:192.168.1.1
//!\return  0不允许，1许可
int UserCheckIP(char *pcIpAddrSrc)
{
	MPU_CHK(pcIpAddrSrc && (0 != pcIpAddrSrc[0]));
	int i = 0;

	DWORD dwIp = 0;

	char acTemp[20];

	char *pcIpAddr = acTemp;

	char *pcSaveptr = NULL;

	snprintf(acTemp, sizeof(acTemp), "%s", pcIpAddrSrc);

	if (1 == CIU_GetIsIPRightEnable())	// 需要检查IP权限
	{

		char seps[] = " .,";

		char *token;

		MPU_DBG("pcIpAddr %s\n", pcIpAddr);
		// 1
		token = strtok_r(pcIpAddr, seps, &pcSaveptr);
		if (!token)
		{
			MPU_ERR("pcIpAddr %s err\n", pcIpAddr);
			return 0;
		}
		dwIp += (((DWORD) (atoi(token))) << 24);
		// 2
		token = strtok_r(NULL, seps, &pcSaveptr);
		if (!token)
		{
			MPU_ERR("pcIpAddr %s err\n", pcIpAddr);
			return 0;
		}
		dwIp += (((DWORD) (atoi(token))) << 16);
		// 3
		token = strtok_r(NULL, seps, &pcSaveptr);
		if (!token)
		{
			MPU_ERR("pcIpAddr %s err\n", pcIpAddr);
			return 0;
		}
		dwIp += (((DWORD) (atoi(token))) << 8);
		// 4
		token = strtok_r(NULL, seps, &pcSaveptr);
		if (!token)
		{
			MPU_ERR("pcIpAddr %s err\n", pcIpAddr);
			return 0;
		}
		dwIp += (((DWORD) (atoi(token))));

		MPU_DBG("dwIp %d %d %d %d\n", (int) (dwIp & 0xff000000) >> 24,
				(int) (dwIp & 0xff0000) >> 16, (int) (dwIp & 0xff00) >> 8, (int) (dwIp & 0xff));

		char acKey[20];

		int iIP = 0;

		for (i = 0; i < DISABLEIP_COUNT; i++)
		{
			snprintf(acKey, sizeof(acKey), "IP_ENABLE_%d", i);
			iIP = MPU_PAR_GetValue("REMOTELOGIN", acKey, 0);
			if (dwIp == (DWORD) iIP)
			{
				MPU_DBG("find %lu\n", (DWORD) iIP);
				return 1;		// 许可
			}
		}

		return 0;				// 不许可
	}
	else						// 不检查IP
	{
		return 1;
	}

}



//!\brief  将字符IP转换为数字
int CIU_TransIpFromStrToDw(char *pcIpAddr, DWORD * pdwIpaddr)
{
	char *pcSaveptr = NULL;

	if (NULL == pdwIpaddr)
	{
		return -1;
	}
	*pdwIpaddr = 0;

	if (NULL == pcIpAddr)
	{
		return -1;
	}

	char szIpAddr[20];

	char seps[] = " .,";

	char *token;

	DWORD dwIp = 0;

	strncpy(szIpAddr, pcIpAddr, sizeof(szIpAddr) - 1);
	pcIpAddr = szIpAddr;

	// 1
	token = strtok_r(pcIpAddr, seps, &pcSaveptr);
	if (!token)
	{
		MPU_DBG("pcIpAddr %s err\n", pcIpAddr);
		return -1;
	}
	dwIp += (((DWORD) (atoi(token))) << 24);
	// 2
	token = strtok_r(NULL, seps, &pcSaveptr);
	if (!token)
	{
		MPU_DBG("pcIpAddr %s err\n", pcIpAddr);
		return -1;
	}
	dwIp += (((DWORD) (atoi(token))) << 16);
	// 3
	token = strtok_r(NULL, seps, &pcSaveptr);
	if (!token)
	{
		MPU_DBG("pcIpAddr %s err\n", pcIpAddr);
		return -1;
	}
	dwIp += (((DWORD) (atoi(token))) << 8);
	// 4
	token = strtok_r(NULL, seps, &pcSaveptr);
	if (!token)
	{
		MPU_DBG("pcIpAddr %s err\n", pcIpAddr);
		return -1;
	}
	dwIp += (((DWORD) (atoi(token))));

	*pdwIpaddr = dwIp;

	return 0;
}

//!\brief  远程用户登陆
//!\param   pUserID   登陆成功后的用户码
//!\param   LPRemoteUserInfo 用户信息
//!\return  0表示成功, 1 IP限制,2 用户信息错误, 3登陆失败(可能MAC限制),4 登陆用户过多, 5 用户被禁用
int UserLogin(eProtocolID eProtocol, DWORD * pUserID, const LPRemoteUserInfo pRemoteUserInfo)
{
	MPU_DBG("%s %s %s\n", pRemoteUserInfo->sUserName, pRemoteUserInfo->sClientMACAddress, pRemoteUserInfo->sUserIPAddress);

	int iRet = 0;

	int i = 0;

	unsigned char acMac[6];

	//char acUserPass[USERPASS_LEN];    // 用户密码 
	char *pcMacStr = pRemoteUserInfo->sClientMACAddress;

	// admin用户则直接登陆
	if (0 == strcmp(pRemoteUserInfo->sUserName, ADMIN_USRNAME))
	{
		goto login;
	}

	// 检查ip
	iRet = UserCheckIP(pRemoteUserInfo->sUserIPAddress);
	if (iRet != 1)
	{
		MPU_INFO("IP %s disable %d\n", pRemoteUserInfo->sUserIPAddress, iRet);
		return 1;
	}

	// 准备验证
	for (i = 0; i < 6; i++)
	{
		acMac[i] = strtol(pcMacStr, NULL, 16);
		pcMacStr = pcMacStr + 3;
		//MPU_DBG("%x", acMac[i]);
	}

	/*iRet = MPU_USR_GetPasswordByName((char *) pRemoteUserInfo->sUserName, acUserPass, USERPASS_LEN);
	   if (0 != iRet)
	   {
	   MPU_INFO("get password fail\n");
	   return 2;                // 
	   } */

	if (0 == strcmp(pRemoteUserInfo->sUserName, DEFAULT_USRNAME))
	{
		MPU_INFO("Is default user\n");
		return 2;
	}

  login:
	HANDLE hUserhandle = NULL;

	// admin用户则直接登陆
	if (0 == strcmp(pRemoteUserInfo->sUserName, ADMIN_USRNAME))
	{
		hUserhandle = MPU_USR_CheckPassword(pRemoteUserInfo->sUserName, pRemoteUserInfo->sUserPass, NULL);
	}
	else
	{
		MPU_DBG("--acMac[6]-- %x %x %x %x %x %x -------\n",acMac[0],acMac[1],acMac[2],acMac[3],acMac[4],acMac[5]);
		hUserhandle = MPU_USR_CheckPassword(pRemoteUserInfo->sUserName, pRemoteUserInfo->sUserPass, acMac);
	}

	if (INVALID_HANDLE == hUserhandle)
	{
		MPU_INFO("check password fail\n");
		return 3;				// 
	}
	else
	{							// 验证成功
		if (0 == MPU_USR_GetEnable(hUserhandle))
		{
			MPU_INFO("is disable user\n");
			return 5;
		}

		int i = 0;

		// 添加远程用户
		for (i = 0; i < LOGINUSER_COUNT; i++)
		{
			MPU_DBG("usr[%d] %s\n", i, g_atLoginUser[i].acUserName);
			if (0 == strcmp(g_atLoginUser[i].acUserName, ""))
			{
				break;
			}
		}

		if (i < LOGINUSER_COUNT)
		{
			time_t nowtime;

			struct tm tNt;

			struct tm *pnt = &tNt;

			g_atLoginUser[i].hUser = hUserhandle;
			g_atLoginUser[i].eProtocol = eProtocol;
			strncpy(g_atLoginUser[i].acUserName, pRemoteUserInfo->sUserName, USERNAME_LEN - 1);
			strncpy(g_atLoginUser[i].acUserIPAddress, pRemoteUserInfo->sUserIPAddress, IP_ADDRESS_LEN - 1);
			*pUserID = i;

			time(&nowtime);
			pnt = localtime_r(&nowtime, &tNt);
			g_atLoginUser[i].dwTime = (DWORD) nowtime;
			snprintf(g_atLoginUser[i].acTime, sizeof(g_atLoginUser[i].acTime), "%04d-%02d-%02d %02d:%02d:%02d",
					pnt->tm_year + 1900, pnt->tm_mon + 1, pnt->tm_mday, pnt->tm_hour, pnt->tm_min, pnt->tm_sec);

			MPU_DBG("usr login %s %d\n", g_atLoginUser[i].acUserName, i);

			DWORD dwIpaddr = 0;

			char acIPAddress[30];

			strncpy(acIPAddress, pRemoteUserInfo->sUserIPAddress, IP_ADDRESS_LEN - 1);
			CIU_TransIpFromStrToDw(acIPAddress, &dwIpaddr);

            /* 如果用户IP不为255.255.255.255，则需要记录日志信息 */
            if (strncmp(pRemoteUserInfo->sUserIPAddress, LOGIN_DEFAULT_IP, IP_ADDRESS_LEN))
            {
    			MPU_OPL_AddLog(EDVR_EVENT_USER, EDVR_EVENT_USR_DETAIL_LOGIN, 0, 0, dwIpaddr, pRemoteUserInfo->sUserName);
    			MPU_INFO("login over %d\n", i);
            }
			return 0;
		}
		else
		{
			MPU_INFO("out of LOGINUSER_COUNT \n");
			return 4;			// 登陆数超限
		}
	}
}


//!\brief  远程用户退出登陆
//!\param   pUserID   登陆用户码
//!\return  0表示成功，小于0表示失败
int UserLogout(DWORD dwUserID)
{

	if (dwUserID < LOGINUSER_COUNT)
	{
		MPU_DBG("id : %lu   %s\n", dwUserID, g_atLoginUser[dwUserID].acUserName);

		DWORD dwIpaddr = 0;

		char acIPAddress[30];

		strncpy(acIPAddress, g_atLoginUser[dwUserID].acUserIPAddress, IP_ADDRESS_LEN - 1);
		CIU_TransIpFromStrToDw(acIPAddress, &dwIpaddr);
        /* 如果用户IP不为255.255.255.255, 则需要记录日志信息 */
        printf("Default Logout Log UserIP is %s\n", acIPAddress);
        if (strncmp(acIPAddress, LOGIN_DEFAULT_IP, 30))
		{
		    MPU_OPL_AddLog(EDVR_EVENT_USER, EDVR_EVENT_USR_DETAIL_LOGOUT, 0, 0, dwIpaddr, g_atLoginUser[dwUserID].acUserName);
        }
		memset(&g_atLoginUser[dwUserID], 0, sizeof (g_atLoginUser[dwUserID]));
		MPU_DBG("UserLogout over\n");
		return 0;
	}
	else
	{
		MPU_ERR("dwUserID %lu\n", dwUserID);
		return -1;
	}
}


//!\brief  获取是否启用IP 权限管理
//!\param   
//!\return  0 不启用，1启用,其它错误
int CIU_GetIsIPRightEnable()
{
	int iEnable = 0;

	iEnable = MPU_PAR_GetValue("REMOTELOGIN", "IP_ENABLE_FLAG", 0);
	if (iEnable >= 0)

	{
		return iEnable;
	}
	else
	{
		MPU_ERR("MPU_PAR_GetValue(REMOTELOGIN, IP_ENABLE_FLAG, 0) erro %d\n", iEnable);
		return -1;
	}
}

//!\brief  设置是否启用IP 权限管理
//!\param    0 不启用，1启用
//!\return 
int CIU_SetIsIPRightEnable(int iIpRightEnable)
{
	int iRet = 0;

	if (iIpRightEnable != 0 && iIpRightEnable != 1)
	{
		MPU_ERR("iIpRightEnable erro %d\n", iIpRightEnable);
		return -1;
	}
	iRet = MPU_PAR_SetValue("REMOTELOGIN", "IP_ENABLE_FLAG", iIpRightEnable);
	if (iRet == 0)
	{
		return 0;
	}
	else
	{
		MPU_ERR("MPU_PAR_SetValue(REMOTELOGIN, IP_ENABLE_FLAG, 0) erro %d\n", iRet);
		return -1;
	}

}

//!\brief  获取许可ip
//!\param   pdwIPEnableList ip数组的地址
//!\return  许可的ip的个数
int CIU_GetIPEnableList(DWORD * pdwIPEnableList)
{
	int i = 0;

	int iIpEnableNum = 0;

	int iIP = 0;

	MPU_CHK(pdwIPEnableList);
	char acKey[20];

	for (i = 0; i < DISABLEIP_COUNT; i++)
	{
		snprintf(acKey, sizeof(acKey), "IP_ENABLE_%d", i);
		iIP = MPU_PAR_GetValue("REMOTELOGIN", acKey, 0);
		if (0 != iIP)
		{
			MPU_DBG("find ip %d\n ", iIP);
			*(pdwIPEnableList + iIpEnableNum) = iIP;
			iIpEnableNum++;
		}
	}
	return iIpEnableNum;
}

//!\brief  添加许可IP
//!\param   szIPAddress ip
//!\return  0成功
int CIU_ChangeIPEnable(DWORD dwIpAddr, int iEnable)
{
	if (0 == dwIpAddr)
	{
		MPU_ERR("dwIpAddr == 0\n");
		return -1;
	}
	int i = 0;

	int iIP = 0;

	char acKey[20];

	if (0 == iEnable)			// 删除IP
	{
		for (i = 0; i < DISABLEIP_COUNT; i++)
		{
			snprintf(acKey, sizeof(acKey), "IP_ENABLE_%d", i);
			iIP = MPU_PAR_GetValue("REMOTELOGIN", acKey, 0);
			if (dwIpAddr == (DWORD) iIP)
			{
				if (0 != MPU_PAR_SetValue("REMOTELOGIN", acKey, 0))
				{
					MPU_ERR("delete ip %lu err\n ", dwIpAddr);
					return -1;
				}
				MPU_INFO("delete ip %lu ok\n ", dwIpAddr);
				return 0;
			}
		}
		MPU_ERR("delete ip %lu err\n ", dwIpAddr);
		return -1;
	}
	else
	{
		// 查找是否已存在
		for (i = 0; i < DISABLEIP_COUNT; i++)
		{
			snprintf(acKey, sizeof(acKey), "IP_ENABLE_%d", i);
			iIP = MPU_PAR_GetValue("REMOTELOGIN", acKey, 0);
			if (dwIpAddr == (DWORD) iIP)
			{
				MPU_INFO("old ip %lu\n ", dwIpAddr);
				return 0;
			}
		}
		// 添加新的IP
		for (i = 0; i < DISABLEIP_COUNT; i++)
		{
			snprintf(acKey, sizeof(acKey), "IP_ENABLE_%d", i);
			iIP = MPU_PAR_GetValue("REMOTELOGIN", acKey, 0);
			if (0 == iIP)
			{
				MPU_INFO("new ip[%d] %s %lu\n ", i, acKey, dwIpAddr);
				if (0 != MPU_PAR_SetValue("REMOTELOGIN", acKey, dwIpAddr))
				{
					MPU_ERR("MPU_PAR_SetValue err\n");
					return -1;
				}
				return 0;
			}
		}

		MPU_ERR("add erro %lu , may be out of count\n", dwIpAddr);
		return -1;
	}
}


//!\brief  清除所有IP
//!\return  0成功
int CIU_ClearIPEnableList()
{
	int i = 0;

	char acKey[20];

	int iRet = 0;

	for (i = 0; i < DISABLEIP_COUNT; i++)
	{
		snprintf(acKey, sizeof(acKey), "IP_ENABLE_%d", i);
		if (0 != MPU_PAR_SetValue("REMOTELOGIN", acKey, 0))
		{
			MPU_ERR("delete ip %s err\n ", acKey);
			iRet = -1;
		}
	}
	return iRet;
}

//!\brief  获取用户在线信息
//!\return  0成功
int CIU_IsOnline(HANDLE hUser)
{
	if (hUser)
	{
		int i = 0;

		for (i = 0; i < LOGINUSER_COUNT; i++)
		{
			if (hUser == g_atLoginUser[i].hUser)
			{
				return 1;
			}
		}
	}
	else
	{
		return 0;
	}
	return 0;
}

//!\brief  获取用户信息
//!\param   
//!\param  
//!\return  0成功
int CIU_GetUserInfo(DWORD dwUserID, HANDLE * hUser, const char **ppcUserName, DWORD * pdwTime, const char **ppcIpAddr, eProtocolID * peProtocol)
{
	if (dwUserID >= LOGINUSER_COUNT)
	{
		MPU_ERR("erro dwUserID %lu\n", dwUserID);
		*ppcUserName = NULL;
		*pdwTime = 0;
		*ppcIpAddr = NULL;
		return -1;
	}
	else
	{
		if (NULL == g_atLoginUser[dwUserID].hUser)
		{
			//MPU_DBG("UserID %lu is NULL\n", dwUserID);
			*ppcUserName = NULL;
			*pdwTime = 0;
			*ppcIpAddr = NULL;
			return -1;
		}

		*hUser = g_atLoginUser[dwUserID].hUser;
		*peProtocol = g_atLoginUser[dwUserID].eProtocol;
		*ppcUserName = g_atLoginUser[dwUserID].acUserName;
		*pdwTime = g_atLoginUser[dwUserID].dwTime;
		*ppcIpAddr = g_atLoginUser[dwUserID].acUserIPAddress;
		MPU_DBG("hUser %lu ;pcUserName %s; pdwTime %lu; pcIpAddr %s\n", (DWORD) hUser,
				(char *) (*ppcUserName), (DWORD) (*pdwTime), (char *) (*ppcIpAddr));
		return 0;
	}
}

//!\brief  获取用户信息
//!\param   
//!\param  
//!\return  0成功
int CIU_GetUserInfo(DWORD dwUserID, HANDLE * hUser, const char **ppcUserName, const char **ppcTime, const char **ppcIpAddr)
{
	if (dwUserID >= LOGINUSER_COUNT)
	{
		MPU_ERR("erro dwUserID %lu\n", dwUserID);
		*ppcUserName = NULL;
		*ppcTime = NULL;
		*ppcIpAddr = NULL;
		return -1;
	}
	else
	{
		if (NULL == g_atLoginUser[dwUserID].hUser)
		{
			//MPU_DBG("UserID %lu is NULL\n", dwUserID);
			*ppcUserName = NULL;
			*ppcTime = NULL;
			*ppcIpAddr = NULL;
			return -1;
		}

		*hUser = g_atLoginUser[dwUserID].hUser;
		*ppcUserName = g_atLoginUser[dwUserID].acUserName;
		*ppcTime = g_atLoginUser[dwUserID].acTime;
		*ppcIpAddr = g_atLoginUser[dwUserID].acUserIPAddress;
		MPU_DBG("hUser %lu ;pcUserName %s; pcTime %s; pcIpAddr %s\n", (DWORD) hUser,
				(char *) (*ppcUserName), (char *) (*ppcTime), (char *) (*ppcIpAddr));
		return 0;
	}
}

//!\brief  获取用户信息
//!\param  ppcUserName  用户名称 
//!\param  pdwIpAddr        用户IP地址
//!\return  0成功
int CIU_GetUserInfo(DWORD dwUserID, const char **ppcUserName, unsigned long *pdwIpAddr)
{
	if (dwUserID >= LOGINUSER_COUNT)
	{
		MPU_ERR("erro dwUserID %lu\n", dwUserID);
		*ppcUserName = NULL;
		*pdwIpAddr = 0;
		return -1;
	}
	else if (NULL == g_atLoginUser[dwUserID].hUser)
	{
		//MPU_ERR("UserID %lu is NULL\n", dwUserID);
		*ppcUserName = NULL;
		*pdwIpAddr = 0;
		return -1;
	}
	else
	{
		*ppcUserName = g_atLoginUser[dwUserID].acUserName;
		CIU_TransIpFromStrToDw(g_atLoginUser[dwUserID].acUserIPAddress, pdwIpAddr);
		return 0;
	}
}

//!\brief  获取用户查询参数
//!\param   dwUserID 用户ID
//!\return  PT_REMOTEQUEERY 查询参数的句柄
PT_REMOTEQUEERY CIU_GetLogQueryParam(DWORD dwUserID)
{
	MPU_CHK(dwUserID < LOGINUSER_COUNT);
	return &(g_atLoginUser[dwUserID].tLogQuery);
}

//!\brief       添加远程操作日志
//!\param   dwUserID                用户ID
//!\param   nLogType                    日志类型
//!\param   nDetail             日志详细信息
//!\param   nAlertChannel           产生日志信息的通道号
//!\param   dwRecordChannelFlag 联动录像的通道标记
//!\return  0添加成功 -1表示失败
int CIU_OPL_AddLog(DWORD dwUserID, int nLogType, int nDetail, int nAlertChannel, unsigned long dwRecordChannelFlag)
{
	unsigned long dwIpAddr;

	const char *pszUserName;

	//记录创建用户日志
	if (0 == CIU_GetUserInfo(dwUserID, &pszUserName, &dwIpAddr))
	{
		return MPU_OPL_AddLog(nLogType, nDetail, nAlertChannel, dwRecordChannelFlag, dwIpAddr, (char *) pszUserName);
	}

	return -1;
}

//!\brief       判断是否同一用户
//!\return  0表示不是，1表示是
int CIU_IsUser(HANDLE hUser, const char *pszUserName)
{
	BOOL bIsUser;
	MPU_DBG("---1---hUser=%d  hUser=%s\n",hUser,pszUserName);

	char szRealUserName[MAX_USERNAME_LEN + 1] = {0};

	int nRet;

	//判断登录用户类型
	nRet = MPU_USR_GetUserName(hUser, szRealUserName, MAX_USERNAME_LEN);
	if (IDVR_OK == nRet)
	{
		bIsUser = (0 == strcmp(szRealUserName, pszUserName));
	}
	else
	{
		bIsUser = FALSE;
	}

	return bIsUser;
}

//!\brief       判断是否同一用户
//!\return  0表示不是，1表示是
int CIU_IsUser(DWORD dwUserID, HANDLE hUser)
{
	BOOL bIsUser;
	MPU_DBG("---2---dwUserID=%d  hUser=%d\n",dwUserID,hUser);
    if (dwUserID == VALUE_INVALUE)
    {
        return TRUE;
    }

	if (dwUserID < LOGINUSER_COUNT)
	{
		if (g_atLoginUser[dwUserID].hUser)
		{
			bIsUser = (g_atLoginUser[dwUserID].hUser == hUser);
		}
		else
		{
			MPU_ERR("dwUserID %lu not exist\n", dwUserID);
			bIsUser = FALSE;
		}
	}
	else
	{
		MPU_ERR("dwUserID %lu is overflow\n", dwUserID);
		bIsUser = FALSE;
	}

	return bIsUser;
}

//!\brief       判断是否同一用户
//!\return  0表示不是，1表示是
int CIU_IsUser(DWORD dwUserID, const char *pszUserName)
{
	BOOL bIsUser;
MPU_DBG("---3---dwUserID=%d  pszUserName=%s\n",dwUserID,pszUserName);
    if (dwUserID == VALUE_INVALUE)
    {
        return TRUE;
    }
	if (dwUserID < LOGINUSER_COUNT)
	{
		if (g_atLoginUser[dwUserID].hUser)
		{
			bIsUser = (0 == strcmp(g_atLoginUser[dwUserID].acUserName, pszUserName));
		}
		else
		{
			MPU_ERR("dwUserID %lu not exist\n", dwUserID);
			bIsUser = FALSE;
		}
	}
	else
	{
		MPU_ERR("dwUserID %lu is overflow\n", dwUserID);
		bIsUser = FALSE;
	}

	return bIsUser;
}

int TutkGetAdminUserID(DWORD *pdwUserID)
{
	int i;

	if(NULL == pdwUserID)
		return -1;

	for(i=0; i<LOGINUSER_COUNT; i++)
	{
		if(0 == strcmp(g_atLoginUser[i].acUserName, ADMIN_USRNAME))
		{
			*pdwUserID = i;
			break;
		}
	}

	return 0;
}

//!\brief       判断是否具有权限
//!\return  0表示无权限，1表示有权限
int CIU_CheckRemoteUser(DWORD dwUserID, REMOTE_ACTION eAction)
{
	if (dwUserID < LOGINUSER_COUNT)
	{
		if (g_atLoginUser[dwUserID].hUser)
		{
			return MPU_USR_GetRemotePrivilege(g_atLoginUser[dwUserID].hUser, eAction);
		}
		else
		{
			MPU_ERR("dwUserID %lu not exist\n", dwUserID);
			return 0;
		}
	}
	else if (dwUserID == (DWORD) (-1))
	{
		//特殊用户
		return 1;
	}
	else
	{
		MPU_ERR("dwUserID %lu not exist\n", dwUserID);
		return 0;
	}

}

//!\brief       判断是否具有权限
//!\return  0表示无权限，1表示有权限
int CIU_CheckRemoteReadUser(DWORD dwUserID, REMOTE_ACTION_V1 eAction)
{
	if (dwUserID < LOGINUSER_COUNT)
	{
		if (g_atLoginUser[dwUserID].hUser)
		{
			return MPU_USR_GetRemoteReadPrivilege(g_atLoginUser[dwUserID].hUser, eAction);
		}
		else
		{
			MPU_ERR("dwUserID %lu not exist\n", dwUserID);
			return 0;
		}
	}
	else if (dwUserID == (DWORD) (-1))
	{
		//特殊用户
		return 1;
	}
	else
	{
		MPU_ERR("dwUserID %lu not exist\n", dwUserID);
		return 0;
	}

}

//!\brief       判断是否具有权限
//!\return  0表示无权限，1表示有权限
int CIU_CheckRemoteUser(DWORD dwUserID, CHANNEL_ACTION eAction, int nCh)
{
	if (dwUserID < LOGINUSER_COUNT)
	{
		if (g_atLoginUser[dwUserID].hUser)
		{
		    //此处暂时屏蔽掉权限的判断，后续整理权限时再做处理
		    return 1;
			return MPU_USR_GetChannelPrivilege(g_atLoginUser[dwUserID].hUser, eAction, nCh, 1);
		}
		else
		{
			MPU_ERR("dwUserID %lu not exist\n", dwUserID);
			return 0;
		}
	}
	else if (dwUserID == (DWORD) (-1))
	{
		//特殊用户
		return 1;
	}
	else
	{
		return 0;
	}
}


//!\brief   网络服务重启
void CIU_ResetServer_Logout()
{
	int iClientIndex = 0;

	int iRet = 0;

	for (iClientIndex = 0; iClientIndex < LOGINUSER_COUNT; iClientIndex++)
	{
		if (g_atLoginUser[iClientIndex].hUser)
		{
			if (iRet != HieServer_DeleteUserForce(iClientIndex))
			{
				MPU_ERR("HieServer_DeleteUserForce(%d) err %d\n", iClientIndex, iRet);
			}
		}
	}
	memset(g_atLoginUser, 0, sizeof (g_atLoginUser));
}

//!\brief   初始化远程预览
void CIU_RegisterLogin()
{
	int nSDKRet;

	//  设置用户操作回调接口    
	memset(g_atLoginUser, 0, sizeof (g_atLoginUser));
	//MPU_DBG("net init %d\n", sizeof (g_atLoginUser));
	UserActionInterface tUserInterface;

	tUserInterface.ifUserLogin = UserLogin;
	tUserInterface.ifUserLogout = UserLogout;
	tUserInterface.ifUserGetPassByName = UserGetPassword;
	nSDKRet = HieServer_UserActionIf(&tUserInterface);
	if (0 != nSDKRet)
	{
		MPU_ERR("HieServer_UserActionIf() erro %d\n", nSDKRet);
	}
	MPU_DBG("HieServer_UserActionIf() return %d\n", nSDKRet);
	//UserStartCheckIp();
}
