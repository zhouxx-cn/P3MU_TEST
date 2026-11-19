#ifndef __COMTRACE_H_
#define __COMTRACE_H_

#include "RNL/idvr_rnl.h"

/******************************************************************************
 *					串口管理调试宏
 *****************************************************************************/

#define COM_LOG(level, x...) \
do {\
	char buf[512];\
	snprintf(buf, sizeof(buf), x);\
	idvr_rnl_log(level, "COM", "[%s line %d] %s", __FILE__, __LINE__, buf);\
} while (0)

#define COM_DBG(x...)			//IDVR_RNL_DEBUG("COM", "[%s:%d]", __FILE__, __LINE__);IDVR_RNL_DEBUG("COM", x)
#define COM_INFO(x...)	        //COM_LOG(LOG_LEVEL_INFO, x)
#define COM_ERROR(x...)	COM_LOG(LOG_LEVEL_ERROR, x)

#endif
