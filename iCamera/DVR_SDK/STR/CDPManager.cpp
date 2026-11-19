
/*!
*\file   CDPManager.cpp 
*\brief  数据处理实现
*\author cwf
*\date	  2009/03/24
*\version 0.1
*/
//#include <iostream.h>
#include "CDRecord.h"
#include "tagRecSeg.h"
#include "CDPManager.h"
#include "debug.h"
#include "CDPopM.h"
#include "CDSegM.h"
#include "CDiskGroup.h"
#include "idvr_errno.h"
#include "CPNodeTest.h"
#include "pthreadmacro.h"
#include "CDiskManager.h"
#include "str_fs.h"
#include "STR_global.h"

extern "C"
{
#include "usb.h"
}

/**************************************************************************
*                            宏定义                                       *
 **************************************************************************/
#define DISKGROUPMASKTOTYPE(DiskGroupMask)		\
if (0 == (DiskGroupMask))							\
{												\
	DiskGroupMask = DISKGROUP_NORMAL;			\
}															\
else if ( 1 == DiskGroupMask )								\
{															\
	DiskGroupMask = DISKGROUP_OVERPLUS;						\
}															\
else if ( 2 == DiskGroupMask )								\
{															\
	DiskGroupMask = DISKGROUP_BACKUP;						\
}

/**************************************************************************
*                            全局定义                                       *
 **************************************************************************/
CDPManager *CDPManager::theCDPManager = NULL;

/*************************************************************************
*                            方法实现                                     *
 **************************************************************************/

//!\brief 功能：打开指定磁盘分组类型的可查读属性.
//!\param GroupType 磁盘分组类型.
//!\return  成功为0，失败为-1
int CDPManager::DPM_SetDiskGroupReadable(int GroupType)
{
	int ret = -1;

	ret = Macro_SetDiskGroupReadable((PKGSTREAMGROUPTYPE) (GroupType + 1));

	return ret;
}

//!\brief   判断某天是否有录像数据
//!\param int DiskGroupMask 0表示查询普通分组+报警分组、1表示冗余分组、2表示备份分组
//!\param   int nYear
//!\param   int nMon
//!\param   int nDay
//!\return  有数据返回true,无数据返回false
//##ModelId=49A646100324
bool CDPManager::IsFileExist(int DiskGroupMask, int nYear, int nMon, int nDay)
{
	bool ret = false;

	tagTime start, stop;

	start.year = nYear;
	start.mon = nMon;
	start.day = nDay;
	start.hour = 0;
	start.min = 0;
	start.sec = 0;

	stop.year = nYear;
	stop.mon = nMon;
	stop.day = nDay;
	stop.hour = 23;
	stop.min = 59;
	stop.sec = 59;

	if (start.Check() || stop.Check())
	{
		return false;
	}

	DISKGROUPMASKTOTYPE(DiskGroupMask);
#ifdef __PKGSTREAM_STORTAGE__
	ret = (0 == Macro_JudgeIFHaveDataByTime((PKGSTREAMGROUPTYPE) DiskGroupMask, start.MkSecond(), stop.MkSecond()));
#elif defined(__JFS_STORAGE__)
	ret = CRecordFile::IsFileExist(DiskGroupMask, nYear, nMon, nDay);
#else
#error not define __PKGSTREAM_STORTAGE__ or __JFS_STORAGE__
#endif

	return ret;
}


//!\brief   判断某月是否有录像数据
//!\param int DiskGroupMask 0表示查询普通分组+报警分组、1表示冗余分组、2表示备份分组
//!\param   int nYear
//!\param   int nMon
//!\return  0-30分别代表1日制31日
//##ModelId=49A646100324
int CDPManager::IsFileExist(int DiskGroupMask, int nYear, int nMon)
{
	unsigned int ret = 0;

	DISKGROUPMASKTOTYPE(DiskGroupMask);

#ifdef __PKGSTREAM_STORTAGE__
	HY_PkgStream_JudgeIFHaveDataByDays((PKGSTREAMGROUPTYPE) DiskGroupMask, start.MkSecond(), 31, &ret);
#elif defined(__JFS_STORAGE__)
	ret = CRecordFile::IsFileExist(DiskGroupMask, nYear, nMon);
#else
#error not define __PKGSTREAM_STORTAGE__ or __JFS_STORAGE__
#endif

	return ret;
}


/*!
 * \brief 获取指定分组类型是否有指定天数(一般用于一个月)的录像数据
 * \param GroupType 指定的分组类型
 * \param tBeginTime 指定时间段的开始时间
 * \param numOfdays 从起始天开始的天数(24小时)
 * \param pResult 返回结果，采用位与方式bit0-bit30分别对应31天
 * \return 成功返回0，其他返回小于0的错误码。
 */
//PSS_API int API_CALL HY_PkgStream_JudgeIFHaveDataByDays(PKGSTREAMGROUPTYPE GroupType, time_t tBeginTime, unsigned int numOfdays, unsigned int *pResult);

//!\brief 获取弹出器中的数据大小（KB）
//!\return  成功为数据大小，失败为-1
//##ModelId=49B8C59C0274
long long CDPManager::DPM_GetSize(void *hHandle)
{
	long long ret = 0;

	CDPopM *theCDPopM = (CDPopM *) hHandle;

	if (NULL == hHandle)
	{
		DEBUG_MSG("无效的指针\n");
		return -1;
	}


	ret = theCDPopM->GetSize();

	return (long long) ret;

}

int CDPManager::DPM_SetAttr(void *hHandle, int DiskGroupMask, int nRefChan, time_t tStartSeekTime, time_t tEndSeekTime, DATAPOP_CALLBACK PopCallBack)
{
	int bDynPop = 1;

	int ret = 0;

	CDPopM *theCDPopM = (CDPopM *) hHandle;

	if (DiskGroupMask < 0 || DiskGroupMask > 2)
	{
		DEBUG_MSG("无效的参数:%d\n", DiskGroupMask);
		return IDVR_ERROR_PARAMETER;
	}

	if (NULL == hHandle)
	{
		DEBUG_MSG("无效的指针\n");
		return IDVR_ERROR_PARAMETER_INVALID_POINTER;
	}

	DISKGROUPMASKTOTYPE(DiskGroupMask);

	ret = theCDPopM->SetAttr(DiskGroupMask, nRefChan, tStartSeekTime, tEndSeekTime, PopCallBack, bDynPop);

	return (int) ret;
}

int CDPManager::DPM_SetAttr(void *hHandle, int DiskGroupMask, int nRefChan, time_t tStartSeekTime, time_t tEndSeekTime, DATAPOP_CALLBACK PopCallBack,
							int bDynPop)
{
	int ret = 0;

	CDPopM *theCDPopM = (CDPopM *) hHandle;

	if (DiskGroupMask < 0 || DiskGroupMask > 2)
	{
		DEBUG_MSG("无效的参数:%d\n", DiskGroupMask);
		return IDVR_ERROR_PARAMETER;
	}

	if (NULL == hHandle)
	{
		DEBUG_MSG("无效的指针\n");
		return IDVR_ERROR_PARAMETER_INVALID_POINTER;
	}

	DISKGROUPMASKTOTYPE(DiskGroupMask);

	ret = theCDPopM->SetAttr(DiskGroupMask, nRefChan, tStartSeekTime, tEndSeekTime, PopCallBack, bDynPop);

	return (int) ret;
}

void *CDPManager::DPM_Create()
{
	CDPopM *theCDPopM = NULL;

#ifndef TEST_MEM_INFO
	theCDPopM = new CDPopM();
#else
	theCDPopM = CommonKit::TCreateObject<CDPopM>::CreateObject(__FILE__, __FUNCTION__, __LINE__);
#endif

	if (0 != theChildObj->Insert((void *) theCDPopM))
	{
#ifndef TEST_MEM_INFO
		delete theCDPopM;
#else
		CommonKit::TDestroyObject<CDPopM>::DestroyObject(theCDPopM);
#endif
		theCDPopM = NULL;

		return NULL;
	}

	theChildObj->Insert((void *) theCDPopM);

	return (void *) theCDPopM;
}

int CDPManager::DPM_Destroy(void *hHandle)
{
	CDPopM *theCDPopM = (CDPopM *) hHandle;

	if (false == theChildObj->IsExist(hHandle))
	{
		DEBUG_MSG("野指针\n");
		return IDVR_ERROR_PARAMETER_INVALID_POINTER;
	}

	if (NULL == hHandle)
	{
		DEBUG_MSG("无效的指针\n");
		return IDVR_ERROR_PARAMETER_INVALID_POINTER;
	}

	theChildObj->Delete(hHandle);

	theCDPopM->Release();

#ifndef TEST_MEM_INFO
	delete theCDPopM;
#else
	CommonKit::TDestroyObject<CDPopM>::DestroyObject(theCDPopM);
#endif
	theCDPopM = NULL;

	return (int) 0;
}

int CDPManager::DPM_AddCh(void *hHandle, int nChannel, int bDynPop)
{
	int ret = 0;

	CDPopM *theCDPopM = (CDPopM *) hHandle;

	if (false == theChildObj->IsExist(hHandle))
	{
		DEBUG_MSG("野指针\n");
		return IDVR_ERROR_PARAMETER_INVALID_POINTER;
	}

	if (NULL == hHandle)
	{
		DEBUG_MSG("无效的指针\n");
		return IDVR_ERROR_PARAMETER_INVALID_POINTER;
	}

	ret = theCDPopM->AddCh(nChannel);

	return (int) ret;
}

int CDPManager::DPM_DelCh(void *hHandle, int nChannel)
{
	int ret = 0;

	CDPopM *theCDPopM = (CDPopM *) hHandle;

	if (false == theChildObj->IsExist(hHandle))
	{
		DEBUG_MSG("野指针\n");
		return IDVR_ERROR_PARAMETER_INVALID_POINTER;
	}

	if (NULL == hHandle)
	{
		DEBUG_MSG("无效的指针\n");
		return IDVR_ERROR_PARAMETER_INVALID_POINTER;
	}

	ret = theCDPopM->DelCh(nChannel);

	return (int) ret;
}

int CDPManager::DPM_Seek(void *hHandle, time_t SeekTime)
{
	int ret = 0;

	CDPopM *theCDPopM = (CDPopM *) hHandle;


	if (false == theChildObj->IsExist(hHandle))
	{
		DEBUG_MSG("野指针\n");
		return IDVR_ERROR_PARAMETER_INVALID_POINTER;
	}

	if (NULL == hHandle)
	{
		DEBUG_MSG("无效的指针\n");
		return IDVR_ERROR_PARAMETER_INVALID_POINTER;
	}

	ret = theCDPopM->Seek(SeekTime);

	return (int) ret;
}

int CDPManager::DPM_AddRecType(void *hHandle, int RecType)
{
	int ret = 0;

	CDPopM *theCDPopM = (CDPopM *) hHandle;


	if (false == theChildObj->IsExist(hHandle))
	{
		DEBUG_MSG("野指针\n");
		return IDVR_ERROR_PARAMETER_INVALID_POINTER;
	}

	if (NULL == hHandle)
	{
		DEBUG_MSG("无效的指针\n");
		return IDVR_ERROR_PARAMETER_INVALID_POINTER;
	}

	ret = theCDPopM->DPM_AddRecType(RecType);

	return (int) ret;
}

int CDPManager::DPM_DelRecType(void *hHandle, int RecType)
{
	int ret = 0;

	CDPopM *theCDPopM = (CDPopM *) hHandle;


	if (false == theChildObj->IsExist(hHandle))
	{
		DEBUG_MSG("野指针\n");
		return IDVR_ERROR_PARAMETER_INVALID_POINTER;
	}

	if (NULL == hHandle)
	{
		DEBUG_MSG("无效的指针\n");
		return IDVR_ERROR_PARAMETER_INVALID_POINTER;
	}

	ret = theCDPopM->DPM_DelRecType(RecType);

	return (int) ret;
}

int CDPManager::DPM_StartPop(void *hHandle)
{
	int ret = 0;

	CDPopM *theCDPopM = (CDPopM *) hHandle;


	if (false == theChildObj->IsExist(hHandle))
	{
		DEBUG_MSG("野指针\n");
		return IDVR_ERROR_PARAMETER_INVALID_POINTER;
	}

	if (NULL == hHandle)
	{
		DEBUG_MSG("无效的指针\n");
		return IDVR_ERROR_PARAMETER_INVALID_POINTER;
	}

	ret = theCDPopM->StartPop();

	return (int) ret;
}

int CDPManager::DPM_StopPop(void *hHandle)
{
	int ret = 0;

	CDPopM *theCDPopM = (CDPopM *) hHandle;

	if (false == theChildObj->IsExist(hHandle))
	{
		DEBUG_MSG("野指针\n");
		return IDVR_ERROR_PARAMETER_INVALID_POINTER;
	}

	if (NULL == hHandle)
	{
		DEBUG_MSG("无效的指针\n");
		return IDVR_ERROR_PARAMETER_INVALID_POINTER;
	}

	ret = theCDPopM->StopPop();
	return (int) ret;
}


//!\brief 获取一帧关键帧
//!\param hDataPopper 数据弹出器的句柄
//!\param iChannel 通道号
//!\param tSeekTime 指定时间簇
//!\param precision 时间簇误差
//!\param pbuf 保存该关键帧的缓冲区指针
//!\param bufsize 输入时为缓冲区大小，输出时为关键帧大小
//!\return 成功返回0，失败返回小于0的错误码。
int CDPManager::DPM_GetOneKeyFrame(HANDLE hHandle, int iChannel, time_t tSeekTime, unsigned int precision, unsigned char *pbuf, unsigned int *bufsize)
{
	int ret = 0;

	CDPopM *theCDPopM = (CDPopM *) hHandle;


	if (false == theChildObj->IsExist(hHandle))
	{
		DEBUG_MSG("野指针\n");
		return IDVR_ERROR_PARAMETER_INVALID_POINTER;
	}

	if (NULL == hHandle)
	{
		DEBUG_MSG("无效的指针\n");
		return IDVR_ERROR_PARAMETER_INVALID_POINTER;
	}

	//调用底层库接口获取一个I帧
	ret = theCDPopM->GetOneKeyFrame(hHandle, iChannel, tSeekTime, precision, pbuf, bufsize);
	WARN_MSG(0 == ret);
	return (int) ret;
}

//!\brief 设置弹出器弹出数据类型
//!\param void* hHandle 弹出器句柄
//!\param int nDataType 数据类型。0为所有帧，1为I帧
//!\param PopKeyInterval 当只弹出关键帧时，该参数有效，表示通道弹出每隔多少关键帧弹出一个，此值限制在0-3，0表示全部弹出
//!\return  成功为0，失败为-1
//##ModelId=49B8C58401F7
int CDPManager::DPM_SetPopDataType(void *hHandle, int nDataType, unsigned int PopKeyInterval)
{
	int ret = 0;

	CDPopM *theCDPopM = (CDPopM *) hHandle;


	if (false == theChildObj->IsExist(hHandle))
	{
		DEBUG_MSG("野指针\n");
		return IDVR_ERROR_PARAMETER_INVALID_POINTER;
	}

	if (NULL == hHandle)
	{
		DEBUG_MSG("无效的指针\n");
		return IDVR_ERROR_PARAMETER_INVALID_POINTER;
	}

	ret = theCDPopM->SetPopDataType(nDataType, PopKeyInterval);
	return (int) ret;
}

int CDPManager::DPM_SetPopDataDirection(void *hHandle, bool bFront)
{
	int ret = 0;

	CDPopM *theCDPopM = (CDPopM *) hHandle;


	if (false == theChildObj->IsExist(hHandle))
	{
		DEBUG_MSG("野指针\n");
		return IDVR_ERROR_PARAMETER_INVALID_POINTER;
	}

	if (NULL == hHandle)
	{
		DEBUG_MSG("无效的指针\n");
		return IDVR_ERROR_PARAMETER_INVALID_POINTER;
	}

	ret = theCDPopM->SetPopDataDirection(bFront == 0);

	return (int) ret;
}

void *CDPManager::DSM_Create()
{
	CDSegM *theCDSegM = NULL;

#ifndef TEST_MEM_INFO
	theCDSegM = new CDSegM();
#else
	theCDSegM = CommonKit::TCreateObject<CDSegM>::CreateObject(__FILE__, __FUNCTION__, __LINE__);
#endif

	if (0 != theChildObj->Insert((void *) theCDSegM))
	{
#ifndef TEST_MEM_INFO
		delete theCDSegM;
#else
		CommonKit::TDestroyObject<CDSegM>::DestroyObject(theCDSegM);
#endif
		theCDSegM = NULL;

		return NULL;
	}


	return (void *) theCDSegM;
}

int CDPManager::DSM_Destroy(void *hHandle)
{
	CDSegM *theCDSegM = (CDSegM *) hHandle;

	if (false == theChildObj->IsExist(hHandle))
	{
		DEBUG_MSG("野指针\n");
		return IDVR_ERROR_PARAMETER_INVALID_POINTER;
	}

	if (NULL == hHandle)
	{
		DEBUG_MSG("无效的指针\n");
		return IDVR_ERROR_PARAMETER_INVALID_POINTER;
	}

	theChildObj->Delete(hHandle);

#ifndef TEST_MEM_INFO
	delete theCDSegM;
#else
	CommonKit::TDestroyObject<CDSegM>::DestroyObject(theCDSegM);
#endif
	theCDSegM = NULL;

	return (int) 0;
}

int CDPManager::DSM_SetAttr(void *hHandle, int DiskGroupMask, time_t tStartSeekTime, time_t tEndSeekTime, SEGMENT_CALLBACK CallBack)
{
	int ret = 0;

	CDSegM *theCDSegM = (CDSegM *) hHandle;

	if (false == theChildObj->IsExist(hHandle))
	{
		DEBUG_MSG("野指针\n");
		return IDVR_ERROR_PARAMETER_INVALID_POINTER;
	}

	if (DiskGroupMask < 0 || DiskGroupMask > 2)
	{
		DEBUG_MSG("无效的参数:%d\n", DiskGroupMask);
		return IDVR_ERROR_PARAMETER;
	}

	if (NULL == hHandle)
	{
		DEBUG_MSG("无效的指针\n");
		return IDVR_ERROR_PARAMETER_INVALID_POINTER;
	}

	DISKGROUPMASKTOTYPE(DiskGroupMask);

	ret = theCDSegM->SetAttr(DiskGroupMask, CallBack, tStartSeekTime, tEndSeekTime);

	return (int) ret;
}

int CDPManager::DSM_AddCh(void *hHandle, int nChannel, int bDynReport)
{
	int ret = 0;

	CDSegM *theCDSegM = (CDSegM *) hHandle;

	if (false == theChildObj->IsExist(hHandle))
	{
		DEBUG_MSG("野指针\n");
		return IDVR_ERROR_PARAMETER_INVALID_POINTER;
	}

	if (NULL == hHandle)
	{
		DEBUG_MSG("无效的指针\n");
		return IDVR_ERROR_PARAMETER_INVALID_POINTER;
	}

	ret = theCDSegM->AddCh(nChannel, bDynReport);

	return (int) ret;
}

int CDPManager::DSM_DelCh(void *hHandle, int nChannel)
{
	int ret = 0;

	CDSegM *theCDSegM = (CDSegM *) hHandle;

	if (false == theChildObj->IsExist(hHandle))
	{
		DEBUG_MSG("野指针\n");
		return IDVR_ERROR_PARAMETER_INVALID_POINTER;
	}

	if (NULL == hHandle)
	{
		DEBUG_MSG("无效的指针\n");
		return IDVR_ERROR_PARAMETER_INVALID_POINTER;
	}

	ret = theCDSegM->DelCh(nChannel);

	return (int) ret;
}

int CDPManager::DSM_StartPop(void *hHandle)
{
	int ret = 0;

	CDSegM *theCDSegM = (CDSegM *) hHandle;

	if (false == theChildObj->IsExist(hHandle))
	{
		DEBUG_MSG("野指针\n");
		return IDVR_ERROR_PARAMETER_INVALID_POINTER;
	}

	if (NULL == hHandle)
	{
		DEBUG_MSG("无效的指针\n");
		return IDVR_ERROR_PARAMETER_INVALID_POINTER;
	}

	ret = theCDSegM->StartPop();

	return (int) ret;
}

int CDPManager::DSM_StopPop(void *hHandle)
{
	int ret = 0;

	CDSegM *theCDSegM = (CDSegM *) hHandle;

	if (false == theChildObj->IsExist(hHandle))
	{
		DEBUG_MSG("野指针\n");
		return IDVR_ERROR_PARAMETER_INVALID_POINTER;
	}

	if (NULL == hHandle)
	{
		DEBUG_MSG("无效的指针\n");
		return IDVR_ERROR_PARAMETER_INVALID_POINTER;
	}

	ret = theCDSegM->StopPop();

	return (int) ret;
}

int CDPManager::DSM_GetRecSegList(int DiskGroupMask, int nChannel, time_t tBeginTime, time_t tEndTime, int RecType, tagRecSeg * *pRecSegHead,
								  int *nCount)
{
	int ret = 0;


	if (DiskGroupMask < 0 || DiskGroupMask > 2)
	{
		DEBUG_MSG("无效的参数:%d\n", DiskGroupMask);
		return IDVR_ERROR_PARAMETER;
	}

	DISKGROUPMASKTOTYPE(DiskGroupMask);
	ret = CDSegM::GetRecSegList(DiskGroupMask, nChannel, tBeginTime, tEndTime, RecType, pRecSegHead, nCount);
	return (int) ret;
}

int CDPManager::DSM_ReleaseRecSegList(tagRecSeg * pRecSegHead)
{
	int ret = 0;

	ret = CDSegM::ReleaseRecSegList(pRecSegHead);;

	return (int) ret;
}

//!\brief 设置录像类型
int CDPManager::DR_SetRecordType(int nCh, int nRecType)
{
	int ret = 0;

	if (NULL == m_CDRecord)
	{
		DEBUG_MSG("无效的指针\n");
		return IDVR_ERROR_PARAMETER_INVALID_POINTER;
	}

	ret = m_CDRecord[nCh].SetRecordType(nCh, nRecType);
	WARN_MSG(0 == ret);

	return (int) ret;
}

/* 触发录像完毕之后，发送消息的回调函数 */
int CDPManager::DR_SetTrigRecordOverCallBack(int nCh, int (*CallBack)(unsigned char *))
{
	int ret = 0;

	if (NULL == m_CDRecord)
	{
		DEBUG_MSG("无效的指针\n");
		return IDVR_ERROR_PARAMETER_INVALID_POINTER;
	}

	ret = m_CDRecord[nCh].SetTrigRecordOverCallBack(CallBack);
	WARN_MSG(0 == ret);

	return (int) ret;
}


//!\brief 设置预录时间(单位: 秒)
int CDPManager::DR_SetPrevTimeSec(int nCh, unsigned int nPrevTimeSec)
{
	int ret = 0;

	if (NULL == m_CDRecord)
	{
		DEBUG_MSG("无效的指针\n");
		return IDVR_ERROR_PARAMETER_INVALID_POINTER;
	}

	ret = m_CDRecord[nCh].SetPrevTimeSec(nCh, nPrevTimeSec);
	WARN_MSG(0 == ret);

	return (int) ret;
}

//!\brief 获取预录时间(单位: 秒)
unsigned int CDPManager::DR_GetPrevTimeSec(int nCh)
{
	int ret = 0;

	if (NULL == m_CDRecord)
	{
		DEBUG_MSG("无效的指针\n");
		return IDVR_ERROR_PARAMETER_INVALID_POINTER;
	}

	ret = m_CDRecord[nCh].GetPrevTimeSec(nCh);
	WARN_MSG(0 == ret);

	return (unsigned int) ret;
}

//!\brief 设置录像延迟时间（秒）
int CDPManager::DR_SetRecordDelayTime (int nCh, unsigned int nDelaySec)
{
	int ret = 0;

	if (NULL == m_CDRecord)
	{
		DEBUG_MSG("无效的指针\n");
		return IDVR_ERROR_PARAMETER_INVALID_POINTER;
	}
	ret = m_CDRecord[nCh].SetRecordDelayTime (nCh, nDelaySec);
	WARN_MSG(0 == ret);
	return ret;
}

//!\brief 获取录像延迟时间（秒）
int CDPManager::DR_GetRecordDelayTime (int nCh, unsigned int *pnDelaySec)
{
	int ret = 0;

	if (NULL == m_CDRecord)
	{
		DEBUG_MSG("无效的指针\n");
		return IDVR_ERROR_PARAMETER_INVALID_POINTER;
	}
	ret = m_CDRecord[nCh].GetRecordDelayTime (nCh, pnDelaySec);
	WARN_MSG(0 == ret);
	return ret;
}

//!\brief 设置录像写入状态 0:暂停  1:写入
int CDPManager::DR_SetRecordWriteStatus(int nCh, int record_write_status)
{
	int ret = 0;

	if (NULL == m_CDRecord)
	{
		DEBUG_MSG("无效的指针\n");
		return IDVR_ERROR_PARAMETER_INVALID_POINTER;
	}
	ret = m_CDRecord[nCh].SetRecordWriteStatus(record_write_status);
	WARN_MSG(0 == ret);
	return ret;
}

/*@breif DR_SetCurrentAviNumPerPage 设置每页显示多少个AVI文件
 *@param [IN]  avi_num_per_page 每页显示的AVI文件个数
 *@return 成功返回0
 *@       失败返回-1
 *@note  每页显示的AVI文件个数不能超过当前有效的AVI个数g_read_curr_avi_num，
 *@      每页显示的AVI文件个数不能超过最大能显示的AVI个数MAX_AVI_NUM_ONE_PAGE
 */
int CDPManager::DR_SetCurrentAviNumPerPage(int avi_num_per_page)
{
    int ret = 0;

    if (NULL == m_CDRecord)
    {
        DEBUG_MSG("无效的指针\n");
        return IDVR_ERROR_PARAMETER_INVALID_POINTER;
    }

    ret = m_CDRecord[0].SetCurrentAviNumPerPage(avi_num_per_page);
    WARN_MSG(0 == ret);

    return (unsigned int) ret;
}

/*@breif DR_GetCurrentAviTotalNum 获取当前AVI总个数
 *@return 成功返回当前总的AVI文件个数
 *@       失败返回-1
 */
int CDPManager::DR_GetCurrentAviTotalNum(void)
{
    int ret = 0;

    if (NULL == m_CDRecord)
    {
        DEBUG_MSG("无效的指针\n");
        return IDVR_ERROR_PARAMETER_INVALID_POINTER;
    }

    ret = m_CDRecord[0].GetCurrentAviTotalNum();

    return (unsigned int) ret;
}

/*@breif DR_SetSwitchPageIsChoose 设置当前选中的是哪一个页面和当前录像路径类型
 *@param [IN]  choose_page 选中的页面(从1开始)
 *@param [IN]  path_type 当前录像路径类型 0: http路径  1:绝对路径
 *@return 成功返回0
 *@      失败返回-1
 */
int CDPManager::DR_SetSwitchPageIsChoose(int choose_page, int path_type)
{
    int ret = 0;

    if (NULL == m_CDRecord)
    {
        DEBUG_MSG("无效的指针\n");
        return IDVR_ERROR_PARAMETER_INVALID_POINTER;
    }

    ret = m_CDRecord[0].SetSwitchPageIsChoose(choose_page, path_type);

    return ret;
}

/*@breif DR_UpdateAviListByTime 通过给定的时间戳更新AVI文件列表
 *@param [IN]  start_time 要搜索的起始时间
 *@param [IN]  stop_time 要搜索的结束时间(为0表示搜索全部文件)
 *@param [OUT] p_search_avi_count 搜索到符合条件的AVI文件总个数
 *@return 成功返回0
 *@      失败返回-1
 */
int CDPManager::DR_UpdateAviListByTime(unsigned int start_time, 
                                              unsigned int stop_time, 
                                              int *p_search_avi_count)
{
    if (NULL == m_CDRecord)
    {
        DEBUG_MSG("无效的指针\n");
        return IDVR_ERROR_PARAMETER_INVALID_POINTER;
    }

    int ret = m_CDRecord[0].UpdateAviListByTime(start_time,
                                                stop_time,
                                                p_search_avi_count);
    WARN_MSG(0 == ret);

    return ret;
}

/*@breif DR_GetCurrentAviListInfo 通过给定的时间戳获取对应的AVI文件列表
 *@param [IN]  read_avi_page 需要获取第几页的AVI信息(从1开始)
 *@param [OUT] p_avi_list 获取到的AVI文件列表
 *@param [OUT] p_avi_num_curr_page 当前页面有多少个AVI文件
 *@return 成功返回0
 *@      失败返回-1
 *@note  需要先调用SetCurrentAviNumPerPage设置当前用户需要设置每页显示多少个AVI文件
 */
int CDPManager::DR_GetCurrentAviListInfo(             int read_avi_page, void **p_avi_list, int *p_avi_num_curr_page)
{
    if (NULL == m_CDRecord)
    {
        DEBUG_MSG("无效的指针\n");
        return IDVR_ERROR_PARAMETER_INVALID_POINTER;
    }

    int ret = m_CDRecord[0].GetCurrentAviListInfo(read_avi_page, p_avi_list, p_avi_num_curr_page);
    WARN_MSG(0 == ret);

    return ret;
}

/*@breif DR_SetCurrentPicNumPerPage 设置每页显示多少个图片文件
 *@param [IN]  pic_num_per_page 每页显示的图片文件个数
 *@return 成功返回0
 *@       失败返回-1
 *@note  每页显示的图片文件个数不能超过当前有效的图片个数g_read_curr_pic_num，
 *@      每页显示的图片文件个数不能超过最大能显示的图片个数MAX_PIC_NUM_ONE_PAGE
 */
int CDPManager::DR_SetCurrentPicNumPerPage(int pic_num_per_page)
{
    int ret = 0;

    if (NULL == m_CDRecord)
    {
        DEBUG_MSG("无效的指针\n");
        return IDVR_ERROR_PARAMETER_INVALID_POINTER;
    }

    ret = m_CDRecord[0].SetCurrentPicNumPerPage(pic_num_per_page);
    WARN_MSG(0 == ret);

    return (unsigned int) ret;
}

/*@breif DR_GetCurrentPicTotalNum 获取当前图片总个数
 *@return 成功返回当前总的图片文件个数
 *@       失败返回-1
 */
int CDPManager::DR_GetCurrentPicTotalNum(void)
{
    int ret = 0;

    if (NULL == m_CDRecord)
    {
        DEBUG_MSG("无效的指针\n");
        return IDVR_ERROR_PARAMETER_INVALID_POINTER;
    }

    ret = m_CDRecord[0].GetCurrentPicTotalNum();

    return (unsigned int) ret;
}

/*@breif DR_UpdatePicListByTime 通过给定的时间戳更新PIC文件列表
 *@param [IN]  start_time 要搜索的起始时间
 *@param [IN]  stop_time 要搜索的结束时间(为0表示搜索全部文件)
 *@param [OUT] p_search_pic_count 搜索到符合条件的PIC文件总个数
 *@return 成功返回0
 *@      失败返回-1
 */
int CDPManager::DR_UpdatePicListByTime(unsigned int start_time, 
                                              unsigned int stop_time, 
                                              int *p_search_pic_count)
{
    if (NULL == m_CDRecord)
    {
        DEBUG_MSG("无效的指针\n");
        return IDVR_ERROR_PARAMETER_INVALID_POINTER;
    }

    int ret = m_CDRecord[0].UpdatePicListByTime(start_time,
                                                stop_time,
                                                p_search_pic_count);
    WARN_MSG(0 == ret);

    return ret;
}


/*@breif DR_GetCurrentPicListInfo 通过给定的时间戳获取对应的图片文件列表
 *@param [IN]  read_pic_page 需要获取第几页的图片信息(从1开始)
 *@param [OUT] p_pic_list 获取到的图片文件列表
 *@param [OUT] p_pic_num_curr_page 当前页面有多少个图片文件 
 *@return 成功返回0
 *@      失败返回-1
 *@note  需要先调用GetPicGlobalHeadInfoFromGlobalFile将需要的数据放到全局结构体里面
 *@      需要先调用SetCurrentPicNumPerPage设置当前用户需要设置每页显示多少个图片文件
 */
int CDPManager::DR_GetCurrentPicListInfo(int read_pic_page, void **p_pic_list, int *p_pic_num_curr_page)
{
    if (NULL == m_CDRecord)
    {
        DEBUG_MSG("无效的指针\n");
        return IDVR_ERROR_PARAMETER_INVALID_POINTER;
    }

    int ret = m_CDRecord[0].GetCurrentPicListInfo(read_pic_page, p_pic_list, p_pic_num_curr_page);
    WARN_MSG(0 == ret);

    return (unsigned int) ret;
}

/* 获取磁盘资源占用情况(单位: MB) */
int CDPManager::DR_GetDiskResourceSize(int *pRecordMB, int *pPictureMB)
{
    if (NULL == m_CDRecord)
    {
        DEBUG_MSG("无效的指针\n");
        return IDVR_ERROR_PARAMETER_INVALID_POINTER;
    }

    int ret = m_CDRecord[0].GetDiskResourceSize(pRecordMB, pPictureMB);

    return ret;
}

/* 获取录像存放的比例(范围0~100) */
int CDPManager::DR_GetVideoUsedSpaceRate(void)
{
    if (NULL == m_CDRecord)
    {
        DEBUG_MSG("无效的指针\n");
        return IDVR_ERROR_PARAMETER_INVALID_POINTER;
    }

    int video_rate = m_CDRecord[0].GetVideoUsedSpaceRate();

    return (unsigned int) video_rate;
}

/* 设置录像存放的比例(范围0~100) */
int CDPManager::DR_SetVideoUsedSpaceRate(int nVideoSpace)
{
    if (NULL == m_CDRecord)
    {
        DEBUG_MSG("无效的指针\n");
        return IDVR_ERROR_PARAMETER_INVALID_POINTER;
    }
    int ret = m_CDRecord[0].SetVideoUsedSpaceRate(nVideoSpace);
    WARN_MSG(0 == ret);

    return (unsigned int) ret;
}

/* 设置录像尺寸 */
int CDPManager::DR_SetRecordSize(int width, int height, int bH265)
{
    if (NULL == m_CDRecord)
    {
        DEBUG_MSG("无效的指针\n");
        return IDVR_ERROR_PARAMETER_INVALID_POINTER;
    }
    int ret = m_CDRecord[0].SetRecordSize(width, height, bH265);
    WARN_MSG(0 == ret);

    return (unsigned int) ret;
}

/*@breif DR_GetFileReadWriteStatus 获取当前文件的读写状态
 *@param [IN]  file_name 需要操作的文件名(绝对路径)
 *@param [OUT] read_write_status 
 *@            0:  当前文件空闲
 *@            1:  当前文件正在读
 *@            2:  当前文件正在写
 *@param [OUT] file_size 文件大小(单位: 字节) 
 *@return 返回0表示成功
 *@      返回-1表示失败
 */
int CDPManager::DR_GetFileReadWriteStatus(const char *file_name, 
                                                  unsigned int *read_write_status,
                                                  unsigned int *file_size)
{
    if (NULL == m_CDRecord)
    {
        DEBUG_MSG("无效的指针\n");
        return IDVR_ERROR_PARAMETER_INVALID_POINTER;
    }

    return m_CDRecord[0].GetFileReadWriteStatus(file_name, read_write_status, file_size);
}

/*@breif DR_GetRecordDataSize 获取指定时间段的录像数据大小
 *@param [IN] start_time 开始时间
 *@param [IN] stop_time  结束时间
 *@param [IN] record_type 筛选的录像类型(当前无效)
 *@param [OUT] p_size 获取到的帧数据大小
 *@return 返回  0 表示获取成功
 *@       返回 -1 表示获取失败
 *@note   获取到的是数据帧大小，包括帧头
 */
int CDPManager::DR_GetRecordDataSize(unsigned int start_time, 
                                                unsigned int stop_time, 
                                                int record_type, 
                                                unsigned int * p_size)
{
    if (NULL == m_CDRecord)
    {
        DEBUG_MSG("无效的指针\n");
        return IDVR_ERROR_PARAMETER_INVALID_POINTER;
    }

    return m_CDRecord[0].GetRecordDataSize(start_time, stop_time, record_type, p_size);
}

/* 获取当前回放的录像信息 */
int CDPManager::DR_GetRecordInfo(int *pCurrentRecordStartTime)
{
    if (NULL == m_CDRecord)
    {
        DEBUG_MSG("无效的指针\n");
        return IDVR_ERROR_PARAMETER_INVALID_POINTER;
    }

    return m_CDRecord[0].GetRecordInfo(pCurrentRecordStartTime);
}

/* 获取文件对应的文件信息 */
int CDPManager::DR_GetRecordFileInfoByFileName(const char* pFileName, void* pFileInfo)
{
    if (NULL == m_CDRecord)
    {
        DEBUG_MSG("无效的指针\n");
        return IDVR_ERROR_PARAMETER_INVALID_POINTER;
    }

    return m_CDRecord[0].GetRecordFileInfoByFileName(pFileName, pFileInfo);
}


/* 设置获取时间偏移回调 */
int CDPManager::DR_SetGetTimeOffsetCallBack(int (*CallBack)(int *, int *, int *))
{
    if (NULL == m_CDRecord)
    {
        DEBUG_MSG("无效的指针\n");
        return IDVR_ERROR_PARAMETER_INVALID_POINTER;
    }

    return m_CDRecord[0].SetGetTimeOffsetCallBack(CallBack);
}

/* 传入的图片名字组成pic_name: 时间戳_标号，如:1532477851_01需要改为1532477851 */
int CDPManager::DR_UploadPictureToSdCard(const char *pic_name, unsigned int trig_type)
{
    if (NULL == m_CDRecord)
    {
        DEBUG_MSG("无效的指针\n");
        return IDVR_ERROR_PARAMETER_INVALID_POINTER;
    }
    int ret = m_CDRecord[0].UploadPictureToSdCard(pic_name, trig_type);
    WARN_MSG(0 == ret);

    return (unsigned int) ret;
}

//!\brief 暂停记录数据
//!\param 无
//!\return  成功返回0，失败为-1
//##ModelId=49A645FD00D0

int CDPManager::PauseRecord(int nCh, int bPause)
{
	int ret = 0;

	if (NULL == m_CDRecord)
	{
		DEBUG_MSG("无效的指针\n");
		return IDVR_ERROR_PARAMETER_INVALID_POINTER;
	}

	ret = m_CDRecord[nCh].PauseRecord(nCh, bPause);


	return (int) ret;
}

//##ModelId=49AF9963016D
int CDPManager::DR_StartRecordData(int nCh, int record_type, int bH265)
{
	int ret = 0;

	if (NULL == m_CDRecord)
	{
		DEBUG_MSG("无效的指针\n");
		return IDVR_ERROR_PARAMETER_INVALID_POINTER;
	}

	ret = m_CDRecord[nCh].StartRecordData(nCh, record_type, bH265);

	return (int) ret;
}

//##ModelId=49AF996A0181
int CDPManager::DR_DelRecordData(int nDiskGroupType, int nCh, int nRecType, time_t tStartTime, time_t tEndTime)
{
	int ret = 0;

	if (NULL == m_CDRecord)
	{
		DEBUG_MSG("无效的指针\n");
		return IDVR_ERROR_PARAMETER_INVALID_POINTER;
	}

	DEBUG_MSG("盘组%d通道%d删除数据%d\n", nDiskGroupType, nCh, nRecType);
	ret = m_CDRecord[nCh].DelRecordData(CDiskGroup::handle[nDiskGroupType]->dev, nCh, nRecType, tStartTime, tEndTime);

	return (int) ret;
}

//##ModelId=49AF996A0181
int CDPManager::DR_DelRecordData(int nDiskGroupType, time_t tStartTime, time_t tEndTime)
{
	int ret = 0;

	if (NULL == m_CDRecord)
	{
		DEBUG_MSG("无效的指针\n");
		return IDVR_ERROR_PARAMETER_INVALID_POINTER;
	}

	ret = CDRecord::DelRecordData(CDiskGroup::handle[nDiskGroupType]->dev, tStartTime, tEndTime);

	return (int) ret;
}

int CDPManager::DR_InputDataToRingBuffer(int nCh, time_t tFrameTime, unsigned long tFrameTimeInUs, const char *Buff, int nLen, int nFramType)
{
	int ret = 0;

	if (NULL == m_CDRecord)
	{
		DEBUG_MSG("无效的指针\n");
		return IDVR_ERROR_PARAMETER_INVALID_POINTER;
	}

	ret = m_CDRecord[nCh].InputDataToRingBuffer(Buff, nLen);

	return ret;
}

int CDPManager::DR_StopRecordAndUinitMemRecord(int nCh)
{
	int ret = 0;

	if (NULL == m_CDRecord)
	{
		DEBUG_MSG("无效的指针\n");
		return IDVR_ERROR_PARAMETER_INVALID_POINTER;
	}

	ret = m_CDRecord[nCh].StopRecordAndUinitMemRecord();

	return ret;
}

int CDPManager::DR_InputData(int nCh, time_t tFrameTime, unsigned long tFrameTimeInUs, const char *Buff, int nLen, int nFramType)
{
	int ret = 0;

	static int ErrorNum = 0;

	CDiskManager *theCDiskManager;

	if (NULL == m_CDRecord)
	{
		DEBUG_MSG("无效的指针\n");
		return IDVR_ERROR_PARAMETER_INVALID_POINTER;
	}

	ret = m_CDRecord[nCh].InputData(nCh, tFrameTime, tFrameTimeInUs, Buff, nLen, nFramType);

	if (0 != ret)
	{
		ErrorNum++;

		if (ErrorNum > 0)
		{
			theCDiskManager = CDiskManager::GetInstance();

			//告知上层，分区出错
			if (NULL != theCDiskManager)
			{
				//theCDiskManager->PushAlarm(NULL);
			}
		}
	}
	else
	{
		ErrorNum = 0;
	}

	return (int) ret;
}


//##ModelId=49BDAF450063
HANDLE CDPManager::DP_BackupCreate(int DiskGroupMask, time_t tStartTime, time_t tStopTime, HY_REPLAY_DATABACKUP_CALLBACK callBack)
{
	HANDLE hBackUp;

	if (DiskGroupMask < 0 || DiskGroupMask > 2)
	{
		DEBUG_MSG("无效的参数:%d\n", DiskGroupMask);
		return NULL;
	}

	DISKGROUPMASKTOTYPE(DiskGroupMask);

	hBackUp = Macro_DataBackupCreate((pkgstreamgrouptype) DiskGroupMask, CDiskGroup::handle[3 /*备份盘组 */ ]->dev, tStartTime, tStopTime, callBack);

	return hBackUp;
}

//##ModelId=49BDAF45013F
int CDPManager::DP_BackupAddCh(HANDLE handle, int nCh)
{
	int ret = IDVR_ERROR_PARAMETER_INVALID_POINTER;

	if (NULL == handle)
	{
		DEBUG_MSG("没有创建备份器\n");
		return ret;
	}

	return (int) ret;
}

int CDPManager::DP_BackupAddRecType(HANDLE handle, int nRecType)
{
	int ret = IDVR_ERROR_PARAMETER_INVALID_POINTER;

	if (NULL == handle)
	{
		DEBUG_MSG("没有创建备份器\n");
		return ret;
	}

	return (int) ret;
}

int CDPManager::DP_BackupStart(HANDLE handle)
{
	int ret = IDVR_ERROR_PARAMETER_INVALID_POINTER;

	if (NULL == handle)
	{
		DEBUG_MSG("没有创建备份器\n");
		return ret;
	}

	ret = Macro_DataBackupStart(handle);

	return (int) ret;
}

int CDPManager::DP_BackupGetSize(HANDLE handle, unsigned long long *nSize)
{
	int ret = IDVR_ERROR_PARAMETER_INVALID_POINTER;

	if (NULL == handle)
	{
		DEBUG_MSG("没有创建备份器\n");
		return -1;
	}

	ret = Macro_GetDataBackupSize(handle, nSize);

	return (int) ret;
}

int CDPManager::DP_BackupStop(HANDLE handle)
{
	int ret = IDVR_ERROR_PARAMETER_INVALID_POINTER;

	if (NULL == handle)
	{
		DEBUG_MSG("没有创建备份器\n");
		return ret;
	}

	ret = Macro_DataBackupStop(handle);

	if (0 != ret)
	{
		DEBUG_MSG("停止备份出错\n");
		return ret;
	}

	return (int) ret;
}

int CDPManager::DP_BackupDestroy(HANDLE handle)
{
	int ret = IDVR_ERROR_PARAMETER_INVALID_POINTER;

	if (NULL == handle)
	{
		DEBUG_MSG("没有创建备份器\n");
		return ret;
	}

	ret = Macro_DataBackupRelease(handle);

	return (int) ret;
}

//声明全局函数
extern int GetEncChNum();

extern int GetAlarmOutNum();

int CDPManager::Init()
{
	if (NULL != m_CDRecord)
	{
		return 0;
	}

#ifndef TEST_MEM_INFO
	theChildObj = new CPNodeTest;
#else
	theChildObj = CommonKit::TCreateObject<CPNodeTest>::CreateObject(__FILE__, __FUNCTION__, __LINE__);
#endif

	DEBUG_MSG(_DPM_VERSION__);

	if (NULL == theChildObj)
	{
		ERROR_MSG("申请内存失败\n");
		return -1;
	}

	m_EnChannel = GetEncChNum();
	if (NULL == m_CDRecord)
	{
//#ifndef TEST_MEM_INFO
		m_CDRecord = new CDRecord[m_EnChannel];
//#else
//		m_CDRecord = CommonKit::TCreateObject<CDRecord>::CreateObject(MAX_BIND_CH_NUM, __FILE__, __FUNCTION__, __LINE__);
//#endif
	}

	INIT_MUTEX(mutex);
	return (int) 0;
}

int CDPManager::UnInit()
{
	if (NULL != m_CDRecord)
	{
#ifndef TEST_MEM_INFO
		delete[]m_CDRecord;
#else
		CommonKit::TDestroyObject<CDRecord>::DestroyObject(m_CDRecord);
#endif
		m_CDRecord = NULL;
	}

	return (int) 0;
}

CDPManager *CDPManager::GetInstance()
{
	if (NULL == theCDPManager)
	{
#ifndef TEST_MEM_INFO
		theCDPManager = new CDPManager;
#else
		theCDPManager = CommonKit::TCreateObject<CDPManager>::CreateObject(__FILE__, __FUNCTION__, __LINE__);
#endif

		if (NULL == theCDPManager)
		{
			DEBUG_MSG("获取管理器实例失败\n");
			return NULL;
		}

		theCDPManager->Init();
	}
	return (CDPManager *) theCDPManager;
}

int CDPManager::ReleaseInstance()
{
	if (NULL != theCDPManager)
	{
#ifndef TEST_MEM_INFO
		delete theCDPManager;
#else
		CommonKit::TDestroyObject<CDPManager>::DestroyObject(theCDPManager);
#endif

		theCDPManager = NULL;
	}

	return (int) 0;
}

//!\brief 获取数据管理器的操作权
//!\param 无
//!\return  成功返回0，失败为-1
int CDPManager::Capture()
{
	LOCK(mutex);
	return 0;
}

//!\brief 释放数据管理器的操作权
//!\param 无
//!\return  成功返回0，失败为-1
int CDPManager::Release()
{
	UNLOCK(mutex);
	return 0;
}

CDPManager::CDPManager()
{
	m_CDRecord = NULL;
}

CDPManager::~CDPManager()
{
}
