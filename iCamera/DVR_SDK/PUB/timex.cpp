
/**
 * \file timex.c
 * 
 * \brief conver different format of time
 * 
 * \author liuk
 * 
 * \version 
 * 
 * \date 15may2008
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "idvr_errno.h"			//错误码
#include "global_macro.h"		//全工程宏定义
// #include "sys_banned.h"
#include "global_gather.h"
#include "timex.h"

time_t rectime_to_time(const char *stime)
{
	struct tm stm;

	char _stime[REC_TIME_LEN];

	char *p;

	if (NULL == stime)
	{
		return 0;
	}

	strncpy(_stime, stime, (REC_TIME_LEN - 1));
	_stime[REC_TIME_LEN - 1] = '\0';

	p = &_stime[REC_TIME_LEN - 1];
	/* sec */
	p = p - 2;
	stm.tm_sec = atoi(p);

	/* min */
	*p = '\0';
	p = p - 2;
	stm.tm_min = atoi(p);

	/* hour */
	*p = '\0';
	p = p - 2;
	stm.tm_hour = atoi(p);

	/* date */
	*p = '\0';
	p = p - 2;
	stm.tm_mday = atoi(p);

	/* mon */
	*p = '\0';
	p = p - 2;
	stm.tm_mon = atoi(p) - 1;

	/* year */
	*p = '\0';
	p = p - 4;
	stm.tm_year = atoi(p) - 1900;

	return mktime(&stm);
}

char *time_to_rectime(char *stime, time_t time)
{
	struct tm stm;

	if (NULL == stime)
	{
		return NULL;
	}

	localtime_r(&time, &stm);

	strftime(stime, 15, "%Y%m%d%H%M%S", &stm);	/* format "YYYYmmDDHHMMSS\0" */

	return stime;
}

static int s_bTimeSyncFlag = 0;

//!\brief 系统时间已经同步
void set_time_sync_flag()
{
	s_bTimeSyncFlag = 1;
}

//!\brief 获取系统时间是否已经同步
int get_time_sync_flag()
{
	return s_bTimeSyncFlag;
}

