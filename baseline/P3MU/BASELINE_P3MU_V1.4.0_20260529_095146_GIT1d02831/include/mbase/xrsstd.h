/**
********************************************************************************
 * @Copyright: 2022 RUISION CO.,LTD. All Rights Reserved.
 * @FilePath: \xrsframework\include\mbase\xrsstd.h
 * @Author: HL.LIU
 * @Version: 
 * @Date: 2022-09-09 14:11:59
 * @Descripttion: 
********************************************************************************
*/
#ifndef _XRSSTD_H_
#define _XRSSTD_H_

#include <unistd.h>
#include <syslog.h>
#include <pthread.h>
#include <linux/types.h>
#include <time.h>
#include <stdio.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/syscall.h>

#define MDBG_DISABLE    /* 启用终端调试打印，不定义则采用syslog系统 */
//#define ERR_DEBUG       /* 打开debug打印等级 */

#ifndef MDBG_DISABLE
#include "mdbg.h"
#else
#include "xrsdef.h"
#endif /* MDBG_DISABLE */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef MEM_MANAGE_DISABLE
#define Mem_Manage_Malloc(CompId, size)           malloc((size))
#define Mem_Manage_Calloc(CompId, nMemb, size)    calloc((nMemb), (size))
#define Mem_Manage_Free(CompId, pVoid)            free((pVoid))
#endif /* NOT MEM_MANAGE_DISABLE */

#define MAX_THREAD_NAME_LEN     16
#define SET_CURRENT_THREAD_PID  (-1)    /* 设置当前线程PID */

typedef enum
{
    CPUID_MASK_1ST = BIT32(0),
    CPUID_MASK_2ND = BIT32(1),
    CPUID_MASK_3RD = BIT32(2),
    CPUID_MASK_4TH = BIT32(3),
} CPUID_MASK_E;

#ifdef MDBG_DISABLE
#define PERROR(errno)   printf(COLOR_STR_RED "COMP_%s: errno=%u, [%s:%s #%d]\n" COLOR_STR_NONE, \
    GET_COMPSTR(GET_COMPID(errno)), GET_ERRNO(errno), __FILE__, __FUNCTION__, __LINE__)

#ifdef ERR_DEBUG
#define TRACE_DBG_LVL(CompId, level, str, args...)  do {                                        \
    switch (level) {                                                                            \
    case PRN_LVL_ERR:                                                                           \
        printf(COLOR_STR_RED "%s>: FUNCTION: %s, LINE: %d, " COLOR_STR_NONE str,                \
            __FILE__, __FUNCTION__, __LINE__, ## args);                                         \
        break;                                                                                  \
    case PRN_LVL_WARNING:                                                                       \
        printf(COLOR_STR_YELLOW "%s>: FUNCTION: %s, LINE: %d, " COLOR_STR_NONE str,             \
            __FILE__, __FUNCTION__, __LINE__, ## args);                                         \
        break;                                                                                  \
    case PRN_LVL_WORKFLOW:                                                                      \
        printf(COLOR_STR_GREEN "%s>: FUNCTION: %s, LINE: %d, " COLOR_STR_NONE str,              \
            __FILE__, __FUNCTION__, __LINE__, ## args);                                         \
        break;                                                                                  \
    case PRN_LVL_INFO:                                                                          \
    case PRN_LVL_DBG0:                                                                          \
    case PRN_LVL_DBG1:                                                                          \
    case PRN_LVL_DBG2:                                                                          \
    case PRN_LVL_DBG3:                                                                          \
    case PRN_LVL_DBG4:                                                                          \
    case PRN_LVL_DBG5:                                                                          \
        printf(COLOR_STR_WHITE "%s>: FUNCTION: %s, LINE: %d, " COLOR_STR_NONE str,              \
            __FILE__, __FUNCTION__, __LINE__, ## args);                                         \
        break;                                                                                  \
    default:                                                                                    \
        break;                                                                                  \
    } } while (0)
#define TRACE_ASSERT(exp, CompId, str, args...) do { if (!(exp)) {                              \
    do {                                                                                        \
        time_t TmTmp = 0; struct tm *pTm = NULL;                                                \
        time(&TmTmp); pTm = gmtime(&TmTmp);                                                     \
        printf(COLOR_STR_RED TIME_STR "assert @ %s>: FUNCTION: %s, LINE: %d, " COLOR_STR_NONE   \
            str, STUTIME4TM_FMT(*pTm), __FILE__, __FUNCTION__, __LINE__, ## args);              \
            usleep(3000000);                                                                    \
        } while (!(exp));                                                                       \
    } } while (0)
#else
#define TRACE_DBG_LVL(CompId, level, str, args...)  do {                                        \
    switch (level) {                                                                            \
    case PRN_LVL_ERR:                                                                           \
        printf(COLOR_STR_RED "%s>: FUNCTION: %s, LINE: %d, " COLOR_STR_NONE str,                \
            __FILE__, __FUNCTION__, __LINE__, ## args);                                         \
        break;                                                                                  \
    case PRN_LVL_WARNING:                                                                       \
        printf(COLOR_STR_YELLOW "%s>: FUNCTION: %s, LINE: %d, " COLOR_STR_NONE str,             \
            __FILE__, __FUNCTION__, __LINE__, ## args);                                         \
        break;                                                                                  \
    case PRN_LVL_WORKFLOW:                                                                      \
        printf(COLOR_STR_GREEN "%s>: FUNCTION: %s, LINE: %d, " COLOR_STR_NONE str,              \
            __FILE__, __FUNCTION__, __LINE__, ## args);                                         \
        break;                                                                                  \
    case PRN_LVL_INFO:                                                                          \
        printf(COLOR_STR_WHITE "%s>: FUNCTION: %s, LINE: %d, " COLOR_STR_NONE str,              \
            __FILE__, __FUNCTION__, __LINE__, ## args);                                         \
        break;                                                                                  \
    default:                                                                                    \
        break;                                                                                  \
    } } while (0)
#define TRACE_ASSERT(exp, CompId, str, args...) do { if (!(exp)) {                              \
    time_t TmTmp = 0; struct tm *pTm = NULL;                                                    \
    time(&TmTmp); pTm = gmtime(&TmTmp);                                                         \
    TRACE_ERR(CompId, COLOR_STR_RED TIME_STR "assert @ %s>: FUNCTION: %s, LINE: %d, "           \
        COLOR_STR_NONE str, STUTIME4TM_FMT(*pTm), __FILE__, __FUNCTION__, __LINE__, ## args);   \
    } } while (0)
#endif /* ERR_DEBUG */

#else
#define PERROR(errno)   do {                                                                    \
    SINT32 MDbgFlag = MDbg_Get_PrnMannerFlag();                                                 \
    switch (MDbgFlag) {                                                                         \
    case PRN_MANNER_TERM:                                                                       \
        printf(COLOR_STR_RED "[%ld]COMP_%s: errno=%u, [%s:%s #%d]\n" COLOR_STR_NONE,            \
            syscall(SYS_gettid), GET_COMPSTR(GET_COMPID(errno)),                                \
            GET_ERRNO(errno), __FILE__, __FUNCTION__, __LINE__);                                \
        break;                                                                                  \
    case PRN_MANNER_SYSLOG:                                                                     \
        syslog(LOG_USER | LOG_ERR, "COMP_%s: errno=%u, [%s:%s #%d]\n",                          \
            GET_COMPSTR(GET_COMPID(errno)), GET_ERRNO(errno),                                   \
            __FILE__, __FUNCTION__, __LINE__);                                                  \
        break;                                                                                  \
    default:                                                                                    \
        break;                                                                                  \
    } } while (0)

#define TRACE_DBG_LVL(CompId, level, str, args...)                                              \
    MDbg_ModLvl_Prn(CompId, level, "%s#%d, " str, __FUNCTION__, __LINE__, ## args)
#ifdef ERR_DEBUG
#define TRACE_ASSERT(exp, CompId, str, args...) do { if (!(exp)) {                              \
    do {                                                                                        \
        time_t TmTmp = 0; struct tm *pTm = NULL;                                                \
        time(&TmTmp); pTm = gmtime(&TmTmp);                                                     \
        printf(COLOR_STR_RED TIME_STR "assert @ %s>: FUNCTION: %s, LINE: %d, " COLOR_STR_NONE   \
            str, STUTIME4TM_FMT(*pTm), __FILE__, __FUNCTION__, __LINE__, ## args);              \
            usleep(3000000); MDbg_Call_Stack();                                                 \
        } while (!(exp));                                                                       \
    } } while (0)
#else
#define TRACE_ASSERT(exp, CompId, str, args...) do { if (!(exp)) {                              \
    time_t TmTmp = 0; struct tm *pTm = NULL;                                                    \
    time(&TmTmp); pTm = gmtime(&TmTmp);                                                         \
    TRACE_ERR(CompId, COLOR_STR_RED TIME_STR "assert @ %s>: FUNCTION: %s, LINE: %d, "           \
        COLOR_STR_NONE str, STUTIME4TM_FMT(*pTm), __FILE__, __FUNCTION__, __LINE__, ## args);   \
    MDbg_Call_Stack();                                                                          \
    } } while (0)
#endif /* ERR_DEBUG */
#endif /* MDBG_DISABLE */

#ifdef ERR_DEBUG
#define TRACE_DBG(str, args...)     printf(COLOR_STR_GREEN "%s>: FUNCTION: %s, LINE: %d, "  \
    COLOR_STR_NONE str, __FILE__, __FUNCTION__, __LINE__, ## args)
#define TRACE_PRI(str, args...)     printf(str, ## args)
#else
#define TRACE_DBG(str, args...)
#define TRACE_PRI(str, args...)
#endif /* ERR_DEBUG */

#define TRACE_REL(str, args...)
#define TRACE_ALW(str, args...)     printf(str, ## args)
#define TRACE_ALWAYS(str, args...)  printf("[%lld] %s>: FUNCTION: %s, LINE: %d, "   \
    str, MVStd_Time_Get_CurMSec(), __FILE__, __FUNCTION__, __LINE__, ## args)

#define TRACE_ERR(CompId, str, args...)     TRACE_DBG_LVL(CompId, PRN_LVL_ERR, str, ## args)
#define TRACE_WARN(CompId, str, args...)    TRACE_DBG_LVL(CompId, PRN_LVL_WARNING, str, ## args)
#define TRACE_FLOW(CompId, str, args...)    TRACE_DBG_LVL(CompId, PRN_LVL_WORKFLOW, str, ## args)
#define TRACE_INFO(CompId, str, args...)    TRACE_DBG_LVL(CompId, PRN_LVL_INFO, str, ## args)
#define TRACE_DBG0(CompId, str, args...)    TRACE_DBG_LVL(CompId, PRN_LVL_DBG0, str, ## args)
#define TRACE_DBG1(CompId, str, args...)    TRACE_DBG_LVL(CompId, PRN_LVL_DBG1, str, ## args)
#define TRACE_DBG2(CompId, str, args...)    TRACE_DBG_LVL(CompId, PRN_LVL_DBG2, str, ## args)
#define TRACE_DBG3(CompId, str, args...)    TRACE_DBG_LVL(CompId, PRN_LVL_DBG3, str, ## args)
#define TRACE_DBG4(CompId, str, args...)    TRACE_DBG_LVL(CompId, PRN_LVL_DBG4, str, ## args)
#define TRACE_DBG5(CompId, str, args...)    TRACE_DBG_LVL(CompId, PRN_LVL_DBG5, str, ## args)


typedef struct
{
    UINT64 USecNext;    /* 下一次执行时间,单位微秒(根据当前时间和usec_interval算出的时间,初始化时为0) */
    UINT32 USecInter;   /* 想要的时间间隔,单位微秒(0-不做USLEEP,非0-将根据时间间隔做相应的休眠) */
    UINT32 Resvd;       /* 保留位 */
} TIME_INTERVAL_S, *PTIME_INTERVAL_S;
#define TIME_INTERVAL_S_LEN sizeof(TIME_INTERVAL_S)
ASSERT_SIZEOF_TYPEDEF_STRUCT(TIME_INTERVAL_S, 16);

#define MAX_MTD_DEV_NUMS        16
#define MAX_MTD_DEV_NAME_LEN    16

typedef struct
{
    UINT8 Upgrade;
    UINT8 Resvd[7];
    SINT64 TolSize;
    SINT64 Surplus;
    char Name[MAX_MTD_DEV_NAME_LEN];
} MTD_DEV_ATTR_S, *PMTD_DEV_ATTR_S;
#define MTD_DEV_ATTR_S_LEN  sizeof(MTD_DEV_ATTR_S)
ASSERT_SIZEOF_TYPEDEF_STRUCT(MTD_DEV_ATTR_S, 24 + MAX_MTD_DEV_NAME_LEN);

typedef struct
{
    UINT8 DevNums;
    UINT8 Resvd[7];
    MTD_DEV_ATTR_S Attr[MAX_MTD_DEV_NUMS];
} MTD_DEVS_ATTR_S, *PMTD_DEVS_ATTR_S;
#define MTD_DEVS_ATTR_S_LEN sizeof(MTD_DEVS_ATTR_S)
ASSERT_SIZEOF_TYPEDEF_STRUCT(MTD_DEVS_ATTR_S, 8 + MTD_DEV_ATTR_S_LEN * MAX_MTD_DEV_NUMS);

/******************************************************************************
 * 函数介绍: 数据处理回调
 * 输入参数: idx: 通道号
 *           fifo: 数据缓存FIFO
 *           data: 数据接收缓冲区
 *           size: 数据接收缓冲区大小
 * 输出参数: 无
 * 返回值  : SINT32
 * 注: 当fifo为0时,数据存储于data中,size表示数据长度
 *     当fifo不为0时,数据存储于fifo中,data表示用户缓冲区地址,size表示缓冲区长度
 *     当fifo,data都为空时,表示无数据需要处理,也即数据处理结束标记
 *****************************************************************************/
typedef SINT32 (*Data_Callback)(SINT32 idx, SINT32 fifo, char *data, SINT32 size);

/******************************************************************************
 * 函数介绍: 消息通知回调
 * 输入参数: wParam: 参数
 *           plParam: 参数
 * 输出参数: 无
 * 返回值  : 无
 *****************************************************************************/
typedef void (*Notify_Callback)(SINT32 wParam, SINT32 *plParam);

/******************************************************************************
 * 函数介绍: 线程启动函数
 * 输入参数: priority: 线程优先级(0-按默认值处理,不做优先级设置,1~99-线程优先级别)
 *           Func: 线程执行函数体
 *           pVoid: 函数参数
 * 输出参数: pid: 线程ID
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 PThread_Start(SINT32 *pid, SINT32 priority, Notify_Callback Func, void *pVoid);

/******************************************************************************
 * 函数功能: 按一定格式设置线程名字(长度不超过MAX_THREAD_NAME_LEN个字符)
 * 输入参数: str: 名字格式
 * 输出参数: 无
 * 返回值  : >=0-成功,<0-错误代码
 *****************************************************************************/
SINT32 PThread_Set_ThreadName(const char *str, ...);

/******************************************************************************
 * 函数功能: 多核平台线程绑定指定CPU核上运行,绑定的CPU核可以是多个
 * 输入参数: pid: 线程号PID(SET_CURRENT_THREAD_PID-表示设置当前线程,函数内部会自动获取当前线程号后设置)
 *           CpuIdMask: CPU序号ID掩码,按位表示(CPUID_MASK_E)
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 PThread_Set_AffinityNP(pthread_t pid, CPUID_MASK_E CpuIdMask);

/******************************************************************************
 * 函数功能: 检查线程锁是否可以加锁
 * 输入参数: pMtx: 线程锁结构体指针
 * 输出参数: 无
 * 返回值  : MV_FALSE-mutex为NULL或已加锁,MV_TRUE-未加锁,<0-错误代码
 *****************************************************************************/
SINT32 PThread_Mutex_CanLock(pthread_mutex_t *pMtx);

/******************************************************************************
 * 函数介绍: 计算该月份总天数
 * 输入参数: year: 年
 *           month: 月份
 * 输出参数: 无
 * 返回值  : 当前月份的天数
 *****************************************************************************/
SINT32 MVStd_Time_DayOfMonth(UINT16 year, UINT8 month);

/******************************************************************************
 * 函数介绍: 计算该日期的星期数
 * 输入参数: year: 年
 *           month: 月份
 *           day: 日期
 * 输出参数: 无
 * 返回值  : 1~7: 星期一~星期天
 *****************************************************************************/
SINT32 MVStd_Time_WeekOfDate(UINT16 year, UINT8 month, UINT8 day);

/******************************************************************************
 * 函数介绍: 计算自1970.1.1 00:00:00至当前日期时间的总秒数,包含当秒
 * 输入参数: SysTm: 系统时间定义结构体
 * 输出参数: 无
 * 返回值  : 自1970.1.1 00:00:00至日期的总秒数
 *****************************************************************************/
time_t MVStd_Time_TimeToSec(SYSTIME SysTm);

/******************************************************************************
 * 函数介绍: 系统时间总秒数到系统时间转换
 * 输入参数: sec: 系统时间总秒数
 *           pSysTm: 系统时间结构体指针
 * 输出参数: pSysTm: 系统时间结构体指针,系统时间
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
UINT32 MVStd_Time_SecToTime(UINT32 sec, PSYSTIME pSysTm);

/******************************************************************************
 * 函数介绍: 本地时间转化为格林尼标准时间
 * 输入参数: Zone: 本地时间相对于格林尼标准时间的时区差
 *           LocTm: 本地时间
 * 输出参数: pGmtTm: 格林尼标准时间
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
UINT32 MVStd_Time_LocToGmt(FLOAT32 Zone, SYSTIME LocTm, PSYSTIME pGmtTm);

/******************************************************************************
 * 函数介绍: 格林尼标准时间转化为本地时间
 * 输入参数: Zone: 格林尼标准时间相对于本地时间的时区差
 *           GmtTm: 格林尼标准时间
 * 输出参数: pLocTm: 本地时间
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
UINT32 MVStd_Time_GmtToLoc(FLOAT32 Zone, SYSTIME GmtTm, PSYSTIME pLocTm);

/******************************************************************************
 * 函数介绍: 从RTC同步系统当前时间(暂未实现)
 * 输入参数: 无
 * 输出参数: 无
 * 返回值  : 0-成功,<0-失败
 *****************************************************************************/
SINT32 MVStd_Time_UtcFromRtc(void);

/******************************************************************************
 * 函数介绍: 从系统当前时间同步RTC(暂未实现)
 * 输入参数: 无
 * 输出参数: 无
 * 返回值  : 0-成功,<0-失败
 *****************************************************************************/
SINT32 MVStd_Time_UtcToRtc(void);

/******************************************************************************
 * 函数介绍: 获取系统当前的当地时间
 * 输入参数: pSysTm: 系统时间结构体指针
 * 输出参数: pSysTm: 系统时间结构体指针,系统当前的当地时间
 * 返回值  : >0-成功(1~7:分别表示星期一到星期天),<0-失败
 *****************************************************************************/
SINT32 MVStd_Time_Get(PSYSTIME pSysTm);

/******************************************************************************
 * 函数介绍: 设置系统当前的当地时间,并未同步RTC
 * 输入参数: pSysTm: 系统时间结构体指针,系统当前的当地时间
 * 输出参数: 无
 * 返回值  : 0-成功,<0-失败
 *****************************************************************************/
SINT32 MVStd_Time_Set(PSYSTIME pSysTm);

/******************************************************************************
 * 函数介绍: 获取系统启动时间,即系统上电经历时间
 * 输入参数: pUpTmSec: 系统上电经历时间秒数
 *           pUpTmMSec: 系统上电经历时间毫秒数
 * 输出参数: pUpTmSec: 系统上电经历时间秒数
 *           pUpTmMSec: 系统上电经历时间毫秒数
 * 返回值  : 0-成功,<0-失败
 *****************************************************************************/
SINT32 MVStd_UpTime_Get(SINT32 *pUpTmSec, SINT32 *pUpTmMSec);

/******************************************************************************
 * 函数介绍: 获取系统启动时间,即系统上电经历时间秒数
 * 输入参数: 无
 * 输出参数: 无
 * 返回值  : >=0-成功,系统启动后所经历的秒数,<0-失败
 *****************************************************************************/
SINT32 MVStd_Time_Get_CurSec(void);

/******************************************************************************
 * 函数介绍: 获取系统启动时间,即系统上电经历时间毫秒
 * 输入参数: 无
 * 输出参数: 无
 * 返回值  : >=0-成功,系统启动后所经历的毫秒,<0-失败
 *****************************************************************************/
UINT64 MVStd_Time_Get_CurMSec(void);

/******************************************************************************
 * 函数介绍: 根据前后两次函数调用,计算程序执行时间,单位毫秒
 * 输入参数: pZeroInit: 值为0时,获取初始值(即系统上电经历时间),返回值为0
 * 输出参数: pUsrTm: user time
 *           pSysTm: sys time
 * 返回值  : 两次调用的时间间隔real time
 *****************************************************************************/
SINT32 MVStd_RunTime_Get_MSec(SINT32 *pZeroInit, SINT32 *pUsrTm, SINT32 *pSysTm);

/******************************************************************************
 * 函数介绍: 根据前后两次执行的系统时间和间隔时间做系统微秒级的休眠
 * 输入参数: pTmInterval
 * 输出参数: 无
 * 返回值  : 0-成功,<0-失败
 *****************************************************************************/
SINT32 MVStd_Time_Interval_USleep(PTIME_INTERVAL_S pTmInterval);

/******************************************************************************
 * 函数介绍: SWVERSION编译时间设置(取当前系统时间),版本号需要手动事先填好,再根据模块ID号打印出来
 * 输入参数: CompId: 模块ID号
 *           pSwVer: 模块软件版本结构体指针
 * 输出参数: pSwVer: 模块软件版本结构体
 * 返回值  : 无
 *****************************************************************************/
void MVStd_SWVer_Set(COMPID CompId, PSWVERSION pSwVer);

/******************************************************************************
 * 函数介绍: 根据目标和源字符串长度拷贝源字符串到目标字符串中,长度取其小者
 * 输入参数: pDst: 目标字符串指针
 *           pSrc: 源字符串指针
 *           DstLen: 目标字符串长度
 * 输出参数: pDst: 目标字符串
 * 返回值  : >=0-表示拷贝字符串长度
 *****************************************************************************/
size_t MVStd_StrL_Cpy(char *pDst, const char *pSrc, size_t DstLen);

/******************************************************************************
 * 函数介绍: 获取MTD设备属性信息
 * 输入参数: pDevsAttr: MTD设备属性结构体指针
 * 输出参数: pDevsAttr: MTD设备属性结构体指针,MTD设备属性
 * 返回值  : 0-成功,<0-失败
 *****************************************************************************/
SINT32 MVStd_Get_MtdDevs_Attr(PMTD_DEVS_ATTR_S pDevsAttr);

/******************************************************************************
 * 函数介绍: 通过MTD设备名获取设备序号
 * 输入参数: pDevName: MTD设备名
 *           DevNameLen: MTD设备名长度
 * 输出参数: 无
 * 返回值  : >=0-成功,设备序号;<0-失败
 *****************************************************************************/
SINT32 MVStd_Get_MtdDevNumFromName(const char *pDevName, SINT32 DevNameLen);

/******************************************************************************
 * 函数介绍: 根据文件路径获取文件目录名和文件名
 * 输入参数: pFilePath: 文件路径
 * 输出参数: pDirName: 文件目录名
 *           pFileName: 文件名
 * 返回值  : 0-成功,<0-失败
 *****************************************************************************/
SINT32 MVStd_Find_DirFileName(const char *pFilePath, char *pDirName, char *pFileName);

pid_t gettid(void);

#if (defined(PLATFORM_UCLIBC) && !defined(PLATFORM_X86))
SINT32 fallocate(SINT32 fd, SINT32 mode, __off_t offset, __off_t len);
#endif /* PLATFORM_UCLIBC and Not PLATFORM_X86 */

/******************************************************************************
 * 函数介绍: 数据16进制打印
 * 输入参数: p: 数据字符串指针
 *           s: 数据字符串长度
 * 返回值  : 无
 *****************************************************************************/
void MVStd_Data_Hex_Dump(char *p, SINT32 s);

SINT32 MVSTD_HexToStr(UINT8 *pHex, UINT32 HexLen, char *pHexStrBuf, UINT32 bLen);
SINT32 MVSTD_Str2Hex(char *pHexStr, UINT8 *pHexBuf, UINT32 bLen);

SINT32 MVStd_MDbg_Reg(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* _XRSSTD_H_ */
