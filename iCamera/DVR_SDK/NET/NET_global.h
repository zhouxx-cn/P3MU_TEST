#ifndef _NET_GLOBAL_H_
#define _NET_GLOBAL_H_

#pragma once

#include <stdio.h>
#include <string.h>
#include "PUB/global_macro.h"
#include "newhie/HieServer.h"
#include "newhie/ZWHosts.h"
#include "newhie/ZWHostsCommon.h"
#include "LogManager.h"

#define NET_ERR(format...)        LOG_E(LOG_MODULE_NET,format)
#define NET_WARN(format...)       LOG_W(LOG_MODULE_NET,format)
#define NET_INFO(format...)       LOG_I(LOG_MODULE_NET,format)
#define NET_DBG(format...)        LOG_D(LOG_MODULE_NET,format)

//使用诊断宏    0表示不使用，1表示出现错误退出程序（用于模块单调），2表示出现错误继续运行（用于模块联调）
#define	USE_NET_ASSERT		0
#define NET_MODULE_NAME		"[NET]"

//使用诊断宏
#if (USE_NET_ASSERT == 0)
//不使用
#define NET_CHK(condition)		((void)0)
#elif (USE_NET_ASSERT == 1)
//退出
#define NET_CHK(condition)		idvr_rnl_assert_ex(NET_MODULE_NAME, !!(condition), 1, "Assert Failed! [%s-%s:%d]: %s", __FILE__, __FUNCTION__, __LINE__, #condition)
#else
//不退出
#define NET_CHK(condition)		idvr_rnl_assert_ex(NET_MODULE_NAME, !!(condition), 0, "Assert Failed! [%s-%s:%d]: %s", __FILE__, __FUNCTION__, __LINE__, #condition)
#endif

extern "C" int mysystem(const char *cmdstring);

#endif
