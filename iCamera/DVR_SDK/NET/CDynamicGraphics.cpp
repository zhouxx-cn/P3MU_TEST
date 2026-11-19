#include "CDynamicGraphics.h"
#include "common_pub.h"
#include "MPU_SYS_API.h"
#include "MPU_Net_API.h"
#include <cstring>
#include <stdio.h>
#include <cstdlib>
#include <sys/time.h>
#include <string>
#include "NET_global.h"
#if defined(__IS_SUPPORT_DYNAMIC_GRAPHICS__)

#define DEFAULT_IMAGE_LENGTH  64 * 1024; // 默认图片缓冲区大小（64KB）

CDynamicGraphics::CDynamicGraphics()
{
    m_RetransmissionMode = 0; //重传模式 0 协议控制重传，1 中间层控制重传。中间层控制重传时，由中间层保存数据，协议控制重传时，由协议保存数据，缓冲区满了之后正常淘汰，没满之前一直重试上报事件
    m_isSupportDynamicGraphics = 1; //是否支持动图（不启用中间层重传才生效），0:不支持，1：支持
    m_isSupportPrerecorded = 1; //是否支持预录动图（支持动图才生效），0:不支持，1：支持
    m_ImageLength = DEFAULT_IMAGE_LENGTH; //抓图图片 buffer 的最大长度（默认大小为 64K）
    m_EventID = 0; //事件ID
    m_bInit = false; //是否初始化
}

CDynamicGraphics::~CDynamicGraphics() {}

// 生成UUID（基于设备DID、MAC、时间、随机数及事件ID）
int CreateUUID(const char* strDID, const char* strMAC, unsigned long long nEventTime, 
               int nEventID, std::string& strUUID) { 
    char strBuf[128] = {0}; 
    char strTime[16] = {0}; 
    char strRandom[16] = {0}; 
    char strEventID[32] = {0}; 
	int DIDLen = strlen(strDID);
	int MACLen = strlen(strMAC);
    // 过滤DID中的'-'
    char strDIDTmp[32] = {0}; 
    int j = 0; 
    for (int i = 0; i < DIDLen; i++) { 
		if (strDID[i] == ',') break;
        if (strDID[i] != '-') strDIDTmp[j++] = strDID[i]; 
    } 
    // 过滤MAC中的':'
    char strMACTmp[32] = {0}; 
    j = 0; 
    for (int i = 0; i < MACLen; i++) { 
        if (strMAC[i] != ':') strMACTmp[j++] = strMAC[i]; 
    } 
    // 填充时间、随机数及事件ID
    snprintf(strTime, 16, "%llu", nEventTime); 
    snprintf(strRandom, 16, "%d", rand()); 
    snprintf(strEventID, 16, "%d", nEventID); 

    // 组合UUID
    snprintf(strBuf, 128, "%s%s%s%s%s", strDIDTmp, strMACTmp, strTime, strRandom, strEventID); 
    strUUID = strBuf; 
    return 0; 
} 

//支持重传才需要实现，暂实现为空
int SendEventStatusCallback(ZWP2P_PUSH_P6SMSG_EVENT_STATUS sendEventStatus)
{
    return 0;
}

//支持重传才需要实现，暂实现为空
int PackageStatusCallback(ZWP2P_PUSH_P6SMSG_PACKSGE_STATUS* packageStatusParam)
{
    return 0;
}

int ImageCaptureCallback(ZWP2P_PUSH_P6SMSG_IMAGE_CAPTURE_PARAM* imageCaptureParam)
{
    if(imageCaptureParam == NULL)
    {
        return -1;
    }
    int nRet = 0;
    int nFileSize = 0;
    char* pPicBuffer = NULL;

    NET_DBG("GetMjpegBuffer Start, ret=%d\n", nRet);
    nRet = GetMjpegBuffer_V2(1,0, &pPicBuffer, (int *)&nFileSize, MAX_PICTURE_BUF_SIZE, NULL);
    if (nRet)
    {
        NET_ERR("GetMjpegBuffer Failed, ret=%d\n", nRet);
        if(pPicBuffer)
        {
            free(pPicBuffer);
        }
        return nRet;
    }
    if(nFileSize > imageCaptureParam->nMaxBufferSize)
    {
        NET_DBG("GetMjpegBuffer Len=%d over max len\n", nFileSize);
        if(pPicBuffer)
        {
            free(pPicBuffer);
        }
        return -1;
    }
    NET_DBG("GetMjpegBuffer Success, Len=%d\n", nFileSize);

    memcpy(imageCaptureParam->buffer,pPicBuffer,nFileSize);
    imageCaptureParam->nPicLen = nFileSize;

    if(pPicBuffer)
    {
        free(pPicBuffer);
    }
    return 0;
}

void CDynamicGraphics::Init()
{
    //设置动态图参数
    ZWP2P_PUSH_P6SMSG_PARAM param;
    memset(&param, 0, sizeof(ZWP2P_PUSH_P6SMSG_PARAM));
    param.nRetransmissionMode = m_RetransmissionMode;
    param.isSupportDynamicGraphics = m_isSupportDynamicGraphics;
    param.isSupportPrerecorded = m_isSupportPrerecorded;
    param.nImageLength = m_ImageLength;
    NET_INFO("CDynamicGraphics::Init, m_RetransmissionMode=%d, m_isSupportDynamicGraphics=%d, m_isSupportPrerecorded=%d\n", m_RetransmissionMode, m_isSupportDynamicGraphics, m_isSupportPrerecorded);
    ZWP2PPUSHVNClient_SetP6SMSGParam(param);
    //设置动态图回调
    ZWP2P_PUSH_P6SMSG_CALLBACK P6SMSGCallback;
    P6SMSGCallback.sendEventStatusCallback = SendEventStatusCallback;
    P6SMSGCallback.imageCaptureCallback = ImageCaptureCallback;
    P6SMSGCallback.packageStatusCallback = PackageStatusCallback;
    ZWP2PPUSHVNClient_SetP6SMsgCallback(P6SMSGCallback);
    m_bInit = true;
}

unsigned int CDynamicGraphics::GetRetransmissionMode()
{
    return m_RetransmissionMode;
}

unsigned int CDynamicGraphics::GetIsSupportDynamicGraphics()
{
    return m_isSupportDynamicGraphics;
}

unsigned int CDynamicGraphics::GetIsSupportPrerecorded()
{
    if(m_isSupportDynamicGraphics == 0)
    {
        return 0;
    }
    return m_isSupportPrerecorded;
}

int CDynamicGraphics::PushEvent(int nEventType,int nChannel,LPZWP2P_PUSH_EVENT_INFO pEventInfo)
{
    if(pEventInfo == NULL)
    {
        NET_ERR("CDynamicGraphics::PushEvent, pEventInfo is NULL\n");
        return -1;
    }

    if(!IsInit())
    {
        NET_ERR("CDynamicGraphics::PushEvent, not init\n");
        return -1;
    }

    int nRet = 0;
    char did[64] = {0};
    char mac[18] = {0};
    ZWP2P_PUSH_P6SMSG_EVENT_INFO P6SMSGEventInfo;
    memset(&P6SMSGEventInfo,0,sizeof(ZWP2P_PUSH_P6SMSG_EVENT_INFO));
    NET_DBG("nEventType = %d, nChannel = %d\n", nEventType, nChannel);
    //填充DID
    GetDID(did, sizeof(did));
    strncpy(P6SMSGEventInfo.cDeviceDID, did, sizeof(P6SMSGEventInfo.cDeviceDID)-1);

    P6SMSGEventInfo.nEventType = nEventType;
    P6SMSGEventInfo.nChannel = nChannel;
    //填充事件时间
    struct timeval tv;
    int nTimeZone = 0;
    nTimeZone = MPU_SYS_GetTimeZone();
    gettimeofday(&tv, NULL);
    tv.tv_sec -= nTimeZone*60;
    P6SMSGEventInfo.nEventTime = tv.tv_sec*1000 + tv.tv_usec/1000;
    P6SMSGEventInfo.nDeviceTime = P6SMSGEventInfo.nEventTime;
    NET_DBG("nTimeZone=%d, nEventTime=%llu\n", nTimeZone, P6SMSGEventInfo.nEventTime);
    //填充背景图数据
    P6SMSGEventInfo.stBackgroundInfo.nValue = 1;
    P6SMSGEventInfo.stBackgroundInfo.strPicBuff = pEventInfo->pMediaData;
    P6SMSGEventInfo.stBackgroundInfo.nPicLen = pEventInfo->nMediaDataLen;
    P6SMSGEventInfo.stBackgroundInfo.imageStorageMode = 0;
    //填充动态图信息
    P6SMSGEventInfo.dynamicGraphicsinfo.nValid = 1;
    P6SMSGEventInfo.dynamicGraphicsinfo.imageSerialNumber = m_isSupportPrerecorded ? 1 : 0;//有预录为1，没有预录为0
    P6SMSGEventInfo.dynamicGraphicsinfo.picType = 0;

    //填充事件UUID
    std::string strUUID;
    m_EventID++;
    MPU_NET_GetMacAddress(mac);
    CreateUUID(P6SMSGEventInfo.cDeviceDID, mac, P6SMSGEventInfo.nEventTime, m_EventID, strUUID);
    strncpy(P6SMSGEventInfo.strUUID, strUUID.c_str(), sizeof(P6SMSGEventInfo.strUUID)-1);
	NET_DBG("UUID [%s]\n",P6SMSGEventInfo.strUUID);
    nRet = ZWP2PPUSHVNClient_P6SMsgPushEvent(&P6SMSGEventInfo);
    if(nRet)
    {
        NET_ERR("ZWP2PPUSHVNClient_P6SMsgPushEvent Failed, ret=%d\n", nRet);
    }
    return nRet;
}

int CDynamicGraphics::GetDID(char *did, int didlen)
{
	TutkCfg stuTutkCfg = {0};
	unsigned long tutkStatus = 0;
	MPU_NET_GetTutkUID(&stuTutkCfg, &tutkStatus);
	snprintf(did, didlen, "%s", stuTutkCfg.szTutkUID);
    return 0;
}

bool CDynamicGraphics::IsInit()
{
    return m_bInit;
}

#endif // __IS_SUPPORT_DYNAMIC_GRAPHICS__