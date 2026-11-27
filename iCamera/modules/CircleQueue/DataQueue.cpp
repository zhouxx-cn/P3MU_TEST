#include <stdio.h>
#include <string.h>
#include <iostream>
#include <algorithm>
#include <sys/time.h>
#include "CircleQueue.h"
#include "DataQueue.h"
#include "User.h"
#include "sys_banned.h"

// 多级别日志控制
#ifndef CQ_LOG_LEVEL
#define CQ_LOG_LEVEL 1  // 0-关闭，1-错误，2-信息，3-调试
#endif

#if CQ_LOG_LEVEL >= 3
#define CQ_DEBUG(fmt, ...) do { \
    printf("[CQ_DEBUG][%s:%d] " fmt, __FILE__, __LINE__, ##__VA_ARGS__); \
} while(0)
#else
#define CQ_DEBUG(fmt, ...)
#endif

#if CQ_LOG_LEVEL >= 2
#define CQ_INFO(fmt, ...) do { \
    printf("[CQ_INFO][%s:%d] " fmt, __FILE__, __LINE__, ##__VA_ARGS__); \
} while(0)
#else
#define CQ_INFO(fmt, ...)
#endif

#if CQ_LOG_LEVEL >= 1
#define CQ_ERR(fmt, ...) do { \
    printf("[CQ_ERR][%s:%d] " fmt, __FILE__, __LINE__, ##__VA_ARGS__); \
} while(0)
#else
#define CQ_ERR(fmt, ...)
#endif

namespace xrs_circle_queue {

//!\brief 判断帧类型是否为I帧（视频关键帧）
static bool IsIFrameType(int32_t s32FrameType)
{
    switch (s32FrameType)
    {
    case PktIFrames:
    case PktQCIFIFrames:
    case PktSQCIFIFrames:
    case PktFourthIFrames:
    case PktCaptureIFrames:
    case PktIFramesForS1:
    case PktQCIFIFramesForS1:
    case PktFourthIFramesForS1:
    case PktIFramesForS2:
    case PktQCIFIFramesForS2:
    case PktFourthIFramesForS2:
    case PktIFramesForS3:
    case PktQCIFIFramesForS3:
    case PktFourthIFramesForS3:
        return true;
    default:
        return false;
    }
}

DataNode::DataNode(DataQueue *ptrQueue, CQ_DATA_S &stData, bool bCopy):
    s32Channel(stData.s32Channel), s32Stream(stData.s32Stream), s32FrameType(stData.s32FrameType),
    pData(stData.pData),s32DataSize(stData.s32DataSize),
    s64Timestamp(stData.s64Timestamp),
    pQueue(ptrQueue),
    s32Seq(stData.s32Seq),
    s32RefCount(0)
{
    tpSteady = std::chrono::steady_clock::now();
    if (bCopy)
    {
        pData = (uint8_t *)malloc(stData.s32DataSize);
        memcpy(pData, stData.pData, stData.s32DataSize);
    }
}

DataNode &DataNode::operator=(const DataNode &refNode)
{
    s32Channel = refNode.s32Channel;
    s32Stream = refNode.s32Stream;
    s32FrameType = refNode.s32FrameType;
    pData = refNode.pData;
    s32DataSize = refNode.s32DataSize;
    s64Timestamp = refNode.s64Timestamp;
    tpSteady = refNode.tpSteady;
    pQueue = refNode.pQueue;
    s32Seq = refNode.s32Seq;
    s32RefCount = refNode.s32RefCount;
    return *this;
}

DataNode::operator CQ_DATA_S() const
{
    CQ_DATA_S stData;
    stData.s32Channel = s32Channel;
    stData.s32Stream = s32Stream;
    stData.s32FrameType = s32FrameType;
    stData.pData = pData;
    stData.s32DataSize = s32DataSize;
    stData.s64Timestamp = s64Timestamp;
    stData.hQueue = reinterpret_cast<CQ_HANDLE>(pQueue);
    stData.s32Seq = s32Seq;
    stData.s32RefCount = s32RefCount;
    return stData;
}

DataQueue::DataQueue(int s32Size, int s32Duration, bool bCopyMode):
    m_s32TotalSpace(s32Size), m_s32MaxDuration(s32Duration), 
    m_bCopyMode(bCopyMode)
{
    m_listQueue.clear();
    m_listExpired.clear();
    m_objStats.m_s32SeqHead = 0;
    m_objStats.m_s32SeqTail = 0;
    m_objStats.m_s32TotalAddNodes = 0;
    m_objStats.m_s32TotalNodeSize = 0;
    m_objStats.m_s32UsedSpace = 0;
	for (int i = 0; i < STREAM_MAX; i++)
	{
		m_objStats.m_s32FrameCount[i] = 0;
	}
    m_objStats.m_timeHead = std::chrono::_V2::steady_clock::now();
    m_objStats.m_timeTail = m_objStats.m_timeHead;
	time_prev = 0;
}

DataQueue::~DataQueue()
{
    // 释放队列中残留的节点内存
    for (auto it = m_listQueue.begin(); it != m_listQueue.end(); it++)
    {
        it->FreeMem();
    }
    m_listQueue.clear();
    for (auto it = m_listExpired.begin(); it != m_listExpired.end(); it++)
    {
        it->FreeMem();
    }
    m_listExpired.clear();
}

int32_t DataQueue::AddNode(CQ_DATA_S &stData)
{
    if (RequestSpace(stData.s32DataSize))
    {
        CQ_DEBUG("Queue %p request space %d failed\n", this, stData.s32DataSize);
        return XRS_CQ_ERR_OOM;
    }

    stData.s32Seq = (m_objStats.m_s32SeqTail+1);
    stData.s32RefCount = 0;
    stData.hQueue = reinterpret_cast<CQ_HANDLE>(this);
    CQ_DEBUG("Queue %p AddData:pData=%p, size=%d, seq=%d, timestamp=%lld\n",
        this,
        stData.pData, stData.s32DataSize, 
        stData.s32Seq, stData.s64Timestamp);

    DataNode nodeNew(this, stData, this->m_bCopyMode);
    if (m_listQueue.empty())
    {
        m_objStats.m_s32SeqHead = stData.s32Seq;
        m_objStats.m_timeHead = nodeNew.Time();
    }
    m_listQueue.push_back(nodeNew);
    m_objStats.m_s32SeqTail = stData.s32Seq;
    m_objStats.m_timeTail = nodeNew.Time();
    m_objStats.m_s32TotalAddNodes++;
    m_objStats.m_s32TotalNodeSize += stData.s32DataSize;
    m_objStats.m_s32UsedSpace += stData.s32DataSize;
	UpdateFrameCount(stData.s32FrameType, 1);

    CQ_DEBUG("m_objStats:\n");
	#if 0
	static unsigned long long u32Count = 0;
	for (int i = 0; i < STREAM_MAX; i++)
	{
		if (0 == u32Count%30)
		{	
			printf("stream[%d] count[%d]\n", i, m_objStats.m_s32FrameCount[i]);
		}
	}
	u32Count++;
	#else
	for (int i = 0; i < STREAM_MAX; i++)
	{
		CQ_DEBUG("stream[%d] count[%d]\n", i, m_objStats.m_s32FrameCount[i]);
	}
	#endif
    CQ_DEBUG("Head={%d},Tai={%d},duration=%lld\n", m_objStats.m_s32SeqHead, m_objStats.m_s32SeqTail,
        std::chrono::duration_cast<std::chrono::milliseconds>(m_objStats.m_timeTail-m_objStats.m_timeHead).count());
    CQ_DEBUG("TotalAddNodes=%d, TotalNodeSize=%d, UsedSpace=%d\n",
        m_objStats.m_s32TotalAddNodes, m_objStats.m_s32TotalNodeSize, m_objStats.m_s32UsedSpace);

#if 0
	struct timeval now;
	
	gettimeofday(&now,NULL);
	if ((0 == time_prev) || (time_prev > now.tv_sec))
	{
		time_prev = now.tv_sec;
	}
	else
	{
		if (now.tv_sec - time_prev > 1)
		{
			CQ_DEBUG("m_s32MaxDuration = %d   TotalAddNodes=%d, TotalNodeSize=%d, UsedSpace=%d\n",m_s32MaxDuration,
        		m_objStats.m_s32TotalAddNodes, m_objStats.m_s32TotalNodeSize, m_objStats.m_s32UsedSpace);

        	time_prev = now.tv_sec;
		}
	}
#endif	

    return XRS_CQ_OK;
}

int32_t DataQueue::GetNextNode(User *pUser, CQ_DATA_S &refData)
{
    if (m_listQueue.empty())
    {
        CQ_DEBUG("Queue %p User %p read failed,  Queue is empty\n", this, pUser);
        return XRS_CQ_ERR_EMPTY;
    }
    DataQueue::NodeList::iterator itNode;
    bool bFirstRead = (pUser->Seq() < 0);
    // 强制拉取最新节点（MJPEG场景）
    if (pUser->GetNewNodeFlag() == 1)
    {
        itNode = m_listQueue.end();
        itNode--;
    }
    // 第一次获取节点，按消费者指定的起始策略定位起始节点（录像/预录制）
    else if (bFirstRead && (CQ_START_NEWEST != pUser->GetStartPolicy()))
    {
        if (!FindStartNode(pUser, itNode))
        {
            CQ_ERR("Queue %p User %p find start idr failed, policy=%d\n",
                this, pUser, pUser->GetStartPolicy());
            return XRS_CQ_ERR_NO_IDR;
        }
    }
    // 第一次获取节点,则直接读最后一个节点（最新的数据）
    else if (bFirstRead)
    {
        itNode = m_listQueue.end();
        itNode--;
    }
    // 头节点的序号比用户最后一次读的节点大，直接读头节点
    else if (m_listQueue.begin()->Seq() > pUser->Seq())
    {
        itNode = m_listQueue.begin();
    }
    // 正常往后读取下一个节点
    else
    {
        itNode = pUser->Node();
        itNode++;
		DataQueue::NodeList::iterator itNextNode = itNode;
        // 没有下一个节点了
        if (itNode == m_listQueue.end())
        {
            CQ_DEBUG("Queue %p User %p read reach tail\n", this, pUser);
            return XRS_CQ_ERR_TAIL;
        }
		itNextNode++;
        // 倒数第二个为空也认为读完了，避免死机问题
        if (itNextNode == m_listQueue.end())
        {
            CQ_DEBUG("Queue %p User %p read reach tail\n", this, pUser);
            return XRS_CQ_ERR_TAIL;
        }
    }
    itNode->AddRef();
    pUser->GetNode(itNode->Seq(), itNode);
    refData = *itNode;
    CQ_DEBUG("Queue %p User %p ReadData: pData=%p, size=%d, seq=%d, timestamp=%lld\n",
        this, pUser,
        refData.pData, refData.s32DataSize, 
        refData.s32Seq, refData.s64Timestamp);

    return XRS_CQ_OK;
}

int32_t DataQueue::GetNode(User *pUser, CQ_DATA_S &refData)
{
    int32_t s32Ret = 0;
    while (0 == (s32Ret = GetNextNode(pUser, refData)))
    {
        if (MatchUser(pUser, refData.s32Channel, refData.s32Stream))
        {
            return XRS_CQ_OK;
        }
        ReleaseNode(pUser);
    }
    return s32Ret;
}

//!\brief 判断通道/码流是否匹配消费者的过滤条件
bool DataQueue::MatchUser(User *pUser, int32_t s32Channel, int32_t s32Stream)
{
    bool bCheckChannel = (0xff != pUser->Param().s32Channel);
    bool bCheckStream = (0xff != pUser->Param().s32Stream);
    if (bCheckChannel && (s32Channel != pUser->Param().s32Channel))
    {
        return false;
    }
    if (bCheckStream && (s32Stream != pUser->Param().s32Stream))
    {
        return false;
    }
    return true;
}

//!\brief 按消费者的起始策略定位起始节点
bool DataQueue::FindStartNode(User *pUser, DataQueue::NodeList::iterator &itNode)
{
    if (m_listQueue.empty())
    {
        return false;
    }

    if (CQ_START_IDR_BEFORE_OFFSET == pUser->GetStartPolicy())
    {
        // 预录制：取[当前时刻 - 预录时长]之前的最近一个I帧
        std::chrono::steady_clock::time_point tpTarget = std::chrono::steady_clock::now() -
            std::chrono::milliseconds(pUser->GetStartOffsetMs());
        DataQueue::NodeList::iterator itOldestIdr = m_listQueue.end();
        DataQueue::NodeList::iterator itHitIdr = m_listQueue.end();
        for (auto it = m_listQueue.begin(); it != m_listQueue.end(); ++it)
        {
            if (!IsIFrameType(it->GetFrameType()) || !MatchUser(pUser, it->GetChannel(), it->GetStream()))
            {
                continue;
            }
            if (itOldestIdr == m_listQueue.end())
            {
                itOldestIdr = it;
            }
            if (it->Time() <= tpTarget)
            {
                itHitIdr = it;
            }
        }
        if (itHitIdr != m_listQueue.end())
        {
            itNode = itHitIdr;
            return true;
        }
        // 预录时长超出缓冲范围，回退到缓冲区内最老的I帧（尽力而为）
        if (itOldestIdr != m_listQueue.end())
        {
            CQ_INFO("Queue %p User %p start offset %d ms exceeds buffer range, fallback to oldest idr\n",
                this, pUser, pUser->GetStartOffsetMs());
            itNode = itOldestIdr;
            return true;
        }
        return false;
    }

    // CQ_START_LATEST_IDR：取缓冲区内最近的一个I帧
    DataQueue::NodeList::iterator itLatestIdr = m_listQueue.end();
    for (auto it = m_listQueue.begin(); it != m_listQueue.end(); ++it)
    {
        if (IsIFrameType(it->GetFrameType()) && MatchUser(pUser, it->GetChannel(), it->GetStream()))
        {
            itLatestIdr = it;
        }
    }
    if (itLatestIdr == m_listQueue.end())
    {
        return false;
    }
    itNode = itLatestIdr;
    return true;
}

int32_t DataQueue::ReleaseNode(User *pUser)
{
	//避免未申请到数据就释放的情况
	if (!pUser->IsBusy())
	{
		//CQ_DEBUG("Queue %p User %p is idle, bad condition!!!!\n", this, pUser);
    	return XRS_CQ_OK;
	}
    NodeList::iterator itFind = FindNode(pUser->Seq(), pUser->Node());
    if (itFind == m_listQueue.end())
    {
        CQ_DEBUG("Queue %p User %p find node %d failed, impossible!!\n", this, pUser, pUser->Seq());
        return XRS_CQ_ERR_INVALID_PARAM;
    }
    // 引用计数递减
    itFind->Release(); 
    pUser->ReleaseNode();

    if (IsExpired(pUser->Seq()))
    {
        // 如果是已经超时的节点，并且无人占用，释放内存，并从超时队列中移除，更新总消耗内存
        if (!itFind->IsBusy())
        {
            CQ_DEBUG("Queue %p User %p free expired node %d size %d !\n", this, pUser, itFind->Seq(), itFind->Size());
            int32_t s32FrameType = itFind->GetFrameType();
            itFind->FreeMem();
            m_objStats.m_s32UsedSpace -= itFind->Size();
            m_listExpired.erase(itFind);
			m_objStats.m_s32TotalAddNodes--;
			UpdateFrameCount(s32FrameType, -1);
        }
    }
    CQ_DEBUG("Queue %p User %p release node %d\n", this, pUser, pUser->Seq());
    // 正常队列中的节点只减引用计数，不释放内存
    return XRS_CQ_OK;
}

bool DataQueue::IsBusy()
{
    // 正常队列中有一个节点被占用，则整个队列繁忙
    // 超时队列不为空，则队列繁忙
    for (auto it = m_listQueue.begin(); it != m_listQueue.end(); it++)
    {
        if (it->IsBusy())
        {
            return true;
        }
    }
    return !m_listExpired.empty();
}


int DataQueue::RequestSpace(int32_t s32Size)
{
    // 如果内存不能得到满足，则一直回收头节点（头节点回收后，下个节点就变成了头节点）
    while ((m_s32TotalSpace-m_objStats.m_s32UsedSpace) < s32Size)
    {
        // 队列为空了还不能满足(不太可能出现)，则应该是出错了
        if (m_listQueue.empty())
        {
            CQ_ERR("Queue %p is empty, but request space %d failed, TotalSpace=%d, UsedSpace=%d !!\n", 
                this, s32Size, m_s32TotalSpace, m_objStats.m_s32UsedSpace);
            // 无法回收到所需要的内存
            return XRS_CQ_ERR_OOM;
        }
        auto itNode = m_listQueue.begin();
        // 头节点被占用，放入超时队列
        if (itNode->IsBusy())
        {
            m_listExpired.push_back(*itNode);
        }
        // 未被占用，则直接释放内存
        else
        {
            itNode->FreeMem();
            CQ_DEBUG("Queue %p free node %d size %d !!\n", this, itNode->Seq(), itNode->Size());
            m_objStats.m_s32UsedSpace -= itNode->Size();
			m_objStats.m_s32TotalAddNodes--;
			UpdateFrameCount(itNode->GetFrameType(), -1);
        }
        // 头节点从队列中删除
        m_listQueue.erase(itNode);
    }
    if (!m_listQueue.empty())
    {
        auto itNode = m_listQueue.begin();
        m_objStats.m_s32SeqHead = itNode->Seq();
        m_objStats.m_timeHead = itNode->Time();
    }
    return XRS_CQ_OK;
}

DataQueue::NodeList::iterator DataQueue::FindNode(int32_t s32Seq, const NodeList::iterator& itNode)
{
    // internal method, do not check parameters
    // search expire list first, because it is very short
    for (auto it = m_listExpired.begin(); it != m_listExpired.end(); it++)
    {
        if (it->Seq() == s32Seq)
        {
            return it;
        }
    }

    return itNode;
}

bool DataQueue::IsExpired(int32_t s32Seq)
{
    // internal method, do not check parameters
    if (m_listQueue.empty())
    {
        return true;
    }
    if (m_listQueue.begin()->Seq() > s32Seq)
    {
        return true;
    }

    return false;
}

User * DataQueue::AddUser()
{
    if (m_vecUser.size() > CQ_MAX_USER)
    {
        // 达到最大的消费者数
        return nullptr;
    }
    User *pUser = new User(this);
    if (nullptr == pUser)
    {
        // oom
        return nullptr;
    }
    m_vecUser.push_back(pUser);
    CQ_INFO("Queue %p add user %p !!\n", this, pUser);
    return pUser;
}

int32_t DataQueue::DeleteUser(User *pUser)
{
    if (nullptr == pUser)
    {
        return XRS_CQ_ERR_INVALID_USER;
    }
    for (auto it = m_vecUser.begin(); it != m_vecUser.end(); ) 
    {
        if (*it == pUser)
        {
            it = m_vecUser.erase(it);
            CQ_INFO("Queue %p delete user %p !!\n", this, pUser);
            delete pUser;
            return XRS_CQ_OK;
        } else {
            ++it;
        }
    }

    return XRS_CQ_ERR_INVALID_USER;
}

int32_t DataQueue::DeleteUser(void)
{
    if (m_vecUser.size() >= CQ_MAX_USER)
    {
        // 达到最大的消费者数
        return XRS_CQ_ERR_TOOMUCH_USER;
    }
    for (auto it = m_vecUser.begin(); it != m_vecUser.end(); ) 
    {
        delete (*it);
        it = m_vecUser.erase(it);
    }

    return XRS_CQ_OK;
}

//!\brief 更新各码流的帧计数统计
void DataQueue::UpdateFrameCount(int32_t s32FrameType, int32_t s32Delta)
{
    if (PktIFrames == s32FrameType || PktPFrames == s32FrameType)
    {
        m_objStats.m_s32FrameCount[MAIN_STREAM] += s32Delta;
    }
    else if (PktQCIFIFrames == s32FrameType || PktQCIFPFrames == s32FrameType)
    {
        m_objStats.m_s32FrameCount[SUB_STREAM] += s32Delta;
    }
    else if (PktSQCIFIFrames == s32FrameType || PktSQCIFPFrames == s32FrameType)
    {
        m_objStats.m_s32FrameCount[THIRD_STREAM] += s32Delta;
    }
    else if (PktFourthIFrames == s32FrameType || PktFourthPFrames == s32FrameType)
    {
        m_objStats.m_s32FrameCount[CAPTURE_STREAM] += s32Delta;
    }
    else if (PktAudioFrames == s32FrameType)
    {
        m_objStats.m_s32FrameCount[AUDIO_STREAM] += s32Delta;
    }
}

//!\brief 清空缓冲区
//  未被占用的节点移入 listFree，由调用者在锁外释放内存；仍被占用的节点移入超时队列，
//  待消费者释放后再回收内存。
//  节点序号不重置，保证老消费者在缓冲区重新有数据后能无缝切换到新数据。
int32_t DataQueue::Clear(NodeList &listFree)
{
    while (!m_listQueue.empty())
    {
        auto itNode = m_listQueue.begin();
        if (itNode->IsBusy())
        {
            // 仍被消费者占用，移入超时队列，等待消费者释放后再回收
            m_listExpired.splice(m_listExpired.end(), m_listQueue, itNode);
        }
        else
        {
            // 无人占用，移出待释放列表，由调用者在锁外释放内存
            m_objStats.m_s32UsedSpace -= itNode->Size();
            m_objStats.m_s32TotalAddNodes--;
            UpdateFrameCount(itNode->GetFrameType(), -1);
            listFree.splice(listFree.end(), m_listQueue, itNode);
        }
    }

    // 序号不重置，仅同步统计信息
    m_objStats.m_s32SeqHead = m_objStats.m_s32SeqTail + 1;
    m_objStats.m_timeHead = std::chrono::steady_clock::now();
    m_objStats.m_timeTail = m_objStats.m_timeHead;

    // 释放消费者当前占用的节点，并修正其读取进度
    // 已读序号不重置，缓冲区重新有数据后，老消费者会自动切换到新数据流
    for (auto it = m_vecUser.begin(); it != m_vecUser.end(); it++)
    {
        User *pUser = *it;
        if (pUser->IsBusy())
        {
            ReleaseNode(pUser);
        }
        // 节点已移出队列，原进度迭代器已失效，指向队尾
        pUser->SetProgress(m_listQueue.end());
    }

    CQ_INFO("Queue %p clear, expired node count %d\n", this, (int32_t)m_listExpired.size());
    return XRS_CQ_OK;
}

//!\brief 强制释放消费者当前占用的节点，并重置其读取进度
int32_t DataQueue::FlushUser(User *pUser)
{
    if (nullptr == pUser)
    {
        return XRS_CQ_ERR_INVALID_USER;
    }
    if (pUser->IsBusy())
    {
        ReleaseNode(pUser);
    }
    // 重置进度，下次取流按起始策略重新定位
    pUser->Reset();
    return XRS_CQ_OK;
}

//!\brief 查询缓冲区状态
int32_t DataQueue::GetStatus(CQ_STATUS_S &refStatus)
{
    refStatus.s32UsedSpace = m_objStats.m_s32UsedSpace;
    refStatus.s32TotalSpace = m_s32TotalSpace;
    refStatus.s32NodeCount = (int32_t)m_listQueue.size();
    refStatus.s32BufferedMs = 0;
    refStatus.s32HasIdr = 0;

    if (!m_listQueue.empty())
    {
        auto itHead = m_listQueue.begin();
        auto itTail = m_listQueue.end();
        itTail--;
        refStatus.s32BufferedMs = (int32_t)std::chrono::duration_cast<std::chrono::milliseconds>(
            itTail->Time() - itHead->Time()).count();
        for (auto it = m_listQueue.begin(); it != m_listQueue.end(); it++)
        {
            if (IsIFrameType(it->GetFrameType()))
            {
                refStatus.s32HasIdr = 1;
                break;
            }
        }
    }
    return XRS_CQ_OK;
}


}
