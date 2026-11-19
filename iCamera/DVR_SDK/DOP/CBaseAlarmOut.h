// Copyright (C) 1991 - 1999 Rational Software Corporation

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CBASEALARMOUT_499CD0370322_INCLUDED
#define _INC_CBASEALARMOUT_499CD0370322_INCLUDED

//!\brief DOP基础类
//##ModelId=499CD0370322
class CBaseAlarmOut
{
  public:

	//##ModelId=49BDC0B702CE
	CBaseAlarmOut()
	{
	};

	//##ModelId=49BDC0B702DE
	~CBaseAlarmOut()
	{
	};
	//!\brief 获取报警输出通道数目
	//!\param 获取支持的报警输出通道数目
	//!\return -1为获取失败 其他为输出通道数
	//##ModelId=499CD0370323
	int GetNum()
	{
		return 0;
	};

	//!\brief 打开设备
	//!\return 0打开成功 -1打开失败
	//##ModelId=499CD0370324
	int Open()
	{
		return 0;
	};

	//!\brief 关闭设备
	//!\return 0正常关闭,其他表示错误码
	//##ModelId=499CD0370325
	int Close()
	{
		return 0;
	};

	//!\brief 设置报警输出状态
	//!\param bOn 0关闭 1打开
	//!\param nPort 报警输出通道号
	//!\return 0表示设置成功 -1表示设置失败
	//##ModelId=499CD0370326
	int SetAlarmOutSate(int bOn, int nPort)
	{
		return 0;
	};

	//!\brief 获取报警输出状态
	//!\param nPort 报警输出通道号
	//!\return 0表示关闭 1表示打开 其他为错误码
	//##ModelId=499CD0370327
	int GetAlarmOutState(int nPort)
	{
		return 0;
	};



};

#endif							/* _INC_CBASEALARMOUT_499CD0370322_INCLUDED */
