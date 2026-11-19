
/*!
*\file   CDPopM.h 
*\brief  数据弹出器声明
*\author cwf
*\date	  2009/03/24
*\version 0.1
*/

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CDPOPM_49A63CA20036_INCLUDED
#define _INC_CDPOPM_49A63CA20036_INCLUDED
#include <time.h>
#include "HANDLE.h"
#include "global_macro.h"

/*!
 * \brief 数据弹出回调函数
 * \param hDataPopper 数据弹出器句柄
 * \param iChannel 数据对应的通道
 * \param int bIndexFrame 是否为索引帧，true是，false不是
 * \param wFrameType 数据对应的类型
 * \param FrameTime 数据对应的时间
 * \param pBuf 数据对应的缓冲区地址
 * \param dwSize 数据对应的大小
 * \param EventID 发生的事件类型，如果是正常的数据弹出，EventID为DATAPOP_EVENT_NONE，其他事件时，帧相关的参数无效
 * \return
*/
#ifndef DATAPOP_CALLBACK
typedef int (*DATAPOP_CALLBACK) (HANDLE hDataPopper, int iChannel, int bIndexFrame, unsigned short wFrameType, time_t FrameTime, unsigned char *pBuf,
								 unsigned long dwSize, unsigned long EventID);
#endif


//数据弹出器
//该类成员函数为虚函数，不提供针对某一文件系统的实现方法，具体
//到不同的文件系统需要不同的实现
//##ModelId=49A63CA20036
class CDPopM
{
  public:
	//!\brief 设置数据弹出器属性。
	//!\param int DiskGroupMask 0表示查询普通分组+报警分组、1表示冗余分组、2表示备份分组
	//!\param nRefChan : 参考通道(可以不设置)。
	//!\param time_t tStartSeekTime  开始时间
	//!\param time_t tEndSeekTime 结束时间
	//!\param int (*callBack)() 回调指针
	//!\return  成功返回0，失败为-1
	//##ModelId=49A6409F02A1
	int SetAttr(int DiskGroupMask, int nRefChan, time_t tStartSeekTime, time_t tEndSeekTime, DATAPOP_CALLBACK PopCallBack, int bDynPop);

	//!\brief 添加弹出数据的通道。
	//!\param int nChannel 通道号。
	//!\return  成功返回0，失败为-1
	//##ModelId=49A640AD0002
	int AddCh(int nChannel);

	//!\brief 移除弹出数据的通道。
	//!\param int nChannel 通道号。
	//!\return  成功返回0，失败为-1
	//##ModelId=49A640D20114
	int DelCh(int nChannel);

	//!\brief 向弹出器添加一种录像类型
	//!\param int RecType 录像类型（见录像模块定义）
	//!\return  成功为0，失败为-1   
	int DPM_AddRecType(int RecType);


	//!\brief 获取一帧关键帧
	//!\param hDataPopper 数据弹出器的句柄
	//!\param iChannel 通道号
	//!\param tSeekTime 指定时间簇
	//!\param precision 时间簇误差
	//!\param pbuf 保存该关键帧的缓冲区指针
	//!\param bufsize 输入时为缓冲区大小，输出时为关键帧大小
	//!\return 成功返回0，失败返回小于0的错误码。
	int GetOneKeyFrame(HANDLE hHandle, int iChannel, time_t tSeekTime, unsigned int precision, unsigned char *pbuf, unsigned int *bufsize);


	//!\brief 获取弹出器中数据的大小
	//!\return  成功为数据的大小（KB），失败为-1
	long long GetSize();

	//!\brief 释放弹出器
	//!\return  成功为0，失败为-1
	int Release();

	//!\brief 删除弹出器中一种录像类型
	//!\param int RecType 录像类型（见录像模块定义）
	//!\return  成功为0，失败为-1
	int DPM_DelRecType(int RecType);

	//!\brief 
	//数据弹出器定位(设定了参考通道，按参考通道定位，未设参考
	//通道，定位为时序上第一个满足条件的通道)。
	//!\param time_t SeekTime 设置的弹出数据的时间
	//!\return  成功返回0，失败为-1
	//##ModelId=49AF92050125
	int Seek(time_t SeekTime);

	//!\brief 弹出器开始弹出数据
	//!\param 无
	//!\return  成功返回0，失败为-1
	//##ModelId=49AF9243021F
	int StartPop();

	//!\brief 弹出器停止弹出数据
	//!\param 无
	//!\return  成功返回0，失败为-1
	//##ModelId=49AF924802DA
	int StopPop();

	//!\brief 设置弹出数据的类型
	//!\param int nDataType 0为全部数据，1为I帧
	//!\return  成功返回0，失败为-1
	//##ModelId=49B8C34D0075
	int SetPopDataType(int nDataType, unsigned int PopKeyInterval);

	//!\brief 设置数据弹出的方向
	//!\param bool bFront 0为向前弹出，1为向后弹出
	//!\return  成功返回0，失败为-1
	//##ModelId=49B8C4720211
	int SetPopDataDirection(bool bFront);

	static int CallBack(HANDLE hDataPopper, int iChannel, int bIndexFrame, unsigned short wFrameType, time_t FrameTime, unsigned char *pBuf,
						unsigned long dwSize, unsigned long EventID, void *UserContext);

	  CDPopM();
	 ~CDPopM();
  private:
	  HANDLE dev;
	unsigned char m_nBindCh[MAX_ENC_CHANNEL_NUM];
	int m_RecordType[MAX_RECORD_TYPE];

	DATAPOP_CALLBACK m_CallBack;

};

#endif							/* _INC_CDPOPM_49A63CA20036_INCLUDED */
