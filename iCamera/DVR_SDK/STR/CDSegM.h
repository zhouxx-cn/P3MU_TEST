
/*!
*\file   CDSegM.h 
*\brief  片段管理声明
*\author cwf
*\date	  2009/03/24
*\version 0.1
*/

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CDSEGM_49A63AE901CD_INCLUDED
#define _INC_CDSEGM_49A63AE901CD_INCLUDED

#include <time.h>
#include "HANDLE.h"
#include "global_macro.h"
struct tagRecSeg;

/*!
 * \brief 录像片段回调函数
 * \param hRecSegReporter 录像片段报告器句柄
 * \param iChannel 该录像片段对应的通道
 * \param tBeginTime 该录像片段对应的起始时间
 * \param tEndTime 该录像片段对应的结束时间
 * \param RecType 该录像片段对应的录像类型
 * \param iChannel 该录像片段对应的通道
 * \param pChContext 暂时没用
 * \return
 */
#ifndef SEGMENT_CALLBACK
typedef int (*SEGMENT_CALLBACK) (HANDLE hRecSegReporter, int iChannel, time_t tBeginTime, time_t tEndTime, unsigned short RecType, void *pChContext);

#endif


//数据弹出管理器
//该类成员函数为虚函数，不提供针对某以文件文件系统的方法，具体
//到不同的文件系统需要不同的实现
//##ModelId=49A63AE901CD
class CDSegM
{
  public:
	//功能：
	//弹出器开始弹出数据
	//参数： 无
	//返回值：成功为0，失败为-1
	//##ModelId=49A63B9C00D0
	int StartPop();

	//功能：
	//弹出其停止弹出
	//参数：无
	//返回值：成功为0，失败为-1
	//##ModelId=49A63BA40226
	int StopPop();


	//功能：
	//添加一个通道至弹出其
	//参数：
	//       nCh:int 通道
	//        int bDynReport  
	//设定是否需要动态报告索引变化,默认不动态报告索引变化(用来
	//动态更新录像片段信息)。
	//返回值：
	//       成功为0，失败为-1
	//##ModelId=49AF87DF01B6
	int AddCh(int nCh, int bDynReport);

	//##ModelId=49AF8942018E
	int DelCh(int nCh);

	//##ModelId=49AF8A5E018C
	int SetAttr(int DiskGroupMask, SEGMENT_CALLBACK CallBack, time_t tBeginTime, time_t tEndTime);

	//##ModelId=49AF8EA30365
	static int ReleaseRecSegList(tagRecSeg * pRecSegHead);

	//功能：获取录像片段信息
	//!\param int DiskGroupMask 0表示查询普通分组+报警分组、1表示冗余分组、2表示备份分组
	//参数：nChannel : Int 通道
	// tBeginTime : time_t 开始时间
	//tEndTime : time_t 结束时间 
	//pRecSegHead : RECSEG **录像片段信息指针
	//返回值：成功为0，失败为-1
	//##ModelId=49AF867C02F6
	static int GetRecSegList(int DiskGroupMask, int nChannel, time_t tBeginTime, time_t tEndTime, int RecType, tagRecSeg ** pRecSegHead, int *nCount);

	  CDSegM();
	 ~CDSegM();

  private:

	//弹出器句柄
	  HANDLE dev;
	//弹出器的开始时间
	time_t m_BeginTime;

	//弹出器的结束时间
	time_t m_EndTime;

	//弹出器的结束时间
	unsigned int m_nDiskGrouMask;

	//片段弹出器回调函数
	SEGMENT_CALLBACK m_CallBack;

	unsigned char m_nBindCh[MAX_ENC_CHANNEL_NUM];

	//片段链表指针
	static tagRecSeg *m_pRecSegHead;

};

#endif							/* _INC_CDSEGM_49A63AE901CD_INCLUDED */
