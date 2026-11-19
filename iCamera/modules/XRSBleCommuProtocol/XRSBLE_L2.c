#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <arpa/inet.h>
#include "XRSBLE_L1.h"
#include "XRSBLE_L2.h"
#include "../PAR/CPubFunc.h"
#include "sys_banned.h"

///////////////////////////////////////调试系统//////////////////////////////////
#define	LOG_LEVEL_DEBUG     2
#define	LOG_LEVEL_INFO      1
#define	LOG_LEVEL_ERROR     0
#define	L2_LOG_LEVEL_DEF    LOG_LEVEL_DEBUG


#ifdef __IS_SUPPORT_ZDEBUG_LOG_MANAGE__
//输出调试信息
#define L2_DBG(format, ...)		ZDEBUG_DBG("module", format, ##__VA_ARGS__)
//输出日志信息
#define L2_INFO(format, ...)	ZDEBUG_INF("module", format, ##__VA_ARGS__)
//输出错误信息
#define L2_ERR(format, ...)		ZDEBUG_ERR("module", format, ##__VA_ARGS__)
#else

static const char *s_pLogLevelStr[] = 
{
    "ERR",
    "INF",
    "DBG",
};

//记录日志
#define L2_LOG(level, x...)        \
do      \
{       \
	if (level <= L2_LOG_LEVEL_DEF)  \
	{   \
        printf("[%s][%s:%d]: ", s_pLogLevelStr[level], __FUNCTION__, __LINE__);	\
	    printf(x);				\
	}   \
}   \
while (0)

//输出调试信息
#define L2_DBG(x...)	L2_LOG(LOG_LEVEL_DEBUG, x)
//输出日志信息
#define L2_INFO(x...)	L2_LOG(LOG_LEVEL_INFO, x)
//输出错误信息
#define L2_ERR(x...)	L2_LOG(LOG_LEVEL_ERROR, x)
#endif
//////////////////////////////////////////////////////////////////////////////
//L2层帧头长度
#define L2_HEAD_LENTH           2
//Key帧头长度
#define KEY_HEAD_LENTH          2
//L2层负载最大长度
#define L2_PAYLOAD_LENTH_MAX    118
//L2层帧最大长度
#define L2_FRAME_LENTH_MAX      (L2_HEAD_LENTH + L2_PAYLOAD_LENTH_MAX)

//XRSBLE协议L2层接收&处理数据
static XRSBLE_S32 XRSBLE_L2_RecvProcess(XRSBLE_IN XRSBLE_U8 *pRecvdata, XRSBLE_IN XRSBLE_U32 u32RecvDataLen, XRSBLE_IN XRSBLE_U16 u16SeqId, XRSBLE_OUT XRSBLE_U8 *u8AckDataBuf, XRSBLE_INOUT XRSBLE_U32 *pDataLen);

//L2层数据接收&处理接口
static AppRecvProcessCallback s_pAppRecvProcessCallback = NULL;
//L2层是否已初始化
static char s_bIsL2Inited = 0;
//L2层接口操作锁
static pthread_mutex_t s_muL2IntfLock = PTHREAD_MUTEX_INITIALIZER;


//将命令数据转化为L2层协议格式（CMD_ID_SET_INFO）
//pBufLen：传入缓冲区大小，传出转化后数据长度
static int CmdDataTransform_SetInfo(XRSBLE_L2CmdDataSet *pCmdDataSet, XRSBLE_U8 *pBuff, XRSBLE_U16 *pBufLen)
{
    int i = 0;
    XRSBLE_U16 u16BufLen = 0;
    XRSBLE_U16 u16ValueLen = 0;
    XRSBLE_S16 *ps16Tmp = NULL;
    XRSBLE_U8 u8Tmp[L2_PAYLOAD_LENTH_MAX] = {0};
    XRSBLE_U16 u16DataLen = 0;
    
    if (NULL == pCmdDataSet
        || NULL == pBuff
        || NULL == pBufLen)
    {
        //参数非法
        L2_ERR("Interface param illegal!!!\n");
        return -1;
    }

    u16BufLen = *pBufLen;
    *pBufLen = 0;
    u16DataLen = 0;

    for (i = 0; i < pCmdDataSet->u8CmdCnt; i++)
    {
        switch (pCmdDataSet->stCmdData[i].u8KeyId)
        {
            case KEY_ID_WIFI_SSID:
            {
                XRSBLE_WifiSsidInfo *pSsidInfo = NULL;
                
                if (NULL == pCmdDataSet->stCmdData[i].pData)
                {
                    //参数非法
                    L2_ERR("Interface param illegal: pCmdData->pData=%p!!!\n",
                        pCmdDataSet->stCmdData[i].pData);
                    break;
                }
                if (pCmdDataSet->stCmdData[i].u16DataLen < sizeof(XRSBLE_WifiSsidInfo))
                {
                    //缓冲区不匹配
                    L2_ERR("Datalen not match: u16DataLen=%d sizeof(struct)=%d!!!\n",
                        pCmdDataSet->stCmdData[i].u16DataLen, sizeof(XRSBLE_WifiSsidInfo));
                    break;
                }

                pSsidInfo = (XRSBLE_WifiSsidInfo *)pCmdDataSet->stCmdData[i].pData;

                //Key
                u8Tmp[u16DataLen] = pCmdDataSet->stCmdData[i].u8KeyId;
                u16DataLen += 1;
                
                //Key header
                u16ValueLen = strlen(pSsidInfo->szWifiSsidBase64);
                u8Tmp[u16DataLen] = (XRSBLE_U8)((u16ValueLen >> 8) & 0x01);
                u16DataLen += 1;
                u8Tmp[u16DataLen] = (XRSBLE_U8)(u16ValueLen & 0xFF);
                u16DataLen += 1;
                
                //Value
                memcpy((u8Tmp+u16DataLen), pSsidInfo->szWifiSsidBase64, u16ValueLen);
                u16DataLen += u16ValueLen;

                L2_DBG("i=%d u8KeyId=%u szWifiSsidBase64=%s u16ValueLen=%u",
                    i, pCmdDataSet->stCmdData[i].u8KeyId,
                    pSsidInfo->szWifiSsidBase64, u16ValueLen);
                
                break;
            }
            case KEY_ID_WIFI_PWD:
            {
                XRSBLE_WifiPwdInfo *pPwdInfo = NULL;

                if (NULL == pCmdDataSet->stCmdData[i].pData)
                {
                    //参数非法
                    L2_ERR("Interface param illegal: pCmdData->pData=%p!!!\n",
                        pCmdDataSet->stCmdData[i].pData);
                    break;
                }
                if (pCmdDataSet->stCmdData[i].u16DataLen < sizeof(XRSBLE_WifiPwdInfo))
                {
                    //缓冲区不匹配
                    L2_ERR("Datalen not match: u16DataLen=%d sizeof(struct)=%d!!!\n",
                        pCmdDataSet->stCmdData[i].u16DataLen, sizeof(XRSBLE_WifiPwdInfo));
                    break;
                }

                pPwdInfo = (XRSBLE_WifiPwdInfo *)pCmdDataSet->stCmdData[i].pData;

                //Key
                u8Tmp[u16DataLen] = pCmdDataSet->stCmdData[i].u8KeyId;
                u16DataLen += 1;
                
                //Key header
                u16ValueLen = strlen(pPwdInfo->szWifiPwdBase64);
                u8Tmp[u16DataLen] = (XRSBLE_U8)((u16ValueLen >> 8) & 0x01);
                u16DataLen += 1;
                u8Tmp[u16DataLen] = (XRSBLE_U8)(u16ValueLen & 0xFF);
                u16DataLen += 1;
                
                //Value
                memcpy((u8Tmp+u16DataLen), pPwdInfo->szWifiPwdBase64, u16ValueLen);
                u16DataLen += u16ValueLen;

                L2_DBG("i=%d u8KeyId=%u szWifiPwdBase64=%s u16ValueLen=%u",
                    i, pCmdDataSet->stCmdData[i].u8KeyId,
                    pPwdInfo->szWifiPwdBase64, u16ValueLen);
                
                break;
            }
            case KEY_ID_TIMEZONE:
            {
                XRSBLE_TimeZoneInfo *pTimeZoneInfo = NULL;

                if (NULL == pCmdDataSet->stCmdData[i].pData)
                {
                    //参数非法
                    L2_ERR("Interface param illegal: pCmdData->pData=%p!!!\n",
                        pCmdDataSet->stCmdData[i].pData);
                    break;
                }
                if (pCmdDataSet->stCmdData[i].u16DataLen < sizeof(XRSBLE_TimeZoneInfo))
                {
                    //缓冲区不匹配
                    L2_ERR("Datalen not match: u16DataLen=%d sizeof(struct)=%d!!!\n",
                        pCmdDataSet->stCmdData[i].u16DataLen, sizeof(XRSBLE_TimeZoneInfo));
                    break;
                }

                pTimeZoneInfo = (XRSBLE_TimeZoneInfo *)pCmdDataSet->stCmdData[i].pData;

                //Key
                u8Tmp[u16DataLen] = pCmdDataSet->stCmdData[i].u8KeyId;
                u16DataLen += 1;
                
                //Key header
                u16ValueLen = 2;
                u8Tmp[u16DataLen] = (XRSBLE_U8)((u16ValueLen >> 8) & 0x01);
                u16DataLen += 1;
                u8Tmp[u16DataLen] = (XRSBLE_U8)(u16ValueLen & 0xFF);
                u16DataLen += 1;
                
                //Value
                ps16Tmp = (XRSBLE_S16 *)(&u8Tmp[u16DataLen]);
                *ps16Tmp = htons(pTimeZoneInfo->s16TimeZoneMin);
                u16DataLen += u16ValueLen;

                L2_DBG("i=%d u8KeyId=%u s16TimeZoneMin=%d u16ValueLen=%u",
                    i, pCmdDataSet->stCmdData[i].u8KeyId,
                    pTimeZoneInfo->s16TimeZoneMin, u16ValueLen);
                
                break;
            }
            default:
            {
                L2_ERR("KEY_ID unsupport: %d!!!\n", pCmdDataSet->stCmdData[i].u8KeyId);
                return -2;
                break;
            }
        }
    }

    if (u16DataLen > u16BufLen)
    {
        //转化后数据长度超出缓冲区大小
        L2_ERR("Data too long: DataLen=%d BufLen=%d!!!\n",
            u16DataLen, u16BufLen);
        return -4;
    }
    else
    {
        memcpy(pBuff, u8Tmp, u16DataLen);
        *pBufLen = u16DataLen;
    }
    
    return 0;
}

//将命令数据转化为L2层协议格式（CMD_ID_SET_INFO）
//pBufLen：传入缓冲区大小，传出转化后数据长度
static int CmdDataTransform_GetInfo(XRSBLE_L2CmdDataSet *pCmdDataSet, XRSBLE_U8 *pBuff, XRSBLE_U16 *pBufLen)
{
    int i = 0;
    XRSBLE_U16 u16BufLen = 0;
    XRSBLE_U16 u16ValueLen = 0;
    XRSBLE_U8 u8Tmp[L2_PAYLOAD_LENTH_MAX] = {0};
    XRSBLE_U16 u16DataLen = 0;
    
    if (NULL == pCmdDataSet
        || NULL == pBuff
        || NULL == pBufLen)
    {
        //参数非法
        L2_ERR("Interface param illegal!!!\n");
        return -1;
    }

    u16BufLen = *pBufLen;
    *pBufLen = 0;
    u16DataLen = 0;

    for (i = 0; i < pCmdDataSet->u8CmdCnt; i++)
    {
        switch (pCmdDataSet->stCmdData[i].u8KeyId)
        {
            case KEY_ID_PROTOC_VER_REQ:
            {
                //Key
                u8Tmp[u16DataLen] = pCmdDataSet->stCmdData[i].u8KeyId;
                u16DataLen += 1;
                
                //Key header
                u16ValueLen = 0;
                u8Tmp[u16DataLen] = (XRSBLE_U8)((u16ValueLen >> 8) & 0x01);
                u16DataLen += 1;
                u8Tmp[u16DataLen] = (XRSBLE_U8)(u16ValueLen & 0xFF);
                u16DataLen += 1;
                u16DataLen += u16ValueLen;
                
                break;
            }
            case KEY_ID_PROTOC_VER_REPLY:
            {
                XRSBLE_ProtocVerInfo *pProtocVerInfo = NULL;

                if (NULL == pCmdDataSet->stCmdData[i].pData)
                {
                    //参数非法
                    L2_ERR("Interface param illegal: pCmdData->pData=%p!!!\n",
                        pCmdDataSet->stCmdData[i].pData);
                    break;
                }
                if (pCmdDataSet->stCmdData[i].u16DataLen < sizeof(XRSBLE_ProtocVerInfo))
                {
                    //缓冲区不匹配
                    L2_ERR("Datalen not match: u16DataLen=%d sizeof(struct)=%d!!!\n",
                        pCmdDataSet->stCmdData[i].u16DataLen, sizeof(XRSBLE_ProtocVerInfo));
                    break;
                }

                pProtocVerInfo = (XRSBLE_ProtocVerInfo *)pCmdDataSet->stCmdData[i].pData;

                //Key
                u8Tmp[u16DataLen] = pCmdDataSet->stCmdData[i].u8KeyId;
                u16DataLen += 1;
                
                //Key header
                u16ValueLen = 1;
                u8Tmp[u16DataLen] = (XRSBLE_U8)((u16ValueLen >> 8) & 0x01);
                u16DataLen += 1;
                u8Tmp[u16DataLen] = (XRSBLE_U8)(u16ValueLen & 0xFF);
                u16DataLen += 1;
                
                //Value
                u8Tmp[u16DataLen] = pProtocVerInfo->u8BleProtocVer;
                u16DataLen += u16ValueLen;
                
                break;
            }
            case KEY_ID_IS_5G_WIFI_REQ:
            {
                //Key
                u8Tmp[u16DataLen] = pCmdDataSet->stCmdData[i].u8KeyId;
                u16DataLen += 1;
                
                //Key header
                u16ValueLen = 0;
                u8Tmp[u16DataLen] = (XRSBLE_U8)((u16ValueLen >> 8) & 0x01);
                u16DataLen += 1;
                u8Tmp[u16DataLen] = (XRSBLE_U8)(u16ValueLen & 0xFF);
                u16DataLen += 1;
                u16DataLen += u16ValueLen;

                break;
            }
            case KEY_ID_IS_5G_WIFI_REPLY:
            {
                XRSBLE_Is5GWifi *pIs5GWifi = NULL;

                if (NULL == pCmdDataSet->stCmdData[i].pData)
                {
                    //参数非法
                    L2_ERR("Interface param illegal: pCmdData->pData=%p!!!\n",
                        pCmdDataSet->stCmdData[i].pData);
                    break;
                }
                if (pCmdDataSet->stCmdData[i].u16DataLen < sizeof(XRSBLE_Is5GWifi))
                {
                    //缓冲区不匹配
                    L2_ERR("Datalen not match: u16DataLen=%d sizeof(struct)=%d!!!\n",
                        pCmdDataSet->stCmdData[i].u16DataLen, sizeof(XRSBLE_Is5GWifi));
                    break;
                }

                pIs5GWifi = (XRSBLE_Is5GWifi *)pCmdDataSet->stCmdData[i].pData;

                //Key
                u8Tmp[u16DataLen] = pCmdDataSet->stCmdData[i].u8KeyId;
                u16DataLen += 1;
                
                //Key header
                u16ValueLen = 1;
                u8Tmp[u16DataLen] = (XRSBLE_U8)((u16ValueLen >> 8) & 0x01);
                u16DataLen += 1;
                u8Tmp[u16DataLen] = (XRSBLE_U8)(u16ValueLen & 0xFF);
                u16DataLen += 1;
                
                //Value
                u8Tmp[u16DataLen] = pIs5GWifi->u8BleIs5GWifi;
                u16DataLen += u16ValueLen;
                
                break;
            }
            default:
            {
                L2_ERR("KEY_ID unsupport: %d!!!\n", pCmdDataSet->stCmdData[i].u8KeyId);
                return -2;
                break;
            }
        }
    }

    if (u16DataLen > u16BufLen)
    {
        //转化后数据长度超出缓冲区大小
        L2_ERR("Data too long: DataLen=%d BufLen=%d!!!\n",
            u16DataLen, u16BufLen);
        return -4;
    }
    else
    {
        memcpy(pBuff, u8Tmp, u16DataLen);
        *pBufLen = u16DataLen;
    }
    
    return 0;
}

//将命令数据转化为L2层协议格式
//pBufLen：传入缓冲区大小，传出转化后数据长度
static int CmdDataTransform(XRSBLE_L2CmdDataSet *pCmdDataSet, XRSBLE_U8 *pBuff, XRSBLE_U16 *pBufLen)
{
    int nRet = 0;
    XRSBLE_U16 u16BufLen = 0;
    XRSBLE_U16 u16BufLen2 = 0;
    XRSBLE_U8 u8Tmp[L2_PAYLOAD_LENTH_MAX] = {0};
    XRSBLE_U8 u8Tmp2[L2_PAYLOAD_LENTH_MAX] = {0};
    XRSBLE_U16 u16DataLen = 0;
    
    if (NULL == pCmdDataSet
        || NULL == pBuff
        || NULL == pBufLen)
    {
        //参数非法
        L2_ERR("Interface param illegal!!!\n");
        return -1;
    }

    u16BufLen = *pBufLen;
    *pBufLen = 0;
    u16DataLen = 0;
    
    //构建L2层帧头(这里建议优化到CmdDataTransform接口内)
    //Command ID
    u8Tmp[0] = pCmdDataSet->stCmdData[0].u8CmdId;
    //reserve
    u8Tmp[1] = 0;
    u16DataLen += L2_HEAD_LENTH;

    u16BufLen2 = u16BufLen - u16DataLen;
    switch (pCmdDataSet->stCmdData[0].u8CmdId)
    {
        case CMD_ID_SET_INFO:
        {
            nRet = CmdDataTransform_SetInfo(pCmdDataSet, u8Tmp2, &u16BufLen2);
            if (0 != nRet)
            {
                //数据转化失败
                L2_ERR("CmdDataTransform_SetInfo failed: nRet=%d!!!\n", nRet);
                return -3;
            }
            
            break;
        }
        case CMD_ID_GET_INFO:
        {
            nRet = CmdDataTransform_GetInfo(pCmdDataSet, u8Tmp2, &u16BufLen2);
            if (0 != nRet)
            {
                //数据转化失败
                L2_ERR("CmdDataTransform_GetInfo failed: nRet=%d!!!\n", nRet);
                return -3;
            }
            
            break;
        }
        default:
        {
            L2_ERR("CMD_ID unsupport: %d!!!\n", pCmdDataSet->stCmdData[0].u8CmdId);
            return -2;
            break;
        }
    }
                
    if ((u16DataLen + u16BufLen2) > u16BufLen)
    {
        //转化后数据长度超出缓冲区大小
        L2_ERR("Data too long: DataLen=%d BufLen=%d!!!\n",
            (u16DataLen + u16BufLen2), u16BufLen);
        return -4;
    }
    else
    {
        memcpy(pBuff, u8Tmp, u16DataLen);
        *pBufLen += u16DataLen;
        memcpy(pBuff+u16DataLen, u8Tmp2, u16BufLen2);
        *pBufLen += u16BufLen2;
    }
    
    return 0;
}

//将命令操作返回值转化为协议数据
//pBufLen：传入缓冲区大小，传出转化后数据长度
static int CmdRetTransform(XRSBLE_L2CmdDataSet *pCmdDataSet, XRSBLE_U8 *pBuff, XRSBLE_U16 *pBufLen)
{
    int i = 0;
    XRSBLE_U16 u16BufLen = 0;
    XRSBLE_U16 u16ValueLen = 0;
    XRSBLE_U8 u8Tmp[L2_PAYLOAD_LENTH_MAX] = {0};
    XRSBLE_U16 u16DataLen = 0;
    XRSBLE_S32 *ps32Tmp = 0;
    XRSBLE_S32 *ps32Tmp2 = 0;
    
    if (NULL == pCmdDataSet
        || NULL == pBuff
        || NULL == pBufLen)
    {
        //参数非法
        L2_ERR("Interface param illegal!!!\n");
        return -1;
    }

    u16BufLen = *pBufLen;
    *pBufLen = 0;
    u16DataLen = 0;

    //构建L2层帧头
    //Command ID
    u8Tmp[0] = pCmdDataSet->stCmdData[0].u8CmdId;
    //reserve
    u8Tmp[1] = 0;
    u16DataLen += L2_HEAD_LENTH;

    for (i = 0; i < pCmdDataSet->u8CmdCnt; i++)
    {
        //Key
        u8Tmp[u16DataLen] = pCmdDataSet->stCmdData[i].u8KeyId;
        u16DataLen += 1;
        
        //Key header
        u16ValueLen = pCmdDataSet->stCmdData[i].u16DataLen;
        u8Tmp[u16DataLen] = (XRSBLE_U8)((u16ValueLen >> 8) & 0x01);
        u16DataLen += 1;
        u8Tmp[u16DataLen] = (XRSBLE_U8)(u16ValueLen & 0xFF);
        u16DataLen += 1;
        
        //Value
        ps32Tmp2 = (XRSBLE_S32 *)(pCmdDataSet->stCmdData[i].pData);
        ps32Tmp = (XRSBLE_S32 *)(&u8Tmp[u16DataLen]);
        *ps32Tmp = htonl(*ps32Tmp2);
        u16DataLen += u16ValueLen;

        L2_DBG("i=%d u8CmdId=%u u8KeyId=%u ps32Tmp2=%d ps32Tmp2=%d\n",
            i, pCmdDataSet->stCmdData[0].u8CmdId,
            pCmdDataSet->stCmdData[i].u8KeyId,
            *ps32Tmp2, *ps32Tmp);
    }    

    if (u16DataLen > u16BufLen)
    {
        //转化后数据长度超出缓冲区大小
        L2_ERR("Data too long: DataLen=%d BufLen=%d!!!\n",
            u16DataLen, u16BufLen);
        return -2;
    }
    else
    {
        memcpy(pBuff, u8Tmp, u16DataLen);
        *pBufLen = u16DataLen;
    }
    
    return 0;
}

//将L2协议数据转化为命令数据并回调给应用层
//pCmdDataSet：传入主命令字/数据类型/操作返回值缓冲区，传出主子命令字&各操作操作返回值
static int ProtocolDataTransform_SetInfo(XRSBLE_L2CmdDataSet *pCmdDataSet, XRSBLE_U8 *pData, XRSBLE_U16 u16DataLen)
{
    int i = 0;
    XRSBLE_U16 u16ValueLen = 0;
    XRSBLE_S16 *ps16Tmp = NULL;
    XRSBLE_U16 u16DealLen = 0;
    XRSBLE_L2CmdData stL2CmdData = {0};
    XRSBLE_S32 *ps32Tmp = 0;
    XRSBLE_S32 s32Ret = 0;
    char bIsParseSucc = 0;
    //注意此块内存大小，需要可容纳下面使用时的各种结构体大小
    XRSBLE_U8 u8BuffTmp[2048] = {0};
    
    if (NULL == pCmdDataSet
        || NULL == pData
        || u16DataLen < (KEY_HEAD_LENTH+1))
    {
        //参数非法
        L2_ERR("Interface param illegal!!!\n");
        return -1;
    }
    // printf("sjz ----- ProtocolDataTransform_SetInfo\n");
    //填充CmdId/数据类型（普通/应答）
    stL2CmdData.u16SeqId = pCmdDataSet->stCmdData[0].u16SeqId;
    stL2CmdData.u8CmdId = pCmdDataSet->stCmdData[0].u8CmdId;
    stL2CmdData.u8DateType = pCmdDataSet->stCmdData[0].u8DateType;
    L2_DBG("sjz ----- SeqId = %d CmdId = %d DateType = %d \n",stL2CmdData.u16SeqId, stL2CmdData.u8CmdId, stL2CmdData.u8DateType);
    //解析Key及value
    u16DealLen = 0;
    for (i = 0; i < SINGLE_SEND_CMD_CNT_MAX; i++)
    {
        L2_DBG("sjz ----- u16DataLen = %d u16DealLen = %d \n",u16DataLen, u16DealLen);
        if ((u16DataLen - u16DealLen) < (KEY_HEAD_LENTH + 1))
        {
            //剩余数据小于最小解析长度，则表示已解析完毕
            break;
        }

        //解析key
        stL2CmdData.u8KeyId = pData[u16DealLen];
        u16DealLen += 1;
        //解析valueLen
        u16ValueLen = ((pData[u16DealLen]&0x1) << 8) | (pData[u16DealLen+1]&0xFF);
        u16DealLen += 2;
        L2_DBG("sjz ----- u8KeyId = %d u16ValueLen = %d \n",stL2CmdData.u8KeyId, u16DealLen);
        switch (stL2CmdData.u8KeyId)
        {
            case KEY_ID_WIFI_SSID:
            {
                XRSBLE_WifiSsidInfo *pSsidInfo = (XRSBLE_WifiSsidInfo *)u8BuffTmp;
                
                //解析Value
                memset(pSsidInfo->szWifiSsidBase64, 0, sizeof(pSsidInfo->szWifiSsidBase64));
                memcpy(pSsidInfo->szWifiSsidBase64, (&pData[u16DealLen]), u16ValueLen);
                u16DealLen += u16ValueLen;
                
                //构建命令结构
                stL2CmdData.u16DataLen = sizeof(XRSBLE_WifiSsidInfo);
                stL2CmdData.pData = (void *)pSsidInfo;
                bIsParseSucc = 1;                
                break;
            }
            case KEY_ID_WIFI_PWD:
            {
                XRSBLE_WifiPwdInfo *pPwdInfo = (XRSBLE_WifiPwdInfo *)u8BuffTmp;

                //解析Value
                memset(pPwdInfo->szWifiPwdBase64, 0, sizeof(pPwdInfo->szWifiPwdBase64));
                memcpy(pPwdInfo->szWifiPwdBase64, (&pData[u16DealLen]), u16ValueLen);
                u16DealLen += u16ValueLen;

                //构建命令结构
                stL2CmdData.u16DataLen = sizeof(XRSBLE_WifiPwdInfo);
                stL2CmdData.pData = (void *)pPwdInfo;

                bIsParseSucc = 1;
                break;
            }
            case KEY_ID_TIMEZONE:
            {
                XRSBLE_TimeZoneInfo *pTimeZoneInfo = (XRSBLE_TimeZoneInfo *)u8BuffTmp;

                //解析Value
                ps16Tmp = (XRSBLE_S16 *)(&pData[u16DealLen]);
                pTimeZoneInfo->s16TimeZoneMin = ntohs(*ps16Tmp);
                u16DealLen += u16ValueLen;

                //构建命令结构
                stL2CmdData.u16DataLen = sizeof(XRSBLE_TimeZoneInfo);
                stL2CmdData.pData = (void *)pTimeZoneInfo;

                bIsParseSucc = 1;                
                break;
            }
            default:
            {
                L2_ERR("KEY_ID unsupport: %d!!!\n", stL2CmdData.u8KeyId);
                //跳过此无效命令
                u16DealLen += u16ValueLen;
                
                bIsParseSucc = 0;
                break;
            }
        }

        s32Ret = -1;
        if (bIsParseSucc)
        {
            //执行操作
            if (NULL != s_pAppRecvProcessCallback)
            {
                s32Ret = s_pAppRecvProcessCallback(&stL2CmdData);
            }
        }
        //填充返回值
        pCmdDataSet->stCmdData[i].u8KeyId = stL2CmdData.u8KeyId;
        ps32Tmp = (XRSBLE_S32 *)pCmdDataSet->stCmdData[i].pData;
        *ps32Tmp = s32Ret;
        pCmdDataSet->u8CmdCnt++;
    }
    
    return 0;
}

//将L2协议数据转化为命令数据并回调给应用层
//pCmdData：传入主命令字/数据类型/操作返回值缓冲区，传出主子命令字&各操作操作返回值
static int ProtocolDataTransform_GetInfo(XRSBLE_L2CmdDataSet *pCmdDataSet, XRSBLE_U8 *pData, XRSBLE_U16 u16DataLen)
{
    int i = 0;
    XRSBLE_U16 u16ValueLen = 0;
    XRSBLE_U16 u16DealLen = 0;
    XRSBLE_L2CmdData stL2CmdData = {0};
    XRSBLE_S32 *ps32Tmp = 0;
    XRSBLE_S32 s32Ret = 0;
    char bIsParseSucc = 0;
    XRSBLE_U8 u8BuffTmp[2048] = {0};
    
    if (NULL == pCmdDataSet
        || NULL == pData
        || u16DataLen < (KEY_HEAD_LENTH+1))
    {
        //参数非法
        L2_ERR("Interface param illegal!!!\n");
        return -1;
    }

    //填充CmdId/数据类型（普通/应答）
    stL2CmdData.u16SeqId = pCmdDataSet->stCmdData[0].u16SeqId;
    stL2CmdData.u8CmdId = pCmdDataSet->stCmdData[0].u8CmdId;
    stL2CmdData.u8DateType = pCmdDataSet->stCmdData[0].u8DateType;
    
    //解析Key及value
    u16DealLen = 0;
    for (i = 0; i < SINGLE_SEND_CMD_CNT_MAX; i++)
    {
        if ((u16DataLen - u16DealLen) < (KEY_HEAD_LENTH + 1))
        {
            //剩余数据小于最小解析长度，则表示已解析完毕
            break;
        }

        //解析key
        stL2CmdData.u8KeyId = pData[u16DealLen];
        u16DealLen += 1;
        //解析valueLen
        u16ValueLen = ((pData[u16DealLen]&0x1) << 8) | (pData[u16DealLen+1]&0xFF);
        u16DealLen += 2;

        switch (stL2CmdData.u8KeyId)
        {
            case KEY_ID_PROTOC_VER_REQ:
            {                
                //构建命令结构
                stL2CmdData.u16DataLen = 0;
                stL2CmdData.pData = NULL;

                bIsParseSucc = 1;
                break;
            }
            case KEY_ID_PROTOC_VER_REPLY:
            {
                XRSBLE_ProtocVerInfo *pProtocVerInfo = (XRSBLE_ProtocVerInfo *)u8BuffTmp;

                //解析Value
                pProtocVerInfo->u8BleProtocVer = pData[u16DealLen];
                u16DealLen += u16ValueLen;

                //构建命令结构
                stL2CmdData.u16DataLen = sizeof(XRSBLE_ProtocVerInfo);
                stL2CmdData.pData = (void *)pProtocVerInfo;

                bIsParseSucc = 1;
                break;
            }
            case KEY_ID_IS_5G_WIFI_REQ:
            {
                //构建命令结构
                stL2CmdData.u16DataLen = 0;
                stL2CmdData.pData = NULL;

                bIsParseSucc = 1;   
                break;
            }
            case KEY_ID_IS_5G_WIFI_REPLY:
            {
                XRSBLE_Is5GWifi *pIs5GWifi = (XRSBLE_Is5GWifi *)u8BuffTmp;

                //解析Value
                pIs5GWifi->u8BleIs5GWifi = pData[u16DealLen];
                u16DealLen += u16ValueLen;

                //构建命令结构
                stL2CmdData.u16DataLen = sizeof(XRSBLE_Is5GWifi);
                stL2CmdData.pData = (void *)pIs5GWifi;

                bIsParseSucc = 1;
                break;
            }
            default:
            {
                L2_ERR("KEY_ID unsupport: %d!!!\n", stL2CmdData.u8KeyId);
                //跳过此无效命令
                u16DealLen += u16ValueLen;

                bIsParseSucc = 0;
                break;
            }
        }

        s32Ret = -1;
        if (bIsParseSucc)
        {
            //执行操作
            if (NULL != s_pAppRecvProcessCallback)
            {
                s32Ret = s_pAppRecvProcessCallback(&stL2CmdData);
            }
        }
        //填充返回值
        pCmdDataSet->stCmdData[i].u8KeyId = stL2CmdData.u8KeyId;
        ps32Tmp = (XRSBLE_S32 *)pCmdDataSet->stCmdData[i].pData;
        *ps32Tmp = s32Ret;
        pCmdDataSet->u8CmdCnt++;
    }
    
    return 0;
}

//将L2协议数据转化为命令数据并回调给应用层
//pCmdDataSet：传出主子命令字&各操作操作返回值
static int ProtocolDataTransform(XRSBLE_L2CmdDataSet *pCmdDataSet, XRSBLE_U8 *pData, XRSBLE_U16 u16DataLen, XRSBLE_U16 u16SeqId)
{
    int i = 0;
    int nRet = 0;
    XRSBLE_U8 u8CmdId = 0;
    XRSBLE_U8 u8Reserve = 0;
    XRSBLE_U16 u16DealLen = 0;
    
    if (NULL == pCmdDataSet
        || NULL == pData
        || u16DataLen < (L2_HEAD_LENTH+KEY_HEAD_LENTH+1))
    {
        //参数非法
        L2_ERR("Interface param illegal!!!\n");
        return -1;
    }

    //解析CmdId
    u8CmdId = pData[0];
    u8Reserve = pData[1];
    u16DealLen += 2;
    L2_DBG(" ---- cmdid = %d reserve = %d len = %d\n",u8CmdId,u8Reserve,u16DealLen);
    pCmdDataSet->u8CmdCnt = 0;
    for (i = 0; i < SINGLE_SEND_CMD_CNT_MAX; i++)
    {
        pCmdDataSet->stCmdData[i].u16SeqId = u16SeqId;
        pCmdDataSet->stCmdData[i].u8CmdId = u8CmdId;
        pCmdDataSet->stCmdData[i].u8DateType = 0;
    }
    
    switch (u8CmdId)
    {
        case CMD_ID_SET_INFO:
        {
            nRet = ProtocolDataTransform_SetInfo(pCmdDataSet, (pData+u16DealLen), (u16DataLen-u16DealLen));
            if (0 != nRet)
            {
                //数据转化失败
                L2_ERR("ProtocolDataTransform_SetInfo failed: nRet=%d!!!\n", nRet);
                return -3;
            }
            
            break;
        }
        case CMD_ID_GET_INFO:
        {
            nRet = ProtocolDataTransform_GetInfo(pCmdDataSet, (pData+u16DealLen), (u16DataLen-u16DealLen));
            if (0 != nRet)
            {
                //数据转化失败
                L2_ERR("ProtocolDataTransform_GetInfo failed: nRet=%d!!!\n", nRet);
                return -3;
            }
            
            break;
        }
        default:
        {
            L2_ERR("CMD_ID unsupport: %d!!!\n", u8CmdId);
            return -2;
            break;
        }
    }
    
    return 0;
}

//将L2协议数据转化为命令数据并回调给应用层(应答数据)
//pCmdData：传入主命令字，传出主子命令字&各操作操作返回值
static int ProtocolDataTransformForAck(XRSBLE_U8 *pData, XRSBLE_U16 u16DataLen, XRSBLE_U16 u16SeqId)
{
    int i = 0;
    XRSBLE_U16 u16ValueLen = 0;
    XRSBLE_U16 u16DealLen = 0;
    XRSBLE_U8 u8CmdId = 0;
    XRSBLE_U8 u8Reserve = 0;
    XRSBLE_L2CmdData stL2CmdData = {0};
    XRSBLE_S32 *ps32Tmp = 0;
    XRSBLE_S32 s32Ret = 0;

	u8Reserve = u8Reserve;
	
    if (NULL == pData
        || u16DataLen < (L2_HEAD_LENTH+KEY_HEAD_LENTH+1))
    {
        //参数非法
        L2_ERR("Interface param illegal!!!\n");
        return -1;
    }

    //解析CmdId
    u8CmdId = pData[0];
    u8Reserve = pData[1];
    u16DealLen += 2;
    
    stL2CmdData.u8CmdId = u8CmdId;
    stL2CmdData.u8DateType = 1;
    stL2CmdData.u16SeqId = u16SeqId;    
    
    //解析Key及value
    for (i = 0; i < SINGLE_SEND_CMD_CNT_MAX; i++)
    {
        if ((u16DataLen - u16DealLen) < (KEY_HEAD_LENTH + 1))
        {
            //剩余数据小于最小解析长度，则表示已解析完毕
            break;
        }

        //解析key
        stL2CmdData.u8KeyId = pData[u16DealLen];
        u16DealLen += 1;
        
        //解析valueLen
        u16ValueLen = ((pData[u16DealLen]&0x1) << 8) | (pData[u16DealLen+1]&0xFF);
        u16DealLen += 2;
        
        //解析Value
        ps32Tmp = (XRSBLE_S32 *)(&pData[u16DealLen]);
        s32Ret = ntohl(*ps32Tmp);
        u16DealLen += u16ValueLen;

        //构建命令结构
        stL2CmdData.u16DataLen = sizeof(s32Ret);
        stL2CmdData.pData = (void *)(&s32Ret);

        //执行操作
        if (NULL != s_pAppRecvProcessCallback)
        {
            s_pAppRecvProcessCallback(&stL2CmdData);
        }
    }
    
    return 0;
}

/*!
 *  功  能: XRSBLE协议L2层初始化（其内部会维护对L1层的初始化）
 *  param1: L2层初始化参数
 *  retval: 0-成功，其他-错误码
 *  说  明: 重复调用，直接返回成功
 */
XRSBLE_S32 XRSBLE_L2_Init(XRSBLE_IN XRSBLE_L2InitParam *pParam)
{
    XRSBLE_S32 s32Ret = 0;
    XRSBLE_L1InitParam stL1InitParam = {0};

    if (NULL == pParam)
    {
        //参数非法
        L2_ERR("Interface param illegal!!!\n");
        return -1;
    }

    pthread_mutex_lock(&s_muL2IntfLock);
    
    if (s_bIsL2Inited)
    {
        pthread_mutex_unlock(&s_muL2IntfLock);
        return 0;
    }

    //记录应用层数据接收&处理接口
    s_pAppRecvProcessCallback = pParam->pAppRecvProcessCallback;

    //初始化L1层
    stL1InitParam.pBleSendDataCallback = pParam->pBleSendDataCallback;
    stL1InitParam.pL2RecvProcessCallback = XRSBLE_L2_RecvProcess;
    s32Ret = XRSBLE_L1_Init(&stL1InitParam);
    if (0 == s32Ret)
    {
        s_bIsL2Inited = 1;
    }
    else
    {
        //L1层初始化失败
        L2_ERR("XRSBLE_L1_Init failed: s32Ret=%d!!!\n", s32Ret);
        s_pAppRecvProcessCallback = NULL;

        pthread_mutex_unlock(&s_muL2IntfLock);
        return -2;
    }

    pthread_mutex_unlock(&s_muL2IntfLock);
    return 0;
}

/*!
 *  功  能: XRSBLE协议L2层逆初始化（其内部会维护对L1层的逆初始化）
 *  retval: 0-成功，其他-错误码
 *  说  明: 重复调用，直接返回成功
 */
XRSBLE_S32 XRSBLE_L2_UnInit()
{
    XRSBLE_S32 s32Ret = 0;

    pthread_mutex_lock(&s_muL2IntfLock);
    
    if (0 == s_bIsL2Inited)
    {
        pthread_mutex_unlock(&s_muL2IntfLock);
        return 0;
    }

    //逆初始化L1层
    s32Ret = XRSBLE_L1_UnInit();
    if (0 == s32Ret)
    {
        s_bIsL2Inited = 0;
        //应用层数据接收&处理接口置空
        s_pAppRecvProcessCallback = NULL;
    }
    else
    {
        //L1层反初始化失败
        L2_ERR("XRSBLE_L1_UnInit failed: s32Ret=%d!!!\n", s32Ret);

        pthread_mutex_unlock(&s_muL2IntfLock);
        return -1;
    }

    pthread_mutex_unlock(&s_muL2IntfLock);
    return 0;
}

/*!
 *  功  能: XRSBLE协议版本号获取
 *  param1: 用于返回XRSBLE协议版本号
 *  retval: 0-成功，其他-错误码
 */
XRSBLE_S32 XRSBLE_ProtocolVerGet(XRSBLE_IN XRSBLE_U8 *pBleProtocVer)
{
    XRSBLE_S32 s32Ret = 0;

    if (NULL == pBleProtocVer)
    {
        L2_ERR("Interface param illegal:pBleProtocVer=%p!!!\n",
            pBleProtocVer);
        return -1;
    }

    //根据版本宏，返回
    *pBleProtocVer = (XRSBLE_U8)XRSBLE_PROTOCOL_VERSION;
    
    return s32Ret;
}

/*!
 *  功  能: XRSBLE协议L2层发送数据
 *  param1: 指向要发送的数据的指针（应用层数据）
 *  param2: 数据发送控制参数
 *  param3: 返回此次发送数据的SequenceId，供后续接收应答数据时寻找对应关系
 *  retval: 0-成功，其他-错误码
 *  说  明: 内部采用异步处理，会将待发送数据先存入队列
 */
XRSBLE_S32 XRSBLE_L2_SendData(XRSBLE_IN XRSBLE_L2CmdDataSet *pCmdDataSet, XRSBLE_IN XRSBLE_SendCtrlParam *pSendCtlParam, XRSBLE_OUT XRSBLE_U16 *pSeqId)
{
    XRSBLE_S32 s32Ret = 0;
    XRSBLE_U32 u32DataLen = 0;
    XRSBLE_U8 u8Buf[L2_FRAME_LENTH_MAX*2] = {0};
    XRSBLE_U16 u16BufLen = 0;

    if (NULL == pCmdDataSet
        || NULL == pSendCtlParam
        || NULL == pSeqId)
    {
        //参数非法
        L2_ERR("Interface param illegal:pCmdDataSet=%p pSendCtlParam=%p pSeqId=%p!!!\n",
            pCmdDataSet, pSendCtlParam, pSeqId);
        return -1;
    }

    pthread_mutex_lock(&s_muL2IntfLock);
    
    if (0 == s_bIsL2Inited)
    {
        //未完成初始化
        L2_ERR("XRSBLE_L2 not init!!!\n");

        pthread_mutex_unlock(&s_muL2IntfLock);
        return -2;
    }

    //按L2层协议转化操作命令
    u16BufLen = sizeof(u8Buf);
    s32Ret = CmdDataTransform(pCmdDataSet, u8Buf, &u16BufLen);
    if (0 != s32Ret)
    {
        //待发送数据长度不合法
        L2_ERR("CmdDataTransform failed: s32Ret=%d!!!\n", s32Ret);
        u16BufLen = 0;
    }
    u32DataLen += u16BufLen;
    
    if (u32DataLen > L2_FRAME_LENTH_MAX
        || u32DataLen < (L2_HEAD_LENTH+KEY_HEAD_LENTH+1))
    {
        //待发送数据长度不合法
        L2_ERR("Send Data lenth illegal: u32DataLen=%d maxSup=%d minSup=%d!!!\n", 
            u32DataLen, L2_FRAME_LENTH_MAX, (L2_HEAD_LENTH+KEY_HEAD_LENTH+1));

        pthread_mutex_unlock(&s_muL2IntfLock);
        return -3;
    }

    //将待发送数据及发送控制参数传递给L1层发送接口
    //同时得到此次发送数据的SequenceId
    s32Ret = XRSBLE_L1_SendData(u8Buf, u32DataLen, pSendCtlParam, pSeqId);
    if (0 != s32Ret)
    {
        //数据发送失败
        L2_ERR("XRSBLE_L1_SendData failed: s32Ret=%d!!!\n", s32Ret);

        pthread_mutex_unlock(&s_muL2IntfLock);
        return -4;
    }    

    pthread_mutex_unlock(&s_muL2IntfLock);
    return 0;
}

/*!
 *  功  能: XRSBLE协议L2层接收&处理数据
 *  param1: 指向接收数据（L1层处理完成后的数据，协议内部使用，包括普通数据/应答数据）
 *  param2: 接收数据长度
 *  param3: 此条数据解析出的SequenceId
 *  param4: 用于接收L2层处理完成后的应答数据(对于普通数据填缓冲区地址，对于应答数据填NULL)
 *  param5: IN：应答数据缓冲区大小 OUT：应答数据长度
 *  retval: 0-成功，其他-错误码
 *  说  明: 其内部解析出操作命令后，调用应用层数据接收回调，并根据各操作返回值，组成L2层应答数据，返回给L1层
 *  注意: 需避免长时间阻塞
 */
static XRSBLE_S32 XRSBLE_L2_RecvProcess(XRSBLE_IN XRSBLE_U8 *pRecvdata, XRSBLE_IN XRSBLE_U32 u32RecvDataLen, XRSBLE_IN XRSBLE_U16 u16SeqId, XRSBLE_OUT XRSBLE_U8 *u8AckDataBuf, XRSBLE_INOUT XRSBLE_U32 *pDataLen)
{
    XRSBLE_S32 s32Ret = 0;
    XRSBLE_L2CmdDataSet stCmdDataSet = {0};
    int i = 0;
    XRSBLE_S32 s32RetSet[SINGLE_SEND_CMD_CNT_MAX] = {0};

    if (NULL == pRecvdata)
    {
        //参数非法
        L2_ERR("Interface param illegal:pRecvdata=%p!!!\n",
            pRecvdata);
        return -1;
    }

    if (0 == u32RecvDataLen
        || u32RecvDataLen < (L2_HEAD_LENTH+KEY_HEAD_LENTH+1))
    {
        //数据长度非法
        L2_ERR("Data lenth illegal:u32RecvDataLen=%d!!!\n", u32RecvDataLen);
        return -2;
    }
    // printf("sjz ---- XRSBLE_L2_RecvProcess\n");
    //pthread_mutex_lock(&s_muL2IntfLock);
    
    if (0 == s_bIsL2Inited)
    {
        //未完成初始化
        L2_ERR("XRSBLE_L2 not init!!!\n");

        //pthread_mutex_unlock(&s_muL2IntfLock);
        return -3;
    }
    // printf("sjz ---- %#x  %d\n",u8AckDataBuf,pDataLen);
    if (NULL != u8AckDataBuf
        && NULL != pDataLen)
    {
        //用于接收存储各命令操作返回值
        for (i = 0; i < SINGLE_SEND_CMD_CNT_MAX; i++)
        {
            stCmdDataSet.stCmdData[i].u16DataLen = sizeof(s32RetSet[i]);
            stCmdDataSet.stCmdData[i].pData = (void *)(&s32RetSet[i]);
        }
        
        //对于普通数据（请求数据），解析L2层数据转化为命令数据，并回调应用层，返回各命令操作返回值
        s32Ret = ProtocolDataTransform(&stCmdDataSet, pRecvdata, (XRSBLE_U16)u32RecvDataLen, u16SeqId);
        if (0 != s32Ret)
        {
            //数据解析失败
            L2_ERR("ProtocolDataTransform failed: s32Ret=%d!!!\n", s32Ret);

            //pthread_mutex_unlock(&s_muL2IntfLock);
            return -4;
        }
        
        //根据应用层各操作返回值，组成应答数据，返回给L1
        s32Ret = CmdRetTransform(&stCmdDataSet, u8AckDataBuf, (XRSBLE_U16 *)pDataLen);
        if (0 != s32Ret)
        {
            //应答数据构建失败失败
            *pDataLen = 0;
            L2_ERR("CmdRetTransform failed: s32Ret=%d!!!\n", s32Ret);

            //pthread_mutex_unlock(&s_muL2IntfLock);
            return -5;
        }
    }
    else
    {
        //对于应答数据，解析L2层数据转化为命令及对应操作返回值，回调给应用层
        s32Ret = ProtocolDataTransformForAck(pRecvdata, (XRSBLE_U16)u32RecvDataLen, u16SeqId);
        if (0 != s32Ret)
        {
            //数据解析失败
            L2_ERR("ProtocolDataTransform failed: s32Ret=%d!!!\n", s32Ret);

            //pthread_mutex_unlock(&s_muL2IntfLock);
            return -4;
        }
    }

    //pthread_mutex_unlock(&s_muL2IntfLock);
    return 0;
}

