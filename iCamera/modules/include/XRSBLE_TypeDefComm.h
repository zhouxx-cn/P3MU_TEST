/*!
 *\file     XRSBLE_TypeDefComm.h
 *\brief	XRSBLE协议层公共数据类型定义
 */
#ifndef __H_XRSBLE_TYPEDEF_COMM__
#define __H_XRSBLE_TYPEDEF_COMM__

#define XRSBLE_IN
#define XRSBLE_OUT
#define XRSBLE_INOUT

//XRSBLE协议版本号
#define XRSBLE_PROTOCOL_VERSION  0

typedef unsigned char XRSBLE_U8;
typedef unsigned short XRSBLE_U16;
typedef signed short XRSBLE_S16;
typedef unsigned int XRSBLE_U32;
typedef signed int XRSBLE_S32;

/*!
 *  功  能: 蓝牙库发送数据接口回调
 *  param1: 指向待发送数据
 *  param2: 待发送数据长度
 *  retval: 0-成功，其他-错误码
 *  说  明: 此接口由蓝牙库提供
 */
typedef XRSBLE_S32 (*BleSendDataCallback)(XRSBLE_IN XRSBLE_U8 *pdata, XRSBLE_IN XRSBLE_U32 u32Len);

//数据发送控制参数
typedef struct stXRSBLE_SendCtrlParam
{
    //发送后等待应答超时时间（单位：ms）
    XRSBLE_U32 u32TimeOutMsec;
    //发送重试次数
    XRSBLE_U8 u8ReTryCnt;
} XRSBLE_SendCtrlParam;

#endif // end of __H_XRSBLE_TYPEDEF_COMM__

