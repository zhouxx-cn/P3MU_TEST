
/*!
*\file		MPU_CIU_Playback.cpp
*\brief	MPU模块，远程回放的实现
*\author	周庆丰
*\date		2009/08/4

功能：
-#	
-#	远程回放

说明：
-#	 
*/
#include <sys/stat.h>
#include "MPU_CIU_global.h"
#include "MPU_CIU_Playback.h"
#ifdef __SUPPORT_SD_CARD__
#include "STR/CDPManager.h"
#endif
#include "AVL/enumRecType.h"
#include "pkg_interface.h"
#include "AVL_global.h"

/* 消除本文件中的snprintf截断警告 */
#pragma GCC diagnostic ignored "-Wformat-truncation"

#define ERR_PLAYBACK_OK      ((int)eQueryOK)	/*!< 查询成功                               */
#define ERR_PLAYBACK_FAILED      ((int)eQueryFailed)	/*!< 查询失败                               */
#define ERR_PLAYBACK_BUSY      ((int)eQueryBusy)	/*!< 查询忙                                 */
#define ERR_PLAYBACK_FINISHED      ((int)eQueryFinished)	/*!< 查询结束                               */

//检查历史通道句柄是否有效，并返回历史通道索引
#define	RPB_CHK_HistoryChannel_HANDLE(handle, index)				\
do																\
{																\
	DWORD mark;													\
																\
	MPU_INFO("entry handle<0x%08x>\n", (int) (handle));				\
																\
	mark = ((DWORD) (handle)) >> PLAYBACK_MARK_SHIFT;				\
	(index) = ((DWORD) (handle)) & PLAYBACK_INDEX_MASK;				\
																\
	if (((DWORD)(index)) >= MAX_PLAYBACK_SUPPORT)					\
	{															\
		printf("invalid handle<0x%08x> index -> %lu\n", (int) (handle), (DWORD)(index));	\
		return ERR_PLAYBACK_FAILED;								\
	}															\
																\
	if (mark != m_PlaybackCfg[index].dwHandeVerfyMark)				\
	{															\
		printf("invalid handle<0x%08x> mark -> 0x%08x\n", (int) (handle), (int) m_PlaybackCfg[index].dwHandeVerfyMark);	\
		return ERR_PLAYBACK_FAILED;								\
	}															\
																\
	if (m_PlaybackCfg[index].hPlayback == NULL)						\
	{															\
		printf("invalid handle<0x%08x> not create\n", (int) (handle));	\
		return ERR_PLAYBACK_FAILED;								\
	}															\
}																\
while (0)

//检查历史查询句柄是否有效，并返回历史查询索引
#define	RPB_CHK_HistoryQuery_HANDLE(handle, index)				\
do																\
{																\
	DWORD mark;													\
																\
																\
	mark = ((DWORD) (handle)) >> PLAYBACK_MARK_SHIFT;				\
	(index) = ((DWORD) (handle)) & PLAYBACK_INDEX_MASK;				\
																\
	if (((DWORD)(index)) >= MAX_PLAYBACK_SUPPORT)					\
	{															\
		printf("invalid handle<0x%08x> index -> %lu\n", (int) (handle), (DWORD)(index));	\
		return ERR_PLAYBACK_FAILED;								\
	}															\
																\
	if (mark != m_PlaybackCfg[index].dwQureyHandeVerfyMark)			\
	{															\
		printf("invalid handle<0x%08x> mark -> 0x%08x\n", (int) (handle), (int) m_PlaybackCfg[index].dwQureyHandeVerfyMark);	\
		return ERR_PLAYBACK_FAILED;								\
	}															\
																\
	if (m_PlaybackCfg[index].hPlayback_info == NULL)					\
	{															\
		printf("invalid handle<0x%08x> not create\n", (int) (handle));	\
		return ERR_PLAYBACK_FAILED;								\
	}															\
}																\
while (0)

/* 字符串时间格式为"yearmondayhourminsec",传出的时间戳为unsigned long类型 
 * 海思的系统库里面的strptime函数有BUG，不能用, 故用sscanf代替
 */
#define TIME_STRING_TO_STAMP(string_time, ul_time)\
    ({\
        struct tm tmTime = {0};\
        sscanf(string_time, "%4d%2d%2d%2d%2d%2d",\
              &tmTime.tm_year, &tmTime.tm_mon, &tmTime.tm_mday,\
              &tmTime.tm_hour, &tmTime.tm_min, &tmTime.tm_sec);\
        tmTime.tm_year -= 1900;\
        tmTime.tm_mon--;\
        tmTime.tm_isdst = -1;\
        ul_time = mktime(&tmTime);\
    })

typedef struct _PLAYBACK_CFG_
{
	HANDLE hPlayback;			//录像数据弹出句柄
	DWORD dwHandeVerfyMark;		//用于验证当前句柄有效性
	DWORD dwRec_type;			//录像数据类型
	DWORD dwStreamChannel;		//数据通道
	DWORD dwStreamDiskGroup;	//数据盘组
	DWORD dwDataQuit;			//数据弹出是否退出
	DWORD dwUserID;				//用户ID
	pthread_t m_hStreamThread;	//发送流线程
	sem_t m_sem_Stream;			//发送流同步
	sem_t m_sem_StreamSend;		//发送流同步1
	DWORD dwIFrame;				//0-normal ，1-只传I帧,当前
	DWORD dwSetIFrame;			//0-normal ，1-只传I帧,将要设置的
	DWORD dwFrameNo;			//帧号
	time_t tStreamStartTime;	//流开始时间
	time_t tStreamStopTime;		//流结束时间
	time_t tStreamEndTime;		//流弹出结束时间
	DWORD dwStreamStart;		//开始流
	DWORD dwStreamSeek;			//Seek流
	DWORD dwStreamStop;			//停止流
	DWORD dwStreamStatus;		//0-停止,1-开始
	DWORD dwDataFill;			//数据是否填充
	HistoryChannelStream tStreamData;	//历史流数据

#ifdef USE_MM
	BYTE *pStream;				//历史流缓冲指针
	DWORD dwStreamSize;			//缓冲大小
#endif

	//////////////////////////////////////////////////////////////////////////
	//片段查询
	HANDLE hPlayback_info;		//录像片段信息弹出句柄
	DWORD dwQureyHandeVerfyMark;	//用于验证当前句柄有效性
	DWORD dwHandle;
	char whichDay[9];
	DWORD dwUserID_info;		//版本信息查询用户ID
	DWORD dwSegDiskGroup;		//片段盘组
	DWORD dwSegChannel;			//片段通道
	time_t tSegStartTime;		//片段开始时间
	time_t tSegStopTime;		//片段结束时间
	DWORD dwInfo_type;			//片段信息类型
	struct tagRecSeg *pRecSegHead[MAX_SUPPORT_RECTYPE];	//片段链表头,支持6种录像类型
	struct tagRecSeg *pRecSegNext;	//片段链Next
	DWORD dwRecSegCount[MAX_SUPPORT_RECTYPE];	//片段个数

    //多通道录像数据推送
    unsigned char m_IsInsertCh[DVR_MAX_VIDEOIN_NUM];            //通道是否已插入
    unsigned char m_FirstNeedIFrameCh[DVR_MAX_VIDEOIN_NUM];     //通道首帧需要I帧
} PLAYBACK_CFG;

//存放片段的数目
typedef struct record_file_header_s
{
	char count_of_recordset[8];
}record_file_header_t;
//存放每个片段信息
typedef struct record_node_s
{
	char stream_type[8];
	char channel[8];
	char begin_time[15];
	char end_time[15];
	char data_size[32];
}record_node_t;

//创建流 线程句柄
static pthread_t m_hCreateThread;

static PLAYBACK_CFG m_PlaybackCfg[MAX_PLAYBACK_SUPPORT];

static sem_t m_sem_create;
static sem_t m_sem_queryOver;
static int m_mark_cnt = 0;

static int DataPop_Callback(HANDLE hDataPopper,
                                 int bIndexFrame, 
                                 unsigned short wFrameType, 
                                 time_t FrameTime, 
                                 unsigned char *pBuf,
                                 unsigned long dwSize, 
                                 unsigned long EventID, 
                                 void *pContext);

static void *RPB_StreamThread(void *lParam);


//检查片段起始时间跟终止时间是否正常
static int segCheck(time_t beginTime, time_t endTime)
{
    if(endTime - beginTime > MAX_SEG_TIME)//一个片段操作一天时间
    {
        printf("Seg too big, begin time %lld, end time %lld\n", beginTime, endTime);
        return 1;
    }
    else if(endTime < beginTime)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

//创建历史流通道
static int HY_DVR_HistoryChannelCreateEx(HANDLE * hIndex, DWORD dwUserID, const LPHistoryChannelParaInfoMultiType pHistoryChannelParaInfo)
{
#ifndef DEBUG_NOT_USE_STR_LIB
	int i;

	int CurrIndex;

	int result;

	time_t Mark_time;

	time_t use_time;

	struct tm stm;

	MPU_DBG("entry ID<%d>\n", (int) dwUserID);

	use_time = time(NULL);

	//检测历史通道句柄指针是否有效
	if (hIndex == NULL)
	{
		printf("[%s-%d] hIndex == NULL\n", __FUNCTION__, __LINE__);
		return ERR_PLAYBACK_FAILED;
	}

	//检测历史通道号是否有效 (0 - 15有效)
	if (pHistoryChannelParaInfo->dwChannel < 0 || pHistoryChannelParaInfo->dwChannel >= DVR_MAX_VIDEOIN_NUM)
	{
		printf("[%s-%d] ch = %d invalid\n", __FUNCTION__, __LINE__, (int) pHistoryChannelParaInfo->dwChannel);
		return ERR_PLAYBACK_FAILED;
	}

	//检测盘组编号是否有效
	if (pHistoryChannelParaInfo->dwDiskGroup <= eDiskGroupTypeBegin || pHistoryChannelParaInfo->dwDiskGroup >= eDiskGroupTypeEnd)
	{
		printf("[%s-%d] diskgroup = %d invalid\n", __FUNCTION__, __LINE__, (int) pHistoryChannelParaInfo->dwDiskGroup);
		return ERR_PLAYBACK_FAILED;
	}

	//检测用户是否有远程回放权限
	if (!MPU_CIU_CheckRemoteUser(dwUserID, CHANNEL_PLAYBACK, pHistoryChannelParaInfo->dwChannel))
	{
		printf("[%s-%d] ID = %d ch = %d no right\n", __FUNCTION__, __LINE__, (int) dwUserID, (int) pHistoryChannelParaInfo->dwChannel);
		return IDVR_CIU_NO_RIGHT;
	}

	//搜索空闲的历史通道索引
	CurrIndex = -1;
	for (i = 0; i < MAX_PLAYBACK_SUPPORT; i++)
	{
        if ((NULL==m_PlaybackCfg[i].hPlayback_info) && (NULL==m_PlaybackCfg[i].hPlayback))
		{
			CurrIndex = i;
			break;
		}
	}

	//没有空闲的历史通道则返回失败
	if (CurrIndex == -1)
	{
		printf("[%s-%d] too much history channel create\n", __FUNCTION__, __LINE__);
		return ERR_PLAYBACK_FAILED;
	}

	//转换查询开始时间
	stm.tm_year = pHistoryChannelParaInfo->cBeginTime.wYear - 1900;
	stm.tm_mon = pHistoryChannelParaInfo->cBeginTime.wMonth - 1;
	stm.tm_mday = pHistoryChannelParaInfo->cBeginTime.wDay;
	stm.tm_hour = pHistoryChannelParaInfo->cBeginTime.wHour;
	stm.tm_min = pHistoryChannelParaInfo->cBeginTime.wMinute;
	stm.tm_sec = pHistoryChannelParaInfo->cBeginTime.wSecond;
	m_PlaybackCfg[CurrIndex].tStreamStartTime = mktime(&stm);

	//转换查询解释时间
	if (pHistoryChannelParaInfo->dwEnableEndTime)
	{
		stm.tm_year = pHistoryChannelParaInfo->cEndTime.wYear - 1900;
		stm.tm_mon = pHistoryChannelParaInfo->cEndTime.wMonth - 1;
		stm.tm_mday = pHistoryChannelParaInfo->cEndTime.wDay;
		stm.tm_hour = pHistoryChannelParaInfo->cEndTime.wHour;
		stm.tm_min = pHistoryChannelParaInfo->cEndTime.wMinute;
		stm.tm_sec = pHistoryChannelParaInfo->cEndTime.wSecond;
		m_PlaybackCfg[CurrIndex].tStreamStopTime = mktime(&stm);
	}
	else
	{
#if 1
		//选择当天结束时间
		stm.tm_year = pHistoryChannelParaInfo->cBeginTime.wYear - 1900;
		stm.tm_mon = pHistoryChannelParaInfo->cBeginTime.wMonth - 1;
		stm.tm_mday = pHistoryChannelParaInfo->cBeginTime.wDay;
		stm.tm_hour = 23;
		stm.tm_min = 59;
		stm.tm_sec = 59;
		m_PlaybackCfg[CurrIndex].tStreamStopTime = mktime(&stm);
#else
		m_PlaybackCfg[CurrIndex].tStreamStopTime = 0;	//无限
#endif
	}

	//设置历史通道配置
	memset(&m_PlaybackCfg[CurrIndex].tStreamData, 0, sizeof (HistoryChannelStream));
	m_PlaybackCfg[CurrIndex].tStreamEndTime = 0;
	m_PlaybackCfg[CurrIndex].dwUserID = dwUserID;
	m_PlaybackCfg[CurrIndex].dwDataQuit = 0;
	m_PlaybackCfg[CurrIndex].dwStreamChannel = pHistoryChannelParaInfo->dwChannel;
	m_PlaybackCfg[CurrIndex].dwRec_type = pHistoryChannelParaInfo->dwType;
	m_PlaybackCfg[CurrIndex].dwDataFill = 0;
	m_PlaybackCfg[CurrIndex].dwStreamStart = 0;
	m_PlaybackCfg[CurrIndex].dwStreamStop = 0;
	m_PlaybackCfg[CurrIndex].dwStreamSeek = 0;
	m_PlaybackCfg[CurrIndex].dwStreamStatus = 0;
	m_PlaybackCfg[CurrIndex].dwIFrame = 0;
	m_PlaybackCfg[CurrIndex].dwSetIFrame = 0;
    m_PlaybackCfg[CurrIndex].m_IsInsertCh[pHistoryChannelParaInfo->dwChannel] = 1;
    m_PlaybackCfg[CurrIndex].m_FirstNeedIFrameCh[pHistoryChannelParaInfo->dwChannel] = 1;

	switch (pHistoryChannelParaInfo->dwDiskGroup)
	{
		case eDiskGroupNormal:
			{
				m_PlaybackCfg[CurrIndex].dwStreamDiskGroup = TypeDiskGroupNormal;
				break;
			}
		case eDiskGroupAlarm:
			{
				m_PlaybackCfg[CurrIndex].dwStreamDiskGroup = TypeDiskGroupAlarm;
				break;
			}
		case eDiskGroupRedundance:
			{
				m_PlaybackCfg[CurrIndex].dwStreamDiskGroup = TypeDiskGroupRedundance;
				break;
			}
		case eDiskGroupBackup:
			{
				m_PlaybackCfg[CurrIndex].dwStreamDiskGroup = TypeDiskGroupBackup;
				break;
			}
		default:
			{
				m_PlaybackCfg[CurrIndex].dwStreamDiskGroup = TypeDiskGroupNormal;
				break;
			}
	}

	//修改操作码计数器
	++m_mark_cnt;
	if (m_mark_cnt > 127)
	{
		m_mark_cnt = 0;
	}

	//生成操作效验码
	Mark_time = time(NULL);
	m_PlaybackCfg[CurrIndex].dwHandeVerfyMark = (((Mark_time & 0xFFFF) | 0xC000) << 8) | m_mark_cnt;

	//生成历史通道句柄
	*hIndex = m_PlaybackCfg[CurrIndex].hPlayback = (HANDLE) ((m_PlaybackCfg[CurrIndex].dwHandeVerfyMark << PLAYBACK_MARK_SHIFT) | CurrIndex);

	//创建流线程信号
	sem_init(&m_PlaybackCfg[CurrIndex].m_sem_Stream, 0, 0);
	sem_init(&m_PlaybackCfg[CurrIndex].m_sem_StreamSend, 0, 0);
	//创建流线程
	result = pthread_create(&m_PlaybackCfg[CurrIndex].m_hStreamThread, NULL, RPB_StreamThread, (void *) m_PlaybackCfg[CurrIndex].hPlayback);
	if (0 != result)
	{
		printf("[%s-%d] call pthread_create( RPB_StreamThread ) failed\n", __FUNCTION__, __LINE__);
		sem_destroy(&m_PlaybackCfg[CurrIndex].m_sem_Stream);
		sem_destroy(&m_PlaybackCfg[CurrIndex].m_sem_StreamSend);

		//清除历史通道管理信息
		m_PlaybackCfg[CurrIndex].hPlayback = NULL;

		return ERR_PLAYBACK_FAILED;
	}

	sem_post(&m_sem_create);	//通知创建 

	use_time = time(NULL) - use_time;

	MPU_DBG("handle<0x%08x>, DiskGroup<%lu>, Channel<%lu>, Time<%d-%d-%d %d:%d:%d -> (EN:%d)%d-%d-%d %d:%d:%d> use %lu s\n",
			 (int) *hIndex, pHistoryChannelParaInfo->dwDiskGroup, pHistoryChannelParaInfo->dwChannel,
			 (int) pHistoryChannelParaInfo->cBeginTime.wYear,
			 (int) pHistoryChannelParaInfo->cBeginTime.wMonth,
			 (int) pHistoryChannelParaInfo->cBeginTime.wDay,
			 (int) pHistoryChannelParaInfo->cBeginTime.wHour,
			 (int) pHistoryChannelParaInfo->cBeginTime.wMinute,
			 (int) pHistoryChannelParaInfo->cBeginTime.wSecond,
			 (int) m_PlaybackCfg[CurrIndex].tStreamStopTime,
			 (int) pHistoryChannelParaInfo->cEndTime.wYear,
			 (int) pHistoryChannelParaInfo->cEndTime.wMonth,
			 (int) pHistoryChannelParaInfo->cEndTime.wDay,
			 (int) pHistoryChannelParaInfo->cEndTime.wHour,
			 (int) pHistoryChannelParaInfo->cEndTime.wMinute, (int) pHistoryChannelParaInfo->cEndTime.wSecond, use_time);

	return ERR_PLAYBACK_OK;
#else
	return ERR_PLAYBACK_FAILED;
#endif
}

//销毁历史流通道
static int HY_DVR_HistoryChannelDestroy(HANDLE hIndex)
{
#ifndef DEBUG_NOT_USE_STR_LIB

	DWORD CurrIndex;

	//检查句柄是否有效并获取历史通道索引
	RPB_CHK_HistoryChannel_HANDLE(hIndex, CurrIndex);

	//通知销毁流线程
	m_PlaybackCfg[CurrIndex].dwDataQuit = 1;
	sem_post(&m_PlaybackCfg[CurrIndex].m_sem_Stream);
	sem_post(&m_PlaybackCfg[CurrIndex].m_sem_StreamSend);

	//通知注册线程销毁
	sem_post(&m_sem_create);

    for (int dwChannel = 0; dwChannel < DVR_MAX_VIDEOIN_NUM; dwChannel++)
    {
        m_PlaybackCfg[CurrIndex].m_IsInsertCh[dwChannel] = 0;
        m_PlaybackCfg[CurrIndex].m_FirstNeedIFrameCh[dwChannel] = 0;
    }

	printf("success destroy <0x%08x>\n", (int) hIndex);
	return ERR_PLAYBACK_OK;
#else
	return ERR_PLAYBACK_FAILED;
#endif
}

//添加流通道号
static int HY_DVR_HistoryChannelInsert(HANDLE hIndex, DWORD dwChannel)
{
#ifndef DEBUG_NOT_USE_STR_LIB
	DWORD CurrIndex;

	//检查句柄是否有效并获取历史通道索引
	RPB_CHK_HistoryChannel_HANDLE(hIndex, CurrIndex);

	//检查历史通道是否创建失败
	if (m_PlaybackCfg[CurrIndex].hPlayback == PLAYBACK_FAILED_HANDLE)
	{
        printf("[%s-%d] create failed, handle <0x%08x> \n", __FUNCTION__, __LINE__, (int) hIndex);
		return ERR_PLAYBACK_FAILED;
	}

	//检查历史通道是否正在创建
	if (m_PlaybackCfg[CurrIndex].hPlayback == hIndex)
	{
        printf("[%s-%d] busy, handle <0x%08x> is creating ...\n", __FUNCTION__, __LINE__, (int) hIndex);
		return ERR_PLAYBACK_BUSY;
	}

	//检查流通道号是否有效
	if (dwChannel < 0 || dwChannel >= DVR_MAX_VIDEOIN_NUM)
	{
        printf("[%s-%d] ch = %d invalid\n", __FUNCTION__, __LINE__, (int) dwChannel);
		return ERR_PLAYBACK_FAILED;
	}

    m_PlaybackCfg[CurrIndex].m_IsInsertCh[dwChannel] = 1;
    m_PlaybackCfg[CurrIndex].m_FirstNeedIFrameCh[dwChannel] = 1;

	MPU_DBG("success <0x%08x> ch = %d\n", (int) hIndex, (int) dwChannel);
	return ERR_PLAYBACK_OK;
#else
	return ERR_PLAYBACK_FAILED;
#endif

}

//删除流通道号
static int HY_DVR_HistoryChannelRemove(HANDLE hIndex, DWORD dwChannel)
{
#ifndef DEBUG_NOT_USE_STR_LIB
	DWORD CurrIndex;

	//检查句柄是否有效并获取历史通道索引
	RPB_CHK_HistoryChannel_HANDLE(hIndex, CurrIndex);


	//检查历史通道是否创建失败
	if (m_PlaybackCfg[CurrIndex].hPlayback == PLAYBACK_FAILED_HANDLE)
	{
        printf("[%s-%d] create failed, handle <0x%08x> \n", __FUNCTION__, __LINE__, (int) hIndex);
		return ERR_PLAYBACK_FAILED;
	}

	//检查历史通道是否正在创建
	if (m_PlaybackCfg[CurrIndex].hPlayback == hIndex)
	{
        printf("[%s-%d] busy, handle <0x%08x>\n", __FUNCTION__, __LINE__, (int) hIndex);
		return ERR_PLAYBACK_BUSY;
	}

	//检查流通道号是否有效
	if (dwChannel < 0 || dwChannel >= DVR_MAX_VIDEOIN_NUM)
	{
        printf("[%s-%d] ch = %d invalid\n", __FUNCTION__, __LINE__, (int) dwChannel);
		return ERR_PLAYBACK_FAILED;
	}

    m_PlaybackCfg[CurrIndex].m_IsInsertCh[dwChannel] = 0;
    m_PlaybackCfg[CurrIndex].m_FirstNeedIFrameCh[dwChannel] = 0;

	MPU_DBG("success <0x%08x> ch = %d\n", (int) hIndex, (int) dwChannel);
	return ERR_PLAYBACK_OK;
#else
	return ERR_PLAYBACK_FAILED;
#endif
}

// 启动流捕获
static int HY_DVR_HistoryChannelStartCapture(HANDLE hIndex)
{
#ifndef DEBUG_NOT_USE_STR_LIB

	DWORD CurrIndex;

	//检查句柄是否有效并获取历史通道索引
	RPB_CHK_HistoryChannel_HANDLE(hIndex, CurrIndex);


	//检查历史通道是否创建失败
	if (m_PlaybackCfg[CurrIndex].hPlayback == PLAYBACK_FAILED_HANDLE)
	{
        printf("[%s-%d] create failed, handle <0x%08x> \n", __FUNCTION__, __LINE__, (int) hIndex);
		return ERR_PLAYBACK_FAILED;
	}

	//检查历史通道是否正在创建或已经开始捕获
	if (m_PlaybackCfg[CurrIndex].hPlayback == hIndex || m_PlaybackCfg[CurrIndex].dwStreamStart == 1)
	{
        printf("[%s-%d] busy, handle <0x%08x> \n", __FUNCTION__, __LINE__, (int) hIndex);
		return ERR_PLAYBACK_BUSY;
	}

	//通知注册线程开始捕获
	m_PlaybackCfg[CurrIndex].dwStreamStart = 1;
	sem_post(&m_sem_create);

	MPU_DBG("success <0x%08x>\n", (int) hIndex);
	return ERR_PLAYBACK_OK;
#else
	return ERR_PLAYBACK_FAILED;
#endif
}

//停止流捕获
static int HY_DVR_HistoryChannelStopCapture(HANDLE hIndex)
{
#ifndef DEBUG_NOT_USE_STR_LIB

	int CurrIndex;

	//检查句柄是否有效并获取历史通道索引
	RPB_CHK_HistoryChannel_HANDLE(hIndex, CurrIndex);

	//检查历史通道是否创建失败
	if (m_PlaybackCfg[CurrIndex].hPlayback == PLAYBACK_FAILED_HANDLE)
	{
        printf("[%s-%d] create failed, handle <0x%08x> \n", __FUNCTION__, __LINE__, (int) hIndex);
		return ERR_PLAYBACK_FAILED;
	}

	//检查历史通道是否正在创建或已经停止捕获
	if (m_PlaybackCfg[CurrIndex].hPlayback == hIndex || m_PlaybackCfg[CurrIndex].dwStreamStop == 1)
	{
        printf("[%s-%d] busy, handle <0x%08x> \n", __FUNCTION__, __LINE__, (int) hIndex);
		return ERR_PLAYBACK_BUSY;
	}

	//通知注册线程停止捕获
	//m_PlaybackCfg[CurrIndex].dwStreamStatus = 0;
	m_PlaybackCfg[CurrIndex].dwStreamStop = 1;
	sem_post(&m_sem_create);

	MPU_DBG("success <0x%08x>\n", (int) hIndex);
	return ERR_PLAYBACK_OK;
#else
	return ERR_PLAYBACK_FAILED;
#endif
}

//流定位
static int HY_DVR_HistoryChannelPosition(HANDLE hIndex, const LPHistoryChannelPositionInfo pHistoryChannelPositionInfo)
{
#ifndef DEBUG_NOT_USE_STR_LIB

	DWORD CurrIndex;

	time_t startTime;

	struct tm stm;

	int status;

	//检查句柄是否有效并获取历史通道索引
	RPB_CHK_HistoryChannel_HANDLE(hIndex, CurrIndex);

	//检查历史通道是否创建失败
	if (m_PlaybackCfg[CurrIndex].hPlayback == PLAYBACK_FAILED_HANDLE)
	{
        printf("[%s-%d] create failed, handle <0x%08x> \n", __FUNCTION__, __LINE__, (int) hIndex);
		return ERR_PLAYBACK_FAILED;
	}

	//检查历史通道是否正在创建或正在定位
	if (m_PlaybackCfg[CurrIndex].hPlayback == hIndex || m_PlaybackCfg[CurrIndex].dwStreamSeek == 1)
	{
        printf("[%s-%d] busy,handle <0x%08x> \n", __FUNCTION__, __LINE__, (int) hIndex);
		return ERR_PLAYBACK_BUSY;
	}

	//流回放已经停止捕获
	status = m_PlaybackCfg[CurrIndex].dwStreamStatus;
	if (status)
	{
		m_PlaybackCfg[CurrIndex].dwStreamStop = 1;
	}

	stm.tm_year = pHistoryChannelPositionInfo->cSeekTime.wYear - 1900;
	stm.tm_mon = pHistoryChannelPositionInfo->cSeekTime.wMonth - 1;
	stm.tm_mday = pHistoryChannelPositionInfo->cSeekTime.wDay;
	stm.tm_hour = pHistoryChannelPositionInfo->cSeekTime.wHour;
	stm.tm_min = pHistoryChannelPositionInfo->cSeekTime.wMinute;
	stm.tm_sec = pHistoryChannelPositionInfo->cSeekTime.wSecond;
	startTime = mktime(&stm);
	m_PlaybackCfg[CurrIndex].tStreamStartTime = startTime;
	m_PlaybackCfg[CurrIndex].dwStreamSeek = 1;

    for (int dwChannel = 0; dwChannel < DVR_MAX_VIDEOIN_NUM; dwChannel++)
    {
        m_PlaybackCfg[CurrIndex].m_FirstNeedIFrameCh[dwChannel] = 1;
    }

	//开启
	if (status)
	{
		m_PlaybackCfg[CurrIndex].dwStreamStart = 1;
	}

	//通知注册线程开始定位
	sem_post(&m_sem_create);

	MPU_DBG("handle = 0x%08x -> 0x%08x,%04d-%02d-%02d %02d:%02d:%02d\n", (int) hIndex, (int) m_PlaybackCfg[CurrIndex].hPlayback, stm.tm_year + 1900,
			 stm.tm_mon + 1, stm.tm_mday, stm.tm_hour, stm.tm_min, stm.tm_sec);
	return ERR_PLAYBACK_OK;
#else
	return ERR_PLAYBACK_FAILED;
#endif
}

//设置弹出速度
static int HY_DVR_HistoryChannelSetSpeed(HANDLE hIndex, DWORD dwSpeed)
{
#ifndef DEBUG_NOT_USE_STR_LIB

	int CurrIndex;

	//检查句柄是否有效并获取历史通道索引
	RPB_CHK_HistoryChannel_HANDLE(hIndex, CurrIndex);

	//检查历史通道是否创建失败
	if (m_PlaybackCfg[CurrIndex].hPlayback == PLAYBACK_FAILED_HANDLE)
	{
        printf("[%s-%d] create failed, handle <0x%08x> \n", __FUNCTION__, __LINE__, (int) hIndex);
		return ERR_PLAYBACK_FAILED;
	}

	//检查历史通道是否正在创建
	if (m_PlaybackCfg[CurrIndex].hPlayback == hIndex)
	{
        printf("[%s-%d] busy, handle <0x%08x>\n", __FUNCTION__, __LINE__, (int) hIndex);
		return ERR_PLAYBACK_BUSY;
	}

	ZW_PkgSd_DataPopSetKeyFrame(m_PlaybackCfg[CurrIndex].hPlayback, (dwSpeed > 4) ? 1 : 0);

#if 0
	if (dwSpeed > 4)
	{
		if (!m_PlaybackCfg[CurrIndex].dwSetIFrame)
		{
			m_PlaybackCfg[CurrIndex].dwSetIFrame = 1;
		}
	}
	else if (m_PlaybackCfg[CurrIndex].dwSetIFrame)
	{
		m_PlaybackCfg[CurrIndex].dwSetIFrame = 0;
	}
#endif

	MPU_DBG("success <0x%08x> dwSpeed = %lu\n", (int) hIndex, dwSpeed);
	return ERR_PLAYBACK_OK;
#else
	return ERR_PLAYBACK_FAILED;
#endif
}

//创建历史流查询
static int HY_DVR_HistoryQueryCreateEx(HANDLE * hIndex, DWORD dwUserID, const LPHistoryQueryInfoMultiType pQuery)
{
#ifndef DEBUG_NOT_USE_STR_LIB

	int i;

	int CurrIndex;

	int iYear, iMon, iDay, iHour, iMin, iSec;

	struct tm stm;

	time_t Mark_time;

	time_t use_time;

	MPU_DBG("entry ID<%d>\n", (int) dwUserID);

	//开始计时
	use_time = time(NULL);

	///检测历史查询句柄指针是否有效
	if (hIndex == NULL)
	{
        printf("[%s-%d] hIndex == NULL\n", __FUNCTION__, __LINE__);
		return ERR_PLAYBACK_FAILED;
	}

	//检查通道号是否有效
	if (pQuery->dwChannel < 0 || pQuery->dwChannel >= DVR_MAX_VIDEOIN_NUM)
	{
        printf("[%s-%d] ch = %d invalid\n", __FUNCTION__, __LINE__, (int) pQuery->dwChannel);
		return ERR_PLAYBACK_FAILED;
	}

	//检查盘组类型是否有效
	if (pQuery->dwDiskGroup <= eDiskGroupTypeBegin || pQuery->dwDiskGroup >= eDiskGroupTypeEnd)
	{
        printf("[%s-%d] diskgroup = %d invalid\n", __FUNCTION__, __LINE__, (int) pQuery->dwDiskGroup);
		return ERR_PLAYBACK_FAILED;
	}

	//检查用户是否具有远程回放权限
	if (!MPU_CIU_CheckRemoteUser(dwUserID, CHANNEL_PLAYBACK, pQuery->dwChannel))
	{
        printf("[%s-%d] id = %d ch = %d no right\n", __FUNCTION__, __LINE__, (int) dwUserID, (int) pQuery->dwChannel);
		return IDVR_CIU_NO_RIGHT;
	}

	//查找空闲通道
	CurrIndex = -1;
	for (i = 0; i < MAX_PLAYBACK_SUPPORT; i++)
	{
        if ((NULL==m_PlaybackCfg[i].hPlayback_info) && (NULL==m_PlaybackCfg[i].hPlayback))
		{
			CurrIndex = i;
			break;
		}
	}

	//检查通道索引是否有效
	if (CurrIndex == -1)
	{
        printf("[%s-%d] too much history channel create\n", __FUNCTION__, __LINE__);
		return eErrorP2PFunctionInUse;
	}

	printf("query from %s to %s \n", pQuery->cBeginTime, pQuery->cEndTime);

	snprintf(m_PlaybackCfg[CurrIndex].whichDay,9,"%s",pQuery->cBeginTime);

	//读取开始时间
	sscanf(pQuery->cBeginTime, "%04d%02d%02d%02d%02d%02d", &iYear, &iMon, &iDay, &iHour, &iMin, &iSec);
	stm.tm_year = iYear - 1900;
	stm.tm_mon = iMon - 1;
	stm.tm_mday = iDay;
	stm.tm_hour = iHour;
	stm.tm_min = iMin;
	stm.tm_sec = iSec;
	m_PlaybackCfg[CurrIndex].tSegStartTime = mktime(&stm);

	//读取结束时间
	sscanf(pQuery->cEndTime, "%04d%02d%02d%02d%02d%02d", &iYear, &iMon, &iDay, &iHour, &iMin, &iSec);
	stm.tm_year = iYear - 1900;
	stm.tm_mon = iMon - 1;
	stm.tm_mday = iDay;
	stm.tm_hour = iHour;
	stm.tm_min = iMin;
	stm.tm_sec = iSec;
	m_PlaybackCfg[CurrIndex].tSegStopTime = mktime(&stm);

	m_PlaybackCfg[CurrIndex].dwSegChannel = pQuery->dwChannel;
	m_PlaybackCfg[CurrIndex].dwInfo_type = pQuery->dwStreamType;

	m_PlaybackCfg[CurrIndex].dwUserID = dwUserID;

	switch (pQuery->dwDiskGroup)
	{
		case eDiskGroupNormal:
			{
				m_PlaybackCfg[CurrIndex].dwSegDiskGroup = TypeDiskGroupNormal;
				break;
			}
		case eDiskGroupAlarm:
			{
				m_PlaybackCfg[CurrIndex].dwSegDiskGroup = TypeDiskGroupAlarm;
				break;
			}
		case eDiskGroupRedundance:
			{
				m_PlaybackCfg[CurrIndex].dwSegDiskGroup = TypeDiskGroupRedundance;
				break;
			}
		case eDiskGroupBackup:
			{
				m_PlaybackCfg[CurrIndex].dwSegDiskGroup = TypeDiskGroupBackup;
				break;
			}
		default:
			{
				m_PlaybackCfg[CurrIndex].dwSegDiskGroup = TypeDiskGroupNormal;
				break;
			}
	}

	//修改操作码计数器
	++m_mark_cnt;
	if (m_mark_cnt > 127)
	{
		m_mark_cnt = 0;
	}

	//生成操作效验码
	Mark_time = time(NULL);
	m_PlaybackCfg[CurrIndex].dwQureyHandeVerfyMark = (((Mark_time & 0xFFFF) | 0xC000) << 8) | m_mark_cnt;

	//生成查询句柄
	*hIndex = m_PlaybackCfg[CurrIndex].hPlayback_info =
		(HANDLE) ((m_PlaybackCfg[CurrIndex].dwQureyHandeVerfyMark << PLAYBACK_MARK_SHIFT) | CurrIndex);

	sem_post(&m_sem_create);	//通知创建 

	use_time = time(NULL) - use_time;

	MPU_DBG("hIndex<0x%08x>, diskgroup %lu, type 0x%x, ch %lu, <%s -> %s> use %lu s\n", (int) *hIndex,
			 m_PlaybackCfg[CurrIndex].dwSegDiskGroup, (int) pQuery->dwStreamType, pQuery->dwChannel, pQuery->cBeginTime, pQuery->cEndTime, use_time);

	return ERR_PLAYBACK_OK;
#else
	return ERR_PLAYBACK_FAILED;
#endif
}

//销毁历史流查询
static int HY_DVR_HistoryQueryDestroy(HANDLE hIndex)
{
#ifndef DEBUG_NOT_USE_STR_LIB

	DWORD CurrIndex;
	char command[128];
	int i;

	//检查历史查询句柄有效性并获取索引
	RPB_CHK_HistoryQuery_HANDLE(hIndex, CurrIndex);
#ifdef NEW_QUERY
	memset(command,0,sizeof(command));
	snprintf(command,sizeof(command), "rm -rf /tmp/pb_q_%d.dat", (int)hIndex);
	mysystem(command);
#endif
	//等待历史查询创建完毕
	while (1)
	{
		if (m_PlaybackCfg[CurrIndex].hPlayback_info != hIndex)
		{
			break;
		}
		//创建中，等待其完成
		usleep(10000);
	}

	//销毁所有录像类型片段链表
	for (i = 0; i < MAX_SUPPORT_RECTYPE; i++)
	{
		if (m_PlaybackCfg[CurrIndex].pRecSegHead[i] != NULL)
		{
			ZW_PkgSd_ReleaseRecSegList(m_PlaybackCfg[CurrIndex].dwHandle,(PSS_REGSEG_T * )(m_PlaybackCfg[CurrIndex].pRecSegHead[i]));
			m_PlaybackCfg[CurrIndex].pRecSegHead[i] = NULL;
			m_PlaybackCfg[CurrIndex].dwRecSegCount[i] = 0;
		}
	}
	m_PlaybackCfg[CurrIndex].pRecSegNext = NULL;

	//清除查询句柄
	m_PlaybackCfg[CurrIndex].hPlayback_info = NULL;

	printf("exit handle<%#x>\n", (int) (hIndex));
	return ERR_PLAYBACK_OK;
#else
	return ERR_PLAYBACK_FAILED;
#endif
}

static int HY_DVR_HistoryQueryCreateFile(HANDLE * hIndex, DWORD dwUserID, const LPHistoryQueryInfoMultiType pQuery, LPRecordSetFileInfo rsinfo)
{
#ifndef DEBUG_NOT_USE_STR_LIB
	DWORD CurrIndex;
	char value[10];
	int i, bFreeflag, ret=0;
	int count=0;
	struct tm tm_time;
	record_node_t recordNode;
	record_file_header_t segCount;
	FILE *fp;
	struct stat buf;

	///检测历史查询句柄指针是否有效
	if (hIndex == NULL)
	{
        printf("[%s-%d] hIndex == NULL\n", __FUNCTION__, __LINE__);
		return ERR_PLAYBACK_FAILED;
	}

	if (rsinfo == NULL)
	{
        printf("[%s-%d] rsinfo == NULL\n", __FUNCTION__, __LINE__);
		return ERR_PLAYBACK_FAILED;
	}

	HY_DVR_HistoryQueryCreateEx(hIndex, dwUserID, pQuery);
	MPU_DBG("wait m_sem_queryOver\n");
	sem_wait(&m_sem_queryOver);
	MPU_DBG("query over\n");

	memset(rsinfo, 0, sizeof (RecordSetFileInfo));

	//检查历史查询句柄有效性并获取索引
	RPB_CHK_HistoryQuery_HANDLE(*hIndex, CurrIndex);

	//检查是否正在创建过程中
	if (m_PlaybackCfg[CurrIndex].hPlayback_info == *hIndex)
	{
		int count = 0;
		while(count++ < 50)
		{
            printf("[%s-%d] busy, handle<0x%08x>\n", __FUNCTION__, __LINE__, (int) *hIndex);
			usleep(10000);
			if(m_PlaybackCfg[CurrIndex].hPlayback_info != *hIndex)
			{
				break;
			}
		}
		if(m_PlaybackCfg[CurrIndex].hPlayback_info == *hIndex)
		{
		    printf("[%s-%d] playback is busy ......\n", __FUNCTION__, __LINE__);
            HY_DVR_HistoryQueryDestroy(*hIndex);
			return ERR_PLAYBACK_BUSY;
		}
	}

	//创建存储文件
	snprintf(rsinfo->m_sFileName,sizeof(rsinfo->m_sFileName), "/tmp/pb_q_%d.dat", (int)*hIndex);
	fp = fopen(rsinfo->m_sFileName, "w");
	if(NULL == fp)
	{
		fprintf(stderr, "fopen file %s fail: %s\n", rsinfo->m_sFileName, strerror(errno));
        HY_DVR_HistoryQueryDestroy(*hIndex);
		return ERR_PLAYBACK_FAILED;
	}
	memset(&segCount,0,sizeof(record_file_header_t));
	fwrite(&segCount,sizeof(record_file_header_t),1,fp);

	while(NULL != m_PlaybackCfg[CurrIndex].pRecSegNext)
	{
		memset(&recordNode, 0, sizeof(record_node_t));
		//检查是否已经到末尾了
		if (m_PlaybackCfg[CurrIndex].pRecSegNext == NULL)
		{
			MPU_DBG("over, handle<0x%08x>\n", (int) *hIndex);
            HY_DVR_HistoryQueryDestroy(*hIndex);
			return ERR_PLAYBACK_FINISHED;
		}
		//MPU_DBG("%d	 to   %d\n",m_PlaybackCfg[CurrIndex].pRecSegNext->tBeginTime,m_PlaybackCfg[CurrIndex].pRecSegNext->tEndTime);
		localtime_r(&m_PlaybackCfg[CurrIndex].pRecSegNext->tBeginTime, &tm_time);
		snprintf(recordNode.begin_time, sizeof(recordNode.begin_time), "%04d%02d%02d%02d%02d%02d",
				tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday, tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec);
		localtime_r(&m_PlaybackCfg[CurrIndex].pRecSegNext->tEndTime, &tm_time);
		snprintf(recordNode.end_time, sizeof(recordNode.end_time), "%04d%02d%02d%02d%02d%02d",
				tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday, tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec);
#if 1
		
		//将时间连续的片段整合成一个片段
		while(1)
		{
			if(NULL == m_PlaybackCfg[CurrIndex].pRecSegNext->ptNext)
				break;
			ret = segCheck(m_PlaybackCfg[CurrIndex].pRecSegNext->tBeginTime, m_PlaybackCfg[CurrIndex].pRecSegNext->tEndTime);
			if(ret)
			{
				m_PlaybackCfg[CurrIndex].pRecSegNext = m_PlaybackCfg[CurrIndex].pRecSegNext->ptNext;
				continue;
			}

			if(((m_PlaybackCfg[CurrIndex].pRecSegNext->ptNext->tBeginTime - m_PlaybackCfg[CurrIndex].pRecSegNext->tEndTime == 0)
				|| (m_PlaybackCfg[CurrIndex].pRecSegNext->ptNext->tBeginTime - m_PlaybackCfg[CurrIndex].pRecSegNext->tEndTime == 1))
				&& (m_PlaybackCfg[CurrIndex].pRecSegNext->iRecType == m_PlaybackCfg[CurrIndex].pRecSegNext->ptNext->iRecType))
			{
				m_PlaybackCfg[CurrIndex].pRecSegNext = m_PlaybackCfg[CurrIndex].pRecSegNext->ptNext;
				localtime_r(&m_PlaybackCfg[CurrIndex].pRecSegNext->tEndTime, &tm_time);
				snprintf(recordNode.end_time, sizeof(recordNode.end_time), "%04d%02d%02d%02d%02d%02d",
						tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday, tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec);
			}
			else
				break;
		}

		//将片段开始时间修改为今天凌晨
		if((0 > strncmp(recordNode.begin_time,m_PlaybackCfg[CurrIndex].whichDay,8)) && 
			(0 <= strncmp(recordNode.end_time,m_PlaybackCfg[CurrIndex].whichDay,8)))
		{
			memset(value,0,sizeof(value));
			snprintf(value,9,"%s",m_PlaybackCfg[CurrIndex].whichDay);
			memset(recordNode.begin_time,0,sizeof(recordNode.begin_time));
			snprintf(recordNode.begin_time,sizeof(recordNode.begin_time),"%s000000",value);
			MPU_DBG("modify first seg include two day, new value %s!\n",recordNode.begin_time);
		}

		//将片段结束时间修改为今天午夜
		if((0 < strncmp(recordNode.end_time,m_PlaybackCfg[CurrIndex].whichDay,8)) && 
			(0 >= strncmp(recordNode.begin_time,m_PlaybackCfg[CurrIndex].whichDay,8)))
		{
			memset(value,0,sizeof(value));
			snprintf(value,9,"%s",m_PlaybackCfg[CurrIndex].whichDay);
			memset(recordNode.end_time,0,sizeof(recordNode.end_time));
			snprintf(recordNode.end_time,sizeof(recordNode.end_time),"%s235959",value);
			MPU_DBG("modify last seg include two day, new value %s!\n",recordNode.end_time);
		}

#endif

		//pNext->dwChannel = m_PlaybackCfg[CurrIndex].dwSegChannel;
		//pNext->dwDataSize = 0;

		//转换流类型
		switch (m_PlaybackCfg[CurrIndex].pRecSegNext->iRecType)
		{

			case 1:
				{
					snprintf(recordNode.stream_type,sizeof(recordNode.stream_type),"%d",GENERAL_RECORD);
					break;
				}
			case 2:
				{
					snprintf(recordNode.stream_type,sizeof(recordNode.stream_type),"%d",MOTION_RECORD);
					break;
				}
			case 4:
				{
					snprintf(recordNode.stream_type,sizeof(recordNode.stream_type),"%d",ALARM_RECORD);
					break;
				}
			case 8:
				{
					snprintf(recordNode.stream_type,sizeof(recordNode.stream_type), "%d", AI_RECORD);
					break;
				}
			default:
				{
					printf("unsupported rec type %d\n", m_PlaybackCfg[CurrIndex].pRecSegNext->iRecType);
                    HY_DVR_HistoryQueryDestroy(*hIndex);
					return ERR_PLAYBACK_FAILED;
				}
		}

		//MPU_DBG("-------%s %s StreamType 0x%x\n",pNext->begin_time,pNext->end_time,pNext->dwStreamType);

		//开始定位下一个
		m_PlaybackCfg[CurrIndex].pRecSegNext = m_PlaybackCfg[CurrIndex].pRecSegNext->ptNext;
		count++;
		fwrite(&recordNode,sizeof(record_node_t),1,fp);
	}

	//重新写入文件头信息
	fseek(fp, 0L, SEEK_SET);
	snprintf(segCount.count_of_recordset,sizeof(segCount.count_of_recordset),"%d",count);
	fwrite(&segCount,sizeof(record_file_header_t),1,fp);
	fclose(fp);
	
	//填写文件信息
	rsinfo->m_dwFileSize = sizeof(record_file_header_t) + count * sizeof(record_node_t);
    stat(rsinfo->m_sFileName, &buf);
	localtime_r(&buf.st_ctime, &tm_time);
	snprintf(rsinfo->m_sCreateTime,sizeof(rsinfo->m_sCreateTime), "%04d%02d%02d%02d%02d%02d",
			tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday, tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec);

	if (m_PlaybackCfg[CurrIndex].pRecSegNext == NULL)
	{
		MPU_DBG("get seg over!\n");
		//为空，则先释放一个链表
		bFreeflag = 0;
		for (i = 0; i < MAX_SUPPORT_RECTYPE; i++)
		{
			if (m_PlaybackCfg[CurrIndex].pRecSegHead[i] != NULL)
			{
				if (!bFreeflag)
				{
					ZW_PkgSd_ReleaseRecSegList(m_PlaybackCfg[CurrIndex].dwHandle,(PSS_REGSEG_T * )(m_PlaybackCfg[CurrIndex].pRecSegHead[i]));
					m_PlaybackCfg[CurrIndex].pRecSegHead[i] = NULL;
					m_PlaybackCfg[CurrIndex].dwRecSegCount[i] = 0;
					bFreeflag = 1;
					continue;
				}
				//得到下一位置
				m_PlaybackCfg[CurrIndex].pRecSegNext = m_PlaybackCfg[CurrIndex].pRecSegHead[i];
				break;
			}
		}
	}

    HY_DVR_HistoryQueryDestroy(*hIndex);
	return ERR_PLAYBACK_OK;
#else
	return ERR_PLAYBACK_FAILED;
#endif
}



//查找下一个片段
static int HY_DVR_HistoryQueryNextEx(HANDLE hIndex, LPHistoryQueryResultMultiType pNext)
{
#ifndef DEBUG_NOT_USE_STR_LIB

	DWORD CurrIndex;

	int i, bFreeflag;

	struct tm tm_time;

	//检查查询结构指针是否有效
	if (pNext == NULL)
	{
        printf("[%s-%d] pNext == NULL\n", __FUNCTION__, __LINE__);
		return ERR_PLAYBACK_FAILED;
	}

	//初始化查询结果
	memset(pNext, 0, sizeof (HistoryQueryResult));

	//检查历史查询句柄有效性并获取索引
	RPB_CHK_HistoryQuery_HANDLE(hIndex, CurrIndex);

	//检查是否正在创建过程中
	if (m_PlaybackCfg[CurrIndex].hPlayback_info == hIndex)
	{
        printf("[%s-%d] busy, handle<0x%08x>\n", __FUNCTION__, __LINE__, (int) hIndex);
		return ERR_PLAYBACK_BUSY;
	}

	//检查是否已经到末尾了
	if (m_PlaybackCfg[CurrIndex].pRecSegNext == NULL)
	{
		MPU_DBG("over, handle<0x%08x>\n", (int) hIndex);
		return ERR_PLAYBACK_FINISHED;
	}
	//MPU_DBG("%d   to   %d\n",m_PlaybackCfg[CurrIndex].pRecSegNext->tBeginTime,m_PlaybackCfg[CurrIndex].pRecSegNext->tEndTime);
	localtime_r(&m_PlaybackCfg[CurrIndex].pRecSegNext->tBeginTime, &tm_time);
	snprintf(pNext->cBeginTime, sizeof(pNext->cBeginTime), "%04d%02d%02d%02d%02d%02d",
			tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday, tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec);
	localtime_r(&m_PlaybackCfg[CurrIndex].pRecSegNext->tEndTime, &tm_time);
	snprintf(pNext->cEndTime, sizeof(pNext->cEndTime), "%04d%02d%02d%02d%02d%02d",
			tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday, tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec);
#if 1
	char value[10];
	//将时间连续的片段整合成一个片段
	while(1)
	{
		if(NULL == m_PlaybackCfg[CurrIndex].pRecSegNext->ptNext)
			break;
		if(((m_PlaybackCfg[CurrIndex].pRecSegNext->ptNext->tBeginTime - m_PlaybackCfg[CurrIndex].pRecSegNext->tEndTime == 0)
			|| (m_PlaybackCfg[CurrIndex].pRecSegNext->ptNext->tBeginTime - m_PlaybackCfg[CurrIndex].pRecSegNext->tEndTime == 1))
			&& (m_PlaybackCfg[CurrIndex].pRecSegNext->iRecType == m_PlaybackCfg[CurrIndex].pRecSegNext->ptNext->iRecType))
		{
			m_PlaybackCfg[CurrIndex].pRecSegNext = m_PlaybackCfg[CurrIndex].pRecSegNext->ptNext;
			localtime_r(&m_PlaybackCfg[CurrIndex].pRecSegNext->tEndTime, &tm_time);
			snprintf(pNext->cEndTime,sizeof(pNext->cEndTime), "%04d%02d%02d%02d%02d%02d",
					tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday, tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec);
		}
		else
			break;
	}
	
	//将片段开始时间修改为今天凌晨
	if((0 > strncmp(pNext->cBeginTime,m_PlaybackCfg[CurrIndex].whichDay,8)) && 
		(0 <= strncmp(pNext->cEndTime,m_PlaybackCfg[CurrIndex].whichDay,8)))
	{
		memset(value,0,sizeof(value));
		snprintf(value,9,"%s",m_PlaybackCfg[CurrIndex].whichDay);
		memset(pNext->cBeginTime,0,sizeof(pNext->cBeginTime));
		snprintf(pNext->cBeginTime, sizeof(pNext->cBeginTime), "%s000000",value);
		MPU_DBG("modify first seg include two day, new value %s!\n",pNext->cBeginTime);
	}

	//将片段结束时间修改为今天午夜
	if((0 < strncmp(pNext->cEndTime,m_PlaybackCfg[CurrIndex].whichDay,8)) && 
		(0 >= strncmp(pNext->cBeginTime,m_PlaybackCfg[CurrIndex].whichDay,8)))
	{
		memset(value,0,sizeof(value));
		snprintf(value,9,"%s",m_PlaybackCfg[CurrIndex].whichDay);
		memset(pNext->cEndTime,0,sizeof(pNext->cEndTime));
		snprintf(pNext->cEndTime, sizeof(pNext->cEndTime), "%s235959",value);
		MPU_DBG("modify last seg include two day, new value %s!\n",pNext->cEndTime);
	}
	
	MPU_DBG("%s to %s\n",pNext->cBeginTime,pNext->cEndTime);

#endif

	pNext->dwChannel = m_PlaybackCfg[CurrIndex].dwSegChannel;
	pNext->dwDataSize = 0;

	//转换流类型
	switch (m_PlaybackCfg[CurrIndex].pRecSegNext->iRecType)
	{
	//这里的 case值
	//来自sd_ops.c中的eNetRecordType枚举
		case 1:
			{
				pNext->dwStreamType = GENERAL_RECORD;
				break;
			}
		case 2:
			{
				pNext->dwStreamType = MOTION_RECORD;
				break;
			}
		case 4:
			{
				pNext->dwStreamType = ALARM_RECORD;
				break;
			}
		case 6:
			{
				pNext->dwStreamType = AI_RECORD;
				break;
			}
	
		default:
			{
				printf("unsupported rec type %d\n", m_PlaybackCfg[CurrIndex].pRecSegNext->iRecType);
				return ERR_PLAYBACK_FAILED;
				break;
			}
	}

	//MPU_DBG("-------%s %s StreamType 0x%x\n",pNext->cBeginTime,pNext->cEndTime,pNext->dwStreamType);

	//开始定位下一个
	m_PlaybackCfg[CurrIndex].pRecSegNext = m_PlaybackCfg[CurrIndex].pRecSegNext->ptNext;

	if (m_PlaybackCfg[CurrIndex].pRecSegNext == NULL)
	{
		MPU_DBG("get seg over!\n");
		//为空，则先释放一个链表
		bFreeflag = 0;
		for (i = 0; i < MAX_SUPPORT_RECTYPE; i++)
		{
			if (m_PlaybackCfg[CurrIndex].pRecSegHead[i] != NULL)
			{
				if (!bFreeflag)
				{
					ZW_PkgSd_ReleaseRecSegList(m_PlaybackCfg[CurrIndex].dwHandle,(PSS_REGSEG_T * )(m_PlaybackCfg[CurrIndex].pRecSegHead[i]));
					m_PlaybackCfg[CurrIndex].pRecSegHead[i] = NULL;
					m_PlaybackCfg[CurrIndex].dwRecSegCount[i] = 0;
					bFreeflag = 1;
					continue;
				}
				//得到下一位置
				m_PlaybackCfg[CurrIndex].pRecSegNext = m_PlaybackCfg[CurrIndex].pRecSegHead[i];
				break;
			}
		}
	}

	return ERR_PLAYBACK_OK;
#else
	return ERR_PLAYBACK_FAILED;
#endif
}


//历史流回放线程
static void *RPB_StreamThread(void *lParam)
{
    PRT_TID();

#ifndef DEBUG_NOT_USE_STR_LIB

	BEGINRUN_THREAD;

	int result;

	DWORD CurrIndex;

	DWORD mark;

	HANDLE hIndex;

	hIndex = (HANDLE) lParam;
	mark = ((DWORD) hIndex) >> PLAYBACK_MARK_SHIFT;
	CurrIndex = ((DWORD) hIndex) & PLAYBACK_INDEX_MASK;

	while (1)
	{
		//退出线程处理
		if (m_PlaybackCfg[CurrIndex].dwDataQuit)
		{
			printf("[%s-%d] quit %#x\n", __FUNCTION__, __LINE__, (int) hIndex);
			break;
		}

		//操作验证码无效
		if (m_PlaybackCfg[CurrIndex].dwHandeVerfyMark != mark)
		{						//当前线程应该退出了
			printf("[%s-%d] invalid mark quit %#x\n", __FUNCTION__, __LINE__, (int) hIndex);
			break;
		}

		//    MPU_DBG("waiting for sem\n");
		sem_wait(&m_PlaybackCfg[CurrIndex].m_sem_Stream);
		//printf("%d-%d-%d-%d-%d\n",m_PlaybackCfg[CurrIndex].dwDataQuit,
		//	(m_PlaybackCfg[CurrIndex].dwHandeVerfyMark != mark), (!m_PlaybackCfg[CurrIndex].dwDataFill),
		//	(!m_PlaybackCfg[CurrIndex].dwStreamStatus),(m_PlaybackCfg[CurrIndex].tStreamData.dwFrameType == HISTORY_STREAM_TIME_END));
		
		//退出线程处理
		if (m_PlaybackCfg[CurrIndex].dwDataQuit)
		{
			printf("[%s-%d] quit\n", __FUNCTION__, __LINE__);
			break;
		}

		//操作验证码无效
		if (m_PlaybackCfg[CurrIndex].dwHandeVerfyMark != mark)
		{						//当前线程应该退出了
			printf("[%s-%d] invalid mark quit%#x\n", __FUNCTION__, __LINE__, (int) hIndex);
			break;
		}

		if (!m_PlaybackCfg[CurrIndex].dwDataFill)
		{						//无数据
			printf("[%s-%d] no data reach 0x%x\n", __FUNCTION__, __LINE__, (int) hIndex);
			continue;
		}

		if (!m_PlaybackCfg[CurrIndex].dwStreamStatus)
		{						//暂停
			printf("[%s-%d] DPM callback pause 0x%x\n", __FUNCTION__, __LINE__, (int) hIndex);
			sem_post(&m_PlaybackCfg[CurrIndex].m_sem_StreamSend);//让流回调函数DataPop_Callback返回，以免照成阻塞
			continue;
		}

#if 1        
		if(m_PlaybackCfg[CurrIndex].tStreamData.dwFrameType == HISTORY_STREAM_TIME_END)
		{
			printf("@@@@@@@@@@@@@@@@@@pop over, send HISTORY_STREAM_TIME_END!@@@@@@@@@@@@@@@@@@\n");
            HistoryChannelStream StreamDataTmp;

            for (int dwChannel = 1; dwChannel < DVR_MAX_VIDEOIN_NUM; dwChannel++)
            {                
                /* 历史流结束，如果chn0以外其他通道也需要推送结束帧类型 */
                if (m_PlaybackCfg[CurrIndex].m_IsInsertCh[dwChannel] == 1)
                {
                    memset(&StreamDataTmp, 0, sizeof(HistoryChannelStream));
                    memcpy(&StreamDataTmp, &m_PlaybackCfg[CurrIndex].tStreamData, sizeof(HistoryChannelStream));
                    StreamDataTmp.dwChannel = dwChannel;
                    result = HieServer_HistoryChannelStream(hIndex, &StreamDataTmp);
                }
            }
		}

		//发送数据
		result = HieServer_HistoryChannelStream(hIndex, &m_PlaybackCfg[CurrIndex].tStreamData);
 		m_PlaybackCfg[CurrIndex].dwDataFill = 0;	//数据已取
		sem_post(&m_PlaybackCfg[CurrIndex].m_sem_StreamSend);
		if (result)
		{						//失败了 
			MPU_DBG("HieServer_HistoryChannelStream send error %d!0x%08x\n", result, (int) hIndex);
		}
#endif
		//   MPU_DBG("send stream succ\n");
	}

	//pthread_detach(pthread_self());
	ENDRUN_THREAD;
	pthread_exit(NULL);

	return NULL;
#else
	return NULL;
#endif
}

static int DataPop_Callback(HANDLE hDataPopper,
                                 int bIndexFrame, 
                                 unsigned short wFrameType, 
                                 time_t FrameTime, 
                                 unsigned char *pBuf,
                                 unsigned long dwSize, 
                                 unsigned long EventID, 
                                 void *pContext)
{
#ifndef DEBUG_NOT_USE_STR_LIB

	int i;

	int CurrIndex;
	static int num=0;
	HANDLE hIndex;

	struct tm tm_time;

	unsigned int startCode;

	int FrameType;

	int bForceJump = 0;			//

	int bEvent = 0;

    int iChannel = 0;
	int VIPFrame = 0;
    PH264DATAPACKETHEADER pFrameHead = NULL;
    if(0 == ((num++)%25))
    {
    	MPU_DBG("%d-inx %d-type %d-time %d-size %d\n",hDataPopper,bIndexFrame,wFrameType,FrameTime,dwSize);
    }
	bEvent = (DATAPOP_EVENT_TIME_OVER == EventID || DATAPOP_EVENT_DISK_ERR == EventID ||
			  DATAPOP_EVENT_TIME_JUMP == EventID || DATAPOP_EVENT_TIME_CURR == EventID);
	//////////////////////////////////////////////////////////////////////////
	if(bEvent)
		MPU_DBG("Get event id=%d\n", EventID);
	//查找弹出器句柄
	CurrIndex = -1;
	for (i = 0; i < MAX_PLAYBACK_SUPPORT; i++)
	{
		if (m_PlaybackCfg[i].hPlayback == hDataPopper && hDataPopper != NULL)
		{
			CurrIndex = i;
			break;
		}
	}
	if (CurrIndex == -1)
	{

		printf("invalid handle[0x%08x] ,maybe DPM stop\n", (int) hDataPopper);
		return ERR_PLAYBACK_FAILED;
	}

#if 1
    if (EventID == DATAPOP_EVENT_NONE)
    {
        /* 判断当前数据帧是主Sensor还是次Sensor，将chn赋值 */
        pFrameHead = (PH264DATAPACKETHEADER)pBuf;
		//VIP帧的帧头信息保留位值会超过10000
		if(pFrameHead->uReserved >= 10000)
		{
			pFrameHead->uReserved -= 10000;
			VIPFrame = 1;
		}
		
        if (pFrameHead->uReserved == 0)
        {
           iChannel = 0; /* 主Sensor */
        }
        else if (pFrameHead->uReserved == 1)
        {
           iChannel = 1; /* 第二Sensor */
        }
        else if (pFrameHead->uReserved == 2)
        {
           iChannel = 2; /* 第三Sensor */
        }
        else if (pFrameHead->uReserved == 3)
        {
           iChannel = 3; /* 第四Sensor */
        }
		unsigned char *bytes = (unsigned char *)&pFrameHead->uReserved;

		if(VIPFrame == 1)
		{

			bytes[1] = 0x01;
		}


        if (iChannel < 0 || iChannel >= DVR_MAX_VIDEOIN_NUM)
        {
            /* 流通道号无效 */
            return 0;
        }

        if (m_PlaybackCfg[CurrIndex].m_IsInsertCh[iChannel] == 0)
        {
            return 0; /* 不存在ch1，丢弃该数据帧 */
        }
        if (m_PlaybackCfg[CurrIndex].m_FirstNeedIFrameCh[iChannel] == 1)
        {
            if (pFrameHead->byFrameType != 0x01) /* 非I帧，丢弃 */
            {
                return 0;
            }
            else
            {
                m_PlaybackCfg[CurrIndex].m_FirstNeedIFrameCh[iChannel] = 0;
            }
        }
    }
    else if (EventID == DATAPOP_EVENT_TIME_OVER)
    {
       iChannel = 0; /* 结束帧，强制改为通道0 */
    }
#endif

	//处理传输帧类型的切换
	if (m_PlaybackCfg[CurrIndex].dwSetIFrame != m_PlaybackCfg[CurrIndex].dwIFrame)
	{							//二者不同
		if (m_PlaybackCfg[CurrIndex].dwIFrame)
		{						//正在做I帧
			if (!bIndexFrame && !VIPFrame)
			{					//非I帧，就不能立即切换了，P帧要丢掉
			}
			else
			{					//I帧,切换
				m_PlaybackCfg[CurrIndex].dwIFrame = 0;
			}
		}
		else
		{						//正常，立即切换
			m_PlaybackCfg[CurrIndex].dwIFrame = 1;
		}
	}

	if (m_PlaybackCfg[CurrIndex].dwIFrame)
	{							//只传I帧
		if (!bIndexFrame && !VIPFrame)
		{						//非I帧
			return 0;
		}
	}

	//生成历史通道句柄
	hIndex = (HANDLE) ((m_PlaybackCfg[CurrIndex].dwHandeVerfyMark << PLAYBACK_MARK_SHIFT) | CurrIndex);

	if (!bEvent)
	{							//当前帧为非事件帧
		int lastEvent;

		lastEvent = m_PlaybackCfg[CurrIndex].tStreamData.dwFrameType;

		//判断是否要强迫一个JUMP
		if (!(HISTORY_STREAM_TIME_END == lastEvent || HISTORY_STREAM_EXCEPTION == lastEvent ||
			  HISTORY_STREAM_TIME_JUMP == lastEvent || HISTORY_STREAM_TIME_CURR == lastEvent))
		{						//上一次不为事件帧,本次也不为事件帧
			if (m_PlaybackCfg[CurrIndex].tStreamEndTime != 0 && (FrameTime - m_PlaybackCfg[CurrIndex].tStreamEndTime > 8 || FrameTime - m_PlaybackCfg[CurrIndex].tStreamEndTime < 0))	//相差8s
			{
				MPU_DBG("Set Force Jump <lasttime = %lu,currtime = %lu>\n", m_PlaybackCfg[CurrIndex].tStreamEndTime, FrameTime);
				bForceJump = 1;
			}
		}

		//记录最后一帧时间
		m_PlaybackCfg[CurrIndex].tStreamEndTime = FrameTime;
	}

	if (m_PlaybackCfg[CurrIndex].dwDataQuit)
	{							//弹出销毁了
		MPU_DBG("DPM callback quit,ch = %d <0x%08x>\n", iChannel, (int) hIndex);
		return ERR_PLAYBACK_FAILED;
	}

	if (!m_PlaybackCfg[CurrIndex].dwStreamStatus)
	{							//暂停
		MPU_DBG("DPM callback pause,ch = %d <0x%08x>\n", iChannel, (int) hIndex);
		m_PlaybackCfg[CurrIndex].dwDataFill = 0;
		return ERR_PLAYBACK_FAILED;
	}

	if (bForceJump)
	{							//强制事件
		EventID = DATAPOP_EVENT_TIME_JUMP;
		bEvent = 1;
	}

	if (bEvent)
	{
		switch (EventID)
		{
			case DATAPOP_EVENT_TIME_JUMP:
				{
					m_PlaybackCfg[CurrIndex].tStreamData.dwFrameType = HISTORY_STREAM_TIME_JUMP;

					break;
				}
			case DATAPOP_EVENT_TIME_OVER:
				{
					m_PlaybackCfg[CurrIndex].tStreamData.dwFrameType = HISTORY_STREAM_TIME_END;

					break;
				}
			case DATAPOP_EVENT_TIME_CURR:
				{
					m_PlaybackCfg[CurrIndex].tStreamData.dwFrameType = HISTORY_STREAM_TIME_CURR;

					break;
				}
			case DATAPOP_EVENT_DISK_ERR:
				{
					m_PlaybackCfg[CurrIndex].tStreamData.dwFrameType = HISTORY_STREAM_EXCEPTION;

					break;
				}
		}

		m_PlaybackCfg[CurrIndex].tStreamData.cBuffer.dwBufLen = 0;
		m_PlaybackCfg[CurrIndex].tStreamData.cBuffer.pBuffer = NULL;

		m_PlaybackCfg[CurrIndex].tStreamData.dwChannel = iChannel;

		m_PlaybackCfg[CurrIndex].tStreamData.dwReserve[0] = 0;

#ifdef __DIGITAL_SUPPORT_SMARTP__
			m_PlaybackCfg[CurrIndex].tStreamData.dwReserve[1] = 1;
#else
			m_PlaybackCfg[CurrIndex].tStreamData.dwReserve[1] = 0;
#endif

		m_PlaybackCfg[CurrIndex].tStreamData.dwReserve[2] = 0;
#if 0
		m_PlaybackCfg[CurrIndex].tStreamData.cTime.wYear = 0;
		m_PlaybackCfg[CurrIndex].tStreamData.cTime.wMonth = 0;
		m_PlaybackCfg[CurrIndex].tStreamData.cTime.wDay = 0;
		m_PlaybackCfg[CurrIndex].tStreamData.cTime.wHour = 0;
		m_PlaybackCfg[CurrIndex].tStreamData.cTime.wMinute = 0;
		m_PlaybackCfg[CurrIndex].tStreamData.cTime.wSecond = 0;
#endif

		// 当前播放的时间超过查询的结束时间则发送结束帧
		if (FrameTime > m_PlaybackCfg[CurrIndex].tStreamStopTime)
		{
			m_PlaybackCfg[CurrIndex].tStreamData.dwFrameType = HISTORY_STREAM_TIME_END;
			bForceJump = 1;
			printf("current time greater than stop time, need stop!\n");
		}

		if (bForceJump)
		{						//强制事件时，先发送事件帧，再发正常帧
			m_PlaybackCfg[CurrIndex].dwDataFill = 1;
			//通知数据已达
			sem_post(&m_PlaybackCfg[CurrIndex].m_sem_Stream);
			while (1)
			{
				if (m_PlaybackCfg[CurrIndex].dwDataQuit)
				{
					printf("DPM callback quit,ch = %d <0x%08x>\n", iChannel, (int) hIndex);
					return ERR_PLAYBACK_FAILED;
				}
				if (!m_PlaybackCfg[CurrIndex].dwStreamStatus)
				{				//暂停
					printf("DPM callback pause,ch = %d <0x%08x>\n", iChannel, (int) hIndex);
					return ERR_PLAYBACK_FAILED;
				}

				sem_wait(&m_PlaybackCfg[CurrIndex].m_sem_StreamSend);
				if (m_PlaybackCfg[CurrIndex].dwDataFill)
				{				//数据未取走
					//usleep(2000);
					continue;
				}
				else
				{				//取走了
					break;
				}
			}
		}
	}

	if ((!bEvent) || bForceJump)
	{							//非事件帧，或者为强制事件
		if (dwSize > MAX_DATA_BUFFER_SIZE || dwSize <= 18)
		{
			printf("Frame length Error ,size = %d\n", (int) dwSize);
			return ERR_PLAYBACK_FAILED;
		}

		startCode = *((unsigned int *) pBuf);
		FrameType = wFrameType;

#if IDVR_BYTEORDER == IDVR_LIL_ENDIAN
		if (startCode != 0xB7010000)
#else
		if (startCode != 0x000001B7)
#endif
		{
			MPU_DBG("Error frame start code = 0x%08X\n", startCode);
		}
		if (0)//m_PlaybackCfg[CurrIndex].dwIFrame)
		{						//只I帧
			if (bForceJump)
			{
				m_PlaybackCfg[CurrIndex].dwFrameNo += 5;
			}
			else
			{
				m_PlaybackCfg[CurrIndex].dwFrameNo++;
			}
			*((int *) (pBuf + 5)) = m_PlaybackCfg[CurrIndex].dwFrameNo;

		}
		else
		{
			m_PlaybackCfg[CurrIndex].dwFrameNo = (DWORD) (*((int *) (pBuf + 4)));
		}

		m_PlaybackCfg[CurrIndex].tStreamData.cBuffer.dwBufLen = dwSize;
		m_PlaybackCfg[CurrIndex].tStreamData.cBuffer.pBuffer = pBuf;

		m_PlaybackCfg[CurrIndex].tStreamData.dwChannel = iChannel;

		m_PlaybackCfg[CurrIndex].tStreamData.dwFrameType = FrameType;

		m_PlaybackCfg[CurrIndex].tStreamData.dwReserve[0] = 0;
#ifdef __DIGITAL_SUPPORT_SMARTP__
		m_PlaybackCfg[CurrIndex].tStreamData.dwReserve[1] = 1;
#else
		m_PlaybackCfg[CurrIndex].tStreamData.dwReserve[1] = 0;
#endif

		m_PlaybackCfg[CurrIndex].tStreamData.dwReserve[2] = 0;

		localtime_r(&FrameTime, &tm_time);
		m_PlaybackCfg[CurrIndex].tStreamData.cTime.wYear = tm_time.tm_year;
		m_PlaybackCfg[CurrIndex].tStreamData.cTime.wMonth = tm_time.tm_mon;
		m_PlaybackCfg[CurrIndex].tStreamData.cTime.wDay = tm_time.tm_mday;
		m_PlaybackCfg[CurrIndex].tStreamData.cTime.wHour = tm_time.tm_hour;
		m_PlaybackCfg[CurrIndex].tStreamData.cTime.wMinute = tm_time.tm_min;
		m_PlaybackCfg[CurrIndex].tStreamData.cTime.wSecond = tm_time.tm_sec;
#if 0
printf("playback time stamp: %d-%d-%d %d:%d:%d\n", 
        tm_time.tm_year, tm_time.tm_mon, tm_time.tm_mday, 
        tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec);
#endif
	}

	m_PlaybackCfg[CurrIndex].dwDataFill = 1;
	//通知数据已达
	sem_post(&m_PlaybackCfg[CurrIndex].m_sem_Stream);

	while (1)
	{
		if (m_PlaybackCfg[CurrIndex].dwDataQuit)
		{
			MPU_DBG("DPM callback quit,ch = %d <0x%08x>\n", iChannel, (int) hIndex);
			return ERR_PLAYBACK_FAILED;
		}

		if (!m_PlaybackCfg[CurrIndex].dwStreamStatus)
		{						//暂停
			MPU_DBG("DPM callback pause,ch = %d <0x%08x>\n", iChannel, (int) hIndex);
			return ERR_PLAYBACK_FAILED;
		}

		sem_wait(&m_PlaybackCfg[CurrIndex].m_sem_StreamSend);
		if (m_PlaybackCfg[CurrIndex].dwDataFill)
		{						//数据未取走
			//usleep(2000);
			continue;
		}
		else
		{						//取走了
			break;
		}
	}


	//  MPU_DBG("HieServer_HistoryChannelStream send <%04d-%02d-%02d %02d:%02d:%02d>\n", tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday, tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec);

	return ERR_PLAYBACK_OK;
#else
	return ERR_PLAYBACK_FAILED;
#endif
}

//历史流工作线程
static void *RPB_WorkThread(void *lParam)
{
    PRT_TID();
#ifndef DEBUG_NOT_USE_STR_LIB

	BEGINRUN_THREAD;

	int CurrIndex;

	HANDLE CurrHandle, CurrSegHandle;

	HANDLE ReadyHandle;

	int result;
	pthread_detach(pthread_self());
	while (1)
	{
		MPU_DBG("wait....\n");
		sem_wait(&m_sem_create);
		MPU_DBG("wake up....\n");

		for (CurrIndex = 0; CurrIndex < MAX_PLAYBACK_SUPPORT; CurrIndex++)
		{
			//计算历史通道句柄及历史查询句柄
			CurrHandle = (HANDLE) ((m_PlaybackCfg[CurrIndex].dwHandeVerfyMark << PLAYBACK_MARK_SHIFT) | CurrIndex);
			CurrSegHandle = (HANDLE) ((m_PlaybackCfg[CurrIndex].dwQureyHandeVerfyMark << PLAYBACK_MARK_SHIFT) | CurrIndex);

			//弹出器已经创建成功，处理启停、定位等操作
			if (m_PlaybackCfg[CurrIndex].hPlayback != NULL && m_PlaybackCfg[CurrIndex].hPlayback != CurrHandle)
			{
				//停止弹出器
				if (m_PlaybackCfg[CurrIndex].dwStreamStop == 1)
				{				//停止
					m_PlaybackCfg[CurrIndex].dwStreamStop = 0;
					/*result = ZW_PkgSd_DataPopStop(m_PlaybackCfg[CurrIndex].hPlayback);
					if (0 != result)
					{
						printf("DPM_StopPop<handle = 0x%08x> failed\n", (int) CurrHandle);
						ReadyHandle = m_PlaybackCfg[CurrIndex].hPlayback;
						m_PlaybackCfg[CurrIndex].hPlayback = PLAYBACK_FAILED_HANDLE;
						ZW_PkgSd_DataPopRelease(ReadyHandle);

					}*/
					ZW_PkgSd_DataPopPause(m_PlaybackCfg[CurrIndex].hPlayback,1);
					m_PlaybackCfg[CurrIndex].dwStreamStatus = 0;

					MPU_DBG("DPM_StopPop index = 0x%08x,handle =0x%08x\n", (int) CurrHandle, (int) m_PlaybackCfg[CurrIndex].hPlayback);
				}

				//定位弹出器
				if (m_PlaybackCfg[CurrIndex].dwStreamSeek == 1)
				{
					m_PlaybackCfg[CurrIndex].dwStreamSeek = 0;
					result = ZW_PkgSd_DataPopSeek(m_PlaybackCfg[CurrIndex].hPlayback,m_PlaybackCfg[CurrIndex].tStreamStartTime);
					if (0 != result)
					{
						printf("DPM_Seek <handle = 0x%08x> failed,try to go back\n", (int) CurrHandle);
					}

					MPU_DBG("DPM_Seek index = 0x%08x,handle =0x%08x\n", (int) CurrHandle, (int) m_PlaybackCfg[CurrIndex].hPlayback);
				}

				//启动弹出器
				if (m_PlaybackCfg[CurrIndex].dwStreamStart == 1)
				{				//开始
					m_PlaybackCfg[CurrIndex].dwStreamStart = 0;
					m_PlaybackCfg[CurrIndex].dwStreamStatus = 1;
					ZW_PkgSd_DataPopPause(m_PlaybackCfg[CurrIndex].hPlayback,0);
					result = ZW_PkgSd_DataPopStart(m_PlaybackCfg[CurrIndex].hPlayback);
					if (0 != result)
					{
						printf("ZW_PkgSd_DataPopStart<handle = 0x%08x> failed\n", (int) CurrHandle);
						ReadyHandle = m_PlaybackCfg[CurrIndex].hPlayback;
						m_PlaybackCfg[CurrIndex].hPlayback = PLAYBACK_FAILED_HANDLE;
						ZW_PkgSd_DataPopStop(ReadyHandle);
						ZW_PkgSd_DataPopRelease(ReadyHandle);
					}
					printf("ZW_PkgSd_DataPopStart index = 0x%08x,handle =0x%08x\n", (int) CurrHandle, (int) m_PlaybackCfg[CurrIndex].hPlayback);
				}

			}

			//////////////////////////////////////////////////////////////////////////
			//片段查询创建
			if (m_PlaybackCfg[CurrIndex].hPlayback_info != NULL && m_PlaybackCfg[CurrIndex].hPlayback_info == CurrSegHandle)
			{
				MPU_DBG("Create Seg: diskgroup %lu, type 0x%x, ch %lu\n", m_PlaybackCfg[CurrIndex].dwSegDiskGroup,
						 (int) m_PlaybackCfg[CurrIndex].dwInfo_type, m_PlaybackCfg[CurrIndex].dwSegChannel);

				int nAllType[4], nCountType = 0, nCurType;
				int Type = 0;

				//普通录像
				if (m_PlaybackCfg[CurrIndex].dwInfo_type & GENERAL_RECORD)
				{
					nAllType[nCountType] = RECORD_TYPE_GENERAL;
					Type |= (1 << (RECORD_TYPE_GENERAL-1));// | 0x2
					++nCountType;
				}

				//手动录像类型
				if (m_PlaybackCfg[CurrIndex].dwInfo_type & MANUAL_RECORD)
				{
					nAllType[nCountType] = RECORD_TYPE_MANUAL;
					Type |= (1 << (RECORD_TYPE_MANUAL-1));// | 0x4
					++nCountType;
				}

				//移动录像类型
				if (m_PlaybackCfg[CurrIndex].dwInfo_type & MOTION_RECORD)
				{
					nAllType[nCountType] = RECORD_TYPE_MOTION;
					Type |= (1 << (RECORD_TYPE_MOTION-1));// | 0x8
					++nCountType;
				}

				//智能录像(人形/人脸/...)
				if (m_PlaybackCfg[CurrIndex].dwInfo_type & AI_RECORD)
				{
					nAllType[nCountType] = RECORD_TYPE_AI;
					Type |= (1 << (RECORD_TYPE_AI-1));// | 0x8
					++nCountType;
				}

				//报警录像类型
				if (m_PlaybackCfg[CurrIndex].dwInfo_type & ALARM_RECORD)
				{
					nAllType[nCountType] = RECORD_TYPE_ALARM;
					Type |= (1 << (RECORD_TYPE_ALARM-1));// | 0x1
					++nCountType;
				}

				for (nCurType = 0; nCurType < nCountType; ++nCurType)
				{
					if (nCurType == 0)
					{
						Type = 0xff;//测试
						m_PlaybackCfg[CurrIndex].dwHandle = ZW_PkgSd_GetRecSegList(
                                                                    m_PlaybackCfg[CurrIndex].hPlayback,
                                                                    m_PlaybackCfg[CurrIndex].tSegStartTime, 
                                                                    m_PlaybackCfg[CurrIndex].tSegStopTime, 
                                                                    Type, 
                                                                    (PSS_REGSEG_T **) (&m_PlaybackCfg[CurrIndex].pRecSegHead[nCurType]), 
                                                                    (int *) &m_PlaybackCfg[CurrIndex].dwRecSegCount[nCurType]);
						if((m_PlaybackCfg[CurrIndex].dwHandle != 0)&&(m_PlaybackCfg[CurrIndex].dwRecSegCount[nCurType] >= 0))
							result = 0;
						else
							result = -1;
					}
					else
					{
						result = 1;
					}

					if (0 != result)
					{
						m_PlaybackCfg[CurrIndex].pRecSegHead[nCurType] = NULL;
						m_PlaybackCfg[CurrIndex].dwRecSegCount[nCurType] = 0;
					}
					else if (NULL == m_PlaybackCfg[CurrIndex].pRecSegNext)
					{
						m_PlaybackCfg[CurrIndex].pRecSegNext = m_PlaybackCfg[CurrIndex].pRecSegHead[nCurType];
					}

					MPU_DBG("Create Seg: type %d, count %lu\n", nAllType[nCurType], m_PlaybackCfg[CurrIndex].dwRecSegCount[nCurType]);
				}

				//创建成功则修改查询句柄
				m_PlaybackCfg[CurrIndex].hPlayback_info = (HANDLE) (0xabcd0000 | CurrIndex);
				#ifdef NEW_QUERY
				sem_post(&m_sem_queryOver);
				#endif
				MPU_DBG("Create Seg: handle<0x%08x>, hPlayback_info<0x%08x>\n", (int) CurrSegHandle, (int) m_PlaybackCfg[CurrIndex].hPlayback_info);
			}

			//////////////////////////////////////////////////////////////////////////
			//数据流
			if (m_PlaybackCfg[CurrIndex].hPlayback != NULL && m_PlaybackCfg[CurrIndex].hPlayback == CurrHandle)
			{					//当前准备创建数据流
				int recType = 0;
				MPU_DBG("Create stream:type = 0x%x, diskgroup = %d, ch = %d...\n", (int) m_PlaybackCfg[CurrIndex].dwRec_type,
						 (int) m_PlaybackCfg[CurrIndex].dwStreamDiskGroup, (int) m_PlaybackCfg[CurrIndex].dwStreamChannel);
				result = 0;

				//普通录像
				if (m_PlaybackCfg[CurrIndex].dwRec_type & GENERAL_RECORD)
				{
					recType |= (1 << 0);//ZW_PkgSd_DataPopRecTypeAdd(ReadyHandle, 0);
				}
				
				//手动录像
				if (m_PlaybackCfg[CurrIndex].dwRec_type & MANUAL_RECORD)
				{
					
				}
				
				//移动侦测录像
				if (m_PlaybackCfg[CurrIndex].dwRec_type & MOTION_RECORD)
				{
					recType |= (1 << 1);//ZW_PkgSd_DataPopRecTypeAdd(ReadyHandle, 1);
				}
				
				//报警录像
				if (m_PlaybackCfg[CurrIndex].dwRec_type & ALARM_RECORD)
				{
					recType |= (1 << 2);//ZW_PkgSd_DataPopRecTypeAdd(ReadyHandle, 2);
				}

				//智能录像
				if (m_PlaybackCfg[CurrIndex].dwRec_type & AI_RECORD)
				{
					recType |= (1 << 3);//ZW_PkgSd_DataPopRecTypeAdd(ReadyHandle, 3);
				}

				ReadyHandle = ZW_PkgSd_DataPopCreate(m_PlaybackCfg[CurrIndex].tStreamStartTime, 
													   m_PlaybackCfg[CurrIndex].tStreamStopTime,
                                                       recType,
                                                       DataPop_Callback, 
                                                       NULL);
				if (ReadyHandle == NULL)
				{
					printf("DPM_Create failed handle = 0x%08x\n", (int) CurrHandle);
					m_PlaybackCfg[CurrIndex].hPlayback = PLAYBACK_FAILED_HANDLE;
					result = -1;
				}
				else
					result = 0;

				if (!result)
				{

					{
						//创建成功
						m_PlaybackCfg[CurrIndex].hPlayback = ReadyHandle;
						MPU_DBG("Create stream finished handle = 0x%08x\n", (int) CurrHandle);
						MPU_DBG("Create stream index = 0x%08x,handle =0x%08x\n", (int) CurrHandle, (int) m_PlaybackCfg[CurrIndex].hPlayback);
					}
				}
			}

			//////////////////////////////////////////////////////////////////////////
			//销毁历史流
			if (m_PlaybackCfg[CurrIndex].hPlayback != NULL && m_PlaybackCfg[CurrIndex].dwDataQuit)
			{
				printf("HY_DVR_HistoryChannelDestroy...start destroy <0x%08x>\n", (int) CurrHandle);
				if (m_PlaybackCfg[CurrIndex].hPlayback == CurrHandle || m_PlaybackCfg[CurrIndex].hPlayback == PLAYBACK_FAILED_HANDLE)
				{
					//销毁流回放线程
					if (0 != m_PlaybackCfg[CurrIndex].m_hStreamThread)
					{
						pthread_detach(m_PlaybackCfg[CurrIndex].m_hStreamThread);
						m_PlaybackCfg[CurrIndex].m_hStreamThread = 0;
					}

					printf("HY_DVR_HistoryChannelDestroy...sem destroy <0x%08x>\n", (int) CurrHandle);
					sem_destroy(&m_PlaybackCfg[CurrIndex].m_sem_Stream);
					sem_destroy(&m_PlaybackCfg[CurrIndex].m_sem_StreamSend);
					m_PlaybackCfg[CurrIndex].hPlayback = NULL;
					printf("HY_DVR_HistoryChannelDestroy...succ<0x%08x>\n", (int) CurrHandle);
				}
				else
				{
					//等待线程退出
					printf("HY_DVR_HistoryChannelDestroy...wait for thread exit <0x%08x>\n", (int) CurrHandle);
					if (0 != m_PlaybackCfg[CurrIndex].m_hStreamThread)
					{
						pthread_detach(m_PlaybackCfg[CurrIndex].m_hStreamThread);
						m_PlaybackCfg[CurrIndex].m_hStreamThread = 0;
					}

					printf("HY_DVR_HistoryChannelDestroy...dpm destroy <0x%08x>\n", (int) CurrHandle);
					ZW_PkgSd_DataPopStop(m_PlaybackCfg[CurrIndex].hPlayback);
					ZW_PkgSd_DataPopRelease(m_PlaybackCfg[CurrIndex].hPlayback);
					printf("HY_DVR_HistoryChannelDestroy...sem destroy <0x%08x>\n", (int) CurrHandle);
					sem_destroy(&m_PlaybackCfg[CurrIndex].m_sem_Stream);
					sem_destroy(&m_PlaybackCfg[CurrIndex].m_sem_StreamSend);
					m_PlaybackCfg[CurrIndex].hPlayback = NULL;
					printf("HY_DVR_HistoryChannelDestroy...succ<0x%08x>\n", (int) CurrHandle);
				}
			}
		}
	}

	ENDRUN_THREAD;
	return NULL;
#else
	return NULL;
#endif
}

int CIU_Playback_init()
{
	int result;

	HistoryChannelInterfaceMultiType hci;

	HistoryQueryInterfaceMultiType hqi;
	HistoryQueryInterfaceMultiTypeEx hqiEx;

	//初始化远程回放管理信息
	memset(m_PlaybackCfg, 0, MAX_PLAYBACK_SUPPORT * sizeof (PLAYBACK_CFG));

	//创建注册检测线程信号灯
	sem_init(&m_sem_create, 0, 0);
	sem_init(&m_sem_queryOver, 0, 0);
	//创建注册检测线程
	if(m_hCreateThread == 0)
	{
		result = pthread_create(&m_hCreateThread, NULL, RPB_WorkThread, NULL);
		if (result < 0)
		{
			printf("call pthread_create fail\n");
			return -1;
		}
	}
	memset(&hci, 0, sizeof (hci));
	memset(&hqi, 0, sizeof (hqi));

	//注册历史通道操作回调函数
	hci.ifHistoryChannelCreate = HY_DVR_HistoryChannelCreateEx;
	hci.ifHistoryChannelDestroy = HY_DVR_HistoryChannelDestroy;
	hci.ifHistoryChannelInsert = HY_DVR_HistoryChannelInsert;
	hci.ifHistoryChannelRemove = HY_DVR_HistoryChannelRemove;
	hci.ifHistoryChannelStartCapture = HY_DVR_HistoryChannelStartCapture;
	hci.ifHistoryChannelStopCapture = HY_DVR_HistoryChannelStopCapture;
	hci.ifHistoryChannelPosition = HY_DVR_HistoryChannelPosition;
	hci.ifHistoryChannelSetSpeed = HY_DVR_HistoryChannelSetSpeed;
	HieServer_HistoryChannelMultiTypeIf(&hci);

	//注册历史流查询接口
	hqi.ifHistoryQueryCreate = HY_DVR_HistoryQueryCreateEx;
	hqi.ifHistoryQueryDestroy = HY_DVR_HistoryQueryDestroy;
	hqi.ifHistoryQueryNext = HY_DVR_HistoryQueryNextEx;
	HieServer_HistoryQueryMultiTypeIf(&hqi);

#ifdef NEW_QUERY
	hqiEx.ifHistoryQueryCreate = HY_DVR_HistoryQueryCreateFile;
	HieServer_HistoryQueryMultiTypeRSFileIf(&hqiEx);
#endif
	return 0;
}
