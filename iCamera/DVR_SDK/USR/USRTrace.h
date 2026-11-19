#ifndef __TRACE_H_
#define __TRACE_H_

#include "RNL/idvr_rnl.h"

#ifndef NO_USE_SQLITE3
//!\brief 系统日志宏
#define USR_LOG(level, x...) \
	do {\
	char buf[512] = {0};\
	snprintf(buf, sizeof(buf), x);\
	idvr_rnl_log(level, "USR", "[%s line %d] %s", __FILE__, __LINE__, buf);\
	} while (0)
#else
#define USR_LOG(level, x...) \
	do {\
	char buf[512] = {0};\
	snprintf(buf, sizeof(buf), x);\
	printf("[%s-%d][USR]: %s", __FILE__, __LINE__, buf);\
	} while (0)
#endif

#ifdef __PRINT_DEBUG__
#define USR_DBG(x...)	USR_LOG(LOG_LEVEL_DEBUG, x)
#else
#define USR_DBG(x...)	((void)0)
#endif

#ifdef __PRINT_INFO__
#define USR_INFO(x...)	USR_LOG(LOG_LEVEL_INFO, x)
#else
#define USR_INFO(x...)	((void)0)
#endif

#ifdef __PRINT_ERROR__
#define USR_ERROR(x...)	USR_LOG(LOG_LEVEL_ERROR, x)
#else
#define USR_ERROR(x...)	((void)0)
#endif

//#define USR_DBG(x...) DVR_RNL_DEBUG("USR", "[%s:%d]", __FILE__, __LINE__);IDVR_RNL_DEBUG("USR", x);
//#define USR_INFO(x...)	USR_LOG(LOG_LEVEL_INFO, x)
//#define USR_ERROR(x...)	USR_LOG(LOG_LEVEL_ERROR, x)

#endif

