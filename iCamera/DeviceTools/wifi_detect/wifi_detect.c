#define _GNU_SOURCE

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if.h>
#include <netinet/in.h>
#include <net/if_arp.h>
#include <sys/time.h>
#include <fcntl.h>
#include <signal.h>
#include <dirent.h>
#include <pthread.h>
#include <ctype.h>
#include <assert.h>

#include <semaphore.h>
#include <sys/stat.h>
#include <sys/socket.h> 
#include <sys/ipc.h>
#include <sys/msg.h>

#if defined (__USE_WIFI__)
#include "libusb.h"
#endif

#ifdef __AUTHORIZE__
#include "Authority.h"
#else
#define MAC_ADDR_LEN    (6)
#endif
#include "mauth.h"
#include "global_string.h"
#include "global_FilePath.h"

#undef _GNU_SOURCE

#define PRTCOLOR
#define MODENAME    "WifiDetect"

/* 打印宏 */
#define MSEG_DEBUG   0x01
#define MSEG_INFO    0x02
#define MSEG_ERROR   0x04

static int LogLevel = /*MSEG_DEBUG | */MSEG_INFO | MSEG_ERROR;

#ifdef PRTCOLOR
#define NONE         "\033[m"  
#define RED          "\033[0;32;31m"
#define LIGHT_RED    "\033[1;31m"
#define GREEN        "\033[0;32;32m"
#define LIGHT_GREEN  "\033[1;32m"
#define BLUE         "\033[0;32;34m"
#define LIGHT_BLUE   "\033[1;34m"
#define DARY_GRAY    "\033[1;30m"
#define CYAN         "\033[0;36m"
#define LIGHT_CYAN   "\033[1;36m"
#define PURPLE       "\033[0;35m"
#define LIGHT_PURPLE "\033[1;35m"
#define BROWN        "\033[0;33m"
#define YELLOW       "\033[1;33m"
#define LIGHT_GRAY   "\033[0;37m"
#define WHITE        "\033[1;37m"
#else
#define NONE
#define RED
#define LIGHT_RED
#define GREEN
#define LIGHT_GREEN
#define BLUE
#define LIGHT_BLUE
#define DARY_GRAY
#define CYAN
#define LIGHT_CYAN
#define PURPLE
#define LIGHT_PURPLE
#define BROWN
#define YELLOW
#define LIGHT_GRAY
#define WHITE
#endif

#undef printf
#define    LPRINT(mylog, LOG_LEVEL, format, args...) do {    \
    if (mylog & LOG_LEVEL)    {    \
        printf(format, ##args);    \
    }    \
} while (0)
#define WIFI_ERROR(format, args...)       LPRINT(LogLevel, MSEG_ERROR, LIGHT_RED "[%s][ERR][%s:%d]" NONE format, MODENAME, __FILE__, __LINE__, ##args)
#define WIFI_INFO(format, args...)        LPRINT(LogLevel, MSEG_INFO, LIGHT_PURPLE "[%s][INFO][%s:%d]" NONE format, MODENAME, __FILE__, __LINE__, ##args)
#define WIFI_DEBUG(format, args...)       LPRINT(LogLevel, MSEG_DEBUG, LIGHT_GREEN "[%s][DBG][%s:%d]" NONE format, MODENAME, __FILE__, __LINE__, ##args)

#define WIFI_EXIST_FlAG         "/tmp/.wifi_if_exist"   // USB Wifi存在，则创建该标志性文件，以便以后使用 
#define WIFI_DRV_NAME_AP        "mt7601Uap"
#define WIFI_DRV_NAME_STA       "mt7601Usta"
#define USB_WIFI_DRV_PATH       "/onvif/wifi"

#define PROC_NET_WIRELESS    "/proc/net/wireless"
#define PROC_NET_DEV         "/proc/net/dev"
#define WPA_SUPPLICANT       "/mnt/app/bin/wpa_supplicant -B -Dnl80211 -iwlan0 -c /etc/wifi.conf -f /tmp/wpa.log"
#define KILL_MAIN_APP        "killall -9 ambarella_test; killall -9 ipc"
#define RUN_MAIN_APP         "/root/edvr/ambarella_test &"
#define DROP_MEMORY          "echo 3 > /proc/sys/vm/drop_caches"
#define MODIFY_MTU           "echo 1460 > /sys/class/net/wlan0/mtu"

#define UDHCPD_CONF    "/mnt/app/custom/config/udhcpd.conf"

/* 产生一个随机的DID号，用于产生随机MAC地址 */
#define GET_RANDOM_DID_NUM   "cat /proc/sys/kernel/random/uuid| cksum | cut -f1 -d\" \""
#if defined (__IS_CUSTOM_LS__)
#define LONGSEE_DEVICE_INFO      "/mnt/mtd/config/longsee_device_info.cfg"
#endif
#define DEFAULT_AP_IP           "223.223.100.1"
#define DEFAULT_AP_NETMASK      "255.255.255.0"
#define DEFAULT_AP_GATEWAY      "223.223.100.1"
#define DEFAULT_STA_IP          "192.168.2.1"
#define DEFAULT_STA_NETMASK     "255.255.255.0"
#define DEFAULT_STA_GATEWAY     "192.168.2.1"
#define DEFAULT_SPFTAP_PWD      "01234567"
#define DEFAULT_AP_PREFIX       "ZWAP_"

//#define AP_NO_PASSWD

#define UID_CFG_PATH            "/mnt/mtd/config/tutkuid.conf"
#define DEVICE_BOARD_TYPE        "/tmp/app/.deviceBoardType"

#define MAX_LEN  18
#define NET_INTERFACE   "eth0"
#define INIT_MAC        "00:00:00:00:00:00"
#define CMD_GET_MAC     "/mnt/app/bin/fw_printenv -n ethaddr"
#define CMD_SET_MAC     "/mnt/app/bin/fw_setenv ethaddr"
#define MAC_FILE        "/mnt/mtd/config/savemac"
#define WLAN_MAC        "/mnt/mtd/config/wifimac.txt"
#define RAND_MAC_FILE   "/mnt/mtd/config/randmac"
#define AUTH_MAC_FILE   "/mnt/mtd/config/authmac"
#define MAC_CMD         "ifconfig"
#define CLOSE_ETH0_NET  "ifconfig eth0 down"
#define OPEN_ETH0_NET   "ifconfig eth0 up"

#define BOA_CONF        "/mnt/mtd/etc/conf.d/boa/boa.conf"
#define BOA_CONF_BAK    "/mnt/mtd/etc/conf.d/boa/boa.conf-bak"
#define BOA_CFG_HEAD    "[BOA_CFG_BEGIN]"
#define BOA_CFG_TAIL    "[BOA_CFG_END]"
#define IPC_UPGRADE_FILE_NAME "ipcUpgrade.bin"
#define SD_UPGRADE_FLAG_FILE_NAME ".sdCardUpgradeFlag"

#ifdef __USE_EMMC__
#define SD_DEV_NODE_PART1  "/dev/mmcblk1p1"
#define MOUNT_SD_PART1     "mount -t vfat /dev/mmcblk1p1 /tmp/app/disk"
#else
#define SD_DEV_NODE_PART1  "/dev/mmcblk0p1"
#define MOUNT_SD_PART1     "mount -t vfat /dev/mmcblk0p1 /tmp/app/disk"
#endif

#define DO_SD_UPGRADE        "/mnt/app/bin/myWatchDogProcess sdUpgrade"

#define EDVR_CFG_FILE_PATH          "/mnt/mtd/custom/config/Edvr.cfg"
#define DEF_EDVR_CFG_FILE_PATH      "/mnt/mtd/custom/config/Edvr.default"
#define WIFI_MODE_0_BAK_FILE_PATH   "/mnt/mtd/config/.WlanWifiMode:0"
#define WIFI_MODE_1_BAK_FILE_PATH   "/mnt/mtd/config/.WlanWifiMode:1"
#define WIFI_MODE_2_BAK_FILE_PATH   "/mnt/mtd/config/.WlanWifiMode:2"
#define WIFI_MODE_BAK_FILE_PATH     "/mnt/mtd/config/.WlanWifiMode*"
#define CUSTOM_INFO                 "/mnt/app/custom/info/custominfo.ini"

#define ADDRESS_BYTES    (4) //Bytes in an address
///加密验证
#define ENCRPTSEND_DATA             0x113
#define ENCRPTGET_TIME              0x114
#define ENCRPTGET_DATA              0x115
#define ENCRPTGET_CURR_TIME         0x116
#define ENCRPTGET_ERR               0x117
#define ENCRPTGET_DEVINFO           0x118

#define DEBUG                //打印控制
#ifdef DEBUG
#define DEBUGPRINTF(x...)    do{printf("\033[1;32m[%s: %d]\033[m", __FUNCTION__, __LINE__);printf(x);}while(0)
#else
#define DEBUGPRINTF(x...) NULL
#endif

#define MAX_USB_DEV      (8)
#define MAX_STRING_SIZE  (64)

static unsigned char hil_encrypt[32+48+32] = {0};
static char hwaddr[32];

static int boardID=0;

/* wifi模式 */
typedef enum
{
    NO_WIFI_DEV   = 0,
    WLAN_STA_MODE = 1,
    SOFT_AP_MODE  = 2,
    UNKNOWN_MODE  = 3,
}eWifiMode;

typedef enum
{
    NO_USB_WIFI     = 0,
    MT7601_DEV      = 1,
    RTL8188FTV_DEV  = 2,
}eUSBWifiDev;

typedef enum
{
    FS_FAT32,
    FS_EXFAT,
    FS_NTFS,
    FS_EXT2,
    FS_ERROR_TYPE,
}E_FILE_SYS_TYPE;

typedef struct tagUSBDevInfo{
    int nIdVendor;
    int nIdProduct;
    int nSerialNum;
    int nLength;
    int nDescriptorType;
    int nCdUSB;
    int nDeviceClass;
    int nDeviceSubClass;
    int nDeviceProtocol;
    int nMaxPacketSize;
    int nNumConfigurations;
    unsigned char strManufacturer[MAX_STRING_SIZE];
    unsigned char strProduct[MAX_STRING_SIZE];
}stuUSBDevInfo, *lpUSBDevInfo;

//配置区名(网络)
//static const char *INet_CfgSection = "INet_Cfg";
//wifi模式
//static const char *INet_WLANWifiMode = "wifiMode";
//AP SSID
static char g_nSoftApSsid[64] = {0};
//AP PASSWD
static char g_nSoftApPasswd[64] = {0};

static unsigned char g_HilEncrypt[32+48+32] = {0};
static int g_nBoardID = 0;

//如果要修改AP的默认IP等参数，
//需要修改定制文件/custom/config/udhcpd_xxx.conf及/custom/info/custominfo.ini
static char g_szDefaultApIp[64] = {0};
static char g_szDefaultApNetmask[64] = {0};
static char g_szDefaultApGateway[64] = {0};

#define HOSTAPD_CONF_FILE_PATH "/tmp/hostapd.conf"
//AP配置无密码格式化
static const char *INet_AP_CONF_NO_PWD_FORMAT = 
	"echo -e "
	"'interface=wlan0\n"
	"driver=hostap\n"
	"ctrl_interface=/var/hostapd\n"
	"ssid=%s\n"\
	"channel=6\n"
	"hw_mode=g\n"
	"ieee80211n=1\n"
	"ht_capab=[SHORT-GI-20][SHORT-GI-40][HT40-]\n'"
	" > %s";
//AP配置有密码格式化
static const char *INet_AP_CONF_FORMAT = 
	"echo -e "
	"'interface=wlan0\n"
	"driver=hostap\n"
	"ctrl_interface=/var/hostapd\n"
	"ssid=%s\n"\
	"channel=6\n"
	"hw_mode=g\n"
	"ieee80211n=1\n"
	"ht_capab=[SHORT-GI-20][SHORT-GI-40][HT40-]\n"
	"wpa=3\n"
	"wpa_key_mgmt=WPA-PSK\n"
	"wpa_pairwise=TKIP CCMP\n"
	"wpa_passphrase=%s\n'"
	" > %s";

static char g_szDefaultApPrefix[32] = {0};   // 默认的AP前缀，如"ZWAP_"

static eUSBWifiDev g_eCurrUSBWifiDev = NO_USB_WIFI;

#ifdef __AUTHORIZE__
static stuShmAuthInfo m_stuAuthInfo;
#endif

#if 0
static int mysystem(const char* pCmdstring)
{
    pid_t pid = 0;
    int nStatus = 0;

    if (NULL == pCmdstring)
    {
        return 1;
    }
    
    if ((pid=vfork()) < 0 )
    {
        printf("vfork failed.cmd:%s\n", pCmdstring);
        nStatus = -1;
    }
    else if (pid==0)
    {
        execl("/bin/sh", "sh", "-c", pCmdstring, (char*)0);
        _exit(127);
    }
    else
    {
        while (waitpid(pid, &nStatus, 0)<0)
        {
            if(errno!=EINTR)
            {
                nStatus = -1;
                break;
            }
        }
    }

    return nStatus;
}
#endif

/*@ preif 在命令command的输出中寻找字符串string
 *@ param [in] command 要执行的命令
 *@ param [in] string  要寻找的字符串(忽略大小写)
 *@ return 0 不存在该字符串
 *@        1 存在该字符串
 *@       -1 出错
 *@ note  忽略字符串的大小写，如果不想忽略，则可以用strstr代替strcasestr
 *@       使用strcasestr的时候，要使用宏_GNU_SOURCE，否则会出现警告(不影响使用)
 *@       可以拓展成命令行字符串的定位
 */
static int IsStringInCommand(const char *pCommand, const char *pString)
{   
    int iRet = 0;
    char *pPoint = NULL;
    FILE *fp = NULL;
    char szBuf[1024] = {0};

    /* 将获取到的信息放到缓冲区buf中 */
    if ((fp = popen(pCommand, "r")) == NULL)
    {
        perror("popen");
        return -1;
    }

    while (fgets(szBuf, sizeof(szBuf), fp) != NULL)
    {
        if ('\n' == szBuf[strlen(szBuf)-1])
        {
            szBuf[strlen(szBuf)-1] =  '\0';
        }

        pPoint = strcasestr(szBuf, pString);
        if (NULL == pPoint)
        {
            iRet = 0;
            continue;
        }
        else
        {
            iRet = 1;
            break;
        }
    }
    if (-1 == pclose(fp))
    {
        perror("pclose");
        return -1;
    }
    
    return iRet;
}

// 描述:  执行命令并将结果存储到result字符串中 
// 参数:  cmd表示要执行的命令
// 返回值:成功返回0，失败返回-1
static int ShellCmdOut(const char *pCmd, char *pResult, int nLen) 
{
    int nRet = 0;
    struct timeval stNow = {0};
    char szfileName[64] = {0};
    char szCommand[256] = {0};
    FILE *fp = NULL;

    if (!pCmd || !strlen(pCmd) || !pResult || nLen <= 0)
    {
        return -1;
    }
    
    gettimeofday(&stNow, NULL);

    memset(szfileName, 0, sizeof(szfileName));
    snprintf(szfileName, sizeof(szfileName), "/tmp/.ShellCmdOut%d", stNow.tv_usec);

    memset(pResult, 0, nLen); 
    memset(szCommand, 0, sizeof(szCommand));
    snprintf(szCommand, sizeof(szCommand), "%s > %s", pCmd, szfileName);

    mysystem(szCommand);
    if (NULL == (fp = fopen(szfileName, "r")))
    {
        perror("fopen");
        nRet = -1;
    }
    else
    {
        fread(pResult, sizeof(char), nLen-1, fp);
        //printf("result: %s\n", pResult);
        if (fclose(fp))
        {
            perror("fclose");
            nRet = -1;
        }
        fp = NULL;
    }

    memset(szCommand, 0, sizeof(szCommand));
    snprintf(szCommand, sizeof(szCommand), "rm -rf %s", szfileName);
    //mysystem(szCommand);
    
    return nRet;                                
}

static int DeviceUSBDevInfo(stuUSBDevInfo USBDevInfo[MAX_USB_DEV], int *usb_dev_count)
{
#if defined (__USE_WIFI__)
    int ret = -1;
    int e = 0;
    int i = 0;

    struct libusb_device *dev = NULL;    
    struct libusb_device **devs = NULL;
    struct libusb_device_handle *handle = NULL;
    struct libusb_device_descriptor desc;

    memset(&desc, 0, sizeof(desc));
    memset(USBDevInfo, 0, sizeof(USBDevInfo));

    // Init libusb
    ret = libusb_init(NULL);
    if(ret < 0)
    {
        printf("Failed to initialise libusb\n");
        return 1;
    }
    else
    {
        printf("USB Init Successful!\n");
    }

    // Get a list os USB devices
    *usb_dev_count = libusb_get_device_list(NULL, &devs);
    if (*usb_dev_count < 0)
    {
        printf("There are no USB devices on bus\n");
        goto error_exit;
    }

    while ((dev = devs[i]) != NULL)
    {
        ret = libusb_get_device_descriptor(dev, &desc);
        if (ret < 0)
        {    
            printf("failed to get device descriptor\n");    
            libusb_free_device_list(devs, 1);    
            libusb_close(handle);    
            goto error_exit;
        }    

        e = libusb_open(dev, &handle);    
        if (e < 0)    
        {    
            printf("error opening device\n");    
            libusb_free_device_list(devs, 1);    
            libusb_close(handle);    
            goto error_exit;
        }

        USBDevInfo[i].nIdVendor = desc.idVendor;
        USBDevInfo[i].nIdProduct = desc.idProduct;
        USBDevInfo[i].nSerialNum = desc.iSerialNumber;
        USBDevInfo[i].nLength = desc.bLength;
        USBDevInfo[i].nDescriptorType = desc.bDescriptorType;
        USBDevInfo[i].nCdUSB = desc.bcdUSB;
        USBDevInfo[i].nDeviceClass = desc.bDeviceClass;
        USBDevInfo[i].nDeviceSubClass = desc.bDeviceSubClass;
        USBDevInfo[i].nDeviceProtocol = desc.bDeviceProtocol;
        USBDevInfo[i].nMaxPacketSize = desc.bMaxPacketSize0;
        USBDevInfo[i].nNumConfigurations = desc.bNumConfigurations;

        e = libusb_get_string_descriptor_ascii(handle, 
                                                  desc.iManufacturer, 
                                                  (unsigned char*)USBDevInfo[i].strManufacturer, 
                                                  sizeof(USBDevInfo[i].strManufacturer));
#if 0
        if (e < 0)
        {
            libusb_free_device_list(devs, 1);
            libusb_close(handle);
            goto error_exit;
        }
#endif
        e = libusb_get_string_descriptor_ascii(handle, 
                                                  desc.iProduct, 
                                                  (unsigned char*)USBDevInfo[i].strProduct, 
                                                  sizeof(USBDevInfo[i].strProduct));    
        if(e < 0)
        {
            libusb_free_device_list(devs, 1);
            libusb_close(handle);
            goto error_exit;
        }
        i ++;
    }
    libusb_free_device_list(devs, 1);
    libusb_close(handle);
    libusb_exit(NULL);
    return 0;

error_exit:
    libusb_exit(NULL);
    return -1;
#else
	return 0;
#endif
}

#ifdef __AUTHORIZE__

#else
static int GetBoardInfo(char *pInfo)
{
    int i = 0; 
    int fd = -1;
    int regi = 0;
    char time_dat[16] = {0};
    char outdat[16] = {0};
    char ctime_dat[16] = {0};
    long boot_sec = 0;
    long curr_time = 0;
    long my_time = 0;
    int sub_sec = 0;
    char dat[64] = {0};
    char out[16] = {0};     

    if (NULL == pInfo)
    {
        return -1;
    }

    fd = open("/dev/encrypt", O_RDWR);
    if (fd < 0)
    {
        WIFI_ERROR("/dev/encrypt open fial\n");
        return -1;
    }

    for (i = 0; i < 16; i++)
    {
        time_dat[i]  = 0;
        ctime_dat[i] = 0;
        outdat[i]    = 0;
    }

    regi = (int)ctime_dat;
    ioctl(fd, ENCRPTGET_CURR_TIME, regi); 
    memcpy(&curr_time, ctime_dat, 4);

    regi = (int)time_dat;
    ioctl(fd, ENCRPTGET_TIME, regi); 
    get_fs_tdata(time_dat, outdat);
    memcpy(&boot_sec, outdat, 4);

    sub_sec = 0x495C0780 - boot_sec;

    if (sub_sec < 0) 
    {
        sub_sec = -sub_sec;
    }
    if (sub_sec > 3600*24)
    {
        WIFI_ERROR("[SDK] sub_sec > 3600*24 sub_sec = 0x%x\n", sub_sec);
        close(fd);
        return -1;
    }

    put_fs_auth(ctime_dat,out);
    regi = (int)out;
    ioctl(fd, ENCRPTSEND_DATA, regi); 
    regi = (int)out;
    ioctl(fd, ENCRPTGET_DATA, regi);

    get_fs_auth(out, ctime_dat);
    if (ctime_dat[4]!=0 ||ctime_dat[5]!=0||ctime_dat[6]!=0||ctime_dat[7]!=0)
    {
        WIFI_ERROR("[SDK] ctime_dat[4]!=0 ||ctime_dat[5]!=0||ctime_dat[6]!=0||ctime_dat[7]!=0)\n");
        return -1;
    }

    memcpy(&my_time, ctime_dat, 4);
    sub_sec= my_time - curr_time;
    if (sub_sec < 0) 
    {
        sub_sec = -sub_sec;
    }
    if (sub_sec > 5)
    {
        WIFI_ERROR("[SDK] sub_sec > 5\n");
        return -1;
    }

    regi =(long)dat;
    ioctl(fd, ENCRPTGET_DEVINFO, regi); 

    memcpy(pInfo, dat, 32);
    get_factory_data(&dat[0], outdat);
    get_factory_data(&dat[16], outdat);
    pInfo[13] = outdat[3];
    pInfo[14] = outdat[2];
    g_nBoardID = (pInfo[13] << 8) | pInfo[14];

    get_factory_data(&dat[24], outdat);
    memcpy(&pInfo[24],outdat,8);
    if(!(((pInfo[24]>=1)&&(pInfo[24]<=31))&& \
         ((pInfo[25]>=0)&&(pInfo[25]<=23))&&  \
         ((pInfo[26]>=0)&&(pInfo[26]<=59))&&  \
         ((pInfo[27]>=0)&&(pInfo[27]<=59))))
    {
        return -1;        
    }
    pInfo[0] = pInfo[28];
    pInfo[1] = pInfo[29];
    pInfo[2] = pInfo[30];
    pInfo[3] = pInfo[31];
    get_factory_data(&dat[32], outdat);
    memcpy(&pInfo[32],outdat,8);
    get_factory_data(&dat[40], outdat);
    memcpy(&pInfo[40],outdat,8);
    get_factory_data(&dat[48], outdat);
    memcpy(&pInfo[48],outdat,8);

    close(fd);

    return 0;
}

//如果文件"/usr/local/etc/tutkuid.conf"不存在，则去根据加密提供的DID去生成该文件
static int get_uid()
{
    int i = 0;    
    int ret = 0;
    char result[128] = {0};
    char *presult = result;
    char test_dat[65] = {0};
    unsigned char data = 0;
    unsigned char data1 = 0;
    char tutkuid[64] = {0};
    FILE *file_new = NULL;

    if(0 == (access(UID_CFG_PATH, F_OK)))
    {
        //如果这个文件已存在则不需要从加密里获取，如果不存在有可能是恢复出厂时删掉了，为了后面ap热点名能读取到DID所以需要生成
        return 0;
    }

    ret = ShellCmdOut("fw_printenv -n uid" , result, sizeof(result)-1);
    if(ret)
    {
        printf("Get uid fail from env!\n");
        return -1;
    }
    
    if(strlen(result) < 20)
    {
        printf("uid raw data err! uid=%s\n", result);
        return -1;
    }
    
    for (i = 0; i < (sizeof(test_dat)-1); i++)
    {
        data = *presult++;
        data1 = *presult++; 
        data = data-0x30;
        data1 = data1-0x30;
        test_dat[i] = data|(data1<<4);
    }               
    
    char test_out_dat[64] = {0};
    char uid[64] = {0};

    get_factory_data(&test_dat[0], test_out_dat);
    get_factory_data(&test_dat[8], &test_out_dat[8]);
    get_factory_data(&test_dat[16], &test_out_dat[16]);
    get_factory_data(&test_dat[24], &test_out_dat[24]);
    get_factory_data(&test_dat[32], &test_out_dat[32]);
    
    if ((test_out_dat[0] == 20)
        || ((test_out_dat[0] == 26))
        || ((test_out_dat[0] == 33)))
    {
        for (i = 0; i < test_out_dat[0]; i++)
        {
            sprintf(tutkuid + i, "%c", test_out_dat[1+i]);  
        }
    }

    //保存至配置文件
    file_new = fopen(UID_CFG_PATH, "w");
    if (NULL == file_new)
    {
        printf("open file failed [%s]\n", UID_CFG_PATH);
        return -1;
    }

    char buff[2+64] = {0};
    snprintf(buff, sizeof(buff), "%d:%s", 1, tutkuid);
    fprintf(file_new, "%s", buff);
    fclose(file_new);
    
    return 0;
}

#endif

//从加密信息中读取UID
static int WifiDetect_GetUidFromEncryInfo(char *pUid)
{
    int i = 0;
    char szResult[128] = {0};
    char *pResult = szResult;
    char cTestData[65] = {0};
    unsigned char ucData = 0;
    unsigned char ucData1 = 0;
    char cTestOutData[64] = {0};

    if (NULL == pUid)
    {
        return -1;
    }

    memset(szResult, 0, sizeof(szResult));
    ShellCmdOut("fw_printenv -n uid", szResult, sizeof(szResult)-1);
    
    memset(cTestData, 0, sizeof(cTestData));
    for (i = 0; i < (sizeof(cTestData)-1); i++)
    {
        ucData = *pResult++;
        ucData1 = *pResult++; 
        ucData = ucData - 0x30;
        ucData1 = ucData1 - 0x30;
        cTestData[i] = ucData | (ucData1 << 4);
    }               
    
    memset(cTestOutData, 0, 64);
    get_factory_data(&cTestData[0], cTestOutData);
    get_factory_data(&cTestData[8], &cTestOutData[8]);
    get_factory_data(&cTestData[16], &cTestOutData[16]);
    get_factory_data(&cTestData[24], &cTestOutData[24]);
    get_factory_data(&cTestData[32], &cTestOutData[32]);
    
    if ((cTestOutData[0] == 20)
    || ((cTestOutData[0] == 26))
    || ((cTestOutData[0] == 33)))
    {
        for (i = 0; i < cTestOutData[0]; i++)
        {
            snprintf(pUid + i, 1, "%c", cTestOutData[1+i]);
        }
    }

    return 0;
}

unsigned AddressBytes(char const *address, unsigned char bytes[ADDRESS_BYTES])
{
    assert(strlen(address) == 2 * ADDRESS_BYTES);

    unsigned i = 0;
    unsigned j = 0;

    for ( ; i <2*ADDRESS_BYTES; ++i,++j)
    {
        // First character of a pair..
        if (isdigit(address[i]))
        {
            // A decimal digit encodes its ASCII value - '0'
            bytes[j] = address[i] - '0';
        }
        else if (isxdigit(address[i]))
        {
            // A hex digit encodes 10 + its ASCII value - 'A'
            bytes[j] = 10 + address[i] - 'A';
        }
        else
        {
            return 0;//Invalid hex
        }

        ++i;// Second character of a pair...
        bytes[j] <<= 4;//Shift the 1st character's value 1 nibble high

        // OR the 2nd character's value into the low nibble of the byte...
        if (isdigit(address[i]))
        {
            bytes[j] |= address[i] - '0';
        }
        else if (isxdigit(address[i]))
        {
            bytes[j] |= 10 + address[i] - 'A';
        }
        else
        {
            return 0;//Invalid hex
        }
    }
    return ADDRESS_BYTES;
}

/* 获取SD卡的序列号 */
static int GetSdCardSerialNum(char SerialNum[32])
{
    memset(SerialNum, 0, 32);

    FILE *fp = NULL;
    DIR *dp = NULL;
    char serial_num_node[64] = {0};
    struct dirent *entry = NULL;
    const char *sd_card_node_dir = "/sys/class/mmc_host/mmc1";

    if ((dp = opendir(sd_card_node_dir)) == NULL)
    {
        fprintf(stderr, "can't open %s\n", sd_card_node_dir);
        return -1;
    }

    while ((entry = readdir(dp)) != NULL)
    {
        if (strstr(entry->d_name, "mmc1:"))
        {
            snprintf(serial_num_node, sizeof(serial_num_node), "%s/%s/cid", 
                     sd_card_node_dir, entry->d_name);
            break;
       }
    }
    
    closedir(dp);
    dp = NULL;

    fp = fopen(serial_num_node, "r");
    if (NULL == fp)
    {
        fprintf(stderr, "can't open %s\n", serial_num_node);
        return -1;
    }
 
    if (NULL == fgets(SerialNum, 32, fp))
    {
        fprintf(stderr, "can't fgets %s\n", serial_num_node);
        fclose(fp);
        fp = NULL;
        return -1;
    }

    fclose(fp);
    fp = NULL;

    return 0;
}

/* 判断Mac地址是否合法, return 1:合法；return 0:非法 */
static int MacIsOK(unsigned char * pMac)
{
    int i = 0;
    int iRet = 0;
    int nInVailed = 0;
    unsigned int ucMac[6] = {0};

    iRet = sscanf(pMac, "%02x:%02x:%02x:%02x:%02x:%02x", 
        &ucMac[0], &ucMac[1], &ucMac[2], &ucMac[3], &ucMac[4], &ucMac[5]);
    for(i=0; i<6; i++)
    {
        if(ucMac[i]<0x0||ucMac[i]>0xff)
        {
            nInVailed =1;
            break;
        }
        nInVailed = 0;
    }

    if (nInVailed || 1 == (ucMac[0] % 2) || iRet!=6)
    {
        iRet =0;
    }
    else
    {
        iRet =1;
    }

    return iRet;
}

/*@ preif GetFileSystemType get the filesystem type of partion
 *@ param [in] dev_node partion dev node
 * @return     the filesystem type. detail for enum E_FILE_SYS_TYPE
 */
static E_FILE_SYS_TYPE GetFileSystemType(const char* dev_node)
{
    if (NULL == dev_node)
    {
        fprintf(stderr, "NULL POINTER EXCEPTION!\n");
        return FS_ERROR_TYPE;
    }

    int dev_fd = -1;
    unsigned char tmpBuffer[1024] = {0};
    unsigned char* pOffset = NULL;
    unsigned int readSize = 0;
    E_FILE_SYS_TYPE ret_type = FS_ERROR_TYPE;
    
    pOffset = &tmpBuffer[0];

    dev_fd = open(dev_node, O_RDONLY);
    if(!dev_fd)
    {
        ret_type = FS_ERROR_TYPE;
        fprintf(stderr, "open device %s failed!\n", dev_node);
        goto RET_EXIT;
    }

    // read the tag data for ntfs/fat32
    readSize = read(dev_fd, pOffset, 256);
    if (!readSize)
    {
        ret_type = FS_ERROR_TYPE;
        fprintf(stderr, "read device file failed!\n");
        goto RET_EXIT;
    }

    // the offset with 0x52 is fat32 tag;
    if (!memcmp((const void*)(pOffset+0x52), (const void*)"FAT32", 5))
    {
        ret_type = FS_FAT32;
        goto RET_EXIT;
    }
    // the offset with 0x3 is ntfs tag;
    else if (!memcmp((const void*)(pOffset+0x3), (const void*)"NTFS", 4))
    {
        ret_type = FS_NTFS;
        goto RET_EXIT;
    }
    // the offset with 0x3 is exfat tag;
    else if (!memcmp((const void*)(pOffset+0x3), (const void*)"EXFAT", 5))
    {
        ret_type = FS_EXFAT;
        goto RET_EXIT;
    }
    else
    {
        // seek to superblock1;
        lseek(dev_fd, sizeof(tmpBuffer), SEEK_SET);

        // read the superblock1 to buffer;
        memset(pOffset, 0x0, sizeof(tmpBuffer));
        readSize = read(dev_fd, pOffset, sizeof(tmpBuffer));
        if(!readSize)
        {
            ret_type = FS_ERROR_TYPE;
            goto RET_EXIT;
        }

        // the offset 0x438 is the tag of ext2:0x53 0xef;
        pOffset = pOffset + 0x38;
        if((0x53 == pOffset[0]) && (0xef == pOffset[1]))
        {
            ret_type = FS_EXT2;
            goto RET_EXIT;
        }
        else
        {
            printf("the offset 0x438 vaule is:%#x %#x!\n", pOffset[0], pOffset[1]);
        }
    }

RET_EXIT:
    if (dev_fd > 0)
    {
        close(dev_fd);
        dev_fd = -1;
    }
    return ret_type;
}

/*@ preif MountvFatNode 封装mount函数，只是针对于VFAT设备的挂载
 *@ param [in] dev 要挂载的目录
 *@ param [in] mnt_point 挂载点
 *@ return -1 挂载失败
 *@        0 挂载成功
 *@ note  较之mount命令，避免了针对异常SD卡挂载塞住的问题
 */
static int MountvFatNode(const char *dev, const char *mnt_point)
{
    if (!dev || !mnt_point)
    {
        DEBUGPRINTF("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    if (access(dev, F_OK) || access(mnt_point, F_OK))
    {
        DEBUGPRINTF("%s or %s not exist!\n", dev, mnt_point);
        return -1;
    }

    int ret = -1;
    E_FILE_SYS_TYPE fs_type = FS_ERROR_TYPE;
    fs_type = GetFileSystemType(dev);

    switch(fs_type)
    {
        case FS_EXT2:
            ret = mount((const char*)dev, (const char*)mnt_point, (const char*)"ext2", 0, "errors=continue");
            break;
        case FS_EXFAT:
            ret = mount((const char*)dev, (const char*)mnt_point, (const char*)"exfat", 0, "errors=continue");
            break;
        case FS_NTFS:
            ret = mount((const char*)dev, (const char*)mnt_point, (const char*)"ntfs3g", 0, "errors=continue");
            break;
        case FS_FAT32:
            ret = mount((const char*)dev, (const char*)mnt_point, (const char*)"vfat", 0, "errors=continue");
            break;
        default:
            DEBUGPRINTF("Unknown filesystem type!\n");
    }

    return ret;
}

int checkIPCUpgrade()
{
    int ret = 0;
    char buf[64] = {0};
    
    /* 检测SD卡是否是新卡，即是否存在mmcblkx节点 */
#ifdef  __USE_EMMC__
    if (0 == IsStringInCommand("ls /dev/mmcblk1*", SD_DEV_NODE_PART1))
#else
    if (0 == IsStringInCommand("ls /dev/mmcblk0*", SD_DEV_NODE_PART1))
#endif
    {
        DEBUGPRINTF("SD Card %s partition fail! exit!", SD_DEV_NODE_PART1);            
        return -1;
    }

    /* 存在mmcblkx节点,则进行挂载测试 */
    if (access("/tmp/app/disk", F_OK))
    {
        mysystem("mkdir /tmp/app/disk");
    }

    /* 进行挂载测试 */
    ret = MountvFatNode(SD_DEV_NODE_PART1, "/tmp/app/disk");
    if (ret)
    {
        DEBUGPRINTF("SD Card mount fail! need format sd card! format sd card!\n");
        return -1;
    }

    //判断上次是否已经升级
    //防止反复升级
    memset(buf, 0, sizeof(buf));
    snprintf(buf, sizeof(buf), "/tmp/app/disk/%s", SD_UPGRADE_FLAG_FILE_NAME);
    if (0 == access(buf, F_OK))
	{
	    memset(buf, 0, sizeof(buf));
        snprintf(buf, sizeof(buf), "rm -rf /tmp/app/disk/%s", SD_UPGRADE_FLAG_FILE_NAME);
        mysystem(buf);
        
        mysystem("umount /tmp/app/disk");
        return 1;
	}
	
    snprintf(buf, sizeof(buf), "/tmp/app/disk/%s", IPC_UPGRADE_FILE_NAME);
    if (access(buf, F_OK))
    {
        DEBUGPRINTF("IPC UPGRADE FILE not exist!\n");
        mysystem("umount /tmp/app/disk");
        return -1;
    }

    mysystem(DO_SD_UPGRADE);

    mysystem("umount /tmp/app/disk");

    return 0;
}

/*从min和max中返回一个随机值*/
static int RandNumGenerator(int min, int max)
{
    int dev_random_fd = -1;
    char *next_random_byte = NULL;
    int bytes_to_read = 0;
    unsigned random_value = 0;

    assert(max > min);

    if (dev_random_fd == -1)
    {
        dev_random_fd = open("/dev/urandom", O_RDONLY);
        assert(dev_random_fd != -1);
    }

    next_random_byte = (char *)&random_value;
    bytes_to_read = sizeof(random_value);

    /* 因为是从/dev/random中读取，read可能会被阻塞，一次读取可能只能得到一个字节，
     * 循环是为了让我们读取足够的字节数来填充random_value.
     * 这里，防止被阻塞，采用读取/dev/urandom的方法
     */
    do
    {
        int bytes_read = 0;
        bytes_read = read(dev_random_fd, next_random_byte, bytes_to_read);
        bytes_to_read -= bytes_read;
        next_random_byte += bytes_read;
    }while(bytes_to_read > 0);

    close(dev_random_fd);
    dev_random_fd = -1;

    return (min + (random_value % (max - min + 1)));
}

/* 根据时间随机生成MAC地址 */
static int GetRandomMacAddr(unsigned char acMac[MAC_ADDR_LEN])
{
    unsigned char bytes[ADDRESS_BYTES] = {0};
    unsigned char env_enetaddr[MAC_ADDR_LEN] = {0};

#if 0
    unsigned char cmd_out[MAC_ADDR_LEN*2+1] = {0};
    unsigned char address[ADDRESS_BYTES*2] = {0};

    /* 获取linux系统的DID号 */
    ShellCmdOut(GET_RANDOM_DID_NUM, cmd_out, sizeof(cmd_out));

    /* 拷贝前8个字符 */
    memcpy(address, cmd_out, sizeof(address));

    /* 每两个字符作为一个十六进制存放 */
    AddressBytes(address, bytes);
#endif

    int i = 0;

    for (i=0; i<4; i++)
    {
        bytes[i] = (unsigned char)RandNumGenerator(0, 255);
    }

    env_enetaddr[0] = 0x5a;
    env_enetaddr[1] = 0x00;
    env_enetaddr[2] = env_enetaddr[1] ^ bytes[0];
    env_enetaddr[3] = env_enetaddr[2] ^ bytes[1];
    env_enetaddr[4] = env_enetaddr[3] ^ bytes[2];
    env_enetaddr[5] = env_enetaddr[4] ^ bytes[3];

    acMac[0] = env_enetaddr[0]; 
    acMac[1] = env_enetaddr[1]; 
    acMac[2] = env_enetaddr[2]; 
    acMac[3] = env_enetaddr[3]; 
    acMac[4] = env_enetaddr[4]; 
    acMac[5] = env_enetaddr[5]; 

    return 0;
}

#if defined (__IS_CUSTOM_LS__)
static void GetLongSeeDeviceInfo(unsigned char *sn,  unsigned char *mac)
{
    FILE *fp = NULL;
    char szBuf[128] = {0};
    char *fpstart = NULL;
    char *fpend = NULL;

    if(NULL == sn || NULL == mac)
    {
        return;
    }
    
    if(0 == access(LONGSEE_DEVICE_INFO,0))
    {
        if (NULL == (fp = fopen(LONGSEE_DEVICE_INFO,"r")))
        {
            perror("fopen");
            return;
        }
        
        if (NULL == fgets(szBuf, sizeof(szBuf), fp))
        {
            perror("fgets");
            fclose(fp);
			fp = NULL;
            return;
        }        

        fpstart = szBuf;
        fpend = strchr(szBuf, '^');
        memcpy(sn, fpstart,  fpend - fpstart);
        memcpy(mac, fpend + 1 , MAX_SERIAL_NUM_LEN);

        fflush(fp);
		fclose(fp);
		fp = NULL;
    }
}
#endif
#ifdef __UBOOT_RECOVERY__
void checkBootEnvMac(char *sys_mac)
{
	char env_mac[32] = {0};
	char cmd[256] = {0};
	FILE *fp = NULL;
	
	memset(env_mac, 0, sizeof(env_mac));     
	if (NULL == (fp = popen(CMD_GET_MAC, "r")))
	{
		perror("popen");
	}

	if(NULL != fp && NULL == fgets(env_mac, MAX_LEN, fp))
	{
		perror("fgets");
		pclose(fp);
		fp = NULL;
	}
	
	env_mac[MAX_LEN] = '\0';

	if(NULL != fp)
	{
		pclose(fp);
	}
	
	if (strncasecmp(sys_mac, env_mac, 17))
	{
		printf("[UBOOT_RECOVERY] Set Boot Env MAC: %s\n", sys_mac);
		snprintf(cmd, sizeof(cmd), "%s %s", CMD_SET_MAC, sys_mac);
		printf("%s\n", cmd);
		mysystem(cmd);
	}
	
	return;
}
#endif
int setMac(void)
{
    char szMacBuf[32] = {0};
    char szCommand[256] = {0};
    char szBuf[MAX_LEN] = {0};
    unsigned char szMacAddr[MAC_ADDR_LEN] = {0};
    FILE *fp = NULL;

/*
 * 之前的逻辑也不会将mac地址保存到uboot中，屏蔽并不影响之前的逻辑
 * 支持uboot恢复功能后，uboot会同步使用设备的mac地址，所以屏蔽该代码
 * 防止uboot的mac地址影响设备的mac地址
 */
#ifndef __UBOOT_RECOVERY__
    /* 如果保存MAC地址的配置文件存在,则将MAC地址设置到u-boot中 */
    if(!access(MAC_FILE,0)) 
    {
        WIFI_INFO("%s exist! reload the mac to u-boot!\n", MAC_FILE);
        if (NULL == (fp = fopen(MAC_FILE,"r")))
        {
            perror("fopen");
            return-1;
        }
        if (NULL == fgets(szBuf, MAX_LEN, fp))
        {
            perror("fgets");
            fclose(fp);
            return-1;
        }
        snprintf(szCommand, sizeof(szCommand), "rm -vf %s", MAC_FILE);
        mysystem(szCommand);
    }
    else//如果没有升级前保存的配置，则从boot环境变量获取是否有用户设置的MAC地址
    {
		memset(szBuf, 0, sizeof(szBuf));     
    	memcpy(szBuf, INIT_MAC, sizeof(szBuf));
		szBuf[MAX_LEN] = '\0';
    }
#endif

	#if defined (__IS_CUSTOM_LS__)
    unsigned char acLongSeeSer[MAX_SERIAL_NUM_LEN] = {0};
    unsigned char acLongSeeMac[MAX_SERIAL_NUM_LEN] = {0};
    GetLongSeeDeviceInfo(acLongSeeSer, acLongSeeMac);
    if(strlen(acLongSeeMac) > 0)
    {
        snprintf(szBuf, sizeof(szBuf), "%s", acLongSeeMac);
    }
	#endif

    // 查看u-boot下的mac地址是否为初始值(00:00:00:00:00:00)或者非法，如果是，则从加密芯片获取信息
    if ((0 == MacIsOK((unsigned char*)szBuf)) 
     || (0 == strncasecmp(INIT_MAC, szBuf, MAX_LEN-1))) 
    {
        WIFI_INFO("Mac Address %s invailed, Resetting it now!\n", szBuf);
        mysystem(CLOSE_ETH0_NET);
        memset(szCommand, 0, sizeof(szCommand));
#ifdef __AUTHORIZE__
        /* 如果授权成功，则直接从授权里面解析出MAC地址 */
        fprintf(stderr, "[%s][%d] **zxr dwAuthVerifyErrno=[%d]**\r\n", __FUNCTION__, __LINE__, m_stuAuthInfo.dwAuthVerifyErrno);        
        fprintf(stderr, "[%s][%d] **zxr szMac=[%s]**\r\n", __FUNCTION__, __LINE__, m_stuAuthInfo.szMac);
        if (AUTH_SUCCESS == m_stuAuthInfo.dwAuthVerifyErrno)
        {
            memcpy(szMacAddr, m_stuAuthInfo.szMac, MAC_ADDR_LEN);
            snprintf(szMacBuf, sizeof(szMacBuf), "%02x:%02x:%02x:%02x:%02x:%02x",
                         szMacAddr[0], szMacAddr[1], szMacAddr[2], 
                         szMacAddr[3], szMacAddr[4], szMacAddr[5]);

            /* 保存当前的MAC地址，后续需要使用 */
            memset(szCommand, 0, sizeof(szCommand));
            snprintf(szCommand, sizeof(szCommand), "echo %s > %s", szMacBuf, AUTH_MAC_FILE);
            mysystem(szCommand);

            memset(szCommand, 0, sizeof(szCommand));
            snprintf(szCommand, sizeof(szCommand), "ifconfig eth0 hw ether %s", szMacBuf);
        }
        else
        {
            if (access(RAND_MAC_FILE, F_OK))
            {
                /* 根据时间随机生成MAC地址 */
                GetRandomMacAddr(szMacAddr);
                snprintf(szMacBuf, sizeof(szMacBuf), "%02x:%02x:%02x:%02x:%02x:%02x",
                         szMacAddr[0], szMacAddr[1], szMacAddr[2], 
                         szMacAddr[3], szMacAddr[4], szMacAddr[5]);

                /* 需要把随机产生的MAC地址保存起来，下次启动还是这个MAC地址 */
                memset(szCommand, 0, sizeof(szCommand));
                snprintf(szCommand, sizeof(szCommand), "echo %s > %s", szMacBuf, RAND_MAC_FILE);
                mysystem(szCommand);
            }
            else
            {
                fp = fopen(RAND_MAC_FILE, "r");
                if (fp)
                {
                    fgets(szMacBuf, sizeof(szMacBuf), fp);
                    fclose(fp);
                    fp = NULL;
                }
                if (strlen(szMacBuf) <= 2)
                {
                    unlink(RAND_MAC_FILE);
                }
            }
            snprintf(szCommand, sizeof(szCommand), "ifconfig eth0 hw ether %s", szMacBuf);
        }

#else
        snprintf(szCommand, sizeof(szCommand), "ifconfig eth0 hw ether %02x:%02x:%02x:%02x:%02x:%02x", 
             0x5a, 0x0, g_HilEncrypt[0], g_HilEncrypt[1], g_HilEncrypt[2], g_HilEncrypt[3]);
#endif
        mysystem(szCommand);
        WIFI_DEBUG("%s\n", szCommand);
        mysystem(OPEN_ETH0_NET);
    }
    else
    {
        mysystem(CLOSE_ETH0_NET);
        memset(szCommand, 0, sizeof(szCommand));
        snprintf(szCommand, sizeof(szCommand), "ifconfig eth0 hw ether %s", szBuf);
        mysystem(szCommand);
        mysystem(OPEN_ETH0_NET);
    }
	
#ifdef __UBOOT_RECOVERY__
	checkBootEnvMac(szMacBuf);
#endif

    return 0;
}

/* 由网口if_name的MAC组合成AP的SSID号 */
static int WifiDetect_FormatSoftapSSID(char *if_name, char *ssid)
{
    struct ifreq ifr = {0};
    int sock = -1; 
    int ret = 0;
    unsigned char *ptr = NULL;

    if ((!if_name) || (!ssid))
    {
        return EFAULT;
    }
    
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) 
    {
        return errno;
    }
    
    strncpy(ifr.ifr_name, if_name, 16);
    if (ioctl(sock, SIOCGIFHWADDR, &ifr) >= 0)
    {
        ptr = (unsigned char *) & ifr.ifr_ifru.ifru_hwaddr.sa_data[0];
        snprintf(ssid, 32, "%s%02X%02X%02X%02X%02X%02X", 
                 g_szDefaultApPrefix, *ptr, *(ptr + 1), *(ptr + 2), *(ptr + 3), *(ptr + 4), *(ptr + 5));    
    }
    else
    {
        ret = errno;
    }

    close(sock);

    return ret;
}

//格式化SSID
static int WifiDetect_SoftapSsidFormat()
{
    int ret = 0;
    int bEnable = 0;
    char szBuf[128] = {0};
    char szUidReal[128] = {0};
    char szUid[128] = {0};
    char *szUidStartFlag = NULL;
    
    memset(szUidReal, 0, sizeof(szUidReal));

    /* AP模式的SSID由"g_szDefaultApPrefix + UID"组成 */
    
    //从文件中读UID
    FILE *file_new = fopen(UID_CFG_PATH, "r");
    if (file_new == NULL)
    {
        WIFI_ERROR("open file failed [%s]\n", UID_CFG_PATH);
    }
    else
    {
        fgets(szBuf, sizeof(szBuf), file_new);
        fclose(file_new);
        file_new = NULL;

        szUidStartFlag = strstr(szBuf, ":");
        if (NULL == szUidStartFlag)
        {
            memcpy(szUidReal, szBuf, sizeof(szUidReal));
        }
        else
        {
            memcpy(szUidReal, szUidStartFlag+1, sizeof(szUidReal));
        }
    }

    //从加密信息中读UID
    if (strlen(szUidReal) < 20)
    {
#ifdef __AUTHORIZE__
        if (AUTH_SUCCESS == m_stuAuthInfo.dwAuthVerifyErrno)
        {
            strncpy(szUidReal, m_stuAuthInfo.struCloudInfo.szDid, MAX_DID_NUM_LEN);
            WIFI_DEBUG("szUidReal:%s\n", szUidReal);
        }
#else
        WifiDetect_GetUidFromEncryInfo(szUidReal);
        WIFI_DEBUG("szUidReal:%s\n", szUidReal);
#endif
    }

    if (strlen(szUidReal) < 20)
    {
        goto DEFAULT_SSID;
    }
    else
    {
        //尚云DID号在逗号前面
        char *szUidEndFlag = strstr(szUidReal, ",");
        memset(g_nSoftApSsid, 0, sizeof(g_nSoftApSsid));
        if (NULL == szUidEndFlag)
        {
            snprintf(g_nSoftApSsid, sizeof(g_nSoftApSsid), "%s%s", g_szDefaultApPrefix, szUidReal);
        }
        else
        {
            strncpy(szUid, szUidReal, szUidEndFlag-szUidReal);
            snprintf(g_nSoftApSsid, sizeof(g_nSoftApSsid), "%s%s", g_szDefaultApPrefix, szUid);
        }

        goto NEXT;
    }
    
DEFAULT_SSID:
    memset(g_nSoftApSsid, 0, sizeof(g_nSoftApSsid));
    ret = WifiDetect_FormatSoftapSSID("eth0", g_nSoftApSsid);
    if (ret)
    {
		ret = WifiDetect_FormatSoftapSSID("wlan0", g_nSoftApSsid);
        if (ret)
        {
            WIFI_ERROR("Fail to called WifiDetect_FormatSoftapSSID\n");
        }
    }

NEXT:
    WIFI_DEBUG("g_nSoftApSsid:%s\n", g_nSoftApSsid);

    return ret;
}

/* 设置软AP的帐号和密码(帐号固定，密码可以修改) */
static int WifiDetect_SetSoftApCFG(const char *pPasswd)
{    
    char szCommand[1024] = {0};

    if (!pPasswd)
    {
        WIFI_ERROR("NULL POINTER EXCEPTION!\n");
        return -1;
    }
    
    if ('\0' == g_nSoftApSsid[0])
    {
        WIFI_ERROR("SoftAP SSID is Null!\n");
        return -1;
    }

    if (0 == strlen(pPasswd))
    {
        snprintf(szCommand, sizeof(szCommand), INet_AP_CONF_NO_PWD_FORMAT, g_nSoftApSsid, HOSTAPD_CONF_FILE_PATH);
        mysystem(szCommand);
    }
    else
    {
        snprintf(szCommand, sizeof(szCommand), INet_AP_CONF_FORMAT, g_nSoftApSsid, g_nSoftApPasswd, HOSTAPD_CONF_FILE_PATH);
        mysystem(szCommand);
    }
    memset(szCommand, 0, sizeof(szCommand));
    snprintf(szCommand, sizeof(szCommand), "hostapd -e /tmp/entropy.bin %s -B", HOSTAPD_CONF_FILE_PATH);
    mysystem(szCommand);
    
    memset(szCommand, 0, sizeof(szCommand));
    snprintf(szCommand, sizeof(szCommand), "udhcpd -fS %s &", UDHCPD_CONF);
    mysystem(szCommand);

    WIFI_INFO("SSID  : %s\n", g_nSoftApSsid);
    WIFI_INFO("PASSWD: %s\n", strlen(pPasswd)?pPasswd:"no passwd");
    
    WIFI_INFO("Set SoftAP over!\n");
    return 0;
}

/* 加载SoftAp模式的驱动:
 * 4. 加载rtutil7601Uap.ko; 
 * 5. 加载mt7601Uap.ko;
 * 6. 加载rtnet7601Uap.ko;
 * 7. 启动ra0网卡;
 * 8. 配置ra0 ip地址;
 */
static int WifiDetect_LoadSoftApDriver(void)
{
    char szCommand[256] = {0};

    WIFI_INFO("======Into Load WIFI AP Mode======\n");
    
    //snprintf(command, sizeof(command), "rm -rf %s", WIFI_EXIST_FlAG);
    //mysystem(command);

    mysystem("ifconfig wlan0 up");

    memset(szCommand, 0, sizeof(szCommand));
    snprintf(szCommand, sizeof(szCommand), "echo WlanNode=wlan0 >> %s", WIFI_EXIST_FlAG);
    mysystem(szCommand);

    memset(szCommand, 0, sizeof(szCommand));
    snprintf(szCommand, sizeof(szCommand), "ifconfig wlan0 %s netmask %s", g_szDefaultApIp, g_szDefaultApNetmask);
    mysystem(szCommand);
    
    memset(szCommand, 0, sizeof(szCommand));
    snprintf(szCommand, sizeof(szCommand), "route add default gw %s", g_szDefaultApGateway);
    mysystem(szCommand);
    
    usleep(500000);

    /* 配置用户名和密码 */
    memset(g_nSoftApSsid, 0, sizeof(g_nSoftApSsid));
    WifiDetect_SoftapSsidFormat();
    WifiDetect_SetSoftApCFG(g_nSoftApPasswd);
    
    WIFI_INFO("****** Configure WIFI Soft Ap Mode Ok! ******\n");

    return 0;
}

/* 卸载SoftAp模式的驱动 */
static int WifiDetect_UnloadSoftApDriver(void)
{
    if (IsStringInCommand("ps", "hostapd"))
    {
        mysystem("killall -9 hostapd");
    }
    
    if (IsStringInCommand("ps", "udhcpd"))
    {
        mysystem("killall -9 udhcpd");
    }

    mysystem("ifconfig wlan0 down");

    WIFI_INFO("======Remove WIFI AP Mode Over======\n");
    return 0;
}

/* 加载STA模式的驱动:
 * 6. 加载mt7601Usta驱动; 
 * 7. 启动wlan0网卡;
 * 8. 配置wlan0 ip地址;
 * 9. 配置WPA_supplicant;
 */
static int WifiDetect_LoadStaModeDriver(void)
{
    char szCommand[256] = {0};
    char szWifiModeName[64] = {0};

    WIFI_INFO("======Into Load WIFI STA Mode======\n");

    if (MT7601_DEV == g_eCurrUSBWifiDev)
    {
        strncpy(szWifiModeName, "mt7601Usta", sizeof(szWifiModeName));
    }
    else if (RTL8188FTV_DEV == g_eCurrUSBWifiDev)
    {
        strncpy(szWifiModeName, "8188fu", sizeof(szWifiModeName));
    }

	if (MT7601_DEV == g_eCurrUSBWifiDev)
    {
		if (0 == IsStringInCommand("lsmod", "mtprealloc"))
	    {
	        memset(szCommand, 0, sizeof(szCommand));
	        snprintf(szCommand, sizeof(szCommand), "insmod %s/%s.ko", USB_WIFI_DRV_PATH, "mtprealloc");
	        mysystem(szCommand); 
	        usleep(100000);
	    }
		
	    if (0 == IsStringInCommand("lsmod", szWifiModeName))
	    {
	        memset(szCommand, 0, sizeof(szCommand));
	        snprintf(szCommand, sizeof(szCommand), "insmod %s/%s.ko", USB_WIFI_DRV_PATH, szWifiModeName);
	        mysystem(szCommand); 
	        usleep(100000);
	    }
	}
	else if (RTL8188FTV_DEV == g_eCurrUSBWifiDev)
	{
		if (0 == IsStringInCommand("lsmod", szWifiModeName))
	    {
	        memset(szCommand, 0, sizeof(szCommand));
	        snprintf(szCommand, sizeof(szCommand), "insmod %s/%s.ko", USB_WIFI_DRV_PATH, szWifiModeName);
	        mysystem(szCommand); 
	        usleep(100000);
	    }
	}

    mysystem("ifconfig wlan0 up");

    memset(szCommand, 0, sizeof(szCommand));
    snprintf(szCommand, sizeof(szCommand), "echo WlanDev=%s > %s", szWifiModeName, WIFI_EXIST_FlAG);
    mysystem(szCommand);
    memset(szCommand, 0, sizeof(szCommand));
    snprintf(szCommand, sizeof(szCommand), "echo WlanNode=wlan0 >> %s", WIFI_EXIST_FlAG);
    mysystem(szCommand);

    // 运行wpa_supplicant程序
    if (0 == IsStringInCommand("ps", "wpa_supplicant"))
    {	
		mysystem(WPA_SUPPLICANT);
    }

    WIFI_INFO("****** Configure WIFI STA Mode Ok! ******\n");

    return 0;
}

/* 卸载STA模式的驱动*/
static int WifiDetect_UnloadStaModeDriver(void)
{
    if (IsStringInCommand("ps", "wpa_supplicant"))
    {
        mysystem("killall -9 wpa_supplicant");
    }

    if (IsStringInCommand("ifconfig -a", "wlan0"))
    {
        mysystem("ifconfig wlan0 down");
    }

    if (MT7601_DEV == g_eCurrUSBWifiDev)
    {
		if (IsStringInCommand("lsmod", "mt7601Usta"))
        {
            mysystem("rmmod mt7601Usta");
        }
		
		if (IsStringInCommand("lsmod", "mtprealloc"))
        {
            mysystem("rmmod mt7601Usta");
        } 
    }
    else if (RTL8188FTV_DEV == g_eCurrUSBWifiDev)
    {
        if (IsStringInCommand("lsmod", "8188fu"))
        {
            mysystem("rmmod 8188fu");
        }
    }

    WIFI_INFO("======Remove WIFI STA Mode Over======\n");
    return 0;
}

/*-****************************************************************** 
去掉字符串首尾的  空格,回车、换行 \r \n 字符
-------------------------- 
参数 pcInOutStr[inout]:输入输出字符串
-------------------------- 
返回 ：无
*******************************************************************-*/ 
static void __TrimSpace(char *pcInOutStr)
{
	char *pcStart = pcInOutStr - 1;
	char *pcEnd = pcInOutStr;
	char *pcInStr = pcInOutStr;

	while(*pcInStr)
	{
		switch(*pcInStr)
		{
			case ' ':
			case '\r':
			case '\n':
				if(pcStart + 1 == pcInStr)
				{
					pcStart = pcInStr;
				}
				break;

			default: 
				break;
		}
		++pcInStr;
	}

	//现在来到了字符串的尾部 反向向前
	--pcInStr;
	++pcStart;

	if(*pcStart == 0)
	{	
		//已经到字符串的末尾了
		*pcInOutStr = 0;
		
		return;
	}

	pcEnd = pcInStr + 1;
	while(pcInStr > pcStart)
	{
		switch(*pcInStr)
		{
			case ' ':
			case '\r':
			case '\n':
				if(pcEnd - 1 == pcInStr)
				{
					pcEnd = pcInStr;
				}
				break;

			default:   
				break;
		}
		--pcInStr;
	}
	
	memmove(pcInOutStr, pcStart, pcEnd - pcStart);
	*(pcInOutStr + (pcEnd - pcStart)) = 0;
}

static int sensor_hwconf_get_id(char *pszSensorName, int NameSizeMax)
{
	char strResult[128]={0};

	if (NULL == pszSensorName ||NameSizeMax <= 0)
	{
		return -1;
	}

	if (ShellCmdOut("awk '/SENSOR:/ {print $2}' /proc/xrs/hwconf", strResult, sizeof(strResult)))
	{
		printf("Fail to get sensor name from probe file\n");
		return -1;
	}

    __TrimSpace(strResult);
    memcpy(pszSensorName, strResult, strlen(strResult));
    
	return 0;
}

static int AdjustSensorType(void)
{
	int iRet = 0;
	char acSensorName[32] = {0};
	FILE *fp = NULL;

	iRet = sensor_hwconf_get_id(acSensorName, sizeof(acSensorName));
	if (0 == iRet)
	{
		fp = fopen(SENSOR_NAME_PATH, "w+");
		if (NULL == fp)
		{
			printf("Fail to fopen %s\n", SENSOR_NAME_PATH);
			return -1;
		}
		
		fwrite(acSensorName, strlen(acSensorName), 1, fp);//写入需要写的那一行
		printf("SensorName=[%s]\n", acSensorName);
		fclose(fp);

		return 0;
	}
	else
	{
		printf("Fail to called sensor_user_i2c_get_id\n");
		return -1;
	}

	return 0;
}

/*
Function1: 带“id”参数:解析出版型并生成一个含版型的隐藏文件，为了方便海思加载驱动时用
Function2: 不带参数:设置MAC地址，加载wifi驱动，检测boa配置文件的头尾是否完整
Function3: 带“noInsmodWifi”参数:设置MAC地址，检测boa配置文件的头尾是否完整
Function4: 带“sdUpgrade”参数，检查SD卡是否有升级包并升级
Function5: 带“getDID”参数：如果文件"/usr/local/etc/tutkuid.conf"不存在，则去根据加密提供的DID去生成该文件
*/
int main(int argc, char *argv[])
{
    int i = 0;
    int iRet = 0;
    char szBuf[512] = {0};
    char szCommand[256] = {0};
    char szCommand1[256] = {0}; 
    char szCommand2[256] = {0};
    FILE *fp = NULL;
    int macSetFlag=0;
    int boardID=0;
    eWifiMode enWifiMode = WLAN_STA_MODE;
    int nLastWlanwifimode = 0;
    char szLastSetValue[128] = {0};
    stuUSBDevInfo USBDevInfo[MAX_USB_DEV];
    int usb_dev_count = 0;
    char szStrMac[32] = {0};
    unsigned char szMacAddr[MAC_ADDR_LEN] = {0};

    AdjustSensorType();

#ifdef __AUTHORIZE__
    MAUTH_DEV_ATTR_S stMauthDevAttr;
    memset(&stMauthDevAttr, 0, sizeof(MAUTH_DEV_ATTR_S));

	iRet = MAuth_Dev_Open();
    iRet |= MAuth_Dev_GetDevAttr(0, &stMauthDevAttr);
    iRet |= MAuth_Dev_Close(0);
    if (iRet != 0)
    {
        WIFI_ERROR("Fail to called MAuth_Dev_GetDevAttr\n");
    }

    if (strlen(stMauthDevAttr.AuthCode) != 0)
    {
        DeEncryptLicense(stMauthDevAttr.AuthCode, &m_stuAuthInfo);
    }
    else
    {
        m_stuAuthInfo.dwAuthVerifyErrno = AUTH_NOT_AUTHORIZE;
    }

    //解析出版型并生成一个含版型的隐藏文件，为了方便海思加载驱动时用
    if((2 == argc) && (0==strcasecmp(argv[1], "getDID")))
    {    
        //如果这个文件已存在则不需要从加密里获取，如果不存在有可能是恢复出厂时删掉了，为了后面ap热点名能读取到DID所以需要生成
        if(0 == (access(UID_CFG_PATH, F_OK)))
        {
            return 0;
        }

        //保存至配置文件
        FILE *file_new = fopen(UID_CFG_PATH, "w");
        if (NULL == file_new)
        {
            WIFI_ERROR("open file failed [%s]\n", UID_CFG_PATH);
            return -1;
        }

        char buff[2+64] = {0};
        snprintf(buff, sizeof(buff), "%d:%s", 1, m_stuAuthInfo.struCloudInfo.szDid);
        fprintf(file_new, "%s", buff);
        fclose(file_new);

        return 0;
    }
    else if ((2 == argc) && (0==strcasecmp(argv[1], "setMac")))
    {
		/*
		 * 之前的逻辑也不会将mac地址保存到uboot中，屏蔽并不影响之前的逻辑
		 * 支持uboot恢复功能后，uboot会同步使用设备的mac地址，所以屏蔽该代码
		 * 防止uboot的mac地址影响设备的mac地址
		 */
#ifndef __UBOOT_RECOVERY__
        //cat /onvif/wifi/hisilicon/wifi_cfg | sed "s/CFG_MAC=.*;/CFG_MAC=5A:58:56:58:57:12;/g" > /tmp/wifi_cfg
        /* 如果保存MAC地址的配置文件存在,则将MAC地址设置到u-boot中 */
        if (!access(MAC_FILE,0)) 
        {
            WIFI_INFO("%s exist! reload the mac to u-boot!\n", MAC_FILE);
            if (NULL == (fp = fopen(MAC_FILE,"r")))
            {
                perror("fopen");
                return -1;
            }
            if (NULL == fgets(szBuf, MAX_LEN, fp))
            {
                perror("fgets");
                fclose(fp);
                return -1;
            }
            snprintf(szCommand, sizeof(szCommand), "rm -vf %s", MAC_FILE);
            mysystem(szCommand);
        }
        else//如果没有升级前保存的配置，则从boot环境变量获取是否有用户设置的MAC地址
        {
            memset(szBuf, 0, sizeof(szBuf));     
			
			/* 如果授权成功，则直接从授权里面解析出MAC地址 */
            if (AUTH_SUCCESS == m_stuAuthInfo.dwAuthVerifyErrno)
            {
                memcpy(szBuf, m_stuAuthInfo.szMac, MAC_ADDR_LEN);
            }
			else
			{
				/* 根据时间随机生成MAC地址 */
				GetRandomMacAddr(szMacAddr);
				snprintf(szBuf, sizeof(szBuf), "%02x:%02x:%02x:%02x:%02x:%02x",
					szMacAddr[0], szMacAddr[1], szMacAddr[2], 
					szMacAddr[3], szMacAddr[4], szMacAddr[5]);
			}
            
            szBuf[MAX_LEN] = '\0';
        }
#endif
        // 查看u-boot下的mac地址是否为初始值(00:00:00:00:00:00)或者非法，如果是，则从加密芯片获取信息
        if ((0 == MacIsOK((unsigned char*)szBuf)) 
            || (0 == strncasecmp(INIT_MAC, szBuf, MAX_LEN-1))) 
        {
            WIFI_INFO("Mac Address %s invailed, Resetting it now!\n", szBuf);
            
#ifdef __AUTHORIZE__
            /* 如果授权成功，则直接从授权里面解析出MAC地址 */
            if (AUTH_SUCCESS == m_stuAuthInfo.dwAuthVerifyErrno)
            {
                memcpy(szMacAddr, m_stuAuthInfo.szMac, MAC_ADDR_LEN);
                szMacAddr[1] = 0x59;
                snprintf(szStrMac, sizeof(szStrMac), "%02x:%02x:%02x:%02x:%02x:%02x", 
                    szMacAddr[0], szMacAddr[1], szMacAddr[2], 
                    szMacAddr[3], szMacAddr[4], szMacAddr[5]);
                snprintf(szCommand, sizeof(szCommand), "cat /onvif/wifi/hisilicon/wifi_cfg_bak | sed \"s/CFG_MAC=.*;/CFG_MAC=%s;/g\" > /tmp/wifi_cfg", 
                    szStrMac);
            }
            else
            {
                if (access(RAND_MAC_FILE, F_OK))
                {
                    /* 根据时间随机生成MAC地址 */
                    GetRandomMacAddr(szMacAddr);
                    snprintf(szStrMac, sizeof(szStrMac), "%02x:%02x:%02x:%02x:%02x:%02x",
                        szMacAddr[0], szMacAddr[1], szMacAddr[2], 
                        szMacAddr[3], szMacAddr[4], szMacAddr[5]);
                    /* 需要把随机产生的MAC地址保存起来，下次启动还是这个MAC地址 */
                    memset(szCommand, 0, sizeof(szCommand));
                    snprintf(szCommand, sizeof(szCommand), "echo %s > %s", szStrMac, RAND_MAC_FILE);
                    mysystem(szCommand);
                }
                else
                {
                    fp = fopen(RAND_MAC_FILE, "r");
                    if (fp)
                    {
                        fgets(szStrMac, sizeof(szStrMac), fp);
                        fclose(fp);
                        fp = NULL;
                    }
                }

                //避免从文件中读出的mac地址字符串末尾带有非法字符
                //影响后续程序执行
                szStrMac[strlen("5a:00:92:04:49:ce")] = '\0';
                snprintf(szCommand, sizeof(szCommand), "cat /onvif/wifi/hisilicon/wifi_cfg_bak | sed \"s/CFG_MAC=.*;/CFG_MAC=%s;/g\" > /tmp/wifi_cfg", 
                    szStrMac);
            }
#else
            snprintf(szCommand, sizeof(szCommand), "cat /onvif/wifi/hisilicon/wifi_cfg_bak | sed \"s/CFG_MAC=.*;/CFG_MAC=%02x:%02x:%02x:%02x:%02x:%02x;/g\" > /tmp/wifi_cfg", 
                0x5a, 0x0, g_HilEncrypt[0], g_HilEncrypt[1], g_HilEncrypt[2], g_HilEncrypt[3]);
#endif
            //set mac
            mysystem(szCommand);
            WIFI_DEBUG("%s\n", szCommand);
        }
        else
        {
            //set mac
            memset(szCommand, 0, sizeof(szCommand));
            //避免从文件中读出的mac地址字符串末尾带有非法字符
            //影响后续程序执行
            szBuf[strlen("5a:00:92:04:49:ce")] = '\0';
            snprintf(szCommand, sizeof(szCommand), "cat /onvif/wifi/hisilicon/wifi_cfg_bak | sed \"s/CFG_MAC=.*;/CFG_MAC=%s;/g\" > /tmp/wifi_cfg", 
                    szBuf);
            mysystem(szCommand);
        }
#ifdef __UBOOT_RECOVERY__
		checkBootEnvMac(szStrMac);
#endif
        return 0;
    }

    if (AUTH_SUCCESS == m_stuAuthInfo.dwAuthVerifyErrno)
    {
        boardID = m_stuAuthInfo.dwBoardID;
    }
    else
    {
        boardID = GetDefaultBoardID();
    }

    memset(szCommand, 0, sizeof(szCommand));
    snprintf(szCommand, sizeof(szCommand), "echo %d > %s", boardID, DEVICE_BOARD_TYPE);
    mysystem(szCommand);

    memset(USBDevInfo, 0, sizeof(USBDevInfo));
    
    if (DeviceUSBDevInfo(USBDevInfo, &usb_dev_count))
    {
        WIFI_ERROR("Failed to call DeviceUSBDevInfo\n");
    }
    else
    {
        for (i=0; i<usb_dev_count; i++)
        {
            if ((0 == strcmp(USBDevInfo[i].strManufacturer, "MediaTek")) 
             || (0x7601 == USBDevInfo[i].nIdProduct))
            {
                g_eCurrUSBWifiDev = MT7601_DEV;
                WIFI_INFO("Detect this IP Camera has mt7601 usb wifi device!\n");
                break;
            }
            else if (0 == strcmp(USBDevInfo[i].strManufacturer, "Realtek"))
            {
                g_eCurrUSBWifiDev = RTL8188FTV_DEV;
                WIFI_INFO("Detect this IP Camera has rtl8188ftv usb wifi device!\n");
                break;
            }
        }
    }

    //解析出版型并生成一个含版型的隐藏文件，为了方便海思加载驱动时用
    if((2 == argc) && (0==strcasecmp(argv[1], "id")))
    {
        return 0;
    }

#else

    //解析出版型并生成一个含版型的隐藏文件，为了方便海思加载驱动时用
    if((2 == argc) && (0==strcasecmp(argv[1], "getDID")))
    {
        iRet = get_uid();

        return 0;
    }
    
    //1 从命令行解析版型及序列号
    iRet = GetBoardInfo(g_HilEncrypt);
    if(iRet)
    {
        WIFI_INFO("decryption fail, exit!\n");
        return -1;
    }

    //1 从加密里面获取版型
    boardID = g_HilEncrypt[13];
    boardID = (boardID & 0xFF ) << 8;
    boardID = boardID | g_HilEncrypt[14];
    memset(szCommand, 0, sizeof(szCommand));
    snprintf(szCommand, sizeof(szCommand), "echo %d > %s", boardID, DEVICE_BOARD_TYPE);
    mysystem(szCommand);

    //解析出版型并生成一个含版型的隐藏文件，为了方便海思加载驱动时用
    if((2 == argc) && (0==strcasecmp(argv[1], "id")))
    {
        memset(szCommand, 0, sizeof(szCommand));
        snprintf(szCommand, sizeof(szCommand), "touch /root/edvr/.BoardType_%x", boardID);
        mysystem(szCommand);    

        return 0;
    }
#endif
    //检查SD卡是否有升级包并升级
    if((2 == argc) && (0==strcasecmp(argv[1], "sdUpgrade")))
    {
        checkIPCUpgrade();
        //这儿不做是否升级了的判断，如果有分区被刷新了由看门狗进程去重启
        sleep(5);
        return 0;
    }

    //根据定制文件，加载AP模式参数
    if (0 == (access(CUSTOM_INFO, F_OK)))
    {
        memset(szBuf, 0, sizeof(szBuf));
        snprintf(szBuf, sizeof(szBuf), "cat %s | grep %s | awk -F \"=\" '{printf $2}'", 
            CUSTOM_INFO, "ApModeIp");
        WIFI_DEBUG("\nszBuf=%s\n", szBuf);
        memset(g_szDefaultApIp, 0, sizeof(g_szDefaultApIp));
        ShellCmdOut(szBuf, g_szDefaultApIp, sizeof(g_szDefaultApIp) - 1);
        WIFI_DEBUG("\ng_szDefaultApIp=%s\n", g_szDefaultApIp);

        memset(szBuf, 0, sizeof(szBuf));
        snprintf(szBuf, sizeof(szBuf), "cat %s | grep %s | awk -F \"=\" '{printf $2}'", 
            CUSTOM_INFO, "ApModeNetmask");
        WIFI_DEBUG("\nszBuf=%s\n", szBuf);
        memset(g_szDefaultApNetmask, 0, sizeof(g_szDefaultApNetmask));
        ShellCmdOut(szBuf, g_szDefaultApNetmask, sizeof(g_szDefaultApNetmask) - 1);
        WIFI_DEBUG("\ng_szDefaultApNetmask=%s\n", g_szDefaultApNetmask);

        memset(szBuf, 0, sizeof(szBuf));
        snprintf(szBuf, sizeof(szBuf), "cat %s | grep %s | awk -F \"=\" '{printf $2}'", 
            CUSTOM_INFO, "ApModeGateway");
        WIFI_DEBUG("\nszBuf=%s\n", szBuf);
        memset(g_szDefaultApGateway, 0, sizeof(g_szDefaultApGateway));
        ShellCmdOut(szBuf, g_szDefaultApGateway, sizeof(g_szDefaultApGateway) - 1);
        WIFI_DEBUG("\ng_szDefaultApGateway=%s\n", g_szDefaultApGateway);

        memset(szBuf, 0, sizeof(szBuf));
        snprintf(szBuf, sizeof(szBuf), "cat %s | grep %s | awk -F \"=\" '{printf $2}'", 
            CUSTOM_INFO, "ApSsidPrefix");
        WIFI_DEBUG("\nszBuf=%s\n", szBuf);
        memset(g_szDefaultApPrefix, 0, sizeof(g_szDefaultApPrefix));
        ShellCmdOut(szBuf, g_szDefaultApPrefix, sizeof(g_szDefaultApPrefix) - 1);
        WIFI_DEBUG("\ng_szDefaultApPrefix=%s\n", g_szDefaultApPrefix);

        memset(szBuf, 0, sizeof(szBuf));
        snprintf(szBuf, sizeof(szBuf), "cat %s | grep %s | awk -F \"=\" '{printf $2}'", 
            CUSTOM_INFO, "ApModePassword");
        WIFI_DEBUG("\nszBuf=%s\n", szBuf);
        memset(g_nSoftApPasswd, 0, sizeof(g_nSoftApPasswd));
        ShellCmdOut(szBuf, g_nSoftApPasswd, sizeof(g_nSoftApPasswd) - 1);
        WIFI_DEBUG("\ng_nSoftApPasswd=%s\n", g_nSoftApPasswd);
    }
    else
    {
        memset(g_szDefaultApIp, 0, sizeof(g_szDefaultApIp));
        snprintf(g_szDefaultApIp, sizeof(g_szDefaultApIp), "%s", DEFAULT_AP_IP); 

        memset(g_szDefaultApNetmask, 0, sizeof(g_szDefaultApNetmask));
        snprintf(g_szDefaultApNetmask, sizeof(g_szDefaultApNetmask), "%s", DEFAULT_AP_NETMASK); 

        memset(g_szDefaultApGateway, 0, sizeof(g_szDefaultApGateway));
        snprintf(g_szDefaultApGateway, sizeof(g_szDefaultApGateway), "%s", DEFAULT_AP_GATEWAY);

        memset(g_szDefaultApPrefix, 0, sizeof(g_szDefaultApPrefix));
        snprintf(g_szDefaultApPrefix, sizeof(g_szDefaultApPrefix), "%s", DEFAULT_AP_PREFIX);

        memset(g_nSoftApPasswd, 0, sizeof(g_nSoftApPasswd));
        snprintf(g_nSoftApPasswd, sizeof(g_nSoftApPasswd), "%s", DEFAULT_SPFTAP_PWD); 
    }

    //1 MAC地址的设置
    iRet = setMac();
    if(iRet)
    {
        WIFI_ERROR("setMac fail, exit!\n");
    }

    if((2 == argc) && (0==strcasecmp(argv[1], "noInsmodWifi")))
    {
        printf("No insmod wifi driver in wifi_detect process!\n");
    }
    else
    {
        //从配置文件获取wifi的目标模式
        memset(szBuf, 0, sizeof(szBuf));
        memset(szLastSetValue, 0, sizeof(szLastSetValue));
        if (0 == (access(EDVR_CFG_FILE_PATH, F_OK)))
        {
            snprintf(szBuf, sizeof(szBuf), "cat %s | grep %s | awk -F \"=\" '{printf $2}'", 
                     EDVR_CFG_FILE_PATH, INet_WLANWifiMode);
            WIFI_DEBUG("\nszBuf=%s\n", szBuf);
            ShellCmdOut(szBuf, szLastSetValue, sizeof(szLastSetValue) - 1);
            WIFI_DEBUG("\nszLastSetValue=%s\n", szLastSetValue);
            sscanf(szLastSetValue, "%d", &nLastWlanwifimode);
            enWifiMode = (eWifiMode)nLastWlanwifimode;
            WIFI_DEBUG("\nenWifiMode=%d\n", enWifiMode);
        }
        else if (0 == (access(DEF_EDVR_CFG_FILE_PATH, F_OK)))
        {
            snprintf(szBuf, sizeof(szBuf), "cat %s | grep %s | awk -F \"=\" '{printf $2}'", 
                     DEF_EDVR_CFG_FILE_PATH, INet_WLANWifiMode);
            ShellCmdOut(szBuf, szLastSetValue, sizeof(szLastSetValue) - 1);
            WIFI_DEBUG("\nszLastSetValue=%s\n", szLastSetValue);
            sscanf(szLastSetValue, "%d", &nLastWlanwifimode);
            enWifiMode = (eWifiMode)nLastWlanwifimode;
            WIFI_DEBUG("\nenWifiMode=%d\n", enWifiMode);
        }
        else
        {
            WIFI_INFO("Config file not found!\n");
            enWifiMode = WLAN_STA_MODE;
            WIFI_DEBUG("\nenWifiMode=%d\n", enWifiMode);
        }
        //和wifi模式备份文件对比
        if (0 == (access(WIFI_MODE_0_BAK_FILE_PATH, F_OK))
            || 0 == (access(WIFI_MODE_1_BAK_FILE_PATH, F_OK))
            || 0 == (access(WIFI_MODE_2_BAK_FILE_PATH, F_OK)))
        {
            memset(szBuf, 0, sizeof(szBuf));
            memset(szLastSetValue, 0, sizeof(szLastSetValue));
            snprintf(szBuf, sizeof(szBuf), "ls %s | awk -F \":\" '{printf $2}'", 
                WIFI_MODE_BAK_FILE_PATH);
            ShellCmdOut(szBuf, szLastSetValue, sizeof(szLastSetValue) - 1);
            WIFI_DEBUG("\nszLastSetValue=%s\n", szLastSetValue);
            sscanf(szLastSetValue, "%d", &nLastWlanwifimode);
            WIFI_DEBUG("\nnLastWlanwifimode=%d\n", nLastWlanwifimode);
            if ((0 == nLastWlanwifimode) 
                || (1 == nLastWlanwifimode) 
                || (2 == nLastWlanwifimode))
            {
                if (enWifiMode != ((eWifiMode)nLastWlanwifimode))
                {
                    WIFI_INFO("Need reset dhcp: from %d to %d\n", enWifiMode, nLastWlanwifimode);
                    enWifiMode = (eWifiMode)nLastWlanwifimode;
                }
            }
        }

        //根据wifi模式配置，尝试加载对应驱动
        if (WLAN_STA_MODE == enWifiMode)
        {
            WifiDetect_LoadStaModeDriver();
        }
        else
        {
            WifiDetect_LoadSoftApDriver();
        }

        //如果驱动加载之后，存在wlan0(ra0)网卡，则说明USB WIFI已经插入
        /* 将获取到的信息放到缓冲区buf中 */
        memset(szCommand, 0, sizeof(szCommand));
        snprintf(szCommand, sizeof(szCommand), "cat %s", PROC_NET_DEV);
        if (!IsStringInCommand(szCommand, "wlan0"))
        {
            //不存在，则卸载驱动
            WIFI_INFO("******Wifi is Not Exist! remove the wifi driver!******\n");
            if (WLAN_STA_MODE == enWifiMode)
            {
                WifiDetect_UnloadStaModeDriver();
            }
            else
            {
                WifiDetect_UnloadSoftApDriver();
            }
        }        
        /* 根据需求，修改MTU的值 */
        mysystem(MODIFY_MTU);
    }    

    /* 检查boa配置文件boa.conf是否被破坏，如果被破坏，则还原boa.conf-bak为boa.conf文件 */
    memset(szCommand, '\0', sizeof(szCommand));
    memset(szCommand1, '\0', sizeof(szCommand1));
    memset(szCommand2, '\0', sizeof(szCommand2));

    /* 打印boa配置文件的第一行和最后一行 */
    snprintf(szCommand1, sizeof(szCommand1), "sed -n '1p' %s", BOA_CONF);
    snprintf(szCommand2, sizeof(szCommand2), "sed -n '$p' %s", BOA_CONF);

    /* 查看第一行是否包含"[BOA_CFG_BEGIN]"并且最后一行是否包含[BOA_CFG_END]字符串，如果是，则表示配置文件没有被破坏，否则表示被破坏了，需要恢复 */
    if ((IsStringInCommand(szCommand1, BOA_CFG_HEAD) != 1)
        || (IsStringInCommand(szCommand2, BOA_CFG_TAIL) != 1))
    {
        WIFI_INFO("%s has been destroyed! recovering it!\n", BOA_CONF);
        snprintf(szCommand, sizeof(szCommand), "cp %s %s -v", BOA_CONF_BAK, BOA_CONF);
        mysystem(szCommand);
    }

    return 0;
}

