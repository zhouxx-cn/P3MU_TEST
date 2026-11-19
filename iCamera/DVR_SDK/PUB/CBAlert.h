#ifndef _INC_CBALERT_H_
#define _INC_CBALERT_H_

#include "tagAlertInfo.h"
#include "tagActionInfo.h"

typedef int (*CBAlert) (struct tagAlertInfo * pAlertInfo, struct tagActionInfo * pActionInfo);

#endif							/* #ifndef _INC_CBALERT_H_ */
