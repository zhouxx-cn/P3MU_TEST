#ifndef DFCLIENT_COMMON_H
#define DFCLIENT_COMMON_H

// #include "InternalCommon.h"

#define MAX_DID_LENGTH      (64)    //UID总长度

/** \brief 入参															*/
#define DF_IN
/** \brief 出参															*/
#define DF_OUT

typedef struct tagHY_DVR_DF_INPUT_PARAM
{
    int nValid;
    char szReserved[128];       // 保留
}HY_DVR_DF_INPUT_PARAM;

typedef struct tagHY_DVR_DF_INTERVAL_INFO
{
    int nValid;
    int nInterval;              // 时间间隔
    char szReserved[128];       // 保留
}HY_DVR_DF_INTERVAL_INFO;

typedef struct tagHY_DVR_DF_DEVICE_FLOW
{              
    int nJR4G;                          // 1 -- 开启， 2 -- 关闭
    unsigned long long dwUpdateTime;    // 4g功能修改时间
    char szReserved[128];               // 保留
}HY_DVR_DF_DEVICE_FLOW;

typedef struct tagHY_DVR_DF_DEVICE_CFG
{
    int nValid;
    HY_DVR_DF_DEVICE_FLOW struDeviceFlow;
    char szReserved[1024];       // 保留
}HY_DVR_DF_DEVICE_CFG;

typedef struct tagHY_DVR_DF_BWCARD_INFO
{
    char strDID[MAX_DID_LENGTH];
    char szReserved[1024];
}HY_DVR_DF_BWCARD_INFO;

typedef enum
{
    eDF_errorcodeBegin = 0,             //起始值
    eDF_errorSuccess,                   //成功
    eDF_errorIccidDidNone,              //ICCID和DID都不存在
    eDF_errorIccidNotExist,             //ICCID不存在
    eDF_errorDealerStrategyRestrict,    //经销商策略限制
    eDF_errorServerSystemException,     //服务器系统异常（例：返回 code 为 9999）

    eDF_errorNoIdleResource = 7,        //原解绑接口使用的枚举值，来自于hiserver，解耦后定义在这里

    eDF_errorcodeEnd,                   //结束值
}eDF_ERRORCODE;
#endif 
