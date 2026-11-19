
/*!
*\file   CDPopM.cpp 
*\brief  数据弹出器
*\author cwf
*\date	  2009/03/24
*\version 0.1
*/
//#include <iostream.h>
#include "CDPopM.h"
#include "str_fs.h"
#include "debug.h"
#include "idvr_errno.h"
// #include "sys_banned.h"
#include "global_gather.h"

#ifdef __SUPPORT_SD_CARD__
/**************************************************************************
*                               方法实现                                   *
 **************************************************************************/
//##ModelId=49A6409F02A1



int CDPopM::CallBack(HANDLE hDataPopper, int iChannel, int bIndexFrame, unsigned short wFrameType, time_t FrameTime, unsigned char *pBuf,
					 unsigned long dwSize, unsigned long EventID, void *UserContext)
{
	CDPopM *Handle = (CDPopM *) UserContext;

	return Handle->m_CallBack(UserContext, iChannel, bIndexFrame, wFrameType, FrameTime, pBuf, dwSize, EventID);
}

int CDPopM::SetAttr(int DiskGroupMask, int nRefChan, time_t tStartSeekTime, time_t tEndSeekTime, DATAPOP_CALLBACK PopCallBack, int bDynPop)
{
	// TODO: Add your specialized code here.
	int ret = 0;

	int i;

	DEBUG_MSG("DiskGroupMask = %d\n", DiskGroupMask);
	if (NULL != dev)
	{
		Macro_DataPopStop(dev);
		Macro_DataPopRelease(dev);
		dev = NULL;
	}

	dev = Macro_DataPopCreate((PKGSTREAMGROUPTYPE) DiskGroupMask, nRefChan, tStartSeekTime, tEndSeekTime, CallBack, bDynPop, this);

	m_CallBack = PopCallBack;

	if (NULL == dev)
	{
		DEBUG_MSG("创建弹出器失败\n");
		ret = -1;
	}
	else
	{
		DEBUG_MSG("创建成功:dev = %p\n", dev);

		for (i = 0; i < MAX_ENC_CHANNEL_NUM; i++)
		{
			if (1 == m_nBindCh[i])
			{
				DEBUG_MSG("添加通道%d\n", i);
				ret = Macro_DataPopChanAdd(dev, i);
			}
		}

		for (i = 0; i < MAX_RECORD_TYPE; i++)
		{
			if (ERROR_RECORD_TYPE != m_RecordType[i])
			{
				DEBUG_MSG("添加录像类型%d\n", i);
				ret = Macro_DataPopRecTypeAdd(dev, m_RecordType[i]);
			}
		}

	}

	return (int) ret;
}

//##ModelId=49A640AD0002
int CDPopM::AddCh(int nChannel)
{
	// TODO: Add your specialized code here.
	int ret = 0;

	if (NULL == dev)
	{
		DEBUG_MSG("没有设置弹出器的属性\n");
		return -1;
	}

	if (MAX_ENC_CHANNEL_NUM <= nChannel)
	{
		return IDVR_ERROR_PARAMETER;
	}

	m_nBindCh[nChannel] = 1;

	DEBUG_MSG("添加通道%d\n", nChannel);
	ret = Macro_DataPopChanAdd(dev, nChannel);
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
int CDPopM::GetOneKeyFrame(HANDLE hHandle, int iChannel, time_t tSeekTime, unsigned int precision, unsigned char *pbuf, unsigned int *bufsize)
{
	// TODO: Add your specialized code here.
	int ret = 0;

	if (NULL == dev)
	{
		DEBUG_MSG("没有设置弹出器的属性\n");
		return -1;
	}

	ret = Macro_DataPopGetOneKeyFrame(dev, iChannel, tSeekTime, precision, pbuf, bufsize);
	WARN_MSG(0 == ret);
	return (int) ret;
}

//##ModelId=49A640D20114
int CDPopM::DelCh(int nChannel)
{
	// TODO: Add your specialized code here.
	int ret = 0;

	if (NULL == dev)
	{
		DEBUG_MSG("没有设置弹出器的属性\n");
		return -1;
	}

	if (MAX_ENC_CHANNEL_NUM <= nChannel)
	{
		return IDVR_ERROR_PARAMETER;
	}

	m_nBindCh[nChannel] = 0;

	ret = Macro_DataPopChanDel(dev, nChannel);
	return (int) ret;
}

//##ModelId=49AF92050125
int CDPopM::Seek(time_t SeekTime)
{
	// TODO: Add your specialized code here.
	int ret = 0;

	if (NULL == dev)
	{
		DEBUG_MSG("没有设置弹出器的属性\n");
		return -1;
	}

	ret = Macro_DataPopTimeSeek(dev, SeekTime);
	return (int) ret;
}

//##ModelId=49AF92050125
long long CDPopM::GetSize()
{
	// TODO: Add your specialized code here.
	long long ret = 0;

	unsigned long long nSize;

	if (NULL == dev)
	{
		DEBUG_MSG("没有设置弹出器的属性\n");
		return -1;
	}

	ret = Macro_DataPopGetDataSize(dev, &nSize);

	if (ret == 0)
	{
		ret = nSize;
	}
	else
	{
		DEBUG_MSG("ret = %d\n", ret);
		ret = 0;
	}

	return (long long) ret;
}

//##ModelId=49AF9243021F
int CDPopM::StartPop()
{
	// TODO: Add your specialized code here.
	int ret = 0;

	DEBUG_MSG("here\n");
	if (NULL == dev)
	{
		DEBUG_MSG("没有设置弹出器的属性\n");
		return -1;
	}

	ret = Macro_DataPopStart(dev);
	return (int) ret;
}


//!\brief 向弹出器添加一种录像类型
//!\param int RecType 录像类型（见录像模块定义）
//!\return  成功为0，失败为-1   
int CDPopM::DPM_AddRecType(int RecType)
{
	int ret = 0;

	int i = 0;

	if (NULL == dev)
	{
		DEBUG_MSG("没有设置弹出器的属性\n");
		return -1;
	}

	ret = Macro_DataPopRecTypeAdd(dev, RecType);

	for (i = 0; i < MAX_RECORD_TYPE; i++)
	{
		if (m_RecordType[i] == RecType)
		{
			break;
		}
	}

	if (i >= MAX_RECORD_TYPE)
	{
		//当前的录像类型没有添加到记录表内
		for (i = 0; i < MAX_RECORD_TYPE; i++)
		{
			if (m_RecordType[i] == ERROR_RECORD_TYPE)
			{
				m_RecordType[i] = RecType;
				break;
			}
		}
	}

	return (int) ret;
}


//!\brief 删除弹出器中一种录像类型
//!\param int RecType 录像类型（见录像模块定义）
//!\return  成功为0，失败为-1
int CDPopM::DPM_DelRecType(int RecType)
{
	int ret = 0;

	int i = 0;

	if (NULL == dev)
	{
		DEBUG_MSG("没有设置弹出器的属性\n");
		return -1;
	}

	ret = Macro_DataPopRecTypeDel(dev, RecType);

	for (i = 0; i < MAX_RECORD_TYPE; i++)
	{
		if (m_RecordType[i] == RecType)
		{
			m_RecordType[i] = ERROR_RECORD_TYPE;
			break;
		}
	}

	return (int) ret;

}

//##ModelId=49AF924802DA
int CDPopM::StopPop()
{
	// TODO: Add your specialized code here.
	int ret = 0;

	if (NULL == dev)
	{
		DEBUG_MSG("没有设置弹出器的属性\n");
		return -1;
	}

	ret = Macro_DataPopStop(dev);
	return (int) ret;
}

//##ModelId=49AF924802DA
int CDPopM::Release()
{
	// TODO: Add your specialized code here.
	int ret = 0;

	if (NULL == dev)
	{
		DEBUG_MSG("没有设置弹出器的属性\n");
		return -1;
	}

	ret = Macro_DataPopStop(dev);
	Macro_DataPopRelease(dev);
	dev = NULL;
	return (int) ret;
}

//##ModelId=49B8C34D0075
int CDPopM::SetPopDataType(int nDataType, unsigned int PopKeyInterval)
{
	// TODO: Add your specialized code here.
	int ret = 0;

	if (NULL == dev)
	{
		DEBUG_MSG("没有设置弹出器的属性\n");
		return -1;
	}
	ret = Macro_DataPopSetKeyFrame(dev, nDataType, PopKeyInterval);
	return (int) ret;
}

//##ModelId=49B8C4720211
int CDPopM::SetPopDataDirection(bool bFront)
{
	// TODO: Add your specialized code here.
	int ret = 0;

	if (NULL == dev)
	{
		DEBUG_MSG("没有设置弹出器的属性\n");
		return -1;
	}
	ret = Macro_DataPopSetDirect(dev, bFront);
	return (int) ret;
}

CDPopM::CDPopM()
{
	int i;

	dev = NULL;
	for (i = 0; i < MAX_ENC_CHANNEL_NUM; i++)
	{
		m_nBindCh[i] = 0;
	}

	for (i = 0; i < MAX_RECORD_TYPE; i++)
	{
		m_RecordType[i] = ERROR_RECORD_TYPE;
	}

	m_CallBack = NULL;
}

CDPopM::~CDPopM()
{
	Release();
}
#endif