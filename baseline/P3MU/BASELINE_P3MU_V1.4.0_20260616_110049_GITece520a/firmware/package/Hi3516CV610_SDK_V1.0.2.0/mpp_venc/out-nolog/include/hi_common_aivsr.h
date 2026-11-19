/*
  Copyright (c), 2024-2024, Shenshu Tech. Co., Ltd.
 */

#ifndef HI_COMMON_AIVSR_H
#define HI_COMMON_AIVSR_H

#include "ot_common_aivsr.h"
#include "hi_common_face_capture.h"
#include "hi_common_fr.h"
#include "hi_common_vlpr.h"

#define HI_AIVSR_MAX_CHN_NUM OT_AIVSR_MAX_CHN_NUM
typedef ot_aivsr_chn hi_aivsr_chn;
#define HI_AIVSR_MODEL_LOAD_FROM_MEMORY OT_AIVSR_MODEL_LOAD_FROM_MEMORY
#define HI_AIVSR_MODEL_LOAD_FROM_PATH OT_AIVSR_MODEL_LOAD_FROM_PATH
#define HI_AIVSR_MODEL_LOAD_BUTT OT_AIVSR_MODEL_LOAD_BUTT
typedef ot_aivsr_model_load_mode hi_aivsr_model_load_mode;
typedef ot_aivsr_input_model hi_aivsr_input_model;
#define HI_AIVSR_TASK_FACE_CAPTURE OT_AIVSR_TASK_FACE_CAPTURE
#define HI_AIVSR_TASK_VLPR OT_AIVSR_TASK_VLPR
#define HI_AIVSR_TASK_FR OT_AIVSR_TASK_FR
#define HI_AIVSR_TASK_BUTT OT_AIVSR_TASK_BUTT
typedef ot_aivsr_task_type hi_aivsr_task_type;
typedef ot_aivsr_chn_attr hi_aivsr_chn_attr;
typedef ot_aivsr_model_priority hi_aivsr_model_priority;
typedef ot_aivsr_chn_param hi_aivsr_chn_param;
typedef ot_aivsr_result_array hi_aivsr_result_array;
typedef ot_aivsr_result_callback hi_aivsr_result_callback;
#define HI_ERR_AIVSR_NULL_PTR OT_ERR_AIVSR_NULL_PTR
#define HI_ERR_AIVSR_NOT_READY OT_ERR_AIVSR_NOT_READY
#define HI_ERR_AIVSR_ILLEGAL_PARAM OT_ERR_AIVSR_ILLEGAL_PARAM
#define HI_ERR_AIVSR_EXIST OT_ERR_AIVSR_EXIST
#define HI_ERR_AIVSR_UNEXIST OT_ERR_AIVSR_UNEXIST
#define HI_ERR_AIVSR_NOT_PERM OT_ERR_AIVSR_NOT_PERM
#endif