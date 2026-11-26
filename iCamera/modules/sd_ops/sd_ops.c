#define _GNU_SOURCE
#define _XOPEN_SOURCE

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <dirent.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <semaphore.h>
#include <libgen.h>
#include <utime.h>
#include <time.h>

#include <net/if.h>
#include <sys/socket.h> 
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/prctl.h>
#include <sys/mount.h>
#include <assert.h>

#include "mysystem.h"
#include "aviapi.h"
#include "disk.h"
#include "zwsdk.h"
#include "global_macro.h"
#include "sd_ops.h"
#include "LogManager.h"
#include "common_pub.h"
#include "../mp4/include/mp4_writer.h"   /* MP4 循环录像底层 */

extern int InitAviWriteBuf(void);
extern int UnInitAviWriteBuf(void);
extern int EDVR_HD_MountNode(const char *dev, const char *mnt_point);
extern int Get_PartitionSector(const char *dev, int partition, unsigned int *nTotalSector, unsigned int *nStartSector);
extern unsigned long Get_SDCardTotalSector(const char *dev);
extern int CheckDiskRWIsDamage(void);
extern int IsStringInCommand(const char *command, const char *string);
extern void TFLOG_SetAllowFlag(int allowflag);
extern int ShellCmdOut(const char *cmd, char *result, int len);
extern int PubFunc_deleteNewlineInString(char *string, int stringMaxLen);
extern int Change_DevFileSystemType(const char *dev, int partition, unsigned char fs_type);

/* 消除本文件中的snprintf截断警告 */
#pragma GCC diagnostic ignored "-Wformat-truncation"

#define ARRAY_SIZEOF(array)     (sizeof(array)/sizeof(array[0]))

//#define SDOPS_SHOW_DETAIL_DEBUGINFO
#define SD_OPS_ERROR(format...)        LOG_E(LOG_MODULE_SD, format)
#define SD_OPS_WRAN(format...)         LOG_W(LOG_MODULE_SD, format)
#define SD_OPS_INFO(format...)         LOG_I(LOG_MODULE_SD, format)
#define SD_OPS_DEBUG(format...)        LOG_D(LOG_MODULE_SD, format)

#define SMARTP_VIFrame 10000

/* 打印精确到毫秒的时间 */
#define _debug_timeprint() \
    do { \
        struct timeval tvTmp = {}; \
        struct tm stmTmp = {}; \
        char szDTime[64] = {}; \
        char szResult[64] = {}; \
        \
        gettimeofday (& tvTmp, NULL); \
        gmtime_r (& tvTmp.tv_sec, & stmTmp); \
        strftime (szDTime, sizeof(szDTime), "%F %T", &stmTmp); \
        snprintf (szResult, sizeof(szResult), "%s.%03d", szDTime, (int)(tvTmp.tv_usec / 1000)); \
        \
        printf ("timestamp[%s %d %s] %s\n", __FILE__,__LINE__,__FUNCTION__, szResult); \
    } while (0)

/* unsigned long类型传入，在32位系统下，时间戳上限2^32=4294967296，即北京时间2106/2/7 14:28:16之后会产生溢出 */
#define ULONG_TO_TIME_1(ul_time, year, mon, day, hour, min, sec)                  \
    do{                                                                            \
       char tmp[32] = {0};                                                        \
       struct tm cur_local_time;                                                  \
       localtime_r(&ul_time, &cur_local_time);                                    \
       SD_DBG("ul_time = %ld\n", ul_time);                                        \
       strftime(tmp, sizeof(tmp), "%Y:%m:%d:%H:%M:%S", &cur_local_time);          \
       SD_DBG("tmp = %s\n", tmp);                                                 \
       sscanf(tmp, "%d:%d:%d:%d:%d:%d", &year, &mon, &day, &hour, &min, &sec);    \
    } while (0)

/* unsigned long类型传入，在32位系统下，时间戳上限2^32=4294967296，即北京时间2106/2/7 14:28:16之后会产生溢出 */
#define ULONG_TO_TIME(ul_time, year, mon, day, hour, min, sec)                      \
        do{                                                                          \
         char tmp[32] = {0};                                                        \
         struct tm stmBuf = {0};                                                       \
         time_t stmTmp = ul_time;                                                        \
         gmtime_r(&stmTmp, &stmBuf);                                          \
         snprintf(tmp, sizeof(tmp), "%d:%02d:%02d:%02d:%02d:%02d", stmBuf.tm_year + 1900, \
         stmBuf.tm_mon + 1, stmBuf.tm_mday, stmBuf.tm_hour, stmBuf.tm_min, stmBuf.tm_sec);   \
         sscanf(tmp, "%d:%d:%d:%d:%d:%d", &year, &mon, &day, &hour, &min, &sec);    \
         } while (0)

/* 字符串时间格式为"yearmondayhourminsec",传出的时间戳为unsigned long类型 
 * 海思的系统库里面的strptime函数有BUG，不能用, 故用sscanf代替
 */
#define TIME_STRING_TO_STAMP(string_time, ul_time)\
    do{\
        struct tm tmTime = {0};\
        sscanf(string_time, "%4d%2d%2d%2d%2d%2d",\
              &tmTime.tm_year, &tmTime.tm_mon, &tmTime.tm_mday,\
              &tmTime.tm_hour, &tmTime.tm_min, &tmTime.tm_sec);\
        tmTime.tm_year -= 1900;\
        tmTime.tm_mon--;\
        tmTime.tm_isdst = -1;\
        ul_time = mktime(&tmTime);\
    } while (0)

/* 字符串时间格式为"yearmonday",传出的时间戳为unsigned long类型 
 * 海思的系统库里面的strptime函数有BUG，不能用, 故用sscanf代替
 */
#define TIME_STRING_TO_STAMP_DAY(string_time, ul_time)\
    do{\
        struct tm tmTime = {0};\
        sscanf(string_time, "%4d%2d%2d",\
               &tmTime.tm_year, &tmTime.tm_mon, &tmTime.tm_mday);\
        tmTime.tm_year -= 1900;\
        tmTime.tm_mon--;\
        tmTime.tm_isdst = -1;\
        ul_time = mktime(&tmTime);\
    } while (0)

/* unsigned long类型传入，在32位系统下，时间戳上限2^32=4294967296，即北京时间2106/2/7 14:28:16之后会产生溢出 */
#define TIME_STAMP_TO_STRING(ul_time, sz_time)\
    do{\
        struct tm s_tm = {0};\
        localtime_r((const time_t *)&ul_time, &s_tm); \
        snprintf(sz_time, 16, "%4d%02d%02d%02d%02d%02d",\
        s_tm.tm_year+1900, s_tm.tm_mon+1, s_tm.tm_mday, s_tm.tm_hour, s_tm.tm_min, s_tm.tm_sec);\
    } while (0)

/* unsigned long类型传入，在32位系统下，时间戳上限2^32=4294967296，即北京时间2106/2/7 14:28:16之后会产生溢出 */
#define TIME_STAMP_TO_STRING_DAY(ul_time, sz_time)\
    do {\
        struct tm s_tm = {0};\
        time_t raw_time = (time_t)(ul_time); \
        localtime_r(&raw_time, &s_tm); \
        snprintf(sz_time, 9, "%4d%02d%02d",\
                s_tm.tm_year + 1900, s_tm.tm_mon + 1, s_tm.tm_mday);\
    } while (0)

/* unsigned long类型传入，在32位系统下，时间戳上限2^32=4294967296，即北京时间2106/2/7 14:28:16之后会产生溢出 */
#define TIME_ULONG_TO_STRING(ul_time, sz_time)                                       \
        do{                                                                               \
            struct tm tm;                                                                \
            localtime_r((const time_t *)&ul_time, &tm);                                                  \
            snprintf(sz_time, 16, "%4d%02d%02d%02d%02d%02d",                              \
            tm.tm_year+1900, tm.tm_mon+1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec); \
    } while (0)

/* 时间转化为时间戳 */
#define TIME_ULONG_TO_TIMESTAMP(year, mon, day, hour, min, sec)                     \
        do{                                                                          \
            struct tm tm;                                                           \
            tm.tm_year = year - 1900;                                               \
            tm.tm_mon = mon - 1;                                                    \
            tm.tm_mday = day;                                                       \
            tm.tm_hour = hour;                                                      \
            tm.tm_min = min;                                                        \
            tm.tm_sec = sec;                                                        \
            mktime(&tm);                                                           \
         } while (0)
 
/* 定义所需的时间变量 */
#define DECLARE_SHOT_CLOCK() struct timeval ss_start,ss_end,ss_diff;

/* 计时开始.用于性能测试 */  
#define START_SHOT_CLOCK()   do{gettimeofday(&ss_start, NULL);}while(0);

/* 计时结束.用于性能测试 */  
#define STOP_SHOT_CLOCK()    do{gettimeofday(&ss_end, NULL);}while(0);

/* 用于计算STOP_SHOT_CLOCK到START_SHOT_CLOCK阶段所消耗的时间(累加, 微秒) */
#define SPEND_SHOT_CLOCK(ss_total)                                 \
    do{                                                            \
        timersub(&ss_end, &ss_start, &ss_diff);                    \
        ss_total += (ss_diff.tv_sec*1000*1000 + ss_diff.tv_usec); \
    }while(0);

/* 计时结束.合并结束和计算步骤，计算STOP_SHOT_CLOCK到当前阶段所消耗的时间(累加, 微秒) */  
#define STOP_SPEND_SHOT_CLOCK(ss_total)                             \
    do{                                                             \
        gettimeofday(&ss_end, NULL);                                \
        timersub(&ss_end, &ss_start, &ss_diff);                     \
        ss_total = (ss_diff.tv_sec*1000*1000 + ss_diff.tv_usec);  \
    }while(0);

/* 用suffix替换字符串src第二个下划线后面的字符串，放入dest里面 */
#define REPLACE_SECOND_UNDERLINE_NEW_SUFFIX(src, suffix, dest, destSize) \
    ({                                                         \
    char tmp[64]={0};                                          \
    char prefix_1[64]={0};                                     \
    char prefix_2[64]={0};                                     \
    sscanf(src, "%[^.].%[^.].%[^.]", prefix_1, prefix_2, tmp); \
    snprintf(dest, destSize, "%s_%s_%s", prefix_1, prefix_2, suffix);     \
    })
    
/* 用suffix替换字符串src点后面的字符串，放入dest里面 */
#define REPLACE_AFTER_POINT_TO_SUFFIX(src, suffix, dest, destSize)  \
    ({                                                    \
    char tmp[64]={0};                                     \
    char prefix[64]={0};                                  \
    sscanf(src, "%[^.].%[^.]", prefix, tmp);              \
    snprintf(dest, destSize, "%s.%s", prefix, suffix);               \
    })

#define MAX_OPERATE_HANDLE       (1)

#define MAX_PICTURE_SIZE_MB      (500)  // 抓图新逻辑: 图片固定为500MB

#define MINI_FREE_SD_NONE        (1024)
#define MINI_FREE_SD_32G         (2048)
#define MINI_FREE_SD_64G         (2560)

#define KILO_BYTE_KB             (1024)           // 2^10
#define MEGA_BYTE_MB             (1048576)        // 2^20
#define GIGA_BYTE_GB             (1073741824)     // 2^30
#define TERA_BYTE_TB             (1099511627776)  // 2^40

#define MAX_I_FRAME_NUM          (2048)
#define ONE_PICTURE_SIZE_KB      (1024)

#define MAX_PIC_NUM              (10000)   // 不做配置使用
#define RESOLVE_VIDEO_NUM        (50)      // 保留空间的视频文件数量

#define MAX_AVI_NUM_ONE_PAGE     (100)     // 每页最多显示多少个AVI文件
#define MAX_PIC_NUM_ONE_PAGE     (100)     // 每页最多显示多少个图片文件

#define MAX_LOG_LINE             (512)     // 操作SD卡日志记录的条数

#define VIDEO_MIN_SIZE_BYTES     (2 << 20) // 当前录像文件的有效大小最小字节数

#define VIDEO_LIMIT_SIZE_BYTES   (4 << 20) // 当前录像文件如果超过9MB，则认为是14MB文件；否则是7MB文件

#define DISK_MIN_FREE_SIZE_MBYTES     (1024) // 当前磁盘的最小剩余容量下限

#define EVENT_RECORD_LATEST_SECONDS      (120)
#define TIMER_RECORD_LATEST_SECONDS      (300)
#define MIN_RECORD_FILE_SIZE_LIMIE     (1024) // 最小录像文件限制(小于该大小的录像将被删除)

#define MAX_NAME_STR             (64)
#define MIN_NAME_STR             (33)
#define MAX_FILE_PATH_STR        (257)
#define MAX_PATH_STR_NUM         (128)
#define MAX_SEARCH_DAY           (365)
#define DAY_TIME_STAMP_SEC       (86400)   // 24*60*60
#define MAX_FILE_NUM             ((DAY_TIME_STAMP_SEC/EVENT_RECORD_LATEST_SECONDS) + 30) // 增加一些余量

/* xxxxxxxxxxxxxx.jpg
 * 年月日时分秒.jpg
 */
#define TIME_SNAPSHOT_MAX_LEN    (18)

#define DIGIT_STR               "0123456789"

#ifdef __USE_EMMC__
#define SD_DEV_NODE         "/dev/mmcblk1p1"
#define SD_MOUNT_POINT      "/tmp/app/disk/mmcblk1p1"
#define OS_SWAP_FILE_PATH   "/tmp/app/disk/mmcblk1p1/.OsSwapAtSdCard"
#define SD_DEV              "/dev/mmcblk1"
#else
#define SD_DEV_NODE         "/dev/mmcblk0p1"
#define SD_MOUNT_POINT      "/tmp/app/disk/mmcblk0p1"
#define OS_SWAP_FILE_PATH   "/tmp/app/disk/mmcblk0p1/.OsSwapAtSdCard"
#define SD_DEV              "/dev/mmcblk0"
#endif

#define SD_PRIMARY_PARTITION_NUM 1

// 自定义SD卡升级包名字文件
#define CUSTOM_SD_UPGRADE_FILE      "/tmp/app/disk/mmcblk0p1/upg.txt"

#define HTTP_DOWNLOAD_RECORD_PATH   "/download/video"
#define HTTP_DOWNLOAD_PICTURE_PATH  "/download/picture"

#define DISK_STATUS_FLAG         	"/tmp/.sd_status"

#define SD_NO_CMD         			"echo 0 > /tmp/.sd_status"
#define SD_NORMAL_CMD        		"echo 5 > /tmp/.sd_status"

#define SD_STATUS_CFG            	"/tmp/.dmesg_status.log"

// 因有些卡异常在主程序运行时无法处理或处理失败，所以记录需要的动作然后在下次启动时checkSDCard里再做处理。记录的动作定义见eSdActionAtStart
#define SD_ACTION_AT_START_FILE  "/mnt/mtd/config/.SDActionAtStart"
#define SD_LOG_FILE  "/mnt/mtd/config/.dmesg_status.log"

#define GET_SYSTEM_UPTIME_COMMAND "cat /proc/uptime | awk -F \".\" '{print $1}'"
#define SD_ABNORMAL_REBOOT_INTERVAL (6 * 60 * 60)

#define UMOUNT_FAIL_REBOOT_FLAG  "/mnt/mtd/config/.umount_fail_need_reboot"
#define RUN_BOA_EXE              "/mnt/app/bin/boa -c /home/http -f /mnt/mtd/etc/conf.d/boa/boa.conf"

#define SHELL_FIND_UDHCPC        "ps | grep -v grep | grep udhcpc | awk -F \"0:00\" '{print $2}'"

#define PER_AVI_TYPE_INFO_SEC_SIZE      (32 * 1024)     // 类型信息大小
#define PER_AVI_FRAME_INFO_SEC_SIZE     (32 * 1024)     // 帧索引信息大小
#define PER_AVI_INFO_SEC_SIZE       (PER_AVI_TYPE_INFO_SEC_SIZE + PER_AVI_FRAME_INFO_SEC_SIZE) // 视频文件附加数据部分大小（当前为 32K类型信息 + 32K帧索引数据）
// #define PRE_AVI_RESOLVED_SIZE    (256*1024)

#define SDOPS_RECORD_DELAY_MAX      (120)
#define SDOPS_RECORD_DELAY_MIN      (5)

#define SD_OPS_MAGIC_NUM           (0xDEADBEEF)
#define DATA_PAKT_START_CODE       (0xB7010000)
#define FRAME_INDEX_START_CODE     (0x494E4458) // indx

#define I_FRAME              (0x01)
#define P_FRAME              (0x02)
#define AUDIO_FRAME          (0x08)

#define G711A_AUDIO_FRAME    (0x1)
#define G711U_AUDIO_FRAME    (0x2)

#define LOADTYPE_H264_ENC    (0x00)
#define LOADTYPE_H265_ENC    (0x04)

#define LOADTYPE_G726_ENC     (0x00)
#define LOADTYPE_G711A_ENC    (0x01)
#define LOADTYPE_G711U_ENC    (0x02)

#define MAX_MP4_EXTRA_SIZE          (64*1024)

#define PER_AVI_MAX_TYPE            (64)        // 每个录像文件的最大关联类型数

static int MAX_SIZE_PER_FRAME   =  (480*1024+28);

#define WRITE_MAX_BUF            (64*1024)

#define MIN_VIDEO_LIMIT_SIZE     (200*1024) // 最小AVI文件限制(小于该尺寸则在网页上面不显示)

/* 将16字节结构体的前3个INT型进行异或，结果放在第4个INT型里面 */
#define XOR_INT_16BYTES(p)       (p[0]^p[1]^p[2])

/* 将32字节结构体的前7个INT型进行异或，结果放在第8个INT型里面 */
#define XOR_INT_32BYTES(p)       (p[0]^p[1]^p[2]^p[3]^p[4]^p[5]^p[6])

/* 将64字节结构体的前15个INT型进行异或，结果放在第16个INT型里面 */
#define XOR_INT_64BYTES(p)       (p[0]^p[1]^p[2]^p[3]^p[4]^p[5]^p[6]^p[7]^p[8]^p[9]^p[10]^p[11]^p[12]^p[13]^p[14])

/* SD卡升级的升级包名称 */
#define IPC_SD_CARD_UPGRADE_FILE_NAME_PREFIX    "ipcUpgrade"
#define IPC_SD_CARD_UPGRADE_FILE_NAME_FOR_SYSTEM   "ipcUpgradeForSystem.bin"
#define SD_UPGRADE_FLAG_FILE_NAME ".sdCardUpgradeFlag"
#define SD_PTZ_TEST_FLAG_FILE_NAME "ptzTestEn"
#define DEVICE_LOG_DIR   "log"

#define OS_SWAP_FILE_NAME                      ".OsSwapAtSdCard"

#define OS_SWAP_FILE_SIZE_BYTE                 (32*1024*1024)
#define OS_SWAP_FILE_WRITE_BLOCK_SIZE_BYTE     (64*1024)

/* 日志文件名称 */
#define IPC_DMESG_FILE_NAME        "dmesg.log"
#define IPC_UAMP_FILE_NAME         "umap.log"
#define IPC_LOGMPP_FILE_NAME       "logmpp.log"

/* SD卡升级的升级包名称 */
#define IPC_SD_CARD_NETCONFIG_FILE_NAME    "SdTypeNetCfg.ini"

/* 临时AVI文件(TMP)信息存放点 */
#define TMP_FILE_INFO_STORE_POINT        "video/.tmp_file_info"

/* 存放临时的tmp的帧索引文件 */
#define TMP_FRAME_INFO_FILE              "video/.frames_index"
#define REPAIRED_VIDEO_MIN_SIZE_BYTES    (500*1024)       //暂定修复阈值为500k,事件录像约为2-3M。
#define REPAIRED_VIDEO_MIN_IFRAME_NUM    (5)  //暂定修复阈值为5个I帧。
#ifdef __SUPPORT_SDCARDHOTSWAP__
static pthread_t          g_DiskSoftHotPlugStatus = -1;
static pthread_t          g_DiskRemoveStatus = -1;
#define SD_SOFT_HOTPLUG_RETRY_TIMES     3
#define SD_WRITE_AVI_FAIL_TIMES         3
#define SDOPS_CMD_SOFT_REMOVE  "echo \"detect\" > /proc/mmc0/status"
// #define SDOPS_CMD_SOFT_INSERT  "echo INSERT > /sys/devices/platform/jz-sdhci.0/present"
#endif

/* MP4文件修复成功标记 */
#define MP4_REPAIR_SUCESS_FLAG           "/tmp/.mp4_repair_success"

/* 该宏用来控制SD_OPS调试打印信息 */
//#define SD_DBG_INF
//#define GET_LIST_DBG
//#define SD_PLK_DBG

#define VIDEO_FORMAT_SUFFIX      "prv"
#define IMAGE_FORMAT_SUFFIX      "jpg"


#ifdef __SUPPORT_128G_DISK__
#define MAX_AVI_FILE_I_FRAME_NUM        (2048)  //14MB // (14 * 1024 / 8) // 1024 > 768个I帧(64Kb => 64/8=8KB
#define AVIAPI_VIDEO_FILESIZE           (14 * 1024 * 1024)  // 视频文件最大大小
#define AVIAPI_VIDEO_TYPE_INFO_SIZE     (32 * 1024)                 // 录像类型信息大小，这里要与sd_ops中SD_OPS_RecordTypeInfo保持一致
#define AVIAPI_VIDEO_FRAME_INFO_SIZE    (32 * 1024)         // 帧索引数据信息大小，
#else
#define MAX_AVI_FILE_I_FRAME_NUM        (1024)  // (6 * 1024 / 8) // 1024 > 768个I帧(64Kb => 64/8=8KB
#define AVIAPI_VIDEO_FILESIZE           (7 * 1024 * 1024)  // 视频文件最大大小
#define AVIAPI_VIDEO_TYPE_INFO_SIZE     (32 * 1024)         // 录像类型信息大小，这里要与sd_ops中SD_OPS_RecordTypeInfo保持一致
#define AVIAPI_VIDEO_FRAME_INFO_SIZE    (16 * 1024)        // 帧索引数据信息大小，
#endif

#define FILE_START_CODE     (0xB6010000)
#define FRAME_START_CODE    0xB7010000

/* 当前系统支持的视频录像类型 */
typedef enum {
    RRCORD_VIDEO_PRV = 0,
    RRCORD_VIDEO_AVI,
    RRCORD_VIDEO_MP4,
    RECORD_VIDEO_END,
}E_RECORD_VIDEO_TYPE;

/* 帧头信息 */
typedef struct tagH264DataPacketHeader
{
    unsigned int        uDataPacketStartCode; //固定为0xB7010000
    unsigned int        uFrameNo;				//媒体数据包的序号(音视频独立编号)
    unsigned char       byFrameType;
    unsigned char       byLoadType;				//媒体数据包的信息,具体含义依赖于帧类型
    unsigned int        uShowTime;				//表示媒体数据包相对于上一帧的偏差时间(频率90Khz)
    unsigned long       u32RealTimeSec;
    unsigned long       u32RealTimeUSec;
    unsigned short      uReserved;
    unsigned int        uOverloadLen;			//表示媒体数据包的数据长度
}__attribute__((packed)) H264DATAPACKETHEADER, *PH264DATAPACKETHEADER;


/* 文件头信息 */
typedef struct tagMP4NewFileHeader
{
    unsigned int FileStartCode;		        //固定为0xB6010000
    unsigned int FileID;					//固定为0x34363248,即H264
    unsigned int FileHeadLen;				//媒体文件头的长度(包含本身4Byte)
    unsigned char Version;					//媒体文件头的版本信息.初始版本为1
    /*
    2Bytes, 表示媒体数据包及媒体索引信息
    MSB B15 		  0:没有索引 1:有索引
    	B1			  0:no Private Data 1:with Private Data
    	B0			  0: Video Only 1:Video&Audio
    */
    short int	DataFlag;		  
    short int	dwImageWidth;			    //视频媒体数据的图像宽度
    short int	dwImageHight; 		        //视频媒体数据的图像高度
} __attribute__ ((packed)) MPEG4FILEHEADER;

/* 绑定在录像文件上的录像类型相关定义，最终需要二进制存储到文件中 */
typedef struct {
    unsigned int uTimeBegin;    // 录像类型对应的开始时间
    unsigned int uTimeEnd;      // 录像类型对应的结束时间
    unsigned int uRecordType;   // 对应于 eRecordType
} SD_OPS_RecordTypeItem;    // sizeof() == 12 

/* 文件里面录像类型结构体(后续如果有需要添加的字段，可以减少保留字段)：32KB */
typedef struct {
    unsigned int uMagicNumber;      // 0xdeadbeef 标记该结构体有效
    unsigned int uFileBeginTime;    // 文件起始帧时间（所有帧）
    unsigned int uFileEndTime;      // 文件结束帧时间（所有帧）
    unsigned int uNum;              // arrayTypeInfo的有效数目
    SD_OPS_RecordTypeItem arrayTypeInfo[PER_AVI_MAX_TYPE];    // 每个录像文件关联的片段。只记录事件片段（即非普通录像）
    char reserved[32 * 1024 - 5 * sizeof(int) - PER_AVI_MAX_TYPE * sizeof(SD_OPS_RecordTypeItem)];
    unsigned int uRealFrameLenth;   // 有效帧数据的长度
}__attribute__((packed)) SD_OPS_RecordTypeInfo;

/* 底层录像类型 */
typedef enum{
    eNoRecord       = 0x0, // 无录像
    eAlarmInRecord  = 0x1, // 报警输入录像           "A"
    eNormalRecord   = 0x2, // 普通录像               "N"
    eManualRecord   = 0x3, // 手动录像(无)
    eMotionRecord   = 0x4, // 移动侦测触发           "M"
    eHttpTrigRecord = 0x5, // Http事件录像           "H"
    eAiRecord       = 0x6, // 智能侦测触发           "I" 
    eSoundRecord    = 0x7, // 声音侦测触发(当前仅用于SD卡抓图)           "S" 
    eRecordTypeNum  = 0x8,
}eRecordType;

/* 上层录像类型 */
typedef enum{
    NO_RECORD        = 0x0, // 无录像    
    NORMAL_RECORD    = 0x1, // 普通录像               "T"    
    MOTION_RECORD    = 0x2, // 移动侦测触发           "M"    
    MANUAL_RECORD    = 0x3, // 手动录像(无)
    ALARMIN_RECORD   = 0x4, // 报警输入录像           "A"
    HTTP_TRIG_RECORD = 0x5, // Http事件录像           "H"
    AI_RECORD        = 0x6, // 智能侦测触发 "I"    //fjz-notice
}eNetRecordType;

typedef enum {
    SD_NO_NODE = 0,      // 无节点无卡
    SD_CARD_ERR,         // 卡错误无卡
    CARD_MOUNT_ERR,      // 挂载失败无卡
    FIRST_DSG_EXP,       // 第一次dmesg异常
    CARD_RW_EXP,         // 卡读写异常
    FILE_MD5_EXP,        // 文件验证异常
    SEC_DSG_EXP,         // 第二次dmesg异常
    INIT_FORMAT,         // 初始化时格式化
    MIDDLE_FORMAT,       // 系统运行中格式化
    CLEAN_ALL_CARD,      // 清空SD卡
    SD_CARD_NORMAL,      // 卡正常
    SD_ADD_PART_ERR,     // 创建分区失败
    MIDDLE_DSG_EXP,      // 中间层dmesg检测异常
    MIDDLE_FILE_MD5_EXP, // 中间层文件验证异常
    SD_CARD_ERR_NUM      // 错误总数
}eSdErrorNo;

typedef enum {
    SD_ACTION_AT_START_INVALID = 0,      // 无效值
    SD_ACTION_AT_START_FORMAT = 1,      // 卡需要格式化
    SD_ACTION_AT_START_REPAIR = 2,      // 卡需要修复
    SD_ACTION_AT_START_NUM      // 错误总数
}eSdActionAtStart;

typedef enum{
    E_FILE_ERR = 0,
    E_FILE_DIR = 1,
    E_MP4_FILE = 2,
    E_PRV_FILE = 3,
    E_AVI_FILE = 4,
    E_JPG_FILE = 5,
    E_TYPE_BUTT,
}T_FILE_TYPE;

typedef enum{
    E_FILE_MAP = 0,
    E_REC_MAP   = 1,
    E_MAP_BUTT,
}T_MAP_TYPE;

typedef struct tagFileNameInfo{
    T_FILE_TYPE eFileType;
    eRecordType  eRecType;
    unsigned int nStartTime;
    unsigned int nStopTime;
}stuFileNameInfo, *lpFileNameInfo;

typedef struct tagFileInfo{
    stuFileNameInfo struFileNameInfo;
    char szFileName[MAX_NAME_STR];
    unsigned int nFileSize;
}stuFileInfo, *lpFileInfo;

typedef struct tagRecType{
    eRecordType eRecType;
    char szRecTypeSuffix[16];
}stuRecType, *lpRecType;

typedef struct tagFileType{
    T_FILE_TYPE eFileType;
    char szFileTypeSuffix[16];
}stuFileType, *lpFileType;

static const stuRecType s_gRecType[] = {
    {eNormalRecord,   "TIMER"},
    {eAlarmInRecord,  "ALARM"},
    {eMotionRecord,   "MOTION"},
    {eHttpTrigRecord, "HTTP"},
    {eAiRecord,       "AI"},
};

static const stuFileType s_gFileType[] = {
    {E_FILE_DIR, ""},
    {E_MP4_FILE, ".mp4"},
    {E_PRV_FILE, ".prv"},
    {E_AVI_FILE, ".avi"},
    {E_JPG_FILE, ".jpg"},
};

#if 0
static const char *gSdErrorMsg[SD_CARD_ERR_NUM] = {
    "[Middle] check SD Card no node\n",
    "[Middle] check SD Card error no card\n",
    "[Middle] check SD Card mount fail no card\n",
    "[Middle] check SD Card error in first dmesg\n",
    "[Middle] check SD Card read and write exception\n",
    "[Middle] check SD Card file md5sum check exception\n",
    "[Middle] check SD Card error in second dmesg\n",
    "[Middle] check SD Card formated in system boot\n",
    "[Middle] check SD Card formated in ambarella_test\n",
    "[Middle] check SD Card has been clean up\n",
    "[Middle] check SD Card is normal\n",
    "[Middle] check SD Card add partition fail\n",
    "[Middle] check SD Card error in middle dmesg\n",
    "[Middle] check SD Card file md5sum check exception in middle\n",
};
#endif

MY_DECRIBE_DECLARE_BEGIN(eSdCardTotalInfoStatus)  // 宏定义开始
    MY_DECRIBE_ADD_DESC(SdCardBootCheck_NoNode, Check no node when device boot\n)  // 注意这里的字符串描述不用加引号
    MY_DECRIBE_ADD_DESC(SdCardBootCheck_MountFail, Check mount fail when device boot\n)
    MY_DECRIBE_ADD_DESC(SdCardBootCheck_DiskExp, Check disk excption when device boot\n)
    MY_DECRIBE_ADD_DESC(SdCardBootCheck_AddPartFail, Check add part fail when device boot\n)
    MY_DECRIBE_ADD_DESC(SdCardBootCheck_ReadWriteFail, Check read and write test when device boot\n)
    MY_DECRIBE_ADD_DESC(SdCardBootCheck_DiskIsNormal, Check disk is normal when device boot\n)

    MY_DECRIBE_ADD_DESC(SdCardFormatStatus_UmountFail, Check umount fail when disk format\n)
    MY_DECRIBE_ADD_DESC(SdCardFormatStatus_FormatFail, Check format fail when disk format\n)
    MY_DECRIBE_ADD_DESC(SdCardFormatStatus_IsFormatting, Check is formating when disk format\n)
    MY_DECRIBE_ADD_DESC(SdCardFormatStatus_FormatOK, Check format ok when disk format\n)

    MY_DECRIBE_ADD_DESC(SdCardRunCheck_DiskReadOnly, Check disk is ReadOnly when device running\n)
    MY_DECRIBE_ADD_DESC(SdCardRunCheck_DiskIsError, Check disk is error when device running\n)
    MY_DECRIBE_ADD_DESC(SdCardRunCheck_FatfsIsError, Check Fatfs is error when device running\n)
    MY_DECRIBE_ADD_DESC(SdCardRunCheck_DiskIsWritting, Check disk is writting when device running\n)
    MY_DECRIBE_ADD_DESC(SdCardRunCheck_DiskIsIdle, Check disk is idle when device running\n)
MY_DECRIBE_DECLARE_END                            // 宏定义结束 

MY_DECRIBE_DECLARE_BEGIN(eSdErrorNo)  // 宏定义开始
    MY_DECRIBE_ADD_DESC(SD_NO_NODE, [check SD Card no node]\n)  // 注意这里的字符串描述不用加引号
    MY_DECRIBE_ADD_DESC(SD_CARD_ERR, [check SD Card error no card]\n)
    MY_DECRIBE_ADD_DESC(CARD_MOUNT_ERR, [check SD Card mount fail no card]\n)
    MY_DECRIBE_ADD_DESC(FIRST_DSG_EXP, [check SD Card error in first dmesg]\n)
    MY_DECRIBE_ADD_DESC(CARD_RW_EXP, [check SD Card read and write exception]\n)
    MY_DECRIBE_ADD_DESC(FILE_MD5_EXP, [check SD Card file md5sum check exception]\n)
    MY_DECRIBE_ADD_DESC(SEC_DSG_EXP, [check SD Card error in second dmesg]\n)
    MY_DECRIBE_ADD_DESC(INIT_FORMAT, [check SD Card formated in system boot]\n)
    MY_DECRIBE_ADD_DESC(MIDDLE_FORMAT, [check SD Card formated in ambarella_test]\n)
    MY_DECRIBE_ADD_DESC(SD_CARD_NORMAL, [check SD Card is normal]\n)
    MY_DECRIBE_ADD_DESC(SD_ADD_PART_ERR, [check SD Card add partition fail]\n)
    MY_DECRIBE_ADD_DESC(MIDDLE_DSG_EXP, [check SD Card error in middle dmesg]\n)
    MY_DECRIBE_ADD_DESC(MIDDLE_FILE_MD5_EXP, [check SD Card file md5sum check exception in middle]\n)
MY_DECRIBE_DECLARE_END                            // 宏定义结束 

//static unsigned char g711u_buf[640] = {0};
static eRecordType g_last_record_type = eNoRecord;
static eRecordType g_curr_record_type = eNoRecord;
static SD_OPS_RecordTypeInfo g_curr_record_info = {SD_OPS_MAGIC_NUM, 0};   // 记录当前录像文件的关联录像类型信息

static int   g_video_space_kb    = 0;    // 录像分区空间大小(单位KB)
static int   g_picture_space_kb  = 0;    // 图片分区空间大小(单位KB)

static int   g_sd_card_has_inited  = 0;
static int   g_sdops_pause_flag = 0;

static char  g_tmp_file_name[MAX_PATH_STR_NUM] = {0};
static char  g_ops_file_path[MAX_NAME_STR]      = {0};

static char  g_sd_dev[20]              = {0};

static int   g_bCheckSdStatusExit = -1;
static eSdCardWriteStatus  		g_sd_write_status    = SdCardIsIDEL;         	// sd卡写状态
static eSdCardStatus       		g_sd_check_status    = NoSdCard;             	// sd卡检测状态
static eSdCardTotalInfoStatus 	g_sd_runing_status	 = SdCardBootCheck_NoNode;	// sd卡运行状态，记录来自MonitorProcess的错误码
static eSdCardFormatStatus 		g_Sd_format_status   = UnFormatted;         	// sd卡格式化状态

static LPEventInfo        g_pEventInfo = NULL;
static DiskDeviceInfoEx   g_diskinfo;

/* 对g_after_sort_file_set和g_read_curr_avi_num加锁 */
static pthread_mutex_t    g_get_files_list_set_mutex = PTHREAD_MUTEX_INITIALIZER;

static char               *popFrameBuff[MAX_OPERATE_HANDLE] = {NULL};      /* 当前POP的帧缓冲(512KBytes)                   */

static sem_t              g_semWakeThread;
static sem_t              g_semDeleteFileThread;
static sem_t              g_semPicSpaceCheckThread;
static pthread_t          g_DiskStatusCheck = 0;
static pthread_t          g_TimerCounterThread = 0;
static pthread_t          g_DiskFileDeleteThread = 0;
static pthread_t          g_CheckImageUsedSpaceThreadProcess = 0;

static int                g_b_need_creat_avi_file = 1;   /* 创建新的AVI文件需要做的工作 */
static int                g_sd_card_format_status = -1;
static int                g_sd_format_over = 0;
static unsigned int       g_second_times = 0;

/* ============================================================================
 * MP4 循环录像相关（Readme 新需求）
 * ----------------------------------------------------------------------------
 * 只按录像时长自动切片，切片间隔由 g_mp4_segment_duration_sec 控制，
 * 上层可通过 SDOPS_SetMp4SegmentDuration 动态调整。
 * 容器为 fMP4（分片 MP4），异常掉电后已写入分片仍可直接播放，
 * 底层复用 modules/mp4 的 MP4Writer 系列接口，不做二次编码。
 * ==========================================================================*/
static int            g_use_mp4_record = 1;   /* 录像容器：1=MP4(新)，0=AVI(旧)，做回退开关 */
static unsigned int   g_mp4_segment_duration_sec = 300; /* 分片时长阈值(秒)，默认 5 分钟 */

static MP4Writer     *g_mp4_writer = NULL;       /* 当前分片写入句柄 */
static char           g_mp4_curr_file_name[MAX_PATH_STR_NUM] = {0}; /* 当前分片绝对路径 */
static unsigned int   g_mp4_seg_begin_real_sec = 0;  /* 当前分片首帧的绝对秒 */
static unsigned int   g_mp4_seg_begin_real_usec = 0; /* 当前分片首帧的绝对微秒 */
static int64_t        g_mp4_seg_begin_frame_abs_us = 0; /* 首帧绝对时间(微秒)，用于归一化 */
static int            g_mp4_have_first_frame = 0;    /* 是否已建立时间基线 */

/* ============================================================================
 * 对外可配置接口：允许上层应用动态调整 MP4 分片时长阈值
 * ==========================================================================*/
void SDOPS_SetMp4SegmentDuration(unsigned int sec)
{
    g_mp4_segment_duration_sec = (sec > 0) ? sec : 300;
}
unsigned int SDOPS_GetMp4SegmentDuration(void)
{
    return g_mp4_segment_duration_sec;
}
void SDOPS_SetRecordContainerToMp4(int use_mp4)
{
    g_use_mp4_record = (use_mp4 != 0) ? 1 : 0;
}

typedef struct tagGlobalStaticVar{
    int curr_avi_index;                         /* 当前操作的AVI的avi_file_set下标             */
    int last_ops_avi_index;                     /* 需要记录上次操作的avi文件下标(初始需为-1)    */
    int curr_ops_avi_index;                     /* 需要记录当前操作的avi文件下标(初始需为-1)    */
    unsigned int usr_search_start_time;         /* 当前用户设置的搜索的起始时间点                     */
    int playback_file_list_index;               /* 当前查询到的集合里面的文件索引下标(个数-1)                 */
    unsigned int rec_file_start_timestamp;      /* 当前录像文件的起始时间 */
    unsigned int rec_file_get_seektime_sec;     /* 当前录像的seektime */
}stuGlobalStaticVar, *pGlobalStaticVar;

/* 搜索的录像文件片段数组 */
typedef struct tagRecordSearchList
{
    char file_name[MAX_NAME_STR];           /* 录像文件的文件名字     */
    unsigned int  start_time;               /* 录像文件开始录制的时间 */
    unsigned int  stop_time;                /* 录像文件结束录制的时间 */
    unsigned int  record_type;              /* 录像文件的文件类型     */
    unsigned int  file_size;                /* 录像文件的文件大小 */
}RecordSearchList, *pRecordSearchList;

typedef struct tagDateTime
{
    int nYear;                              /*!< 年 */
    int nMonth;                             /*!< 月 */
    int nDay;                               /*!< 日 */
    int nHour;                              /*!< 时 */
    int nMinute;                            /*!< 分 */
    int nSecond;                            /*!< 秒 */
} stuDateTime, *LPDateTime;

/* AVI信息在网页上面显示的信息 */
typedef struct avi_read_list_s
{
    char file_name[MAX_NAME_STR];           /* AVI文件的文件名字      */
    char file_path[MAX_NAME_STR];           /* AVI文件的文件路径      */
    unsigned int  file_size;                /* AVI录像文件的大小      */
    stuDateTime   start_time;               /* AVI文件开始录制的时间 */
    stuDateTime   stop_time;                /* AVI文件结束录制的时间 */
} Avi_read_list, *pAvi_read_list;  

/* 图片信息在网页上面显示的信息 */
typedef struct pic_read_list_s
{
    char file_name[MAX_NAME_STR];           /* PIC文件的文件名字      */
    char file_path[MAX_NAME_STR];           /* PIC文件的文件路径      */
    unsigned int  file_size;                /* PIC图片文件的大小      */
    stuDateTime   cap_time;                 /* 抓图时间            */
} Pic_read_list, *pPic_read_list;  

/* 当前操作的文件的读写状态 */
typedef enum{
    IS_IDLE     = 0,                        /* 当前文件空闲   */
    IS_READING  = 1,                        /* 当前文件正在读 */
    IS_WRITTING = 2                         /* 当前文件正在写 */
}eReadWriteStatus;

typedef struct tagAviFileReadWriteInfo
{
    char     ops_file_name[MAX_NAME_STR];            /* 当前操作的AVI文件名 */
    eReadWriteStatus  read_write_status;             /* 当前AVI的读写状态   */
}stuAviFileReadWriteInfo, *pAviFileReadWriteInfo;

/* 当前正在录像文件的查询状态 */
typedef enum{
    NO_RECORD_FILE,             // 当前查询的时间段没有录像文件
	HAS_RECORD_FILE,            // 当前查询的时间段有录像文件
	RECORD_FILE_IS_WRITTING,    // 当前查询的时间段有文件正在写
}E_RECORDING_FILE_STATUS;

/* 当前正在录像的文件查询信息 */
typedef struct tagCurrTimeSegRecInfo{
    E_RECORDING_FILE_STATUS eRecordingFileStatus;   // 查询时间段文件信息
	int nRecordingFileStartTime;                    // 查询时间段文件真实的起始录像时间
	int nRecordingFileStopTime;                     // 查询时间段文件真实的结束录像时间(从查询的那一刻算作结束时间)
	int nRecordingFileWritingTime;                  // 查询时间段文件结束时间到起始时间持续的时间
	int nRecordingFileWritingSize;                  // 查询时间段文件结束时间到起始时间的文件大小
}stuCurrTimeSegRecInfo, *pCurrTimeSegRecInfo;

static stuAviFileReadWriteInfo g_avi_file_read_write_info = {
    .ops_file_name      = {0},
    .read_write_status  = IS_IDLE
};

/* 当前AVI文件在网页上面显示信息结构体数组 */
static Avi_read_list g_read_avi_list[MAX_AVI_NUM_ONE_PAGE];

static unsigned int  g_one_page_show_avi_count = 50;

/* 当前图片文件在网页上面显示信息结构体数组 */
static Pic_read_list g_read_pic_list[MAX_PIC_NUM_ONE_PAGE];

static unsigned int  g_one_page_show_pic_count = 50;

static stuGlobalStaticVar g_read_global_static_var[MAX_OPERATE_HANDLE] = {
    {0, -1, -1, 0, 0, 0}
};

AviSegLinkList *SEG_LINK[4] = {NULL};

PopHandle *pop_handle = NULL;

int errorcode = 0;

static char        g_last_pic_name_pre[MAX_NAME_STR]   = {0};

static stuFileInfo *g_pFilesListSet[MAX_OPERATE_HANDLE] = {NULL};

static pAvi_read_list g_pAviReadListSet = NULL;  /* 查询录像文件列表的文件集 */
static pPic_read_list g_pPicReadListSet = NULL;  /* 查询抓图文件列表的文件集 */
static int g_nReadTotalRecFileCount = 0; /* 查询到的所有录像文件数 */
static int g_nReadTotalPicFileCount = 0; /* 查询到的所有抓图文件数 */

static int g_nChoosePage = 1; /* 选中的页面(从1开始) */
static int g_nPageType = 0;   /* 当前录像路径类型 0: http路径  1:绝对路径 */

static pthread_mutex_t    seg_wr_lock               = PTHREAD_MUTEX_INITIALIZER;  //用于seg读写的锁
static pthread_mutex_t    global_mutex              = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t    global_file_mutex         = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t    global_disk_write_mutex   = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t    global_pop_frame_mutex    = PTHREAD_MUTEX_INITIALIZER;

/* 录像类型相关 */
static int SDOPS_SeekFdToRecordTypeInfo (int fd);
static int SDOPS_SaveTypeInfoToFile (const SD_OPS_RecordTypeInfo *info, const char *szFileName);
static int SDOPS_GetRecordTypePriority (eRecordType eA);
static int SDOPS_RecordTypeCompare (eRecordType eA, eRecordType eB);
static int SDOPS_InitRecordTypeInfo (const H264DATAPACKETHEADER *pFrameHead, SD_OPS_RecordTypeInfo *info);
static int SDOPS_CompleteRecordTypeInfo (SD_OPS_RecordTypeInfo *info);

static void *DelayReboot(void *pParam);
static void SetSDActionAtStart(eSdActionAtStart action);

/* 文件写完毕之后，需要从MPU层获取当前UTC时间和实际时间的偏移 */
int (*m_GetTimeOffsetCallBack)(int *, int *, int *) = NULL;

#ifdef __SUPPORT_SDCARDHOTSWAP__
extern sem_t              g_semDiskDelThread;
extern sem_t              g_semDiskAddThread;
extern int                Hotplug_flag;
static void* RunDiskSoftHotplugThread(void *lparam)
{
    PRT_TID();
    pthread_detach(pthread_self());

    int ret = 0;
    int retry = SD_SOFT_HOTPLUG_RETRY_TIMES;
    Hotplug_flag = 1;
#ifdef __SUPPORT_SAVE_TF_LOG__
    TFLOG_SetAllowFlag(0);
#endif	
    SD_OPS_INFO("Start trigger disk & record uninit!\n");
    ret = sem_post(&g_semDiskDelThread);
    sleep(4);
    SD_OPS_INFO("Start Disk Soft Hotplug!\n");
    mysystem(SDOPS_CMD_SOFT_REMOVE);
    // sleep(2);
    // mysystem(SDOPS_CMD_SOFT_INSERT);
    sleep(3);
    while(retry > 0)
    {
        SD_OPS_ERROR("check sd retry:%d!\n", retry);
        if (access(SD_DEV, F_OK) != 0)
        {
            mysystem(SDOPS_CMD_SOFT_REMOVE);
            sleep(2);
            retry--;
            continue;
        }
        break;
    }
    if (retry > 0)
    {
        sem_post(&g_semDiskAddThread);
    }
	else
    {
		mysystem("echo 8 > /tmp/.sd_status");
		SD_OPS_ERROR("check sd erro !\n");
    }
#ifdef __SUPPORT_SAVE_TF_LOG__
	TFLOG_SetAllowFlag(1);
#endif	
    Hotplug_flag = 0;
    g_DiskSoftHotPlugStatus = -1;
    SD_OPS_INFO("Stop Disk Soft Hotplug!\n");
    return NULL;
}
static void* RunDiskRemoveThread(void *lparam)
{
    PRT_TID();
    pthread_detach(pthread_self());

#ifdef __SUPPORT_SAVE_TF_LOG__
    TFLOG_SetAllowFlag(0);
#endif	
	SD_OPS_INFO("Start trigger disk & record uninit!\n");
    sem_post(&g_semDiskDelThread);
    sleep(4);
    SD_OPS_INFO("Start Disk Remove\n");
    usleep(500000);
    mysystem(SDOPS_CMD_SOFT_REMOVE);
    sleep(2);
    g_DiskRemoveStatus = -1;
	mysystem("echo 8 > /tmp/.sd_status");
    SD_OPS_INFO("Stop Disk Remove\n");
    return NULL;
}
static int SDOPS_SoftHotplugTFCard()
{
    int ret = 0;
    static int hotplug_cnt = 0;
    static int remove_cnt = 0;
    if (hotplug_cnt < 5)
    {
        if (-1 == g_DiskSoftHotPlugStatus)
        {
            hotplug_cnt++;
            if (pthread_create(&g_DiskSoftHotPlugStatus, NULL, RunDiskSoftHotplugThread, NULL))
            {
                SD_OPS_WRAN("Fail to create CheckDiskStatusThread thread!\n");
            }
        }
        else
        {
            ret = -1;
            SD_OPS_ERROR("thread softhotplug is running, return...\n");
        }
    }
    else
    {
        if ((-1 == g_DiskRemoveStatus) && (-1 == g_DiskSoftHotPlugStatus))
        {
            remove_cnt++;
            if (pthread_create(&g_DiskRemoveStatus, NULL, RunDiskRemoveThread, NULL))
            {
                SD_OPS_WRAN("Fail to create CheckDiskStatusThread thread!\n");
            }
        }
        else
        {
            ret = -1;
            SD_OPS_ERROR("thread remove or softhotplug is running, return...\n");
        }
    }

    SD_OPS_ERROR("SoftHotplug check hotplug_cnt:%d, remove_cnt:%d\n", hotplug_cnt, remove_cnt);
    return ret;
}
#endif
/* 获取文件大小 */
static int GetFileSize(const char *path)  
{  
    int filesize = -1;      
    struct stat statbuff;

    if (path == NULL)
    {
        SD_OPS_ERROR("NULL POINTER EXCEPTION\n");
        return -1;
    }
    
    if(stat(path, &statbuff) < 0)
    {  
        return filesize;  
    }
    else
    {  
        filesize = statbuff.st_size;  
    }  
    return filesize;  
}

/* 读写文件按照64KB为单位进行 */
static int copy_align(const char *src_file, const char *dest_file)
{
    if (!src_file || !dest_file)
    {
        SD_OPS_ERROR("NULL POINT EXCEPTION!\n");
        return -1;
    }

    int src_fd = -1;
    int dest_fd = -1;
    int bytes_read = 0;
    int bytes_write = 0;    
    char *ptr = NULL;
    //char buffer[WRITE_MAX_BUF] = {0};
    char *buffer = NULL;

    buffer = (char *)malloc(WRITE_MAX_BUF);
    if (NULL == buffer)
    {
        printf("malloc picture buffer failed, size=%d\n", WRITE_MAX_BUF);
        return -1;
    }

    /* 打开源文件 */
    if((src_fd=open(src_file, O_RDONLY)) == -1)
    {
        SD_OPS_ERROR("Open %s Fail!\n", src_file);
        SAFE_FREE(buffer);
        return -1;
    }

    /* 打开目的文件 */
    if((dest_fd=open(dest_file, O_RDWR | O_CREAT | O_TRUNC, 0777)) == -1)
    {
        SD_OPS_ERROR("Open %s Fail!\n", dest_file);
        SAFE_CLOSE(src_fd);
        SAFE_FREE(buffer);
        return -1;
    }

    /* 文件拷贝 */
    while((bytes_read=read(src_fd, buffer, sizeof(buffer))) > 0)
    {
        /* 一个致命的错误发生了 */
        if((bytes_read==-1) && (errno!=EINTR))
        {
            break;
        }
        else if(bytes_read > 0)
        {
            ptr = buffer;

            while((bytes_write=write(dest_fd, ptr, bytes_read)) > 0)
            {
                /* 一个致命错误发生了 */
                if((bytes_write==-1) && (errno!=EINTR))
                {
                    break;
                }
                /* 写完了所有读的字节 */
                else if(bytes_write == bytes_read) 
                {
                    break;
                }
                /* 只写了一部分,继续写 */
                else if(bytes_write > 0)
                {
                    ptr += bytes_write;
                    bytes_read -= bytes_write;
                }
            }
            /* 写的时候发生的致命错误 */
            if(bytes_write == -1)
            {
                break;
            }
        }
    }
    SAFE_CLOSE(src_fd);
    SAFE_CLOSE(dest_fd);
    SAFE_FREE(buffer);

    return 0;
}

static void rsleep(unsigned int ntime)
{
    fd_set rfds;

    struct timeval tv;

    FD_ZERO(&rfds);
    FD_SET(0, &rfds);            /* Wait up to five seconds. */
    tv.tv_sec = ntime / 1000000;
    tv.tv_usec = ntime % 1000000;
    select(0, &rfds, NULL, NULL, &tv);    /* Don't rely on the value of tv now! */

    return;
}

/* 递归创建目录 */
static int CreateDirent(const char *sPathName)
{
    int i = 0;
    char DirName[256] = {0};  

    DIR *pdir = NULL;

    if (sPathName == NULL)
    {
        SD_OPS_ERROR("NULL POINTER EXCEPTION\n");
        return -1;
    }

    if (0 == access(sPathName, F_OK))
    {
        return 0;
    }

    strncpy(DirName, sPathName, sizeof(DirName) - 1);

    size_t len = strlen(DirName);
    if (len > 0 && DirName[len - 1] != '/')
    {
        strncat(DirName, "/", sizeof(DirName) - len - 1);
    }

    for (i=1; i<strlen(DirName); i++)
    {  
        if (DirName[i]=='/')  
        {  
            DirName[i] = 0;
            pdir = opendir(DirName);
            if (NULL == pdir)
            {  
                if (mkdir(DirName, 0755) == -1) 
                {   
                    perror("mkdir error");   
                    return -1; 
                }
            }
            else
            {
                closedir(pdir);
            }
            DirName[i] = '/';
        }
    }
    return 0;
}

static int GetFileLines(const char* pFile)
{
    if (NULL == pFile)
    {
        fprintf(stderr, "NULL POINTER EXCEPTION\n");
        return -1;
    }

    FILE *fp = NULL; 
    char buf_line[512] = {0}; 
    int lines = 0;

    fp = fopen(pFile, "r");
    if(NULL == fp)
    {
        fprintf(stderr, "Fail to fopen %s: %s\n", pFile, strerror(errno));
        return 0;
    }

    while(!feof(fp))
    {
        fgets(buf_line, sizeof(buf_line), fp);
        lines ++;
    }

    fclose(fp);
    fp = NULL;

    return lines;
}

static int WriteLog(const char* pFile, const char *format, ...)
{
    if (!pFile)
    {
        printf("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    int lines = 0;
    FILE *fp = NULL;

    lines = GetFileLines(pFile);
    if (lines >= MAX_LOG_LINE)
    {
        fp = fopen(pFile, "r+");
    }
    else
    {
        fp = fopen(pFile, "a+");
    }

    if (!fp)
    {
        fprintf(stderr, "Fail to fopen %s: %s\n", pFile, strerror(errno));
        return -1;
    }

    va_list arg;
    int done = 0;

    va_start (arg, format);

    //获取当前时间段信息
    struct tm tm_log;
    time_t time_log = time(NULL);
    localtime_r(&time_log, &tm_log);
    
    fprintf(fp, "[%04d-%02d-%02d %02d:%02d:%02d] ", 
            tm_log.tm_year + 1900, 
            tm_log.tm_mon + 1, 
            tm_log.tm_mday, 
            tm_log.tm_hour, 
            tm_log.tm_min, 
            tm_log.tm_sec);

    done = vfprintf (fp, format, arg);
    va_end (arg);

    fflush(fp);
    fclose(fp);
    fp = NULL;
        
    return done;
}

#if !defined(__CLOSE_SWAP__)
//swap开启线程
//这里后续还要完善一下，swap启动过程中
//执行格式化的处理
static void *SwapOpenThread(void *pParam)
{
    PRT_TID();
    pthread_detach(pthread_self());

    int nRet = 0;
    int fdSwap = -1;
    int nWroteSize = 0;
    int nWhileCntMax = 0;
    int nFileSize = 0;
    char szStrTmp[256] = {0};
    char *pBuffer = NULL;
    
    //swap启动中，再次进来直接返回
    if (1 == s_nSwapRunStatus)
    {
        printf("Swap is startting!\n");
         pthread_exit(NULL);
        return NULL;
    }
    //swap已启动，再次进来直接返回
    else if (2 == s_nSwapRunStatus)
    {
        printf("Swap is running!\n");
         pthread_exit(NULL);
        return NULL;
    }
    else
    {
        s_nSwapRunStatus = 1;
    }

    //是否需要中断此线程
    if (s_bIsSwapOpenThreadExit)
    {
        s_bIsSwapOpenThreadExit = 0;
        s_nSwapRunStatus = 0;
        printf("SwapOpenThread exit!\n");
         pthread_exit(NULL);
        return NULL;
    }
        
    //文件不存在或文件大小不符合预期则重新创建
    //防止上一次创建swap文件中途设备重启
    //导致本次swap启动可能出现异常
    nFileSize = GetFileSize(OS_SWAP_FILE_PATH);
    SD_OPS_DEBUG("\n%s nFileSize=%d\n", OS_SWAP_FILE_PATH, nFileSize);
    if (nFileSize <= 0
        || nFileSize < OS_SWAP_FILE_SIZE_BYTE)
    {
        //创建交换文件
        fdSwap = open(OS_SWAP_FILE_PATH, O_WRONLY | O_CREAT | O_TRUNC);
        if (0 >= fdSwap)
        {
            printf("open file %s fail!\n", OS_SWAP_FILE_PATH);
            s_bIsSwapOpenThreadExit = 0;
            s_nSwapRunStatus = 0;
            pthread_exit(NULL);
        }

        pBuffer = (char*)calloc(1, OS_SWAP_FILE_WRITE_BLOCK_SIZE_BYTE);
        if (NULL == pBuffer)
        {
            printf("NULL == pBuffer!\n");
            if (fdSwap > 0)
            {
                close(fdSwap);
                fdSwap = -1;
            }
            s_bIsSwapOpenThreadExit = 0;
            s_nSwapRunStatus = 0;
            pthread_exit(NULL);
        }
    
        //防止可能的死循环
        nWhileCntMax = (OS_SWAP_FILE_SIZE_BYTE / OS_SWAP_FILE_WRITE_BLOCK_SIZE_BYTE) * 2;
        while (nWhileCntMax--)
        {
            //是否需要中断此线程
            if (s_bIsSwapOpenThreadExit)
            {
                if (fdSwap > 0)
                {
                    close(fdSwap);
                    fdSwap = -1;
                }
                if (NULL != pBuffer)
                {
                    free(pBuffer);
                    pBuffer = NULL;
                }

                s_bIsSwapOpenThreadExit = 0;
                s_nSwapRunStatus = 0;
                printf("SwapOpenThread exit!\n");
                 pthread_exit(NULL);
                return NULL;
            }
        
            nRet = write(fdSwap, pBuffer, OS_SWAP_FILE_WRITE_BLOCK_SIZE_BYTE);
            if (nRet < 0)
            {
                printf("write file %s fail!\n", OS_SWAP_FILE_PATH);
            }

            nWroteSize += nRet;
            if (nWroteSize >= OS_SWAP_FILE_SIZE_BYTE)
            {
                break;
            }
                
            //20ms睡眠，避免长时间占用CPU
            //usleep(10000);
            usleep(20000);
        }

        if (fdSwap > 0)
        {
            close(fdSwap);
            fdSwap = -1;
        }
        if (NULL != pBuffer)
        {
            free(pBuffer);
            pBuffer = NULL;
        }
    }

    //是否需要中断此线程
    if (s_bIsSwapOpenThreadExit)
    {
        s_bIsSwapOpenThreadExit = 0;
        s_nSwapRunStatus = 0;
        printf("SwapOpenThread exit!\n");
         pthread_exit(NULL);
        return NULL;
    }
        
    //初始化交换文件
    memset(szStrTmp, 0, sizeof(szStrTmp));
    snprintf(szStrTmp, sizeof(szStrTmp), "mkswap %s %d", 
        OS_SWAP_FILE_PATH, (OS_SWAP_FILE_SIZE_BYTE / 1024));
    mysystem(szStrTmp);

    //启动swap
    memset(szStrTmp, 0, sizeof(szStrTmp));
    snprintf(szStrTmp, sizeof(szStrTmp), "swapon %s", OS_SWAP_FILE_PATH);
    mysystem(szStrTmp);

    s_bIsSwapOpenThreadExit = 0;
    s_nSwapRunStatus = 2;
    
    pthread_exit(NULL);
    return NULL;
}
#endif

/*****************************************静态函数结束********************************/
static const char * EnumTypeMapStr(T_MAP_TYPE map_type, int e_type)
{
    int i = 0; 
    if (E_FILE_MAP == map_type)
    {
        int count = sizeof(s_gFileType) / sizeof(s_gFileType[0]);
        for (i=0; i<count; i++)
        {
            if ((T_FILE_TYPE)e_type == s_gFileType[i].eFileType)
            {
                return s_gFileType[i].szFileTypeSuffix;
            }
        }
    }
    else if (E_REC_MAP == map_type)
    {
        int count = sizeof(s_gRecType) / sizeof(s_gRecType[0]);
        for (i=0; i<count; i++)
        {
            if ((eRecordType)e_type == s_gRecType[i].eRecType)
            {
                return s_gRecType[i].szRecTypeSuffix;
            }
        }
    }

    return NULL;
}

static int StrMapEnumType(T_MAP_TYPE map_type, const char *p_str_suffix)
{
    int i = 0;    
    if (E_FILE_MAP == map_type)
    {
        int count = sizeof(s_gFileType) / sizeof(s_gFileType[0]);
        for (i=0; i<count; i++)
        {
            if (strstr(s_gFileType[i].szFileTypeSuffix, p_str_suffix))
            {
                return s_gFileType[i].eFileType;
            }
        }
        return E_FILE_ERR;
    }
    else if (E_REC_MAP == map_type)
    {
        int count = sizeof(s_gRecType) / sizeof(s_gRecType[0]);
        for (i=0; i<count; i++)
        {
            if (strstr(s_gRecType[i].szRecTypeSuffix, p_str_suffix))
            {
                return s_gRecType[i].eRecType;
            }
        }
        return eNoRecord;
    }
	return 0;
}

/* 映射录像类型: 上层 ---> 底层 */
static inline eRecordType ToSdRecordTypeMap(eNetRecordType net_record_type)
{
    switch(net_record_type)
    {
        case NORMAL_RECORD:    return eNormalRecord;
        case MOTION_RECORD:    return eMotionRecord;
        case MANUAL_RECORD:    return eManualRecord;
        case ALARMIN_RECORD:   return eAlarmInRecord;
        case HTTP_TRIG_RECORD: return eHttpTrigRecord;
        case AI_RECORD:        return eAiRecord;
        default: return eNoRecord;
    }
}

/* 映射录像类型: 底层 ---> 上层 */
static inline eNetRecordType ToNetRecordTypeMap(eRecordType sd_record_type)
{
    switch(sd_record_type)
    {
        case eNormalRecord:   return NORMAL_RECORD;
        case eMotionRecord:   return MOTION_RECORD;
        case eManualRecord:   return MANUAL_RECORD;
        case eAlarmInRecord:  return ALARMIN_RECORD;
        case eHttpTrigRecord: return ALARMIN_RECORD; // 当前仅仅用报警输入代替HTTP触发事件
        case eAiRecord:       return AI_RECORD;
        default: return eNormalRecord;
    }
}

/**************************************************************************************************/

/* 初始化双向链表 */
AviSegLinkList  *InitNewAviSegLinklist(int b_get_link_list)
{
    int array_index = 0;

    if (b_get_link_list)
    {
        array_index = 0;
    }
    else
    {
        array_index = 2;
    }

    if(NULL == SEG_LINK[array_index])
    {
#ifndef TEST_MEM_INFO
        SEG_LINK[array_index] = (AviSegLinkList *)malloc(sizeof(AviSegLinkList));
#else
        SEG_LINK[array_index] = (AviSegLinkList *)MemoryMalloc(sizeof(AviSegLinkList), "sd_ops", __FILE__, __FUNCTION__, __LINE__);
#endif
        if(NULL == SEG_LINK[array_index])
        {
            SD_OPS_ERROR("Fail to malloc!\n");
            return NULL;
        }
        memset((char *)SEG_LINK[array_index], 0, sizeof(AviSegLinkList));
        return SEG_LINK[array_index];
    }

    if(NULL == SEG_LINK[array_index+1])
    {
#ifndef TEST_MEM_INFO
        SEG_LINK[array_index+1] = (AviSegLinkList *)malloc(sizeof(AviSegLinkList));
#else
        SEG_LINK[array_index+1] = (AviSegLinkList *)MemoryMalloc(sizeof(AviSegLinkList), "sd_ops", __FILE__, __FUNCTION__, __LINE__);
#endif
        if(NULL == SEG_LINK[array_index+1])
        {        
            SD_OPS_ERROR("Fail to malloc!\n");
            return NULL;
        }
        memset((char *)SEG_LINK[array_index+1], 0, sizeof(AviSegLinkList));
        return SEG_LINK[array_index+1];
    }

    SD_OPS_ERROR("POP Full! SEG_LINK[%d]=%p, SEG_LINK[%d]=%p\n", array_index, SEG_LINK[array_index], array_index+1, SEG_LINK[array_index+1]);

    return NULL;
}

/* 添加一个双向链表节点 */
AviSegLinkList *AddNewAviSegLinkList(AviSegLinkList *plink)
{
    if (NULL == plink)
    {
        SD_OPS_ERROR("NULL POINTER EXCEPTION!\n");
        return NULL;
    }

    AviSegLinkList *p_tmp_link = NULL;
#ifndef TEST_MEM_INFO
    p_tmp_link = (AviSegLinkList *)malloc(sizeof(AviSegLinkList));
#else
    p_tmp_link = (AviSegLinkList *)MemoryMalloc(sizeof(AviSegLinkList), "sd_ops", __FILE__, __FUNCTION__, __LINE__);
#endif
    if (NULL == p_tmp_link)
    {
        plink->next_link = NULL; 
        return NULL;
    }
    
    plink->next_link = (void *)p_tmp_link;
    memset(plink->next_link, 0, sizeof(AviSegLinkList));
    p_tmp_link->prev_link = (void *)plink;

    return p_tmp_link;
}

/* 释放整个链表 */
void FreeAviSegLinkLists(int b_get_link_list, AviSegLinkList *link_head)
{
    if (NULL == link_head)
    {
        SD_OPS_ERROR("NULL POINTER EXCEPTION!\n");
        return;
    }

    int array_index = 0;

    if (b_get_link_list)
    {
        array_index = 0;
    }
    else
    {
        array_index = 2;
    }

    AviSegLinkList *tmp = NULL;
    AviSegLinkList *tmp1 = NULL;

    tmp = link_head;

    while(NULL != tmp->prev_link)
    {
        tmp = (AviSegLinkList *)tmp->prev_link;
    }
    
    tmp1 = tmp;
    
    if(tmp1 == SEG_LINK[array_index])
    {
        SEG_LINK[array_index] = NULL;
    }
    else
    {
        SEG_LINK[array_index+1] = NULL;
    }
    
    while(NULL != tmp1)
    {
        tmp = (AviSegLinkList *)tmp1->next_link;
#ifndef TEST_MEM_INFO
        free(tmp1);
#else
        MemoryFree(tmp1);
#endif
        tmp1 = tmp;     
    }
}

/* 初始化弹出句柄 */
void InitPopHandle(void)
{
    pop_handle = NULL;
}

/* 创建一个弹出句柄(只允许创建两个弹出句柄) */
void  *CreateOnePopHandle(int *curr_handle)
{
	unsigned int i = 0;

	for (i = 0; i < MAX_OPERATE_HANDLE;i++)
	{
		if (pop_handle == NULL)
		{
       		pop_handle = (PopHandle *)malloc(sizeof(PopHandle));
	        pop_handle->read_link_handle = NULL;
	        *curr_handle = i;
	        return (void *)pop_handle;
		}
	}

	SD_OPS_ERROR("pop_handle full\n");

	return NULL;
}

/* 销毁一个弹出句柄 */
int DestroyOnePopHandle(int handle)
{
	unsigned int i = 0;
	int found = 0;

	for (i = 0; i < MAX_OPERATE_HANDLE; i++)
	{
		if (pop_handle == (PopHandle *)handle)
    	{
	        SD_OPS_DEBUG("release pop_handle[%d]=%p\n", i, pop_handle);
            pop_handle = NULL;
	        found = 1;
	        break;
    	}
	}

	if (0 == found)
	{
		SD_OPS_ERROR("destroy handle wrong, handle=%d\n", handle);
        return -1;
	}

	return 0;
}

/* 释放一个弹出句柄 */
void FreeOnePopHandle(int handle)
{
    PopHandle *pophandle = (PopHandle *)handle;
    SAFE_FREE(pophandle);

    return;
}

/* 检测SD卡是否在运行 */
int CheckSdCardIsRunning(void)
{
    //是否有人在查询
    if((NULL != SEG_LINK[0]) || (NULL != SEG_LINK[1]))
    {
        SD_OPS_ERROR("have people get rec seg list now!\n");
        return -1;
    }
    //是否有人在回放
    if((NULL != SEG_LINK[2]) || (NULL != SEG_LINK[3]))
    {
        SD_OPS_ERROR("have people look for rec date now!\n");
        return -1;
    }

    return 0;
}

static int com_avi_i_frame_time(const void *a, const void *b)
{
    const IFrameInfo *aa = (const IFrameInfo *)a;
    const IFrameInfo *bb = (const IFrameInfo *)b;

    if (aa->CurIFrameTime < bb->CurIFrameTime)
    {
        return -1;
    }
    else if (aa->CurIFrameTime > bb->CurIFrameTime)
    {
        return 1;
    }
    else
    {
        /* 多通道录像，时间戳秒数相同的I帧，再通过偏移地址进行排序，优化录像回放同步 */
        if (aa->CurrFramePos < bb->CurrFramePos)
        {
            return -1;
        }
        else if (aa->CurrFramePos > bb->CurrFramePos)
        {
            return 1;
        }
        return 0;
    }
}

/* 删除多余文件及文件夹 */
static int DeleteOtherFiles(const char *dir)
{
    DIR *dp = NULL;
    struct dirent *entry = NULL;
    char dest_path[MAX_PATH_STR_NUM]  = {0};
    char customSdUpgradeName[128] = {0};
    FILE *fp = NULL;

    if (NULL == dir)
    {
        SD_OPS_ERROR("NULL POINTER EXCEPTION!\n");
        return -1;
    }
    
    if ((dp = opendir(dir)) == NULL)
    {
        SD_OPS_ERROR("can't open directory: %s\n", dir);
        return -1;
    }

	if (!access(CUSTOM_SD_UPGRADE_FILE, F_OK))
	{
		if (NULL == fp)
		{
			fp = fopen(CUSTOM_SD_UPGRADE_FILE, "r");
		}

		if (NULL != fp)
		{
			fread(customSdUpgradeName, sizeof(customSdUpgradeName), 1, fp);
			fclose(fp);
		}
        PubFunc_deleteNewlineInString(customSdUpgradeName, sizeof(customSdUpgradeName));
	}

    while ((entry = readdir(dp)) != NULL)
    {
        if((0==strcmp(entry->d_name, "video"))
        || (0==strcmp(entry->d_name, "picture"))
        || (0==strcmp(entry->d_name, "input"))
        || (0==strcmp(entry->d_name, "output"))
        || (0==strcmp(entry->d_name, "WifiInfo.txt"))
        || (0==strcmp(entry->d_name, "auth"))
        || (0==strncmp(entry->d_name, IPC_SD_CARD_UPGRADE_FILE_NAME_PREFIX, strlen(IPC_SD_CARD_UPGRADE_FILE_NAME_PREFIX)))
        || (0==strncmp(entry->d_name, IPC_SD_CARD_UPGRADE_FILE_NAME_FOR_SYSTEM, sizeof(entry->d_name)))
        || (0==strncmp(entry->d_name, SD_UPGRADE_FLAG_FILE_NAME, sizeof(entry->d_name)))
        || (0==strncmp(entry->d_name, IPC_SD_CARD_NETCONFIG_FILE_NAME, sizeof(entry->d_name)))
        || (0==strncmp(entry->d_name, SD_PTZ_TEST_FLAG_FILE_NAME, sizeof(entry->d_name)))
        || (0==strncmp(entry->d_name, DEVICE_LOG_DIR, sizeof(entry->d_name)))
#if defined __AGINGTEST__
        || (0==strncmp(entry->d_name, "aging_test.list", sizeof(entry->d_name)))
        || (0==strncmp(entry->d_name, "aging_test.g711u", sizeof(entry->d_name)))
#endif
        || (0==strncmp(entry->d_name, OS_SWAP_FILE_NAME, sizeof(entry->d_name)))
        || (0 == strcmp(entry->d_name, ".")) 
        || (0 == strcmp(entry->d_name, ".."))
		|| (0 == strcmp(entry->d_name, "restore.bin"))
		|| (0 == strcmp(entry->d_name, "upg.txt"))
		|| (0 == strcmp(entry->d_name, customSdUpgradeName)))
        {
            continue;
        }

        memset(dest_path, 0, sizeof(dest_path));
        snprintf(dest_path, sizeof(dest_path), "%s/%s", dir, entry->d_name);
        SAFE_REMOVE(dest_path);
    }

    closedir(dp);
    dp = NULL;

    return 0;
}

/*@breif UploadPictureToSdCard 导入一个图片到SD卡里面(写入SD卡)
 *@param [IN] pic_name 图片的名字(图片名字组成: 时间戳_标号)
 *@param [IN] trig_type 触发类型
 *@return 成功返回0
 *@      失败返回-1
 *@note   1. 传入的图片名字组成: 时间戳_标号，如:1532477851_01需要改为1532477851;
 *@       2. 图片的个数上限为MAX_PICTURE_NUMBERS个，可以修改;
 *@       2. 当图片个数到达上限，则采用删除最旧的文件的方式进行循环删除;
 *@       3. 在图片从内存拷贝到SD卡的过程中，采用64字节对齐的copy方式，提高写效率;
 *@       4. 在循环覆盖的时候，需要及时更新图片全局结构体索引，类似于AVI录像
 */
extern int UploadPictureToSdCard(const char *pic_name, unsigned int trig_type)
{
    if (NULL == pic_name)
    {
        SD_OPS_ERROR("NULL POINTER EXCEPTION\n");
        return -1;
    }

    char szTempPicFile[MAX_PATH_STR_NUM] = {0};
    snprintf(szTempPicFile, sizeof(szTempPicFile), "%s/%s", CAPTURE_MEM_DIR_FOR_SD, pic_name);

    if (0 == g_sd_card_has_inited)
    {
        SAFE_REMOVE(szTempPicFile);
        return -1;
    }

    if (Formatting == g_Sd_format_status)
    {
        SD_OPS_DEBUG("Sd Card format status is %d not ok!\n", g_Sd_format_status);
        SAFE_REMOVE(szTempPicFile);
        return 0;
    }

    if (trig_type < 0 || trig_type >= eRecordTypeNum)
    {
        SD_OPS_ERROR("trig_type is %d out of [0, %d), use default: timer\n", 
            trig_type, eRecordTypeNum);
        trig_type = eNormalRecord;
    }

    char pic_dir[64] = {0};
    char pic_name_string[64] = {0};
    char pic_name_timestamp[64] = {0};
    char src_file[MAX_PATH_STR_NUM] = {0};
    char dst_file[MAX_PATH_STR_NUM] = {0};
    char pic_abs_dir[MAX_PATH_STR_NUM] = {0};

    int ret = -1;
    unsigned int time_stamp = 0;
    time_t timeTemp;

    if (strchr(pic_name, '_'))
    {
        memcpy(pic_name_timestamp, pic_name, strchr(pic_name, '_')-pic_name);

        /* 如果出现和前面一模一样的图片信息，则跳过去 */
        if (0 == strcmp(pic_name_timestamp, g_last_pic_name_pre))
        {
            SD_OPS_DEBUG("pic_name_timestamp = %s are same as last upload!\n", pic_name_timestamp);
            SAFE_REMOVE(szTempPicFile);
            return 0;
        }
    }
    else
    {
        SAFE_REMOVE(szTempPicFile);
        return -1;
    }

    /* 过滤掉: 1970-01-01 08:00:00    */
    if ((strlen(pic_name_timestamp)>0) && (0==atoi(pic_name_timestamp)))
    {
        SD_OPS_ERROR("Error timestamp: %s, becase is 1970-01-01 08:00:00\n", pic_name);
        SAFE_REMOVE(szTempPicFile);
        return 0;
    }

    sem_post(&g_semPicSpaceCheckThread);

    /* 将文件名转化为时间戳 */
    time_stamp = atoi(pic_name_timestamp);
    timeTemp = time_stamp;
    TIME_ULONG_TO_STRING(timeTemp, pic_name_string);

    /* 文件夹创建 */
    TIME_STAMP_TO_STRING_DAY(timeTemp, pic_dir);
    snprintf(pic_abs_dir, sizeof(pic_abs_dir), "%s/picture/%s", 
              SD_MOUNT_POINT, pic_dir);
    CreateDirent(pic_abs_dir);

    snprintf(src_file, sizeof(src_file), "%s/%s", CAPTURE_MEM_DIR_FOR_SD, pic_name);

    if (eNormalRecord == trig_type)
    {
        snprintf(dst_file, sizeof(dst_file), "%s/%s.jpg", pic_abs_dir, pic_name_string);
    }
    else if (eSoundRecord == trig_type)
    {
        snprintf(dst_file, sizeof(dst_file), "%s/S_%s.jpg", pic_abs_dir, pic_name_string);
    }
    else
    {
        snprintf(dst_file, sizeof(dst_file), "%s/%s_%s.jpg", pic_abs_dir, EnumTypeMapStr(E_REC_MAP, trig_type), pic_name_string);
    }

    if (access(src_file, F_OK))
    {
        SD_OPS_ERROR("src image %s not exist!\n", src_file);
        SAFE_REMOVE(szTempPicFile);
        return -1;
    }

    pthread_mutex_lock(&global_disk_write_mutex);

    g_sd_write_status = SdCardIsWriting;

    ret = copy_align(src_file, dst_file);
    if (ret)
    {
        SD_OPS_ERROR("Fail to copy %s to %s\n", src_file, dst_file);
        SAFE_UNLINK(src_file);
        SAFE_REMOVE(szTempPicFile);
        g_sd_write_status = SdCardIsIDEL;        
        pthread_mutex_unlock(&global_disk_write_mutex);
        return -1;
    }

    SAFE_UNLINK(src_file);
    SAFE_REMOVE(szTempPicFile);

    strncpy(g_last_pic_name_pre, pic_name_timestamp, sizeof(g_last_pic_name_pre) - 1);

    g_sd_write_status = SdCardIsIDEL;

    pthread_mutex_unlock(&global_disk_write_mutex);

    return 0;
}

/***********************************************************************/

/**
 * @brief SDOPS_DebugPrint_DiskDeviceInfoEx
 * @brief debug use
 */
static int SDOPS_DebugPrint_DiskDeviceInfoEx (const DiskDeviceInfoEx *psDiskInfoEx, 
    const char *file, int line, const char *function)
{
#ifndef SDOPS_SHOW_DETAIL_DEBUGINFO
    return 0;
#endif // SDOPS_SHOW_DETAIL_DEBUGINFO

    printf ("[%s %d %s] ", file, line, function);

    printf ("DiskInfoEx: name=%s,no=%s,dev=%s,totoalspace=%lu,"
        "iswriting=%d,rwstatus=%d,partionnum=%d\n",
        psDiskInfoEx->DeviceName,
        psDiskInfoEx->SerialNo,
        psDiskInfoEx->Dev,
        psDiskInfoEx->Total_disk_space,
        psDiskInfoEx->IsWritting,
        psDiskInfoEx->RW_Status,
        psDiskInfoEx->PartitionsNum);
    
    const int nMax = ARRAY_SIZEOF (psDiskInfoEx->Partitions);
	int i = 0;
    for (i = 0; i < nMax && i < psDiskInfoEx->PartitionsNum; ++i)
    {
        const PARTITONDEVICEINFO *p = & (psDiskInfoEx->Partitions[i]);

        printf ("{[%d] type=%d,par_has_mounted=%d,total_partition_space=%lu[%.2f G],"
                "available_space=%lu[%.2f G],devname=,mount_point=,dwStartCHS=%lu,dwEndtCHS=%lu}\n",
                i,
                p->Type, 
                p->Partition_has_mounted,
                p->Total_partitions_space,
                p->Total_partitions_space / (1024.0),
                p->Available_partitions_space,
                p->Available_partitions_space / (1024.0),
                // p->DevName,
                // p->Mount_point,
                p->dwStartCHS,
                p->dwEndtCHS);
    }

    return 0;
}

static int CheckDirFileTypeExist(const char* dir_path, const char* file_type, int recursive_check)
{
    if (!dir_path || !file_type)
    {
        SD_OPS_ERROR("NULL POINTER EXCEPTION\n");
        return -1;
    }

    if (access(dir_path, F_OK))
    {
        SD_OPS_ERROR("dir %s not exist!\n", dir_path);
        return -1;
    }

    DIR *dp = NULL;    
    int is_exist = 0;
    struct dirent *entry = NULL;

    if ((dp = opendir(dir_path)) == NULL)
    {
        return -1;
    }

    while ((entry = readdir(dp)) != NULL)
    {
        if((0 == strcmp(entry->d_name, ".")) || (0 == strcmp(entry->d_name, "..")))
        {
            continue;
        }

        if (4 == entry->d_type)
        {
            if (recursive_check)
            {
                char subdir[256] = {0};
                snprintf(subdir, sizeof(subdir), "%s/%s", dir_path, entry->d_name);
                is_exist = CheckDirFileTypeExist(subdir, file_type, recursive_check);
            }
        }
        else
        {
            if (strstr(entry->d_name, file_type))
            {
                SD_OPS_DEBUG("file %s has %s type\n", entry->d_name, file_type);
                is_exist = 1;
                break;
            }
            else
            {
                is_exist = 0;
            }
        }
    }
    closedir(dp);
    dp = NULL;

    return is_exist;
}

/* 文件系统相关初始化，完成的工作有:
 * 1. 获取当前是否有一个SD卡；
 * 2. 获取SD卡相关信息，存放到全局结构体里面；
 * 3. 判断SD卡分区是否为Fat32格式，如果不是，则需要格式化为Fat32格式；
 * 4. 判断是否为新的SD卡，如果是，则不需要重新初始化； 
 * 5. 判断是否挂载，如果没有挂载，则需要挂载；
 */
static int InitFileSystem(void)
{
    int ret = 0;
    
    /* 获取当前SD卡的个数，当前只支持一个SD卡 */
    ret = EDVR_HD_GetTotalDiskCount();
    if (1 != ret)
    {
        SD_OPS_DEBUG("Sd Card count %d Detect Fail!\n", ret);

        return 1;
    }

    /* 获取SD卡的设备名，存放在全局结构体里面 */
    memset(&g_diskinfo, 0, sizeof(g_diskinfo));
    ret = EDVR_HD_GetDiskInfoEx(0, &g_diskinfo);
    if (0 != ret)
    {
        SD_OPS_ERROR("EDVR_HD_GetDiskInfoEx return %d\n", ret);
        return ret;
    }

    /* 创建挂载点 */
    CreateDirent(SD_MOUNT_POINT);

    /* 判断SD卡分区是否有FAT32格式(g_diskinfo.PartitionsNum为0表示没有FAT32分区)，如果没有，则需要格式化 */
    SD_OPS_DEBUG("g_diskinfo.PartitionsNum = %d\n", g_diskinfo.PartitionsNum);
    SD_OPS_DEBUG("g_diskinfo.Partitions[0].DevName = %s\n", g_diskinfo.Partitions[0].DevName);

    if (g_diskinfo.PartitionsNum == 0)
    {
        if (g_diskinfo.PartitionsNum == 0)
        {            
            SD_OPS_ERROR("SD Card has no parts, need to format it!\n");

            ret = EDVR_HD_FormatPartition(g_diskinfo.Partitions[0].DevName);
            if (ret == 0)
	        {
				Change_DevFileSystemType(g_diskinfo.Dev,1,FS_FAT32_VALUE);
	        }
			
            if(0 != ret)
            {
                SD_OPS_ERROR("Format SD Card %s Fail!\n", g_diskinfo.Dev);
                return ret;
            }
        }
        /* 重新获取相关参数 */
        memset(&g_diskinfo, 0, sizeof(g_diskinfo));
        ret = EDVR_HD_GetDiskInfoEx(0, &g_diskinfo);
        if (0 != ret)
        {
            SD_OPS_ERROR("EDVR_HD_GetDiskInfoEx return %d\n", ret);
            return ret;
        }
        SD_OPS_DEBUG("Format %s success!\n", g_diskinfo.Partitions[0].DevName);        
    }
    strncpy(g_sd_dev, g_diskinfo.Dev, sizeof(g_sd_dev) - 1);
    
    /* 判断SD卡是否为新磁盘，如果不是，则需要重新初始化 */
    ret = EDVR_HD_IsNewDiskEx(g_sd_dev);
    if (1 == ret)
    {
        SD_OPS_DEBUG("%s is new disk, ret = %d, need to format!\n", g_sd_dev, ret);
        ret = EDVR_HD_Init(g_sd_dev);
        {
            SD_OPS_ERROR("Init SD Card Fail! ret = %d\n", ret);            
            return ret;
        }
        rsleep(1000000);
        /* 重新获取SD卡信息 */
        memset(&g_diskinfo, 0, sizeof(g_diskinfo));
        ret = EDVR_HD_GetDiskInfoEx(0, &g_diskinfo);
        if (0 != ret)
        {
            SD_OPS_ERROR("EDVR_HD_GetDiskInfoEx return %d\n", ret);
            return ret;
        }
    }
    else if (-1 == ret)
    {
        SD_OPS_ERROR("EDVR_HD_IsNewDiskEx Fail return %d\n", ret);
        return ret;
    }
    else
    {
    }

REMOUNT_DISK:
    /* 判断是否已经挂载，如果没有挂载，则需要挂载 */
    if (1 != g_diskinfo.Partitions[0].Partition_has_mounted)
    {
        /* 挂载分区，如果挂载失败则执行dosfsck */
        ret = EDVR_HD_MountPartition(g_diskinfo.Partitions[0].DevName);
        if (0 != ret)
        {
            /* 如果还是挂载失败，则需要格式化SD卡为FAT32格式 */
            SD_OPS_DEBUG("mount %s fail! need to format sd card to fat32!\n", g_diskinfo.Partitions[0].DevName);
			
            ret = EDVR_HD_FormatPartition(g_diskinfo.Partitions[0].DevName);
            if (ret == 0)
	        {
				Change_DevFileSystemType(g_diskinfo.Dev,1,FS_FAT32_VALUE);
	        }
			
            if(0 != ret)
            {
                SD_OPS_ERROR("Format SD Card %s Fail!\n", g_diskinfo.Partitions[0].DevName);
                return ret;
            }
            /* 再次尝试挂载，如果还是失败，则直接返回出错 */
            ret = EDVR_HD_MountPartition(g_diskinfo.Partitions[0].DevName);
            if (0 != ret)
            {
                SD_OPS_ERROR("mount %s fail!\n", g_diskinfo.Partitions[0].DevName);
                return ret;
            }
        }
    }

    /* 为了剔除旧录像方案，这里针对旧录像方案做出检测，如果符合条件，需要对SD卡进行格式化：
     * 1. 根目录下面存在全局索引文件(.db)；
     * 2. video目录下面存在.prv录像文件；
     */
    int i = 0;
    int exist = 1;
    const char *check_file_list[] = {
        "global_index.db",
        "global_index_bak.db",
        "pic_global_index.db",
        "pic_global_index_bak.db",
        ".prv"
    };
    char video_dir_path[MAX_PATH_STR_NUM] = {0};

    snprintf(video_dir_path, sizeof(video_dir_path), "%s/video", SD_MOUNT_POINT);

    for (i=0; i<sizeof(check_file_list)/sizeof(check_file_list[0])-1; i++)
    {
        if (0 == CheckDirFileTypeExist(SD_MOUNT_POINT, check_file_list[i], 0))
        {
            exist = 0;
            break;
        }
    }

    if (exist && CheckDirFileTypeExist(video_dir_path, check_file_list[i], 0)>0)
    {
        SD_OPS_DEBUG("Old prv record detected! need format sd card!\n");
        if (1 == g_diskinfo.Partitions[0].Partition_has_mounted)
        {
            EDVR_HD_UMountPartition(g_diskinfo.Partitions[0].Mount_point);
        }
        ret = EDVR_HD_FormatPartition(g_diskinfo.Partitions[0].DevName);
        if (0 != ret)
        {
            SD_OPS_ERROR("Format SD Card %s Fail!\n", g_diskinfo.Partitions[0].DevName);
            return ret;
        }

        /* 获取SD卡的设备名(该接口仅仅只能获取SD卡设备名，其他信息需要用下面接口获取) */
        memset(&g_diskinfo, 0, sizeof(g_diskinfo));
        ret = EDVR_HD_GetDiskInfoEx(0, &g_diskinfo);
        if (0 != ret)
        {
            SD_OPS_ERROR("EDVR_HD_GetDiskInfoEx return %d\n", ret);
            return ret;
        }
        goto REMOUNT_DISK;
    }
    else
    {
       SD_OPS_DEBUG("All file is suit for current record type!\n");
    }

    /* 检查SD卡里面是否有多余无用的文件或者文件夹，如果有则删除 */
    ret = DeleteOtherFiles(g_diskinfo.Partitions[0].Mount_point);
    if (0 != ret)
    {
        SD_OPS_ERROR("Fail to Delete Other Files!\n");
    }

    /* 挂载之后，重新获取SD卡的信息，存放在全局结构体里面 */
    memset(&g_diskinfo, 0, sizeof(g_diskinfo));
    ret = EDVR_HD_GetDiskInfoExFromDev(g_sd_dev, &g_diskinfo);
    if (0 != ret)
    {
        SD_OPS_ERROR("EDVR_HD_GetDiskInfoExFromDev return %d\n", ret);
        return ret;
    }

    SDOPS_DebugPrint_DiskDeviceInfoEx (&g_diskinfo, __FILE__,__LINE__,__FUNCTION__);

    return ret;
}

/*@breif SD_GetFileNameInfo 根据文件名获取文件信息
 *@param [IN] p_file_name 文件名
 *@param [OUT] p_name_info 获取到的文件信息
 *@return 返回0  成功
 *@       返回-1 错误
 *@note   文件名的格式: 
 *@       20200630092601-20200630113601_1.jpg
 *@       20200630112601-20200903113601_ALARM.mp4
 *@       20200630132601-20200903113601.prv
 */
int SD_GetFileNameInfo(const char *p_file_name, lpFileNameInfo p_name_info)
{
    if (!p_file_name || !p_name_info)
    {
        SD_OPS_ERROR("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    /* 目录单独处理 */
    if (E_FILE_DIR == p_name_info->eFileType)
    {
        char name_time_str[9] = {0};
        memcpy(name_time_str, p_file_name, sizeof(name_time_str)-1);
        if (strspn(name_time_str, DIGIT_STR) != strlen(name_time_str))
        {
            SD_OPS_DEBUG("dir name %s not date format!\n", name_time_str);
            return -1;
        }
        TIME_STRING_TO_STAMP_DAY(name_time_str, p_name_info->nStartTime);
        return 0;
    }

    /* 图片单独处理 */
    if (E_JPG_FILE == p_name_info->eFileType)
    {
		char name_time_str[15] = {0};
		
    	//定时抓图图片
    	if (strlen(p_file_name) <= TIME_SNAPSHOT_MAX_LEN)
    	{
			memcpy(name_time_str, p_file_name, sizeof(name_time_str)-1);
		}
		else
		{
			memcpy(name_time_str, strchr(p_file_name, '_') + 1, sizeof(name_time_str)-1);
		}

		if (strspn(name_time_str, DIGIT_STR) != strlen(name_time_str))
        {
            SD_OPS_DEBUG("image name %s not date format!\n", name_time_str);
            return -1;
        }
		
		TIME_STRING_TO_STAMP(name_time_str, p_name_info->nStartTime);
        
        return 0;
    }

    /* 初步过滤 */
    if (strlen(p_file_name) < MIN_NAME_STR || NULL == strchr(p_file_name, '-') || NULL == strchr(p_file_name, '.'))
    {
        //SD_OPS_ERROR("File %s not suit sd file format!\n", p_file_name);
        return -1;
    }

    char *p = NULL;
    char start_time_str[15] = {0};
    char stop_time_str[15]   = {0};

	/* 定时录像 */
	if (strlen(p_file_name) == MIN_NAME_STR)
	{
		memcpy(start_time_str, p_file_name, sizeof(start_time_str)-1);
		memcpy(stop_time_str,  p_file_name+sizeof(start_time_str), sizeof(start_time_str)-1);
	}
	else /* 事件录像 */
	{
		memcpy(start_time_str, strchr(p_file_name, '_') + 1, sizeof(start_time_str)-1);
		memcpy(stop_time_str,  strchr(p_file_name, '-') + 1, sizeof(stop_time_str)-1);
	}

    /* 过滤非数字的字符串 */
    if ((strspn(start_time_str, DIGIT_STR) != strlen(start_time_str))
     || (strspn(stop_time_str, DIGIT_STR) != strlen(stop_time_str)))
    {
        SD_OPS_ERROR("%s and %s not date format!\n", start_time_str, stop_time_str);
        return -1;
    }

    TIME_STRING_TO_STAMP(start_time_str, p_name_info->nStartTime);
    TIME_STRING_TO_STAMP(stop_time_str,    p_name_info->nStopTime);

    if (MIN_NAME_STR == strlen(p_file_name))
    {
        p_name_info->eRecType = eNormalRecord;
        if ((p=strchr(p_file_name, '.')))
        {
            p_name_info->eFileType = StrMapEnumType(E_FILE_MAP, p);
        }
        else
        {
            p_name_info->eFileType = E_FILE_ERR;
        }
    }
    else
    {
        if ((p = strchr(p_file_name, '_')))
        {
            char tmp_str[16] = {0};
            memcpy(tmp_str, p_file_name, p - p_file_name);
            p_name_info->eRecType = StrMapEnumType(E_REC_MAP, tmp_str);
        }
        else
        {
            p_name_info->eRecType = eNoRecord;
        }

        p = strchr(p_file_name, '.');
        if (p)
        {
            p_name_info->eFileType = StrMapEnumType(E_FILE_MAP, p);
        }
        else
        {
            p_name_info->eFileType = E_FILE_ERR;
        }
    }

    return 0;
}

/*@breif SD_GetFileListInDir 获取对应文件夹下面的文件信息
 *@param [IN] p_dir 文件夹 
 *@param [IN] p_filter_suffix 需要过滤的文件类型(文件尾)
 *@param [OUT] p_files_list 获取到的文件列表信息(需要手动释放空间)
 *@param [OUT] p_file_count 获取到的文件信息的个数
 *@return 返回0  成功
 *@       返回-1 错误
 *@note   1. p_files_list由于是采用calloc分配连续的空间，所以有以下两点需要注意：
 *@          a.由一维数组模拟二维数组，内存上面是连续的，可以使用qsort等算法排序；
 *@          b.使用完毕之后，需要手动free空间，否则会造成内存泄露；
 *@       2. p_filter_suffix可以为空，如果为空表示不过滤任何类型文件
 */
int SD_GetFileListInDir(const char *p_dir, const char *p_filter_suffix, lpFileInfo *p_files_list, int *p_file_count)
{
    if (NULL==p_dir || NULL==p_files_list || NULL==p_file_count)
    {
        SD_OPS_ERROR("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    *p_file_count = 0;

    int i = 0;
    DIR *dp = NULL;
    char file_path[MAX_FILE_PATH_STR] = {0};
    struct dirent *entry = NULL;
    struct stat statbuff;

    if ((dp = opendir(p_dir)) == NULL)
    {
        SD_OPS_ERROR("Fail to called opendir %s\n", p_dir);
        return -1;
    }

    /* 分配一组连续的内存，以防后续需要用qsort进行排序 */
    p_files_list[0] = (stuFileInfo *)calloc(MAX_FILE_NUM, sizeof(stuFileInfo));
    if (NULL == p_files_list[0])
    {
        SD_OPS_ERROR("Fail to called calloc!\n");
        closedir(dp);
        dp = NULL;
        return -1;
    }

    for (i=1; i<MAX_FILE_NUM; i++) //确保指针连续方可使用qsort
    {
        p_files_list[i] = p_files_list[i-1] + 1;
    }

    i = 0;
    while ((entry = readdir(dp)) != NULL)
    {
		if (i >= MAX_FILE_NUM)
		{
			SD_OPS_ERROR("exceed the max file num, drop it!\n");
			break;
		}

        if((0 == strcmp(entry->d_name, ".")) || (0 == strcmp(entry->d_name, "..")))
        {
            continue;
        }

        /* 当前只判断文件和目录 */
        if (entry->d_type != 4 && entry->d_type != 8)
        {
            continue;
        }

        /* 跳过索引文件 */
        if (0 == strcmp(entry->d_name, ".frames_index"))
        {
            continue;
        }

        /* 针对录像文件，video目录下面都是按照日期排列的文件夹，需要对文件夹进行初步的过滤 */
		#if 0
        if (p_filter_suffix && strcmp(p_filter_suffix, IMAGE_FORMAT_SUFFIX) && (0 == isdigit(entry->d_name[0])))
        {
            continue;
        }
		#endif

        if (p_filter_suffix)
        {
            if (strstr(entry->d_name, p_filter_suffix))
            {
                snprintf(p_files_list[i]->szFileName, MAX_NAME_STR, "%s", entry->d_name);
                if (4 == entry->d_type)
                {
                    p_files_list[i]->nFileSize = 0;
                    p_files_list[i]->struFileNameInfo.eFileType = E_FILE_DIR;
                }
                else
                {
                    memset(&statbuff, 0, sizeof(statbuff));
                    memset(file_path, 0, sizeof(file_path));
                    snprintf(file_path, sizeof(file_path), "%s/%s", p_dir, entry->d_name);
                    stat(file_path, &statbuff);
                    p_files_list[i]->nFileSize = statbuff.st_size;
                    if (0 == strcmp(p_filter_suffix, IMAGE_FORMAT_SUFFIX))
                    {
                        p_files_list[i]->struFileNameInfo.eFileType = E_JPG_FILE;
                    }
                }
                if (0 == SD_GetFileNameInfo(p_files_list[i]->szFileName, &p_files_list[i]->struFileNameInfo))
                {
                    (*p_file_count) ++;
                }
                i ++;
            }
        }
        else
        {
            snprintf(p_files_list[i]->szFileName, MAX_NAME_STR, "%s", entry->d_name);
            if (4 == entry->d_type)
            {
                p_files_list[i]->nFileSize = 0;
                p_files_list[i]->struFileNameInfo.eFileType = E_FILE_DIR;
            }
            else
            {
                memset(&statbuff, 0, sizeof(statbuff));
                memset(file_path, 0, sizeof(file_path));
                snprintf(file_path, sizeof(file_path), "%s/%s", p_dir, entry->d_name);
                stat(file_path, &statbuff);
                p_files_list[i]->nFileSize = statbuff.st_size;
            }

            if (0 == SD_GetFileNameInfo(p_files_list[i]->szFileName, &p_files_list[i]->struFileNameInfo))
            {
                (*p_file_count) ++;
            }
            i ++;
        }
    }

    closedir(dp);
    dp = NULL;

    return 0;
}

static int cmp_time_stamp(const void *p_a, const void *p_b)
{
    if (p_a && p_b)
    {
        const stuFileInfo *p_aa = (const stuFileInfo *)p_a;
        const stuFileInfo *p_bb = (const stuFileInfo *)p_b;
        return (p_aa->struFileNameInfo.nStartTime) - (p_bb->struFileNameInfo.nStartTime);
    }
    return -1;
}

/*@breif SD_GetOldestFile 获取当前文件夹下面最老的文件夹下面最旧的文件
 *@param [IN] p_dir 文件夹 
 *@param [IN] p_filter_suffix 需要过滤的文件类型(文件尾)
 *@param [OUT] p_oldest_file 获取到的最老的文件(绝对路径)
 *@param [IN] max_file_size 最大的p_oldest_file
 *@return 返回0  成功
 *@       返回-1 错误
 *@note   1. p_filter_suffix可以为空，如果为空表示不过滤任何类型文件
 *@       2. 当前接口会先根据文件夹名字找到当前目录下面最旧的一个目录，然后再找到该目录下面最旧的文件
 *@       3. 文件夹的命名方式为: "年月日" 例如:20200907
 */
int SD_GetOldestFile(const char *p_dir, const char *p_filter_suffix, char * p_oldest_file, int max_file_size)
{
    if (NULL==p_dir || NULL==p_oldest_file)
    {
        SD_OPS_ERROR("NULL POINTER EXCEPTION!\n");
        return -1;
    }
    
    int file_count = 0;    
    char dest_dir[MAX_FILE_PATH_STR] = {0};
    lpFileInfo p_file_list[MAX_FILE_NUM] = {NULL};

    /* 寻找最旧的目录(不需要加上过滤类型) */
    if (SD_GetFileListInDir(p_dir, NULL, p_file_list, &file_count))
    {
        SD_OPS_ERROR("Fail to called SD_GetFileListInDir\n");
        return -1;
    }

    if (file_count < 1)
    {
        SD_OPS_ERROR("Dir %s not has suit dir!\n", p_dir);
        SAFE_FREE(p_file_list[0]);
        return -1;
    }

    qsort(*p_file_list, file_count, sizeof(stuFileInfo), cmp_time_stamp);

    snprintf(dest_dir, sizeof(dest_dir), "%s/%s", p_dir, p_file_list[0]->szFileName);

    SAFE_FREE(p_file_list[0]);

    /* 寻找目录里面最旧的文件(需要加上过滤类型) */
    if (SD_GetFileListInDir(dest_dir, p_filter_suffix, p_file_list, &file_count))
    {
        SD_OPS_ERROR("Fail to called SD_GetFileListInDir\n");
        return -1;
    }

   /* 当最旧目录里面没有找到一个符合条件的文件，则删除该目录
    * 注意：如果里面存在其他不符合条件的文件(不含p_filter_suffix字符串的文件)，则表明该文件无文件
    *       系统会自动删除该目录，包括所有的不符合条件的文件，例如这里的tmp文件
    */
    if (file_count < 1)
    {
        SD_OPS_DEBUG("Need to delete empty dir %s\n", dest_dir);
        snprintf(p_oldest_file, max_file_size, "%s", dest_dir);
        SAFE_FREE(p_file_list[0]);
        return 0;
    }

    qsort(*p_file_list, file_count, sizeof(stuFileInfo), cmp_time_stamp);

    snprintf(p_oldest_file, max_file_size, "%s/%s", dest_dir, p_file_list[0]->szFileName);

    // memcpy(p_oldest_file, p_file_list[0], sizeof(stuFileInfo));
    
    SAFE_FREE(p_file_list[0]);

    return 0;
}

/**
 * @brief SDOPS_GetRecordTypePriority 获取某个录像类型的优先级
 * @param [in] 录像类型
 * @return 录像类型的优先级
 * @brief 若类型不合法，则认为是最低优先级
 */
static int SDOPS_GetRecordTypePriority (eRecordType eA)
{
    int nP = 0;
    switch (eA)
    {
        case eNoRecord:
            nP = 0;
            break;
        case eManualRecord:
            nP = 1;
            break;
        case eNormalRecord:
            nP = 2;
            break;
        case eMotionRecord:
            nP = 3;
            break;
        case eAiRecord:
            nP = 4;
            break;
        case eAlarmInRecord:
            nP = 5;
            break;
        case eHttpTrigRecord:
            nP = 6;
            break;
        default:
            SD_OPS_ERROR ("unknown type = %d\n", eA);
            assert (0);
            nP = 0;
            break;
    }
    return nP;
}

/**
 * @brief SDOPS_RecordTypeCompare 比较两个录像类型的优先级，优先级高的为大
 * @param [in] eA 第一个类型
 * @param [in] eB 第二个类型
 * @return eA>eB(+) eA==eB(0) eA<eB(-)
 */
static int SDOPS_RecordTypeCompare (eRecordType eA, eRecordType eB)
{
    return SDOPS_GetRecordTypePriority(eA) - SDOPS_GetRecordTypePriority(eB);
}

/**
 * 设置项
 */
static int SDOPS_RecordTypeInfo_Set (SD_OPS_RecordTypeInfo *info, const SD_OPS_RecordTypeItem *item, int nIndex)
{
    assert (nIndex >= 0 && nIndex < info->uNum);
    if (nIndex < 0 || nIndex >= info->uNum)
    {
        SD_OPS_ERROR ("inner error, nIndex = %d\n", nIndex);
        return -1;
    }

    info->arrayTypeInfo[nIndex] = *item;
    return 0;
}

/**
 * 获取项
 */
static int SDOPS_RecordTypeInfo_Get (const SD_OPS_RecordTypeInfo *info, SD_OPS_RecordTypeItem *item, int nIndex)
{
    assert (nIndex >= 0 && nIndex < info->uNum);
    if (nIndex < 0 || nIndex >= info->uNum)
    {
        SD_OPS_ERROR ("inner error, nIndex = %d\n", nIndex);
        return -1;
    }

    *item = info->arrayTypeInfo[nIndex];
    return 0;
}

/**
 * @brief debug use
 */
static int SDOPS_DebugPrint_RecordTypeInfo (const SD_OPS_RecordTypeInfo *info, 
    const char *file, int line, const char *function)
{
#ifndef SDOPS_SHOW_DETAIL_DEBUGINFO
    return 0;
#endif 

    printf ("[%s %d %s] ", file, line, function);

    struct tm stmBegin = {};
    struct tm stmEnd = {};
    char szBegin[64] = {};
    char szEnd[64] = {};

    time_t tFileBegin = (time_t)info->uFileBeginTime;
    time_t tFileEnd = (time_t)info->uFileEndTime;
    gmtime_r (&tFileBegin, &stmBegin);
    gmtime_r (&tFileEnd, &stmEnd);
    strftime (szBegin, sizeof(szBegin), "%F %T", &stmBegin);
    strftime (szEnd, sizeof(szEnd), "%F %T", &stmEnd);

    printf ("magic=%#x, filebegin=%s, fileend=%s, num=%u\n", 
        info->uMagicNumber, szBegin, szEnd, info->uNum);

    for (unsigned int i = 0; i < info->uNum && i < ARRAY_SIZEOF(info->arrayTypeInfo); ++i)
    {
        time_t tBegin = (time_t)info->arrayTypeInfo[i].uTimeBegin;
        time_t tEnd = (time_t)info->arrayTypeInfo[i].uTimeEnd;
        unsigned int uType = info->arrayTypeInfo[i].uRecordType;
        gmtime_r (&tBegin, &stmBegin);
        gmtime_r (&tEnd, &stmEnd);
        strftime (szBegin, sizeof(szBegin), "%F %T", &stmBegin);
        strftime (szEnd, sizeof(szEnd), "%F %T", &stmEnd);
        printf ("{[%d]: begin=%s,end=%s,type=%u}\n", 
            i, szBegin, szEnd, uType);
    }

    return 0;
}

/**
 * @brief SDOPS_CompleteRecordTypeInfo 完善录像类型信息。
 * @brief 包括：见代码中注释
 * @param [IN OUT] info 录像类型信息
 * @return 成功0 失败-1
 * @note 只对正确的数据进行完善，不修正出错的数据。
 */
static int SDOPS_CompleteRecordTypeInfo (SD_OPS_RecordTypeInfo *info)
{
    assert (info->uNum >= 0 && info->uNum <= ARRAY_SIZEOF(info->arrayTypeInfo));
    if (info->uNum < 0 || info->uNum > ARRAY_SIZEOF(info->arrayTypeInfo))
    {
        SD_OPS_ERROR ("inner error, num = %u\n", info->uNum);
        return -1;
    }

    if (info->uNum == 0)
    {
        return 0;
    }

    /**
     * 修正1 :
     * 修正文件结束时间和最后一项的结束时间不一致的情况（问题来源：见 SDOPS_UpdateRecordTypeInfo）
     */
    if (1)
    {
        SD_OPS_RecordTypeItem sItemLast = {};
        SDOPS_RecordTypeInfo_Get (info, & sItemLast, info->uNum - 1);
        if (sItemLast.uTimeEnd < info->uFileEndTime)
        { /* 将最后一项扩展到文件结束时间 */
            SDOPS_DebugPrint_RecordTypeInfo (info, __FILE__,__LINE__,__FUNCTION__);

            sItemLast.uTimeEnd = info->uFileEndTime;
            SDOPS_RecordTypeInfo_Set (info, & sItemLast, info->uNum - 1);
            
            SDOPS_DebugPrint_RecordTypeInfo (info, __FILE__,__LINE__,__FUNCTION__);
        }
    }

#if 0
/**
 * 修正1.1 如果后端有空隙，则以普通录像填充
 */
if (info->arrayTypeInfo[info->uNum - 1].uTimeEnd < info->uFileEndTime)
{
    if (ARRAY_SIZEOF(info->arrayTypeInfo) == info->uNum)
    { /* 没有多余的空间，则直接将该项扩展 */
        SDOPS_DebugPrint_RecordTypeInfo (info, __FILE__,__LINE__,__FUNCTION__);
        info->arrayTypeInfo[info->uNum - 1].uTimeEnd = info->uFileEndTime;
        SDOPS_DebugPrint_RecordTypeInfo (info, __FILE__,__LINE__,__FUNCTION__);
    }
    else 
    {
        SDOPS_DebugPrint_RecordTypeInfo (info, __FILE__,__LINE__,__FUNCTION__);
        
        SD_OPS_RecordTypeItem sItemAhead = {};
        SDOPS_RecordTypeInfo_Get (info, &sItemAhead, info->uNum - 1);

        SD_OPS_RecordTypeItem item = {};
        item.uTimeBegin = sItemAhead.uTimeEnd;
        item.uTimeEnd = info->uFileEndTime;
        item.uRecordType = eNormalRecord;
        SDOPS_RecordTypeInfo_Add (info, &item);

        SDOPS_DebugPrint_RecordTypeInfo (info, __FILE__,__LINE__,__FUNCTION__);
    }

}

SDOPS_DebugPrint_RecordTypeInfo (info, __FILE__,__LINE__,__FUNCTION__);
for (unsigned int i = 1; i < info->uNum; ++i)
{
    SD_OPS_RecordTypeItem *item = &info->arrayTypeInfo[i];
    SD_OPS_RecordTypeItem *item_ahead = &info->arrayTypeInfo[i - 1];

    /**
     * 修正 1.1：如果两项之间有空隙，则尝试加入普通录像
     */
    if (item->uTimeBegin - item_ahead->uTimeEnd > 0)
    {
        if (SDOPS_RecordTypeInfo_Full(info))
        {
            item->uTimeBegin = item_ahead->uTimeEnd;
        }
        else 
        {
            SD_OPS_RecordTypeItem sItemNew = {};
            sItemNew.uTimeBegin = item_ahead->uTimeEnd;
            sItemNew.uTimeEnd = item->uTimeBegin;
            sItemNew.uRecordType = eNormalRecord;
            SDOPS_RecordTypeInfo_Insert (info, &sItemNew, i);
        }
    }
}
SDOPS_DebugPrint_RecordTypeInfo (info, __FILE__,__LINE__,__FUNCTION__);
#endif 
    return 0;
}

/**
 * @brief SDOPS_SeekFdToRecordTypeInfo 将录像文件的文件描述符定位到类型关联信息的起始位置
 * @param [IN] fd 录像文件的文件描述符
 * @return 成功返回0 失败返回-1
 */
static int SDOPS_SeekFdToRecordTypeInfo (int fd)
{
    if (fd < 0)
    {
        SD_OPS_ERROR("fd invalid\n");
        return -1;
    }

    int ret = lseek (fd, -(PER_AVI_INFO_SEC_SIZE), SEEK_END);
    if (ret < 0)
    {
        SD_OPS_ERROR("lseek failed\n");
        return -1;
    }

    return 0;
}

/**
 * @brief SDOPS_SaveTypeInfoToFile 将录像文件的类型关联信息存到文件中
 * @param [IN] info 关联信息
 * @param [IN] szFileName 文件名
 * @return 成功返回0 失败返回-1
 */
static int SDOPS_SaveTypeInfoToFile (const SD_OPS_RecordTypeInfo *info, const char *szFileName)
{
    SD_OPS_DEBUG("save info to file %s\n", szFileName);
    SDOPS_DebugPrint_RecordTypeInfo (info, __FILE__,__LINE__,__FUNCTION__);

    /* 制作备份 */
    SD_OPS_RecordTypeInfo sInfoTmp = *info;

    /* 对信息进行完善 */
    SDOPS_CompleteRecordTypeInfo (&sInfoTmp);

    int fd = open (szFileName, O_WRONLY);
    if (fd < 0)
    {
        SD_OPS_ERROR ("open file <%s> failed\n", szFileName);
        return -1;
    }

    SDOPS_SeekFdToRecordTypeInfo (fd);
    assert (sizeof(SD_OPS_RecordTypeInfo) <= PER_AVI_TYPE_INFO_SEC_SIZE);

    write (fd, & sInfoTmp, sizeof(sInfoTmp));

    close (fd);
    fd = -1;

    SD_OPS_DEBUG("save finished\n");
    
    return 0;
}

/**
 * @brief SDOPS_InitRecordTypeInfo 初始化录像类型信息，每个文件的第一帧到来时调用
 * @param [IN] pFrameHead 帧头
 * @param [OUT] info 录像类型信息
 * @return 成功返回0 失败返回-1
 */
static int SDOPS_InitRecordTypeInfo (const H264DATAPACKETHEADER *pFrameHead, SD_OPS_RecordTypeInfo *info)
{
    /* debug use */
    SDOPS_DebugPrint_RecordTypeInfo (info, __FILE__,__LINE__,__FUNCTION__);

    /* 取出帧的时间 */
    unsigned int uFrameTime = pFrameHead->u32RealTimeSec;

    info->uMagicNumber = SD_OPS_MAGIC_NUM;
    info->uFileBeginTime = uFrameTime;
    info->uFileEndTime = uFrameTime;

    /* debug use */
    SDOPS_DebugPrint_RecordTypeInfo (info, __FILE__,__LINE__,__FUNCTION__);

    return 0;
}

/****************************************************************************************/

/* ============================================================================
 * SD_OPS 模块设计文档 —— MP4 循环录像改造
 * ----------------------------------------------------------------------------
 * 【背景】
 *   本文件原有 WriteDataToSdCard 使用 AVI 容器（InitAviActionMode +
 *   WriteFrameToAvi + StopAviAction），切片条件包含「定时录像时长 /
 *   事件录像时长 / 录像类型切换 / 跨天对时」等多重规则。
 *   本次改造把底层容器换成 MP4（fMP4 分片格式），切片逻辑简化为
 *   单一条件：累计录像时长 >= 可配置阈值 g_mp4_segment_duration_sec。
 *
 * 【核心设计】
 *   1) 复用 modules/mp4 的 MP4Writer 系列接口（Create/WriteFrame/
 *      Flush/Close/Destroy），不重新造 MP4 封装轮子，也不依赖 ffmpeg；
 *   2) MP4Writer 内部使用 fMP4（ftyp + moov + 循环(moof+mdat) + mfra）
 *      布局：moov 在首帧前写入并 fsync，每个分片的 moof 索引与数据
 *      一起落盘，掉电后已写入分片可独立播放（参考 untrunc 原理）；
 *   3) 小批量帧缓存 + 周期性 fsync 由 MP4WriterConfig 控制：
 *      max_samples=30 / max_bytes=512KB / max_duration_ms=2000 /
 *      sync_interval_ms=2000，适配 SD 卡写入特性；
 *   4) 文件路径：<挂载点>/video/YYYYMMDD/rec_YYYYMMDD_HHMMSS.mp4；
 *   5) 只保留「时长达到阈值」这一种切分片条件，录像类型切换、跨天
 *      等规则暂不参与切分片（由上层业务自行按需调用 need_update_file
 *      强制切文件）；
 *   6) 时间戳归一化：以分片首帧的绝对时间戳(微秒)为 DTS 零点，
 *      后续帧 DTS/PTS = 绝对时间 - 首帧绝对时间，保证 MP4 时间轴单调。
 *
 * 【全局变量入口】
 *   - g_mp4_segment_duration_sec : 切片时长阈值，SDOPS_SetMp4SegmentDuration()
 *   - g_use_mp4_record            : 容器开关，1=MP4 / 0=AVI(回退)
 *
 * 【异常保护】
 *   - NULL 入参 / SD 卡未初始化 / 格式化中 / 不可写：直接返回 -1 或 0，
 *     不做任何 MP4 操作；
 *   - MP4Writer_WriteFrame 返回 -1：立刻 Close+Destroy 当前句柄，
 *     重置内部状态，返回 -1；上层收到错误后停止录像即可；
 *   - CreateDirent / MP4Writer_Create 失败：返回 -1，句柄保持 NULL；
 *   - 句柄释放：切分片/错误/退出时统一 Close+Destroy，并置 NULL，
 *     避免悬挂指针与内存泄漏。
 *
 * 【与旧接口的兼容】
 *   - avi_file_write_over / out_avi_file / rec_start_time 的语义保持：
 *     切出新文件时 avi_file_write_over=1、out_avi_file=basename；
 *   - need_update_file 保持为可强制切分片的外部输入；
 *   - 返回值：成功=0 / 切出新文件=1 / 失败=-1，与旧实现一致。
 * ============================================================================
 */

/* 本文件原有 WriteDataToSdCard 的 AVI 实现已整体替换为下面的 MP4 循环录像版本。 */
/*@breif WriteDataToSdCard MP4 循环录像帧写入
 *@param [IN]  frame_size 一帧数据的大小
 *@param [IN]  frame_buffer 帧数据(H.264 Annex-B 裸流)
 *@param [IN]  frame_head 帧头信息
 *@param [IN]  need_update_file 外部请求强制切文件(可由上层置 1)
 *@param [OUT] avi_file_write_over 当前分片写完毕切出新文件时置 1
 *@param [OUT] out_avi_file 切出的新文件名(avi_file_write_over=1 时有效)
 *@param [OUT] rec_start_time 新分片起始绝对时间(秒)
 *@return 0=成功 / 1=切出新文件 / -1=失败
 *@note  底层 MP4 封装复用 MP4Writer，不做二次编码，适配 SD 卡小批量缓存 + 周期 fsync。
 */
extern int WriteDataToSdCard(int frame_size,                            /* 一帧数据的大小 */
                                      unsigned char * frame_buffer,        /* 纯264数据buffer */
                                      unsigned char *frame_head,           /* 帧头buffer */
                                      unsigned int *need_update_file,      /* 是否需要马上更新文件 */
                                      unsigned int *avi_file_write_over,   /* 当前文件是否写完毕 */
                                      char *out_avi_file,                  /* 新生成的文件名 */
                                      unsigned int *rec_start_time)         /* 新分片录像的开始时间 */
{
    int ret = 0;
    int y = 0, mon = 0, d = 0, h = 0, min = 0, s = 0;
    int64_t      cur_abs_us = 0;       /* 当前帧绝对时间(微秒) */
    int64_t      cur_dts_us = 0;       /* 当前帧相对分片首帧的 DTS(微秒) */
    int64_t      cur_pts_us = 0;       /* 当前帧相对分片首帧的 PTS(微秒) */
    int          is_key = 0;
    unsigned int seg_duration_sec = 0;
    char         record_dir[16] = {0};
    char         base_file_name[MAX_NAME_STR] = {0};
    char         full_path[MAX_PATH_STR_NUM] = {0};
    MP4WriterInfo info;
    MP4WriterConfig cfg;

    if (!frame_buffer || !frame_head || !out_avi_file)
    {
        SD_OPS_ERROR("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    if (0 == g_sd_card_has_inited)
    {
        return -1;
    }

    if (Formatting == g_Sd_format_status)
    {
        return 0;
    }

    if (SdCardReadWrite != g_sd_check_status)
    {
        return -1;
    }

    /* 与旧 AVI 实现的回退开关：0 时返回 0 保持上层空跑 */
    if (!g_use_mp4_record)
    {
        return 0;
    }

    pthread_mutex_lock(&global_disk_write_mutex);

    H264DATAPACKETHEADER *pRawHead = (H264DATAPACKETHEADER *)frame_head;

    g_sd_write_status = SdCardIsWriting;

    cur_abs_us = (int64_t)pRawHead->u32RealTimeSec * 1000000LL
               + (int64_t)pRawHead->u32RealTimeUSec;
    is_key = (I_FRAME == pRawHead->byFrameType) ? 1 : 0;

    /* ===== 1. 初始化 MP4 时间基线 ===== */
    if (!g_mp4_have_first_frame)
    {
        /* 首帧即作为当前分片的时间零点；后续帧 DTS/PTS 均以此为基准归一化 */
        g_mp4_seg_begin_frame_abs_us = cur_abs_us;
        g_mp4_seg_begin_real_sec     = pRawHead->u32RealTimeSec;
        g_mp4_seg_begin_real_usec    = pRawHead->u32RealTimeUSec;
        g_mp4_have_first_frame = 1;
    }

    cur_dts_us = cur_abs_us - g_mp4_seg_begin_frame_abs_us;
    cur_pts_us = cur_dts_us;  /* 简单场景：PTS = DTS；若 B 帧存在可通过 ctts 处理 */
    if (cur_dts_us < 0)
    {
        /* 时间跳变(对时)兜底：钳为 0 并重置基线，避免 MP4 时间轴倒回 */
        g_mp4_seg_begin_frame_abs_us = cur_abs_us;
        g_mp4_seg_begin_real_sec     = pRawHead->u32RealTimeSec;
        g_mp4_seg_begin_real_usec    = pRawHead->u32RealTimeUSec;
        cur_dts_us = 0;
        cur_pts_us = 0;
    }

    seg_duration_sec = (unsigned int)(cur_abs_us - g_mp4_seg_begin_frame_abs_us) / 1000000LL;

    /* ===== 2. 切分片判断 =====
     * 触发切分片的条件：
     *   a) 上层请求强制切文件（need_update_file=1），且当前分片已写入至少 1 帧
     *   b) 累计时长 >= 配置阈值 g_mp4_segment_duration_sec
     *
     * 注意：在 SD 卡路径的挂载点/目录已变化（如跨天/重启）时，也会在后续
     *       创建新 Writer 时根据当前挂载点重建目录，保证路径正确。
     */
    int need_new_seg = 0;
    if (g_mp4_writer != NULL)
    {
        if ((need_update_file && *need_update_file == 1) ||
            (seg_duration_sec >= g_mp4_segment_duration_sec))
        {
            need_new_seg = 1;
        }
    }

    if (need_new_seg && g_mp4_writer != NULL)
    {
        /* 关闭当前分片：Close 会触发 Flush + 写 mfra + fsync */
        ret = MP4Writer_Close(g_mp4_writer);
        if (ret != 0)
        {
            SD_OPS_ERROR("MP4Writer_Close failed: %s\n", MP4Writer_GetLastError(g_mp4_writer));
        }
        MP4Writer_Destroy(g_mp4_writer);
        g_mp4_writer = NULL;

        /* 通知上层：当前分片已写完，返回 basename 到 out_avi_file */
        if (g_mp4_curr_file_name[0] != '\0')
        {
            const char *base = strrchr(g_mp4_curr_file_name, '/');
            base = (base != NULL) ? (base + 1) : g_mp4_curr_file_name;
            snprintf(out_avi_file, MAX_NAME_STR, "%s", base);
            *avi_file_write_over = 1;
        }

        /* 重置分片计数器，下一帧会创建新 Writer */
        g_mp4_have_first_frame = 0;
        *need_update_file = 0;

        SD_OPS_INFO("MP4 segment closed, duration=%us, file=%s\n",
                    seg_duration_sec, g_mp4_curr_file_name);
    }

    /* ===== 3. 创建新分片 Writer ===== */
    if (g_mp4_writer == NULL)
    {
        ULONG_TO_TIME((pRawHead->u32RealTimeSec), y, mon, d, h, min, s);

        /* 目录结构：<挂载点>/video/YYYYMMDD/ */
        snprintf(record_dir, sizeof(record_dir), "%04d%02d%02d", y, mon, d);
        snprintf(full_path, sizeof(full_path), "%s/video/%s",
                 g_diskinfo.Partitions[0].Mount_point, record_dir);
        CreateDirent(full_path);

        /* 文件名：rec_YYYYMMDD_HHMMSS.mp4 */
        snprintf(base_file_name, sizeof(base_file_name),
                 "rec_%04d%02d%02d_%02d%02d%02d.mp4", y, mon, d, h, min, s);
        snprintf(g_mp4_curr_file_name, sizeof(g_mp4_curr_file_name),
                 "%s/%s", full_path, base_file_name);

        /* Writer 初始化参数：宽高传 0，由 MP4Writer 内部从 SPS 自动解析 */
        memset(&info, 0, sizeof(info));
        info.width  = 0;
        info.height = 0;
        info.fps    = 25;
        info.start_time = pRawHead->u32RealTimeSec;

        /* 切片/刷盘配置：嵌入式 SD 卡友好 */
        memset(&cfg, 0, sizeof(cfg));
        cfg.max_samples      = 30;
        cfg.max_bytes        = 512 * 1024;
        cfg.max_duration_ms  = 2000;
        cfg.sync_interval_ms = 2000;

        g_mp4_writer = MP4Writer_Create(g_mp4_curr_file_name, &info, &cfg);
        if (g_mp4_writer == NULL)
        {
            /* Create 失败时句柄为 NULL，无法调用 GetLastError，直接输出文件名 */
            SD_OPS_ERROR("MP4Writer_Create failed for: %s\n", g_mp4_curr_file_name);
            g_sd_write_status = SdCardWriteFail;
            pthread_mutex_unlock(&global_disk_write_mutex);
            return -1;
        }

        /* 新分片起始时间 */
        g_mp4_seg_begin_frame_abs_us = cur_abs_us;
        g_mp4_seg_begin_real_sec     = pRawHead->u32RealTimeSec;
        g_mp4_seg_begin_real_usec    = pRawHead->u32RealTimeUSec;
        g_mp4_have_first_frame = 1;

        if (rec_start_time)
        {
            *rec_start_time = g_mp4_seg_begin_real_sec;
        }

        SD_OPS_INFO("MP4 segment opened: %s\n", g_mp4_curr_file_name);

        /* 旧 AVI 辅助函数/变量：保留以备后续回退复用，这里显式引用避免 -Wunused-function/-Wunused-variable */
        (void)SDOPS_InitRecordTypeInfo;
        (void)SDOPS_SaveTypeInfoToFile;
        (void)g_ops_file_path;
        (void)g_curr_record_info;
        (void)g_last_record_type;
    }

    /* ===== 4. 写入当前帧 =====
     * keyframe 传 -1 让 Writer 自动识别 IDR，也可以由 byFrameType 直接给出 0/1。
     * 这里显式传递 is_key 以便避免内部解析额外开销。
     */
    ret = MP4Writer_WriteFrame(g_mp4_writer,
                               frame_buffer, frame_size,
                               cur_pts_us, cur_dts_us,
                               is_key);
    if (ret != 0)
    {
        SD_OPS_ERROR("MP4Writer_WriteFrame failed: %s (size=%d)\n",
                     MP4Writer_GetLastError(g_mp4_writer), frame_size);
        /* 出错时 Close+Destroy，释放资源并防止悬挂指针 */
        MP4Writer_Close(g_mp4_writer);
        MP4Writer_Destroy(g_mp4_writer);
        g_mp4_writer = NULL;
        g_mp4_have_first_frame = 0;
        g_sd_write_status = SdCardWriteFail;
        pthread_mutex_unlock(&global_disk_write_mutex);
        return -1;
    }

    /* ===== 5. 更新全局状态 ===== */
    g_second_times = seg_duration_sec;  /* 与旧接口保持同名，供外部查询当前分片时长 */
    g_sd_write_status = SdCardIsWriting;

    pthread_mutex_unlock(&global_disk_write_mutex);
    return (need_new_seg) ? 1 : 0;
}

static int BinareySearchEx(unsigned int *data, int data_count, unsigned int key)
{
    if (NULL == data)
    {
        SD_OPS_ERROR("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    int i = 0;
    int j = data_count - 1;
    int mid = 0;

    while(i < j)
    {
        mid = i + ((j - i + 1) >> 1);
        if (key <= data[mid])
        {
            j = mid - 1;
        }
        else
        {
            i = mid;
        }
    }
    if (data[i] < key)
    {
        return i;
    }
    else
    {
        return -1;
    }              
}

/*@breif BinareySearchEx 二分法查找递增表中最接近最大的数(可以相等)
 *@param [IN] data 欲查找的递增数组表
 *@param [IN] data_count 该数组的元素个数
 *@param [IN] key 欲查找的数
 *@return 成功返回数组下标
 *@      失败返回-1
 *@note  支持重复数据, 返回最近最大的数的下标
 */
static int BinarySearchNearMax(unsigned int *data, int data_count, unsigned int key)
{
    if (NULL == data)
    {
        SD_OPS_ERROR("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    int i = 0;
    int j = data_count - 1;
    int mid = 0;

#if 1
    /* 考虑回退的情况 */
    if (key >= data[j])
    {
        return j;
    }
#endif

    while(i < j)
    {
        mid = i + ((j - i) >> 1);
        if (key <= data[mid])
        {
            j = mid;
        }
        else
        {
            i = mid + 1;
        }
    }

    if (data[i] >= key)
    {
        return i; 
    }
    else
    {
        return -1;    
    }
}

/*@breif BinareySearchEx 二分法查找递增表中最接近最小的数(可以相等)
 *@param [IN] data 欲查找的递增数组表
 *@param [IN] data_count 该数组的元素个数
 *@param [IN] key 欲查找的数
 *@return 成功返回数组下标
 *@      失败返回-1
 *@note  支持重复数据, 返回最近最小的数的下标
 */
static int BinarySearchNearMin(unsigned int *data, int data_count, unsigned int key)
{
    if (NULL == data)
    {
        SD_OPS_ERROR("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    int i = 0;
    int j = data_count - 1;
    int mid = 0;

#if 1
    /* 考虑回退的情况 */
    if (key <= data[i])
    {
        return i;
    }
#endif

    while(i < j)
    {
        mid = i + ((j - i + 1) >> 1);
        if (key <= data[mid])
        {
            j = mid - 1;
        }
        else
        {
            i = mid;
        }
    }

    if(data[i] < key)
    {
        return i;
    }
    else
    {
        return -1; 
    }
}

/*@breif GetIFrameListFromAviFile 从avi文件里面获取I帧信息表放在句柄里面
 *@param [IN]  avi_file 欲操作的AVI文件
 *@param [OUT] i_frame_handle 从avi_file里面获取到的I帧信息句柄
 *@param [OUT] effect_count 句柄里面有效的结构体个数
 *@return 成功返回0
 *@      失败返回-1
 */
int GetIFrameListFromAviFile(const char *avi_file, IFrameInfo *i_frame_handle, int *effect_count)
{
    if (!avi_file || !i_frame_handle || !effect_count)
    {
        SD_OPS_ERROR("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    IFrameInfo iframe_list[MAX_I_FRAME_NUM];
    memset(iframe_list, 0, sizeof(iframe_list));

    pthread_mutex_lock(&global_file_mutex);
    int fd = open(avi_file, O_RDONLY, S_IRWXU);
    if (-1 == fd)
    {
        SD_OPS_ERROR("open %s fail!\n", avi_file);        
        pthread_mutex_unlock(&global_file_mutex);
        return -1;
    }
    
    /* 直接定位指针到I帧信息的开始处 */
    lseek(fd, -(PER_AVI_FRAME_INFO_SEC_SIZE), SEEK_END);

    /* 直接读取16KB的INFO信息大小 */
    read(fd, &iframe_list[0], sizeof(iframe_list));
    SAFE_CLOSE(fd);

    pthread_mutex_unlock(&global_file_mutex);

    qsort(iframe_list, MAX_I_FRAME_NUM, sizeof(IFrameInfo), com_avi_i_frame_time);

    int i = 0;
    int j = 0;

    /* 除去结构体里面时间戳为0的情况 */
    for (i=0; i<MAX_I_FRAME_NUM; i++)
    {
        if (iframe_list[i].CurIFrameTime > 0)
        {
            memcpy(&i_frame_handle[j++], &iframe_list[i], sizeof(IFrameInfo));
        }
    }

    *effect_count = j;

    return 0;    
}

/*@breif SetCurrentAviNumPerPage 设置每页显示多少个AVI文件
 *@param [IN]  avi_num_per_page 每页显示的AVI文件个数
 *@return 成功返回0
 *@      失败返回-1
 *@note  每页显示的AVI文件个数不能超过当前有效的AVI个数g_read_curr_avi_num，
 *@      每页显示的AVI文件个数不能超过最大能显示的AVI个数MAX_AVI_NUM_ONE_PAGE
 */
extern int SetCurrentAviNumPerPage(int avi_num_per_page)
{
    g_one_page_show_avi_count = avi_num_per_page > MAX_AVI_NUM_ONE_PAGE ? MAX_AVI_NUM_ONE_PAGE : avi_num_per_page;
    SD_OPS_DEBUG("avi_num_per_page = %d, g_one_page_show_avi_count = %d\n", avi_num_per_page, g_one_page_show_avi_count);

    return 0;
}

/*@breif GetCurrentAviTotalNum 获取当前AVI总个数
 *@param [OUT] p_current_avi_count 
 *@return 成功返回当前总的AVI文件个数
 *@       失败返回-1
 */
extern int GetCurrentAviTotalNum(void)
{
    return g_nReadTotalRecFileCount;
}

/*@breif SetSwitchPageIsChoose 设置当前选中的是哪一个页面和当前录像路径类型
 *@param [IN]  choose_page 选中的页面(从1开始)
 *@param [IN]  path_type 当前录像路径类型 0: http路径  1:绝对路径
 *@return 成功返回0
 *@      失败返回-1
 */
extern int SetSwitchPageIsChoose(int choose_page, int path_type)
{
    if (choose_page==g_nChoosePage && path_type==g_nPageType)
    {
        return 0;
    }

    g_nChoosePage = choose_page;
    g_nPageType = path_type;

    return 0;
}

/*@breif UpdateAviListByTime 通过给定的时间戳更新AVI文件列表
 *@param [IN]  start_time 要搜索的起始时间
 *@param [IN]  stop_time 要搜索的结束时间(为0表示搜索全部文件)
 *@param [OUT] p_search_avi_count 搜索到符合条件的AVI文件总个数
 *@return 成功返回0
 *@      失败返回-1
 */
extern int UpdateAviListByTime(unsigned int start_time, unsigned int stop_time, int *p_search_avi_count)
{
    if (NULL == p_search_avi_count)
    {
        SD_OPS_ERROR("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    if (start_time >= stop_time)
    {
        if (stop_time > 0)
        {
            SD_OPS_ERROR("start_time %d >= stop_time %d\n", start_time, stop_time);
            return -1;
        }
    }

    int i = 0;
    int j = 0;
    int day_count = 0;
    int file_count = 0;
    int total_file_count = 0;
    unsigned int start_time_day = 0;
    unsigned int stop_time_day = 0;
    char dest_dir[MAX_FILE_PATH_STR] = {0};
    char day_dir_str[9] = {0};
    time_t day_stamp = 0;
    pAvi_read_list p_tmp = NULL;
    lpFileInfo p_file_list[MAX_FILE_NUM] = {NULL};
    stuDateTime date_time = {0,0,0,0,0,0};

    start_time_day = start_time / DAY_TIME_STAMP_SEC;
    stop_time_day  = stop_time   / DAY_TIME_STAMP_SEC;
    day_count = (stop_time_day - start_time_day) + 1;

    /* 释放之前的内存(如果有) */
    SAFE_FREE(g_pAviReadListSet);
    g_nReadTotalRecFileCount = 0;

    for (i=0; i< day_count; i++)
    {
        day_stamp = (start_time_day + i) * DAY_TIME_STAMP_SEC;
        TIME_STAMP_TO_STRING_DAY(day_stamp, day_dir_str);
        snprintf(dest_dir, sizeof(dest_dir), "%s/video/%s", SD_MOUNT_POINT, day_dir_str);
        if (0 == access(dest_dir, F_OK))
        {
            /* 寻找指定目录下面的文件列表 */
            if (SD_GetFileListInDir(dest_dir, VIDEO_FORMAT_SUFFIX, p_file_list, &file_count))
            {
                SD_OPS_ERROR("Fail to called SD_GetFileListInDir\n");
                continue;
            }
            if (file_count < 1)
            {
                SD_OPS_ERROR("Dir %s not has suit dir!\n", dest_dir);
                SAFE_FREE(p_file_list[0]);
                continue;
            }
            total_file_count = g_nReadTotalRecFileCount;
            g_nReadTotalRecFileCount += file_count;

            p_tmp = NULL;  // 注意避免产生realloc的副作用
            p_tmp = (pAvi_read_list)realloc(g_pAviReadListSet, g_nReadTotalRecFileCount*sizeof(Avi_read_list));
            if (NULL == p_tmp)
            {            
                SD_OPS_ERROR("Fail to realloc g_pAviReadListSet\n");
                g_nReadTotalRecFileCount -= file_count;
                SAFE_FREE(p_file_list[0]);
                return -1;
            }
            g_pAviReadListSet = p_tmp;

            for (j=0; j<file_count; j++)
            {
                if (p_file_list[j]->struFileNameInfo.nStartTime < start_time
                 || p_file_list[j]->struFileNameInfo.nStopTime > stop_time)
                {
                    g_nReadTotalRecFileCount -- ;
                    total_file_count --;
                    continue;
                }
                snprintf(g_pAviReadListSet[total_file_count+j].file_name, MAX_NAME_STR, "%s", 
                          p_file_list[j]->szFileName);
                ULONG_TO_TIME((p_file_list[j]->struFileNameInfo.nStartTime), \
                              (date_time.nYear), (date_time.nMonth), (date_time.nDay), \
                              (date_time.nHour), (date_time.nMinute), (date_time.nSecond));
                g_pAviReadListSet[total_file_count+j].start_time = date_time;

                /* 当前录像路径类型 0: http路径  1:绝对路径 */
          
                snprintf(g_pAviReadListSet[total_file_count+j].file_path, MAX_NAME_STR, "/%04d%02d%02d", 
                      date_time.nYear, date_time.nMonth, date_time.nDay);
           
                ULONG_TO_TIME((p_file_list[j]->struFileNameInfo.nStopTime), \
                              (date_time.nYear), (date_time.nMonth), (date_time.nDay), \
                              (date_time.nHour), (date_time.nMinute), (date_time.nSecond));
                g_pAviReadListSet[total_file_count+j].stop_time = date_time;
                g_pAviReadListSet[total_file_count+j].file_size = p_file_list[j]->nFileSize;
            }
            SAFE_FREE(p_file_list[0]);
        }
    }

    *p_search_avi_count = g_nReadTotalRecFileCount;

    return 0;
}

/*@breif GetAviListInfoByTimeStamp 通过给定的时间戳获取对应的AVI文件列表
 *@param [IN]  read_avi_page 需要获取第几页的AVI信息(从1开始)
 *@param [OUT] p_avi_list 获取到的AVI文件列表
 *@param [OUT] p_avi_num_curr_page 当前页面有多少个AVI文件
 *@return 成功返回0
 *@      失败返回-1
 *@note  需要先调用SetCurrentAviNumPerPage设置当前用户需要设置每页显示多少个AVI文件
 *@note  然后调用UpdateAviListByTime
 */
extern int GetAviListInfoByTimeStamp(             int read_avi_page, void **p_avi_list, int *p_avi_num_curr_page)
{
    if ((NULL==p_avi_list) || (NULL==p_avi_num_curr_page))
    {
        SD_OPS_ERROR("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    int i = 0, j = 0;
    int begin_index = 0, end_index = 0;

    /* 采用时间顺序:    新->旧 排列 */
    end_index = (g_nReadTotalRecFileCount - 1) - (g_one_page_show_avi_count * read_avi_page);
    begin_index = end_index - g_one_page_show_avi_count + 1;
    begin_index = begin_index < 0 ? 0 : begin_index;

    for (i=end_index, j=0; i>=begin_index; i--, j++)
    {
    	
        memcpy(&g_read_avi_list[j], &g_pAviReadListSet[i], sizeof(Avi_read_list));

		if(g_nPageType)
		{
			snprintf(g_read_avi_list[j].file_path, sizeof(g_read_avi_list[j].file_path), "%s/video%s", \
				SD_MOUNT_POINT, g_pAviReadListSet[i].file_path);
		}
		else
		{
			snprintf(g_read_avi_list[j].file_path, sizeof(g_read_avi_list[j].file_path), "%s%s", \
			HTTP_DOWNLOAD_RECORD_PATH, g_pAviReadListSet[i].file_path);
		}
#ifdef GET_LIST_DBG
        SD_OPS_DEBUG("[%02d] %s\n", j, g_read_avi_list[j].file_name);
#endif
    }

    *p_avi_list = (void*)g_read_avi_list;
    *p_avi_num_curr_page = j;

    return 0;
}

/*@breif SetCurrentPicNumPerPage 设置每页显示多少个图片文件
 *@param [IN]  pic_num_per_page 每页显示的图片文件个数
 *@return 成功返回0
 *@      失败返回-1
 *@note  每页显示的图片文件个数不能超过当前有效的图片个数g_read_curr_pic_num，
 *@      每页显示的图片文件个数不能超过最大能显示的图片个数MAX_PIC_NUM_ONE_PAGE
 */
extern int SetCurrentPicNumPerPage(int pic_num_per_page)
{
    g_one_page_show_pic_count = pic_num_per_page > MAX_PIC_NUM_ONE_PAGE ? MAX_PIC_NUM_ONE_PAGE : pic_num_per_page;
    SD_OPS_DEBUG("pic_num_per_page = %d, g_one_page_show_pic_count = %d\n", pic_num_per_page, g_one_page_show_pic_count);

    return 0;
}

/*@breif GetCurrentPicTotalNum 获取当前图片总个数
 *@return 成功返回当前总的图片文件个数
 *@       失败返回-1
 */
extern int GetCurrentPicTotalNum(void)
{
    return g_nReadTotalPicFileCount;
}

/*@breif UpdatePicListByTime 通过给定的时间戳更新PIC文件列表
 *@param [IN]  start_time 要搜索的起始时间
 *@param [IN]  stop_time 要搜索的结束时间(为0表示搜索全部文件)
 *@param [OUT] p_search_pic_count 搜索到符合条件的PIC文件总个数
 *@return 成功返回0
 *@      失败返回-1
 */
extern int UpdatePicListByTime(unsigned int start_time, 
                                     unsigned int stop_time, 
                                     int *p_search_pic_count)
{
    if (NULL == p_search_pic_count)
    {
        SD_OPS_ERROR("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    if (start_time >= stop_time)
    {
        if (stop_time > 0)
        {
            SD_OPS_ERROR("start_time %d >= stop_time %d\n", start_time, stop_time);
            return -1;
        }
    }

    int i = 0;
    int j = 0;
    int k = 0;
    int day_count = 0;
    int file_count = 0;
    int total_file_count = 0;
    unsigned int start_time_day = 0;
    unsigned int stop_time_day = 0;
    char dest_dir[MAX_FILE_PATH_STR] = {0};
    char day_dir_str[9] = {0};
    time_t day_stamp = 0;
    pPic_read_list p_tmp = NULL;
    lpFileInfo p_file_list[MAX_FILE_NUM] = {NULL};
    stuDateTime date_time = {0,0,0,0,0,0};

    start_time_day = start_time / DAY_TIME_STAMP_SEC;
    stop_time_day  = stop_time   / DAY_TIME_STAMP_SEC;
    day_count = (stop_time_day - start_time_day) + 1;

    /* 释放之前的内存(如果有) */
    SAFE_FREE(g_pPicReadListSet);
    g_nReadTotalPicFileCount = 0;

    for (i=0; i< day_count; i++)
    {
        day_stamp = (start_time_day + i) * DAY_TIME_STAMP_SEC;
        TIME_STAMP_TO_STRING_DAY(day_stamp, day_dir_str);

        for (k=0; k<sizeof(s_gRecType)/sizeof(s_gRecType[0]); k++)
        {
            snprintf(dest_dir, sizeof(dest_dir), "%s/picture/%s", 
                     SD_MOUNT_POINT,    day_dir_str);
            if (0 == access(dest_dir, F_OK))
            {
                /* 寻找指定目录下面的文件列表 */
                if (SD_GetFileListInDir(dest_dir, IMAGE_FORMAT_SUFFIX, p_file_list, &file_count))
                {
                    SD_OPS_ERROR("Fail to called SD_GetFileListInDir\n");
                    continue;
                }
                if (file_count < 1)
                {
                    SD_OPS_ERROR("Dir %s not has suit dir!\n", dest_dir);
                    SAFE_FREE(p_file_list[0]);
                    continue;
                }
                total_file_count = g_nReadTotalPicFileCount;
                g_nReadTotalPicFileCount += file_count;

                p_tmp = NULL;  // 注意避免产生realloc的副作用
                p_tmp = (pPic_read_list)realloc(g_pPicReadListSet, g_nReadTotalPicFileCount*sizeof(Pic_read_list));
                if (NULL == p_tmp)
                {
                    SD_OPS_ERROR("Fail to realloc pPic_read_list\n");
                    g_nReadTotalPicFileCount -= file_count;
                    SAFE_FREE(p_file_list[0]);
                    return -1;
                }
                g_pPicReadListSet = p_tmp;

                for (j=0; j<file_count; j++)
                {
                    if (p_file_list[j]->struFileNameInfo.nStartTime < start_time)
                    {
                        g_nReadTotalPicFileCount -- ;
                        total_file_count --;
                        continue;
                    }
                    snprintf(g_pPicReadListSet[total_file_count+j].file_name, MAX_NAME_STR, "%s", 
                              p_file_list[j]->szFileName);
                    ULONG_TO_TIME((p_file_list[j]->struFileNameInfo.nStartTime), \
                                  (date_time.nYear), (date_time.nMonth), (date_time.nDay), \
                                  (date_time.nHour), (date_time.nMinute), (date_time.nSecond));
                    g_pPicReadListSet[total_file_count+j].cap_time = date_time;
                    g_pPicReadListSet[total_file_count+j].file_size = p_file_list[j]->nFileSize;

                    /* 当前录像路径类型 0: http路径  1:绝对路径 */
                   
                    snprintf(g_pPicReadListSet[total_file_count+j].file_path, MAX_NAME_STR, "/%04d%02d%02d", 
                             date_time.nYear, date_time.nMonth, date_time.nDay);

                }
                SAFE_FREE(p_file_list[0]);
            }
        }
    }

    *p_search_pic_count = g_nReadTotalPicFileCount;

    return 0;
}

/*@breif GetPictureListInfoByTimeStamp 通过给定的时间戳获取对应的图片文件列表
 *@param [IN]  read_pic_page 需要获取第几页的图片信息(从1开始)
 *@param [OUT] p_pic_list 获取到的图片文件列表
 *@param [OUT] p_pic_num_curr_page 当前页面有多少个图片文件 
 *@return 成功返回0
 *@      失败返回-1
 *@note  需要先调用GetPicGlobalHeadInfoFromGlobalFile将需要的数据放到全局结构体里面
 *@      需要先调用SetCurrentPicNumPerPage设置当前用户需要设置每页显示多少个图片文件
 */
extern int GetPictureListInfoByTimeStamp(int read_pic_page, void **p_pic_list, int *p_pic_num_curr_page)
{
    if ((NULL==p_pic_list) || (NULL==p_pic_num_curr_page))
    {
        SD_OPS_ERROR("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    int i = 0, j = 0;
    int begin_index = 0, end_index = 0;

    /* 采用时间顺序:    新->旧 排列 */
    end_index = (g_nReadTotalPicFileCount - 1) - (g_one_page_show_pic_count * read_pic_page);
    begin_index = end_index - g_one_page_show_pic_count + 1;
    begin_index = begin_index < 0 ? 0 : begin_index;

    for (i=end_index, j=0; i>=begin_index; i--, j++)
    {
        memcpy(&g_read_pic_list[j], &g_pPicReadListSet[i], sizeof(Pic_read_list));

		if(g_nPageType)
		{
			snprintf(g_read_pic_list[j].file_path, sizeof(g_read_pic_list[j].file_path), "%s/picture%s", \
				SD_MOUNT_POINT, g_pPicReadListSet[i].file_path);
		}
		else
		{
			snprintf(g_read_pic_list[j].file_path, sizeof(g_read_pic_list[j].file_path), "%s%s", \
				HTTP_DOWNLOAD_PICTURE_PATH, g_pPicReadListSet[i].file_path);
		}
		
#if 0
        SD_OPS_DEBUG("[%02d] %s\n", j, g_read_avi_list[j].file_name);
#endif
    }

    *p_pic_list = (void*)g_read_pic_list;
    *p_pic_num_curr_page = j;

    return 0;

}

/*@breif GetRecordFileSeekTimeFrameInfo 从给定的时间里面获取符合该时间最近的I帧信息(对seek时间进行I帧修正)
 *@param [IN]  record_file    录像文件
 *@param [IN]  seek_time      seek时间
 *@param [OUT] p_iframe_info  获取到的I帧信息
 *@param [OUT] p_iframe_info_pre  获取到的前一个I帧信息
 *@return 成功返回0
 *@       失败返回-1
 */
static int GetRecordFileSeekTimeFrameInfo(const char *record_file, 
                                                          unsigned int seek_time, 
                                                          int b_need_correct_time,
                                                          IFrameInfo *p_iframe_info,
                                                          IFrameInfo *p_iframe_info_pre
                                                          )
{
    if (!record_file || !p_iframe_info)
    {
        SD_OPS_ERROR("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    if (access(record_file, F_OK))
    {
        SD_OPS_ERROR("record file %s not exist!\n", record_file);
        return -1;
    }

    SD_OPS_DEBUG("record_file is %s\n", record_file);

    int i = 0, j = 0, count = 0;
    int k = 0;
    unsigned int iframe_time_list[MAX_I_FRAME_NUM] = {0};
    IFrameInfo iframe_list[MAX_I_FRAME_NUM] = {0};

    if (GetIFrameListFromAviFile(record_file, iframe_list, &count) < 0)
    {
        SD_OPS_ERROR("Fail to called GetIFrameListFromAviFile\n");
        return -1;
    }

    for (i=0; i<count; i++)
    {
        iframe_time_list[i] = iframe_list[i].CurIFrameTime;
    }

    /* 采用以下算法搜索会更加准确 */
    j = BinarySearchNearMax(iframe_time_list, i, seek_time);
    j = j < 0 ? 0 : j;

    k = BinarySearchNearMin(iframe_time_list, i, seek_time);
    k = k < 0 ? 0 : k;

#if 1
    int y1, m1, d1, h1, min1, s1;
    int y2, m2, d2, h2, min2, s2;
    int y3, m3, d3, h3, min3, s3;

    ULONG_TO_TIME(iframe_list[j].CurIFrameTime, y1, m1, d1, h1, min1, s1);
    ULONG_TO_TIME(seek_time, y2, m2, d2, h2, min2, s2);
    ULONG_TO_TIME(iframe_list[k].CurIFrameTime, y3, m3, d3, h3, min3, s3);

SD_OPS_INFO("Find index %d in iframe time %u[%04d-%02d-%02d %02d-%02d-%02d] nearest %d[%04d-%02d-%02d %02d-%02d-%02d]\n",
        j, 
        iframe_list[j].CurIFrameTime, y1, m1, d1, h1, min1, s1, 
        seek_time, y2, m2, d2, h2, min2, s2);

SD_OPS_INFO("Find index %d in previous iframe time %u[%04d-%02d-%02d %02d-%02d-%02d] nearest %d[%04d-%02d-%02d %02d-%02d-%02d]\n",
        k, 
        iframe_list[k].CurIFrameTime, y3, m3, d3, h3, min3, s3, 
        seek_time, y2, m2, d2, h2, min2, s2);

#endif

#if 1
    /* 多通道设备定位I帧时，定位到2S时间内最早通道的I帧，优化回放同步 */
    if (SYSTEM_SUPPORT_CHANNEL > 1)
    {
        int fd = -1;
        H264DATAPACKETHEADER frame_head = {0};
        H264DATAPACKETHEADER last_frame_head = {0};
    
        fd = open(record_file, O_RDONLY, S_IRWXU);
        if (fd > 0)
        {
            do {
                /* 首先定位到文件的I帧的位置 */
                if (-1 == lseek(fd, iframe_list[j].CurrFramePos, SEEK_SET))
                {
                    break;
                }
                /* 读帧头(28字节) */
                if (sizeof(H264DATAPACKETHEADER) != read(fd, &frame_head, sizeof(H264DATAPACKETHEADER)))
                {
                    break;
                }

                for (i=0; i<SYSTEM_SUPPORT_CHANNEL; i++)
                {
                    if (j <= 0)
                    {
                        break;
                    }

                    /* 在定位到文件的下一个I帧的位置 */
                    if (-1 == lseek(fd, iframe_list[(j-1)].CurrFramePos, SEEK_SET))
                    {
                        break;
                    }
                    /* 读帧头(28字节) */
                    if (sizeof(H264DATAPACKETHEADER) != read(fd, &last_frame_head, sizeof(H264DATAPACKETHEADER)))
                    {
                        break;
                    }

                    /* 如果相差不到2S，则优先定位到最早的I帧 */
                    if (abs(frame_head.u32RealTimeSec - last_frame_head.u32RealTimeSec) <= 2)
                    {
                        j = ((j-1 < 0) ? 0 : (j-1));
                        k = ((k-1 < 0) ? 0 : (k-1));
                        memcpy(&frame_head, &last_frame_head, sizeof(H264DATAPACKETHEADER));
                        continue;
                    }
                    break;
                }

            }while(0);

            SAFE_CLOSE(fd);
        }
    
        ULONG_TO_TIME(iframe_list[j].CurIFrameTime, y1, m1, d1, h1, min1, s1);
    
        SD_OPS_INFO("[Ch0] Find index %d in iframe time %u[%04d-%02d-%02d %02d-%02d-%02d] nearest %d[%04d-%02d-%02d %02d-%02d-%02d]\n",
                j,
                iframe_list[j].CurIFrameTime, y1, m1, d1, h1, min1, s1, 
                seek_time, y2, m2, d2, h2, min2, s2);
    }
#endif

/*
    时间戳矫正方式：

    如果用户需要搜索的范围是: 2~65
    则最后应该为：4~68
      2                                                 65 
      |                                                 |
      V                                                 V
    ----------------------------------------------  ----------------------------------------------------------
    |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |
    |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |
    |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |
    ----------------------------------------------  ----------------------------------------------------------
    0  4  8  12 16 20 24 28 32 36 40 44 48 52 56    60 64 68 72 76 80 84 88 92 96 100 ...
*/

    if (b_need_correct_time)
    {
        int fd = -1;
        int read_count = -1;
        int is_first_frame = 1;
        H264DATAPACKETHEADER frame_head = {0};
        H264DATAPACKETHEADER tmp_frame_head = {0};

        fd = open(record_file, O_RDONLY, S_IRWXU);
        if (fd < 0)
        {
            SD_OPS_ERROR("Fail to open %s\n", record_file);
        }
        else
        {
            /* 首先定位到文件的I帧的位置 */
            lseek(fd, iframe_list[j].CurrFramePos, SEEK_SET);

            /* 然后逐帧读取当前的数据 */
            while(1)
            {
                /* 读帧头(28字节) */
                read_count = read (fd, &tmp_frame_head, sizeof(H264DATAPACKETHEADER));

                /* 读取长度为0表示文件读完毕 */
                if (0 == read_count)
                {
                    break;
                }

                /* 判断读的长度是否足够 */
                if (sizeof(H264DATAPACKETHEADER) != read_count)
                {
                    /* 文件指针还原到前一帧数据的尾部 */
                    lseek(fd, -read_count, SEEK_CUR);
                    SD_OPS_DEBUG("Read framehead len [%d != 28]!\n", read_count);
                    goto READ_EXIT;
                }

                /* 校验帧头(当读到文件尾的时候，也会校验头出错) */
                if (tmp_frame_head.uDataPacketStartCode != 0xB7010000)
                {
                    /* 文件指针还原到前一帧数据的尾部 */
                    lseek(fd, -read_count, SEEK_CUR);
                    SD_OPS_DEBUG("Framehead start code [%#x < 0xB7010000]!\n", tmp_frame_head.uDataPacketStartCode);
                    goto READ_EXIT;
                }

                /* 如果获取到的帧时间大于seek的时间且当前为I帧，则表示当前已经读完毕，直接跳出(不包括当前的I帧) */
                if ((tmp_frame_head.u32RealTimeSec > seek_time) 
                 && (I_FRAME == tmp_frame_head.byFrameType)
                 && (0 == is_first_frame))
                {
                    /* 文件指针还原到前一帧数据的尾部 */
                    lseek(fd, -read_count, SEEK_CUR);
                    SD_OPS_DEBUG("u32RealTimeSec[%d] > seek_time[%d] and is i frame, read over!\n", 
                            tmp_frame_head.u32RealTimeSec, seek_time);
                    goto READ_EXIT;
                }
                is_first_frame = 0;

                /* 跳过帧数据 */
                lseek(fd, tmp_frame_head.uOverloadLen, SEEK_CUR);

                /* 拷贝帧头数据 */
                memcpy(&frame_head, &tmp_frame_head, sizeof(H264DATAPACKETHEADER));
            }
READ_EXIT:
            p_iframe_info->CurIFrameTime = frame_head.u32RealTimeSec;   // 当前时间
            p_iframe_info->ShowTime = frame_head.uShowTime;             // 当前I帧的相对时间戳
            p_iframe_info->IFrameNum = frame_head.uFrameNo;             // 当前I帧编号在总视频帧里面的编号
            p_iframe_info->CurrFramePos = lseek(fd, 0, SEEK_CUR);       // 当前帧在文件中的偏移地址(PRV格式有效)
            SAFE_CLOSE(fd);
            memcpy(p_iframe_info_pre, p_iframe_info, sizeof(IFrameInfo));
#if 0
printf("frame_head: u32RealTimeSec=%d, uShowTime=%d, CurrFramePos=%d\n",
        frame_head.u32RealTimeSec, frame_head.uShowTime, p_iframe_info->CurrFramePos);
#endif
        }
    }
    else
    {
        memcpy(p_iframe_info, &iframe_list[j], sizeof(IFrameInfo));
        memcpy(p_iframe_info_pre, &iframe_list[k], sizeof(IFrameInfo));
    }

    return 0;
}

/*@breif ReadAviSegLinkByTime 从给定的时间段里面获取符合该时间段的avi文件链表
 *@param [IN]  pop_handle_inx 当前查找的handle索引
 *@param [IN]  start_time 欲查找的开始时间戳
 *@param [IN]  stop_time  欲查找的结束时间戳
 *@param [IN]  record_type  欲查找的文件类型
 *@param [OUT] plink  获取到的链表头指针
 *@return 成功返回文件个数
 *@       失败返回-1
 *@note   查询的时间段不能超过一天即24小时
 */
int ReadAviSegLinkByTime(int pop_handle_inx, 
                                    time_t start_time, 
                                    time_t stop_time, 
                                    int record_type, 
                                    AviSegLinkList **plink)
{
    if (!plink)
    {
       SD_OPS_ERROR("NULL POINTER EXCEPTION!\n");
       return -1;
    }

    if (stop_time < start_time)
    {
        SD_OPS_ERROR("Seek time format error!\n");
        return -1;
    }

    /* 搜索时间限定不能超过24H */
    if (stop_time - start_time > 24*60*60)
    {
        SD_OPS_ERROR("Find time segment too long! not more than 24h!\n");
        return -1;
    }

    int i = 0;
    int file_count = 0;
    int real_count = 0;
    char dest_dir[MAX_FILE_PATH_STR] = {0};
    struct tm s_tm = {0};
    lpFileInfo p_file_list[MAX_FILE_NUM] = {NULL};
	lpFileInfo p_file_free = NULL;

    AviSegLinkList *plink_tmp  = NULL;
    AviSegLinkList *plink_tmp2 = NULL;
    time_t startTimeTemp = start_time;

    localtime_r(&startTimeTemp, &s_tm);
    snprintf(dest_dir, sizeof(dest_dir), "%s/video/%4d%02d%02d", 
             SD_MOUNT_POINT, s_tm.tm_year+1900, s_tm.tm_mon+1, s_tm.tm_mday);

    if (SD_GetFileListInDir(dest_dir, VIDEO_FORMAT_SUFFIX, p_file_list, &file_count))
    {
        SD_OPS_ERROR("Fail to get files set list in dir %s\n", dest_dir);
        return -1;
    }

    if (file_count < 1)
    {
        SD_OPS_ERROR("Dir %s not has suit dir!\n", dest_dir);
        SAFE_FREE(p_file_list[0]);
        return -1;
    }

    pthread_mutex_lock(&seg_wr_lock);

	// 先记录指针地址
	p_file_free = p_file_list[0];
	// 进行一次排序，不排序会导致当天录像的时间范围异常
	qsort(*p_file_list, file_count, sizeof(stuFileInfo), cmp_time_stamp);

    plink_tmp2 = plink_tmp = *plink;

    for (i=0; i<file_count; i++)
    {
        if (p_file_list[i]->struFileNameInfo.nStartTime >= p_file_list[i]->struFileNameInfo.nStopTime)
        {
            continue;
        }
        /* 此处先不用管根据类型选择录像 */
    //  if (p_after_sort_file_set[i].record_type == record_type)
        {
            plink_tmp = AddNewAviSegLinkList(plink_tmp2);
            plink_tmp->start_time  = p_file_list[i]->struFileNameInfo.nStartTime;
            plink_tmp->stop_time   = p_file_list[i]->struFileNameInfo.nStopTime;
            plink_tmp->record_type = (unsigned int)ToNetRecordTypeMap((eRecordType)p_file_list[i]->struFileNameInfo.eRecType);
            memcpy(plink_tmp->file_name, p_file_list[i]->szFileName, MAX_NAME_STR);
        }

#ifdef GET_LIST_DBG
        char begin_time[32] = {0};
        char end_time[32]   = {0};
        time_t timeTemp;
        timeTemp = plink_tmp->start_time;
        TIME_ULONG_TO_STRING(timeTemp, begin_time);
        timeTemp = plink_tmp->stop_time;
        TIME_ULONG_TO_STRING(timeTemp, end_time);
        SD_OPS_DEBUG("plink_tmp->file_name = %s [%s - %s - %d]\n", plink_tmp->file_name, begin_time, end_time, plink_tmp->record_type);
#endif

        plink_tmp->prev_link  = plink_tmp2;
        plink_tmp->next_link  = (void *)NULL;
        plink_tmp2->next_link = (void *)plink_tmp;    
        plink_tmp2 = plink_tmp;
        memcpy(&g_pFilesListSet[pop_handle_inx][real_count], p_file_list[i], sizeof(stuFileInfo));
        real_count++;
    }
    g_read_global_static_var[pop_handle_inx].playback_file_list_index = real_count - 1;

    pthread_mutex_unlock(&seg_wr_lock);

	SAFE_FREE(p_file_free);

    return real_count;
}

/*@breif SearchRecordByTime 查询某一天是否有录像文件
 *@param [IN]  iYear 欲查找的开始时间——年
 *@param [IN]  iMonth 欲查找的开始时间——月
 *@param [IN]  iDay 欲查找的开始时间——日
 *@param [OUT] ifExist 是否有AVI文件
 *@return 成功返回 0
 *@       失败返回-1
 */
int SearchRecordByTime(int iYear, int iMonth, int iDay, int *ifExist)
{
    if (!ifExist)
    {
       SD_OPS_ERROR("NULL POINTER EXCEPTION!\n");
       return -1;
    }

    *ifExist = 0;

    char dir_path[MAX_FILE_PATH_STR] = {0};

    snprintf(dir_path, sizeof(dir_path), "%s/video/%04d%02d%02d", SD_MOUNT_POINT, iYear, iMonth, iDay);

    if (0 == access(dir_path, F_OK))
    {
        *ifExist = 1;
    }
    else
    {
        *ifExist = 0;
    }

    return 0;
}

/*@breif SeekTimePointFromRecFileList 从给出的文件链表结构体里面获取对应的AVI文件进而获取对应的I帧帧号
 *@param [IN] pop_index 当前回放的序号
 *@param [IN] seek_time 欲查找的开始时间戳
 *@param [OUT] get_file_info 获取到的录像文件信息
 *@param [OUT] curr_rec_index 获取到当前操作的录像文件的索引
 *@return 成功返回0
 *@       失败返回-1
 */
int SeekTimePointFromRecFileList(int pop_index, 
                                                unsigned int seek_time, 
                                                void *get_file_info, 
                                                int *curr_rec_index)
{
    if (!get_file_info || !curr_rec_index)
    {
       SD_OPS_ERROR("NULL POINTER EXCEPTION!\n");
       return -1;
    }

    int read_curr_rec_num = 0;
    unsigned int playback_file_list_start_time = 0;
    unsigned int playback_file_list_stop_time = 0;
    unsigned int playback_file_list_end_start_time = 0;

    lpFileInfo p_file_info = (lpFileInfo)get_file_info;

    pthread_mutex_lock(&g_get_files_list_set_mutex);
    read_curr_rec_num = g_read_global_static_var[pop_index].playback_file_list_index;
    pthread_mutex_unlock(&g_get_files_list_set_mutex);

    lpFileInfo p_file_set   = NULL;
    p_file_set = (lpFileInfo)calloc(read_curr_rec_num+1, sizeof(stuFileInfo));
    if (NULL == p_file_set)
    {
        SD_OPS_ERROR("Fail to calloc!\n");
        return -1;
    }

    /* 拷贝到临时结构体 */
    pthread_mutex_lock(&g_get_files_list_set_mutex);
    memcpy(p_file_set, g_pFilesListSet[pop_index], (read_curr_rec_num+1)*sizeof(stuFileInfo));
    pthread_mutex_unlock(&g_get_files_list_set_mutex);

    playback_file_list_start_time = p_file_set[0].struFileNameInfo.nStartTime;
    playback_file_list_stop_time = p_file_set[read_curr_rec_num].struFileNameInfo.nStopTime;
    playback_file_list_end_start_time = p_file_set[read_curr_rec_num].struFileNameInfo.nStartTime;

    /* 判断定位的区域是否有录像，如果没有，则根据情况起始时间点 */
    if (seek_time < playback_file_list_start_time
     || seek_time > playback_file_list_stop_time)
    {
        // Warning: seek time 1601337600 is out of [1601372289, 1601390597]!
    
        SD_OPS_DEBUG("Warning: seek time %d is out of [%d, %d]!\n", 
                seek_time, playback_file_list_start_time, playback_file_list_stop_time);

        /* Seek的时间早于需要查找片段的开始时间 */
        if (seek_time < playback_file_list_start_time)
        {
            SD_OPS_DEBUG("seek_time less of seek_seg_start_index_time, set in accordance with the situation!\n");
            /* 如果Seek的时间早于用户搜索录像片段的起始时间，则将Seek时间置为搜索片段的起始时间 */
            if (g_read_global_static_var[pop_index].usr_search_start_time <= playback_file_list_stop_time)
            {
                SD_OPS_DEBUG("set it into the search end of start time %d!\n", g_read_global_static_var[pop_index].usr_search_start_time);
                seek_time = g_read_global_static_var[pop_index].usr_search_start_time;
            }
            else
            {
                /* 否则将Seek时间置为当前录像片段的起始时间 */
                SD_OPS_DEBUG("set it to start_time of seek_seg_start_index\n");
                seek_time = playback_file_list_start_time;
            }
        }
        /* Seek的时间晚于需要查找片段的结束时间 */
        else if (seek_time > playback_file_list_stop_time)
        {
            SD_OPS_DEBUG("seek_time over seek_seg_start_index_time, set in accordance with the situation!\n");
            /* 如果Seek的时间早于用户搜索录像片段的起始时间，则将Seek时间置为搜索片段的起始时间 */
            if (g_read_global_static_var[pop_index].usr_search_start_time <= playback_file_list_stop_time)
            {
                SD_OPS_DEBUG("set it into the search end of start time %d!\n", g_read_global_static_var[pop_index].usr_search_start_time);
                seek_time = playback_file_list_end_start_time;
            }
            else
            {
                SD_OPS_DEBUG("set it into the start time of seg!\n");
                seek_time = playback_file_list_start_time;
            }
        }
    }

    int i = 0;
    unsigned int rec_start_time_list[MAX_FILE_NUM] = {0};

    for (i=0; i<=read_curr_rec_num; i++)
    {
        rec_start_time_list[i] = p_file_set[i].struFileNameInfo.nStartTime;
    }

    /* 二分法查找获取到最接近的文件名 */
    i = BinareySearchEx(rec_start_time_list, i, seek_time);
    i = i < 0 ? 0 : i;

    *curr_rec_index = i;

    /* 做一些精调处理(注意不要数组越界) */
    if ((*curr_rec_index < read_curr_rec_num)
     && (seek_time > p_file_set[*curr_rec_index].struFileNameInfo.nStopTime)
     && (seek_time <= p_file_set[*curr_rec_index+1].struFileNameInfo.nStartTime))
    {
        SD_OPS_DEBUG("seek time: %d, find it [xxx, %d], next is [%d, xxx], reindex it!\n",
                seek_time,
                p_file_set[*curr_rec_index].struFileNameInfo.nStopTime, 
                p_file_set[*curr_rec_index+1].struFileNameInfo.nStartTime);
                (*curr_rec_index) ++;
    }

#ifdef GET_LIST_DBG
SD_OPS_DEBUG("playback_file_list_handle[%d].record_type = %d, start_time = %d, stop_time = %d\n", 
        *curr_rec_index,
        p_file_set[*curr_rec_index].struFileNameInfo.eRecType,
        p_file_set[*curr_rec_index].struFileNameInfo.nStartTime,
        p_file_set[*curr_rec_index].struFileNameInfo.nStopTime);
#endif

#ifdef GET_LIST_DBG
    SD_OPS_DEBUG("i = %d, *curr_rec_index = %d, get_rec_name = %s\n", 
           i, *curr_rec_index, p_file_set[*curr_rec_index].szFileName);
#endif

    /* 从当前AVI文件里面获取最接近的I帧帧号 */
    char rec_file[MAX_PATH_STR_NUM] = {0};
    char rec_dir[MAX_NAME_STR] = {0};
    time_t timeTemp = p_file_set[*curr_rec_index].struFileNameInfo.nStartTime;

    TIME_STAMP_TO_STRING_DAY(timeTemp, rec_dir);

SD_OPS_DEBUG("rec_dir = %s\n", rec_dir);

    snprintf(rec_file, sizeof(rec_file), "%s/video/%s/%s", 
              SD_MOUNT_POINT, rec_dir, p_file_set[*curr_rec_index].szFileName);

#ifdef GET_LIST_DBG
    SD_OPS_DEBUG("rec_dir = %s, rec_file = %s\n", rec_dir, rec_file);
#endif

    /* 如果当前文件链表里面的avi文件不存在(被覆盖),则需要跳过该文件,继续判断下一个文件 */
    while(access(rec_file, F_OK))
    {
        SD_OPS_DEBUG("%s not exist! Skip it and continue ...\n", rec_file);
        if(++(*curr_rec_index) > read_curr_rec_num)
        {
            SD_OPS_DEBUG("*curr_rec_index = %d over last_index = %d\n", 
                    *curr_rec_index, read_curr_rec_num);
            SAFE_FREE(p_file_set);
            return -1;
        }
        memset(rec_file, 0, sizeof(rec_file));
        snprintf(rec_file, sizeof(rec_file), "%s/video/%s/%s",
                 SD_MOUNT_POINT, rec_dir, p_file_set[*curr_rec_index].szFileName);
    }

    memcpy(p_file_info, &p_file_set[*curr_rec_index], sizeof(stuFileInfo));
    SAFE_FREE(p_file_set);

    return 0;
}

/*@breif SeekTimePointFromRecFileList 从给出的文件链表结构体里面获取对应的AVI文件进而获取对应的I帧帧号
 *@param [IN] pop_index 当前回放的序号
 *@param [IN] seek_time 欲查找的开始时间戳
 *@param [OUT] get_file_info 获取到的录像文件信息
 *@param [OUT] curr_rec_index 获取到当前操作的录像文件的索引
 *@return 成功返回0
 *@       失败返回-1
 */
int SeekTimePointFromAviFileList(int pop_index, 
                                                unsigned int seek_time, 
                                                void *get_file_info, 
                                                int *curr_rec_index,
                                                void *iframe_info,
                                                void *iframe_info_pre)
{
    if (!get_file_info || !curr_rec_index || !iframe_info || !iframe_info_pre)
    {
       SD_OPS_ERROR("NULL POINTER EXCEPTION!\n");
       return -1;
    }

    int read_curr_rec_num = 0;
    unsigned int playback_file_list_start_time = 0;
    unsigned int playback_file_list_stop_time = 0;
    unsigned int playback_file_list_end_start_time = 0;

    lpFileInfo p_file_info = (lpFileInfo)get_file_info;

    pthread_mutex_lock(&g_get_files_list_set_mutex);
    read_curr_rec_num = g_read_global_static_var[pop_index].playback_file_list_index;
    pthread_mutex_unlock(&g_get_files_list_set_mutex);

    lpFileInfo p_file_set   = NULL;
    p_file_set = (lpFileInfo)calloc(read_curr_rec_num+1, sizeof(stuFileInfo));
    if (NULL == p_file_set)
    {
        SD_OPS_ERROR("Fail to calloc!\n");
        return -1;
    }

    /* 拷贝到临时结构体 */
    pthread_mutex_lock(&g_get_files_list_set_mutex);
    memcpy(p_file_set, g_pFilesListSet[pop_index], (read_curr_rec_num+1)*sizeof(stuFileInfo));
    pthread_mutex_unlock(&g_get_files_list_set_mutex);

    playback_file_list_start_time = p_file_set[0].struFileNameInfo.nStartTime;
    playback_file_list_stop_time = p_file_set[read_curr_rec_num].struFileNameInfo.nStopTime;
    playback_file_list_end_start_time = p_file_set[read_curr_rec_num].struFileNameInfo.nStartTime;

    /* 判断定位的区域是否有录像，如果没有，则根据情况起始时间点 */
    if (seek_time < playback_file_list_start_time
     || seek_time > playback_file_list_stop_time)
    {
        // Warning: seek time 1601337600 is out of [1601372289, 1601390597]!
    
        SD_OPS_DEBUG("Warning: seek time %d is out of [%d, %d]!\n", 
                seek_time, playback_file_list_start_time, playback_file_list_stop_time);

        /* Seek的时间早于需要查找片段的开始时间 */
        if (seek_time < playback_file_list_start_time)
        {
            SD_OPS_DEBUG("seek_time less of seek_seg_start_index_time, set in accordance with the situation!\n");
            /* 如果Seek的时间早于用户搜索录像片段的起始时间，则将Seek时间置为搜索片段的起始时间 */
            if (g_read_global_static_var[pop_index].usr_search_start_time <= playback_file_list_stop_time)
            {
                SD_OPS_DEBUG("set it into the search end of start time %d!\n", g_read_global_static_var[pop_index].usr_search_start_time);
                seek_time = g_read_global_static_var[pop_index].usr_search_start_time;
            }
            else
            {
                /* 否则将Seek时间置为当前录像片段的起始时间 */
                SD_OPS_DEBUG("set it to start_time of seek_seg_start_index\n");
                seek_time = playback_file_list_start_time;
            }
        }
        /* Seek的时间晚于需要查找片段的结束时间 */
        else if (seek_time > playback_file_list_stop_time)
        {
            SD_OPS_DEBUG("seek_time over seek_seg_start_index_time, set in accordance with the situation!\n");
            /* 如果Seek的时间早于用户搜索录像片段的起始时间，则将Seek时间置为搜索片段的起始时间 */
            if (g_read_global_static_var[pop_index].usr_search_start_time <= playback_file_list_stop_time)
            {
                SD_OPS_DEBUG("set it into the search end of start time %d!\n", g_read_global_static_var[pop_index].usr_search_start_time);
                seek_time = playback_file_list_end_start_time;
            }
            else
            {
                SD_OPS_DEBUG("set it into the start time of seg!\n");
                seek_time = playback_file_list_start_time;
            }
        }
    }

    int i = 0;
    unsigned int rec_start_time_list[MAX_FILE_NUM] = {0};

    for (i=0; i<=read_curr_rec_num; i++)
    {
        rec_start_time_list[i] = p_file_set[i].struFileNameInfo.nStartTime;
    }

    /* 二分法查找获取到最接近的文件名 */
    i = BinareySearchEx(rec_start_time_list, i, seek_time);
    i = i < 0 ? 0 : i;

    *curr_rec_index = i;

    /* 做一些精调处理(注意不要数组越界) */
    if ((*curr_rec_index < read_curr_rec_num)
     && (seek_time > p_file_set[*curr_rec_index].struFileNameInfo.nStopTime)
     && (seek_time <= p_file_set[*curr_rec_index+1].struFileNameInfo.nStartTime))
    {
        SD_OPS_DEBUG("seek time: %d, find it [xxx, %d], next is [%d, xxx], reindex it!\n",
                seek_time,
                p_file_set[*curr_rec_index].struFileNameInfo.nStopTime, 
                p_file_set[*curr_rec_index+1].struFileNameInfo.nStartTime);
                (*curr_rec_index) ++;
    }

#ifndef GET_LIST_DBG
SD_OPS_DEBUG("playback_file_list_handle[%d].record_type = %d, start_time = %d, stop_time = %d\n", 
        *curr_rec_index,
        p_file_set[*curr_rec_index].struFileNameInfo.eRecType,
        p_file_set[*curr_rec_index].struFileNameInfo.nStartTime,
        p_file_set[*curr_rec_index].struFileNameInfo.nStopTime);
#endif

#ifndef GET_LIST_DBG
    SD_OPS_DEBUG("i = %d, *curr_rec_index = %d, get_rec_name = %s\n", 
           i, *curr_rec_index, p_file_set[*curr_rec_index].szFileName);
#endif

    /* 从当前AVI文件里面获取最接近的I帧帧号 */
    char rec_file[MAX_PATH_STR_NUM] = {0};
    char rec_dir[MAX_NAME_STR] = {0};

    TIME_STAMP_TO_STRING_DAY(p_file_set[*curr_rec_index].struFileNameInfo.nStartTime, rec_dir);

SD_OPS_DEBUG("rec_dir = %s\n", rec_dir);

    snprintf(rec_file, sizeof(rec_file), "%s/video/%s/%s", 
              SD_MOUNT_POINT, rec_dir, p_file_set[*curr_rec_index].szFileName);

#ifndef GET_LIST_DBG
    SD_OPS_DEBUG("rec_dir = %s, rec_file = %s\n", rec_dir, rec_file);
#endif

    /* 如果当前文件链表里面的avi文件不存在(被覆盖),则需要跳过该文件,继续判断下一个文件 */
    while(access(rec_file, F_OK))
    {
        SD_OPS_DEBUG("%s not exist! Skip it and continue ...\n", rec_file);
        if(++(*curr_rec_index) > read_curr_rec_num)
        {
            SD_OPS_DEBUG("*curr_rec_index = %d over last_index = %d\n", 
                    *curr_rec_index, read_curr_rec_num);
            SAFE_FREE(p_file_set);
            return -1;
        }
        memset(rec_file, 0, sizeof(rec_file));
        snprintf(rec_file, sizeof(rec_file), "%s/video/%s/%s",
                 SD_MOUNT_POINT, rec_dir, p_file_set[*curr_rec_index].szFileName);
    }

    memcpy(p_file_info, &p_file_set[*curr_rec_index], sizeof(stuFileInfo));
    SAFE_FREE(p_file_set);
    
    return GetRecordFileSeekTimeFrameInfo(rec_file, seek_time, 0, iframe_info, iframe_info_pre);
}
void printsegtime(int FrameTime)
{
    struct tm tm_time;
    time_t timeTemp = FrameTime;

    localtime_r(&timeTemp, &tm_time);

    SD_OPS_DEBUG("time:%d-%d-%d %d:%d:%d\n",tm_time.tm_year+1900,tm_time.tm_mon+1,tm_time.tm_mday,tm_time.tm_hour,tm_time.tm_min,tm_time.tm_sec);
}

/* 停止对应录像的回放，关闭对应的句柄，释放资源，还原原来的全局变量 */
int StopRecordPlayback(int pop_index)
{
    pthread_mutex_lock(&global_pop_frame_mutex);
    SD_OPS_DEBUG("stop palyback %s handle %d\n", VIDEO_FORMAT_SUFFIX, pop_index);
    StopAviAction(STOP_READ_AVI, pop_index);

    SAFE_FREE(popFrameBuff[pop_index]);

    g_read_global_static_var[pop_index].curr_avi_index = 0;
    g_read_global_static_var[pop_index].last_ops_avi_index = -1;
    g_read_global_static_var[pop_index].curr_ops_avi_index = -1;
    g_avi_file_read_write_info.read_write_status = IS_IDLE;

    pthread_mutex_unlock(&global_pop_frame_mutex);

    return 0;
}

/*@breif pop_one_frame_from_sector 回放抛出一帧数据给上层
 *@param [IN] PoPhandle Pop句柄
 *@param [IN] rec_type 欲查找的弹出类型(暂不支持)
 *@param [OUT] p_frame_buffer 获取到的帧缓冲
 *@param [OUT] pSize 获取到的帧的大小
 *@param [OUT] pFrametype 获取到的帧的类型
 *@param [OUT] pKeyframe 获取到的帧是否为关键帧
 *@param [OUT] pRealTimeSec 获取到的帧的绝对时间戳(秒)
 *@param [OUT] pRealTimeUSec 获取到的帧的绝对时间戳(微秒)
 *@param [IN] pNeedSeek 是否需要seek定位(首次和双击需要进行seek定位)
 *@return 返回0表示成功抛出一帧数据
 *@      返回-1表示抛出错误
 *@      返回1表示文件抛出结束
 *@      返回2表示文件不存在，跳过该文件(注意，此时p_frame_buffer为空，不可触发回调)
 */
extern int pop_one_frame_from_sector(PopHandle *PoPhandle, 
                                             int rec_type, 
                                             char **p_frame_buffer, 
                                             int *pSize, 
                                             int *pFrametype, 
                                             int *pKeyframe, 
                                             unsigned int *pRealTimeSec, 
                                             unsigned int *pRealTimeUSec,
                                             int *pNeedSeek,
                                             int *VipFrame)
{
    if (!PoPhandle || !pSize || !pFrametype || !pKeyframe || !pRealTimeSec || !pRealTimeUSec || !pNeedSeek)
    {
        SD_OPS_ERROR("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    *pSize = 0;

    /* 如果模块暂停，则返回结束 */
    if (g_sdops_pause_flag)
    {
        SD_OPS_DEBUG("pop while init == 0\n");
        return 1;
    }

    int ret = 0;
    int errorcode = 0;
    int pop_index = -1;
    stuFileInfo rec_file_info = {0};
    char ops_rec_file[MAX_PATH_STR_NUM] = {0};
    char ops_rec_dir[MAX_NAME_STR]   = {0};
    int seek_time_is_zero = 0;          // 当前开始播放的时间是否为00:00:00
    int effect_count = 0;
    int file_offset = 0;
    IFrameInfo iframe_list[MAX_I_FRAME_NUM];
    IFrameInfo iframe_info = {0, 0, 0, 0};
    IFrameInfo iframe_info_pre = {0, 0, 0, 0};

    H264DATAPACKETHEADER frame_head = {
        .uDataPacketStartCode = 0xB7010000,
        .uFrameNo             = 0x0,
        .byFrameType          = 0x01,  //I帧(0x1), P帧(0x2), 音频帧(0x8)
        .byLoadType           = 0x0,   //H264(0x0), H265(0x4), MJPEG(0x5); PktG711_ALAW(0x1),PktG711_ULAW(0x2)
        .uShowTime            = 0x0,
        .u32RealTimeSec       = 0x0,
        .u32RealTimeUSec      = 0x0,
        .uReserved            = 0x0,
        .uOverloadLen         = 0x0
    };

    pthread_mutex_lock(&global_pop_frame_mutex);

	int i = 0;
	int found = 0;
	for (i = 0;i < MAX_OPERATE_HANDLE;i++)
	{
		if (PoPhandle == pop_handle)
		{
			pop_index = i;
			found = 1;
			break;
		}
	}

	if (0 == found)
	{
		errorcode = ERR_DISK_HAND_FULL;     // 已满，无法请求
        pthread_mutex_unlock(&global_pop_frame_mutex);
        return -1;                          // 最多同时只有两个pop
    }

    /* 首先需要进行seek定位，定位了之后就知道当前操作的数组下标是哪一个了 */
    if (*pNeedSeek)
    {
#ifdef SD_PLK_DBG
        SD_OPS_DEBUG("Start to seek time point from %s file list [seek_time %d]...\n", VIDEO_FORMAT_SUFFIX, PoPhandle->start_time);
#endif
        if (SeekTimePointFromAviFileList(pop_index,
                                            PoPhandle->start_time, 
                                            &rec_file_info, 
                                            &g_read_global_static_var[pop_index].curr_avi_index,
                                            &iframe_info,
                                            &iframe_info_pre))
        {
            SD_OPS_ERROR("Fail to call SeekTimePointFromRecFileList\n");
            pthread_mutex_unlock(&global_pop_frame_mutex);
            return -1;
        }

//只有在拖动到00:00:00时，才定位到前一个I帧播放。
        if (0 == (PoPhandle->start_time % 86400))
        {
            seek_time_is_zero = 1;
        }
        else
        {
            seek_time_is_zero = 0;
        }

#ifdef SD_PLK_DBG
        SD_OPS_DEBUG("Seek current start_time=%d, rec_name=%s!\n", PoPhandle->start_time, rec_file_info.szFileName);
#endif
        g_read_global_static_var[pop_index].curr_ops_avi_index = g_read_global_static_var[pop_index].curr_avi_index;

#ifdef SD_PLK_DBG
        SD_OPS_DEBUG("After seek the curr_ops_avi_index is %d, file start time is %d\n", 
                g_read_global_static_var[pop_index].curr_ops_avi_index, rec_file_info.struFileNameInfo.nStartTime);
#endif

        g_read_global_static_var[pop_index].rec_file_start_timestamp = rec_file_info.struFileNameInfo.nStartTime;
#ifdef SD_PLK_DBG
        SD_OPS_DEBUG("rec_file_start_timestamp = %d\n", rec_file_info.struFileNameInfo.nStartTime);
#endif
        *pNeedSeek = 0;
    }

    if (g_read_global_static_var[pop_index].curr_ops_avi_index != g_read_global_static_var[pop_index].last_ops_avi_index)
    {
        StopAviAction(STOP_READ_AVI, pop_index);
        g_avi_file_read_write_info.read_write_status = IS_IDLE;

        SAFE_FREE (popFrameBuff[pop_index]);

        TIME_STAMP_TO_STRING_DAY(g_pFilesListSet[pop_index][g_read_global_static_var[pop_index].curr_ops_avi_index].struFileNameInfo.nStartTime, ops_rec_dir);
        snprintf(ops_rec_file, sizeof(ops_rec_file), "%s/video/%s/%s",  SD_MOUNT_POINT, ops_rec_dir, 
                 g_pFilesListSet[pop_index][g_read_global_static_var[pop_index].curr_ops_avi_index].szFileName);

#ifdef SD_PLK_DBG
        SD_OPS_DEBUG("g_pFilesListSet[%d][%d].szFileName = %s\n",
                pop_index, g_read_global_static_var[pop_index].curr_ops_avi_index, 
                g_pFilesListSet[pop_index][g_read_global_static_var[pop_index].curr_ops_avi_index].szFileName);
        SD_OPS_DEBUG("Seek next ops_rec_file = %s!\n", ops_rec_file);
#endif

        if (access(ops_rec_file, F_OK))
        {
#ifdef SD_PLK_DBG
            SD_OPS_DEBUG("[stop_index=%d curr_index=%d] %s not exist! Skip it and continue ...\n", 
                    g_read_global_static_var[pop_index].playback_file_list_index, 
                    g_read_global_static_var[pop_index].curr_ops_avi_index, 
                    ops_rec_file);
#endif

#ifdef SD_PLK_DBG
            SD_OPS_DEBUG("g_read_global_static_var[pop_index].curr_ops_avi_index = %d, need ++!\n", 
                    g_read_global_static_var[pop_index].curr_ops_avi_index);
#endif

            if (++g_read_global_static_var[pop_index].curr_ops_avi_index > g_read_global_static_var[pop_index].playback_file_list_index)
            {
#ifdef SD_PLK_DBG
                SD_OPS_DEBUG("%s file play over!\n", VIDEO_FORMAT_SUFFIX);
#endif
                g_read_global_static_var[pop_index].curr_avi_index = 0;
                g_read_global_static_var[pop_index].last_ops_avi_index = -1;
                g_read_global_static_var[pop_index].curr_ops_avi_index = -1;
                pthread_mutex_unlock(&global_pop_frame_mutex);
                return 2;
            }

            pthread_mutex_unlock(&global_pop_frame_mutex);
            return 0;
        }

        /* 针对回放途中，切换文件的时候，需要获取一下当前的第一个I帧时间 */
        if (0 == iframe_info.IFrameNum)
        {
            /* 获取当前文件的I帧列表，以便后面换文件使用 */
            if (GetIFrameListFromAviFile(ops_rec_file, iframe_list, &effect_count))
            {
                SD_OPS_ERROR("Fail to called GetIFrameListFromAviFile!\n");
                errorcode = ERR_DISK_OPEN_FILE_FAIL; //无法打开文件
                *p_frame_buffer = NULL;
                pthread_mutex_unlock(&global_pop_frame_mutex);
                return -1;
            }
            #if 0
            /* 更新对当前的时间戳 */
            g_read_global_static_var[pop_index].video_real_time = iframe_list[0].CurIFrameTime;
            g_read_global_static_var[pop_index].audio_real_time = iframe_list[0].CurIFrameTime;
            #endif
        }

        /* 判断当前文件是否正在写，如果正在写，则跳过该文件 */
        while(1)
        {
            if ((0 == strncmp(g_avi_file_read_write_info.ops_file_name, rec_file_info.szFileName, sizeof(rec_file_info.szFileName)))
              && (IS_WRITTING == g_avi_file_read_write_info.read_write_status))
            {
                SD_OPS_DEBUG("%s file %s is writting, Skip it and continue ...\n", VIDEO_FORMAT_SUFFIX, rec_file_info.szFileName);
                continue;
            }
            else
            {
                strncpy(g_avi_file_read_write_info.ops_file_name, rec_file_info.szFileName, sizeof(rec_file_info.szFileName));
                g_avi_file_read_write_info.read_write_status = IS_READING;
                break;
            }
        }

        /* 初始化当前AVI文件为读模式 */
        if (InitAviActionMode(ops_rec_file, 1, pop_index))
        {
            SD_OPS_ERROR("Fail to called InitAviActionMode!\n");
            errorcode = ERR_DISK_OPEN_FILE_FAIL; //无法打开文件
            *p_frame_buffer = NULL;
            pthread_mutex_unlock(&global_pop_frame_mutex);
            return -1;   
        }

        if (popFrameBuff[pop_index] == NULL)
        {
            popFrameBuff[pop_index] = calloc(1, MAX_SIZE_PER_FRAME);
        }

        g_read_global_static_var[pop_index].rec_file_start_timestamp = g_pFilesListSet[pop_index][g_read_global_static_var[pop_index].curr_ops_avi_index].struFileNameInfo.nStartTime;
        g_read_global_static_var[pop_index].last_ops_avi_index = g_read_global_static_var[pop_index].curr_ops_avi_index;
        g_read_global_static_var[pop_index].rec_file_get_seektime_sec = 0;
    }

    if ( 0 == seek_time_is_zero && iframe_info.IFrameNum > 0)
    {
        /* 私有格式需要传入当前I帧的位置 */
        if (SetAviIndexPosition(pop_index, iframe_info.CurrFramePos))
        {
            SD_OPS_ERROR("Fail to called SetAviIndexPosition!\n");
            StopAviAction(STOP_READ_AVI, pop_index);
            g_avi_file_read_write_info.read_write_status = IS_IDLE;
            pthread_mutex_unlock(&global_pop_frame_mutex);
            return -1;
        }
    }
    else if (1 == seek_time_is_zero && iframe_info_pre.IFrameNum > 0)
    {
        SD_OPS_DEBUG(" now is 00:00:00, set position pre frame\n");
        /* 私有格式需要传入当前I帧的位置 */
        if (SetAviIndexPosition(pop_index, iframe_info_pre.CurrFramePos))
        {
            SD_OPS_ERROR("Fail to called SetAviIndexPosition previous!\n");
            StopAviAction(STOP_READ_AVI, pop_index);
            g_avi_file_read_write_info.read_write_status = IS_IDLE;
            pthread_mutex_unlock(&global_pop_frame_mutex);
            return -1;
        }        
    }

    *p_frame_buffer = popFrameBuff[pop_index];

    /* 释放了，就直接报错 */
    if (NULL == *p_frame_buffer)
    { 
        SD_OPS_ERROR ("buffer is null\n");
        return -1;
    }

    /* 读取一帧PRV数据 */
    ret = ReadPrvFrame(pop_index, (void *)&frame_head, (unsigned char *)(*p_frame_buffer+sizeof(H264DATAPACKETHEADER)), MAX_SIZE_PER_FRAME, &file_offset);
    #if 0
    if (1 == record_stop_in_zero)
    {
        if (frame_head.u32RealTimeSec > zero_iframe_info_aft.CurIFrameTime)
        {
            SD_OPS_INFO("play record at 00:00:00, but end at:%u, %u\n", frame_head.u32RealTimeSec, zero_iframe_info_aft.CurIFrameTime);
            record_stop_in_zero = 0;
            g_record_across_day = 0;

            memset(&zero_iframe_info_pre, 0, sizeof(IFrameInfo));
            memset(&zero_iframe_info_aft, 0, sizeof(IFrameInfo));

            StopAviAction(STOP_READ_AVI, pop_index);

            g_read_global_static_var[pop_index].curr_avi_index = -1;
            g_read_global_static_var[pop_index].last_ops_avi_index = -1;
            g_read_global_static_var[pop_index].curr_ops_avi_index = -1;
            g_avi_file_read_write_info.read_write_status = IS_IDLE;

            pthread_mutex_unlock(&global_pop_frame_mutex);
            return 1;            
        }
    }
    #endif

    if (1 == ret)
    {
#ifdef SD_PLK_DBG
        SD_OPS_DEBUG("[index=%d] Read %s file Over!\n", 
                g_read_global_static_var[pop_index].curr_ops_avi_index, VIDEO_FORMAT_SUFFIX);

        SD_OPS_DEBUG("g_read_global_static_var[pop_index].curr_ops_avi_index = %d\n", 
                g_read_global_static_var[pop_index].curr_ops_avi_index);
        SD_OPS_DEBUG("g_read_global_static_var[pop_index].playback_file_list_index = %d\n",
                g_read_global_static_var[pop_index].playback_file_list_index);

        SD_OPS_DEBUG("g_pFilesListSet[%d][%d].start_time = %d\n", pop_index, g_read_global_static_var[pop_index].curr_ops_avi_index-1,  
                g_pFilesListSet[pop_index][g_read_global_static_var[pop_index].curr_ops_avi_index-1].struFileNameInfo.nStartTime);
        SD_OPS_DEBUG("g_pFilesListSet[%d][%d].start_time = %d\n", pop_index, g_read_global_static_var[pop_index].curr_ops_avi_index,  
                g_pFilesListSet[pop_index][g_read_global_static_var[pop_index].curr_ops_avi_index].struFileNameInfo.nStartTime);
        SD_OPS_DEBUG("g_pFilesListSet[%d][%d].start_time = %d\n", pop_index, g_read_global_static_var[pop_index].curr_ops_avi_index+1, 
                g_pFilesListSet[pop_index][g_read_global_static_var[pop_index].curr_ops_avi_index+1].struFileNameInfo.nStartTime);
#endif
        StopAviAction(STOP_READ_AVI, pop_index);
        g_avi_file_read_write_info.read_write_status = IS_IDLE;

#ifdef SD_PLK_DBG
        SD_OPS_DEBUG("g_read_global_static_var[pop_index].curr_ops_avi_index = %d, need ++!\n", 
                g_read_global_static_var[pop_index].curr_ops_avi_index);
#endif
        /* 当前搜索到的文件列表里面的AVI全部播放完毕 */
        if (++g_read_global_static_var[pop_index].curr_ops_avi_index > g_read_global_static_var[pop_index].playback_file_list_index)
        {
#ifdef SD_PLK_DBG
            SD_OPS_DEBUG("%s file play over!\n", VIDEO_FORMAT_SUFFIX);
#endif
            g_read_global_static_var[pop_index].curr_avi_index = 0;
            g_read_global_static_var[pop_index].last_ops_avi_index = -1;
            g_read_global_static_var[pop_index].curr_ops_avi_index = -1;
            g_avi_file_read_write_info.read_write_status = IS_IDLE;

            pthread_mutex_unlock(&global_pop_frame_mutex);
            return 1;
        } 
    }
    else
    {
        *pSize = frame_head.uOverloadLen;
        *pFrametype = frame_head.byFrameType;
		if(frame_head.uReserved >= SMARTP_VIFrame)
		{
			printf("[CYADD] uReserved >= SMARTP_VIFrame the frame is VIFrame!!! \n");
			*VipFrame = 1;
		}

        /* 注意：还有其它种类的I帧,这里暂不处理 */
        if (I_FRAME == *pFrametype)
        {
            *pKeyframe = 1;
        }
        else
        {
            *pKeyframe = 0;
        }

        *pRealTimeSec  = frame_head.u32RealTimeSec;
        *pRealTimeUSec = frame_head.u32RealTimeUSec;
        *pFrametype    = frame_head.byFrameType;

#ifdef SD_PLK_DBG
        static unsigned int last_video_showtime = 0;
        static unsigned int last_audio_showtime = 0;
        unsigned int y,mon,d,h,min,s;

        ULONG_TO_TIME(frame_head.u32RealTimeSec, y, mon, d, h, min, s);

        if (8 == frame_head.byFrameType)
        {
            SD_OPS_DEBUG("%04d-%02d-%02d %02d:%02d:%02d\n", y, mon, d, h, min, s);
#if 1
            SD_OPS_DEBUG("Audio: FrameType=%d, FrameNo=%d, nOverloadLen=%d, ShowTime=%d(sub_audio=%d), u32RealTimeSec=%d, u32RealTimeUSec=%d\n",
                    frame_head.byFrameType,
                    frame_head.uFrameNo,
                    frame_head.uOverloadLen,
                    frame_head.uShowTime,
                    frame_head.uShowTime - last_audio_showtime,
                    frame_head.u32RealTimeSec,
                    frame_head.u32RealTimeUSec);
#else
            SD_OPS_DEBUG("Audio: FrameType=%d, FrameNo=%d, nOverloadLen=%d, u32RealTimeSec=%d\n",
                    frame_head.FrameType,
                    frame_head.FrameNo,
                    frame_head.nOverloadLen,
                    frame_head.u32RealTimeSec);
#endif
                    last_audio_showtime = frame_head.uShowTime;
        }
        else
        {
            SD_OPS_DEBUG("%04d-%02d-%02d %02d:%02d:%02d\n", y, mon, d, h, min, s);
#if 1
            SD_OPS_DEBUG("Video: FrameType=%d, FrameNo=%d, nOverloadLen=%d, ShowTime=%d(sub_video=%d), u32RealTimeSec=%d, u32RealTimeUSec=%d\n",
                    frame_head.byFrameType,
                    frame_head.uFrameNo,
                    frame_head.uOverloadLen,
                    frame_head.uShowTime,
                    frame_head.uShowTime - last_video_showtime,
                    frame_head.u32RealTimeSec,
                    frame_head.u32RealTimeUSec);
#else
            SD_OPS_DEBUG("Video: FrameType=%d, FrameNo=%d, nOverloadLen=%d, u32RealTimeSec=%d\n",
                    frame_head.FrameType,
                    frame_head.FrameNo,
                    frame_head.nOverloadLen,
                    frame_head.u32RealTimeSec);
#endif
                    last_video_showtime = frame_head.uShowTime;
        }
#endif
        memcpy(*p_frame_buffer, &frame_head, sizeof(frame_head));

        *pSize += sizeof(frame_head);
    }
    pthread_mutex_unlock(&global_pop_frame_mutex);
    
    return errorcode;
}

/**********************************************************************************************/

/*@breif CheckDiskStatus 从标记文件里面获取SD卡状态(该状态在系统启动的时候传入)
 *@return 返回 1 表示SD卡正常
 *@      返回 0 表示SD卡异常或者无SD卡
 *@note  状态位已经放在全局变量里面
 */
static int CheckDiskStatus(void)
{
    if (access(DISK_STATUS_FLAG, F_OK))
    {
        SD_OPS_ERROR("Disk status file %s not exist!\n", DISK_STATUS_FLAG);
        return -1;
    }

    FILE *fp = NULL;
    fp = fopen(DISK_STATUS_FLAG, "r");
    if (!fp)
    {
        SD_OPS_ERROR("Fail to fopen %s\n", DISK_STATUS_FLAG);
        return -1;
    }

    char buff[4] = {0};
    if (NULL == fgets(buff, sizeof(buff), fp))
    {
        SD_OPS_ERROR("Fail to fgets %s\n", DISK_STATUS_FLAG);
        SAFE_FCLOSE(fp);
        return -1;
    }
    SAFE_FCLOSE(fp);

    int disk_status = atoi(buff);

    switch(disk_status)
    {
        case SdCardBootCheck_NoNode: 
        {
            g_sd_check_status = NoSdCard;             
            break;
        }
        case SdCardBootCheck_MountFail: 
        {
            g_sd_check_status = SdCardMountErr; 
            WriteLog(SD_STATUS_CFG, MY_DESCRIBE(eSdCardTotalInfoStatus, SdCardBootCheck_MountFail));
            break;
        }
        case SdCardBootCheck_AddPartFail: 
        {
            g_sd_check_status = SdCardAddPartErr; 
            WriteLog(SD_STATUS_CFG, MY_DESCRIBE(eSdCardTotalInfoStatus, SdCardBootCheck_AddPartFail));
            break;
        }
        case SdCardBootCheck_DiskIsNormal:
        {
            g_sd_check_status = SdCardReadWrite; 
            break;
        }
        case SdCardRunCheck_FatfsIsError: 
        {
            g_sd_check_status = SdCardFatfsErr; 
			g_sd_runing_status = SdCardRunCheck_FatfsIsError;
            WriteLog(SD_STATUS_CFG, MY_DESCRIBE(eSdCardTotalInfoStatus, SdCardRunCheck_FatfsIsError));
            break;
        }

        case SdCardBootCheck_DiskExp: 
        {
            g_sd_check_status = SdCardErr; 
            WriteLog(SD_STATUS_CFG, MY_DESCRIBE(eSdCardTotalInfoStatus, SdCardBootCheck_DiskExp));
            break;
        }

        case SdCardRunCheck_DiskIsError:
        {
            g_sd_check_status = SdCardErr; 
			g_sd_runing_status = SdCardRunCheck_DiskIsError;
            WriteLog(SD_STATUS_CFG, MY_DESCRIBE(eSdCardTotalInfoStatus, SdCardRunCheck_DiskIsError));
            break;
        }

        case SdCardRunCheck_DiskReadOnly:
        {
            g_sd_check_status = SdCardReadOnly; 
            WriteLog(SD_STATUS_CFG, MY_DESCRIBE(eSdCardTotalInfoStatus, SdCardRunCheck_DiskReadOnly));
            break;
        }

        case SdCardBootCheck_ReadWriteFail:
        {
            g_sd_check_status = SdCardReadOnly; 
            WriteLog(SD_STATUS_CFG, MY_DESCRIBE(eSdCardTotalInfoStatus, SdCardBootCheck_ReadWriteFail));
            break;
        }

		case SdCardFormatStatus_FormatFail:
        {
            g_sd_check_status = SdCardErr;
            g_sd_runing_status = SdCardRunCheck_DiskIsError;
            WriteLog(SD_STATUS_CFG, MY_DESCRIBE(eSdCardTotalInfoStatus, SdCardFormatStatus_FormatFail));
            break;
        }
    }

    if (SdCardBootCheck_DiskIsNormal != disk_status)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

/**
********************************************************************************
 * @函数功能: 处理监测进程MonitorProcess发现的一些SD卡异常，主应用主动做一些动作尝试恢复设备当前问题
 * @作者    : HL.LIU
 * @param[in] *needReboot 返回是否需要重启
 * @返回值  : 
 * @其他    : 
********************************************************************************
*/
static int respondSDDmesgAbnormal(int *needReboot)
{
    int ret = 0;
    char result[128] = {0};
    static time_t systemUptime = 0;

    *needReboot = 0;
    if((SdCardRunCheck_DiskIsError == g_sd_runing_status) || (SdCardRunCheck_FatfsIsError == g_sd_runing_status))
    {
        //获取系统已运行的时间
        ret = ShellCmdOut(GET_SYSTEM_UPTIME_COMMAND, result, sizeof(result));
        if(0 == ret)
        {
            systemUptime = atoi(result);
        }
        else
        {
            SD_OPS_ERROR("Run command : %s fail\n", GET_SYSTEM_UPTIME_COMMAND);
        }

        // 判断是否达到启动的时长，防止反复重启
        if(systemUptime >= SD_ABNORMAL_REBOOT_INTERVAL)
        {
            WriteLog(SD_LOG_FILE, MY_DESCRIBE(eSdCardTotalInfoStatus, g_sd_runing_status));
            //根据具体内核错误类型然后给SD_ACTION_AT_START_FILE传值，在下次启动时做卡操作
            switch(g_sd_runing_status)
            {
                case SdCardRunCheck_DiskIsError: 
                    *needReboot = 1;
                    break;
                case SdCardRunCheck_FatfsIsError: 
                    // 下次启动时需要在checkSDCard里强制对卡做修复
                    SetSDActionAtStart(SD_ACTION_AT_START_REPAIR);
                    *needReboot = 1;
                    break;
                default:
                    break;
            }
            
        }
    }

    return 0;
}

/* 检测磁盘状态线程，用于监测Disk是否出现损坏 */
static void* CheckDiskStatusThread(void *lparam)
{
    PRT_TID();
    pthread_detach(pthread_self());

    int speedCount = 0;
	int needReboot = 0;
	pthread_t pThreadID;

    while(1)
    {
        CheckDiskStatus();

		if(!needReboot)
        {
            respondSDDmesgAbnormal(&needReboot);
        }

        if(needReboot)
        {
            pthread_create(&pThreadID, NULL, DelayReboot, NULL);
        }

        sleep(30);

        if(((SdCardErr == g_sd_check_status) 
          || (SdCardReadOnly == g_sd_check_status)
          || (SdCardFatfsErr == g_sd_check_status) 
          || (SdCardMountErr == g_sd_check_status)
          || (SdCardAddPartErr == g_sd_check_status)) 
          && (speedCount < 3))
        {
            SD_OPS_DEBUG("Detect Sd Card is Error!\n");
            speedCount++;
        }
    }

    return NULL;
}

static void* DiskFileDeleteThreadProcess(void *lparam)
{
    PRT_TID();
    pthread_detach(pthread_self());

    char video_dir[MAX_FILE_PATH_STR] = {0};
    char delete_file[MAX_FILE_PATH_STR] = {0};
    unsigned long dwAvailableSize = 0;

    snprintf(video_dir, MAX_FILE_PATH_STR, "%s/video", SD_MOUNT_POINT);

    int iRet = 0;

    for (;;)
    {
        iRet = sem_wait(&g_semDeleteFileThread);
        if(0 != iRet)
        {
            usleep(100000);
            continue;
        }

        /* 循环删除最旧的文件，直至大小足够剩余空间 */
        for (;;)
        {
            dwAvailableSize = 0;
            EDVR_HD_GetMountPointSpace(g_diskinfo.Partitions[0].Mount_point, NULL, NULL, &dwAvailableSize);
            if (dwAvailableSize < DISK_MIN_FREE_SIZE_MBYTES)
            {
                memset(delete_file, 0, sizeof(delete_file));
                if (SD_GetOldestFile(video_dir, VIDEO_FORMAT_SUFFIX, delete_file, MAX_FILE_PATH_STR))
                {
                    SD_OPS_ERROR("Fail to get oldest file in %s!\n", video_dir);
                }
                else
                {
                    SD_OPS_DEBUG("disk available size is %d MB, less than mini limit size %d MB, delete file %s!\n", 
                            dwAvailableSize, DISK_MIN_FREE_SIZE_MBYTES, delete_file);
                    /* 如果文件存在，则会删除文件，否则会删除空目录 */
                    SAFE_REMOVE(delete_file);
                }
            }
            else
            {
                break;
            }
        }
    }

    return NULL;
}

static void* CheckImageUsedSpaceThreadProcess(void* lparam)
{
    PRT_TID();
    pthread_detach(pthread_self());

    int64_t dir_size_bytes = 0;
    char image_dir[MAX_FILE_PATH_STR] = {0};
    char delete_file[MAX_FILE_PATH_STR] = {0};

    snprintf(image_dir, MAX_FILE_PATH_STR, "%s/picture/", SD_MOUNT_POINT);

    for (;;)
    {
        sem_wait(&g_semPicSpaceCheckThread);

        while ((dir_size_bytes=GET_DIR_SIZE(image_dir)) / MEGA_BYTE_MB > MAX_PICTURE_SIZE_MB)
        {
            memset(delete_file, 0, sizeof(delete_file));
            if (SD_GetOldestFile(image_dir, IMAGE_FORMAT_SUFFIX, delete_file, MAX_FILE_PATH_STR))
            {
                SD_OPS_ERROR("Fail to get oldest file in %s!\n", image_dir);
            }
            else
            {
                SD_OPS_DEBUG("image used space is %lld MB over max limit size %d MB delete file %s!\n", 
                        dir_size_bytes/MEGA_BYTE_MB, MAX_PICTURE_SIZE_MB, delete_file);
                /* 如果文件存在，则会删除文件，否则会删除空目录 */
                SAFE_REMOVE(delete_file);
            }
        }
    }

    return NULL;
}

static void* TimerCounterThreadProcess(void* lparam)
{
    PRT_TID();
    pthread_detach(pthread_self());

    for (;;)
    {
        TimerMilliseconds(1000);
        g_second_times ++;
    }

    return NULL;
}

/*@breif GetRecordListByTime 根据时间段获取当前录像文件列表
 *@param [IN] start_time 开始时间
 *@param [IN] stop_time  结束时间
 *@param [IN] record_type 筛选的录像类型(当前无效)
 *@param [OUT] p_record_search_list 获取到的文件列表(函数里面已经动态分配内存，后续需要释放)
 *@param [OUT] p_count 获取到的文件个数
 *@return 返回  0 表示获取成功
 *@       返回 -1 表示获取失败
 *@note   (1) 文件列表采用线性的动态数组分配的方式，函数使用完毕时候，需要释放空间，避免内存泄漏；
 *@       (2) 搜索时间限定不能超过24H；
 *@       (3) 搜索片段时间一定是按照start_time排完序之后的内容；
 */
static int GetRecordListByTime(unsigned int start_time, 
                                         unsigned int stop_time, 
                                         int record_type, 
                                         pRecordSearchList *p_record_search_list,
                                         int *p_count)
{
    if (!p_record_search_list || !p_count)
    {
       SD_OPS_ERROR("NULL POINTER EXCEPTION!\n");
       return -1;
    }

    *p_count = 0;

    if (stop_time < start_time)
    {
        SD_OPS_ERROR("Seek time format error!\n");
        return -1;
    }

    /* 搜索时间限定不能超过24H */
    if (stop_time - start_time > 24*60*60)
    {
        SD_OPS_ERROR("Find time segment too long! not more than 24h!\n");
        return -1;
    }
#if 0
    int read_curr_avi_num = 0;

    pthread_mutex_lock(&g_get_files_list_set_mutex);
    read_curr_avi_num = g_read_curr_avi_num;
    pthread_mutex_unlock(&g_get_files_list_set_mutex);

    Avi_file_info *p_after_sort_file_set   = NULL;
    p_after_sort_file_set = (Avi_file_info *)calloc(g_read_curr_avi_num, sizeof(Avi_file_info));
    if (NULL == p_after_sort_file_set)
    {
        SD_OPS_ERROR("Fail to calloc!\n");
        return -1;
    }

    /* 拷贝到临时结构体 */
    pthread_mutex_lock(&g_get_files_list_set_mutex);
    memcpy(p_after_sort_file_set, g_after_sort_file_set, read_curr_avi_num*sizeof(Avi_file_info));
    pthread_mutex_unlock(&g_get_files_list_set_mutex);

    /* 注意: 此处的g_after_sort_file_set里面一定是按照start_time排完序之后的内容，否则会出错 */
    start_time = start_time<p_after_sort_file_set[0].start_time?p_after_sort_file_set[0].start_time:start_time;
    stop_time   = stop_time>p_after_sort_file_set[read_curr_avi_num-1].stop_time?p_after_sort_file_set[read_curr_avi_num-1].stop_time:stop_time;

    int i = 0, j = 0;
    int m = 0, n = 0;
    unsigned int last_rec_time = -1;
    char file_name[32] = {0};
    char dest_path[MAX_PATH_STR_NUM] = {0};

    unsigned int curr_record_times[MAX_FILE_NUM] = {0};

    for (i=0; i<read_curr_avi_num; i++)
    {
        curr_record_times[i] = p_after_sort_file_set[i].start_time;

#ifdef SD_DBG_INF
        SD_DBG("curr_record_times[%d] = %d stop_time: %d\n", 
                i, curr_record_times[i], p_after_sort_file_set[i].stop_time);
#endif
    }

    m = BinareySearchEx(curr_record_times, read_curr_avi_num, start_time);

    if (m >= 1)
    {
        if (p_after_sort_file_set[m-1].stop_time >= start_time)
        {
            m -= 1;
        }
    }
    m = m < 0 ? 0 : m;

    /* 避免出现搜索片段出现2016年的问题 */
    if (p_after_sort_file_set[m].stop_time < start_time)
    {
        SD_DBG("p_after_sort_file_set[%d].stop_time %d < start_time %d jump it!\n", 
                m, p_after_sort_file_set[m].stop_time, start_time);
        m ++;
    }

    n = BinareySearchEx(curr_record_times, read_curr_avi_num, stop_time);

    n = n < 0 ? 0 : n;

    *p_count = n - m + 1;

    if (*p_count <= 0)
    {
        SD_ERR("Not found record file in %d to %d\n", start_time, stop_time);
        SAFE_FREE(p_after_sort_file_set);
        return -1;
    }

    /* 分配一组连续的内存，以防后续需要用qsort进行排序 */
    p_record_search_list[0] = (RecordSearchList *)calloc(sizeof(RecordSearchList), *p_count);
    if (NULL == p_record_search_list[0])
    {
        SD_OPS_ERROR("Fail to calloc!\n");
        SAFE_FREE(p_after_sort_file_set);
        return -1;
    }
    
    for (i=1; i<(*p_count); i++)
    {
        p_record_search_list[i] = p_record_search_list[i-1] + 1;
    }

    for (i=m,j=0; i<=n; i++, j++)
    {
#if 0
        /* 暂时不考虑类型过滤的情况 */
        if (record_type != p_after_sort_file_set[i].record_type)
        {
            continue;
        }
#endif

        GetFileNameByStartTimestamp(p_after_sort_file_set[i].record_type, 
                                      p_after_sort_file_set[i].start_time, 
                                      VIDEO_FORMAT_SUFFIX, 
                                      file_name);

        /* 如果文件不存在，则需要跳过去(分配的内存还在) */
        snprintf(dest_path, sizeof(dest_path), "%s/video/%s", SD_MOUNT_POINT, file_name);
        if (access(dest_path, F_OK))
        {
            SD_SYS("%s not exist! jump it!\n", dest_path);
            (*p_count) --;
            j --;
            continue;
        }

        SD_DBG("index = %d find record file is %s\n", i, dest_path);

        /* 进行赋值 */
        strncpy(p_record_search_list[j]->file_name, file_name, sizeof(file_name) - 1);
        p_record_search_list[j]->start_time  = p_after_sort_file_set[i].start_time;
        p_record_search_list[j]->stop_time   = p_after_sort_file_set[i].stop_time;
        p_record_search_list[j]->file_size   = GetFileSize(dest_path);
        p_record_search_list[j]->record_type = (unsigned int)ToNetRecordTypeMap((eRecordType)p_after_sort_file_set[i].record_type);
    }

    SAFE_FREE(p_after_sort_file_set);
#else
    return -1;
#endif
    
    return 0;
}

/*@breif GetRecordFileLastFramePos 获取录像文件最后一帧数据的位置
 *@param [IN] record_file 录像文件
 *@param [OUT] p_last_frame_pos 最后一帧数据的位置
 *@return 返回  0 表示获取成功
 *@       返回 -1 表示获取失败
 */
static int GetRecordFileLastFramePos(const char *record_file, 
                                                   unsigned int *p_last_frame_pos)
{
    if (!record_file || !p_last_frame_pos)
    {
        SD_OPS_ERROR("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    if (access(record_file, F_OK))
    {
        SD_OPS_ERROR("record file %s not exist!\n", record_file);
        return -1;
    }

#if 0

    SD_DBG("record_file is %s\n", record_file);

    /* 帧数据起始位置 */
    int fd = -1;
    int read_count = 0;
    SD_OPS_RecordTypeInfo record_type_info = {0};

    fd = open(record_file, O_RDONLY, S_IRWXU);
    if (fd < 0)
    {
        SD_SYS("Fail to open %s!\n", record_file);
        return -1;
    }

    /* 直接定位到文件倒数64KB的位置，获取32KB的SD_OPS_RecordTypeInfo类型 */
    lseek(fd,  -(64*1024), SEEK_END);

    /* 读取32KB的SD_OPS_RecordTypeInfo信息 */
    read_count = read(fd, &record_type_info, sizeof(SD_OPS_RecordTypeInfo));
    if (read_count != sizeof(SD_OPS_RecordTypeInfo))
    {
        SD_SYS("Fail to read record_type_info [%d != %d]!\n", read_count, sizeof(SD_OPS_RecordTypeInfo));
        goto EXIT_READ;
    }

    /* 校验信息头 */
    if (record_type_info.uMagicNumber != 0xDEADBEEF)
    {
        /* 文件指针还原到前一帧数据的尾部 */
        SD_OPS_ERROR("record_type_info start code [%#x != 0xDEADBEEF]!\n", record_type_info.uMagicNumber);
        goto EXIT_READ;
    }

    /* 获取当前最后一帧数据的结束地址(文件头+帧数据内容) */
    *p_last_frame_pos = record_type_info.uRealFrameLenth + sizeof(MPEG4FILEHEADER);
    SAFE_CLOSE(fd);
    return 0;

EXIT_READ:

    SAFE_CLOSE(fd);
	return -1;
#else
    return -1;
#endif
}

/*@breif GetRecordFileSeekInfo 获取录像文件Seek的信息
 *@param [IN] record_file 录像文件
 *@param [IN] seek_time seek的时间
 *@param [OUT] p_seek_time_iframe_time 当前seek_time的I帧的时间戳(单位：秒)
 *@param [OUT] p_seek_to_begin_size_bytes 从seek_time到帧头持续的录像数据(单位：bytes)
 *@param [OUT] p_seek_to_end_size_bytes 从seek_time到结束帧持续的录像数据(单位：bytes)
 *@return 返回  0 表示获取成功
 *@       返回 -1 表示获取失败
 */
static int GetRecordFileSeekInfo(const char *record_file, 
                                            unsigned int seek_time, 
                                            int b_need_correct_time,
                                            unsigned int *p_seek_time_iframe_time,
                                            unsigned int *p_seek_to_begin_size_bytes,
                                            unsigned int *p_seek_to_end_size_bytes)
{
    if (!record_file 
     || !p_seek_time_iframe_time
     || !p_seek_to_begin_size_bytes
     || !p_seek_to_end_size_bytes)
    {
        SD_OPS_ERROR("NULL POINTER EXCEPTION!\n");
        return -1;
    }

#if 0

    int ret = -1;
    IFrameInfo iframe_info = {0};

    ret = GetRecordFileSeekTimeFrameInfo(record_file, seek_time, b_need_correct_time, &iframe_info);
    if (ret)
    {
        SD_OPS_ERROR("Fail to called GetRecordFileSeekTimeFrameInfo: %s!\n", record_file);
        *p_seek_time_iframe_time = 0;
    }
    else
    {
        *p_seek_time_iframe_time = iframe_info.CurIFrameTime;
    }

#if 1
    int y, m, d, h, min, s;
    ULONG_TO_TIME(iframe_info.CurIFrameTime, y, m, d, h, min, s);
    SD_DBG("found I frame timestamp %d ---> %04d-%02d-%02d %02d:%02d:%02d\n", 
             iframe_info.CurIFrameTime, y, m, d, h, min, s);
#endif

#if 0
    最新录像文件组成格式：
    |---------------文件头(19字节)---------------|
    |--------------------帧头--------------------|
    |--------------------数据--------------------|
    ... ...
    ... ...
    |--------------------帧头--------------------|
    |--------------------数据--------------------|
    ... ...
    ... ...
    |-------------------脏数据-------------------|
    |---------SD_OPS_RecordTypeInfo(32K)---------|
    |--------------I帧索引集(32K)----------------|
#endif

    int fd = -1;
    int read_count = 0;
    SD_OPS_RecordTypeInfo record_type_info = {0};

    fd = open(record_file, O_RDONLY, S_IRWXU);
    if (fd < 0)
    {
        SD_SYS("Fail to open %s!\n", record_file);
        return -1;
    }

    /* 直接定位到文件倒数64KB的位置，获取32KB的SD_OPS_RecordTypeInfo类型 */
    lseek(fd,  -(64*1024), SEEK_END);

    /* 读取32KB的SD_OPS_RecordTypeInfo信息 */
    read_count = read(fd, &record_type_info, sizeof(SD_OPS_RecordTypeInfo));
    if (read_count != sizeof(SD_OPS_RecordTypeInfo))
    {
        SD_SYS("Fail to read record_type_info [%d != %d]!\n", read_count, sizeof(SD_OPS_RecordTypeInfo));
        goto EXIT_READ;
    }

    /* 校验信息头 */
    if (record_type_info.uMagicNumber != 0xDEADBEEF)
    {
        /* 文件指针还原到前一帧数据的尾部 */
        SD_OPS_ERROR("record_type_info start code [%#x != 0xDEADBEEF]!\n", record_type_info.uMagicNumber);
        goto EXIT_READ;
    }
    SAFE_CLOSE(fd);

    /* 定位时间到开始I帧的文件大小(帧头+帧数据) = 当前定位的I帧位置 - 开始帧的开始位置 */
    *p_seek_to_begin_size_bytes = iframe_info.CurrFramePos - sizeof(MPEG4FILEHEADER);
    *p_seek_to_begin_size_bytes = *p_seek_to_begin_size_bytes < 0 ? 0 : *p_seek_to_begin_size_bytes;
    *p_seek_to_begin_size_bytes = *p_seek_to_begin_size_bytes 
                                   > record_type_info.uRealFrameLenth ? record_type_info.uRealFrameLenth : *p_seek_to_begin_size_bytes;

    /* 定位时间到结束帧(非I帧)的文件大小(帧头+帧数据) =        最后一个帧的位置(包括尾部的P帧)  - 开始I帧的位置 */
    *p_seek_to_end_size_bytes = record_type_info.uRealFrameLenth + sizeof(MPEG4FILEHEADER) - iframe_info.CurrFramePos;
    *p_seek_to_end_size_bytes = *p_seek_to_end_size_bytes < 0 ? 0 : *p_seek_to_end_size_bytes;
    *p_seek_to_end_size_bytes = *p_seek_to_end_size_bytes 
                                 > record_type_info.uRealFrameLenth ? record_type_info.uRealFrameLenth : *p_seek_to_end_size_bytes;

    SD_DBG("iframe_info.CurrFramePos = %d\n", iframe_info.CurrFramePos);
    SD_OPS_DEBUG("iframe_info.CurIFrameTime = %d\n", iframe_info.CurIFrameTime);
    SD_DBG("record_type_info.uRealFrameLenth = %d\n", record_type_info.uRealFrameLenth);
    SD_OPS_DEBUG("*p_seek_to_begin_size_bytes = %d\n", *p_seek_to_begin_size_bytes);
    SD_OPS_DEBUG("*p_seek_to_end_size_bytes = %d\n",   *p_seek_to_end_size_bytes);

    return 0;

EXIT_READ:

    *p_seek_to_begin_size_bytes = 0;
    *p_seek_to_end_size_bytes = 0;

    SAFE_CLOSE(fd);
    return -1;
#else
    return -1;
#endif
}

//修复tmp录像（大于REPAIRED_VIDEO_MIN_SIZE_BYTES，且包含REPAIRED_VIDEO_MIN_IFRAME_NUM个以上的I帧才去修复）
static int RepairTmpPrvFile()
{
    int fd = 0;
    int nReadLen = 0;
    int nFileSize = 0;
    int y=0,mon=0,d=0,h=0,min=0,s=0;
    off_t nSeekCur = 0, nSeekTail = 0;
    unsigned int uFirstFrameTime = 0, uLastFrameTime = 0;
    char strDestPath[64]  = {0};
    char strTmpFile[64] = {0};
    char strPrvFile[128] = {0};
    char strBeginTime[32] = {0};
    char strStopTime[32] = {0};
    char strFileDate[32] = {0};
    FILE *fpDestPath = NULL;
    int nIFrameNum = 0, nVideoFrameNum = 0;
    MPEG4FILEHEADER PrvHeader;
    memset(&PrvHeader, 0, sizeof(MPEG4FILEHEADER));
    
    H264DATAPACKETHEADER FrameHead;
    
    SD_OPS_RecordTypeInfo TypeInfo;
    memset(&TypeInfo, 0, sizeof(SD_OPS_RecordTypeInfo));
    
    IFrameInfo arrIFrameInfo[MAX_AVI_FILE_I_FRAME_NUM];
    memset(&arrIFrameInfo, 0, sizeof(IFrameInfo)*MAX_AVI_FILE_I_FRAME_NUM);

    SD_OPS_DEBUG("Record repair start\n");
    //判断.tmp_file_info文件存在
    snprintf(strDestPath, sizeof(strDestPath), "%s/%s", SD_MOUNT_POINT, TMP_FRAME_INFO_FILE);
    if (0 != access(strDestPath, F_OK))
    {
        SD_OPS_ERROR("File %s does not exist!\n",strDestPath);
        goto RemoveTmpAndRetrun;
    }

    //获取.tmp_file_info保存的文件名
    fpDestPath = fopen(strDestPath, "r");
    if (fpDestPath)
    {
        fread(strTmpFile, sizeof(strTmpFile), 1, fpDestPath);
        fclose(fpDestPath);
        fpDestPath = NULL;
        strTmpFile[strlen(strTmpFile)] = '\0';
    }
    else
    {
        SD_OPS_ERROR("Open file %s error!\n",strDestPath);
        goto RemoveTmpAndRetrun;
    }
    
    //判断tmp文件存在
    if (0 != access(strTmpFile, F_OK))
    {
        SD_OPS_ERROR("File %s does not exist!\n",strTmpFile);
        goto RemoveTmpAndRetrun;
    }
    
    nFileSize = GetFileSize(strTmpFile);
    if (nFileSize < REPAIRED_VIDEO_MIN_SIZE_BYTES)
    {
        SD_OPS_ERROR("filename %s filesize %d Bytes < %d!\n", strTmpFile, nFileSize, REPAIRED_VIDEO_MIN_SIZE_BYTES);
        goto RemoveTmpAndRetrun;
    }
    SD_OPS_DEBUG("Get tmp record filename %s, filesize %d Bytes\n", strTmpFile, nFileSize);
    //开始修复
    time_t NowTime = 0;
    time(&NowTime);

    fd = open(strTmpFile, O_RDWR);
    if(fd <= 0)
    {
        SD_OPS_ERROR("Open file %s error!\n",strTmpFile);
        goto RemoveTmpAndRetrun;
    }
    
    //读文件头
    nSeekTail = lseek (fd, 0, SEEK_END);
    nSeekCur = lseek (fd, 0, SEEK_SET);
    if((nSeekTail - nSeekCur) >= sizeof(MPEG4FILEHEADER))
    {
        nReadLen = read (fd, &PrvHeader, sizeof(MPEG4FILEHEADER));
        if(nReadLen != sizeof(MPEG4FILEHEADER))
        {
            SD_OPS_ERROR("Read prv header error!\n");
            goto RepairFail;
        }

        if(PrvHeader.FileStartCode != FILE_START_CODE)
        {
            SD_OPS_ERROR("PrvHeader.FileStartCode[%#X] != %#X \n", PrvHeader.FileStartCode, FILE_START_CODE);
            goto RepairFail;
        }
        //printMPEG4FileHeader(&PrvHeader);
    }
    else
    {
        SD_OPS_ERROR("Read PrvHeader file end! %ld < %lu\n", (nSeekTail - nSeekCur), sizeof(MPEG4FILEHEADER));
        goto RepairFail;
    }

    //读帧
    while(1)
    {
        memset(&FrameHead, 0, sizeof(H264DATAPACKETHEADER));
        nSeekCur = lseek (fd, 0, SEEK_CUR);
        if((nSeekTail - nSeekCur) >= sizeof(H264DATAPACKETHEADER))
        {
            nReadLen = read (fd, &FrameHead, sizeof(H264DATAPACKETHEADER));
            if(nReadLen != sizeof(H264DATAPACKETHEADER))
            {
                SD_OPS_ERROR("Read prv header error!\n");
                goto RepairFail;
            }
        }
        else
        {
            SD_OPS_ERROR("Read PRIVATEFRAMEHEADER file end! %ld < %lu\n",(nSeekTail - nSeekCur),sizeof(H264DATAPACKETHEADER));
            ftruncate(fd,nSeekCur);
            break;
        }
        
        if(FrameHead.uDataPacketStartCode != FRAME_START_CODE)
        {
            SD_OPS_DEBUG("FrameHead.DataPacketStartCode[%#X] != %#X\n", FrameHead.uDataPacketStartCode, FRAME_START_CODE);
            SD_OPS_DEBUG("nIFrameNum[%d] / nVideoFrameNum[%d]\n",nIFrameNum,nVideoFrameNum);
            lseek (fd, nSeekCur, SEEK_SET);
            ftruncate(fd,nSeekCur);
            break;
        }

        if((nSeekTail - nSeekCur >= sizeof(H264DATAPACKETHEADER) + FrameHead.uOverloadLen) /*&& FrameHead.u32RealTimeSec <= NowTime*/)
        {
            lseek (fd, FrameHead.uOverloadLen, SEEK_CUR);
            
            if(!uFirstFrameTime){
                uFirstFrameTime = FrameHead.u32RealTimeSec;
            }
            uLastFrameTime = FrameHead.u32RealTimeSec;
            
            if (I_FRAME == FrameHead.byFrameType)
            {
                arrIFrameInfo[nIFrameNum].CurIFrameTime = FrameHead.u32RealTimeSec;
                arrIFrameInfo[nIFrameNum].ShowTime      = FrameHead.uShowTime;
                arrIFrameInfo[nIFrameNum].IFrameNum     = nVideoFrameNum;
                arrIFrameInfo[nIFrameNum].CurrFramePos  = nSeekCur;
                nIFrameNum ++;
            }
            
            if (AUDIO_FRAME != FrameHead.byFrameType)
            {
                nVideoFrameNum ++;
            }
        }
        else
        {
            SD_OPS_DEBUG("read frame file end! %ld < %lu || Time %u > %ld\n",(nSeekTail - nSeekCur), (sizeof(H264DATAPACKETHEADER) + FrameHead.uOverloadLen), FrameHead.u32RealTimeSec, NowTime);
            lseek (fd, nSeekCur, SEEK_SET);
            ftruncate(fd,nSeekCur);
            break;
        }
    }

    //AOV模式下文件可能只有一个I帧，故此限制可以去掉。
    //判断I帧个数大于30个
    if(nIFrameNum < REPAIRED_VIDEO_MIN_IFRAME_NUM)
    {
        SD_OPS_ERROR("nIFrameNum[%d] < %d!\n", nIFrameNum, REPAIRED_VIDEO_MIN_IFRAME_NUM);
        goto RepairFail;
    }

    //修复录像类型
    nSeekCur = lseek (fd, 0, SEEK_CUR);
    TypeInfo.uMagicNumber = SD_OPS_MAGIC_NUM;
    TypeInfo.uFileBeginTime = uFirstFrameTime;
    TypeInfo.uFileEndTime = uLastFrameTime;
    TypeInfo.uNum = 1;
    TypeInfo.arrayTypeInfo[0].uTimeBegin = uFirstFrameTime;
    TypeInfo.arrayTypeInfo[0].uTimeEnd = uLastFrameTime;
    TypeInfo.arrayTypeInfo[0].uRecordType = eNormalRecord;
    TypeInfo.uRealFrameLenth = nSeekCur - sizeof(MPEG4FILEHEADER);
    write (fd, &TypeInfo, sizeof(SD_OPS_RecordTypeInfo));
    //printRecordTypeInfo(&TypeInfo);

    //修复i帧统计信息
    write (fd, &arrIFrameInfo, sizeof(IFrameInfo)*MAX_AVI_FILE_I_FRAME_NUM);
    //printIFrameInfo((IFrameInfo*)&arrIFrameInfo, (int)MAX_AVI_FILE_I_FRAME_NUM);

    close(fd);
    fd = 0;

    ULONG_TO_TIME((TypeInfo.uFileBeginTime), y, mon, d, h, min, s);
    snprintf(strBeginTime, sizeof(strBeginTime), "%04d%02d%02d%02d%02d%02d", y, mon, d, h, min, s);
    ULONG_TO_TIME((TypeInfo.uFileEndTime), y, mon, d, h, min, s);
    snprintf(strStopTime, sizeof(strStopTime), "%04d%02d%02d%02d%02d%02d", y, mon, d, h, min, s);
    snprintf(strFileDate, sizeof(strFileDate), "%04d%02d%02d", y, mon, d);

    const char* ai_part = strstr(strTmpFile, "_AI");
    const char* motion_part = strstr(strTmpFile, "_MOTION");
    
    if (ai_part != NULL) 
    {
        // 如果包含 "_AI"，生成新的文件名
        snprintf(strPrvFile, sizeof(strPrvFile), "%s/video/%s/AI_%s-%s%s", SD_MOUNT_POINT, strFileDate, strBeginTime, strStopTime, EnumTypeMapStr(E_FILE_MAP, E_PRV_FILE));
    }
    else if (motion_part != NULL) 
    {
        // 如果包含 "_MOTION"，生成新的文件名
        snprintf(strPrvFile, sizeof(strPrvFile), "%s/video/%s/MOTION_%s-%s%s", SD_MOUNT_POINT, strFileDate, strBeginTime, strStopTime, EnumTypeMapStr(E_FILE_MAP, E_PRV_FILE));
    }
    else 
    {
        // 如果都不包含，生成默认的文件名
        snprintf(strPrvFile, sizeof(strPrvFile), "%s/video/%s/%s-%s%s", SD_MOUNT_POINT, strFileDate, strBeginTime, strStopTime, EnumTypeMapStr(E_FILE_MAP, E_PRV_FILE));
    }

    // 使用rename函数重命名文件
    if (rename(strTmpFile, strPrvFile) == 0) 
    {
        SD_OPS_DEBUG("Record repair succ, %s -> %s\n", strTmpFile, strPrvFile);
    }
    else 
    {
        SD_OPS_DEBUG("Record repair succ, %s -> %s\n", strTmpFile, strPrvFile);
        perror("文件重命名失败");
    }

    return 0;
RepairFail:
    if(fd) {
        close(fd);
    }

RemoveTmpAndRetrun:
    if (strDestPath[0] != '\0') {
        SAFE_UNLINK(strDestPath);
    }
    if (strTmpFile[0] != '\0') {
        SAFE_UNLINK(strTmpFile);
    }

    SD_OPS_ERROR("Record repair fail\n");
    return -1;
}

/* ----------------------------------------------------------------------------------------------- */

/* SD卡初始化（暂时不支持SD卡的热拔插） */
extern int Disk_Init(void)
{
    SD_OPS_DEBUG("Disk_Init Start ...\n");

    int ret = 0;
    if (g_bCheckSdStatusExit == 0)
    {
        SD_OPS_DEBUG("Disk is aready init...\n");
        return 0;
    }
    SD_OPS_DEBUG("Disk_Init Start ...\n");
    if (1 != g_bCheckSdStatusExit)
    {
    	if (1 != CheckDiskStatus())
    	{
			//关闭SD卡电源
			ZWSys_DownSdPower();
        	SD_OPS_ERROR("Disk check fail! no SdCard or SdCard error DownSdPower\n");
            return -1;
        }
    }

    int i = 0;
    char remote_dir[MAX_NAME_STR] = {0};
    char file_path[MAX_PATH_STR_NUM] = {0};

    SD_OPS_DEBUG("MAX_SIZE_PER_FRAME=%d\n", MAX_SIZE_PER_FRAME);

    for (i = 0; i < MAX_OPERATE_HANDLE; i++)
    {
        if (NULL == g_pFilesListSet[i])
        {
            g_pFilesListSet[i] = (stuFileInfo *)calloc(MAX_FILE_NUM, sizeof(stuFileInfo));
        }
    }

    pthread_mutex_lock(&global_mutex);

    sem_init(&g_semWakeThread, 0, 0);
    sem_init(&g_semPicSpaceCheckThread, 0, 0);

    g_sd_card_has_inited = 0;
    g_b_need_creat_avi_file = 1;

    /* 初始化一些和SD卡相关文件系统 */
    ret = InitFileSystem();
    if ((0 != ret) && (1 != ret))
    {
        SD_OPS_ERROR("System init fail!\n");   
        goto INIT_ERR;
    }

    if (1 == ret)
    {
        SD_OPS_DEBUG("Sd Card not detect!\n");   
        g_sd_check_status = NoSdCard;
        mysystem(SD_NO_CMD); 
        goto INIT_ERR;
    }
    else
    {
        if (g_diskinfo.RW_Status)
        {
            g_sd_check_status = SdCardReadWrite;        
        }
        else
        {
            g_sd_check_status = SdCardReadOnly;
        }
        mysystem(SD_NORMAL_CMD);    
    }

	/* 初始化write缓冲区 */
    if (InitAviWriteBuf() < 0)
    {
        goto INIT_ERR;
    }

    g_picture_space_kb =   MAX_PICTURE_SIZE_MB << 10;
    g_video_space_kb = (g_diskinfo.Partitions[0].Total_partitions_space - MAX_PICTURE_SIZE_MB) << 10;

    g_sd_card_has_inited = 1;

    snprintf(remote_dir, sizeof(remote_dir), "%s/video", g_diskinfo.Partitions[0].Mount_point);
    ret = CreateDirent(remote_dir);
    if (0 != ret)
    {
        SD_OPS_ERROR("Fail to create %s dir!\n", remote_dir);
        goto INIT_ERR;
    }

    snprintf(remote_dir, sizeof(remote_dir), "%s/picture", g_diskinfo.Partitions[0].Mount_point);
    ret = CreateDirent(remote_dir);
    if (0 != ret)
    {
        SD_OPS_ERROR("Fail to create %s dir!\n", remote_dir);
        goto INIT_ERR;
    }

    /* 设置文件的路径(必须要最新调用) */
    snprintf(file_path, sizeof(file_path), "%s/%s", SD_MOUNT_POINT, TMP_FRAME_INFO_FILE);
    //SetFrameFileName(file_path);

    /* 修复tmp文件 */
    RepairTmpPrvFile();

    g_Sd_format_status = FormatOk;

    pthread_mutex_unlock(&global_mutex);

    SD_OPS_DEBUG("g_diskinfo.Partitions[0].DevName = %s\n", g_diskinfo.Partitions[0].DevName);
    SD_OPS_DEBUG("g_diskinfo.Partitions[0].Total_partitions_space = %ld MB\n", g_diskinfo.Partitions[0].Total_partitions_space);
    SD_OPS_DEBUG("g_diskinfo.Partitions[0].Available_partitions_space = %ld MB\n", g_diskinfo.Partitions[0].Available_partitions_space);
    SD_OPS_DEBUG("g_diskinfo.Partitions[0].Mount_point = %s\n", g_diskinfo.Partitions[0].Mount_point);

    if (0 == g_DiskStatusCheck)
    {
        /* 创建磁盘检测线程 */
        if (pthread_create(&g_DiskStatusCheck, NULL, CheckDiskStatusThread, NULL))
        {
            SD_OPS_ERROR("Fail to create CheckDiskStatusThread thread!\n");
        }
    }

    if (0 == g_TimerCounterThread)
    {
        /* 计时器线程 */
        if (pthread_create(&g_TimerCounterThread, NULL, TimerCounterThreadProcess, NULL))
        {
            SD_OPS_ERROR("Fail to create TimerCounterThreadProcess thread!\n");
        }
    }

    if (0 == g_DiskFileDeleteThread)
    {
		sem_init(&g_semDeleteFileThread, 0, 0);
        /* 删除文件线程 */
        if (pthread_create(&g_DiskFileDeleteThread, NULL, DiskFileDeleteThreadProcess, NULL))
        {
            SD_OPS_ERROR("Fail to create DiskFileDeleteThreadProcess thread!\n");
        }
    }

    if (0 == g_CheckImageUsedSpaceThreadProcess)
    {
        /* 删除图片线程 */
        if (pthread_create(&g_CheckImageUsedSpaceThreadProcess, NULL, CheckImageUsedSpaceThreadProcess, NULL))
        {
            SD_OPS_ERROR("Fail to create CheckImageUsedSpaceThreadProcess thread!\n");
        }
    }

    /* 系统起来之后，需要执行一次检测 */
    sem_post(&g_semDeleteFileThread);
    sem_post(&g_semPicSpaceCheckThread);

#if !defined(__CLOSE_SWAP__)
    //swap开启
    s_bIsSwapOpenThreadExit = 0;
    if (pthread_create(&pThread, NULL, SwapOpenThread, NULL))
    {
        SD_OPS_ERROR("Fail to create SwapOpenThread thread!\n");
    }
#endif

    SD_OPS_DEBUG("Disk_Init Successful\n");
	g_bCheckSdStatusExit = 0;

    return 0;

INIT_ERR:
	UnInitAviWriteBuf();

    for (i=0; i < MAX_OPERATE_HANDLE; i++)
    {
        SAFE_FREE(g_pFilesListSet[i]);
    }

    g_sd_card_has_inited = 0;
    g_sd_check_status    = SdCardErr;
    g_Sd_format_status   = UnFormatted;

    sem_destroy(&g_semWakeThread);
    sem_destroy(&g_semDeleteFileThread);
    sem_destroy(&g_semPicSpaceCheckThread);

    pthread_mutex_unlock(&global_mutex);

    SD_OPS_DEBUG("Sd Card Init Fail!\n");
    return ret;
} 

extern int Disk_InitV2(int nMaxFrameSize)
{ 
    MAX_SIZE_PER_FRAME = nMaxFrameSize;
    return Disk_Init();
}

/* SD卡反初始化 */
extern int Disk_Uinit(void)
{
    int i = 0;
    int ret = 0;
    if (g_bCheckSdStatusExit)
    {
        SD_OPS_ERROR("Disk is aready uinit\n");
        return 0;
    }
#if !defined(__CLOSE_SWAP__)
    char command[128] = {0};
    s_bIsSwapOpenThreadExit = 1;
    if (1 == s_nSwapRunStatus)    //swap启动中
    {
        i = 200;
        while (i--)
        {
            usleep(50000);
            if (1 != s_nSwapRunStatus)
            {
                break;
            }
        }
    }
    memset(command, 0, sizeof(command));
    snprintf(command, sizeof(command), "swapoff %s", OS_SWAP_FILE_PATH);
    mysystem(command);
    s_nSwapRunStatus = 0;
#endif       
	StopAviAction(STOP_WRITE_AVI, 0);
	for (i=0; i<2; i++)
	{
		StopAviAction(STOP_READ_AVI, i);
	}
  
    ret = EDVR_HD_RemoveDiskCallBack(g_sd_dev);
    if (0 != ret)
    {
        SD_OPS_ERROR("remove %s failed!\n", g_sd_dev);
        ret = -1;
    }
    else
    {
        SD_OPS_DEBUG("remove %s success!\n", g_sd_dev);
    }

    sem_destroy(&g_semWakeThread);

    SAFE_FREE(g_pEventInfo);
    for (i = 0 ; i<MAX_OPERATE_HANDLE; i++)
    {
        SAFE_FREE(g_pFilesListSet[i]);
    }

	UnInitAviWriteBuf();

    g_sd_card_has_inited = 0;
    g_bCheckSdStatusExit = 1;
	g_sd_check_status = NoSdCard;
	mysystem(SD_NO_CMD);

    return ret;
}

static int SDOPS_FreeMemUse ()
{
    return 0;
}

static int SDOPS_ReallocMem ()
{
    SD_OPS_DEBUG ("\n");
    return 0;
}

int SDOPS_Disk_Pause ()
{
    /* 封锁接口调用 */
    g_sdops_pause_flag = 1;

    /* 释放内存占用 */
    SDOPS_FreeMemUse ();

    return 0;
}

int SDOPS_Disk_Resume ()
{
    /* 恢复内存占用 */
    SDOPS_ReallocMem ();

    /* 开放接口调用 */
    g_sdops_pause_flag = 0;

    return 0;
}

/*=========================================================================================================*/

/* 获取磁盘资源占用情况(单位: MB) */
extern int GetDiskResourceSize(int *pRecordMB, int *pPictureMB)
{
    if (!pRecordMB || !pPictureMB)
    {
        SD_OPS_ERROR("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    *pPictureMB = 0;
    *pRecordMB  = 0;

    if (0 == g_sd_card_has_inited)
    {
        return -1;
    }

    *pPictureMB  = g_picture_space_kb >> 10;
    *pRecordMB   = g_video_space_kb   >> 10;

    SD_OPS_DEBUG("pRecordMB=%d, pPictureMB=%d\n", *pRecordMB, *pPictureMB);

    return 0;
}

/* 获取录像存放的比例(范围0~100) */
extern int GetVideoUsedSpaceRate(void)
{
    return 0;
}

/* 设置录像存放的比例(范围0~100) */
extern int SetVideoUsedSpaceRate(int nVideoSpace)
{
    // 暂时不支持设置
    return 0;
}

/* 设置录像尺寸 */
extern int SetRecordSize(int width, int height, int bH265)
{
    SetAviRecSize(width, height, bH265);

    return 0;
}

/* SD卡文件完整性检测 */
static int SdCardReadWriteTest(void)
{
    SD_OPS_DEBUG("Start to Sd Card Read and Write test!\n");
    
    if (access("/tmp/app/disk", F_OK))
    {
        mysystem("mkdir /tmp/app/disk");
    }

    int ret = 0;

    char md5_before[32] = {0};
    char md5_after[32]  = {0};

    /* Step 1: 挂载 */
    ret = EDVR_HD_MountNode(SD_DEV_NODE, "/tmp/app/disk");
    if (ret)
    {
        SD_OPS_ERROR("SD Card mount fail!\n");
        return -1;
    }

    /* Step 2: 写测试 */

    /* Step 2.1 先删除原来存在的测试文件(如果有)，然后创建新的文件 */
    mysystem("rm -rf /tmp/app/disk/test_file.txt");
    mysystem("echo test_file > /tmp/app/disk/test_file.txt");

    /* 如果测试文件不存在，则表示创建失败 */
    if (access("/tmp/app/disk/test_file.txt", F_OK))
    {
        SD_OPS_ERROR("SD Card write fail!\n");
        goto TEST_FAIL;
    }

    /* Step 2.2 计算MD5值 */
    ret = ShellCmdOut("md5sum /tmp/app/disk/test_file.txt", md5_before, sizeof(md5_before));
    if (ret)
    {
        SD_OPS_ERROR("md5sum test file fail!\n");
        goto TEST_FAIL;
    }

    /* Step 2.3 卸载分区 */
    mysystem("umount /tmp/app/disk");

    /* Step 3: 读测试 */
    
    /* Step 3.1 重新挂载分区 */
    ret = EDVR_HD_MountNode(SD_DEV_NODE, "/tmp/app/disk");
    if (ret)
    {
        SD_OPS_ERROR("SD Card mount fail!\n");
        return -1;
    }

    /* Step 3.2 重新计算MD5值 */
    ret = ShellCmdOut("md5sum /tmp/app/disk/test_file.txt", md5_after, sizeof(md5_after));
    if (ret)
    {
        SD_OPS_ERROR("md5sum test file fail!\n");
        goto TEST_FAIL;
    }
    
    mysystem("rm -rf /tmp/app/disk/test_file.txt");
    mysystem("umount /tmp/app/disk");
    mysystem("rm -rf /tmp/app/disk");

    /* Step 4: 对比两个MD5值的区别 */
    if (0 == strncasecmp(md5_after, md5_before, sizeof(md5_after)))
    {
        SD_OPS_DEBUG("Good! Sd Card test Ok!\n");
        return 0;
    }
    else
    {
        SD_OPS_ERROR("Bad! Sd Card test Fail!\n");
        return -1;
    }

TEST_FAIL:
    mysystem("rm -rf /tmp/app/disk/test_file.txt");
    mysystem("umount /tmp/app/disk");
    mysystem("rm -rf /tmp/app/disk");

    return -1;
}

/*
    修复TF卡：重建分区表
@note: 针对TF卡出现读写异常 进行分区修复并格式化。
*/
int RepairAbnormalSdCard()
{
    int ret = -1;

//step1. 获取TF卡信息
    memset(&g_diskinfo, 0, sizeof(g_diskinfo));
    ret = EDVR_HD_GetDiskInfoEx(0, &g_diskinfo);
    if(ret)
    {
        SD_OPS_ERROR("EDVR_HD_GetDiskInfoEx fail!\n");
        return -1;
    }

//step2. 如果挂载上需要卸载
    if (g_diskinfo.Partitions[0].Partition_has_mounted)
    {
        SD_OPS_DEBUG("###### %s mounted, need unmount ######\n", g_diskinfo.Partitions[0].Mount_point);
        ret = EDVR_HD_UMountPartition(g_diskinfo.Partitions[0].Mount_point);
        if (0 != ret)
        {
            SD_OPS_DEBUG("umount %s fail! sleep a moment and retry!\n", g_diskinfo.Partitions[0].Mount_point);
            mysystem("sync");
            sleep(3);

            ret = EDVR_HD_UMountPartition(g_diskinfo.Partitions[0].Mount_point);
            if (0 != ret)
            {
                SD_OPS_ERROR("umount sd fail at twice!!!\n");
                mysystem("sync");
#ifdef __USE_EMMC__
                mysystem("lsof | grep -v grep | grep \"mmcblk1p1\"");
#else
                mysystem("lsof | grep -v grep | grep \"mmcblk0p1\"");
#endif
                return -1;
            }
            else
            {
                SD_OPS_INFO("umount sd success at twice!!!\n");
            }
        }
    }

//step3. 重建分区表
    ret = EDVR_HD_ReBuildPartition(SD_DEV, SD_PRIMARY_PARTITION_NUM);
    if (0 != ret)
    {
        SD_OPS_ERROR("Rebuild partition failed\n");
        return -1;
    }

    if (access(SD_DEV_NODE, F_OK) != 0)
    {
        SD_OPS_ERROR("fdisk failed, %s don't exist!!!\n", SD_DEV_NODE);
        return -1;
    }
    else
    {
        SD_OPS_DEBUG("fdisk successful, %s exist!!!\n", SD_DEV_NODE);
    }

//step4. 格式化
    ret = EDVR_HD_FormatPartition(g_diskinfo.Partitions[0].DevName);
    if (0 != ret)
    {
        SD_OPS_ERROR("Format %s fail!\n", g_diskinfo.Partitions[0].Mount_point);
        g_sd_card_format_status = 0;
        return -1;
    }

	if (ret == 0)
    {
		Change_DevFileSystemType(g_diskinfo.Dev,1,FS_FAT32_VALUE);
    }

    SD_OPS_INFO("Repair SDCard Successful!!!\n");

    return 0;
}

//异常TF卡修复后重启
static void SetSDActionAtStart(eSdActionAtStart action)
{
    char command[128] = {0};

    snprintf(command, sizeof(command), "echo %d > %s", action, SD_ACTION_AT_START_FILE);
    mysystem(command);
}

//异常TF卡修复后重启
static void *DelayReboot(void *pParam)
{
    PRT_TID();
    pthread_detach(pthread_self());

// 询问app樊工，设备每10s查询一次格式化状态
    sleep(10);
    SD_OPS_INFO("Repair SD card successful and reboot now!\n");
	RebootLinux(REBOOT_FORMAT_SDCARD);
    return NULL;
}

/*
*@breif CheckPartitionTableDamage 检测分区表是否异常
 *@return  返回0表示分区表正常
 *@         返回-1表示其他错误
 *@         返回1表示分区表异常
 *@note  
*/
int CheckPartitionTableDamage(void)
{
	unsigned int nSdTotalSector = 0;
	unsigned int nPartitonStartSector = 0;
	unsigned int nPartitonTotalSector = 0;
	int nRet = 0;

	nRet = Get_PartitionSector(SD_DEV,1,&nPartitonTotalSector,&nPartitonStartSector);
    if (0 != nRet)
    {
		SD_OPS_ERROR("Get_PartitionSector failed!\n");
        return -1;
    }

    nSdTotalSector = Get_SDCardTotalSector(SD_DEV);
    if (0 == nSdTotalSector)
    {
		SD_OPS_ERROR("Get_SDCardTotalSector failed!\n");
        return -1;
    }

	//分区表异常
    if ((nPartitonStartSector + nPartitonTotalSector) > nSdTotalSector)
    {
    	SD_OPS_INFO("nPartitonStartSector = %d\n",nPartitonStartSector);
    	SD_OPS_INFO("nPartitonTotalSector = %d\n",nPartitonTotalSector);
    	SD_OPS_INFO("nSdTotalSector = %d\n",nSdTotalSector);
		SD_OPS_ERROR("Partition abnormal !\n");
        return 1;
    }

    return 0;
}

/* 格式化SD卡(返回值: -1:失败; 0:成功 1:没有SD卡) */
extern int FormatSdCard(void)
{
    int i = 0;
    int ret = 0;
    int bUnmountFail = 0;

    char remote_dir[MAX_NAME_STR] = {0};
    pthread_t pThread;
	static int sdCardHasInited = -1;//卡是否有初始化，Disk_Init成功调用

    /* 检查当前SD卡是否存在 */
    if (0 == EDVR_HD_GetSdCardCount())
    {
        SD_OPS_DEBUG("###### No SdCard Found ######\n");
        g_Sd_format_status = NoDiskFound;
        return -1;
    }

    /* 检查SD卡是否在运行 */
    ret = CheckSdCardIsRunning();
    if (ret)
    {
        SD_OPS_DEBUG("Warning: SD Card is Running! format later!\n");
        g_Sd_format_status = DeviceIsBusy;
        return 0;
    }

    /* 格式化必须等待上次完成才可以 */
    if (Formatting == g_Sd_format_status)
    {
        SD_OPS_DEBUG("Warning: SD Card is formatting! format later!\n");
        return 0;
    }

#if !defined(__CLOSE_SWAP__)

    //确保此刻不存在swap启动线程
    s_bIsSwapOpenThreadExit = 1;
    if (1 == s_nSwapRunStatus)    //swap启动中
    {
        //等待swap启动线程退出
        //超时时间10S
        i = 200;
        while (i--)
        {
            usleep(50000);
            //swap启动线程已退出
            if (1 != s_nSwapRunStatus)
            {
                break;
            }
        }
    }
    //若此时swap启动线程扔运行中
    //则不允许格式化
    if (1 == s_nSwapRunStatus)    
    {
        SD_OPS_DEBUG("Warning: SD Card is Running! format later!\n");
        g_Sd_format_status = DeviceIsBusy;
        return 0;
    }

#endif    

    /* 格式化的时候要等数据传输结束 */
    pthread_mutex_lock(&global_mutex);

    SD_OPS_DEBUG("Prepare to format SD Card!\n");

	//保存卡是否正确初始化状态
    if(-1 == sdCardHasInited)
    {
        sdCardHasInited = g_sd_card_has_inited;
    }
    g_sd_card_has_inited = 0;//限制对卡的操作

    g_Sd_format_status = Formatting;

    g_sd_card_has_inited = 0;

    /* 停止SD卡写录像 */
    StopAviAction(STOP_WRITE_AVI, 0);

    /* 停止SD卡读取录像 */
    for (i=0; i<2; i++)
    {
        StopAviAction(STOP_READ_AVI, i);
    }

#ifdef __SUPPORT_SAVE_TF_LOG__
	TFLOG_SetAllowFlag(0);
#endif	

    memset(g_avi_file_read_write_info.ops_file_name, 0, sizeof(g_avi_file_read_write_info.ops_file_name));
    g_avi_file_read_write_info.read_write_status = IS_IDLE;

    /* 关闭全局索引文件描述符 */
    usleep(500000);
    mysystem("sync");

    mysystem("killall -9 boa");
    usleep(500000);
    mysystem("sync");

#if !defined(__CLOSE_SWAP__)
    {
        char command[64] = {0};
        //确保关闭swap
        memset(command, 0, sizeof(command));
        snprintf(command, sizeof(command), "swapoff %s", OS_SWAP_FILE_PATH);
        mysystem(command);
        s_nSwapRunStatus = 0;
    }
#endif

    if ((!CheckDiskRWIsDamage()) || (0 != CheckPartitionTableDamage()))
    {
        SD_OPS_INFO("SD card RW test is damage, and partition is abnoraml! Repair it when formatting!\n");

        //防止重复操作
        if (access("/tmp/.sd_repair_success", F_OK) == 0)
        {
            SD_OPS_DEBUG("Repair sdcard complete, waiting for reboot!\n");
            g_Sd_format_status = FormatOk;
            pthread_mutex_unlock(&global_mutex);
            return 0;
        }

        ret = RepairAbnormalSdCard();
        if (ret == 0)
        {
            SD_OPS_DEBUG("Repair SDCard success!\n");

            g_Sd_format_status = FormatOk;
            mysystem("touch /tmp/.sd_repair_success");
            WriteLog(SD_STATUS_CFG, MY_DESCRIBE(eSdCardTotalInfoStatus, SdCardFormatStatus_FormatOK));
            pthread_mutex_unlock(&global_mutex);

            // 修复成功不能马上使用TF卡录像，重启重新初始化录像资源
            pthread_create(&pThread, NULL, DelayReboot, NULL);
            return 0;
        }
        else
        {
            // 修复失败相当于原逻辑中TF卡异常时不让格式化
            SD_OPS_DEBUG("Repair SDCard failed!\n");

            g_Sd_format_status = UmountFail;
            WriteLog(SD_STATUS_CFG, MY_DESCRIBE(eSdCardTotalInfoStatus, SdCardFormatStatus_FormatFail));
            pthread_mutex_unlock(&global_mutex);

            // 修复失败自动重启，不用将问题再留给用户
            SetSDActionAtStart(SD_ACTION_AT_START_FORMAT);
            pthread_create(&pThread, NULL, DelayReboot, NULL);

            return -1;
        }
    }

    /* 获取当前SD卡的个数，当前只支持一个SD卡 */
    ret = EDVR_HD_GetTotalDiskCount();
    if (1 != ret)
    {
        SD_OPS_DEBUG("No Sd Card Found!\n");
        g_sd_check_status = NoSdCard;
        goto FormatSdCardFail;
    }

    memset(&g_diskinfo, 0, sizeof(g_diskinfo));
    ret = EDVR_HD_GetDiskInfoEx(0, &g_diskinfo);
    if (0 != ret)
    {
        SD_OPS_ERROR("EDVR_HD_GetDiskInfoEx fail!\n");
        goto FormatSdCardFail;
    }
    
    chdir("/");
    
    /* 不可以格式化已经挂载的分区，如果分区已经挂载，则需要卸载 */
    if (g_diskinfo.Partitions[0].Partition_has_mounted)
    {
        /* 卸载分区 */
        SD_OPS_DEBUG("umount %s\n", g_diskinfo.Partitions[0].Mount_point);
        ret = EDVR_HD_UMountPartition(g_diskinfo.Partitions[0].Mount_point);
        if (0 != ret)
        {
            /* 卸载失败之后，等待3秒，再次卸载 */
            SD_OPS_DEBUG("umount %s fail! sleep a moment and retry!\n", g_diskinfo.Partitions[0].Mount_point);
            mysystem("sync");
            sleep(3);
            ret = EDVR_HD_UMountPartition(g_diskinfo.Partitions[0].Mount_point);
            if (0 != ret)
            {
                bUnmountFail = 1;
                mysystem("sync");
                SD_OPS_ERROR("umount sd fail! need to reboot and format retry!\n");
#ifdef __USE_EMMC__
                mysystem("lsof | grep -v grep | grep \"mmcblk1p1\"");
#else
                mysystem("lsof | grep -v grep | grep \"mmcblk0p1\"");
#endif

                goto FormatSdCardFail;
            }
        }
    }
    
    /* 格式化分区 */
    if (0 == bUnmountFail)
    {
        g_sd_card_format_status = -1;    

		ret = EDVR_HD_ReBuildPartition(SD_DEV, SD_PRIMARY_PARTITION_NUM);
        if (0 != ret)
        {
            SD_OPS_ERROR("SD card repart failed!\n");
        }
        else
        {
            SD_OPS_INFO("SD card repart successful!\n");
        }
        
//      sem_post(&g_semWakeThread);
        ret = EDVR_HD_FormatPartition(g_diskinfo.Partitions[0].DevName);
		if (ret == 0)
        {
			Change_DevFileSystemType(g_diskinfo.Dev,1,FS_FAT32_VALUE);
        }
        
        if (0 != ret)
        {
            SD_OPS_ERROR("Format %s fail!\n", g_diskinfo.Partitions[0].Mount_point);
            g_sd_card_format_status = 0;
            goto FormatSdCardFail;
        }
        g_sd_card_format_status = 0;

        mysystem("dmesg -c");
        WriteLog(SD_STATUS_CFG, MY_DESCRIBE(eSdErrorNo, MIDDLE_FORMAT));

		/* 检查SD卡文件完整性 */
		if (SdCardReadWriteTest())
		{
			SD_OPS_ERROR("Sd Card read write fail in middle!\n");
			WriteLog(SD_STATUS_CFG, MY_DESCRIBE(eSdErrorNo, MIDDLE_FILE_MD5_EXP));
			goto FormatSdCardFail;
		}

        /* 重新挂载分区(为了防止dosfsck命令占用太多CPU，这里格式化之后不进行dosfsck) */
        ret = DiskMountPartitionNoDosfsck(g_diskinfo.Partitions[0].DevName);
        if (0 != ret)
        {
            SD_OPS_ERROR("mount %s fail!\n", g_diskinfo.Partitions[0].Mount_point);  
            goto FormatSdCardFail;
        }
    }

    /* 检查SD卡系统是否有dmesg错误 */
    if ( (1==IsStringInCommand("dmesg", "mmcblk0: error"))
      || (1==IsStringInCommand("dmesg", "mmc1: error")))
    {
        SD_OPS_ERROR("First check sd card kernel error message in middle exception!\n");
        WriteLog(SD_STATUS_CFG, MY_DESCRIBE(eSdErrorNo, MIDDLE_DSG_EXP));
        goto FormatSdCardFail;
    }

    /* SD卡格式化正常后,将状态置为正常 5 - 正常 */
    mysystem(SD_NORMAL_CMD);
    /* 重新获取分区信息 */
    memset(&g_diskinfo, 0, sizeof(g_diskinfo));
    ret = EDVR_HD_GetDiskInfoEx(0, &g_diskinfo);
    if (0 != ret)
    {
        SD_OPS_ERROR("EDVR_HD_GetDiskInfoEx fail!\n");
        goto FormatSdCardFail;
    }
    
    strncpy(g_sd_dev, g_diskinfo.Dev, sizeof(g_sd_dev) - 1);
    memset(&g_diskinfo, 0, sizeof(g_diskinfo));
    ret = EDVR_HD_GetDiskInfoExFromDev(g_sd_dev, &g_diskinfo);
    if (0 != ret)
    {
        SD_OPS_ERROR("EDVR_HD_GetDiskInfoExFromDev fail!\n");
        goto FormatSdCardFail;
    }

    if (g_diskinfo.RW_Status)
    {
        g_sd_check_status = SdCardReadWrite;
    }
    else
    {
        g_sd_check_status = SdCardReadOnly;
    }

#if !defined(__CLOSE_SWAP__)  
    //开启swap
    s_bIsSwapOpenThreadExit = 0;
    pthread_create(&pThread, NULL, SwapOpenThread, NULL);
#endif    

    /* 格式化之后，需要重新获设置录像和图片存放的空间比例*/
    g_picture_space_kb =   MAX_PICTURE_SIZE_MB << 10;
    g_video_space_kb = (g_diskinfo.Partitions[0].Total_partitions_space - MAX_PICTURE_SIZE_MB) << 10;

	//如果初始化时有正确的初始化了，那格式化成功后才能恢复此变量为已初始化
    if(sdCardHasInited)
    {
        g_sd_card_has_inited = 1;
    }

    g_sd_card_has_inited = 1;

    g_second_times = 0;

    memset(g_tmp_file_name, 0, sizeof(g_tmp_file_name));

    /* 创建必要的文件夹 */
    snprintf(remote_dir, sizeof(remote_dir), "%s/video", g_diskinfo.Partitions[0].Mount_point);
    ret = CreateDirent(remote_dir);
    if (0 != ret)
    {
        SD_OPS_ERROR("Fail to create %s dir!\n", remote_dir);
        goto FormatSdCardFail;
    }

    snprintf(remote_dir, sizeof(remote_dir), "%s/picture", g_diskinfo.Partitions[0].Mount_point);
    ret = CreateDirent(remote_dir);
    if (0 != ret)
    {
        SD_OPS_ERROR("Fail to create %s dir!\n", remote_dir);
        goto FormatSdCardFail;
    }
    
    SD_OPS_DEBUG("g_diskinfo.Partitions[0].DevName = %s\n", g_diskinfo.Partitions[0].DevName);
    SD_OPS_DEBUG("g_diskinfo.Partitions[0].Total_partitions_space = %ld MB\n", g_diskinfo.Partitions[0].Total_partitions_space);
    SD_OPS_DEBUG("g_diskinfo.Partitions[0].Available_partitions_space = %ld MB\n", g_diskinfo.Partitions[0].Available_partitions_space);
    SD_OPS_DEBUG("g_diskinfo.Partitions[0].Mount_point = %s\n", g_diskinfo.Partitions[0].Mount_point);


    mysystem(RUN_BOA_EXE);

    g_b_need_creat_avi_file = 1;
    g_Sd_format_status = FormatOk;
    g_sd_format_over = 1;

    SD_OPS_DEBUG("Format Sd Card OK!\n");
    WriteLog(SD_STATUS_CFG, MY_DESCRIBE(eSdCardTotalInfoStatus, SdCardFormatStatus_FormatOK));

    pthread_mutex_unlock(&global_mutex);

#ifdef __SUPPORT_SAVE_TF_LOG__
	TFLOG_SetAllowFlag(1);
#endif	

	//启动时是卡异常所以未完成Disk_Init，现在格式化成功了但是用户仍然无法正常使用卡，这时需要重启
    if(0 == sdCardHasInited)
    {
        pthread_create(&pThread, NULL, DelayReboot, NULL);
    }

    return 0;

FormatSdCardFail:
    g_sd_card_has_inited = 0;
    if (bUnmountFail)
    {
        g_Sd_format_status = UmountFail;
        WriteLog(SD_STATUS_CFG, MY_DESCRIBE(eSdCardTotalInfoStatus, SdCardFormatStatus_UmountFail));
    }
    else
    {
        g_Sd_format_status = FormatErr;
        WriteLog(SD_STATUS_CFG, MY_DESCRIBE(eSdCardTotalInfoStatus, SdCardFormatStatus_FormatFail));
    }

    SD_OPS_ERROR("Format Sd Card Fail!\n");

	mysystem(RUN_BOA_EXE);

    pthread_mutex_unlock(&global_mutex);

    // 修复失败自动重启，不用将问题再留给用户
    SetSDActionAtStart(SD_ACTION_AT_START_FORMAT);
    pthread_create(&pThread, NULL, DelayReboot, NULL);

#ifdef __SUPPORT_SAVE_TF_LOG__
	TFLOG_SetAllowFlag(1);
#endif	
    
    return -1;
}

/* 获取SD卡状态 */
extern int GetSdCardFormatStatus(void)
{
//  SD_DBG("g_Sd_format_status = %d\n", g_Sd_format_status);
    return g_Sd_format_status;
}

/* 获取写SD卡的状态 */
extern int GetSdCardWriteStatus(void)
{
//  SD_DBG("g_sd_write_status = %d\n", g_sd_write_status);
    return g_sd_write_status;
}

/* 获取SD卡相关信息 */
extern int GetSdCardInfo(LPSdCardStatus pSdCardStatus)
{   
    if (Formatting == g_Sd_format_status)
    {
        SD_OPS_DEBUG("Sd Card is formatting, please try it later!\n");
        return -1;
    }

    stuSdCardStatus  sdcard_status;
    unsigned long dwTotalSize = 0;
    unsigned long dwAvailableSize = 0;

    EDVR_HD_GetMountPointSpace(g_diskinfo.Partitions[0].Mount_point, NULL, &dwTotalSize, &dwAvailableSize);

    memset(&sdcard_status, 0, sizeof(stuSdCardStatus));

    sdcard_status.eSdcardStatus = g_sd_check_status;
    sdcard_status.dwTotalSpace = dwTotalSize;
    sdcard_status.dwAvailableSpace = dwAvailableSize;

    memcpy(pSdCardStatus, &sdcard_status, sizeof(stuSdCardStatus));

    return 0;
}

/* 设置SD卡录像类型
 * 注意: 需要对录像类型进行优先级处理: HTTP_EVENT > ALARM_IN > MOTION > TIMER
 */
extern void SetSdRecordType(int nRecordType)
{
    if (nRecordType < eNoRecord || nRecordType > eRecordTypeNum)
    {
        SD_OPS_DEBUG("nRecordType = %d, out of [%d, %d]\n", 
                nRecordType, eNoRecord, eRecordTypeNum);
        return ;
    }

    if (SDOPS_RecordTypeCompare((eRecordType)nRecordType, g_curr_record_type) > 0)
    {
        g_curr_record_type = (eRecordType)nRecordType;
    }
}

/* 获取SD卡录像类型 */
extern int GetSdRecordType(void)
{
    return (int)g_curr_record_type;
}

/*@breif GetFileReadWriteStatus 获取当前文件的读写状态
 *@param [IN]  file_name 需要操作的文件名(绝对路径)
 *@param [OUT] read_write_status 
 *@            0:  当前文件空闲
 *@            1:  当前文件正在读
 *@            2:  当前文件正在写
 *@param [OUT] file_size 文件大小(单位: 字节) 
 *@return 返回0表示成功
 *@      返回-1表示失败
 */
extern int GetFileReadWriteStatus(const char *file_name, 
                                         unsigned int *read_write_status,
                                         unsigned int *file_size)
{
    if (!file_name || !read_write_status || !file_size)
    {
        SD_OPS_ERROR("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    /* 当前文件不存在 */
    if (access(file_name, F_OK))
    {
        SD_OPS_ERROR("File %s not exist!\n", file_name);
        return -1;
    }

    *file_size = GetFileSize(file_name);
	*read_write_status = 0; // 空闲
    
    return 0;
}

/* 设置获取时间偏移回调 */
extern int SetGetTimeOffsetCallBack(int (*CallBack)(int*, int *, int *))
{
    if (NULL == CallBack)
    {
        return -1;        
    }

    m_GetTimeOffsetCallBack = CallBack;

    return 0;
}

/* 设置录像延迟时间，获取之后再查一下 */
int SDOPS_SetRecordDelayTime (unsigned int nDelaySecs)
{
    return 0;
}

/* 获取录像延迟时间 */
int SDOPS_GetRecordDelayTime (unsigned int *pnDelaySecs)
{
    return 0;
}

/*@breif SDOPS_GetRecordDataSize 获取指定时间段的录像数据大小
 *@param [IN] start_time 开始时间
 *@param [IN] stop_time  结束时间
 *@param [IN] record_type 筛选的录像类型(当前无效)
 *@param [OUT] p_size 获取到的帧数据大小
 *@return 返回  0 表示获取成功
 *@       返回 -1 表示获取失败
 *@note   获取到的是数据帧大小，包括帧头
 */
int SDOPS_GetRecordDataSize (unsigned int start_time, 
                                          unsigned int stop_time, 
                                          int record_type,
                                          unsigned int *p_size)
{
    if (!p_size)
    {
        SD_OPS_ERROR("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    int i = 0;
    int ret = -1;
    int file_count = 0;
    char dest_file[MAX_PATH_STR_NUM] = {0};
    unsigned int last_frame_pos = 0;
    unsigned int seek_to_begin_size_bytes_first = 0;  // 第一个文件: 从seek的开始位置到文件第一帧的大小
    unsigned int seek_to_begin_size_bytes_last = 0;   // 最后一文件: 从seek的开始位置到文件第一帧的大小
    unsigned int seek_to_end_size_bytes_first = 0;      // 第一个文件: 从seek的开始位置到文件最后一帧的大小
    unsigned int seek_to_end_size_bytes_last = 0;       // 最后一文件: 从seek的开始位置到文件最后一帧的大小

    unsigned int seek_time_ifame_time = 0;
    RecordSearchList *p_record_search_list[MAX_FILE_NUM] = {NULL};

    /* Step 1: 搜索符合条件的录像文件列表 */
    ret = GetRecordListByTime(start_time, stop_time, record_type, p_record_search_list, &file_count);
    if (ret)
    {
        SD_OPS_ERROR("Fail to called GetRecordListByTime\n");
        return -1;
    }

#ifdef SD_DBG_INF
    for (i=0; i<file_count; i++)
    {
        SD_OPS_DEBUG("[%d] %s: [%d - %d] - %d - %d bytes\n", i,
                p_record_search_list[i]->file_name,
                p_record_search_list[i]->start_time,
                p_record_search_list[i]->stop_time,
                p_record_search_list[i]->record_type,
                p_record_search_list[i]->file_size);
    }
#endif

    /* 未找到符合条件的录像文件 */
    if (file_count < 1)
    {
        SD_OPS_ERROR("Not found record file in [%d, %d]\n", start_time, stop_time);
        SAFE_FREE(p_record_search_list[0]);
        return -1;
    }

    /* Step 2: 去掉首尾的两个文件(后面单独处理) */
    if (file_count > 2)
    {
        for (i=1; i<file_count-1; i++)
        {
            if (strlen(p_record_search_list[i]->file_name))
            {
                memset(dest_file, 0, sizeof(dest_file));
                snprintf(dest_file, sizeof(dest_file), "%s/video/%s", 
                          SD_MOUNT_POINT, p_record_search_list[i]->file_name);
                GetRecordFileLastFramePos(dest_file, &last_frame_pos);
                *p_size += (last_frame_pos - sizeof(MPEG4FILEHEADER));
            }
        }
    }

    /* Step 3: 处理第一个录像文件 */
    snprintf(dest_file, sizeof(dest_file), "%s/video/%s", 
              SD_MOUNT_POINT, p_record_search_list[0]->file_name);
    ret = GetRecordFileSeekInfo(dest_file, 
                                  start_time, 
                                  0,
                                  &seek_time_ifame_time,
                                  &seek_to_begin_size_bytes_first, 
                                  &seek_to_end_size_bytes_first);
    if (ret)
    {
        SD_OPS_ERROR("Fail to get file %s seek_time %d info!\n", 
                p_record_search_list[0]->file_name, start_time);
        SAFE_FREE(p_record_search_list[0]);
        return -1;
    }

    /* Step 4: 处理最后一个文件 */
    memset(dest_file, 0, sizeof(dest_file));
    snprintf(dest_file, sizeof(dest_file), "%s/video/%s", 
              SD_MOUNT_POINT, p_record_search_list[file_count-1]->file_name);
    ret = GetRecordFileSeekInfo(dest_file, 
                                  stop_time, 
                                  1,
                                  &seek_time_ifame_time,
                                  &seek_to_begin_size_bytes_last, 
                                  &seek_to_end_size_bytes_last);
    if (ret)
    {
        SD_OPS_ERROR("Fail to get file %s seek_time %d info!\n", 
                p_record_search_list[file_count-1]->file_name, stop_time);
        SAFE_FREE(p_record_search_list[0]);
        return -1;
    }

    SAFE_FREE(p_record_search_list[0]);


/* |-----------------------|-----------------------| |-----------------------| |-----------------------|-----------------------|
 *                         V ---- seek_to_end ----    ----- total_file ------   ---- seek_to_begin --- V
 * 
 *
 * |—————————————|———————————————————————————————————————|———————————|
 *  ----- seek_to_begin ---- V
 *  ———————————————————-----——seek_to_begin—————————---———————————-V
 */
    /* 针对搜索片段在一个文件之内的情况，需要单独处理 */
    if (file_count > 1)
    {
        *p_size += (seek_to_end_size_bytes_first + seek_to_begin_size_bytes_last);
    }
    else
    {
        *p_size += (seek_to_begin_size_bytes_last - seek_to_begin_size_bytes_first);
    }

    return 0;
}

/*@breif SDOPS_GetRecordInfo 获取当前录像时间的录像信息
 *@param [OUT] p_curr_rec_start_time 获取到的录像文件起始时间
 *@return 返回  0 表示获取成功
 *@       返回 -1 表示获取失败
 */
int SDOPS_GetRecordInfo(int *p_curr_rec_start_time)
{
    /* TODO: */
    return 0;
}

/****************************************************************************************************************************/
// 20201116042608-20201116042813_MOTION.mp4
static int parserSearchTime(const char *pFileNameStr, time_t *pBeginTimeStamp, time_t *pEndTimeStamp)
{
    if (!pFileNameStr || !pBeginTimeStamp || !pEndTimeStamp || strlen(pFileNameStr)<29)
    {
        fprintf(stderr, "[%s-%d] param error!\n", __FILE__, __LINE__);
        return -1;
    }

    char begin_time_str[15] = {0};
    char end_time_str[15] = {0};
    char *p = NULL;

    if ((p = strrchr(pFileNameStr, '/')))
    {
        memcpy(begin_time_str, p+1, sizeof(begin_time_str)-1);
        memcpy(end_time_str, p+1+sizeof(begin_time_str), sizeof(end_time_str)-1);
    }
    else
    {
        memcpy(begin_time_str, pFileNameStr, sizeof(begin_time_str)-1);
        memcpy(end_time_str, pFileNameStr+sizeof(begin_time_str), sizeof(end_time_str)-1);
    }

    if (strspn(begin_time_str, DIGIT_STR) == strlen(begin_time_str))
    {
        TIME_STRING_TO_STAMP(begin_time_str, *pBeginTimeStamp);
    }
    else
    {
        fprintf(stderr, "[%s-%d] Error! File name str is %s, can not parser begin time!\n", 
                __FILE__, __LINE__, pFileNameStr);
        return -1;
    }

    if (strspn(end_time_str, DIGIT_STR) == strlen(end_time_str))
    {
        TIME_STRING_TO_STAMP(end_time_str, *pEndTimeStamp);
    }
    else
    {
        fprintf(stderr, "[%s-%d] Error! File name str is %s, can not parser end time!\n", 
                __FILE__, __LINE__, pFileNameStr);
        return -1;
    }

    return 0;
}

/*@breif SDOPS_GetRecordFileInfoByFileName 获取文件名对应的文件信息
 *@param[in]  file_name 文件名
 *@param[out]  file_info 文件信息
 *@return 成功返回0 失败返回-1
 */
int SDOPS_GetRecordFileInfoByFileName(const char* file_name, void* file_info)
{
    if (!file_name || !file_info)
    {    
        SD_OPS_ERROR("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    time_t begin_time_stamp = 0;
    time_t end_time_stamp = 0;
    stuDateTime date_time = {0};
    char dest_path[15] = {0};
    char abs_rec_file[MAX_PATH_STR_NUM] = {0};
    Avi_read_list *p_file_info = (Avi_read_list *)file_info;

    if (parserSearchTime(file_name, &begin_time_stamp, &end_time_stamp))
    {
        SD_OPS_ERROR("Fail to parser search time!\n");
        return -1;
    }

    TIME_STAMP_TO_STRING_DAY(begin_time_stamp, dest_path);

    snprintf(p_file_info->file_path, MAX_NAME_STR, "%s/video/%s", SD_MOUNT_POINT, dest_path);

    snprintf(abs_rec_file, MAX_PATH_STR_NUM, "%s/%s", p_file_info->file_path, p_file_info->file_name);

    if (access(abs_rec_file, F_OK))
    {
        SD_OPS_ERROR("%s not exist!\n", abs_rec_file);
        return -1;
    }

    p_file_info->file_size =  GetFileSize(abs_rec_file);

    ULONG_TO_TIME(begin_time_stamp,
                  (date_time.nYear), (date_time.nMonth), (date_time.nDay), \
                  (date_time.nHour), (date_time.nMinute), (date_time.nSecond));

    memcpy(&p_file_info->start_time, &date_time, sizeof(stuDateTime));

    ULONG_TO_TIME(end_time_stamp,
                  (date_time.nYear), (date_time.nMonth), (date_time.nDay), \
                  (date_time.nHour), (date_time.nMinute), (date_time.nSecond));

    memcpy(&p_file_info->stop_time, &date_time, sizeof(stuDateTime));

    return 0;
}
