/*******************************************************************************
     Copyright (c) 2020-2031, XXXX
--------------------------------------------------------------------------------
                              sdk_pub_log.h
  Project Code: log
   Module Name: sdk_pub_log.h
  Date Created: 2020.08.17
        Author: zhujiajia
   Description: 提供打印日志记录接口，可选打印到串口或指定的日志文件

--------------------------------------------------------------------------------
  Modification History
  DATE        NAME             DESCRIPTION
--------------------------------------------------------------------------------
  YYYY-MM-DD

*******************************************************************************/
#ifndef _SDK_PUB_LOG_H_
#define _SDK_PUB_LOG_H_



#ifdef __cplusplus
extern "C"{
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdarg.h>

#define TFCARD_MOUNT	   "/tmp/app/disk/mmcblk0p1"
#define LOG_PATH           "/tmp/app/disk/mmcblk0p1/output/log"   /* 日志存储默认路径 */
#define TFCARD_NODE		   "/dev/mmcblk0p1"
#define LOG_FILE_LINE_MAX  5000                      /* 单个日志文件最大行数 */
#define LOG_PATH_MAX_SZ    256

#define SDK_LOG_VER           1
#define SDK_LOG_MAGIC         0xAAEE

/* 添加日志类型 */
enum
{
    LOG_F_ONE = 0,
    LOG_F_TWO,
    LOG_F_THE,
    LOG_F_FOR,
    LOG_F_FIF,

    LOG_F_BUTT,
};



/* 日志文件描述，写入日志文件头部 */
typedef struct tagLogDescription
{
    unsigned short u16Magic;    /* 魔术字，用于校验格式 */
    unsigned char u8Version;    /* 日志版本 */
    int iLined;      /* 已写入行数 */
    unsigned char  u8RcovrCnt;  /* 覆盖次数 */
}LOG_DESC_S;


/* 如何记录行数 */
typedef struct tagLogCb
{
    pthread_mutex_t stMutex;
    int iPrintLevel;
    /* 日志控制块,后续功能扩展 */
    LOG_DESC_S stLogDescTbl[LOG_F_BUTT];
}LOG_CB_S;

/* 打印等级枚举 */
enum
{
    LEVEL_ERROR = 0,
    LEVEL_WARN,
    LEVEL_LOG,
    LEVEL_INFO,

    /* DEBUG用 */
    LEVEL_DEBUG,
    
    LEVEL_BUTT,
};

/* 输出枚举 */
enum
{
    LOG_STD =  (1 << 0),
    LOG_FILE = (1 << 1),

    LOG_BUTT,
};


#define LOG_PRINT_LEVEL            LEVEL_INFO            /* 当前默认打印等级 */


#ifdef __SUPPORT_SAVE_TF_LOG__
/* 对外示例接口 */
#define TF_GPRS_LOG_INFO(format, args...) \
do{\
    write_tflog(LOG_FILE, LEVEL_INFO, "[GPRS][%s-%d]" format, __FILE__, __LINE__, ##args);\
  }while(0)
#else
#define TF_GPRS_LOG_INFO(format, args...)
#endif

void write_tflog(unsigned     char u8Target, unsigned char u8PrintLevel, const char *format, ...);
void TFLOG_Init();
void TFLOG_SetAllowFlag(int allowflag);


#ifdef  __cplusplus
    }
#endif


#endif

