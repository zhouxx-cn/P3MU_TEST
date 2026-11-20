/*!
 *\file		MPU_CIU_Preview.cpp
 *\brief	MPU模块，远程预览的实现
 *\author	林云
 *\date		2009/06/15
 */
#include "MPU_CIU_global.h"
#include "AVL/CChannelManager.h"
#include "MPU_CIU_Preview.h"
#include "MPU_LCD_API.h"
#ifdef __USE_CIRCLEQUEUE__
#include "CircleQueue.h"
#include "MemRecord.h"
#include "common_pub.h"
#endif

/* 私有帧头信息 */
typedef struct tagPrivateFrameHeader
{
    unsigned int        DataPacketStartCode;   // 固定为0xB7010000
    unsigned int        FrameNo;               // 媒体数据包的序号(音视频独立编号)
    unsigned char       FrameType;
    unsigned char       byLoadType; // 媒体数据包的信息,具体含义依赖于帧类型(H264:0x00,H265:        0x04,MJPEG:0x05)
    unsigned int        ShowTime;             // 表示媒体数据包相对于上一帧的偏差时间(频率90Khz)
    unsigned long       u32RealTimeSec;
    unsigned long       u32RealTimeUSec;
    unsigned short      uReserved;
    unsigned int        nOverloadLen;        // 表示媒体数据包的数据长度
}__attribute__((packed)) PRIVATEFRAMEHEADER, *LPPRIVATEFRAMEHEADER;

#if defined (__IS_SUPPORT_LCD_MANAGER__)

// 远程视频通话流私有帧头结构体，按 1 字节对齐，固定28字节
#pragma pack(push, 1)
typedef struct {
    uint32_t DataPacketStartCode;
    uint32_t FrameNo;
    uint8_t  FrameType;
    uint8_t  FrameInfo;
    uint32_t ShowTime;
    uint32_t Sec;
    uint32_t uSec;
    uint8_t  reserved[2];
    uint32_t nOverloadLen;
} zwFrameHeader;
#pragma pack(pop)

/* 定义位掩码常量（nCallState使用） */
#define STREAM_VOICE_MASK    0x01   /* 音频开启 */
#define STREAM_VIDEO_MASK    0x02   /* 视频开启 */

#define VIDEO_SAVE_FILE_TEST  (0)
typedef struct _VIDEOCALL_CFG_
{
    BYTE nCallState;    /* 视频通话状态：0x00空闲 0x01语音通话中 0x02视频通话中 0x03音视频通话中 */
    DWORD nUserId;      /* 用户ID，当前无效 */
	eVideoMode nVMode;  /* 视频通话视频流编码类型            */
    eVoiceMode nAMode;  /* 视频通话语音流编码类型 */    
#if (VIDEO_SAVE_FILE_TEST)
    FILE *pFd;
#endif
} VIDEOCALL_CFG;
static VIDEOCALL_CFG g_VideoCallCfg;

/* 视频通话变量调用互斥锁 */
static pthread_mutex_t g_mutexVideoCall = PTHREAD_MUTEX_INITIALIZER;
#endif

#ifdef __USE_CIRCLEQUEUE__
//循环缓冲队列句柄
static CQ_HANDLE g_CircleQueueHandler = {0}; 
static CQ_HANDLE g_MjpegCircleQueueHandler[MAX_CH_NUM] = {0}; 

//共流缓冲区创建失败标识
static int 	g_CircleCreateFailedFlag = 0;

//!\brief 初始化循环缓冲队列
static int CircleQueueInit(void)
{
	CQ_PARAM_S stCQParam;
#if (MAX_CH_NUM == 1)	
		double nMainMaxSize = {MAIN_SUB_QUEUE_SIZE};
#elif (MAX_CH_NUM == 2)
		double nMainMaxSize = {MAIN_SUB_QUEUE_SIZE+SECOND_MAIN_SUB_QUEUE_SIZE};
#elif (MAX_CH_NUM == 3)	
		double nMainMaxSize = {MAIN_SUB_QUEUE_SIZE+SECOND_MAIN_SUB_QUEUE_SIZE+ SECOND_MAIN_SUB_QUEUE_SIZE};
#elif (MAX_CH_NUM == 4)
		double nMainMaxSize = {MAIN_SUB_QUEUE_SIZE+SECOND_MAIN_SUB_QUEUE_SIZE+SECOND_MAIN_SUB_QUEUE_SIZE+SECOND_MAIN_SUB_QUEUE_SIZE};
#endif

	/* 主子码流流缓冲队列 */
    stCQParam.s32MaxSize = (int)nMainMaxSize;
    stCQParam.s32MaxDuration = QUEUE_DURATION;	 //帧数据限制最大长度10s
    stCQParam.enMode = CQ_MODE_BYPASS;   //数据直接入队列，队列内不进行分配空间拷贝

    g_CircleQueueHandler = CircleQueue_Create(&stCQParam);

    /* Mjpeg流缓冲队列 */
    for (int SnsIdx = 0; SnsIdx < MAX_CH_NUM; SnsIdx++)
    {
        stCQParam.s32MaxSize = MJPEG_QUEUE_SIZE;
        stCQParam.s32MaxDuration = QUEUE_DURATION;   //帧数据限制最大长度10s
        stCQParam.enMode = CQ_MODE_BYPASS;   //数据直接入队列，队列内不进行分配空间拷贝
        g_MjpegCircleQueueHandler[SnsIdx] = CircleQueue_Create(&stCQParam);

        if (CQ_INVALID_HANDLE == g_CircleQueueHandler || CQ_INVALID_HANDLE == g_MjpegCircleQueueHandler[SnsIdx])
        {
            g_CircleCreateFailedFlag = 1;
            OPEN_LOG_E("CircleQueueInit Failed! [%u] [%d--%u]\n", g_CircleQueueHandler, SnsIdx, g_MjpegCircleQueueHandler[SnsIdx]);
        }
    }

    return 0;
}

//!\brief 协议添加循环缓冲区消费者
//!\param eProtocolID eProtocol 协议类型
//!\param DWORD dwUserID 用户ID
//!\param DWORD dwChannel 通道，从0开始
//!\param eMediaCodeType eType   码流类型
//!\param ivoid * vReserve 保留字段指针
//!\param DWORD *pdwConnectionId 输出参数，连接ID
//!\return 
static int NetCircleQueueAddUser(  eProtocolID eProtocol,DWORD dwUserID, DWORD dwChannel, 
										eMediaCodeType eType, void * vReserve,DWORD *pdwConnectionId)
{
    MPU_INFO("NetCircleQueueAddUser eProtocol(%d) dwUserID(%ld) dwChannel(%ld) eType(%d)\n", eProtocol, dwUserID, dwChannel, eType);
	CQ_USER_PARAM_S stUserParam;
	CQ_USER hUser;

	//约束中间层只能获取MJPEG帧,前提是中间层业务调用需要使用eProtocolBegin作为协议类型
	if ((eProtocolBegin == eProtocol) && (eType != eMediaCodefourthStream))
	{
		MPU_ERR("Only get Mjpeg stream!!!\n");
		return -1;
	}

	if (NULL == pdwConnectionId)
	{
		MPU_ERR("NetCircleQueueAddUser pdwConnectionId is NULL\n");
		return -1;
	}

	stUserParam.s32Channel = dwChannel;
	//协议码流索引跟中间层映射关系
	if (eMediaCodeMain == eType)
	{
		stUserParam.s32Stream = STREAM_TYPE_MAIN;
	}
	else if (eMediaCodeAssist == eType)
	{
		stUserParam.s32Stream = STREAM_TYPE_AUX;
	}
	else if (eMediaCodeHDAssist == eType)
	{
		stUserParam.s32Stream = STREAM_TYPE_HDAUX;
	}
	else if (eMediaCodefourthStream == eType)
	{
		stUserParam.s32Stream = STREAM_TYPE_FOURTH;
	}
	else if (0xff == eType)
	{
		//全码流
		stUserParam.s32Stream = 0xff;
	}
	else
	{
		MPU_ERR("NetCircleQueueAddUser err eType = %d not support\n",eType);
		return -1;
	}
	
	if (eMediaCodefourthStream == eType)
	{
		hUser = CircleQueue_AddUser(g_MjpegCircleQueueHandler[dwChannel], &stUserParam);
        MPU_INFO("CircleQueue_AddUser hUser = %u\n", hUser);
		if (0 == hUser)
		{
			MPU_ERR("CircleQueue_AddUser err hUser = %d\n",hUser);
			return -1;
		}
	}
	else
	{
		hUser = CircleQueue_AddUser(g_CircleQueueHandler, &stUserParam);
        MPU_INFO("CircleQueue_AddUser hUser = %u\n", hUser);
		if (0 == hUser)
		{
			MPU_ERR("CircleQueue_AddUser err hUser = %d\n",hUser);
			return -1;
		}
	}

    *pdwConnectionId = hUser;

#ifdef __ONE_CLICK_CALL__
	//通知一键呼叫线程直播流开启传递当前ConnectionId
	if(eProtocolTUTK == eProtocol)
	{
		CIU_CALL_GetCallConnectionId(hUser);
	}
#endif

    if (XRS_CQ_ERR_INVALID_HANDLE == hUser || XRS_CQ_ERR_TOOMUCH_USER == hUser)
    {
        return hUser;
    }

    return 0;
}

//!\brief 协议删除循环缓冲区消费者
//!\param DWORD pdwConnectionId 连接ID
//!\param ivoid * vReserve 保留字段指针
//!\return 
static int NetCircleQueueDeleteUser(DWORD pdwConnectionId, void * vReserve)
{
    MPU_INFO("NetCircleQueueDeleteUser pdwConnectionId = %ld\n", pdwConnectionId);

#ifdef __ONE_CLICK_CALL__
	//通知一键呼叫线程直播流关闭传递当前ConnectionId
	CIU_CALL_GetHangupConnectionId(pdwConnectionId);
#endif

    int ret = CircleQueue_DeleteUser(pdwConnectionId);

    MPU_INFO("NetCircleQueueDeleteUser pdwConnectionId = %ld ret = %d\n", pdwConnectionId, ret);
	return ret;
}

//!\brief 协议获取循环缓冲区数据
//!\param DWORD pdwConnectionId 连接ID
//!\param LPRealStreamMedia pRealStreamMedia 实时流数据信息
//!\param ivoid * vReserve 保留字段指针
//!\return 
static int NetCircleQueueGetData(DWORD dwConnectionId, LPRealStreamMedia pRealStreamMedia,void * vReserve)
{
	CQ_DATA_S stData;
	struct tm tm_time;
	CChannelManager *pMgr = CChannelManager::GetInstanceHandle();
	int nType = 0;
	unsigned int tFrameTime;
	unsigned int tFrameTime_usec;
	time_t tFrameTimeTemp;

	if (NULL == pRealStreamMedia || NULL == pMgr)
	{
		MPU_ERR("NetCircleQueueGetData pRealStreamMedia is NULL\n");
		return -1;
	}

    int ret = CircleQueue_GetData(dwConnectionId, &stData);
    if (0 != ret)
    {
		return -1;
    }
    MPU_DBG("NetCircleQueueGetData dwConnectionId = %lu\n", dwConnectionId);

    if (STREAM_TYPE_MAIN == stData.s32Stream || 
    	STREAM_TYPE_AUX == stData.s32Stream || 
    	STREAM_TYPE_HDAUX == stData.s32Stream)
    {
		nType = pMgr->GetStreamType(stData.s32Stream);
		if ((PktAudioFrames == stData.s32FrameType) && (1 != nType))
		{
			//码流类型为视频流时,协议获取数据时过滤音频数据
			CircleQueue_ReleaseData(dwConnectionId, &stData);
			return -1;
		}
    }

	stData.s32Stream = (stData.s32Stream == 1) ? 2 : (stData.s32Stream == 0) ? 1 : stData.s32Stream; //全码流类型转换 1:主码流 2:子码流

    tFrameTime = stData.s64Timestamp / 1000000;
    tFrameTime_usec = stData.s64Timestamp % 1000000;
	tFrameTimeTemp = tFrameTime;

    pRealStreamMedia->eType = eMainVideo;	// 无实际意义
	pRealStreamMedia->cBuffer.pBuffer = (BYTE *) stData.pData;
	pRealStreamMedia->cBuffer.dwBufLen = stData.s32DataSize;
	pRealStreamMedia->dwFrameType = stData.s32FrameType;
	localtime_r(& tFrameTimeTemp, &tm_time);
	pRealStreamMedia->cTime.wYear = tm_time.tm_year + 1900;
	pRealStreamMedia->cTime.wMonth = tm_time.tm_mon + 1;
	pRealStreamMedia->cTime.wDay = tm_time.tm_mday;
	pRealStreamMedia->cTime.wHour = tm_time.tm_hour;
	pRealStreamMedia->cTime.wMinute = tm_time.tm_min;
	pRealStreamMedia->cTime.wSecond = tm_time.tm_sec;
	pRealStreamMedia->cTime.wWeek = tm_time.tm_wday;
	pRealStreamMedia->cTime.wMillisecond = tFrameTime_usec / 1000;
    pRealStreamMedia->dwReserve[0] = (stData.s32Channel << 16) ;
    pRealStreamMedia->dwReserve[0] |= stData.s32Stream;
#ifdef __DIGITAL_SUPPORT_SMARTP__
	pRealStreamMedia->dwReserve[1] = 1;
#else
	pRealStreamMedia->dwReserve[1] = 0;
#endif
	pRealStreamMedia->dwReserve[2] = 0;
	
	PRIVATEFRAMEHEADER* RawHead = (PRIVATEFRAMEHEADER*)stData.pData;
	if (RawHead->uReserved >= 10000)
	{
		unsigned char *bytes = (unsigned char *)&RawHead->uReserved;
		bytes[1] = 0x01;
	}

	return 0;
}

//!\brief 协议释放循环缓冲区数据
//!\param DWORD pdwConnectionId 连接ID
//!\param LPRealStreamMedia pRealStreamMedia 实时流数据信息
//!\param ivoid * vReserve 保留字段指针
//!\return 
static int NetCircleQueueReleaseData(DWORD dwConnectionId, LPRealStreamMedia pRealStreamMedia,void * vReserve)
{
	CQ_DATA_S stData;

	if (NULL == pRealStreamMedia)
	{
		MPU_ERR("NetCircleQueueReleaseData pRealStreamMedia is NULL\n");
		return -1;
	}

	stData.pData = pRealStreamMedia->cBuffer.pBuffer;
	stData.s32DataSize = pRealStreamMedia->cBuffer.dwBufLen;

	return CircleQueue_ReleaseData(dwConnectionId,&stData);
}


//!\brief 录像添加循环缓冲区消费者
//!\param DWORD dwChannel 通道，从0开始
//!\param unsigned int dwStreamType   码流类型
//!\param DWORD *pdwConnectionId 输出参数，连接ID
//!\return 
static int RecordCircleQueueAddUser(unsigned int dwChannel,unsigned int dwStreamType,unsigned int *pdwConnectionId)
{
	CQ_USER_PARAM_S stUserParam;

	if (NULL == pdwConnectionId)
	{
		MPU_ERR("RecordCircleQueueAddUser pdwConnectionId is NULL\n");
		return -1;
	}

	if (dwChannel >= MAX_CH_NUM)
	{
		MPU_ERR("dwChannel (%d) Not Support. MAX_CH_NUM(%d)\n",dwChannel,MAX_CH_NUM);
		return -1;
	}

	stUserParam.s32Channel = 0xff;
	stUserParam.s32Stream = dwStreamType;
	
    CQ_USER hUser = CircleQueue_AddUser(g_CircleQueueHandler, &stUserParam);
    if ((0 == hUser) || 
        (XRS_CQ_ERR_INVALID_HANDLE == hUser))
    {
    	MPU_ERR("RecordCircleQueueAddUser hUser = %d\n",hUser);
		return -1;
    }

    *pdwConnectionId = hUser;

    if (XRS_CQ_ERR_INVALID_HANDLE == hUser || XRS_CQ_ERR_TOOMUCH_USER == hUser)
    {
        return hUser;
    }

    return 0;
}


//!\brief 录像删除循环缓冲区消费者
//!\param unsigned int pdwConnectionId 连接ID
//!\return 
static int RecordCircleQueueDeleteUser(unsigned int pdwConnectionId)
{
	return CircleQueue_DeleteUser(pdwConnectionId);
}


//!\brief 录像获取循环缓冲区数据
//!\param unsigned int pdwConnectionId 连接ID
//!\param char **pBuffer 流数据
//!\param unsigned int *pLen 数据长度
//!\return -1: 异常状态 -2:缓冲区无数据 -3: 非录像所需码流
static int RecordCircleQueueGetData(unsigned int dwConnectionId, char **pBuffer,unsigned int *pLen)
{
	CQ_DATA_S stData;
	int ret = 0;
	int nRecordIndex = 0;
	CChannelManager *pMgr = CChannelManager::GetInstanceHandle();

	if (NULL == pBuffer || NULL == pLen)
	{
		return -1;
	}


	ret = CircleQueue_GetData(dwConnectionId, &stData);
	

	if (XRS_CQ_ERR_TAIL == ret)
	{
		//缓冲区无数据
		return -2;
	}
	
    if (0 != ret)
    {
    	MPU_ERR("CircleQueue_GetData failed = %d\n", ret);
		return -1;
    }

    //等IR-CUT切换完毕后再开始录像
	if (0 == GetSystemStartOverFlag())
	{
		//保证读的序列号保持最新
		CircleQueue_ReleaseData(dwConnectionId, &stData);
		return -1;
	}

	//多目同时录像，同主或者同子
    nRecordIndex = pMgr->GetRecStreamIndex(0);

	//录像码流与实时流数据码流一致
    if (nRecordIndex == stData.s32Stream)
    {
		if ((PktAudioFrames != stData.s32FrameType) &&
			(PktIFrames != stData.s32FrameType) &&
			(PktPFrames != stData.s32FrameType) &&
			(PktBPFrames != stData.s32FrameType) &&
			(PktBBPFrames != stData.s32FrameType) &&
			(PktQCIFIFrames != stData.s32FrameType) &&
			(PktQCIFPFrames != stData.s32FrameType) &&
			(PktSQCIFIFrames != stData.s32FrameType) &&
			(PktSQCIFPFrames != stData.s32FrameType) &&
			(PktIFramesForS1 != stData.s32FrameType) &&
			(PktPFramesForS1 != stData.s32FrameType) &&
			(PktQCIFIFramesForS1 != stData.s32FrameType) &&
			(PktQCIFPFramesForS1 != stData.s32FrameType) &&
			(PktIFramesForS2 != stData.s32FrameType) &&
			(PktPFramesForS2 != stData.s32FrameType) &&
			(PktQCIFIFramesForS2 != stData.s32FrameType) &&
			(PktQCIFPFramesForS2 != stData.s32FrameType) &&
			(PktIFramesForS3 != stData.s32FrameType) &&
			(PktPFramesForS3 != stData.s32FrameType) &&
			(PktQCIFIFramesForS3 != stData.s32FrameType) &&
			(PktQCIFPFramesForS3 != stData.s32FrameType))
		{
			/* 非需要录像的帧类型, 释放掉 */
			CircleQueue_ReleaseData(dwConnectionId, &stData);
			return -3;
		}
    }
    else
    {
    	/* 非录像码流帧, 释放掉 */
		CircleQueue_ReleaseData(dwConnectionId, &stData);
		return -3;
    }

    *pBuffer = (char *)stData.pData;
    *pLen = stData.s32DataSize;

	return 0;
}


//!\brief 录像释放循环缓冲区数据
//!\param unsigned int pdwConnectionId 连接ID
//!\param char *pBuffer 流数据
//!\param unsigned int pLen 数据长度
//!\return 
static int RecordCircleQueueReleaseData(unsigned int dwConnectionId, char *pBuffer,unsigned int pLen)
{
	CQ_DATA_S stData;

	if (NULL == pBuffer)
	{
		MPU_ERR("RecordCircleQueueReleaseData pBuffer is NULL\n");
		return -1;
	}

	stData.pData = (uint8_t *)pBuffer;
	stData.s32DataSize = pLen;

	return CircleQueue_ReleaseData(dwConnectionId,&stData);
}

//!\brief 子码流(AUX,240x320)循环缓冲区取数据：仅返回子码流视频帧的H264裸流(剥离28字节私有帧头)
//!\param unsigned int dwConnectionId 连接ID
//!\param char **pBuffer 输出: H264裸流起始(已跳过私有帧头)
//!\param unsigned int *pLen 输出: H264裸流长度
//!\return 0:成功 -1:异常 -2:缓冲区无数据 -3:非子码流视频帧(已释放)
static int SubStreamCircleQueueGetData(unsigned int dwConnectionId, char **pBuffer, unsigned int *pLen)
{
	CQ_DATA_S stData;
	int ret = 0;

	if (NULL == pBuffer || NULL == pLen)
	{
		return -1;
	}

	ret = CircleQueue_GetData(dwConnectionId, &stData);
	if (XRS_CQ_ERR_TAIL == ret)
	{
		//缓冲区无新数据
		return -2;
	}
	if (0 != ret)
	{
		return -1;
	}

	//仅保留子码流(STREAM_TYPE_AUX)的视频帧(PktQCIFIFrames/PktQCIFPFrames)
	if ((STREAM_TYPE_AUX != stData.s32Stream) ||
		((PktQCIFIFrames != stData.s32FrameType) && (PktQCIFPFrames != stData.s32FrameType)))
	{
		CircleQueue_ReleaseData(dwConnectionId, &stData);
		return -3;
	}

	//循环队列节点 = [28字节私有帧头 PRIVATEFRAMEHEADER] + [H264裸流]
	if (stData.s32DataSize <= (int)sizeof(PRIVATEFRAMEHEADER))
	{
		CircleQueue_ReleaseData(dwConnectionId, &stData);
		return -3;
	}

	//剥离私有帧头, 向上层返回纯 H264 裸流(释放时只按消费者句柄释放, 不依赖该指针)
	*pBuffer = (char *)stData.pData + sizeof(PRIVATEFRAMEHEADER);
	*pLen    = (unsigned int)(stData.s32DataSize - (int)sizeof(PRIVATEFRAMEHEADER));

	return 0;
}

//!\brief 获取子码流(AUX,240x320)循环缓冲区消费接口(用法参考录像写卡 MemRecord)
//!\param LPRecordInterface pInterface 输出: 取流回调集合
//!\return 0 成功, -1 失败
int CIU_GetSubStreamRecordInterface(LPRecordInterface pInterface)
{
	if (NULL == pInterface)
	{
		MPU_ERR("CIU_GetSubStreamRecordInterface pInterface is NULL\n");
		return -1;
	}

	if (CQ_INVALID_HANDLE == g_CircleQueueHandler || 0 == g_CircleQueueHandler)
	{
		MPU_ERR("CIU_GetSubStreamRecordInterface CircleQueue not init!\n");
		return -1;
	}

	//确保子码流编码器已开启, 使 240x320 子码流持续写入循环缓冲区
	MPU_AVL_AuxCoder_Enable(0, 1);

	pInterface->ifRecordUserAdd     = RecordCircleQueueAddUser;
	pInterface->ifRecordUserDelete  = RecordCircleQueueDeleteUser;
	pInterface->ifRecordGetData     = SubStreamCircleQueueGetData;
	pInterface->ifRecordReleaseData = RecordCircleQueueReleaseData;

	return 0;
}

//!\brief 设备向循环队列发送数据的回调函数
//!\param int nCh 通道
//!\param char *Buff 帧数据
//!\param int nBuffLen 数据长度
//!\param int Type   帧类型
//!\param int nStreamType 流类型
//!\param unsigned int tFrameTime 时间秒
//!\param  unsigned int tFrameTime_usec 时间微秒
//!\return 
static int CircleQueueAvCallBack(int nCh, char *Buff, int nBuffLen, int Type, int nStreamType, unsigned int tFrameTime, unsigned int tFrameTime_usec)
{
	CQ_DATA_S stData = {0};
	int result = 0;
	char *pdata = NULL;
	
	if (g_CircleCreateFailedFlag)
	{
		MPU_ERR("g_CircleQueueHandler or g_MjpegCircleQueueHandler create failed!\n");
		if (NULL != Buff && ((PktIFrames == Type) || (PktPFrames == Type) ||
            (PktIFramesForS1 == Type) || (PktPFramesForS1 == Type) ||
            (PktQCIFIFramesForS1 == Type) || (PktQCIFPFramesForS1 == Type) ||
            (PktIFramesForS2 == Type) || (PktPFramesForS2 == Type) ||
            (PktQCIFIFramesForS2 == Type) || (PktQCIFPFramesForS2 == Type) ||
            (PktIFramesForS3 == Type) || (PktPFramesForS3 == Type) ||
            (PktQCIFIFramesForS3 == Type) || (PktQCIFPFramesForS3 == Type) ||
			(PktBPFrames == Type) || (PktBBPFrames == Type) || 
			(PktQCIFIFrames == Type) || (PktQCIFPFrames == Type) || 
			(PktSQCIFIFrames == Type) || (PktSQCIFPFrames == Type) || 
			(PktFourthIFrames == Type) || (PktFourthPFrames == Type) ||
			(PktFourthIFramesForS1 == Type) || (PktFourthPFramesForS1 == Type) ||
			(PktFourthIFramesForS2 == Type) || (PktFourthPFramesForS2 == Type) ||
			(PktFourthIFramesForS3 == Type) || (PktFourthPFramesForS3 == Type)))
		{
			free(Buff);
			Buff = NULL;
		}
		return -1;
	}

	if ((PktFourthIFrames != Type) &&
		(PktFourthPFrames != Type) &&
		(PktFourthIFramesForS1 != Type) &&
		(PktFourthPFramesForS1 != Type) &&
		(PktFourthIFramesForS2 != Type) &&
		(PktFourthPFramesForS2 != Type) &&
		(PktFourthIFramesForS3 != Type) &&
		(PktFourthPFramesForS3 != Type) &&
		(PktIFrames != Type) &&
		(PktPFrames != Type) &&
		(PktBPFrames != Type) &&
		(PktBBPFrames != Type) &&
		(PktQCIFIFrames != Type) &&
		(PktQCIFPFrames != Type) &&
		(PktSQCIFIFrames != Type) &&
        (PktIFramesForS1 != Type) &&
        (PktPFramesForS1 != Type) &&
        (PktQCIFIFramesForS1 != Type) &&
        (PktQCIFPFramesForS1 != Type) &&
        (PktIFramesForS2 != Type) &&
        (PktPFramesForS2 != Type) &&
        (PktQCIFIFramesForS2 != Type) &&
        (PktQCIFPFramesForS2 != Type) &&
        (PktIFramesForS3 != Type) &&
        (PktPFramesForS3 != Type) &&
        (PktQCIFIFramesForS3 != Type) &&
        (PktQCIFPFramesForS3 != Type) &&
		(PktSQCIFPFrames != Type))
	{
		//非视频流数据，分配空间。视频流数据sdk中动态分配空间
		pdata = (char *)malloc(nBuffLen);
		if (NULL == pdata)
		{
			MPU_ERR("g_CircleQueueHandler malloc failed!\n");
			return -1;
		}

		memcpy(pdata,Buff,nBuffLen);
	}
	else
	{
		pdata = Buff;
	}
    
	switch(Type)
	{
		case PktIFramesForS1:
            nCh = 1;    Type = PktIFrames; break;
		case PktPFramesForS1:
            nCh = 1;    Type = PktPFrames; break;
		case PktQCIFIFramesForS1:
            nCh = 1;    Type = PktQCIFIFrames; break;
        case PktQCIFPFramesForS1:
            nCh = 1;    Type = PktQCIFPFrames; break;
		case PktFourthIFramesForS1:
            nCh = 1;    Type = PktFourthIFrames; break;
        case PktFourthPFramesForS1:
            nCh = 1;    Type = PktFourthPFrames; break;
        case PktIFramesForS2:
            nCh = 2;    Type = PktIFrames; break;
		case PktPFramesForS2:
            nCh = 2;    Type = PktPFrames; break;
		case PktQCIFIFramesForS2:
            nCh = 2;    Type = PktQCIFIFrames; break;
        case PktQCIFPFramesForS2:
            nCh = 2;    Type = PktQCIFPFrames; break;
		case PktFourthIFramesForS2:
            nCh = 2;    Type = PktFourthIFrames; break;
        case PktFourthPFramesForS2:
            nCh = 2;    Type = PktFourthPFrames; break;
        case PktIFramesForS3:
            nCh = 3;    Type = PktIFrames; break;
		case PktPFramesForS3:
            nCh = 3;    Type = PktPFrames; break;
		case PktQCIFIFramesForS3:
            nCh = 3;    Type = PktQCIFIFrames; break;
        case PktQCIFPFramesForS3:
            nCh = 3;    Type = PktQCIFPFrames; break;
		case PktFourthIFramesForS3:
            nCh = 3;    Type = PktFourthIFrames; break;
        case PktFourthPFramesForS3:
            nCh = 3;    Type = PktFourthPFrames; break;
        default: break;
    }

	stData.pData = (uint8_t *)pdata;
	stData.s32DataSize = nBuffLen;
	stData.s32Channel = nCh;
	stData.s32Stream = nStreamType;
	stData.s32FrameType = Type;
	stData.s64Timestamp = tFrameTime;
	stData.s64Timestamp = stData.s64Timestamp*1000000;
	stData.s64Timestamp += tFrameTime_usec;
	if ((PktFourthIFrames == Type) || (PktFourthPFrames == Type))
	{
		result = CircleQueue_AddData(g_MjpegCircleQueueHandler[nCh], &stData);
		if (0 != result)
		{
			printf("--- MJPEG CircleQueue_AddData err reuslt = %d, nCh = %d\n", result, nCh);
			free(pdata);
		}
	}
	else
	{
		result = CircleQueue_AddData(g_CircleQueueHandler, &stData);
		if (0 != result)
		{
			printf("--- CircleQueue_AddData err reuslt = %d, nCh = %d\n", result, nCh);
			free(pdata);
		}
	}

	return result;
}

//!\brief   初始化远程预览
int CIU_GetMjpegStreamCircleQueueInterface(LPPullRealStreamInterface pMjpegStreamInterface)
{
	MPU_DBG("=======CIU_RegisterPreview======\n");
    if (g_MjpegCircleQueueHandler[0] == 0)
    {
        MPU_ERR("CircleQueueInterface not init!\n");
        return -1;
    }
	//设置网络拉流方式来获取流数据回调接口
	pMjpegStreamInterface->ifRealStreamConnectionAdd = NetCircleQueueAddUser;
	pMjpegStreamInterface->ifRealStreamConnectionDelete = NetCircleQueueDeleteUser;
	pMjpegStreamInterface->ifRealStreamConnectionGetData = NetCircleQueueGetData;
	pMjpegStreamInterface->ifRealStreamConnectionReleaseData = NetCircleQueueReleaseData;

	return 0;
}
#endif

static int m_PreviewStream = eMediaCodeMain;
int MPU_CIU_PreView_GetPreviewStream()
{
	return m_PreviewStream;
}

int StreamStartCapture(DWORD dwUserID, eMediaCodeType eType, DWORD dwChannel)
{
	MPU_INFO("dwUserID(%lu) type(%d), CH(%u)\n", dwUserID, (int) eType, dwChannel);
	MPU_CHK(dwChannel < 16);

	if (0 == CIU_CheckRemoteUser(dwUserID, CHANNEL_PREVIEW, dwChannel))
	{
		MPU_ERR("no config right  :CHANNEL_PREVIEW\n");
		return IDVR_CIU_NO_RIGHT;
	}

#ifdef __ONE_CLICK_CALL__
	//通知一键呼叫线程直播流开启
	CIU_CALL_SetRecvCallState(3);
#endif

	m_PreviewStream = (int )eType;

	if (eMediaCodeAssist == eType)
	{
		MPU_AVL_AuxCoder_Enable(dwChannel, 1);
		MPU_DBG("AuxCoder_Enable %lu %d\n", dwChannel, 1);
	}
#ifdef USE_HDAUX
	else if (eMediaCodeHDAssist == eType)
	{
		MPU_AVL_HDAuxCoder_Enable(dwChannel, 1);
		MPU_DBG("HDAuxCoder_Enable %lu %d\n", dwChannel, 1);
	}
#endif

    static int first_RealStream = 1;
	if (1 == first_RealStream)
	{
	    //设备不支持分享时，创建标记文件。用于修复绑定时没有S码造成存在8分钟的扫码时间，影响正常功能
        if (0 == IsSupportDevShareFunc() && access("/data/config/config/noSupportShare",F_OK) != 0  && 
            access("/dev/tmp/app/disk/mmcblk0p1/output/ProductTest",F_OK))
		{
			mysystem("touch /data/config/config/noSupportShare");
			MPU_INFO("touch 'noSupportShare' success\n");
		}
		first_RealStream = 0;
	}
	return 0;
}

int StreamStopCapture(DWORD dwUserID, eMediaCodeType eType, DWORD dwChannel)
{
	MPU_INFO("dwUserID(%lu) type(%d), CH(%u)\n", dwUserID, (int) eType, dwChannel);
	MPU_CHK(dwChannel < 16);
	if (eMediaCodeAssist == eType)
	{
		//切换到主码流时字码流不关闭
		//MPU_AVL_AuxCoder_Enable(dwChannel, 0); // 1 开启子码流， 0 关闭子码流
		MPU_DBG("AuxCoder_Enable %lu %d\n", dwChannel, 0);
	}
#ifdef USE_HDAUX
	else if (eMediaCodeHDAssist == eType)
	{
		MPU_AVL_HDAuxCoder_Enable(dwChannel, 0);
		MPU_DBG("HDAuxCoder_Enable %lu %d\n", dwChannel, 0);
	}
#endif
#ifdef __ONE_CLICK_CALL__
	//通知一键呼叫线程直播流关闭
	CIU_CALL_SetRecvCallState(4);
#endif

	m_PreviewStream = eMediaCodeMain;
	return 0;
}

int StreamAccessProc(DWORD dwUserID, LPRealStreamAccess cRealStreamAccess)
{
    static int nSeeStreamCnt = 0;

	if (!cRealStreamAccess)
	{
		return -1;
	}

    if (0 == cRealStreamAccess->m_dwAction)	//连接
	{
		nSeeStreamCnt++;
	}
	else	//断开
	{
		if (nSeeStreamCnt > 0)
		{
			nSeeStreamCnt--;
		}
	}

    if (nSeeStreamCnt == 0)
    {
        //通知低帧模式关闭-SLOWFRAMESCENE_CLOSESTREAM
        ZWSys_EncSetSlowFrameMode(4);
    }
    else if(nSeeStreamCnt > 0)
    {
        //通知低帧模式改变-SLOWFRAMESCENE_OPENSTREAM
        ZWSys_EncSetSlowFrameMode(3);
    }

	return CIU_Update_RealStream_Connect_INFO( (int)cRealStreamAccess->m_dwChannel, (int)cRealStreamAccess->m_eStreamType, (int)cRealStreamAccess->m_dwAction);
}

int VoiceStreamCreate(DWORD dwUserId, DWORD dwChannel, eVoiceMode eMode)
{
	MPU_DBG("VoiceStreamCreate (ch %lu, mode %d)\n", dwChannel, eMode);

	if (0 == CIU_CheckRemoteUser(dwUserId, REMOTE_VOICE))
	{
		MPU_ERR("no config right  :REMOTE_VOICE\n");
		return IDVR_CIU_NO_RIGHT;
	}

	//设置编码模式
	if (eVoiceG711U == eMode)
	{
	    MPU_DBG("\n-----设置编码模式---eVoiceG711U-\n");
		if(0 != ZWSys_SetTalkingMode(eMode))
			return -1;
	}
	else if(eVoiceG726 == eMode)
	{
	    MPU_DBG("\n-----设置编码模式---eVoiceG726--\n");
		if(0 != ZWSys_SetTalkingMode(eMode))
			return -1;
	}
	else if(eVoiceG711A == eMode)
	{
    	MPU_DBG("\n-----设置编码模式---eVoiceG711A--\n");
		if(0 != ZWSys_SetTalkingMode(eMode))
			return -1;
	}
	else
	{
		MPU_DBG("\n-----no support Audio mode %d--\n",eMode);
		return -1;
	}

    //打开AEC
    ZWSys_SwitchAecMode(1);
	int iRet = ZWSys_SetTalkingEnable(1);

	MPU_DBG("=======VoiceStreamCreate==end!==== [%d]\n", iRet);
	return iRet;
}

int VoiceStreamDestroy(DWORD dwUserId, DWORD dwChannel)
{
	MPU_DBG("VoiceStreamDestroy \n");

    //关闭AEC    
	ZWSys_SetTalkingEnable(0);
    ZWSys_SwitchAecMode(0);
	return 0;
}

int VoiceStreamRead(DWORD dwUserId, DWORD dwChannel, const BYTE * pBuffer, DWORD dwBufLen)
{
	return ZWSys_SetTalkingData((char *) pBuffer, (unsigned int)dwBufLen);
}

static int TalkingReadCallback(unsigned long DSPNumber)
{
	char DSPVoiceBuffer[640 * 4];//640 * 4
	unsigned long DSPVoiceSize = 640 * 4;//640 * 4

	int iRet = 0;
	memset(DSPVoiceBuffer,0,640 * 4);
	iRet = ZWSys_ReadTalkingData(DSPVoiceBuffer, (unsigned int *)&DSPVoiceSize);
	if (0 == iRet)
	{
		RealStreamMedia tVoiceStreamMedia;
		tVoiceStreamMedia.eType = eMainSound;
		tVoiceStreamMedia.cBuffer.pBuffer = (BYTE *) DSPVoiceBuffer;
		tVoiceStreamMedia.cBuffer.dwBufLen = DSPVoiceSize;
		HieServer_VoiceStreamMedia(0, &tVoiceStreamMedia);
	}

	return 0;
}

void CIU_SetTalkCallBack(Talk_Callback pTalkCallBack)
{
    /* TODO */
}

#if defined (__IS_SUPPORT_LCD_MANAGER__)
int VideoStreamCreate(DWORD dwUserId, DWORD dwChannel, LPVideoStreamMedia VideoStreamMedia)
{
    if (VideoStreamMedia == NULL)
    {
    	MPU_ERR("VideoStreamCreate (usr:%lu ch:%lu)\n", dwUserId, dwChannel);
        return -1;
    }

    if (eVoiceH264 != VideoStreamMedia->eMode ||
        eVoiceG711U != VideoStreamMedia->eVoiceCodec ||
        eVideoCallSMediaTypeBegin >= VideoStreamMedia->eMediaType ||
        eVideoCallSMediaTypeEnd <= VideoStreamMedia->eMediaType)
    {
        MPU_ERR("VideoStreamCreate Unsupported format [%d-%d-%d]\n", VideoStreamMedia->eMode, VideoStreamMedia->eVoiceCodec, VideoStreamMedia->eMediaType);
        return -1;
    }

	MPU_INFO("VideoStreamCreate start (usr:%lu ch:%lu eMode:%d-%d Type:%d)\n", dwUserId, dwChannel, VideoStreamMedia->eMode, VideoStreamMedia->eVoiceCodec, VideoStreamMedia->eMediaType);

    pthread_mutex_lock(&g_mutexVideoCall);
    if (g_VideoCallCfg.nCallState)  /* 音频或视频通话中，返回busy */
    {
        pthread_mutex_unlock(&g_mutexVideoCall);
        MPU_ERR("=======VideoStreamCreate Failed! The device is busy!==== [%d]\n", g_VideoCallCfg.nCallState);
        return eErrorP2PMidDevIsBusy;
    }

    if (VideoStreamMedia->eMediaType == eVideoCallMediaTypeAudio)
    {
        MPU_LCD_SetCallStatus(2);   /* LCD显示语音通话中 */
        g_VideoCallCfg.nCallState |= STREAM_VOICE_MASK;
    }
    else if (VideoStreamMedia->eMediaType == eVideoCallMediaTypeVideo)
    {
        MPU_LCD_SetCallStatus(1);   /* LCD显示视频通话中 */
        g_VideoCallCfg.nCallState |= STREAM_VIDEO_MASK;
    }
    else if (VideoStreamMedia->eMediaType == eVideoCallMediaTypeVideoAndAudio)
    {
        MPU_LCD_SetCallStatus(1);   /* LCD显示视频通话中 */        
        g_VideoCallCfg.nCallState |= STREAM_VOICE_MASK;
        g_VideoCallCfg.nCallState |= STREAM_VIDEO_MASK;
    }

    g_VideoCallCfg.nUserId = dwUserId;
    g_VideoCallCfg.nVMode = VideoStreamMedia->eMode;
    g_VideoCallCfg.nAMode = VideoStreamMedia->eVoiceCodec;

#if (VIDEO_SAVE_FILE_TEST)
    /* 测试代码：保存到SD卡中 */
    if (g_VideoCallCfg.pFd == NULL) {
        char filepath[128] = {0};
        snprintf(filepath, sizeof(filepath), "/tmp/app/disk/mmcblk0p1/output/videocall_id%lu_ch%lu_mode%d_%llu.bin", dwUserId, dwChannel, VideoStreamMedia->eMode, time(NULL));
        g_VideoCallCfg.pFd = fopen(filepath, "wb+");
        MPU_INFO("[VideoStreamCreate] fopen pFd[%p] filepath[%s]\n", g_VideoCallCfg.pFd, filepath);
    }
#endif

    //立即唤醒(退出休眠模式)
    MPU_SYS_AwakeSchedule_DorEnable(1);

    /* 语音对讲准备 */
    ZWSys_SetTalkingMode(VideoStreamMedia->eVoiceCodec); /* 设置音频编码模式 */
    ZWSys_SwitchAecMode(1);                             /* 开启AEC */
	ZWSys_SetTalkingEnable(1);                          /* 开启对讲 */

    /* 视频解码准备 */
    ZWSys_LCDCreateVdecChn(VideoStreamMedia->eMode);    /* 开启Vdec解码通道 */
    pthread_mutex_unlock(&g_mutexVideoCall);

	MPU_INFO("VideoStreamCreate end (usr:%lu ch:%lu eMode:%d-%d Type:%d)\n", dwUserId, dwChannel, VideoStreamMedia->eMode, VideoStreamMedia->eVoiceCodec, VideoStreamMedia->eMediaType);
	return 0;
}

int VideoStreamDestroy(DWORD dwUserId, DWORD dwChannel)
{
	MPU_INFO("VideoStreamDestroy start (usr:%lu ch:%lu)\n", dwUserId, dwChannel);

    pthread_mutex_lock(&g_mutexVideoCall);

    if (g_VideoCallCfg.nUserId != dwUserId)
    {
        pthread_mutex_unlock(&g_mutexVideoCall);
        MPU_ERR("VideoStreamDestroy Failed! UserID is not match! [%d-%d]\n", dwUserId, g_VideoCallCfg.nUserId);
        return -1;
    }

    if (g_VideoCallCfg.nCallState == 0) /* 无通话，返回错误 */
    {
        pthread_mutex_unlock(&g_mutexVideoCall);
        MPU_ERR("VideoStreamDestroy Failed! This Call is closed!\n");
        return -1;
    }

    /* 视频 */
    ZWSys_LCDDestoryVdecChn(g_VideoCallCfg.nVMode); /* 销毁dec解码通道 */

    /* 语音 */
	ZWSys_SetTalkingEnable(0);  /* 关闭对讲 */
    ZWSys_SwitchAecMode(0);     /* 关闭AEC */

    MPU_LCD_SetCallStatus(4);   /* LCD显示挂断 */

#if (VIDEO_SAVE_FILE_TEST)
    /* 测试代码：保存到SD卡中 */
    if (g_VideoCallCfg.pFd != NULL)
    {    
        MPU_INFO("[VideoStreamDestroy] fclose pFd[%p]\n", g_VideoCallCfg.pFd);
        fclose(g_VideoCallCfg.pFd);
        g_VideoCallCfg.pFd = NULL;
    }
#endif
    memset(&g_VideoCallCfg, 0, sizeof(VIDEOCALL_CFG));
    pthread_mutex_unlock(&g_mutexVideoCall);

	MPU_INFO("VideoStreamDestroy end (usr:%lu ch:%lu)\n", dwUserId, dwChannel);
    return 0;
}

int VideoStreamRead(DWORD dwUserId, DWORD dwChannel, const BYTE * pBuffer, DWORD dwBufLen)
{
    if (pBuffer == NULL || dwBufLen == 0)
    {
        MPU_ERR("VideoStreamRead Failed! Parameter error[%p -- %lu]\n", pBuffer, dwBufLen);
        return -1;
    }
    const zwFrameHeader *pHeader = (const zwFrameHeader *)pBuffer;
    if (pHeader->DataPacketStartCode != 0xB7010000)
    {
        MPU_ERR("VideoStreamRead Failed! DataPacketStartCode Err!\n");
        return -1;
    }

    if (pHeader->nOverloadLen != (dwBufLen-sizeof(zwFrameHeader)))
    {
        MPU_ERR("VideoStreamRead Failed! nOverloadLen[%u] dwBufLen[%u]\n", pHeader->nOverloadLen, dwBufLen);
        return -1;
    }

    pthread_mutex_lock(&g_mutexVideoCall);

    if (g_VideoCallCfg.nUserId != dwUserId)
    {
        pthread_mutex_unlock(&g_mutexVideoCall);
        MPU_ERR("VideoStreamRead Failed! UserID is not match! [%d-%d]\n", dwUserId, g_VideoCallCfg.nUserId);
        return -1;
    }

    if (g_VideoCallCfg.nCallState == 0) /* 无通话，返回错误 */
    {
        pthread_mutex_unlock(&g_mutexVideoCall);
        MPU_ERR("VideoStreamRead Failed! The device is idle!\n");
        return -1;
    }

#if (VIDEO_SAVE_FILE_TEST)
    /* 测试代码：保存到SD卡中 */
    if (g_VideoCallCfg.pFd != NULL)
    {
    	unsigned long len = fwrite(pBuffer, 1, dwBufLen, g_VideoCallCfg.pFd);
    	if (len != dwBufLen) {
            MPU_INFO("[VideoStreamRead] fwrite failed len[%d] dwBufLen[%lu]\n", len, dwBufLen);
    	} else {
            MPU_INFO("[VideoStreamRead] fwrite success pFd[%p] dwBufLen[%lu]\n", g_VideoCallCfg.pFd, dwBufLen);
        }
    }
#endif
    pthread_mutex_unlock(&g_mutexVideoCall);

    if (pHeader->FrameType == eFrameAudioFrames)
    {
        if (g_VideoCallCfg.nCallState & STREAM_VOICE_MASK)
        {
            /* 喂流至对讲通道 */
            ZWSys_SetTalkingData((char *)pBuffer+sizeof(zwFrameHeader), dwBufLen-sizeof(zwFrameHeader));
        }
    }
    else
    {
        if (g_VideoCallCfg.nCallState & STREAM_VIDEO_MASK)
        {
            /* 喂流至DEC解码通道 */
            ZWSys_LCDSendStreamToChn(g_VideoCallCfg.nVMode, (char *)pBuffer+sizeof(zwFrameHeader), dwBufLen-sizeof(zwFrameHeader));
        }
    }

    return 0;
}

int VideoStreamControl(DWORD dwUserId, DWORD dwChannel, LPVideoCallMediaControl VideoCallMediaControl)
{
    if (VideoCallMediaControl == NULL)
    {
        MPU_ERR("VideoStreamRead Failed! Parameter error\n");
        return -1;
    }

    if (eVideoCallSMediaTypeBegin >= VideoCallMediaControl->eMediaType ||
        eVideoCallSMediaTypeEnd <= VideoCallMediaControl->eMediaType)
    {
        MPU_ERR("VideoStreamCreate Unsupported MediaType [%d]\n", VideoCallMediaControl->eMediaType);
        return -1;
    }

	MPU_INFO("VideoStreamControl start (usr:%lu ch:%lu Type:%d State:%d)\n", dwUserId, dwChannel, VideoCallMediaControl->eMediaType, g_VideoCallCfg.nCallState);

    BYTE nNewState = 0;
    if (VideoCallMediaControl->eMediaType == eVideoCallMediaTypeAudio)
    {
        nNewState |= STREAM_VOICE_MASK;
    }
    else if (VideoCallMediaControl->eMediaType == eVideoCallMediaTypeVideo)
    {
        nNewState |= STREAM_VIDEO_MASK;
    }
    else if (VideoCallMediaControl->eMediaType == eVideoCallMediaTypeVideoAndAudio)
    {
        nNewState |= STREAM_VOICE_MASK;
        nNewState |= STREAM_VIDEO_MASK;
    }

    pthread_mutex_lock(&g_mutexVideoCall);
    if (g_VideoCallCfg.nUserId != dwUserId)
    {
        pthread_mutex_unlock(&g_mutexVideoCall);
        MPU_ERR("VideoStreamControl Failed! UserID is not match! [%d-%d]\n", dwUserId, g_VideoCallCfg.nUserId);
        return -1;
    }

    if (g_VideoCallCfg.nCallState == 0) /* 无通话，返回错误 */
    {
        pthread_mutex_unlock(&g_mutexVideoCall);
        MPU_ERR("VideoStreamDestroy Failed! The device is idle!\n");
        return -1;
    }

    if (g_VideoCallCfg.nCallState == nNewState)
    {
        pthread_mutex_unlock(&g_mutexVideoCall);
        return 0;
    }

    if ((g_VideoCallCfg.nCallState & STREAM_VIDEO_MASK) && !(nNewState & STREAM_VIDEO_MASK))
    {    
        /* 如果当前视频开启中，控制需要关闭视频流 */
        MPU_LCD_SetCallStatus(2);   /* LCD显示语音通话中 */
    }
    else if (!(g_VideoCallCfg.nCallState & STREAM_VIDEO_MASK) && (nNewState & STREAM_VIDEO_MASK))
    {    
        /* 如果当前视频关闭中，控制需要打开视频流 */
        MPU_LCD_SetCallStatus(1);   /* LCD显示视频通话中 */
    }
    g_VideoCallCfg.nCallState = nNewState;

    pthread_mutex_unlock(&g_mutexVideoCall);

	MPU_INFO("VideoStreamControl end (usr:%lu ch:%lu Type:%d State:%d)\n", dwUserId, dwChannel, VideoCallMediaControl->eMediaType, g_VideoCallCfg.nCallState);
    return 0;
}

void GetExternPreviewCallState(int *state)
{
    *state = (int)(g_VideoCallCfg.nCallState);
}
#endif

//!\brief   初始化远程预览
void CIU_RegisterPreview()
{
	int nSDKRet;
	MPU_DBG("=======CIU_RegisterPreview======\n");

	// 设置实时流捕获回调接口
	StreamCaptureInterface tCaptureInterface;
	tCaptureInterface.ifStreamStartCapture = StreamStartCapture;
	tCaptureInterface.ifStreamStopCapture = StreamStopCapture;
	nSDKRet = HieServer_StreamCaptureIf(&tCaptureInterface);
	MPU_DBG("HieServer_StreamCaptureIf() return %d\n", nSDKRet);
	RealStreamAccessInterface tStreamAccessInterface;
	tStreamAccessInterface.ifStreamAccess = StreamAccessProc;
	nSDKRet = HieServer_StreamAccessIf(&tStreamAccessInterface);
	MPU_DBG("HieServer_StreamAccessIf() return %d\n", nSDKRet);

	// 设置语音流操作回调接口
	ZWSys_RegisterTalkingReadCallback(TalkingReadCallback);
	VoiceStreamInterface tVoiceInterface;	
	tVoiceInterface.ifVoiceStreamCreate = VoiceStreamCreate;
	tVoiceInterface.ifVoiceStreamDestroy = VoiceStreamDestroy;
	tVoiceInterface.ifVoiceStreamRead = VoiceStreamRead;
	nSDKRet = HieServer_VoiceStreamIf(&tVoiceInterface);
	MPU_DBG("HieServer_VoiceStreamIf() return %d\n", nSDKRet);

#if defined (__IS_SUPPORT_LCD_MANAGER__)
	// 设置视频流操作回调接口
    memset(&g_VideoCallCfg, 0, sizeof(VIDEOCALL_CFG));
	VideoStreamInterface tVideoInterface;
	tVideoInterface.ifVideoStreamCreate = VideoStreamCreate;
	tVideoInterface.ifVideoStreamDestroy = VideoStreamDestroy;
	tVideoInterface.ifVideoStreamRead = VideoStreamRead;
	tVideoInterface.ifVideoStreamControl = VideoStreamControl;
	nSDKRet = HieServer_VideoStreamIf(&tVideoInterface);
	MPU_DBG("HieServer_VideoStreamIf() return %d\n", nSDKRet);
#endif

#ifdef __USE_CIRCLEQUEUE__
	//初始化流缓冲队列
	CircleQueueInit();

#ifdef __SUPPORT_SD_CARD__
	//设置录像获取流数据回调接口
	RecordInterface tRecordInterface;
	tRecordInterface.ifRecordUserAdd = RecordCircleQueueAddUser;
	tRecordInterface.ifRecordUserDelete = RecordCircleQueueDeleteUser;
	tRecordInterface.ifRecordGetData = RecordCircleQueueGetData;
	tRecordInterface.ifRecordReleaseData = RecordCircleQueueReleaseData;
	nSDKRet = MemRecord_SetRecordInterface(&tRecordInterface);
	MPU_DBG("MemRecord_SetRecordInterface() return %d\n", nSDKRet);
#endif

	//设置网络拉流方式来获取流数据回调接口
	PullRealStreamInterface tRealStreamInterface;
	tRealStreamInterface.ifRealStreamConnectionAdd = NetCircleQueueAddUser;
	tRealStreamInterface.ifRealStreamConnectionDelete = NetCircleQueueDeleteUser;
	tRealStreamInterface.ifRealStreamConnectionGetData = NetCircleQueueGetData;
	tRealStreamInterface.ifRealStreamConnectionReleaseData = NetCircleQueueReleaseData;
	nSDKRet = HieServer_PullRealStreamIf(&tRealStreamInterface);
	MPU_DBG("HieServer_PullRealStreamIf() return %d\n", nSDKRet);

    CChannelManager *pMgr = CChannelManager::GetInstanceHandle();
    if (NULL != pMgr)
    {
        pMgr->SetAVCallBack(CircleQueueAvCallBack);
    }
#endif	
}

//!\brief   测试用
static int TalkingReadCallback_local(unsigned long DSPNumber)
{
	MPU_DBG("=======TalkingReadCallback_local======\n");
	char DSPVoiceBuffer[640 * 4];
	unsigned long DSPVoiceSize = 640 * 4;

	int iRet = ZWSys_ReadTalkingData(DSPVoiceBuffer, (unsigned int*)&DSPVoiceSize);
	if (0 == iRet)
	{
		ZWSys_SetTalkingData(DSPVoiceBuffer, (unsigned int)DSPVoiceSize);
	}
	else
	{
		MPU_ERR("ZWSys_ReadTalkingData	erro %d\n", iRet);
	}

	//MPU_DBG("DSPVoiceSize = %lu, iRet = %d, nRet = %d\n", DSPVoiceSize, iRet, nRet);
	return 0;
}

//!\brief    测试用 开启语音对讲
void CIU_StartLocalVoice(DWORD dwChannel)
{
	MPU_DBG("CIU_StartLocalVoice \n");
	ZWSys_RegisterTalkingReadCallback(TalkingReadCallback_local);
	ZWSys_SetTalkingEnable(1);
}

//!\brief    测试用 停止语音对讲
void CIU_StopLocalVoice(DWORD dwChannel)
{
	MPU_DBG("CIU_StopLocalVoice \n");
	ZWSys_SetTalkingEnable(0);
	ZWSys_RegisterTalkingReadCallback(TalkingReadCallback);
}
