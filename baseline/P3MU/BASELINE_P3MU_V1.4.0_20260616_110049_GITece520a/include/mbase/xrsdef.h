/**
****************************************************************************************
 * @FilePath: xrsdef.h
 * @Author: HL.LIU
 * @Date: 2022-09-01 17:16:56
 * @LastEditors: 
 * @LastEditTime: 2022-09-01 17:16:56
 * @Copyright: 2022 RUISION CO.,LTD. All Rights Reserved.
 * @Descripttion: 
****************************************************************************************
*/
#ifndef _XRSDEF_H_
#define _XRSDEF_H_

#ifndef __KERNEL__

#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h> /* __BYTE_ORDER */
#include <string.h>
#include <pthread.h>
#endif /* NOT __KERNEL__ */

/* Define default endianness */
#ifndef __LITTLE_ENDIAN
#define __LITTLE_ENDIAN 1234
#endif /* __LITTLE_ENDIAN */

#ifndef __BYTE_ORDER
#warning "Byte order not defined on your system, assuming little endian!"
#define __BYTE_ORDER    __LITTLE_ENDIAN
#endif /* NOT __BYTE_ORDER */

#ifndef NULL
#define NULL    0
#endif

#ifdef POINTER_SIZE_8
#define POINTER_TYPE    UINT64
#define POINTER_SSTR    "%llx"
#define POINTER_PSTR    "0x%llx"
#else
#define POINTER_TYPE    UINT32
#define POINTER_SSTR    "%x"
#define POINTER_PSTR    "0x%x"
#endif /* POINTER_SIZE_8 */

#ifdef OFF64_T_LONG_INT
#define OFF64_T_PSTR    "%ld"
#else
#define OFF64_T_PSTR    "%lld"
#endif /* POINTER_SIZE_8 */

#define ALL_CHANNEL (-1)    /* 参数获取和设置时,用该值表示全部通道 */
#define XRS_INVALID  (-1)
#define IPC_PARAM   0666

#define MAX_MAC_STR_LEN 6

#define SYS_PROC_NAME   "eye"

#define MAX_FILE_NAME_LEN   128
#define MAX_DIR_NAME_LEN    128
#define MAX_FILE_PATH_LEN   256

#define SYSTIME_FORM_YEAR   2000    /* */

#define ONE_MSEC_PER_USEC   1000
#define ONE_SEC_PER_MSEC    1000
#define ONE_SEC_PER_USEC    (ONE_MSEC_PER_USEC * ONE_SEC_PER_MSEC)

#define XRS_USleep(us)   usleep(us)
#define XRS_MSleep(ms)   usleep((ms) * ONE_MSEC_PER_USEC)
#define XRS_Sleep(s)     usleep((s) * ONE_SEC_PER_USEC)

#define DATA_HEX_DUMP_CNT   16
#define DATA_HEX_DUMP_STR   "%02X-%02X-%02X-%02X-%02X-%02X-%02X-%02X-%02X-%02X-%02X-%02X-%02X-%02X-%02X-%02X"

/* 数据类型定义 */
typedef signed char         SINT8;
typedef signed short        SINT16;
typedef signed int          SINT32;
typedef signed long         SLNG32;
typedef signed long long    SINT64;
typedef unsigned char       UINT8;
typedef unsigned short      UINT16;
typedef unsigned int        UINT32;
typedef unsigned long       ULNG32;
typedef unsigned long long  UINT64;
typedef float               FLOAT32;
typedef double              FLOAT64;
typedef enum
{
    XRS_FALSE = 0,
    XRS_TRUE  = 1
} XRS_BOOL;

#define IP4(a)  ((a) & 0xFF)
#define IP3(a)  (((a) >> 8) & 0xFF)
#define IP2(a)  (((a) >> 16) & 0xFF)
#define IP1(a)  (((a) >> 24) & 0xFF)

#define IP4N(a) IP1(a)
#define IP3N(a) IP2(a)
#define IP2N(a) IP3(a)
#define IP1N(a) IP4(a)

#define LGTR(a, b)  ((a) > (b))     /* 左大于右*/
#define LGER(a, b)  ((a) >= (b))    /* 左大于等于右*/
#define LLTR(a, b)  ((a) < (b))     /* 左小于右 */
#define LLER(a, b)  ((a) <= (b))    /* 左小于等于右 */
#define LNER(a, b)  ((a) != (b))    /* 左不等于右 */
#define LEQR(a, b)  ((a) == (b))    /* 左等于右 */

#define IMIN(a, b)      ((a) < (b) ? (a) : (b)) /* a <= b */
#define IMAX(a, b)      ((a) > (b) ? (a) : (b)) /* a > b */
#define CLIP(x, a, b)   IMAX(a, IMIN(x, b))     /* a <= x <= b */
#define IZERO(a, b)     ((a) ? (b) : 0)         /* a为0则0,否则为b */
#define MUX(x, a, b)    ((x) ? (a) : (b))       /* x不为0则为a,为0则为b */

/* 全开区间,左右开闭区间判断 */
#define ISIN(x, a, b)   ((x) > (a) && (x) < (b))
#define ISLIN(x, a, b)  ((x) >= (a) && (x) < (b))
#define ISRIN(x, a, b)  ((x) > (a) && (x) <= (b))
#define ISLRIN(x, a, b) ((x) >= (a) && (x) <= (b))

#ifndef BIT32
#define BIT32(x)    (1 << (x))
#endif /* BIT32 */
#define BIT64(x)    (1ULL << (x))

#define SET_BIT32(x, n) ((x) | BIT32(n))        /* 设置数字某一位为1 */
#define GET_BIT32(x, n) (((x) >> (n)) & 0x1)    /* 获取数字某一位的值(0或1) */
#define CLR_BIT32(x, n) ((x) & ~(BIT32(n)))     /* 清楚数字某一位为0 */
#define CHK_BIT32(x, n) ((x) & BIT32(n))        /* 确认数字某一位的值(0或1) */

#define SET_BIT64(x, n) ((x) | BIT64(n))        /* 设置数字某一位为1 */
#define GET_BIT64(x, n) (((x) >> (n)) & 0x1LL)  /* 获取数字某一位的值(0或1) */
#define CLR_BIT64(x, n) ((x) & ~(BIT64(n)))     /* 清楚数字某一位为0 */
#define CHK_BIT64(x, n) ((x) & BIT64(n))        /* 确认数字某一位的值(0或1) */

#define UBYTESEL(x, b)          (((x) >> ((b) << 3)) & 0xFF)
#define PACKBYTES(h, l)         ((((h) << 8) | (l)) & 0xFFFF)   /* h: 高8位,l: 低8位 */
#define PACK16LSB(h, l)         ((((h) << 16) & 0xFFFF0000) | ((l) & 0x0000FFFF))   /* h: 高16位,l: 低16位 */
#define BCD2DEC(x)              (((x) >> 4) & 0xF) * 10 + ((x) & 0xF)
#define DEC2BCD(x)              (((x) / 10) << 4) | ((x) % 10)
#define PACKCOLOR(r, g, b)      ((((b) >> 3) & 0x1F) | (((g) << 2) & 0x3E0) | (((r) << 7) & 0x7C00))
#define ALIGN_N(x, a)           (((x) + ((a) - 1)) & ~((a) - 1))
#define CLOSE_ALIGN_BIT32(x, b) MUX((x) & BIT32(b - 1), ((x) + (BIT32(b) - 1)) & ~(BIT32(b) - 1), (x) & ~(BIT32(b) - 1))
#define ALIGN_BACK(x, a)        (((x) / (a)) * (a))
#define DIV_45(a, b)            (((a) + ((b) / 2)) / ((b) ? (b) : 1))   /* 拥有四舍五入功能的整形除法 */

#define ISZERO(a)               (((a) >= -1e-10) && ((a) <= 1e-10))

#define U8TOU32(u24, u16, u8, u0)   ((((UINT32)(u24) & 0xFF) << 24) |   \
    (((UINT32)(u16) & 0xFF) << 16) | (((UINT32)(u8) & 0xFF) << 8) | ((UINT32)(u0) & 0xFF))

#define U8TOU64(u56, u48, u40, u32, u24, u16, u8, u0)                   \
    ((((UINT64)(u56) & 0xFF) << 56) | (((UINT64)(u48) & 0xFF) << 48) |  \
     (((UINT64)(u40) & 0xFF) << 40) | (((UINT64)(u32) & 0xFF) << 32) |  \
     (((UINT64)(u24) & 0xFF) << 24) | (((UINT64)(u16) & 0xFF) << 16) |  \
     (((UINT64)( u8) & 0xFF) <<  8) |  ((UINT64)( u0) & 0xFF))

#define BYTE_TO_KB(x)   ((x) >> 10)
#define BYTE_TO_MB(x)   ((x) >> 20)
#define BYTE_TO_GB(x)   ((UINT64)(x) >> 30)
#define KB_TO_BYTE(x)   ((x) << 10)
#define MB_TO_BYTE(x)   ((x) << 20)
#define GB_TO_BYTE(x)   ((UINT64)(x) << 30)

/* returns the n'th least significant byte */
#define _GET_BYTE(x, n) (((x) >> (8 * (n))) & 0xFF)
#define _PED_SWAP16(x)  ((_GET_BYTE(x, 0) << 8) + (_GET_BYTE(x, 1) << 0))
#define _PED_SWAP32(x)  ((_GET_BYTE(x, 0) << 24) + (_GET_BYTE(x, 1) << 16) +        \
    (_GET_BYTE(x, 2) << 8) + (_GET_BYTE(x, 3) << 0))
#define _PED_SWAP64(x)  ((_GET_BYTE(x, 0) << 56) + (_GET_BYTE(x, 1) << 48) +        \
    (_GET_BYTE(x, 2) << 40) + (_GET_BYTE(x, 3) << 32) + (_GET_BYTE(x, 4) << 24) +   \
    (_GET_BYTE(x, 5) << 16) + (_GET_BYTE(x, 6) << 8)  + (_GET_BYTE(x, 7) << 0))
#define PED_SWAP16(x)   ((UINT16)_PED_SWAP16((UINT16)(x)))
#define PED_SWAP32(x)   ((UINT32)_PED_SWAP32((UINT32)(x)))
#define PED_SWAP64(x)   ((UINT64)_PED_SWAP64((UINT64)(x)))

#if (__BYTE_ORDER == __LITTLE_ENDIAN)
#define PED_CPU_TO_LE16(x)  (x)
#define PED_CPU_TO_BE16(x)  PED_SWAP16(x)
#define PED_CPU_TO_LE32(x)  (x)
#define PED_CPU_TO_BE32(x)  PED_SWAP32(x)
#define PED_CPU_TO_LE64(x)  (x)
#define PED_CPU_TO_BE64(x)  PED_SWAP64(x)
#define PED_LE16_TO_CPU(x)  (x)
#define PED_BE16_TO_CPU(x)  PED_SWAP16(x)
#define PED_LE32_TO_CPU(x)  (x)
#define PED_BE32_TO_CPU(x)  PED_SWAP32(x)
#define PED_LE64_TO_CPU(x)  (x)
#define PED_BE64_TO_CPU(x)  PED_SWAP64(x)
#else /* !(__BYTE_ORDER == __LITTLE_ENDIAN) */
#error "***Error: Sorry, Not __LITTLE_ENDIAN"
#define PED_CPU_TO_LE16(x)  PED_SWAP16(x)
#define PED_CPU_TO_BE16(x)  (x)
#define PED_CPU_TO_LE32(x)  PED_SWAP32(x)
#define PED_CPU_TO_BE32(x)  (x)
#define PED_CPU_TO_LE64(x)  PED_SWAP64(x)
#define PED_CPU_TO_BE64(x)  (x)
#define PED_LE16_TO_CPU(x)  PED_SWAP16(x)
#define PED_BE16_TO_CPU(x)  (x)
#define PED_LE32_TO_CPU(x)  PED_SWAP32(x)
#define PED_BE32_TO_CPU(x)  (x)
#define PED_LE64_TO_CPU(x)  PED_SWAP64(x)
#define PED_BE64_TO_CPU(x)  (x)
#endif /* !(__BYTE_ORDER == __LITTLE_ENDIAN) */

#define GET16(p)        (UINT16)((((UINT8 *)(p))[1] << 8) + (((UINT8 *)(p))[0]))
#define GET32(p)        (UINT32)((((UINT8 *)(p))[3] << 24) + (((UINT8 *)(p))[2] << 16) + \
    (((UINT8 *)(p))[1] << 8) + (((UINT8 *)(p))[0]))
#define GET64(p)        (UINT64)((((UINT8 *)(p))[7] << 56) + (((UINT8 *)(p))[6] << 48 ) + \
    (((UINT8 *)(p))[5] << 40) + (((UINT8 *)(p))[4] << 32) + (((UINT8 *)(p))[3] << 24 ) + \
    (((UINT8 *)(p))[2] << 16) + (((UINT8 *)(p))[1] << 8) + (((UINT8 *)(p))[0]))
#define SET16(p, v)     ((UINT8 *)(p))[0] = ((v)) & 0xFF; ((UINT8 *)(p))[1] = ((v) >> 8) & 0xFF
#define SET32(p, v)     ((UINT8 *)(p))[0] = ((v)) & 0xFF; ((UINT8 *)(p))[1] = ((v) >> 8) & 0xFF; \
    ((UINT8 *)(p))[2] = ((v) >> 16) & 0xFF; ((UINT8 *)(p))[3] = ((v) >> 24) & 0xFF
#define SET64(p, v)     ((UINT8 *)(p))[0] = ((v)) & 0xFF; ((UINT8 *)(p))[1] = ((v) >> 8) & 0xFF; \
    ((UINT8 *)(p))[2] = ((v) >> 16) & 0xFF; ((UINT8 *)(p))[3] = ((v) >> 24) & 0xFF; \
    ((UINT8 *)(p))[4] = ((v) >> 32) & 0xFF; ((UINT8 *)(p))[5] = ((v) >> 40) & 0xFF; \
    ((UINT8 *)(p))[6] = ((v) >> 48) & 0xFF; ((UINT8 *)(p))[7] = ((v) >> 56) & 0xFF
#define U64HICASTU32(a) ((UINT64)(a) >> 32)
#define U64LOCASTU32(a) ((UINT64)(a) & 0xFFFFFFFF)

/*  Macro to check expected size of struct at compile time using a */
/*  dummy typedef.  On size mismatch, compiler reports a negative array */
/*  size.  If you see an error message of this form, it means that the */
/*  #pragma pack(1) pragma below is not having the desired effect on */
/*  your compiler. */
#define ASSERT_SIZEOF_STRUCT(s, n)          typedef char assert_sizeof_struct_##s[MUX(sizeof(struct s) == (n), 1, -1)]
#define ASSERT_SIZEOF_TYPEDEF_STRUCT(s, n)  typedef char assert_sizeof_typedef_struct_##s[MUX(sizeof(s) == (n), 1, -1)]
#define ASSERT_ENUM_VLAUE(e, v)             typedef char assert_enum_vaule_##e[((UINT32)(e) == (UINT32)(v)) ? 1 : (-1)]
#define ASSERT_NUM_BE_VLAUE(be, v)          typedef char assert_num_vaule_##be[((UINT32)(be) >= (UINT32)(v)) ? 1 : (-1)]

#define MACRO_STRINGIFY(s)  #s
#define XMACRO_STRINGIFY(s) MACRO_STRINGIFY(s)
#ifdef XRS_SRC_REVISION
#define GET_XRS_SRC_REVISION(mn) do {                                        \
    char ver[BUFF_SIZE_1024] = {0}; snprintf(ver, sizeof(ver) - 1, "%s\n",  \
    XMACRO_STRINGIFY(mn)" : "XMACRO_STRINGIFY(XRS_SRC_REVISION)" @ "         \
    XMACRO_STRINGIFY(__DATE__)", "XMACRO_STRINGIFY(__TIME__)); } while(0)
#else
#define GET_XRS_SRC_REVISION(mn)
#endif /* SVN_VERSION_STRING */

/******************************************************************************
 * 错误代码定义: 类型为SINT32,错误代码必须是负数,所以最高位置1,32位定义如下:
 *  +-+-----+-------+---------------+---------------+---------------+
 *  |1|0 9 8|7 6 5 4|3 2 1 0 9 8 7 6|5 4 3 2 1 0 9 8|7 6 5 4 3 2 1 0|
 *  +-+-----+-------+---------------+---------------+---------------+
 *  |1|LEVEL|   R   |    COMPID     |            ERRORNO            |
 *  +-+-----+-------+---------------+---------------+---------------+
 * 这里: LEVEL:错误级别,R:保留位,COMPID:模块号,ERRORNO:错误号
 * 用户模块错误号从32开始定义,当返回错误时,采用ERRNO(ErrNo, CompId)形式
 * 错误代码采用枚举定义,格式为COMP_ERROR_XXX,全部大写,各模块按此标准定义
 *****************************************************************************/
#define ERRNO_L(Level, Error, Id)   (0x80000000 | ((Level) << 28) | ((Id) << 16) | (Error))

/*
 * 错误级别
 * ERROR_LEVEL_MODULE: 单独某一模块重启后就可以解决该问题
 * ERROR_LEVEL_SYSTEM: 整个应用软件系统重启才可以解决该问题
 * ERROR_LEVEL_OS: 机器重启才可以解决该问题
 */
#define ERROR_LEVEL_MODULE  0   /* 模块级别错误 */
#define ERROR_LEVEL_SYSTEM  1   /* 系统级别错误 */
#define ERROR_LEVEL_OS      2   /* 操作系统级别错误 */

#define GET_ERRNO(ErrNo)    ((ErrNo) & 0x0000FFFF)
#define GET_COMPID(ErrNo)   (((ErrNo) >> 16) & 0xFF)
#define GET_COMPSTR(CompId) MUX(CompId < COMP_MAX, COMPSTR[CompId], "UNKNOWN")
#define ERRNO(Error, Id)    ERRNO_L(ERROR_LEVEL_MODULE, Error, Id)

#define SWVER_BUILD_STR     "V%d.%d.%d build%04d%02d%02d [Build Time: %s %s]"
#define SWVER_BUILD_FMT(S)  (S).majver, (S).secver, (S).minver, (S).year + 2000, (S).month, (S).day, __DATE__, __TIME__
#define TRACE_SWVER(C, S)   printf("COMP_%s: " SWVER_BUILD_STR "\n", GET_COMPSTR(C), SWVER_BUILD_FMT(S))

/******************************************************************************
 * 公共命令定义: 用于模块xxx_ioctrl(..., Cmd, ...)中Cmd的定义
 * CMD域中0~31为公共命令,32~255可由模块自定义
 * 控制命令采用枚举定义,格式为COMP_CMD_XXX,全部大写,各模块按此标准定义
 *****************************************************************************/
#define CMDNO(Cmd, Id)  (((Id) << 16) | (Cmd))

/* 打印方式 */
typedef enum
{
    PRN_MANNER_NULL = 0,
    PRN_MANNER_TERM,
    PRN_MANNER_SYSLOG,
    PRN_MANNER_MAX
} PRN_MANNER_E;

/* 打印级别判断方式 */
typedef enum
{
    PRN_SHOW_LVL_LEQ = 0, /* 打印小于等于设置的级别 */
    PRN_SHOW_LVL_EQL,     /* 只打印和设置相等的级别 */
    PRN_SHOW_LVL_BGT,     /* 打印大于设置的级别 */
    PRN_SHOW_MAX
} PRN_SHOW_E;

/* 打印级别 */
typedef enum
{
    PRN_LVL_NONE,       /* NONE */
    PRN_LVL_ERR,        /* 错误(红色) */
    PRN_LVL_WARNING,    /* 警告(黄色) */
    PRN_LVL_WORKFLOW,   /* 流程(绿色) */
    PRN_LVL_INFO,       /* 信息 */
    PRN_LVL_DBG0,       /* 调试 */
    PRN_LVL_DBG1,       /* 调试 */
    PRN_LVL_DBG2,       /* 调试 */
    PRN_LVL_DBG3,       /* 调试 */
    PRN_LVL_DBG4,       /* 调试 */
    PRN_LVL_DBG5,       /* 调试 */
    PRN_LVL_MAX
} PRN_LVL_E;

#ifdef __GNUC__
#define MASK_NO_USED_WARNING            __attribute__((unused)) /* 屏蔽变量或函数未使用告警,慎重使用 */

/* 基础类型原子操作,在多线程情况共享下全局量用这几个宏来算术运算,以保证结果正确,或者也可加互斥锁 */
#define INCREMENT_ATOMIC(var, add_val)  __sync_add_and_fetch(&(var), add_val)
#define DECREMENT_ATOMIC(var, sub_val)  __sync_sub_and_fetch(&(var), sub_val)
#define OR_ATOMIC(var, or_val)          __sync_or_and_fetch(&(var), or_val)
#define AND_ATOMIC(var, and_val)        __sync_and_and_fetch(&(var), and_val)
#define XOR_ATOMIC(var, xor_val)        __sync_xor_and_fetch(&(var), xor_val)
#define NAND_ATOMIC(var, nand_val)      __sync_nand_and_fetch(&(var), nand_val) /* var = ~(var & nand_val)*/
#else
#define MASK_NO_USED_WARNING
/* 需要使用对应的实现 */
#define INCREMENT_ATOMIC(var, add_val)  __sync_add_and_fetch(&(var), add_val)
#define DECREMENT_ATOMIC(var, sub_val)  __sync_sub_and_fetch(&(var), sub_val)
#define OR_ATOMIC(var, or_val)          __sync_or_and_fetch(&(var), or_val)
#define AND_ATOMIC(var, and_val)        __sync_and_and_fetch(&(var), and_val)
#define XOR_ATOMIC(var, xor_val)        __sync_xor_and_fetch(&(var), xor_val)
#define NAND_ATOMIC(var, nand_val)      __sync_nand_and_fetch(&(var), nand_val) /* var = ~(var & nand_val)*/
#endif /* __GNUC__ */

/* 闰年判断,能被4整除且不能被100整除,或者能被400整除 */
#define IS_LEAP_YEAR(y) (((0 == ((y) & 3)) & (((y) % 100) != 0)) | (0 == ((y) % 400)))

/******************************************************************************
 * 修改提示字符颜色,对应颜色表
 * none         = "\033[0m"
 * black        = "\033[0;30m"
 * dark_gray    = "\033[1;30m"
 * blue         = "\033[0;34m"
 * light_blue   = "\033[1;34m"
 * green        = "\033[0;32m"
 * light_green -= "\033[1;32m"
 * cyan         = "\033[0;36m"
 * light_cyan   = "\033[1;36m"
 * red          = "\033[0;31m"
 * light_red    = "\033[1;31m"
 * purple       = "\033[0;35m"
 * light_purple = "\033[1;35m"
 * brown        = "\033[0;33m"
 * yellow       = "\033[1;33m"
 * light_gray   = "\033[0;37m"
 * white        = "\033[1;37m"
 *****************************************************************************/
#define COLOR_STR_NONE          "\033[0m"
#define COLOR_STR_TWINKLE       "\033[5m"
#define COLOR_STR_BLACK         "\033[0;30m"
#define COLOR_STR_LIGHT_GRAY    "\033[0;37m"
#define COLOR_STR_DARK_GRAY     "\033[1;30m"
#define COLOR_STR_BLUE          "\033[0;34m"
#define COLOR_STR_LIGHT_BLUE    "\033[1;34m"
#define COLOR_STR_GREEN         "\033[0;32m"
#define COLOR_STR_LIGHT_GREEN   "\033[1;32m"
#define COLOR_STR_CYAN          "\033[0;36m"
#define COLOR_STR_LIGHT_CYAN    "\033[1;36m"
#define COLOR_STR_RED           "\033[0;31m"
#define COLOR_STR_LIGHT_RED     "\033[1;31m"
#define COLOR_STR_PURPLE        "\033[0;35m"
#define COLOR_STR_LIGHT_PURPLE  "\033[1;35m"
#define COLOR_STR_BROWN         "\033[0;33m"
#define COLOR_STR_YELLOW        "\033[1;33m"
#define COLOR_STR_WHITE         "\033[1;37m"

#define TIME_STR                "[%04d-%02d-%02d %02d:%02d:%02d]"
#define SYSTIME4TM_FMT(SysTm)   (SysTm).year + 2000, (SysTm).month,         \
    (SysTm).day, (SysTm).hour, (SysTm).min, (SysTm).sec
#define STUTIME4TM_FMT(StuTm)   (StuTm).tm_year + 1900, (StuTm).tm_mon + 1, \
    (StuTm).tm_mday, (StuTm).tm_hour, (StuTm).tm_min, (StuTm).tm_sec

#define GET_DATE_YEAR   ((((__DATE__[7] - '0') * 10 + (__DATE__[8] - '0')) * 10 +   \
    (__DATE__[9] - '0')) * 10 + (__DATE__[10] - '0'))
#define GET_DATE_MONTH  (__DATE__[2] == 'n' ? (__DATE__[1] == 'a' ? 0 : 5) :        \
    __DATE__[2] == 'b' ? 1 : __DATE__[2] == 'r' ? (__DATE__[0] == 'M' ? 2 : 3) :    \
    __DATE__[2] == 'y' ? 4 : __DATE__[2] == 'l' ? 6 :                               \
    __DATE__[2] == 'g' ? 7 : __DATE__[2] == 'p' ? 8 :                               \
    __DATE__[2] == 't' ? 9 : __DATE__[2] == 'v' ? 10 : 11)
#define GET_DATE_DAY    ((__DATE__[4] == ' ' ? 0 : __DATE__[4] - '0') * 10 + (__DATE__[5] - '0'))

typedef enum
{
    XRS_SORT_ORDER,      /* 正序 */
    XRS_SORT_REVERSE,    /* 倒序 */
    XRS_SORT_BUTT
} XRS_SORT_TYPE_E;

typedef enum
{
    XRS_ROTATION_0 = 0, 
    XRS_ROTATION_90, 
    XRS_ROTATION_180, 
    XRS_ROTATION_270, 
    XRS_ROTATION_BUTT
} XRS_ROTATION_E;

/* HV Flip 模式 */
typedef enum
{
	XRS_FLIP_NORMAL_MODE = 0,	    /* 正常模式 */
	XRS_FLIP_H_MODE,				/* 镜像模式 */
	XRS_FLIP_V_MODE,				/* 翻转模式 */
	XRS_FLIP_HV_MODE,			    /* 镜像翻转模式 */
	XRS_FLIP_MODE_BUTT,             /* 用于判断参数的有效性，参数大小必须小于这个值 */
} XRS_HVFLIP_E;

/* 模块ID定义 */
typedef enum
{
    COMP_COMMON     = 0,
    COMP_VIDEOIN,
    COMP_VIDEOOUT,
    COMP_VIDEOENC,
    COMP_VIDEODEC,
    COMP_AUDIOIN,
    COMP_AUDIOOUT,
    COMP_AUDIOENC,
    COMP_AUDIODEC,
    COMP_RECORD,
    COMP_PLAYBACK   = 10,
    COMP_STATMACH,
    COMP_NETMS,
    COMP_UPGRADE,
    COMP_WEBSERV,
    COMP_SERIAL,
    COMP_PTZ,
    COMP_GUI,
    COMP_FIFO,
    COMP_EVENT,
    COMP_MSSTD      = 20,
    COMP_LINK,
    COMP_TIMER,
    COMP_MSG,               /* 消息队列 */
    COMP_SEM,               /* 进程间信号灯 */
    COMP_SHM,               /* 进程间共享内存 */
    COMP_NTP,
    COMP_PPPOE,
    COMP_SYS,
    COMP_NETWORK,
    COMP_DAEMON     = 30,   /* 守护进程 */
    COMP_EXTCMD,
    COMP_CMS,               /* 集中管理平台 */
    COMP_NETAPI,            /* 网络接口模块 */
    COMP_SMTPCLI,           /* SMTPCLI模块 */
    COMP_VIDEOAD,           /* 视频AD模块 */
    COMP_PCIV,              /* 主从片间通讯PCIV模块 */
    COMP_CDRAPP,            /* 刻录模块 */
    COMP_PCIVMSG,           /* PCIV模块 */
    COMP_VIDEOCFG,          /* 视频基础模块的配置模块 */
    COMP_SOFTRAID   = 40,
    COMP_VIDEOVOSD,         /* 视频基础模块的OSD模块 */
    COMP_FTPUPGRADE,        /* FTP升级模块 */
    COMP_FONT,
    COMP_LIST,
    COMP_XRSSSSMART,
    COMP_XRSSSCOMM,
    COMP_XRSSSFAT32,
    COMP_XRSSSDEV,
    COMP_XRSSSMGR,
    COMP_XRSSSLOG    = 50,
    COMP_XRSSSSTORE,
    COMP_XRSSSCTRL,
    COMP_XRSSSQUERY,
    COMP_XRSSSPLAYBACK,
    COMP_XRSSSBACKUP,
    COMP_VIDEOVPSS,
    COMP_VIDEOVDA,
    COMP_SDKDAEMON,         /* 守护线程 */
    COMP_CONFIG,
    COMP_HALMOD     = 60,   /* 硬件抽象层模块 */
    COMP_TOAVI,
    COMP_MEMMANAGE,         /* 内存管理模块 */
    COMP_GUITRANS,          /* GUI转换层 */
    COMP_TOMP4,             /* MP4转换模块 */
    COMP_PROCCOMM,          /* PROC_COMM模块 */
    COMP_XRSSSFILE,          /* XRSSS_FILE模块 */
    COMP_UPNP,              /* UPNP模块 */
    COMP_IPDEV,             /* IPDEV模块 */
    COMP_RTSP,              /* RTSP模块 */
    COMP_FTPAPI     = 70,   /* FTP应用接口模块 */
    COMP_XRSSSMKFS,          /* XRSSS_MKFS模块 */
    COMP_FLASHLOG,          /* 写FLASHLOG模块 */
    COMP_SSTRING,           /* SSTRING模块 */
    COMP_THREADPOOL,        /* THREADPOOL模块 */
    COMP_MPOOL,             /* 内存池MPOOL模块 */
    COMP_RTMP,              /* RTMP模块 */
    COMP_ISAPI,             /* ISAPI模块 */
    COMP_AUDIOCFG,          /* 音频基础模块的配置模块 */
    COMP_POECTRL,           /* POE功率控制模块 */
    COMP_FTPUPLOAD  = 80,   /* FTP主动上传模块 */
    COMP_PROXY,             /* 执行代理服务 */
    COMP_SDKDISK,           /* SDK DISK */
    COMP_GB28181,           /* GB28181国标平台 */
    COMP_ONVIF,             /* ONVIF协议模块 */
    COMP_SDKUART,           /* SDK串口模块 */
    COMP_SDKNET,            /* SDK网络模块 */
    COMP_SDKMANAGE,         /* SDK管理模块 */
    COMP_SDKMEDIA,          /* SDK媒体模块 */
    COMP_SDKIPM,            /* SDK IP管理模块 */
    COMP_SDKSTRM    = 90,   /* SDK流管理模块 */
    COMP_SLIVE,             /* SLIVE模块 */
    COMP_AACENC,            /* AAC编码模块 */
    COMP_AACDEC,            /* AAC解码模块 */
    COMP_OEM_CUSTOM,        /* OEM客户定制模块 */
    COMP_NETFILTER,         /*  */
    COMP_DRIVE_GOOGLE,      /* Google Drive存储 */
    COMP_FACE_SNAP,         /* 人脸抓拍模块 */
    COMP_MBLOCK,            /* 内存块MBLOCK模块 */
    COMP_DETECTOR,          /* 实时检测模块 */
    COMP_LIBJPEG_9C = 100,  /* libjpeg-9c模块 */
    COMP_JPGTRANS,          /* jpgtrans模块 */
    COMP_MPEG_TS,           /* MPEG_TS模块 */
    COMP_SADP,              /* SADP模块 */
    COMP_FACE_ZW,           /* 卓威人脸抓拍 */
    COMP_FACE_TD,           /* 天地人脸抓拍 */
    COMP_PLATE_ZENITH,      /* 臻识车牌识别 */
    COMP_G722CODEC,         /* G722CODEC模块 */
    COMP_ZWP2P,             /* 卓威P2P模块 */
    COMP_RTSPSRV,           /* RTSPSRV模块 */
    COMP_WEATHER    = 110,  /* 天气预报模块 */
    COMP_OSW,               /* ONVIF_SWITCH模块 */
    COMP_JPEGTURBO,         /* JPEGTURBO模块 */
    COMP_XRSSS_AIFS,         /* XRSSS_AIFS模块 */
    COMP_VIDEOISP,          /* 视频ISP模块 */
    COMP_SENSOR,            /* SENSOR模块 */
    COMP_MEGCONN,           /* MEGCONN模块 */
    COMP_QRCODE,            /* QRCODE二维码模块 */
    COMP_MAUTH,             /* MAUTH模块 */
    COMP_PTESTTOOL,         /* PTESTTOOL产测工具模块 */
    COMP_TZ         = 120,  /* TZ时区模块 */
    COMP_ONENET,            /* ONENET模块 */
    COMP_XRSSSFEATURE,		/* 人脸特征模块 */
    COMP_THERM,             /* 测温模块 */
    COMP_BLUETOOTH,         /* BLUETOOTH模块 */
	COMP_WXMINIAPP,			/* 微信小程序 */
	COMP_WXWORK,            /* 企业微信程序 */
	COMP_CMHXQ,             /* 中移和小区 */
	COMP_WSWEBAPI,          /* WEB SOCKET WEBAPI */
	COMP_ANTIEPIDEMIC,      /* 防疫平台 */
    COMP_MEGLINK,           /* MEGLINK模块 */
    COMP_VAUTORATE,         /* 码率自适应模块 */
    COMP_LCD,               /* LCD模块 */
    COMP_MAX
} COMPID, *PCOMPID;

/* 模块名称定义 */
static const char *const COMPSTR[] MASK_NO_USED_WARNING =
{
    "COMMON",           /* (0) */
    "VIDEOIN",
    "VIDEOOUT",
    "VIDEOENC",
    "VIDEODEC",
    "AUDIOIN",
    "AUDIOOUT",
    "AUDIOENC",
    "AUDIODEC",
    "RECORD",
    "PLAYBACK",         /* (10) */
    "STATMACH",
    "NETMS",
    "UPGRADE",
    "WEBSERV",
    "SERIAL",
    "PTZ",
    "GUI",
    "FIFO",
    "EVENT",
    "MSSTD",            /* (20) */
    "LINK",
    "TIMER",
    "MSG",              /* 消息队列 */
    "SEM",              /* 进程间信号灯 */
    "SHM",              /* 进程间共享内存 */
    "NTP",
    "PPPOE",
    "SYS",
    "NETWORK",
    "DAEMON",           /* 守护进程(30) */
    "EXTCMD",
    "CMS",
    "NETAPI",
    "SMTPCLI",          /* SMTPCLI模块 */
    "VIDEOAD",
    "PCIV",
    "CDRAPP",           /* 刻录模块 */
    "PCIVMSG",
    "VIDEOCFG",
    "SOFTRAID",         /* (40) */
    "VIDEOOSD",
    "FTPUPGRADE",
    "FONT",
    "LIST",
    "XRSSSSMART",
    "XRSSSCOMM",
    "XRSSSFAT32",
    "XRSSSDEV",
    "XRSSSFS",
    "XRSSSLOG",          /* (50) */
    "XRSSSSTORE",
    "XRSSSCTRL",
    "XRSSSQUERY",
    "XRSSSPLAYBACK",
    "XRSSSBACKUP",
    "VIDEOVPSS",
    "VIDEOVDA",
    "SDKDAEMON",        /* 守护线程 */
    "CONFIG",
    "HALMOD",           /* (60) */
    "TOAVI",
    "MEMMANAGE",
    "GUITRANS",
    "TOMP4",
    "PROCCOMM",         /* PROC_COMM模块 */
    "XRSSSFILE",         /* XRSSS_FILE模块 */
    "UPNP",             /* UPNP模块 */
    "IPDEV",            /* IPDEV模块 */
    "RTSP",             /* RTSP模块 */
    "FTPAPI",           /* FTP应用接口模块(70) */
    "XRSSSMKFS",         /* XRSSS_MKFS模块 */
    "FLASHLOG",         /* 写FLASHLOG模块 */
    "SSTRING",          /* SSTRING模块 */
    "THREADPOOL",       /* THREADPOOL模块 */
    "MPOOL",            /* 内存池MPOOL */
    "RTMP",             /* RTMP模块 */
    "ISAPI",            /* ISAPI模块 */
    "AUDIOCFG",         /* 音频基础模块的配置模块 */
    "POECTRL",          /* POE功率控制 */
    "FTPUPLOAD",        /* FTP主动上传模块(80) */
    "PROXY",            /* 执行代理 */
    "SDKDISK",          /* SDK DISK */
    "GB28181",          /* GB28181国标平台 */
    "ONVIF_DISCOVER",   /* ONVIF DISCOVER */
    "SDKUART",          /* SDK串口模块 */
    "SDKNET",           /* SDK网络模块 */
    "SDKMANAGE",        /* SDK管理模块 */
    "SDKMEDIA",         /* SDK媒体模块 */
    "SDKIPM",           /* SDK IP管理模块 */
    "SDKSTRM",          /* SDK流管理模块(90) */
    "SLIVE",            /* SLIVE模块 */
    "AACENC",           /* AAC编码模块 */
    "AACDEC",           /* AAC解码模块 */
    "OEM_CUSTOM",       /* OEM客户定制模块 */
    "NETFILTER",        /* */
    "DRIVE_GOOGLE",     /* Google Drive存储 */
    "FACE_SNAP",  		/* 人脸抓拍模块 */
    "MBLOCK",           /* 内存块MBLOCK模块 */
    "DETECTOR",         /* 实时检测模块 */
    "LIBJPEG_9C",       /* libjpeg-9c模块(100) */
    "JPGTRANS",         /* jpgtrans模块 */
    "MPEG_TS",          /* MPEG_TS模块 */
    "SADP",             /* SADP模块 */
    "FACE_ZW",          /* 卓威人脸抓拍 */
    "FACE_TD",          /* 天地人脸抓拍 */
    "PLATE_ZENITH",     /* 臻识车牌识别 */
    "G722CODEC",        /* G722CODEC模块 */
    "ZWP2P",            /* 卓威P2P模块 */
    "RTSPSRV",          /* RTSPSRV模块 */
    "WEATHER",          /* 天气预报模块(110) */
    "OSW",              /* ONVIF_SWITCH模块 */
    "JPEGTURBO",        /* JPEGTURBO模块 */
    "XRSSS_AIFS",        /* XRSSS_AIFS模块 */
    "VIDEOISP",         /* 视频ISP模块 */
    "SENSOR",           /* SENSOR模块 */
    "MEGCONN",          /* MEGCONN模块 */
    "QRCODE",           /* QRCODE二维码模块 */
    "MAUTH",            /* MAUTH模块 */
    "PTESTTOOL",        /* PTESTTOOL产测工具模块 */
    "TZ",               /* TZ时区模块(120) */
    "ONENET",           /* ONENET模块 */
    "XRSSSFEATURE",		/* 人脸特征模块 */
    "THERM",            /* 测温模块 */
    "BLUETOOTH",        /* BLUETOOTH模块 */
	"WXMINIAPP",		/* 微信小程序 */
    "WXWORK",           /* 企业微信程序 */
	"CMHXQ",            /* 中移和小区 */
    "WSWEBAPI",         /* WEB SOCKET WEBAPI */
    "ANTIEPIDEMIC",     /* 防疫平台 */
    "MEGLINK",          /* MEGLINK模块 */
    NULL                /* 字符串结束 */
};

typedef enum
{
    COMMON_ERROR_HANDLEINVAL = 1,       /* 句柄描述字非法 */
    COMMON_ERROR_HANDLEOPEND,           /* 句柄已打开 */
    COMMON_ERROR_NOCMD,                 /* 命令不允许 */
    COMMON_ERROR_PARAM,                 /* 参数错误 */
    COMMON_ERROR_NOMEM,                 /* 没有足够内存 */
    COMMON_ERROR_OPEN,                  /* 打开文件错误 */
    COMMON_ERROR_READ,                  /* 读取文件数据错误 */
    COMMON_ERROR_LSEEK,                 /* 定位文件指针错误 */
    COMMON_ERROR_WRITE,                 /* 写入文件数据错误 */
    COMMON_ERROR_FSYNC,                 /* 同步文件数据错误 */
    COMMON_ERROR_CLOSE,                 /* 关闭文件错误 */
    COMMON_ERROR_FCNTL, 
    COMMON_ERROR_IOCTL, 
    COMMON_ERROR_THREADSTART,           /* 线程启动错误 */
    COMMON_ERROR_THREADSTARTED,         /* 线程已启动 */
    COMMON_ERROR_SETTHREADNAME,         /* 设置线程名错误 */
    COMMON_ERROR_SETPTHREADAFFINITY,    /* 设置线程运行属性错误 */
    COMMON_ERROR_FILE_OPEN,             /* 打开文件错误 */
    COMMON_ERROR_FILE_STAT,             /* 获取文件状态错误 */
    COMMON_ERROR_FILE_READ,             /* 读取文件错误 */
    COMMON_ERROR_FILE_WRITE,            /* 写入文件错误 */
    COMMON_ERROR_FILE_CLOSE,            /* 关闭文件错误 */
    COMMON_ERROR_UNLINK, 
    COMMON_ERROR_SYMLINK, 
    COMMON_ERROR_READLINK, 
    COMMON_ERROR_NOSPACE,				/* 当前存储空间不足 */
    COMP_ERROR_START = 32,              /* 用户模块起始错误号 */
} COMMON_ERROR_E;

typedef enum
{
    COMMON_CMD_GETVERSION = CMDNO(0, COMP_COMMON),  /* 获取模块版本 */
    COMMON_CMD_RESET,       /* 复位当前句柄 */
    COMMON_CMD_GETHEALTHY,  /* 获取句柄健康状况,param(SINT32 *),按位表示各通道状态(0-正常,1-故障) */
    COMMON_CMD_GETPID,      /* 获取句柄线程ID,param(SINT32 *) */
    COMMON_CMD_GETPROCINFO, /* 获取各模块的运行信息 */
} COMMON_CMD_E;

typedef struct
{
    SINT32 x, y, w, h;
} XRS_RECT_S, *PXRS_RECT_S;
#define XRS_RECT_S_LEN   sizeof(XRS_RECT_S)
ASSERT_SIZEOF_TYPEDEF_STRUCT(XRS_RECT_S, 16);

typedef struct
{
    SINT32 x, y;
} XRS_POINT_S, *PXRS_POINT_S;
#define XRS_POINT_S_LEN  sizeof(XRS_POINT_S)
ASSERT_SIZEOF_TYPEDEF_STRUCT(XRS_POINT_S, 8);

typedef struct
{
    UINT32 w;
    UINT32 h;
} XRS_SIZE_S, *PXRS_SIZE_S;
#define XRS_SIZE_S_LEN   sizeof(XRS_SIZE_S)
ASSERT_SIZEOF_TYPEDEF_STRUCT(XRS_SIZE_S, 8);

#ifndef __KERNEL__
typedef struct
{
    pid_t Pid;      /* 父进程ID,通过getpid获取 */
    pid_t Tid;      /* 内核态线程ID,通过gettid获取 */
    pthread_t Self; /* 用户态线程ID,通过pthread_self获取 */
} XRS_PID_S, *PXRS_PID_S;
#define XRS_PID_S_LEN    sizeof(XRS_PID_S)
#ifdef POINTER_SIZE_8
ASSERT_SIZEOF_TYPEDEF_STRUCT(XRS_PID_S, 16);
#else
ASSERT_SIZEOF_TYPEDEF_STRUCT(XRS_PID_S, 12);
#endif /* POINTER_SIZE_8 */
#endif /* __KERNEL__ */

typedef struct
{
    SINT32 RtErr;   /* 错误号 */
    SINT32 ErrLine; /* 错误行号 */
} XRS_ERR_S, *PXRS_ERR_S;
#define XRS_ERR_S_LEN    sizeof(XRS_ERR_S)
ASSERT_SIZEOF_TYPEDEF_STRUCT(XRS_ERR_S, 8);

#define XRS_ERR_S_SET(ME, RE, EL)    (ME).RtErr = (RE), (ME).ErrLine = (EL)
#define PXRS_ERR_S_SET(PE, RE, EL)   (PE)->RtErr = (RE), (PE)->ErrLine = (EL)

typedef struct
{
    SINT32 size;
#ifdef POINTER_SIZE_8
    SINT32 Resvd;
#endif /* POINTER_SIZE_8 */
    char *pbuff;
} BUFF_S, *PBUFF_S;
#define BUFF_S_LEN  sizeof(BUFF_S)
#ifdef POINTER_SIZE_8
ASSERT_SIZEOF_TYPEDEF_STRUCT(BUFF_S, 16);
#else
ASSERT_SIZEOF_TYPEDEF_STRUCT(BUFF_S, 8);
#endif /* POINTER_SIZE_8 */

typedef struct
{
    UINT8 *pbuff;
    SINT32 used_size;
    SINT32 free_size;
} BUFF_EXT_S, *PBUFF_EXT_S;
#define BUFF_EXT_S_LEN  sizeof(BUFF_EXT_S)
#ifdef POINTER_SIZE_8
ASSERT_SIZEOF_TYPEDEF_STRUCT(BUFF_EXT_S, 16);
#else
ASSERT_SIZEOF_TYPEDEF_STRUCT(BUFF_EXT_S, 12);
#endif /* POINTER_SIZE_8 */

typedef enum
{
    THREAD_IS_EXIT = 0x00,  /* 线程退出 */
    THREAD_IS_RUNNING,      /* 线程运行 */
    THREAD_IS_STOP,         /* 线程停止 */
} THREAD_STAT_E;

typedef enum
{
    BUFF_SIZE_32  = 32,   /* 字符串长度  32字节 */
    BUFF_SIZE_64  = 64,   /* 字符串长度  64字节 */
    BUFF_SIZE_128 = 128,  /* 字符串长度 128字节 */
    BUFF_SIZE_256 = 256,  /* 字符串长度 256字节 */
    BUFF_SIZE_512 = 512,  /* 字符串长度 512字节 */
    BUFF_SIZE_1024= 1024, /* 字符串长度1024字节 */
} BUFF_SIZE_E;

/* 模块软件版本定义 */
#if (__BYTE_ORDER == __LITTLE_ENDIAN)
typedef struct  /* 系统时间定义 */
{
    UINT32 sec : 6;     /* 秒: 0~63 */
    UINT32 min : 6;     /* 分: 0~63 */
    UINT32 hour : 5;    /* 时: 0~23 */
    UINT32 day : 5;     /* 日: 1~31 */
    UINT32 month : 4;   /* 月: 1~12 */
    UINT32 year : 6;    /* 年: 0~63(从SYSTIME_FORM_YEAR年算起至今的年数) */
} SYSTIME, *PSYSTIME;

typedef struct  /* 软件版本定义 */
{
    UINT32 minver : 6;  /* 0~63 */
    UINT32 secver : 6;  /* 0~63 */
    UINT32 majver : 5;  /* 0~31 */
    UINT32 day : 5;     /* 日: 1~31 */
    UINT32 month : 4;   /* 月: 1~12 */
    UINT32 year : 6;    /* 年: 0~63(从SYSTIME_FORM_YEAR年算起至今的年数) */
} SWVERSION, *PSWVERSION;
#else /* !(__BYTE_ORDER == __LITTLE_ENDIAN) */
#error "***Error: Sorry, Not __LITTLE_ENDIAN"
typedef struct  /* 系统时间定义 */
{
    UINT32 year : 6;    /* 年: 0~63(从SYSTIME_FORM_YEAR年算起至今的年数) */
    UINT32 month : 4;   /* 月: 1~12 */
    UINT32 day : 5;     /* 日: 1~31 */
    UINT32 hour : 5;    /* 时: 0~23 */
    UINT32 min : 6;     /* 分: 0~63 */
    UINT32 sec : 6;     /* 秒: 0~63 */
} SYSTIME, *PSYSTIME;

typedef struct  /* 软件版本定义 */
{
    UINT32 year : 6;    /* 年: 0~63(从SYSTIME_FORM_YEAR年算起至今的年数) */
    UINT32 month : 4;   /* 月: 1~12 */
    UINT32 day : 5;     /* 日: 1~31 */
    UINT32 majver : 5;  /* 0~31 */
    UINT32 secver : 6;  /* 0~63 */
    UINT32 minver : 6;  /* 0~63 */
} SWVERSION, *PSWVERSION;
#endif /* !(__BYTE_ORDER == __LITTLE_ENDIAN) */
#define SYSTIME_LEN     sizeof(SYSTIME)
#define SWVERSION_LEN   sizeof(SWVERSION)
ASSERT_SIZEOF_TYPEDEF_STRUCT(SYSTIME, 4);
ASSERT_SIZEOF_TYPEDEF_STRUCT(SWVERSION, 4);

#define VINFO2STR(Str, SwVer)   sprintf(Str, "V%d.%d.%d build%04d%02d%02d\n",           \
    ((SWVERSION)SwVer).majver, ((SWVERSION)SwVer).secver, ((SWVERSION)SwVer).minver,    \
    ((SWVERSION)SwVer).year + 2000, ((SWVERSION)SwVer).month, ((SWVERSION)SwVer).day)

#define VINFO2STREXT(Str, SwVer, ExtStr, BuildStr) {                                            \
    if (strlen(ExtStr)) {                                                                       \
        sprintf(Str, "V%d.%d.%d.%s %s%04d%02d%02d\n", ((SWVERSION)SwVer).majver,                \
            ((SWVERSION)SwVer).secver, ((SWVERSION)SwVer).minver, ExtStr, BuildStr,             \
            ((SWVERSION)SwVer).year + 2000, ((SWVERSION)SwVer).month, ((SWVERSION)SwVer).day);  \
    } else {                                                                                    \
        sprintf(Str, "V%d.%d.%d %s%04d%02d%02d\n", ((SWVERSION)SwVer).majver,                   \
            ((SWVERSION)SwVer).secver, ((SWVERSION)SwVer).minver, BuildStr,                     \
            ((SWVERSION)SwVer).year + 2000, ((SWVERSION)SwVer).month, ((SWVERSION)SwVer).day);  \
    } }

/* 加密类型 */
typedef enum
{
    ENCRYPT_TYPE_NONE = 0,  /* 未加密 */
    ENCRYPT_TYPE_AES,
    ENCRYPT_TYPE_BUTT,
} ENCRYPT_TYPE_E;

#endif /* _XRSDEF_H_ */
