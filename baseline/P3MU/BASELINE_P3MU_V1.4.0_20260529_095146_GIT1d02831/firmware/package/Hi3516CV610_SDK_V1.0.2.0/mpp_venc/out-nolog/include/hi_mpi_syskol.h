/*
  Copyright (c), 2001-2025, Shenshu Tech. Co., Ltd.
 */

#ifndef HI_MPI_SYSKOL_H
#define HI_MPI_SYSKOL_H

#include "hi_common.h"
#include "hi_common_syskol.h"

#ifdef __cplusplus
extern "C" {
#endif

hi_s32 hi_mpi_syskol_set_param(const hi_syskol_param *param);
hi_s32 hi_mpi_syskol_get_param(hi_syskol_param *param);

hi_s32 hi_mpi_syskol_sleep(hi_s32 milli_sec);
hi_s32 hi_mpi_syskol_wakeup(hi_void);

hi_s32 hi_mpi_syskol_suspend(hi_void);

#ifdef __cplusplus
}
#endif
#endif /* HI_MPI_SYSKOL_H */
