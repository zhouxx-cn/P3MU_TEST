// Copyright (C) 1991 - 1999 Rational Software Corporation

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CBASEALARMIN_499B632A00C1_INCLUDED
#define _INC_CBASEALARMIN_499B632A00C1_INCLUDED

//!\brief 报警输入设备基础类
//##ModelId=499B632A00C1
class CBaseAlarmIn
{
  public:
	//!\brief 打开设备，开始工作
	//!\return 0成功打开 -1失败
	//##ModelId=49A2152B0234
	int Open()
	{
		return 0;
	};

	//!\brief 关闭设备，停止工作
	//!\return 0成功打开 -1失败
	//##ModelId=49A2153D0399
	int Close()
	{
		return 0;
	};

	//!\brief 获得报警输入个数
	//!\return 输入个数
	int GetNum()
	{
		return 0;
	};

	//!\brief 获取报警输入状态
	//!\param nPort 端口号
	//!\return 0表示无报警输入 1表示有报警输入 其他表示错误码
	//##ModelId=49A2154400D2
	int GetAlarmInState(int nPort)
	{
		return 0;
	};

	//##ModelId=49A34D10028B
	CBaseAlarmIn()
	{
	};

	//##ModelId=49A34D120298
	~CBaseAlarmIn()
	{
	};

};

#endif							/* _INC_CBASEALARMIN_499B632A00C1_INCLUDED */
