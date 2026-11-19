//AVL_global.h
#ifndef _DIP_GLOBAL_H_
#define _DIP_GLOBAL_H_
//#include <iostream>
#include "tagAlertInfo.h"
#include "tagActionInfo.h"

#include "global_gather.h"
#include "idvr_rnl.h"

#define MODULENAME_DIP "[DIP]"

#define TRACE(x...)   IDVR_RNL_DEBUG(MODULENAME_DIP,x)
#define TRACE_WARN(x...)   IDVR_RNL_INFO(MODULENAME_DIP,x)
#define TRACE_ERR(x...)   IDVR_RNL_ERROR(MODULENAME_DIP,x)

typedef int (*ALARMCALLBACK) (struct tagAlertInfo * pAlertInfo, struct tagActionInfo * pActionInfo);

#endif
