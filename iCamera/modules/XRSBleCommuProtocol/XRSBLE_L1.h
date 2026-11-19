/*!
 *\file     XRSBLE_L1.h
 *\brief	XRSBLE协议L1层头文件
 */
#ifndef __H_XRSBLE_L1__
#define __H_XRSBLE_L1__

#include "XRSBLE_TypeDefComm.h"

/*!
 *  功  能: L2层数据接收&处理回调
 *  param1: 指向接收数据（L1层处理完成后的数据，协议内部使用，包括普通数据/应答数据）
 *  param2: 接收数据长度
 *  param3: 此条数据解析出的SequenceId
 *  param4: 用于接收L2层处理完成后的应答数据(对于普通数据填缓冲区地址，对于应答数据填NULL)
 *  param5: IN：应答数据缓冲区大小 OUT：应答数据长度
 *  retval: 0-成功，其他-错误码
 *  说  明: 此接口由L2层提供
 */
typedef XRSBLE_S32 (*L2RecvProcessCallback)(XRSBLE_IN XRSBLE_U8 *pRecvdata, XRSBLE_IN XRSBLE_U32 u32RecvDataLen, XRSBLE_IN XRSBLE_U16 u16SeqId, XRSBLE_OUT XRSBLE_U8 *u8AckDataBuf, XRSBLE_INOUT XRSBLE_U32 *pDataLen);

//L1层初始化参数
typedef struct stXRSBLE_L1InitParam
{
    //蓝牙发送数据接口
	BleSendDataCallback pBleSendDataCallback;
    //L2层数据接收&处理接口
	L2RecvProcessCallback pL2RecvProcessCallback;
} XRSBLE_L1InitParam;

#ifdef __cplusplus
extern "C" {
#endif

/*!
 *  功  能: XRSBLE协议L1层初始化（这里主要供L2层调用，应用层不需要调用）
 *  param1: L1层初始化参数
 *  retval: 0-成功，其他-错误码
 *  说  明: 重复调用，直接返回成功
 */
XRSBLE_S32 XRSBLE_L1_Init(XRSBLE_IN XRSBLE_L1InitParam *pParam);

/*!
 *  功  能: XRSBLE协议L1层逆初始化（这里主要供L2层调用，应用层不需要调用）
 *  retval: 0-成功，其他-错误码
 *  说  明: 重复调用，直接返回成功
 */
XRSBLE_S32 XRSBLE_L1_UnInit();

/*!
 *  功  能: XRSBLE协议L1层发送数据（这里主要供L2层调用，应用层不需要调用）
 *  param1: 指向要发送的数据的指针（L2层次数据）
 *  param2: 待发送的数据的长度
 *  param3: 数据发送控制参数
 *  param4: 返回此次发送数据的SequenceId，供后续接收应答数据时寻找对应关系
 *  retval: 0-成功，其他-错误码
 *  说  明: 内部采用异步处理，会将待发送数据先存入队列
 */
XRSBLE_S32 XRSBLE_L1_SendData(XRSBLE_IN XRSBLE_U8 *pdata, XRSBLE_IN XRSBLE_U32 u32Len, XRSBLE_IN XRSBLE_SendCtrlParam *pSendCtlParam, XRSBLE_OUT XRSBLE_U16 *pSeqId);

/*!
 *  功  能: XRSBLE协议L1层接收&处理数据（这里供应用层调用，设置给蓝牙库，收到数据时回调）
 *  param1: 指向接收到的数据的指针（蓝牙接收数据）
 *  param2: 接收到的数据的长度
 *  param3: conidx标记
 *  retval: 0-成功，其他-错误码
 *  说  明: 蓝牙收到数据后，回调此接口，内部进行数据校验/解析/进一步回调L2层，同时其内部会做应答处理
 *  注意: 需避免长时间阻塞
 */
XRSBLE_S32 XRSBLE_L1_RecvProcess(XRSBLE_IN XRSBLE_U8 *pdata, XRSBLE_IN XRSBLE_U32 u32Len, int conidxFlag);

#ifdef __cplusplus
}
#endif

#endif // end of __H_XRSBLE_L1__

