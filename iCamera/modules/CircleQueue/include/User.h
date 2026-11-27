#ifndef __XRS_CIRCLE_QUEUE_USER_H_
#define __XRS_CIRCLE_QUEUE_USER_H_
#include <vector>
#include "CircleQueue.h"
#include "DataQueue.h"

namespace xrs_circle_queue {

class User
{
private:
    DataQueue *m_queueData;
    int32_t m_s32SeqGot;
    int32_t m_s32SeqLast;
	int32_t m_NewNodeFlag;
    DataQueue::NodeList::iterator m_itProgress;
    CQ_USER_PARAM_S m_strParam;
    // 首次取流的起始策略
    CQ_START_POLICY_E m_enStartPolicy;
    // 预录制回溯时长（毫秒）
    int32_t m_s32StartOffsetMs;
public:
    User(DataQueue *pQueue): 
        m_queueData(pQueue),
        m_s32SeqGot(-1),
        m_s32SeqLast(-1),
        m_NewNodeFlag(0),
        m_enStartPolicy(CQ_START_NEWEST),
        m_s32StartOffsetMs(0)
    {
        m_strParam.s32Channel = 0xff;
        m_strParam.s32Stream = 0xff;
    };
    User(User &refUser):
        m_queueData(refUser.m_queueData),m_s32SeqGot(refUser.m_s32SeqGot)
    {
        m_strParam = refUser.m_strParam;
    };
    CQ_USER_PARAM_S & Param() 
    {
        return m_strParam;
    };
    ~User() {};
    bool IsBusy() { return (m_s32SeqGot != -1); };
    int32_t GetNode(int32_t s32NodeSeq, const DataQueue::NodeList::iterator& itNode);
    int32_t ReleaseNode();
    DataQueue::NodeList::iterator Node() { return m_itProgress; };
    int32_t Seq() { return m_s32SeqLast; };
    DataQueue *Queue() { return m_queueData; };
	
	int32_t SetNewNodeFlag(int32_t NewNodeFlag) { m_NewNodeFlag = NewNodeFlag; return m_NewNodeFlag;};
	int32_t GetNewNodeFlag() { return m_NewNodeFlag; };

    void SetStartPolicy(CQ_START_POLICY_E enPolicy, int32_t s32OffsetMs)
    {
        m_enStartPolicy = enPolicy;
        m_s32StartOffsetMs = s32OffsetMs;
    };
    CQ_START_POLICY_E GetStartPolicy() { return m_enStartPolicy; };
    int32_t GetStartOffsetMs() { return m_s32StartOffsetMs; };

    // 修正进度迭代器（节点被移动到超时队列后使用）
    void SetProgress(const DataQueue::NodeList::iterator& itNode) { m_itProgress = itNode; };

    // 重置读取进度，下次取流按起始策略重新定位
    void Reset()
    {
        m_s32SeqGot = -1;
        m_s32SeqLast = -1;
        m_itProgress = m_queueData->EndNode();
    };
};
}
#endif

