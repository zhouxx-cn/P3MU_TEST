/*!
*\file   CSysManager.cpp 
*\brief  系统管理的实现
*\author cwf
*\date	  2009/03/24
*\version 0.1
*/

//#include <iostream.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#include "global_macro.h"
#include "CIP2PasswdResume.h"
#include "CMaintenance.h"
#include "CWatchDog.h"
#include "dst.h"
#include "CSysManager.h"
#include "zwsdk.h"
#ifndef NO_USE_BUZZER
#include "CBuzzer.h"
#endif
#include "sntp.h"
#if defined (__IS_SUPPORT_APP_SHARE__)
#include "common_pub.h"
#include "stdDes.h"
#include "base64Lib.h"
#include "Authority.h"
#include "timex.h"
#endif
#include "debug.h"

/**************************************************************************
*                            常量                                        *
**************************************************************************/
#define SCODE_MAX_LEN (32)

/**************************************************************************
*                          数据类型                                       *
**************************************************************************/

/**************************************************************************
*                           全局变量                                      *
**************************************************************************/
static int IntervalHour = 1;//ntp同步间隔时间 单位：小时

//初始化全局变量
CSysManager* CSysManager::theCSysManager = NULL;

// 检测当前互联网状态回调函数
int (*m_CheckInterNetStatusCallback)(int *) = NULL;

/**************************************************************************
*                           局部函数原型                                  *
 **************************************************************************/
static void *AutoNtpThreadProcess(void *param);
static void *NtpTimeCountThreadProcess(void *param);
static void *CheckInternetthreadIDProcess(void *param);

/**************************************************************************
*                           方法实现                                      *
 **************************************************************************/

#if defined (__IS_SUPPORT_APP_SHARE__)
//!\brief  检查并从文件中获取 SCode
//!\param  const char *file    文件目录
//!\param  char *code          S码数据
//!\param  int codeLen         数据长度
//!\return  成功为0，失败为-1
int CheckAndGetSCodeFromFile(const char *file, char *code, int codeLen)
{
    int rc = 0;
    CConfig *pConf = CConfig::NewInstance();

    // 1. 文件存在与否
    if (0 != access(file, F_OK))
    {
		if (pConf)
		{
			CConfig::ReleaseInstance(pConf);
		}
        CSYS_ERR("Scode File not exist [%s]\n", file);
        return -1;
    }

    // 2.file格式检查
    rc = pConf->LoadFile(file);
    if (0 != rc)
    {
        CConfig::ReleaseInstance(pConf);
        CSYS_ERR("Scode File format error! [%s]\n", file);
        return -1;
    }

    // 3.判断Scode内容长度, 固定为32位+@
    pConf->GetValue("ERP", "SCode", "", code, codeLen);
    if (strlen(code) != SCODE_MAX_LEN+1)
    {
        CConfig::ReleaseInstance(pConf);
        CSYS_ERR("Scode length error! [%s][%d][%s]\n", file, strlen(code), code);
        return -1;
    }

    CConfig::ReleaseInstance(pConf);
    CSYS_ERR("Scode Check success! [%s]\n", file);
    return 0;
}

//!\brief   判断s码的配置文件是否残缺进行相互恢复
//!\return  成功为0，失败为-1
int CheckDevSCodeFromFileAndRecover()
{
    int nEcode = 0;
	char buf[128] = {0};
    char szSCodeInfo[128] = {0};
	char szSCodeBakInfo[128] = {0};
    
    /* 1、如果scode和其备份文件均存在，判断内容的完整性 */
    if ((0 == access(SCODE_CONF, F_OK)) && (0 == access(SCODE_BACKUP_CONF, F_OK)))
    {
        if (CheckAndGetSCodeFromFile(SCODE_CONF, szSCodeInfo, sizeof(szSCodeInfo)))
        {
            nEcode = 1;
        }
        if (CheckAndGetSCodeFromFile(SCODE_BACKUP_CONF, szSCodeBakInfo, sizeof(szSCodeBakInfo)))
        {
            nEcode |= (1 << 1);
        }

        if (0 == nEcode)
        {
            if (0 != strncmp(szSCodeInfo, szSCodeBakInfo, SCODE_MAX_LEN))
            {
                memset(buf, 0, sizeof(buf));
                snprintf(buf,sizeof(buf), "cp -rf %s %s", SCODE_BACKUP_CONF, SCODE_CONF);
                mysystem(buf);
                CSYS_INF("Scode: Restore from the backup file\n");
            }
        }
        else if (1 == nEcode)
        {
            memset(buf, 0, sizeof(buf));
            snprintf(buf,sizeof(buf), "cp -rf %s %s", SCODE_BACKUP_CONF, SCODE_CONF);
            mysystem(buf);
            CSYS_INF("Scode: Restore from the backup file\n");
        }
        else if (2 == nEcode)
        {
            memset(buf, 0, sizeof(buf));
            snprintf(buf,sizeof(buf), "cp -rf %s %s", SCODE_CONF, SCODE_BACKUP_CONF);
            mysystem(buf);
            CSYS_INF("Scode: Add to the backup file\n");
        }
        else
        {
            unlink(SCODE_CONF);
            unlink(SCODE_BACKUP_CONF);
            nEcode = -1;
        }
    }
    /* 2、如果只存在备份文件，判断内容的完整性，恢复scode */
    else if ((0 != access(SCODE_CONF, F_OK)) && (0 == access(SCODE_BACKUP_CONF, F_OK)))
    {
        if (0 == CheckAndGetSCodeFromFile(SCODE_BACKUP_CONF, szSCodeBakInfo, sizeof(szSCodeBakInfo)))
        {
            memset(buf, 0, sizeof(buf));
            snprintf(buf,sizeof(buf), "cp -rf %s %s", SCODE_BACKUP_CONF, SCODE_CONF);
            mysystem(buf);
            CSYS_INF("Scode: Restore from the backup file\n");
        }
        else
        {
            unlink(SCODE_BACKUP_CONF);
            nEcode = -1;
        }
    }
    /* 3、如果只存在scode文件，判断内容的完整性，完成备份 */
    else if ((0 == access(SCODE_CONF, F_OK)) && (0 != access(SCODE_BACKUP_CONF, F_OK)))
    {
        if (0 == CheckAndGetSCodeFromFile(SCODE_CONF, szSCodeInfo, sizeof(szSCodeInfo)))
        {
            memset(buf, 0, sizeof(buf));
            snprintf(buf,sizeof(buf), "cp -rf %s %s", SCODE_CONF, SCODE_BACKUP_CONF);
            mysystem(buf);
            CSYS_INF("Scode: Add to the backup file\n");
        }
		else
		{
			unlink(SCODE_CONF);
			nEcode = -1;
		}
    }
    /* 4、如果都不存在返回-1，尝试从License 2分区中读取S码 */
    else
    {
        //SetSCodeToFile(SCODE_CONF, "@1");
        CSYS_INF("Scode: Try to read the License 2 partition\n");
        return -1;
    }

    /* scode和其备份文件均异常，需要创建一个默认scode文件，值@1只是为了让APP提示设备异常 */
    if (-1 == nEcode)
    {
        //SetSCodeToFile(SCODE_CONF, "@1");
		return -1;
    }

    return 0;
}
#endif

//!\brief 获取系统管理句柄
//!\return  成功为为句柄，失败为NULL
//##ModelId=49BB333100F8
CSysManager* CSysManager::GetInstance()
{
	// TODO: Add your specialized code here.

	//初始化系统管理模块
	if (NULL == theCSysManager)
	{
		theCSysManager                          = new CSysManager();
		if ( NULL != theCSysManager)
		{
#ifndef NO_USE_BUZZER
			theCSysManager->theCBuzzer          = new CBuzzer();
			theCSysManager->theCBuzzer->StartBuzzerTask();
#endif

			theCSysManager->theCIP2PasswdResume = new CIP2PasswdResume();
			theCSysManager->theCIP2PasswdResume->StartResumeTask();
			theCSysManager->theCWatchDog        = new CWatchDog();

        	//从配置文件初始化
        	CConfig *pConfig = CConfig::GetInstance();
        	if (NULL != pConfig)
        	{
				char strTimeZone[6] = {0};
				pConfig->GetValue("PUB", "TimeZone", "8", strTimeZone, sizeof(strTimeZone) - 1);
				theCSysManager->m_timezone = (int)(atof(strTimeZone) * 60);

    			theCSysManager->m_ntp_enable = pConfig->GetValue(INTP_CfgSection, "Enable", 0);
    			theCSysManager->m_nIntervalSec = pConfig->GetValue(INTP_CfgSection, "IntervalSec", 1);
                IntervalHour = (theCSysManager->m_nIntervalSec >= 3600) ? (theCSysManager->m_nIntervalSec / 3600) : theCSysManager->m_nIntervalSec;
                
    			pConfig->GetValue(INTP_CfgSection, "DomainName", "time.windows.com", theCSysManager->m_ntp_server, 256);			


			    if (theCSysManager->m_ntp_server[strlen(theCSysManager->m_ntp_server)-1] == '\r' || theCSysManager->m_ntp_server[strlen(theCSysManager->m_ntp_server)-1] == '\n')
                {
                    theCSysManager->m_ntp_server[strlen(theCSysManager->m_ntp_server)-1] = '\0';
                }

        	}
        	else
        	{
                theCSysManager->m_timezone = 8 * 60;
                theCSysManager->m_ntp_enable = 0;
                theCSysManager->m_nIntervalSec = 1;
                theCSysManager->m_net_has_connected = 0;                
                strncpy(theCSysManager->m_ntp_server, "time.windows.com", sizeof(theCSysManager->m_ntp_server) - 1);
        	}
		#if defined (__IS_SUPPORT_APP_SHARE__)
			theCSysManager->m_GetSCodeValid = 1;

            /* 设备S码状态初始化，S码存放于配置文件中 */
            theCSysManager->InitDevSCodeState();
		#endif

			DST_Init();
#if 0
            //开机后如果使能了ntp则立即对时
            if(theCSysManager->m_ntp_enable)
            {
                theCSysManager->m_configChanged = 1;
            }
#endif
		    //创建同步线程开始同步
            int nret = pthread_create(&theCSysManager->m_autopNtp, NULL, AutoNtpThreadProcess, theCSysManager);
            if(nret)
            {
                CSYS_WRAN("pthread create AutoNtpThread ERROR!!!\n");
        	}
			//创建计时线程
            nret = pthread_create(&theCSysManager->m_ntp_TimethrdID, NULL, NtpTimeCountThreadProcess, theCSysManager);
            if(nret)
            {
                CSYS_WRAN("pthread create NtpTimeCountThreadProcess ERROR!!!\n");
        	}
			//创建互联网检测线程
            nret = pthread_create(&theCSysManager->m_CheckInternetthreadID, NULL, CheckInternetthreadIDProcess, theCSysManager);
            if(nret)
            {
                CSYS_WRAN("pthread create CheckInternetthreadIDProcess ERROR!!!\n");
        	}            
		}
	}
	return (CSysManager*)theCSysManager;
}


//!\brief 销毁实例
//!\return  成功为0，失败为-1
//##ModelId=49BB333100FA
int CSysManager::ReleaseInstance()
{
	// TODO: Add your specialized code here.

	//释放系统管理模块
	if (NULL != theCSysManager)
	{
    #ifndef NO_USE_BUZZER
            theCSysManager->theCBuzzer->StopBuzzerTask();
        //  delete theCSysManager->theCBuzzer;
    #endif

		theCSysManager->theCIP2PasswdResume->StopResumeTask();
		//delete theCSysManager->theCIP2PasswdResume;
		//delete theCSysManager->theCWatchDog;
		//delete theCSysManager;
		//theCSysManager = NULL;
	}

	CSYS_INF("ReleaseInstance End!\n");
	return (int)0;
}

//!\brief 开始蜂鸣器工作任务
//!\praram void
//!\return  成功为0，失败为-1
//##ModelId=49BB33310102
int CSysManager::StartBuzzerTask()
{
	// TODO: Add your specialized code here.
	
	int ret = 0;

#ifndef NO_USE_BUZZER
    if (NULL != theCSysManager->theCBuzzer)
    {
        ret = theCSysManager->theCBuzzer->StartBuzzerTask();
    }
#endif

	return (int)ret;
}

//!\brief 设置蜂鸣器蜂鸣持续时间
//!\praram int times 蜂鸣器持续蜂鸣时间.0为持续蜂鸣
//!\return  成功为0，失败为-1
int CSysManager::SetBuzzerTime(unsigned int times)
{
	int ret = 0;
	
#ifndef NO_USE_BUZZER
    if (NULL != theCSysManager->theCBuzzer)
    {
        ret = theCSysManager->theCBuzzer->SetBuzzerTime(times);
    }
#endif	
	
	return (int)ret;	
}


//!\brief 设置夏时制规则
//!\return  成功为0，失败为-1
int CSysManager::SetDST(LPDVR_DST_EX Dst)
{
	int nRet = 0;
	nRet = EDVR_MAN_SetDST(Dst);
	return nRet;
}

//!\brief 获取夏时制规则
//!\return  成功为0，失败为-1
int CSysManager::GetDST(LPDVR_DST_EX Dst)
{
	EDVR_MAN_GetDST(Dst);
	return 0;
}


//!\brief 设置时区
//!\return  成功为0，失败为-1
int CSysManager::SetTimeZone(int nTimeZone)
{
	int nRet = 0;
	
    if (m_timezone == nTimeZone)
    {
        return 0;
    }
    m_timezone = nTimeZone;
	m_configChanged = 1;

    /* 将当前设置的时区保存到 */
    char cmdStr[128] = {0};
    snprintf(cmdStr, sizeof(cmdStr), "echo %d > %s", m_timezone, TIME_ZONE_CFG);
    mysystem(cmdStr);
	
	CConfig *pConfig = CConfig::GetInstance();
	if (NULL != pConfig)
	{   
		double fTimeZone = (double)nTimeZone/(double)60;
		char timeZoneValue[16];
		snprintf(timeZoneValue, sizeof(timeZoneValue), "%g", fTimeZone);
		//timeZoneValue[5] = 0;
		nRet = pConfig->SetValue("PUB","TimeZone",timeZoneValue);
		if (nRet == 0 && pConfig->m_bReadOnlyCfgFlag == 1)
		{
			m_timezone = (pConfig->GetValue("PUB", "TimeZone", 8) * 60);
		}
		
		return 0;
	}
	
	return -1;
}

//!\brief 获取时区
//!\return  成功为0，失败为-1
int CSysManager::GetTimeZone()
{
	return m_timezone;
}

//!\brief 设置ntp使能
//!\return  成功为0，失败为-1
int CSysManager::SetNtpEnable(int enable)
{
	int result = 0;
	
    if (m_ntp_enable == enable)
    {
        return 0;
    }
    m_ntp_enable = !m_ntp_enable;
	if(m_ntp_enable)
    {
        m_configChanged = 1;
    }
	CConfig *pConfig = CConfig::GetInstance();
	if (NULL != pConfig)
	{   
	    result = pConfig->SetValue(INTP_CfgSection, "Enable", m_ntp_enable);
		if (result == 0 && pConfig->m_bReadOnlyCfgFlag == 1)
		{
			m_ntp_enable = pConfig->GetValue(INTP_CfgSection, "Enable", 1);
		}
		
		return 0;
	}
	return -1;
}

//!\brief 获取ntp使能
int CSysManager::GetNtpEnable()
{
    return m_ntp_enable;
}

//int isDoNtpSyncIfSucc = 0;

/*
//!\brief NTP计时线程
//!\@cyn 2016_12_22
*/
static void *NtpTimeCountThreadProcess(void *param)
{
    PRT_TID();
    pthread_detach(pthread_self());

	CSysManager *pCsys = (CSysManager *)param;
	int timeCount_sec=0;				//以秒计时
	int timeCount_hour=0;				//小时计时
	int countFlag=0;					//开始计时标识
	time_t now;
	struct tm timenow = {0};

	while(1)
	{
		if(pCsys->m_TimeCountFlag)//在上次对时完成后，将置开始计时标识变量为1
		{
			pCsys->m_TimeCountFlag=0;
            char tBuff[64] = {0};
			time(&now);
			localtime_r(&now,&timenow); 
            asctime_r(&timenow,tBuff);
			CSYS_DBG("\n\n******start ntp time count: %s,******m_nIntervalSec=%d IntervalHour=%d\n\n", tBuff,pCsys->m_nIntervalSec, IntervalHour);
			

			timeCount_sec=0;
			timeCount_hour=0;
			countFlag=1;
			
	
    
    
    
    	}
		if(countFlag==1)
		{
			if(timeCount_sec==3600)// 1h
			{
				timeCount_hour++;
				//if(timeCount_hour==pCsys->m_nIntervalSec)
				if(timeCount_hour == IntervalHour)
				{
					countFlag=0;
					
					pCsys->m_TimeToSyncFlag=1;//置对时标识
					CSYS_DBG("\n\n******time to sync ntp*******\n\n");
				}
				timeCount_sec=0;
			}
			timeCount_sec++;

		}		
		usleep(1000000);//睡眠1s
	}
	return NULL;

}
static void *AutoNtpThreadProcess(void *param)
{
    PRT_TID();
    pthread_detach(pthread_self());

    CSysManager *pCsys = (CSysManager *)param;
    //time_t now;
	int statusFlag  = 0;
	int flag = 0;			//开机设置
	int ret  = 0;

    int bReActive     = 0;  // 是否激活(对时失败则激活)
    int nCurrRetryCnt = 0;  // 当前重试次数
    int nTimeCount    = 0;  // 重试计时
    int bNeedRetry    = 0;  // 是否需要重试
#ifdef __MODULE_4G_SUPPORT__
    int nNtpRetryIntrvSec = 1; //NTP对时重试时间间隔
#else
	int nNtpRetryIntrvSec = 15; //NTP对时重试时间间隔	
#endif
    /*
    while(1)
    {
        pthread_testcancel();
        if (0 == pCsys->m_nIntervalSec)
        {
            return 0;
        }
        pCsys->DoNtpSync();
        sleep(pCsys->m_nIntervalSec);
    }
    */
     if (0 == pCsys->m_ntp_enable)
    {
    	//未使能时
		pCsys->SetNtpSuccessFlag(1);
    }
     
    while (1)
    {
        /* 如果当前互联网正常且需要开机对时，则需要立即对时 */
        if (pCsys->m_net_has_connected)
        {
            if (0 == flag)
            {
                // 开机后如果使能了ntp则立即对时
                if (pCsys->m_ntp_enable)
                {
                    pCsys->m_configChanged = 1;
                    CSYS_DBG("ntp has enable! need do sntp sync time now!\n");
                }
                flag = 1;
            }
        }
        /* 检测互联网线程结束后，如果仍然未连接互联网，需要开启对时，互联网恢复时第一时间开启对时 */
        else
        {
            if (0 == flag && 1 == pCsys->m_CheckNetThreadExitFlag)
            {
                if (pCsys->m_ntp_enable)
                {
                    pCsys->m_configChanged = 1;
                    CSYS_DBG("ntp has enable! need do sntp sync time now!\n");
                }
                flag = 1;
            }
        }
    
        //now = time(NULL);/* 获取当前系统时间 */
		statusFlag = pCsys->m_configChanged;
		pCsys->m_configChanged = 0;
        /* 每天的0点同步更新时间 */
        /* 如果重试的过程中有事件在执行对时，则一切重试清零 */
        if (statusFlag)
		{
            bReActive = 0;
            bNeedRetry = 0;
            nTimeCount = 0;
            nCurrRetryCnt = 0;
		}
        /* 如果计时时间到或者外部执行对时或者需要重试，则执行对时 */
        if ((pCsys->m_TimeToSyncFlag) || statusFlag || bNeedRetry)
        {
        	CSYS_DBG("\n\n****** ready DoNtpSync-----------------\n\n");
            if (pCsys->m_ntp_enable)
            {
            	pCsys->m_TimeToSyncFlag = 0;//时间到标识置0
                ret = pCsys->DoNtpSync();
                if (0 == ret)
                {
                    CSYS_INF("******After %d times ntp sync successful!*****\n", nCurrRetryCnt+1);
					pCsys->m_TimeCountFlag = 1;//开始计时
                    bReActive = 0;
                    nTimeCount = 0;                    
                    bNeedRetry = 0;
                    nCurrRetryCnt = 0;
#ifdef __MODULE_4G_SUPPORT__
                    nNtpRetryIntrvSec = 1;
#else
					nNtpRetryIntrvSec = 15;					
#endif
                    pCsys->SetNtpSuccessFlag(1);
                }
                else
                {
                    /* 对时失败需要激活重试选项 */                    
                    CSYS_ERR("sntp synchronizes time fail!\n");
                    bReActive = 1;
		            bNeedRetry = 0;
		            nTimeCount = 0;
                }
            }
        }
        nTimeCount++;

        /* 如果对时失败需要进行重试ntp对时 */
        if (bReActive)
        {
            /* 如果计时时间超过15S则需要去重试对时一次 */
            if (nTimeCount >= nNtpRetryIntrvSec)
            {
                nTimeCount = 0;
                bNeedRetry = 1; 
                nCurrRetryCnt++;
            }
            else
            {
                bNeedRetry = 0;
            }
            /* 如果重试次数超过10次，则一切重试清零 */
#ifdef __MODULE_4G_SUPPORT__    
            if (nCurrRetryCnt >= 120)
#else
            /* 如果重试次数超过10次，则一切重试清零 */
            if (nCurrRetryCnt >= 10)
#endif
            {
            	//对不上永久对时，修改重试间隔为60秒
				nNtpRetryIntrvSec = 60;
            }
			//CSYS_DBG("======bReActive=%d, bNeedRetry=%d, nTimeCount=%d, nCurrRetryCnt=%d======\n", bReActive, bNeedRetry, nTimeCount, nCurrRetryCnt);
        }
        
        usleep(1000000);
    }
	
    return NULL;
}

/* 检测互联网连通线程 */
static void *CheckInternetthreadIDProcess(void *param)
{
    PRT_TID();
    pthread_detach(pthread_self());

    CSysManager *pCsys = (CSysManager *)param;
    int conn_status = 0;
    int i = 0;

    CSYS_DBG("Start to check Internet connect status ...\n");

    /* 300秒钟检测当前外网网络状态(如果成功则提前退出，否则超时退出) */
    /* 实际测试如果一直无法连接互联网，该线程由于for循环会持续1个小时以上 */
    for (i=0; i<300; i++)
    {
        if (m_CheckInterNetStatusCallback)
        {
            m_CheckInterNetStatusCallback(&conn_status);
            if (conn_status)
            {
                pCsys->m_net_has_connected = 1;
                break;
            }
            else
            {
                pCsys->m_net_has_connected = 0;
            }
        }

        sleep(1);
    }
	pCsys->m_CheckNetThreadExitFlag = 1;

    CSYS_INF("Current Internet is %s!\n", conn_status?"OK":"Break");

    return NULL;
}

//int isDoNtpSync = 0;

int CSysManager::DoNtpSync()
{
    int ret = SyncNtpDate(m_ntp_server, m_timezone * 60);

//  if (nret_fd <= 0)
    if (0 == ret)
    {
        //通知系统SDK对时
		ZWSys_EncSetCurrentTime();

        set_time_sync_flag();

        CSYS_INF("Sntp running successful, if dst is enable, need to manual fresh the dst!\n");
		
		//对时成功，如果DST使能，则需要手动刷新DST
		if (EDVR_MAN_IsEnableDST())
		{
		    EDVR_MAN_ManualRefresh_DST();
		}
    }
    else
    {
        CSYS_ERR("Sntp running fail!\n");
//      close(nret_fd);
//      mysystem("rm -rf /tmp/ntp_ret");
        return 1;
    }
	
    return 0;
}

//!\brief 设置ntp同步成功标识
void CSysManager::SetNtpSuccessFlag(int flag)
{
	m_ntp_request_is_success = flag;
}

//!\brief 获取ntp同步成功标识
int CSysManager::GetNtpSuccessFlag()
{
	return m_ntp_request_is_success;
}

//!\brief 设置ntp同步时间间隔
//!\return  成功为0，失败为-1
int CSysManager::SetNtpInterval(int nIntervalSec)
{
    if (m_nIntervalSec == nIntervalSec)
    {
        return 0;
    }
    m_nIntervalSec = nIntervalSec;
	m_configChanged = 1;//状态改变
	CSYS_DBG("\n\n*******m_nIntervalSec changged to :%d*******\n\n",m_nIntervalSec);

    IntervalHour = (m_nIntervalSec >= 3600) ? (m_nIntervalSec / 3600) : m_nIntervalSec;
    
	CConfig *pConfig = CConfig::GetInstance();
	if (NULL != pConfig)
	{   
	    pConfig->SetValue(INTP_CfgSection, "IntervalSec", m_nIntervalSec);
		return 0;
	}
	return -1;
}

//!\brief 获取ntp同步间隔
int CSysManager::GetNtpInterval()
{
    return m_nIntervalSec;
}

//!\brief 设置ntp服务器
//!\return  成功为0，失败为-1
int CSysManager::SetNtpServer(const char *ntp_server)
{
    if (NULL == ntp_server)
    {
        return -1;
    }
    else
    {
        int nbufLen = strlen(ntp_server);
        if (strcmp(ntp_server, m_ntp_server) == 0)
        {
            return 0;
        }
        memset(m_ntp_server, 0, sizeof(m_ntp_server));
        strncpy(m_ntp_server, ntp_server, nbufLen - 1);
		if (m_ntp_server[strlen(m_ntp_server)-1] == '\r' || m_ntp_server[strlen(m_ntp_server)-1] == '\n')
			m_ntp_server[strlen(m_ntp_server)-1] = '\0';
		
        CConfig *pConfig = CConfig::GetInstance();
        pConfig->SetValue(INTP_CfgSection, "DomainName", theCSysManager->m_ntp_server);
		m_configChanged = 1;
    }
    return 0;
}

//!\brief 获取ntp服务器
//!\return  成功为0，失败为-1
int CSysManager::GetNtpServer(char *ntp_server, int nbufLen)
{
    if (NULL == ntp_server)
    {
        return -1;
    }
    if ((unsigned int)nbufLen < strlen(m_ntp_server))
    {
        return -1;
    }

    strncpy(ntp_server, m_ntp_server, nbufLen - 1);
    return 0;
}

//!\brief 启用夏时制
//!\return  成功为0，失败为-1
int CSysManager::EnableDST(unsigned int enable)
{
	EDVR_MAN_EnableDST(enable);
	return 0;
}

//!\brief 获取是否启用夏时制
//!\return  成功为0，失败为-1
int CSysManager::IsEnableDST()
{
	return EDVR_MAN_IsEnableDST();
}

//!\brief 获取当前DST的偏移时间(秒)
//!\return 偏移(秒)
int CSysManager::GetDstBias()
{
    return EDVR_MAN_GetDstBias();
}

//!\brief 获取当前DST是否已经工作(起效)
//!\return 0不工作(时间未到或者DST关闭) 1起效
int CSysManager::GetDstIsWork()
{
    return EDVR_MAN_GetDstIsWork();
}

DSTCallBack CSysManager::DstCb = NULL;
void CSysManager::EDVR_CallBack(time_t t)
{
	tagTime Time = {0};
	Time.MkTag(t);
	if (DstCb != NULL)
	{
		DstCb(Time);
	}
}

//!\brief 设置启用夏时制时的回调函数
//!\return  成功为0，失败为-1
//##ModelId=49BB33310122
int CSysManager::SetDSTCallBack(DSTCallBack Cb)
{
	DstCb = Cb;
	EDVR_MAN_SetCallBack(EDVR_CallBack);
	return 0;
}

// 设置检测当前互联网状态回调函数
int CSysManager::SetCheckInterNetStatusCallBack(int (*CallBack)(int *))
{
    if (NULL == CallBack)
    {
        return -1;        
    }

    m_CheckInterNetStatusCallback = CallBack;

    return 0;
}

//!\brief 获取蜂鸣器蜂鸣持续时间
//!\praram int times 蜂鸣器持续蜂鸣时间.0为持续蜂鸣
//!\return  成功为0，失败为-1
int CSysManager::GetBuzzerTime()
{
    int ret = 0;
#ifndef NO_USE_BUZZER
    if (NULL != theCSysManager->theCBuzzer)
    {
        ret = theCSysManager->theCBuzzer->GetBuzzerTime();
    }
#endif	

	return (int)ret;
}
//!\brief 设置蜂鸣器工作
//!\praram enumBuzzerType enumBuzzer 蜂鸣类型、为NOBUZZER停止蜂鸣
//!\return  成功为0，失败为-1
//##ModelId=49BB33310103
int CSysManager::KeepBuzzer( enumBuzzerType enumBuzzer)
{
	// TODO: Add your specialized code here.
	
    int ret = 0;
#ifndef NO_USE_BUZZER	
    if (NULL != theCSysManager->theCBuzzer)
    {
        ret = theCSysManager->theCBuzzer->KeepBuzzer(enumBuzzer);
    }
#endif	

	return (int)ret;
}


//!\brief 蜂鸣一次
//!\praram enumBuzzerType enumBuzzer 蜂鸣类型
//!\return  成功为0，失败为-1
//##ModelId=49BB33310106
int CSysManager::Buzzer(enumBuzzerType enumBuzzer)
{
	// TODO: Add your specialized code here.
	
    int ret = 0;
#ifndef NO_USE_BUZZER	
    if (NULL != theCSysManager->theCBuzzer)
    {
        ret = theCSysManager->theCBuzzer->Buzzer(enumBuzzer);
    }
#endif	

	
	return (int)ret;
}

//##ModelId=49BB3331010C
int CSysManager::Reboot_safe()
{
	// TODO: Add your specialized code here.
	return (int)0;
}

//##ModelId=49BB3331010D
int CSysManager::Halt_safe()
{
	// TODO: Add your specialized code here.
	return (int)0;
}


//!\brief 验证系统板
//!\praram 无
//!\return  成功为0，失败为-1
//##ModelId=49BB3331010E
int CSysManager::BoardAuthen(void )
{
	return 0;
}

//!\brief 验证系统板
//!\praram 无
//!\return  成功为0，失败为-1
//##ModelId=49BB3331010E
int CSysManager::Led(int ledHZ)
{
	// TODO: Add your specialized code here.
	int ret = -1;	

	return (int) ret;
}

//!\brief	开始复位检测
//!\praram	bool bStart	是否
//!\return  成功为0，失败为-1
//##ModelId=49BB33310110
int CSysManager::SetResumeTask(bool bStart)
{
	// TODO: Add your specialized code here.
	
	
	int ret = -1;
	
	if (NULL != theCSysManager->theCIP2PasswdResume)
	{
		if (bStart == true)
		{
			ret = theCSysManager->theCIP2PasswdResume->StartResumeTask();
		}
		else
		{
			ret = theCSysManager->theCIP2PasswdResume->StopResumeTask();
		}
	}
	
	return (int)ret;
}


//!\brief 开始复位检测
//!\praram void(*CallBack)(void) 回调函数指针
//!\return  成功为0，失败为-1
//##ModelId=49BB33310116
int CSysManager::SetResumeCallBack(void (*CallBack)(void))
{
	// TODO: Add your specialized code here.
	
	int ret = -1;
	
	if (NULL != theCSysManager->theCIP2PasswdResume)
	{
		theCSysManager->theCIP2PasswdResume->SetResumeCallBack(CallBack);
	}
	
	return (int)ret;
}

//!\brief 设置测试模式
//!\praram nTestMode 模式值 0-出厂模式， 1-工厂模式
//!\return  成功为0，失败为-1
int CSysManager::SetProductTestMode(int nTestMode)
{
	int ret = -1;
	
	if (NULL != theCSysManager->theCIP2PasswdResume)
	{
		ret = theCSysManager->theCIP2PasswdResume->SetProductTestMode(nTestMode);
	}

	return (int)ret;
	
}

//!\brief 获取复位状态
//!\praram nStatus 状态值 0-没有复位 1-已经按下复位
//!\return  成功为0，失败为-1
int CSysManager::GetHardResetStatus(int *nStatus)
{
	int ret = -1;
	
	if (NULL != theCSysManager->theCIP2PasswdResume)
	{
		ret = theCSysManager->theCIP2PasswdResume->GetHardResetStatus(nStatus);
	}

	return (int)ret;

}

//!\brief 清空硬复位状态
int  CSysManager::ClearHardResetStatus()
{
	int ret = -1;
	
	if (NULL != theCSysManager->theCIP2PasswdResume)
	{
		theCSysManager->theCIP2PasswdResume->ClearHardResetStatus();
		return 0;
	}

	return (int)ret;
}

//!\brief 添加一个看门狗任务
//!\praram char *szDogName 看门狗名字
//!\praram int nTimer 
//需要喂狗的时间。喂狗时间大于该时间可能会导致失败重启
//!\return  成功为该任务的句柄，失败为NULL
//##ModelId=49BB33310118
HANDLE CSysManager::AddDogTask(const char* szDogName, int nTimer)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	HANDLE handle = NULL;
	
	if (NULL != theCSysManager->theCWatchDog)
	{
		handle = theCSysManager->theCWatchDog->AddDogTask(szDogName,nTimer);
	}
	
	return handle;
}


//!\brief 注销一个看门狗任务
//!\praram HWND handle 看门狗任务句柄
//!\return  成功为0，失败为-1
//##ModelId=49BB33310120
int CSysManager::DestroyDogTask(HANDLE handle)
{
	// TODO: Add your specialized code here.
	int ret = -1;
	
	if (NULL != theCSysManager->theCWatchDog)
	{
		ret = theCSysManager->theCWatchDog->DestroyDogTask(handle);
	}
	
	return ret;
}


//!\brief 喂狗操作
//!\praram HWND handle 看门狗任务句柄
//!\return  成功为0，失败为-1
//##ModelId=49BB33310122
int CSysManager::WatchDog(HANDLE handle)
{
	// TODO: Add your specialized code here.
	int ret = -1;
	
	if (NULL != theCSysManager->theCWatchDog)
	{
		ret = theCSysManager->theCWatchDog->WatchDog(handle);
	}
	
	return ret;
}

#if defined (__IS_SUPPORT_APP_SHARE__)
//!\brief 初始化设备的S码状态
//!\rerurn 成功返回0,m_GetSCodeValid等于0返回-1
int CSysManager::InitDevSCodeState()
{
	if (0 == m_GetSCodeValid)
    {
        return -1;
    }

    int ret = 0;

    if (0 != CheckDevSCodeFromFileAndRecover()) 
    {
		char TutkUID[MAX_DID_LENGTH] = {0};
		char PushId[MAX_PUSH_ID_LENGTH] = {0};

		ret = GetUIDInfoFromLicensePart(TutkUID, MAX_DID_LENGTH, PushId, MAX_PUSH_ID_LENGTH);
		if(!ret)
		{
			CSYS_INF("Start to rebuild Scode!\n");
			CreateSCode(TutkUID);
		}
		else
		{
			CSYS_ERR("Init Scode, but cannot get UID from license part!\n");
		}
    }
	else
	{
        CSYS_INF("CheckDevSCodeFromFileAndRecover return [%d]\n", ret);
    }

    CSYS_INF("Init Scode End! [%d]\n", ret);
    return 0;
	
}

//!\brief 根据绑定状态获取S码
//!\return  成功为0，失败为-1
int CSysManager::GetSCodeOfBindState(char *code, int *codeLen)
{
	if ((NULL == code) || (*codeLen <= 0) || (0 == m_GetSCodeValid))
    {
        CSYS_ERR("Param Err! code[%p] codeLen[%d]!\n", code, *codeLen);
        return -1;
    }

	// 获取SCODE_INI配置文件中的数据
	// 若无值则说明设备已经被绑定且未复位，返回失败;
	// 若有值，则检查@前面的数据，有效则说明设备已经被绑定且未复位，返回失败;
	// 若@后面的数据有效则返回数据给code
    int ret = -1;
	char *p = NULL;
	char szSCodeInfo[128] = {0};
	char szSCode[64] = {0};
	char szSCodeBak[64] = {0};
	if (GetSCodeFromFile(SCODE_CONF, szSCodeInfo ,sizeof(szSCodeInfo)))
    {
        CSYS_ERR("Fail To Get szSCodeInfo From SCODE_INI\n");
        return -1;
    }
	else
	{
	    if (strlen(szSCodeInfo) <= 0)
	    {
	        return -1;
	    }

		p = strchr(szSCodeInfo, '@');
		if (NULL != p)
		{
			memcpy(szSCode, szSCodeInfo, p-szSCodeInfo);
			memcpy(szSCodeBak, p+1, szSCodeInfo+strlen(szSCodeInfo)-p-1);
			if (strlen(szSCode) > 0)
			{
				return -1;
			}
			else if (strlen(szSCodeBak) > 0)
			{
				memcpy(code, szSCodeBak, strlen(szSCodeBak));
				*codeLen = strlen(code);
				ret = 0;
			}
		}
	}

	return ret;
}

//!\brief 获取复位前的S码
//!\return  成功为0，失败为-1
int CSysManager::GetSCodeBak(char *code, int *codeLen)
{
	if ((NULL == code) || (*codeLen <= 0) || (0 == m_GetSCodeValid))
    {
        CSYS_ERR("Param Err! code[%p] codeLen[%d]!\n", code, *codeLen);
        return -1;
    }

    int ret = -1;

	/* 获取SCODE_INI配置文件中的数据，有效则返回为S码 */
	char *p = NULL;
	char szSCodeInfo[128] = {0};
	if (GetSCodeFromFile(SCODE_CONF, szSCodeInfo, sizeof(szSCodeInfo)))
    {
        CSYS_ERR("Fail To Get szSCodeInfo From Scode_Conf\n");
        return -1;
    }
	else
	{
	    if (strlen(szSCodeInfo) <= 0)
	    {
	        return -1;
	    }

		p = strchr(szSCodeInfo, '@');
		if (NULL != p)
		{
			memcpy(code, p+1, szSCodeInfo+strlen(szSCodeInfo)-p-1);
			*codeLen = strlen(code);
			ret = 0;
		}
	}

    return ret;
}

//!\brief 获取S码
//!\return  成功为0，失败为-1
int CSysManager::GetSCode(char *code, int *codeLen)
{
    if ((NULL == code) || (*codeLen <= 0) || (0 == m_GetSCodeValid))
    {
        CSYS_ERR("Param Err! code[%p] codeLen[%d]!\n", code, *codeLen);
        return -1;
    }

    int ret = -1;

	/* 获取SCODE_INI配置文件中的数据，有效则返回为S码 */
	char *p = NULL;
	char szSCodeInfo[128] = {0};
	if (GetSCodeFromFile( SCODE_CONF,szSCodeInfo, sizeof(szSCodeInfo)))
    {
        CSYS_ERR("Fail To Get szSCodeInfo From Scode_Conf\n");
        return -1;
    }
	else
	{
	    if (strlen(szSCodeInfo) <= 0)
	    {
	        return -1;
	    }

		p = strchr(szSCodeInfo, '@');
		if (NULL != p)
		{
			memcpy(code, szSCodeInfo, p-szSCodeInfo);
			*codeLen = strlen(code);
			ret = 0;
		}
	}

    return ret;
}

//!\brief 设置S码
//!\return  成功为0，失败为-1
int CSysManager::SetSCode(const char *code, int codeLen)
{
    if ((NULL == code) || (codeLen <= 0) || (0 == m_GetSCodeValid))
    {
        CSYS_ERR("Param Err! code[%p] codeLen[%d]!\n", code, codeLen);
        return -1;
    }
    int ret = 0;
    /*设置S码到SCODE_INI配置文件中@前面的位置 */
    pthread_mutex_lock(&m_MutexSCode);

	char command[128] = {0};
    snprintf(command, sizeof(command), "%s@", code);
    ret = SetSCodeToFile(SCODE_CONF,command);
    if (0 == ret)
    {
        memset(command, 0, sizeof(command));
        snprintf(command,sizeof(command), "cp -rf %s %s", SCODE_CONF, SCODE_BACKUP_CONF);
        mysystem(command);
    }

	pthread_mutex_unlock(&m_MutexSCode);

    return ret;
}


//!\brief 清除S码
//!\return  成功为0，失败为-1
int CSysManager::ClearSCode()
{
    int ret = 0;
    /*设置S码到SCODE_INI配置文件中@前面的位置 */
    pthread_mutex_lock(&m_MutexSCode);

    if (access(SCODE_CONF, F_OK) == 0)
	{
		//清除S码备份
		unlink(SCODE_CONF);
	}

    if (access(SCODE_BACKUP_CONF, F_OK) == 0)
	{
		//清除S码备份
		unlink(SCODE_BACKUP_CONF);
	}

	pthread_mutex_unlock(&m_MutexSCode);

    return ret;
}


//!\brief 根据DID生成SS码
//!\return  成功为0，失败为-1
int CSysManager::CreateSCode(char *uid)
{
	if (NULL == uid )
    {
        CSYS_ERR("Param Err! uid is null!\n");
        return -1;
    }

	if (0 == strlen(uid))
	{
		CSYS_ERR("uid is empty!\n");
        return -1;
	}

	//根据DID和时间生成一个S2,用DES来生成
    int ret = 0;
    int nDataLen = 0;
    char strKey[MAX_DID_LENGTH] = {0};
    char strIn[256] = {0};
    char strOut[256] = {0};
    char command[512] = {0};
    
    pthread_mutex_lock(&m_MutexSCode);

    memset(strKey, 0, sizeof(strKey));
    memset(strIn, 0, sizeof(strIn));
    memset(strOut, 0, sizeof(strOut));
    memset(command, 0, sizeof(command));

    time_t t = time(NULL);
	struct tm curr_tm = {0};
	localtime_r(&t, &curr_tm);
    strftime(strIn, sizeof(strIn) - 1, "S%Y%m%d-%H:%M:%SS", &curr_tm);

	snprintf(strKey, sizeof(strKey), "%s", uid);
	if (',' == strKey[19])
	{
		//只用逗号之前的
		strKey[19] = 0;
	}
    nDataLen = ((strlen(strIn)+1)+7)&0xfffffff8;

    CSYS_INF("strIn[%s] strKey[%s]\n", strIn, strKey);
    int rc =StdDesEncryption((unsigned char*)strIn, (unsigned char*)strOut, nDataLen, 
            (unsigned char*)strKey, ENCRYPTION_MODE);
    memset(strIn, 0, sizeof(strIn));
    base64Encode((unsigned char*)strOut, strIn, nDataLen);
    CSYS_INF("strOut[%s]\n", strIn);

    if (0 == rc)
    {
        snprintf(command, sizeof(command), "@%s", strIn);
        ret = SetSCodeToFile(SCODE_CONF,command);
		if (access(SCODE_BACKUP_CONF, F_OK) == 0)
		{
			unlink(SCODE_BACKUP_CONF);
		}
    }

	pthread_mutex_unlock(&m_MutexSCode);

    return ret;
}

int CSysManager::ResetSCode()
{
    int ret = 0;

	pthread_mutex_lock(&m_MutexSCode);
	m_GetSCodeValid = 0;

	char *p = NULL;
	char szSCodeInfo[128] = {0};
	char szSCode[64] = {0};
	char szSCodeBak[64] = {0};
	char command[512] = {0};
    char fbuf[64] = { 0 };
	if (GetSCodeFromFile(SCODE_CONF,szSCodeInfo, sizeof(szSCodeInfo)))
	{
		pthread_mutex_unlock(&m_MutexSCode);
		CSYS_ERR("Fail To Get szSCodeAll From Scode_Conf\n");
		return -1;
	}
	else
	{
	    if (strlen(szSCodeInfo) <= 0)
	    {
	    	pthread_mutex_unlock(&m_MutexSCode);
	        return -1;
	    }

		p = strchr(szSCodeInfo, '@');
		if (NULL != p)
		{
			memcpy(szSCode, szSCodeInfo, p-szSCodeInfo);
			memcpy(szSCodeBak, p+1, szSCodeInfo+strlen(szSCodeInfo)-p-1);
		}
	}

	if (strlen(szSCode) > 0)
	{
		snprintf(command, sizeof(command), "@%s", szSCode);
		ret = SetSCodeToFile(SCODE_CONF,command);
        if (access(SCODE_BACKUP_CONF, F_OK) == 0)
        {
            unlink(SCODE_BACKUP_CONF);
        }
		
	}
    else
    {
        //若S2存在直接返回，不存在则根据DID和时间生成一个S2,用DES来生成
        if (strlen(szSCodeBak) <= 0 || (0 == strncmp(szSCodeBak, "1", strlen("1"))))
        {
            //根据DID和时间生成一个S2,用DES来生成
            int nDataLen = 0;
            char strKey[MAX_DID_LENGTH] = {0};
            char strIn[256] = {0};
            char strOut[256] = {0};

            memset(strKey, 0, sizeof(strKey));
            memset(strIn, 0, sizeof(strIn));
            memset(strOut, 0, sizeof(strOut));

            time_t t = time(NULL);
			struct tm curr_tm = {0};
			localtime_r(&t, &curr_tm);
            strftime(strIn, sizeof(strIn) - 1, "S%Y%m%d-%H:%M:%SS", &curr_tm);
            FILE *file_new = NULL;
            int nTutkUID_Enable = 0;
            char strTutkUID[MAX_DID_LENGTH] = {0};
        	file_new = fopen(TutkUID_CONF, "r");
        	if (NULL != file_new)
        	{
                fread(fbuf, sizeof(char), sizeof(fbuf), file_new);
		        sscanf(fbuf, "%d:%s", &nTutkUID_Enable, strTutkUID);	
        		// fscanf(file_new, "%d:%s", &nTutkUID_Enable, strTutkUID);	//如果m_sTutkUID之前有数据，只会覆盖一部分，因此需要清零该字符
        		fflush(file_new);
        		fclose(file_new);
                if (strlen(strTutkUID) >= 20)
                {
                    char did[MAX_DID_LENGTH] = {0};
                    if (',' == strTutkUID[19])//尚云DID号在逗号前面
                    {
                        strncpy(did, strTutkUID, 19);
                        snprintf(strKey, sizeof(strKey), "%s", did);
                    }
                    else
                    {
                        snprintf(strKey, sizeof(strKey), "%s", strTutkUID);
                    }

                }
        	}
            if (0 == strlen(strKey))
            {
                ZWSys_GetTUTKUID(strKey);
            }

			if (0 == strlen(strKey))
			{
				pthread_mutex_unlock(&m_MutexSCode);

				CSYS_INF("UID is Empty\n");
	        	return 0;
			}

			if (',' == strKey[19])
			{
				//只用逗号之前的
				strKey[19] = 0;
			}
            
            nDataLen = ((strlen(strIn)+1)+7)&0xfffffff8;

            CSYS_INF("strIn[%s] strKey[%s]\n", strIn, strKey);
            int rc =StdDesEncryption((unsigned char*)strIn, (unsigned char*)strOut, nDataLen, 
                    (unsigned char*)strKey, ENCRYPTION_MODE);
            memset(strIn, 0, sizeof(strIn));
            base64Encode((unsigned char*)strOut, strIn, nDataLen);
            CSYS_INF("strOut[%s]\n", strIn);

            if (0 == rc)
            {
                snprintf(command, sizeof(command), "@%s", strIn);
                ret = SetSCodeToFile(SCODE_CONF,command);
				if (access(SCODE_BACKUP_CONF, F_OK) == 0)
				{
					unlink(SCODE_BACKUP_CONF);
				}
            }
        }
    }
	pthread_mutex_unlock(&m_MutexSCode);
	
    return ret;
}
#endif

//获取设备启动时间
int CSysManager::GetDeviceOpenTime(long long *deviceOpenTime)
{
	FILE* fp = NULL;
    char fbuf[16] = { 0 };
	fp = fopen("/proc/uptime", "r");
	if (fp != NULL)
	{
        fread(fbuf, sizeof(char), sizeof(fbuf), fp);
		sscanf(fbuf, "%lld", deviceOpenTime);
		// fscanf(fp, "%lld", deviceOpenTime);
		fclose(fp);
	}
	else
	{
		CSYS_ERR("read file /proc/uptime failed!\n");
		return -1;
	}

	return 0;
}

CSysManager::CSysManager()
{	
#ifndef NO_USE_BUZZER	
    //蜂鸣器
    theCBuzzer = NULL;
#endif	

	//没有使用
//	theCSysStart = NULL;
	
	//IP复位
	theCIP2PasswdResume = NULL;
	
	
	//看门狗
	theCWatchDog = NULL;
	
	//验证
	theCBoardAuthen = NULL;

	m_configChanged =0;//NTP状态改变标识位初始化
	
	m_TimeCountFlag=1;
	m_TimeToSyncFlag=0;
    m_CheckNetThreadExitFlag=0;
    m_ntp_request_is_success = 0;
#if defined (__IS_SUPPORT_APP_SHARE__)
	m_MutexSCode = PTHREAD_MUTEX_INITIALIZER;
	m_GetSCodeValid = 0;
#endif
}

CSysManager::~CSysManager()
{
}



