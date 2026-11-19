/*
  Copyright (c), 2024-2024, Shenshu Tech. Co., Ltd.
 */

#ifndef HI_MPI_AIVSR_H
#define HI_MPI_AIVSR_H

#include "hi_common_aivsr.h"
#include "hi_common_aidetect.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Prototype    : hi_mpi_aivsr_create_chn
 * Description  : Create aivsr channel and set channel attr.
 * Parameters   : hi_aivsr_chn           chn            Input channel,set by user. Range
 *                hi_aivsr_input_model   *input_model   Input model info.
 *                hi_aivsr_chn_attr      *chn_attr      Input channel attr.
 * Return Value : TD_SUCCESS: Success;Error codes: Failure.
 * Date         : 2024-08-29
 *
 */
td_s32 hi_mpi_aivsr_create_chn(hi_aivsr_chn chn, const hi_aivsr_input_model *input_model,
    const hi_aivsr_chn_attr *chn_attr);

/*
 * Prototype    : hi_mpi_aivsr_destroy_chn
 * Description  : Destroy  channel.
 * Parameters   : hi_aivsr_chn chn  Input channel, must exist.
 * Return Value : TD_SUCCESS: Success;Error codes: Failure.
 * Date         : 2024-08-29
 *
 */
td_s32 hi_mpi_aivsr_destroy_chn(hi_aivsr_chn chn);

/*
 * Prototype    : hi_mpi_aidetect_set_chn_attr
 * Description  : Set channel attr.
 * Parameters   : hi_aivsr_chn        chn          Input channel, must exist.
 *                hi_aivsr_chn_attr   *chn_attr    Chn attr.
 * Return Value : TD_SUCCESS: Success;Error codes: Failure.
 * Date         : 2024-08-29
 *
 */
td_s32 hi_mpi_aivsr_set_chn_attr(hi_aivsr_chn chn, const hi_aivsr_chn_attr *chn_attr);

/*
 * Prototype    : hi_mpi_aidetect_get_chn_attr
 * Description  : Get chanel attr.
 * Parameters   : hi_aivsr_chn       chn        Input channel,must exist.
 *                hi_aivsr_chn_attr  *chn_attr  Chn attr.
 * Return Value : TD_SUCCESS: Success;Error codes: Failure.
 * Date         : 2024-08-29
 *
 */
td_s32 hi_mpi_aivsr_get_chn_attr(hi_aivsr_chn chn, hi_aivsr_chn_attr *chn_attr);

/*
 * Prototype    : hi_mpi_aivsr_set_chn_param
 * Description  : Set channel param.
 * Parameters   : hi_aivsr_chn        chn          Input channel, must exist.
 *                hi_aivsr_chn_param  *chn_param    Chn param.
 * Return Value : TD_SUCCESS: Success;Error codes: Failure.
 * Date         : 2024-08-29
 *
 */
td_s32 hi_mpi_aivsr_set_chn_param(hi_aivsr_chn chn, const hi_aivsr_chn_param *chn_param);

/*
 * Prototype    : hi_mpi_aivsr_get_chn_param
 * Description  : Get chanel attr.
 * Parameters   : hi_aivsr_chn        chn        Input channel,must  exist.
 *                hi_aivsr_chn_param  *chn_attr  Chn param.
 * Return Value : TD_SUCCESS: Success;Error codes: Failure.
 * Date         : 2024-08-29
 *
 */
td_s32 hi_mpi_aivsr_get_chn_param(hi_aivsr_chn chn, hi_aivsr_chn_param *chn_param);

/*
 * Prototype    : hi_mpi_aivsr_process
 * Description  : aivsr processing, sync interface, input frame and chn, then get the plate result.
 * Parameters   : hi_aivsr_chn      chn        Input channel, must exist.
 *                hi_video_frame    *frame     Input source frame data, default nv21.The input data must not be null.
 *                hi_aidetect_object_of_one_class *object  Input vehicle result from aidetect
 *                hi_aivsr_result_array           *result    Output plate result.
 * Return Value : TD_SUCCESS: Success; Error codes: Failure.
 * Date         : 2024-08-29
 *
 */
td_s32 hi_mpi_aivsr_process(hi_aivsr_chn chn, const hi_video_frame *frame,
    const hi_aidetect_object_of_one_class *object, hi_aivsr_result_array *result);

/*
 * Prototype    : hi_mpi_aivsr_register_result_callback
 * Description  : register result callback.
 * Parameters   : hi_aivsr_chn                  chn        Input channel must not  exist.
 *                hi_mpi_aivsr_result_callback  callback   Input result callback
 * Return Value: TD_SUCCESS: Success; Error codes: Failure.
 * Date         : 2024-08-29
 *
 */
td_s32 hi_mpi_aivsr_register_result_callback(hi_aivsr_chn chn, const hi_aivsr_result_callback callback);

/*
 * Prototype    : hi_mpi_aivsr_unregister_result_callback
 * Description  : unregister result callback.
 * Parameters   : hi_aivsr_chn     chn        Input channel must not  exist.
 * Return Value: TD_SUCCESS: Success; Error codes: Failure.
 * Date         : 2024-08-29
 *
 */
td_s32 hi_mpi_aivsr_unregister_result_callback(hi_aivsr_chn chn);

/*
 * Prototype    : hi_mpi_aivsr_async_process
 * Description  : aivsr processing, async interface, input frame and chn ,then get the plate result.
 * Parameters   : hi_aivsr_chn      chn         Input channel, must exist.
 *                hi_video_frame    *frame      Input source frame data, default nv21.The input data must not be null.
 *                hi_aidetect_object_of_one_class  *object   Input vehicle result from aidetect.
                  hi_s32            milli_sec    timeout.
 * Return Value : TD_SUCCESS: Success; Error codes: Failure.
 * Date         : 2024-08-29
 *
 */
td_s32 hi_mpi_aivsr_async_process(hi_aivsr_chn chn, const hi_video_frame *frame,
    const hi_aidetect_object_of_one_class *object, hi_s32 milli_sec);

#ifdef __cplusplus
}
#endif

#endif