#ifndef __ATBM_NIBLE_API__
#define __ATBM_NIBLE_API__

typedef int (*RecvDataCallback)(unsigned char *pdata, unsigned int nLen, int conidxFlag);

typedef struct stBluetoothParam
{
    //蓝牙设备名(不宜过长，尽量控制在25字节内，否则蓝牙广播可能会失败)
	char szDeviceName[128];
	//蓝牙接收到数据，将回调此接口
	RecvDataCallback pCallbackRecvData;
} BluetoothParam;

#ifdef __cplusplus
extern "C" {
#endif

/*!
 *  功  能: 打开蓝牙
 *  param1: 蓝牙初始化参数
 *  retval: 0-成功，其他-错误码
 */
int BluetoothOpen(BluetoothParam *pParam, const uint8_t * bluemac);

/*!
 *  功  能: 蓝牙库发送数据接口
 *  param1: 指向待发送数据
 *  param2: 待发送数据长度
 * 	param3: conidx标记
 *  retval: 0-成功，其他-错误码
 */
int BluetoothSendData(unsigned char *pdata, unsigned int u32Len, int conidxFlag);

/*!
 *  功  能: 关闭蓝牙
 *  retval: 0-成功，其他-错误码
 */
int BluetoothClose(void);

/*!
 *  功  能: 搜索其他设备的蓝牙
 *  retval: 0-成功，其他-错误码
 */
int BluetoothScan(void);
/*!
 *  功  能: 连接指定mac的蓝牙设备
 *  retval: 0-成功，其他-错误码
 */
int BluetoothConnectDev(uint8_t *mac_addr,uint8_t type);
/*!
 *  功  能: 断开连接的设备
 *  retval: 0-成功，其他-错误码
 */
int BluetoothDisconnectDev();



int get_bt_client_con_status();
#ifdef __cplusplus
}
#endif

#endif

