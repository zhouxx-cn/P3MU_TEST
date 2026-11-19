/*
  Copyright (c), 2001-2025, Shenshu Tech. Co., Ltd.
 */

#ifndef HI_MPI_BLA_H
#define HI_MPI_BLA_H

#include "hi_common_video.h"
#include "hi_common_bla.h"

hi_s32 hi_mpi_bla_calibration(const hi_video_frame *master_frame, const hi_video_frame *slave_frame,
    const hi_float down_scale_ratio, hi_point_f *master_coord);

hi_s32 hi_mpi_bla_check_calibration_info(hi_u32 num, const hi_bla_calibration_info *calibration_info, hi_bool *valid);

hi_s32 hi_mpi_bla_calculate_motor_coord(hi_u32 num, const hi_bla_calibration_info *calibration_info,
    const hi_point_f *master_coord, hi_point_f *slave_coord);

#endif /* HI_MPI_BLA_H */
