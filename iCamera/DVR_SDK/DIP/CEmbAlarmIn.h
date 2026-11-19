// Copyright (C) 1991 - 1999 Rational Software Corporation

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CEMBALARMIN_49A2165703B2_INCLUDED
#define _INC_CEMBALARMIN_49A2165703B2_INCLUDED

#include "CBaseAlarmIn.h"

//!\brief 内嵌报警输入设备
//##ModelId=49A2165703B2
class CEmbAlarmIn:public CBaseAlarmIn
{
  public:
	CEmbAlarmIn();
	~CEmbAlarmIn();
	//!\brief 获取报警输入状态
	//!\param nPort 端口号
	//!\return 0表示无报警输入 1表示有报警输入 其他表示错误码
	//##ModelId=49A2154400D2
	int GetAlarmInState(int nPort);

	//!\brief 获取端口数目
	//!\return 端口个数
	int GetNum();

	//!\brief 设置端口数目
	//!\return 端口个数
	int SetNum(int iNum);

  private:
	//端口个数
	int m_iNum;
};

#endif							/* _INC_CEMBALARMIN_49A2165703B2_INCLUDED */
