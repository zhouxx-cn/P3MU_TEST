//DOP_global.h
#ifndef _DOP_GLOBAL_H_
#define _DOP_GLOBAL_H_


#include "idvr_rnl.h"

#include "global_gather.h"
#define MODULENAME "[DOP]"

#define USE_RNL

#ifdef USE_RNL
#define TRACE(x...)   IDVR_RNL_DEBUG(MODULENAME,x)
#define TRACE_WARN(x...)   IDVR_RNL_INFO(MODULENAME,x)
#define TRACE_ERR(x...)   IDVR_RNL_ERROR(MODULENAME,x)
#else
#define USE_TRACE
#define USE_TRACE_WARN
#define USE_TRACE_ERR

#ifdef USE_TRACE
#define TRACE printf("[%s %d %s]",__FILE__,__LINE__,__FUNCTION__);printf
#else
#define TRACE 0?1:printf
#endif


#ifdef USE_TRACE_WARN
#define TRACE_WARN printf("[Warning:%s %d %s]",__FILE__,__LINE__,__FUNCTION__);printf
#else
#define TRACE_WARN 0?1:printf
#endif

#ifdef USE_TRACE_ERR
#define TRACE_ERR printf("[Error:%s %d %s]",__FILE__,__LINE__,__FUNCTION__);printf
#else
#define TRACE_ERR 0?1:printf
#endif

#endif


#ifndef NULL
#ifdef __cplusplus
#define NULL    0
#else
#define NULL    ((void *)0)
#endif
#endif

#endif
