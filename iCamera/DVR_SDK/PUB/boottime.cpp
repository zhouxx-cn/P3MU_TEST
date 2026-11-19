
/*!
 * *\file   boottime.cpp 
 * *\brief  获取已经开机的时间
 * *\author cwf
 * *\date	  2009/03/24
 * *\version 0.1
 * */
#include <sys/sysinfo.h>
//#include <iostream.h>

//!\brief 蜂鸣器工作线程
////!\praram 指针
////!\return  成功为0，失败为-1
int GetTimeSinceBoot()
{
	struct sysinfo info;

	sysinfo(&info);
	return info.uptime;
}
