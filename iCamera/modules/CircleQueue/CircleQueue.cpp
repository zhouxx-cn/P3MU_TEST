#include <iostream>
#include "CircleQueue.h"
#include "DataQueue.h"
#include "User.h"
#include "sys_banned.h"
using namespace xrs_circle_queue;

CQ_HANDLE CircleQueue_Create (CQ_PARAM_S *pstParam)
{
    DataQueue *pQueue = new DataQueue(pstParam->s32MaxSize, pstParam->s32MaxDuration, (pstParam->enMode == CQ_MODE_COPY));
    if (nullptr == pQueue)
    {
        return CQ_INVALID_HANDLE;
    }

    return (CQ_HANDLE)pQueue;
}

int CircleQueue_Destroy(CQ_HANDLE hCircleQueue)
{
    if (hCircleQueue == CQ_INVALID_HANDLE)
    {
        return XRS_CQ_ERR_INVALID_HANDLE;
    }
    DataQueue *pQueue = reinterpret_cast<DataQueue *>(hCircleQueue);
    if (pQueue->IsBusy())
    {
        return XRS_CQ_ERR_BUSY;
    }
    pQueue->DeleteUser();
    delete pQueue;
    return XRS_CQ_OK; 
}

CQ_USER CircleQueue_AddUser(CQ_HANDLE hCircleQueue, CQ_USER_PARAM_S *pstUserParam)
{
    if (hCircleQueue == CQ_INVALID_HANDLE)
    {
        return XRS_CQ_ERR_INVALID_HANDLE;
    }
    if (nullptr == pstUserParam)
    {
        return 0;
    }

    DataQueue *pQueue = reinterpret_cast<DataQueue *>(hCircleQueue);
    User *pUser =  pQueue->AddUser();
    if(pUser == nullptr)
    {
        return 0;
    }
    pUser->Param() = *pstUserParam;
	//判断当新添加用户句柄 流类型为MJPEG时，修改标志，后续每次通过此句柄获取流数据时，强制获取最新的流数据
	if(pstUserParam->s32Stream == 3)
	{
		pUser->SetNewNodeFlag(1);
	}
	else
	{
		pUser->SetNewNodeFlag(0);
	}

    return reinterpret_cast<CQ_USER>(pUser);
}

//!\brief添加消费者（扩展接口，支持指定首次取流的起始策略）
CQ_USER CircleQueue_AddUserEx(CQ_HANDLE hCircleQueue, CQ_USER_PARAM_EX_S *pstUserParam)
{
    if (hCircleQueue == CQ_INVALID_HANDLE)
    {
        return XRS_CQ_ERR_INVALID_HANDLE;
    }
    if (nullptr == pstUserParam)
    {
        return 0;
    }

    DataQueue *pQueue = reinterpret_cast<DataQueue *>(hCircleQueue);
    User *pUser = pQueue->AddUser();
    if (nullptr == pUser)
    {
        return 0;
    }
    pUser->Param() = pstUserParam->stBase;
    pUser->SetStartPolicy(pstUserParam->enStartPolicy, pstUserParam->s32StartOffsetMs);
    // MJPEG流默认强制拉取最新节点；但指定了I帧起始策略时（录像场景）需按顺序取流，故不置该标志
    pUser->SetNewNodeFlag((pstUserParam->stBase.s32Stream == 3 &&
        CQ_START_NEWEST == pstUserParam->enStartPolicy) ? 1 : 0);

    return reinterpret_cast<CQ_USER>(pUser);
}

//!\brief删除消费者
int CircleQueue_DeleteUser(CQ_USER hQueueUser)
{
    if (hQueueUser == CQ_INVALID_USER || hQueueUser == XRS_CQ_ERR_TOOMUCH_USER)
    {
        return XRS_CQ_ERR_INVALID_HANDLE;
    }
    
    User *pUser = reinterpret_cast<User *>(hQueueUser);
    DataQueue *pQueue = pUser->Queue();
    const std::lock_guard<DataQueue> lock(*pQueue);
    if (pUser->IsBusy())
    {
        return XRS_CQ_ERR_BUSY;
    }
    return pQueue->DeleteUser(pUser);
}

//!\brief写入数据到缓冲队列
int CircleQueue_AddData(CQ_HANDLE hQueue, CQ_DATA_S *pstData)
{
    if (hQueue == CQ_INVALID_HANDLE)
    {
        return XRS_CQ_ERR_INVALID_HANDLE;
    }

    DataQueue *pQueue = reinterpret_cast<DataQueue *>(hQueue);
    const std::lock_guard<DataQueue> lock(*pQueue);
    return pQueue->AddNode(*pstData);
}

//!\brief从缓冲队列读取一个节点的数据信息，并将该节点的引用计数加1
int CircleQueue_GetData(CQ_USER hQueueUser, CQ_DATA_S *pstData)
{
    int32_t s32Result = 0;
    if (hQueueUser == CQ_INVALID_USER || hQueueUser == XRS_CQ_ERR_TOOMUCH_USER)
    {
        return XRS_CQ_ERR_INVALID_HANDLE;
    }
    
    User *pUser = reinterpret_cast<User *>(hQueueUser);
    DataQueue *pQueue = pUser->Queue();
    const std::lock_guard<DataQueue> lock(*pQueue);
    s32Result = pQueue->GetNode(pUser, *pstData);
    return s32Result;
}

//!\brief将队列节点的引用计数减1，当节点的引用计数为0时，无消费者正在使用此节点
int CircleQueue_ReleaseData(CQ_USER hQueueUser, CQ_DATA_S *pstData)
{
    if (hQueueUser == CQ_INVALID_USER || hQueueUser == XRS_CQ_ERR_TOOMUCH_USER)
    {
        return XRS_CQ_ERR_INVALID_USER;
    }

    User *pUser = reinterpret_cast<User *>(hQueueUser);
    DataQueue *pQueue = pUser->Queue();
    const std::lock_guard<DataQueue> lock(*pQueue);
    pQueue->ReleaseNode(pUser);
    return XRS_CQ_OK;
}

//!\brief清空缓冲区（未占用的节点在锁外释放内存，不阻塞调用方）
int32_t CircleQueue_Clear(CQ_HANDLE hCircleQueue)
{
    if (hCircleQueue == CQ_INVALID_HANDLE)
    {
        return XRS_CQ_ERR_INVALID_HANDLE;
    }

    DataQueue *pQueue = reinterpret_cast<DataQueue *>(hCircleQueue);
    DataQueue::NodeList listFree;
    {
        const std::lock_guard<DataQueue> lock(*pQueue);
        pQueue->Clear(listFree);
    }
    // 锁外释放内存，避免长时间占用锁影响数据写入
    for (auto it = listFree.begin(); it != listFree.end(); it++)
    {
        it->FreeMem();
    }
    listFree.clear();
    return XRS_CQ_OK;
}

//!\brief重置消费者读取进度，并强制释放该消费者当前占用的节点
int32_t CircleQueue_FlushUser(CQ_USER hQueueUser)
{
    if (hQueueUser == CQ_INVALID_USER || hQueueUser == XRS_CQ_ERR_TOOMUCH_USER)
    {
        return XRS_CQ_ERR_INVALID_USER;
    }

    User *pUser = reinterpret_cast<User *>(hQueueUser);
    DataQueue *pQueue = pUser->Queue();
    const std::lock_guard<DataQueue> lock(*pQueue);
    return pQueue->FlushUser(pUser);
}

//!\brief查询缓冲区状态
int32_t CircleQueue_GetStatus(CQ_HANDLE hCircleQueue, CQ_STATUS_S *pstStatus)
{
    if (hCircleQueue == CQ_INVALID_HANDLE)
    {
        return XRS_CQ_ERR_INVALID_HANDLE;
    }
    if (nullptr == pstStatus)
    {
        return XRS_CQ_ERR_INVALID_PARAM;
    }

    DataQueue *pQueue = reinterpret_cast<DataQueue *>(hCircleQueue);
    const std::lock_guard<DataQueue> lock(*pQueue);
    return pQueue->GetStatus(*pstStatus);
}

//!\brief设置日志级别
void CircleQueue_SetLogLevel(int  s32Level)
{
}
