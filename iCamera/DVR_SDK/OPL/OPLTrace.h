#ifndef __OPL_TRACE_H_
#define __OPL_TRACE_H_

#include <time.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include "RNL/idvr_rnl.h"

#define OPL_LOG(level, x...) \
do {\
	char buf[512];\
	snprintf(buf,sizeof(buf), x);\
	idvr_rnl_log(level, "OPL", "[%s line %d] %s", __FILE__, __LINE__, buf);\
} while (0)

#define OPL_DBG(x...)			//IDVR_RNL_DEBUG("OPL", "[%s:%d]", __FILE__, __LINE__);IDVR_RNL_DEBUG("OPL", x);
#define OPL_INFO(x...)	OPL_LOG(LOG_LEVEL_INFO, x)
#define OPL_ERROR(x...)	OPL_LOG(LOG_LEVEL_ERROR, x)

#if 0

//!\brief 计时开始宏
//!\param 循环执行的次数
#define BEGIN_TIME(count)  \
{\
   struct timeval tv;\
   struct timeval tv_end;\
   int TEST_COUNT = 0;\
   gettimeofday(&tv, NULL);\
   for (TEST_COUNT = 0; TEST_COUNT < count; TEST_COUNT++)\
   {

//!\brief 计时结束宏
#define END_TIME \
   }\
   gettimeofday(&tv_end, NULL);\
   OPL_DBG("usec=%ld\n", tv_end.tv_sec*1000000 + tv_end.tv_usec - tv.tv_sec*1000000 - tv.tv_usec);\
}

#else
#define BEGIN_TIME(count)
#define END_TIME
#endif

#endif
