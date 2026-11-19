#ifndef __PTZTRACE_H_
#define __PTZTRACE_H_

#include <time.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include "RNL/idvr_rnl.h"
#include "LogManager.h"

#define PTZ_DBG(format...)           LOG_D(LOG_MODULE_PTZ,format)
#define PTZ_INFO(format...)          LOG_I(LOG_MODULE_PTZ,format)
#define PTZ_WARN(format...)          LOG_W(LOG_MODULE_PTZ,format)
#define PTZ_ERROR(format...)         LOG_E(LOG_MODULE_PTZ,format)

#endif
