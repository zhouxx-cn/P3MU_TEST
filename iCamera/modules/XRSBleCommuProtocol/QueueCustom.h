/*!
 *\file     QueueCustom.h
 *\brief	定制版本队列实现
 */
#ifndef __H_QUEUE_CUSTOM__
#define __H_QUEUE_CUSTOM__

#ifdef __cplusplus
extern "C" {
#endif

//初始化参数
typedef struct stXRSBLE_QueueInitParam
{
    //队列中节点个数
    unsigned int u32NodeCnt;
    //每个节点占用空间大小
    unsigned int u32NodeSize;
} XRSBLE_QueueInitParam;

//队列上下文
typedef struct stXRSBLE_QueueCtx
{
    //队列中有效节点个数
    unsigned int u32ValidNodeCnt;
    //队头位置索引
    unsigned int u32HeadIndex;
    //队尾位置索引
    unsigned int u32TailIndex;
    //队列地址
    void *QueueAddr;
    //队列初始化信息
    XRSBLE_QueueInitParam stInitParam;
} XRSBLE_QueueCtx;

//初始化队列
int XRSBLE_QueueInit(XRSBLE_QueueInitParam *pParam, XRSBLE_QueueCtx **ppQueueCtx);
//队尾入队列
int XRSBLE_QueuePush(XRSBLE_QueueCtx *pQueueCtx, void *pData, unsigned int u32DataLen);
//队头出队列
int XRSBLE_QueuePop(XRSBLE_QueueCtx *pQueueCtx);
//获取队列头部元素
int XRSBLE_QueueFront(XRSBLE_QueueCtx *pQueueCtx, void **ppData);
//销毁队列
int XRSBLE_QueueDestroy(XRSBLE_QueueCtx *pQueueCtx);

#ifdef __cplusplus
}
#endif

#endif // end of __H_QUEUE_CUSTOM__

