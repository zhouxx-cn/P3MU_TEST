#ifndef __XTRACE_H__
#define __XTRACE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "xrsdef.h"

/**
 * @brief 初始化调试追踪功能
 *
 * @param use 是否启用，0 - 不启用， 1 - 启用
 * @param checkms 检测线程检测间隔时间
 * @param timeoutms 互斥锁超时时间，持有或者等待锁超过这个时间则判断为异常
 * @return int 0 - 成功，其他 - 失败
 */
SINT32 XTrace_Init(UINT32 use, UINT32 intervalms, UINT32 timeoutms);
void XTrace_Destory();

SINT32 XTrace_MutexPush(pthread_mutex_t *lock, const char *name, const char *func, int line);
SINT32 XTrace_MutexTryPush(pthread_mutex_t *lock, const char *name, const char *func, int line);
SINT32 XTrace_MutexPop(pthread_mutex_t *lock, const char *name);

#define XTRACE_MUTEX_INIT(mutex, attr) pthread_mutex_init((mutex), (attr))
#define XTRACE_MUTEX_DESTORY(mutex) pthread_mutex_destroy((mutex))
#define XTRACE_MUTEX_TRYLOCK(mutex) XTrace_MutexTryPush((mutex), (#mutex), __FUNCTION__, __LINE__)
#define XTRACE_MUTEX_LOCK(mutex) XTrace_MutexPush((mutex), (#mutex), __FUNCTION__, __LINE__)
#define XTRACE_MUTEX_UNLOCK(mutex) XTrace_MutexPop((mutex), (#mutex))

#ifdef __cplusplus
}
#endif

#endif /* __XTRACE_H__ */