#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "QueueCustom.h"
#include "crc16_modbus.h"
#include "XRSBLE_L1.h"
#include "../PAR/CPubFunc.h"
#include "sys_banned.h"
#include "zwsdk.h"
///////////////////////////////////////调试系统//////////////////////////////////
#define	LOG_LEVEL_DEBUG     2
#define	LOG_LEVEL_INFO      1
#define	LOG_LEVEL_ERROR     0
#define	L1_LOG_LEVEL_DEF    LOG_LEVEL_DEBUG


#ifdef __IS_SUPPORT_ZDEBUG_LOG_MANAGE__
//输出调试信息
#define L1_DBG(format, ...)		ZDEBUG_DBG("module", format, ##__VA_ARGS__)
//输出日志信息
#define L1_INFO(format, ...)	ZDEBUG_INF("module", format, ##__VA_ARGS__)
//输出错误信息
#define L1_ERR(format, ...)		ZDEBUG_ERR("module", format, ##__VA_ARGS__)
#else

static const char *s_pLogLevelStr[] = 
{
    "ERR",
    "INF",
    "DBG",
};

//记录日志
#define L1_LOG(level, x...)        \
do      \
{       \
    if (level <= L1_LOG_LEVEL_DEF)  \
    {   \
        printf("[%s][%s:%d]: ", s_pLogLevelStr[level], __FUNCTION__, __LINE__);	\
	    printf(x);				\
    }   \
}   \
while (0)


//输出调试信息
#define L1_DBG(x...)	L1_LOG(LOG_LEVEL_DEBUG, x)
//输出日志信息
#define L1_INFO(x...)	L1_LOG(LOG_LEVEL_INFO, x)
//输出错误信息
#define L1_ERR(x...)	L1_LOG(LOG_LEVEL_ERROR, x)
#endif

//////////////////////////////////////////////////////////////////////////////
//发送队列长度
#define SEND_QUEUE_SIZE_MAX     10
//L1层帧头长度
#define L1_HEAD_LENTH           8
//L1层负载最大长度
#define L1_PAYLOAD_LENTH_MAX    120
//L1层帧最大长度
#define L1_FRAME_LENTH_MAX      (L1_HEAD_LENTH + L1_PAYLOAD_LENTH_MAX)
//L1层帧头magicNum
#define L1_HEAD_MAGICNUM        0xAB

//待发送数据及发送控制参数
typedef struct stSendDataAndParam
{
    //待发送数据（L1层）
	XRSBLE_U8 u8SendData[L1_FRAME_LENTH_MAX];
	//待发送数据长度
	XRSBLE_U16 u16DataLen;
	//发送参数
	XRSBLE_SendCtrlParam stSendCtlParam;
	//此数据分配到的SequenceId
	XRSBLE_U16 u16SeqId;
} SendDataAndParam;

//蓝牙发送数据接口(蓝牙库中实现)
static BleSendDataCallback s_pBleSendDataCallback = NULL;
//L2层数据接收&处理接口
static L2RecvProcessCallback s_pL2RecvProcessCallback = NULL;
//发送队列上下文
static XRSBLE_QueueCtx *s_pSendQueueCtx = NULL;
//L1层是否已初始化
static char s_bIsL1Inited = 0;
//L1层接口操作锁
static pthread_mutex_t s_muL1IntfLock = PTHREAD_MUTEX_INITIALIZER;
//发送队列锁
static pthread_mutex_t s_muSendQueueLock = PTHREAD_MUTEX_INITIALIZER;
//发送数据线程
static pthread_t s_pthSendThread = (pthread_t)0;
//是否退出发送线程
static char bIsExitSendThread = 0;

//Sequence Id生成
static XRSBLE_U16 SequenceIdGenerate()
{
    static XRSBLE_U16 s_u16SequenceId = 0;
    return s_u16SequenceId++;
}

//从发送队列中删除掉指定节点
static int SendQueueNodeDel(XRSBLE_U16 u16SeqIdDest)
{
    int nRet = 0;
    SendDataAndParam *pData = NULL;
    
    pthread_mutex_lock(&s_muSendQueueLock);
    nRet = XRSBLE_QueueFront(s_pSendQueueCtx, (void **)&pData);
    if (0 != nRet
        && -3 != nRet)
    {
        L1_ERR("XRSBLE_QueueFront failed:nRet=%d!!!\n", nRet);
    }
    if (NULL != pData)
    {
        //判断当前正在处理的这条数据是否还在队列中
        if (u16SeqIdDest == pData->u16SeqId)
        {
            //还在，则从发送队列中清除掉此数据
            nRet = XRSBLE_QueuePop(s_pSendQueueCtx);
            if (0 != nRet
                && -3 != nRet)
            {
                L1_ERR("XRSBLE_QueuePop failed:nRet=%d!!!\n", nRet);
            }
        }
    }    
    pthread_mutex_unlock(&s_muSendQueueLock);

    return nRet;
}

//发送数据线程
static void *SendDataThread(void *ptr)
{
    int nRet = 0;
    //发送任务是否空闲
    char bIsIdle = 1;
    //是否需要发送
    char bIsNeedSend = 0;
    //线程休眠时长
    const unsigned int u32SleepMs = 50;
    //指向队列中的头个数据
    SendDataAndParam *pData = NULL;
    //当前在处理的数据
    SendDataAndParam stSendDataAndParamCur = {0};
    //超时值备份
    XRSBLE_U32 u32TimeOutMsecBak = 0;
	ptr = ptr;
	
    while (1)
    {
        if (bIsExitSendThread)
        {
            break;
        }

        if (NULL == s_pBleSendDataCallback)
        {
            usleep(u32SleepMs*1000);
            continue;
        }

        //有数据需处理
        if (0 == bIsIdle)
        {
            //处理超时及重试
            if (stSendDataAndParamCur.stSendCtlParam.u32TimeOutMsec > u32SleepMs)
            {
                //超时倒记时
                stSendDataAndParamCur.stSendCtlParam.u32TimeOutMsec -= u32SleepMs;
            }
            else
            {
                if (0 == stSendDataAndParamCur.stSendCtlParam.u8ReTryCnt)
                {
                    //重试已全部超时，丢弃此条数据
                    SendQueueNodeDel(stSendDataAndParamCur.u16SeqId);
                }
                else
                {
                    //重试下一次
                    stSendDataAndParamCur.stSendCtlParam.u8ReTryCnt--;
                    stSendDataAndParamCur.stSendCtlParam.u32TimeOutMsec = u32TimeOutMsecBak;
                    bIsNeedSend = 1;
                }
            }
        }
        
        pthread_mutex_lock(&s_muSendQueueLock);
        pData = NULL;
        nRet = XRSBLE_QueueFront(s_pSendQueueCtx, (void **)&pData);
        if (-1 == nRet
            || -2 == nRet)
        {
            L1_ERR("XRSBLE_QueueFront failed:nRet=%d!!!\n", nRet);
        }
        else if (-3 == nRet)    //队列已空
        {
            bIsIdle = 1;
            bIsNeedSend = 0;
        }
        else
        {
            if (NULL != pData)
            {
                if (bIsIdle)
                {
                    //新数据待发送
                    stSendDataAndParamCur = *pData;
                    u32TimeOutMsecBak = stSendDataAndParamCur.stSendCtlParam.u32TimeOutMsec;
                    bIsIdle = 0;
                    bIsNeedSend = 1;
                }
                else
                {
                    //对比当前正在处理的这条数据是否还在队列中（即还未收到应答）
                    if (stSendDataAndParamCur.u16SeqId != pData->u16SeqId)
                    {
                        //目前在处理中的这条数据，已经收到应答，在队列中已被清除
                        //则取下条数据
                        stSendDataAndParamCur = *pData;
                        u32TimeOutMsecBak = stSendDataAndParamCur.stSendCtlParam.u32TimeOutMsec;
                        bIsIdle = 0;
                        bIsNeedSend = 1;
                    }
                }
            }
        }
        pthread_mutex_unlock(&s_muSendQueueLock);

        //有数据需处理
        if (0 == bIsIdle
            && bIsNeedSend)
        {
            if (NULL != s_pBleSendDataCallback)
            {
                #if 0   //debug
                ///////////////////////////////////////
                L1_DBG("\n[FAN-BLE][%s][%d]u16SeqId=%u u16DataLen=%u u8SendData=\n",
                    __FUNCTION__, __LINE__, 
                    stSendDataAndParamCur.u16SeqId,
                    stSendDataAndParamCur.u16DataLen);
                //debug
                int i = 0;
                for (i = 0; i < stSendDataAndParamCur.u16DataLen; i++)
                {
                    L1_DBG("%02x ", stSendDataAndParamCur.u8SendData[i]);
                }
                L1_DBG("\n[FAN-BLE][%s][%d]///////////////////\n",
                    __FUNCTION__, __LINE__);
                ///////////////////////////////////////
                #endif
            #if defined (__H4S_W__) || defined (__H8SX__) || defined (__H8SVX__) || defined (__H5SVX__) || defined (__H4_W__)
                ZWSys_GetWiFiInfo(&wifiInfo);
                if(!strstr((char *)wifiInfo.Model, "H3873UV100"))
                {
                    L1_INFO("send data with conidx(0x0)!!!\n");
                    //新版本lbh_server数据格式为conidx(0x0)+UUID+data，发送数据时需要在开头加上0x0
                    nRet = s_pBleSendDataCallback(stSendDataAndParamCur.u8SendData, 
                        stSendDataAndParamCur.u16DataLen, 1);
                }
                else
            #endif
                {
                    nRet = s_pBleSendDataCallback(stSendDataAndParamCur.u8SendData, 
                        stSendDataAndParamCur.u16DataLen, 0);
                }
            

                if (0 != nRet)
                {
                    L1_ERR("s_pBleSendDataCallback failed:nRet=%d!!!\n", nRet);
                }
                else
                {
                    bIsNeedSend = 0;
                }
            }
        }
        
        usleep(u32SleepMs*1000);
    }
    
    return NULL;
}

/*!
 *  功  能: XRSBLE协议L1层初始化（这里主要供L2层调用，应用层不需要调用）
 *  param1: L1层初始化参数
 *  retval: 0-成功，其他-错误码
 *  说  明: 重复调用，直接返回成功
 */
XRSBLE_S32 XRSBLE_L1_Init(XRSBLE_IN XRSBLE_L1InitParam *pParam)
{
    XRSBLE_S32 s32Ret = 0;
    XRSBLE_QueueInitParam stQueueInitParam = {0};

    if (NULL == pParam)
    {
        L1_ERR("Interface param illegal:pParam=%p!!!\n",
            pParam);
        return -1;
    }

    pthread_mutex_lock(&s_muL1IntfLock);

    if (s_bIsL1Inited)
    {
        pthread_mutex_unlock(&s_muL1IntfLock);
        return 0;
    }

    //记录回调接口
    s_pBleSendDataCallback = pParam->pBleSendDataCallback;
    s_pL2RecvProcessCallback = pParam->pL2RecvProcessCallback;
    
    //创建发送队列
    stQueueInitParam.u32NodeCnt = SEND_QUEUE_SIZE_MAX;
    stQueueInitParam.u32NodeSize = sizeof(SendDataAndParam);
    pthread_mutex_lock(&s_muSendQueueLock);
    s32Ret = XRSBLE_QueueInit(&stQueueInitParam, &s_pSendQueueCtx);
    pthread_mutex_unlock(&s_muSendQueueLock);
    if (0 != s32Ret)
    {
        s_pBleSendDataCallback = NULL;
        s_pL2RecvProcessCallback = NULL;
    
        //队列创建失败
        L1_ERR("XRSBLE_QueueInit err: s32Ret=%d!!!\n",
            s32Ret);
            
        pthread_mutex_unlock(&s_muL1IntfLock);
        return -2;
    }

    //创建发送线程
    bIsExitSendThread = 0;
    s32Ret = pthread_create(&s_pthSendThread, NULL, SendDataThread, NULL);
    if (0 != s32Ret)
    {
        pthread_mutex_lock(&s_muSendQueueLock);
        XRSBLE_QueueDestroy(s_pSendQueueCtx);
        s_pSendQueueCtx = NULL;
        pthread_mutex_unlock(&s_muSendQueueLock);
        
        s_pBleSendDataCallback = NULL;
        s_pL2RecvProcessCallback = NULL;
    
        //线程创建失败
        L1_ERR("Send Thread create failed: s32Ret=%d!!!\n",
            s32Ret);
            
        pthread_mutex_unlock(&s_muL1IntfLock);
        return -3;
    }

    s_bIsL1Inited = 1;

    pthread_mutex_unlock(&s_muL1IntfLock);
    return s32Ret;
}

/*!
 *  功  能: XRSBLE协议L1层逆初始化（这里主要供L2层调用，应用层不需要调用）
 *  retval: 0-成功，其他-错误码
 *  说  明: 重复调用，直接返回成功
 */
XRSBLE_S32 XRSBLE_L1_UnInit()
{
    XRSBLE_S32 s32Ret = 0;

    pthread_mutex_lock(&s_muL1IntfLock);
    
    if (0 == s_bIsL1Inited)
    {
        pthread_mutex_unlock(&s_muL1IntfLock);
        return 0;
    }

    //销毁发送线程
    bIsExitSendThread = 1;
    pthread_join(s_pthSendThread, NULL);
    s_pthSendThread = (pthread_t)0;
    bIsExitSendThread = 0;

    //销毁发送队列
    pthread_mutex_lock(&s_muSendQueueLock);
    XRSBLE_QueueDestroy(s_pSendQueueCtx);
    s_pSendQueueCtx = NULL;
    pthread_mutex_unlock(&s_muSendQueueLock);

    //回调接口置空
    s_pBleSendDataCallback = NULL;
    s_pL2RecvProcessCallback = NULL;

    s_bIsL1Inited = 0;

    pthread_mutex_unlock(&s_muL1IntfLock);
    return s32Ret;
}

/*!
 *  功  能: XRSBLE协议L1层发送数据（这里主要供L2层调用，应用层不需要调用）
 *  param1: 指向要发送的数据的指针（L2层次数据）
 *  param2: 待发送的数据的长度
 *  param3: 数据发送控制参数
 *  param4: 返回此次发送数据的SequenceId，供后续接收应答数据时寻找对应关系
 *  retval: 0-成功，其他-错误码
 *  说  明: 内部采用异步处理，会将待发送数据先存入队列
 */
XRSBLE_S32 XRSBLE_L1_SendData(XRSBLE_IN XRSBLE_U8 *pdata, XRSBLE_IN XRSBLE_U32 u32Len, XRSBLE_IN XRSBLE_SendCtrlParam *pSendCtlParam, XRSBLE_OUT XRSBLE_U16 *pSeqId)
{
    XRSBLE_S32 s32Ret = 0;
    XRSBLE_U16 u16DataLen = 0;
    SendDataAndParam stuSendDataAndParam = {0};
    XRSBLE_U16 u16PayloadLen = 0;
    XRSBLE_U16 u16Crc = 0;
    XRSBLE_U16 u16SeqId = 0;
    XRSBLE_U16 *pu16Tmp = NULL;

    if (NULL == pdata
        || NULL == pSendCtlParam
        || NULL == pSeqId)
    {
        L1_ERR("Interface param illegal: pdata=%p pSendCtlParam=%p pSeqId=%p!!!\n",
            pdata, pSendCtlParam, pSeqId);
        return -1;
    }

    if (0 == u32Len
        || u32Len > L1_PAYLOAD_LENTH_MAX)
    {
        L1_ERR("Data lenth illegal: u32Len=%d maxSup=%d!!!\n", 
            u32Len, L1_PAYLOAD_LENTH_MAX);
        return -2;
    }

    pthread_mutex_lock(&s_muL1IntfLock);
    
    if (0 == s_bIsL1Inited)
    {
        //未完成初始化
        L1_ERR("XRSBLE_L1 not init!!!\n");

        pthread_mutex_unlock(&s_muL1IntfLock);
        return -3;
    }

    //构建L1层帧头
    //magicNum
    stuSendDataAndParam.u8SendData[0] = (XRSBLE_U8)L1_HEAD_MAGICNUM;
    //ErrFlag/AckFlag/Version
    stuSendDataAndParam.u8SendData[1] = (0x0<<5) | (0x0<<4) | (XRSBLE_PROTOCOL_VERSION&0x0F);
    //注意：本地字节序转网络
    //PayloadLen
    u16PayloadLen = u32Len;
    pu16Tmp = (XRSBLE_U16 *)(&stuSendDataAndParam.u8SendData[2]);
    *pu16Tmp = htons(u16PayloadLen);
    //CRC
    stuSendDataAndParam.u8SendData[4] = 0;
    stuSendDataAndParam.u8SendData[5] = 0;
    //SeqId
    //生成Sequence Id
    u16SeqId = SequenceIdGenerate();
    pu16Tmp = (XRSBLE_U16 *)(&stuSendDataAndParam.u8SendData[6]);
    *pu16Tmp = htons(u16SeqId);

    //填充负载
    memcpy((stuSendDataAndParam.u8SendData+L1_HEAD_LENTH), pdata, u16PayloadLen);

    //补充CRC校验码
    /* 注意：
    这里的CRC校验码需要对整个L1 header及L1 payload进行CRC计算得出。
    发送端组帧，先将L1 header中的CRC16填充为0，对整个L1 header及L1 payload
    进行CRC计算得到CRC校验码后，填入CRC16位置处。
    接收端解帧，先将L1 header中的CRC16取出备份，然后将L1 header中的CRC16覆写为0，
    并对整个L1 header及L1 payload进行CRC计算得到CRC校验码，
    最后通过比对CRC校验码确定数据有效性。*/
    u16DataLen = L1_HEAD_LENTH + u16PayloadLen;
    u16Crc = Crc16Modbus(stuSendDataAndParam.u8SendData, u16DataLen);
    pu16Tmp = (XRSBLE_U16 *)(&stuSendDataAndParam.u8SendData[4]);
    *pu16Tmp = htons(u16Crc);

    #if 0   //debug
    ///////////////////////////////////////
    L1_DBG("\n[FAN-BLE][%s][%d]u16SeqId=%u u16DataLen=%u u8SendData=\n",
        __FUNCTION__, __LINE__, u16SeqId, u16DataLen);
    //debug
    int i = 0;
    for (i = 0; i < u16DataLen; i++)
    {
        L1_DBG("%02x ", stuSendDataAndParam.u8SendData[i]);
    }
    L1_DBG("\n[FAN-BLE][%s][%d]///////////////////\n",
        __FUNCTION__, __LINE__);
    ///////////////////////////////////////
    #endif

    //将待发送数据及发送控制参数放入队列
    stuSendDataAndParam.stSendCtlParam = *pSendCtlParam;
    stuSendDataAndParam.u16SeqId = u16SeqId;
    stuSendDataAndParam.u16DataLen = u16DataLen;
    pthread_mutex_lock(&s_muSendQueueLock);
    s32Ret = XRSBLE_QueuePush(s_pSendQueueCtx, (void *)(&stuSendDataAndParam), sizeof(stuSendDataAndParam));
    pthread_mutex_unlock(&s_muSendQueueLock);
    if (0 != s32Ret)
    {
        //发送数据失败
        L1_ERR("XRSBLE_QueuePush err: s32Ret=%d!!!\n", s32Ret);

        pthread_mutex_unlock(&s_muL1IntfLock);
        return -4;
    }
    
    //返回此次发送数据的SequenceId
    *pSeqId = u16SeqId;

    pthread_mutex_unlock(&s_muL1IntfLock);
    return s32Ret;
}

/*!
 *  功  能: XRSBLE协议L1层接收&处理数据（这里供应用层调用，设置给蓝牙库，收到数据时回调）
 *  param1: 指向接收到的数据的指针（蓝牙接收数据）
 *  param2: 接收到的数据的长度
 *  retval: 0-成功，其他-错误码
 *  说  明: 蓝牙收到数据后，回调此接口，内部进行数据校验/解析/进一步回调L2层，同时其内部会做应答处理
 *  注意: 需避免长时间阻塞
 */
XRSBLE_S32 XRSBLE_L1_RecvProcess(XRSBLE_IN XRSBLE_U8 *pdata, XRSBLE_IN XRSBLE_U32 u32Len, int conidxFlag)
{
    XRSBLE_S32 s32Ret = 0;
    XRSBLE_U8 u8Buf[L1_FRAME_LENTH_MAX] = {0};
    XRSBLE_U8 u8AckBuf[L1_PAYLOAD_LENTH_MAX] = {0};
    XRSBLE_U32 u32AckLen = 0;
    //校验是否失败
    XRSBLE_U8 bIsChkFailed = 0;
    XRSBLE_U8 u8MagicNum = 0;
    XRSBLE_U8 u8ErrFlag = 0;
    XRSBLE_U8 u8AckFlag = 0;
    XRSBLE_U8 u8Version = 0;
    XRSBLE_U16 u16PayloadLen = 0;
    XRSBLE_U16 u16Crc = 0;
    XRSBLE_U16 u16CrcBak = 0;
    XRSBLE_U16 u16SeqId = 0;
    XRSBLE_U16 *pu16Tmp = NULL;

    if (NULL == pdata)
    {
        L1_ERR("Interface param illegal:pdata=%p!!!\n",
            pdata);
        return -1;
    }

    if (u32Len < L1_HEAD_LENTH)
    {
        L1_ERR("Data lenth illegal:u32Len=%d minSup=%d!!!\n", 
            u32Len, L1_HEAD_LENTH);
        return -2;
    }

    //pthread_mutex_lock(&s_muL1IntfLock);

    if (0 == s_bIsL1Inited)
    {
        //未完成初始化
        L1_ERR("XRSBLE_L1 not init!!!\n");
        
        //pthread_mutex_unlock(&s_muL1IntfLock);
        return -3;
    }

    #if 0   //debug
    ///////////////////////////////////////
    L1_DBG("\n[FAN-BLE][%s][%d]u32Len=%u pdata=\n",
        __FUNCTION__, __LINE__, u32Len);
    //debug
    int i = 0;
    for (i = 0; i < u32Len; i++)
    {
        L1_DBG("%02x ", pdata[i]);
    }
    L1_DBG("\n[FAN-BLE][%s][%d]///////////////////\n",
        __FUNCTION__, __LINE__);
    ///////////////////////////////////////
    #endif

    if (u32Len > L1_FRAME_LENTH_MAX)
    {
        L1_ERR("Data lenth illegal: u32Len=%d, maxSup=%d!!!\n", 
            u32Len, L1_FRAME_LENTH_MAX);
        //长度超出合法范围，放弃解析，直接应答错误
        bIsChkFailed = 1;
        pu16Tmp = (XRSBLE_U16 *)(&u8Buf[6]);
        u16SeqId = ntohs(*pu16Tmp);
    }
    else
    {
        memcpy(u8Buf, pdata, u32Len);

        //解析L1层帧头
        u8MagicNum = u8Buf[0];
        u8ErrFlag = (u8Buf[1]&0x20)>>5;
        u8AckFlag = (u8Buf[1]&0x10)>>4;
        u8Version = u8Buf[1]&0x0F;
        //注意：网络字节序转本地
        pu16Tmp = (XRSBLE_U16 *)(&u8Buf[2]);
        u16PayloadLen = ntohs(*pu16Tmp);
        pu16Tmp = (XRSBLE_U16 *)(&u8Buf[4]);
        u16CrcBak = ntohs(*pu16Tmp);
        pu16Tmp = (XRSBLE_U16 *)(&u8Buf[6]);
        u16SeqId = ntohs(*pu16Tmp);

        //计算CRC
        /* 注意：
        这里的CRC校验码需要对整个L1 header及L1 payload进行CRC计算得出。
        发送端组帧，先将L1 header中的CRC16填充为0，对整个L1 header及L1 payload
        进行CRC计算得到CRC校验码后，填入CRC16位置处。
        接收端解帧，先将L1 header中的CRC16取出备份，然后将L1 header中的CRC16覆写为0，
        并对整个L1 header及L1 payload进行CRC计算得到CRC校验码，
        最后通过比对CRC校验码确定数据有效性。*/
        u8Buf[4] = 0x0;
        u8Buf[5] = 0x0;
        u16Crc = Crc16Modbus(u8Buf, u32Len);

        /* 校验数据 */
        if (L1_HEAD_MAGICNUM != u8MagicNum)
        {
            //magicNum校验不通过
            L1_ERR("L1_HEAD_MAGICNUM not match: local=%d, remote=%d!!!\n", 
                L1_HEAD_MAGICNUM, u8MagicNum);
            bIsChkFailed = 1;
        }
        if (u16Crc != u16CrcBak)
        {
            //crc校验不通过
            L1_ERR("Crc check failed: local=%#x, remote=%#x!!!\n", 
                u16Crc, u16CrcBak);
            bIsChkFailed = 1;
        }

        L1_DBG("\nu8MagicNum=%#x u8ErrFlag=%u u8AckFlag=%u u8Version=%u u16PayloadLen=%u u16CrcBak=%#x u16Crc=%#x u16SeqId=%u\n",
            u8MagicNum, u8ErrFlag, u8AckFlag, u8Version,
            u16PayloadLen, u16CrcBak, u16Crc, u16SeqId);
        
        //比对协议版本
        //考虑到获取设备协议版本号也是走这里，这里无法区分
        //因此对版本号不匹配，只做打印提示，不能直接不处理或应答错误
        //目前方案：依赖APP端获取设备协议版本号后，判断是否匹配及决定是否交互)
        if (XRSBLE_PROTOCOL_VERSION != u8Version)
        {
            //版本号不匹配
            L1_ERR("Warren：XRSBLE_PROTOCOL_VERSION not match: local=%d, remote=%d!!!\n",
                XRSBLE_PROTOCOL_VERSION, u8Version);
        }

        //校验通过
        if (0 == bIsChkFailed)
        {
            //收到的是应答数据
            if (u8AckFlag)
            {
                //应答成功
                if (0 == u8ErrFlag)
                {
                    //从发送队列中清除掉此数据(此条数据发送完成)
                    SendQueueNodeDel(u16SeqId);

                    //将应答数据回调给L2层（用于传递对应请求操作的返回值）
                    if (NULL != s_pL2RecvProcessCallback)
                    {
                        s32Ret = s_pL2RecvProcessCallback((u8Buf+L1_HEAD_LENTH), u16PayloadLen, u16SeqId, NULL, NULL);
                        if (0 != s32Ret)
                        {
                            L1_ERR("s_pL2RecvProcessCallback failed: s32Ret=%d!!!\n", s32Ret);
                        }
                    }
                }
            }
            else
            {
                //将普通数据回调给L2层
                if (NULL != s_pL2RecvProcessCallback)
                {
                    u32AckLen = sizeof(u8AckBuf);
                    s32Ret = s_pL2RecvProcessCallback((u8Buf+L1_HEAD_LENTH), u16PayloadLen, u16SeqId, u8AckBuf, &u32AckLen);
                    if (0 != s32Ret)
                    {
                        u32AckLen = 0;
                        L1_ERR("s_pL2RecvProcessCallback failed: s32Ret=%d!!!\n", s32Ret);
                    }
                }
            }            
        }
    }
    
    //针对收到的普通数据（即请求数据）做应答
    if (0 == u8AckFlag)
    {
        memset(u8Buf, 0, sizeof(u8Buf));
        
        //构建应答帧帧头
        //magicNum
        u8Buf[0] = (XRSBLE_U8)L1_HEAD_MAGICNUM;
        //ErrFlag/AckFlag/Version
        if (bIsChkFailed)
        {
            u8Buf[1] = (0x1<<5) | (0x1<<4) | (XRSBLE_PROTOCOL_VERSION&0x0F);
        }
        else
        {
            u8Buf[1] = (0x0<<5) | (0x1<<4) | (XRSBLE_PROTOCOL_VERSION&0x0F);
        }

        //注意：本地字节序转网络
        //PayloadLen
        u16PayloadLen = u32AckLen;
        pu16Tmp = (XRSBLE_U16 *)(&u8Buf[2]);
        *pu16Tmp = htons(u16PayloadLen);
        //CRC
        u8Buf[4] = 0;
        u8Buf[5] = 0;
        //SeqId
        pu16Tmp = (XRSBLE_U16 *)(&u8Buf[6]);
        *pu16Tmp = htons(u16SeqId);

        //填充负载
        if (u16PayloadLen > 0)
        {
            memcpy((u8Buf+L1_HEAD_LENTH), u8AckBuf, u16PayloadLen);
        }

        //补充CRC校验码
        u16Crc = Crc16Modbus(u8Buf, (L1_HEAD_LENTH+u16PayloadLen));
        pu16Tmp = (XRSBLE_U16 *)(&u8Buf[4]);
        *pu16Tmp = htons(u16Crc);

        L1_DBG("u16Crc=%#x *pu16Tmp=%#x u16PayloadLen=%u\n",
            u16Crc, *pu16Tmp, u16PayloadLen);

        #if 0   //debug
        ///////////////////////////////////////
        L1_DBG("\n[FAN-BLE][%s][%d]uLen=%u u8Buf=\n",
            __FUNCTION__, __LINE__,
            L1_HEAD_LENTH+u16PayloadLen);
        //debug
        for (i = 0; i < (L1_HEAD_LENTH+u16PayloadLen); i++)
        {
            L1_DBG("%02x ", u8Buf[i]);
        }
        L1_DBG("\n[FAN-BLE][%s][%d]///////////////////\n",
            __FUNCTION__, __LINE__);
        ///////////////////////////////////////
        #endif

        //发送应答数据
        if (NULL != s_pBleSendDataCallback)
        {
            s32Ret = s_pBleSendDataCallback(u8Buf, (L1_HEAD_LENTH+u16PayloadLen), conidxFlag);
            if (0 != s32Ret)
            {
                L1_ERR("s_pBleSendDataCallback(ACK) failed:s32Ret=%d!!!\n", s32Ret);
            }
        }
    }

    //pthread_mutex_unlock(&s_muL1IntfLock);
    return 0;
}

