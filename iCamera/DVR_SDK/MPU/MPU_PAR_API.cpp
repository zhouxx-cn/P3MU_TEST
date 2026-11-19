
/*!
 *\file		MPU_PAR_API.cpp
 *\brief	MPU模块，参数管理相关接口实现
 *\author	林云
 *\date		2009/03/20

功能：
-#	保存参数
-#	恢复默认参数
-#	导入导出参数

说明：
-#	各模块参数配置由各子模块负责
-#	导入导出时，都由上层GUI通过BKD模块接口获取U盘挂载点后，检索目录，生成配置文件全路径名
 */


#include "MPU_global.h"
#include "common_pub.h"
/**************************************************************************
 *                                                       定义局部宏                                                            *
 **************************************************************************/

//调用配置管理器执行动作
#define MPU_PAR_CALL(x)															\
do																				\
{																				\
	int nRet;																		\
																				\
	CConfig *pConfig = CConfig::GetInstance();											\
																				\
	if (NULL != pConfig)															\
	{																			\
		nRet = pConfig->x;															\
		if (IDVR_OK != nRet)														\
		{																		\
			MPU_ERR("%s, nRet = %d\n", #x, nRet);									\
		}																		\
	}																			\
	else																			\
	{																			\
		MPU_ERR("Get CConfig instance fail!\n");										\
		MPU_CHK(NULL != pConfig);													\
		nRet = IDVR_MPU_NO_CCONFIG_INSTANCE;										\
	}																			\
																				\
	return nRet;																	\
}																				\
while (0)


/**************************************************************************
 *                     模块内部接口                                       *
 **************************************************************************/
extern "C"	int mysystem(const char* cmdstring);
	
//!\brief   初始化参数管理器
//!\return  0表示成功，其它为错误码
int MPU_PAR_Init()
{
	int nRet = -1;
	FILE *pp = NULL;

	BOOL bNeedSave = FALSE;
	BOOL bDefaultError = FALSE;
	int onetime = 1;
	char buf[256] = {0};
	char cfgbuf[20] = {0};
	int  cfgvalue = 0;
	int busrdefaultflag = FALSE;
	char fileName[64] = {0};
	int i = 0;
startCheck:
	//导入当前配置文件
	nRet = MPU_PAR_LoadConfig(IDVR_CURRENT_CONFIG_FILE_PATH);

	//导入备份配置文件
	if (IDVR_OK != nRet)
	{
		bNeedSave = TRUE;
		nRet = MPU_PAR_LoadConfig(IDVR_BACKUP_CONFIG_FILE_PATH);
		//删除Edvr.cfg
		unlink(IDVR_CURRENT_CONFIG_FILE_PATH);

	}

	//导入备份配置文件
	if (IDVR_OK != nRet)
	{
		busrdefaultflag = TRUE;
		nRet = MPU_PAR_LoadConfig(IDVR_DEFAULT_CONFIG_FILE_PATH);
		unlink(IDVR_BACKUP_CONFIG_FILE_PATH);
	}

	if (IDVR_OK != nRet)
	{
		bDefaultError = TRUE;
		unlink(IDVR_DEFAULT_CONFIG_FILE_PATH);
	}

	//boa配置不存在或者boa没起来
	if ((access(BOA_CONF_FILE_PATH, 0)) || (!IsStringInCommand("ps", "boa")))
	{
		memset(buf, 0, sizeof(buf));
		snprintf(buf, sizeof(buf), "mkdir -p %s", BOA_FILE_PATH);
		mysystem(buf);

		memset(buf, 0, sizeof(buf));
		snprintf(buf, sizeof(buf), "cp %s %s", BOA_APP_BAK_FILE_PATH, BOA_CONF_FILE_PATH);
		mysystem(buf);

		memset(buf, 0, sizeof(buf));
		snprintf(buf, sizeof(buf), "cp %s %s", BOA_APP_BAK_FILE_PATH, BOA_CONF_BAK_FILE_PATH);
		mysystem(buf);

		memset(buf, 0, sizeof(buf));
		snprintf(buf, sizeof(buf), "cp %s %s", "/mnt/app/data/config/etc/conf.d/boa/mime.types", "/mnt/mtd/etc/conf.d/boa/mime.types");
		mysystem(buf);

		memset(buf, 0, sizeof(buf));
		snprintf(buf, sizeof(buf), "cp %s %s", UserFile_BAK, "/mnt/mtd/etc/conf.d/boa/userlist.conf");
		mysystem(buf);
		printf("boa config file is broken! has been recover! will reboot boa!\n");
		sleep(5);
        
		if (bNeedSave)
		{
			MPU_PAR_SaveConfigReal();
		}
		
		PAR_ForbiddenSaveFile();
        sleep(2);
		mysystem("reboot");
	}

	if (bDefaultError)
	{
		memset(buf, 0, sizeof(buf));
		snprintf(buf, sizeof(buf), "mkdir -p %s", "/mnt/mtd/config");
		mysystem(buf);

		if(onetime)
		{
			onetime = 0;
			memset(buf, 0, sizeof(buf));
			snprintf(buf, sizeof(buf), "cp %s %s", IDVR_DEFAULT_CONFIG_2_FILE_PATH, IDVR_DEFAULT_CONFIG_FILE_PATH);
			mysystem(buf);
			memset(buf, 0, sizeof(buf));
			snprintf(buf, sizeof(buf), "cp %s %s", IDVR_DEFAULT_CONFIG_2_FILE_PATH, IDVR_CURRENT_CONFIG_FILE_PATH);
			mysystem(buf);
			memset(buf, 0, sizeof(buf));
			snprintf(buf, sizeof(buf), "cp %s %s", IDVR_DEFAULT_CONFIG_2_FILE_PATH, IDVR_BACKUP_CONFIG_FILE_PATH);
			mysystem(buf);
			goto startCheck;
		}
	}

	//除Edvr.cfg外有一个配置文件正确
	if (bNeedSave)
	{

		//保存为Edvr.cfg
		MPU_PAR_SaveConfigAs(IDVR_CURRENT_CONFIG_FILE_PATH);
		nRet = MPU_PAR_LoadConfig(IDVR_CURRENT_CONFIG_FILE_PATH);

		if (IDVR_PAR_ERROR_EMPTY_FILE == nRet)
		{
			MPU_PAR_SaveConfig();
			nRet = 0;
		}
		if(busrdefaultflag)
		{
		    //获取新的配置文件读取实例
		    CConfig *pCfg = CConfig::GetInstance();
		    if(pCfg == NULL)
		    	{
					goto error_out;
		    	}
			
			memset(buf, 0, sizeof(buf));
			memset(cfgbuf, 0, sizeof(cfgbuf));
			bzero(fileName, sizeof(fileName));
			snprintf(fileName, sizeof(fileName), "/tmp/.MPU_PAR_Init%d", i++);
		    snprintf(buf, sizeof(buf), "ls /mnt/mtd/config/.Ip* | awk -F \":\" '{printf $2}' > %s", fileName);
			mysystem(buf);
			if ((pp = fopen(fileName, "r")) == NULL)
			{
				printf("fopen %s fail!\n", fileName);
			}
			else
				{
			fgets(cfgbuf, sizeof(cfgbuf), pp);
			cfgbuf[strlen(cfgbuf)] = '\0';
					fclose(pp);
				}
			bzero(buf, sizeof(buf));
			snprintf(buf, sizeof(buf), "rm -rf %s", fileName);
			mysystem(buf);
			if(strncmp(cfgbuf, "", strlen(cfgbuf)))
			{
			    if (0 == nRet)
			    {
			        pCfg->SetValue(INet_CfgSection, INet_IP_key, cfgbuf);
			    }	 
			}

			memset(buf, 0, sizeof(buf));
			memset(cfgbuf, 0, sizeof(cfgbuf));
			bzero(fileName, sizeof(fileName));
			snprintf(fileName, sizeof(fileName), "/tmp/.MPU_PAR_Init%d", i++);
		    snprintf(buf, sizeof(buf), "ls /mnt/mtd/config/.GATEWAY* | awk -F \":\" '{printf $2}' > %s", fileName);
			mysystem(buf);
			if ((pp = fopen(fileName, "r")) == NULL)
			{
				printf("fopen %s fail!\n", fileName);
			}
			else
				{
			fgets(cfgbuf, sizeof(cfgbuf), pp);
			cfgbuf[strlen(cfgbuf)] = '\0';
					fclose(pp);
				}
			bzero(buf, sizeof(buf));
			snprintf(buf, sizeof(buf), "rm -rf %s", fileName);
			mysystem(buf);
			if(strncmp(cfgbuf, "", strlen(cfgbuf)))
			{
			    if (0 == nRet)
			    {
			        pCfg->SetValue(INet_CfgSection, INet_Gateway_key, cfgbuf);
			    }	 
			}

			memset(buf, 0, sizeof(buf));
			memset(cfgbuf, 0, sizeof(cfgbuf));
			bzero(fileName, sizeof(fileName));
			snprintf(fileName, sizeof(fileName), "/tmp/.MPU_PAR_Init%d", i++);
		    snprintf(buf, sizeof(buf), "ls /mnt/mtd/config/.MASK* | awk -F \":\" '{printf $2}' > %s", fileName);
			mysystem(buf);
			if ((pp = fopen(fileName, "r")) == NULL)
			{
				printf("fopen %s fail!\n", fileName);
			}
			else
			{
			fgets(cfgbuf, sizeof(cfgbuf), pp);
			cfgbuf[strlen(cfgbuf)] = '\0';
				fclose(pp);
			}
			bzero(buf, sizeof(buf));
			snprintf(buf, sizeof(buf), "rm -rf %s", fileName);
			mysystem(buf);

			if(strncmp(cfgbuf, "", strlen(cfgbuf)))
			{
				if (0 == nRet)
				{
					pCfg->SetValue(INet_CfgSection, INet_SubnetMask_key, cfgbuf);
				}	 
			}
			
			if (access("/mnt/mtd/config/.DHCP:0", F_OK)==0)
				{
					memset(buf, 0, sizeof(buf));
					memset(cfgbuf, 0, sizeof(cfgbuf));
					bzero(fileName, sizeof(fileName));
					snprintf(fileName, sizeof(fileName), "/tmp/.MPU_PAR_Init%d", i++);
				    snprintf(buf, sizeof(buf), "ls /mnt/mtd/config/.DHCP* | awk -F \":\" '{printf $2}' > %s", fileName);
					mysystem(buf);
					if ((pp = fopen(fileName, "r")) == NULL)
					{
						printf("fopen %s fail!\n", fileName);
					}
					else
						{
					fgets(cfgbuf, sizeof(char), pp);
							fclose(pp);
						}
					bzero(buf, sizeof(buf));
					snprintf(buf, sizeof(buf), "rm -rf %s", fileName);
					mysystem(buf);

					cfgvalue = atoi(cfgbuf);
				    if (0 == nRet)
				    {
				        pCfg->SetValue(INet_CfgSection, INet_DHCP_key, cfgvalue);
				    }				
				}

			if (access("/mnt/mtd/config/.IPADJUST:0", F_OK)==0)
				{
					memset(buf, 0, sizeof(buf));
					memset(cfgbuf, 0, sizeof(cfgbuf));
					bzero(fileName, sizeof(fileName));
					snprintf(fileName, sizeof(fileName), "/tmp/.MPU_PAR_Init%d", i++);
				    snprintf(buf, sizeof(buf), "ls /mnt/mtd/config/.IPADJUST* | awk -F \":\" '{printf $2}' > %s", fileName);
					mysystem(buf);
					if ((pp = fopen(fileName, "r")) == NULL)
					{
						printf("fopen %s fail!\n", fileName);
					}
					else
						{
					fgets(cfgbuf, sizeof(char), pp);
							fclose(pp);
						}
					bzero(buf, sizeof(buf));
					snprintf(buf, sizeof(buf), "rm -rf %s", fileName);
					mysystem(buf);
					
					cfgvalue = atoi(cfgbuf);
				    if (0 == nRet)
				    {
				        pCfg->SetValue(INet_CfgSection, INet_IPAdjust_key, cfgvalue);
				    }						
				}

			MPU_PAR_SaveConfig();
		}
	}

	/* 1. Edvr.default ------> Edvr.cfg/Edvr.cfg-bak
     * 2. Edvr.default error
     * 以上两种情况需要恢复Device.cfg
     */
    if (busrdefaultflag || bDefaultError)
    {
        busrdefaultflag = FALSE;
        bDefaultError   = FALSE;
		printf("some cfg has broken! need restore device config!\n");
		//重新把定制配置替换
		MPU_PAR_CustomFileDate_Replace(CUSTOM_CONFIGMODE_FILE, 1);
        goto startCheck;
    }    
	
error_out:
	return nRet;
}

//!\brief   逆初始化参数管理器
//!\return  0表示成功，其它为错误码
int PAR_Uninit()
{
#if 0
	//保存配置
	MPU_PAR_SaveConfig();
#endif
	//释放空间
	MPU_PAR_CALL(UnloadFile());
}

int PAR_ForbiddenSaveFile()
{
	MPU_PAR_CALL(ForbiddenSaveFile());
}

int PAR_UnForbiddenSaveFile()
{
	MPU_PAR_CALL(UnForbiddenSaveFile());
}

void PAR_NET_Resume(const char *func)
{	
    char command[256] = {0};
	char subkey_conf[64] = {0};
    int counter = 0;

    printf("--------- Restore PAR_NET_Resume default factory over -%s- --------\n", func);

    /* 需要恢复的文件: Edvr.cfg、Edvr.cfg-bak、boa.conf、boa.conf-bak */
	snprintf(command, sizeof(command), "cp -v %s %s", IDVR_DEFAULT_CONFIG_FILE_PATH, IDVR_CURRENT_CONFIG_FILE_PATH);
	mysystem(command); // Edvr.cfg
	
	memset(command, 0, sizeof(command));
	snprintf(command, sizeof(command), "cp -v %s %s", IDVR_DEFAULT_CONFIG_FILE_PATH, IDVR_BACKUP_CONFIG_FILE_PATH);
	mysystem(command); // Edvr.cfg-bak

	memset(command, 0, sizeof(command));
	snprintf(command, sizeof(command), "cp -v %s %s", BOA_APP_BAK_FILE_PATH, BOA_CONF_FILE_PATH);
	mysystem(command); // boa.conf

	memset(command, 0, sizeof(command));
	snprintf(command, sizeof(command), "cp -v %s %s", BOA_APP_BAK_FILE_PATH, BOA_CONF_BAK_FILE_PATH);
	mysystem(command); // boa.conf-bak

    /* 需要删除的文件: tutkuid.conf、resolv.conf、resolv_wlan.conf、usr.db、log.db、userlist.conf */
#if 0//恢复出厂时不去删除DID的配置文件
    memset(command, 0, sizeof(command));
	snprintf(command, sizeof(command), "rm -v %s", TUTK_UID_FILE_PATH);
	mysystem(command); // tutkuid.conf
#endif

	memset(command, 0, sizeof(command));
	snprintf(command, sizeof(command), "cp -v %s %s", RESOLV_CONF_BAK_FILE_PATH, RESOLV_CONF_FILE_PATH_REAL);
	mysystem(command); // resolv.conf(需要清空)

	memset(command, 0, sizeof(command));
	snprintf(command, sizeof(command), "cp -v %s %s", RESOLV_WLAN_CONF_BAK_FILE_PATH, RESOLV_WLAN_CONF_FILE_PATH);
	mysystem(command); // resolv_wlan.conf(需要清空)

	memset(command, 0, sizeof(command));
	snprintf(command,sizeof(command), "cp -v %s %s", RESOLV_ETH0_CONF_BAK_FILE_PATH, RESOLV_ETH0_CONF_FILE_PATH);
	mysystem(command); // resolv_eth0.conf(需要清空)

	memset(command, 0, sizeof(command));
	snprintf(command, sizeof(command), "rm -v %s", USR_DB_FILE_PATH);
	mysystem(command); // usr.db

	memset(command, 0, sizeof(command));
	snprintf(command, sizeof(command), "rm -v %s", USR_DB_FILE_PATH_TXT);
	mysystem(command); // usr.txt
    //新建定制配置文件的标记，使定制的密码等生效
    memset(command, 0, sizeof(command));
    snprintf(command, sizeof(command), "touch %s", CUSTOM_MODE_FLAG);
    mysystem(command);
    
	memset(command, 0, sizeof(command));
	snprintf(command, sizeof(command), "rm -v %s", LOG_DB_FILE_PATH); 
	mysystem(command); // log.db

	memset(command, 0, sizeof(command));
	snprintf(command, sizeof(command), "cp -v %s %s", UserFile_BAK, UserFile);
	mysystem(command); // userlist.conf

 	memset(command, 0, sizeof(command));
	snprintf(command, sizeof(command), "rm -v  /mnt/mtd/config/AfInfo.default");
	mysystem(command); // AfInfo.default
	
	memset(command, 0, sizeof(command));
	snprintf(command, sizeof(command), "rm -v  %s %s",CLOUD_STORAGE_MODE_DOMESTIC_FLAG,CLOUD_STORAGE_MODE_FOREIGN_FLAG);
	mysystem(command); // CloudMode0,CloudMode1
	
	memset(command, 0, sizeof(command));
	snprintf(command, sizeof(command), "rm -v  /data/config/config/noSupportShare");
	mysystem(command);

    if (0 == access(SPEECH_MOTION_ALARM_CUSTOM_FILE, 0))
	{
		memset(command, 0, sizeof(command));
		snprintf(command, sizeof(command), "rm -v %s", SPEECH_MOTION_ALARM_CUSTOM_FILE);
		mysystem(command);
	}

#if defined (__IS_SUPPORT_REMINDER_SCHEDULE__)
    SAFE_UNLINK(SPEECH_REMIND_SCHEDULE_CUSTOM_FILE1);
    SAFE_UNLINK(SPEECH_REMIND_SCHEDULE_CUSTOM_FILE2);
    SAFE_UNLINK(SPEECH_REMIND_SCHEDULE_CUSTOM_FILE3);
#endif

#if defined(__IS_SUPPORT_LCD_MANAGER__)
    SAFE_UNLINK(LCD_USR_BACKGROUND_IMAGE_PATH);
#endif


	mysystem("rm /mnt/mtd/config/.Ip*");
	mysystem("rm /mnt/mtd/config/.GATEWAY*");
	mysystem("rm /mnt/mtd/config/.MASK*");
	mysystem("rm /mnt/mtd/config/.DHCP*");
	mysystem("rm /mnt/mtd/config/.IPADJUST*");
	mysystem("rm /mnt/mtd/config/.OSD");
#if defined (__IS_SUPPORT_WLAN__)
	mysystem("rm /mnt/mtd/config/.GateWay*");
	mysystem("rm /mnt/mtd/config/.WlanIp* ");
	mysystem("rm /mnt/mtd/config/.WlanMASK* ");
	mysystem("rm /mnt/mtd/config/.WlanGateWay* ");
	mysystem("rm /mnt/mtd/config/.WlanDHCP* ");
	mysystem("rm /mnt/mtd/config/.wificfg.ini ");
	mysystem("rm /mnt/mtd/config/.WlanWifiMode* ");
	mysystem("rm /mnt/mtd/config/.wifiConSuc* ");
	mysystem("rm /mnt/mtd/config/.webCfgWifi* ");
#endif
	mysystem("rm /mnt/mtd/config/passengerFlow.db");
	mysystem("rm /mnt/mtd/config/passengerFlow.db-bak");

	memset(command, 0, sizeof(command));
	snprintf(command, sizeof(command), "rm -v %s", MOTO_FILE_CFG); 
	mysystem(command); // eptzPreset.cfg

	/* 删除推送的Subkey信息 */
    snprintf(subkey_conf, sizeof(subkey_conf), "%s%s", SUBKEY_PARAM_DIR, SUBKEY_PARAM_CONFIG);
	if(0 == access(subkey_conf, F_OK))
	{
	    printf("%s is exist! need to remove it for subkey!\n", subkey_conf);
		remove(subkey_conf);
	}
	
	//删除SPS文件(避免无法订阅推送问题)
	if(0 == access(SPS_INFO_CFG, F_OK))
	{
	    printf("SPS_INFO_CFG is exist! need to remove it!!\n");
		remove(SPS_INFO_CFG);
	}

	mysystem(RESTART_TIMES);	
	mysystem(SAVE_ENV);
	OPEN_LOG_I("reset clean RESTART_TIMES\n");

	mysystem("sync");
#ifdef __MODULE_4G_SUPPORT__
    ZWSys_SetSpeechContent(SPEECH_4G_SYS_RESET_SUCCESS);
#else
    ZWSys_SetSpeechContent(SPEECH_SYS_RESET_SUCCESS);
#endif
    //循环查询，等待播放结束
    while(1)
    {
        sleep(1);
        if (0 == ZWSys_GetSpeechPlayingStatus())
        {
            break;
        }
        if (++counter > SPEECH_PLAY_TIME_OUT_SEC)
        {
            break;
        }
    }
	RebootLinux(REBOOT_RESUME);
}

int PAR_LockBakFile()
{
	MPU_PAR_CALL(LockBakFile());
}

/**************************************************************************
 *                     参数管理                                           *
 **************************************************************************/

//!\brief       保存当前配置参数到文件
//!\return  0表示成功，其它为错误码
int MPU_PAR_SaveConfig()
{
	//调用配置管理器执行动作
	MPU_PAR_CALL(SaveFile());
}

// 功能同MPU_PAR_SaveConfig ，同步即时保存
int MPU_PAR_SaveConfigReal()
{
	MPU_PAR_CALL(SaveFileReal());
}


//!\brief       恢复默认配置，本函数只用于配置文件已正常加载情况下的恢复参数。恢复默认前会备份先前加载的配置文件,备份文件名为文件名+-bak
//!\param   pszDefFileName      默认配置文件名称（含路径）
//!\return  0表示成功，其它为错误码 
int MPU_PAR_ResumeDefaultConfig(const char *pszDefFileName)
{
	//调用配置管理器执行动作
	MPU_PAR_CALL(ResumeDefCfg(pszDefFileName));
}

//!\brief   升级配置文件 升级配置文件前会备份先前加载的配置文件，备份文件名为文件名+-bak
//!\param   pszNewCfgFileName 升级的配置文件名称
//!\param   pszFlagFile 标记更新的项的文件,为NULL则将新配置文件中的值添加到原配置文件中
//!\return  0表示成功,其他为错误码
int MPU_PAR_UpdateConfig(const char *pszNewCfgFileName, const char *pszFlagFileName)
{
	MPU_PAR_CALL(UpdateCfg(pszNewCfgFileName, pszFlagFileName));
}

//!\brief   导入配置文件
//!\param   szFullFileName  配置文件名（含路径）
//!\return  0表示成功，其它为错误码
int MPU_PAR_LoadConfig(const char *pszFullFileName)
{
	//调用配置管理器执行动作
	MPU_PAR_CALL(LoadFile(pszFullFileName));
}

//!\brief   导出配置文件
//!\param   szFullFileName  新的配置文件名（含路径）
//!\return  0表示成功，其它为错误码
int MPU_PAR_SaveConfigAs(const char *pszFullFileName)
{
	//调用配置管理器执行动作
	MPU_PAR_CALL(SaveAsFile(pszFullFileName));
}

//!\brief   获取配置参数
//!\param   pszSection      区段名
//!\param   pszKey          关键字名
//!\param   nDefault        缺省值
//!\return  返回整形参数值
int MPU_PAR_GetValue(const char *pszSection, const char *pszKey, int nDefault)
{
	CConfig *pConfig = CConfig::GetInstance();

	if (NULL != pConfig)
	{
		return pConfig->GetValue(pszSection, pszKey, nDefault);
	}
	else
	{
		MPU_ERR("Get CConfig instance fail!\n");
		MPU_CHK(NULL != pConfig);
		return nDefault;
	}
}

//!\brief   设置配置参数
//!\param   pszSection      区段名
//!\param   pszKey          关键字名
//!\param   nValue          整形参数值
//!\return  0表示成功，其它为错误码
int MPU_PAR_SetValue(const char *pszSection, const char *pszKey, int nValue)
{
	//调用配置管理器执行动作
	MPU_PAR_CALL(SetValue(pszSection, pszKey, nValue));
}

//!\brief   获取配置参数
//!\param   pszSection      区段名
//!\param   pszKey          关键字名
//!\param   pszDefault      缺省值
//!\param   pValue          返回字符串型参数值
//!\param   nSize           buf空间大小
//!\return  本接口字符串型参数值到pValue指向的buf，并返回pValue
char *MPU_PAR_GetValue(const char *pszSection, const char *pszKey, const char *pszDefault, char *pValue, int nSize)
{
	CConfig *pConfig = CConfig::GetInstance();

	if (NULL != pConfig)
	{
		return pConfig->GetValue(pszSection, pszKey, pszDefault, pValue, nSize);
	}
	else
	{
		MPU_ERR("Get CConfig instance fail!\n");
		MPU_CHK(NULL != pConfig);

		if (NULL == pValue)
		{
			MPU_ERR("NULL == pValue\n");
			MPU_CHK(NULL != pValue);
			return NULL;
		}
		else if (0 >= nSize)
		{
			MPU_ERR("0 >= nSize\n");
			MPU_CHK(nSize > 0);
			return NULL;
		}
		else if (NULL == pszDefault)
		{
			pValue[0] = 0;
		}
		else
		{
			strncpy(pValue, pszDefault, nSize - 1);
			pValue[nSize - 1] = 0;
		}

		return pValue;
	}
}

//!\brief   设置配置参数
//!\param   pszSection      区段名
//!\param   pszKey          关键字名
//!\param   pValue          字符串型参数值
//!\return  0表示成功，其它为错误码
int MPU_PAR_SetValue(const char *pszSection, const char *pszKey, const char *pValue)
{
	//调用配置管理器执行动作
	MPU_PAR_CALL(SetValue(pszSection, pszKey, pValue));
}

//!\brief  	定制文件数据替换配置文件数据
//!\param   iCustomFile_Path    定制文件路径
//!\return  0表示成功，其它为错误码 
int MPU_PAR_CustomFileDate_Replace(const char *iCustomFile_Path, int nOperation)
{
 /* 需要新实例化一个对象，避免内存参数出现了乱窜的情况 */
    CConfig *pCfg = CConfig::NewInstance();
	char command[64] = {0};
	int retval = 0;
	
    if (NULL == pCfg)
    {
        printf("[%s-%d][ERR] NULL == cfg \n", __FILE__, __LINE__);
        return -1;
    }

	int Ret =  pCfg->LoadFile(iCustomFile_Path);
	if (0 != Ret)
	{
		printf("[%s-%d][ERR] Load File [%s] Fail\n", __FILE__, __LINE__, iCustomFile_Path);
        return -1;
	}

	if (0 == access(CUSTOM_MODE_RECOVER_DEFAULT, F_OK))
	{
		memset(command, 0, sizeof(command));
		snprintf(command, sizeof(command), "rm	%s", CUSTOM_MODE_RECOVER_DEFAULT);
		mysystem(command);
		
		retval = pCfg->ReplaceFileCfg(IDVR_DEFAULT_CONFIG_FILE_PATH);
		if (retval)
		{
			printf("[%s-%d][ERR] Fail to UpdateCfg %s\n", __FILE__, __LINE__, IDVR_DEFAULT_CONFIG_FILE_PATH);
			CConfig::ReleaseInstance(pCfg);
			return retval;
		}
	}
	else if (0 == access(CUSTOM_MODE_FLAG, F_OK) || 1 == nOperation)
	{
		retval = pCfg->ReplaceFileCfg(IDVR_CURRENT_CONFIG_FILE_PATH);
		if (retval)
		{
			printf("[%s-%d][ERR] Fail to UpdateCfg %s\n", __FILE__, __LINE__, IDVR_CURRENT_CONFIG_FILE_PATH);
	        CConfig::ReleaseInstance(pCfg);
	        return retval;
		}
		//设置admin用户密码
		#if defined (__IR_CUT_CONTROL_IMPROVE__) 
		USR_ResumeAdminPassword();
        #endif
        //等初始化完成再删除标记
        #if !defined (__IR_CUT_CONTROL_IMPROVE__) 
		memset(command, 0, sizeof(command));
		snprintf(command, sizeof(command), "rm  %s", CUSTOM_MODE_FLAG);
		mysystem(command);
        #endif

		//不必要的操作
		#if 0
		memset(command, 0, sizeof(command));
		snprintf(command, sizeof(command), "cp -v %s %s", IDVR_CURRENT_CONFIG_FILE_PATH, IDVR_BACKUP_CONFIG_FILE_PATH);
		mysystem(command);
		#endif
		
		retval = pCfg->ReplaceFileCfg(IDVR_DEFAULT_CONFIG_FILE_PATH);
		if (retval)
		{
			printf("[%s-%d][ERR] Fail to UpdateCfg %s\n", __FILE__, __LINE__, IDVR_DEFAULT_CONFIG_FILE_PATH);
			CConfig::ReleaseInstance(pCfg);
			return retval;
		}
	}
	
	CConfig::ReleaseInstance(pCfg);
	return 0;
}


/**************************************************************************
 *                                                        注销局部宏                                                           *
 **************************************************************************/
#undef MPU_PAR_CALL				//调用配置管理器执行动作
