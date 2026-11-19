//!\file wifi.c
//!\brief WIFI配置功能实现

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pthread.h>

#undef _GNU_SOURCE

#include "wifi_api.h"
#include "mysystem.h"

#include "dictionary.h"
#include "iniparser.h"
#include "sys_banned.h"

extern int IsStringInCommand(const char *pCommand, const char *pString);
extern int ShellCmdOut(const char *pCmd, char *pResult, int nLen);

#ifndef IWNAME
//#define IWNAME "wlan0"//ra0, wlan0
#endif
#define WIFI_USE_8189

#define WPA_LOG_FILE "/tmp/wpa.log"				//如果修改该路径，需要同步修改MonitorProcess、softAp命令定义
#define WIFI_KEY_INCORRECT "reason=WRONG_KEY"

static char IWNAME[6] = {0};
static int g_wifiStatus = 0;
static WLAN_ACCESSPOINT s_stWifiScanResultCache[WLAN_MAX_AP_NUM] = {0};
static int s_nWifiScanResultCntCache = 0;

#ifndef WIFI_EXIST_FlAG
#define WIFI_EXIST_FlAG      "/tmp/.wifi_if_exist"   // Wifi存在的标志，里面存放无线网卡名字
#endif

#define WIFI_ERR(x...)		\
do {						\
	printf("[WIFI][ERR][%d]", __LINE__);	\
	printf(x);				\
} while(0)

#define WIFI_INFO(x...)		\
do {						\
	printf("[WIFI][INFO][%d]", __LINE__);	\
	printf(x);				\
} while(0)

//#define DEBUG_WIFI

#ifdef DEBUG_WIFI
#define WIFI_DBG(x...)		\
do {						\
	printf("[WIFI][DEBUG][%d]", __LINE__);	\
	printf(x);				\
} while(0)
#else
#define WIFI_DBG(x...)
#endif

#define IW_CONN_CFG "/etc/network/wifi-cfg"

#define UTF8_ENCODE_SIZE 3
#define GBK_ENCODE_SIZE 2
//utf-8：1110 xxxx 10xx xxxx 10xx xxxx
#define IS_UTF8_ENCODE(X,Y,Z) ((0xe == (X>>4))&&(0x2 == (Y>>6))&&(0x2 == (Z>>6))?1:0)
#define IS_GBK_ENCODE(X) ((0x80 < X)? 1:0)

#define TEST_GET_WIFI_INFO_MAX   20

#define WIFI_SCAN_RESULT_CACHE_PATH "/tmp/.wifi_scan_result_cache"

#define ARRAY_SIZEOF(array)		(sizeof(array)/sizeof(array[0]))

typedef enum
{
    UTF8_ENCODE=0,
    GBK_ENCODE,
    SUM_ENCODE
}SSID_ENCODE;

typedef enum
{
	WSA_SSID,

	WSA_AP,

	WSA_NETWORKTYPE,
	
	WSA_AUTHMODE,

	WSA_ENCTYPE,

	WSA_DEFKEYID,

	WSA_KEY1,

	WSA_KEY2,

	WSA_KEY3,

	WSA_KEY4,

	WSA_WPAKEY
} WIFI_SET_ACTION;

typedef enum
{
	PN_ENABLE,
	PN_NETWORKTYPE,
	PN_AUTHMODE,
	PN_ENCTYPE,
	PN_DEFKEYID,
	PN_KEY1,
	PN_KEY2,
	PN_KEY3,
	PN_KEY4,
	PN_WPAKEY,
	PN_SSID
} PARAMNAME;

typedef enum
{
    ESSID = 0,
    QUALITY,
    ENCRYPTIONKEY,
    IE_0,
    GROUPCIPHER_0,
    PAIRWISECIPHERS_0,
    AUTHENTICATIONSUITES_0,
    IE_1,
    GROUPCIPHER_1,
    PAIRWISECIPHERS_1,
    AUTHENTICATIONSUITES_1
} WLAN_ACCESSPOINTINFO;

typedef enum
{
    QUALITY_8189 = 0,
	ENCRYPTIONKEY_8189,
    ESSID_8189,
	Extra_0_8189,
	Extra_1_8189,
    IE_6_8189,
    GROUPCIPHER_6_8189,
    PAIRWISECIPHERS_6_8189,
    AUTHENTICATIONSUITES_6_8189,
    IE_10_8189,
    GROUPCIPHER_10_8189,
    PAIRWISECIPHERS_10_8189,
    AUTHENTICATIONSUITES_10_8189
} WLAN_ACCESSPOINTINFO_8189;



typedef struct {
	char *name;
	PARAMNAME ename;
} WIFIPARAM;

#if 0
static WIFIPARAM wifi_param_table[] = 
{
	{"ENABLE", PN_ENABLE},
	{"NETWORKTYPE",	PN_NETWORKTYPE},
	{"AUTHMODE", PN_AUTHMODE},
	{"ENCTYPE",	PN_ENCTYPE},
	{"DEFKEYID", PN_DEFKEYID},
	{"KEY1", PN_KEY1},
	{"KEY2", PN_KEY2},
	{"KEY3", PN_KEY3},
	{"KEY4", PN_KEY4},
	{"WPAKEY", PN_WPAKEY},
	{"SSID", PN_SSID}
};
#endif

static int  wifi_hasdev = 0;
static int  wifi_enable = 0;
static int  wifi_max_sem = 0;
//static int  wifi_if_get_ssid = 0;
static char wifi_curr_ssid[WLAN_MAX_SSID_LEN] = {0};

static const char *accesspoints_info[] = {
        "ESSID",
        "Quality",
        "EncryptionKey",
        "IE_0",
        "GroupCipher_0",
        "PairwiseCiphers_0",
        "AuthenticationSuites_0",
        "IE_1",
        "GroupCipher_1",
        "PairwiseCiphers_1",
        "AuthenticationSuites_1"
    };

static const char *accesspoints_info_8189[] = {
        "ESSID",
        "Quality",
        "EncryptionKey",
        "IE",
        "GroupCipher",
        "PairwiseCiphers",
        "AuthenticationSuites",
    };

#if 1

/* 针对wifi的SSID，需要处理如下特殊字符(注意字符数组溢出)：
 * "  _ - ! " # $ % ' ( ) * + , . / : ; < = > ? @ [ \ ] ^ ` { | } ~ "
 * 其中，需要添加转义的字符：
 * 空格；双引号；反引号和转义符；
 */

#endif

static WLAN_CONNECTION wifi_connection;
static pthread_mutex_t m_getAccessPointsLock;
static pthread_mutex_t m_getWifiStatusLock;

static int clear_WPA_command_log();
static int wifi_get_curr_statusEx(int *status);

/**
 * 清除 iwlist wlan0 scan 产生的 与 accesspoints_info_8189 提到的不兼容的行
 * @szFile 输入文件，同时也是输出文件
 */
static int Wifi_ClearUnusedRowForWIFI(const char *szFile)
{
	const char szTmpFileName[128] = "/tmp/tmp_wifi_row_clean.txt";

	if (access (szTmpFileName, F_OK) == 0)
	{
		WIFI_ERR ("ERROR, tmp file detected\n");
		unlink (szTmpFileName);
	}

	char szCmd [256] = {};
	snprintf (szCmd, sizeof(szCmd), "cat %s | sed  '/^Extra/d' > %s", 
		szFile, szTmpFileName);

	mysystem (szCmd);	// 移除指定行，并存储到临时文件

	snprintf (szCmd, sizeof(szCmd), "mv %s %s", szTmpFileName, szFile);

	mysystem (szCmd);	// 修改临时文件为输出文件

	/* debug use */
	if (0)
	{
		snprintf (szCmd, sizeof(szCmd), "echo wifi info;cat %s; echo wifi info end",
			szFile);
		mysystem (szCmd);
	}

	return 0;
}

/* 字符串格式化为MAC地址，将获取到的字符串的"-"替换为":" */
#define STRING_FORMAT_TO_MAC(SRC_STR, DEST_STR, DEST_STR_SIZE)  \
({                                               \
    unsigned int ucMac[6] = {0};                 \
    sscanf(SRC_STR,"%02x-%02x-%02x-%02x-%02x-%02x", &ucMac[0],&ucMac[1],&ucMac[2],&ucMac[3],&ucMac[4],&ucMac[5]); \
    snprintf(DEST_STR, DEST_STR_SIZE, "%02x:%02x:%02x:%02x:%02x:%02x", ucMac[0],ucMac[1],ucMac[2],ucMac[3],ucMac[4],ucMac[5]);   \
 })

/* MAC地址格式化字符串，将MAC地址的"-"替换为":" */
#define MAC_FORMAT_TO_STRING(SRC_STR, DEST_STR, DEST_STR_SIZE)  \
({                                               \
    unsigned int ucMac[6] = {0};                 \
    sscanf(SRC_STR,"%02x-%02x-%02x-%02x-%02x-%02x", &ucMac[0],&ucMac[1],&ucMac[2],&ucMac[3],&ucMac[4],&ucMac[5]); \
    snprintf(DEST_STR, DEST_STR_SIZE, "%02x:%02x:%02x:%02x:%02x:%02x", ucMac[0],ucMac[1],ucMac[2],ucMac[3],ucMac[4],ucMac[5]);   \
 })
 
/*@ brief wifi_check_ssid_encode 检测SSID字符串中存在的编码格式
 *@ param[IN]  src 欲操作的字符串
 *@ param[OUT] ssid_encode 输出编码格式
 *@ return -1:失败; 0:成功
 */
static int wifi_check_ssid_encode(const char *src,SSID_ENCODE *ssid_encode)
{
	int srcc=0;
	int srclen=0;

    if(NULL == src)
    {
        WIFI_ERR("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    srclen = strlen(src);
    
	while(1)
	{
		if(UTF8_ENCODE_SIZE > srclen-srcc)
		{
			if((GBK_ENCODE_SIZE == srclen-srcc)&&(IS_GBK_ENCODE(src[srcc])))
			{
			    //gbk
                WIFI_INFO("*********gbk encode********\n");
                *ssid_encode = GBK_ENCODE;
                return 0;
			}
			break;
		}

        if(IS_UTF8_ENCODE(src[srcc],src[srcc+1],src[srcc+2]))
		{
		    WIFI_INFO("*********utf encode********\n");
            *ssid_encode = UTF8_ENCODE;
            return 0;
		}
        else if(IS_GBK_ENCODE(src[srcc]))
		{
		    WIFI_INFO("*********gbk encode********\n");
            *ssid_encode = GBK_ENCODE;
            return 0;
		}
		else
		{
		    //ascii
            srcc++;
		}
	}
    *ssid_encode = UTF8_ENCODE;
	return 0;
}

/*@ brief StripString 提取原始字符串start_char字符和stop_char字符之间的字符串(不包括这两个字符)
 *@ param[IN]  src_str 欲操作的字符串
 *@ param[IN]  start_char 提取的起始字符
 *@ param[IN]  end_char 提取的结束字符
 *@ param[OUT] dst_str 输出字符串
 *@ return -1:失败; 0:成功
 */
static int StripString(const char *src_str, char start_char, char end_char, char *dst_str)
{
    if (!src_str || !dst_str)
    {
        WIFI_ERR("NULL POINTER EXCEPTION!\n");
        return -1;
    }
    char *p1 = strchr(src_str, start_char);
    //第一次出现的位置
    //char *p2 = strchr(src_str, end_char);
    //最后出现的位置
    char *p2 = strrchr(src_str, end_char);

    
    if (p1 && p2) {
        strncpy(dst_str, p1+1, p2-p1-1);
        dst_str[p2-p1-1] = '\0';
    } else {
        return -1;
    }
    return 0;
}

/*@ brief IncludeChinese 判断字符串str里面是否有中文
 *@ param[IN] str 欲判断的字符串
 *@ return -1:错误 0:无中文 1:有中文
 */
static inline int IncludeChinese(const char *str)
{
    if (NULL == str)
    {
        WIFI_ERR("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    char c = '\0';
    while(1)
    {
        c = *str++;
        /* 如果到字符串尾则说明该字符串没有中文字符 */
        if (c == '\0')
        {
            break;    
        }
        /* 如果到字符串尾则说明该字符串没有中文字符 */
        if ((c&0x80) && (*str&0x80))
        {                                                                                                            
            return 1;
        }
    }
    return 0;
}

static int getMinValue(int *buf, int len, int *MinNum)
{
	int i;
	int MinValue;

	if((NULL == buf) || (NULL == MinNum))
		return -1;
	
	for(i=0,MinValue=*buf,*MinNum=0;i<len;i++)
	{
		if(*(buf+i) < MinValue)
		{
			MinValue = *(buf+i);
			*MinNum = i;
		}
	}
	*MinNum = *MinNum + 1;

	return 0;
}

/* 获取当前连接wifi的ssid的信号强度的百分比，以便进行码率适配 */
int wifi_get_ap_strength_percent(void)
{
    int curStrength = 0;
    
    char tempfile[256] = {0};
	char buffer[512] = {0};
//    char *getssid = " wpa_cli status | grep ssid | grep -v bssid | awk -F \"=\" '{print $2}'";
    char *point = NULL;
    FILE *fp = NULL;
	struct timeval now;
	    
	if (!wifi_hasdev)
	{
	    return -1;
	}

    /* 1. 获取当前wifi连接状态，只获取连接上的ssid的信号强度 */
    if (IsStringInCommand("wpa_cli status", "COMPLETED") != 1)
    {
        WIFI_ERR("Wifi is disconnect!\n");
        return -1;
    }
    
    /* 2. 获取当前wifi连接的ssid: wpa_cli status | grep ssid | grep -v bssid | awk -F "=" '{print $2}' */
	/*
	if (wifi_if_get_ssid == 0)
    {
        memset(wifi_curr_ssid, 0, sizeof(wifi_curr_ssid));
        ShellCmdOut(getssid, wifi_curr_ssid, sizeof(wifi_curr_ssid));
        wifi_curr_ssid[strlen(wifi_curr_ssid)-1] = '\0';
        wifi_if_get_ssid = 1;
    }
    */
    /* 3. 根据ssid获取当前的信号强度 */
	gettimeofday(&now, NULL);
#ifdef __MUSL__
    snprintf(tempfile, 255, "/var/run/wpabss.%lld", now.tv_usec);
#else
    snprintf(tempfile, 255, "/var/run/wpabss.%ld", now.tv_usec);
#endif
	snprintf(buffer, sizeof(buffer), "/usr/sbin/wpa_cli bss %s > %s", wifi_curr_ssid, tempfile);
	mysystem(buffer);
    
    fp = fopen(tempfile, "r");
	if (NULL == fp)
	{
		WIFI_ERR("...open(/var/run/wpabss.%d) faild!....\n", getpid());
		return -1;
	}

    while (!feof(fp))
	{
		memset(buffer, 0, sizeof(buffer));
		fgets(buffer, 256, fp);
        point = strstr(buffer, "level");        
        if (point)
        {
            point += 6;
            sscanf(point, "%d", &curStrength);
            break;
        }
    }
    fclose(fp);
	unlink(tempfile);
	point = NULL;
    
    if (!curStrength || !wifi_max_sem)
    {        
        WIFI_ERR("curStrength = %d, wifi_max_sem = %d\n", curStrength, wifi_max_sem);
        return -1;
    }
    
    return (curStrength*100/wifi_max_sem);
}

static int set_wep_key(int index, char *value);

static int load_conn_cfg(void);

static int save_conn_cfg(void);

static int has_dev(const char *devname);

static int enable(int enable);

static int MaxSemOfWlanDev(void);

int wifi_init(unsigned int wifiMode)
{
	WIFI_DBG("Enter wifi_init\n");
	int ret = 0; 
  	if (2 == wifiMode)
	{
		strncpy(IWNAME, "wlan0", sizeof(IWNAME) - 1);
		IWNAME[sizeof(IWNAME) - 1] = '\0';
	}
	else
	{
		strncpy(IWNAME, "wlan0", sizeof(IWNAME) - 1);
		IWNAME[sizeof(IWNAME) - 1] = '\0';
		load_conn_cfg();
	}

	if (has_dev(IWNAME))
	{
		wifi_hasdev = 1;
	}
	else
	{
		WIFI_INFO("no wifi dev\n");
		wifi_hasdev = 0;
		WIFI_DBG("Exit wifi_init\n");
		return -1;
	}

    memset(&wifi_connection, 0, sizeof(wifi_connection));
	/* 获取无线网卡类型 */
    wifi_max_sem = MaxSemOfWlanDev();
    WIFI_DBG("wifi_max_sem = %d\n", wifi_max_sem);
    pthread_mutex_init(&m_getAccessPointsLock, NULL);
    pthread_mutex_init(&m_getWifiStatusLock, NULL);

	WIFI_DBG("Exit wifi_init\n");
	return ret;
}

int wifi_set_enable(int bEnable)
{
	if (!wifi_hasdev)
	{
		return -1;
	}
	WIFI_DBG("Enter wifi_set_enable, bEnable=%d\n", bEnable);
	if (bEnable != wifi_enable)
	{
		wifi_enable = (!bEnable) ? 0 : 1;
		enable(wifi_enable);
		save_conn_cfg();
	}
	WIFI_DBG("Exit wifi_set_enable\n");

	return 0;
}

void sort_by_quality(wifiinfo_t* wifi_list, int wifi_list_len)
{
    int i;
    int j;
    wifiinfo_t wifiList;
	memset(&wifiList, 0, sizeof(wifiinfo_t));
    for (i=0; i<wifi_list_len; i++) {
        for (j=0; j<wifi_list_len-i-1; j++) {
            if (wifi_list[j].quality < wifi_list[j+1].quality) {
                memcpy(&wifiList, &wifi_list[j+1], sizeof(wifiinfo_t));
                memcpy(&wifi_list[j+1], &wifi_list[j], sizeof(wifiinfo_t));
                memcpy(&wifi_list[j], &wifiList, sizeof(wifiinfo_t));
            }
        }
    }
}
static int wifi_parse_encry_mode(wifiinfo_t *wifilist, char *enc_flag)
{
	char *str1 = NULL;
	str1 = strstr(enc_flag, "WPA-PSK-CCMP");
	if (str1) {
        wifilist->enc_type = ENCTYPE_WPA_AES;
		goto quitout;
	}
	str1 = strstr(enc_flag, "WPA-PSK-TKIP");
	if (str1) {
        wifilist->enc_type = ENCTYPE_WPA_TKIP;
		goto quitout;
	}
	str1 = strstr(enc_flag, "WPA2-PSK-TKIP");
	if (str1) {
        wifilist->enc_type = ENCTYPE_WPA2_TKIP;
		goto quitout;
	}
	str1 = strstr(enc_flag, "WPA2-PSK-CCMP");
	if (str1) {
        wifilist->enc_type = ENCTYPE_WPA2_AES;
		goto quitout;
	}

	str1 = strstr(enc_flag, "WPS");
	if (str1) {
        wifilist->enc_type = ENCTYPE_WPA_AES;
		goto quitout;
	}
	
    if (!strcmp(enc_flag, "[ESS]")) {
        wifilist->enc_type = ENCTYPE_NONE;
        goto quitout;
    }

    wifilist->enc_type = ENCTYPE_WPA_AES;
	
quitout:
	return 0;
}

static int wifi_parse_ap_result(char *result_buf, wifiinfo_t *wifilist, int list_size)
{
	int list_count = 0;
	char *p_save = NULL;
	char *pstr1 = NULL;
	char *pstrbuf = NULL;
	int sccanf_count = 0;
	wifiinfo_t info;    
	char mac[32];
	int frequency;
    char enc_flag[64];
		
	pstr1 = strstr(result_buf, "/ ssid\n");
	if (pstr1 == NULL) {
		return -1;
	}
	
	pstrbuf = pstr1 + strlen("/ ssid\n");
	
	pstr1 = strtok_r(pstrbuf, "\n", &p_save);
	if (pstr1 == NULL) {
		return -1;
	}

	while (pstr1) {
		if (list_count >= list_size) {
			break;
		}

		memset(&info, 0, sizeof(info));
        memset(mac, 0, sizeof(mac));
        memset(enc_flag, 0, sizeof(enc_flag));
		sccanf_count = sscanf(pstr1, "%s %d %d %s %32[^\r\n]s", mac, &frequency,
			&info.quality, enc_flag, info.essid);
		if (sccanf_count != 5) {
			printf("sccanf_count = %d is wrong \n", sccanf_count);			
		} else {
			memcpy(&wifilist[list_count], &info, sizeof(info));
			wifi_parse_encry_mode(&wifilist[list_count], enc_flag);
			list_count++;
		}
		
		pstr1 = strtok_r(NULL, "\n", &p_save);
	}

	printf("list_count = %d\n", list_count);
	return list_count;
}

int wifi_scan_ap_result(wifiinfo_t *wifilist, int list_size)
{
	FILE *pfd = NULL;
	char shellcmd[256] = {0};
	char buffer[4096] = {0};
	int readsize = 0;
	int wifi_count = 0;
    wifiinfo_t tmpInfo[60];

	//扫描
    mysystem("wpa_cli -iwlan0 scan");
    usleep(1000 * 100);
	memset(shellcmd, 0, sizeof(shellcmd));
	snprintf(shellcmd, sizeof(shellcmd) - 1, "wpa_cli  -iwlan0 scan_result");
	
	pfd = popen(shellcmd, "r");
	if (pfd == NULL) {
		return -1;
	}
	readsize = fread(buffer, 1, sizeof(buffer), pfd);
	pclose(pfd);
    if (0 >= readsize) {
        return -1;
    }
    readsize = readsize <= sizeof(buffer) - 1 ? readsize : sizeof(buffer) - 1;
    buffer[readsize] = '\0';

	memset(tmpInfo, 0, sizeof(tmpInfo));
    wifi_count = wifi_parse_ap_result(buffer, tmpInfo, 30 * 2);
    sort_by_quality(tmpInfo, wifi_count);
    wifi_count = wifi_count < list_size ? wifi_count : list_size;
    memcpy(wifilist, tmpInfo, sizeof(wifiinfo_t) * wifi_count);

    return wifi_count;
}
int wifi_get_enable(void)
{
	//WIFI_DBG("Enter wifi_get_Enable\n");
	if (!wifi_hasdev)
	{
		//WIFI_DBG("no wifi dev\n");
		return 2;
	}
	else if (wifi_enable)
	{
		//WIFI_DBG("wifi enabled\n");
		return 1;
	}
	else
	{
		//WIFI_DBG("wifi disabled\n");
		return 0;
	}
}


/* 由无线网卡类型 */
int MaxSemOfWlanDev(void)
{
	int nMaxSem = 0;
	FILE *fp = NULL;
	char buf[128] = {0};
	char command[256] = {0};
	char fileName[64] = {0};

	strncpy(fileName, "/tmp/.MaxSemOfWlanDev", sizeof(fileName) - 1);
	snprintf(command, sizeof(command), "cat %s | grep WlanDev -m 1 | awk -F \"=\" '{print $2}' > %s", WIFI_EXIST_FlAG, fileName);

	mysystem(command);
	/* 将获取到的信息放到缓冲区buf中 */
	if ((fp = fopen(fileName, "r")) == NULL)
	{
		WIFI_ERR("Fail to fopen %s: %s\n", fileName, strerror(errno));
		return -1;
	}
	
	if (fgets(buf, sizeof(buf), fp) == NULL)
	{
		perror("fgets");
		fclose(fp);
		return -1;
	}
	
	if (buf[strlen(buf)-1] == '\n')
	{
		buf[strlen(buf)-1] = '\0';
	}

	if (strcmp(buf, "8189es") == 0)
	{
		nMaxSem = 100;
	}
	else if (strcmp(buf, "mt7601Usta") == 0)
	{
		nMaxSem = 255;
	}
	else
	{
		WIFI_ERR("Not support this wlan device %s, default MaxSem is 100!\n", buf);
		nMaxSem = 100;
	}
	
	fclose(fp);
	
	return nMaxSem;
}


int ApCmpMinSem(char *getCurrMsg, int nMinSem, int ifAPMode)
{
	int nCurrSem = 0;
	int i = 0;
    
	if (getCurrMsg == NULL || nMinSem < 0 || nMinSem > wifi_max_sem)
		return -1;

    char *p = getCurrMsg;

    if(ifAPMode)
    {
        // 打印出来的信息是经过对齐的，信号强度是从第81个空格开始的
        p += 80;
        sscanf(p, "%d", &nCurrSem);
    }
    else
    {
        for (i=0; i<2; i++)  // 跳过两个/t
        {
            while (('\t' != *p) && ('\0' != *p))
            {
                p++;
            }
            p++;
        }
        sscanf(p, "%d ", &nCurrSem);
		nCurrSem = nCurrSem * 100 / wifi_max_sem;
    }
//	printf("Current Sem is %d\n", nCurrSem);

	if (nCurrSem < 0 || nCurrSem > wifi_max_sem)
		return -1;
	
	if (nCurrSem > nMinSem)
		return 1;
	else if (nCurrSem == nMinSem)
		return 0;
	else
		return -1;
}

//跳过空格到达下个有效数据位置
int toNextValueSeg(char **p, int *lineEnd)
{
	*lineEnd = 0;

	while (' ' == **p)
	{
		//行结束判断
		if('\0' == **p)
		{ 
			*lineEnd = 1;
			break;
		}
		(*p)++;
	}

	return 0;
}

//获取有效字符
int getValueString(char *buf, char **p, int *lineEnd, int ifGetSSID)
{
	char *pShadow = *p;
	int i=0;
	int ssidLen = 33;
	*lineEnd = 0;
	int mac[6] = {0};
	int cnt = 0;

	if(ifGetSSID)
	{
		//先判断SSID是否为空
		cnt = sscanf(*p, "%02x:%02x:%02x:%02x:%02x:%02x", &mac[0], &mac[1], &mac[2], &mac[3], &mac[4], &mac[5]);
		if (6 == cnt)
		{
			//SSID号为空，丢掉
			printf("Warn: SSID is NULL!\n");
			*lineEnd = 1;
			return 0;
		}
		while((' ' == pShadow[ssidLen - 1]) && (ssidLen > 0))
		{
			ssidLen--;
		}

		if(ssidLen > 0)
		{
			strncpy(buf, *p, ssidLen - 1);
			(*p) += ssidLen;
		}
		else
		{
			*lineEnd = 1;
			return 0;
		}
	}
	else
	{
		while(' ' != **p)
		{
			//行结束判断
			if('\0' == **p)
			{
				*lineEnd = 1;
				break;
			}

			buf[i++] = **p;
			(*p)++;
		}
	}

	return 0;
}

int decryptionSecurity(char *value, int *authentication, int *encrypType)
{
	char *p = value;

	if(strstr(value, "WPAPSK") || strstr(value, "WPA1PSK"))
		*authentication = AM_WPAPSK;
	else if(strstr(value, "WPA2PSK"))
		*authentication = AM_WPA2PSK;
	else if(strstr(value, "WPA2"))
		*authentication = AM_WPA2;
	else if(strstr(value, "WPA"))
		*authentication = AM_WPA;
	else if(strstr(value, "NONE"))
	{
		*authentication = AM_OPEN;
		*encrypType = ET_NONE;
		return 0;
	}
	else if(strstr(value, "WEP"))
	{
		*authentication = AM_SHARED;
		*encrypType = ET_WEP;
		return 0;
	}
	else
	{
		printf("ERROR: unknown authentication %s\n", value);
		return -1;
	}

	while(('/' != *p) && ('\0' != *p))
	{
		p++;
	}
	
	if('/' == *p)
	{
		p++;
        if(strstr(p, "AES"))
        {
            *encrypType = ET_AES;
        }
        else if(strstr(p, "TKIP"))
        {
            *encrypType = ET_TKIP;
        }
        else
        {
            printf("ERROR: unknown encrypType %s\n", p);
            return -1;
        }
	}
	else
	{
		printf("ERROR: unknown encrypType %s\n", value);
		return -1;
	}

	return 0;
}

int accessPointsCmp(const void *a, const void *b)
{
	const WLAN_ACCESSPOINT *aa = (const WLAN_ACCESSPOINT *)a;
	const WLAN_ACCESSPOINT *bb = (const WLAN_ACCESSPOINT *)b;
	
	return bb->nQuality - aa->nQuality;
}

int wifi_AP_get_accesspoints(WLAN_ACCESSPOINT aps[WLAN_MAX_AP_NUM], int *count)
{
	WIFI_DBG("Enter wifi_get_accesspoints\n");
	int ret = 0;
	int i = 0;
	int AccessPointQuality[WLAN_MAX_AP_NUM] = {0};
	int AccessPointFlowFlag=0;
	int minValue=0;
	int lineEnd = 0;
	int channel = 0;
	struct timeval now;
	char *p = NULL;
	int cnt = 0;
	char tempfile[256] = {0};
	char buffer[512] = {0};
	char value[128] = {0};
	int mac[6] = {0};
	
	gettimeofday(&now, NULL);
	memset(aps, 0, sizeof(WLAN_ACCESSPOINT) * WLAN_MAX_AP_NUM);

	pthread_mutex_lock(&m_getAccessPointsLock);

	/* 该命令会影响WIFI性能，不能频繁执行，否则会造成网络ping包很大或者超时 */
	mysystem("iwpriv ra0 set SiteSurvey=0");
	usleep(50000);
#ifdef __MUSL__
	snprintf(tempfile, 255, "/var/run/aps.%lld", now.tv_usec);
#else
	snprintf(tempfile, 255, "/var/run/aps.%ld", now.tv_usec);
#endif
	snprintf(buffer, sizeof(buffer), "iwpriv ra0 get_site_survey>%s", tempfile);
	mysystem(buffer);
	FILE *fp = fopen(tempfile, "r");
	if (NULL == fp)
	{
		printf("\n.......fopen	tempfile...faild!!!!!!......\n");
		unlink(tempfile);
		pthread_mutex_unlock(&m_getAccessPointsLock);
		return -1;
	}
	
	*count = 0;
	while (!feof(fp))
	{
		memset(buffer, 0, sizeof(buffer));
		fgets(buffer, 256, fp);
		lineEnd = 0;
		printf("buffer=%s\n", buffer);
		/* 如果当前是第十一个AP信号，而且信号强度比前面十个的最小值要大，就覆盖这个最小值 */
#if 1
		if (AccessPointFlowFlag==1)
		{
			if (ApCmpMinSem(buffer, aps[minValue-1].nQuality, 1)==1)
			{
				(*count) = minValue - 1;
			}
			else
				continue;
		}
#endif
		//信道获取
		cnt = sscanf(buffer, "%d", &channel);
		if((cnt < 1) || (cnt > 13))
			continue;

		p = &buffer[2];
		toNextValueSeg(&p, &lineEnd);
		if(lineEnd)
		{
			memset(&aps[*count], 0, sizeof(aps[0]));
			continue;
		}
		//printf("buf:%s\n", p);	

		//获取SSID
		memset(value, 0, sizeof(value));
		getValueString(value, &p, &lineEnd, 1);
		if(lineEnd)
		{
			memset(&aps[*count], 0, sizeof(aps[0]));
			continue;
		}
		//printf("Get ssid:%s\n", value);
		//printf("buf:%s\n", p);
	 	memcpy(aps[*count].szSSID, value, WLAN_MAX_SSID_LEN);
		toNextValueSeg(&p, &lineEnd);
		if(lineEnd)
		{
			memset(&aps[*count], 0, sizeof(aps[0]));
			continue;
		}
		//printf("buf:%s\n", p);	

		//获取MAC地址
		memset(value, 0, sizeof(value));
		getValueString(value, &p, &lineEnd, 0);
		if(lineEnd)
		{
			memset(&aps[*count], 0, sizeof(aps[0]));
			continue;
		}
		//printf("Get MAC:%s\n", value);
		//printf("buf:%s\n", p);		
		cnt = sscanf(value, "%02x:%02x:%02x:%02x:%02x:%02x", &mac[0], &mac[1], &mac[2], &mac[3], &mac[4], &mac[5]);
		if (cnt < 6)
		{
			continue;
		}
		snprintf(aps[*count].szMacAddress, 18, "%02x:%02x:%02x:%02x:%02x:%02x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
		toNextValueSeg(&p, &lineEnd);
		if(lineEnd)
		{
			memset(&aps[*count], 0, sizeof(aps[0]));
			continue;
		}
		//printf("buf:%s\n", p);	

		//获取加密
		memset(value, 0, sizeof(value));
		getValueString(value, &p, &lineEnd, 0);
		if(lineEnd)
		{
			memset(&aps[*count], 0, sizeof(aps[0]));
			continue;
		}
		//printf("Get Enc:%s\n", value);
		//printf("buf:%s\n", p);	
		ret = decryptionSecurity(value, &aps[*count].nAuthentication, &aps[*count].nEncrypType);
		if(ret)
		{
			memset(&aps[*count], 0, sizeof(aps[0]));
			continue;
		}
		//printf("Get nAuthentication=%d, nEncrypType=%d\n", aps[*count].nAuthentication, aps[*count].nEncrypType);
		toNextValueSeg(&p, &lineEnd);
		if(lineEnd)
		{
			memset(&aps[*count], 0, sizeof(aps[0]));
			continue;
		}
		//printf("buf:%s\n", p);
		
		//获取信号强度
		memset(value, 0, sizeof(value));
		getValueString(value, &p, &lineEnd, 0);
		if(lineEnd)
		{
			memset(&aps[*count], 0, sizeof(aps[0]));
			continue;
		}
		sscanf(value, "%d", &aps[*count].nQuality);
		//printf("Get Siganl:%d\n", aps[*count].nQuality);
		//printf("buf:%s\n", p);	
		//aps[*count].nQuality = aps[*count].nQuality > wifi_max_sem ? wifi_max_sem : aps[*count].nQuality;
		//aps[*count].nQuality = (aps[*count].nQuality) * 100 / wifi_max_sem; // 将变化值限定在0-100之内
		AccessPointQuality[*count] = aps[*count].nQuality;

		/* 如果当前在处理第WLAN_MAX_AP_NUM个(下标为WLAN_MAX_AP_NUM-1) */
		if((((WLAN_MAX_AP_NUM - 1) == (*count))) || (AccessPointFlowFlag))
		{
			AccessPointFlowFlag = 1;
			minValue = WLAN_MAX_AP_NUM;
			/* 获取前面十个AP信号中的最小值的坐标+1 */
			getMinValue(AccessPointQuality, WLAN_MAX_AP_NUM, &minValue);
			printf("minValue=%d\n", minValue);
		}
		else
			(*count)++;

	}
	if(AccessPointFlowFlag)
	{
		*count = WLAN_MAX_AP_NUM;
	}

	fclose(fp);
	unlink(tempfile);
	pthread_mutex_unlock(&m_getAccessPointsLock);

	int findLastAp = 0;
	
	for(i=0; i<WLAN_MAX_AP_NUM; i++)
	{
		if(strlen(wifi_connection.szSSID) && (!strcmp(aps[i].szSSID, wifi_connection.szSSID)))
		{
			WLAN_ACCESSPOINT temp;
			findLastAp = 1;
			printf("--------------findLastAp------------\n");
			memset(&temp, 0, sizeof(WLAN_ACCESSPOINT));
			memcpy(&temp, &aps[0], sizeof(WLAN_ACCESSPOINT));
			memcpy(&aps[0], &aps[i], sizeof(WLAN_ACCESSPOINT));
			memcpy(&aps[i], &temp, sizeof(WLAN_ACCESSPOINT));
		}
	}

	if(findLastAp)
		qsort(&aps[1], WLAN_MAX_AP_NUM - 1, sizeof(WLAN_ACCESSPOINT), accessPointsCmp);
	else
		qsort(&aps[0], WLAN_MAX_AP_NUM, sizeof(WLAN_ACCESSPOINT), accessPointsCmp);

	/*for(i=0;i<WLAN_MAX_AP_NUM;i++)
	{
		printf("%s     %s     %d-%d     %d\n",
			aps[i].szSSID, aps[i].szMacAddress, aps[i].nAuthentication, aps[i].nEncrypType, aps[i].nQuality);
	}*/

	return ret;
}


/* 中文wifi编码转换 */
int wifi_cn_ssid_encchange(WLAN_ACCESSPOINT *pApInfo)
{
	int iSsidlen = 0;
	int i = 0;
	FILE *fp = NULL;
	char buffer[256];

	memset(buffer, 0, 256);
	
    /* 中文转换 */
    iSsidlen = strlen(pApInfo->szSSID);
    for (i = 0; i < iSsidlen - 1; i++)
    {
        if ( '\\' == pApInfo->szSSID[i] && 'x' == pApInfo->szSSID[i + 1] )
        {
            //printf("[mytest]bCn iSsidlen = %d Ssid = %s\r\n", iSsidlen, pApInfo->szSSID);
            break;
        }
    }

    if (iSsidlen <= i)
    {
        /* 无中文格式SSID */
        return -1;
    }
    
    /* FIXME : linux echo -e 进行转码 */
    snprintf(buffer, 255, "echo -ne \"%s\" > %s", pApInfo->szSSID, "/tmp/aa");
    mysystem(buffer);
    fp = fopen("/tmp/aa", "r+");
    if (NULL == fp)
    {
        return -1;
    }

    memset(pApInfo->szSSID, 0, WLAN_MAX_SSID_LEN);
    fread(pApInfo->szSSID, 1, 128, fp);
    fclose(fp);

    return 0; 
}

static void get_ap_info_from_wpa_results(char *buf, WLAN_ACCESSPOINT *pApInfo)
{
	char *cur = NULL;
	char *next = NULL;
	char bss[24] = {0};
	char freq[16] = {0};
	char sig[16] = {0};
	int signal_level = 0;
	char flag[32] = {0};
	char ssid[256] = {0};
	//printf("-----1-----\n");
	//printf("buf == %s \n",buf);
	// bss
	cur = buf;
	if(NULL != strstr(cur,"bssid"))
	{
		return;
	}
	strtok_r(cur, "\t", &next);
	if (cur != next)
	{
		if(NULL != next)
			*(next - 1) = '\0';
	}
	//printf("buf bss == %s \n",cur);
	strncpy(bss, cur, sizeof(bss) - 1);
	// mac must has :
	if (NULL == strchr(bss, ':'))
	{
		return;
	}
	// freq
	cur = next;
	strtok_r(cur, "\t", &next);
	if (cur != next)
	{
		if(NULL != next)
			*(next - 1) = '\0';
	}
	//printf("buf freq == %s \n",cur);
	strncpy(freq, cur, sizeof(freq) - 1);
	// signal
	cur = next;
	strtok_r(cur, "\t", &next);
	if (cur != next)
	{
		if(NULL != next)
			*(next - 1) = '\0';
	}
	//printf("buf signal == %s \n",cur);
	strncpy(sig, cur, sizeof(sig));
	signal_level = strtol(sig, NULL, 0);
	// impossible, not to here
	if (signal_level >= 0)
	{
		signal_level = 0;
	}
	// >= -30dbm max strength, stand by the route
	// >= -50dBm excellent signal strength
	else if (signal_level >= -50)
	{
		signal_level = 100;
	}
	// >= -60dBm still good, reliable signal strength
	else if (signal_level >= -60)
	{
		signal_level = 80;
	}
	// >= -67dBm minimum value for all services that require smooth and reliable data traffic.
	else if (signal_level >= -67)
	{
		signal_level = 60;
	}
	// The signal is not very strong, but mostly sufficient.
	else if (signal_level >= -70)
	{
		signal_level = 40;
	}
	// Minimum value required to make a connection. 
	// You cannot count on a reliable connection or sufficient signal strength to use services at this level
	else if (signal_level >= -80)
	{
		signal_level = 20;
	}
	//It is very unlikely that you will be able to connect or make use of any services with this signal strength.
	else
	{
		signal_level = 0;
	}
	// flag
	cur = next;
	strtok_r(cur, "\t", &next);
	if (cur != next)
	{
		if(NULL != next)
			*(next - 1) = '\0';
	}
	//printf("buf flag == %s \n",cur);
	strncpy(flag, cur, sizeof(flag) - 1);

	// ssid
	cur = next;
	strtok_r(cur, "\t", &next);
	//printf("111buf ssid == %s \n",cur);
	if(NULL != next)
	{
		if ('\t' == *(next-1)) 
		{
			*(next - 1) = '\0';
		}
		else
		{
			*next = '\0';
		}
	}
	// printf("222buf ssid == %s \n",cur);
	strncpy(ssid, cur, sizeof(ssid) - 1);

	if ('\0' == ssid[0])
	{
		pApInfo->szSSID[0] = '\0';
		return;
	}
	pApInfo->nWirelessMode = 3;
	pApInfo->nQuality = signal_level;
	strncpy(pApInfo->szSSID, ssid, sizeof(pApInfo->szSSID) - 1);
	strncpy(pApInfo->szMacAddress, bss, sizeof(pApInfo->szMacAddress) - 1);
	if (strstr(flag, "WPA2-PSK"))
	{
		pApInfo->nAuthentication = AM_WPA2PSK;
		if (strstr(flag, "TKIP"))
		{
			pApInfo->nEncrypType = ET_TKIP;
		}
		else
		{
			pApInfo->nEncrypType = ET_AES;
		}
	}
	else if (strstr(flag, "WPA-PSK"))
	{
		pApInfo->nAuthentication = AM_WPAPSK;
		if (strstr(flag, "TKIP"))
		{
			pApInfo->nEncrypType = ET_TKIP;
		}
		else
		{
			pApInfo->nEncrypType = ET_AES;
		}
	}
	else if (strstr(flag, "WEP"))
	{
		pApInfo->nAuthentication = AM_SHARED;
		pApInfo->nEncrypType = ET_WEP;
	}
	else
	{
		pApInfo->nAuthentication = AM_OPEN;
		pApInfo->nEncrypType = ET_NONE;
	}
}

int wifi_get_accesspoints_wpa(WLAN_ACCESSPOINT aps[WLAN_MAX_AP_NUM], int *count)
{
	int tryCount = 3;
	WLAN_ACCESSPOINT tmp_aps[128] = {0};
	WLAN_ACCESSPOINT aps_local;
	WLAN_ACCESSPOINT aps_cn;
	char buf[256] = {0};
//	char bss[24];
//	char freq[16];
//	char sig[16];
//	char flag[32];
//	char ssid[256];
//	char *cur = NULL;
	int id = 0;
	int i = 0;
	FILE *fp = NULL;	
	int iRet = 0;
	int iCnLocal = 0;


	if (NULL != aps)
	{
		memset(aps, 0, sizeof(WLAN_ACCESSPOINT) * WLAN_MAX_AP_NUM);
	}
	if (NULL != count)
	{
		*count = 0;
	}

	pthread_mutex_lock(&m_getAccessPointsLock);

	/* 该命令会影响WIFI性能，不能频繁执行，否则会造成网络ping包很大或者超时 */
	memset(&aps_local, 0, sizeof(aps_local));
	memset(&aps_cn, 0, sizeof(aps_cn));
	char tempfile[256] = {0};
	char buffer[512] = {0};

	struct timeval now;

	while (tryCount--)
	{
		gettimeofday(&now, NULL);
	#ifdef __MUSL__
		snprintf(tempfile, 255, "/var/run/aps.%lld", now.tv_usec);
	#else
		snprintf(tempfile, 255, "/var/run/aps.%ld", now.tv_usec);
	#endif
		snprintf(buffer, sizeof(buffer), "/usr/sbin/wpa_cli -iwlan0 scan");
		mysystem(buffer);
		snprintf(buffer, sizeof(buffer), "/usr/sbin/wpa_cli -iwlan0 scan_results > %s", tempfile);
		mysystem(buffer);
		fp = fopen(tempfile, "r+");
		// command failed, sleep and retry
		if (fp == NULL)
		{
			perror("fopen fail");
			sleep(1);
			continue;
		}
		
		fseek(fp, 0, SEEK_SET);
		while (!feof(fp))
		{
			if (NULL == fgets(buf, sizeof(buf), fp))
				continue;
			if (('\0' == buf[0]) || ('\r' == buf[0]) || ('\n' == buf[0]))
				continue;
			if (('\r' == buf[strlen(buf) - 1]) || ('\n' == buf[strlen(buf) - 1]))
				buf[strlen(buf) - 1] = '\0';

			memset(&aps_local, 0, sizeof(aps_local));
			get_ap_info_from_wpa_results(buf, &aps_local);
            if ('\0' == aps_local.szSSID[0] //空ssid
                    || !strncmp(aps_local.szSSID, "ZWAP_", 5) //热点ZWAP_
                    || 32 == aps_local.szSSID[0]//空格
                    || (92 == aps_local.szSSID[0] && 120 == aps_local.szSSID[1] && 48 == aps_local.szSSID[2] && 48 == aps_local.szSSID[3]))//Ssid = \x00\x00\x00
			{
				continue;
			}

			iRet = wifi_cn_ssid_encchange(&aps_local);
			if (0 == iRet)
			{
                iCnLocal = id;
			}

			tmp_aps[id] = aps_local;
			id++;
		}
		// scan failed, sleep and retry
		if (0 == id)
		{
			fclose(fp);
			unlink(tempfile);
			sleep(1);
			continue;
		}

		for (i = 0; i < id; i++)
		{
#if 0		
			printf("ssid:%s\n", tmp_aps[i].szSSID);
			printf("mac:%s\n", tmp_aps[i].szMacAddress);
			printf("quality:%d\n", tmp_aps[i].nQuality);
			printf("auth:%d\n", tmp_aps[i].nAuthentication);
			printf("type:%d\n", tmp_aps[i].nEncrypType);
			printf("\n");
#endif			
		}

        fclose(fp);
		unlink(tempfile);
		break;
	}
	aps_cn = tmp_aps[iCnLocal];

	/* 将热点信息按照信号强度进行快排 */
	qsort(tmp_aps, i, sizeof(WLAN_ACCESSPOINT), accessPointsCmp);

	*count = i > WLAN_MAX_AP_NUM ? WLAN_MAX_AP_NUM : i;

	/* 取其最大前WLAN_MAX_AP_NUM的热点信息 */
	for (i = 0; i < *count; i++)
	{
		if (0 == strcasecmp(tmp_aps[i].szSSID, wifi_connection.szSSID))
		{
			WLAN_ACCESSPOINT temp;
			memset(&temp, 0, sizeof(WLAN_ACCESSPOINT));
			/* 如果当前设置的wifi热点在前WLAN_MAX_AP_NUM里面，则需要将其置顶 */
			memcpy(&temp, &tmp_aps[0], sizeof(WLAN_ACCESSPOINT));
			memcpy(&tmp_aps[0], &tmp_aps[i], sizeof(WLAN_ACCESSPOINT));
			memcpy(&tmp_aps[i], &temp, sizeof(WLAN_ACCESSPOINT));
			break;
		}
	}

	memcpy(aps, tmp_aps, *count * sizeof(WLAN_ACCESSPOINT));
	//更新wifi扫描cache
	unlink(WIFI_SCAN_RESULT_CACHE_PATH);
	s_nWifiScanResultCntCache = *count;
	memset(s_stWifiScanResultCache, 0, sizeof(s_stWifiScanResultCache));
	memcpy(s_stWifiScanResultCache, tmp_aps, *count * sizeof(WLAN_ACCESSPOINT));
	pthread_mutex_unlock(&m_getAccessPointsLock);

#if 0 /* 调试用，保留上库 */
    if( 0 != iCnLocal)
	    iRet = wifi_cn_connect_test(&aps_cn);
#endif

	return 0;
}


int wifi_get_signal_info_wpa(LPWLAN_ACCESSSIGNALINFO signal_info)
{
    int tryCount = 3;
    char buf[256] = {0};
    int idx = 0;
    int rssi = 0;
    int signal_level = 0;

    char *rssi_line = NULL;
    int rssi_found = 0;

    /* 该命令会影响WIFI性能，不能频繁执行，否则会造成网络ping包很大或者超时 */
    char tempfile[256] = {0};
    char buffer[512] = {0};
    char rssi_value[256] = {0};
//    char *getssid = " wpa_cli status | grep ssid | grep -v bssid | awk -F \"=\" '{print $2}'";
    FILE *fp = NULL;
       
    if (!wifi_hasdev)
    {
       return -1;
    }

    /* 1. 获取当前wifi连接状态，只获取连接上的ssid的信号强度 */
    if (IsStringInCommand("wpa_cli status", "COMPLETED") != 1)
    {
       WIFI_ERR("Wifi is disconnect!\n");
       return -1;
    }

    /* 2. 获取当前wifi连接的ssid: wpa_cli status | grep ssid | grep -v bssid | awk -F "=" '{print $2}' */
	/*
	if (wifi_if_get_ssid == 0)
    {
       memset(wifi_curr_ssid, 0, sizeof(wifi_curr_ssid));
       ShellCmdOut(getssid, wifi_curr_ssid, sizeof(wifi_curr_ssid));
       wifi_curr_ssid[strlen(wifi_curr_ssid)-1] = '\0';
       wifi_if_get_ssid = 1;
    }
*/
    struct timeval now;

    while (tryCount--)
    {
        gettimeofday(&now, NULL);
    #ifdef __MUSL__
        snprintf(tempfile, 255, "/var/run/wpabss.%lld", now.tv_usec);
    #else
        snprintf(tempfile, 255, "/var/run/wpabss.%ld", now.tv_usec);
    #endif
        //snprintf(buffer, sizeof(buffer), "/usr/sbin/wpa_cli -iwlan0 signal_poll > %s", tempfile);
        snprintf(buffer, sizeof(buffer), "/usr/sbin/wpa_cli bss %s > %s", wifi_curr_ssid, tempfile);
        mysystem(buffer);
        fp = fopen(tempfile, "r+");
        // command failed, sleep and retry
        if (fp == NULL)
        {
            perror("fopen fail");
            sleep(1);
            continue;
        }

        fseek(fp, 0, SEEK_SET);
        while (!feof(fp))
        {
            if (NULL == fgets(buf, sizeof(buf), fp))
                continue;
            if (('\0' == buf[0]) || ('\r' == buf[0]) || ('\n' == buf[0]))
                continue;
            if (('\r' == buf[strlen(buf) - 1]) || ('\n' == buf[strlen(buf) - 1]))
                buf[strlen(buf) - 1] = '\0';

            // 检查当前行是否包含"RSSI="
            if (strstr(buf, "level=")) 
            {
                rssi_line = buf;
                rssi_found = 1;
                idx++;
                break;
            }
        }

        // scan failed, sleep and retry
        if (0 == idx)
        {
            fclose(fp);
            unlink(tempfile);
            sleep(1);
            continue;
        }
        
        // 如果找到了RSSI行，则提取RSSI值
        if (rssi_found) 
        {
           //RSSI值紧跟在等号后面，我们找到等号并复制其后的内容
           char *equals_sign = strstr(rssi_line, "=");
           if (equals_sign) 
           {
               strncpy(rssi_value, equals_sign + 1, sizeof(rssi_value) - 1);
               // 移除RSSI值后的换行符和空格（如果有）
               rssi_value[strcspn(rssi_value, "\n\r ")] = '\0';
               rssi = atoi(rssi_value);
               // 打印RSSI值
               printf("[%s:%d] RSSIValue: %s, RSSI:%d\n", __FILE__, __LINE__, rssi_value, rssi);
               
           } 
           else 
           {
               printf("[%s:%d] Error parsing RSSI value.\n", __FILE__, __LINE__);
               rssi = -50;
           }
        } 

        if (rssi >= 0)
        {
            signal_level = 0;
        }
        // >= -30dbm max strength, stand by the route
        // >= -50dBm excellent signal strength
        else if (rssi >= -50)
        {
            signal_level = 100;
        }
        // >= -60dBm still good, reliable signal strength
        else if (rssi >= -60)
        {
            signal_level = 80;
        }
        // >= -67dBm minimum value for all services that require smooth and reliable data traffic.
        else if (rssi >= -67)
        {
            signal_level = 60;
        }
        // The signal is not very strong, but mostly sufficient.
        else if (rssi >= -70)
        {
            signal_level = 40;
        }
        // Minimum value required to make a connection. 
        // You cannot count on a reliable connection or sufficient signal strength to use services at this level
        else if (rssi >= -80)
        {
            signal_level = 20;
        }
        //It is very unlikely that you will be able to connect or make use of any services with this signal strength.
        else
        {
            signal_level = 0;
        }

        printf("[%s：%d] ssid:%s, quality:%d\n", __FILE__, __LINE__, wifi_curr_ssid, signal_level);
        
        fclose(fp);
        unlink(tempfile);
        break;
    }

    signal_info->nQuality = signal_level;
    strncpy(signal_info->szSSID, wifi_curr_ssid, sizeof(wifi_curr_ssid) - 1);
    return 0;
}

int wifi_get_signal_quality(int *nQuality)
{
    char cmd[128] = {0};
    snprintf(cmd, sizeof(cmd), "wpa_cli -i wlan0 signal_poll 2>/dev/null | grep -E '^(RSSI|NOISE)=' | awk -F= '{print $2}'");

    FILE *fp = popen(cmd, "r");
    if (!fp) {
        printf("popen failed!\n");
        return -1;
    }

    char buf1[64] = {0};
    char buf2[64] = {0};

    // 读第一行 (RSSI)
    if (fgets(buf1, sizeof(buf1), fp) == NULL) {
        pclose(fp);
        return -1;
    }
    // 读第二行 (NOISE)
    if (fgets(buf2, sizeof(buf2), fp) == NULL) {
        pclose(fp);
        return -1;
    }
    pclose(fp);

    int rssi, noise;
    if (sscanf(buf1, "%d", &rssi) != 1 || sscanf(buf2, "%d", &noise) != 1) {        
        printf("wifi_get_signal_quality -- buf1[%s] buf2[%s]\n", buf1, buf2);
        return -1;
    }

    int snr = rssi - noise;               // 信噪比 (dB)
    if (snr <= 0) {
        *nQuality = 0;
    } else if (snr >= 30) {
        *nQuality = 100;
    } else {
        *nQuality = ((snr * 100) / 30); // 线性映射
    }

    printf("wifi_get_signal_quality -- nQuality[%d] rssi[%d] noise[%d]\n", *nQuality, rssi, noise);
    return 0;
}


#if 0
int wifi_get_accesspoints(WLAN_ACCESSPOINT aps[WLAN_MAX_AP_NUM], int *count)
{   
	int tryCount = 3;
    dictionary *ini = NULL;

    if (NULL != aps)
    {
        memset(aps, 0, sizeof(aps));
    }
    if (NULL != count)
    {
        *count = 0;
    }

    pthread_mutex_lock(&m_getAccessPointsLock);

    /* 该命令会影响WIFI性能，不能频繁执行，否则会造成网络ping包很大或者超时 */
    memset(aps, 0, sizeof(aps));
    char tempfile[256] = {0};
    char buffer[256] = {0};

    struct timeval now;

	while(tryCount--)
	{
	    gettimeofday(&now, NULL);
	    snprintf(tempfile, 255, "/var/run/aps.%ld", now.tv_usec);
	    snprintf(buffer, 255, "/usr/sbin/iwlist wlan0 scan > %s", tempfile);
	    mysystem(buffer);

		Wifi_ClearUnusedRowForWIFI (tempfile);

	    ini = iniparser_load(tempfile); // 不需要额外打开文件
	    if (NULL == ini)
	    {
	        printf("\n.......fopen  tempfile...faild!!!!!!......\n");//有时出现搜索不到热点再刷一次就好了就是进到这一步了
	        if(0 == tryCount)
	        {
		        pthread_mutex_unlock(&m_getAccessPointsLock);
		        unlink(tempfile);
		        return -1;
			}
			usleep(100000);
            unlink(tempfile);
	        continue;
	    }
	    else
	    {
            break;
	    }
	}

    WLAN_ACCESSPOINT tmp_aps[128] = {0};

    int i = 0;
    int j = 0;
	int m = 0;
    int b = 0;
    int key_num = 0;
    int accesspoints_num = 0;

    char *s = NULL;
    char *k = NULL;
    char buf[128] = {0};
    char mac[32] = {0};
    char ssid[WLAN_MAX_SSID_LEN] = {0};

    char EncryEnable[4] = {0};
    char IE[2][16] = {0};
    char GroupCipher[2][16] = {0};
    char PairwiseCiphers[2][16] = {0};
    char AuthenticationSuites[2][16] = {0};
    SSID_ENCODE ssid_encode = UTF8_ENCODE;

    /* 获取热点总数 */
    accesspoints_num = iniparser_getnsec(ini);    
    accesspoints_num = accesspoints_num > 128 ? 128 : accesspoints_num;
    
    printf("accesspoints num is %d\n", accesspoints_num);
 
    for (i=0; i<accesspoints_num; i++)
    {
        /* 逐个热点获取名称 */
        s = iniparser_getsecname(ini, i);
        
        snprintf(buf, sizeof(buf), "%s:ESSID", s);
        k = iniparser_getstring(ini, buf, "error");

        if(wifi_check_ssid_encode(k,&ssid_encode))
        {
            b ++;
            continue;
        }
        /* 不支持ESSID含有gbk编码的中文热点和为空的热点 */
        if ((GBK_ENCODE == ssid_encode) || !strcmp(k, "[]"))
        {
            b ++;
            continue;
        }
        
        /* 格式化MAC地址并填充到结构体里面 */
        MAC_FORMAT_TO_STRING(s, mac, sizeof(mac));
        strncpy(tmp_aps[i-b].szMacAddress, mac, sizeof(tmp_aps[i-b].szMacAddress));

        /* 逐个热点信息分析 */
        key_num = iniparser_getsecnkeys(ini, s);
        
        for (j=0; j<key_num && j < ARRAY_SIZEOF(accesspoints_info_8189); j++)
        {
            memset(buf, 0, sizeof(buf));
#ifdef WIFI_USE_8189
			for (m=0; m<TEST_GET_WIFI_INFO_MAX; m++)
			{
				if (j>=3)
				{
					snprintf(buf, sizeof(buf), "%s:%s_%d", s, accesspoints_info_8189[j], m);
				}
				else
				{
					snprintf(buf, sizeof(buf), "%s:%s", s, accesspoints_info_8189[j]);
				}
#else
			{
				snprintf(buf, sizeof(buf), "%s:%s", s, accesspoints_info[j]);
#endif
            //snprintf(buf, sizeof(buf), "%s:%s", s, accesspoints_info[j]);
	            if (iniparser_find_entry(ini, buf))
	            {
	                k = iniparser_getstring(ini, buf, "test");
					printf("=-=-=-=-=-=-=iniparser_getstring  k=%s, i=%d!\n", k, j);
	                switch(j)
	                {
	                    case ESSID:
	                    {
	                        memset(ssid, 0, sizeof(ssid));
	                        if (0 == StripString(k, '[', ']', ssid))
	                        {
	                            strncpy(tmp_aps[i-b].szSSID, ssid, sizeof(tmp_aps[i-b].szSSID));
	                        }
	                        break;
	                    }
#ifdef WIFI_USE_8189
	                    case QUALITY:                tmp_aps[i-b].nQuality = (atoi(k)*100)/70;    break;
#else
						case QUALITY:                tmp_aps[i-b].nQuality = atoi(k);    break;
#endif
	                    case ENCRYPTIONKEY:          strncpy(EncryEnable, k, sizeof(EncryEnable) - 1);             break;
	                    case IE_0:                   strncpy(IE[0], k, sizeof(IE[0]) - 1);                   break;
	                    case GROUPCIPHER_0:          strncpy(GroupCipher[0], k, sizeof(GroupCipher[0]) - 1);          break;
	                    case PAIRWISECIPHERS_0:      strncpy(PairwiseCiphers[0], k, sizeof(PairwiseCiphers[0]) - 1);      break;
	                    case AUTHENTICATIONSUITES_0: strncpy(AuthenticationSuites[0], k, sizeof(AuthenticationSuites[0]) - 1); break;
	                    case IE_1:                   strncpy(IE[1], k, sizeof(IE[1]) - 1);                   break;
	                    case GROUPCIPHER_1:          strncpy(GroupCipher[1], k, sizeof(GroupCipher[1]) - 1);          break;
	                    case PAIRWISECIPHERS_1:      strncpy(PairwiseCiphers[1], k, sizeof(PairwiseCiphers[1]) - 1);      break;
	                    case AUTHENTICATIONSUITES_1: strncpy(AuthenticationSuites[1], k, sizeof(AuthenticationSuites[1]) - 1); break;
	                    default:
	                    {
	                        break;
	                    }
	                }
#ifdef WIFI_USE_8189
					break;
#endif
	            }
            }
            if (0 == strcasecmp("off", EncryEnable))
            {
                tmp_aps[i-b].nAuthentication = AM_OPEN;
                tmp_aps[i-b].nEncrypType = ET_NONE;
            }
            else
            {
                if (strcasestr(IE[0], "WPA") || strcasestr(IE[0], "WPA2"))
                {
                    tmp_aps[i-b].nAuthentication = AM_WPAPSK;
                }
                else
                {
                    tmp_aps[i-b].nAuthentication = AM_SHARED;
                }
                
                if (strcasestr(GroupCipher[0], "CCMP"))
                {
                    tmp_aps[i-b].nEncrypType = ET_AES;
                }
                else
                {
                    if (AM_SHARED == tmp_aps[i-b].nAuthentication)
                    {
                        
                        tmp_aps[i-b].nEncrypType = ET_WEP;
                    }
                    else
                    {
                        tmp_aps[i-b].nEncrypType = ET_TKIP;
                    }                    
                }
            }
        }
    }
    
    iniparser_freedict(ini);

    unlink(tempfile);

    /* 将热点信息按照信号强度进行快排 */
    qsort(tmp_aps, i-b, sizeof(WLAN_ACCESSPOINT), accessPointsCmp);

    *count = i-b > WLAN_MAX_AP_NUM ? WLAN_MAX_AP_NUM : i-b;

    /* 取其最大前WLAN_MAX_AP_NUM的热点信息 */
    
    for (i=0; i<*count; i++)
    {
		if(0 == strcasecmp(tmp_aps[i].szSSID, wifi_connection.szSSID))
		{
			WLAN_ACCESSPOINT temp;
			memset(&temp, 0, sizeof(WLAN_ACCESSPOINT));
			/* 如果当前设置的wifi热点在前WLAN_MAX_AP_NUM里面，则需要将其置顶 */
			memcpy(&temp, &tmp_aps[0], sizeof(WLAN_ACCESSPOINT));
			memcpy(&tmp_aps[0], &tmp_aps[i], sizeof(WLAN_ACCESSPOINT));
			memcpy(&tmp_aps[i], &temp, sizeof(WLAN_ACCESSPOINT));
            break;
		}
    }

    memcpy(aps, tmp_aps, *count*sizeof(WLAN_ACCESSPOINT));  

    //更新wifi扫描cache
    unlink(WIFI_SCAN_RESULT_CACHE_PATH);
    s_nWifiScanResultCntCache = *count;
    memset(s_stWifiScanResultCache, 0, sizeof(s_stWifiScanResultCache));
    memcpy(s_stWifiScanResultCache, tmp_aps, *count*sizeof(WLAN_ACCESSPOINT));

    pthread_mutex_unlock(&m_getAccessPointsLock);

    return 0;
}
#else
int wifi_get_accesspoints(WLAN_ACCESSPOINT aps[WLAN_MAX_AP_NUM], int *count)
{
    return wifi_get_accesspoints_wpa(aps, count);
}
#endif

//!\brief 获取周围的热点信息(仅供测试wifi是否正常用)
//!\param aps 热点数组
//!\param count 热点个数
//!\return 0表示获取成功,其他表示错误码 
int wifi_get_accesspoints_for_test(WLAN_ACCESSPOINT aps[WLAN_MAX_AP_NUM], int *count)
{   
    dictionary *ini = NULL;

    if (NULL != aps)
    {
        memset(aps, 0, sizeof(WLAN_ACCESSPOINT) * WLAN_MAX_AP_NUM);
    }
    if (NULL != count)
    {
        *count = 0;
    }

	Wifi_ClearUnusedRowForWIFI (WIFI_SCAN_RESULT_CACHE_PATH);
            
    ini = iniparser_load(WIFI_SCAN_RESULT_CACHE_PATH);
    if (NULL != ini)
    {
        //从wifi扫描cache文件中读取
        WLAN_ACCESSPOINT tmp_aps[128] = {0};
        int i = 0;
        int j = 0;
        int m = 0;
        int b = 0;
        int key_num = 0;
        int accesspoints_num = 0;

        const char *s = NULL;
        const char *k = NULL;
        char buf[128] = {0};
        char mac[32] = {0};
        char ssid[WLAN_MAX_SSID_LEN] = {0};

        char EncryEnable[4] = {0};
        char IE[2][16] = {0};
        char GroupCipher[2][16] = {0};
        char PairwiseCiphers[2][16] = {0};
        char AuthenticationSuites[2][16] = {0};
        SSID_ENCODE ssid_encode = UTF8_ENCODE;

        /* 获取热点总数 */
        accesspoints_num = iniparser_getnsec(ini);    
        accesspoints_num = accesspoints_num > 128 ? 128 : accesspoints_num;
        
        printf("accesspoints num is %d\n", accesspoints_num);
     
        for (i = 0; i < accesspoints_num; i++)
        {
            /* 逐个热点获取名称 */
            s = iniparser_getsecname(ini, i);
            
            snprintf(buf, sizeof(buf), "%s:ESSID", s);
            k = iniparser_getstring(ini, buf, "error");

            if(wifi_check_ssid_encode(k,&ssid_encode))
            {
                b ++;
                continue;
            }
            /* 不支持ESSID含有gbk编码的中文热点和为空的热点 */
            if ((GBK_ENCODE == ssid_encode) || !strcmp(k, "[]"))
            {
                b ++;
                continue;
            }
            
            /* 格式化MAC地址并填充到结构体里面 */
            MAC_FORMAT_TO_STRING(s, mac, sizeof(mac));
            strncpy(tmp_aps[i-b].szMacAddress, mac, sizeof(tmp_aps[i-b].szMacAddress) - 1);

            /* 逐个热点信息分析 */
            key_num = iniparser_getsecnkeys(ini, s);
            
            for (j=0; j<key_num && j < sizeof(accesspoints_info_8189) / sizeof(accesspoints_info_8189[0]); j++)
            {
                memset(buf, 0, sizeof(buf));
#ifdef WIFI_USE_8189
                for (m=0; m<TEST_GET_WIFI_INFO_MAX; m++)
                {
                    if (m>3)
                    {
                        snprintf(buf, sizeof(buf), "%s:%s_%d", s, accesspoints_info_8189[j], m);
                    }
                    else
                    {
                        snprintf(buf, sizeof(buf), "%s:%s", s, accesspoints_info_8189[j]);
                    }
#else
                {
                    snprintf(buf, sizeof(buf), "%s:%s", s, accesspoints_info[j]);
#endif
                //snprintf(buf, sizeof(buf), "%s:%s", s, accesspoints_info[j]);
                    if (iniparser_find_entry(ini, buf))
                    {
                        k = iniparser_getstring(ini, buf, "test");
                        printf("=-=-=-=-=-=-=iniparser_getstring  k=%s, i=%d!\n", k, j);
                        switch(j)
                        {
                            case ESSID:
                            {
                                memset(ssid, 0, sizeof(ssid));
                                if (0 == StripString(k, '[', ']', ssid))
                                {
                                    strncpy(tmp_aps[i-b].szSSID, ssid, sizeof(tmp_aps[i-b].szSSID) - 1);
                                }
                                break;
                            }
#ifdef WIFI_USE_8189
                            case QUALITY:                tmp_aps[i-b].nQuality = (atoi(k)*100)/70;    break;
#else
                            case QUALITY:                tmp_aps[i-b].nQuality = atoi(k);    break;
#endif
                            case ENCRYPTIONKEY:          strncpy(EncryEnable, k, sizeof(EncryEnable) - 1);             break;
                            case IE_0:                   strncpy(IE[0], k, sizeof(IE[0]) - 1);                   break;
                            case GROUPCIPHER_0:          strncpy(GroupCipher[0], k, sizeof(GroupCipher[0]) - 1);          break;
                            case PAIRWISECIPHERS_0:      strncpy(PairwiseCiphers[0], k, sizeof(PairwiseCiphers[0]) - 1);      break;
                            case AUTHENTICATIONSUITES_0: strncpy(AuthenticationSuites[0], k, sizeof(AuthenticationSuites[0]) - 1); break;
                            case IE_1:                   strncpy(IE[1], k, sizeof(IE[1]) - 1);                   break;
                            case GROUPCIPHER_1:          strncpy(GroupCipher[1], k, sizeof(GroupCipher[1]) - 1);          break;
                            case PAIRWISECIPHERS_1:      strncpy(PairwiseCiphers[1], k, sizeof(PairwiseCiphers[1]) - 1);      break;
                            case AUTHENTICATIONSUITES_1: strncpy(AuthenticationSuites[1], k, sizeof(AuthenticationSuites[1]) - 1); break;
                            default:
                            {
                                break;
                            }
                        }
#ifdef WIFI_USE_8189
                        break;
#endif
                    }
                }
                if (0 == strcasecmp("off", EncryEnable))
                {
                    tmp_aps[i-b].nAuthentication = AM_OPEN;
                    tmp_aps[i-b].nEncrypType = ET_NONE;
                }
                else
                {
                    if (strcasestr(IE[0], "WPA") || strcasestr(IE[0], "WPA2"))
                    {
                        tmp_aps[i-b].nAuthentication = AM_WPAPSK;
                    }
                    else
                    {
                        tmp_aps[i-b].nAuthentication = AM_SHARED;
                    }
                    
                    if (strcasestr(GroupCipher[0], "CCMP"))
                    {
                        tmp_aps[i-b].nEncrypType = ET_AES;
                    }
                    else
                    {
                        if (AM_SHARED == tmp_aps[i-b].nAuthentication)
                        {
                            
                            tmp_aps[i-b].nEncrypType = ET_WEP;
                        }
                        else
                        {
                            tmp_aps[i-b].nEncrypType = ET_TKIP;
                        }                    
                    }
                }
            }
        }
        
        iniparser_freedict(ini);

        unlink(WIFI_SCAN_RESULT_CACHE_PATH);

        /* 将热点信息按照信号强度进行快排 */
        qsort(tmp_aps, i-b, sizeof(WLAN_ACCESSPOINT), accessPointsCmp);

        *count = i-b > WLAN_MAX_AP_NUM ? WLAN_MAX_AP_NUM : i-b;

        /* 取其最大前WLAN_MAX_AP_NUM的热点信息 */
        for (i=0; i<*count; i++)
        {
            if (0 == strcasecmp(tmp_aps[i].szSSID, wifi_connection.szSSID))
            {
                WLAN_ACCESSPOINT temp;
                memset(&temp, 0, sizeof(WLAN_ACCESSPOINT));
                /* 如果当前设置的wifi热点在前WLAN_MAX_AP_NUM里面，则需要将其置顶 */
                memcpy(&temp, &tmp_aps[0], sizeof(WLAN_ACCESSPOINT));
                memcpy(&tmp_aps[0], &tmp_aps[i], sizeof(WLAN_ACCESSPOINT));
                memcpy(&tmp_aps[i], &temp, sizeof(WLAN_ACCESSPOINT));
                break;
            }
        }
        
        memcpy(aps, tmp_aps, *count*sizeof(WLAN_ACCESSPOINT));

        s_nWifiScanResultCntCache = *count;
        memset(s_stWifiScanResultCache, 0, sizeof(s_stWifiScanResultCache));
        memcpy(s_stWifiScanResultCache, tmp_aps, *count*sizeof(WLAN_ACCESSPOINT));
    }
    else
    {
        if (0 == strlen(s_stWifiScanResultCache[0].szSSID))
        {
            //本次启动未扫描过，需扫描一次
            return wifi_get_accesspoints(aps, count);
        }
        else
        {
            //从wifi扫描cache返回结果
            *count = s_nWifiScanResultCntCache;
            memcpy(aps, s_stWifiScanResultCache, s_nWifiScanResultCntCache*sizeof(WLAN_ACCESSPOINT));
        }
    }
    
    return 0;
}

int wifi_get_conn_cfg(LPWLAN_CONNECTION conn)
{
	//WIFI_DBG("Enter wifi_get_conn_cfg\n");
	memset(conn, 0, sizeof(WLAN_CONNECTION));
	memcpy(conn, &wifi_connection, sizeof(WLAN_CONNECTION));

    /* 上层IE进行一个转换 */
    if (conn->nNetworkType == 2)
    {
        conn->nNetworkType = 0;
    }else{
        conn->nNetworkType = 1;
    }
    
    WIFI_DBG("conn->nEncrypType = %d\n", conn->nEncrypType);

#if 0
    conn->struKeyCfg.bValid = 1;
    conn->struKeyCfg.Key1.bValid = 1;
    conn->struKeyCfg.Key2.bValid = 1;
    conn->struKeyCfg.Key3.bValid = 1;
    conn->struKeyCfg.Key4.bValid = 1;
    conn->struKeyCfg.WPAKey.bValid = 1;
#endif
    conn->struKeyCfg.bValid = 1;
    conn->struKeyCfg.WPAKey.bValid = 1;

    #if 0
	wifi_get_conn_status(&conn->bConnect, szSSID, szMacAddress);
	#else
	//该函数供协议调用的，为了防止频繁调用，所以获取连接状态的历史值
	wifi_get_curr_statusEx(&conn->bConnect);
	#endif
	
	return 0;
}

#define GET_WIFI_LIST_CNT_MAX (WLAN_MAX_AP_NUM)
int wifi_get_conn_info_wpa(int from_ssid, const char *data, LPWLAN_CONNECTION conn)
{
	if (!data || !conn)
	{
		printf("NULL POINTER EXCEPTION!\n");
		return -1;
	}

	WLAN_ACCESSPOINT ap;
	//初始化默认值
	conn->nNetworkType = 2;
	conn->nAuthMode = 3;
	conn->nEncrypType = 2;

	// 空密码时默认的认证模式和加密算法进行适配
	if (0 == strlen(conn->struKeyCfg.WPAKey.szKey))
	{
		conn->nAuthMode = 0;
		conn->nEncrypType = 0;
	}

	char command[512] = {0};
	char tempfile[256] = {0};
	struct timeval now;

	char buf[128] = {0};
	char mac[32] = {0};

	pthread_mutex_lock(&m_getAccessPointsLock);

	gettimeofday(&now, NULL);
	#ifdef __MUSL__
		snprintf(tempfile, 255, "/var/run/aps.%lld", now.tv_usec);
	#else
		snprintf(tempfile, 255, "/var/run/aps.%ld", now.tv_usec);
	#endif
	if (from_ssid)
	{
		snprintf(command, sizeof(command),
				 "wpa_cli -iwlan0 scan; wpa_cli -iwlan0 scan_results | grep \"\\\\<%s\\\\>\" > %s",
				 data, tempfile);
	}
	else
	{
		MAC_FORMAT_TO_STRING(data, mac, sizeof(mac) - 1);
		snprintf(command, sizeof(command),
				 "wpa_cli -iwlan0 scan; wpa_cli -iwlan0 scan_results | grep -i \"^%s\" > %s",
				 mac, tempfile);
	}
	printf(command);
	mysystem(command);

	int nTryCnt = 0;
	struct stat stat_info = {0};
	//等待结果生成
	while (1)
	{
		usleep(10000);
		stat(tempfile, &stat_info);
		if (stat_info.st_size <= 0)
		{
			nTryCnt++;
			if (nTryCnt >= GET_WIFI_LIST_CNT_MAX)
			{
				WIFI_ERR("......%s not exist......\n", tempfile);
				unlink(tempfile);
				pthread_mutex_unlock(&m_getAccessPointsLock);
				return -1;
			}
		}
		else
		{
			break;
		}
	}

	FILE *fp = fopen(tempfile, "r+");
	if (fp == NULL)
	{
		perror("fopen fail");
		pthread_mutex_unlock(&m_getAccessPointsLock);
		return -1;
	}

	fseek(fp, 0, SEEK_SET);
	while (!feof(fp))
	{
		if (NULL == fgets(buf, sizeof(buf), fp))
			continue;
		if (('\0' == buf[0]) || ('\r' == buf[0]) || ('\n' == buf[0]))
			continue;
		if (('\r' == buf[strlen(buf) - 1]) || ('\n' == buf[strlen(buf) - 1]))
			buf[strlen(buf) - 1] = '\0';

		memset(&ap, 0, sizeof(ap));
		printf("[%s %d] WIFI wpa_cli get_con_info : %s \n",__FILE__,__LINE__,buf);
		get_ap_info_from_wpa_results(buf, &ap);
		if ('\0' == ap.szSSID[0])
		{
			continue;
		}
		break;
	}
	conn->bConnect = 0;
	strncpy(conn->szSSID, ap.szSSID, sizeof(conn->szSSID));
	strncpy(conn->szMacAddress, ap.szMacAddress, sizeof(conn->szMacAddress));
	conn->nAuthMode = ap.nAuthentication;
	conn->nEncrypType = ap.nEncrypType;
	conn->nNetworkType = 2;
	if ((AM_WPA2PSK == ap.nAuthentication) || (AM_WPAPSK == ap.nAuthentication))
	{
		conn->struKeyCfg.bValid = 1;
		conn->struKeyCfg.WPAKey.bValid = 1;
	}
	else if (ET_WEP == ap.nEncrypType)
	{
		conn->struKeyCfg.bValid = 1;
	}

	fclose(fp);
	unlink(tempfile);

	//为了加快生产测试时，wifi测试的流程
	//这里提前将需要的数据准备好
	//if (0 == strlen(s_stWifiScanResultCache[0].szSSID))
	//{
	//    int nCountTmp = 0;
	//    WLAN_ACCESSPOINT stWifiScanTmp[WLAN_MAX_AP_NUM];
	// 	wifi_get_accesspoints_for_test(1, stWifiScanTmp, &nCountTmp);
	//}

	pthread_mutex_unlock(&m_getAccessPointsLock);

    return 0;
}
int wifi_get_conn_info(int from_ssid, const char *data, LPWLAN_CONNECTION conn)
{
    if (!data || !conn)
    {
        printf("NULL POINTER EXCEPTION!\n");
        return -1;
    }
    
	//初始化默认值    
	conn->nNetworkType = 0;
	conn->nAuthMode = 2;
	conn->nEncrypType = 3;

    char command[512]  = {0};    
	char tempfile[256] = {0};
	struct timeval now;

    pthread_mutex_lock(&m_getAccessPointsLock);

    dictionary *ini = NULL;
    int i = 0;
	int j = 0;
    int key_num = 0;
    const char *s = NULL;
    const char *k = NULL;
    char buf[128] = {0};
    char mac[32] = {0};
    char ssid[WLAN_MAX_SSID_LEN] = {0};

    char EncryEnable[4] = {0};
    char IE[2][16] = {0};
    char GroupCipher[2][16] = {0};
    char PairwiseCiphers[2][16] = {0};
    char AuthenticationSuites[2][16] = {0};
    
    memset(IE, 0, sizeof(IE));
    memset(GroupCipher, 0, sizeof(GroupCipher));
    memset(PairwiseCiphers, 0, sizeof(PairwiseCiphers));
    memset(AuthenticationSuites, 0, sizeof(AuthenticationSuites));

	gettimeofday(&now, NULL);
	#ifdef __MUSL__
		snprintf(tempfile, 255, "/var/run/aps.%lld", now.tv_usec);
	#else
		snprintf(tempfile, 255, "/var/run/aps.%ld", now.tv_usec);
	#endif
    if (from_ssid)
    {
        //这里发现一个问题，开机第一次执行    ,不会有结果输出，第二次就正常
    	/* grep搜索时只会找到包含该字符串的热点，所有当一个热点名    		包含    	   另
    	   一个热点名字时可能出现找错，目前的处理只能尽量的降低这种概率
    	 */
        /*grep搜索时只会找到包含该字符串的热点，所有当一个热点名
                    包含另一个热点名字时可能出现找错，目前的处理只能尽量的
                    降低这种概率*/
#ifdef WIFI_USE_8189
		snprintf(command, sizeof(command), 
			"iwlist wlan0 scan > %s; cat %s | grep -i \"%s=\\[%s\\]\" -B 3 -A 11 -m 3 | grep -B 14 \"^$\" > %s", 
			WIFI_SCAN_RESULT_CACHE_PATH, WIFI_SCAN_RESULT_CACHE_PATH, 
			accesspoints_info[ESSID], data, tempfile);
#else
		snprintf(command, sizeof(command), 
            "iwlist wlan0 scan > %s; cat %s | grep -i \"%s=\\[%s\\]\" -B 1 -A 11 -m 1 | grep -B 12 \"^$\" > %s", 
            WIFI_SCAN_RESULT_CACHE_PATH, WIFI_SCAN_RESULT_CACHE_PATH, 
            accesspoints_info[ESSID], data, tempfile);
#endif
    }
    else
    {
        MAC_FORMAT_TO_STRING(data, mac, sizeof(mac));
        snprintf(command, sizeof(command),
            "iwlist wlan0 scan > %s; cat %s | grep -i \"%s\" -A 12 | grep -B 12 \"^$\" > %s", 
            WIFI_SCAN_RESULT_CACHE_PATH, WIFI_SCAN_RESULT_CACHE_PATH, 
            mac, tempfile);
    }

    mysystem(command);
	
    int nTryCnt = 0;
    struct stat stat_info = {0};
    //等待结果生成
    while (1)
    {
        usleep(10000);
        stat(tempfile, &stat_info);
        if (stat_info.st_size <= 0)
        {
            nTryCnt++;
            if (nTryCnt >= GET_WIFI_LIST_CNT_MAX)
            {
                WIFI_ERR("......%s not exist......\n", tempfile);
                unlink(tempfile);
                pthread_mutex_unlock(&m_getAccessPointsLock);
                return -1;
            }
        }
        else
        {
            break;
        }
    }

	Wifi_ClearUnusedRowForWIFI (tempfile);

    ini = iniparser_load(tempfile); // 不需要额外打开文件
    if (NULL == ini)
    {
        WIFI_ERR(".......load %s...faild!!!!!!......\n", tempfile);
        unlink(tempfile);
        pthread_mutex_unlock(&m_getAccessPointsLock);
        return -1;
    }

	printf("=-=-=-=-=-=-=iniparser_getnsec pre ini->size:%d\n", ini->size);
    for (i=0 ; i<ini->size ; i++)
    {
        if (NULL == ini->key[i])
        {
            continue;
        }
        printf("=-=-=-=-=-=-=iniparser_getnsec ini->key[%d]=%s\n", i, ini->key[i]);
    }

    /* 获取热点总数 */
    if(iniparser_getnsec(ini) >= 1)
    {
    	printf("=-=-=-=-=-=-=iniparser_getnsec after!\n");
        /* 逐个热点获取名称 */
        s = iniparser_getsecname(ini, 0);
        
        snprintf(buf, sizeof(buf), "%s:ESSID", s);
        k = iniparser_getstring(ini, buf, "error");
        
        /* 不支持ESSID为中文和空的热点 */
        if (!IncludeChinese(k) && strncmp(k, "[]", 2))
        {
            /* 格式化MAC地址并填充到结构体里面 */
            STRING_FORMAT_TO_MAC(s, mac, sizeof(mac));
            strncpy(conn->szMacAddress, mac, sizeof(conn->szMacAddress) - 1);

            /* 逐个信息分析 */
            key_num = iniparser_getsecnkeys(ini, s);
            for (i=0; i < key_num && i < sizeof(accesspoints_info_8189) / sizeof(accesspoints_info_8189[0]); i++)
            {
                memset(buf, 0, sizeof(buf));
#ifdef WIFI_USE_8189
				for (j=0; j<TEST_GET_WIFI_INFO_MAX; j++)
				{
					if (i>=3)
					{
						snprintf(buf, sizeof(buf), "%s:%s_%d", s, accesspoints_info_8189[i], j);
					}
					else
					{
						snprintf(buf, sizeof(buf), "%s:%s", s, accesspoints_info_8189[i]);
					}
#else
					{
						snprintf(buf, sizeof(buf), "%s:%s", s, accesspoints_info[i]);
#endif
	                if (iniparser_find_entry(ini, buf))
	                {
	                    k = iniparser_getstring(ini, buf, "test");
						printf("=-=-=-=-=-=-=iniparser_getstring  k=%s, i=%d!\n", k, i);
	                    switch(i)
	                    {
	                        case ESSID:
	                        {
	                            memset(ssid, 0, sizeof(ssid));
	                            if (0 == StripString(k, '[', ']', ssid))
	                            {
	                                strncpy(conn->szSSID, ssid, sizeof(conn->szSSID) - 1);
	                            }
	                            break;
	                        }
	                        
	                        case QUALITY:                /* conn->nQuality = atoi(k); */     break;
	                        case ENCRYPTIONKEY:          strncpy(EncryEnable, k, sizeof(EncryEnable) - 1);             break;
	                        case IE_0:                   strncpy(IE[0], k, sizeof(IE[0]) - 1);                   break;
	                        case GROUPCIPHER_0:          strncpy(GroupCipher[0], k, sizeof(GroupCipher[0]) - 1);          break;
	                        case PAIRWISECIPHERS_0:      strncpy(PairwiseCiphers[0], k, sizeof(PairwiseCiphers[0]) - 1);      break;
	                        case AUTHENTICATIONSUITES_0: strncpy(AuthenticationSuites[0], k, sizeof(AuthenticationSuites[0]) - 1); break;
	                        case IE_1:                   strncpy(IE[1], k, sizeof(IE[1]) - 1);                   break;
	                        case GROUPCIPHER_1:          strncpy(GroupCipher[1], k, sizeof(GroupCipher[1]) - 1);          break;
	                        case PAIRWISECIPHERS_1:      strncpy(PairwiseCiphers[1], k, sizeof(PairwiseCiphers[1]) - 1);      break;
	                        case AUTHENTICATIONSUITES_1: strncpy(AuthenticationSuites[1], k, sizeof(AuthenticationSuites[1]) - 1); break;
	                        default:WIFI_ERR("Not support this access point info %s\n", k);    break;
	                    }
#ifdef WIFI_USE_8189
						break;
#endif
	                }
				}

				printf("=-=-=-=-=-=-=iniparser_getnsec after EncryEnable=%s, IE[0]=%s, GroupCipher[0]=%s!\n", EncryEnable, IE[0], GroupCipher[0]);
                if (0 == strcasecmp("off", EncryEnable))
                {
                    conn->nAuthMode   = AM_OPEN;
                    conn->nEncrypType = ET_NONE;
                }
                else
                {
                    if (strcasestr(IE[0], "WPA") || strcasestr(IE[0], "WPA2"))
                    {
                        conn->nAuthMode = AM_WPAPSK;
                        conn->struKeyCfg.bValid = 1;
                        conn->struKeyCfg.WPAKey.bValid = 1;
                    }
                    else
                    {
                        conn->nAuthMode = AM_SHARED;
                        conn->struKeyCfg.bValid = 1;
                    }
            
                    if (strcasestr(GroupCipher[0], "CCMP"))
                    {
                        conn->nEncrypType = ET_AES;
                    }
                    else
                    {
                        if (AM_SHARED == conn->nAuthMode)
                        {
                            conn->nEncrypType = ET_WEP;
                        }
                        else
                        {
                            conn->nEncrypType = ET_TKIP;
                        }
                    }
                }
            }
        }
		
    }
    
    iniparser_freedict(ini);
	unlink(tempfile);

    //为了加快生产测试时，wifi测试的流程
    //这里提前将需要的数据准备好
    if (0 == strlen(s_stWifiScanResultCache[0].szSSID))
    {
        int nCountTmp = 0;
        WLAN_ACCESSPOINT stWifiScanTmp[WLAN_MAX_AP_NUM];
    	wifi_get_accesspoints_for_test(stWifiScanTmp, &nCountTmp);
    }
	
    pthread_mutex_unlock(&m_getAccessPointsLock);

    return 0;
}

/* 针对字符串里面的特殊字符，进行转义处理 */
static int convertSpecialCharInString(const char *oldString, char *newString, unsigned int newStringSize)
{
    if (!oldString || !newString)
    {
        printf("Null Pointer Exception!\n");
        return -1;
    }

	int stringLen = strlen(oldString);
	int i=0;
    int j=0;
	memset(newString, 0, newStringSize);

	for(i=0; i<stringLen; i++)
	{
        //for (k=0; k<sizeof(special_characters); k++)
        {
            /* 针对特殊字符，由于添加了转移字符，字符串长度变大，需要判断是否溢出 */
            //if(oldString[i] == special_characters[k])
            {
                if (j + 1 > newStringSize)
                {
                    return 0;
                }
                else
                {
                    newString[j++] = '\\';
                    //break;
                }
            }
        }

        if (j < newStringSize)
        {
            newString[j++] = oldString[i];
        }
	}

	return 0;
}

int wifi_disable_conn()
{
	char command[256] = {0};
	int add_network_count = 0;
	int i = 0;

    /* 获取当前已经添加了几个wifi网卡 */
    ShellCmdOut("/usr/sbin/wpa_cli list_networks | wc -l", command, sizeof(command));

    add_network_count = atoi(command) - 2;

    for(i=0; i<add_network_count; i++)
    {
        memset(command, 0, sizeof(command));
        snprintf(command, sizeof(command), "/usr/sbin/wpa_cli remove_network %d", i);
        mysystem(command);
    }

	return 0;
}

int wifi_set_conn_cfg(LPWLAN_CONNECTION conn)
{
	printf("Enter wifi_set_conn_cfg: connect=%d, ssid=%s, mac=%s, networktype=%d, authmode=%d, encrypt=%d\n",
		conn->bConnect, conn->szSSID, conn->szMacAddress, conn->nNetworkType, conn->nAuthMode, conn->nEncrypType);

	char command[512] = {0};
	int ret = -1;
	int add_network_count = 0;
	int i = 0;
    
	if (!wifi_hasdev)
	{
		return -1;
	}

	if(0 == strlen(conn->szSSID))
	{
		WIFI_ERR("wifi SSID is NUll!\n");
		return 0;
	}

	//清除WPA命令的log信息，为后面提供log信息检测提供准确的信息
	clear_WPA_command_log();
	
    // 0表示Managed, 1表示Ad-hoc
    if (conn->nNetworkType == 0)
    {
        conn->nNetworkType = 2;
    }else{
        conn->nNetworkType = 1;
    }
    
	WLAN_AUTHMODE nMode = (WLAN_AUTHMODE)conn->nAuthMode;
	WLAN_ENCRYPTYPE nEncrypType = (WLAN_ENCRYPTYPE)conn->nEncrypType;
	LPWIFI_KEY_CFG pKeyCfg = &conn->struKeyCfg;

	if ((AM_OPEN == nMode) || (AM_SHARED == nMode))
	{
		if (ET_WEP == nEncrypType)
		{
			WIFI_KEY *pKey = &pKeyCfg->Key1+pKeyCfg->nDefKeyID-1;
			if ((0 == pKeyCfg->bValid) || (0 >= pKeyCfg->nDefKeyID) || (4 < pKeyCfg->nDefKeyID))
			{
				WIFI_ERR("KeyCfg Is Valid, Mode=%d, EncrypType=%d\n", nMode, nEncrypType);
				return -1;
			}
			else if (!pKey->bValid)
			{
				WIFI_ERR("KeyCfg Is Valid, DefKeyID=%d, Mode=%d, EncrypType=%d\n", pKeyCfg->nDefKeyID, nMode, nEncrypType);
				return -1;
			}
			else
			{
				int nKeyLen = strlen(pKey->szKey);
				if ((5 != nKeyLen) && (10 != nKeyLen) && (13 != nKeyLen) && (26 != nKeyLen))
				{
					WIFI_ERR("Key %d is Valid, Value=%s\n", pKeyCfg->nDefKeyID, pKey->szKey);
					return -1;
				}
			}
		}
	}
	else if ((AM_WPAPSK == nMode) || (AM_WPA2PSK == nMode))
	{
		if ((ET_TKIP == nEncrypType) || (ET_AES == nEncrypType))
		{
			if ((0 == pKeyCfg->bValid) || (0 == pKeyCfg->WPAKey.bValid))
			{
				WIFI_ERR("WPAKeyCfg Is invalid, Mode=%d, EncrypType=%d\n", nMode, nEncrypType);
				return -1;
			}
			else
			{
				int nKeyLen = strlen(pKeyCfg->WPAKey.szKey);
				if ((nKeyLen < 8) || (nKeyLen > 63))
				{
					WIFI_ERR("WPAKey is Valid, Value=%s\n", pKeyCfg->WPAKey.szKey);
					return -1;
				}
			}
		}
		else
		{
			WIFI_ERR("EncrypType Is Valid, Mode=%d, EncrypType=%d\n", nMode, nEncrypType);
			return -1;
		}
	}
	else if (AM_NONE == nMode)
	{
		;
	}
	else
	{
		WIFI_ERR("Mode Is Valid, Mode=%d\n", nMode);
		return -1;
	}

    printf("-----------------set conn 1---------------\n");
		/* 获取当前已经添加了几个wifi网卡 */
		ShellCmdOut("/usr/sbin/wpa_cli list_networks | wc -l", command, sizeof(command));
	
		add_network_count = atoi(command) - 2;
		printf("----------------add_network_count = %d -------------\n",add_network_count);
		for(i=0; i<add_network_count; i++)
		{
			enable(0);
			memset(command, 0, sizeof(command));
			snprintf(command, sizeof(command), "/usr/sbin/wpa_cli remove_network %d", i);
			mysystem(command);
		}
	//	  snprintf(command, sizeof(command), "/usr/sbin/wpa_cli remove_network 0");
	//	  mysystem(command);

	mysystem("/usr/sbin/wpa_cli -i wlan0 add_network");
	printf("-----------------set conn 2---------------\n");
	int tryCount = 5;
	while(tryCount--)//等待增加的网络配置生成
	{
	    /* 获取当前已经添加了几个wifi网卡 */
	    ShellCmdOut("/usr/sbin/wpa_cli list_networks | wc -l", command, sizeof(command));
		printf("try..., %d\n", atoi(command));

	    if(atoi(command) - 2)
	    {
			break;
	    }
		
		usleep(10000);
	}

	switch (nMode)
	{
		case AM_OPEN:
		case AM_SHARED:
		switch (nEncrypType)
		{
			case ET_NONE:
				mysystem("wpa_cli set_network 0 key_mgmt \"NONE\"");
				break;
			case ET_WEP:
				mysystem("wpa_cli set_network 0 key_mgmt \"NONE\"");
				if (pKeyCfg->bValid)
				{
					if (pKeyCfg->Key1.bValid)
					{
						set_wep_key(0, pKeyCfg->Key1.szKey);
					}
					if (pKeyCfg->Key2.bValid)
					{
						set_wep_key(0, pKeyCfg->Key2.szKey);
					}
					if (pKeyCfg->Key3.bValid)
					{
						set_wep_key(0, pKeyCfg->Key3.szKey);
					}
					if (pKeyCfg->Key4.bValid)
					{
						set_wep_key(0, pKeyCfg->Key4.szKey);
					}

					snprintf(command, 255, "/usr/sbin/wpa_cli set_network 0 wep_tx_keyidx %d", pKeyCfg->nDefKeyID);
					mysystem(command);
				}
				break;
			default:
				WIFI_ERR("Unsupported EncrypType %d in Open/Shared Mode, default: open\n", nEncrypType);
				mysystem("wpa_cli set_network 0 key_mgmt \"NONE\"");
				break;
		}
		break;
	case AM_WPAPSK:
	case AM_WPA2PSK:
	case AM_WPA:
	case AM_WPA2:
	case AM_WPANONE:
		switch (nEncrypType)
		{
			case ET_TKIP:
			case ET_AES:
				/*if(AM_WPA2PSK == nMode)
				{
					snprintf(command, 255, "/usr/sbin/wpa_cli set_network 0 key_mgmt WPA2-PSK");
					mysystem(command);
					printf("%s\n", command);
				}*/
				if (pKeyCfg->WPAKey.bValid)
				{
					printf("conver before psk = %s\n", pKeyCfg->WPAKey.szKey);
					char newPSK[WLAN_MAX_KEY_LEN * 2] = {0};
					convertSpecialCharInString(pKeyCfg->WPAKey.szKey, newPSK, sizeof(newPSK));
					printf("conver psk = %s\n", newPSK);

					snprintf(command, sizeof(command), "/usr/sbin/wpa_cli set_network 0 psk \\\"%s\\\"", newPSK);
                    /* 避免用wpa_cli命令设置失败的情况出现 */
                    if (1 != IsStringInCommand(command, "OK"))
                    {
                        memset(command, 0, sizeof(command));
                        snprintf(command, sizeof(command), "iwpriv wlan0 set WPAPSK=\"%s\"", newPSK);
                        printf("wpa_cli set psk fail, use: %s\n", command);
                        mysystem(command);
					}
				}
				break;
			default:
				WIFI_ERR("Unsupported EncrypType %d in WPA(2)(PSK) Mode\n", nEncrypType);
				break;
		}
		break;
	case AM_NONE:
			snprintf(command, 255, "/usr/sbin/wpa_cli set_network 0 key_mgmt NONE");
			mysystem(command);
		break;
	default:
		WIFI_ERR("Unsupported Auth Mode %d\n", nMode);
		break;
	}
	printf("-----------------set conn 3---------------\n");
    memset(command, 0, sizeof(command));
	snprintf(command, 255, "/usr/sbin/wpa_cli set_network 0 scan_ssid 1");
	mysystem(command);

    memset(command, 0, sizeof(command));
	snprintf(command, 255, "/usr/sbin/wpa_cli set_network 0 priority 1");
	mysystem(command);

	printf("conn->szSSID = %s\n", conn->szSSID);
	char newSSID[WLAN_MAX_SSID_LEN * 2] = {0};
	convertSpecialCharInString(conn->szSSID, newSSID, sizeof(newSSID));
	printf("newSSID = %s\n", newSSID);
	snprintf(command, sizeof(command), "/usr/sbin/wpa_cli set_network 0 ssid \\\"%s\\\"", newSSID);
	mysystem(command);
	
	printf("-----------------set conn 4---------------\n");
	mysystem(command);
	printf("-----------------set conn 5---------------\n");
	/* 避免用wpa_cli命令设置失败的情况出现 */
    if (1 != IsStringInCommand(command, "OK"))
    {
        memset(command, 0, sizeof(command));
        snprintf(command, sizeof(command), "iwpriv wlan0 set SSID=\"%s\"", newSSID);
        printf("wpa_cli set ssid fail, use: %s\n", command);
        mysystem(command);
    }
	printf("-----------------set conn 6------------wifi_enable = %d---\n", wifi_enable);
	memcpy(&wifi_connection, conn, sizeof(WLAN_CONNECTION));
	memcpy(&wifi_curr_ssid, conn->szSSID, WLAN_MAX_SSID_LEN-1);
	save_conn_cfg();
	enable(wifi_enable);
	ret = 0;

	printf("Exit wifi_set_conn_cfg-----------------set conn 6---------------\n");

	return ret;
}

int wifi_test_conn_cfg(LPWLAN_CONNECTION conn)
{
	WIFI_DBG("Enter wifi_test_conn_cfg: connect=%d, ssid=%s, mac=%s, networktype=%d, authmode=%d, encrypt=%d\n",
		conn->bConnect, conn->szSSID, conn->szMacAddress, conn->nNetworkType, conn->nAuthMode, conn->nEncrypType);

	char command[512] = {0};
	int ret = -1;
	int add_network_count = 0;
	int i = 0;
    
	if (!wifi_hasdev)
	{
		return 1;
	}

	if(0 == strlen(conn->szSSID))
	{
		WIFI_ERR("wifi SSID is NUll!\n");
		return 0;
	}

	//清除WPA命令的log信息，为后面提供log信息检测提供准确的信息
	clear_WPA_command_log();
	
    // 0表示Managed, 1表示Ad-hoc
    if (conn->nNetworkType == 0)
    {
        conn->nNetworkType = 2;
    }
    else
    {
        conn->nNetworkType = 1;
    }
    
	WLAN_AUTHMODE nMode = (WLAN_AUTHMODE)conn->nAuthMode;
	WLAN_ENCRYPTYPE nEncrypType = (WLAN_ENCRYPTYPE)conn->nEncrypType;
	LPWIFI_KEY_CFG pKeyCfg = &conn->struKeyCfg;

	if ((AM_OPEN == nMode) || (AM_SHARED == nMode))
	{
		if (ET_WEP == nEncrypType)
		{
			WIFI_KEY *pKey = &pKeyCfg->Key1+pKeyCfg->nDefKeyID-1;
			if ((0 == pKeyCfg->bValid) || (0 >= pKeyCfg->nDefKeyID) || (4 < pKeyCfg->nDefKeyID))
			{
				WIFI_ERR("KeyCfg Is Valid, Mode=%d, EncrypType=%d\n", nMode, nEncrypType);
				return -1;
			}
			else if (!pKey->bValid)
			{
				WIFI_ERR("KeyCfg Is Valid, DefKeyID=%d, Mode=%d, EncrypType=%d\n", pKeyCfg->nDefKeyID, nMode, nEncrypType);
				return -1;
			}
			else
			{
				int nKeyLen = strlen(pKey->szKey);
				if ((5 != nKeyLen) && (10 != nKeyLen) && (13 != nKeyLen) && (26 != nKeyLen))
				{
					WIFI_ERR("Key %d is Valid, Value=%s\n", pKeyCfg->nDefKeyID, pKey->szKey);
					return -1;
				}
			}
		}
	}
	else if ((AM_WPAPSK == nMode) || (AM_WPA2PSK == nMode))
	{
		if ((ET_TKIP == nEncrypType) || (ET_AES == nEncrypType))
		{
			if ((0 == pKeyCfg->bValid) || (0 == pKeyCfg->WPAKey.bValid))
			{
				WIFI_ERR("WPAKeyCfg Is invalid, Mode=%d, EncrypType=%d\n", nMode, nEncrypType);
				return -1;
			}
			else
			{
				int nKeyLen = strlen(pKeyCfg->WPAKey.szKey);
				if ((nKeyLen < 8) || (nKeyLen > 63))
				{
					WIFI_ERR("WPAKey is Valid, Value=%s\n", pKeyCfg->WPAKey.szKey);
					return -1;
				}
			}
		}
		else
		{
			WIFI_ERR("EncrypType Is Valid, Mode=%d, EncrypType=%d\n", nMode, nEncrypType);
			return -1;
		}
	}
	else if (AM_NONE == nMode)
	{
		;
	}
	else
	{
		WIFI_ERR("Mode Is Valid, Mode=%d\n", nMode);
		return -1;
	}

    printf("-----------------set test conn 1---------------\n");
	
    /* 获取当前已经添加了几个wifi网卡 */
    ShellCmdOut("/usr/sbin/wpa_cli list_networks | wc -l", command, sizeof(command));

    add_network_count = atoi(command) - 2;

    for(i=0; i<add_network_count; i++)
    {
        memset(command, 0, sizeof(command));
        snprintf(command, sizeof(command), "/usr/sbin/wpa_cli remove_network %d", i);
        mysystem(command);
    }
    
	mysystem("/usr/sbin/wpa_cli add_network");
	printf("-----------------set test conn 2---------------\n");

	switch (nMode)
	{
		case AM_OPEN:
		case AM_SHARED:
		switch (nEncrypType)
		{
			case ET_NONE:
				mysystem("wpa_cli set_network 0 key_mgmt \"NONE\"");
				break;
			case ET_WEP:
				mysystem("wpa_cli set_network 0 key_mgmt \"NONE\"");
				if (pKeyCfg->bValid)
				{
					if (pKeyCfg->Key1.bValid)
					{
						set_wep_key(0, pKeyCfg->Key1.szKey);
					}
					if (pKeyCfg->Key2.bValid)
					{
						set_wep_key(0, pKeyCfg->Key2.szKey);
					}
					if (pKeyCfg->Key3.bValid)
					{
						set_wep_key(0, pKeyCfg->Key3.szKey);
					}
					if (pKeyCfg->Key4.bValid)
					{
						set_wep_key(0, pKeyCfg->Key4.szKey);
					}

					snprintf(command, 255, "/usr/sbin/wpa_cli set_network 0 wep_tx_keyidx %d", pKeyCfg->nDefKeyID);
					mysystem(command);
				}
				break;
			default:
				WIFI_ERR("Unsupported EncrypType %d in Open/Shared Mode, default: open\n", nEncrypType);
				mysystem("wpa_cli set_network 0 key_mgmt \"NONE\"");
		}
		break;
	case AM_WPAPSK:
	case AM_WPA2PSK:
	case AM_WPA:
	case AM_WPA2:
	case AM_WPANONE:
		switch (nEncrypType)
		{
			case ET_TKIP:
			case ET_AES:
				if (pKeyCfg->WPAKey.bValid)
				{
					printf("conver before psk = %s\n", pKeyCfg->WPAKey.szKey);
					char newPSK[WLAN_MAX_KEY_LEN * 2] = {0};
					convertSpecialCharInString(pKeyCfg->WPAKey.szKey, newPSK, sizeof(newPSK));
					printf("conver psk = %s\n", newPSK);

					snprintf(command, sizeof(command), "/usr/sbin/wpa_cli set_network 0 psk \\\"%s\\\"", newPSK);
                    /* 避免用wpa_cli命令设置失败的情况出现 */
                    if (1 != IsStringInCommand(command, "OK"))
                    {
                        memset(command, 0, sizeof(command));
                        snprintf(command, sizeof(command), "iwpriv wlan0 set WPAPSK=\"%s\"", newPSK);
                        mysystem(command);
					}
				}
				break;
			default:
				WIFI_ERR("Unsupported EncrypType %d in WPA(2)(PSK) Mode\n", nEncrypType);
				break;
		}
		break;
	case AM_NONE:
			snprintf(command, 255, "/usr/sbin/wpa_cli set_network 0 key_mgmt NONE");
			mysystem(command);
		break;
	default:
		WIFI_ERR("Unsupported Auth Mode %d\n", nMode);
		break;
	}
	printf("-----------------set test conn 3---------------\n");
	
    memset(command, 0, sizeof(command));
	snprintf(command, 255, "/usr/sbin/wpa_cli set_network 0 scan_ssid 1");
	mysystem(command);

    memset(command, 0, sizeof(command));
	snprintf(command, 255, "/usr/sbin/wpa_cli set_network 0 priority 1");
	mysystem(command);

printf("conn->szSSID = %s\n", conn->szSSID);

	char newSSID[WLAN_MAX_SSID_LEN * 2] = {0};
	convertSpecialCharInString(conn->szSSID, newSSID, sizeof(newSSID));

printf("newSSID = %s\n", newSSID);

	snprintf(command, sizeof(command), "/usr/sbin/wpa_cli set_network 0 ssid \\\"%s\\\"", newSSID);
	/* 避免用wpa_cli命令设置失败的情况出现 */
    if (1 != IsStringInCommand(command, "OK"))
    {
        memset(command, 0, sizeof(command));
        snprintf(command, sizeof(command), "iwpriv wlan0 set SSID=\"%s\"", newSSID);
        mysystem(command);
    }

	enable(1);
	ret = 0;

	WIFI_DBG("Exit wifi_set_conn_cfg-----------------set test conn 4---------------\n");

	return ret;
}

#define WIFI_STATUS_GET_INTRVAL 2

//暂时没有用到该函数
int wifi_get_conn_status(int *status, char ssid[WLAN_MAX_SSID_LEN], char mac[WLAN_MAC_ADDRESS_LEN])
{
	WIFI_DBG("Enter wifi_get_conn_status\n");
	char buffer[4096] = {0};
	int ret = 0;
    char tempfile[256] = {0};
    struct timeval now = {0};
    long long lCallIntrvalUsec = 0;
    static int s_nWifiStatusBak = 0;
    static char s_szSsidBak[WLAN_MAX_SSID_LEN] = {0};
    static char s_szMacBak[WLAN_MAC_ADDRESS_LEN] = {0};
    static struct timeval s_stLastCallTime = {0};

	if (NULL != status)
	{
		*status = 0;
	}
	else
	{
		WIFI_DBG("Exit wifi_get_conn_status\n");
		return -1;
	}

	if (!wifi_hasdev)
	{
		return 0;
	}

	//WIFI_STATUS_GET_INTRVAL秒之内
	//再次获取直接返回上一次的结果
	gettimeofday(&now, NULL);
    lCallIntrvalUsec = (now.tv_sec - s_stLastCallTime.tv_sec) * 1000000 + 
        (now.tv_usec - s_stLastCallTime.tv_usec);
	if(lCallIntrvalUsec < 0)
	{
		s_stLastCallTime = now; 
	}
    if (lCallIntrvalUsec < (WIFI_STATUS_GET_INTRVAL * 1000000))
    {
        if (NULL != status)
	    {
            *status = s_nWifiStatusBak;
        }

        if (NULL != ssid)
		{
			strncpy(ssid, s_szSsidBak, WLAN_MAX_SSID_LEN);
			ssid[WLAN_MAX_SSID_LEN-1] = '\0';
		}
		
        if (NULL != mac)
		{
			strncpy(mac, s_szMacBak, WLAN_MAC_ADDRESS_LEN);
			mac[WLAN_MAC_ADDRESS_LEN-1] = '\0';
		}

		return 0;
    }
	
	snprintf(tempfile, 255, "/var/run/wpastatus.%lld", now.tv_usec);
	snprintf(buffer, sizeof(buffer), "/usr/sbin/wpa_cli status>%s", tempfile);
	mysystem(buffer);
	static int status_tmp = 0; //保持上一次的值，只有读取文件中的状态为未连接时，才去切换为非连接

	*status = status_tmp;	//赋初值
	FILE *fp = fopen(tempfile, "r");
	if (NULL == fp)
	{
		fprintf(stderr, "Fail to open (/var/run/wpastatus.%lld): %s\n", now.tv_usec, strerror(errno));
		return -1;
	}
	
	while (!feof(fp))
	{
		buffer[0] = '\0';
		fgets(buffer, 256, fp);
		
		if (0 == strncmp("bssid", buffer, 5))
		{
			if (NULL != mac)
			{
				strncpy(mac, buffer+6, WLAN_MAC_ADDRESS_LEN - 1);
				mac[WLAN_MAC_ADDRESS_LEN-1] = '\0';
			}
		}
		else if (0 == strncmp("ssid", buffer, 4))
		{
			if (NULL != ssid)
			{
				strncpy(ssid, buffer+5, WLAN_MAX_SSID_LEN);
				ssid[WLAN_MAX_SSID_LEN-1] = '\0';
				char *p = ssid;
				while (('\n' != *p) && ('\0' != *p))
				{
					p++;
				}
				*p = '\0';
			}
		}
		else if (0 == strncmp("wpa_state", buffer, 9))
		{
			if (0 == strncmp("COMPLETED", buffer+10, 9))
			{
				status_tmp = 1;
			}
			else
			{
				status_tmp = 0;
			}
		}
	}
	*status = status_tmp;	//根据最后文件内容改变状态
	fclose(fp);
	unlink(tempfile);

	//备份结果
	if (NULL != status)
    {
        s_nWifiStatusBak = *status;
    }

    if (NULL != ssid)
	{
	    memset(s_szSsidBak, 0, sizeof(s_szSsidBak));
		strncpy(s_szSsidBak, ssid, WLAN_MAX_SSID_LEN);
	}
	
    if (NULL != mac)
	{
	    memset(s_szMacBak, 0, sizeof(s_szMacBak));
		strncpy(s_szMacBak, mac, WLAN_MAC_ADDRESS_LEN);
	}

    //记录最后一次调用的时间
	s_stLastCallTime = now; 

	return ret;
}


/*
该接口定义为给设备内部用，因为是直接获取的真实状态且很耗时所以上层协议获取时只给历史状态，
1.net模块wlan检测线程会调用来获取真实状态，每秒调用一次
2.wifi检测时也会调用来获取连接状态
*/
int wifi_get_curr_status(int *status)
{
    //WIFI_DBG("Enter wifi_get_curr_status\n");
    int ret = 0;
    long long lCallIntrvalUsec = 0;
    static struct timeval s_stLastCallTime = {0};
    struct timeval now = {0};

	pthread_mutex_lock(&m_getWifiStatusLock);

    if (NULL != status)
    {
        *status = 0;
    }
    else
    {
        WIFI_ERR("status == NULL\n");
		pthread_mutex_unlock(&m_getWifiStatusLock);
        return -1;
    }

    if (!wifi_hasdev)
    {
		pthread_mutex_unlock(&m_getWifiStatusLock);
        return 0;
    }

	//WIFI_STATUS_GET_INTRVAL秒之内
	//再次获取直接返回上一次的结果
	gettimeofday(&now, NULL);
    lCallIntrvalUsec = (now.tv_sec - s_stLastCallTime.tv_sec) * 1000000 + (now.tv_usec - s_stLastCallTime.tv_usec);
	if(lCallIntrvalUsec < 0)
	{
		s_stLastCallTime = now; 
	}
    if (lCallIntrvalUsec < (WIFI_STATUS_GET_INTRVAL * 1000000))
    {
        if (NULL != status)
	    {
            *status = g_wifiStatus;
        }
		WIFI_DBG("Use histroy wifi status!\n");
		pthread_mutex_unlock(&m_getWifiStatusLock);
		return 0;
    }

    ret = IsStringInCommand("wpa_cli status", "COMPLETED");
    if (1 == ret)
    {
        *status = 1; 
    }
    else if (0 == ret)
    {
        *status = 0;
    }
    else
    {
        WIFI_ERR("Fail to call IsStringInCommand\n");
        *status = 0;
		pthread_mutex_unlock(&m_getWifiStatusLock);
        return -1;
    }
    WIFI_DBG("Current wlan0 status is %s\n", *status==1?"COMPLETED":"break");
	g_wifiStatus = *status;
    //记录最后一次调用的时间
	s_stLastCallTime = now; 
	pthread_mutex_unlock(&m_getWifiStatusLock);

    return 0;
}

/*
获取的是wifi_get_curr_status获取的历史连接状态
*/
static int wifi_get_curr_statusEx(int *status)
{
	pthread_mutex_lock(&m_getWifiStatusLock);

    if (NULL != status)
    {
        *status = 0;
    }
    else
    {
        WIFI_ERR("status == NULL\n");
		pthread_mutex_unlock(&m_getWifiStatusLock);
        return -1;
    }
	*status = g_wifiStatus;
	
	pthread_mutex_unlock(&m_getWifiStatusLock);

	return 0;
}

int wifi_reset_enable(void)
{
    char buffer[255] = {0};
	snprintf(buffer, sizeof(buffer), "/usr/sbin/wpa_cli %s_network 0", "disable");
	mysystem(buffer);

    usleep(100000);

    memset(buffer, 0, sizeof(buffer));
    snprintf(buffer, sizeof(buffer), "/usr/sbin/wpa_cli %s_network 0", "enable");
    mysystem(buffer);
    
    return 0;
}

int wifi_set_config_enable(int enable)
{
	char buffer[255] = {0};

	snprintf(buffer, 255, "/usr/sbin/wpa_cli %s_network 0", enable ? "enable" : "disable");
	mysystem(buffer);

	return 0;
}

int enable(int enable)
{
	char buffer[255] = {0};

	snprintf(buffer, 255, "/usr/sbin/wpa_cli %s_network 0", enable ? "enable" : "disable");
	mysystem(buffer);
	// mysystem("/usr/sbin/wpa_cli reconnect");

    return 0;
}

int set_wep_key(int index, char *value)
{
	int len = strlen(value);
	char cmd[256] = {0};
	switch (len)
	{
    	case 10:
    	case 26:
    		snprintf(cmd, 255, "/usr/sbin/wpa_cli set_network 0 wep_key%d %s", index, value);
    		mysystem(cmd);
    		break;
    	case 5:
    	case 13:
    		snprintf(cmd, 255, "/usr/sbin/wpa_cli set_network 0 wep_key%d \"%s\"", index, value);
    		mysystem(cmd);
    		break;
	}
	printf("%s\n", cmd);
	return 0;
}

int load_conn_cfg()
{
    printf("\n...wifi.c..load_conn_cfg()..\n");

	//mysystem("/usr/sbin/wpa_cli reconfigure");	//force wpa_supplicant to re-read its configuration file

	return 0;
}

int save_conn_cfg(void)
{
	mysystem("/usr/sbin/wpa_cli save_config");

	return 0;
}

//!\brief 获取wifi设备是否存在
//! 读取/proc/net/dev 看是否有无线设备
int has_dev(const char *devname)
{
	char buffer[512] = {0};
	char *dev = NULL;
	char *devend = NULL;
	int ret = 0;

	FILE *fp = fopen("/proc/net/dev", "r");
	if (NULL == fp)
	{
		return 0;
	}

	fgets(buffer, 512, fp);
	fgets(buffer, 512, fp);

	while (1)
	{
		if (feof(fp))
		{
			break;
		}

		fgets(buffer, 512, fp);
		dev = buffer;
		while (isspace(*dev))
		{
			dev++;
		}

		devend = strchr(dev, ':');
		if (NULL == devend)
		{
			break;
		}
		*devend = '\0';

		if (0 == strncmp(dev, devname, strlen(devname)+1))
		{
			ret = 1;
		}
	}

    fclose(fp);
    fp = NULL;
    
	return ret;
}

/*@brief     StringExistInFile 查看文件中是否存在字符串
 *@param[in] string      欲查找的字符串
 *@param[in] stringSize  字符串大小(建议使用strlen(string)+1)
 *@param[in] fp          文件指针
 *@return    字符串存在返回1, 否则返回0
 */
static int StringExistInFile(const char * string, int stringSize, FILE * fp) 
{
    int i=0, j=0, end=0;
    int bFound = 0;
    char *part = NULL;
    char *temp = NULL;
    
    part = (char *)malloc(stringSize*sizeof(char));    
    fseek(fp, 0L, SEEK_END);
    end = ftell(fp) - stringSize + 2;
    temp = part;

    while(i < end) 
    {
        j = 0;
        fseek(fp, (long)i++, SEEK_SET);
        fgets(part, stringSize, fp);
        while(*part) 
        {
            if(*string == *part) 
            {
                j++;
                string++;
                part++;    // 注意此处的指针已经自增
                continue;
            }
            bFound = 0;
            break;
        }
        if(j == stringSize - 1) 
        {
            bFound = 1;
            break;
        }else{
            string -= j;
            part -= j;
        }
    }
    /* 文件指针重新定位到头 */
    fseek(fp, 0, SEEK_SET);

    /* 不可以释放part，因为part已经跳到其他地址了 */
    free(temp);
    temp = NULL;

    return bFound;
}

static int clear_WPA_command_log()
{
	char command[64] = {0};

	snprintf(command, sizeof(command), "echo > %s", WPA_LOG_FILE);
	mysystem(command);
	printf("clear file %s !\n", WPA_LOG_FILE);

	return 0;
}

/*@brief     get_wifi_fail_reason 获取wifi连接失败的原因，目前只有密码错误的检查
 *@param[out] reason  返回失败的原因，0:不明的原因。1：密码错误
 *@return    成功返回0，失败返回-1
 */
int get_wifi_fail_reason(int *reason)
{
	int ret = 0;

    FILE *fp = fopen(WPA_LOG_FILE, "r");
    if (fp)
    {
        if (StringExistInFile(WIFI_KEY_INCORRECT, strlen(WIFI_KEY_INCORRECT)+1, fp) == 1)
        {
        	printf("Get fail reason:%s\n", WIFI_KEY_INCORRECT);
            *reason = 1;
        }
		else
			{
				*reason = 0;
			}
        fclose(fp);
    }
	else
		{
			printf("Open file %s fail!\n", WPA_LOG_FILE);
		}
    fp = NULL;

	return ret;
}
