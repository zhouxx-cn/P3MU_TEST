// Copyright (C) 1991 - 1999 Rational Software Corporation

//#include <iostream>
#include <stdlib.h>
#include <asm/types.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <net/route.h>
#include <netdb.h>

#include <netinet/if_ether.h>

#include <linux/if.h>
#include <linux/ethtool.h>

#include <linux/if_ether.h>
#include <linux/sockios.h>
#include <linux/mii.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include <netpacket/packet.h>
#include <sys/stat.h>
#include <stdarg.h>
#include <dirent.h>
#include <math.h>
#include "NET_global.h"
#include "CConfig.h"
#include <sys/wait.h>
#include <assert.h>
#include <pthread.h>
#include <sys/time.h>
#include <arpa/nameser.h>
#include <resolv.h>
#include "../PAR/CPubFunc.h"
#include "global_macro.h"
#include "ip.h"
#include "INet.h"
#include "adsl.h"
#include "wifi_api.h"
#include "zwsdk.h"
#include "mysystem.h"
#include "Authority.h"

#include "ping.h"
#include "IPPPoE.h"
#include "cJSON.h"
#include "TDES.h"
#include "PUB/common_pub.h"
#include "../PUB/DEVICECAP.h"
#include "base64Lib.h"
#include "PUB_BoradInfo.h"
#include "inifile.h"
#ifdef __IS_SUPPORT_HIS_BLUETOOTH__
#include "his_ble_api.h"
#endif
#ifdef __MODULE_4G_SUPPORT__
#include "../../modules/GPRS/include/4g_mode.h"
#include "newhie/DFClient.h"
#include "newhie/DFClient_Common.h"
#include "newhie/ZW_HTTP_CLIENT_API.h"
#include "newhie/ZW_HTTP_CLIENT_Common.h"
#endif

#ifdef __IS_SUPPORT_AIC_BLUETOOTH__
#include "XRSBLE_L1.h"
#include "XRSBLE_L2.h"
#include "SYS/CSysManager.h"
#include "atbm_nimble_api.h"
#endif


#ifdef __SUPPORT_SAVE_TF_LOG__
#include "../../modules/GPRS/include/tflog.h"
#endif
extern "C" {
	#include "mauth.h"
}
#if defined  (__USE_FTP__)
#include "ftpClient.h"
#endif

#if defined(__IS_SUPPORT_DYNAMIC_GRAPHICS__)
#include "CDynamicGraphics.h"
#endif

/* 消除本文件中的snprintf截断警告 */
#pragma GCC diagnostic ignored "-Wformat-truncation"
/* 消除本文件中的类型转换警告 */
#pragma GCC diagnostic ignored "-Wcast-qual"

#if defined (__AUTH_V2__)
enum LineNum {
    LICENSE_LINENUM			= 1,
    CONNECT_STRING_LINENUM	= 2,
    PLATFORM_LINENUM		= 3,
    PUSH_KEY_LINENUM		= 4,
    GG_CLOUD_LINENUM        = 5,
    CRC_LINENUM             = 6,
    LINENUM_MAX
};

char g_szSupportPlatformName[PLATFORM_NUM][PLATFORM_LENTH] = {"NGLS", "IOTB", "BOTC", "IOTD", "IOTE", "IOTF","IOTG","YLTB","IOTH"};
static char g_szoldPlatformName[3][PLATFORM_LENTH] = {"NGLS", "IOTB", "BOTC"};//这几个平台不需要连接串
#endif

//是否支持phy   设备默认支持有线配网
static int bSupportPhy = 1;

#define CURL_OK (0)
#define CURL_200_OK (200)
#define CURL_RESOLVE_HOST_NAME_FAILED (6)
#define CURL_TIME_OUT (9)
#define CURL_SSL_CONNECT_ERR (35)
#define CODE_200_OK (200)
#define CODE_RANDOM_ERR (5001)
#define CODE_PARAMETER_ERR (2001)
#define CODE_SYSTEM_ERR (2302)

#ifdef __MODULE_4G_SUPPORT__
static int bNeed4GScanStatue = 1; 

//流量运营平台
static const char *g_szSupportFlowOpreatePlatform[] = 
{
	FORWARD_4G_NUMBER,
	FORWARD_4G_NUMBER1,
	FORWARD_4G_NUMBER2,
	FORWARD_4G_NUMBER3,
	FORWARD_4G_NUMBER4
};
#endif


pthread_mutex_t m_ThreadParamLock;

static int Bluetoothstarttime = 0;

#ifdef __IS_SUPPORT_AIC_BLUETOOTH__
//蓝牙设备名前缀
#define BLUE_TOOTH_DEV_NAME_PRX "ZWBLE_"
AppRecvProcessCallback INet::m_cbBlueToothRecvDataCb = NULL;
#endif



#if defined (__IS_SUPPORT_WLAN__)
static int ifWifiNewConnectCfg = 0;
int g_DistributionNetwordFlag = 0;
pthread_mutex_t m_WifiLock;
static WLAN_CONNECTION lastConnection;
#endif

pthread_mutex_t m_WriteDNSFileLock = PTHREAD_MUTEX_INITIALIZER;

#define DNS_ADDR_TOTALNUM_NET (5)
#define DNS_ADDR_LEN_NET (32)
static char	s_DnsAddr[DNS_ADDR_TOTALNUM_NET][DNS_ADDR_LEN_NET];	/*DNS 地址*/
#define WIFI_DRV_PATH_ATBM_6132U     "/usr/ko/atbm6x3x_wifi_usb.ko"

#define NORMAL_EVENT_CHANNEL    100
#define REALTIME_EVENT_CHANNEL  200

typedef struct tagDhcpSuccInfo {
	 int s_Eth0DhcpSuccFlag;
	 int s_Eth0DhcpSuccTime;
	 int s_Wlan0DhcpSuccFlag;
	 int s_Wlan0DhcpSuccTime;
	 pthread_mutex_t SetDNSFileLock;
}DhcpSuccInfo;

static DhcpSuccInfo s_NetCardDhcpInfo;
static int g_TimtOut = 0;
static int g_TimtCount= 0;
// 语音播放超时时间

#define WLAN_Enable_Check(bWlanEnable) \
do { \
	if (!bWlanEnable) \
	{ \
		return -1; \
	} \
} while (0)

#if defined (__IS_SUPPORT_P6SMESSAGE__)
typedef struct tageBoardIdProductModelMap
{
	int nBoardId;
	int nDeviceType;
	char szProductModel[128];
} BOARDID_PRODUCT_MODEL_MAP;


#endif

//////////////////////////////////////////////////////////////////////////
//INet 相关
//////////////////////////////////////////////////////////////////////////

static const int INet_HttpPort_Default_i = 80;
static const int INet_DHCP_Default_i = 1;
static const  int  IFTP_Port_Default_i = 21;
static int g_ethLinkStatus = 1;//0 -- interface link down, 1 -- interface link up
#if defined (__IS_SUPPORT_WLAN__)
static int g_bApModeInitOk  = 0;
#endif
static int IpChangeStatus=0;
static int IPAdjustCloseFlag = 0;

static bool bNeedUpdateRouter = FALSE;
static bool bOperatingRouter  = FALSE;

/* 设备能力支持 */
static int bSupportULK      = 1;
static int bSupportTUTK     = 1;
static int bSupportIPAdjust = 1;
static int bSupportFTP      = 0;
static int bIPAdjustNoModifyGateway = 0;
static int bSupportWIFI      = 0;
static int bSupportWlanAP   = 0;
static int bNeedGenSubKey   = 0;
static int bNeedClrSubKey   = 0;
static int bSupportGPRS     = 0;
static int bSupportOnlyCloudCard	= 0;

static int bSupportQrScan   = 0;

static int g_DHCPEvent   = 0;
static char IWNAME[24] = "wlan0";//ra0
static const char * const des_state[5] = { "Connected", "Disconnect", "Unknown", "IpConflict", "IP change" };
static const unsigned int crc32tab[] = {
    0x00000000L, 0x77073096L, 0xee0e612cL, 0x990951baL,
    0x076dc419L, 0x706af48fL, 0xe963a535L, 0x9e6495a3L,
    0x0edb8832L, 0x79dcb8a4L, 0xe0d5e91eL, 0x97d2d988L,
    0x09b64c2bL, 0x7eb17cbdL, 0xe7b82d07L, 0x90bf1d91L,
    0x1db71064L, 0x6ab020f2L, 0xf3b97148L, 0x84be41deL,
    0x1adad47dL, 0x6ddde4ebL, 0xf4d4b551L, 0x83d385c7L,
    0x136c9856L, 0x646ba8c0L, 0xfd62f97aL, 0x8a65c9ecL,
    0x14015c4fL, 0x63066cd9L, 0xfa0f3d63L, 0x8d080df5L,
    0x3b6e20c8L, 0x4c69105eL, 0xd56041e4L, 0xa2677172L,
    0x3c03e4d1L, 0x4b04d447L, 0xd20d85fdL, 0xa50ab56bL,
    0x35b5a8faL, 0x42b2986cL, 0xdbbbc9d6L, 0xacbcf940L,
    0x32d86ce3L, 0x45df5c75L, 0xdcd60dcfL, 0xabd13d59L,
    0x26d930acL, 0x51de003aL, 0xc8d75180L, 0xbfd06116L,
    0x21b4f4b5L, 0x56b3c423L, 0xcfba9599L, 0xb8bda50fL,
    0x2802b89eL, 0x5f058808L, 0xc60cd9b2L, 0xb10be924L,
    0x2f6f7c87L, 0x58684c11L, 0xc1611dabL, 0xb6662d3dL,
    0x76dc4190L, 0x01db7106L, 0x98d220bcL, 0xefd5102aL,
    0x71b18589L, 0x06b6b51fL, 0x9fbfe4a5L, 0xe8b8d433L,
    0x7807c9a2L, 0x0f00f934L, 0x9609a88eL, 0xe10e9818L,
    0x7f6a0dbbL, 0x086d3d2dL, 0x91646c97L, 0xe6635c01L,
    0x6b6b51f4L, 0x1c6c6162L, 0x856530d8L, 0xf262004eL,
    0x6c0695edL, 0x1b01a57bL, 0x8208f4c1L, 0xf50fc457L,
    0x65b0d9c6L, 0x12b7e950L, 0x8bbeb8eaL, 0xfcb9887cL,
    0x62dd1ddfL, 0x15da2d49L, 0x8cd37cf3L, 0xfbd44c65L,
    0x4db26158L, 0x3ab551ceL, 0xa3bc0074L, 0xd4bb30e2L,
    0x4adfa541L, 0x3dd895d7L, 0xa4d1c46dL, 0xd3d6f4fbL,
    0x4369e96aL, 0x346ed9fcL, 0xad678846L, 0xda60b8d0L,
    0x44042d73L, 0x33031de5L, 0xaa0a4c5fL, 0xdd0d7cc9L,
    0x5005713cL, 0x270241aaL, 0xbe0b1010L, 0xc90c2086L,
    0x5768b525L, 0x206f85b3L, 0xb966d409L, 0xce61e49fL,
    0x5edef90eL, 0x29d9c998L, 0xb0d09822L, 0xc7d7a8b4L,
    0x59b33d17L, 0x2eb40d81L, 0xb7bd5c3bL, 0xc0ba6cadL,
    0xedb88320L, 0x9abfb3b6L, 0x03b6e20cL, 0x74b1d29aL,
    0xead54739L, 0x9dd277afL, 0x04db2615L, 0x73dc1683L,
    0xe3630b12L, 0x94643b84L, 0x0d6d6a3eL, 0x7a6a5aa8L,
    0xe40ecf0bL, 0x9309ff9dL, 0x0a00ae27L, 0x7d079eb1L,
    0xf00f9344L, 0x8708a3d2L, 0x1e01f268L, 0x6906c2feL,
    0xf762575dL, 0x806567cbL, 0x196c3671L, 0x6e6b06e7L,
    0xfed41b76L, 0x89d32be0L, 0x10da7a5aL, 0x67dd4accL,
    0xf9b9df6fL, 0x8ebeeff9L, 0x17b7be43L, 0x60b08ed5L,
    0xd6d6a3e8L, 0xa1d1937eL, 0x38d8c2c4L, 0x4fdff252L,
    0xd1bb67f1L, 0xa6bc5767L, 0x3fb506ddL, 0x48b2364bL,
    0xd80d2bdaL, 0xaf0a1b4cL, 0x36034af6L, 0x41047a60L,
    0xdf60efc3L, 0xa867df55L, 0x316e8eefL, 0x4669be79L,
    0xcb61b38cL, 0xbc66831aL, 0x256fd2a0L, 0x5268e236L,
    0xcc0c7795L, 0xbb0b4703L, 0x220216b9L, 0x5505262fL,
    0xc5ba3bbeL, 0xb2bd0b28L, 0x2bb45a92L, 0x5cb36a04L,
    0xc2d7ffa7L, 0xb5d0cf31L, 0x2cd99e8bL, 0x5bdeae1dL,
    0x9b64c2b0L, 0xec63f226L, 0x756aa39cL, 0x026d930aL,
    0x9c0906a9L, 0xeb0e363fL, 0x72076785L, 0x05005713L,
    0x95bf4a82L, 0xe2b87a14L, 0x7bb12baeL, 0x0cb61b38L,
    0x92d28e9bL, 0xe5d5be0dL, 0x7cdcefb7L, 0x0bdbdf21L,
    0x86d3d2d4L, 0xf1d4e242L, 0x68ddb3f8L, 0x1fda836eL,
    0x81be16cdL, 0xf6b9265bL, 0x6fb077e1L, 0x18b74777L,
    0x88085ae6L, 0xff0f6a70L, 0x66063bcaL, 0x11010b5cL,
    0x8f659effL, 0xf862ae69L, 0x616bffd3L, 0x166ccf45L,
    0xa00ae278L, 0xd70dd2eeL, 0x4e048354L, 0x3903b3c2L,
    0xa7672661L, 0xd06016f7L, 0x4969474dL, 0x3e6e77dbL,
    0xaed16a4aL, 0xd9d65adcL, 0x40df0b66L, 0x37d83bf0L,
    0xa9bcae53L, 0xdebb9ec5L, 0x47b2cf7fL, 0x30b5ffe9L,
    0xbdbdf21cL, 0xcabac28aL, 0x53b39330L, 0x24b4a3a6L,
    0xbad03605L, 0xcdd70693L, 0x54de5729L, 0x23d967bfL,
    0xb3667a2eL, 0xc4614ab8L, 0x5d681b02L, 0x2a6f2b94L,
    0xb40bbe37L, 0xc30c8ea1L, 0x5a05df1bL, 0x2d02ef8dL
};

extern "C" int mysystem(const char *cmdstringl);

static int ipadjust_count = 0;//ip自适应修改ip的次数，ip每跳一次MULTI_DETECT_CYCLE加10，最高60

static int ipWillBeChanged=0;
static char futureIp[MAX_IP_STR_LEN];
static int dhcpWillBeChanged=0;
static int futureDhcp;
static int subMaskWillBeChanged=0;
static char futureSubMask[MAX_IP_STR_LEN];

//线程函数
static void *Fnx_PushInitThread(void *lParam);
static int ParseDnsAddr(void);
static void *Fnx_CheckThread(void *data);
#if defined (__IS_SUPPORT_WLAN__)
static void *SendWifiTestCommandThread(void *lParam);
static void *ApStaStatusCheckThread(void *lParam);
#endif
static void *OperateRouterThread(void * lParam);
#if defined (__MODULE_4G_SUPPORT__)
static void *GPRS_CheckStatusThread(void * lParam);
static void *CheckCardTypeTimerThread(void *lParam);
#endif
static void *IPAdjust_CheckThread(void *lParam);
static void *IPAdjustProcessThread(void *lParam);
static void TruncateIPString(char *szIPString);
static int isValidPort(unsigned short nPort);
static int CheckIpConflic(char *interface,  char *ipaddr);
static void *DomainResolutionManageThread(void *lParam);

#ifdef __IS_SUPPORT_AIC_BLUETOOTH__
static void *btProTestFuncThread(void *lParam);
#endif


//##ModelId=49BDB4D202C8
INet *INet::m_instance = 0;

int INet::m_IPAlarmType = 0;

int INet::m_PHYAlarmType = 0;

int (*INet::m_FxnCallBack_IP) (struct tagAlertInfo * pAlertInfo, struct tagActionInfo * pActionInfo) = NULL;

int (*INet::m_FxnCallBack_PHY) (struct tagAlertInfo * pAlertInfo, struct tagActionInfo * pActionInfo) = NULL;

#if defined (__IS_SUPPORT_WLAN__)
static int g_wifiFakeSTAModeEnable = 0;
static int g_eth0Break = 0;

#ifdef AP_NO_PASSWD
//AP配置无密码格式化
static const char *INet_AP_CONF_NO_PWD_FORMAT = 
	"echo -e "
	"'interface=wlan0\n"
	"driver=nl80211\n"
	"ssid=%s\n"
	"hw_mode=g\n"
	"channel=11\n"
	"skip_inactivity_poll=1\n"
	"ap_max_inactivity=300\n"
	"beacon_int=100\n"
	"dtim_period=1'\n"
	" > %s";
static const char *INet_AP_CONF_NO_PWD_FORMAT1 = 
	"echo -e "
	"'max_num_sta=9\n"
	"rts_threshold=2347\n"
	"fragm_threshold=2346\n"
	"macaddr_acl=0\n"
	"auth_algs=3\n"
	"ignore_broadcast_ssid=0\n"
	"wmm_enabled=1\n"
	"wmm_ac_bk_cwmin=4\n"
	"wmm_ac_bk_cwmax=10\n"
	"wmm_ac_bk_aifs=7\n"
	"wmm_ac_bk_txop_limit=0'\n"
	" >> %s";
static const char *INet_AP_CONF_NO_PWD_FORMAT2 = 
	"echo -e "
	"'wmm_ac_bk_acm=0\n"
	"wmm_ac_be_aifs=3\n"
	"wmm_ac_be_cwmin=4\n"
	"wmm_ac_be_cwmax=10\n"
	"wmm_ac_be_txop_limit=0\n"
	"wmm_ac_be_acm=0\n"
	"wmm_ac_vi_aifs=2\n"
	"wmm_ac_vi_cwmin=3\n"
	"wmm_ac_vi_cwmax=4\n"
	"wmm_ac_vi_txop_limit=94\n"
	"wmm_ac_vi_acm=0\n"
	"wmm_ac_vo_aifs=2\n"
	"wmm_ac_vo_cwmin=2\n"
	"wmm_ac_vo_cwmax=3\n"
	"wmm_ac_vo_txop_limit=47\n"
	"wmm_ac_vo_acm=0\n'"
	" >> %s";
static const char *INet_AP_CONF_NO_PWD_FORMAT3 = 
	"echo -e "
	"'eapol_key_index_workaround=0\n"
	"own_ip_addr=127.0.0.1\n"
	"country_code=US\n"
	"ieee80211d=1\n"
	"#ieee80211n=1\n"
	"#ht_capab=[SHORT-GI-20][RX-STBC1]\n"
	"#ht_capab=[HT20][SHORT-GI-20][RX-STBC1] #only HT20\n"
	"#ht_capab=[HT40-][SHORT-GI-40][RX-STBC1] #only HT40\n"
	"#ht_capab=[HT20][SHORT-GI-20][HT40-][SHORT-GI-40] #HT40 and HT20\n'"
	" >> %s";
#else
//AP配置有密码格式化
static const char *INet_AP_CONF_FORMAT = 
	"echo -e "
	"'interface=wlan0\n"
	"driver=nl80211\n"
	"ssid=%s\n"
	"hw_mode=g\n"
	"channel=11\n"
	"skip_inactivity_poll=1\n"
	"ap_max_inactivity=300\n"
	"beacon_int=100\n"
	"dtim_period=1\n"
	"max_num_sta=9\n"
	"rts_threshold=2347\n"
	"fragm_threshold=2346\n'"
	" > %s";
static const char *INet_AP_CONF_FORMAT1 = 
	"echo -e "
	"'macaddr_acl=0\n"
	"auth_algs=3\n"
	"ignore_broadcast_ssid=0\n"
	"wmm_enabled=1\n"
	"wmm_ac_bk_cwmin=4\n"
	"wmm_ac_bk_cwmax=10\n"
	"wmm_ac_bk_aifs=7\n"
	"wmm_ac_bk_txop_limit=0\n"
	"wmm_ac_bk_acm=0\n"
	"wmm_ac_be_aifs=3\n"
	"wmm_ac_be_cwmin=4\n"
	"wmm_ac_be_cwmax=10\n"
	"wmm_ac_be_txop_limit=0\n'"
	" >> %s";
static const char *INet_AP_CONF_FORMAT2 = 
	"echo -e "
	"'wmm_ac_be_acm=0\n"
	"wmm_ac_vi_aifs=2\n"
	"wmm_ac_vi_cwmin=3\n"
	"wmm_ac_vi_cwmax=4\n"
	"wmm_ac_vi_txop_limit=94\n"
	"wmm_ac_vi_acm=0\n"
	"wmm_ac_vo_aifs=2\n"
	"wmm_ac_vo_cwmin=2\n"
	"wmm_ac_vo_cwmax=3\n"
	"wmm_ac_vo_txop_limit=47\n"
	"wmm_ac_vo_acm=0\n"
	"eapol_key_index_workaround=0\n"
	"own_ip_addr=127.0.0.1\n"
	"country_code=US\n"
	"ieee80211d=1\n"
	"#ieee80211n=1\n'"
	" >> %s";
static const char *INet_AP_CONF_FORMAT3 = 
	"echo -e "
	"'#ht_capab=[SHORT-GI-20][RX-STBC1]\n"
	"#ht_capab=[HT20][SHORT-GI-20][RX-STBC1] #only HT20\n"
	"#ht_capab=[HT40-][SHORT-GI-40][RX-STBC1] #only HT40\n"
	"#ht_capab=[HT20][SHORT-GI-20][HT40-][SHORT-GI-40] #HT40 and HT20\n"
	"#WPA2-AES\n"
	"wpa=2\n"
	"wpa_passphrase=%s\n"
	"wpa_key_mgmt=WPA-PSK\n"
	"wpa_pairwise=CCMP\n'"
	" >> %s";
#endif
#endif

#if defined (__IS_SUPPORT_WLAN__)
#define WIFI_FLAG_HI_H383U          "/mnt/mtd/config/WifiModuleType_h383u"
#define WIFI_FLAG_ATBM_6132U        "/mnt/mtd/config/WifiModuleType_atbm6132"

static int UnloadSoftApDriver_8189(void);
static int UnloadStaModeDriver_8189(void);
static char g_nSoftApSsid[64]   = {0};
static char g_nSoftApPasswd[64] = {0};

//如果要修改AP的默认IP等参数，
//需要修改定制文件/custom/config/udhcpd.conf及/custom/info/custominfo.ini
static char g_szDefaultApIp[64] = {0};
static char g_szDefaultApNetmask[64] = {0};
static char g_szDefaultApGateway[64] = {0};
static char g_szDefaultApPrefix[32]    = {0};
#endif

#if defined (__IS_SUPPORT_WLAN__)
//蓝牙相关
static char g_nBleName[32] = {0};
static char g_szBlePrefix[16]    = {0};
#define DEFAULT_BLE_PREFIX    "ZWBLE_"

//TF卡配网相关
static const char *WIFI_SD_PATH     = "/tmp/SdTypeNetCfg.ini";
static const char *WIFI_SD_TMP_PATH = "/tmp/SdTypeNetCfgTmp.ini";
static const char *INet_WLANKey = "Wlan_Key";
#endif

#ifdef __IS_SUPPORT_HIS_BLUETOOTH__
//蓝牙接收消息回调
static int ParseBleDataCallback(unsigned char *pDataBuf, unsigned int uiDataLen, uint8_t Key_name)
{
    static unsigned char szWlanSSID[WLAN_MAX_SSID_LEN] = {0};
    static unsigned char szWlanKey[WLAN_MAX_KEY_LEN] = {0};
    static unsigned char szTimeZone[8] = {0};
    
    static int bIsFindWlanSSID = 0;
    static int bIsFindWlanKey = 0;
    static int bIsFindTimeZone = 0;

    unsigned int i = 0;
    unsigned char *pStrTmp = NULL;
    unsigned char szStrTmp[256] = {0};

    static int nTimeZone = 0;
    unsigned int uiDealLen = 0;
    int nCurrWifiMode = 0;
    WLAN_CONNECTION stWlanConCfg = {0};

    INet *pNet = INet::GetInstance();
    if (NULL == pNet) {
        return -1;
    }

    if (0x00 == Key_name)
    {
        goto InfoComplete;
    }

    if (NULL == pDataBuf || uiDataLen < 0)
    {
        return -1;
    }

    pStrTmp = pDataBuf;

    //解析SSID
    if (0 == bIsFindWlanSSID && 0x01 == Key_name)
    {
        i = 0;
        memset(szStrTmp, 0, sizeof(szStrTmp));
        while (uiDealLen < uiDataLen)
        {
            if ('\n' == *pStrTmp
                || '\r' == *pStrTmp
                || i >= (sizeof(szStrTmp)-2))
            {
                pStrTmp++;
                uiDealLen++;
                continue;
            }
            if ('\0' == *pStrTmp)
            {
                pStrTmp++;
                uiDealLen++;
                break;
            }

            szStrTmp[i] = *pStrTmp;
            pStrTmp++;
            uiDealLen++;
            i++;
        }
        
        memset(szWlanSSID, 0, sizeof(szWlanSSID));
        base64Decode((char *)szStrTmp, szWlanSSID);
        szWlanSSID[sizeof(szWlanSSID)-1] = 0;

        int SSIDLen = strlen((char *)szWlanSSID);
        if (SSIDLen <= 0) 
        {
            NET_ERR("BLE Scan Info Error [SSIDLen = %d], Clear out!\n", SSIDLen);
            goto isInfoError;
        }

        bIsFindWlanSSID = 1;
        goto isInfoComplete;
    }

    //解析password
    if (0 == bIsFindWlanKey && 0x02 == Key_name)
    {
        i = 0;
        memset(szStrTmp, 0, sizeof(szStrTmp));
        while (uiDealLen < uiDataLen)
        {
            if ('\n' == *pStrTmp
                || '\r' == *pStrTmp
                || i >= (sizeof(szStrTmp)-2))
            {
                pStrTmp++;
                uiDealLen++;
                continue;
            }
            if ('\0' == *pStrTmp)
            {
                pStrTmp++;
                uiDealLen++;
                break;
            }

            szStrTmp[i] = *pStrTmp;
            pStrTmp++;
            uiDealLen++;
            i++;
        }

        memset(szWlanKey, 0, sizeof(szWlanKey));
        base64Decode((char *)szStrTmp, szWlanKey);
        szWlanKey[sizeof(szWlanKey)-1] = 0;
        
        // 防止带验证低于8位后AP模式切换异常, 做简单判断
        int keyLen = strlen((char *)szWlanKey);
        if ( keyLen < 0 || (keyLen < 8 && keyLen > 0) ) 
        {
            NET_ERR("BLE Scan Info Error [keyLen = %d], Clear out!\n", keyLen);
            goto isInfoError;
        }

        bIsFindWlanKey = 1;
        goto isInfoComplete;
    }

    //解析时区
    if (0 == bIsFindTimeZone && 0x03 == Key_name)
    {
        memset(szTimeZone, 0, sizeof(szTimeZone));
        i = 0;
        while (uiDealLen < uiDataLen)
        {
            if ('\n' == *pStrTmp
                || '\r' == *pStrTmp
                || i >= (sizeof(szTimeZone)-2))
            {
                pStrTmp++;
                uiDealLen++;
                continue;
            }
            if ('\0' == *pStrTmp)
            {
                pStrTmp++;
                uiDealLen++;
                break;
            }

            szTimeZone[i] = *pStrTmp;
            pStrTmp++;
            uiDealLen++;
            i++;
        }

        nTimeZone = (szTimeZone[0] << 8) + szTimeZone[1];
        bIsFindTimeZone = 1;
        goto isInfoComplete;
    }

//判断是否集齐全部信息，没有集齐就返回0退出
isInfoComplete:
    if (bIsFindWlanSSID != 1 || bIsFindWlanKey != 1 || bIsFindTimeZone != 1)
    {
        NET_WARN("BLE Scan Info [SSID:%d] [Key:%d] [TimeZone:%d]\n",bIsFindWlanSSID,bIsFindWlanKey,bIsFindTimeZone);
        return 0;
    }

    //提示集齐全部信息后
    ZWSys_SetSpeechContent(SPEECH_SYS_QRCODE_SCAN_OVER);
    NET_INFO("BLE Scan Info Complete \n");
    NET_INFO("WLAN SSID: %s\n", szWlanSSID);
    NET_INFO("WLAN Key: %s\n", szWlanKey);
    NET_INFO("nTimeZone: %d\n", nTimeZone);

    return 1;

InfoComplete:
    bIsFindWlanSSID = 0;
    bIsFindWlanKey = 0;

    //查询上次配网是否已结束
    if (0 == pNet->WLAN_IsConfigureNetIng())
    {
        //AP模式下才配网
        pNet->NET_GetCurrWifiMode(&nCurrWifiMode);
        if (SOFT_AP_MODE == nCurrWifiMode)  
        {
            //配网
            memset(&stWlanConCfg, 0, sizeof(stWlanConCfg));
            memcpy(stWlanConCfg.szSSID, szWlanSSID, sizeof(stWlanConCfg.szSSID)-1);
            stWlanConCfg.struKeyCfg.bValid = 1;
            stWlanConCfg.struKeyCfg.Key1.bValid = 1;
            memcpy(stWlanConCfg.struKeyCfg.Key1.szKey, szWlanKey, 
            sizeof(stWlanConCfg.struKeyCfg.Key1.szKey)-1);
            stWlanConCfg.struKeyCfg.Key2.bValid = 1;
            memcpy(stWlanConCfg.struKeyCfg.Key2.szKey, szWlanKey, 
            sizeof(stWlanConCfg.struKeyCfg.Key2.szKey)-1);
            stWlanConCfg.struKeyCfg.Key3.bValid = 1;
            memcpy(stWlanConCfg.struKeyCfg.Key3.szKey, szWlanKey, 
            sizeof(stWlanConCfg.struKeyCfg.Key3.szKey)-1);
            stWlanConCfg.struKeyCfg.Key4.bValid = 1;
            memcpy(stWlanConCfg.struKeyCfg.Key4.szKey, szWlanKey, 
            sizeof(stWlanConCfg.struKeyCfg.Key4.szKey)-1);
            stWlanConCfg.struKeyCfg.WPAKey.bValid = 1;
            memcpy(stWlanConCfg.struKeyCfg.WPAKey.szKey, szWlanKey, 
            sizeof(stWlanConCfg.struKeyCfg.WPAKey.szKey)-1);

            pNet->WLAN_SetConnection(&stWlanConCfg);

            //设置时区
            if (bIsFindTimeZone)
            {
                MPU_SYS_SetTimeZone(nTimeZone);
                bIsFindTimeZone = 0;
                nTimeZone = 0;
            }
        }
    }

    return 2;

//判断解析结果是否有问题，有问题全部清空
isInfoError:

    memset(szWlanSSID, 0, sizeof(szWlanSSID));
    memset(szWlanKey, 0, sizeof(szWlanKey));
    memset(szTimeZone, 0, sizeof(szTimeZone));

    bIsFindWlanSSID = 0;
    bIsFindWlanKey = 0;
    bIsFindTimeZone = 0;
    nTimeZone = 0;

    return -1;
}
#endif

static int checkETH0Break(int *ifBreak)
{
	int sock = 0;
	int mii_val;
	struct ifreq ifr;
	struct ethtool_value edata;
	mii_ioctl_data *mii;

	//注意bSupportPhy是否已被赋值
	if (0 == bSupportPhy)
	{
		NET_INFO(" not support eth0\n");
		*ifBreak = 1;
		return 0;
	}

	//检测网络是否断开
	if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		NET_ERR(" socket on eth0 failed: %s\n", strerror(errno));
		*ifBreak = 1;
	}
	else
	{
		strncpy(ifr.ifr_name, "eth0", sizeof(ifr.ifr_name) - 1);
		edata.cmd = ETHTOOL_GLINK;

		ifr.ifr_data = (char *) &edata;
		if (-1 == ioctl(sock, SIOCETHTOOL, &ifr))//获取当前网络连接状态
		{
			NET_ERR("ETHTOOL_GLINK failed: %s\n", strerror(errno));

			//将继续进行 SIOCGMIIPHY
			if (ioctl(sock, SIOCGMIIPHY, &ifr) < 0)
			{
				NET_ERR("SIOCGMIIPHY on eth0 failed: %s\n", strerror(errno));
			}
			else
			{
				mii = (mii_ioctl_data *) & ifr.ifr_data;
				mii->reg_num = MII_BMSR;
				if (ioctl(sock, SIOCGMIIREG, &ifr) < 0)
				{
					NET_ERR("SIOCGMIIREG on eth0 failed: %s\n", strerror(errno));
				}
				mii_val = mii->val_out;

				if (!(mii_val & BMSR_LSTATUS))
				{
					*ifBreak = 1;
				}
			}
		}
		else
		{
			*ifBreak = (edata.data ? 0 : 1);	//0 - 连接 ,1-断开lhl modify
		}
		close(sock);
	}

	return 0;
}

#if defined (__IS_SUPPORT_WLAN__)
//检查文件是否存在
static int IsFileExsit(const char *pszFileName)
{
	return (0 == access(pszFileName, F_OK));
}

static int GetWlanSSIDBak(char *SSID_BAK)
{
	if (SSID_BAK == NULL)
	{
		return 0;
	}
	
	CConfig *wifiConfig = CConfig::NewInstance();
	if (IsFileExsit(WIFI_CFG_INI))
	{
		int nRet = wifiConfig->LoadFile(WIFI_CFG_INI);
 		if (0 == nRet)
		{
			wifiConfig->GetValue(INet_CfgSection, INet_WLANSSID_key, " ", SSID_BAK, 128);
		}		
	}			
	wifiConfig->ReleaseInstance(wifiConfig);
	return 0;
}

static int saveWlanSSIDBak(const char *file, const char *newSSID)
{
	XTRACE_MUTEX_LOCK(&m_WifiLock);
	CConfig *wifiConfig = CConfig::NewInstance();
    int nRet = wifiConfig->LoadFile(file);
 	if (0 == nRet || IDVR_PAR_ERROR_EMPTY_FILE==nRet)
	{
		nRet = wifiConfig->SetValue(INet_CfgSection, INet_WLANSSID_key, newSSID);
	}
	wifiConfig->SaveFile();
	wifiConfig->ReleaseInstance(wifiConfig);
	mysystem(RM_WIFI_CFG_BAK);
	XTRACE_MUTEX_UNLOCK(&m_WifiLock);
	return 0;
}

static int GetWlanEncodeEnableBak(int *EncodeEnableBak)
{	
	if(EncodeEnableBak == NULL)
	{
		return 0;
	}
	
	CConfig *wifiConfig = CConfig::NewInstance();
	if(IsFileExsit(WIFI_CFG_INI))
	{
    	int nRet = wifiConfig->LoadFile(WIFI_CFG_INI);
 		if (0 == nRet)
		{
			(*EncodeEnableBak) = wifiConfig->GetValue(INet_CfgSection, INet_WLAN_KEY_ENCODE_ENABLE, 0);
		}		
	}			
	wifiConfig->ReleaseInstance(wifiConfig);
	return 0;
}

static int GetWlanWpaKeyBak(char* WpaKeyBak ,int EncodeEnable)
{	
	if(WpaKeyBak == NULL)
	{
		return 0;
	}
	char base64[(WLAN_MAX_KEY_LEN + (3 - 1))/3*4] = {0};
	
	CConfig *wifiConfig = CConfig::NewInstance();
	if(IsFileExsit(WIFI_CFG_INI))
	{
    	int nRet = wifiConfig->LoadFile(WIFI_CFG_INI);
 		if (0 == nRet)
		{
			if(EncodeEnable)
			{
				wifiConfig->GetValue(INet_CfgSection, INet_WLANWPAKey_key, "" ,base64, sizeof(base64) - 1);
				// zw_base64_decode((unsigned char*)WpaKeyBak, WLAN_MAX_KEY_LEN, (unsigned char*)base64, strlen(base64));
				unsigned long decode_len = WLAN_MAX_KEY_LEN;
				WpaKeyBak = TBase64::Base64_Decode(base64, decode_len);
			}
			else
			{
				wifiConfig->GetValue(INet_CfgSection, INet_WLANWPAKey_key, "" ,WpaKeyBak, WLAN_MAX_KEY_LEN - 1);
			}
			
		}		
	}			
	wifiConfig->ReleaseInstance(wifiConfig);
	return 0;
}

static int saveWlanWPAKeyBak(const char *file,int arg, char *newWPAKey)
{
	char base64Key[(WLAN_MAX_KEY_LEN + (3 - 1))/3*4] = {0};
	XTRACE_MUTEX_LOCK(&m_WifiLock);
	CConfig *wifiConfig = CConfig::NewInstance();
    int nRet = wifiConfig->LoadFile(file);
 	if (0 == nRet || IDVR_PAR_ERROR_EMPTY_FILE==nRet)
	{
		if(arg == 1)
		{
			// Base64_Encode((unsigned char*)base64Key, sizeof(base64Key), (unsigned char*)newWPAKey, strlen(newWPAKey));
			newWPAKey = TBase64::Base64_Encode(base64Key, sizeof(base64Key));
			wifiConfig->SetValue(INet_CfgSection, INet_WLANWPAKey_key, base64Key);
		}
		else
		{
			wifiConfig->SetValue(INet_CfgSection, INet_WLANWPAKey_key, newWPAKey);
		}
	}	
	wifiConfig->SaveFile();
	wifiConfig->ReleaseInstance(wifiConfig);
	mysystem(RM_WIFI_CFG_BAK);
	XTRACE_MUTEX_UNLOCK(&m_WifiLock);

	return 0;
}

static int saveWlanKeyEncodeBak(const char *file, int arg)
{
	XTRACE_MUTEX_LOCK(&m_WifiLock);
	CConfig *wifiConfig = CConfig::NewInstance();
    int nRet = wifiConfig->LoadFile(file);
 	if (0 == nRet || IDVR_PAR_ERROR_EMPTY_FILE == nRet)
	{
		wifiConfig->SetValue(INet_CfgSection, INet_WLAN_KEY_ENCODE_ENABLE, arg);
	}
	wifiConfig->SaveFile();
	wifiConfig->ReleaseInstance(wifiConfig);
	mysystem(RM_WIFI_CFG_BAK);
	XTRACE_MUTEX_UNLOCK(&m_WifiLock);

	return 0;
}

/* 由网口if_name的MAC组合成AP的SSID号 */
static int FormatSoftapSSID(const char *if_name, char *ssid, int ssidSize)
{
    struct ifreq ifr;

    int sock = -1; 
    int ret  = 0;

    unsigned char *ptr = NULL;

    if ((!if_name) || (!ssid))
    {
        return -1;
    }
    
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) 
    {
        return errno;
    }
    strncpy(ifr.ifr_name, if_name, sizeof(ifr.ifr_name) - 1);
    if (ioctl(sock, SIOCGIFHWADDR, &ifr) >= 0)
    {
        ptr = (unsigned char *) & ifr.ifr_ifru.ifru_hwaddr.sa_data[0];
        snprintf(ssid, ssidSize, "%s%02X%02X%02X%02X%02X%02X", 
                g_szDefaultApPrefix, 
                *ptr, *(ptr + 1), *(ptr + 2), *(ptr + 3), *(ptr + 4), *(ptr + 5));
    }
    else
    {
        ret = errno;
    }
    close(sock);

    return ret;
}

/* 由网口if_name的MAC组合成BleName */
static int FormatSoftBleName(char *if_name)
{
    struct ifreq ifr;

    int sock = -1; 
    int ret  = 0;

    unsigned char *ptr = NULL;

    if (!if_name)
    {
        return -1;
    }
    
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) 
    {
        return errno;
    }
    
    strncpy(ifr.ifr_name, if_name, 16);
    if (ioctl(sock, SIOCGIFHWADDR, &ifr) >= 0)
    {
        ptr = (unsigned char *) & ifr.ifr_ifru.ifru_hwaddr.sa_data[0];
        snprintf(g_nBleName, sizeof(g_nBleName),"%s%02X%02X%02X%02X%02X%02X", 
                g_szBlePrefix, 
                *ptr, *(ptr + 1), *(ptr + 2), *(ptr + 3), *(ptr + 4), *(ptr + 5));
    }
    else
    {
        ret = errno;
    }

    close(sock);

    return ret;
}

static int saveWlanWifiModeBak(int newWlanWifiMode)
{
	char buf[64] = {0};
	memset(buf, 0, sizeof(buf));
	strncpy(buf,"rm /mnt/mtd/config/.WlanWifiMode* ",sizeof(buf) - 1);
	mysystem(buf);
	
	memset(buf, 0, sizeof(buf));
	snprintf(buf, sizeof(buf), "touch /mnt/mtd/config/.WlanWifiMode:%d", newWlanWifiMode);
	mysystem(buf);	

	return 0;
}

/* 设置软AP的帐号和密码(帐号固定，密码可以修改) */
static int SetSoftApCFG_8189(const char *passwd)
{
    char command[512] = {0};

    if (!passwd)
    {
        NET_ERR("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    if (0 == g_bApModeInitOk)
    {
        return -1;
    }

    if ('\0' == g_nSoftApSsid[0])
    {
        NET_ERR("SoftAP SSID is Null!\n");
        return -1;
    }

	UnloadSoftApDriver_8189();

#ifdef AP_NO_PASSWD
    /* 配置 hostapd.conf 文件 */
	snprintf(command, sizeof(command), INet_AP_CONF_NO_PWD_FORMAT, g_nSoftApSsid,HOSTAPD_CONF_FILE_PATH);
	if (mysystem(command))
    {
    	NET_ERR("Fail to exec: %s\n", command);
    	return -1;
    }
	memset(command, 0, sizeof(command));
	snprintf(command, sizeof(command), INet_AP_CONF_NO_PWD_FORMAT1,HOSTAPD_CONF_FILE_PATH);
	if (mysystem(command))
    {
    	NET_ERR("Fail to exec: %s\n", command);
    	return -1;
    }
	memset(command, 0, sizeof(command));
	snprintf(command, sizeof(command), INet_AP_CONF_NO_PWD_FORMAT2,HOSTAPD_CONF_FILE_PATH);
	if (mysystem(command))
    {
    	NET_ERR("Fail to exec: %s\n", command);
    	return -1;
    }
	memset(command, 0, sizeof(command));
	snprintf(command, sizeof(command), INet_AP_CONF_NO_PWD_FORMAT3,HOSTAPD_CONF_FILE_PATH);
	if (mysystem(command))
    {
    	NET_ERR("Fail to exec: %s\n", command);
    	return -1;
    }
	memset(command, 0, sizeof(command));
	/* 执行文件 */
    snprintf(command, sizeof(command), "hostapd  /mnt/mtd/config/hostapd.conf &");
	// snprintf(command, sizeof(command), "hostapd -e /usr/sbin/entropy.bin /mnt/mtd/config/hostapd.conf &");
    if (mysystem(command))
    {
    	NET_ERR("Fail to exec: %s\n", command);
    	return -1;
    }
#else
	snprintf(command, sizeof(command), INet_AP_CONF_FORMAT, g_nSoftApSsid, HOSTAPD_CONF_FILE_PATH);
	if (mysystem(command))
    {
    	NET_ERR("Fail to exec: %s\n", command);
    	return -1;
    }
	memset(command, 0, sizeof(command));
	snprintf(command, sizeof(command), INet_AP_CONF_FORMAT1, HOSTAPD_CONF_FILE_PATH);
	if (mysystem(command))
    {
    	NET_ERR("Fail to exec: %s\n", command);
    	return -1;
    }
	memset(command, 0, sizeof(command));
	snprintf(command, sizeof(command), INet_AP_CONF_FORMAT2, HOSTAPD_CONF_FILE_PATH);
	if (mysystem(command))
    {
    	NET_ERR("Fail to exec: %s\n", command);
    	return -1;
    }
	memset(command, 0, sizeof(command));
	snprintf(command, sizeof(command), INet_AP_CONF_FORMAT3, passwd, HOSTAPD_CONF_FILE_PATH);
	if (mysystem(command))
    {
    	NET_ERR("Fail to exec: %s\n", command);
    	return -1;
    }
	memset(command, 0, sizeof(command));
    snprintf(command, sizeof(command), "hostapd %s &", HOSTAPD_CONF_FILE_PATH);
	// snprintf(command, sizeof(command), "hostapd -e /usr/sbin/entropy.bin %s -B", HOSTAPD_CONF_FILE_PATH);
    if (mysystem(command))
    {
    	NET_ERR("Fail to exec: %s\n", command);
    	return -1;
    }
#endif
    NET_INFO("SSID  : %s\n", g_nSoftApSsid);
    //APINFO("PASSWD: %s\n", passwd);
    usleep(500000);
    NET_INFO("Set SoftAP over!\n");
	/* 执行DHCPd */
	/* 检查  /tmp/udhcpd.leases*/
	if(0 != access("/tmp/udhcpd.leases", F_OK))
	{
		mysystem("touch /tmp/udhcpd.leases");
	}
	mysystem("udhcpd -f /mnt/mtd/config/udhcpd.conf &");
	/* 成功启动AP模式的小程序 */

    return 0;
}

static int LoadWifiDriver(int wifimode)
{
    char szCommand[128] = {0};
    memset(szCommand, 0, sizeof(szCommand)); 
	INet *pNet = NULL;
    pNet = INet::GetInstance();

    NET_INFO("****** WifiModuleType:%d, mode:%d ******\n", pNet->m_eCurrUSBWifiDev, wifimode);
    if (WIFI_MODULE_ATBM_6132U == pNet->m_eCurrUSBWifiDev)
    {
        if (0 == IsStringInCommand("lsmod", "atbm6x3x_wifi_usb"))
        {
            snprintf(szCommand, sizeof(szCommand), "insmod %s wifi_bt_comb=1", WIFI_DRV_PATH_ATBM_6132U);
            NET_INFO("ATBM606x_wifi_usb wifi_bt_comb\n");

            mysystem(szCommand); 
            usleep(100000);
        }
        else
        {
            if (pNet != NULL && SOFT_AP_MODE == wifimode)
            {
                //此时可知用户需要进行配网，自动开启蓝牙
                pNet->WLAN_BluetoothOpen();
            }
        }
    }
    return 0;   
}

static int LoadSoftApDriver_8189(void)
{
    char szCommand[256] = {0};
	INet *pNet = NULL;
	int nTryCnt = 3;

    NET_INFO("======Into Load WIFI AP Mode======\n");

	pNet = INet::GetInstance();
	if (NULL != pNet)
	{
		//标志等待配网
		pNet->WLAN_SetWifiLinkStatus(0);
	}
	UnloadStaModeDriver_8189();
	LoadWifiDriver(SOFT_AP_MODE);

    //等待wlan0起来，再往后执行
    while (nTryCnt-- > 0)
    {
        mysystem("ifconfig wlan0 up");
        if (0 == IsStringInCommand("ifconfig ", "wlan0"))
        {
            usleep(500000);
            continue;
        }

        break;
    }

	/*配置网卡信息*/
    memset(szCommand, 0, sizeof(szCommand));
    snprintf(szCommand, sizeof(szCommand), "echo WlanDev=nl80211Uap > %s", WIFI_EXIST_FlAG);
    mysystem(szCommand);
    memset(szCommand, 0, sizeof(szCommand));
    snprintf(szCommand, sizeof(szCommand), "echo WlanNode=%s >> %s", "wlan0", WIFI_EXIST_FlAG);
    mysystem(szCommand);
    memset(szCommand, 0, sizeof(szCommand));
    snprintf(szCommand, sizeof(szCommand), "ifconfig wlan0 %s netmask %s", g_szDefaultApIp, g_szDefaultApNetmask);
    mysystem(szCommand);

    memset(szCommand, 0, sizeof(szCommand));
    snprintf(szCommand, sizeof(szCommand), "route add default gw %s", g_szDefaultApGateway);
    mysystem(szCommand);

    g_bApModeInitOk = 1;
    usleep(500000);
    SetSoftApCFG_8189(g_nSoftApPasswd);

    NET_ERR("****** Configure WIFI Soft Ap Mode Ok! ******\n");
    return 0;
}

/* 卸载SoftAp模式的gongju  */
static int UnloadSoftApDriver_8189(void)
{
	if (IsStringInCommand("ps", "hostapd"))
	{
		mysystem("killall -9 hostapd");
	}

	if (IsStringInCommand("ps", "udhcpd"))
	{
		mysystem("killall -9 udhcpd");
	}

	return 0;
}
int INet::NET_GetWiFiModule()//获取wifi模组型号
{
    DEV_WIFI_INFO_S modudleInfo = { 0 };
    /* 获取模块信息 */
    if(ZWSys_GetWiFiInfo(&modudleInfo))
    {
        m_eCurrUSBWifiDev = WIFI_MODULE_HI_H383U;
        NET_ERR("get wifi module fail \n");
        return -1;
    }
    /* ZWSys_GetWiFiInfo 接口获取模块信息的，暂时调用有问题 */
    if(modudleInfo.Support == 0)
    {
        NET_ERR("dev not support wifi module\n");
        /* 需要关闭 wifi 启动的功能 */
        return -1;
    }
    NET_INFO(" ==== Support = %d Freq = %d Gen = %d Model = %s \n",modudleInfo.Support, modudleInfo.Freq,\
        modudleInfo.Gen, modudleInfo.Model);
    if(fifthGener == modudleInfo.Freq && (NULL != strstr((char *)modudleInfo.Model, "ATBM6132BU")))  // 5G
    {
        m_eCurrUSBWifiDev = WIFI_MODULE_ATBM_6132U;
    }
    else if(tow_fourGener == modudleInfo.Freq && (NULL != strstr((char *)modudleInfo.Model, "H3873UV100")))  // 2.4G
    {
        m_eCurrUSBWifiDev = WIFI_MODULE_HI_H383U;
    }
	 else if(NULL != strstr((char *)modudleInfo.Model, "AIC8800D80"))  // 2.4G 5G
    {
        m_eCurrUSBWifiDev = WIFI_MODULE_AIC_8800DL;
    }
	else if(NULL != strstr((char *)modudleInfo.Model, "AIC8800DL"))  // 2.4G
    {
        m_eCurrUSBWifiDev = WIFI_MODULE_AIC_8800DL_2_4G;
    }
    NET_INFO("==== Support m_eCurrUSBWifiDev = %d\n", m_eCurrUSBWifiDev);
	 
    return 0;
}

int INet::NET_GetWiFiUSBWifiDev()
{
	return m_eCurrUSBWifiDev;
}

static int  LoadStaModeDriver_8189(void)
{
	char szCommand[256] = {0};
	INet *pNet = NULL;
	char buf[64] = {0};
	char c_wlanIP[20] = {0};

    NET_INFO("======Into Load WIFI STA Mode======\n");
	pNet = INet::GetInstance();
	if (NULL != pNet)
	{
		// 标志配网中
		pNet->WLAN_SetWifiLinkStatus(1);
	}

	UnloadSoftApDriver_8189();
	LoadWifiDriver(WLAN_STA_MODE);

#if defined (__IS_SUPPORT_WLAN__)
    if ((WIFI_MODULE_HI_H383U ==pNet->m_eCurrUSBWifiDev)
		|| (WIFI_MODULE_ATBM_6132U == pNet->m_eCurrUSBWifiDev)
		|| (WIFI_MODULE_AIC_8800DL == pNet->m_eCurrUSBWifiDev)		
		|| (WIFI_MODULE_AIC_8800DL_2_4G == pNet->m_eCurrUSBWifiDev))
    {
        memset(szCommand, 0, sizeof(szCommand));
        snprintf(szCommand, sizeof(szCommand), "ifconfig wlan0 down");
        mysystem(szCommand);
        usleep(200000);
    }
#endif

    /* 此处必须判断wlan0网口是否存在 */
    if(mysystem("ifconfig -a wlan0 up"))
    {
        NET_ERR("Start wlan0 error!\n");
    }

    memset(szCommand, 0, sizeof(szCommand));
    snprintf(szCommand, sizeof(szCommand), "echo WlanDev=nl80211Usta > %s", WIFI_EXIST_FlAG);
    mysystem(szCommand);
    memset(szCommand, 0, sizeof(szCommand));
    snprintf(szCommand, sizeof(szCommand), "echo WlanNode=%s >> %s", "wlan0", WIFI_EXIST_FlAG);
    mysystem(szCommand);

    // 运行wpa_supplicant程序
    if (0 == IsStringInCommand("ps", "wpa_supplicant"))
    {
        if (mysystem(RUN_WPA_SUPPLICANT))
        {
            NET_ERR("run wpa_supplicant error!\n");
        }
		memset(buf, 0, sizeof(buf));
		strncpy(buf, "ls /mnt/mtd/config/.WlanIp* | awk -F \":\" '{printf $2}'", sizeof(buf) - 1);
		ShellCmdOut(buf, c_wlanIP, sizeof(c_wlanIP) - 1);
		if (0 != pNet->checkIPAddr(c_wlanIP))
		{
			memset(c_wlanIP,0,sizeof(c_wlanIP));
			strncpy(c_wlanIP, "192.168.2.11", sizeof(c_wlanIP) - 1);
		}
		/* 使用 ifconfig 设置网卡 IP 会自动设置路由 */
		memset(szCommand, 0, sizeof(szCommand));
		snprintf(szCommand, sizeof(szCommand), "ifconfig wlan0 %s", c_wlanIP);
        if (mysystem(szCommand))
        {
            NET_ERR("Configure wlan0 ip error!\n");
        }
		/* 设备会自动添加无线路由到路由表 ----- 删除路由表 ;在有线下;*/
		if((0 == pNet->NET_GetStatus()) || (g_ethLinkStatus == 1))
		{
			mysystem("/usr/sbin/operate_router delRoute wlan0");
		}
    }
    NET_INFO("****** Configure WIFI STA Mode Ok! ******\n");
	return 0;
}

/* 卸载STA模式的驱动*/
static int UnloadStaModeDriver_8189(void)
{
    if (IsStringInCommand("ps", "wpa_supplicant"))
    {
        mysystem("killall -9 wpa_supplicant");
    }
    if (IsStringInCommand("ps", "udhcpc"))
    {
        mysystem("killall -9 udhcpc");
    }
    NET_INFO("======Remove WIFI STA Mode Over======\n");
    return 0;
}

/*
从配置文件获取wifi模式配置
*/
static int loadParamInsmod_8189()
{
	int wifiModeCfg = SOFT_AP_MODE;
	int wifiModeCfgBak = -1;
	char szBuf[512] = {0};
	char szLastSetValue[128] = {0};
	int loadFileSuc = 0;
	 
	//获取新的配置文件读取实例
	CConfig *pCfg = CConfig::GetInstance();
	if (pCfg)
	{
		wifiModeCfg = pCfg->GetValue(INet_CfgSection, INet_WLANWifiMode, SOFT_AP_MODE);
		g_wifiFakeSTAModeEnable = pCfg->GetValue(INet_CfgSection, INet_WLANFakeSTAMode, 0);
		loadFileSuc = 1;
	}
	 
	//和wifi模式备份文件对比
	if (0 == (access(WIFI_MODE_0_BAK_FILE_PATH, F_OK))
		|| 0 == (access(WIFI_MODE_1_BAK_FILE_PATH, F_OK))
		|| 0 == (access(WIFI_MODE_2_BAK_FILE_PATH, F_OK)))
	{
		snprintf(szBuf, sizeof(szBuf), "ls %s | awk -F \":\" '{printf $2}'", WIFI_MODE_BAK_FILE_PATH);
		ShellCmdOut(szBuf, szLastSetValue, sizeof(szLastSetValue) - 1);
		sscanf(szLastSetValue, "%d", &wifiModeCfgBak);
		if ((0 == wifiModeCfgBak)
			|| (1 == wifiModeCfgBak)
			|| (2 == wifiModeCfgBak))
		{
			if (wifiModeCfg != ((eWifiMode)wifiModeCfgBak))
			{
				NET_WARN("Need reset dhcp: from %d to %d\n", wifiModeCfg, wifiModeCfgBak);
				wifiModeCfg = (eWifiMode)wifiModeCfgBak;
			}
		}
	}
	
	g_eth0Break = 1; // 默认断开
	checkETH0Break(&g_eth0Break);
	NET_INFO("**********======g_eth0Break:%d======***********\n", g_eth0Break);
	
	if (loadFileSuc)
	{
		NET_INFO("======wifiModeCfg:%d======\n", wifiModeCfg);
		if (SOFT_AP_MODE == wifiModeCfg)
		{
			if (0 == g_eth0Break)	//如果是AP模式,且插着有线,则切换成STA模式，并产生一个标记
			{
				wifiModeCfg = WLAN_STA_MODE;
				g_wifiFakeSTAModeEnable = 1;
				NET_WARN("Force to STA mode!\n");
			}
			else
			{
				g_wifiFakeSTAModeEnable = 0;
			}
		}
		else	//STA模式
		{
			if (g_wifiFakeSTAModeEnable)	//假STA模式
			{
				if (0 == g_eth0Break)	//有线连着
				{
				}
				else	//有线拔掉,则要恢复到AP模式
				{
					wifiModeCfg = SOFT_AP_MODE;
					g_wifiFakeSTAModeEnable = 0;
					NET_WARN("Recover to AP mode!\n");
				}
			}
		}
	}

	//没有接网线---- 切之前没有配置过网络
	//则强制为STA模式
	/* 之前没有配过网的，就进入假 STA 模式 */
    #if 0
	if ((access("/mnt/mtd/config/.wifiConSuc:0", F_OK)==0) || (access("/mnt/mtd/config/.wifiConSuc:1", F_OK)!=0)
        || (0 != g_eth0Break && 0 == access(WIFI_SD_PATH, F_OK)))
	{
		wifiModeCfg = WLAN_STA_MODE;
	    g_wifiFakeSTAModeEnable = 1;
	}
	if((access("/mnt/mtd/config/.wifiConSuc:1", F_OK)==0) && 0 != g_eth0Break)
    {
	 	wifiModeCfg = WLAN_STA_MODE;
	  	g_wifiFakeSTAModeEnable = 0;
    }
    #endif
    if (0 != g_eth0Break
		&& 0 == access(WIFI_SD_PATH, F_OK))
    {
       wifiModeCfg = WLAN_STA_MODE;
	   g_wifiFakeSTAModeEnable = 0;
    }
	if (pCfg)
	{
		pCfg->SetValue(INet_CfgSection, INet_WLANWifiMode, wifiModeCfg);
		saveWlanWifiModeBak(wifiModeCfg);
		pCfg->SetValue(INet_CfgSection, INet_WLANFakeSTAMode, g_wifiFakeSTAModeEnable);
		pCfg->SaveFileReal();
	}

	NET_INFO("======wifiModeCfg:%d======\n", wifiModeCfg);
	
	//根据wifi模式配置，尝试加载对应驱动
	if (WLAN_STA_MODE == wifiModeCfg)
	{
		INet *pNet = INet::GetInstance();
		if (NULL != pNet)
		{
			NET_INFO("WLAN_BluetoothClose\n");
		
			pNet->WLAN_BluetoothClose();
		}  

		LoadStaModeDriver_8189();
	}
	else
	{	
		INet *pNet = INet::GetInstance();
		if (NULL != pNet)
		{
			NET_INFO("WLAN_BluetoothOpen\n");
			pNet->WLAN_BluetoothOpen();
		}  

		LoadSoftApDriver_8189();
	}
	
	return 0;
}

static int WIFI_INIT(void)
{
	int ret = 0;
	//根据配置文件选择模式
	loadParamInsmod_8189();
    return ret;
}
#endif

/*@brief     GetNetlinkStatus 获取网卡if_name的连接状态
 *@param[in] if_name     网卡名字eth0, wlan0或ra0
 *@return    -1 -- error, details can check errno
 *            1 -- interface link up
 *            0 -- interface link down
 *@note      如果是在虚拟机上测试，需要root权限
 *           可以通过getuid()!=0判断非root用户
 */
static int GetNetlinkStatus(const char *if_name)  
{  
    static int valid = 0;
    static int lastStatus = 1;

    valid = !valid;

    if(0 == valid)//每两次调用做一次
    {
        return lastStatus;
    }
        
    if (NULL == if_name)
    {
        NET_ERR("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    int skfd = -1;
    struct ifreq ifr;
    struct ethtool_value edata;
    int status = 0;
    int i;
    mii_ioctl_data *mii;
    int mii_val;

    //检测网络是否断开
///////////////////////////////////////////////////////////////////////////////
// 修改人:   刘熙明
// 修改原因: 根据配置屏蔽对Phy操作
// 修改时间: 2019.06.11
    if (0 == bSupportPhy)
    {
        NET_DBG(" not support eth0\n");
    }
    else if ((skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        NET_ERR(" socket on eth0 failed: %s\n", strerror(errno));
    }
    else
    {
        strncpy(ifr.ifr_name, IFNAME, IFNAMSIZ);
        edata.cmd = ETHTOOL_GLINK;

        ifr.ifr_data = (char *) &edata;
        if ((i = ioctl(skfd, SIOCETHTOOL, &ifr)) == -1)//获取当前网络连接状态
        {
            NET_ERR("ETHTOOL_GLINK failed: %s\n", strerror(errno));

            //将继续进行 SIOCGMIIPHY
            if (ioctl(skfd, SIOCGMIIPHY, &ifr) < 0)
            {
                NET_ERR("SIOCGMIIPHY on eth0 failed: %s\n", strerror(errno));
            }
            else
            {
                mii = (mii_ioctl_data *) & ifr.ifr_data;
                mii->reg_num = MII_BMSR;
                if (ioctl(skfd, SIOCGMIIREG, &ifr) < 0)
                {
                    NET_ERR("SIOCGMIIREG on eth0 failed: %s\n", strerror(errno));
                }
                mii_val = mii->val_out;

                if (!(mii_val & BMSR_LSTATUS))
                {
                    status = 0;
                }
            }

        }
        else
        {
            status = (edata.data ? 1 : 0);
        }
        close(skfd);

    }
    lastStatus = status;

    return status;
}  

static int saveIpBak(char *newIP)
{
	char buf[64];
	memset(buf, 0, sizeof(buf));
	strncpy(buf,"rm /mnt/mtd/config/.Ip* ", sizeof(buf) - 1);
	mysystem(buf);
	
	memset(buf, 0, sizeof(buf));
	snprintf(buf, sizeof(buf), "touch /mnt/mtd/config/.Ip:%s", newIP);
	mysystem(buf);	
	
	return 0;
}

static int saveGatewayBak(char *newGateway)
{
	char buf[64];
	memset(buf, 0, sizeof(buf));
	strncpy(buf,"rm /mnt/mtd/config/.GATEWAY* ", sizeof(buf) - 1);
	mysystem(buf);
	
	memset(buf, 0, sizeof(buf));
	snprintf(buf, sizeof(buf), "touch /mnt/mtd/config/.GATEWAY:%s", newGateway);
	mysystem(buf);	
	
	return 0;
}

static int saveSubMaskBak(char *newSubMask)
{
	char buf[64];
	memset(buf, 0, sizeof(buf));
	strncpy(buf,"rm /mnt/mtd/config/.MASK* ", sizeof(buf) - 1);
	mysystem(buf);
	
	memset(buf, 0, sizeof(buf));
	snprintf(buf, sizeof(buf), "touch /mnt/mtd/config/.MASK:%s", newSubMask);
	mysystem(buf);	

	return 0;
}

static int saveDhcpBak(int newDhcp)
{
	char buf[64];
	memset(buf, 0, sizeof(buf));
	strncpy(buf,"rm /mnt/mtd/config/.DHCP* ", sizeof(buf) - 1);
	mysystem(buf);
	
	memset(buf, 0, sizeof(buf));
	snprintf(buf, sizeof(buf), "touch /mnt/mtd/config/.DHCP:%d", newDhcp);
	mysystem(buf);	
	
	return 0;
}
#if defined (__IS_SUPPORT_WLAN__)

static int saveWlanDhcpBak(int newDhcp)
{
	char buf[64] = {0};
	memset(buf, 0, sizeof(buf));
	strncpy(buf,"rm /mnt/mtd/config/.WlanDHCP* ", sizeof(buf) - 1);
	mysystem(buf);
	
	memset(buf, 0, sizeof(buf));
	snprintf(buf, sizeof(buf), "touch /mnt/mtd/config/.WlanDHCP:%d", newDhcp);
	mysystem(buf);	
	
	return 0;
}

static int saveConnectSuccess(int status)
{
	char buf[64] = {0};
	memset(buf, 0, sizeof(buf));
	strncpy(buf,"rm /mnt/mtd/config/.wifiConSuc* ", sizeof(buf) - 1);
	mysystem(buf);
	memset(buf, 0, sizeof(buf));
	snprintf(buf, sizeof(buf), "touch /mnt/mtd/config/.wifiConSuc:%d", status);
	mysystem(buf);
	
	return 0;
}
static int saveWlanIpBak(char *newIP)
{
	char buf[64] = {0};
	memset(buf, 0, sizeof(buf));
	strncpy(buf,"rm /mnt/mtd/config/.WlanIp* ", sizeof(buf) - 1);
	mysystem(buf);
	memset(buf, 0, sizeof(buf));
	snprintf(buf, sizeof(buf), "touch /mnt/mtd/config/.WlanIp:%s", newIP);
	mysystem(buf);	
	
	return 0;
}

static int saveWlanSubMaskBak(char *newSubMask)
{
	char buf[64] = {0};
	memset(buf, 0, sizeof(buf));
	strncpy(buf,"rm /mnt/mtd/config/.WlanMASK* ", sizeof(buf) - 1);
	mysystem(buf);
	
	memset(buf, 0, sizeof(buf));
	snprintf(buf, sizeof(buf), "touch /mnt/mtd/config/.WlanMASK:%s", newSubMask);
	mysystem(buf);	

	return 0;
}

static int saveWlanGateWayBak(char *newGateWay)
{
	char buf[64] = {0};
	memset(buf, 0, sizeof(buf));
	strncpy(buf,"rm /mnt/mtd/config/.WlanGateWay* ", sizeof(buf) - 1);
	mysystem(buf);
	
	memset(buf, 0, sizeof(buf));
	snprintf(buf, sizeof(buf), "touch /mnt/mtd/config/.WlanGateWay:%s", newGateWay);
	mysystem(buf);	

	return 0;
}
#endif

static int saveIPAdjustBak(int IPAdjust)
{
	char buf[64];
	memset(buf, 0, sizeof(buf));	
	strncpy(buf,"rm /mnt/mtd/config/.IPADJUST* ", sizeof(buf) - 1);
	mysystem(buf);	
	
	memset(buf, 0, sizeof(buf));	
	snprintf(buf, sizeof(buf), "touch /mnt/mtd/config/.IPADJUST:%d", IPAdjust);
	mysystem(buf);	
	
	return 0;
}

/*
 * 当DHCP获取失败后根据SN来设置IP
 */
int INet::SetIpFromSN(int *dat)
{
	if (0 == bSupportPhy)
	{
		NET_INFO(" not support eth0\n");
		return 0;
	}

    if (0 == bSupportIPAdjust)
    {
        NET_INFO(" not support IPAdjust \n");
        return 0;
    }

	int ret,i=0;
	int dat1;
	unsigned int sn = 0;
	char ipaddr[MAX_IP_STR_LEN];
	char szBroadcastAddr[MAX_IP_STR_LEN];
	int status=-1,count=0;

	char szSerialNum[32] = {0};
    char szShortSerialNum[10] = {0};

    XTRACE_MUTEX_LOCK(&m_IpDhcpLock);

    /* 截取字符串的前10位作为当前用的sn */
	ZWSys_GetSerialNum((unsigned char *)szSerialNum);
    strncpy(szShortSerialNum, szSerialNum, sizeof(szShortSerialNum) - 1);
    sn = atoi(szShortSerialNum);

	do
	{
		dat1 = (sn+i) % 253 + 2;//范围在2~254
		dat1 &= 0XFF;
		memset(ipaddr, 0, sizeof(ipaddr));
		snprintf(ipaddr, sizeof(ipaddr), "%s%d", INet_Default_Head_s, dat1);
		NET_DBG("sn times=%d, ip=%s\n",i,ipaddr);

		ret = CheckIpConflic((char *) IFNAME,  ipaddr);
		if(ret)//该IP已被占用
		{
			i++;
			if(253 == i)
			{
				NET_ERR("no unused Ip!\n");
                XTRACE_MUTEX_UNLOCK(&m_IpDhcpLock);  
				return -1;
			}
			status = -1;
			continue;
		}

		ret = set_if_address((char *) IFNAME, ipaddr, SET_ADDRESS);
		if (ret)
		{
			NET_ERR("Set ipaddress failed (%d) ipaddr =%s \n", ret, ipaddr);
            XTRACE_MUTEX_UNLOCK(&m_IpDhcpLock);              
			return -1;
		}
		get_if_address((char *) IFNAME, szBroadcastAddr, sizeof(szBroadcastAddr), GET_BROADCAST);
		set_if_address((char *) IFNAME, szBroadcastAddr, SET_BROADCAST);
#ifndef DEL_MULTICAST_ROUTE
		mysystem("route add -net 224.0.0.0 netmask 224.0.0.0 dev eth0");//组播
#endif
	    m_bIsAddNetRoute = 0;	

		strncpy(m_sLocalIP, ipaddr, sizeof(m_sLocalIP) - 1);
		//不保存配置文件
		//m_pCConfig->SetValue(INet_CfgSection, INet_IP_key, m_sLocalIP);
		i++;
		if(253 == i)
		{
			NET_ERR("no unused Ip!\n");
            XTRACE_MUTEX_UNLOCK(&m_IpDhcpLock);              
			return -1;
		}
		while((-1 == (status = IsIpConflict(1))) && (++count<8))
		{
			sleep(1);
			NET_DBG("-------------count=%d------------IsIpConflict=%d\n", count, status);
		}
		
		mysystem("ifconfig");
		
		break;
	}
	while(status);
	
	NET_DBG("m_sLocalIP=%s\n", m_sLocalIP);

    if (bSupportWIFI)
    {
        /* 设置完IP地址需要更新路由 */
        bNeedUpdateRouter = TRUE;
    }

    XTRACE_MUTEX_UNLOCK(&m_IpDhcpLock);

	return 0;
}

/*
*随机生成IP
*/
int INet::SetIpRand(const char *hostIp)
{
	if (0 == bSupportPhy)
	{
		NET_INFO(" not support eth0\n");
		return 0;
	}

    
    if (0 == bSupportIPAdjust)
    {
        NET_INFO(" not support IPAdjust\n");
        return 0;
    }

	int ret = 0;
	unsigned int dat1,dat2,dat3,dat4;
	char ipaddr[MAX_IP_STR_LEN];
	char szBroadcastAddr[MAX_IP_STR_LEN];
	int status=-1,count=0;
	char gateWay[MAX_IP_STR_LEN] = {0};
    
    /*rand()用来产生随机IP,用数组来记录，IP即为数组下标，冲突或不可用将此下标的值给1*/
    unsigned char ip_value[256] = {0};//记录冲突的表,冲突或不可用则赋1
    int rand_count = 0;//2 —— 254 计数253
    int rand_value = 0;//rand()产生的值，即数组下标
    int rand_same = 0;//rand()产生的值后，与之前产生的值一样时的计数。达到一定次数(暂定为60)后就查表找可用的IP
    int search_count = 0, left = 0, right = 0;//在左或右半部分查找时，冲突时的次数。left right左右部分的起始值
    int conflict_count = 0;//每次都是第一次检测就冲突的计数。
    int search_falg = 0;//在表中查找的标记
    
    ip_value[0] = ip_value[1] = ip_value[255] = 1;

    XTRACE_MUTEX_LOCK(&m_IpDhcpLock);

	sscanf(hostIp, "%d.%d.%d.%d", &dat1,&dat2,&dat3,&dat4);

    struct timeval IPtm ={0};//rand seed for rand()
    gettimeofday(&IPtm,NULL);
    srand((unsigned)(IPtm.tv_usec));
    
    do
	{  
        if(IP_NUM == rand_count)//无任一IP可用
        {
            NET_DBG("no unused Ip 1 !\n");
            XTRACE_MUTEX_UNLOCK(&m_IpDhcpLock); 
            return -1; 
        }
		unsigned int tt = time(NULL);
        rand_value = rand_r(&tt) % 253 + 2;//IP范围 2 ~ 254
        
        if(rand_same <= RAND_SAME_MAX )//随机产生冲突的总次数未到100次，依旧随机
        {
            if(0 == ip_value[rand_value])//暂时randIP可用
            {
                rand_count++;
                dat4 = rand_value;
                ip_value[rand_value] = 1;
            }else
            {
                rand_same++;
                continue;
            }
        }
        else//随机产生冲突的总次数超过限制，依此时的随机值在表中找可用的
        {
            search_falg = 0;
            search_count = 0;
            
            for(left = rand_value;left >= IP_DAT4_MIN;left--)//先从左半部分找,下标大到小。范围[2,rand_value]
            {
                if(ip_value[left])//冲突
                {
                    search_count++;
                    continue;
                }else
                {
                    NET_DBG("\n test info left rand_value can use = %d\n",left);
                    rand_count++;
                    dat4 = left;
                    ip_value[left] = 1;
                    break;
                }
            }

            if(search_count == (rand_value + 1 - IP_DAT4_MIN))//左半部分无可用IP，则到右半部分找，下标小到大
            {
                search_falg = 1;
                search_count = 0;

                for(right = rand_value;right <= IP_DAT4_MAX;right++)//范围[rand_value,254]
                {
                    if(ip_value[right])//冲突
                    {
                        search_count++;
                        continue;                        
                    }else
                    {
                        NET_DBG("\n test info right rand_value can use = %d\n",right);
                        rand_count++;
                        dat4 = right;
                        ip_value[right] = 1; 
                        break;
                    }
                }
            }

            if((1 == search_falg) && (search_count == (IP_DAT4_MAX  + 1 - rand_value)))//右半部分无可用IP，则无任一IP可用
            {
                NET_DBG("no unused Ip 2 !\n");
                XTRACE_MUTEX_UNLOCK(&m_IpDhcpLock); 
                return -1;
            }
        }

		dat4 &= 0XFF;
		memset(ipaddr, 0, sizeof(ipaddr));
		snprintf(ipaddr, sizeof(ipaddr), "%d.%d.%d.%d", dat1, dat2, dat3, dat4);
		
		ret = CheckIpConflic((char *) IFNAME,  ipaddr);
		if(ret)//该IP已被占用
		{
			status = -1;
            conflict_count ++;
            
            if(IP_NUM == conflict_count)//每个IP第一次检测就冲突，即设备多于可用IP。全都走一遍了就设置此轮的最后一个随机的IP。
            {
                goto set;//直接设置下去，不再走第二次冲突检测的流程
            }else
            {
                continue;  
            }
		}

		//可能在生成随机IP的时候关闭自适应，这时候不应该设置IP等其他信息，应直接返回
		if (!m_bIP_Adjust_Enbale)
		{
            XTRACE_MUTEX_UNLOCK(&m_IpDhcpLock);
			return 0;
		}
		
		ret = set_if_address((char *) IFNAME, ipaddr, SET_ADDRESS);
		if (ret)
		{
			NET_ERR("Set ipaddress failed (%d) ipaddr =%s \n", ret, ipaddr);
            XTRACE_MUTEX_UNLOCK(&m_IpDhcpLock);            
			return -1;
		}

		if(!bIPAdjustNoModifyGateway)
		{
			sscanf(ipaddr, "%d.%d.%d.%d", &dat1, &dat2, &dat3, &dat4);
			snprintf(gateWay, sizeof(gateWay), "%d.%d.%d.1", dat1, dat2, dat3);
			ret = set_if_address((char *) IFNAME, gateWay, SET_GATEWAY);
			if (ret)
			{
				NET_ERR("Set gateway failed (%d) gateway =%s \n", ret, gateWay);
	            XTRACE_MUTEX_UNLOCK(&m_IpDhcpLock);            
				return -1;
			}

			strncpy(m_sGateWay, gateWay, sizeof(m_sGateWay) - 1); 
			m_pCConfig->SetValue(INet_CfgSection, INet_Gateway_key, m_sGateWay);
			saveGatewayBak(m_sGateWay);
		}

		get_if_address((char *) IFNAME, szBroadcastAddr, sizeof(szBroadcastAddr), GET_BROADCAST);

		set_if_address((char *) IFNAME, szBroadcastAddr, SET_BROADCAST);

#ifndef DEL_MULTICAST_ROUTE
		mysystem("route add -net 224.0.0.0 netmask 224.0.0.0 dev eth0");//组播
#endif
	    m_bIsAddNetRoute = 0;	

		strncpy(m_sLocalIP, ipaddr, sizeof(m_sLocalIP) - 1);	
		m_pCConfig->SetValue(INet_CfgSection, INet_IP_key, m_sLocalIP);
		saveIpBak(m_sLocalIP);

		while((-1 == (status = IsIpConflict(1))) && (++count<8))
		{
			sleep(1);
			NET_DBG("-------------count=%d------------IsIpConflict=%d\n", count, status);
		}

        if(8 == count)
        {
            if (status == -1)
            {
                XTRACE_MUTEX_UNLOCK(&m_IpDhcpLock);                
                return 0;
            }
        }
		count = 0;
	}
	while(status);
	
	NET_DBG("m_sLocalIP=%s\n", m_sLocalIP);

    XTRACE_MUTEX_UNLOCK(&m_IpDhcpLock);

	return 0;

    set:
		//可能在生成随机IP的时候关闭自适应，这时候不应该设置IP等其他信息，应直接返回
		if (!m_bIP_Adjust_Enbale)
		{
            XTRACE_MUTEX_UNLOCK(&m_IpDhcpLock);
			return 0;
		}
		
		ret = set_if_address((char *) IFNAME, ipaddr, SET_ADDRESS);
		if (ret)
		{
			NET_ERR("Set ipaddress failed (%d) ipaddr =%s \n", ret, ipaddr);
            XTRACE_MUTEX_UNLOCK(&m_IpDhcpLock);            
			return -1;
		}

		if(!bIPAdjustNoModifyGateway)
		{
			sscanf(ipaddr, "%d.%d.%d.%d", &dat1, &dat2, &dat3, &dat4);
			snprintf(gateWay, sizeof(gateWay), "%d.%d.%d.1", dat1, dat2, dat3);
			ret = set_if_address((char *) IFNAME, gateWay, SET_GATEWAY);
			if (ret)
			{
				NET_ERR("Set gateway failed (%d) gateway =%s \n", ret, gateWay);
	            XTRACE_MUTEX_UNLOCK(&m_IpDhcpLock);            
				return -1;
			}
			
			strncpy(m_sGateWay, gateWay,sizeof(m_sGateWay) - 1); 
			m_pCConfig->SetValue(INet_CfgSection, INet_Gateway_key, m_sGateWay);
			saveGatewayBak(m_sGateWay);
		}

		get_if_address((char *) IFNAME, szBroadcastAddr, sizeof(szBroadcastAddr), GET_BROADCAST);

		set_if_address((char *) IFNAME, szBroadcastAddr, SET_BROADCAST);

#ifndef DEL_MULTICAST_ROUTE
		mysystem("route add -net 224.0.0.0 netmask 224.0.0.0 dev eth0");//组播
#endif
	    m_bIsAddNetRoute = 0;	

		strncpy(m_sLocalIP, ipaddr, sizeof(m_sLocalIP) - 1);	
		m_pCConfig->SetValue(INet_CfgSection, INet_IP_key, m_sLocalIP);
		saveIpBak(m_sLocalIP);

        XTRACE_MUTEX_UNLOCK(&m_IpDhcpLock);  
        
        return 0;
}

/*
*判断ipc是否有连接，只判断命令端口、媒体端口、rtsp端口
*/
int INet::NET_IfIPCConnected(int *connected)
{
    if (0 == bSupportIPAdjust)
	{
        NET_INFO(" not support IPAdjust\n");
        return 0;
    }

    if (NULL == connected)
    {
        return -1;
    }

	int ret;
	int MediaPort,RtspPort;
	char command[64];

	*connected = 0;
	//CommandPort = m_pCConfig->GetValue(INet_PortCfgSection, "CommandPort", 6060);
	MediaPort = m_pCConfig->GetValue(INet_PortCfgSection, "MediaPort", 6066);
	RtspPort = m_pCConfig->GetValue(INet_PortCfgSection, "RtpPort", 554);

	memset(command, 0, sizeof(command));
	snprintf(command, sizeof(command), "netstat -an | grep %d", MediaPort);
	NET_DBG("%s\n", command);
	ret = IsStringInCommand(command, "ESTABLISHED");
	if(1 == ret)
	{
		NET_DBG("IPC is connected!\n");
		*connected = 1;
		return 0;
	}

	memset(command, 0, sizeof(command));
	snprintf(command, sizeof(command), "netstat -an | grep %d", RtspPort);
	NET_DBG("%s\n", command);
	ret = IsStringInCommand(command, "ESTABLISHED");
	if(1 == ret)
	{
		NET_DBG("IPC is connected!\n");
		*connected = 1;
		return 0;
	}

	return 0;
}

/*
*判断IPC当前IP是否跟后端同网段
*/
int INet::NET_ifChangeNet(const char *pszIPAddress)
{
    if (0 == bSupportIPAdjust)
	{
        NET_INFO(" not support IPAdjust\n");
        return 0;
    }

	int dat1,dat2,dat3,dat4;
	int dat11,dat22,dat33,dat44;
	char localIPAddress[MAX_IP_STR_LEN] = {0};

	sscanf(pszIPAddress, "%d.%d.%d.%d", &dat1,&dat2,&dat3,&dat4);
	NET_GetIPAddress(localIPAddress, sizeof(localIPAddress));
	sscanf(localIPAddress, "%d.%d.%d.%d", &dat11,&dat22,&dat33,&dat44);
	NET_DBG("NVR IP:%s, IPC:%s!\n", pszIPAddress, localIPAddress);

	if((dat1==dat11) && (dat2==dat22) && (dat3==dat33))
		return 1;
	else
		return 0;
}

int INet::NET_SetIPAddressAdjustEnable(int enable)
{
    if (bSupportIPAdjust)
    {
        m_bIP_Adjust_Enbale = (enable > 0 ? 1 : 0);
        m_pCConfig->SetValue(INet_CfgSection, INet_IPAdjust_key, m_bIP_Adjust_Enbale);
    }
    else
    {
        m_bIP_Adjust_Enbale = 0;
    }

	saveIPAdjustBak(m_bIP_Adjust_Enbale);
	
	return 0;
}

int INet::NET_GetIPAddressAdjustEnable(int *enable)
{
    if (bSupportIPAdjust)
    {
        if(NULL == enable)
        {
            NET_ERR("NULL == enable\n");
            return -1;
        }
        *enable = m_bIP_Adjust_Enbale;
    }
    else
    {
        // 探测协议库中做了兼容处理，传出的值做了+1处理
        // -1 - 不支持 0 - 关闭 1 - 开启
        *enable = -1;
    }

	return 0;
}

int INet::NET_IPAddressIfSameWithNvr(const char *pszIPAddress, int *same)
{
    if (0 == bSupportIPAdjust)
	{
        //NET_INFO(" not support IPAdjust\n");
        return 0;
    }

	int connected = 0;
	char localIP[IP_MAX_LEN] = {0};
	char defaultIP[IP_MAX_LEN] = {0};

	NET_DBG("into NET_IPAddressIfSameWithNvr!\n");
	if (NULL == same)
	{
		NET_ERR("NULL == same\n");
		return -1;
	}
	*same = 1;

	//是否开启IP自适应
	if(0 == m_bIP_Adjust_Enbale)
	{
		NET_DBG("IP adjust is disable!\n");
		return 0;
	}

	if (NULL == pszIPAddress)
	{
		NET_ERR("NULL == pszIPAddress || NULL == same\n");
		return -1;
	}

	//检查IP是否合法
	if (!IsValidIP(pszIPAddress))
	{
		NET_ERR("invalid ip =%s\n", pszIPAddress);
		return -1;
	}

    //检查IP地址最后一位是否是0或者255
    if (!IsValidIpAddr(pszIPAddress))
    {
		NET_ERR("invalid ip =%s\n", pszIPAddress);
		return -1;
    }

    int first = atoi(pszIPAddress);
    if((0 == first) || (127 == first) || (first > 223))
    {
        NET_ERR("ip first is invalid: %s\n", pszIPAddress);
        return -1;
    }
	
	//是否是同网段
	if(NET_ifChangeNet(pszIPAddress))
	{
		if (1 == firstSameIP)
		{
			m_pCConfig->GetValue(INet_CfgSection, "LocalIP", "192.168.1.11", localIP, sizeof(localIP));
			m_pCConfig->GetValue(INet_CfgSection, "LocalIP", "192.168.1.11", localIP, sizeof(localIP));

			//获取新的配置文件读取实例
			int ret = 0;
			CConfig *pDefCfg = CConfig::NewInstance();
			if (NULL != pDefCfg)
			{
				ret = pDefCfg->LoadFile(IDVR_DEFAULT_CONFIG_FILE_PATH);
				if (0 == ret)
				{
					pDefCfg->GetValue(INet_CfgSection, "LocalIP", "192.168.1.11", defaultIP, sizeof(defaultIP));
				}
			}
			pDefCfg->ReleaseInstance(pDefCfg);
			if (strncmp(localIP, defaultIP, IP_MAX_LEN))//当前IP是否和默认IP相同，相同则可以改IP，不相同不能改IP
			{
				return 0;
				NET_DBG("IPC the local ip is same as deflaut ip, not do IP adjust!\n");
			}
		}
		else
		{
			NET_DBG("IPC in the same network segment, not do IP adjust!\n");
			return 0;
		}
	}

	if(strstr(pszIPAddress, "10.0.0."))
	{
		NET_DBG("Don't adjust, NVR ip is %s\n", pszIPAddress);
		return 0;
	}

	//是否有连接
	NET_IfIPCConnected(&connected);
	if(connected)
	{
		NET_DBG("I am connected, don't do IP adjust!\n");
		return 0;
	}
	*same = 0;

	return 0;
}

/*
 * 自适应IP功能，根据后端探测设备的网络信息设置IPC
 */
int INet::NET_SetIPAddressByNVR(const char *pszIPAddress)
{
    if (0 == bSupportIPAdjust)
	{
        NET_INFO(" not support IPAdjust\n");
        return 0;
    }

	int ret = 0;
	int i = 0;
	int dat1,dat2,dat3,dat4;
	int dat11,dat22,dat33,dat44;

	NET_DBG("Into NET_SetIPAddressByNVR-------pszIPAddress:%s--!\n", pszIPAddress);

	//是否开启IP自适应
	if(0 == m_bIP_Adjust_Enbale)
	{
		NET_DBG("IP adjust is disable!\n");
		return 0;
	}
	
	XTRACE_MUTEX_LOCK(&m_detectResourceLock);

	if (detectNum >= DETECT_MAX_NUM)
	{
        NET_ERR("detect queue is full\n");
		XTRACE_MUTEX_UNLOCK(&m_detectResourceLock);
		return -1;
	}
	else
	{
		if (NULL == pszIPAddress)
		{
			NET_ERR("NULL == pszIPAddress\n");
			XTRACE_MUTEX_UNLOCK(&m_detectResourceLock);
			return -1;
		}

		//检查IP是否合法
		if (!IsValidIP(pszIPAddress))
		{
			NET_ERR("invalid ip =%s\n", pszIPAddress);
			XTRACE_MUTEX_UNLOCK(&m_detectResourceLock);
			return -1;
		}

	    //检查IP地址最后一位是否是0或者255
	    if (!IsValidIpAddr(pszIPAddress))
	    {
			NET_ERR("invalid ip =%s\n", pszIPAddress);
			XTRACE_MUTEX_UNLOCK(&m_detectResourceLock);
			return -1;
	    }

	    int first = atoi(pszIPAddress);
	    if((0 == first) || (127 == first) || (first > 223))
	    {
	        NET_ERR("ip first is invalid: %s\n", pszIPAddress);
			XTRACE_MUTEX_UNLOCK(&m_detectResourceLock);
	        return -1;
	    }
		
		//判断是否有重复的探测源(同一网段)
		sscanf(pszIPAddress, "%d.%d.%d.%d", &dat11,&dat22,&dat33,&dat44);
		for (i = 0; i < detectNum; i++)
		{
			sscanf(detectQueue[i], "%d.%d.%d.%d", &dat1,&dat2,&dat3,&dat4);
			if ((dat1 == dat11) && (dat2 == dat22) && (dat3 == dat33))
			{
				NET_ERR("network segment is same\n");
				XTRACE_MUTEX_UNLOCK(&m_detectResourceLock);
				return -1;
			}
		}

		memcpy(detectQueue[detectNum++], pszIPAddress, IP_MAX_LEN);
		XTRACE_MUTEX_UNLOCK(&m_detectResourceLock);
	}

	return ret;
}

#if defined (__USE_FTP__)
/*-----------------------------------FTP相关操作-------------------------------*/
int INet::NET_SetMotionCallBack(int (*CallBack) (int))			//ftp移动侦测回调
{
#if 0
//  m_FxnMotionCallBack = CallBack;

    if (SetMotionCallBack(CallBack) == -1)
    {
        return -1;
    }
#endif    
    return 0;
}

/* 设置用户的ftp参数 */
int INet::NET_SetFtpParam(const pFTP_user_param pFtpUsrParam)
{
    if (NULL == pFtpUsrParam)
    {
        NET_ERR("NULL == pszSubnetMask\n");
        return -1;
	}

    /* 判断参数是否发生改变 */
    if (   (m_FtpUsrInfo.ftpenable   == pFtpUsrParam->ftpenable)
        && (m_FtpUsrInfo.port        == pFtpUsrParam->port)
        && (m_FtpUsrInfo.transt_type == pFtpUsrParam->transt_type)
        && (m_FtpUsrInfo.nFtpMode	 == pFtpUsrParam->nFtpMode)
        && (strcmp(m_FtpUsrInfo.host, pFtpUsrParam->host) == 0)
        && (strcmp(m_FtpUsrInfo.user, pFtpUsrParam->user) == 0)
        && (strcmp(m_FtpUsrInfo.pass, pFtpUsrParam->pass) == 0)
        && (strcmp(m_FtpUsrInfo.rmPath,pFtpUsrParam->rmPath)==0)
       )
    {
        NET_WARN("\n[INet.cpp]ftp param is not change!\n\n");
        return 0;
    }

    SetFtpParam(pFtpUsrParam, m_sTutkUID); //将用户摄制的参数放到全局变量中
	
    memcpy(&m_FtpUsrInfo, pFtpUsrParam, sizeof(m_FtpUsrInfo));

	m_pCConfig->SetValue(IFTP_CfgSection, IFTP_Enable_key, m_FtpUsrInfo.ftpenable);
	m_pCConfig->SetValue(IFTP_CfgSection, IFTP_Port_key, m_FtpUsrInfo.port);
	m_pCConfig->SetValue(IFTP_CfgSection, IFTP_Type_key, m_FtpUsrInfo.transt_type);
	m_pCConfig->SetValue(IFTP_CfgSection, IFTP_Host_key, m_FtpUsrInfo.host);
	m_pCConfig->SetValue(IFTP_CfgSection, IFTP_User_key, m_FtpUsrInfo.user);
	m_pCConfig->SetValue(IFTP_CfgSection, IFTP_Pass_key, m_FtpUsrInfo.pass);
	m_pCConfig->SetValue(IFTP_CfgSection, IFTP_Mode_key, m_FtpUsrInfo.nFtpMode);
	m_pCConfig->SetValue(IFTP_CfgSection, IFTP_RmPath_key, m_FtpUsrInfo.rmPath);

	return 0;
}

/* 获取用户设置的ftp参数 */
int INet::NET_GetFtpParam(pFTP_user_param pFtpUsrParam)
{
    memcpy(pFtpUsrParam, &m_FtpUsrInfo, sizeof(m_FtpUsrInfo));

    /* 还可以调用以下接口进行获取 */
    //GetFtpParam(pFtpUsrParam);
    
    return 0;
}

/* 获取当前的FTP主机连接状态(成功return 1；失败 return 0;) */
int INet::NET_GetFtpConnStatus()
{
    int iStatus = 0;

    /* 如果有线或者无线至少有一个连接上去 */
    if ((m_iNetStatus==0) || (m_iWlan_Status==0))
    {
        iStatus = GetFtpConnStatus();
    }else{
        iStatus = 0;
    }
    
    return iStatus;
}

/* 获取FTP传输类型(0:不传 1:图片 2:录像 3:ALL ) */
int INet::NET_GetFtpTransferType(int *p_transfer_type)
{
    if (NULL == p_transfer_type)
    {
        NET_ERR("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    *p_transfer_type = m_FtpUsrInfo.transt_type;

    return 0;
}

/*-----------------------------------end of FTP--------------------------*/
#endif

/* 获取当前哪一个网卡有效 */
int INet::NET_GetCurrNetCardType(int *pCurrNetCardType)
{	
    *pCurrNetCardType = m_nCurrNetCardType;

    return 0;
}

/* 在设置完网络参数之后需要更新路由 */
int INet::NET_NeedUpdateRouter(void)
{
    bNeedUpdateRouter = 1;
    
    return 0;
}

void INet::NET_InitAuthP2pInfo()
{
	int nRet = 0;
	StuAuthP2pInfo stAuthP2pData = {0};
	
	nRet = Auth_GetAuthP2pInfo(&stAuthP2pData); 
	if (nRet < 0)
	{
		NET_ERR("Auth_GetKeepAliveInfo failed (%d)\n", nRet);
	}

	
    if(1 == stAuthP2pData.iP2PHDR)
    {
        m_AuthP2pInfo.iP2PHDR = 1;
    }
    else
    {
        m_AuthP2pInfo.iP2PHDR = 0;
    }
	NET_INFO("NET_InitAuthP2pInfo iP2PHDR:%d\n",m_AuthP2pInfo.iP2PHDR);
}


//##ModelId=49BB426A02C5
INet::INet()
{
	int i = 0;
	
	m_hCheckThread = (pthread_t) NULL;
	m_IPAdjustCheckThread = (pthread_t) NULL;
	m_hCheckThread = (pthread_t) NULL;
	m_IPAdjustCheckThread = (pthread_t) NULL;
	m_UpnpCheckThread = (pthread_t) NULL;
    m_OperateRouterThread = (pthread_t) NULL;
    m_FtpOperateThread = (pthread_t) NULL;
#if defined (__IS_SUPPORT_WLAN__)
    m_SendWifiTestCommandThread = (pthread_t) NULL;
    m_wifiApStaStatusCheckThread = (pthread_t) NULL;
#endif

	m_hPushInitThread= (pthread_t) NULL;
#if defined (__USE_APPLY_DID_MANUAL__)
    m_ApplyDID = (pthread_t) NULL;
#endif

#ifdef __IS_SUPPORT_AIC_BLUETOOTH__
		m_btProTestFunc = (pthread_t) NULL;
#endif

	m_bCheck_exit = 0;
	m_bZWPushInit = 0;
	m_iNetStatus = 2;			//未知
	//  m_iNetStatus_last = 2;
	m_iArpReplyBuffSize = 0;
	m_iArpPacketSize = 0;

	m_bDHCP_Enable = 0;
	m_iPconf_Enable=FALSE;
	m_pCConfig = NULL;
	m_pArpReplyBuff = NULL;
	m_pArpPacket = NULL;
	m_sLocalIP_old[0] = 0;
#if defined (__IS_SUPPORT_WLAN__)
	m_bIsConfigureNetIng = 0;
	m_nUdpPushStatus = 0;
#endif
	m_YLDid=0;
	TcpRelayEnable = 0;
	m_bBeep[0] = 0;
	m_bSTS[0] = 0;
	m_bEmail[0] = 0;
	m_bSMS[0] = 0;
	m_bDOP[0] = 0;
	m_bGUI[0] = 0;
	m_bRecord[0] = 0;
	m_bPush[0] = 0;
	m_bLightAlarm[0] = 0;

	m_bBeep[1] = 0;
	m_bSTS[1] = 0;
	m_bEmail[1] = 0;
	m_bSMS[1] = 0;
	m_bDOP[1] = 0;
	m_bGUI[1] = 0;
	m_bRecord[1] = 0;
	m_bPush[1] = 0;
	m_bLightAlarm[1] = 0;

	m_bIP_Adjust_Enbale = 1;
#if defined (__IS_SUPPORT_WLAN__)
    m_wifiMode = WLAN_STA_MODE;
    m_wifiToStaFlag = 0;
    m_changeWifiConnectFlag = 0;
	m_wifiFakeSTAModeEnable = 0;
	m_curToolStatus = noToolRun;
	memset(m_AccessPoints, 0, sizeof(m_AccessPoints));
#endif
	memset(m_P2PDownloadURL, 0, sizeof(m_P2PDownloadURL));

	memset(&m_tWlan_Connection, 0, sizeof(m_tWlan_Connection));	
	memset(m_sTutkUID, 0, sizeof(m_sTutkUID));
	memset(m_szPushSubKey, 0, sizeof(m_szPushSubKey));
	m_bIsSuportSubKey = 0;
#if defined (__IS_SUPPORT_WLAN__)
	memset(m_szSessionID, 0, sizeof(m_szSessionID));
	m_wifiConnChange = 0;
	m_nWifiLinkStatus = 0;
	m_iWlanScanStatus = 0;
#endif
    m_supportSetDid = 1;


	for (i = 0; i < DETECT_MAX_NUM; i++)
	{
		memset(detectQueue[i], 0, IP_MAX_LEN);
	}

	detectNum = 0;
	firstSameIP = 1;

#if defined (__USE_APPLY_DID_MANUAL__)
    m_iApplyStatus = 1;
    m_iApplyErrCode = 0;
    m_iApplyFromSFlag = 0;
    m_RequestUrlPort = 8089;
    memset(m_RequestUrl, 0, MAX_REQUEST_URL_LEN);
#endif

	memset(m_sPrimaryDNS, 0, sizeof(m_sPrimaryDNS));
	memset(m_sSecondaryDNS, 0, sizeof(m_sSecondaryDNS));
	s_NetCardDhcpInfo.s_Eth0DhcpSuccFlag = 0;
	s_NetCardDhcpInfo.s_Eth0DhcpSuccTime = 0;
	s_NetCardDhcpInfo.s_Wlan0DhcpSuccFlag = 0;
	s_NetCardDhcpInfo.s_Wlan0DhcpSuccTime = 0;
	s_NetCardDhcpInfo.SetDNSFileLock = PTHREAD_MUTEX_INITIALIZER;

#ifdef __MODULE_4G_SUPPORT__
	m_bWlan_Enable = 2;
	m_P2pProductMode = 0;	
	memset(m_PcMac, 0, sizeof(m_PcMac));	
	m_iConnServer = 1;
    m_IsJinRuiSimCard = -1;
	m_IsBindState = -1;
	m_CardTypeTmp = -1;
	m_CardTypeTmp_BW = -1;
	m_CardTypeTmp_Clould = -1;
	m_nAllowUseEth0 = 0;
	m_nProductState = 0;
    m_FlowCustomType = CUSTOMTYPE_NOTFLOW;
    memset(m_szUserId, 0, sizeof(m_szUserId));
    memset(m_szServerAddress, 0, sizeof(m_szServerAddress));
    memset(m_szRandomNumber, 0, sizeof(m_szRandomNumber));
    m_b4gScanEnd = 0;
#endif

#ifdef __IS_SUPPORT_AIC_BLUETOOTH__
			m_bIsBlueToothOpen = 0;
			if(0 == access(PRODUCTTEST_WIFI_FILE, 0))
			{
				m_blueMode = btProTest;
			}
			else
			{
				m_blueMode = btGeneral;
			}
#endif



}

int INet::Init()
{
//	NET_DBG("--------Inet.cpp-----Net::init()------初始化网络-----------\n");
	int ret;
#if defined (__IS_SUPPORT_WLAN__)
	char lastSetValue[20] = {0};
    unsigned int realWifiMode = 0;
	char buf[256] = {0};
#endif
	//检测是否已经初始化
	if ((NULL != (void *) m_hCheckThread) || (m_bCheck_exit))
	{
		//NET_ERR("inited\n");
		if(m_bCheck_exit)
			NET_DBG("Check Net has been exit, no init again!\n");
		return 0;
	}

	//获取WIFI网卡设备名字
#if defined (__IS_SUPPORT_WLAN__)
	NET_GetWiFiModule();
#endif

	//ARP
	m_iArpPacketSize = sizeof (struct ethhdr) + sizeof (struct ether_arp);
	m_iArpPacketSize += 4;
	m_pArpPacket = (unsigned char *) new char[m_iArpPacketSize];

	//ARP包缓冲分配是否失败
	if (NULL == m_pArpPacket)
	{
		NET_ERR("NULL == m_pArpPacket\n");
		UnInit();
		return -1;
	}
	memset(m_pArpPacket, 0, m_iArpPacketSize);
	//应答缓冲分配
	m_iArpReplyBuffSize = 65535;
	m_pArpReplyBuff = new char[m_iArpReplyBuffSize];

	if (NULL == m_pArpReplyBuff)
	{
		NET_ERR("NULL == m_pArpReplyBuff\n");
		UnInit();
		return -1;
	}

	memset(m_pArpReplyBuff, 0, m_iArpReplyBuffSize);
	//获取配置实例
	NET_DBG("--------Inet.cpp-----Net::init()------获取配置实例-----------------\n");
	m_pCConfig = CConfig::GetInstance();
	if (NULL == m_pCConfig)
	{
		NET_ERR("NULL == m_pCConfig\n");
		UnInit();
		return -1;
	}

#if defined (__IS_SUPPORT_WLAN__)
	g_DistributionNetwordFlag = 0;
#endif
	m_iNetStatus = 2;			//未知
	m_iWlan_Status = 2;
	m_iWlan_DHCP_Status = 0;    //未连接
	//  m_iNetStatus_last = 2;
	m_iNetDHCPFailed = 0;
	m_iWlan_DHCPFailed = 0;
    m_ifRouteDeleted = 0;
    m_bIsAddWifiRoute = 0;
    m_bIsAddNetRoute = 0;
	m_iNetDHCPEnableShadow = 0;
	m_iNetDHCPFailedTimes = 0;
#if defined (__IS_SUPPORT_WLAN__)
	m_wifiApToStaCfgOver = 0;
	m_wifiCfgMode = eWifiCfgMode_Normal;
    m_wifiSdCfgMode = eWifiCfgMode_Normal;
#endif
	//将这两个数组初始化为空字符，防止乱码
	memset(m_sTutkUID,0,sizeof(char)*MAX_UID);
	memset(m_sUlkUID,0,sizeof(char)*MAX_UID);

	XTRACE_MUTEX_INIT(&m_writeLock, NULL);
	XTRACE_MUTEX_INIT(&m_setIPLock, NULL);
	XTRACE_MUTEX_INIT(&m_FileLock, NULL);
	XTRACE_MUTEX_INIT(&m_HTMLLock, NULL);
	XTRACE_MUTEX_INIT(&m_OperateRouteLock, NULL);
	XTRACE_MUTEX_INIT(&m_OperateDhcpLock, NULL);
    XTRACE_MUTEX_INIT(&m_IpDhcpLock, NULL);
	XTRACE_MUTEX_INIT(&m_detectResourceLock, NULL);
#if defined (__IS_SUPPORT_WLAN__)
	XTRACE_MUTEX_INIT(&m_muUdpPushLock, NULL);
	XTRACE_MUTEX_INIT(&m_WifiLock, NULL);
	XTRACE_MUTEX_INIT(&m_SetWifiModeLock, NULL);
    XTRACE_MUTEX_INIT(&m_SetWifiConnectLock, NULL);
#endif
	XTRACE_MUTEX_INIT(&m_ThreadParamLock, NULL);
    m_bWifiTestIsBusy = 0;
    m_bWifiTestHasSet = 0;

    m_acDomainSuffixExist = 0;
    memset(m_acDomainSuffix, 0, sizeof(m_acDomainSuffix));
    m_acDomainSuffixExist_Test = 0;
    memset(m_acDomainSuffix_Test, 0, sizeof(m_acDomainSuffix_Test));

	m_YLDid = 0;
    sem_init(&m_semWakeThread, 0, 0);

    Net_LoadAuthPushParam();

#ifdef __IS_SUPPORT_AIC_BLUETOOTH__
		m_blueProTestStatus = (int)btProTestfalse;
		m_isProTestClient = 0;
#endif

	//导入配置
	NET_DBG("----导入配置----Inet.cpp-----Net::init()------loadparam()-----------------\n");
	LoadParam();

    if (bSupportPhy)
    {
        // 默认为有线网卡
        m_nCurrNetCardType = 1;
    }
    else
    {
        // 默认为无线网卡
        m_nCurrNetCardType = 3;
    }

#ifdef USE_HOSTS_PARSER
    ret = Hosts_Initialize(IHOST_CONF, IHOST_PARSER_CACHE_CONF, RESOLV_CONF);
    if (ret)
    {
        NET_ERR("Fail to called Hosts_Initialize, errno = %d\n", ret);
    }
    else
    {
        NET_INFO("Hosts_Initialize Called over!\n");
    }
#endif
#if defined (__IS_SUPPORT_WLAN__)
	if (bSupportWIFI)
	{
    	if (0 != access(WIFI_SD_PATH, F_OK))
    	{
    	    char did[MAX_DID_LENGTH] = {0};
    		/* AP模式的SSID由"g_szDefaultApPrefix + UID"组成 */
    		memset(g_nSoftApSsid, 0, sizeof(g_nSoftApSsid));
    		if (strlen(m_sTutkUID) < 20)
    		{
    			FormatSoftapSSID("eth0", g_nSoftApSsid,sizeof(g_nSoftApSsid));
    			NET_WARN("TutkUid is invalid, default: %s\n", g_nSoftApSsid);
    		}
    		else
    		{
    			if (',' == m_sTutkUID[19])//尚云DID号在逗号前面
    			{
    				strncpy(did, m_sTutkUID, 19);
    				snprintf(g_nSoftApSsid, sizeof(g_nSoftApSsid), "%s%s", 
    							g_szDefaultApPrefix, did);
    			}
    			else
    			{
    				 
    				snprintf(g_nSoftApSsid, sizeof(g_nSoftApSsid), "%s%s", 
    							g_szDefaultApPrefix, m_sTutkUID);
    			}
    		}

    		/* BleName由"g_szBlePrefix + UID"组成 */
    		memset(g_nBleName, 0, sizeof(g_nBleName));
    		if (strlen(m_sTutkUID) < 20)
    		{
    			FormatSoftBleName((char *)"wlan0");
    			NET_WARN("TutkUid is invalid, BleName default: %s\n", g_nBleName);
    		}
    		else
    		{
    			if (',' == m_sTutkUID[19])//尚云DID号在逗号前面
    			{
    				strncpy(did, m_sTutkUID, 19);
    				snprintf(g_nBleName, sizeof(g_nBleName), "%s%s", 
    							g_szBlePrefix, did);
    			}
    			else
    			{
    				snprintf(g_nBleName, sizeof(g_nBleName), "%s%s", 
    							g_szBlePrefix, m_sTutkUID);
    			}
    		}
                
			if (WIFI_MODULE_HI_H383U == m_eCurrUSBWifiDev)
            {
#ifdef __IS_SUPPORT_HIS_BLUETOOTH__
                His_BLE_setBluetoothName((const char*)g_nBleName);
                His_BLE_registerBluetoothCallback(ParseBleDataCallback);
#endif
            }
    		WIFI_INIT();  /* 启动对应的驱动 */
			NET_INFO("WIFI_INIT end\n");
    		/* 该选项必须放在LoadParam()函数之后，否则bSupportWlanAP和m_sTutkUID不会赋值 */
    		if (bSupportWlanAP)
    		{
    			m_wifiMode = (eWifiMode)(m_pCConfig->GetValue(INet_CfgSection, INet_WLANWifiMode, WLAN_STA_MODE));
    			if ((access(WIFI_MODE_0_BAK_FILE_PATH, F_OK)==0)\
    				|| (access(WIFI_MODE_1_BAK_FILE_PATH, F_OK)==0)\
    				|| (access(WIFI_MODE_2_BAK_FILE_PATH, F_OK)==0))
    			{
    				memset(lastSetValue, 0, sizeof(lastSetValue));
    				memset(buf, 0, sizeof(buf));
    				strncpy(buf, "ls /mnt/mtd/config/.WlanWifiMode* | awk -F \":\" '{printf $2}'",sizeof(buf) - 1);
    				ShellCmdOut(buf, lastSetValue, sizeof(lastSetValue) - 1);
    				int lastWlanwifimode = 0;
    				sscanf(lastSetValue, "%d", &lastWlanwifimode);
    				if((0 == lastWlanwifimode) || (1 == lastWlanwifimode) || (2 == lastWlanwifimode))
    				{
    					if(m_wifiMode != ((eWifiMode)lastWlanwifimode))
    					{
    						NET_INFO("Need reset dhcp: from %d to %d\n", m_wifiMode, lastWlanwifimode);
    						m_wifiMode = (eWifiMode)lastWlanwifimode;
    						m_pCConfig->SetValue(INet_CfgSection, INet_WLANWifiMode, lastWlanwifimode);
    					}
    				}
    				else
    				{
    					NET_ERR("Saved aptostaFlag cfg error!\n");
    				}
    			}
    			else
    			{
    				saveWlanWifiModeBak(m_wifiMode);	
    			}
    			strncpy(IWNAME, "wlan0",sizeof(IWNAME) - 1);

    			//起来需检测配置与实际网卡模式是否匹配
    			if (WIFI_MODULE_HI_H383U == m_eCurrUSBWifiDev
					|| WIFI_MODULE_ATBM_6132U == m_eCurrUSBWifiDev
					|| WIFI_MODULE_AIC_8800DL == m_eCurrUSBWifiDev
					|| WIFI_MODULE_AIC_8800DL_2_4G == m_eCurrUSBWifiDev)
    			{
    				if (IsStringInCommand("ps", "wpa_supplicant"))
    				{
    					realWifiMode = WLAN_STA_MODE;
    				}
    				else if (IsStringInCommand("ps", "hostapd"))
    				{
    					realWifiMode = SOFT_AP_MODE;
    				}
    				else
    				{
    					realWifiMode = UNKNOWN_MODE;
    				}
    			}
    			else 
    			{
    				realWifiMode = NO_WIFI_DEV;
    			}
    			m_wifiFakeSTAModeEnable = m_pCConfig->GetValue(INet_CfgSection, INet_WLANFakeSTAMode, 0);
    			if((m_wifiFakeSTAModeEnable) && (SOFT_AP_MODE == m_wifiMode))
    			{
    				m_wifiFakeSTAModeEnable = 0;
    				m_pCConfig->SetValue(INet_CfgSection, INet_WLANFakeSTAMode, 0);
    				NET_ERR("m_wifiFakeSTAModeEnable=1 don't exist in AP mode, clear WLANFakeSTAMode flag!\n");
    			}
				NET_INFO("--------Inet.cpp----(%d, %d) --start\n",m_wifiMode,realWifiMode); 
    			//检查配置是否与实际wifi驱动一致
    			if(m_wifiMode != realWifiMode)
    			{
    				if(NO_WIFI_DEV == realWifiMode)
    				{
    					 
    					m_wifiMode = NO_WIFI_DEV;
    					m_pCConfig->SetValue(INet_CfgSection, INet_WLANWifiMode, m_wifiMode);
    					saveWlanWifiModeBak(m_wifiMode);
    				}
    				else if(WLAN_STA_MODE == realWifiMode)//需要切出AP模式
    				{
    					NET_INFO("--------Inet.cpp----NET_SetWifiMode(2, 1) --start\n");
    					NET_SetWifiMode(2, 1);						
						NET_INFO("--------Inet.cpp----NET_SetWifiMode(2, 1) --end\n");
    				}
    				else if(SOFT_AP_MODE == realWifiMode)
    				{
    					NET_INFO("--------Inet.cpp----NET_SetWifiMode(1, 1) --start\n"); 
    					NET_SetWifiMode(1, 1);
						NET_INFO("--------Inet.cpp----NET_SetWifiMode(1, 1) --start\n");
    					NET_ERR("--------Inet.cpp-----Net::init()---m_wifiMode %d----realWifiMode %d--\n", m_wifiMode, realWifiMode);
    				}
    			}
    		}
    	}
    	else
    	{
    		//支持简化版本配网文件(不需要自带首尾标志)
    		//确保SD卡配网文件有首尾标志
    		FILE *pCfgFile = fopen(WIFI_SD_PATH, "rb");
    		if (NULL != pCfgFile)
    		{
    			FILE *pCfgFile2 = fopen(WIFI_SD_TMP_PATH, "wb");
    			if (NULL != pCfgFile2)
    			{
    				fwrite(CFG_FILE_HEAD_FLAG, strlen(CFG_FILE_HEAD_FLAG), 1, pCfgFile2);
    				
    				memset(buf, 0, sizeof(buf));
    				while (fgets(buf, sizeof(buf), pCfgFile) != NULL)
    				{
    					fwrite(buf, strlen(buf), 1, pCfgFile2);
    					memset(buf, 0, sizeof(buf));
    				}
    				
    				fwrite(CFG_FILE_END_FLAG, strlen(CFG_FILE_END_FLAG), 1, pCfgFile2);

    				fclose(pCfgFile2);
    				pCfgFile2 = NULL;

    				memset(buf, 0, sizeof(buf));
    				snprintf(buf, sizeof(buf), "rm %s", WIFI_SD_PATH);
    				mysystem(buf);
    				memset(buf, 0, sizeof(buf));
    				snprintf(buf, sizeof(buf), "mv %s %s", WIFI_SD_TMP_PATH, WIFI_SD_PATH);
    				mysystem(buf);
    			}

    			fclose(pCfgFile);
    			pCfgFile = NULL;
    		}
    	}
	    NET_DBG("--------Inet.cpp-----Net::init()------WLAN_LoadParam()-----------------\n");
	    WLAN_LoadParam();
    }
#endif
	//激活网络
	NET_DBG("---激活网络-----Inet.cpp-----Net::init()--------------NET_ReActive()--------------\n");
	NET_ReActive();

	m_bCheck_exit = 0;
	//Fnx_PushInitThread(this);
	if(m_sTutkUID[0]=='Y'&&m_sTutkUID[1]=='L')
	{
		m_YLDid =1;
	}

	//4G模块初始化处理
#ifdef __MODULE_4G_SUPPORT__
	NET_4GModule_Init();
	Module4G_SetDevDidType(m_YLDid);
	if(m_YLDid &&  0 == TcpRelayEnable)
	{
		m_pCConfig->SetValue(TcpRelay, TcpEnable, 1);
		TcpRelayEnable = 1;
	}
#endif 

	/* 初始化设置p2p信息 */
	NET_InitAuthP2pInfo();

	// 初始的时候需要填充有线网卡默认有效
#if defined (__IS_SUPPORT_WLAN__)
    if (bSupportWIFI)
	{
        mysystem("echo 1 > /tmp/.effect_net_card");
    }
#endif
	#if defined (__USE_P2P_PUSH__)
	//Push线程
	NET_DBG("--------Inet.cpp-----Net::init()----------创建Push线程--------------\n");
	ret = pthread_create(&m_hPushInitThread, NULL, Fnx_PushInitThread, this);
	if (ret < 0)
	{
		NET_ERR("pthread_create<Fnx_PushInitThread> failed(%d)\n", ret);
		UnInit();
		return -1;
	}
	#endif

	//创建注册检测线程
	NET_DBG("--------Inet.cpp-----Net::init()----------创建注册检测线程--------------\n");
	ret = pthread_create(&m_hCheckThread, NULL, Fnx_CheckThread, this);
	if (ret < 0)
	{
		NET_ERR("pthread_create<Fnx_CheckThread> failed(%d)\n", ret);
		UnInit();
		return -1;
	}

	if (bSupportIPAdjust)
	{
        NET_DBG("--------Inet.cpp-----Net::init()----------创建IP自适应检测线程--------------\n");
    	ret = pthread_create(&m_IPAdjustCheckThread, NULL, IPAdjust_CheckThread, this);
    	if (ret < 0)
    	{
    		NET_ERR("pthread_create<IPAdjust_CheckThread> failed(%d)\n", ret);
    		UnInit();
    		return -1;
    	}

		NET_DBG("--------Inet.cpp-----Net::init()----------创建IP自适处理应线程--------------\n");
    	ret = pthread_create(&m_IPAdjustProcessThread, NULL, IPAdjustProcessThread, this);
    	if (ret < 0)
    	{
    		NET_ERR("pthread_create<IPAdjustProcessThread> failed(%d)\n", ret);
    		UnInit();
    		return -1;
    	}
	}

 #if defined (__IS_SUPPORT_WLAN__) 
    if (bSupportWIFI)
    {
        NET_DBG("--------Inet.cpp-----Net::init()----------创建ROUTE检测线程--------------\n");
        ret = pthread_create(&m_OperateRouterThread, NULL, OperateRouterThread, this);
        if (ret < 0)
        {
            NET_ERR("pthread_create<OperateRouterThread> failed(%d)\n", ret);
            UnInit();
            return -1;
        }

        NET_DBG("--------Inet.cpp-----Net::init()----------创建发送WIFI测试线程--------------\n");
        ret = pthread_create(&m_SendWifiTestCommandThread, NULL, SendWifiTestCommandThread, this);
        if (ret < 0)
        {
            NET_ERR("pthread_create<SendWifiTestCommandThread> failed(%d)\n", ret);
            UnInit();
            return -1;
        }
    }
    if (bSupportWlanAP)
    {
        // AP与STA相关
        ret = pthread_create(&m_wifiApStaStatusCheckThread, NULL, ApStaStatusCheckThread, this);
        if (ret < 0)
        {
            NET_ERR("pthread_create<ApStaStatusCheckThread> failed(%d)\n", ret);
            UnInit();
            return -1;
        }
    }
#endif

#ifdef __IS_SUPPORT_AIC_BLUETOOTH__
	if((btProTest == m_blueMode) 
		&& (WIFI_MODULE_ATBM_6132U == m_eCurrUSBWifiDev 
		|| WIFI_MODULE_AIC_8800DL == m_eCurrUSBWifiDev
		|| WIFI_MODULE_AIC_8800DL_2_4G == m_eCurrUSBWifiDev))
	{
		ret = pthread_create(&m_btProTestFunc, NULL, btProTestFuncThread, this);
		if (ret < 0)
		{
			NET_ERR("pthread_create<btProTestFuncThread> failed(%d)\n", ret);
			UnInit();
			return -1;
		}
	}
#endif

    NET_DBG("---------Inet.cpp-----Net::init()---End!!!!!!!!------\n");
#if defined (__IS_SUPPORT_WLAN__)
	/* 针对WIFI和4G模块，在系统启动的时候，需要配置一下路由 */
    if (bSupportWIFI)
    {
        /* 所有网络参数设置完毕之后，需要更新路由 */
        bNeedUpdateRouter = TRUE;
	}
#endif

    NET_arp_update_neighbors();
	return 0;
}

int INet::UnInit()
{
	//反初始化推送
	if (m_bZWPushInit)
	{
#if defined (__USE_P2P_PUSH__)
		ZWP2P_UINT32 zwRet = ZWP2PPUSHVNClient_Deinitialize();
		if (eZWP2P_PUSH_ERR_SUCCESS != zwRet)
		{
			NET_ERR("ZWP2PPUSHVNClient_Deinitialize failed(%d)\n", zwRet);
		}
		else
		{
			m_bZWPushInit = 0;
		}
#else
		m_bZWPushInit = 0;
#endif
	}
	if (0 != m_hPushInitThread)
	{
		pthread_cancel(m_hPushInitThread);
		pthread_join(m_hPushInitThread, NULL);
		m_hPushInitThread = 0;
	}
	
	//结束注册检测线程
	m_bCheck_exit = 1;
	if (0 != m_hCheckThread)
	{
		pthread_cancel(m_hCheckThread);
		pthread_join(m_hCheckThread, NULL);
		m_hCheckThread = 0;
	}

    if (0 != m_IPAdjustCheckThread)
    {
		pthread_cancel(m_IPAdjustCheckThread);
		pthread_join(m_IPAdjustCheckThread, NULL);
		m_IPAdjustCheckThread = 0;
    }

    if (0 != m_UpnpCheckThread)
    {
		pthread_cancel(m_UpnpCheckThread);
		pthread_join(m_UpnpCheckThread, NULL);
		m_UpnpCheckThread = 0;
    }

    if (0 != m_OperateRouterThread)
    {
        pthread_cancel(m_OperateRouterThread);
        pthread_join(m_OperateRouterThread, NULL);
        m_OperateRouterThread = 0;
    }
#if defined (__IS_SUPPORT_WLAN__)
	if (0 != m_SendWifiTestCommandThread)
    {
        pthread_cancel(m_SendWifiTestCommandThread);
        pthread_join(m_SendWifiTestCommandThread, NULL);
        m_SendWifiTestCommandThread = 0;
    }
    
    if (0 != m_wifiApStaStatusCheckThread)
    {
        pthread_cancel(m_wifiApStaStatusCheckThread);
        pthread_join(m_wifiApStaStatusCheckThread, NULL);
        m_wifiApStaStatusCheckThread = 0;
    }
#endif

#ifdef __IS_SUPPORT_AIC_BLUETOOTH__
		if (0 != m_btProTestFunc)
		{
			pthread_cancel(m_btProTestFunc);
			pthread_join(m_btProTestFunc, NULL);
			m_btProTestFunc = 0;
		} 
#endif

    if (0 != m_FtpOperateThread)
    {
        pthread_cancel(m_FtpOperateThread);
        pthread_join(m_FtpOperateThread, NULL);
        m_FtpOperateThread = 0;
    }

#if defined (__USE_APPLY_DID_MANUAL__)
    if (0 != m_ApplyDID)
    {
        pthread_cancel(m_ApplyDID);
        pthread_join(m_ApplyDID, NULL);
        m_ApplyDID = 0;
    }
#endif    

#if defined (__MODULE_4G_SUPPORT__)
	if (0 != m_OperateRouterThread)
    {
        pthread_cancel(m_OperateRouterThread);
        pthread_join(m_OperateRouterThread, NULL);
        m_OperateRouterThread = 0;
    }
    
    if (0 != m_GPRS_CheckStatusThread)
    {
        pthread_cancel(m_GPRS_CheckStatusThread);
        pthread_join(m_GPRS_CheckStatusThread, NULL);
        m_GPRS_CheckStatusThread = 0;
    }

    if (0 != m_4GModule_ProcessThread)
    {
        pthread_cancel(m_4GModule_ProcessThread);
        pthread_join(m_4GModule_ProcessThread, NULL);
        m_4GModule_ProcessThread = 0;
    }

    if (0 != m_CheckCardTypeTimerThread)
    {
        pthread_cancel(m_CheckCardTypeTimerThread);
        pthread_join(m_CheckCardTypeTimerThread, NULL);
        m_CheckCardTypeTimerThread = 0;
    }
#endif

	if (NULL != m_pArpPacket)
	{
		delete m_pArpPacket;
	}
	if (NULL != m_pArpReplyBuff)
	{
		delete m_pArpReplyBuff;
	}
	m_iArpReplyBuffSize = 0;
	m_iArpPacketSize = 0;
	m_FxnCallBack_IP = NULL;
	m_FxnCallBack_PHY = NULL;
	m_bDHCP_Enable = 0;
	m_iNetStatus = 2;			//未知
	//  m_iNetStatus_last = 2;
	return 0;
}

//##ModelId=49BB426A02C6
INet::~INet()
{
	UnInit();

}
char * INet::GetDevName(int num, char *DevName)
{
	return DevName;
}

int INet::NET_GetDhcpEnableEx()
{
	NET_DBG("dhcpWillBeChanged=%d, futureDhcp=%d, m_bDHCP_Enable=%d\n", dhcpWillBeChanged, futureDhcp, m_bDHCP_Enable);
	if(dhcpWillBeChanged)
		return futureDhcp;
	else
		return m_bDHCP_Enable;
}

//##ModelId=49BB426A02CF
int INet::NET_GetDhcpEnable()
{
	return m_bDHCP_Enable;
}

int INet::NET_GetStatus()
{
	return m_iNetStatus;
}

#if defined (__IS_SUPPORT_WLAN__)
int INet::WLAN_GetWifiMode()
{
	return m_wifiMode;
}
#endif

int INet::NET_SetTutkUIDEnable(int bEnable)
{
	int ret=0;

	return ret;
}

int INet::NET_GetTutkUIDEnable()
{
	return m_TutkUID_Enable;
}
/**
 * pSetMethod: 设置DID方式  0--用户手动设置DID  1--自动设置DID（如：手动申请did）
*/
int INet::NET_SetTutkUID(TutkCfg *pTutkCfg, int pSetMethod)
{
	if (NULL == pTutkCfg)
    {
        NET_ERR("NULL==pTutkCfg\n");
        return -1;        
    }

    FILE *file_new = NULL;
	char buff[24+MAX_DID_LENGTH] = {0};
	int bNeedRebootFlag = FALSE;

#ifdef __MODULE_4G_SUPPORT__
	int bSdkSupport4G = 0;
#endif

#ifdef __MODULE_4G_SUPPORT__
    ZWSys_Get4GSupportV2(&bSdkSupport4G);
    if (bSdkSupport4G)
    {
    	for (unsigned int i = 0;i < sizeof(g_szSupportFlowOpreatePlatform) / sizeof(char *);i++)
    	{
    		if (0 == strncmp(m_sTutkUID,g_szSupportFlowOpreatePlatform[i],strlen(g_szSupportFlowOpreatePlatform[i])))
    		{
    			if (0 != strncmp(pTutkCfg->szTutkUID,g_szSupportFlowOpreatePlatform[i],strlen(g_szSupportFlowOpreatePlatform[i])))
    			{
    				//运营模式号段 ，不相同不允许设置
    				NET_ERR("UID is error [%s] [%s]\n", m_sTutkUID, pTutkCfg->szTutkUID);
    				return -1;
    			}
    		}
    	}

		if (0 == ZWSys_CheckDidIsFlowOpreate(m_sTutkUID))
        {
			if (0 != strncmp(m_sTutkUID,pTutkCfg->szTutkUID,4))
			{
				//非运营模式号段 ，不相同不允许设置
                NET_ERR("UID is error [%s] [%s]\n", m_sTutkUID, pTutkCfg->szTutkUID);
				return -1;
			}
		}
    }
#endif

	m_pCConfig->SetValue(INet_CfgSection, "p2pEnable", pTutkCfg->bEnable);

	if (strlen(pTutkCfg->szPushID) < 1)
	{
		//不支持DID
		snprintf(buff, sizeof(buff), "%s", 
			pTutkCfg->szTutkUID);
	}
	else
	{
		snprintf(buff, sizeof(buff), "%s%c%s", 
			pTutkCfg->szTutkUID, MAX_PUSH_ID_SFD, pTutkCfg->szPushID);
	}
	
	if ((0 == strcmp(buff, m_sTutkUID)) 
		&& (strlen(buff) == strlen(m_sTutkUID))
		&& (m_TutkUID_Enable == pTutkCfg->bEnable))
	{
		return 0;
	}
	
	if (0 != strcmp(buff, m_sTutkUID)
		|| strlen(buff) != strlen(m_sTutkUID))
	{
		bNeedRebootFlag = TRUE;
        char command [128] = {0};
        snprintf(command, sizeof(command), "touch %s", TUTK_UID_CHANGE_FLAG);
        mysystem(command);
        NET_ERR("change uid touch flag!\n");
	}
	
	m_TutkUID_Enable = pTutkCfg->bEnable;
		
#ifdef SOFT_ENCRYPTION
    /* 如果取消授权或者授权失败，则不让设置DID号 */
    if (!access(AUTHORIZE_LICENSE_FILE_CFG, F_OK))
#endif
	{
#if defined (__AUTH_V2__)

		memset(m_sTutkUID, 0, sizeof(m_sTutkUID));
		strncpy(m_sTutkUID, buff, sizeof(m_sTutkUID) - 1);
#else
		memset(m_sTutkUID, 0, sizeof(m_sTutkUID));
		strncpy(m_sTutkUID, buff, sizeof(m_sTutkUID) - 1);
#endif
	}

	file_new = fopen(TUTK_UID_FILE_PATH_BAK, "w");
	if (file_new == NULL)
	{
		NET_ERR("open file failed [%s]\n", TUTK_UID_FILE_PATH_BAK);
		return -1;
	}
	
	memset(buff, 0, sizeof (buff));
    snprintf(buff, sizeof(buff), "%d:%s", m_TutkUID_Enable, m_sTutkUID);
    fprintf(file_new, "%s", buff);
    fclose(file_new);
	
    snprintf(buff, sizeof(buff), "mv %s %s", TUTK_UID_FILE_PATH_BAK, TutkUID_CONF);
    mysystem(buff);

	//用户修改了DID之后，需要创建标记重新生成Subkey
    memset(buff, 0, sizeof(buff));
    snprintf(buff, sizeof(buff), "touch %s", SUBKEY_NEED_CLEAR_FLAG);
    mysystem(buff);

    memset(buff, 0, sizeof(buff));
    snprintf(buff, sizeof(buff), "touch %s", SUBKEY_NEED_GENERAL_FLAG);
    mysystem(buff); 

#if defined (__USE_APPLY_DID_MANUAL__)
    m_iApplyStatus = 0;

    stuShmAuthInfo authinfo;
    Authorize_GetAuthDevInfo(&authinfo);

	memcpy(authinfo.struCloudInfo.szDid, m_sTutkUID, MAX_UID);
    Authorize_SetAuthDevInfo(&authinfo);
#endif

#ifdef __MODULE_4G_SUPPORT__
    ZWSys_Get4GSupportV2(&bSdkSupport4G);
    if (bSdkSupport4G)
    {
    	int dwCapability = 0;	
    	
    	//如果是运营号段的设备，需要判断是否激活了运营模式，出图显示未激活
    	if (1 == ZWSys_CheckDidIsFlowOpreate(m_sTutkUID))
    	{
    		ZWSys_GetActiveStatusCode(&dwCapability);
    		if (0 == (dwCapability & CAPABILITY_EXTERNSIMCARD))
    		{
    			//出图显示未激活
    			ZWSys_SetNoActiveOperModeOSDDisplay();
    		}
    	}

        char cmd_string[128];

    	//删除解绑标识
    	memset(cmd_string,0,sizeof(cmd_string));
    	snprintf(cmd_string,sizeof(cmd_string)-1,"rm %s -rf",MODULE_4G_BIND_FLAG);
    	mysystem(cmd_string);
    }
#endif
    if (bNeedRebootFlag)
    {
        NET_WARN("**********change Tutk UID need reboot !**********\n");
        CConfig *pConfig = CConfig::GetInstance();
        if (NULL != pConfig)
        {
            pConfig->SaveFileReal();
			pConfig->ForbiddenSaveFile();
        }
        mysystem("reboot -d 10 &");
    }
    NET_INFO("my add: ----NET_SetTutkUID---return\n");

    return 0;
}

int INet::Net_LoadAuthPushParam()
{
    int ret = 0;
    char *pDomain = NULL;
    char *pDomainSuffix = NULL;
    stuConnectInfo connect_info;
	 bool rc = 0;
    memset(&connect_info, 0, sizeof(connect_info));

    ret = Authorize_GetConnectInfo(CONNECT_STRING_LINENUM, &connect_info);
    if (ret != 0)
    {
        NET_ERR("Get ConnectString failed\n");
        m_acDomainSuffixExist = 0;
        return 0;
    }

    if ((pDomain = strstr(connect_info.szConnectString, ".com")))
    {
        m_acDomainSuffixExist = 1;
		m_acDomainSuffixExist_Test = 1;
        connect_info.szConnectString[strlen(connect_info.szConnectString)] = '\0';
        pDomainSuffix = strrchr(connect_info.szConnectString, '@');
        memcpy(m_acDomainSuffix, pDomainSuffix+1, sizeof(m_acDomainSuffix));
		memcpy(m_acDomainSuffix_Test, pDomainSuffix+1, sizeof(m_acDomainSuffix_Test));
        NET_INFO("init m_acDomainSuffix:%s\n", m_acDomainSuffix);
			
	    //2023.07.03协议周工强烈要求加上高优先级能力的判断
	    rc = GetCapSupportFromDeviceCapXML_Bool("Device:FunctionList:Push:HighPriority", false);
	    if (rc == false)
	    {
	        m_acDomainSuffixExist = 0;
			memset(m_acDomainSuffix, 0, sizeof(m_acDomainSuffix));
	        NET_WARN("HighPriority rc:%d\n", rc);
	        return 0;
	    }
    }
    else
    {
        m_acDomainSuffixExist = 0;
		m_acDomainSuffixExist_Test = 0;
    }
	//2023.07.03协议周工强烈要求加上高优先级能力的判断
   rc = GetCapSupportFromDeviceCapXML_Bool("Device:FunctionList:Push:HighPriority", false);
   if (rc == false)
   {
	   m_acDomainSuffixExist = 0;
	   memset(m_acDomainSuffix, 0, sizeof(m_acDomainSuffix));
	   NET_WARN("HighPriority rc:%d\n", rc);
	   return 0;
   }

    NET_INFO("init AuthPushParam SuffixExist:%d, HighPriority:%d\n", m_acDomainSuffixExist, rc);
    return 0;
}

#if defined (__AUTH_V2__)
int INet::NET_SetConnectString(P2P_CONNECT_INFO *pConnectString, const unsigned char *pszAuthLicense)
{
	stuShmAuthInfo auth_info;
	unsigned int i = 0;
	int ret = 0;
	ZWHost_DOMAIN_INFO_LIST stZwhostDomainInfo = {0};

	//如果在设置DID时不允许设置DID，后面的连接串不需要再继续设置了
	if (0 == m_supportSetDid)
	{
		return -1;
	}

	if (pszAuthLicense != NULL)
	{
		memset(&auth_info, 0, sizeof(auth_info));
		DeEncryptLicense(pszAuthLicense, &auth_info);

		for (i = 0; i < sizeof(g_szoldPlatformName)/sizeof(g_szoldPlatformName[0]); i++)
		{
			if (!strncmp((char *)auth_info.struCloudInfo.szDid, g_szoldPlatformName[i], 4))
			{
				//在支持表表示是老的DID，不需要进行连接串的设置，直接返回0
				return 0;
			}
		}
	}

	//剩下的是新平台，若新平台没有传连接串则有问题
	if (!strncmp(pConnectString->szConnectString, "", sizeof(pConnectString->szConnectString)))//如果连接串信息为空则返回-1
	{
		return -1;
	}

    stuConnectInfo connect_info;
    memset(&connect_info, 0, sizeof(connect_info));

	memcpy(connect_info.szConnectString, pConnectString->szConnectString, strlen(pConnectString->szConnectString));
	memcpy(connect_info.szPlatformName, pConnectString->szPlatformName, strlen(pConnectString->szPlatformName));
	memcpy(connect_info.szAuthWord, pConnectString->szAuthWord, strlen(pConnectString->szAuthWord));
    Authorize_GetConnectInfo(GG_CLOUD_LINENUM, &connect_info);

    Authorize_SetConnectInfo(LINENUM_MAX, &connect_info);

	for (i = 0; i < 8; i++)
	{
		if (!strncmp(pConnectString->struDomain[i].szDomain, "", sizeof(pConnectString->struDomain[i].szDomain)))
		{
			continue;
		}

		memcpy(stZwhostDomainInfo.stDomainInfoList[i].cDomain, pConnectString->struDomain[i].szDomain, sizeof(pConnectString->struDomain[i].szDomain));
		memcpy(stZwhostDomainInfo.stDomainInfoList[i].cIPv4, pConnectString->struDomain[i].szIP, sizeof(pConnectString->struDomain[i].szIP));
		stZwhostDomainInfo.nCount++;
	}


	//如果iP2PHDR为1，则不需要调用此接口，协议那边会确保AuthP2pInfo会优先下发
    if(1 != m_AuthP2pInfo.iP2PHDR)
    {
    	NET_INFO("Hosts_UpdateDomainList iP2PHDR:%d\n",m_AuthP2pInfo.iP2PHDR);
    	//更新zwhost
    	ret = Hosts_UpdateDomainList(&stZwhostDomainInfo);
    	if (ret < 0)
    	{
    		NET_ERR("[%s:%d] Hosts_UpdateDomainList failed!\n", __FUNCTION__, __LINE__);
    	}
    }

#if defined (__USE_P2P_PUSH__)
	//设置AuthWord
	ZWP2PPUSHVNClient_SetCustomAuthWord(pConnectString->szAuthWord);
#endif

	return 0;
}
#endif

int INet::NET_SetP6SMSGServerStatics(int PushMode,int PushStatus)
{
	if ((PushMode < 0 || PushMode > 1) || (PushStatus < 0 || PushStatus > 1))
	{
		NET_ERR("param wrong !\n");
		return -1;
	}


	ZWP2P_PUSH_P6SMSG_SERVER_STATICS P6smsgCfg;
	P6smsgCfg.nPushMode = PushMode;
	P6smsgCfg.nPushStatus = PushStatus;

	ZWP2PPUSHVNClient_SetP6SMSGServerStatics(P6smsgCfg);
	return 0;
}

int INet::Net_GetNetCardInfo(NetCardInfo *NetworkInfo)
{
    if (NULL == NetworkInfo)
    {
        NET_ERR("NetworkInfo is NULL!\n");
        return -1;
    }

    NetworkInfo->nNetworkType = 0;
    memset(NetworkInfo->WIFIspeed1, 0, sizeof(NetworkInfo->WIFIspeed1));
    memset(NetworkInfo->WIFIspeed2, 0, sizeof(NetworkInfo->WIFIspeed2));
    
    if (bSupportPhy)//支持有线
    {
        NetworkInfo->nNetworkType = NetworkInfo->nNetworkType | (1 << 0);
    }

    if (bSupportWIFI)//支持无线wifi
    {
        NetworkInfo->nNetworkType = NetworkInfo->nNetworkType | (1 << 1);
        switch(m_eCurrUSBWifiDev)
        {
            case WIFI_MODULE_ATBM_6132U://5G双频 带蓝牙
            case WIFI_MODULE_AIC_8800DL:
            {
               NetworkInfo->nBluetoothIsExist = 1;
               NetworkInfo->nSparklinkIsExist = 0;
               NetworkInfo->nWIFIband = 1;
               strncpy(NetworkInfo->WIFIspeed1, "2.4G", sizeof(NetworkInfo->WIFIspeed1)-1);
               strncpy(NetworkInfo->WIFIspeed2, "5G", sizeof(NetworkInfo->WIFIspeed1)-1);
            }
            break;
            case WIFI_MODULE_HI_H383U://2.4G单频   带蓝牙 带星闪
            case WIFI_MODULE_AIC_8800DL_2_4G:
            {
              NetworkInfo->nBluetoothIsExist = 1;
              NetworkInfo->nSparklinkIsExist = 0;
              NetworkInfo->nWIFIband = 0;
              strncpy(NetworkInfo->WIFIspeed1, "2.4G", sizeof(NetworkInfo->WIFIspeed1)-1);
            }
            break;
            default:
            {
                NetworkInfo->nBluetoothIsExist = 0;
                NetworkInfo->nSparklinkIsExist = 0;
                NetworkInfo->nWIFIband = 0;
                strncpy(NetworkInfo->WIFIspeed1,  "2.4G",sizeof(NetworkInfo->WIFIspeed1)-1);
                NET_ERR("Get wifi module info error, type:%d\n", __FUNCTION__, __LINE__, m_eCurrUSBWifiDev);
            }
            break;
        }

    }
    else
    {
        //不支持wifi模块时全部重置
        NetworkInfo->nBluetoothIsExist = 0;
        NetworkInfo->nSparklinkIsExist = 0;
        NetworkInfo->nWIFIband = 0;
    }

    if (bSupportGPRS)//支持无线4G
    {
        NetworkInfo->nNetworkType = NetworkInfo->nNetworkType | (1 << 2);
    }


    return 0;
}

char *INet::NET_GetTutkUID(TutkCfg *pTutkCfg, unsigned long *pTutkStatus)
{
	if (NULL == pTutkCfg)
    {
        NET_ERR("NULL == pTutkCfg\n");
        return NULL;        
    }

	char *pPushIdStartPos = NULL;

	pTutkCfg->bEnable = 0;
#if 0
	char buff[24+MAX_DID_LENGTH] = {0};
	FILE *file_new = fopen(TutkUID_CONF, "r");
	if (file_new == NULL)
	{
		NET_ERR("open file failed [%s]\n", TutkUID_CONF);
		return NULL;
	}
	fscanf(file_new, "%d:%s", pTutkCfg->bEnable, buff);
	fclose(file_new);

	memset(pTutkCfg->szTutkUID, 0, sizeof(pTutkCfg->szTutkUID));
	memset(pTutkCfg->szPushID, 0, sizeof(pTutkCfg->szPushID));
	pPushIdStartPos = strchr(buff, MAX_PUSH_ID_SFD);
	if (NULL != pPushIdStartPos)
	{
		memcpy(pTutkCfg->szTutkUID, buff, (int)(pPushIdStartPos-buff));
		strncpy(pTutkCfg->szPushID, pPushIdStartPos+1, sizeof(pTutkCfg->szPushID) - 1);
	}
	else
	{
		//不支持推送号
		strncpy(pTutkCfg->szTutkUID, buff, sizeof(pTutkCfg->szTutkUID) - 1);
	}
#else
	if (m_pCConfig->CheckKeyExist(INet_CfgSection, "p2pEnable"))
	{
		m_TutkUID_Enable = m_pCConfig->GetValue(INet_CfgSection, "p2pEnable", 0);
	}

	pTutkCfg->bEnable = m_TutkUID_Enable;
    
#if defined (__USE_APPLY_DID_MANUAL__)
    pTutkCfg->iStatus = m_iApplyStatus;
    pTutkCfg->iErrCode = m_iApplyErrCode;
#endif

	memset(pTutkCfg->szTutkUID, 0, sizeof(pTutkCfg->szTutkUID));
	memset(pTutkCfg->szPushID, 0, sizeof(pTutkCfg->szPushID));

#ifdef SOFT_ENCRYPTION
    /* 如果取消授权或者授权失败，则清空DID号 */
    if (access(AUTHORIZE_LICENSE_FILE_CFG, F_OK))
    {
        return 0;
    }

    /* 设置授权之后，从标记获取DID号 */
    if (0 == access(AUTH_DID_NUM_FLAG, F_OK))
    {
        char did_buff[MAX_UID] = {0};
        FILE *fp = fopen(AUTH_DID_NUM_FLAG, "r");
        if (fp)
        {
    		fread(did_buff, sizeof(char), sizeof(did_buff), fp);
            fclose(fp);
            fp = NULL;
            if (strlen(did_buff))
            {
                m_TutkUID_Enable = 1;
                memset(m_sTutkUID, 0, sizeof(m_sTutkUID));
                strncpy(m_sTutkUID, did_buff, sizeof(m_sTutkUID) - 1);
            }
        }        
    }
#endif
	pPushIdStartPos = strchr(m_sTutkUID, MAX_PUSH_ID_SFD);
	if (NULL != pPushIdStartPos)
	{
		memcpy(pTutkCfg->szTutkUID, m_sTutkUID, (int)(pPushIdStartPos-m_sTutkUID));
		strncpy(pTutkCfg->szPushID, pPushIdStartPos+1, sizeof(pTutkCfg->szPushID) - 1);
	}
	else
	{
		//不支持推送号
		strncpy(pTutkCfg->szTutkUID, m_sTutkUID, sizeof(pTutkCfg->szTutkUID) - 1);
	}
#endif

	if (NULL != pTutkStatus)
	{
		*pTutkStatus = m_iTutkStatus;
	}

	return pTutkCfg->szTutkUID;
}

#if defined (__AUTH_V2__)
int INet::NET_GetConnectString(P2P_CONNECT_INFO *pConnectString)
{
    stuConnectInfo connect_info;
    memset(&connect_info, 0, sizeof(stuConnectInfo));

    if (Authorize_GetConnectInfo(CONNECT_STRING_LINENUM, &connect_info))
    {
        NET_ERR("Get ConnectString failed\n");
        return -1;
    }
	NET_INFO("szConnectString: %s\n",connect_info.szConnectString);

	unsigned int copy_length = 0;
	char *start_ops = NULL;
	char *at_pos = NULL;

	start_ops = connect_info.szConnectString;
	at_pos = strchr(connect_info.szConnectString, '@');
	
	if (at_pos != NULL) 
	{
		copy_length = at_pos - start_ops;	
		if (copy_length < sizeof(pConnectString->szConnectString)) 
		{			
            memcpy(pConnectString->szConnectString, connect_info.szConnectString, copy_length);
        }		
	}
	else
	{
	    memcpy(pConnectString->szConnectString, connect_info.szConnectString, sizeof(pConnectString->szConnectString));
	}
	
	NET_INFO("szConnectString: %s\n",pConnectString->szConnectString);

    if (Authorize_GetConnectInfo(PLATFORM_LINENUM, &connect_info))
    {
        NET_ERR("Get PlatformName failed\n");
        return -1;
    }
    memcpy(pConnectString->szPlatformName, connect_info.szPlatformName, sizeof(pConnectString->szPlatformName));

    if (Authorize_GetConnectInfo(PUSH_KEY_LINENUM, &connect_info))
    {
        NET_ERR("Get AuthWord failed\n");
        return -1;
    }
    memcpy(pConnectString->szAuthWord, connect_info.szAuthWord, sizeof(pConnectString->szAuthWord));

    return 0;
}

#endif

int INet::Net_SetAuthDomainSuffixExist(int isExist)
{
	if (m_acDomainSuffixExist != isExist)
	{
		m_acDomainSuffixExist = isExist;
	}
	return 0;
}

int INet::Net_GetAuthDomainSuffixExist(int *isExist)
{
	*isExist = m_acDomainSuffixExist;
	return 0;
}

int INet::Net_GetTutkAndDomainSuffix(char *Tutk, int TutkLen, char *Domainsuffix, int DomainLen)
{
    if (Tutk == NULL || Domainsuffix == NULL || TutkLen == 0 || DomainLen == 0)
    {
        return -1;
    }

    strncpy(Tutk, m_sTutkUID, TutkLen);
    strncpy(Domainsuffix, m_acDomainSuffix, DomainLen);
    return 0;
}

int INet::Net_GetAuthDomainSuffixExist_Test(int *isExist)
{
	*isExist = m_acDomainSuffixExist_Test;
	return 0;
}

int INet::Net_GetTutkAndDomainSuffix_Test(char *Tutk, int TutkLen, char *Domainsuffix, int DomainLen)
{
    if (Tutk == NULL || Domainsuffix == NULL || TutkLen == 0 || DomainLen == 0)
    {
        return -1;
    }

    strncpy(Tutk, m_sTutkUID, TutkLen);
    strncpy(Domainsuffix, m_acDomainSuffix_Test, DomainLen);
    return 0;
}


//在调用到Net_ClearAuthDomainSuffix之前已经调用ZWSys_RemoveAuthLicense擦除License 3
int INet::Net_ClearAuthDomainSuffix()
{
	NET_WARN("Clear AuthDomainSuffix\n");
	m_acDomainSuffixExist = 0;
	ZWP2PPUSHVNClient_CleanConfigFile();
	return 0;
}

int INet::NET_SetAuthInfoPushDomainSuffix(P2P_TRANSPARENT_INFO_PUSHDOMAIN *pPushDomainParam)
{
	memset(m_acDomainSuffix, 0, sizeof(m_acDomainSuffix));
	if (strlen(pPushDomainParam->acDomainSuffix) > 0)
	{
		memcpy(m_acDomainSuffix, pPushDomainParam->acDomainSuffix, sizeof(pPushDomainParam->acDomainSuffix));
		NET_INFO("[SET] m_acDomainSuffix - %s \n",m_acDomainSuffix);

		/*将事件高优先级域名写入host文件*/
		int ret = 0;
		ZWHost_DOMAIN_INFO_LIST stZwhostDomainInfo = {0};
		ZWP2P_PUSH_DOMAIN_PARAMS stZWPushDomainParams = {0};

		stuShmAuthInfo auth_info;
        Authorize_GetAuthDevInfo(&auth_info);

		//更新zwhost
		ret = Hosts_UpdateDomainList(&stZwhostDomainInfo);
		if (strlen(m_acDomainSuffix) > 0)
		{
			strncpy(stZWPushDomainParams.cDID, (char *)auth_info.struCloudInfo.szDid, sizeof(stZWPushDomainParams.cDID));
			strncpy(stZWPushDomainParams.cDomainSuffix, m_acDomainSuffix, sizeof(stZWPushDomainParams.cDomainSuffix));
			ret = ZWP2PPUSHVNClient_SetDomainParam(&stZWPushDomainParams);
			if (ret != 0)
			{
				NET_ERR("DomainSuffix:%s call ZWP2PPUSHVNClient_SetDomainParam failed\n", m_acDomainSuffix);
			}
			m_acDomainSuffixExist = 1;		
		}
		else
		{
			m_acDomainSuffixExist = 0;
		}

		/*域名后缀保存在授权文件中防止丢失*/
		char szConnectString[256] = {0};
        stuConnectInfo connect_info;

        memset(&connect_info, 0, sizeof(connect_info));
        if (Authorize_GetConnectInfo(CONNECT_STRING_LINENUM, &connect_info))
        {
            return -1;
        }

		snprintf(szConnectString, sizeof(szConnectString), "%s@%s", connect_info.szConnectString, m_acDomainSuffix);

        memset(&connect_info, 0, sizeof(connect_info));
		memcpy(connect_info.szConnectString, szConnectString, sizeof(connect_info.szConnectString));
        Authorize_SetConnectInfo(CONNECT_STRING_LINENUM, &connect_info);
	}
	return 0;
}

int INet::NET_GetAuthInfoPushDomainSuffix(P2P_TRANSPARENT_INFO_PUSHDOMAIN *pPushDomainParam)
{
	if (pPushDomainParam == NULL)
	{
		NET_ERR("NULL==pPushDomainParam\n");
		return -1;
	}

	memset(pPushDomainParam->acDomainSuffix, 0, sizeof(pPushDomainParam->acDomainSuffix));
	if (strlen(m_acDomainSuffix) > 0)
	{
		memcpy(pPushDomainParam->acDomainSuffix, m_acDomainSuffix, sizeof(m_acDomainSuffix));
		NET_INFO("[GET] m_acDomainSuffix - %s \n",m_acDomainSuffix);
		
	}

	return 0;
}

#if defined (__IS_SUPPORT_4G_CLOUD_DOMAIN__)
int INet::NET_AddDomainForGetAlias(char *pDomain)
{
    if (!pDomain)
    {
        NET_ERR("NULL pointer !\n");
        return -1;
    }

    if (0 == strlen(pDomain))
    {
        NET_ERR("invalid Domain !\n");
        return -1;
    }

    NET_INFO("set pDomain: %s \n", pDomain);
    Hosts_AddDomainForGetAlias(pDomain);

    return 0;
}

int INet::NET_SetAddDomainList2Local(const NET_DOMAIN_INFO_LIST DomainList)
{
    if (DomainList.nCount <= 0)
    {
        NET_ERR("invalid param \n");
        return -1;
    }

    ZWHost_DOMAIN_INFO_LIST list;
    memset(&list, 0, sizeof(list));
    list.nCount = DomainList.nCount;
    for (int i = 0; i < list.nCount; ++i)
    {
        memcpy(&(list.stDomainInfoList[i].cDomain[0]), &(DomainList.stDomainInfoList[i].cDomain[0]), sizeof(list.stDomainInfoList[i].cDomain));
        memcpy(&(list.stDomainInfoList[i].cIPv4[0]), &(DomainList.stDomainInfoList[i].cIPv4[0]), sizeof(list.stDomainInfoList[i].cIPv4));
        NET_INFO("list.stDomainInfoList[%d].cDomain: %s ipv4: %s \n", i, list.stDomainInfoList[i].cDomain, list.stDomainInfoList[i].cIPv4);   
    }
    
    Hosts_UpdateDomainList(&list);

    return 0;
}

int INet::NET_SetDomainListManagerServer(TransParent_INFO *pTransParentInfo)
{
    if (NULL == pTransParentInfo)
    {
        NET_ERR("invalid param \n");
        return -1;
    }
    
    if (strlen(pTransParentInfo->cDomainServer) > 0)
    {
        TutkCfg TutkDid;
        memset(&TutkDid, 0, sizeof(TutkDid));
        ZWHost_DomainListManagerServerParam DomainListManagerServerParam;
        memset(&DomainListManagerServerParam, 0, sizeof(ZWHost_DomainListManagerServerParam));
    
        DomainListManagerServerParam.port = pTransParentInfo->port;
        memcpy(&DomainListManagerServerParam.cDomainServer, &pTransParentInfo->cDomainServer, sizeof(DomainListManagerServerParam.cDomainServer));
    
        NET_4G_GetTutkUID(&TutkDid);
    
        if (0 == strlen(&TutkDid.szTutkUID[0]))
        {
            //无DID
            NET_ERR("empty did \n");
            return -1;
        }
        else
        {
            memcpy(&DomainListManagerServerParam.cDID[0], &TutkDid.szTutkUID[0], sizeof(DomainListManagerServerParam.cDID));
        }    
        NET_INFO("did: %s cDomainServer: %s port: %d \n",DomainListManagerServerParam.cDID, DomainListManagerServerParam.cDomainServer, DomainListManagerServerParam.port); 

        int ret = Hosts_SetDomainListManagerServer(&DomainListManagerServerParam);
    
        /*域名后缀保存在授权文件中防止丢失*/
        stuConnectInfo connect_info;
        memset(&connect_info, 0, sizeof(connect_info));

        snprintf(connect_info.sz4GCloud, sizeof(connect_info.sz4GCloud), "%s#%d", DomainListManagerServerParam.cDomainServer, DomainListManagerServerParam.port);
        Authorize_SetConnectInfo(GG_CLOUD_LINENUM, &connect_info);

        NET_INFO("the NET_SetDomainListManagerServer ret = %d\n",ret);
    }
    return 0;
}
#endif

int INet::NET_GetTutkStatus(int *pTutkStatus)
{
	*pTutkStatus = m_iTutkStatus;

	return 0;
}

int INet::NET_SetTutkStatus(int *pTutkStatus)
{

	m_iTutkStatus = *pTutkStatus;

#ifdef __MODULE_4G_SUPPORT__
    if (bSupportGPRS)
    {
    	//设置p2p状态
    	Module4G_SetTutkStatus(m_iTutkStatus);
    }
#endif	

	return 0;
}

int INet::NET_GetP2PDownloadURL(char *pDownloadURL)
{
	if(NULL == pDownloadURL)
	{
		NET_ERR("NULL==pDownloadURL\n");
		return -1;		
	}

	memcpy(pDownloadURL, m_P2PDownloadURL, sizeof(m_P2PDownloadURL));
	return 0;
}

/*-------------------- ULK相关配置 --------------------*/
int INet::NET_SetUlkUIDEnable(int bEnable)
{
	int ret=0;
	return ret;
}

int INet::NET_GetUlkUIDEnable()
{
	int ret=0;
	return ret;
}

int INet::NET_SetUlkUID(char bEnable, const char *pUlkUID)
{
	int result=0;
    
    if (bSupportULK)
    {
        FILE *file_new;
        char buff[128];

        if(NULL == pUlkUID)
        {
        	NET_ERR("NULL==pUlkUID\n");
        	return -1;		
        }

        if((0 == strcmp(pUlkUID, m_sUlkUID)) && (m_UlkUID_Enable == bEnable))
        	return 0;

        file_new = fopen(ULKUID_CONF_BAK, "w");

        if (file_new == NULL)
        {
        	NET_ERR("open file failed [%s]\n", ULKUID_CONF_BAK);
        	return -1;
        }

        memset(buff, 0, sizeof (buff));
        snprintf(buff, sizeof(buff), "%d:%s", bEnable, pUlkUID);
        fwrite(buff, strlen(buff), 1, file_new);

        fflush(file_new);
        fclose(file_new);

        NET_DBG("===ULK buff: %s===\n", buff);

        memset(buff, 0, sizeof(buff));
        snprintf(buff, sizeof(buff), "mv %s %s", ULKUID_CONF_BAK, ULKUID_CONF);
        result = mysystem(buff);
        if (result)
        {
            NET_ERR("mysystem <%s> failed(%d)\n", buff, result);
            return -1;
        }

        NET_DBG("Set UUID is %s, ret = %d\n", pUlkUID, result);

        m_UlkUID_Enable = bEnable;
        memcpy(m_sUlkUID, pUlkUID, sizeof(m_sUlkUID));
	}

    return result;
}

int INet::NET_SetUUID(char *pUlkUID)
{
    int result=0;

    if (bSupportULK)
    {
        FILE *file_new;
        char buff[128];
        
        if(NULL == pUlkUID)
        {
            NET_ERR("NULL==pUlkUID\n");
            return -1;        
        }

        if(0 == strcmp(pUlkUID, m_sUlkUID))
            return 0;

        file_new = fopen(ULKUID_CONF_BAK, "w");

        if (file_new == NULL)
        {
            NET_ERR("open file failed [%s]\n", ULKUID_CONF_BAK);
            return -1;
        }
        
        memset(buff, 0, sizeof (buff));
    	snprintf(buff, sizeof(buff), "%d:%s", m_UlkUID_Enable, pUlkUID);
        fwrite(buff, strlen(buff), 1, file_new);
        
        fflush(file_new);
        fclose(file_new);

        NET_INFO("===ULK UID: %s===\n", pUlkUID);

        memset(buff, 0, sizeof(buff));
        snprintf(buff, sizeof(buff), "mv %s %s", ULKUID_CONF_BAK, ULKUID_CONF);
        result = mysystem(buff);
        if (result)
        {
            NET_ERR("mysystem <%s> failed(%d)\n", buff, result);
            return -1;
        }

        NET_INFO("Set UUID is %s, ret = %d\n", pUlkUID, result);

        memcpy(m_sUlkUID, pUlkUID, sizeof(m_sUlkUID));
    }

    return result;
}

char *INet::NET_GetUlkUID(char *bEnable, char *pUlkUID, int *UlkStatus)
{
    if (bSupportULK)
    {
    	if((NULL == pUlkUID) || (NULL == bEnable) || (NULL == UlkStatus))
    	{
    		NET_ERR("NULL==pUlkUID || NULL == bEnable || NULL == UlkStatus\n");
    		return NULL;		
    	}

    	*bEnable = m_UlkUID_Enable;
		snprintf(pUlkUID, strlen(m_sUlkUID)+1, "%s", m_sUlkUID);
    	//memcpy(pUlkUID, m_sUlkUID, strlen(m_sUlkUID));
    	*UlkStatus = m_iUlkStatus;
	}

	return pUlkUID;
}

int INet::NET_SetUlkStatus(int *pUlkStatus)
{
    if (bSupportULK)
    {
	    m_iUlkStatus = *pUlkStatus;
	}

	return 0;
}

/* 检测当前网络是否可以连接互联网(conn_status=1表示可以联通互联网) */
int INet::Net_CheckIfNetAccessToInternet(int *conn_status)
{
    if (!conn_status)
    {
        NET_ERR("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    char net_card[16] = {0};

    /* 此处加锁是为了在操作路由之后，再进行ping测试外网 */
    XTRACE_MUTEX_LOCK(&m_OperateRouteLock);
    if (3 == m_nCurrNetCardType)
    {
        strncpy(net_card, IWNAME, sizeof(net_card) - 1);
    }
    else
    {
        strncpy(net_card, IFNAME, sizeof(net_card) - 1);
    }
    XTRACE_MUTEX_UNLOCK(&m_OperateRouteLock);
    if (eP2PStauts_InternetReady == m_iTutkStatus)    //P2P就绪
    {
        *conn_status = 1;
    }
	else if (m_YLDid && (1 == ping(net_card, GOOGLE_DOMAIN_YL_1) || 1 == ping(net_card, GOOGLE_DOMAIN_YL_2)))
	{
		*conn_status = 1;
	}
    else if (1 == ping(net_card, GOOGLE_DOMAIN))
    {
		*conn_status = 1;
    }
	else
	{
		*conn_status = 0;
	}
	
    return 0;
}

/*---------------------------------------------------------------*/

int INet::NET_SetIPCallBack(int AlarmType, int (*CallBack) (struct tagAlertInfo * pAlertInfo, struct tagActionInfo * pActionInfo))
{
	m_IPAlarmType = AlarmType;
	m_FxnCallBack_IP = CallBack;
	return 0;
}

int INet::NET_SetPHYCallBack(int AlarmType, int (*CallBack) (struct tagAlertInfo * pAlertInfo, struct tagActionInfo * pActionInfo))
{
	m_PHYAlarmType = AlarmType;
	m_FxnCallBack_PHY = CallBack;
	return 0;
}

//!\brief 设置联动动作
//!\param nType 类型
//  !      ACTION_BEEP 声音提示
//  !      ACTION_SENDTOSERVER 上传报警服务器
//  !      ACTION_EMAIL 联动Email
//  !      ACTION_SMS 联动短信   
//  !      ACTION_ALARMOUT 报警输出
//!\param int param 如果是报警输出则为端口号掩位,其他情况不关心
int INet::NET_SetIPAction(int nType, int bEnable)
{
	NET_DBG("\n...INet.cpp...INet::NET_SetIPAction...设置联动动作...nType=0x%x..bEnable=%d.\n",nType,bEnable);
	switch (nType)
	{
		case ACTION_BEEP:
			{
				m_bBeep[0] = (bEnable != 0);
				m_pCConfig->SetValue(INet_CfgSection, "Net_Action_ip_beep", m_bBeep[0]);
				break;
			}
		case ACTION_SENDTOSERVER:
			{
				m_bSTS[0] = (bEnable != 0);
				m_pCConfig->SetValue(INet_CfgSection, "Net_Action_ip_sts", m_bSTS[0]);
				break;
			}
		case ACTION_EMAIL:
			{
				m_bEmail[0] = (bEnable != 0);
				m_pCConfig->SetValue(INet_CfgSection, "Net_Action_ip_email", m_bEmail[0]);
				break;
			}
		case ACTION_SMS:
			{
				m_bSMS[0] = (bEnable != 0);
				m_pCConfig->SetValue(INet_CfgSection, "Net_Action_ip_sms", m_bSMS[0]);
				break;
			}
		case ACTION_ALARMOUT:
			{
				m_bDOP[0] = (bEnable & 0xFF);	//支持8个
				m_pCConfig->SetValue(INet_CfgSection, "Net_Action_ip_dop", m_bDOP[0]);
				break;
			}
		case ACTION_GUI:
			{
				m_bGUI[0] = (bEnable != 0);
				m_pCConfig->SetValue(INet_CfgSection, "Net_Action_ip_gui", m_bGUI[0]);
				break;
			}
		case ACTION_PUSH:
			{
				m_bPush[0] = (bEnable != 0);
				m_pCConfig->SetValue(INet_CfgSection, "Net_Action_ip_push", m_bPush[0]);
				break;
			}
		case ACTION_LIGHTALARM:
			{
				m_bLightAlarm[0] = (bEnable != 0);
				m_pCConfig->SetValue(INet_CfgSection, "Net_Action_ip_m_lightalarm", m_bLightAlarm[0]);
				break;
			}
		default:
			{
				NET_INFO("unsupported nType = %x\n", nType);
				return -1;
			}
	}
	return 0;
}

int INet::NET_SetPHYAction(int nType, int bEnable)
{
	switch (nType)
	{
		case ACTION_BEEP:
			{
				m_bBeep[1] = (bEnable != 0);
				m_pCConfig->SetValue(INet_CfgSection, "Net_Action_phy_beep", m_bBeep[1]);
				break;
			}
		case ACTION_SENDTOSERVER:
			{
				m_bSTS[1] = (bEnable != 0);
				m_pCConfig->SetValue(INet_CfgSection, "Net_Action_phy_sts", m_bSTS[1]);
				break;
			}
		case ACTION_EMAIL:
			{
				m_bEmail[1] = (bEnable != 0);
				m_pCConfig->SetValue(INet_CfgSection, "Net_Action_phy_email", m_bEmail[1]);
				break;
			}
		case ACTION_SMS:
			{
				m_bSMS[1] = (bEnable != 0);
				m_pCConfig->SetValue(INet_CfgSection, "Net_Action_phy_sms", m_bSMS[1]);
				break;
			}
		case ACTION_ALARMOUT:
			{
				m_bDOP[1] = (bEnable & 0xFF);	//支持8个
				m_pCConfig->SetValue(INet_CfgSection, "Net_Action_phy_dop", m_bDOP[1]);
				break;
			}
		case ACTION_GUI:
			{
				m_bGUI[1] = (bEnable != 0);
				m_pCConfig->SetValue(INet_CfgSection, "Net_Action_phy_gui", m_bGUI[1]);
				break;
			}
		case ACTION_RECORD:
			{
				m_bRecord[1] = (bEnable & 0xFFFF);	//支持16个
				m_pCConfig->SetValue(INet_CfgSection, "Net_Action_phy_record", m_bRecord[1]);
				break;
			}
		case ACTION_PUSH:
			{
				m_bPush[1] = (bEnable != 0);
				m_pCConfig->SetValue(INet_CfgSection, "Net_Action_phy_push", m_bPush[1]);
				break;
			}
		case ACTION_LIGHTALARM:
			{
				m_bLightAlarm[1] = (bEnable != 0);
				m_pCConfig->SetValue(INet_CfgSection, "Net_Action_ip_m_lightalarm", m_bLightAlarm[1]);
				break;
			}
		default:
			{
				NET_INFO("unsupported nType = %x\n", nType);
				return -1;
			}
	}
	return 0;
}

//!\brief 获取联动动作
//!\param nType 类型
//  !      ACTION_BEEP 声音提示
//  !      ACTION_SENDTOSERVER 上传报警服务器
//  !      ACTION_EMAIL 联动Email
//  !      ACTION_SMS 联动短信   
//  !      ACTION_ALARMOUT 报警输出
//!\return 返回是否设置，如果是报警输出则返回端口号掩位
int INet::NET_GetIPAction(int nType)
{
NET_DBG("\n...INet.cpp.......INet::NET_GetIPAction......获取联动动作.....\n");
	int result = 0;

	switch (nType)
	{
		case ACTION_BEEP:
			{
				result = m_bBeep[0];
				break;
			}
		case ACTION_SENDTOSERVER:
			{
				result = m_bSTS[0];
				break;
			}
		case ACTION_EMAIL:
			{
				result = m_bEmail[0];
				break;
			}
		case ACTION_SMS:
			{
				result = m_bSMS[0];
				break;
			}
		case ACTION_ALARMOUT:
			{
				result = m_bDOP[0];	//支持8个
				break;
			}
		case ACTION_GUI:
			{
				result = m_bGUI[0];
				break;
			}
		case ACTION_PUSH:
			{
				result = m_bPush[0];
				break;
			}
		case ACTION_LIGHTALARM:
			{
				result = m_bLightAlarm[0];
				break;
			}
		default:
			{
				NET_INFO("unsupported nType = %x\n", nType);
				return 0;
			}
	}
	return result;
}

int INet::NET_GetPHYAction(int nType)
{
	int result = 0;

	switch (nType)
	{
		case ACTION_BEEP:
			{
				result = m_bBeep[1];
				break;
			}
		case ACTION_SENDTOSERVER:
			{
				result = m_bSTS[1];
				break;
			}
		case ACTION_EMAIL:
			{
				result = m_bEmail[1];
				break;
			}
		case ACTION_SMS:
			{
				result = m_bSMS[1];
				break;
			}
		case ACTION_ALARMOUT:
			{
				result = m_bDOP[1];	//支持8个
				break;
			}
		case ACTION_GUI:
			{
				result = m_bGUI[1];
				break;
			}
		case ACTION_RECORD:
			{
				result = m_bRecord[1];
				break;
			}
		case ACTION_PUSH:
			{
				result = m_bPush[1];
				break;
			}
		case ACTION_LIGHTALARM:
			{
				result = m_bLightAlarm[1];
				break;
			}
		default:
			{
				NET_INFO("unsupported nType = %x\n", nType);
				return 0;
			}
	}
	return result;
}

int INet::NET_SetDhcpEnableEx(int bEnable, int *dhcpChanged)
{
	int ret = 0;

	if (NULL != dhcpChanged)
		*dhcpChanged = 0;

	if (m_bDHCP_Enable != bEnable)
	{
		//把将要修改的ip地址存起来
		futureDhcp = bEnable;
		//标记有将要修改的dhcp，上层获取时判断用
		dhcpWillBeChanged = 1;
		if(NULL != dhcpChanged)
			*dhcpChanged = 1;
	}
	NET_DBG("bEnable=%d, m_bDHCP_Enable=%d\n", bEnable, m_bDHCP_Enable);

	return ret;
}

//防止DHCP在反复的申请改变ip
int INet::DhcpIPFixed(const char *pNetCard,char *command, int size)
{
	int ret = 0;
	char IP[20]={0};
	char buf[128]={0};

	if(NULL == pNetCard || NULL == command) 
	{
		NET_ERR("pNetCard or command Pointer is NULL\n");
		return -1;
	}
	if((0 != strcmp(IFNAME, pNetCard)) && (0 != strcmp(IWNAME, pNetCard)))
	{
		NET_ERR("Not support net card %s operation for dhcp\n", pNetCard);
		return -1;
	}

	if(0 == strcmp(IFNAME, pNetCard))
	{
		strncpy(buf, "ls /mnt/mtd/config/.Ip* | awk -F \":\" '{printf $2}'", sizeof(buf) - 1);
	}
	else if(0 == strcmp(IWNAME, pNetCard))
	{
		strncpy(buf, "ls /mnt/mtd/config/.WlanIp* | awk -F \":\" '{printf $2}'", sizeof(buf) - 1);
	}
	
	ShellCmdOut(buf, IP, sizeof(IP) - 1);
	
	if (0 == checkIPAddr(IP))
	{
		snprintf(command, size, "udhcpc -a -b -i %s -x hostname:IPCAM -R -r %s", pNetCard,IP);		
	}
	else
	{
		snprintf(command, size, "udhcpc -a -b -i %s -x hostname:IPCAM -R", pNetCard);
	}

    return ret;
}

//DHCP的开启与关闭操作
int INet::enableDhcpOperation(int enable, const char *pNetCard)
{
	char buf[128] = {0};
	char command[128] = {0};
	int ret = 0;
	int pid = 0;

    if ((0 == strcmp(IFNAME, pNetCard)) && (0 == bSupportPhy))
	{
		NET_INFO("Not support eth0\n");
		return -1;
	}

	if((0 != strcmp(IFNAME, pNetCard)) && (0 != strcmp(IWNAME, pNetCard)))
	{
		NET_ERR("Not support net card %s operation for dhcp\n", pNetCard);
		return -1;
	}

	XTRACE_MUTEX_LOCK(&m_OperateDhcpLock);
	if(0 == enable)
	{
		snprintf(command, sizeof(command), "ps | grep \"udhcpc -a -b -i %s -x hostname IPCAM -R\" | grep -v grep | awk -F \" \" '{print $1}'", pNetCard);

		ret = ShellCmdOut(command, buf, sizeof(buf));//PID最大为5位
		if(ret)
		{
			NET_ERR("Not found udhcpc -a -b -i eth0 -x hostname IPCAM\n");
		}
		else
		{
			pid = atoi(buf);
			if((pid > 100) && (pid <= 0x8000))//100是经验值，0x8000是linux最大pid号
			{
				memset(buf, 0, sizeof(buf));
				snprintf(buf, sizeof(buf), "kill %d", pid);
				mysystem(buf);
			}
			else
				NET_ERR("found udhcpc -a -b -i eth0 -x hostname IPCAM PID number is invalid\n");
		}
			NET_DBG("##############kill %s Dhcp, pid=%d\n", pNetCard, pid);
	}
	else
	{
		if (0 == strcmp(IFNAME, pNetCard))
		{
			if (0 == access(DHCP_GW_ADDR, F_OK))
			{
				remove(DHCP_GW_ADDR);
			}
		}
		else if (0 == strcmp(IWNAME, pNetCard))
		{
			if (0 == access(WLAN_GW_ADDR, F_OK))
			{
				remove(WLAN_GW_ADDR);
			}
		}

		snprintf(command, sizeof(command), "ps | grep \"udhcpc -a -b -i %s -x hostname IPCAM -R\" | grep -v grep | awk -F \" \" '{print $1}'", pNetCard);

		ret = ShellCmdOut(command, buf, sizeof(buf));//PID最大为5位
		if (ret)
        {
            NET_ERR("Not found udhcpc -a -b -i eth0 -x hostname IPCAM \n");
        }
		else
		{
			pid = atoi(buf);
			if((pid > 100) && (pid <= 0x8000))//100是经验值，0x8000是linux最大pid号
			{
				memset(buf, 0, sizeof(buf));
				snprintf(buf, sizeof(buf), "kill %d", pid);
				mysystem(buf);
			}
			else
				NET_ERR("found udhcpc -a -b -i eth0 -x hostname IPCAM PID number is invalid\n");
		}
		NET_DBG("##############kill %s Dhcp, pid=%d\n", pNetCard, pid);

		memset(command, 0, sizeof(command));
#if 1           
		DhcpIPFixed(pNetCard, command, sizeof(command));
		NET_INFO("[NetCard-%s][command-%s]\n",pNetCard,command);
		if (IsStringInCommand("ps", "udhcpc"))
		{
			mysystem("killall -9 udhcpc");
		}
#else
        snprintf(command, sizeof(command), "udhcpc -a -b -i %s -x hostname:IPCAM -R", pNetCard);
#endif
        mysystem(command);
		NET_DBG("##############Start %s Dhcp\n", pNetCard);
	}
	XTRACE_MUTEX_UNLOCK(&m_OperateDhcpLock);

	return 0;
}

//##ModelId=49BB426A02D0
int INet::NET_SetDhcpEnable(int bEnable)
{
	NET_DBG("---NET_SetDhcpEnable---bEnable=%d\n", bEnable);

	if (bEnable)
	{
		//之前为静态，则开启
		if (m_bDHCP_Enable == 0)
		{
			m_bDHCP_Enable = 1;
			NET_ReActive();
		}
	}
	else
	{
		if (m_bDHCP_Enable != 0)
		{
			/*关闭有线的DHCP*/
			enableDhcpOperation(0, IFNAME);
			
			m_bDHCP_Enable = 0;
			//重激活eth0
			NET_ReActive();
		}
	}

	//写配置
	if (m_pCConfig->SetValue(INet_CfgSection, INet_DHCP_key, m_bDHCP_Enable) < 0)
	{
		NET_ERR("SetValue failed\n");
		return -1;
	}
	
	saveDhcpBak(m_bDHCP_Enable);	
	dhcpWillBeChanged = 0;
	
	return 0;
}

int INet::NET_ReActive()
{
	if (0 == bSupportPhy)
	{
		NET_INFO(" not support eth0\n");
		return 0;
	}

	int ret;
    char command[64] = {0};

	NET_DBG("\n...INet.cpp......NET_ReActive()......begin....\n");
	//如果是动态IP，则做重获IP处理
	if (m_bDHCP_Enable)
	{
#if 0
		return NET_RenewDhcp();
#else
		NET_DBG("\n...INet.cpp..IP为动态，重获IP处理.....\n");
		m_iNetDHCPFailed = 1;
		return 0;
#endif
	}

    snprintf(command, sizeof(command), "rm -rf %s", DHCP_GW_ADDR);
    mysystem(command);

	//从配置文件获取静态的IP地址
	m_pCConfig->GetValue(INet_CfgSection, INet_IP_key, INet_IP_Default_s, m_sLocalIP, MAX_IP_STR_LEN);

	//设置IP
	NET_DBG("\n...INet.cpp.......NET_ReActive().........设置IP.....\n");
	ret = set_if_address((char *) IFNAME, (char *) m_sLocalIP, SET_ADDRESS);
	if (ret)
	{
		NET_ERR("Set ipaddress failed (%d) m_sLocalIP =%s \n", ret, m_sLocalIP);
		return -1;
	}

    //当设备从DHCP设置成静态IP，而这个静态IP又是用的以前的地址，需要在这里将此变量清0，不然会造成easytool搜索到的IP与设备实际IP不一致的问题
	if (ipWillBeChanged && (strncmp(m_sLocalIP, futureIp, MAX_IP_STR_LEN) == 0))
	{
	    ipWillBeChanged = 0;
	}

	//设置子网掩码
	NET_DBG("\n...INet.cpp.........NET_ReActive().......设置子网掩码......\n");
	ret = set_if_address((char *) IFNAME, (char *) m_sSubNetMask, SET_NETMASK);
	if (ret)
	{
		NET_ERR("Set SubnetMask failed (%d) m_sSubNetMask =%s \n", ret, m_sSubNetMask);
		return -1;
	}

	//设置网关
	NET_DBG("\n...INet.cpp.........NET_ReActive().........设置网关....\n");
	ret = set_if_address((char *) IFNAME, m_sGateWay, SET_GATEWAY);
	if (ret)
	{
		NET_ERR("set_if_address<SET_GATEWAY> fail(%d), Gateway = %s\n", ret, m_sGateWay);
		return -1;
	}
	//NET_INFO("set_if_address<SET_GATEWAY> ok, Gateway = %s\n", m_sGateWay);

#ifndef DEL_MULTICAST_ROUTE
	mysystem("route add -net 224.0.0.0 netmask 224.0.0.0 dev eth0");//组播
#endif
	return 0;
}

//##ModelId=49BDBBB00207
int INet::NET_RenewDhcp()
{
	static int count =0;

    /* 如果当前设备不是eth0，则不去获取DHCP */
    if (1 != m_nCurrNetCardType)
    {
        NET_DBG("Don't do eth0 dhcp, m_nCurrNetCardType is [%d]\n", m_nCurrNetCardType);
        return 0;
    }

	if (0 == bSupportPhy)
	{
		NET_INFO(" not support eth0\n");
		return 0;
	}

	if (m_bDHCP_Enable)
	{
		if (m_iNetDHCPEnableShadow)//连续3次获取DHCP失败
		{
			count++;
			if(count >= 20)
				count = 0;//大概1分钟会进行一次尝试
			else
				return 0;
		}

		int result=0;
		FILE *fp;
		int bPDNSGet;
		char strGw[MAX_IP_STR_LEN];
        char strDns0[MAX_IP_STR_LEN] = {0};
        char strDns1[MAX_IP_STR_LEN] = {0};
		char line_buffer[256];
		char *pstr;
		
		NET_DBG("\n...INet.cpp.....INet...NET_RenewDhcp().....begin....\n");
	
        /* 由于现在采用了使用了DHCPC续租功能，会出现DHCPC程序一直在后台执行
         * 故以前的udhcpc已经启动这部分逻辑需要去掉，否则系统永远不会执行dhcpc命令
        */
		enableDhcpOperation(1, IFNAME);
		
		//完了后读配置
		NET_DBG("\n...INet.cpp......INet...NET_RenewDhcp()....读配置....\n");
		fp = fopen(DHCP_GW_ADDR, "r");
		if (fp == NULL)//DHCP不成功就不会创建此文件
		{
			NET_INFO("fopen<%s> failed\n", DHCP_GW_ADDR);
			m_iNetDHCPFailed = 1;
			m_iNetDHCPFailedTimes++;
			NET_DBG("m_iNetDHCPFailedTimes=%d\n", m_iNetDHCPFailedTimes);

            if (bSupportIPAdjust)
            {
    			if (DHCP_FAILE_TIMES <= m_iNetDHCPFailedTimes)
    			{
    				NET_DBG("dhcp fail %d times!\n", DHCP_FAILE_TIMES);
    				int connected = 0;
    				m_iNetDHCPFailedTimes = 0;
    				m_iNetDHCPFailed = 0;
    				//是否有连接
    				NET_IfIPCConnected(&connected);

    				if ((!m_iNetDHCPEnableShadow) && (!connected))
    				{
    					//自动生成IP，默认是192.168.1.X
    					//SetIpFromSN(&dat);
    				}
    				m_iNetDHCPEnableShadow = 1; 

    				return -1;
    			}
            }
		}
		else
		{
			m_iNetDHCPFailedTimes = 0;
			m_iNetDHCPEnableShadow = 0;
			if (NULL != fgets(strGw, MAX_IP_STR_LEN, fp))
			{
				//去掉换行					
				PubFunc_deleteNewlineInString(strGw, MAX_IP_STR_LEN);
				//检查IP是否合法
				if (!IsValidIP(strGw))
				{
					NET_ERR("invalid ip =%s\n", strGw);
				}
				else
				{
					TruncateIPString(strGw);
					#if 1
					result = set_if_address((char *) IFNAME, strGw, SET_GATEWAY);
					if (result)
					{
						NET_ERR("set_if_address<SET_GATEWAY> fail(%d), Gateway = %s\n", result, strGw);
					}
					#else
					result = NET_SetGateway(strGw);
					if (result)
					{
						NET_ERR("NET_SetGateway fail(%d), Gateway = %s\n", result, strGw);
					}
					#endif
					m_iNetDHCPFailed = 0;
					//NET_INFO("set_if_address<SET_GATEWAY> ok, Gateway = %s\n", strGw);
					
                    /* DHCP获取到了之后，需要操作一下路由 */
					bNeedUpdateRouter = 1;
				}
			}

			fclose(fp);
			fp = NULL;
		}

		//读DNS
		NET_DBG("\n...INet.cpp.......NET_RenewDhcp()....读DNS....\n");
		fp = fopen(RESOLV_ETH0_CONF, "r");
		if (fp == NULL)
		{
			NET_INFO("fopen<%s> failed\n", RESOLV_ETH0_CONF);
		}
		else if (0 == m_iNetDHCPFailed)
		{
			bPDNSGet = 0;
			memset(strDns0, 0, sizeof(strDns0));
			memset(strDns1, 0, sizeof(strDns1));
			memset(line_buffer, 0, 256);
			pstr = fgets(line_buffer, 256, fp);
			while ((!feof(fp)) && (NULL != pstr))
			{
				//Skip Comment Line
				if (strstr(line_buffer, "#") != NULL)
				{
				}
				else
				{
					if (bPDNSGet == 0)
					{
						if (sscanf(line_buffer, "nameserver %s", strDns0))
						{
							bPDNSGet = 1;
						}
					}
					else
					{
						sscanf(line_buffer, "nameserver %s", strDns1);
						NET_DBG("while exit\n");
						break;
					}
				}

				memset(line_buffer, 0, 256);
				pstr = fgets(line_buffer, 256, fp);
			}
			fclose(fp);

			//若获取失败,则使用网关作为DNS
			if (!IsValidIP(strDns0))
			{
				memset(strDns0, 0, sizeof(strDns0));
				snprintf(strDns0, sizeof(strDns0), "%s", strGw);
			}
			if (!IsValidIP(strDns1))
			{
				memset(strDns1, 0, sizeof(strDns1));
				snprintf(strDns1, sizeof(strDns1), "%s", strGw);
			}

			s_NetCardDhcpInfo.s_Eth0DhcpSuccFlag = 1;
			s_NetCardDhcpInfo.s_Eth0DhcpSuccTime = 1; // 先置为1，防止check 太慢
			ETH0_SetDNS(strDns0, strDns1);	
		}
#ifndef DEL_MULTICAST_ROUTE
		mysystem("route add -net 224.0.0.0 netmask 224.0.0.0 dev eth0");
#endif
        if (0 == g_DHCPEvent)
        {
            HieServer_RefreshEvent(USEREVENT_ALARM_NOTICE, ALERT_IPCHANGED, 0, 0);
    		HieServer_RefreshEventEx(USEREVENT_ALARM_NOTICE, 0, ALERT_IPCHANGED, 0, 0, NULL, 0);
            g_DHCPEvent = 1;
        }
	}
	return 0;
}

int INet::IsValidGW(const char *ipaddr)
{
	unsigned int ip;
    unsigned int ip1, ip2, ip3, ip4;
    sscanf(ipaddr, "%d.%d.%d.%d", &ip1, &ip2, &ip3, &ip4);
    if (ip1 == 255 || ip2 == 255 )
    {
        return 0;
    }	

	ip = inet_addr(ipaddr);
	if ((unsigned int) (-1) == ip || 0 == ip)
	{
		return 0;
	}
	return 1;
}

int INet::IsValidIP(const char *ipaddr)
{
	unsigned int ip;

	ip = inet_addr(ipaddr);
	if ((unsigned int) (-1) == ip || 0 == ip)
	{
		return 0;
	}
	return 1;
}
/**
********************************************************************************
 * @函数功能: 判断局域网（LAN）内IP地址是否有效
 * A类私有IP地址范围：10.0.0.0 到 10.255.255.255
 * B类私有IP地址范围：172.16.0.0 到 172.31.255.255
 * C类私有IP地址范围：192.168.0.0 到 192.168.255.255
 * @作者    : HL.LIU
 * @param[in] ip
 * @返回值  : 0-无效IP，1-有效IP
 * @其他    : 
********************************************************************************
*/
int INet::IsValidIpAddr(const char* ip) 
{
    int nums[4] = {0};
    char temp[MAX_IP_STR_LEN] = {0};
	char *saveptr = NULL;
	char *token = NULL;
	
    strncpy(temp, ip, sizeof(temp));
	token = strtok_r(temp, ".", &saveptr);
    
    if (NULL == token) 
	{
		return 0; //没有找到点分隔符
    }
	
    for (int i = 0; i < 4 && token != NULL; i++) 
	{
        nums[i] = atoi(token);
        if (nums[i] < 0 || nums[i] > 255)
		{
			return 0; //每段数字必须在0-255之间
		}
        token = strtok_r(NULL, ".", &saveptr);
    }
    
    if (token != NULL) 
	{
		return 0; //超过4段
	}
	
	//0通常保留用作网络地址表示网络本身, 255通常用作广播地址
    if (0 == nums[3] || 255 == nums[3])
    {
        return 0;
    }
    
    // 检查是否为私有IP地址
    if ((nums[0] == 10) ||
        (nums[0] == 172 && nums[1] >= 16 && nums[1] <= 31) ||
        (nums[0] == 192 && nums[1] == 168)) 
	{
        return 1; // 属于私有IP地址
    } 
	else 
	{
        return 0; // 不属于私有IP地址
    }
}

//##ModelId=49BB426A02D2

char *INet::NET_GetIPAddress(char *pszIPAddress,int addressSize)
{
	if (0 == bSupportPhy)
	{
		NET_INFO(" not support eth0\n");
		return pszIPAddress;
	}

	int ret;

	if (NULL == pszIPAddress)
	{
		NET_ERR("NULL==pszIPAddress\n");
		return NULL;
	}
	
	ret = get_if_address((char *) IFNAME, (char *) pszIPAddress, addressSize, GET_ADDRESS);
	if (ret)
	{
		*pszIPAddress = '\0';
		NET_ERR("Get ipaddress failed (%d) \n", ret);
		return NULL;
	}

	//意义改变，保存的是当前的IP，而不是以前设置的静态IP
	strncpy(m_sLocalIP, pszIPAddress, sizeof(m_sLocalIP) - 1);
	NET_DBG("Get ipaddress	(%s) \n", pszIPAddress);
	
	return pszIPAddress;
}

char *INet::NET_GetIPAddressEx(char *pszIPAddress,int addressSize)
{
	if (0 == bSupportPhy)
	{
		NET_DBG(" not support eth0\n");
		return pszIPAddress;
	}

	int ret;

	if (NULL == pszIPAddress)
	{
		NET_ERR("NULL==pszIPAddress\n");
		return NULL;
	}
	//如果有将要改的ip地址
	if(ipWillBeChanged)
    {   
		memcpy(pszIPAddress, futureIp, sizeof(futureIp));
    }
	else
	{
		ret = get_if_address((char *) IFNAME, (char *) pszIPAddress, addressSize, GET_ADDRESS);
		if (ret)
		{
			*pszIPAddress = '\0';
			NET_ERR("Get ipaddress failed (%d) \n", ret);
			return NULL;
		}
	}

	NET_DBG("\n---INet.cpp---------net init NET_GetIPAddressEx ---%s--ipWillBeChanged %d-\n", pszIPAddress, ipWillBeChanged);

	return pszIPAddress;
}

//mpu层设置ip前先调用此接口来保存将要设置的ip
//防止分离线程睡眠期间NVR获取了老的ip地址然后
//设置网络参数又把老ip地址设置下来了
int INet::NET_SetIPAddressEx(const char *pszIPAddress, int *ipChanged)
{
	int ret = 0;
	
	if(NULL != ipChanged)
	{
		*ipChanged = 0;
	}

	XTRACE_MUTEX_LOCK(&m_writeLock);

	//不管DHCP是否打开，均认为是设静态IP
	if (NULL == pszIPAddress)
	{
		NET_ERR("NULL == pszIPAddress\n");
		XTRACE_MUTEX_UNLOCK(&m_writeLock);
		return -1;
	}
	NET_DBG("\n---INet.cpp---------net init NET_SetIPAddressEx ---%s---\n", pszIPAddress);
	//检查IP是否合法
	if (!IsValidIP(pszIPAddress))
	{
		NET_ERR("invalid ip =%s\n", pszIPAddress);
		XTRACE_MUTEX_UNLOCK(&m_writeLock);
		return -1;
	}

	//检查IP地址最后一位是否是0或者255
	if (!IsValidIpAddr(pszIPAddress))
	{
		NET_ERR("invalid ip =%s\n", pszIPAddress);
		XTRACE_MUTEX_UNLOCK(&m_writeLock);
		return -1;
	}

	if (!IsValidGW(pszIPAddress))
	{
		NET_ERR("invalid ip =%s\n", pszIPAddress);
		XTRACE_MUTEX_UNLOCK(&m_writeLock);
		return -1;
	}

	int first = atoi(pszIPAddress);
	if((0 == first) || (127 == first) || (first > 223))
	{
		NET_ERR("ip first is invalid: %s\n", pszIPAddress);
		XTRACE_MUTEX_UNLOCK(&m_writeLock);
		return -1;
	}

	//判断IP是否更新
	if (strncmp(m_sLocalIP, pszIPAddress, MAX_IP_STR_LEN) == 0)
	{
		NET_INFO("The same IP set\n");
		XTRACE_MUTEX_UNLOCK(&m_writeLock);
		return 0;
	}

	memset(futureIp, 0, sizeof(futureIp));
	//把将要修改的ip地址存起来
	strncpy(futureIp, pszIPAddress, sizeof(futureIp) - 1);
	//标记有将要修改的ip，上层获取时判断用
	ipWillBeChanged = 1;

	if (NULL != ipChanged)
	{
		*ipChanged = 1;
	}

    if (bSupportWIFI)
    {
        /* 设置完IP地址后需要更新路由 */
        bNeedUpdateRouter = TRUE;
	}

	XTRACE_MUTEX_UNLOCK(&m_writeLock);
	
	return ret;
}

//##ModelId=49BB426A02D4
int INet::NET_SetIPAddress(const char *pszIPAddress)
{
	if (0 == bSupportPhy)
	{
		NET_INFO("not support eth0\n");
		return 0;
	}

	int ret = 0;
	static int firstCall = 1;

	XTRACE_MUTEX_LOCK(&m_writeLock);

	//不管DHCP是否打开，均认为是设静态IP
	if (NULL == pszIPAddress)
	{
		NET_ERR("NULL == pszIPAddress\n");
		XTRACE_MUTEX_UNLOCK(&m_writeLock);
		return -1;
	}
	NET_DBG("\n---INet.cpp---------net init NET_SetIPAddress ---%s---\n", pszIPAddress);
	//检查IP是否合法
	if (!IsValidIP(pszIPAddress))
	{
		NET_ERR("invalid ip =%s\n", pszIPAddress);
		XTRACE_MUTEX_UNLOCK(&m_writeLock);
		return -1;
	}

    //检查IP地址最后一位是否是0或者255
    if (!IsValidIpAddr(pszIPAddress))
    {
		NET_ERR("invalid ip =%s\n", pszIPAddress);
		XTRACE_MUTEX_UNLOCK(&m_writeLock);
		return -1;
    }

	if (!IsValidGW(pszIPAddress))
	{
		NET_ERR("invalid ip =%s\n", pszIPAddress);
		XTRACE_MUTEX_UNLOCK(&m_writeLock);
		return -1;
	}

#if 0
	//是否跟网关同网段，不允许不跟网关同一网段
	if(!NET_ifChangeGW(pszIPAddress))
	{
		NET_ERR("IPC in the not same gateway segment, not do IP change!\n");
		XTRACE_MUTEX_UNLOCK(&m_writeLock);
		return -1;
	}	
#endif	
    int first = atoi(pszIPAddress);
    if((0 == first) || (127 == first) || (first > 223))
    {
        NET_ERR("ip first is invalid: %s\n", pszIPAddress);
		XTRACE_MUTEX_UNLOCK(&m_writeLock);
        return -1;
    }

	//判断IP是否更新
	if (strncmp(m_sLocalIP, pszIPAddress, MAX_IP_STR_LEN) == 0)
	{
		NET_INFO("The same IP set\n");
		XTRACE_MUTEX_UNLOCK(&m_writeLock);
		return 0;
	}

	ret = set_if_address((char *) IFNAME, (char *) pszIPAddress, SET_ADDRESS);
	if (ret)
	{
		NET_ERR("Set ipaddress failed (%d) pszIPAddress =%s \n", ret, pszIPAddress);
		XTRACE_MUTEX_UNLOCK(&m_writeLock);
		return -1;
	}

	char szBroadcastAddr[MAX_IP_STR_LEN];

	get_if_address((char *) IFNAME, szBroadcastAddr, sizeof(szBroadcastAddr), GET_BROADCAST);
	set_if_address((char *) IFNAME, szBroadcastAddr, SET_BROADCAST);

	strncpy(m_sLocalIP, pszIPAddress, sizeof(m_sLocalIP) - 1);

	m_pCConfig->SetValue(INet_CfgSection, INet_IP_key, m_sLocalIP);
	
	if(!firstCall)
	{
		saveIpBak(m_sLocalIP);
	}
	firstCall = 0;
	
#ifndef DEL_MULTICAST_ROUTE
	mysystem("route add -net 224.0.0.0 netmask 224.0.0.0 dev eth0");//组播
#endif
    m_bIsAddNetRoute = 0;
	ipWillBeChanged = 0;

	XTRACE_MUTEX_UNLOCK(&m_writeLock);
	return ret;
}

char *INet::NET_GetSubnetMaskEx(char *pszSubnetMask,int SubnetMaskSize)
{
	if (0 == bSupportPhy)
	{
		NET_INFO(" not support eth0\n");
		return pszSubnetMask;
	}

	int ret;

	if (NULL == pszSubnetMask)
	{
		NET_ERR("NULL==pszSubnetMask\n");
		return NULL;
	}

	if (subMaskWillBeChanged)
    {
        memcpy(pszSubnetMask, futureSubMask, sizeof(m_sSubNetMask));
    }   
	else
	{
		ret = get_if_address((char *) IFNAME, (char *) pszSubnetMask, SubnetMaskSize, GET_NETMASK);
		if (ret)
		{
			*pszSubnetMask = '\0';
			NET_ERR("Get SubnetMask failed (%d) \n", ret);
			return NULL;
		}
	}
	//NET_DBG("\n---INet.cpp---------NET_GetSubnetMaskEx:%s------\n", pszSubnetMask);
	return pszSubnetMask;
}

//##ModelId=49BB426A02D6
char *INet::NET_GetSubnetMask(char *pszSubnetMask , int MaskSize)
{
	if (0 == bSupportPhy)
	{
		NET_DBG(" not support eth0\n");
		return pszSubnetMask;
	}

	int ret;

	if (NULL == pszSubnetMask)
	{
		NET_ERR("NULL==pszSubnetMask\n");
		return NULL;
	}

	ret = get_if_address((char *) IFNAME, (char *) pszSubnetMask, MaskSize, GET_NETMASK);
	if (ret)
	{
		*pszSubnetMask = '\0';
		NET_ERR("Get SubnetMask failed (%d) \n", ret);
		return NULL;
	}

	return pszSubnetMask;
}

int INet::NET_SetSubnetMaskEx(const char *pszSubnetMask, int *subMaskChanged)
{
	if (0 == bSupportPhy)
	{
		NET_INFO(" not support eth0\n");
		return -1;
	}

	//不管DHCP是否打开，均认为是设静态
	if (NULL == pszSubnetMask)
	{
		NET_ERR("NULL == pszSubnetMask\n");
		return -1;
	}
	
	NET_DBG("\n---INet.cpp---------NET_SetSubnetMaskEx:%s---m_sSubNetMask:%s---\n", pszSubnetMask, m_sSubNetMask);
	
	//检查IP是否合法
	if (!IsValidIP(pszSubnetMask))
	{
		NET_ERR("invalid ip =%s\n", pszSubnetMask);
		return -1;
	}
	
#if 1
	//判断IP是否更新
	if (strncmp(m_sSubNetMask, pszSubnetMask, MAX_IP_STR_LEN) == 0)
	{
		NET_INFO("The same subnet mask set\n");
		return 0;
	}
#endif

	memset(futureSubMask, 0, sizeof(futureSubMask));
	//把将要修改的ip地址存起来
	strncpy(futureSubMask, pszSubnetMask, sizeof(futureSubMask) - 1);
	//标记有将要修改的ip，上层获取时判断用
	subMaskWillBeChanged = 1;
	
	if(NULL != subMaskChanged)
		*subMaskChanged = 1;

	NET_DBG("\n---INet.cpp---------NET_SetSubnetMaskEx:%s----return --\n", pszSubnetMask);
	return 0;
}

//##ModelId=49BB426A02D8
int INet::NET_SetSubnetMask(const char *pszSubnetMask)
{
	if (0 == bSupportPhy)
	{
		NET_INFO(" not support eth0\n");
		return -1;
	}

	int ret;
	static int first = 1;

	//不管DHCP是否打开，均认为是设静态
	if (NULL == pszSubnetMask)
	{
		NET_ERR("NULL == pszSubnetMask\n");
		return -1;
	}
	//检查IP是否合法
	if (!IsValidIP(pszSubnetMask))
	{
		NET_ERR("invalid ip =%s\n", pszSubnetMask);
		return -1;
	}
#if 0
	//判断IP是否更新
	if (strncmp(m_sSubNetMask, pszSubnetMask, MAX_IP_STR_LEN) == 0)
	{
		NET_INFO("The same subnet mask set\n");
		return 0;
	}
#endif

	ret = set_if_address((char *) IFNAME, (char *) pszSubnetMask, SET_NETMASK);
	if (ret)
	{
		NET_ERR("Set subnetMask failed (%d) pszSubnetMask =%s \n", ret, pszSubnetMask);		
		subMaskWillBeChanged = 0;
		return -1;
	}
	strncpy(m_sSubNetMask, pszSubnetMask, sizeof(m_sSubNetMask) - 1);

	m_pCConfig->SetValue(INet_CfgSection, INet_SubnetMask_key, m_sSubNetMask);

	if(!first)
	{
		saveSubMaskBak(m_sSubNetMask);
	}
	first = 0;

#ifndef DEL_MULTICAST_ROUTE
	mysystem("route add -net 224.0.0.0 netmask 224.0.0.0 dev eth0");
#endif

	subMaskWillBeChanged = 0;

	return 0;
}

//##ModelId=49BB426A02DA
char *INet::NET_GetGateway(char *pszGateway, int gateWaySize)
{
	if (0 == bSupportPhy)
	{
		NET_DBG(" not support eth0\n");
		return pszGateway;
	}

	int ret;

	if (NULL == pszGateway)
	{
		NET_ERR("NULL == pszGateway\n");
		return NULL;
	}

	ret = get_if_address((char *) IFNAME, pszGateway, gateWaySize, GET_GATEWAY);
	if (ret)
	{
		*pszGateway = '\0';
		NET_INFO("get_if_address<GET_GATEWAY> fail(%d)\n", ret);
		return NULL;
	}
	//NET_INFO("get_if_address<GET_GATEWAY> ok, Gateway = %s\n", pszGateway);

	return pszGateway;
}

//##ModelId=49BB426A02DC
int INet::NET_SetGateway(const char *pszGateway)
{
	if (0 == bSupportPhy)
	{
		NET_INFO(" not support eth0\n");
		return -1;
	}

	int ret;

	//不管DHCP是否打开，均认为是设静态
	if (NULL == pszGateway)
	{
		NET_ERR("NULL == pszGateway\n");
		return -1;
	}
	//检查IP是否合法
	if (!IsValidIP(pszGateway))
	{
		NET_ERR("nvalid ip =%s\n", pszGateway);
		return -1;
	}
	
	if (!IsValidGW(pszGateway))
	{
		NET_ERR("invalid ip =%s\n", pszGateway);
		return -1;
	}
#if 0
	//判断IP是否更新
	if (strncmp(m_sGateWay, pszGateway, MAX_IP_STR_LEN) == 0)
	{
		NET_INFO("The same Gateway set\n");
		return 0;
	}
#endif
	ret = set_if_address((char *) IFNAME, (char *) pszGateway, SET_GATEWAY);
	if (ret)
	{
		NET_ERR("set_if_address<SET_GATEWAY> fail(%d), Gateway = %s\n", ret, pszGateway);
		return -1;
	}
	//NET_INFO("set_if_address<SET_GATEWAY> ok, Gateway = %s\n", pszGateway);

	strncpy(m_sGateWay, pszGateway, sizeof(m_sGateWay) - 1);
	m_pCConfig->SetValue(INet_CfgSection, INet_Gateway_key, m_sGateWay);
	saveGatewayBak(m_sGateWay);
	m_pCConfig->SaveFileReal();

	return 0;
}

//##ModelId=49BB426A02DE
unsigned int INet::NET_GetHttpPort()
{
	return m_uiHttpPort;
}

int INet::TestPort(unsigned int port)
{
	struct sockaddr_in sin;

	int sock_des;
	NET_DBG("TestPort start \n");

	if ((port > 65535) || (port <= 1))
	{
		NET_ERR("Invalid param(port=%d)\n", port);
		return 0;
	}
	sock_des = socket(AF_INET, SOCK_STREAM, 0);
	if (sock_des == -1)
	{
		NET_ERR("socket failed\n");
		return 0;
	}
	bzero(&sin, sizeof (sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = INADDR_ANY;
	sin.sin_port = htons(port);
	if (bind(sock_des, (struct sockaddr *) &sin, sizeof (sin)) == -1)
	{
		close(sock_des);
		NET_ERR("bind port failed\n");
		return 0;
	}
	close(sock_des);
	NET_DBG("TestPort over \n");
	return 1;
}

typedef struct taglink {
char *buf;
struct taglink *next;
} my_link, *LPmy_link;

int INet::NET_SetCommandPort(unsigned int usPort)
{
	int ret = 0;	

	if(m_uiCommandPort == usPort)
		return 1;

	ret = isValidPort(usPort);
	if((0 != ret) || (usPort == m_uiHttpPort) || (usPort == m_uiMediaPort) || (usPort == m_uiRtspPort)
		|| (usPort == m_uiMobilePort) || (usPort == m_uiAutoDetectPort)|| (usPort == m_uiHKPrivatePort))
	{
		NET_ERR("param error! port=%d\n", usPort);
		return -1;
	}

	if (m_pCConfig->SetValue(INet_PortCfgSection, "CommandPort", usPort) < 0)
	{
		NET_ERR("SetValue failed\n");
		return -1;
	}

	m_uiCommandPort = usPort ;

	return ret;
}

unsigned int INet::NET_GetCommandPort()
{
	return m_uiCommandPort;
}

int INet::NET_SetMediaPort(unsigned int usPort)
{
	int ret = 0;	

	if(m_uiMediaPort == usPort)
		return 1;
	
	ret = isValidPort(usPort);
	if((0 != ret) || (usPort == m_uiHttpPort) || (usPort == m_uiCommandPort) || (usPort == m_uiRtspPort)
		|| (usPort == m_uiMobilePort) || (usPort == m_uiAutoDetectPort)|| (usPort == m_uiHKPrivatePort))
	{
		NET_ERR("param error! port=%d\n", usPort);
		return -1;
	}

	if (m_pCConfig->SetValue(INet_PortCfgSection, "MediaPort", usPort) < 0)
	{
		NET_ERR("SetValue failed\n");
		return -1;
	}

	m_uiMediaPort = usPort ;

	return ret;
}

unsigned int INet::NET_GetMediaPort()
{
	return m_uiMediaPort;
}

int INet::NET_SetRtspPort(unsigned int usPort)
{
	int ret = 0;	

	if(m_uiRtspPort == usPort)
		return 1;//返回1，不返回0

	ret = isValidPort(usPort);
	if((0 != ret) || (usPort == m_uiHttpPort) || (usPort == m_uiCommandPort) || (usPort == m_uiMediaPort)
		|| (usPort == m_uiMobilePort) || (usPort == m_uiAutoDetectPort)|| (usPort == m_uiHKPrivatePort))
	{
		NET_ERR("param error! port=%d\n", usPort);
		return -1;
	}

	if (m_pCConfig->SetValue(INet_PortCfgSection, "RtpPort", usPort) < 0)
	{
		NET_ERR("SetValue failed\n");
		return -1;
	}

	m_uiRtspPort = usPort ;

	return ret;
}

unsigned int INet::NET_GetRtspPort()
{
	return m_uiRtspPort;
}

int INet::NET_SetAutoDetectPort(unsigned int usPort)
{
	int ret = 0;	

	if(m_uiAutoDetectPort == usPort)
		return 1;

	ret = isValidPort(usPort);
	if((0 != ret) || (usPort == m_uiHttpPort) || (usPort == m_uiCommandPort) || (usPort == m_uiMediaPort)
		|| (usPort == m_uiMobilePort) || (usPort == m_uiRtspPort)|| (usPort == m_uiHKPrivatePort))
	{
		NET_ERR("param error! port=%d\n", usPort);
		return -1;
	}

	if (m_pCConfig->SetValue(INet_PortCfgSection, "AutoDetectPort", usPort) < 0)
	{
		NET_ERR("SetValue failed\n");
		return -1;
	}

	m_uiAutoDetectPort = usPort ;

	return ret;
}

unsigned int INet::NET_GetAutoDetectPort()
{
	return m_uiAutoDetectPort;
}

int INet::NET_SetMobilePort(unsigned int usPort)
{
	int ret = 0;	

	if(m_uiMobilePort == usPort)
		return 1;

	ret = isValidPort(usPort);
	if((0 != ret) || (usPort == m_uiHttpPort) || (usPort == m_uiCommandPort) || (usPort == m_uiMediaPort)
		|| (usPort == m_uiAutoDetectPort) || (usPort == m_uiRtspPort) || (usPort == m_uiHKPrivatePort))
	{
		NET_ERR("param error! port=%d\n", usPort);
		return -1;
	}

	if (m_pCConfig->SetValue(INet_PortCfgSection, "MobilePort", usPort) < 0)
	{
		NET_ERR("SetValue failed\n");
		return -1;
	}

	m_uiMobilePort = usPort ;

	return ret;
}

unsigned int INet::NET_GetMobilePort()
{
	return m_uiMobilePort;
}

// for HK 
int INet::NET_SetHKPrivatePort(unsigned int usPort)
{
	int ret = 0;	

	if(m_uiHKPrivatePort == usPort)
		return 1;

	ret = isValidPort(usPort);
	if((0 != ret) || (usPort == m_uiHttpPort) || (usPort == m_uiCommandPort) || (usPort == m_uiMediaPort)
		|| (usPort == m_uiAutoDetectPort) || (usPort == m_uiRtspPort) || (usPort == m_uiMobilePort))
	{
		NET_ERR("param error! port=%d\n", usPort);
		return -1;
	}

	if (m_pCConfig->SetValue(INet_PortCfgSection, "HKPrivatePort", usPort) < 0)
	{
		NET_ERR("SetValue failed\n");
		return -1;
	}

	m_uiHKPrivatePort = usPort ;
    
	return ret;    
}

unsigned int INet::NET_GetHKPrivatePort()
{
    return m_uiHKPrivatePort;
}

/* 获取当前用的DID是否支持Subkey */
int INet::Net_IsSupportSetSubkeyByDID(void)
{
    return m_bIsSuportSubKey;
}

/* 设置推送AI参数 */
int INet::Net_SetPushAiParam(const char *pPushSubKey)
{
    if (NULL == pPushSubKey)
    {
        NET_ERR("NULL POINTER EXCEPTION!\n");
        return -1;
    }
    if (0 == strncmp(pPushSubKey, m_szPushSubKey, strlen(m_szPushSubKey)))
    {
        NET_WARN("Push Ai Param not change!\n");
        return 0;
    }

	if (m_pCConfig->SetValue(IPUSH_AI_CfgSection, IPUSH_AISubKey_Key, pPushSubKey) < 0)
	{
		NET_ERR("SetValue failed\n");
		return -1;
	}
    /* 由于该函数在NET模块内部调用，故需要调用Save函数对参数进行保存 */
    m_pCConfig->SaveFile();

    memset(m_szPushSubKey, 0, sizeof(m_szPushSubKey));
    strncpy(m_szPushSubKey, pPushSubKey, sizeof(m_szPushSubKey) - 1);

    return 0;
}

/* 获取推送AI参数 */
int INet::Net_GetPushAiParam(char *pPushSubKey)
{
    if (NULL == pPushSubKey)
    {
        NET_ERR("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    strncpy(pPushSubKey, m_szPushSubKey, strlen(m_szPushSubKey) - 1);

    return 0;
}

//##ModelId=49BB426A02DF
//  /mnt/mtd/config/httpd.conf    /etc/conf.d/boa/boa.conf      /mnt/mtd/config/Edvr.cfg
int INet::NET_SetHttpPort(unsigned int usPort)
{
	FILE *file_new;
	int needReboot=0;
	char buff[1024];
	int find=0;
	int i,nret=0;
	int result;

	//判断端口是否更新
	if (m_uiHttpPort == usPort)
	{
		NET_INFO("The same port set\n");
		return 0;
	}
	nret = isValidPort(usPort);
	if((0 != nret) || (usPort == m_uiCommandPort) || (usPort == m_uiMediaPort) || (usPort == m_uiAutoDetectPort) 
		|| (usPort == m_uiRtspPort) || (usPort == m_uiMobilePort) || (usPort == m_uiHKPrivatePort))
	{
		NET_ERR("param error! port=%d\n", usPort);
		return -1;
	}

	needReboot = 1;

NET_DBG("my add: NET_SetHttpPort %d\n",usPort);
	//测试端口是否有效
	if (!TestPort(usPort))
	{
		NET_ERR("*******the port %d can't use*******\n", usPort);
		return -1;
	}

#ifdef ADD_ONVIF
	FILE *fp = NULL;
	char *pfind;
	char cache[256];
	long insertPoint,copyPoint;
	NET_DBG("--------------------wait---------------------\n");

	XTRACE_MUTEX_LOCK(&m_writeLock);
	NET_DBG("----------------------pass-------------------\n");

	fp = fopen(BOA_CONF_PATH, "r+");
	if(NULL == fp)
	{
	   NET_ERR("--------------fopen %s fail -------------!\n", BOA_CONF_PATH);
	   XTRACE_MUTEX_UNLOCK(&m_writeLock);
	   return -1;
	}
	NET_DBG("@@@@@@@@@@@@@@@@@@@@@@@@111111@@@@@@@@@@@@@@@@@@@@@@@\n");

	while (!feof(fp))
	{
		memset(buff, 0, sizeof (buff));
		insertPoint = ftell(fp);
		fgets(buff, sizeof (buff), fp);
		copyPoint = ftell(fp);

		if (buff[0] != '#')
		{
			if(NULL != (pfind = strstr(buff, PORT_KEY)))
			{
				memset(cache, 0, sizeof(cache));
				snprintf(cache, sizeof(cache), "%s%d\n", PORT_KEY, usPort);
				fclose(fp);
				if(0 != insertDateToFile(BOA_CONF_PATH, insertPoint, copyPoint, cache))
				{
					NET_ERR("insertDateToFile fail, return <-1>\n");
					XTRACE_MUTEX_UNLOCK(&m_writeLock);
					return -1;
				}
				find=1;
				break;
			}
		}
	}
	if(0 == find)//如果未找见端口配置
	{
		NET_INFO("----%s  bad---recover to default---------\n", BOA_CONF_PATH);
		memset(buff, 0, sizeof(buff));
		snprintf(buff, sizeof(buff), "cp %s %s", BOA_CONF_BAK_PATH, BOA_CONF_PATH);
		mysystem(buff);
		usPort = 80;
	}
	NET_DBG("@@@@@@@@@@@@@@@@@@@@@@@@222222@@@@@@@@@@@@@@@@@@@@@@@\n",usPort);
	XTRACE_MUTEX_UNLOCK(&m_writeLock);
	i = Get_UID("/mnt/app/bin/boa");
	if (-1 != i)
	{
		snprintf(buff, sizeof(buff), "kill -9 %d", i);
		result = mysystem(buff);
		if (result)
		{
			NET_ERR("mysystem <%s> failed(%d)\n", buff, result);
			return -1;
		}
	}
	result = mysystem(BoaSeverProcStart);
	if (result)
	{
		NET_ERR("mysystem <%s> failed(%d)\n", BoaSeverProcStart, result);
		return -1;
	}
#else
    char buf[50];
    snprintf(buf, sizeof(buf), "/usr/sbin/httpd -p %d -h /home/http", usPort);
	result = mysystem(buf);
	if (result)
	{
		NET_ERR("mysystem <%s> failed(%d)\n", buf, result);
		return -1;
	}

#endif

	m_uiHttpPort = usPort;
	//写配置
	if (m_pCConfig->SetValue(INet_CfgSection, INet_HttpPort_key, usPort) < 0)
	{
		NET_ERR("SetValue failed\n");
		return -1;
	}
	NET_DBG("set http port success !\n");

	file_new = fopen(HTTP_SERVER_CFG_NAME_BAK, "w");

	if (file_new == NULL)
	{
		NET_ERR("open file failed [%s]\n", HTTP_SERVER_CFG_NAME_BAK);
		return -1;
	}
	
	memset(buff, 0, sizeof (buff));
	snprintf(buff, sizeof(buff), "%d", usPort);
	fprintf(file_new, "%s", buff);

	fclose(file_new);

	snprintf(buff, sizeof(buff), "mv %s %s", HTTP_SERVER_CFG_NAME_BAK, HTTP_SERVER_CFG_NAME);
	result = mysystem(buff);
	if (result)
	{
		NET_ERR("mysystem <%s> failed(%d)\n", buff, result);
		return -1;
	}

	NET_INFO("*****************needReboot=%d*****************\n",needReboot);
	return needReboot;
}

//!\brief得到命令为CmdName的进程号
//!\param char *CmdName为进程的命令号
//!\return 正确返回进程号，错误返回－1
int INet::Get_UID(const char *CmdName)
{
	DIR *dp;

	char buff[20];

	struct dirent *entry;

	struct stat statbuf;

	FILE *file;
	int uid = 0;

	if ((dp = opendir("/proc")) == NULL)
	{
		return -1;
	}
	//NET_DBG("Cmd len %d \n",strlen(CmdName));
	while ((entry = readdir(dp)) != NULL)
	{
		char Path[128] = "";

		snprintf(Path, sizeof(Path), "%s/%s", "/proc", entry->d_name);

		lstat(Path, &statbuf);
		if (S_ISDIR(statbuf.st_mode))
		{

			snprintf(Path, sizeof(Path), "%s/%s/%s", "/proc", entry->d_name, "cmdline");
			file = fopen(Path, "r");
			if (NULL != file)
			{
				fgets(buff, sizeof (buff), file);
				if (0 == strncmp(CmdName, buff, strlen(CmdName)))
				{
					NET_INFO("return %s\n", entry->d_name);
					uid = atoi(entry->d_name);
					fclose(file);
					closedir(dp);
					return uid;
				}
				fclose(file);
			}
		}
	}
	closedir(dp);
	return -1;
}

//##ModelId=49BB426A02E1
char *INet::NET_GetMacAddress(char *pszMacAddress, int macSize)
{
	if (0 == bSupportPhy)
	{
		NET_DBG(" not support eth0\n");
		return pszMacAddress;
	}

	static int first = 1;
	int ret;
	stuShmAuthInfo auth_info;
    unsigned int nAuthState = 0;
    unsigned char szAuthCode[256] = {0};
	memset(&auth_info, 0, sizeof(stuShmAuthInfo));

	if (NULL == pszMacAddress)
	{
		NET_ERR("NULL==pszMacAddress\n");
		return NULL;
	}

	if (first)
	{
		ret = get_if_address((char *) IFNAME, (char *) pszMacAddress, MAX_MAC_STR_LENGTH, GET_MAC);
		if (ret)
		{
			NET_ERR("Set MacAddress failed (%d) pszMacAddress =%s \n", ret, pszMacAddress);
			return NULL;
		}
		first = 0;
	}
	else
	{
		snprintf(pszMacAddress, MAX_MAC_STR_LENGTH, "%02x:%02x:%02x:%02x:%02x:%02x",
			(unsigned char)m_sMAC_addr[0],
			(unsigned char)m_sMAC_addr[1],  
			(unsigned char)m_sMAC_addr[2],  
			(unsigned char)m_sMAC_addr[3], 
			(unsigned char)m_sMAC_addr[4], 
			(unsigned char)m_sMAC_addr[5]);
	}

	// 如果有线mac地址不合法(比如为空)，则从授权里面获取
	// mac 地址长度17 >>> szMac :5a:5a:00:44:d6:b4 strlen(szMac)  :17
	if (17 != strlen(pszMacAddress))
	{
		NET_ERR("invalid mac addr :%s strlen(pszMacAddress) :%d , need get from auth once !\n", pszMacAddress, strlen(pszMacAddress));
		ZWSys_GetAuthInfo(szAuthCode, &nAuthState);
		/* 如果未授权、取消授权或者授权失败，则不解析mac */
		if (!nAuthState)
		{
			NET_ERR("IPC not authorize, please authorize it and retry!\n");
			return NULL;
		}
        
        Authorize_GetAuthDevInfo(&auth_info);

    	snprintf(pszMacAddress, MAX_MAC_STR_LENGTH, "%02x:%02x:%02x:%02x:%02x:%02x",
        	(unsigned char)auth_info.szMac[0],
       		(unsigned char)auth_info.szMac[1],
        	(unsigned char)auth_info.szMac[2],
       		(unsigned char)auth_info.szMac[3],
       	    (unsigned char)auth_info.szMac[4],
        	(unsigned char)auth_info.szMac[5]);

		for (int num= 0; num < 6; ++num)
		{
			if (m_sMAC_addr[num] != auth_info.szMac[num])
			{
				m_sMAC_addr[num] = auth_info.szMac[num];
			}
		}
		//NET_ERR("init pszMacAddress :%s auth :%s size :%d \n", pszMacAddress, auth_info.szMac, sizeof(auth_info.szMac));
	}

	return pszMacAddress;
}

CConfig *INet::GetConfig()
{
    return m_pCConfig;
}

#if defined (__IS_SUPPORT_P6SMESSAGE__)
static int GetDeviceParam(ZWPush_P6SMSG_SERVER_INFO *ServerInfo)
{
	int Reversion = 0;
	int Major_version = 0;
	int Minor_version = 0;
	char BuildDate[9] = {0};

	DeviceDescription s_tDevCap_pub = {0};
	stuShmAuthInfo stShmAuthInfo = {0};

	CConfig *cCfg = CConfig::GetInstance();

	unsigned int i = 0;
	int nRet = 0;
	char chip_id[64] = {0};

	unsigned int nAuthState = 0;
    unsigned char szAuthCode[256] = {0};

	static int s_accesssNum = 0;               //同一参数第一次从接口获取值，第二次直接用静态变量的值
	static char s_boardId[64] = {0};  		   //boardid 
	static int s_customer = 0;				   // 客户id
	static char s_mac[64] = {0};               // mac地址
	static char s_sn[64] = {0};                // sn号
	static char s_deviceType[128] = {0};       // 设备型号
    static char s_softwareVersion[128] = {0};  // 固件版本
    static char s_versionId[64] = {0};         // 客户版本id
	static char s_chipId[64] = {0};			   // chip id
	static unsigned char s_videoChannels;      //通道数 
	static unsigned char s_audioChannels; 	   //音频通道数 
	static unsigned char s_alarmInChannels;    //报警输入通道数 
	static unsigned char s_alarmOutChannels;   //报警输入通道数 
	static unsigned char s_voiceInChannels;    //报警输入通道数 

	ZWSys_GetAuthInfo(szAuthCode, &nAuthState);
    if (0 == nAuthState)
    {
        return AUTH_NOT_AUTHORIZE;
    }
	else
	{
		if(0 == s_accesssNum)
		{
			// szClientId 优先使用授权码内部的customID
            Authorize_GetAuthDevInfo(&stShmAuthInfo);
			if (AUTH_SUCCESS == stShmAuthInfo.dwAuthVerifyErrno)
			{
				snprintf(ServerInfo->boardId, sizeof(ServerInfo->boardId), "%lX", stShmAuthInfo.dwBoardID);//获取boardId
				memcpy(s_boardId, ServerInfo->boardId, sizeof(ServerInfo->boardId));

				ServerInfo->customer = stShmAuthInfo.dCustomID;//客户id
				s_customer = stShmAuthInfo.dCustomID;

				snprintf(ServerInfo->mac, sizeof(ServerInfo->mac), "%02x:%02x:%02x:%02x:%02x:%02x", 
				stShmAuthInfo.szMac[0], stShmAuthInfo.szMac[1], stShmAuthInfo.szMac[2],
				stShmAuthInfo.szMac[3], stShmAuthInfo.szMac[4], stShmAuthInfo.szMac[5]);//mac地址
				memcpy(s_mac, ServerInfo->mac, sizeof(ServerInfo->mac));

				snprintf(ServerInfo->sn, sizeof(ServerInfo->sn), "%s", stShmAuthInfo.szSerialNum);//序列号
				memcpy(s_sn, ServerInfo->sn, sizeof(ServerInfo->sn));
			}

			//获取设备型号
			for (i = 0; i < gstuBoardInfoSize; i++)
			{
				if (stShmAuthInfo.dwBoardID == gstuBoardInfo[i].dwBoardId)
				{
					snprintf(ServerInfo->deviceType, sizeof(ServerInfo->deviceType), "%lX", gstuBoardInfo[i].nDeviceType);//获取设备型号
					memcpy(s_deviceType, ServerInfo->deviceType, sizeof(ServerInfo->deviceType));
				}
			}

			//获取固件版本
			cCfg = CConfig::NewInstance();
			nRet = cCfg->LoadFile(IDVR_VERSION_CONFIG_FILE_PATH);
			if (0 == nRet)
			{
				//软件版本
				Major_version = cCfg->GetValue("Software", "Major_version", 0);
				Minor_version = cCfg->GetValue("Software", "Minor_version", 0);
				Reversion = cCfg->GetValue("Software", "Reversion", 0);
				cCfg->GetValue("Software", "BuildDate", "", BuildDate, sizeof(BuildDate));
			}
			snprintf(ServerInfo->softwareVersion, sizeof(ServerInfo->softwareVersion), "%d.%d.%d-%s", Major_version, Minor_version, Reversion, BuildDate);
			memcpy(s_softwareVersion, ServerInfo->softwareVersion, 14);

			//获取客户版本id
			nRet = cCfg->LoadFileRO(CUSTOM_INFO);
			if(0 == nRet)
			{
				cCfg->GetValue(CLOUD_UPGRADE_CUSTOM_SECTION_NAME,
						CLOUD_UPGRADE_CUSTOM_CUSTOM_NO_KEY_NAME, 
						CLOUD_UPGRADE_CUSTOM_CUSTOM_NO_VALUE_DEF, 
						ServerInfo->versionId, sizeof(ServerInfo->versionId));
				memcpy(s_versionId, ServerInfo->versionId, sizeof(ServerInfo->versionId));
			}
			CConfig::ReleaseInstance(cCfg);

			//获取chipid
			nRet = GetDeviceUUID(chip_id, sizeof(chip_id)-1);
			if (0 == nRet)
			{
				memcpy(ServerInfo->chipId, chip_id, 24);
				memcpy(s_chipId, chip_id, 24);
			}

			//获取设备能力
			EDVRS_DDT_GetDeviceDescriptionByID(&s_tDevCap_pub, stShmAuthInfo.dwBoardID);

			//设备型谱名称，如HY-2016HC
			ServerInfo->videoChannels = s_tDevCap_pub.bytVideoInChannels;  /*视频输入路数 */
			ServerInfo->audioChannels = s_tDevCap_pub.bytAudioInChannels;  /*音频输入路数 */
			ServerInfo->alarmInChannels = s_tDevCap_pub.bytAlarmInChannels;  /*报警输入路数 */
			ServerInfo->alarmOutChannels = s_tDevCap_pub.bytAlarmOutChannels;    /*报警输出路数 */
			ServerInfo->voiceInChannels = s_tDevCap_pub.bytVoiceInChannels;  /*语音对讲输入路数 */

			s_videoChannels = ServerInfo->videoChannels;
			s_audioChannels = ServerInfo->audioChannels;
			s_alarmInChannels = ServerInfo->alarmInChannels;
			s_alarmOutChannels = ServerInfo->alarmOutChannels;
			s_voiceInChannels = ServerInfo->voiceInChannels; 

			s_accesssNum++;
		}
		else
		{
			memcpy(ServerInfo->boardId, s_boardId, sizeof(s_boardId));
			ServerInfo->customer = s_customer;
			memcpy(ServerInfo->mac, s_mac, sizeof(s_mac));
			memcpy(ServerInfo->sn, s_sn, sizeof(s_sn));
			memcpy(ServerInfo->deviceType, s_deviceType, sizeof(s_deviceType));
			memcpy(ServerInfo->softwareVersion, s_softwareVersion, sizeof(s_softwareVersion));
			memcpy(ServerInfo->versionId, s_versionId, sizeof(s_versionId));
			memcpy(ServerInfo->chipId, s_chipId, 24);
			ServerInfo->videoChannels = s_videoChannels;
			ServerInfo->audioChannels = s_audioChannels;
			ServerInfo->alarmInChannels = s_alarmInChannels;
			ServerInfo->alarmOutChannels = s_alarmOutChannels;
			ServerInfo->voiceInChannels = s_voiceInChannels;
		}
	}

	return nRet;
}
#endif

static void *Fnx_PushInitThread(void *lParam)
{
#if defined (__USE_P2P_PUSH__)
    PRT_TID();

    INet *pNet = NULL;
    TutkCfg stTutkCfg = {0};
    unsigned long ulTutkStatus = 0;
    ZWP2P_UINT32 zwRet = 0;
    ZWP2P_PUSH_INIT_PUSH_PARAMETERS stInitPushParam = {0};
    ZWP2P_PUSH_SUBKEY_INPUT_PARAMS PushSubkeyParameters = {0};
	#if defined(__IS_SUPPORT_DYNAMIC_GRAPHICS__)
	CDynamicGraphics *pCDynamicGraphics = CDynamicGraphics::GetInstance();
	#endif
#if defined (__IS_SUPPORT_P6SMESSAGE__)
	ZWPush_P6SMSG_SERVER_INFO ServerInfo = {0};
#endif
    char szSubKey[64] = {0};
    FILE *fp = NULL;
	CConfig *cCfg = CConfig::GetInstance();

    if (NULL == lParam)
    {
        return (void *)-1;
    }

	pNet = (INet *)lParam;
	pNet->NET_GetTutkUID(&stTutkCfg, &ulTutkStatus);
    pNet->NET_GetMacAddress(PushSubkeyParameters.cMac);

    snprintf(PushSubkeyParameters.SubkeyParamDir, sizeof(PushSubkeyParameters.SubkeyParamDir), "%s", SUBKEY_PARAM_DIR);
	NET_INFO("[ZK-DEBUG]SubkeyParamDir=%s, cMac=%s\n", PushSubkeyParameters.SubkeyParamDir, PushSubkeyParameters.cMac);

	while (1)
	{
		zwRet = ZWP2PPUSHVNClient_InitSubkeyParam(&PushSubkeyParameters);
		NET_INFO("ZWP2PPUSHVNClient_InitSubkeyParam End,Ret=%d\n", zwRet);
		if (eZWP2P_PUSH_ERR_SUCCESS != zwRet)
		{
		   NET_ERR("ZWP2PPUSHVNClient_InitSubkeyParam failed(%d)(%s)(%s)\n", 
					zwRet, PushSubkeyParameters.cMac, PushSubkeyParameters.SubkeyParamDir);
		}
		else
		{
			NET_INFO("ZWP2PPUSHVNClient_InitSubkeyParam success!\n");
			break;	
		}
		sleep(1);
	}

	int DomainSuffixExist = 0;
    int nRet = 0;
	char command[128] = {0};

	pNet->Net_GetAuthDomainSuffixExist(&DomainSuffixExist);
	
	//在 ZWP2PPUSHVNClient_InitSubkeyParam之后判断是否需要重新设置域名
	if (DomainSuffixExist == 1)
	{
		char tutk[64] = {0};
		char domainsuffix[64] = {0};
		ZWP2P_PUSH_DOMAIN_PARAMS stZWPushDomainParams = {0};

		nRet = pNet->Net_GetTutkAndDomainSuffix(tutk, sizeof(tutk), domainsuffix, sizeof(domainsuffix));
		strncpy(stZWPushDomainParams.cDID, tutk, sizeof(stZWPushDomainParams.cDID));
		strncpy(stZWPushDomainParams.cDomainSuffix, domainsuffix, sizeof(stZWPushDomainParams.cDomainSuffix));

		nRet = ZWP2PPUSHVNClient_SetDomainParam(&stZWPushDomainParams);
		NET_INFO("Reinit DomainParam return:%d!\n", nRet);

		memset(command, 0, sizeof(command));
		snprintf(command, sizeof(command), "rm -r %s", TUTK_UID_CHANGE_FLAG);
		mysystem(command);
	}

#if defined (__IS_SUPPORT_4G_CLOUD_DOMAIN__)
    TutkCfg TutkDid;
    memset(&TutkDid, 0, sizeof(TutkDid));
    stuConnectInfo connect_info;    
    memset(&connect_info, 0, sizeof(connect_info));
    ZWHost_DomainListManagerServerParam DomainListParam;
    memset(&DomainListParam, 0, sizeof(ZWHost_DomainListManagerServerParam));

    pNet->NET_4G_GetTutkUID(&TutkDid);
    snprintf(DomainListParam.cDID,sizeof(DomainListParam.cDID),"%s",TutkDid.szTutkUID);

    Authorize_GetConnectInfo(GG_CLOUD_LINENUM, &connect_info);

    if (sscanf(connect_info.sz4GCloud, "%255[^#]#%d", DomainListParam.cDomainServer, &DomainListParam.port) == 2)
    {
        NET_INFO("Hosts_SetDomainListManagerServer Host: %s\n", DomainListParam.cDomainServer);
        NET_INFO("Hosts_SetDomainListManagerServer Port: %d\n", DomainListParam.port);
        Hosts_SetDomainListManagerServer(&DomainListParam);
    }
    else
    {
        NET_INFO("Invalid input format.\n");
    }
#endif

	if (strnlen(stTutkCfg.szPushID, sizeof(stTutkCfg.szPushID)-1) > 0)
	{
	    /* 利用新的DID进行初始化推送 */
	    memset(stInitPushParam.cDeviceDid, 0, ZWP2P_PUSH_MAX_DID_LENGTH);
        snprintf(stInitPushParam.cDeviceDid, ZWP2P_PUSH_MAX_DID_LENGTH, "%s%c%s", stTutkCfg.szTutkUID, MAX_PUSH_ID_SFD, stTutkCfg.szPushID);
        stInitPushParam.nPushInterval = 3;
        stInitPushParam.nMaxEventCount = 5;
        NET_INFO("Call ZWP2PPUSHVNClient_Initialize===DID=%s\n", stInitPushParam.cDeviceDid);
        zwRet = ZWP2PPUSHVNClient_Initialize(&stInitPushParam);
        if (eZWP2P_PUSH_ERR_SUCCESS != zwRet)
        {
            NET_ERR("ZWP2PPUSHVNClient_Initialize failed(%d)\n", zwRet);
            pNet->Net_SetZwP2pPushInit(0);
        }
        else
        {
            NET_INFO("ZWP2PPUSHVNClient_Initialize success!\n");
            pNet->Net_SetZwP2pPushInit(1);
			
#if defined (__IS_SUPPORT_P6SMESSAGE__)
			//获取设备参数
			GetDeviceParam(&ServerInfo);
			ZWP2PPUSHVNClient_SetP6SMSGServerInfo(ServerInfo);
			NET_INFO("ZWP2PPUSHVNClient_SetP6SMSGServerInfo End\n");
#endif

			//在初始化成功后调用
			while (1)
	        {
	            /* 循环产生subkey直至成功退出 */
				zwRet = ZWP2PPUSHVNClient_GetSubkey(szSubKey, sizeof(szSubKey));
				NET_INFO("ZWP2PPUSHVNClient_GetSubkey End, szSubKey=%s, Ret=%d\n", szSubKey, zwRet);
	            if (eZWP2P_PUSH_ERR_SUCCESS == zwRet)
	            {
	                /* 保存当前的SubKey */
	                pNet->Net_SetPushAiParam(szSubKey);

	                /* 将本次的UID保存到配置文件 */
	                fp = fopen(LAST_TUTK_UID_FILE_PATH, "w");
	                if (NULL == fp)
	                {
	                    NET_ERR("open file failed [%s]\n", LAST_TUTK_UID_FILE_PATH);
	                }
	                else
	                {
	                    fprintf(fp, "%s", stInitPushParam.cDeviceDid);
	                    fclose(fp);
	                    fp = NULL;
	                    NET_INFO("write did %s to LAST_TUTK_UID_FILE_PATH over!\n", stInitPushParam.cDeviceDid);
	                }
	                /* 只有生成成功才能清除标记文件 */
	                unlink(SUBKEY_NEED_GENERAL_FLAG);
	                bNeedGenSubKey = 0;
	                NET_INFO("ZWP2P general SubKey success! push subkey is %s\n", szSubKey);
	                break;
	            }
	            else
	            {
	                /* 否则如果失败，则需要一直调用该接口，进行判断是否生成成功(如果重启，重启之后还有继续) */
	                NET_INFO("ZWP2PPUSHVNClient_GetSubkey! ret = %d\n", zwRet);
	            }
	            sleep(1);
	        }
        }

#if defined (__AUTH_V2__)
        stuConnectInfo connect_info;    
        memset(&connect_info, 0, sizeof(connect_info));

		zwRet = Authorize_GetConnectInfo(PUSH_KEY_LINENUM, &connect_info);
		if (zwRet != 0)
		{
			NET_ERR("Get AuthWord failed\n");
		}

		zwRet = ZWP2PPUSHVNClient_SetCustomAuthWord((char *)connect_info.szAuthWord);
		if (eZWP2P_PUSH_ERR_SUCCESS != zwRet)
        {
            NET_ERR("ZWP2PPUSHVNClient_SetCustomAuthWord failed(%d)\n", zwRet);
        }
#endif
    }
    else
    {
        NET_INFO("Not need init push stTutkCfg.szPushID=%s!\n", stTutkCfg.szPushID);
        pNet->Net_SetZwP2pPushInit(0);
    }

#if defined (__IS_SUPPORT_P6SMESSAGE__)
	while (1)
	{
		cCfg->GetValue("P6SMessage", "ServerIpAndPort", "", ServerInfo.url, sizeof(ServerInfo.url));
		if (strlen(ServerInfo.url))
		{
			ZWP2PPUSHVNClient_SetP6SMSGServerURL(ServerInfo.url);
			NET_INFO("ZWP2PPUSHVNClient_SetP6SMSGServerURL End, url is %s\n", ServerInfo.url);
			break;
		}
		sleep(1);
	}
#endif

#if defined(__IS_SUPPORT_DYNAMIC_GRAPHICS__)
	pCDynamicGraphics->Init();
#endif

#endif

    return (void *)NULL;
}

#if defined (__IS_SUPPORT_WLAN__)
static void check_wifi_link_status(INet *pnet)
{
    static int tick = 0;
    static int tickFlag = 0;
    int nCounter = 0;
    int pnStatus = 0;

    if (tickFlag == 0)
    {
        tick++;
        if (tick == 1 || tick == 15 || tick == 35)
        {
            /*获取wifi的配网状态 */
            pnet->WLAN_GetWifiLinkStatus(&pnStatus);
            if (pnStatus == 0)
            {
                if (tick == 35)
                {
                    tickFlag = 1024;
                    tick = 0;
                }

                /* 开启二维码扫描 */
                if (bSupportQrScan && tick == 1)
                {
                    SetQRScanStatus(1);
                }

                /* 提示进行配网 */
                ZWSys_SetSpeechContent(SPEECH_WIFI_PLEASE_CONNECT);
                //循环查询，等待播放结束
                while (1)
                {
                    sleep(1);
                    if (0 == ZWSys_GetSpeechPlayingStatus())
                    {
                        break;
                    }
                    if (++nCounter > SPEECH_PLAY_TIME_OUT_SEC)
                    {
                        nCounter = 0;
                        break;
                    }
                }
            }
            else
            {
                tickFlag = 1024;
            }
        }
    }

}
#endif 

static void *Fnx_CheckThread(void *lParam)
{
    PRT_TID_NAME("NET_CheckThread");

	int ret = 0;
	INet *pnet = (INet *) lParam;
	if (NULL == pnet)
	{
		NET_ERR("NULL==pnet\n");
		return NULL;
	}

	while (1)
	{
		ret = pnet->NET_CheckStatus();
		if (ret == -1)
		{
			NET_ERR("thread exit...\n");
			break;
		}
#if defined (__IS_SUPPORT_WLAN__)
		if (bSupportWIFI)
		{
			ret = pnet->WLAN_CheckStatus();
			if (ret == -1)
			{
				NET_ERR("thread exit...\n");
				break;
			}

		    if ( pnet->BluetoothIsOpen() && pnet->Get_BluetoothStartTime() > 300 ) 
            {
                NET_INFO("get Ble RunningTime > 300s, stop Bluetooth Work\n");
                pnet->WLAN_BluetoothClose();
            }


            /* 超时关闭二维码扫描 */
            if ((bSupportQrScan) && (0 != GetQRScanStatus()) && (0 != GetQRScanCloseFlags()))  
            {
                SetQRScanStatus(0);
            }

            /* 启动时等待连接提示音处理 */
            check_wifi_link_status(pnet);
		}
#endif

#if defined (__USE_APPLY_DID_MANUAL__)
        if (g_TimtOut) {
            g_TimtCount++;
        }

        if (g_TimtCount >= 9) {
            pnet->m_iApplyStatus = 2;
            g_TimtCount = 0;
            g_TimtOut = 0;
        }
#endif
		// 有线dns  
		if (1 == s_NetCardDhcpInfo.s_Eth0DhcpSuccFlag)
		{
			s_NetCardDhcpInfo.s_Eth0DhcpSuccTime ++;
		}
		else
		{
			s_NetCardDhcpInfo.s_Eth0DhcpSuccTime = 0;
		}

		if (s_NetCardDhcpInfo.s_Eth0DhcpSuccTime >= PROTOCOL_SETDNS_WAIT_DHCPSUCC_INTERVAL)
		{
			s_NetCardDhcpInfo.s_Eth0DhcpSuccTime = 0;
			s_NetCardDhcpInfo.s_Eth0DhcpSuccFlag = 0;
		}

		// 无线dns 
		if (1 == s_NetCardDhcpInfo.s_Wlan0DhcpSuccFlag)
		{
			s_NetCardDhcpInfo.s_Wlan0DhcpSuccTime++;
		}
		else
		{
			s_NetCardDhcpInfo.s_Wlan0DhcpSuccTime = 0;
		}
		
		if (s_NetCardDhcpInfo.s_Wlan0DhcpSuccTime >= PROTOCOL_SETDNS_WAIT_DHCPSUCC_INTERVAL)
		{
			s_NetCardDhcpInfo.s_Wlan0DhcpSuccTime = 0;
			s_NetCardDhcpInfo.s_Wlan0DhcpSuccFlag = 0;
		}
        
        /* P2P断开，但是设备能连外网情况下进行重启恢复 */
        pnet->checkP2PStatusAbnormal();

        sleep(1);               //一秒检测一次
	}
	return NULL;
}

static void *IPAdjustProcessThread(void *lParam)
{
    PRT_TID();

	INet *pnet = (INet *) lParam;
	int enable = 1;
	int connected = 0;
	char localIP[IP_MAX_LEN] = {0};
	char defaultIP[IP_MAX_LEN] = {0};
	char tmpDetectData[IP_MAX_LEN] = {0};
	int tmpDetectNum = 0;
	int i = 0;
	int count = MULTI_DETECT_CYCLE;

	while (1)
	{
		pnet->NET_IfIPCConnected(&connected);
		pnet->NET_GetIPAddressAdjustEnable(&enable);
		if (enable && !connected && (count >= MULTI_DETECT_CYCLE + ipadjust_count * 10))//使能ip自适应且没有流连接,并且计数超过MULTI_DETECT_CYCLE
		{
			memset(tmpDetectData, 0, IP_MAX_LEN);
			//把全局的资源队列的队列头的数据取出来
			XTRACE_MUTEX_LOCK(&pnet->m_detectResourceLock);
			tmpDetectNum = pnet->detectNum;
			if (tmpDetectNum > 0)
			{
				memcpy(tmpDetectData, pnet->detectQueue[0], IP_MAX_LEN);
				count = 0;
			}
			else
			{
				XTRACE_MUTEX_UNLOCK(&pnet->m_detectResourceLock);
				sleep(1);
				continue;
			}
			XTRACE_MUTEX_UNLOCK(&pnet->m_detectResourceLock);

			pnet->NET_GetIPAddressAdjustEnable(&enable);
			//是否开启IP自适应
			if(0 == enable)
			{
				NET_DBG("IP adjust is disable!\n");
				sleep(1);
				count = 0;
				continue;
			}
			
			//是否是同网段
			if(pnet->NET_ifChangeNet(tmpDetectData))
			{
				if (1 == pnet->firstSameIP)
				{
					CConfig *pCfg = CConfig::GetInstance();
					pCfg->GetValue(INet_CfgSection, "LocalIP", "192.168.1.11", localIP, sizeof(localIP));
					pCfg->GetValue(INet_CfgSection, "LocalIP", "192.168.1.11", localIP, sizeof(localIP));

					//获取新的配置文件读取实例
					int ret = 0;
					CConfig *pDefCfg = CConfig::NewInstance();
					if (NULL != pDefCfg)
					{
						ret = pDefCfg->LoadFile(IDVR_DEFAULT_CONFIG_FILE_PATH);
						if (0 == ret)
						{
							pDefCfg->GetValue(INet_CfgSection, "LocalIP", "192.168.1.11", defaultIP, sizeof(defaultIP));
						}
					}
					pDefCfg->ReleaseInstance(pDefCfg);
					if (strncmp(localIP, defaultIP, IP_MAX_LEN))//当前IP是否和默认IP相同，相同则可以改IP，不相同不能改IP
					{
						NET_WARN("IPC the local ip is same as deflaut ip, not do IP adjust!\n");
						goto deleteData;	
					}

					pnet->firstSameIP = 0;
				}
				else
				{
					NET_DBG("IPC in the same network segment, not do IP adjust!\n");
					goto deleteData;	
				}
			}

			//是否有连接
			pnet->NET_IfIPCConnected(&connected);
			if(connected)
			{
				NET_DBG("I am connected, don't do IP adjust!\n");
				sleep(1);
				count = 0;
				continue;
			}

			XTRACE_MUTEX_LOCK(&pnet->m_writeLock);

			//根据后端生成IP
			if(pnet->SetIpRand(tmpDetectData))
			{
				NET_ERR("NET_SetIPAddressByNVR---rand ip fail!\n");
				sleep(1);
				count = 0;
				XTRACE_MUTEX_UNLOCK(&pnet->m_writeLock);
				continue;
			}
			
			//关闭DHCP
			pnet->NET_SetDhcpEnable(0);
			pnet->NET_GetIPAddress(localIP, sizeof(localIP));
			
			XTRACE_MUTEX_UNLOCK(&pnet->m_writeLock);

			deleteData:
			XTRACE_MUTEX_LOCK(&pnet->m_detectResourceLock);
			for (i = 1; i < pnet->detectNum; i++)
			{
				//把队列前移
				memcpy(pnet->detectQueue[i-1], pnet->detectQueue[i], IP_MAX_LEN);
			}
			//清空队列的最后一个
			memset(pnet->detectQueue[pnet->detectNum-1], 0, IP_MAX_LEN);
			pnet->detectNum--;
			XTRACE_MUTEX_UNLOCK(&pnet->m_detectResourceLock);
		}
		else if (!enable || connected)
		{
			//清空队列
			XTRACE_MUTEX_LOCK(&pnet->m_detectResourceLock);
			for (i = 0; i < pnet->detectNum; i++)
			{
				memset(pnet->detectQueue[i], 0 , sizeof(pnet->detectQueue[i]));
			}
			pnet->detectNum = 0;
			XTRACE_MUTEX_UNLOCK(&pnet->m_detectResourceLock);
		}
		
		sleep(1);				//一秒检测一次
		
		count++;
		count = count >= MULTI_DETECT_CYCLE + ipadjust_count * 10 ? MULTI_DETECT_CYCLE + ipadjust_count * 10 : count;
	}
	return NULL;
}

static void *IPAdjust_CheckThread(void *lParam)
{
    PRT_TID();

	int tick=0;
	int connected;
	int ipAdjustEnable=1;
	INet *pnet = (INet *) lParam;
	char bFirst = 1;
	char localIP[IP_MAX_LEN] = {0};
	char defaultIP[IP_MAX_LEN] = {0};

	if (NULL == pnet)
	{
		NET_ERR("NULL==pnet\n");
		return NULL;
	}
	while (1)
	{
		pnet->NET_GetIPAddressAdjustEnable(&ipAdjustEnable);
		pnet->NET_GetDhcpEnable();
		
		if (1 == bFirst)
		{
			CConfig *pCfg = CConfig::GetInstance();
			pCfg->GetValue(INet_CfgSection, "LocalIP", "192.168.1.11", localIP, sizeof(localIP));

			//获取新的配置文件读取实例
			int ret = 0;
			CConfig *pDefCfg = CConfig::NewInstance();
			if (NULL != pDefCfg)
			{
				ret = pDefCfg->LoadFile(IDVR_DEFAULT_CONFIG_FILE_PATH);
				if (0 == ret)
				{
					pDefCfg->GetValue(INet_CfgSection, "LocalIP", "192.168.1.11", defaultIP, sizeof(defaultIP));
				}
			}
			pDefCfg->ReleaseInstance(pDefCfg);

			bFirst = 0;
		}
		
		if(ipAdjustEnable)
		{
			tick++;
			
			//5分钟去检测下有没有连接
			if(!(tick%300))
			{
				connected = 0;
				pnet->NET_IfIPCConnected(&connected);
				//NET_INFO("tick %d...%s connect!\n", tick, connected == 0 ? "no" : "has");
				if(!connected)
				{
					tick = 0;
					sleep(1);
					continue;
				}
				//NET_INFO("time flow...start check connect\n");
			}
			if(tick >= 21600)//6小时21600
			{
				IPAdjustCloseFlag = 1;
				tick = 0;
			}
			//NET_INFO("tick...tick=%d, time=%d, connected=%d, IpChangeStatus=%d\n", tick, time(NULL), connected, IpChangeStatus);
			if(IpChangeStatus)//IP改变则清零
			{
				if(3 <= ++ipadjust_count)
				{
					ipadjust_count = 3;
				}
				IpChangeStatus = 0;
				tick = 0;
			}
		}
		else
		{
			tick = 0;
			ipadjust_count = 0;
		}
		sleep(1);				//一秒检测一次
	}
	return NULL;
}

static int check_protocol_url(int eP2pType,char* pDownloadURL, int DownloadURLSize)
{
	FILE *PROFP = NULL;
	char probuf[64]={0};
	int nRet = IDVR_OK;

    if(0 == access(PROTOCOL_FILE, F_OK))
	{
        //获取新的配置文件读取实例
        CConfig *pcfg = CConfig::NewInstance();

        if (0 == pcfg->LoadFileRO(PROTOCOL_FILE)) 
    	{        
			
    		if(NULL != pcfg->GetValue("P2P", "APPDOWNLOAD", "", probuf, sizeof(probuf)))
    		{
                if((PROFP = fopen(PROTOCOL_FILE,"r")) != NULL )
				{
					strncpy(pDownloadURL, probuf,DownloadURLSize - 1);
					fclose(PROFP);
				}
    		}
    	}
    	pcfg->ReleaseInstance(pcfg);
    	return 0;
	}
	else //定制文件读取失败
	{
		goto ERR;
	}
ERR:
	if ((0 == access(P2P_LIB_NAME_TUTK, 0)) && (access(P2P_LIB_NAME_SHYUN, 0) != 0)) //tutk
	{
		strncpy(pDownloadURL, "http://115.28.147.140/p2p_direct.html",DownloadURLSize - 1);
	}
	else if (0 == access(P2P_LIB_NAME_ULK, 0)) //ULK
	{
		strncpy(pDownloadURL, "http://www.zwcloud.wang/p2p_direct_anyan.html",DownloadURLSize - 1);
	}
	else if (0 == access(P2P_LIB_NAME_SHYUN, 0)) //shangyun	
	{
		strncpy(pDownloadURL, "http://www.zwcloud.wang/p2p_direct_p6s.html",DownloadURLSize - 1);						
	}

	return nRet;
}

int INet::checkIPAddr(const char *pszIPAddress)
{
	if (NULL == pszIPAddress)
	{
		NET_ERR("NULL == pszIPAddress\n");
		return -1;
	}
	//检查IP是否合法
	if (!IsValidIP(pszIPAddress))
	{
		NET_ERR("invalid ip =%s\n", pszIPAddress);
		return -1;
	}

    //检查IP地址最后一位是否是0或者255
    if (!IsValidIpAddr(pszIPAddress))
    {
		NET_ERR("invalid ip =%s\n", pszIPAddress);
		return -1;
    }

	if (!IsValidGW(pszIPAddress))
	{
		NET_ERR("invalid ip =%s\n", pszIPAddress);
		return -1;
	}

    int first = atoi(pszIPAddress);
    if((0 == first) || (127 == first) || (first > 223))
    {
        NET_ERR("ip first is invalid: %s\n", pszIPAddress);
        return -1;
    }

	return 0;
}

int INet::checkMask(const char *pszSubnetMask)
{
	if (NULL == pszSubnetMask)
	{
		NET_ERR("NULL == pszSubnetMask\n");
		return -1;
	}
	
	if (!IsValidIP(pszSubnetMask))
	{
		NET_ERR("invalid ip =%s\n", pszSubnetMask);
		return -1;
	}
	
	return 0;
}

//!\brief   恢复出厂网络配置,配合升级
//有无线功能的中间层同步时,对于无线的网络也需要恢复
void INet::Update_Recover()
{
    NET_DBG("\nflag exist so enter Update_Recover\n");

    //获取新的配置文件读取实例
	CConfig *pDefaultCfg = CConfig::NewInstance();

    char ipaddr[256];
    char netmask[256];
    char gateway[256];

    memset(ipaddr, 0, sizeof(ipaddr));
    memset(netmask, 0, sizeof(netmask));
    memset(gateway, 0, sizeof(gateway));

	//从缺省配置文件中读取
	int nRet = pDefaultCfg->LoadFileRO(IDVR_DEFAULT_CONFIG_FILE_PATH);

    if (0 == nRet)
	{
		int enable = 0;
        char command[128];

		enable = pDefaultCfg->GetValue("INet_Cfg", "DHCP_Enable", 0);
		NET_SetDhcpEnable(enable);

		enable = pDefaultCfg->GetValue("INet_Cfg", "IPAdjust_Enable", 0);
        NET_SetIPAddressAdjustEnable(enable);

		pDefaultCfg->GetValue("INet_Cfg", "LocalIP", "192.168.1.11", ipaddr, sizeof (ipaddr));
		NET_SetIPAddress(ipaddr);
		
		pDefaultCfg->GetValue("INet_Cfg", "LocalSubnetMask", "255.255.255.0", netmask, sizeof (netmask));
		NET_SetSubnetMask(netmask);

		pDefaultCfg->GetValue("INet_Cfg", "LocalGateway", "192.168.1.1", gateway, sizeof (gateway));
		NET_SetGateway(gateway);

        memset(command, 0, sizeof(command));
	    snprintf(command, sizeof(command), "cp -v %s %s", RESOLV_CONF_BAK_FILE_PATH, RESOLV_CONF_FILE_PATH_REAL);
	    mysystem(command); //恢复DNS
	}   
    
    CConfig::ReleaseInstance(pDefaultCfg);
}

int INet::LoadParam()
{
    int s32Ret;
	int getPort;
	char mac[MAX_MAC_STR_LENGTH] = {0};
	char ip[MAX_IP_STR_LEN] = {0};
    char gateway[MAX_IP_STR_LEN] = {0};
    char netmask[MAX_IP_STR_LEN] = {0};
	char lastSetValue[MAX_IP_STR_LEN] = {0};
	char buf[256] = {0};

    //申请did
	char *fpstart = NULL;
	char *fpend = NULL; 
	char UrlportStr[6] = {0};

	int mac0, mac1, mac2, mac3, mac4, mac5;
    char value[64] = {0};
	int eP2pType = 0;
    
    CConfig *pcfg = CConfig::NewInstance();	

    /* 从用户配置文件里面获取用户是否需要支持该功能 */
    memset(value, 0, sizeof(value));
    if (0 == GetIniSectionItem(CUSTOM_CONFIG, "ULK", "bSupportULK", value, sizeof(value)))
        bSupportULK = atoi(value);

    memset(value, 0, sizeof(value));
    if (0 == GetIniSectionItem(CUSTOM_CONFIG, "TUTK", "bSupportTUTK", value, sizeof(value)))
        bSupportTUTK = atoi(value);

    memset(value, 0, sizeof(value));
    if (0 == GetIniSectionItem(CUSTOM_CONFIG, "IPADJUST", "bSupportIPAdjust", value, sizeof(value)))
        bSupportIPAdjust = atoi(value);
        
    memset(value, 0, sizeof(value));
    if (0 == GetIniSectionItem(CUSTOM_CONFIG, "FTP", "bSupportFTP", value, sizeof(value)))
    {
    	bSupportFTP = atoi(value);
		
	}
#if defined (__IS_SUPPORT_WLAN__)
	memset(value, 0, sizeof(value));
    if (0 == GetIniSectionItem(CUSTOM_CONFIG, "WIFI", "bSupportWlanAP", value, sizeof(value)))
    {
    	bSupportWlanAP = atoi(value);
	}
#endif

	//能力定制
	if (0 == pcfg->LoadFileRO(CUSTOM_ABILITY_FILE))
	{
		bSupportPhy = pcfg->GetValue("FunctionSupport", "IsSupportWired", bSupportPhy);
        bSupportQrScan = pcfg->GetValue("FunctionSupport", "IsSupportQrScan", bSupportQrScan);
		bSupportOnlyCloudCard = pcfg->GetValue("FunctionSupport", "IsSupportOnlyCloudCard", bSupportOnlyCloudCard);
	}

#ifdef __MODULE_4G_SUPPORT__
    ZWSys_Get4GSupport(&bSupportGPRS);
    if (bSupportGPRS)
    {//使用4G时，清除wifi标识
        /* 删除wifi存在的标记文件 */
        SAFE_UNLINK(WIFI_EXIST_FlAG);
        bSupportWIFI = 0;
        bSupportWlanAP = 0;

        //wifi 开关关闭状态
        m_bWlan_Enable = 2;
    }
#endif

	NET_INFO("DeviceAbility:\n\
	bSupportULK=%d,\n\
	bSupportTUTK=%d,\n\
	bSupportIPAdjust=%d,\n\
	bSupportFTP=%d,\n\
	bSupportWlanAP=%d,\n\
	bSupportPhy=%d,\n\
	bSupportQrScan=%d,\n\
	bSupportGPRS=%d\n",
        bSupportULK,bSupportTUTK,bSupportIPAdjust, bSupportFTP, bSupportWlanAP, bSupportPhy, bSupportQrScan, bSupportGPRS);

#if defined (__IS_SUPPORT_WLAN__)
	//获取新的配置文件读取实例
	if ((0 == pcfg->LoadFileRO(CUSTOM_CONFIG_FILE)) && (pcfg->CheckSectionExist("ApModeCfg")))
    {
		memset(g_szDefaultApIp, 0, sizeof(g_szDefaultApIp));
		pcfg->GetValue("ApModeCfg", "ApModeIp", DEFAULT_AP_IP, g_szDefaultApIp, sizeof(g_szDefaultApIp));
		memset(g_szDefaultApNetmask, 0, sizeof(g_szDefaultApNetmask));
		pcfg->GetValue("ApModeCfg", "ApModeNetmask", DEFAULT_AP_NETMASK, g_szDefaultApNetmask, sizeof(g_szDefaultApNetmask));
		memset(g_szDefaultApGateway, 0, sizeof(g_szDefaultApGateway));
		pcfg->GetValue("ApModeCfg", "ApModeGateway", DEFAULT_AP_GATEWAY, g_szDefaultApGateway, sizeof(g_szDefaultApGateway));

		/* 导入AP模式下SSID的前缀 */
		memset(g_szDefaultApPrefix, 0, sizeof(g_szDefaultApPrefix));
		pcfg->GetValue("ApModeCfg", "ApSsidPrefix", DEFAULT_AP_PREFIX, g_szDefaultApPrefix, sizeof(g_szDefaultApPrefix));

		/* 导入AP模式下的密码 */
		memset(g_nSoftApPasswd, 0, sizeof(g_nSoftApPasswd));
		pcfg->GetValue("ApModeCfg", "ApModePassword", DEFAULT_SPFTAP_PWD, g_nSoftApPasswd, sizeof(g_nSoftApPasswd));

        /* 导入蓝牙广播的前缀 */
        memset(g_szBlePrefix, 0, sizeof(g_szBlePrefix));
        pcfg->GetValue("BleCfg", "BlePrefix", DEFAULT_BLE_PREFIX, g_szBlePrefix, sizeof(g_szBlePrefix));
    }
	else if (0 == pcfg->LoadFileRO(CUSTOM_INFO))
	{
		memset(g_szDefaultApIp, 0, sizeof(g_szDefaultApIp));
		pcfg->GetValue("ApModeCfg", "ApModeIp", DEFAULT_AP_IP, g_szDefaultApIp, sizeof(g_szDefaultApIp));
		memset(g_szDefaultApNetmask, 0, sizeof(g_szDefaultApNetmask));
		pcfg->GetValue("ApModeCfg", "ApModeNetmask", DEFAULT_AP_NETMASK, g_szDefaultApNetmask, sizeof(g_szDefaultApNetmask));
		memset(g_szDefaultApGateway, 0, sizeof(g_szDefaultApGateway));
		pcfg->GetValue("ApModeCfg", "ApModeGateway", DEFAULT_AP_GATEWAY, g_szDefaultApGateway, sizeof(g_szDefaultApGateway));
        /* 导入AP模式下SSID的前缀 */
        memset(g_szDefaultApPrefix, 0, sizeof(g_szDefaultApPrefix));
        pcfg->GetValue("ApModeCfg", "ApSsidPrefix", DEFAULT_AP_PREFIX, g_szDefaultApPrefix, sizeof(g_szDefaultApPrefix));
        
        /* 导入AP模式下的密码 */
        memset(g_nSoftApPasswd, 0, sizeof(g_nSoftApPasswd));
        pcfg->GetValue("ApModeCfg", "ApModePassword", DEFAULT_SPFTAP_PWD, g_nSoftApPasswd, sizeof(g_nSoftApPasswd));

        /* 导入蓝牙广播的前缀 */
        memset(g_szBlePrefix, 0, sizeof(g_szBlePrefix));
        pcfg->GetValue("BleCfg", "BlePrefix", DEFAULT_BLE_PREFIX, g_szBlePrefix, sizeof(g_szBlePrefix));
	}
	else
	{
        memset(g_szDefaultApIp, 0, sizeof(g_szDefaultApIp));
        snprintf(g_szDefaultApIp, sizeof(g_szDefaultApIp), "%s", DEFAULT_AP_IP); 

        memset(g_szDefaultApNetmask, 0, sizeof(g_szDefaultApNetmask));
        snprintf(g_szDefaultApNetmask, sizeof(g_szDefaultApNetmask), "%s", DEFAULT_AP_NETMASK); 
		
        memset(g_szDefaultApGateway, 0, sizeof(g_szDefaultApGateway));
        snprintf(g_szDefaultApGateway, sizeof(g_szDefaultApGateway), "%s", DEFAULT_AP_GATEWAY);  
        /* 导入AP模式下SSID的前缀 */
        memset(g_szDefaultApPrefix, 0, sizeof(g_szDefaultApPrefix));
        pcfg->GetValue("ApModeCfg", "ApSsidPrefix", DEFAULT_AP_PREFIX, g_szDefaultApPrefix, sizeof(g_szDefaultApPrefix));
        
        /* 导入AP模式下的密码 */
        memset(g_nSoftApPasswd, 0, sizeof(g_nSoftApPasswd));
        pcfg->GetValue("ApModeCfg", "ApModePassword", DEFAULT_SPFTAP_PWD, g_nSoftApPasswd, sizeof(g_nSoftApPasswd));
        /* 导入蓝牙广播的前缀 */
        memset(g_szBlePrefix, 0, sizeof(g_szBlePrefix));
        snprintf(g_szBlePrefix, sizeof(g_szBlePrefix), "%s", DEFAULT_BLE_PREFIX);
    }

	//更新udhcpd.conf配置文件
    char strUdhcpIPStart[64] = "223.223.100.2";
    char strUdhcpIPEnd[64] = "223.223.100.254";
    char strUdhcpOptRouter[64] = "223.223.100.1";
    int ipaddr[4] = {223,223,100,1};
    char command[256] = {0};
    sscanf(g_szDefaultApGateway, "%d.%d.%d.%d", &ipaddr[0], &ipaddr[1], &ipaddr[2], &ipaddr[3]);
    snprintf(strUdhcpIPStart, sizeof(strUdhcpIPStart), "%d.%d.%d.2", ipaddr[0], ipaddr[1], ipaddr[2]);
    snprintf(strUdhcpIPEnd, sizeof(strUdhcpIPEnd), "%d.%d.%d.254", ipaddr[0], ipaddr[1], ipaddr[2]);
    snprintf(strUdhcpOptRouter, sizeof(strUdhcpOptRouter), "%d.%d.%d.1", ipaddr[0], ipaddr[1], ipaddr[2]);
    snprintf(command, sizeof(command), "sed -i \"/^start /cstart %s\" %s", strUdhcpIPStart, UDHCPD_CONFIG_FILE);
    mysystem(command);
    snprintf(command, sizeof(command), "sed -i \"/^end /cend %s\" %s", strUdhcpIPEnd, UDHCPD_CONFIG_FILE);
    mysystem(command);
    snprintf(command, sizeof(command), "sed -i \"/^option subnet /coption subnet %s\" %s", g_szDefaultApNetmask, UDHCPD_CONFIG_FILE);
    mysystem(command);
    snprintf(command, sizeof(command), "sed -i \"/^opt router /copt router %s\" %s", strUdhcpOptRouter, UDHCPD_CONFIG_FILE);
    mysystem(command);

    ZWSys_GetWifiExist(&bSupportWIFI);
    if ((0 != bSupportWIFI) && (1 != bSupportWIFI))
    {
        bSupportWIFI = 0;
        bSupportWlanAP = 0;
        m_bWlan_Enable = 2;
    }
#endif

	TcpRelayEnable = m_pCConfig->GetValue(TcpRelay, TcpEnable, 0);
	if(TcpRelayEnable < 0)
	{
		TcpRelayEnable =0;
	}
#if defined (__IS_SUPPORT_WLAN__)
	TcpRelayEnable =0;//wifi设备默认关闭状态设置
#endif

	pcfg->ReleaseInstance(pcfg);

	//获取原有IP
	//NET_GetIPAddress(m_sLocalIP);
	//获取配置IP
	m_pCConfig->GetValue(INet_CfgSection, INet_IP_key, INet_IP_Default_s, ip, MAX_IP_STR_LEN);
	if (!IsValidIP(ip))
	{							//配置IP无效,取使用原有IP，如果原本就无IP，则使用缺省IP
		NET_ERR(" load cfg--  ip is invalid(%s)\n", ip);
		strncpy(ip, INet_IP_Default_s, sizeof(ip) - 1);
	}

    if (0 == access(CUSTOM_MODE_FLAG, F_OK))
    {
        saveIpBak(ip);
    }
    else
    {
    	//如果二IP不同，则以配置IP为主。
    	strncpy(buf, "ls /mnt/mtd/config/.Ip* | awk -F \":\" '{printf $2}'", sizeof(buf) - 1);
    	ShellCmdOut(buf, lastSetValue, sizeof(lastSetValue) - 1);
    	if(0 == checkIPAddr(lastSetValue))
    	{
    		if(0 != strcmp(ip, lastSetValue))
    		{
    			NET_INFO("Need reset ip addr: from %s to %s\n", ip, lastSetValue);
    			m_pCConfig->SetValue(INet_CfgSection, INet_IP_key, lastSetValue);
    			strncpy(ip, lastSetValue, sizeof(ip) - 1);
    		}
    	}
    	else
    	{
    		saveIpBak(ip);
    	}
   }

	NET_SetIPAddress(ip);

	//开启第二IP(暂时屏蔽掉，因为wifi情况下，容易出现ping不通外面的windows系统)
	//mysystem("ifconfig eth0:1 169.254.1.1");
    
	//获得子网掩码
	//NET_GetSubnetMask(m_sSubNetMask);

	m_pCConfig->GetValue(INet_CfgSection, INet_SubnetMask_key, INet_SubnetMask_Default_s, netmask, MAX_IP_STR_LEN);
	if (!IsValidIP(netmask))
	{
		NET_ERR(" load cfg--  subnetmask is invalid(%s)\n", netmask);
		strncpy(netmask, INet_SubnetMask_Default_s, sizeof(netmask) - 1);
	}

    if (0 == access(CUSTOM_MODE_FLAG, F_OK))
    {
        saveSubMaskBak(netmask);
    }
    else 
    {
    	//MASK校验
    	memset(lastSetValue, 0, sizeof(lastSetValue));
    	memset(buf, 0, sizeof(buf));
    	strncpy(buf, "ls /mnt/mtd/config/.MASK* | awk -F \":\" '{printf $2}'", sizeof(buf) - 1);
    	ShellCmdOut(buf, lastSetValue, sizeof(lastSetValue) - 1);
    	if(0 == checkMask(lastSetValue))
    	{
    		if(0 != strcmp(netmask, lastSetValue))
    		{
    			NET_INFO("Need reset mask addr: from %s to %s\n", netmask, lastSetValue);
    			m_pCConfig->SetValue(INet_CfgSection, INet_SubnetMask_key, lastSetValue);
    			strncpy(netmask, lastSetValue, sizeof(netmask) - 1);
    		}
    	}
    	else
    	{
    		saveSubMaskBak(netmask);
    	}
    }

	NET_SetSubnetMask(netmask);

	//获得网关
	//NET_GetGateway(m_sGateWay);

	m_pCConfig->GetValue(INet_CfgSection, INet_Gateway_key, INet_Gateway_Default_s, gateway, MAX_IP_STR_LEN);
	if (!IsValidIP(gateway))
	{
		NET_ERR("load cfg -- gateway is invalid(%s)\n", gateway);
		strncpy(gateway, INet_Gateway_Default_s,sizeof(gateway) - 1);
	}

	if (0 == access(CUSTOM_MODE_FLAG, F_OK))
    {
        saveGatewayBak(gateway);
    }
    else
    {
    	//GATEWAY校验
    	memset(lastSetValue, 0, sizeof(lastSetValue));
    	memset(buf, 0, sizeof(buf));
    	strncpy(buf, "ls /mnt/mtd/config/.GATEWAY* | awk -F \":\" '{printf $2}'", sizeof(buf) - 1);
    	ShellCmdOut(buf, lastSetValue, sizeof(lastSetValue) - 1);
    	if(IsValidGW(lastSetValue))
    	{
    		if(0 != strcmp(gateway, lastSetValue))
    		{
    			NET_INFO("Need reset gateway addr: from %s to %s\n", gateway, lastSetValue);
    			m_pCConfig->SetValue(INet_CfgSection, INet_Gateway_key, lastSetValue);
    			strncpy(gateway, lastSetValue, sizeof(gateway) - 1);
    		}
    	}
    	else
    	{
    		saveGatewayBak(gateway);
    	}
    }

	NET_SetGateway(gateway);
    
	HieServer_RefreshEvent(USEREVENT_ALARM_NOTICE, ALERT_IPCHANGED, 0, 0);
	HieServer_RefreshEventEx(USEREVENT_ALARM_NOTICE, 0, ALERT_IPCHANGED, 0, 0, NULL, 0);

	//获取MAC
	NET_GetMacAddress(mac);

	sscanf(mac, "%02x:%02x:%02x:%02x:%02x:%02x", &mac0, &mac1, &mac2, &mac3, &mac4, &mac5);

	m_sMAC_addr[0] = mac0;
	m_sMAC_addr[1] = mac1;
	m_sMAC_addr[2] = mac2;
	m_sMAC_addr[3] = mac3;
	m_sMAC_addr[4] = mac4;
	m_sMAC_addr[5] = mac5;
	NET_DBG("MAC_addr:%02x:%02x:%02x:%02x:%02x:%02x",
			*m_sMAC_addr, *(m_sMAC_addr + 1), *(m_sMAC_addr + 2), *(m_sMAC_addr + 3), *(m_sMAC_addr + 4), *(m_sMAC_addr + 5));

#ifdef ADD_ONVIF
	//设置HTTP端口
	getPort = Get_UID("/mnt/app/bin/boa");
	if (-1 != getPort)
	{							//已运行
		char command[256] = {0};
		char *pfind = NULL;
		unsigned int nport=80;
		int i = 0;
		int found = 0;
		FILE *file = NULL;
		char buff[1024] = {0};
		
		file = fopen(BOA_CONF_PATH, "r");
		if (file == NULL)
		{
			NET_ERR("open file failed [%s]\n", BOA_CONF_PATH);
			memset(command, 0, sizeof(command));
			snprintf(command, sizeof(command), "cp %s %s", BOA_CONF_BAK_PATH, BOA_CONF_PATH);
			mysystem(command);
			i = Get_UID("/mnt/app/bin/boa");
			if (-1 != i)
			{
				memset(command, 0, sizeof(command));
				snprintf(command, sizeof(command), "kill -9 %d", i);
				mysystem(command);
			}
			mysystem(BoaSeverProcStart);
			memset(command, 0, sizeof(command));
			snprintf(command, sizeof(command), "echo 80 > %s", HTTP_SERVER_CFG_NAME);
			mysystem(command);
			nport = 80;
			found = 1;
		}
		else
		{
			while (!feof(file))
			{
				memset(buff, 0, sizeof (buff));
				fgets(buff, sizeof (buff), file);
				if (buff[0] != '#')
				{
					if(NULL != (pfind = strstr(buff, PORT_KEY)))
					{
						sscanf(pfind, "Port %d", &nport);
						NET_DBG("get http port %d\n", nport);
						found = 1;
						break;
					}
				}
			}
			NET_INFO("########nport = %d\n",nport);
			fclose(file);

			if(!found)
			{
				//未找到端口
				memset(command, 0, sizeof(command));
				snprintf(command, sizeof(command), "cp %s %s", BOA_CONF_BAK_PATH, BOA_CONF_PATH);
				mysystem(command);
				i = Get_UID("/mnt/app/bin/boa");
				if (-1 != i)
				{
					snprintf(buff, sizeof(buff), "kill -9 %d", i);
					mysystem(buff);
				}
				mysystem(BoaSeverProcStart);
				memset(command, 0, sizeof(command));
				snprintf(command, sizeof(command), "echo 80 > %s", HTTP_SERVER_CFG_NAME);
				mysystem(command);
				nport = 80;
			}
		}
		m_uiHttpPort = nport;
	}
	else
	{							//未运行
		NET_INFO("http sever is stop,to start it!\n");
		char command[128];
		memset(command, 0, sizeof(command));
		snprintf(command, sizeof(command), "cp -v %s %s", BOA_CONF_BAK_PATH, BOA_CONF_PATH);//防止文件损坏无法登陆
		mysystem(command);
		mysystem(BoaSeverProcStart);
		
		memset(command, 0, sizeof(command));
		snprintf(command, sizeof(command), "echo 80 > %s", HTTP_SERVER_CFG_NAME);//防止文件损坏无法登陆
		mysystem(command);
		m_uiHttpPort = 80;
	}
#endif

	m_uiCommandPort = m_pCConfig->GetValue(INet_PortCfgSection, "CommandPort", 6060);
	m_uiMediaPort = m_pCConfig->GetValue(INet_PortCfgSection, "MediaPort", 6066);
	m_uiRtspPort = m_pCConfig->GetValue(INet_PortCfgSection, "RtpPort", 554);
	m_uiAutoDetectPort = m_pCConfig->GetValue(INet_PortCfgSection, "AutoDetectPort", 5555);
	m_uiMobilePort = m_pCConfig->GetValue(INet_PortCfgSection, "MobilePort", 10000);
	m_uiHKPrivatePort = m_pCConfig->GetValue(INet_PortCfgSection,"HKPrivatePort",8000);

	//load DHCP使能
	m_bDHCP_Enable = m_pCConfig->GetValue(INet_CfgSection, INet_DHCP_key, INet_DHCP_Default_i);
    if (0 == access(CUSTOM_MODE_FLAG, F_OK))
    {
        saveDhcpBak(m_bDHCP_Enable);    
    }
    else
    {
	    //DHCP校验
    	if ((access("/mnt/mtd/config/.DHCP:0", F_OK)==0) || (access("/mnt/mtd/config/.DHCP:1", F_OK)==0))
    	{
    		memset(lastSetValue, 0, sizeof(lastSetValue));
    		memset(buf, 0, sizeof(buf));
    		strncpy(buf, "ls /mnt/mtd/config/.DHCP* | awk -F \":\" '{printf $2}'", sizeof(buf) - 1);
    		ShellCmdOut(buf, lastSetValue, sizeof(lastSetValue) - 1);
    		int lastDHCP = 0;
    		sscanf(lastSetValue, "%d", &lastDHCP);
    		if((0 == lastDHCP) || (1 == lastDHCP))
    		{
    			if(m_bDHCP_Enable != lastDHCP)
    			{
    				NET_INFO("Need reset dhcp: from %d to %d\n", m_bDHCP_Enable, lastDHCP);
    				m_bDHCP_Enable = lastDHCP;
    				m_pCConfig->SetValue(INet_CfgSection, INet_DHCP_key, lastDHCP);
    			}
    		}
    		else
    		{
    			NET_ERR("Saved dhcp cfg error!\n");
    		}
    	}
    	else
    	{
    		saveDhcpBak(m_bDHCP_Enable);	
    	}
    }

	m_bIP_Adjust_Enbale = m_pCConfig->GetValue(INet_CfgSection, INet_IPAdjust_key, 1);
    if (0 == access(CUSTOM_MODE_FLAG, F_OK))
    {
        saveIPAdjustBak(m_bIP_Adjust_Enbale);   
    }
    else
    {
    	if ((access("/mnt/mtd/config/.IPADJUST:0", F_OK)==0) || (access("/mnt/mtd/config/.IPADJUST:1", F_OK)==0))
    	{
    		memset(lastSetValue, 0, sizeof(lastSetValue));
    		memset(buf, 0, sizeof(buf));
    		strncpy(buf, "ls /mnt/mtd/config/.IPADJUST* | awk -F \":\" '{printf $2}'", sizeof(buf) - 1);
    		ShellCmdOut(buf, lastSetValue, sizeof(lastSetValue) - 1);
    		int lastIpadjust = 0;
    		sscanf(lastSetValue, "%d", &lastIpadjust);
    		if((0 == lastIpadjust) || (1 == lastIpadjust))
    		{
    			if(m_bIP_Adjust_Enbale != lastIpadjust)
    			{
    				NET_INFO("Need reset IP Adjust: from %d to %d\n", m_bIP_Adjust_Enbale, lastIpadjust);
    				m_bIP_Adjust_Enbale = lastIpadjust;
    				m_pCConfig->SetValue(INet_CfgSection, INet_IPAdjust_key, m_bIP_Adjust_Enbale);
    			}
    		}
    		else
    		{
    			NET_ERR("Saved dhcp cfg error!\n");
    		}
    	}
    	else
    	{
    		saveIPAdjustBak(m_bIP_Adjust_Enbale);	
    	}
    }

	//报警动作
	m_bBeep[0] = m_pCConfig->GetValue(INet_CfgSection, "Net_Action_ip_beep", 0);
	m_bSTS[0] = m_pCConfig->GetValue(INet_CfgSection, "Net_Action_ip_sts", 0);
	m_bEmail[0] = m_pCConfig->GetValue(INet_CfgSection, "Net_Action_ip_email", 0);
	m_bSMS[0] = m_pCConfig->GetValue(INet_CfgSection, "Net_Action_ip_sms", 0);
	m_bDOP[0] = m_pCConfig->GetValue(INet_CfgSection, "Net_Action_ip_dop", 0);
	m_bGUI[0] = m_pCConfig->GetValue(INet_CfgSection, "Net_Action_ip_gui", 0);
	m_bPush[0] = m_pCConfig->GetValue(INet_CfgSection, "Net_Action_ip_push", 0);
	m_bLightAlarm[0] = m_pCConfig->GetValue(INet_CfgSection, "Net_Action_ip_lightalarm", 0);

	m_bBeep[1] = m_pCConfig->GetValue(INet_CfgSection, "Net_Action_phy_beep", 0);
	m_bSTS[1] = m_pCConfig->GetValue(INet_CfgSection, "Net_Action_phy_sts", 0);
	m_bEmail[1] = m_pCConfig->GetValue(INet_CfgSection, "Net_Action_phy_email", 0);
	m_bSMS[1] = m_pCConfig->GetValue(INet_CfgSection, "Net_Action_phy_sms", 0);
	m_bDOP[1] = m_pCConfig->GetValue(INet_CfgSection, "Net_Action_phy_dop", 0);
	m_bGUI[1] = m_pCConfig->GetValue(INet_CfgSection, "Net_Action_phy_gui", 0);
	m_bRecord[1] = m_pCConfig->GetValue(INet_CfgSection, "Net_Action_phy_record", 0);
	m_bPush[1] = m_pCConfig->GetValue(INet_CfgSection, "Net_Action_phy_push", 0);
	m_bLightAlarm[1] = m_pCConfig->GetValue(INet_CfgSection, "Net_Action_ip_lightalarm", 0);

	CConfig *pCfg = CConfig::NewInstance();
	s32Ret = pCfg->LoadFileRO(CUSTOM_INFO_FILE);
	if (0 == s32Ret)
	{
		bIPAdjustNoModifyGateway = pCfg->GetValue("Device", "IPAdjustNoModifyGateway", 0);
	}
	CConfig::ReleaseInstance(pCfg);

	FILE *file_new;

    int TutkUID_Enable = 0;
    FILE *fp2pena;
	int bNeedChgCfgFlag = FALSE;
    
	file_new = fopen(TutkUID_CONF, "r");
	if (file_new == NULL)
	{
		if (m_pCConfig->CheckKeyExist(INet_CfgSection, "p2pEnable"))
		{
			m_TutkUID_Enable = m_pCConfig->GetValue(INet_CfgSection, "p2pEnable", 0);
		}
		else
		{
			m_TutkUID_Enable = 1;
		}

		memset(m_sTutkUID, 0, sizeof(m_sTutkUID));
		NET_ERR("open file failed [%s]\n", TutkUID_CONF);
	}
	else
	{
		char fbuf[512] = { 0 };
		memset(m_sTutkUID, 0, sizeof(m_sTutkUID));
		fread(fbuf,sizeof(char), 512, file_new);
		sscanf(fbuf, "%d:%s", &m_TutkUID_Enable, m_sTutkUID);	//如果m_sTutkUID之前有数据，只会覆盖一部分，因此需要清零该字符
		fflush(file_new);
		fclose(file_new);

		//根据定制的来决定使能状态
		m_TutkUID_Enable = m_pCConfig->GetValue(INet_CfgSection, "p2pEnable", 0);
		bNeedChgCfgFlag = TRUE;
	}

    if (0 == access(P2PENA_SWITCH_FILE_PATH, F_OK))
    {
        fp2pena = fopen(P2PENA_SWITCH_FILE_PATH, "r");
        if(NULL != fp2pena)
        {
			char fbuf[512] = { 0 };
			fread(fbuf,sizeof(char), 512, fp2pena);
			sscanf(fbuf, "%d", &TutkUID_Enable);
    		fflush(fp2pena);
    		fclose(fp2pena);  
        }else
        {
            TutkUID_Enable = 1;
        }

        m_TutkUID_Enable = (m_TutkUID_Enable == TutkUID_Enable) ? m_TutkUID_Enable : TutkUID_Enable;
        mysystem("rm /mnt/mtd/config/P2PEnableSwitch.conf");
    }

#if 0
	if(access(P2P_LIB_NAME_TUTK, 0) != 0) //tutk so don't exist
	{
		m_TutkUID_Enable = 0;
	}
#endif

	//如果用户没设置则使用默认
	if (0 == strlen(m_sTutkUID))
	{
		memset(m_sTutkUID, 0, sizeof(m_sTutkUID));
		ZWSys_GetTUTKUID(m_sTutkUID);
		bNeedChgCfgFlag = TRUE;
	}
	m_iTutkStatus = 0;

	//判断当前DID是否支持SubKey
    if (strnlen(m_sTutkUID, sizeof(m_sTutkUID)-1) > 0)
    {	
#if defined (__USE_P2P_PUSH__)
        if (eZWP2P_PUSH_ERR_SUCCESS == ZWP2PPUSHVNClient_IsSupportSetSubkeyByDID(m_sTutkUID))
        {
            m_bIsSuportSubKey = 1;
        }
        else
        {
            m_bIsSuportSubKey = 0;
        }
#else
		m_bIsSuportSubKey = 0;
#endif
        NET_INFO("Current did %s is %s subkey!\n", m_sTutkUID, m_bIsSuportSubKey?"support":"not support");
    }

	//保存至配置文件
	if (access(TutkUID_CONF, 0) != 0 || bNeedChgCfgFlag)
	{
		file_new = fopen(TUTK_UID_FILE_PATH_BAK, "w");
	    if (NULL == file_new)
	    {
	        NET_ERR("open file failed [%s]\n", TUTK_UID_FILE_PATH_BAK);
	    }
		else
		{
			char buff[24+MAX_DID_LENGTH] = {0};
			snprintf(buff, sizeof(buff), "%d:%s", m_TutkUID_Enable, m_sTutkUID);
			fprintf(file_new, "%s", buff);
			fclose(file_new);
			
			snprintf(buff, sizeof(buff), "mv %s %s", TUTK_UID_FILE_PATH_BAK, TutkUID_CONF);
			mysystem(buff);
		}

        if (m_bIsSuportSubKey)
        {
            // 如果TUTK DID发生改变，则需要重新生成Subkey
            NET_WARN("Detect first need to general push sub key ...\n");
            bNeedGenSubKey = 1;
        }
	}
	else
	{
        if (m_bIsSuportSubKey)
        {        
            // 如果TUTK DID发生改变，则需要重新清除Subkey
            if (0 == access(SUBKEY_NEED_CLEAR_FLAG, F_OK))
            {
                NET_WARN("tutk has changed last boot! need to clear push sub key ...\n");
                bNeedClrSubKey = 1;
            }

            // 如果TUTK DID发生改变，则需要重新生成Subkey
            if (0 == access(SUBKEY_NEED_GENERAL_FLAG, F_OK))
            {
                NET_WARN("tutk has changed last boot! need to general push sub key ...\n");
                bNeedGenSubKey = 1;
            }
        }
	}

    if (m_bIsSuportSubKey)
    {
		m_pCConfig->GetValue(IPUSH_AI_CfgSection, IPUSH_AISubKey_Key, IPUSH_AISubKey_Default_s, m_szPushSubKey, sizeof(m_szPushSubKey));
    }

#if defined (__USE_APPLY_DID_MANUAL__)
    stuShmAuthInfo authinfo;
    Authorize_GetAuthDevInfo(&authinfo);

	memset(buf, 0, sizeof(buf));
	memcpy(buf, authinfo.struCloudInfo.szDid, MAX_DID_LEN);

	if (strlen(buf))
	{
		//比较，不一致就用分区的
		if (strncmp(buf, m_sTutkUID, MAX_UID))
		{
			//更新且保存到文件
			strncpy(m_sTutkUID, buf, MAX_UID - 1);
			file_new = fopen(TutkUID_CONF, "w");
			if (NULL != file_new)
			{
				memset(buf, 0, sizeof(buf));
				snprintf(buf, sizeof(buf), "%d:%s", m_TutkUID_Enable, m_sTutkUID);
				fprintf(file_new, "%s", buf);
				fclose(file_new);
			}
		}
	}
	else
	{
		if (strlen(m_sTutkUID))
		{
			//备份到分区中
            memcpy(authinfo.struCloudInfo.szDid, m_sTutkUID, MAX_DID_LEN);
            Authorize_SetAuthDevInfo(&authinfo);
		}
	}
#endif

	//根据定制文件中的app下载链接
	check_protocol_url(eP2pType, m_P2PDownloadURL,sizeof(m_P2PDownloadURL));
	
    if (bSupportULK)
    {
    	char buf[1024] = {0};
    	int  cur_pos = 0;
    	/* ULK配置文件保存 */
    	file_new = NULL;
    	memset(buf, 0, sizeof(buf));
    	cur_pos = 0;
    	file_new = fopen(ULKUID_CONF, "r");
    	if (file_new == NULL)
    	{
    		m_UlkUID_Enable = 1;
    		memset(m_sUlkUID, 0, sizeof(m_sUlkUID));
    		NET_ERR("open file failed [%s]\n", ULKUID_CONF);
    	}
    	else
    	{	
    		cur_pos = ftell(file_new);		  
    		fread(buf, sizeof(char), 24, file_new);
    		NET_INFO("%s:%s\n", ULKUID_CONF, buf);
    		fseek(file_new, cur_pos, SEEK_SET); 
    		char fbuf[512] = { 0 };
			fread(fbuf,sizeof(char), 512, file_new);
			sscanf(fbuf, "%d:%s", &cur_pos, m_sUlkUID);
    		// fscanf(file_new, "%d:%s", &cur_pos, m_sUlkUID);
    		m_UlkUID_Enable = cur_pos;					 /* 字节序的问题，必须这么做 */
    		NET_INFO("For fscanf m_UlkUID:%d, %s\n", m_UlkUID_Enable, m_sUlkUID);
    		fclose(file_new);
    	}
		if(access(P2P_LIB_NAME_ULK, 0) != 0) //ulk so don't exist
		{
			m_UlkUID_Enable = 0;
		}		
    	NET_INFO("m_UlkUID_Enable:%d\n", m_UlkUID_Enable);
    	m_iUlkStatus = 0;
    }	

#if defined (__USE_FTP__)
    if (bSupportFTP)
    {
        // 从配置文件获取相关配置信息
        m_FtpUsrInfo.ftpenable   = m_pCConfig->GetValue(IFTP_CfgSection, IFTP_Enable_key, 0);
        m_FtpUsrInfo.port        = m_pCConfig->GetValue(IFTP_CfgSection, IFTP_Port_key, IFTP_Port_Default_i);
        m_FtpUsrInfo.transt_type = (eFtpTransType)m_pCConfig->GetValue(IFTP_CfgSection, IFTP_Type_key, SEND_NONE);
#ifdef FTP_SEND_PIC_ONLY
		m_FtpUsrInfo.transt_type = SEND_PICTURE;
#endif
		m_FtpUsrInfo.nFtpMode    = m_pCConfig->GetValue(IFTP_CfgSection, IFTP_Mode_key,0);
		m_pCConfig->GetValue(IFTP_CfgSection, IFTP_Host_key, IFTP_Host_Default_s, m_FtpUsrInfo.host, sizeof(m_FtpUsrInfo.host));
		m_pCConfig->GetValue(IFTP_CfgSection, IFTP_User_key, IFTP_User_Default_s, m_FtpUsrInfo.user, sizeof(m_FtpUsrInfo.user));
		m_pCConfig->GetValue(IFTP_CfgSection, IFTP_Pass_key, IFTP_Pass_Default_s, m_FtpUsrInfo.pass, sizeof(m_FtpUsrInfo.pass));
		m_pCConfig->GetValue(IFTP_CfgSection, IFTP_RmPath_key, IFTP_RmPath_Default_s, m_FtpUsrInfo.rmPath, sizeof(m_FtpUsrInfo.rmPath));
    }
#endif

    if(0 == access(RECOVER_FILE,F_OK))
    {
        char fst[32];

        Update_Recover();
        
        memset(fst,0,sizeof(fst));
        snprintf(fst, sizeof (fst), "rm %s",RECOVER_FILE);
        mysystem(fst);
    }

#if defined (__USE_APPLY_DID_MANUAL__)
    //加载申请did的服务器地址，并获得端口
    if (0 == access(APPLYDIDSERVERINFOCFG, F_OK))
    {
        file_new = fopen(APPLYDIDSERVERINFOCFG, "r");
        if(NULL != file_new)
        {
    		fread(m_RequestUrl, sizeof(char), MAX_REQUEST_URL_LEN, file_new);
            fflush(file_new);
            fclose(file_new);

            if (strlen(m_RequestUrl) <= MIX_REQUEST_URL_LEN)
            {
                snprintf(m_RequestUrl, sizeof(m_RequestUrl), "%s", g_ApplyDidDefaultURL);
            }
            else if (('\n' == m_RequestUrl[strlen(m_RequestUrl) - 1]) || ('\r' == m_RequestUrl[strlen(m_RequestUrl) - 1]))
            {
                m_RequestUrl[strlen(m_RequestUrl) - 1] = '\0';
            }
        }
        else
        {
            snprintf(m_RequestUrl, sizeof(m_RequestUrl), "%s", g_ApplyDidDefaultURL);
        }
    }
    else
    {
        snprintf(m_RequestUrl, sizeof(m_RequestUrl), "%s", g_ApplyDidDefaultURL);
    }

	fpstart = strchr(m_RequestUrl, ':');	
	fpstart = strchr(fpstart+1, ':');	
	fpend = strchr(fpstart+1, '/');
	
    memcpy(UrlportStr, (fpstart + 1), (fpend - fpstart - 1));	
	
	m_RequestUrlPort = atoi(UrlportStr);
#endif

#ifdef __MODULE_4G_SUPPORT__
	m_pCConfig->GetValue(INet_CfgSection, "Appid", "E6bmvnsJgmLd4GpP", m_szAppid, 64);
	m_pCConfig->GetValue(INet_CfgSection, "Appkey", "oXT3v7zRoXCMNhFfAzqpygTWTaUFFd", m_szAppkey, 256);

	m_pCConfig->GetValue(INet_CfgSection, "NorthLatiAppid", "sjyis76adz1dy4dd", m_szNorthLatiAppid, 64);
	m_pCConfig->GetValue(INet_CfgSection, "NorthLatiAppkey", "yok2puhuy4yj5vhg", m_szNorthLatiAppkey, 256);
	m_NorthLatiCardType = 0;
#endif

	// 加载dns配置文件
	//DNS1
	m_pCConfig->GetValue(IPPPOE_CfgSection, IPPPOE_PrimaryDNS_key, DEFAULT_RESOLV_CONF, ip, 20);
	if (IsValidIP(ip))//IsValidIP() 将一个点分十进制的IP转换成一个长整数型数
	{
		strncpy(m_sPrimaryDNS, ip,sizeof(m_sPrimaryDNS)-1);
	}
	else
	{
		strncpy(m_sPrimaryDNS, DEFAULT_RESOLV_CONF,sizeof(m_sPrimaryDNS)-1);
	}

	//DNS2
	m_pCConfig->GetValue(IPPPOE_CfgSection, IPPPOE_SecondaryDNS_key, DEFAULT_RESOLV_CONF_LOCAL, ip, 20);
	if (IsValidIP(ip))
	{
		strncpy(m_sSecondaryDNS, ip,sizeof(m_sSecondaryDNS)-1);
	}
	else
	{
		strncpy(m_sSecondaryDNS, DEFAULT_RESOLV_CONF_LOCAL,sizeof(m_sSecondaryDNS)-1);
	}
	NET_INFO("loadparam [dnstest] init load maindns :%s  seconddns :%s \n", m_sPrimaryDNS, m_sSecondaryDNS);

	ParseDnsAddr();
	//永远成功
	return 0;
}

int INet::WLAN_LoadParam()
{
#if defined (__IS_SUPPORT_WLAN__)
	int i = 0;
	int nRet = 0;
    char mac[20] = {0};
    char ip[20]  = {0};
	char lastSetValue[20] = {0};
	char buf[256] = {0};
	char gateway[20] = {0};
    char netmask[20] = {0};
	char SsidBak[128] = {0};
	char WpaKeyBak[128] = {0};
    int mac0, mac1, mac2, mac3, mac4, mac5;
	char tmpwlan0dns[20]= {0};

    wifi_init(m_wifiMode);
	m_wifiCfgMode = eWifiCfgMode_Normal;

	m_bWlan_Enable = m_pCConfig->GetValue(INet_CfgSection, INET_WLANEnable_key, 1);
    wifi_set_enable(m_bWlan_Enable);
    m_bWlan_Enable = wifi_get_enable();

    //设置wifi状态灯
    if(SOFT_AP_MODE == m_wifiMode)
    {
    	m_nWifiLinkStatus = 0;	//标志等待配网
        ZWSys_SetWifiStatusLed(IN_AP_STATUS);
		return 0;
    }
    else
	{
		m_nWifiLinkStatus = 1;	//标志配网中；连接wifi中
		ZWSys_SetWifiStatusLed(IN_WIFI_CONNECTING);
		if (0 == m_wifiFakeSTAModeEnable)
		{
			WLAN_BluetoothClose();

			LoadStaModeDriver_8189();
		}
	}

    if (bSupportWlanAP)
    {
        m_wifiToStaFlag = m_pCConfig->GetValue(INet_CfgSection, INet_WLAN_AP_TO_STA_FLAG_key, 0);
    }
    else
    {
        m_wifiToStaFlag = 0;
    }

    //获取原有IP
    WLAN_GetIPAddress(m_sWlan_LocalIP);
	//获取配置IP
	m_pCConfig->GetValue(INet_CfgSection, INet_WLANIP_key, INet_WLANIP_Default_s, ip, 20);
	if (!IsValidIP(ip))
	{	//配置IP无效,取使用原有IP，如果原本就无IP，则使用缺省IP
		NET_ERR(" load cfg--  ip is invalid(%s)\n", ip);
		strncpy(ip, INet_WLANIP_Default_s, sizeof(ip) - 1);
	}

	//如果二IP不同，则以配置IP为主。
	memset(lastSetValue, 0, sizeof(lastSetValue));
	memset(buf, 0, sizeof(buf));
	strncpy(buf, "ls /mnt/mtd/config/.WlanIp* | awk -F \":\" '{printf $2}'", sizeof(buf) - 1);
	ShellCmdOut(buf, lastSetValue, sizeof(lastSetValue) - 1);
	if(0 == checkIPAddr(lastSetValue))
	{
		if(0 != strcmp(ip, lastSetValue))
		{
			NET_INFO("Need reset ip addr: from %s to %s\n", ip, lastSetValue);
			m_pCConfig->SetValue(INet_CfgSection, INet_WLANIP_key, lastSetValue);
			strncpy(ip, lastSetValue, sizeof(ip) - 1);
		}
	}
	else
	{
		saveWlanIpBak(ip);
	}
	WLAN_SetIPAddress(ip);

    //获得子网掩码
    WLAN_GetSubnetMask(m_sWlan_SubNetMask);

	m_pCConfig->GetValue(INet_CfgSection, INet_WLANSubnetMask_key, INet_SubnetMask_Default_s, netmask, 20);
	if (!IsValidIP(netmask))
	{
		NET_ERR(" load cfg--  subnetmask is invalid(%s)\n", netmask);
		strncpy(netmask, INet_SubnetMask_Default_s, sizeof(netmask) - 1);
	}
	//MASK校验
	memset(lastSetValue, 0, sizeof(lastSetValue));
	memset(buf, 0, sizeof(buf));
	strncpy(buf, "ls /mnt/mtd/config/.WlanMASK* | awk -F \":\" '{printf $2}'", sizeof(buf) - 1);
	ShellCmdOut(buf, lastSetValue, sizeof(lastSetValue) - 1);
	if (0 == checkMask(lastSetValue))
	{
		if (0 != strcmp(netmask, lastSetValue))
		{
			NET_INFO("Need reset mask addr: from %s to %s\n", netmask, lastSetValue);
			m_pCConfig->SetValue(INet_CfgSection, INet_WLANSubnetMask_key, lastSetValue);
			strncpy(netmask, lastSetValue, sizeof(netmask) - 1);
		}
	}
	else
	{
		saveWlanSubMaskBak(netmask);
	}

	WLAN_SetSubnetMask(netmask);
    //获得网关
    WLAN_GetGateway(m_sWlan_GateWay, sizeof(m_sWlan_GateWay));
	m_pCConfig->GetValue(INet_CfgSection, INet_WLANGateway_key, INet_Gateway_Default_s, gateway, 20);
	if (!IsValidIP(gateway))
	{
		NET_ERR("load cfg -- gateway is invalid(%s)\n", gateway);
		strncpy(gateway, INet_Gateway_Default_s, sizeof(gateway) - 1);
	}
	memset(lastSetValue, 0, sizeof(lastSetValue));
	memset(buf, 0, sizeof(buf));
	strncpy(buf, "ls /mnt/mtd/config/.WlanGateWay* | awk -F \":\" '{printf $2}'", sizeof(buf) - 1);
	ShellCmdOut(buf, lastSetValue, sizeof(lastSetValue) - 1);
	if (IsValidGW(lastSetValue))
	{
		if (0 != strcmp(gateway, lastSetValue))
		{
			NET_INFO("Need reset mask addr: from %s to %s\n", gateway, lastSetValue);
			m_pCConfig->SetValue(INet_CfgSection, INet_WLANGateway_key, lastSetValue);
			strncpy(gateway, lastSetValue, sizeof(gateway) - 1);
		}
	}
	else
	{
		saveWlanGateWayBak(gateway);
	}
	
	WLAN_SetGateway(gateway);
	//获取DNS
	m_pCConfig->GetValue(IPPPOE_CfgSection, INet_WLANDNS0_key, DEFAULT_RESOLV_CONF, tmpwlan0dns, 20);
	if (IsValidIP(tmpwlan0dns))//IsValidIP() 将一个点分十进制的IP转换成一个长整数型数
	{
		strncpy(m_sWlanPrimaryDNS, tmpwlan0dns,sizeof(m_sWlanPrimaryDNS)-1);
	}
	else
	{
		strncpy(m_sWlanPrimaryDNS, DEFAULT_RESOLV_CONF,sizeof(m_sWlanPrimaryDNS)-1);
	}

	memset(tmpwlan0dns, 0, sizeof(tmpwlan0dns));
    m_pCConfig->GetValue(IPPPOE_CfgSection, INet_WLANDNS1_key, DEFAULT_RESOLV_CONF_LOCAL, tmpwlan0dns, 20);
	if (IsValidIP(tmpwlan0dns))//IsValidIP() 将一个点分十进制的IP转换成一个长整数型数
	{
        snprintf(m_sWlanSecondaryDNS, sizeof(m_sWlanSecondaryDNS), "%s", tmpwlan0dns);
	}
	else
	{
        snprintf(m_sWlanSecondaryDNS, sizeof(m_sWlanSecondaryDNS), "%s", DEFAULT_RESOLV_CONF_LOCAL);
	}
	NET_INFO("[dnstest] init wlanloadparam m_sWlanPrimaryDNS : %s m_sWlanSecondaryDNS: %s \n", m_sWlanPrimaryDNS, m_sWlanSecondaryDNS);

	//获取MAC
	WLAN_GetMacAddress(mac);
	sscanf(mac, "%02x:%02x:%02x:%02x:%02x:%02x", &mac0, &mac1, &mac2, &mac3, &mac4, &mac5);

	m_sWlan_MAC_addr[0] = mac0;
	m_sWlan_MAC_addr[1] = mac1;
	m_sWlan_MAC_addr[2] = mac2;
	m_sWlan_MAC_addr[3] = mac3;
	m_sWlan_MAC_addr[4] = mac4;
	m_sWlan_MAC_addr[5] = mac5;
	NET_DBG("MAC_addr:%02x:%02x:%02x:%02x:%02x:%02x",
			*m_sWlan_MAC_addr, *(m_sWlan_MAC_addr + 1), *(m_sWlan_MAC_addr + 2), *(m_sWlan_MAC_addr + 3), *(m_sWlan_MAC_addr + 4),
			*(m_sWlan_MAC_addr + 5));
    //load DHCP使能(无线网卡WLAN0)
    m_bWlan_DHCP_Enable = m_pCConfig->GetValue(INet_CfgSection, INet_WLANDHCP_key, INet_DHCP_Default_i);
	//DHCP校验
	if ((access("/mnt/mtd/config/.WlanDHCP:0", F_OK)==0) || (access("/mnt/mtd/config/.WlanDHCP:1", F_OK)==0))
	{
		memset(lastSetValue, 0, sizeof(lastSetValue));
		memset(buf, 0, sizeof(buf));
		strncpy(buf, "ls /mnt/mtd/config/.WlanDHCP* | awk -F \":\" '{printf $2}'", sizeof(buf) - 1);
		ShellCmdOut(buf, lastSetValue, sizeof(lastSetValue) - 1);
		int lastDHCP = 0;
		sscanf(lastSetValue, "%d", &lastDHCP);
		if((0 == lastDHCP) || (1 == lastDHCP))
		{
			if(m_bWlan_DHCP_Enable != lastDHCP)
			{
				NET_INFO("Need reset dhcp: from %d to %d\n", m_bWlan_DHCP_Enable, lastDHCP);
				m_bWlan_DHCP_Enable = lastDHCP;
				m_pCConfig->SetValue(INet_CfgSection, INet_WLANDHCP_key, lastDHCP);
			}
		}
		else
		{
			NET_ERR("Saved dhcp cfg error!\n");
		}
	}
	else
	{
		saveWlanDhcpBak(m_bWlan_DHCP_Enable);	
	}

	//AP模式下如果不存在SD卡配网文件
	//则到这里就可以返回成功了
	if ((SOFT_AP_MODE == m_wifiMode) && (0 != access(WIFI_SD_PATH, F_OK)))
    {
		return 0;
    }
	WLAN_ReadConnCfg(m_pCConfig);
	int encodeEnable = 0;
	encodeEnable = m_pCConfig->GetValue(INet_CfgSection, INet_WLAN_KEY_ENCODE_ENABLE, 0);
	if(IsFileExsit(WIFI_CFG_INI))
	{
		//获得SSID
		GetWlanSSIDBak(SsidBak);
		if(SsidBak[0] != '\0')
		{
			strncpy(m_tWlan_Connection.szSSID,SsidBak, sizeof(m_tWlan_Connection.szSSID) - 1);
			m_pCConfig->SetValue(INet_CfgSection, INet_WLANSSID_key, m_tWlan_Connection.szSSID);
		}
		else
		{
			saveWlanSSIDBak(WIFI_CFG_INI, m_tWlan_Connection.szSSID);
		}
		//获得是否需要加密
		int EnEcode = 0;
		GetWlanEncodeEnableBak(&EnEcode);
		GetWlanWpaKeyBak(WpaKeyBak, EnEcode);
		m_pCConfig->SetValue(INet_CfgSection, INet_WLAN_KEY_ENCODE_ENABLE, EnEcode);
		if(WpaKeyBak[0] != '\0')
		{
			strncpy(m_tWlan_Connection.struKeyCfg.WPAKey.szKey, WpaKeyBak, sizeof(m_tWlan_Connection.struKeyCfg.WPAKey.szKey) - 1);
			WLAN_SaveWifiKey(m_tWlan_Connection.struKeyCfg.WPAKey.szKey);
		}
		else
		{
			saveWlanWPAKeyBak(WIFI_CFG_INI, encodeEnable, m_tWlan_Connection.struKeyCfg.WPAKey.szKey);
		}
	}
	else
	{
		saveWlanSSIDBak(WIFI_CFG_INI, m_tWlan_Connection.szSSID);
		saveWlanKeyEncodeBak(WIFI_CFG_INI, encodeEnable);
		saveWlanWPAKeyBak(WIFI_CFG_INI, encodeEnable, m_tWlan_Connection.struKeyCfg.WPAKey.szKey);
	}

	//假STA模式不需要也不能做下面的处理
	//否则会影响后续配网
	//主要是一些标志的处理m_bIsConfigureNetIng
	if (1 == m_wifiFakeSTAModeEnable)
	{
		return 0;
	}

    WLAN_ReadConnCfgFromSD();
    
    /* 暂存之前的值，用作判断比较 */
	int nAuthMode = m_tWlan_Connection.nAuthMode;      // 暂存之前的加密模式
	int nEncrypType = m_tWlan_Connection.nEncrypType; // 暂存之前的验证模式
	if(strlen(m_tWlan_Connection.szSSID))
	{
		//这里存在问题，刚启动时只获取一次不一定能获取到
		//这里获取多次，都失败，则直接使用最大概率的加密配置方式
		for (i = 0; i < 5; i++)
		{
			nRet = wifi_get_conn_info_wpa(1, m_tWlan_Connection.szSSID, &m_tWlan_Connection);
			if (0 == nRet)
			{
			    /* 如果加密模式和验证模式与后面获取的有差异，则需要更新相关配置和状态 */
                int bNeedSaveCfg = 0;
			    if (nAuthMode != m_tWlan_Connection.nAuthMode)
			    {
                    bNeedSaveCfg = 1;
                    m_pCConfig->SetValue(INet_CfgSection, INet_WLANAuthMode_key, m_tWlan_Connection.nAuthMode);
			    }
                if (nEncrypType != m_tWlan_Connection.nEncrypType)
                {
                    bNeedSaveCfg = 1;
                    m_pCConfig->SetValue(INet_CfgSection, INet_WLANEncType_key, m_tWlan_Connection.nEncrypType);
                }
                if (bNeedSaveCfg)
                {
                    NET_INFO("Wifi need change authmode or encryptypt and save it!\n");
                    if (2==m_tWlan_Connection.nAuthMode || 3==m_tWlan_Connection.nAuthMode)
                    {
                        m_tWlan_Connection.struKeyCfg.WPAKey.bValid = 1;
                    }
                    m_pCConfig->SaveFile();
                }
				break;
			}
			SelectSleepUSec(50000);
		}
	}
	m_bIsConfigureNetIng = 1;
	NET_INFO("Enter WLAN_LoadParam: struKeyCfg.bValid=%d, struKeyCfg.WPAKey.bValid=%d\n",
		m_tWlan_Connection.struKeyCfg.bValid, m_tWlan_Connection.struKeyCfg.WPAKey.bValid);
    wifi_set_conn_cfg(&m_tWlan_Connection);

    if (m_wifiToStaFlag)
    {
        WLAN_setFromApToStaConnectionCfgFlag(1);//连接配置完后开始超时计数
    }
#endif
    //永远成功
    return 0;
}

/* 启用arping命令，防止XP不能自动重连 */
int INet::NET_arp_update_neighbors()
{
	if (0 == bSupportPhy)
	{
		NET_INFO("not support eth0\n");
		return 0;
	}

	char command[256];
	memset(command, 0, sizeof(command));
	snprintf(command, sizeof(command), "arping -U -I eth0 -s %s -w 1 %s &", m_sLocalIP, m_sGateWay);
	mysystem(command);
	NET_DBG("%s\n", command);

	return 0;
}

int INet::NET_CheckStatus()
{
	int ret = 0;
	int status = 0, saveStatus;

	char m_sLocalIP_new[MAX_IP_STR_LEN];
	int i;
	static int IpConflictCnt = 0;
#if defined (__IS_SUPPORT_WLAN__)
    static int LanBreakCnt = 0;
#endif

	if (m_bCheck_exit)
	{
		NET_DBG("stop check status \n");
		return -1;
	}

#if defined (__IS_SUPPORT_WLAN__) || defined (__MODULE_4G_SUPPORT__)
	// 根据不同的网卡使用不同的编码参数
	if (1 == m_nCurrNetCardType) //有线
	{
		ZWSys_SetCurrNetCardType(eXRS_NETCARD_WIRED_1);
	}
	else if (2 == m_nCurrNetCardType) //4G
	{
		ZWSys_SetCurrNetCardType(eXRS_NETCARD_4G);
	}
	else if (3 == m_nCurrNetCardType) //WIFI
	{
		ZWSys_SetCurrNetCardType(eXRS_NETCARD_WIFI);
	}
	else //其他
	{
		ZWSys_SetCurrNetCardType(eXRS_NETCARD_WIRED_Unknown);
	}
#endif

#ifdef __MODULE_4G_SUPPORT__
    if (bSupportGPRS)
    {
    	if ((0 == m_nProductState) && (BINDING_MODE == m_nSimCardBindState) && (0 == m_nAllowUseEth0))
    	{
    		//不是产测模式，并且是运营模式，不做有线检测
    		return 0;
    	}
    }
#endif

    if (bSupportIPAdjust)
    {
    	if(IPAdjustCloseFlag && m_bIP_Adjust_Enbale)
    	{
    		IPAdjustCloseFlag = 0;
    		m_bIP_Adjust_Enbale = 0;
			m_pCConfig->SetValue(INet_CfgSection, INet_IPAdjust_key, m_bIP_Adjust_Enbale);
			saveIPAdjustBak(m_bIP_Adjust_Enbale);
			//连接达到6小时关闭自适应时也关闭DHCP并保存配置,保存当前IP地址
			if(m_bDHCP_Enable)
			{
				m_bDHCP_Enable = 0;
				//写配置
				if (m_pCConfig->SetValue(INet_CfgSection, INet_DHCP_key, m_bDHCP_Enable) < 0)
				{
					NET_ERR("SetValue failed\n");
				}
				saveDhcpBak(m_bDHCP_Enable);
			}
			char pszIPAddress[MAX_IP_STR_LEN];
			ret = get_if_address((char *) IFNAME, (char *) pszIPAddress, sizeof(pszIPAddress), GET_ADDRESS);
			if (ret)
			{
				*pszIPAddress = '\0';
				NET_ERR("Get ipaddress failed (%d) \n", ret);
				return 0;
			}
			strncpy(m_sLocalIP, pszIPAddress, sizeof(m_sLocalIP) - 1);
			m_pCConfig->SetValue(INet_CfgSection, INet_IP_key, m_sLocalIP);
			saveIpBak(m_sLocalIP);

    		NET_DBG("IPC has connected 12h, close IP Adjust and DHCP!\n");
			m_pCConfig->SaveFile();
    	}
    }

	//1.先检查物理是否断开
	status = GetNetlinkStatus(IFNAME);
	g_ethLinkStatus = status;

	status = !status;
	//bit0-[0/1]物理断/开,bit1-[0/1]IP更新,bit2-[0/1]ip冲突,bit3-[0/1]IP获取失败

	if (status & 1)
	{	
		//断开了,开始回调
		goto do_callback;
	}
	//2.检测是否有IP更新
	if (NULL != NET_GetIPAddress(m_sLocalIP_new, sizeof(m_sLocalIP_new)))
	{
		//不相等，即有新IP.
		if (strncmp(m_sLocalIP_new, m_sLocalIP_old, MAX_IP_STR_LEN) != 0)
		{		    
			status |= (1 << 1);
			NET_DBG(" new ip = %s\n", m_sLocalIP_new);
			IpChangeStatus = 1;
			strncpy(m_sLocalIP_old, m_sLocalIP_new, MAX_IP_STR_LEN - 1);
			
			IpConflictCnt = 0;
			goto do_callback;
		}
	}
	else
	{
		//获取IP失败
		NET_INFO("NET_GetIPAddress failed\n");
		status |= (1 << 3);
		goto do_callback;
	}
	//3. IP冲突

	//在网络连接的情况下,检测IP是否冲突
	//  NET_DBG(" check ip conflict status=%d\n", status);

	ret = IsIpConflict(0);
	if (ret > 0)
	{
		status |= (1 << 2);		//IP冲突
		IpConflictCnt = 5;
	}
	else
	{							//5次检测到无冲突后才认为已经正常
		IpConflictCnt--;
		if (IpConflictCnt < 0)
		{
			IpConflictCnt = 0;
		}
		else
		{
			status |= (1 << 2);	//IP冲突
		}
	}

do_callback:
	saveStatus = (status & 1);  // 0表示连接上，1表示断开
	if (status & 1)
	{							//网线断
		NET_DBG("NET_CheckStatus: reticle is broken!\n");
#if defined (__IS_SUPPORT_WLAN__)
		if (bSupportWIFI && m_wifiFakeSTAModeEnable && m_bWlan_Enable && (WLAN_STA_MODE == m_wifiMode))
		{
            NET_INFO("Check lan status has break last %d second(s)\n", LanBreakCnt+1);
		    /* 累计检测到5次网线断开，则表明网线真的断开 */
            if (++LanBreakCnt >= 5)
		    {
        	    LanBreakCnt = 0;
        	    /* 如果是上层准备修改wifi配置或者配置设置完等待wifi连接过程中都不响应拔网线重启 */
    			if((m_wifiConnChange) || (m_changeWifiConnectFlag))
    			{
    				NET_INFO("m_wifiConnChange = %d, m_changeWifiConnectFlag = %d!\n", m_wifiConnChange, m_changeWifiConnectFlag);
    			}
    			else
    			{
					//这里需要切换为AP模式并提示配网
					//不再进行重启
					m_wifiMode = SOFT_AP_MODE;
					m_pCConfig->SetValue(INet_CfgSection, INet_WLANWifiMode, m_wifiMode);
					saveWlanWifiModeBak(m_wifiMode);
					/* 更新路由 */
					LoadSoftApDriver_8189();   /* 切AP模式 */

					WLAN_BluetoothOpen();
					
					m_wifiFakeSTAModeEnable = 0;
					m_pCConfig->SetValue(INet_CfgSection, INet_WLANFakeSTAMode, m_wifiFakeSTAModeEnable);
					m_pCConfig->SaveFileReal();

					if (bSupportQrScan)
            		{
                        SetQRScanStatus(1); //同时开启二维码扫描
					}
                    //提示进行配网
                    ZWSys_SetSpeechContent(SPEECH_WIFI_PLEASE_CONNECT);
                    //循环查询，等待播放结束
                    i = 0;
                    while (1)
                    {
                        sleep(1);
                        if (0 == ZWSys_GetSpeechPlayingStatus())
                        {
                            break;
                        }
                        if (++i > SPEECH_PLAY_TIME_OUT_SEC)
                        {
                            i = 0;
                            break;
                        }
                    }
    			}
			}
		}
#endif

		if (NULL != m_FxnCallBack_PHY)
		{
			struct tagAlertInfo pAlertInfo;

			struct tagActionInfo pActionInfo;
			pAlertInfo.AlertType = m_PHYAlarmType;
			pAlertInfo.nChannel = 0;
			pAlertInfo.dwReserved = 0;

			pActionInfo.dwReserved = 0;
			pActionInfo.nChannel = 0;
			//     if (m_bGUI[1])
			if((0 != NET_GetStatus()))
			{
				pActionInfo.tActionType = ACTION_GUI;
				m_FxnCallBack_PHY(&pAlertInfo, &pActionInfo);
			}

			if (m_bBeep[1])
			{
				pActionInfo.tActionType = ACTION_BEEP;
				m_FxnCallBack_PHY(&pAlertInfo, &pActionInfo);
			}
			if (m_bLightAlarm[1])
			{
				pActionInfo.tActionType = ACTION_LIGHTALARM;
				m_FxnCallBack_PHY(&pAlertInfo, &pActionInfo);
			}
			if (m_bSTS[1])
			{
				pActionInfo.tActionType = ACTION_SENDTOSERVER;
				m_FxnCallBack_PHY(&pAlertInfo, &pActionInfo);
			}
			if (m_bEmail[1])
			{
				pActionInfo.tActionType = ACTION_EMAIL;
				m_FxnCallBack_PHY(&pAlertInfo, &pActionInfo);
			}
			if (m_bSMS[1])
			{
				pActionInfo.tActionType = ACTION_SMS;
				m_FxnCallBack_PHY(&pAlertInfo, &pActionInfo);
			}
			if (m_bDOP[1])
			{
				pActionInfo.tActionType = ACTION_ALARMOUT;
				for (i = 0; i < 8; i++)
				{
					if (m_bDOP[1] & (1 << i))
					{
						pActionInfo.nChannel = i;
						m_FxnCallBack_PHY(&pAlertInfo, &pActionInfo);
					}
				}
			}

			if (m_bRecord[1])
			{
				pActionInfo.tActionType = ACTION_RECORD;
				for (i = 0; i < 16; i++)
				{
					if (m_bRecord[1] & (1 << i))
					{
						pActionInfo.nChannel = i;
						m_FxnCallBack_PHY(&pAlertInfo, &pActionInfo);
					}
				}
			}
		}

		NET_RenewDhcp();
	}
	else if (m_iNetStatus == 1 || m_iNetStatus == 2)
	{							//通，且上次为断
#if defined (__IS_SUPPORT_WLAN__)
    	if (bSupportWIFI)
        {
    		if (SOFT_AP_MODE == m_wifiMode)
    		{
    			//关闭二维码扫描
    			if (bSupportQrScan)
                {
                    SetQRScanStatus(0);
                }
    			//转成STA模式
    			m_wifiMode = WLAN_STA_MODE;
    			m_pCConfig->SetValue(INet_CfgSection, INet_WLANWifiMode, WLAN_STA_MODE);
    			saveWlanWifiModeBak(m_wifiMode);

    			//建立假STA标志
    			m_wifiFakeSTAModeEnable = 1;
    			m_pCConfig->SetValue(INet_CfgSection, INet_WLANFakeSTAMode, 1);
    			m_pCConfig->SaveFileReal();

    			WLAN_BluetoothClose();
				
    			LoadStaModeDriver_8189();
    		}
    	}
#endif
        NET_INFO("ETH0 SPEECH_NET_CONNECT_SUCCESS\n");
        ZWSys_SetSpeechContent(SPEECH_NET_CONNECT_SUCCESS);
        //循环查询，等待播放结束
        i = 0;
        while (1)
        {
            sleep(1);
            if (0 == ZWSys_GetSpeechPlayingStatus())
            {
                break;
            }
            if (++i > SPEECH_PLAY_TIME_OUT_SEC)
            {
                i = 0;
                break;
            }
        }

		if(NULL != m_FxnCallBack_PHY)
		{
			struct tagAlertInfo pAlertInfo;
			struct tagActionInfo pActionInfo;
			pAlertInfo.AlertType = m_PHYAlarmType;
			pAlertInfo.nChannel = 0;
			pAlertInfo.dwReserved = 1;

			pActionInfo.dwReserved = 0;
			pActionInfo.nChannel = 0;
			//     if (m_bGUI[1])
			if((0 != NET_GetStatus())) 
			{
				pActionInfo.tActionType = ACTION_GUI;
				m_FxnCallBack_PHY(&pAlertInfo, &pActionInfo);
			}
			//推送上一次出现过网络断
			if (m_bPush[1])
			{
				pActionInfo.tActionType = ACTION_PUSH;
				m_FxnCallBack_PHY(&pAlertInfo, &pActionInfo);
			}
		}

		NET_arp_update_neighbors();
	}
	//IP回调
	if (status & 2)
	{							//IP有更新
		NET_DBG("NET_CheckStatus: ip is update!\n");
		saveStatus = 4;
		if (NULL != m_FxnCallBack_IP)
		{
			struct tagAlertInfo pAlertInfo;

			struct tagActionInfo pActionInfo;
			//NET_INFO("my add: m_IPAlarmType=%d\n", m_IPAlarmType);
			pAlertInfo.AlertType = m_IPAlarmType;
			pAlertInfo.nChannel = 0;
			pAlertInfo.dwReserved = 1;

			pActionInfo.dwReserved = 0;
			pActionInfo.nChannel = 0;
			//   if (m_bGUI[0])
			{
				pActionInfo.tActionType = ACTION_GUI;
				m_FxnCallBack_IP(&pAlertInfo, &pActionInfo);
			}
			if (m_bPush[0])
			{
				pActionInfo.tActionType = ACTION_PUSH;
				m_FxnCallBack_IP(&pAlertInfo, &pActionInfo);
			}

            if (!g_DHCPEvent || !m_bDHCP_Enable)
            {
                pAlertInfo.AlertType = ALERT_TYPE_IPCHANGED;
                pActionInfo.tActionType = ACTION_GUI;
                m_FxnCallBack_IP(&pAlertInfo, &pActionInfo);
            }
            g_DHCPEvent = 0;
		}
	}
	if (status & 4)
	{							//IP有冲突
		NET_DBG("NET_CheckStatus: ip conflict!\n");
		saveStatus = 3;
		if (NULL != m_FxnCallBack_IP)
		{
			struct tagAlertInfo pAlertInfo;

			struct tagActionInfo pActionInfo;

			pAlertInfo.AlertType = m_IPAlarmType;
			pAlertInfo.nChannel = 0;
			pAlertInfo.dwReserved = 0;

			pActionInfo.dwReserved = 0;
			pActionInfo.nChannel = 0;

			pActionInfo.dwReserved = (unsigned long) m_sOtherMAC_addr;

			//   if (m_bGUI[0])
			{
				NET_DBG("my add:------m_bGUI[0]=1-------\n");
				pActionInfo.tActionType = ACTION_GUI;
				m_FxnCallBack_IP(&pAlertInfo, &pActionInfo);
			}

			if (m_bBeep[0])
			{
				NET_DBG("my add:------m_bBeep[0]=1-------\n");
				pActionInfo.tActionType = ACTION_BEEP;
				m_FxnCallBack_IP(&pAlertInfo, &pActionInfo);
			}
			if (m_bLightAlarm[0])
			{
				NET_DBG("my add:------m_bLightAlarm[0]=1-------\n");
				pActionInfo.tActionType = ACTION_LIGHTALARM;
				m_FxnCallBack_IP(&pAlertInfo, &pActionInfo);
			}
			if (m_bSTS[0])
			{
				NET_DBG("my add:------m_bSTS[0]=1-------\n");
				pActionInfo.tActionType = ACTION_SENDTOSERVER;
				m_FxnCallBack_IP(&pAlertInfo, &pActionInfo);
			}
			if (m_bEmail[0])
			{
				NET_DBG("my add:------m_bEmail[0]=1-------\n");
				pActionInfo.tActionType = ACTION_EMAIL;
				m_FxnCallBack_IP(&pAlertInfo, &pActionInfo);
			}
			if (m_bSMS[0])
			{
				NET_DBG("my add:------m_bSMS[0]=1-------\n");
				pActionInfo.tActionType = ACTION_SMS;
				m_FxnCallBack_IP(&pAlertInfo, &pActionInfo);
			}

			if (m_bDOP[0])
			{
				NET_DBG("my add:------m_bDOP[0]=1-------\n");
				pActionInfo.tActionType = ACTION_ALARMOUT;
				for (i = 0; i < 8; i++)
				{
					if (m_bDOP[0] & (1 << i))
					{
						pActionInfo.nChannel = i;
						m_FxnCallBack_IP(&pAlertInfo, &pActionInfo);
					}
				}

			}
		}

		NET_RenewDhcp();
	}
	else if (m_iNetStatus == 3)
	{							//上一次是冲突的，现在不是了
		if (NULL != m_FxnCallBack_IP)
		{
			struct tagAlertInfo pAlertInfo;

			struct tagActionInfo pActionInfo;

			pAlertInfo.AlertType = m_IPAlarmType;
			pAlertInfo.nChannel = 0;
			pAlertInfo.dwReserved = 1;

			pActionInfo.dwReserved = 0;
			pActionInfo.nChannel = 0;

			pActionInfo.dwReserved = (unsigned long) m_sOtherMAC_addr;

			//   if (m_bGUI[0])
			{
				pActionInfo.tActionType = ACTION_GUI;
				m_FxnCallBack_IP(&pAlertInfo, &pActionInfo);
			}
			
			//推送上一次发生过IP冲突
			if (m_bPush[0])
			{
				pActionInfo.tActionType = ACTION_PUSH;
				m_FxnCallBack_IP(&pAlertInfo, &pActionInfo);
			}
		}

	}
	if (m_iNetStatus != saveStatus)
	{
		m_iNetStatus = saveStatus;

		NET_INFO("status:%s\n", des_state[saveStatus]);
		if (saveStatus == 3)	//IP冲突
		{
			NET_INFO("IP conflict\n%-12s:[%02x:%02x:%02x:%02x:%02x:%02x]\n%-12s:[%s]\n", "HWaddr", m_sOtherMAC_addr[0], m_sOtherMAC_addr[1],
				 m_sOtherMAC_addr[2], m_sOtherMAC_addr[3], m_sOtherMAC_addr[4], m_sOtherMAC_addr[5], "inet addr", m_sLocalIP_new);
		}
	}
	if (m_iNetDHCPFailed || (status & 8))
	{							//上次DHCP失败，则更新DHCP
		NET_DBG("NET_CheckStatus: m_iNetDHCPFailed = 1\n");
		NET_RenewDhcp();
	}

	return m_iNetStatus;
}

struct arpMsg
{
    struct ethhdr ethhdr;           /* Ethernet header */
    u_short htype;              /* hardware type (must be ARPHRD_ETHER) */
    u_short ptype;              /* protocol type (must be ETH_P_IP) */
    u_char  hlen;               /* hardware address length (must be 6) */
    u_char  plen;               /* protocol address length (must be 4) */
    u_short operation;          /* ARP opcode */
    u_char  sHaddr[6];          /* sender's hardware address */
    u_char  sInaddr[4];         /* sender's IP address */
    u_char  tHaddr[6];          /* target's hardware address */
    u_char  tInaddr[4];         /* target's IP address */
    u_char  pad[18];            /* pad for min. Ethernet payload (60 bytes) */
};

struct server_config_t
{
    u_int32_t server;       /* Our IP, in network order */
    u_int32_t start;        /* Start address of leases, network order */
    u_int32_t end;          /* End of leases, network order */
    struct option_set *options; /* List of DHCP options loaded from the config file */
    char *interface;        /* The name of the interface to use */
    int ifindex;            /* Index number of the interface to use */
    unsigned char arp[6];       /* Our arp address */
    unsigned long lease;        /* lease time in seconds (host order) */
    unsigned long max_leases;   /* maximum number of leases (including reserved address) */
    char remaining;         /* should the lease file be interpreted as lease time remaining, or
                     * as the time the lease expires */
    unsigned long auto_time;    /* how long should udhcpd wait before writing a config file.
                     * if this is zero, it will only write one on SIGUSR1 */
    unsigned long decline_time;     /* how long an address is reserved if a client returns a
                         * decline message */
    unsigned long conflict_time;    /* how long an arp conflict offender is leased for */
    unsigned long offer_time;   /* how long an offered address is reserved */
    unsigned long min_lease;    /* minimum lease a client can request*/
    char *lease_file;
    char *pidfile;
    char *notify_file;      /* What to run whenever leases are written */
    u_int32_t siaddr;       /* next server bootp option */
    char *sname;            /* bootp server name */
    char *boot_file;        /* bootp boot file option */
};

/* globals */
struct server_config_t server_config;

int read_interface(char *interface, int *ifindex, u_int32_t *addr, unsigned char *arp)
{
    int fd;
    struct ifreq ifr;
    struct sockaddr_in *our_ip;
	char str[32] = { 0 };
    memset(&ifr, 0, sizeof(struct ifreq));
    if((fd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW)) >= 0)
    {
        ifr.ifr_addr.sa_family = AF_INET;
        strncpy(ifr.ifr_name, interface, sizeof(ifr.ifr_name) - 1);
        /*this is not execute*/
        if(addr)
        {
            if(ioctl(fd, SIOCGIFADDR, &ifr) == 0)
            {
                our_ip = (struct sockaddr_in *) &ifr.ifr_addr;
                *addr = our_ip->sin_addr.s_addr;
                NET_INFO("%s (our ip) = %s\n", ifr.ifr_name, inet_ntop(AF_INET, &our_ip->sin_addr, str, sizeof(str)));
            }
            else
            {
                NET_INFO("SIOCGIFADDR failed, is the interface up and configured?: %s\n", strerror(errno));
				close(fd);
                return -1;
            }
        }
        if(ioctl(fd, SIOCGIFINDEX, &ifr) == 0)
        {
            NET_INFO("adapter index %d\n", ifr.ifr_ifindex);
            *ifindex = ifr.ifr_ifindex;
        }
        else
        {
            NET_INFO("SIOCGIFINDEX failed!: %s\n", strerror(errno));
			close(fd);
            return -1;
        }
        if(ioctl(fd, SIOCGIFHWADDR, &ifr) == 0)
        {
            memcpy(arp, ifr.ifr_hwaddr.sa_data, 6);
            NET_INFO("adapter hardware address %02x:%02x:%02x:%02x:%02x:%02x\n",
                         arp[0], arp[1], arp[2], arp[3], arp[4], arp[5]);
        }
        else
        {
            NET_INFO("SIOCGIFHWADDR failed!: %s\n", strerror(errno));
			close(fd);
            return -1;
        }
    }
    else
    {
        NET_INFO("socket failed!: %s\n", strerror(errno));
        return -1;
    }
    close(fd);
    return 0;
}

/* args:    yiaddr - what IP to ping
 *      ip - our ip
 *      mac - our arp address
 *      interface - interface to use
 * retn:    1 addr free
 *      0 addr used
 *      -1 error
 */

/* FIXME: match response against chaddr */
int arpping(u_int32_t yiaddr, u_int32_t ip, unsigned char *mac, char *interface)
{
    int timeout = 2;
    int     optval = 1;
    int s;          /* socket */
    int rv = 1;         /* return value */
    struct sockaddr addr;       /* for interface name */
    struct arpMsg   arp;
    fd_set      fdset;
    struct timeval  tm;
    time_t      prevTime;
    if((s = socket(PF_PACKET, SOCK_PACKET, htons(ETH_P_ARP))) == -1)
    {
        NET_INFO("Could not open raw socket\n");
        return -1;
    }
    if(setsockopt(s, SOL_SOCKET, SO_BROADCAST, &optval, sizeof(optval)) == -1)
    {
        NET_INFO("Could not setsocketopt on raw socket\n");
        close(s);
        return -1;
    }
    /* send arp request */
    memset(&arp, 0, sizeof(arp));
    memcpy(arp.ethhdr.h_dest, MAC_BCAST_ADDR, 6);    /* MAC DA */
    memcpy(arp.ethhdr.h_source, mac, 6);        /* MAC SA */
    arp.ethhdr.h_proto = htons(ETH_P_ARP);      /* protocol type (Ethernet) */
    arp.htype = htons(ARPHRD_ETHER);        /* hardware type */
    arp.ptype = htons(ETH_P_IP);            /* protocol type (ARP message) */
    arp.hlen = 6;                   /* hardware address length */
    arp.plen = 4;                   /* protocol address length */
    arp.operation = htons(ARPOP_REQUEST);       /* ARP op code */
    memcpy(arp.tInaddr, &yiaddr, sizeof(yiaddr));       /* target IP address */
    memcpy(arp.sInaddr, &ip, sizeof(ip));       /* source IP address */
    * ((u_int *) arp.sInaddr) = ip;         /* source IP address */
    memcpy(arp.sHaddr, mac, 6);         /* source hardware address */
    strncpy(addr.sa_data, interface, sizeof(addr.sa_data) - 1);
    if(sendto(s, &arp, sizeof(arp), 0, &addr, sizeof(addr)) < 0)
    { rv = 0; }
    /* wait arp reply, and check it */
    tm.tv_usec = 0;
    time(&prevTime);
    while(timeout > 0)
    {
        FD_ZERO(&fdset);
        FD_SET(s, &fdset);
        tm.tv_sec = timeout;
        if(select(s + 1, &fdset, (fd_set *) NULL, (fd_set *) NULL, &tm) < 0)
        {
            NET_INFO("Error on ARPING request: %s\n", strerror(errno));
            if(errno != EINTR) { rv = 0; }
        }
        else if(FD_ISSET(s, &fdset))
        {
            if(recv(s, &arp, sizeof(arp), 0) < 0) { rv = 0; }
            if(arp.operation == htons(ARPOP_REPLY) &&
                    bcmp(arp.tHaddr, mac, 6) == 0 &&
                    * ((u_int *) arp.sInaddr) == yiaddr)
            {
                NET_INFO("Valid arp reply receved for this address\n");
                rv = 0;
                break;
            }
        }
        timeout -= time(NULL) - prevTime;
        time(&prevTime);
    }
    close(s);
    NET_INFO("%salid arp replies for this address\n", rv ? "No v" : "V");
    return rv;
}

/* check is an IP is taken, if it is, add it to the lease table */
int check_ip(u_int32_t addr, char *interface)
{
    struct in_addr temp;
	char str[32] = { 0 };
    if(arpping(addr, server_config.server, server_config.arp, interface) == 0)
    {
        temp.s_addr = addr;
        NET_INFO("%s belongs to someone, reserving it for %ld seconds\n",
        inet_ntop(AF_INET, &temp, str, sizeof(str)), server_config.conflict_time);
        return 1;
    }
    else { return 0; }
}

/*****************************************************************************
 函 数 名  : CheckIpConflic
 功能描述  : 检测ip是否可用，是否冲突
 输入参数  : char *interface
             char *ipaddr
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :
*****************************************************************************/
static int CheckIpConflic(char *interface,  char *ipaddr)
{
    if (read_interface(interface, &(server_config.ifindex), &server_config.server, server_config.arp) < 0)
    {
        return -1;
    }
    if(!check_ip(inet_addr(ipaddr), interface))
    {
        NET_INFO("IP:%s can use\n", ipaddr);
        return 0;
    }
    else
    {
        NET_INFO("IP:%s conflict\n", ipaddr);
        return -1;
    }
	
    return 0;
}

// 检测是否存在与本机IP地址重复的主机
int INet::IsIpConflict(int checkEnable)
{
	if(0 == checkEnable)
		return 0;
NET_DBG("start conflict check!\n");
	struct sockaddr_in from;

	int m_sock;

	int recved;

	socklen_t fromlen = sizeof (struct in_addr);

	char localIP[MAX_IP_STR_LEN];
	struct timeval tv = { 1, 0 };
	int ret = -1;

	unsigned int destIp;

	int arpPkgSize = sizeof (struct ethhdr) + sizeof (struct ether_arp);

	memset(&from, 0x0, sizeof (struct sockaddr_in));

	// 获取本地IP地址
	NET_GetIPAddress(localIP, sizeof(localIP));

	//  sscanf(localIP,"%d.%d.%d.%d",&localIp2[0],&localIp2[1],&localIp2[2],&localIp2[3]);
	destIp = inet_addr(localIP);

	if ((m_sock = socket(AF_INET, SOCK_PACKET, htons(0x0003))) < 0)
	{
		NET_ERR("socket failed\n");
		return -1;
	}
	
	setsockopt(m_sock, SOL_SOCKET, SO_SNDTIMEO, (void *) &tv, sizeof (tv));
	setsockopt(m_sock, SOL_SOCKET, SO_RCVTIMEO, (void *) &tv, sizeof (tv));

	//开始发送ARP包
	ArpSend(m_sock, IFNAME, (unsigned char *) m_sMAC_addr, destIp);

	//接受应答
	recved = recvfrom(m_sock, m_pArpReplyBuff, m_iArpReplyBuffSize, 0, (struct sockaddr *) &from, &fromlen);
	//关闭socket 
	close(m_sock);
    
	//开始处理应答信息
	while (recved > 0 && recved >= arpPkgSize)
	{
		struct ethhdr *eth_hdr = (struct ethhdr *) m_pArpReplyBuff;

		switch (ntohs(eth_hdr->h_proto))
		{

			case 0x0806:		// ARP packet
				{
					
					ret = IsResponseFromOthers(m_pArpReplyBuff, m_sMAC_addr);
					if (ret > 0)
					{
						//  NET_INFO("IP conflict\n");
						return TRUE;
					}
					else if (ret < 0)
					{
						return -1;
					}
					else
					{
						return FALSE;
					}
					break;
				}
			default:
				break;
		}

		recved -= arpPkgSize;
	}

	return ret;
}

// 发送arp包
int INet::ArpSend(int m_sock, const char *ifname, unsigned char srcHWAddr[6], unsigned int destIp)
{
	struct ethhdr *ethHdr = (struct ethhdr *) m_pArpPacket;

	struct ether_arp *arp = (struct ether_arp *) (m_pArpPacket + sizeof (struct ethhdr));

	struct sockaddr sa;


	memset(ethHdr->h_dest, 0xff, sizeof (ethHdr->h_dest));

	memcpy(ethHdr->h_source, srcHWAddr, 6);

	ethHdr->h_proto = htons(ETHERTYPE_ARP);

	arp->arp_hrd = htons(ARPHRD_ETHER);
	arp->arp_pro = htons(0x0800);
	arp->arp_hln = 6;
	arp->arp_pln = 4;
	arp->arp_op = htons(ARPOP_REQUEST);

	memcpy(arp->arp_sha, srcHWAddr, 6);

	memcpy(arp->arp_spa, &destIp, 4);
	memset(arp->arp_spa, 0, 4);


	//memcpy(arp->arp_tpa, (void *) &destIp, 4);
	arp->arp_tpa[0] =  destIp&0xff;
	arp->arp_tpa[1] = (destIp >> 8)&0xff;
	arp->arp_tpa[2] = (destIp >> 16)&0xff;
	arp->arp_tpa[3] = (destIp >> 24)&0xff;

	memset(arp->arp_tha, 0xff, sizeof (arp->arp_tha));

	strncpy(sa.sa_data, ifname,sizeof(sa.sa_data) - 1);

	if (sendto(m_sock, m_pArpPacket, m_iArpPacketSize, 0, &sa, sizeof (sa)) < 0)
	{
		NET_ERR("sendto failed\n");

		return -1;
	}

	return 0;
}

// 判断响应包是否由其它主机发送
int INet::IsResponseFromOthers(const char *buff, const char *localHWAddr)
{
    int i = 0;
	const struct ether_arp *arp = (const struct ether_arp *) (buff + sizeof (struct ethhdr));

	if (arp->arp_op != ARPOP_InREQUEST && arp->arp_op != htons(ARPOP_REPLY))
	{
		// NET_DBG("line = %d %d\n",__LINE__,arp->arp_op);
		return -1;
	}
	if (memcmp(arp->arp_tha, localHWAddr, 6) != 0)///*arp_tha为 目标的硬件地址*/
	{
		// NET_DBG("line = %d\n",__LINE__);
		return -1;
	}

    /* 目的端的协议地址为0，使能ipconf__Enable */
    for (i=0;i<4;i++)
    {
    	if (arp->arp_tpa[i] != 0)/*arp_tpa目标的协议地址*/
    	{
    		m_iPconf_Enable=FALSE;
    		break;
    	}
    	else
    	{
    		m_iPconf_Enable=TRUE;
    	}
    }

	/*arp_sha为发送者的硬件地址*/
	if ((m_iPconf_Enable)&&(memcmp(arp->arp_sha, localHWAddr, 6) != 0))	// 不是本机MAC地址 且 目的端的IP为0 返回IP冲突
	{
		m_sOtherMAC_addr[0] = arp->arp_sha[0];
		m_sOtherMAC_addr[1] = arp->arp_sha[1];
		m_sOtherMAC_addr[2] = arp->arp_sha[2];
		m_sOtherMAC_addr[3] = arp->arp_sha[3];
		m_sOtherMAC_addr[4] = arp->arp_sha[4];
		m_sOtherMAC_addr[5] = arp->arp_sha[5];

        NET_DBG("\n---INet.cpp------IsResponseFromOthers()----IP冲突-----\n");
		return TRUE;
	}

	return FALSE;
}

INet *INet::GetInstance()
{
	if (m_instance == 0)
	{
		m_instance = new INet;
		if(0 != m_instance)
		{
			m_instance->Init();
		}
	}
	return m_instance;
}

void INet::Release()
{
	if (m_instance != 0)
	{
		delete m_instance;

		m_instance = 0;
	}
}

void* OperateRouterThread(void * lParam)
{
    PRT_TID();
    NET_DBG(">>>>>>>>>> enter, pid = %d\n", getpid());

    //获取当前实例
    INet *pNet = (INet *)lParam;

    if (NULL != pNet)
    {
#ifdef __MODULE_4G_SUPPORT__	
        if (bSupportGPRS)
        {
		    pNet->Operate4GModuleRouterThreadProcess();	
        }
        else	
#endif            
        {
       	    pNet->OperateRouterThreadProcess();	
        }
    }
    else
    {
        NET_ERR("get pNet instance fail\n");
        NET_CHK(NULL != pNet);
    }

    NET_DBG(">>>>>>>>>> exit, pid = %d\n", getpid());

    return NULL;
}

/*@breif ParseDnsAddr 解析DNS地址
 *@       成功返回0
 *@       失败返回-1
 *@       strlen("nameserver  ") 空格的个数必须使用两个，和写入保持一致, 否则可能会导致配置文件被重复修改
 */
static int ParseDnsAddr(void)
{
	FILE *fp = NULL;
	char buf_line[128] = {0}; 
	char *find_start = NULL;
	char *find_end = NULL;
	unsigned int num = 0;

	fp = fopen(RESOLV_CONF, "r");
	if (NULL == fp)
	{
		NET_ERR("[dnstest] dns fp is NULL!\n");
        return -1;
	}

	for (unsigned int testnum = 0;  testnum < (sizeof(s_DnsAddr)/sizeof(s_DnsAddr[0])); testnum++ )
	{
		memset(&s_DnsAddr[testnum], 0, sizeof(s_DnsAddr[testnum]));
	}

	while(!feof(fp))
    {
    	// 有线/wifi    dns 配置文件最后一个是网关
		if (num >= (sizeof(s_DnsAddr)/sizeof(s_DnsAddr[0])))
		{
			break;
		}

    	memset(buf_line,0, sizeof(buf_line));
        fgets(buf_line, sizeof(buf_line), fp);

        find_start = strstr(buf_line,"nameserver");
        find_end = strstr(buf_line,"\n");

        if ((NULL != find_start) && (NULL != find_end))
        {
			if (find_end - find_start - strlen("nameserver  ") < DNS_ADDR_LEN_NET) // 空格要是两个 -- "nameserver  "
			{
				memset(s_DnsAddr[num],0, DNS_ADDR_LEN_NET);
				memcpy(s_DnsAddr[num],find_start+strlen("nameserver  "),find_end - find_start - strlen("nameserver  "));
	        	NET_INFO("DNSADDR[%u] = %s \n",num, s_DnsAddr[num]);
	        	num++;
	        }
        }
    }

    fclose(fp);
    return 0;
}

static int IsSameFile(const char *pDNS1, const char *pDns2)
{
	if (!pDNS1 || !pDns2)
	{
		return 0;
	}
	//NET_INFO("[dnstest] pDNS: %s	strlen: %d	cache : %s	 strlen: %d \n", pDNS1, strlen(pDNS1), pDns2, strlen(pDns2));

	if(0 == strncmp(pDNS1, pDns2, strlen(pDNS1)) && strlen(pDNS1) == strlen(pDns2))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

/**
********************************************************************************
 * @函数功能: 通过zwhost接口计算域名解析时间
 * @作者    : liushun
 * @param[in] *pDNS 主DNS
 * @param[in] *pSNS	次DNS
 * @param[in] *pGateWay	网关
 * @param[out] *plist	输出排序好的dns列表
 * @返回值  : 0表示成功, 否则表示错误码 
 * @其他    : 
********************************************************************************
*/
int INet::NET_UseZwhostCalcDomain(const char *pDNS, const char *pSNS, const char *pGateWay, ZWHost_DNS_SERVER_INFO_LIST *plist)
{
	int i = 0;
	int j = 0;
	int ret = -1;
	//有效DNS个数，默认包含8.8.8.8 114.114.114.114。所以是2个
	int nValidDNSCount = 2;
	ZWHost_DNS_SERVER_INFO_LIST DnsList[1] = {0};

	//将默认的DNS填入。
	memcpy((void *)DnsList[0].stDNSServerInfoList[0].cDNSServer,DEFAULT_RESOLV_CONF,strlen(DEFAULT_RESOLV_CONF));
	memcpy((void *)DnsList[0].stDNSServerInfoList[1].cDNSServer,DEFAULT_RESOLV_CONF_LOCAL,strlen(DEFAULT_RESOLV_CONF_LOCAL));

	if (pDNS && IsValidIP(pDNS))
	{
		if((0 == strncmp(DEFAULT_RESOLV_CONF, pDNS, strlen(DEFAULT_RESOLV_CONF)) && strlen(DEFAULT_RESOLV_CONF) == strlen(pDNS))
			|| (0 == strncmp(DEFAULT_RESOLV_CONF_LOCAL, pDNS, strlen(DEFAULT_RESOLV_CONF_LOCAL)) && strlen(DEFAULT_RESOLV_CONF_LOCAL) == strlen(pDNS)))
		{
			//主DNS与(8.8.8.8)/(114.114.114.114)相同 
		}
		else
		{
			memcpy((void *)DnsList[0].stDNSServerInfoList[nValidDNSCount++].cDNSServer,pDNS,strlen(pDNS));
		}
	}

	if (pSNS && IsValidIP(pSNS))
	{
		if((0 == strncmp(DEFAULT_RESOLV_CONF, pSNS, strlen(DEFAULT_RESOLV_CONF)) && strlen(DEFAULT_RESOLV_CONF) == strlen(pSNS))
			|| (0 == strncmp(DEFAULT_RESOLV_CONF_LOCAL, pSNS, strlen(DEFAULT_RESOLV_CONF_LOCAL)) && strlen(DEFAULT_RESOLV_CONF_LOCAL) == strlen(pSNS)))
		{
			//次DNS与(8.8.8.8)/(114.114.114.114)相同 
		}
		else
		{
			if(strlen(pDNS) == strlen(pSNS) && (0 == strncmp(pDNS, pSNS, strlen(pSNS))))
			{
				//主DNS与次DNS相同
			}
			else
			{
				memcpy((void *)DnsList[0].stDNSServerInfoList[nValidDNSCount++].cDNSServer,pSNS,strlen(pSNS));
			}
		}
	}

	if (pGateWay && IsValidIP(pGateWay))
	{
		if((pDNS && (0 == strncmp(pDNS, pGateWay, strlen(pDNS)) && strlen(pDNS) == strlen(pGateWay)))
			|| (pSNS &&(0 == strncmp(pSNS, pGateWay, strlen(pSNS)) && strlen(pSNS) == strlen(pGateWay))))
		{
			//主次DNS中有网关
		}
		else
		{
			memcpy((void *)DnsList[0].stDNSServerInfoList[nValidDNSCount++].cDNSServer,pGateWay,strlen(pGateWay));
		}
	}


	DnsList[0].nValidDNSServerCount = nValidDNSCount;
	ret = Hosts_MultiThread_GetDomainResolutionTime(ZWHOST_DOMAIN_RESOLUTION,DnsList,ZWHOST_RESOLUTION_TIMEOUT);
	if(!ret)
	{
		for (i = 0; i < nValidDNSCount - 1; i++) {
			int swapped = 0; // 优化：无交换则提前退出
			for (j = 0; j < nValidDNSCount -1 - i; j++) {
				ZWHost_DNS_SERVER_INFO *cur = &DnsList[0].stDNSServerInfoList[j];
				ZWHost_DNS_SERVER_INFO *next = &DnsList[0].stDNSServerInfoList[j+1];
	
				// 排序核心逻辑
				if (cur->nParseStatus < next->nParseStatus) {
					// case1：当前是失败(0)，下一个是成功(1) → 交换（成功的靠前）
					ZWHost_DNS_SERVER_INFO tmp;
					memcpy(&tmp, cur, sizeof(ZWHost_DNS_SERVER_INFO));
					memcpy(cur, next, sizeof(ZWHost_DNS_SERVER_INFO));
					memcpy(next, &tmp, sizeof(ZWHost_DNS_SERVER_INFO));
					swapped = 1;
				} else if (cur->nParseStatus == next->nParseStatus && cur->nParseStatus == 1) {
					// case2：两者都成功 → 耗时大的往后排
					if (cur->nParseTime > next->nParseTime) {
						ZWHost_DNS_SERVER_INFO tmp;
						memcpy(&tmp, cur, sizeof(ZWHost_DNS_SERVER_INFO));
						memcpy(cur, next, sizeof(ZWHost_DNS_SERVER_INFO));
						memcpy(next, &tmp, sizeof(ZWHost_DNS_SERVER_INFO));
						swapped = 1;
					}
				}
				// case3：两者都失败 → 不交换（保留原序）
			}
			if (!swapped) {
				break; // 无交换，说明已排好序，提前退出
			}
		}
		for(i = 0; i < nValidDNSCount; i++)
		{
			NET_INFO("stDNSServerInfoList[%d] DNS = [%s] nParseStatus = [%d] nParseTime = [%d]\n",
				i,DnsList[0].stDNSServerInfoList[i].cDNSServer,
				DnsList[0].stDNSServerInfoList[i].nParseStatus,
				DnsList[0].stDNSServerInfoList[i].nParseTime);
		}
	}
	else
	{
		NET_ERR("Hosts_MultiThread_GetDomainResolutionTime err! ret = [%d]\n",ret);
	}
	memcpy((void *)plist,(void *)DnsList,sizeof(ZWHost_DNS_SERVER_INFO_LIST));

	return ret;
}



/*************************************************
 *@brief IsNeedUpdateDnsFileByZwhost 判断DNS文件是否需要更新根据zwhost接口排序
 *@param[in]   ： *list zwhost查询排序后的dns列表
 *@return 
    - 0: 如果所有地址与对应文件中的地址相同，则不需要更新
    - 1: 如果有任何地址与对应文件中的地址不同，则需要更新
    
 * 文件记录的DNS地址规则：
 * 1. 主DNS地址
 * 2. 次DNS地址
 * 3. 备选1DNS地址
 * 4. 备选2DNS地址
 * 5. 网关地址
 *@Return    
**************************************************/
int IsNeedUpdateDnsFileByZwhost(ZWHost_DNS_SERVER_INFO_LIST *list)
{
	if(NULL == list)
	{
		return 1;
	}
	int i = 0;
	int ret = 0;
	int validnum = 0;
	int dnstotalnum = 0;
	validnum = list->nValidDNSServerCount;
	dnstotalnum = sizeof(s_DnsAddr)/sizeof(s_DnsAddr[0]);
	validnum = (validnum > dnstotalnum) ? dnstotalnum : validnum;

	for(i = 0; i < validnum; i++)
	{
		if (strlen(&s_DnsAddr[i][0]) > 0)
		{
			if(IsSameFile((const char *)list->stDNSServerInfoList[i].cDNSServer, (const char *)&s_DnsAddr[i][0]))
			{
				continue;
			}
			else
			{
				ret = 1;
				break;
			}
		}
		else
		{
			ret = 1;
		}
	}
	return ret;
}


/**
********************************************************************************
 * @函数功能: 通过zwhost排序后的dns列表写入reslove.conf
 * @作者        :   liushun
 * @param[in]   ： *list zwhost查询排序后的dns列表
 * @返回值      : 0-成功，非0失败
 * @其他        : 
********************************************************************************
*/
int WriteDNSFileByZwhost(ZWHost_DNS_SERVER_INFO_LIST *list)
{
	if(NULL == list)
	{
		return -1;
	}

	int i = 0;
	int ret = 0;
	int validnum = 0;
	char system_cmd[512] = {0};
	XTRACE_MUTEX_LOCK(&m_WriteDNSFileLock);
	ret = IsNeedUpdateDnsFileByZwhost(list);
	if (1 != ret)
	{
		//NET_DBG("[dnstest] all same, no need to update resovl.conf ! \n");
		XTRACE_MUTEX_UNLOCK(&m_WriteDNSFileLock);
		return -1;
	}
	validnum = list->nValidDNSServerCount;
	snprintf(system_cmd,sizeof(system_cmd) - 1, "echo > %s", RESOLV_CONF_FILE_PATH_REAL);
	mysystem(system_cmd);

	for(i = 0; i < validnum; i++)
	{
		memset(system_cmd, 0, sizeof(system_cmd));
		if(0 == i)
		{
			snprintf(system_cmd, sizeof(system_cmd) - 1, "sed -i '1i\\nameserver  %s' %s", list->stDNSServerInfoList[i].cDNSServer, RESOLV_CONF_FILE_PATH_REAL);
		}
		else
		{
			snprintf(system_cmd,sizeof(system_cmd) - 1, "echo 'nameserver  %s' >> %s", list->stDNSServerInfoList[i].cDNSServer, RESOLV_CONF_FILE_PATH_REAL);
		}
		mysystem(system_cmd);
	}
	// 有写入就更新一下dns缓存，以便于和下次写入进行比对，重复则不进行写入了
	ParseDnsAddr();

	XTRACE_MUTEX_UNLOCK(&m_WriteDNSFileLock);
	return 0;
}

#if 0
int INet::WriteDNSFile(const char *pDNS, const char *pSNS, const char *pGateWay)
{
	int ret = 0;
	int flag_dns_analysis_succ = 0;
	char system_cmd[255] = {0};
    const char *domain = "www.baidu.com";

	// 注意锁里面要去掉打印的
	XTRACE_MUTEX_LOCK(&m_WriteDNSFileLock);
	// 先读取dns配置文件缓存, 如果重复就不再写入
	ret = IsNeedUpdateDnsFile(pDNS, pSNS, pGateWay);
	if (1 != ret)
	{
		XTRACE_MUTEX_UNLOCK(&m_WriteDNSFileLock);
		return -1;
	}
	else
	{
		NET_INFO("start to update resovl.conf \n");
	}

    memset(system_cmd, 0, sizeof(system_cmd));
	snprintf(system_cmd, sizeof(system_cmd)-1, "echo > %s", RESOLV_CONF_FILE_PATH_REAL);
	mysystem(system_cmd);

	// 先写主dns, 避免和默认dns 重复
	if (pDNS && IsValidIP(pDNS))
	{
		if (IsSameFile(pDNS, DEFAULT_RESOLV_CONF) || IsSameFile(pDNS, DEFAULT_RESOLV_CONF_LOCAL))
		{
			//NET_INFO("[dnstest] same dns as default file pDNS: %s \n ", pDNS);
		}
		else
		{
			memset(system_cmd, 0, sizeof(system_cmd));
            if ((0 == flag_dns_analysis_succ) && (dns_client_commit(pDNS, domain) > 0))
            {
                snprintf(system_cmd, sizeof(system_cmd)-1, "sed -i '1i\\nameserver  %s' %s", pDNS, RESOLV_CONF_FILE_PATH_REAL);
                flag_dns_analysis_succ = 1;
            }
            else
            {
                snprintf(system_cmd, sizeof(system_cmd)-1, "echo 'nameserver  %s' >> %s", pDNS, RESOLV_CONF_FILE_PATH_REAL);
            }
			mysystem(system_cmd);
		}
	}

	// 避免和 默认以及主dns重复
	if (pSNS && IsValidIP(pSNS))
	{
		if (IsSameFile(pSNS, DEFAULT_RESOLV_CONF) || IsSameFile(pSNS, DEFAULT_RESOLV_CONF_LOCAL))
		{
			//NET_INFO("[dnstest] same dns as default file pSNS: %s \n ", pSNS);
		}
		else if(pDNS && IsSameFile(pSNS, pDNS))
		{
			//NET_INFO("[dnstest] same dns as main dns: %s \n ", pSNS);
		}
		else
		{
			memset(system_cmd, 0, sizeof(system_cmd));
            if ((0 == flag_dns_analysis_succ) && (dns_client_commit(pSNS, domain) > 0))
            {
                snprintf(system_cmd, sizeof(system_cmd)-1, "sed -i '1i\\nameserver  %s' %s", pSNS, RESOLV_CONF_FILE_PATH_REAL);
                flag_dns_analysis_succ = 1;
            }
            else
            {
                snprintf(system_cmd, sizeof(system_cmd)-1, "echo 'nameserver  %s' >> %s", pSNS, RESOLV_CONF_FILE_PATH_REAL);
            }
			mysystem(system_cmd);
		}
	}

	// 写默认dns
	memset(system_cmd, 0, sizeof(system_cmd));
    if ((0 == flag_dns_analysis_succ) && (dns_client_commit(DEFAULT_RESOLV_CONF_LOCAL, domain) > 0))
    {
        snprintf(system_cmd, sizeof(system_cmd)-1, "sed -i '1i\\nameserver  %s' %s", DEFAULT_RESOLV_CONF_LOCAL, RESOLV_CONF_FILE_PATH_REAL);
        flag_dns_analysis_succ = 1;
    }
    else
    {
        snprintf(system_cmd, sizeof(system_cmd)-1, "echo 'nameserver  %s' >> %s", DEFAULT_RESOLV_CONF_LOCAL, RESOLV_CONF_FILE_PATH_REAL);
    }
	mysystem(system_cmd);

	memset(system_cmd, 0, sizeof(system_cmd));
    if ((0 == flag_dns_analysis_succ) && (dns_client_commit(DEFAULT_RESOLV_CONF, domain) > 0))
    {
        snprintf(system_cmd, sizeof(system_cmd)-1, "sed -i '1i\\nameserver  %s' %s", DEFAULT_RESOLV_CONF, RESOLV_CONF_FILE_PATH_REAL);
        flag_dns_analysis_succ = 1;
    }
    else
    {
        snprintf(system_cmd, sizeof(system_cmd)-1, "echo 'nameserver  %s' >> %s", DEFAULT_RESOLV_CONF, RESOLV_CONF_FILE_PATH_REAL);
    }
	mysystem(system_cmd);

	// 网关避免和主次dns重复
	if (pGateWay && IsValidIP(pGateWay))
	{
		if ((pDNS && IsSameFile(pGateWay, pDNS)) || (pSNS &&IsSameFile(pGateWay, pSNS)))
		{
			//NET_INFO("[dnstest] same dns as pGateWay: %s pDNS: %s pSNS: %s \n ", pGateWay, pDNS, pSNS);
		}
		else
		{
			memset(system_cmd, 0, sizeof(system_cmd));
            if ((0 == flag_dns_analysis_succ) && (dns_client_commit(pGateWay, domain) > 0))
            {
                snprintf(system_cmd, sizeof(system_cmd)-1, "sed -i '1i\\nameserver  %s' %s", pGateWay, RESOLV_CONF_FILE_PATH_REAL);
                flag_dns_analysis_succ = 1;
            }
            else
            {
                snprintf(system_cmd, sizeof(system_cmd)-1, "echo 'nameserver  %s' >> %s", pGateWay, RESOLV_CONF_FILE_PATH_REAL);
            }
			mysystem(system_cmd);
		}
	}

	// 有写入就更新一下dns缓存，以便于和下次写入进行比对，重复则不进行写入了
	ParseDnsAddr();

	XTRACE_MUTEX_UNLOCK(&m_WriteDNSFileLock);
	return 0;
}
#else

int INet::WriteDNSFile(const char *pDNS, const char *pSNS, const char *pGateWay)
{
	if(NULL == pDNS || NULL == pSNS || NULL == pGateWay)
	{
		NET_ERR("Write Param is err\n");
		return -1;
	}
	//创建临时线程处理修改
	static pthread_t WriteDnsFileThreadId = 0;
	static DNSThrParam Thrparam[1] = {0};

	pthread_mutex_lock(&m_ThreadParamLock);
	strncpy(Thrparam[0].primaryDNS,pDNS,sizeof(Thrparam[0].primaryDNS)-1);
	strncpy(Thrparam[0].secondaryDNS,pSNS,sizeof(Thrparam[0].secondaryDNS)-1);
	strncpy(Thrparam[0].gateway,pGateWay,sizeof(Thrparam[0].gateway)-1);
	if(0 == WriteDnsFileThreadId)
	{
		Thrparam[0].count = 0;
		Thrparam[0].pThrID = &WriteDnsFileThreadId;
		if(0 != pthread_create(&WriteDnsFileThreadId, NULL, DomainResolutionManageThread, Thrparam))
		{
			WriteDnsFileThreadId = 0;
		}
	}
	else
	{
		Thrparam[0].count++;
	}
	pthread_mutex_unlock(&m_ThreadParamLock);
	return 0;
}


#endif
//!brief 路由表操作线程
void* INet::OperateRouterThreadProcess()
{
    char command[64] = {0};
    int connStatus = 0;
    int lastNetStatus = -1;  // 默认无效值，意图是首次条件一定能满足
    int result = 0;
    int lan_break_count = 0;
	//主DNS地址
	char tmpPrimaryDNS[20]={0};
	//第二DNS地址
	char tmpSecondaryDNS[20]={0};
	char szGateway[32]={0};
    SelectSleepSec(10);

    /*
     *  需要更新路由的条件(任意一条达到都要更新路由):
     *  1. 网线状态改变(插拔网线)；
     *  2. IP有更新(包括有线无线)；
     *  3. DHCP状态发生改变(包括有线无线)；
     *  4. 重启网卡；
     */
    while(1)
    {
        SelectSleepSec(1);
        /* connStatus:1连上; connStatus:0断开 */
		connStatus = g_ethLinkStatus;

		//先更新变量
		if (0 == connStatus)//无线
		{
		    m_nCurrNetCardType = 3;  /* 无线 */
		}
		else
		{
			m_nCurrNetCardType = 1;    /* 有线 */
		}
		if (lastNetStatus != connStatus)  /* 当有线和无线进行了相互切换 */
		{
	#if defined (__IS_SUPPORT_WLAN__)
			switchNetCardOperate(connStatus);
	#endif
		}

        /* 如果物理网线状态发生改变(断开或者连接)，则需要进行操作(路由和切换网卡) */
		if (((lastNetStatus!=connStatus)&&(connStatus!=-1)) || (TRUE==bNeedUpdateRouter))
		{
            /* 针对非手动切换路由的情况，如果当前状态切换了，且网络是断开的情况 */
            if ((FALSE==bNeedUpdateRouter) && (0==connStatus))
            {
                /* 累计5次如果一直网线是断开的，则需要切换路由 */
                if (++lan_break_count >= 5)
                {
                    lan_break_count = 0;
                    NET_INFO("Detect lan has broken last 5 seconds!\n");
                }
                else
                {
                    /* 注意此处不能对lastNetStatus和lan_break_count进行赋值 */
                    continue;
                }
            }

            /* 其他情况下，需要置为 0 */
            lan_break_count = 0;

            NET_INFO("===Condition has changed! Need to operate router===\n");

            XTRACE_MUTEX_LOCK(&m_OperateRouteLock);

            bOperatingRouter = TRUE;

            mysystem("/usr/sbin/operate_router delRoute eth0");
			mysystem("/usr/sbin/operate_router delRoute wlan0");

            mysystem("route -n");
			/* 发现一个问题，就是设置route之后，网关的设置，是通过读取 tmp/wlan.gw.gw.gw文件实现的，
			   设备中没有这个文件就会去读取/etc/network/ 的 interfaces 文件，设置默认值 */
			if (connStatus == 0)
			{
                NET_INFO("[%d -> 0] Lan Port has break, need delete net route and add wireless route!\n", lastNetStatus);
				mysystem("/usr/sbin/operate_router addRoute wlan0");
                m_nCurrNetCardType = 3;          /* 当前有效的是无线网卡 */
            }
            else
            {                                    /* 有线连上--->删除无线路由，添加有线路由 */ 
                NET_INFO("[%d -> %d] need delete wireless route and add net route!\n", lastNetStatus, connStatus);
                mysystem("/usr/sbin/operate_router addRoute eth0");
                m_nCurrNetCardType = 1;          /* 当前有效的是有线网卡 */ 
            }
			/* 避免更新路由的时候，网关不对，设置好后，更新下网关*/

            bOperatingRouter = FALSE;
            memset(command, 0, sizeof(command)); /* 创建一个标记文件，里面存放当前有效的网卡类型 */
            snprintf(command, sizeof(command), "echo %d > /tmp/.effect_net_card", m_nCurrNetCardType);
            mysystem(command);
            NET_INFO("run: %s\n", command);
            lastNetStatus = connStatus;

            if (TRUE == bNeedUpdateRouter)
            {
                bNeedUpdateRouter = FALSE;
            }

            /* 设置路由之后，需要清除一下ARP缓存，以免不能上外网 */
            mysystem(ClearARPCache);
            NET_INFO("run: %s\n", ClearARPCache);

            mysystem("route -n");

            NET_INFO("===Operate router Over===\n");
  
            XTRACE_MUTEX_UNLOCK(&m_OperateRouteLock);

            /* 网络状态发生改变需要重启网络探测服务(用以防止私有服务探测不到) */
            NET_INFO("--- Reboot device probe server (HieServer_DeviceProbeStop) start ---\n");
            HieServer_DeviceProbeStop();
            HieServer_DeviceProbeStart();
            NET_INFO("--- Reboot device probe server (HieServer_DeviceProbeStart) stop ---\n");

			// 获取配置文件对应字段的dns
			if (connStatus == 0)  
			{
				m_pCConfig->GetValue(IPPPOE_CfgSection, INet_WLANDNS0_key, DEFAULT_RESOLV_CONF, tmpPrimaryDNS, 20);
				m_pCConfig->GetValue(IPPPOE_CfgSection, INet_WLANDNS1_key, DEFAULT_RESOLV_CONF_LOCAL, tmpSecondaryDNS, 20);
			}
			else
			{
				m_pCConfig->GetValue(IPPPOE_CfgSection, IPPPOE_PrimaryDNS_key, DEFAULT_RESOLV_CONF, tmpPrimaryDNS, 20);
				m_pCConfig->GetValue(IPPPOE_CfgSection, IPPPOE_SecondaryDNS_key, DEFAULT_RESOLV_CONF_LOCAL, tmpSecondaryDNS, 20);
			}

			
			if(m_YLDid)
			{
				strncpy(tmpPrimaryDNS, GOOGLE_DOMAIN_YL_1, sizeof(tmpPrimaryDNS) - 1);
				strncpy(tmpSecondaryDNS, GOOGLE_DOMAIN_YL_2, sizeof(tmpSecondaryDNS) - 1);
			}

			// 将网关作为备份dns
			if (connStatus == 0)                 
			{
				/* 有线断开，wlan0需要设置DNS */
				if (bSupportWIFI)
				{		
					memset(szGateway, 0, sizeof(szGateway));
					WLAN_GetGateway(szGateway);
				}
            }
            else
            {   
            	/* 有线连上--->重新替换有线DNS */ 
				if (bSupportWIFI)
				{			
					memset(szGateway, 0, sizeof(szGateway));
					NET_GetGateway(szGateway);
				}
            }
			
			NET_INFO("OperateRouterThreadProcess [dnstest] [write] %s cfgfile maindns: %s  seconddns: %s  gateway :%s \n", (0==connStatus) ? "wlan0":"eth0", tmpPrimaryDNS, tmpSecondaryDNS, szGateway);
			
			ZWHost_DNS_SERVER_INFO_LIST DnsList[1] = {0};
			NET_UseZwhostCalcDomain(tmpPrimaryDNS,tmpSecondaryDNS, szGateway,DnsList);
			WriteDNSFileByZwhost(DnsList);

			res_init();
			
#ifdef USE_HOSTS_PARSER
            /* 网络状态发生改变需要更新hosts */
            NET_INFO("--- Update the hosts start ---\n");
            result = Hosts_Update();
            if (result)
            {
                NET_ERR("Fail to update the hosts, errno = %d\n", result);
            }
            else
            {
                NET_INFO("--- Update the hosts over ---\n");
            }

			res_init();
#endif			
        }
    }

    return NULL;
}

#if defined (__IS_SUPPORT_WLAN__)
int INet::switchNetCardOperate(int isWire)
{
	if (bSupportWIFI)
    {
    	//处理无线的连接
    	if (WLAN_STA_MODE == m_wifiMode)
    	{
    		CfgWirelessWhenWireChanged(isWire);
    	}
    }
	return 0;
}

void* SendWifiTestCommandThread(void *lParam)
{
    PRT_TID();
    pthread_detach(pthread_self());

    NET_DBG(">>>>>>>>>> enter, pid = %d\n", getpid());

    //获取当前实例
    INet *pNet = (INet *)lParam;
    if (NULL != pNet)
    {
       pNet->SendWifiTestCommandThreadProcess();
    }
    else
    {
        NET_ERR("get pNet instance fail\n");
        NET_CHK(NULL != pNet);
    }
    NET_DBG(">>>>>>>>>> exit, pid = %d\n", getpid());

    return NULL;
}

//监控wifi测试恢复配置线程
void* INet::SendWifiTestCommandThreadProcess(void)
{
	int ret = 0;
    while(1)
    {
        // 等待唤醒(如果用户点击了wifi测试按钮且上次处理已经结束)
        sem_wait(&m_semWakeThread);
		NET_INFO("---SendWifiTest---\n");
		//m_wifiCfgMode = eWifiCfgMode_Normal;

		wifi_set_config_enable(0);
		ret = wifi_set_conn_cfg(&m_tWlan_Connection);
		if(-1 == ret)//表示参数有问题，说明前一次就是问题参数，那么此时就关掉连接就行了
		{
			wifi_disable_conn();
		}
        m_bWifiTestIsBusy = 0;
        /* 由于先前设置过网络参数，以便测试wifi，所以后面还原参数之后，需要更新路由 */
        bNeedUpdateRouter = TRUE;
	}
    return NULL;
}

/* 设备重连线程 */
void* INet::WlanReconnectThreadProcess()
{
    int i = 0;
	int scanflag = 0;
    while(1)
    {
       /*  循环检测条件:
        * 1. 当前是STA模式；
        * 2. 当前网线断开；
        * 3. 当前WIFI处于断开状态；
        */
        if (WLAN_STA_MODE == m_wifiMode  // STA模式
        && (0 == g_ethLinkStatus)        // 网线断开
        && (0 != m_iWlan_Status))        // WIFI断开
        {
			/*if 里面全部屏蔽做验证 ----疑似泄漏 需要再做验证 */
            NET_INFO("Detect that wifi is disconnect! reconnect it---!\n");
            if(strlen(m_tWlan_Connection.szSSID))
            {
                //这里存在问题，刚启动时只获取一次不一定能获取到
                //这里获取多次，都失败，则直接使用最大概率的加密配置方式
                for (i = 0; i < 5; i++)
                {
                    if (0 == wifi_get_conn_info_wpa(1, m_tWlan_Connection.szSSID, &m_tWlan_Connection))
                    {
						scanflag = 1;
                        break;
                    }
                    usleep(50000);
                }
            }
			if(scanflag)
			{
				scanflag = 0;
				wifi_set_conn_cfg(&m_tWlan_Connection);
				// 如果DHCP使能则需要重新获取
				if(m_bWlan_DHCP_Enable)
				{
					NET_INFO("DISABLE_WIFI_CONNECT_WHEN_WIRE====do dhcp!\n");
					m_iWlan_DHCPFailed = 1;
				}
				bNeedUpdateRouter = TRUE;
			}
        }

        sleep(60);  // 每隔1分钟检测一次
    }
    
    return NULL;
}

static void *ApStaStatusCheckThread(void *lParam)
{
    PRT_TID();
    char command[64] = {0};
    INet *pnet = (INet *) lParam;
	unsigned int wifiModeCheckCount = 0;

    while(1)
    {
		/*添加 AP / STA 模式的小工具检测 */
		/* AP mode need udhcpd and hostapt | sta mode need wpa_supplicant */
		/* triple check */

		if(pnet->WLAN_GetWifiMode() == SOFT_AP_MODE)
		{
			/* 没有找到对应的小程序 */
			if (!IsStringInCommand("ps", "hostapd"))
			{
				wifiModeCheckCount++;
			}
			else 
			{
				wifiModeCheckCount = 0;
			}
			if((wifiModeCheckCount % 10 == 0) && (wifiModeCheckCount < 31) && (wifiModeCheckCount > 1))
			{
				NET_INFO("wifi ap mode rerun hostapd\n");
				/* 没有找到 hostapt */
				memset(command, 0, sizeof(command));
				snprintf(command, sizeof(command), "hostapd %s &", HOSTAPD_CONF_FILE_PATH);
				if (mysystem(command))
				{
					NET_ERR("Fail to exec: %s\n", command);
				}
			}
		}
		else if (pnet->WLAN_GetWifiMode() == WLAN_STA_MODE) 
		{
			/* 没有找到对应的小程序 */
			if(!IsStringInCommand("ps", "wpa_supplicant"))
			{
				wifiModeCheckCount++;
			}
			else
			{
				wifiModeCheckCount = 0;
			}
			if((wifiModeCheckCount % 10 == 0) && (wifiModeCheckCount < 31) && (wifiModeCheckCount > 1))
			{
				if (mysystem(RUN_WPA_SUPPLICANT))
				{
					NET_ERR("run wpa_supplicant error!\n");
				}
			}
		}
        SelectSleepSec(1);
    }
    return NULL;
}

int INet::WLAN_SetConnectionEx(LPWLAN_CONNECTION pConnection)
{
	//检测热点信息是否有效,有些情况下是调用大结构体设置网络参数
	//当在AP模式下时防止无效的热点设置下来
	if(0 == strlen(pConnection->szSSID))
	{
		NET_ERR("wifi SSID is NUll!\n");
		return 0;
	}

    WLAN_CONNECTION connectionCfg;

    XTRACE_MUTEX_LOCK(&m_SetWifiConnectLock);

    if (bSupportWlanAP)
    {
        //AP模式下接收到配置后保存信息然后切换到STA模式
        if(SOFT_AP_MODE == m_wifiMode)
        {
            XTRACE_MUTEX_UNLOCK(&m_SetWifiConnectLock);
            return 0;
        }
    }

    memcpy(&connectionCfg, pConnection, sizeof(WLAN_CONNECTION));

	/*wifi热点的设置目前客户端并未传MAC地址下来*/
    if ((0 == strncmp(m_tWlan_Connection.szSSID, connectionCfg.szSSID, WLAN_MAX_SSID_LEN))
        && (m_tWlan_Connection.nAuthMode == connectionCfg.nAuthMode)
        && (m_tWlan_Connection.nEncrypType == connectionCfg.nEncrypType))
    {
        if (ET_NONE == m_tWlan_Connection.nEncrypType)
        {
            XTRACE_MUTEX_UNLOCK(&m_SetWifiConnectLock);
            return 0;
        }
        else if ((ET_WEP == m_tWlan_Connection.nEncrypType)
                 && (m_tWlan_Connection.struKeyCfg.nDefKeyID == connectionCfg.struKeyCfg.nDefKeyID)
                 &&
                 (((0 == m_tWlan_Connection.struKeyCfg.nDefKeyID)
                   && (0 == strncmp(m_tWlan_Connection.struKeyCfg.Key1.szKey, connectionCfg.struKeyCfg.Key1.szKey, WLAN_MAX_KEY_LEN)))
                  || ((1 == m_tWlan_Connection.struKeyCfg.nDefKeyID)
                      && (0 == strncmp(m_tWlan_Connection.struKeyCfg.Key2.szKey, connectionCfg.struKeyCfg.Key2.szKey, WLAN_MAX_KEY_LEN)))
                  || ((2 == m_tWlan_Connection.struKeyCfg.nDefKeyID)
                      && (0 == strncmp(m_tWlan_Connection.struKeyCfg.Key3.szKey, connectionCfg.struKeyCfg.Key3.szKey, WLAN_MAX_KEY_LEN)))
                  || ((3 == m_tWlan_Connection.struKeyCfg.nDefKeyID)
                      && (0 == strncmp(m_tWlan_Connection.struKeyCfg.Key4.szKey, connectionCfg.struKeyCfg.Key4.szKey, WLAN_MAX_KEY_LEN)))))
        {
            XTRACE_MUTEX_UNLOCK(&m_SetWifiConnectLock);
            return 0;
        }
        else if (0 == strncmp(m_tWlan_Connection.struKeyCfg.WPAKey.szKey, connectionCfg.struKeyCfg.WPAKey.szKey, WLAN_MAX_KEY_LEN))
        {
            XTRACE_MUTEX_UNLOCK(&m_SetWifiConnectLock);
            return 0;
        }
    }
	m_wifiConnChange = 1;
	NET_WARN("======================wifi cfg will change===========\n");
    XTRACE_MUTEX_UNLOCK(&m_SetWifiConnectLock);

    return 0;
}

int INet::WLAN_SendTestConfigure(LPWLAN_CONNECTION pConnection)
{
    if (NULL == pConnection)
    {
        NET_ERR("pConnection==NULL\n");
		//-1:无线参数无效
        return 16010;
    }
    
    /* 只有有线下面才可以进行wifi测试 */
    if (1 != m_nCurrNetCardType)
    {
        NET_INFO("---m_nCurrNetCardType(%d) != 1---\n", m_nCurrNetCardType);
		//1:无线网卡未启用
        return 16011;
    }

    /* 只有在STA模式下面才可以进行wifi测试 */
    if (bSupportWlanAP)
    {
        if (WLAN_STA_MODE != m_wifiMode)
        {
            NET_INFO("---m_wifiMode(%d) != 1---\n", m_wifiMode);
			//2:当前处于AP模式,STA模式才支持测试
            return 16012;
        }
    }
	
    /* 必须等待一个测试完毕之后才可以做下一个测试 */
    if (1 == m_bWifiTestIsBusy)
    {
        NET_INFO("---Wifi test is busy---\n");
		//3:测试中，请稍候...
        return 16013;
    }
	
    m_bWifiTestIsBusy = 1;//从开始测试到测试结束返回上次配置过程中该变量置位
	
    XTRACE_MUTEX_LOCK(&m_SetWifiConnectLock);
    int ret = wifi_test_conn_cfg(pConnection);
    if (ret)
    {
        NET_ERR("Call wifi_test_conn_cfg fail!\n");
        XTRACE_MUTEX_UNLOCK(&m_SetWifiConnectLock);

		if (-1 == ret)
		{
			//-1:无线参数无效
			m_bWifiTestIsBusy = 0;
			return 16014;
		}
		else
		{
			//4:没有可用的无线设备
			m_bWifiTestIsBusy = 0;
			return 16015;
		}
    }
    m_bWifiTestHasSet = 1;
    
    XTRACE_MUTEX_UNLOCK(&m_SetWifiConnectLock);
    
    return ret;
}

/* 获取测试wifi的状态 return -1:必须先调用设置接口 0:断开  1:连上 2:密码错误*/
int INet::WLAN_GetWifiTestStatus()
{
    int status = 0;
    int i = 0;
    int sem_value = 0;
	int failReason = 0;
	int ret = 0;

    if (0 == m_bWifiTestHasSet)
    {
        NET_ERR("Must be called WLAN_SendTestConfigure first!\n");
        return -1;
    }
    
    m_bWifiTestHasSet = 0;

    // 重试次数不可以超过前面测试设置的次数，否则获取的结果会出现错误
    while(i++ <= 15)
    {
        SelectSleepSec(1);
        if (0 != wifi_get_curr_status(&status))
        {
            NET_ERR("wifi get conn status failed\n");
            status = 0;
            continue;
        }
		
		if(0 == status)//如果连接失败了则去查下错误原因
		{
			ret = get_wifi_fail_reason(&failReason);
			if(0 == ret)
			{
				switch(failReason)
				{
					case 1:
						status = 2;
						break;
				}
			}
		}
		
        if ((1 == status) || (2 == status)) // 成功或密码错误则直接返回
        {
            break;
        }

        //  不成功则重试15次，如果还不成功则超时退出
    }
    
    sem_getvalue(&m_semWakeThread, &sem_value);
    if (sem_value == 0)
    {
        sem_post(&m_semWakeThread);
        NET_INFO("---Send post to set old wifi configuration---\n");
    }
    
    return status;
}

//是否正在进行wifi配置
int INet::WLAN_IsConfigureNetIng()
{
	return m_bIsConfigureNetIng;
}

//保存二维码配网交互配对信息
int INet::WLAN_SaveQrConfigureWlanSessionID(char *pszData)
{
	if (NULL == pszData)
	{
		return -1;
	}
	
    memset(m_szSessionID, 0, sizeof(m_szSessionID));
	snprintf(m_szSessionID, sizeof(m_szSessionID), "%s", pszData);
	
	return 0;
}

//获取二维码配网交互配对信息
int INet::WLAN_GetQrConfigureWlanSessionID(char *pszData, int nBufLen)
{
	int nRet = IDVR_OK;

	if (NULL == pszData || nBufLen <= 0)
	{
		NET_ERR("Param invalid\n");
		return -1;
	}

    if (1 != bSupportWIFI)
    {
        return -1;
    }
	snprintf(pszData, nBufLen, "%s", m_szSessionID);
	return nRet;
}

//UDP方式推送DID给客户端
int INet::UdpPushDid()
{
	int nRet = 0;
	int nResult = 0;
	TutkCfg stTutkCfg = {0};
	unsigned long ulTutkStatus = 0;
	char *pTmp = NULL;
	char szStrTmp[512] = {0};
	char szStrTmp2[64] = {0};
	static int s_sockfd = -1;
	static int nSendToFaildCnt = 0;
	struct sockaddr_in addr = {0};

	XTRACE_MUTEX_LOCK(&m_muUdpPushLock);

	//线程需要强制退出
	if (m_bCheck_exit)
	{
		NET_DBG("stop UdpPushDid\n");
		m_nUdpPushStatus = 0;
		if (s_sockfd >= 0)
		{
			close(s_sockfd);
			s_sockfd = -1;
		}

		nResult = 1;
		goto UdpPushFuncEnd;
	}

	if (2 == m_nUdpPushStatus)	//UDP推送环境就绪
	{
		if (s_sockfd < 0)
		{
			//创建socket对象
		    s_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
			if (s_sockfd < 0)
			{
				NET_ERR("socket create faild:sockfd=%d\n", s_sockfd);
				nResult = -1;
				goto UdpPushFuncEnd;
			}

			nSendToFaildCnt = 0;
		}
	    //创建网络通信对象
		bzero(&addr, sizeof(addr));
	    addr.sin_family = AF_INET;
	    addr.sin_port = m_u32ClientPort;
	    addr.sin_addr.s_addr = m_u32ClientIp;
		
		memset(szStrTmp, 0, sizeof(szStrTmp));
		NET_GetTutkUID(&stTutkCfg, &ulTutkStatus);
		pTmp = strchr(stTutkCfg.szTutkUID, ',');
		if (NULL != pTmp)
		{
			memcpy(szStrTmp2, stTutkCfg.szTutkUID, (int)(pTmp-stTutkCfg.szTutkUID));
		}
		else
		{
			strncpy(szStrTmp2, stTutkCfg.szTutkUID, sizeof(szStrTmp2) - 1);
		}
		if (strlen(szStrTmp2) <= 0)
		{
			NET_ERR("TutkUID len:%d\n", strlen(szStrTmp2));
			nResult = -1;
			goto UdpPushFuncEnd;
		}
		snprintf(szStrTmp, sizeof(szStrTmp), "[P2P]S=%s&DID=%s", 
			m_szSessionID, szStrTmp2);
		
		nRet = sendto(s_sockfd, szStrTmp, strlen(szStrTmp)+1, 
			0, (struct sockaddr*)&addr, sizeof(addr));

		if (nRet != ((int)(strlen(szStrTmp)+1)))
		{
			NET_ERR("sendto err:%d\n", nRet);
			//如果连续出错x次
			//则直接结束本次推送
			nSendToFaildCnt++;
			if (nSendToFaildCnt >= 1000)
			{
				//标志推送完成并退出推送线程
				m_nUdpPushStatus = 0;
				//关闭socket
				if (s_sockfd >= 0)
				{
					close(s_sockfd);
					s_sockfd = -1;
				}

				nSendToFaildCnt = 0;
				nResult = 1;
				goto UdpPushFuncEnd;
			}
			else
			{	
				nResult = -1;
				goto UdpPushFuncEnd;
			}
		}
		else
		{
			//成功一次则重置计数
			nSendToFaildCnt = 0;
		}

		m_nUdpPushCnt++;
		if (m_nUdpPushCnt >= 30)
		{
			//标志推送完成并退出推送线程
			m_nUdpPushStatus = 0;
			//关闭socket
			if (s_sockfd >= 0)
			{
				close(s_sockfd);
				s_sockfd = -1;
			}

			nSendToFaildCnt = 0;
			nResult = 1;
			goto UdpPushFuncEnd;
		}
	}
	else
	{
		//可能网络发生改变
		//这里关闭后续重建
		if (s_sockfd >= 0)
		{
			close(s_sockfd);
			s_sockfd = -1;
		}

		nResult = -1;
		goto UdpPushFuncEnd;
	}

UdpPushFuncEnd:
	XTRACE_MUTEX_UNLOCK(&m_muUdpPushLock);
	return nResult;
}

static void *UdpPushThread(void *lParam)
{
    PRT_TID();
	int nRet = 0;
	INet *pNet = (INet *)lParam;

	if (NULL == pNet)
	{
		NET_ERR("NULL==pNet\n");
		return NULL;
	}
	
	while (1)
	{
		nRet = pNet->UdpPushDid();
		if (-1 == nRet)
		{
			//需要重试
			SelectSleepMSec(50);
			continue;
		}
		else if (0 == nRet)
		{
			//继续下一次
			SelectSleepSec(2);
			continue;
		}
		else
		{
			//推送完成
			break;
		}
    }
	
    return NULL;
}

//设置配网客户端地址信息
int INet::WLAN_SetClientAddrInfo(unsigned int u32Ip, unsigned int u32Port)
{
	int nRet = 0;
	XTRACE_MUTEX_LOCK(&m_muUdpPushLock);
	if (0 == m_nUdpPushStatus)	//上次推送线程已经退出,重新创建
	{
		nRet = pthread_create(&m_hUdpPushThread, NULL, UdpPushThread, this);
	    if (nRet < 0)
	    {
	        NET_ERR("pthread_create<Upnp_CheckThread> failed(%d)\n", nRet);
			XTRACE_MUTEX_UNLOCK(&m_muUdpPushLock);
	        return -1;
	    }
		m_nUdpPushStatus = 1;	//标志后续需要进行UDP推送
		m_u32ClientIp = u32Ip;
		m_u32ClientPort = u32Port;
		m_nUdpPushCnt = 0;		//重置计数		
	}
	else	//上次推送线程还未退出,继续使用
	{
		m_nUdpPushStatus = 1;	//标志后续需要重新进行UDP推送
		m_u32ClientIp = u32Ip;
		m_u32ClientPort = u32Port;
		m_nUdpPushCnt = 0;		//重置计数		
	}
	XTRACE_MUTEX_UNLOCK(&m_muUdpPushLock);
	return 0;
}

/* 获取当前配网模式是常规模式还是SD卡配网模式 */
int INet::NET_GetCurrWifiCfgMode(int *pCurrWifiCfgMode)
{
    *pCurrWifiCfgMode = m_wifiSdCfgMode;
    return 0;
}

/* 获取当前wifi是STA模式还是AP模式*/
int INet::NET_GetCurrWifiMode(int *pCurrWifiMode)
{
    *pCurrWifiMode = m_wifiMode;
    return 0;
}

int INet::WLAN_SetWifiLinkStatus(int nNewStatus)
{
	m_nWifiLinkStatus = nNewStatus;
	return 0;
}

//设置wifi配网状态:0-等待配网 1-连接wifi中 2-wifi连接成功(wifi连接失败，重新返回等待配网)
int INet::WLAN_GetWifiLinkStatus(int *pnStatus)
{
	if (NULL == pnStatus)
	{
		return -1;
	}

	*pnStatus = m_nWifiLinkStatus;
	return 0;
}

int INet::NET_SetWlanScanStatue(int *pWlanScanStatus)
{
	if(NULL == pWlanScanStatus)
	{
		NET_ERR("NULL==pWlanScanStatus\n");
		return -1;
	}
	m_iWlanScanStatus = *pWlanScanStatus;
	return 0;
}

int INet::NET_GetWlanScanStatue()
{
	return m_iWlanScanStatus;
}
#endif

int INet::WLAN_GetDhcpEnable()
{
	return m_bWlan_DHCP_Enable;
}

int INet::WLAN_SetDhcpEnable(int bEnable)
{
#if defined (__IS_SUPPORT_WLAN__)
    if (bSupportWIFI)
    {
        WLAN_Enable_Check(m_bWlan_Enable);
        NET_DBG("---WLAN_SetDhcpEnable---bEnable=%d\n", bEnable);
        NET_DBG("my add: [%s:%d] WLAN_SetDhcpEnable=%d\n", __FUNCTION__, __LINE__, bEnable);

    	if (bEnable)
    	{
    		//之前为静态，则开启
    		if (m_bWlan_DHCP_Enable == 0)
    		{
    			m_bWlan_DHCP_Enable = 1;
    			WLAN_ReActive();
    		}
        }
        else
        {
            if (m_bWlan_DHCP_Enable != 0)
            {
    			enableDhcpOperation(0, IWNAME);
                m_bWlan_DHCP_Enable = 0;
                //重激活wlan0
                WLAN_ReActive();
            }

    	}
    	//写配置
    	if (m_pCConfig->SetValue(INet_CfgSection, INet_WLANDHCP_key, m_bWlan_DHCP_Enable) < 0)
    	{
    		NET_ERR("SetValue failed\n");
    		return -1;
    	}
    	saveWlanDhcpBak(m_bWlan_DHCP_Enable);
    }
#endif
	return 0;
}

int INet::WLAN_RenewDhcp()
{
#if defined (__IS_SUPPORT_WLAN__)
    if (bSupportWIFI)
    {
    	static unsigned int wlanDHCPFailedTimes = 0;

        WLAN_Enable_Check(m_bWlan_Enable);

        /* 在AP模式下面不需要进行DHCP */
        if (SOFT_AP_MODE == m_wifiMode)
        {
            return -1;
        }

        /* 如果当前wifi网络没有连接上就不需要进行DHCP */
        if (0 != m_iWlan_Status)
        {
            return -1;
        }

        if (m_bWlan_DHCP_Enable)
        {
    		/* 如果当前设备不是wlan0，则不去获取DHCP */
    	    if (3 != m_nCurrNetCardType)
    	    {
    	    	NET_INFO("Don't do wlan0 dhcp, because is eth0!\n");
    	        return 0;
    	    }

            int result;
    		FILE *fp;
            int bPDNSGet;
            char strGw[20] = {0};
            char strDns0[20] = {0};
            char strDns1[20] = {0};
            char line_buffer[256] = {0};
    		char *pstr;

            //保存DHCP获取的IP地址,静态与动态不做区分
            m_iWlan_DHCP_Status = 2;//正在获取DHCP

            /* 由于现在采用了使用了DHCPC续租功能，会出现DHCPC程序一直在后台执行
             * 故以前的udhcpc已经启动这部分逻辑需要去掉，否则系统永远不会执行dhcpc命令
             */
    		enableDhcpOperation(1, IWNAME);
    		if(m_iWlan_DHCPFailed == 1 && m_iWlan_Status == 0)
    		{
    			m_iWlan_DHCPFailed = 0;
    		}
            //完了后读配置
            fp = fopen(WLAN_GW_ADDR, "r");
            if (fp == NULL)
            {
                NET_DBG("fopen<%s> failed\n", WLAN_GW_ADDR);
                wlanDHCPFailedTimes++;
            	m_iWlan_DHCPFailed = 1;
                NET_DBG("------------wlanDHCPFailedTimes=%d\n", wlanDHCPFailedTimes);
                // 当wifi连接成功之后且连续十次获取不到ip地址则会重新开关wifi配置使能用以降低dhcp获取不到的问题
                if(10 == wlanDHCPFailedTimes)
                {
                    wifi_reset_enable();
                    //wlanDHCPFailedTimes = 0;// 这样会每6次执行一下
                }
            }
            else
            {
                wlanDHCPFailedTimes = 0;
                if (NULL != fgets(strGw, sizeof(strGw), fp))
                {                    //读到了
    				PubFunc_deleteNewlineInString(strGw, sizeof(strGw));
                    //检查IP是否合法
                    if (!IsValidIP(strGw))
                    {
                        NET_ERR("invalid ip =%s\n", strGw);
                    }
                    else if (m_bWlan_DHCP_Enable)
                    {
    					
                        TruncateIPString(strGw);
                        result = set_if_address((char *) IWNAME, strGw, SET_GATEWAY);
                        if (result)
                        {
                            NET_ERR("set_if_address<SET_GATEWAY> fail(%d), Gateway = %s\n", result, strGw);
                        }
                        m_iWlan_DHCPFailed = 0;
#if 1
                        //保存DHCP获取的IP地址,静态与动态不做区分
                        char ipadd[20] = {0};
                        char gateway[20] = {0};
                        char netmask[20] = {0};
    					
                        WLAN_GetIPAddress(ipadd);
                        WLAN_GetGateway(gateway, sizeof(gateway));
                        WLAN_GetSubnetMask(netmask);
                        
                        XTRACE_MUTEX_LOCK(&m_writeLock);
                        m_pCConfig->SetValue(INet_CfgSection, INet_WLANIP_key, ipadd);
                        strncpy(m_sWlan_LocalIP, ipadd, sizeof(m_sWlan_LocalIP) - 1);
    					saveWlanIpBak(ipadd);
    					
                        m_pCConfig->SetValue(INet_CfgSection, INet_WLANGateway_key, gateway);
                        strncpy(m_sWlan_GateWay, gateway, sizeof(m_sWlan_GateWay) - 1);
    					saveWlanGateWayBak(gateway);
                        m_pCConfig->SetValue(INet_CfgSection, INet_WLANSubnetMask_key, netmask);
                        strncpy(m_sWlan_SubNetMask, netmask, sizeof(m_sWlan_SubNetMask) - 1);
    					saveWlanSubMaskBak(netmask);
    					
                        XTRACE_MUTEX_UNLOCK(&m_writeLock);
                        m_pCConfig->SaveFile();
#endif
                        /* DHCP获取到了之后，需要操作一下路由 */
                        bNeedUpdateRouter = TRUE;
    				}
                }
    			
    			fclose(fp);
    			fp = NULL;
    		}

    		if(m_iWlan_DHCPFailed)
    		{
    			m_iWlan_DHCP_Status = 3;//获取DHCP失败，继续获取
    		}
    		else
    		{
    			m_iWlan_DHCP_Status = 1;//获取DHCP成功
    		}
    		//读DNS
    		fp = fopen(RESOLV_WLAN_CONF_FILE_PATH, "r");
    		if (fp == NULL)
    		{
    			NET_INFO("fopen<%s> failed\n", RESOLV_WLAN_CONF_FILE_PATH);
    		}
    		else if(0 == m_iWlan_DHCPFailed)
    		{
    			bPDNSGet = 0;
    			memset(strDns0, 0, sizeof(strDns0));
    			memset(strDns1, 0, sizeof(strDns1));
    			memset(line_buffer, 0, sizeof(line_buffer));
    			pstr = fgets(line_buffer, sizeof(line_buffer), fp);
    			while ((!feof(fp)) && (NULL != pstr))
    			{
    				//Skip Comment Line
    				if (strstr(line_buffer, "#") != NULL)
    				{
    				}
    				else
    				{
    					if (bPDNSGet == 0)
    					{
    						if (sscanf(line_buffer, "nameserver %s", strDns0))
    						{
    							bPDNSGet = 1;
    						}
    					}
    					else
    					{
    						sscanf(line_buffer, "nameserver %s", strDns1);
    						NET_DBG("while exit\n");
    						break;
    					}
    				}
    				
    				memset(line_buffer, 0, sizeof(line_buffer));
    				pstr = fgets(line_buffer, sizeof(line_buffer), fp);
    			}
    			fclose(fp);

    			//若获取失败,则使用网关作为DNS
    			if (!IsValidIP(strDns0))
    			{
    				memset(strDns0, 0, sizeof(strDns0));
    				snprintf(strDns0, sizeof(strDns0), "%s", strGw);
    			}
    			if (!IsValidIP(strDns1))
    			{
    				memset(strDns1, 0, sizeof(strDns1));
    				snprintf(strDns1, sizeof(strDns1), "%s", WLAN_DEFAULT_DNS);
    			}
    			
    			if (m_iWlan_DHCP_Status==1)
    			{
    				s_NetCardDhcpInfo.s_Wlan0DhcpSuccFlag = 1;
    				s_NetCardDhcpInfo.s_Wlan0DhcpSuccTime = 1; // 防止check 过慢
    				WLAN_SetDNS(strDns0, strDns1);
    			}
    		}
    	}
	}
#endif
	return 0;
}

char *INet::WLAN_GetIPAddress(char *pszIPAddress, int addressSize)
{
#if defined (__IS_SUPPORT_WLAN__)
    if (bSupportWIFI)
    {
        int ret = 0;

    	if (NULL == pszIPAddress)
    	{
    		NET_ERR("NULL==pszIPAddress\n");
    		return NULL;
    	}

    	if (2 == wifi_get_enable())
    	{
    		NET_ERR("no wifi dev\n");
    		return NULL;
    	}

       ret = get_if_address((char *) IWNAME, (char *) pszIPAddress, addressSize, GET_ADDRESS);
        if (ret)
        {
            *pszIPAddress = '\0';
            NET_ERR("Get ipaddress failed (%d) %s\n", ret, strerror(errno));
            return NULL;
        }
    }
    else
    {
        return NULL;
    }
#endif
	return pszIPAddress;
}

int INet::WLAN_ReNewWpaCli()
{
#if defined (__IS_SUPPORT_WLAN__)
    if (bSupportWIFI)
    {
    	/* 关闭无线的DHCP --手动需改 无线IP，就关闭DHCPc */
    	if (WLAN_GetDhcpEnable())
    	{
    		enableDhcpOperation(0, IWNAME);
    	}
    	/* 获取wifi模块是不是正常的 0：设备配置文件异常 1: 正常  2：驱动加载异常  */
    	if (1 == WLAN_GetWiFiModuleEnable())
    	{
    		if (WLAN_STA_MODE == m_wifiMode)  
    		{
    			/* 关 */
    			mysystem("wpa_cli disable_network 0");
    			/* 开 */
    			mysystem("wpa_cli enable_network 0");
        	}
    	}
	}
#endif
	return 0;
}

int INet::WLAN_SetIPAddress(const char *pszIPAddress)
{
#if defined (__IS_SUPPORT_WLAN__)
    if (bSupportWIFI)
    {
        WLAN_Enable_Check(m_bWlan_Enable);

        int ret = 0;

    	//不管DHCP是否打开，均认为是设静态IP
    	if (NULL == pszIPAddress)
    	{
    		NET_ERR("NULL == pszIPAddress\n");
    		return -1;
    	}

    	//检查IP是否合法
    	if (!IsValidIP(pszIPAddress))
    	{
    		NET_ERR("invalid ip =%s\n", pszIPAddress);
    		return -1;
    	}

        //检查IP地址最后一位是否是0或者255
        if (!IsValidIpAddr(pszIPAddress))
        {
            NET_ERR("invalid ip = %s\n", pszIPAddress);
            return -1;
        }

        int first = atoi(pszIPAddress);
        if((0 == first) || (127 == first) || (first > 223))
        {
            NET_ERR("ip first is invalid: %s\n", pszIPAddress);
            return -1;
        }

    	if (1 != wifi_get_enable())
    	{
    		NET_ERR("wifi disabled\n");
    		return -1;
    	}

        //判断IP是否更新
        if (strncmp(m_sWlan_LocalIP, pszIPAddress, 16) == 0)
        {
            NET_INFO("The same IP(%s) set\n", pszIPAddress);
            return 0;
        }
        ret = set_if_address((char *) IWNAME, (char *) pszIPAddress, SET_ADDRESS);
        if (ret)
        {
            NET_ERR("Set ipaddress failed (%d) pszIPAddress =%s \n", ret, pszIPAddress);
            return -1;
        }
        strncpy(m_sWlan_LocalIP, pszIPAddress, sizeof(m_sWlan_LocalIP) - 1);

        m_pCConfig->SetValue(INet_CfgSection, INet_WLANIP_key, m_sWlan_LocalIP);
        saveWlanIpBak(m_sWlan_LocalIP);

    	WLAN_ReNewWpaCli();
    }
#endif
    return 0;
}

char *INet::WLAN_GetSubnetMask(char *pszSubnetMask ,int SubnetMaskSize)
{
#if defined (__IS_SUPPORT_WLAN__)
    if (bSupportWIFI)
    {
    	int ret;
    	if (NULL == pszSubnetMask)
    	{
    		NET_ERR("NULL==pszSubnetMask\n");
    		return NULL;
    	}
    	if (2 == wifi_get_enable())
    	{
    		return NULL;
    	}
        ret = get_if_address((char *) IWNAME, (char *) pszSubnetMask, SubnetMaskSize, GET_NETMASK);
        if (ret)
        {
            *pszSubnetMask = '\0';
            NET_ERR("Get SubnetMask failed (%d) \n", ret);
            return NULL;
        }
    }
    else
    {
        return NULL;
    }
#endif
	return pszSubnetMask;
}

int INet::WLAN_SetSubnetMask(const char *pszSubnetMask)
{
	WLAN_Enable_Check(m_bWlan_Enable);
	return 0;
}

char *INet::WLAN_GetGateway(char *pszGateway, int GatewaySize)
{
#if defined (__IS_SUPPORT_WLAN__)
    if (bSupportWIFI)
    {
        int ret=0;
        //NET_DBG("\n...INet.cpp.....INet::WLAN_GetGateway()....begin.....\n");
        if (2 == wifi_get_enable())
        {
            return NULL;
        }

    	if (NULL == pszGateway)
    	{
    		NET_ERR("NULL == pszGateway\n");
    		return NULL;
    	}

        if(SOFT_AP_MODE == m_wifiMode)
            strncpy(pszGateway, g_szDefaultApGateway, GatewaySize - 1);
        else
            ret = get_if_address((char *) IWNAME, pszGateway, GatewaySize, GET_GATEWAY);
        if (ret)
        {
            *pszGateway = '\0';
            TruncateIPString(pszGateway);
            NET_INFO("set_if_address<GET_GATEWAY> fail(%d)\n", ret);
            return NULL;
        }
    }
    else
    {
        return NULL;
    }
#endif
	return pszGateway;
}

int INet::WLAN_SetGateway(const char *pszGateway)
{
	WLAN_Enable_Check(m_bWlan_Enable);
#if defined (__IS_SUPPORT_WLAN__)
	if (bSupportWIFI)
    {
		int ret;
		NET_DBG("\n...INet.cpp......INet::WLAN_SetGateway()....begin...pszGateway=%s..\n", pszGateway);
		if (1 != wifi_get_enable())
		{
			NET_ERR("wifi disabled\n");
			return -1;
		}

	    //不管DHCP是否打开，均认为是设静态
	    if (NULL == pszGateway)
	    {
	        NET_ERR("NULL == pszGateway\n");
	        return -1;
	    }
	    //检查IP是否合法
	    if (!IsValidIP(pszGateway))
	    {
	        NET_ERR("nvalid ip =%s\n", pszGateway);
	        return -1;
	    }
	    
	    //判断网关是否更新
	    if (strncmp(m_sWlan_GateWay, pszGateway, 20) == 0)
	    {
	        NET_INFO("The same Gateway(%s) set\n", pszGateway);
	        return 0;
	    }

		ret = set_if_address((char *) IWNAME, (char *) pszGateway, SET_GATEWAY);
		if (ret)
		{
			NET_ERR("set_if_address<SET_GATEWAY> fail(%d), Gateway = %s\n", ret, pszGateway);
			return -1;
		}

		strncpy(m_sWlan_GateWay, pszGateway, sizeof(m_sWlan_GateWay) - 1);
		m_pCConfig->SetValue(INet_CfgSection, INet_WLANGateway_key, m_sWlan_GateWay);
	  	saveWlanGateWayBak(m_sWlan_GateWay);
	}
	else
	{
		return -1;
	}
#endif
	return 0;
}

char *INet::WLAN_GetMacAddress(char *pszMacAddress, int MacAddressSize)
{
    static int isFirstCall = 1;  // 静态变量，标记是否是第一次调用
    static unsigned char szMac[MAC_ADDR_LEN] = {0};  // 静态变量保存第一次获取的 MAC 地址

    if (!bSupportWIFI)
    {
        return NULL;
    }

    if (NULL == pszMacAddress)
    {
        NET_ERR("NULL==pszMacAddress\n");
        return NULL;
    }

    if (2 == wifi_get_enable())
    {
        return NULL;
    }

    int num= 0;

    // 如果是第一次调用，获取 MAC 地址
    if (isFirstCall)
    {
        int ret = 0;
        unsigned int mac0, mac1, mac2, mac3, mac4, mac5;
        stuShmAuthInfo auth_info;
        memset(&auth_info, 0, sizeof(stuShmAuthInfo));

        unsigned int nAuthState = 0;
        unsigned char szAuthCode[256] = {0};

        ZWSys_GetAuthInfo(szAuthCode, &nAuthState);
        if (!nAuthState)
        {
            ret = get_if_address((char *) "wlan0", (char *) pszMacAddress, MacAddressSize, GET_MAC);
            if (ret)
            {
                NET_ERR("Set MacAddress failed (%d) pszMacAddress =%s m_wifiMode = %d\n", ret, pszMacAddress, m_wifiMode);
                return NULL;
            }
            else
            {
                // 保存第一次获取到的 MAC 地址到静态变量 szMac
                sscanf(pszMacAddress, "%02x:%02x:%02x:%02x:%02x:%02x", &mac0, &mac1, &mac2, &mac3, &mac4, &mac5);
                szMac[0] = mac0;
                szMac[1] = mac1;
                szMac[2] = mac2;
                szMac[3] = mac3;
                szMac[4] = mac4;
                szMac[5] = mac5;
                
                isFirstCall = 0;  // 标记第一次调用已经完成
                return pszMacAddress;
            }
        }
        else
        {
            Authorize_GetAuthDevInfo(&auth_info);
            snprintf(pszMacAddress, MAX_MAC_STR_LENGTH, "%02x:%02x:%02x:%02x:%02x:%02x",
                (unsigned char)auth_info.szMac[0],
                (unsigned char)auth_info.szMac[1],
                (unsigned char)auth_info.szMac[2],
                (unsigned char)auth_info.szMac[3],
                (unsigned char)auth_info.szMac[4],
                (unsigned char)auth_info.szMac[5]);

            // 保存第一次获取到的 MAC 地址到静态变量 szMac
            for (num= 0; num < MAC_ADDR_LEN; ++num)
            {
                if (szMac[num] != auth_info.szMac[num])
                {
                    szMac[num] = auth_info.szMac[num];
                }
            }

            isFirstCall = 0;  // 标记第一次调用已经完成
            return pszMacAddress;
        }
    }
    else
    {
        // 如果不是第一次调用，直接将保存的 MAC 地址返回
        snprintf(pszMacAddress, MAX_MAC_STR_LENGTH, "%02x:%02x:%02x:%02x:%02x:%02x",
            szMac[0], szMac[1], szMac[2], szMac[3], szMac[4], szMac[5]);

        return pszMacAddress;
    }

}

int INet::WLAN_SetDNS(char *PrimaryDNS, char *SecondaryDNS)
{
#if defined (__IS_SUPPORT_WLAN__)
	char tmpWlanGateWay[20]={0};

	if (1 != bSupportWIFI)
    {
        return -1;
    }

    NET_DBG("\n...INet.cpp\n...INet::WLAN_SetDNS()....\n");
    if (1 != wifi_get_enable())
    {
        NET_ERR("wifi disabled\n");
        return -1;
    }

	if (NULL == PrimaryDNS)
	{
		NET_ERR("NULL == PrimaryDNS\n");
		return -1;
	}
	if (!IsValidIP(PrimaryDNS))
	{
		NET_ERR("invalid PrimaryDNS = %s\n", PrimaryDNS);
		return -2;
	}
	if (NULL != SecondaryDNS && (!IsValidIP(SecondaryDNS)))
	{
		NET_ERR("invalid SecondaryDNS = %s\n", SecondaryDNS);
		return -3;
	}

	strncpy(m_sWlanPrimaryDNS, PrimaryDNS, sizeof(m_sWlanPrimaryDNS) - 1);
	m_pCConfig->SetValue(IPPPOE_CfgSection, INet_WLANDNS0_key, m_sWlanPrimaryDNS);

    if (NULL != SecondaryDNS)
    {
        strncpy(m_sWlanSecondaryDNS, SecondaryDNS, sizeof(m_sWlanSecondaryDNS) - 1 );
    }
    m_pCConfig->SetValue(IPPPOE_CfgSection, INet_WLANDNS1_key, m_sWlanSecondaryDNS);
    m_pCConfig->SaveFile();

	// 如果当前是无线网卡 才更新dns 配置文件
	if (3 == m_nCurrNetCardType)
	{
		WLAN_GetGateway(tmpWlanGateWay);
		NET_INFO("WLAN_SetDNS [dnstest] [write] wlan0 update dnsfile gateway :%s \n", tmpWlanGateWay);
		WriteDNSFile(PrimaryDNS, SecondaryDNS, tmpWlanGateWay);
	}

    //设备更新网络后，如果路由器的网段不一致，会引起域名解析失效    
    res_init();

#ifdef USE_HOSTS_PARSER
    /* 更新hosts */;
    int result = 0;
    result = Hosts_Update();
    if (result)
    {
		NET_ERR("Fail to update the hosts, errno = %d\n", result);
    }
#endif
	/*DNS改变后调用res_init 防止域名解析失败*/
	res_init();
	
#endif
	return 0;
}

// 要加锁
int INet::ETH0_SetDNS(char *PrimaryDNS, char *SecondaryDNS)
{
	int result = 0;
	int CurrNetCardType = 1;
	char tmpGateWay[20]={0};

	if (NULL == PrimaryDNS)
	{
		NET_ERR("NULL == PrimaryDNS\n");
		return -1;
	}
	if (!IsValidIP(PrimaryDNS))
	{
		NET_ERR("invalid PrimaryDNS = %s\n", PrimaryDNS);
		return -2;
	}
	if (NULL != SecondaryDNS && (!IsValidIP(SecondaryDNS)))
	{
		NET_ERR("invalid SecondaryDNS = %s\n", SecondaryDNS);
		return -3;
	}

	// dhcp 获取到的dns 和 协议下发的dns 可能存在冲突，需要加锁处理
	pthread_mutex_lock(&s_NetCardDhcpInfo.SetDNSFileLock);
	
	strncpy(m_sPrimaryDNS, PrimaryDNS, 20);
	m_pCConfig->SetValue(IPPPOE_CfgSection, IPPPOE_PrimaryDNS_key, m_sPrimaryDNS);

	if (NULL != SecondaryDNS)
	{
		strncpy(m_sSecondaryDNS, SecondaryDNS, 20);
		m_pCConfig->SetValue(IPPPOE_CfgSection, IPPPOE_SecondaryDNS_key, m_sSecondaryDNS);
	}

	m_pCConfig->SaveFile();
	
	NET_INFO("ETH0_SetDNS [dnstest] PrimaryDNS: %s SecondaryDNS: %s \n", PrimaryDNS, SecondaryDNS);

	NET_GetCurrNetCardType(&CurrNetCardType);
	//NET_ERR("[dnstest] current use %s \n",  (1==CurrNetCardType)?"eth0":"wlan0");
	// 当前网卡类型是有线
	if (1 == CurrNetCardType)
	{
		// 只有当前是有线才设置dns 到配置文件中去, 否则只会更新配置文件		
		NET_GetGateway(tmpGateWay);
		NET_INFO("ETH0_SetDNS [dnstest] [write]  eth0 dns file, tmpGateWay: %s \n", tmpGateWay);
		WriteDNSFile(PrimaryDNS, SecondaryDNS, tmpGateWay);
	}
	pthread_mutex_unlock(&s_NetCardDhcpInfo.SetDNSFileLock);

    //设备更新网络后，如果路由器的网段不一致，会引起域名解析失效    
    res_init();

#ifdef USE_HOSTS_PARSER
    result = Hosts_Update();
    if (result)
    {
		NET_ERR("Fail to update the hosts, errno = %d\n", result);
    }
#endif

	/*DNS改变后调用res_init 防止域名解析失败*/
	res_init();

	return 0;
}

int INet::NETCARD_GetDhcpSuccTime(int *nEth0DhcpSuccTime, int *nWlan0DhcpSuccTime)
{
	if (nEth0DhcpSuccTime)
	{
		if (1 == s_NetCardDhcpInfo.s_Eth0DhcpSuccFlag)
		{
			*nEth0DhcpSuccTime = s_NetCardDhcpInfo.s_Eth0DhcpSuccTime;
		}
		else
		{
			*nEth0DhcpSuccTime = 0;
		}
	}

	if (nWlan0DhcpSuccTime)
	{
		if (1 == s_NetCardDhcpInfo.s_Wlan0DhcpSuccFlag)
		{
			*nWlan0DhcpSuccTime = s_NetCardDhcpInfo.s_Wlan0DhcpSuccTime;
		}
		else
		{
			*nWlan0DhcpSuccTime = 0;
		}
	}

	return 0;
}

int INet::ETH0_GetDNS(char *PrimaryDNS, char *SecondaryDNS)
{
	FILE *fp = NULL;;
	int bPDNSGet = 0;
	char strDns0[20] = {0};
	char strDns1[20] = {0};
	char line_buffer[256] = {0};
	char *pstr = NULL;

	if (NULL == PrimaryDNS)
	{
		if (NULL != SecondaryDNS)
		{
			strncpy(SecondaryDNS, DEFAULT_RESOLV_CONF_LOCAL, 20);
		}

		NET_ERR("NULL == PrimaryDNS\n");
		return -1;
	}

	//优先从变量中读取
	//避免频繁访问文件
	if (IsValidIP(m_sPrimaryDNS) && IsValidIP(m_sSecondaryDNS))
	{
		//NET_INFO("[dnstest] eth0 getdns m_sPrimaryDNS: %s m_sSecondaryDNS: %s \n", m_sPrimaryDNS, m_sSecondaryDNS);
		strncpy(PrimaryDNS, m_sPrimaryDNS, 20);
		if (NULL != SecondaryDNS)
		{
			strncpy(SecondaryDNS, m_sSecondaryDNS, 20);
		}

		return 0;
	}
	
	//读DNS配置
	fp = fopen(RESOLV_ETH0_CONF, "r");
	NET_INFO("ETH0_GetDNS [dnstest] get eth0 dns cfg file ? xxx\n");
	if (NULL == fp)
	{
		strncpy(PrimaryDNS, DEFAULT_RESOLV_CONF, 20);

		if (NULL != SecondaryDNS)
		{
			strncpy(SecondaryDNS, DEFAULT_RESOLV_CONF_LOCAL, 20);
		}

		NET_ERR("NULL == fp\n");
		return -2;
	}
	else
	{
		bPDNSGet = 0;
		strDns0[0] = 0;
		strDns1[0] = 0;

		memset(line_buffer, 0, 256);
		pstr = fgets(line_buffer, 256, fp);

		while ((!feof(fp)) && (NULL != pstr))
		{
			//Skip Comment Line
			if (strstr(line_buffer, "#") != NULL)
			{

			}
			else
			{
			    if(strstr(line_buffer, "nameserver") != NULL)
			    {
    				if (bPDNSGet == 0)
    				{
    					sscanf(line_buffer, "nameserver %s", strDns0);
    					bPDNSGet = 1;
    				}
    				else
    				{
    					
    					sscanf(line_buffer, "nameserver %s", strDns1);
    					break;
    				}
				}
			}

			memset(line_buffer, 0, 256);

			pstr = fgets(line_buffer, 256, fp);
		}

		fclose(fp);

		if (IsValidIP(strDns0))
		{
			strncpy(PrimaryDNS, strDns0,20);
		}
		else
		{
			strncpy(PrimaryDNS, DEFAULT_RESOLV_CONF_LOCAL, 20);
		}

		if (NULL != SecondaryDNS)
		{
			if (IsValidIP(strDns1))
			{
				strncpy(SecondaryDNS, strDns1,20);
			}
			else
			{
				strncpy(SecondaryDNS, DEFAULT_RESOLV_CONF_LOCAL, 20);
			}
		}
	}

	return 0;
}

int INet::WLAN_GetDNS(char *PrimaryDNS, char *SecondaryDNS ,int DnsSize)
{
#if defined (__IS_SUPPORT_WLAN__)
	if (1 != bSupportWIFI)
    {
        return -1;
    }

	if (2 == wifi_get_enable())
	{
		return -1;
	}

	FILE *fp = NULL;
	int bPDNSGet = 0;
    char strDns0[20] = {0};
    char strDns1[20] = {0};
    char line_buffer[256] = {0};
	char *pstr = NULL;

	if (NULL == PrimaryDNS)
	{
		if (NULL != SecondaryDNS)
		{
			strncpy(SecondaryDNS, WLAN_SecondaryDNS_Default_s, DnsSize - 1);
		}

		NET_ERR("NULL == PrimaryDNS\n");
		return -1;
	}

	//优先从变量中读取
	//避免频繁访问文件
	if (IsValidIP(m_sWlanPrimaryDNS) && IsValidIP(m_sWlanSecondaryDNS))
	{
		strncpy(PrimaryDNS, m_sWlanPrimaryDNS, sizeof(m_sWlanPrimaryDNS) - 1);
		if (NULL != SecondaryDNS)
	    {
	        strncpy(SecondaryDNS, m_sWlanSecondaryDNS, DnsSize - 1);
	    }

		return 0;
	}

	//读DNS
	fp = fopen(RESOLV_WLAN_CONF_FILE_PATH, "r");
	if (NULL == fp)
	{
		strncpy(PrimaryDNS, WLAN_PrimaryDNS_Default_s, DnsSize - 1);

		if (NULL != SecondaryDNS)
		{
			strncpy(SecondaryDNS, WLAN_SecondaryDNS_Default_s, DnsSize - 1);
		}

		NET_ERR("NULL == fp\n");
		return -2;
	}
	else
	{
		bPDNSGet = 0;
		strDns0[0] = 0;
		strDns1[0] = 0;

		memset(line_buffer, 0, 256);
		pstr = fgets(line_buffer, 256, fp);

		while ((!feof(fp)) && (NULL != pstr))
		{
			//Skip Comment Line
			if (strstr(line_buffer, "#") != NULL)
			{

			}
			else
			{
				if(strstr(line_buffer, "nameserver") != NULL)
                {
    				if (bPDNSGet == 0)
    				{
    					sscanf(line_buffer, "nameserver %s", strDns0);
    					bPDNSGet = 1;
    				}
    				else
    				{
    					sscanf(line_buffer, "nameserver %s", strDns1);
    					break;
    				}
                }            
			}

			memset(line_buffer, 0, 256);

			pstr = fgets(line_buffer, 256, fp);
		}

		fclose(fp);

		if (IsValidIP(strDns0))
		{
			strncpy(PrimaryDNS, strDns0, DnsSize - 1);
		}
		else
		{
			strncpy(PrimaryDNS, WLAN_PrimaryDNS_Default_s, DnsSize - 1);
		}

		if (NULL != SecondaryDNS)
		{
			if (IsValidIP(strDns1))
			{
				strncpy(SecondaryDNS, strDns1, DnsSize - 1);
			}
			else
			{
				strncpy(SecondaryDNS, WLAN_SecondaryDNS_Default_s, DnsSize - 1);
			}
		}
	}
#endif
	return 0;
}

int INet::WLAN_GetDhcpState()
{
	return m_iWlan_DHCP_Status;
}
#if defined (__IS_SUPPORT_WLAN__)

static void *Fnx_SwitchSTAConnectThread(void *lParam)
{
    PRT_TID();
    int ret=0, i=0;
    INet *pnet = (INet *) lParam;
    WLAN_CONNECTION wifiConnection;

    pthread_detach(pthread_self());
    if (NULL == pnet)
    {
        NET_ERR("NULL==pnet\n");
        return NULL;
    }

    memset(&wifiConnection, 0, sizeof(WLAN_CONNECTION));
    //pnet->GetConfig()->GetValue(INet_CfgSection, INet_WLANWPAKey_key, "", wifiConnection.struKeyCfg.WPAKey.szKey, WLAN_MAX_KEY_LEN - 1);
	pnet->WLAN_GetWifiKey(wifiConnection.struKeyCfg.WPAKey.szKey);
    pnet->GetConfig()->GetValue(INet_CfgSection, INet_WLANSSID_key, "", wifiConnection.szSSID, WLAN_MAX_SSID_LEN - 1);
    wifiConnection.struKeyCfg.WPAKey.bValid = 1;
    wifiConnection.struKeyCfg.bValid = 1;

    NET_INFO("wifi_get_conn_info_wpa!\n");
    //驱动刚加载可能还获取不到热点，所以最多尝试25次
    for(i=0; i<25; i++)
    {
        SelectSleepUSec(200000);
        ret = wifi_get_conn_info_wpa(1, wifiConnection.szSSID, &wifiConnection);
        if(!ret)
            break;
    }
    NET_INFO("szSSID:%s, key:%s, nNetworkType=%d, nAuthMode=%d, nEncrypType=%d, try count %d\n", 
        wifiConnection.szSSID, wifiConnection.struKeyCfg.WPAKey.szKey,
        wifiConnection.nNetworkType,
        wifiConnection.nAuthMode, wifiConnection.nEncrypType, i);

    //强制设置不进行参数是否改变的检查
    pnet->WLAN_SetConnection(&wifiConnection, 1);
    //通知从ap切到sta的配置已完成
    pnet->WLAN_setFromApToStaConnectionCfgFlag(1);

    return NULL;
}

int INet::NET_SetWifiMode(unsigned int mode, int force)
{
    if (bSupportWlanAP)
    {
        char command[64] = {0};
        eWifiMode wifi_mode = UNKNOWN_MODE;
		int nCounter = 0;

        if(((SOFT_AP_MODE != (eWifiMode)mode) && (WLAN_STA_MODE != (eWifiMode)mode))
        || ((0 != force)  && (1 != force)))
        {
            NET_ERR("NET_SetWifiMode param error! mode=%d, force=%d\n", mode, force);
            return -1;
        }

        XTRACE_MUTEX_LOCK(&m_SetWifiModeLock);

        //if((m_wifiMode != (eWifiMode)mode) || force)
        {
            memset(command, 0, sizeof(command));
            snprintf(command, sizeof(command), "lsmod");

            //获取当前真实的wifi模式
            if (IsStringInCommand("ps", "hostapd"))
	        {
	            wifi_mode = SOFT_AP_MODE;
	        }
			else if (IsStringInCommand("ps", "wpa_supplicant"))
			{
				wifi_mode = WLAN_STA_MODE;
			}
			else
            {
                wifi_mode = UNKNOWN_MODE;
            }

            NET_INFO("----check---wifi mode change, from %d to %d\n", wifi_mode, mode);
			if(m_wifiFakeSTAModeEnable && (WLAN_STA_MODE == (eWifiMode)mode) && (WLAN_STA_MODE == wifi_mode))
			{
				NET_INFO("Switch to STA mode, clear WLANFakeSTAMode flag!\n ");
				m_wifiFakeSTAModeEnable = 0;
				m_pCConfig->SetValue(INet_CfgSection, INet_WLANFakeSTAMode, m_wifiFakeSTAModeEnable);
			}

            //根据驱动类型来判断最靠谱
            if ((eWifiMode)mode != wifi_mode)
            {
				if(m_wifiFakeSTAModeEnable && (SOFT_AP_MODE == (eWifiMode)mode) && (WLAN_STA_MODE == wifi_mode))
				{
					NET_INFO("Do not respond Switch to AP mode, when WLANFakeSTAMode set!\n ");
					return 0;
				}

                m_pCConfig->SetValue(INet_CfgSection, INet_WLANWifiMode, mode);
				saveWlanWifiModeBak(mode);
                m_wifiMode = (eWifiMode)mode;
                
                //强制打开wifi使能
                m_pCConfig->SetValue(INet_CfgSection, INET_WLANEnable_key, 1);
                m_pCConfig->SaveFile();

				if (SOFT_AP_MODE == (eWifiMode)mode)
				{
					m_changeWifiConnectFlag = 0;//都要切到AP模式了就不用管STA模式下的连接了
					// 切换模式
					LoadSoftApDriver_8189();

					WLAN_BluetoothOpen();
					ZWSys_SetSpeechContent(SPEECH_WIFI_IN_AP_STATUS);
					//循环查询，等待播放结束
					while (1)
					{
						sleep(1);
						if (0 == ZWSys_GetSpeechPlayingStatus())
						{
							break;
						}
						if (++nCounter > SPEECH_PLAY_TIME_OUT_SEC)
						{
							nCounter = 0;
							break;
						}
					}
					//设置wifi状态灯
					ZWSys_SetWifiStatusLed(IN_AP_STATUS);
					memset(command, 0, sizeof(command));
					snprintf(command, sizeof(command), "echo AP > %s", WIFI_MODE_FLAG);
					mysystem(command);
				}
				else if (WLAN_STA_MODE == (eWifiMode)mode)
				{
					// 切换模式
					LoadStaModeDriver_8189();

					//其内部竟会杀掉主进程
					//设置wifi状态灯
					ZWSys_SetWifiStatusLed(IN_WIFI_CONNECTING);   
					memset(command, 0, sizeof(command));
					snprintf(command, sizeof(command), "echo STA > %s", WIFI_MODE_FLAG);
					mysystem(command);

					//处理STA模式下的配置
					pthread_t m_witchStaThread;
					//sta模式会去配置相关连接所以耗时有几秒
					pthread_create(&m_witchStaThread, NULL, Fnx_SwitchSTAConnectThread, this);
				}
                else
				{
					NET_ERR("Unknow mode [%u]\n", mode);
				}
            }
        }

        XTRACE_MUTEX_UNLOCK(&m_SetWifiModeLock);
    }

	if (bSupportWIFI)
    {
	    //设置wifi状态灯
	    if (SOFT_AP_MODE == m_wifiMode)
	    {
	        ZWSys_SetWifiStatusLed(IN_AP_STATUS);
	    }
		else
	    {
	    	ZWSys_SetWifiStatusLed(IN_WIFI_CONNECTING);
		}
	}
    return 0;
}

int INet::WLAN_GetStatus()
{
	return m_iWlan_Status;
}
#endif
int INet::WLAN_ReActive()
{
    WLAN_Enable_Check(m_bWlan_Enable);
#if defined (__IS_SUPPORT_WLAN__)
    NET_DBG("\n...INet.cpp......INet::WLAN_ReActive()....\n");
    int ret = 0;
    char command[64] = {0};

	if (1 != bSupportWIFI)
    {
        return -1;
    }

	if (1 != wifi_get_enable())
	{
		return -1;
	}
	
	//如果是动态IP，则做重获IP处理
	if (m_bWlan_DHCP_Enable)
	{
#if 0
		return NET_RenewDhcp();
#else
		m_iWlan_DHCPFailed = 1;
		return 0;
#endif
	}
	
    snprintf(command, sizeof(command), "rm -rf %s", WLAN_GW_ADDR);
    mysystem(command);
	
    //设置IP
    ret = set_if_address((char *) IWNAME, (char *) m_sWlan_LocalIP, SET_ADDRESS);
    if (ret)
    {
        NET_ERR("Set ipaddress failed (%d) m_sLocalIP =%s \n", ret, m_sWlan_LocalIP);
        return -1;
    }
	//设置子网掩码
	ret = set_if_address((char *) IWNAME, (char *) m_sWlan_SubNetMask, SET_NETMASK);
	if (ret)
	{
		NET_ERR("Set SubnetMask failed (%d) m_sSubNetMask =%s \n", ret, m_sWlan_SubNetMask);
		return -1;
	}
	//设置网关
	ret = set_if_address((char *) IWNAME, m_sWlan_GateWay, SET_GATEWAY);
	if (ret)
	{
		NET_ERR("set_if_address<SET_GATEWAY> fail(%d), Gateway = %s\n", ret, m_sWlan_GateWay);
		return -1;
	}
#endif
    return 0;
}

int INet::WLAN_GetAccessPoints(WLAN_ACCESSPOINT pAccessPoints[WLAN_MAX_AP_NUM], int *pnAPCount)
{
    int ret = 0;
    NET_DBG("----------------WLAN_GetAccessPoints----------------\n");

#if defined (__IS_SUPPORT_WLAN__)
	if (bSupportWIFI)
    {
		ret = wifi_get_accesspoints_wpa(pAccessPoints, pnAPCount);
		if (0 == ret)
		{
			memcpy(m_AccessPoints, pAccessPoints, sizeof(m_AccessPoints));
			g_DistributionNetwordFlag = 1;
		}
	}
#endif
    return ret;
}

int INet::WLAN_GetAccessSignalInfo(LPWLAN_ACCESSSIGNALINFO signal_info)
{
    int ret = 0;
    NET_DBG("----------------WLAN_GetAccessPoints----------------\n");

#if defined (__IS_SUPPORT_WLAN__)
    if (bSupportWIFI)
    {
        ret = wifi_get_signal_info_wpa(signal_info);
    }
#endif
    return ret;
}

int INet::WLAN_GetAccessSignalQuality(int *nQuality)
{
    int ret = 0;

#if defined (__IS_SUPPORT_WLAN__)
    if (bSupportWIFI)
    {
        ret = wifi_get_signal_quality(nQuality);
    }
#endif
    return ret;
}

int INet::WLAN_GetConnection(LPWLAN_CONNECTION pConnection)
{
    memset(pConnection, 0, sizeof(WLAN_CONNECTION));
#if defined (__IS_SUPPORT_WLAN__)
    if((bSupportWIFI) && (SOFT_AP_MODE != m_wifiMode))
    {
        wifi_get_conn_cfg(pConnection);
#ifdef DISABLE_WIFI_CONNECT_WHEN_WIRE
		if((ifWifiNewConnectCfg) && (1 == m_nCurrNetCardType))
		{
			memcpy(pConnection, &m_tWlan_Connection, sizeof(WLAN_CONNECTION));
			pConnection->bConnect = 0;
			/* 上层IE进行一个转换 */
			if (pConnection->nNetworkType == 2)
			{
				pConnection->nNetworkType = 0;
			}else{
				pConnection->nNetworkType = 1;
			}
		}
#endif
		//如果wifi参数即将要改变，则返回wifi的连接状态为未连接，用户一般保存后就获取为了避免歧义，先强制为未连接
		if(m_wifiConnChange)
		{
			pConnection->bConnect = 0;
		}

    }
#endif
    return 0;
}

#if defined (__IS_SUPPORT_WLAN__)
int INet::WLAN_SaveWifiKey(char *key)
{
	if (1 != bSupportWIFI)
    {
        return 0;
    }

#ifdef ENCODE_WIFI_KEY_USE_BASE64
	char base64Key[(WLAN_MAX_KEY_LEN + (3 - 1))/3*4] = {0};

	//做长度判断，每3个字节编码为4个字节，剩下不足的编出来也是4个字节
	//做加密
	NET_INFO("key=%s, len=%d\n", key, strlen(key));
	// Base64_Encode((unsigned char*)base64Key, sizeof(base64Key), (unsigned char*)key, strlen(key));
	key = TBase64::Base64_Encode(base64Key, sizeof(base64Key));
	NET_INFO("encode after:%s\n", base64Key);
	//保存加密后的密码
	m_pCConfig->SetValue(INet_CfgSection, INet_WLANWPAKey_key, base64Key);
	saveWlanWPAKeyBak(WIFI_CFG_INI, 0, base64Key);
	//保存当前密码是否为加密的使能
	m_pCConfig->SetValue(INet_CfgSection, INet_WLAN_KEY_ENCODE_ENABLE, 1);
	saveWlanKeyEncodeBak(WIFI_CFG_INI, 1);
	//必须保证该使能与密码是配套的
#else
	m_pCConfig->SetValue(INet_CfgSection, INet_WLANWPAKey_key, key);
	saveWlanWPAKeyBak(WIFI_CFG_INI, 0, key);
#endif
	m_pCConfig->SaveFile();
	return 0;
}

int INet::WLAN_GetWifiKey(char *key)
{
	#ifdef ENCODE_WIFI_KEY_USE_BASE64
	char base64Key[(WLAN_MAX_KEY_LEN + (3 - 1))/3*4] = {0};
	int encodeEnable = 0;

	//获取密码加密是否使能
	encodeEnable = m_pCConfig->GetValue(INet_CfgSection, INet_WLAN_KEY_ENCODE_ENABLE, 0);
	if(encodeEnable)
	{
		m_pCConfig->GetValue(INet_CfgSection, INet_WLANWPAKey_key, "", base64Key, sizeof(base64Key) - 1);
		// zw_base64_decode((unsigned char*)key, WLAN_MAX_KEY_LEN, (unsigned char*)base64Key, strlen(base64Key));
		int decode_len = WLAN_MAX_KEY_LEN;
		key = TBase64::Base64_Decode(base64Key, decode_len);
	}
	else
	{
		m_pCConfig->GetValue(INet_CfgSection, INet_WLANWPAKey_key, "", key, WLAN_MAX_KEY_LEN - 1);
	}
	#else
	m_pCConfig->GetValue(INet_CfgSection, INet_WLANWPAKey_key, "", key, WLAN_MAX_KEY_LEN - 1);
	#endif

	return 0;
}

int INet::WLAN_getWLANWPAKey(char *szSSID,int *authMode, int *encryptype)
{
	int count = 0;
	if(szSSID == NULL || authMode == NULL || encryptype == NULL)
	{
		NET_ERR("updateConnectionCfg: szSSID is NULL!\n");
		return -1;
	}
	for(count=0; count< WLAN_MAX_AP_NUM; count++)
    {
		// NET_INFO("m_AccessPoints[%d].szSSID:%s\n", count, m_AccessPoints[count].szSSID);
        // NET_INFO("m_AccessPoints[%d].nEncrypType : %d\n", count, m_AccessPoints[count].nEncrypType);
		// NET_INFO("m_AccessPoints[%d].nAuthentication : %d\n", count, m_AccessPoints[count].nAuthentication);
		if(!strcasecmp(szSSID, m_AccessPoints[count].szSSID))
        {
            *authMode = m_AccessPoints[count].nAuthentication;
            *encryptype = m_AccessPoints[count].nEncrypType;
            NET_DBG("find same ap!\n");
            break;
        }
    }
    //未找到
    if(count >= WLAN_MAX_AP_NUM)
    {
        *authMode = 2;
        *encryptype = 3;
    }
    
    return 0;
}

static int WLAN_updateConnectionCfg(LPWLAN_CONNECTION pConnection)
{
    int i = 0;
    int count = 0;

    WLAN_ACCESSPOINT accessPoints[WLAN_MAX_AP_NUM];
    memset(accessPoints, 0, sizeof(accessPoints));

    wifi_get_accesspoints_wpa(accessPoints, &count);
    for(i=0; i<count; i++)
    {
        if(!strcasecmp(pConnection->szSSID, accessPoints[i].szSSID))
        {
            pConnection->nNetworkType = 0;
            pConnection->nAuthMode = accessPoints[i].nAuthentication;
            pConnection->nEncrypType = accessPoints[i].nEncrypType;
            NET_DBG("find same ap!\n");
            break;
        }
    }

    //未找到
    if(i >= WLAN_MAX_AP_NUM)
    {
        pConnection->nNetworkType = 0;
        pConnection->nAuthMode = 2;
        pConnection->nEncrypType = 3;
    }
    
    return 0;
}
#endif

int INet::WLAN_SetConnection(LPWLAN_CONNECTION pConnection,int force)
{
#if defined (__IS_SUPPORT_WLAN__)
	if (1 != bSupportWIFI)
    {
        return 0;
    }

#if 1
NET_INFO("New ===============================\n\
		pConnection->bConnect = %d,\n\
		pConnection->nAuthMode = %d,\n\
		pConnection->nEncrypType = %d,\n\
		pConnection->nNetworkType = %d,\n\
		pConnection->struKeyCfg.bValid = %d,\n\
		pConnection->struKeyCfg.nDefKeyID = %d,\n\
		pConnection->struKeyCfg.Key1.bValid = %d,\n\
		pConnection->struKeyCfg.Key1.szKey = %s,\n\
		pConnection->struKeyCfg.Key2.bValid = %d,\n\
		pConnection->struKeyCfg.Key2.szKey = %s,\n\
		pConnection->struKeyCfg.Key3.bValid = %d,\n\
		pConnection->struKeyCfg.Key3.szKey = %s,\n\
		pConnection->struKeyCfg.Key4.bValid = %d,\n\
		pConnection->struKeyCfg.Key4.szKey = %s,\n\
		pConnection->struKeyCfg.WPAKey.bValid = %d,\n\
		pConnection->struKeyCfg.WPAKey.szKey = %s,\n\
		pConnection->szMacAddress = %s,\n\
		pConnection->szSSID = %s\n",
        pConnection->bConnect,
        pConnection->nAuthMode,
        pConnection->nEncrypType,
        pConnection->nNetworkType,
        pConnection->struKeyCfg.bValid,    
        pConnection->struKeyCfg.nDefKeyID,
        pConnection->struKeyCfg.Key1.bValid,
        pConnection->struKeyCfg.Key1.szKey,
        pConnection->struKeyCfg.Key2.bValid,
        pConnection->struKeyCfg.Key2.szKey,
        pConnection->struKeyCfg.Key3.bValid,
        pConnection->struKeyCfg.Key3.szKey,
        pConnection->struKeyCfg.Key4.bValid,
        pConnection->struKeyCfg.Key4.szKey,
        pConnection->struKeyCfg.WPAKey.bValid,
        pConnection->struKeyCfg.WPAKey.szKey,
        pConnection->szMacAddress,
        pConnection->szSSID);
#endif
	//检测热点信息是否有效,有些情况下是调用大结构体设置网络参数
	//当在AP模式下时防止无效的热点设置下来
	if(0 == strlen(pConnection->szSSID))
	{
		NET_ERR("wifi SSID is NUll!\n");
		return 0;
	}
	int ret = 0;
    int m_bWlan_DHCP_Enable_bak;
    WLAN_CONNECTION connectionCfg;

    XTRACE_MUTEX_LOCK(&m_SetWifiConnectLock);

	//m_wifiCfgMode = eWifiCfgMode_Normal;
	m_bIsConfigureNetIng = 1;

    if (bSupportWlanAP)
    {
        //AP模式下接收到配置后保存信息然后切换到STA模式
        if(SOFT_AP_MODE == m_wifiMode)
        {
            //m_pCConfig->SetValue(INet_CfgSection, INet_WLANWPAKey_key, pConnection->struKeyCfg.WPAKey.szKey);
			WLAN_SaveWifiKey(pConnection->struKeyCfg.WPAKey.szKey);
            m_pCConfig->SetValue(INet_CfgSection, INet_WLANSSID_key, pConnection->szSSID);
			saveWlanSSIDBak(WIFI_CFG_INI, pConnection->szSSID);
            m_pCConfig->SetValue(INet_CfgSection, INet_WLAN_AP_TO_STA_FLAG_key, 1);
            m_wifiToStaFlag = 1;
            NET_SetWifiMode(1, 0);
            NET_INFO("check wifi config: ssid=%s, key=%s\n", pConnection->szSSID, pConnection->struKeyCfg.WPAKey.szKey);
            XTRACE_MUTEX_UNLOCK(&m_SetWifiConnectLock);
            int counter = 0;
            ZWSys_SetSpeechContent(SPEECH_WIFI_CONNECTING);

            //循环查询，等待播放结束
            while (1)
            {
                SelectSleepSec(1);
                if (0 == ZWSys_GetSpeechPlayingStatus())
                {
                    break;
                }
                if (++counter > SPEECH_PLAY_TIME_OUT_SEC)
                {
                    counter = 0;
                    break;
                }
            }
            return 0;
        }
    }
    memcpy(&connectionCfg, pConnection, sizeof(WLAN_CONNECTION));

    //因为只响应结构体里面的SSID跟加密，所以需要用上次搜索到的认证模式跟加密方式
    if (SOFT_AP_MODE == m_wifiMode)
    {
        WLAN_updateConnectionCfg(&connectionCfg);
    }

	/*wifi热点的设置目前客户端并未传MAC地址下来*/
    if ((0 == strncmp(m_tWlan_Connection.szSSID, connectionCfg.szSSID, WLAN_MAX_SSID_LEN))
        && (m_tWlan_Connection.nAuthMode == connectionCfg.nAuthMode)
        && (m_tWlan_Connection.nEncrypType == connectionCfg.nEncrypType)
        //&& (m_tWlan_Connection.nNetworkType == pConnection->nNetworkType)
        //&& (0 == strncmp(m_tWlan_Connection.szMacAddress, connectionCfg.szMacAddress, WLAN_MAC_ADDRESS_LEN))
        && (0 == force)
        )
    {
        if (ET_NONE == m_tWlan_Connection.nEncrypType)
        {
            XTRACE_MUTEX_UNLOCK(&m_SetWifiConnectLock);
            return 0;
        }
        else if ((ET_WEP == m_tWlan_Connection.nEncrypType)
                 && (m_tWlan_Connection.struKeyCfg.nDefKeyID == connectionCfg.struKeyCfg.nDefKeyID)
                 &&
                 (((0 == m_tWlan_Connection.struKeyCfg.nDefKeyID)
                   && (0 == strncmp(m_tWlan_Connection.struKeyCfg.Key1.szKey, connectionCfg.struKeyCfg.Key1.szKey, WLAN_MAX_KEY_LEN)))
                  || ((1 == m_tWlan_Connection.struKeyCfg.nDefKeyID)
                      && (0 == strncmp(m_tWlan_Connection.struKeyCfg.Key2.szKey, connectionCfg.struKeyCfg.Key2.szKey, WLAN_MAX_KEY_LEN)))
                  || ((2 == m_tWlan_Connection.struKeyCfg.nDefKeyID)
                      && (0 == strncmp(m_tWlan_Connection.struKeyCfg.Key3.szKey, connectionCfg.struKeyCfg.Key3.szKey, WLAN_MAX_KEY_LEN)))
                  || ((3 == m_tWlan_Connection.struKeyCfg.nDefKeyID)
                      && (0 == strncmp(m_tWlan_Connection.struKeyCfg.Key4.szKey, connectionCfg.struKeyCfg.Key4.szKey, WLAN_MAX_KEY_LEN)))))
        {
            XTRACE_MUTEX_UNLOCK(&m_SetWifiConnectLock);
            return 0;
        }
        else if (0 == strncmp(m_tWlan_Connection.struKeyCfg.WPAKey.szKey, connectionCfg.struKeyCfg.WPAKey.szKey, WLAN_MAX_KEY_LEN))
        {
            XTRACE_MUTEX_UNLOCK(&m_SetWifiConnectLock);
            return 0;
        }
    }
	if (0 == force)
	{
        int counter = 0;
		ZWSys_SetSpeechContent(SPEECH_WIFI_CONNECTING);

        //循环查询，等待播放结束
        while(1)
        {
            SelectSleepSec(1);
            if (0 == ZWSys_GetSpeechPlayingStatus())
            {
                break;
            }
            if (++counter > SPEECH_PLAY_TIME_OUT_SEC)
            {
                break;
            }
        }
	}

    if ((bSupportWlanAP) && (WLAN_STA_MODE==m_wifiMode) && (!force))
    {
        //上层(force为0)设置wifi连接配置前先保存以前的连接配置
        WLAN_GetConnection(&lastConnection);
    }
	if (1 == g_DistributionNetwordFlag)
	{
		ret = WLAN_getWLANWPAKey(connectionCfg.szSSID,&connectionCfg.nAuthMode,&connectionCfg.nEncrypType);
		if(!ret)
		{
			NET_INFO("ssid para find success %d %d !!!\n",connectionCfg.nAuthMode,connectionCfg.nEncrypType);
		}
		else
		{
			NET_ERR("ssid para find fail !!!\n");
		}
		g_DistributionNetwordFlag = 0;
	}
#ifdef DISABLE_WIFI_CONNECT_WHEN_WIRE
	if((1 == m_nCurrNetCardType) && (0 == m_wifiToStaFlag))
	{
		memcpy(&m_tWlan_Connection, &connectionCfg, sizeof(WLAN_CONNECTION));
		WLAN_SaveConnCfg(m_pCConfig);
		ifWifiNewConnectCfg = 1;
		NET_INFO("DISABLE_WIFI_CONNECT_WHEN_WIRE====save wifi config only!\n");
		XTRACE_MUTEX_UNLOCK(&m_SetWifiConnectLock);
		
		m_wifiConnChange = 0;//清空wifi改变的标记，获取状态的接口正确返回wifi连接状态
		return 0;
	}
#endif
    NET_INFO("===Wlan Connection param has changed, need to refresh Netcard===\n");

	LoadStaModeDriver_8189();
    /*在切换热点之前刷新(重置)一下网卡*/
    /* 注意: 
     * 1. 在切换热点的时候，需要把DHCP打开，因为我们不知道需要连接的热点的网段是多少，需要自动获取.
     * 2. 在重置了无线路由器密码的时候，需要开启DHCP.
     * 3. 在连上无线WIFI之前，DHCP始终应该打开，因为我们需要知道所连热点的网段，所以连上之前，设置
     *    DHCP无效。
     */
    m_bWlan_DHCP_Enable_bak = m_bWlan_DHCP_Enable;
    WLAN_SetDhcpEnable(FALSE);
    WLAN_SetEnable(FALSE);
    WLAN_SetEnable(TRUE);
    ret = wifi_set_conn_cfg(&connectionCfg);   /* 连接线程 */
    if (ret)
    {
    	NET_ERR("wifi_set_conn_cfg fail = %d\n", ret);
        XTRACE_MUTEX_UNLOCK(&m_SetWifiConnectLock);
		
		m_wifiConnChange = 0;//清空wifi改变的标记，获取状态的接口正确返回wifi连接状态
        return -1;
    }
	
    //如果上层设置wifi连接配置完成后需要开启计时判断能否连上
    if(bSupportWlanAP && (WLAN_STA_MODE == m_wifiMode) && (!force))
    {
        m_changeWifiConnectFlag = 1;
    }
	m_wifiConnChange = 0;//清空wifi改变的标记，获取状态的接口正确返回wifi连接状态

    //如果以前DHCP是打开的，配置完成后需要重新DHCP获取
    if (m_bWlan_DHCP_Enable_bak)
    {
        NET_DBG("my add: [%s:%d] SET m_iWlan_DHCPFailed = 1\n", __FUNCTION__, __LINE__);
        m_iWlan_DHCPFailed = 1;
    }
	//这里存在问题，上面刚把wifi信息设置下去后
	//短时间内获取连接状态不一定是实时的
	//这样获取出来的就不一定是上面设置的
    wifi_get_conn_cfg(&m_tWlan_Connection);
    WLAN_SaveConnCfg(m_pCConfig);
    WLAN_SetDhcpEnable(TRUE);
    XTRACE_MUTEX_UNLOCK(&m_SetWifiConnectLock);
#endif
	return 0;
}
#if defined (__IS_SUPPORT_WLAN__)
int INet::CfgWirelessWhenWireChanged(int isWire)
{
	if (m_wifiMode == WLAN_STA_MODE)
	{
		if(0 == isWire)//无线
		{
			NET_INFO("DISABLE_WIFI_CONNECT_WHEN_WIRE====is wireless!\n");
			if(ifWifiNewConnectCfg)
			{
				NET_INFO("DISABLE_WIFI_CONNECT_WHEN_WIRE====do new wifi config!\n");
				//按照有线时设置的配置重新设置
				WLAN_SetConnection(&m_tWlan_Connection, 1);
				ifWifiNewConnectCfg = 0;
			}
			else
			{
				NET_INFO("DISABLE_WIFI_CONNECT_WHEN_WIRE====do old wifi config!\n");
				wifi_set_conn_cfg(&m_tWlan_Connection);
				//如果DHCP使能则需要重新获取
				if(m_bWlan_DHCP_Enable)
				{
					NET_INFO("DISABLE_WIFI_CONNECT_WHEN_WIRE====do dhcp!\n");
					m_iWlan_DHCPFailed = 1;
				}
			}
		}
		else//有线
		{
			NET_INFO("DISABLE_WIFI_CONNECT_WHEN_WIRE====is wire!\n");
			wifi_set_config_enable(0);
		}
	}

	return 0;
}

int INet::WLAN_setFromApToStaConnectionCfgFlag(int cfgOverFlag)
{
	m_wifiApToStaCfgOver = cfgOverFlag;
	return 0;
}

#endif

int INet::WLAN_GetWiFiModuleEnable()
{
	return m_bWlan_Enable;
}

int INet::WLAN_SetEnable(int bEnable)
{
    int ret = 0;

#if defined (__IS_SUPPORT_WLAN__)
    if (bSupportWIFI)
    {
        bEnable = 1;

    	if (m_bWlan_Enable == bEnable)
    	{
    		return 0;
    	}
    	
    	ret = wifi_set_enable(bEnable);

        if (0 == ret)
        {    
            m_bWlan_Enable = wifi_get_enable();
            m_pCConfig->SetValue(INet_CfgSection, INET_WLANEnable_key, m_bWlan_Enable);
        }
        else
            return ret;
	}
#endif
	return ret;
}

#if defined (__IS_SUPPORT_WLAN__)
/*当wifi设备异常时, 重新加载wifi驱动*/
static void ResetWifidriverWhenWifiFault()
{
    return;
}

static void Wlan_CheckWifiUsbDevStatus()
{
	static int pwr_reset = 0; /*网卡驱动是否需要重启计数*/
	while( 0 != access(USBDEV_IDPRODUCT, F_OK) || 0 != access(USBDEV_IDVENDOR, F_OK) )
	{
		pwr_reset++;
		if (pwr_reset >= 4) 
		{
            RebootLinux(REBOOT_WIFI_NO_IDPRODUCT);
			return;
		} 
		else 
		{
			ResetWifidriverWhenWifiFault();
			sleep(5);
			continue;
		}
	}
}

static void ResetWpaToolrWhenWifiFault()
{
    NET_WARN("ResetWpaToolrWhenWifiFault !!! \n");
    if (mysystem("wpa_cli status"))
    {
    	NET_ERR("Fail to exec \"wpa_cli status\"\n");
    	return ;
    }

    if (mysystem("wpa_cli -i wlan0 disable_network 0"))
    {
    	NET_ERR("Fail to exec: \"wpa_cli -i wlan0 disable_network 0\"\n");
    	return ;
    }

    sleep(1);

    if (mysystem("wpa_cli -i wlan0 enable_network 0"))
    {
    	NET_ERR("Fail to exec: \"wpa_cli -i wlan0 enable_network 0\"\n");
    	return ;
    }

    return ;
}

int INet::WLAN_CheckStatus()
{
	//struct sockaddr_in sockaddr;
	int ret = 0;
	int status = 0, saveStatus;
    char m_sWlan_LocalIP_new[20] = {0};
	int i = 0;
	int counter = 0;
	static int WlanIpConflictCnt = 0;
	static int WlanIpDisconnect = 0;
	static int WlanIPGetFailed = 0;
    static unsigned int m_fromApToStaConnectTick = 0;       // 连接尝试次数
    static unsigned int m_setWifiConnectTick = 0;           // 连接尝试次数
	static unsigned int wifiConnectedTick = 0;				//wifi连接上后计数
	static unsigned int wifiBrokenFlag = 0;					//wifi断连的标记
	static unsigned int wifiBrokenTick = 0;					//wifi连接失败后计数
	static char bIsFirstWifiConnectFail = 1;
	static char nFirstWifiConnectFailTick = 0;
    static int mConnectedSuccessfulOnce = 0;
    static int wlanPingFailedCnt = 0;                       //wifi连接成功时ping失败的计数
    static int wlanPingResFlag = 0;                         //wifi连接成功时ping失败后复位标记
	int nCounter = 0;
	int changeWifiCfg = 0;
    wifiinfo_t wifilist[WIFI_LIST_MAX];
    int wifi_list_count = 0;
    int got_ap = FALSE;
    char wifi_ssid[128] = {0};
    static int pwr_reset = 0;

    Wlan_CheckWifiUsbDevStatus();
    
    if (m_bCheck_exit)
    {
        NET_DBG("stop check status \n");
        return -1;
    }

	if (bIsFirstWifiConnectFail)
	{
		//只有STA模式才存在此逻辑
		if (m_wifiMode != WLAN_STA_MODE)
	    {
			
	    	bIsFirstWifiConnectFail = 0;
	    }
	}

	if (m_iWlanScanStatus && (0 == g_ethLinkStatus))  //扫码配网下
	{
		bIsFirstWifiConnectFail = 0;
	}

#ifdef DISABLE_WIFI_CONNECT_WHEN_WIRE
	//如果当前为有线，则不去检测wifi状态
	if((WLAN_STA_MODE == m_wifiMode) && (1 == m_nCurrNetCardType) && (0 == m_wifiToStaFlag))
	{
//		NET_INFO("DISABLE_WIFI_CONNECT_WHEN_WIRE====not do wlan check pthread!\n");
		if(m_wifiMode != SOFT_AP_MODE)//设置wifi状态灯
			ZWSys_SetWifiStatusLed(IN_WIFI_CONNECTING);
		return 0;
	}
#endif

	if (!m_bWlan_Enable || (2 == wifi_get_enable()))
	{
		WlanIPGetFailed = 1;
		WlanIpDisconnect = 0;
		return 0;
	}

    //bit0-[0/1]物理断/开,bit1-[0/1]IP更新,bit2-[0/1]ip冲突,bit3-[0/1]IP获取失败,bit4-[0/1]禁用/启用
    status = 0;
    //////////////////////////////////////////////////////////////////////////
    if (m_wifiMode == SOFT_AP_MODE)
    {
    	bIsFirstWifiConnectFail = 0;
    }
    else
	{
        // 1.先检查物理是否断开
        if (0 != wifi_get_curr_status(&status))
        {
            NET_ERR("wifi get curr conn status failed\n");
            status = 0;
            return 0;
        }
	}

    // 转换一下状态
    status = !status;  // 0:连接上  1:断开

    if (status & 1)  // 断开了
    {
        if (m_wifiMode != SOFT_AP_MODE)
        {
            //设置wifi状态灯
            ZWSys_SetWifiStatusLed(IN_WIFI_CONNECTING);
        }

		//已配网且开机首次就未连接上才播报
		//当有线接入状态发生改变时重新检测
		if (1 == g_ethLinkStatus)	//lan link up
		{
			//只有STA模式才存在此逻辑
			if (WLAN_STA_MODE == m_wifiMode)
		    {
		    	//重置计数及标志
		    	//为下次有线断开重新检测做准备
		    	bIsFirstWifiConnectFail = 1;
		    	nFirstWifiConnectFailTick = 0;
			}
		}
		else
		{
			if (bIsFirstWifiConnectFail)  /* 没有配网的时候断开的情况 */
			{
				nFirstWifiConnectFailTick++;
				if ((30 == nFirstWifiConnectFailTick) 
					|| (60 == nFirstWifiConnectFailTick) 
					|| (90 == nFirstWifiConnectFailTick))
		        {
            		ZWSys_SetSpeechContent(SPEECH_WIFI_CONNECT_FAIL);

					//循环查询，等待播放结束
					nCounter = 0;
				    while (1)
				    {
				        sleep(1);
				        if (0 == ZWSys_GetSpeechPlayingStatus())
				        {
				            break;
				        }
				        if (++nCounter > SPEECH_PLAY_TIME_OUT_SEC)
				        {
				            nCounter = 0;
				            break;
				        }
				    }
					
					m_bIsConfigureNetIng = 0;
		        }
				
		        if (nFirstWifiConnectFailTick >= 70)	//播报次数达到3次后不再播报
		        {
		            bIsFirstWifiConnectFail = 0;
		        }
			}

		}

        //如果稳定连接达到60次计数，开始去检查是否因为链路问题导致的断连
        if (wifiConnectedTick > 60)
        {
            //如果没有人为的操作
            if((0 == m_changeWifiConnectFlag) || (0 == m_wifiConnChange) || (0 == m_bWifiTestIsBusy))
            {
                wifiBrokenFlag = 1;//设置wifi链路异常标记
                wifiBrokenTick = 0;
            }
        }

        if (wifiBrokenFlag)
        {
            wifiBrokenTick++;
            if(wifiBrokenTick >= 60)//播报次数达到3次后不再播报
            {
                wifiBrokenFlag = 0;
            }
        }
        wifiConnectedTick = 0;//计数清零
        wlanPingFailedCnt = 0;
        wlanPingResFlag = 0;

        if (bSupportWlanAP)
        {
            //从AP切换到了STA
            if (m_wifiToStaFlag && m_wifiApToStaCfgOver)
            {
                if (m_fromApToStaConnectTick++ > 15)
                {
                    saveConnectSuccess(0);
                    //新版本语音没有密码错误的类型
                    int failReason = 0;
                    ret = get_wifi_fail_reason(&failReason); /* 获取失败原因 */
                    if ((0 == ret) && (failReason))
                    {
                        ZWSys_SetSpeechContent(SPEECH_WIFI_CONNECT_FAIL_PWD_WRONG);
                    }
                    else
                    {
                        ZWSys_SetSpeechContent(SPEECH_WIFI_CONNECT_FAIL);
                    }

                    //循环查询，等待播放结束
                    while(1)
                    {
						
                        SelectSleepSec(1);
                        if (0 == ZWSys_GetSpeechPlayingStatus())
                        {
                            break;
                        }
                        if (++counter > SPEECH_PLAY_TIME_OUT_SEC)
                        {
                            counter = 0;
                            break;
                        }
                    }
					SelectSleepSec(1);

                    m_wifiToStaFlag = 0;
                    m_fromApToStaConnectTick = 0;
                    m_wifiApToStaCfgOver = 0;
                    m_pCConfig->SetValue(INet_CfgSection, INet_WLAN_AP_TO_STA_FLAG_key, m_wifiToStaFlag);
                    m_pCConfig->SaveFile();
                    NET_DBG("my add: wifi connect fail! Set ap mode\n");
                    NET_SetWifiMode(2, 0);  /* 切换wifi模式 AP */

					//提示扫码完成后
				    ZWSys_SetSpeechContent(SPEECH_WIFI_PLEASE_CONNECT);
					//循环查询，等待播放结束
					counter = 0;
				    while(1)
                    {
                        SelectSleepSec(1);
                        if (0 == ZWSys_GetSpeechPlayingStatus())
                        {
                            break;
                        }
                        if (++counter > SPEECH_PLAY_TIME_OUT_SEC)
                        {
                            counter = 0;
                            break;
                        }
                    }
					m_bIsConfigureNetIng = 0;
                }
            }
            //wifi连接配置更新了
            if((m_changeWifiConnectFlag) && (!m_wifiToStaFlag) ) //如果是ap到sta，则不recovery，如果是sta到sta失败的话recovery
            {
                if(m_setWifiConnectTick++ > 15)
                {
					saveConnectSuccess(0);
                    /* 新版本语音没有密码错误的类型 */
					int failReason = 0;
					ret = get_wifi_fail_reason(&failReason);
					if((0 == ret) && (failReason))
					{
						ZWSys_SetSpeechContent(SPEECH_WIFI_CONNECT_FAIL_PWD_WRONG);
					}
					else
					{
						ZWSys_SetSpeechContent(SPEECH_WIFI_CONNECT_FAIL);
					}

                    //循环查询，等待播放结束
                    counter = 0;
                    while(1)
                    {
                        SelectSleepSec(1);
                        if (0 == ZWSys_GetSpeechPlayingStatus())
                        {
                            break;
                        }
                        if (++counter > SPEECH_PLAY_TIME_OUT_SEC)
                        {
                            counter = 0;
                            break;
                        }
                    }
                    m_changeWifiConnectFlag = 0;
                    m_setWifiConnectTick = 0;
                    NET_DBG("my add: wifi connect fail! recover last wifi cfg\n");
                    //新的wifi配置连接失败需要切回以前的配置
                    WLAN_SetConnection(&lastConnection, 1);
					//提示扫码完成后
				    ZWSys_SetSpeechContent(SPEECH_WIFI_PLEASE_CONNECT);
					//循环查询，等待播放结束
					counter = 0;
				    while(1)
                    {
                        SelectSleepSec(1);
                        if (0 == ZWSys_GetSpeechPlayingStatus())
                        {
                            break;
                        }
                        if (++counter > SPEECH_PLAY_TIME_OUT_SEC)
                        {
                            counter = 0;
                            break;
                        }
                    }
					m_bIsConfigureNetIng = 0;
                }
            }
        }
        //开始回调
        goto do_callback;
    }
    else
    {
        if(m_wifiMode != SOFT_AP_MODE)//设置wifi状态灯
        {
        	m_nWifiLinkStatus = 2;	//标志配网完成
            ZWSys_SetWifiStatusLed(IN_WIFI_CONNECTED);
        }
		wifiConnectedTick++;//计数累加
		wifiBrokenFlag = 0;

		//连续3次检测到连接上才做UDP推送
		if (wifiConnectedTick >= 3)
		{
			XTRACE_MUTEX_LOCK(&m_muUdpPushLock);
			if (1 == m_nUdpPushStatus)
			{
				m_nUdpPushStatus = 2;	//标志UDP推送环境就绪
			}
			XTRACE_MUTEX_UNLOCK(&m_muUdpPushLock);
		}

		//连续五次检测到连接上才认为连接上
		if (wifiConnectedTick >= 5)
		{
			bIsFirstWifiConnectFail = 0;
            mConnectedSuccessfulOnce = 1;

			//配网成功
			if (bSupportWlanAP && changeWifiCfg)
			{
				char ipadd[20] = {0};
				char gateway[20] = {0};
				char netmask[20] = {0};
				uint8_t samecomple[2][3] = { 0 };
				changeWifiCfg = 0;
				WLAN_GetIPAddress(ipadd);
				WLAN_GetGateway(gateway, sizeof(gateway));
				WLAN_GetSubnetMask(netmask);
				sscanf(ipadd,"%d.%d.%d.%d",(int *)&samecomple[0][0],(int *)&samecomple[0][1],(int *)&samecomple[0][2],(int *)&samecomple[0][3]);
				sscanf(gateway,"%d.%d.%d.%d",(int *)&samecomple[1][0],(int *)&samecomple[1][1],(int *)&samecomple[1][2],(int *)&samecomple[1][3]);
				if((samecomple[0][0] == samecomple[1][0]) && (samecomple[0][1] == samecomple[1][1]) &&(samecomple[0][2] == samecomple[1][2]))
				{
					NET_INFO("ip and way is sample\n");
				}
				else
				{
					memset(gateway, 0, sizeof(gateway));
					snprintf(gateway, sizeof(gateway), "%d.%d.%d.1",samecomple[0][0],samecomple[0][1],samecomple[0][2]);
					WLAN_SetGateway(gateway);
					NET_INFO("ip and way is no sample\n");
				}
				saveWlanIpBak(ipadd);
				saveWlanGateWayBak(gateway);
				saveWlanSubMaskBak(netmask);
			}
		}
		//配网完成后关掉二维码扫描功能
		if (bSupportQrScan)
        {
            SetQRScanStatus(0);
        }
        if (bSupportWlanAP)
        {
            //从AP切换到了STA
            if(m_wifiToStaFlag && m_wifiApToStaCfgOver)
            {
				saveConnectSuccess(1);
                NET_INFO("AP -> STA SPEECH_WIFI_CONNECT_SUCCESS\n");
                ZWSys_SetSpeechContent(SPEECH_WIFI_CONNECT_SUCCESS);
                m_wifiToStaFlag = 0;
                m_fromApToStaConnectTick = 0;
                m_wifiApToStaCfgOver = 0;
				IPAdjustCloseFlag = 1;     /* wifi 连接成功后就要关闭自适应 */
                m_pCConfig->SetValue(INet_CfgSection, INet_WLAN_AP_TO_STA_FLAG_key, m_wifiToStaFlag);
                m_pCConfig->SaveFile();
				m_wifiFakeSTAModeEnable = 0;
				m_pCConfig->SetValue(INet_CfgSection, INet_WLANFakeSTAMode, m_wifiFakeSTAModeEnable);
				m_pCConfig->SaveFileReal();
                NET_DBG("my add: wifi has connected\n");
                //因为无线已经连接上了，所以只有当有线插着的才需要处理
                if(1 == m_nCurrNetCardType)
                {            
                    CfgWirelessWhenWireChanged(1);
                }
                
				WLAN_BluetoothClose();
            }
            //wifi连接配置更新了
            if (m_changeWifiConnectFlag)
            {
                m_changeWifiConnectFlag = 0;
				changeWifiCfg = 1;
				saveConnectSuccess(1);
				m_wifiFakeSTAModeEnable = 0;
				m_pCConfig->SetValue(INet_CfgSection, INet_WLANFakeSTAMode, m_wifiFakeSTAModeEnable);
				m_pCConfig->SaveFileReal();

                NET_INFO("STA -> STA cfg update SPEECH_WIFI_CONNECT_SUCCESS\n");
				ZWSys_SetSpeechContent(SPEECH_WIFI_CONNECT_SUCCESS);
                //循环查询，等待播放结束
				counter = 0;
                while(1)
                {
                    SelectSleepSec(1);
                    if (0 == ZWSys_GetSpeechPlayingStatus())
                    {
                        break;
                    }
                    if (++counter > SPEECH_PLAY_TIME_OUT_SEC)
                    {
						
                        counter = 0;
                        break;
                    }
                }
				#ifdef SUPPORT_FORCE_STA_WHEN_AP_WIRE
				if(m_wifiFakeSTAModeEnable)
				{
					NET_INFO("Wifi has connected, clear WLANFakeSTAMode flag!\n");
					m_wifiFakeSTAModeEnable = 0;
					m_pCConfig->SetValue(INet_CfgSection, INet_WLANFakeSTAMode, m_wifiFakeSTAModeEnable);
					m_pCConfig->SaveFile();
				}
				#endif
				
				WLAN_RenewDhcp();
                NET_DBG("my add: wifi has connected\n");
            }

			if (eWifiCfgMode_SdCard == m_wifiCfgMode)
			{
				sleep(4);
 				ZWSys_SetSpeechContent(SPEECH_NET_CONNECT_SUCCESS);
                //循环查询，等待播放结束
				counter = 0;
                while (1)
                {
                	NET_INFO("wwwww counter=%d\n",counter);
                    sleep(1);
                    if (0 == ZWSys_GetSpeechPlayingStatus())
                    {
                        break;
                    }
                    if (++counter > SPEECH_PLAY_TIME_OUT_SEC)
                    {
                        counter = 0;
                        break;
                    }
                }

				m_pCConfig->SetValue(INet_CfgSection, INet_WLANWifiMode, WLAN_STA_MODE);
				saveWlanWifiModeBak(WLAN_STA_MODE);
				m_pCConfig->SetValue(INet_CfgSection, INet_WLANFakeSTAMode, 1);
				m_pCConfig->SaveFile();

				//播报一次后，后续不在需要
				//将标志还原
				m_wifiCfgMode = eWifiCfgMode_Normal;
			}

            m_setWifiConnectTick = 0;
        }

        /* 网络连接成功，30S ping一次网关 */
		if (wifiConnectedTick % 30 == 0)
        {
            if (1 != myping((const char *)IWNAME, m_sWlan_GateWay))
            {
                wlanPingFailedCnt++;
            }
            else
            {            
                wlanPingResFlag = 0;
                wlanPingFailedCnt = 0;
            }
            NET_INFO("Wifi PingCheck, Flag[%d] Cnt[%d]\n", wlanPingResFlag, wlanPingFailedCnt);
        }

		m_bIsConfigureNetIng = 0;
    }

    WlanIpDisconnect = 0;
    // 2.检测是否有IP更新
    if (NULL != WLAN_GetIPAddress(m_sWlan_LocalIP_new))
    {
        NET_DBG("my add:WLAN_GetIPAddress=%s\n", m_sWlan_LocalIP_new);
        //不相等，即有新IP.
        if (WlanIPGetFailed || strncmp(m_sWlan_LocalIP_new, m_sWlan_LocalIP_old, 20) != 0)
        {
            status |= (1 << 1);
            NET_DBG(" new ip = %s\n", m_sWlan_LocalIP_new);
            strncpy(m_sWlan_LocalIP_old, m_sWlan_LocalIP_new, sizeof(m_sWlan_LocalIP_old) - 1);
            WlanIpConflictCnt = 0;
            WlanIPGetFailed = 0;
            goto do_callback;
        }
    }
    else
    {
        WlanIPGetFailed = 1;
        //获取IP失败
        NET_INFO("NET_GetIPAddress failed\n");
        status |= (1 << 3);
        goto do_callback;
    }

    //3. 在网络连接的情况下,检测IP是否冲突
	ret = IsWlanIpConflict();
	if (ret > 0)
	{
		status |= (1 << 2);		//IP冲突
		WlanIpConflictCnt = 5;
	}
	else
	{							//5次检测到无冲突后才认为已经正常
		WlanIpConflictCnt--;
		if (WlanIpConflictCnt < 0)
		{
			WlanIpConflictCnt = 0;

		}
		else
		{
			status |= (1 << 2);	//IP冲突
		}
	}
	
  do_callback:
	saveStatus = (status & 1);
	if (status & 1)
	{
		//无线断开
		if (WlanIpDisconnect < 3)
		{
			
			saveStatus = (status & (~1));
			WlanIpDisconnect++;
		}
		else if (NULL != m_FxnCallBack_PHY)
		{
			struct tagAlertInfo pAlertInfo;
			struct tagActionInfo pActionInfo;
			
			pAlertInfo.AlertType = m_PHYAlarmType;
			pAlertInfo.nChannel = 0;
			pAlertInfo.dwReserved = (WLAN0_INDEX << 16) | 0;
			pActionInfo.dwReserved = 0;
			pActionInfo.nChannel = 0;
			
			// if (m_bGUI[1])
			if((0 != NET_GetStatus())) 
			{
				pActionInfo.tActionType = ACTION_GUI;
				m_FxnCallBack_PHY(&pAlertInfo, &pActionInfo);
			}
			
			#if 0
			if (m_bPush[1])
			{
				pActionInfo.tActionType = ACTION_PUSH;
				m_FxnCallBack_PHY(&pAlertInfo, &pActionInfo);
			}
			#endif
			
			if (m_bBeep[1])
			{
				pActionInfo.tActionType = ACTION_BEEP;
				m_FxnCallBack_PHY(&pAlertInfo, &pActionInfo);
			}
			if (m_bSTS[1])
			{
				pActionInfo.tActionType = ACTION_SENDTOSERVER;
				m_FxnCallBack_PHY(&pAlertInfo, &pActionInfo);
			}
			if (m_bEmail[1])
			{
				pActionInfo.tActionType = ACTION_EMAIL;
				m_FxnCallBack_PHY(&pAlertInfo, &pActionInfo);
			}
			if (m_bSMS[1])
			{
				pActionInfo.tActionType = ACTION_SMS;
				m_FxnCallBack_PHY(&pAlertInfo, &pActionInfo);
			}

			if (m_bDOP[1])
			{
				pActionInfo.tActionType = ACTION_ALARMOUT;
				for (i = 0; i < 8; i++)
				{
					if (m_bDOP[1] & (1 << i))
					{
						pActionInfo.nChannel = i;
						m_FxnCallBack_PHY(&pAlertInfo, &pActionInfo);
					}
				}

			}
			
			WLAN_RenewDhcp();

#if 0
			if (m_bRecord[1])
			{
				pActionInfo.tActionType = ACTION_RECORD;
				for (i = 0; i < 16; i++)
				{
					if (m_bDOP[1] & (1 << i))
					{
						pActionInfo.nChannel = i;
						m_FxnCallBack_PHY(&pAlertInfo, &pActionInfo);
					}
				}
			}
#endif
		}

	}
	else if (m_iWlan_Status == 1 && (NULL != m_FxnCallBack_PHY))
	{							//通，且上次为断
		struct tagAlertInfo pAlertInfo;
		struct tagActionInfo pActionInfo;

		pAlertInfo.AlertType = m_PHYAlarmType;
		pAlertInfo.nChannel = 0;
		pAlertInfo.dwReserved = (WLAN0_INDEX << 16) | 1;
		pActionInfo.dwReserved = 0;
		pActionInfo.nChannel = 0;
		
		//if (m_bGUI[1])
		if((0 != NET_GetStatus())) 
		{
			pActionInfo.tActionType = ACTION_GUI;
			m_FxnCallBack_PHY(&pAlertInfo, &pActionInfo);
		}
		if (m_bPush[1])
		{
			pActionInfo.tActionType = ACTION_PUSH;
			m_FxnCallBack_PHY(&pAlertInfo, &pActionInfo);
		}
	}
	//IP回调
	if (status & 2)
	{							//IP有更新
		saveStatus = 4;
		if (NULL != m_FxnCallBack_IP)
		{
			struct tagAlertInfo pAlertInfo;
			struct tagActionInfo pActionInfo;

			pAlertInfo.AlertType = m_IPAlarmType;
			pAlertInfo.nChannel = 0;
			pAlertInfo.dwReserved = (WLAN0_INDEX << 16) | 1;
			pActionInfo.dwReserved = 0;
			pActionInfo.nChannel = 0;
			
			//if (m_bGUI[0])
			{
				pActionInfo.tActionType = ACTION_GUI;
				m_FxnCallBack_IP(&pAlertInfo, &pActionInfo);
			}
			if (m_bPush[0])
			{
				pActionInfo.tActionType = ACTION_PUSH;
				m_FxnCallBack_IP(&pAlertInfo, &pActionInfo);
			}
		}
	}
	if (status & 4)
	{							//IP有冲突
		saveStatus = 3;
		if (NULL != m_FxnCallBack_IP)
		{
			struct tagAlertInfo pAlertInfo;
			struct tagActionInfo pActionInfo;

			pAlertInfo.AlertType = m_IPAlarmType;
			pAlertInfo.nChannel = 0;
			pAlertInfo.dwReserved = (WLAN0_INDEX << 16) | 0;
			pActionInfo.dwReserved = 0;
			pActionInfo.nChannel = 0;
			pActionInfo.dwReserved = (unsigned long) m_sOtherMAC_addr;

			//if (m_bGUI[0])
			{
				pActionInfo.tActionType = ACTION_GUI;
				m_FxnCallBack_IP(&pAlertInfo, &pActionInfo);
			}

			#if 0
			if (m_bPush[0])
			{
				pActionInfo.tActionType = ACTION_PUSH;
				m_FxnCallBack_IP(&pAlertInfo, &pActionInfo);
			}
			#endif

			if (m_bBeep[0])
			{
				pActionInfo.tActionType = ACTION_BEEP;
				m_FxnCallBack_IP(&pAlertInfo, &pActionInfo);
			}
			if (m_bSTS[0])
			{
				pActionInfo.tActionType = ACTION_SENDTOSERVER;
				m_FxnCallBack_IP(&pAlertInfo, &pActionInfo);
			}
			if (m_bEmail[0])
			{
				pActionInfo.tActionType = ACTION_EMAIL;
				m_FxnCallBack_IP(&pAlertInfo, &pActionInfo);
			}
			if (m_bSMS[0])
			{
				pActionInfo.tActionType = ACTION_SMS;
				m_FxnCallBack_IP(&pAlertInfo, &pActionInfo);
			}

			if (m_bDOP[0])
			{
				pActionInfo.tActionType = ACTION_ALARMOUT;
				for (i = 0; i < 8; i++)
				{
					if (m_bDOP[0] & (1 << i))
					{
						pActionInfo.nChannel = i;
						m_FxnCallBack_IP(&pAlertInfo, &pActionInfo);
					}
				}
			}

		}
		WLAN_RenewDhcp();
	}
	else if (m_iWlan_Status == 3)
	{
		//上一次是冲突的，现在不是了
		if (NULL != m_FxnCallBack_IP)
		{
			struct tagAlertInfo pAlertInfo;
			struct tagActionInfo pActionInfo;

			pAlertInfo.AlertType = m_IPAlarmType;
			pAlertInfo.nChannel = 0;
			pAlertInfo.dwReserved = (WLAN0_INDEX << 16) | 1;
			pActionInfo.dwReserved = 0;
			pActionInfo.nChannel = 0;
			pActionInfo.dwReserved = (unsigned long) m_sOtherMAC_addr;

			//if (m_bGUI[0])
			{
				pActionInfo.tActionType = ACTION_GUI;
				m_FxnCallBack_IP(&pAlertInfo, &pActionInfo);
			}

			if (m_bPush[0])
			{
				pActionInfo.tActionType = ACTION_PUSH;
				m_FxnCallBack_IP(&pAlertInfo, &pActionInfo);
			}
		}
	}
	if (m_iWlan_Status != saveStatus)
	{
		m_iWlan_Status = saveStatus;
		NET_INFO("wifi status:%s\n", des_state[saveStatus]);
		if (saveStatus == 3)	//IP冲突
		{
			NET_INFO("Wifi IP conflict\n%-12s:[%02x:%02x:%02x:%02x:%02x:%02x]\n%-12s:[%s]\n", "HWaddr", m_sOtherMAC_addr[0], m_sOtherMAC_addr[1],
				m_sOtherMAC_addr[2], m_sOtherMAC_addr[3], m_sOtherMAC_addr[4], m_sOtherMAC_addr[5], "inet addr", m_sWlan_LocalIP_new);
		}
	}
	if (m_iWlan_DHCPFailed || (status & 8))
	{							//上次DHCP失败，则更新DHCP
		if ((status & 1) ^ 1)
		{
			WLAN_RenewDhcp();
			// m_iWlan_DHCPFailed = 0;
		}
        else
        {
            if (mConnectedSuccessfulOnce == 1 && g_ethLinkStatus == 0) 
            {
                pwr_reset++;
                
                if( pwr_reset == 30 )
                {
                    ResetWifidriverWhenWifiFault();
                    sleep(5);
                    ResetWpaToolrWhenWifiFault();
                    usleep(200 * 1000);
                }
                if( pwr_reset % 19 == 0 )
                {
                    //搜索附近热点
                    for (i = 0; i < 2; i++) {
                        memset(wifilist, 0, sizeof(wifilist));
                        wifi_list_count = wifi_scan_ap_result(wifilist, WIFI_LIST_MAX);  
                        if (wifi_list_count > 0) {
                            break;
                        }
                        sleep(3);
                    }
                
                    if ( wifi_list_count <= 0 ) {
                        ResetWpaToolrWhenWifiFault();
                        return(m_iWlan_Status);
                    }
                
                    char *getssid = (char *)"wpa_cli status| grep ssid| grep -v bssid| awk -F \"=\" '{print $2}'";
                    memset(wifi_ssid, 0, sizeof(wifi_ssid));
                    ShellCmdOut(getssid, wifi_ssid, sizeof(wifi_ssid));
                    wifi_ssid[strlen(wifi_ssid)-1] = '\0';
                
                    got_ap = FALSE;
                    for (i = 0; i < wifi_list_count; i++) {
                        if (strncmp(wifilist[i].essid, wifi_ssid, strlen(wifilist[i].essid)) == 0) {
                            got_ap = TRUE;
                            NET_INFO("Search to complete: %s\n",wifi_ssid);
                            break;
                        }
                    }
                
                    if ( got_ap == FALSE ) {
                        NET_INFO("Search to complete: %s fail\n",wifi_ssid);
                        ResetWpaToolrWhenWifiFault();
                        return(m_iWlan_Status);
                    }
                }

                if( pwr_reset% 60 == 0)
                {
                    RebootLinux(REBOOT_WIFI_STATUS_ABNORMAL);
                }
           }
        }
	}

    // status状态正常，但是ping检查失败，尝试重启一次wifi
    if ((wlanPingFailedCnt >= 3) && (wlanPingResFlag == 0) && (mConnectedSuccessfulOnce == 1))
    {
        NET_WARN("Wifi PingCheck failed, reset wpa. Cnt[%d]\n", wlanPingFailedCnt);
        wlanPingResFlag = 1;
        wifi_reset_enable();
    }

	return m_iWlan_Status;
}

int INet::IsWlanIpConflict()
{
	return 0;
}

int INet::WLAN_ReadConnCfgFromSD()
{
    if (0 == access(WIFI_SD_PATH, F_OK))
    {
		CConfig *pcfg = CConfig::NewInstance(); 
		NET_INFO("WLAN_ReadConnCfgFromSD 1\n");
		if (NULL != pcfg)
		{
			NET_INFO("WLAN_ReadConnCfgFromSD 2\n");
			if (0 == pcfg->LoadFileRO(WIFI_SD_PATH))
			{
				NET_INFO("WLAN_ReadConnCfgFromSD 3\n");
				pcfg->GetValue(INet_CfgSection, INet_WLANSSID_key, "", m_tWlan_Connection.szSSID, WLAN_MAX_SSID_LEN - 1);
				pcfg->GetValue(INet_CfgSection, INet_WLANKey, "", m_tWlan_Connection.struKeyCfg.Key1.szKey, WLAN_MAX_KEY_LEN - 1);
				pcfg->GetValue(INet_CfgSection, INet_WLANKey, "", m_tWlan_Connection.struKeyCfg.Key2.szKey, WLAN_MAX_KEY_LEN - 1);
				pcfg->GetValue(INet_CfgSection, INet_WLANKey, "", m_tWlan_Connection.struKeyCfg.Key3.szKey, WLAN_MAX_KEY_LEN - 1);
				pcfg->GetValue(INet_CfgSection, INet_WLANKey, "", m_tWlan_Connection.struKeyCfg.Key4.szKey, WLAN_MAX_KEY_LEN - 1);
				pcfg->GetValue(INet_CfgSection, INet_WLANKey, "", m_tWlan_Connection.struKeyCfg.WPAKey.szKey, WLAN_MAX_KEY_LEN - 1);
				CConfig::ReleaseInstance(pcfg);
				pcfg = NULL;

				m_wifiCfgMode = eWifiCfgMode_SdCard;
                m_wifiSdCfgMode = eWifiCfgMode_SdCard;

				return 0;
			}
			//失败的情况下也需要释放
			CConfig::ReleaseInstance(pcfg);
			pcfg = NULL;
		}
    }
	
	return -1;
}


int INet::WLAN_ReadConnCfg(CConfig * pConfig)
{
	m_tWlan_Connection.nNetworkType = pConfig->GetValue(INet_CfgSection, INet_WLANNetworkType_key, 2);
	m_tWlan_Connection.nAuthMode = pConfig->GetValue(INet_CfgSection, INet_WLANAuthMode_key, 0);
	m_tWlan_Connection.nEncrypType = pConfig->GetValue(INet_CfgSection, INet_WLANEncType_key, 0);

//  NET_INFO("Get nAuthMode=%d, nEncrypType=%d, nNetworkType=%d\n", m_tWlan_Connection.nAuthMode, m_tWlan_Connection.nEncrypType,m_tWlan_Connection.nNetworkType);

	if (ET_NONE == m_tWlan_Connection.nEncrypType)
	{
		m_tWlan_Connection.struKeyCfg.bValid = 1;
		m_tWlan_Connection.struKeyCfg.WPAKey.bValid = 1;
	}
	else if (ET_WEP == m_tWlan_Connection.nEncrypType)
	{
		m_tWlan_Connection.struKeyCfg.nDefKeyID = pConfig->GetValue(INet_CfgSection, INet_WLANDefKeyID_key, 0);
		switch (m_tWlan_Connection.struKeyCfg.nDefKeyID)
		{
			case 1:
				m_tWlan_Connection.struKeyCfg.bValid = 1;
				m_tWlan_Connection.struKeyCfg.Key1.bValid = 1;
				pConfig->GetValue(INet_CfgSection, INet_WLANKey1_key, "", m_tWlan_Connection.struKeyCfg.Key1.szKey, WLAN_MAX_KEY_LEN - 1);
				break;
			case 2:
				m_tWlan_Connection.struKeyCfg.bValid = 1;
				m_tWlan_Connection.struKeyCfg.Key2.bValid = 1;
				pConfig->GetValue(INet_CfgSection, INet_WLANKey2_key, "", m_tWlan_Connection.struKeyCfg.Key2.szKey, WLAN_MAX_KEY_LEN - 1);
				break;
			case 3:
				m_tWlan_Connection.struKeyCfg.bValid = 1;
				m_tWlan_Connection.struKeyCfg.Key3.bValid = 1;
				pConfig->GetValue(INet_CfgSection, INet_WLANKey3_key, "", m_tWlan_Connection.struKeyCfg.Key3.szKey, WLAN_MAX_KEY_LEN - 1);
				break;
			case 4:
				m_tWlan_Connection.struKeyCfg.bValid = 1;
				m_tWlan_Connection.struKeyCfg.Key4.bValid = 1;
				pConfig->GetValue(INet_CfgSection, INet_WLANKey4_key, "", m_tWlan_Connection.struKeyCfg.Key4.szKey, WLAN_MAX_KEY_LEN - 1);
				break;
			default:
				m_tWlan_Connection.nEncrypType = 0;
				m_tWlan_Connection.struKeyCfg.bValid = 0;
				break;
		}
	}
	else
	{
		m_tWlan_Connection.struKeyCfg.bValid = 1;//lhl add
		m_tWlan_Connection.struKeyCfg.WPAKey.bValid = 1;
        //pConfig->GetValue(INet_CfgSection, INet_WLANWPAKey_key, "", m_tWlan_Connection.struKeyCfg.WPAKey.szKey, WLAN_MAX_KEY_LEN - 1);
		WLAN_GetWifiKey(m_tWlan_Connection.struKeyCfg.WPAKey.szKey);
	}

	pConfig->GetValue(INet_CfgSection, INet_WLANSSID_key, "", m_tWlan_Connection.szSSID, WLAN_MAX_SSID_LEN - 1);
	return 0;
}

int INet::WLAN_SaveConnCfg(CConfig * pConfig)
{
    NET_DBG("m_tWlan_Connection.nAuthMode = %d\n", m_tWlan_Connection.nAuthMode);

	pConfig->SetValue(INet_CfgSection, INet_WLANNetworkType_key, m_tWlan_Connection.nNetworkType);
	pConfig->SetValue(INet_CfgSection, INet_WLANAuthMode_key, m_tWlan_Connection.nAuthMode);
	pConfig->SetValue(INet_CfgSection, INet_WLANEncType_key, m_tWlan_Connection.nEncrypType);
	switch (m_tWlan_Connection.nEncrypType)
	{
		case ET_NONE:
			pConfig->SetValue(INet_CfgSection, INet_WLANEncType_key, m_tWlan_Connection.nEncrypType);
			break;
		case ET_WEP:
			pConfig->SetValue(INet_CfgSection, INet_WLANDefKeyID_key, m_tWlan_Connection.struKeyCfg.nDefKeyID);
			switch (m_tWlan_Connection.struKeyCfg.nDefKeyID)
			{
				case 1:
					pConfig->SetValue(INet_CfgSection, INet_WLANKey1_key, m_tWlan_Connection.struKeyCfg.Key1.szKey);
					break;
				case 2:
					pConfig->SetValue(INet_CfgSection, INet_WLANKey2_key, m_tWlan_Connection.struKeyCfg.Key2.szKey);
					break;
				case 3:
					pConfig->SetValue(INet_CfgSection, INet_WLANKey3_key, m_tWlan_Connection.struKeyCfg.Key3.szKey);
					break;
				case 4:
					pConfig->SetValue(INet_CfgSection, INet_WLANKey4_key, m_tWlan_Connection.struKeyCfg.Key4.szKey);
					break;
				default:
					break;
			}
			break;
		default:
            //pConfig->SetValue(INet_CfgSection, INet_WLANWPAKey_key, m_tWlan_Connection.struKeyCfg.WPAKey.szKey);
			WLAN_SaveWifiKey(m_tWlan_Connection.struKeyCfg.WPAKey.szKey);
            break;
	}
	pConfig->SetValue(INet_CfgSection, INet_WLANSSID_key, m_tWlan_Connection.szSSID);
	saveWlanSSIDBak(WIFI_CFG_INI,m_tWlan_Connection.szSSID);
	WLAN_SaveWifiKey(m_tWlan_Connection.struKeyCfg.WPAKey.szKey);
    return 0;
}
#endif

void TruncateIPString(char *szIPString)
{
	char *p = szIPString;

	while (('\0' != *p) && ('\n' != *p) && ('\r' != *p))
	{
		p++;
	}
	*p = '\0';
}

static int isValidPort(unsigned short nPort)
{
	if(0 == nPort)
		return -1;
	else
		return 0;
}

void INet::Net_SetZwP2pPushInit(int isInited)
{
	m_bZWPushInit = isInited;
}

int INet::Net_ZwP2pPushEvent(ZWP2P_UINT32 nEventType, 
	ZWP2P_UINT32 nChannel, LPZWP2P_PUSH_EVENT_INFO pEventInfo)
{
	ZWP2P_UINT32 zwRet = 0;
	
#if defined(__IS_SUPPORT_DYNAMIC_GRAPHICS__)
	CDynamicGraphics *pCDynamicGraphics = CDynamicGraphics::GetInstance();
	pEventInfo->nSendMode = 2;
#endif

#if defined (__USE_P2P_PUSH__)
	if (m_bZWPushInit)
	{
        if (eDeviceEventTypeKeyToCall == nEventType || eDeviceCustomEventTypeStopKey2Call == nEventType)
        {
            if (0 == m_acDomainSuffixExist )
            {
                pEventInfo->dwEventCH = NORMAL_EVENT_CHANNEL;
            }
            else
            {
                pEventInfo->dwEventCH = REALTIME_EVENT_CHANNEL;
            }
        }
        else
        {
            pEventInfo->dwEventCH = NORMAL_EVENT_CHANNEL;
        }
#if defined (__AUTH_V2__)
		zwRet = ZWP2PPUSHVNClient_PushEvent_V2(nEventType, nChannel, pEventInfo);
		NET_DBG("ZWP2PPUSHVNClient_PushEvent_V2 zwRet=%d cH[%d] TYPE[%d]\n", zwRet, pEventInfo->dwEventCH, nEventType);
#else
		zwRet = ZWP2PPUSHVNClient_PushEvent(nEventType, nChannel, pEventInfo);
		NET_DBG("ZWP2PPUSHVNClient_PushEvent zwRet=%d cH[%d] TYPE[%d]\n", zwRet, pEventInfo->dwEventCH, nEventType);
#endif

#if defined(__IS_SUPPORT_DYNAMIC_GRAPHICS__)
	pCDynamicGraphics->PushEvent(nEventType,nChannel,pEventInfo);
#endif
	}
	else
	{
		NET_ERR("ZWP2PPUSHVNClient not init!!!\n");
		zwRet = -1;
	}
#else
	zwRet = -1;
#endif

	return (int)zwRet;
}

#if defined (__IS_SUPPORT_P6SMESSAGE__)
// 获取云消息URL
int INet::NET_GetMpServerParamCfg(char *ServerIpAndPort, char *UrlPathTimeCheck, char *UrlPathEventPolicy, char *UrlPathEventPost, char *UrlPathSetXmlStatus, char *SrvMaintenance)
{
	int nRet = 0;

	CConfig *pCfg = CConfig::GetInstance();

	if (pCfg != NULL)
	{
		pCfg->GetValue(P6S_SERVER_PARAM, "ServerIpAndPort", "", ServerIpAndPort, 128);
		pCfg->GetValue(P6S_SERVER_PARAM, "UrlPathTimeCheck", "", UrlPathTimeCheck, 128);
		pCfg->GetValue(P6S_SERVER_PARAM, "UrlPathEventPolicy", "", UrlPathEventPolicy, 128);
		pCfg->GetValue(P6S_SERVER_PARAM, "UrlPathEventPost", "", UrlPathEventPost, 128);
		pCfg->GetValue(P6S_SERVER_PARAM, "UrlPathSetXmlStatus", "", UrlPathSetXmlStatus, 128);
		pCfg->GetValue(P6S_SERVER_PARAM, "SrvMaintenance", "", SrvMaintenance, 128);
	}
	else
	{
		//读取配置文件失败
		NET_ERR("no CConfig instance\n");

		nRet = -1;
	}

	return nRet;
}

// 设置云消息URL
int INet::NET_SetMpServerParamCfg(char *ServerIpAndPort, char *UrlPathTimeCheck, char *UrlPathEventPolicy, char *UrlPathEventPost, char *UrlPathSetXmlStatus, char *SrvMaintenance)
{
	int nRet = 0;

	//获取新的配置文件读取实例
	CConfig *pCfg = CConfig::GetInstance();

	if (pCfg != NULL)
	{
		pCfg->SetValue(P6S_SERVER_PARAM, "ServerIpAndPort", ServerIpAndPort);
		pCfg->SetValue(P6S_SERVER_PARAM, "UrlPathTimeCheck", UrlPathTimeCheck);
		pCfg->SetValue(P6S_SERVER_PARAM, "UrlPathEventPolicy", UrlPathEventPolicy);
		pCfg->SetValue(P6S_SERVER_PARAM, "UrlPathEventPost", UrlPathEventPost);
		pCfg->SetValue(P6S_SERVER_PARAM, "UrlPathSetXmlStatus", UrlPathSetXmlStatus);
		pCfg->SetValue(P6S_SERVER_PARAM, "SrvMaintenance", SrvMaintenance);
	}
	else
	{
		//读取配置文件失败
		NET_ERR("no CConfig instance\n");

		nRet = -1;
	}

	return nRet;
}
#endif

#ifdef __IS_SUPPORT_AIC_BLUETOOTH__

static int loadBlueToothServer()
{
	int load_count = 0;
	if (0 == IsStringInCommand("ps", "lbh_server"))
	{
		NET_DBG("load lbh_server \n");
		/* 会读取同目录下的 `ble_userconfig.json` ，后者使用标准不带注释的json语法 */
		if(mysystem("lbh_server -p /usr/sbin/ble_userconfig.json -s ble usb &"))
		{
			NET_ERR("lbh_server load fail \n");
			return -1;
		}
		while(0 == IsStringInCommand("ps", "lbh_server"))
		{
			sleep(1);
			load_count++;
			if(load_count%7 == 0)
			{
				mysystem("lbh_server -p /usr/sbin/ble_userconfig.json -s ble usb &");
				NET_ERR("reload lbh_server\n");
			}
			if(load_count >= 21)
			{
				/* 再次加载三次，如果没有加载出来，应该是加载不出来了 */
				NET_ERR("lbh_server thrice load fail\n");
				break;
			}
		}
	}
	return 0;
}

static int unloadBlueToothServer()
{
	if (1 == IsStringInCommand("ps", "lbh_server"))
	{
		if(mysystem("killall -9 lbh_server"))
		{
			NET_ERR("kill lbh_server fail \n");
			return -1;
		}
	}
	return 0;
}

//蓝牙收数据回调
//注意: 不可在此接口内部调用XRSBLE协议库中其他接口，否则将死锁
int INet::BlueToothRecvData(XRSBLE_L2CmdData *pCmdData)
{
    int nRet = -1;
    if (NULL == pCmdData)
    {
        return -1;
    }
    if (NULL != m_cbBlueToothRecvDataCb)
    {
        nRet = m_cbBlueToothRecvDataCb(pCmdData);
    }
    //将处理返回值返回去
    return nRet;
}

//设置应用层蓝牙收数据回调接口
int INet::NET_SetAppBlueToothRecvCallback(AppRecvProcessCallback pFunc)
{
    if (NULL == pFunc)
    {
        return -1;
    }

    m_cbBlueToothRecvDataCb = pFunc;
    return 0;
}

int INet::setBlueTestStatus(int status)
{
	m_blueProTestStatus = status;
	return 0;
}
int INet::getBlueTestStatus(int *status)
{
	*status = m_blueProTestStatus;
	return m_blueProTestStatus;
}
int INet::WLAN_setBluetoothProTestClient(int status)
{
	m_isProTestClient = status;
	return 0;
}

// 开始搜索其他的蓝牙设备
int INet::WLAN_BluetoothScanDev()
{
	BluetoothScan();
	return 0;
}
// 开始连接指定mac的蓝牙设备
int INet::WLAN_BluetoothMacConnectDev(uint8_t *macAddr,uint8_t type)
{
	if(BluetoothConnectDev(macAddr,type))
	{
		return -1;
	}
	return 0;
}

int INet::WLAN_BluetoothDisconnectDev()
{
	if(BluetoothDisconnectDev())
	{
		return -1;
	}
	return 0;
}

int INet::WLAN_BluetoothConnectStatus()
{
	return get_bt_client_con_status();
}


static void *btProTestFuncThread(void *lParam)
{
    PRT_TID();
	SelectSleepSec(5);
	INet *pNet = NULL;
	int DevTeststatus = 0;  // 1:被连接设备   0：发起连接的设备
	int btTestAudio = 1;    // 是否播报语音 1：播报 ， 0：不播报
	char btMacStr[32] = { 0 };   /* 专业安防的蓝牙模块是通过MAC地址来进行连接，*/
	char btMacHead[16] = { 0 };
	char btDevName[32] = { 0 };
	uint8_t szMac[6] = { 0 };
	char valueTemp[32] = { 0 };
	uint8_t addrtype = 0;
	int ble_status = 0;
	pNet = INet::GetInstance();
	if(0 == access(PRODUCTTEST_FILE, 0))
    {
		memset(valueTemp, 0, sizeof(valueTemp));
    	if (0 == GetIniSectionItem(PRODUCTTEST_FILE, BLUE_CfgSection, BLUE_DEV_TYPE, valueTemp, sizeof(valueTemp)))
        {
			DevTeststatus = atoi(valueTemp);
		}
		else
		{
			DevTeststatus = 0;
		}
		memset(valueTemp, 0, sizeof(valueTemp));
		if (0 == GetIniSectionItem(PRODUCTTEST_FILE, BLUE_CfgSection, BLUE_TEST_MAC, valueTemp, sizeof(valueTemp)))
        {
			strncpy(btMacStr, valueTemp, sizeof(btMacStr));
		}
		else
		{
			strncpy(btMacStr, "01:02:03:04:05:06", sizeof(btMacStr));
		}
		if(NULL == strstr(btMacStr,BLUE_TOOTH_DEV_NAME_PRX))
		{
			sscanf(btMacStr, "%02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx", &szMac[0],&szMac[1],&szMac[2],&szMac[3],&szMac[4],&szMac[5]);
			szMac[5] += 1;
		}
		else {
			sscanf(btMacStr, "%s_%02hhx_%02hhx_%02hhx_%02hhx_%02hhx_%02hhx", btMacHead,&szMac[0],&szMac[1],&szMac[2],&szMac[3],&szMac[4],&szMac[5]);
			szMac[5] += 1;
		}
		NET_INFO("=== %s   %02x===\n",btMacStr,szMac[5]);
		memset(valueTemp, 0, sizeof(valueTemp));
		if (0 == GetIniSectionItem(PRODUCTTEST_FILE, BLUE_CfgSection, BLUE_DEV_NAME, valueTemp, sizeof(valueTemp)))
        {
			strncpy(btDevName, valueTemp, sizeof(btDevName));
		}
		else
		{
			strncpy(btDevName, "ZWBLE_altobeam", sizeof(btDevName));
		}
		
		NET_INFO("status = %d mac = %s audio = %d devName = %s \n",DevTeststatus,btMacStr,btTestAudio,btDevName);
		if(DevTeststatus == 1)
		{
			int count = 0;
			int clientConSuc = 0;
			printf("bt pro server\n");
			pNet->setBlueTestStatus((int)btProTestfalse);
			pNet->WLAN_BluetoothOpen();
			// ResetWifidriverWhenWifiFault();   // 测试卸载驱动然后在加载，会不会影响蓝牙的功能
			while(1)
			{
				SelectSleepSec(1);
				/* 等待被测试设备发送数据过来 */
				clientConSuc = pNet->WLAN_BluetoothConnectStatus();
				if(btTestAudio && clientConSuc)
				{
					count = 0;
					clientConSuc = 0;
					ZWSys_SetSpeechContent(SPEECH_SYS_QRCODE_SCAN_OVER);
					// 循环查询，等待播放结束
					int i = 0;
					while (1)
					{
						SelectSleepMSec(100);
						if (0 == ZWSys_GetSpeechPlayingStatus())
						{
							break;
						}
						if (++i > (100))
						{
							i = 0;
							break;
						}
					}
				}
				if(++count > 60)
				{
					count = 0;
					/* 一分钟没有连接，就播放等待连接 */
					ZWSys_SetSpeechContent(SPEECH_WIFI_PLEASE_CONNECT);
					// 循环查询，等待播放结束
					int i = 0;
					while (1)
					{
						SelectSleepMSec(100);
						if (0 == ZWSys_GetSpeechPlayingStatus())
						{
							break;
						}
						if (++i > (100))
						{
							i = 0;
							break;
						}
					}
				}
			}
		}
		else if(DevTeststatus == 0)
		{
			int getStatusCnt = 0;
			NET_INFO("bt pro client\n");
			pNet->WLAN_BluetoothOpen();
			SelectSleepSec(2);
			/* 加载产测的client */
			/* 拉线程进行连接 */
			NET_INFO("connect mac = %02x:%02x:%02x:%02x:%02x:%02x\n", szMac[0],szMac[1],szMac[2],szMac[3],szMac[4],szMac[5]);
			pNet->WLAN_BluetoothMacConnectDev((uint8_t *)szMac,addrtype);
			while(getStatusCnt++ < 10)
			{
				ble_status = pNet->WLAN_BluetoothConnectStatus();
				if(ble_status)
				{
					break;
				}
				SelectSleepSec(1);
			}
			if(ble_status == 1 && btTestAudio == 1)
			{
				ZWSys_SetSpeechContent(SPEECH_SYS_QRCODE_SCAN_OVER);
				// 循环查询，等待播放结束
				int i = 0;
				while (1)
				{
					SelectSleepMSec(100);
					if (0 == ZWSys_GetSpeechPlayingStatus())
					{
						break;
					}
					if (++i > (100))
					{
						i = 0;
						break;
					}
				}
				pNet->setBlueTestStatus((int)btProTestTrue);
			}
			else{
				pNet->setBlueTestStatus((int)btProTestfalse);
			}
			pNet->WLAN_BluetoothDisconnectDev();
		}
    }
	else
	{
		/* 没有产测的时候，默认是1 */
		pNet->setBlueTestStatus((int)btProTestfalse);
		NET_INFO("no pro file\n");	
	}
	return NULL;
}


#endif

//开启蓝牙
int INet::WLAN_BluetoothOpen()
{

	if (m_bIsBlueToothOpen)
	{
		NET_INFO("Blue is open\n");
	    return 0;
	}
	
	time_t cur_tm = time(NULL); // 获取当前时间
	Bluetoothstarttime = (int)cur_tm;

#ifdef __IS_SUPPORT_HIS_BLUETOOTH__
	if (WIFI_MODULE_HI_H383U == m_eCurrUSBWifiDev)
	{

		His_BLE_setBluetoothName((const char*)g_nBleName);
		His_BLE_registerBluetoothCallback(ParseBleDataCallback);
		His_BLE_startBluetoothWork();
		m_bIsBlueToothOpen = 1;
		NET_INFO("[testblue] open ! \n");
		return 0;
	}
#endif		
		


#ifdef __IS_SUPPORT_AIC_BLUETOOTH__

    int nRet = 0;
	BluetoothParam stBtParam = {0};
	TutkCfg stuTutkCfg = {0};
	unsigned long ulTutkStatus = 0;	
	char szTmp[64] = {0};
	uint8_t szMac[6] = { 0 };
	char szMacStr[32] = { 0 };
	char *pTmp = NULL;
	XRSBLE_L2InitParam stL2InitParam = {0};
	uint16_t uidCheckFlag = 0;

	NET_INFO("blue tooth open\n");
	while(uidCheckFlag++ < 20)
	{
		ulTutkStatus = 0;
		memset(&stuTutkCfg, 0, sizeof(stuTutkCfg));
		memset(szTmp, 0, sizeof(szTmp));
		NET_GetTutkUID(&stuTutkCfg, &ulTutkStatus);
		pTmp = strchr(stuTutkCfg.szTutkUID, ',');
		if (NULL != pTmp)
		{
			memcpy(szTmp, stuTutkCfg.szTutkUID, (int)(pTmp-stuTutkCfg.szTutkUID));
		}
		else
		{
			strncpy(szTmp, stuTutkCfg.szTutkUID, sizeof(szTmp) - 1);
		}
		if (strlen(szTmp) > 10)
		{
			snprintf(stBtParam.szDeviceName, sizeof(stBtParam.szDeviceName), "%s%s", 
				BLUE_TOOTH_DEV_NAME_PRX, szTmp);
				break;
		}
		else
		{
			usleep(200000);
		}
	}
	NET_GetMacAddress(szMacStr);
	NET_INFO("---my adv mac1 = %s \n", szMacStr);
	sscanf(szMacStr, "%02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx", &szMac[0],&szMac[1],&szMac[2],&szMac[3],&szMac[4],&szMac[5]);
	szMac[0] = ((szMac[0] % 16) * 16) + (szMac[0] / 16);
	szMac[1] = ((szMac[1] % 16) * 16) + (szMac[1] / 16);
	NET_DBG("bt addr = %02x %02x %02x %02x %02x %02x\n", szMac[0], szMac[1], szMac[2], szMac[3], szMac[4], szMac[5]);

	if (WIFI_MODULE_AIC_8800DL == m_eCurrUSBWifiDev ||WIFI_MODULE_AIC_8800DL_2_4G == m_eCurrUSBWifiDev)
	{
		loadBlueToothServer();
		usleep(200000);
	NET_INFO("bt name = %s \n", stBtParam.szDeviceName);
		//打开蓝牙
		stBtParam.pCallbackRecvData = XRSBLE_L1_RecvProcess;
		nRet = BluetoothOpen(&stBtParam, szMac);
		if (0 != nRet)
		{
			NET_ERR("BluetoothOpen err: nRet=%d\n", nRet);
			return -1;
		}
	NET_INFO("blue open success\n");
	}

    //初始化蓝牙通信协议
    stL2InitParam.pAppRecvProcessCallback = BlueToothRecvData;
	stL2InitParam.pBleSendDataCallback = BluetoothSendData;
	nRet = XRSBLE_L2_Init(&stL2InitParam);
	if (0 != nRet)
    {
		NET_ERR("XRSBLE_L2_Init err: nRet=%d\n", nRet);
		BluetoothClose();
		return -2;
	}

    m_bIsBlueToothOpen = 1;

#endif

    return 0;
}

//关闭蓝牙
int INet::WLAN_BluetoothClose()
{

    if (0 == m_bIsBlueToothOpen)
	{
		NET_INFO("Blue is close\n");
	    return 0;
	}
#ifdef __IS_SUPPORT_HIS_BLUETOOTH__

	if (WIFI_MODULE_HI_H383U == m_eCurrUSBWifiDev)
	{

		NET_INFO("blue tooth close\n");
		His_BLE_stopBluetoothWork();
	    m_bIsBlueToothOpen = 0;
		return 0;
		
	}
#endif


#ifdef __IS_SUPPORT_AIC_BLUETOOTH__

    int nRet = 0;
    //蓝牙通信协议逆初始化
	nRet = XRSBLE_L2_UnInit();
	if (0 != nRet)
    {
		NET_ERR("XRSBLE_L2_UnInit err: nRet=%d\n", nRet);
		return -1;
	}

	//关闭蓝牙
	//注意：此接口会阻塞3S左右
	nRet = BluetoothClose();
	if (0 != nRet)
	{
		NET_ERR("BluetoothClose err: nRet=%d\n", nRet);
		return -2;
	}
	unloadBlueToothServer();
	NET_INFO("blue tooth close\n");

    m_bIsBlueToothOpen = 0;
#endif

    return 0;
}

int INet::BluetoothIsOpen()
{
    return m_bIsBlueToothOpen;
}

int INet::Get_BluetoothStartTime()
{

	if(Bluetoothstarttime == 0)
	{
		return -1;
	}
	else
	{
		time_t cur_tm = time(NULL); // 获取当前时间
        int running_time = (int)(cur_tm - Bluetoothstarttime);
        return running_time; // 返回蓝牙启动到现在的时间（秒数）
	}
    return 0;
}




#ifdef __MODULE_4G_SUPPORT__
/* 获取DID ,逗号之前的 */
char *INet::NET_4G_GetTutkUID(TutkCfg *pTutkCfg)
{
	char *pPushIdStartPos = NULL;

	if (NULL == pTutkCfg)
    {
        NET_ERR("NULL == pTutkCfg\n");
        return NULL;
    }

	memset(pTutkCfg->szTutkUID, 0, sizeof(pTutkCfg->szTutkUID));
    
	pPushIdStartPos = strchr(m_sTutkUID, ',');
	if (NULL != pPushIdStartPos)
	{
		memcpy(pTutkCfg->szTutkUID, m_sTutkUID, (int)(pPushIdStartPos-m_sTutkUID));
	}

	return pTutkCfg->szTutkUID;
}

int INet::SetConnServerStatus(int ConnServer)
{
	m_iConnServer = ConnServer;
	NET_ERR("m_iConnServer :%d \n", m_iConnServer);
    return 0;
}

//P2P产测线程
static void* P2PProductTestThread(void * lParam)
{
    PRT_TID();

	NET_DBG(">>>>>>>>>> enter, pid = %d\n", getpid());
	BEGINRUN_THREAD;

	int pHttpHandler = 0;
	ServerInfo sServerInfo;
	HTTP_4G_DEVICE_P2P sReportInfo;
	unsigned int nAuthState = 0;
    unsigned char szAuthCode[256] = {0};
    char value[64] = {0};
    int count = 0;
	int nActiveStutusCode = 0;
	int nRet = 0;
	//获取当前实例
	INet *pNet = (INet *)lParam;
    e4GModuleType moduletype = UNKNOW_MODULE;
    int nBoardID = 0;
    const char *nBoardType = "";

	memset(&sServerInfo,0,sizeof(sServerInfo));
	memset(&sReportInfo,0,sizeof(sReportInfo));

	pNet->NET_P2P_ProductTestGetUID(sReportInfo.did);
	pNet->NET_P2P_ProductTestGetPcMac(sReportInfo.pcMac);
	pNet->NET_GetMacAddress(sReportInfo.deviceMac);

    sReportInfo.deviceType = PUB_FindDeviceTypeInfoById(nBoardID);
	ZWSys_GetSerialNum((unsigned char *)sReportInfo.sn);

    /* 获取当前设备的版型ID */
    ZWSys_GetBoardInfo(&nBoardID);
    /* 获取当前设备的版型类型 */
    nBoardType = PUB_FindBoardTypeInfoById(nBoardID);
    /* 获取当前设备的授权状态 */
    ZWSys_GetAuthInfo(szAuthCode, &nAuthState);
    /* 获取当前设备的激活码 */
    ZWSys_GetActiveStatusCode(&nActiveStutusCode);

	nRet = read_profile_string("SystemFunction", "FunctionFullType",  value,sizeof(value),"",CUSTOM_CONFIG_FILE);
	//第一优先级判断全定制，为空则判断第二优先级
	if (strlen(value) > 0)
	{
		memset(sReportInfo.firmwareVersion,0, sizeof(sReportInfo.firmwareVersion));
		snprintf(sReportInfo.firmwareVersion, sizeof(sReportInfo.firmwareVersion), "%s", value);
	}
    else
	{
		nRet = read_profile_string("SystemFunction", "FunctionType",  value,sizeof(value),"",CUSTOM_CONFIG_FILE);		
        if ((0 == strncmp(value, nBoardType, strlen(nBoardType))) && (1 == nAuthState))
        {
            if (nActiveStutusCode & CAPABILITY_EXTERNSIMCARD)
            {
                InsertStringAtFirstDash(nBoardType, "L", sReportInfo.firmwareVersion, sizeof(sReportInfo.firmwareVersion));
            }
            else
            {
                while (1)
                {
                    if ((1 == Module4G_IsModuleTypeSet()) || count++ >= 3)
                    {
                       break;
                    }
                    sleep(1);
                }
                ZWSys_GetModuleType(&moduletype);

                if (QUECTEL_EG800 == moduletype || ZHONGYUN_ZX800SG_S == moduletype || ZHONGYUN_ZX800HG_S == moduletype)
                {
                    if (1 == pNet->Net_GetIsCloudCardDev())
                    {
                        InsertStringAtFirstDash(nBoardType, "SUY", sReportInfo.firmwareVersion, sizeof(sReportInfo.firmwareVersion));
                    }
                    else
                    {	
                        InsertStringAtFirstDash(nBoardType, "SU", sReportInfo.firmwareVersion, sizeof(sReportInfo.firmwareVersion));
                    }
                }
				else if (ZHONGYUN_ZX800SG_A == moduletype || ZHONGYUN_ZX800HG_A == moduletype)
                {
                    if (1 == pNet->Net_GetIsCloudCardDev())
                    {
                        InsertStringAtFirstDash(nBoardType, "SAY", sReportInfo.firmwareVersion, sizeof(sReportInfo.firmwareVersion));
                    }
                    else
                    {	
                        InsertStringAtFirstDash(nBoardType, "SA", sReportInfo.firmwareVersion, sizeof(sReportInfo.firmwareVersion));
                    }
                }
				else if (ZHONGYUN_ZX800SG == moduletype || ZHONGYUN_ZX800HG == moduletype)
				{
                    if (1 == pNet->Net_GetIsCloudCardDev())
                    {
                        InsertStringAtFirstDash(nBoardType, "SY", sReportInfo.firmwareVersion, sizeof(sReportInfo.firmwareVersion));
                    }
                    else
                    {	
                        InsertStringAtFirstDash(nBoardType, "S", sReportInfo.firmwareVersion, sizeof(sReportInfo.firmwareVersion));
                    }
                }
                else
                {
                    if (1 == pNet->Net_GetIsCloudCardDev())
                    {
                        InsertStringAtFirstDash(nBoardType, "SY", sReportInfo.firmwareVersion, sizeof(sReportInfo.firmwareVersion));
                    }
                    else
                    {
                        InsertStringAtFirstDash(nBoardType, "S", sReportInfo.firmwareVersion, sizeof(sReportInfo.firmwareVersion));
                    }
                }
            }
        }
	}

	//strncpy(sServerInfo.serverAddr,"p6sproduct-test.p6sai.com",sizeof(sServerInfo.serverAddr)-1);  //测试服务器
	strncpy(sServerInfo.serverAddr,"p6sproduct.p6sai.com",sizeof(sServerInfo.serverAddr)-1);		//正式服务器
	sServerInfo.nPort = 8089;
	sServerInfo.isHttps = 1;
	sServerInfo.connectTimeOutMs = 6000;
	sServerInfo.waitResponseTimeOutMs = 6000;
	ZWHTTPCLIENT_CurlInit();
	
	while (1)
	{	
		NET_DBG("pNet->m_iTutkStatus :%d \n", pNet->m_iTutkStatus);
		if (eP2PStauts_InternetReady == pNet->m_iTutkStatus) //P2P就绪
		{
#if 1	
			nRet = ZWHTTPCLIENT_Init(&pHttpHandler);
			if (0 != nRet)
			{
				NET_ERR("ZWHTTPCLIENT_Init failed, nRet = %d\n",nRet);
				sleep(1);
				continue;
			}
			//1s发送一次
			ZWHTTPCLIENT_HttpVisitor(pHttpHandler,&sServerInfo,HTTP_POST,CMD_TYPE_4G,(char *)&sReportInfo,sizeof(sReportInfo),NULL,0);
			ZWHTTPCLIENT_UnInit(pHttpHandler);
#endif	
		}
		// 恢复出厂,停止转发
		if(0 == pNet->m_iConnServer)
		{
			NET_ERR("recieve factory command, will stop conn \n");
			break;
		}
		sleep(1);
	}
	
	ZWHTTPCLIENT_CurlUnInit();

	NET_DBG(">>>>>>>>>> exit, pid = %d\n", getpid());
	ENDRUN_THREAD;

	return NULL;
}

//获取uid
int INet::NET_P2P_ProductTestGetUID(char *pUid)
{
	if (NULL == pUid)
	{
		NET_ERR("NULL == pUid\n");
		return -1;
	}

	memcpy(pUid,m_sTutkUID,sizeof(m_sTutkUID));

	return 0;
}

//获取pcmac
int INet::NET_P2P_ProductTestGetPcMac(char *pMac)
{
	if (NULL == pMac)
	{
		NET_ERR("NULL == pUid\n");
		return -1;
	}

	memcpy(pMac,m_PcMac,sizeof(m_PcMac));

	return 0;
}

//初始化P2P产测
void INet::NET_P2P_ProductTestInit()
{
	int nRet = 0;
	pthread_t pThread;
	CConfig *pCfg = CConfig::NewInstance();

	if (NULL == pCfg)
	{
		NET_ERR("NULL == pCfg\n");
		return;
	}

	if (0 == strlen(m_sTutkUID))
	{
		NET_ERR("m_sTutkUID is empty!\n");
		return;
	}

	if (0 == access(PRODUCTTEST_FILE,F_OK))
	{
		nRet = pCfg->LoadFileRO(PRODUCTTEST_FILE);
		if (IDVR_OK != nRet)
		{
			NET_ERR("LoadFileRO %s failed! nRet = %d\n",PRODUCTTEST_FILE,nRet);
			return;
		}
	}

	if (pCfg->CheckSectionExist("PC"))
	{
		pCfg->GetValue("PC", "PCMAC", "", m_PcMac, sizeof(m_PcMac)-1);
	}

	pCfg->ReleaseInstance(pCfg);

	//存在pcmac 认为p2p产测模式
	if (strlen(m_PcMac) > 0)
	{
		m_P2pProductMode = 1;
	}

	if (m_P2pProductMode)
	{
		//设置分辨率为2MP
		NET_P2P_ProductTest_Resolution_Init();
	
		//创建P2P产测线程
		nRet = pthread_create(&pThread, NULL, P2PProductTestThread, this);
        if (nRet < 0)
        {
            NET_ERR("pthread_create<P2PProductTestThread> failed(%d)\n", nRet);
            return;
        }
	}
}

//P2P产测模式下
void INet::NET_P2P_ProductTest_Resolution_Init()
{
	CODEC_PARA_S cp;
	int result = 0;
	HANDLE theEncHandle = ZWSys_EncChannelOpen(0);

	if (NULL == theEncHandle)
	{
		NET_ERR("ZWSys_EncChannelOpen is Failed!\n");
		return;
	}

	//P2P产测设置 分辨率2MP
	memset(&cp,0,sizeof(CODEC_PARA_S));
	cp.BitRate = 1536000;
	cp.BitRateCtlMode = 0;
	cp.FrameRate = 10;
	cp.KeyFrameInterval = 20;
	cp.PicQualityLever = 2;
	cp.PictureRes.m_PicHeight = 1080;
	cp.PictureRes.m_PicWidth = 1920;

	result = ZWSys_EncSetCodecPara(theEncHandle, 0, &cp, LtH265Video);

	if (result)
	{
		NET_ERR("ZWSys_EncSetCodecPara failed (%d)\n", result);
	}

	return;
}

int INet::SetScanEndStatus(int scanend)
{
    m_b4gScanEnd = scanend;
    NET_ERR("m_b4gScanEnd :%d\n", m_b4gScanEnd);
    return 0;
}

int INet::SetUserId(char *userid)
{
    if (NULL == userid)
    {
        return -1;
    }

    memset(m_szUserId, 0, sizeof(m_szUserId));
    memcpy(m_szUserId, userid, sizeof(m_szUserId));
    NET_ERR("m_szUserId :%s\n", m_szUserId);
    return 0;
}

int INet::SetServerAddress(char *serveraddress)
{
    if (NULL == serveraddress)
    {
        return -1;
    }
    
    memset(m_szServerAddress, 0, sizeof(m_szServerAddress));
    memcpy(m_szServerAddress, serveraddress, sizeof(m_szServerAddress));
    NET_ERR("m_szServerAddress :%s\n", m_szServerAddress);
    return 0;
}

int INet::SetRandomNumber(char *randomnumber)
{
    if (NULL == randomnumber)
    {
        return -1;
    }
    
    memset(m_szRandomNumber, 0, sizeof(m_szRandomNumber));
    memcpy(m_szRandomNumber, randomnumber, sizeof(m_szRandomNumber));
    NET_ERR("m_szRandomNumber :%s\n", m_szRandomNumber);
    return 0;
}

//P2P绑定线程
static void* P2P4GBindThread(void * lParam)
{
    PRT_TID();

    pthread_detach(pthread_self());
    
    PRT_TID();
    NET_DBG(">>>>>>>>>> enter, pid = %d\n", getpid());
    BEGINRUN_THREAD;
    /* 等待10S再启动，保证后续状态值正常获取 */
    sleep(10);

    int nRet = 0;
    int jsonBufferLen = 0;
    int pHttpHandler = 0;
    int count = 0;
    int code_value = 0;
    int devicetype = 0;
    int nActiveStutusCode = 0;

    unsigned int nAuthState = 0;
    unsigned long int userid = 0;

    char *pTmp = NULL;
    char *end = NULL; 
    char did[64] = {0};
    char didtmp[64] = {0};
    char iccid[64] = {0};
    char devicemodel[64] = {0};
    char version[64] = {0};
    char value[64] = {0};
    char response[2048] = {0};
    char jsonBuffer[2048] = {0};
    unsigned char szAuthCode[256] = {0};
    DEVICEVERSION dv = {0};
    TransDataServerInfo ServerInfo;
    e4GModuleType moduletype = UNKNOW_MODULE;
    int nBoardID = 0;
    const char *nBoardType = "";
    INet *pNet = (INet *)lParam;

    memset(&ServerInfo,0,sizeof(ServerInfo));

    /* 获取did号 */
    pNet->NET_P2P_ProductTestGetUID(didtmp);
    pTmp = strchr(didtmp, ',');
    if (NULL != pTmp) {
        memcpy(did, didtmp, (int)(pTmp-didtmp));
    } else {
        strncpy(did, didtmp, sizeof(did));
    }
    /* 获取iccid */
    Module4G_GetIccid(iccid);
    /* 获取设备版本信息 */
    MPU_PUB_GetDeviceVersion(&dv, 0);
    strncpy(version, dv.m_szSoftwareVersion, sizeof(version));
    /* 获取当前设备的版型ID */
    ZWSys_GetBoardInfo(&nBoardID);
    /* 获取当前设备的版型类型 */
    nBoardType = PUB_FindBoardTypeInfoById(nBoardID);
    /* 获取当前设备的授权状态 */
    ZWSys_GetAuthInfo(szAuthCode, &nAuthState);
    /* 获取当前设备的激活码 */
    ZWSys_GetActiveStatusCode(&nActiveStutusCode);

    nRet = read_profile_string("SystemFunction", "FunctionFullType",  value,sizeof(value),"",CUSTOM_CONFIG_FILE);
    //第一优先级判断全定制，为空则判断第二优先级
    if (strlen(value) > 0)
    {
        memset(devicemodel,0, sizeof(devicemodel));
        snprintf(devicemodel, sizeof(devicemodel), "%s", value);
    }
    else
    {
        nRet = read_profile_string("SystemFunction", "FunctionType",  value,sizeof(value),"",CUSTOM_CONFIG_FILE);
        if ((0 == strncmp(value, nBoardType, strlen(nBoardType))) && (1 == nAuthState))
        {
            if (nActiveStutusCode & CAPABILITY_EXTERNSIMCARD)
            {
                InsertStringAtFirstDash(nBoardType, "L", devicemodel, sizeof(devicemodel));
            }
            else
            {
                while (1)
                {
                   if ((1 == Module4G_IsModuleTypeSet()) || count++ >= 3)
                   {
                        break;
                   }
                   sleep(1);
                }
                ZWSys_GetModuleType(&moduletype);

                if (QUECTEL_EG800 == moduletype || ZHONGYUN_ZX800SG_S == moduletype || ZHONGYUN_ZX800HG_S == moduletype)
                {
                	//欧版云卡
                    if (1 == pNet->Net_GetIsCloudCardDev())
                    {
                        InsertStringAtFirstDash(nBoardType, "SUY", devicemodel, sizeof(devicemodel));
                    }
                    else
                    {
                        InsertStringAtFirstDash(nBoardType, "SU", devicemodel, sizeof(devicemodel));
                    }
                }
				else if (ZHONGYUN_ZX800SG_A == moduletype || ZHONGYUN_ZX800HG_A == moduletype)
                {
                	//美版云卡
				    if (1 == pNet->Net_GetIsCloudCardDev())
                    {
                        InsertStringAtFirstDash(nBoardType, "SAY", devicemodel, sizeof(devicemodel));
                    }
                    else
                    {
                        InsertStringAtFirstDash(nBoardType, "SA", devicemodel, sizeof(devicemodel));
                    }
				}
				else if (ZHONGYUN_ZX800SG == moduletype || ZHONGYUN_ZX800HG == moduletype)
				{	//亚版云卡
					if (1 == pNet->Net_GetIsCloudCardDev())
					{
                        InsertStringAtFirstDash(nBoardType, "SY", devicemodel, sizeof(devicemodel));
					}
					else
					{
                        InsertStringAtFirstDash(nBoardType, "S", devicemodel, sizeof(devicemodel));
					}
				}
                else
                {
                    if (1 == pNet->Net_GetIsCloudCardDev())
                    {
                        InsertStringAtFirstDash(nBoardType, "SY", devicemodel, sizeof(devicemodel));
                    }
                    else
                    {
                        InsertStringAtFirstDash(nBoardType, "S", devicemodel, sizeof(devicemodel));
                    }
                }
            }
			devicetype = PUB_FindDeviceTypeInfoById(nBoardID);
        }
    }
    //NET_INFO("channel %d, devicetype %0X, did %s, iccid %s, devicemodel %s, version %s\n", channel, devicetype, did, iccid, devicemodel, version);

    ZWHTTPCLIENT_CurlInit();
    while (1)
    {
        if (0 == bNeed4GScanStatue)
        {
            NET_INFO("The device is bound, return now!\n");
            break;
        }
        
        NET_DBG("pNet->m_iTutkStatus :%d \n", pNet->m_iTutkStatus);
        if (eP2PStauts_InternetReady == pNet->m_iTutkStatus && 1 == pNet->m_b4gScanEnd) //P2P就绪且扫码完成
        {
            nRet = ZWHTTPCLIENT_Init(&pHttpHandler);
            if (0 != nRet)
            {
                NET_ERR("ZWHTTPCLIENT_Init failed, nRet = %d\n",nRet);
                sleep(1);
                continue;
            }

            memset(jsonBuffer, 0, sizeof(jsonBuffer));
            memset(response, 0, sizeof(response));
            userid = strtoul(pNet->m_szUserId, &end, 10); // 假设是十进制

            snprintf(ServerInfo.serverUrl, sizeof(ServerInfo.serverUrl), "https://%s", pNet->m_szServerAddress);
            NET_INFO("serverUrl:%s\n", ServerInfo.serverUrl);
            ServerInfo.isHttps = true;
            ServerInfo.connectTimeOutMs = 10000;
            ServerInfo.waitResponseTimeOutMs = 10000; 
            eZWHTTP_METHOD method = HTTP_POST;
            char header[128] = "Content-Type:application/json";
            ZWHTTPCLIENT_HttpAddHeader(pHttpHandler, header);
            
            // 创建一个cJSON对象
            cJSON *root = cJSON_CreateObject();
            // 向cJSON对象中添加字段
            cJSON_AddNumberToObject(root, "userId", userid);
            cJSON_AddStringToObject(root, "code", pNet->m_szRandomNumber);
            cJSON_AddStringToObject(root, "did", did);
            cJSON_AddStringToObject(root, "deviceModel", devicemodel);
            cJSON_AddNumberToObject(root, "channel", ZWSys_GetMaxSensorCnt());
            cJSON_AddStringToObject(root, "version", version);
            cJSON_AddNumberToObject(root, "deviceType", devicetype);
            cJSON_AddStringToObject(root, "iccId", iccid);
            // 将cJSON对象转换为字符串并打印
            char *rendered = cJSON_Print(root);
            strncpy(jsonBuffer, rendered, sizeof(jsonBuffer));
            // 释放cJSON对象占用的内存
            cJSON_Delete(root);
            free(rendered);

            jsonBufferLen = strlen(jsonBuffer) + 1;
            NET_INFO("jsonBufferLen:%d, jsonBuffer:%s\n", jsonBufferLen, jsonBuffer);
       
            nRet = ZWHTTPCLIENT_HttpTransparentData(pHttpHandler, &ServerInfo, method, jsonBuffer, jsonBufferLen, response, sizeof(response));
            if (CURL_OK == nRet || CURL_200_OK == nRet)
            {
                NET_INFO("response: %s\n", response);
                //根据code值完成逻辑
                cJSON * json = cJSON_Parse(response);
                if (NULL == json)
                {
                    const char *error_ptr = cJSON_GetErrorPtr();
                    if (error_ptr != NULL) 
                    {
                       fprintf(stderr, "Error before: %s\n", error_ptr);
                    }
                }

                cJSON * JsonOperate = cJSON_GetObjectItem(json, "code");
                if (JsonOperate != NULL) 
                {
                    code_value = JsonOperate->valueint;
                    NET_INFO(" Code: %d\n", code_value);
                }
                else
                {
                    NET_INFO("Failed to find code or it's not a number.\n");
                }
                
                // 清理
                cJSON_Delete(json);

                if (CODE_200_OK == code_value)
                {
                    //播报语音上报信息成功
                    NET_INFO("Successfully reported to the server.\n");
                }
                else if (CODE_RANDOM_ERR == code_value)
                {
                    //播报语音提示用户刷新二维码
                    NET_INFO("The random code has already been used.\n");
 
                }
                else if (CODE_PARAMETER_ERR == code_value)
                {
                    NET_INFO("Request parameters are empty or format is incorrect.\n");
                }
                else if (CODE_SYSTEM_ERR == code_value)
                {
                    NET_INFO("System Error\n");
                }
                pNet->m_b4gScanEnd = 0;

            }
            else if (CURL_TIME_OUT == nRet || CURL_RESOLVE_HOST_NAME_FAILED == nRet || CURL_SSL_CONNECT_ERR == nRet)
            {
                NET_ERR("ZWHTTPCLIENT_HttpTransparentData TimeOut Or CurlResolveHostNameFailed or CurlSslConnectErr, nRet = %d\n",nRet);
                sleep(3);
            }
            else
            {
                NET_ERR("ZWHTTPCLIENT_HttpTransparentData Err!, nRet = %d\n",nRet);
                pNet->m_b4gScanEnd = 0;
            }
            
            ZWHTTPCLIENT_UnInit(pHttpHandler);
        }
        sleep(1);
    }
    
    ZWHTTPCLIENT_CurlUnInit();

    NET_DBG(">>>>>>>>>> exit, pid = %d\n", getpid());
    ENDRUN_THREAD;

    return NULL;
}

//初始化扫码绑定流程
void INet::NET_P2P_4GBindInit()
{
    int nRet = 0;
    char szSCode[64] = {0};
    int nSCodeLen = sizeof(szSCode);
    pthread_t pThread;
    
    int rc = MPU_SYS_DeviceBind_GetSCode(szSCode, &nSCodeLen);
    if ((0 != rc) || (0 == strlen(szSCode)))
    {
		Module4G_SetBindMode(0);
        NET_WARN("The device is not bound!\n");
    }
    else
    {
		Module4G_SetBindMode(1);
        NET_ERR("The device is bound, return now!\n");
        return;
    }
    
    
    if (0 == strlen(m_sTutkUID))
    {
        NET_ERR("m_sTutkUID is empty!\n");
        return;
    }

    //创建扫码绑定线程
    nRet = pthread_create(&pThread, NULL, P2P4GBindThread, this);
    if (nRet < 0)
    {
        NET_ERR("pthread_create<P2P4GBindThread> failed(%d)\n", nRet);
        return;
    }
}

//初始化4G相关数据
void INet::NET_4GModule_Init()
{
	int nActiveStutusCode = 0;
    int bSdkSupport4G = 0;

    if (bSupportGPRS)
    {
    	//不支持金睿卡
    	m_CardTypeTmp = 0;
        //设备开机允许使用有线网络半小时
    	m_nAllowUseEth0 = 1;

    	//初始化产测状态
    	if (0 == access(PRODUCTTEST_FILE,F_OK))
    	{
    		//有产测文件，前30分钟可以使用有线网络
    		m_nProductState = 1;
    		NET_P2P_ProductTestInit();
    	}
    	else
    	{
    		m_nProductState = 0;
            NET_P2P_4GBindInit();
    	}

    	//根据激活码，初始化4G模块工作模式
    	ZWSys_GetActiveStatusCode(&nActiveStutusCode);

        if (nActiveStutusCode & CAPABILITY_EXTERNSIMCARD)
        {
    		m_nSimCardBindState = BINDING_MODE;
			
			NET_DBG("ZWSys_GetActiveStatusCode , BINDING_MODE\n");
        }
        else
        {
    		m_nSimCardBindState = UNBINDING_MODE;
			NET_DBG("ZWSys_GetActiveStatusCode , UNBINDING_MODE\n");
        }

    	Module4G_Init(); 

    	if (BINDING_MODE == m_nSimCardBindState)
    	{
        	if (0 == access(MODULE_4G_BIND_FLAG,F_OK))
    	    {
    	    	//解绑了，认为是标准模式
    			m_IsBindState = 0;
    			m_nSimCardBindState = UNBINDING_MODE;

    			Module4G_LinkSpeechControl(LINKSPEECH_NOTORIGINAL_UNBIND);
    	    }
        }
        else
        {
    		Module4G_LinkSpeechControl(LINKSPEECH_NOTORIGINAL_UNBIND);
        }

        //根据DID获取客户类型
        ZWSys_DidForCustomerType(m_sTutkUID,&m_FlowCustomType);
        NET_INFO("CustomerType = %d\n",m_FlowCustomType); 
        if (CUSTOMTYPE_LONGTU == m_FlowCustomType)
    	{
    		//隆图客户不支持金睿卡
    		m_CardTypeTmp = 0;

    		//隆图客户不支持北纬
    		m_CardTypeTmp_BW = 0;
    	}
    	else if ((CUSTOMTYPE_SUNYIYANG == m_FlowCustomType) ||
    			 (CUSTOMTYPE_YUNRUI == m_FlowCustomType))
    	{
    		//森亦杨、云瑞客户不支持金睿卡
    		m_CardTypeTmp = 0;

    		//森亦杨、云瑞客户不支持云仓库
    		//m_CardTypeTmp_Clould = 0;
    	}
#if defined(__SUPPORT_COMMON_DID_OPREATE__)	
        else if (CUSTOMTYPE_XINRUISHI_NORMAL == m_FlowCustomType)
        {
            //普通运营号段不支持北纬
            m_CardTypeTmp_BW = 0;
        }
#endif

    	Module4G_SetWorkMode(m_nSimCardBindState);

    	//设置网线当前连接状态至4G模块
    	m_CheckEth0Count = 0;
    	m_PrevEth0LinkStatus = GetNetlinkStatus(IFNAME);
    	Module4G_SetWireConnStatus(m_PrevEth0LinkStatus);

    	//设置产测模式
    	Module4G_SetProductMode(m_nProductState);
		
		Module4G_SetCloudCardOnly(bSupportOnlyCloudCard);

    	//设置p2p状态
    	Module4G_SetTutkStatus(m_iTutkStatus);

    	Module4G_SetAllowUseEth0Flag(m_nAllowUseEth0);

    	if (BINDING_MODE == m_nSimCardBindState)
    	{
    		m_nCurrNetCardType = 2;
    	}
#if 0
    	//内置卡绑定状态下限制使用有线
    	if (1 == m_nProductState || UNBINDING_MODE == m_nSimCardBindState || 1 == m_nAllowUseEth0)
    	{
    		mysystem("ifconfig eth0 up");
    	}
    	else
    	{
    		//删除有线路由，关闭有线网卡
    		mysystem("/usr/sbin/operate_router delRoute eth0");
    		mysystem("ifconfig eth0 down");
    	}
#else
		mysystem("ifconfig eth0 up");

#endif
    	//开机产测一次
    	Module4G_SetProductTest(1);
    	Module4G_SetAllowFlag(1);
    }
    
    ZWSys_Get4GSupportV2(&bSdkSupport4G);
    if (bSdkSupport4G)
    {
    	//播报使用有线网络语音
    	if (m_nProductState || UNBINDING_MODE == m_nSimCardBindState || 1 == m_nAllowUseEth0)
    	{			
    		int nBreak = 0;
    		int nCounter = 0;
    		checkETH0Break(&nBreak);
    		if (0 == nBreak)
    		{
                char szSCode[64] = {0};
                int nSCodeLen = sizeof(szSCode);
                int rc = MPU_SYS_DeviceBind_GetSCode(szSCode, &nSCodeLen);
                
                int nSupportBoardCast = 0;
                int ret = ZWSys_GetBroadCastEnable(&nSupportBoardCast);

                if (((0 != rc) || (0 == strlen(szSCode))) || ((ret == 0) && nSupportBoardCast))
                {
    				ZWSys_SetSpeechContent(SPEECH_4G_USE_WIRED);
    				//循环查询，等待播放结束
    				nCounter = 0;
    				while (1)
    				{
    					sleep(1);
    					if (0 == ZWSys_GetSpeechPlayingStatus())
    					{
    						break;
    					}
    					if (++nCounter > SPEECH_PLAY_TIME_OUT_SEC)
    					{
    						nCounter = 0;
    						break;
    					}
    				}     
                }

    	   }
    	}
    }
	m_RecordCardTypeTmp = m_CardTypeTmp;
	m_RecordCardTypeTmp_BW = m_CardTypeTmp_BW;
	m_RecordCardTypeTmp_Clould = m_CardTypeTmp_Clould;
	m_RecordBindState = m_IsBindState;
}

void* CheckCardTypeTimerThread(void *lparam)
{
    PRT_TID();
    pthread_detach(pthread_self());

    NET_DBG(">>>>>>>>>> enter, pid = %d\n", getpid());
    BEGINRUN_THREAD;

    //获取当前实例
    INet *pNet = (INet *)lparam;

    if (NULL != pNet)
    {
       pNet->CheckCardTypeTimerThreadProcess();
    }
    else
    {
        NET_ERR("get pNet instance fail\n");
        NET_CHK(NULL != pNet);
    }

    NET_DBG(">>>>>>>>>> exit, pid = %d\n", getpid());
    ENDRUN_THREAD;

    return NULL;
}

/* 网络路由表处理线程 */
void* INet::Operate4GModuleRouterThreadProcess()
{
    char command[64] = {0};
    int connStatus = 0;
    int lastNetStatus = -1;//默认无效值，意图是首次条件一定能满足
    int result = 0;
    int lan_break_count = 0;
    int n4GLinkStatue = 0;
    int last4GLinkStatue = -1;//默认无效值，意图是首次条件一定能满足
    int n4G_connect_count = 0;
	//主DNS地址
	char tmpPrimaryDNS[20]={0};
	//第二DNS地址
	char tmpSecondaryDNS[20]={0};
	char szGateway[32]={0};

    SelectSleepSec(10);
    NET_INFO("----Start to operate router----\n");

    /*
     *  需要更新路由的条件(任意一条达到都要更新路由):
     *  1. 网线状态改变(插拔网线)；
     *  2. IP有更新(包括有线无线)；
     *  3. DHCP状态发生改变(包括有线无线)；
     *  4. 重启网卡；
     */
    while(1)
    {        
        SelectSleepSec(1);

        /* connStatus:1连上; connStatus:0断开 */ 
		connStatus = g_ethLinkStatus;

        if (BINDING_MODE == m_nSimCardBindState && 0 == m_nProductState && 0 == m_nAllowUseEth0)
		{
			//运营模式，不用管有线路由
			continue;
		}

        if (0 == connStatus)//无线
		{
			m_nCurrNetCardType = 2;  /* 无线 */
		}
		else
		{
			m_nCurrNetCardType = 1;    /* 有线 */
		}

        /* 如果物理网线状态发生改变(断开或者连接)，则需要进行操作(路由和切换网卡) */
        if (((lastNetStatus!=connStatus)&&(connStatus!=-1)) || (TRUE == bNeedUpdateRouter))
        {
            /* 针对非手动切换路由的情况，如果当前状态切换了，且网络是断开的情况 */
            if ((FALSE==bNeedUpdateRouter) && (0==connStatus))
            {
                /* 累计5次如果一直网线是断开的，则需要切换路由 */
                if (++lan_break_count >= 5)
                {
                    lan_break_count = 0;
                    NET_INFO("Detect lan has broken last 5 seconds!\n");
                }
                else
                {
                    /* 注意此处不能对lastNetStatus和lan_break_count进行赋值 */
                    continue;
                }
            }

            /* 其他情况下，需要置为0 */
            lan_break_count = 0;

            NET_INFO("===Condition has changed! Need to operate router===\n");

            pthread_mutex_lock(&m_OperateRouteLock);

            bOperatingRouter = TRUE;

			mysystem("/usr/sbin/operate_router delRoute eth0");
      
            mysystem("route -n");
            
            if (connStatus == 1)    
            {                                    /* 有线连上--->添加有线路由 */ 
                NET_INFO("[%d -> %d] need delete wireless route and add net route!\n", lastNetStatus, connStatus);

				mysystem("/usr/sbin/operate_router addRoute eth0");
            }

            bOperatingRouter = FALSE;

            memset(command, 0, sizeof(command)); /* 创建一个标记文件，里面存放当前有效的网卡类型 */
            snprintf(command, sizeof(command), "echo %d > /tmp/.effect_net_card", m_nCurrNetCardType);
            mysystem(command);
            NET_INFO("run: %s\n", command);
            lastNetStatus = connStatus;
            if (TRUE == bNeedUpdateRouter)
            {
                bNeedUpdateRouter = FALSE;
            }

            /* 设置路由之后，需要清除一下ARP缓存，以免不能上外网 */
            mysystem(ClearARPCache);
            NET_INFO("run: %s\n", ClearARPCache);

            mysystem("route -n");

            NET_INFO("===Operate router Over===\n");
  
            pthread_mutex_unlock(&m_OperateRouteLock);

            /* 网络状态发生改变需要重启网络探测服务(用以防止私有服务探测不到) */
            NET_INFO("--- Reboot device probe server (HieServer_DeviceProbeStop) start ---\n");
            HieServer_DeviceProbeStop();
            HieServer_DeviceProbeStart();
            NET_INFO("--- Reboot device probe server (HieServer_DeviceProbeStart) stop ---\n");

            // 获取配置文件对应字段的dns
			if (connStatus != 0)  
			{
				m_pCConfig->GetValue(IPPPOE_CfgSection, IPPPOE_PrimaryDNS_key, DEFAULT_RESOLV_CONF, tmpPrimaryDNS, 20);
				m_pCConfig->GetValue(IPPPOE_CfgSection, IPPPOE_SecondaryDNS_key, DEFAULT_RESOLV_CONF_LOCAL, tmpSecondaryDNS, 20);
				memset(szGateway, 0, sizeof(szGateway));
				NET_GetGateway(szGateway);
				NET_INFO("Operate4GModuleRouterThreadProcess [dnstest] [write] %s cfgfile maindns: %s  seconddns: %s  gateway :%s \n", (0==connStatus) ? "wlan0":"eth0", tmpPrimaryDNS, tmpSecondaryDNS, szGateway);
				ZWHost_DNS_SERVER_INFO_LIST DnsList[1] = {0};
				NET_UseZwhostCalcDomain(tmpPrimaryDNS,tmpSecondaryDNS, szGateway,DnsList);
				WriteDNSFileByZwhost(DnsList);

			}

			res_init();

            result = Hosts_Update();
            if (result)
            {
                NET_ERR("Fail to update the hosts, errno = %d\n", result);
            }
            else
            {
                NET_INFO("--- Update the hosts over ---\n");
            }

            /*DNS改变后调用res_init 防止域名解析失败*/
	        res_init();
        }
        
        //有线连接断开，且4g连接正常需要更新下DNS信息。
        if (0 == connStatus)
        {
            Module4G_GetModuleInternetConnStatus(&n4GLinkStatue);
            if (last4GLinkStatue != n4GLinkStatue)
            {
                if (n4GLinkStatue)
                {
                    n4G_connect_count++;
                }
                else
                {
                    n4G_connect_count = 0;
                    last4GLinkStatue = n4GLinkStatue;
                }

                if (n4G_connect_count >= 3)
                {
                    ParseDnsAddr();
                    last4GLinkStatue = n4GLinkStatue;
                    NET_INFO("---Operate4GModuleRouterThreadProcess 4GUpdate DnsAddr!---\n");
                }
            }
        }
    }

    return NULL;
}

/* 获取4G工作模式 */
int INet::Module4G_GetCardBindMode()
{
	return m_nSimCardBindState;
}

/* 获取产测状态 */
int INet::Module4G_GetProductState()
{
	return m_nProductState;
}

/*加载P2P协议*/
void INet::Module4G_LoadTutkProtocol(int nEth0Status,int nTime)
{
#define MAX_WAIT_LOADTUTK_TIME			(2*60)		//超过这个时间也加载p2p协议

	static int WaitTime = 0;
	static int Loadflag = 0;
	static int LoadSuccess = 0;
	int module_link_status = 0;		//4G网络连接状态
	int ret = 0;

	if (1 == Loadflag)
	{
		if (0 != LoadSuccess)
		{
			NET_INFO("Load Tutk Failed ,try again!\n");
			LoadSuccess = HieServer_LoadProtocol(eProtocolTUTK);
		}
		
		return;
	}

	WaitTime += nTime;
	if (WaitTime >= MAX_WAIT_LOADTUTK_TIME)
	{
		LoadSuccess = HieServer_LoadProtocol(eProtocolTUTK);
		Loadflag = 1;
		NET_INFO("TimeOut Load Tutk\n");
	}
	else
	{
		if (1 == nEth0Status)
		{
			LoadSuccess = HieServer_LoadProtocol(eProtocolTUTK);
			Loadflag = 1;
			NET_INFO("Use Eth0 Load Tutk\n");
		}
		else
		{
			//获取模组当前联网状态
			ret = Module4G_GetModuleInternetConnStatus(&module_link_status);
			if (ret)
			{
				NET_ERR("get InternetConnStatus fail\n");
				return;
			}

			if (1 == module_link_status)
			{
				LoadSuccess = HieServer_LoadProtocol(eProtocolTUTK);
				Loadflag = 1;
				NET_INFO("Use 4G Load Tutk\n");
			}
		}
	}
}

/* 4G状态检测线程 */
void* INet::Module4G_CheckStatusThreadProcess()
{
	int nRreshNetStatue = 0;
	int nRresh4GNetStatue = 0;
	char szNet_Ifname[16] = {0};
	int nEth0LinkStatus = 0;
    int nCounter = 0;
	int n4GLinkStatue = 0;
	int need_stop_QRscan = 0;
    int b4GConnectSuccess = 0;
    bool bQrScanFlag = FALSE;
    static int nWaitNtpTime = 0;
    static int nPingFailedCount = 0;

    sleep(5);

    while(1)
    {
        sleep(2);

		nEth0LinkStatus = GetNetlinkStatus(IFNAME);

		if (nEth0LinkStatus != m_PrevEth0LinkStatus)
		{
			m_CheckEth0Count++;
		}
		else
		{
			m_CheckEth0Count = 0;
		}

		//连续5次检测到变化状态，才切换
		if (m_CheckEth0Count >= 5)
		{
			m_CheckEth0Count = 0;
			m_PrevEth0LinkStatus = nEth0LinkStatus;
			Module4G_SetWireConnStatus(nEth0LinkStatus);
		}

		Module4G_LoadTutkProtocol(m_PrevEth0LinkStatus,2);

        //判断是否允许使用4G模组联网
		Modlue4G_AllowHandler(0);
		//判断是否允许使用有线联网
		//Modlue4G_Eth0AllowHandler(2);

		if (UNBINDING_MODE == m_nSimCardBindState || 1 == m_nProductState || 1 == m_nAllowUseEth0)
		{
			//标准模式或产测模式，根据有线连接状态切换类型
			if (0 == m_PrevEth0LinkStatus)//4G
			{
				m_nCurrNetCardType = 2;
			}
			else
			{
				m_nCurrNetCardType = 1;
			}
		}

		if ((0 == nRresh4GNetStatue) && (0 == nEth0LinkStatus))
		{
			//通知协议网络连接情况
			Module4G_GetModuleInternetConnStatus(&n4GLinkStatue);
			if (n4GLinkStatue)
			{
			    //ntp对时成功或者等了6s
				nWaitNtpTime += 2;
				if ((MPU_SYS_GetNtpSuccessFlag()) ||
					(nWaitNtpTime >= 6))
				{
    				HieServer_RefreshEventEx(USEREVENT_DEVICE_NET_CONNECT_STATUS, EVENT_NET_CONNECTED, 
    						0, 0, 0, NULL, 0);

    				nRresh4GNetStatue = 1;
    				nRreshNetStatue = 1;	

    				//更新域名解析缓存
                	Hosts_Update();
                }
                b4GConnectSuccess = 1;
			}
		}
		
		if ((0 == nRreshNetStatue) && (1 == nEth0LinkStatus) && (UNBINDING_MODE == m_nSimCardBindState || m_nProductState || 1 == m_nAllowUseEth0))
		{
			strncpy(szNet_Ifname,IFNAME,sizeof(szNet_Ifname));

			if (1 == myping(szNet_Ifname, GOOGLE_DOMAIN))
			{
				HieServer_RefreshEventEx(USEREVENT_DEVICE_NET_CONNECT_STATUS, EVENT_NET_CONNECTED, 
						0, 0, 0, NULL, 0);
                char szSCode[64] = {0};
                int nSCodeLen = sizeof(szSCode);
                int rc = MPU_SYS_DeviceBind_GetSCode(szSCode, &nSCodeLen);
#ifndef __MODULE_4G_SUPPORT__   
                if((0 != rc) || (0 == strlen(szSCode)))
#else
                int nSupportBoardCast = 0;
                int ret = ZWSys_GetBroadCastEnable(&nSupportBoardCast);
                if (((0 != rc) || (0 == strlen(szSCode))) || ((ret == 0) && nSupportBoardCast))
#endif  
                {
                    ZWSys_SetSpeechContent(SPEECH_4G_CONNECT_SUCCESS);
                    
                    //循环查询，等待播放结束
                    nCounter = 0;
                    while (1)
                    {
                        sleep(1);
                        if (eSpeechPlayIdle == ZWSys_GetSpeechPlayingStatus())
                        {
                            break;
                        }
                        if (++nCounter > SPEECH_PLAY_TIME_OUT_SEC)
                        {
                            nCounter = 0;
                            break;
                        }
                    }
                }
                nRreshNetStatue = 1;
            }   
            else
            {
                //连续5次播报失败
                nPingFailedCount++;
                if (nPingFailedCount >= 5)
                {
                    char szSCode[64] = {0};
                    int nSCodeLen = sizeof(szSCode);
                    int rc = MPU_SYS_DeviceBind_GetSCode(szSCode, &nSCodeLen);
#ifndef __MODULE_4G_SUPPORT__
                    if ((0 != rc) || (0 == strlen(szSCode)))
#else
                    int nSupportBoardCast = 0;
                    int ret = ZWSys_GetBroadCastEnable(&nSupportBoardCast);
                    if (((0 != rc) || (0 == strlen(szSCode))) || ((ret == 0) && nSupportBoardCast))
#endif
                    {
                        ZWSys_SetSpeechContent(SPEECH_4G_CONNECT_FAILED);
                        //循环查询，等待播放结束
                        nCounter = 0;
                        while (1)
                        {
                            sleep(1);
                            if (eSpeechPlayIdle == ZWSys_GetSpeechPlayingStatus())
                            {
                                break;
                            }
                            if (++nCounter > SPEECH_PLAY_TIME_OUT_SEC)
                            {
                                nCounter = 0;
                                break;
                            }
                        }
                    }
    				nRreshNetStatue = 1;
    			 }				
    		}		
        }

        do
        {
            if (0 == bNeed4GScanStatue || 1 == m_nProductState)
            {
                if (1 == GetQRScanStatus())
                {
                    //关闭二维码扫描
                    SetQRScanStatus(0);
                    NET_INFO("SetQRScanStatus 0!\n");
                }
                break;
            }

            //下面开始是4G扫码添加设备逻辑
            if (1 == b4GConnectSuccess)
            {
                need_stop_QRscan = GetQRScanCloseFlags();
                if (0 != need_stop_QRscan)	
                {
                    SetQRScanStatus(0);
                    NET_INFO("SetQRScanStatus 0!\n");
                }

                char szSCode[64] = {0};
                int nSCodeLen = sizeof(szSCode);
                int rc = MPU_SYS_DeviceBind_GetSCode(szSCode, &nSCodeLen);
                if ((0 != rc || 0 == strlen(szSCode)) &&  0 != access("/data/config/config/noSupportShare",F_OK))
                {
                    //设备未被绑定，且无有线连接开启扫码
                    if (0 == nEth0LinkStatus)
                    {
                        //开启二维码扫描,只能打开一次，防止8分钟时间到后关闭配网后又打开此功能。
                        if (0 == GetQRScanStatus() && FALSE == bQrScanFlag)
                        {
                            SetQRScanStatus(1);
                            bQrScanFlag = TRUE;
                            NET_INFO("SetQRScanStatus 1!\n");
                        }
                    }
                }
                else 
                {
                     NET_INFO("The device is bound, return now!\n");
                     bNeed4GScanStatue = 0;
                     break;
                }
            }
        } while (0);
    }

    return NULL;
}

static void* GPRS_CheckStatusThread(void *lParam)
{
    PRT_TID();

    NET_DBG(">>>>>>>>>> enter, pid = %d\n", getpid());
    BEGINRUN_THREAD;

    //获取当前实例
    INet *pNet = (INet *)lParam;

    if (NULL != pNet)
    {
    	pNet->Module4G_CheckStatusThreadProcess();
    }
    else
    {
        NET_ERR("get pNet instance fail\n");
        NET_CHK(NULL != pNet);
    }

    NET_DBG(">>>>>>>>>> exit, pid = %d\n", getpid());
    ENDRUN_THREAD;

    return NULL;
}

#define FAILED_RETRY_PERIOD		 2		//前10次访问失败，重新访问间隔
#define COM_FAILED_RETRY_PERIOD 120		//10次访问失败后，重新访问间隔
#define FAILED_RETRY_COUNT_JR	10		//快速访问次数，超过后慢速访问
#define LAST_FAILED_RETRY_PERIOD 60		//60次访问失败后，重新访问间隔
#define FAILED_RETRY_COUNT		 60		//快速访问次数，超过后慢速访问

/* 获取金睿4G解绑状态 */
int INet::Module4G_GetCardBindState(int reset)
{

	static int PeriodValue = 0;		 //参数调用周期
	static int PeriodCount = 0;		 //计时器
	static int ComFailedFlag = 0;	 //通讯失败标识
	static int ComFailedCount = 0;	 //通讯失败计时器
	
	HY_DVR_DF_DEVICE_CFG deviceCfg;
	int module_link_status = 0;		//4G网络连接状态
	int ret = 0;
	TutkCfg TutkDid;
	NET_4G_GetTutkUID(&TutkDid);

	if(1 == reset)
	{
		PeriodValue = 0;		 
		PeriodCount = 0;		 
		ComFailedFlag = 0;	 
		ComFailedCount = 0;	 

		return 0;
	}

	if (0 == strlen(&TutkDid.szTutkUID[0]))
	{
		//无DID
		return 0;
	}
	//获取模组当前联网状态
	ret = Module4G_GetModuleInternetConnStatus(&module_link_status);
	if (ret)
	{
		NET_ERR("get InternetConnStatus fail ret = %d\n",ret);
		return 0;
	}
	if (0 == module_link_status && 0 != m_iNetStatus)
	{
		//未联网
		return 0;
	}


	if (-1 == m_IsBindState)
	{
		//设备未解绑或者未获取解绑状态时
		PeriodCount += 2;
		if (PeriodCount < PeriodValue)
        {
            return 0;
        }  

		PeriodCount = 0;
		ComFailedFlag = 0;
		
		memset(&(deviceCfg.struDeviceFlow), 0, sizeof(deviceCfg.struDeviceFlow));
	    memset(&deviceCfg, 0, sizeof(deviceCfg));
	    ret = DFClient_GetDeviceCFG(&TutkDid.szTutkUID[0], &deviceCfg);

	    if (eErrorNoIdleResource == ret)
	    {
	    	//未解绑
			m_IsBindState = 1;
	    }
	    else if (0 == ret)
	    {
			if (deviceCfg.nValid)
			{
				if (2 == deviceCfg.struDeviceFlow.nJR4G)
				{
					//解绑
					m_IsBindState = 0;
					//加白
					m_IsAddwhite = 1;
				}
				else
				{
					//未解绑
					m_IsBindState = 1;
					//未加白
					m_IsAddwhite = 0;
				}
			}
			else
			{
				ComFailedFlag = 1;
				NET_ERR("get DFClient_GetDeviceCFG fail nValid = %d\n",deviceCfg.nValid);
			}
	    }
	    else
	    {
	    	ComFailedFlag = 1;
			NET_ERR("get DFClient_GetDeviceCFG fail ret = %d\n",ret);
	    }	
		
		if (ComFailedFlag)
		{
			ComFailedCount++;
			if (ComFailedCount >= FAILED_RETRY_COUNT_JR)
			{
				PeriodValue = COM_FAILED_RETRY_PERIOD; 
			}
		}
	}

	return 0;
}

/* 检测是否是北纬卡 */
int INet::Module4G_CheckBWCardType(int reset)
{
	static int PeriodCount = FAILED_RETRY_PERIOD;		 //计时器
	static int ComFailedCount = 0;	 //通讯失败计时器
	static int PeriodCountMax = FAILED_RETRY_PERIOD;
	char sIccid[32] = {0};
	int nRet = 0;
	int nCardType = 0;
	HY_DVR_DF_BWCARD_INFO Bw_Info;

	int module_link_status = 0;		//4G网络连接状态
	int ret = 0;

	if(1 == reset)
	{
		PeriodCount = FAILED_RETRY_PERIOD;		 
		ComFailedCount = 0;	 
		PeriodCountMax = FAILED_RETRY_PERIOD;
		return 0;
	}

	//获取模组当前联网状态
	ret = Module4G_GetModuleInternetConnStatus(&module_link_status);
	if (ret)
	{
		NET_ERR("get InternetConnStatus fail\n");
		return 0;
	}

	if (0 == module_link_status)
	{
		//未联网
		return 0;
	}
	
	TutkCfg TutkDid;
	NET_4G_GetTutkUID(&TutkDid);

	if (0 == strlen(&TutkDid.szTutkUID[0]))
	{
		//无DID
		return 0;
	}

	memcpy(&Bw_Info.strDID,&TutkDid.szTutkUID[0],sizeof(TutkDid.szTutkUID));

	Module4G_GetIccid(sIccid);

	if (0 == strlen(sIccid))
	{
		return 0;
	}

	PeriodCount += 2;
	if (PeriodCount >= PeriodCountMax)
	{
		PeriodCount = 0;
	
		nRet = DFClient_GetBWCardTypeV2(sIccid,0,&nCardType,&Bw_Info);
		if (0 == nRet)
		{
			if (1 == nCardType)
			{
				//北纬卡
				m_IsJinRuiSimCard = 2;
				NET_INFO("SimCard is Bw card\n"); 
			}
			else
			{
				m_CardTypeTmp_BW = 0;
				NET_INFO("SimCard not Bw card \n"); 
			}
		}
		else
		{
			NET_INFO("SimCard get error\n"); 

			ComFailedCount++;
			if (ComFailedCount >= FAILED_RETRY_COUNT)
			{
				PeriodCountMax = LAST_FAILED_RETRY_PERIOD; 
			}
		}
	}

	return 0;
}

/* 检测是否是云仓库卡 */
int INet::Module4G_CheckClouldWareHouseCardType(int reset)
{
	static int PeriodCount = FAILED_RETRY_PERIOD;		 //计时器
	static int ComFailedCount = 0;	 //通讯失败计时器
	static int PeriodCountMax = FAILED_RETRY_PERIOD;
	char sIccid[32] = {0};
	int nRet = 0;
    int nCardType = 0;
	int nCardTypeV2 = 0;
	int GetDealer_num = 5;

	int module_link_status = 0;		//4G网络连接状态
	int ret = 0;

	if(1 == reset)
	{
		PeriodCount = FAILED_RETRY_PERIOD;		 //计时器
		ComFailedCount = 0;	 //通讯失败计时器
		PeriodCountMax = FAILED_RETRY_PERIOD;
		return 0;
	}

	//获取模组当前联网状态
	ret = Module4G_GetModuleInternetConnStatus(&module_link_status);
	if (ret)
	{
		NET_ERR("get InternetConnStatus fail\n");
		return 0;
	}

	if (0 == module_link_status)
	{
		//未联网
		return 0;
	}

	Module4G_GetIccid(sIccid);

	if (0 == strlen(sIccid))
	{
		return 0;
	}

	TutkCfg TutkDid;
	NET_4G_GetTutkUID(&TutkDid);

	if (0 == strlen(&TutkDid.szTutkUID[0]))
	{
		NET_INFO("Module4G_CheckClouldWareHouseCardType return\n");
		//无DID
		return 0;
	}

	PeriodCount += 2;
	if (PeriodCount >= PeriodCountMax)
	{
		PeriodCount = 0;
		while(1)
		{
			nRet = DFClient_GetDealerInfo(sIccid,&TutkDid.szTutkUID[0],&nCardTypeV2);
			NET_INFO("DFClient_GetDealerInfo ret  = %d\n",nRet);
			if (0 == nRet)
			{
				NET_INFO("nCardTypeV2 nCardTypeV2 = %d\n",nCardTypeV2); 
			
				if (eDF_errorSuccess == nCardTypeV2)
				{
					//云仓库卡
					nCardType = 1;
					break;
				}
				else
				{
					nCardType = 0;
				}
			}

			if(GetDealer_num < 0)
			{
				break;
			}

			GetDealer_num--;

			sleep(2);
		}

		if (0 == nRet)
		{
			if (1 == nCardType)
			{
				//云仓库卡
				m_IsJinRuiSimCard = 3;
				NET_INFO("SimCard is Clould card\n"); 
			}
			else
			{
				m_CardTypeTmp_Clould = 0;
				NET_INFO("SimCard not Clould card \n"); 
			}
		}
		else
		{
			NET_INFO("SimCard get error\n"); 

			ComFailedCount++;
			if (ComFailedCount >= FAILED_RETRY_COUNT)
			{
				PeriodCountMax = LAST_FAILED_RETRY_PERIOD; 
			}
		}
	}

	return 0;
}

/* 检测卡类型计时器线程 */
void* INet::CheckCardTypeTimerThreadProcess(void)
{
    while(1)
    {
		NET_DBG("m_judgehalfhour = %d ,m_nCheckCardTypeCounter = %d, m_judgehalfcount = %d\n",m_judgehalfhour,m_nCheckCardTypeCounter,m_judgehalfcount);
		if(0 == m_judgehalfhour)
		{
			m_nCheckCardTypeCounter ++;

			if(1800 == m_nCheckCardTypeCounter)
			{
				m_judgehalfcount += 1;
				m_nCheckCardTypeCounter = 0;
				m_CardTypeTmp_Clould = m_RecordCardTypeTmp_Clould;
				m_CardTypeTmp_BW  = m_RecordCardTypeTmp_BW;
				m_CardTypeTmp = m_RecordCardTypeTmp;
				m_IsBindState = m_RecordBindState;
				m_judgehalfhour = -1;

				Module4G_SetAllowFlag(1);
				Module4G_GetCardBindState(1);
				Module4G_CheckClouldWareHouseCardType(1);
				Module4G_CheckBWCardType(1);
				
				Modlue4G_AllowHandler(1);
			}
		}
		if(12 == m_judgehalfcount)
		{
			NET_ERR("check 12 times card failed ,reboot now!\n");
			RebootLinux(REBOOT_CHECKCARD_TIMEOUT);
		}
        SelectSleepSec(1);
    }

    return NULL;
}

#define TESTCARDFILE "/mnt/mtd/config/testcardtype"

// 4G连接网络时长--秒
#define GPRSCONNETTIME  (60)
// 语音播报次数 3
#define SPEECHTTIMES  (3)

/*@breif SetSpeechContent 播报语音
 *@param [IN] nSpeechType 语音类型
 *@       
 *@       
 */
static void SetSpeechContent(int nSpeechType)
{
	static unsigned char speedflag[8] = {0};
	
	int nCounter = 0;
	char szSCode[64] = {0};
	int nSCodeLen = sizeof(szSCode);
	int rc = -1;

	NET_ERR("nSpeechType :%d \n", nSpeechType);
	if (nSpeechType < SPEECH_4G_CARDTYPE_ERRORCODE1 || nSpeechType > SPEECH_4G_CARDTYPE_ERRORCODE6)
	{
		return;
	}

	NET_ERR("speedflag[%d ] :%d SPEECHTTIMES :%d \n", nSpeechType-SPEECH_4G_CARDTYPE_ERRORCODE1, speedflag[nSpeechType-SPEECH_4G_CARDTYPE_ERRORCODE1], SPEECHTTIMES);
	//控制语音播报只播报一次,后面触发就不播放了
	if (SPEECHTTIMES <= speedflag[nSpeechType-SPEECH_4G_CARDTYPE_ERRORCODE1])
	{
		return;
	}

	speedflag[nSpeechType-SPEECH_4G_CARDTYPE_ERRORCODE1] ++;

#if defined (__IS_SUPPORT_APP_SHARE__)	
	if (IsSupportDevShareFunc())
	{
		rc = MPU_SYS_DeviceBind_GetSCode(szSCode, &nSCodeLen);
	}
#endif

#if defined (__IS_SUPPORT_APP_SHARE__)	    
	if (((0 != rc) || (0 == strlen(szSCode)))
		&& (IsSupportDevShareFunc()))
	{
#endif	
		NET_ERR("errcode nSpeechType :%d \n", ((SpeechType_T)nSpeechType));
		ZWSys_SetSpeechContent((SpeechType_T)nSpeechType);
	    //循环查询，等待播放结束
	    nCounter = 0;
	    while (1)
	    {
	        sleep(1);
	        if (0 == ZWSys_GetSpeechPlayingStatus())
	        {
	            break;
	        }
	        
	        if (++nCounter > SPEECH_PLAY_TIME_OUT_SEC)
	        {
	            nCounter = 0;
	            break;
	        }
	    }
#if defined (__IS_SUPPORT_APP_SHARE__)	        
    }
#endif    
}

/*

联网时间达到1min, 如没有获取到卡类型，播报对应的错误码
1.所有平台均失败，错误码1；
2.不满足上述条件，若有云仓库平台失败，错误码2；
3.不满足上述条件，若有北纬平台失败，错误码3；
4.不满足上述条件，若有金睿平台失败，错误码4；
5.不满足上述条件，若有加白平台失败，错误码5；
6.平台查询成功，但是p2p连接失败，播报错误码6;
*/
static int GetCardtypeErrCode(const int conntime, const int IsJinRuiSimCard, const int CardTypeTmp, const int CardTypeTmp_Clould, const int CardTypeTmp_BW, const int m_BindState, const int TutkStatus)
{
	//获取模组当前联网状态
	int ret = 0;
	int module_link_status = 0;
	static int timecount = 0;
	
	if (conntime <= 0)
	{
		return 0;
	}

	if (timecount > conntime + SPEECHTTIMES)
	{
		return 0;
	}
	
	ret = Module4G_GetModuleInternetConnStatus(&module_link_status);
	if (ret)
	{
		NET_ERR("get InternetConnStatus fail \n");
		timecount = 0;
		return 0;
	}

	if (0 == module_link_status)
	{
		//未联网
		timecount = 0;
		NET_ERR("get InternetConnStatus fail \n");
		return 0;
	}

	timecount ++;
//	NET_ERR("timecount :%d conntime :%d  IsJinRuiSimCard :%d  CardTypeTmp :%d CardTypeTmp_Clould :%d CardTypeTmp_BW :%d m_BindState :%d \n", timecount, conntime, IsJinRuiSimCard, CardTypeTmp, CardTypeTmp_Clould, CardTypeTmp_BW, m_BindState);
	
	if ((timecount >= conntime) && (timecount < conntime + SPEECHTTIMES) )
	{
		// 所有平台都未查询到卡类型----错误码 1
		if (-1 == IsJinRuiSimCard && -1 == CardTypeTmp_Clould && -1 == CardTypeTmp_BW)
		{
			return 1;
		}
		// 云仓库平台失败----错误码 2
		else if(-1 == IsJinRuiSimCard && -1 == CardTypeTmp_Clould)
		{
			return 2;
		}
		//北纬平台失败----错误码 3
		else if(-1 == IsJinRuiSimCard && -1 == CardTypeTmp_BW)
		{
			return 3;
		}
		//金睿平台失败----错误码 4
		else if(-1 == IsJinRuiSimCard && -1 == CardTypeTmp)
		{
			return 4;
		}
		//加白平台失败----错误码 5
		else if(0 == IsJinRuiSimCard && -1 == m_BindState)
		{
			return 5;
		}
		//p2p失败----错误码 6
		else if(2 != TutkStatus)
		{
			return 6;
		}
			
	}

	return 0;
}

static void BroadCastCardTypeErrCode(const int conntime, const int IsJinRuiSimCard, const int CardTypeTmp, const int CardTypeTmp_Clould, const int CardTypeTmp_BW, const int IsBindState, const int TutkStatus)
{
	int CardtypeErrCode = 0;

	CardtypeErrCode = GetCardtypeErrCode(conntime, IsJinRuiSimCard, CardTypeTmp, CardTypeTmp_Clould, CardTypeTmp_BW, IsBindState, TutkStatus);
	//NET_ERR("CardtypeErrCode :%d \n", CardtypeErrCode);
	if (0 < CardtypeErrCode)
	{
		SetSpeechContent(SPEECH_4G_CARDTYPE_ERROR+CardtypeErrCode);
		// 写日志
#ifdef __SUPPORT_SAVE_TF_LOG__ 
		TF_GPRS_LOG_INFO("Platform errcode :%d \n", CardtypeErrCode);
#endif
	}
}

/* 根据是否是金睿卡、解绑状态、判定是否允许使用4G联网 */
void INet::Modlue4G_AllowHandler(int reset)
{
	static int first = 1;

	if(1 == reset)
	{
		first = 1;
		return;
	}

	if (UNBINDING_MODE == m_nSimCardBindState)
	{
		//标准模式。不用处理
		return;
	}

	// 本函数每隔两秒运行一次，所以除以2
	if (2 == m_nCurrNetCardType)
	{
		BroadCastCardTypeErrCode((GPRSCONNETTIME/2), m_IsJinRuiSimCard, m_CardTypeTmp, m_CardTypeTmp_Clould, m_CardTypeTmp_BW, m_IsBindState, m_iTutkStatus);
	}

	if (!first)
	{
		return;
	}

	//非绑定卡才需要获取解绑状态
	Module4G_GetCardBindState(0);

	if (-1 == m_IsJinRuiSimCard)
	{
		//未确定卡类型
		if (-1 == m_CardTypeTmp_Clould)
		{
			//判断是否是云仓库卡
			Module4G_CheckClouldWareHouseCardType(0);
		}
	}
	
	if ((-1 == m_IsJinRuiSimCard) && (0 == m_CardTypeTmp) && (0 == m_CardTypeTmp_Clould))
	{
		//非金睿卡、非北纬卡、非云仓库卡
		m_IsJinRuiSimCard = 0;
	}

	
	//还未获取到结果
	if (-1 == m_IsJinRuiSimCard)
	{
		return;
	}

	if (0 == m_IsJinRuiSimCard)
	{
		//非绑定卡
		if (-1 == m_IsBindState)
		{
			//未获取到解绑状态，不限制4G
			return;
		}

		if (m_IsBindState)
		{
			//未解绑，不允许使用4G联网
			Module4G_SetAllowFlag(0);
			Module4G_LinkSpeechControl(LINKSPEECH_NOTORIGINAL_BIND);
			m_judgehalfhour = 0;  //开始计时
		}
		else
		{
			char cmd_string[128];

			//创建解绑标识
			memset(cmd_string,0,sizeof(cmd_string));
			snprintf(cmd_string,sizeof(cmd_string)-1,"touch %s",MODULE_4G_BIND_FLAG);
			mysystem(cmd_string);

			//打开有线网络
			mysystem("ifconfig eth0 up");

			//设置当前模式为标准模式
			m_nSimCardBindState = UNBINDING_MODE;
			Module4G_SetWorkMode(m_nSimCardBindState);

			Module4G_LinkSpeechControl(LINKSPEECH_NOTORIGINAL_UNBIND);
			m_judgehalfhour = 1;
		}

		first = 0;
	}
	else
	{
		first = 0;
		Module4G_LinkSpeechControl(LINKSPEECH_ORIGINAL);
		m_judgehalfhour = 1;
	}
}

/* 根据是否是产测模式、运营模式、判定是否允许使用有线联网 */
void INet::Modlue4G_Eth0AllowHandler(int nTime)
{
#define ETH0_MAX_ALLOW_USE_TIME			(30*60)		//有线允许使用时长

	static int nLinkTimes = 0;
	if ((BINDING_MODE == m_nSimCardBindState && 0 == m_IsAddwhite) && (1 == m_nProductState || 1 == m_nAllowUseEth0))
	{
		nLinkTimes += nTime;
		if (nLinkTimes >= ETH0_MAX_ALLOW_USE_TIME)
		{
			m_nAllowUseEth0 = 0;
			m_nProductState = 0;
			Module4G_SetProductMode(m_nProductState);
			Module4G_SetAllowUseEth0Flag(m_nAllowUseEth0);

			m_nCurrNetCardType = 2;

			//删除有线路由，关闭有线网卡
			mysystem("/usr/sbin/operate_router delRoute eth0");
			mysystem("ifconfig eth0 down");

			NET_INFO("Limit Use Eth0\n"); 
		}
	}
	else
	{
		nLinkTimes = 0;
	}
}

/*设置开始执行产测流程*/
int INet::Net_SetProductTest(int isTest)
{   
    if (bSupportGPRS)
    {
	    Module4G_SetProductTest(isTest);
    }
    
	return 0;
}

/*获取4G测试数据*/
int INet::Net_Get4GProductTestData(Module4G_ProductTest_Data_t *pData)
{
    if (bSupportGPRS)
    {
        Module4G_GetProductTestData(pData);
    }

	return 0;
}

/*设置4G模块配置*/
int INet::Net_Set4GControlConfig(HY_DVR_4G_CARD_SELECT_CFG_INFO *pcfg)
{
	int nRet = 0;
	nRet = Module4G_Set4GControlConfig(pcfg->nValid,pcfg->nCardID);

	return nRet;
}

/*获取4G模块配置*/
int INet::Net_Get4GControlConfig(HY_DVR_4G_CARD_SELECT_CFG_INFO *pcfg)
{
	int nRet = 0;
	nRet = Module4G_Get4GControlConfig(&pcfg->nValid,&pcfg->nCardID,pcfg->cICCID,&pcfg->nAutoSwitchCard);

	return nRet;
}

/*获取4G切卡结果*/
int INet::Net_Get4GSwitchResult(HY_DVR_4G_CARD_SELECT_RESULT_INFO *pcfg)
{
	int nRet = 0;
	nRet = Module4G_Get4GSwitchResult(&pcfg->nValid,&pcfg->iSelectErrCode);
	return nRet;

}

/*获取4G卡信息*/
int INet::Net_Get4GCardInfo(int nSim,int *nCardType,char *Iccid,int *nCardStatus,char *szOperator)
{
	int nRet = 0;
	HY_DVR_4G_NETCARD_INFO cardcfg = {0};
    char Version[128];
	if (NULL == nCardType || NULL == Iccid || NULL == nCardStatus || NULL == szOperator)
	{
		NET_ERR("Param invalid\n");
		return -1;
	}

    if (bSupportGPRS)
    {
    	nRet = Net_GetSimCardInfo(cardcfg.szIMSI, cardcfg.szIMEI,cardcfg.szACT, szOperator, cardcfg.szBand,Iccid,Version);
    	NET_ERR("nRet = %d\nszOperator = %s\n",nRet,szOperator);
    }

	return nRet;
}

/* 获取4G模组信息 */
int INet::Net_Get4GModuleInfo(char *Version,int *iSignal4G)
{
	int nRet = 0;

	if (NULL == Version || NULL == iSignal4G)
	{
		NET_ERR("Param invalid\n");
		return -1;
	}

    if (bSupportGPRS)
    {
    	if (Module4G_GetVersion(Version))
    	{
    		NET_ERR("Fail to called Module4G_GetVersion!\n");
            return -1;
    	}

    	if (Module4G_GetModule4GSignalQuality(iSignal4G))
    	{
    		NET_ERR("Fail to called Module4G_GetModule4GSignalQuality!\n");
            return -1;
    	}
    }

	return nRet;
}

/* 获取双sim卡信息 */
int INet::Net_GetSimIndexCardInfo(int nSim, int* nIsInUse, char *szOperator, int *nSignalStrength,
			char *szICCID, char *szIMEI, char *sz4GModuleVersion)
{
	int nRet = 0;

	if (NULL == nIsInUse || NULL == szOperator || NULL == nSignalStrength || NULL == szICCID || NULL == szIMEI || NULL == sz4GModuleVersion)
	{
		NET_ERR("Param invalid\n");
		return -1;
	}

	if (Module4G_GetVersion(sz4GModuleVersion))
	{
		NET_ERR("Fail to get 4GModuleVersion!\n");
        return -1;
	}

	if (Module4G_GetSimIndexInfo(nSim, nIsInUse, szOperator, nSignalStrength, szICCID, szIMEI))
	{
		NET_ERR("Fail to get SimIndexInfo!\n");
        return -1;
	}

	return nRet;
}
			



/* 获取当前是否是p2p产测模式 */
int INet::Net_IsP2PProductTestMode(void)
{
	return m_P2pProductMode;
}

/* 检测当前GPRS模块是否可以连接互联网 */
int INet::Net_CheckIfGPRSAccessToInernet(int *gprs_status)
{
    if (bSupportGPRS)
    {
    	if (Module4G_GetModuleInternetConnStatus(gprs_status))
    	{
    		NET_ERR("Fail to called Module4G_GetModuleInternetConnStatus!\n");
            return -1;
    	}
    }
    return 0;
}

/* 获取当前SIM卡的信息 */
int INet::Net_GetSimCardInfo(char *imsi, 
                                       char *imei, 
                                       char *act, 
                                       char *oper, 
                                       char *band,
                                       char *iccid,
                                       char *sVersion)
{

    if (!imsi || !imei || !act || !oper || !band || !iccid || !sVersion)
    {
        NET_ERR("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    if (bSupportGPRS)
    {
    	if (Module4G_GetSimInfo(imsi,imei,act,oper,band,iccid,sVersion))
    	{
    		NET_ERR("Fail to called Module4G_GetSimInfo!\n");
            return -1;
    	}
    }

    return 0;
}

/* 获取当前模块的信号强度 */
int INet::Net_GetGPRSModuleSignalQuality(int *signal_quality)
{
    if (!signal_quality)
    {
        NET_ERR("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    if (bSupportGPRS)
    {
        if (Module4G_GetModuleSignalQuality(signal_quality))
        {
            NET_ERR("Fail to called Module4G_GetModuleSignalQuality!\n");
            return -1;
        }
    }

    return 0;
}

/* 检查Sim卡的状态0:成功 1:SIM卡锁住 2:失败 */
int INet::NET_GPRS_GetSimCardStatus(int * sim_card_status)
{
    if (bSupportGPRS)
    {
        /* 初始化状态为失败 */
        *sim_card_status = 2;

        if (!sim_card_status)
        {
            NET_ERR("NULL POINTER EXCEPTION!\n");
            return -1;
        }

        if (Module4G_GetModuleSimStatus(sim_card_status))
        {
            NET_ERR("Fail to called GPRS_GetSimCardStatus!\n");
            return -1;
        }

        if (1 == *sim_card_status)
        {
    		*sim_card_status = 0;
        }
        else
        {
    		*sim_card_status = 2;
        }
    }

    return 0;
}

/* 检查Sim卡的状态 0:成功 1:失败 */
int INet::NET_GPRS_GetSimCardStatus1(int * sim_card_status)
{
    if (bSupportGPRS)
    {
        /* 初始化状态为失败 */
        *sim_card_status = 1;

        if (!sim_card_status)
        {
            NET_ERR("NULL POINTER EXCEPTION!\n");
            return -1;
        }

        if (Module4G_GetModuleSimStatus(sim_card_status))
        {
            NET_ERR("Fail to called GPRS_GetSimCardStatus!\n");
            return -1;
        }
    }

    return 0;
}

int INet::NET_GPRS_GetFlAppid(char  * gprs_appid)
{
    if (NULL == gprs_appid)
    {
        NET_ERR("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    strncpy(gprs_appid, m_szAppid, (size_t)sizeof(m_szAppid));

	return 0;
}

/*获取卡类型*/
int INet::Net_GPRS_FL_SimCardType(int *nType)
{
	if (NULL == nType)
    {
        NET_ERR("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    if (bSupportGPRS)
    {
    	if (BINDING_MODE == m_nSimCardBindState)
    	{
    		if (m_IsJinRuiSimCard > 0)
    		{
    			*nType = 1;
    		}
    		else
    		{
    			*nType = 0;
    		}
    	}
    	else
    	{
    		//非运营模式，不会p2p断开网络.设置不是金睿卡
    		*nType = 0;
    	} 
    }
    else
    {
        *nType = 0;
    }
    
	return 0;
}

/*获取允许联网时间*/
int INet::Net_GPRS_FL_AllowTime(int *nTime)
{
	if (NULL == nTime)
    {
        NET_ERR("NULL POINTER EXCEPTION!\n");
        return -1;
    }

	//金睿卡，15分钟断开连接
	*nTime = 15*60;
	
	return 0;
}

int INet::NET_GPRS_GetFlAppkey(char  * gprs_appkey)
{
	 if (NULL == gprs_appkey)
    {
        NET_ERR("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    strncpy(gprs_appkey, m_szAppkey, (size_t)sizeof(m_szAppkey));

	return 0;
}

//获取北纬平台appid
int INet::NET_GPRS_GetNorthLatiAppid(char  * gprs_appid)
{
	
    if (NULL == gprs_appid)
    {
        NET_ERR("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    strncpy(gprs_appid, m_szNorthLatiAppid, (size_t)sizeof(m_szNorthLatiAppid));

	return 0;
}

//获取北纬平台key
int INet::NET_GPRS_GetNorthLatiAppkey(char  * gprs_appkey)
{
	 if (NULL == gprs_appkey)
    {
        NET_ERR("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    strncpy(gprs_appkey, m_szNorthLatiAppkey, (size_t)sizeof(m_szNorthLatiAppkey));

	return 0;
}

//获取北纬平台cardtype
int INet::NET_GPRS_GetNorthLatiCardType(char  * gprs_cardtype,int len)
{
	 if (NULL == gprs_cardtype)
    {
        NET_ERR("NULL POINTER EXCEPTION!\n");
        return -1;
    }

	snprintf(gprs_cardtype,len-1,"%d",m_NorthLatiCardType);

	return 0;
}

//设置卡类型
int INet::NET_SetSimCardType(int  * simcard_type)
{
	if (NULL == simcard_type)
    {
        NET_ERR("NULL POINTER EXCEPTION!\n");
        return -1;
    } 	

	return 0;
}

//获取当前云卡模式
int INet::Net_GetUcWorkMode(int *piUcWorkMode)
{
    if (!bSupportGPRS)
    {
        return -1;
    }

    if (NULL == piUcWorkMode)
    {
        NET_ERR("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    if(Module4G_GetUcWorkMode(piUcWorkMode))
    {
        NET_ERR("Fail to called Module4G_GetUcWorkMode!\n");
        return -1;
    }

    return 0;
}

//获取是否是云卡设备
int INet::Net_GetIsCloudCardDev(void)
{
    if (!bSupportGPRS)
    {
        return 0;
    }

#ifdef __MODULE_4G_SUPPORT__
    return Module4G_GetIsCloudCardDev();
#endif
    return 0;
}

#endif

int INet::checkP2PStatusAbnormal()
{
    int conn_status = 0;
    static unsigned char sflag = 0;
    static unsigned char timecnt = 0;
    static unsigned char rebootcnt = 0;

    /* p2p状态正常，直接返回 */
    if (eP2PStauts_InternetReady == m_iTutkStatus)
    {
        sflag = 1;
        timecnt = 0;
        rebootcnt = 0;
        return 0;
    }

    if (sflag == 0)
    {
        return 0;
    }

    /* 每隔30S获取当前设备实际联网状态 */
    if (++timecnt >= 30)
    {
        timecnt = 0;

        Net_CheckIfNetAccessToInternet(&conn_status);
        if (conn_status == 1)
        {
            rebootcnt++;
            NET_ERR("[P2P_ERR] m_iTutkStatus[%d] rebootcnt[%d]\n", m_iTutkStatus, rebootcnt);
        }
        else
        {
            rebootcnt = 0;
        }
    }

    /* 连续10次出现有网络，但是P2P不在线，重启设备 */
    if (rebootcnt >= 10)
    {
        RebootLinux(REBOOT_P2P_STATUS_ABNORMAL);
    }

    return 0;
}


//设置AuthP2p信息
int INet::NET_SetAuthP2pInfo(Net_AuthP2pInfo *ptAuthP2pInfo)
{
    if (NULL == ptAuthP2pInfo)
    {
        NET_ERR("Param invalid\n");
        return -1;
    }

    int nRet = 0;
    StuAuthP2pInfo stAuthP2pInfo = {0};

	m_AuthP2pInfo.iP2PHDR = ptAuthP2pInfo->iP2PHDR;
    m_AuthP2pInfo.bValidP2pInfo = ptAuthP2pInfo->bValidP2pInfo;

    stAuthP2pInfo.iP2PHDR = m_AuthP2pInfo.iP2PHDR;

    nRet = Auth_SetAuthP2pInfo(&stAuthP2pInfo);
    NET_INFO("Auth_SetKeepAliveInfo nRet=%d,stAuthP2pInfo.iP2PHDR=%d\n", nRet,stAuthP2pInfo.iP2PHDR);
    if (0 != nRet)
    {
        NET_ERR("NET_SetAuthP2pInfo failed: ret=%d \n", nRet);
        return -2;
    }
    
    return 0;
}


//获取授权P2P信息
int INet::NET_GetAuthP2pInfo(Net_AuthP2pInfo *ptAuthP2pInfo)
{
    if (NULL == ptAuthP2pInfo)
    {
        NET_ERR("Param invalid\n");
        return -1;
    }

    *ptAuthP2pInfo = m_AuthP2pInfo;

    return 0;
}

static void *DomainResolutionManageThread(void *lParam)
{
	int result = 0;
	DNSThrParam *pParam = (DNSThrParam *)lParam;
	INet *pnet = INet::GetInstance();
	ZWHost_DNS_SERVER_INFO_LIST DnsList[1] = {0};

	pthread_detach(pthread_self());
	while(1)
	{
		//主DNS地址
		char tmpPrimaryDNS[32]={0};
		//第二DNS地址
		char tmpSecondaryDNS[32]={0};
		char szGateway[32]={0};

		pthread_mutex_lock(&m_ThreadParamLock);
        // 拷贝最新参数到线程局部变量（解锁后不影响）
        strncpy(tmpPrimaryDNS, pParam->primaryDNS, sizeof(tmpPrimaryDNS)-1);
        strncpy(tmpSecondaryDNS, pParam->secondaryDNS, sizeof(tmpSecondaryDNS)-1);
        strncpy(szGateway, pParam->gateway, sizeof(szGateway)-1);
		tmpPrimaryDNS[sizeof(tmpPrimaryDNS)-1] = '\0';
        tmpSecondaryDNS[sizeof(tmpSecondaryDNS)-1] = '\0';
        szGateway[sizeof(szGateway)-1] = '\0';
        pthread_mutex_unlock(&m_ThreadParamLock);

		memset(DnsList, 0, sizeof(ZWHost_DNS_SERVER_INFO_LIST));
		pnet->NET_UseZwhostCalcDomain(tmpPrimaryDNS,tmpSecondaryDNS,szGateway,DnsList);
		pthread_mutex_lock(&m_ThreadParamLock);
		if(pParam->count > 0)
		{
			pParam->count = 0;
			pthread_mutex_unlock(&m_ThreadParamLock);
			continue;
		}
		pthread_mutex_unlock(&m_ThreadParamLock);
		WriteDNSFileByZwhost(DnsList);

		res_init();
			
		/* 网络状态发生改变需要更新hosts */
		NET_DBG("--- Update the hosts start ---\n");
		result = Hosts_Update();
		if (result)
		{
			NET_ERR("Fail to update the hosts, errno = %d\n", result);
		}

		res_init();	
		break;
	}
	pthread_mutex_lock(&m_ThreadParamLock);
	if(pParam->pThrID != NULL)
	{
		*(pParam->pThrID) = 0;
	}
	pthread_mutex_unlock(&m_ThreadParamLock);
	return NULL;
}

int INet::TcpRelay_GetEnable(int *bEnable)
{
	if (NULL!=bEnable)
	{	
		*bEnable = TcpRelayEnable;
	}
	NET_INFO("Get TcpRelay:%d\n",*bEnable);
	return 0;
}
int INet::TcpRelay_SetEnable(int *bEnable)
{
	NET_INFO("Set TcpRelay:%d\n",*bEnable);
	if (NULL!=bEnable)
	{
		//获取新的配置文件读取实例
		CConfig *pCfg = CConfig::GetInstance();	
		if (pCfg)
		{
			pCfg->SetValue(TcpRelay, TcpEnable, *bEnable);
			TcpRelayEnable = *bEnable;
		}
	}
	HY_DVR_TCPRELAY_ENABLE cfg;
	cfg.iValid = 1;
	cfg.iEnable = *bEnable;
	HieServer_SetConfig(PROTOCOL_TUTK  , HY_DVR_SET_TCPRELAY_INFO, TCPRELAY_ENABLE, (char *)&cfg, sizeof(cfg));
	return 0;
}

