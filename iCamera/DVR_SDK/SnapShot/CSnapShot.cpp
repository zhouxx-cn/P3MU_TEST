
/*!
*\file   CSnapShot.cpp 
*\brief  抓图类
*\author 樊聪
*\date   2018年7月26日 18:21:20
*\version 0.3
*/
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>

#include "global_macro.h"
#include "CSnapShot.h"
#include "debug.h"
#include "PAR/CConfig.h"
#include "mysystem.h"

#include "SnapShot_global.h"
#include "common_pub.h"

/* 消除本文件中的snprintf截断警告 */
#pragma GCC diagnostic ignored "-Wformat-truncation"

/**************************************************************************
*                            常量                                     *
**************************************************************************/

/**************************************************************************
*                            宏                                      *
**************************************************************************/

/* unsigned long类型传入，在32位系统下，时间戳上限2^32=4294967296，即北京时间2106/2/7 14:28:16之后会产生溢出 */
#define TIME_ULONG_TO_STRING(ul_time, sz_time)                                       \
    ({                                                                               \
        struct tm tm;                                                                \
        localtime_r((const time_t *)&ul_time, &tm);                                  \
        snprintf(sz_time, 16, "%4d%02d%02d%02d%02d%02d",                             \
        tm.tm_year+1900, tm.tm_mon+1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec); \
})

//#define TEST_TRUE


/**************************************************************************
 *                          数据类型                                    *
 **************************************************************************/

/**************************************************************************
 *                           全局变量                                   *
 **************************************************************************/

/* 源图片文件存放文件夹 */
static char szPicFilePath[64] = {0};

/* 记录上一个时间，避免由于对时造成时间反转 */
static time_t g_LastTimeSec = 0;
static time_t g_LastTimeMs  = 0;

/* 临时备份文件存放文件夹 */
static char szPicFileBakPath_email[64] = {0};
static char szPicFileBakPath_sd[64]    = {0};
static char szPicFileBakPath_ftp[64]   = {0};
static char szPicFileBakPath_push[64]  = {0};

/* 远程文件存放文件夹 */
static char szOpsPicFile_email[128] = {0};
static char szOpsPicFile_ftp[64]    = {0};
static char szOpsPicFile_sd[64]     = {0};
static char szOpsPicFile_push[64]   = {0};

/* 临时抓图编码(分为FTP和SD卡两个，避免出现当出现同一时刻抓图的时候，
 * 前一个文件抓图完毕之后，删掉源文件而导致后面一个事件由于源文件被
 * 删掉而抓图失败，其中gTempCaptureNum[0]为上传到FTP的事件类型，
 * gTempCaptureNum[1]为上传到SD卡的事件类型
 */
static int gTempCaptureNum[2] = {0};

static const char *SnapShotSection = "SnapShot";

CSnapShot *CSnapShot::theCSnapShot = NULL;

/**************************************************************************
 *                           局部函数原型                                 *
 **************************************************************************/

static void* CapSecondTimerThread(void *lparam);

static void* TimedCaptureThread(void *lparam);

#ifdef SAVE_PIC_IN_MEM

static char *picBuff = NULL;
static pthread_mutex_t g_pic_mem_mutex;

typedef struct tagPicInfo
{
    time_t ntime;        // 时间戳(秒)
    time_t utime;        // 时间戳(微秒)
    int startPoint;   // 在缓冲区的起始位置
    int len;          // 图片大小
} onePicInfo, *ponePicInfo;

typedef struct tagPicBuffInfo
{
    char *pBuff;       // 指向缓冲区
    int buffSize;      // 缓冲区的大小
    
    int picCount;      // 已缓冲图片的张数
    int usedSize;      // 已用的缓冲区尺寸
    int unusedSize;    // 剩余的缓冲区尺寸
    
    int writeOffset;   // 写的起始位置
    int readOffset;    // 读的位置

    int writePicIndex; // 下次写第几张图
    onePicInfo  picInfo[SUPPORT_PIC_NUM];
} PicBuffInfo, *pPicBuffInfo;

static PicBuffInfo mem_picInfo;

//插入一张图片
static int mem_inputOnePic(time_t ntime, time_t utime, const char *buffer_data, int nsize)
{
#if defined (__USE_MJPEG_CIRCLE_BUFFER__)
	return 0;
#endif

    int len1 = 0;
    int len2 = 0;
    int overLen = 0;

    pthread_mutex_lock(&g_pic_mem_mutex);

    // 判断整个BUFF是否够放该图片
    if (mem_picInfo.buffSize < nsize)
    {
        printf("Pic size too big, nsize=%d mem buffer=%d\n", nsize, mem_picInfo.buffSize);
        pthread_mutex_unlock(&g_pic_mem_mutex);
        return -1;
    }

    // 循环删除判断
    while(1)
    {
        // 如果空间放得下，则直接跳出
        if (mem_picInfo.unusedSize + mem_picInfo.picInfo[mem_picInfo.writePicIndex].len     >= nsize)
        {
            break;
        }
 
        // 找到要写的上一个文件索引，也就是最旧的一个文件
        mem_picInfo.writePicIndex = (mem_picInfo.writePicIndex + 1) % SUPPORT_PIC_NUM;

        // 判断当前图片数据是否有效
        if (mem_picInfo.picInfo[mem_picInfo.writePicIndex].len > 0)
        {
            // 更新已用的缓冲区尺寸
            mem_picInfo.usedSize -= mem_picInfo.picInfo[mem_picInfo.writePicIndex].len;
            // 更新剩余的缓冲区尺寸
            mem_picInfo.unusedSize += mem_picInfo.picInfo[mem_picInfo.writePicIndex].len;
            // 更新已缓冲图片的张数
            mem_picInfo.picCount --;
            // 淘汰最旧的图片
            memset(&mem_picInfo.picInfo[mem_picInfo.writePicIndex], 0, sizeof(onePicInfo));        

        }
#if 0
        printf("mem_picInfo.usedSize:%d, mem_picInfo.unusedSize:%d, mem_picInfo.picCount:%d, mem_picInfo.writePicIndex:%d\n",
               mem_picInfo.usedSize,
               mem_picInfo.unusedSize,
               mem_picInfo.picCount,
               mem_picInfo.writePicIndex);
        printf("nsize = %d\n", nsize);
        printf("mem_picInfo.writeOffset = %#x\n", mem_picInfo.writeOffset);
#endif
	}

    pthread_mutex_unlock(&g_pic_mem_mutex);

    //==============开始更新信息============
    overLen = mem_picInfo.picInfo[mem_picInfo.writePicIndex].len;
    mem_picInfo.picInfo[mem_picInfo.writePicIndex].len = nsize;    
    mem_picInfo.picInfo[mem_picInfo.writePicIndex].ntime = ntime;
    mem_picInfo.picInfo[mem_picInfo.writePicIndex].utime = utime;
    mem_picInfo.picInfo[mem_picInfo.writePicIndex].startPoint = mem_picInfo.writeOffset;

    //判断写的位置距离缓冲区尾部是否够放
    if (mem_picInfo.writeOffset + nsize > mem_picInfo.buffSize)
    {
        //缓冲区尾部不够放,一部分放到缓冲区头部
        len1 = mem_picInfo.buffSize - mem_picInfo.writeOffset;
        len2 = nsize - len1;
        memcpy(&mem_picInfo.pBuff[mem_picInfo.writeOffset], buffer_data, len1);
        memcpy(&mem_picInfo.pBuff[0], buffer_data + len1, len2);
        mem_picInfo.writeOffset = len2 ;
    }
    else
    {
        //缓冲区尾部够放
        memcpy(&mem_picInfo.pBuff[mem_picInfo.writeOffset], buffer_data, nsize);
        mem_picInfo.writeOffset += nsize ;
    }

    //查询图片是否达到最大张数上限,需要覆盖
    if (SUPPORT_PIC_NUM == mem_picInfo.picCount)
    {
        mem_picInfo.picCount = SUPPORT_PIC_NUM;
        mem_picInfo.usedSize = mem_picInfo.usedSize - overLen + nsize;
        mem_picInfo.unusedSize = mem_picInfo.buffSize - mem_picInfo.usedSize;
        
    }
    else
    {
        mem_picInfo.picCount++;
        mem_picInfo.usedSize = mem_picInfo.usedSize    + nsize;
        mem_picInfo.unusedSize = mem_picInfo.buffSize - mem_picInfo.usedSize;
    }
    if (mem_picInfo.writePicIndex == (SUPPORT_PIC_NUM - 1))
    {
        mem_picInfo.writePicIndex = 0;
    }
    else
    {
        mem_picInfo.writePicIndex++;
    }
    
    pthread_mutex_unlock(&g_pic_mem_mutex);

#if 0
printf("picCount=%d, usedSize=%d, unusedSize=%d, writeOffset=%d, writePicIndex=%d\n",
    mem_picInfo.picCount,
    mem_picInfo.usedSize,
    mem_picInfo.unusedSize,
    mem_picInfo.writeOffset,
    mem_picInfo.writePicIndex);

    int i;
    for(i=0; i<SUPPORT_PIC_NUM; i++)
    {
        printf("pic[%d], ntime=%d, utime=%d, startPoint=%d, len=%d\n", i, 
               mem_picInfo.picInfo[i].ntime, 
               mem_picInfo.picInfo[i].utime, 
               mem_picInfo.picInfo[i].startPoint, 
               mem_picInfo.picInfo[i].len);
    }
#endif

    return 0;
}

//删除所有图片
static int mem_rmAllPic()
{
    pthread_mutex_lock(&g_pic_mem_mutex);
    
    memset(&mem_picInfo, 0, sizeof(PicBuffInfo));
    mem_picInfo.pBuff = picBuff;
    mem_picInfo.picCount = 0;
    mem_picInfo.buffSize = PIC_BUFF_MAX_SIZE;
    mem_picInfo.usedSize = 0;
    mem_picInfo.unusedSize = PIC_BUFF_MAX_SIZE;
    mem_picInfo.writeOffset = 0;
    mem_picInfo.readOffset = 0;
    mem_picInfo.writePicIndex = 0;

    pthread_mutex_unlock(&g_pic_mem_mutex);

    return 0;
}

#if defined (__USE_MJPEG_CIRCLE_BUFFER__)
//获取最近某个时间点的图片
static int mem_getPicByTime(char *buff1, int *len1, time_t *ntime1, time_t *utime1, 
                                    char *buff2, int *len2, time_t *ntime2, time_t *utime2, 
                                    int *needNum, time_t ntime, time_t utime)
{
	int nRet = 0;

    pthread_mutex_lock(&g_pic_mem_mutex);

    switch(*needNum)
    {
        case 1:
			{
				long long timeStampMs = 0;
				if ((nRet = GetMjpegBuffer(0, 0, &buff1, len1, ONE_PIC_MAX_SIZE, &timeStampMs)))
				{
					printf("GetMjpegBuffer Failed, ret=%d\n", nRet);
					return -2;
				}

				*ntime1 = timeStampMs / 1000;
				*utime1 = timeStampMs;
				break;
			}
            
        case 2:
			{
				long long timeStampMs = 0;
				if ((nRet = GetMjpegBuffer(0, 0, &buff1, len1, ONE_PIC_MAX_SIZE, &timeStampMs)))
				{
					printf("[%s %d] GetMjpegBuffer Failed, ret=%d\n", __FUNCTION__, __LINE__, nRet);
					return -2;
				}

				*ntime1 = timeStampMs / 1000;
				*utime1 = timeStampMs;

				if ((nRet = GetMjpegBuffer(0, 0, &buff2, len2, ONE_PIC_MAX_SIZE, &timeStampMs)))
				{
					printf("[%s %d] GetMjpegBuffer Failed, ret=%d\n", __FUNCTION__, __LINE__, nRet);
					return -2;
				}

				*ntime2 = timeStampMs / 1000;
				*utime2 = timeStampMs;
				break;
			}
    }
    
    pthread_mutex_unlock(&g_pic_mem_mutex);

    return 0;
}
#else
//获取最近某个时间点的图片
static int mem_getPicByTime(char *buff1, int *len1, int *ntime1, int *utime1, 
                                    char *buff2, int *len2, int *ntime2, int *utime2, 
                                    int *needNum, int ntime, int utime)
{
    int i = 0, j = 0;
    int len = 0;
    int ret = 0;
    long long tmp_time_ms = 0;
    long long in_time_ms = 0;
    long long hisTimeMs[SUPPORT_PIC_NUM] = {0};

    pthread_mutex_lock(&g_pic_mem_mutex);

    if (mem_picInfo.picCount < *needNum)
    {
        printf("picCount not enough!\n");
        pthread_mutex_unlock(&g_pic_mem_mutex);
        return -1;
    }

    /* 将时间戳转化为毫秒进行计算(注意此处必须先把秒数进行强转，否则会出现溢出) */    
    in_time_ms = (long long)(ntime) * 1000 + utime / 1000;

    for(i=0; i<SUPPORT_PIC_NUM; i++)
    {
        tmp_time_ms = (long long)(mem_picInfo.picInfo[i].ntime)*1000 + mem_picInfo.picInfo[i].utime/1000;
        if(tmp_time_ms)
        {
            hisTimeMs[j++] = tmp_time_ms;
        }
    }

    if(mem_picInfo.picCount > 1)
    {
        qsort(hisTimeMs, mem_picInfo.picCount, sizeof(long long), com_time_stamp);
    }

    j = BinarySearchNearMax(hisTimeMs, mem_picInfo.picCount, in_time_ms);

//printf("ntime = %d, utime = %d, in_time_ms = %lld\n", ntime, utime, in_time_ms);
//printf("hisTimeMs[%lld] [%lld] [%lld], j=%d\n", 
//        hisTimeMs[0], hisTimeMs[1], hisTimeMs[2], j);

    if(j < 0)
    {
        *needNum = 0;
        pthread_mutex_unlock(&g_pic_mem_mutex);
        printf("pic buff is null!\n");
        return -1;
    }

    switch(*needNum)
    {
        case 1:
            for(i=0; i<SUPPORT_PIC_NUM; i++)
            {
                tmp_time_ms = (long long)(mem_picInfo.picInfo[i].ntime)*1000 + mem_picInfo.picInfo[i].utime/1000;
                if(hisTimeMs[j] == tmp_time_ms)
                {
                    if(mem_picInfo.picInfo[i].startPoint + mem_picInfo.picInfo[i].len > mem_picInfo.buffSize)
                    {
                        len = mem_picInfo.buffSize - mem_picInfo.picInfo[i].startPoint;
                        memcpy(buff1, &mem_picInfo.pBuff[mem_picInfo.picInfo[i].startPoint], len);
                        memcpy(buff1 + len, &mem_picInfo.pBuff[0], mem_picInfo.picInfo[i].len - len);
                    }
                    else
                    {
                        memcpy(buff1, &mem_picInfo.pBuff[mem_picInfo.picInfo[i].startPoint], mem_picInfo.picInfo[i].len);
                    }
                    *len1 = mem_picInfo.picInfo[i].len;
                    *ntime1 = mem_picInfo.picInfo[i].ntime;
                    *utime1 = mem_picInfo.picInfo[i].utime;
                    *needNum = 1;
                    break;
                }
            }
            if(SUPPORT_PIC_NUM == i)
            {
                printf("not find hisTime[%d] = %lld\n", j, hisTimeMs[j]);
                *needNum = 0;
                ret = -1;
            }
            
            break;
            
        case 2:
            for(i=0; i<SUPPORT_PIC_NUM; i++)
            {
                tmp_time_ms = (long long)(mem_picInfo.picInfo[i].ntime)*1000 + mem_picInfo.picInfo[i].utime/1000;
                if(hisTimeMs[j] == tmp_time_ms)
                {
                    if(mem_picInfo.picInfo[i].startPoint + mem_picInfo.picInfo[i].len > mem_picInfo.buffSize)
                    {
                        len = mem_picInfo.buffSize - mem_picInfo.picInfo[i].startPoint;
                        memcpy(buff1, &mem_picInfo.pBuff[mem_picInfo.picInfo[i].startPoint], len);
                        memcpy(buff1 + len, &mem_picInfo.pBuff[0], mem_picInfo.picInfo[i].len - len);
                    }
                    else
                    {
                        memcpy(buff1, &mem_picInfo.pBuff[mem_picInfo.picInfo[i].startPoint], mem_picInfo.picInfo[i].len);
                    }
                    *len1 = mem_picInfo.picInfo[i].len;
                    *ntime1 = mem_picInfo.picInfo[i].ntime;
                    *utime1 = mem_picInfo.picInfo[i].utime;

//printf("[%d] *len1=%d, *ntime1=%d, *utime1=%d\n", i, *len1, *ntime1, *utime1);

                    *needNum = 1;
                    break;
                }
            }
            if(SUPPORT_PIC_NUM == i)
            {
                printf("not find hisTime[%d] = %lld\n", j, hisTimeMs[j]);
                *needNum = 0;
                ret = -1;
                break;
            }

            for(i=0; i<SUPPORT_PIC_NUM; i++)
            {
                tmp_time_ms = (long long)(mem_picInfo.picInfo[i].ntime)*1000 + mem_picInfo.picInfo[i].utime/1000;
                if((j >= 1) && (hisTimeMs[j - 1] == tmp_time_ms))
                {
                    if(mem_picInfo.picInfo[i].startPoint + mem_picInfo.picInfo[i].len > mem_picInfo.buffSize)
                    {
                        len = mem_picInfo.buffSize - mem_picInfo.picInfo[i].startPoint;
                        memcpy(buff2, &mem_picInfo.pBuff[mem_picInfo.picInfo[i].startPoint], len);
                        memcpy(buff2 + len, &mem_picInfo.pBuff[0], mem_picInfo.picInfo[i].len - len);
                    }
                    else
                    {
                        memcpy(buff2, &mem_picInfo.pBuff[mem_picInfo.picInfo[i].startPoint], mem_picInfo.picInfo[i].len);
                    }
                    *len2 = mem_picInfo.picInfo[i].len;
                    *ntime2 = mem_picInfo.picInfo[i].ntime;
                    *utime2 = mem_picInfo.picInfo[i].utime;
#if 0
for(int t=0; t<SUPPORT_PIC_NUM; t++)
{
    printf("mem_picInfo.picInfo[%d].len = %d, mem_picInfo.picInfo[%d].ntime = %d, mem_picInfo.picInfo[%d].utime = %d\n", 
            t, mem_picInfo.picInfo[t].len,
            t, mem_picInfo.picInfo[t].ntime,
            t, mem_picInfo.picInfo[t].utime);
}
#endif
                    *needNum = 2;
                    break;
                }
            }
            if(SUPPORT_PIC_NUM == i)
            {
                printf("not find hisTime[%d - 2] = %lld\n", j, hisTimeMs[j - 2]);
                ret = -1;
                break;
            }
            break;
    }
    
    pthread_mutex_unlock(&g_pic_mem_mutex);

    return 0;
}
#endif

//将buff数据写入到文件
static int writebufToFile(const char *buff, int len, const char *fileName)
{
    if((NULL == buff) || (NULL == fileName))
    {
        printf("param is NULL!\n");
        return -1;
    }

    FILE *fp = NULL;
    int nwritelen = 0;
    
    fp = fopen(fileName, "w+");
    if(fp == NULL)
    {
        printf("open file %s failed\n", fileName);
        return -1;
    }
 
    nwritelen = fwrite(buff, len, 1, fp);
    if(nwritelen != 1)
    {
        printf("write file %s fail!\n", fileName);
        fclose(fp);
        char comman[128] = {0};
        snprintf(comman, sizeof(comman), "rm -rf %s", fileName);
        mysystem(comman);
        return -1;
    }
    
    fclose(fp);

    return 0;
}

#endif

CSnapShot::CSnapShot()
{    
    memset(szPicFilePath, 0, sizeof(szPicFilePath));      
    memset(szPicFileBakPath_sd, 0, sizeof(szPicFileBakPath_sd));      
    memset(szPicFileBakPath_ftp, 0, sizeof(szPicFileBakPath_ftp));      
    memset(szPicFileBakPath_email, 0, sizeof(szPicFileBakPath_email));      
    memset(szPicFileBakPath_push, 0, sizeof(szPicFileBakPath_push));      

    memset(szOpsPicFile_sd, 0, sizeof(szOpsPicFile_sd));      
    memset(szOpsPicFile_ftp, 0, sizeof(szOpsPicFile_ftp));      
    memset(szOpsPicFile_email, 0, sizeof(szOpsPicFile_email));      
    memset(szOpsPicFile_push, 0, sizeof(szOpsPicFile_push));      

    m_TimedCaptureCallBack = NULL;

    m_nTimeCount = 0;

    m_timed_capture_id = 0;
    m_sencond_timer_id = 0;

    m_bEnableSdTimedCapture = 0;
    m_nSdTimedCapTimeInterval = MIN_SNAPSHOT_TIME_INTERVAL;

    m_bEnableFTPTimedCapture = 0;
    m_nFTPTimedCapTimeInterval = MIN_SNAPSHOT_TIME_INTERVAL;

}

//构造函数
CSnapShot::CSnapShot(time_t tStartTime, time_t tStopTime, int nChannel)
{
    
}

/* 获取对象实例化 */
CSnapShot *CSnapShot::GetInstance()
{
    if (NULL == theCSnapShot)
    {
#ifndef TEST_MEM_INFO
        theCSnapShot = new CSnapShot;
#else
		theCSnapShot = CommonKit::TCreateObject<CSnapShot>::CreateObject(__FILE__, __FUNCTION__, __LINE__);
#endif

        if (NULL == theCSnapShot)
        {
            TRACE("Get theCSnapShot Instance Fail!\n");
            return NULL;
        }
        if (0 != theCSnapShot)
        {
            theCSnapShot->Init();
        }
    }
    return (CSnapShot *) theCSnapShot;
}

/* 释放实例化指针 */
int CSnapShot::ReleaseInstance()
{
    if (NULL != theCSnapShot)
    {
        theCSnapShot->UnInit();
    
#ifndef TEST_MEM_INFO
        delete theCSnapShot;
#else
		CommonKit::TDestroyObject<CSnapShot>::DestroyObject(theCSnapShot);
#endif

        theCSnapShot = NULL;
    }

    return (int) 0;
}

int CSnapShot::Init()
{
    //获取配置实例
    m_pCConfig = CConfig::GetInstance();
    if (NULL == m_pCConfig)
    {
        TRACE("NULL == m_pCConfig\n");
        return -1;
    }

    LoadParam();

    pthread_mutex_init(&m_cap_mutex, NULL);
    pthread_mutex_init(&m_operate_pic_mutex, NULL);

#if defined (__USE_FTP__) || defined (__SUPPORT_SD_CARD__)
    if (pthread_create(&m_timed_capture_id, NULL, TimedCaptureThread, this))
    {
        TRACE("Fail to create pthread TimedCaptureThread\n");
        return -1;
    }

    if (pthread_create(&m_sencond_timer_id, NULL, CapSecondTimerThread, this))
    {
        TRACE("Fail to create pthread CapSecondTimerThread\n");
        UnInit();
        return -1;
    }
#endif    

#if defined (__USE_EMAIL__) || defined (__USE_FTP__) || defined (__SUPPORT_SD_CARD__)  

#if !defined (__USE_MJPEG_CIRCLE_BUFFER__)
    picBuff = (char *)malloc(PIC_BUFF_MAX_SIZE);
    if (NULL == picBuff)
    {
        TRACE("Fail to malloc size %d\n", PIC_BUFF_MAX_SIZE);
        UnInit();
        return -1;
    }
#endif

#endif

#ifdef SAVE_PIC_IN_MEM
    memset(&mem_picInfo, 0, sizeof(PicBuffInfo));
    mem_picInfo.pBuff = picBuff;
    mem_picInfo.picCount = 0;
    mem_picInfo.buffSize = PIC_BUFF_MAX_SIZE;
    mem_picInfo.usedSize = 0;
    mem_picInfo.unusedSize = PIC_BUFF_MAX_SIZE;
    mem_picInfo.writeOffset = 0;
    mem_picInfo.readOffset = 0;
    mem_picInfo.writePicIndex = 0;

    pthread_mutex_init(&g_pic_mem_mutex, NULL);
#endif

    return 0;
}

int CSnapShot::UnInit()
{
    if (0 != m_timed_capture_id)
    {
        pthread_cancel(m_timed_capture_id);
        pthread_join(m_timed_capture_id, NULL);
        m_timed_capture_id = 0;
    }

    if (0 != m_sencond_timer_id)
    {
        pthread_cancel(m_sencond_timer_id);
        pthread_join(m_sencond_timer_id, NULL);
        m_sencond_timer_id = 0;
    }

    if(picBuff)
    {
        free(picBuff);
        picBuff = NULL;
    }

    return 0;
}

//!\brief   触发搬图
//!\param   nalarm_type  触发联动事件的类型(0:Email 1:FTP 2:SD 3:PUSH)
//!\param   ntime  事件触发的时间(秒)
//!\param   utime  事件触发的时间(微秒)
//!\return  0表示成功，其它为错误码
int CSnapShot::TrigEvent(int nalarm_type, time_t ntime, time_t utime)
{
#if defined (__USE_EMAIL__) || defined (__USE_FTP__) || defined (__SUPPORT_SD_CARD__)  
    if (nalarm_type > 3 || nalarm_type < 0)
    {
        TRACE("nalarm_type %d out of [0,3]\n", nalarm_type);
        return -1;
    }

  //printf("===trig capture picture to tmp dir, nalarm_type=%d===\n", nalarm_type);

    int ret = 0;
	#if defined (__HI3516EV300__)
	char *buff1 = NULL;
#ifndef TEST_MEM_INFO
	buff1 = (char *)malloc(ONE_PIC_MAX_SIZE * sizeof(char));//这里由于EV300抓图是512的大小，导致栈爆掉，所以申请动态的
#else
	buff1 = (char *)MemoryMalloc(ONE_PIC_MAX_SIZE * sizeof(char), "SnapShot", __FILE__, __FUNCTION__, __LINE__);
#endif
	#else
	char buff1[ONE_PIC_MAX_SIZE] = {0};
	#endif
    char tmp_str[32] = {0};
    int len1 = 0;
    time_t ntime1 = 0;
    time_t utime1 = 0;
    int needNum = 1;

    /* 0:Email 1:FTP 2:SD 3:PUSH */
    switch(nalarm_type)
    {
        case 0: // Email (文件名组成: 时间字符串.jpg 例如: 20180727142329.jpg)
        {
            #if defined (__HI3516EV300__)
			char *buff2 = NULL;
#ifndef TEST_MEM_INFO
			buff2 = (char *)malloc(ONE_PIC_MAX_SIZE * sizeof(char));//这里由于EV300抓图是512的大小，导致栈爆掉，所以申请动态的
#else
			buff2 = (char *)MemoryMalloc(ONE_PIC_MAX_SIZE * sizeof(char), "SnapShot", __FILE__, __FUNCTION__, __LINE__);
#endif
			#else
            char buff2[ONE_PIC_MAX_SIZE] = {0};
			#endif
            int len2 = 0;
            time_t ntime2 = 0;
            time_t utime2 = 0;
            char file[64] = {0};

            needNum = 2;
            mem_getPicByTime(buff1, &len1, &ntime1, &utime1, 
                              buff2, &len2, &ntime2, &utime2, 
                              &needNum, ntime, utime);

            pthread_mutex_lock(&m_operate_pic_mutex);
            if(needNum >= 1)
            {
                memset(szOpsPicFile_email, 0, sizeof(szOpsPicFile_email));
                /* 将需要操作的图片文件拷贝到目标文件夹里面 */
                TIME_ULONG_TO_STRING(ntime1, tmp_str);
                snprintf(szOpsPicFile_email, sizeof(szOpsPicFile_email), "%s/%s_01.jpg", szPicFileBakPath_email, tmp_str);
                ret = writebufToFile(buff1, len1, szOpsPicFile_email);
                if(ret)
                {
                    printf("ready file %s fail!\n", szOpsPicFile_email);
                    memset(szOpsPicFile_email, 0, sizeof(szOpsPicFile_email));
                }
            }

            if(2 == needNum)
            {
                memset(tmp_str, 0, sizeof(tmp_str));
                TIME_ULONG_TO_STRING(ntime2, tmp_str);
                snprintf(file, sizeof(file), "%s/%s_02.jpg", szPicFileBakPath_email, tmp_str);
                ret = writebufToFile(buff2, len2, file);
                strlcat(szOpsPicFile_email, " ", 1);
                strlcat(szOpsPicFile_email, file, sizeof(szOpsPicFile_email));
            
                if(ret)
                {
                    printf("ready file %s fail!\n", szOpsPicFile_email);
                    memset(szOpsPicFile_email, 0, sizeof(szOpsPicFile_email));
                }
            }
            pthread_mutex_unlock(&m_operate_pic_mutex);

			#if defined (__HI3516EV300__)
#ifndef TEST_MEM_INFO
			free(buff2);
#else
			MemoryFree(buff2);
#endif
			buff2 = NULL;
			#endif

            break;
        }
        case 1: // 事件触发到FTP (文件名组成: 时间字符串_标号.jpg 例如: 20180727142329_01.jpg)
        {
            needNum = 1;
            mem_getPicByTime(buff1, &len1, &ntime1, &utime1, 
                              NULL, NULL, NULL, NULL, 
                              &needNum, ntime, utime);

            if(1 == needNum)
            {
                pthread_mutex_lock(&m_operate_pic_mutex);
                memset(szOpsPicFile_ftp, 0, sizeof(szOpsPicFile_ftp));
                if (gTempCaptureNum[0]++ >= MAX_TEMP_PIC_NUM_TYPE)
                {
                    gTempCaptureNum[0] = 0;
                }
                /* 将需要操作的图片文件拷贝到目标文件夹里面 */
                TIME_ULONG_TO_STRING(ntime1, tmp_str);
                snprintf(szOpsPicFile_ftp, sizeof(szOpsPicFile_ftp), "%s/%s_%02d.jpg", 
                         szPicFileBakPath_ftp, tmp_str, gTempCaptureNum[0]);
                ret = writebufToFile(buff1, len1, szOpsPicFile_ftp);
                if(ret)
                {
                    printf("ready file %s fail!\n", szOpsPicFile_ftp);
                    memset(szOpsPicFile_ftp, 0, sizeof(szOpsPicFile_ftp));
                }
                memset(szOpsPicFile_ftp, 0, sizeof(szOpsPicFile_ftp));
                snprintf(szOpsPicFile_ftp, sizeof(szOpsPicFile_ftp), "%s_%02d.jpg", tmp_str, gTempCaptureNum[0]);

                pthread_mutex_unlock(&m_operate_pic_mutex);
            }
            break;
        }
        case 2: // SD卡 (文件名组成: 时间戳_标号 例如: 1532477851_01)
        {
            needNum = 1;
            mem_getPicByTime(buff1, &len1, &ntime1, &utime1, 
                              NULL, NULL, NULL, NULL, 
                              &needNum, ntime, utime);
            if(1 == needNum)
            {
                pthread_mutex_lock(&m_operate_pic_mutex);
                memset(szOpsPicFile_sd, 0, sizeof(szOpsPicFile_sd));
                if (gTempCaptureNum[1]++ >= MAX_TEMP_PIC_NUM_TYPE)
                {
                    gTempCaptureNum[1] = 0;
                }

                /* 将需要操作的图片文件拷贝到目标文件夹里面 */
                snprintf(szOpsPicFile_sd, sizeof(szOpsPicFile_sd), "%s/%lld_%02d", 
                         szPicFileBakPath_sd, ntime1, gTempCaptureNum[1]);
                ret = writebufToFile(buff1, len1, szOpsPicFile_sd);

    //          printf("write pic = %s\n", szOpsPicFile_sd);
                memset(szOpsPicFile_sd, 0, sizeof(szOpsPicFile_sd));
                snprintf(szOpsPicFile_sd, sizeof(szOpsPicFile_sd), "%lld_%02d", ntime1, gTempCaptureNum[1]);
    //          printf("get on pic = %s!\n", szOpsPicFile_sd);
                if(ret)
                {
                    printf("ready file %s fail!\n", szOpsPicFile_sd);
                    memset(szOpsPicFile_sd, 0, sizeof(szOpsPicFile_sd));
                }

                pthread_mutex_unlock(&m_operate_pic_mutex);
            }
            break;
        }
        case 3: // 事件触发到PUSH (文件名组成: 时间字符串_标号.jpg 例如: 20180727142329_01.jpg)
        {
            needNum = 1;
            mem_getPicByTime(buff1, &len1, &ntime1, &utime1, 
                              NULL, NULL, NULL, NULL, 
                              &needNum, ntime, utime);

            if(1 == needNum)
            {
                pthread_mutex_lock(&m_operate_pic_mutex);
                memset(szOpsPicFile_push, 0, sizeof(szOpsPicFile_push));
                if (gTempCaptureNum[0]++ >= MAX_TEMP_PIC_NUM_TYPE)
                {
                    gTempCaptureNum[0] = 0;
                }
                /* 将需要操作的图片文件拷贝到目标文件夹里面 */
                TIME_ULONG_TO_STRING(ntime1, tmp_str);
                snprintf(szOpsPicFile_push, sizeof(szOpsPicFile_ftp), "%s/%s_%02d.jpg", 
                         szPicFileBakPath_push, tmp_str, gTempCaptureNum[0]);
                ret = writebufToFile(buff1, len1, szOpsPicFile_push);
                if(ret)
                {
                    printf("ready file %s fail!\n", szOpsPicFile_push);
                    memset(szOpsPicFile_push, 0, sizeof(szOpsPicFile_push));
                }
                memset(szOpsPicFile_push, 0, sizeof(szOpsPicFile_push));
                snprintf(szOpsPicFile_push, sizeof(szOpsPicFile_push), "%s_%02d.jpg", tmp_str, gTempCaptureNum[0]);

                pthread_mutex_unlock(&m_operate_pic_mutex);
            }
            break;
        }
        default:
        {
			#if defined (__HI3516EV300__)
#ifndef TEST_MEM_INFO
			free(buff1);
#else
			MemoryFree(buff1);
#endif
			buff1 = NULL;
			#endif
            return -1;
        }
    }

	#if defined (__HI3516EV300__)
#ifndef TEST_MEM_INFO
	free(buff1);
#else
	MemoryFree(buff1);
#endif
	buff1 = NULL;
	#endif
#endif
    return 0;
}

//!\brief   获取操作的文件名
//!\param   nalarm_type  触发联动事件的类型(0:Email 1:FTP 2:SD 3:PUSH)
//!\return  0表示成功，其它为错误码
int CSnapShot::GetOpsPicFileName(int nalarm_type, char *pic_file)
{
    if (!pic_file)
    {
        TRACE("[%s-%d] NULL POINTER EXCEPTION!\n", __FILE__, __LINE__);
        return -1;
    }

    if (nalarm_type > 3 || nalarm_type < 0)
    {
        TRACE("nalarm_type %d out of [0,3]\n", nalarm_type);
        return -1;
    }

    pthread_mutex_lock(&m_operate_pic_mutex);

    switch(nalarm_type)
    {
        case 0:
        {
            strncpy(pic_file, szOpsPicFile_email, sizeof(szOpsPicFile_email) - 1);
            break;
        }
        case 1:
        {
            strncpy(pic_file, szOpsPicFile_ftp, sizeof(szOpsPicFile_ftp) - 1);
            break;
        }
        case 2:
        {
            strncpy(pic_file, szOpsPicFile_sd, sizeof(szOpsPicFile_sd) - 1);
            break;
        }
        case 3:
        {
            strncpy(pic_file, szOpsPicFile_push, sizeof(szOpsPicFile_push) - 1);
            break;
        }
        
        default:
        {    
            pthread_mutex_unlock(&m_operate_pic_mutex);
            return -1;
        }
    } 

    pthread_mutex_unlock(&m_operate_pic_mutex);
    //printf("GetOpsPicFileName====nalarm_type=%d, pic_file=%s\n", nalarm_type, pic_file);

    return 0;
}

//!\brief 生成一个快照文件
//!\param char *buffer_data 快照的数据，将该buff直接写入文件。
//!\param time_t ntime 时间(秒)
//!\param time_t utime 时间(微秒)
//!\param int nsize    快照buff的长度
//!\return  成功为0，失败为-1
int CSnapShot::CapturePic(time_t ntime, time_t utime, char *buffer_data, int nsize)
{
    if (NULL == buffer_data)
    {
        TRACE("buffer_data == NULL\n");
        return -1;
    }
#if defined (__USE_EMAIL__) || defined (__USE_FTP__) || defined (__SUPPORT_SD_CARD__)
    pthread_mutex_lock(&m_cap_mutex);

	/* 时间戳与上一帧相同则直接返回 */
	if ((g_LastTimeSec == ntime) && (g_LastTimeMs==utime))
	{
		pthread_mutex_unlock(&m_cap_mutex);
        return 0;
	}

    /* 如果时间发生翻转，则需要删除原来的所有图片，重新开始抓图 */
    if (g_LastTimeSec > ntime)
    {
        mem_rmAllPic();
    }
    g_LastTimeSec = ntime;
    g_LastTimeMs  = utime;

    mem_inputOnePic(ntime, utime, buffer_data, nsize);
    pthread_mutex_unlock(&m_cap_mutex);
#endif
    return 0;
}

/*@breif SetSnapShotParam 设置SD卡抓图参数
 *@param [IN] bEnable 是否使能SD卡抓图
 *@param [IN] nTimeInterval 抓图的时间间隔
 *@return 成功返回0
 *@       失败返回-1
 *@note   抓图时间间隔范围在MIN_SNAPSHOT_TIME_INTERVAL和MAX_SNAPSHOT_TIME_INTERVAL之间
 */
int CSnapShot::SetSdSnapShotParam(int bEnable, int nTimeInterval)
{
    nTimeInterval = nTimeInterval < MIN_SNAPSHOT_TIME_INTERVAL ? MIN_SNAPSHOT_TIME_INTERVAL : nTimeInterval;
    nTimeInterval = nTimeInterval > MAX_SNAPSHOT_TIME_INTERVAL ? MAX_SNAPSHOT_TIME_INTERVAL : nTimeInterval;

    if ((m_nSdTimedCapTimeInterval==nTimeInterval) && (m_bEnableSdTimedCapture==bEnable))
    {
        TRACE("Sd card snapshot param not change!\n");
        return 0;
    }

    m_pCConfig->SetValue(SnapShotSection, "EnableSdTimedCapture",   bEnable);
    m_pCConfig->SetValue(SnapShotSection, "SdTimedCapTimeInterval", nTimeInterval);

    m_bEnableSdTimedCapture = bEnable;
    m_nSdTimedCapTimeInterval = nTimeInterval;

    m_nTimeCount = 0;

    return 0;
}

/*@breif GetSdSnapShotParam 获取SD卡抓图参数
 *@param [OUT] pEnable 是否使能SD卡抓图
 *@param [OUT] pTimeInterval 抓图的时间间隔
 *@return 成功返回0
 *@       失败返回-1
 *@note   抓图时间间隔范围在MIN_SNAPSHOT_TIME_INTERVAL和MAX_SNAPSHOT_TIME_INTERVAL之间
 */
int CSnapShot::GetSdSnapShotParam(int *pEnable, int *pTimeInterval)
{
    if (NULL==pEnable || NULL==pTimeInterval)
    {
        TRACE("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    *pEnable = m_bEnableSdTimedCapture;
    *pTimeInterval = m_nSdTimedCapTimeInterval;

    return 0;
}

/*@breif SetFTPSnapShotParam 设置FTP抓图参数
 *@param [IN] bEnable 是否使能FTP抓图
 *@param [IN] nTimeInterval 抓图的时间间隔
 *@return 成功返回0
 *@       失败返回-1
 *@note   抓图时间间隔范围在MIN_SNAPSHOT_TIME_INTERVAL和MAX_SNAPSHOT_TIME_INTERVAL之间
 */
int CSnapShot::SetFTPSnapShotParam(int bEnable, int nTimeInterval)
{
    nTimeInterval = nTimeInterval < MIN_SNAPSHOT_TIME_INTERVAL ? MIN_SNAPSHOT_TIME_INTERVAL : nTimeInterval;
    nTimeInterval = nTimeInterval > MAX_SNAPSHOT_TIME_INTERVAL ? MAX_SNAPSHOT_TIME_INTERVAL : nTimeInterval;

    if ((m_nFTPTimedCapTimeInterval==nTimeInterval) && (m_bEnableFTPTimedCapture==bEnable))
    {
        TRACE("FTP snapshot param not change!\n");
        return 0;
    }

    m_pCConfig->SetValue(SnapShotSection, "EnableFTPTimedCapture",   bEnable);
    m_pCConfig->SetValue(SnapShotSection, "FTPTimedCapTimeInterval", nTimeInterval);

    m_bEnableFTPTimedCapture = bEnable;
    m_nFTPTimedCapTimeInterval = nTimeInterval;

    m_nTimeCount = 0;

    return 0;
}

/*@breif GetFTPSnapShotParam 获取FTP抓图参数
 *@param [OUT] pEnable 是否使能FTP抓图
 *@param [OUT] pTimeInterval 抓图的时间间隔
 *@return 成功返回0
 *@       失败返回-1
 *@note   抓图时间间隔范围在MIN_SNAPSHOT_TIME_INTERVAL和MAX_SNAPSHOT_TIME_INTERVAL之间
 */
int CSnapShot::GetFTPSnapShotParam(int *pEnable, int *pTimeInterval)
{
    if (NULL==pEnable || NULL==pTimeInterval)
    {
        TRACE("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    *pEnable = m_bEnableFTPTimedCapture;
    *pTimeInterval = m_nFTPTimedCapTimeInterval;

    return 0;
}

#if defined (__USE_FTP__) || defined (__SUPPORT_SD_CARD__)
/* 秒钟计时器线程 */
int CSnapShot::CapSecondTimerThreadProcess()
{
    while(1)
    {
        /* 由于是unsigned int类型，故不需要设置上限，让其自动翻转 */
        m_nTimeCount ++;    
        sleep(1);
    }
    return 0;
}

static void* CapSecondTimerThread(void *lparam)
{
	prctl(PR_SET_NAME,"CapSecondTimerThread");
    PRT_TID();

    //获取当前实例
    CSnapShot *pCSnapShot = (CSnapShot *)lparam;

    if (NULL != pCSnapShot)
    {
       pCSnapShot->CapSecondTimerThreadProcess();
    }
    else
    {
        TRACE("get pCSnapShot instance fail\n");
    }

    return NULL;
}

/* 定时抓图线程 */
int CSnapShot::TimedCaptureThreadProcess()
{
    while(1)
    {
        #ifdef __USE_FTP__
        if (m_bEnableFTPTimedCapture)
        {
            m_nFTPTimedCapTimeInterval = m_nFTPTimedCapTimeInterval 
                                       < MIN_SNAPSHOT_TIME_INTERVAL ? MIN_SNAPSHOT_TIME_INTERVAL : m_nFTPTimedCapTimeInterval;
            if (0 == m_nTimeCount % m_nFTPTimedCapTimeInterval)
            {
                /* 触发FTP定时抓图回调 */
                if (m_TimedCaptureCallBack)
                {
                    m_TimedCaptureCallBack(1);
                }
            }
        }
        #endif

        #ifdef __SUPPORT_SD_CARD__
        if (m_bEnableSdTimedCapture)
        {
            m_nSdTimedCapTimeInterval = m_nSdTimedCapTimeInterval 
                                      < MIN_SNAPSHOT_TIME_INTERVAL ? MIN_SNAPSHOT_TIME_INTERVAL : m_nSdTimedCapTimeInterval;
            if (0 == m_nTimeCount % m_nSdTimedCapTimeInterval)
            {
                /* 触发SD卡定时抓图回调 */
                if (m_TimedCaptureCallBack)
                {
                    m_TimedCaptureCallBack(2);
                }
            }
        }
        #endif

        sleep(1);
    }

    return 0;
}

static void* TimedCaptureThread(void *lparam)
{
	prctl(PR_SET_NAME,"TimedCaptureThread");
    PRT_TID();

    //获取当前实例
    CSnapShot *pCSnapShot = (CSnapShot *)lparam;

    if (NULL != pCSnapShot)
    {
        pCSnapShot->TimedCaptureThreadProcess();
    }
    else
    {
        TRACE("get pCSnapShot instance fail\n");
    }

    return NULL;
}
#endif

CConfig *CSnapShot::GetConfig()
{
    return m_pCConfig;
}

int CSnapShot::LoadParam()
{
    m_bEnableSdTimedCapture = m_pCConfig->GetValue(SnapShotSection, "EnableSdTimedCapture", 0);
    m_nSdTimedCapTimeInterval = m_pCConfig->GetValue(SnapShotSection, "SdTimedCapTimeInterval", 10);

    m_bEnableFTPTimedCapture = m_pCConfig->GetValue(SnapShotSection, "EnableFTPTimedCapture", 0);
    m_nFTPTimedCapTimeInterval = m_pCConfig->GetValue(SnapShotSection, "FTPTimedCapTimeInterval", 10);

    CConfig *pCfg = CConfig::NewInstance();
    int nRet = pCfg->LoadFileRO(CUSTOM_CONFIG);
    if (0 == nRet)
    {
        pCfg->GetValue("SnapShot", "PicFilePath", "/tmp/picture/", szPicFilePath, sizeof(szPicFilePath));
        pCfg->GetValue("SnapShot", "PicFileBakPath_sd", "/tmp/picture/pic_for_sd/", szPicFileBakPath_sd, sizeof(szPicFileBakPath_sd));
        pCfg->GetValue("SnapShot", "PicFileBakPath_ftp", "/tmp/picture/pic_for_ftp/", szPicFileBakPath_ftp, sizeof(szPicFileBakPath_ftp));
        pCfg->GetValue("SnapShot", "PicFileBakPath_email", "/tmp/picture/pic_for_email/", szPicFileBakPath_email, sizeof(szPicFileBakPath_email));
        pCfg->GetValue("SnapShot", "PicFileBakPath_push", "/tmp/picture/pic_for_push/", szPicFileBakPath_push, sizeof(szPicFileBakPath_push));
    }
    CConfig::ReleaseInstance(pCfg);

    if (access(szPicFilePath, F_OK)) {  mkdir(szPicFilePath, 0777);  }
    if (access(szPicFileBakPath_sd, F_OK)) {  mkdir(szPicFileBakPath_sd, 0777);  }
    if (access(szPicFileBakPath_ftp, F_OK)) {  mkdir(szPicFileBakPath_ftp, 0777);  }
    if (access(szPicFileBakPath_email, F_OK)) {  mkdir(szPicFileBakPath_email, 0777);  }
    if (access(szPicFileBakPath_push, F_OK)) {  mkdir(szPicFileBakPath_push, 0777);  }

    return 0;
}

/* 定时时间到达之后，触发抓图回调 */
int CSnapShot::SetTimedCaptureCallBack(int (*CallBack)(int capture_type))
{
    if (NULL == CallBack)
    {
        TRACE("NULL POINTER EXCEPTION!\n");
        return -1;        
    }

    m_TimedCaptureCallBack = CallBack;

    return 0;
}
