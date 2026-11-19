#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <pthread.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <net/if_arp.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <stdbool.h>
#include <poll.h>
#include <sys/eventfd.h>
#include <sys/epoll.h>
#include <sys/timerfd.h>
#include "queue_client.h"
#ifdef CONFIG_LBH_CODEC
#include "lsbc.h"
#include "aacdec.h"
#include "coder.h"
#endif //CONFIG_LBH_CODEC
#include <sys/stat.h>
#include "lbh_bt.h"
#include "lbh_ble.h"
#include "lbh_codec.h"

#include "atbm_nimble_api.h"

int server_or_client = SERVER;
char* s_hci_command[256];
char btName[128] = { 0 };
int btNameLen = 0;
int lbh_blue_operator = 0;
void helper(){
    printf("\t<-s> to be client app. ex. \"lbh_client -s \n");
    printf("\t<-s> to be client app send test. ex. \"lbh_client -c \n");
    //printf("\t<-c> to send hci cmd to interface.\n");
}

// 一旦进入，在Socket被销毁或者lbh_server被KILL之前，不会退出
void * MsgCbInitThread(void *para)
{
    lbh_client_socket_and_msg_cb_init();
    printf("lbh_client thread exit!!!\n");
    return NULL;
}

int blueToothNameChange(char *s_strName, int str_len, char *c_hexName,int *hex_len)
{
    if(s_strName == NULL || c_hexName == NULL)
    {
        printf("input para error\n");
        return -1;
    }
    if(str_len > 32)
    {
        printf("str_len is length \n");
        return -1;
    }
    printf(" bt = ");
    for(int ii = 0; ii < str_len; ii++)
    {
        sprintf(c_hexName[ii+2],"0x%2x",s_strName[ii]);
        *hex_len++;
        printf(" %x ",c_hexName[ii]);
    }
    printf(" \n ");
    return 0;
}

int get_bt_client_con_status()
{
    return get_ble_connect_status();
}


/*!
 *  功  能: 打开蓝牙
 *  param1: 蓝牙初始化参数
 *  retval: 0-成功，其他-错误码
 */
 
int BluetoothOpen(BluetoothParam *pParam, const uint8_t * bluemac)
{
    if (NULL == pParam)
    {
        printf("Invalid param:NULL!\n");
        return -1;
    }
    str_change(pParam->szDeviceName,strlen(pParam->szDeviceName));
    str_mac_change(bluemac,6);
    set_recv_data_cb(pParam->pCallbackRecvData);
#ifdef LBH_BLE_ENABLE
    app_queue_reg_callback(client_queue_msg_callback);
#endif
#ifdef LBH_BT_ENABLE
    app_queue_reg_bt_msg_callback(client_queue_bt_msg_callback);
#endif
    // 初始化-----需要一个线程进行初始化 
    pthread_t lbh_msg_cb_init;
    pthread_create( &lbh_msg_cb_init, NULL, MsgCbInitThread, NULL);
    pthread_detach(lbh_msg_cb_init);
    
    lbh_blue_operator = 1;
	return 0;
}

/*!
 *  功  能: 蓝牙库发送数据接口
 *  param1: 指向待发送数据
 *  param2: 待发送数据长度
 *  retval: 0-成功，其他-错误码
 */

char send_buff[64] = { 0 };

char read_uuid_buff[16] = {0xfb,0x34,0x9b,0x5f,0x80,0x00,0x00,0x80,0x00,0x10,0x00,0x00,0x90,0x2a,0x00,0x00};
int BluetoothSendData(unsigned char *pdata, unsigned int u32Len, int conidxFlag)
{
    int nRet = 0;
    if (NULL == pdata
        || 0 == u32Len)
    {
		printf("BluetoothSendData: Interface param illegal!\n");
		return -1;
	}
    /* 发送数据接口 */
    memset(send_buff,0,sizeof(send_buff));
    memcpy(send_buff,read_uuid_buff,16);    /* read 的 UUID 开头 */
    memcpy(send_buff + 16,pdata,u32Len);    /* 然后后面包含数据 */
    u32Len += 16;     /* data + header(uuid--16) */

#if defined (__H4S_W__) || defined (__H8SX__) || defined (__H8SVX__)  || defined (__H5SVX__) || defined (__H4_W__)
    //接收数据时开头去掉了0x0,在应答时需要补上
    if(1 == conidxFlag)
    {
        if(u32Len + 1 > sizeof(send_buff))
        {
            printf("u32Len len(%d) > send_buff len(%d)!\n", u32Len+1, sizeof(send_buff));
            return -2;
        }
        else
        {
            memmove(send_buff + 1, send_buff, u32Len);
            send_buff[0] = 0x00; // 头部插入0x00
            u32Len += 1;
        }
    }
#endif

    nRet = app_ble_smartconfig_send_notification(send_buff,u32Len);
    if (0 != nRet)
    {
		printf("ble_smart_cfg_send_notification failed: nRet=%d!\n", nRet);
		return -2;
	}
    nRet =app_ble_smartconfig_update_read_value(send_buff,u32Len);
    if (0 != nRet)
    {
		printf("ble_smart_update_read_value failed: nRet=%d!\n", nRet);
		return -2;
	}
	
    return 0;
}

/*!
 *  功  能: 关闭蓝牙
 *  retval: 0-成功，其他-错误码
 */
int BluetoothClose(void)
{
    if(lbh_blue_operator)
    {
        lbh_blue_operator = 0;
    app_ble_adv_stop_msg_send();
        if(app_ble_udfc_connect_cancel())
        {
            printf("cancel disconnect fail\n");
            return -1;
        }
        printf("cancel disconnect success\n");
    }
    return 0;
}

int BluetoothScan(void)
{
    if(lbh_open_scan_bt())
    {
        return -1;
    }
    return 0;
}
int BluetoothConnectDev(uint8_t *mac_addr,uint8_t type)
{
    if(lbh_ble_connect_bt_dev(mac_addr,type))
    {
        return -1;
    }
    return 0;
}
int BluetoothDisconnectDev()
{
    if(lbh_ble_disconnect_bt_dev())
    {
        return -1;
    }
    return 0;
}


int parse_cmd_line(int argc, char **argv){
    int command_number = argc;
    int command_counter = 1;
    int hci_command_counter = 0;
    //int error_command = 0;

    while(command_counter < command_number){
        if(!strcmp(argv[command_counter], "-s")){
            if(!(command_number >= 3)){
                printf("error command \r\n");
                return ERR;
            }
            printf("bt_hci_tool server setting\r\n");
            server_or_client = SERVER;
            command_counter++;
        }else if(!strcmp(argv[command_counter], "-c")){
            server_or_client = CLIENT;
            command_counter++;
            hci_command_counter = 0;
            while(command_counter < command_number){
                s_hci_command[hci_command_counter++] = argv[command_counter];
                command_counter++;
            }
            return OK;
        } else{
            if (strcmp(argv[1], "-h") != 0) {
                printf("unknow command %s \r\n", argv[command_counter]);
            }
            helper();
            return ERR;
        }

        command_counter++;
    }

    helper();
    return ERR;

}

unsigned char atoh(char* s_data){
    unsigned char ret_value;
    char temp_value = 0;

    //h
    if(s_data[0] >= 48 && s_data[0] <= 57){
        temp_value = s_data[0] - 48;
    }else if(s_data[0] >= 65 && s_data[0] <= 70){
        temp_value = (s_data[0] - 65) + 10;
    }else if(s_data[0] >= 97 && s_data[0] <= 102){
        temp_value = (s_data[0] - 97) + 10;
    }

    ret_value = temp_value;
    ret_value = ret_value << 4;
    //printf("atoh1: ret_value:%x\r\n", ret_value);
    //l
    if(s_data[1] >= 48 && s_data[1] <= 57){
        temp_value = s_data[1] - 48;
    }else if(s_data[1] >= 65 && s_data[1] <= 70){
        temp_value = (s_data[1] - 65) + 10;
    }else if(s_data[1] >= 97 && s_data[1] <= 102){
        temp_value = (s_data[1] - 97) + 10;
    }

    ret_value |= temp_value;
    //printf("atoh2: ret_value:%x\r\n", ret_value);

    return ret_value;

}

void start_server(){
    int ret;
#ifdef CONFIG_LBH_CODEC
    pthread_t bt_codec_th;
    ret = pthread_create( &bt_codec_th, NULL, app_codec_thread, NULL);
    if(ret == ERR){
        printf("bt_codec_th fail \r\n");
        exit(0);
    } 
#endif //CONFIG_LBH_CODEC

#ifdef LBH_BLE_ENABLE
    app_queue_reg_callback(client_queue_msg_callback);
#endif
#ifdef LBH_BT_ENABLE
    app_queue_reg_bt_msg_callback(client_queue_bt_msg_callback);
#endif
    printf("start init client msg\n");
    lbh_client_socket_and_msg_cb_init();
}

#if BLE_TEST
int main(int argc, char *argv[]){
#if 0
    int ret = parse_cmd_line(argc, argv);
    if (ret) {
        return ret;
    }
#endif
    if(server_or_client == SERVER){
        start_server();
    } else if(server_or_client == CLIENT){

    }
    return 0;
}
#endif