
/*!
 *\file		global_macro.h
 *\brief	PUB模块，全局宏定义
 *\author	林云
 *\date		2009/03/23
 */

#ifndef _INC_GLOBAL_MACRO_H_
#define _INC_GLOBAL_MACRO_H_

#ifndef _GNU_SOURCE
#define _GNU_SOURCE         /* See feature_test_macros(7) */
#endif

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>

#include <sys/prctl.h>
#include <sys/syscall.h>   /* For SYS_xxx definitions */
#include <sys/select.h>
#include <sys/stat.h>
#ifdef __cplusplus
extern "C"
{
#endif
	#include "xtrace.h"
#ifdef __cplusplus
}
#endif
#ifdef __cplusplus
extern "C"
{
#endif
	#include "xtrace.h"
#ifdef __cplusplus
}
#endif

#include "global_gather.h"
#define _LINUX 1
/*以下三个定义放到Makefile中根据板型各自定义*/
//#define ACTIVE_CODE_SUPPORT 1
//#define AF_DEV_SUPPORT 1
//#define DL_ACTIVE_CODE_SUPPORT 1

//数据类型定义
#ifndef BOOL
#define BOOL	int
#endif


#ifndef NULL
#ifdef __cplusplus
#define NULL    0
#else
#define NULL    ((void *)0)
#endif
#endif

#ifndef FALSE
#define FALSE	0
#endif

#ifndef TRUE
#define TRUE	1
#endif

#if defined(EGUI_FOR_ICAMERA_NXP8850_BASED_DVR)
#define USE_ICAMERA	1
#endif

#ifdef EGUI_FOR_ICAMERA_NXP8850_BASED_DVR
#define USE_HDAUX	1
#define USE_FOURTH_STREAM 1
#endif

#ifndef UNUSED
#define UNUSED(x) (void)(x)
#endif

#ifndef RemoveDir
static inline int RemoveDir(const char *dir)
{
    if (NULL == dir)
    {
        fprintf(stderr, "NULL POINTER EXCEPTION!\n");
        return -1;
    }

    char dir_name[512] = {0};
    DIR *dirp = NULL;
    struct dirent *dp = NULL;
    struct stat dir_stat = {0};

    // 参数传递进来的目录不存在，直接返回
    if ( 0 != access(dir, F_OK) )
    {
        return 0;
    }

    // 获取目录属性失败，返回错误
    if ( 0 > stat(dir, &dir_stat) )
    {
        fprintf(stderr, "get directory stat error: %s\n", strerror(errno));
        return -1;
    }

    if ( S_ISREG(dir_stat.st_mode) ) // 普通文件直接删除
    {
        remove(dir);
    }
    else if ( S_ISDIR(dir_stat.st_mode) ) // 目录文件，递归删除目录中内容
    {
        dirp = opendir(dir);
        while ( (dp=readdir(dirp)) != NULL )
        {
            // 忽略 . 和 ..
            if ( (0 == strcmp(".", dp->d_name)) || (0 == strcmp("..", dp->d_name)) )
            {
                continue;
            }

            snprintf(dir_name, sizeof(dir_name), "%s/%s", dir, dp->d_name);
            RemoveDir(dir_name);   // 递归调用
        }
        closedir(dirp);
        dirp = NULL;
        rmdir(dir); // 删除空目录
    }
    else
    {
        fprintf(stderr, "unknown file type: %s\n", strerror(errno));
    }

    return 0;
}
#endif


#ifdef __IS_SUPPORT_CLOUDWORLD__
#define CLOUD_STORAGE_MODE_DOMESTIC                     0
#define CLOUD_STORAGE_MODE_FOREIGN                      1
#define CLOUD_STORAGE_MODE_UNKNOW  	            		2

//云存模式标志
#define CLOUD_STORAGE_MODE_DOMESTIC_FLAG    "/data/config/config/CloudMode0"
#define CLOUD_STORAGE_MODE_FOREIGN_FLAG     "/data/config/config/CloudMode1"
#endif

#ifndef GetDirSize
static inline int64_t GetDirSize(const char *dir)
{
    if (NULL == dir)
    {
        fprintf(stderr, "NULL POINTER EXCEPTION!\n");
        return -1;
    }

    // 参数传递进来的目录不存在，直接返回
    if ( 0 != access(dir, F_OK) )
    {
        return 0;
    }

    DIR *dp = NULL;
    struct dirent *entry = NULL;
    struct stat statbuf = {0};
    int64_t totalSize = 0;

    if ((dp = opendir(dir)) == NULL)
    {
        fprintf(stderr, "Cannot open dir %s: %s\n", dir, strerror(errno));
        return -1;
    }
    
    // 先加上自身目录的大小
    lstat(dir, &statbuf);
    totalSize += statbuf.st_size;

    while ((entry = readdir(dp)) != NULL)
    {
        char subdir[512] = {0};
        snprintf(subdir, sizeof(subdir), "%s/%s", dir, entry->d_name);
        lstat(subdir, &statbuf);
        
        if (S_ISDIR(statbuf.st_mode))
        {
            if (!strcmp(".", entry->d_name) || !strcmp("..", entry->d_name))
            {
                continue;
            }

            int64_t subDirSize = GetDirSize(subdir);
            totalSize += subDirSize;
        }
        else
        {
            totalSize += statbuf.st_size;
        }
    }

    closedir(dp);
    dp = NULL;

    return totalSize;   
}

#endif

#ifndef IsDirEmpty
static inline int IsDirEmpty(const char *dir)
{
    if (NULL == dir)
    {
        fprintf(stderr, "NULL POINTER EXCEPTION!\n");
        return -1;
    }

    // 参数传递进来的目录不存在，直接返回
    if ( 0 != access(dir, F_OK) )
    {
        return -1;
    }

    int n = 0;
    DIR *dp = NULL;
    struct dirent *entry = NULL;

    if ((dp = opendir(dir)) == NULL)
    {
        fprintf(stderr, "Cannot open dir %s: %s\n", dir, strerror(errno));
        return -1;
    }

    while ((entry = readdir(dp)) != NULL)
    {
        if (++n > 2)
            break;
    }

    closedir(dp);
    dp = NULL;

    if (n <= 2)
    {
        return 1;
    }

    return 0;
}
#endif


/* 为了防止内存泄漏和出现野指针的情况
 * 在使用close, fclose和free的时候，尽量使用如下的安全宏，避免出问题
 */
#ifndef TEST_MEM_INFO
#ifndef SAFE_FREE
#define SAFE_FREE(p)\
{\
	if ((p) != NULL)\
	{\
		free((p));\
		(p) = NULL;\
	}\
}
#endif
#else
#ifndef SAFE_FREE
#define SAFE_FREE(p)\
{\
	if ((p) != NULL)\
	{\
		MemoryFree((p));\
		(p) = NULL;\
	}\
}
#endif
#endif

#ifndef SAFE_FCLOSE
#define SAFE_FCLOSE(fp)\
{\
    if ((fp) != NULL)\
    {\
        fclose((fp));\
        (fp) = NULL;\
    }\
}
#endif

#ifndef SAFE_CLOSE
#define SAFE_CLOSE(fd)\
{\
    if (((fd)) > 0)\
    {\
        close(((fd)));\
        ((fd)) = -1;\
    }\
}
#endif

#ifndef SAFE_UNLINK
#define SAFE_UNLINK(x)\
{\
    if (0 == access((x), F_OK))\
    {\
        unlink((x));\
    }\
}
#endif

#ifndef SAFE_REMOVE
#define SAFE_REMOVE(x)      RemoveDir(x)
#endif

#ifndef GET_DIR_SIZE
#define GET_DIR_SIZE(x)     GetDirSize(x)
#endif

#ifndef IS_DIR_EMPTY
#define IS_DIR_EMPTY(x)     IsDirEmpty(x)
#endif

/* 定义映射表结构体 */
typedef struct
{
	unsigned id;
	const char* desc;
}MY_DESCRIBE_COMM_TYPE;

/* 映射函数 */
static const char* GetDescByID(MY_DESCRIBE_COMM_TYPE *desc, unsigned id) __attribute__((used));
static const char* GetDescByID(MY_DESCRIBE_COMM_TYPE *desc, unsigned id)  
{ 
	int i = 0;

	while (0xffff != desc[i].id)
	{
		if(desc[i].id == id)
		{
			return desc[i].desc;
		}
		i++;
	}

	return "Can't find the ID";
}

/* 采用宏的#处理方法对宏进行封装 */
#define MY_DECRIBE_DECLARE_BEGIN(type) static MY_DESCRIBE_COMM_TYPE  MY_describe_desc_##type[]={
#define MY_DECRIBE_DECLARE_END  {0xffff, "end"} };
#define MY_DECRIBE_ADD_DESC(id,str) {id, #str},
#define MY_DESCRIBE(type, id) GetDescByID(MY_describe_desc_##type,id)

#ifndef PRT_TID
#define PRT_TID()\
    ({prctl(PR_SET_NAME, __FUNCTION__, 0, 0, 0);\
      FILE *tid_log_fp = NULL;\
      tid_log_fp = fopen("/tmp/.tid_log", "a");\
      fprintf(tid_log_fp, "[%s:%d] thread: %s tid: %ld\n", __FILE__, __LINE__, __FUNCTION__, syscall(SYS_gettid));\
      fclose(tid_log_fp);\
      tid_log_fp = NULL;\
    })

#define PRT_TID_NAME(_NAME_)\
    ({prctl(PR_SET_NAME, _NAME_, 0, 0, 0);\
      FILE *tid_log_fp = NULL;\
      tid_log_fp = fopen("/tmp/.tid_log", "a");\
      fprintf(tid_log_fp, "[%s:%d] thread: %s tid: %ld\n", __FILE__, __LINE__, __FUNCTION__, syscall(SYS_gettid));\
      fclose(tid_log_fp);\
      tid_log_fp = NULL;\
    })
#endif

/**
 * \def IDVR_LIL_ENDIAN
 * \brief Little endianness.
 */
#define IDVR_LIL_ENDIAN  1234

#define RTMP_SUPPORT 				(0)
#define AF_SUPPORT 					(1)
#define DOUBLE_LIGHT_SUPPORT 		(3)
#define FACE_CAP_SUPPORT 			(4)
#define FACE_RECO_SUPPORT 			(5)
#define LICENSE_PLATE_RECO_SUPPORT 	(14)
#define EXTERNSIMCARD_SUPPORT    	(15)		//支持外置卡


#define CAPABILITY_RTMP			(1<<RTMP_SUPPORT)
#define CAPABILITY_AF			(1<<AF_SUPPORT)
#define CAPABILITY_DL			(1<<DOUBLE_LIGHT_SUPPORT)
#define CAPABILITY_FC			(1<<FACE_CAP_SUPPORT)
#define CAPABILITY_FR			(1<<FACE_RECO_SUPPORT)
#define CAPABILITY_LPR			(1<<LICENSE_PLATE_RECO_SUPPORT)
#define CAPABILITY_EXTERNSIMCARD (1 << EXTERNSIMCARD_SUPPORT)


#define RTMP_CFG "RTMP_cfg"

/**
 * \def IDVR_BIG_ENDIAN
 * \brief Big endianness.
 */
#define IDVR_BIG_ENDIAN  4321


#if  defined(__i386__) || defined(__ia64__) || (defined(__alpha__) || defined(__alpha)) || defined(__arm__) || (defined(__mips__) && defined(__MIPSEL__)) || defined(__LITTLE_ENDIAN__)
#define IDVR_BYTEORDER   IDVR_LIL_ENDIAN
#else
#define IDVR_BYTEORDER   IDVR_BIG_ENDIAN
#endif
#ifndef iDvrArchSwap16
static inline unsigned short iDvrArchSwap16(unsigned short D)
{
	return ((D << 8) | (D >> 8));
}
#endif
#ifndef iDvrArchSwap32
static inline unsigned int iDvrArchSwap32(unsigned int D)
{
	return ((D << 24) | ((D << 8) & 0x00FF0000) | ((D >> 8) & 0x0000FF00) | (D >> 24));
}
#endif

#ifndef DEFINE_THREAD_NAME
#define DEFINE_THREAD_NAME(x) do{\
    static int index = 0;\
    char thname[16]; \
    snprintf(thname, sizeof(thname), "%s%d", x, __sync_fetch_and_add(&index, 1));\
    prctl(PR_SET_NAME, (unsigned long)thname, 0, 0, 0);\
}while(0);
#endif


/* Byteswap item from the specified endianness to the native endianness */
#if IDVR_BYTEORDER == IDVR_LIL_ENDIAN

/** Swaps a 16-bit little endian integer to the native endianness. */
#define iDvrArchSwapLE16(X)        (X)

/** Swaps a 32-bit little endian integer to the native endianness. */
#define iDvrArchSwapLE32(X)        (X)

/** Swaps a 16-bit big endian integer to the native endianness. */
#define iDvrArchSwapBE16(X)        iDvrArchSwap16(X)

/** Swaps a 32-bit big endian integer to the native endianness. */
#define iDvrArchSwapBE32(X)        iDvrArchSwap32(X)
#else
#define iDvrArchSwapLE16(X)        iDvrArchSwap16(X)
#define iDvrArchSwapLE32(X)        iDvrArchSwap32(X)
#define iDvrArchSwapBE16(X)        (X)
#define iDvrArchSwapBE32(X)        (X)
#endif


#ifndef TimerSeconds
// select实现秒级定时器
static inline void TimerSeconds(unsigned long seconds)
{
	if (seconds <= 0)
	{
		return ;
	}

    int err = -1;
    struct timeval tv = {0, 0};

    tv.tv_sec = seconds;
    tv.tv_usec = 0;

    do
	{
       err = select(0, NULL, NULL, NULL, &tv);
    }while((err<0) && (EINTR==errno));
}
#endif

#ifndef TimerMilliseconds
// select实现毫秒级别定时器
static inline void TimerMilliseconds(unsigned long mSec)
{
	if (mSec <= 0)
	{
		return ;
	}

    int err = -1;
    struct timeval tv = {0, 0};

    tv.tv_sec = mSec / 1000;
    tv.tv_usec = (mSec % 1000) * 1000;

    do
	{
       err = select(0, NULL, NULL, NULL, &tv);
    }while((err<0) && (EINTR==errno));
}
#endif

#ifndef TimerMicroseconds
// select实现微妙级别定时器
static inline void TimerMicroseconds(unsigned long uSec)
{
	if (uSec <= 0)
	{
		return ;
	}

    int err = -1;
    struct timeval tv = {0, 0};

    tv.tv_sec = uSec / 1000000;
    tv.tv_usec = uSec % 1000000;

    do
	{
        err = select(0, NULL, NULL, NULL, &tv);
    }while((err<0) && (EINTR==errno));
}
#endif

/* select精确睡眠(秒) */
#define SelectSleepSec(X)      TimerSeconds(X)
/* select精确睡眠(毫秒) */
#define SelectSleepMSec(X)     TimerMilliseconds(X)
/* select精确睡眠(微妙) */
#define SelectSleepUSec(X)     TimerMicroseconds(X)

/****************************************************************************/


//spot口数目
#define MAX_SPOT_NUM (4)

/*! 字符串表示的时间长度，格式"YYYYmmDDHHMMSS\0"*/
#define REC_TIME_LEN			15


/* 播放模式 */
#define AVP_MODE_NORMAL				0
#define AVP_MODE_BROWSE				1
#define AVP_IS_NORMAL_MODE(x)		((x) == AVP_MODE_NORMAL)
#define AVP_IS_BROWSE_MODE(x)		((x) == AVP_MODE_BROWSE)


/* 播放方向 */
#define AVP_DIRECT_FORWARD			0
#define AVP_DIRECT_BACKWARD			1
#define AVP_IS_FORWARD(x)			((x) == AVP_DIRECT_FORWARD)
#define AVP_IS_BACKWARD(x)			((x) == AVP_DIRECT_BACKWARD)


/* others */
#define SET_BIT(x, i)				((x) |= (1 << i))
#define UNSET_BIT(x, i)				((x) &= ~(1 << i))
#define IS_SET_BIT(x, i)			((x) & (1 << i))

//云台速度上限
#define MOTO_SPEED_H_MAX 9
#define MOTO_SPEED_V_MAX 9

#endif							/* #ifndef _INC_GLOBAL_MACRO_H_ */
