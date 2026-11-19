//AVL_global.h
#ifndef _AVL_GLOBAL_H_
#define _AVL_GLOBAL_H_
#include "tagAlertInfo.h"
#include "tagActionInfo.h"
#include "idvr_rnl.h"
#include "global_gather.h"
#include "LogManager.h"

//#define TRACE 1?1:printf

#define AVL_ERR(format...)                LOG_E(LOG_MODULE_AVL, format)
#define AVL_WARN(format...)               LOG_W(LOG_MODULE_AVL, format)
#define AVL_INFO(format...)               LOG_I(LOG_MODULE_AVL, format)
#define AVL_DBG(format...)                LOG_D(LOG_MODULE_AVL, format)
#define TRACE_AVL(format...)              LOG_D(LOG_MODULE_AVL, format)
//#define TRACE_WARN(format...)             LOG_D(LOG_MODULE_AVL, format)

#define ALARM_ERR(format...)              LOG_E(LOG_MODULE_AVL, format)
#define ALARM_WARN(format...)             LOG_W(LOG_MODULE_AVL, format)
#define ALARM_INFO(format...)             LOG_I(LOG_MODULE_AVL, format)
#define ALARM_DBG(format...)              LOG_D(LOG_MODULE_AVL, format)

#define OSD_ERR(format...)                LOG_E(LOG_MODULE_AVL, format)
#define OSD_WARN(format...)               LOG_W(LOG_MODULE_AVL, format)
#define OSD_INFO(format...)               LOG_I(LOG_MODULE_AVL, format)
#define OSD_DBG(format...)                LOG_D(LOG_MODULE_AVL, format)

typedef int (*ALARMCALLBACK) (struct tagAlertInfo * pAlertInfo, struct tagActionInfo * pActionInfo);

/* 帧头信息 */
typedef struct tagH264DataPacketHeader
{
    unsigned int        uDataPacketStartCode; //固定为0xB7010000
    unsigned int        uFrameNo;				//媒体数据包的序号(音视频独立编号)
    unsigned char       byFrameType;
    unsigned char       byLoadType;				//媒体数据包的信息,具体含义依赖于帧类型
    unsigned int        uShowTime;				//表示媒体数据包相对于上一帧的偏差时间(频率90Khz)
    unsigned long       u32RealTimeSec;
    unsigned long       u32RealTimeUSec;
    unsigned short      uReserved;
    unsigned int        uOverloadLen;			//表示媒体数据包的数据长度
}__attribute__((packed)) H264DATAPACKETHEADER, *PH264DATAPACKETHEADER;

#endif
