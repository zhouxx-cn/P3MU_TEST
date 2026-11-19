#ifndef _LBH_BLE_H_
#define _LBH_BLE_H_

/**
 * @brief     Prerequisite of terminating LBH
 * 
 * @attention Must be invoked from thread other than LBH Client thread
 * 
*/
#include "../../atbm_nimble_api.h"
#define BLE_TEST 0
#define SERVER_S 0

int recv_data_to_app(unsigned char * data, unsigned int data_len, int conidxFlag);
int set_recv_data_cb(RecvDataCallback cb);
void lbh_terminate();
void str_change(const char * name,int len);
int lbh_scan_bt_dev_parse(char *devList[], int *devNum);
void client_queue_msg_callback(uint8_t *data, uint16_t length);
int lbh_scan_bt(char *indevlist, int inlen);
int lbh_ble_connect_bt_dev(uint8_t * addr , uint8_t type);
int get_ble_connect_status();
int lbh_ble_disconnect_bt_dev();
#endif//_LBH_BLE_H_