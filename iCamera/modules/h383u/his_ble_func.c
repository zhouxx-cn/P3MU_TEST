#include <stdlib.h>
#include <unistd.h>
#include <sys/select.h>
#include <signal.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/prctl.h>
#include <time.h>
#include <sys/ioctl.h>
#include <net/if.h>

#include "bts_def.h"
#include "bts_gatt_stru.h"
#include "bts_gatt_server.h"
#include "bts_le_gap.h"
#include "errcode.h"
#include "bts_device_manager.h"

#include "his_ble_func.h"
#include "global_macro.h"
/*  宏定义---------------------------------------------------------------------*/
/* gatt server id */
#define BLE_SERVER_ID 			1

/* octets of 16 bits */
#define LENGTH_16 					2

/* invalid server ID */
#define INVALID_SERVER_ID 			0

#define BLE_SERVICE_NUM 2

/* Service UUID */
#define BLE_UUID_SERVER_SERVICE 0x181C
/* Characteristic UUID */
#define BLE_CHARACTERISTIC_UUID_TX 0x2A90
/* Characteristic UUID */
#define BLE_CHARACTERISTIC_UUID_RX 0x2A8A
/* Client Characteristic Configuration UUID */
#define BLE_CLIENT_CHARACTERISTIC_CONFIGURATION 0x2902

/* Ble Adv type Flag length */
#define BLE_ADV_FLAG_LEN 0x03
/* Ble Adv data length */
#define BLE_GENERAL_BYTE_1 1
/* Ble Adv appearance length */
#define BLE_ADV_APPEARANCE_LENGTH 4
/* Ble Adv appearance type */
#define BLE_ADV_APPEARANCE_DATA_TYPE 0x19
/* Ble appearance type */
#define BLE_ADV_CATEGORY_VALUE 0x0080
/* Ble categorylength */
#define BLE_ADV_CATEGORY_LEN 2
/* Ble name adv param type length */
#define BLE_ADV_PARAM_DATATYPE_LENGTH 1
/* Ble name adv name type */
#define BLE_ADV_LOCAL_NAME_DATA_TYPE 0x09
/* Ble name adv tx power type */
#define BLE_ADV_TX_POWER_LEVEL       0x0A
/* Ble name adv tx power response type */
#define BLE_SCAN_RSP_TX_POWER_LEVEL_LEN 0x03
/* Ble adv flag data */
#define BLE_ADV_FLAG_DATA 0x05

/* Ble adv min interval */
#define BLE_ADV_MIN_INTERVAL 0x30
/* Ble adv max interval */
#define BLE_ADV_MAX_INTERVAL 0x60
/* Ble adv handle */
#define BTH_GAP_BLE_ADV_HANDLE_DEFAULT 0x01
/* Ble adv duration */
#define BTH_GAP_BLE_ADV_FOREVER_DURATION 0

#define MAX_NAME_LENGTH 26
#define EXT_ADV_OR_SCAN_RSP_DATA_LEN 251
#define ADV_APPEA_CATOGORY_HIGH 8

/*  全局变量---------------------------------------------------------------------*/
static uint16_t g_ble_conn_id = 0;
static uint8_t g_ble_name_value[] = { 'x', 'r', 's', '_', 'i', 'p', 'c', '\0' };
static uint8_t g_server_app_uuid[] = { 0x00, 0x00 };
static uint8_t g_ble_server_addr[] = { 0x11, 0x22, 0x33, 0x44, 0x55, 0x66 };
static uint8_t g_server_id = INVALID_SERVER_ID;
static uint16_t g_notify_indicate_handle = 0;
static uint8_t g_service_num = 0;
static bool g_ble_enable_stats = false;
static bool g_ble_adv_stats = false;

static pthread_mutex_t m_ParseBleDataLock = PTHREAD_MUTEX_INITIALIZER;

// 接口声明
static void ble_add_service(void);

/*  蓝牙是否运行---------------------------------------------------------------------*/
static int g_his_ble_is_running = 0;
static int g_his_ble_stop_in_progress = 0; //防止多次调用stop

//防止多线程同时调用start和stop
static pthread_mutex_t ble_mutex = PTHREAD_MUTEX_INITIALIZER;

// 设置 g_his_ble_is_running 的接口
static int his_set_ble_running(int value) 
{
    pthread_mutex_lock(&ble_mutex);
    if(g_his_ble_is_running == value)
    {
        pthread_mutex_unlock(&ble_mutex);
        return -1;
    }
    g_his_ble_is_running = value;
    pthread_mutex_unlock(&ble_mutex);
    return 0;
}

// 获取 g_his_ble_is_running 的接口
static int his_get_ble_running() 
{
    int value;
    pthread_mutex_lock(&ble_mutex);
    value = g_his_ble_is_running;
    pthread_mutex_unlock(&ble_mutex);
    return value;
}

/*  记录蓝牙启动时间---------------------------------------------------------------------*/
static time_t his_start_Ble_seconds = 0;

int his_getBleRunningTimeSec()
{
    if (his_start_Ble_seconds == 0)
    {
        return -1; // 如果 his_start_Ble_seconds 为 0，返回 -1
    } 
    else
    {
        time_t cur_tm = time(NULL); // 获取当前时间
        int running_time = (int)(cur_tm - his_start_Ble_seconds);
        return running_time; // 返回蓝牙启动到现在的时间（秒数）
    }
}

/*---------------------------------------------------------------------*/
static char g_local_name[MAX_NAME_LENGTH] = { 'H', 'I', '_', '1', '2', '3', '4', '5', '6' };

void his_setBluetoothName(const char* name) 
{
    if(name == NULL)
    {
        return;
    }
    size_t len = strlen(name);
    if (len > MAX_NAME_LENGTH)
    {
        len = MAX_NAME_LENGTH;
    }

    memset(g_local_name, 0, MAX_NAME_LENGTH);
    memcpy(g_local_name, name, len);
}

/*---------------------------------------------------------------------*/
static his_ble_data_callback pParseBleDataCallback = NULL;
int his_register_ble_callback(his_ble_data_callback pCallback)
{
    if (pCallback == NULL)
    {
        return -1;
    }
    pParseBleDataCallback = pCallback;
    return 0;
}

/*  全局变量操作接口---------------------------------------------------------------------*/
static bool g_connection_state = false;
static bool ble_get_connection_state(void)
{
    return g_connection_state;
}

static void ble_set_connection_state(bool value)
{
    if(g_connection_state == value)
    {
        return;
    }
    g_connection_state = value;
}

/*  是否收集配网内容---------------------------------------------------------------------*/
static bool g_info_complete = false;
static bool ble_get_info_complete(void)
{
    return g_info_complete;
}

static void ble_set_info_complete(bool value)
{
    if(g_info_complete == value)
    {
        return;
    }
    g_info_complete = value;
}

/*  adv层---------------------------------------------------------------------*/
typedef enum ble_adv_filter_policy {
    BLE_ADV_FILTER_POLICY_SCAN_ANY_CONNECT_ANY =                     0x00,
    BLE_ADV_FILTER_POLICY_SCAN_WHITE_LIST_CONNECT_ANY =              0x01,
    BLE_ADV_FILTER_POLICY_SCAN_ANY_CONNECT_WHITE_LIST =              0x02,
    BLE_ADV_FILTER_POLICY_SCAN_WHITE_LIST_CONNECT_WHITE_LIST =       0x03
} ble_adv_filter_policy_t;

typedef enum ble_adverting_type {
    BLE_ADV_TYPE_CONNECTABLE_UNDIRECTED =                            0x00,
    BLE_ADV_TYPE_CONNECTABLE_HIGH_DUTY_CYCLE_DIRECTED =              0x01,
    BLE_ADV_TYPE_SCANNABLE_UNDIRECTED =                              0x02,
    BLE_ADV_TYPE_NON_CONNECTABLE_UNDIRECTED =                        0x03,
    BLE_ADV_TYPE_CONNECTABLE_LOW_DUTY_CYCLE_DIRECTED =               0x04
} ble_adverting_type_t;

typedef enum ble_adv_channel_map {
    BLE_ADV_CHANNEL_MAP_CH_37 =                      0x01,
    BLE_ADV_CHANNEL_MAP_CH_38 =                      0x02,
    BLE_ADV_CHANNEL_MAP_CH_39 =                      0x04,
    BLE_ADV_CHANNEL_MAP_CH_37_CH_38 =                0x03,
    BLE_ADV_CHANNEL_MAP_CH_37_CH_39 =                0x05,
    BLE_ADV_CHANNEL_MAP_CH_38_CH_39 =                0x06,
    BLE_ADV_CHANNEL_MAP_CH_DEFAULT =                 0x07
} ble_adv_channel_map_t;

typedef struct {
    uint8_t length;             /* 广播数据类型长度 */
    uint8_t adv_data_type;      /* 广播数据类型 */
    uint8_t flags;              /* 广播数据标志 */
} ble_adv_flag;

typedef struct {
    uint8_t length;                             /* 设备外观数据类型长度 */
    uint8_t adv_data_type;                      /* 设备外观数据类型 */
    uint8_t catogory_id[BLE_ADV_CATEGORY_LEN];  /* 设备外观数据 */
} ble_appearance_t;

typedef struct {
    uint8_t length;             /* 广播设备名称类型长度 */
    uint8_t adv_data_type;      /* 设备名称类型 */
    int8_t *name;               /* 设备名称数据指针 */
} ble_local_name_t;

typedef struct {
    uint8_t length;             /* 广播发送功率长度 */
    uint8_t adv_data_type;      /* 广播发送数据类型 */
    uint8_t tx_power_value;     /* 广播发送数据 */
} ble_tx_power_level_t;

static inline uint8_t u16_low_u8(uint16_t val)
{
    return (uint8_t)((uint16_t)(val) & 0xff);
}

static inline uint8_t u16_high_u8(uint16_t val)
{
    return (uint8_t)(((uint16_t)(val) >> ADV_APPEA_CATOGORY_HIGH) & 0xff);
}

static uint8_t ble_set_adv_flag_data(uint8_t *set_adv_data_position, uint8_t max_len)
{
    ble_adv_flag adv_flags = {
        .length = BLE_ADV_FLAG_LEN - BLE_GENERAL_BYTE_1,
        .adv_data_type = 1,
        .flags = BLE_ADV_FLAG_DATA
    };
    if (memcpy_s(set_adv_data_position, max_len, &adv_flags, BLE_ADV_FLAG_LEN) != 0) {
        return 0;
    }
    return BLE_ADV_FLAG_LEN;
}

static uint8_t ble_set_adv_appearance(uint8_t *set_adv_data_position, uint8_t max_len)
{
    ble_appearance_t adv_appearance_data = {
        .length = BLE_ADV_APPEARANCE_LENGTH - BLE_GENERAL_BYTE_1,
        .adv_data_type = BLE_ADV_APPEARANCE_DATA_TYPE,
        .catogory_id = { u16_low_u8(BLE_ADV_CATEGORY_VALUE), u16_high_u8(BLE_ADV_CATEGORY_VALUE) }
    };
    if (memcpy_s(set_adv_data_position, max_len, &adv_appearance_data, BLE_ADV_APPEARANCE_LENGTH) != 0) {
        return 0;
    }
    return BLE_ADV_APPEARANCE_LENGTH;
}

#if 0   //为支持普通广播长字符串名称，屏蔽
static uint8_t ble_set_adv_name(uint8_t *set_adv_data_position, uint8_t max_len)
{
    uint8_t len = BLE_ADV_PARAM_DATATYPE_LENGTH + BLE_ADV_PARAM_DATATYPE_LENGTH;
    ble_local_name_t adv_local_name_data = {
        .length = (uint8_t)(BLE_ADV_PARAM_DATATYPE_LENGTH + sizeof(g_local_name)),
        .adv_data_type = BLE_ADV_LOCAL_NAME_DATA_TYPE
    };

    if (memcpy_s(set_adv_data_position, max_len, &adv_local_name_data, len) != 0)
    {
        return 0;
    }

    if (memcpy_s((set_adv_data_position+len), (size_t)(max_len-len), g_local_name, sizeof(g_local_name)) != 0)
    {
        return 0;
    }
    len = (uint8_t)(len + sizeof(g_local_name));
    return len;
}
#endif

static uint8_t ble_set_adv_appearance_data(uint8_t *set_adv_data_position, uint8_t max_len)
{
    uint8_t idx = 0;

    idx += ble_set_adv_appearance(set_adv_data_position, max_len);
    // idx += ble_set_adv_name(set_adv_data_position + idx, (max_len - idx));
    return idx;
}

static uint16_t ble_server_set_adv_data(uint8_t *set_adv_data, uint8_t adv_data_max_len)
{
    uint8_t idx = 0;

    if ((set_adv_data == NULL) || (adv_data_max_len == 0)) {
        return 0;
    }

    idx += ble_set_adv_flag_data(set_adv_data, adv_data_max_len);
    idx += ble_set_adv_appearance_data(&set_adv_data[idx], adv_data_max_len - idx);
    return idx;
}

static uint16_t ble_set_scan_response_data(uint8_t *scan_rsp_data, uint8_t scan_rsp_data_max_len)
{
    uint8_t idx = 0;

    if (scan_rsp_data == NULL || scan_rsp_data_max_len == 0) {
        return 0;
    }

    /* tx power level */
    ble_tx_power_level_t tx_power_level = {
        .length = BLE_SCAN_RSP_TX_POWER_LEVEL_LEN - BLE_GENERAL_BYTE_1,
        .adv_data_type = BLE_ADV_TX_POWER_LEVEL,
        .tx_power_value = 0
    };

    if (memcpy_s(scan_rsp_data, scan_rsp_data_max_len, &tx_power_level, sizeof(ble_tx_power_level_t)) != 0) {
        return 0;
    }

    idx += BLE_SCAN_RSP_TX_POWER_LEVEL_LEN;

    /* set local name */
    scan_rsp_data[idx++] = sizeof(g_local_name) + BLE_GENERAL_BYTE_1;
    scan_rsp_data[idx++] = BLE_ADV_LOCAL_NAME_DATA_TYPE;
    if ((idx + sizeof(g_local_name)) > scan_rsp_data_max_len) {
        return 0;
    }
    if (memcpy_s(&scan_rsp_data[idx], scan_rsp_data_max_len - idx, g_local_name,
                 sizeof(g_local_name)) != 0) {
        return 0;
    }
    idx += sizeof(g_local_name);
    return idx;
}

static uint8_t ble_set_adv_data(void)
{
    /* set adv data */
    uint8_t set_adv_data[EXT_ADV_OR_SCAN_RSP_DATA_LEN] = { 0 };
    uint16_t adv_data_len = ble_server_set_adv_data(set_adv_data, EXT_ADV_OR_SCAN_RSP_DATA_LEN);
    if ((adv_data_len > EXT_ADV_OR_SCAN_RSP_DATA_LEN) || (adv_data_len == 0)) {
        return 0;
    }

    /* set scan response data */
    uint8_t set_scan_rsp_data[EXT_ADV_OR_SCAN_RSP_DATA_LEN] = { 0 };
    uint16_t scan_rsp_data_len = ble_set_scan_response_data(set_scan_rsp_data, EXT_ADV_OR_SCAN_RSP_DATA_LEN);
    if ((scan_rsp_data_len > EXT_ADV_OR_SCAN_RSP_DATA_LEN) || (scan_rsp_data_len == 0)) {
        return 0;
    }

    gap_ble_config_adv_data_t cfg_adv_data = {0};
    cfg_adv_data.adv_data = set_adv_data;
    cfg_adv_data.adv_length = adv_data_len;

    cfg_adv_data.scan_rsp_data = set_scan_rsp_data;
    cfg_adv_data.scan_rsp_length = scan_rsp_data_len;
    HINF("ble_set_adv_data, adv_data_len: %hu, scan_rsp_data_len: %hu\n", adv_data_len, scan_rsp_data_len);
    return gap_ble_set_adv_data(BTH_GAP_BLE_ADV_HANDLE_DEFAULT, &cfg_adv_data);
}

static uint8_t ble_start_adv(void)
{
    errcode_t ret = ERRCODE_BT_SUCCESS;
    uint8_t adv_id = BTH_GAP_BLE_ADV_HANDLE_DEFAULT;

    gap_ble_adv_params_t adv_para = {
        .min_interval = BLE_ADV_MIN_INTERVAL,
        .max_interval = BLE_ADV_MAX_INTERVAL,
        .duration = BTH_GAP_BLE_ADV_FOREVER_DURATION,
        .peer_addr.type = BT_ADDRESS_TYPE_PUBLIC_DEVICE_ADDRESS,
        /* 广播通道选择bitMap, 可参考BleAdvChannelMap */
        .channel_map = BLE_ADV_CHANNEL_MAP_CH_DEFAULT,
        .adv_type = BLE_ADV_TYPE_CONNECTABLE_UNDIRECTED,
        .adv_filter_policy = BLE_ADV_FILTER_POLICY_SCAN_ANY_CONNECT_ANY
    };

    (void)memset_s(&adv_para.peer_addr.addr, BD_ADDR_LEN, 0, BD_ADDR_LEN);
    HINF("ble_start_adv adv_id %hhu\n", adv_id);
    ret |= gap_ble_set_adv_param(adv_id, &adv_para);
    ret |= gap_ble_start_adv(adv_id);
    return ret;
}

static void print_hex(uint8_t *buffer, uint16_t len, char *flag)
{
    int i;
    printf("---code [%s][", flag);
    for(i = 0; i < len; i++){
        printf("0x%02hhX ",buffer[i]);
    }
    printf("]---\n");
}

/*  获取 wlan0 网口的 MAC 地址---------------------------------------------------------------------*/
static int get_mac_address(const char *if_name, uint8_t *mac_address) 
{
    struct ifreq ifr;

    int sock = -1; 
    int ret  = 0;

    unsigned char *ptr = NULL;

    if ((!if_name) || (!mac_address))
    {
        return -1;
    }
    
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) 
    {
        return -1;
    }
    
    strncpy(ifr.ifr_name, if_name, 16);
    if (ioctl(sock, SIOCGIFHWADDR, &ifr) >= 0)
    {
        ptr = (uint8_t *) & ifr.ifr_ifru.ifru_hwaddr.sa_data[0];
        if (memcpy_s(mac_address, BD_ADDR_LEN, ptr, BD_ADDR_LEN) != 0) 
        {
            HERR("get_mac_address memcpy failed\n");
            ret = -1;
        }
    }
    else
    {
        ret = -1;
    }

    close(sock);

    return ret;
}

/*  gatts 回发数据---------------------------------------------------------------------*/
/* device向host发送数据：input report */
static errcode_t ble_server_send_report(uint8_t *data, uint16_t len)
{
    printf("\n");
    gatts_ntf_ind_t param = { 0 };
    uint16_t conn_id = g_ble_conn_id;
    param.attr_handle = g_notify_indicate_handle;
    param.value_len = len;
    param.value = data;
    HINF("send report, conn_id: %hu, indicate_handle: %hu, data[%s], len: %hu\n", g_ble_conn_id, g_notify_indicate_handle, data, len);
    print_hex(data, len, "send notify");
    
    errcode_t ret = gatts_notify_indicate(BLE_SERVER_ID, conn_id, &param);
    if (ret != ERRCODE_BT_SUCCESS) 
    {
        HERR("ble_server_send_report,gatts_notify_indicate fail, ret = %d\n", ret);
        return ERRCODE_BT_FAIL;
    }

    return ERRCODE_BT_SUCCESS;
}
/*---------------------------------------------------------------------*/
#undef MAX
#define MAX(a,b)	(((a) > (b)) ? (a) : (b))
#undef MIN
#define MIN(a,b)	(((a) < (b)) ? (a) : (b))

/* CRC16 余式表 */
static uint16_t crctalbeabs[] = { 
	0x0000, 0xCC01, 0xD801, 0x1400, 0xF001, 0x3C00, 0x2800, 0xE401, 
	0xA001, 0x6C00, 0x7800, 0xB401, 0x5000, 0x9C01, 0x8801, 0x4400 
};

/*!
 *  功  能: CRC16校验
 *  param1: 指向要校验的数据的指针
 *  param2: 要校验的数据的长度
 *  retval: 校验所得到的值，uint16_t 类型
 * 
 *  说  明: 本次CRC校验为查表法，多项式为 x16+x15+x2+1(0x8005)，CRC的初始值为0xFFFF
 */
static uint16_t Crc16(uint8_t *ptr, uint32_t len) 
{
	uint16_t crc = 0xffff; 
	uint32_t i;
	uint8_t ch;
 
	for (i = 0; i < len; i++) {
		ch = *ptr++;
		crc = crctalbeabs[(ch ^ crc) & 15] ^ (crc >> 4);
		crc = crctalbeabs[((ch >> 4) ^ crc) & 15] ^ (crc >> 4);
	} 
	
	return crc;
}

//蓝牙数据包结构体
typedef struct {
    uint8_t magic_byte; //Magic byte,有效值为0xAB
    uint8_t flag_info;  //2 bits Reserve,1 bits Err flag,1 bits Ack flag,4 bits Version
    uint8_t payload_length[2]; //Payload length
    uint8_t crc16[2];
    uint8_t Sequence_id[2];
}__attribute__((packed)) L1_HEADER;   

typedef struct {
    uint8_t command_id; //Command id
    uint8_t Reserve;  //Reserve
}__attribute__((packed)) L2_HEADER; 

#define MTU_SIZE            (128)                           //128
#define L1_HEADER_LENTH     (sizeof(L1_HEADER))             //8
#define L1_BODY_LENTH       (MTU_SIZE - L1_HEADER_LENTH)    //120
#define L2_HEADER_LENTH     (sizeof(L2_HEADER))             //2
#define L2_BODY_LENTH       (L1_BODY_LENTH - L2_HEADER_LENTH)    //118
#define KEY_LENTH           (1)
#define KEY_HEADER_LENTH    (2)
#define KEY_BODY_MAX_LENTH  (L2_BODY_LENTH - KEY_LENTH - KEY_LENTH)

static void CheckAndParseBleData(uint8_t *pDataBuf, uint16_t uiDataLen)
{
    if (pParseBleDataCallback == NULL)
    {
        HERR("pParseBleDataCallback == NULL");
        return;
    }

    if (uiDataLen < L1_HEADER_LENTH)
    {
        HERR("The packet[%hu] is too short !\n",uiDataLen);
        return;
    }

    pthread_mutex_lock(&m_ParseBleDataLock);
    //验证数据包
    //L1
    uint8_t L1_packet[MTU_SIZE] = {0};
    memcpy(L1_packet, pDataBuf, MIN(uiDataLen,MTU_SIZE));

    L1_HEADER L1_header = {0};
    memcpy(&L1_header, L1_packet, L1_HEADER_LENTH);

    //拆L1头
    uint8_t magic_byte = L1_header.magic_byte;
    uint8_t Err_flag   = L1_header.flag_info & 0x20;
    uint8_t Ack_flag   = L1_header.flag_info & 0x10;
    uint8_t Version    = L1_header.flag_info & 0x0F;
    uint16_t payload_length = ((uint16_t)L1_header.payload_length[0] << 8) | (uint16_t)L1_header.payload_length[1];
    uint16_t crc16          = ((uint16_t)L1_header.crc16[0] << 8) | (uint16_t)L1_header.crc16[1];
    uint16_t Sequence_id    = ((uint16_t)L1_header.Sequence_id[0] << 8) | (uint16_t)L1_header.Sequence_id[1];

    HINF("L1 magic_byte: 0x%02X \n",magic_byte);
    HINF("L1 Err_flag:   0x%02X \n",Err_flag);
    HINF("L1 Ack_flag:   0x%02X \n",Ack_flag);
    HINF("L1 Version:    0x%02X \n",Version);
    HINF("L1 payload_length: 0x%04X \n",payload_length);
    HINF("L1 crc16:          0x%04X \n",crc16);
    HINF("L1 Sequence_id:    0x%04X \n",Sequence_id);

    if ((magic_byte != 0xAB) || (Version != 0x00) || (payload_length <= L2_HEADER_LENTH)) 
    {
        HERR("L1 Header Is Error\n");
        goto L1HeaderIsError;
    }

    //生成不带crc的L1整体
    memset(L1_header.crc16, 0, sizeof(L1_header.crc16));
    memcpy(L1_packet, &L1_header, L1_HEADER_LENTH);

    uint16_t crc_L1 = Crc16(L1_packet, MIN(uiDataLen,MTU_SIZE));
    if (crc_L1 != crc16) 
    {
        HERR("L1 crc_L1:          0x%04X \n",crc_L1);
        goto Crc16IsError;
    }

    //L2
    L2_HEADER L2_header = {0};
    memcpy(&L2_header, L1_packet + L1_HEADER_LENTH, L2_HEADER_LENTH);

    uint8_t command_id = L2_header.command_id;
    HINF("L1 command_id: 0x%02X \n",command_id);

    uint16_t L2_body_length = payload_length - L2_HEADER_LENTH;
    if (L2_body_length < (KEY_LENTH + KEY_HEADER_LENTH)) 
    {
        HERR("The L2_body[%hu] is too short !\n",L2_body_length);
        goto L2BodyIsShort;
    }

    uint8_t L2_body[L2_BODY_LENTH] = {0};
    uint8_t *p_L2_body = (uint8_t *)L2_body;
    memcpy(p_L2_body, L1_packet + L1_HEADER_LENTH + L2_HEADER_LENTH, L2_body_length);

    //key
    uint8_t Key_name = 0;
    uint8_t Key_header[KEY_HEADER_LENTH] = {0};
    uint8_t Key_body[KEY_BODY_MAX_LENTH] = {0};
    uint16_t Key_body_length = 0;

    memcpy(&Key_name, p_L2_body, KEY_LENTH);
    memcpy(Key_header, p_L2_body+KEY_LENTH, KEY_HEADER_LENTH);

    Key_body_length = Key_header[1] & 0x01FF;
    Key_body_length = ((uint16_t)(Key_header[0]& 0x01) << 8) | (uint16_t)Key_header[1];
    memcpy(Key_body, p_L2_body + KEY_LENTH + KEY_HEADER_LENTH, Key_body_length);

    HINF("Key_name: 0x%02X \n",Key_name);
    HINF("Key_body_length: 0x%04X \n",Key_body_length);
    HINF("Key_body: [%s] \n",Key_body);
    print_hex(Key_body, Key_body_length, "Key_body");
    uint16_t crc_return = 0;

    //成功拿到了数据
    if (command_id == 0x02)
    {
        //获取设备信息
        if (Key_name == 0x01)
        {
            //回个收到
            L1_packet[0] = 0xAB;
            L1_packet[1] = 0x10;
            L1_packet[2] = 0x00;
            L1_packet[3] = 0x09;
            memset(L1_packet + L1_HEADER_LENTH + L2_HEADER_LENTH + KEY_LENTH, 0, L2_BODY_LENTH - KEY_LENTH);
            L1_packet[12] = 0x04;
            crc_return = Crc16(L1_packet, L1_HEADER_LENTH + L2_HEADER_LENTH + KEY_LENTH + KEY_HEADER_LENTH + 4);
            L1_packet[4] = (uint8_t) ((crc_return >> 8) & 0xFF);
            L1_packet[5] = (uint8_t) (crc_return & 0xFF);
            
            ble_server_send_report( L1_packet, (L1_HEADER_LENTH + L2_HEADER_LENTH + KEY_LENTH + KEY_HEADER_LENTH + 4));
            usleep(200 * 1000);

            //回个版本号
            L1_packet[0] = 0xAB;
            L1_packet[1] = 0x00;
            L1_packet[2] = 0x00;
            L1_packet[3] = 0x06;
            L1_packet[4] = 0x00;
            L1_packet[5] = 0x00;
            L1_packet[6] = 0x00;
            L1_packet[7] = L1_packet[7] + 1;

            L1_packet[8] = 0x01;
            L1_packet[9] = 0x00;

            L1_packet[10] = 0x02;
            L1_packet[11] = 0x00;
            L1_packet[12] = 0x01;
            L1_packet[13] = 0x00;

            crc_return = Crc16(L1_packet, L1_HEADER_LENTH + L2_HEADER_LENTH + KEY_LENTH + KEY_HEADER_LENTH + 1);
            L1_packet[4] = (uint8_t) ((crc_return >> 8) & 0xFF);
            L1_packet[5] = (uint8_t) (crc_return & 0xFF);

            ble_server_send_report( L1_packet, (L1_HEADER_LENTH + L2_HEADER_LENTH + KEY_LENTH + KEY_HEADER_LENTH + 1));
            
            pthread_mutex_unlock(&m_ParseBleDataLock);
            return;
        }
        else if (Key_name == 0x03) //发送是否支持5G双频信息
        {
            //回个收到
            L1_packet[0] = 0xAB;
            L1_packet[1] = 0x10;
            L1_packet[2] = 0x00;
            L1_packet[3] = 0x09;
            memset(L1_packet + L1_HEADER_LENTH + L2_HEADER_LENTH + KEY_LENTH, 0, L2_BODY_LENTH - KEY_LENTH);
            L1_packet[12] = 0x04;
            crc_return = Crc16(L1_packet, L1_HEADER_LENTH + L2_HEADER_LENTH + KEY_LENTH + KEY_HEADER_LENTH + 4);
            L1_packet[4] = (uint8_t) ((crc_return >> 8) & 0xFF);
            L1_packet[5] = (uint8_t) (crc_return & 0xFF);
            
            ble_server_send_report( L1_packet, (L1_HEADER_LENTH + L2_HEADER_LENTH + KEY_LENTH + KEY_HEADER_LENTH + 4));
            usleep(200 * 1000);
            //回是否支持5G WiFi双频蓝牙
            L1_packet[0] = 0xAB;
            L1_packet[1] = 0x00;
            L1_packet[2] = 0x00;
            L1_packet[3] = 0x06;
            L1_packet[4] = 0x00;
            L1_packet[5] = 0x00;
            L1_packet[6] = 0x00;
            L1_packet[7] = L1_packet[7] + 1;

            L1_packet[8] = 0x02;
            L1_packet[9] = 0x00;

            L1_packet[10] = 0x04;
            L1_packet[11] = 0x00;
            L1_packet[12] = 0x01;
            L1_packet[13] = 0x00;

            crc_return = Crc16(L1_packet, L1_HEADER_LENTH + L2_HEADER_LENTH + KEY_LENTH + KEY_HEADER_LENTH + 1);
            L1_packet[4] = (uint8_t) ((crc_return >> 8) & 0xFF);
            L1_packet[5] = (uint8_t) (crc_return & 0xFF);

            ble_server_send_report( L1_packet, (L1_HEADER_LENTH + L2_HEADER_LENTH + KEY_LENTH + KEY_HEADER_LENTH + 1));
            
            pthread_mutex_unlock(&m_ParseBleDataLock);
            return;
        }
        else
        {
            goto UnsupportCommand;
        }
    }
    else if (command_id == 0x01)
    {
        //设置设备信息
        if (Key_name != 0x01 && Key_name != 0x02 && Key_name != 0x03)
        {
            goto UnsupportCommand;
        }

        int ret = pParseBleDataCallback( Key_body, Key_body_length, Key_name);
        if (ret == -1)
        {
            //返回失败回复
            L1_packet[0] = 0xAB;
            L1_packet[1] = 0x30;
            L1_packet[2] = 0x00;
            L1_packet[3] = 0x09;
            memset(L1_packet + L1_HEADER_LENTH + L2_HEADER_LENTH + KEY_LENTH, 0, L2_BODY_LENTH - KEY_LENTH);
            L1_packet[12] = 0x04;
            L1_packet[16] = 0x01;
            crc_return = Crc16(L1_packet, L1_HEADER_LENTH + L2_HEADER_LENTH + KEY_LENTH + KEY_HEADER_LENTH + 4);
            L1_packet[4] = (uint8_t) ((crc_return >> 8) & 0xFF);
            L1_packet[5] = (uint8_t) (crc_return & 0xFF);

            ble_server_send_report( L1_packet, (L1_HEADER_LENTH + L2_HEADER_LENTH + KEY_LENTH + KEY_HEADER_LENTH + 4));

            pthread_mutex_unlock(&m_ParseBleDataLock);
            return;
        }
        else
        {
            //返回成功回复
            L1_packet[0] = 0xAB;
            L1_packet[1] = 0x10;
            L1_packet[2] = 0x00;
            L1_packet[3] = 0x09;
            memset(L1_packet + L1_HEADER_LENTH + L2_HEADER_LENTH + KEY_LENTH, 0, L2_BODY_LENTH - KEY_LENTH);
            L1_packet[12] = 0x04;
            crc_return = Crc16(L1_packet, L1_HEADER_LENTH + L2_HEADER_LENTH + KEY_LENTH + KEY_HEADER_LENTH + 4);
            L1_packet[4] = (uint8_t) ((crc_return >> 8) & 0xFF);
            L1_packet[5] = (uint8_t) (crc_return & 0xFF);

            ble_server_send_report( L1_packet, (L1_HEADER_LENTH + L2_HEADER_LENTH + KEY_LENTH + KEY_HEADER_LENTH + 4));

            pthread_mutex_unlock(&m_ParseBleDataLock);
            if (ret == 1)
            {
                ble_set_info_complete(true);
            }

            return;
        }
    }
    else
    {
        //不支持的字段
        goto UnsupportCommand;
    }
L1HeaderIsError://头重信息错了，回一下
Crc16IsError:   //crc错了，回一下
L2BodyIsShort:  //没能拿到数据，回一下
UnsupportCommand:
    L1_packet[0] = 0xAB;
    L1_packet[1] = 0x30;
    
    L1_packet[2] = 0x00;
    L1_packet[3] = 0x05;

    memset(L1_packet + L1_HEADER_LENTH + L2_HEADER_LENTH + KEY_LENTH, 0, L2_BODY_LENTH - KEY_LENTH);
    crc_return = Crc16(L1_packet, L1_HEADER_LENTH + L2_HEADER_LENTH + KEY_LENTH + KEY_HEADER_LENTH);

    L1_packet[4] = (uint8_t) ((crc_return >> 8) & 0xFF);
    L1_packet[5] = (uint8_t) (crc_return & 0xFF);

    ble_server_send_report( L1_packet, (L1_HEADER_LENTH + L2_HEADER_LENTH + KEY_LENTH + KEY_HEADER_LENTH));

    pthread_mutex_unlock(&m_ParseBleDataLock);
    return;
}

/*  gap 回调处理---------------------------------------------------------------------*/
static pthread_t _g_thread_ble_loop = 0;

static void *his_ble_loop(void *Param)
{
    PRT_TID();

    HINF("his_ble_loop start, ble start\n");
    while (1)
    {
        usleep(100 * 1000);

        if (ble_get_info_complete() == true)
        {
            ble_set_info_complete(false);
            if (pParseBleDataCallback != NULL)
            {
                pParseBleDataCallback( NULL, 0, 0x00);
                his_ble_stop();

                HINF("his_ble_loop end, info complete\n");
                break;
            }
        }

        if (g_ble_enable_stats == false)
        {
            HINF("his_ble_loop end, ble stop\n");
            break;
        }
    }

    HINF("his_ble_loop end\n");
    _g_thread_ble_loop = 0;
    return NULL;
}

static void ble_server_enable_cbk(uint8_t status)
{
    HINF("ble_enable cbk, status: %hhu\n", status);
    g_ble_enable_stats = true;

    if (_g_thread_ble_loop == 0)
    {
        pthread_create(&_g_thread_ble_loop, NULL, his_ble_loop, (void *)NULL);
        pthread_detach(_g_thread_ble_loop);
    }

    HINF("set local name\n");
    errcode_t ret = gap_ble_set_local_name(g_ble_name_value, sizeof(g_ble_name_value));
    if (ret != ERRCODE_BT_SUCCESS) 
    {
        HERR("ble_server_init,gap_ble_set_local_name fail, ret = %d\n", ret);
        return;
    }

    bd_addr_t ble_addr = { 0 };
    ble_addr.type = BT_ADDRESS_TYPE_PUBLIC_DEVICE_ADDRESS;
    uint8_t mac_address[BD_ADDR_LEN] = {0};
    ret = get_mac_address("wlan0", mac_address);
    if (ret != 0)
    {
        HERR("wlan0 not found, Using default BLE MAC address: %02x:%02x:%02x:%02x:%02x:%02x\n", 
                        g_ble_server_addr[0], g_ble_server_addr[1], g_ble_server_addr[2], g_ble_server_addr[3], g_ble_server_addr[4], g_ble_server_addr[5]);
        if (memcpy_s(ble_addr.addr, BD_ADDR_LEN, g_ble_server_addr, sizeof(g_ble_server_addr)) != 0) 
        {
            HERR("add server app addr memcpy failed\n");
            return;
        }
    }
    else
    {
        if (mac_address[BD_ADDR_LEN-1] == 0xFF)
        {
            mac_address[BD_ADDR_LEN-1] = 0x00;
        }
        else 
        {
            mac_address[BD_ADDR_LEN-1] += 0x01;
        }

        HERR("wlan0 found, BLE MAC address: %02x:%02x:%02x:%02x:%02x:%02x\n", 
                        mac_address[0], mac_address[1], mac_address[2], mac_address[3], mac_address[4], mac_address[5]);


        if (memcpy_s(ble_addr.addr, BD_ADDR_LEN, mac_address, BD_ADDR_LEN) != 0) 
        {
            HERR("add server app addr memcpy failed\n");
            return;
        }
    }

    HINF("set local addr\n");
    ret = gap_ble_set_local_addr(&ble_addr);
    if (ret != ERRCODE_BT_SUCCESS) 
    {
        HERR("ble_server_init,gap_ble_set_local_addr fail, ret = %d\n", ret);
        return;
    }

    ret = gap_ble_set_local_appearance((uint16_t) GAP_BLE_APPEARANCE_TYPE_UNKNOWN);
    if (ret != ERRCODE_BT_SUCCESS) 
    {
        HERR("ble_server_init,gap_ble_set_local_appearance fail, ret = %d\n", ret);
        return;
    }

    bt_uuid_t app_uuid = { 0 };
    app_uuid.uuid_len = sizeof(g_server_app_uuid);
    if (memcpy_s(app_uuid.uuid, app_uuid.uuid_len, g_server_app_uuid, sizeof(g_server_app_uuid)) != 0) 
    {
        HERR("add server app uuid memcpy failed\n");
        return;
    }

    HINF("register server\n");
    ret = gatts_register_server(&app_uuid, &g_server_id);
    if ((ret != ERRCODE_BT_SUCCESS) || (g_server_id == INVALID_SERVER_ID)) 
    {
        HERR("ble_server_init,gatts_register_server fail, ret = %d\n", ret);
    }
    HINF("register server g_server_id : %hhu\n", g_server_id);

    HINF("add service\n");
    ble_add_service(); /* 添加服务 */
}

static void ble_server_disable_cbk(uint8_t status)
{
    HINF("ble_disable cbk, status: %hhu\n", status);
    g_ble_enable_stats = false;
}

static void ble_server_set_adv_data_cbk(uint8_t adv_id, errcode_t status)
{
    HINF("set_adv_data cbk, adv_id: %hhu, status: %d\n", adv_id, status);
}

static void ble_server_set_adv_param_cbk(uint8_t adv_id, errcode_t status)
{
    HINF("set_adv_param cbk, adv_id: %hhu, status: %d\n", adv_id, status);
}

static void ble_server_adv_enable_cbk(uint8_t adv_id, adv_status_t status)
{
    HINF("adv enable cbk, adv_id: %hhu, status: %d\n", adv_id, status);
    g_ble_adv_stats = true;
}

static void ble_server_connect_change_cbk(uint16_t conn_id, bd_addr_t *addr, gap_ble_conn_state_t conn_state,
                                        gap_ble_pair_state_t pair_state, gap_ble_disc_reason_t disc_reason)
{
    g_ble_conn_id = conn_id;
    HINF("connect state change cbk, conn_id: %hu, status: %d, pair_status:%#x, addr[%02X:%02X:%02X:%02X:%02X:%02X] disc_reason %#x\n",
                                    conn_id, conn_state, pair_state, 
                                    addr->addr[0], addr->addr[1], addr->addr[2], addr->addr[3], addr->addr[4], addr->addr[5],
                                    disc_reason);

    if ((conn_state == GAP_BLE_STATE_DISCONNECTED)) 
    {
        ble_set_connection_state(false);
        if (disc_reason != GAP_BLE_CONN_TERMINATE_BY_LOCAL_HOST) 
        {
            ble_set_adv_data();
            ble_start_adv();
        }
    } 
    else if (conn_state == GAP_BLE_STATE_CONNECTED)
    {
        ble_set_connection_state(true);
    }
}

static void ble_server_adv_stop_cbk(uint8_t adv_id, adv_status_t status)
{
    HINF("adv stop cbk, adv_id: %hhu, status:%d\n", adv_id, status);
    g_ble_adv_stats = false;
}

static void ble_server_pair_result_cb(uint16_t conn_id, const bd_addr_t *addr, errcode_t status)
{
    HINF("pair result cbk, conn_id: %hu, status: %d, addr[%02X:%02X:%02X:%02X:%02X:%02X]\n", conn_id, status, 
                addr->addr[0], addr->addr[1], addr->addr[2], addr->addr[3], addr->addr[4], addr->addr[5]);
}

static void ble_server_adv_terminate_cbk(uint8_t adv_id, adv_status_t status)
{
    HINF("adv terminate cbk, adv_id: %hhu, status: %d\n", adv_id, status);
}

/*  gatts 回调处理---------------------------------------------------------------------*/
/* 将uint16的uuid数字转化为bt_uuid_t */
static void bts_data_to_uuid_len2(uint16_t uuid_data, bt_uuid_t *out_uuid)
{
    out_uuid->uuid_len = LENGTH_16;
    out_uuid->uuid[0] = (uint8_t)(uuid_data >> 8); /* 8: octet bit num */
    out_uuid->uuid[1] = (uint8_t)(uuid_data);
}

/* 添加发送服务的所有特征和描述符 */
static void ble_add_tx_characters_and_descriptors(uint8_t server_id, uint16_t srvc_handle)
{
    HINF("TX characters, server_id: %hhu, srv_handle: %hu \n", server_id, srvc_handle);
    bt_uuid_t characters_uuid = { 0 };
    uint8_t characters_value[] = { 0x54, 0x58, 0x63};
    bts_data_to_uuid_len2(BLE_CHARACTERISTIC_UUID_TX, &characters_uuid);
    gatts_add_chara_info_t character;
    gatts_add_character_result_t result = { 0 };
    uint16_t handle = 0;
    character.chara_uuid = characters_uuid;
    character.properties = GATT_CHARACTER_PROPERTY_BIT_NOTIFY | GATT_CHARACTER_PROPERTY_BIT_READ;
    character.permissions = GATT_ATTRIBUTE_PERMISSION_READ | GATT_ATTRIBUTE_PERMISSION_WRITE;
    character.value_len = sizeof(characters_value);
    character.value = characters_value;
    gatts_add_characteristic_sync(server_id, srvc_handle, &character, &result);
    HINF("characters uuid: %02x %02x\n", characters_uuid.uuid[0], characters_uuid.uuid[1]);
    HINF("ble_add_tx_characters_and_descriptors:characters handle: %d value_hdl: %d\n", result.handle, result.value_handle);

    g_notify_indicate_handle = result.value_handle;

    static uint8_t ccc_val[] = { 0x54, 0x58, 0x64 }; // notify
    bt_uuid_t ccc_uuid = { 0 };
    bts_data_to_uuid_len2(BLE_CLIENT_CHARACTERISTIC_CONFIGURATION, &ccc_uuid);
    gatts_add_desc_info_t descriptor;
    descriptor.desc_uuid = ccc_uuid;
    descriptor.permissions = GATT_ATTRIBUTE_PERMISSION_READ | GATT_ATTRIBUTE_PERMISSION_WRITE;    
    descriptor.value_len = sizeof(ccc_val);
    descriptor.value = ccc_val;
    gatts_add_descriptor_sync(server_id, srvc_handle, &descriptor, &handle);
    HINF("descriptor uuid: %02x %02x\n", ccc_uuid.uuid[0], ccc_uuid.uuid[1]);
    HINF("ble_add_tx_characters_and_descriptors:hdl: %d\n", handle);
}

/* 添加接收服务的所有特征和描述符 */
static void ble_add_rx_characters_and_descriptors(uint8_t server_id, uint16_t srvc_handle)
{
    HINF("RX characters, server_id: %hhu, srv_handle: %hu \n", server_id, srvc_handle);
    bt_uuid_t characters_uuid = { 0 };
    uint8_t characters_value[] = { 0x52, 0x58, 0x63 };
    bts_data_to_uuid_len2(BLE_CHARACTERISTIC_UUID_RX, &characters_uuid);
    gatts_add_chara_info_t character;
    gatts_add_character_result_t result = { 0 };
    uint16_t handle = 0;
    character.chara_uuid = characters_uuid;
    character.properties = GATT_CHARACTER_PROPERTY_BIT_READ | GATT_CHARACTER_PROPERTY_BIT_WRITE_NO_RSP;
    character.permissions = GATT_ATTRIBUTE_PERMISSION_READ | GATT_ATTRIBUTE_PERMISSION_WRITE;
    character.value_len = sizeof(characters_value);
    character.value = characters_value;

    gatts_add_characteristic_sync(server_id, srvc_handle, &character, &result);
    HINF("characters uuid: %02x %02x\n", characters_uuid.uuid[0], characters_uuid.uuid[1]);
    HINF("ble_add_rx_characters_and_descriptors: characters handle: %d value_hdl: %d\n", result.handle, result.value_handle);

    bt_uuid_t ccc_uuid = { 0 };
    /* client characteristic configuration value for test */
    static uint8_t ccc_val[] = { 0x52, 0x58, 0x64 };
    bts_data_to_uuid_len2(BLE_CLIENT_CHARACTERISTIC_CONFIGURATION, &ccc_uuid);
    gatts_add_desc_info_t descriptor;
    descriptor.desc_uuid = ccc_uuid;
    descriptor.permissions = GATT_ATTRIBUTE_PERMISSION_READ | GATT_ATTRIBUTE_PERMISSION_WRITE;
    descriptor.value_len = sizeof(ccc_val);
    descriptor.value = ccc_val;
    gatts_add_descriptor_sync(server_id, srvc_handle, &descriptor, &handle);
    HINF("descriptor ccc_uuid: %02x %02x\n", ccc_uuid.uuid[0], ccc_uuid.uuid[1]);
    HINF("ble_add_rx_characters_and_descriptors:hdl: %d\n", handle);
}

/* 服务删除回调 */
static void ble_server_service_delete_all_cbk(uint8_t server_id, errcode_t status)
{
    HINF("delete all characters_and_descriptors cbk, service: %hhu, status: %d\n", server_id, status);
    g_service_num = 0;
    gatts_unregister_server(server_id);
}

/* 开始服务回调 */
static void ble_server_service_start_cbk(uint8_t server_id, uint16_t handle, errcode_t status)
{
    g_service_num++;
    HINF("service start cbk, service: %hhu, service_hdl: %hu, status: %d\n", server_id, handle, status);
    HINF("service start cbk, g_service_num: %hhu\n", g_service_num);

    if ((g_service_num == BLE_SERVICE_NUM) && (status == 0))
    {
        HINF("service start cbk,start adv\n");
        ble_set_adv_data();
        ble_start_adv();
    }
}

/* 结束服务回调 */
static void ble_server_service_stop_cbk(uint8_t server_id, uint16_t handle, errcode_t status)
{
    g_service_num--;
    HINF("service stop cbk, service: %hhu, service_hdl: %hu, status: %d\n", server_id, handle, status);
    HINF("service stop cbk, g_service_num: %hhu\n", g_service_num);

    if ((g_service_num == 0) && (status == 0))
    {
        gatts_delete_all_services(server_id);
    }
}

static void ble_receive_read_req_cbk(uint8_t server_id, uint16_t conn_id, gatts_req_read_cb_t *read_cb_para, errcode_t status)
{
    printf("\n");
    HINF("ble read cbk, server_id: %hhu, conn_id: %hu, status: %d\n", server_id, conn_id, status);
    HINF("ble read cbk, req_id: %hu, att_handle: %hu, offset: %hu, need_rsp: %d, is_long: %d\n",
                        read_cb_para->request_id, read_cb_para->handle, read_cb_para->offset,
                        read_cb_para->need_rsp, read_cb_para->is_long);
}

static void ble_receive_write_req_cbk(uint8_t server_id, uint16_t conn_id, gatts_req_write_cb_t *write_cb_para, errcode_t status)
{
    printf("\n");
    HINF("ble write cbk, server_id: %hhu, conn_id: %hu, status: %d, value[%s], len: %hu, need_rsp: %d\n", 
                server_id, conn_id, status, write_cb_para->value, write_cb_para->length, write_cb_para->need_rsp);

    print_hex(write_cb_para->value, write_cb_para->length, "ble write cbk");
    if (GAP_BLE_STATE_CONNECTED == ble_get_connection_state())
    {
        CheckAndParseBleData( write_cb_para->value, write_cb_para->length);
    }
}

static void ble_mtu_changed_cbk(uint8_t server_id, uint16_t conn_id, uint16_t mtu_size, errcode_t status)
{
    HINF("Mtu Changed cbk, server_id: %hhu, conn_id: %hu, mtusize: %hu, status: %d\n", server_id, conn_id, mtu_size, status);
}

static errcode_t ble_dev_manager_callbacks()
{
    bts_dev_manager_callbacks_t dev_mgr = {0};
    dev_mgr.ble_enable_cb            = ble_server_enable_cbk;
    dev_mgr.ble_disable_cb           = ble_server_disable_cbk;

    errcode_t ret = bts_dev_manager_register_callbacks(&dev_mgr);
    if (ret != ERRCODE_BT_SUCCESS) 
    {
        HERR("reg dev manager cbk failed, ret = %d\n", ret);
        return ret;
    }

    return ERRCODE_BT_SUCCESS;
}

static errcode_t ble_server_register_callbacks()
{
    gap_ble_callbacks_t gap_cb = { 0 };
    gap_cb.set_adv_data_cb          = ble_server_set_adv_data_cbk;
    gap_cb.set_adv_param_cb         = ble_server_set_adv_param_cbk;
    gap_cb.start_adv_cb             = ble_server_adv_enable_cbk;
    gap_cb.stop_adv_cb              = ble_server_adv_stop_cbk;
    gap_cb.conn_state_change_cb     = ble_server_connect_change_cbk;
    gap_cb.pair_result_cb           = ble_server_pair_result_cb;
    gap_cb.terminate_adv_cb         = ble_server_adv_terminate_cbk;
    gap_cb.auth_complete_cb         = NULL;
    gap_cb.conn_param_update_cb     = NULL;

    errcode_t ret = gap_ble_register_callbacks(&gap_cb);
    if (ret != ERRCODE_BT_SUCCESS) 
    {
        HERR("reg gap cbk failed, ret = %d\n", ret);
        return ret;
    }

    gatts_callbacks_t service_cb = { 0 };
    service_cb.start_service_cb         = ble_server_service_start_cbk;
    service_cb.stop_service_cb          = ble_server_service_stop_cbk;
    service_cb.delete_service_cb        = ble_server_service_delete_all_cbk;
    service_cb.read_request_cb          = ble_receive_read_req_cbk;
    service_cb.write_request_cb         = ble_receive_write_req_cbk;
    service_cb.mtu_changed_cb           = ble_mtu_changed_cbk;

    ret = gatts_register_callbacks(&service_cb);
    if (ret != ERRCODE_BT_SUCCESS) 
    {
        HERR("reg gatts service cbk failed, ret = %d\n", ret);
        return ret;
    }

    return ret;
}

/*  内部调用---------------------------------------------------------------------*/
static void ble_server_init()
{
    HINF("register callbacks\n");
    ble_server_register_callbacks();
    ble_dev_manager_callbacks();

    ble_set_connection_state(false);

    HINF("enable ble\n");
    errcode_t ret = enable_ble();
    if (ret != ERRCODE_BT_SUCCESS) 
    {
        HERR("ble_server_init,enable_ble fail, ret = %d\n", ret);
        return;
    }

    return;
}

/* 创建服务 */
static void ble_add_service(void)
{
    bt_uuid_t service_uuid = { 0 };
    uint16_t handle = 0;
    bts_data_to_uuid_len2(BLE_UUID_SERVER_SERVICE, &service_uuid);
    gatts_add_service_sync(BLE_SERVER_ID, &service_uuid, true, &handle);
    HINF("gatts_add_service_sync, service_handle: %d \n", handle);

    gatts_set_mtu_size(g_server_id, (uint16_t)256);

    ble_add_rx_characters_and_descriptors(BLE_SERVER_ID, handle);
    ble_add_tx_characters_and_descriptors(BLE_SERVER_ID, handle);
    gatts_start_service(BLE_SERVER_ID, handle);
}

static void ble_server_uninit()
{
    uint8_t adv_id = BTH_GAP_BLE_ADV_HANDLE_DEFAULT;

    if (ble_get_connection_state() == true)
    {
        gap_ble_remove_all_pairs();
        while (ble_get_connection_state())
        {
            HINF("wait remove all pairs, connection_state: %d\n", ble_get_connection_state());
            usleep(100 * 1000);
        }
    }
    
    gap_ble_stop_adv(adv_id);
    while (g_ble_adv_stats == true)
    {
        HINF("wait stop adv, g_ble_adv_stats : %d\n", g_ble_adv_stats);
        usleep(100 * 1000);
    }

    disable_ble();
    while (g_ble_enable_stats)
    {
        HINF("wait disable ble, g_ble_enable_stats: %d \n", g_ble_enable_stats);
        usleep(100 * 1000);
    }

    return;
}

/* 对外接口---------------------------------------------------------------------*/
void his_ble_start()
{
    if(his_set_ble_running(1))
    {
        HERR("his_ble_start has already lock!\n");
        return;
    }

    ble_server_init();

    his_start_Ble_seconds = time(NULL);

    return;
}

void his_ble_stop()
{
    if (g_his_ble_stop_in_progress) 
    {
        HINF("his_ble_stop is already in progress.\n");
        return;
    }
    
    if(0 == his_get_ble_running())
    {
        HINF("his_ble_stop g_his_ble_is_running == 0 \n");
        g_his_ble_stop_in_progress = 0; // 复位标志位
        return ;
    }

    g_his_ble_stop_in_progress = 1; // 设置标志位

    his_start_Ble_seconds = 0;

    ble_server_uninit();
    usleep(10 * 1000);

    his_set_ble_running(0);

    g_his_ble_stop_in_progress = 0; // 复位标志位

    /* 1205: 修复关闭蓝牙后, 再次开启蓝牙失败问题 */
    g_service_num = 0;

    return;
}

