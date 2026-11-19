// Copyright (C) 1991 - 1999 Rational Software Corporation
//#include <iostream>
#include <fstream>

//#include "hisdk.h"
#include "zwsdk.h"
#include "USRTrace.h"
#include "global_macro.h"
#include "idvr_errno.h"
#include "PAR/CConfig.h"
#include "CUserDao.h"
#include "CUserDaoEx.h"
#include "CUserManager.h"
#include "mysystem.h"
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <net/if.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include "../PAR/CPubFunc.h"
#include "des.h"
// #include "sys_banned.h"
#include "global_gather.h"
#include "common_pub.h"

const char *s_aszIllegeAccessActionKey[] = {"Beep_Enable", "Server_Enable", "Email_Enable", "GUI_Enable", "AlarmOut_Enable", "Push_Enable", "LightAlarm_Enable"};

#define USR_LOCK pthread_mutex_lock(&m_mtxUser)

#define USR_UNLOCK pthread_mutex_unlock(&m_mtxUser)

//##ModelId=49BE185C02C2
CUserManager *CUserManager::m_pInstance = NULL;

 /**************************************************************************
  * 					公共函数实现									   *
  **************************************************************************/
static int FormatMacAddress(const char *if_name, char *dest_str)
{
    struct ifreq ifr;

    char tmp_buf[32] = {0};

    int sock = -1; 
    int ret  = 0;

    unsigned char *ptr = NULL;

    if ((!if_name) || (!dest_str))
    {
        return -1;
    }

    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        return errno;
    }
    
    strncpy(ifr.ifr_name, if_name, 16);
    if (ioctl(sock, SIOCGIFHWADDR, &ifr) >= 0)
    {
        int mac0, mac1, mac2, mac3, mac4, mac5;
        ptr = (unsigned char *) & ifr.ifr_ifru.ifru_hwaddr.sa_data[0];
       snprintf(tmp_buf,sizeof(tmp_buf), "%X:%X:%X:%X:%X:%X", *ptr, *(ptr + 1), *(ptr + 2), *(ptr + 3), *(ptr + 4), *(ptr + 5));    
        sscanf(tmp_buf, "%X:%X:%X:%X:%X:%X", &mac0, &mac1, &mac2, &mac3, &mac4, &mac5);
        dest_str[0] = mac0;
        dest_str[1] = mac1;
        dest_str[2] = mac2;
        dest_str[3] = mac3;
        dest_str[4] = mac4;
        dest_str[5] = mac5;
        dest_str[6] = 0x0;
        dest_str[7] = 0x0;
    }
    else
    {
        ret = errno;
    }

    close(sock);

    return ret;
}

//#define DES_DBG

static int  DesEncode(const char *if_name, char *dest_psk)
{
    if (NULL==if_name || NULL==dest_psk)
    {
        USR_ERROR("NULL POINTER EXCEPTION!\n");
        return -1;
    }
    
    int  i = 0; 
    char out_msg[8]={0};
    char key_str[8]={0};
    char mac_msg[8]={0};
    
    if (FormatMacAddress(if_name, mac_msg))
    {
        USR_ERROR("Fail to called FormatMacAddress");
        return -1;
    }
    
#ifdef DES_DBG
    printf("mac_msg: ");
    for (i=0; i<8; i++)
    {
        printf("%#x ", mac_msg[i]);
    }
    printf("\n");
#endif

    snprintf(key_str, sizeof(key_str), "BITRONV");

    key_str[7] = mac_msg[0] ^ mac_msg[1] ^ mac_msg[2] ^ mac_msg[3] ^ mac_msg[4] ^ mac_msg[5];

#ifdef DES_DBG
    printf("key_str: ");
    for (i=0; i<8; i++)
    {
        printf("%#x ", key_str[i]);
    }
    printf("\n");
#endif

    DesEncryption(out_msg, mac_msg, sizeof(mac_msg), key_str, sizeof(key_str), 1);

#ifdef DES_DBG
    printf("out_msg: ");
    for (i=0; i<8; i++)
    {
        printf("%#x ", out_msg[i]);
    }
    printf("\n");
#endif

    for (i=0; i<8; i++)
    {
        dest_psk[i] = out_msg[i];        
    }
    
    return 0;
}



/*@ preif 因为配置分区满了后，sqlite加载了配置分区的数据库后到
 *@让程序崩溃,为了让配置分区满了后程序依然能跑，所以加载
 *@ 前需要判断分区是否满，如果满了就将数据库拷贝到内存，
 *@然后再加载
 *@ return 0 分区未满
 *@ return 1 分区已满
 */
static int ISCfgPartitionFull()
{
	int ret = 0;
	char command[64] = {0};
	char testFile[] = "/mnt/mtd/config/.checkFull";

	//检查配置分区挂载点
	ret = IsStringInCommand("mount", "config");
	if(1 == ret)
	{
		//检查分区是否满
		snprintf(command,sizeof(command), "touch %s", testFile);
		mysystem(command);

		if(0 == access(testFile, F_OK))
		{
			//分区未满
			memset(command, 0, sizeof(command));
			snprintf(command,sizeof(command), "rm %s", testFile);
			mysystem(command);	
			
			return 0;
		}
		else
			{
				return 1;
			}
	}
	else
		{
			//配置分区未挂载或者挂载点不是config，按默认处理
			return 0;
		}
}

//##ModelId=499CCB2A02F9
CUserManager::CUserManager():m_nUserCount(0), m_nAlarmType(0), m_fnIllegeAccess(NULL), m_nWorkMode(0), m_nChannelCount(0)
{
	pthread_mutex_init(&m_mtxUser, NULL);
	// TODO: Add your specialized code here.

    char value[64];
    memset(value, 0 , sizeof(value));
	memset(m_szDesSuperPwd, 0, sizeof(m_szDesSuperPwd));
}

//##ModelId=499CCB3A01A7
CUserManager::~CUserManager()
{
#ifdef DAOEX
	CUserDaoEx *pDao = CUserDaoEx::GetInstance();
	CUserDaoEx::DestroyInstance(pDao);
#else
	CUserDao *pDao = CUserDao::GetInstance();
	CUserDao::DestroyInstance(pDao);
#endif	
	// TODO: Add your specialized code here.
}

//!\brief 获取CUserManager实例
//! 若CUserManager实例第一次创建则从用户数据库
//! 加载用户信息，初始化用户列表
//!\return NULL表示获取失败；其他为CUserManager实例指针
//##ModelId=499CCB5500E8
CUserManager *CUserManager::USR_GetInstance()
{
    int UserInfoReset = 0;
    char command[128] = {0};
    
	if (m_pInstance == (CUserManager *)0)
	{
		m_pInstance = new CUserManager();
		if (m_pInstance != NULL)
		{
			CConfig *pConfig = CConfig::GetInstance();
			if (NULL == pConfig)
			{
				delete m_pInstance;
				m_pInstance = NULL;
				return m_pInstance;
			}

            UserInfoReset = pConfig->GetValue("UserInfo", "ResetUserNamePwd", 0);

			m_pInstance->m_nWorkMode = pConfig->GetValue("USR", "WorkMode", 0);
			m_pInstance->m_nChannelCount = ZWSys_GetTotalEncChannels() ; 
			pConfig->GetValue("USR", "superpass", "909776",m_pInstance->m_szSuperPassword,sizeof(m_pInstance->m_szSuperPassword));

			m_pInstance->m_nActionParam[USR_BEEP_KEY] = pConfig->GetValue(ILLEGEACCESS_SECTION, s_aszIllegeAccessActionKey[USR_BEEP_KEY], 0);
			m_pInstance->m_nActionParam[USR_SERVER_KEY] = pConfig->GetValue(ILLEGEACCESS_SECTION, s_aszIllegeAccessActionKey[USR_SERVER_KEY], 0);
			m_pInstance->m_nActionParam[USR_EMAIL_KEY] = pConfig->GetValue(ILLEGEACCESS_SECTION, s_aszIllegeAccessActionKey[USR_EMAIL_KEY], 0);
			m_pInstance->m_nActionParam[USR_GUI_KEY] = pConfig->GetValue(ILLEGEACCESS_SECTION, s_aszIllegeAccessActionKey[USR_GUI_KEY], 0);
			m_pInstance->m_nActionParam[USR_ALARMOUT_KEY] = pConfig->GetValue(ILLEGEACCESS_SECTION, s_aszIllegeAccessActionKey[USR_ALARMOUT_KEY], 0);
			m_pInstance->m_nActionParam[USR_PUSH_KEY] = pConfig->GetValue(ILLEGEACCESS_SECTION, s_aszIllegeAccessActionKey[USR_PUSH_KEY], 0);
#ifdef DAOEX
			CUserDaoEx *pDao = CUserDaoEx::GetInstance();
#else
			CUserDao *pDao = CUserDao::GetInstance();
#endif
			if (NULL == pDao)
			{
				USR_ERROR("[%s]ERR: Create UserDao failed", __FUNCTION__);
				delete m_pInstance;
				m_pInstance = NULL;
			}
			else
			{
				int ret = 0;
				ret = ISCfgPartitionFull();
				if(0 == ret)
				{           
                #ifdef DAOEX
                    if (1 == UserInfoReset)
                    {
                        //需要重新生成user.txt文件
                        memset(command, 0, sizeof(command));
                        snprintf(command,sizeof(command), "rm -rf %s", USR_DB_FILE_PATH_TXT);
                        mysystem(command);                    
                    }
					pDao->BindDb(USR_DB_FILE_PATH_TXT, UserInfoReset);
                #else
					pDao->BindDb(USR_DB_FILE_PATH);
		        #endif
				}
				else
				{
					char command[64] = {0};
					//如果分区满则将数据库拷贝到内存
					memset(command, 0, sizeof(command));
					snprintf(command,sizeof(command), "cp /mnt/mtd/config/user.db /tmp/app/user.db");
					mysystem(command);

                #ifdef DAOEX
                    if (1 == UserInfoReset)
                    {
                        memset(command, 0, sizeof(command));
                        snprintf(command,sizeof(command), "rm -rf %s", USR_DB_FILE_PATH_TXT);
                        mysystem(command);                            
                    }

                    pDao->BindDb(USR_DB_FILE_PATH_TXT, UserInfoReset);
                #else
                    pDao->BindDb("/tmp/app/user.db");
                #endif

				}
				int nResult = m_pInstance->LoadUser();
				if (nResult != 0)
				{
					USR_ERROR("[%s]ERR:Load User Failed", __FUNCTION__);
					delete m_pInstance;
					m_pInstance = NULL;
				}

				if (1 == UserInfoReset)
				{
                    pConfig->SetValue("UserInfo", "ResetUserNamePwd", 0);
				}
			}
		}
	}
	// TODO: Add your specialized code here.
    if (16 != strlen(m_pInstance->m_szDesSuperPwd))
    {
        char dest_psk[8] = {0};
        //printf("start synthetic the super password!\n");
#if defined (__MODULE_4G_SUPPORT__)
        if (-1 == DesEncode("usb0", dest_psk))
#elif defined (__IS_SUPPORT_WLAN__)
        if (-1 == DesEncode("wlan0", dest_psk))
#else
        if (-1 == DesEncode("eth0", dest_psk))
#endif
        {
            USR_ERROR("Fail to called DesEncode\n");
        }
        else
        {
            snprintf(m_pInstance->m_szDesSuperPwd, sizeof(m_pInstance->m_szDesSuperPwd), "%02X%02X%02X%02X%02X%02X%02X%02X", 
                    0xFF&dest_psk[0], 0xFF&dest_psk[1], 0xFF&dest_psk[2], 0xFF&dest_psk[3], 
                    0xFF&dest_psk[4], 0xFF&dest_psk[5], 0xFF&dest_psk[6], 0xFF&dest_psk[7]);
			 m_pInstance->m_szDesSuperPwd[sizeof(m_pInstance->m_szDesSuperPwd) - 1] = '\0';
        }
    }

	// NOTE: Requires a correct return value to compile.
	return m_pInstance;
}

//\brief 释放CUserManager资源，清除用户列表
//\return 无
//##ModelId=499CCB610230
void CUserManager::USR_DestroyInstance()
{
	if (m_pInstance != NULL)
	{
		delete m_pInstance;
	}
	// TODO: Add your specialized code here.
}

//!\brief 获取超级密码
//!\return   0获取成功，获取失败为-1
int CUserManager::USR_GetSuperPassword(char *pszSuperPassword)
{   
    if (NULL == pszSuperPassword)
    {
        USR_ERROR("NULL POINTER EXCEPTION!\n");
        return -1;
    }
	
    //if (16 != strlen(m_szDesSuperPwd))
    {        
        char dest_psk[8] = {0};
#if defined (__MODULE_4G_SUPPORT__)
        if (-1 == DesEncode("usb0", dest_psk))
#elif defined (__IS_SUPPORT_WLAN__)
        if (-1 == DesEncode("wlan0", dest_psk))
#else
        if (-1 == DesEncode("eth0", dest_psk))
#endif
        {
            USR_ERROR("Fail to called DesEncode\n");
            return -1;
        }
        snprintf(m_szDesSuperPwd, sizeof(m_szDesSuperPwd), "%02X%02X%02X%02X%02X%02X%02X%02X", 
                0xFF&dest_psk[0], 0xFF&dest_psk[1], 0xFF&dest_psk[2], 0xFF&dest_psk[3], 
                0xFF&dest_psk[4], 0xFF&dest_psk[5], 0xFF&dest_psk[6], 0xFF&dest_psk[7]);
		m_pInstance->m_szDesSuperPwd[sizeof(m_pInstance->m_szDesSuperPwd) - 1] = '\0';
    }
    
    strncpy(pszSuperPassword, m_szDesSuperPwd, sizeof(m_szDesSuperPwd) - 1);

	return 0;
}

//!\brief 获取用户模块模式
//!\return   0为普通工作模式，1为高级工作模式，失败为-1
int CUserManager::USR_GetWorkMode()
{
	return m_nWorkMode;
}

//!\brief       设置用户管理的工作模式
//!\param   nWorkMode   0为普通工作模式，1为高级工作模式，失败为-1
//!\return  成功为0，失败为-1	
int CUserManager::USR_SetWorkMode(int nWorkMode)
{
	USR_LOCK;
	if (m_nWorkMode == nWorkMode)
	{
		USR_UNLOCK;
		return 0;
	}

	int nRet;

	//从配置文件初始化
	CConfig *pConfig = CConfig::GetInstance();

	if (NULL != pConfig)
	{
		nRet = pConfig->SetValue("USR", "WorkMode", nWorkMode);
		m_nWorkMode = nWorkMode;
		//恢复到普通模式不再恢复权限
		//if (0 == m_nWorkMode)
		//{
		//	ResumeDefault();
		//}
	}
	else
	{
		nRet = IDVR_ERROR_CONFIGFILE;
	}
	USR_UNLOCK;

	return nRet;
}

int CUserManager::ResumeDefault()
{
	int i = 0;
#ifdef DAOEX
    CUserDaoEx *pDao = CUserDaoEx::GetInstance();
#else
    CUserDao *pDao = CUserDao::GetInstance();
#endif

	for (i = 0; i < m_nUserCount; i++)
	{
		memset(m_tUserList[i].m_szMacAddress, 0, MAC_ADDRESS_BYTES);
		if (0 == strcmp(m_tUserList[i].m_szUsername, ADMIN_USRNAME))
		{
			pDao->SaveUser(m_tUserList[i]);
			continue;
		}
		#if 0
		else if (0 == strcmp(m_tUserList[i].m_szUsername, DEFAULT_USRNAME))
		{
			m_tUserList[i].m_dwPreview[1] = 0;
		}
		#endif
		else
		{
			m_tUserList[i].m_dwPreview[1] = (unsigned long)-1;
		}
		//m_tUserList[i].m_dwLocal = 0;
		//m_tUserList[i].m_dwRemote = 0;
		m_tUserList[i].m_dwPreview[0] = (unsigned long)-1;
		m_tUserList[i].m_dwPlayback[0] = 0;
		m_tUserList[i].m_dwPlayback[1] = 0;
		m_tUserList[i].m_dwPtzCtrl[0] = 0;
		m_tUserList[i].m_dwPtzCtrl[1] = 0;
		m_tUserList[i].m_dwManualRecord[0] = 0;
		m_tUserList[i].m_dwManualRecord[1] = 0;
		pDao->SaveUser(m_tUserList[i]);
	}

	return 0;
}

int CUserManager::USR_ResetAdminPassword()
{
	USR_LOCK;
#ifdef DAOEX
    CUserDaoEx *pDao = CUserDaoEx::GetInstance();
#else
    CUserDao *pDao = CUserDao::GetInstance();
#endif

	int nRet = 0;
	int i = 0;

	for (i = 0; i < m_nUserCount; i++)
	{
		if (0 == strcmp(m_tUserList[i].m_szUsername, ADMIN_USRNAME))
		{
			m_tUserList[i].SetPassword(DEFAULT_PASSWORD);
			nRet = pDao->SaveUser(m_tUserList[i]);
			break;
		}
	}

	unlink(UserFile);
	unlink(USR_DB_FILE_PATH);
	unlink(USR_DB_FILE_PATH_TXT);
	
	USR_UNLOCK;

	return nRet;
}

//!\brief 添加新的用户
//!\param pUser  指向要添加的新用户的指针
//!\return NULL表示添加成功 其他表示添加失败
//##ModelId=499CCB850354
HANDLE CUserManager::USR_AddUser(CUser *pUser)
{
	if (NULL == pUser)
	{
		USR_DBG("param error, pUser=%p\n", pUser);
		return INVALID_HANDLE;
	}
	
#ifdef DAOEX
	CUserDaoEx *pDao = CUserDaoEx::GetInstance();
#else
	CUserDao *pDao = CUserDao::GetInstance();
#endif
	USR_LOCK;
	if (IDVR_MAX_USER <= (unsigned int)m_nUserCount)
	{
		USR_UNLOCK;
		return INVALID_HANDLE;
	}

	HANDLE hUser = INVALID_HANDLE;
	//将用户插入到数据库
	if (0 == pDao->InsertUser(*pUser))
	{
		//重新获取用户
		m_nUserCount = pDao->GetUsers(m_tUserList, IDVR_MAX_USER);

		int i = 0;
		for (i = 0; i < m_nUserCount; i++)
		{
			//返回用户句柄
			if (0 == strcmp(m_tUserList[i].m_szUsername, pUser->m_szUsername))
			{
				hUser = (HANDLE)m_tUserList[i].m_nUserId;
				USR_UNLOCK;
				return hUser;
			}
		}
		USR_UNLOCK;
		return INVALID_HANDLE;
	}
	else
	{
		USR_UNLOCK;
		return INVALID_HANDLE;
	}
	
}

//!\brief 从用户列表删除用户
//!\param hUser 用户句柄
//!\return 0表示删除成功 -1表示删除失败
//##ModelId=499CCB9A0105
int CUserManager::USR_RemoveUser(HANDLE hUser)
{
	if (INVALID_HANDLE == hUser)
	{
		USR_DBG("param error, hUser = %ld", (long int)hUser);
		return IDVR_USR_ERROR_INVALID_HANDLE;
	}
	
	USR_LOCK;
	int ret = 0;

	//从数据库中删除该用户
#ifdef DAOEX
	CUserDaoEx *pDao = CUserDaoEx::GetInstance();
#else
	CUserDao *pDao = CUserDao::GetInstance();
#endif
	USR_DBG("hUser = %ld\n", (unsigned long)hUser);
	ret = pDao->DeleteUser((int)hUser);
	USR_DBG("ret = %d\n", ret);

	//用户不存在
	if (ret == IDVR_USR_ERROR_USER_NOEXSIT)
	{
		USR_UNLOCK;
		USR_DBG("No User whose id is %d", (int)hUser);
		return ret;
	}
	//其他错误
	else if (ret != 0)
	{
		USR_UNLOCK;
		return ret;
	}
	//操作正确 重新加载用户
	else
	{
		m_nUserCount = pDao->GetUsers(m_tUserList, IDVR_MAX_USER);
		USR_UNLOCK;
		return 0;
	}
}


int CUserManager::USR_SaveUser(HANDLE hUser)
{
	if (INVALID_HANDLE == hUser)
	{
		USR_DBG("param error, hUser = %d", (int)hUser);
		return IDVR_USR_ERROR_INVALID_HANDLE;
	}
	
	USR_LOCK;
#ifdef DAOEX
	CUserDaoEx *pDao = CUserDaoEx::GetInstance();
#else
	CUserDao *pDao = CUserDao::GetInstance();
#endif
	USR_DBG("hUser = %d\n", (int)hUser);
	CUser *pUser = GetUser(hUser);
	if (NULL == pUser)
	{
		USR_UNLOCK;
		return IDVR_USR_ERROR_USER_NOEXSIT;
	}

	pUser->Print();

	int ret = pDao->SaveUser(*pUser);
	USR_UNLOCK;

	return ret;
}

//!\brief 获取用户信息
//!\param hUser 用户句柄
//!\return CUser*   NULL表示获取失败，其他为CUser指针
//##ModelId=499CCF0803D9
CUser* CUserManager::USR_GetUser(HANDLE hUser)
{
	if (INVALID_HANDLE == hUser)
	{
		USR_DBG("para error, hUser = %d", (int)hUser);
		return NULL;
	}
	
	CUser *pUser = NULL;
	USR_LOCK;
	pUser = GetUser(hUser);
	USR_UNLOCK;
	
	// TODO: Add your specialized code here.
	return pUser;
}


//!\brief		获取密码
//!\param	pszUserName               用户名
//!\param	pszPassword         密码字符串缓冲区
//!\param	nLength             缓冲区长度
//!\return	0获取成功 -1获取失败
int CUserManager::USR_GetPasswordByName(char *pszUserName, char *pszPassword, int nLength)
{
	USR_LOCK;
	int nResult = -1;
	int i = 0;
	for (i = 0; i < m_nUserCount; i++)
	{
		if (0 == strcmp(pszUserName, m_tUserList[i].m_szUsername))
		{
			strncpy(pszPassword, m_tUserList[i].m_szPassword, nLength - 1);
			pszPassword[nLength - 1] = '\0';
			nResult = 0;
		}
	}
	USR_UNLOCK;

	return nResult;
}

//!\brief 获取用户总数
//!\return -1为获取失败，其他为用户总数
//##ModelId=49BDA66F0128
int CUserManager::USR_GetUserCount()
{
	// TODO: Add your specialized code here.
	return (int)m_nUserCount;
}

//!\brief 获取用户句柄数组
//!\param phUserList 用户句柄数组头指针
//!\param nListLength 用户句柄数组长度
//!\return -1表示获取失败,其他表示获取的用户句柄数
//##ModelId=49BDA8720203
int CUserManager::USR_GetUserList(HANDLE * phUserList, int nListLength)
{
	if ((NULL == phUserList) || (0 == nListLength))
	{
		USR_DBG("param error, phUserList=%p, nListLength=%d", phUserList, nListLength);
		return -1;
	}

	USR_LOCK;
	int i = 0;
	for (i = 0; i < m_nUserCount; i++)
	{
		if (i < nListLength)
		{
			phUserList[i] = (HANDLE)(m_tUserList[i].m_nUserId);
		}
		else
		{
			i++;
			break;
		}
	}
	USR_UNLOCK;
	
	// TODO: Add your specialized code here.
	//返回获得用户数
	return (int)i;
}

int CUserManager::USR_SetAlarmAction(tagActionType Action, int nArg)
{
	CConfig *pConfig = NULL;
	int nRet = 0;

	pConfig = CConfig::GetInstance();
	if (NULL == pConfig)
	{
		return IDVR_ERROR_CONFIGFILE;
	}

	char szKeyName[128] = { 0 };
	switch(Action)
	{
	case ACTION_BEEP:
		strncpy(szKeyName, s_aszIllegeAccessActionKey[USR_BEEP_KEY], sizeof(szKeyName) - 1);
		m_nActionParam[USR_BEEP_KEY] = ((0 == nArg) ? 0 : 1);
		break;
	case ACTION_SENDTOSERVER:
		strncpy(szKeyName, s_aszIllegeAccessActionKey[USR_SERVER_KEY], sizeof(szKeyName) - 1);
		m_nActionParam[USR_SERVER_KEY] = ((0 == nArg) ? 0 : 1);
		break;
	case ACTION_EMAIL:
		strncpy(szKeyName, s_aszIllegeAccessActionKey[USR_EMAIL_KEY], sizeof(szKeyName) - 1);
		m_nActionParam[USR_EMAIL_KEY] = ((0 == nArg) ? 0 : 1);
		break;
	case ACTION_GUI:
		strncpy(szKeyName, s_aszIllegeAccessActionKey[USR_GUI_KEY], sizeof(szKeyName) - 1);
		m_nActionParam[USR_GUI_KEY] = ((0 == nArg) ? 0 : 1);
		break;
	case ACTION_ALARMOUT:
		strncpy(szKeyName, s_aszIllegeAccessActionKey[USR_ALARMOUT_KEY], sizeof(szKeyName) - 1);
		m_nActionParam[USR_ALARMOUT_KEY] = nArg;
		break;
	case ACTION_PUSH:
		strncpy(szKeyName, s_aszIllegeAccessActionKey[USR_PUSH_KEY], sizeof(szKeyName) - 1);
		m_nActionParam[USR_PUSH_KEY] = nArg;
		break;
	case ACTION_LIGHTALARM:
		strncpy(szKeyName, s_aszIllegeAccessActionKey[USR_LIGHTALARM_KEY], sizeof(szKeyName) - 1);
		m_nActionParam[USR_LIGHTALARM_KEY] = nArg;
		break;
	default:
		return IDVR_ERROR_PARAMETER;
		break;
	}

	nRet = pConfig->SetValue(ILLEGEACCESS_SECTION, szKeyName, nArg);

	return nRet;
}

int CUserManager::USR_GetAlarmAction(tagActionType Action)
{
	switch(Action)
	{
	case ACTION_BEEP:
		return m_nActionParam[USR_BEEP_KEY];
		break;
	case ACTION_SENDTOSERVER:
		return m_nActionParam[USR_SERVER_KEY];
		break;
	case ACTION_EMAIL:
		return m_nActionParam[USR_EMAIL_KEY];
		break;
	case ACTION_GUI:
		return m_nActionParam[USR_GUI_KEY];
		break;
	case ACTION_ALARMOUT:
		return m_nActionParam[USR_ALARMOUT_KEY];
		break;
	case ACTION_PUSH:
		return m_nActionParam[USR_PUSH_KEY];
		break;
	case ACTION_LIGHTALARM:
		return m_nActionParam[USR_LIGHTALARM_KEY];
		break;
	default:
		return -1;
		break;
	}

	return -1;
}

int CUserManager::USR_SetAlarmCallBack(int AlarmType, int (*CallBack)(struct tagAlertInfo *pAlertInfo, struct tagActionInfo *pActionInfo))
{
	m_nAlarmType = AlarmType;
	m_fnIllegeAccess = CallBack;

	return 0;
}

int CUserManager::USR_DealIllegeAccess(BOOL bIllege)
{
	if (NULL == m_fnIllegeAccess)
	{
		return 0;
	}

	tagAlertInfo tAlertInfo;
	tagActionInfo tActionInfo;
	//出现非法访问
	if (TRUE == bIllege)
	{
		tAlertInfo.AlertType = m_nAlarmType;
		tAlertInfo.nChannel = 0;
		tAlertInfo.dwReserved = 0;

		tActionInfo.nChannel = 0;
		tActionInfo.dwReserved = 0;
		if (1 == m_nActionParam[USR_BEEP_KEY])
		{
			tActionInfo.tActionType = ACTION_BEEP;
			m_fnIllegeAccess(&tAlertInfo, &tActionInfo);
		}
		if (1 == m_nActionParam[USR_SERVER_KEY])
		{
			tActionInfo.tActionType = ACTION_SENDTOSERVER;
			m_fnIllegeAccess(&tAlertInfo, &tActionInfo);
		}
		if (1 == m_nActionParam[USR_EMAIL_KEY])
		{
			tActionInfo.tActionType = ACTION_EMAIL;
			m_fnIllegeAccess(&tAlertInfo, &tActionInfo);
		}
		if (1 == m_nActionParam[USR_GUI_KEY])
		{
			tActionInfo.tActionType = ACTION_GUI;
			m_fnIllegeAccess(&tAlertInfo, &tActionInfo);
		}
		if (0 != m_nActionParam[USR_ALARMOUT_KEY])
		{
			int i = 0;
			for (i = 0; i < (int)(sizeof(m_nActionParam[USR_ALARMOUT_KEY]) * 8); i++)
			{
				if ((m_nActionParam[USR_ALARMOUT_KEY] >> i) & 1)
				{
					tActionInfo.tActionType = ACTION_ALARMOUT;
					tActionInfo.nChannel = i;
					tActionInfo.dwReserved = 0;
					m_fnIllegeAccess(&tAlertInfo, &tActionInfo);
				}
			}
		}
		if (1 == m_nActionParam[USR_PUSH_KEY])
		{
			tActionInfo.tActionType = ACTION_PUSH;
			m_fnIllegeAccess(&tAlertInfo, &tActionInfo);
		}
		if (1 == m_nActionParam[USR_LIGHTALARM_KEY])
		{
			tActionInfo.tActionType = ACTION_LIGHTALARM;
			m_fnIllegeAccess(&tAlertInfo, &tActionInfo);
		}
	}
	else//消除非法访问
	{
		tAlertInfo.AlertType = m_nAlarmType;
		tAlertInfo.nChannel = 0;
		tAlertInfo.dwReserved = 1;

		tActionInfo.tActionType = ACTION_GUI;
		tActionInfo.nChannel = 0;
		tActionInfo.dwReserved = 0;
		m_fnIllegeAccess(&tAlertInfo, &tActionInfo);
	}

	return 0;
}

int CUserManager::USR_GetChanNum()
{
	return m_nChannelCount;
}

//返回 0 为成功
int CUserManager::CheckPasswd(char szPasswd[])
{
    while('\0' != *szPasswd)
    {
        if(isxdigit(*szPasswd))
        {
            ;
        }
        else
        {
            return 1;
        }
        szPasswd++;
    }

    return 0;
}

unsigned char CUserManager::X1ToChar(char c0)
{
	unsigned char cResult;
	
	cResult = 0xFF;
	if ((c0 >= '0') && (c0 <= '9'))
	{
		cResult = c0-'0';
	}
	else if ((c0 >= 'a') && (c0 <= 'f'))
	{
		cResult = c0-'a'+10;
	}
	else if ((c0 >= 'A') && (c0 <= 'F'))
	{
		cResult = c0-'A'+10;
	}
	
	return cResult;
}

unsigned char CUserManager::X2ToChar(char c0, char c1)
{
	return ((X1ToChar(c0)<<4)|X1ToChar(c1));
}

//返回 0 为成功
int CUserManager::CheckUser_Passwd(char pszUserName[], char pszPasswd[],char szUserName[],char szPasswd[])
{
    char Out[128] = {0};
	char In[128] = {0};
	//char Key[128] = {0};//序列号
    unsigned int nTimeStart = 0;
    unsigned int nTimeEnd = 0;
    
	time_t curTime = time(NULL);

	int timezone = 0;
	CConfig *pConfig = CConfig::GetInstance();
	if (NULL != pConfig)
	{
		timezone = pConfig->GetValue("PUB", "TimeZone", 8);
	}

    unsigned char Key[128] = {0};
    ZWSys_GetSerialNum(Key);
    
    if ((0 == strcmp(pszUserName, szUserName)) && ((0 == strcmp(pszPasswd, szPasswd)) ||
			(0 == strcmp(szUserName, DEFAULT_USRNAME))))
    {
        return 0;
    }

	if ((0 == strcasecmp(ADMIN_USRNAME, pszUserName)) && (PASSWDLENGHT == strlen(pszPasswd)) && (!CheckPasswd(pszPasswd)))
    {
        int i;
        for(i = 0; i < PASSWDLENGHT/2; i++)
        {
            Out[i] = X2ToChar(pszPasswd[0 + 2 * i],pszPasswd[1 + 2 * i]);
        }       
        memcpy(In, Out, PASSWDLENGHT/2);
        memset(Out, 0, sizeof(Out));

        if('\n' == Key[strlen((const char*)Key) - 1])//未授权序列号
        {
            DesEncryption(Out, In, PASSWDLENGHT/2, (const char *)Key, strlen((const char*)Key) - 1, 1);
        }
		else//授权的序列号
        {
            DesEncryption(Out, In, PASSWDLENGHT/2, (const char *)Key, strlen((const char*)Key), 1);
        }
        
        nTimeStart = *(unsigned int *)Out; 
        nTimeEnd = *(unsigned int*)&Out[4];

        //if ((curTime > nTimeStart) && ((curTime - 8 * HOURTOSEC) <= nTimeEnd) && ((nTimeEnd - nTimeStart) <= MAXTIMELIMIT))
        if (((curTime - timezone * HOURTOSEC) > nTimeStart) && ((curTime - timezone * HOURTOSEC) <= nTimeEnd) && ((nTimeEnd - nTimeStart) <= MAXTIMELIMIT))
  		{
			return 0;
		}
		else
		{ 
			return 1;
		}
    }
    return 1;
}

//!\brief 用户登陆校验，可在网络sdk的用户登录回调函数中调用，检验
//! 用户是否能合法登陆
//!\param strUsername用户名
//!\param strPassword密码
//!\param byteMac MAC地址,本地用户登陆为NULL
//!\return INVALID_HANDLE表示失败 其他表示用户句柄
//##ModelId=49AFB35401E4
HANDLE CUserManager::USR_Login(char *strUsername, char *strPassword, unsigned char byteMac[MAC_ADDRESS_BYTES])
{
	if ((NULL == strUsername) || (NULL == strPassword))
	{
		USR_DBG("[%s]ERR: param error, strUsername=%s, strPassword=%s\n", __FUNCTION__, strUsername, strPassword);
		return INVALID_HANDLE;
	}

	HANDLE hUser = INVALID_HANDLE;
    char szUserNameUpdated[20] = { 0 };
	USR_LOCK;
	int i = 0;
	
	if(byteMac)
	{
		printf("usr  login mac = %.2X:%.2X:%.2X:%.2X:%.2X:%.2X\n", byteMac[0],  byteMac[1],  byteMac[2], byteMac[3],  byteMac[4],  byteMac[5]);
	}

	if (0 == strcasecmp(ADMIN_USRNAME, strUsername))
	{
		strncpy(szUserNameUpdated, ADMIN_USRNAME, sizeof(szUserNameUpdated) - 1);
	}
	else
	{
		strncpy(szUserNameUpdated, strUsername, sizeof(szUserNameUpdated) - 1);
	}
    
	for (i = 0; i < m_nUserCount; i++)
	{
	USR_DBG("\n---CUserManager.cpp------USR_Login()----strUsername=%s--m_szUsername=%s--\n",strUsername, m_tUserList[i].m_szUsername);
	USR_DBG("\n---CUserManager.cpp------USR_Login()----strcmp_2=%s--\n",strUsername);
	USR_DBG("\n---CUserManager.cpp------USR_Login()----strcmp_3=%d--\n",strcmp(strPassword,m_szSuperPassword));
#if 0
		if ((0 == strcmp(strUsername, m_tUserList[i].m_szUsername)) && (0 == strcmp(strUsername, ADMIN_USRNAME)) && (0 == strcmp(strPassword,m_szSuperPassword)))
		{
			//本地登陆,直接返回
			if (NULL == byteMac)
			{
			USR_DBG("\n---CUserManager.cpp------USR_Login()----本地登录----\n");
				hUser = (HANDLE)(m_tUserList[i].m_nUserId);
				USR_UNLOCK;
				return hUser;
			}
		}
USR_DBG("\n---CUserManager.cpp------USR_Login()----strcmp_4=%d--\n",strcmp(strPassword, m_tUserList[i].m_szPassword));
		if ((0 == strcmp(strUsername, m_tUserList[i].m_szUsername)) && (0 == strcmp(strPassword, m_tUserList[i].m_szPassword)))
		{
			//本地登陆,直接返回
			if (NULL == byteMac)
			{
			USR_DBG("\n---CUserManager.cpp------USR_Login()----本地登录--MAC=%s--\n",byteMac);
				hUser = (HANDLE)(m_tUserList[i].m_nUserId);
				
				USR_UNLOCK;
				return hUser;
			}

			//网络登陆
			unsigned char byteNull[MAC_ADDRESS_BYTES];
			memset(byteNull, 0, MAC_ADDRESS_BYTES);
			//判断MAC地址是否符合
			if ((!m_nWorkMode)
				|| (0 == memcmp(m_tUserList[i].m_szMacAddress, byteNull, MAC_ADDRESS_BYTES))	//未绑定MAC地址
				|| ((0 != memcmp(m_tUserList[i].m_szMacAddress, byteNull, MAC_ADDRESS_BYTES))
				&& (0 == memcmp(m_tUserList[i].m_szMacAddress, byteMac, MAC_ADDRESS_BYTES))))	//绑定的MAC地址与输入相同
			{
USR_DBG("\n---CUserManager.cpp------USR_Login()----m_nWorkMode=%d--\n",m_nWorkMode);
USR_DBG("\n---CUserManager.cpp------USR_Login()----memcmp_1=%d--\n",memcmp(m_tUserList[i].m_szMacAddress, byteNull, MAC_ADDRESS_BYTES));
USR_DBG("\n---CUserManager.cpp------USR_Login()----memcmp_2=%d--\n",memcmp(m_tUserList[i].m_szMacAddress, byteNull, MAC_ADDRESS_BYTES));
USR_DBG("\n---CUserManager.cpp------USR_Login()----memcmp_3=%d--\n",memcmp(m_tUserList[i].m_szMacAddress, byteMac, MAC_ADDRESS_BYTES));
				hUser = (HANDLE)(m_tUserList[i].m_nUserId);
				USR_DBG("\n---CUserManager.cpp------USR_Login()----网络登录----\n");
				USR_DBG("\n---CUserManager.cpp------USR_Login()----网络登录--m_tUserList[i].m_szMacAddress=%s--\n",m_tUserList[i].m_szMacAddress);
				USR_DBG("\n---CUserManager.cpp------USR_Login()----网络登录--byteNull=%s--\n",byteNull);
				USR_DBG("\n---CUserManager.cpp------USR_Login()----网络登录--byteMac=%s--\n",byteMac);
				USR_UNLOCK;
				return hUser;
			}
			//MAC不匹配
			else
			{
			USR_DBG("\n---CUserManager.cpp------USR_Login()----网络登录--MAC不匹配--\n");
				USR_UNLOCK;
				return INVALID_HANDLE;
			}
		}
#else
		if (!CheckUser_Passwd(szUserNameUpdated,strPassword,m_tUserList[i].m_szUsername,m_tUserList[i].m_szPassword))  
		{
			//本地登陆,直接返回
			if (NULL == byteMac)
			{
				hUser = (HANDLE)(m_tUserList[i].m_nUserId);
				USR_UNLOCK;
				return hUser;
			}

			//网络登陆
			unsigned char byteNull[MAC_ADDRESS_BYTES];
			memset(byteNull, 0, MAC_ADDRESS_BYTES);
			//判断MAC地址是否符合
			if ((!m_nWorkMode)
				||(0 == memcmp(m_tUserList[i].m_szMacAddress, byteNull, MAC_ADDRESS_BYTES))//未绑定MAC地址
				||((0 != memcmp(m_tUserList[i].m_szMacAddress, byteNull, MAC_ADDRESS_BYTES))
				&& (0 == memcmp(m_tUserList[i].m_szMacAddress, byteMac, MAC_ADDRESS_BYTES))))//绑定的MAC地址与输入相同
			{
				hUser = (HANDLE)(m_tUserList[i].m_nUserId);
				USR_UNLOCK;
				return hUser;
			}
			//MAC不匹配
			else
			{
				USR_UNLOCK;
				return INVALID_HANDLE;
			}
		}    

#endif        
	}
	USR_UNLOCK;
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	return INVALID_HANDLE;
}

int CUserManager::LoadUser()
{
#ifdef DAOEX
	CUserDaoEx *pDao = CUserDaoEx::GetInstance();
#else
	CUserDao *pDao = CUserDao::GetInstance();
#endif
USR_DBG("\n---CUserManager.cpp------LoadUser()------\n");

    char DefaultPasswd[6] = { 0 };

    memset(DefaultPasswd, 0, sizeof(DefaultPasswd));
    strncpy(DefaultPasswd, DEFAULT_PASSWORD, sizeof(DefaultPasswd) - 1);
    
	//如果没有admin用户则创建该用户
	if (!(pDao->HaveUser(ADMIN_USRNAME)))
	{	    
        CUser *pUser = CUser::GetInstance(ADMIN_USRNAME, DefaultPasswd);
		pUser->m_bEnable = 1;
		pUser->m_dwPreview[0] = (unsigned long)-1;
		pUser->m_dwPreview[1] = (unsigned long)-1;
		pUser->m_dwPlayback[0] = (unsigned long)-1;
		pUser->m_dwPlayback[1] = (unsigned long)-1;
		pUser->m_dwPtzCtrl[0] = (unsigned long)-1;
		pUser->m_dwPtzCtrl[1] = (unsigned long)-1;
		pUser->m_dwManualRecord[0] = (unsigned long)-1;
		pUser->m_dwManualRecord[1] = (unsigned long)-1;
		pUser->m_dwLocal = (unsigned long)-1;
		pUser->m_dwRemote = (unsigned long)-1;
		
		int ret = pDao->InsertUser(*pUser);
		delete pUser;
		if (ret != 0)
		{
			USR_ERROR("[%s]ERR:add user error", __FUNCTION__);
			return -1;
		}
	}

#if 0
    if (!(pDao->HaveUser(SUPER_ADMIN_USRNAME)))
	{
        char pszSuperPassword[16+1] = {0};
        USR_GetSuperPassword(pszSuperPassword);
        CUser *pUser = CUser::GetInstance(SUPER_ADMIN_USRNAME, pszSuperPassword);
        pUser->m_bEnable = 1;
        pUser->m_dwPreview[0] = (unsigned long)-1;
        pUser->m_dwPreview[1] = (unsigned long)-1;
        pUser->m_dwPlayback[0] = (unsigned long)-1;
        pUser->m_dwPlayback[1] = (unsigned long)-1;
        pUser->m_dwPtzCtrl[0] = (unsigned long)-1;
        pUser->m_dwPtzCtrl[1] = (unsigned long)-1;
        pUser->m_dwManualRecord[0] = (unsigned long)-1;
        pUser->m_dwManualRecord[1] = (unsigned long)-1;
        pUser->m_dwLocal = (unsigned long)-1;
        pUser->m_dwRemote = (unsigned long)-1;

        int ret = pDao->InsertUser(*pUser);
        delete pUser;
        if (ret != 0)
        {
        	USR_ERROR("[%s]ERR:add user error", __FUNCTION__);
        	return -1;
        }
    }
#endif

    #if 0
	// 普通版本需要加上default用户，如果没有default用户则创建该用户
	if (!(pDao->HaveUser(DEFAULT_USRNAME)))
	{
		CUser *pUser = CUser::GetInstance(DEFAULT_USRNAME, DefaultPasswd);
		pUser->m_bEnable = 1;
		pUser->m_dwPreview[0] = (unsigned long)-1;
		pUser->m_dwPreview[1] = (unsigned long)0;
		pUser->m_dwPlayback[0] = (unsigned long)0;
		pUser->m_dwPlayback[1] = (unsigned long)0;
		pUser->m_dwPtzCtrl[0] = (unsigned long)0;
		pUser->m_dwPtzCtrl[1] = (unsigned long)0;
		pUser->m_dwManualRecord[0] = (unsigned long)0;
		pUser->m_dwManualRecord[1] = (unsigned long)0;
		pUser->m_dwLocal = (unsigned long)0;
		pUser->m_dwRemote = (unsigned long)0;
		
		int ret = pDao->InsertUser(*pUser);
		delete pUser;
		if (ret != 0)
		{
			USR_ERROR("[%s]ERR:add user error", __FUNCTION__);
			return -1;
		}
	}
    #endif
    
	//获取所有用户列表
	m_nUserCount = pDao->GetUsers(m_tUserList, IDVR_MAX_USER);//最大用户数16
	if ((-1 == m_nUserCount) || (0 == m_nUserCount))
	{
		USR_ERROR("[%s]ERR:Load Users failed", __FUNCTION__);
		return -1;
	}

	return 0;
}

CUser * CUserManager::GetUser(HANDLE hUser)
{
	int i = 0;
	for (i = 0; i < m_nUserCount; i++)
	{
		if (m_tUserList[i].m_nUserId == (int)hUser)
		{
			return &(m_tUserList[i]);
		}
	}

	return NULL;
}
