// Copyright (C) 1991 - 1999 Rational Software Corporation

/*
--#修改记录1 yuk@higheasyrd.com 2009/04/17
--#1.增加MkTag(timt_t tTIme)接口
--#2.增加typedef
--#3.将cout改为printf，消除编译警告
*/

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_TAGTIME_49BE041900FF_INCLUDED
#define _INC_TAGTIME_49BE041900FF_INCLUDED

#include <time.h>
#include <stdio.h>
//#include <iostream>

//##ModelId=49BE041900FF
typedef struct tagTime
{
	int year;
	int mon;
	int day;
	int hour;
	int min;
	int sec;

	/*!
	 *\fn int mktime()
	 *\brief 将时间转化为秒
	 *\return 秒
	 */
	int MkSecond()
	{
		struct tm tm;
		  tm.tm_year = year - 1900;

		  tm.tm_mon = mon - 1;

		  tm.tm_mday = day;

		  tm.tm_hour = hour;

		  tm.tm_min = min;

		  tm.tm_sec = sec;

		  return mktime(&tm);
	};


	/*!
	 *\fn int mktime()
	 *\brief 将时间转化为秒
	 *\return 秒
	 */
	int GetWeekDay()
	{
		struct tm result;

		time_t t = MkSecond();

		localtime_r(&t, &result);

		//printf("\n%d年%02d月%02d日-%02d:%02d:%02d\n", tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
		return result.tm_wday;
	};

	/*!
	 *\fn int mktime()
	 *\brief 秒转化为结构体的形式
	 *\return 秒
	 */
	void MkTag()
	{
		struct tm tm;

		time_t now = time(NULL);

		localtime_r(&now, &tm);
		year = tm.tm_year + 1900;

		mon = tm.tm_mon + 1;

		day = tm.tm_mday;

		hour = tm.tm_hour;

		min = tm.tm_min;

		sec = tm.tm_sec;
	};

	/*!
	 *\fn MkTag(time_t nSecond)
	 *\brief 秒转化为结构体的形式
	 *\param tTime 从1970年1月1日0时0分0秒算起的秒数
	 *\return 无
	 */
	void MkTag(const time_t tTime)
	{
		struct tm tmval;

		localtime_r(&tTime, &tmval);

		year = tmval.tm_year + 1900;

		mon = tmval.tm_mon + 1;

		day = tmval.tm_mday;

		hour = tmval.tm_hour;

		min = tmval.tm_min;

		sec = tmval.tm_sec;
	};

	/*!
	 *\fn int CheckDate(int year, int mon, int day)
	 *\brief 检查日期
	 *\param year 年份
	 *\param mon 月份
	 *\param day 日期
	 *\return -1 不合法 0 合法
	 */
	int Check()
	{
		if ((year > 2037) || (year < 1970) || (mon > 12) || (mon < 1) || (day > 31) || (day < 1))
		{						//超过允许的正常时间
			return -1;
		}
		else if ((2 == mon) || (4 == mon) || (6 == mon) || (9 == mon) || (11 == mon))
		{						// 2,4,6,9,11月份
			if (((0 == year % 4) && (0 != year % 100)) || (0 == year % 400))
			{
				if (day > 29 && (mon == 2))
				{				//闰年2月  
					return -1;
				}
			}
			else				//非闰年
			{
				if (day > 28 && (mon == 2))
				{				//非闰年2月
					return -1;
				}
			}
			if (day > 30)		//4，6，9，11月份
			{
				return -1;
			}
		}

		return ((hour < 0 || hour > 23 || min < 0 || min > 59) ? -1 : 0);
	};

	/*!
	 *\fn int mktime()
	 *\brief 将时间转化为秒
	 *\return 秒
	 */
	void print()
	{
#if 0
		cout << year << "年";
		cout << mon << "月";
		cout << day << "日";
		cout << hour << "时";
		cout << min << "分";
		cout << sec << "秒";
		cout << "---秒：" << MkSecond() << endl;
#else
		//printf("%d-%d-%d %d:%d:%d  ---  %d seconds\n", year, mon, day, hour, min, sec, MkSecond());
#endif
	};

} IDVR_SDK_TIME;

#endif							/* _INC_TAGTIME_49BE041900FF_INCLUDED */
