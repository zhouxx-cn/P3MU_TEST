
/**
  * filename : udpate.h
  *
  * modificate :
  *				20070701 - verify device version, release v2.1
  *				20070625 - fix bugs, write file incompletely. release v2.0
  *             20070608 - assemble tar and gunzip source code.
  *             20070530 - fix bugs, release v1.0.
  *             20070529 - create.
 */

#ifndef _UPDATE_H_
#define _UPDATE_H_

#define UPDATE_FILE_PATH		"/mnt/mtd/zw_device_update.tar.gz"
#define VERSION_FILE_NAME		"/mnt/app/device/DeviceVersion.ini"
#define VERIFY_FILE_NAME		"/mnt/app/version"
#define FLAG_FILE_NAME			"/tmp/app/.update_success"
#define DEFAULT_CFG_FILE		"/mnt/mtd/config/Edvr.default"
#define NORMAL_CFG_FILE			"/mnt/mtd/config/Edvr.cfg"
#define UPDATE_CFG_FILE			"/mnt/mtd/config/update.cfg"
#define UPDATEFLAG_CFG_FILE		"/mnt/mtd/config/update.flag"
#define UPDATE_LOGO_BMP       	"/tmp/app/res/logo_update.bmp"

/** buffer length */
#define BUFLEN			128


/** include */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>
#include <stdarg.h>
#include <string.h>
#include <strings.h>
#include <fnmatch.h>
#include <sys/wait.h>
#include <pthread.h>
#include <signal.h>
#include <errno.h>
#include "idvr_rnl.h"


#define MODULENAME "[update]"


#define USE_RNL

#ifdef USE_RNL

#define TRACE(x...)   IDVR_RNL_INFO(MODULENAME,x)
#define TRACE_WARN(x...)   IDVR_RNL_INFO(MODULENAME,x)
#define TRACE_ERR(x...)   IDVR_RNL_ERROR(MODULENAME,x)

#else
#define USE_TRACE
#define USE_TRACE_WARN
#define USE_TRACE_ERR

#ifdef USE_TRACE
#define TRACE printf("[%s %d %s]",__FILE__,__LINE__,__FUNCTION__);printf
#else
#define TRACE 0?1:printf
#endif


#ifdef USE_TRACE_WARN
#define TRACE_WARN printf("[Warning:%s %d %s]",__FILE__,__LINE__,__FUNCTION__);printf
#else
#define TRACE_WARN 0?1:printf
#endif

#ifdef USE_TRACE_ERR
#define TRACE_ERR printf("[Error:%s %d %s]",__FILE__,__LINE__,__FUNCTION__);printf
#else
#define TRACE_ERR 0?1:printf
#endif

#endif
#define PRNT TRACE
#endif

