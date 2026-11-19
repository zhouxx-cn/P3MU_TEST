#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/ioctl.h>
#include <sys/time.h>  
#include <sys/prctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <dirent.h>
#include <semaphore.h>
#include <signal.h>

#include "RingBuffer.h"
#include "sd_ops.h"
#include "MemRecord.h"
#include "global_macro.h"
#include "sys_banned.h"
#include "LogManager.h"
#ifdef TEST_MEM_INFO
#define MODULE_NAME "memrecord"
#include "MemoryInterface.h"
#endif

#define MEM_ERR(format...)              LOG_E(LOG_MODULE_SD, format)
#define MEM_WRAN(format...)             LOG_W(LOG_MODULE_SD, format)
#define MEM_INFO(format...)             LOG_I(LOG_MODULE_SD, format)
#define MEM_DBG(format...)              LOG_D(LOG_MODULE_SD, format)

//录像类型：
//包括：无录像、报警、普通、移动侦测、HTTP触发
typedef enum{
	RECORD_TYPE_NO = 0,    //无录像
	RECORD_TYPE_ALARM,     //报警录像类型
	RECORD_TYPE_GENERAL,   //普通类型
	RECORD_TYPE_MANUAL,    //手动录像类型
	RECORD_TYPE_MOTION,    //移动录像
	RECORD_TYPE_HTTP_TRIG,
	RECORD_TYPE_AI,        //智能录像(人形/人脸/...) //fjz-notice
}enumRecType;

typedef enum{
    RECORD_PAUSE = 0, // 录像暂停
    RECORD_WRITE  = 1, // 录像继续
}enumRecStatus;

static int PER_FRAME_MAX_SIZE = (480*1024); // (512*1024)
static int RING_BUFFER_SIZE   = (1<<20);    // (1*1024*1024) 环形缓冲区的buffer大小必须为2的n次幂

#define MAX_PREV_RECORD_TIME_SEC       (120) // 最大预录时间

#define DEFAULT_PREV_RECORD_TIME_SEC (15) // 默认预录时间

#define LIMITE_TIME          (10)

/* 该宏用来控制MEM调试打印信息 */
#define MEM_DBG_INF

// static char g_frame_data[PER_FRAME_MAX_SIZE] = {0};
static char* g_frame_data = NULL;

static pRingBuffer  g_p_ring_handle = NULL;

static pthread_mutex_t *p_ringbuffer_lock = NULL;

static pthread_mutex_t memrecord_write_lock = PTHREAD_MUTEX_INITIALIZER;

static pthread_mutex_t memrecord_record_status_lock = PTHREAD_MUTEX_INITIALIZER;

static int g_mem_record_has_inited = 0;

static int g_exit_flag = 0;

static int g_record_type = 0;

static int g_need_stop_record = 0;
static int g_memrecord_pause_flag = 0;
static int g_memrecord_getbufdatatoavi_pause_flag = 0;
static sem_t g_sem_pauseandresume;
static sem_t g_sem_pauseandresume_feedback;

static enumRecStatus g_e_record_status = RECORD_WRITE;

static int g_encode_type = 1; // 0:H264  1:H265

static unsigned char g_out_avi_file[64] = {0};

static sem_t g_semTrigRecordOverCallBackThread;

static unsigned int g_need_update_file  = 0;
static unsigned int g_need_record         = 0;
static unsigned int g_b_first_frame       = 1;
static unsigned int g_prev_time_sec       = DEFAULT_PREV_RECORD_TIME_SEC;
static unsigned int g_rec_start_time     = 0;
static unsigned int g_b_first_set_codec = 1;
static int g_usr_record_type = -1;  //用户录像类型  0-连续录像,1-事件录像
static int g_usr_record_enable = -1; //用户录像使能

// Http event触发录像完毕之后，发送消息的回调函数
int (*m_TrigRecordOverCallBack)(unsigned char *) = NULL;

/* 线程列表 */
typedef struct tagThreadList{
    void * (*funcp) (void*);
    void *pthread_attr;
    void *args;
}ThreadList;

#ifdef __USE_CIRCLEQUEUE__

static RecordInterface g_RecordInterface = {NULL,NULL,NULL,NULL};

/*!
* @brief
*	设置录像取流数据回调
* @param pInterface
*	拉流方式接口信息
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
int MemRecord_SetRecordInterface(const LPRecordInterface pInterface)
{
	if (NULL == pInterface)
	{
		return -1;
	}

	memcpy(&g_RecordInterface,pInterface,sizeof(RecordInterface));
	return 0;
}

static void *GetBufferDataToWriteAvi(void *lparam)
{
    PRT_TID();
    pthread_detach(pthread_self());

    PRIVATEFRAMEHEADER *RawHead = NULL;
    PRIVATEFRAMEHEADER RawHeadTmp;

    unsigned int  avi_write_over = 0;
    unsigned long temp_real_time_sec = 0;
    unsigned long prev_frame_real_time_sec = 0;
    int last_encode_type = g_encode_type;

	unsigned int UserHandler = 0;
	int result = 0;
	char *pStreamBuff = NULL;
	unsigned int nStreamLen = 0;
	unsigned int frame_size = 0;
    int is_need_sleep = 0;

	while(1)
	{
		sleep(1);
	
		if ((NULL != g_RecordInterface.ifRecordGetData) &&
			(NULL != g_RecordInterface.ifRecordReleaseData) &&
			(NULL != g_RecordInterface.ifRecordUserAdd) &&
			(NULL != g_RecordInterface.ifRecordUserDelete))
		{
			break;
		}
	}

        g_RecordInterface.ifRecordUserAdd(0, 0xff, &UserHandler);

	MEM_INFO("create thread GetBufferDataToWriteAvi\n");

    for(;;)
    {
        is_need_sleep = 1;
        if (g_memrecord_getbufdatatoavi_pause_flag && g_b_first_frame)//判断文件写完后再暂停录像
        {
            MEM_DBG ("going to pause\n");
			//暂停时，删除消费者
            g_RecordInterface.ifRecordUserDelete(UserHandler);
            /* 通知即将进入暂停状态 */
            sem_post (& g_sem_pauseandresume_feedback);
            /* 进入暂停状态 */
            sem_wait (& g_sem_pauseandresume);
			//解除暂停后，重新添加消费者
            g_RecordInterface.ifRecordUserAdd(0,0xff,&UserHandler);
        }

        if (0 == g_mem_record_has_inited)
        {
            g_exit_flag = 1;
			g_need_update_file = 1;
            g_b_first_frame = 1;
		    MEM_INFO("record exit GetBufferDataToWriteAvi\n");
            break;
        }    
        
        if (g_need_record)
        {
			//获取流数据
			result = g_RecordInterface.ifRecordGetData(UserHandler, &pStreamBuff,&nStreamLen);
			if (-1 == result)
			{
				//获取数据错误
				continue;
			}
			else if (-3 == result)
			{
				/* 非录像所需码流帧, 缓冲区释放 */
				continue;
			}

			if (-2 != result && nStreamLen < sizeof(PRIVATEFRAMEHEADER))
			{
				//循环队列有数据，且数据长度太短
				g_RecordInterface.ifRecordReleaseData(UserHandler, pStreamBuff,nStreamLen);
				continue;
			}
        
            if (-2 != result)  // 缓冲区是否有数据
            {
                is_need_sleep = 0;
                pthread_mutex_lock(&memrecord_write_lock);
                frame_size = nStreamLen - sizeof(PRIVATEFRAMEHEADER);
                RawHead = &RawHeadTmp;
                memcpy(RawHead,pStreamBuff,sizeof(PRIVATEFRAMEHEADER));

                g_frame_data = pStreamBuff; //包含帧头数据一并写入。
                if (frame_size == RawHead->nOverloadLen && 0xB7010000 == RawHead->DataPacketStartCode)
                {
                    /* 针对于非首帧数据，需要判断NTP是否出现了对时 */
                    if (!g_b_first_frame)
                    {
                        /* 发生时间跳变，判断是否需要切换文件 */
                        if (labs(RawHead->u32RealTimeSec - prev_frame_real_time_sec) >= 10)
                        {
                            /* 切换文件的条件(1为必要条件，2和3任选其一即可)：
                             * 1. 前一帧数据的时间和当前的帧数据时间差值超过10秒；
                             * 2. 当前帧的时间早于当前录像文件的起始时间；
                             * 3. 当前帧的时间晚于录像的起始时间+10分钟；
                             */
                            if (RawHead->u32RealTimeSec < g_rec_start_time 
                             || g_rec_start_time + 600  < RawHead->u32RealTimeSec)
                            {
#ifdef MEM_DBG_INF
                                MEM_INFO("Need auto update file! time [%lu(s) to %lu(s)]\n", 
                                          RawHead->u32RealTimeSec, prev_frame_real_time_sec);
#endif
                                g_need_update_file = 1;
                                /* 结束时间已经变成了NTP对时之后的时间，需要暂存起来，使用前一帧的时间来替换该值 */
                                temp_real_time_sec = RawHead->u32RealTimeSec;
                                RawHead->u32RealTimeSec = prev_frame_real_time_sec;
                            }
                        }
#if 1
                        if (g_b_first_set_codec)
                        {
                            g_b_first_set_codec = 0;
                            last_encode_type = g_encode_type;
                        }

                        /* 如果编码类型发生改变，则需要切换文件 */
                        if (last_encode_type != g_encode_type)
                        {
#ifdef MEM_DBG_INF
                            MEM_INFO("Need auto update file! last [%s] now [%s]\n", 
                                      last_encode_type?"H265":"H264", g_encode_type?"H265":"H264");
#endif
                            g_need_update_file = 1;
                            last_encode_type = g_encode_type;
                        }
#endif
                    }

                    WriteDataToSdCard(RawHead->nOverloadLen,
                                      (unsigned char *)g_frame_data,
                                      (unsigned char *)RawHead,
                                      &g_need_update_file,
                                      &avi_write_over,
                                      g_out_avi_file,
                                      &g_rec_start_time);
                    prev_frame_real_time_sec = RawHead->u32RealTimeSec;
					//printf("prev  frametype:%d  prevtime: %d\n",RawHead->FrameType,prev_frame_real_time_sec);
                    if (temp_real_time_sec)
                    {
                        prev_frame_real_time_sec = temp_real_time_sec;
                        temp_real_time_sec = 0;
                    }
                    g_b_first_frame = 0;

                    /* 如果文件写完毕则需要触发回调函数 */
                    if (avi_write_over)
                    {
                        avi_write_over = 0;
                        g_need_record  = 0;
                        g_need_update_file = 0;
                        g_b_first_frame = 1;
                        sem_post(&g_semTrigRecordOverCallBackThread);
                        pthread_mutex_unlock(&memrecord_write_lock);
                        //释放结点
                        g_RecordInterface.ifRecordReleaseData(UserHandler, pStreamBuff,nStreamLen);
                        continue;
                    }
                }
                else
                {
                    MEM_ERR("Get frame head from CircQueue checksum frame head fail!\n");
                }

                pthread_mutex_unlock(&memrecord_write_lock);

                //释放结点
                g_RecordInterface.ifRecordReleaseData(UserHandler, pStreamBuff,nStreamLen);
            }
        }
        else
        {
			//录像未使能or不是连续录像
        	if ((0 != g_usr_record_type) || (0 == g_usr_record_enable))
			{
				//获取流数据
		        result = g_RecordInterface.ifRecordGetData(UserHandler, &pStreamBuff,&nStreamLen);
		        if (-1 == result)
		        {
		        	//获取数据错误
					continue;
		        }
                else if (-3 == result)
				{
					/* 非录像所需码流帧, 缓冲区释放 */
					continue;
				}                

		        if (-2 != result && nStreamLen < sizeof(PRIVATEFRAMEHEADER))
		        {
		        	//循环队列有数据，且数据长度太短
					g_RecordInterface.ifRecordReleaseData(UserHandler, pStreamBuff,nStreamLen);
					continue;
		        }
				//未进行录像时，要保持读序列号增加。
				if (-2 != result)
				{
					RawHead = (PRIVATEFRAMEHEADER *)pStreamBuff;
					//释放结点
					g_RecordInterface.ifRecordReleaseData(UserHandler, pStreamBuff,nStreamLen);
				}
				else
				{
					//缓冲区为空，休眠100ms
					usleep(100000);
					continue;
				}
			}
        }

		if (is_need_sleep)
        {
            usleep(10*1000); /* 防止 CPU 过高 */
        }
    }
    g_RecordInterface.ifRecordUserDelete(UserHandler);

    return NULL;
}
#endif

/* 异步执行录像切换文件时候的回调函数，防止塞住取流线程 */
static void *DoTrigRecordOverCallBack(void *lparam)
{
    PRT_TID();
    pthread_detach(pthread_self());

	MEM_INFO("create thread TrigRecordOver CallBack\n");
    sleep(2);
    while(1)
    {
		if (0 == g_mem_record_has_inited)
		{
			MEM_INFO("record exit TrigRecordOver CallBack\n");
			break;
		}  
        sem_wait(&g_semTrigRecordOverCallBackThread);
        if (m_TrigRecordOverCallBack)
        {
#ifdef MEM_DBG_INF
            MEM_INFO("avi file wrote over! need called callback!\n");
#endif
            m_TrigRecordOverCallBack(g_out_avi_file);
            memset(g_out_avi_file, 0, sizeof(g_out_avi_file));
        }
    }
    return NULL;
}

static ThreadList thread_list[] = {
    {GetBufferDataToWriteAvi,  NULL, NULL},
    {DoTrigRecordOverCallBack, NULL, NULL},
};

__attribute__((unused)) static pRingBuffer MemRecord_AllocRingBuffer (int nSize, pthread_mutex_t *pMutex)
{
    if (!IS_POWER_OF_2(nSize))  
    {
        MEM_INFO("Size of buffer is %d\n", nSize);
        nSize = RoundUpToPowerOfTwo(nSize);
        MEM_INFO("Size of buffer not be power of 2, change to be power of 2 is %d\n", nSize);
    }

    void * buffer = malloc(nSize);
    if (! buffer)
    {
        MEM_ERR("Failed to malloc memory");
        return NULL;
    }

    memset(buffer, 0, nSize);

    int resolve_size = nSize >> 2;
    resolve_size = resolve_size > PER_FRAME_MAX_SIZE ? PER_FRAME_MAX_SIZE : resolve_size;
	MEM_INFO("ring_buffer_size=%d, resolve_size=%d\n", nSize, resolve_size);

    pRingBuffer pHandle = ring_buffer_init (buffer, nSize, resolve_size, pMutex);
    return pHandle;
}

int MemRecordInitV2(MemRecordInitParam *pInitParam)
{
    if (pInitParam != NULL)
    {
        RING_BUFFER_SIZE = pInitParam->nRingBufSize;
        PER_FRAME_MAX_SIZE = pInitParam->nMaxFrameSize;
    }
    MemRecordInit();
    return 0;
}

/*@breif SetRecordStatus 设置录像的写入状态
 *@param IN status 写入状态 0:暂停  1:继续写入 参见enumRecStatus
 */
int SetRecordWriteStatus(int status)
{
    if (status == (int)g_e_record_status)
    {
        return 0;
    }

    pthread_mutex_lock(&memrecord_record_status_lock);
    g_e_record_status = (enumRecStatus)status;

    if (RECORD_PAUSE == g_e_record_status)
    {
        g_need_stop_record = 1;
    }
    pthread_mutex_unlock(&memrecord_record_status_lock);

    return 0;
}

int MemRecordInit(void)
{
#ifndef TEST_MEM_INFO
    p_ringbuffer_lock = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
#else
	p_ringbuffer_lock = (pthread_mutex_t *)MemoryMalloc(sizeof(pthread_mutex_t), "MemRecord", __FILE__, __FUNCTION__, __LINE__);
#endif
    if (!p_ringbuffer_lock)
    {
        MEM_ERR("Failed to malloc lock memory");
        return -1;
    }

    if (pthread_mutex_init(p_ringbuffer_lock, NULL) != 0)
    {
        MEM_ERR("Failed init mutex");
        pthread_mutex_destroy(p_ringbuffer_lock);
        SAFE_FREE(p_ringbuffer_lock);
        return -1;
    }

#ifndef __USE_CIRCLEQUEUE__
    g_frame_data = (char*) malloc (PER_FRAME_MAX_SIZE);
    
    /* 为环形缓冲区分配总空间 */
    size = RING_BUFFER_SIZE;

    g_p_ring_handle = MemRecord_AllocRingBuffer (size, p_ringbuffer_lock);
    if (NULL == g_p_ring_handle)
    {
        pthread_mutex_destroy(p_ringbuffer_lock);
        SAFE_FREE(p_ringbuffer_lock);
        MEM_ERR ("failed to create ring buffer\n");
        return -1;
    }
#else
	// memset(&g_RecordInterface,0,sizeof(g_RecordInterface));
#endif

    sem_init(&g_semTrigRecordOverCallBackThread, 0, 0);
    sem_init(&g_sem_pauseandresume, 0, 0);
    sem_init(&g_sem_pauseandresume_feedback, 0, 0);

    pthread_t pid[10];
    int i = 0;
    int num = sizeof(thread_list)/sizeof(thread_list[0]);

    for (i=0; i<num; i++)
    {
        pthread_create(&pid[i], thread_list[i].pthread_attr, thread_list[i].funcp, (void*)thread_list[i].args);
    }

    g_mem_record_has_inited = 1;
    
    return 0;
}

void MemRecordExit(void)
{
    MEM_INFO("Detect that need reboot, need stop avi recording ...\n");

    pthread_mutex_lock(&memrecord_write_lock);
    g_need_update_file = 1;
    pthread_mutex_unlock(&memrecord_write_lock);

    /* 睡一秒等待退出 */
    sleep(1);

    g_mem_record_has_inited = 0;
	sem_post(&g_semTrigRecordOverCallBackThread);
    sleep(1);

    int count = 100;
    while(count--)
    {
        if (g_exit_flag)
        {
            break;
        }

        usleep(10000);
    }

    if (g_exit_flag)
    {
#ifndef __USE_CIRCLEQUEUE__
        ring_buffer_free(g_p_ring_handle);
#endif
        if (NULL != p_ringbuffer_lock)
        {
            pthread_mutex_destroy(p_ringbuffer_lock);
            SAFE_FREE(p_ringbuffer_lock);
        }        
                
        sem_destroy(&g_semTrigRecordOverCallBackThread);
		sem_destroy(&g_sem_pauseandresume);
        sem_destroy(&g_sem_pauseandresume_feedback);
        g_exit_flag = 0;
    }
#ifndef __USE_CIRCLEQUEUE__ 
    SAFE_FREE (g_frame_data);
#endif
}

static int MemRecord_WaitThreadPause ()
{
    MEM_DBG ("\n");

    MEM_DBG ("wait for thread GetBufferDataToWriteAvi to pause\n");
    g_memrecord_getbufdatatoavi_pause_flag = 1;
    sem_wait (& g_sem_pauseandresume_feedback);
    MEM_DBG ("thread GetBufferDataToWriteAvi paused\n");

    return 0;
}

static int MemRecord_FreeMemUse ()
{
    MEM_DBG ("\n");
	
#ifndef __USE_CIRCLEQUEUE__ 
    ring_buffer_free (g_p_ring_handle);
    g_p_ring_handle = NULL;

    if(g_frame_data)
    {
        SAFE_FREE (g_frame_data);
    }
#endif

    return 0;
}

static int MemRecord_WaitThreadContinue ()
{
    MEM_DBG ("\n");

    g_memrecord_getbufdatatoavi_pause_flag = 0;
    sem_post (& g_sem_pauseandresume);

    return 0;
}

static int MemRecord_ReallocMem ()
{
    MEM_DBG ("\n");
	
#ifndef __USE_CIRCLEQUEUE__ 
    int size = RING_BUFFER_SIZE;
    g_p_ring_handle = MemRecord_AllocRingBuffer (size, p_ringbuffer_lock);

    g_frame_data = (char*) malloc (PER_FRAME_MAX_SIZE);
#endif

    return 0;
}

int MemRecord_Pause ()
{
    MEM_DBG ("\n");

    if (g_memrecord_pause_flag)
    {
        MEM_ERR ("pause while paused\n");
        return -1;
    }

    /* 封锁接口调用 */
    g_memrecord_pause_flag = 1;

    /* 暂停线程 */
    MemRecord_WaitThreadPause ();

    /* 释放内存占用 */
    MemRecord_FreeMemUse ();

    return 0;
}

int MemRecord_Resume ()
{
    MEM_DBG ("\n");

    /* 恢复内存占用 */
    MemRecord_ReallocMem ();

    /* 恢复线程 */
    MemRecord_WaitThreadContinue ();

    /* 开放接口调用 */
    g_memrecord_pause_flag = 0;

    return 0;
}

/* 获取系统预录时间 */
int GetPrevTimeSec(void)
{
    return g_prev_time_sec;
}

/**
 * 设置录像延迟时间
 */
int MEMRECORD_SetDelayTime (unsigned int nDelaySecs)
{
    int ret = SDOPS_SetRecordDelayTime (nDelaySecs);
    return ret;
}

/**
 * 获取录像延迟时间
 */
int MEMRECORD_GetDelayTime (unsigned int *pnDelaySecs)
{
    int ret = SDOPS_GetRecordDelayTime (pnDelaySecs);
    return ret; 
}

/* 设置系统预录时间(单位: 秒) */
void SetPrevTimeSec(unsigned int prev_time_sec)
{
    /* 暂时不支持预录时间设置，默认为DEFAULT_PREV_RECORD_TIME_SEC */
return ;

    if (0 == g_mem_record_has_inited)
    {
        return;
    }

    MEM_DBG("Current set prev record time is %d seconds\n", prev_time_sec);

    prev_time_sec = prev_time_sec > MAX_PREV_RECORD_TIME_SEC ? MAX_PREV_RECORD_TIME_SEC : prev_time_sec;

    if (prev_time_sec != g_prev_time_sec)
    {
        g_prev_time_sec = prev_time_sec;
    }
}

/* 获取触发录像类型 */
int GetTrigRecordType(void)
{
    return GetSdRecordType();
}

/* 设置触发录像类型 */
int SetTrigRecordType(int record_type)
{
#ifdef MEM_DBG_INF
    MEM_DBG("Set record type is %d\n", record_type);
#endif

    SetSdRecordType(record_type);

    g_record_type = record_type;

    return 0;
}

/* 开始录像 */
void StartRecord(int record_type, int bH265)
{
    if (0 == g_mem_record_has_inited)
    {
        return;
    }

    if (g_memrecord_pause_flag)
    {
        MEM_DBG ("call while paused\n");
        return ;
    }

    SetSdRecordType(record_type);

    /* 如果录像类型为不录像，则直接返回 */
    if (RECORD_TYPE_NO == record_type)
    {
        return;
    }

    /* 如果切换了高级编码类型，则需要马上切换文件 */
    if (g_encode_type != bH265)
    {
        MEM_DBG("Codec changed! g_encode_type %d bH265 %d\n", g_encode_type, bH265);
        g_need_record = 0;
        g_encode_type = bH265;
    }

    if (0 == g_need_record)
    {
        g_record_type = record_type;
        g_encode_type = bH265;
        g_need_record = 1;
        MEM_INFO("Start to record [%d] [%s]...\n", record_type, bH265?"H265":"H264");
    }
}

/* 设置用户录像类型 */
void SetUsrRecordType(int type,int enable)
{
	g_usr_record_type = type;
	g_usr_record_enable = enable;
}

/* 获取录像类型和使能*/
void GetUsrRecordType(int *type,int *enable)
{
    *type = g_usr_record_type;
    *enable = g_usr_record_enable;
}

/*@breif 从环形缓冲区里面获取一帧数据写成AVI到SD卡
 *@param IN frame_size   帧大小
 *@param IN frame_buffer 帧缓冲
 *@note 不可以塞死
 */
int  RawDataWirteProcess(int frame_size, const char * frame_buffer)
{
    if (0 == g_mem_record_has_inited)
    {
        return 0;
    }

    if (g_memrecord_pause_flag)
    {
        MEM_DBG ("call while paused\n");
        return 0;
    }

#if 0

    static int i = 0;
    if (i < 4000)
    {
        i ++;
        return 0;
    }
    else
    {
        if (i >= 10000)
        {
            return 0;
        }
    
        static FILE *fp = NULL;
        if (NULL == fp)
        {
            printf("------------------------start to write test.264--------------------\n");
            fp = fopen("/root/test.264", "w");
        }
        
        if (fp)
        {
            i ++;
            fwrite(frame_buffer, frame_size, 1, fp);
            if (i >= 20000)
            {
                printf("-------------------write test.264 over-------------------\n");
                fflush(fp);
                SAFE_FCLOSE(fp);
                i = 40000;
            }
        }
    }
    return 0;

#endif

    PRIVATEFRAMEHEADER RawHead;

    memcpy(&RawHead, frame_buffer, sizeof(RawHead));

    /* 写环形缓冲区之前, 需要校验帧头信息 */
    if (0xB7010000 == RawHead.DataPacketStartCode)
    {
        my_ring_buffer_put(g_p_ring_handle, frame_buffer, frame_size);
    }
    else
    {
        MEM_ERR("Frame head crc fail is %#x != 0xB7010000\n", RawHead.DataPacketStartCode);
    }

    return 0;
}

/* 触发录像完毕之后，发送消息的回调函数 */
int SetTrigRecordOverCallBack(int (*CallBack)(unsigned char *))
{
    if (NULL == CallBack)
    {
        return -1;        
    }

    m_TrigRecordOverCallBack = CallBack;

    return 0;
}
