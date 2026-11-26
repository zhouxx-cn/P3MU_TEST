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

/* 当前操作的文件的读写状态 */
typedef enum{
    IS_IDLE     = 0,                        /* 当前文件空闲   */
    IS_READING  = 1,                        /* 当前文件正在读 */
    IS_WRITTING = 2                         /* 当前文件正在写 */
}eReadWriteStatus;

/* 当前AVI文件在网页上面显示信息结构体数组 */
static Avi_read_list g_read_avi_list[MAX_AVI_NUM_ONE_PAGE];

static unsigned int  g_one_page_show_avi_count = 50;

/* 当前图片文件在网页上面显示信息结构体数组 */
static Pic_read_list g_read_pic_list[MAX_PIC_NUM_ONE_PAGE];

static unsigned int  g_one_page_show_pic_count = 50;

AviSegLinkList *SEG_LINK[4] = {NULL};

PopHandle *pop_handle = NULL;

int errorcode = 0;

static char        g_last_pic_name_pre[MAX_NAME_STR]   = {0};

static stuFileInfo *g_pFilesListSet[MAX_OPERATE_HANDLE] = {NULL};

static int g_nChoosePage = 1; /* 选中的页面(从1开始) */
static int g_nPageType = 0;   /* 当前录像路径类型 0: http路径  1:绝对路径 */

static pthread_mutex_t    seg_wr_lock               = PTHREAD_MUTEX_INITIALIZER;  //用于seg读写的锁
static pthread_mutex_t    global_mutex              = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t    global_file_mutex         = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t    global_disk_write_mutex   = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t    global_pop_frame_mutex    = PTHREAD_MUTEX_INITIALIZER;

static void *DelayReboot(void *pParam);
static void SetSDActionAtStart(eSdActionAtStart action);

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
int CreateDirent(const char *sPathName)
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

    SD_OPS_DEBUG("Disk_Init Successful\n");
	g_bCheckSdStatusExit = 0;

    return 0;

INIT_ERR:	

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
