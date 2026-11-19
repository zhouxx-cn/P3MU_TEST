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
#include "lbh_codec.h"
#include "lbh_ble.h"

#define SOCKER_BUFFER_SIZE  1024

uint8_t test_buff[] = {0xfb,0x34,0x9b,0x5f,0x80,0x00,0x00,0x80,0x00,0x10,0x00,0x00,0xc8,0xfe,0x00,0x00,\
                   0x00,0x22,0x33,0x44,0x55,0x11,0x22,0x33,0x44,0x55,0x11,0x22,0x33,0x44,0x55,0x11,0x22,0x33,0x44,0x55,\
                   0x11,0x22,0x33,0x44,0x55,0x11,0x22,0x33,0x44,0x55,0x11,0x22,0x33,0x44,0x55,0x11,0x22,0x33,0x44,0x55,\
                   0x11,0x22,0x33,0x44,0x55,0x11,0x22,0x33,0x44,0x55,0x11,0x22,0x33,0x44,0x55,0x11,0x22,0x33,0x44,0x55,\
                   0x11,0x22,0x33,0x44,0x55,0x11,0x22,0x33,0x44,0x55,0x11,0x22,0x33,0x44,0x55,0x11,0x22,0x33,0x44,0x55};
static volatile struct lbh_ble_env_t {
    volatile bool advertising;
    volatile bool scanning;
    volatile bool connected;
    volatile bool terminating;
} lbh_ble_env;

/* 定义一个回调，用于接收应用的接口 */
RecvDataCallback bt_recv_data_cb = NULL;
/* 记录产测下设备属于什么设备 */
int pt_mode = 0;
uint16_t bt_con_handel = 0;
typedef struct BTCONPARA
{
    uint8_t serMac[6];
    uint8_t serType;
    uint8_t enable;
}btConServerPara;
btConServerPara ConPara;
int server_connect_status = 0;
// int connectFinsh = 0;
int udfc_status = 0;
int recv_data_to_app(unsigned char * data, unsigned int data_len, int conidxFlag)
{
    if(bt_recv_data_cb == NULL || data == NULL)
    {
        return -1;
    }
    printf("ble--len %d \n recv app data : ", data_len);
    for(int ii = 0 ; ii < data_len; ii++)
    {
        printf(" %02x ", data[ii]);
    }
    printf("\n");
    bt_recv_data_cb(data,data_len, conidxFlag);
    return 0;
}


int set_recv_data_cb(RecvDataCallback cb)
{
    if(cb == NULL)
    {
        printf("set_recv_data_cb input para error\n");
        return -1;
    }
    bt_recv_data_cb = cb;
    return 0;
}

void* lbh_tx_test_thread( void *arg ){
    // int test_round = 0;
    // sleep(4);
    // printf("lbh_tx_test_thread ready %p\r\n", arg);
    // app_ble_adv_start_msg_send();
    // printf("%s exit", __func__);
    // sleep(4);
    // app_ble_smartconfig_send_notification(test_buff,80);
    // // app_ble_smartconfig_send_indication(test_buff,80);
    // while(test_round < 10){
    //     sleep(4);
    //     app_ble_smartconfig_update_read_value(test_buff,20);
    //     test_buff[16]++;
    //     test_round++;
    // }
    lbh_terminate();

    return NULL;
}
/* 获取蓝牙名称 */
int lbh_scan_bt(char *indevlist, int inlen)
{
    if(indevlist == NULL)
    {
        return -1;
    }
    if(strstr(indevlist,"ZWBLE"))
    {
        #if BLE_TEST
        uint8_t btaddr[6] = {0xA4,0xEF,0x41,0x00,0x00,0xA5}; 
        // uint8_t btaddr[6] = {0xA5,0x00,0x00,0x41,0xEF,0xA4}; 
        printf("---0000-- %s ----%d-\n", indevlist,inlen);
        lbh_ble_connect_bt_dev(btaddr, 0);
        lbh_ble_disconnect_bt_dev();
        printf("---1111-- %s ----%d-\n", indevlist,inlen);
        lbh_ble_connect_bt_dev(btaddr, 1);
        lbh_ble_disconnect_bt_dev();
        #endif
    }
    return 0;
}


/* 扫描其他蓝牙设备---获取其他蓝牙名称 */
/* 返回值  < 0 :设备异常   = 0 ：设备扫描正常  >0 : 没有搜索到设备 */
int lbh_scan_bt_dev_parse(char *devList[], int *devNum)
{
    if(devList == NULL || devNum == NULL)
    {
        printf("get blue list ,input para error \n");
        return -1;
    }
    /* 获取蓝牙的列表进行解析 */
    return 0;
}
/* 开启蓝牙扫描 */
int lbh_open_scan_bt()
{
    app_ble_set_scan_param(0,1000);   /* 开启扫描前，设置扫描的间隔和窗口 */
    app_ble_scan_msg_start();       /* 开启扫描 */
    return 0;
}

void * lbh_con_bt_mac_thread(void *arg)
{
    int ii = 0;
    while(ii++ < 20)
    {
        // uint8_t btaddr[6] = {0xA4,0xEF,0x41,0x00,0x00,0xA5}; 
        printf("------%d-------\n", ii);
        if(app_ble_udfc_connect(ConPara.serMac, ConPara.serType))
        {
            printf("1 connect buletooth fail \n");
        }
        printf("connect buletooth success\n");
        sleep(2);
        /* 发起断开连接 */
        lbh_ble_disconnect_bt_dev();
        sleep(2);
        if(udfc_status)  // 连接成功 
        {
            udfc_status  = 0;
            // connectFinsh = 1;
            /* 断开连接 */
            app_ble_disconnect_msg_send(bt_con_handel);
            return NULL;
        }
    }
    // connectFinsh = 1;
    return NULL;
}


/* 连接其他的蓝牙设备 */
int lbh_ble_connect_bt_dev(uint8_t * addr , uint8_t type)
{
    uint8_t server_addr[6] = { 0 };
    if(addr == NULL)
    {
        printf("0 connect buletooth fail \n");
        return -1;
    }
    printf("xrs addr %02x %02x %02x %02x %02x %02x \n",addr[0],addr[1],addr[2],addr[3],addr[4],addr[5]);
    for(int ii = 0; ii < 6; ii++)
    {
        ConPara.serMac[5 - ii] = addr[ii];
    }
    ConPara.serType = type;
    ConPara.enable = 1;
    printf("xrs server_addr %02x %02x %02x %02x %02x %02x \n",ConPara.serMac[0],ConPara.serMac[1],ConPara.serMac[2],ConPara.serMac[3],ConPara.serMac[4],ConPara.serMac[5]);
    pthread_t lbh_tx_test_th;
    pthread_create( &lbh_tx_test_th, NULL, lbh_con_bt_mac_thread, NULL);
    return 0;
}


/* 断开和其他设备蓝牙的连接 */
int lbh_ble_disconnect_bt_dev()
{
    if(app_ble_udfc_connect_cancel())
    {
        printf("cancel disconnect fail\n");
        return -1;
    }
    printf("cancel disconnect success\n");
    return 0;
}

void lbh_terminate()
{
    lbh_ble_env.terminating = true;
    // if (lbh_ble_env.advertising) {
    //     app_ble_adv_stop_msg_send();
    // }
    // if (lbh_ble_env.scanning) {
    //     app_ble_scan_msg_stop();
    // }
    if (lbh_ble_env.connected) {
        app_ble_disconnect_msg_send(1);
    }
    while (lbh_ble_env.advertising || lbh_ble_env.scanning || lbh_ble_env.connected) {
        sleep(1);
    }
    lbh_ble_env.terminating = false;
}
uint8_t scan_buff[64] = { 0 };

void str_change(const char * name,int len)
{
    memset(scan_buff,0,sizeof(scan_buff));
    strncpy(scan_buff, name, len);
}





/* 设置设备蓝牙名字 */
void set_blue_tooth_dev_name(const char * str_str, int str_len,char *hex_str)
{
    if(str_str == NULL || hex_str == NULL)
    {
        printf("input para error\n");
        return ;
    }
    if(str_len > 32)
    {
        printf("str_len is length \n");
        return ;
    }
    if(str_len < 8)
    {
        uint8_t scan_response_buff[] = {0x0D,0x09,0x5A,0x57,0x42,0x4C,0x45,0x5F,0x38,0x34,0x35,0x36,0x32,0x42};
        memcpy(hex_str, scan_response_buff, 14);
    }
    else{
        char hex_len = 0;
        str_len += 1;
        hex_str[0] = str_len;
        hex_str[1] = 0x09;
        for(int ii = 0; ii < str_len; ii++)
        {
            hex_str[ii + 2] = str_str[ii];
        }
    }
    
    return ;
}

uint8_t mac_buff[6] = { 0 };

void str_mac_change(const uint8_t * name,int len)
{
    if(name == NULL)
    {
        printf("str_mac_change input is NULL\n");
        return ;
    }
    printf("-b-l-e-blueMac = %02x %02x %02x %02x %02x %02x \n", name[0],name[1],name[2],name[3],name[4],name[5]);
    memset(mac_buff,0,sizeof(mac_buff));
    for(int ii = 0; ii < len; ii++)
    {
        mac_buff[ii] = name[ii];
    }
}
void set_blue_tooth_dev_mac(const uint8_t * str_str, int str_len,uint8_t *hex_str)
{
    if(str_str == NULL || hex_str == NULL)
    {
        printf("input macpara error\n");
        return ;
    }
    if(str_len > 8)
    {
        printf("str_len is length \n");
        return ;
    }
    for(int ii = 0; ii < str_len; ii++)
    {
        hex_str[5 - ii] = str_str[ii];
    }
    printf("-b-l-e-blueMac5 = %02x %02x %02x %02x %02x %02x \n", hex_str[0],hex_str[1],hex_str[2],hex_str[3],hex_str[4],hex_str[5]);
    printf("-b-l-e-blueMac5 OK--%d\n",str_len);
}
int lbh_bt_connect_success(int status)
{
    server_connect_status = status;
    if(status)
    {
        return 0;
    }
    return -1;
}

int get_ble_connect_status()
{
    if(server_connect_status)
    {
        server_connect_status = 0;
        return 1;
    }
    return 0;
}
int ble_udfc_connect_status(int status)
{
    udfc_status = status;
    return 0;
}

void client_queue_msg_callback(uint8_t *data, uint16_t length)
{
    // printf("client_queue_msg_callback\n");
    int i = 0;
    BLE_HST_MSG msg;
        if(length>0){
        for(i = 3; i < length; i++){
            printf("%02X ", data[i]);
        }
        printf("\r\n");
        msg.msg_id = data[0];
        msg.len = length -3;
        msg.buff = data+3;
        switch(msg.msg_id ){
            case BLE_INIT_DONE:
                {
                    //uint8_t buff[64] = {0x0b,0x09,0x54,0x38,0x34,0x31,0x36,0x5f,0x78,0x78,0x78,0x78};
                                                    /* len + type + value */
                    // "ZWBLE_8456232CA"
                    // uint8_t scan_response_buff[] = {0x0F,0x09,0x6E,0x6F,0x6F,0x6E,0x6E,0x2D,0x30,0x30,0x30,0x62,0x2D,0x30,0x31,0x64};
                #if BLE_TEST
                    uint8_t scan_response_buff[] = {0x0F,0x09,0x5A,0x57,0x42,0x4C,0x45,0x5F,0x63,0x6C,0x69,0x65,0x6E,0x74,0x0D,0x0A};
                #else
                    uint8_t scan_response_buff[64] = { 0 };
                    set_blue_tooth_dev_name(scan_buff, strlen(scan_buff), scan_response_buff);
                    printf("scan_response_buff = %s \n",scan_response_buff);
                #endif
                    uint8_t adv_data_buff[] = {0x18,0x21,0x00,0x50,0xEB,0x37,0x6F,0x26,0x15,0xB7,0x17,0x42,0x56,0x56,0x58,0x11,\
                                        0x3D,0x3E,0x0B,0x00,0x4F,0x4E,0x1D,0x00,0x00};
                #if BLE_TEST
                    #if SERVER_S
                        uint8_t btaddr[6] = {0xA4,0xEF,0x41,0x10,0x10,0xA5}; 
                    #else
                        uint8_t btaddr[6] = {0xA4,0x92,0x72,0x04,0x10,0x94}; 
                    #endif
                #else
                    uint8_t btaddr[6] = { 0 };
                    set_blue_tooth_dev_mac(mac_buff, sizeof(mac_buff), btaddr);
                #endif
                    char ble_gap_name[64] = {0};
                #if BLE_TEST
                    strcpy(ble_gap_name, "BLE_SMARTCONFIG_TEST_CASE");
                #else
                    strcpy(ble_gap_name, scan_response_buff);
                #endif
                    printf("BLE  INITIATIVE  DONE\r\n");
                    app_ble_set_bt_addr(btaddr);
                    app_ble_gap_name_set_msg_send((uint8_t *)ble_gap_name,strlen(ble_gap_name));
                    app_ble_adv_param_set_msg_send(160,160,6);
                    app_ble_adv_data_set_msg_send(adv_data_buff,sizeof(adv_data_buff));
                    app_ble_scan_response_set_msg_send(scan_response_buff,strlen(scan_response_buff));
                    app_ble_adv_start_msg_send();
                    #if BLE_TEST
                    #if SERVER_S
                    #else
                        // lbh_con_bt_mac_thread(NULL);
                        pthread_t lbh_tx_test_th;
                        pthread_create( &lbh_tx_test_th, NULL, lbh_con_bt_mac_thread, NULL);
                    #endif
                    #else
                    #endif
                }
                break;
            case BLE_MSG_DONE:
                printf("BLE_CLEITN_MSG_SEND_DONE\r\n");

                break;
            case BLE_ADV_ENABLE:
                printf("BLE_ADV_ENABLE\r\n");
                lbh_ble_env.advertising = true;
                break;
            case BLE_ADV_DISABLE:
                printf("BLE_ADV_DISABLE\r\n");
                lbh_ble_env.advertising = false;
                break;
            case BLE_SCAN_ENABLE:
                printf("BLE_SCAN_ENABLE\r\n");
                lbh_ble_env.scanning = true;
                break;
            case BLE_SCAN_DISABLE:
                printf("BLE_SCAN_DISABLE\r\n");
                lbh_ble_env.scanning = false;
                break;
                /* 获取搜索到的蓝牙设备名称 */
            case BLE_ADV_REPORT:
            {
                struct gapm_ext_adv_report_ind *ptr = (struct gapm_ext_adv_report_ind *)msg.buff;
                printf("BLE_ADV_REPORT\n");
                printf("actv idx %x, info %x \n",ptr->actv_idx,ptr->info);
                printf("trans_addr.addr_type %x, addr %x,%x,%x,%x,%x,%x \n",ptr->trans_addr.addr_type,\
                ptr->trans_addr.addr.addr[0],ptr->trans_addr.addr.addr[1],\
                ptr->trans_addr.addr.addr[2],ptr->trans_addr.addr.addr[3],\
                ptr->trans_addr.addr.addr[4],ptr->trans_addr.addr.addr[5]);
                printf("target_addr,addrtype %x, addr %x,%x,%x,%x,%x,%x \n",ptr->target_addr.addr_type,\
                ptr->target_addr.addr.addr[0],ptr->target_addr.addr.addr[1],\
                ptr->target_addr.addr.addr[2],ptr->target_addr.addr.addr[3],\
                ptr->target_addr.addr.addr[4],ptr->target_addr.addr.addr[5]);
                printf("tx_pwr %x, rssi %d, phy_prim %x, phy_second %x,\
                adv_sid %x, period_adv_intv %x \n",\
                ptr->tx_pwr,ptr->rssi,ptr->phy_prim,ptr->phy_second,ptr->adv_sid,ptr->period_adv_intv);
                printf("length %d,data %x,%x,%x,%x,%x,%x,%x \n",ptr->length,\
                ptr->data[0],ptr->data[1],ptr->data[2],ptr->data[3],ptr->data[4],ptr->data[5],ptr->data[6]);
                // if(msg.buff[27] = 0x09)
                // {
                //     // printf("---sjz---recv scan dev---%d---\n",msg.len);
                //     // for(int ii = 0; ii < msg.len; ii++)
                //     // {
                //     //     printf(" %02x ", msg.buff[ii]);
                //     // } 
                //     // printf("\n");
                //     lbh_scan_bt(msg.buff + 28 ,msg.buff[0]);
                // }
            }
                break;
            /*  收到了对端设备的 WRITE 操作，buff 中的前 16 字节为被操作 characteristic 的 uuid，之后为发送的数据 */
            case BLE_SMARTCONFIG_DATA_RECV:    /* 手机向设备发送数据的地方 */
                printf("-----sjz---bluetooth---recv data----%d-\n",msg.len);
                int conidxFlag = 0;
			
                #if defined (__H4S_W__) || defined (__H8SX__) || defined (__H8SVX__) || defined (__H5SVX__) || defined (__H4_W__)
                    //新版本lbh_server数据格式为conidx(0x0)+UUID+data,需要跳过这一字节
                    if(0x0 == msg.buff[0])
                    {
                        conidxFlag = 1;
                        /* 将接收的数据传到回调上 */
                        recv_data_to_app(msg.buff + 17 ,msg.len - 17, conidxFlag);
                    }
                    else
                #endif
                    {
                        /* 将接收的数据传到回调上 */
                        recv_data_to_app(msg.buff + 16 ,msg.len - 16, conidxFlag);
                    }
                break;
            case BLE_CONNECTION_IND:
                {
                    struct ble_connection_ind *ptr = (struct ble_connection_ind *)msg.buff ;
                    printf("BLE_CONNECTION conhdl 0x%x,interval %d, peer_addr_type %d,addr: 0x%x,0x%x,0x%x,0x%x,0x%x,0x%x \r\n",
                        ptr->conhdl,ptr->con_interval,ptr->peer_addr_type,
                        ptr->peer_addr.addr[0],ptr->peer_addr.addr[1],ptr->peer_addr.addr[2],
                        ptr->peer_addr.addr[3],ptr->peer_addr.addr[4],ptr->peer_addr.addr[5]);
                    // app_ble_set_data_len(1,128,0x200);
                    lbh_ble_env.connected = true;
                    lbh_bt_connect_success(1);
                    ble_udfc_connect_status(1);
                    bt_con_handel = ptr->conhdl;
                    if(0)//open for test notification or indication or update read value
                    {
                        pthread_t lbh_tx_test_th;
                        pthread_create( &lbh_tx_test_th, NULL, lbh_tx_test_thread, NULL);
                    }
                }
                break;
            case BLE_DISCONNECTED:
                {
                    struct ble_disconnect_ind *ptr = (struct ble_disconnect_ind *)msg.buff ;
                    printf("BLE_DISCONNECT conhdl 0x%x ,reason 0x%x \r\n",ptr->conhdl,ptr->reason);
                    lbh_ble_env.connected = false;
                    if (!lbh_ble_env.terminating) {
                        app_ble_adv_start_msg_send();
                    }
                }
                break;
            case BLE_CONNECTION_PARAM_UPDATE:
                {
                    struct ble_param_update_req_ind *ptr = (struct ble_param_update_req_ind *)msg.buff ;
                    printf("BLE_CONNECTION_PARAM_UPDATE intv_min 0x%x ,intv_max 0x%x, latency 0x%x,time_out 0x%x\r\n",
                        ptr->intv_min,ptr->intv_max,ptr->latency,ptr->time_out);
                }
                break;
            case BLE_EXCHANGED_MTU_IND:
                {
                    struct ble_mtu_changed_ind *ptr = (struct ble_mtu_changed_ind *)msg.buff ;
                    printf("BLE_EXCHANGED_MTU_IND mtu 0x%x ,seq_num 0x%x \r\n",
                        ptr->mtu,ptr->seq_num);
                }
                break;
            case BLE_PKT_SIZE_IND:
                {
                    struct ble_pkt_size_ind *ptr = (struct ble_pkt_size_ind *)msg.buff ;
                    printf("BLE_PKT_SIZE_IND max_rx_octets 0x%x ,max_rx_time 0x%x ,max_tx_octets 0x%x ,max_tx_time 0x%x\r\n",
                        ptr->max_rx_octets,ptr->max_rx_time,ptr->max_tx_octets,ptr->max_tx_time);
                }
                break;
            case BLE_SERVICE_CHANGED_IND:
                {
                    printf("BLE_SERVICE_CHANGED_IND\n");
                }
                break;
            case BLE_NTF_SENT_DONE:
                {
                    printf("BLE_NTF_SENT_DONE\r\n");
                    app_ble_smartconfig_send_notification(test_buff,80);
                }
                break;
            case BLE_IND_SENT_DONE:
                {
                    uint8_t status = msg.buff[0];
                    printf("BLE_IND_SENT_DONE %d\r\n",status);
                    if(status == 0)
                        app_ble_smartconfig_send_indication(test_buff,80);
                }
                break;
            case BLE_CON_UPDATE_CMP_DONE:
                {
                    struct ble_param_updated_ind *ptr = (struct ble_param_updated_ind *)msg.buff ;
                    printf("BLE_CON_UPDATE_CMP_DONE ,interval 0x%x, latency 0x%x, to 0x%x\r\n",ptr->con_interval,ptr->con_latency,ptr->sup_to);
                }
                break;
            default:
                break;
        }
    }
}

