#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "his_ble_func.h"

//设置解析回调函数，收到数据后解析成功则关闭蓝牙
int His_BLE_registerBluetoothCallback(his_ble_data_callback pCallback)
{
    his_register_ble_callback(pCallback);
    return 0;
}

//设置蓝牙名称
void His_BLE_setBluetoothName(const char* name)
{
    his_setBluetoothName(name);
}

int His_BLE_getBleRunningTimeSec()
{
    return his_getBleRunningTimeSec();
}

//启动蓝牙
void His_BLE_startBluetoothWork()
{
    HINF("start Bluetooth Work start\n");
    his_ble_start();
    HINF("start Bluetooth Work end\n");

    return;
}

//关闭蓝牙
void His_BLE_stopBluetoothWork()
{
    HINF("stop Bluetooth Work start\n");
    his_ble_stop();
    HINF("stop Bluetooth Work end\n");

    return;
}

