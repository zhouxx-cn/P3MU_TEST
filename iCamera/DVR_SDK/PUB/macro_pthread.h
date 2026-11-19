#ifndef __MACRO_PTHREAD_H__
#define __MACRO_PTHREAD_H__
#include <sys/types.h>
#include <unistd.h>
#include "RNL/idvr_rnl.h"

#define BEGINRUN_THREAD  IDVR_RNL_INFO("PUB", "[%s %s %d] begin run thread %d !\n",__FILE__,__FUNCTION__,__LINE__,getpid());
#define ENDRUN_THREAD    IDVR_RNL_INFO("PUB", "[%s %s %d] end   run thread %d !\n",__FILE__,__FUNCTION__,__LINE__,getpid());

#endif
