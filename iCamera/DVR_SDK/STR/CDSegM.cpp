
/*!
*\file   CDSegM.cpp 
*\brief  数据片段管理
*\author cwf
*\date	  2009/03/24
*\version 0.1
*/

//#include <iostream.h>
#include "pkg_interface.h"
#include "tagRecSeg.h"
#include "CDSegM.h"
#include "debug.h"
#include "str_fs.h"
#include "tagTime.h"
#include "idvr_errno.h"
// #include "sys_banned.h"
#include "global_gather.h"
tagRecSeg *CDSegM::m_pRecSegHead = NULL;

/**************************************************************************
*                               方法实现                                   *
 **************************************************************************/

CDSegM::CDSegM()
{
	// TODO: Add your specialized code here.
	int i;

	m_BeginTime = 0;
	m_EndTime = 0;
	m_nDiskGrouMask = 0;

	for (i = 0; i < MAX_ENC_CHANNEL_NUM; i++)
	{
		m_nBindCh[i] = 0;
	}

	dev = NULL;
}

CDSegM::~CDSegM()
{
	// TODO: Add your specialized code here.
	m_BeginTime = 0;
	m_EndTime = 0;

	if (NULL != dev)
	{
		Macro_RecSegReporterRelease(dev);
		dev = NULL;
	}
}

//##ModelId=49A63B9C00D0
int CDSegM::StartPop()
{
	// TODO: Add your specialized code here.

	int ret = 0;

	DEBUG_MSG("开始弹出\n");
	if (NULL == dev)
	{
		DEBUG_MSG("弹出器没有初始化\n");
		return -1;
	}

	ret = Macro_RecSegGet(dev, m_BeginTime, m_EndTime);

	return (int) ret;
}

//##ModelId=49A63BA40226
int CDSegM::StopPop()
{
	// TODO: Add your specialized code here.
	int ret = 0;

	if (NULL != dev)
	{
		ret = Macro_RecSegReporterRelease(dev);
		dev = NULL;
		SetAttr(m_nDiskGrouMask, m_CallBack, m_BeginTime, m_EndTime);
	}

	return (int) ret;
}


//##ModelId=49AF87DF01B6
int CDSegM::AddCh(int nCh, int bDynReport)
{
	int ret = 0;

	if (NULL == dev)
	{
		DEBUG_MSG("弹出器没有初始化\n");
		return -1;
	}

	if (MAX_ENC_CHANNEL_NUM <= nCh)
	{
		return IDVR_ERROR_PARAMETER;
	}

	m_nBindCh[nCh] = 1;

	ret = Macro_RecSegChanAdd(dev, nCh, bDynReport);

	return (int) ret;
}

//##ModelId=49AF8942018E
int CDSegM::DelCh(int nCh)
{
	// TODO: Add your specialized code here.
	int ret = 0;

	if (NULL == dev)
	{
		DEBUG_MSG("弹出器没有初始化\n");
		return -1;
	}

	if (MAX_ENC_CHANNEL_NUM <= nCh)
	{
		return IDVR_ERROR_PARAMETER;
	}

	m_nBindCh[nCh] = 0;
	ret = Macro_RecSegChanDel(dev, nCh);

	return (int) ret;
}

//##ModelId=49AF8A5E018C
int CDSegM::SetAttr(int DiskGroupMask, SEGMENT_CALLBACK CallBack, time_t tBeginTime, time_t tEndTime)
{
	// TODO: Add your specialized code here.
	int ret = 0;

	int i;

	DEBUG_MSG("DiskGroupMask = %d\n", DiskGroupMask);
	if (NULL != dev)
	{
		ret = Macro_RecSegReporterRelease(dev);
		dev = NULL;
	}

	dev = Macro_RecSegReporterCreate((PKGSTREAMGROUPTYPE) DiskGroupMask, CallBack, (void *) this);
	if (NULL == dev)
	{
		DEBUG_MSG("初始化弹出器失败\n");
		return -1;
	}
	m_nDiskGrouMask = DiskGroupMask;
	m_BeginTime = tBeginTime;
	m_EndTime = tEndTime;
	m_CallBack = CallBack;

	for (i = 0; i < MAX_ENC_CHANNEL_NUM; i++)
	{
		if (1 == m_nBindCh[i])
		{
			DEBUG_MSG("添加通道%d\n", i);
			ret = Macro_RecSegChanAdd(dev, i, 1);
		}
	}

	return (int) ret;
}

//##ModelId=49AF8EA30365
int CDSegM::ReleaseRecSegList(tagRecSeg * pRecSegHead)
{
	// TODO: Add your specialized code here.
	if (NULL != m_pRecSegHead)
	{
#ifdef USE_SD_PKG
		
#else
		Macro_ReleaseRecSegList((PSS_REGSEG *) pRecSegHead);
#endif
		m_pRecSegHead = NULL;
	}

	return (int) 0;
}

//##ModelId=49AF867C02F6
int CDSegM::GetRecSegList(int DiskGroupMask, int nChannel, time_t tBeginTime, time_t tEndTime, int RecType, tagRecSeg ** pRecSegHead, int *nCount)
{
	// TODO: Add your specialized code here.
	int ret = 0;
#ifdef USE_SD_PKG
	//ret = ZW_PkgSd_GetRecSegList(tBeginTime, tEndTime, RecType, (PSS_REGSEG_T **) pRecSegHead, nCount);
#else
	ret = Macro_GetRecSegList((PKGSTREAMGROUPTYPE) DiskGroupMask, nChannel, tBeginTime, tEndTime, RecType, (PSS_REGSEG **) pRecSegHead, nCount);
#endif
	return (int) ret;
}
