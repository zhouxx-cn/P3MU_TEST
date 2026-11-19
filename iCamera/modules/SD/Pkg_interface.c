/*!
*\file   Pkg_interface.c 
*\brief  SD卡录像回放接口
*\author 樊聪
*\date	 2017年8月25日 15:44:52
*\version 0.2
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <utime.h>
#include <sys/time.h>

#include "disk.h"
#include "pkg_interface.h"
#include "global_macro.h"
#include "MemRecord.h"

#include "sd_ops.h"
#include "sys_banned.h"

#ifdef DEBUG
#define pkg_debug(x...) do {printf("[debug][%s %d %s]", \
	__FILE__,__LINE__,__FUNCTION__);printf(x);} while (0)
#define pkg_info(x...) do {printf("[info][%s %d %s]", \
	__FILE__,__LINE__,__FUNCTION__);printf(x);} while (0)
#define pkg_error(x...) do {printf("[error][%s %d %s]", \
	__FILE__,__LINE__,__FUNCTION__);printf(x);} while (0)
#else 
#define pkg_debug(x...) do {;} while (0)
#define pkg_info(x...) do {printf("[info][%s %d %s]", \
	__FILE__,__LINE__,__FUNCTION__);printf(x);} while (0)
#define pkg_error(x...) do {printf("[error][%s %d %s]", \
	__FILE__,__LINE__,__FUNCTION__);printf(x);} while (0)
#endif 

#define PKG_CHECK_PAUSE(pause_flag, ret) \
    do { if (pause_flag) return ret; } while (0)

/* 打印精确到毫秒的时间 */
#define _debug_timeprint() \
    do { \
        struct timeval tvTmp = {}; \
        struct tm stmTmp = {}; \
        char szDTime[64] = {}; \
        char szResult[96] = {}; \
        \
        gettimeofday (& tvTmp, NULL); \
        gmtime_r (& tvTmp.tv_sec, & stmTmp); \
        strftime (szDTime, sizeof(szDTime), "%F %T", &stmTmp); \
        snprintf (szResult, sizeof(szResult), "%s.%03d", szDTime, (int)(tvTmp.tv_usec / 1000)); \
        \
        printf ("timestamp[%s %d %s] %s\n", __FILE__,__LINE__,__FUNCTION__, szResult); \
    } while (0)

static void *Do_Thread_pop_stream(HANDLE hDataPopper);
int PkgSd_GetRecSegList(int handle, AviSegLinkList *plink, time_t tBeginTime, time_t tEndTime, int iRecType);
int StopRecordPlayback(int pop_index);

static pthread_mutex_t change_seg_type_lock = PTHREAD_MUTEX_INITIALIZER;
static unsigned int binited = 0;
static int s_pkg_pause = 0;

#ifdef __SUPPORT_SDCARDHOTSWAP__
sem_t        g_semDiskAddThread;
sem_t        g_semDiskDelThread;
static int   g_bCheckHotplug = 1;
static char  g_addDiskDev[128] = {0};
static char  g_delDiskDev[128] = {0};
#endif
/* 帧头信息 */
typedef struct tagH264DataPacketHeader1
{
    unsigned int        uDataPacketStartCode; //固定为0xB7010000
    unsigned int        uFrameNo;                //媒体数据包的序号(音视频独立编号)
    unsigned char       byFrameType;
    unsigned char       byLoadType;                //媒体数据包的信息,具体含义依赖于帧类型
    unsigned int        uShowTime;                //表示媒体数据包相对于上一帧的偏差时间(频率90Khz)
    unsigned long       u32RealTimeSec;
    unsigned long       u32RealTimeUSec;
    unsigned short      uReserved;
    unsigned int        uOverloadLen;            //表示媒体数据包的数据长度
}__attribute__((packed)) H264DATAPACKETHEADER1, *PH264DATAPACKETHEADER1;

typedef struct
{
    int start_pop_flag;
    int pop_pause;
    pthread_t thread_pop_frames;
    pthread_mutex_t m_mutex;            // 对变量处理的锁
    pthread_mutex_t m_mutexOperate;     // 对pop操作锁
    sem_t sem_stop_pop;
    int b_need_seek;                    // 是否需要进行seek定位
    unsigned int u_search_start_time;   // 当前search的开始时间
    unsigned int u_search_stop_time;    // 当前search的结束时间
    int handle;
    BOOL b_pop_iframe;                   // 是否只弹出I帧
}ZW_POP_Thread_dat;

/* 线程锁必须要初始化，否则会出现一些意想不到的错误(例如，死锁或锁不上) */
ZW_POP_Thread_dat thread_pop_config[2] = {
    {0, 0, 0, PTHREAD_MUTEX_INITIALIZER, PTHREAD_MUTEX_INITIALIZER, {{0}}, 1, 0, 0, 0, 0},
    {0, 0, 0, PTHREAD_MUTEX_INITIALIZER, PTHREAD_MUTEX_INITIALIZER, {{0}}, 1, 0, 0, 0, 0}
};

//虽然playback中的句柄已经设置为1，但是这里还是删除thread_pop_config[1]的使用
int set_handle(int handle)
{
    if (thread_pop_config[0].handle == 0)
    {
        thread_pop_config[0].handle = handle;
    }
    else
    {
        printf("what's happen!, thread_pop_config[0].handle=%#x, thread_pop_config[1].handle=%#x\n",
        thread_pop_config[0].handle, thread_pop_config[1].handle);
        return -1;
    }
    return 0;
}

int get_num(int handle)
{
    if(thread_pop_config[0].handle == handle)
    {
        return 0;
    }
    else
    {
        printf("thread_pop_config[0]=%#x,thread_pop_config[1]=%#x, inputHandle=%#x\n",
        thread_pop_config[0].handle, thread_pop_config[1].handle, handle);
        return -1;
    }
}

#ifdef __SUPPORT_SDCARDHOTSWAP__
static void *CheckAddHotplugThreadProcess(void* lparam)
{
    PRT_TID_NAME("CheckAddHotplugThread");

	while (g_bCheckHotplug)
	{	
		if (0 == sem_wait(&g_semDiskAddThread))
		{
			printf("Detect sd card add ...\n");
			Disk_Init();
			MemRecordInit();
			printf("Detect sd card add finish...\n");
		}
		else
		{
			printf("sem_wait error\n");
		}
		usleep(1000000);
	}

	return NULL;
}
static void *CheckDelHotplugThreadProcess(void* lparam)
{
    PRT_TID_NAME("CheckDelHotplugThread");

	while (g_bCheckHotplug)
	{
		if (0 == sem_wait(&g_semDiskDelThread))
		{
			printf("Detect sd card delete ...\n");
			MemRecordExit();
			Disk_Uinit();
			printf("Detect sd card delete finish...\n");
		}
		else
		{
			printf("sem_wait error\n");
		}
		usleep(1000000);
	}
	return NULL;
}
static int StartCheckHotplug(void)
{
	int nRet = -1;
	pthread_t thread_t = -1;
	pthread_attr_t new_attr;
	pthread_attr_init(&new_attr);
	pthread_attr_setdetachstate(&new_attr, PTHREAD_CREATE_DETACHED);
	sem_init(&g_semDiskAddThread, 0, 0);
	sem_init(&g_semDiskDelThread, 0, 0);
	nRet = pthread_create(&thread_t, &new_attr, CheckAddHotplugThreadProcess, NULL);
	if (nRet != 0)
	{
		printf("create CheckAddHotplugThreadProcess thread error.....\n");
		return -1;
	}
	nRet = pthread_create(&thread_t, &new_attr, CheckDelHotplugThreadProcess, NULL);
	if (nRet != 0)
	{
		printf("create CheckDelHotplugThreadProcess thread error.....\n");
		return -1;
	}
	return 0;
}
static void SD_AddCallBack(const char *dev)
{
	printf("detect insert sd card %s\n", dev);
	memset(g_addDiskDev, 0, sizeof (g_addDiskDev));
	strncpy(g_addDiskDev, dev, sizeof (g_addDiskDev) - 1);
}

static void SD_RemoveCallBack(const char *dev)
{
	printf("detect remove sd card %s\n", dev);
	memset(g_delDiskDev, 0, sizeof (g_delDiskDev));
	strncpy(g_delDiskDev, dev, sizeof (g_delDiskDev) - 1);

	//不管是正常拔出还是异常拔出，都软件热插拔一下
    //SDOPS_SoftHotplugTFCard();
}

PSS_API int API_CALL ZW_PkgSD_Hotplug()
{
    printf("ZW_PkgSD_Hotplug init start\n");

    StartCheckHotplug();
    
    SdOps_EDVR_HD_DeviceAddEvent(SD_AddCallBack);
    SdOps_EDVR_HD_DeviceRemoveEvent(SD_RemoveCallBack);
    SdOps_EDVR_HD_SetEventType((DiskActType) (DISKACT_SDCARD | DISKACT_USB));

    printf("ZW_PkgSD_Hotplug init stop\n");
    return 0;
}
#endif
PSS_API int API_CALL ZW_PkgSD_Init(ZW_PKG_PARAM_T *s)
{
    if(binited)
    {
        printf("ZW_PkgSD_Init Have inited\n");
        return 0;
    }

    int ret = 0;
    
    InitPopHandle();

    if (s != NULL)
    {    
        ret = Disk_InitV2(s->maxFrameBufSize);
        if (ret)
        {
            printf("Fail to Init Sd Module!\n");
            binited = 0;
            return -1;
        }

        MemRecordInitParam memParam;
        memParam.nRingBufSize = s->maxRingBufSize;
        memParam.nMaxFrameSize = s->maxFrameBufSize;
        ret = MemRecordInitV2(&memParam);
        if (ret)
        {          
            printf("Fail to Init MemRecord!\n");
            binited = 0;
            return -1;
        }

        binited = 1;
	#ifdef __SUPPORT_SDCARDHOTSWAP__
		ZW_PkgSD_Hotplug();
	#endif
    }

    return 0;
}


/**
 * SD卡模块功能暂停
 * 暂停后，所有功能不可用，并释放占用的内存
 */
PSS_API int API_CALL ZW_PkgSD_Pause()
{
    pkg_info ("\n");

    /* 封锁所有接口 */
    if (! binited)
    {
        pkg_error ("pause, not init\n");
        return -1;
    }

    if (s_pkg_pause)
    {
        pkg_error ("pause, already paused\n");
        return -1;
    }

    s_pkg_pause = 1;

    /* 暂停子模块 */
    
    pkg_info ("pause sdops\n");
    SDOPS_Disk_Pause ();

    pkg_info ("pause memrecord\n");
    MemRecord_Pause ();

    pkg_info ("pause finished\n");

    return 0;
}

/**
 * SD卡模块功能继续
 * 恢复后，重置暂停之前的操作，重新获取需要的内存
 */
PSS_API int API_CALL ZW_PkgSD_Resume()
{
    pkg_info ("\n");

    /* 封锁所有接口 */
    if (! binited)
    {
        pkg_error ("resume, not init\n");
        return -1;
    }

    if (! s_pkg_pause)
    {
        pkg_error ("resume, not paused\n");
        return -1;
    }

    /* 恢复子模块 */
    pkg_info ("resume sdops\n");
    SDOPS_Disk_Resume ();

    pkg_info ("resume memrecord\n");
    MemRecord_Resume ();

    pkg_info ("resume finished\n");

    s_pkg_pause = 0;

    return 0;
}

PSS_API  int API_CALL ZW_PkgSd_DiskPartFormat(const char *sDeviceSerial)
{
    PKG_CHECK_PAUSE (s_pkg_pause, -1);

    int ret = 0;

//  pthread_mutex_lock(&change_seg_type_lock);
    ret = FormatSdCard();
//  pthread_mutex_unlock(&change_seg_type_lock);

    if (ret)
    {
        binited = 0;
    }
    else
    {
        binited = 1;
    }

    return ret;
}

PSS_API int API_CALL ZW_PkgSd_GetSdFormatStatus(void)
{
    PKG_CHECK_PAUSE (s_pkg_pause, -1);

    return (int)GetSdCardFormatStatus();
}

PSS_API int API_CALL ZW_PkgSd_GetDiskWriteStatus(int *writeStatus)
{
    PKG_CHECK_PAUSE (s_pkg_pause, -1);

    *writeStatus = GetSdCardWriteStatus();
    return 0;
}

PSS_API int API_CALL ZW_PkgSd_SetDiskGroupCycleMode( int iCycleMode)
{
return 0;

#if 0

    if(!binited )
    {
        //printf("ZW_PkgSD_Init not inited \n");
        return -1;
    }
    SetCircleMode(iCycleMode);
    return 0;
#endif

}

// 0:无录像 1:报警录像 2:普通录像 3:无录像 4:移动侦测录像
// 换录像类型，会自动处理换片段
PSS_API int API_CALL ZW_Pkg_SetSegType(int  seg_rec_type)
{
    if(!binited)
    {
        return -1;
    }

    PKG_CHECK_PAUSE (s_pkg_pause, -1);

    pthread_mutex_lock(&change_seg_type_lock);
	SetTrigRecordType(seg_rec_type);
    pthread_mutex_unlock(&change_seg_type_lock);

    return 0;
}

/* 触发录像完毕之后，发送消息的回调函数 */
PSS_API int API_CALL ZW_Pkg_SetTrigRecordOverCallBack(int (*CallBack)(unsigned char *))
{
    if(!binited)
    {
        return -1;
    }

    PKG_CHECK_PAUSE (s_pkg_pause, -1);

    SetTrigRecordOverCallBack(CallBack);

    return 0;
}

/* 开启录像 */
PSS_API int API_CALL ZW_Pkg_StartRecord(int record_type, int bH265)
{
    if(!binited)
    {
        return -1;
    }

    PKG_CHECK_PAUSE (s_pkg_pause, -1);

    pthread_mutex_lock(&change_seg_type_lock);
	StartRecord(record_type, bH265);
    pthread_mutex_unlock(&change_seg_type_lock);

    return 0;
}

/* 设置预录时间(单位: 秒) */
PSS_API int API_CALL ZW_Pkg_SetPrevTimeSec(unsigned int prev_time_sec)
{
    if(!binited)
    {
        return -1;
    }

    PKG_CHECK_PAUSE (s_pkg_pause, -1);

    pthread_mutex_lock(&change_seg_type_lock);
	SetPrevTimeSec(prev_time_sec);
    pthread_mutex_unlock(&change_seg_type_lock);

    return 0;
}

/* 获取预录时间(单位: 秒) */
PSS_API unsigned int API_CALL ZW_Pkg_GetPrevTimeSec(void)
{
    if(!binited)
    {
        return -1;
    }

    PKG_CHECK_PAUSE (s_pkg_pause, -1);

	return GetPrevTimeSec();
}

/* 设置录像延迟时间（单位：秒） */
PSS_API int API_CALL ZW_Pkg_SetRecordDelayTime (unsigned int nDelaySecs)
{
    if(!binited)
    {
        return -1;
    }

    PKG_CHECK_PAUSE (s_pkg_pause, -1);

    pthread_mutex_lock(&change_seg_type_lock);
    int ret = MEMRECORD_SetDelayTime (nDelaySecs);
    pthread_mutex_unlock(&change_seg_type_lock);

    return ret ;
}

/* 获取录像延迟时间（单位：秒） */
PSS_API int API_CALL ZW_Pkg_GetRecordDelayTime (unsigned int *pnDelaySecs)
{
    if(!binited)
    {
        return -1;
    }

    PKG_CHECK_PAUSE (s_pkg_pause, -1);

    int ret = MEMRECORD_GetDelayTime (pnDelaySecs);
    return ret ;
}

/*!
 * \brief 将数据写入SD卡
 * \param wFrameType 数据帧类型  *
 * \param pFirstBuf 写入的一帧数据
 * \param iFirstLength 写入的一帧数据长度
 * \return 成功返回0，失败返回小于0的错误码
 */
PSS_API int API_CALL ZW_PkgSd_DataInput(const char *pFirstBuf, int iFirstLength)
{
    if(!binited)
    {
        return 0;
    }

    PKG_CHECK_PAUSE (s_pkg_pause, -1);

	int ret = RawDataWirteProcess(iFirstLength, pFirstBuf);
    
    return ret;
}

/* 传入的图片名字组成pic_name: 时间戳_标号，如:1532477851_01需要改为1532477851 */
PSS_API int API_CALL ZW_PkgSd_UploadPictureToSdCard(const char *pic_name, unsigned int trig_type)
{
    if(!binited)
    {
        return 0;
    }

    PKG_CHECK_PAUSE (s_pkg_pause, -1);

//  pthread_mutex_lock(&change_seg_type_lock);
	int ret =  UploadPictureToSdCard(pic_name, trig_type);
//  pthread_mutex_unlock(&change_seg_type_lock);

    return ret;
}

PSS_API int API_CALL ZW_PkgSd_StopRecordAndUinitMemRecord(void)
{
    if(!binited)
    {
        return -1;
    }

    PKG_CHECK_PAUSE (s_pkg_pause, -1);

    MemRecordExit();
	return 0;
}

PSS_API int API_CALL ZW_PkgSd_UnInitSdCard(void)
{
    int ret = 0;
    
    if(0 != binited)
    {
        binited = 0;
		return -1;
    }

    PKG_CHECK_PAUSE (s_pkg_pause, -1);

    ret = Disk_Uinit();
	if (ret < 0)
	{
        printf("移除SD卡失败, ret = %d\n", ret);
		return -1;
	}
	return 0;
}


/*@breif ZW_PkgSd_SetCurrentAviNumPerPage 设置每页显示多少个AVI文件
 *@param [IN]  avi_num_per_page 每页显示的AVI文件个数
 *@return 成功返回0
 *@       失败返回-1
 *@note  每页显示的AVI文件个数不能超过当前有效的AVI个数g_read_curr_avi_num，
 *@      每页显示的AVI文件个数不能超过最大能显示的AVI个数MAX_AVI_NUM_ONE_PAGE
 */
PSS_API int API_CALL ZW_PkgSd_SetCurrentAviNumPerPage(int avi_num_per_page)
{
    if(!binited)
    {
        return -1;
    }

    PKG_CHECK_PAUSE (s_pkg_pause, -1);

    return SetCurrentAviNumPerPage(avi_num_per_page);
}

/*@breif ZW_PkgSd_GetCurrentAviTotalNum 获取当前AVI总个数
 *@return 成功返回当前总的AVI文件个数
 *@      失败返回-1
 */
PSS_API int API_CALL ZW_PkgSd_GetCurrentAviTotalNum(void)
{
    if(!binited)
    {
        return -1;
    }

    PKG_CHECK_PAUSE (s_pkg_pause, -1);

    return GetCurrentAviTotalNum();
}

/*@breif ZW_PkgSd_SetSwitchPageIsChoose 设置当前选中的是哪一个页面和当前录像路径类型
 *@param [IN]  choose_page 选中的页面(从1开始)
 *@param [IN]  path_type 当前录像路径类型 0: http路径  1:绝对路径
 *@return 成功返回0
 *@      失败返回-1
 */
PSS_API int API_CALL ZW_PkgSd_SetSwitchPageIsChoose(int choose_page, int path_type)
{
    if(!binited)
    {
        return -1;
    }

    return SetSwitchPageIsChoose(choose_page, path_type);
}

/*@breif ZW_PkgSd_UpdateAviListByTime 通过给定的时间戳获取符合时间段的文件总数
 *@param [IN]  start_time 要搜索的起始时间
 *@param [IN]  stop_time 要搜索的结束时间(为0表示搜索全部文件)
 *@param [OUT] p_search_avi_count 符合当前时间段的文件总数
 *@return 成功返回0
 *@      失败返回-1
 */
PSS_API int API_CALL ZW_PkgSd_UpdateAviListByTime(unsigned int start_time,
                                                           unsigned int stop_time,
                                                           int *p_search_avi_count)
{
    if(!binited)
    {
        return -1;
    }

    PKG_CHECK_PAUSE (s_pkg_pause, -1);
    /* 通过给定的时间戳更新AVI文件列表 */
    return UpdateAviListByTime(start_time, stop_time, p_search_avi_count);
}

/*@breif ZW_PkgSd_GetCurrentAviListInfo 通过给定的时间戳获取对应的AVI文件列表
 *@param [IN]  read_avi_page 需要获取第几页的AVI信息(从1开始)
 *@param [OUT] p_avi_list 获取到的AVI文件列表
 *@param [OUT] p_avi_num_curr_page 当前页面有多少个AVI文件
 *@return 成功返回0
 *@      失败返回-1
 *@note  需要先调用SetCurrentAviNumPerPage设置当前用户需要设置每页显示多少个AVI文件
 */
PSS_API int API_CALL ZW_PkgSd_GetCurrentAviListInfo(                int read_avi_page, void **p_avi_list, int *p_avi_num_curr_page)
{
    if(!binited)
    {
        return -1;
    }

    PKG_CHECK_PAUSE (s_pkg_pause, -1);

    /* 调用以下接口获取AVI文件列表 */
    int ret = GetAviListInfoByTimeStamp(  read_avi_page, p_avi_list, p_avi_num_curr_page);
    return ret;
}

/*@breif ZW_PkgSd_SetCurrentPicNumPerPage 设置每页显示多少个图片文件
 *@param [IN]  pic_num_per_page 每页显示的图片文件个数
 *@return 成功返回0
 *@       失败返回-1
 *@note  每页显示的图片文件个数不能超过当前有效的图片个数g_read_curr_pic_num，
 *@      每页显示的图片文件个数不能超过最大能显示的图片个数MAX_PIC_NUM_ONE_PAGE
 */
PSS_API int API_CALL ZW_PkgSd_SetCurrentPicNumPerPage(int pic_num_per_page)
{
    if(!binited)
    {
        return -1;
    }

    PKG_CHECK_PAUSE (s_pkg_pause, -1);

    return SetCurrentPicNumPerPage(pic_num_per_page);
}

/*@breif ZW_PkgSd_GetCurrentPicTotalNum 获取当前图片总个数
 *@return 成功返回当前总的图片文件个数
 *@       失败返回-1
 */
PSS_API int API_CALL ZW_PkgSd_GetCurrentPicTotalNum(void)
{
    if(!binited)
    {
        return -1;
    }

    PKG_CHECK_PAUSE (s_pkg_pause, -1);

    return GetCurrentPicTotalNum();
}

/*@breif ZW_PkgSd_UpdatePicListByTime 通过给定的时间戳获取符合时间段的文件总数
 *@param [IN]  start_time 要搜索的起始时间
 *@param [IN]  stop_time 要搜索的结束时间(为0表示搜索全部文件)
 *@param [OUT] p_search_pic_count 符合当前时间段的文件总数
 *@return 成功返回0
 *@      失败返回-1
 */
PSS_API int API_CALL ZW_PkgSd_UpdatePicListByTime(unsigned int start_time,
                                                           unsigned int stop_time,
                                                           int *p_search_pic_count)
{
    if(!binited)
    {
        return -1;
    }

    PKG_CHECK_PAUSE (s_pkg_pause, -1);
    /* 通过给定的时间戳更新PIC文件列表 */
    return UpdatePicListByTime(start_time, stop_time, p_search_pic_count);
}

/*@breif ZW_PkgSd_GetCurrentPicListInfo 通过给定的时间戳获取对应的图片文件列表
 *@param [IN]  read_pic_page 需要获取第几页的图片信息(从1开始)
 *@param [OUT] p_pic_list 获取到的图片文件列表
 *@param [OUT] p_pic_num_curr_page 当前页面有多少个图片文件 
 *@return 成功返回0
 *@      失败返回-1
 *@note  需要先调用GetPicGlobalHeadInfoFromGlobalFile将需要的数据放到全局结构体里面
 *@      需要先调用SetCurrentPicNumPerPage设置当前用户需要设置每页显示多少个图片文件
 */
PSS_API int API_CALL ZW_PkgSd_GetCurrentPicListInfo(                int read_pic_page, void **p_pic_list, int *p_pic_num_curr_page)
{
    if(!binited)
    {
        return -1;
    }

    PKG_CHECK_PAUSE (s_pkg_pause, -1);

    int ret = -1;

    /* 调用以下接口获取PIC文件列表 */
    ret = GetPictureListInfoByTimeStamp(   read_pic_page,  p_pic_list,  p_pic_num_curr_page);
    return ret;
}

/* 获取磁盘资源占用情况(单位: MB) */
PSS_API int API_CALL ZW_PkgSd_GetDiskResourceSize(int *pRecordMB, int *pPictureMB)
{
    if(!binited)
    {
        return -1;
    }

    PKG_CHECK_PAUSE (s_pkg_pause, -1);

    return GetDiskResourceSize(pRecordMB, pPictureMB);
}

/* 获取录像存放的比例(范围0~100) */
PSS_API int API_CALL ZW_PkgSd_GetVideoUsedSpaceRate(void)
{
    if(!binited)
    {
        return -1;
    }

    PKG_CHECK_PAUSE (s_pkg_pause, -1);

    return GetVideoUsedSpaceRate();
}

/* 设置录像存放的比例(范围0~100) */
PSS_API int API_CALL ZW_PkgSd_SetVideoUsedSpaceRate(int nVideoSpace)
{
    if(!binited)
    {
        return -1;
    }

    PKG_CHECK_PAUSE (s_pkg_pause, -1);

    return SetVideoUsedSpaceRate(nVideoSpace);
}

/* 设置录像尺寸 */
PSS_API int API_CALL ZW_PkgSd_SetRecordSize(int width, int height, int bH265)
{
    if(!binited)
    {
        return -1;
    }

    PKG_CHECK_PAUSE (s_pkg_pause, -1);

    return SetRecordSize(width, height, bH265);
}

/*@breif ZW_PkgSd_GetFileReadWriteStatus 获取当前文件的读写状态
 *@param [IN]  file_name 需要操作的文件名(绝对路径)
 *@param [OUT] read_write_status 
 *@            0:  当前文件空闲
 *@            1:  当前文件正在读
 *@            2:  当前文件正在写
 *@param [OUT] file_size 文件大小(单位: 字节) 
 *@return 返回0表示成功
 *@      返回-1表示失败
 */
PSS_API int API_CALL ZW_PkgSd_GetFileReadWriteStatus(const char *file_name,
                                                               unsigned int *read_write_status,
                                                               unsigned int *file_size)
{
    if(!binited)
    {
        return -1;
    }

    PKG_CHECK_PAUSE (s_pkg_pause, -1);

    return GetFileReadWriteStatus(file_name, read_write_status, file_size);
}

/*@breif ZW_PkgSd_SDOPS_GetRecordDataSize 获取指定时间段的录像数据大小
 *@param [IN] start_time 开始时间
 *@param [IN] stop_time  结束时间
 *@param [IN] record_type 筛选的录像类型(当前无效)
 *@param [OUT] p_size 获取到的帧数据大小
 *@return 返回  0 表示获取成功
 *@       返回 -1 表示获取失败
 *@note   获取到的是数据帧大小，包括帧头
 */
PSS_API int API_CALL ZW_PkgSd_SDOPS_GetRecordDataSize(unsigned int start_time, 
                                                                       unsigned int stop_time, 
                                                                       int record_type, 
                                                                       unsigned int * p_size)
{
    if(!binited)
    {
        return -1;
    }

    PKG_CHECK_PAUSE (s_pkg_pause, -1);

    return SDOPS_GetRecordDataSize(start_time, stop_time, record_type, p_size);
}

/* 获取当前回放的录像信息 */
PSS_API int ZW_PkgSd_SDOPS_GetRecordInfo(int *pCurrentRecordStartTime)
{
    if(!binited)
    {
        return -1;
    }

    return SDOPS_GetRecordInfo(pCurrentRecordStartTime);
}

/* 获取文件对应的文件信息 */
PSS_API int ZW_PkgSd_SDOPS_GetRecordFileInfoByFileName(const char* pFileName, void* pFileInfo)
{
    if(!binited)
    {
        return -1;
    }

    return SDOPS_GetRecordFileInfoByFileName(pFileName, pFileInfo);
}

/* 设置获取时间偏移回调 */
PSS_API int API_CALL ZW_PkgSd_SetGetTimeOffsetCallBack(int (*CallBack)(int *, int *, int *))
{
    return SetGetTimeOffsetCallBack(CallBack);
}

/*@breif ZW_PkgSd_GetDiskInfo 获取磁盘信息
 *@param [OUT] sutats        SD卡状态
 *@param [OUT] total_space;     SD(分区)总大小(单位: MB)
 *@param [OUT] available_space  SD(分区)可用空间(单位: MB)
 *@return 返回0表示成功
 *@      返回-1表示失败
 */
PSS_API int API_CALL ZW_PkgSd_GetDiskInfo(int *sutats, unsigned long *total_space, unsigned long *available_space)
{
    PKG_CHECK_PAUSE (s_pkg_pause, -1);

    stuSdCardStatus sd_status;

    if (GetSdCardInfo(&sd_status))
    {
        return -1;
    }

    *sutats = (int)sd_status.eSdcardStatus;
    *total_space = sd_status.dwTotalSpace;
    *available_space = sd_status.dwAvailableSpace;

    return 0;
}

PSS_API void API_CALL ZW_PkgSd_UpdateInfo(void)
{
//    UpdateGlobalAviFileSet();
}

/*
 *\brief 查询某天是否存在录像
 *\param ifExist为1表示有数据，0表示无数据
 */
PSS_API int API_CALL ZW_PkgSd_IsFileExist(int iYear, int iMonth, int iDay, int *ifExist)
{
    /* 首先必须先清空，防止出现无SD卡也显示有录像的情况 */
    *ifExist = 0;   

    if (NULL == ifExist)
    {
        printf("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    if(!binited)
    {
        return 0;
    }

    PKG_CHECK_PAUSE (s_pkg_pause, -1);

    if (SearchRecordByTime(iYear, iMonth, iDay, ifExist))
    {
        printf("Called SearchRecordByTime fail\n");
        return -1;
    }

    return 0;
}

/*!
 * \brief 获得指定通道在指定时间段内指定录像类型的录像片断
 * \param tBeginTime 起始时间
 * \param tEndTime 结束时间
 * \param iRecType 录像类型(如果0xff 表明不需要过滤录像类型),0x1表示普通录像0x2移动录像 0x4报警录像
 * \param pRecSegCount 保存返回获取片断的个数
 * \return 成功返回0，失败返回小于0
 */
PSS_API int API_CALL ZW_PkgSd_GetRecSegList(HANDLE hDataPopper,
                                                        time_t tBeginTime, 
                                                        time_t tEndTime, 
                                                        int iRecType,
                                                        PSS_REGSEG_T ** pRecSegHead, 
                                                        int *pRecSegCount)
{
    if(!binited)
    {
        *pRecSegCount = 0;
        return 0;
    }

    PKG_CHECK_PAUSE (s_pkg_pause, -1);

    PSS_REGSEG_T  *pseglink = NULL;
    PSS_REGSEG_T  *ptmpseg  = NULL;
    AviSegLinkList *plink    = NULL;
    AviSegLinkList *plinktmp = NULL;

    int i = 0;
    int num   = 0;
    num = get_num((int)hDataPopper);
    if(num < 0)
    {
        return -1;
    }

    plink = InitNewAviSegLinklist(1);
    plinktmp = plink;

    printf("[%s-%s-%d] num = %d iRecType = %#x\n", __FILE__, __FUNCTION__, __LINE__, num, iRecType);

    if(NULL == plink)
    {
        *pRecSegCount = 0;
        return 0;
    }
    
    *pRecSegCount = ReadAviSegLinkByTime(num, tBeginTime, tEndTime, iRecType, &plinktmp);
    printf("num=%d, *pRecSegCount = %d\n", num, *pRecSegCount);

    if( *pRecSegCount <= 0)
    {
        FreeAviSegLinkLists(1, plink);
        plink = NULL;
        return 0;
    }

    pseglink = (PSS_REGSEG_T *)calloc(1, sizeof(PSS_REGSEG_T));
    if(NULL == pseglink)
    {
        return 0;
    }
    
    ptmpseg = pseglink;
    
    printf("ptmpseg = %p\n", ptmpseg);
    
    plinktmp = plink->next_link;
    for(i=0; i<*pRecSegCount; i++)
    {

        pseglink->iRecType   = plinktmp->record_type;
        pseglink->tBeginTime = plinktmp->start_time;
        pseglink->tEndTime   = plinktmp->stop_time;
        //printf("-------count=%d,RecType=%d,bt=%d et=%d\n",i, pseglink->iRecType,pseglink->tBeginTime,pseglink->tEndTime);
        if(i == (*pRecSegCount)-1)
        {
            pseglink->ptNext = NULL;
        }
        else
        {
            pseglink->ptNext = malloc(sizeof(PSS_REGSEG_T));
        }
        pseglink = (PSS_REGSEG_T*)pseglink->ptNext;

        plinktmp = plinktmp->next_link;  

    }
    *pRecSegHead = ptmpseg;

    return (int)plink;
}

/*!
 * \brief 释放录像片断信息列表所占内存
 * \return 成功返回0，失败返回小于0的错误码。
 */
PSS_API int API_CALL ZW_PkgSd_ReleaseRecSegList(int linkhandle, PSS_REGSEG_T * pRecSegHead)
{
    if(!binited)
    {
        return -1;
    }

    PKG_CHECK_PAUSE (s_pkg_pause, -1);

    PSS_REGSEG_T   *ptr   = NULL;
    AviSegLinkList *plink = NULL;
    
    plink = (AviSegLinkList *)linkhandle;

    while(pRecSegHead != NULL)
    {
        ptr = (PSS_REGSEG_T*)pRecSegHead->ptNext;
        free(pRecSegHead);
        pRecSegHead = ptr;
    }
    
    if (plink)
    {
        FreeAviSegLinkLists(1, plink);
        plink = NULL;
    }

    return 0;
}

PSS_API int API_CALL ZW_PkgSd_GetDataSize(time_t tBeginTime, 
                                                    time_t tEndTime, 
                                                    int iRecType,
                                                    unsigned long long *size)
{
    if(!binited)
    {
        return 0;
    }

    if(NULL == size)
    {
        printf("size == NULL!\n");
        return -1;
    }

    PKG_CHECK_PAUSE (s_pkg_pause, -1);

    int ret = 0;
    struct timeval ss_start={0,0},ss_end={0,0},ss_diff={0,0};

    printsegtime(tBeginTime);
    printsegtime(tEndTime);

    printf("type = %#x\n", iRecType);

    _debug_timeprint();

    /* SDOPS_GetRecordDataSize 仅支持 unsigned int，临时变量中转一次 */
    unsigned int tmpSize = 0;
    gettimeofday(&ss_start, NULL);
    ret = SDOPS_GetRecordDataSize(tBeginTime, tEndTime, iRecType, &tmpSize);
    if(0 != ret)
    {
        printf("SDOPS_GetRecordDataSize fail!\n");
        *size = 0;
        return -1;
    }
    *size = (unsigned long long)tmpSize;
    gettimeofday(&ss_end, NULL);

    timersub(&ss_end, &ss_start, &ss_diff);
    unsigned int ss_total = (ss_diff.tv_sec*1000*1000 + ss_diff.tv_usec);

    printf("get data size %lld bytes, spent %u ms\n", *size, ss_total/1000);

    _debug_timeprint();

    return ret;
}

POP_SS POP_SSHandle[4];

/*!
 * \brief 创建数据弹出器

 * \param tStartSeekTime 初始弹出数据的时间
 * \param tEndSeekTime 弹出数据的结束时间。当此时间为0时，数据进行动态弹出。如果结束时间是一个未来时间，则到达当前时间时，通过回调接口告之该事件。
 * \param cbReplayCallback 设定数据弹出的回调函数接口，弹出的数据按弹出器句柄、通道、帧类型、时间簇、事件类型
 *       （跳到新的时间片/查询结束/当前数据查询完毕等）等参数形式送回放线程。此回调与DSP上送编码数据接口相当
 * \param pContext 上下文，上层软件可以传入此参数，在弹出回调中会给出该参数
 * \return 成功返回创建的数据弹出器的句柄,失败返回NULL。
 */
PSS_API HANDLE API_CALL ZW_PkgSd_DataPopCreate(time_t tStartSeekTime, 
                                                           time_t tEndSeekTime, 
                                                           int recType,
                                                           ZW_REPLAY_DATAPOP_CALLBACK cbReplayCallback,
                                                           void *pContext) 
{
    if(!binited)
    {
        return NULL;
    }

    /* 此函数返回 HANDLE，PKG_CHECK_PAUSE 宏不能直接用 -1 */
    if (s_pkg_pause)
    {
        return NULL;
    }

    void *handle  = (void *)NULL;
    POP_SS *pophandle     = NULL;
    POP_SS *tmppophandle  = NULL;
    AviSegLinkList *plink = NULL;

    int ret = 0;
    int curr_handle = 0;

    printf("iRecType = %d\n", recType);
    printsegtime(tStartSeekTime);
    printsegtime(tEndSeekTime);

    pophandle = (POP_SS *)POP_SSHandle;

    handle = CreateOnePopHandle(&curr_handle);

    printf("curr_handle = %d, handle = %p\n", curr_handle, handle);
    if (NULL == handle)
    {
        printf("curr_handle = %d, more than 1 pop handle error\n", curr_handle);
        return NULL;
    }

    plink = InitNewAviSegLinklist(0);
    if(NULL == plink)
    {
        return NULL;
    }
    tmppophandle =(POP_SS *) &pophandle[curr_handle];
    tmppophandle->popcallback = cbReplayCallback;
    tmppophandle->handle = (int)handle;
    tmppophandle->tSeekBeginTime = tStartSeekTime;
    tmppophandle->tSeekEndTime = tEndSeekTime;
    tmppophandle->plink = (int*)plink;
    tmppophandle->recmask = recType;//0xff;
    tmppophandle->bpopstop = 0;
    ((PopHandle *)(tmppophandle->handle))->read_link_handle  =(void *)plink;
    pophandle->bkeyframe = 0;
    printf("tmppophandle = %p curr_handle = %d\n", tmppophandle, curr_handle);
    set_handle((int)tmppophandle);

    tmppophandle->pContext = pContext;
    ret = PkgSd_GetRecSegList(curr_handle, 
                              (AviSegLinkList *)tmppophandle->plink, 
                              tmppophandle->tSeekBeginTime,
                              tmppophandle->tSeekEndTime,
                              tmppophandle->recmask);
    if(ret < 0)
    {
        if(tmppophandle->plink)
        {
            FreeAviSegLinkLists(0, (AviSegLinkList *)tmppophandle->plink);
            tmppophandle->plink = NULL;
        }

        if(tmppophandle->handle)
        {
            FreeOnePopHandle(tmppophandle->handle);
        }

        if(tmppophandle->handle)
        {
            DestroyOnePopHandle(tmppophandle->handle);            
            tmppophandle->handle = 0;
        }

        if(thread_pop_config[0].handle == (int)tmppophandle)
        {
            memset(&thread_pop_config[0], 0, sizeof(ZW_POP_Thread_dat));
        }
        else
        {
            memset(&thread_pop_config[1], 0, sizeof(ZW_POP_Thread_dat));
        }
        
        return NULL;
    }

    sem_init(&thread_pop_config[curr_handle].sem_stop_pop, 0, 0);

    return tmppophandle;
}

/*!
 * \brief 添加弹出数据的录像类型
 * \param hDataPopper 数据弹出器的句柄
 * \param nRecType 录像类型
 * \return 成功返回0，失败返回小于0的错误码。
 */
PSS_API int API_CALL ZW_PkgSd_DataPopRecTypeAdd(HANDLE hDataPopper, int nRecType)
{
    if(!binited)
    {
        return -1;
    }
    
    PKG_CHECK_PAUSE (s_pkg_pause, -1);
    
    POP_SS *pophandle = NULL;
    pophandle = (POP_SS *) hDataPopper;
    pophandle->recmask |= (1<< nRecType);
    
    return 0;
}

/*!
 * \brief 移除弹出数据的录像类型
 * \param hDataPopper 数据弹出器的句柄
 * \param nRecType 录像类型
 * \return 成功返回0，失败返回小于0的错误码。
 */
PSS_API int API_CALL ZW_PkgSd_DataPopRecTypeDel(HANDLE hDataPopper, int nRecType)
{
    if(!binited)
    {
        return -1; 
    }

    PKG_CHECK_PAUSE (s_pkg_pause, -1);

    POP_SS *pophandle = NULL;
    pophandle = (POP_SS *) hDataPopper;
    pophandle->recmask &= ~(1<< nRecType);
    
    return 0;
}

int PkgSd_GetRecSegList(int handle, AviSegLinkList *plink, time_t tBeginTime, time_t tEndTime, int iRecType)
{
    if (NULL == plink)
    {
        printf("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    if(!binited)
    {
        return -1;
    }

    PKG_CHECK_PAUSE (s_pkg_pause, -1);

    int RecSegCount = 0;

    AviSegLinkList *plinktmp = NULL;

    plinktmp = plink;
    printf("plink = %p\n", plink);

    if(plink < 0)
    {
        return -1;
    }

    /* 为了提高效率，当前暂时不需要对start_time进行I帧的矫正 */

    struct timeval ss_start={0,0},ss_end={0,0},ss_diff={0,0};

    _debug_timeprint();
    gettimeofday(&ss_start, NULL);
    RecSegCount = ReadAviSegLinkByTime(handle, 
                                        tBeginTime, tEndTime, iRecType, 
                                        &plinktmp);
    gettimeofday(&ss_end, NULL);

    _debug_timeprint();

    timersub(&ss_end, &ss_start, &ss_diff);
    unsigned int ss_total = (ss_diff.tv_sec*1000*1000 + ss_diff.tv_usec);

    printf("handle = %d, RecSegCount = %d, spent %u ms\n",  handle, RecSegCount, ss_total/1000);

    if(RecSegCount <= 0)
    {
        return -1;
    }

    thread_pop_config[handle].u_search_start_time  = tBeginTime;
    thread_pop_config[handle].u_search_stop_time   = tEndTime;

    return 0;
}

/*!
 * \brief 开始数据弹出
 * \param hDataPopper 数据弹出器的句柄
 * \return 成功返回0，失败返回小于0的错误码。
 */
PSS_API int API_CALL ZW_PkgSd_DataPopStart(HANDLE hDataPopper)
{
    if(!binited)
    {
        return -1;
    }

    PKG_CHECK_PAUSE (s_pkg_pause, -1);

    int num = 0;
    num = get_num((int)hDataPopper);
    if(num < 0)
    {
        return -1;
    }

    POP_SS *pophandle = NULL;

    pthread_attr_t new_attr;

    pophandle =(POP_SS *)hDataPopper;
    pthread_mutex_lock(&thread_pop_config[num].m_mutexOperate);

    if((0 == pophandle->bpopstop) && (1 == thread_pop_config[num].start_pop_flag))
    {
        pthread_mutex_unlock(&thread_pop_config[num].m_mutexOperate);
        printf("pop has start!\n");
        return 0;
    }

    printf("ZW_PkgSd_DataPopStart--1--\n");

    printf("ZW_PkgSd_DataPopStart--2--\n");

    pthread_attr_init(&new_attr);
    pthread_attr_setdetachstate(&new_attr, PTHREAD_CREATE_DETACHED);

    thread_pop_config[num].start_pop_flag = 1;
    thread_pop_config[num].b_need_seek = 1;
    pophandle->bpopstop = 0;

    _debug_timeprint();

    pthread_create(&thread_pop_config[num].thread_pop_frames, &new_attr, Do_Thread_pop_stream, hDataPopper);
    printf("ZW_PkgSd_DataPopStart--3--\n");
    pthread_mutex_unlock(&thread_pop_config[num].m_mutexOperate);

    // pthread_join(thread_pop_config[num].thread_pop_frames,NULL);
    //启动线程
    return 0;
}

//双击时间轴的地方就是tSeekTime
PSS_API int API_CALL ZW_PkgSd_DataPopSeek(HANDLE hDataPopper, time_t tSeekTime)
{
    if(!binited)
    {
        return -1;
    }
    
    PKG_CHECK_PAUSE (s_pkg_pause, -1);
    
    int num = 0;
    num = get_num((int)hDataPopper);
    if(num < 0)
    {
        return -1;
    }

    int startpop   = 0;
    int popStatus  = 0;
    int popStatus2 = 0;
    
    POP_SS *pophandle = NULL;

    pthread_attr_t new_attr;

    pophandle = (POP_SS *)hDataPopper;
    
    // if(pophandle->tSeekBeginTime == tSeekTime)
    // {
    //     return 0;
    // }
    
    pthread_mutex_lock(&thread_pop_config[num].m_mutexOperate);
    startpop = thread_pop_config[num].start_pop_flag;

    printf("ZW_PkgSd_DataPopSeek--1--startpop=%d\n",startpop);
    if(1 == startpop)
    {    
        printf("wait thread_pop_config[%d].m_mutex\n", num);
        pthread_mutex_lock(&thread_pop_config[num].m_mutex);
        printf("get thread_pop_config[%d].m_mutex\n", num);
        popStatus = pophandle->bpopstop;
        popStatus2 = thread_pop_config[num].start_pop_flag;
        thread_pop_config[num].start_pop_flag = 0;
        pophandle->bpopstop =1;
        pthread_mutex_unlock(&thread_pop_config[num].m_mutex);
        if((0 == popStatus) && (1 == popStatus2))
        {
            sem_wait(&thread_pop_config[num].sem_stop_pop);
        }
    }
    printf("ZW_PkgSd_DataPopSeek--2--\n");

    pophandle->tSeekBeginTime = tSeekTime;
    ((PopHandle *)(pophandle->handle))->read_link_handle = (void *)pophandle->plink;

    if(1 == startpop)
    {
        pthread_attr_init(&new_attr);
        pthread_attr_setdetachstate(&new_attr, PTHREAD_CREATE_DETACHED);

        thread_pop_config[num].start_pop_flag = 1;
        pophandle->bpopstop = 0;
        thread_pop_config[num].b_need_seek = 1;
        pthread_create(&thread_pop_config[num].thread_pop_frames, &new_attr, Do_Thread_pop_stream, hDataPopper);
    }
    printf("ZW_PkgSd_DataPopSeek--3--\n");
    pthread_mutex_unlock(&thread_pop_config[num].m_mutexOperate);

    return 0;
}

/*!
 * \brief 停止数据弹出
 * \param hDataPopper 数据弹出器的句柄
 * \return 成功返回0，失败返回小于0的错误码。
 */
PSS_API int API_CALL ZW_PkgSd_DataPopStop(HANDLE hDataPopper)
{
    if(!binited)
    {
      return -1;
    }

    PKG_CHECK_PAUSE (s_pkg_pause, -1);

    int num = 0;
    num = get_num((int)hDataPopper);
    if(num < 0)
    {
        return -1;
    }

    int popStatus  = 0;
    int popStatus2 = 0;

    POP_SS *pophandle = NULL;
    
    printf("ZW_PkgSd_DataPopStop--1--\n");
    pophandle = (POP_SS *)hDataPopper;
    pthread_mutex_lock(&thread_pop_config[num].m_mutexOperate);
    if((1 == pophandle->bpopstop) && (0 == thread_pop_config[num].start_pop_flag))
    {
        pthread_mutex_unlock(&thread_pop_config[num].m_mutexOperate);
        printf("pop has stop!\n");
        return 0;
    }
    pthread_mutex_lock(&thread_pop_config[num].m_mutex);
    popStatus = pophandle->bpopstop;
    popStatus2 = thread_pop_config[num].start_pop_flag;
    thread_pop_config[num].start_pop_flag = 0;
    thread_pop_config[num].pop_pause = 0;
    pophandle->bpopstop = 1;
    pthread_mutex_unlock(&thread_pop_config[num].m_mutex);
    printf("ZW_PkgSd_DataPopStop--2--\n");
    if((0 == popStatus) && (1 == popStatus2))
    {
        sem_wait(&thread_pop_config[num].sem_stop_pop);
    }
    pthread_mutex_unlock(&thread_pop_config[num].m_mutexOperate);
    printf("ZW_PkgSd_DataPopStop--3--\n");

    return 0;
}

//上层点击暂停时，调用此接口
PSS_API int API_CALL ZW_PkgSd_DataPopPause(HANDLE hDataPopper, int pause)
{
    if(!binited)
    {
        return -1;
    }

    PKG_CHECK_PAUSE (s_pkg_pause, -1);

    int num = 0;
    num = get_num((int)hDataPopper);
    if(num < 0)
    {
        return -1;
    }
    
    pthread_mutex_lock(&thread_pop_config[num].m_mutex);
    printf("pop pause = %d\n", pause);
    thread_pop_config[num].pop_pause = pause;
    pthread_mutex_unlock(&thread_pop_config[num].m_mutex);

    return 0;
}

//POP 线程
void *Do_Thread_pop_stream(HANDLE hDataPopper)
{
    PRT_TID_NAME("DoThreadPoPStream");

    if(!binited)
    {
        return NULL;
    }

    /* 此函数返回 void *，PKG_CHECK_PAUSE 宏不能直接用 -1 */
    if (s_pkg_pause)
    {
        return NULL;
    }

    int num   = 0;
    num = get_num((int)hDataPopper);
    if(num < 0)
    {
        return NULL;
    }

    POP_SS    *pophandle     = NULL;
    PopHandle *zw_pop_handle = NULL;
    
    unsigned char *pbuffer = NULL;
    int size = 0;
    int count = 0;
    int ret   = 0;
    int Frametype = 0;
    int bKeyFrame = 0;
	int VipFrame = 0;
    unsigned int tFrameTimeSec  = 0;
    unsigned int tFrameTimeUSec = 0;
    unsigned int popDataSize = 0;

    printf("Do_Thread_pop_stream hDataPopper = %p\n", hDataPopper);
    pophandle = (POP_SS *)hDataPopper;    
    printf("Do_Thread_pop_stream (PopHandle *)(pophandle->handle))->read_link_handle = %p\n", ((PopHandle *)(pophandle->handle))->read_link_handle);
    printf("thread_pop_config[%d].start_pop_flag = %d\n", num, thread_pop_config[num].start_pop_flag);
    printf("pophandle->bpopstop = %d num = %d\n", pophandle->bpopstop, num);
    printf("%p-%d-%d\n",((PopHandle *)(pophandle->handle))->read_link_handle, pophandle->bpopstop, thread_pop_config[num].start_pop_flag);
    printf("thread_pop_config[%d].b_need_seek = %d\n", num, thread_pop_config[num].b_need_seek);
    zw_pop_handle = (PopHandle *)pophandle->handle;
    zw_pop_handle->start_time = pophandle->tSeekBeginTime;
    zw_pop_handle->stop_time  = pophandle->tSeekEndTime;

    _debug_timeprint();

    while(((PopHandle *)(pophandle->handle))->read_link_handle != NULL && !pophandle->bpopstop && thread_pop_config[num].start_pop_flag)
    {
        //暂停时阻塞(暂停的时候不需要关闭回放句柄)
        while(1 == thread_pop_config[num].pop_pause)
        {
//          printf("pause playing ...\n");
            if((1 == pophandle->bpopstop) || (0 == thread_pop_config[num].start_pop_flag))
            {
                printf("************out pop thread*********\n");
                goto outThread;
            }
            usleep(5000);
        }

        //停止回放(停止的时候需要关闭回放句柄)
        if((1 == pophandle->bpopstop) && (0 == thread_pop_config[num].start_pop_flag))
        {
            printf("************out pop thread! stop palyback!*********\n");
            break;
        }

        bKeyFrame = pophandle->bkeyframe;
		VipFrame = 0;

        // return 返回0表示成功抛出一帧数据;            返回-1表示抛出错误;        返回1表示文件抛出结束; 返回2表示文件不存在，跳过该文件
        /* pop_one_frame_from_sector 第 3 参期望 char **，需做类型转换 */
        ret = pop_one_frame_from_sector((PopHandle *)pophandle->handle,
                                         pophandle->recmask,
                                         (char **)&pbuffer,
                                         &size,
                                         &Frametype,
                                         &bKeyFrame,
                                         &tFrameTimeSec,
                                         &tFrameTimeUSec,
                                         &(thread_pop_config[num].b_need_seek)
                                         ,&VipFrame);
        if(ret < 0)
        {
            printf("error pop_one_frame_from_sector------------------\n");
            break;
        }

        if(ret != 1)
        {
            if (2 == ret)
            {
                // printf("ret = 2: continue, size = %d\n", size);
                continue;
            }
            if (pbuffer)
            {
#if 0
                /* 如果抛出的帧的时间戳小于搜索的最前时间，则跳过丢弃该帧 */
                if (tFrameTimeSec < thread_pop_config[num].u_search_start_time)
                {
                    printf("Warning: tFrameTimeSec=%d, thread_pop_config[%d].u_search_start_time=%d\n", 
                            tFrameTimeSec, num, thread_pop_config[num].u_search_start_time);
                    //usleep(500);
                    //continue;
                }

                /* 如果抛出的帧的时间戳大于搜索的最后时间且是I帧(避免I帧间隔太大造成I帧后面的P帧丢失的情况)，则停止回放 */
                if (tFrameTimeSec > thread_pop_config[num].u_search_stop_time)
                {
                    printf("************search time is arrival, pop over*********\n");
                    goto outThread;
                }
#endif
                /* 如果需要只弹出I帧，则需要丢弃非I帧数据(只是一个命令) */
				/*支持smartP产品需要排除smartP的VIP帧，VIP帧不能抛弃*/
                if (thread_pop_config[num].b_pop_iframe && (!bKeyFrame) && VipFrame != 1)
                {
                    continue;
                }
                /*!
                 * \param hDataPopper 数据弹出器句柄
                 * \param bIndexFrame 是否是建立索引的帧
                 * \param wFrameType 数据对应的类型
                 * \param tFrameTime 数据帧的绝对时间簇，精确度为秒
                 * \param pBuf 数据对应的缓冲区地址
                 * \param dwSize 数据对应的大小
                 * \param EventID 发生的事件类型，如果是正常的数据弹出，EventID为DATAPOP_EVENT_NONE，其他事件时，帧相关的参数无效
                 * \param pContext 创建弹出器时传入的上下文
                 * \return
                 */
                pophandle->popcallback(hDataPopper, 0, Frametype, tFrameTimeSec, pbuffer, size, DATAPOP_EVENT_NONE, pophandle->pContext);
                popDataSize += size;
            }
        }
        else
        {
            printf("************all file list pop over*********\n");
            goto outThread;
        }

        pbuffer = NULL;

        if(++count > 20)
        {
            count = 0;
            usleep(5000);
        }
    }

outThread:

    /*!
     * \param hDataPopper 数据弹出器句柄
     * \param bIndexFrame 是否是建立索引的帧
     * \param wFrameType 数据对应的类型
     * \param tFrameTime 数据帧的绝对时间簇，精确度为秒
     * \param pBuf 数据对应的缓冲区地址
     * \param dwSize 数据对应的大小
     * \param EventID 发生的事件类型，如果是正常的数据弹出，EventID为DATAPOP_EVENT_NONE，其他事件时，帧相关的参数无效
     * \param pContext 创建弹出器时传入的上下文
     * \return
     */
    printf("hand=%p, bpopstop=%d, start_pop_flag=%d, tFrameTimeSec=%d\n", 
            ((PopHandle *)(pophandle->handle))->read_link_handle, 
            pophandle->bpopstop, 
            thread_pop_config[num].start_pop_flag,
            tFrameTimeSec);
    pophandle->popcallback(hDataPopper, 0, Frametype, tFrameTimeSec, pbuffer, size, DATAPOP_EVENT_TIME_OVER, pophandle->pContext);//时间帧，pop到当前结束时间

    printf("------pop thread exit-----num=%d--------popDataSize=%d bytes---------\n", num, popDataSize);

    _debug_timeprint();

    /* 停止对应录像的回放 */
    StopRecordPlayback(num);
    sem_post(&thread_pop_config[num].sem_stop_pop);

    return NULL;
}

/*!
 * \brief 释放数据弹出器
 * \param hDataPopper 数据弹出器的句柄
 * \return 成功返回0，失败返回小于0的错误码。
 */
PSS_API int API_CALL ZW_PkgSd_DataPopRelease(HANDLE hDataPopper)
{
    if(!binited)
    {
        return -1;
    }
    
    PKG_CHECK_PAUSE (s_pkg_pause, -1);
    
    int num = 0;
    num = get_num((int)hDataPopper);
    if(num < 0)
    {
        return -1;
    }

    POP_SS *pophandle = NULL;
    
    pophandle =(POP_SS *)hDataPopper;
    if((0 != thread_pop_config[num].start_pop_flag) || (1 != pophandle->bpopstop))
    {
        printf("please stop pop first! %d-%d\n",thread_pop_config[num].start_pop_flag,pophandle->bpopstop);
        return -1;
    }

    printf("release hand = %p num = %d\n", hDataPopper, num);
    if(pophandle->plink)
    {
        FreeAviSegLinkLists(0, (AviSegLinkList *)pophandle->plink); 
        pophandle->plink = NULL;
    }

    printf("step 1\n");
    if(pophandle->handle)
    {
        FreeOnePopHandle(pophandle->handle);
    }
    
    printf("step 2\n");
    printf("step 3\n");
    if(pophandle->handle)
    {
        DestroyOnePopHandle(pophandle->handle);
        pophandle->handle = 0;
    }
    memset(&thread_pop_config[num], 0, sizeof(ZW_POP_Thread_dat));
    printf("step 4\n");
    //pophandle = NULL; //可能有错误，无法传递      

    return 0;
}

/*!
 * \brief 数据弹出时是否只弹出关键帧
 * \param hDataPopper 数据弹出器的句柄
 * \param bKeyFrame 是否只弹出关键帧，如果为TRUE则只弹出关键帧，如果为FALSE则弹出所有帧。
 * \param PopKeyInterval 当只弹出关键帧时，该参数有效，
 *  表示通道弹出每隔多少关键帧弹出一个，此值限制在0-3，0表示全部弹出
 * \return 成功返回0，失败返回小于0的错误码。
 */
PSS_API int API_CALL ZW_PkgSd_DataPopSetKeyFrame(HANDLE hDataPopper, BOOL bKeyFrame)
{
    if(!binited)
    {
        return -1;
    }

    int num = 0;
    num = get_num((int)hDataPopper);
    if(num < 0)
    {
        return -1;
    }

    pthread_mutex_lock(&thread_pop_config[num].m_mutex);
    printf("pop b_pop_iframe = %d\n", bKeyFrame);
    thread_pop_config[num].b_pop_iframe = bKeyFrame;
    pthread_mutex_unlock(&thread_pop_config[num].m_mutex);
    return 0;
} 

void pop_callback(HANDLE hDataPopper, BOOL bIndexFrame, unsigned short wFrameType, time_t tFrameTime, unsigned char *pBuf, unsigned long dwSize, unsigned long EventID, void *pContext)
{
//   printf("hDataPopper = %p wFrameType=%d tFrameTime=%d, pBuf = %p ,dwSize =%ld \n", hDataPopper,wFrameType,tFrameTime,pBuf,dwSize);
}

#if 0
void main()
{
 char dev[30];
 FILE *fp;
 T_SDINFO diskinfo;
 H264DATAPACKETHEADER1 *pheader;
 int i;
 int segcount=0;
 PSS_REGSEG_T *pregseg;
 int linkhandle;
 int pop_handle;
 fp = fopen("/mnt/tt/1.mp4","rb");
 if(fp <0)
     {
      printf("file not exsist \n");
     return;
     }
 ZW_PkgSD_Init();
 ZW_PkgSd_GetDiskInfo(&diskinfo);
 printf("%s %s %d\n",diskinfo.acModelName,diskinfo.acSerialNo,diskinfo.dwTotal_Disk_Space);
 ZW_PkgSd_SetDiskGroupCycleMode(1);
//for(;;)
     {
     fseek(fp,0,SEEK_SET);
 fread(one_frame,1,19,fp);
 printf("one_frame[0]=%d \n",one_frame[0]);
 for(i=0;i<20000;i++)
 {
   printf("=====================================================frame %d================================\n",i);
   fread(one_frame,1,sizeof(H264DATAPACKETHEADER1),fp);
   pheader = (H264DATAPACKETHEADER1 *)one_frame;
    printf("one_frame =0x%x,0x%x \n", pheader->uOverloadLen,pheader->uDataPacketStartCode );
   fread(one_frame+sizeof(H264DATAPACKETHEADER1),1, pheader->uOverloadLen,fp);
   ZW_PkgSd_DataInput(one_frame,pheader->uOverloadLen+sizeof(H264DATAPACKETHEADER1));
 //  usleep(15000);
 }
     }
     linkhandle=ZW_PkgSd_GetRecSegList(0x536BA65d,0x536ba720,0xff,&pregseg,&segcount);
    printf("1111111 linkhandle = %x \n",linkhandle);

if(linkhandle >0)
    ZW_PkgSd_ReleaseRecSegList(linkhandle,pregseg);
    pop_handle = ZW_PkgSd_DataPopCreate(0x536BA65d,0x536ba720,pop_callback,NULL);
    printf("1111111 pop_handle = %x \n",pop_handle);
    ZW_PkgSd_DataPopStart(pop_handle);
    printf("ZW_PkgSd_DataPopStart \n" );
     getchar();
    ZW_PkgSd_DataPopStop(pop_handle);

 fclose(fp);

}
#endif
