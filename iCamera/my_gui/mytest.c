#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <unistd.h>
#include <assert.h>
#include <fcntl.h>
#include <pthread.h>
#include <wchar.h>
#include <time.h>
#include <math.h>
#include <net/if.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/file.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/if_ether.h>
#include <net/route.h>
#include <netdb.h>
#include <linux/ethtool.h>
#include <linux/sockios.h>
#include <linux/mii.h>
#include <arpa/inet.h>
#include <netpacket/packet.h>
#include <sys/signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>

#include "zwsdk.h"
#include "idvr_errno.h"
#include "include/MPU_API.h"
#include "tagPtzCode.h"
#include "SYS/CBaseUpdate.h"
#include "Authority.h"
#include "RNL/idvr_rnl.h"
#include "mysystem.h"
#include "tagLog.h"
#include "CConfig.h"
#include "INet.h"
#include "base64Lib.h"
#include "mxml.h"
#include "enumRecType.h"
#include "base64Lib.h"
#include "MPU/MPU_global.h"
#include "common_pub.h"
#include "PRO/CProCustom.h"
#include "PUB/PUB_LightBoard.h"
#include "RShellCmd.h"
#include "PUB_BoradInfo.h"
#include "MPU_SYS_API.h"

#ifdef __AGINGTEST__
/* 云台控制相关 */
#include "PTZ/CPtzManager.h"
#include "motoLib.h"
#endif
#ifdef __IS_SUPPORT_AIC_BLUETOOTH__
#include "XRSBLE_L2.h"
#endif

extern "C"
{
    #include "xtrace.h"
}

#if defined (__IS_SUPPORT_PTZ_BUILTIN__) && defined (__IS_SUPPORT_PTZ_BUILTIN_HUMAN_TRACK__)
/* 云台控制相关 */
#include "PTZ/CPtzManager.h"
#include "motoLib.h"
#endif

#include "../PAR/CPubFunc.h"

#ifndef THREAD_RENAME
#define THREAD_RENAME do {\
    MAIN_LOG_I("[%s-%s-%d] thread_rename\n", __FILE__, __func__, __LINE__);\
    prctl(PR_SET_NAME, __func__, 0, 0, 0);\
}while(0)
#endif

/* MAIN模块日志接口 */
#define MAIN_LOG_E(format...)           LOG_E(LOG_MODULE_MAIN,format)
#define MAIN_LOG_W(format...)           LOG_W(LOG_MODULE_MAIN,format)
#define MAIN_LOG_I(format...)           LOG_I(LOG_MODULE_MAIN,format)
#define MAIN_LOG_D(format...)           LOG_D(LOG_MODULE_MAIN,format)
#define MAIN_LOG_H(desc,data,len)       LOG_H(LOG_MODULE_MAIN,desc,data,len)

#define BOA_CONF_PATH "/mnt/mtd/etc/conf.d/boa/boa.conf"

#define CMD_DROP_CACHE_2 "echo 2 > /proc/sys/vm/drop_caches"

//为了保险起见
static pthread_mutex_t g_DeviceCapLock = PTHREAD_MUTEX_INITIALIZER;

#ifdef __AGINGTEST__
//老化测试功能
typedef struct
{
    int aging_time; 		//老化总时长:-1~45000分钟，其中-1表示一直进行老化测试
    int ptz_enable;			//云台老化使能：0|1
    int ptz_time;
    int ptz_speed;
    int led_white_enable;	//白光灯老化使能：0|1
    int led_white_time;
    int led_red_enable;		//红外灯老化使能：0|1
    int led_red_time;
    int ir_cut_enable;		//ir cut老化使能：0|1
    int ir_cut_time;
    int audio_out_enable;	//音频老化使能：0|1
    int audio_out_time;
    int alarm_out_enable; 	//报警输出使能：0|1
    int alarm_out_time;
}AGI_InitParams_st;

typedef enum _StatusRet{
    ERR_NON = 0,
    ERR_MEM,
    ERR_FILE,
    ERR_INI,
    ERR_NON_EXIST,
}StatusRet;

enum STATE{
    STATE_IDLE,
    STATE_COMMEMT,
    STATE_SECT,
    STATE_KEY,
    STATE_VALUE,
};

//云台移动位置
enum PTZMOTION{
    PTZ_TOPOS_VMIN_HMIN,
    PTZ_TOPOS_VMIN_HMAX,
    PTZ_TOPOS_VMAX_HMAX,
    PTZ_TOPOS_VMAX_HMIN,
};
#endif

//  结构定义
typedef struct tagGUIState
{
    int iAlarmInState;
    int iEncodeCh;							//实际编码通道
    int iAlarmInNum;
    int iAlarmOutNum;
} T_GUIState, *PT_GUIState;					//!< GUI内部使用的状态

typedef enum enumShutDownType
{
    eHalt,
    eReboot
} eShutDownType;

typedef struct tagPrvChannel
{
    unsigned int ulMotionTimeLast;
    unsigned int ulHumanShapeTimeLast;
    unsigned int ulSoundTimeLast;
} T_CHANNELSTATE, *PT_CHANNELSTATE;

typedef struct search_device_ability
{
    const char *section;
    const char *key;
    int  *value;
}SDA,*pSDA;

typedef struct set_xml_tree
{
    const char *p;
    int value;
}SXT,*pSXT;

typedef struct tagRebootParam{
    int nSleepSeconds;      /* 需要睡眠的时间 */
    int bUpdateOps;         /* 当前操作是否是升级包升级 */
}T_REBOOT_PARAM, *PT_REBOOT_PARAM;

//  全局变量
static int uninitFlag = 0;
static sem_t m_semStartRebootOpsThread;
static T_GUIState g_tGuiState;			//!< 设置界面用到的信息
static T_REBOOT_PARAM m_stuRebootParam;
static T_CHANNELSTATE tChannelsState[MAX_ENC_CHANNEL_NUM];	// 通道信息

#if defined (__USE_FTP__)
static int g_last_alarm_type = -1;
static unsigned int g_last_alarm_comming = 0;
#endif

static int s_bIsUpdate = 0;
static HANDLE s_hUpdateMask = (HANDLE) 0xabcd4321;
static pthread_mutex_t s_hUpdateLock = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t s_hMDLock = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t s_hHumanShapeLock = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t s_hSoundLock = PTHREAD_MUTEX_INITIALIZER;


#ifdef __IS_SUPPORT_AIC_BLUETOOTH__
//蓝牙配网线程
static void *Fnx_BlueToothConfigureNetThread(void *lParam);
//蓝牙请求命令返回线程
static void *Fnx_BlueToothReqRetThread(void *lParam);
#endif


int SetBit32(int *piBit32, int iIndex, int iValue)
{
    if (iIndex < 0 || iIndex >= 32)
        return -1;
    if (0 == iValue)
        *piBit32 = (*piBit32 & (~(1 << iIndex)));
    else
        *piBit32 = (*piBit32 | 1 << iIndex);
    return 0;
}

int GetBit32(int iBit32, int iIndex)
{
    if (iIndex < 0 || iIndex >= 32)
        return -1;
    return (0 != (iBit32 & 1 << iIndex));
}

int IPFromString(const char *szIPAddr, DWORD * pdwIPAddr)
{
    if (NULL == szIPAddr || NULL == pdwIPAddr)
    {
        return -1;
    }

    struct in_addr inaddr;

    if (inet_aton(szIPAddr, &inaddr))
    {
        *pdwIPAddr = (DWORD) ntohl(inaddr.s_addr);
    }
    else
    {
        *pdwIPAddr = 0;
        return -1;
    }

    return 0;
}

//!\brief       报警、异常联动抓图
//!\return  成功为0，其它为错误码
static int AlarmAction_CapturePic(int nCh, char *szFileName)
{
    struct timeval now_tv;

    gettimeofday(&now_tv, NULL);
    //把与触发时间最接近的2张图片cp到/tmp/picture/pic_foremail
    MPU_STR_TrigEventCapturePic(0, now_tv.tv_sec, now_tv.tv_usec);

    ///tmp/picture/pic_foremail里的文件字符串放到szFileName里
    MPU_STR_SnapShotM_GetLastPic(nCh, 0, szFileName);
    MAIN_LOG_I("\n\n***** AlarmAction_CapturePic szFileName = %s\n\n", szFileName);
    //类似/tmp/picture/pic_for_email/20161230112904766.jpg /tmp/picture/pic_for_email/20161230112904490.jpg

    return 0;

}

#ifdef __USE_EMAIL__
//!\brief       报警、异常联动Email
//!\return  成功为0，其它为错误码
static void AlarmAction_MakeEmail(int nAlarmType, int nChannel, char *pszTopic, char *pszContext, char *pszAffix, int pszAffixSize)
{
    DEVICECAP tDeviceCap;

    DEVICEVERSION tDeviceVer;

    int i=0, nCount=0;

    char szIpAddress[20] = {0};
    char szAlias[21]     = {0};
    char ifname[16]      = {0};
    char szUserEmailBody[NET_MAX_EMAIL_BODY_LEN] 	={0};
    time_t tCurrent;

    struct tm tmCurrent;

    //初始化
    *pszTopic = 0;
    *pszContext = 0;
    int ContextLen = 0;
    char acContext[1024] = {0};

    MAIN_LOG_I("\n\n***MakeEmail nAlarmType=%d\n\n",nAlarmType);
    // 获取网络地址,根据网络连接方式
    if (MPU_PPPoE_GetEnable())
    {
        // PPPoE方式
        if (2 == MPU_PPPoE_IsConnected())
        {
            MPU_PPPoE_GetIPAddress(szIpAddress);
        }
    }
    else
    {
        // DHCP方式 静态IP方式(需要获取当前使用的是哪一个网卡才能获取对应的MAC地址)
            GetCurrNetCardType(ifname, sizeof(ifname));

        if (0 == strcmp(ifname, "eth0"))
        {
            MPU_NET_GetIPAddress(szIpAddress);
        }
        else
        {
            #if defined (__IS_SUPPORT_WLAN__)
            int bSdkSupportWifi = 0;
            ZWSys_GetWifiExist(&bSdkSupportWifi);
            if (bSdkSupportWifi)
            {
                MPU_WLAN_GetIPAddress(szIpAddress);
            }
            #endif
        }
    }
    memset(&tDeviceCap, 0, sizeof(tDeviceCap));
    MPU_PUB_GetDeviceCap(&tDeviceCap);

    memset(&tDeviceVer, 0, sizeof(tDeviceVer));
    MPU_PUB_GetDeviceVersion(&tDeviceVer);

    MPU_PUB_GetDevAlias(szAlias, sizeof(szAlias));
    MPU_EML_GetBody(szUserEmailBody, NET_MAX_EMAIL_BODY_LEN);

    //正文
    ContextLen += snprintf(acContext+ContextLen, (sizeof(acContext) - ContextLen), "[User Info]\n %s\n\n", szUserEmailBody);
    ContextLen += snprintf(acContext+ContextLen, (sizeof(acContext) - ContextLen), "[Device Info]\n");
    ContextLen += snprintf(acContext+ContextLen, (sizeof(acContext) - ContextLen), "Device Name: %s\n", szAlias);
    //去掉版型
    //ContextLen += snprintf(acContext+ContextLen, (sizeof(acContext) - ContextLen), "Device Model: %s\n", tDeviceCap.m_szTypeName);
    ContextLen += snprintf(acContext+ContextLen, (sizeof(acContext) - ContextLen), "IP Address: %s\n", szIpAddress);
    ContextLen += snprintf(acContext+ContextLen, (sizeof(acContext) - ContextLen), "Serial Number: %s\n",  tDeviceVer.m_szSerialNum);
    ContextLen += snprintf(acContext+ContextLen, (sizeof(acContext) - ContextLen), "Hardware Version: %s\n", tDeviceVer.m_szHardwareVersion);
    ContextLen += snprintf(acContext+ContextLen, (sizeof(acContext) - ContextLen), "Software Version: %s\n", tDeviceVer.m_szSoftwareVersion);
    ContextLen += snprintf(acContext+ContextLen, (sizeof(acContext) - ContextLen), "Firmware Version: %s\n", tDeviceVer.m_szDspVersion);

    ContextLen += snprintf(acContext+ContextLen, (sizeof(acContext) - ContextLen), "\n");
    ContextLen += snprintf(acContext+ContextLen, (sizeof(acContext) - ContextLen), "[Event Info]\n");

    MAIN_LOG_I("\n\n***MakeEmail szUserEmailBody=%s\n\n",szUserEmailBody);
    MAIN_LOG_I("\n\n***MakeEmail Device Model=%s\n\n",tDeviceCap.m_szTypeName);
    MAIN_LOG_I("\n\n***MakeEmail szAlias=%s\n\n",szAlias);

    switch (nAlarmType)
    {
        case ALERT_TYPE_MOTION:
            {
                //正文
                ContextLen += snprintf(acContext+ContextLen, (sizeof(acContext) - ContextLen), "Type: Motion Detection\n");
                ContextLen += snprintf(acContext+ContextLen, (sizeof(acContext) - ContextLen), "Channel: %d\n", nChannel + 1);

                //附件
                nCount = MPU_AVL_GetChNum();
                for (i = 0; i < nCount; ++i)
                {
                    //抓拍快照
                    if (0 != AlarmAction_CapturePic(i, pszAffix))
                    {
                        MAIN_LOG_I("AlarmAction_CapturePic error \n");
                    }
                    MAIN_LOG_I("\n\n***make email pszAffix=%s\n\n",pszAffix);
                }
                break;
            }

        case ALERT_TYPE_HUMANSHAPE:
            {
                //正文
                ContextLen += snprintf(acContext+ContextLen, (sizeof(acContext) - ContextLen), "Type: Human Shape Detection\n");
                ContextLen += snprintf(acContext+ContextLen, (sizeof(acContext) - ContextLen), "Channel: %d\n", nChannel + 1);

                //附件
                nCount = MPU_AVL_GetChNum();
                for (i = 0; i < nCount; ++i)
                {
                    //抓拍快照
                    if (0 != AlarmAction_CapturePic(i, pszAffix))
                    {
                        MAIN_LOG_I("AlarmAction_CapturePic error \n");
                    }
                    MAIN_LOG_I("\n\n***make email pszAffix=%s\n\n",pszAffix);
                }
                break;
            }

        case ALERT_TYPE_SOUNDDETECT:
            {
                //正文
                ContextLen += snprintf(acContext+ContextLen, (sizeof(acContext) - ContextLen), "Type: Sound Detection\n");
                ContextLen += snprintf(acContext+ContextLen, (sizeof(acContext) - ContextLen), "Channel: %d\n", nChannel + 1);

                //附件
                nCount = MPU_AVL_GetChNum();
                for (i = 0; i < nCount; ++i)
                {
                    //抓拍快照
                    if (0 != AlarmAction_CapturePic(i, pszAffix))
                    {
                        MAIN_LOG_I("AlarmAction_CapturePic error \n");
                    }
                    MAIN_LOG_I("\n\n***make email pszAffix=%s\n\n",pszAffix);
                }
                break;
            }

        default:
            {
                //不支持的报警类型

                //标题
                snprintf(pszTopic, 256, "Exception [%d] from %s [%s]", nAlarmType, tDeviceCap.m_szTypeName, szIpAddress);

                //正文
                ContextLen += snprintf(acContext+ContextLen, (sizeof(acContext) - ContextLen), "Type: %d\n", nAlarmType);
                break;
            }

    }

    //正文－时间
    time(&tCurrent);
    localtime_r(&tCurrent, &tmCurrent);

    ContextLen += snprintf(acContext+ContextLen, (sizeof(acContext) - ContextLen), "Date: %04d-%02d-%02d\n", 1900 + tmCurrent.tm_year, 1 + tmCurrent.tm_mon, tmCurrent.tm_mday);
    ContextLen += snprintf(acContext+ContextLen, (sizeof(acContext) - ContextLen), "Time: %02d:%02d:%02d\n", tmCurrent.tm_hour, tmCurrent.tm_min, tmCurrent.tm_sec);

    //memcpy(pszContext, acContext, strlen(acContext));
    memcpy(pszContext, acContext, ContextLen);
}
#endif

//!\brief   判断是否联动
//!\return  成功为0，其它为错误码
static bool AlarmAction_IsLink(int nAlarmType, int nChannel, int nActionType)
{
    bool bEnable = true;

    switch (nAlarmType)
    {
        case ALERT_TYPE_MOTION:
        {
            //移动侦测
            if (nChannel < 0 || nChannel >= MPU_AVL_GetChNum())
            {
                MAIN_LOG_I("ALERT_TYPE_ALARMIN %d invalid channel\n", nChannel);
                bEnable = false;
            }
            else
            {
                if (nActionType == ACTION_EMAIL)
                {
                    if (MPU_AVL_IsEnableAction(nChannel, ALERT_TYPE_MOTION, ACTION_EMAIL) <= 0)
                    {
                        //MAIN_LOG_I("ALERT_TYPE_MOTION %d not link email\n", nChannel);
                        bEnable = false;
                    }
                }
                else if (nActionType == ACTION_FTP)
                {
                    if (MPU_AVL_IsEnableAction(nChannel, ALERT_TYPE_MOTION, ACTION_FTP) <= 0)
                    {
                        //MAIN_LOG_I("ALERT_TYPE_MOTION %d not link ftp\n", nChannel);
                        bEnable = false;
                    }
                }
                else if (nActionType == ACTION_SNAPSHOT)
                {
                    if (MPU_AVL_IsEnableAction(nChannel, ALERT_TYPE_MOTION, ACTION_SNAPSHOT) <= 0)
                    {
                        //MAIN_LOG_I("ALERT_TYPE_MOTION %d not link snapshot\n", nChannel);
                        bEnable = false;
                    }
                }
                else if (nActionType == ACTION_PUSH)
                {
                    if (MPU_AVL_IsEnableAction(nChannel, ALERT_TYPE_MOTION, ACTION_PUSH) <= 0)
                    {
                        //MAIN_LOG_I("ALERT_TYPE_MOTION %d not link ACTION_PUSH\n", nChannel);
                        bEnable = false;
                    }
                }
            }
            break;
        }

        case ALERT_TYPE_HUMANSHAPE:
        {
            //人形侦测
            if (nChannel < 0 || nChannel >= MPU_AVL_GetChNum())
            {
                MAIN_LOG_I("ALERT_TYPE_HUMANSHAPE %d invalid channel\n", nChannel);
                bEnable = false;
            }
            else
            {
                if (nActionType == ACTION_EMAIL)
                {
                    if (MPU_AVL_IsEnableAction(nChannel, ALERT_TYPE_HUMANSHAPE, ACTION_EMAIL) <= 0)
                    {
                        //MAIN_LOG_I("ALERT_TYPE_HUMANSHAPE %d not link email\n", nChannel);
                        bEnable = false;
                    }
                }
                else if (nActionType == ACTION_FTP)
                {
                    if (MPU_AVL_IsEnableAction(nChannel, ALERT_TYPE_HUMANSHAPE, ACTION_FTP) <= 0)
                    {
                        //MAIN_LOG_I("ALERT_TYPE_HUMANSHAPE %d not link ftp\n", nChannel);
                        bEnable = false;
                    }
                }
                else if (nActionType == ACTION_SNAPSHOT)
                {
                    if (MPU_AVL_IsEnableAction(nChannel, ALERT_TYPE_HUMANSHAPE, ACTION_SNAPSHOT) <= 0)
                    {
                        //MAIN_LOG_I("ALERT_TYPE_HUMANSHAPE %d not link snapshot\n", nChannel);
                        bEnable = false;
                    }
                }
                else if (nActionType == ACTION_PUSH)
                {
                    if (MPU_AVL_IsEnableAction(nChannel, ALERT_TYPE_HUMANSHAPE, ACTION_PUSH) <= 0)
                    {
                        //MAIN_LOG_I("ALERT_TYPE_HUMANSHAPE %d not link ACTION_PUSH\n", nChannel);
                        bEnable = false;
                    }
                }
            }
            break;
        }

        case ALERT_TYPE_SOUNDDETECT:
        {
            //哭声侦测
            if (nChannel < 0 || nChannel >= MPU_AVL_GetChNum())
            {
                MAIN_LOG_I("ALERT_TYPE_SOUNDDETECT %d invalid channel\n", nChannel);
                bEnable = false;
            }
            else
            {
                if (nActionType == ACTION_EMAIL)
                {
                    if (MPU_AVL_IsEnableAction(nChannel, ALERT_TYPE_SOUNDDETECT, ACTION_EMAIL) <= 0)
                    {
                        MAIN_LOG_I("ALERT_SOUNDDETECT %d not link email\n", nChannel);
                        bEnable = false;
                    }
                }
                else if (nActionType == ACTION_FTP)
                {
                    if (MPU_AVL_IsEnableAction(nChannel, ALERT_TYPE_SOUNDDETECT, ACTION_FTP) <= 0)
                    {
                        MAIN_LOG_I("ALERT_SOUNDDETECT %d not link ftp\n", nChannel);
                        bEnable = false;
                    }
                }
                else if (nActionType == ACTION_SNAPSHOT)
                {
                    if (MPU_AVL_IsEnableAction(nChannel, ALERT_TYPE_SOUNDDETECT, ACTION_SNAPSHOT) <= 0)
                    {
                        MAIN_LOG_I("ALERT_SOUNDDETECT %d not link snapshot\n", nChannel);
                        bEnable = false;
                    }
                }
                else if (nActionType == ACTION_PUSH)
                {
                    if (MPU_AVL_IsEnableAction(nChannel, ALERT_TYPE_SOUNDDETECT, ACTION_PUSH) <= 0)
                    {
                        MAIN_LOG_I("ALERT_SOUNDDETECT %d not link ACTION_PUSH\n", nChannel);
                        bEnable = false;
                    }
                }
            }
            break;
        }

        default:
            {
                MAIN_LOG_I("unsupported alarm type %d\n", nAlarmType);
                bEnable = false;
                break;
            }
    }

    return bEnable;
}

//!\brief       报警、异常联动Email
//!\return  成功为0，其它为错误码
int AlarmAction_SendEmail(int nAlarmType, int nChannel)
{
    int nRet = 0;

#ifdef __USE_EMAIL__
    int nCnt, nEmailNum, nLink;

    HANDLE hUserList[IDVR_MAX_USER];

    char *pszEmailList[IDVR_MAX_USER];

    char szTopic[100] = {0};

    char szContext[1024] = {0};

    char szAffix[128] = {0};

    unsigned int bTimeIntervalArrival = 0;

    //判断是否允许Email联动
    if (!AlarmAction_IsLink(nAlarmType, nChannel, ACTION_EMAIL))
    {
        //MAIN_LOG_I("not link email\n");
        return -2;
    }
    MPU_EML_GetEmailTimeIntervalIsArrival(&bTimeIntervalArrival);
    if(0==bTimeIntervalArrival)
    {
        MAIN_LOG_I("email timeinterval is not arrival.\n");
        return -3;
    }
    //判断Email功能是否配置ok
    if (0 != MPU_EML_IsReady())
    {
        MAIN_LOG_I("invalid smtp server config\n");
        return -1;
    }

    //判断网线是否连接
    nLink = 1;
    nLink &= MPU_NET_GetStatus();
#ifdef USE_ICAMERA
#if defined (__IS_SUPPORT_WLAN__)
    int bSdkSupportWifi = 0;
    ZWSys_GetWifiExist(&bSdkSupportWifi);
    if (bSdkSupportWifi)
    {
        nLink &= MPU_WLAN_GetStatus();
    }
#endif

#endif
    if (1 == nLink)
    {
        MAIN_LOG_I("not connect in newwork\n");
        return -3;
    }

    //初始化用户邮件列表
    nEmailNum = 0;
    memset(pszEmailList, 0, sizeof (pszEmailList));

    nCount = 3;
    MAIN_LOG_I("User count is %d, %d\n", nCount, sizeof (pszEmailList));

    for (unsigned i = 0; i < nCount; ++i)
    {
        char szUserName[MAX_USERNAME_LEN];
        //申请内存
        if (NULL == pszEmailList[nEmailNum])
        {
            pszEmailList[nEmailNum] = new char[NET_MAX_EMAIL_LEN];

            if (NULL == pszEmailList[nEmailNum])
            {
                MAIN_LOG_I("User[%d] \"%s\" new %d bytes buffer fail\n", i, szUserName, NET_MAX_EMAIL_LEN);
                break;
            }
            else
            {
//				MAIN_LOG_I("new buffer %d\n", nEmailNum);
            }
        }

        if (0 != MPU_EML_GetRecvAddresser(pszEmailList[nEmailNum],NET_MAX_EMAIL_LEN,i))
        {
            MAIN_LOG_I("User[%d] \"%s\" get email fail\n", i, szUserName);
            continue;
        }

        //判断是否合法Email地址
        if (0 != MPU_EML_CheckEmail(pszEmailList[nEmailNum]))
        {
            MAIN_LOG_I("User[%d] \"%s\" email \"%s\" is invalid\n", i, szUserName, pszEmailList[nEmailNum]);
            continue;
        }

//		MAIN_LOG_I("User[%d] \"%s\" email \"%s\" is valid\n", i, szUserName, pszEmailList[nEmailNum]);

        ++nEmailNum;
    }

    //没有收件人
    if (0 == nEmailNum)
    {
        MAIN_LOG_I("no valid addressee email\n");
        nRet = -4;

        goto release_emaillist;
    }

    //生成标题
    AlarmAction_MakeEmail(nAlarmType, nChannel, szTopic, szContext, szAffix,sizeof(szAffix));

    if (0 != MPU_EML_SendList((char **) pszEmailList, nEmailNum, NULL, szContext, szAffix, 0))
    {
        MAIN_LOG_I("send %d email fail\n", nEmailNum);
        nRet = -5;
    }
    else
    {
        nRet = IDVR_OK;
    }


  release_emaillist:			//删除邮件列表缓冲区
    while (nEmailNum >= 0)
    {
        if (NULL != pszEmailList[nEmailNum])
        {
            delete[]pszEmailList[nEmailNum];
            pszEmailList[nEmailNum] = NULL;

//			MAIN_LOG_I("delete buffer %d\n", nEmailNum);
        }

        --nEmailNum;
    }
#endif
    return nRet;
}

int SendTestEmail(HANDLE hUser)
{
    int iRet = 0;
    int nEmailNum;	//存储实际收件地址个数
    int i;
    int SnapShotFlag=0;
    int ch_count=0;

    char *pszEmailList[IDVR_MAX_USER];//

    char szAddressee[EMAIL_LIMIT]  = {0};

    char szUser[NET_MAX_EMAIL_LEN] = {0};//发件地址
    char szIpAddress[20]           = {0};
    char szAlias[21]               = {0};
    char szContext[1024]            = {0};
    char szUserEmailBody[NET_MAX_EMAIL_BODY_LEN] 	={0};
    char ifname[16]                = {0};
    char szAffix[128]          = {0};//附件字符串组
    int szConCount = 0;

    iRet = MPU_EML_IsBusy();
    if (1 == iRet)
    {
        MAIN_LOG_I("\n--[%s][%s]--Now email is sending, try later\n", __FILE__, __FUNCTION__);
        return 0;
    }

    bzero(szAffix, sizeof(szAffix));

    MAIN_LOG_I("\n--%s------------%s-----------begin--------\n", __FILE__, __FUNCTION__);

    // DHCP方式 静态IP方式(需要获取当前使用的是哪一个网卡才能获取对应的MAC地址)
    GetCurrNetCardType(ifname, sizeof(ifname));

    if (0 == strcmp(ifname, "eth0"))
    {
        MPU_NET_GetIPAddress(szIpAddress);
    }
    else
    {
    #if defined (__IS_SUPPORT_WLAN__)
        int bSdkSupportWifi = 0;
        ZWSys_GetWifiExist(&bSdkSupportWifi);
        if (bSdkSupportWifi)
        {
            MPU_WLAN_GetIPAddress(szIpAddress);
        }
    #endif
    }

    if (0 == iRet)
    {
        // 发送测试邮件
        DEVICECAP tDeviceCap;

        DEVICEVERSION tDeviceVer;

        MPU_PUB_GetDeviceCap(&tDeviceCap);
        MPU_PUB_GetDeviceVersion(&tDeviceVer,0);
        MPU_PUB_GetDevAlias(szAlias, sizeof (szAlias));
        // MPU_USR_GetUserName(hUser, szUser, MAX_USERNAME_LEN);
        // MAIN_LOG_I("MPU_PUB_GetDevAlias: szAlias = %s\n", szAlias);

        MPU_EML_GetAddresserEmail(szUser, NET_MAX_EMAIL_LEN);
        MPU_EML_GetBody(szUserEmailBody, NET_MAX_EMAIL_BODY_LEN);
        SnapShotFlag=MPU_EML_GetPic_att();

        nEmailNum = 0;
        memset(pszEmailList, 0, sizeof (pszEmailList));
        for(i=0;i<3;i++)
        {
            // 申请内存
            if (NULL == pszEmailList[nEmailNum])
            {
                pszEmailList[nEmailNum] = new char[NET_MAX_EMAIL_LEN];

                if (NULL == pszEmailList[nEmailNum])
                {
                    MAIN_LOG_I("User[%d] new %d bytes buffer fail\n", i, NET_MAX_EMAIL_LEN);
                    break;
                }
                else
                {
                    // MAIN_LOG_I("new buffer %d\n", nEmailNum);
                }
            }
            if (0 != MPU_EML_GetRecvAddresser(pszEmailList[nEmailNum],NET_MAX_EMAIL_LEN,i))
            {
                MAIN_LOG_I("User[%d] get email fail\n", i);
                continue;
            }
            // 判断是否合法Email地址
            if (0 != MPU_EML_CheckEmail(pszEmailList[nEmailNum]))
            {
                MAIN_LOG_I("User[%d] \"%s\" email is invalid\n", i, pszEmailList[nEmailNum]);
                continue;
            }
            ++nEmailNum;
        }

        //正文
        szConCount += snprintf(szContext + szConCount, (sizeof(szContext) - szConCount), "This is a test email sent by user \"%s\".\n\n", szUser);
        szConCount += snprintf(szContext + szConCount, (sizeof(szContext) - szConCount), "[User Info]\n %s\n\n", szUserEmailBody);
        szConCount += snprintf(szContext + szConCount, (sizeof(szContext) - szConCount), "[Device Info]\n");
        szConCount += snprintf(szContext + szConCount, (sizeof(szContext) - szConCount), "Device Name: %s\n", szAlias);
        //去掉版型
        //szConCount += snprintf(szContext + szConCount, (sizeof(szContext) - szConCount), "Device Model: %s\n", tDeviceCap.m_szTypeName);
        szConCount += snprintf(szContext + szConCount, (sizeof(szContext) - szConCount), "IP Address: %s\n", szIpAddress);
        szConCount += snprintf(szContext + szConCount, (sizeof(szContext) - szConCount), "Serial Number: %s\n", tDeviceVer.m_szSerialNum);
        szConCount += snprintf(szContext + szConCount, (sizeof(szContext) - szConCount), "Hardware Version: %s\n", tDeviceVer.m_szHardwareVersion);
        szConCount += snprintf(szContext + szConCount, (sizeof(szContext) - szConCount), "Software Version: %s\n", tDeviceVer.m_szSoftwareVersion);
        szConCount += snprintf(szContext + szConCount, (sizeof(szContext) - szConCount), "Firmware Version: %s\n", tDeviceVer.m_szDspVersion);

        //附件
        //pszAffix += snprintf(pszAffix, sizeof(szAffix), " ");
        if(SnapShotFlag)
        {
            ch_count = MPU_AVL_GetChNum();

            for (i = 0; i < ch_count; ++i)
            {
                //抓拍快照
                if (0 == AlarmAction_CapturePic(i, szAffix))
                {
                    //pszAffix += snprintf(pszAffix, sizeof(szAffix), " \"%s\"", szFileName);
                    MAIN_LOG_I("\n\n*****pszAffix=%s\n\n",szAffix);
                }
            }
        }

        //远程发送测试邮件
        MAIN_LOG_I("\n--%s------------%s------szAddressee=%s----\n", __FILE__, __FUNCTION__, szAddressee);

        //没有收件人
        if (0 == nEmailNum)
        {
            MAIN_LOG_I("no valid addressee email\n");
            iRet = -4;
        }
        if (0 != MPU_EML_SendList((char **) pszEmailList, nEmailNum, NULL, szContext, szAffix, 1))
        {
            MAIN_LOG_I("send %d email fail\n", nEmailNum);
            iRet = -5;
        }
        else
        {
            MPU_EML_GetTestSendStatus(&iRet);
            MAIN_LOG_I("\n\n%s %s iRet=%d\n\n",__FILE__,__FUNCTION__,iRet);

        }
    }

    return iRet;
}

static int TrigWriteFileOverCallBack(unsigned char *file_name)
{
#if defined (__USE_FTP__)
    int ftp_trans_type = 0;
    char command[256] = {0};
    char acNewName[64] = {0};
    char *temp = NULL;
    time_t tCurrentTime = 0;
    struct tm tmnow = {0};
    char acTmp[128] = {0};
    int last_alarm_type_bak = 0;

    if (NULL == file_name)
    {
        MAIN_LOG_I("[%s-%s-%d] [ERR] NULL POINTER EXCEPTION!\n", __FILE__, __func__, __LINE__);
        return -1;
    }

    if(0 == GetFtpEnable())
    {
        //MAIN_LOG_I("ftp not enable.\n");
        return -1;
    }

    /* 判断是否允许FTP联动 */
    if (-1 == g_last_alarm_type
        || !AlarmAction_IsLink(g_last_alarm_type, 0, ACTION_FTP))
    {
        //MAIN_LOG_I("not link ftp\n");
        return -1;
    }

    /* 如果上次触发没有到来，则表明当前触发是定时录像完毕之后的触发，直接忽略 */
    if (0 == g_last_alarm_comming)
    {
        MAIN_LOG_I("last alarm is not comming ...\n");
        return -1;
    }

    last_alarm_type_bak = g_last_alarm_type;
    g_last_alarm_comming = 0;
    g_last_alarm_type = -1;

    /* 传输类型(0:不传 1:图片 2:录像 3:ALL ) */
    MPU_NET_GetFtpTransferType(&ftp_trans_type);

    /* 如果当前的FTP传输类型是录像或者ALL，且当前移动侦测触发录像功能关闭，则需要强制打开该功能 */
    if ((2==ftp_trans_type) || (3==ftp_trans_type))
    {
        //按时间格式生成文件名，不用传递过来的1.avi
        time(&tCurrentTime);
        localtime_r(&tCurrentTime, &tmnow);
           snprintf(acTmp, sizeof(acTmp), "%04d%02d%02d%02d%02d%02d000.avi",
               tmnow.tm_year+1900, tmnow.tm_mon+1,
               tmnow.tm_mday, tmnow.tm_hour,
               tmnow.tm_min, tmnow.tm_sec);

        switch (last_alarm_type_bak)
        {
            default:
            case ALERT_TYPE_MOTION:
            {
                snprintf(acNewName, sizeof(acNewName), "M_%s", acTmp);
                break;
            }

            case ALERT_TYPE_HUMANSHAPE:
            {
                snprintf(acNewName, sizeof(acNewName), "M_%s", acTmp);
                break;
            }
        }

        snprintf(command, sizeof(command), "cp %s/%s %s/%s", LOCAL_FTP_VIDEO_DIR, file_name, LOCAL_FTP_VIDEO_UPLOAD_DIR,acTmp);
        mysystem(command);

        /* 开始进行FTP文件传输(不能阻塞) */
        if(0 != SetFtpTransferName((unsigned char *)acTmp, (unsigned char *)acNewName))
        {
            MAIN_LOG_I("[%s-%s-%d][ERR]SetFtpTransferName %s to %s fail!\n",
                __FILE__, __func__, __LINE__, acTmp, acNewName);
            return -1;
        }
    }
    else
    {
        MAIN_LOG_I("Current ftp tigger type is %d not video\n", ftp_trans_type);
    }

    //MAIN_LOG_I("[%s-%d] ------ out %s ------ \n", __FILE__, __LINE__, __func__);
#endif

    return 0;
}

//!\brief	报警、异常联动Ftp
//!\return  成功为0，其它为错误码
int AlarmAction_SendFtp(int nAlarmType, int nChannel)
{
#if defined (__USE_FTP__)
    int ftp_trans_type = 0;
    char tmp[256] = {0};
    char szFileName[256] = {0};
    struct timeval now_tv = {0};
    static unsigned int s_ftp_old_time_sec = 0;

    if (0 == GetFtpEnable())
    {
        //MAIN_LOG_I("ftp not enable.\n");
        return -2;
    }

    //判断是否允许FTP联动
    if (!AlarmAction_IsLink(nAlarmType, nChannel, ACTION_FTP))
    {
        MAIN_LOG_I("not link ftp\n");
        return -2;
    }

    /* 传输类型(0:不传 1:图片 2:录像 3:ALL ) */
    MPU_NET_GetFtpTransferType(&ftp_trans_type);

    /* 如果当前触发类型为录像或者全部否触发，则需要记录当前触发状态 */
    if ((2==ftp_trans_type) || (3==ftp_trans_type))
    {
        g_last_alarm_comming = 1;
        g_last_alarm_type = nAlarmType;
    }

    if ((1==ftp_trans_type) || (3==ftp_trans_type))
    {
        gettimeofday(&now_tv, NULL);//获取时间

        if ((now_tv.tv_sec - s_ftp_old_time_sec) <= MIN_FTP_CAPTURE_SECOND_INTERVAL)
        {
            //MAIN_LOG_I("trigger ftp picture is busy, please wait ...\n");
            return 0;
        }

        /* 把与触发时间最接近的1张图片cp到/tmp/picture/pic_for_ftp */
        MPU_STR_TrigEventCapturePic(1, now_tv.tv_sec, now_tv.tv_usec);
        s_ftp_old_time_sec = now_tv.tv_sec;

        /* 获取最后一次抓图的文件名 */
        MPU_STR_SnapShotM_GetLastPic(nChannel, 1, tmp);

        /* FTP传输的时候需要添加前缀 */
        switch(nAlarmType)
        {
            case ALERT_TYPE_MOTION:
            {
                snprintf(szFileName, sizeof(szFileName), "M_%s", tmp);
                break;
            }
            case ALERT_TYPE_HUMANSHAPE:
            {
                snprintf(szFileName, sizeof(szFileName), "Human_%s", tmp);
                break;
            }
            case ALERT_TYPE_SOUNDDETECT:
            {
                snprintf(szFileName, sizeof(szFileName), "Sound_%s", tmp);
                break;
            }
            default:
            {
                snprintf(szFileName, sizeof(szFileName), "M_%s", tmp);
                break;
            }
        }

        /* 开始进行FTP文件传输(不能阻塞) */
        if(0 != SetFtpTransferName((unsigned char *)tmp, (unsigned char *)szFileName))
        {
            MAIN_LOG_I("[%s-%s-%d] [ERR] SetFtpTransferName %s to %s fail!\n",
                   __FILE__, __func__, __LINE__, tmp, szFileName);
            return -1;
        }

        MAIN_LOG_I("\n\n***** note ipc is going to send pic to ftp server. tmp=[%s]\n\n", tmp);
    }
#endif

    return 0;
}

//!\brief    报警、异常联动下载到SD卡
//!\return  成功为0，其它为错误码
int AlarmAction_UploadSdCard(int nAlarmType, int nChannel)
{
    char szFileName[64] = {0};
    char tmp[128] = {0};
    struct timeval now_tv = {0};
    static unsigned int s_sd_old_time_sec = 0;
    int minSdCapturetimeInterval = MIN_SD_CAPTURE_SECOND_INTERVAL;
    int nEnbale = 0;

    /* 判断是否允许SD联动 */
    if (!AlarmAction_IsLink(nAlarmType, nChannel, ACTION_SNAPSHOT))
    {
        MAIN_LOG_I("not link sd card snapshot!\n");
        return -2;
    }

    gettimeofday(&now_tv, NULL);//获取时间

    MPU_STR_GetSdSnapShotParam(&nEnbale, &minSdCapturetimeInterval);

    if (abs(int(now_tv.tv_sec - s_sd_old_time_sec) <= minSdCapturetimeInterval))
    {
        MAIN_LOG_I("trigger sd is busy, please wait ...\n");
        return 0;
    }

    /* 把与触发时间最接近的1张图片cp到/tmp/picture/pic_for_sd */
    MPU_STR_TrigEventCapturePic(2, now_tv.tv_sec, now_tv.tv_usec);
    s_sd_old_time_sec = now_tv.tv_sec;

    /* 获取最后一次抓图的文件名("时间戳_标号"组成文件名) */
    MPU_STR_SnapShotM_GetLastPic(nChannel, 2, szFileName);

    /* 1. SD卡里面已经根据当前传入的类型进行添加前缀了;
     * 2. 传入的图片名字组成pic_name: 时间戳_标号，如:1532477851_01需要改为1532477851
     */
    //MPU_AVL_UploadPictureToSdCard(nChannel, szFileName, nAlarmType);
    switch (nAlarmType)
    {
        case ALERT_TYPE_MOTION:
        {
            MPU_AVL_UploadPictureToSdCard(nChannel, szFileName, 4);
            break;
        }
        case ALERT_TYPE_HUMANSHAPE:
        {
            MPU_AVL_UploadPictureToSdCard(nChannel, szFileName, 6);
            break;
        }
        case ALERT_TYPE_SOUNDDETECT:
        {
            MPU_AVL_UploadPictureToSdCard(nChannel, szFileName, 7);
            break;
        }
    }

    /* 如果出错或者无SD卡，也需要删除原始文件，避免出现内存里面文件过多 */
    snprintf(tmp, sizeof(tmp), "%s/%s",  CAPTURE_MEM_DIR_FOR_SD, szFileName);
    SAFE_REMOVE(tmp);

    return 0;
}

int SendTestFtp(HANDLE hUser)
{
    int ret=0;

#if defined (__USE_FTP__)
    MAIN_LOG_I("\n\n[%s][%d] SendTestFtp \n\n",__FUNCTION__,__LINE__);
    ret=SendFtpTestFile();		//ftp测试文件调用ftplib的接口
#endif

    return ret;
}

int SysUpdateStart()
{
    HANDLE bUpdate = NULL;

    int nResult = 0;

    pthread_mutex_lock(&s_hUpdateLock);

//  MAIN_LOG_I("1. s_bIsUpdate: %d, s_hUpdateMask: %d\n", s_bIsUpdate, s_hUpdateMask);

    if (!s_bIsUpdate)
    {
        bUpdate = (HANDLE) (((int) time(NULL)) | 0xabcd0123);
        s_hUpdateMask = bUpdate;
        s_bIsUpdate = 1;
        nResult = (int) bUpdate;
    }
    pthread_mutex_unlock(&s_hUpdateLock);

//  MAIN_LOG_I("2. s_bIsUpdate: %d, s_hUpdateMask: %d\n", s_bIsUpdate, s_hUpdateMask);


    return nResult;
}

int SysUpdateStop(DWORD hUpdate)
{
    int nResult = 0;

    pthread_mutex_lock(&s_hUpdateLock);
    if (hUpdate != (DWORD) s_hUpdateMask)
    {
        nResult = -1;
    }
    s_bIsUpdate = 0;
    s_hUpdateMask = (HANDLE) 0xabcd4321;
    pthread_mutex_unlock(&s_hUpdateLock);

//  MAIN_LOG_I("3. s_bIsUpdate: %d, s_hUpdateMask: %d\n", s_bIsUpdate, s_hUpdateMask);

    return nResult;
}

/* 恢复默认时需要恢复ULK为使能 */
static int RecoverUlkEnable(void)
{
    FILE *fd=NULL;
    char *point = NULL;
    char buff[32];
    char result[32];

    fd = fopen("/mnt/mtd/config/ulkuid.conf", "w+");
    if (fd == NULL)
    {
        MAIN_LOG_I("open file failed [%s]\n", "/mnt/mtd/config/ulkuid.conf");
        fclose(fd);
        return -1;
    }

    fgets(result, 32, fd);

    point = strstr(result, "0:");
    if (point != NULL)
    {
        MAIN_LOG_I("Has disable ULK!\n");
        fclose(fd);
        return 0;
    }

    memset(buff, 0, sizeof (buff));
    strncpy(buff, "1:",sizeof(buff) - 1);

    fwrite(buff, strlen(buff), 1, fd);

    fflush(fd);
    fclose(fd);

    return 0;
}

/* 通用的线程，用于升级或者配置导入等需要等待网络返回之后的重启操作 */
static void* common_reboot(void * lparam)
{
    PRT_TID();
    pthread_detach(pthread_self());

    THREAD_RENAME;

    sem_wait(&m_semStartRebootOpsThread);

    /* 睡眠等待返回给网络层，防止网络层塞住 */
    if (m_stuRebootParam.nSleepSeconds)
    {
        sleep(m_stuRebootParam.nSleepSeconds);
    }

	RebootLinux(REBOOT_REMOTE_OPERATION);
    //REBOOT;

    return NULL;
}

//!\brief   远程配置文件分解成配置文件和能力文件
//!\param   Aims_pach      远程配置文件
//!\param   CfgFile_pach   参数配置文件
//!\param   AblityFlie_pach   设备能力文件
//!\return  0表示成功，其它为错误码
int Mytest_Analysis_CfgFileAndAbilityFile_Callback(const char* Aims_pach, const char* ReadOnlyCfgFile_pach, const char* CfgFile_pach, const char* AblityFile_pach)
{
    FILE* iCfgFile = NULL;
    FILE* iAblityFile = NULL;
    FILE* iAimsFile = NULL;
    FILE* iReadOnlyCfgFile = NULL;
    char buf[128] = {0};
    char bufTell = 0;
    char bRoCfgFileFlag = FALSE;
    char bCfgFileFlag = FALSE;
    char bAblityFileFlag = FALSE;

    char bIsFoundDivideFlag = FALSE;
    char bIsNeedInsertStartFlag = TRUE;
    if (-1 == access(Aims_pach,0))
    {
        printf("%s file does not exist [%s_%d]\n",Aims_pach, __FUNCTION__, __LINE__);
        return -1;
    }

    if (((iCfgFile = fopen(CfgFile_pach, "wb")) == NULL)
        || ((iAblityFile = fopen(AblityFile_pach, "wb")) == NULL)
        || ((iReadOnlyCfgFile = fopen(ReadOnlyCfgFile_pach, "wb")) == NULL)
         || ((iAimsFile = fopen(Aims_pach, "r")) == NULL))
    {
        fclose(iReadOnlyCfgFile);
        fclose(iCfgFile);
        fclose(iAblityFile);
        fclose(iAimsFile);
        //打开失败
        printf("File open failed [%s_%d]\n", __FUNCTION__, __LINE__);
        return -1;
    }
    if (fread(&bufTell, 1, 1, iAimsFile) <= 0)
    {
        printf("The content of the file is empty [%s_%d]\n", __FUNCTION__, __LINE__);
        fclose(iReadOnlyCfgFile);
        fclose(iCfgFile);
        fclose(iAblityFile);
        fclose(iAimsFile);

        return -1;
    }

    fseek(iAimsFile, 0, SEEK_SET);
    fseek(iReadOnlyCfgFile, 0, SEEK_SET);
    fseek(iCfgFile, 0, SEEK_SET);
    fseek(iAblityFile, 0, SEEK_SET);
    char buff[1024]= "";
    FILE* fd_Write = iReadOnlyCfgFile;//首先句柄指向只读配置文件

    //简化版本格式(不需要定制文件中带首尾标志)
    //拆分时，程序手动添加首尾标志
    bIsFoundDivideFlag = FALSE;
    bIsNeedInsertStartFlag = TRUE;
    bRoCfgFileFlag = TRUE;

    //产测工具定制后，定制文件中会有两个首位标志，这里将首位标志屏蔽
    while (fgets(buff, sizeof(buff), iAimsFile) != NULL)
    {
        if (bIsNeedInsertStartFlag)
        {
            //fwrite(CFG_FILE_HEAD_FLAG, strlen(CFG_FILE_HEAD_FLAG), 1, fd_Write);
            bIsNeedInsertStartFlag = FALSE;
        }

        if (bRoCfgFileFlag)
        {
            if (strncmp(buff ,CFG_FILE_FLAG, strlen(CFG_FILE_FLAG)) == 0)
            {
                bCfgFileFlag = TRUE;
                bRoCfgFileFlag = FALSE;
                bIsFoundDivideFlag = TRUE;
                //fwrite(CFG_FILE_END_FLAG, strlen(CFG_FILE_END_FLAG), 1, fd_Write);
                bIsNeedInsertStartFlag = TRUE;

                fd_Write = iCfgFile; //更换写文件句柄
                continue;
            }
            else
            {
                fwrite(buff, strlen(buff), 1, fd_Write);
            }
        }

        if (bCfgFileFlag)
        {
            if (strncmp(buff ,CFG_FILE_FLAG, strlen(CFG_FILE_FLAG)) == 0)
            {
                bAblityFileFlag = TRUE;
                bCfgFileFlag = FALSE;
                bIsFoundDivideFlag = TRUE;
                //fwrite(CFG_FILE_END_FLAG, strlen(CFG_FILE_END_FLAG), 1, fd_Write);
                bIsNeedInsertStartFlag = TRUE;

                fd_Write = iAblityFile; //更换写文件句柄
                continue;
            }
            else
            {
                fwrite(buff, strlen(buff), 1, fd_Write);
            }
        }

        if (bAblityFileFlag)
        {
            fwrite(buff, strlen(buff), 1, fd_Write);
        }

        memset(buff, 0, sizeof(buff));
    }
    //fwrite(CFG_FILE_END_FLAG, strlen(CFG_FILE_END_FLAG), 1, fd_Write);

    //要判断iAlims 文件格式 是否正确
    if (!bIsFoundDivideFlag)
    {
        //文件格式个数有误!
        //删除已分解的文件
        memset(buff, 0, sizeof(buff));
        snprintf(buff, sizeof(buff), "rm  %s", ReadOnlyCfgFile_pach);
        mysystem(buff);
        memset(buff, 0, sizeof(buff));
        snprintf(buff, sizeof(buff), "rm  %s", CfgFile_pach);
        mysystem(buff);
        memset(buff, 0, sizeof(buff));
        snprintf(buff, sizeof(buff), "rm  %s", AblityFile_pach);
        mysystem(buff);

        fclose(iReadOnlyCfgFile);
        fclose(iCfgFile);
        fclose(iAblityFile);
        fclose(iAimsFile);
        return -3;
    }

    fclose(iReadOnlyCfgFile);
    fclose(iCfgFile);
    fclose(iAblityFile);
    fclose(iAimsFile);

    memset(buf, 0, sizeof(buf));
    snprintf(buf, sizeof(buf), "cp %s %s", CUSTOM_READONLY_CONFIG_FILE, CUSTOM_BACKUP_READONLY_CONFIG_FILE);
    mysystem(buf);
    memset(buf, 0, sizeof(buf));
    snprintf(buf, sizeof(buf), "cp %s %s", CUSTOM_CONFIGMODE_FILE, CUSTOM_BACKUP_CONFIGMODE_FILE);
    mysystem(buf);
    memset(buf, 0, sizeof(buf));
    snprintf(buf, sizeof(buf), "cp %s %s", CUSTOM_ABILITY_FILE, CUSTOM_BACKUP_ABILITY_FILE);
    mysystem(buf);

    //从 customcfg.ini 定制获取云台速度上限
    int nSpeedHmax = 0;
    CConfig *pCustomCfg = CConfig::NewInstance();
    int s32Ret = pCustomCfg->LoadFile(CUSTOM_CONFIG_FILE);
    if (0 == s32Ret)
    {
        nSpeedHmax = pCustomCfg->GetValue("PTZ_Set_0", "MaxMotorSpeedH", MOTO_SPEED_H_MAX);
    }
    else
    {
        s32Ret = pCustomCfg->LoadFile(CUSTOM_BACKUP_CONFIGMODE_FILE);
        if (0 == s32Ret)
        {
            nSpeedHmax = pCustomCfg->GetValue("PTZ_Set_0", "MaxMotorSpeedH", MOTO_SPEED_H_MAX);
        }
    }
    CConfig::ReleaseInstance(pCustomCfg);
    pCustomCfg = NULL;
    //设置云台速度上限
    MOTO_SetSpeedMax(nSpeedHmax, MOTO_SPEED_V_MAX);
    return 0;
}

//unsigned long dwOperation
//  1-重启,2-关机,3-待机，4-回复系统默认
//  10-导入参数
//  11-视频预览通道别名变更
//  12-报警输入通道别名变更
//  13-报警输出通道别名变更
//  14-设备别名变更
//  15-用户权限变更
//  16- 删除用户
//  17-  报警清除
//  18-  变更日期时间格式
//  19- 变更预览OSD坐标
//  eRemoteSetStorageMode 变更高级存储模式,参数表示新的模式
int MainRemoteControl_Callback(unsigned long dwUserID, eRemoteControlNotify eOperation, unsigned long dwParam1)
{
    extern int NetChFlag;
    int Ntime = 0;

    if (eRemoteReboot == eOperation || eRemoteHalt == eOperation)
    {
        if (0 == MPU_CIU_CheckRemoteUser(dwUserID, REMOTE_HALT))
        {
            // 没有权限
            MAIN_LOG_I("dwUserID %lu, no right to REMOTE_HALT", dwUserID);
            return -1;
        }

        //SendNotifyMessage(g_tGuiWnd.hDeskWnd, MSG_SHUTDOWN, dwUserID, (eRemoteHalt == eOperation) ? eHalt : eReboot);
        MAIN_LOG_I("eOperation == eRemoteReboot || eOperation ==  eRemoteHalt\n");
        //MPU_Uninit();

        //远程重启前判断是否有未结束的网络配置
        for(Ntime=0;Ntime<10;Ntime++) //检查10秒钟，超时后重启
        {
            MAIN_LOG_I("wait reboot NetChFlag = %d\n",NetChFlag);
            if(NetChFlag == 0)
            {
                MPU_PAR_SaveConfigReal();
                PAR_ForbiddenSaveFile();
                mysystem("sync");

                m_stuRebootParam.nSleepSeconds = 4;
                sem_post(&m_semStartRebootOpsThread);
                break;
            }
            sleep(1);
        }


        if(Ntime >= 10)
        {
            MPU_PAR_SaveConfigReal();
            PAR_ForbiddenSaveFile();
            m_stuRebootParam.nSleepSeconds = 1;
            sem_post(&m_semStartRebootOpsThread);
        }

        return 0;
    }
#if defined (__IR_CUT_CONTROL_IMPROVE__)
    else if (eRemoteRestoreFactory == eOperation)
    {
        if (0 == MPU_CIU_CheckRemoteUser(dwUserID, REMOTE_HALT))
        {
            // 没有权限
            MAIN_LOG_I("dwUserID %lu, no right to REMOTE_SETTINGS", dwUserID);
            return -1;
        }
        else
        {
            #if 1
            PAR_ForbiddenSaveFile();
            PAR_NET_Resume(__FUNCTION__);
            PAR_UnForbiddenSaveFile();
            #else
            HY_DVR_RemoteReboot(dwUserID, 26)
            #endif
        }

        //SendNotifyMessage(g_tGuiWnd.hDeskWnd, MSG_SHUTDOWN, (WPARAM) LOCAL_USER_ID, (LPARAM) eReboot);
        MAIN_LOG_I("eRemoteRestoreFactory == eOperation\n");
    #if 1
        m_stuRebootParam.nSleepSeconds = 1;
        sem_post(&m_semStartRebootOpsThread);
    #else
        mysystem("reboot");
    #endif

        return 0;
    }
#endif
    else if (eRemoteSetDefault == eOperation)
    {
        if (0 == MPU_CIU_CheckRemoteUser(dwUserID, REMOTE_SETTINGS))
        {
            // 没有权限
            MAIN_LOG_I("dwUserID %lu, no right to REMOTE_SETTINGS", dwUserID);
            return -1;
        }
        else
        {
            FILE *fd=NULL;
            char buff[1024];

            int result = MPU_PAR_ResumeDefaultConfig("/mnt/mtd/config/Edvr.default");

            //获取新的配置文件读取实例
            CConfig *pCfg = CConfig::NewInstance();
            if(pCfg)
            {
                int nRet = pCfg->LoadFileRO(IDVR_RECOVER_CONTROL_CONFIG_FILE_PATH);
                if (0 == nRet)
                {
                    int revertFlag = 0;
                    revertFlag = pCfg->GetValue(INet_CfgSection, INet_IP_key, 0);
                    if(0 == revertFlag)
                    {
                        mysystem("rm /mnt/mtd/config/.Ip*");
                    }

                    revertFlag = pCfg->GetValue(INet_CfgSection, INet_Gateway_key, 0);
                    if(0 == revertFlag)
                    {
                        mysystem("rm /mnt/mtd/config/.GATEWAY*");
                    }

                    revertFlag = pCfg->GetValue(INet_CfgSection, INet_SubnetMask_key, 0);
                    if(0 == revertFlag)
                    {
                        mysystem("rm /mnt/mtd/config/.MASK*");
                    }

                    revertFlag = pCfg->GetValue(INet_CfgSection, INet_DHCP_key, 0);
                    if(0 == revertFlag)
                    {
                        mysystem("rm /mnt/mtd/config/.DHCP*");
                    }

                    revertFlag = pCfg->GetValue(INet_CfgSection, INet_IPAdjust_key, 0);
                    if(0 == revertFlag)
                    {
                        mysystem("rm /mnt/mtd/config/.IPADJUST*");
                    }
                #if defined (__IS_SUPPORT_WLAN__)
                    //WLAN简单恢复时，根据Edvr.flag来确认是否需要恢复默认配置
                    /******************begin*********************/
                    revertFlag = pCfg->GetValue(INet_CfgSection, INet_WLANIP_key, 0);
                    if(0 == revertFlag)
                    {
                        mysystem("rm /usr/local/etc/.WlanIp* ");
                    }
                    revertFlag = pCfg->GetValue(INet_CfgSection, INet_WLANSubnetMask_key, 0);
                    if(0 == revertFlag)
                    {
                        mysystem("rm /usr/local/etc/.WlanMASK* ");
                    }
                    revertFlag = pCfg->GetValue(INet_CfgSection, INet_WLANGateway_key, 0);
                    if(0 == revertFlag)
                    {
                        mysystem("rm /usr/local/etc/.WlanGateWay* ");
                    }
                    revertFlag = pCfg->GetValue(INet_CfgSection, INet_WLANDHCP_key, 0);
                    if(0 == revertFlag)
                    {
                        mysystem("rm /usr/local/etc/.WlanDHCP* ");
                    }
                    revertFlag = pCfg->GetValue(INet_CfgSection, INet_WLANWPAKey_key, 0);
                    if(0 == revertFlag)
                    {
                        mysystem("rm /usr/local/etc/.wificfg.ini ");
                    }
                    revertFlag = pCfg->GetValue(INet_CfgSection, INet_WLANSSID_key, 0);
                    if(0 == revertFlag)
                    {
                        mysystem("rm /usr/local/etc/.wificfg.ini ");
                    }
                    revertFlag = pCfg->GetValue(INet_CfgSection, INet_WLANWifiMode, 0);
                    if(0 == revertFlag)
                    {
                        mysystem("rm /usr/local/etc/.WlanWifiMode* ");
                    }
                    /********************end**********************/
                #endif
                    revertFlag = pCfg->GetValue("ChannelAlias_0", "Unicode", 0);
                    if(0 == revertFlag)
                    {
                        mysystem("rm /mnt/mtd/config/.OSD");
                    }

                }
                else
                {
                    MAIN_LOG_I("Load file %s fail!\n", IDVR_RECOVER_CONTROL_CONFIG_FILE_PATH);
                }
                CConfig::ReleaseInstance(pCfg);
            }

            fd = fopen(HTTP_SERVER_CFG_NAME, "w");//端口号恢复默认
            if (fd == NULL)
            {
                MAIN_LOG_I("open file failed [%s]\n", HTTP_SERVER_CFG_NAME);
                fclose(fd);
                return -1;
            }

            memset(buff, 0, sizeof (buff));
            snprintf(buff, sizeof(buff), "%d", 80);
            fprintf(fd, "%s", buff);
            fclose(fd);
            RecoverUlkEnable();

            #ifdef ADD_ONVIF
            FILE *fp = NULL;
            char *pfind;
            char cache[256];
            long insertPoint,copyPoint;

            fp = fopen(BOA_CONF_PATH, "r+");
            if(NULL == fp)
            {
               MAIN_LOG_I("fopen %s fail !\n", BOA_CONF_PATH);
               return -1;
            }

            while (!feof(fp))
            {
                memset(buff, 0, sizeof (buff));
                insertPoint = ftell(fp);
                fgets(buff, sizeof (buff), fp);
                copyPoint = ftell(fp);

                if ('#' != buff[0])
                {
                    if(NULL != (pfind = strstr(buff, PORT_KEY)))
                    {
                        memset(cache, 0, sizeof(cache));
                        snprintf(cache, sizeof(cache), "%s%d\n", PORT_KEY, 80);
                        fclose(fp);
                        if(0 != insertDateToFile(BOA_CONF_PATH, insertPoint, copyPoint, cache))
                        {
                            MAIN_LOG_I("insertDateToFile fail, return <-1>\n");
                        }
                        break;
                    }
                }
            }

            #endif

            if (result)
            {
                MAIN_LOG_I("MPU_PAR_ResumeDefaultConfig failed(%d)\n", result);
                return -1;
            }
            else
            {
                char acUerName[64];

                char acIPAddress[20];

                char acLoginTime[30];

                HANDLE hUser = NULL;

                DWORD dwIpaddr = 0;

                MPU_CIU_GetRemoteByID(dwUserID, &hUser, acUerName, sizeof(acUerName), acIPAddress, sizeof(acIPAddress), acLoginTime, sizeof(acLoginTime));
                IPFromString(acIPAddress, &dwIpaddr);
                MPU_OPL_AddLog(EDVR_EVENT_SETTING, EDVR_EVENT_SETTING_DETAIL_DEFAULT_SETTING, 0, 0, dwIpaddr, acUerName);
            }

        }
        PAR_ForbiddenSaveFile();
        //SendNotifyMessage(g_tGuiWnd.hDeskWnd, MSG_SHUTDOWN, (WPARAM) LOCAL_USER_ID, (LPARAM) eReboot);
        MAIN_LOG_I("eRemoteSetDefault == eOperation\n");
        mysystem("sync");
        sleep(1);
        mysystem("sync");
        sleep(1);

#if 1
        m_stuRebootParam.nSleepSeconds = 1;
        sem_post(&m_semStartRebootOpsThread);
#else
        mysystem("reboot");
#endif
        return 0;
    }
    else if (eOperation == eRemoteSendTestEmail)
    {
        if (0 == MPU_CIU_CheckRemoteUser(dwUserID, REMOTE_SETTINGS))
        {
            // 没有权限
            MAIN_LOG_I("dwUserID %lu, no right to REMOTE_SETTINGS", dwUserID);
            return -1;
        }
        else
        {
            HANDLE hRemoteUser = NULL;

            MPU_CIU_GetRemoteByID(dwUserID, &hRemoteUser, NULL, 0, NULL, 0, NULL, 0);

            return SendTestEmail(hRemoteUser);
        }
    }
    else if (eOperation == eRemoteUpdateConfig)	//  10-导入参数
    {
        if (0 == MPU_CIU_CheckRemoteUser(dwUserID, REMOTE_SETTINGS))
        {
            // 没有权限
            MAIN_LOG_I("dwUserID %lu, no right to REMOTE_SETTINGS", dwUserID);
            return -1;
        }
        else
        {
            int result = MPU_PAR_ResumeDefaultConfig(IMPORTFILENAME);

            if (result)
            {
                MAIN_LOG_I("MPU_PAR_ResumeDefaultConfig failed(%d)\n", result);
                return -1;
            }
            else
            {
                char acUerName[64];

                char acIPAddress[20];

                char acLoginTime[30];

                HANDLE hUser = NULL;

                DWORD dwIpaddr = 0;

                MPU_CIU_GetRemoteByID(dwUserID, &hUser, acUerName, sizeof(acUerName), acIPAddress, sizeof(acIPAddress), acLoginTime, sizeof(acLoginTime));
                IPFromString(acIPAddress, &dwIpaddr);
                MPU_OPL_AddLog(EDVR_EVENT_SETTING, EDVR_EVENT_SETTING_DETAIL_IMPORT_SETTINGS, 0, 0, dwIpaddr, acUerName);
                //SendNotifyMessage(g_tGuiWnd.hDeskWnd, MSG_SHUTDOWN, (WPARAM) LOCAL_USER_ID, (LPARAM) eReboot);
                MAIN_LOG_I("eRemoteUpdateConfig == eOperation\n");
#if 1
                m_stuRebootParam.nSleepSeconds = 1;
                sem_post(&m_semStartRebootOpsThread);
#else
                mysystem("reboot");
#endif

                return 0;
            }

        }
        return 0;
    }
    //      11-视频预览通道别名变更
    else if (eRemoteSetVideoInAlias == eOperation)
    {
        return 0;
    }
//  12-报警输入通道别名变更
    else if (eRemoteSetAlarmInAlias == eOperation)
    {
        return 0;
    }
//  13-报警输出通道别名变更
    else if (eRemoteSetAlarmOutAlias == eOperation)
    {
        return 0;
    }
//  14-设备别名变更
    else if (eRemoteSetDevAlias == eOperation)
    {
        return 0;
    }
//  15-用户权限变更
    else if (eRemoteSetUserRight == eOperation)
    {
        return 0;
    }
//  16- 删除用户
    else if (eRemoteRemoveUser == eOperation)
    {
        return 0;
    }
//  17-  报警清除
    else if (eRemoteClearAlarm == eOperation)
    {
        MPU_ClearAlarm();

        // 对于非特殊用户，需要添加到日志里面去
        if (dwUserID != (DWORD) (-1))
        {
            char szUsername[20];
            char szIP[20];
            int result;
            unsigned long dwIP;

            result = MPU_CIU_GetRemoteByID(dwUserID, NULL, szUsername, sizeof(szUsername), szIP, sizeof(szIP), NULL, 0);
            if (!result)
            {
                IPFromString(szIP, &dwIP);
                MPU_OPL_AddLog(EDVR_EVENT_OPT, EDVR_EVENT_OPT_DETAIL_CLEAR_ALARM, 0, 0, dwIP, szUsername);
            }
        }
        return 0;
    }
//  18-  变更日期时间格式
    else if (eRemoteSetTimeFormat == eOperation)
    {
        return 0;
    }
//  19- 变更预览OSD坐标
    else if (eRemoteMoveVideoInAlias == eOperation)
    {
        return 0;
    }
    // 变更高级存储模式,参数表示新的模式
    else if (eRemoteSetStorageMode == eOperation)
    {
        if (0 == dwParam1)
        {
            //SendNotifyMessage(g_tGuiWnd.hDeskWnd, MSG_SHUTDOWN_TIMER, (WPARAM) LOCAL_USER_ID, (LPARAM) eReboot);
            MAIN_LOG_I("eOperation == eRemoteSetStorageMode\n");
            return 0;
        }
    }
    // 变更编码模式
    else if (eRemoteSetHCResMode == eOperation)
    {
        //SendNotifyMessage(g_tGuiWnd.hDeskWnd, MSG_SHUTDOWN_TIMER, (WPARAM) LOCAL_USER_ID, (LPARAM) eReboot);
        MAIN_LOG_I("eOperation == eRemoteSetHCResMode\n");
        return 0;
    }
    //27-发送测试FTP
    else if (eRemoteSendTestFtp == eOperation)
    {
        if (0 == MPU_CIU_CheckRemoteUser(dwUserID, REMOTE_SETTINGS))
        {
            // 没有权限
            MAIN_LOG_I("dwUserID %lu, no right to REMOTE_SETTINGS", dwUserID);
            return -1;
        }
        else
        {
            HANDLE hRemoteUser = NULL;
            MPU_CIU_GetRemoteByID(dwUserID, &hRemoteUser, NULL, 0, NULL, 0, NULL, 0);
            return SendTestFtp(hRemoteUser);
        }
    }
    return -1;
}

/*@breif StartTimedCaptureCallBack 定时抓图时间到达触发的回调函数
 *@param [IN] capture_type 录像类型 1:FTP 2:SD
 *@return 成功返回0 错误返回-1
 *@note 当前操作不能被阻塞(ftp操作不能阻塞)
 *@     如果该回调被塞住，则会出现抓图一直失败的问题
 */
static int StartTimedCaptureCallBack(int capture_type)
{
    if ((1!=capture_type) && (2!=capture_type))
    {
        MAIN_LOG_I("[%s-%d] capture_type %d out of [1,2]\n", __func__, __LINE__, capture_type);
        return -1;
    }

    struct timeval now_tv = {0};
    char szFileName[128] = {0};
    char tmp[64] = {0};

    gettimeofday(&now_tv, NULL);//获取时间

    /* 把与触发时间最接近的1张图片cp到/tmp/picture/pic_for_xxx */
    MPU_STR_TrigEventCapturePic(capture_type, now_tv.tv_sec, now_tv.tv_usec);      //触发抓图

    /* tmp/picture/pic_for_xxx里的文件字符串放到tmp里 */
    MPU_STR_SnapShotM_GetLastPic(0, capture_type, tmp);  //获取最后一次抓图的文件名

    /* 当前的定时抓图是SD卡抓图 */
    if (2 == capture_type)
    {
        #ifdef __SUPPORT_SD_CARD__
        /* 1. SD卡里面已经根据当前传入的类型进行添加前缀了;
         * 2. 传入的图片名字组成pic_name: 时间戳_标号，如:1532477851_01需要改为1532477851
         */
        MPU_AVL_UploadPictureToSdCard(0, tmp, capture_type);

        /* 如果出错或者无SD卡，也需要删除原始文件，避免出现内存里面文件过多 */
        snprintf(szFileName, sizeof(szFileName), "%s/%s", CAPTURE_MEM_DIR_FOR_SD, tmp);
        SAFE_REMOVE(szFileName);
        #endif
    }
    else if (1 == capture_type)/* 当前的定时抓图是FTP抓图 */
    {
#if defined (__USE_FTP__)
        int ftp_trans_type = 0;

        if (GetFtpEnable())
        {
            /* 传输类型(0:不传 1:图片 2:录像 3:ALL ) */
            MPU_NET_GetFtpTransferType(&ftp_trans_type);
            if ((1==ftp_trans_type) || (3==ftp_trans_type))
            {
                /* FTP传输的时候需要添加前缀 */
                snprintf(szFileName, sizeof(szFileName), "T_%s", tmp);

                /* 开始进行FTP文件传输(不能阻塞) */
                if(0 != SetFtpTransferName((unsigned char *)tmp, (unsigned char *)szFileName))
                {
                    MAIN_LOG_I("[%s-%s-%d] [ERR] SetFtpTransferName %s to %s fail!\n",
                           __FILE__, __func__, __LINE__, tmp, szFileName);
                    return -1;
                }
            }
            else
            {
                /* 如果出错也需要删除原始文件，避免出现内存里面文件过多 */
                snprintf(szFileName, sizeof(szFileName), "%s/%s", CAPTURE_MEM_DIR_FOR_FTP, tmp);
                SAFE_REMOVE(szFileName);
            }
        }
        else
        {
            /* 如果出错也需要删除原始文件，避免出现内存里面文件过多 */
            snprintf(szFileName, sizeof(szFileName), "%s/%s", CAPTURE_MEM_DIR_FOR_FTP, tmp);
            SAFE_REMOVE(szFileName);
        }
#endif
    }

    return 0;
}

/* 获取当前UTC时间和系统时间回调 */
static int GetTimeOffsetCallBack(int *time_zone,
                                            int *dst_status,
                                            int *dst_bias)
{
    if (!time_zone || !dst_status || !dst_bias)
    {
        MAIN_LOG_I("[%s-%d] [error] NULL POINTER EXCEPTION!\n", __FUNCTION__, __LINE__);
        return -1;
    }

    *time_zone   = MPU_SYS_GetTimeZone();
    *dst_status = MPU_SYS_GetDstIsWork();
    *dst_bias    = MPU_SYS_GetDstBias();

    return 0;
}

/* 检测当前互联网状态回调函数(不允许跨模块调用函数) */
static int CheckInterNetStatusCallBack(int *conn_status)
{
    if (NULL == conn_status)
    {
        MAIN_LOG_I("[%s-%s-%d] [ERR] NULL POINTER EXCEPTION!\n", __FILE__, __FUNCTION__, __LINE__);
        return -1;
    }

    MPU_Net_CheckIfNetAccessToInternet(conn_status);
    MAIN_LOG_I("Get internet is %s\n", *conn_status ? "Connect" : "Break");

    return 0;
}

#ifdef __AGINGTEST__
//老化测试功能
static void StrRemoveBoundSpace(char *str)
{
    int i;
    char *ps = str;
    int len = strlen(str);

    if (ps == NULL) {
        return;
    }
    i = 0;
    while ((*ps == ' ') && (*ps != '\0')) {
        ps++;
        i++;
    }
    if (len - i > 0){
        memmove(str, ps, len-i+1);//remove the left space
    }

    while (((len - i) > 0) && (*(str+len-i-1) == ' ')) {//remove the right space
        i++;
    }

    *(str+len-i) = '\0';
}

static StatusRet FindString(char *buf, int bufSize, const char* sect, const char *key, const char *file)
{
    char *text, *p, *ps = NULL, *ptemp;
    char first_flg = 0;//indicate that have been read from key line
    int lentext;//the read line length,wipe off 0x0a
    FILE *fd;
    enum STATE state = STATE_IDLE;
    StatusRet ret = ERR_NON;

    if ((fd = fopen(file,"rt")) == NULL) {
        MAIN_LOG_I("open file failed, path:%s\n",file);
        return ERR_FILE;
    }
    if ((text = (char*)malloc(INI_MAX_LINE + 1)) == NULL) {
        fclose(fd);
        return ERR_MEM;
    }
    ptemp = (char*)malloc(INI_MAX_LINE);

    fseek(fd, 0, SEEK_SET);
    while (fgets(text, INI_MAX_LINE, fd) != NULL) {
        lentext = strlen(text);//wipe off the LF character for IniGetString
        if (*(text+lentext-1) == '\n') {
            *(text+lentext-1) = '\0';
            if (*(text+lentext-2) == '\r') {
                *(text+lentext-2) = '\0';
            }
        }

        p = text;
        while (*p != '\0')
        {
            switch(state)
            {
            case STATE_IDLE:
                if (*p == '[') {
                    state = STATE_SECT;
                    ps = p + 1;
                } else {
                    *p = '\0';
                    continue;//skip the current line
                }
                break;
            case STATE_SECT:
                if (*p == ']') {
                    *p = '\0';
                    if (strcmp(ps, sect)) {//not identical
                        state = STATE_IDLE;
                    } else {
                        state = STATE_KEY;
                    }
                    continue;//skip the current line
                } else {
                    if (*(p+1) == '\0') {
                        ret = ERR_INI;
                        goto out;
                    }
                }
                break;
            case STATE_KEY:
                if ((*p == ';') || (*(p+1) == '\0')) {//exit before '='
                    *p = '\0';
                    if (first_flg) {//handle the found key which has not '='&value
                        first_flg = 0;
                        StrRemoveBoundSpace(ps);
                        if (!strcmp(ps, key)) {//identical
                            *buf = '\0';
                            ret =  ERR_INI;
                            goto out;
                        } else {
                            ret = ERR_NON_EXIST;
                        }
                    } else {//if hasn't found key,regard as none exist
                        ret = ERR_NON_EXIST;
                    }
                    *p = '\0';
                    continue;//skip the current line
                }
                else
                    if ((*p == '[')
                        && (first_flg == 0)) {//the section doesn't have key
                        *buf = '\0';
                        ret =  ERR_INI;
                        goto out;
                    }
                    else
                        if (*p == '=') {
                            first_flg = 0;
                            strncpy(ptemp,p+1,INI_MAX_LINE);
                            *p = '\0';
                            StrRemoveBoundSpace(ps);
                            if (strcmp(ps, key)) {//not identical
                                continue;//skip the current line
                            } else {//get the value
                                p = ps = ptemp;
                                while (*p != '\0' && (*p != ';')) {
                                    p++;
                                }
                                *p = '\0';
                                StrRemoveBoundSpace(ps);
                                strncpy(buf, ps, bufSize - 1);
                                ret = ERR_NON;
                                goto out;
                            }
                        }
                        else
                            if(!first_flg) {//first key character in the key line
                                first_flg = 1;
                                ps = p;
                            }
                break;
            default:
                break;
            }
            p++;
        }
    }

out:
    free(ptemp);
    ptemp = NULL;

    free(text);
    text = NULL;

    fclose(fd);
    fd = 0;

    return ret;
}

static bool IniGetInt(const char* section, const char *key, int *value, const char *file)
{
    bool ret = false;
    char bufsize = 32;
    char *buf = (char *)malloc(bufsize);
    memset(buf, 0, bufsize);

    if (FindString(buf, bufsize, section, key, file) == ERR_NON) {
        *value = atoi(buf);
        ret = true;
    }

    free(buf);

    return ret;
}

//初始化Params参数
static int get_aging_test_params( AGI_InitParams_st *init_params)
{
    memset(init_params, 0, sizeof(AGI_InitParams_st));

    IniGetInt("AGING", "aging_time",          &(init_params->aging_time),         SD_AGING_TEST_LIST_FILE_NAME);
    IniGetInt("AGING", "ptz_enable",          &(init_params->ptz_enable),         SD_AGING_TEST_LIST_FILE_NAME);
    IniGetInt("AGING", "led_white_enable",    &(init_params->led_white_enable),   SD_AGING_TEST_LIST_FILE_NAME);
    IniGetInt("AGING", "led_red_enable",      &(init_params->led_red_enable),     SD_AGING_TEST_LIST_FILE_NAME);
    IniGetInt("AGING", "ir_cut_enable",       &(init_params->ir_cut_enable),      SD_AGING_TEST_LIST_FILE_NAME);
    IniGetInt("AGING", "audio_out_enable",    &(init_params->audio_out_enable),   SD_AGING_TEST_LIST_FILE_NAME);
    IniGetInt("AGING", "led_red_time",        &(init_params->led_red_time),       SD_AGING_TEST_LIST_FILE_NAME);
    IniGetInt("AGING", "led_white_time",      &(init_params->led_white_time),     SD_AGING_TEST_LIST_FILE_NAME);
    IniGetInt("AGING", "ir_cut_time",         &(init_params->ir_cut_time),        SD_AGING_TEST_LIST_FILE_NAME);
    IniGetInt("AGING", "audio_out_time",      &(init_params->audio_out_time),     SD_AGING_TEST_LIST_FILE_NAME);
    IniGetInt("AGING", "ptz_time",            &(init_params->ptz_time),           SD_AGING_TEST_LIST_FILE_NAME);
    IniGetInt("AGING", "ptz_speed",           &(init_params->ptz_speed),          SD_AGING_TEST_LIST_FILE_NAME);
    IniGetInt("AGING", "alarm_out_enable",    &(init_params->alarm_out_enable),   SD_AGING_TEST_LIST_FILE_NAME);
    IniGetInt("AGING", "alarm_out_time",      &(init_params->alarm_out_time),     SD_AGING_TEST_LIST_FILE_NAME);

    //参数合法性判断
    init_params->ir_cut_enable    = (init_params->ir_cut_enable != 0 ? 1:0);
    init_params->ptz_enable       = (init_params->ptz_enable != 0 ? 1:0);
    init_params->audio_out_enable = (init_params->audio_out_enable != 0 ? 1:0);
    init_params->led_red_enable   = (init_params->led_red_enable != 0 ? 1:0);
    init_params->led_white_enable   = (init_params->led_white_enable != 0 ? 1:0);
    init_params->alarm_out_enable   = (init_params->alarm_out_enable != 0 ? 1:0);

    if (init_params->aging_time > AGING_TEST_MAX_TIME)
    {
        init_params->aging_time = AGING_TEST_DEFAULT_TIME;//24小时=1440分钟
        MAIN_LOG_I("aging_time: The default value is used\n");
    }
    else if (init_params->aging_time < -1)
    {
        init_params->aging_time = 0;//0小时
        MAIN_LOG_I("aging_time: 0\n");
    }

    if (init_params->ir_cut_time < 3 || init_params->ir_cut_time > 60){
        init_params->ir_cut_time = 30;
        MAIN_LOG_I("ir_cut_time: The default value is used\n");
    }

    if (init_params->ptz_time < 1 || init_params->ptz_time > 60){
        init_params->ptz_time = 30;
        MAIN_LOG_I("ptz_time: The default value is used\n");
    }

    if (init_params->ptz_speed < 1 || init_params->ptz_speed > 10){
        init_params->ptz_speed = 3;
        MAIN_LOG_I("ptz_speed: The default value is used\n");
    }

    if (init_params->audio_out_time < 0 || init_params->audio_out_time > 300){
        init_params->audio_out_time = 120;
        MAIN_LOG_I("audio_out_time: The default value is used\n");
    }

    if (init_params->led_red_time < 10 || init_params->led_red_time > 60){
        init_params->led_red_time = 40;
        MAIN_LOG_I("led_red_time: The default value is used\n");
    }

    if (init_params->led_white_time < 10 || init_params->led_white_time > 60){
        init_params->led_white_time = 20;
        MAIN_LOG_I("led_white_time: The default value is used\n");
    }

    if (init_params->alarm_out_time < 10 || init_params->alarm_out_time > 60){
        init_params->alarm_out_time = 20;
        MAIN_LOG_I("alarm_out_time: The default value is used\n");
    }

    MAIN_LOG_I("%s aging_time:%d\n"
            "ir_cut_enable:%d, ir_cut_time:%d\n"
            "ptz_enable:%d, ptz_time:%d, ptz_speed:%d\n"
            "audio_out_enable:%d, audio_out_time:%d\n"
            "led_red_enable:%d, led_red_time:%d\n"
            "led_white_enable:%d, led_white_time:%d\n"
            "alarm_out_enable:%d, alarm_out_time:%d\n",
            AGING_TEST_MODULE_NAME,
            init_params->aging_time,
            init_params->ir_cut_enable,init_params->ir_cut_time,
            init_params->ptz_enable,init_params->ptz_time,init_params->ptz_speed,
            init_params->audio_out_enable,init_params->audio_out_time,
            init_params->led_red_enable,init_params->led_red_time,
            init_params->led_white_enable,init_params->led_white_time,
            init_params->alarm_out_enable,init_params->alarm_out_time);

    return 0;
}

//ircut老化测试
static void aging_test_ircut()
{
    static int ir_flag = 0;
    void *hChannelHandle = ZWSys_EncChannelOpen(0);

    if (ir_flag)
    {
        ZWSys_SetForceNightStatus( hChannelHandle, 0);
        MAIN_LOG_I("%s ircut: disenable ForceNight\n",AGING_TEST_MODULE_NAME);
        ir_flag = 0;
    }
    else
    {
        ZWSys_SetForceNightStatus( hChannelHandle, 1);
        MAIN_LOG_I("%s ircut: enable ForceNight\n",AGING_TEST_MODULE_NAME);
        ir_flag = 1;
    }
}

#if defined (__IS_SUPPORT_PTZ_BUILTIN__) && defined (__IS_SUPPORT_PTZ_BUILTIN_HUMAN_TRACK__)
//云台老化测试
static void aging_test_ptz()
{
    static int cmd[4]  = {0};
    int ch = 0, nHMax = 0, nVMax = 0;
    int nX = 0, nY = 0;

    CPtzManager *pPtz = CPtzManager::PTZ_GetInstance ();
    for(int i = 0; i < 4; i++)
    {
        if (i == 1) {
            continue;
        }
        pPtz->PTZ_GetMaxStep(i, &nHMax, &nVMax);

        if (PTZ_TOPOS_VMIN_HMIN == cmd[i])
        {
            nX = 0;
            nY = 0;
            cmd[i] = PTZ_TOPOS_VMAX_HMAX;
        }
        else if (PTZ_TOPOS_VMAX_HMAX == cmd[i])
        {
            nX = nHMax;
            nY = nVMax;
            cmd[i] = PTZ_TOPOS_VMIN_HMIN;
        }
        MOTO_ToPos_AgingTest(nX, nY, i);
    }

    /* 记录最新的位置（避免跑走了） */
    char szSectionName[64] = {};
    snprintf(szSectionName, sizeof(szSectionName), "PTZ_Set_%d", ch);

    CConfig *pConfig = CConfig::GetInstance();
    pConfig->SetValue(szSectionName, "HorizontalAngle", nX);
    pConfig->SetValue(szSectionName, "VerticalAngle", nY);
    pConfig->SaveFile();
}
#endif

void* start_aging_test( void *param)
{
    PRT_TID();

    int aging_test_time = 0;
    int never_stop_test = 0;
    long time_interval = 0;
    int nAlarmOutStatue = 1;
    int lightOPenOrCloseFlag = 0;//红白灯开关标志
    int recordEnable = 0;
    char buf[128] = {0};
    AssistantCtrlInfo stAssitInfo = { PDEV_WHITE_LED, 0};
    AGI_InitParams_st g_init_params = {0};
    tagRecordScheduleV2 sched = {0};

    get_aging_test_params(&g_init_params);

    //老化总时长判断
    if (g_init_params.aging_time == 0)
    {
        return NULL;    //不进行老化测试
    }
    else if (g_init_params.aging_time == -1)
    {
       never_stop_test = 1;   //持续老化测试不停止
    }
    else
    {
       aging_test_time = 60 * g_init_params.aging_time;//秒
    }

    //关闭普通事件
    MPU_AVL_AreaDefense_Motion_Enable(0, 0);
    MPU_AVL_AreaDefense_HumanShape_Enable(0, 0);

#if defined (__IS_SUPPORT_PTZ_BUILTIN__) && defined (__IS_SUPPORT_PTZ_BUILTIN_HUMAN_TRACK__)
    //设置老化测试云台速度(默认最大速度)
    MPU_PTZ_SetSpeed(g_init_params.ptz_speed, 0);
#endif

    while (1)
    {
        //ircut
        if ((1 == g_init_params.ir_cut_enable) && (0 == time_interval % g_init_params.ir_cut_time))
        {
            aging_test_ircut();
        }

        //ptz
        if((1 == g_init_params.ptz_enable) && (0 == time_interval % g_init_params.ptz_time))
        {
        #if defined (__IS_SUPPORT_PTZ_BUILTIN__) && defined (__IS_SUPPORT_PTZ_BUILTIN_HUMAN_TRACK__)
            aging_test_ptz();
        #endif
        }

         //audio out
        if ((1 == g_init_params.audio_out_enable) && (0 == time_interval % g_init_params.audio_out_time))
        {
            ZWSys_SetSpeechContent(SPEECH_AGING_TEST);
        }

        //alarmout
        if((1 == g_init_params.alarm_out_enable) && (0 == time_interval % g_init_params.alarm_out_time))
        {
            if(1 == nAlarmOutStatue)
            {
                ZWSys_AlarmOut(0, nAlarmOutStatue);
                nAlarmOutStatue = 0;
            }
            else
            {
                ZWSys_AlarmOut(0, nAlarmOutStatue);
                nAlarmOutStatue = 1;
            }
        }

        //red led && white led
        if ((1 == g_init_params.led_red_enable) && (1 == g_init_params.led_white_enable))
        {
            //白光红外轮流开，先红再白
            if(0 == time_interval%(g_init_params.led_red_time + g_init_params.led_white_time))
            {
                //关白光
                stAssitInfo.enAssisstantType = PDEV_WHITE_LED;
                stAssitInfo.nCtrlStatus = 0;
                ZWSys_AssistantControl(&stAssitInfo);

                //开红外
                stAssitInfo.enAssisstantType = PDEV_INFRARED_LED;
                stAssitInfo.nCtrlStatus = 1;
                ZWSys_AssistantControl(&stAssitInfo);
                MAIN_LOG_I("%s led: red[1], white[0]\n", AGING_TEST_MODULE_NAME);
            }
            else if(g_init_params.led_red_time == time_interval%(g_init_params.led_red_time + g_init_params.led_white_time))
            {
                //关红外
                stAssitInfo.enAssisstantType = PDEV_INFRARED_LED;
                stAssitInfo.nCtrlStatus = 0;
                ZWSys_AssistantControl(&stAssitInfo);

                //开白光
                stAssitInfo.enAssisstantType = PDEV_WHITE_LED;
                stAssitInfo.nCtrlStatus = 1;
                ZWSys_AssistantControl(&stAssitInfo);
                MAIN_LOG_I("%s led: red[0], white[1]\n", AGING_TEST_MODULE_NAME);
            }
        }
        else if ((1 == g_init_params.led_red_enable) && (1 != g_init_params.led_white_enable))
        {
            if (0 == time_interval % g_init_params.led_red_time)
            {
                //关白光
                stAssitInfo.enAssisstantType = PDEV_WHITE_LED;
                stAssitInfo.nCtrlStatus = 0;
                ZWSys_AssistantControl(&stAssitInfo);

                if (0 == lightOPenOrCloseFlag)
                {
                    //开红外
                    stAssitInfo.enAssisstantType = PDEV_INFRARED_LED;
                    stAssitInfo.nCtrlStatus = 1;
                    ZWSys_AssistantControl(&stAssitInfo);
                    lightOPenOrCloseFlag = 1;
                }
                else
                {
                    //关红外
                    stAssitInfo.enAssisstantType = PDEV_INFRARED_LED;
                    stAssitInfo.nCtrlStatus = 0;
                    ZWSys_AssistantControl(&stAssitInfo);
                    lightOPenOrCloseFlag = 0;
                }
            }
        }
        else if ((1 != g_init_params.led_red_enable) && (1 == g_init_params.led_white_enable))
        {
            if (0 == time_interval % g_init_params.led_white_time)
            {
                //关红外
                stAssitInfo.enAssisstantType = PDEV_INFRARED_LED;
                stAssitInfo.nCtrlStatus = 0;
                ZWSys_AssistantControl(&stAssitInfo);

                if (0 == lightOPenOrCloseFlag)
                {
                    //开白光
                    stAssitInfo.enAssisstantType = PDEV_WHITE_LED;
                    stAssitInfo.nCtrlStatus = 1;
                    ZWSys_AssistantControl(&stAssitInfo);
                    lightOPenOrCloseFlag = 1;
                }
                else
                {
                    //关白光
                    stAssitInfo.enAssisstantType = PDEV_WHITE_LED;
                    stAssitInfo.nCtrlStatus = 0;
                    ZWSys_AssistantControl(&stAssitInfo);
                    lightOPenOrCloseFlag = 0;
                }
            }

        }

        //判断是否退出
        sleep(1);
        time_interval++;
        if(time_interval >= aging_test_time && never_stop_test == 0)
        {
            break;
        }
    }

    //恢复默认状态
    //ircut
    void *hChannelHandle = ZWSys_EncChannelOpen(0);
    ZWSys_SetForceNightStatus( hChannelHandle, 0);

    //先关闭报警输出
    ZWSys_AlarmOut(0, 0);

    //red led && white led
    //关白光
    stAssitInfo.enAssisstantType = PDEV_WHITE_LED;
    stAssitInfo.nCtrlStatus = 0;
    ZWSys_AssistantControl(&stAssitInfo);

    //关红外
    stAssitInfo.enAssisstantType = PDEV_INFRARED_LED;
    stAssitInfo.nCtrlStatus = 0;
    ZWSys_AssistantControl(&stAssitInfo);

    snprintf(buf, sizeof(buf), "rm -rf %s", SD_AGING_TEST_LIST_FILE_NAME);
    mysystem(buf);

    return NULL;
}

int init_aging_test()
{
    if (0 != access(SD_AGING_TEST_LIST_FILE_NAME, F_OK))
    {
        return 0;
    }

    pthread_t thread_aging_test = -1;
    pthread_attr_t attr; //线程属性
    pthread_attr_init(&attr);  //初始化线程属性
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);      //设置线程属性

    //拉起老化线程
    if (pthread_create(&thread_aging_test, &attr, start_aging_test, NULL))
    {
        return -1;
    }

    pthread_attr_destroy(&attr);
    return 0;
}
#endif

//二维码扫描回调
int QrScanCallback(unsigned char *pDataBuf, unsigned int uiDataLen)
{
    if (NULL == pDataBuf
        || uiDataLen <= 0)
    {
        return -1;
    }

    unsigned int i = 0;
    char szWlanSSID[WLAN_MAX_SSID_LEN] = {0};
    char szWlanKey[WLAN_MAX_KEY_LEN] = {0};
    char szTimeZone[8] = {0};
    char szSession[32] = {0};
    char szUserId[64] = {0};
    char szServerAddress[128] = {0};
    char szRandomNumber[16] = {0};
    char bIsFindWlanSSID = 0;
    char bIsFindWlanKey = 0;
    char bIsFindTimeZone = 0;
    char bIsFindSession = 0;
    char bIsFindClientAddr = 0;
    char bIsFindUserId = 0;
    char bIsFindServerAddr = 0;
    char bIsFindRandomNumber = 0;
    char *pStrTmp = NULL;
    char *pStrTmp2 = NULL;
    char szStrTmp[256] = {0};
    int nTmp = 0;
    int nTimeZone = 0;
    unsigned int u32ClientIpAddr = 0;
    unsigned int u32ClientPort = 0;
    unsigned int uiDealLen = 0;
    int nCurrWifiMode = 0;
    WLAN_CONNECTION stWlanConCfg = {0};

    //sample:"S=UFJPRFVDVF9URVNUX0RPTlRMSU5L&P=NTY1NjUwOTk=&T=+480&K=aw5fd&A=C0A80101+7E3"
    //初始化接收环境
    memset(szStrTmp, 0, sizeof(szStrTmp));
    memset(szWlanSSID, 0, sizeof(szWlanSSID));
    memset(szWlanKey, 0, sizeof(szWlanKey));
    memset(szTimeZone, 0, sizeof(szTimeZone));
    memset(szSession, 0, sizeof(szSession));
    nTimeZone = 0;
    bIsFindWlanSSID = 0;
    bIsFindWlanKey = 0;
    bIsFindTimeZone = 0;
    bIsFindSession = 0;
    bIsFindClientAddr = 0;
    u32ClientIpAddr = 0;
    u32ClientPort = 0;

    MAIN_LOG_I("\n[FAN-DEBUG][%s][%d]pDataBuf=%s uiDataLen=%d\n",
       __FUNCTION__, __LINE__,
       pDataBuf, uiDataLen);
#ifdef __IS_SUPPORT_WLAN__
    int nWlanScanStatue =1;

    MPU_NET_SetWlanScanStatus(&nWlanScanStatue);
#endif
    //提示扫码完成后
    ZWSys_SetSpeechContent(SPEECH_SYS_QRCODE_SCAN_OVER);
    //循环查询，等待播放结束
    i = 0;
    while (1)
    {
        SelectSleepMSec(100);
        if (0 == ZWSys_GetSpeechPlayingStatus())
        {
            break;
        }
        if (++i > (100))
        {
            i = 0;
            break;
        }
    }
    //解析扫码内容
    pStrTmp = (char *)pDataBuf;
    uiDealLen = 0;
    while (uiDealLen < uiDataLen)
    {
#if defined (__IS_SUPPORT_WLAN__)
        //解析SSID
        if (0 == bIsFindWlanSSID)
        {
            pStrTmp2 = strstr(pStrTmp, "S=");
            if (NULL != pStrTmp2)
            {
                pStrTmp = pStrTmp2;
                nTmp = strlen("S=");
                pStrTmp += nTmp;
                uiDealLen += nTmp;

                i = 0;
                memset(szStrTmp, 0, sizeof(szStrTmp));
                while (uiDealLen < uiDataLen)
                {
                    if ('\n' == *pStrTmp
                        || '\r' == *pStrTmp
                        || i >= (sizeof(szStrTmp)-2))
                    {
                        pStrTmp++;
                        uiDealLen++;
                        continue;
                    }
                    if ('&' == *pStrTmp
                        || '\0' == *pStrTmp)
                    {
                        pStrTmp++;
                        uiDealLen++;
                        break;
                    }

                    szStrTmp[i] = *pStrTmp;
                    pStrTmp++;
                    uiDealLen++;
                    i++;
                }

                base64Decode(szStrTmp, (unsigned char *)szWlanSSID);
                szWlanSSID[sizeof(szWlanSSID)-1] = 0;
                bIsFindWlanSSID = 1;
                continue;
            }
        }

        //解析password
        if (0 == bIsFindWlanKey)
        {
            pStrTmp2 = strstr(pStrTmp, "P=");
            if (NULL != pStrTmp2)
            {
                pStrTmp = pStrTmp2;
                nTmp = strlen("P=");
                pStrTmp += nTmp;
                uiDealLen += nTmp;

                i = 0;
                memset(szStrTmp, 0, sizeof(szStrTmp));
                while (uiDealLen < uiDataLen)
                {
                    if ('\n' == *pStrTmp
                        || '\r' == *pStrTmp
                        || i >= (sizeof(szStrTmp)-2))
                    {
                        pStrTmp++;
                        uiDealLen++;
                        continue;
                    }
                    if ('&' == *pStrTmp
                        || '\0' == *pStrTmp)
                    {
                        pStrTmp++;
                        uiDealLen++;
                        break;
                    }

                    szStrTmp[i] = *pStrTmp;
                    pStrTmp++;
                    uiDealLen++;
                    i++;
                }

                base64Decode(szStrTmp, (unsigned char *)szWlanKey);
                szWlanKey[sizeof(szWlanKey)-1] = 0;
                bIsFindWlanKey = 1;
                continue;
            }
        }

        //解析时区
        if (0 == bIsFindTimeZone)
        {
            pStrTmp2 = strstr(pStrTmp, "T=");
            if (NULL != pStrTmp2)
            {
                pStrTmp = pStrTmp2;
                nTmp = strlen("T=");
                pStrTmp += nTmp;
                uiDealLen += nTmp;

                i = 0;
                while (uiDealLen < uiDataLen)
                {
                    if ('\n' == *pStrTmp
                        || '\r' == *pStrTmp
                        || i >= (sizeof(szTimeZone)-2))
                    {
                        pStrTmp++;
                        uiDealLen++;
                        continue;
                    }
                    if ('&' == *pStrTmp
                        || '\0' == *pStrTmp)
                    {
                        pStrTmp++;
                        uiDealLen++;
                        break;
                    }

                    szTimeZone[i] = *pStrTmp;
                    pStrTmp++;
                    uiDealLen++;
                    i++;
                }

                nTimeZone = atoi(szTimeZone);
                bIsFindTimeZone = 1;
                continue;
            }
        }

        //解析配对信息
        if (0 == bIsFindSession)
        {
            pStrTmp2 = strstr(pStrTmp, "K=");
            if (NULL != pStrTmp2)
            {
                pStrTmp = pStrTmp2;
                nTmp = strlen("K=");
                pStrTmp += nTmp;
                uiDealLen += nTmp;

                i = 0;
                while (uiDealLen < uiDataLen)
                {
                    if (i >= (sizeof(szSession)-2))
                    {
                        pStrTmp++;
                        uiDealLen++;
                        continue;
                    }
                    if ('&' == *pStrTmp
                        || '\0' == *pStrTmp)
                    {
                        pStrTmp++;
                        uiDealLen++;
                        break;
                    }

                    szSession[i] = *pStrTmp;
                    pStrTmp++;
                    uiDealLen++;
                    i++;
                }

                bIsFindSession = 1;
                continue;
            }
        }

        //解析手机地址信息
        if (0 == bIsFindClientAddr)
        {
            pStrTmp2 = strstr(pStrTmp, "A=");
            if (NULL != pStrTmp2)
            {
                pStrTmp = pStrTmp2;
                nTmp = strlen("A=");
                pStrTmp += nTmp;
                uiDealLen += nTmp;

                i = 0;
                memset(szStrTmp, 0, sizeof(szStrTmp));
                while (uiDealLen < uiDataLen)
                {
                    if (i >= (sizeof(szStrTmp)-2))
                    {
                        pStrTmp++;
                        uiDealLen++;
                        continue;
                    }
                    if ('&' == *pStrTmp
                        || '\0' == *pStrTmp)
                    {
                        pStrTmp++;
                        uiDealLen++;
                        break;
                    }

                    szStrTmp[i] = *pStrTmp;
                    pStrTmp++;
                    uiDealLen++;
                    i++;
                }

                MAIN_LOG_I("\n[FAN-DEBUG][%s][%d]szStrTmp=%s\n",
                   __FUNCTION__, __LINE__,
                   szStrTmp);
                sscanf(szStrTmp, "%X+%X", &u32ClientIpAddr, &u32ClientPort);
                MAIN_LOG_I("\n[FAN-DEBUG][%s][%d][%X, %X]\n",
                   __FUNCTION__, __LINE__,
                   u32ClientIpAddr, u32ClientPort);
                bIsFindClientAddr = 1;
                continue;
            }
        }
#elif defined (__MODULE_4G_SUPPORT__)
        //解析服务器上传域名和接口名称
         if (0 == bIsFindServerAddr)
         {
             pStrTmp2 = strstr(pStrTmp, "V8=");
             if (NULL != pStrTmp2)
             {
                 pStrTmp = pStrTmp2;
                 nTmp = strlen("V8=");
                 pStrTmp += nTmp;
                 uiDealLen += nTmp;

                 i = 0;
                 memset(szServerAddress, 0, sizeof(szServerAddress));
                 while (uiDealLen < uiDataLen)
                 {
                     if ('\n' == *pStrTmp
                       || '\r' == *pStrTmp
                       || i >= (sizeof(szServerAddress)-2))
                     {
                         pStrTmp++;
                         uiDealLen++;
                         continue;
                     }
                     if ('&' == *pStrTmp
                       || '\0' == *pStrTmp)
                     {
                         pStrTmp++;
                         uiDealLen++;
                         break;
                     }

                     szServerAddress[i] = *pStrTmp;
                     pStrTmp++;
                     uiDealLen++;
                     i++;
                 }
                 bIsFindServerAddr = 1;
                 continue;
             }
         }

         //解析随机值
         if (0 == bIsFindRandomNumber)
         {
             pStrTmp2 = strstr(pStrTmp, "RN=");
             if (NULL != pStrTmp2)
             {
                 pStrTmp = pStrTmp2;
                 nTmp = strlen("RN=");
                 pStrTmp += nTmp;
                 uiDealLen += nTmp;

                 i = 0;
                 memset(szRandomNumber, 0, sizeof(szRandomNumber));
                 while (uiDealLen < uiDataLen)
                 {
                     if ('\n' == *pStrTmp
                       || '\r' == *pStrTmp
                       || i >= (sizeof(szRandomNumber)-2))
                     {
                         pStrTmp++;
                         uiDealLen++;
                         continue;
                     }
                     if ('&' == *pStrTmp
                       || '\0' == *pStrTmp)
                     {
                         pStrTmp++;
                         uiDealLen++;
                         break;
                     }

                     szRandomNumber[i] = *pStrTmp;
                     pStrTmp++;
                     uiDealLen++;
                     i++;
                 }
                 bIsFindRandomNumber = 1;
                 continue;
             }
         }

         //解析UserId
         if (0 == bIsFindUserId)
         {
             pStrTmp2 = strstr(pStrTmp, "U=");
             if (NULL != pStrTmp2)
             {
                 pStrTmp = pStrTmp2;
                 nTmp = strlen("U=");
                 pStrTmp += nTmp;
                 uiDealLen += nTmp;

                 i = 0;
                 memset(szUserId, 0, sizeof(szUserId));
                 while (uiDealLen < uiDataLen)
                 {
                     if ('\n' == *pStrTmp
                         || '\r' == *pStrTmp
                         || i >= (sizeof(szUserId)-2))
                     {
                         pStrTmp++;
                         uiDealLen++;
                         continue;
                     }
                     if ('&' == *pStrTmp
                         || '\0' == *pStrTmp)
                     {
                         pStrTmp++;
                         uiDealLen++;
                         break;
                     }

                     szUserId[i] = *pStrTmp;
                     pStrTmp++;
                     uiDealLen++;
                     i++;
                 }
                 bIsFindUserId = 1;
                 continue;
             }
         }
#endif

        //避免内容非法导致出不去
        pStrTmp++;
        uiDealLen++;
    }

#if defined (__IS_SUPPORT_WLAN__)
    //配网信息必须有效才使用本次扫描到的信息
    if (bIsFindWlanSSID
        && strlen(szWlanSSID) > 0)
    {
        //查询上次配网是否已结束
        if (0 == MPU_WLAN_IsConfigureNetIng())
        {
            /* 配网 */
            // AP模式下才配网
            MPU_NET_GetCurrWifiMode(&nCurrWifiMode);
            if (SOFT_AP_MODE == nCurrWifiMode)
            {
                if (bIsFindClientAddr)
                {
                    //保存配网客户端地址信息
                    MPU_WLAN_SetClientAddrInfo(u32ClientIpAddr, u32ClientPort);
                }

                //配网
                memset(&stWlanConCfg, 0, sizeof(stWlanConCfg));
                memcpy(stWlanConCfg.szSSID, szWlanSSID, sizeof(stWlanConCfg.szSSID)-1);
                stWlanConCfg.struKeyCfg.bValid = 1;
                // stWlanConCfg.struKeyCfg.Key1.bValid = 1;
                // memcpy(stWlanConCfg.struKeyCfg.Key1.szKey, szWlanKey,
                //     sizeof(stWlanConCfg.struKeyCfg.Key1.szKey)-1);
                // stWlanConCfg.struKeyCfg.Key2.bValid = 1;
                // memcpy(stWlanConCfg.struKeyCfg.Key2.szKey, szWlanKey,
                //     sizeof(stWlanConCfg.struKeyCfg.Key2.szKey)-1);
                // stWlanConCfg.struKeyCfg.Key3.bValid = 1;
                // memcpy(stWlanConCfg.struKeyCfg.Key3.szKey, szWlanKey,
                //     sizeof(stWlanConCfg.struKeyCfg.Key3.szKey)-1);
                // stWlanConCfg.struKeyCfg.Key4.bValid = 1;
                // memcpy(stWlanConCfg.struKeyCfg.Key4.szKey, szWlanKey,
                //     sizeof(stWlanConCfg.struKeyCfg.Key4.szKey)-1);
                stWlanConCfg.struKeyCfg.WPAKey.bValid = 1;
                memcpy(stWlanConCfg.struKeyCfg.WPAKey.szKey, szWlanKey,
                    sizeof(stWlanConCfg.struKeyCfg.WPAKey.szKey)-1);
                // 防止带验证低于8位后AP模式切换异常, 做简单判断
                int keyLen = strlen(szWlanKey);
                if ((keyLen == 0) || (keyLen >= 8))
                {
                    /* 切换到STA模式 */
                    MPU_WLAN_SetConnection(&stWlanConCfg);
                }
                else
                {
                    //提示扫码完成后
                    ZWSys_SetSpeechContent(SPEECH_WIFI_CONNECTING);
                    //循环查询，等待播放结束
                    i = 0;
                    while (1)
                    {
                        SelectSleepMSec(100);
                        if (0 == ZWSys_GetSpeechPlayingStatus())
                        {
                            break;
                        }
                        if (++i > (10*10))
                        {
                            i = 0;
                            break;
                        }
                    }

                    //提示扫码完成后
                    ZWSys_SetSpeechContent(SPEECH_WIFI_CONNECT_FAIL_PWD_WRONG);
                    //循环查询，等待播放结束
                    i = 0;
                    while (1)
                    {
                        SelectSleepMSec(100);
                        if (0 == ZWSys_GetSpeechPlayingStatus())
                        {
                            break;
                        }
                        if (++i > (10*10))
                        {
                            i = 0;
                            break;
                        }
                    }

                        //提示扫码完成后
                    ZWSys_SetSpeechContent(SPEECH_WIFI_PLEASE_CONNECT);
                    //循环查询，等待播放结束
                    i = 0;
                    while (1)
                    {
                        SelectSleepMSec(100);
                        if (0 == ZWSys_GetSpeechPlayingStatus())
                        {
                            break;
                        }
                        if (++i > (10*10))
                        {
                            i = 0;
                            break;
                        }
                    }
                }

                //设置时区
                if (bIsFindTimeZone)
                {
                    nTmp = MPU_SYS_SetTimeZone(nTimeZone);
                }

                //保存配对信息
                if (bIsFindSession)
                {
                    nTmp = MPU_WLAN_SaveQrConfigureWlanSessionID(szSession);
                }
            }
        }
    }
#elif defined (__MODULE_4G_SUPPORT__)
    //4G绑定设置信息逻辑
    if (bIsFindServerAddr && strlen(szServerAddress) > 0)
    {
        //MAIN_LOG_I("[%s:%d]szServerAddress:%s\n", __FILE__, __LINE__, szServerAddress);
        MPU_Net_SetServerAddress(szServerAddress);
        if (bIsFindRandomNumber && strlen(szRandomNumber) > 0)
        {
            //MAIN_LOG_I("[%s:%d]szRandomNumber:%s\n", __FILE__, __LINE__, szRandomNumber);
            MPU_Net_SetRandomNumber(szRandomNumber);
            if (bIsFindUserId && strlen(szUserId) > 0)
            {
                //MAIN_LOG_I("[%s:%d]szUserId:%s\n", __FILE__, __LINE__, szUserId);
                MPU_Net_SetUserId(szUserId);
                MPU_Net_SetScanEndStatus(1);
            }
        }
    }
#endif
    return 0;
}

void InitDeviceCapacity()
{
    g_tGuiState.iEncodeCh = MPU_AVL_GetChNum();
    g_tGuiState.iAlarmInNum = MPU_DIP_GetPortNum();
    g_tGuiState.iAlarmOutNum = MPU_DOP_GetPortNum();
}

int EncChNum()
{
    if (g_tGuiState.iEncodeCh > 0 && g_tGuiState.iEncodeCh <= MAX_ENC_CHANNEL_NUM)
    {
        return g_tGuiState.iEncodeCh;
    }
    else
    {
        return MAX_ENC_CHANNEL_NUM;
    }
}

int AlarmInNum()
{
    return g_tGuiState.iAlarmInNum;
}


//!\brief 创建通道OSD控件
//!\param HWND hWndDesk 桌面窗口的句柄
//!\return 无
int Osd_Init()
{
    int i = 0;

    int iChNum = EncChNum();
    for (i = 0; i < iChNum; i++)
    {
        tChannelsState[i].ulMotionTimeLast = 0;
        tChannelsState[i].ulHumanShapeTimeLast = 0;
        tChannelsState[i].ulSoundTimeLast = 0;
    }

    return 0;
}

void CreateStartFlag()
{
    int fdStartFlag = open(STARTFLAG_NAME, O_RDWR | O_CREAT | O_TRUNC);

    close(fdStartFlag);
}

void DeleteStartFlag()
{
    unlink(STARTFLAG_NAME);
}

int Halt(DWORD dwRemoteUserID, DWORD nHaltType)
{
    eShutDownType eOperation = (eShutDownType) nHaltType;

    int nEncID, nCount;

    if ((DWORD) LOCAL_USER_ID != dwRemoteUserID)
    {
        HANDLE hUser;

        char szUserName[MAX_USERNAME_LEN + 4];

        char szIPAddr[16];

        char szLoginTime[32];

        MPU_CIU_GetRemoteByID(dwRemoteUserID, &hUser, szUserName, sizeof(szUserName), szIPAddr, sizeof(szIPAddr), szLoginTime, sizeof(szLoginTime));
        if (eHalt == eOperation)
        {
            MPU_OPL_AddLog(EDVR_EVENT_OPT, EDVR_EVENT_OPT_DETAIL_SHUTDOWN, 0, 0, 1, szUserName);
        }
        else
        {
            MPU_OPL_AddLog(EDVR_EVENT_OPT, EDVR_EVENT_OPT_DETAIL_REMOTE_RESTART, 0, 0, 1, szUserName);
        }
    }
    else
    {
        MPU_OPL_AddLog(EDVR_EVENT_OPT, EDVR_EVENT_OPT_DETAIL_SHUTDOWN, 0, 0, 0, NULL);
    }

    DeleteStartFlag();

    //需暂停录像
    nCount = MPU_AVL_GetChNum();
    for (nEncID = 0; nEncID < nCount; ++nEncID)
    {
        MPU_AVL_Record_Pause(nEncID, TRUE);
    }

    MPU_PTZ_SetEletronicProp(0, ePropreboot, 0);

    if (eHalt != eOperation)
    {
        MPU_SYS_Reboot_safe();
    }
    else
    {
        MPU_SYS_Halt_safe();
    }

    return 0;
}


//!\brief 维护回调函数
void Maintenance_Cb()
{
    int ret = 1;
    MAIN_LOG_I("-----into Maintenance_Cb-----\n");
    //ret = SendNotifyMessage(g_tGuiWnd.hDeskWnd, MSG_SHUTDOWN, (WPARAM) LOCAL_USER_ID, (LPARAM) eReboot);
    if (ret)
    {
        MPU_PAR_SaveConfig();
        PAR_ForbiddenSaveFile();
        Halt((DWORD) LOCAL_USER_ID, (DWORD) eReboot);
    }
}

//!\brief 休眠唤醒或唤醒计划回调函数
void AwakeSchedule_Cb(int AwakeMode)
{
    // 睡眠需要联动功能项：云台、移动追踪、告警及播报、隐私遮挡
    MAIN_LOG_I("----- Into AwakeSchedule_Cb: %d -----\n", AwakeMode);

    static EventDevSleepState Edata = {0};
    Edata.dwID++;

    if (AwakeMode)
    {
        // 执行唤醒相关动作
        MPU_AVL_UpdateDorStatus(0);		// 解除移动、人形、追踪屏蔽
        MPU_PTZ_UpdateDorStatus(0);
        MPU_STR_Disk_ResumeSdRecord();
        ZWSys_SetDormancyState(0);
        Edata.nStatus = 1;
    }
    else
    {
        // 执行休眠相关动作
        MPU_AVL_UpdateDorStatus(1);		// 开启屏蔽移动、人形、追踪
        MPU_PTZ_UpdateDorStatus(1);
        ZWSys_SetDormancyState(1);
        MPU_STR_Disk_PauseSdRecord();
        Edata.nStatus = 0;
    }
}

static void *DIP_Check_thread(void *param)
{
    PRT_TID();
    pthread_detach(pthread_self());

    int iState,i;
    while(1)
    {
        if(1 == uninitFlag)
            break;
        iState = MPU_DIP_GetAllState();
        for (i = 0; i < ALARM_NUM; i++)
        {
            // if (1 == g_aiAlarmIn[i])
            if ((iState & (1 << i)))
            {
                if (0 == GetBit32(g_tGuiState.iAlarmInState, i))
                {
                    char szUsername[MAX_USERNAME_LEN + 1];
                    HANDLE hUser = MPU_USR_GetLocalLoginUser();
                    MPU_USR_GetUserName(hUser, szUsername, MAX_USERNAME_LEN);
                    MPU_OPL_AddLog(EDVR_EVENT_ALARM, EDVR_EVENT_ALARM_DETAIL_INPUT_ALARM,
                                   i, 0, 0, szUsername);
                    AlarmAction_SendEmail(ALERT_TYPE_ALARMIN, i);
                    AlarmAction_SendFtp(ALERT_TYPE_ALARMIN, i);        //报警触发
                    AlarmAction_UploadSdCard(ALERT_TYPE_ALARMIN, i);
                }
            }
        }
        #if 0
        if (0 != iState)
            LED_On(LED19);
        else
            LED_Off(LED19);
        #endif
        g_tGuiState.iAlarmInState = iState;

        sleep(1);
    }

    return NULL;
}

static unsigned int  myGetTickCount()
{
    unsigned int uptime = 0;
    struct timespec on;
    if(clock_gettime(CLOCK_MONOTONIC, &on) == 0)
             uptime = on.tv_sec*1000 + on.tv_nsec/1000000;
    return uptime;
}

//当2s没有回调时认为移动消失
static void *Motion_Check_thread(void *param)
{
    PRT_TID();
    pthread_detach(pthread_self());

    int i = 0;
    unsigned int ulMotionTimeLast[MAX_ENC_CHANNEL_NUM] = {0}, ulMotionTimeNow = 0;
    char szUsername[MAX_USERNAME_LEN + 1] = {0};
    HANDLE hUser = NULL;

    while(1)
    {
        if(1 == uninitFlag)
            break;

        //for(i=0;i<MAX_ENC_CHANNEL_NUM;i++)
        for(i=0;i<1;i++)
        {
            pthread_mutex_lock(&s_hMDLock);
            if (tChannelsState[i].ulMotionTimeLast==0 || ulMotionTimeLast[i]==tChannelsState[i].ulMotionTimeLast)
            {
                pthread_mutex_unlock(&s_hMDLock);
                continue;
            }

            ulMotionTimeLast[i] = tChannelsState[i].ulMotionTimeLast;

            pthread_mutex_unlock(&s_hMDLock);

            ulMotionTimeNow = myGetTickCount();
            if (ulMotionTimeNow <= ulMotionTimeLast[i])
                continue;

            if (ulMotionTimeNow-ulMotionTimeLast[i] >= 2000)
            {
                //now no motion detect
                hUser = MPU_USR_GetLocalLoginUser();
                MPU_USR_GetUserName(hUser, szUsername, MAX_USERNAME_LEN);
                MPU_OPL_AddLog(EDVR_EVENT_ALARM, EDVR_EVENT_ALARM_DETAIL_MOTION_STOP, i, 0, 0, szUsername);
            }
            else
            {
                //now there is motion detect
                hUser = MPU_USR_GetLocalLoginUser();  //获取本地登录用户
                MPU_USR_GetUserName(hUser, szUsername, MAX_USERNAME_LEN);
                MPU_OPL_AddLog(EDVR_EVENT_ALARM, EDVR_EVENT_ALARM_DETAIL_MOTION_START,
                              i, 0, 0, szUsername);   //添加日志

                AlarmAction_SendEmail(ALERT_TYPE_MOTION, i);
                AlarmAction_SendFtp(ALERT_TYPE_MOTION, i);		//报警触发
                AlarmAction_UploadSdCard(ALERT_TYPE_MOTION, i);
            }
        }
        usleep(100 *1000);
    }
    return NULL;
}

//当2s没有回调时认为移动消失
static void *HumanShape_Check_thread(void *param)
{
    PRT_TID();
    pthread_detach(pthread_self());

    int i = 0;
    unsigned int ulTimeLast[MAX_ENC_CHANNEL_NUM] = {0}, ulTimeNow = 0;

    while (1)
    {
        if (1 == uninitFlag)
        {
            break;
        }

        //for(i=0;i<MAX_ENC_CHANNEL_NUM;i++)
        for (i = 0; i < 1; i++)
        {
            pthread_mutex_lock(&s_hHumanShapeLock);
            if (tChannelsState[i].ulHumanShapeTimeLast == 0
                || ulTimeLast[i] == tChannelsState[i].ulHumanShapeTimeLast)
            {
                pthread_mutex_unlock(&s_hHumanShapeLock);
                continue;
            }
            ulTimeLast[i] = tChannelsState[i].ulHumanShapeTimeLast;
            pthread_mutex_unlock(&s_hHumanShapeLock);

            ulTimeNow = myGetTickCount();
            if (ulTimeNow <= ulTimeLast[i])
                continue;

            if (ulTimeNow-ulTimeLast[i] >= 2000)
            {
            }
            else
            {
                AlarmAction_SendEmail(ALERT_TYPE_HUMANSHAPE, i);
                AlarmAction_SendFtp(ALERT_TYPE_HUMANSHAPE, i);		//报警触发
                AlarmAction_UploadSdCard(ALERT_TYPE_HUMANSHAPE, i);
            }
        }
        usleep(100 *1000);
    }
    return NULL;
}

//当2s没有回调时认为声音消失
static void *Sound_Check_thread(void *param)
{
    PRT_TID();
    pthread_detach(pthread_self());

    int i = 0;
    unsigned int ulTimeLast[MAX_ENC_CHANNEL_NUM] = {0}, ulTimeNow = 0;
    //char szUsername[MAX_USERNAME_LEN + 1] = {0};
    //HANDLE hUser = NULL;

    while (1)
    {
        for (i = 0; i < 1; i++)
        {
            pthread_mutex_lock(&s_hSoundLock);
            if (tChannelsState[i].ulSoundTimeLast == 0
                || ulTimeLast[i] == tChannelsState[i].ulSoundTimeLast)
            {
                pthread_mutex_unlock(&s_hSoundLock);
                continue;
            }
            ulTimeLast[i] = tChannelsState[i].ulSoundTimeLast;
            pthread_mutex_unlock(&s_hSoundLock);

            ulTimeNow = myGetTickCount();
            if (ulTimeNow <= ulTimeLast[i])
                continue;

            if (ulTimeNow-ulTimeLast[i] >= 2000)
            {
            }
            else
            {
                AlarmAction_SendEmail(ALERT_TYPE_SOUNDDETECT, i);
                AlarmAction_SendFtp(ALERT_TYPE_SOUNDDETECT, i);		//报警触发
                AlarmAction_UploadSdCard(ALERT_TYPE_SOUNDDETECT, i);
            }
        }
        usleep(100 *1000);
    }
    return NULL;
}


//!\brief 移动侦测的osd 显示回调函数
//!\param struct tagAlertInfo *pAlertInfo   报警信息
//!\param struct tagActionInfo *pActionInfo 报警处理信息
//!\return
int Osd_MotionCallBack(struct tagAlertInfo *pAlertInfo, struct tagActionInfo *pActionInfo)
{
    assert(NULL != pAlertInfo && pActionInfo != NULL);
    if (pAlertInfo->AlertType != ALERT_TYPE_MOTION)
    {
        MAIN_LOG_I("Osd_MotionCallBack :it's not motion alert\n");
        return -1;
    }
    if (pAlertInfo->nChannel < 0 || pAlertInfo->nChannel >= EncChNum())
    {
        MAIN_LOG_I("Osd_MotionCallBack :pAlertInfo->nChannel erro\n");
        return -1;
    }

    //只是用来通知运动侦测已经停止，目前用不上
    if (pAlertInfo->dwReserved == 0)
        return 0;

    pthread_mutex_lock(&s_hMDLock);
    tChannelsState[pAlertInfo->nChannel].ulMotionTimeLast = myGetTickCount();
    pthread_mutex_unlock(&s_hMDLock);

    return 0;
}

//!\brief 人形检测的osd 显示回调函数
//!\param struct tagAlertInfo *pAlertInfo   报警信息
//!\param struct tagActionInfo *pActionInfo 报警处理信息
//!\return
int Osd_HumanShapeCallBack(struct tagAlertInfo *pAlertInfo, struct tagActionInfo *pActionInfo)
{
    assert(NULL != pAlertInfo && pActionInfo != NULL);
    if (pAlertInfo->AlertType != ALERT_TYPE_HUMANSHAPE)
    {
        MAIN_LOG_I("Osd_MotionCallBack :it's not human alert\n");
        return -1;
    }
    if (pAlertInfo->nChannel < 0 || pAlertInfo->nChannel >= EncChNum())
    {
        MAIN_LOG_I("Osd_MotionCallBack :pAlertInfo->nChannel erro\n");
        return -1;
    }

    //只是用来通知运动侦测已经停止，目前用不上
    if (pAlertInfo->dwReserved == 0)
        return 0;

    pthread_mutex_lock(&s_hHumanShapeLock);
    tChannelsState[pAlertInfo->nChannel].ulHumanShapeTimeLast = myGetTickCount();
    pthread_mutex_unlock(&s_hHumanShapeLock);

    return 0;
}

//!\brief 声音检测的osd 显示回调函数
//!\param struct tagAlertInfo *pAlertInfo   报警信息
//!\param struct tagActionInfo *pActionInfo 报警处理信息
//!\return
int Osd_SoundCallBack(struct tagAlertInfo *pAlertInfo, struct tagActionInfo *pActionInfo)
{
    assert(NULL != pAlertInfo && pActionInfo != NULL);
    if (pAlertInfo->AlertType != ALERT_TYPE_SOUNDDETECT)
    {
        MAIN_LOG_I("Osd_SoundCallBack :it's not motion alert\n");
        return -1;
    }
    if (pAlertInfo->nChannel < 0 || pAlertInfo->nChannel >= EncChNum())
    {
        MAIN_LOG_I("Osd_SoundCallBack :pAlertInfo->nChannel erro\n");
        return -1;
    }

    //只是用来通知声音检测已经停止，目前用不上
    if (pAlertInfo->dwReserved == 0)
        return 0;

    pthread_mutex_lock(&s_hSoundLock);
    tChannelsState[pAlertInfo->nChannel].ulSoundTimeLast = myGetTickCount();
    pthread_mutex_unlock(&s_hSoundLock);

    return 0;
}


//!\brief 报警回调函数
//!\param struct tagAlertInfo *pAlertInfo 报警输入
//!\param struct tagActionInfo *pActionInfo 报警响应
//!\return

//type: -1 - 无;0- 报警输入；1-IP冲突；2-网络断开;3-硬盘故障;4-盘组满;5-盘组异常;6-访问非法;7-视频丢失;8-移动侦测
//int AlarmIn_ListCallBackFxn(int ch,int type);
int MainAlarmCallBack(struct tagAlertInfo *pAlertInfo, struct tagActionInfo *pActionInfo)
{
    //MAIN_LOG_I("--MainAlarmCallBack---AlertType=0x%x---tActionType=0x%x\n",pAlertInfo->AlertType,pActionInfo->tActionType);
    assert(NULL != pAlertInfo && pActionInfo != NULL);
    switch (pAlertInfo->AlertType)
    {
            //!报警输入
        case ALERT_TYPE_ALARMIN:
            {
                int iChannnel = pAlertInfo->nChannel;

                if (iChannnel < 0 || iChannnel >= AlarmInNum())
                {
                    MAIN_LOG_I("iChannnel = %d erro\n", iChannnel);
                    return -1;
                }
                static pthread_t DIPThrdID=0;			//线程ID
                if(0 == DIPThrdID)
                    pthread_create(&DIPThrdID, NULL, DIP_Check_thread, NULL);

                MAIN_LOG_I("[%s] ch =%d alarm in\n", __FUNCTION__, iChannnel);
                break;
            }
            //!移动侦测
        case ALERT_TYPE_MOTION:
            {
                // 移动侦测的osd 显示回调函数
                MAIN_LOG_I("ALERT_TYPE_MOTION ch %d\n", pAlertInfo->nChannel);
                Osd_MotionCallBack(pAlertInfo, pActionInfo);

                static pthread_t MotionThrdID=0;			//线程ID
                if(0 == MotionThrdID)
                    pthread_create(&MotionThrdID, NULL, Motion_Check_thread, NULL); //创建移动侦测线程
                break;
            }
        case ALERT_TYPE_HUMANSHAPE:
            {
                // 人形检测的osd 显示回调函数
                MAIN_LOG_I("ALERT_TYPE_HUMANSHAPE ch %d\n", pAlertInfo->nChannel);
                Osd_HumanShapeCallBack(pAlertInfo, pActionInfo);

                static pthread_t HumanShapeThrdID=0;			//线程ID
                if (0 == HumanShapeThrdID)
                {
                    pthread_create(&HumanShapeThrdID, NULL, HumanShape_Check_thread, NULL); //创建人形检测线程
                }
                break;
            }
        case ALERT_TYPE_SOUNDDETECT:
            {
                // 声音检测
                //MAIN_LOG_I("ALERT_SOUNDDETECT ch %d\n", pAlertInfo->nChannel);
                Osd_SoundCallBack(pAlertInfo, pActionInfo);

                static pthread_t SoundThrdID=0;            //线程ID
                if (0 == SoundThrdID)
                {
                    pthread_create(&SoundThrdID, NULL, Sound_Check_thread, NULL); //创建声音检测线程
                }
                break;
            }

            //!编码异常
        case ALERT_TYPE_ENCODEERROR:
            MAIN_LOG_I("ALERT_TYPE_ENCODEERROR:" __TIME__ "-" __DATE__ "\n");
            break;

            //!视频丢失
        case ALERT_TYPE_VIDEOLOST:
            MAIN_LOG_I("ALERT_TYPE_VIDEOLOST:" __TIME__ "-" __DATE__ "\n");
            break;
            //!视频异常
        case ALERT_TYPE_VIDEOEXCEPTION:
            MAIN_LOG_I("!!!!!!!ALERT_TYPE_VIDEOEXCEPTION\n");
            break;
            //!硬盘错误
        case ALERT_TYPE_DISKERROR:
            MAIN_LOG_I("!!!!!!!ALERT_TYPE_DISKERROR\n");
            break;
            //!盘组异常
        case ALERT_TYPE_DISKGROUP_ERROR:
            MAIN_LOG_I("!!!!!!!ALERT_TYPE_DISKGROUP_ERROR\n");
            break;
            //!IP冲突
        case ALERT_TYPE_IPCONFLICT:
            MAIN_LOG_I("!!!!!!!ALERT_TYPE_IPCONFLICT\n");
            break;
            //!非法访问
        case ALERT_TYPE_ILLEGEACCESS:
            MAIN_LOG_I("!!!!!!!ALERT_TYPE_ILLEGEACCESS\n");
            break;
            //!网线断
        case ALERT_TYPE_RETICLEDISCONNECT:
            //MAIN_LOG_I("!!!!!!!ALERT_TYPE_RETICLEDISCONNECT\n");
            break;
            //!盘组满
        case ALERT_TYPE_DISKFULL:
            MAIN_LOG_I(" call ALERT_TYPE_DISKFULL !!!!!!\n");
            break;
    }
    return 0;
}

#if defined (__IS_SUPPORT_WLAN__)
//0-ifBreak:link up
//1-ifBreak:link down
static int checkETH0Break(int *ifBreak)
{
    int sock = 0;
    int mii_val = 0;
    struct ifreq ifr = {0};
    struct ethtool_value edata = {0};
    mii_ioctl_data *mii = NULL;

    //检测网络是否断开
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        MAIN_LOG_I("socket on eth0 failed: %s\n", strerror(errno));
        *ifBreak = 1;
    }
    else
    {
        strncpy(ifr.ifr_name, "eth0", sizeof(ifr.ifr_name) - 1);
        edata.cmd = ETHTOOL_GLINK;

        ifr.ifr_data = (char *) &edata;
        if (-1 == ioctl(sock, SIOCETHTOOL, &ifr))//获取当前网络连接状态
        {
            MAIN_LOG_I("ETHTOOL_GLINK failed: %s\n", strerror(errno));

            //将继续进行 SIOCGMIIPHY
            if (ioctl(sock, SIOCGMIIPHY, &ifr) < 0)
            {
                MAIN_LOG_I("SIOCGMIIPHY on eth0 failed: %s\n", strerror(errno));
            }
            else
            {
                mii = (mii_ioctl_data *) & ifr.ifr_data;
                mii->reg_num = MII_BMSR;
                if (ioctl(sock, SIOCGMIIREG, &ifr) < 0)
                {
                    MAIN_LOG_I("SIOCGMIIREG on eth0 failed: %s\n", strerror(errno));
                }
                mii_val = mii->val_out;

                if (!(mii_val & BMSR_LSTATUS))
                {
                    *ifBreak = 1;
                }
            }

        }
        else
        {
            *ifBreak = (edata.data ? 0 : 1);	//0 - 连接 ,1-断开lhl modify
        }
        close(sock);

    }

    return 0;
}
#endif

int StartSystemStatusLed()
{
    int rc = 0;
    int counter = 0;
    char szSCode[64] = {0};
    int nSCodeLen = sizeof(szSCode);

    ZWSys_StartSysStatusLed(ZWSys_EncChannelOpen(0),LED_HZ);
    rc = MPU_SYS_DeviceBind_GetSCode(szSCode, &nSCodeLen);

#ifndef __MODULE_4G_SUPPORT__
    if((0 != rc) || (0 == strlen(szSCode)))
#else
    int nSupportBoardCast = 0;
    int ret = ZWSys_GetBroadCastEnable(&nSupportBoardCast);
    if(((0 != rc) || (0 == strlen(szSCode))) || ((ret == 0) && nSupportBoardCast))
#endif
    {
#ifdef __MODULE_4G_SUPPORT__
        ZWSys_SetSpeechContent(SPEECH_4G_SYS_START_OVER);
#else
        ZWSys_SetSpeechContent(SPEECH_SYS_START_OVER);
#endif
        //循环查询，等待播放结束
        counter = 0;
        while (1)
        {
            sleep(1);
            if (0 == ZWSys_GetSpeechPlayingStatus())
            {
                break;
            }
            if (++counter > 10)
            {
                counter = 0;
                break;
            }
        }
    }

    return 0;
}

static void *jffs2_check(void *param)
{
    PRT_TID();
    pthread_detach(pthread_self());

    char command[64] = {0};
    char buf[256] = {0};
    int nRebootFlag = 0;
    int nCheckFlag = 1;
    FILE *fp;
    char *point;

     while (1)
     {
        if (nCheckFlag)
        {
            sleep(25);
            nCheckFlag = 0;
            continue;
        }

        //获得dmesg信息
        memset(command, 0, sizeof(command));
        snprintf(command, sizeof(command), "dmesg > /tmp/.dmesg");
        mysystem(command);

        fp = fopen("/tmp/.dmesg", "r");
        if(fp == NULL)
        {
            MAIN_LOG_I("dmesg---fopen %s faile!\n", "/tmp/.dmesg");
            sleep(10);
            continue;
        }

        memset(buf, 0, sizeof(buf));
        while(NULL != fgets(buf, 255, fp))
        {
            point = strstr(buf, "check_node_data");
            if(NULL != point)
            {
                nRebootFlag = 1;
                break;
            }
            else
            {
                nRebootFlag = 0;
            }
            memset(buf, 0, sizeof(buf));
        }
        fclose(fp);

        if (0 != access("/mnt/mtd/config/.jffs2Err:2", F_OK))
        {
            if (1 == nRebootFlag)
            {
                if (0 == access("/mnt/mtd/config/.jffs2Err:1", F_OK))
                {
                    MAIN_LOG_I("\n Second dmesg info : jffs2 notice! so touch /mnt/mtd/config/.jffs2Err:2\n");
                    mysystem("rm /mnt/mtd/config/.jffs2Err:1");
                    mysystem("touch /mnt/mtd/config/.jffs2Err:2");
                    sleep(2);
                    mysystem("reboot");
                }
                else
                {
                    MAIN_LOG_I("\n First dmesg info : jffs2 notice! so touch /mnt/mtd/config/.jffs2Err:1\n");
                    mysystem("touch /mnt/mtd/config/.jffs2Err:1");
                    sleep(2);
                    mysystem("reboot");
                }
            }
            else if (0 == access("/mnt/mtd/config/.jffs2Err:1", F_OK) && 0 == nRebootFlag)//第二次正常了，就把标记删掉
            {
                MAIN_LOG_I("\n Second dmesg info is normal, jffs2 is normal \n");
                mysystem("rm /mnt/mtd/config/.jffs2Err:1");
            }
        }
        else if (0 == access("/mnt/mtd/config/.jffs2Err:2", F_OK) && 0 == nRebootFlag)//第三次正常了，就把标记删掉
        {
            MAIN_LOG_I("\n Third dmesg info is normal, jffs2 is normal \n");
            mysystem("rm /mnt/mtd/config/.jffs2Err:2");
        }

        sleep(10);
     }

}

int Server(DWORD dwPort, int protocol)
{
    int h;
    DWORD Type = SOCK_STREAM;
    DWORD ProtocolName = IPPROTO_TCP;
    sockaddr_in	saddr;
    memset(&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = INADDR_ANY;
    saddr.sin_port = ntohs((WORD)dwPort);

    if(0 == protocol)
    {
        Type = SOCK_STREAM;
        ProtocolName = IPPROTO_TCP;
    }
    else
        if(1 == protocol)
        {
            Type = SOCK_DGRAM;
            ProtocolName = IPPROTO_UDP;
        }

    //启动网络服务
    h = socket(AF_INET, Type, ProtocolName);
    if (0 == h || -1 == h)
    {
        MAIN_LOG_I("socket error!\n");
        return -1;
    }

    int iValue = 1;
    //地址重用
    setsockopt(h, SOL_SOCKET, SO_REUSEADDR, (char*)&iValue, sizeof (iValue));

    //绑定
    DWORD lErrorCode = bind(h, (sockaddr*)&saddr, sizeof(saddr));
    if (0 != lErrorCode)
    {
        close(h);
        //MAIN_LOG_I("bind error!\n");
        return -1;
    }

    if(0 == protocol)
    {
        //监听
        lErrorCode = listen(h, 1);
        if (0 != lErrorCode)
        {
            MAIN_LOG_I("listen error!\n");
            close(h);
            return -1;
        }
    }
    close(h);

    return 0;
}

static void *port_check(void *param)
{
    PRT_TID();
    pthread_detach(pthread_self());

    CConfig *pcfg = CConfig::GetInstance();
    int commandPort;
    int mediaPort;
    int rtspPort;
    int autoDetectPort;

    int ret;

    int checkCount = 0;//超时时间

    //等待私有协议加载了再启动其它协议
    while(access("/tmp/.otherProtocolStarted", 0))
    {
        sleep(1);
        checkCount++;
        if(checkCount >= 300)//5 分钟
            break;
    }
//	mysystem("rm -rf /tmp/.otherProtocolStarted");
//	MAIN_LOG_I("check /tmp/.otherProtocolStarted\n");
    sleep(30);

    if (NULL != pcfg)
    {
        commandPort = pcfg->GetValue("CIU", "CommandPort", 6060);
        mediaPort = pcfg->GetValue("CIU", "MediaPort", 6066);
        rtspPort = pcfg->GetValue("CIU", "RtpPort", 554);
        autoDetectPort = pcfg->GetValue("CIU", "AutoDetectPort", 5555);

        MAIN_LOG_I("get CommandPort=%d, MediaPort=%d, RtpPort=%d, AutoDetectPort=%d\n", commandPort, mediaPort, rtspPort, autoDetectPort);

        ret = Server(commandPort, 0);
        if(0 == ret)
        {
            MPU_PAR_SaveConfigReal();
            PAR_ForbiddenSaveFile();
            MAIN_LOG_I("########### No Create Command Port listen, need reboot ###########\n");
            mysystem("reboot");
        }

        ret = Server(mediaPort, 0);
        if(0 == ret)
        {
            MPU_PAR_SaveConfigReal();
            PAR_ForbiddenSaveFile();
            MAIN_LOG_I("########### No Create Media Port listen, need reboot ###########\n");
            mysystem("reboot");
        }

        /*ret = Server(rtspPort, 0);
        if(0 == ret)
        {
            MAIN_LOG_I("########### No Create rtsp Port listen, need reboot ###########\n");
            mysystem("reboot");
        }*/
//udp的监听端口可以重复创建，所以不能用来检测自动探测端口
#if 0
        ret = Server(autoDetectPort, 1);
        if(0 == ret)
        {
            MAIN_LOG_I("########### No Create Auto Detect Port listen, need reboot ###########\n");
            mysystem("netstat -an");
            mysystem("reboot");
        }
#endif
    }
    return NULL;
}

/*
*此功能为了防止ONVIF TEST工具将探测模式改为1后重启后IP变了改不回来
*做的保护，目前该模式还未在其它地方用到过，所以可以在启动后
*3分钟直接将它强制改为0，经测试正常情况ONVIF TEST测试以前会在之前设置了
*/
static void *onvifDiscoveryMode_check(void *param)
{
    PRT_TID();
    pthread_detach(pthread_self());

    CConfig *pConfig = CConfig::GetInstance();
    int mode = 0;

    sleep(180);
    if (NULL != pConfig)
    {
        mode = pConfig->GetValue(DEVICE_INFO, "DiscoveryMode", 0);
        if(mode)//非零表示不允许探测
        {
            pConfig->SetValue(DEVICE_INFO, "DiscoveryMode", 1);
            pConfig->SaveFile();
        }
    }
    return NULL;
}

#if defined (__IS_SUPPORT_PTZ_BUILTIN__) && defined (__IS_SUPPORT_PTZ_BUILTIN_HUMAN_TRACK__)
static void *moto_check(void *param)
{
    PRT_TID();
    pthread_detach(pthread_self());

    int nval = 0;
    int nSpeed = 6;
    int status = 0;
    int timeout = 300; //默认超时时间
    CConfig *pcfg = NULL;
    CPtzManager *pMgr = NULL;

    pcfg = CConfig::GetInstance();
    pMgr = CPtzManager::PTZ_GetInstance();

    if ((NULL == pMgr) || (NULL == pcfg))
    {
        return NULL;
    }

    /* 1、等待云台自检完成 */
    while (timeout)
    {
        pMgr->PTZ_GetStartStatus(&status);
        // 没有电机或电机自检和回到初始位置完成
        if (1 == status)
        {
            break;
        }
        SelectSleepSec(1);
        timeout--;
    }

    /* 2、恢复到配置文件中的速度值 */
    nSpeed = MPU_PTZ_GetSpeed(0);
    MPU_PTZ_SetSpeed(nSpeed, 0);

    /* 3、依据掉电前配置开始巡航 */
    nval = pcfg->GetValue("PTZ_Advance", "PTZ_MotorCruiseEnable", 0);
    if (0 != nval)
    {
        pMgr->PTZ_StartMotorCruise(0);
    }

    return NULL;
}
#endif

static void * SystemStartOverThread(void *lParam)
{
    PRT_TID();
    pthread_detach(pthread_self());

    int IRCutCheckSucFlag = 0;

    while(1)
    {
        if(0 == ZWSys_GetIrcutCheckStatus(&IRCutCheckSucFlag))
        {
            if(IRCutCheckSucFlag)
            {
                SetSystemStartOverFlag(1);
                break;
            }
        }
        sleep(1);
    }
    return NULL;
}

//设备启动后线程监测
int SystemStartThreadManager()
{
    pthread_t threadId_closewait;
    pthread_t threadId_port;
    pthread_t threadId_onvifcheck;
    pthread_t threadId_jffs2Err;
    pthread_t threadId_motocheck;
    pthread_t threadId_SystemStartOver;
    pthread_t threadId_BlueToothReqRet;
    pthread_t threadId_BlueToothConfigureNet;

    if (pthread_create(&threadId_closewait, NULL, common_reboot, NULL) != 0)
    {
        MAIN_LOG_I("########### create threadId_closewait pthread fail ###########\n");
    }

    if (pthread_create(&threadId_port, NULL, port_check, NULL) != 0)
    {
        MAIN_LOG_I("########### create threadId_closewait pthread fail ###########\n");
    }

    if (pthread_create(&threadId_onvifcheck, NULL, onvifDiscoveryMode_check, NULL) != 0)
    {
        MAIN_LOG_I("########### create threadId_closewait pthread fail ###########\n");
    }

    if (pthread_create(&threadId_jffs2Err, NULL, jffs2_check, NULL) != 0)
    {
        MAIN_LOG_I("########### create jffs2_check pthread fail ###########\n");
    }

#if defined (__IS_SUPPORT_PTZ_BUILTIN__) && defined (__IS_SUPPORT_PTZ_BUILTIN_HUMAN_TRACK__)
    if (pthread_create(&threadId_motocheck, NULL, moto_check, NULL) != 0)
    {
        MAIN_LOG_I("########### create moto_check pthread fail ###########\n");
    }
#endif

    if (pthread_create(&threadId_SystemStartOver, NULL, SystemStartOverThread, NULL) != 0)
    {
        MAIN_LOG_I("########### create SystemStartOverThread pthread fail ###########\n");
    }
	
#ifdef __IS_SUPPORT_AIC_BLUETOOTH__
	if(1 == MPU_Net_GetWiFiUSBWifiDev()|| 4 == MPU_Net_GetWiFiUSBWifiDev())
	{
		if (pthread_create(&threadId_BlueToothReqRet, NULL, Fnx_BlueToothReqRetThread, NULL) != 0)
        {
            printf("########### create SystemStartOverThread pthread fail ###########\n");
        }

        if (pthread_create(&threadId_BlueToothConfigureNet, NULL, Fnx_BlueToothConfigureNetThread, NULL) != 0)
        {
            printf("########### create SystemStartOverThread pthread fail ###########\n");
        }
	}
#endif

    return 0;
}

static char head_buf[] = "\
<?xml version=\"1.0\"?>\n\
<root xmlns=\"urn:schemas-upnp-org:device-1-0\">\n\
\t<specVersion>\n\
\t\t<major>1</major>\n\
\t\t<minor>0</minor>\n\
\t</specVersion>\n\
\t<device>\n\
\t\t<deviceType>urn:schemas-upnp-org:device:DigitalSecurityCamera:1</deviceType>\n\
\t\t<friendlyName>";

static char Serverlist[] = "\
<serviceList>\n\
\t<service>\n\
\t\t<serviceType>urn:schemas-upnp-org:service:ipccontrol:1</serviceType>\n\
\t\t<serviceId>urn:upnp-org:serviceId:ipccontrol1</serviceId>\n\
\t\t<controlURL>/upnp/control/tvcontrol1</controlURL>\n\
\t\t<eventSubURL>/upnp/event/tvcontrol1</eventSubURL>\n\
\t\t<SCPDURL>/ipccontrolSCPD.xml</SCPDURL>\n\
\t</service>\n\
</serviceList>";

static int WriteXml(const char *file_path, const char *modeNum, const char *serialNum, const char *softwareVersion)
{
    int fd = -1;
    int	 s32Ret = 0;
    char value[64];
    char buf[2048];
    char buf_tmp[256];
    char manufac[128];

    memset(buf,  0, sizeof(buf));
    memset(buf_tmp,  0, sizeof(buf_tmp));
    memset(manufac,  0, sizeof(manufac));

    /* 获取制造商 */
    memset(value, 0, sizeof(value));
    s32Ret = GetIniSectionItem(CUSTOM_CONFIG, "CGI", "Manufacturer", value, sizeof(value));
    if (s32Ret==0)
    {
        value[strlen(value)-1] = '\0';
        snprintf(manufac, sizeof(manufac), "%s", value);
    }else{
        snprintf(manufac, sizeof(manufac), "ONVIF_IPNC");
    }

    strncpy(buf, head_buf, strlen(head_buf));
    snprintf(buf_tmp, sizeof(buf_tmp), "%s%s</friendlyName>\n", modeNum, serialNum);
    strncat(buf, buf_tmp, strlen(buf_tmp));
    memset(buf_tmp,  0, sizeof(buf_tmp));

    snprintf(buf_tmp, sizeof(buf_tmp), "\t\t<manufacturer>%s</manufacturer>\n", manufac);
    strncat(buf, buf_tmp, strlen(buf_tmp));
    memset(buf_tmp,  0, sizeof(buf_tmp));
//  snprintf(buf_tmp, sizeof(buf_tmp), "\t\t<manufacturerURL>http://www.zwcloud.wang/</manufacturerURL>\n");
    snprintf(buf_tmp, sizeof(buf_tmp), "\t\t<manufacturerURL></manufacturerURL>\n");
    strncat(buf, buf_tmp, strlen(buf_tmp));
    memset(buf_tmp,  0, sizeof(buf_tmp));
    snprintf(buf_tmp, sizeof(buf_tmp), "\t\t<modelDescription>IP Camera</modelDescription>\n");
    strncat(buf, buf_tmp, strlen(buf_tmp));
    memset(buf_tmp,  0, sizeof(buf_tmp));
    snprintf(buf_tmp, sizeof(buf_tmp), "\t\t<modelName>%s</modelName>\n", modeNum);
    strncat(buf, buf_tmp, strlen(buf_tmp));
    memset(buf_tmp,  0, sizeof(buf_tmp));
    snprintf(buf_tmp, sizeof(buf_tmp), "\t\t<modelNumber>%s</modelNumber>\n", softwareVersion);
    strncat(buf, buf_tmp, strlen(buf_tmp));
    memset(buf_tmp,  0, sizeof(buf_tmp));
//	snprintf(buf_tmp, sizeof(buf_tmp), "\t\t<modelURL>http://www.zwcloud.wang/</modelURL>\n");
    snprintf(buf_tmp, sizeof(buf_tmp), "\t\t<modelURL></modelURL>\n");
    strncat(buf, buf_tmp, strlen(buf_tmp));
    memset(buf_tmp,  0, sizeof(buf_tmp));
    snprintf(buf_tmp, sizeof(buf_tmp), "\t\t<serialNumber>%s</serialNumber>\n", serialNum);
    strncat(buf, buf_tmp, strlen(buf_tmp));
    memset(buf_tmp,  0, sizeof(buf_tmp));
    snprintf(buf_tmp, sizeof(buf_tmp), "\t\t<UDN>uuid:Upnp-%s%s</UDN>\n", modeNum, serialNum);
    strncat(buf, buf_tmp, strlen(buf_tmp));
    strncat(buf, Serverlist, strlen(Serverlist));
    memset(buf_tmp,  0, sizeof(buf_tmp));
    snprintf(buf_tmp, sizeof(buf_tmp), "\t\t<presentationURL>/ipc.html</presentationURL>\n");
    strncat(buf, buf_tmp, strlen(buf_tmp));
    memset(buf_tmp,  0, sizeof(buf_tmp));
    snprintf(buf_tmp, sizeof(buf_tmp), "\t</device>\n");
    strncat(buf, buf_tmp, strlen(buf_tmp));
    memset(buf_tmp,  0, sizeof(buf_tmp));
    snprintf(buf_tmp, sizeof(buf_tmp), "</root>\n");
    strncat(buf, buf_tmp, strlen(buf_tmp));

    fd = open(file_path, O_RDWR|O_CREAT|O_TRUNC, 0777);
    if (-1 == fd)
    {
        perror("open fail!");
        return -1;
    }

    flock(fd, LOCK_EX); //文件加锁
    write(fd, buf, strlen(buf));
    flock(fd, LOCK_UN); //释放文件锁

    close(fd);

    return 0;
}

static int FullWithTheXMLFile(void)
{
    int s32Ret = 0;
    int nBoardID;
    unsigned char szSerialNum[32] = {0};
    char modeNum[30] = {0};
    char cmd_buf[256] = {0};

    char ip[20] = {0};
    char netmask[20] = {0};
    char gateway[20] = {0};

    char value[64] = {0};

    int Major_version = 0;
    int Minor_version = 0;
    int Reversion = 0;
    char SoftwareVersion[128];
    char BuildDate[9];

    ZWSys_GetBoardInfo(&nBoardID);
    ZWSys_GetSerialNum(szSerialNum);

    /* 获取CGI相关版型 */
    memset(value, 0, sizeof(value));
    s32Ret = GetIniSectionItem(CUSTOM_CONFIG, "CGI", "DeviceModel", value,sizeof(value));

    strncpy(modeNum, PUB_FindBoardTypeInfoById(nBoardID), sizeof(modeNum) - 1);

    if (strlen(value) > 0)
    {
        memset(modeNum, 0, 30);
        value[strlen(value)-1] = '\0';
        strncpy(modeNum, value, sizeof(modeNum) - 1);
    }

    snprintf(cmd_buf, sizeof(cmd_buf), "%s/ipc.xml", UPNP_CFG_PATH);

    //获取新的配置文件读取实例
    CConfig *pCfg = CConfig::NewInstance();

    s32Ret = pCfg->LoadFile(IDVR_VERSION_CONFIG_FILE_PATH);
    if (0 == s32Ret)
    {
        Major_version = pCfg->GetValue("Software", "Major_version", 0);
        Minor_version = pCfg->GetValue("Software", "Minor_version", 0);
        Reversion = pCfg->GetValue("Software", "Reversion", 0);
        pCfg->GetValue("Software", "BuildDate", "", BuildDate, sizeof (BuildDate));
        snprintf(SoftwareVersion, sizeof(SoftwareVersion), "%d.%d.%d-%s", Major_version, Minor_version, Reversion, BuildDate);
    }

    memset(value, 0, sizeof(value));
    s32Ret = GetIniSectionItem(CUSTOM_CONFIG, "CGI", "BuildDate", value, sizeof(value));
    /* 防止出现数组越界而出现段错误 */
    if ((s32Ret==0) && (strlen(value)-1<sizeof(BuildDate)))
    {
        memset(BuildDate, 0, sizeof(BuildDate));
        value[strlen(value)-1] = '\0';
        snprintf(SoftwareVersion, sizeof(SoftwareVersion), "%d.%d.%d-%s", Major_version, Minor_version, Reversion, value);
        MAIN_LOG_I("SoftwareVersion = %s\n", SoftwareVersion);
    }

    s32Ret = pCfg->LoadFile(IDVR_CURRENT_CONFIG_FILE_PATH);
    if (0 == s32Ret)
    {
        pCfg->GetValue("INet_Cfg", "LocalIP", "192.168.1.11", ip, 20);
        pCfg->GetValue("INet_Cfg", "LocalSubnetMask", "255.255.255.0", netmask, 20);
        pCfg->GetValue("INet_Cfg", "LocalGateway", "192.168.1.1", gateway, 20);
    }

    CConfig::ReleaseInstance(pCfg);

    s32Ret = WriteXml(cmd_buf, modeNum, (const char *)szSerialNum, SoftwareVersion);
    if (s32Ret)
    {
        MAIN_LOG_I("fail to called WriteXml, return %d\n", s32Ret);
        return s32Ret;
    }

/*
    Usage:
        ipc -ip ipaddress -port port -desc desc_doc_name -webdir web_dir_path -netmask netmask -gateway gateway -configfilepath config_file_fullpath[/config/netconfig.cfg] -pipepath pipe_fullpath[/tmp/my_fifo]
        ipc -help
    eg:
        ipc -desc ipc.xml -webdir /usercfg/upnp -ip 192.168.1.131 -netmask 255.255.255.0 -gateway 192.168.1.1 -configfilepath /usercfg/upnp/netconfig.cfg -pipepath /tmp/my_fifo &
*/
    memset(cmd_buf, '\0', sizeof(cmd_buf));
    snprintf(cmd_buf, sizeof(cmd_buf), "%s -desc ipc.xml -webdir %s -ip %s -netmask %s -gateway %s -configfilepath %s/netconfig.cfg -pipepath /tmp/my_fifo &",
                      CGI_BIN, UPNP_CFG_PATH, ip, netmask, gateway, UPNP_CFG_PATH);
    MAIN_LOG_I("%s is running!\n", cmd_buf);
    mysystem(cmd_buf);

    return s32Ret;
}

//记录MD5值   将shell的结果进行互换位置
static int recordMD5Values(const char *inputFile, const char *md5RecordFile)
{
    char command[256] = {0};
    memset(command, 0, sizeof(command));
    snprintf(command, sizeof(command), "echo -e  `md5sum %s` >> %s",inputFile, md5RecordFile);
    mysystem(command);

    return 0 ;
}

/**
********************************************************************************
 * @函数功能: 更新定制相关的文件的MD5值到配置
 * @作者    : HL.LIU
 * @返回值  :
 * @其他    :
********************************************************************************
*/
static int updateCustomFileMD5()
{
    char command[256] = {0};
    memset(command, 0, sizeof(command));
    snprintf(command, sizeof(command), "rm %s", CFGMD5_PATH);
    mysystem(command);
    recordMD5Values(CUSTOM_MODE_FILE, CFGMD5_PATH);
    recordMD5Values(CUSTOM_READONLY_CONFIG_FILE, CFGMD5_PATH);
    recordMD5Values(CUSTOM_CONFIGMODE_FILE, CFGMD5_PATH);
    recordMD5Values(CUSTOM_ABILITY_FILE, CFGMD5_PATH);
    return 0;
}

/* 在含有属性的节点处干预解析格式，防止出现属性换行 */
const char *whitespace_cb(mxml_node_t *node, int where)
{
    const char *node_name = NULL;
    node_name = mxmlGetElement(node);

    if ((0 == strcmp(node_name, "Channel")) || (0 == strcmp(node_name, "ImageMode")))
    {
        if (MXML_WS_BEFORE_OPEN == where)
        {
            return "\n";
        }
        else if (MXML_WS_AFTER_OPEN == where)
        {
            return "\n";
        }
        else if (MXML_WS_AFTER_OPEN == where)
        {
            return "\n";
        }
        else if (MXML_WS_AFTER_CLOSE == where)
        {
            return "\n";
        }
    }
    return NULL;
}

static int GenDevCapByCustom(int nMaxSensorCnt)
{
    int s32Ret = 0;

    int bSupportFaceNoExposure = 0;
    int bSupportLicensePlate = 0;
    //灯板旧能力
    int bSupportColorNight = 0;             //全彩白
    int bSupportIntelligentNight = 0;       //双光
    int bSupportVariableInfrared = 0;       //无极红
    int bSupportVariableWhite = 0;          //无极白
    int bSupportCloudUpgrade = 0;           //云升级
    int bSupportCloudUpgradeValid = 0;
    int bSupportCloudStorage = 0;           //云存储
    int bSupportCloudStorageValid = 0;
    int bSupportStorageCapacity = 0;        //SD卡容量
    int bSupportStorageCapacityVaild = 0;
    int bSupportStoragePercent = 0;         //SD卡存储百分比
    int bSupportStoragePercentVaild = 0;
    int bSupportSplitCh1To2 = 0;            //假3/4目能力
    int bSupportSplitCh1To2Valid = 0;
    int bSupportSplitCh12To2 = 0;           //假5目能力
    int bSupportSplitCh12To2Valid = 0;
    int bSupportSplitCh0123To2 = 0;         //假8目能力
    int bSupportSplitCh0123To2Valid = 0;
    int bSupportDigitZoom = 0;              //电子放大
    int bSupportDigitZoomVaild = 0;
    int bDeviceShared = 0;                  //支持分享
    int bDeviceSharedValid = 0;
    int bSupportMotor = 0;                  //马达
    int bSupportWired= 0;                   //有线
    int bSupportWiredAlarm = 0;
    int bSupportSerialTransparent = 0;
    int bSupportAlarmOut = 0;               //报警输出
    int bSupportAlarmOutVaild = 0;
    int bSupportNightLedEnable = 0; 		//支持小夜灯功能
    int bSupportKeyToCall = 0;				//一键呼叫
    int bSupportKeyToCallVaild = 0;
    #ifdef __IS_SUPPORT_LONG_SHORT_FOCUS_ZOOM__
    int bZoomCombination = 0;				//变倍镜头组合
    #endif
    int isHidePreviewPeopleTrack = 0; //是否隐藏预览人形跟踪
	int isHidePreviewPeopleTrackVaild = 0; 

    mxml_node_t *tree = NULL;
    mxml_node_t *node = NULL;
    mxml_node_t *node_channel = NULL;
    mxml_node_t *node_dev_funclist = NULL;
    mxml_node_t *node_funclist = NULL;
    mxml_node_t *node_appUI = NULL;
    mxml_node_t *node_appPUSH = NULL;
    mxml_node_t *node_IrCutMode = NULL;
    mxml_node_t *node_ImageMode = NULL;
    mxml_node_t *node_motor = NULL;
    mxml_node_t *node_SmartEvent = NULL;
    mxml_node_t *node_PeopleTrack = NULL;
    LightBoardType eLightBoardType = LightBoardType_DoubleSoftLightSensitive;

    FILE *pFile = 0;
    const char *value = NULL;

    //获取新的配置文件读取实例
    CConfig *pCfg = CConfig::NewInstance();

    s32Ret = pCfg->LoadFileRO(CUSTOM_ABILITY_FILE);
    if (0 == s32Ret)
    {
#if defined (__IS_SUPPORT_PTZ_BUILTIN__) && defined (__IS_SUPPORT_PTZ_BUILTIN_HUMAN_TRACK__)
        if (pCfg->CheckKeyExist("FunctionSupport", "IsSupportMotor"))
        {
            bSupportMotor = pCfg->GetValue("FunctionSupport", "IsSupportMotor", 1);
        }
#endif

        if (pCfg->CheckKeyExist("FunctionSupport", "IsSupportWired"))
        {
            bSupportWired = pCfg->GetValue("FunctionSupport", "IsSupportWired", 0);
#if defined (__PRODUCT_P3MU__)
            bSupportWired = 0; //P3MU强制上报有线能力为false
#endif
        }

        if (pCfg->CheckKeyExist("FunctionSupport", "IsSupportWiredAlarm"))
        {
            bSupportWiredAlarm = pCfg->GetValue("FunctionSupport", "IsSupportWiredAlarm", 0);
        }

        if (pCfg->CheckKeyExist("FunctionSupport", "IsSupportSerialTransparent"))
        {
            bSupportSerialTransparent = pCfg->GetValue("FunctionSupport", "IsSupportSerialTransparent", 0);
        }

#if defined (__IS_SUPPORT_APP_SHARE__)
        if (pCfg->CheckKeyExist("FunctionSupport", "IsSupportDevShared"))
        {
            bDeviceSharedValid = 1;
            bDeviceShared = pCfg->GetValue("FunctionSupport", "IsSupportDevShared", bDeviceShared);
        }
#endif

        if (pCfg->CheckKeyExist("FunctionSupport", "IsSupportCloudUpgrade"))
        {
            bSupportCloudUpgradeValid = 1;
            bSupportCloudUpgrade = pCfg->GetValue("FunctionSupport", "IsSupportCloudUpgrade", bSupportCloudUpgrade);
        }

        if (pCfg->CheckKeyExist("FunctionSupport", "IsSupportCloudStorage"))
        {
            bSupportCloudStorageValid = 1;
            bSupportCloudStorage = pCfg->GetValue("FunctionSupport", "IsSupportCloudStorage", bSupportCloudStorage);
        }

        if (pCfg->CheckKeyExist("UI", "IsSupportStorageCapacity"))
        {
            bSupportStorageCapacityVaild = 1;
            bSupportStorageCapacity = pCfg->GetValue("UI", "IsSupportStorageCapacity", bSupportStorageCapacity);
        }

        if (pCfg->CheckKeyExist("UI", "IsSupportStoragePercent"))
        {
            bSupportStoragePercentVaild = 1;
            bSupportStoragePercent = pCfg->GetValue("UI", "IsSupportStoragePercent", bSupportStoragePercent);
        }

        if (pCfg->CheckKeyExist("UI", "IsSupportSplitCh1To2"))
        {
            bSupportSplitCh1To2Valid = 1;

            /* IsSupportSplitCh1To2代表假3/4目能力，此项目代码中表示真3目变假4目能力，真四目设备需忽略该能力 */
            bSupportSplitCh1To2 = pCfg->GetValue("UI", "IsSupportSplitCh1To2", bSupportSplitCh1To2);
        }

        if (pCfg->CheckKeyExist("UI", "IsSupportSplitCh12To2"))
        {
            bSupportSplitCh12To2Valid = 1;

            /* IsSupportSplitCh12To2代表假5目能力，此项目代码中表示真3目变假5目能力，真四目设备需忽略该能力 */
            if (nMaxSensorCnt == 3)
            {
                bSupportSplitCh12To2 = pCfg->GetValue("UI", "IsSupportSplitCh12To2", bSupportSplitCh12To2);
            }

            /* 真三目设备，假四目和假五目能力互斥，定制成假五目则强制关闭假四目能力 */
            if (bSupportSplitCh12To2 == 1)
            {
                bSupportSplitCh1To2 = 0;
            }
        }

        if (pCfg->CheckKeyExist("UI", "IsSupportSplitCh0123To2"))
        {
            bSupportSplitCh0123To2Valid = 1;

            /* bSupportSplitCh0123To2代表假8目能力，真三目设备需忽略该能力 */
            if (nMaxSensorCnt == 4)
            {
                bSupportSplitCh0123To2 = pCfg->GetValue("UI", "IsSupportSplitCh0123To2", bSupportSplitCh0123To2);
            }
        }

		if (pCfg->CheckKeyExist("UI", "IsSupportHidePreviewPeopleTrack"))
        {
            isHidePreviewPeopleTrackVaild = 1;
			isHidePreviewPeopleTrack = pCfg->GetValue("UI", "IsSupportHidePreviewPeopleTrack", isHidePreviewPeopleTrack);
        }

        if (pCfg->CheckKeyExist("FunctionSupport", "IsSupportDigitZoom"))
        {
            bSupportDigitZoomVaild = 1;
            bSupportDigitZoom = pCfg->GetValue("FunctionSupport", "IsSupportDigitZoom", bSupportDigitZoom);
        }

        if (pCfg->CheckKeyExist("FunctionSupport", "IsSupportAlarmOut"))
        {
            bSupportAlarmOutVaild = 1;
            bSupportAlarmOut = pCfg->GetValue("FunctionSupport", "IsSupportAlarmOut", bSupportAlarmOut);
        }

        if (pCfg->CheckKeyExist("FunctionSupport", "IsSupportKeyToCall"))
        {
            bSupportKeyToCallVaild = 1;
            bSupportKeyToCall = pCfg->GetValue("FunctionSupport", "IsSupportKeyToCall", bSupportKeyToCall);
        }

        //报警输出和一件呼叫不会同时存在，一键呼叫如果打开则报警输出无效
        if	(bSupportKeyToCallVaild == 1 && bSupportKeyToCall == 1)
        {
            bSupportAlarmOutVaild = 1;
            bSupportAlarmOut = 0;
        }
        else
        {
            if (pCfg->CheckKeyExist("FunctionSupport", "IsSupportAlarmOut"))
            {
                bSupportAlarmOutVaild = 1;
                bSupportAlarmOut = pCfg->GetValue("FunctionSupport", "IsSupportAlarmOut", bSupportAlarmOut);
            }
        }
    }
    CConfig::ReleaseInstance(pCfg);
    pCfg = NULL;

    MAIN_LOG_I("Support ability [%d] [%d-%d] [%d-%d] [%d-%d] [%d-%d] [%d-%d] [%d-%d] [%d-%d] [%d-%d] [%d-%d] [%d-%d] [%d-%d] [%d-%d] [%d-%d]\n",
                    nMaxSensorCnt,
                    bSupportMotor, bSupportWired,
                    bDeviceSharedValid, bDeviceShared,
                    bSupportCloudUpgradeValid, bSupportCloudUpgrade,
                    bSupportCloudStorageValid, bSupportCloudStorage,
                    bSupportStorageCapacityVaild, bSupportStorageCapacity,
                    bSupportStoragePercentVaild, bSupportStoragePercent,
                    bSupportSplitCh1To2Valid, bSupportSplitCh1To2,
                    bSupportSplitCh12To2Valid, bSupportSplitCh12To2,
                    bSupportSplitCh0123To2Valid, bSupportSplitCh0123To2,
                    bSupportDigitZoomVaild,bSupportDigitZoom,
                    bSupportAlarmOutVaild,bSupportAlarmOut,
                    bSupportKeyToCallVaild,bSupportKeyToCall,
                    isHidePreviewPeopleTrackVaild,isHidePreviewPeopleTrack);

    pCfg = CConfig::NewInstance();
    //夜视模式能力根据灯板类型来确定
    s32Ret = pCfg->LoadFileRO(CUSTOM_CONFIG_FILE);
    if (0 == s32Ret)
    {
        eLightBoardType = (LightBoardType)pCfg->GetValue("SystemFunction", "LightBoardType", 0);

        PUB_LightBoard_Capbility sLBCap = {};
        int ret_getcap = PUB_LightBoard_GetCapbility (eLightBoardType, & sLBCap);
        if (ret_getcap < 0)
        {
            MAIN_LOG_E("get cap failed, e=%d\n", eLightBoardType);
        }

        bSupportColorNight = sLBCap.bSupportColorNight;
        bSupportIntelligentNight = sLBCap.bSupportIntelligentNight;
        bSupportVariableInfrared = sLBCap.bSupportVariableInfrared;
        bSupportVariableWhite = sLBCap.bSupportVariableWhite;
        bSupportFaceNoExposure = sLBCap.bSupportFaceNoExposure;
        bSupportLicensePlate = sLBCap.bSupportLicensePlate;
    }
#ifdef __IS_SUPPORT_LONG_SHORT_FOCUS_ZOOM__
    //变倍放大功能，拖动条倍数显示根据镜头组合来 4+6mm最大12X 其他默认18X
    if (0 == s32Ret)
    {
        bZoomCombination = (LightBoardType)pCfg->GetValue("ZoomLensCfg", "ZoomLensCombination", 0);
    }
#endif
    CConfig::ReleaseInstance(pCfg);
    pCfg = NULL;

    /* 支持双光源默认支持夜灯 */
    if (bSupportIntelligentNight)
    {
        bSupportNightLedEnable = 1;
    }


    pFile = fopen(CUSTOM_DEVICE_CAP_FILE, "r");
    if (NULL == pFile)
    {
        fprintf(stderr, "open the %s error: %s!\n", CUSTOM_DEVICE_CAP_FILE, strerror(errno));
        return -1;
    }
    tree = mxmlLoadFile(NULL, pFile, MXML_TEXT_CALLBACK);
    fclose(pFile);
    pFile = NULL;

    if (NULL == tree)
    {
        MAIN_LOG_E("Load file %s error!\n", CUSTOM_DEVICE_CAP_FILE);
        return -1;
    }

    node = mxmlFindElement(tree, tree, "Device", NULL, NULL, MXML_DESCEND);
    if (NULL == node)
    {
        MAIN_LOG_E("can not find element Device!\n");
        mxmlRelease(tree);
        return -1;
    }

    node_funclist = mxmlFindElement(node, tree, "FunctionList", NULL, NULL, MXML_DESCEND);
    if (NULL == node_funclist)
    {
        MAIN_LOG_E("can not find element FunctionList!\n");
        mxmlRelease(tree);
        return -1;
    }
    node_appPUSH = mxmlFindElement(node_funclist, tree, "Push", NULL, NULL, MXML_DESCEND);
    if (NULL == node_appPUSH)
    {
        MAIN_LOG_E("can not find element AppUI!\n");
        mxmlRelease(tree);
        return -1;
    }
    else
    {
        /* 一键呼叫。以下三个能力和一键呼叫是配套的 */
        if (bSupportKeyToCallVaild)
        {
            mxml_node_t* node_KeyToCall = mxmlFindElement (node_appPUSH, tree, "Key2Call", NULL, NULL, MXML_DESCEND);
            if (NULL == node_KeyToCall)
            {
                MAIN_LOG_E("can not find element Key2Call\n");
            }
            else
            {
                mxml_node_t* node_KeyToCall_support = mxmlFindElement (node_KeyToCall, tree, "Support", NULL, NULL, MXML_DESCEND);
                if (NULL == node_KeyToCall_support)
                {
                    MAIN_LOG_E("can not find element DigitZoom-Support\n");
                }
                else
                {
                    value = mxmlGetText (node_KeyToCall_support, NULL);
                    if (NULL == value)
                    {
                        MAIN_LOG_E("get Key2Call error\n");
                    }
                    else
                    {
                        /* IsSupportKeyToCall=1 一键呼叫 */
                        mxmlSetText(node_KeyToCall_support, 0, ((bSupportKeyToCall) ? "true" : "false"));
                    }
                }
            }

            mxml_node_t* node_StopKeyToCall = mxmlFindElement (node_appPUSH, tree, "StopKey2Call", NULL, NULL, MXML_DESCEND);
            if (NULL == node_StopKeyToCall)
            {
                MAIN_LOG_E("can not find element StopKey2Call\n");
            }
            else
            {
                mxml_node_t* node_StopKeyToCall_support = mxmlFindElement (node_StopKeyToCall, tree, "Support", NULL, NULL, MXML_DESCEND);
                if (NULL == node_StopKeyToCall_support)
                {
                    MAIN_LOG_E("can not find element StopKeyToCall-Support\n");
                }
                else
                {
                    value = mxmlGetText (node_StopKeyToCall_support, NULL);
                    if (NULL == value)
                    {
                        MAIN_LOG_E("get StopKey2Call error\n");
                    }
                    else
                    {
                        /* IsSupportKeyToCall=1 一键呼叫 */
                        mxmlSetText(node_StopKeyToCall_support, 0, ((bSupportKeyToCall) ? "true" : "false"));
                    }
                }
            }

            mxml_node_t* node_HighPriority = mxmlFindElement (node_appPUSH, tree, "HighPriority", NULL, NULL, MXML_DESCEND);
            if (NULL == node_HighPriority)
            {
                MAIN_LOG_E("can not find element HighPriority\n");
            }
            else
            {
                mxml_node_t* node_HighPriority_support = mxmlFindElement (node_HighPriority, tree, "Support", NULL, NULL, MXML_DESCEND);
                if (NULL == node_HighPriority_support)
                {
                    MAIN_LOG_E("can not find element HighPriority-Support\n");
                }
                else
                {
                    value = mxmlGetText (node_HighPriority_support, NULL);
                    if (NULL == value)
                    {
                        MAIN_LOG_E("get HighPriority error\n");
                    }
                    else
                    {
                    /* IsSupportKeyToCall=1 一键呼叫 */
                        mxmlSetText(node_HighPriority_support, 0, ((bSupportKeyToCall) ? "true" : "false"));
                    }
                }

            }
        }
    }

    node_appUI = mxmlFindElement(node_funclist, tree, "AppUI", NULL, NULL, MXML_DESCEND);
    if (NULL == node_appUI)
    {
        MAIN_LOG_E("can not find element AppUI!\n");
        mxmlRelease(tree);
        return -1;
    }
    else
    {
        /* SD卡容量显示 */
        if (bSupportStorageCapacityVaild)
        {
            mxml_node_t* node_storage_support = mxmlFindElement (node_appUI, tree, "StorageCapacity", NULL, NULL, MXML_DESCEND);
            if (NULL == node_storage_support)
            {
                MAIN_LOG_E("can not find element StorageCapacity\n");
            }
            else
            {
                value = mxmlGetText (node_storage_support, NULL);
                if (NULL == value)
                {
                    MAIN_LOG_E("get StorageCapacity error\n");
                }
                else
                {
                    mxmlSetText(node_storage_support, 0, (bSupportStorageCapacity) ? "true" : "false");
                }
            }
        }

        /* SD卡存储占用百分比显示 */
        if (bSupportStoragePercentVaild)
        {
            mxml_node_t* node_storage_percent_support = mxmlFindElement (node_appUI, tree, "StoragePercent", NULL, NULL, MXML_DESCEND);
            if (NULL == node_storage_percent_support)
            {
                MAIN_LOG_E("can not find element StoragePercent\n");
            }
            else
            {
                value = mxmlGetText (node_storage_percent_support, NULL);
                if (NULL == value)
                {
                    MAIN_LOG_E("get StoragePercent error\n");
                }
                else
                {
                    mxmlSetText(node_storage_percent_support, 0, (bSupportStoragePercent) ? "true" : "false");
                }
            }
        }

        /* UI 电子放大 */
        if (bSupportDigitZoomVaild)
        {
            mxml_node_t* node_DigitZoom = mxmlFindElement (node_appUI, tree, "DigitZoom", NULL, NULL, MXML_DESCEND);
            if (NULL == node_DigitZoom)
            {
                MAIN_LOG_E("can not find element DigitZoom\n");
            }
            else
            {
                mxml_node_t* node_DigitZoom_support = mxmlFindElement (node_DigitZoom, tree, "Support", NULL, NULL, MXML_DESCEND);
                if (NULL == node_DigitZoom_support)
                {
                    MAIN_LOG_E("can not find element DigitZoom-Support\n");
                }
                else
                {
                    value = mxmlGetText (node_DigitZoom_support, NULL);
                    if (NULL == value)
                    {
                        MAIN_LOG_E("get DigitZoom-Support error\n");
                    }
                    else
                    {
                        mxmlSetText(node_DigitZoom_support, 0, (bSupportDigitZoom) ? "true" : "false");
                    }
                }
            }
        }

#ifdef __IS_SUPPORT_LONG_SHORT_FOCUS_ZOOM__

        if (bZoomCombination == 2)
        {
            mxml_node_t* node_DigitZoom = mxmlFindElement (node_appUI, tree, "ZoomScaleList", NULL, NULL, MXML_DESCEND);
            if (NULL == node_DigitZoom)
            {
                MAIN_LOG_E("can not find element DigitZoom\n");
            }
            else
            {
                mxml_node_t* node_DigitZoom_support = mxmlFindElement (node_DigitZoom, tree, "ScaleList", NULL, NULL, MXML_DESCEND);
                if (NULL == node_DigitZoom_support)
                {
                    MAIN_LOG_E("can not find element DigitZoom-Support\n");
                }
                else
                {
                    value = mxmlGetText (node_DigitZoom_support, NULL);
                    if (NULL == value)
                    {
                        MAIN_LOG_E("get DigitZoom-Support error\n");
                    }
                    else
                    {
                        mxmlSetText(node_DigitZoom_support, 0,"1,2,3,4,5,6,7,8,9,10,11,12");
                    }
                }
            }
        }

#endif

        /* 报警输出界面版本 */
        if (bSupportAlarmOutVaild)
        {
            mxml_node_t* node_AlarmOutVersion = mxmlFindElement (node_appUI, tree, "AlarmOutVersion", NULL, NULL, MXML_DESCEND);
            if (NULL == node_AlarmOutVersion)
            {
                MAIN_LOG_E("can not find element AlarmOutVersion\n");
            }
            else
            {
                value = mxmlGetText (node_AlarmOutVersion, NULL);
                if (NULL == value)
                {
                    MAIN_LOG_E("get AlarmOutVersion error\n");
                }
                else
                {
                    /* AlarmOutVersion=1 红蓝指示灯界面 */
                    mxmlSetText(node_AlarmOutVersion, 0, ((bSupportAlarmOut) ? "1" : "0"));
                }
            }
        }

        /* 多目APP界面显示版本 */
        {
            mxml_node_t* node_PreviewVersion = mxmlFindElement (node_appUI, tree, "PreviewVersion", NULL, NULL, MXML_DESCEND);
            if (NULL == node_PreviewVersion)
            {
                MAIN_LOG_E("can not find element PreviewVersion\n");
            }
            else
            {
                value = mxmlGetText (node_PreviewVersion, NULL);
                if (NULL == value)
                {
                    MAIN_LOG_E("get PreviewVersion error\n");
                }
                else
                {
                    mxmlSetText(node_PreviewVersion, 0, ((nMaxSensorCnt == 4) ? "2" : "0"));
                }
            }

            mxml_node_t* node_PlaybackVersion = mxmlFindElement (node_appUI, tree, "PlaybackVersion", NULL, NULL, MXML_DESCEND);
            if (NULL == node_PlaybackVersion)
            {
                MAIN_LOG_E("can not find element PlaybackVersion\n");
            }
            else
            {
                value = mxmlGetText (node_PlaybackVersion, NULL);
                if (NULL == value)
                {
                    MAIN_LOG_E("get PlaybackVersion error\n");
                }
                else
                {
                    mxmlSetText(node_PlaybackVersion, 0, ((nMaxSensorCnt == 4) ? "2" : "0"));
                }
            }
        }

        /* 预览人形跟踪 */
        if (isHidePreviewPeopleTrackVaild)
        {
            mxml_node_t* node_PeopleTrack = mxmlFindElement (node_appUI, tree, "HidePreviewPeopleTrack", NULL, NULL, MXML_DESCEND);
            if (NULL == node_PeopleTrack)
            {
                MAIN_LOG_E("can not find element HidePreviewPeopleTrack\n");
            }
            else
            {
                value = mxmlGetText (node_PeopleTrack, NULL);
                if (NULL == value)
                {
                    MAIN_LOG_E("get HidePreviewPeopleTrack error\n");
                }
                else
                {
                    mxmlSetText(node_PeopleTrack, 0, (isHidePreviewPeopleTrack) ? "true" : "false");
                }
            }
        }
		else
		{
			mxml_node_t* node_PeopleTrack = mxmlFindElement (node_appUI, tree, "HidePreviewPeopleTrack", NULL, NULL, MXML_DESCEND);
            if (NULL == node_PeopleTrack)
            {
                MAIN_LOG_E("can not find element HidePreviewPeopleTrack\n");
            }
            else
            {
                value = mxmlGetText (node_PeopleTrack, NULL);
                if (NULL == value)
                {
                    MAIN_LOG_E("get HidePreviewPeopleTrack error\n");
                }
                else
                {
                    mxmlSetText(node_PeopleTrack, 0,  "false");
                }
            }	
		}
    }

#if defined (__IS_SUPPORT_PTZ_BUILTIN__) && defined (__IS_SUPPORT_PTZ_BUILTIN_HUMAN_TRACK__)
    /* 马达 */
    node_motor = mxmlFindElement(node_funclist, tree, "Motor", NULL, NULL, MXML_DESCEND);
    if (node_motor == NULL)
    {
        mxmlRelease(tree);
        return -1;
    }
    else
    {
        value = mxmlGetText(node_motor, NULL);
        if (NULL == value)
        {
            MAIN_LOG_E("get Motor error!\n");
        }
        mxmlSetText(node_motor, 0, (1 == bSupportMotor) ? "true" : "false");
    }
#endif

#if defined (__IS_SUPPORT_APP_SHARE__)
    /* 设备分享：支持定制 */
    if (bDeviceSharedValid)
    {
        mxml_node_t *node_dev_shared = mxmlFindElement (node_funclist, tree, "DeviceShared", NULL, NULL, MXML_DESCEND);
        if (NULL == node_dev_shared)
        {
            MAIN_LOG_E("can not find element DeviceShared\n");
        }
        else
        {
            value = mxmlGetText (node_dev_shared, NULL);
            if (NULL == value)
            {
                MAIN_LOG_E("get DeviceShared error\n");
            }
            else
            {
                mxmlSetText(node_dev_shared, 0, (bDeviceShared) ? "true" : "false");
            }
        }
    }
#endif

    /* 云升级：支持定制 */
    if (bSupportCloudUpgradeValid)
    {
        mxml_node_t* node_cloud_upgrade = mxmlFindElement (node_funclist, tree, "cloud_upgrade", NULL, NULL, MXML_DESCEND);
        if (NULL == node_cloud_upgrade)
        {
            MAIN_LOG_E("can not find element cloud_upgrade\n");
        }
        else
        {
            value = mxmlGetText (node_cloud_upgrade, NULL);
            if (NULL == value)
            {
                MAIN_LOG_E("get cloud_upgrade error\n");
            }
            else
            {
                mxmlSetText(node_cloud_upgrade, 0, (bSupportCloudUpgrade) ? "true" : "false");
            }
        }
    }

    /* 云存储：支持定制 */
    if (bSupportCloudStorageValid)
    {
        mxml_node_t* node_cloud_storage = mxmlFindElement (node_funclist, tree, "CloudStorage", NULL, NULL, MXML_DESCEND);
        if (NULL == node_cloud_storage)
        {
            MAIN_LOG_E("can not find element CloudStorage\n");
        }
        else
        {
            mxml_node_t* node_cloud_storage_support = mxmlFindElement (node_cloud_storage, tree, "Support", NULL, NULL, MXML_DESCEND);
            if (NULL == node_cloud_storage_support)
            {
                MAIN_LOG_E("can not find element CloudStorage-Support\n");
            }
            else
            {
                value = mxmlGetText (node_cloud_storage_support, NULL);
                if (NULL == value)
                {
                    MAIN_LOG_E("get cloud_upgrade-support error\n");
                }
                else
                {
                    mxmlSetText(node_cloud_storage_support, 0, (bSupportCloudStorage) ? "true" : "false");
                }
            }
        }
    }

#if (SYSTEM_SUPPORT_CHANNEL >= 2)
    /* 假多目能力 */
    if (bSupportSplitCh1To2Valid || bSupportSplitCh0123To2Valid)
    {
        mxml_node_t* MultiLens = mxmlFindElement (node_funclist, tree, "MultiLens", NULL, NULL, MXML_DESCEND);
        if (NULL == MultiLens)
        {
            MAIN_LOG_E("can not find element MultiLens\n");
        }
        else
        {
            if (bSupportSplitCh1To2Valid)
            {
            #if (SYSTEM_SUPPORT_CHANNEL == 2)
                /* 假3目能力，此项目中表示假四目设备能力 */
                mxml_node_t* SplitCh1To2 = mxmlFindElement (MultiLens, tree, "SplitCh1To2", NULL, NULL, MXML_DESCEND);
                if (NULL == SplitCh1To2)
                {
                    MAIN_LOG_E("can not find element MultiLens-SplitCh1To2\n");
                }
                else
                {
                    value = mxmlGetText (SplitCh1To2, NULL);
                    if (NULL == value)
                    {
                        MAIN_LOG_E("get MultiLens-SplitCh1To2 error\n");
                    }
                    else
                    {
                        mxmlSetText(SplitCh1To2, 0, (bSupportSplitCh1To2) ? "true" : "false");
                    }
                }
            #else if(SYSTEM_SUPPORT_CHANNEL == 3)
            /* 假4目能力，此项目中表示假四目设备能力 */
            mxml_node_t* SplitCh0To2 = mxmlFindElement (MultiLens, tree, "SplitCh0To2", NULL, NULL, MXML_DESCEND);
            if (NULL == SplitCh0To2)
            {
                MAIN_LOG_E("can not find element MultiLens-SplitCh0To2\n");
            }
            else
            {
                value = mxmlGetText (SplitCh0To2, NULL);
                if (NULL == value)
                {
                    MAIN_LOG_E("get MultiLens-SplitCh0To2 error\n");
                }
                else
                {
                    mxmlSetText(SplitCh0To2, 0, (bSupportSplitCh1To2) ? "true" : "false");
                }
            }
            #endif
            }

            if (bSupportSplitCh12To2Valid)
            {
                /* 假5目能力 */
                mxml_node_t* SplitCh12To2 = mxmlFindElement (MultiLens, tree, "SplitCh12To2", NULL, NULL, MXML_DESCEND);
                if (NULL == SplitCh12To2)
                {
                    MAIN_LOG_E("can not find element MultiLens-SplitCh12To2\n");
                }
                else
                {
                    value = mxmlGetText (SplitCh12To2, NULL);
                    if (NULL == value)
                    {
                        MAIN_LOG_E("get MultiLens-SplitCh12To2 error\n");
                    }
                    else
                    {
                        mxmlSetText(SplitCh12To2, 0, (bSupportSplitCh12To2) ? "true" : "false");
                    }
                }
            }

            if (bSupportSplitCh0123To2Valid)
            {
                /* 假8目能力 */
                mxml_node_t* SplitCh0123To2 = mxmlFindElement (MultiLens, tree, "SplitCh0123To2", NULL, NULL, MXML_DESCEND);
                if (NULL == SplitCh0123To2)
                {
                    MAIN_LOG_E("can not find element MultiLens-SplitCh0123To2\n");
                }
                else
                {
                    value = mxmlGetText (SplitCh0123To2, NULL);
                    if (NULL == value)
                    {
                        MAIN_LOG_E("get MultiLens-SplitCh0123To2 error\n");
                    }
                    else
                    {
                        mxmlSetText(SplitCh0123To2, 0, (bSupportSplitCh0123To2) ? "true" : "false");
                    }
                }
            }
        }
    }
#endif

    /* 一键夜灯能力 */
    mxml_node_t* node_nightled_enable = mxmlFindElement (node_funclist, tree, "NightLedEnable", NULL, NULL, MXML_DESCEND);
    if (NULL == node_nightled_enable)
    {
        printf ("can not find element NightLedEnable\n");
    }
    else
    {
        mxml_node_t* node_nightled_support = mxmlFindElement (node_nightled_enable, tree, "Support", NULL, NULL, MXML_DESCEND);
        if (NULL == node_nightled_support)
        {
            printf ("can not find element NightLedEnable-Support\n");
        }
        else
        {
            value = mxmlGetText (node_nightled_support, NULL);
            if (NULL == value)
            {
                printf ("get NightLedEnable-Support error\n");
            }
            else
            {
                mxmlSetText (node_nightled_support, 0, (bSupportNightLedEnable) ? "true" : "false");
                printf("[%s] bSupportNightLedEnable [%d]\n", __FUNCTION__, bSupportNightLedEnable);
            }
        }
    }

    /* 有线能力 */
    mxml_node_t *node_WiredNetwork = mxmlFindElement(node_funclist, tree, "WiredNetwork", NULL, NULL, MXML_DESCEND);
    if (node_WiredNetwork == NULL)
    {
        MAIN_LOG_E("can not find element WiredNetwork!\n");
    }
    else
    {
        value = mxmlGetText(node_WiredNetwork, NULL);
        if (NULL == value)
        {
            MAIN_LOG_E("get WiredNetwork error!\n");
        }
        else
        {
            mxmlSetText(node_WiredNetwork, 0, (bSupportWired) ? "true" : "false");
        }
    }

    mxml_node_t *node_WiredAlarm = mxmlFindElement(node_funclist, tree, "WiredAlarm", NULL, NULL, MXML_DESCEND);
    if (node_WiredAlarm == NULL)
    {
        MAIN_LOG_E("can not find element WiredAlarm!\n");
    }
    else
    {
        value = mxmlGetText(node_WiredAlarm, NULL);
        if (NULL == value)
        {
            MAIN_LOG_E("get WiredAlarm error!\n");
        }
        else
        {
           mxmlSetText(node_WiredAlarm, 0, (bSupportWiredAlarm) ? "true" : "false");
        }
    }

    mxml_node_t *node_SerialTransparent = mxmlFindElement(node_funclist, tree, "SerialTransparent", NULL, NULL, MXML_DESCEND);
    if (node_SerialTransparent == NULL)
    {
        MAIN_LOG_E("can not find element SerialTransparent!\n");
    }
    else
    {
        value = mxmlGetText(node_SerialTransparent, NULL);
        if(NULL == value)
        {
            MAIN_LOG_E("get SerialTransparent error!\n");
        }
        else
        {
            mxmlSetText(node_SerialTransparent, 0, (bSupportSerialTransparent) ? "true" : "false");
        }
    }

    node = mxmlFindElement(tree, tree, "Device", NULL, NULL, MXML_DESCEND);
    if (NULL == node)
    {
        MAIN_LOG_E("can not find element Device!\n");
        mxmlRelease(tree);
        return -1;
    }

#if (SYSTEM_SUPPORT_CHANNEL >= 3)
    mxml_node_t *node_StreamCount = mxmlFindElement(node, tree, "StreamCount", NULL, NULL, MXML_DESCEND);
    if (node_StreamCount == NULL)
    {
        MAIN_LOG_E("can not find StreamCount\n");
    }
    else
    {
        value = mxmlGetText(node_StreamCount, NULL);
        if (NULL == value)
        {
            MAIN_LOG_E("get StreamCount error!\n");
        }
        mxmlSetText(node_StreamCount, 0, (4 == nMaxSensorCnt) ? "4" : "3");
    }
#endif

    node_dev_funclist = mxmlFindElement(node, tree, "FunctionList", NULL, NULL, MXML_DESCEND);
    if (NULL == node_dev_funclist)
    {
        MAIN_LOG_E("can not find element FunctionList!\n");
        mxmlRelease(tree);
        return -1;
    }

    node = mxmlFindElement(node, tree, "FunctionListAboutChannel", NULL, NULL, MXML_DESCEND);
    if (NULL == node)
    {
        MAIN_LOG_E("can not find element FunctionListAboutChannel!\n");
        mxmlRelease(tree);
        return -1;
    }

    node = mxmlFindElement(node, tree, "ChannelList", NULL, NULL, MXML_DESCEND);
    if (NULL == node)
    {
        MAIN_LOG_E("can not find element ChannelList!\n");
        mxmlRelease(tree);
        return -1;
    }

    node_channel = mxmlFindElement(node, tree, "Channel", NULL, NULL, MXML_DESCEND);
    if (NULL == node_channel)
    {
        MAIN_LOG_E("can not find element Channel!\n");
        mxmlRelease(tree);
        return -1;
    }

    node = mxmlFindElement(node_channel, tree, "ColorNight", NULL, NULL, MXML_DESCEND);
    if (NULL == node)
    {
        MAIN_LOG_E("can not find element ColorNight!\n");
        mxmlRelease(tree);
        return -1;
    }

    value = mxmlGetText(node, NULL);
    if (NULL == value)
    {
        MAIN_LOG_E("get ColorNight error!\n");
        return -1;
    }
    mxmlSetText(node, 0, (1 == bSupportColorNight) ? "true" : "false");

    node = mxmlFindElement(node_channel, tree, "IntelligentNight", NULL, NULL, MXML_DESCEND);
    if (NULL == node)
    {
        MAIN_LOG_E("can not find element IntelligentNight!\n");
        mxmlRelease(tree);
        return -1;
    }

    value = mxmlGetText(node, NULL);
    if (NULL == value)
    {
        MAIN_LOG_E("get IntelligentNight error!\n");
        return -1;
    }
    mxmlSetText(node, 0, (1 == bSupportIntelligentNight) ? "true" : "false");

    node_IrCutMode = mxmlFindElement(node_channel, tree, "IrCutMode", NULL, NULL, MXML_DESCEND);
    if (NULL == node)
    {
        MAIN_LOG_E("can not find element IrCutMode!\n");
        mxmlRelease(tree);
        return -1;
    }

    node = mxmlFindElement(node_IrCutMode, tree, "VariableInfrared", NULL, NULL, MXML_DESCEND);
    if (NULL == node)
    {
        MAIN_LOG_E("can not find element VariableInfrared!\n");
        mxmlRelease(tree);
        return -1;
    }

    value = mxmlGetText(node, NULL);
    if (NULL == value)
    {
        MAIN_LOG_E("get VariableInfrared error!\n");
        return -1;
    }
    mxmlSetText(node, 0, (1 == bSupportVariableInfrared) ? "true" : "false");

    node = mxmlFindElement(node_IrCutMode, tree, "VariableWhite", NULL, NULL, MXML_DESCEND);
    if (NULL == node)
    {
        MAIN_LOG_E("can not find element VariableWhite!\n");
        mxmlRelease(tree);
        return -1;
    }

    value = mxmlGetText(node, NULL);
    if (NULL == value)
    {
        MAIN_LOG_E("get VariableWhite error!\n");
        return -1;
    }
    mxmlSetText(node, 0, (1 == bSupportVariableWhite) ? "true" : "false");

    node_ImageMode = mxmlFindElement(node_channel, tree, "ImageMode", NULL, NULL, MXML_DESCEND);
    if (NULL == node)
    {
        MAIN_LOG_E("can not find element ImageMode!\n");
        mxmlRelease(tree);
        return -1;
    }

    node = mxmlFindElement(node_ImageMode, tree, "FaceNoExposure", NULL, NULL, MXML_DESCEND);
    if (NULL == node)
    {
        MAIN_LOG_E("can not find element FaceNoExposure!\n");
        mxmlRelease(tree);
        return -1;
    }

    value = mxmlGetText(node, NULL);
    if (NULL == value)
    {
        MAIN_LOG_E("get FaceNoExposure error!\n");
        return -1;
    }
    mxmlSetText(node, 0, (1 == bSupportFaceNoExposure) ? "true" : "false");

    node = mxmlFindElement(node_ImageMode, tree, "LicensePlate", NULL, NULL, MXML_DESCEND);
    if (NULL == node)
    {
        MAIN_LOG_E("can not find element LicensePlate!\n");
        mxmlRelease(tree);
        return -1;
    }

    value = mxmlGetText(node, NULL);
    if (NULL == value)
    {
        MAIN_LOG_E("get LicensePlate error!\n");
        return -1;
    }
    mxmlSetText(node, 0, (1 == bSupportLicensePlate) ? "true" : "false");

    node_SmartEvent = mxmlFindElement(node_channel, tree, "SmartEvent", NULL, NULL, MXML_DESCEND);
    if (node_SmartEvent == NULL)
    {
        MAIN_LOG_E("can not find element SmartEvent!\n");
        mxmlDelete(tree);
        return -1;
    }
    else
    {
        node_PeopleTrack = mxmlFindElement(node_SmartEvent, tree, "PeopleTrack", NULL, NULL, MXML_DESCEND);
        if(node_PeopleTrack == NULL)
        {
            MAIN_LOG_E("can not find element PeopleTrack!\n");
        }
        else
        {
            value = mxmlGetText(node_PeopleTrack, NULL);
            if (NULL == value)
            {
                MAIN_LOG_E("get PeopleTrack error!\n");
            }

            if (0 == bSupportMotor) {
                mxmlSetText(node_PeopleTrack, 0, "false");
            }
        }
    }

    pFile = fopen(CUSTOM_DEVICE_CAP_FILE, "w");
    mxmlSaveFile(tree, pFile, whitespace_cb);
    mxmlRelease(tree);

    if (pFile)
    {
        fflush(pFile);
        fclose(pFile);
        pFile = NULL;
    }

    return 0;
}

bool SetCapFromDeviceCapXML_Bool(mxml_node_t *tree, const char *pos, int df)
{
    bool ret = -1;
    char *p = NULL;
    const char *value = NULL;
    char tmp[256] = {0};
    mxml_node_t *node = NULL;
    char *pcSaveptr = NULL;

    if (pos)
    {
        if (tree)
        {
            node = tree;
            snprintf(tmp, sizeof(tmp), "%s", pos);
            p = strtok_r(tmp, ":", &pcSaveptr);
            while (p)
            {
                node = mxmlFindElement(node, tree, p, NULL, NULL, MXML_DESCEND);
                if (NULL == node)
                {
                    break;
                }
                p = strtok_r(NULL, ":", &pcSaveptr);
                if (NULL == p)
                {
                    value = mxmlGetText(node, NULL);
                    if (value)
                    {
                        MAIN_LOG_I("Value[%s]\n", value);
                        if (strcasecmp(value, "true") == 0 || strcasecmp(value, "false") == 0)
                        {
                            if(df == 0)
                            {
                                MAIN_LOG_I("set false\n");
                                ret = mxmlSetText(node, 0, "false");
                            }
                            else if (df == 1)
                            {
                                MAIN_LOG_I("set true\n");
                                   ret = mxmlSetText(node, 0, "true");
                            }
                            //其他值则不去修改原始配置文件
                        }
                    }
                }
            }
        }
    }
    return ret;
}

static int GenDevPTZCapByCustom(unsigned int isDLauthorization)
{
    int i = 0;
    int s32Ret = 0;
    FILE *pFile = 0;
    CConfig *pCfg = NULL;
    mxml_node_t *tree = NULL;
    int bSupportMotor = 1;
    int bSupportIgnoreShakingHead = 1;
    int bSupportUpDown = 1;
    int bSupportLeftRight = 1;
    int bSupportPreset = 1;
    int bSupportCruise = 1;
    int bSupportTrackZoom = 1;
#if defined(__PRODUCT_G84F__) || defined (__PRODUCT_W84F__) || defined (__PRODUCT_G63F__)|| defined (__PRODUCT_W63F__)
    int bSupportMultiPtzDev = 1;
	int bSptPtzLeft = 3;
	int bSptPtzRight = 3;
#endif
    /* 此处注意：后续index值必须小于数组成员个数 */
    int index = 0;
#ifdef __DIGITAL_ZOOM_DRAG_SUPPORT__
    SXT xml_tree_data[7];
#else
    SXT xml_tree_data[6];
#endif
    memset(xml_tree_data, 0, sizeof(xml_tree_data));

    /* 从customability.ini加载定制 */
    pCfg = CConfig::NewInstance();
    s32Ret = pCfg->LoadFileRO (CUSTOM_ABILITY_FILE);
    if (0 == s32Ret)
    {
        if (pCfg->CheckKeyExist ("FunctionSupport", "IsSupportMotor") )
        {
            //是否支持巡航bSupportCruise
            bSupportMotor = pCfg->GetValue ("FunctionSupport", "IsSupportMotor", bSupportMotor);
        }
        if (pCfg->CheckKeyExist ("FunctionSupport", "IsSupportCruise") )
        {
            //是否支持巡航bSupportCruise
            bSupportCruise = pCfg->GetValue ("FunctionSupport", "IsSupportCruise", bSupportCruise);
        }
        if (pCfg->CheckKeyExist ("FunctionSupport", "IsSupportTrackZoom") )
        {
            //是否支持数字放大功能（跟踪放大）
            bSupportTrackZoom = pCfg->GetValue ("FunctionSupport", "IsSupportTrackZoom", bSupportTrackZoom);
        }
#if defined(__PRODUCT_G84F__)|| defined (__PRODUCT_W84F__) || defined (__PRODUCT_G63F__)|| defined (__PRODUCT_W63F__)
		if (pCfg->CheckKeyExist ("FunctionSupport", "IsSupportMultiPtzDev") )
		{
            //是否支持多云台功能
            bSupportMultiPtzDev = pCfg->GetValue ("FunctionSupport", "IsSupportMultiPtzDev", bSupportMultiPtzDev);
        }
#endif
    }
    CConfig::ReleaseInstance (pCfg);
    pCfg = NULL;
#if defined(__PRODUCT_G84F__) || defined (__PRODUCT_W84F__) || defined (__PRODUCT_G63F__)|| defined (__PRODUCT_W63F__)
	/* 从customability.ini加载定制 */
	pCfg = CConfig::NewInstance();
	s32Ret = pCfg->LoadFileRO (CUSTOM_CONFIGMODE_FILE);
	if (0 == s32Ret )
	{
		if (pCfg->CheckKeyExist ("MultiPtzDevCfg", "bSptPtzLeft") )
		{
			if(bSupportMultiPtzDev)
			{
				//是否支持多云台功能
            	bSptPtzLeft = pCfg->GetValue ("MultiPtzDevCfg", "bSptPtzLeft", bSptPtzLeft);
			}
			else
			{
				bSptPtzLeft = 0;			
			}

        }
		if (pCfg->CheckKeyExist ("MultiPtzDevCfg", "bSptPtzRight") )
		{
			if(bSupportMultiPtzDev)
			{
				//是否支持多云台功能
            	bSptPtzRight = pCfg->GetValue ("MultiPtzDevCfg", "bSptPtzRight", bSptPtzRight);
			}
			else
			{
				bSptPtzRight = 0;
			}

        }
	}
	CConfig::ReleaseInstance (pCfg);
	pCfg = NULL;
#endif

    if (bSupportMotor == 0)
    {
        bSupportIgnoreShakingHead =0;
        bSupportUpDown = 0;
        bSupportLeftRight = 0;
        bSupportPreset = 0;
        bSupportCruise = 0;
        bSupportTrackZoom = 0;
    }

    xml_tree_data[index].p = "Channel:Support";
    xml_tree_data[index].value = bSupportMotor;
    index++;

    xml_tree_data[index].p = "Channel:IgnoreShakingHead";
    xml_tree_data[index].value = bSupportIgnoreShakingHead;
    index++;

    xml_tree_data[index].p = "Channel:PanTile:UpDown:Support";
    xml_tree_data[index].value = bSupportUpDown;
    index++;

    xml_tree_data[index].p = "Channel:PanTile:LeftRight:Support";
    xml_tree_data[index].value = bSupportLeftRight;
    index++;

    xml_tree_data[index].p = "Channel:Preset:Support";
    xml_tree_data[index].value = bSupportPreset;
    index++;

    xml_tree_data[index].p = "Channel:Cruise:Support";
    xml_tree_data[index].value = bSupportCruise;
    index++;
#ifdef __DIGITAL_ZOOM_DRAG_SUPPORT__
    xml_tree_data[index].p = "Channel:Zoom:Support";
    xml_tree_data[index].value = bSupportTrackZoom;
    index++;
#endif

    pFile = fopen(PTZ_CAP_FILE_PATH, "r");
    if (pFile == NULL)
    {
        fprintf(stderr, "open the %s error: %s!\n", PTZ_CAP_FILE_PATH, strerror(errno));
        return -1;
    }
    tree = mxmlLoadFile(NULL, pFile, MXML_TEXT_CALLBACK);
    if (tree == NULL)
    {
        MAIN_LOG_I("Load file %s error!\n", PTZ_CAP_FILE_PATH);
        fclose(pFile);
        return -1;
    }
    fclose(pFile);

    while (i < index)
    {
        MAIN_LOG_I("index[%d] [%s:%d]\n", i, xml_tree_data[i].p, xml_tree_data[i].value);

        if (-1 != xml_tree_data[i].value)
        {
            s32Ret = SetCapFromDeviceCapXML_Bool(tree, xml_tree_data[i].p, xml_tree_data[i].value);
            if (0 != s32Ret)
            {
                MAIN_LOG_I("file:%s line:%d i:%d xml file set error!\n",__FILE__,__LINE__,i);
                return -1;
            }
        }
        i++;
    }
#if defined(__PRODUCT_G84F__) || defined (__PRODUCT_W84F__) || defined (__PRODUCT_G63F__)|| defined (__PRODUCT_W63F__)
    /*能力集支持多云台*/
    mxml_node_t *ptz_cap_node = mxmlFindElement(tree, tree, "PTZCap", NULL, NULL, MXML_DESCEND);


		mxml_node_t *chn_node_2 = mxmlFindElement(ptz_cap_node, ptz_cap_node, "Channel", "ChannelID", "2", MXML_DESCEND);
		if (NULL != chn_node_2)
		{
			
	        mxml_node_t *node_pantile_chn2 = mxmlFindElement(chn_node_2, ptz_cap_node, "PanTile", NULL, NULL, MXML_DESCEND);
	        if (node_pantile_chn2)
	        {
	        	
	            //水平
	            mxml_node_t * node_leftright_chn2 = mxmlFindElement(node_pantile_chn2, ptz_cap_node, "LeftRight", NULL, NULL, MXML_DESCEND);
	            if (node_leftright_chn2)
	            {
	            	 mxml_node_t * node_leftright_support_chn2 = mxmlFindElement(node_leftright_chn2, ptz_cap_node, "Support", NULL, NULL, MXML_DESCEND);
	                if(node_leftright_support_chn2)
	                {	
						mxmlSetText(node_leftright_support_chn2, 0, (bSptPtzLeft==1||bSptPtzLeft==3)? "true":"false");
					}
	            }
	            //垂直
	            mxml_node_t * node_updown_chn2 = mxmlFindElement(node_pantile_chn2, ptz_cap_node, "UpDown", NULL, NULL, MXML_DESCEND);
	            if (node_updown_chn2)
	            {	
					mxml_node_t * node_updown_support_chn2 = mxmlFindElement(node_updown_chn2 , ptz_cap_node, "Support", NULL, NULL, MXML_DESCEND);
	                if(node_updown_support_chn2)
	                {	
	                	
						mxmlSetText(node_updown_support_chn2, 0, bSptPtzLeft==3 ? "true":"false");
					}
	            }
	        }
		}
		mxml_node_t *chn_node_3 = mxmlFindElement(ptz_cap_node, ptz_cap_node, "Channel", "ChannelID", "3", MXML_DESCEND);
		if (NULL != chn_node_3)
		{
	        mxml_node_t *node_pantile_chn3 = mxmlFindElement(chn_node_3, ptz_cap_node, "PanTile", NULL, NULL, MXML_DESCEND);
	        if (node_pantile_chn3)
	        {
				//水平
	            mxml_node_t * node_leftright_chn3 = mxmlFindElement(node_pantile_chn3, ptz_cap_node, "LeftRight", NULL, NULL, MXML_DESCEND);
	            if (node_leftright_chn3)
	            {
	            	mxml_node_t * node_leftright_support_chn3 = mxmlFindElement(node_leftright_chn3, ptz_cap_node, "Support", NULL, NULL, MXML_DESCEND);
					if(node_leftright_support_chn3)
					{
						mxmlSetText(node_leftright_support_chn3, 0, (bSptPtzRight==1||bSptPtzRight==3)? "true":"false");
					}
	            }
	            //垂直
	            mxml_node_t * node_updown_chn3 = mxmlFindElement(node_pantile_chn3, ptz_cap_node, "UpDown", NULL, NULL, MXML_DESCEND);
	            if (node_updown_chn3)
	            {
	            	mxml_node_t * node_updown_support_chn3 = mxmlFindElement(node_updown_chn3, ptz_cap_node, "Support", NULL, NULL, MXML_DESCEND);
					if(node_updown_support_chn3)
					{
						mxmlSetText(node_updown_support_chn3, 0, bSptPtzRight==3 ? "true":"false");
					}
	            }
	        }
		}
	
#endif

    pFile = fopen(PTZ_CAP_FILE_PATH, "w");
    if (NULL == pFile)
    {
        MAIN_LOG_I("fopen PTZ_CAP_FILE_PATH file fail\n");
        return -1;
    }
    mxmlSaveFile(tree, pFile, whitespace_cb);
    mxmlRelease(tree);

    if (pFile)
    {
        fflush(pFile);
        fclose(pFile);
        pFile = 0;
    }

    return 0;
}

//!\func   将部分定制信息填写到各文件中（产测工具）
void Mytest_ProductionTestTool()
{
    //灯板亮灯电平
    int nDLopenStatusCm = 1; //亮灯电平（0高 1低）
    //语言类型
    int nLanguageType = 0; //播报的语言，0：中，1：英，2：其它语言

    char cBoardId[32] = {0};
    int nBoardID = 0;

    // 普通红外模式
    int bSupportNormalInfraredCm = 0; //普通红外夜视（0不支持 1支持）
    int bSptNormIRPassiveCm = 0; //普通红外夜视下自动被动
    int bSptNormIRDayCm = 0; //普通红外夜视下白天
    int bSptNormIRNightCm = 0; //普通红外夜视下晚上
    // 全彩白光/智能全彩
    int bSupportColorNightCm = 0; //全彩白光
    int bSptColorNightAutoCm = 0; //全彩白光下自动
    // 智能双光/智能夜视
    int bSupportIntelligentNightCm = 0; //智能双光

    int nVideoOrientation = 0; //安装方式
    int nAlarmOutDelayTime = 0;//报警输出时间
    int s32Ret = 0;
    char command[128] = {0};

    //填能力用
    int nLightBoardType = 0;
    int nVideoColor = 0;

    int nInfraredBrightness = 0;//红光灯亮度值

    IrcutTimeModeInfo info;

    void *hChannelHandle = ZWSys_EncChannelOpen(0);

    CConfig *pCfgFile = CConfig::GetInstance();

    //获取新的配置文件读取实例
    CConfig *pCustomCfg = CConfig::NewInstance();
    stuShmAuthInfo stShmAuthInfo;
    memset(&stShmAuthInfo, 0, sizeof(stShmAuthInfo));
    Authorize_GetAuthDevInfo(&stShmAuthInfo);
    if (AUTH_SUCCESS == stShmAuthInfo.dwAuthVerifyErrno)
    {
        nBoardID = stShmAuthInfo.dwBoardID;
    }
    else
    {
        nBoardID = GetDefaultBoardID();
    }
    snprintf(cBoardId, sizeof(cBoardId), "BoardID-%x", nBoardID);

    s32Ret = pCustomCfg->LoadFile(CUSTOM_MODE_FILE);
    if (0 == s32Ret)
    {
        nLightBoardType = pCustomCfg->GetValue("SystemFunction", "LightBoardType", 6);
        nVideoColor = pCustomCfg->GetValue("ISP_S1", "VideoColor", 6);
        nVideoOrientation = pCustomCfg->GetValue(cBoardId, "VideoOrientation", 0);
        nDLopenStatusCm = pCustomCfg->GetValue("Device", "DLopenStatus", 0);
        nLanguageType = pCustomCfg->GetValue("PUB", "SpeechLanguage", 1);
        /* 通用里是AlarmOutDelayTime_1，详细配置里是AlarmOutDelayTime_00，只针对报警输出1*/
        nAlarmOutDelayTime = pCustomCfg->GetValue("AlarmOutCfg", "AlarmOutDelayTime_1", 10);
    }

    switch(nLightBoardType)
    {
        case LDR_2PIN_INFRARED_LIGHT_BOARD:
        case NO_LDR_STEPLESS_INFRARED_BOARD:
            bSupportNormalInfraredCm = 1;
            bSptNormIRPassiveCm = 1;
            bSptNormIRDayCm = 1;
            bSptNormIRNightCm = 1;
            break;
        case LDR_3PIN_LIGHT_BOARD:
        case NO_LDR_2PIN_LIGHT_BOARD:
            bSupportNormalInfraredCm = 1;
            bSptNormIRPassiveCm = 1;
            bSptNormIRDayCm = 1;
            bSptNormIRNightCm = 1;
            bSupportColorNightCm = 1;
            bSptColorNightAutoCm = 1;
            bSupportIntelligentNightCm = 1;
        default:
            bSupportNormalInfraredCm = 1;
            bSptNormIRPassiveCm = 1;
            bSptNormIRDayCm = 1;
            bSptNormIRNightCm = 1;
            bSupportColorNightCm = 1;
            bSptColorNightAutoCm = 1;
            bSupportIntelligentNightCm = 1;
            break;
    }

    s32Ret = pCustomCfg->LoadFile(CUSTOM_ABILITY_FILE);
    if (0 == s32Ret)
    {
        // 普通红外模式
        pCustomCfg->SetValue(IMAGECFG, "bSupportNormalInfrared", bSupportNormalInfraredCm);
        pCustomCfg->SetValue(IMAGECFG, "bSptNormIRPassive", bSptNormIRPassiveCm);
        pCustomCfg->SetValue(IMAGECFG, "bSptNormIRDay", bSptNormIRDayCm);
        pCustomCfg->SetValue(IMAGECFG, "bSptNormIRNight", bSptNormIRNightCm);

        // 全彩白光/智能全彩
        pCustomCfg->SetValue(IMAGECFG, "bSupportColorNight", bSupportColorNightCm);
        pCustomCfg->SetValue(IMAGECFG, "bSptColorNightAuto", bSptColorNightAutoCm);

        // 智能双光/智能夜视
        pCustomCfg->SetValue(IMAGECFG, "bSupportIntelligentNight", bSupportIntelligentNightCm);

        // 灯板高低开
        pCustomCfg->SetValue(IMAGECFG, "DLopenStatus", nDLopenStatusCm);
    }
    pCustomCfg->SaveFileReal();

    s32Ret = pCustomCfg->LoadFile(CUSTOM_CONFIGMODE_FILE);
    if (0 == s32Ret)
    {
        if (nVideoOrientation >= 0)
        {
            pCustomCfg->SetValue("ISP_S1", "VideoOrientation", nVideoOrientation);
        }
        pCustomCfg->SetValue("ISP_S1", "VideoColor", nVideoColor);
        pCustomCfg->SetValue(cBoardId, "VideoColor", nVideoColor);
        if (nAlarmOutDelayTime >= 0)
        {
            pCustomCfg->SetValue("AlarmOutCfg", "AlarmOutDelayTime_00", nAlarmOutDelayTime);
        }
        pCustomCfg->SetValue("PUB", "SpeechLanguage", nLanguageType);
        pCustomCfg->SetValue("SystemFunction", "LightBoardType", nLightBoardType);
    }
    pCustomCfg->SaveFileReal();

    memset(command, 0, sizeof(command));
    snprintf(command, sizeof(command), "cp %s %s", CUSTOM_ABILITY_FILE, CUSTOM_BACKUP_ABILITY_FILE);
    mysystem(command);

    memset(command, 0, sizeof(command));
    snprintf(command, sizeof(command), "cp %s %s", CUSTOM_CONFIGMODE_FILE, CUSTOM_BACKUP_CONFIGMODE_FILE);
    mysystem(command);

    CConfig::ReleaseInstance(pCustomCfg);

    USR_ResumeAdminPassword();

    //重新把定制配置替换
    MPU_PAR_CustomFileDate_Replace(CUSTOM_CONFIGMODE_FILE, 1);

    //将灯板相关字段设置到sdk，可以实时生效
    //设置灯板ID
    ZWSys_LightBoardType(nLightBoardType);

    //设置高低电平
    ZWSys_SetLightOpenStatus(nDLopenStatusCm);

    //设置红外模式
    info.nStartHour = pCfgFile->GetValue("ISP_S1", "DayNightStartHour", 7);
    info.nStartMin = pCfgFile->GetValue("ISP_S1", "DayNightStartMin", 0);
    info.nStartSec= pCfgFile->GetValue("ISP_S1", "DayNightStartSec", 0);
    info.nStopHour = pCfgFile->GetValue("ISP_S1", "DayNightStopHour", 19);
    info.nStopMin= pCfgFile->GetValue("ISP_S1", "DayNightStopMin", 0);
    info.nStopSec= pCfgFile->GetValue("ISP_S1", "DayNightStopSec", 0);
    ZWSys_EncSetDayNightMode(hChannelHandle, (DAYNIGHTMODE_T)nVideoColor, &info);

    return;
}

#if defined (__SUPPORT_PRODUCT_TEST_CUSTOM_SPEECH_FILES__)
/* 将src源base64加密文件，解码生成dst文件              */
static int SrcFileBase64DecodeToDstFile(const char* srcfile, const int srcfileSize, const char* dstfile)
{
    if (srcfile == NULL || srcfileSize <= 0 || dstfile == NULL)
    {
        return -1;
    }

    FILE *pSrcFile = NULL;
    FILE *pDstFile = NULL;
    char *encoded_data = NULL;
    char *decoded_data = NULL;
    int decoded_size = 0;
    int nWRLens = 0;

    /* 1、获取源Base64数据 */
    encoded_data = (char *)malloc(srcfileSize+1);
    if (encoded_data == NULL)
    {
        MAIN_LOG_E("Memory allocation failed\n");
        goto READ_END;
    }

    pSrcFile = fopen(srcfile, "rb");
    if (pSrcFile == NULL)
    {
        MAIN_LOG_E("Error: open %s\n", srcfile);
        goto READ_END;
    }

    nWRLens = fread(encoded_data, 1, srcfileSize, pSrcFile);
    if (nWRLens != srcfileSize)
    {
        MAIN_LOG_E("Error: read %s, nWRLens: [%d -- %d]\n", srcfile, nWRLens, srcfileSize);
        goto READ_END;
    }
    encoded_data[srcfileSize] = '\0';

    /* 2、解码Base64数据 */
    decoded_size = (srcfileSize / 4 * 3);
    decoded_data = (char *)malloc(decoded_size);
    if (decoded_data == NULL)
    {
        MAIN_LOG_E("Memory allocation failed\n");
        goto DECODE_END;
    }

    base64Decode(encoded_data, (unsigned char *)decoded_data);

    /* 3、写入解码后数据 */
    pDstFile = fopen(dstfile, "wb");
    if (pDstFile == NULL)
    {
        MAIN_LOG_E("Error: open %s\n", dstfile);
        goto WRITE_END;
    }

    nWRLens = fwrite(decoded_data, 1, decoded_size, pDstFile);
    if (nWRLens != decoded_size)
    {
        MAIN_LOG_E("Error: write %s, nWRLens: [%d -- %d]\n", dstfile, nWRLens, decoded_size);
        goto WRITE_END;
    }

    SAFE_FCLOSE(pSrcFile);
    SAFE_FCLOSE(pDstFile);
    SAFE_FREE(decoded_data);
    SAFE_FREE(encoded_data);
    return 0;

WRITE_END:
    SAFE_FCLOSE(pDstFile);

DECODE_END:
    SAFE_FREE(decoded_data);

READ_END:
    SAFE_FCLOSE(pSrcFile);
    SAFE_FREE(encoded_data);

    return -1;
}
#endif

// 返回0表示成功, 1表示写失败,2表示取消,3表示flash已经满,4表示文件版本不对
int MainUpload_Callback(DWORD dwUserID, int iFileType, unsigned long ulTransSize, unsigned long ulTotalSize, const char *pcIp)
{
    static HANDLE hRemoteUpdate = 0;
    static DWORD s_dwUserID = 0;
    static HANDLE hTemp = 0;
    static int firstComplete = 0;//第一次完成后第二次直接返回
    static int secondComplete = 0;//第二次后面多传的字节都记为第二次完成
    char szCmd[256] = {0};

    //MAIN_LOG_I("pcIp = %s, dwUserID=%d, iFileType=%d, ulTransSize=%d, ulTotalSize=%d\n",  pcIp, dwUserID,iFileType,ulTransSize,ulTotalSize);

    if ((int) 1 == iFileType)	// 系统升级包
    {
        if (NULL != pcIp)		// 开始升级
        {
            //MAIN_LOG_I("---1-1--\n");
            hTemp = (HANDLE) SysUpdateStart();
            if (NULL != hTemp)
            {
                hRemoteUpdate = hTemp;
                //MAIN_LOG_I("---1-1- %d-\n", (int) hRemoteUpdate);
                return (int) hRemoteUpdate;
            }
            else
            {
                return 0;
            }
        }
        else
        {
            if (ulTransSize - ulTotalSize <= 1 && 0 == firstComplete)
            {
                SysUpdateStop((DWORD) hRemoteUpdate);
                int iRet = MPU_SYS_Update_CheckVersion(UPDATEFILENAME);//UPDATEFILENAME

                if (1 == iRet)	// 合法
                {
                    char acUerName[64] = {0};
                    char acIPAddress[20] = {0};
                    char acLoginTime[30] = {0};

                    HANDLE hUser = NULL;

                    DWORD dwIpaddr = 0;
                    MPU_CIU_GetRemoteByID(dwUserID, &hUser, acUerName, sizeof(acUerName), acIPAddress, sizeof(acIPAddress), acLoginTime, sizeof(acLoginTime));
                    IPFromString(acIPAddress, &dwIpaddr);
                    MPU_OPL_AddLog(EDVR_EVENT_OPT, EDVR_EVENT_OPT_DETAIL_UPDATE, 0, 0, dwIpaddr, acUerName);

                    snprintf(szCmd,sizeof(szCmd), "touch %s", UPDATEFILENAME_SUCC);
                    mysystem(szCmd);
                    MPU_SYS_Buzzer(BUZZER500);
#ifdef __8M_FLASH__
                    mysystem("/mnt/app/bin/update &");
#else
                    MAIN_LOG_I(">>>>>SendNotifyMessage<<<<<< start reboot\n");
                    //SendNotifyMessage(g_tGuiWnd.hDeskWnd, MSG_REMOTE_UPDATEOVER, 0, 0);
                    //mysystem("reboot");
                    //chdir ("/");
                    //mysystem("./root/edvr/update.sh &");
#endif
                    ZWSys_SwitchToUpgradeMode(0, NULL, NULL, NULL);
                    firstComplete = 1;
                    secondComplete = 0;
                    return 0;
                }
                else			// 版本不对
                {
                    SysUpdateStop((DWORD) hRemoteUpdate);
                    MPU_SYS_Buzzer(BUZZER500);

                    //海思平台下载升级包前卸载掉了分区
                    MPU_PAR_SaveConfigReal();
                    PAR_ForbiddenSaveFile();

                    MAIN_LOG_I("System will reboot!\n");
                    m_stuRebootParam.nSleepSeconds = 2;
                    sem_post(&m_semStartRebootOpsThread);

                    WriteStateToFile(UPGRADE_CHECKING_FAIL);
                    sleep(5);

                    return 4;
                }
            }
            else if (1 == firstComplete || 1 == secondComplete)//升级包后面多传的一个字节会进来这里，这里返回一个特殊值给write做判断
            {
                if (1 == firstComplete)
                {
                    secondComplete = 1;
                }
                firstComplete = 0;
                return 9;
            }
            else
            {
                if (0xffffffff == ulTransSize)	// 失败
                {
                    SysUpdateStop((DWORD) hRemoteUpdate);
                    MPU_SYS_Buzzer(BUZZER500);

                    WriteStateToFile(UPGRADE_CHECKING_FAIL);
                    sleep(5);

                    return 1;
                }

                return 0;
            }
        }

    }
    else if ((int) 3 == iFileType)	// 导入配置
    {
//	    MAIN_LOG_I("pcIp = %#x\n", pcIp);
        if (NULL != pcIp)		// 开始升级
        {
            hTemp = (HANDLE) SysUpdateStart();
            if (NULL != hTemp)
            {
                hRemoteUpdate = hTemp;
                s_dwUserID = dwUserID;
                return (int) hRemoteUpdate;
            }
            else
            {
                return 0;
            }
        }
        else
        {
            MAIN_LOG_I("ulTotalSize=%ld, ulTransSize=%ld\n", ulTotalSize, ulTransSize);

            if (ulTransSize - ulTotalSize <= 1 && 0 == firstComplete)
            {
                SysUpdateStop((DWORD) hRemoteUpdate);
                if (0 != MainRemoteControl_Callback(s_dwUserID, eRemoteUpdateConfig, 0))
                {
                    return 4;
                }

                firstComplete = 1;
                secondComplete = 0;
            }
            else if (1 == firstComplete || 1 == secondComplete)//升级包后面多传的一个字节会进来这里，这里返回一个特殊值给write做判断
            {
                if (1 == firstComplete)
                {
                    secondComplete = 1;
                }
                firstComplete = 0;
                return 9;
            }
            else
            {
                if (0xffffffff == ulTransSize)	// 失败
                {
                    SysUpdateStop((DWORD) hRemoteUpdate);
                    char szCmd[256] = "";

                    int iRet = 0;

                    snprintf(szCmd, sizeof(szCmd), "rm  %s", IMPORTFILENAME);
                    iRet = mysystem(szCmd);
                    if (iRet)
                    {
                        MAIN_LOG_I("rm Error. szCmd = [%s]. code = %d.\n", szCmd, iRet);
                    }
                    return 1;
                }

            }
            MAIN_LOG_I("MainUpload_Callback %lu %lu\n", ulTransSize, ulTotalSize);
        }
        return 0;
    }
    else if ((int) 2 == iFileType)
    {
        static char acPtzProtocal[200];

        // 开始升级云台协议，pcIp 代表协议绝对地址
        if (NULL != pcIp)
        {
            hTemp = (HANDLE) SysUpdateStart();
            if (NULL != hTemp)
            {
                snprintf(acPtzProtocal, sizeof(acPtzProtocal), "%s", pcIp);
                hRemoteUpdate = hTemp;
                s_dwUserID = dwUserID;
                return (int) hRemoteUpdate;	//;
            }
            else
                return 0;
        }
        else
        {
            if (ulTransSize - ulTotalSize <= 1 && 0 == firstComplete)
            {
                int iRetOver = 0;

                SysUpdateStop((DWORD) hRemoteUpdate);
                if (0 == MPU_PTZ_InstallProtocol(acPtzProtocal))
                {
                    char acUerName[64];

                    char acIPAddress[20];

                    char acLoginTime[30];

                    HANDLE hUser = NULL;

                    DWORD dwIpaddr = 0;
                    MPU_CIU_GetRemoteByID(dwUserID, &hUser, acUerName, sizeof(acUerName), acIPAddress, sizeof(acIPAddress), acLoginTime, sizeof(acLoginTime));
                    IPFromString(acIPAddress, &dwIpaddr);
                    MPU_OPL_AddLog(EDVR_EVENT_SETTING, EDVR_EVENT_SETTING_DETAIL_IMPORT_PTZ, 0, 0, dwIpaddr, acUerName);
                    iRetOver = 0;
                }
                else
                {
                    iRetOver = 4;
                }

                char szCmd[256] = "";

                int iRet = 0;

                snprintf(szCmd, sizeof(szCmd), "rm  %s/*", PTZ_PATH);
                iRet = mysystem(szCmd);
                if (iRet)
                {
                    MAIN_LOG_I("rm Error. szCmd = [%s]. code = %d.\n", szCmd, iRet);
                }
                firstComplete = 1;
                secondComplete = 0;
                return iRetOver;
            }
            else if (1 == firstComplete || 1 == secondComplete)//升级包后面多传的一个字节会进来这里，这里返回一个特殊值给write做判断
            {
                if (1 == firstComplete)
                {
                    secondComplete = 1;
                }
                firstComplete = 0;
                return 9;
            }
            else
            {
                if (0xffffffff == ulTransSize)	// 失败
                {
                    SysUpdateStop((DWORD) hRemoteUpdate);
                    char szCmd[256] = "";

                    int iRet = 0;

                    snprintf(szCmd, sizeof(szCmd), "rm  %s", acPtzProtocal);
                    iRet = mysystem(szCmd);
                    if (iRet)
                    {
                        MAIN_LOG_I("rm Error. szCmd = [%s]. code = %d.\n", szCmd, iRet);
                    }
                    return 1;
                }
            }
        }
        return 0;
    }
    else if ((int) 6 == iFileType)	// 上传图像调节文件
    {
        if (NULL != pcIp)		// 开始升级
        {
            hTemp = (HANDLE) SysUpdateStart();
            if (NULL != hTemp)
            {
                hRemoteUpdate = hTemp;
                s_dwUserID = dwUserID;
                return (int) hRemoteUpdate;
            }
            else
            {
                return 0;
            }
        }
        else
        {
            if (ulTransSize - ulTotalSize <= 1 && 0 == firstComplete)
            {
                SysUpdateStop((DWORD) hRemoteUpdate);
                void *hChannelHandle = ZWSys_EncChannelOpen(0);
                if (0 != ZWSys_SetTuneToneCurve(hChannelHandle, 0, ISPTONECURVEFILENAME))
                {
                    return 4;
                }
                firstComplete = 1;
                secondComplete = 0;
            }
            else if (1 == firstComplete || 1 == secondComplete)//升级包后面多传的一个字节会进来这里，这里返回一个特殊值给write做判断
            {
                if (1 == firstComplete)
                {
                    secondComplete = 1;
                }
                firstComplete = 0;
                return 9;
            }
            else
            {
                if (0xffffffff == ulTransSize)	// 失败
                {
                    SysUpdateStop((DWORD) hRemoteUpdate);
                    char szCmd[256] = "";

                    int iRet = 0;

                    snprintf(szCmd, sizeof(szCmd), "rm  %s", ISPTONECURVEFILENAME);
                    iRet = mysystem(szCmd);
                    if (iRet)
                    {
                        MAIN_LOG_I("rm Error. szCmd = [%s]. code = %d.\n", szCmd, iRet);
                    }
                    return 1;
                }

            }
            MAIN_LOG_I("MainUpload_Callback %lu %lu\n", ulTransSize, ulTotalSize);
        }
        return 0;
    }
    else if(((int)eTransferFileTypeCustomFileUpload) == iFileType)
    {
        if (NULL != pcIp)		// 开始升级
        {
            hTemp = (HANDLE) SysUpdateStart();
            if (NULL != hTemp)
            {
                hRemoteUpdate = hTemp;
                s_dwUserID = dwUserID;
                return (int) hRemoteUpdate;
            }
            else
            {
                return 0;
            }
        }
        else
        {
            //由于产测工具定制完后设备不会重启，避免通用参数定制完后，再定制详细参数失败
            if (ulTransSize - ulTotalSize <= 1)
            {
                SysUpdateStop((DWORD) hRemoteUpdate);
                //判断是否有这个文件
                if (0 == access(CUSTOM_MODE_FILE , 0))//存在文件
                {
                    //判断文件格式
                    FILE* ifd = NULL;
                    char command[64] = "";
                    char buff[1024] = "";
                    if ((ifd = fopen(CUSTOM_MODE_FILE, "r")) == NULL)
                    {
                        fclose(ifd);
                        MAIN_LOG_I("%s File open failed [%s_%d]\n",CUSTOM_MODE_FILE, __FUNCTION__, __LINE__);
                        return 1;
                    }

                    fgets(buff, sizeof(buff)-1, ifd);
                    //int ilen = strlen(buff);
                    //if(('\n' == buff[ilen - 1]) && ('\r' == buff[ilen - 2]))
                    {
                        memset(command, 0, sizeof(command));
                        snprintf(command, sizeof(command), "dos2unix %s", CUSTOM_MODE_FILE);
                        mysystem(command);
                    }

                    fclose(ifd);

                    int ret = Mytest_Analysis_CfgFileAndAbilityFile_Callback(CUSTOM_MODE_FILE, CUSTOM_READONLY_CONFIG_FILE, CUSTOM_CONFIGMODE_FILE, CUSTOM_ABILITY_FILE);
                    if (ret != 0)
                    {
                        //失败
                        return 4;
                    }
                    else
                    {
                        //由定制确定各类信息并写到对应文件
                        Mytest_ProductionTestTool();
                        memset(command, 0, sizeof(command));
                        snprintf(command, sizeof(command), "touch %s", CUSTOM_MODE_FLAG);
                        mysystem(command);

                    }
                    ProCustomRealize();
                }
                else
                {
                    MAIN_LOG_I(" %s file does not exist \n",CUSTOM_MODE_FILE);
                }
            }
            else
            {
                if (0xffffffff == ulTransSize)	// 失败
                {
                    SysUpdateStop((DWORD) hRemoteUpdate);
                    char szCmd[256] = "";

                    int iRet = 0;
                    snprintf(szCmd, sizeof(szCmd), "rm  %s", CUSTOM_MODE_FILE);
                    iRet = mysystem(szCmd);
                    if (iRet)
                    {
                        MAIN_LOG_I("rm Error. szCmd = [%s]. code = %d.\n", szCmd, iRet);
                    }
                    return 1;
                }
            }
            MAIN_LOG_I("MainUpload_Callback %lu %lu\n", ulTransSize, ulTotalSize);
        }
        return 0;
    }
    else if (((int)eTransferFileTypeConfigAndCap) == iFileType || ((int)eTransferFileTypeCustomConfigFileAll) == iFileType)	// 设置配置和能力
    {
        if (NULL != pcIp)		// 开始升级
        {
            hTemp = (HANDLE) SysUpdateStart();
            if (NULL != hTemp)
            {
                hRemoteUpdate = hTemp;
                s_dwUserID = dwUserID;
                return (int) hRemoteUpdate;
            }
            else
            {
                return 0;
            }
        }
        else
        {
            if (ulTransSize - ulTotalSize <= 1 && 0 == firstComplete)
            {
                SysUpdateStop((DWORD) hRemoteUpdate);
                //判断是否有这个文件
                if (0 == access(CUSTOM_MODE_FILE , 0))//存在文件
                {
                    //判断文件格式
                    FILE* ifd = NULL;
                    char command[64] = "";
                    char buff[1024] = "";
                    if ((ifd = fopen(CUSTOM_MODE_FILE, "r")) == NULL)
                    {
                        fclose(ifd);
                        MAIN_LOG_I("%s File open failed [%s_%d]\n",CUSTOM_MODE_FILE, __FUNCTION__, __LINE__);
                        return 1;
                    }

                    fgets(buff, sizeof(buff)-1, ifd);
                    int ilen = strlen(buff);
                    if(('\n' == buff[ilen - 1]) && ('\r' == buff[ilen - 2]))
                    {
                        memset(command, 0, sizeof(command));
                        snprintf(command, sizeof(command), "dos2unix  %s", CUSTOM_MODE_FILE);
                        mysystem(command);
                    }

                    fclose(ifd);

                    //#if defined (__IR_CUT_CONTROL_IMPROVE__)
                    int ret = Mytest_Analysis_CfgFileAndAbilityFile_Callback(CUSTOM_MODE_FILE, CUSTOM_READONLY_CONFIG_FILE, CUSTOM_CONFIGMODE_FILE, CUSTOM_ABILITY_FILE);
                    if (ret != 0)
                    {
                        //失败
                        return 4;
                    }
                    else
                    {
                        //新建定制配置文件的标记
                        memset(command, 0, sizeof(command));
                        snprintf(command, sizeof(command), "touch %s", CUSTOM_MODE_FLAG);
                        mysystem(command);
                    }

                    firstComplete = 1;
                    secondComplete = 0;
                }
                else
                {
                    MAIN_LOG_I(" %s file does not exist \n", CUSTOM_MODE_FILE);
                }
            }
            else if (1 == firstComplete || 1 == secondComplete)//升级包后面多传的一个字节会进来这里，这里返回一个特殊值给write做判断
            {
                if (1 == firstComplete)
                {
                    secondComplete = 1;
                }
                firstComplete = 0;
                return 9;
            }
            else
            {
                if (0xffffffff == ulTransSize)	// 失败
                {
                    SysUpdateStop((DWORD) hRemoteUpdate);
                    char szCmd[256] = "";

                    int iRet = 0;
                    snprintf(szCmd, sizeof(szCmd), "rm  %s", CUSTOM_MODE_FILE);
                    iRet = mysystem(szCmd);
                    if (iRet)
                    {
                        MAIN_LOG_I("rm Error. szCmd = [%s]. code = %d.\n", szCmd, iRet);
                    }
                    return 1;
                }

            }
            MAIN_LOG_I("MainUpload_Callback %lu %lu\n", ulTransSize, ulTotalSize);
        }
        return 0;
    }
#if defined (__SUPPORT_PRODUCT_TEST_CUSTOM_SPEECH_FILES__)
    else if (((int)eTransferFileTypeAllAudioData) == iFileType)	// 设置提示音文件
    {
        if (NULL != pcIp)		// 开始升级
        {
            hTemp = (HANDLE) SysUpdateStart();
            if (NULL != hTemp)
            {
                hRemoteUpdate = hTemp;
                s_dwUserID = dwUserID;
                return (int) hRemoteUpdate;
            }
            else
            {
                return 0;
            }
        }
        else
        {
            if (ulTransSize - ulTotalSize <= 0)
            {
                SysUpdateStop((DWORD) hRemoteUpdate);
                //判断是否有这个文件
                if (0 == access(CUSTOM_ALL_SPEECH_TAR_TMP_PATH, F_OK))
                {
                    int ret = 0;
                    do {
                        /* 将接收到的tmp文件Base64解密 */
                        ret = SrcFileBase64DecodeToDstFile(CUSTOM_ALL_SPEECH_TAR_TMP_PATH, ulTotalSize, CUSTOM_ALL_SPEECH_TAR_PATH);
                        if (0 != ret)
                        {
                            MAIN_LOG_I("rm && cp failed [%s] [%s]\n", DEV_ALL_SPEECH_REAL_PATH, CUSTOM_ALL_SPEECH_TAR_DECODE_PATH);
                            break;
                        }

                        /* 将解密后的文件进行解压 */
                        char szCmd[256] = {0};
                        snprintf(szCmd, sizeof(szCmd), "mkdir -p %s; tar -xvf %s -C %s", CUSTOM_ALL_SPEECH_TAR_DECODE_PATH, CUSTOM_ALL_SPEECH_TAR_PATH, CUSTOM_ALL_SPEECH_TAR_DECODE_PATH);
                        ret = mysystem(szCmd);
                        if ((0 != ret) || (IS_DIR_EMPTY(CUSTOM_ALL_SPEECH_TAR_DECODE_PATH)))
                        {
                            MAIN_LOG_E("tar %s failed\n", CUSTOM_ALL_SPEECH_TAR_PATH);
                            ret = -1;
                            break;
                        }

                        /* 完全覆盖系统默认语音 */
                        memset(szCmd, 0, sizeof(szCmd));
                        snprintf(szCmd, sizeof(szCmd), "rm -rf %s/*; cp -rf %s/* %s/; sync\n", DEV_ALL_SPEECH_REAL_PATH, CUSTOM_ALL_SPEECH_TAR_DECODE_PATH, DEV_ALL_SPEECH_REAL_PATH);
                        ret = mysystem(szCmd);
                        if ((0 != ret) || (IS_DIR_EMPTY(DEV_ALL_SPEECH_REAL_PATH)))
                        {
                            MAIN_LOG_E("rm && cp failed [%s] [%s]\n", DEV_ALL_SPEECH_REAL_PATH, CUSTOM_ALL_SPEECH_TAR_DECODE_PATH);
                            ret = -1;
                            break;
                        }
                    } while(0);

                    SAFE_REMOVE(CUSTOM_ALL_SPEECH_TAR_TMP_PATH);
                    SAFE_REMOVE(CUSTOM_ALL_SPEECH_TAR_PATH);
                    SAFE_REMOVE(CUSTOM_ALL_SPEECH_TAR_DECODE_PATH);

                    if (ret != 0)
                    {
                        return 1;   //失败
                    }
                }
                else
                {
                    MAIN_LOG_E("%s file does not exist \n", CUSTOM_ALL_SPEECH_TAR_TMP_PATH);
                    return 1;
                }
            }
            else
            {
                if (0xffffffff == ulTransSize)	// 失败
                {
                    SysUpdateStop((DWORD) hRemoteUpdate);

                    SAFE_REMOVE(CUSTOM_ALL_SPEECH_TAR_TMP_PATH);
                    return 1;
                }
            }
        }
        return 0;
    }
#endif
    WriteStateToFile(UPGRADE_CHECKING_FAIL);
    sleep(5);
    return -1;
}

#define MAX_LINE_LENGTH 256
#define MAX_SECTIONS 128
#define MAX_KEY_LENGTH 32
#define MAX_VALUE_LENGTH 256

// Key-Value结构体
typedef struct {
    char key[MAX_KEY_LENGTH];
    char value[MAX_VALUE_LENGTH];
} KeyValuePair;

// Section结构体
typedef struct {
    char section_name[MAX_KEY_LENGTH];
    KeyValuePair pairs[64];
    int pair_count;
    int start_line;
} Section;

// 文件行结构体
typedef struct {
    char line[MAX_LINE_LENGTH];
    int is_section;
    int is_comment;  // 注释行标记
} FileLine;

void parse_ini_file(const char *filename, Section *sections,
                   int *section_count, FileLine **lines, int *line_count)
{
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open file");
        return;
    }


    char line_buffer[MAX_LINE_LENGTH];
    Section *current_section = NULL;
    int capacity = 100;
    *line_count = 0;

    // 初始分配内存
    *lines = (FileLine*)malloc(capacity * sizeof(FileLine));
    if (!*lines) {
        perror("Memory allocation failed");
        fclose(file);
        return;
    }

    while (fgets(line_buffer, sizeof(line_buffer), file)) {
        // 动态扩容检查
        if (*line_count >= capacity) {
            capacity *= 2;
            FileLine *temp = (FileLine*)realloc(*lines, capacity * sizeof(FileLine));
            if (!temp) {
                free(*lines);
                perror("Memory reallocation failed");
                fclose(file);
                return;
            }
            *lines = temp;
        }

        // 复制行数据
        strncpy((*lines)[*line_count].line, line_buffer, MAX_LINE_LENGTH);
        (*lines)[*line_count].line[MAX_LINE_LENGTH-1] = '\0';
        (*lines)[*line_count].is_section = 0;
        (*lines)[*line_count].is_comment = 0;

        /* 识别注释行（以;开头的行） */
        if (line_buffer[0] == ';' || line_buffer[0] == '\n') {
            (*lines)[*line_count].is_comment = 1;
        }
        // 处理section头
        else if (line_buffer[0] == '[') {
            char *end = strchr(line_buffer, ']');
            if (end) {
                *end = '\0';
                // 检查是否已存在该section
                int found = 0;
                for (int i = 0; i < *section_count; i++) {
                    if (strcmp(sections[i].section_name, line_buffer+1) == 0) {
                        current_section = &sections[i];
                        current_section->start_line = *line_count;
                        found = 1;
                        break;
                    }
                }

                // 新section处理
                if (!found) {
                    current_section = &sections[*section_count];
                    (*section_count)++;
                    strncpy(current_section->section_name,
                            line_buffer+1, MAX_KEY_LENGTH-1);
                    current_section->section_name[MAX_KEY_LENGTH-1] = '\0';
                    current_section->start_line = *line_count;
                    current_section->pair_count = 0;
                }
                (*lines)[*line_count].is_section = 1;
            }
        }
        // 处理键值对
        else if (current_section) {
            char *equals = strchr(line_buffer, '=');
            if (equals) {
                *equals = '\0';
                char *key = line_buffer;
                char *value = equals + 1;

                // 去除键值两端的空白
                while (*key && isspace(*key)) key++;
                char *end_key = key + strlen(key) - 1;
                while (end_key > key && isspace(*end_key))
                    *(end_key--) = '\0';

                while (*value && isspace(*value)) value++;
                end_key = value + strlen(value) - 1;
                while (end_key > value && isspace(*end_key))
                    *(end_key--) = '\0';

                // 添加或更新键值对
                int found = 0;
                for (int i = 0; i < current_section->pair_count; i++) {
                    if (strcmp(current_section->pairs[i].key, key) == 0) {
                        strncpy(current_section->pairs[i].value,
                                value, MAX_VALUE_LENGTH-1);
                        current_section->pairs[i].value[MAX_VALUE_LENGTH-1] = '\0';
                        found = 1;
                        break;
                    }
                }

                if (!found && (unsigned int)current_section->pair_count < (sizeof(current_section->pairs) / sizeof(KeyValuePair))) {
                    strncpy(current_section->pairs[current_section->pair_count].key,
                            key, MAX_KEY_LENGTH-1);
                    current_section->pairs[current_section->pair_count].key[MAX_KEY_LENGTH-1] = '\0';
                    strncpy(current_section->pairs[current_section->pair_count].value,
                            value, MAX_VALUE_LENGTH-1);
                    current_section->pairs[current_section->pair_count].value[MAX_VALUE_LENGTH-1] = '\0';
                    current_section->pair_count++;
                }
            }
            (*lines)[*line_count].is_section = 0;
        }
        (*line_count)++;
        }
    fclose(file);
    return;
}

void CheckSDUpdateCustomMode()
{
    const char *source_path = "/tmp/custommode.ini";
    const char *dest_path = "/mnt/mtd/config/custommode.ini";
    // 初始化数据结构
    if(access(source_path,F_OK) != 0)
    {
        printf("not fund [%s]\n",source_path);
        return ;
    }

    Section source_sections[32];
    Section target_sections[32];
    FileLine *target_lines = NULL;
    FileLine *target_lines_a = NULL;

    int target_line_count = 0;
    int target_line_count_a = 0;

    int section_count = 0;
    int target_section_count = 0;
    memset(source_sections, 0, sizeof(source_sections));
    memset(target_sections, 0, sizeof(target_sections));
    section_count = 0;
    target_section_count = 0;
    target_line_count = 0;

    // 解析源文件
    parse_ini_file(source_path, source_sections, &section_count, &target_lines_a, &target_line_count);

    // 解析目标文件
    parse_ini_file(dest_path, target_sections, &target_section_count,
                   &target_lines, &target_line_count);

    // 合并逻辑
    for (int i = 0; i < section_count; i++) {
        for (int j = 0; j < target_section_count; j++) {
            if (strcmp(source_sections[i].section_name,
                target_sections[j].section_name) == 0) {
                // 更新目标section的键值对
                for (int k = 0; k < source_sections[i].pair_count; k++) {
                    int found = 0;
                    for (int m = 0; m < target_sections[j].pair_count; m++) {
                        if (strcmp(source_sections[i].pairs[k].key,
                            target_sections[j].pairs[m].key) == 0) {
                            strncpy(target_sections[j].pairs[m].value,
                                    source_sections[i].pairs[k].value,
                                    MAX_VALUE_LENGTH-1);
                            target_sections[j].pairs[m].value[MAX_VALUE_LENGTH-1] = '\0';
                            found = 1;
                            break;
                        }
                    }
                    // 如果源文件中有新键，添加到目标section
                    if (!found && (unsigned int)target_sections[j].pair_count < (sizeof(target_sections[j].pairs) / sizeof(KeyValuePair))) {
                        strncpy(target_sections[j].pairs[target_sections[j].pair_count].key,
                                source_sections[i].pairs[k].key,
                                MAX_KEY_LENGTH-1);
                        target_sections[j].pairs[target_sections[j].pair_count].key[MAX_KEY_LENGTH-1] = '\0';
                        strncpy(target_sections[j].pairs[target_sections[j].pair_count].value,
                                source_sections[i].pairs[k].value,
                                MAX_VALUE_LENGTH-1);
                        target_sections[j].pairs[target_sections[j].pair_count].value[MAX_VALUE_LENGTH-1] = '\0';
                        target_sections[j].pair_count++;
                    }
                }
                break;
            }
        }
    }

    // 写入目标文件（保留原始结构）
    FILE *dest_file = fopen(dest_path, "w");
    if (!dest_file) {
        perror("Failed to open destination file");
        return;
    }

    // 逐行写入，修改已存在的section
    for (int i = 0; i < target_line_count; ) {

        if (target_lines[i].is_comment) {
            /* 直接写入注释行 */
            fprintf(dest_file, "%s", target_lines[i].line);
            i++;
        }
        else if (target_lines[i].is_section) {
            // 查找当前section
            char section_name[MAX_KEY_LENGTH];
            sscanf(target_lines[i].line, "[%[^]]", section_name);
            section_name[MAX_KEY_LENGTH-1] = '\0';

            for (int j = 0; j < target_section_count; j++) {
                if (strcmp(section_name, target_sections[j].section_name) == 0) {
                    // 写入修改后的section内容
                    fprintf(dest_file, "[%s]\n", target_sections[j].section_name);
                    for (int k = 0; k < target_sections[j].pair_count; k++) {
                        fprintf(dest_file, "%s=%s\n",
                                target_sections[j].pairs[k].key,
                                target_sections[j].pairs[k].value);
                    }
                    // 跳过原始section的剩余行
                    i++;
                    while (i < target_line_count &&
                           !target_lines[i].is_section &&
                           !target_lines[i].is_comment) {
                        i++;
                    }
                    break;
                }
            }
        } else {
            // 非section行直接写入
            fprintf(dest_file, "%s", target_lines[i++].line);
            i++;
        }
    }

    fclose(dest_file);

    // 释放内存
    if (target_lines) {
        free(target_lines);
        target_lines = NULL;
        free(target_lines_a);
        target_lines_a = NULL;
    }

    int ret = Mytest_Analysis_CfgFileAndAbilityFile_Callback(CUSTOM_MODE_FILE, CUSTOM_READONLY_CONFIG_FILE, CUSTOM_CONFIGMODE_FILE, CUSTOM_ABILITY_FILE);
    if (ret != 0)
    {
        printf("CheckSDUpdateCustomMode Analysis File !!!\n");
    }
    else
    {
        char command[256]  = {0};
        //由定制确定各类信息并写到对应文件
        Mytest_ProductionTestTool();
        memset(command, 0, sizeof(command));
        snprintf(command, sizeof(command), "touch %s", CUSTOM_MODE_FLAG);
        mysystem(command);

    }
    mysystem("rm /tmp/custommode.ini");
    return;
}


#if defined (__SUPPORT_PRODUCT_TEST_CUSTOM_SPEECH_FILES__)
static int CheckSDUpdateAllAudioFiles()
{
    if (access(CUSTOM_ALL_SPEECH_TAR_PATH, F_OK) != 0)
    {
        return 0;
    }

    int ret = 0;
    char szCmd[256] = {0};

    /* MD5校验，一致则不升级，避免重复写入可读写分区 */
    if (access(CUSTOM_ALL_SPEECH_TAR_MD5_FILE, F_OK) == 0)
    {
        do {
            int fd = -1;
            char calc_md5[64] = {0};
            char file_md5[64] = {0};
            int nFileMd5Len = 0;
            int nCalcMd5Len = 0;

            /* 获取分区文件中的MD5值 */
            fd = open(CUSTOM_ALL_SPEECH_TAR_MD5_FILE, O_RDONLY);
            if (-1 == fd)
            {
                MAIN_LOG_E("open file error:%s\n", CUSTOM_ALL_SPEECH_TAR_MD5_FILE);
                break;
            }

            memset(file_md5, 0, sizeof(file_md5));
            ret = read(fd, file_md5, 32);
            if (-1 == ret)
            {
                MAIN_LOG_E("read file error:%s\n", CUSTOM_ALL_SPEECH_TAR_MD5_FILE);
                close(fd);
                break;
            }
            close(fd);

            /* 计算要升级的语音压缩包的MD5值 */
            snprintf(szCmd, sizeof(szCmd), "md5sum %s | awk '{print $1}'", CUSTOM_ALL_SPEECH_TAR_PATH);
            ret = ShellCmdOut(szCmd, calc_md5, sizeof(calc_md5));
            if (ret)
            {
                MAIN_LOG_E("md5sum %s fail!\n", CUSTOM_ALL_SPEECH_TAR_PATH);
                break;
            }

            /* 去掉结尾的回车换行 */
            calc_md5[strlen(calc_md5)-1] = '\0';

            nFileMd5Len = strlen(file_md5);
            nCalcMd5Len = strlen(calc_md5);

            MAIN_LOG_I("len:%d file_md5:%s\n", nFileMd5Len, file_md5);
            MAIN_LOG_I("len:%d calc_md5:%s\n", nCalcMd5Len, calc_md5);

            /* 比较MD5值是否相等 */
            if ((nFileMd5Len > 0) && (nFileMd5Len == nCalcMd5Len) && (0 == strncmp(calc_md5,file_md5,nFileMd5Len)))
            {
                goto FUN_END;
            }
        } while(0);

    }

    /* 文件进行解压 */
    memset(szCmd, 0, sizeof(szCmd));
    snprintf(szCmd, sizeof(szCmd), "mkdir -p %s; tar -xvf %s -C %s", CUSTOM_ALL_SPEECH_TAR_DECODE_PATH, CUSTOM_ALL_SPEECH_TAR_PATH, CUSTOM_ALL_SPEECH_TAR_DECODE_PATH);
    ret = mysystem(szCmd);
    if ((0 != ret) || (IS_DIR_EMPTY(CUSTOM_ALL_SPEECH_TAR_DECODE_PATH)))
    {
        MAIN_LOG_E("tar %s failed\n", CUSTOM_ALL_SPEECH_TAR_PATH);
        ret = -1;
        goto FUN_END;
    }

    /* 完全覆盖系统默认语音 */
    memset(szCmd, 0, sizeof(szCmd));
    snprintf(szCmd, sizeof(szCmd), "rm -rf %s/*; cp -rf %s/* %s/; sync\n", DEV_ALL_SPEECH_REAL_PATH, CUSTOM_ALL_SPEECH_TAR_DECODE_PATH, DEV_ALL_SPEECH_REAL_PATH);
    ret = mysystem(szCmd);
    if ((0 != ret) || (IS_DIR_EMPTY(DEV_ALL_SPEECH_REAL_PATH)))
    {
        MAIN_LOG_E("rm && cp failed [%s] [%s]\n", DEV_ALL_SPEECH_REAL_PATH, CUSTOM_ALL_SPEECH_TAR_DECODE_PATH);
        ret = -1;
        goto FUN_END;
    }

    /* 保存MD5值 */
    memset(szCmd, 0, sizeof(szCmd));
    snprintf(szCmd, sizeof(szCmd), "md5sum %s | awk '{print $1}' > %s", CUSTOM_ALL_SPEECH_TAR_PATH, CUSTOM_ALL_SPEECH_TAR_MD5_FILE);
    mysystem(szCmd);

FUN_END:
    SAFE_REMOVE(CUSTOM_ALL_SPEECH_TAR_PATH);
    SAFE_REMOVE(CUSTOM_ALL_SPEECH_TAR_DECODE_PATH);

    return 0;
}
#endif

//修改控制客户端的能力xml文件,放到前面，防止上报给app的接口获取的快导致一些定制未初始化成功
static int modifyDeviceCap()
{
    pthread_mutex_lock(&g_DeviceCapLock);

    /* 首先从SDK里面获取当前设备的激活状态码 */
    int MaxSns = ZWSys_GetMaxSensorCnt();

    /* 配置设备能力 */
    GenDevCapByCustom(MaxSns);

    GenDevPTZCapByCustom(0);
    pthread_mutex_unlock(&g_DeviceCapLock);
    return 0;
}

int check_protocol_type(void)
{
    char cutbuf[64]={0};
    char probuf[64]={0};
    char acCmd[256] = {0};
    int ret = 0;
    char ip[20]={0};

    //获取新的配置文件读取实例
    CConfig *pCfg = CConfig::NewInstance();
    CConfig *pCfg_Protocol = CConfig::NewInstance();

    //如果使用了产测工具定制APPUrl,就用custominfo定制的，没有就用以前的
    if(0 == pCfg->LoadFileRO(CUSTOM_CONFIG_FILE))
    {
        if (pCfg->CheckKeyExist("P2P", "APPDOWNLOAD"))
        {
            pCfg->GetValue("P2P", "APPDOWNLOAD", "", cutbuf, sizeof(cutbuf));
        }
        else
        {
            //避免设备在没有用产测工具定制时，APPURL为空的情况
            if(0 == access(CUSTOM_FILE, F_OK))
            {
                if(0 == pCfg->LoadFileRO(CUSTOM_FILE))
                {
                    pCfg->GetValue("P2P", "APPDOWNLOAD", "", cutbuf, sizeof(cutbuf));
                }
                else //定制文件读取失败
                {
                    goto ERR;
                }
            }
            else //定制文件不存在
            {
                goto ERR;
            }
        }
    }
    else //定制文件读取失败
    {
        goto ERR;
    }

    if(0 == pCfg_Protocol->LoadFile(PROTOCOL_FILE))
    {
        if(NULL != pCfg_Protocol->GetValue("P2P", "APPDOWNLOAD", "", probuf, sizeof(probuf)))
        {
            if(0 == strncmp(cutbuf, probuf, sizeof(cutbuf))) //一致，不用处理
            {
                MAIN_LOG_I("Needn't update\n");

                pCfg->ReleaseInstance(pCfg);
                pCfg_Protocol->ReleaseInstance(pCfg_Protocol);
                return 0;
            }
            else
            {
                MAIN_LOG_I("get link ls %s\n",cutbuf);
                pCfg_Protocol->SetValue("P2P", "APPDOWNLOAD", cutbuf);
                pCfg_Protocol->SaveFile();

                pCfg->ReleaseInstance(pCfg);
                pCfg_Protocol->ReleaseInstance(pCfg_Protocol);
                return 0;
            }
        }
    }
    else
    {
        snprintf(acCmd, sizeof(acCmd), "cp %s %s", PROTOCOL_HTML_PATH, PROTOCOL_FILE);
        mysystem(acCmd);
        MAIN_LOG_I("get link ls %s\n",cutbuf);

        if(0 == pCfg_Protocol->LoadFile(PROTOCOL_FILE))
        {
            pCfg_Protocol->SetValue("P2P", "APPDOWNLOAD", cutbuf);
        }
        pCfg_Protocol->SaveFile();
        pCfg->ReleaseInstance(pCfg);
        pCfg_Protocol->ReleaseInstance(pCfg_Protocol);
        return 0;
    }

ERR:
        /********识别P2P类型*******/
    MAIN_LOG_I("\nDefault Config\n");
    snprintf(acCmd, sizeof(acCmd), "cp %s %s", PROTOCOL_HTML_PATH, PROTOCOL_FILE);
    mysystem(acCmd);

    if(0 == pCfg->LoadFile(PROTOCOL_FILE))
    {
        if ((0 == access(P2P_TYPE_TUTK, 0)) && (access(P2P_TYPE_SHYUN, 0) != 0)) //tutk
        {
            pCfg->SetValue("P2P", "APPDOWNLOAD", "http://115.28.147.140/p2p_direct.html");
        }
        else if (0 == access(P2P_TYPE_ULK, 0)) //ULK
        {
            pCfg->SetValue("P2P", "APPDOWNLOAD", "http://www.zwcloud.wang/p2p_direct_anyan.html");
        }
        else if (0 == access(P2P_TYPE_SHYUN, 0))//shangyun
        {
            CConfig *Edvr_cfg = CConfig::NewInstance();
            if(0 == Edvr_cfg->LoadFileRO(IDVR_DEFAULT_CONFIG_FILE_PATH))
            {
                if(NULL != Edvr_cfg->GetValue("INet_Cfg", "LocalIP", "", ip, sizeof(ip)))
                {
                    if(0 == strncmp("192.168.1.10",ip,12)) //根据默认IP来区分协议库
                    {
                        MAIN_LOG_I("Default IP is 192.168.1.10\n");
                        pCfg->SetValue("P2P", "APPDOWNLOAD", "http://www.zwcloud.wang/p2p_direct_c365.html");
                    }
                    else
                    {
                         MAIN_LOG_I("Normal version \n");
                         pCfg->SetValue("P2P", "APPDOWNLOAD", "http://www.zwcloud.wang/p2p_direct_p6s.html");
                    }
                }
                else
                {
                    MAIN_LOG_I("LoadConfig IDVR_DEFAULT_CONFIG_FILE_PATH error \n");
                    pCfg->SetValue("P2P", "APPDOWNLOAD", "http://www.zwcloud.wang/p2p_direct_c365.html");
                }
            }
            else
            {
                  MAIN_LOG_I("LoadConfig IDVR_DEFAULT_CONFIG_FILE_PATH error \n");
                  pCfg->SetValue("P2P", "APPDOWNLOAD", "http://www.zwcloud.wang/p2p_direct_c365.html");
            }
            Edvr_cfg->ReleaseInstance(Edvr_cfg);
        }
    }
    pCfg->SaveFile();
    pCfg->ReleaseInstance(pCfg);
    pCfg_Protocol->ReleaseInstance(pCfg_Protocol);
    return ret;
}

int SetSysStartTag(int Stat)
{
    int nRet = 0;
    nRet = ZWSys_SetSysStartTag(Stat);

    return nRet;
}

static int CheckCustomFile()
{
    int nRet = 0;
    char buf[128];

    CConfig *pCfg = CConfig::NewInstance();

    if (0 == (access(CUSTOM_BACKUP_READONLY_CONFIG_FILE, 0)))
    {
        nRet = pCfg->LoadFileRO(CUSTOM_READONLY_CONFIG_FILE);
        //pCfg->SaveFileReal(1);
        if (IDVR_OK != nRet)
        {
            memset(buf, 0, sizeof(buf));
            snprintf(buf, sizeof(buf), "cp -v %s %s", CUSTOM_BACKUP_READONLY_CONFIG_FILE, CUSTOM_READONLY_CONFIG_FILE);
            mysystem(buf);
        }
    }
    else
    {
        memset(buf, 0, sizeof(buf));
        snprintf(buf, sizeof(buf), "cp -v %s %s", CUSTOM_READONLY_CONFIG_FILE, CUSTOM_BACKUP_READONLY_CONFIG_FILE);
        mysystem(buf);
    }

    if (0 == (access(CUSTOM_BACKUP_CONFIGMODE_FILE, 0)))
    {
        nRet = pCfg->LoadFileRO(CUSTOM_CONFIGMODE_FILE);
        //pCfg->SaveFileReal(1);
        if (IDVR_OK != nRet)
        {
            memset(buf, 0, sizeof(buf));
            snprintf(buf, sizeof(buf), "cp -v %s %s", CUSTOM_BACKUP_CONFIGMODE_FILE, CUSTOM_CONFIGMODE_FILE);
            mysystem(buf);
        }
    }
    else
    {
        memset(buf, 0, sizeof(buf));
        snprintf(buf, sizeof(buf), "cp -v %s %s", CUSTOM_CONFIGMODE_FILE, CUSTOM_BACKUP_CONFIGMODE_FILE);
        mysystem(buf);
    }

    if (0 == (access(CUSTOM_BACKUP_ABILITY_FILE, 0)))
    {
        nRet = pCfg->LoadFileRO(CUSTOM_ABILITY_FILE);
        //pCfg->SaveFileReal(1);
        if (IDVR_OK != nRet)
        {
            memset(buf, 0, sizeof(buf));
            snprintf(buf, sizeof(buf), "cp -v %s %s", CUSTOM_BACKUP_ABILITY_FILE, CUSTOM_ABILITY_FILE);
            mysystem(buf);
        }
    }
    else
    {
        memset(buf, 0, sizeof(buf));
        snprintf(buf, sizeof(buf), "cp -v %s %s", CUSTOM_ABILITY_FILE, CUSTOM_BACKUP_ABILITY_FILE);
        mysystem(buf);
    }

    pCfg->ReleaseInstance(pCfg);

    return nRet;
}

void SetCallBack()
{
    int iRet = 0;

    int iChNum = EncChNum();

    int i = 0;

    // 设置移动侦测报警回调函数
    iRet = MPU_AVL_SetAlarmCallBack(ALERT_TYPE_MOTION, MainAlarmCallBack);
    if (iRet != 0)
    {
        MAIN_LOG_I("MPU_AVL_SetAlarmCallBack erro\n");
    }
    for (i = 0; i < iChNum; i++)
    {
        MPU_AVL_EnableAction(i, ALERT_TYPE_MOTION, ACTION_GUI, 1);
    }

    // 设置人形检测报警回调函数
    iRet = MPU_AVL_SetAlarmCallBack(ALERT_TYPE_HUMANSHAPE, MainAlarmCallBack);
    if (iRet != 0)
    {
        MAIN_LOG_I("MPU_AVL_SetAlarmCallBack erro\n");
    }
    for (i = 0; i < iChNum; i++)
    {
        MPU_AVL_EnableAction(i, ALERT_TYPE_HUMANSHAPE, ACTION_GUI, 1);
    }

    // 设置声音侦测报警回调函数
    iRet = MPU_AVL_SetAlarmCallBack(ALERT_TYPE_SOUNDDETECT, MainAlarmCallBack);
    if (iRet != 0)
    {
        MAIN_LOG_I("MPU_AVL_SetAlarmCallBack erro\n");
    }
    for (i = 0; i < iChNum; i++)
    {
        MPU_AVL_EnableAction(i, ALERT_TYPE_SOUNDDETECT, ACTION_GUI, 1);
    }

    /*
     * 非法访问
     * ALERT_TYPE_ILLEGEACCESS,
     */
    MPU_USR_SetAlarmCallBack(ALERT_TYPE_ILLEGEACCESS, MainAlarmCallBack);

    //!IP冲突
    MPU_NET_SetIPCallBack(ALERT_TYPE_IPCONFLICT, MainAlarmCallBack);

    //!网线断
    MPU_NET_SetPHYCallBack(ALERT_TYPE_RETICLEDISCONNECT, MainAlarmCallBack);

    //!硬盘错误
    MPU_STR_Disk_SetErrorNotifyCallback(ALERT_TYPE_DISKERROR, MainAlarmCallBack);

    //!硬盘满
    MPU_STR_DiskGroup_SetFullCallBack(ALERT_TYPE_DISKFULL, MainAlarmCallBack);

    //盘组异常
    MPU_STR_DiskGroup_SetErrorCallBack(ALERT_TYPE_DISKGROUP_ERROR, MainAlarmCallBack);

    MPU_CIU_SetUpdateNotifyCallback(MainUpload_Callback);
    MPU_CIU_SetRemoteControlCallback(MainRemoteControl_Callback);

    /* 设置录像写文件完毕之后的触发回调 */
    MPU_AVL_SetTrigRecordOverCallBack(TrigWriteFileOverCallBack);

    /* 获取当前UTC时间和系统时间回调 */
    MPU_AVL_SetGetTimeOffsetCallBack(0, GetTimeOffsetCallBack);

    /* 设置定时抓图回调 */
    MPU_STR_SetTimedCaptureCallBack(StartTimedCaptureCallBack);

    MPU_SYS_SetCheckInterNetStatusCallBack(CheckInterNetStatusCallBack);
    //设置二维码扫描回调
    ZWSys_RegisterQrScanCallback(QrScanCallback);

    MPU_SYS_Maintenance_SetCallBack(Maintenance_Cb);

    MPU_SYS_AwakeSchedule_SetCallBack(AwakeSchedule_Cb);
}

//开机检测是否需要重新拆分
static int CustommodeRestore()
{
    char command[256] = {0};

    if (0 == access(CUSTOM_RESTORE_MODE_FLAG, F_OK))
    {
        int ret = Mytest_Analysis_CfgFileAndAbilityFile_Callback(CUSTOM_MODE_FILE, CUSTOM_READONLY_CONFIG_FILE, CUSTOM_CONFIGMODE_FILE, CUSTOM_ABILITY_FILE);
        if (0 == ret)
        {
            memset(command, 0, sizeof(command));
            snprintf(command, sizeof(command), "rm %s",CUSTOM_RESTORE_MODE_FLAG);
            mysystem(command);

            MAIN_LOG_I("[%s-%d][%s] Need to be split again !!!!![%s][%s][%s][%s][%s]\n",__FILE__,__LINE__,__FUNCTION__,CUSTOM_MODE_FILE,CUSTOM_READONLY_CONFIG_FILE,CUSTOM_CONFIGMODE_FILE, CUSTOM_ABILITY_FILE);
        }
        else
        {
            //失败
            MAIN_LOG_I("[%s-%d][%s] Split failed!!!!! \n",__FILE__,__LINE__,__FUNCTION__);
            return 4;
        }

        memset(command, 0, sizeof(command));
        snprintf(command, sizeof(command), "touch %s", CUSTOM_MODE_FLAG);
        mysystem(command);

        Mytest_ProductionTestTool();
        updateCustomFileMD5();
    }

    return 0;
}

#if defined(__STATIC_LINK_STDC__)
//判断文件是否可读
static int check_update (char *_fpath)
{
    struct stat st;

    if (access ((char *) (_fpath), R_OK | F_OK) < 0)
    {
        return -1;
    }
    else
    {
        stat ((char *) _fpath, &st);
        if (0 == st.st_size)
        {
            MAIN_LOG_I("[update] %s size is 0\n",_fpath);
            return -1;
        }
        else
        {
            return 0;
        }
    }
}

//检测是否更新升级配置文件
static int CheckUpdateFile()
{
    int nRet = 0;
    char buf[256];

    if ((0 == check_update ((char *) (UPDATE_CFG_FILE))) &&
        (0 == check_update ((char *) (UPDATEFLAG_CFG_FILE))))
    {
        MAIN_LOG_I("[update] Start\n");

        CConfig *cfg = CConfig::NewInstance();
        nRet = cfg->LoadFile(IDVR_CURRENT_CONFIG_FILE_PATH);
        if (0 != nRet)
        {
            MAIN_LOG_I("[update] Load File Failed\n");
        }

        //升级配置
        nRet = cfg->UpdateCfg(UPDATE_CFG_FILE, UPDATEFLAG_CFG_FILE);
        if (0 != nRet)
        {
            MAIN_LOG_I("[update] UpdateCfg Failed\n");
        }

        nRet = cfg->SaveFileReal();
        if (0 != nRet)
        {
            MAIN_LOG_I("[update] SaveFileReal Failed nRet(%d)\n",nRet);
        }

        cfg->ForbiddenSaveFile();
        cfg->ReleaseInstance(cfg);

        //删除升级配置文件
        memset(buf, 0, sizeof(buf));
        snprintf(buf, sizeof(buf), "rm %s %s", UPDATE_CFG_FILE, UPDATEFLAG_CFG_FILE);
        mysystem(buf);

        MAIN_LOG_I("[update] End\n");
    }

    return nRet;
}
#endif

//设置云卡能力集回调
int SetCloudCardCapCallback(int iSupport)
{
    FILE *pFile = NULL;
    const char *value = NULL;

    mxml_node_t *tree = NULL;
    mxml_node_t *node = NULL;
    mxml_node_t *node_4gconfig = NULL;
    mxml_node_t *node_cloudcard = NULL;
    mxml_node_t *node_support = NULL;

    pthread_mutex_lock(&g_DeviceCapLock);
    pFile = fopen(CUSTOM_DEVICE_CAP_FILE, "r");
    if(pFile == NULL)
    {
        fprintf(stderr, "open the %s error: %s!\n", CUSTOM_DEVICE_CAP_FILE, strerror(errno));
        pthread_mutex_unlock(&g_DeviceCapLock);
        return -1;
    }
    tree = mxmlLoadFile(NULL, pFile, MXML_TEXT_CALLBACK);
    if(tree == NULL)
    {
        MAIN_LOG_I("Load file %s error!\n", CUSTOM_DEVICE_CAP_FILE);
        fclose(pFile);
        pthread_mutex_unlock(&g_DeviceCapLock);
        return -1;
    }

    fclose(pFile);

    node = mxmlFindElement(tree, tree, "Device", NULL, NULL, MXML_DESCEND);
    if(node == NULL)
    {
        MAIN_LOG_I("can not find element Device!\n");
        mxmlRelease(tree);
        pthread_mutex_unlock(&g_DeviceCapLock);
        return -1;
    }

    node_4gconfig = mxmlFindElement(node, tree, "_4GConfig", NULL, NULL, MXML_DESCEND);
    if(node_4gconfig == NULL)
    {
        MAIN_LOG_I("can not find element _4GConfig!\n");
        mxmlRelease(tree);
        pthread_mutex_unlock(&g_DeviceCapLock);
        return -1;
    }

    node_cloudcard = mxmlFindElement(node_4gconfig, tree, "CloudCard", NULL, NULL, MXML_DESCEND);
    if(node_cloudcard == NULL)
    {
        MAIN_LOG_I("can not find element CloudCard!\n");
        mxmlRelease(tree);
        pthread_mutex_unlock(&g_DeviceCapLock);
        return -1;
    }

    node_support = mxmlFindElement(node_cloudcard, tree, "Support", NULL, NULL, MXML_DESCEND);
    if(node_support == NULL)
    {
        MAIN_LOG_I("can not find element CloudCard!\n");
        mxmlRelease(tree);
        pthread_mutex_unlock(&g_DeviceCapLock);
        return -1;
    }
    else
    {
        value = mxmlGetText(node_support, NULL);
        if(NULL == value)
        {
            MAIN_LOG_I("get CloudCard error!\n");
        }
        mxmlSetText(node_support, 0, (1 == iSupport) ? "true": "false");
    }

    pFile = fopen(CUSTOM_DEVICE_CAP_FILE, "w");
    if (pFile)
    {
        mxmlSaveFile(tree, pFile, whitespace_cb);
        fflush(pFile);
        fclose(pFile);
        pFile = NULL;
    }

    mxmlRelease(tree);
    pthread_mutex_unlock(&g_DeviceCapLock);

    return 0;
}

#ifdef __IS_SUPPORT_AIC_BLUETOOTH__
//蓝牙配网数据-SSID
static char s_szWlanSSID[WLAN_MAX_SSID_LEN] = {0};
//蓝牙配网数据-PWD
static char s_szWlanKey[WLAN_MAX_KEY_LEN] = {0};
//蓝牙配网数据-SSID是否拿到
static char s_bIsFindWlanSSID = 0;
//蓝牙配网数据-PWD是否拿到
static char s_bIsFindWlanKey = 0;
//蓝牙配网数据控制锁
pthread_mutex_t s_muBtCfgNetDataLock = PTHREAD_MUTEX_INITIALIZER;
//蓝牙配网线程退出标志
static char s_bIsExitBtCfgNetTh = 0;
//蓝牙请求命令返回线程退出标志
static char s_bIsExitBtReqRetTh = 0;
static XRSBLE_L2CmdDataSet s_stCmdDataSet = {0};
//蓝牙发送待数据控制锁
static pthread_mutex_t s_muBtSendDataLock = PTHREAD_MUTEX_INITIALIZER;
//蓝牙发送待数据信号量
static sem_t semBtSendData;

//蓝牙配网线程
static void *Fnx_BlueToothConfigureNetThread(void *lParam)
{
    int i = 0;
    int nCurrWifiMode = 0;
    int nWlanScanStatue = 1;
    WLAN_CONNECTION stWlanConCfg = {0};
    char bIsFindWlanSSID = 0;
    char bIsFindWlanKey = 0;
    char szWlanSSID[WLAN_MAX_SSID_LEN] = {0};
    char szWlanKey[WLAN_MAX_KEY_LEN] = {0};
    pthread_detach(pthread_self());

    PRT_TID();

    while (1)
    {
        //判断是否需要退出
        if (s_bIsExitBtCfgNetTh)
        {
            break;
        }

        pthread_mutex_lock(&s_muBtCfgNetDataLock);
        if (s_bIsFindWlanSSID
            && s_bIsFindWlanKey)
        {
            bIsFindWlanSSID = 1;
            bIsFindWlanKey = 1;

            memset(szWlanSSID, 0, sizeof(szWlanSSID));
            snprintf(szWlanSSID, sizeof(szWlanSSID), "%s", s_szWlanSSID);
            memset(szWlanKey, 0, sizeof(szWlanKey));
            snprintf(szWlanKey, sizeof(szWlanKey), "%s", s_szWlanKey);

            //取走后清空
            s_bIsFindWlanSSID = 0;
            s_bIsFindWlanKey = 0;
            memset(s_szWlanSSID, 0, sizeof(s_szWlanSSID));
            memset(s_szWlanKey, 0, sizeof(s_szWlanKey));
        }
        pthread_mutex_unlock(&s_muBtCfgNetDataLock);

        if (bIsFindWlanSSID
            && bIsFindWlanKey)
        {
            //提示配网信息传输完成
            ZWSys_SetSpeechContent(SPEECH_SYS_QRCODE_SCAN_OVER);
            //循环查询，等待播放结束
            i = 0;
            while (1)
            {
                SelectSleepMSec(100);
                if (eSpeechPlayIdle == ZWSys_GetSpeechPlayingStatus())
                {
                    break;
                }
                if (++i > (SPEECH_PLAY_TIME_OUT_SEC*10))
                {
                    i = 0;
                    break;
                }
            }

            //配网信息必须有效才使用本次扫描到的信息
            if (bIsFindWlanSSID && strlen(szWlanSSID) > 0)
            {
                //查询上次配网是否已结束
                if (0 == MPU_WLAN_IsConfigureNetIng())
                {
                    /*配网*/
                    //AP模式下才配网
                    MPU_NET_GetCurrWifiMode(&nCurrWifiMode);
                    if (SOFT_AP_MODE == nCurrWifiMode)
                    {
                        //配网
                        memset(&stWlanConCfg, 0, sizeof(stWlanConCfg));
                        memcpy(stWlanConCfg.szSSID, szWlanSSID, sizeof(stWlanConCfg.szSSID)-1);
                        stWlanConCfg.struKeyCfg.bValid = 1;
                        stWlanConCfg.struKeyCfg.Key1.bValid = 1;
                        memcpy(stWlanConCfg.struKeyCfg.Key1.szKey, szWlanKey,
                            sizeof(stWlanConCfg.struKeyCfg.Key1.szKey)-1);
                        stWlanConCfg.struKeyCfg.Key2.bValid = 1;
                        memcpy(stWlanConCfg.struKeyCfg.Key2.szKey, szWlanKey,
                            sizeof(stWlanConCfg.struKeyCfg.Key2.szKey)-1);
                        stWlanConCfg.struKeyCfg.Key3.bValid = 1;
                        memcpy(stWlanConCfg.struKeyCfg.Key3.szKey, szWlanKey,
                            sizeof(stWlanConCfg.struKeyCfg.Key3.szKey)-1);
                        stWlanConCfg.struKeyCfg.Key4.bValid = 1;
                        memcpy(stWlanConCfg.struKeyCfg.Key4.szKey, szWlanKey,
                            sizeof(stWlanConCfg.struKeyCfg.Key4.szKey)-1);
                        stWlanConCfg.struKeyCfg.WPAKey.bValid = 1;
                        memcpy(stWlanConCfg.struKeyCfg.WPAKey.szKey, szWlanKey,
                            sizeof(stWlanConCfg.struKeyCfg.WPAKey.szKey)-1);

                        // 防止带验证低于8位后AP模式切换异常, 做简单判断
                        int keyLen = strlen(szWlanKey);
                        if ((keyLen == 0) || (keyLen >= 8))
                        {
                            MPU_WLAN_SetConnection(&stWlanConCfg);
                        }
                        else
                        {
                            //提示扫码完成后
                            ZWSys_SetSpeechContent(SPEECH_WIFI_CONNECTING);
                            //循环查询，等待播放结束
                            i = 0;
                            while (1)
                            {
                                SelectSleepMSec(100);
                                if (eSpeechPlayIdle == ZWSys_GetSpeechPlayingStatus())
                                {
                                    break;
                                }
                                if (++i > (SPEECH_PLAY_TIME_OUT_SEC*10))
                                {
                                    i = 0;
                                    break;
                                }
                            }

                            //提示扫码完成后
                            ZWSys_SetSpeechContent(SPEECH_WIFI_CONNECT_FAIL_PWD_WRONG);
                            //循环查询，等待播放结束
                            i = 0;
                            while (1)
                            {
                                SelectSleepMSec(100);
                                if (eSpeechPlayIdle == ZWSys_GetSpeechPlayingStatus())
                                {
                                    break;
                                }
                                if (++i > (SPEECH_PLAY_TIME_OUT_SEC*10))
                                {
                                    i = 0;
                                    break;
                                }
                            }

                            //提示扫码完成后
                            ZWSys_SetSpeechContent(SPEECH_WIFI_PLEASE_CONNECT);
                            //循环查询，等待播放结束
                            i = 0;
                            while (1)
                            {
                                SelectSleepMSec(100);
                                if (eSpeechPlayIdle == ZWSys_GetSpeechPlayingStatus())
                                {
                                    break;
                                }
                                if (++i > (SPEECH_PLAY_TIME_OUT_SEC*10))
                                {
                                    i = 0;
                                    break;
                                }
                            }
                        }
                    }
                }
            }

            //配网处理结束后，清空本次数据，为下次配网做准备
            memset(szWlanSSID, 0, sizeof(szWlanSSID));
            memset(szWlanKey, 0, sizeof(szWlanKey));
            bIsFindWlanSSID = 0;
            bIsFindWlanKey = 0;
        }
        else
        {
            usleep(100000);
        }
    }

    return NULL;
}

//蓝牙请求命令返回线程
static void *Fnx_BlueToothReqRetThread(void *lParam)
{
    int i = 0;
    XRSBLE_L2CmdDataSet stCmdDataSet = {0};
    XRSBLE_SendCtrlParam stSendCtlParam = {0};
    XRSBLE_U16 u16SeqId = 0;
    unsigned char bIsNeedSend = 0;
    unsigned char *pBufTmp = NULL;
    pthread_detach(pthread_self());

    PRT_TID();

    //注意此块内存大小，需要可容纳下面使用时的各种结构体大小
    pBufTmp = (unsigned char*)malloc(1024);
    if (NULL == pBufTmp)
    {
        printf("\n[%s][%d]malloc failed!!!\n",
            __FUNCTION__, __LINE__);
        return NULL;
    }

    while (1)
    {
        //判断是否需要退出
        if (s_bIsExitBtReqRetTh)
        {
            break;
        }

        if (sem_trywait(&semBtSendData) != 0 )
        {
            usleep(100*1000);
            continue;
        }

        pthread_mutex_lock(&s_muBtSendDataLock);
        stCmdDataSet = s_stCmdDataSet;
        //取走后清空
        memset(&s_stCmdDataSet, 0, sizeof(s_stCmdDataSet));
        pthread_mutex_unlock(&s_muBtSendDataLock);

        bIsNeedSend = 0;
        if (stCmdDataSet.u8CmdCnt > 0)
        {
            if (CMD_ID_GET_INFO == stCmdDataSet.stCmdData[0].u8CmdId)
            {
                if (KEY_ID_PROTOC_VER_REPLY == stCmdDataSet.stCmdData[0].u8KeyId)
                {
                    XRSBLE_ProtocVerInfo *pProtocVerInfo = (XRSBLE_ProtocVerInfo *)pBufTmp;
                    XRSBLE_ProtocolVerGet(&(pProtocVerInfo->u8BleProtocVer));
                    stCmdDataSet.stCmdData[0].u8DateType = 0;
                    stCmdDataSet.stCmdData[0].pData = (void *)pProtocVerInfo;
                    stCmdDataSet.stCmdData[0].u16DataLen = sizeof(XRSBLE_ProtocVerInfo);

                    stSendCtlParam.u32TimeOutMsec = 1000;
                    stSendCtlParam.u8ReTryCnt = 3;
                    bIsNeedSend = 1;
                }
                else if (KEY_ID_IS_5G_WIFI_REPLY == stCmdDataSet.stCmdData[0].u8KeyId)
                {
                    XRSBLE_Is5GWifi *pIs5GWifi = (XRSBLE_Is5GWifi *)pBufTmp;
					if(4 == MPU_Net_GetWiFiUSBWifiDev())
					{
						pIs5GWifi->u8BleIs5GWifi = 0 ;
					}
					else
					{
						pIs5GWifi->u8BleIs5GWifi = 1 ;
					}
						
                    stCmdDataSet.stCmdData[0].u8DateType = 0;
                    stCmdDataSet.stCmdData[0].pData = (void *)pIs5GWifi;
                    stCmdDataSet.stCmdData[0].u16DataLen = sizeof(XRSBLE_Is5GWifi);

                    stSendCtlParam.u32TimeOutMsec = 1000;
                    stSendCtlParam.u8ReTryCnt = 3;
                    bIsNeedSend = 1;
                }
            }
        }

        if (bIsNeedSend)
        {
            XRSBLE_L2_SendData(&stCmdDataSet, &stSendCtlParam, &u16SeqId);
            printf("\n[FAN-BLE][%s][%d]u8CmdId=%u u8KeyId=%u u16SeqId=%u\n",
                __FUNCTION__, __LINE__,
                stCmdDataSet.stCmdData[0].u8CmdId,
                stCmdDataSet.stCmdData[0].u8KeyId, u16SeqId);
        }
    }

    if (NULL != pBufTmp)
    {
        free(pBufTmp);
        pBufTmp = NULL;
    }

    return NULL;
}

//蓝牙命令处理-SetInfo
static int BlueToothCmdProcess_SetInfo(XRSBLE_L2CmdData *pCmdData)
{
    int nRet = 0;
    char szStrTmp[256] = {0};
    char szStrTmp2[256] = {0};

    if (NULL == pCmdData)
    {
        return -1;
    }

    switch (pCmdData->u8KeyId)
    {
        case KEY_ID_WIFI_SSID:
        {
            if (NULL == pCmdData->pData)
            {
                nRet = IDVR_CIU_STRUCT_UNVALID;
                printf("pCmdData->pData=NULL\n");
                break;
            }
            if (pCmdData->u16DataLen < sizeof(XRSBLE_WifiSsidInfo))
            {
                nRet = IDVR_CIU_BUF_SMALL;
                printf("buffer is too small, need %d, real %lu\n",
                    sizeof(XRSBLE_WifiSsidInfo), pCmdData->u16DataLen);
                break;
            }

            //printf("\n[FAN-BLE][%s][%d]u8KeyId=%u\n",
            //    __FUNCTION__, __LINE__,
            //    pCmdData->u8KeyId);

            XRSBLE_WifiSsidInfo *pWifiSsidInfo = (XRSBLE_WifiSsidInfo*)pCmdData->pData;

            memset(szStrTmp, 0, sizeof(szStrTmp));
            memset(szStrTmp2, 0, sizeof(szStrTmp2));
            snprintf(szStrTmp, sizeof(szStrTmp), "%s", pWifiSsidInfo->szWifiSsidBase64);
            //printf("\n[FAN-BLE][%s][%d]szStrTmp=%s\n",
            //    __FUNCTION__, __LINE__,
            //    szStrTmp);
            base64Decode(szStrTmp, (unsigned char *)szStrTmp2);
            //printf("\n[FAN-BLE][%s][%d]szStrTmp2=%s\n",
            //    __FUNCTION__, __LINE__,
            //    szStrTmp2);
            pthread_mutex_lock(&s_muBtCfgNetDataLock);
            memset(s_szWlanSSID, 0, sizeof(s_szWlanSSID));
            snprintf(s_szWlanSSID, sizeof(s_szWlanSSID), "%s", szStrTmp2);
            s_bIsFindWlanSSID = 1;
            //printf("\n[FAN-BLE][%s][%d]s_szWlanSSID=%s\n",
            //    __FUNCTION__, __LINE__, s_szWlanSSID);
            pthread_mutex_unlock(&s_muBtCfgNetDataLock);

            nRet = 0;
            break;
        }
        case KEY_ID_WIFI_PWD:
        {
            if (NULL == pCmdData->pData)
            {
                nRet = IDVR_CIU_STRUCT_UNVALID;
                printf("pCmdData->pData=NULL\n");
                break;
            }
            if (pCmdData->u16DataLen < sizeof(XRSBLE_WifiPwdInfo))
            {
                nRet = IDVR_CIU_BUF_SMALL;
                printf("buffer is too small, need %d, real %lu\n",
                    sizeof(XRSBLE_WifiPwdInfo), pCmdData->u16DataLen);
                break;
            }

            XRSBLE_WifiPwdInfo *pWifiPwdInfo = (XRSBLE_WifiPwdInfo*)pCmdData->pData;

            memset(szStrTmp, 0, sizeof(szStrTmp));
            memset(szStrTmp2, 0, sizeof(szStrTmp2));
            snprintf(szStrTmp, sizeof(szStrTmp), "%s", pWifiPwdInfo->szWifiPwdBase64);
            base64Decode(szStrTmp, (unsigned char *)szStrTmp2);

            pthread_mutex_lock(&s_muBtCfgNetDataLock);
            memset(s_szWlanKey, 0, sizeof(s_szWlanKey));
            snprintf(s_szWlanKey, sizeof(s_szWlanKey), "%s", szStrTmp2);
            s_bIsFindWlanKey = 1;
            pthread_mutex_unlock(&s_muBtCfgNetDataLock);

            //printf("\n[FAN-BLE][%s][%d]s_szWlanKey=%s\n",
            //    __FUNCTION__, __LINE__, s_szWlanKey);

            nRet = 0;
            break;
        }
        case KEY_ID_TIMEZONE:
        {
            if (NULL == pCmdData->pData)
            {
                nRet = IDVR_CIU_STRUCT_UNVALID;
                printf("pCmdData->pData=NULL\n");
                break;
            }
            if (pCmdData->u16DataLen < sizeof(XRSBLE_TimeZoneInfo))
            {
                nRet = IDVR_CIU_BUF_SMALL;
                printf("buffer is too small, need %d, real %lu\n",
                    sizeof(XRSBLE_TimeZoneInfo), pCmdData->u16DataLen);
                break;
            }

            XRSBLE_TimeZoneInfo *pTimeZoneInfo = (XRSBLE_TimeZoneInfo*)pCmdData->pData;
            //printf("\n[FAN-BLE][%s][%d]s16TimeZoneMin=%d\n",
            //    __FUNCTION__, __LINE__,
            //    pTimeZoneInfo->s16TimeZoneMin);
            //设置时区
            nRet = MPU_SYS_SetTimeZone((int)pTimeZoneInfo->s16TimeZoneMin);
            if (0 != nRet)
            {
                //操作失败
                nRet = 1;
            }
            else
            {
                //操作失败
                nRet = 0;
            }

            break;
        }
        default:
        {
            //不支持的KeyId
            nRet = IDVR_CIU_UNSUPPORTED_MINORTYPE;
            break;
        }
    }

    return nRet;
}

//蓝牙命令处理-GetInfo
static int BlueToothCmdProcess_GetInfo(XRSBLE_L2CmdData *pCmdData)
{
    int nRet = 0;
    char szStrTmp[256] = {0};
    char szStrTmp2[256] = {0};

    if (NULL == pCmdData)
    {
        return -1;
    }

    switch (pCmdData->u8KeyId)
    {
        case KEY_ID_PROTOC_VER_REQ:
        {
            //填充需要发送的主子命令，发送线程自行补充其他数据
            pthread_mutex_lock(&s_muBtSendDataLock);
            s_stCmdDataSet.u8CmdCnt = 1;
            s_stCmdDataSet.stCmdData[0].u8CmdId = CMD_ID_GET_INFO;
            s_stCmdDataSet.stCmdData[0].u8KeyId = KEY_ID_PROTOC_VER_REPLY;
            pthread_mutex_unlock(&s_muBtSendDataLock);
            //通知数据发送线程，对此请求响应返回
            sem_post(&semBtSendData);

            nRet = 0;
            break;
        }
        case KEY_ID_PROTOC_VER_REPLY:
        {
            if (NULL == pCmdData->pData)
            {
                nRet = IDVR_CIU_STRUCT_UNVALID;
                printf("pCmdData->pData=NULL\n");
                break;
            }
            if (pCmdData->u16DataLen < sizeof(XRSBLE_ProtocVerInfo))
            {
                nRet = IDVR_CIU_BUF_SMALL;
                printf("buffer is too small, need %d, real %lu\n",
                    sizeof(XRSBLE_ProtocVerInfo), pCmdData->u16DataLen);
                break;
            }

            XRSBLE_ProtocVerInfo *pProtocVerInfo = (XRSBLE_ProtocVerInfo*)pCmdData->pData;
            printf("Remote device BleProtocVer=%d\n", pProtocVerInfo->u8BleProtocVer);

            nRet = 0;
            break;
        }
        case KEY_ID_IS_5G_WIFI_REQ:
        {
            //填充需要发送的主子命令，发送线程自行补充其他数据
            pthread_mutex_lock(&s_muBtSendDataLock);
            s_stCmdDataSet.u8CmdCnt = 1;
            s_stCmdDataSet.stCmdData[0].u8CmdId = CMD_ID_GET_INFO;
            s_stCmdDataSet.stCmdData[0].u8KeyId = KEY_ID_IS_5G_WIFI_REPLY;
            pthread_mutex_unlock(&s_muBtSendDataLock);
            //通知数据发送线程，对此请求响应返回
            sem_post(&semBtSendData);

            nRet = 0;
            break;
        }
        case KEY_ID_IS_5G_WIFI_REPLY:
        {
            if (NULL == pCmdData->pData)
            {
                nRet = IDVR_CIU_STRUCT_UNVALID;
                printf("pCmdData->pData=NULL\n");
                break;
            }
            if (pCmdData->u16DataLen < sizeof(XRSBLE_Is5GWifi))
            {
                nRet = IDVR_CIU_BUF_SMALL;
                printf("buffer is too small, need %d, real %lu\n",
                    sizeof(XRSBLE_Is5GWifi), pCmdData->u16DataLen);
                break;
            }

            XRSBLE_Is5GWifi *pIs5GWifi = (XRSBLE_Is5GWifi*)pCmdData->pData;
            printf("Remote device BleIs5GWifi=%d\n", pIs5GWifi->u8BleIs5GWifi);

            nRet = 0;
            break;
        }
        default:
        {
            //不支持的KeyId
            nRet = IDVR_CIU_UNSUPPORTED_MINORTYPE;
            break;
        }
    }

    return nRet;
}

//蓝牙收数据回调
//注意: 不可在此接口内部调用XRSBLE协议库中其他接口，否则将死锁
static int AppRecvDataCallback(XRSBLE_L2CmdData *pCmdData)
{
    int nRet = 0;

    if (NULL == pCmdData)
    {
        return -1;
    }

    //printf("\n[FAN-BLE][%s][%d]pData=%#x u16DataLen=%u u8CmdId=%u u8KeyId=%u u8DateType=%u\n",
    //    __FUNCTION__, __LINE__,
    //    pCmdData->pData, pCmdData->u16DataLen,
    //    pCmdData->u8CmdId, pCmdData->u8KeyId,
    //    pCmdData->u8DateType);

    if (1 == pCmdData->u8DateType)
    {
        //暂时没有确认操作返回值的需要
        //对于应答数据直接丢弃
        return 0;
    }

    switch (pCmdData->u8CmdId)
    {
        case CMD_ID_SET_INFO:
        {
            nRet = BlueToothCmdProcess_SetInfo(pCmdData);
            break;
        }
        case CMD_ID_GET_INFO:
        {
            nRet = BlueToothCmdProcess_GetInfo(pCmdData);
            break;
        }
        default:
        {
            //不支持的CmdId
            nRet = IDVR_CIU_UNSUPPORTED_MAJORTYPE;
            break;
        }
    }

    return nRet;
}
#endif

//检测清除启动次数计数
static void CheckClearRestartTimes()
{
    static int nCheckCount = 0;
    static int nExeFlag = 0;

    if (1 == nExeFlag)
    {
        return;
    }

    if (0 == access(PRODUCTTEST_FILE,F_OK))
    {//存在产测文件，清除计数
        nExeFlag = 1;
        mysystem(RESTART_TIMES);
        mysystem(SAVE_ENV);

        MAIN_LOG_W("PRODUCTTEST_FILE clean RESTART_TIMES\n");
        return;
    }

    nCheckCount++;
    if (nCheckCount >= 5)
    {//启动5分钟后，清除计数
        nExeFlag = 1;
        mysystem(RESTART_TIMES);
        mysystem(SAVE_ENV);
        MAIN_LOG_W("MAX_CHECK_TIME clean RESTART_TIMES\n");
    }
}

static void sigstop(int signo)
{
    int n = 0;
    char buf[128];

    if (signo == SIGSEGV) {
        n = snprintf(buf, sizeof(buf),"[Error] Segmentation fault detected! Signal:%d\n", signo);
    } else {
        n = snprintf(buf, sizeof(buf),"[Error] Signal %d caught, aborting.\n", signo);
    }
    (void)write(STDERR_FILENO, buf, n);   // write() 是异步信号安全的

    _exit(1);
}

static void init_signal_handler()
{
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));

    // SIGPIPE: 忽略
    sa.sa_handler = SIG_IGN;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGPIPE, &sa, NULL);

    // SIGALRM: 保留SA_RESTART,避免超时机制产生的SIGALRM中断系统调用
    sa.sa_handler = SIG_DFL;
    sa.sa_flags = SA_RESTART;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGALRM, &sa, NULL);

    // SIGINT/SIGQUIT/SIGTERM: 优雅退出，统一用sigstop
    sa.sa_handler = sigstop;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    sigaddset(&sa.sa_mask, SIGTERM);
    sigaddset(&sa.sa_mask, SIGQUIT);
    sigaction(SIGINT, &sa, NULL);

    sigemptyset(&sa.sa_mask);
    sigaddset(&sa.sa_mask, SIGINT);
    sigaddset(&sa.sa_mask, SIGQUIT);
    sigaction(SIGTERM, &sa, NULL);

    sigemptyset(&sa.sa_mask);
    sigaddset(&sa.sa_mask, SIGINT);
    sigaddset(&sa.sa_mask, SIGTERM);
    sigaction(SIGQUIT, &sa, NULL);

    // SIGSEGV: 单独处理，极简且安全
    sa.sa_handler = sigstop;
    sa.sa_flags = SA_RESETHAND;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGSEGV, &sa, NULL);
}


// 去除字符串首尾空白
static void trim(char *str) {
    char *start = str;
    char *end;
    while (isspace((unsigned char)*start)) start++;
    if (*start == 0) {
        str[0] = '\0';
        return;
    }
    end = start + strlen(start) - 1;
    while (end > start && isspace((unsigned char)*end)) end--;
    *(end + 1) = '\0';
    if (start != str) memmove(str, start, strlen(start) + 1);
}

// 转为小写
static void to_lower(char *dst, const char *src) {
    while (*src) {
        *dst++ = tolower((unsigned char)*src++);
    }
    *dst = '\0';
}

/**
 * 从hosts文件中查找指定域名对应的IP地址
 * @param hosts_path   hosts文件路径，例如 "/etc/hosts" 或 "C:\\Windows\\System32\\drivers\\etc\\hosts"
 * @param domain       要查找的域名（不区分大小写）
 * @param ip_buffer    IP地址的缓冲区
 * @return             1 表示找到域名，IP；0 表示未找到或出错
 */
int lookup_hosts(const char *hosts_path, const char *domain,const char *ipaddr) {
    FILE *fp = fopen(hosts_path, "r");
    if (!fp) return 0;

    char line[512] = {0};
    char lower_domain[256] = {0};
    to_lower(lower_domain, domain);

    int found = 0;

    while (fgets(line, sizeof(line), fp)) {
        // 跳过空行和注释行
        char *p = line;
        while (isspace((unsigned char)*p)) p++;
        if (*p == '#' || *p == '\0') continue;

        // 去掉行内注释
        char *comment = strchr(line, '#');
        if (comment) *comment = '\0';

        // 分离IP和域名列表
        char ip[128] = {0};
        char rest[512] = {0};
        if (sscanf(line, "%127s %511[^\n]", ip, rest) != 2) continue;

        // 解析域名列表（空格/制表符分隔）
        char *token;
        char *saveptr;
        char rest_copy[512] = {0};

		strncpy(rest_copy, rest, sizeof(rest_copy) - 1);
		rest_copy[sizeof(rest_copy) - 1] = '\0';
		
        token = strtok_r(rest_copy, " \t", &saveptr);
        while (token != NULL) {
            trim(token);
            char lower_token[256] = {0};
            to_lower(lower_token, token);
            if (strcmp(lower_token, lower_domain) == 0) {
                // 找到匹配的域名
                if(strncmp(ipaddr, ip, strlen(ipaddr)) == 0)
                {
                	found = 1;
                	break;
                }
            }
            token = strtok_r(NULL, " \t", &saveptr);
        }
        if (found) break;
    }

    fclose(fp);
    return found;
}

//产测模式下对于YLTB号段，则需要修正hosts文件
int CheckHostsConf(void)
{
	int iCurRet1 = 0;
	int iCurRet2 = 0;
	int iCurRet3 = 0;
	int iIsNeedAmendHosts = 0;
	char cmd_string[128];
	stuShmAuthInfo stShmAuthInfo = {0};

	//是否是产测模式
	if (0 == access("/tmp/ProductTest", 0))
	{
		//是否是YLTB号段
    	Authorize_GetAuthDevInfo(&stShmAuthInfo);
		MAIN_LOG_I("dwAuthVerifyErrno:%d, szDid:%s\n",stShmAuthInfo.dwAuthVerifyErrno,stShmAuthInfo.struCloudInfo.szDid);
		if(AUTH_SUCCESS == stShmAuthInfo.dwAuthVerifyErrno)
		{
			if(0 == strncmp((const char*)stShmAuthInfo.struCloudInfo.szDid, "YLTB", 4))
			{
				iIsNeedAmendHosts = 1;
			}
			else
			{
				iIsNeedAmendHosts = 0;
			}

		}

	}
	else
	{
		iIsNeedAmendHosts = 0;
	}

	MAIN_LOG_I("iIsNeedAmendHosts:%d\n",iIsNeedAmendHosts);
	
	if(iIsNeedAmendHosts)
	{
		iCurRet1 = lookup_hosts(HOSTS_CONFIG_ETC_FILE, "ruisionvps1.com", "106.52.167.241");
		iCurRet2 = lookup_hosts(HOSTS_CONFIG_ETC_FILE, "ruisionvps2.com", "106.52.167.241");
		iCurRet3 = lookup_hosts(HOSTS_CONFIG_ETC_FILE, "ruisionvps3.com", "106.52.167.241");
		if(iCurRet1 != 1 || iCurRet2 != 1 || iCurRet3 != 1)
		{
			//只要有一个不匹配，则使用YLTB HOSTS覆盖，YLTB HOSTS文件这三个都需要修改
			if (0 == access(YLTB_HOSTS_CONFIG_ETC_FILE, 0))
			{
				memset(cmd_string,0,sizeof(cmd_string));
				snprintf(cmd_string,sizeof(cmd_string)-1,"cp %s %s -rf",YLTB_HOSTS_CONFIG_ETC_FILE, HOSTS_CONFIG_ETC_FILE);
				mysystem(cmd_string);
			}
		}
	}
	else
	{
		iCurRet1 = lookup_hosts(HOSTS_CONFIG_ETC_FILE, "ruisionvps1.com", "106.52.167.241");
		iCurRet2 = lookup_hosts(HOSTS_CONFIG_ETC_FILE, "ruisionvps2.com", "106.52.167.241");
		iCurRet3 = lookup_hosts(HOSTS_CONFIG_ETC_FILE, "ruisionvps3.com", "106.52.167.241");
		if(iCurRet1 == 1 || iCurRet2 == 1 || iCurRet3 == 1)
		{
			//只要有一个匹配上了，则需要恢复为NORMAL HOSTS
			if (0 == access(NORMAL_HOSTS_CONFIG_ETC_FILE, 0))
			{
				memset(cmd_string,0,sizeof(cmd_string));
				snprintf(cmd_string,sizeof(cmd_string)-1,"cp %s %s -rf",NORMAL_HOSTS_CONFIG_ETC_FILE, HOSTS_CONFIG_ETC_FILE);
				mysystem(cmd_string);
			}
		}
	}

	return 0;
}



int main(void)
{
    unsigned long cnt = 0;

    init_signal_handler();

    sem_init(&m_semStartRebootOpsThread, 0, 0);

#if defined (__SUPPORT_PRODUCT_TEST_CUSTOM_SPEECH_FILES__)
    CheckSDUpdateAllAudioFiles();
#endif
    CheckSDUpdateCustomMode();

    InitLicenseAuthorize();

	CheckHostsConf();

#if defined(__STATIC_LINK_STDC__)
    CheckUpdateFile();
#endif

    if (check_protocol_type() == 0)
    {
        MAIN_LOG_I("Load P2P Protocol successful.\n");
        //mysystem("ln -s /mnt/mtd/http/Protocol.html /home/http/Protocol.html");
    }
    /* init logvel */
    init_log_level();

	SAFE_UNLINK(P2P_SEND_DATA_FAIL_FLAG);

    //默认不启用，需要用时，0改成1即可
    XTrace_Init(1, 10*1000, 1000);

    CustommodeRestore();

    CheckCustomFile();
    MPU_PAR_CustomFileDate_Replace(CUSTOM_CONFIGMODE_FILE, 0);

    //MAIN_LOG_I("\n----Gui_main.c---------main--------------------1-------MPU_PAR_Init---------\n");
    MPU_PAR_Init();

    ZWSys_SysInitSensorCnt();

    modifyDeviceCap();

    //MAIN_LOG_I("\n----Gui_main.c---------main--------------------4--------MPU_Init();---------\n");
    MPU_Init();

    CreateStartFlag();
    MPU_OPL_AddLog(EDVR_EVENT_OPT, EDVR_EVENT_OPT_DETAIL_POWERON, 0, 0, 0, NULL);

#ifdef __MODULE_4G_SUPPORT__
    Module4G_SetCloudCardCapCallback(SetCloudCardCapCallback);
#endif

    //MAIN_LOG_I("\n----Gui_main.c---------main----------------10------MPU_Init2()-------\n");
    MPU_Init2();

    //MAIN_LOG_I("\n----Gui_main.c---------main--------------------11-------InitDeviceCapacity----------\n");
    InitDeviceCapacity();

    //MAIN_LOG_I("\n----Gui_main.c---------main--------------------12--------Osd_Init()---------\n");
    Osd_Init();

#ifdef __IS_SUPPORT_AIC_BLUETOOTH__
        //设置应用层蓝牙数据接收回调接口
        MPU_NET_SetAppBlueToothRecvCallback(AppRecvDataCallback);
        sem_init(&semBtSendData, 0, 0);
        s_bIsExitBtReqRetTh = 0;
        s_bIsExitBtCfgNetTh = 0;
#endif
    MAIN_LOG_I("\n----Gui_main.c---------main--------------------12--------MPU_NET_SetAppBlueToothRecvCallback---------\n");

    SetCallBack();

    SystemStartThreadManager();

    FullWithTheXMLFile();

    RShellCmdInit();
    /* 设置设备正常启动标志 */
    SetSysStartTag(0);

#ifdef __AGINGTEST__
    //老化测试
    init_aging_test();
#endif
    SetDevInitOverFlag(1);
    StartSystemStatusLed();

#if defined (__IR_CUT_CONTROL_IMPROVE__)
    if (0 == access(CUSTOM_MODE_FLAG, F_OK))
    {
        //定制标记需要等所有功能初始化完成后删除!
        char command[64] = {0};
        memset(command, 0, sizeof(command));
        snprintf(command, sizeof(command), "rm  %s", CUSTOM_MODE_FLAG);
        mysystem(command);
    }
#endif

    while(1)
    {
        sleep(60);

        CheckClearRestartTimes();

        /* 暂定1小时清理下系统缓存 */
        if (((cnt++) % 60) == 0)
        {
            mysystem(CMD_DROP_CACHE_2);
        }
    }

    MPU_Uninit();

    return 0;
}
