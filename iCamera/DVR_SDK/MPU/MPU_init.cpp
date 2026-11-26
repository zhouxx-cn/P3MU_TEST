
/*!
 *\file		MPU_init.cpp
 *\brief	MPU模块，初始化相关API实现
 *\author	林云
 *\date		2009/04/09
 */

#include "MPU_global.h"
#include "PUB/macro_pthread.h"
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include "mysystem.h"
#include "MPU_debug.h"
#include "Authority.h"
#include "PUB_Language.h"
#include "common_pub.h"


/**************************************************************************
 *                                                     声明局部宏定义                                                  *
 **************************************************************************/

//调用初始化动作
#define MPU_CALL(x)																\
do																				\
{																				\
	nRet = x;																		\
																				\
	if (IDVR_OK != nRet)															\
	{																			\
		MPU_ERR("%s, return %d\n", #x, nRet);										\
		MPU_CHK(IDVR_OK == nRet);												\
	}																			\
}																				\
while (0)


/**************************************************************************
 *                                                     声明全局变量                                                 *
 **************************************************************************/

PMPU_MANAGER_INFO g_pMpuInfo = NULL;

//是否启动关机过程
BOOL g_bShutdown = FALSE;

#ifdef __IS_SUPPORT_CLOUDWORLD__
int g_CloudStorageMode = 0;
#endif

/* 线程管理资源 */
typedef struct
{
    int				m_Exit;
    pthread_t		m_ThreadId;			//线程ID
    int				m_Pri;
}ThreadInfo_t;

static int CreatThread(ThreadInfo_t *pThInfo, int Pri, void *(*start_routine)(void*))
{
    pthread_attr_t		attr;
    struct sched_param 	param;
    int					RetValue;

    if (Pri == 0)
    {
        pthread_attr_init(&attr);
        pthread_attr_setschedpolicy(&attr, SCHED_OTHER);
        pthread_attr_getschedparam(&attr, &param);
        param.sched_priority = 0;
        pthread_attr_setschedparam(&attr, &param);
        RetValue = pthread_create(&pThInfo->m_ThreadId, &attr, start_routine, pThInfo);
        pthread_attr_destroy(&attr);
    }	
    else
    {
        pthread_attr_init(&attr);
        pthread_attr_setschedpolicy(&attr, SCHED_RR );
        pthread_attr_getschedparam(&attr, &param);
        param.sched_priority = Pri;
        pthread_attr_setschedparam(&attr, &param);
        RetValue = pthread_create(&pThInfo->m_ThreadId, &attr, start_routine, pThInfo);
        pthread_attr_destroy(&attr);
    }

    if (RetValue != 0)
        pThInfo->m_ThreadId = 0;
    return RetValue;
}

#ifdef __IS_SUPPORT_CLOUDWORLD__
int MPU_SetCloudStorageMode(int mode)
{
    if (g_CloudStorageMode != mode)
    {
        g_CloudStorageMode = mode;
    }

    return 0;
}

int MPU_GetCloudStorageMode(int *mode)
{
    *mode = g_CloudStorageMode;
    return 0;
}

// 存在定制项的产品，优先以 CLOUD_STORAGE_MODE_DOMESTIC_FLAG和 CLOUD_STORAGE_MODE_FOREIGN_FLAG为准
int GetCustomCloudStorageMode()
{
    int ret = -1;
    int nCustomMode = -1;
    int nFlagMode = -1;
    int nTrueMode = -1;

    CConfig *pCustomModecfg = CConfig::NewInstance();
    if(pCustomModecfg)
    {
        ret = pCustomModecfg->LoadFileRO(CUSTOM_MODE_FILE);
        if(0 == ret)
        {    
            nCustomMode = pCustomModecfg->GetValue("CloudStorageCfg", "ForceCloudStorageMode", -1);
        }
        CConfig::ReleaseInstance(pCustomModecfg);
    }

    if (access(CLOUD_STORAGE_MODE_DOMESTIC_FLAG, F_OK) == 0)
    {	
        nFlagMode = CLOUD_STORAGE_MODE_DOMESTIC;
    }
    else if (access(CLOUD_STORAGE_MODE_FOREIGN_FLAG, F_OK) == 0)
    {
        nFlagMode = CLOUD_STORAGE_MODE_FOREIGN;
    }

    if (nFlagMode != -1)
    {
    	
        nTrueMode = nFlagMode;
    }
    else
    {
        nTrueMode = nCustomMode;
    }

    if (nTrueMode <= -1)
    {
        nTrueMode = CLOUD_STORAGE_MODE_UNKNOW;
    }

    MPU_ERR("Cloud mode: custom:%d, flag:%d, true:%d\n", nCustomMode, nFlagMode, nTrueMode);
    return nTrueMode;
}
#endif

/* 加载协议接口线程 */
static void *LoadProtocol(void *param)
{
    PRT_TID();
    pthread_detach(pthread_self());

    BEGINRUN_THREAD;

    int nSDKRet;       	//启动远程服务	
    int checkCount = 0;//超时时间

	//等待私有协议加载了再启动其它协议
	while(access("/tmp/.hieserverStarted", 0))
	{
		sleep(1);
		checkCount++;
		if(checkCount >= 300)//5 分钟
			break;
	}

	mysystem("rm -rf /tmp/.hieserverStarted");
	MPU_DBG("check /tmp/.hieserverStarted\n");
	sleep(8);
    MPU_DBG("---MPU_CIU_Init.cpp-----CIU_Init()----...HieServer_AttachProtocol();........启动远程服务............\n");	 

    //字段值为1，启用此协议,由定制控制
    int CGIValid = 1;
    int RTSPValid = 1;
    int OnvifValid = 1;
    int TUTKValid = 1;
    int GB28181Valid = 1;
    int bSupportCloudStorage = 0;

    //获取新的配置文件读取实例
    CConfig *pCfg = CConfig::NewInstance();
	nSDKRet = pCfg->LoadFileRO(CUSTOM_ABILITY_FILE);
    if (0 == nSDKRet)
    {
        if (pCfg->CheckKeyExist("FunctionSupport", "IsSupportCloudStorage"))
        {
            bSupportCloudStorage = pCfg->GetValue("FunctionSupport", "IsSupportCloudStorage", bSupportCloudStorage);
        }
    }
    CConfig::ReleaseInstance(pCfg);
    pCfg = NULL;

    pCfg = CConfig::NewInstance();
    if (pCfg)
    {
		nSDKRet = pCfg->LoadFileRO(CUSTOM_INFO_FILE);
		if (0 == nSDKRet)
		{
			CGIValid = pCfg->GetValue("NetProtocol", "bCGIValid", 1);
			RTSPValid = pCfg->GetValue("NetProtocol", "bRTSPValid", 1);
			OnvifValid = pCfg->GetValue("NetProtocol", "bOnvifValid", 1);
			TUTKValid = pCfg->GetValue("NetProtocol", "bTUTKValid", 1);
            GB28181Valid = pCfg->GetValue("NetProtocol", "bGB28181Valid", 1);
		}
		CConfig::ReleaseInstance(pCfg);
    }

#if defined (__PRODUCT_P4U__) || defined (__PRODUCT_P4__)
    /* P4/P4U强制支持onvif，绕过配置 */
    RTSPValid = 1;
    OnvifValid = 1;
#endif

    if (CGIValid) {
        HieServer_LoadProtocol(eProtocolCGI);
    }

    if (RTSPValid) {
        HieServer_LoadProtocol(eProtocolRTSP);
    }

    if (OnvifValid) {
        HieServer_LoadProtocol(eProtocolOnvif);
    }

    if(TUTKValid)
    {
#ifndef __MODULE_4G_SUPPORT__	
		HieServer_LoadProtocol(eProtocolTUTK);
#else
		int bSupport4G = 0;
		ZWSys_Get4GSupport(&bSupport4G);
		if (0 == bSupport4G)
		{
			HieServer_LoadProtocol(eProtocolTUTK);
		}
#endif
    }

    if (GB28181Valid) {
        HieServer_LoadProtocol(eProtocolGB28181);
    }

#if defined (__IS_SUPPORT_P6SMESSAGE__)
    HieServer_LoadProtocol(eProtocolP6SMessagePush);
#endif

#ifdef __IS_SUPPORT_CLOUDWORLD__
	if(access(CLOUD_STORAGE_MODE_DOMESTIC_FLAG, F_OK) == 0)
	{
		HieServer_LoadProtocol(eProtocolCloudStorage);
		MPU_SetCloudStorageMode(CLOUD_STORAGE_MODE_DOMESTIC);
	}
	else if( access(CLOUD_STORAGE_MODE_FOREIGN_FLAG, F_OK) == 0)
	{
		HieServer_LoadProtocol(eProtocolCloudStorage);
		MPU_SetCloudStorageMode(CLOUD_STORAGE_MODE_FOREIGN);
	}
#else
	if (bSupportCloudStorage)
	{	
		HieServer_LoadProtocol(eProtocolCloudStorage);
	}
#endif

	//标记其它协议启动完,端口检测机制可以去检测
	mysystem("touch /tmp/.otherProtocolStarted");

    ENDRUN_THREAD;	
    return NULL;
}

extern int SaveUsrInfoIntoCFG(void);

static void *mpu_init_thread(void *param)
{
    PRT_TID();
	BEGINRUN_THREAD;

	int nRet;
    ThreadInfo_t LoaderProtocolThread;    

	CreatThread(&LoaderProtocolThread, 20, LoadProtocol);

	//初始化系统SDK
	MPU_DBG("[sofware]...mpu_init_thread....初始化系统SDK..\n");
	PUB_LanguageInit();

	nRet = ZWSys_Init(0);
	if (0 != nRet)
	{
		MPU_ERR("\n[MPU] ZWSys_Init fail, nRet = %d, need reboot !\n", nRet);
		MPU_PAR_SaveConfigReal();
		#ifdef __SUPPORT_SD_CARD__
		MPU_AVI_StopRecordAndUinitMemRecord(0);
		#endif
		PAR_ForbiddenSaveFile();
		mysystem("reboot");
	}

#if defined(__IS_SUPPORT_LCD_MANAGER__)
    //运行显示屏初始化
    MPU_CALL(LCD_Init());
#endif

	CConfig *pcfg = CConfig::GetInstance();
    if (pcfg)
    {
        //设置音量
        nRet = pcfg->GetValue("AudioIn", "AudioVol", 90);
        ZWSys_SetAudioInVol(ZWSys_EncChannelOpen(0), nRet);
        nRet = pcfg->GetValue("AudioOut", "AudioVol", 95);
		ZWSys_SetAudioOutVol(ZWSys_EncChannelOpen(0), nRet, 1);
    
        int counter = 0;
        char szSCode[64] = {0};
        int nSCodeLen = sizeof(szSCode);
        int rc = MPU_SYS_DeviceBind_GetSCode(szSCode, &nSCodeLen);
        
#ifndef __MODULE_4G_SUPPORT__	
        if ((0 != rc) || (0 == strlen(szSCode)))
#else
        int nSupportBoardCast = 0;
        int ret = ZWSys_GetBroadCastEnable(&nSupportBoardCast);
        MPU_INFO("rc :%d szSCode :%s - nSupportBoardCast = %d ret = %d \n", rc, szSCode, nSupportBoardCast, ret);
        if (((0 != rc) || (0 == strlen(szSCode))) || ((ret == 0) && nSupportBoardCast))
#endif
        {
#ifdef __MODULE_4G_SUPPORT__
            ZWSys_SetSpeechContent(SPEECH_4G_SYS_STARTING);
#else
            ZWSys_SetSpeechContent(SPEECH_SYS_STARTING);
#endif
            //循环查询，等待播放结束
            while (1)
            {
                sleep(1);
                if (0 == ZWSys_GetSpeechPlayingStatus())
                {
                    break;
                }
                if (++counter > 10)
                {
                    break;
                }
            }
        }
    }

	//运行日志初始化
	//MPU_DBG("\n....mpu_init_thread......//进入运行日志初始...............\n");
	MPU_CALL(OPL_Init());

	//初始化用户管理模块
	//MPU_DBG("\n....mpu_init_thread......//进入初始化用户管理模块...............\n");
	MPU_CALL(USR_Init());

    SaveUsrInfoIntoCFG();

	//初始化预览管理模块
	//MPU_DBG("\n....mpu_init_thread......//初始化预览管理模块..............\n");
	MPU_CALL(PRW_Init());

	ENDRUN_THREAD;
	return NULL;
}

//!\brief   MPU模块初始化
//!\return  成功为0，其它为错误码
int MPU_Init()
{
	int nRet;

	pthread_t thrdID;			//线程ID

	//MPU_INFO("enter\n");

	g_pMpuInfo = (PMPU_MANAGER_INFO) malloc(sizeof (MPU_MANAGER_INFO));
	if (NULL == g_pMpuInfo)
	{
		MPU_ERR("new %d bytes, g_pMpuInfo\n", sizeof (MPU_MANAGER_INFO));
		MPU_CHK(NULL != g_pMpuInfo);
		return IDVR_ERROR_ALLOC;
	}

	//初始化全局变量
	memset(g_pMpuInfo, 0, sizeof (MPU_MANAGER_INFO));

	nRet = pthread_create(&thrdID, NULL, mpu_init_thread, NULL);
	if (0 == nRet)
	{
		nRet = pthread_join(thrdID, NULL);
		if (0 != nRet)
		{
			perror("pthread_join()");
			MPU_ERR("join mpu_init_thread fail, nRet = %d\n", nRet);
		}
	}
	else
	{
		perror("pthread_create()");
		MPU_ERR("create mpu_init_thread fail, nRet = %d\n", nRet);

		mpu_init_thread(NULL);
	}

	MPU_DBG("out, return %d\n", nRet);
	return nRet;
}

static void *mpu_init2_thread(void *param)
{
    PRT_TID();

    BEGINRUN_THREAD;
 
    int nRet;

    //初始化存储管理
    MPU_DBG("\n........MPU_init.cpp.....1........STR_Init...初始化存储管理\n");
    MPU_CALL(STR_Init());

    //初始化系统管理
    MPU_DBG("\n......MPU_init.cpp.......3.....SYS_Init......初始化系统管理\n");
    MPU_CALL(SYS_Init());

    MPU_SYS_Led(NORMAL_HZ);

    //初始化通道管理
    MPU_DBG("\n........MPU_init.cpp....5.......AVL_Init..初始化通道管理\n");
    MPU_CALL(AVL_Init());

    //初始化网络
    MPU_DBG("\n......MPU_init.cpp.......7...........NET_Init.初始化网络...\n");
    MPU_CALL(NET_Init());

    //初始化PPPOE
    MPU_DBG("\n......MPU_init.cpp........8.........PPPoE_Init...初始化PPPOE\n");
    MPU_CALL(PPPoE_Init());

    //初始化DDNS
    MPU_DBG("\n......MPU_init.cpp........9..........DDNS_Init...初始化DDNS\n");
    MPU_CALL(DDNS_Init());

    //初始化NTP
    MPU_DBG("\n......MPU_init.cpp........10..........NTP_Init...初始化NTP\n");
    MPU_CALL(NTP_Init());

    //初始化CIU
    MPU_DBG("\n........MPU_init.cpp......11.....CIU_Init.....初始化CIU\n");
    MPU_CALL(CIU_Init());

    //初始化CIU
    MPU_CALL(NMS_Init());
    MPU_CALL(ISP_Init());

    MPU_DBG("//all init");

    ENDRUN_THREAD;
    return NULL;
}

//!\brief   MPU模块初始化
//!\return  成功为0，其它为错误码
int MPU_Init2()
{
	int nRet;

	pthread_t thrdID;			//线程ID

	MPU_DBG("enter\n");

	nRet = pthread_create(&thrdID, NULL, mpu_init2_thread, NULL);
	if (0 == nRet)
	{
		nRet = pthread_join(thrdID, NULL);
		if (0 != nRet)
		{
			perror("pthread_join()");
			MPU_ERR("join mpu_init2_thread fail, nRet = %d\n", nRet);
		}
	}
	else
	{
		perror("pthread_create()");
		MPU_ERR("create mpu_init2_thread fail, nRet = %d\n", nRet);

		mpu_init2_thread(NULL);
	}

	MPU_DBG("out, return %d\n", nRet);
	return nRet;
}

//!\brief   MPU模块逆初始化
//!\return  成功为0，其它为错误码
int MPU_Uninit()
{
	int nRet, nCh, nCount,nSDKRet;

	MPU_DBG("enter\n");

	//进入关机过程
	g_bShutdown = TRUE;
	MPU_CALL(PAR_LockBakFile());

	//先暂停录像
	nCount = MAX_ENC_CHANNEL_NUM;
	for (nCh = 0; nCh < nCount; ++nCh)
	{
		MPU_CALL(MPU_AVL_Record_Pause(nCh, TRUE));
	}

	//关闭预览声音
	MPU_CALL(MPU_PRW_StopAudioPreview());

	//逆初始化CIU
	MPU_DBG("\n----------CIU_Uninit---------\n");
	
#if 1
    char *p = NULL;

    *p = 'a';
    exit(1);
#endif

	MPU_CALL(CIU_Uninit());

	//逆初始化报警输出
	MPU_DBG("\n----------DOP_Uninit---------\n");
	MPU_CALL(DOP_Uninit());

	//逆初始化报警输入
	MPU_DBG("\n----------DIP_Uninit---------\n");
	MPU_CALL(DIP_Uninit());

	//逆初始化NTP
	MPU_DBG("\n----------NTP_Uninit---------\n");
	MPU_CALL(NTP_Uninit());

	//逆初始化DDNS
	MPU_DBG("\n----------DDNS_Uninit---------\n");
	MPU_CALL(DDNS_Uninit());

	//逆初始化网络
	MPU_DBG("\n----------NET_Uninit---------\n");
	MPU_CALL(NET_Uninit());

	//逆初始化系统管理
	MPU_CALL(SYS_Uninit());

	//逆初始化通道管理
	MPU_DBG("\n----------AVL_Uninit---------\n");
	MPU_CALL(AVL_Uninit());

	//逆初始化存储管理
	MPU_DBG("\n----------STR_Uninit---------\n");
	MPU_CALL(STR_Uninit());

	//逆初始化PTZ模块
	MPU_CALL(PTZ_Uninit());

	//逆初始化预览管理模块
	MPU_CALL(PRW_Uninit());

	//逆初始化用户管理模块
	MPU_CALL(USR_Uninit());

	//运行日志逆初始化
	MPU_CALL(OPL_Uninit());

	//逆初始化参数管理模块
	MPU_CALL(PAR_Uninit());

	//逆初始化系统SDK
	MPU_DBG("\n-----逆初始化系统SDK--------ZWSys_DeInit()---------\n");
	nSDKRet = ZWSys_DeInit();
	MPU_DBG("call ZWSys_DeInit(), return %d\n", nSDKRet);
	MPU_DBG("\n-----逆初始化系统SDK完成---------ZWSys_DeInit()--------\n");

	MPU_DBG("out, return %d\n", nRet);
	return nRet;
}


/**************************************************************************
 *                            注销局部宏定义                              *
 **************************************************************************/
#undef MPU_CALL					//调用初始化动作
