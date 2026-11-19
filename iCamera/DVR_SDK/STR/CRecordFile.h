// Copyright (C) 1991 - 1999 Rational Software Corporation

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CSNAPSHOT_49B0B4170037_INCLUDED
#define _INC_CSNAPSHOT_49B0B4170037_INCLUDED
#include <list>
#include <string>
#include "tagTime.h"
#include "enumRecType.h"
#include "pkgstreamstorage.h"
#include "str_fs.h"
#include "global_constant.h"




#ifdef __JFS__
class CFileManager;
using std::string;



//##ModelId=49A78C120167
typedef struct __tag_RecordFileInfo__
{
  public:
	//!<快照名称
	//##ModelId=49A78C120172
	char m_szFileName[256];

	//!<所属通道
	//##ModelId=49A7969A002D
	int m_nChannel;

	//!<抓拍时间
	//##ModelId=49A7969A002E
	int m_nTime;

	//!<数据时间
	//##ModelId=49A7969A002E
	int m_nDateTime;


	//!<数据类型
	//##ModelId=49A7969A002E
	int m_Type;

	//##ModelId=49A7969A002F
	bool m_bLocked;
	bool m_bMark;
} tag_RecordFileInfo;



//!\brief  快照
//##ModelId=49B0B4170037
class CRecordFile
{
  public:
	//!\brief 获取符合文件的个数
	//!\param hHandle : HANDLE 符合文件的个数
	//!\return  文件的个数
	//##ModelId=49B0B46A03BC
	int GetCount(HANDLE _this);

	//!\brief 获取指定快照信息
	//!\param int nIndex 文件在总个数中的索引。从0开始
	//!\param tag_RecordFileInfo* Info 快照信息
	//!\return  成功为0，失败为-1
	//##ModelId=49B0B4730148
	int GetInfo(HANDLE _this, int nIndex, tag_RecordFileInfo * Info);

	//!\brief 锁定文件
	//!\param int nIndex 文件在总个数中的索引。从0开始
	//!\return  成功为0，失败为-1
	//##ModelId=49B0B48902B2
	int Lock(HANDLE _this, int nIndex);

	//!\brief 解锁文件
	//!\param int nIndex 文件在总个数中的索引。从0开始
	//!\return  成功为0，失败为-1
	//##ModelId=49B0B49602A6
	int Unlock(HANDLE _this, int nIndex);


	//!\brief 设置文件保持时间
	//!\param const unsigned int nSeconds 单位秒
	//!\return  成功为0，失败为-1
	//##ModelId=49B0B49602A6
	int SetFileStrTime(const unsigned int nSeconds);


	static int Lock(char *FileName);


	//##ModelId=49B0B49602A6
	static int Unlock(char *FileName);


	//!\brief 创建一个快照查询
	//!\return  成功为0，失败为-1
	//##ModelId=49BEFE81035D    
	HANDLE CreateList(int nCh, time_t nBeginTime, time_t nEndTime, int RecType);

	//!\brief 释放一个查找句柄
	//!\return 无
	//##ModelId=49BEFE8103C1
	void ReleaseList(HANDLE _this);

	//!\brief 设置选择标记
	//!\param int nStartIndex 开始索引
	//!\param int nStopIndex  结束索引
	//!\param int bMark;TRUE为标记，FALSE取消标记
	//!\return 成功为0，失败为-1
	//##ModelId=49BEFE8103C1
	int SetMark(HANDLE _this, int nStartIndex, int nStopIndex, unsigned int bMark);

	//!\brief 设置选择标记
	//!\param int nIndex 文件在总个数中的索引。从0开始
	//!\param int bMark;TRUE为标记，FALSE取消标记
	//!\return 成功为0，失败为-1
	//##ModelId=49BEFE8103C1
	int SetMark(HANDLE _this, int nIndex, unsigned int bMark);


	//!\brief 获取选择标记总数
	//!\return 成功为标记的总数，失败为-1
	//##ModelId=49BEFE8103C1
	int GetMarkSum(HANDLE _this);

	//!\brief 获取是否被标记
	//!\return 标记范围ture，没有标记范围0，失败返回-1
	//##ModelId=49BEFE8103C1
	int IsMark(HANDLE _this, int Index);


	//!\brief   判断某个月份的每一天是否有图片文件
	//!\param   int nCh
	//!\param   int nYear
	//!\param   int nMon
	//!\param   int nDay
	//!\return  有文件为1，无文件为0 -1表示参数错误
	//##ModelId=49A646100324
	static int IsFileExist(int nCh, int nYear, int nMon, int nDay);
	static int IsFileExist(int DiskGroupMask, int nYear, int nMon);


	//!\brief 生成一个快照文件
	//!\param char* buff 快照的数据，将该buff直接写入文件。
	//!\param int time tagTime 时间
	//!\param int len    快照buff的长度
	//!\param int FileType  
	//!\param Buff   输出文件名
	//!\return  成功为0，失败为-1
	//##ModelId=49BEFE8103D5
	static int CapturePic(int nCh, time_t time, enumRecType FileType, char *Buff);

	//!\brief   添加一个挂载点
	//!\param   char *MountPoint 挂载点
	//!\return  成功为0，其它为错误码
	//##ModelId=49A646100324
	static int AddMountPoint(char *MountPoint);

	//!\brief   删除一个挂载点
	//!\param   char *MountPoint 挂载点
	//!\return  成功为0，其它为错误码
	//##ModelId=49A646100324
	static int DelMountPoint(char *MountPoint);

	//!\brief   处理硬盘异常
	//!\param   void *param 设备节点
	//!\return  成功为0，其它为错误码
	static int DVR_DiskException(void *param);

	//!\brief  磁盘满回调函数
	//!\param   int mode 磁盘满的方式
	//!\return  成功为0，其它为错误码
	static void DiskFull(int mode, char *p);

	//!\brief   添加一个磁盘
	//!\param   char *Dev 磁盘节点
	//!\return  成功为0，其它为错误码
	//##ModelId=49A646100324
	static int AddDisk(char *Dev);


	//!\brief 初始化文件管理器
	//!\return  成功为0，失败为-1
	//##ModelId=49B9FDEC0338
	static int SetDiskFull(void *_this, int Mode);


	//!\brief   删除一个磁盘
	//!\param   char *Dev 磁盘节点
	//!\return  成功为0，其它为错误码
	//##ModelId=49A646100324
	static int DelDisk(char *Dev);

	//!\brief 初始化文件管理器
	//!\return  成功为0，失败为-1
	//##ModelId=49B9FDEC0338
	static int Init(char *Flag);

	static CRecordFile *CreateInstance(char *pcDiskGroupName, int iCycleMode, PKGSTREAMGROUPTYPE GroupType);


	//!\brief 获取组内剩余容量的大小
	//!\return  成功为0，失败为-1
	static int GetDiskGroupRemainSize(void *_this, unsigned long *Size);
	static void *GetDiskGroupInfo(int nIndex, PSS_DISKGROUPINFO * Info);
	static int DiskGroupBind(int nCh, int RecType, void *_this);
	static int DiskGroupUnBind(int nCh, int RecType, void *_this);
	static int DiskGroupPartAdd(void *_this, const char *Dev);
	static int DiskGroupPartDel(void *_this, const int Dev);
	//!\brief 设置盘满回调
	//!\return  成功为0，失败为-1
	//##ModelId=49B9FDEC0338
	static int SetDiskFullCallBack(HY_DATAINPUT_DISKGROUPFULL_CALLBACK cbDiskGroupFullCallback);

	static int StartRecord(int nCh, int iRecType);
	static int StartRecord(int nCh, int iRecType, time_t time);
	static int StopRecord(int nCh);
	static int DataInput(int nCh, unsigned short wFrameType, BOOL bIndexable, DWORD dwUserIndex, void *pFirstBuf, int iFirstLength, void *pSecondBuf,
						 int iSecondLength, time_t tFrameTime, unsigned long tFrameTimeInUs);

	static int DelData(void *_this, time_t nBeginTime, time_t nEndTime);


	/*!
	 * \brief 创建录像片段报告器
	 * \param GroupType 硬盘分组类型
	 * \param cbGetIndexCallback 回调函数，返回0时，分段报告器中止查询
	 * \param pChContext 上下文
	 * \return 成功返回录像片段报告器的句柄，失败返回NULL。
	 */
	static HANDLE RecSegReporterCreate(PKGSTREAMGROUPTYPE DiskGroupMask, HY_REC_SEGMENT_CALLBACK cbGetIndexCallback, void *pChContext);

	/*!
	 * \brief 释放录像片段报告器
	 * \param hIndexReporter 录像片段报告器的句柄
	 * \return 成功返回0，失败返回小于0的错误码。
	 */
	static int RecSegReporterRelease(HANDLE hIndexReporter);
	/*!
	 * \brief 添加获取录像片段的通道
	 * \param hIndexReporter 录像片段报告器的句柄
	 * \param iChannel 通道号
	 * \param bDynReport 设定是否需要动态报告索引变化,默认不动态报告索引变化(用来动态更新录像片段信息)。
	 * \return 成功返回0，失败返回小于0的错误码。
	 */
	static int RecSegChanAdd(HANDLE hIndexReporter, int iChannel, int bDynReport);
	/*!
	 * \brief 添加获取录像片段的通道
	 * \param hIndexReporter 录像片段报告器的句柄
	 * \param iChannel 通道号
	 * \param bDynReport 设定是否需要动态报告索引变化,默认不动态报告索引变化(用来动态更新录像片段信息)。
	 * \return 成功返回0，失败返回小于0的错误码。
	 */
	static int RecSegChanDel(HANDLE hIndexReporter, int iChannel);

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
	static int RecSegGet(HANDLE hIndexReporter, time_t tBeginTime, time_t tEndTime);


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
	static int GetRecSegList(PKGSTREAMGROUPTYPE GroupType, int iChannel, time_t tBeginTime, time_t tEndTime, int iRecType, PSS_REGSEG ** pRecSegHead,
							 int *pRecSegCount);

	/*!
	 * \brief 释放录像片断信息列表所占内存
	 * \return 成功返回0，失败返回小于0的错误码。
	 */
	static int ReleaseRecSegList(PSS_REGSEG * pRecSegHead);



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
	static HANDLE DataPopCreate(PKGSTREAMGROUPTYPE GroupType, int nRefChan, time_t tStartSeekTime, time_t tEndSeekTime,
								HY_REPLAY_DATAPOP_CALLBACK cbReplayCallback, BOOL bDynPop, void *pContext);



	/*!
	 * \brief 添加弹出数据的通道
	 * \param hDataPopper 数据弹出器的句柄
	 * \param nChannel 通道号
	 * \return 成功返回0，失败返回小于0的错误码。
	 */
	static int DataPopChanAdd(HANDLE hDataPopper, int nChannel);



	/*!
	 * \brief 移除弹出数据的通道
	 * \param hDataPopper 数据弹出器的句柄
	 * \param nChannel 通道号
	 * \return 成功返回0，失败返回小于0的错误码。
	 */
	static int DataPopChanDel(HANDLE hDataPopper, int nChannel);



	/*!
	 * \brief 添加弹出数据的录像类型
	 * \param hDataPopper 数据弹出器的句柄
	 * \param nRecType 录像类型
	 * \return 成功返回0，失败返回小于0的错误码。
	 */
	static int DataPopRecTypeAdd(HANDLE hDataPopper, int nRecType);



	/*!
	 * \brief 移除弹出数据的录像类型
	 * \param hDataPopper 数据弹出器的句柄
	 * \param nRecType 录像类型
	 * \return 成功返回0，失败返回小于0的错误码。
	 */
	static int DataPopRecTypeDel(HANDLE hDataPopper, int nRecType);

	/*!
	 * \brief 数据弹出时是否只弹出关键帧
	 * \param hDataPopper 数据弹出器的句柄
	 * \param bKeyFrame 是否只弹出关键帧，如果为TRUE则只弹出关键帧，如果为FALSE则弹出所有帧。
	 * \param PopKeyInterval 当只弹出关键帧时，该参数有效，
	 *  表示通道弹出每隔多少关键帧弹出一个，此值限制在0-3，0表示全部弹出
	 * \return 成功返回0，失败返回小于0的错误码。
	 */
	static int DataPopSetKeyFrame(HANDLE hIndexReporter, BOOL bKeyFrame, unsigned long PopKeyInterval);

	static int GetSize(HANDLE hIndexReporter, unsigned long long *Size);

	/*!
	 * \brief 设置数据弹出方向
	 * \param hDataPopper 数据弹出器的句柄
	 * \param bKeyFrame 数据弹出方向，如果为TRUE则向后弹出，如果为FALSE则向前弹出
	 * \return 成功返回0，失败返回小于0的错误码。
	 */
	static int DataPopSetDirect(HANDLE hIndexReporter, BOOL bSequential);


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
	static int DataPopGetOneKeyFrame(HANDLE hIndexReporter, int iChannel, time_t tSeekTime, unsigned int precision, unsigned char *pbuf,
									 unsigned int *bufsize);


	/*!
	 * \brief 数据弹出器定位(设定了参考通道，按参考通道定位，未设参考通道，定位为时序上第一个满足条件的通道)
	 * \param hDataPopper 数据弹出器的句柄
	 * \param SeekTime 设置的弹出数据的时间
	 * \return 成功返回0，失败返回小于0的错误码。
	 */
	static int DataPopTimeSeek(HANDLE hDataPopper, time_t SeekTime);


	/*!
	 * \brief 开始数据弹出
	 * \param hDataPopper 数据弹出器的句柄
	 * \return 成功返回0，失败返回小于0的错误码。
	 */
	static int DataPopStart(HANDLE hDataPopper);



	/*!
	 * \brief 停止数据弹出
	 * \param hDataPopper 数据弹出器的句柄
	 * \return 成功返回0，失败返回小于0的错误码。
	 */
	static int DataPopStop(HANDLE hDataPopper);

	/*!
	 * \brief 释放数据弹出器
	 * \param hDataPopper 数据弹出器的句柄
	 * \return 成功返回0，失败返回小于0的错误码。
	 */
	static int DataPopRelease(HANDLE hDataPopper);
	static int GetIndexFileName(char *Name, char *IndexName);


  private:

	friend class CHardDisk;
	//构造函数
	CRecordFile(time_t tStartTime, time_t tStopTime, int nChannel);
	//##ModelId=49BEFE810316
	CRecordFile();

	//底层文件管理实例
	CFileManager *theCFileManager;


	std::list < string > m_lDev;

	static CRecordFile *m_CRecorFile[MAX_DISK_GROUP_NUM];
#ifdef __IO_AD__
	static FILE *m_File[__REAL__CH__];
	static FILE *m_IndexFile[__REAL__CH__];
#else
	static int m_File[__REAL__CH__];
	static int m_IndexFile[__REAL__CH__];
#endif
	static char *m_FileName[__REAL__CH__];

	static time_t m_FileStartTime[__REAL__CH__];
	static time_t m_FileWriteLen[__REAL__CH__];
	static int m_FileType[__REAL__CH__];

	//文件打包长度
	static int m_nFileLen;

	static HY_DATAINPUT_DISKGROUPFULL_CALLBACK m_CbDiskFull;

	int m_nFindIndex;
	//数据保持时间  
	unsigned int m_nSeconds;

};
#endif
#endif							/* _INC_CSNAPSHOT_49B0B4170037_INCLUDED */
