
/*!
 *\file	MPU_CIU_Config_User.cpp
 *\brief	MPU模块，远程配置，用户管理
 *\author	林云
 *\date	2009/8/17

功能：
-#	用户管理信息

说明：
-#	本文件使用新的网络库实现
 */

#include <netinet/in.h>
#include <arpa/inet.h>
#include <ctype.h>
#include "MPU_CIU_global.h"

#define USER_LEVEL_SAVE

extern int GetDevCustom_User(LPHY_DVR_WORKMODE pcfg);
extern int mysystem(const char * cmdstring);

//extern int SetDevCustom_User(LPHY_DVR_WORKMODE pcfg);

#if defined (__USE_STRONG_PW__)
static int CheckPasswordValid(char *pcPwd, int len)
{
    //8-16位，只能用数字、小写字母、大写字母，必须包含数字跟大写字母
    int bFindDigit = 0;
    int bFindUpper = 0;
    int bFindOther = 0;
    int i = 0;
    
    if (len < 8 || len > 16 || !pcPwd)
    {
        return -1;
    }

    for (i=0;i<len;i++)
    {
        if(isupper(pcPwd[i]) || (pcPwd[i] >= 'a' && pcPwd[i] <= 'z') || isdigit(pcPwd[i]))
        {
            if (isupper(pcPwd[i]))
            {
                bFindUpper = 1;
                continue;
            }

            if (isdigit(pcPwd[i]))
            {
                bFindDigit = 1;
                continue;
            }
        }
        else
        {
            bFindOther = 1;
        }
    }

    if (bFindUpper && bFindDigit && !bFindOther)
    {
        return 0;
    }
    
    return -1;
}
#endif

// 将用户名和对应的密码写入一个维护文本进行维护
int SaveUsrInfoIntoCFG(void)
{
    HANDLE hUserList[IDVR_MAX_USER] = {0};
    char buf[256] = {0};
    int i = 0;
    int nCount = 0;
    int nSDKRet = 0;
    FILE *fp = NULL;
	char szUserName[DVR_MAX_USERNAME_LEN + 1] = {0};
	char szPassword[DVR_MAX_USERNAME_LEN + 1] = {0};
	char szDesSuperPwd[16+1] = {0};

    fp = fopen(UserFile, "w+");
    if (NULL == fp)
    {
        perror("fopen");
        return -1;
    }
    
    //获取用户列表
    nCount = MPU_USR_GetUserList(hUserList, IDVR_MAX_USER);
    if (nCount > DVR_MAX_USER_NUM)
    {
        MPU_ERR("remote config user %d, real %d\n", DVR_MAX_USER_NUM, nCount);
    }

    for (i = 0; i < nCount; ++i)
    {
        //获取用户名szUserName
        nSDKRet = MPU_USR_GetUserName(hUserList[i], szUserName, DVR_MAX_USERNAME_LEN);
        MPU_CHK(0 == nSDKRet);

		if (0 == strncmp(szUserName, SUPER_ADMIN_USRNAME, sizeof(SUPER_ADMIN_USRNAME)))
		{
			//获取用户名szUserName对应的密码szUserName
			nSDKRet = MPU_USR_GetPasswordByName(szUserName, szPassword, DVR_MAX_PASSWORD_LEN);
			MPU_CHK(0 == nSDKRet);

			//printf("SaveUsrInfoIntoCFG start synthetic the super password!\n");
			MPU_USR_GetSuperPassword(szDesSuperPwd);

			if (16 != strlen(szPassword) || 0 != strcmp(szPassword, szDesSuperPwd))
			{
				nSDKRet = MPU_USR_SetPassword(hUserList[i], szDesSuperPwd);				
				MPU_CHK(0 == nSDKRet);

				MPU_USR_SaveUser(hUserList[i]);
			}

			break;
		}
    }    

    for (i = 0; i < nCount; ++i)
    {
        memset(buf, 0 , sizeof(buf));

        //获取用户名szUserName
        nSDKRet = MPU_USR_GetUserName(hUserList[i], szUserName, DVR_MAX_USERNAME_LEN);
        MPU_CHK(0 == nSDKRet);
        //获取用户名szUserName对应的密码szUserName
        nSDKRet = MPU_USR_GetPasswordByName(szUserName, szPassword, DVR_MAX_PASSWORD_LEN);
        MPU_CHK(0 == nSDKRet);
        /* 将用户名和密码和超级密码写入一个维护文本供boa使用 */
		snprintf(buf, sizeof(buf), "%s:%s\r\n", szUserName, szPassword);
		
        //printf("User Info: %s\n", buf);
        fwrite(buf, 1, strlen(buf), fp);   
        fflush(fp);
    }    
    fclose(fp);
    
    /* 给boa发送消息告知已经有文件修改了 */
    mysystem(SendMSGToBoa);

    return 0;
}

//获取单个用户信息，需外部将缓冲区清零
static int GetUserInfo(DWORD dwUserID, HANDLE hUser, ZW_USER_BASIC_CFG * pcfg)
{
	int nSDKRet;

	if (CIU_IsUser(dwUserID, ADMIN_USRNAME) || (dwUserID==VALUE_INVALUE) || CIU_IsUser(dwUserID, SUPER_ADMIN_USRNAME))
	{
		if (CIU_IsUser(hUser, ADMIN_USRNAME) || CIU_IsUser(hUser, SUPER_ADMIN_USRNAME))
		{
			pcfg->level = eZWUserLevel_Administrator;
		}
		else if (CIU_IsUser(hUser, DEFAULT_USRNAME))
		{
			pcfg->level = eZWUserLevel_DefaultUser;
		}
#ifdef USER_LEVEL_SAVE
		//email段未用到，暂用该段来表示用户的等级
		char email[256] = {0};
		nSDKRet = MPU_USR_GetEmailAddress(hUser, email, DVR_MAX_EMAIL_LEN);
		MPU_CHK(0 == nSDKRet);
		pcfg->level = (eZWUserLevel)email[0];
#else
		pcfg->level = eZWUserLevel_Normal;
#endif

		//管理员用户(后门-1)可以获取所有用户的信息
	}
	else if (CIU_IsUser(dwUserID, hUser))
	{
		pcfg->level = eZWUserLevel_Normal;
		//普通用户可以获取自己的信息
	}
	else if (CIU_IsUser(dwUserID, DEFAULT_USRNAME) || CIU_IsUser(dwUserID, SUPER_ADMIN_USRNAME))
	{
		pcfg->level = eZWUserLevel_DefaultUser;
		//MPU_ERR("can not get default user info\n");
		//缺省用户的信息没必要获取
		return IDVR_CIU_CALLSDK_FAIL;
	}
	else
	{
		//MPU_ERR("not admin or login user\n");
		return IDVR_CIU_NO_RIGHT;
	}


	//本结构体是否有效，=0表示无效
	pcfg->bValid = 1;

	/* 密码 */
	nSDKRet = MPU_USR_GetPassword(hUser, pcfg->szUserPassword, DVR_MAX_PASSWORD_LEN);
	MPU_CHK(0 == nSDKRet);

	nSDKRet = MPU_USR_GetBindMacAddress(hUser, (unsigned char *)pcfg->szBindMAC);
	MPU_CHK(0 == nSDKRet);

	/* 物理地址 */
	unsigned char NullMac[] = {0,0,0,0,0,0};
	if(0 != memcmp(&pcfg->szBindMAC[0], NullMac, 6))
	{
		pcfg->bBindMAC = 1;
	}
	else
		pcfg->bBindMAC = 0;
	//Email地址
	//nSDKRet = MPU_USR_GetEmailAddress(hUser, pcfg->szEmail, DVR_MAX_EMAIL_LEN);
	//MPU_CHK(0 == nSDKRet);

	return IDVR_OK;
}

//设置单个用户信息
static int SetUserInfo(DWORD dwUserID, HANDLE hUser, ZW_USER_BASIC_CFG* pcfg)
{
	int nSDKRet;
	char szTmpUserPassword[DVR_MAX_PASSWORD_LEN] = {0};
	
	if (CIU_IsUser(dwUserID, ADMIN_USRNAME) || (dwUserID==VALUE_INVALUE) || CIU_IsUser(dwUserID, SUPER_ADMIN_USRNAME))
	{			
		//管理员用户或者后台-1可以修改所有用户的信息
	}
	else if (CIU_IsUser(dwUserID, hUser))
	{
		//普通用户可以修改自己的信息
	}
	else if (CIU_IsUser(hUser, DEFAULT_USRNAME))
	{
		//缺省用户的信息不能修改
		MPU_ERR("can not change default user info\n");
		return IDVR_CIU_CALLSDK_FAIL;
	}
	else
	{
		MPU_ERR("not admin & login user\n");
		return IDVR_CIU_NO_RIGHT;
	}

	nSDKRet = MPU_USR_GetPassword(hUser, szTmpUserPassword, DVR_MAX_PASSWORD_LEN);
	//printf("nSDKRet=%d, szTmpUserPassword=%s, eUsage=%d\n", nSDKRet, szTmpUserPassword, pcfg->eUsage);
	MPU_CHK(0 == nSDKRet);
	if ((0 == strncmp(szTmpUserPassword, pcfg->szUserPassword, DVR_MAX_PASSWORD_LEN)) && pcfg->eUsage != ZW_USER_CREATE)
	{
		printf("The password is same at last time!\n");
		return IDVR_ERROR_SAME_PASSWORD;
	}
	else
	{
		printf("The password is different at last time!\n");
	}

	#if defined (__USE_STRONG_PW__)
	nSDKRet = CheckPasswordValid(pcfg->szUserPassword, strlen(pcfg->szUserPassword));
	if (nSDKRet != 0)
	{
		fprintf(stderr, "[%s][%d] ********zxr invalid passwd.[%s]*****\r\n",__FUNCTION__,__LINE__, pcfg->szUserPassword);
		return 6;//设备弱密码返回值，对应协议eErrorWeakPassword
	}
	#endif

	/* 密码 */
	nSDKRet = MPU_USR_SetPassword(hUser, pcfg->szUserPassword);
	MPU_CHK(0 == nSDKRet);
MPU_DBG("--SetUserInfo--bBindMAC=%d--szBindMAC=%s------\n",pcfg->bBindMAC,pcfg->szBindMAC);

    nSDKRet = SaveUsrInfoIntoCFG();
    MPU_CHK(0 == nSDKRet);

	/* 物理地址 */
	if(1 == pcfg->bBindMAC)
	{
		nSDKRet = MPU_USR_SetBindMacAddress(hUser, (unsigned char *)pcfg->szBindMAC);
		MPU_CHK(0 == nSDKRet);
	}
	else{
		nSDKRet = MPU_USR_SetBindMacAddress(hUser, (unsigned char *)NULL);
		MPU_CHK(0 == nSDKRet);
	}
	
	//Email地址
	//nSDKRet = MPU_USR_SetEmailAddress(hUser, pcfg->szEmail);
	//MPU_CHK(0 == nSDKRet);

	return IDVR_OK;
}

//获取单个用户权限，需外部将缓冲区清零
static int GetUserRight(DWORD dwUserID, HANDLE hUser, ZW_AUTHORITY* pcfg)
{
    int nCh, nCount;

    if (CIU_IsUser(dwUserID, ADMIN_USRNAME) || (dwUserID==VALUE_INVALUE) || CIU_IsUser(dwUserID, SUPER_ADMIN_USRNAME))
    {
        //管理员用户或者后台-1，可以获取所有用户权限
    }
    else if (CIU_IsUser(dwUserID, hUser))
    {
        //普通用户可以获取自己的权限
    }
    else
    {
        MPU_ERR("not admin & login\n");
        return IDVR_CIU_NO_RIGHT;
    }

    //本结构体是否有效，=0表示无效
    pcfg->bValid = 1;

    /*数组0: 本地系统设置 */
    /*数组1: 文件管理 */
    /*数组2: 录像备份 */
    /*数组3: 系统关闭 */
    /*数组4: 邮件通知 */
    pcfg->dwLocalAuthorityMask |= MPU_USR_GetLocalPrivilege(hUser, LOCAL_SETTINGS);
    pcfg->dwLocalAuthorityMask |= (MPU_USR_GetLocalPrivilege(hUser, LOCAL_FILE) << 1);
    //pcfg->nLocalRight[2] = MPU_USR_GetLocalPrivilege(hUser, LOCAL_BACKUP);
    pcfg->dwLocalAuthorityMask |= (MPU_USR_GetLocalPrivilege(hUser, LOCAL_HALT) << 2);
    //pcfg->nLocalRight[4] = MPU_USR_GetLocalPrivilege(hUser, LOCAL_EMAIL);

    /*数组0: 系统设置 */
    /*数组1: 语音对讲 */
    /*数组2: 串口输出 */
    /*数组3: 报警输出 */
    /*数组4: 远程升级 */
    /*数组5: 关闭系统 */
    pcfg->dwRemoteAuthorityMask |= MPU_USR_GetRemotePrivilege(hUser, REMOTE_SETTINGS);
    pcfg->dwRemoteAuthorityMask |= MPU_USR_GetRemotePrivilege(hUser, REMOTE_VOICE)<<4;
    pcfg->dwRemoteAuthorityMask |= MPU_USR_GetRemotePrivilege(hUser, REMOTE_COM)<<3;
    pcfg->dwRemoteAuthorityMask |= MPU_USR_GetRemotePrivilege(hUser, REMOTE_ALARMOUT)<<5;
    pcfg->dwRemoteAuthorityMask |= MPU_USR_GetRemotePrivilege(hUser, REMOTE_UPDATE)<<1;
    pcfg->dwRemoteAuthorityMask |= MPU_USR_GetRemotePrivilege(hUser, REMOTE_HALT)<<2;

    //获取录像通道总数
    nCount = MPU_AVL_GetChNum();
    if (nCount > DVR_MAX_VIDEOIN_NUM)
    {
        MPU_ERR("remote config picture %d, real %d\n", DVR_MAX_VIDEOIN_NUM, nCount);
        nCount = DVR_MAX_VIDEOIN_NUM;
    }

    //获取各通道权限
    for (nCh = 0; nCh < nCount; ++nCh)
    {
        /*数组0: 本地回放 */
        /*数组1: 本地预览 */
        /*数组2: 本地云台控制 */
        /*数组3: 本地手动录像 */
        /*数组4: 远程回放 */
        /*数组5: 远程预览 */
        /*数组6: 远程云台控制 */
        /*数组7: 远程手动录像 */

        if (MPU_USR_GetChannelPrivilege(hUser, CHANNEL_PLAYBACK, nCh, FALSE))
        {
#if (MAX_CH_NUM == 1)	
            SET_BIT(pcfg->dwLocalPlaybackMask, nCh);
#elif (MAX_CH_NUM == 2)	
            SET_BIT(pcfg->dwLocalPlaybackMask, nCh);
            SET_BIT(pcfg->dwLocalPlaybackMask, 1);
#elif (MAX_CH_NUM == 3)
            SET_BIT(pcfg->dwLocalPlaybackMask, nCh);
            SET_BIT(pcfg->dwLocalPlaybackMask, 1);
            SET_BIT(pcfg->dwLocalPlaybackMask, 2);
#elif (MAX_CH_NUM == 4)
            SET_BIT(pcfg->dwLocalPlaybackMask, nCh);
            SET_BIT(pcfg->dwLocalPlaybackMask, 1);
            SET_BIT(pcfg->dwLocalPlaybackMask, 2);
            SET_BIT(pcfg->dwLocalPlaybackMask, 3);
#endif
        }

        if (MPU_USR_GetChannelPrivilege(hUser, CHANNEL_PREVIEW, nCh, FALSE))
        {
#if (MAX_CH_NUM == 1)	
            SET_BIT(pcfg->dwLocalPreviewMask, nCh);
#elif (MAX_CH_NUM == 2)	
            SET_BIT(pcfg->dwLocalPreviewMask, nCh);
            SET_BIT(pcfg->dwLocalPreviewMask, 1);
#elif (MAX_CH_NUM == 3)
            SET_BIT(pcfg->dwLocalPreviewMask, nCh);
            SET_BIT(pcfg->dwLocalPreviewMask, 1);
            SET_BIT(pcfg->dwLocalPreviewMask, 2);
#elif (MAX_CH_NUM == 4)
            SET_BIT(pcfg->dwLocalPreviewMask, nCh);
            SET_BIT(pcfg->dwLocalPreviewMask, 1);
            SET_BIT(pcfg->dwLocalPreviewMask, 2);
            SET_BIT(pcfg->dwLocalPreviewMask, 3);
#endif
        }

        if (MPU_USR_GetChannelPrivilege(hUser, CHANNEL_PTZCTRL, nCh, FALSE))
        {
#if (MAX_CH_NUM == 1)	
            SET_BIT(pcfg->dwLocalPTZMask, nCh);
#elif (MAX_CH_NUM == 2)	
            SET_BIT(pcfg->dwLocalPTZMask, nCh);
            SET_BIT(pcfg->dwLocalPTZMask, 1);
#elif (MAX_CH_NUM == 3)
            SET_BIT(pcfg->dwLocalPTZMask, nCh);
            SET_BIT(pcfg->dwLocalPTZMask, 1);
            SET_BIT(pcfg->dwLocalPTZMask, 2);
#elif (MAX_CH_NUM == 4)
            SET_BIT(pcfg->dwLocalPTZMask, nCh);
            SET_BIT(pcfg->dwLocalPTZMask, 1);
            SET_BIT(pcfg->dwLocalPTZMask, 2);
            SET_BIT(pcfg->dwLocalPTZMask, 3);
#endif
        }

        if (MPU_USR_GetChannelPrivilege(hUser, CHANNEL_MANUALRECORD, nCh, FALSE))
        {
#if (MAX_CH_NUM == 1)	
            SET_BIT(pcfg->dwLocalManualControlMask, nCh);
#elif (MAX_CH_NUM == 2)	
            SET_BIT(pcfg->dwLocalManualControlMask, nCh);
            SET_BIT(pcfg->dwLocalManualControlMask, 1);
#elif (MAX_CH_NUM == 3)
            SET_BIT(pcfg->dwLocalManualControlMask, nCh);
            SET_BIT(pcfg->dwLocalManualControlMask, 1);
            SET_BIT(pcfg->dwLocalManualControlMask, 2);
#elif (MAX_CH_NUM == 4)
            SET_BIT(pcfg->dwLocalManualControlMask, nCh);
            SET_BIT(pcfg->dwLocalManualControlMask, 1);
            SET_BIT(pcfg->dwLocalManualControlMask, 2);
            SET_BIT(pcfg->dwLocalManualControlMask, 3);
#endif
        }

        if (MPU_USR_GetChannelPrivilege(hUser, CHANNEL_PLAYBACK, nCh, TRUE))
        {
#if (MAX_CH_NUM == 1)	
            SET_BIT(pcfg->dwRemotePlaybackMask, nCh);
#elif (MAX_CH_NUM == 2)	
            SET_BIT(pcfg->dwRemotePlaybackMask, nCh);
            SET_BIT(pcfg->dwRemotePlaybackMask, 1);
#elif (MAX_CH_NUM == 3)
            SET_BIT(pcfg->dwRemotePlaybackMask, nCh);
            SET_BIT(pcfg->dwRemotePlaybackMask, 1);
            SET_BIT(pcfg->dwRemotePlaybackMask, 2);
#elif (MAX_CH_NUM == 4)
            SET_BIT(pcfg->dwRemotePlaybackMask, nCh);
            SET_BIT(pcfg->dwRemotePlaybackMask, 1);
            SET_BIT(pcfg->dwRemotePlaybackMask, 2);
            SET_BIT(pcfg->dwRemotePlaybackMask, 3);
#endif
        }

        if (MPU_USR_GetChannelPrivilege(hUser, CHANNEL_PREVIEW, nCh, TRUE))
        {
#if (MAX_CH_NUM == 1)	
            SET_BIT(pcfg->dwRemotePreviewMask, nCh);
#elif (MAX_CH_NUM == 2)	
            SET_BIT(pcfg->dwRemotePreviewMask, nCh);
            SET_BIT(pcfg->dwRemotePreviewMask, 1);
#elif (MAX_CH_NUM == 3)
            SET_BIT(pcfg->dwRemotePreviewMask, nCh);
            SET_BIT(pcfg->dwRemotePreviewMask, 1);
            SET_BIT(pcfg->dwRemotePreviewMask, 2);
#elif (MAX_CH_NUM == 4)
            SET_BIT(pcfg->dwRemotePreviewMask, nCh);
            SET_BIT(pcfg->dwRemotePreviewMask, 1);
            SET_BIT(pcfg->dwRemotePreviewMask, 2);
            SET_BIT(pcfg->dwRemotePreviewMask, 3);
#endif
        }

        if (MPU_USR_GetChannelPrivilege(hUser, CHANNEL_PTZCTRL, nCh, TRUE))
        {
#if (MAX_CH_NUM == 1)	
            SET_BIT(pcfg->dwRemotePTZMask, nCh);
#elif (MAX_CH_NUM == 2)	
            SET_BIT(pcfg->dwRemotePTZMask, nCh);
            SET_BIT(pcfg->dwRemotePTZMask, 1);
#elif (MAX_CH_NUM == 3)
            SET_BIT(pcfg->dwRemotePTZMask, nCh);
            SET_BIT(pcfg->dwRemotePTZMask, 1);
            SET_BIT(pcfg->dwRemotePTZMask, 2);
#elif (MAX_CH_NUM == 4)
            SET_BIT(pcfg->dwRemotePTZMask, nCh);
            SET_BIT(pcfg->dwRemotePTZMask, 1);
            SET_BIT(pcfg->dwRemotePTZMask, 2);
            SET_BIT(pcfg->dwRemotePTZMask, 3);
#endif
        }

        if (MPU_USR_GetChannelPrivilege(hUser, CHANNEL_MANUALRECORD, nCh, TRUE))
        {
#if (MAX_CH_NUM == 1)	
            SET_BIT(pcfg->dwRemoteManualControlMask, nCh);
#elif (MAX_CH_NUM == 2)	
            SET_BIT(pcfg->dwRemoteManualControlMask, nCh);
            SET_BIT(pcfg->dwRemoteManualControlMask, 1);
#elif (MAX_CH_NUM == 3)
            SET_BIT(pcfg->dwRemoteManualControlMask, nCh);
            SET_BIT(pcfg->dwRemoteManualControlMask, 1);
            SET_BIT(pcfg->dwRemoteManualControlMask, 2);
#elif (MAX_CH_NUM == 4)
            SET_BIT(pcfg->dwRemoteManualControlMask, nCh);
            SET_BIT(pcfg->dwRemoteManualControlMask, 1);
            SET_BIT(pcfg->dwRemoteManualControlMask, 2);
            SET_BIT(pcfg->dwRemoteManualControlMask, 3);
#endif
		}
	}
	MPU_DBG("--GetUserRight--dwLocalAuthorityMask=0X%x----------\n",pcfg->dwLocalAuthorityMask);
	MPU_DBG("--GetUserRight--dwRemoteAuthorityMask=0X%x----------\n",pcfg->dwRemoteAuthorityMask);	
	MPU_DBG("--GetUserRight--dwLocalPreviewMask=0X%x--dwLocalPlaybackMask=0X%x--dwLocalPTZMask=0X%x--dwLocalManualControlMask=0X%x----\n",
		pcfg->dwLocalPreviewMask,pcfg->dwLocalPlaybackMask,pcfg->dwLocalPTZMask,pcfg->dwLocalManualControlMask);
	MPU_DBG("--GetUserRight--dwRemotePreviewMask=0X%x--dwRemotePlaybackMask=0X%x--dwRemotePTZMask=0X%x--dwRemoteManualControlMask=0X%x----\n",
		pcfg->dwRemotePreviewMask,pcfg->dwRemotePlaybackMask,pcfg->dwRemotePTZMask,pcfg->dwRemoteManualControlMask);


	return IDVR_OK;
}

//设置单个用户权限
static int SetUserRight(DWORD dwUserID, HANDLE hUser, ZW_AUTHORITY * pcfg)
{
	int bEnable, nCh, nCount, nSDKRet;

	///////////////////////////////////////////////////////////////////////////////
	//本结构体是否有效，=0表示无效
	if (0 == pcfg->bValid)
	{
		MPU_DBG("0 == pcfg->bValid\n");
		return IDVR_CIU_STRUCT_UNVALID;
	}

	if (!CIU_IsUser(dwUserID, ADMIN_USRNAME) && (dwUserID!=VALUE_INVALUE) && !CIU_IsUser(dwUserID, SUPER_ADMIN_USRNAME))
	{
		//只有管理员用户可以设置所有用户权限
		MPU_ERR("not admin user\n");
		return IDVR_CIU_NO_RIGHT;
	}

	//管理员权限是不能修改的
	if (CIU_IsUser(hUser, ADMIN_USRNAME) || CIU_IsUser(hUser, SUPER_ADMIN_USRNAME))
	{
		MPU_ERR("can not change admin user or super admin user right\n");
		return IDVR_CIU_CALLSDK_FAIL;
	}

	/*数组0: 本地系统设置 */
	/*数组1: 文件管理 */
	/*数组2: 录像备份 */
	/*数组3: 系统关闭 */
	/*数组4: 邮件通知 */
	nSDKRet = MPU_USR_SetLocalPrivilege(hUser, (pcfg->dwLocalAuthorityMask & eZWLocalAuthority_Configure), 
					LOCAL_SETTINGS);
	MPU_CHK(0 == nSDKRet);
	nSDKRet = MPU_USR_SetLocalPrivilege(hUser, (pcfg->dwLocalAuthorityMask & eZWLocalAuthority_Advanced)>>1,
					LOCAL_FILE);
	MPU_CHK(0 == nSDKRet);

	//nSDKRet = MPU_USR_SetLocalPrivilege(hUser, pcfg->nLocalRight[2], LOCAL_BACKUP);
	//MPU_CHK(0 == nSDKRet);
	nSDKRet = MPU_USR_SetLocalPrivilege(hUser,(pcfg->dwLocalAuthorityMask & eZWLocalAuthority_PoweroffOrReboot)>>2, LOCAL_HALT);
	MPU_CHK(0 == nSDKRet);

	//nSDKRet = MPU_USR_SetLocalPrivilege(hUser, pcfg->nLocalRight[4], LOCAL_EMAIL);
	//MPU_CHK(0 == nSDKRet);
	MPU_DBG("--SetUserRight--dwLocalAuthorityMask=0X%x----------\n",pcfg->dwLocalAuthorityMask);
	MPU_DBG("--SetUserRight--dwRemoteAuthorityMask=0X%x----------\n",pcfg->dwRemoteAuthorityMask);	
	MPU_DBG("--SetUserRight--dwLocalPreviewMask=0X%x--dwLocalPlaybackMask=0X%x--dwLocalPTZMask=0X%x--dwLocalManualControlMask=0X%x----\n",
		pcfg->dwLocalPreviewMask,pcfg->dwLocalPlaybackMask,pcfg->dwLocalPTZMask,pcfg->dwLocalManualControlMask);
	MPU_DBG("--SetUserRight--dwRemotePreviewMask=0X%x--dwRemotePlaybackMask=0X%x--dwRemotePTZMask=0X%x--dwRemoteManualControlMask=0X%x----\n",
		pcfg->dwRemotePreviewMask,pcfg->dwRemotePlaybackMask,pcfg->dwRemotePTZMask,pcfg->dwRemoteManualControlMask);

	//缺省用户没有远程权限
	if (!CIU_IsUser(hUser, DEFAULT_USRNAME))
	{
		/*数组0: 系统设置 */
		/*数组1: 语音对讲 */
		/*数组2: 串口输出 */
		/*数组3: 报警输出 */
		/*数组4: 远程升级 */
		/*数组5: 关闭系统 */
		nSDKRet = MPU_USR_SetRemotePrivilege(hUser, pcfg->dwRemoteAuthorityMask & eZWRemoteAuthority_Configure, REMOTE_SETTINGS);
		MPU_CHK(0 == nSDKRet);
		nSDKRet = MPU_USR_SetRemotePrivilege(hUser,(pcfg->dwRemoteAuthorityMask & eZWRemoteAuthority_RealTalk)>>4, REMOTE_VOICE);
		MPU_CHK(0 == nSDKRet);
		nSDKRet = MPU_USR_SetRemotePrivilege(hUser, (pcfg->dwRemoteAuthorityMask & eZWRemoteAuthority_SerialPortOutput)>>3, REMOTE_COM);
		MPU_CHK(0 == nSDKRet);
		nSDKRet = MPU_USR_SetRemotePrivilege(hUser, (pcfg->dwRemoteAuthorityMask & eZWRemoteAuthority_Alarmout)>>5, REMOTE_ALARMOUT);
		MPU_CHK(0 == nSDKRet);
		nSDKRet = MPU_USR_SetRemotePrivilege(hUser, (pcfg->dwRemoteAuthorityMask & eZWRemoteAuthority_Advanced)>>1, REMOTE_UPDATE);
		MPU_CHK(0 == nSDKRet);
		nSDKRet = MPU_USR_SetRemotePrivilege(hUser, (pcfg->dwRemoteAuthorityMask & eZWRemoteAuthority_PoweroffOrReboot)>>2, REMOTE_HALT);
		MPU_CHK(0 == nSDKRet);
	}

	//获取录像通道总数
	nCount = MPU_AVL_GetChNum();
	if (nCount > DVR_MAX_VIDEOIN_NUM)
	{
		MPU_ERR("remote config picture %d, real %d\n", DVR_MAX_VIDEOIN_NUM, nCount);
		nCount = DVR_MAX_VIDEOIN_NUM;
	}

	//获取各通道权限
	for (nCh = 0; nCh < nCount; ++nCh)
	{
		/*数组0: 本地回放 */
		/*数组1: 本地预览 */
		/*数组2: 本地云台控制 */
		/*数组3: 本地手动录像 */
		/*数组4: 远程回放 */
		/*数组5: 远程预览 */
		/*数组6: 远程云台控制 */
		/*数组7: 远程手动录像 */

		bEnable = IS_SET_BIT(pcfg->dwLocalPlaybackMask, nCh) ? TRUE : FALSE;
		nSDKRet = MPU_USR_SetChannelPrivilege(hUser, bEnable, CHANNEL_PLAYBACK, nCh, FALSE);
		MPU_CHK(0 == nSDKRet);
		bEnable = IS_SET_BIT(pcfg->dwLocalPreviewMask, nCh) ? TRUE : FALSE;
		nSDKRet = MPU_USR_SetChannelPrivilege(hUser, bEnable, CHANNEL_PREVIEW, nCh, FALSE);
		MPU_CHK(0 == nSDKRet);
		bEnable = IS_SET_BIT(pcfg->dwLocalPTZMask, nCh) ? TRUE : FALSE;
		nSDKRet = MPU_USR_SetChannelPrivilege(hUser, bEnable, CHANNEL_PTZCTRL, nCh, FALSE);
		MPU_CHK(0 == nSDKRet);
		bEnable = IS_SET_BIT(pcfg->dwLocalManualControlMask, nCh) ? TRUE : FALSE;
		nSDKRet = MPU_USR_SetChannelPrivilege(hUser, bEnable, CHANNEL_MANUALRECORD, nCh, FALSE);
		MPU_CHK(0 == nSDKRet);

		//缺省用户没有远程权限
		if (!CIU_IsUser(hUser, DEFAULT_USRNAME))
		{
			bEnable = IS_SET_BIT(pcfg->dwRemotePlaybackMask, nCh) ? TRUE : FALSE;
			nSDKRet = MPU_USR_SetChannelPrivilege(hUser, bEnable, CHANNEL_PLAYBACK, nCh, TRUE);
			MPU_CHK(0 == nSDKRet);
			bEnable = IS_SET_BIT(pcfg->dwRemotePreviewMask, nCh) ? TRUE : FALSE;
			nSDKRet = MPU_USR_SetChannelPrivilege(hUser, bEnable, CHANNEL_PREVIEW, nCh, TRUE);
			MPU_CHK(0 == nSDKRet);
			bEnable = IS_SET_BIT(pcfg->dwRemotePTZMask, nCh) ? TRUE : FALSE;
			nSDKRet = MPU_USR_SetChannelPrivilege(hUser, bEnable, CHANNEL_PTZCTRL, nCh, TRUE);
			MPU_CHK(0 == nSDKRet);

			bEnable = IS_SET_BIT(pcfg->dwRemoteManualControlMask, nCh) ? TRUE : FALSE;
			nSDKRet = MPU_USR_SetChannelPrivilege(hUser, bEnable, CHANNEL_MANUALRECORD, nCh, TRUE);
			MPU_CHK(0 == nSDKRet);
		}

	}
	return IDVR_OK;
}

//获取单个用户属性，需外部将缓冲区清零
static int GetUser(DWORD dwUserID, HANDLE hUser, ZW_USER_BASIC_CFG * pcfg)
{
	int nSDKRet;

	//本结构体是否有效，=0表示无效
	pcfg->bValid = 1;

	/* 用户名 最大16位 */
	nSDKRet = MPU_USR_GetUserName(hUser, pcfg->szUserName, DVR_MAX_USERNAME_LEN);
	MPU_CHK(0 == nSDKRet);

    if (0 == strcmp(pcfg->szUserName, SUPER_ADMIN_USRNAME))
    {
        memset(pcfg, 0, sizeof(ZW_USER_BASIC_CFG));
        return IDVR_USR_ERROR_INVALID_USERNAME;
    }

	//获取用户使能信息
	//GetUserEnable(hUser, &pcfg->struUserEnable);

	//获取用户基本信息
	GetUserInfo(dwUserID, hUser, pcfg);

	//获取用户权限
	GetUserRight(dwUserID, hUser, &pcfg->struAuthority);

	return IDVR_OK;
}

//设置单个用户属性
static int SetUser(DWORD dwUserID, HANDLE hUser, ZW_USER_BASIC_CFG*  pcfg)
{
	int nSDKRet, bChange = FALSE;

	///////////////////////////////////////////////////////////////////////////////
	//本结构体是否有效，=0表示无效
	if (0 == pcfg->bValid)
	{
		MPU_DBG("0 == pcfg->bValid\n");
		return IDVR_CIU_STRUCT_UNVALID;
	}

	//设置用户基本信息
	nSDKRet = SetUserInfo(dwUserID, hUser, pcfg);
	if (IDVR_ERROR_SAME_PASSWORD == nSDKRet)
	{
		return IDVR_ERROR_SAME_PASSWORD;
	}
    else
    if (6 == nSDKRet)
    {
        return 6;
    }
    
	if (IDVR_OK == nSDKRet)
	{
		bChange = TRUE;
	}

	//设置用户权限
	nSDKRet = SetUserRight(dwUserID, hUser, &pcfg->struAuthority);

	if (IDVR_OK == nSDKRet)
	{
		bChange = TRUE;
	}

	//修改过用户信息
	if (bChange)
	{
		//保存用户信息
		MPU_USR_SaveUser(hUser);

        nSDKRet = SaveUsrInfoIntoCFG();
        MPU_CHK(0 == nSDKRet);

		//记录编辑用户日志
		CIU_OPL_AddLog(dwUserID, EDVR_EVENT_USER, EDVR_EVENT_USR_DETAIL_EDIT_USER, 0, 0);
	}

	//通知GUI用户权限变更
	CIU_NotifyRemoteControl(dwUserID, eRemoteSetUserRight, (unsigned long) hUser);

	return IDVR_OK;
}

//设置单个用户属性
static int CreateUser(DWORD dwUserID, ZW_USER_BASIC_CFG * pcfg)
{
	HANDLE hUser;
	int nSDKRet;
	
	///////////////////////////////////////////////////////////////////////////////
	//本结构体是否有效，=0表示无效
	if (0 == pcfg->bValid)
	{
		MPU_DBG("0 == pcfg->bValid\n");
		return IDVR_CIU_STRUCT_UNVALID;
	}

	//用户使能信息必须有效
	/*if (0 == pcfg->struUserEnable.bValid)
	{
		MPU_DBG("0 == pcfg->struUserEnable.bValid\n");
		return IDVR_CIU_STRUCT_UNVALID;
	}

	LPHY_DVR_USER_INFO pinfo = &pcfg->struUserInfo;

	//用户基本信息必须有效
	if (0 == pinfo->bValid)
	{
		MPU_ERR("user info not valid\n");
		return IDVR_CIU_STRUCT_UNVALID;
	}*/

	//判断用户权限
	if (CIU_IsUser(dwUserID, ADMIN_USRNAME) || (dwUserID==VALUE_INVALUE) || CIU_IsUser(dwUserID, SUPER_ADMIN_USRNAME))
	{
		//只有管理员用户和后台-1可以创建用户
	}
	else
	{
		MPU_ERR("not admin or super admin\n");
		return IDVR_CIU_NO_RIGHT;
	}

#if defined (__USE_STRONG_PW__)
	nSDKRet = CheckPasswordValid(pcfg->szUserPassword, strlen(pcfg->szUserPassword));
	if (nSDKRet != 0)
	{
		fprintf(stderr, "[%s][%d] ********zxr invalid passwd.[%s]*****\r\n",__FUNCTION__,__LINE__, pcfg->szUserPassword);
		return 6;//设备弱密码返回值，对应协议eErrorWeakPassword
	}
#endif

#ifdef USER_LEVEL_SAVE
//email段未用到，暂用该段来表示用户的等级
	char email[256] = {0};
	email[0] = pcfg->level;
	hUser = MPU_USR_AddUser(pcfg->szUserName, pcfg->szUserPassword, email,(unsigned char*) pcfg->szBindMAC, 1);
#else
	hUser = MPU_USR_AddUser(pcfg->szUserName, pcfg->szUserPassword, NULL,(unsigned char*) pcfg->szBindMAC, 1);
#endif

	if (INVALID_HANDLE != hUser)
	{	
        nSDKRet = SaveUsrInfoIntoCFG();
        MPU_CHK(0 == nSDKRet);
        
		//记录创建用户日志
		CIU_OPL_AddLog(dwUserID, EDVR_EVENT_USER, EDVR_EVENT_USR_DETAIL_ADD_USER, 0, 0);

		return IDVR_OK;
	}
	else
	{
		return IDVR_CIU_CALLSDK_FAIL;
	}
}

//删除单个用户属性(顺便将用户名和对应的密码写到一个维护文本中进行维护)
static int RemoveUser(DWORD dwUserID, ZW_USER_BASIC_CFG * pcfg)
{
	HANDLE hUser;
	int nSDKRet;

	///////////////////////////////////////////////////////////////////////////////
	//本结构体是否有效，=0表示无效
	if (0 == pcfg->bValid)
	{
		MPU_DBG("0 == pcfg->bValid\n");
		return IDVR_CIU_STRUCT_UNVALID;
	}

	if (!CIU_IsUser(dwUserID, ADMIN_USRNAME) && (dwUserID!=VALUE_INVALUE) && !CIU_IsUser(dwUserID, SUPER_ADMIN_USRNAME))
	{
		//只有管理员、超级管理员用户和后台用户-1可以删除用户
		MPU_ERR("not admin user or super admin user\n");
		return IDVR_CIU_NO_RIGHT;
	}

	//管理员及缺省用户不能删除
	if ((0 == strcmp(pcfg->szUserName, ADMIN_USRNAME)) || (0 == strcmp(pcfg->szUserName, DEFAULT_USRNAME)) || (0 == strcmp(pcfg->szUserName, SUPER_ADMIN_USRNAME)))
	{
		MPU_ERR("can not remove %s user\n", pcfg->szUserName);
		return IDVR_CIU_CALLSDK_FAIL;
	}

	//获取用户句柄
	hUser = USR_GetHandle(pcfg->szUserName);

	if (INVALID_HANDLE != hUser)
	{
		if (MPU_USR_GetLocalLoginUser() == hUser)
		{
			//本地登录用户不能删除
			MPU_ERR("remove %s fail, is local login\n", pcfg->szUserName);
			return IDVR_CIU_CALLSDK_FAIL;
		}
		else if (CIU_IsOnline(hUser))
		{
			//远程登录用户不能删除
			MPU_ERR("remove %s fail, is online\n", pcfg->szUserName);
			return IDVR_CIU_CALLSDK_FAIL;
		}

		nSDKRet = MPU_USR_RemoveUser(hUser);
		MPU_CHK(0 == nSDKRet);

		if (0 == nSDKRet)
		{
            nSDKRet = SaveUsrInfoIntoCFG();
            MPU_CHK(0 == nSDKRet);
		
			//记录删除用户日志
			CIU_OPL_AddLog(dwUserID, EDVR_EVENT_USER, EDVR_EVENT_USR_DETAIL_REMOVE_USER, 0, 0);

			//通知GUI用户删除
			CIU_NotifyRemoteControl(dwUserID, eRemoteRemoveUser, 0);
			return IDVR_OK;
		}
		else
		{
			MPU_ERR("remove %s fail\n", pcfg->szUserName);
			return IDVR_CIU_CALLSDK_FAIL;;
		}
	}
	else
	{
		MPU_ERR("%s is not exist\n", pcfg->szUserName);
		return IDVR_OK;
	}
}


static int GetOnlineUser(LPHY_DVR_ONLINEUSER_CFG pcfg)
{
	int nRet = IDVR_OK;

	REMOTECLIENTINFO atRemoteClient[LOGINUSER_COUNT];

	int nCount = MPU_CIU_GetClientInfo(atRemoteClient);

	memset(pcfg, 0, sizeof (HY_DVR_ONLINEUSER_CFG));

	if (0 == nCount)
	{
		return IDVR_OK;
	}

	struct tm tmval;
	struct in_addr inaddr;
	time_t LoginTimeTemp;
	int i = 0;

	pcfg->bValid = 1;
	for (i = 0; i < nCount; i++)
	{
		pcfg->struOnlineUser[i].bValid = 1;
		pcfg->struOnlineUser[i].bConnect = 1;
		pcfg->struOnlineUser[i].bRemote = 1;
		strncpy(pcfg->struOnlineUser[i].szUserName, atRemoteClient[i].szUserName, MAX_USERNAME_LEN + 1);

		inet_aton(atRemoteClient[i].szIPAddress, &inaddr);
		pcfg->struOnlineUser[i].dwIPAddress = (DWORD) ntohl(inaddr.s_addr);

		pcfg->struOnlineUser[i].dwProtocolID = (unsigned long) atRemoteClient[i].eProtocol;
		pcfg->struOnlineUser[i].dwUserID = atRemoteClient[i].dwUserID;

		HieServer_GetUserBitRate(atRemoteClient[i].dwUserID, pcfg->struOnlineUser[i].dwBitrate);
		LoginTimeTemp = atRemoteClient[i].dwLoginTime;
		localtime_r(& LoginTimeTemp, &tmval);
		pcfg->struOnlineUser[i].struLoginTime.nYear = tmval.tm_year + 1900;
		pcfg->struOnlineUser[i].struLoginTime.nMonth = tmval.tm_mon + 1;
		pcfg->struOnlineUser[i].struLoginTime.nDay = tmval.tm_mday;
		pcfg->struOnlineUser[i].struLoginTime.nHour = tmval.tm_hour;
		pcfg->struOnlineUser[i].struLoginTime.nMinute = tmval.tm_min;
		pcfg->struOnlineUser[i].struLoginTime.nSecond = tmval.tm_sec;
	}

	return nRet;
}

static int SetOnlineUserOne(const DWORD dwUserID, const LPHY_DVR_ONLINEUSER pcfg)
{
	int nRet = IDVR_OK;

	///////////////////////////////////////////////////////////////////////////////
	//本结构体是否有效，=0表示无效
	if (0 == pcfg->bValid)
	{
		MPU_DBG("0 == pcfg->bValid\n");
		return IDVR_CIU_STRUCT_UNVALID;
	}

	if (!pcfg->bConnect)
	{
		// 不能踢自己
		if (pcfg->dwUserID == dwUserID)
		{
			nRet = IDVR_CIU_NO_RIGHT;
		}
		else
		{
			MPU_CIU_DisconnectClient(pcfg->dwUserID);
		}
	}

	return nRet;
}

static int SetOnlineUser(const DWORD dwUserID, const LPHY_DVR_ONLINEUSER_CFG pcfg)
{
	int nRet = IDVR_OK;

	///////////////////////////////////////////////////////////////////////////////
	//本结构体是否有效，=0表示无效
	if (0 == pcfg->bValid)
	{
		MPU_DBG("0 == pcfg->bValid\n");
		return IDVR_CIU_STRUCT_UNVALID;
	}

	int i = 0;

	for (i = 0; i < DVR_MAX_ONLINEUSER_NUM; i++)
	{
		nRet = SetOnlineUserOne(dwUserID, &pcfg->struOnlineUser[i]);
	}

	return nRet;
}

static int GetWhiteIP(LPHY_DVR_WHITEIP pWhiteIP, const int nMaxIPCount)
{
	DWORD dwWhiteIP[MAX_WHITEIP_NUM];

	int nCount = 0;

	int i = 0;

	nCount = CIU_GetIPEnableList(dwWhiteIP);

	for (i = 0; i < nCount && i < nMaxIPCount; i++)
	{
		pWhiteIP[i].bValid = 1;
		pWhiteIP[i].dwWhiteIP = dwWhiteIP[i];
	}

	return IDVR_OK;
}

static int SetWhiteIP(const DWORD dwUserID, const LPHY_DVR_WHITEIP pWhiteIP, const int nMaxIPCount)
{
	DWORD dwWhiteIP[MAX_WHITEIP_NUM];

	int bIPEnable[MAX_WHITEIP_NUM];

	int nCount = 0;

	int i = 0;

	int nRet = IDVR_OK;

	memset(bIPEnable, 0, MAX_WHITEIP_NUM * sizeof (int));
	nCount = CIU_GetIPEnableList(dwWhiteIP);

	// 添加配置里面的IP到白名单中
	for (i = 0; i < nMaxIPCount && i < MAX_WHITEIP_NUM; i++)
	{
		if (0 == pWhiteIP[i].bValid)
		{
			continue;
		}

		int j = 0;

		int bUsed = 0;

		// 检查是否已经在白名单中
		for (j = 0; j < nCount; j++)
		{
			if (pWhiteIP[i].dwWhiteIP == dwWhiteIP[j])
			{
				bIPEnable[j] = 1;
				bUsed = 1;
				break;
			}
		}

		// 不在，则添加到其中
		if (!bUsed)
		{
			CIU_ChangeIPEnable(pWhiteIP[i].dwWhiteIP, 1);
		}
	}

	// 删除配置里面没有的IP
	for (i = 0; i < nCount; i++)
	{
		if (0 == bIPEnable[i])
		{
			CIU_ChangeIPEnable(dwWhiteIP[i], 0);
		}
	}

	return nRet;
}

int GetWhiteIPEnable(LPHY_DVR_WHITEIP_ENABLE pcfg)
{
	pcfg->bValid = 1;

	pcfg->bEnable = CIU_GetIsIPRightEnable();

	return IDVR_OK;
}

int SetWhiteIPEnable(LPHY_DVR_WHITEIP_ENABLE pcfg)
{
	int nRet = IDVR_OK;

	if (pcfg->bValid)
	{
		CIU_SetIsIPRightEnable(pcfg->bEnable);
	}

	return nRet;
}

//获取用户信息，需外部将缓冲区清零
static int GetUserCfg(DWORD dwUserID, LPHY_DVR_USER_CFG pcfg)
{
	int nRet = IDVR_OK, i, nCount;

	HANDLE hUserList[IDVR_MAX_USER];

	///////////////////////////////////////////////////////////////////////////////
	//本结构体是否有效，=0表示无效
	pcfg->bValid = 1;

	//高级用户管理模式，0为普通模式，1为高级模式
//	GetDevCustom_User(&pcfg->struWorkMode);

	//获取用户列表
	nCount = MPU_USR_GetUserList(hUserList, IDVR_MAX_USER);
	if (nCount > DVR_MAX_USER_NUM)
	{
		MPU_ERR("remote config user %d, real %d\n", DVR_MAX_USER_NUM, nCount);
		nCount = DVR_MAX_USER_NUM;
	}

	int j = 0;

#if 0
	if(-2 == dwUserID)
	{
		TutkGetAdminUserID(&dwUserID);
		MPU_DBG("Tutk get admin dwUserID=%d\n", dwUserID);
		for (i = 0; (i < nCount) && (j < DVR_MAX_USER_NUM); ++i)
		{
			if(CIU_IsUser(dwUserID,hUserList[i]))
			{
				GetUser(dwUserID, hUserList[i], &pcfg->struUserBasic[i]);
				MPU_DBG("user = %s\n", pcfg->struUserBasic[i].szUserName);
			}
			else
				memset(&pcfg->struUserBasic[i],0,sizeof(ZW_USER_BASIC_CFG));

			j++;
		}
		return nRet;
	}
#endif
	for (i = 0; (i < nCount) && (j < DVR_MAX_USER_NUM); ++i)
	{
		if (CIU_IsUser(dwUserID, ADMIN_USRNAME) || CIU_IsUser(dwUserID,hUserList[i]))
		{
			GetUser(dwUserID, hUserList[i], &pcfg->struUserBasic[i]);
		}
        else
        {
            memset(&pcfg->struUserBasic[i],0,sizeof(ZW_USER_BASIC_CFG));
        }

		j++;
	}
#if 0
	GetOnlineUser(&pcfg->struUserOnline);
	GetWhiteIPEnable(&pcfg->struWhiteIPEnable);
	GetWhiteIP(pcfg->struWhiteIP, DVR_MAX_WHITEIP_NUM);
#endif
	return nRet;
}

//设置用户信息(顺便将用户名和对应的密码写到一个维护文本中进行维护)
static int SetUserCfg(DWORD dwUserID, LPHY_DVR_USER_CFG pcfg)
{
	int nRet = IDVR_OK, i, j, nCount, nSDKRet;
	HANDLE hUserList[IDVR_MAX_USER];

	///////////////////////////////////////////////////////////////////////////////
	//本结构体是否有效，=0表示无效
	if (0 == pcfg->bValid)
	{
		MPU_DBG("0 == pcfg->bValid\n");
		return IDVR_CIU_STRUCT_UNVALID;
	}


	//高级用户管理模式，0为普通模式，1为高级模式
	//SetDevCustom_User(&pcfg->struWorkMode);


	//获取用户列表
	nCount = MPU_USR_GetUserList(hUserList, IDVR_MAX_USER);
	if (nCount > DVR_MAX_USER_NUM)
	{
		MPU_ERR("remote config user %d, real %d\n", DVR_MAX_USER_NUM, nCount);
	}

	for (i = 0; i < nCount; ++i)
	{
		char szUserName[DVR_MAX_USERNAME_LEN + 1] = {0};

		//获取用户名
		nSDKRet = MPU_USR_GetUserName(hUserList[i], szUserName, DVR_MAX_USERNAME_LEN);
		MPU_CHK(0 == nSDKRet);

        if (0 == strcmp(SUPER_ADMIN_USRNAME, szUserName))
        {
            printf("---super admin user not be set---\n");
            continue;
        }

		//查找用户句柄
		for (j = 0; j < DVR_MAX_USER_NUM; ++j)
		{
			ZW_USER_BASIC_CFG* pUser = &pcfg->struUserBasic[j];

			if (0 != pUser->bValid)
			{
				if (0 == strcmp(szUserName, pUser->szUserName))
				{
					nSDKRet = SetUser(dwUserID, hUserList[i], pUser);
					#if 0
					if (IDVR_ERROR_SAME_PASSWORD == nSDKRet)
					{
						return IDVR_ERROR_SAME_PASSWORD;
					}
					#endif
					break;
				}
			}
		}
	}

    nSDKRet = SaveUsrInfoIntoCFG();
    MPU_CHK(0 == nSDKRet);

	SetOnlineUser(dwUserID, &pcfg->struUserOnline);
	SetWhiteIPEnable(&pcfg->struWhiteIPEnable);
	SetWhiteIP(dwUserID, pcfg->struWhiteIP, DVR_MAX_WHITEIP_NUM);

	return nRet;
}

int CIU_GetConfigure_User(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen)
{
	int nRet = IDVR_OK;

	MPU_DBG("ID %d, Major %d, Minor %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType);

	switch (dwMinorType)
	{
			/*获取全体用户信息 */
		case USERCFG_ALL:
			{
				///////////////////////////////////////////////////////////////////////////////
				//判断buf空间是否足够
				if (*pdwBufLen < USER_CFG_LEN)
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", USER_CFG_LEN, *pdwBufLen);
					break;
				}

				LPHY_DVR_USER_CFG pcfg = (LPHY_DVR_USER_CFG) pBuffer;

				///////////////////////////////////////////////////////////////////////////////
				//初始化输出缓冲区
				memset(pcfg, 0, USER_CFG_LEN);
				*pdwBufLen = USER_CFG_LEN;

				nRet = GetUserCfg(dwUserID, pcfg);
			}
			break;

		case USERCFG_USER_ONLINE:
			{
				///////////////////////////////////////////////////////////////////////////////
				//判断buf空间是否足够
				if (*pdwBufLen < sizeof (HY_DVR_ONLINEUSER_CFG))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof (HY_DVR_ONLINEUSER_CFG), *pdwBufLen);
					break;
				}

				LPHY_DVR_ONLINEUSER_CFG pcfg = (LPHY_DVR_ONLINEUSER_CFG) pBuffer;

				///////////////////////////////////////////////////////////////////////////////
				//初始化输出缓冲区
				memset(pcfg, 0, USER_CFG_LEN);
				*pdwBufLen = sizeof (HY_DVR_ONLINEUSER_CFG);

				nRet = GetOnlineUser(pcfg);
			}
			break;

		case USERCFG_WHITE_IP_ENABLE:
			{
				///////////////////////////////////////////////////////////////////////////////
				//判断buf空间是否足够
				if (*pdwBufLen < sizeof (HY_DVR_WHITEIP_ENABLE))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof (HY_DVR_WHITEIP_ENABLE), *pdwBufLen);
					break;
				}

				LPHY_DVR_WHITEIP_ENABLE pcfg = (LPHY_DVR_WHITEIP_ENABLE) pBuffer;

				///////////////////////////////////////////////////////////////////////////////
				//初始化输出缓冲区
				memset(pcfg, 0, USER_CFG_LEN);
				*pdwBufLen = sizeof (HY_DVR_WHITEIP_ENABLE);

				nRet = GetWhiteIPEnable(pcfg);
			}
			break;

		case USERCFG_WHITE_IP:
			{
				///////////////////////////////////////////////////////////////////////////////
				//判断buf空间是否足够
				if (*pdwBufLen < sizeof (HY_DVR_WHITEIP) * DVR_MAX_WHITEIP_NUM)
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof (HY_DVR_WHITEIP) * DVR_MAX_WHITEIP_NUM, *pdwBufLen);
					break;
				}

				LPHY_DVR_WHITEIP pWhiteIP = (LPHY_DVR_WHITEIP) pBuffer;

				nRet = GetWhiteIP(pWhiteIP, DVR_MAX_WHITEIP_NUM);
				break;
			}

		default:
			{
				nRet = IDVR_CIU_UNSUPPORTED_MINORTYPE;
				MPU_ERR("unsupported dwMinorType = %lu\n", dwMinorType);
			}
			break;
	}

	MPU_DBG("ID %d, Major %d, Minor %d, return %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType, nRet);
	return nRet;
}

int CIU_SetConfigure_User(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, const BYTE * pBuffer, DWORD dwBufLen)
{
	int nRet = IDVR_OK;

	MPU_DBG("ID %d, Major %d, Minor %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType);

	switch (dwMinorType)
	{
			/*设置全体用户信息 */
		case USERCFG_ALL:
			{
				///////////////////////////////////////////////////////////////////////////////
				//判断buf空间是否足够
				if (dwBufLen < USER_CFG_LEN)
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", USER_CFG_LEN, dwBufLen);
					break;
				}

				LPHY_DVR_USER_CFG pcfg = (LPHY_DVR_USER_CFG) pBuffer;

				nRet = SetUserCfg(dwUserID, pcfg);
			}
			break;

		case USERCFG_ONE:
			{
				///////////////////////////////////////////////////////////////////////////////
				//判断buf空间是否足够
				if (dwBufLen < sizeof (ZW_USER_BASIC_CFG))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof (ZW_USER_BASIC_CFG), dwBufLen);
					break;
				}

				ZW_USER_BASIC_CFG *pcfg = (ZW_USER_BASIC_CFG*) pBuffer;

				HANDLE hUser = USR_GetHandle(pcfg->szUserName);

				if (INVALID_HANDLE != hUser)
				{
					nRet = SetUser(dwUserID, hUser, pcfg);
				}
				else
				{
					nRet = IDVR_CIU_CALLSDK_FAIL;
					MPU_ERR("%s is not exist\n", pcfg->szUserName);
				}
			}
			break;

		case USERCFG_CREATE:
			{
				///////////////////////////////////////////////////////////////////////////////
				//判断buf空间是否足够
				if (dwBufLen < sizeof (ZW_USER_BASIC_CFG))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof (ZW_USER_BASIC_CFG), dwBufLen);
					break;
				}

				ZW_USER_BASIC_CFG *pcfg = (ZW_USER_BASIC_CFG*) pBuffer;

				nRet = CreateUser(dwUserID, pcfg);
			}
			break;

		case USERCFG_REMOVE:
			{
				///////////////////////////////////////////////////////////////////////////////
				//判断buf空间是否足够
				if (dwBufLen < sizeof (ZW_USER_BASIC_CFG))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof (ZW_USER_BASIC_CFG), dwBufLen);
					break;
				}

				ZW_USER_BASIC_CFG *pcfg = (ZW_USER_BASIC_CFG*) pBuffer;

				nRet = RemoveUser(dwUserID, pcfg);
			}
			break;

		case USERCFG_USER_ONLINE:
			{
				///////////////////////////////////////////////////////////////////////////////
				//判断buf空间是否足够
				if (dwBufLen < sizeof (HY_DVR_ONLINEUSER_CFG))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof (HY_DVR_ONLINEUSER_CFG), dwBufLen);
					break;
				}

				if (!CIU_IsUser(dwUserID, ADMIN_USRNAME) && !CIU_IsUser(dwUserID, SUPER_ADMIN_USRNAME))
				{
					//只有管理员和超级管理员用户可以设置所有用户权限
					MPU_ERR("not admin or super admin user\n");
					return IDVR_CIU_NO_RIGHT;
				}

				LPHY_DVR_ONLINEUSER_CFG pcfg = (LPHY_DVR_ONLINEUSER_CFG) pBuffer;

				nRet = SetOnlineUser(dwUserID, pcfg);
			}
			break;
		case USERCFG_USER_ONLINE_ONE:
			{
				///////////////////////////////////////////////////////////////////////////////
				//判断buf空间是否足够
				if (dwBufLen < sizeof (HY_DVR_ONLINEUSER))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof (HY_DVR_ONLINEUSER), dwBufLen);
					break;
				}

				if (!CIU_IsUser(dwUserID, ADMIN_USRNAME) && !CIU_IsUser(dwUserID, SUPER_ADMIN_USRNAME))
				{
					//只有管理员用户可以设置所有用户权限
					MPU_ERR("not admin or super admin user\n");
					return IDVR_CIU_NO_RIGHT;
				}

				LPHY_DVR_ONLINEUSER pcfg = (LPHY_DVR_ONLINEUSER) pBuffer;

				nRet = SetOnlineUserOne(dwUserID, pcfg);
			}
			break;
		case USERCFG_WHITE_IP_ENABLE:
			{
				///////////////////////////////////////////////////////////////////////////////
				//判断buf空间是否足够
				if (dwBufLen < sizeof (HY_DVR_WHITEIP_ENABLE))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof (HY_DVR_WHITEIP_ENABLE), dwBufLen);
					break;
				}

				if (!CIU_IsUser(dwUserID, ADMIN_USRNAME) && !CIU_IsUser(dwUserID, SUPER_ADMIN_USRNAME))
				{
					//只有管理员用户可以设置所有用户权限
					MPU_ERR("not admin or super admin user\n");
					return IDVR_CIU_NO_RIGHT;
				}

				LPHY_DVR_WHITEIP_ENABLE pcfg = (LPHY_DVR_WHITEIP_ENABLE) pBuffer;

				nRet = SetWhiteIPEnable(pcfg);
			}
			break;
		case USERCFG_WHITE_IP:
			{
				///////////////////////////////////////////////////////////////////////////////
				//判断buf空间是否足够
				if (dwBufLen < sizeof (HY_DVR_WHITEIP) * DVR_MAX_WHITEIP_NUM)
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof (HY_DVR_WHITEIP) * DVR_MAX_WHITEIP_NUM, dwBufLen);
					break;
				}

				if (!CIU_IsUser(dwUserID, ADMIN_USRNAME) && !CIU_IsUser(dwUserID, SUPER_ADMIN_USRNAME))
				{
					//只有管理员用户可以设置白名单
					MPU_ERR("not admin or super admin user\n");
					return IDVR_CIU_NO_RIGHT;
				}

				LPHY_DVR_WHITEIP pWhiteIP = (LPHY_DVR_WHITEIP) pBuffer;

				nRet = SetWhiteIP(dwUserID, pWhiteIP, DVR_MAX_WHITEIP_NUM);
				break;
			}
		default:
			{
				nRet = IDVR_CIU_UNSUPPORTED_MINORTYPE;
				MPU_ERR("unsupported dwMinorType = %lu\n", dwMinorType);
			}
			break;
	}

	MPU_DBG("ID %d, Major %d, Minor %d, return %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType, nRet);
	return nRet;
}
