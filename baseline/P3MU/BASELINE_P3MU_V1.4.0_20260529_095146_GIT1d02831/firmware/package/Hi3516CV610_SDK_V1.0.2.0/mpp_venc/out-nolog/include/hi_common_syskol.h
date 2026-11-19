/*
  Copyright (c), 2001-2025, Shenshu Tech. Co., Ltd.
 */

#ifndef HI_COMMON_SYSKOL_H
#define HI_COMMON_SYSKOL_H

#include "ot_common_syskol.h"

#ifdef __cplusplus
extern "C" {
#endif

#define HI_SYSKOL_CAPTURE_MODE_NORMAL OT_SYSKOL_CAPTURE_MODE_NORMAL
#define HI_SYSKOL_CAPTURE_MODE_SINGLE OT_SYSKOL_CAPTURE_MODE_SINGLE
#define HI_SYSKOL_CAPTURE_MODE_BUTT OT_SYSKOL_CAPTURE_MODE_BUTT
#define HI_SYSKOL_SLEEP_MODE_NONE OT_SYSKOL_SLEEP_MODE_NONE
#define HI_SYSKOL_SLEEP_MODE_LIGHT OT_SYSKOL_SLEEP_MODE_LIGHT
#define HI_SYSKOL_SLEEP_MODE_DEEP OT_SYSKOL_SLEEP_MODE_DEEP
#define HI_SYSKOL_SLEEP_MODE_USER OT_SYSKOL_SLEEP_MODE_USER
#define HI_SYSKOL_SLEEP_MODE_BUTT OT_SYSKOL_SLEEP_MODE_BUTT
typedef ot_syskol_capture_mode hi_syskol_capture_mode;
typedef ot_syskol_sleep_mode hi_syskol_sleep_mode;
typedef ot_syskol_sleep_user_param hi_syskol_sleep_user_param;
typedef ot_syskol_param hi_syskol_param;

#ifdef __cplusplus
}
#endif
#endif /* HI_COMMON_SYSKOL_H */
