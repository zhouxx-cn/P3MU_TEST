/*!
 *\file     XRSBLE_L2.h
 *\brief	XRSBLE协议L2层头文件
 */
#ifndef __H_XRSBLE_L2__
#define __H_XRSBLE_L2__

#include "XRSBLE_TypeDefComm.h"

/* Command Id定义（即主命令） */
#define CMD_ID_SET_INFO         0x1
#define CMD_ID_GET_INFO         0x2
////////////////////////////////

/* Key Id定义（即子命令） */
//CMD_ID_SET_INFO 下子命令：
//将要连接的wifi热点SSID（对应 XRSBLE_WifiSsidInfo）
#define KEY_ID_WIFI_SSID        0x1
//将要连接的wifi热点密码（对应 XRSBLE_WifiPwdInfo）
#define KEY_ID_WIFI_PWD         0x2
//时区信息（对应 XRSBLE_TimeZoneInfo）
#define KEY_ID_TIMEZONE         0x3

//CMD_ID_GET_INFO 下子命令：
//设备蓝牙协议版本号请求
#define KEY_ID_PROTOC_VER_REQ       0x1
//设备蓝牙协议版本号返回（对应 XRSBLE_ProtocVerInfo）
#define KEY_ID_PROTOC_VER_REPLY     0x2
//设备是否支持5G双频请求
#define  KEY_ID_IS_5G_WIFI_REQ      0x3
//设备是否支持5G双频返回（对应 XRSBLE_Is5GWifi）
#define  KEY_ID_IS_5G_WIFI_REPLY    0x4
////////////////////////////////

//单次发送命令最大个数
#define SINGLE_SEND_CMD_CNT_MAX (4)

//命令数据
typedef struct stXRSBLE_L2CmdData
{
    //应答数据/普通数据（应用层无需处理发送应答，发送数据时固定填0）
    //0-普通数据 1-应答数据
    XRSBLE_U8 u8DateType;
    //此条数据对应的SequenceId(仅处理接收数据时使用)
    XRSBLE_U16 u16SeqId;
    //主命令（一次发送中携带的多条命令，
    //主命令需相同，协议内部只取第一条数据的主命令）
	XRSBLE_U8 u8CmdId;
	//子命令
	XRSBLE_U8 u8KeyId;
	//具体数据（根据主子命令，对应不同结构体）
	//对于应答数据，此指针指向XRSBLE_S32类型数据（存放命令操作返回值）
	void *pData;
	//数据长度（pData指向结构体的大小）
	XRSBLE_U16 u16DataLen;
} XRSBLE_L2CmdData;

/*!
 *  功  能: 应用层数据接收&处理回调
 *  param1: 指向接收数据（L2层处理完成后的数据）
 *  retval: 0-成功，其他-错误码（不同操作类型，错误码不同）
 *  说  明: 需避免长时间阻塞
 *  注意: 不可在此接口内部调用XRSBLE协议库中其他接口，否则将死锁
 */
typedef XRSBLE_S32 (*AppRecvProcessCallback)(XRSBLE_IN XRSBLE_L2CmdData *pCmdData);

//L2层初始化参数
typedef struct stXRSBLE_L2InitParam
{
    //蓝牙发送数据接口
	BleSendDataCallback pBleSendDataCallback;
	//应用层数据接收&处理接口
	AppRecvProcessCallback pAppRecvProcessCallback;
} XRSBLE_L2InitParam;

//命令数据集合
typedef struct stXRSBLE_L2CmdDataSet
{
    //操作命令集合
	XRSBLE_L2CmdData stCmdData[SINGLE_SEND_CMD_CNT_MAX];
	//集合中命令个数
	XRSBLE_U8 u8CmdCnt;
} XRSBLE_L2CmdDataSet;

/* 命令结构体定义 */
//将要连接的wifi热点SSID（对应 KEY_ID_WIFI_SSID）
typedef struct stXRSBLE_WifiSsidInfo
{
    //base64编码的wifi SSID信息
    char szWifiSsidBase64[128];
    //保留字段
    XRSBLE_U8 u8Reserve[128];
} XRSBLE_WifiSsidInfo;

//将要连接的wifi热点密码（对应 KEY_ID_WIFI_PWD）
typedef struct stXRSBLE_WifiPwdInfo
{
    //base64编码的wifi 密码信息
    char szWifiPwdBase64[128];
    //保留字段
    XRSBLE_U8 u8Reserve[128];
} XRSBLE_WifiPwdInfo;

//时区信息（对应 KEY_ID_TIMEZONE）
typedef struct stXRSBLE_TimeZoneInfo
{
    //分钟表示的时区值（可正负）
    XRSBLE_S16 s16TimeZoneMin;
    //保留字段
    XRSBLE_U8 u8Reserve[48];
} XRSBLE_TimeZoneInfo;

//设备蓝牙协议版本信息（对应 KEY_ID_PROTOC_VER_REPLY）
typedef struct stXRSBLE_ProtocVerInfo
{
    //设备蓝牙协议版本号
    XRSBLE_U8 u8BleProtocVer;
    //保留字段
    XRSBLE_U8 u8Reserve[48];
} XRSBLE_ProtocVerInfo;

//设备蓝牙是否支持5G双频（对应 KEY_ID_IS_5G_WIFI_REPLY）
typedef struct stXRSBLE_Is5GWifi
{
    //设备是否支持5G双频
    XRSBLE_U8 u8BleIs5GWifi;
    //保留字段
    XRSBLE_U8 u8Reserve[48];
} XRSBLE_Is5GWifi;
////////////////////////////////

#ifdef __cplusplus
extern "C" {
#endif

/*!
 *  功  能: XRSBLE协议L2层初始化（其内部会维护对L1层的初始化）
 *  param1: L2层初始化参数
 *  retval: 0-成功，其他-错误码
 *  说  明: 重复调用，直接返回成功
 */
XRSBLE_S32 XRSBLE_L2_Init(XRSBLE_IN XRSBLE_L2InitParam *pParam);

/*!
 *  功  能: XRSBLE协议L2层逆初始化（其内部会维护对L1层的逆初始化）
 *  retval: 0-成功，其他-错误码
 *  说  明: 重复调用，直接返回成功
 */
XRSBLE_S32 XRSBLE_L2_UnInit();

/*!
 *  功  能: XRSBLE协议版本号获取
 *  param1: 用于返回XRSBLE协议版本号
 *  retval: 0-成功，其他-错误码
 */
XRSBLE_S32 XRSBLE_ProtocolVerGet(XRSBLE_IN XRSBLE_U8 *pBleProtocVer);

/*!
 *  功  能: XRSBLE协议L2层发送数据
 *  param1: 指向要发送的数据的指针（应用层数据）
 *  param2: 数据发送控制参数
 *  param3: 返回此次发送数据的SequenceId，供后续接收应答数据时寻找对应关系
 *  retval: 0-成功，其他-错误码
 *  说  明: 内部采用异步处理，会将待发送数据先存入队列
 */
XRSBLE_S32 XRSBLE_L2_SendData(XRSBLE_IN XRSBLE_L2CmdDataSet *pCmdDataSet, XRSBLE_IN XRSBLE_SendCtrlParam *pSendCtlParam, XRSBLE_OUT XRSBLE_U16 *pSeqId);

#ifdef __cplusplus
}
#endif

#endif // end of __H_XRSBLE_L2__

