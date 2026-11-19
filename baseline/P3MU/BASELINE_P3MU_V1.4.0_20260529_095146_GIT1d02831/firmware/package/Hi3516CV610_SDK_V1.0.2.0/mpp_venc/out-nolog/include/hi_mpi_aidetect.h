/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#ifndef HI_MPI_AIDETECT_H
#define HI_MPI_AIDETECT_H

#include "hi_common_aidetect.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Prototype    : hi_mpi_aidetect_create_chn
 * Description  : Create detect channel and set channel attr.
 * Parameters   : hi_aidetect_chn   chn            Input  channel,set by user
 * hi_aidetect_input_model   *input_model          Input model info.
 * hi_aidetect_chn_attr       *chn_attr            Chn attr, include tracking enable.
 * Return Value : HI_SUCCESS: Success;Error codes: Failure.
 * Date         : 2024-02-20
 *
 */
hi_s32 hi_mpi_aidetect_create_chn(hi_aidetect_chn chn, const hi_aidetect_input_model *input_model,
    const hi_aidetect_chn_attr *chn_attr);

/*
 * Prototype    : hi_mpi_aidetect_destroy_chn
 * Description  : Destroy  channel.
 * Parameters   : hi_aidetect_chn chn  Input  channel, must exist.
 * Return Value : HI_SUCCESS: Success;Error codes: Failure.
 * Date         : 2024-02-20
 *
 */
hi_s32 hi_mpi_aidetect_destroy_chn(hi_aidetect_chn chn);

/*
 * Prototype    : hi_mpi_aidetect_set_chn_attr
 * Description  : Set channel attr.
 * Parameters   : hi_aidetect_chn   chn          Input  channel,must  exist.
 * hi_aidetect_chn_attr             *chn_attr    Chn attr.
 * Return Value : HI_SUCCESS: Success;Error codes: Failure.
 * Date         : 2024-02-20
 *
 */
hi_s32 hi_mpi_aidetect_set_chn_attr(hi_aidetect_chn chn, const hi_aidetect_chn_attr *chn_attr);

/*
 * Prototype    : hi_mpi_aidetect_get_chn_attr
 * Description  : Get chanel attr.
 * Parameters   : hi_aidetect_chn chn   Input  channel,must  exist.
 * hi_aidetect_chn_attr *chn_attr       Chn attr.
 * Return Value : HI_SUCCESS: Success;Error codes: Failure.
 * Date         : 2024-02-20
 *
 */
hi_s32 hi_mpi_aidetect_get_chn_attr(hi_aidetect_chn chn, hi_aidetect_chn_attr *chn_attr);

/*
 * Prototype    : hi_mpi_aidetect_set_chn_param
 * Description  : Set channel param.
 * Parameters   : hi_aidetect_chn   chn          Input  channel,must  exist.
 * hi_aidetect_chn_param            *chn_param    Chn param.
 * Return Value : HI_SUCCESS: Success;Error codes: Failure.
 * Date         : 2024-02-20
 *
 */
hi_s32 hi_mpi_aidetect_set_chn_param(hi_aidetect_chn chn, const hi_aidetect_chn_param *chn_param);

/*
 * Prototype    : hi_mpi_aidetect_get_chn_param
 * Description  : Get chanel attr.
 * Parameters   : hi_aidetect_chn chn   Input  channel,must  exist.
 * hi_aidetect_chn_param *chn_attr       Chn param.
 * Return Value : HI_SUCCESS: Success;Error codes: Failure.
 * Date         : 2024-02-20
 *
 */
hi_s32 hi_mpi_aidetect_get_chn_param(hi_aidetect_chn chn, hi_aidetect_chn_param *chn_param);

/*
 * Prototype    : hi_mpi_aidetect_get_model_info
 * Description  : Get the resolution of the input frame and model threshs
 * Parameters   : hi_aidetect_chn       chn                 Intput channel ,must   exist.
 * hi_aidetect_model_info               *model_info         The model params.
 * Return Value : HI_SUCCESS: Success;Error codes: Failure.
 * Date         : 2024-02-20
 *
 */
hi_s32 hi_mpi_aidetect_get_model_info(hi_aidetect_chn chn, hi_aidetect_model_info *model_info);

/*
 * Prototype    : hi_mpi_aidetect_process
 * Description  : Detect processing, input frame and chn ,then get the detect and track result.
 * Parameters   : hi_aidetect_chn   chn             Input channel must not  exist.
 * hi_video_frame                   *frame          Input source frame data, default nv21.The input data must not be
 * null. hi_aidetect_result_array         *detect_result  Output detect and track result. Return Value : HI_SUCCESS:
 * Success;Error codes: Failure. Date         : 2024-02-20
 *
 */
hi_s32 hi_mpi_aidetect_process(hi_aidetect_chn chn, const hi_video_frame *frame,
    hi_aidetect_result_array *detect_result);

/*
 * Prototype    : hi_mpi_aidetect_set_log_level
 * Description  : Set the log level, default level is 0, 0-error 1-warn 2-info.
 * Parameters   : hi_u32   log_level  Set log level,range:[0,2].
 * Return Value : HI_SUCCESS: Success;Error codes: Failure.
 * Date         : 2024-02-20
 *
 */
hi_s32 hi_mpi_aidetect_set_log_level(hi_u32 log_level);

/*
 * Prototype    : hi_mpi_aidetect_get_log_level
 * Description  : Get the log level, default level is 0, 0-error 1-warn 2-info.
 * Parameters   : hi_u32   *log_level  Set log level,range:[0,2].
 * Return Value : HI_SUCCESS: Success;Error codes: Failure.
 * Date         : 2024-02-20
 *
 */
hi_s32 hi_mpi_aidetect_get_log_level(hi_u32 *log_level);

/*
 * Prototype    : hi_mpi_aidetect_query_status
 * Description  : Get the chn status.
 * Parameters   : hi_aidetect_chn   chn             Input channel must not  exist.
 * hi_aidetect_chn_status   *status  total input frames and current framerate.
 * Return Value : HI_SUCCESS: Success;Error codes: Failure.
 * Date         : 2024-02-20
 *
 */
hi_s32 hi_mpi_aidetect_query_status(hi_aidetect_chn chn, hi_aidetect_chn_status *status);

#ifdef __cplusplus
}
#endif

#endif
