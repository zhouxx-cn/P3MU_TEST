#ifndef __IDVR_WIFI_H_
#define __IDVR_WIFI_H_

#if defined(__cplusplus)
extern "C" {
#endif

//!brief WIFI热点名称最大长度 */
#define WLAN_MAX_SSID_LEN (128)
//!brief WIFI密钥最大长度 */
#define WLAN_MAX_KEY_LEN (128)
//!brief 可以识别的WIFI热点最大个数 */
#define WLAN_MAX_AP_NUM (32)
//!\brief 物理地址字符串长度
#define WLAN_MAC_ADDRESS_LEN (18)
#define WIFI_INFO_MAX_LINE     (20)
#define MAX_LEN (18)

typedef enum
{
	AM_OPEN = 0,
	AM_SHARED,
	AM_WPAPSK,
	AM_WPA2PSK,
	AM_WPANONE,
	AM_WPA,
	AM_WPA2,
	AM_NONE
} WLAN_AUTHMODE;

typedef enum
{
	ET_NONE = 0,
	ET_WEP,
	ET_TKIP,
	ET_AES
} WLAN_ENCRYPTYPE;

typedef struct
{
	// !\brief 热点名称
	char szSSID[WLAN_MAX_SSID_LEN];

	//!\brief 热点MAC地址
	char szMacAddress[WLAN_MAC_ADDRESS_LEN];

	//!\brief 热点信号强度
	int nQuality;

	//!0表示Open,1表示Shared,2表示WPAPSK, 3表示WPA2PSK, 4表示WPANONE(ad-hoc模式),5表示WPA,6表示WPA2
	int nAuthentication;

	//! 0表示NONE 1表示WEP 2表示TKIP 3表示AES
	int nEncrypType;

	//!\brief 热点支持的规范
	// 1: legacy 11B only
	// 2: legacy 11A only
	// 3: legacy 11a/b/g mixed
	// 4: legacy 11G only
	// 5: 11ABGN mixed
	// 6: 11N only
	// 7: 11GN mixed
	// 8: 11AN mixed
	// 9: 11BGN mixed
	int nWirelessMode;
} WLAN_ACCESSPOINT, *LPWLAN_ACCESSPOINT;

typedef struct
{
    // !\brief 热点名称
    char szSSID[WLAN_MAX_SSID_LEN];
    //!\brief 热点信号强度
    int nQuality;
} WLAN_ACCESSSIGNALINFO, *LPWLAN_ACCESSSIGNALINFO;

typedef struct _wifiinfo_s {
    char essid[33];
    unsigned int enc_type;
    unsigned int quality;
} wifiinfo_t;
typedef enum wifi_enctype_e_ {
	ENCTYPE_INVALID,
	ENCTYPE_NONE,
	ENCTYPE_WEP,
	ENCTYPE_WPA_TKIP,
	ENCTYPE_WPA_AES,
	ENCTYPE_WPA2_TKIP,
	ENCTYPE_WPA2_AES
} enctype_t;
typedef struct 
{
	int bValid;

	// 密码
	char szKey[WLAN_MAX_KEY_LEN];
} WIFI_KEY;

typedef struct 
{
	int bValid;
	int nDefKeyID;
	WIFI_KEY Key1;
	WIFI_KEY Key2;
	WIFI_KEY Key3;
	WIFI_KEY Key4;
	WIFI_KEY WPAKey;
} WIFI_KEY_CFG, *LPWIFI_KEY_CFG;

typedef struct
{
	//!\brief 是否连接
	int bConnect;

	// !\brief 热点名称
	char szSSID[WLAN_MAX_SSID_LEN];

	//!\brief 热点MAC地址
	char szMacAddress[WLAN_MAC_ADDRESS_LEN];

	// 网络类型
	// 0 "Auto",
	// 1 "Ad-Hoc",
	// 2 "Managed", 一般应设为此模式
	// 3 "Master",
	// 4 "Repeater",
	// 5 "Secondary",
	// 6 "Monitor",
	// 7 "Unknown/bug"
	int nNetworkType;
	
	// 认证模式
	// 0-OPEN,1-SHARED,2-WPAPSK,3-WPA2PSK,4-WPANONE,5-WPA,6-WPA2
	int nAuthMode;

	// 加密算法 0-NONE,1-WEP,2-TKIP,3-AES
	int nEncrypType;

	// 密码配置
	WIFI_KEY_CFG struKeyCfg;
} WLAN_CONNECTION, *LPWLAN_CONNECTION;

#if 0
char * const WIFINETWORKTYPE[] =
{
	"Auto",
	"Adhoc",
	"Infra",
	"Master",
	"Repeater",
	"Secondary",
	"Monitor",
	"Unknown"
};

char * const WIFIAUTHMODE[] =
{
	"OPEN",
	"SHARED",
	"WPAPSK",
	"WPA2PSK",
	"WPANONE",
	"WPA",
	"WPA2"
};

char * const WIFIENCTYPE[] =
{
	"NONE",
	"WEP",
	"TKIP",
	"AES",
};

char *const WIFIPROTOCOL[] = 
{
	"legacy 11b/g mixed", 
	"legacy 11B only",
	"legacy 11A only",
	"legacy 11a/b/g mixed",
	"legacy 11G only",
	"11ABGN mixed",
	"11N only",
	"11GN mixed",
	"11AN mixed",
	"11BGN mixed",
	"11AGN mixed"
};
#endif

//!\brief wifi模块初始化
int wifi_init(unsigned int wifiMode);
int wifi_reset_enable(void);

int wifi_set_config_enable(int enable);
int wifi_reboot_interface(void);
int wifi_set_enable(int bEnable);

int wifi_get_enable(void);

int wifi_get_dev(void);
int wifi_get_conn_info_wpa(int from_ssid, const char *data, LPWLAN_CONNECTION conn);
int wifi_get_conn_info(int from_ssid, const char *data, LPWLAN_CONNECTION conn);
int wifi_disable_conn();
//!\brief 获取周围的热点信息
//!\param aps 热点数组
//!\param count 热点个数
//!\return 0表示获取成功,其他表示错误码
int wifi_get_accesspoints(WLAN_ACCESSPOINT aps[WLAN_MAX_AP_NUM], int *count);
int wifi_get_accesspoints_wpa(WLAN_ACCESSPOINT aps[WLAN_MAX_AP_NUM], int *count);
int wifi_AP_get_accesspoints(WLAN_ACCESSPOINT aps[WLAN_MAX_AP_NUM], int *count);
//!\brief 设置wifi连接参数
//!\param conn 连接参数
//!\return 0表示设置成功 其他表示表示错误码
int wifi_set_conn_cfg(LPWLAN_CONNECTION conn);
int wifi_scan_ap_result(wifiinfo_t *wifilist, int list_size);
//!\brief 获取wifi连接参数
//!\param conn 连接参数
//!\return 0表示成功, 其他表示错误码
int wifi_get_conn_cfg(LPWLAN_CONNECTION conn);

int wifi_test_conn_cfg(LPWLAN_CONNECTION conn);
//!\brief 获取wifi的连接状态
//!\param status 状态 0表示未连接 1表示已连接
//!\param ssid 当前正在连接热点的SSID, mac 当前连接热点的MAC
//!\return 0表示获取成功，其他表示错误码
int wifi_get_conn_status(int *status, char ssid[WLAN_MAX_SSID_LEN], char mac[WLAN_MAC_ADDRESS_LEN]);
int wifi_get_signal_info_wpa(LPWLAN_ACCESSSIGNALINFO signal_info);
int wifi_get_signal_quality(int *nQuality);
int wifi_get_curr_status(int *status);
int wifi_get_ap_strength_percent(void);
int get_wifi_fail_reason(int *reason);
#if defined(__cplusplus)
}
#endif

#endif /*__IDVR_WIFI_H_*/

