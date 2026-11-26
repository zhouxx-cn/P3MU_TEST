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
#include "..//mp4/include/mp4_writer.h"
#include "common.h"
#include "MemRecord.h"
#include "global_macro.h"
#include "sys_banned.h"
#include "LogManager.h"
#include "sd_ops.h"
#ifdef TEST_MEM_INFO
#define MODULE_NAME "memrecord"
#include "MemoryInterface.h"
#endif

#define REC_ERR(format...)              LOG_E(LOG_MODULE_SD, format)
#define REC_WRAN(format...)             LOG_W(LOG_MODULE_SD, format)
#define REC_INFO(format...)             LOG_I(LOG_MODULE_SD, format)
#define REC_DBG(format...)              LOG_D(LOG_MODULE_SD, format)

#define LIMITE_TIME          (10)

/* 该宏用来控制MEM调试打印信息 */
#define MEM_DBG_INF

static char* g_frame_data = NULL;

static pthread_mutex_t memrecord_write_lock = PTHREAD_MUTEX_INITIALIZER;

static int g_mem_record_has_inited = 0;

static int g_exit_flag = 0;

static int g_memrecord_pause_flag = 0;
static int g_memrecord_getbufdatatoavi_pause_flag = 0;
static sem_t g_sem_pauseandresume;
static sem_t g_sem_pauseandresume_feedback;

static int g_encode_type = 1; // 0:H264  1:H265

static char g_out_avi_file[128] = {0};

static sem_t g_semTrigRecordOverCallBackThread;

static unsigned int g_need_update_file  = 0;
static unsigned int g_need_record         = 0;
static unsigned int g_b_first_frame       = 1;
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

/****************************************************************************************/
#define MAX_NAME_STR             (64)
#define MIN_NAME_STR             (33)
#define MAX_FILE_PATH_STR        (257)
#define MAX_PATH_STR_NUM         (128)
#define MAX_SEARCH_DAY           (365)
#define DAY_TIME_STAMP_SEC       (86400)   // 24*60*60
#define MAX_FILE_NUM             ((DAY_TIME_STAMP_SEC/EVENT_RECORD_LATEST_SECONDS) + 30) // 增加一些余量

/* ============================================================================
 * MP4 循环录像相关（Readme 新需求）
 * ----------------------------------------------------------------------------
 * 只按录像时长自动切片，切片间隔由 g_mp4_segment_duration_sec 控制，
 * 上层可通过 SDOPS_SetMp4SegmentDuration 动态调整。
 * 容器为 fMP4（分片 MP4），异常掉电后已写入分片仍可直接播放，
 * 底层复用 modules/mp4 的 MP4Writer 系列接口，不做二次编码。
 * ==========================================================================*/
static unsigned int   g_mp4_segment_duration_sec = 300; /* 分片时长阈值(秒)，默认 5 分钟 */

static MP4Writer     *g_mp4_writer = NULL;       /* 当前分片写入句柄 */
static char           g_mp4_curr_file_name[MAX_PATH_STR_NUM] = {0}; /* 当前分片绝对路径 */
static unsigned int   g_mp4_seg_begin_real_sec = 0;  /* 当前分片首帧的绝对秒 */
static unsigned int   g_mp4_seg_begin_real_usec = 0; /* 当前分片首帧的绝对微秒 */
static int64_t        g_mp4_seg_begin_frame_abs_us = 0; /* 首帧绝对时间(微秒)，用于归一化 */
static int            g_mp4_have_first_frame = 0;    /* 是否已建立时间基线 */

/* ============================================================================
 * 对外可配置接口：允许上层应用动态调整 MP4 分片时长阈值
 * ==========================================================================*/
void REC_SetMp4SegmentDuration(unsigned int sec)
{
    g_mp4_segment_duration_sec = (sec > 0) ? sec : 300;
}
unsigned int REC_GetMp4SegmentDuration(void)
{
    return g_mp4_segment_duration_sec;
}

/* ============================================================================
 * SD_OPS 模块设计文档 —— MP4 循环录像改造
 * ----------------------------------------------------------------------------
 * 【背景】
 *   本文件原有 WriteDataToSdCard 使用 AVI 容器（InitAviActionMode +
 *   WriteFrameToAvi + StopAviAction），切片条件包含「定时录像时长 /
 *   事件录像时长 / 录像类型切换 / 跨天对时」等多重规则。
 *   本次改造把底层容器换成 MP4（fMP4 分片格式），切片逻辑简化为
 *   单一条件：累计录像时长 >= 可配置阈值 g_mp4_segment_duration_sec。
 *
 * 【核心设计】
 *   1) 复用 modules/mp4 的 MP4Writer 系列接口（Create/WriteFrame/
 *      Flush/Close/Destroy），不重新造 MP4 封装轮子，也不依赖 ffmpeg；
 *   2) MP4Writer 内部使用 fMP4（ftyp + moov + 循环(moof+mdat) + mfra）
 *      布局：moov 在首帧前写入并 fsync，每个分片的 moof 索引与数据
 *      一起落盘，掉电后已写入分片可独立播放（参考 untrunc 原理）；
 *   3) 小批量帧缓存 + 周期性 fsync 由 MP4WriterConfig 控制：
 *      max_samples=30 / max_bytes=512KB / max_duration_ms=2000 /
 *      sync_interval_ms=2000，适配 SD 卡写入特性；
 *   4) 文件路径：<挂载点>/video/YYYYMMDD/rec_YYYYMMDD_HHMMSS.mp4；
 *   5) 只保留「时长达到阈值」这一种切分片条件，录像类型切换、跨天
 *      等规则暂不参与切分片（由上层业务自行按需调用 need_update_file
 *      强制切文件）；
 *   6) 时间戳归一化：以分片首帧的绝对时间戳(微秒)为 DTS 零点，
 *      后续帧 DTS/PTS = 绝对时间 - 首帧绝对时间，保证 MP4 时间轴单调。
 *
 * 【全局变量入口】
 *   - g_mp4_segment_duration_sec : 切片时长阈值，SDOPS_SetMp4SegmentDuration()
 *
 * 【异常保护】
 *   - NULL 入参 / SD 卡未初始化 / 格式化中 / 不可写：直接返回 -1 或 0，
 *     不做任何 MP4 操作；
 *   - MP4Writer_WriteFrame 返回 -1：立刻 Close+Destroy 当前句柄，
 *     重置内部状态，返回 -1；上层收到错误后停止录像即可；
 *   - CreateDirent / MP4Writer_Create 失败：返回 -1，句柄保持 NULL；
 *   - 句柄释放：切分片/错误/退出时统一 Close+Destroy，并置 NULL，
 *     避免悬挂指针与内存泄漏。
 * 
 * ============================================================================
 */

/* 本文件原有 WriteDataToSdCard 的 AVI 实现已整体替换为下面的 MP4 循环录像版本。 */
/*@breif WriteDataToSdCard MP4 循环录像帧写入
 *@param [IN]  frame_size 一帧数据的大小
 *@param [IN]  frame_buffer 帧数据(H.264 Annex-B 裸流)
 *@param [IN]  frame_head 帧头信息
 *@param [IN]  need_update_file 外部请求强制切文件(可由上层置 1)
 *@param [OUT] avi_file_write_over 当前分片写完毕切出新文件时置 1
 *@param [OUT] out_avi_file 切出的新文件名(avi_file_write_over=1 时有效)
 *@param [OUT] rec_start_time 新分片起始绝对时间(秒)
 *@return 0=成功 / 1=切出新文件 / -1=失败
 *@note  底层 MP4 封装复用 MP4Writer，不做二次编码，适配 SD 卡小批量缓存 + 周期 fsync。
 */
extern int REC_WriteData(int frame_size,                            /* 一帧数据的大小 */
                                      unsigned char * frame_buffer,        /* 纯264数据buffer */
                                      unsigned char *frame_head,           /* 帧头buffer */
                                      unsigned int *need_update_file,      /* 是否需要马上更新文件 */
                                      unsigned int *avi_file_write_over,   /* 当前文件是否写完毕 */
                                      char *out_avi_file,                  /* 新生成的文件名 */
                                      unsigned int *rec_start_time)         /* 新分片录像的开始时间 */
{
    int ret = 0;
    int y = 0, mon = 0, d = 0, h = 0, min = 0, s = 0;
    int64_t      cur_abs_us = 0;       /* 当前帧绝对时间(微秒) */
    int64_t      cur_dts_us = 0;       /* 当前帧相对分片首帧的 DTS(微秒) */
    int64_t      cur_pts_us = 0;       /* 当前帧相对分片首帧的 PTS(微秒) */
    int          is_key = 0;
    unsigned int seg_duration_sec = 0;
    char         record_dir[16] = {0};
    char         base_file_name[MAX_NAME_STR] = {0};
    char         full_path[MAX_NAME_STR] = {0};
    MP4WriterInfo info;
    MP4WriterConfig cfg;

    if (!frame_buffer || !frame_head || !out_avi_file)
    {
        REC_ERR("NULL POINTER EXCEPTION!\n");
        return -1;
    }	

    H264DATAPACKETHEADER *pRawHead = (H264DATAPACKETHEADER *)frame_head;
	
    cur_abs_us = (int64_t)pRawHead->u32RealTimeSec * 1000000LL
               + (int64_t)pRawHead->u32RealTimeUSec;
    is_key = (I_FRAME == pRawHead->FrameType) ? 1 : 0;

    /* ===== 1. 初始化 MP4 时间基线 ===== */
    if (!g_mp4_have_first_frame)
    {
        /* 首帧即作为当前分片的时间零点；后续帧 DTS/PTS 均以此为基准归一化 */
        g_mp4_seg_begin_frame_abs_us = cur_abs_us;
        g_mp4_seg_begin_real_sec     = pRawHead->u32RealTimeSec;
        g_mp4_seg_begin_real_usec    = pRawHead->u32RealTimeUSec;
        g_mp4_have_first_frame = 1;
    }

    cur_dts_us = cur_abs_us - g_mp4_seg_begin_frame_abs_us;
    cur_pts_us = cur_dts_us;  /* 简单场景：PTS = DTS；若 B 帧存在可通过 ctts 处理 */
    if (cur_dts_us < 0)
    {
        /* 时间跳变(对时)兜底：钳为 0 并重置基线，避免 MP4 时间轴倒回 */
        g_mp4_seg_begin_frame_abs_us = cur_abs_us;
        g_mp4_seg_begin_real_sec     = pRawHead->u32RealTimeSec;
        g_mp4_seg_begin_real_usec    = pRawHead->u32RealTimeUSec;
        cur_dts_us = 0;
        cur_pts_us = 0;
    }

    seg_duration_sec = (unsigned int)(cur_abs_us - g_mp4_seg_begin_frame_abs_us) / 1000000LL;

    /* ===== 2. 切分片判断 =====
     * 触发切分片的条件：
     *   a) 上层请求强制切文件（need_update_file=1），且当前分片已写入至少 1 帧
     *   b) 累计时长 >= 配置阈值 g_mp4_segment_duration_sec
     *
     * 注意：在 SD 卡路径的挂载点/目录已变化（如跨天/重启）时，也会在后续
     *       创建新 Writer 时根据当前挂载点重建目录，保证路径正确。
     */
    int need_new_seg = 0;
    if (g_mp4_writer != NULL)
    {
        if ((need_update_file && *need_update_file == 1) ||
            (seg_duration_sec >= g_mp4_segment_duration_sec))
        {
            need_new_seg = 1;
        }
    }

    if (need_new_seg && g_mp4_writer != NULL)
    {
        /* 关闭当前分片：Close 会触发 Flush + 写 mfra + fsync */
        ret = MP4Writer_Close(g_mp4_writer);
        if (ret != 0)
        {
            REC_ERR("MP4Writer_Close failed: %s\n", MP4Writer_GetLastError(g_mp4_writer));
        }
        MP4Writer_Destroy(g_mp4_writer);
        g_mp4_writer = NULL;

        /* 通知上层：当前分片已写完，返回 basename 到 out_avi_file */
        if (g_mp4_curr_file_name[0] != '\0')
        {
            const char *base = strrchr(g_mp4_curr_file_name, '/');
            base = (base != NULL) ? (base + 1) : g_mp4_curr_file_name;
            snprintf(out_avi_file, 128, "%s", base);
            *avi_file_write_over = 1;
        }

        /* 重置分片计数器，下一帧会创建新 Writer */
        g_mp4_have_first_frame = 0;
        *need_update_file = 0;

        REC_INFO("MP4 segment closed, duration=%us, file=%s\n",
                    seg_duration_sec, g_mp4_curr_file_name);
    }

    /* ===== 3. 创建新分片 Writer ===== */
    if (g_mp4_writer == NULL)
    {
        /* 目录结构：<挂载点>/video/YYYYMMDD/ */
        snprintf(record_dir, sizeof(record_dir), "%04d%02d%02d", y, mon, d);
		/*暂时使用写死的，后续SD卡模块调整好了需要增加获取接口到全局变量中*/
        snprintf(full_path, sizeof(full_path), "%s/video/%s",
                 "/tmp/app/disk/mmcblk0p1/", record_dir);
        CreateDirent(full_path);

        /* 文件名：rec_YYYYMMDD_HHMMSS.mp4 */
        snprintf(base_file_name, sizeof(base_file_name),
                 "rec_%04d%02d%02d_%02d%02d%02d.mp4", y, mon, d, h, min, s);
        snprintf(g_mp4_curr_file_name, sizeof(g_mp4_curr_file_name),
                 "%s/%s", full_path, base_file_name);

        /* Writer 初始化参数：宽高传 0，由 MP4Writer 内部从 SPS 自动解析 */
        memset(&info, 0, sizeof(info));
        info.width  = 0;
        info.height = 0;
        info.fps    = 25;
        info.start_time = pRawHead->u32RealTimeSec;

        /* 切片/刷盘配置：嵌入式 SD 卡友好 */
        memset(&cfg, 0, sizeof(cfg));
        cfg.max_samples      = 30;
        cfg.max_bytes        = 512 * 1024;
        cfg.max_duration_ms  = 2000;
        cfg.sync_interval_ms = 2000;

        g_mp4_writer = MP4Writer_Create(g_mp4_curr_file_name, &info, &cfg);
        if (g_mp4_writer == NULL)
        {
            /* Create 失败时句柄为 NULL，无法调用 GetLastError，直接输出文件名 */
            REC_ERR("MP4Writer_Create failed for: %s\n", g_mp4_curr_file_name);

            return -1;
        }

        /* 新分片起始时间 */
        g_mp4_seg_begin_frame_abs_us = cur_abs_us;
        g_mp4_seg_begin_real_sec     = pRawHead->u32RealTimeSec;
        g_mp4_seg_begin_real_usec    = pRawHead->u32RealTimeUSec;
        g_mp4_have_first_frame = 1;

        if (rec_start_time)
        {
            *rec_start_time = g_mp4_seg_begin_real_sec;
        }

        REC_INFO("MP4 segment opened: %s\n", g_mp4_curr_file_name);
    }

    /* ===== 4. 写入当前帧 =====
     * keyframe 传 -1 让 Writer 自动识别 IDR，也可以由 FrameType 直接给出 0/1。
     * 这里显式传递 is_key 以便避免内部解析额外开销。
     */
    ret = MP4Writer_WriteFrame(g_mp4_writer,
                               frame_buffer, frame_size,
                               cur_pts_us, cur_dts_us,
                               is_key);
    if (ret != 0)
    {
        REC_ERR("MP4Writer_WriteFrame failed: %s (size=%d)\n",
                     MP4Writer_GetLastError(g_mp4_writer), frame_size);
        /* 出错时 Close+Destroy，释放资源并防止悬挂指针 */
        MP4Writer_Close(g_mp4_writer);
        MP4Writer_Destroy(g_mp4_writer);
        g_mp4_writer = NULL;
        g_mp4_have_first_frame = 0;		
        return -1;
    }

    return (need_new_seg) ? 1 : 0;
}


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
int REC_SetRecordInterface(const LPRecordInterface pInterface)
{
	if (NULL == pInterface)
	{
		return -1;
	}

	memcpy(&g_RecordInterface,pInterface,sizeof(RecordInterface));
	return 0;
}

static void *REC_GetBufferData(void *lparam)
{
    PRT_TID();
    pthread_detach(pthread_self());

    H264DATAPACKETHEADER *RawHead = NULL;
    H264DATAPACKETHEADER RawHeadTmp;

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

	REC_INFO("create thread GetBufferDataToWriteAvi\n");

    for(;;)
    {
        is_need_sleep = 1;
        if (g_memrecord_getbufdatatoavi_pause_flag && g_b_first_frame)//判断文件写完后再暂停录像
        {
            REC_DBG ("going to pause\n");
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
		    REC_INFO("record exit GetBufferDataToWriteAvi\n");
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

			if (-2 != result && nStreamLen < sizeof(H264DATAPACKETHEADER))
			{
				//循环队列有数据，且数据长度太短
				g_RecordInterface.ifRecordReleaseData(UserHandler, pStreamBuff,nStreamLen);
				continue;
			}
        
            if (-2 != result)  // 缓冲区是否有数据
            {
                is_need_sleep = 0;
                pthread_mutex_lock(&memrecord_write_lock);
                frame_size = nStreamLen - sizeof(H264DATAPACKETHEADER);
                RawHead = &RawHeadTmp;
                memcpy(RawHead,pStreamBuff,sizeof(H264DATAPACKETHEADER));

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
                                REC_INFO("Need auto update file! time [%lu(s) to %lu(s)]\n", 
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
                            REC_INFO("Need auto update file! last [%s] now [%s]\n", 
                                      last_encode_type?"H265":"H264", g_encode_type?"H265":"H264");
#endif
                            g_need_update_file = 1;
                            last_encode_type = g_encode_type;
                        }
#endif
                    }

                    REC_WriteData(RawHead->nOverloadLen,
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
                    REC_ERR("Get frame head from CircQueue checksum frame head fail!\n");
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

		        if (-2 != result && nStreamLen < sizeof(H264DATAPACKETHEADER))
		        {
		        	//循环队列有数据，且数据长度太短
					g_RecordInterface.ifRecordReleaseData(UserHandler, pStreamBuff,nStreamLen);
					continue;
		        }
				//未进行录像时，要保持读序列号增加。
				if (-2 != result)
				{
					RawHead = (H264DATAPACKETHEADER *)pStreamBuff;
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

static ThreadList thread_list[] = {
    {REC_GetBufferData,  NULL, NULL},
};

int REC_Init(void)
{
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

void REC_Exit(void)
{
    REC_INFO("Detect that need reboot, need stop avi recording ...\n");

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
        sem_destroy(&g_semTrigRecordOverCallBackThread);
		sem_destroy(&g_sem_pauseandresume);
        sem_destroy(&g_sem_pauseandresume_feedback);
        g_exit_flag = 0;
    } 
    SAFE_FREE (g_frame_data);
}

static int REC_WaitThreadPause ()
{
    REC_DBG ("\n");

    REC_DBG ("wait for thread GetBufferDataToWriteAvi to pause\n");
    g_memrecord_getbufdatatoavi_pause_flag = 1;
    sem_wait (& g_sem_pauseandresume_feedback);
    REC_DBG ("thread GetBufferDataToWriteAvi paused\n");

    return 0;
}

static int REC_FreeMemUse ()
{
    REC_DBG ("\n");

    if(g_frame_data)
    {
        SAFE_FREE (g_frame_data);
    }

    return 0;
}

static int REC_WaitThreadContinue ()
{
    REC_DBG ("\n");

    g_memrecord_getbufdatatoavi_pause_flag = 0;
    sem_post (& g_sem_pauseandresume);

    return 0;
}

static int REC_ReallocMem ()
{
    REC_DBG ("\n");
	
    return 0;
}

int REC_Destory ()
{
    REC_DBG ("\n");

    if (g_memrecord_pause_flag)
    {
        REC_ERR ("pause while paused\n");
        return -1;
    }

    /* 封锁接口调用 */
    g_memrecord_pause_flag = 1;

    /* 暂停线程 */
    REC_WaitThreadPause ();

    /* 释放内存占用 */
    REC_FreeMemUse ();

    return 0;
}

int REC_Create ()
{
    REC_DBG ("\n");

    /* 恢复内存占用 */
    REC_ReallocMem ();

    /* 恢复线程 */
    REC_WaitThreadContinue ();

    /* 开放接口调用 */
    g_memrecord_pause_flag = 0;

    return 0;
}

/* 开始录像 */
int REC_Start(int record_type, int bH265)
{
    if (0 == g_mem_record_has_inited)
    {
        return -1;
    }

    if (0 == g_need_record)
    {
        g_encode_type = bH265;
        g_need_record = 1;
        REC_INFO("Start to record [%d] [%s]...\n", record_type, bH265?"H265":"H264");
    }
	return 0;
}

int REC_Stop()
{
	if (0 == g_mem_record_has_inited)
    {
        return -1;
    }
	if(1 == g_need_record)
	{
		g_need_record = 0;
	}
	return 0;
}

