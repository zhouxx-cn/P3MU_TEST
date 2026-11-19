// Copyright (C) 1991 - 1999 Rational Software Corporation

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CEXTALARMOUT_499CC3FA0060_INCLUDED
#define _INC_CEXTALARMOUT_499CC3FA0060_INCLUDED

#include "CBaseAlarmOut.h"




//!\brief 外部报警输出设备类
//##ModelId=499CC3FA0060
class CExtAlarmOut:public CBaseAlarmOut
{
  public:
	//!\brief 获得外部报警输出设备实例的指针
	//!\return NULL表示获取失败 其他表示指针值
	//##ModelId=49BDC4580138
	static CExtAlarmOut *GetInstance();
	//!\brief 销毁CEmbAlarmOut实例
	//!\return 引用计数
	//##ModelId=49BDC4580158
	static int Release();
	//!\brief 获得报警输出端口数
	//!\return -1表示获取失败,其他为端口数
	//##ModelId=49BDC4580196
	int GetNum();
	//!\brief 打开设备，开始工作
	//!\return 0表示打开成功 -1表示打开失败
	//##ModelId=49BDC45801A5
	int Open();
	//!\brief 关闭设备，停止工作
	//!\return 0表示成功关闭 其他表示错误码
	//##ModelId=49BDC45801B5
	int Close();
	//!\brief 获取报警输出端口状态
	//!\param nPort 端口号
	//!\return 0表示端口打开，1表示端口关闭，其他表示错误码
	//##ModelId=49BDC45801C5
	int GetAlarmOutState(int nPort);
	//!\brief 设置报警输出端口状态
	//!\param bOn 0表示端口打开，1表示端口关闭
	//!\param nPort 端口号
	//!\return 0设置成功 -1设置失败
	//##ModelId=49BDC4580203
	int SetAlarmOutState(int bOn, int nPort);


  private:  CExtAlarmOut();
	 ~CExtAlarmOut();
	static CExtAlarmOut *m_pInstance;
	//实例引用计数
	static int m_iInstance_Cnt;
	//端口数
	int m_nNum;





};

#endif							/* _INC_CEXTALARMOUT_499CC3FA0060_INCLUDED */
