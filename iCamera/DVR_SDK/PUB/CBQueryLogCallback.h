#ifndef __CB_QUERYLOGCALLBACK_H_
#define __CB_QUERYLOGCALLBACK_H_

#include "HANDLE.h"

typedef int (*CBQueryLogCallback) (HANDLE hQueryHandle, int nLogCount);
#endif
