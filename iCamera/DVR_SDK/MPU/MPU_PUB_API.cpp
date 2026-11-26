
/*!
 *\file        MPU_PUB_API.cpp
 *\brief    MPU模块，公共基础相关API实现
 *\author    林云
 *\date        2009/03/20

功能：
-#    全局函数定义
 */

#include "MPU_global.h"
#include "ddt_so.h"
#include "PUB/mysystem.h"
#include "PUB/ConvertCharset.h"
#include "MPU_PRW_API.h"
#include "COsd.h"
#include "../PAR/CPubFunc.h"
#include "common_pub.h"
#if defined (__IS_SUPPORT_LCD_MANAGER__)
#include "MPU_LCD_API.h"
#endif
#include "PUB_BoradInfo.h"

/* 消除本文件中的snprintf截断警告 */
#pragma GCC diagnostic ignored "-Wformat-truncation"

#define CLOUD_STORAGE_INFO_SECTION            "CloudStorage"
#define CLOUD_STORAGE_INFO_DEVID_KEY        "DevId"
#define CLOUD_STORAGE_INFO_DOMAIN_NAME_KEY    "DomainName"
#define CLOUD_STORAGE_INFO_ENABLE_KEY        "Enable"
#define CLOUD_STORAGE_INFO_SUBSCRIBE_STATUS_KEY    "SubscribeStatus"
#define CLOUD_STORAGE_INFO_DEALER_ID_KEY    "DealerID"
#define CLOUD_STORAGE_INFO_STREAM_MODE_KEY    "StreamMode"
#define CLOUD_STORAGE_INFO_RECORD_MODE_KEY    "RecordMode"

#define CLOUD_STORAGE_INFO_DEVID_BAK_FILE    "/mnt/mtd/config/.CloudStorageDevIdBak"
#define CLOUD_STORAGE_INFO_DOMAIN_NAME_BAK_FILE    "/mnt/mtd/config/.CloudStorageDomainBak"

static const char *s_szCloudStorageScheduletime = "111111111111111111111111111111111111111111111111";


#ifdef __IS_SUPPORT_CLOUDWORLD__
#define CLOUD_STORAGE_MODE_KEY              "CloudMode"
#endif


//面板回调指针
static PANEL_CALLBACK s_pCallback_Panel = NULL;

static int s_bGetDevCap_pub = FALSE;

static DeviceDescription s_tDevCap_pub = {0};

static stuDeviceFunctionMap s_tDeviceFunc[32] = {
    {"telnetd_enable",  0},
    {"telnetd_disable", 1}
    /* 由于是透传，所以可以在后面添加其他的命令 */
};

//当前升级模式
static UPGRADE_MODE s_enUpgradeMode = NOT_UPGRADE_MODE;
static pthread_mutex_t s_mutexUpgradeMode = PTHREAD_MUTEX_INITIALIZER;

//解析透传过来的数据，兼容以前没有头的的格式
static int DeviceFunctionMapFunc(const char *pFunctionString)
{
    if (!pFunctionString)
    {
        MPU_ERR("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    char command[64] = {0};

    //检查老的格式是否有有效数据
    int iItemCount = sizeof(s_tDeviceFunc)/sizeof(s_tDeviceFunc[0]);
    for (int i=0; i<iItemCount; i++)
    {
        if (0 == strcmp(pFunctionString, s_tDeviceFunc[i].sFunctionString))
        {
            switch(s_tDeviceFunc[i].eDeviceEnum)
            {
                case 0:
                {
                    mysystem("telnetd&");
                    usleep(100000);
                    memset(command, 0, sizeof(command));
                    snprintf(command, sizeof(command), "touch %s", P2PUDAnyFile);
                    mysystem(command);
                    break;
                }

                case 1:
                {
                    mysystem("killall -9 telnetd");
                    usleep(100000);
                    memset(command, 0, sizeof(command));
                    snprintf(command, sizeof(command), "rm -rf %s", P2PUDAnyFile);
                    mysystem(command);
                    break;
                }

                default:
                {
                    MPU_ERR("Not Support operate %d\n", (s_tDeviceFunc[i].eDeviceEnum));
                    return -1;
                }
            }
            return 0;
        }
    }

    //检查采用新格式的数据
    char *pMovePoint = (char *)pFunctionString;
    char *pFind = NULL;
    int majorComm = 0;
    int minorComm = 0;
    int dataLen = 0;
    int ret = 0;

    //找主命令
    pFind = strcasestr((char *)pFunctionString, TRANSPARENT_DATA_MAIN_COMM);
    if(NULL == pFind)
    {
        MPU_ERR("Not find %s!\n", TRANSPARENT_DATA_MAIN_COMM);
        return -1;
    }
    pMovePoint = pFind + (strlen(TRANSPARENT_DATA_MAIN_COMM) + 1);
    sscanf(pMovePoint, "%d", &majorComm);

    //找次命令
    pFind = strcasestr((char *)pFunctionString, TRANSPARENT_DATA_MINOR_COMM);
    if(NULL == pFind)
    {
        MPU_ERR("Not find %s!\n", TRANSPARENT_DATA_MINOR_COMM);
        return -1;
    }
    pMovePoint = pFind + (strlen(TRANSPARENT_DATA_MINOR_COMM) + 1);
    sscanf(pMovePoint, "%d", &minorComm);

    //找数据长度
    pFind = strcasestr((char *)pFunctionString, TRANSPARENT_DATA_LEN);
    if(NULL == pFind)
    {
        MPU_ERR("Not find %s!\n", TRANSPARENT_DATA_LEN);
        return -1;
    }
    pMovePoint = pFind + (strlen(TRANSPARENT_DATA_LEN) + 1);
    sscanf(pMovePoint, "%d", &dataLen);

    //找数据起始位置
    pFind = strcasestr((char *)pFunctionString, TRANSPARENT_DATA_CONTENET_HEAD);
    if(NULL == pFind)
    {
        MPU_ERR("Not find %s!\n", TRANSPARENT_DATA_CONTENET_HEAD);
        return -1;
    }
    pMovePoint = pFind + strlen(TRANSPARENT_DATA_CONTENET_HEAD) + 4;

    //printf("majorComm=%d, minorComm=%d, dataLen=%d\n", majorComm, minorComm, dataLen);
    //printf("0x%x-0x%x-0x%x-0x%x\n", pMovePoint[32], pMovePoint[33], pMovePoint[34], pMovePoint[35]);

    switch(majorComm)
    {
        case 1:
            if (1 == minorComm)//传音频报警定制声音
            {
                //设置到底层
                ret = MPU_AVL_SetAudioCustomVoice(0, 0, pMovePoint, dataLen);
                if (ret) {
                    MPU_ERR("MPU_AVL_SetAudioCustomVoice fail! minorComm[%d]\n", minorComm);
                    return -1;
                }
            }
#if defined (__IS_SUPPORT_REMINDER_SCHEDULE__)
            else if (26 == minorComm)// 定时提醒自定义语音1
            {
                //设置到底层
                ret = MPU_AVL_SetAudioCustomVoice(0, 1, pMovePoint, dataLen);
                if (ret) {
                    MPU_ERR("MPU_AVL_SetAudioCustomVoice fail! minorComm[%d]\n", minorComm);
                    return -1;
                }
            }
            else if (27 == minorComm)// 定时提醒自定义语音2
            {
                //设置到底层
                ret = MPU_AVL_SetAudioCustomVoice(0, 2, pMovePoint, dataLen);
                if (ret) {
                    MPU_ERR("MPU_AVL_SetAudioCustomVoice fail! minorComm[%d]\n", minorComm);
                    return -1;
                }
            }
            else if (28 == minorComm)// 定时提醒自定义语音3
            {
                //设置到底层
                ret = MPU_AVL_SetAudioCustomVoice(0, 3, pMovePoint, dataLen);
                if (ret) {
                    MPU_ERR("MPU_AVL_SetAudioCustomVoice fail! minorComm[%d]\n", minorComm);
                    return -1;
                }
            }
#endif
#if defined (__IS_SUPPORT_LCD_MANAGER__)
            else if (29 == minorComm)// LCD背景图
            {
                //设置到底层
                ret = MPU_LCD_SetBackgroundImageData((const char *)pMovePoint, dataLen);
                if (ret) {
                    MPU_ERR("MPU_LCD_SetBackgroundImageData fail! minorComm[%d]\n", minorComm);
                    return -1;
                }
            }
#endif
            else
            {
                MPU_ERR("not support majorComm=%d, minorComm=%d\n", majorComm, minorComm);
                return -1;
            }
            break;
        default:
            MPU_ERR("not support majorComm=%d, minorComm=%d\n", majorComm, minorComm);
            return -1;
    }

    return 0;
}

static void GetDeviceDescription()
{
    if (!s_bGetDevCap_pub)
    {
        int nSDKRet;
        int nBoardID = 0;

        //获取板型
        ZWSys_GetBoardInfo(&nBoardID);
        nSDKRet = EDVRS_DDT_GetDeviceDescriptionByID(&s_tDevCap_pub, nBoardID);
        if (0 == nSDKRet)
        {
            s_bGetDevCap_pub = TRUE;
        }
        else
        {
            //读取配置文件失败
            MPU_ERR("EDVRS_DDT_GetDeviceDescriptionByID, nBoardID = 0x%x, return %d\n", (int)nBoardID, nSDKRet);
        }
    }
}

//!\brief       获取面板ID
//!\return  面板ID
int PUB_GetDvrID()
{
    if (NULL != s_pCallback_Panel)
    {
        return (*s_pCallback_Panel) (PANEL_CMD_GETID, 0);
    }

    MPU_DBG("not set callback\n");
    return -1;
}

//!\brief       设置面板ID
//!\param   nID 新的面板ID
//!\return  0表示成功，其它为错误码
int PUB_SetDvrID(int nID)
{
    int nRet = -1;

    if (NULL != s_pCallback_Panel)
    {
        nRet = (*s_pCallback_Panel) (PANEL_CMD_SETID, nID);
    }

    return nRet;
}

//!\brief       设置面板操作回调函数
//!\param   pCallback   回调函数指针
//!\return  0表示成功，其它为错误码
int MPU_PUB_SetPanelCallback(PANEL_CALLBACK pCallback)
{
    s_pCallback_Panel = pCallback;

    return IDVR_OK;
}

//!\brief   获取设备信息
//!\param   pDI             设备信息
//!\return  0表示成功，其它为错误码
int MPU_PUB_GetDeviceInfo(PDEVICEINFO pDI)
{
    //判断指针型参数是否合法
    MPU_CHECK_POINTER_PARAMETER(pDI);

    //获取配置文件管理器实例指针
    CConfig *pCfg = CConfig::GetInstance();

    if (NULL != pCfg)
    {
        //从配置文件初始化
        pDI->m_nAutoLockScreenTime = pCfg->GetValue("PUB", "AutoLockScreenTime", 60);
        pDI->m_nMenuLanguage = pCfg->GetValue("PUB", "MenuLanguage", 0);
        pDI->m_nDateFormat = pCfg->GetValue("PUB", "DateFormat", 0);
        pDI->m_nTimeFormat = pCfg->GetValue("PUB", "TimeFormat", 0);
    }
    else
    {
        //读取配置文件失败
        MPU_ERR("no CConfig instance\n");
        MPU_CHK(NULL != pCfg);

        memset(pDI, 0, sizeof (DEVICEINFO));

        pDI->m_nAutoLockScreenTime = 60;
    }

    return IDVR_OK;
}

//!\brief   设置设备信息
//!\param   pDI             设备信息
//!\return  0表示成功，其它为错误码
int MPU_PUB_SetDeviceInfo(PDEVICEINFO pDI)
{
    int nRet;

    //判断指针型参数是否合法
    MPU_CHECK_POINTER_PARAMETER(pDI);

    //从配置文件初始化
    CConfig *pCfg = CConfig::GetInstance();

    if (NULL != pCfg)
    {
        //保存到配置文件
        pCfg->SetValue("PUB", "AutoLockScreenTime", pDI->m_nAutoLockScreenTime);
        pCfg->SetValue("PUB", "MenuLanguage", pDI->m_nMenuLanguage);
        pCfg->SetValue("PUB", "DateFormat", pDI->m_nDateFormat);
        pCfg->SetValue("PUB", "TimeFormat", pDI->m_nTimeFormat);

        nRet = IDVR_OK;
    }
    else
    {
        //读取配置文件失败
        MPU_ERR("no CConfig instance\n");
        MPU_CHK(NULL != pCfg);

        nRet = IDVR_ERROR_CONFIGFILE;
    }

    return nRet;
}

//!\brief   获取设备能力
//!\param   pDC             设备能力
//!\return  0表示成功，其它为错误码
int MPU_PUB_GetDeviceCap(PDEVICECAP pDC)
{
    //判断指针型参数是否合法
    MPU_CHECK_POINTER_PARAMETER(pDC);
    memset(pDC, 0, sizeof (DEVICECAP));

    GetDeviceDescription();

    if (s_bGetDevCap_pub)
    {
        strncpy(pDC->m_szTypeName, s_tDevCap_pub.szTypeName, sizeof (pDC->m_szTypeName) - 1);
#ifdef __MODULE_4G_SUPPORT__
        int count = 0;
        int nActiveStutusCode = 0;
        unsigned int nAuthState = 0;
        unsigned char szAuthCode[256] = {0};
        int nBoardID = 0;
        const char *nBoardType;

        /* 获取当前设备的版型ID */
        ZWSys_GetBoardInfo(&nBoardID);
        /* 获取当前设备的版型类型 */
        nBoardType = PUB_FindBoardTypeInfoById(nBoardID);
        /* 获取当前设备的授权状态 */
        ZWSys_GetAuthInfo(szAuthCode, &nAuthState);
        /* 获取当前设备的激活码 */
        ZWSys_GetActiveStatusCode(&nActiveStutusCode);

        if ((strlen(nBoardType) == strlen(pDC->m_szTypeName)) && (0 == strcmp(pDC->m_szTypeName, nBoardType)))
        {
            if (1 == nAuthState)
            {
                if (nActiveStutusCode & CAPABILITY_EXTERNSIMCARD)
                {
                    InsertStringAtFirstDash(nBoardType, "L", pDC->m_szTypeName, sizeof(pDC->m_szTypeName));
                }
                else
                {
                    while (1)
                    {
                       if ((1 == Module4G_IsModuleTypeSet()) || count++ >= 12)
                       {
                            break;
                       }
                       sleep(1);
                    }

                    e4GModuleType moduletype = UNKNOW_MODULE;
                    ZWSys_GetModuleType(&moduletype);
                    if (QUECTEL_EG800 == moduletype || ZHONGYUN_ZX800SG_S == moduletype || ZHONGYUN_ZX800HG_S == moduletype)
                    {
                        //欧版云卡
                        if (1 == MPU_Net_GetIsCloudCardDev())
                        {
                            InsertStringAtFirstDash(nBoardType, "SUY", pDC->m_szTypeName, sizeof(pDC->m_szTypeName));
                        }
                        else
                        {
                            InsertStringAtFirstDash(nBoardType, "SU", pDC->m_szTypeName, sizeof(pDC->m_szTypeName));
                        }
                    }
                    else if (ZHONGYUN_ZX800SG_A == moduletype || ZHONGYUN_ZX800HG_A == moduletype)
                    {
                        //美版云卡
                        if (1 == MPU_Net_GetIsCloudCardDev())
                        {
                            InsertStringAtFirstDash(nBoardType, "SAY", pDC->m_szTypeName, sizeof(pDC->m_szTypeName));
                        }
                        else
                        {
                            InsertStringAtFirstDash(nBoardType, "SA", pDC->m_szTypeName, sizeof(pDC->m_szTypeName));
                        }
                    }
                    else if (ZHONGYUN_ZX800SG == moduletype || ZHONGYUN_ZX800HG == moduletype)
                    {
                        //亚版云卡
                        if (1 == MPU_Net_GetIsCloudCardDev())
                        {
                            InsertStringAtFirstDash(nBoardType, "SY", pDC->m_szTypeName, sizeof(pDC->m_szTypeName));
                        }
                        else
                        {
                            //亚版非云卡
                            InsertStringAtFirstDash(nBoardType, "S", pDC->m_szTypeName, sizeof(pDC->m_szTypeName));
                        }
                    }
                    else
                    {
                        if (1 == MPU_Net_GetIsCloudCardDev())
                        {
                            InsertStringAtFirstDash(nBoardType, "SY", pDC->m_szTypeName, sizeof(pDC->m_szTypeName));
                        }
                        else
                        {
                            InsertStringAtFirstDash(nBoardType, "S", pDC->m_szTypeName, sizeof(pDC->m_szTypeName));
                        }
                    }
                }
            }
        }
#endif
        pDC->m_bytVideoInChannels = s_tDevCap_pub.bytVideoInChannels;
        pDC->m_bytAudioInChannels = s_tDevCap_pub.bytAudioInChannels;
        pDC->m_bytAlarmInChannels = s_tDevCap_pub.bytAlarmInChannels;
        pDC->m_bytAlarmOutChannels = s_tDevCap_pub.bytAlarmOutChannels;
        pDC->m_bytVGAChannels = s_tDevCap_pub.bytVGAChannels;
        pDC->m_bytSpotOutChannles = s_tDevCap_pub.bytSpotOutChannles;
        pDC->m_bytVoiceInChannels = s_tDevCap_pub.bytVoiceInChannels;
        pDC->m_bytMaxDiskNums = s_tDevCap_pub.bytMaxDiskNums;

        for (int iChannel = 0; iChannel < MAX_CHANNEL_NUM; ++iChannel)
        {
            pDC->m_bytVideoSteamType[iChannel] = s_tDevCap_pub.bytVideoSteamType[iChannel];    /*支持的码流类型， STREAM_MAIN/STREAM_SUB */
            pDC->m_dwMainVideoSreamResolutionCap[iChannel] = s_tDevCap_pub.bytMainVideoSreamResolutionCap[iChannel];    /*主码流支持的压缩尺寸CR_CIF/CR_2CIF/CR_4CIF */
            pDC->m_dwSubVideoSreamResolutionCap[iChannel] = s_tDevCap_pub.bytSubVideoSreamResolutionCap[iChannel];    /*子码流支持的压缩尺寸CR_CIF/CR_2CIF/CR_4CIF */
        }
    }

    return IDVR_OK;
}



//!\brief   获取设备能力
//!\param   pDV             设备版本
//!\param   resVersion      重新获版本信息
//!\return  0表示成功，其它为错误码
int MPU_PUB_GetDeviceVersion(PDEVICEVERSION pDV,int resVersion)
{
    char valueS[64] = {0};
    static int frist = 1;
    static char szHardwareVersion[32] = {0};
    static char BuildDateS[9+14] = {0};
    static char BuildDatePure[9] = {0};
    static int Major_versionS = 0;
    static int Minor_versionS = 0;
    static int ReversionS = 0;

    static char BuildDateD[9+14] = {0};
    static int Major_versionD = 0;
    static int Minor_versionD = 0;
    static int ReversionD = 0;
    int nRet = 0;
    static int filter_date = 0;

    //判断指针型参数是否合法
    MPU_CHECK_POINTER_PARAMETER(pDV);
    memset(pDV, 0, sizeof (DEVICEVERSION));

    if(1 == frist || 1 == resVersion)
    {
        //获取新的配置文件读取实例
        CConfig *pCfg = CConfig::NewInstance();
        if(NULL == pCfg)
        {
            MPU_ERR("CConfig::NewInstance Fail!\n");
            return -1;
        }

        //从版本信息中获取
        nRet = pCfg->LoadFile(IDVR_VERSION_CONFIG_FILE_PATH);
        if (0 == nRet)
        {
            nRet = GetIniSectionItem(CUSTOM_CONFIGMODE_FILE, "SystemFunction", "HideVersionTime", valueS, sizeof(valueS));//如果没有定制则用custommode.ini中默认的
            if (0 == nRet)
            {
                if (1 == atoi(valueS))
                {
                    filter_date = 1;
                }
                else if(0 == atoi(valueS))
                {
                    filter_date = 0;
                }
            }
            //硬件版本
            pCfg->GetValue("DeviceVersion", "version", "", szHardwareVersion, sizeof (pDV->m_szHardwareVersion));
            snprintf(pDV->m_szHardwareVersion, sizeof(pDV->m_szHardwareVersion), "%s",szHardwareVersion);

            //软件版本
            Major_versionS = pCfg->GetValue("Software", "Major_version", 0);
            Minor_versionS = pCfg->GetValue("Software", "Minor_version", 0);
            ReversionS = pCfg->GetValue("Software", "Reversion", 0);
            pCfg->GetValue("Software", "BuildDate", "", BuildDateS, sizeof (BuildDateS));
            // 日期固定为8字节
            memcpy(BuildDatePure, BuildDateS, 8);
            BuildDatePure[8] = '\0';

            memset(valueS, 0, sizeof(valueS));
            nRet = GetIniSectionItem(CUSTOM_CONFIG, "CGI", "BuildDate", valueS, sizeof(valueS));

            /* 防止出现数组越界而出现段错误(strlen(value)不包含'\0') */
            if ((nRet==0) && (strlen(valueS)-1<sizeof(BuildDateS)))
            {
                memset(BuildDateS, 0, sizeof(BuildDateS));
                valueS[strlen(valueS)-1] = '\0';
            //  printf("value = %s\n", value);
                strncpy(BuildDateS, valueS, strlen(valueS)+1);
            }

            memset(valueS, 0, sizeof(valueS));
            nRet = GetIniSectionItem(CUSTOM_CONFIGMODE_FILE, "SystemFunction", "CustomVersionInfo", valueS, sizeof(valueS));//如果没有定制则用custommode.ini中默认的
            if(-1 == nRet)
            {
                MPU_ERR("Get CustomVersionInfo error! Please check the custommode.ini format!");
            }

            /* 防止出现数组越界而出现段错误(strlen(value)不包含'\0') */
            if ((nRet==0) && ((strlen(valueS)-1)<(sizeof(BuildDateS)-8)))
            {
                valueS[strlen(valueS)-1] = '\0';
                strncat(BuildDateS, valueS, strlen(valueS));
                if(1 == filter_date)
                {
                    snprintf(pDV->m_szSoftwareVersion, sizeof(pDV->m_szSoftwareVersion), "%d.%d.%d", Major_versionS, Minor_versionS, ReversionS);
                    snprintf(pDV->m_szSoftwareVersionPure, sizeof(pDV->m_szSoftwareVersionPure), "%d.%d.%d", Major_versionS, Minor_versionS, ReversionS);
                }
                else
                {
                    snprintf(pDV->m_szSoftwareVersion, sizeof(pDV->m_szSoftwareVersion), "%d.%d.%d-%s", Major_versionS, Minor_versionS, ReversionS, BuildDateS);
                    snprintf(pDV->m_szSoftwareVersionPure, sizeof(pDV->m_szSoftwareVersionPure), "%d.%d.%d-%s", Major_versionS, Minor_versionS, ReversionS, BuildDatePure);
                }
            }
            else
            {
                if(1 == filter_date)
                {
                    snprintf(pDV->m_szSoftwareVersion, sizeof(pDV->m_szSoftwareVersion), "%d.%d.%d", Major_versionS, Minor_versionS, ReversionS);
                    snprintf(pDV->m_szSoftwareVersionPure, sizeof(pDV->m_szSoftwareVersionPure), "%d.%d.%d", Major_versionS, Minor_versionS, ReversionS);
                }
                else
                {
                    snprintf(pDV->m_szSoftwareVersion, sizeof(pDV->m_szSoftwareVersion), "%d.%d.%d-%s", Major_versionS, Minor_versionS, ReversionS, BuildDateS);
                    snprintf(pDV->m_szSoftwareVersionPure, sizeof(pDV->m_szSoftwareVersionPure), "%d.%d.%d-%s", Major_versionS, Minor_versionS, ReversionS, BuildDatePure);
                }
            }

            //编码版本
            Major_versionD = pCfg->GetValue("DSPSoftware", "Major_version", 0);
            Minor_versionD = pCfg->GetValue("DSPSoftware", "Minor_version", 0);
            ReversionD = pCfg->GetValue("DSPSoftware", "Reversion", 0);
            pCfg->GetValue("DSPSoftware", "BuildDate", "", BuildDateD, sizeof (BuildDateD));
            snprintf(pDV->m_szDspVersion, sizeof(pDV->m_szDspVersion), "%d.%d.%d-%s", Major_versionD, Minor_versionD, ReversionD, BuildDateD);

            frist = 0;
            printf("Get value from config\n");
        }

        CConfig::ReleaseInstance(pCfg);
    }
    else if(0 == frist)
    {
        snprintf(pDV->m_szHardwareVersion, sizeof(pDV->m_szHardwareVersion), "%s",szHardwareVersion);
        if(1 == filter_date)
        {
            snprintf(pDV->m_szSoftwareVersion, sizeof(pDV->m_szSoftwareVersion), "%d.%d.%d", Major_versionS, Minor_versionS, ReversionS);
            snprintf(pDV->m_szSoftwareVersionPure, sizeof(pDV->m_szSoftwareVersionPure), "%d.%d.%d", Major_versionS, Minor_versionS, ReversionS);
        }
        else
        {
            snprintf(pDV->m_szSoftwareVersion,sizeof(pDV->m_szSoftwareVersion), "%d.%d.%d-%s", Major_versionS, Minor_versionS, ReversionS, BuildDateS);
            snprintf(pDV->m_szSoftwareVersionPure,sizeof(pDV->m_szSoftwareVersionPure), "%d.%d.%d-%s", Major_versionS, Minor_versionS, ReversionS, BuildDatePure);
        }
        //编码版本
        snprintf(pDV->m_szDspVersion, sizeof(pDV->m_szDspVersion), "%d.%d.%d-%s", Major_versionD, Minor_versionD, ReversionD, BuildDateD);
    }

    //printf("pDV->m_szHardwareVersion = %s\n",pDV->m_szHardwareVersion);
    //printf("pDV->m_szSoftwareVersion = %s\n",pDV->m_szSoftwareVersion);
    //printf("pDV->m_szDspVersion = %s\n",pDV->m_szDspVersion);

    ZWSys_GetSerialNum(pDV->m_szSerialNum);

    return IDVR_OK;
}

//!\brief   设置设备系统时间
//!\param   tNewTime        新的系统时间
//!\return  0表示成功，其它为错误码
int MPU_PUB_SetSystemTime(time_t tNewTime)
{
    int nEncCount = 0, nEncID = 0;
    int nRet = IDVR_OK, nSDKRet = 0;
    int nDstOffsSec = 0, bDstIsEnable = 0;

    //获取通道总数
    nEncCount = MAX_ENC_CHANNEL_NUM;
    MPU_DBG("---MPU_PUB_SetSystemTime----tNewTime=%d----\n",tNewTime);

    //需暂停录像
    for (nEncID = 0; nEncID < nEncCount; ++nEncID)
    {
        MPU_AVL_Record_Pause(nEncID, TRUE);
    }

    // 1. 检测DST是否开启
    bDstIsEnable = MPU_SYS_IsEnableDST();
    if (bDstIsEnable)
    {
        nDstOffsSec = MPU_SYS_GetDstBias();
        if (nDstOffsSec)
        {
            tNewTime -= nDstOffsSec;
            printf("Dst is enable, need set time to offset sub %d seconds\n", nDstOffsSec);
        }
    }
    //设置时间
    char tBuf[64] = { 0 };
    nSDKRet = stime(&tNewTime);
    if (0 != nSDKRet)
    {
        //设置失败
        ctime_r(&tNewTime,tBuf);
        MPU_ERR("call stime() fail, tNewTime %lu, return %d, mktime %s \n", tNewTime, nSDKRet, tBuf);
        MPU_CHK(0 == nSDKRet);
        nRet = IDVR_ERROR_TIME;
    }
    else
    {
        //如果夏令时开启了，则需要将当前linux系统时间（未操作DST的时间）写入RTC硬件，再然后将linux时间加上DST时间差
        //写RTC硬件的耗时不同平台可能不同，为了防止阻塞网络调用必须异步，且DST开启后RTC写完毕了还需要恢复系统时间
        if (bDstIsEnable && nDstOffsSec)
        {
            tNewTime += nDstOffsSec;
            ZWSys_EncSetCurrentTimeEx(tNewTime);
        }
        else
            {
                //通知系统SDK对时
                ZWSys_EncSetCurrentTime();
            }
    }

    //恢复录像
    for (nEncID = 0; nEncID < nEncCount; ++nEncID)
    {
        MPU_AVL_Record_Pause(nEncID, FALSE);
    }

    return nRet;
}

//!\brief       获取设备别名
//!\param   szAlias     别名缓冲区(21 bytes)
//!\param   nLen        缓冲区大小
//!\return  0表示成功，其它为错误码
int MPU_PUB_GetDevAlias(char *szAlias, int nLen)
{
    CConfig *pConfig;

    if (NULL == szAlias)
    {
        MPU_ERR("NULL == szAlias\n");
        return -1;
    }

    if (nLen <= 0)
    {
        MPU_ERR("nLen(%d) <= 0\n", nLen);
        return -2;
    }

    //从配置文件初始化
    pConfig = CConfig::GetInstance();

    if (NULL != pConfig)
    {
        //从配置文件中获取
        pConfig->GetValue(SECTION_DEV_ALIAS, "Name", DEFNAME_DEV_ALIAS, szAlias, nLen);
    }
    else
    {
        //读取配置文件失败
        MPU_ERR("no CConfig instance\n");

        snprintf(szAlias, nLen - 1, DEFNAME_DEV_ALIAS);
        szAlias[nLen - 1] = '\0';
    }

    return IDVR_OK;
}

//!\brief       获取视频通道别名
//!\param   nChannel    通道号
//!\param   szAlias     别名缓冲区(21 bytes)
//!\param   nLen        缓冲区大小
//!\return  0表示成功，其它为错误码
int MPU_PUB_GetCameraAlias(int nCh, char *szAlias, int nLen)
{
    CConfig *pConfig;

    if (NULL == szAlias)
    {
        MPU_ERR("NULL == szAlias\n");
        return -1;
    }

    if (nLen <= 0)
    {
        MPU_ERR("nLen(%d) <= 0\n", nLen);
        return -2;
    }

    //从配置文件初始化
    pConfig = CConfig::GetInstance();

    if (NULL != pConfig)
    {
        char szSection[30];

        char szDefaultName[30];

        snprintf(szSection, sizeof(szSection), SECTION_OSD_ALIAS, nCh);

        snprintf(szDefaultName,sizeof(szDefaultName), DEFNAME_OSD_ALIAS, nCh + 1);

        //从配置文件中获取
        pConfig->GetValue(szSection, "Name", szDefaultName, szAlias, nLen);
    }
    else
    {
        //读取配置文件失败
        MPU_ERR("no CConfig instance\n");

        snprintf(szAlias, nLen - 1, DEFNAME_OSD_ALIAS, nCh + 1);
        szAlias[nLen - 1] = '\0';
    }

    return IDVR_OK;
}

//!\brief       获取报警输入别名
//!\param   nCh 通道号
//!\param   szAlias     别名缓冲区(21 bytes)
//!\param   nLen        缓冲区大小
//!\return  0表示成功，其它为错误码
int MPU_PUB_GetAlarmInputAlias(int nCh, char *szAlias, int nLen)
{
    CConfig *pConfig;

    if (NULL == szAlias)
    {
        MPU_ERR("NULL == szAlias\n");
        return -1;
    }

    if (nLen <= 0)
    {
        MPU_ERR("nLen(%d) <= 0\n", nLen);
        return -2;
    }

    //从配置文件初始化
    pConfig = CConfig::GetInstance();

    if (NULL != pConfig)
    {
        char szSection[30];

        char szDefaultName[30];

        snprintf(szSection, sizeof(szSection), SECTION_DIP_ALIAS, nCh);

        snprintf(szDefaultName, sizeof(szDefaultName), DEFNAME_DIP_ALIAS, nCh + 1);

        //从配置文件中获取
        pConfig->GetValue(szSection, "Name", szDefaultName, szAlias, nLen);
    }
    else
    {
        //读取配置文件失败
        MPU_ERR("no CConfig instance\n");

        snprintf(szAlias, nLen - 1, DEFNAME_DIP_ALIAS, nCh + 1);
        szAlias[nLen - 1] = '\0';
    }

    return IDVR_OK;
}

//!\brief       获取报警输出别名
//!\param   nCh 通道号
//!\param   szAlias     别名缓冲区(21 bytes)
//!\param   nLen        缓冲区大小
//!\return  0表示成功，其它为错误码
int MPU_PUB_GetAlarmOutputAlias(int nCh, char *szAlias, int nLen)
{
    CConfig *pConfig;

    if (NULL == szAlias)
    {
        MPU_ERR("NULL == szAlias\n");
        return -1;
    }

    if (nLen <= 0)
    {
        MPU_ERR("nLen(%d) <= 0\n", nLen);
        return -2;
    }

    //从配置文件初始化
    pConfig = CConfig::GetInstance();

    if (NULL != pConfig)
    {
        char szSection[30];

        char szDefaultName[30];

        snprintf(szSection, sizeof(szSection), SECTION_DOP_ALIAS, nCh);

        snprintf(szDefaultName, sizeof(szDefaultName), DEFNAME_DOP_ALIAS, nCh + 1);

        //从配置文件中获取
        pConfig->GetValue(szSection, "Name", szDefaultName, szAlias, nLen);
    }
    else
    {
        //读取配置文件失败
        MPU_ERR("no CConfig instance\n");

        snprintf(szAlias, nLen - 1, DEFNAME_DOP_ALIAS, nCh + 1);
        szAlias[nLen - 1] = '\0';
    }

    return IDVR_OK;
}
#if 0
//获取别名，需外部将缓冲区清零
int PUB_GetAlias(const char *szSection, const char *szDefaultName, LPHY_DVR_ALIAS_CFG pcfg)
{
    int nRet = IDVR_OK;

    LPHY_DVR_FONT_INFO pFont = &pcfg->struFontInfo;

    CConfig *pConfig;

    ///////////////////////////////////////////////////////////////////////////////
    //本结构体是否有效，=0表示无效
    pcfg->bValid = 1;
    pFont->bValid = 1;

    //从配置文件初始化
    pConfig = CConfig::GetInstance();

    if (NULL != pConfig)
    {
        //从配置文件中获取
        pConfig->GetValue(szSection, "Name", szDefaultName, pcfg->strName, sizeof (pcfg->strName));
        pcfg->nWidth = pConfig->GetValue(szSection, "Width", 100);
        pcfg->nHeight = pConfig->GetValue(szSection, "Height", 20);
        pcfg->nCharSet = pConfig->GetValue(szSection, "CharSet", 0);

        pFont->nFontSize = pConfig->GetValue(szSection, "FontSize", 20);
        pFont->nFontStyle = pConfig->GetValue(szSection, "FontStyle", 400);
        pConfig->GetValue(szSection, "FontName", "Times New Roman", pFont->szFontName, sizeof (pFont->szFontName));
    }
    else
    {
        //读取配置文件失败
        MPU_ERR("no CConfig instance\n");
        MPU_CHK(NULL != pConfig);

        strncpy(pcfg->strName, szDefaultName, sizeof (pcfg->strName) - 1);
        pcfg->strName[sizeof (pcfg->strName) - 1] = '\0';

        pcfg->nWidth = 100;
        pcfg->nHeight = 20;
        pcfg->nCharSet = 0;

        pFont->nFontSize = 20;
        pFont->nFontStyle = 400;
        strncpy(pFont->szFontName, "Times New Roman", sizeof (pFont->szFontName) - 1);
        pFont->szFontName[sizeof (pFont->szFontName) - 1] = '\0';
    }

    return nRet;
}
#endif
#if 0
//设置别名
int PUB_SetAlias(const char *szSection, LPHY_DVR_ALIAS_CFG pcfg)
{
    int nRet;

    CConfig *pConfig;

    ///////////////////////////////////////////////////////////////////////////////
    //本结构体是否有效，=0表示无效
    if (0 == pcfg->bValid)
    {
        MPU_DBG("0 == pcfg->bValid, %s\n", szSection);
        return IDVR_CIU_STRUCT_UNVALID;
    }

    //从配置文件初始化
    pConfig = CConfig::GetInstance();

    if (NULL != pConfig)
    {
        LPHY_DVR_FONT_INFO pFont = &pcfg->struFontInfo;

        //从配置文件中获取
        pConfig->SetValue(szSection, "Name", pcfg->strName);

        pConfig->SetValue(szSection, "Width", pcfg->nWidth);
        pConfig->SetValue(szSection, "Height", pcfg->nHeight);
        pConfig->SetValue(szSection, "CharSet", pcfg->nCharSet);

        if (0 != pFont->bValid)
        {
            pConfig->SetValue(szSection, "FontSize", pFont->nFontSize);
            pConfig->SetValue(szSection, "FontStyle", pFont->nFontStyle);

            pConfig->SetValue(szSection, "FontName", pFont->szFontName);
        }
        else
        {
            MPU_DBG("font config not valid, %s\n", szSection);
        }

        nRet = IDVR_OK;
    }
    else
    {
        //写取配置文件失败
        MPU_ERR("no CConfig instance, %s\n", szSection);
        MPU_CHK(NULL != pConfig);

        nRet = IDVR_ERROR_CONFIGFILE;
    }

    return nRet;
}
#endif
//获取别名，需外部将缓冲区清零
int MPU_PUB_GetAlias(const char *szSection, const char *szDefaultName, LPHY_DVR_ALIAS_CFG_1 pcfg)
{
    int nRet = IDVR_OK, i=0;

    LPHY_DVR_FONT_INFO pFont = &pcfg->struFontInfo;

    CConfig *pConfig;

    char utf8[DVR_MAX_CHANNELNAME_LEN * 6 + 1]={0};//unicode最大5位，每个数字间还有一个冒号
    //此处的数组大小要和下面的MPU_PUB_SetAlias函数中的value结构体大小一致，否则会出现IE界面获取出现乱码的情况
    unsigned int value[256]={0};

    ///////////////////////////////////////////////////////////////////////////////
    //本结构体是否有效，=0表示无效
    pcfg->bValid = 1;
    pFont->bValid = 1;

    //从配置文件初始化
    pConfig = CConfig::GetInstance();

    if (NULL != pConfig)
    {
        //从配置文件中获取
        pConfig->GetValue(szSection, "Unicode", szDefaultName, utf8, sizeof (utf8));
        sscanf(utf8, "%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d",
            &value[0],&value[1],  &value[2], &value[3], &value[4],
            &value[5],&value[6],  &value[7], &value[8], &value[9],
            &value[10],&value[11],&value[12],&value[13],&value[14],
            &value[15],&value[16],&value[17],&value[18],&value[19]);

        pcfg->nWidth = pConfig->GetValue(szSection, "Width", 100);
        pcfg->nHeight = pConfig->GetValue(szSection, "Height", 20);
        pcfg->nCharSet = pConfig->GetValue(szSection, "CharSet", 0);
        pFont->nFontSize = pConfig->GetValue(szSection, "FontSize", 20);
        pFont->nFontStyle = pConfig->GetValue(szSection, "FontStyle", 400);

        pConfig->GetValue(szSection, "FontName", "Times New Roman", pFont->szFontName, sizeof (pFont->szFontName));

       MPU_DBG("[%s:%d]Get Unicode are %s\n", __FUNCTION__, __LINE__, utf8);
    }
    else
    {
        //读取配置文件失败
        MPU_ERR("no CConfig instance\n");
        MPU_CHK(NULL != pConfig);

        strncpy(utf8, szDefaultName, sizeof (utf8) - 1);
        utf8[sizeof (utf8) - 1] = '\0';

        pcfg->nWidth = 100;
        pcfg->nHeight = 20;
        pcfg->nCharSet = 0;

        pFont->nFontSize = 20;
        pFont->nFontStyle = 400;
        strncpy(pFont->szFontName, "Times New Roman", sizeof (pFont->szFontName) - 1);
        pFont->szFontName[sizeof (pFont->szFontName) - 1] = '\0';
    }

    for (i=0; i<(DVR_MAX_CHANNELNAME_LEN - 1); i++)
    {
       pcfg->strName[i] = value[i];
    }
    pcfg->strName[DVR_MAX_CHANNELNAME_LEN - 1] = 0;

    return nRet;
}

//设置别名
int MPU_PUB_SetAlias(const char *szSection, LPHY_DVR_ALIAS_CFG_1 pcfg)
{
    int nRet;

    CConfig *pConfig;

    ///////////////////////////////////////////////////////////////////////////////
    //本结构体是否有效，=0表示无效
    if (0 == pcfg->bValid)
    {
        MPU_DBG("0 == pcfg->bValid, %s\n", szSection);
        return IDVR_CIU_STRUCT_UNVALID;
    }

    //从配置文件初始化
    pConfig = CConfig::GetInstance();

    if (NULL != pConfig)
    {
        LPHY_DVR_FONT_INFO pFont = &pcfg->struFontInfo;

        char utf8[DVR_MAX_CHANNELNAME_LEN * 6 + 1]={0};//unicode最大5位，每个数字间还有一个冒号

        //将unicode转换为utf8保存
        pcfg->strName[sizeof(pcfg->strName)/2 - 1] = 0;
        uc16_conv_to_utf8((unsigned char *) utf8, (const unsigned char *) pcfg->strName, (sizeof (pcfg->strName) - 1) << 1);

        char value[256];
        memset(value, 0, sizeof(value));
        snprintf(value, sizeof(value), "%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d",
            pcfg->strName[0],pcfg->strName[1],pcfg->strName[2],pcfg->strName[3],
            pcfg->strName[4],pcfg->strName[5],pcfg->strName[6],pcfg->strName[7],
            pcfg->strName[8],pcfg->strName[9],pcfg->strName[10],pcfg->strName[11],
            pcfg->strName[12],pcfg->strName[13],pcfg->strName[14],pcfg->strName[15],
            pcfg->strName[16],pcfg->strName[17],pcfg->strName[18],pcfg->strName[19]);
        pConfig->SetValue(szSection, "Unicode", value);

//         MPU_DBG("[%s:%d]Set Unicode are %s\n", __FUNCTION__, __LINE__, value);

        //从配置文件中获取
        pConfig->SetValue(szSection, "NameUTF8", utf8);

        pConfig->SetValue(szSection, "Width", pcfg->nWidth);
        pConfig->SetValue(szSection, "Height", pcfg->nHeight);
        pConfig->SetValue(szSection, "CharSet", pcfg->nCharSet);

        if (0 != pFont->bValid)
        {
            pConfig->SetValue(szSection, "FontSize", pFont->nFontSize);
            pConfig->SetValue(szSection, "FontStyle", pFont->nFontStyle);
            pConfig->SetValue(szSection, "FontName", pFont->szFontName);
        }
        else
        {
            MPU_DBG("font config not valid, %s\n", szSection);
        }

        nRet = IDVR_OK;
    }
    else
    {
        //写取配置文件失败
        MPU_ERR("no CConfig instance, %s\n", szSection);
        MPU_CHK(NULL != pConfig);

        nRet = IDVR_ERROR_CONFIGFILE;
    }

    return nRet;
}


//!\brief   判断是否是80XXHC或HS
//!\return  TRUE表示是，FALSE表示不是
BOOL PUB_Is80XX_HC_HS()
{
    return FALSE;
}

//!\brief   判断是否是8008HT或8016HT
//!\return  TRUE表示是，FALSE表示不是
int MPU_PUB_Is8008_or_8016HT()
{
    return FALSE;
}

// 设置 SPOT 模式
int MPU_PUB_SetSpotMode(int nMode)
{
    int nOldMode = MPU_PRW_GetSpotMode();

    if (nOldMode == nMode)
    {
        return 0;
    }
    return MPU_PRW_SetSpotMode(nMode);
}

int MPU_PUB_GetSpotMode()
{
    return MPU_PRW_GetSpotMode();
}

int MPU_PUB_GetCustomeFunction(LPHY_DVR_CUSTOMER_INFO pDV)
{
    int nRet = -1;
    DEVICECAP dc;
    //判断指针型参数是否合法
    MPU_CHECK_POINTER_PARAMETER(pDV);
    CConfig *pConfig = NULL;

    //从配置文件初始化
    pConfig = CConfig::GetInstance();
    if (NULL != pConfig)
    {
        pConfig->GetValue(DEVICE_INFO, "Country", "China", pDV->szCountry, sizeof (pDV->szCountry));
        pConfig->GetValue(DEVICE_INFO, "City", "", pDV->szCity, sizeof (pDV->szCity));
        pConfig->GetValue(DEVICE_INFO, "Manufacturer", "ONVIF_IPNC", pDV->Manaufacture, sizeof (pDV->Manaufacture));
        pConfig->GetValue(DEVICE_INFO, "Name", "IPCAM", pDV->DeviceName, sizeof (pDV->DeviceName));
        pConfig->GetValue(DEVICE_INFO, "Model", "", pDV->Model, sizeof (pDV->Model));
        pConfig->GetValue(DEVICE_INFO, "Owner", "", pDV->cOwner, sizeof (pDV->cOwner));
        pDV->bytDiscoveryMode = pConfig->GetValue(DEVICE_INFO, "DiscoveryMode", 0);
        pDV->cGBCmdSafeMode = pConfig->GetValue(DEVICE_INFO, "SafeMode", 0);
        pDV->cGBRegisterWay = pConfig->GetValue(DEVICE_INFO, "RegisterWay", 1);
        pDV->cSecrecy = pConfig->GetValue(DEVICE_INFO, "Secrecy", 0);
    }
    else
    {
        strncpy(pDV->szCountry, "China", sizeof(pDV->szCountry) - 1);
        strncpy(pDV->szCity, "", sizeof(pDV->szCountry) - 1);
        strncpy(pDV->Manaufacture, "ONVIF_IPNC", sizeof(pDV->Manaufacture) - 1);
        strncpy(pDV->DeviceName, "IPCAM", sizeof(pDV->DeviceName) - 1);
        strncpy(pDV->Model, "", sizeof(pDV->Model) - 1);
        strncpy(pDV->cOwner, "", sizeof(pDV->cOwner) - 1);
        pDV->bytDiscoveryMode = 0;
        pDV->cGBCmdSafeMode = 0;
        pDV->cGBRegisterWay = 0;
        pDV->cSecrecy = 0;
    }

    pDV->pGB2Unicode = (void *)gb2unicode;
    pDV->pUnicode2GB = (void *)HKUniToStr;

    //获取设备能力
    nRet = MPU_PUB_GetDeviceCap(&dc);
    MPU_CHK(0 == nRet);

    //设备型谱名称，如HY-2016HC
    strncpy(pDV->Model, dc.m_szTypeName, sizeof (pDV->Model) - 1);

    return IDVR_OK;
}

int MPU_PUB_SetDeviceFunction(const char *pBuffer)
{
    int ret = 0;

    //判断指针型参数是否合法
    MPU_CHECK_POINTER_PARAMETER(pBuffer);

    ret = DeviceFunctionMapFunc(pBuffer);

    return ret;
}

int MPU_PUB_GetCustomer(LPHY_DVR_CUSTOMER_INFO pDV)
{
    int nRet = 0;
    DEVICECAP dc;
    //判断指针型参数是否合法
    MPU_CHECK_POINTER_PARAMETER(pDV);
    //printf("+++++++++++++++++++++MPU_PUB_GetCustomer+++++++++++++++++++++++++\n");

    CConfig *pCfg;

    //从配置文件初始化
    pCfg = CConfig::GetInstance();

    if (NULL != pCfg)
    {
        pDV->nValid = 1;

        pCfg->GetValue(DEVICE_INFO, "Country", "", pDV->szCountry, sizeof (pDV->szCountry));
        pCfg->GetValue(DEVICE_INFO, "City", "", pDV->szCity, sizeof (pDV->szCity));
        pCfg->GetValue(DEVICE_INFO, "Manufacturer", "", pDV->Manaufacture, sizeof (pDV->Manaufacture));
        pCfg->GetValue(DEVICE_INFO, "Name", "", pDV->DeviceName, sizeof (pDV->DeviceName));
        pCfg->GetValue(DEVICE_INFO, "Owner", "", pDV->cOwner, sizeof (pDV->cOwner));

        pDV->bytDiscoveryMode = pCfg->GetValue(DEVICE_INFO, "DiscoveryMode", 0);
        pDV->cGBCmdSafeMode = pCfg->GetValue(DEVICE_INFO, "SafeMode", 0);
        pDV->cGBRegisterWay = pCfg->GetValue(DEVICE_INFO, "RegisterWay", 1);
        pDV->cSecrecy = pCfg->GetValue(DEVICE_INFO, "Secrecy", 0);

        pDV->pGB2Unicode = (void *)gb2unicode;
        pDV->pUnicode2GB = (void *)HKUniToStr;

        if (strlen(pDV->Model) == 0)
        {
            //获取设备能力
            nRet = MPU_PUB_GetDeviceCap(&dc);
            MPU_CHK(0 == nRet);

            //设备型谱名称，如HY-2016HC
            strncpy(pDV->Model, dc.m_szTypeName, sizeof (pDV->Model) - 1);
        }

        nRet = IDVR_OK;
    }
    else
    {
        //读取配置文件失败
        MPU_ERR("no CConfig instance\n");
        MPU_CHK(NULL != pCfg);

        nRet = IDVR_ERROR_CONFIGFILE;
    }

    return nRet;
}

int MPU_PUB_SetCustomer(LPHY_DVR_CUSTOMER_INFO pDV)
{
    int nRet = 0;

    //判断指针型参数是否合法
    MPU_CHECK_POINTER_PARAMETER(pDV);
    //printf("+++++++++++++++++++++MPU_PUB_SetCustomer+++++++++++++++++++++++++\n");

    CConfig *pCfg;

    //从配置文件初始化
    pCfg = CConfig::GetInstance();

    if (NULL != pCfg)
    {
        pCfg->SetValue(DEVICE_INFO, "Country", pDV->szCountry);
        pCfg->SetValue(DEVICE_INFO, "City", pDV->szCity);
        pCfg->SetValue(DEVICE_INFO, "Manufacturer", pDV->Manaufacture);
        pCfg->SetValue(DEVICE_INFO, "Owner", pDV->cOwner);

        pCfg->SetValue(DEVICE_INFO, "SafeMode", pDV->cGBCmdSafeMode);
        pCfg->SetValue(DEVICE_INFO, "RegisterWay", pDV->cGBRegisterWay);
        pCfg->SetValue(DEVICE_INFO, "Secrecy", pDV->cSecrecy);

        nRet = IDVR_OK;
    }
    else
    {
        //读取配置文件失败
        MPU_ERR("no CConfig instance\n");
        MPU_CHK(NULL != pCfg);

        nRet = IDVR_ERROR_CONFIGFILE;
    }

    return nRet;
}

int MPU_PUB_GetDevInstallInfo(ZW_MULTI_CHANNEL_INSTALLTION_INFO *pcfg)
{
    int nRet = 0;

    char value[100] = {0};

    //判断指针型参数是否合法
    MPU_CHECK_POINTER_PARAMETER(pcfg);
    //printf("+++++++++++++++++++++MPU_PUB_GetDevInstallInfo+++++++++++++++++++++++++\n");

    CConfig *pCfg;

    //从配置文件初始化
    pCfg = CConfig::GetInstance();

    if (NULL != pCfg)
    {
        pcfg->bValid = 1;

        pcfg->stInstallInfo[0].bValid = 1;

        pCfg->GetValue(GB28181_InstallSec, "CompanyName", "", pcfg->stInstallInfo[0].strCompanyName, sizeof (pcfg->stInstallInfo[0].strCompanyName));
        pCfg->GetValue(GB28181_InstallSec, "TelNum", "", pcfg->stInstallInfo[0].strTelNum, sizeof (pcfg->stInstallInfo[0].strTelNum));
        pCfg->GetValue(GB28181_InstallSec, "InstallAddress", "", pcfg->stInstallInfo[0].cInstallAddress, sizeof (pcfg->stInstallInfo[0].cInstallAddress));

        pCfg->GetValue(GB28181_InstallSec, "InstallTime", "", value, 100);
        sscanf(value, "%d:%d:%d-%d:%d:%d", &pcfg->stInstallInfo[0].struInstallTime.nYear, &pcfg->stInstallInfo[0].struInstallTime.nMonth, &pcfg->stInstallInfo[0].struInstallTime.nDay \
            , &pcfg->stInstallInfo[0].struInstallTime.nHour, &pcfg->stInstallInfo[0].struInstallTime.nMinute, &pcfg->stInstallInfo[0].struInstallTime.nSecond);
        pCfg->GetValue(GB28181_InstallSec, "MaintainTime", "", value, 100);
        sscanf(value, "%d:%d:%d-%d:%d:%d", &pcfg->stInstallInfo[0].struMaintainTime.nYear, &pcfg->stInstallInfo[0].struMaintainTime.nMonth, &pcfg->stInstallInfo[0].struMaintainTime.nDay \
            , &pcfg->stInstallInfo[0].struMaintainTime.nHour, &pcfg->stInstallInfo[0].struMaintainTime.nMinute, &pcfg->stInstallInfo[0].struMaintainTime.nSecond);

        nRet = IDVR_OK;
    }
    else
    {
        //读取配置文件失败
        MPU_ERR("no CConfig instance\n");
        MPU_CHK(NULL != pCfg);

        nRet = IDVR_ERROR_CONFIGFILE;
    }

    return nRet;
}

int MPU_PUB_SetDevInstallInfo(ZW_MULTI_CHANNEL_INSTALLTION_INFO *pcfg)
{
    int nRet = 0;

    char str[100] = {0};

    //判断指针型参数是否合法
    MPU_CHECK_POINTER_PARAMETER(pcfg);
    //printf("+++++++++++++++++++++MPU_PUB_GetDevInstallInfo+++++++++++++++++++++++++\n");

    CConfig *pCfg;

    //从配置文件初始化
    pCfg = CConfig::GetInstance();

    if (NULL != pCfg)
    {
        pCfg->SetValue(GB28181_InstallSec, "CompanyName", pcfg->stInstallInfo[0].strCompanyName);
        pCfg->SetValue(GB28181_InstallSec, "TelNum", pcfg->stInstallInfo[0].strTelNum);
        pCfg->SetValue(GB28181_InstallSec, "InstallAddress", pcfg->stInstallInfo[0].cInstallAddress);

        snprintf(str, sizeof(str), "%d:%d:%d-%d:%d:%d", pcfg->stInstallInfo[0].struInstallTime.nYear, pcfg->stInstallInfo[0].struInstallTime.nMonth, pcfg->stInstallInfo[0].struInstallTime.nDay \
            , pcfg->stInstallInfo[0].struInstallTime.nHour, pcfg->stInstallInfo[0].struInstallTime.nMinute, pcfg->stInstallInfo[0].struInstallTime.nSecond);
        pCfg->SetValue(GB28181_InstallSec, "InstallTime", str);

        memset((void *)str,0,sizeof(str));
        snprintf(str, sizeof(str), "%d:%d:%d-%d:%d:%d", pcfg->stInstallInfo[0].struMaintainTime.nYear, pcfg->stInstallInfo[0].struMaintainTime.nMonth, pcfg->stInstallInfo[0].struMaintainTime.nDay \
            , pcfg->stInstallInfo[0].struMaintainTime.nHour, pcfg->stInstallInfo[0].struMaintainTime.nMinute, pcfg->stInstallInfo[0].struMaintainTime.nSecond);
        pCfg->SetValue(GB28181_InstallSec, "MaintainTime", str);

        nRet = IDVR_OK;
    }
    else
    {
        //读取配置文件失败
        MPU_ERR("no CConfig instance\n");
        MPU_CHK(NULL != pCfg);

        nRet = IDVR_ERROR_CONFIGFILE;
    }

    return nRet;
}

int MPU_PUB_GetDeviceFunction(char *pBuffer)
{
    //判断指针型参数是否合法
    MPU_CHECK_POINTER_PARAMETER(pBuffer);

    /* 暂时不支持 */

    return IDVR_OK;
}

//获取当前升级模式
int MPU_PUB_SetCurrentUpgradeMode(UPGRADE_MODE enUpgradeMode)
{
    if (enUpgradeMode < NOT_UPGRADE_MODE
        || enUpgradeMode >= MODE_BUTT)
    {
        return -1;
    }

    pthread_mutex_lock(&s_mutexUpgradeMode);
    if (NOT_UPGRADE_MODE != enUpgradeMode
        && NOT_UPGRADE_MODE != s_enUpgradeMode
        && enUpgradeMode != s_enUpgradeMode)
    {
        pthread_mutex_unlock(&s_mutexUpgradeMode);
        return -1;
    }
    s_enUpgradeMode = enUpgradeMode;
    pthread_mutex_unlock(&s_mutexUpgradeMode);

    return 0;
}

//设置当前升级模式
int MPU_PUB_GetCurrentUpgradeMode(UPGRADE_MODE *pUpgradeMode)
{
    if (NULL == pUpgradeMode)
    {
        return -1;
    }

    pthread_mutex_lock(&s_mutexUpgradeMode);
    *pUpgradeMode = s_enUpgradeMode;
    pthread_mutex_unlock(&s_mutexUpgradeMode);

    return 0;
}

#ifdef __IS_SUPPORT_CLOUDWORLD__
int MPU_PUB_CreateCloudModeFlag(int mode)
{
    char command[64] = {0};
    if (mode == CLOUD_STORAGE_MODE_DOMESTIC)
    {
        snprintf(command, sizeof(command), "touch %s", CLOUD_STORAGE_MODE_DOMESTIC_FLAG);
        mysystem(command);

        memset(command, 0, sizeof(command));
        snprintf(command, sizeof(command), "rm -rf %s", CLOUD_STORAGE_MODE_FOREIGN_FLAG);
        mysystem(command);
    }
    else if (mode == CLOUD_STORAGE_MODE_FOREIGN)
    {
        snprintf(command, sizeof(command), "touch %s", CLOUD_STORAGE_MODE_FOREIGN_FLAG);
        mysystem(command);

        memset(command, 0, sizeof(command));
        snprintf(command, sizeof(command), "rm -rf %s", CLOUD_STORAGE_MODE_DOMESTIC_FLAG);
        mysystem(command);
    }

    return 0;
}

//!\brief       获取设备云存储模式
//!\return  0表示成功，其它为错误码
int MPU_PUB_GetCloudModeCfg(HY_DVR_CLOUD_STORAGE_MODE *pcfg)
{
    CConfig *pConfig = NULL;
    int custommode = 0;

    if (NULL == pcfg)
    {
        MPU_ERR("NULL == pcfg\n");
        return -1;
    }

    //从配置文件初始化
    pConfig = CConfig::GetInstance();
    if (NULL != pConfig)
    {
        //从配置文件中获取

        pcfg->nMode = pConfig->GetValue(CLOUD_STORAGE_INFO_SECTION, CLOUD_STORAGE_MODE_KEY, CLOUD_STORAGE_MODE_DOMESTIC);
    }
    else
    {
        //读取配置文件失败
        MPU_ERR("no CConfig instance\n");
    }

    custommode = GetCustomCloudStorageMode();
    if ((custommode != -1) && (custommode != pcfg->nMode))
    {
        pcfg->nMode = custommode;
    }
    MPU_ERR("Get cloud storage mode custom:%d, final:%d\n", custommode, pcfg->nMode);
    return IDVR_OK;
}

//!\brief       设置设备云存储模式
//!\return  0表示成功，其它为错误码
int MPU_PUB_SetCloudModeCfg(HY_DVR_CLOUD_STORAGE_MODE *pcfg)
{
    CConfig *pConfig = NULL;
    int oldmode = 0;

    if (NULL == pcfg)
    {
        MPU_ERR("NULL == pcfg\n");
        return -1;
    }

    if (pcfg->nMode > 1 || pcfg->nMode < 0)
    {

        MPU_ERR("CloudMode is [0|1] not:%d\n", pcfg->nMode);
        return IDVR_CIU_PARAM_OVERFLOW;
    }

    MPU_GetCloudStorageMode(&oldmode);

    MPU_PUB_CreateCloudModeFlag(pcfg->nMode);

    MPU_SetCloudStorageMode(pcfg->nMode);

    HieServer_LoadProtocol(eProtocolCloudStorage);

    MPU_ERR("Now set cloud mode new:%d, old:%d, load %s!\n", pcfg->nMode, oldmode, pcfg->nMode?"foreign":"domestic");

    pConfig = CConfig::GetInstance();
    if (NULL != pConfig)
    {
        pConfig->SetValue(CLOUD_STORAGE_INFO_SECTION, CLOUD_STORAGE_MODE_KEY, pcfg->nMode);
    }
    else
    {
        MPU_ERR("no CConfig instance\n");
    }

    return IDVR_OK;
}
#endif


int MPU_PUB_SetCustomeFunction(LPHY_DVR_CUSTOMER_INFO pDV)
{
    CConfig *pConfig;

    //从配置文件初始化
    pConfig = CConfig::GetInstance();

    if (NULL != pConfig)
    {
        pConfig->SetValue(DEVICE_INFO, "Country", pDV->szCountry);
        pConfig->SetValue(DEVICE_INFO, "City", pDV->szCity);
        pConfig->SetValue(DEVICE_INFO, "Manufacturer", pDV->Manaufacture);
        pConfig->SetValue(DEVICE_INFO, "Owner", pDV->cOwner);
        pConfig->SetValue(DEVICE_INFO, "Model", pDV->Model);

        pConfig->SetValue(DEVICE_INFO, "SafeMode", pDV->cGBCmdSafeMode);
        pConfig->SetValue(DEVICE_INFO, "RegisterWay", pDV->cGBRegisterWay);
        pConfig->SetValue(DEVICE_INFO, "Secrecy", pDV->cSecrecy);

        pConfig->SetValue(DEVICE_INFO, "DiscoveryMode", pDV->bytDiscoveryMode);
    }
    else
    {
        //读取配置文件失败
        MPU_ERR("no CConfig instance\n");
        MPU_CHK(NULL != pConfig);
    }

    return IDVR_OK;
}

//!\brief       获取设备云存储信息
//!\return  0表示成功，其它为错误码
int MPU_PUB_GetCloudStorageInfo(HY_DVR_CLOUD_STORAGE *pcfg)
{
    CConfig *pConfig = NULL;
    FILE *pFile = NULL;
    char szStrTmp[256] = {0};

    if (NULL == pcfg)
    {
        MPU_ERR("NULL == pcfg\n");
        return -1;
    }

    //从配置文件初始化
    pConfig = CConfig::GetInstance();
    if (NULL != pConfig)
    {
        //从配置文件中获取
        pConfig->GetValue(CLOUD_STORAGE_INFO_SECTION, CLOUD_STORAGE_INFO_DEVID_KEY, "", pcfg->strDeviceID, sizeof(pcfg->strDeviceID));
        pConfig->GetValue(CLOUD_STORAGE_INFO_SECTION, CLOUD_STORAGE_INFO_DOMAIN_NAME_KEY, "", pcfg->strDomainName, sizeof(pcfg->strDomainName));
        //printf("\n[FAN-DEBUG][%s][%d]strDeviceID=%s\n",
        //    __FUNCTION__, __LINE__, pcfg->strDeviceID);

        pFile = fopen(CLOUD_STORAGE_INFO_DEVID_BAK_FILE, "r");
        if (NULL != pFile)
        {
            memset(szStrTmp, 0, sizeof(szStrTmp));
            fread(szStrTmp, sizeof(szStrTmp), 1, pFile);
            fclose(pFile);
            pFile = NULL;

            if (0 != strncmp(szStrTmp, pcfg->strDeviceID, sizeof(pcfg->strDeviceID)))
            {
                memset(pcfg->strDeviceID, 0, sizeof(pcfg->strDeviceID));
                snprintf(pcfg->strDeviceID, sizeof(pcfg->strDeviceID), "%s", szStrTmp);
                pConfig->SetValue(CLOUD_STORAGE_INFO_SECTION, CLOUD_STORAGE_INFO_DEVID_KEY, pcfg->strDeviceID);
            }
        }

        pFile = fopen(CLOUD_STORAGE_INFO_DOMAIN_NAME_BAK_FILE, "r");
        if (NULL != pFile)
        {
            memset(szStrTmp, 0, sizeof(szStrTmp));
            fread(szStrTmp, sizeof(szStrTmp), 1, pFile);
            fclose(pFile);
            pFile = NULL;

            if (0 != strncmp(szStrTmp, pcfg->strDomainName, sizeof(pcfg->strDomainName)))
            {
                memset(pcfg->strDomainName, 0, sizeof(pcfg->strDomainName));
                snprintf(pcfg->strDomainName, sizeof(pcfg->strDomainName), "%s", szStrTmp);
                pConfig->SetValue(CLOUD_STORAGE_INFO_SECTION, CLOUD_STORAGE_INFO_DOMAIN_NAME_KEY, pcfg->strDomainName);
            }
        }
    }
    else
    {
        //读取配置文件失败
        MPU_ERR("no CConfig instance\n");
    }

    return IDVR_OK;
}

//!\brief       设置设备云存储信息
//!\return  0表示成功，其它为错误码
int MPU_PUB_SetCloudStorageInfo(HY_DVR_CLOUD_STORAGE *pcfg)
{
    CConfig *pConfig = NULL;
    FILE *pFile = NULL;

    if (NULL == pcfg)
    {
        MPU_ERR("NULL == pcfg\n");
        return -1;
    }

    //从配置文件初始化
    pConfig = CConfig::GetInstance();
    if (NULL != pConfig)
    {
        pConfig->SetValue(CLOUD_STORAGE_INFO_SECTION, CLOUD_STORAGE_INFO_DEVID_KEY, pcfg->strDeviceID);
        pConfig->SetValue(CLOUD_STORAGE_INFO_SECTION, CLOUD_STORAGE_INFO_DOMAIN_NAME_KEY, pcfg->strDomainName);

        pFile = fopen(CLOUD_STORAGE_INFO_DEVID_BAK_FILE, "w");
        if (NULL != pFile)
        {
            fwrite(pcfg->strDeviceID, sizeof(pcfg->strDeviceID), 1, pFile);
            fclose(pFile);
            pFile = NULL;
        }

        pFile = fopen(CLOUD_STORAGE_INFO_DOMAIN_NAME_BAK_FILE, "w");
        if (NULL != pFile)
        {
            fwrite(pcfg->strDomainName, sizeof(pcfg->strDomainName), 1, pFile);
            fclose(pFile);
            pFile = NULL;
        }
    }
    else
    {
        //读取配置文件失败
        MPU_ERR("no CConfig instance\n");
    }

    return IDVR_OK;
}

//!\brief        获取设备云存储信息
//!\return    0表示成功，其它为错误码
int MPU_PUB_GetCloudStorageBaseCfg(HY_DVR_CLOUD_STROAGE_BASE_CONFIG *pcfg)
{
    if (NULL == pcfg)
    {
        MPU_ERR("NULL == pcfg\n");
        return -1;
    }

    CConfig *pConfig = NULL;

    //从配置文件初始化
    pConfig = CConfig::GetInstance();
    pcfg->cloudStorageRegion = 0;
    if (NULL != pConfig)
    {
        //从配置文件中获取
        pcfg->nEnableStatus = pConfig->GetValue(CLOUD_STORAGE_INFO_SECTION, CLOUD_STORAGE_INFO_ENABLE_KEY, 0);
        #ifdef __IS_SUPPORT_CLOUDWORLD__
        pcfg->cloudStorageRegion = GetCustomCloudStorageMode();
        #endif
    }
    else
    {
        //读取配置文件失败
        MPU_ERR("no CConfig instance\n");
    }
    return IDVR_OK;
}

//!\brief       设置设备云存储信息
//!\return  0表示成功，其它为错误码
int MPU_PUB_SetCloudStorageBaseCfg(HY_DVR_CLOUD_STROAGE_BASE_CONFIG *pcfg)
{
    CConfig *pConfig = NULL;

    if (NULL == pcfg)
    {
        MPU_ERR("NULL == pcfg\n");
        return -1;
    }

    //从配置文件初始化
    pConfig = CConfig::GetInstance();
    if (NULL != pConfig)
    {
        pConfig->SetValue(CLOUD_STORAGE_INFO_SECTION, CLOUD_STORAGE_INFO_ENABLE_KEY, pcfg->nEnableStatus);
    }
    else
    {
        //读取配置文件失败
        MPU_ERR("no CConfig instance\n");
    }

    return IDVR_OK;
}

//!\brief        设置设备云存储信息
//!\return    0表示成功，其它为错误码
int MPU_PUB_SetCloudStorageSubscribeStatus(HY_DVR_CLOUD_STROAGE_OPEN_STATUS *pcfg)
{
    CConfig *pConfig = NULL;

    if (NULL == pcfg)
    {
        MPU_ERR("NULL == pcfg\n");
        return -1;
    }

    //从配置文件初始化
    pConfig = CConfig::GetInstance();
    if (NULL != pConfig)
    {
        pConfig->SetValue(CLOUD_STORAGE_INFO_SECTION, CLOUD_STORAGE_INFO_SUBSCRIBE_STATUS_KEY, pcfg->nSubscribeStatus);
        pConfig->SetValue(CLOUD_STORAGE_INFO_SECTION, CLOUD_STORAGE_INFO_DEALER_ID_KEY, pcfg->strDealerID);
    }
    else
    {
        //读取配置文件失败
        MPU_ERR("no CConfig instance\n");
    }

    return IDVR_OK;
}

//!\brief        获取设备云存储录像配置
//!\return    0表示成功，其它为错误码
int MPU_PUB_GetCloudStorageRecordCfg(HY_DVR_CLOUD_STROAGE_RECORD_PLAN *pcfg)
{
    CConfig *pConfig = NULL;
    char key_str[256] = {0};
    char value[256] = {0};
    int i = 0;
    int j = 0;

    if (NULL == pcfg)
    {
        MPU_ERR("NULL == pcfg\n");
        return -1;
    }

    //从配置文件初始化
    pConfig = CConfig::GetInstance();
    if (NULL != pConfig)
    {
        //从配置文件中获取
        pcfg->nRecordMode = pConfig->GetValue(CLOUD_STORAGE_INFO_SECTION, CLOUD_STORAGE_INFO_RECORD_MODE_KEY, 0);
        pcfg->nStreamMode = pConfig->GetValue(CLOUD_STORAGE_INFO_SECTION, CLOUD_STORAGE_INFO_STREAM_MODE_KEY, 0);
        for (i = 0; i < DVR_MAX_DAYS; i++)
        {
            snprintf(key_str, sizeof(key_str), "ScheduleCfgV2_%01d", i);
            pConfig->GetValue(CLOUD_STORAGE_INFO_SECTION, key_str, s_szCloudStorageScheduletime, value, sizeof(value));
            for (j = 0; j < DVR_MAX_TIMESEGMENT; j++)
            {
                pcfg->recordPlan[i][j] = value[j] - '0';
            }
        }
    }
    else
    {
        //读取配置文件失败
        MPU_ERR("no CConfig instance\n");
    }

    return IDVR_OK;
}

//!\brief        设置设备云存储录像配置
//!\return    0表示成功，其它为错误码
int MPU_PUB_SetCloudStorageRecordCfg(HY_DVR_CLOUD_STROAGE_RECORD_PLAN *pcfg)
{
    CConfig *pConfig = NULL;
    char key_str[256] = {0};
    char value[256] = {0};
    int i = 0;
    int j = 0;

    if (NULL == pcfg)
    {
        MPU_ERR("NULL == pcfg\n");
        return -1;
    }

    //从配置文件初始化
    pConfig = CConfig::GetInstance();
    if (NULL != pConfig)
    {
        pConfig->SetValue(CLOUD_STORAGE_INFO_SECTION, CLOUD_STORAGE_INFO_RECORD_MODE_KEY, pcfg->nRecordMode);
        pConfig->SetValue(CLOUD_STORAGE_INFO_SECTION, CLOUD_STORAGE_INFO_STREAM_MODE_KEY, pcfg->nStreamMode);
        for (i = 0; i < DVR_MAX_DAYS; i++)
        {
            for (j = 0; j < DVR_MAX_TIMESEGMENT; j++)
            {
                value[j] = pcfg->recordPlan[i][j] + '0';
            }
            value[j] = 0;
            snprintf(key_str, sizeof(key_str), "ScheduleCfgV2_%01d", i);

            pConfig->SetValue(CLOUD_STORAGE_INFO_SECTION, key_str, value);
        }
    }
    else
    {
        //读取配置文件失败
        MPU_ERR("no CConfig instance\n");
    }

    return IDVR_OK;
}

int MPU_PUB_GetCloudStorageRecordCfg_V2(HY_DVR_CLOUD_STROAGE_RECORD_PLAN *pcfg, int nChannelMaxNum)
{
    CConfig *pConfig = NULL;
    char key_str[256] = {0};
    char value[256] = {0};
    int i = 0;
    int j = 0;
    int k = 0;

    if (NULL == pcfg)
    {
        printf("NULL == pcfg\n");
        return -1;
    }

    if (SYSTEM_SUPPORT_CHANNEL > nChannelMaxNum)
    {
        printf("buffer is too small, nChannelNum = %d\n", SYSTEM_SUPPORT_CHANNEL);
        return -1;
    }

    //从配置文件初始化
    pConfig = CConfig::GetInstance();
    if (NULL != pConfig)
    {
        for (k = 0; k < SYSTEM_SUPPORT_CHANNEL; k++)
        {
            snprintf(key_str, sizeof(key_str), CLOUD_STORAGE_INFO_RECORD_MODE_KEY"_CH%01d", k);
            pcfg[k].nRecordMode = pConfig->GetValue(CLOUD_STORAGE_INFO_SECTION, key_str, 0);

            if (k != 0)
            {
                /* AIOT设备强制非通道0为子码流录像 */
                pcfg[k].nStreamMode = 1;
            }
            else
            {
                memset(key_str, 0, sizeof(key_str));
                snprintf(key_str, sizeof(key_str),CLOUD_STORAGE_INFO_STREAM_MODE_KEY"_CH%01d", k);
                pcfg[k].nStreamMode = pConfig->GetValue(CLOUD_STORAGE_INFO_SECTION, key_str, 0);
            }

            for (i = 0; i < DVR_MAX_DAYS; i++)
            {
                memset(key_str, 0, sizeof(key_str));
                snprintf(key_str, sizeof(key_str),"ScheduleCfgV2_CH%01d_%01d", k, i);
                pConfig->GetValue(CLOUD_STORAGE_INFO_SECTION, key_str, s_szCloudStorageScheduletime, value, sizeof(value));
                for (j = 0; j < DVR_MAX_TIMESEGMENT; j++)
                {
                    pcfg[k].recordPlan[i][j] = value[j] - '0';
                }
            }

            pcfg[k].nChannel = k;
        }
    }
    else
    {
        //读取配置文件失败
        printf("no CConfig instance\n");
    }

    return IDVR_OK;
}

//!\brief        设置设备云存储录像配置
//!\return    0表示成功，其它为错误码
int MPU_PUB_SetCloudStorageRecordCfg_V2(HY_DVR_CLOUD_STROAGE_RECORD_PLAN *pcfg, int nChannelMaxNum)
{
    CConfig *pConfig = NULL;
    char key_str[256] = {0};
    char value[256] = {0};
    int i = 0;
    int j = 0;
    int k = 0;

    if (NULL == pcfg)
    {
        printf("NULL == pcfg\n");
        return -1;
    }

    int nChannelNum = (SYSTEM_SUPPORT_CHANNEL > nChannelMaxNum ? nChannelMaxNum : SYSTEM_SUPPORT_CHANNEL);

    //从配置文件初始化
    pConfig = CConfig::GetInstance();
    if (NULL != pConfig)
    {
        for (k = 0; k < nChannelNum; k++)
        {
            snprintf(key_str, sizeof(key_str),CLOUD_STORAGE_INFO_RECORD_MODE_KEY"_CH%01d", k);
            pConfig->SetValue(CLOUD_STORAGE_INFO_SECTION, key_str, pcfg->nRecordMode);

            memset(key_str, 0, sizeof(key_str));
            snprintf(key_str, sizeof(key_str), CLOUD_STORAGE_INFO_STREAM_MODE_KEY"_CH%01d", k);

            if (k != 0)
            {
                /* AIOT设备强制非通道0为子码流录像 */
                pConfig->SetValue(CLOUD_STORAGE_INFO_SECTION, key_str, 1);
            }
            else
            {
                pConfig->SetValue(CLOUD_STORAGE_INFO_SECTION, key_str, pcfg->nStreamMode);
            }

            for (i = 0; i < DVR_MAX_DAYS; i++)
            {
                for (j = 0; j < DVR_MAX_TIMESEGMENT; j++)
                {
                    value[j] = pcfg->recordPlan[i][j] + '0';
                }
                value[j] = 0;

                snprintf(key_str, sizeof(key_str), "ScheduleCfgV2_CH%01d_%01d", k, i);
                pConfig->SetValue(CLOUD_STORAGE_INFO_SECTION, key_str, value);
            }
        }
    }
    else
    {
        //读取配置文件失败
        printf("no CConfig instance\n");
    }

    return IDVR_OK;
}
