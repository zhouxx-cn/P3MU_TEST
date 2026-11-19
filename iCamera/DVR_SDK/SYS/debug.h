#ifndef __CSYS_DEBUG_H__
#define  __CSYS_DEBUG_H__

#include <stdio.h>
#include "LogManager.h"

#define CSYS_ERR(format...)     LOG_E(LOG_MODULE_SYS, format)
#define CSYS_WRAN(format...)    LOG_W(LOG_MODULE_SYS, format)
#define CSYS_INF(format...)     LOG_I(LOG_MODULE_SYS, format)
#define CSYS_DBG(format...)     LOG_D(LOG_MODULE_SYS, format)

#endif
