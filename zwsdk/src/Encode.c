#include <stdlib.h>

#include "Init.h"			//初始化
#include "EncoderLib.h"		//编码实现
#include "Common.h"			//公用定义，包括资源控制
#include "xtrace.h"
#include "halmod.h"
#include "Zoom.h"
#include "hisdk.h"
#include "common_pub.h"

//蓝屏帧
static unsigned char s_blueIFrame[]=
{
    0x0, 0x0, 0x0, 0x1, 0x67, 0x42, 0x0, 0x14, 0x95,
    0xa8, 0x58, 0x7e, 0x40, 0x0, 0x0, 0x0, 0x1, 0x68, 0xce,
    0x3c, 0x80, 0x0, 0x0, 0x0, 0x1, 0x6, 0xe5, 0x1, 0xc5,
    0x80, 0x0, 0x0, 0x0, 0x1, 0x65, 0xb8, 0x0, 0x0, 0x9,
    0x8b, 0x90, 0x10, 0xff, 0xff, 0xfc, 0x11, 0x45, 0x0, 0x1,
    0x13, 0xff, 0x1c, 0x0, 0x4, 0xd5, 0xa3, 0x80, 0x0, 0x83,
    0x7d, 0xf7, 0xdf, 0x7d, 0xf7, 0xdf, 0x7d, 0xf7, 0xdf, 0x7d,
    0xf7, 0xdf, 0x7d, 0xf7, 0xdf, 0x7d, 0xf5, 0xd7, 0x5d, 0x75,
    0xd7, 0x5d, 0x75, 0xd7, 0x5d, 0x75, 0xd7, 0x5d, 0x75, 0xd7,
    0x5d, 0x75, 0xd7, 0x5d, 0x75, 0xd7, 0x5d, 0x75, 0xd7, 0x5d,
    0x75, 0xd7, 0x5d, 0x75, 0xd7, 0x5d, 0x75, 0xd7, 0x5d, 0x75,
    0xd7, 0x5d, 0x75, 0xd7, 0x5d, 0x75, 0xd7, 0x5d, 0x75, 0xd7,
    0x5d, 0x75, 0xd7, 0x5d, 0x75, 0xd7, 0x5d, 0x75, 0xd7, 0x5d,
    0x75, 0xd7, 0x5d, 0x75, 0xd7, 0x5d, 0x75, 0xd7, 0x5d, 0x75,
    0xd7, 0x5d, 0x75, 0xd7, 0x5d, 0x75, 0xd7, 0x5d, 0x75, 0xd7,
    0x5d, 0x75, 0xd7, 0x5d, 0x75, 0xd7, 0x5d, 0x75, 0xd7, 0x5d,
    0x75, 0xd7, 0x5d, 0x75, 0xd7, 0x5d, 0x75, 0xd7, 0x5d, 0x75,
    0xd7, 0x5d, 0x75, 0xd7, 0x5d, 0x75, 0xd7, 0x5d, 0x75, 0xd7,
    0x5d, 0x75, 0xd7, 0x5d, 0x75, 0xd7, 0x5d, 0x75, 0xd7, 0x5d,
    0x75, 0xd7, 0x5d, 0x75, 0xd7, 0x5d, 0x75, 0xd7, 0x5d, 0x75,
    0xd7, 0x5d, 0x75, 0xd7, 0x5d, 0x75, 0xd7, 0x5d, 0x75, 0xd7,
    0x5d, 0x75, 0xd7, 0x5d, 0x75, 0xd7, 0x5d, 0x75, 0xd7, 0x5d,
    0x75, 0xd7, 0x5d, 0x75, 0xd7, 0x5d, 0x75, 0xd7, 0x5d, 0x75,
    0xd7, 0x5d, 0x75, 0xd7, 0x5d, 0x75, 0xd7, 0x5d, 0x75, 0xd7,
    0x5d, 0x75, 0xd7, 0x5d, 0x75, 0xd7, 0x5d, 0x75, 0xd7, 0x5d,
    0x75, 0xd7, 0x5d, 0x75, 0xd7, 0x5d, 0x75, 0xd7, 0x5d, 0x75,
    0xd7, 0x5d, 0x75, 0xd7, 0x5d, 0x75, 0xd7, 0x5d, 0x75, 0xd7,
    0x5d, 0x75, 0xd7, 0x5d, 0x75, 0xd7, 0x5d, 0x75, 0xd7, 0x5d,
    0x75, 0xd7, 0x5d, 0x75, 0xd7, 0x5d, 0x75, 0xd7, 0x5d, 0x75,
    0xd7, 0x5d, 0x75, 0xd7, 0x5d, 0x75, 0xd7, 0x5d, 0x75, 0xd7,
    0x5d, 0x75, 0xd7, 0x5d, 0x75, 0xd7, 0x5d, 0x78
};

#if (MAX_CH_NUM == 1)
static unsigned char *s_pBuleScreenVideoBuffer[SUPPORT_MAX_SENSOR_COUNT] = {NULL};
#elif (MAX_CH_NUM == 2)
static unsigned char *s_pBuleScreenVideoBuffer[SUPPORT_MAX_SENSOR_COUNT] = {NULL, NULL};
#elif (MAX_CH_NUM == 3)
static unsigned char *s_pBuleScreenVideoBuffer[SUPPORT_MAX_SENSOR_COUNT] = {NULL, NULL, NULL};
#elif (MAX_CH_NUM == 4)
static unsigned char *s_pBuleScreenVideoBuffer[SUPPORT_MAX_SENSOR_COUNT] = {NULL, NULL, NULL, NULL};
#endif

//-----------------------------------------------------------------------------
//	说明:	初始化系统,该函数必须在应用软件启动时首先调用；
//  具体流程如下：
//	<1>		完成系统SDK需要的编码通道句柄与解码通道句柄各种初始化工作。
//  <2>		初始化海思的SDK内存
//  <3>		初始化VI、VPSS、VENC模块
//  <4>		开辟写文件线程线程。
//	返回:	成功为编解码通道总和，其它为0
//-----------------------------------------------------------------------------

int     MP4Sys_InitDSPs(void)
{
    ZW_INFO("--------into MP4Sys_InitDSPs--------\n");

    char Info[64] = {0};
    SDK_ResInfo_t   *pSDK_Info = NULL;
    SystemInfo_t    *pSysInfo = NULL;

    struct tm tnow_r;
    time_t CurTime;

    time(&CurTime);
    localtime_r(&CurTime, &tnow_r);
    ZW_INFO("Current Time: %04d-%02d-%02d %02d:%02d:%02d\n",
            (1900 + (int)tnow_r.tm_year),
            (int)tnow_r.tm_mon + 1,
            (int)tnow_r.tm_mday,
            (int)tnow_r.tm_hour,
            (int)tnow_r.tm_min,
            (int)tnow_r.tm_sec);

    // 初始化全局变量
    HY_Res_InitSDKInfo();
    HY_Res_InitSystemInfo();
    HY_Enc_InitEncChInfo();
    HY_Enc_InitTalkingChInfo();

    HY_Res_GetSDKInfo(&pSDK_Info);
    HY_Res_GetSystemInfo(&pSysInfo);

    /* 防止用户多线程同时调用MP4Sys_DeInitDSPs和MP4Sys_InitDSPs，简单保护一下 */
    while (pSDK_Info->m_bSDKInitLock)
    {
        HY_Res_SDK_Usleep(1000000);
        ZW_ERROR("wait m_bSDKInitLock !!!!!!!!!!!\n");
    }

    pSDK_Info->m_bSDKInitLock = TRUE;
    if ((int)SDK_INIT_MAGIC == pSDK_Info->m_bSDKInit)
    {
        pSDK_Info->m_bSDKInitLock = FALSE;
        return HI_SDK_SUCESS;
    }

    Zview_Res_Init(Info);		//初始化资源信息,设置pSysInfo->m_VIChnWidth等参数

    HY_Res_RWLock_init(&pSDK_Info->m_Lock);

    /* 初始化配置信息 */
    /* SDK初始化 */
    if (0 != Zview_SDK_Init())
    {
        ZW_ERROR("Zview_SDK_Init() Fail !!!!!!!!!!!\n");
        goto MP4Sys_InitDSPs_Error0;
    }
    ZW_DEBUG("Zview_SDK_Init: OK !!!!!!!!!!!\n");

    /* 创建SDK需要的线程 */
    if (Zview_CreatThread() != 0)
    {
        ZW_ERROR("Zview_CreatThread Fail\n");
        goto MP4Sys_InitDSPs_Error1;
    }
    ZW_DEBUG("Zview_CreatThread: OK !!!!!!!!!!!\n");

    pSDK_Info->m_bSDKInit 		= SDK_INIT_MAGIC;
    pSDK_Info->m_bSDKInitLock 	= FALSE;

    ZW_DEBUG("Zview_SysInit OK\n");
    return HI_SDK_SUCESS;

MP4Sys_InitDSPs_Error1:
    Zview_ExitThread();

MP4Sys_InitDSPs_Error0:
    Zview_Res_DeInit();
    HY_Res_RWLock_destory(&pSDK_Info->m_Lock);
    pSDK_Info->m_bSDKInit 		= 0;
    pSDK_Info->m_bSDKInitLock 	= FALSE;
    ZW_ERROR("[SDK] MP4Sys_InitDSPs FAIL !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
    return ERR_SDK_INITFAIL;
}

//-----------------------------------------------------------------------------
//	说明:	关闭所有打开的编码通道和解码通道，释放占用的系统资源,该函数应在应用软件退出时调用；
//	返回:	成功为系统中可以使用的编码通道和解码通道总数，其它为定义的错误号；
//-----------------------------------------------------------------------------
int     MP4Sys_DeInitDSPs()
{
    SDK_ResInfo_t		*pSDK_Info = NULL;
    SystemInfo_t		*pSysInfo = NULL;
    ZW_INFO("--------into MP4Sys_DeInitDSPs--------\n");

    HY_Res_GetSDKInfo(&pSDK_Info);
    HY_Res_GetSystemInfo(&pSysInfo);

    /* 防止用户多线程同时调用MP4Sys_DeInitDSPs和MP4Sys_InitDSPs，简单保护一下 */
    while (pSDK_Info->m_bSDKInitLock)
    {
        HY_Res_SDK_Usleep(1000000);
        ZW_DEBUG("MP4Sys_DeInitDSPs: wait m_bSDKInitLock !!!!!!!!!!!\n");
    }
    pSDK_Info->m_bSDKInitLock = TRUE;
    if (pSDK_Info->m_bSDKInit != SDK_INIT_MAGIC)
    {
        pSDK_Info->m_bSDKInitLock	= FALSE;
        return ERR_SYS_BUSY;
    }

    /* 将资源上锁, 禁止所有接口调用 */
    pSDK_Info->m_bSDKInit = 0;

    /* 退出SDK创建的线程,确保SDK自身不会调用接口 */
    Zview_ExitThread();

    HY_Res_WR_Lock(&pSDK_Info->m_Lock);

    Zview_SDK_DeInit();

    Zview_Res_DeInit(); 			//释放全部资源

    HY_Res_WR_UnLock(&pSDK_Info->m_Lock);
    HY_Res_RWLock_destory(&pSDK_Info->m_Lock);
    pSDK_Info->m_bSDKInitLock	= FALSE;
    ZW_DEBUG("MP4Sys_DeInitDSPs OK !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");

    return HI_SDK_SUCESS;
}

int MP4Sys_FreeVMM()
{
    return 0;
}

int MP4Sys_RmMediaDrv()
{
    return 0;
}

//-----------------------------------------------------------------------------
//进入/退出升级模式(反初始化SDK，并启动蓝屏帧)
//-----------------------------------------------------------------------------
#define MAX_VMM_BLOCK_NUM		6

int MP4Sys_SwitchToUpgradeMode(int bInto, unsigned int *pu32PhyAddr, void **ppVirtAddr, unsigned int *puMmzAllocSize)
{
    int i = 0;
    int nRet = 0;
    SDK_ResInfo_t	*pSDK_Info = NULL;
    SystemInfo_t	*pSysInfo = NULL;
    unsigned long aMmzAllocTrySize[MAX_VMM_BLOCK_NUM] = {0};
    unsigned long  u32VmmStartAddr = 0;
    char sVmmStartAddr[32] = {0};
    unsigned long  u32VmmSize = 0;
    char s32VmmSize[32] = {0};
    char s32VmmUnit[4] = {0};
    char command[256] = {0};
    static unsigned long   s_u32PhyAddr = 0;
    static void 	*s_pVirtAddr = NULL;
    static unsigned int s_uMmzAllocSize = 0;
    ZW_INFO("--------into MP4Sys_SwitchToUpgradeMode----bInto=%d----\n", bInto);

    if (bInto && (NULL == pu32PhyAddr || NULL == ppVirtAddr))
    {
        return -1;
    }

    HY_Res_GetSDKInfo(&pSDK_Info);
    HY_Res_GetSystemInfo(&pSysInfo);

    if (bInto)
    {
        if (UpgradeMode == pSDK_Info->m_enSdkModeCur)
        {
            *pu32PhyAddr = s_u32PhyAddr;
            *ppVirtAddr = s_pVirtAddr;
            *puMmzAllocSize = s_uMmzAllocSize;
            return HI_SDK_SUCESS;
        }

        //1.反初始化SDK
        nRet = MP4Sys_DeInitDSPs();
        if (HI_SDK_SUCESS != nRet)
        {
            return -1;
        }

        //2.标志进入升级模式
        pSDK_Info->m_enSdkModeCur = UpgradeMode;

        // 移除驱动
        MP4Sys_RmMediaDrv();
        MP4Sys_FreeVMM();

        //3.启动蓝屏蓝屏帧线程
        if (0 == pSDK_Info->m_stBuleScreenThreadInfo.thThreadId)
        {
            pSDK_Info->m_stBuleScreenThreadInfo.bExit = 0;
            pthread_create(&(pSDK_Info->m_stBuleScreenThreadInfo.thThreadId),
                NULL, BuleScreenThread, &(pSDK_Info->m_stBuleScreenThreadInfo));
        }

        // 从load_media获取mmz起始地址
        ShellCmdOut("grep -E '^mmz_(start)=.*;' /usr/etc/load_media | awk -F'[=]' '{print $2 }' | awk -F'[;]' '{print $1 }' | xargs printf %d", sVmmStartAddr, sizeof(sVmmStartAddr));
        u32VmmStartAddr = atol(sVmmStartAddr);

        // 从load_media获取mmz单位
        ShellCmdOut("grep -E '^mmz_(size)=.*;' /usr/etc/load_media | awk -F'[=;]' '{print $2 }' | rev | cut -b 1", s32VmmUnit, sizeof(s32VmmUnit));

        // 从load_media获取mmz大小
        snprintf(command, sizeof(command), "grep -E '^mmz_(size)=.*;' /usr/etc/load_media | awk -F'[=]' '{print $2 }' | awk -F'[%c;]' '{print $1 }'", s32VmmUnit[0]);
        ShellCmdOut(command, s32VmmSize, sizeof(s32VmmSize));
        u32VmmSize = atol(s32VmmSize);

        ZW_INFO("u32VmmStartAddr=%#lx, u32VmmSize=%ld%s\n", u32VmmStartAddr, u32VmmSize, s32VmmUnit);

        for (i = 0; i < MAX_VMM_BLOCK_NUM; i++)
        {
            if (!strncmp(s32VmmUnit, "K", 1))
            {
                aMmzAllocTrySize[i] = u32VmmSize * 1024 - 2 * i * 1024 * 1024;
            }
            else if (!strncmp(s32VmmUnit, "M", 1))
            {
                aMmzAllocTrySize[i] = (u32VmmSize - 2 * i) * 1024 * 1024;
            }

            //4.申请MMZ内存块供存储升级包使用
            if (0 == s_u32PhyAddr && NULL == s_pVirtAddr)
            {
                ZW_INFO("memmap %#lx bytes for upgrade.\n", aMmzAllocTrySize[i]);
                s_pVirtAddr = memmap(u32VmmStartAddr, aMmzAllocTrySize[i]);
                if (s_pVirtAddr)
                {
                    s_u32PhyAddr = u32VmmStartAddr;
                    ZW_INFO("memmap size=%#lx s_u32PhyAddr=0x%lx\n", aMmzAllocTrySize[i], s_u32PhyAddr);
                    s_uMmzAllocSize = aMmzAllocTrySize[i];
                    break;
                }
                else
                {
                    ZW_INFO("fail memmap size=%#lx s_u32PhyAddr=0x%lx\n", aMmzAllocTrySize[i], s_u32PhyAddr);
                    s_u32PhyAddr = 0;
                    s_pVirtAddr = NULL;
                    s_uMmzAllocSize = 0;
                }
            }
        }

        *pu32PhyAddr = s_u32PhyAddr;
        *ppVirtAddr = s_pVirtAddr;
        *puMmzAllocSize = s_uMmzAllocSize;
    }
    else
    {
        if (NormalMode == pSDK_Info->m_enSdkModeCur)
        {
            return HI_SDK_SUCESS;
        }
        ZW_INFO("start unmap s_pVirtAddr=0x%p, s_uMmzAllocSize=0x%x\n", s_pVirtAddr, s_uMmzAllocSize);
        //1.释放申请的MMZ内存
        nRet = memunmap(s_pVirtAddr);
        //MI_SYS_MMA_Free(s_u32PhyAddr);
        if (0 != nRet)
        {
            ZW_ERROR("\nHI_MPI_SYS_MmzFree err! ret=%#x s_u32PhyAddr=%lx s_pVirtAddr=%p\n",
                nRet, s_u32PhyAddr, s_pVirtAddr);
        }
        else
        {
            s_u32PhyAddr = 0;
            s_pVirtAddr = NULL;
            s_uMmzAllocSize = 0;
        }
        //2.退出蓝屏线程
        if (0 != pSDK_Info->m_stBuleScreenThreadInfo.thThreadId)
        {
            pSDK_Info->m_stBuleScreenThreadInfo.bExit = 1;
            pthread_join(pSDK_Info->m_stBuleScreenThreadInfo.thThreadId, NULL);
            pSDK_Info->m_stBuleScreenThreadInfo.thThreadId = 0;
        }

        //3.释放蓝屏数据缓冲区
        for (int i=0; i < SUPPORT_MAX_SENSOR_COUNT; i++)
        {
            if (NULL != s_pBuleScreenVideoBuffer[i])
            {
                free(s_pBuleScreenVideoBuffer[i]);
                s_pBuleScreenVideoBuffer[i] = NULL;
            }
        }

        //4.标志退出升级模式
        pSDK_Info->m_enSdkModeCur = NormalMode;
    }

    return HI_SDK_SUCESS;
}

int     MP4Sys_GetBoardInfo(void)
{
    SystemInfo_t		*pSysInfo;
    SDK_ResInfo_t   *pSDK_Info = NULL;
    static int      s_nBoardInfoBak = 0;

    //	ZW_INFO("--------into MP4Sys_GetBoardInfo--------\n");

    HY_Res_GetSDKInfo(&pSDK_Info);

    //升级模式下pSysInfo内容已被清空
    if (UpgradeMode == pSDK_Info->m_enSdkModeCur)
    {
        return s_nBoardInfoBak;
    }

    if (HY_Res_SDK_Check() != 0)
    {
        ZW_ERROR("SDK Not Init\n");
        return ERR_SDK_NOINIT;
    }

    HY_Res_GetSystemInfo(&pSysInfo);
    s_nBoardInfoBak = pSysInfo->m_BoardInfo;
    return (pSysInfo->m_BoardInfo);
}

int 	MP4Sys_GetSensorType(BoardSensorType_t *SensorType)
{
//	ZW_INFO("=============into MP4Sys_GetSensorType===========\n");

    // if (HY_Res_SDK_Check() != 0)
    // {
    //     ZW_ERROR("SDK Not Init\n");
    //     return ERR_SDK_NOINIT;
    // }

    *SensorType = 0x1;

    return 0;
}

int     MP4Sys_GetTUTKUID(char *puid)
{
    SystemInfo_t		*pSysInfo;

    if (HY_Res_SDK_Check() != 0)
    {
        ZW_ERROR("SDK Not Init\n");
        return ERR_SDK_NOINIT;
    }
    if(NULL == puid)
    {
        ZW_DEBUG("%s: NULL == puid\n", __FUNCTION__);
        return ERR_SDK_NOINIT;
    }
    HY_Res_GetSystemInfo(&pSysInfo);
    memcpy(puid, pSysInfo->pTUTKUID, sizeof(pSysInfo->pTUTKUID));

    return 0;
}

int  MP4Sys_GetSerialNum(unsigned char *pszSerialNum)
{
    SystemInfo_t		*pSysInfo;
//	ZW_INFO("--------into MP4Sys_GetSerialNum--------\n");

    if (HY_Res_SDK_Check() != 0)
    {
        ZW_ERROR("SDK Not Init\n");
        return ERR_SDK_NOINIT;
    }

    HY_Res_GetSystemInfo(&pSysInfo);

    strncpy((char *)pszSerialNum, (char *)pSysInfo->m_szSerialNum, MAX_SERIAL_NUM_LEN);

    return 0;
}

/*@ brief MP4Sys_GetSecretPinStatus   获取复位管脚状态
 *@ param[out] *pResetStatus 引脚状态:0表示检测到复位信息；1表示未检测到复位信息
 *@ return 0 success; other fail
 */
int     MP4Sys_GetSecretPinStatus(int *pResetStatus)
{
#if 0//#ifdef SUPPORT_USB_WIFI
    *pResetStatus = 1;   //返回状态1,始终未检测到复位信息,否则IP将不断复位
    return 0;
#else
    int ret = 0;
    SystemInfo_t	   *pSysInfo;

//	ZW_INFO("--------into MP4Sys_GetSecretPinStatus--------\n");

    if (HY_Res_SDK_Check() != 0)
    {
        ZW_ERROR("SDK Not Init\n");
        return ERR_SDK_NOINIT;
    }
    HY_Res_GetSystemInfo(&pSysInfo);

    XTRACE_MUTEX_LOCK(&pSysInfo->m_SecretResetLock);
    //添加获取reset管角获取的代码，GPIO获取
    ret = HY_GetSecretPinStatus(pResetStatus);
    if(ret < 0)
    {
        ZW_ERROR("Get Secret Pin Status err\n");
    }
    XTRACE_MUTEX_UNLOCK(&pSysInfo->m_SecretResetLock);

    return ret;
#endif
}

#ifdef __ONE_CLICK_CALL__
int MP4Sys_GetCallPinStatus(int *pCallStatus, int *pHangupStatus)
{
    int ret = 0;
    if (HY_Res_SDK_Check() != 0)
    {
        ZW_ERROR("SDK Not Init\n");
        return ERR_SDK_NOINIT;
    }

    ret = HY_GetCallPinStatus(pCallStatus, pHangupStatus);
    if (ret < 0) {
        ZW_ERROR("Get Call Pin Status err\n");
    }

    return ret;
}

int MP4Sys_GetCallPinStatus_Test(int *pCallStatus, int *pHangupStatus)
{
    int ret = 0;
    if (HY_Res_SDK_Check() != 0)
    {
        ZW_ERROR("SDK Not Init\n");
        return ERR_SDK_NOINIT;
    }

    ret = HY_GetCallPinStatus_Test(pCallStatus, pHangupStatus);
    if (ret < 0) {
        ZW_ERROR("Get Call Pin Status err\n");
    }

    return ret;
}

int MP4Sys_SetCallPinStatus_Test(int pCallStatus, int pHangupStatus)
{
    int ret = 0;
    if (HY_Res_SDK_Check() != 0)
    {
        ZW_ERROR("SDK Not Init\n");
        return ERR_SDK_NOINIT;
    }

    ret = HY_SetCallPinStatus_Test(pCallStatus, pHangupStatus);
    if (ret < 0) {
        ZW_ERROR("Get Call Pin Status err\n");
    }

    return ret;
}


int MP4Sys_SetPauseAudioAlarm()
{
    /* 接口加锁 */
    if (HY_Res_SDK_Lock() != 0)
    {
        ZW_ERROR("SDK Not Init\n");
        return ERR_SDK_NOINIT;
    }
    HY_Enc_SetPauseAudioAlarm();

    HY_Res_SDK_UnLock();

    return 0;
}

int MP4Sys_SetAlternateResetButton(int enable)
{
    /* 接口加锁 */
    if (HY_Res_SDK_Lock() != 0)
    {
        ZW_ERROR("SDK Not Init\n");
        return ERR_SDK_NOINIT;
    }
    HY_Enc_SetAlternateResetButton(enable);

    HY_Res_SDK_UnLock();

    return 0;
}

int MP4Sys_SetRecoveryAudioAlarm()
{
    /* 接口加锁 */
    if (HY_Res_SDK_Lock() != 0)
    {
        ZW_ERROR("SDK Not Init\n");
        return ERR_SDK_NOINIT;
    }
    HY_Enc_SetRecoveryAudioAlarm();

    HY_Res_SDK_UnLock();

    return 0;
}

//-----------------------------------------------------------------------------
//	说明：		设置一键呼叫提示音打断标记
//	输入参数：	iState    0:不打断     1:立即打断     2:整句播放完打断
//	返回:		成功为 0, 失败其它；
//-----------------------------------------------------------------------------
int MP4Sys_SetKey2callhanupState(int iState)
{
    /* 接口加锁 */
    if (HY_Res_SDK_Lock() != 0)
    {
        ZW_ERROR("SDK Not Init\n");
        return ERR_SDK_NOINIT;
    }

    HY_Enc_SetKey2callhanupState(iState);

    HY_Res_SDK_UnLock();

    return 0;
}

int MP4Sys_GetKey2callDuSpeechStatus(int *status)
{
    CChannelContext *pChannelContext = NULL;

    if (HY_Res_SDK_Check() != 0)
    {
        ZW_ERROR("SDK Not Init\n");
        return ERR_SDK_NOINIT;
    }

    HY_Enc_GetEncChInfo(&pChannelContext);

    *status = pChannelContext->m_key2callDuSpeechStatus;
    return 0;
}

int MP4Sys_SetKeyToCall(int enable)
{
    /* 接口加锁 */
    if (HY_Res_SDK_Lock() != 0)
    {
        ZW_ERROR("SDK Not Init\n");
        return ERR_SDK_NOINIT;
    }
    HY_Enc_SetKeyToCall(enable);

    HY_Res_SDK_UnLock();

    return 0;
}

int MP4Sys_SetKeyToCallHardwareType(int type)
{
    /* 接口加锁 */
    if (HY_Res_SDK_Lock() != 0)
    {
        ZW_ERROR("SDK Not Init\n");
        return ERR_SDK_NOINIT;
    }

    HY_Enc_SetKeyToCallHardwareType(type);

    HY_Res_SDK_UnLock();
    return 0;
}
#endif

//-----------------------------------------------------------------------------
//	说明：	获取Edvr系统中编码通道总数；
//	返回:	成功为Edvr系统中可以使用的编码通道总数，失败为-1
//  注意:	该接口为频繁调用接口，简单保护
//-----------------------------------------------------------------------------。
int     MP4Sys_GetTotalEncChannels( )
{
    int					TotalNum = 0;
//	ZW_INFO("--------into MP4Sys_GetTotalEncChannels--------\n");

    if (HY_Res_SDK_Check() != 0)
    {
        ZW_ERROR("SDK Not Init\n");
        return ERR_SDK_NOINIT;
    }
    TotalNum = 1;
    return TotalNum;
}

//-----------------------------------------------------------------------------
//	说明:		获取对应编码通道的操作句柄,与对应通道的相关的操作必须使用 该句柄；
//	输入参数:	int ChannelNum 视频通道号；
//	返回:		成功返回对应通道有效句柄, 失败返回 -1；
//-----------------------------------------------------------------------------
void*   MP4Sys_EncChannelOpen(int ChannelNum)
{
    CChannelContext	*pChannelContext = NULL;
//	ZW_INFO("--------into MP4Sys_EncChannelOpen--------\n");

    /* 接口加锁 */
    if (HY_Res_SDK_Lock() != 0)
    {
        ZW_ERROR("SDK Not Init\n");
        return (void *)-1;
    }
    /* 参数检查 */
    if ((ChannelNum < 0) || (ChannelNum >= 1))
    {
        ZW_ERROR("ChannelNum = %d Err\n", ChannelNum);
        HY_Res_SDK_UnLock();
        return (void *)-1;
    }

    HY_Enc_GetEncChInfo(&pChannelContext);
    HY_Res_SDK_UnLock();
//  ZW_DEBUG("%s ChannelNum = %d pChannelContext = 0x%#x\n", __FUNCTION__, ChannelNum, (int)pChannelContext);
    return (void *)pChannelContext;
}

int     MP4Sys_EncChannelClose(void *hChannelHandle)
{
//	ZW_INFO("--------into MP4Sys_EncChannelClose--------\n");

    /* 接口加锁 */
    if (HY_Res_SDK_Lock() != 0)
    {
        ZW_ERROR("SDK Not Init\n");
        return ERR_SDK_NOINIT;
    }

    HY_Res_SDK_UnLock();
    return HI_SDK_SUCESS;
}

//-----------------------------------------------------------------------------
//	说明：	设置RTC硬件时间
//	返回:		成功为0其他为错误号
//-----------------------------------------------------------------------------
int     MP4Sys_EncSetCurrentTime( )
{
    ZW_INFO("--------into MP4Sys_EncSetCurrentTime--------\n");

    /* 接口加锁 */
    if (HY_Res_SDK_Lock() != 0)
    {
        ZW_ERROR("SDK Not Init\n");
        return ERR_SDK_NOINIT;
    }

    HY_Enc_SetCurrentTime(0);
    HY_Res_SDK_UnLock();
    return HI_SDK_SUCESS;
}

//-----------------------------------------------------------------------------
//	说明：	设置RTC硬件时间，且完成后将系统时间改为设定的tNewTime时间
//	返回:		成功为0其他为错误号
//-----------------------------------------------------------------------------
int     MP4Sys_EncSetCurrentTimeEx(int tNewTime)
{
    ZW_INFO("--------into MP4Sys_EncSetCurrentTimeEx----tNewTime=%d----\n", tNewTime);

    /* 接口加锁 */
    if (HY_Res_SDK_Lock() != 0)
    {
        ZW_ERROR("SDK Not Init\n");
        return ERR_SDK_NOINIT;
    }

    HY_Enc_SetCurrentTime(tNewTime);
    HY_Res_SDK_UnLock();
    return HI_SDK_SUCESS;
}

int     MP4Sys_EncSetVideoPara(void *hChannelHandle, int Brightness, int Contrast, int Saturation, int Hue, int Sharpness)
{
    CChannelContext		*pChannelContext = (CChannelContext*)hChannelHandle;

    ZW_INFO("--------into MP4Sys_EncSetVideoPara--------\n");
#ifdef NO_CHANGE_IMAGE_SETING
    return 0;
#endif
    /* 接口加锁 */
    if (HY_Res_SDK_Lock() != 0)
    {
        ZW_ERROR("SDK Not Init\n");
        return ERR_SDK_NOINIT;
    }

    /* 先禁止在未启动前调用 */
    if(pChannelContext->m_bRecord != TRUE)
    {
        SDKPRINT("please start encode first !\n");
        HY_Res_SDK_UnLock();
        return -1 ;
    }

    /* 参数检查 */
    if( !HY_Enc_IsEncHandleOk( hChannelHandle ) )
    {
        ZW_ERROR("INVALID_HANDLE\n");
        HY_Res_SDK_UnLock();
        return ERR_INVALID_HANDLE ;
    }
    ZW_DEBUG("Brightness=%d, Contrast=%d, Saturation=%d, Hue=%d, Sharpness=%d\n", Brightness, Contrast, Saturation, Hue, Sharpness);
    Brightness      = Brightness < 0 ? 0 : ( Brightness > 100 ? 100 : Brightness ) ;
    Contrast        = Contrast < 0 ? 0 : ( Contrast > 100 ? 100 : Contrast ) ;
    Saturation      = Saturation < 0 ? 0 : ( Saturation > 100 ? 100 : Saturation ) ;
    Hue             = Hue < 0 ? 0 : ( Hue > 100 ? 100: Hue ) ;
    Sharpness       = Sharpness < 0 ? 0 : ( Sharpness > 100 ? 100: Sharpness ) ;

    pChannelContext->Contrast       = Contrast ;
    pChannelContext->Bright         = Brightness;
    pChannelContext->Saturation     = Saturation;
    pChannelContext->Hue            = Hue;
    pChannelContext->Sharpness		= Sharpness;
    if(0 != HY_Enc_SetVideoPara(pChannelContext))
       {
        HY_Res_SDK_UnLock();
        ZW_DEBUG("MP4Sys_EncSetVideoPara fail !\n");
        return -1;
    }
    HY_Res_SDK_UnLock();						//释放接口锁
    ZW_DEBUG("MP4Sys_EncSetVideoPara ok !\n");
    return HI_SDK_SUCESS;
}

//-----------------------------------------------------------------------------
//	说明：		注册压缩数据回调函数
//	输入参数：	STREAM_READ_CALLBACK	StreamReadCallback	数据流准备好时会调用该函数
//	返回:		成功为 0 , 其它为定义的错误号；
//-----------------------------------------------------------------------------
int     MP4Sys_EncRegisterStreamReadCallback(STREAM_READ_CALLBACK StreamReadCallback)
{
    SystemInfo_t		*pSysInfo = NULL;
    SDK_ResInfo_t		*pSDK_Info = NULL;

    ZW_INFO("--------into MP4Sys_EncRegisterStreamReadCallback--------\n");

    /* 参数检查 */
    if (StreamReadCallback == NULL)
    {
        ZW_ERROR("INVALID_ARGUMENT\n");
        return ERR_INVALID_ARGUMENT ;
    }
    /* 接口加锁 */
    if (HY_Res_SDK_Lock() != 0)
    {
        ZW_ERROR("SDK Not Init\n");
        return ERR_SDK_NOINIT;
    }

    HY_Res_GetSystemInfo(&pSysInfo);
    HY_Res_GetSDKInfo(&pSDK_Info);
    pSysInfo->m_pStreamReadCallback = StreamReadCallback;
    pSDK_Info->m_stBuleScreenThreadInfo.cbStreamReadCallback = pSysInfo->m_pStreamReadCallback;

    HY_Res_SDK_UnLock();
    return HI_SDK_SUCESS;
}

//-----------------------------------------------------------------------------
//	说明：		启动编码
//	输入参数：	hChannelHandle		对应通道句柄；
//	返回： 		成功为 0 , 其它为定义的错误号
//-----------------------------------------------------------------------------
int     MP4Sys_EncStartVideoCapture(void *hChannelHandle)
{
    CChannelContext		*pChannelContext = (CChannelContext*)hChannelHandle;
    SystemInfo_t		*pSysInfo;

    ZW_INFO("--------into MP4Sys_EncStartVideoCapture--------\n");

    /* 接口加锁 */
    if (HY_Res_SDK_Lock() != 0)
    {
        ZW_ERROR("SDK Not Init\n");
        return ERR_SDK_NOINIT;
    }

    /* 参数检查 */
    if( !HY_Enc_IsEncHandleOk( hChannelHandle ) )
    {
        ZW_ERROR("INVALID_HANDLE\n");
        HY_Res_SDK_UnLock();
        return ERR_INVALID_HANDLE ;
    }

    HY_Res_GetSystemInfo(&pSysInfo);
    HY_Res_RD_Lock(&pSysInfo->m_SysTemConfigLock);			//加锁禁止系统修改制式
    XTRACE_MUTEX_LOCK(&pChannelContext->m_SensorLock);
    XTRACE_MUTEX_LOCK(&pChannelContext->m_EncLock);

    if (!pChannelContext->m_bRecord)
    {
        XTRACE_MUTEX_LOCK(&pChannelContext->m_VencLock);
        HY_Enc_StartEnc(pChannelContext, FIRST_STREAM);
        pChannelContext->m_bRecord = TRUE;
        HY_Enc_StartEnc(pChannelContext, SECOND_STREAM);
        pChannelContext->m_bRecordAux = TRUE;

        if (pChannelContext->m_bSupportFourthStream)
        {
            HY_Enc_StartEnc(pChannelContext, FOURTH_STREAM);
            pChannelContext->m_bRecordFourth= TRUE;
        }
        XTRACE_MUTEX_UNLOCK(&pChannelContext->m_VencLock);
    }

    XTRACE_MUTEX_UNLOCK(&pChannelContext->m_EncLock);
    XTRACE_MUTEX_UNLOCK(&pChannelContext->m_SensorLock);
    HY_Res_RD_UnLock(&pSysInfo->m_SysTemConfigLock);	//允许系统修改制式
    HY_Res_SDK_UnLock();								//释放接口锁

    return HI_SDK_SUCESS;
}

//-----------------------------------------------------------------------------
//	说明：		停止编码
//	输入参数：	hChannelHandle		对应通道句柄；
//	返回： 		成功为 0 , 其它为定义的错误号
//-----------------------------------------------------------------------------
int     MP4Sys_EncStopVideoCapture(void *hChannelHandle)
{
    return 0;
}

//-----------------------------------------------------------------------------
//	说明：		获取一帧压缩数据
//	输入参数：	void * hChannelHandle	对应通道句柄；
//	输出参数：	void** pDataBuf	压缩数据缓冲区的地址
//	输出参数：	int *Length		缓冲区的长度
//	输出参数：	int *FrameType			帧类型
//	输出参数：	unsigned int *RealTime_Sec		绝对时间簇的秒部分
//	输出参数：	unsigned int *RealTime_USec		绝对时间簇的微秒部分
//	返回:		成功为 0, 失败-1；
//	注意：		在启动压缩、运动运动检测之后，调用此函数获得数据,为了改善性能,
//				该接口不做参数检查,而且限定在回调函数中调用,所以不做系统接口约束,
//-----------------------------------------------------------------------------
int MP4Sys_EncReadStreamData(void *hChannelHandle, char **pDataBuf, int *Length, int *FrameType, unsigned int *RealTime_Sec, unsigned int *RealTime_USec, int *StreamType)
{
    SDK_ResInfo_t   *pSDK_Info = NULL;
    CChannelContext *pChCtx    = NULL;

    static int frameType = 0;
    int RetVal = 0;
    int CallbackType = -1;

    HY_Res_GetSDKInfo(&pSDK_Info);

    /* 参数检查 */
    if (!HY_Enc_IsEncHandleOk(hChannelHandle))
    {
        ZW_ERROR("INVALID_HANDLE\n");
        return ERR_INVALID_HANDLE;
    }
    pChCtx = (CChannelContext *)hChannelHandle;

    //升级模式下直接给蓝屏帧
    if (UpgradeMode == pSDK_Info->m_enSdkModeCur)
    {
        if (NULL == pDataBuf || NULL == Length || NULL == FrameType || NULL == RealTime_Sec || NULL == RealTime_USec)
        {
            return -1;
        }

        unsigned char *pTmp = NULL;
        H264DATAPACKETHEADER FrameHead = {0};
        int upgrade_stream_index = FIRST_SENSOR;

        if (0 == frameType)
        {
            FrameHead.byFrameType = PktIFrames;
            if (TRUE == pChCtx->bUpgradeStreamOK[FIRST_SENSOR])
            {
                *FrameType = PktIFrames;
                upgrade_stream_index = FIRST_SENSOR;
                *StreamType = STREAM_CALLBACK_MAIN;
            }
#if (SYSTEM_SUPPORT_CHANNEL >= 2)
            else if (TRUE == pChCtx->bUpgradeStreamOK[SEC_SENSOR])
            {
                *FrameType = PktIFramesForS1;
                upgrade_stream_index = SEC_SENSOR;
                *StreamType = STREAM_CALLBACK_MAIN_SEC;
            }
#endif
#if (SYSTEM_SUPPORT_CHANNEL >= 3)
            else if (TRUE == pChCtx->bUpgradeStreamOK[THIRD_SENSOR])
            {
                *FrameType = PktIFramesForS2;
                upgrade_stream_index = THIRD_SENSOR;
                *StreamType = STREAM_CALLBACK_MAIN_THIRD;
            }
#endif
#if (SYSTEM_SUPPORT_CHANNEL >= 4)

            else if (TRUE == pChCtx->bUpgradeStreamOK[FOURTH_SENSOR])
            {
                *FrameType = PktIFramesForS3;
                upgrade_stream_index = FOURTH_SENSOR;
                *StreamType = STREAM_CALLBACK_MAIN_FOURTH;
            }
#endif
            frameType = 1;
        }
        else
        {
            FrameHead.byFrameType = PktQCIFIFrames;
            if (TRUE == pChCtx->bUpgradeStreamOK[FIRST_SENSOR])
            {
                *FrameType = PktQCIFIFrames;
                upgrade_stream_index = FIRST_SENSOR;
                *StreamType = STREAM_CALLBACK_SUB;
            }
#if (SYSTEM_SUPPORT_CHANNEL >= 2)
            else if (TRUE == pChCtx->bUpgradeStreamOK[SEC_SENSOR])
            {
                *FrameType = PktQCIFIFramesForS1;
                upgrade_stream_index = SEC_SENSOR;
                *StreamType = STREAM_CALLBACK_SUB_SEC;
            }
#endif
#if (SYSTEM_SUPPORT_CHANNEL >= 3)
            else if (TRUE == pChCtx->bUpgradeStreamOK[THIRD_SENSOR])
            {
                *FrameType = PktQCIFIFramesForS2;
                upgrade_stream_index = THIRD_SENSOR;
                *StreamType = STREAM_CALLBACK_SUB_THIRD;
            }
#endif
#if (SYSTEM_SUPPORT_CHANNEL >= 4)
            else if (TRUE == pChCtx->bUpgradeStreamOK[FOURTH_SENSOR])
            {
                *FrameType = PktQCIFIFramesForS3;
                upgrade_stream_index = FOURTH_SENSOR;
                *StreamType = STREAM_CALLBACK_SUB_FOURTH;
            }
#endif
            frameType = 0;
        }
        /* 升级的时候会导致录像时间戳异常；所以加上时间戳 */
        struct timeval  tv = {0};
        gettimeofday(&tv, NULL);

        FrameHead.byLoadType = H264;
        FrameHead.uDataPacketStartCode = FRAME_START_CODE;
        FrameHead.uFrameNo = 0;
        FrameHead.uShowTime = 0;
        FrameHead.u32RealTimeSec = tv.tv_sec;
        FrameHead.u32RealTimeUSec = tv.tv_usec;
        FrameHead.uReserved = 0;
        FrameHead.uOverloadLen = sizeof(s_blueIFrame);

        *Length = FrameHead.uOverloadLen + sizeof(H264DATAPACKETHEADER);
        //*FrameType = FrameHead.byFrameType;
        *RealTime_Sec = FrameHead.u32RealTimeSec;
        *RealTime_USec = FrameHead.u32RealTimeUSec;

        if (NULL == s_pBuleScreenVideoBuffer[upgrade_stream_index])
        {
            //+64:保险起见，可以不要
            s_pBuleScreenVideoBuffer[upgrade_stream_index] = (unsigned char *)malloc(*Length + 64);
            if (NULL == s_pBuleScreenVideoBuffer[upgrade_stream_index])
            {
                ZW_ERROR("s_pBuleScreenVideoBuffer malloc failed!\n");
                return -1;
            }
        }

        pTmp = s_pBuleScreenVideoBuffer[upgrade_stream_index];
        memcpy(pTmp, &FrameHead, sizeof(H264DATAPACKETHEADER));
        pTmp += sizeof(H264DATAPACKETHEADER);
        memcpy(pTmp, s_blueIFrame, sizeof(s_blueIFrame));
        *pDataBuf = (char *)(s_pBuleScreenVideoBuffer[upgrade_stream_index]);

        return HI_SDK_SUCESS;
    }

    /* 软件可能在回调过程中停止录像并释放相应资源，在这里判断一下，如果已经停止录像返回没有数据 */
    if (!pChCtx->m_bRecord)
    {
        return ERR_ENCODENOTSTART;
    }

    /* 从海思编码库获取编码数据 */
    if (pChCtx->bMainStreamOK[FIRST_SENSOR])
        CallbackType = STREAM_CALLBACK_MAIN;
    else if (pChCtx->bAuxStreamOK[FIRST_SENSOR])
        CallbackType = STREAM_CALLBACK_SUB;

#if SYSTEM_SUPPORT_CHANNEL >= 2
    else if (pChCtx->bMainStreamOK[SEC_SENSOR])
        CallbackType = STREAM_CALLBACK_MAIN_SEC;
    else if (pChCtx->bAuxStreamOK[SEC_SENSOR])
        CallbackType = STREAM_CALLBACK_SUB_SEC;
#endif
#if SYSTEM_SUPPORT_CHANNEL >= 3
    else if (pChCtx->bMainStreamOK[THIRD_SENSOR])
        CallbackType = STREAM_CALLBACK_MAIN_THIRD;
    else if (pChCtx->bAuxStreamOK[THIRD_SENSOR])
        CallbackType = STREAM_CALLBACK_SUB_THIRD;
#endif
#if SYSTEM_SUPPORT_CHANNEL >= 4
    else if (pChCtx->bMainStreamOK[FOURTH_SENSOR])
        CallbackType = STREAM_CALLBACK_MAIN_FOURTH;
    else if (pChCtx->bAuxStreamOK[FOURTH_SENSOR])
        CallbackType = STREAM_CALLBACK_SUB_FOURTH;
#endif
    else if (pChCtx->bAudioStreamOK)
        CallbackType = STREAM_CALLBACK_AUDIO;
    /*多通道移动抓图*/
    else if (pChCtx->bMotionStateOK[FIRST_SENSOR])
        CallbackType = STREAM_CALLBACK_MOTION;
#if SYSTEM_SUPPORT_HD_ALGO_CHANNEL >= 2
    else if (pChCtx->bMotionStateOK[SEC_SENSOR])
        CallbackType = STREAM_CALLBACK_MOTION_SEC;
#endif
#if SYSTEM_SUPPORT_HD_ALGO_CHANNEL >= 3
    else if (pChCtx->bMotionStateOK[THIRD_SENSOR])
        CallbackType = STREAM_CALLBACK_MOTION_THIRD;
#endif
#if SYSTEM_SUPPORT_HD_ALGO_CHANNEL >= 4
    else if (pChCtx->bMotionStateOK[FOURTH_SENSOR])
        CallbackType = STREAM_CALLBACK_MOTION_FOURTH;
#endif
    /*多通道人形抓图*/
    else if (pChCtx->m_bHumanShapeState[FIRST_SENSOR]) //优先级放 至抓图前
        CallbackType = STREAM_CALLBACK_HUMANSHAPE;
#if SYSTEM_SUPPORT_HD_ALGO_CHANNEL >= 2
    else if (pChCtx->m_bHumanShapeState[SEC_SENSOR]) //优先级放 至抓图前
        CallbackType = STREAM_CALLBACK_HUMANSHAPE_SEC;
#endif
#if SYSTEM_SUPPORT_HD_ALGO_CHANNEL >= 3
    else if (pChCtx->m_bHumanShapeState[THIRD_SENSOR]) //优先级放 至抓图前
        CallbackType = STREAM_CALLBACK_HUMANSHAPE_THIRD;
#endif
#if SYSTEM_SUPPORT_HD_ALGO_CHANNEL >= 4
    else if (pChCtx->m_bHumanShapeState[FOURTH_SENSOR]) //优先级放 至抓图前
        CallbackType = STREAM_CALLBACK_HUMANSHAPE_FOURTH;
#endif
    else if (pChCtx->bFourthStreamOK[FIRST_SENSOR])
        CallbackType = STREAM_CALLBACK_MJPEG;
#if SYSTEM_SUPPORT_CHANNEL >= 2
    else if (pChCtx->bFourthStreamOK[SEC_SENSOR])
        CallbackType = STREAM_CALLBACK_MJPEG_SEC;
#endif
#if SYSTEM_SUPPORT_CHANNEL >= 3
    else if (pChCtx->bFourthStreamOK[THIRD_SENSOR])
        CallbackType = STREAM_CALLBACK_MJPEG_THIRD;
#endif
#if SYSTEM_SUPPORT_CHANNEL >= 4
    else if (pChCtx->bFourthStreamOK[FOURTH_SENSOR])
        CallbackType = STREAM_CALLBACK_MJPEG_FOURTH;
#endif
    else
        CallbackType = 0xFF; // 无其他流就是则获取视频流
    *StreamType = CallbackType;
    RetVal = HY_Enc_ReadOneFrame(pChCtx, (char **)pDataBuf, Length, FrameType, CallbackType);
    if (RetVal != 0)
    {
        return ERR_PACKET_NO_DATA;
    }

    *RealTime_Sec = pChCtx->m_RealTime;
    *RealTime_USec = pChCtx->m_RealTime_USec;
    return HI_SDK_SUCESS;
}

//-----------------------------------------------------------------------------
//	说明：		将当前编码帧强制设定为 I 帧
//	输入参数：	void * hChannelHandle	对应通道句柄；
//	返回:		成功为 0, 失败-1；
//-----------------------------------------------------------------------------
int     MP4Sys_EncCaptureIFrame(void *hChannelHandle, int nSensorIndex)
{
    int s32Ret = HI_SUCCESS;
    CChannelContext		*pChannelContext = (CChannelContext*)hChannelHandle;
    SystemInfo_t		*pSysInfo;

    //ZW_INFO("--------into MP4Sys_EncCaptureIFrame--------\n");

    /* 接口加锁 */
    if (HY_Res_SDK_Lock() != 0)
    {
        ZW_ERROR("SDK Not Init\n");
        return ERR_SDK_NOINIT;
    }

    /* 先禁止在未启动前调用 */
    if(pChannelContext->m_bRecord != TRUE)
    {
        SDKPRINT("please start encode first !\n");
        HY_Res_SDK_UnLock();
        return -1 ;
    }

    /* 参数检查 */
    if( !HY_Enc_IsEncHandleOk( hChannelHandle ) )
    {
        ZW_ERROR("INVALID_HANDLE\n");
        HY_Res_SDK_UnLock();
        return ERR_INVALID_HANDLE ;
    }

    HY_Res_GetSystemInfo(&pSysInfo);
    HY_Res_RD_Lock(&pSysInfo->m_SysTemConfigLock);			//加锁禁止系统修改制式
    XTRACE_MUTEX_LOCK(&pChannelContext->m_SensorLock);
    XTRACE_MUTEX_LOCK(&pChannelContext->m_EncLock);

    s32Ret = HY_Enc_ForceIFrame(pChannelContext, nSensorIndex);
    if (s32Ret)
        ZW_ERROR("Fail to call HY_Enc_ForceIFrame, ret = %#x\n", s32Ret);

    XTRACE_MUTEX_UNLOCK(&pChannelContext->m_EncLock);
    XTRACE_MUTEX_UNLOCK(&pChannelContext->m_SensorLock);
    HY_Res_RD_UnLock(&pSysInfo->m_SysTemConfigLock);	//允许系统修改制式
    HY_Res_SDK_UnLock();								//释放接口锁
    return HI_SDK_SUCESS;
}

//-----------------------------------------------------------------------------
//	说明：		设置编码类型
//	输入参数：	hChannelHandle	对应通道句柄；
//	输入参数：	Type				编码类型；
//	返回:		成功为 0, 失败-1；
//-----------------------------------------------------------------------------
int     MP4Sys_EncSetStreamType(void *hChannelHandle, unsigned short Type)
{
    CChannelContext     *pChannelContext = (CChannelContext*)hChannelHandle;
    SystemInfo_t        *pSysInfo;

    ZW_INFO("--------into MP4Sys_EncSetStreamType:%d-------\n", Type);

    /* 接口加锁 */
    if (HY_Res_SDK_Lock() != 0)
    {
        ZW_ERROR("SDK Not Init\n");
        return ERR_SDK_NOINIT;
    }
    /* 参数检查 */
    if( !HY_Enc_IsEncHandleOk( hChannelHandle ) )
    {
        ZW_ERROR("INVALID_HANDLE\n");
        HY_Res_SDK_UnLock();
        return ERR_INVALID_HANDLE ;
    }

    HY_Res_GetSystemInfo(&pSysInfo);
    HY_Res_RD_Lock(&pSysInfo->m_SysTemConfigLock);			//加锁禁止系统修改制式
    XTRACE_MUTEX_LOCK(&pChannelContext->m_SensorLock);
    XTRACE_MUTEX_LOCK(&pChannelContext->m_EncLock);
    if (pChannelContext->m_AudioChNum <= 0)  				//如果没有音频通道，即没有音频设备
    {
        if (Type != STREAM_TYPE_VIDEO)                      //如果此时不是单独的视频流，则返回失败，不能修改码流制式
        {
            ZW_ERROR("Only Support STREAM_TYPE_VIDEO\n");
            XTRACE_MUTEX_UNLOCK(&pChannelContext->m_EncLock);
            XTRACE_MUTEX_UNLOCK(&pChannelContext->m_SensorLock);
            HY_Res_RD_UnLock(&pSysInfo->m_SysTemConfigLock);	//允许系统修改制式
            HY_Res_SDK_UnLock();								//释放接口锁
            return ERR_NOT_SUPPORT;
        }
    }

    if (pChannelContext->m_streamType != Type) 			// 码流类型发生改变
    {
        pChannelContext->m_streamType = Type ;
    }

    XTRACE_MUTEX_UNLOCK(&pChannelContext->m_EncLock);
    XTRACE_MUTEX_UNLOCK(&pChannelContext->m_SensorLock);
    HY_Res_RD_UnLock(&pSysInfo->m_SysTemConfigLock);	//允许系统修改制式
    HY_Res_SDK_UnLock();								//释放接口锁
    return HI_SDK_SUCESS;
}

//-----------------------------------------------------------------------------
//	说明：		获取编码类型
//	输入参数：	hChannelHandle	对应通道句柄；
//	输入参数：	*StreamType		编码类型；
//	返回:		成功为 0, 失败-1；
//-----------------------------------------------------------------------------
int     MP4Sys_EncGetStreamType(void *hChannelHandle, unsigned short *StreamType)
{
    CChannelContext		*pChannelContext = (CChannelContext*)hChannelHandle;

    SDKPRINT("--------into MP4Sys_EncGetStreamType--------\n");

    /* 接口加锁 */
    if (HY_Res_SDK_Lock() != 0)
    {
        ZW_ERROR("SDK Not Init\n");
        return ERR_SDK_NOINIT;
    }
    /* 参数检查 */
    if( !HY_Enc_IsEncHandleOk( hChannelHandle ) )
    {
        ZW_ERROR("INVALID_HANDLE\n");
        HY_Res_SDK_UnLock();
        return ERR_INVALID_HANDLE ;
    }

    XTRACE_MUTEX_LOCK(&pChannelContext->m_SensorLock);
    XTRACE_MUTEX_LOCK(&pChannelContext->m_EncLock);
    *StreamType         = pChannelContext->m_streamType ;

    XTRACE_MUTEX_UNLOCK(&pChannelContext->m_EncLock);
    XTRACE_MUTEX_UNLOCK(&pChannelContext->m_SensorLock);
    HY_Res_SDK_UnLock();								//释放接口锁
    return HI_SDK_SUCESS;
}

//-----------------------------------------------------------------------------
//	说明：		设置编码参数
//	输入参数：	void * hChannelHandle	对应通道句柄；
//	输入参数：	EncChannel			    1表示子码流，其他表示主码流；
//	输入参数：	pCodecPara				编码参数配置；
//	返回:		成功为 0, 失败-1；
//-----------------------------------------------------------------------------
int MP4Sys_EncSetCodecPara(void *hChannelHandle, int EncChannel, CODEC_PARA_S *ppCodecPara, EncodeType_t encType)
{
    int                 s32Ret = 0;
    int					BitRate = 0;		   //Bit率
    int					FrameRate = 0;		   //帧率
    int					BitRateCtlMode = 0;	   //码率控制类型：0为VBR， 1为CBR
    int					KeyFrameInterval = 0;  //关键帧间隔
    int					MaxBps = 0;
    int					MaxFrameRate = 0;
    int 				MaxFrameRateAux = 0;
    int                 PicQualityLever = 0;
    unsigned short int  PicWidth = 0;
    unsigned short int  PicHeight = 0;
    EncodeType_t        nEncodeType;             //当前编码类型:  H264,  H265
    CChannelContext		*pChCtx = NULL;
    SystemInfo_t		*pSysInfo = NULL;
    CODEC_PARA_S	    *pEncodeAttr = NULL;
    CODEC_PARA_S		setCodeParam = {0};
    CODEC_PARA_S 		*pCodecPara = &setCodeParam;
    ZW_DEBUG("--------into MP4Sys_EncSetCodecPara--------\n");

#ifdef NO_CHANGE_ENCODE
    return 0;
#endif

    if (NULL == hChannelHandle)
    {
        ZW_ERROR("PARAM CANNOT be NULL\n");
        return -1;
    }

    pChCtx = (CChannelContext*)hChannelHandle;

    HY_Res_GetSystemInfo(&pSysInfo);

    /* 接口加锁 */
    if (0 != HY_Res_SDK_Lock())
    {
        ZW_ERROR("SDK Not Init\n");
        return ERR_SDK_NOINIT;
    }

    /* 参数检查 */
    if(!HY_Enc_IsEncHandleOk(hChannelHandle))
    {
        ZW_ERROR("INVALID_HANDLE\n");
        HY_Res_SDK_UnLock();
        return ERR_INVALID_HANDLE ;
    }

    /* 只有启动编码之后才能设置编码参数 */
    if(TRUE != pChCtx->m_bRecord)
    {
        SDKPRINT("please start encode first !\n");
        HY_Res_SDK_UnLock();
        return -1 ;
    }

    if (NULL == ppCodecPara)
    {
        ZW_ERROR("pCodecPara == NULL\n");
        HY_Res_SDK_UnLock();
        return ERR_INVALID_ARGUMENT ;
    }

    memcpy(pCodecPara, ppCodecPara, sizeof(CODEC_PARA_S));
    //控制最大帧率
    MaxFrameRate = (pSysInfo->m_Frequ_HZ==CMOS_FREQUENCY_60HZ) ? 30 : 25;

    if (LtH264Video != encType && LtH265Video != encType && LtH265PlusVideo != encType && LtH264PlusVideo != encType)
    {
        ZW_ERROR("encType = %#x\n", encType);
        HY_Res_SDK_UnLock();
        return ERR_INVALID_ARGUMENT ;
    }

    HY_Res_RD_Lock(&pSysInfo->m_SysTemConfigLock);			//加锁禁止系统修改制式
    XTRACE_MUTEX_LOCK(&pChCtx->m_SensorLock);
    XTRACE_MUTEX_LOCK(&pChCtx->m_EncLock);

    if ((1 == pSysInfo->m_bSupportWifi || 1 == pSysInfo->m_bSupportSDCard ) && pCodecPara->BitRate >= 4096000)
    {
        BitRate = 4096;
    }
    else
    {
        BitRate = (pCodecPara->BitRate / 1000);
    }
    FrameRate	     = pCodecPara->FrameRate;
    BitRateCtlMode	 = pCodecPara->BitRateCtlMode;
    KeyFrameInterval = pCodecPara->KeyFrameInterval;
    PicQualityLever  = pCodecPara->PicQualityLever;
    PicWidth = pCodecPara->PictureRes.m_PicWidth;
    PicHeight = pCodecPara->PictureRes.m_PicHeight;

    if (PicHeight <= 0 || PicWidth <= 0)
    {
        ZW_ERROR("Invalid picture size, width = %d, height = %d\n", PicWidth, PicHeight);
        XTRACE_MUTEX_UNLOCK(&pChCtx->m_EncLock);
        XTRACE_MUTEX_UNLOCK(&pChCtx->m_SensorLock);
        HY_Res_RD_UnLock(&pSysInfo->m_SysTemConfigLock);    //允许系统修改制式
        HY_Res_SDK_UnLock();                                //释放接口锁
        return ERR_INVALID_ARGUMENT ;
    }

    ZW_DEBUG("EncChannel = %d, EncodeType = %#x, Bit = %dk, FRate = %d, BitRateCtlMode = %d, Key = %d, picQ = %d width = %d height = %d\n",
                    EncChannel, encType, BitRate, FrameRate, BitRateCtlMode, KeyFrameInterval, PicQualityLever, PicWidth, PicHeight);

    /* 1.根据传入的码流类型，获取当前的编码参数和编码类型 */
    if (FIRST_STREAM == EncChannel)
    {
        nEncodeType = pChCtx->m_EncType;
        pEncodeAttr = &pChCtx->m_EncAttr;
    }
    else if (SECOND_STREAM == EncChannel)
    {
        nEncodeType = pChCtx->m_AuxEncType;
        pEncodeAttr = &pChCtx->m_EncAttrAux;
    }
    else
    {
        XTRACE_MUTEX_UNLOCK(&pChCtx->m_EncLock);
        XTRACE_MUTEX_UNLOCK(&pChCtx->m_SensorLock);
        HY_Res_RD_UnLock(&pSysInfo->m_SysTemConfigLock);    //允许系统修改制式
        HY_Res_SDK_UnLock();                                //释放接口锁
        return HI_SDK_SUCESS ;
    }

    /* 2.判断传入的参数是否有改变 */
    if((pEncodeAttr->EncType == encType)
    && (pEncodeAttr->PictureRes.m_PicWidth == PicWidth && pEncodeAttr->PictureRes.m_PicHeight == PicHeight)
    && (pEncodeAttr->BitRate == BitRate)
    && (pEncodeAttr->FrameRate == FrameRate)
    && (pEncodeAttr->BitRateCtlMode == BitRateCtlMode)
    && (pEncodeAttr->KeyFrameInterval == KeyFrameInterval)
    && ((pEncodeAttr->PicQualityLever == PicQualityLever) || (BitRateCtlMode == 1)))
    {
        ZW_DEBUG("MP4Sys_EncSetCodecPara Stream %d encode no change !\n", EncChannel);
        XTRACE_MUTEX_UNLOCK(&pChCtx->m_EncLock);
        XTRACE_MUTEX_UNLOCK(&pChCtx->m_SensorLock);
        HY_Res_RD_UnLock(&pSysInfo->m_SysTemConfigLock);    //允许系统修改制式
        HY_Res_SDK_UnLock();                                //释放接口锁
        return HI_SDK_SUCESS ;
    }

    if (FIRST_STREAM == EncChannel)  // 主码流
    {
#if defined (__PRODUCT_P4__)
        if (PicWidth > VSIZE_4M_WIDTH || PicHeight > VSIZE_4M_HEIGHT)
#elif defined(__PRODUCT_P4U__)
        if (PicWidth > VSIZE_5M_WIDTH || PicHeight > VSIZE_5M_HEIGHT)
#else
        if (PicWidth > VSIZE_3M_WIDTH || PicHeight > VSIZE_3M_HEIGHT)
#endif
        {
            ZW_ERROR("Main: PicWidth = %d  PicHeight = %d INVALID_ARGUMENT\n", PicWidth, PicHeight);
            XTRACE_MUTEX_UNLOCK(&pChCtx->m_EncLock);
            XTRACE_MUTEX_UNLOCK(&pChCtx->m_SensorLock);
            HY_Res_RD_UnLock(&pSysInfo->m_SysTemConfigLock);	//允许系统修改制式
            HY_Res_SDK_UnLock();								//释放接口锁
            return ERR_INVALID_ARGUMENT ;
        }

        if (0 == BitRateCtlMode)
            PicQualityLever = pCodecPara->PicQualityLever;
        else
            PicQualityLever = 0;

        MaxBps = MAX_MAIN_BPS;
        MaxFrameRate = MAX_MAIN_FPS;

        pChCtx->m_EncType = encType;
        pChCtx->m_EncAttr.EncType = encType;
        pChCtx->m_EncAttr.BitRate = (BitRate < 32) ? 32 : ((BitRate > MaxBps) ? MaxBps : BitRate);
        pChCtx->m_EncAttr.PicQualityLever = PicQualityLever;
        if (ENCODE_BITRATE_CTRLMODE_VBR == BitRateCtlMode)
        {
            pChCtx->m_EncAttr.BitRateCtlMode = ENCODE_BITRATE_CTRLMODE_AVBR;
        }
        else
        {
            pChCtx->m_EncAttr.BitRateCtlMode = BitRateCtlMode;
        }

        pChCtx->m_EncAttr.PictureRes.m_PicWidth = PicWidth;
        pChCtx->m_EncAttr.PictureRes.m_PicHeight = PicHeight;
        ZW_DEBUG("[MAIN] m_PicWidth: %d, m_PicHeight: %d, PicWidth: %d, PicHeight: %d\n",
            pChCtx->m_PictureRes[FIRST_STREAM].m_PicWidth,
            pChCtx->m_PictureRes[FIRST_STREAM].m_PicHeight,
            PicWidth, PicHeight);

        pChCtx->m_EncAttr.FrameRate = (FrameRate < 1) ? 1 : ((FrameRate > MaxFrameRate) ? MaxFrameRate : FrameRate);
        ZW_DEBUG("[MAIN] FrameRate=%d, MaxFrameRate=%d, m_EncAttr.FrameRate=%d, PicQualityLever=%d\n",
            FrameRate, MaxFrameRate,
            pChCtx->m_EncAttr.FrameRate, pChCtx->m_EncAttr.PicQualityLever);

        if (((PicWidth > 0 && PicHeight > 0) && (pChCtx->m_PictureRes[FIRST_STREAM].m_PicWidth != PicWidth || pChCtx->m_PictureRes[FIRST_STREAM].m_PicHeight != PicHeight))
            || (nEncodeType != encType))
        {
            XTRACE_MUTEX_LOCK(&pSysInfo->m_SysOSDLock);
            pChCtx->m_PictureRes[FIRST_STREAM].m_PicWidth = PicWidth;
            pChCtx->m_PictureRes[FIRST_STREAM].m_PicHeight = PicHeight;
            XTRACE_MUTEX_LOCK(&pSysInfo->m_WriteFileLock);
            s32Ret = HY_Enc_SwitchPicFormat(pChCtx, EncChannel);
            if (s32Ret < 0)
            {
                ZW_ERROR("Main Fail to called HY_Enc_SwitchPicFormat! ret = %#x\n", s32Ret);
                XTRACE_MUTEX_UNLOCK(&pSysInfo->m_WriteFileLock);
                XTRACE_MUTEX_UNLOCK(&pSysInfo->m_SysOSDLock);
                XTRACE_MUTEX_UNLOCK(&pChCtx->m_EncLock);
                XTRACE_MUTEX_UNLOCK(&pChCtx->m_SensorLock);
                HY_Res_RD_UnLock(&pSysInfo->m_SysTemConfigLock);    //允许系统修改制式
                HY_Res_SDK_UnLock();                                //释放接口锁
                return s32Ret;
            }

            XTRACE_MUTEX_UNLOCK(&pSysInfo->m_WriteFileLock);
            XTRACE_MUTEX_UNLOCK(&pSysInfo->m_SysOSDLock);
        }
    }
    else if (SECOND_STREAM == EncChannel) // 子码流
    {
        if (PicWidth > VSIZE_720P_WIDTH || PicHeight > VSIZE_720P_HEIGHT)
        {
            ZW_ERROR("Aux: PicWidth = %d PicHeight =%d INVALID_ARGUMENT\n", PicWidth, PicHeight);
            XTRACE_MUTEX_UNLOCK(&pChCtx->m_EncLock);
            XTRACE_MUTEX_UNLOCK(&pChCtx->m_SensorLock);
            HY_Res_RD_UnLock(&pSysInfo->m_SysTemConfigLock);    //允许系统修改制式
            HY_Res_SDK_UnLock();                                //释放接口锁
            return ERR_INVALID_ARGUMENT ;
        }

        if (0 == BitRateCtlMode)
            PicQualityLever = pCodecPara->PicQualityLever;
        else
            PicQualityLever = 0;

        MaxBps = MAX_BPS_360P;
        MaxFrameRateAux = 20;
        if (pChCtx->m_EncAttrAux.FrameRate >= 25)
        {
            MaxFrameRateAux = (pSysInfo->m_Frequ_HZ==CMOS_FREQUENCY_60HZ) ? 30 : 25;
        }

        pChCtx->m_AuxEncType = encType;
        pChCtx->m_EncAttrAux.EncType = encType;
        pChCtx->m_EncAttrAux.BitRate = (pCodecPara->BitRate / 1000);//底层单位为kb
        pChCtx->m_EncAttrAux.PicQualityLever = PicQualityLever;
        if (ENCODE_BITRATE_CTRLMODE_VBR == BitRateCtlMode)
        {
            pChCtx->m_EncAttrAux.BitRateCtlMode = ENCODE_BITRATE_CTRLMODE_AVBR;
        }
        else
        {
            pChCtx->m_EncAttrAux.BitRateCtlMode = BitRateCtlMode;
        }
        pChCtx->m_EncAttrAux.PictureRes.m_PicWidth = PicWidth;
        pChCtx->m_EncAttrAux.PictureRes.m_PicHeight = PicHeight;
        pChCtx->m_EncAttrAux.FrameRate = (FrameRate < 1) ? 1 : ((FrameRate > MaxFrameRateAux) ? MaxFrameRateAux : FrameRate);

        if (((PicWidth > 0 && PicHeight > 0) && (pChCtx->m_PictureRes[SECOND_STREAM].m_PicWidth != PicWidth || pChCtx->m_PictureRes[SECOND_STREAM].m_PicHeight != PicHeight))
            || (nEncodeType != encType))
        {
            XTRACE_MUTEX_LOCK(&pSysInfo->m_SysOSDLock);
            pChCtx->m_PictureRes[SECOND_STREAM].m_PicWidth = PicWidth;
            pChCtx->m_PictureRes[SECOND_STREAM].m_PicHeight = PicHeight;
            XTRACE_MUTEX_LOCK(&pSysInfo->m_WriteFileLock);
            s32Ret = HY_Enc_SwitchPicFormat(pChCtx, EncChannel);
            if (s32Ret < 0)
            {
                ZW_ERROR("Aux Fail to called HY_Enc_SwitchPicFormat! ret = %#x\n", s32Ret);
                XTRACE_MUTEX_UNLOCK(&pSysInfo->m_WriteFileLock);
                XTRACE_MUTEX_UNLOCK(&pSysInfo->m_SysOSDLock);
                XTRACE_MUTEX_UNLOCK(&pChCtx->m_EncLock);
                XTRACE_MUTEX_UNLOCK(&pChCtx->m_SensorLock);
                HY_Res_RD_UnLock(&pSysInfo->m_SysTemConfigLock);    //允许系统修改制式
                HY_Res_SDK_UnLock();                                //释放接口锁
                return s32Ret;
            }

            XTRACE_MUTEX_UNLOCK(&pSysInfo->m_WriteFileLock);
            XTRACE_MUTEX_UNLOCK(&pSysInfo->m_SysOSDLock);
        }
    }

    //自适应调整编码参数
    HY_Enc_AdjustEncCap();
    //需要重新设置一下镜像参数
    HY_Enc_SetFlipRotateMode(pChCtx, pChCtx->flipRotate);

#if 0
    s32Ret = HY_Enc_SetFrameRate(pChCtx->pPlatformChannelhandle,(ENCODE_STREAM_E)EncChannel, FrameRate, pChCtx->m_iSlowFrameMode);
    if (s32Ret)
    {
        ZW_ERROR("set HY_Enc_SetFrameRate error! ret = %d\n", s32Ret);
        XTRACE_MUTEX_UNLOCK(&pChCtx->m_EncLock);
        XTRACE_MUTEX_UNLOCK(&pChCtx->m_SensorLock);
        HY_Res_RD_UnLock(&pSysInfo->m_SysTemConfigLock);	//允许系统修改制式
        HY_Res_SDK_UnLock();
        return s32Ret;
    }
#endif

    XTRACE_MUTEX_UNLOCK(&pChCtx->m_EncLock);
    XTRACE_MUTEX_UNLOCK(&pChCtx->m_SensorLock);
    HY_Res_RD_UnLock(&pSysInfo->m_SysTemConfigLock);	//允许系统修改制式
    HY_Res_SDK_UnLock();								//释放接口锁

    ZW_DEBUG("Picture Format Switch Success!\n");
    return HI_SDK_SUCESS;
}

//-----------------------------------------------------------------------------
//	说明：		设置编码扩展参数
//	输入参数：	void * hChannelHandle	对应通道句柄；
//	输入参数：	stream					1表示子码流，0表示主码流  2表示第二子码流
//	输入参数：	ppCodecPara				编码参数配置；
//	返回:		成功为 0, 失败-1；
//-----------------------------------------------------------------------------
int     MP4Sys_EncSetCodecParaExt(void *hChannelHandle, int stream, CODEC_PARA_S_EXT *ppCodecPara)
{
    CChannelContext		*pChannelContext = (CChannelContext*)hChannelHandle;
    SystemInfo_t		*pSysInfo;
    int					KeyFrameInterval;  //关键帧间隔

    CODEC_PARA_S	   *pEncodeAttr = NULL;
    CODEC_PARA_S_EXT   setCodeParam;
    CODEC_PARA_S_EXT   *pCodecPara = &setCodeParam;

    ZW_DEBUG("--------into MP4Sys_EncSetCodecParaExt--------\n");
#ifdef NO_CHANGE_ENCODE
        return 0;
#endif

    /* 接口加锁 */
    if (HY_Res_SDK_Lock() != 0)
    {
        ZW_ERROR("SDK Not Init\n");
        return ERR_SDK_NOINIT;
    }

    /* 参数检查 */
    if(!HY_Enc_IsEncHandleOk(hChannelHandle))
    {
        ZW_ERROR("INVALID_HANDLE\n");
        HY_Res_SDK_UnLock();
        return ERR_INVALID_HANDLE ;
    }

    /* 只有启动编码之后才能设置编码参数 */
    if(pChannelContext->m_bRecord != TRUE)
    {
        SDKPRINT("please start encode first !\n");
        HY_Res_SDK_UnLock();
        return -1 ;
    }

    if (ppCodecPara == NULL)
    {
        ZW_ERROR("pCodecPara == NULL\n");
        HY_Res_SDK_UnLock();
        return ERR_INVALID_ARGUMENT ;
    }

    memcpy(pCodecPara, ppCodecPara, sizeof(CODEC_PARA_S_EXT));

    HY_Res_GetSystemInfo(&pSysInfo);
    HY_Res_RD_Lock(&pSysInfo->m_SysTemConfigLock);			//加锁禁止系统修改制式
    XTRACE_MUTEX_LOCK(&pChannelContext->m_SensorLock);
    XTRACE_MUTEX_LOCK(&pChannelContext->m_EncLock);
    XTRACE_MUTEX_LOCK(&pSysInfo->m_WriteFileLock);

    ZW_DEBUG("bAux = %d,KeyFrameInterval = %d\r\n", stream, pCodecPara->KeyFrameInterval);
    KeyFrameInterval = pCodecPara->KeyFrameInterval;

    /* 1.根据传入的码流类型，获取当前的编码参数和编码类型 */
    if(stream == 0)
    {
        pEncodeAttr = &pChannelContext->m_EncAttr;
    }
    else if (stream == 1)
    {
        pEncodeAttr = &pChannelContext->m_EncAttrAux;
    }
    else
    {
        XTRACE_MUTEX_UNLOCK(&pSysInfo->m_WriteFileLock);
        XTRACE_MUTEX_UNLOCK(&pChannelContext->m_EncLock);
        XTRACE_MUTEX_UNLOCK(&pChannelContext->m_SensorLock);
        HY_Res_RD_UnLock(&pSysInfo->m_SysTemConfigLock);    //允许系统修改制式
        HY_Res_SDK_UnLock();                                //释放接口锁
        return HI_SDK_SUCESS ;
    }

    /* 2.判断传入的参数是否有改变 */
    if(pEncodeAttr->KeyFrameInterval == KeyFrameInterval)
    {
        ZW_DEBUG("MP4Sys_EncSetCodecParaExt Stream %d encode no change !\n", stream);
        XTRACE_MUTEX_UNLOCK(&pSysInfo->m_WriteFileLock);
        XTRACE_MUTEX_UNLOCK(&pChannelContext->m_EncLock);
        XTRACE_MUTEX_UNLOCK(&pChannelContext->m_SensorLock);
        HY_Res_RD_UnLock(&pSysInfo->m_SysTemConfigLock);    //允许系统修改制式
        HY_Res_SDK_UnLock();                                //释放接口锁
        return HI_SDK_SUCESS ;
    }

    pEncodeAttr->KeyFrameInterval = (KeyFrameInterval < 2) ? 2 : KeyFrameInterval;
    ZW_INFO("stream[%d] KeyFrameInterval = %d\n", stream, pEncodeAttr->KeyFrameInterval);

    //HY_Enc_AdjustEncCap();          //自适应调整资源配置

    XTRACE_MUTEX_UNLOCK(&pSysInfo->m_WriteFileLock);
    XTRACE_MUTEX_UNLOCK(&pChannelContext->m_EncLock);
    XTRACE_MUTEX_UNLOCK(&pChannelContext->m_SensorLock);
    HY_Res_RD_UnLock(&pSysInfo->m_SysTemConfigLock);	//允许系统修改制式
    HY_Res_SDK_UnLock();								//释放接口锁

    ZW_DEBUG("MP4Sys_EncSetCodecParaExt Success!\n");

    return 0;
}

int MP4Sys_EncGetCodecPara(void *hChannelHandle, int bAux, CODEC_PARA_S * pCodecPara)
{
    CChannelContext *pChannelContext = (CChannelContext *) hChannelHandle;
    SystemInfo_t *pSysInfo;

    //ZW_DEBUG("=============into MP4Sys_EncGetCodecPara===========\n");

    // 接口加锁
    if (HY_Res_SDK_Lock() != 0)
    {
        ZW_ERROR("SDK Not Init\n");
        return ERR_SDK_NOINIT;
    }
    // 参数检查
    if( !HY_Enc_IsEncHandleOk( hChannelHandle ) )
    {
        ZW_ERROR("INVALID_HANDLE\n");
        HY_Res_SDK_UnLock();
        return ERR_INVALID_HANDLE ;
    }
    if (pCodecPara == NULL)
    {
        ZW_ERROR("[SDK]pCodecPara == NULL\n");
        HY_Res_SDK_UnLock();
        return ERR_INVALID_ARGUMENT;
    }

    HY_Res_GetSystemInfo(&pSysInfo);
    HY_Res_RD_Lock(&pSysInfo->m_SysTemConfigLock);    //加锁禁止系统修改制式
    XTRACE_MUTEX_LOCK(&pChannelContext->m_SensorLock);
    XTRACE_MUTEX_LOCK(&pChannelContext->m_EncLock);

    if (0 == bAux)
    {
        memcpy(pCodecPara, &pChannelContext->m_EncAttr, sizeof(CODEC_PARA_S));
    }
    else if (1 == bAux)
    {
        memcpy(pCodecPara, &pChannelContext->m_EncAttrAux, sizeof(CODEC_PARA_S));
    }
    else if (3 == bAux)
    {
        memcpy(pCodecPara, &pChannelContext->m_EncAttrFourth, sizeof(CODEC_PARA_S));
    }

    XTRACE_MUTEX_UNLOCK(&pChannelContext->m_EncLock);
    XTRACE_MUTEX_UNLOCK(&pChannelContext->m_SensorLock);
    HY_Res_RD_UnLock(&pSysInfo->m_SysTemConfigLock);  //允许系统修改制式
    HY_Res_SDK_UnLock();                              //释放接口锁

    return 0;
}

//-----------------------------------------------------------------------------
//	说明：		启动辅码流编码
//	输入参数：	hChannelHandle		对应通道句柄；
//	返回： 		成功为 0 , 其它为定义的错误号
//-----------------------------------------------------------------------------
int     MP4Sys_EncStartAuxCodec(void *hChannelHandle)
{
    //三码流模式下，子码流做为存储码流，在启动编码时与主码流一起被打开
    //原来打开子码流的控制接口为打开三码流
    SDKPRINT("--------into MP4Sys_EncStartAuxCodec--------\n");
    return 0;
}

//-----------------------------------------------------------------------------
//	说明：		停止辅码流编码
//	输入参数：	hChannelHandle		对应通道句柄；
//	返回： 		成功为 0 , 其它为定义的错误号
//-----------------------------------------------------------------------------
int     MP4Sys_EncStopAuxCodec(void *hChannelHandle)
{
    ZW_INFO("--------into MP4Sys_EncStopAuxCodec--------\n");
    return 0;
    return MP4Sys_EncStopAuxCodec(hChannelHandle);
}

//-----------------------------------------------------------------------------
//	说明：		设置文本OSD显示内容、坐标(单行OSD)
//	输入参数：	hChannelHandle	对应通道句柄；
//	输入参数：	TextOSDInfo_t	OSD文本点阵信息
//	返回:		成功为 0, 失败其它；
//-----------------------------------------------------------------------------
int MP4Sys_EncSetOsdText(void *hChannelHandle, TextOSDInfo_t *pOSDTextInfo)
{
    int Start_X, Start_Y;
    int TotalWidth, TotalHeight;
    short *pSrc;
    CChannelContext *pChCtx = (CChannelContext *)hChannelHandle;
    SystemInfo_t *pSysInfo;

    ZW_INFO("--------into MP4Sys_EncSetOsdText--------\n");
#ifdef NO_SET_OSD
    return 0;
#endif

    ZW_DEBUG("text osd info: x=%d, y=%d, TextNum=%d\n", pOSDTextInfo->Start_X, pOSDTextInfo->Start_Y, pOSDTextInfo->TextNum);

    /* 接口加锁 */
    if (HY_Res_SDK_Lock() != 0)
    {
        ZW_ERROR("SDK Not Init\n");
        return ERR_SDK_NOINIT;
    }

    /* 参数检查 */
    if (!HY_Enc_IsEncHandleOk(hChannelHandle))
    {
        ZW_ERROR("INVALID_HANDLE\n");
        HY_Res_SDK_UnLock();
        return ERR_INVALID_HANDLE;
    }

    if (pOSDTextInfo == NULL)
    {
        ZW_ERROR("INVALID_ARGUMENT pOSDTextInfo == NULL\n");
        HY_Res_SDK_UnLock();
        return ERR_INVALID_ARGUMENT;
    }

    if ((pOSDTextInfo->Start_X > 352) || (pOSDTextInfo->Start_Y > 288) || (pOSDTextInfo->Start_X < 0) || (pOSDTextInfo->Start_Y < 0))
    {
        ZW_ERROR("Start Point Error : x = %d, y = %d\n", pOSDTextInfo->Start_X, pOSDTextInfo->Start_Y);
        HY_Res_SDK_UnLock();
        return ERR_INVALID_ARGUMENT;
    }

    HY_Res_GetSystemInfo(&pSysInfo);

    /* 检测所有字符的总宽度 */
    TotalWidth = 0;
    TotalHeight = 0;
    pSrc = (short *)pOSDTextInfo->pPixinfo;
    /* 检查叠加区域 */
    Start_X = pOSDTextInfo->Start_X & 0xFFFFFFF8;
    Start_Y = pOSDTextInfo->Start_Y & 0xFFFFFFFE;

    ZW_DEBUG("Text: width = %d, Height = %d\n", pSrc[0], pSrc[1]);
    TotalWidth = pSrc[0];
    TotalWidth = (TotalWidth + 7) & 0xFFFFFFF8; // 8对齐
    TotalHeight = pSrc[1];

    if ((TotalWidth < 0) || (TotalHeight < 0))
    {
        ZW_ERROR("Start_X = %d, TotalWidth = %d, Start_Y = %d, TotalHeight = %d Rect Out of display range!!\n",
                 Start_X, TotalWidth, Start_Y, TotalHeight);
        HY_Res_SDK_UnLock();
        return ERR_INVALID_ARGUMENT;
    }

    if ((TotalWidth * TotalHeight) > (352 * 288))
    {
        ///文本OSD文本过长
        ZW_ERROR("Size Err\n");
        HY_Res_SDK_UnLock();
        return ERR_INVALID_ARGUMENT;
    }

#ifdef SUPPORT_TEXT_OSD_TWO_LINE
    /*解析是否含有换行符^,如果有则需要考虑换行，目前只支持两行*/
    int enterChar = 0;
    int newline = 0;
    int lineWidth1 = 0;
    int lineWidth2 = 0;
    int strip1 = 0;
    int strip2 = 0;
    int i = 0;
    //检测是否有换行，及两个最大的宽度是多少
    while ((0 != pOSDTextInfo->strOSD[i]) && (i < TEXT_OSD_MAX_LEN))
    {
        if (NEW_LINE_CHAR == pOSDTextInfo->strOSD[i]) //开始字符是换行符不算,也可跳过连续换行符
        {
            if (0 != lineWidth1)
            {
                enterChar = 1; //找到一个有效换行符
            }
            i++;
            continue;
        }

        if (enterChar) //换行符后面有有效的字符则产生新的一行
        {
            newline = 1;
        }

        if (0 == newline) //计算第一行的长度
        {
            if (0 == pOSDTextInfo->strOSD[i] >> 8) // ASC
            {
                lineWidth1 += ASC_WIDTH;
            }
            else
            {
                lineWidth1 += HZ_WIDTH;
            }
        }
        else //计算第二行的长度
        {
            if (0 == pOSDTextInfo->strOSD[i] >> 8) // ASC
            {
                lineWidth2 += ASC_WIDTH;
            }
            else
            {
                lineWidth2 += HZ_WIDTH;
            }
        }
        i++;
    }

    //根据多行调整整个的宽高，总宽度为最长那行的宽度
    if (1 == newline)
    {
        TotalHeight = TotalHeight * 2;
        if (lineWidth1 >= lineWidth2)
        {
            TotalWidth = lineWidth1;
        }
        else
        {
            TotalWidth = lineWidth2;
        }
    }
#endif

    XTRACE_MUTEX_LOCK(&pSysInfo->m_SysOSDLock);
    XTRACE_MUTEX_LOCK(&pChCtx->m_OSDLock);
    // OSDV2 是否刷新bitmap
    // pChCtx->m_TextOSDV2Info.valid = 0;
    /* 分配文本点阵、宏块信息空间 */
    HIEASY_FREE(pChCtx->pTextOSDBit_Src);
    if (TotalWidth * TotalHeight > 0)
    {
        pChCtx->pTextOSDBit_Src = (char *)malloc(TotalWidth * TotalHeight / 8); // 12.375KB
        if (pChCtx->pTextOSDBit_Src == NULL)
        {
            ZW_ERROR("Text Malloc pTextOSDBit_Src Fail, W = %d, H = %d\n", TotalWidth, TotalHeight);
            XTRACE_MUTEX_UNLOCK(&pChCtx->m_OSDLock);
            XTRACE_MUTEX_UNLOCK(&pSysInfo->m_SysOSDLock);
            HY_Res_SDK_UnLock(); //释放接口锁
            return ERR_MALLOC_ERROR;
        }
        memset(pChCtx->pTextOSDBit_Src, 0, (TotalWidth * TotalHeight / 8));

#ifdef SUPPORT_TEXT_OSD_TWO_LINE
        strip1 = (TotalWidth >> 3);
        strip2 = ((lineWidth1 + lineWidth2) >> 3);

        if (1 == newline)
        {
            for (i = 0; i < (TotalHeight >> 1); i++)
            {
                memcpy(&pChCtx->pTextOSDBit_Src[i * strip1], &pOSDTextInfo->pPixinfo[4 + i * strip2], (lineWidth1 >> 3));
                memcpy(&pChCtx->pTextOSDBit_Src[(i + (TotalHeight >> 1)) * strip1], &pOSDTextInfo->pPixinfo[4 + i * strip2 + (lineWidth1 >> 3)], lineWidth2 >> 3);
            }
        }
        else
        {
            memcpy(pChCtx->pTextOSDBit_Src, pOSDTextInfo->pPixinfo + 4, TotalWidth * TotalHeight / 8);
        }

#else
        memcpy(pChCtx->pTextOSDBit_Src, pOSDTextInfo->pPixinfo + 4, TotalWidth * TotalHeight / 8);
#endif
    }

    pChCtx->TextOSDRect_Src.left = Start_X;
    pChCtx->TextOSDRect_Src.top = Start_Y;
    pChCtx->TextOSDRect_Src.right = Start_X + TotalWidth;
    pChCtx->TextOSDRect_Src.bottom = Start_Y + TotalHeight;

    if (0 != HY_Enc_SetTextOSDBitMap(pChCtx, MAX_STREAM_COUNT, pChCtx->m_bEnableTextOSD))
    {
        XTRACE_MUTEX_UNLOCK(&pChCtx->m_OSDLock);
        XTRACE_MUTEX_UNLOCK(&pSysInfo->m_SysOSDLock);
        HY_Res_SDK_UnLock(); //释放接口锁
        ZW_ERROR("SetTextOSDBitMap fail !\n");
        return -1;
    }
    ZW_DEBUG("SetTextOSDBitMap ok !\n");

    XTRACE_MUTEX_UNLOCK(&pChCtx->m_OSDLock);
    XTRACE_MUTEX_UNLOCK(&pSysInfo->m_SysOSDLock);
    HY_Res_SDK_UnLock(); //释放接口锁
    return HI_SDK_SUCESS;
}

//-----------------------------------------------------------------------------
//	说明：		设置文本OSD显示内容、坐标
//	输入参数：	hChannelHandle	对应通道句柄；
//	输入参数：	TextOSDInfo_t	OSD文本点阵信息
//	返回:		成功为 0, 失败其它；
//-----------------------------------------------------------------------------
int     MP4Sys_EncSetOsdTextEx(void *hChannelHandle, TextOSDInfoEx_t *pOSDTextExInfo)
{
    int					Start_X, Start_Y;
    int					TotalWidth, TotalHeight;
    short               *pSrc;
    CChannelContext		*pChCtx = (CChannelContext*)hChannelHandle;
    SystemInfo_t		*pSysInfo;

    ZW_INFO("--------into MP4Sys_EncSetOsdTextEx--------\n");
#ifdef NO_SET_OSD
        return 0;
#endif

    if (pOSDTextExInfo == NULL)
    {
        ZW_ERROR("INVALID_ARGUMENT pOSDTextInfo == NULL\n");
        return ERR_INVALID_ARGUMENT;
    }

    ZW_DEBUG("text osd info: x=%d, y=%d, TextNum=%d\n", pOSDTextExInfo->Start_X, pOSDTextExInfo->Start_Y, pOSDTextExInfo->TextNum);

    /* 接口加锁 */
    if (HY_Res_SDK_Lock() != 0)
    {
        ZW_ERROR("SDK Not Init\n");
        return ERR_SDK_NOINIT;
    }
    /* 参数检查 */
    if( !HY_Enc_IsEncHandleOk(hChannelHandle))
    {
        ZW_ERROR("INVALID_HANDLE\n");
        HY_Res_SDK_UnLock();
        return ERR_INVALID_HANDLE ;
    }

    if((pOSDTextExInfo->Start_X > 352) || (pOSDTextExInfo->Start_Y > 288) || (pOSDTextExInfo->Start_X < 0) || (pOSDTextExInfo->Start_Y < 0))
    {
        ZW_ERROR("Start Point Error : x = %d, y = %d\n", pOSDTextExInfo->Start_X, pOSDTextExInfo->Start_Y);
        HY_Res_SDK_UnLock();
        return ERR_INVALID_ARGUMENT;
    }

    HY_Res_GetSystemInfo(&pSysInfo);

    /* 检测所有字符的总宽度 */
    TotalWidth	= 0;
    TotalHeight	= 0;
    pSrc = (short *)pOSDTextExInfo->pPixinfo;
    /* 检查叠加区域 */
    Start_X = pOSDTextExInfo->Start_X & 0xFFFFFFF8;
    Start_Y = pOSDTextExInfo->Start_Y & 0xFFFFFFFE;

    ZW_DEBUG("Text: width = %d, Height = %d\n", pSrc[0], pSrc[1]);
    TotalWidth = pSrc[0];
    TotalWidth	= (TotalWidth + 7) & 0xFFFFFFF8;//8对齐
    TotalHeight = pSrc[1];

    if ((TotalWidth < 0) || (TotalHeight < 0))
    {
        ZW_ERROR("Start_X = %d, TotalWidth = %d, Start_Y = %d, TotalHeight = %d Rect Out of display range!!\n",
                  Start_X, TotalWidth, Start_Y, TotalHeight);
        HY_Res_SDK_UnLock();
        return ERR_INVALID_ARGUMENT ;
    }

    if((TotalWidth * TotalHeight) > (352 * 288))
    {
        ///文本OSD文本过长
        ZW_ERROR("Size Err\n");
        HY_Res_SDK_UnLock();
        return ERR_INVALID_ARGUMENT;
    }

    #ifdef SUPPORT_TEXT_OSD_TWO_LINE
    /*解析是否含有换行符^,如果有则需要考虑换行，目前只支持两行*/
    int enterChar = 0;
    int newline = 0;
    int lineWidth1 = 0;
    int lineWidth2 = 0;
    int strip1 = 0;
    int strip2 = 0;
    int i = 0;
    //检测是否有换行，及两个最大的宽度是多少
    while((0 != pOSDTextExInfo->strOSD[i]) && (i < 120))
    {
        if(NEW_LINE_CHAR == pOSDTextExInfo->strOSD[i])//开始字符是换行符不算,也可跳过连续换行符
        {
            if(0 != lineWidth1)
            {
                enterChar = 1;//找到一个有效换行符
            }
            i++;
            continue;
        }

        if(enterChar)//换行符后面有有效的字符则产生新的一行
        {
            newline = 1;
        }

        if(0 == newline)//计算第一行的长度
        {
            if(0 == pOSDTextExInfo->strOSD[i] >> 8)//ASC
            {
                lineWidth1 += ASC_WIDTH;
            }
            else
            {
                lineWidth1 += HZ_WIDTH;
            }
        }
        else//计算第二行的长度
        {
            if(0 == pOSDTextExInfo->strOSD[i] >> 8)//ASC
            {
                lineWidth2 += ASC_WIDTH;
            }
            else
            {
                lineWidth2 += HZ_WIDTH;
            }
        }
        i++;
    }

    //根据多行调整整个的宽高，总宽度为最长那行的宽度
    if(1 == newline)
    {
        TotalHeight = TotalHeight * 2;
        if(lineWidth1 >= lineWidth2)
        {
            TotalWidth = lineWidth1;
        }
        else
        {
            TotalWidth = lineWidth2;
        }
    }
    #endif

    XTRACE_MUTEX_LOCK(&pSysInfo->m_SysOSDLock);
    XTRACE_MUTEX_LOCK(&pChCtx->m_OSDLock);
    // OSDV2
    // pChCtx->m_TextOSDV2Info.valid = 0;
    /* 分配文本点阵、宏块信息空间 */
    HIEASY_FREE(pChCtx->pTextOSDBit_Src);
    if (TotalWidth * TotalHeight > 0)
    {
        pChCtx->pTextOSDBit_Src = (char *)malloc(TotalWidth * TotalHeight / 8);  // 12.375KB
        if (pChCtx->pTextOSDBit_Src == NULL)
        {
            ZW_ERROR("Text Malloc pTextOSDBit_Src[index] Fail, W = %d, H = %d\n", TotalWidth, TotalHeight);
            XTRACE_MUTEX_UNLOCK(&pChCtx->m_OSDLock);
            XTRACE_MUTEX_UNLOCK(&pSysInfo->m_SysOSDLock);
            HY_Res_SDK_UnLock();								//释放接口锁
            return ERR_MALLOC_ERROR;
        }

        memset(pChCtx->pTextOSDBit_Src, 0, (TotalWidth * TotalHeight / 8));

#ifdef SUPPORT_TEXT_OSD_TWO_LINE
        strip1 = (TotalWidth >> 3);
        strip2 = ((lineWidth1 + lineWidth2) >> 3);

        if(1 == newline)
        {
            for(i=0; i < (TotalHeight >> 1); i++)
            {
                memcpy(&pChCtx->pTextOSDBit_Src[i * strip1], &pOSDTextExInfo->pPixinfo[4 + i * strip2], (lineWidth1 >> 3));
                memcpy(&pChCtx->pTextOSDBit_Src[(i + (TotalHeight >> 1)) * strip1], &pOSDTextExInfo->pPixinfo[4 + i * strip2 + (lineWidth1 >> 3)], lineWidth2 >> 3);
            }
        }
        else
        {
            memcpy(pChCtx->pTextOSDBit_Src, pOSDTextExInfo->pPixinfo + 4, TotalWidth * TotalHeight / 8);
        }
#else
        memcpy(pChCtx->pTextOSDBit_Src, pOSDTextExInfo->pPixinfo + 4, TotalWidth * TotalHeight / 8);
#endif

    }

    pChCtx->TextOSDRect_Src.left 	= Start_X;
    pChCtx->TextOSDRect_Src.top 	= Start_Y;
    pChCtx->TextOSDRect_Src.right	= Start_X + TotalWidth;
    pChCtx->TextOSDRect_Src.bottom = Start_Y + TotalHeight;

    if (0 != HY_Enc_SetTextOSDBitMap(pChCtx, MAX_STREAM_COUNT, pChCtx->m_bEnableTextOSD))
    {
        XTRACE_MUTEX_UNLOCK(&pChCtx->m_OSDLock);
        XTRACE_MUTEX_UNLOCK(&pSysInfo->m_SysOSDLock);
        HY_Res_SDK_UnLock();								//释放接口锁
        ZW_ERROR("SetTextOSDBitMap fail !\n");
        return -1;
    }
    ZW_DEBUG("SetTextOSDBitMap ok !\n");

    XTRACE_MUTEX_UNLOCK(&pChCtx->m_OSDLock);
    XTRACE_MUTEX_UNLOCK(&pSysInfo->m_SysOSDLock);
    HY_Res_SDK_UnLock();								//释放接口锁
    return HI_SDK_SUCESS;
}

//对应客户端的textosd1(文本OSD)
int MP4Sys_EncSetOsdV2Text(void *hChannelHandle, TextOSDV2Info_t *pOSDV2TextInfo)
{
    int totalWidth = 0, totalHeight = 0;
    int pixWidth = 0, pixHeight = 0;
    int startX = 0, startY = 0;
    CChannelContext		*pChCtx = (CChannelContext*)hChannelHandle;
    SystemInfo_t		*pSysInfo;
    short               *pSrc;
    int stripSrc = 0, stripDst = 0;
    int i = 0, j = 0, k = 0;

    char *pOSDV2Src = NULL;
    unsigned char *pOSDV2Dst2 = NULL;
    int MallocSize = 0, stride = 0;
    char                ptmp;
    unsigned char 		bit;
    char                *pixel;
    int pixelCount = 0;
    int offset_byte = 0;
    int offset_bit = 0;

    ZW_INFO("--------into MP4Sys_EncSetOsdV2Text --------\n");
    #ifdef NO_SET_OSD
        return 0;
    #endif

    if (NULL == pOSDV2TextInfo)
    {
        ZW_ERROR("INVALID_ARGUMENT pOSDTextInfo == NULL\n");
        return ERR_INVALID_ARGUMENT;
    }

    ZW_DEBUG("text OSDV2 info: x=%d, y=%d\n", pOSDV2TextInfo->Start_X, pOSDV2TextInfo->Start_Y);

    /* 接口加锁 */
    if (HY_Res_SDK_Lock() != 0)
    {
        ZW_ERROR("SDK Not Init\n");
        return ERR_SDK_NOINIT;
    }
    /* 参数检查 */
    if( !HY_Enc_IsEncHandleOk(hChannelHandle))
    {
        ZW_ERROR("INVALID_HANDLE\n");
        HY_Res_SDK_UnLock();
        return ERR_INVALID_HANDLE ;
    }

    HY_Res_GetSystemInfo(&pSysInfo);

    /* 检测所有字符的总宽度 */
    totalWidth	= 0;
    totalHeight	= 0;
    pSrc = (short *)pOSDV2TextInfo->pPixinfo;
    /* 检查叠加区域 */
    startX = pOSDV2TextInfo->Start_X & 0xFFFFFFF8;
    startY = pOSDV2TextInfo->Start_Y & 0xFFFFFFFE;

    ZW_DEBUG("Text: width = %d, Height = %d, PixWidth = %d, PixHeight = %d\n", pSrc[0], pSrc[1], pSrc[2], pSrc[3]);
    totalWidth = pSrc[0];
    totalWidth	= (totalWidth + 7) & 0xFFFFFFF8;//8对齐
    totalHeight = pSrc[1];
    // 区域点阵的宽高已经在中间层计算好，不需要重复计算
    pixWidth = pSrc[2];
    pixHeight = pSrc[3];

    if ((pixWidth < 0) || (pixHeight < 0))
    {
        ZW_ERROR("startX = %d, totalWidth = %d, startY = %d, totalHeight = %d Rect Out of display range!!\n",
                  startX, totalWidth, startY, totalHeight);
        HY_Res_SDK_UnLock();
        return ERR_INVALID_ARGUMENT ;
    }

    if((pixWidth * pixHeight) > (VIDEO_WIDTH * VIDEO_HEIGHT))
    {
        ///文本OSD文本过长
        ZW_ERROR("Size Err\n");
        HY_Res_SDK_UnLock();
        return ERR_INVALID_ARGUMENT;
    }

    XTRACE_MUTEX_LOCK(&pSysInfo->m_SysOSDLock);
    XTRACE_MUTEX_LOCK(&pChCtx->m_OSDLock);

    SysTextOSDV2_t *pSysOSDV2Info = &pChCtx->m_TextOSDV2Info;
    /* 重新分配20个关键字点阵内存,白色、黑色各一套，到时候直接贴图 */
    HIEASY_FREE(pSysOSDV2Info->pWhiteFontInfo);//OSD刷新线程需要不断的更新叠加信息，因此直到下一次set时才释放
    MallocSize = pOSDV2TextInfo->keyWordFontWidth * pOSDV2TextInfo->keyWordFontHeight * 20 / BYTE_PIXELS;

    pSysOSDV2Info->pWhiteFontInfo = (char *)malloc(MallocSize);
    if(NULL == pSysOSDV2Info->pWhiteFontInfo)
    {
        ZW_ERROR("pFontInfo malloc fail, w = %d, h = %d\n", pOSDV2TextInfo->keyWordFontWidth, pOSDV2TextInfo->keyWordFontHeight);
        XTRACE_MUTEX_UNLOCK(&pChCtx->m_OSDLock);
        XTRACE_MUTEX_UNLOCK(&pSysInfo->m_SysOSDLock);
        HY_Res_SDK_UnLock();								//释放接口锁
        return ERR_MALLOC_ERROR;
    }

    memset(pSysOSDV2Info->pWhiteFontInfo, 0, MallocSize);

    /* 转换海思需要的点阵 */
    pOSDV2Src	= (char  *)pOSDV2TextInfo->pKeywordPixInfo;
    pOSDV2Dst2	= (unsigned char *)pSysOSDV2Info->pWhiteFontInfo;
    stride = (pOSDV2TextInfo->keyWordFontWidth + 7) >> 3;
    for (i = 0; i < 20 * pOSDV2TextInfo->keyWordFontHeight; i++)
    {
        pixel = pOSDV2Src + i * stride;
        ptmp = *pixel;
        bit = 0;
        for (k = 0; k < pOSDV2TextInfo->keyWordFontWidth; k++)
        {
            if(bit == 0)
            {
                ptmp = *pixel ++;
                bit = 0x80;
            }

            offset_byte = pixelCount / BYTE_PIXELS;
            offset_bit = (pixelCount % BYTE_PIXELS) * PIXEL_BITS;
            if ((ptmp & bit) != 0)
            {
                pOSDV2Dst2[offset_byte] |= (COLOR_WITHE << offset_bit);
            }

            pixelCount ++;
            bit = (bit >> 1) & 0xFF;
        }
    }

    /* 保存配置信息 */
    pSysOSDV2Info->FontHeight = pOSDV2TextInfo->keyWordFontHeight;
    pSysOSDV2Info->FontWidth = pOSDV2TextInfo->keyWordFontWidth;
    pSysOSDV2Info->lineNum = pOSDV2TextInfo->lineNum;
    memcpy(pSysOSDV2Info->lineInfo, pOSDV2TextInfo->lineInfo, sizeof(OSDV2TextLineInfo_t)*OSDV2_TEXT_MAX_LINE);

    /* 分配文本点阵、宏块信息空间 */
    HIEASY_FREE(pChCtx->pTextOSDBit_Src);
    if (pixWidth * pixHeight > 0)
    {
        //osd文本拷贝附加信息之前的数据
        pChCtx->pTextOSDBit_Src = (char *)malloc(pixWidth * pixHeight / 8);
        if (pChCtx->pTextOSDBit_Src == NULL)
        {
            ZW_ERROR("Text Malloc pTextOSDBit_Src Fail, W = %d, H = %d\n", pixWidth, pixHeight);
            XTRACE_MUTEX_UNLOCK(&pChCtx->m_OSDLock);
            XTRACE_MUTEX_UNLOCK(&pSysInfo->m_SysOSDLock);
            HY_Res_SDK_UnLock();								//释放接口锁
            return ERR_MALLOC_ERROR;
        }
        memset(pChCtx->pTextOSDBit_Src, 0, ((pixWidth * pixHeight) / 8));
        // 将点阵信息分行拷贝到SDK
        stripSrc = (totalWidth >> 3);
        stripDst = (pixWidth >> 3);

        int pixNum = 0;
        int srcCopyStart = 0, srcCopyPos = 0;
        int dstCopyPos = 0;
        char alignmentNum = 0;//偏移的字符个数

        /**
         * stripSrc: 字符总个数
         * totalHeight : 字高16
         * stripDst： 最大行字符个数
         *
        */
        int lineNum = pOSDV2TextInfo->lineNum;
        OSDV2TextLineInfo_t *lineInfo = pOSDV2TextInfo->lineInfo;

        for(i = 0; i < lineNum; i++)
        {// 字符行
            pixNum = 0;
            alignmentNum = 0;

            //当前OSD在右边
            if(startX > (VIDEO_WIDTH / 2))
            {
                //当前行字符总个数小于最大行字符个数，则右对齐
                if( (lineInfo[i].pixWidth>>3) < stripDst)
                {
                    alignmentNum = stripDst - (lineInfo[i].pixWidth>>3);
                }
            }

            for(j = 0; j < totalHeight; j++)
            {// 点阵行
                srcCopyPos = 0;
                dstCopyPos = 0;
                for(k = 0; k < lineInfo[i].keywordNum; k++)
                {// 预留附加信息刷新区域
                    if((lineInfo[i].keyWords[k].pos>>3) > dstCopyPos) // 拷贝附加信息前的数据
                    {
                        int copyWidth = (lineInfo[i].keyWords[k].pos>>3) - dstCopyPos;
                        memcpy(&pChCtx->pTextOSDBit_Src[(j + i*totalHeight) * stripDst + dstCopyPos + alignmentNum], &pOSDV2TextInfo->pPixinfo[8 + j * stripSrc + srcCopyStart + srcCopyPos], copyWidth);
                        dstCopyPos += copyWidth;
                        srcCopyPos += copyWidth;
                        pixNum += copyWidth;
                    }
                    //空出附加信息的位置
                    dstCopyPos += lineInfo[i].keyWords[k].pixWidth>>3;
                }

                if(dstCopyPos < (lineInfo[i].pixWidth>>3))
                {
                    // 拷贝剩余数据
                    int copyWidth = (lineInfo[i].pixWidth>>3) - dstCopyPos;
                    memcpy(&pChCtx->pTextOSDBit_Src[(j + i*totalHeight) * stripDst + dstCopyPos + alignmentNum], &pOSDV2TextInfo->pPixinfo[8 + j * stripSrc + srcCopyStart + srcCopyPos], copyWidth);
                    pixNum += copyWidth;//每次累加当前行字符的个数
                }
            }
            srcCopyStart += pixNum/totalHeight;
        }
    }

    pChCtx->TextOSDRect_Src.left 	= startX;
    pChCtx->TextOSDRect_Src.top 	= startY;
    pChCtx->TextOSDRect_Src.right	= startX + pixWidth;
    pChCtx->TextOSDRect_Src.bottom = startY + pixHeight;

    if(0 != HY_Enc_SetTextOSDBitMap(pChCtx, MAX_STREAM_COUNT, pChCtx->m_bEnableTextOSD))
    {
        // 解决上一次设备内存不足，再设置提示重复创建的问题
        // Hi_MPP_DeInitOsd(pChCtx->pPlatformChannelhandle, FALSE);

        XTRACE_MUTEX_UNLOCK(&pChCtx->m_OSDLock);
        XTRACE_MUTEX_UNLOCK(&pSysInfo->m_SysOSDLock);
        HY_Res_SDK_UnLock();								//释放接口锁
        ZW_ERROR("SetTextOSDBitMap fail !\n");
        return -1;
    }
    ZW_DEBUG("SetTextOSDBitMap ok !\n");

    XTRACE_MUTEX_UNLOCK(&pChCtx->m_OSDLock);
    XTRACE_MUTEX_UNLOCK(&pSysInfo->m_SysOSDLock);
    HY_Res_SDK_UnLock();								//释放接口锁
    return HI_SDK_SUCESS;
}

int     MP4Sys_StartSysStatusLed(void *hChannelHandle, int freq)
{
    CChannelContext    *pChannelContext = (CChannelContext*)hChannelHandle;

    ZW_DEBUG("=============into MP4Sys_StartSysStatusLed===========\n");

    pthread_t i_IrcutThreadId;

    pthread_create(&i_IrcutThreadId, NULL, HY_Switch_Ircut_Twice, (void *)pChannelContext);

    return 0;
}

//-----------------------------------------------------------------------------
//	说明：		设置时间OSD显示内容、坐标
//	输入参数：	hChannelHandle	对应通道句柄；
//	输入参数：	TimeOSDInfo_t	OSD时间点阵信息
//	返回:		成功为 0, 失败其它；
//-----------------------------------------------------------------------------
int     MP4Sys_EncSetOsdTime(void *hChannelHandle, TimeOSDInfo_t *pOSDTimeInfo)
{
    int					i, k, ret;
    int					BlockNum, stride;
    char                ptmp;
    unsigned char		bit;
    char                *pixel;
    unsigned char 		*pDst2;
    char				*pSrc;
    CChannelContext		*pChCtx = (CChannelContext*)hChannelHandle;
    SysTimeOSD_t		*pSysTime;
    int					MallocSize;
    SystemInfo_t		*pSysInfo;
    int pixelCount = 0;
    int offset_byte = 0;
    int offset_bit = 0;

    ZW_INFO("--------into MP4Sys_EncSetOsdTime--------\n");
#ifdef NO_SET_OSD
        return 0;
#endif

    ZW_DEBUG("[OSD][TIME]: x=%d, y=%d, w=%d, h=%d, bTranslant=%d, bHour24=%d, DisMod=%d\n",
        pOSDTimeInfo->Start_X, pOSDTimeInfo->Start_Y, pOSDTimeInfo->FontWidth, pOSDTimeInfo->FontHeight,
        pOSDTimeInfo->bTranslant, pOSDTimeInfo->bHour24, pOSDTimeInfo->DisMod);

    /* 接口加锁 */
    if (HY_Res_SDK_Lock() != 0)
    {
        ZW_ERROR("SDK Not Init\n");
        return ERR_SDK_NOINIT;
    }
    /* 参数检查 */
    if( !HY_Enc_IsEncHandleOk( hChannelHandle ) )
    {
        ZW_ERROR("INVALID_HANDLE\n");
        HY_Res_SDK_UnLock();
        return ERR_INVALID_HANDLE ;
    }

    HY_Res_GetSystemInfo(&pSysInfo);

    /* 参数检查 */
    if (pOSDTimeInfo == NULL)
    {
        ZW_ERROR("INVALID_ARGUMENT pOSDTimeInfo == NULL\n");
        HY_Res_SDK_UnLock();
        return ERR_INVALID_ARGUMENT ;
    }
    if (pOSDTimeInfo->bHour24)		//显示格式：2008-06-06 09:28:32			19个宏块
        BlockNum = 19;
    else							//显示格式：2008-06-06 AM 09:28:32		22个宏块
        BlockNum = 22;
    if ((pOSDTimeInfo->FontWidth <= 0) || (pOSDTimeInfo->FontHeight <= 0) ||(pOSDTimeInfo->FontWidth > 32) || (pOSDTimeInfo->FontHeight > 32))
    {
        ZW_ERROR("INVALID_ARGUMENT FontWidth = %d, FontHeight = %d\n", pOSDTimeInfo->FontWidth, pOSDTimeInfo->FontHeight);
        HY_Res_SDK_UnLock();
        return ERR_INVALID_ARGUMENT ;
    }
    if ((pOSDTimeInfo->Start_X > 352) || (pOSDTimeInfo->Start_Y > 288) || (pOSDTimeInfo->Start_X < 0) || (pOSDTimeInfo->Start_Y < 0))
    {
        ZW_ERROR("Rect Out of display range!!\n");
        HY_Res_SDK_UnLock();
        return ERR_INVALID_ARGUMENT ;
    }

    pSysTime = &pChCtx->m_TimeOSDInfo;

    //判断参数是否改变
    if((pOSDTimeInfo->FontWidth == pSysTime->FontWidth)
    && (pOSDTimeInfo->FontHeight == pSysTime->FontHeight)
    && ((pOSDTimeInfo->Start_X & 0xFFFFFFF8) == pSysTime->rect.left)
    && ((pOSDTimeInfo->Start_Y /*& 0xFFFFFFFE*/) == pSysTime->rect.top)
    && (pOSDTimeInfo->bHour24 == pSysTime->bHour24)
    && (pOSDTimeInfo->DisMod == pSysTime->DisMod))
    {
        ZW_DEBUG("time osd set no change !\n");
        HY_Res_SDK_UnLock();
        return 0;
    }

    XTRACE_MUTEX_LOCK(&pSysInfo->m_SysOSDLock);
    XTRACE_MUTEX_LOCK(&pChCtx->m_OSDLock);

    /* 重新分配15个关键字点阵内存,白色、黑色各一套，到时候直接贴图 */
    // TODO:实际使用只有白色一套，可以去掉黑色关键字的内存占用
    HIEASY_FREE(pSysTime->pWhiteFontInfo);
    MallocSize = pOSDTimeInfo->FontWidth * pOSDTimeInfo->FontHeight * 15 / BYTE_PIXELS;

    pSysTime->pWhiteFontInfo = (char *)malloc(MallocSize);
    if(NULL == pSysTime->pWhiteFontInfo)
    {
        ZW_ERROR("pFontInfo malloc fail, w = %d, h = %d\n", pOSDTimeInfo->FontWidth, pOSDTimeInfo->FontHeight);
        XTRACE_MUTEX_UNLOCK(&pChCtx->m_OSDLock);
        XTRACE_MUTEX_UNLOCK(&pSysInfo->m_SysOSDLock);
        HY_Res_SDK_UnLock();								//释放接口锁
        return ERR_MALLOC_ERROR;
    }

    memset(pSysTime->pWhiteFontInfo, 0, MallocSize);

    /* 转换海思需要的点阵 */
    pSrc	= (char  *)pOSDTimeInfo->pFontPixinfo;
    pDst2	= (unsigned char *)pSysTime->pWhiteFontInfo;
    stride = (pOSDTimeInfo->FontWidth + 7) >> 3;
    for (i = 0; i < 15 * pOSDTimeInfo->FontHeight; i++)
    {
        pixel = pSrc + i * stride;
        ptmp = *pixel;
        bit = 0;
        for (k = 0; k < pOSDTimeInfo->FontWidth; k++)
        {
            if(bit == 0)
            {
                ptmp = *pixel ++;
                bit = 0x80;
            }

            offset_byte = pixelCount / BYTE_PIXELS;
            offset_bit = (pixelCount % BYTE_PIXELS) * PIXEL_BITS;
            if ((ptmp & bit) != 0)
            {
                pDst2[offset_byte] |= (COLOR_WITHE << offset_bit);
            }

            pixelCount ++;
            bit = (bit >> 1) & 0xFF;
        }
    }

    /* 保存配置信息 */
    pSysTime->FontWidth		= pOSDTimeInfo->FontWidth;
    pSysTime->FontHeight	= pOSDTimeInfo->FontHeight;
    pSysTime->bHour24		= pOSDTimeInfo->bHour24;
    pSysTime->DisMod		= pOSDTimeInfo->DisMod;
    pSysTime->rect.left		= pOSDTimeInfo->Start_X & 0xFFFFFFF8;
    pSysTime->rect.top		= pOSDTimeInfo->Start_Y ;//& 0xFFFFFFFE;
    pSysTime->rect.right	= pSysTime->rect.left + pSysTime->FontWidth * BlockNum;
    pSysTime->rect.bottom	= pSysTime->rect.top + pSysTime->FontHeight;

    ret = HY_Enc_SetTimeOSDBitMap(pChCtx, pSysTime, MAX_STREAM_COUNT, pChCtx->m_bEnableTimeOSD);
    if(0 != ret)
    {
        XTRACE_MUTEX_UNLOCK(&pChCtx->m_OSDLock);
        XTRACE_MUTEX_UNLOCK(&pSysInfo->m_SysOSDLock);
        HY_Res_SDK_UnLock();								//释放接口锁
        ZW_ERROR("SetTimeOSDBitMap fail !\n");
        return -1;
    }
    pChCtx->m_bCreatTimeOSDRegion = TRUE;
    ZW_DEBUG("SetTimeOSDBitMap ok !\n");
    XTRACE_MUTEX_UNLOCK(&pChCtx->m_OSDLock);
    XTRACE_MUTEX_UNLOCK(&pSysInfo->m_SysOSDLock);
    HY_Res_SDK_UnLock();								//释放接口锁
    return HI_SDK_SUCESS;
}

//-----------------------------------------------------------------------------
//	说明：		启动或停止OSD
//	输入参数：	hChannelHandle	对应通道句柄；
//	输入参数：	Enable			大于0为启动，0为关闭
//	返回:		成功为 0, 失败-1；
//-----------------------------------------------------------------------------
int     MP4Sys_EncSetOsd(void *hChannelHandle, int EnableTime, int EnableText)
{
    int ret = 0;
    CChannelContext		*pChCtx = (CChannelContext*)hChannelHandle;
    SystemInfo_t		*pSysInfo = NULL;

    ZW_INFO("--------into MP4Sys_EncSetOsd---EnableTime=%d---EnableText=%d------\n", EnableTime, EnableText);
#ifdef NO_SET_OSD
    return 0;
#endif
    /* 接口加锁 */
    if (HY_Res_SDK_Lock() != 0)
    {
        ZW_ERROR("SDK Not Init\n");
        return ERR_SDK_NOINIT;
    }

    /* 参数检查 */
    if( !HY_Enc_IsEncHandleOk( hChannelHandle ) )
    {
        ZW_ERROR("INVALID_HANDLE\n");
        HY_Res_SDK_UnLock();
        return ERR_INVALID_HANDLE ;
    }

    //参数是否改变
    if((EnableTime == pChCtx->m_bEnableTimeOSD) && (EnableText == pChCtx->m_bEnableTextOSD))
    {
        ZW_DEBUG("osd enable no change !\n");
        HY_Res_SDK_UnLock();
        return 0 ;
    }

    HY_Res_GetSystemInfo(&pSysInfo);

    XTRACE_MUTEX_LOCK(&pSysInfo->m_SysOSDLock);
    XTRACE_MUTEX_LOCK(&pChCtx->m_OSDLock);
    /* 判断海思的MPP区域是否可用，能用则做相应操作，不能用则只是设置标记，等设置内容后再启用 */
    if(EnableTime == pChCtx->m_bEnableTimeOSD)
    {
        ZW_DEBUG("time osd enable no change !\n");
    }
    else
    {
        pChCtx->m_bEnableTimeOSD = EnableTime?TRUE:FALSE;

        ret = HY_Enc_SetTimeOSDBitMap(pChCtx, &pChCtx->m_TimeOSDInfo, MAX_STREAM_COUNT, pChCtx->m_bEnableTimeOSD);
        if (0 != ret)
        {
            ZW_ERROR("%s time osd fail !\n", EnableTime ? "start" : "stop");
        }
    }


    if(EnableText == pChCtx->m_bEnableTextOSD)
    {
        ZW_DEBUG("text osd 0 enable no change!\n");
    }
    else
    {
        pChCtx->m_bEnableTextOSD = EnableText?TRUE:FALSE;

        ret = HY_Enc_SetTextOSDBitMap(pChCtx, MAX_STREAM_COUNT, pChCtx->m_bEnableTextOSD);
        if (0 != ret)
        {
            ZW_ERROR("%s text osd fail !\n", EnableText ? "start" : "stop");
        }
    }

    XTRACE_MUTEX_UNLOCK(&pChCtx->m_OSDLock);
    XTRACE_MUTEX_UNLOCK(&pSysInfo->m_SysOSDLock);
    HY_Res_SDK_UnLock();								//释放接口锁
    return HI_SDK_SUCESS;
}

//人形跟踪使能开关
int     MP4Sys_EncSetHumanTrackEn(void *hChannelHandle, int bEnable)
{
    CChannelContext 	*pChannelContext = (CChannelContext*)hChannelHandle;

    ZW_DEBUG("--------into MP4Sys_EncSetHumanTrackEn bEnable=[%d]--------\n", bEnable);

#ifdef NO_SET_HUMAN_DETECT
    return 0;
#endif

    /* 接口加锁 */
    if (HY_Res_SDK_Lock() != 0)
    {
        ZW_ERROR("SDK Not Init\n");
        return ERR_SDK_NOINIT;
    }

    /* 参数检查 */
    if( !HY_Enc_IsEncHandleOk( hChannelHandle ) )
    {
        ZW_ERROR("INVALID_HANDLE\n");
        HY_Res_SDK_UnLock();
        return ERR_INVALID_HANDLE ;
    }

    HY_Enc_SetHumanTrackEn(pChannelContext, bEnable);

    HY_Res_SDK_UnLock();
    return 0;
}

//人形检测使能开关
int     MP4Sys_EncSetHumanDetectEnable(void *hChannelHandle, int bEnable)
{
    CChannelContext 	*pChannelContext = (CChannelContext*)hChannelHandle;

    ZW_DEBUG("--------into MP4Sys_EncSetHumanDetectEnable bEnable=[%d]--------\n", bEnable);

#ifdef NO_SET_HUMAN_DETECT
    return 0;
#endif

    /* 接口加锁 */
    if (HY_Res_SDK_Lock() != 0)
    {
        ZW_ERROR("SDK Not Init\n");
        return ERR_SDK_NOINIT;
    }

    /* 参数检查 */
    if( !HY_Enc_IsEncHandleOk( hChannelHandle ) )
    {
        ZW_ERROR("INVALID_HANDLE\n");
        HY_Res_SDK_UnLock();
        return ERR_INVALID_HANDLE ;
    }

    XTRACE_MUTEX_LOCK(&pChannelContext->m_SensorLock);
    XTRACE_MUTEX_LOCK(&pChannelContext->m_EncLock);
    XTRACE_MUTEX_LOCK(&pChannelContext->m_HumanShapeLock);
    if (pChannelContext->m_bStartHuman != bEnable)
    {
        pChannelContext->m_bStartHuman = bEnable;
        for(int i = 0;i < ALGO_CHANNEL;i++)
        {
            pChannelContext->m_bHumanShapeState[i] = FALSE;
            if(i == 0)
            pChannelContext->m_bHumanStateEx = pChannelContext->m_bHumanShapeState[i];
        }
        pChannelContext->m_bHumanStateIsValue = TRUE;
        HY_Enc_SetHumanDetectEnable(pChannelContext, bEnable);
    }
    XTRACE_MUTEX_UNLOCK(&pChannelContext->m_HumanShapeLock);
    XTRACE_MUTEX_UNLOCK(&pChannelContext->m_EncLock);
    XTRACE_MUTEX_UNLOCK(&pChannelContext->m_SensorLock);

    HY_Res_SDK_UnLock();
    return 0;
}

int MP4Sys_SetHumanOverlayInfo(void *hChannelHandle, char nMainStreamHumanOverlay, char nSubStreamHumanOverlay)
{
    int nRet = 0;
    CChannelContext 	*pChannelContext = (CChannelContext*)hChannelHandle;

    ZW_DEBUG("--------into MP4Sys_SetHumanOverlayInfo-----%d--%d-\n", nMainStreamHumanOverlay,     nSubStreamHumanOverlay);
#ifdef NO_SET_HUMAN_DETECT
    return 0;
#endif
    /* 接口加锁 */
    if (HY_Res_SDK_Lock() != 0)
    {
        ZW_ERROR("SDK Not Init\n");
        return ERR_SDK_NOINIT;
    }

    /* 参数检查 */
    if (!HY_Enc_IsEncHandleOk(hChannelHandle))
    {
        ZW_ERROR("INVALID_HANDLE\n");
        HY_Res_SDK_UnLock();
        return ERR_INVALID_HANDLE;
    }
    pChannelContext->m_HumanAreaOverlayEnable[FIRST_STREAM] = nMainStreamHumanOverlay;
    pChannelContext->m_HumanAreaOverlayEnable[SECOND_STREAM] = nSubStreamHumanOverlay;
    pChannelContext->m_HumanAreaOverlayEnable[THIRD_STREAM] = nSubStreamHumanOverlay;//第二子码流与子码流保持一致

    HY_Res_SDK_UnLock();
    return nRet;
}

int     MP4Sys_EncSetHumanShapeCfgV2(void *hChannelHandle, LpHumanShapeCfgV2 pCfg)
{
    int nRet = 0;
    CChannelContext 	*pChannelContext = (CChannelContext*)hChannelHandle;
    HumanShapeCfgV2       stHumanShapeCfg = {0};
    int i = 0;
    int j = 0;

    ZW_DEBUG("--------into MP4Sys_EncSetHumanShapeCfgV2----nSenstive %d----\n", pCfg->nSenstive);
#ifdef NO_SET_HUMAN_DETECT
    return 0;
#endif

    /* 接口加锁 */
    if (HY_Res_SDK_Lock() != 0)
    {
        ZW_ERROR("SDK Not Init\n");
        return ERR_SDK_NOINIT;
    }

    /* 参数检查 */
    if (!HY_Enc_IsEncHandleOk(hChannelHandle))
    {
        ZW_ERROR("INVALID_HANDLE\n");
        HY_Res_SDK_UnLock();
        return ERR_INVALID_HANDLE;
    }
    if (NULL == pCfg)
    {
        ZW_ERROR("NULL == pCfg\n");
        HY_Res_SDK_UnLock();
        return ERR_INVALID_ARGUMENT;
    }

    memset(&stHumanShapeCfg, 0, sizeof(stHumanShapeCfg));

    if (pCfg->nSenstive >= MAX_HUMAN_SHAPE_SENSTIVE_NUM)
    {
        pCfg->nSenstive = MAX_HUMAN_SHAPE_SENSTIVE_NUM - 1;
    }
    else if (pCfg->nSenstive < 0)
    {
        pCfg->nSenstive = 0;
    }
    stHumanShapeCfg.nSenstive = pCfg->nSenstive;

    for (i = 0; i < pCfg->nDetectRegionNum; i++)
    {
        POLYGON poly = pCfg->rcDetectRegion[i];
        for (j = 0; j < poly.nValidNum; j++)
        {
            VECTOR vector = poly.point[j];
            if (vector.x < 0)
            {
                vector.x = 0;
            }
            if (vector.x > MAX_HUMAN_SHAPE_REGION_WIDTH)
            {
                vector.x = MAX_HUMAN_SHAPE_REGION_WIDTH;
            }
            if (vector.y < 0)
            {
                vector.y = 0;
            }
            if (vector.y > MAX_HUMAN_SHAPE_REGION_HEIGHT)
            {
                vector.y = MAX_HUMAN_SHAPE_REGION_HEIGHT;
            }
        }

        stHumanShapeCfg.rcDetectRegion[stHumanShapeCfg.nDetectRegionNum] = pCfg->rcDetectRegion[i];
        stHumanShapeCfg.nDetectRegionNum++;
    }

    XTRACE_MUTEX_LOCK(&pChannelContext->m_HumanShapeLock);
    if (0 != memcmp(&(pChannelContext->m_stHumanShapeCfgV2), &stHumanShapeCfg,
        sizeof(pChannelContext->m_stHumanShapeCfgV2)))
    {
        pChannelContext->m_stHumanShapeCfgV2 = stHumanShapeCfg;
        for(int i = 0;i < ALGO_CHANNEL;i++)
        {
            pChannelContext->m_bHumanShapeState[i] = FALSE;
            if(i == 0)
            pChannelContext->m_bHumanStateEx = pChannelContext->m_bHumanShapeState[i];
        }
        pChannelContext->m_bHumanStateIsValue = TRUE;
        nRet = HY_Enc_SetHumanShapeCfgV2(pChannelContext);
    }
    XTRACE_MUTEX_UNLOCK(&pChannelContext->m_HumanShapeLock);

    HY_Res_SDK_UnLock();
    return nRet;
}

int     MP4Sys_EncSetDetectSchedule(void *hChannelHandle, int enable)
{
    int nRet = 0;
    CChannelContext     *pChCtx = (CChannelContext *)hChannelHandle;

    ZW_DEBUG("--------into MP4Sys_EncSetDetectSchedule--------\n");
#ifdef NO_SET_HUMAN_DETECT
    return 0;
#endif
    /* 接口加锁 */
    if (HY_Res_SDK_Lock() != 0)
    {
        ZW_ERROR("SDK Not Init\n");
        return ERR_SDK_NOINIT;
    }

    /* 参数检查 */
    if (!HY_Enc_IsEncHandleOk(hChannelHandle))
    {
        ZW_ERROR("INVALID_HANDLE\n");
        HY_Res_SDK_UnLock();
        return ERR_INVALID_HANDLE;
    }
    pChCtx->m_HumanDetectSchedule = enable;

    HY_Res_SDK_UnLock();
    return nRet;
}

//人形布防配置
int     MP4Sys_EncSetHumanShapeSchedule(void *hChannelHandle, LpAiScehdule Schedule)
{
    int nRet = 0;
    CChannelContext 	*pChannelContext = (CChannelContext *)hChannelHandle;

    ZW_DEBUG("--------into MP4Sys_EncSetHumanShapeCfg--------\n");
#ifdef NO_SET_HUMAN_DETECT
    return 0;
#endif
    /* 接口加锁 */
    if (HY_Res_SDK_Lock() != 0)
    {
        ZW_ERROR("SDK Not Init\n");
        return ERR_SDK_NOINIT;
    }

    /* 参数检查 */
    if (!HY_Enc_IsEncHandleOk(hChannelHandle))
    {
        ZW_ERROR("INVALID_HANDLE\n");
        HY_Res_SDK_UnLock();
        return ERR_INVALID_HANDLE;
    }
    if (NULL == Schedule)
    {
        ZW_ERROR("NULL == cfg\n");
        HY_Res_SDK_UnLock();
        return ERR_INVALID_ARGUMENT;
    }

    memcpy(&pChannelContext->m_stHumanShapeSchedule, Schedule, sizeof(AiScehdule));

    HY_Res_SDK_UnLock();
    return nRet;
}

//-----------------------------------------------------------------------------
//	说明：		按照宏块设置运动检测区域
//	输入参数：	hChannelHandle		对应通道句柄
//	输入参数：	listOfAreas			矩形区域数组(共192个数)
//	返回： 		成功为 0 , 其它为定义的错误号
//-----------------------------------------------------------------------------
int     MP4Sys_EncSetupMotionDetection( void *hChannelHandle, unsigned char *listOfAreas)
{
    CChannelContext		*pChannelContext = (CChannelContext*)hChannelHandle;
    SystemInfo_t		*pSysInfo;
    int i = 0, j = 0;
    char setBlock[192] = {0};

    ZW_INFO("--------into MP4Sys_EncSetupMotionDetection--------\n");
#ifdef	NO_SET_MOTIONDETECT
    return 0;
#endif
    /* 接口加锁 */
    if (HY_Res_SDK_Lock() != 0)
    {
        ZW_ERROR("SDK Not Init\n");
        return ERR_SDK_NOINIT;
    }
    /* 参数检查 */
    if( !HY_Enc_IsEncHandleOk( hChannelHandle ) )
    {
        ZW_ERROR("INVALID_HANDLE\n");
        HY_Res_SDK_UnLock();
        return ERR_INVALID_HANDLE ;
    }
    if (listOfAreas == NULL)
    {
        ZW_ERROR("INVALID_ARGUMENT listOfAreas is NULL\n");
        HY_Res_SDK_UnLock();
        return ERR_INVALID_ARGUMENT ;
    }

    //将按位保存的方式转换为按字节保存
    for(i=0;i<24;i++)
    {
        for(j=0;j<8;j++)
        {
            if(listOfAreas[i] & (1 << j))
                setBlock[(i<<3) + j] = 1;
            else
                setBlock[(i<<3) + j] = 0;
        }
    }

#if 0
    if(0 == strncmp(setBlock, pChannelContext->pUsrBlock, sizeof(setBlock)))
    {
        ZW_DEBUG("Param not change!\n");
        HY_Res_SDK_UnLock();
        return 0 ;
    }
    else
#endif
    {
        memcpy(pChannelContext->pUsrBlock, setBlock, sizeof(setBlock));
    }

    HY_Res_GetSystemInfo(&pSysInfo);
    HY_Res_RD_Lock(&pSysInfo->m_SysTemConfigLock);			//加锁禁止系统修改制式
    XTRACE_MUTEX_LOCK(&pChannelContext->m_SensorLock);
    XTRACE_MUTEX_LOCK(&pChannelContext->m_EncLock);
    XTRACE_MUTEX_LOCK(&pChannelContext->m_MDLock);

    pChannelContext->m_bMotionState		= FALSE;

    XTRACE_MUTEX_UNLOCK(&pChannelContext->m_MDLock);
    XTRACE_MUTEX_UNLOCK(&pChannelContext->m_EncLock);
    XTRACE_MUTEX_UNLOCK(&pChannelContext->m_SensorLock);
    HY_Res_RD_UnLock(&pSysInfo->m_SysTemConfigLock);
    HY_Res_SDK_UnLock();								//释放接口锁
    ZW_DEBUG("MP4Sys_EncSetupMotionDetection ok !\n");

    return HI_SDK_SUCESS;
}

//移动布防配置
int     MP4Sys_EncSetMotionSchedule(void *hChannelHandle, LpAiScehdule Schedule)
{
    int nRet = 0;
    CChannelContext 	*pChannelContext = (CChannelContext *)hChannelHandle;

    ZW_DEBUG("--------into MP4Sys_EncSetMotionSchedule--------\n");
#ifdef	NO_SET_MOTIONDETECT
    return 0;
#endif
    /* 接口加锁 */
    if (HY_Res_SDK_Lock() != 0)
    {
        ZW_ERROR("SDK Not Init\n");
        return ERR_SDK_NOINIT;
    }

    /* 参数检查 */
    if (!HY_Enc_IsEncHandleOk(hChannelHandle))
    {
        ZW_ERROR("INVALID_HANDLE\n");
        HY_Res_SDK_UnLock();
        return ERR_INVALID_HANDLE;
    }
    if (NULL == Schedule)
    {
        ZW_ERROR("NULL == cfg\n");
        HY_Res_SDK_UnLock();
        return ERR_INVALID_ARGUMENT;
    }

    memcpy(&pChannelContext->m_stMotionSchedule, Schedule, sizeof(AiScehdule));

    HY_Res_SDK_UnLock();
    return nRet;
}

//-----------------------------------------------------------------------------
//	说明：		启动对应通道的运动检测
//	输入参数：	hChannelHandle		对应通道句柄
//	返回： 		成功为 0 , 其它为定义的错误号
//-----------------------------------------------------------------------------
int     MP4Sys_EncStartMotionDetection(void *hChannelHandle)
{
    CChannelContext		*pChCtx = (CChannelContext*)hChannelHandle;
    SystemInfo_t		*pSysInfo;

    ZW_DEBUG("--------into MP4Sys_EncStartMotionDetection--------\n");
#ifdef	NO_SET_MOTIONDETECT
        return 0;
#endif

    /* 接口加锁 */
    if (HY_Res_SDK_Lock() != 0)
    {
        ZW_ERROR("SDK Not Init\n");
        return ERR_SDK_NOINIT;
    }
    /* 参数检查 */
    if( !HY_Enc_IsEncHandleOk( hChannelHandle ) )
    {
        ZW_ERROR("INVALID_HANDLE\n");
        HY_Res_SDK_UnLock();
        return ERR_INVALID_HANDLE ;
    }

    /* 获取运动数据之后不能停止运动检测，否则会导致程序异常，将运动分析、运动设置、制式切换三者互斥 */
    HY_Res_GetSystemInfo(&pSysInfo);
    HY_Res_RD_Lock(&pSysInfo->m_SysTemConfigLock);			//加锁禁止系统修改制式
    XTRACE_MUTEX_LOCK(&pChCtx->m_SensorLock);
    XTRACE_MUTEX_LOCK(&pChCtx->m_EncLock);
    XTRACE_MUTEX_LOCK(&pChCtx->m_MDLock);

    if (!pChCtx->m_bStartMDForDL)
    {
        pChCtx->m_bStartMDForDL	= TRUE; 	// 给智能双光判断使用
    }

    if (!pChCtx->m_bStartMD)  // 如果移动侦测原来关闭，则打开移动侦测
    {
        if(0 != HY_Enc_MD_Start(pChCtx))
           {
            XTRACE_MUTEX_UNLOCK(&pChCtx->m_MDLock);
            XTRACE_MUTEX_UNLOCK(&pChCtx->m_EncLock);
            XTRACE_MUTEX_UNLOCK(&pChCtx->m_SensorLock);
            HY_Res_RD_UnLock(&pSysInfo->m_SysTemConfigLock);
            HY_Res_SDK_UnLock();								//释放接口锁
            ZW_ERROR("MP4Sys_EncStartMotionDetection fail !\n");
            return -1;
        }
        pChCtx->m_bStartMD		= TRUE;  // 打开移动侦测
        pChCtx->m_bMotionState	= FALSE; // 默认没有移动
    }
    pChCtx->m_bStartMidMotion = TRUE; // 中间层关闭移动侦测
    XTRACE_MUTEX_UNLOCK(&pChCtx->m_MDLock);
    XTRACE_MUTEX_UNLOCK(&pChCtx->m_EncLock);
    XTRACE_MUTEX_UNLOCK(&pChCtx->m_SensorLock);
    HY_Res_RD_UnLock(&pSysInfo->m_SysTemConfigLock);
    HY_Res_SDK_UnLock();								//释放接口锁
    ZW_INFO("MP4Sys_EncStartMotionDetection ok !\n");

    return HI_SDK_SUCESS;
}

//-----------------------------------------------------------------------------
//	说明：		停止对应通道的运动检测
//	输入参数：	hChannelHandle		对应通道句柄
//	返回： 		成功为 0 , 其它为定义的错误号
//-----------------------------------------------------------------------------
int     MP4Sys_EncStopMotionDetection(void *hChannelHandle)
{
    CChannelContext		*pChCtx = (CChannelContext*)hChannelHandle;
    SystemInfo_t		*pSysInfo;

    ZW_DEBUG("--------into MP4Sys_EncStopMotionDetection--------\n");
#ifdef	NO_SET_MOTIONDETECT
        return 0;
#endif

    /* 接口加锁 */
    if (HY_Res_SDK_Lock() != 0)
    {
        ZW_ERROR("SDK Not Init\n");
        return ERR_SDK_NOINIT;
    }
    /* 参数检查 */
    if( !HY_Enc_IsEncHandleOk( hChannelHandle ) )
    {
        ZW_ERROR("INVALID_HANDLE\n");
        HY_Res_SDK_UnLock();
        return ERR_INVALID_HANDLE ;
    }

    /* 获取运动数据之后不能停止运动检测，否则会导致程序异常，将运动分析、运动设置、制式切换三者互斥 */
    HY_Res_GetSystemInfo(&pSysInfo);
    HY_Res_RD_Lock(&pSysInfo->m_SysTemConfigLock);			//加锁禁止系统修改制式
    XTRACE_MUTEX_LOCK(&pChCtx->m_SensorLock);
    XTRACE_MUTEX_LOCK(&pChCtx->m_EncLock);
    XTRACE_MUTEX_LOCK(&pChCtx->m_MDLock);

    if (pChCtx->m_bStartMDForDL)
    {
        pChCtx->m_bStartMDForDL	= FALSE;	// 给智能双光判断使用
    }

    if (pChCtx->m_bStartMD)
    {
        if(0 != HY_Enc_MD_Stop(pChCtx))
           {
            XTRACE_MUTEX_UNLOCK(&pChCtx->m_MDLock);
            XTRACE_MUTEX_UNLOCK(&pChCtx->m_EncLock);
            XTRACE_MUTEX_UNLOCK(&pChCtx->m_SensorLock);
            HY_Res_RD_UnLock(&pSysInfo->m_SysTemConfigLock);
            HY_Res_SDK_UnLock();								//释放接口锁
            ZW_ERROR("MP4Sys_EncStopMotionDetection fail !\n");
            return HI_SDK_SUCESS;
        }
        pChCtx->m_bStartMD		= FALSE;
        pChCtx->m_bMotionState	= FALSE;
    }
    pChCtx->m_bStartMidMotion = FALSE; // 中间层关闭移动侦测
    XTRACE_MUTEX_UNLOCK(&pChCtx->m_MDLock);
    XTRACE_MUTEX_UNLOCK(&pChCtx->m_EncLock);
    XTRACE_MUTEX_UNLOCK(&pChCtx->m_SensorLock);
    HY_Res_RD_UnLock(&pSysInfo->m_SysTemConfigLock);
    HY_Res_SDK_UnLock();								//释放接口锁
    ZW_DEBUG("MP4Sys_EncStopMotionDetection ok !\n");

    return HI_SDK_SUCESS;
}

//-----------------------------------------------------------------------------
//	说明：		调整运动分析灵敏度
//	输入参数：	hChannelHandle	对应通道句柄
//	输入参数：	iGrade			运动分析灵敏度等级（0 - 高，1 - 中， 2 - 低）
//	输入参数：	iMotionDetectFps	高速运动检测的帧间隔，取值范围 0-12；0 表示不作高速运动检测，通常值为4；
//	返回： 		成功为 0 , 其它为定义的错误号
//-----------------------------------------------------------------------------
int     MP4Sys_EncAdjustMotionDetectPrecision(void *hChannelHandle, int iGrade, int iMotionDetectFps)
{
    BOOL				bParaOk = TRUE;
    CChannelContext		*pChannelContext = (CChannelContext*)hChannelHandle;
    SystemInfo_t		*pSysInfo;

    ZW_INFO("--------into MP4Sys_EncAdjustMotionDetectPrecision-iGrade:%d--iMotionDetectFps:%d------\n", iGrade, iMotionDetectFps);
#ifdef	NO_SET_MOTIONDETECT
        return 0;
#endif

    /* 接口加锁 */
    if (HY_Res_SDK_Lock() != 0)
    {
        ZW_ERROR("SDK Not Init\n");
        return ERR_SDK_NOINIT;
    }
    /* 参数检查 */
    if( !HY_Enc_IsEncHandleOk( hChannelHandle ) )
    {
        ZW_ERROR("INVALID_HANDLE\n");
        HY_Res_SDK_UnLock();
        return ERR_INVALID_HANDLE ;
    }
    if ((iMotionDetectFps < 0) || (iMotionDetectFps > 255))
    {
        ZW_ERROR("iMotionDetectFps = %d\n", iMotionDetectFps);
        bParaOk = FALSE;
    }
    if ((iGrade < 0) || (iGrade > 6))
    {
        ZW_ERROR("iGrade Invalid : %d\n", iGrade);
        bParaOk = FALSE;
    }
    if (!bParaOk)
    {
        HY_Res_SDK_UnLock();
        return ERR_INVALID_ARGUMENT ;
    }

    if(pChannelContext->MotionGrade == iGrade)
    {
        ZW_DEBUG("Param not change!\n");
        HY_Res_SDK_UnLock();
        return 0 ;
    }

    /* 获取运动数据之后不能停止运动检测，否则会导致程序异常，将运动分析、运动设置、制式切换三者互斥 */
    HY_Res_GetSystemInfo(&pSysInfo);
    HY_Res_RD_Lock(&pSysInfo->m_SysTemConfigLock);			//加锁禁止系统修改制式
    XTRACE_MUTEX_LOCK(&pChannelContext->m_SensorLock);
    XTRACE_MUTEX_LOCK(&pChannelContext->m_EncLock);
    XTRACE_MUTEX_LOCK(&pChannelContext->m_MDLock);

    if (pChannelContext->MotionGrade != iGrade)
    {
        pChannelContext->m_bMotionState = FALSE;
        pChannelContext->MotionGrade = iGrade;

        if (pChannelContext->m_bStartMD)
        {
            HY_Enc_MD_SetPrecision(pChannelContext);
        }
    }

    XTRACE_MUTEX_UNLOCK(&pChannelContext->m_MDLock);
    XTRACE_MUTEX_UNLOCK(&pChannelContext->m_EncLock);
    XTRACE_MUTEX_UNLOCK(&pChannelContext->m_SensorLock);
    HY_Res_RD_UnLock(&pSysInfo->m_SysTemConfigLock);
    HY_Res_SDK_UnLock();								//释放接口锁
    return HI_SDK_SUCESS;
}

/* 设置抓图参数,此函数无效不允许随机设置抓图码流的分辨率，默认跟随子码流 */
int     MP4Sys_EncSetSnapPara(void *hChannelHandle, ResolutionParam_t *PictureRes, int nQuality)
{
    ZW_INFO("--------MP4Sys_EncSetSnapPara succ--------\n");
    return HI_SDK_SUCESS;
}

int     MP4Sys_EncSetSnapParaV2(void *hChannelHandle, int nQuality, char nStreamType)
{
    int s32Ret = 0;
    SystemInfo_t		*pSysInfo = NULL;
    CChannelContext 	*pChCtx = (CChannelContext*)hChannelHandle;

    ZW_INFO("--------into MP4Sys_EncSetSnapParaV2--------\n");
#ifdef NO_CHANGE_ENCODE
    return 0;
#endif

    if (0 == pChCtx->m_bSupportFourthStream)
    {
        ZW_INFO("Not Support Setting Fourth Stream For Now!\n");
        return 0;
    }

    if ((nStreamType < SameAsMainStream) || (nStreamType > SameAsAuxStream) || (nQuality < 0) || (nQuality > 2))
    {
        ZW_ERROR("Invalid parm nStreamType=%d, nQuality=%d\n", nStreamType, nQuality);
        return ERR_INVALID_ARGUMENT ;
    }

    if (!pChCtx)
    {
        ZW_ERROR("pChannelContext is NULL\n");
        return ERR_INVALID_ARGUMENT ;
    }

    /* 接口加锁 */
    if (HY_Res_SDK_Lock() != 0)
    {
        ZW_ERROR("SDK Not Init\n");
        return ERR_SDK_NOINIT;
    }

    /* 参数检查 */
    if(!HY_Enc_IsEncHandleOk(hChannelHandle))
    {
        ZW_ERROR("INVALID_HANDLE\n");
        HY_Res_SDK_UnLock();
        return ERR_INVALID_HANDLE ;
    }

    /* 只有启动编码之后才能设置编码参数 */
    if(TRUE != pChCtx->m_bRecordFourth)
    {
        SDKPRINT("please start capture encode first !\n");
        HY_Res_SDK_UnLock();
        return -1 ;
    }

    HY_Res_GetSystemInfo(&pSysInfo);
    HY_Res_RD_Lock(&pSysInfo->m_SysTemConfigLock);

    XTRACE_MUTEX_LOCK(&pChCtx->m_SensorLock);
    XTRACE_MUTEX_LOCK(&pChCtx->m_EncLock);
    XTRACE_MUTEX_LOCK(&pSysInfo->m_SysOSDLock);

    //mjpeg抓图超大帧的问题还未解决,暂时只能同子码流
    nStreamType = SameAsAuxStream;

    if(SameAsMainStream == nStreamType)
    {
        if(JPEG_QUALITY_HIGH == nQuality)
        {
            pChCtx->m_EncAttrFourth.PicQualityLever = MAIN_CAPTURE_QUALITY_HIGH;
        }
        else if(JPEG_QUALITY_MID == nQuality)
        {
            pChCtx->m_EncAttrFourth.PicQualityLever = MAIN_CAPTURE_QUALITY_MID;
        }
        else if (JPEG_QUALITY_LOW == nQuality)
        {
            pChCtx->m_EncAttrFourth.PicQualityLever = MAIN_CAPTURE_QUALITY_LOW;
        }
    }
    else if(SameAsAuxStream == nStreamType)
    {
        if(JPEG_QUALITY_HIGH == nQuality)
        {
            pChCtx->m_EncAttrFourth.PicQualityLever = AUX_CAPTURE_QUALITY_HIGH;
        }
        else if(JPEG_QUALITY_MID == nQuality)
        {
            pChCtx->m_EncAttrFourth.PicQualityLever = AUX_CAPTURE_QUALITY_MID;
        }
        else if (JPEG_QUALITY_LOW == nQuality)
        {
            pChCtx->m_EncAttrFourth.PicQualityLever = AUX_CAPTURE_QUALITY_LOW;
        }
    }

    if (nStreamType == pChCtx->m_CaptureType)
    {
        ZW_DEBUG("Param is not change!\n");
        XTRACE_MUTEX_UNLOCK(&pSysInfo->m_SysOSDLock);
        XTRACE_MUTEX_UNLOCK(&pChCtx->m_EncLock);
        XTRACE_MUTEX_UNLOCK(&pChCtx->m_SensorLock);
        HY_Res_RD_UnLock(&pSysInfo->m_SysTemConfigLock);	//允许系统修改制式
        HY_Res_SDK_UnLock();								//释放接口锁
        return 0;
    }
    pChCtx->m_CaptureType = nStreamType;

    XTRACE_MUTEX_LOCK(&pSysInfo->m_WriteFileLock);
    s32Ret = HY_Enc_SwitchPicFormat(pChCtx, FOURTH_STREAM);
    if (s32Ret)
    {
        ZW_ERROR("FOURTH_STREAM Fail to called HY_Enc_SwitchPicFormat! ret = %#x\n", s32Ret);
        XTRACE_MUTEX_UNLOCK(&pSysInfo->m_WriteFileLock);
        XTRACE_MUTEX_UNLOCK(&pSysInfo->m_SysOSDLock);
        XTRACE_MUTEX_UNLOCK(&pChCtx->m_EncLock);
        XTRACE_MUTEX_UNLOCK(&pChCtx->m_SensorLock);
        HY_Res_RD_UnLock(&pSysInfo->m_SysTemConfigLock);	//允许系统修改制式
        HY_Res_SDK_UnLock();								//释放接口锁
        return s32Ret;
    }

    HY_Enc_AdjustEncCap();          //自适应调整资源配置

    XTRACE_MUTEX_UNLOCK(&pSysInfo->m_WriteFileLock);
    XTRACE_MUTEX_UNLOCK(&pSysInfo->m_SysOSDLock);
    XTRACE_MUTEX_UNLOCK(&pChCtx->m_EncLock);
    XTRACE_MUTEX_UNLOCK(&pChCtx->m_SensorLock);
    HY_Res_RD_UnLock(&pSysInfo->m_SysTemConfigLock);	//允许系统修改制式
    HY_Res_SDK_UnLock();								//释放接口锁

    ZW_INFO("--------MP4Sys_EncSetSnapParaV2 succ--------\n");

    return HI_SDK_SUCESS;
}

//-----------------------------------------------------------------------------
//	说明：		设置报警输出生效与否，目前仅用于产测时立即生效
//	返回:		成功为 0, 失败-1；
//-----------------------------------------------------------------------------
int     MP4Sys_AlarmOutSupport(int nSupport)
{
    SDKPRINT("--------into MP4Sys_AlarmOutEn----nEnable = %d--\n", nSupport);

    /* 接口加锁 */
    if (HY_Res_SDK_Lock() != 0)
    {
        ZW_ERROR("SDK Not Init\n");
        return ERR_SDK_NOINIT;
    }

    //处理
    HY_Enc_SetAlarmOutSupport(nSupport);
    HY_Res_SDK_UnLock();

    return HI_SDK_SUCESS;
}

//-----------------------------------------------------------------------------
//	说明：		设置报警输出高低开，目前仅用于产测时立即生效
//	返回:		成功为 0, 失败-1；
//-----------------------------------------------------------------------------
int     MP4Sys_AlarmOutDefaultStatus(int nStatus)
{
    SDKPRINT("--------into AlarmOutDefaultStatus----nStatus = %d--\n", nStatus);

    /* 接口加锁 */
    if (HY_Res_SDK_Lock() != 0)
    {
        ZW_ERROR("SDK Not Init\n");
        return ERR_SDK_NOINIT;
    }

    //处理
    HY_Enc_SetAlarmOutDefaultStatus(nStatus);
    HY_Res_SDK_UnLock();

    return HI_SDK_SUCESS;
}

//-----------------------------------------------------------------------------
//	说明：		设置继电器状态
//	输入参数：	nPort	指定报警输出通道的编号
//	输入参数：	isOpen	设置报警的输出状态，为 0，则继电器闭合，为 1，则继电器断开
//	返回:		成功为 0, 失败-1；
//  注意:		该接口为频繁调用接口，简单保护
//-----------------------------------------------------------------------------
int     MP4Sys_AlarmOut (long nPort, int bOpen)
{
    SDKPRINT("--------into MP4Sys_AlarmOut----nPort %ld--bOpen %d--\n", nPort, bOpen);

    /* 接口加锁 */
    if (HY_Res_SDK_Lock() != 0)
    {
        ZW_ERROR("SDK Not Init\n");
        return ERR_SDK_NOINIT;
    }

    //处理
    HY_Enc_SetAlarmOut(nPort,bOpen);
    HY_Res_SDK_UnLock();

    return HI_SDK_SUCESS;
}

int     MP4Sys_LightAlarm (int nMode, int bOpen)
{
    SDKPRINT("--------into MP4Sys_LightAlarm----nMode %d--bOpen %d--\n", nMode, bOpen);

    /* 接口加锁 */
    if (HY_Res_SDK_Lock() != 0)
    {
        ZW_ERROR("SDK Not Init\n");
        return ERR_SDK_NOINIT;
    }

    //处理
    HY_Enc_SetLightAlarm(nMode, bOpen);
    HY_Res_SDK_UnLock();

    return HI_SDK_SUCESS;
}

int  MP4Sys_GetAdcStatus(int *nAdcStatus)
{
    SDKPRINT("--------into MP4Sys_GetAdcStatus ------");
    /* 接口加锁 */
    if (HY_Res_SDK_Lock() != 0) {
        ZW_ERROR("SDK Not Init\n");
        return ERR_SDK_NOINIT;
    }

    //处理
    Hi_MPP_GetIRStateLevel((UINT32 *)nAdcStatus);
    HY_Res_SDK_UnLock();
    return HI_SDK_SUCESS;
}

//-----------------------------------------------------------------------------
//	说明：		获取警输入的状态
//	返回:		警输入的状态，用户需要位与的方式获取
//  注意:		该接口为频繁调用接口，简单保护
//-----------------------------------------------------------------------------
unsigned int    MP4Sys_AlarmIn(int *pAlarmIndata)
{
    /* 接口加锁 */
    if (HY_Res_SDK_Lock() != 0)
    {
        ZW_ERROR("SDK Not Init\n");
        return ERR_SDK_NOINIT;
    }

    //处理
    HY_Enc_GetAlarmIn(pAlarmIndata);
    HY_Res_SDK_UnLock();
    //SDKPRINT("--------into MP4Sys_AlarmIn----status %d----\n", pAlarmIndata);

    return 0;
}

//-----------------------------------------------------------------------------
//	说明：		设置语音对讲模式(支持互信互通所添加的接口)
//	输入参数：	Mode		见AudioEncodeMode_t
//	返回： 		成功为 0， 其它为定义的错误号；
//-----------------------------------------------------------------------------
int     MP4Sys_SetTalkingMode(int Mode)
{
    SystemInfo_t		*pSysInfo;
    CTalkingChannel_t	*pTalkingCh;

    SDKPRINT("--------into MP4Sys_SetTalkingMode--------\n");
#ifdef NO_SET_AUDIO
        return 0;
#endif

    /* 接口加锁 */
    if (HY_Res_SDK_Lock() != 0)
    {
        ZW_ERROR("SDK Not Init\n");
        return ERR_SDK_NOINIT;
    }

    if(eCodecG711U != Mode && eCodecG711A != Mode)
    {
        ZW_ERROR("Param err: Mode = %d", Mode);
        HY_Res_SDK_UnLock();
        return ERR_INVALID_ARGUMENT;
    }

    HY_Res_GetSystemInfo(&pSysInfo);
    if(pSysInfo->m_TotalTalkingChannel <= 0)
    {
        ZW_ERROR("m_TotalTalkingChannel = %d\n", pSysInfo->m_TotalTalkingChannel);
        HY_Res_SDK_UnLock();
        return ERR_NOT_SUPPORT;
    }

    if(pSysInfo->m_TalkingMode == Mode)
    {
        HY_Res_SDK_UnLock();
        return 0;
    }

    HY_Enc_GetTalkingChInfo(&pTalkingCh);
    if(pTalkingCh->m_bStart)
    {
        HY_Res_SDK_UnLock();
        ZW_DEBUG("bTalking\n");
        return ERR_SYS_BUSY;
    }

    XTRACE_MUTEX_LOCK(&pTalkingCh->m_Lock);
    pSysInfo->m_TalkingMode = Mode;
    //  pTalkingCh->m_TalkingOut.BufSize = 0;
    if(eCodecG711A == Mode)
    {
        pSysInfo->m_AudioFrameSize = 320;
    }
    else if(eCodecG711U == Mode)
    {
        pSysInfo->m_AudioFrameSize = 320;
    }

    XTRACE_MUTEX_UNLOCK(&pTalkingCh->m_Lock);

    HY_Res_SDK_UnLock();
    return HI_SDK_SUCESS;
}

int     MP4Sys_SetAudioInMode(int mode)
{
    SystemInfo_t	  *pSysInfo = NULL;

    ZW_DEBUG("=============into MP4Sys_SetAudioInMode=====%s======\n", mode==MIC_IN ? "MIC_IN" : "LINE_IN");
#ifdef NO_SET_AUDIO
        return 0;
#endif

    if((mode != MIC_IN) && (mode != LINE_IN))
    {
        ZW_ERROR("%s Param err: Mode = %d", __FUNCTION__, mode);
        return ERR_INVALID_ARGUMENT;
    }

    // 接口加锁
    if (HY_Res_SDK_Lock() != 0)
    {
        ZW_ERROR("%s : SDK Not Init\n", __FUNCTION__);
        return ERR_SDK_NOINIT;
    }

    HY_Res_GetSystemInfo(&pSysInfo);

    if(pSysInfo->m_TotalAudioEncChannel <= 0)
    {
        ZW_ERROR("%s : m_TotalAudioEncChannel = %d\n", __FUNCTION__, pSysInfo->m_TotalAudioEncChannel);
        HY_Res_SDK_UnLock();
        return ERR_NOT_SUPPORT;
    }

    HY_Res_RD_Lock(&pSysInfo->m_SysTemConfigLock);

    pSysInfo->m_AudioInMode = mode;

    HY_Res_RD_UnLock(&pSysInfo->m_SysTemConfigLock);

    HY_Res_SDK_UnLock();

    return HI_SDK_SUCESS;
}

//-----------------------------------------------------------------------------
//    说明：      设置语音播报的数据
//    输入参数：    bEnable
//    输入参数： dataFormat 0：pcm格式，1：G711u格式
//    返回	：  成功为 0 , 0x102：当前为对讲模式
//-----------------------------------------------------------------------------
int MP4Sys_SetSpeechingEnable(int bEnable, int dataFormat)
{
    BOOL				 bEn;
    SystemInfo_t		*pSysInfo;
    CTalkingChannel_t	*pTalkingCh;
    int nret;
    int nNextMode = 0;

    //ZW_DEBUG("=============into MP4Sys_SetSpeechingEnable====bEnable %d=======\n",bEnable);
#ifdef NO_USE_TALKING
    return 0;
#endif

    // 接口加锁
    if (HY_Res_SDK_Lock() != 0)
    {
        ZW_ERROR("%s : SDK Not Init\n", __FUNCTION__);
        return ERR_SDK_NOINIT;
    }
    HY_Res_GetSystemInfo(&pSysInfo);

    bEn = (bEnable != 0) ? 1 : 0;
    HY_Enc_GetTalkingChInfo(&pTalkingCh);

    XTRACE_MUTEX_LOCK(&pTalkingCh->m_Lock);
    nNextMode = HY_Speaker_CalcNextMode(pSysInfo, SPEECH_ENABLE, bEnable);

    if ((nret = HY_Speaker_CalcCurrMode(pSysInfo)) == nNextMode)
    {
        HY_Speaker_ChangeMode(pSysInfo, SPEECH_ENABLE, bEn);
        XTRACE_MUTEX_UNLOCK(&pTalkingCh->m_Lock);
        HY_Res_SDK_UnLock();
        return nret == TALK_ENABLE ? 0x102 : 0;
    }

    pSysInfo->m_SpeechMode = dataFormat;

    HY_Speaker_ChangeMode(pSysInfo, SPEECH_ENABLE, bEn);

    nret = HY_Speaker_CalcCurrMode(pSysInfo);
    // 当前已在对讲，m_bStart置1
    if (TALK_ENABLE == nret)
    {
        pTalkingCh->m_bStart  = 1;
    }
    else
    {
        pTalkingCh->m_bStart  = 0;
    }

    pTalkingCh->m_TalkingOut.WritePos = 0;
    pTalkingCh->m_TalkingOut.ReadPos = 0;

    XTRACE_MUTEX_UNLOCK(&pTalkingCh->m_Lock);

    HY_Res_SDK_UnLock();
    ZW_DEBUG(" [DONE] %s speech success !\n", bEn ? "enable":"disable");

    return nret == TALK_ENABLE ? 0x102 : HI_SDK_SUCESS;
}

int     MP4Sys_SetAudioCodec(int ACodec)
{
    int ret = 0;
    SystemInfo_t    *pSysInfo = NULL;
    CChannelContext *pChCtx = NULL;

    ZW_DEBUG("=============into MP4Sys_SetAudioCodec=====%s======\n", ACodec==eCodecG711A ? "eCodecG711A" : "eCodecG711U");

#ifdef NO_SET_AUDIO
        return 0;
#endif
#if 1
    if((ACodec != (int)eCodecG711A) && (ACodec != (int)eCodecG711U))
    {
        ZW_ERROR("Param err: ACodec = %d",ACodec);
        return ERR_INVALID_ARGUMENT;
    }

    // 接口加锁
    if (HY_Res_SDK_Lock() != 0)
    {
        ZW_ERROR("SDK Not Init\n");
        return ERR_SDK_NOINIT;
    }

    HY_Res_GetSystemInfo(&pSysInfo);
    HY_Enc_GetEncChInfo(&pChCtx);

    if(pSysInfo->m_TotalAudioEncChannel <= 0)
    {
        ZW_ERROR("m_TotalAudioEncChannel = %d\n", pSysInfo->m_TotalAudioEncChannel);
        HY_Res_SDK_UnLock();
        return ERR_NOT_SUPPORT;
    }

    if (ACodec != pChCtx->m_AudioCodec)
    {
        pChCtx->m_AudioCodec = ACodec;
        ret = HY_Enc_SetAudioCodec(pChCtx);
    }

    HY_Res_SDK_UnLock();
#endif
    return ret;
}

//-----------------------------------------------------------------------------
//	说明：		启动或关闭语音对讲
//	输入参数：	bEnable	0表示停止, 1表示启动
//	返回： 		成功为 0， 其它为定义的错误号；
//-----------------------------------------------------------------------------
int     MP4Sys_SetTalkingEnable(int bEnable)
{
    BOOL                 bEn;
    SystemInfo_t		*pSysInfo;
    CTalkingChannel_t	*pTalkingCh;
    int nNextMode = 0;

    ZW_DEBUG("--------into MP4Sys_SetTalkingEnable--------\n");
#ifdef NO_SET_AUDIO
        return 0;
#endif

    /* 接口加锁 */
    if (HY_Res_SDK_Lock() != 0)
    {
        ZW_ERROR("SDK Not Init\n");
        return ERR_SDK_NOINIT;
    }

    HY_Res_GetSystemInfo(&pSysInfo);
    if(pSysInfo->m_TotalTalkingChannel <= 0)
    {
        ZW_ERROR("m_TotalTalkingChannel = %d\n", pSysInfo->m_TotalTalkingChannel);
        HY_Res_SDK_UnLock();
        return ERR_NOT_SUPPORT;
    }
    HY_Enc_GetTalkingChInfo(&pTalkingCh);
    bEn = (bEnable != 0) ? TRUE : FALSE;

    ZW_INFO("===pTalkingCh->m_bStart:%d===\n", pTalkingCh->m_bStart);

    XTRACE_MUTEX_LOCK(&pTalkingCh->m_Lock);
    nNextMode = HY_Speaker_CalcNextMode(pSysInfo, TALK_ENABLE, bEnable);

    if (HY_Speaker_CalcCurrMode(pSysInfo) == nNextMode)
    {
        HY_Speaker_ChangeMode(pSysInfo, TALK_ENABLE, bEn);
        XTRACE_MUTEX_UNLOCK(&pTalkingCh->m_Lock);
        HY_Res_SDK_UnLock();
        return 0;
    }

    HY_Speaker_ChangeMode(pSysInfo, TALK_ENABLE, bEn);

    // 当前已在对讲，m_bStart置1
    if (TALK_ENABLE == HY_Speaker_CalcCurrMode(pSysInfo))
    {
        pTalkingCh->m_bStart  = 1;
    }
    else
    {
        pTalkingCh->m_bStart  = 0;
    }

    pTalkingCh->m_TalkingOut.WritePos = 0;
    pTalkingCh->m_TalkingOut.ReadPos = 0;

    XTRACE_MUTEX_UNLOCK(&pTalkingCh->m_Lock);

    HY_Res_SDK_UnLock();

    return HI_SDK_SUCESS;
}

//-----------------------------------------------------------------------------
//	说明：		获取语音对讲数据(从对讲缓冲区中获取数据抛给上层)
//	输入参数：	pBuf	对讲数据存放地址
//	输入参数：	pSize	对讲数据大小存放地址
//	返回： 		成功为 0， 其它为定义的错误号；
//  注意:		该接口为频繁调用型接口,简单保护
//-----------------------------------------------------------------------------
int     MP4Sys_ReadTalkingData(char *DataBuf, unsigned int* BufSize)
{
    SystemInfo_t		*pSysInfo;
    CTalkingChannel_t	*pTalkingCh;

//	SDKPRINT("--------into MP4Sys_ReadTalkingData--------\n");
#ifdef NO_SET_AUDIO
        return 0;
#endif

    /* 接口加锁 */
    if (HY_Res_SDK_Lock() != 0)
    {
        ZW_ERROR("SDK Not Init\n");
        return ERR_SDK_NOINIT;
    }

    HY_Res_GetSystemInfo(&pSysInfo);
    if(pSysInfo->m_TotalTalkingChannel <= 0)
    {
        ZW_ERROR("m_TotalTalkingChannel = %d\n", pSysInfo->m_TotalTalkingChannel);
        HY_Res_SDK_UnLock();
        return ERR_NOT_SUPPORT;
    }

    HY_Enc_GetTalkingChInfo(&pTalkingCh);
    if(!pTalkingCh->m_bStart)
    {
        ZW_ERROR("Talking Disable\n");
        HY_Res_SDK_UnLock();
        return ERR_SDK_NOINIT;
    }

    if (*BufSize >= pTalkingCh->m_TalkingIn.BufSize)
    {
        memcpy(DataBuf, pTalkingCh->m_TalkingIn.pBuf, pTalkingCh->m_TalkingIn.BufSize);
        *BufSize = pTalkingCh->m_TalkingIn.WritePos;
    }
    else
    {
        memcpy(DataBuf, pTalkingCh->m_TalkingIn.pBuf, *BufSize);
    }

    //pTalkingCh->m_TalkingIn.BufSize -= *BufSize;
    pTalkingCh->m_TalkingIn.WritePos = 0;
    HY_Res_SDK_UnLock();
    return HI_SDK_SUCESS;
}

//-----------------------------------------------------------------------------
//	说明：		发送语音对讲数据(将上层的数据发送到缓冲区)
//	输入参数：	DataBuf	对讲数据存放地址
//	输入参数：	DataSize	对讲数据大小
//	返回： 		成功为 0， 其它为定义的错误号；
//  注意:		该接口为频繁调用型接口,简单保护
//-----------------------------------------------------------------------------
int     MP4Sys_SetTalkingData(char *DataBuf, unsigned int DataSize)
{
    SystemInfo_t		*pSysInfo = NULL;
    CTalkingChannel_t	*pTalkingCh = NULL;

//	SDKPRINT("--------into MP4Sys_SetTalkingData--------\n");
#ifdef NO_SET_AUDIO
        return 0;
#endif

    /* 接口加锁 */
    if (HY_Res_SDK_Lock() != 0)
    {
        ZW_ERROR("SDK Not Init\n");
        return ERR_SDK_NOINIT;
    }

    HY_Res_GetSystemInfo(&pSysInfo);
    if((NULL == DataBuf) || (DataSize > TALKINGSIZE))//320*4
    {
        ZW_ERROR("Data Buf Error : bufaddr = 0x%p, size = %u\n", DataBuf, DataSize);
        HY_Res_SDK_UnLock();
        return ERR_INVALID_ARGUMENT;
    }

    if(pSysInfo->m_TotalTalkingChannel <= 0)
    {
        ZW_ERROR("m_TotalTalkingChannel = %d\n", pSysInfo->m_TotalTalkingChannel);
        HY_Res_SDK_UnLock();
        return ERR_NOT_SUPPORT;
    }
    HY_Enc_GetTalkingChInfo(&pTalkingCh);
    if(!pTalkingCh->m_bStart)
    {
        ZW_DEBUG("Talking Disable\n");
        HY_Res_SDK_UnLock();
        return ERR_SDK_NOINIT;
    }

#if 0
    if (1 != pSysInfo->m_SpeechTalkingMode)
    {
        ZW_DEBUG("Talking Disable\n");
        HY_Res_SDK_UnLock();
        return ERR_SDK_NOINIT;
    }
#endif

    if (HY_Speaker_CalcCurrMode(pSysInfo) != TALK_ENABLE)
    {
        ZW_DEBUG("Talking Disable\n");
        HY_Res_SDK_UnLock();
        return ERR_INVALID_ARGUMENT;
    }

    XTRACE_MUTEX_LOCK(&pTalkingCh->m_Lock);

    // ZW_INFO("sjz-getdata--pTalkingCh->m_TalkingOut.WritePos:%d\n", pTalkingCh->m_TalkingOut.WritePos);
    //m_TalkingOut.pBuf开辟的是4096
    if((pTalkingCh->m_TalkingOut.WritePos + DataSize) > pTalkingCh->m_TalkingOut.BufSize)
    {
        ZW_ERROR("Data Buf full : bufaddr = 0x%p, size = %u\n", DataBuf, DataSize);
        XTRACE_MUTEX_UNLOCK(&pTalkingCh->m_Lock);
        HY_Res_SDK_UnLock();
        return ERR_INVALID_ARGUMENT;
    }

    memcpy(pTalkingCh->m_TalkingOut.pBuf + pTalkingCh->m_TalkingOut.WritePos, DataBuf, DataSize);
//  pTalkingCh->m_TalkingOut.BufSize  += DataSize;
    pTalkingCh->m_TalkingOut.WritePos += DataSize;

//	ZW_INFO("===[%s]pTalkingCh->m_TalkingOut.BufSize:%d===\n", __FUNCTION__, pTalkingCh->m_TalkingOut.BufSize);

    XTRACE_MUTEX_UNLOCK(&pTalkingCh->m_Lock);

    HY_Res_SDK_UnLock();
    return HI_SDK_SUCESS;
}

//-----------------------------------------------------------------------------
//    说明：      设置语音播报的数据
//    输入参数：    DataBuf
//    输入参数：    DataSize
//    返回	：  成功为 0 , 0x101：已恢复为了空闲，0x102：当前为对讲模式
//-----------------------------------------------------------------------------
int     MP4Sys_SetSpeechData(char *DataBuf, unsigned long DataSize)
{
//	return 0;

    SystemInfo_t		*pSysInfo;
    CTalkingChannel_t	*pTalkingCh;

    //ZW_DEBUG("--------into MP4Sys_SetSpeechData--------\n");

    /* 接口加锁 */
    if (HY_Res_SDK_Lock() != 0)
    {
        ZW_ERROR("SDK Not Init\n");
        return ERR_SDK_NOINIT;
    }

    HY_Res_GetSystemInfo(&pSysInfo);
    if((NULL == DataBuf) || (DataSize > TALKINGSIZE))//320*4
    {
        ZW_ERROR("Data Buf Error : bufaddr = 0x%p, size = %ld\n", DataBuf, DataSize);
        HY_Res_SDK_UnLock();
        return ERR_INVALID_ARGUMENT;
    }

    if(pSysInfo->m_TotalTalkingChannel <= 0)
    {
        ZW_ERROR("m_TotalTalkingChannel = %d\n", pSysInfo->m_TotalTalkingChannel);
        HY_Res_SDK_UnLock();
        return ERR_NOT_SUPPORT;
    }
    HY_Enc_GetTalkingChInfo(&pTalkingCh);

    XTRACE_MUTEX_LOCK(&pTalkingCh->m_Lock);
    /*
    if (1 != pSysInfo->m_SpeechTalkingMode)
    {
        ZW_DEBUG("Talking Disable, or m_SpeechTalkingMode = %d\n", pSysInfo->m_SpeechTalkingMode);
        HY_Res_SDK_UnLock();
        XTRACE_MUTEX_UNLOCK(&pTalkingCh->m_Lock);
        if (0 == pSysInfo->m_SpeechTalkingMode)
        {
            return 0x101;
        }
        else
        {
            return 0x102;
        }
    }*/
    if (HY_Speaker_CalcCurrMode(pSysInfo) == 0)
    {
        ZW_DEBUG("Talking Disable, or m_SpeechTalkingMode = %d\n", pSysInfo->m_SpeechTalkingMode);
        XTRACE_MUTEX_UNLOCK(&pTalkingCh->m_Lock);
        HY_Res_SDK_UnLock();
        return 0x101;
    }
    else if (HY_Speaker_CalcCurrMode(pSysInfo) != SPEECH_ENABLE)
    {
        ZW_DEBUG("Talking Disable, or m_SpeechTalkingMode = %d\n", pSysInfo->m_SpeechTalkingMode);
        XTRACE_MUTEX_UNLOCK(&pTalkingCh->m_Lock);
        HY_Res_SDK_UnLock();
        return 0x102;
    }
    //m_TalkingOut.pBuf开辟的是4096
    if((pTalkingCh->m_TalkingOut.WritePos + DataSize) > pTalkingCh->m_TalkingOut.BufSize)
    {
        ZW_INFO("TalkingOut buffer full\n");
        XTRACE_MUTEX_UNLOCK(&pTalkingCh->m_Lock);
        HY_Res_SDK_UnLock();
        return ERR_BUFFER_LEN_SHORT;
    }

    memcpy(pTalkingCh->m_TalkingOut.pBuf + pTalkingCh->m_TalkingOut.WritePos, DataBuf, DataSize);
    pTalkingCh->m_TalkingOut.WritePos += DataSize;
    XTRACE_MUTEX_UNLOCK(&pTalkingCh->m_Lock);

    HY_Res_SDK_UnLock();
    return HI_SDK_SUCESS;
}

/* 获取当前播放器的播放状态 */
int    MP4Sys_GetSpeechPlayingStatus(void)
{
    CChannelContext *pChContext = NULL;
    HY_Enc_GetEncChInfo(&pChContext);

    return pChContext->m_nSpeechPlayStatus;
}

//-----------------------------------------------------------------------------
//	说明：		设置语音对讲数据获取回调函数
//	输入参数：	TalkingReadCallback 回调函数
//	返回： 		成功为 0， 其它为定义的错误号；
//-----------------------------------------------------------------------------
int     MP4Sys_RegisterTalkingReadCallback(TALKING_READ_CALLBACK TalkingReadCallback)
{
    SystemInfo_t		*pSysInfo = NULL;

//	SDKPRINT("--------into MP4Sys_RegisterTalkingReadCallback--------\n");

    /* 接口加锁 */
    if (HY_Res_SDK_Lock() != 0)
    {
        ZW_ERROR("SDK Not Init\n");
        return ERR_SDK_NOINIT;
    }

    HY_Res_GetSystemInfo(&pSysInfo);
    pSysInfo->m_pTalkingCallBack = TalkingReadCallback;

    HY_Res_SDK_UnLock();
    return HI_SDK_SUCESS;
}

int		MP4Sys_EncSetBacklightMode(void *hChannelHandle,  int enable)
{
    CChannelContext *pChannelContext = (CChannelContext*)hChannelHandle;
    SystemInfo_t	*pSysInfo = NULL;

    ZW_DEBUG("=============into MP4Sys_EncSetBacklightMode===========\n");
#ifdef NO_CHANGE_IMAGE_SETING
    return 0;
#endif

    /* 接口加锁 */
    if (HY_Res_SDK_Lock() != 0)
    {
        ZW_DEBUG("%s : SDK Not Init\n", __FUNCTION__);
        return ERR_SDK_NOINIT;
    }
    //参数范围检查
    if( !HY_Enc_IsEncHandleOk( hChannelHandle ) )
    {
        ZW_DEBUG("%s INVALID_HANDLE\n", __FUNCTION__);
        HY_Res_SDK_UnLock();
        return ERR_INVALID_HANDLE ;
    }

    if(pChannelContext->m_bRecord != TRUE)
    {
        SDKPRINT("please start encode first !\n");
        HY_Res_SDK_UnLock();
        return -1 ;
    }
    if((enable != 0) && (enable != 1))
    {
        HY_Res_SDK_UnLock();
        ZW_DEBUG("Backlight Mode limited at 0~1 !\n");
        return ERR_INVALID_ARGUMENT;
    }

    HY_Res_GetSystemInfo(&pSysInfo);
    HY_Res_RD_Lock(&pSysInfo->m_SysTemConfigLock);
    XTRACE_MUTEX_LOCK(&pChannelContext->m_SensorLock);

    if(HY_Enc_SetBacklightMode(pChannelContext, enable) < 0)
    {
        XTRACE_MUTEX_UNLOCK(&pChannelContext->m_SensorLock);
        HY_Res_RD_UnLock(&pSysInfo->m_SysTemConfigLock);
        HY_Res_SDK_UnLock();
        ZW_DEBUG("set backlight mode fail !\n");
        return -1 ;
    }
    XTRACE_MUTEX_UNLOCK(&pChannelContext->m_SensorLock);
    HY_Res_RD_UnLock(&pSysInfo->m_SysTemConfigLock);
    HY_Res_SDK_UnLock();
    ZW_DEBUG("set backlight mode success !\n");

    return HI_SDK_SUCESS;
}

int 	MP4Sys_SetWhiteBalance(void *hChannelHandle, WHITEBALANCE_T *pWhiteBalance)
{
    SystemInfo_t	*pSysInfo;
    CChannelContext *pChannelContext = (CChannelContext *) hChannelHandle;
    WHITEBALANCE_T WhiteBalance;

    ZW_DEBUG("=============into ZWSys_SetWhiteBalance===========\n");
#ifdef NO_CHANGE_IMAGE_SETING
    return 0;
#endif

    /* 接口加锁 */
    if (HY_Res_SDK_Lock() != 0)
    {
        ZW_DEBUG("%s : SDK Not Init\n", __FUNCTION__);
        return ERR_SDK_NOINIT;
    }
    ZW_DEBUG("enable=%d, mode=%d, RedGain=%d, BlueGain=%d!\n",
        pWhiteBalance->WBEnable, pWhiteBalance->WBWorkMode, pWhiteBalance->RedGain, pWhiteBalance->BlueGain);

    //参数范围检查
    if( !HY_Enc_IsEncHandleOk( hChannelHandle ) )
    {
        ZW_DEBUG("%s INVALID_HANDLE\n", __FUNCTION__);
        HY_Res_SDK_UnLock();
        return ERR_INVALID_HANDLE ;
    }
    if(pChannelContext->m_bRecord != TRUE)
    {
        SDKPRINT("please start encode first !\n");
        HY_Res_SDK_UnLock();
        return -1 ;
    }
    if(pWhiteBalance == NULL)
    {
        HY_Res_SDK_UnLock();
        ZW_DEBUG("param is null !\n");
        return ERR_INVALID_ARGUMENT;
    }
    memcpy(&WhiteBalance, pWhiteBalance, sizeof(WHITEBALANCE_T));
    if((WhiteBalance.WBWorkMode < WB_MODE_INCANDS) || (WhiteBalance.WBWorkMode > WB_MODE_LOCK))
    {
        HY_Res_SDK_UnLock();
        ZW_DEBUG("White Balance Work Mode limited at 0~10!\n");
        return ERR_INVALID_ARGUMENT;
    }
    if(WB_MODE_MANUAL == WhiteBalance.WBWorkMode)
    {
        if((WhiteBalance.RedGain < 0) || (WhiteBalance.RedGain > 100) || (WhiteBalance.BlueGain < 0) || (WhiteBalance.BlueGain > 100))
        {
            HY_Res_SDK_UnLock();
            ZW_DEBUG("RedGain=%d BlueGain=%d limited at 0~100!\n", WhiteBalance.RedGain, WhiteBalance.BlueGain);
            return ERR_INVALID_ARGUMENT;
        }
    }

    if(WB_MODE_AUTO == WhiteBalance.WBWorkMode)
        WhiteBalance.WBEnable = 0;

    HY_Res_GetSystemInfo(&pSysInfo);
    HY_Res_RD_Lock(&pSysInfo->m_SysTemConfigLock);
    XTRACE_MUTEX_LOCK(&pChannelContext->m_SensorLock);
    if(HY_Enc_SetWhiteBalance(pChannelContext, &WhiteBalance) < 0)
    {
        XTRACE_MUTEX_UNLOCK(&pChannelContext->m_SensorLock);
        HY_Res_RD_UnLock(&pSysInfo->m_SysTemConfigLock);
        HY_Res_SDK_UnLock();
        ZW_DEBUG("set white balance fail !\n");
        return -1;//
    }

    XTRACE_MUTEX_UNLOCK(&pChannelContext->m_SensorLock);
    HY_Res_RD_UnLock(&pSysInfo->m_SysTemConfigLock);
    HY_Res_SDK_UnLock();
    ZW_DEBUG("set white balance ok !\n");

    return HI_SDK_SUCESS;
}

int 	MP4Sys_SetAEParam(void *hChannelHandle, AE_T *pAeParam)
{
    SystemInfo_t	*pSysInfo = NULL;
    CChannelContext *pChannelContext = (CChannelContext *) hChannelHandle;
    AE_T AeParam;
    static int startflag =0;
    int ae_mode = AE_MODE_MANUAL;

    ZW_INFO("=============into MP4Sys_SetAEParam===========\n");
#ifdef NO_CHANGE_IMAGE_SETING
    return 0;
#endif

#if 0
    ZW_INFO("[syh-20190520]UNSupport AntiFlicker for now.\n");
    pAeParam->antiFlicker = CMOS_FREQUENCY_50HZ;
#endif

    /* 接口加锁 */
    if (HY_Res_SDK_Lock() != 0)
    {
        ZW_ERROR("SDK Not Init\n");
        return ERR_SDK_NOINIT;
    }

    if(pAeParam == NULL)
    {
        HY_Res_SDK_UnLock();
        ZW_ERROR("param is null !\n");
        return ERR_INVALID_ARGUMENT;
    }
    memcpy(&AeParam,pAeParam,sizeof(AE_T));

    ZW_DEBUG("antiFlicker = %d, MinShutterTime = %d, MaxShutterTime = %d, sensorGain = %d\n",
        AeParam.antiFlicker, AeParam.MinShutterTime, AeParam.MaxShutterTime, AeParam.sensorGain);
    //参数范围检查
    if(!HY_Enc_IsEncHandleOk( hChannelHandle ) )
    {
        ZW_ERROR("INVALID_HANDLE\n");
        HY_Res_SDK_UnLock();
        return ERR_INVALID_HANDLE ;
    }
    if(pChannelContext->m_bRecord != TRUE)
    {
        SDKPRINT("please start encode first !\n");
        HY_Res_SDK_UnLock();
        return -1 ;
    }

    if((AeParam.antiFlicker != CMOS_FREQUENCY_50HZ) && (AeParam.antiFlicker != CMOS_FREQUENCY_60HZ) && (AeParam.antiFlicker != CMOS_FREQUENCY_CLOSE))
    {
        ZW_ERROR("antiFlicker limited at 0~2 \n");
        HY_Res_SDK_UnLock();
        return ERR_INVALID_ARGUMENT ;
    }
    if((AeParam.MinShutterTime < 0) || (AeParam.MinShutterTime > 10000))
    {
        ZW_ERROR("MinShutterTime limited at 0~10000 \n");
        HY_Res_SDK_UnLock();
        return ERR_INVALID_ARGUMENT ;
    }
    if((AeParam.MaxShutterTime < 0) || (AeParam.MaxShutterTime > 10000))
    {
        ZW_ERROR("MaxShutterTime limited at 0~10000 \n");
        HY_Res_SDK_UnLock();
        return ERR_INVALID_ARGUMENT ;
    }

    //转换成底层接口
    if(AeParam.MaxShutterTime > AeParam.MinShutterTime)
    {
        ZW_ERROR("MinShutterTime %d is less than MaxShutterTime %d!\n", AeParam.MinShutterTime, AeParam.MaxShutterTime);
        HY_Res_SDK_UnLock();
        return ERR_INVALID_ARGUMENT ;
    }

    if(0 == AeParam.MinShutterTime)//自动
        AeParam.MinShutterTime = 10000;
    else
        AeParam.MinShutterTime = (AeParam.MinShutterTime > 10000) ? 10000 : AeParam.MinShutterTime;

    if(0 == AeParam.MaxShutterTime)
    {
        ae_mode = AE_MODE_AUTO;
        AeParam.MaxShutterTime = 10;
    }
    else
    {
        ae_mode = AE_MODE_MANUAL;
        AeParam.MaxShutterTime = (AeParam.MaxShutterTime > 10000) ? 10000 : AeParam.MaxShutterTime;
    }

    //快门速度要大于vin捕获的帧率,vin默认为25fps
    if((AeParam.MinShutterTime < 25) && (AeParam.MinShutterTime != 0))
        AeParam.MinShutterTime = 25;
    if((AeParam.MaxShutterTime < 10) && (AeParam.MaxShutterTime != 0))
        AeParam.MaxShutterTime = 10;

    HY_Res_GetSystemInfo(&pSysInfo);
    HY_Res_RD_Lock(&pSysInfo->m_SysTemConfigLock);

    if ((pSysInfo->m_Frequ_HZ != AeParam.antiFlicker) || (startflag == 0))
    {
        pSysInfo->m_Frequ_HZ = AeParam.antiFlicker;
#if defined(__IS_SUPPORT_12FPS__)
        HY_ChangeSensorFrameRate(pChannelContext, AeParam.antiFlicker);
#endif
    }
    startflag = 1;
    XTRACE_MUTEX_LOCK(&pChannelContext->m_SensorLock);
    if(HY_Enc_SetAEParam(pChannelContext, &AeParam, ae_mode) < 0)
    {
        XTRACE_MUTEX_UNLOCK(&pChannelContext->m_SensorLock);
        HY_Res_RD_UnLock(&pSysInfo->m_SysTemConfigLock);
        HY_Res_SDK_UnLock();
        ZW_ERROR("################### set ae fail ###################\n");
        return -1;
    }

    //需要重新设置一下镜像参数
    //HY_Enc_SetFlipRotateMode(pChannelContext, pChannelContext->flipRotate);

    XTRACE_MUTEX_UNLOCK(&pChannelContext->m_SensorLock);
    HY_Res_RD_UnLock(&pSysInfo->m_SysTemConfigLock);
    HY_Res_SDK_UnLock();
    ZW_INFO("set ae success!\n");

    return HI_SDK_SUCESS;
}

//设置ircut被动模式的灵敏度
int	MP4Sys_EncSetIrCut(void *hChannelHandle,  int nDayNightSensity)
{
    return	HI_SDK_SUCESS;
}

/*设置日夜自动(主动模式)的灵敏度*/
int	MP4Sys_EncSetIrcutInitiativeModeSensity(void *hChannelHandle,  int Sensity)
{
    CChannelContext *pChannelContext = (CChannelContext*)hChannelHandle;

    ZW_INFO("=============into MP4Sys_EncSetIrcutInitiativeModeSensity===Sensity:%d===========\n", Sensity);
#ifdef NO_CHANGE_IMAGE_SETING
    return 0;
#endif

    // 接口加锁
    if (HY_Res_SDK_Lock() != 0)
    {
        ZW_INFO("%s : SDK Not Init\n", __FUNCTION__);
        return ERR_SDK_NOINIT;
    }
    // 参数检查
    if( !HY_Enc_IsEncHandleOk(hChannelHandle))
    {
        ZW_INFO("%s INVALID_HANDLE\n", __FUNCTION__);
        HY_Res_SDK_UnLock();
        return ERR_INVALID_ARGUMENT ;
    }

    //参数范围
    if((Sensity < 0) || (Sensity > 100))
    {
        ZW_INFO("ircut thres limited at 0~100\n");
        HY_Res_SDK_UnLock();
        return ERR_INVALID_ARGUMENT;
    }

    pChannelContext->m_InitiativeModeSensity = Sensity;

    HY_Res_SDK_UnLock();

    return	HI_SDK_SUCESS;
}


/* 设置IRCUT切换方向 */
int MP4Sys_EncSetIrcutDirection(void *hChannelHandle,  int mode)
{
    CChannelContext *pChannelContext = (CChannelContext*)hChannelHandle;

    ZW_DEBUG("=============into MP4Sys_EncSetIrcutDirection===========\n");
//#ifdef NO_CHANGE_IMAGE_SETING
//	return 0;
//#endif

    // 接口加锁
    if (HY_Res_SDK_Lock() != 0)
    {
        ZW_DEBUG("%s : SDK Not Init\n", __FUNCTION__);
        return ERR_SDK_NOINIT;
    }
    // 参数检查
    if( !HY_Enc_IsEncHandleOk(hChannelHandle))
    {
        ZW_DEBUG("%s INVALID_HANDLE\n", __FUNCTION__);
        HY_Res_SDK_UnLock();
        return ERR_INVALID_HANDLE ;
    }

    if (pChannelContext->m_IrcutDeriction == mode)
    {
        HY_Res_SDK_UnLock();
        return 0;
    }

    if((mode !=0) && (mode != 1))
    {
        ZW_DEBUG("fail!, mode = %d\n", mode);
        HY_Res_SDK_UnLock();
        return -1 ;
    }
    pChannelContext->m_IrcutDeriction = mode;
    XTRACE_MUTEX_LOCK(&pChannelContext->m_IrcutLock);
    pChannelContext->m_IrcutState = pChannelContext->m_IrcutStateHis;
    XTRACE_MUTEX_UNLOCK(&pChannelContext->m_IrcutLock);
    HY_Res_SDK_UnLock();

    return	HI_SDK_SUCESS;
}

/* 设置被动模式IRcut切换时间*/
int	MP4Sys_EncSetDayNightSwitchTime(void *hChannelHandle,  int time)
{
    CChannelContext *pChannelContext = (CChannelContext*)hChannelHandle;

    ZW_DEBUG("=============into MP4Sys_EncSetDayNightSwitchTime=====time %d======\n", time);
#ifdef NO_CHANGE_IMAGE_SETING
    return 0;
#endif

    // 接口加锁
    if (HY_Res_SDK_Lock() != 0)
    {
        ZW_DEBUG("%s : SDK Not Init\n", __FUNCTION__);
        return ERR_SDK_NOINIT;
    }
    // 参数检查
    if( !HY_Enc_IsEncHandleOk(hChannelHandle))
    {
        ZW_DEBUG("%s INVALID_HANDLE\n", __FUNCTION__);
        HY_Res_SDK_UnLock();
        return ERR_INVALID_HANDLE ;
    }
    // 设置日夜模式转换时间范围为[3, 15]
    // time = ((time < 3) ? 3 : ((time > 15) ? 15 : time));
    if((time < 3) || (time > 15))
    {
        ZW_DEBUG("fail!, time = %d\n", time);
        HY_Res_SDK_UnLock();
        return -1 ;
    }

    pChannelContext->m_DayNightSwitchTime = time;

    HY_Res_SDK_UnLock();

    return	HI_SDK_SUCESS;
}

/*设置无极白光生效阀值*/
int MP4Sys_SetWhiteLightThreshold(void *hChannelHandle,  int time)
{
    CChannelContext *pChannelContext = (CChannelContext*)hChannelHandle;

    ZW_INFO("=============into MP4Sys_SetWhiteLightThreshold=====time %d======\n", time);
#ifdef NO_CHANGE_IMAGE_SETING
    return 0;
#endif

    // 接口加锁
    if (HY_Res_SDK_Lock() != 0)
    {
        ZW_INFO("%s : SDK Not Init\n", __FUNCTION__);
        return ERR_SDK_NOINIT;
    }
    // 参数检查
    if( !HY_Enc_IsEncHandleOk(hChannelHandle))
    {
        ZW_INFO("%s INVALID_HANDLE\n", __FUNCTION__);
        HY_Res_SDK_UnLock();
        return ERR_INVALID_HANDLE ;
    }
    if((time < 0) || (time > 100))
    {
        ZW_INFO("fail!, time = %d\n", time);
        HY_Res_SDK_UnLock();
        return -1 ;
    }

    pChannelContext->m_WhiteLightThreshold = time;
    ZW_INFO("============= m_WhiteLightThreshold=%d======\n", pChannelContext->m_WhiteLightThreshold);

    HY_Res_SDK_UnLock();

    return	HI_SDK_SUCESS;
}

/*设置普通白光生效阀值*/
int MP4Sys_SetNormalWhiteLightThreshold(void *hChannelHandle, int C2Blkthreshold, int Blk2Cthreshold)
{
    CChannelContext *pChannelContext = (CChannelContext*)hChannelHandle;

    ZW_INFO("=============into MP4Sys_SetWhiteLightThreshold=====threshold %d %d======\n", C2Blkthreshold, Blk2Cthreshold);
#ifdef NO_CHANGE_IMAGE_SETING
    return 0;
#endif

    // 接口加锁
    if (HY_Res_SDK_Lock() != 0)
    {
        ZW_INFO("%s : SDK Not Init\n", __FUNCTION__);
        return ERR_SDK_NOINIT;
    }
    // 参数检查
    if( !HY_Enc_IsEncHandleOk(hChannelHandle))
    {
        ZW_INFO("%s INVALID_HANDLE\n", __FUNCTION__);
        HY_Res_SDK_UnLock();
        return ERR_INVALID_HANDLE ;
    }
    if(((C2Blkthreshold < 0) || (C2Blkthreshold > 100)) && ((Blk2Cthreshold < 0) || (Blk2Cthreshold > 100)))
    {
        ZW_INFO("fail!, C2Blk Blk2C = %d %d\n", C2Blkthreshold, Blk2Cthreshold);
        HY_Res_SDK_UnLock();
        return -1 ;
    }

    pChannelContext->m_NormalWhiteC2BlkThreshold = C2Blkthreshold;
    pChannelContext->m_NormalWhiteBlk2CThreshold = Blk2Cthreshold;

    ZW_INFO("============= C2Blk=%d Blk2C=%d =====\n", pChannelContext->m_NormalWhiteC2BlkThreshold, pChannelContext->m_NormalWhiteBlk2CThreshold);

    HY_Res_SDK_UnLock();

    return	HI_SDK_SUCESS;
}

int	MP4Sys_SetInfraredThreshold(void *hChannelHandle,  int time)
{
    CChannelContext *pChannelContext = (CChannelContext*)hChannelHandle;

    ZW_INFO("=============into MP4Sys_SetInfraredThreshold=====time %d======\n", time);
#ifdef NO_CHANGE_IMAGE_SETING
    return 0;
#endif

    // 接口加锁
    if (HY_Res_SDK_Lock() != 0)
    {
        ZW_INFO("%s : SDK Not Init\n", __FUNCTION__);
        return ERR_SDK_NOINIT;
    }
    // 参数检查
    if( !HY_Enc_IsEncHandleOk(hChannelHandle))
    {
        ZW_INFO("%s INVALID_HANDLE\n", __FUNCTION__);
        HY_Res_SDK_UnLock();
        return ERR_INVALID_HANDLE ;
    }
    if((time < 0) || (time > 100))
    {
        ZW_INFO("fail!, time = %d\n", time);
        HY_Res_SDK_UnLock();
        return -1 ;
    }

    pChannelContext->m_InfraredThreshold = time;
    ZW_INFO("============= m_InfraredThreshold=%d======\n", pChannelContext->m_InfraredThreshold);

    HY_Res_SDK_UnLock();

    return	HI_SDK_SUCESS;
}

int	MP4Sys_SetColorNightThreshold(void *hChannelHandle,  int threshold)
{
    CChannelContext *pChannelContext = (CChannelContext*)hChannelHandle;

    ZW_INFO("=============into MP4Sys_SetColorNightThreshold=====threshold %d======\n", threshold);
#ifdef NO_CHANGE_IMAGE_SETING
    return 0;
#endif

    // 接口加锁
    if (HY_Res_SDK_Lock() != 0)
    {
        ZW_INFO("%s : SDK Not Init\n", __FUNCTION__);
        return ERR_SDK_NOINIT;
    }
    // 参数检查
    if( !HY_Enc_IsEncHandleOk(hChannelHandle))
    {
        ZW_INFO("%s INVALID_HANDLE\n", __FUNCTION__);
        HY_Res_SDK_UnLock();
        return ERR_INVALID_HANDLE ;
    }
    if((threshold < 0) || (threshold > 100))
    {
        ZW_INFO("fail!, threshold = %d\n", threshold);
        HY_Res_SDK_UnLock();
        return -1 ;
    }

    pChannelContext->m_ColorNightThreshold = threshold;
    ZW_INFO("============= m_ColorNightThreshold=%d======\n", pChannelContext->m_ColorNightThreshold);

    HY_Res_SDK_UnLock();

    return	HI_SDK_SUCESS;
}

int	MP4Sys_SetIntelligentThreshold(void *hChannelHandle,  int threshold)
{
    CChannelContext *pChannelContext = (CChannelContext*)hChannelHandle;

    ZW_INFO("=============into MP4Sys_SetIntelligentThreshold=====threshold %d======\n", threshold);
#ifdef NO_CHANGE_IMAGE_SETING
    return 0;
#endif

    // 接口加锁
    if (HY_Res_SDK_Lock() != 0)
    {
        ZW_INFO("%s : SDK Not Init\n", __FUNCTION__);
        return ERR_SDK_NOINIT;
    }
    // 参数检查
    if( !HY_Enc_IsEncHandleOk(hChannelHandle))
    {
        ZW_INFO("%s INVALID_HANDLE\n", __FUNCTION__);
        HY_Res_SDK_UnLock();
        return ERR_INVALID_HANDLE ;
    }
    if((threshold < 0) || (threshold > 100))
    {
        ZW_INFO("fail!, threshold = %d\n", threshold);
        HY_Res_SDK_UnLock();
        return -1 ;
    }

    pChannelContext->m_IntelligentThreshold = threshold;
    ZW_INFO("============= m_IntelligentThreshold=%d======\n", pChannelContext->m_IntelligentThreshold);

    HY_Res_SDK_UnLock();

    return	HI_SDK_SUCESS;
}

int	MP4Sys_SetInfraredWorkMode(void *hChannelHandle,  int mode)
{
    CChannelContext *pChannelContext = (CChannelContext*)hChannelHandle;

    ZW_INFO("=============into MP4Sys_SetInfraredThreshold=====time %d======\n", mode);
#ifdef NO_CHANGE_IMAGE_SETING
    return 0;
#endif

    // 接口加锁
    if (HY_Res_SDK_Lock() != 0)
    {
        ZW_INFO("%s : SDK Not Init\n", __FUNCTION__);
        return ERR_SDK_NOINIT;
    }
    // 参数检查
    if( !HY_Enc_IsEncHandleOk(hChannelHandle))
    {
        ZW_INFO("%s INVALID_HANDLE\n", __FUNCTION__);
        HY_Res_SDK_UnLock();
        return ERR_INVALID_HANDLE ;
    }
    if((mode < 0) || (mode > 1))
    {
        ZW_INFO("fail!, mode = %d\n", mode);
        HY_Res_SDK_UnLock();
        return -1 ;
    }

    pChannelContext->m_InfraredWorkMode = mode;
    ZW_INFO("============= m_InfraredWorkMode=%d======\n", pChannelContext->m_InfraredWorkMode);

    HY_Res_SDK_UnLock();

    return	HI_SDK_SUCESS;
}

int	MP4Sys_SetInfraredBrightness(void *hChannelHandle,  int brightness)
{
    CChannelContext *pChannelContext = (CChannelContext*)hChannelHandle;

    ZW_INFO("=============into MP4Sys_SetInfraredThreshold=====time %d======\n", brightness);
#ifdef NO_CHANGE_IMAGE_SETING
    return 0;
#endif

    // 接口加锁
    if (HY_Res_SDK_Lock() != 0)
    {
        ZW_INFO("%s : SDK Not Init\n", __FUNCTION__);
        return ERR_SDK_NOINIT;
    }
    // 参数检查
    if( !HY_Enc_IsEncHandleOk(hChannelHandle))
    {
        ZW_INFO("%s INVALID_HANDLE\n", __FUNCTION__);
        HY_Res_SDK_UnLock();
        return ERR_INVALID_HANDLE ;
    }
    if((brightness < 0) || (brightness > 100))
    {
        ZW_INFO("fail!, brightness = %d\n", brightness);
        HY_Res_SDK_UnLock();
        return -1 ;
    }

    pChannelContext->m_InfraredBrightness = brightness;
    ZW_INFO("============= m_InfraredBrightness=%d======\n", pChannelContext->m_InfraredBrightness);

    HY_Res_SDK_UnLock();

    return	HI_SDK_SUCESS;
}

int     MP4Sys_SetColorNightCfg(void *hChannelHandle, int lastTime)
{
    CChannelContext *pChannelContext = (CChannelContext*)hChannelHandle;

    ZW_INFO("=============into MP4Sys_SetColorNightCfg=====lastTime %d======\n", lastTime);
#ifdef NO_CHANGE_IMAGE_SETING
    return 0;
#endif

    // 接口加锁
    if (HY_Res_SDK_Lock() != 0)
    {
        ZW_DEBUG("%s : SDK Not Init\n", __FUNCTION__);
        return ERR_SDK_NOINIT;
    }
    // 参数检查
    if( !HY_Enc_IsEncHandleOk(hChannelHandle))
    {
        ZW_DEBUG("%s INVALID_HANDLE\n", __FUNCTION__);
        HY_Res_SDK_UnLock();
        return ERR_INVALID_HANDLE ;
    }
    // 设置时间范围为[3, 600]
    if((lastTime < 3) || (lastTime > 600))
    {
        ZW_DEBUG("fail!, lastTime = %d\n", lastTime);
        HY_Res_SDK_UnLock();
        return -1 ;
    }

    pChannelContext->m_colorNightLastTime = lastTime * 1000 * 1000;

    HY_Res_SDK_UnLock();

    return	HI_SDK_SUCESS;
}

/* 开启一键夜灯模式 */
int MP4Sys_EncSetNightLedMode( NightLedCommand_T NightLedFlag )
{
    CChannelContext	*pChannelContext = NULL;
    HY_Enc_GetEncChInfo(&pChannelContext);

    SystemInfo_t *pSysInfo = NULL;
    HY_Res_GetSystemInfo(&pSysInfo);

    // 接口加锁
    if (HY_Res_SDK_Lock() != 0)
    {
        ZW_DEBUG("%s : SDK Not Init\n", __FUNCTION__);
        return ERR_SDK_NOINIT;
    }

    if (NightLedFlag != pChannelContext->m_NightLedFlag)
    {
        pChannelContext->m_NightLedFlag = NightLedFlag;
    }

    HY_Res_SDK_UnLock();
    ZW_DEBUG("set DayNight mode success !\n");

    return  HI_SDK_SUCESS;
}

int MP4Sys_EncSetNightLedDuration( int LedDuration )
{
    CChannelContext	*pChannelContext = NULL;
    HY_Enc_GetEncChInfo(&pChannelContext);

    // 接口加锁
    if (HY_Res_SDK_Lock() != 0)
    {
        ZW_DEBUG("%s : SDK Not Init\n", __FUNCTION__);
        return ERR_SDK_NOINIT;
    }

    if (LedDuration != pChannelContext->m_NightLedTime)
    {
        pChannelContext->m_NightLedTime = LedDuration;
    }

    HY_Res_SDK_UnLock();
    ZW_DEBUG("set DayNight Duration success !\n");

    return  HI_SDK_SUCESS;
}


/* 设置主动模式IRCUT切换时间 */
int	MP4Sys_EncSetDayNightInitiativeSwitchTime(void *hChannelHandle,  int time)
{
    CChannelContext *pChannelContext = (CChannelContext*)hChannelHandle;

    ZW_DEBUG("=============into MP4Sys_EncSetDayNightInitiativeSwitchTime=====time %d======\n", time);
#ifdef NO_CHANGE_IMAGE_SETING
    return 0;
#endif

    // 接口加锁
    if (HY_Res_SDK_Lock() != 0)
    {
        ZW_DEBUG("%s : SDK Not Init\n", __FUNCTION__);
        return ERR_SDK_NOINIT;
    }
    // 参数检查
    if( !HY_Enc_IsEncHandleOk(hChannelHandle))
    {
        ZW_DEBUG("%s INVALID_HANDLE\n", __FUNCTION__);
        HY_Res_SDK_UnLock();
        return ERR_INVALID_HANDLE ;
    }
    // 设置日夜模式转换时间范围为[3, 15]
    //time = ((time < 3) ? 3 : ((time > 15) ? 15 : time));
    if((time < 3) || (time > 15))
    {
        ZW_DEBUG("fail!, time = %d\n", time);
        HY_Res_SDK_UnLock();
        return -1 ;
    }

    pChannelContext->m_DayNightInitiativeSwitchTime = time;

    HY_Res_SDK_UnLock();

    return	HI_SDK_SUCESS;
}

/* 设置IRCUT的日夜转换模式 */
int		MP4Sys_EncSetDayNightMode(void *hChannelHandle, DAYNIGHTMODE_T mode, LP_IrcutTimeModeInfo pIrcutTimeModeInfo)
{
    CChannelContext *pChannelContext = (CChannelContext*)hChannelHandle;
    SystemInfo_t	*pSysInfo = NULL;

    ZW_DEBUG("=============into MP4Sys_EncSetDayNightMode==mode:%d=========\n", mode);
#ifdef NO_CHANGE_IMAGE_SETING
    return 0;
#endif

    // 接口加锁
    if (HY_Res_SDK_Lock() != 0)
    {
        ZW_DEBUG("%s : SDK Not Init\n", __FUNCTION__);
        return ERR_SDK_NOINIT;
    }
    // 参数检查
    if( !HY_Enc_IsEncHandleOk(hChannelHandle))
    {
        ZW_DEBUG("%s INVALID_HANDLE\n", __FUNCTION__);
        HY_Res_SDK_UnLock();
        return ERR_INVALID_HANDLE ;
    }
    if(pChannelContext->m_bRecord != TRUE)
    {
        SDKPRINT("please start encode first !\n");
        HY_Res_SDK_UnLock();
        return -1 ;
    }

    if((mode < IRCUT_PASSIVITY) || (mode > IRCUT_NORMAL_WHITELIGHT))
    {
        HY_Res_SDK_UnLock();
        ZW_DEBUG("Day Night Mode limited at 0~9 !\n");
        return ERR_INVALID_ARGUMENT;
    }
    if((IRCUT_TIME == pChannelContext->DayNightMode) && (NULL == pIrcutTimeModeInfo))
    {
        HY_Res_SDK_UnLock();
        ZW_DEBUG("NULL == pIrcutTimeModeInfo !\n");
        return ERR_INVALID_ARGUMENT;
    }

    HY_Res_GetSystemInfo(&pSysInfo);
    HY_Res_RD_Lock(&pSysInfo->m_SysTemConfigLock);
    XTRACE_MUTEX_LOCK(&pChannelContext->m_SensorLock);
#if 0
    if(IRCUT_PASSIVITY == mode)
    {
        pChannelContext->m_PtzCtrlIrcut = 1;
    }
    else
    {
        pChannelContext->m_PtzCtrlIrcut = 0;
    }
#endif
    if((IRCUT_DAY == mode) || (IRCUT_NIGHT == mode))
    {
        if(HY_Enc_SetDayNightMode(pChannelContext, mode == IRCUT_DAY ? ISDAY:ISNIGHT) < 0)
        {
            XTRACE_MUTEX_UNLOCK(&pChannelContext->m_SensorLock);
            HY_Res_RD_UnLock(&pSysInfo->m_SysTemConfigLock);
            HY_Res_SDK_UnLock();
            ZW_DEBUG("set DayNight mode fail !\n");
            return -1 ;
        }
    }

    if(IRCUT_INTELLIGENT_WHITELIGHT == mode || IRCUT_COLOR_NIGHT == mode)
    {
        if(ISNIGHT == pChannelContext->m_DayNightStutas)
        {
            if(HY_Enc_SetDayNightMode(pChannelContext, ISNIGHT) < 0)
            {
                XTRACE_MUTEX_UNLOCK(&pChannelContext->m_SensorLock);
                HY_Res_RD_UnLock(&pSysInfo->m_SysTemConfigLock);
                HY_Res_SDK_UnLock();
                ZW_DEBUG("set DayNight mode fail !\n");
                return -1 ;
            }
        }
    }
    pChannelContext->DayNightMode = mode;

    HY_Enc_SetLocalIrcutMode(pChannelContext, mode);

    if(IRCUT_TIME == pChannelContext->DayNightMode)
    {
        memcpy(&pChannelContext->m_ircutTimeModeInfo, pIrcutTimeModeInfo, sizeof(IrcutTimeModeInfo));
        ZW_DEBUG("from %d-%d-%d to %d-%d-%d\n",
            pIrcutTimeModeInfo->nStartHour,pIrcutTimeModeInfo->nStartMin,pIrcutTimeModeInfo->nStartSec,
            pIrcutTimeModeInfo->nStopHour,pIrcutTimeModeInfo->nStopMin,pIrcutTimeModeInfo->nStopSec);
    }

    XTRACE_MUTEX_UNLOCK(&pChannelContext->m_SensorLock);
    HY_Res_RD_UnLock(&pSysInfo->m_SysTemConfigLock);
    HY_Res_SDK_UnLock();
    ZW_DEBUG("set DayNight mode success !\n");

#if 0
    //网页端切换为了被动模式则给球机发信号
    if(IRCUT_PASSIVITY == pChannelContext->DayNightMode)
    {
        char buf[7] = {0};
        int len = 7;

        buf[0] = 0x51;
        buf[1] = 0x01;
        buf[2] = 0x04;
        buf[3] = 0x79;
        buf[4] = 0x3;
        if(1 == pChannelContext->m_DayNightStutas)
        {
            buf[5] = 0x2;
        }
        else
        {
            buf[5] = 0x3;
        }
        buf[6] = 0xff;

        XTRACE_MUTEX_LOCK(&pChannelContext->m_PTZSendLock);
        if(pChannelContext->m_SerailDataReadCallBack)
        {
            pChannelContext->m_SerailDataReadCallBack(buf, len);
        }
        XTRACE_MUTEX_UNLOCK(&pChannelContext->m_PTZSendLock);
    }
    else if((IRCUT_DAY != mode) && (IRCUT_NIGHT != mode))
    {
        char buf[7] = {0};
        int len = 7;

        buf[0] = 0x51;
        buf[1] = 0x01;
        buf[2] = 0x04;
        buf[3] = 0x79;
        buf[4] = 0x3;

        if(1 == pChannelContext->m_DayNightStutas)
        {
            buf[5] = 0x0;
        }
        else
        {
            buf[5] = 0x1;
        }
        buf[6] = 0xff;

        XTRACE_MUTEX_LOCK(&pChannelContext->m_PTZSendLock);
        if(pChannelContext->m_SerailDataReadCallBack)
        {
            pChannelContext->m_SerailDataReadCallBack(buf, len);
        }
        XTRACE_MUTEX_UNLOCK(&pChannelContext->m_PTZSendLock);
    }
#endif

    return	HI_SDK_SUCESS;
}

#ifdef __AGINGTEST__
/* 强制开启夜晚状态开关（老化测试） */
int	MP4Sys_SetForceNightStatus(void *hChannelHandle, int enable)
{
    CChannelContext *pChannelContext = (CChannelContext*)hChannelHandle;
    SystemInfo_t	*pSysInfo = NULL;
    HY_Res_GetSystemInfo(&pSysInfo);

    // 接口加锁
    if (HY_Res_SDK_Lock() != 0)
    {
        ZW_DEBUG("%s : SDK Not Init\n", __FUNCTION__);
        return ERR_SDK_NOINIT;
    }
    // 参数检查
    if ( !HY_Enc_IsEncHandleOk(pChannelContext))
    {
        ZW_DEBUG("%s INVALID_HANDLE\n", __FUNCTION__);
        HY_Res_SDK_UnLock();
        return ERR_INVALID_HANDLE ;
    }


    HY_Res_RD_Lock(&pSysInfo->m_SysTemConfigLock);
    XTRACE_MUTEX_LOCK(&pChannelContext->m_SensorLock);


    if(HY_Enc_SetDayNightMode(pChannelContext, enable) < 0)
    {
        XTRACE_MUTEX_UNLOCK(&pChannelContext->m_SensorLock);
        HY_Res_RD_UnLock(&pSysInfo->m_SysTemConfigLock);
        HY_Res_SDK_UnLock();
        return -1 ;
    }

    XTRACE_MUTEX_UNLOCK(&pChannelContext->m_SensorLock);
    HY_Res_RD_UnLock(&pSysInfo->m_SysTemConfigLock);
    HY_Res_SDK_UnLock();

    return 0;
}
#endif

int 	MP4Sys_EncSetVarWhiteMode(void * hChannelHandle, NIGHTVISIONWORKMODE_T mode, NIGHTVISIONCUSTOMMODE_T manualMode,  int bright)
{
    CChannelContext *pChannelContext = (CChannelContext*)hChannelHandle;

    ZW_INFO("=============into MP4Sys_EncSetColorNightMode=====mode %d===manualMode %d======\n", mode, manualMode);

    // 接口加锁
    if (HY_Res_SDK_Lock() != 0)
    {
        ZW_INFO("%s : SDK Not Init\n", __FUNCTION__);
        return ERR_SDK_NOINIT;
    }
    // 参数检查
    if (!HY_Enc_IsEncHandleOk(hChannelHandle))
    {
        ZW_INFO("%s INVALID_HANDLE\n", __FUNCTION__);
        HY_Res_SDK_UnLock();
        return ERR_INVALID_HANDLE ;
    }
    if ((mode < 0) || (mode > 1))
    {
        ZW_INFO("fail!, var white mode = %d\n", mode);
        HY_Res_SDK_UnLock();
        return -1 ;
    }
    if ((manualMode < 0) || (manualMode > 1))
    {
        ZW_INFO("fail!, var white manualMode = %d\n", manualMode);
        HY_Res_SDK_UnLock();
        return -1 ;
    }
    if ((bright < 0) || (bright > 100))
    {
        ZW_INFO("fail!, var white bright = %d\n", bright);
        HY_Res_SDK_UnLock();
        return -1 ;
    }

    pChannelContext->VarWhiteMode = mode;
    pChannelContext->VarWhiteCustomMode = manualMode;
    pChannelContext->VarWhiteCustomBright = bright;

    HY_Res_SDK_UnLock();

    return	HI_SDK_SUCESS;
}

int 	MP4Sys_EncSetVarWhiteModeInfo(void *hChannelHandle, LP_IrcutNightVisionTimeInfo pVarWhiteModeInfo, int nightThr, int dayThr)
{
    CChannelContext *pChannelContext = (CChannelContext*)hChannelHandle;

    //ZW_INFO("=============into MP4Sys_EncSetColorNightMode=====mode %d======\n", mode);

    // 接口加锁
    if (HY_Res_SDK_Lock() != 0)
    {
        ZW_INFO("%s : SDK Not Init\n", __FUNCTION__);
        return ERR_SDK_NOINIT;
    }
    // 参数检查
    if(!HY_Enc_IsEncHandleOk(hChannelHandle))
    {
        ZW_INFO("%s INVALID_HANDLE\n", __FUNCTION__);
        HY_Res_SDK_UnLock();
        return ERR_INVALID_HANDLE ;
    }
    if((nightThr < 0) || (nightThr > 100))
    {
        ZW_INFO("fail!, var white nightThr = %d\n", nightThr);
        HY_Res_SDK_UnLock();
        return -1 ;
    }
    if((dayThr < 0) || (dayThr > 100))
    {
        ZW_INFO("fail!, var white dayThr = %d\n", dayThr);
        HY_Res_SDK_UnLock();
        return -1 ;
    }

    pChannelContext->m_VarWhiteNightThreshold = nightThr;
    pChannelContext->m_VarWhiteDayThreshold = dayThr;

    memcpy(&pChannelContext->m_ircutVarWhiteModeInfo, pVarWhiteModeInfo, sizeof(IrcutNightVisionTimeInfo));
    ZW_DEBUG("from %d-%d-%d to %d-%d-%d\n",
        pVarWhiteModeInfo->nStartHour,pVarWhiteModeInfo->nStartMin,pVarWhiteModeInfo->nStartSec,
        pVarWhiteModeInfo->nStopHour,pVarWhiteModeInfo->nStopMin,pVarWhiteModeInfo->nStopSec);

    HY_Res_SDK_UnLock();

    return	HI_SDK_SUCESS;
}


/* PTZ设置IRCUT的日夜转换模式        dayNight:0 day,1 night*/
int		MP4Sys_EncPTZSetIrcutPassivity(void *hChannelHandle, int dayNight)
{
    CChannelContext *pChannelContext = (CChannelContext*)hChannelHandle;
    SystemInfo_t	*pSysInfo;

    ZW_DEBUG("=============into MP4Sys_EncPTZSetIrcutPassivity==mode:%d=========\n", dayNight);
#ifdef NO_CHANGE_IMAGE_SETING
    return 0;
#endif

    // 接口加锁
    if (HY_Res_SDK_Lock() != 0)
    {
        ZW_DEBUG("%s : SDK Not Init\n", __FUNCTION__);
        return ERR_SDK_NOINIT;
    }
    // 参数检查
    if( !HY_Enc_IsEncHandleOk(hChannelHandle))
    {
        ZW_DEBUG("%s INVALID_HANDLE\n", __FUNCTION__);
        HY_Res_SDK_UnLock();
        return ERR_INVALID_HANDLE ;
    }
    if(pChannelContext->m_bRecord != TRUE)
    {
        SDKPRINT("please start encode first !\n");
        HY_Res_SDK_UnLock();
        return -1 ;
    }

    if((dayNight != 0) && (dayNight != 1))
    {
        HY_Res_SDK_UnLock();
        ZW_DEBUG("Day Night Mode limited at 0~1 !\n");
        return ERR_INVALID_ARGUMENT;
    }


    HY_Res_GetSystemInfo(&pSysInfo);
    HY_Res_RD_Lock(&pSysInfo->m_SysTemConfigLock);
    XTRACE_MUTEX_LOCK(&pChannelContext->m_SensorLock);

    pChannelContext->DayNightMode = IRCUT_PASSIVITY;
    pChannelContext->m_PtzCtrlIrcut = 1;
    pChannelContext->m_PtzSetDayOrNight = dayNight;

    HY_Enc_SetLocalIrcutMode(pChannelContext, pChannelContext->DayNightMode);

    XTRACE_MUTEX_UNLOCK(&pChannelContext->m_SensorLock);
    HY_Res_RD_UnLock(&pSysInfo->m_SysTemConfigLock);
    HY_Res_SDK_UnLock();
    ZW_DEBUG("set DayNight mode success !\n");

    return	HI_SDK_SUCESS;
}

/**
 * @description: 更新PTZ状态运动状态
 * @param {void} *hChannelHandle 通道句柄
 * @param {int} *isDay 是否是白天
 * @return {int} 0：成功 <0：失败
 */
int MP4Sys_UpdatePtzStatus(void *hChannelHandle, ePtzStatus enStatus)
{
    CChannelContext *pChannelContext = (CChannelContext *)hChannelHandle;

    // ZW_ERROR("=============into MP4Sys_UpdatePtzStatus==========\n");

    // 接口加锁
    if (HY_Res_SDK_Lock() != 0)
    {
        ZW_DEBUG("%s : SDK Not Init\n", __FUNCTION__);
        return ERR_SDK_NOINIT;
    }
    // 参数检查
    if (!HY_Enc_IsEncHandleOk(hChannelHandle))
    {
        ZW_DEBUG("%s INVALID_HANDLE\n", __FUNCTION__);
        HY_Res_SDK_UnLock();
        return ERR_INVALID_HANDLE;
    }

    if (enStatus != pChannelContext->m_enPtzStatus)
    {
        pChannelContext->m_enPtzStatus = enStatus;

        switch (enStatus)
        {
            case ePtzStatus_Stop:
                {
                    HY_Enc_SetAudioInVol(pChannelContext, pChannelContext->m_nAudioInVolNormal, 1);
                    break;
                }

            case ePtzStatus_Moving:
                {
                    if(pChannelContext->m_nAudioInVolNormal != 0)
                    {
                        HY_Enc_SetAudioInVol(pChannelContext, AUDIO_AI_PTZ_MOVE, 0);
                    }
                    break;
                }
        }
    }

    HY_Res_SDK_UnLock();
    return HI_SDK_SUCESS;
}

/* 跟踪是否回到了看守位 */
int MP4Sys_PtzForTrackBacktoPreset(void *hChannelHandle, ePtz2PresetStatus toPresetStat)
{
    int nRet = 0;
    CChannelContext 	*pChannelContext = (CChannelContext*)hChannelHandle;

    ZW_DEBUG("--------into MP4Sys_PtzForTrackBacktoPreset------\n");

    /* 接口加锁 */
    if (HY_Res_SDK_Lock() != 0)
    {
        ZW_ERROR("SDK Not Init\n");
        return ERR_SDK_NOINIT;
    }

    /* 参数检查 */
    if (!HY_Enc_IsEncHandleOk(hChannelHandle))
    {
        ZW_ERROR("INVALID_HANDLE\n");
        HY_Res_SDK_UnLock();
        return ERR_INVALID_HANDLE;
    }

    pChannelContext->m_iPtzTrackBacktoPreset = toPresetStat;

    HY_Res_SDK_UnLock();
    return nRet;
}

/**
 * @description: 获取当前得日夜状态
 * @param {void} *hChannelHandle 通道句柄
 * @param {int} *isDay 是否是白天
 * @return {int} 0：成功 <0：失败
 */
int MP4Sys_EncGetDayNightStatus(void *hChannelHandle, int *isDay)
{
    //ZW_DEBUG("=============into MP4Sys_EncGetDayNightStatus===========\n");

    CChannelContext *pChCtx = (CChannelContext *)hChannelHandle;
    SystemInfo_t *pSysInfo;

#ifdef NO_CHANGE_IMAGE_SETING
    return 0;
#endif

    // 接口加锁
    if (HY_Res_SDK_Lock() != 0)
    {
        ZW_DEBUG("%s : SDK Not Init\n", __FUNCTION__);
        return ERR_SDK_NOINIT;
    }

    // 参数检查
    if (!HY_Enc_IsEncHandleOk(hChannelHandle))
    {
        ZW_DEBUG("%s INVALID_HANDLE\n", __FUNCTION__);
        HY_Res_SDK_UnLock();
        return ERR_INVALID_HANDLE;
    }

    if (pChCtx->m_bRecord != TRUE)
    {
        SDKPRINT("please start encode first !\n");
        HY_Res_SDK_UnLock();
        return -1;
    }

    if (!isDay)
    {
        ZW_ERROR("NULL Param!\n");
        HY_Res_SDK_UnLock();
        return ERR_INVALID_ARGUMENT;
    }

    HY_Res_GetSystemInfo(&pSysInfo);
    HY_Res_RD_Lock(&pSysInfo->m_SysTemConfigLock);
    XTRACE_MUTEX_LOCK(&pChCtx->m_SensorLock);
    *isDay = pChCtx->m_DayNightStutas;
    XTRACE_MUTEX_UNLOCK(&pChCtx->m_SensorLock);
    HY_Res_RD_UnLock(&pSysInfo->m_SysTemConfigLock);
    HY_Res_SDK_UnLock();

    return HI_SDK_SUCESS;
}

int 	MP4Sys_EncSetColorNightMode(void *hChannelHandle, NIGHTVISIONWORKMODE_T mode, LP_IrcutNightVisionTimeInfo pColorNightModeInfo)
{
    CChannelContext *pChannelContext = (CChannelContext*)hChannelHandle;

    ZW_INFO("=============into MP4Sys_EncSetColorNightMode=====mode %d======\n", mode);

    // 接口加锁
    if (HY_Res_SDK_Lock() != 0)
    {
        ZW_INFO("%s : SDK Not Init\n", __FUNCTION__);
        return ERR_SDK_NOINIT;
    }
    // 参数检查
    if(!HY_Enc_IsEncHandleOk(hChannelHandle))
    {
        ZW_INFO("%s INVALID_HANDLE\n", __FUNCTION__);
        HY_Res_SDK_UnLock();
        return ERR_INVALID_HANDLE ;
    }
    if((mode < 0) || (mode > 1))
    {
        ZW_INFO("fail!, color night mode = %d\n", mode);
        HY_Res_SDK_UnLock();
        return -1 ;
    }

    pChannelContext->ColorNightMode = mode;

    if(WORKMODE_CUSTOM == pChannelContext->ColorNightMode)
    {
        memcpy(&pChannelContext->m_ircutColorNightModeInfo, pColorNightModeInfo, sizeof(IrcutNightVisionTimeInfo));
        ZW_DEBUG("from %d-%d-%d to %d-%d-%d\n",
            pColorNightModeInfo->nStartHour,pColorNightModeInfo->nStartMin,pColorNightModeInfo->nStartSec,
            pColorNightModeInfo->nStopHour,pColorNightModeInfo->nStopMin,pColorNightModeInfo->nStopSec);
    }

    HY_Res_SDK_UnLock();

    return	HI_SDK_SUCESS;
}

int	MP4Sys_SetDefogStrength(void *hChannelHandle,  int strength)
{
    SystemInfo_t    *pSysInfo = NULL;
    CChannelContext *pChannelContext = (CChannelContext *) hChannelHandle;

    ZW_INFO("=============into MP4Sys_SetDefogStrength===========\n");
#ifdef NO_CHANGE_IMAGE_SETING
    return 0;
#endif

    /* 接口加锁 */
    if (HY_Res_SDK_Lock() != 0)
    {
        ZW_ERROR("SDK Not Init\n");
        return ERR_SDK_NOINIT;
    }

    //参数范围检查
    if(!HY_Enc_IsEncHandleOk( hChannelHandle))
    {
        ZW_ERROR("INVALID_HANDLE\n");
        HY_Res_SDK_UnLock();
        return ERR_INVALID_HANDLE ;
    }

    if((strength < 0) || (strength > 100))
    {
        ZW_INFO("fail!, strength = %d\n", strength);
        HY_Res_SDK_UnLock();
        return -1 ;
    }

    if(pChannelContext->m_bRecord != TRUE)
    {
        SDKPRINT("please start encode first !\n");
        HY_Res_SDK_UnLock();
        return -1 ;
    }

    HY_Res_GetSystemInfo(&pSysInfo);
    HY_Res_RD_Lock(&pSysInfo->m_SysTemConfigLock);

    XTRACE_MUTEX_LOCK(&pChannelContext->m_SensorLock);
    if(HY_Enc_SetDefogStrengthParam(pChannelContext, strength) < 0)
    {
        XTRACE_MUTEX_UNLOCK(&pChannelContext->m_SensorLock);
        HY_Res_RD_UnLock(&pSysInfo->m_SysTemConfigLock);
        HY_Res_SDK_UnLock();
        ZW_ERROR("################### set DefogStrength fail ###################\n");
        return -1;
    }

    XTRACE_MUTEX_UNLOCK(&pChannelContext->m_SensorLock);
    HY_Res_RD_UnLock(&pSysInfo->m_SysTemConfigLock);
    HY_Res_SDK_UnLock();
    ZW_INFO("set DefogStrength success!\n");

    return HI_SDK_SUCESS;
}

int	MP4Sys_SetAntiFlickerStrength(void *hChannelHandle, int strength)
{
    SystemInfo_t    *pSysInfo = NULL;
    CChannelContext *pChannelContext = (CChannelContext *) hChannelHandle;

    ZW_INFO("=============into MP4Sys_SetAntiFlickerStrength===========\n");
#ifdef NO_CHANGE_IMAGE_SETING
    return 0;
#endif

    /* 接口加锁 */
    if (HY_Res_SDK_Lock() != 0)
    {
        ZW_ERROR("SDK Not Init\n");
        return ERR_SDK_NOINIT;
    }

    //参数范围检查
    if(!HY_Enc_IsEncHandleOk( hChannelHandle))
    {
        ZW_ERROR("INVALID_HANDLE\n");
        HY_Res_SDK_UnLock();
        return ERR_INVALID_HANDLE ;
    }

    if((strength < 0) || (strength > 100))
    {
        ZW_INFO("fail!, strength = %d\n", strength);
        HY_Res_SDK_UnLock();
        return -1 ;
    }

    if(pChannelContext->m_bRecord != TRUE)
    {
        SDKPRINT("please start encode first !\n");
        HY_Res_SDK_UnLock();
        return -1 ;
    }

    HY_Res_GetSystemInfo(&pSysInfo);
    HY_Res_RD_Lock(&pSysInfo->m_SysTemConfigLock);

    XTRACE_MUTEX_LOCK(&pChannelContext->m_SensorLock);
    if(HY_Enc_SetAntiFlickerStrengthParam(pChannelContext, strength) < 0)
    {
        XTRACE_MUTEX_UNLOCK(&pChannelContext->m_SensorLock);
        HY_Res_RD_UnLock(&pSysInfo->m_SysTemConfigLock);
        HY_Res_SDK_UnLock();
        ZW_ERROR("################### set AntiFlickerStrength fail ###################\n");
        return -1;
    }

    XTRACE_MUTEX_UNLOCK(&pChannelContext->m_SensorLock);
    HY_Res_RD_UnLock(&pSysInfo->m_SysTemConfigLock);
    HY_Res_SDK_UnLock();
    ZW_INFO("set AntiFlickerStrength success!\n");

    return HI_SDK_SUCESS;
}

int	MP4Sys_SetImageMode(void *hChannelHandle,  int mode)
{
    CChannelContext *pChannelContext = (CChannelContext*)hChannelHandle;

    ZW_INFO("=============into MP4Sys_SetImageMode=====mode %d======\n", mode);
#ifdef NO_CHANGE_IMAGE_SETING
    return 0;
#endif

    // 接口加锁
    if (HY_Res_SDK_Lock() != 0)
    {
        ZW_INFO("%s : SDK Not Init\n", __FUNCTION__);
        return ERR_SDK_NOINIT;
    }
    // 参数检查
    if(!HY_Enc_IsEncHandleOk(hChannelHandle))
    {
        ZW_INFO("%s INVALID_HANDLE\n", __FUNCTION__);
        HY_Res_SDK_UnLock();
        return ERR_INVALID_HANDLE ;
    }
    if((mode < 0) || (mode > 10))
    {
        ZW_INFO("fail!, mode = %d\n", mode);
        HY_Res_SDK_UnLock();
        return -1 ;
    }

    pChannelContext->m_ImageMode = mode;

    HY_Enc_SetLocalImageMode(pChannelContext, mode);
    ZW_INFO("============= m_ImageMode=%d======\n", pChannelContext->m_ImageMode);

    HY_Res_SDK_UnLock();

    return	HI_SDK_SUCESS;
}

//宽动态
int 	MP4Sys_SetLocalExpMode(void *hChannelHandle, int ExpMode)
{
    SystemInfo_t	*pSysInfo;
    CChannelContext *pChannelContext = (CChannelContext *) hChannelHandle;

    ZW_DEBUG("=============into MP4Sys_SetLocalExpMode=====ExpMode %d======\n", ExpMode);
#ifdef NO_CHANGE_IMAGE_SETING
    return 0;
#endif

    /* 接口加锁 */
    if (HY_Res_SDK_Lock() != 0)
    {
        ZW_DEBUG("%s : SDK Not Init\n", __FUNCTION__);
        return ERR_SDK_NOINIT;
    }

    //参数范围检查
    if( !HY_Enc_IsEncHandleOk( hChannelHandle ) )
    {
        ZW_DEBUG("%s INVALID_HANDLE\n", __FUNCTION__);
        HY_Res_SDK_UnLock();
        return ERR_INVALID_HANDLE ;
    }
    if(pChannelContext->m_bRecord != TRUE)
    {
        SDKPRINT("please start encode first !\n");
        HY_Res_SDK_UnLock();
        return -1 ;
    }

    if((ExpMode < 0) || (ExpMode > 4))
    {
        ZW_DEBUG("MCTFStrength limited at 0~4");
        HY_Res_SDK_UnLock();
        return ERR_INVALID_ARGUMENT;
    }

    HY_Res_GetSystemInfo(&pSysInfo);
    HY_Res_RD_Lock(&pSysInfo->m_SysTemConfigLock);

    XTRACE_MUTEX_LOCK(&pSysInfo->m_WriteFileLock);
    XTRACE_MUTEX_LOCK(&pChannelContext->m_InitVPSSLock);
    XTRACE_MUTEX_LOCK(&pChannelContext->m_SensorLock);

    if(HY_Enc_SetLocalExpMode(pChannelContext, ExpMode) < 0)
    {
        XTRACE_MUTEX_UNLOCK(&pChannelContext->m_SensorLock);
        XTRACE_MUTEX_UNLOCK(&pChannelContext->m_InitVPSSLock);
        XTRACE_MUTEX_UNLOCK(&pSysInfo->m_WriteFileLock);
        HY_Res_RD_UnLock(&pSysInfo->m_SysTemConfigLock);
        HY_Res_SDK_UnLock();
        ZW_DEBUG("set Exposure Mode fail !\n");
        return -1;
    }

    XTRACE_MUTEX_UNLOCK(&pChannelContext->m_SensorLock);
    XTRACE_MUTEX_UNLOCK(&pChannelContext->m_InitVPSSLock);
    XTRACE_MUTEX_UNLOCK(&pSysInfo->m_WriteFileLock);

    HY_Res_RD_UnLock(&pSysInfo->m_SysTemConfigLock);
    HY_Res_SDK_UnLock();
    ZW_DEBUG("set Exposure Mode ok !\n");

    return HI_SDK_SUCESS;
}

//3D降噪
int	 	MP4Sys_SetMCTFStrength(void *hChannelHandle, int MCTFStrength)
{
    SystemInfo_t	*pSysInfo;
    CChannelContext *pChannelContext = (CChannelContext *) hChannelHandle;

    ZW_DEBUG("=============into MP4Sys_SetMCTFStrength=====MCTFStrength %d======\n", MCTFStrength);
#ifdef NO_CHANGE_IMAGE_SETING
    return 0;
#endif

    /* 接口加锁 */
    if (HY_Res_SDK_Lock() != 0)
    {
        ZW_DEBUG("%s : SDK Not Init\n", __FUNCTION__);
        return ERR_SDK_NOINIT;
    }

    //参数范围检查
    if( !HY_Enc_IsEncHandleOk( hChannelHandle ) )
    {
        ZW_DEBUG("%s INVALID_HANDLE\n", __FUNCTION__);
        HY_Res_SDK_UnLock();
        return ERR_INVALID_HANDLE ;
    }
    if(pChannelContext->m_bRecord != TRUE)
    {
        SDKPRINT("please start encode first !\n");
        HY_Res_SDK_UnLock();
        return -1 ;
    }
    if((MCTFStrength < 0) || (MCTFStrength > 3))
    {
        ZW_DEBUG("MCTFStrength limited at 0~3");
        HY_Res_SDK_UnLock();
        return ERR_INVALID_ARGUMENT;
    }

    HY_Res_GetSystemInfo(&pSysInfo);
    HY_Res_RD_Lock(&pSysInfo->m_SysTemConfigLock);
    XTRACE_MUTEX_LOCK(&pChannelContext->m_SensorLock);

    if(HY_Enc_SetMCTFStrength(pChannelContext, MCTFStrength) != 0)
    {
        XTRACE_MUTEX_UNLOCK(&pChannelContext->m_SensorLock);
        HY_Res_RD_UnLock(&pSysInfo->m_SysTemConfigLock);
        HY_Res_SDK_UnLock();
        ZW_DEBUG("set MCTF Strength fail !\n");
        return -1;//
    }

    XTRACE_MUTEX_UNLOCK(&pChannelContext->m_SensorLock);
    HY_Res_RD_UnLock(&pSysInfo->m_SysTemConfigLock);
    HY_Res_SDK_UnLock();
    ZW_DEBUG("set MCTF Strength ok !\n");

    return HI_SDK_SUCESS;
}

int MP4Sys_SetLensOrientation(unsigned int nLensOrientation)
{
    SystemInfo_t *pSysInfo = NULL;
    HY_Res_GetSystemInfo(&pSysInfo);
    CChannelContext *pChannelContext = NULL;
    HY_Enc_GetEncChInfo(&pChannelContext);

    ZW_DEBUG("[imagetest] restart to set orientation  %d =====\n", nLensOrientation);

    if (pSysInfo->m_LensOrientation != nLensOrientation)
    {
        pSysInfo->m_LensOrientation = nLensOrientation;
        HY_Enc_SetFlipRotateMode(pChannelContext, pChannelContext->flipRotate);
    }

    return 0;
}

//旋转
int 	MP4Sys_SetFlipRotateMode(void *hChannelHandle, FLIP_ROTATE_MODE_E flipRotate)
{
    CChannelContext 	*pChannelContext = (CChannelContext*)hChannelHandle;
    SystemInfo_t		*pSysInfo = NULL;

    SDKPRINT("--------into MP4Sys_SetFlipRotateMode---flipRotate %d-----\n", flipRotate);

    /* 接口加锁 */
    if (HY_Res_SDK_Lock() != 0)
    {
        ZW_ERROR("SDK Not Init\n");
        return ERR_SDK_NOINIT;
    }
    /* 参数检查 */
    if( !HY_Enc_IsEncHandleOk( hChannelHandle ) )
    {
        ZW_ERROR("INVALID_HANDLE\n");
        HY_Res_SDK_UnLock();
        return ERR_INVALID_HANDLE ;
    }

    /* 先禁止在未启动前调用 */
    if(pChannelContext->m_bRecord != TRUE)
    {
        SDKPRINT("please start encode first !\n");
        HY_Res_SDK_UnLock();
        return -1 ;
    }

    HY_Res_GetSystemInfo(&pSysInfo);

    HY_Res_RD_Lock(&pSysInfo->m_SysTemConfigLock);			//加锁禁止系统修改制式
    XTRACE_MUTEX_LOCK(&pChannelContext->m_SensorLock);
    XTRACE_MUTEX_LOCK(&pChannelContext->m_EncLock);

    if(HY_Enc_SetFlipRotateMode(pChannelContext, flipRotate) < 0)
    {
        XTRACE_MUTEX_UNLOCK(&pChannelContext->m_EncLock);
        XTRACE_MUTEX_UNLOCK(&pChannelContext->m_SensorLock);
        HY_Res_RD_UnLock(&pSysInfo->m_SysTemConfigLock);    //允许系统修改制式
        HY_Res_SDK_UnLock(); 							   //释放接口锁
        ZW_DEBUG(" [DONE] Set Flip Rotate Mode fail !\n");
        return -1;
    }

    pChannelContext->flipRotate = flipRotate;

    XTRACE_MUTEX_UNLOCK(&pChannelContext->m_EncLock);
    XTRACE_MUTEX_UNLOCK(&pChannelContext->m_SensorLock);
    HY_Res_RD_UnLock(&pSysInfo->m_SysTemConfigLock);	//允许系统修改制式
    HY_Res_SDK_UnLock();								//释放接口锁
    ZW_DEBUG(" [DONE] Set Flip Rotate Mode ok !\n");

    return HI_SDK_SUCESS;
}

//pc/tv模式
int 	MP4Sys_SetChromaScale(void *hChannelHandle, CHROMA_SCALE_E chromaScale)
{
    return HI_SDK_SUCESS;
}

int		MP4Sys_SetDcIris(void *hChannelHandle, int enable)
{
    CChannelContext 	*pChannelContext = (CChannelContext*)hChannelHandle;
    SystemInfo_t		*pSysInfo = NULL;

    ZW_DEBUG("--------into MP4Sys_SetDcIris---enable %d-----\n", enable);
#ifdef NO_CHANGE_IMAGE_SETING
    return 0;
#endif

    /* 接口加锁 */
    if (HY_Res_SDK_Lock() != 0)
    {
        ZW_ERROR("%s : SDK Not Init\n", __FUNCTION__);
        return ERR_SDK_NOINIT;
    }
    /* 参数检查 */
    if( !HY_Enc_IsEncHandleOk( hChannelHandle ) )
    {
        ZW_ERROR("%s INVALID_HANDLE\n", __FUNCTION__);
        HY_Res_SDK_UnLock();
        return ERR_INVALID_HANDLE ;
    }

    /* 先禁止在未启动前调用 */
    if(pChannelContext->m_bRecord != TRUE)
    {
        SDKPRINT("please start encode first !\n");
        HY_Res_SDK_UnLock();
        return -1 ;
    }

    HY_Res_GetSystemInfo(&pSysInfo);

    HY_Res_RD_Lock(&pSysInfo->m_SysTemConfigLock);			//加锁禁止系统修改制式
    XTRACE_MUTEX_LOCK(&pChannelContext->m_SensorLock);
    XTRACE_MUTEX_LOCK(&pChannelContext->m_EncLock);

    if(HY_Enc_SetDcIris(pChannelContext, enable) < 0)
    {
        XTRACE_MUTEX_UNLOCK(&pChannelContext->m_EncLock);
        XTRACE_MUTEX_UNLOCK(&pChannelContext->m_SensorLock);
        HY_Res_RD_UnLock(&pSysInfo->m_SysTemConfigLock);	//允许系统修改制式
        HY_Res_SDK_UnLock();							   //释放接口锁
        ZW_DEBUG(" [DONE] Set DC IRIS fail !\n");
        return -1;
    }

    XTRACE_MUTEX_UNLOCK(&pChannelContext->m_EncLock);
    XTRACE_MUTEX_UNLOCK(&pChannelContext->m_SensorLock);
    HY_Res_RD_UnLock(&pSysInfo->m_SysTemConfigLock);	//允许系统修改制式
    HY_Res_SDK_UnLock();								//释放接口锁
    ZW_DEBUG(" [DONE] Set DC IRIS ok !\n");

    return HI_SDK_SUCESS;
}

int 	MP4Sys_SetDcIrisDuty(void *hChannelHandle, int duty)
{
    return HI_SDK_SUCESS;
}

int     MP4Sys_SetAudioInVol(void * hChannelHandle, int vol)
{
    CChannelContext *pChannelContext = (CChannelContext*)hChannelHandle;

    ZW_DEBUG("=============into MP4Sys_SetAudioInVol==vol:%d=========\n", vol);
#ifdef NO_SET_AUDIO
    return 0;
#endif

    // 接口加锁
    if (HY_Res_SDK_Lock() != 0)
    {
        ZW_DEBUG("%s : SDK Not Init\n", __FUNCTION__);
        return ERR_SDK_NOINIT;
    }
    // 参数检查
    if( !HY_Enc_IsEncHandleOk(hChannelHandle))
    {
        ZW_DEBUG("%s INVALID_HANDLE\n", __FUNCTION__);
        HY_Res_SDK_UnLock();
        return ERR_INVALID_ARGUMENT ;
    }

    //参数范围
    if((vol < 0) || (vol > 100))
    {
        ZW_DEBUG("vol thres limited at 0~100\n");
        HY_Res_SDK_UnLock();
        return ERR_INVALID_ARGUMENT;
    }

    XTRACE_MUTEX_LOCK(&pChannelContext->m_AudioLock);
    if(HY_Enc_SetAudioInVol(pChannelContext, vol, 0) < 0)
    {
        XTRACE_MUTEX_UNLOCK(&pChannelContext->m_AudioLock);
        HY_Res_SDK_UnLock();
        ZW_ERROR("################### set ae fail ###################\n");
        return -1;
    }

    pChannelContext->m_nAudioInVolNormal = vol;

    XTRACE_MUTEX_UNLOCK(&pChannelContext->m_AudioLock);

    HY_Res_SDK_UnLock();

    return	HI_SDK_SUCESS;
}

int     MP4Sys_SetAudioOutVol(void * hChannelHandle, int vol, int IsUpdateVoiceCache,  int m_IntercomMode)
{
    CChannelContext *pChannelContext = (CChannelContext*)hChannelHandle;

    //ZW_DEBUG("=============into MP4Sys_SetAudioOutVol==vol:%d, IsUpdateVoiceCache:%d=========\n", vol, IsUpdateVoiceCache);
#ifdef NO_SET_AUDIO
    return 0;
#endif

    // 接口加锁
    if (HY_Res_SDK_Lock() != 0)
    {
        ZW_DEBUG("%s : SDK Not Init\n", __FUNCTION__);
        return ERR_SDK_NOINIT;
    }
    // 参数检查
    if( !HY_Enc_IsEncHandleOk(hChannelHandle))
    {
        ZW_DEBUG("%s INVALID_HANDLE\n", __FUNCTION__);
        HY_Res_SDK_UnLock();
        return ERR_INVALID_ARGUMENT ;
    }

    //参数范围
    if((vol < 0) || (vol > 100))
    {
        ZW_DEBUG("vol thres limited at 0~100\n");
        HY_Res_SDK_UnLock();
        return ERR_INVALID_ARGUMENT;
    }

    XTRACE_MUTEX_LOCK(&pChannelContext->m_AudioLock);
    if(HY_Enc_SetAudioOutVol(pChannelContext, vol, IsUpdateVoiceCache, m_IntercomMode) < 0)
    {
        XTRACE_MUTEX_UNLOCK(&pChannelContext->m_AudioLock);
        HY_Res_SDK_UnLock();
        ZW_ERROR("################### set ae fail ###################\n");
        return -1;
    }

    XTRACE_MUTEX_UNLOCK(&pChannelContext->m_AudioLock);
    HY_Res_SDK_UnLock();

    return	HI_SDK_SUCESS;
}

int	MP4Sys_SetAudioMappingVol(void * hChannelHandle, int vol, int Type)
{
    CChannelContext *pChannelContext = (CChannelContext*)hChannelHandle;

    //ZW_DEBUG("=============into MP4Sys_SetAudioOutVol==vol:%d, IsUpdateVoiceCache:%d=========\n", vol, IsUpdateVoiceCache);
#ifdef NO_SET_AUDIO
    return 0;
#endif

    // 接口加锁
    if (HY_Res_SDK_Lock() != 0)
    {
        ZW_DEBUG("%s : SDK Not Init\n", __FUNCTION__);
        return ERR_SDK_NOINIT;
    }
    // 参数检查
    if( !HY_Enc_IsEncHandleOk(hChannelHandle))
    {
        ZW_DEBUG("%s INVALID_HANDLE\n", __FUNCTION__);
        HY_Res_SDK_UnLock();
        return ERR_INVALID_ARGUMENT ;
    }

    //参数范围
    if((vol < 0) || (vol > 100)||(Type < 0)||(Type > 3))
    {
        ZW_DEBUG("vol thres limited at 0~100 | Type thres limited at 0~4\n");
        HY_Res_SDK_UnLock();
        return ERR_INVALID_ARGUMENT;
    }

    XTRACE_MUTEX_LOCK(&pChannelContext->m_AudioLock);
    HY_Enc_SetAudioMappingVol(pChannelContext, vol, Type);
    XTRACE_MUTEX_UNLOCK(&pChannelContext->m_AudioLock);
    HY_Res_SDK_UnLock();

    return	HI_SDK_SUCESS;
}



int MP4Sys_EncGetIntercomMode( int *iIntercomMode )
{
    CChannelContext	*pChannelContext = NULL;
    HY_Enc_GetEncChInfo(&pChannelContext);

    // 接口加锁
    if (HY_Res_SDK_Lock() != 0)
    {
        ZW_DEBUG("%s : SDK Not Init\n", __FUNCTION__);
        return ERR_SDK_NOINIT;
    }

    *iIntercomMode = pChannelContext->m_IntercomMode;

    HY_Res_SDK_UnLock();
    ZW_DEBUG("get Intercom Mode success !\n");

    return  HI_SDK_SUCESS;
}

int MP4Sys_EncSetIntercomMode( int iIntercomMode )
{
    CChannelContext	*pChannelContext = NULL;
    HY_Enc_GetEncChInfo(&pChannelContext);

    // 接口加锁
    if (HY_Res_SDK_Lock() != 0)
    {
        ZW_DEBUG("%s : SDK Not Init\n", __FUNCTION__);
        return ERR_SDK_NOINIT;
    }

    if (iIntercomMode != pChannelContext->m_IntercomMode)
    {
        pChannelContext->m_IntercomMode = iIntercomMode;
    }

    HY_Res_SDK_UnLock();
    ZW_DEBUG("set Intercom Mode success !\n");

    return  HI_SDK_SUCESS;
}

/* 直接从加密设备里面读取版型，不需要初始化SDK */
int     MP4Sys_GetBoardTypeNoSDK(char *pInfo)
{
    int s32Ret = 0;
#if 0
    int	BoardId = 0;
    char info[64];
    memset(info, 0, sizeof(info));

    s32Ret = HY_Res_GetInfo(info);
    if (s32Ret)
    {
        ZW_ERROR("Fail to Called HY_Res_GetInfo, ret = %#x\n", s32Ret);
        return s32Ret;
    }

    /* 版型格式为0x[13][14] */
    BoardId = info[13];
    BoardId = (BoardId & 0xFF) << 8;
    BoardId = BoardId | info[14];
    sprintf(pInfo, "0x%X", BoardId);
#endif
    return s32Ret;
}

int MP4Sys_SetSerialDataCallBack(SERIALDATA_READ_CALLBACK SerailDataReadCallBack)
{
    return 0;
}

/* 获取激活状态码(AF和双光源) */
int MP4Sys_GetActiveStatusCode(int *pActiveStatusCode)
{
    if (NULL == pActiveStatusCode)
    {
        ZW_ERROR("pActiveStatusCode is NULL\n");
        return ERR_INVALID_ARGUMENT;
    }

    /* 接口加锁 */
    if (HY_Res_SDK_Lock() != 0)
    {
        ZW_ERROR("SDK Not Init\n");
        return ERR_SDK_NOINIT;
    }

    SystemInfo_t *pSysInfo = NULL;

    HY_Res_GetSystemInfo(&pSysInfo);
    *pActiveStatusCode = pSysInfo->m_nFuncActiveStatusCode;

    HY_Res_SDK_UnLock();

    return 0;
}

/* 设置激活状态码(AF和双光源) */
int MP4Sys_SetActiveStatusCode(int nActiveStatusCode)
{
    /* 接口加锁 */
    if (HY_Res_SDK_Lock() != 0)
    {
        ZW_ERROR("SDK Not Init\n");
        return ERR_SDK_NOINIT;
    }

    SystemInfo_t *pSysInfo = NULL;

    HY_Res_GetSystemInfo(&pSysInfo);
    pSysInfo->m_nFuncActiveStatusCode |= nActiveStatusCode;

    HY_Res_SDK_UnLock();

    return 0;
}

// 未授权(授权失败)时，需要在页面上面显示特定的字符串
int MP4Sys_SetNoAuthInfoOSDDisplay(void)
{
    FILE *fid = NULL;
    CChannelContext         *pChCtx = NULL;

    HY_Enc_GetEncChInfo(&pChCtx);

    if (HY_Res_SDK_Check() != 0)
    {
        ZW_ERROR("SDK Not Init\n");
        return ERR_SDK_NOINIT;
    }

    fid = fopen(UNAUTHORIZED_OSD_PATH, "rb");
    if (NULL == fid)
    {
        ZW_ERROR("\nOpen %s faild\n", UNAUTHORIZED_OSD_PATH);
        return -1;
    }

    //获取文件大小
    fseek(fid, 0L, SEEK_END);
    int nSize = ftell(fid);

    char *pBuffer = (char *)malloc(nSize);
    if (NULL == pBuffer)
    {
        ZW_ERROR("malloc(%d) faild\n", nSize);
    }
    else
    {
        memset(pBuffer, 0, nSize);

        fseek(fid, 0L, SEEK_SET);
        fread(pBuffer, sizeof(char), nSize, fid);

        //清除之前OSD2的数据
        memset(&pChCtx->m_TextOSDV2Info, 0 , sizeof(SysTextOSDV2_t));

        TextOSDInfo_t stTextOsd = {0};
        stTextOsd.TextNum = 10;
        stTextOsd.Start_X = 0;
        stTextOsd.Start_Y = 0;
        stTextOsd.pPixinfo = pBuffer;
        MP4Sys_EncSetOsdText(MP4Sys_EncChannelOpen(0), &stTextOsd);

        free(pBuffer);
        pBuffer = NULL;
    }

    fclose(fid);

    return 0;
}

/* 设置授权信息，成功返回授权错误码，失败返回-1 */
int MP4Sys_SetAuthInfo(const unsigned char *pszAuthLicense)
{
    int ret = -1;

    if (NULL == pszAuthLicense)
    {
        ZW_ERROR("pszAuthLicense is NULL!\n");
        return -1;
    }

    /* 接口加锁 */
    if (HY_Res_SDK_Lock() != 0)
    {
        return ERR_SDK_NOINIT;
    }

    ret = HY_Res_SetAuthInfo(pszAuthLicense);

    HY_Res_SDK_UnLock();

    return ret;
}

/* 获取授权码和授权状态，成功返回0，失败返回-1 */
int MP4Sys_GetAuthInfo(unsigned char *pszAuthLicense, unsigned int *pAuthState)
{
    if (NULL==pszAuthLicense || NULL==pAuthState)
    {
        ZW_ERROR("pszAuthLicense or pAuthState is NULL!\n");
        return -1;
    }

    if (HY_Res_SDK_Check() != 0)
    {
        ZW_ERROR("SDK Not Init\n");
        return ERR_SDK_NOINIT;
    }

    int ret = -1;

    ret = HY_Res_GetAuthInfo(pszAuthLicense, pAuthState);

    return ret;
}

/* 移除(取消)授权，直接返回REMOVE_AUTH_SUCCESS */
int MP4Sys_RemoveAuthLicense(void)
{
    SystemInfo_t *pSysInfo = NULL;

    if (HY_Res_SDK_Check() != 0)
    {
        ZW_ERROR("SDK Not Init\n");
        return ERR_SDK_NOINIT;
    }

    HY_Res_GetSystemInfo(&pSysInfo);
#ifdef CHECK_AUTHEN

    /* 恢复成默认版型 */
    pSysInfo->m_BoardInfo = GetDefaultBoardID();

    /* 清空授权码 */
    memset(pSysInfo->m_szAuthLicense, 0, sizeof(pSysInfo->m_szAuthLicense));

    /* 取消授权不用清空序列号 */
#if 0
    /* 清空序列号 */
    memset(pSysInfo->m_szSerialNum, 0, sizeof(pSysInfo->m_szSerialNum));
#endif
    /* 删除用户设置的DID配置文件 */
    if (0 == access(DID_CONF, F_OK))
    {
        unlink(DID_CONF);
    }
    if (0 == access(DID_CONF_BAK, F_OK))
    {
        unlink(DID_CONF_BAK);
    }

    /* 删除u-boot下面"auth_license"字段和AUTHORIZE_LICENSE_FILE_CFG配置文件     */
    RemoveLicenseAuthorize();

    /* OSD显示"未授权"字样 */
    MP4Sys_SetNoAuthInfoOSDDisplay();
#endif
    /* 清空状态 */
    pSysInfo->m_bAuthEncryptOK = FALSE;

    return REMOVE_AUTH_SUCCESS;
}

int MP4Sys_LightBoardType(int nLightType)
{
    SystemInfo_t *pSysInfo = NULL;

    if (HY_Res_SDK_Check() != 0)
    {
        ZW_ERROR("SDK Not Init\n");
        return ERR_SDK_NOINIT;
    }

    HY_Res_GetSystemInfo(&pSysInfo);
#if defined(__PRODUCT_G43TA__)
    //G43TA需求，产测定制硬光敏无效
    if(nLightType != 1 && nLightType != 6)
#endif
    pSysInfo->m_LightBoardTypeID = nLightType;

    return 0;
}

int MP4Sys_SetAlarmLightStayTime (int nEnable, int nStayTime)
{
#ifdef NO_SET_AUDIO
    return 0;
#endif

    int nRet = 0;
    /* 接口加锁 */
    if (HY_Res_SDK_Lock() != 0)
    {
        ZW_ERROR("SDK Not Init\n");
        return ERR_SDK_NOINIT;
    }

    nRet = HY_Enc_SetAlarmLightStayTime(nEnable, nStayTime);

    HY_Res_SDK_UnLock();

    return nRet;
}

int MP4Sys_SetAlarmLightStart ()
{
#ifdef NO_SET_AUDIO
    return 0;
#endif

    /* 接口加锁 */
    if (HY_Res_SDK_Lock() != 0)
    {
        ZW_ERROR("SDK Not Init\n");
        return ERR_SDK_NOINIT;
    }
    HY_Enc_SetAlarmLightStart();
    HY_Res_SDK_UnLock();

    return HI_SDK_SUCESS;
}

int MP4Sys_SetSpeechLanType(eSpeechLanguage speech_language)
{
#ifdef NO_SET_AUDIO
    return 0;
#endif
      /* 接口加锁 */
    if (HY_Res_SDK_Lock() != 0) {
        ZW_ERROR("SDK Not Init\n");
        return ERR_SDK_NOINIT;
    }

    HY_Enc_SetAlarmPlayCfg(speech_language);
    HY_Res_SDK_UnLock();

    return 0;
}

int MP4Sys_SetSpeechContent(SpeechType_T speech)
{
    CChannelContext	*pChannelContext = NULL;
    SystemInfo_t *pSysInfo = NULL;

#ifdef NO_SET_AUDIO
    return 0;
#endif

    HY_Res_GetSystemInfo(&pSysInfo);

    /* 如果当前不支持音频报警，则需要排除音频报警的类型 */
    if (FALSE == pSysInfo->m_bSupportAudioAlarm)
    {
        if (IS_ALARM_SPEECH_TYPE(speech))
        {
            return 0;
        }
    }

    /* 如果当前不支持音频播报，则需要排除音频播报的类型 */
    if (FALSE == pSysInfo->m_bSupportAudioBroadcast)
    {
        if (!IS_ALARM_SPEECH_TYPE(speech))
        {
            return 0;
        }
    }

    /* 接口加锁 */
    if (HY_Res_SDK_Lock() != 0)
    {
        ZW_ERROR("SDK Not Init\n");
        return ERR_SDK_NOINIT;
    }

    HY_Enc_GetEncChInfo(&pChannelContext);

    if ((0 == pChannelContext->m_SysStarted) && (IS_ALARM_SPEECH_TYPE(speech)))
    {
        HY_Res_SDK_UnLock();
        return 0;
    }

    //声音报警配置只针对报警语音(如果新增了语音报警的类型那这里需要添加)
    if ((0 == pChannelContext->m_AlarmSpeechCfg.enable || 1 == pChannelContext->m_recvPauseAudioLightAlarm)
         && (IS_ALARM_SPEECH_TYPE(speech)))
    {
        HY_Res_SDK_UnLock();
        return 0;
    }

    HY_Enc_SetSpeechContent(speech);

    HY_Res_SDK_UnLock();
    return 0;
}

int MP4Sys_SetAlarmSpeechCfg(void *hChannelHandle, SpeechAlarmInfo_t *speechCfg)
{
    CChannelContext *pChannelContext = (CChannelContext *) hChannelHandle;

    ZW_INFO("enable[%d], SpeechType[%d], Volume[%d], Times[%d]\n", speechCfg->enable, \
            speechCfg->alarmSpeechType, speechCfg->broadCastVolume, speechCfg->broadCastTimes);

    //参数检查
    if ((0 != speechCfg->alarmSpeechType) && (1 != speechCfg->alarmSpeechType) && (2 != speechCfg->alarmSpeechType))
    {
        ZW_ERROR("Param err, alarmSpeechType=%d\n", speechCfg->alarmSpeechType);
        return ERR_SDK_INVALID_ARGUMENT;
    }
    if ((speechCfg->alarmOutDelay < 5) ||  (speechCfg->alarmOutDelay > 60))
    {
        ZW_ERROR("Param err, alarmOutDelay=%d\n", speechCfg->alarmOutDelay);
        return ERR_SDK_INVALID_ARGUMENT;
    }
    if ((speechCfg->broadCastVolume < 0) ||  (speechCfg->broadCastVolume > 100))
    {
        ZW_ERROR("Param err, alarmVolume=%d\n", speechCfg->broadCastVolume);
        return ERR_SDK_INVALID_ARGUMENT;
    }

    /* 接口加锁 */
    if (HY_Res_SDK_Lock() != 0)
    {
        ZW_ERROR("SDK Not Init\n");
        return ERR_SDK_NOINIT;
    }

    memcpy(&pChannelContext->m_AlarmSpeechCfg, speechCfg, sizeof(SpeechAlarmInfo_t));

    HY_Res_SDK_UnLock();
    return 0;
}

int MP4Sys_SetRemindSpeechCfg(void *hChannelHandle, int TypeIdx, SpeechRemindInfo_t *speechCfg)
{
    CChannelContext *pChannelContext = (CChannelContext *) hChannelHandle;

    ZW_INFO("Idx[%d] enable[%d], SpeechType[%d], Volume[%d], Times[%d]\n", TypeIdx, speechCfg->enable, \
            speechCfg->remindSpeechType, speechCfg->broadCastVolume, speechCfg->broadCastTimes);

    //参数检查
    if (TypeIdx < eSpeechType_Remind_1 || TypeIdx >= eSpeechTypeRemindMax)
    {
        ZW_ERROR("Param err, TypeIdx=%d\n", TypeIdx);
        return ERR_SDK_INVALID_ARGUMENT;
    }
    if (speechCfg->remindSpeechType > 7)
    {
        ZW_ERROR("Param err, alarmSpeechType=%d\n", speechCfg->remindSpeechType);
        return ERR_SDK_INVALID_ARGUMENT;
    }
    if (speechCfg->broadCastVolume > 100)
    {
        ZW_ERROR("Param err, alarmVolume=%d\n", speechCfg->broadCastVolume);
        return ERR_SDK_INVALID_ARGUMENT;
    }
    if (speechCfg->broadCastTimes > 10)
    {
        ZW_ERROR("Param err, broadCastTimes=%d\n", speechCfg->broadCastTimes);
        return ERR_SDK_INVALID_ARGUMENT;
    }

    /* 接口加锁 */
    if (HY_Res_SDK_Lock() != 0)
    {
        ZW_ERROR("SDK Not Init\n");
        return ERR_SDK_NOINIT;
    }

    memcpy(&pChannelContext->m_RemindSpeechCfg[TypeIdx], speechCfg, sizeof(SpeechRemindInfo_t));

    HY_Res_SDK_UnLock();
    return 0;
}

//-----------------------------------------------------------------------------
//	说明：		设置定时提醒提示音打断标记
//	输入参数：	iState    0:不打断     其他:立即打断
//	返回:		打断成功为 1, 失败其它；
//-----------------------------------------------------------------------------
int MP4Sys_SetRemindHangupState(int iState)
{
    /* 接口加锁 */
    if (HY_Res_SDK_Lock() != 0)
    {
        ZW_ERROR("SDK Not Init\n");
        return ERR_SDK_NOINIT;
    }

    int ret = 0;
    CChannelContext *pChContext = NULL;
    HY_Enc_GetEncChInfo(&pChContext);

    /* 定时提醒语音播放中才允许被外部打断 */
    if (IS_REMIND_SPEECH_TYPE(pChContext->m_SpeechNow) && pChContext->m_nSpeechPlayStatus != eSpeechPlayIdle)
    {
        pChContext->m_recvRemindHangUp = iState;
        ret = 1;
    }

    HY_Res_SDK_UnLock();
    return ret;
}

int MP4Sys_SetAudioCustomVoice(void *hChannelHandle, int customtype, char *pBuff, int len)
{
    CChannelContext *pChannelContext = (CChannelContext *) hChannelHandle;

    ZW_DEBUG("=============into MP4Sys_SetAudioCustomVoice==========\n");
#ifdef NO_SET_AUDIO
        return 0;
#endif

    /* 接口加锁 */
    if (HY_Res_SDK_Lock() != 0)
    {
        ZW_ERROR("SDK Not Init\n");
        return ERR_SDK_NOINIT;
    }

    //参数检查
    if ((NULL == pBuff) || (len <= 0))
    {
        ZW_ERROR("Param err, len=%d\n", len);
        HY_Res_SDK_UnLock();
        return ERR_SDK_INVALID_ARGUMENT;
    }

    HY_Enc_SetAudioCustomVoice(pChannelContext, customtype, pBuff, len);

    HY_Res_SDK_UnLock();
    return 0;
}

int MP4Sys_GetBitrateInfo(void *hChannelHandle, int dataArr[], int arrlen)
{
    int ret = 0;

    if (HY_Res_SDK_Check() != 0)
    {
        ZW_ERROR("SDK Not Init\n");
        return ERR_SDK_NOINIT;
    }

    if (arrlen < 0 || NULL == dataArr || NULL == hChannelHandle)
    {
        return ERR_INVALID_ARGUMENT;
    }

    ret = HY_Enc_GetBitrateInfo(hChannelHandle, dataArr, arrlen);

    return ret;
}

/* 设置按键状态获取回调函数 */
int MP4Sys_RegisterKeyStautsCallBack(KEY_STATUS_CALLBACK getKeyStateCallback)
{
    if (HY_Res_SDK_Check() != 0)
    {
        ZW_ERROR("SDK Not Init\n");
        return ERR_SDK_NOINIT;
    }

    if (NULL == getKeyStateCallback)
    {
        ZW_ERROR("INVALID param\n");
        return -1;
    }
    else
    {
        return HY_Enc_RegisterKeyStautsCallBack(getKeyStateCallback);
    }
}
#ifdef __SUPPORT_WIFI_FUNC__
int MP4Sys_SetLinkStatus(int status)
{
    int ret = 0;

    if (HY_Res_SDK_Check() != 0)
    {
        ZW_ERROR("SDK Not Init\n");
        return ERR_SDK_NOINIT;
    }

    HY_Res_SetLinkStatus(status);

    return ret;
}


int MP4Sys_SetWifiStatusLed(WIFI_STATUS_T wifiStatus)
{
    /* 接口加锁 */
    if (HY_Res_SDK_Lock() != 0)
    {
        ZW_ERROR("SDK Not Init\n");
        return ERR_SDK_NOINIT;
    }

    HY_Res_SetWifiStatusLed(wifiStatus);

    HY_Res_SDK_UnLock();

    return 0;
}
#endif

//二维码扫描功能使能
int MP4Sys_EnableQrScan(int bEnable)
{
    // ZW_DEBUG("=============into MP4Sys_EnableQrScan=========\n");

    /* 接口加锁 */
    if (HY_Res_SDK_Lock() != 0)
    {
        ZW_ERROR("SDK Not Init\n");
        return ERR_SDK_NOINIT;
    }

    if (HY_EnableQrScan(bEnable))
    {
        HY_Res_SDK_UnLock();
        return -1;
    }

    HY_Res_SDK_UnLock();

    return 0;
}

int MP4Sys_RegisterQrScanCallback(QR_SCAN_CALLBACK pCallback)
{
    SystemInfo_t *pSysInfo = NULL;

    ZW_INFO("--------into MP4Sys_RegisterQrScanCallback--------\n");

    /* 参数检查 */
    if (pCallback == NULL)
    {
        ZW_ERROR("INVALID_ARGUMENT\n");
        return ERR_INVALID_ARGUMENT;
    }
    /* 接口加锁 */
    if (HY_Res_SDK_Lock() != 0)
    {
        ZW_ERROR("SDK Not Init\n");
        return ERR_SDK_NOINIT;
    }

    HY_Res_GetSystemInfo(&pSysInfo);
    pSysInfo->m_pQrScanCallback = pCallback;

    HY_Res_SDK_UnLock();
    return HI_SDK_SUCESS;
}

int MP4Sys_RegisterMotoSavePosCallback(Moto_SavePos_CALLBACK pCallback)
{
    SystemInfo_t *pSysInfo = NULL;

    ZW_INFO("--------into MP4Sys_RegisterMotoSavePosCallback--------\n");

    /* 参数检查 */
    if (pCallback == NULL)
    {
        ZW_ERROR("INVALID_ARGUMENT\n");
        return ERR_INVALID_ARGUMENT;
    }
    /* 接口加锁 */
    if (HY_Res_SDK_Lock() != 0)
    {
        ZW_ERROR("SDK Not Init\n");
        return ERR_SDK_NOINIT;
    }

    HY_Res_GetSystemInfo(&pSysInfo);
    pSysInfo->m_pMotoSavePosCallback = pCallback;

    HY_Res_SDK_UnLock();
    return HI_SDK_SUCESS;
}


int MP4Sys_SetCurrNetCardType(XRS_NETCARD_TYPE_E status)
{
    ZW_DEBUG("--------into MP4Sys_SetCurrNetCardType---status %d-----\n", status);

    /* 接口加锁 */
    if (HY_Res_SDK_Lock() != 0)
    {
        ZW_ERROR("SDK Not Init\n");
        return ERR_SDK_NOINIT;
    }

    HY_Res_SetCurrNetCardType(status);

    HY_Res_SDK_UnLock();

    return 0;
}

//-----------------------------------------------------------------------------
//	说明：		设备外设控制
//	输入参数：AssistantCtrlInfo	pCtrlInfo	控制信息
//	返回:		成功为 0 , 其它为定义的错误号；
//-----------------------------------------------------------------------------
int MP4Sys_AssistantControl(AssistantCtrlInfo *pCtrlInfo)
{
    ZW_DEBUG("=============into MP4Sys_AssistantControl=========\n");

    /* 接口加锁 */
    if (HY_Res_SDK_Lock() != 0)
    {
        ZW_ERROR("SDK Not Init\n");
        return ERR_SDK_NOINIT;
    }

    if (HY_AssistantControl(pCtrlInfo))
    {
        HY_Res_SDK_UnLock();
        return -1;
    }

    HY_Res_SDK_UnLock();

    return 0;
}


int MP4Sys_GetUniqueID(char *uniqueid, int len)
{
    int nRet = 0;
    SystemInfo_t		*pSysInfo = NULL;

    ZW_DEBUG("--------into MP4Sys_GetUniqueID--------\n");

    /* 接口加锁 */
    if (HY_Res_SDK_Lock() != 0)
    {
        ZW_ERROR("%s : SDK Not Init\n", __FUNCTION__);
        return ERR_SDK_NOINIT;
    }

    if (NULL == uniqueid || len < UNIQUEID_BUF_LEN)
    {
        HY_Res_SDK_UnLock();
        return -1;
    }

    HY_Res_GetSystemInfo(&pSysInfo);
    memcpy(uniqueid, pSysInfo->m_sUniqueID, UNIQUEID_BUF_LEN);

    HY_Res_SDK_UnLock();

    ZW_INFO("[CHIP ID] %s\n", pSysInfo->m_sUniqueID);
    return nRet;
}

/**
 * @description: 设置系统启动状态标志
 * @param {int} Stat 启动状态标记
 * @return {int} 正常返回0，异常返回<0
 */
SINT32 MP4Sys_SetSysStartTag(int Stat)
{
    if (HY_Res_SDK_Check() != 0)
    {
        ZW_ERROR("SDK Not Init\n");
        return ERR_SDK_NOINIT;
    }

    return HY_SetSysStartTag(Stat);
}

/**
 * @description: 初始化KS HalMod模块
 * @return {int} 正常返回0，异常返回<0
 */
SINT32 MP4Sys_HalModInit()
{
    if (HY_Res_SDK_Check() != 0)
    {
        ZW_ERROR("SDK Not Init\n");
        return ERR_SDK_NOINIT;
    }

    return HY_HalModInit();
}

/**
 * @description: 反初始化KS HalMod
 * @return {SINT32} 正常返回0，异常返回<0
 */
SINT32 MP4Sys_HalModDeInit()
{
    return HY_HalModDeInit();
}

int MP4Sys_SetDormancyState(int iState)
{
    /* 接口加锁 */
    if (HY_Res_SDK_Lock() != 0)
    {
        ZW_ERROR("SDK Not Init\n");
        return ERR_SDK_NOINIT;
    }

    HY_Enc_SetDormancyState(iState);

    HY_Res_SDK_UnLock();

    return 0;
}

int MP4Sys_EncSetSlowFrameMode( int SlowFrameMode)
{
    CChannelContext *pChannelContext = NULL;
    SystemInfo_t    *pSysInfo;
    HY_Enc_GetEncChInfo(&pChannelContext);
    HY_Res_GetSystemInfo(&pSysInfo);

    // 接口加锁
    if (HY_Res_SDK_Lock() != 0)
    {
        ZW_DEBUG("%s : SDK Not Init\n", __FUNCTION__);
        return ERR_SDK_NOINIT;
    }

    if (0 == pSysInfo->m_bSupportCloudIPCSlowFrame)
    {
        //ZW_ERROR("Device don't support slow frame mode\n");
        HY_Res_SDK_UnLock();
        return  HI_SDK_SUCESS;
    }

    if (SlowFrameMode < SLOWFRAME_SCENE_FORCESLOW ||  SlowFrameMode > SLOWFRAME_SCENE_CLOSESTREAM)
    {
        ZW_ERROR("SlowFrameMode:%d is out of range\n", SlowFrameMode);
        HY_Res_SDK_UnLock();
        return ERR_INVALID_ARGUMENT ;
    }

    if (SlowFrameMode == SLOWFRAME_SCENE_OPENSTREAM)
    {
        pChannelContext->m_isRuRemotepreview = 1;
    }
    else if (SlowFrameMode == SLOWFRAME_SCENE_CLOSESTREAM)
    {
        pChannelContext->m_isRuRemotepreview = 0;
    }

    if (SlowFrameMode == SLOWFRAME_SCENE_TRIGGERALARM || pChannelContext->m_isRuRemotepreview == 1)
    {
        XTRACE_MUTEX_LOCK(&pChannelContext->m_SlowFrameLock);
        pChannelContext->m_iSlowFrameMode = SLOWFRAME_SCENE_NONE;
        XTRACE_MUTEX_UNLOCK(&pChannelContext->m_SlowFrameLock);
    }
    pChannelContext->m_iSlowFrameResetFlag = 1;

    //ZW_DEBUG("m_iSlowFrameMode = %d, m_isRuRemotepreview = %d\n", pChannelContext->m_iSlowFrameMode, pChannelContext->m_isRuRemotepreview);

    HY_Res_SDK_UnLock();
    //ZW_DEBUG("set SlowFrame Mode success !\n");
    return  HI_SDK_SUCESS;
}

int MP4Sys_SetLightOpenStatus(int Status)
{
    CChannelContext	*pChannelContext = NULL;
    HY_Enc_GetEncChInfo(&pChannelContext);

    // 接口加锁
    if (HY_Res_SDK_Lock() != 0)
    {
        ZW_DEBUG("%s : SDK Not Init\n", __FUNCTION__);
        return ERR_SDK_NOINIT;
    }

    Status = (0 != Status) ? 1 : 0;
    if (Status != pChannelContext->m_IrPin3Default)
    {
        pChannelContext->m_IrPin3Default = Status;
    }

    HY_Res_SDK_UnLock();
    ZW_DEBUG("set Light OpenStatus success !\n");

    return  HI_SDK_SUCESS;
}

int MP4Sys_SetProductMode(int nProductMode)
{
    int ret = 0;

    if (HY_Res_SDK_Check() != 0) {
        ZW_ERROR("SDK Not Init\n");
        return ERR_SDK_NOINIT;
    }
    ret = HY_Enc_SetProductMode(nProductMode);

    return ret;
}

//设置声音侦测配置
int MP4Sys_SetAudioDetectCfg(void *hChannelHandle, LpAudioDetectCfg pCfg)
{
    int nRet = 0;
    CChannelContext *pChannelContext = (CChannelContext*)hChannelHandle;

    ZW_DEBUG("--------into MP4Sys_SetAudioDetectCfg--------\n");

    /* 接口加锁 */
    if (HY_Res_SDK_Lock() != 0)
    {
        ZW_ERROR("SDK Not Init\n");
        return ERR_SDK_NOINIT;
    }

    /* 参数检查 */
    if (!HY_Enc_IsEncHandleOk(hChannelHandle))
    {
        ZW_ERROR("INVALID_HANDLE\n");
        HY_Res_SDK_UnLock();
        return ERR_INVALID_HANDLE;
    }
    if (NULL == pCfg)
    {
        ZW_ERROR("NULL == pCfg\n");
        HY_Res_SDK_UnLock();
        return ERR_INVALID_ARGUMENT;
    }

    if (pCfg->nEnable != pChannelContext->m_stAudioDetectCfg.nEnable ||
        pCfg->nSenstive != pChannelContext->m_stAudioDetectCfg.nSenstive)
    {
        HY_Enc_SetAudioDetectCfg(pChannelContext, pCfg);
        memcpy(&(pChannelContext->m_stAudioDetectCfg), pCfg, sizeof(AudioDetectCfg));
    }

    HY_Res_SDK_UnLock();
    return nRet;
}

//注册声音侦测报警回调
int MP4Sys_RegisterAudioDetectCallback(AudioDetect_CALLBACK pCallback, void *UserData)
{
    SystemInfo_t *pSysInfo = NULL;

    ZW_DEBUG("--------into MP4Sys_RegisterAudioDetectCallback--------\n");

    /* 参数检查 */
    if (NULL == pCallback)
    {
        ZW_ERROR("INVALID_ARGUMENT\n");
        return ERR_INVALID_ARGUMENT ;
    }

    /* 接口加锁 */
    if (HY_Res_SDK_Lock() != 0)
    {
        ZW_ERROR("SDK Not Init\n");
        return ERR_SDK_NOINIT;
    }

    HY_Res_GetSystemInfo(&pSysInfo);
    pthread_mutex_lock(&pSysInfo->m_muAudioDetectCallbackLock);
    pSysInfo->m_pAudioDetectCallback = pCallback;
    pSysInfo->m_pAudioDetectCallbackData = UserData;
    pthread_mutex_unlock(&pSysInfo->m_muAudioDetectCallbackLock);

    HY_Res_SDK_UnLock();
    return HI_SDK_SUCESS;
}

int MP4Sys_SwitchAecMode(int AecMode)
{
    /* 接口加锁 */
    if (HY_Res_SDK_Lock() != 0)
    {
        ZW_ERROR("SDK Not Init\n");
        return ERR_SDK_NOINIT;
    }
    HY_Enc_SwitchAecMode(AecMode);
    HY_Res_SDK_UnLock();

    return 0;
}

int MP4Sys_SysInitSensorCnt()
{
    return HY_Res_SysInitSensorCnt();
}

int MP4Sys_GetMaxSensorCnt()
{
    return HY_Res_GetMaxSensorCnt();
}

int MP4Sys_GetSensorConfCorrect(int CheckCnt)
{
    return HY_Res_GetSensorConfCorrect(CheckCnt);
}

//设置人形跟踪回调函数
int MP4Sys_HumanTrackerCallBack(HumanTracker_CALLBACK pFn_HumanTracker)
{
    SystemInfo_t *pSysInfo = NULL;
    HY_Res_GetSystemInfo(&pSysInfo);

    // 接口加锁
    if (HY_Res_SDK_Lock() != 0)
    {
        ZW_DEBUG("%s : SDK Not Init\n", __FUNCTION__);
        return ERR_SDK_NOINIT;
    }

    pSysInfo->m_pHumanTrackerCallback = pFn_HumanTracker;

    HY_Res_SDK_UnLock();

    return	HI_SDK_SUCESS;
}


int MP4Sys_GetWiFiInfo(void *wifiInfo)
{
    if (wifiInfo == NULL)
    {
        ZW_ERROR("input para error\n");
        return -1;
    }

    if (HY_Enc_GetWiFiInfo(wifiInfo))
    {
        ZW_ERROR("=============MP4Sys_GetWiFiInfo get wifi info error===========\n");
        return -1;
    }

    return 0;
}

int MP4Sys_P2PNotifySendDataFail(const char * pBuffer, UINT32 dwBufLen)
{
    /* 接口加锁 */
    if (HY_Res_SDK_Lock() != 0)
    {
        ZW_ERROR("SDK Not Init\n");
        return ERR_SDK_NOINIT;
    }

    HY_Res_P2PNotifySendDataFail(pBuffer, dwBufLen);

    HY_Res_SDK_UnLock();
    return 0;
}

//SD卡下电
int MP4Sys_DownSdPower()
{
    //关闭SD卡电源。
    ZW_PRINTF("HalMod_Set_SdPower\n");
    HalMod_Set_SdPower(0,0);
    return 0;
}

#ifdef __MODULE_4G_SUPPORT__
int  MP4Sys_SetModuleType(e4GModuleType moduletype)
{
    /* 接口加锁 */
    if (HY_Res_SDK_Lock() != 0)
    {
        return ERR_SDK_NOINIT;
    }
    SystemInfo_t *pSysInfo = NULL;
    HY_Res_GetSystemInfo(&pSysInfo);

    pSysInfo->m_4Gmoduletype = moduletype;
    ZW_DEBUG("pSysInfo->m_4Gmoduletype: %d \n", pSysInfo->m_4Gmoduletype);

    HY_Res_SDK_UnLock();

    return 0;

}

int MP4Sys_Set4GTelecomOperatorType(e4GTelecomOperatorType telecomOperatorType)
{
    /* 接口加锁 */
    if (HY_Res_SDK_Lock() != 0)
    {
        ZW_ERROR("SDK Not Init\n");
        return ERR_SDK_NOINIT;
    }

    SystemInfo_t *pSysInfo = NULL;
    HY_Res_GetSystemInfo(&pSysInfo);

    pSysInfo->m_4GTelecomOperatorType = telecomOperatorType;

    HY_Res_SDK_UnLock();
    return 0;
}

int  MP4Sys_GetModuleType(e4GModuleType *moduletype)
{
    int ret = -1;

    /* 接口加锁 */
    if (HY_Res_SDK_Lock() != 0)
    {
        return ERR_SDK_NOINIT;
    }

    if (moduletype == NULL)
    {
        HY_Res_SDK_UnLock();
        return ret;
    }

    SystemInfo_t *pSysInfo = NULL;
    HY_Res_GetSystemInfo(&pSysInfo);

    if (pSysInfo)
    {
        *moduletype = pSysInfo->m_4Gmoduletype;
    }
    else
    {
        ZW_ERROR("NULL Pointer \n");
        HY_Res_SDK_UnLock();
        return -1;
    }
    ZW_DEBUG("pSysInfo->m_4Gmoduletype: %d \n", *moduletype);
    HY_Res_SDK_UnLock();
    return 0;
}


//复位4G模组,先断开，再上电
int MP4Sys_4GModuleReset()
{
    //和wifi代码相同，但底层是WiFi引脚和4g引脚相反。
    mysystem("echo 0 > /sys/bus/usb/devices/1-1/authorized");
    HalMod_Set_WifiPower(0,0);
    sleep(5);
    HalMod_Set_WifiPower(0,1);
    sleep(5);
    mysystem("echo 1 > /sys/bus/usb/devices/1-1/authorized");
    sleep(1);
    return 0;
}


//获取是否支持绑定后播报语音
int MP4Sys_GetBroadCastEnable(int *nEnable)
{
    /* 接口加锁 */
    if (HY_Res_SDK_Lock() != 0)
    {
        ZW_ERROR("SDK Not Init\n");
        return ERR_SDK_NOINIT;
    }

    SystemInfo_t *pSysInfo = NULL;

    HY_Res_GetSystemInfo(&pSysInfo);
    *nEnable = pSysInfo->m_BroadCastEnable;

    HY_Res_SDK_UnLock();

    return 0;
}

//设置云卡支持模式至sdk
int MP4Sys_SetCloudCardmode(int CloudCardmode)
{
    /* 接口加锁 */
    if (HY_Res_SDK_Lock() != 0)
    {
        return ERR_SDK_NOINIT;
    }
    SystemInfo_t *pSysInfo = NULL;
    HY_Res_GetSystemInfo(&pSysInfo);

        pSysInfo->m_CloudCardmode = CloudCardmode;
    ZW_DEBUG("pSysInfo->CloudCardmode: %d \n", pSysInfo->m_CloudCardmode);

    HY_Res_SDK_UnLock();
    return 0;
}


#endif

int MP4Sys_GetRealKeyFrameInterval(int nChannel,int *nFrameRateInterval ,int *nKeyFrameInterval)
{
    if (HY_Res_SDK_Lock() != 0)
    {
        ZW_ERROR("SDK Not Init\n");
        return ERR_SDK_NOINIT;
    }

    if (NULL==nFrameRateInterval && NULL == nKeyFrameInterval )
    {
        ZW_ERROR("fail!, param error, nKeyFrameInterval=NULL\n");
        HY_Res_SDK_UnLock();
        return -1 ;
    }

    if(HY_Enc_GetRealKeyFrameInterval(nChannel,nFrameRateInterval, nKeyFrameInterval))
    {
        ZW_ERROR("fail!, HY_Enc_GetRealKeyFrameInterval error\n");
        HY_Res_SDK_UnLock();
        return -1 ;
    }

    HY_Res_SDK_UnLock();
    return 0;
}


#if defined(__DIGITAL_ZOOM_DRAG_SUPPORT__)
int MP4Sys_SetDragZoomRatio(double ratio,int eOpreator)
{
    int ret = 0;
    /* 接口加锁 */
    if (HY_Res_SDK_Lock() != 0) {
        ZW_ERROR("SDK Not Init\n");
        return ERR_SDK_NOINIT;
    }

    ret = Zoom_SetDragZoomRatio(ratio,eOpreator);

    HY_Res_SDK_UnLock();
    return ret;
}

int MP4Sys_GetDragZoomRatio(double *ratio)
{
    int ret = 0;
    /* 接口加锁 */
    if (HY_Res_SDK_Lock() != 0) {
        ZW_ERROR("SDK Not Init\n");
        return ERR_SDK_NOINIT;
    }

    ret = Zoom_GetDragZoomRatio(ratio);

    HY_Res_SDK_UnLock();
    return ret;
}
#if defined(__IS_SUPPORT_LONG_SHORT_FOCUS_ZOOM__)
int MP4Sys_GetEachFocusZoom(double *shortZoom, double *middleZoom, double *longZoom)
{
    int ret = 0;
    /* 接口加锁 */
    if (HY_Res_SDK_Lock() != 0) {
        ZW_ERROR("SDK Not Init\n");
        return ERR_SDK_NOINIT;
    }

    ret = Zoom_GetEachFocusZoom(shortZoom, middleZoom, longZoom);

    HY_Res_SDK_UnLock();
    return ret;
}
#endif

int MP4Sys_SetDragZoomCmd(DRAG_ZOOM_CMD nCmd,int eOpreator)
{
    int ret = 0;
    /* 接口加锁 */
    if (HY_Res_SDK_Lock() != 0)
    {
        ZW_ERROR("SDK Not Init\n");
        return ERR_SDK_NOINIT;
    }

    ret = Zoom_SetDragZoomCmd(nCmd,eOpreator);

    HY_Res_SDK_UnLock();
    return ret;
}

double MP4Sys_GetCurRatio()
{
	double CurRatio = 0;
	if (HY_Res_SDK_Lock() != 0)
	{
		ZW_ERROR("SDK Not Init\n");
		return ERR_SDK_NOINIT;
	}
	CurRatio = Zoom_GetCurRatio();
	
	HY_Res_SDK_UnLock();
	return CurRatio ;
}




#if defined __IS_SUPPORT_LONG_SHORT_FOCUS_ZOOM__
int MP4Sys_SetZoomLens(int nLens)
{
    int ret = 0;
    /* 接口加锁 */
    if (HY_Res_SDK_Lock() != 0)
    {
        ZW_ERROR("SDK Not Init\n");
        return ERR_SDK_NOINIT;
    }

    ret = Zoom_SetZoomLens(nLens);

    HY_Res_SDK_UnLock();
    return ret;
}

int  MP4Sys_GetCurrentSensor()
{
	int  CurrentSensor = 0;
	if (HY_Res_SDK_Lock() != 0)
	{
		ZW_ERROR("SDK Not Init\n");
		return ERR_SDK_NOINIT;
	}
	CurrentSensor = Zoom_GetCurrentSensor();
	
	HY_Res_SDK_UnLock();
	return CurrentSensor;
}

#endif

int MP4Sys_GetDragZoomChgStatus(int *status)
{
    int ret = 0;
    /* 接口加锁 */
    if (HY_Res_SDK_Lock() != 0)
    {
        ZW_ERROR("SDK Not Init\n");
        return ERR_SDK_NOINIT;
    }

    ret = Zoom_GetDragZoomChgStatus(status);

    HY_Res_SDK_UnLock();
    return ret;
}

int MP4Sys_SetDragZoomScaleMagnification(double ratio)
{
    int ret = 0;
    /* 接口加锁 */
    if (HY_Res_SDK_Lock() != 0) {
        ZW_ERROR("SDK Not Init\n");
        return ERR_SDK_NOINIT;
    }

    ret = Zoom_SetDragZoomScaleMagnification(ratio);

    HY_Res_SDK_UnLock();
    return ret;
}

int MP4Sys_GetDragZoomRange(double *min,double *max)
{
    int ret = 0;
    /* 接口加锁 */
    if (HY_Res_SDK_Lock() != 0) {
        ZW_ERROR("SDK Not Init\n");
        return ERR_SDK_NOINIT;
    }

    ret = Zoom_GetDragZoomRange(min,max);

    HY_Res_SDK_UnLock();
    return ret;
}

#endif

#if defined(__IS_SUPPORT_LCD_MANAGER__)
int MP4Sys_LCDRefreshFrame(char *pData, unsigned int DataSize)
{
    int ret = 0;
    /* 接口加锁 */
    if (HY_Res_SDK_Lock() != 0) {
        ZW_ERROR("SDK Not Init\n");
        return ERR_SDK_NOINIT;
    }

    ret = HY_Enc_LCDRefreshFrame(pData, DataSize);

    HY_Res_SDK_UnLock();
    return ret;
}

int MP4Sys_LCDSetBackLightLevel(int level)
{
    int ret = 0;
    /* 接口加锁 */
    if (HY_Res_SDK_Lock() != 0) {
        ZW_ERROR("SDK Not Init\n");
        return ERR_SDK_NOINIT;
    }

    ret = HY_Enc_LCDSetBackLightLevel(level);

    HY_Res_SDK_UnLock();
    return ret;
}

int MP4Sys_LCDCreateVdecChn(int nType)
{
    int ret = 0;
    /* 接口加锁 */
    if (HY_Res_SDK_Lock() != 0) {
        ZW_ERROR("SDK Not Init\n");
        return ERR_SDK_NOINIT;
    }

    ret = HY_Enc_LCDCreateVdecChn(nType);

    HY_Res_SDK_UnLock();
    return ret;
}

int MP4Sys_LCDSendStreamToChn(int nType, char *pData, unsigned int DataSize)
{
    int ret = 0;
    /* 接口加锁 */
    if (HY_Res_SDK_Lock() != 0) {
        ZW_ERROR("SDK Not Init\n");
        return ERR_SDK_NOINIT;
    }

    ret = HY_Enc_LCDSendStreamToChn(nType, pData, DataSize);

    HY_Res_SDK_UnLock();
    return ret;
}

int MP4Sys_LCDDestoryVdecChn(int nType)
{
    int ret = 0;
    /* 接口加锁 */
    if (HY_Res_SDK_Lock() != 0) {
        ZW_ERROR("SDK Not Init\n");
        return ERR_SDK_NOINIT;
    }

    ret = HY_Enc_LCDDestoryVdecChn(nType);

    HY_Res_SDK_UnLock();
    return ret;
}
#endif

/* 设置左右球/枪的sensor通道交换支持标志 */
int MP4Sys_SetSptSwapLRPTZChannel(int bEnable)
{
    /* 接口加锁 */
    if (HY_Res_SDK_Lock() != 0)
    {
        ZW_DEBUG("SDK Not Init\n");
        return ERR_SDK_NOINIT;
    }

    SystemInfo_t *pSysInfo = NULL;
    HY_Res_GetSystemInfo(&pSysInfo);

    pSysInfo->m_bSptSwapLRViewChannel = bEnable ? 1 : 0;

    HY_Res_SDK_UnLock();

    return 0;
}

/* 获取左右球/枪的sensor通道交换支持标志 */
int MP4Sys_GetSptSwapLRPTZChannel(int *pbEnable)
{
    /* 接口加锁 */
    if (HY_Res_SDK_Lock() != 0)
    {
        ZW_DEBUG("SDK Not Init\n");
        return ERR_SDK_NOINIT;
    }

    if (NULL == pbEnable)
    {
        ZW_ERROR("MP4Sys_GetSptSwapLRPTZChannel invalid param\n");
        HY_Res_SDK_UnLock();
        return ERR_SDK_INVALID_ARGUMENT;
    }

    SystemInfo_t *pSysInfo = NULL;
    HY_Res_GetSystemInfo(&pSysInfo);

    *pbEnable = pSysInfo->m_bSptSwapLRViewChannel;

    HY_Res_SDK_UnLock();

    return 0;
}


