
/*!
 *\file		timex.h
 *\brief	PUB模块，时间转换函数
 *\author	林云
 *\date		2009/03/30
 */

#ifndef _INC_TIMEX_H_
#define _INC_TIMEX_H_

#include <time.h>


//!\brief   时间转换函数（char * 转 time_t）
//!\param   stime   字符串型时间（YYYYmmDDHHMMSS）
//!\return  time_t类型时间
time_t rectime_to_time(const char *stime);

//!\brief   时间转换函数（time_t 转 char *）
//!\param   stime   返回字符串型时间（YYYYmmDDHHMMSS）
//!\param   time    time_t型时间
//!\return  stime
char *time_to_rectime(char *stime, time_t time);

#if defined(__cplusplus)
extern "C" {
#endif
//!\brief 系统时间已经同步
void set_time_sync_flag();

//!\brief 获取系统时间是否已经同步
int get_time_sync_flag();
#if defined(__cplusplus)
}
#endif

#endif							/* #ifndef _INC_TIMEX_H_ */
