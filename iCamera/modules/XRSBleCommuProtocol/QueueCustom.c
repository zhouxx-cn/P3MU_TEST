#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "QueueCustom.h"
#include "sys_banned.h"
//初始化队列
int XRSBLE_QueueInit(XRSBLE_QueueInitParam *pParam, XRSBLE_QueueCtx **ppQueueCtx)
{
    int nRet = 0;
    unsigned int u32Size = 0;
    
    if (NULL == pParam
        || NULL == ppQueueCtx)
    {
        //参数非法
        return -1;
    }

    *ppQueueCtx = (XRSBLE_QueueCtx *)malloc(sizeof(XRSBLE_QueueCtx));
    if (NULL == *ppQueueCtx)
    {
        //内存申请失败
        return -2;
    }

    memset((*ppQueueCtx), 0, sizeof(XRSBLE_QueueCtx));
    (*ppQueueCtx)->stInitParam = *pParam;
    (*ppQueueCtx)->u32HeadIndex = 0;
    (*ppQueueCtx)->u32TailIndex = 0;
    (*ppQueueCtx)->u32ValidNodeCnt = 0;
    u32Size = pParam->u32NodeCnt * pParam->u32NodeSize;
    (*ppQueueCtx)->QueueAddr = malloc(u32Size);
    if (NULL == (*ppQueueCtx)->QueueAddr)
    {
        free(*ppQueueCtx);
        *ppQueueCtx = NULL;

        //内存申请失败
        return -2;
    }

    memset((*ppQueueCtx)->QueueAddr, 0, u32Size);
    
    return nRet;
}

//队尾入队列
int XRSBLE_QueuePush(XRSBLE_QueueCtx *pQueueCtx, void *pData, unsigned int u32DataLen)
{
    int nRet = 0;
    unsigned int u32Offset = 0;
    unsigned int u32Index = 0;
    
    if (NULL == pQueueCtx
        || NULL == pData)
    {
        //参数非法
        return -1;
    }

    if (NULL == pQueueCtx->QueueAddr)
    {
        //队列未初始化
        return -2;
    }

    if (0 == u32DataLen
        || u32DataLen > pQueueCtx->stInitParam.u32NodeSize)
    {
        //数据长度非法
        return -3;
    }

    if (pQueueCtx->u32ValidNodeCnt >= pQueueCtx->stInitParam.u32NodeCnt)
    {
        //队列满
        return -4;
    }

    u32Index = pQueueCtx->u32TailIndex;
    u32Offset = u32Index * pQueueCtx->stInitParam.u32NodeSize;
    memcpy((void *)((unsigned char *)pQueueCtx->QueueAddr + u32Offset), pData, u32DataLen);
        
    pQueueCtx->u32ValidNodeCnt++;
    pQueueCtx->u32TailIndex = (u32Index + 1) % pQueueCtx->stInitParam.u32NodeCnt;
    
    return nRet;
}

//队头出队列
int XRSBLE_QueuePop(XRSBLE_QueueCtx *pQueueCtx)
{
    int nRet = 0;
    unsigned int u32Offset = 0;
    unsigned int u32Index = 0;
    
    if (NULL == pQueueCtx)
    {
        //参数非法
        return -1;
    }

    if (NULL == pQueueCtx->QueueAddr)
    {
        //队列未初始化
        return -2;
    }

    if (0 == pQueueCtx->u32ValidNodeCnt)
    {
        //队列空
        return -3;
    }
    
    u32Index = pQueueCtx->u32HeadIndex;
    u32Offset = u32Index * pQueueCtx->stInitParam.u32NodeSize;
    memset((void *)((unsigned char *)pQueueCtx->QueueAddr + u32Offset), 0, pQueueCtx->stInitParam.u32NodeSize);

    pQueueCtx->u32ValidNodeCnt--;
    pQueueCtx->u32HeadIndex = (u32Index + 1) % pQueueCtx->stInitParam.u32NodeCnt;
    
    return nRet;
}

//获取队列头部元素
int XRSBLE_QueueFront(XRSBLE_QueueCtx *pQueueCtx, void **ppData)
{
    int nRet = 0;
    unsigned int u32Offset = 0;
    unsigned int u32Index = 0;
    
    if (NULL == pQueueCtx
        || NULL == ppData)
    {
        //参数非法
        return -1;
    }

    if (NULL == pQueueCtx->QueueAddr)
    {
        //队列未初始化
        return -2;
    }

    if (0 == pQueueCtx->u32ValidNodeCnt)
    {
        //队列空
        return -3;
    }

    u32Index = pQueueCtx->u32HeadIndex;
    u32Offset = u32Index * pQueueCtx->stInitParam.u32NodeSize;
    *ppData = (void *)((unsigned char *)pQueueCtx->QueueAddr + u32Offset);
    
    return nRet;
}

//销毁队列
int XRSBLE_QueueDestroy(XRSBLE_QueueCtx *pQueueCtx)
{
    int nRet = 0;
    
    if (NULL == pQueueCtx)
    {
        //参数非法
        return -1;
    }

    if (NULL != pQueueCtx->QueueAddr)
    {
        free(pQueueCtx->QueueAddr);
        pQueueCtx->QueueAddr = NULL;
    }
    free(pQueueCtx);

    return nRet;
}

