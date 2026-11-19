
/*!
*\file   CRecordFile.cpp 
*\brief  文件管理类
*\author cwf
*\date	  2009/03/24
*\version 0.1
*/
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <algorithm>
#include <iostream>
#include <stdlib.h>
#include "global_macro.h"
#include "CRecordFile.h"
#include "debug.h"
#include "STR/CDiskManager.h"
#include "STR/tagDiskInfo.h"
#include "tagTime.h"
#include "tagMotionSchedule.h"
#include "CRecordFile_DPM.h"
#include "global_gather.h"
extern "C"
{
#include "disk.h"
};

#include <unistd.h>
#ifdef __JFS__




/**************************************************************************
*                            常量                                        *
**************************************************************************/


/**************************************************************************
*                            宏                                          *
**************************************************************************/

#define LOCALTIME(StartTime,tm,year,nMonth,nDay,nHour,nMinute,nSecond)  	localtime_r(&StartTime, &tm);\
	year = tm.tm_year + 1900;\
	nMonth = tm.tm_mon + 1;\
	nDay = tm.tm_mday;\
	nHour = tm.tm_hour;\
	nMinute  = tm.tm_min;\
nSecond  = tm.tm_sec;

#define write_disk_error(x...)  \
{ \
	int len = file_size("/tmp/.disk_error.txt");\
	FILE *file;\
	if (len > 1024 * 1024)\
	{\
	file = fopen("/tmp/.disk_error.txt","wb");\
	}\
	else\
	{\
	file = fopen("/tmp/.disk_error.txt","ab+");\
	}\
	if ( NULL == file)\
	{\
	printf("\n[%s %d] open .disk_error.txt failed\n",__FILE__,__LINE__);\
	}\
	else\
	{\
	struct tm tm;time_t time1 = time(NULL);\
	int year,month,day,hour,mini,sec; \
	LOCALTIME(time1,tm,year,month,day,hour,mini,sec);\
	fprintf(file,"%d - %d - %d  %d -%d -%d\n",year,month,day,hour,mini,sec);\
	fprintf(file,"[%s %d]\n",__FUNCTION__,__LINE__);\
	fprintf(file,x);\
	fprintf(file,"\n");\
	fclose(file);\
	}\
}

/**************************************************************************
*                          数据类型                                       *
**************************************************************************/


/**************************************************************************
*                           全局变量                                      *
**************************************************************************/

//CFileManager *CRecordFile::theCFileManager = NULL;
HY_DATAINPUT_DISKGROUPFULL_CALLBACK CRecordFile::m_CbDiskFull = NULL;

#if (MAX_DISK_GROUP_NUM == 1)
CRecordFile *CRecordFile::m_CRecorFile[MAX_DISK_GROUP_NUM] = { NULL };
#endif

#ifndef __IO_AD__

//文件打包长度
int CRecordFile::m_nFileLen = 900;

#if (__REAL__CH__== 1)
int CRecordFile::m_File[__REAL__CH__] = { -1 };
#endif

#if (__REAL__CH__== 16)
int CRecordFile::m_File[__REAL__CH__] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };
#endif

#if (__REAL__CH__== 1)
int CRecordFile::m_IndexFile[__REAL__CH__] = { -1 };
#endif

#if (__REAL__CH__== 16)
int CRecordFile::m_IndexFile[__REAL__CH__] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };
#endif

#else

#if (__REAL__CH__== 16)
FILE *CRecordFile::m_File[__REAL__CH__] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };
#endif

#if (__REAL__CH__== 1)
FILE *CRecordFile::m_File[__REAL__CH__] = { NULL };
#endif

#if (__REAL__CH__== 16)
FILE *CRecordFile::m_IndexFile[__REAL__CH__] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };
#endif

#if (__REAL__CH__== 1)
FILE *CRecordFile::m_IndexFile[__REAL__CH__] = { NULL };
#endif

#endif							//__IO_AD__


#if (__REAL__CH__== 1)
time_t CRecordFile::m_FileStartTime[__REAL__CH__] = { 0 };
#endif

#if (__REAL__CH__== 16)
time_t CRecordFile::m_FileStartTime[__REAL__CH__] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
#endif

#if (__REAL__CH__== 1)
time_t CRecordFile::m_FileWriteLen[__REAL__CH__] = { 0 };
#endif

#if (__REAL__CH__== 16)
time_t CRecordFile::m_FileWriteLen[__REAL__CH__] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
#endif

#if (__REAL__CH__== 1)
char *CRecordFile::m_FileName[__REAL__CH__] = { NULL };
#endif

#if (__REAL__CH__== 16)
char *CRecordFile::m_FileName[__REAL__CH__] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };
#endif

#if (__REAL__CH__== 1)
int CRecordFile::m_FileType[__REAL__CH__] = { -1 };
#endif

#if (__REAL__CH__== 16)
int CRecordFile::m_FileType[__REAL__CH__] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };
#endif


class CDSegM_Record
{
  public:
	CRecordFile * m_RecordFile;

	//片段弹出器用的变量
	HY_REC_SEGMENT_CALLBACK m_cbGetIndexCallback;
	void *m_pChContext;
	unsigned long m_nChMask;
	static int m_iNum;

	CDSegM_Record()
	{
		m_RecordFile = NULL;
		m_nChMask = 0;
		m_pChContext = NULL;
		m_cbGetIndexCallback = NULL;
		m_iNum++;
	}

	~CDSegM_Record()
	{
		m_RecordFile = NULL;
		m_nChMask = 0;
		m_pChContext = NULL;
		m_cbGetIndexCallback = NULL;
		m_iNum--;
	}
};

int CDSegM_Record::m_iNum = 0;

/**************************************************************************
*                           方法实现                                      *
 **************************************************************************/

//##ModelId=49B0B46A03BC
int CRecordFile::GetCount(HANDLE _this)
{
	return 0;
}

//##ModelId=49B0B4730148
int CRecordFile::GetInfo(HANDLE _this, int nIndex, tag_RecordFileInfo * Info)
{
	int ret = -1;

	return (int) ret;
}

/*!
* \brief
* 写取文件的最后修改时间.
* 
* \param FileName
* 文件名.
* 
* \param TimeT
* 保存文件最后修改时间的结构体指针.
* 
* \returns
* 无.
*/
void Read_Modi_Time(char *FileName, time_t * TimeT);

/*!
* \brief
* 设置文件最后修改时间.
* 
* \param FileName
* 文件名.
* 
* \param TimeT
* 文件最后修改时间的结构体指.
* 
* \returns
* 无.
*/
void Set_Modi_Time(char *FileName, time_t TimeT);

int CRecordFile::Lock(char *FileName)
{
	int ret = -1;
	return ret;
}

//!\brief 设置文件保持时间
//!\param const unsigned int nSeconds 单位秒
//!\return  成功为0，失败为-1
//##ModelId=49B0B49602A6
int CRecordFile::SetFileStrTime(const unsigned int nSeconds)
{
	return 0;
}

//##ModelId=49B0B49602A6
int CRecordFile::Unlock(char *FileName)
{
	return -1;
}

//##ModelId=49B0B48902B2
int CRecordFile::Lock(HANDLE _this, int nIndex)
{
	return -1;
}

//!\brief 设置选择标记
//!\param int nStartIndex 开始索引
//!\param int nStopIndex  结束索引
//!\param int bMark;TRUE为标记，FALSE取消标记
//!\return 成功为0，失败为-1
//##ModelId=49BEFE8103C1
int CRecordFile::SetMark(HANDLE _this, int nStartIndex, int nStopIndex, unsigned int bMark)
{
	return -1;
}

//!\brief 设置选择标记
//!\param int nIndex 索引
//!\param int bMark;TRUE为标记，FALSE取消标记
//!\return 成功为0，失败为-1
//##ModelId=49BEFE8103C1
int CRecordFile::SetMark(HANDLE _this, int nIndex, unsigned int bMark)
{
	return -1;
}

//!\brief 获取选择标记总数
//!\return 成功为标记的总数，失败为-1
//##ModelId=49BEFE8103C1
int CRecordFile::GetMarkSum(HANDLE _this)
{
	return -1;
}


//!\brief 获取是否被标记
//!\return 标记范围ture，没有标记范围0，失败返回-1
//##ModelId=49BEFE8103C1
int CRecordFile::IsMark(HANDLE _this, int Index)
{
	return -1;
}

//##ModelId=49B0B49602A6
int CRecordFile::Unlock(HANDLE _this, int nIndex)
{
	return -1;
}

	//!\brief 获取组内剩余容量的大小
	//!\return  成功为0，失败为-1
int CRecordFile::GetDiskGroupRemainSize(void *_this, unsigned long *Size)
{
	*Size = 0;
	return 0;
}

void *CRecordFile::GetDiskGroupInfo(int nIndex, PSS_DISKGROUPINFO * Info)
{
	return NULL;
}

int CRecordFile::DiskGroupBind(int nCh, int RecType, void *_this)
{
	return 0;
}

int CRecordFile::DiskGroupUnBind(int nCh, int RecType, void *_this)
{
	return 0;
}


int CRecordFile::DiskGroupPartAdd(void *_this, const char *Dev)
{
	return 0;
}


int CRecordFile::DiskGroupPartDel(void *_this, const int Dev)
{
	return 0;
}

//##ModelId=49BEFE810316
CRecordFile::CRecordFile()
{
	// TODO: Add your specialized code here.
}

CRecordFile *CRecordFile::CreateInstance(char *pcDiskGroupName, int iCycleMode, PKGSTREAMGROUPTYPE GroupType)
{
	return NULL;
}

int CRecordFile::GetIndexFileName(char *Name, char *IndexName)
{
	return -1;
}


static const unsigned char H264_HEADER[19] =
	//{ 0x00, 0x00, 0x01, 0xC6, 0x4D, 0x50, 0x47, 0x34, 0x01, 0x00, 0x19, 0x80, 0x84, 0x1E, 0x00, 0xC0, 0x02, 0x40, 0x02 };//704 x 576 v
	{ 0x00, 0x00, 0x01, 0xB6, 0x48, 0x32, 0x36, 0x34, 0x13, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x80, 0x07, 0x38, 0x04 };//1920 x 1080 av

int CRecordFile::StartRecord(int nCh, int iRecType)
{
	return -1;
}

int CRecordFile::StartRecord(int nCh, int iRecType, time_t time)
{
	return -1;
}

int CRecordFile::StopRecord(int nCh)
{
	return 0;
}

int CRecordFile::DataInput(int nCh, unsigned short wFrameType, BOOL bIndexable, DWORD dwUserIndex, void *pFirstBuf, int iFirstLength,
						   void *pSecondBuf, int iSecondLength, time_t tFrameTime, unsigned long tFrameTimeInUs)
{
	return -1;
}


int CRecordFile::DelData(void *_this, time_t nBeginTime, time_t nEndTime)
{
	return 0;
}

//##ModelId=49BEFE81035D
HANDLE CRecordFile::CreateList(int nCh, time_t nBeginTime, time_t nEndTime, int RecType)
{
    return NULL;
}


//!\brief   判断某个月份的是否有图片文件
//!\param   int nCh
//!\param   int nYear
//!\param   int nMon
//!\param   int nDay
//!\return  有文件为1，无文件为0 -1表示参数错误
//##ModelId=49A646100324
#define SETBIT(y,x)	(y |= (1<<(x)))
int CRecordFile::IsFileExist(int DiskGroupMask, int nYear, int nMon, int nDay)
{
	return -1;
}

int CRecordFile::IsFileExist(int DiskGroupMask, int nYear, int nMon)
{
	return -1;
}


/*!
 * \brief 创建录像片段报告器
 * \param GroupType 硬盘分组类型
 * \param cbGetIndexCallback 回调函数，返回0时，分段报告器中止查询
 * \param pChContext 上下文
 * \return 成功返回录像片段报告器的句柄，失败返回NULL。
 */
HANDLE CRecordFile::RecSegReporterCreate(PKGSTREAMGROUPTYPE DiskGroupMask, HY_REC_SEGMENT_CALLBACK cbGetIndexCallback, void *pChContext)
{
    return NULL;
}


/*!
 * \brief 释放录像片段报告器
 * \param hIndexReporter 录像片段报告器的句柄
 * \return 成功返回0，失败返回小于0的错误码。
 */
int CRecordFile::RecSegReporterRelease(HANDLE hIndexReporter)
{
	return 0;
}

#define SETBIT(y,x)	(y |= (1<<(x)))
#define UNSETBIT(y,x)	(y &= ~(1<<(x)))
#define GETBIT(y,x)	((y) & (1<<(x)))

/*!
 * \brief 添加获取录像片段的通道
 * \param hIndexReporter 录像片段报告器的句柄
 * \param iChannel 通道号
 * \param bDynReport 设定是否需要动态报告索引变化,默认不动态报告索引变化(用来动态更新录像片段信息)。
 * \return 成功返回0，失败返回小于0的错误码。
 */
int CRecordFile::RecSegChanAdd(HANDLE hIndexReporter, int iChannel, int bDynReport)
{
    return -1;
}

/*!
 * \brief 添加获取录像片段的通道
 * \param hIndexReporter 录像片段报告器的句柄
 * \param iChannel 通道号
 * \param bDynReport 设定是否需要动态报告索引变化,默认不动态报告索引变化(用来动态更新录像片段信息)。
 * \return 成功返回0，失败返回小于0的错误码。
 */
int CRecordFile::RecSegChanDel(HANDLE hIndexReporter, int iChannel)
{
	return -1;
}

/*!
 * \brief 获得当前设定时段的所有录像时间片段，通过设定的HY_REC_SEGMENT_CALLBACK回调函数返回结果
 * \param hIndexReporter 录像片段报告器的句柄
 * \param tBeginTime 获取录像片段的开始时间
 * \param tEndTime 获取录像片段的结束时间
 * \return 成功返回0，失败返回小于0的错误码。
 * \说明：
         若设置了动态报告，则在HY_PkgStream_RecSegGet设定的BeginTime、EndTime之间有索引被覆盖或有新的索引诞生将通过HY_REC_SEGMENT_CALLBACK回调告诉上层程序。
         设定获取索引的时间区间，不用关闭hIndexReporter即可再次设定新的区间。HY_REC_SEGMENT_CALLBACK调用的线程环境：
         1、调用HY_PkgStream_RecSegGet后回调函数使用HY_PkgStream_RecSegGet的线程环境搜索片段，这意味着直到所有请求的索引片段回调完毕后HY_PkgStream_RecSegGet才返回
         2、自动报告时调用的回调函数线程环境为内部工作线程
 */
int CRecordFile::RecSegGet(HANDLE hIndexReporter, time_t tBeginTime, time_t tEndTime)
{
    return -1;
}

int CDPM_Record::m_iNum = 0;

/*!
 * \brief 创建数据弹出器
 * \param GroupType 硬盘分组类型
 * \param nRefChan 参考通道(可以不设置，值为0xFFFFFFFF表示不设置)
 * \param tStartSeekTime 初始弹出数据的时间
 * \param tEndSeekTime 弹出数据的结束时间。当此时间为0时，数据进行动态弹出。如果结束时间是一个未来时间，则到达当前时间时，通过回调接口告之该事件。
 * \param cbReplayCallback 设定数据弹出的回调函数接口，弹出的数据按弹出器句柄、通道、帧类型、时间簇、事件类型
 *       （跳到新的时间片/查询结束/当前数据查询完毕等）等参数形式送回放线程。此回调与DSP上送编码数据接口相当
 * \param bDynPop 是否动态定位弹出，即播放完毕后，是否主动重新定位继续弹出。
 *        当用于回放时，设置为TRUE，用于备份时，设置为FALSE
 * \param pContext 上下文，上层软件可以传入此参数，在弹出回调中会给出该参数
 * \return 成功返回创建的数据弹出器的句柄,失败返回NULL。
 */
HANDLE CRecordFile::DataPopCreate(PKGSTREAMGROUPTYPE GroupType, int nRefChan, time_t tStartSeekTime, time_t tEndSeekTime,
								  HY_REPLAY_DATAPOP_CALLBACK cbReplayCallback, BOOL bDynPop, void *pContext)
{
	return NULL;
}



/*!
 * \brief 添加弹出数据的通道
 * \param hDataPopper 数据弹出器的句柄
 * \param nChannel 通道号
 * \return 成功返回0，失败返回小于0的错误码。
 */
int CRecordFile::DataPopChanAdd(HANDLE hIndexReporter, int iChannel)
{
	return -1;
}

/*!
 * \brief 移除弹出数据的通道
 * \param hDataPopper 数据弹出器的句柄
 * \param nChannel 通道号
 * \return 成功返回0，失败返回小于0的错误码。
 */
int CRecordFile::DataPopChanDel(HANDLE hIndexReporter, int iChannel)
{
	return -1;
}

/*!
 * \brief 添加弹出数据的录像类型
 * \param hDataPopper 数据弹出器的句柄
 * \param nRecType 录像类型
 * \return 成功返回0，失败返回小于0的错误码。
 */
int CRecordFile::DataPopRecTypeAdd(HANDLE hIndexReporter, int nRecType)
{
	return -1;
}

/*!
 * \brief 移除弹出数据的录像类型
 * \param hDataPopper 数据弹出器的句柄
 * \param nRecType 录像类型
 * \return 成功返回0，失败返回小于0的错误码。
 */
int CRecordFile::DataPopRecTypeDel(HANDLE hIndexReporter, int nRecType)
{
	return -1;
}

/*!
 * \brief 数据弹出时是否只弹出关键帧
 * \param hDataPopper 数据弹出器的句柄
 * \param bKeyFrame 是否只弹出关键帧，如果为TRUE则只弹出关键帧，如果为FALSE则弹出所有帧。
 * \param PopKeyInterval 当只弹出关键帧时，该参数有效，
 *  表示通道弹出每隔多少关键帧弹出一个，此值限制在0-3，0表示全部弹出
 * \return 成功返回0，失败返回小于0的错误码。
 */
int CRecordFile::DataPopSetKeyFrame(HANDLE hIndexReporter, BOOL bKeyFrame, unsigned long PopKeyInterval)
{
	return -1;
}

/*!
 * \brief 数据弹出时是否只弹出关键帧
 * \param hDataPopper 数据弹出器的句柄
 * \param bKeyFrame 是否只弹出关键帧，如果为TRUE则只弹出关键帧，如果为FALSE则弹出所有帧。
 * \param PopKeyInterval 当只弹出关键帧时，该参数有效，
 *  表示通道弹出每隔多少关键帧弹出一个，此值限制在0-3，0表示全部弹出
 * \return 成功返回0，失败返回小于0的错误码。
 */
int CRecordFile::GetSize(HANDLE hIndexReporter, unsigned long long *pSize)
{
	return -1;
}


/*!
 * \brief 设置数据弹出方向
 * \param hDataPopper 数据弹出器的句柄
 * \param bKeyFrame 数据弹出方向，如果为TRUE则向后弹出，如果为FALSE则向前弹出
 * \return 成功返回0，失败返回小于0的错误码。
 */
int CRecordFile::DataPopSetDirect(HANDLE hIndexReporter, BOOL bSequential)
{
	return -1;
}



/*!
 * \brief 获取一帧关键帧
 * \param hDataPopper 数据弹出器的句柄
 * \param iChannel 通道号
 * \param tSeekTime 指定时间簇
 * \param precision 时间簇误差
 * \param pbuf 保存该关键帧的缓冲区指针
 * \param bufsize 输入时为缓冲区大小，输出时为关键帧大小
 * \return 成功返回0，失败返回小于0的错误码。
 */
int CRecordFile::DataPopGetOneKeyFrame(HANDLE hIndexReporter, int iChannel, time_t tSeekTime, unsigned int precision, unsigned char *pbuf,
									   unsigned int *bufsize)
{
	return -1;
}


/*!
 * \brief 数据弹出器定位(设定了参考通道，按参考通道定位，未设参考通道，定位为时序上第一个满足条件的通道)
 * \param hDataPopper 数据弹出器的句柄
 * \param SeekTime 设置的弹出数据的时间
 * \return 成功返回0，失败返回小于0的错误码。
 */
int CRecordFile::DataPopTimeSeek(HANDLE hIndexReporter, time_t SeekTime)
{
	return -1;
}


/*!
 * \brief 开始数据弹出
 * \param hDataPopper 数据弹出器的句柄
 * \return 成功返回0，失败返回小于0的错误码。
 */
int CRecordFile::DataPopStart(HANDLE hIndexReporter)
{
	return -1;
}

/*!
 * \brief 停止数据弹出
 * \param hDataPopper 数据弹出器的句柄
 * \return 成功返回0，失败返回小于0的错误码。
 */
int CRecordFile::DataPopStop(HANDLE hIndexReporter)
{
	return -1;
}

/*!
 * \brief 释放数据弹出器
 * \param hDataPopper 数据弹出器的句柄
 * \return 成功返回0，失败返回小于0的错误码。
 */
int CRecordFile::DataPopRelease(HANDLE hIndexReporter)
{
	return -1;
}

/*!
 * \brief 获得指定通道在指定时间段内指定录像类型的录像片断
 * \param GroupType 硬盘分组类型
 * \param iChannel 录像通道号
 * \param tBeginTime 起始时间
 * \param tEndTime 结束时间
 * \param iRecType 录像类型
 * \param pRecSegCount 保存返回获取片断的个数
 * \return 成功返回0，失败返回小于0的错误码。
 */
int CRecordFile::GetRecSegList(PKGSTREAMGROUPTYPE GroupType, int iChannel, time_t tBeginTime, time_t tEndTime, int iRecType,
							   PSS_REGSEG ** pRecSegHead, int *pRecSegCount)
{
	return -1;
}


/*!
 * \brief 释放录像片断信息列表所占内存
 * \return 成功返回0，失败返回小于0的错误码。
 */
int CRecordFile::ReleaseRecSegList(PSS_REGSEG * pRecSegHead)
{
	return -1;
}

//##ModelId=49BEFE8103C1
void CRecordFile::ReleaseList(HANDLE _this)
{
}

//构造函数
CRecordFile::CRecordFile(time_t tStartTime, time_t tStopTime, int nChannel)
{
}


//!\brief 生成一个快照文件
//!\param char* buff 快照的数据，将该buff直接写入文件。
//!\param int time tagTime 时间
//!\param int len    快照buff的长度
//!\param int FileType  
//!\param Buff   输出文件名
//!\return  成功为0，失败为-1
//##ModelId=49BEFE8103D5
int CRecordFile::CapturePic(int nCh, time_t time, enumRecType FileType, char *Buff)
{
    return -1;
}

struct TestComp
{
	TestComp(std::string id):m_id(id)
	{
	}

	bool operator() (const string & test)
	{
		return strcmp(test.c_str(), m_id.c_str()) == 0 ? true : false;
	}

	std::string m_id;
};


/*!
 * \brief
 * 写取文件的最后修改时间.
 * 
 * \param FileName
 * 文件名.
 * 
 * \param TimeT
 * 保存文件最后修改时间的结构体指针.
 * 
 * \returns
 * 无.
 */
void Read_Modi_Time(char *FileName, time_t * TimeT);

/*!
 * \brief
 * 设置文件最后修改时间.
 * 
 * \param FileName
 * 文件名.
 * 
 * \param TimeT
 * 文件最后修改时间的结构体指.
 * 
 * \returns
 * 无.
 */
void Set_Modi_Time(char *FileName, time_t TimeT);

void fsck(char *MountPoint)
{
	return;
}

//!\brief   添加一个挂载点
//!\param   char *MountPoint 挂载点
//!\return  成功为0，其它为错误码
//##ModelId=49A646100324
int CRecordFile::AddMountPoint(char *MountPoint)
{
	return -1;
}


//!\brief   删除一个挂载点
//!\param   char *MountPoint 挂载点
//!\return  成功为0，其它为错误码
//##ModelId=49A646100324
int CRecordFile::DelMountPoint(char *MountPoint)
{
	return -1;
}

//!\brief   处理硬盘异常
//!\param   void *param 设备节点
//!\return  成功为0，其它为错误码
int CRecordFile::DVR_DiskException(void *param)
{
	return -1;
}

extern "C" int ZWSys_GetTotalEncChannels();//MP4Sys_GetTotalEncChannels

class CChannelManager;

//!\brief  磁盘满回调函数
//!\param   int mode 磁盘满的方式
//!\return  成功为0，其它为错误码
void CRecordFile::DiskFull(int mode, char *p)
{
	return;
}

#ifdef __PKGSTREAM__
//获取SD卡设备所以
static int Get_Disk_DevIndex(const char *dev)
{
#ifdef __SD__
	return dev[11] - '0';		// "/dev/mmcblk0"
#else
	return dev[7] - 'a';		// "/dev/sda"
#endif
}
#endif

//获取SD卡分区索引


//!\brief   添加一个磁盘
//!\param   char *Dev 磁盘节点
//!\return  成功为0，其它为错误码
//##ModelId=49A646100324
int CRecordFile::AddDisk(char *Dev)
{
	return -1;
}


//!\brief   删除一个磁盘
//!\param   char *Dev 磁盘节点
//!\return  成功为0，其它为错误码
//##ModelId=49A646100324
int CRecordFile::DelDisk(char *Dev)
{
	return -1;
}

//!\brief 初始化文件管理器
//!\return  成功为0，失败为-1
//##ModelId=49B9FDEC0338
int CRecordFile::Init(char *Flag)
{
	return -1;
}


//!\brief 初始化文件管理器
//!\return  成功为0，失败为-1
//##ModelId=49B9FDEC0338
int CRecordFile::SetDiskFull(void *_this, int Mode)
{
	return -1;
}

//!\brief 设置盘满回调
//!\return  成功为0，失败为-1
//##ModelId=49B9FDEC0338
int CRecordFile::SetDiskFullCallBack(HY_DATAINPUT_DISKGROUPFULL_CALLBACK cbDiskGroupFullCallback)
{
	return -1;
}

#endif
