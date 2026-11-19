#ifndef __HIS_BLE_FUNC_H__
#define __HIS_BLE_FUNC_H__

#include <stdbool.h>
#include <unistd.h>
#include <inttypes.h>

// 定义颜色转义序列
#define COLOR_GREEN "\033[0;32m"
#define COLOR_YELLOW "\033[0;33m"
#define COLOR_RED "\033[0;31m"
#define COLOR_BLUE "\033[0;34m"
#define COLOR_RESET "\033[0m"

#define HINF(fmt, ...) printf(COLOR_GREEN "[H BLE]"fmt COLOR_RESET "", ##__VA_ARGS__)
#define HWAR(fmt, ...) printf(COLOR_YELLOW "[H BLE]"fmt COLOR_RESET "", ##__VA_ARGS__)
#define HERR(fmt, ...) printf(COLOR_RED "[H BLE]"fmt COLOR_RESET "", ##__VA_ARGS__)
#define HCBK(fmt, ...) printf(COLOR_BLUE "[H BLE CBK]"fmt COLOR_RESET "", ##__VA_ARGS__)

#ifndef true
#define true 1
#endif

#ifndef false
#define false 0
#endif

typedef int (*his_ble_data_callback)( uint8_t *pDataBuf, unsigned int uiDataLen, uint8_t Key_name);

int his_register_ble_callback(his_ble_data_callback pCallback);
void his_setBluetoothName(const char* name);
int his_getBleRunningTimeSec();

void his_ble_start();
void his_ble_stop();


#endif

