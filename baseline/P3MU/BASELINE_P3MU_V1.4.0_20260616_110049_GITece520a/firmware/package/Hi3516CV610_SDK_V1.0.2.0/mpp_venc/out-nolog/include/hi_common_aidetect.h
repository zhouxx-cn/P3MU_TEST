/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#ifndef HI_COMMON_AIDETECT_H
#define HI_COMMON_AIDETECT_H

#include <unistd.h>
#include <stdlib.h>
#include "hi_type.h"
#include "hi_common_video.h"
#include "ot_common_aidetect.h"

#define HI_AIDETECT_MAX_CHN_NUM             OT_AIDETECT_MAX_CHN_NUM
typedef ot_aidetect_chn                     hi_aidetect_chn;
typedef ot_aidetect_priority                hi_aidetect_priority;
#define HI_AIDETECT_TRACK_STATUS_NEW        OT_AIDETECT_TRACK_STATUS_NEW
#define HI_AIDETECT_TRACK_STATUS_UPDATE     OT_AIDETECT_TRACK_STATUS_UPDATE
#define HI_AIDETECT_TRACK_STATUS_DIE        OT_AIDETECT_TRACK_STATUS_DIE
#define HI_AIDETECT_TRACK_STATUS_VALID      OT_AIDETECT_TRACK_STATUS_VALID
#define HI_AIDETECT_TRACK_STATUS_BUTT       OT_AIDETECT_TRACK_STATUS_BUTT
typedef ot_aidetect_track_status            hi_aidetect_track_status;
#define HI_AIDETECT_CLASS_FACE              OT_AIDETECT_CLASS_FACE
#define HI_AIDETECT_CLASS_HUMAN             OT_AIDETECT_CLASS_HUMAN
#define HI_AIDETECT_CLASS_VEHICLE           OT_AIDETECT_CLASS_VEHICLE
#define HI_AIDETECT_CLASS_PET               OT_AIDETECT_CLASS_PET
#define HI_AIDETECT_CLASS_GARBAGE           OT_AIDETECT_CLASS_GARBAGE
#define HI_AIDETECT_CLASS_BAG               OT_AIDETECT_CLASS_BAG
#define HI_AIDETECT_CLASS_WALLET            OT_AIDETECT_CLASS_WALLET
#define HI_AIDETECT_CLASS_PHONE             OT_AIDETECT_CLASS_PHONE
#define HI_AIDETECT_CLASS_HEAD_SHOULDER     OT_AIDETECT_CLASS_HEAD_SHOULDER
#define HI_AIDETECT_CLASS_BICYCLE           OT_AIDETECT_CLASS_BICYCLE
#define HI_AIDETECT_CLASS_MOTORCYCLE        OT_AIDETECT_CLASS_MOTORCYCLE
#define HI_AIDETECT_CLASS_PACKAGE           OT_AIDETECT_CLASS_PACKAGE
#define HI_AIDETECT_CLASS_BUTT              OT_AIDETECT_CLASS_BUTT
typedef ot_aidetect_class                   hi_aidetect_class;
typedef ot_aidetect_model_info              hi_aidetect_model_info;
typedef ot_aidetect_track                   hi_aidetect_track;
typedef ot_aidetect_chn_attr                hi_aidetect_chn_attr;
typedef ot_aidetect_threshold               hi_aidetect_threshold;
typedef ot_aidetect_chn_param               hi_aidetect_chn_param;
typedef ot_aidetect_object                  hi_aidetect_object;
typedef ot_aidetect_object_of_one_class     hi_aidetect_object_of_one_class;
typedef ot_aidetect_result_array            hi_aidetect_result_array;
#define HI_AIDETECT_MODEL_LOAD_FROM_MEMORY  OT_AIDETECT_MODEL_LOAD_FROM_MEMORY
#define HI_AIDETECT_MODEL_LOAD_FROM_PATH    OT_AIDETECT_MODEL_LOAD_FROM_PATH
#define HI_AIDETECT_MODEL_LOAD_BUTT         OT_AIDETECT_MODEL_LOAD_BUTT
typedef ot_aidetect_model_load_mode         hi_aidetect_model_load_mode;
typedef ot_aidetect_input_model             hi_aidetect_input_model;
typedef ot_aidetect_chn_status              hi_aidetect_chn_status;

#define HI_ERR_AIDETECT_NULL_PTR            OT_ERR_AIDETECT_NULL_PTR
#define HI_ERR_AIDETECT_NOT_READY           OT_ERR_AIDETECT_NOT_READY
#define HI_ERR_AIDETECT_ILLEGAL_PARAM       OT_ERR_AIDETECT_ILLEGAL_PARAM
#define HI_ERR_AIDETECT_EXIST               OT_ERR_AIDETECT_EXIST
#define HI_ERR_AIDETECT_UNEXIST             OT_ERR_AIDETECT_UNEXIST
#define HI_ERR_AIDETECT_NOT_PERM            OT_ERR_AIDETECT_NOT_PERM

#endif
