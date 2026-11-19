#ifndef __HIS_BLE_API_H__
#define __HIS_BLE_API_H__

#if defined(__cplusplus)
extern "C" {
#endif

typedef int (*his_ble_data_callback)( uint8_t *pDataBuf, unsigned int uiDataLen, uint8_t Key_name);

//设置解析回调函数，收到数据后解析成功则关闭蓝牙
int His_BLE_registerBluetoothCallback(his_ble_data_callback pCallback);

//设置蓝牙名称
void His_BLE_setBluetoothName(const char* name);

//获取蓝牙启动到现在的时间
int His_BLE_getBleRunningTimeSec();

//启动蓝牙
int His_BLE_startBluetoothWork();

//关闭蓝牙
void His_BLE_stopBluetoothWork();

#if defined(__cplusplus)
}
#endif

#endif

