
/*!
*\file   str_fs.h 
*\brief  描述使用的文件系统参数
*\author cwf
*\date	  2009/03/24
*\version 0.1
*/
#ifndef __STR_FS_H__
#define __STR_FS_H__

/**************************************************************************
*                            全局宏定义                                   *
 **************************************************************************/
#ifdef __PKGSTREAM__
#define __PKGSTREAM_STORTAGE__
#endif

#ifndef USE_SD_PKG
#define USE_SD_PKG
#endif

#ifndef __PKGSTREAM_STORTAGE__

//////////////////////////////////////////////////////////////////////////
//jfs文件系统库
//////////////////////////////////////////////////////////////////////////
#define __JFS_STORAGE__
#define __FS_VERSION__                          \
("***************STR:jfs 1.0.0 ****************\n	\
***************支持普通盘组*****************")

//支持的盘组个数
#define  MAX_DISK_GROUP_NUM                       1
#include "CRecordFile.h"

#else


//////////////////////////////////////////////////////////////////////////
//流式存储文件系统
//////////////////////////////////////////////////////////////////////////

#include "pkgstreamstorage.h"

#define __FS_VERSION__                           \
"************STR:pkgstream 1.0.0 ***\n	\
********支持普通、报警、冗余、备份盘组*******"

//支持的盘组个数
#define  MAX_DISK_GROUP_NUM                        4
#endif


/*!
 * \brief
 * 存储系统的初始化.
 * 
 * \param void
 * 
 * \return
 * 成功返回0，失败返回-1.
 */


#ifdef __PKGSTREAM_STORTAGE__

#define Macro_Init HY_PkgStream_Init

#elif defined(__JFS_STORAGE__)

#define Macro_Init  CRecordFile::Init

#endif

#ifdef _DEBUG

#endif

/*!
 * \brief
 * 存储系统重要错误信息回调函数注册.
 * 
 * \param cbSysErrCallback
 * 回调用函数.
 * 
 * \return
 * 成功返回0，失败返回-1.
 */
#ifdef __PKGSTREAM_STORTAGE__

#define Macro_SysErrorCallbackRegister HY_PkgStream_SysErrorCallbackRegister

#elif defined(__JFS_STORAGE__)

#define Macro_SysErrorCallbackRegister(x...)

#endif


/*!
 * \brief
 * 获取硬盘个数.
 *
 * \return
 * 获取含有的sata硬盘个数.
 */
#ifdef __PKGSTREAM_STORTAGE__

#define Macro_GetDiskCount HY_PkgStream_GetDiskCount

#elif defined(__JFS_STORAGE__)

#define Macro_GetDiskCount

#endif



/*!
 * \brief
 * 获取磁盘分组的个数.
 *
 * \return
 * 返回磁盘分组的个数.
 */
#ifdef __PKGSTREAM_STORTAGE__

#define Macro_GetDiskGroupCount  HY_PkgStream_GetDiskGroupCount

#elif defined(__JFS_STORAGE__)

#define Macro_GetDiskGroupCount()  1

#endif



/*!
 * \brief
 * 获取磁盘信息, 存入ptDiskInfo.
 *
 * \param iDiskIndex
 * 磁盘编号(取值范围在0——(硬盘个数-1)).
 *
 * \param ptDiskInfo
 * [out]磁盘信息结构体指针.
 *
 * \return
 * 成功返回0, 失败返回-1.
 */
#ifdef __PKGSTREAM_STORTAGE__

#define Macro_GetDiskInfo  HY_PkgStream_GetDiskInfo

#elif defined(__JFS_STORAGE__)

#define Macro_GetDiskInfo

#endif



/*
 * \brief
 * 对指定的硬盘用指定的分区信息进行分区.
 *
 * \param iDiskIndex
 * 硬盘编号(取值范围在0——(硬盘个数-1)).
 *
 * \param ptPartParamInfo
 * 硬盘分区参数信息结构体指针.
 *
 * \return
 * 成功返回0，失败返回-1.
*/
#ifdef __PKGSTREAM_STORTAGE__

#define Macro_DiskFdisk  HY_PkgStream_DiskFdisk

#elif defined(__JFS_STORAGE__)

#define Macro_DiskFdisk

#endif



/*!
 * \brief
 * 创建磁盘分组.
 *
 * \param pcDiskGroupName
 * 磁盘分组名
 *
 * \param ucCycleMode
 * 磁盘轮转的方式(停止或覆盖).
 *
 * \return
 * 磁盘分组句柄结构体信息指针.
*/
#ifdef __PKGSTREAM_STORTAGE__

#define Macro_DiskGroupCreate    HY_PkgStream_DiskGroupCreate

#elif defined(__JFS_STORAGE__)

#define Macro_DiskGroupCreate  CRecordFile::CreateInstance

#endif



/*!
 * \brief
 * 向指定硬盘分组中添加新硬盘分区.
 *
 * \param ptDiskGroup
 * 磁盘组句柄结构体指针.
 *
 * \param pcDevPart
 * 分区节点名.
 *
 * \return
 * 成功返回0, 失败返回-1.
*/
#ifdef __PKGSTREAM_STORTAGE__

#define Macro_DiskGroupPartAdd   HY_PkgStream_DiskGroupPartAdd

#elif defined(__JFS_STORAGE__)

#define Macro_DiskGroupPartAdd  CRecordFile::DiskGroupPartAdd

#endif



/*!
 * \brief
 * 删除指定分组上的指定分区.
 *
 * \param ptDiskGroup
 * 分组句柄.
 *
 * \param pcDevPart
 * 分区节点名.
 *
 * \return
 * 成功返回0, 失败返回-1.
*/
#ifdef __PKGSTREAM_STORTAGE__

#define Macro_DiskGroupPartDel   HY_PkgStream_DiskGroupPartDel

#elif defined(__JFS_STORAGE__)

#define Macro_DiskGroupPartDel  CRecordFile::DiskGroupPartDel

#endif



/*!
 * \brief
 * 获取指定的磁盘分区信息.
 *
 * \param iGroupIndex
 * 磁盘分组编号(从0开始).
 *
 * \param ptDiskGroupInfo
 * 磁盘分组信息.
 *
 * \return
 * 成功返回磁盘组句柄, 失败返回NULL.
*/
#ifdef __PKGSTREAM_STORTAGE__

#define Macro_GetDiskGroupInfo   HY_PkgStream_GetDiskGroupInfo

#elif defined(__JFS_STORAGE__)

#define Macro_GetDiskGroupInfo  CRecordFile::GetDiskGroupInfo

#endif



/*!
 * \brief
 * 删除指定的硬盘分组.
 *
 * \param ptDiskGroup
 * 硬盘分组句柄.
 *
 * \return
 * 成功返回0, 失败返回-1.
 */
#ifdef __PKGSTREAM_STORTAGE__
#define Macro_DiskGroupDestroy HY_PkgStream_DiskGroupDestroy
#elif defined(__JFS_STORAGE__)

#define Macro_DiskGroupDestroy(x...)

#endif



/*!
 * \brief
 * 打开指定磁盘分组类型的可查读属性.
 *
 * \param GroupType
 * 磁盘分组类型.
 *
  * \return
 * 成功返回0, 失败返回-1.
 */
#ifdef __PKGSTREAM_STORTAGE__
#define Macro_SetDiskGroupReadable HY_PkgStream_SetDiskGroupReadable
#elif defined(__JFS_STORAGE__)

#define Macro_SetDiskGroupReadable

#endif



/*!
 * \brief
 * 设置磁盘分组的满回调
 *
 * \param cbDiskGroupFullCallback
 * 回调函数句柄
 *
  * \return
 * 成功返回0, 失败返回-1.
 */
#ifdef __PKGSTREAM_STORTAGE__
#define Macro_SetDiskGroupFullCallback HY_PkgStream_SetDiskGroupFullCallback
#elif defined(__JFS_STORAGE__)

#define Macro_SetDiskGroupFullCallback  CRecordFile::SetDiskFullCallBack

#endif



/*!
 * \brief
 * 对指定磁盘分区格式化.
 *
 * \param iDiskIndex
 * 磁盘编号.
 *
 * \param iPartNo
 * 磁盘分区在磁盘中的编号.
 *
 * \return
 * 成功返回0, 失败返回-1.
 */
#ifdef __PKGSTREAM_STORTAGE__
#define Macro_DiskPartFormat HY_PkgStream_DiskPartFormat
#elif defined(__JFS_STORAGE__)

#define Macro_DiskPartFormat(x...)  -1

#endif



/*!
 * \brief
 * 获取最新数据位置.
 *
 * \param ptDiskGroup
 * 磁盘分区句柄结构体指针.
 *
 * \parm ptDataPos
 * 存放分组最新数据位置的结构体指针.
 *
 * \return
 * 成功返回0, 失败返回-1.
 */
#ifdef __PKGSTREAM_STORTAGE__
#define Macro_GetMostNewDataPos HY_PkgStream_GetMostNewDataPos
#elif defined(__JFS_STORAGE__)

#define Macro_GetMostNewDataPos

#endif


/*!
 * \brief
 * 获取最新数据位置.
 *
 * \param ptDiskGroup
 * 磁盘分区句柄结构体指针.
 *
 * \parm ptDataPos
 * 存放分组最新数据位置的结构体指针.
 *
 * \return
 * 成功返回0, 失败返回-1.
 */
#ifdef __PKGSTREAM_STORTAGE__
#define Macor_GetDiskGroupWrittingDiskInfo HY_PkgStream_GetDiskGroupWrittingDiskInfo
#elif defined(__JFS_STORAGE__)

#define Macor_GetDiskGroupWrittingDiskInfo

#endif

/*!
 * \brief
 * 获取分组上的坏块信息.
 *
 * \param hDiskGroup
 * 磁盘组句柄.
 *
 * \param ptBadBlockInfo
 * [out]存放磁盘分组信息的结构体指针.
 *
 * \return
 * 成功返回0, 失败返回-1.
 */
#ifdef __PKGSTREAM_STORTAGE__
#define Macro_GetBadBlockInfo  HY_PkgStream_GetBadBlockInfo
#elif defined(__JFS_STORAGE__)

#define Macro_GetBadBlockInfo

#endif



/*!
 * \brief
 * 释放磁盘分组坏块信息表
 *
 * \param ptBadBlockInfo
 * 存放磁盘分组信息的结构体指针.
 *
 * \return
 * 成功返回0, 失败返回-1.
 */
#ifdef __PKGSTREAM_STORTAGE__
#define Macro_ReleaseBlockInfo HY_PkgStream_ReleaseBlockInfo
#elif defined(__JFS_STORAGE__)

#define Macro_ReleaseBlockInfo

#endif



/*!
 * \brief 将数据写入录像通道中
 * \param iChannel 录像通道号
 * \param wFrameType 数据帧类型
 * \param bIndexable 该帧数据是否建立关键帧索引（通常AV数据的I帧和用户信息数据需要建立索引）
 * \param dwUserIndex 用户信息索引值，当数据帧类型为用户数据时，需要指定该帧用户数据的索引值
 * \param pFirstBuf 写入的第一块数据缓存指针
 * \param iFirstLength 写入的第一块数据长度
 * \param pSecondBuf 写入的第二块数据缓存指针
 * \param iSecondLength 写入的第二块数据长度
 * \return 成功返回0，失败返回小于0的错误码
 */
#ifdef __PKGSTREAM_STORTAGE__
#define Macro_DataInput HY_PkgStream_DataInput
#elif defined(__JFS_STORAGE__)

#define Macro_DataInput CRecordFile::DataInput

#endif



/*!
 * \brief 将指定录像通道的指定录像类型绑定到指定硬盘分组
 * \param iChannel 录像通道号
 * \param iRecType 录像类型
 * \param hDiskGroup 硬盘分组句柄
 * \return 成功返回0，失败返回小于0的错误码
 */
#ifdef __PKGSTREAM_STORTAGE__
#define Macro_DiskGroupBind HY_PkgStream_DiskGroupBind
#elif defined(__JFS_STORAGE__)

#define Macro_DiskGroupBind CRecordFile::DiskGroupBind

#endif



/*!
 * \brief 将指定录像通道的指定录像类型取消绑定到指定硬盘分组
 * \param iChannel 录像通道号
 * \param iRecType 录像类型
 * \param hDiskGroup 硬盘分组句柄
 * \return 成功返回0，失败返回小于0的错误码
 */
#ifdef __PKGSTREAM_STORTAGE__
#define Macro_DiskGroupUnBind HY_PkgStream_DiskGroupUnBind
#elif defined(__JFS_STORAGE__)

#define Macro_DiskGroupUnBind CRecordFile::DiskGroupUnBind

#endif



/*!
 * \brief 启动指定录像通道的指定录像类型
 * \param iChannel 录像通道号
 * \param iRecType 录像类型
 * \return 成功返回0，失败返回小于0的错误码
 */
#ifdef __PKGSTREAM_STORTAGE__
#define Macro_RecStart HY_PkgStream_RecStart
#elif defined(__JFS_STORAGE__)

#define Macro_RecStart CRecordFile::StartRecord

#endif



/*!
 * \brief 停止指定录像通道的录像
 * \param iChannel 录像通道号
 * \return 成功返回0，失败返回小于0的错误码
 */
#ifdef __PKGSTREAM_STORTAGE__
#define Macro_RecStop HY_PkgStream_RecStop
#elif defined(__JFS_STORAGE__)

#define Macro_RecStop  CRecordFile::StopRecord

#endif



/*!
 * \brief 获得指定通道在指定时间段内指定录像类型的录像片断
 * \param iChannel 录像通道号
 * \param tBeginTime 起始时间
 * \param tEndTime 结束时间
 * \param iRecType 录像类型
 * \param pRecSegCount 保存返回获取片断的个数
 * \return 成功返回0，失败返回小于0的错误码。
 */
#ifdef __PKGSTREAM_STORTAGE__
#define Macro_GetRecSegList  HY_PkgStream_GetRecSegList
#elif defined(__JFS_STORAGE__)

#define Macro_GetRecSegList  CRecordFile::GetRecSegList

#endif



/*!
 * \brief 释放录像片断信息列表所占内存
 * \return 成功返回0，失败返回小于0的错误码。
 */
#ifdef __PKGSTREAM_STORTAGE__
#define Macro_ReleaseRecSegList HY_PkgStream_ReleaseRecSegList
#elif defined(__JFS_STORAGE__)

#define Macro_ReleaseRecSegList  CRecordFile::ReleaseRecSegList

#endif



/*!
 * \brief 创建录像片段报告器
 * \param cbGetIndexCallback 回调函数，返回0时，分段报告器中止查询
 * \param pChContext 上下文
 * \return 成功返回录像片段报告器的句柄，失败返回NULL。
 */
#ifdef __PKGSTREAM_STORTAGE__
#define Macro_RecSegReporterCreate  HY_PkgStream_RecSegReporterCreate
#elif defined(__JFS_STORAGE__)

#define Macro_RecSegReporterCreate  CRecordFile::RecSegReporterCreate

#endif



/*!
 * \brief 释放录像片段报告器
 * \param hIndexReporter 录像片段报告器的句柄
 * \return 成功返回0，失败返回小于0的错误码。
 */
#ifdef __PKGSTREAM_STORTAGE__
#define Macro_RecSegReporterRelease HY_PkgStream_RecSegReporterRelease
#elif defined(__JFS_STORAGE__)

#define Macro_RecSegReporterRelease   CRecordFile::RecSegReporterRelease

#endif



/*!
 * \brief 添加获取录像片段的通道
 * \param hIndexReporter 录像片段报告器的句柄
 * \param iChannel 通道号
 * \param bDynReport 设定是否需要动态报告索引变化,默认不动态报告索引变化(用来动态更新录像片段信息)。
 * \return 成功返回0，失败返回小于0的错误码。
 */
#ifdef __PKGSTREAM_STORTAGE__
#define Macro_RecSegChanAdd HY_PkgStream_RecSegChanAdd
#elif defined(__JFS_STORAGE__)

#define Macro_RecSegChanAdd  CRecordFile::RecSegChanAdd

#endif



/*!
 * \brief 移除获取录像片段的通道
 * \param hIndexReporter 录像片段报告器的句柄
 * \param iChannel 通道号
 * \return 成功返回0，失败返回小于0的错误码。
 */
#ifdef __PKGSTREAM_STORTAGE__
#define Macro_RecSegChanDel HY_PkgStream_RecSegChanDel
#elif defined(__JFS_STORAGE__)

#define Macro_RecSegChanDel  CRecordFile::RecSegChanDel

#endif



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
#ifdef __PKGSTREAM_STORTAGE__
#define Macro_RecSegGet HY_PkgStream_RecSegGet
#elif defined(__JFS_STORAGE__)

#define Macro_RecSegGet   CRecordFile::RecSegGet

#endif



/*!
 * \brief 创建数据弹出器
 * \param nRefChan 参考通道(可以不设置，值为0xFFFFFFFF表示不设置)
 * \param tStartSeekTime 初始弹出数据的时间
 * \param tEndSeekTime 弹出数据的结束时间。当此时间为0时，数据进行动态弹出。如果结束时间是一个未来时间，则到达当前时间时，通过回调接口告之该事件。
 * \param cbReplayCallback 设定数据弹出的回调函数接口，弹出的数据按弹出器句柄、通道、帧类型、时间簇、事件类型
 *       （跳到新的时间片/查询结束/当前数据查询完毕等）等参数形式送回放线程。此回调与DSP上送编码数据接口相当
 * \return 成功返回创建的数据弹出器的句柄,失败返回NULL。
 */
#ifdef __PKGSTREAM_STORTAGE__
#define Macro_DataPopCreate HY_PkgStream_DataPopCreate
#elif defined(__JFS_STORAGE__)

#define Macro_DataPopCreate  CRecordFile::DataPopCreate

#endif



/*!
 * \brief 添加弹出数据的通道
 * \param hDataPopper 数据弹出器的句柄
 * \param nChannel 通道号
 * \return 成功返回0，失败返回小于0的错误码。
 */
#ifdef __PKGSTREAM_STORTAGE__
#define Macro_DataPopChanAdd HY_PkgStream_DataPopChanAdd
#elif defined(__JFS_STORAGE__)

#define Macro_DataPopChanAdd  CRecordFile::DataPopChanAdd

#endif



/*!
 * \brief 移除弹出数据的通道
 * \param hDataPopper 数据弹出器的句柄
 * \param nChannel 通道号
 * \return 成功返回0，失败返回小于0的错误码。
 */
#ifdef __PKGSTREAM_STORTAGE__
#define Macro_DataPopChanDel HY_PkgStream_DataPopChanDel
#elif defined(__JFS_STORAGE__)

#define Macro_DataPopChanDel  CRecordFile::DataPopChanDel

#endif


/*!
 * \brief 数据弹出时是否只弹出关键帧
 * \param hDataPopper 数据弹出器的句柄
 * \param bKeyFrame 是否只弹出关键帧，如果为TRUE则只弹出关键帧，如果为FALSE则弹出所有帧。
 * \return 成功返回0，失败返回小于0的错误码。
 */
#ifdef __PKGSTREAM_STORTAGE__
#define Macro_DataPopSetKeyFrame HY_PkgStream_DataPopSetKeyFrame
#elif defined(__JFS_STORAGE__)

#define Macro_DataPopSetKeyFrame  CRecordFile::DataPopSetKeyFrame

#endif



/*!
 * \brief 设置数据弹出方向
 * \param hDataPopper 数据弹出器的句柄
 * \param bKeyFrame 数据弹出方向，如果为TRUE则向后弹出，如果为FALSE则向前弹出
 * \return 成功返回0，失败返回小于0的错误码。
 */
#ifdef __PKGSTREAM_STORTAGE__
#define Macro_DataPopSetDirect HY_PkgStream_DataPopSetDirect
#elif defined(__JFS_STORAGE__)

#define Macro_DataPopSetDirect  CRecordFile::DataPopSetDirect

#endif

/*!
 * \brief 数据弹出器定位(设定了参考通道，按参考通道定位，未设参考通道，定位为时序上第一个满足条件的通道)
 * \param hDataPopper 数据弹出器的句柄
 * \param SeekTime 设置的弹出数据的时间
 * \return 成功返回0，失败返回小于0的错误码。
 */
#ifdef __PKGSTREAM_STORTAGE__
#define Macro_DataPopTimeSeek HY_PkgStream_DataPopTimeSeek
#elif defined(__JFS_STORAGE__)

#define Macro_DataPopTimeSeek   CRecordFile::DataPopTimeSeek

#endif



/*!
 * \brief 按用户信息索引值定位
 * \param hDataPopper 数据弹出器的句柄
 * \param cbSeekUserFilter 根据用户索引信息判别定位条件的回调函数，返回BOOL值，表示是否满足定位条件。
 * \return 成功返回0，失败返回小于0的错误码。
 */
#ifdef __PKGSTREAM_STORTAGE__
#define Macro_DataPopUserinfoSeek HY_PkgStream_DataPopUserinfoSeek
#elif defined(__JFS_STORAGE__)

#define Macro_DataPopUserinfoSeek(x...)  -1

#endif



/*!
 * \brief 开始数据弹出
 * \param hDataPopper 数据弹出器的句柄
 * \return 成功返回0，失败返回小于0的错误码。
 */
#ifdef __PKGSTREAM_STORTAGE__
#define Macro_DataPopStart HY_PkgStream_DataPopStart
#elif defined(__JFS_STORAGE__)

#define Macro_DataPopStart  CRecordFile::DataPopStart

#endif



/*!
 * \brief 停止数据弹出
 * \param hDataPopper 数据弹出器的句柄
 * \return 成功返回0，失败返回小于0的错误码。
 */
#ifdef __PKGSTREAM_STORTAGE__
#define Macro_DataPopStop HY_PkgStream_DataPopStop
#elif defined(__JFS_STORAGE__)

#define Macro_DataPopStop  CRecordFile::DataPopStop

#endif



/*!
 * \brief 释放数据弹出器
 * \param hDataPopper 数据弹出器的句柄
 * \return 成功返回0，失败返回小于0的错误码。
 */
#ifdef __PKGSTREAM_STORTAGE__
#define Macro_DataPopRelease HY_PkgStream_DataPopRelease
#elif defined(__JFS_STORAGE__)

#define Macro_DataPopRelease  CRecordFile::DataPopRelease

#endif



/*!
 * \brief
 * 创建关键帧信息查询器.
 *
 * \param iRefChan
 * 参考通道号.
 *
 * \param bDynPop
 * 判断是否动态弹出.
 *
 * \param tFirstSeekTime
 * 初始弹出数据的时间.
 *
 * \param cbKeyPkgFilter
 * 关键数据查询过滤回调.
 *
 * \return
 * 成功返回信息查询器句柄, 失败返回NULL.
 */
#ifdef __PKGSTREAM_STORTAGE__
#define Macro_KeyPkgQueryCreate HY_PkgStream_KeyPkgQueryCreate
#elif defined(__JFS_STORAGE__)

#define Macro_KeyPkgQueryCreate

#endif



/*!
 * \brief
 * 定位关键数据包.
 *
 * \param hKeyPkgQuery
 * 信息查询器句柄.
 *
 * \param pFrameType
 * 保存关键数据包类型的指针.
 *
 * \param pLength
 * 保存关键数据包大小的指针.
 *
 * \return
 * 成功返回0, 失败返回-1.
 */
#ifdef __PKGSTREAM_STORTAGE__
#define Macro_KeyPkgSeek HY_PkgStream_KeyPkgSeek
#elif defined(__JFS_STORAGE__)

#define Macro_KeyPkgSeek

#endif



/*!
 * \brief
 * 读取关键数据包.
 *
 * \param pFrameType
 * 保存关键数据包类型的指针.
 *
 * \param pLength
 * 保存关键数据包大小的指针.
 *
 * \return
 * 成功返回0, 失败返回-1.
 */
#ifdef __PKGSTREAM_STORTAGE__
#define Macro_KeyPkgGet HY_PkgStream_KeyPkgGet
#elif defined(__JFS_STORAGE__)

#define Macro_KeyPkgGet

#endif



/*!
 * \brief
 * 释放关键帧信息查询器.
 *
 * \param hKeyPkgQuery
 * 关键帧信息查询器句柄.
 *
 * \return
 * 成功返回0, 失败返回-1.
 */
#ifdef __PKGSTREAM_STORTAGE__
#define Macro_KeyPkgQueryRelease HY_PkgStream_KeyPkgQueryRelease
#elif defined(__JFS_STORAGE__)

#define Macro_KeyPkgQueryRelease

#endif



/*!
 * \brief
 * 删除磁盘分组上指定通道在指定时间段内的数据.
 *
 * \param GroupType
 * 磁盘分组类型.
 *
 * \param nChannel
 * 指定通道号.
 *
 * \param iRecType
 * 录像类型.
 *
 * \param tStartTime
 * 时间段的起始时间.
 *
 * \param tEndTime
 * 时间段的结束时间.
 *
 * \return
 * 成功返回0, 失败返回-1.
 */
#ifdef __PKGSTREAM_STORTAGE__
#define Macro_RecDataRemove HY_PkgStream_RecDataRemove
#elif defined(__JFS_STORAGE__)

#define Macro_RecDataRemove

#endif

/*!
* \brief
* 删除指定磁盘分组上在指定时间段内的录像数据.
*
* \param hDiskGroup
* 磁盘分组句柄.
*
* \param tStartTime
* 时间段的起始时间.
*
* \param tEndTime
* 时间段的结束时间.
*
* \return
* 成功返回0, 失败返回-1.
*/
#ifdef __PKGSTREAM_STORTAGE__
#define Macro_RecDataRemoveEx HY_PkgStream_RecDataRemoveEx
#elif defined(__JFS_STORAGE__)

#define Macro_RecDataRemoveEx  CRecordFile::DelData

#endif


/*!
 * \brief
 * 添加新硬盘.
 *
 * \param sDevName.
 * 硬盘结点名称.
 *
 * \return
 * 成功返回0, 失败返回错误码.
 */
#ifdef __PKGSTREAM_STORTAGE__
#define Macro_HotplugDiskAdd HY_PkgStream_HotplugDiskAdd
#elif defined(__JFS_STORAGE__)

#define Macro_HotplugDiskAdd  CRecordFile::AddDisk

#endif



/*!
 * \brief
 * 移除硬盘.
 *
 * \param sDevName.
 * 硬盘结点名称.
 *
 * \return
 * 成功返回0, 失败返回错误码.
 */
#ifdef __PKGSTREAM_STORTAGE__
#define Macro_HotplugDiskRemove HY_PkgStream_HotplugDiskRemove
#elif defined(__JFS_STORAGE__)

#define Macro_HotplugDiskRemove  CRecordFile::DelDisk

#endif

/*!
 * \brief 创建数据备份器
 * \paarm tStartSeekTime 备份数据的起始时间
 * \paarm tEndSeekTime 备份数据的结束时间
 * \param cbBackupCallback 回调接口，参数为已备份大小(单位：KB,全F表示结束)，当前绝对时间
 * \return 成功返回创建的数据备份器的句柄,失败返回NULL。
 */
#ifdef __PKGSTREAM_STORTAGE__
#define Macro_DataBackupCreate HY_PkgStream_DataBackupCreate
#elif defined(__JFS_STORAGE__)

#define Macro_DataBackupCreate(x...)  NULL

#endif



/*!
 * \brief 添加备份的录像类型
 * \param hDataBackup 数据备份器的句柄
 * \param nRecType 录像类型
 * \return 成功返回0，失败返回小于0的错误码。
 */
#ifdef __PKGSTREAM_STORTAGE__
#define Macro_DataBackupAddRecType HY_PkgStream_DataBackupAddRecType
#elif defined(__JFS_STORAGE__)

#define Macro_DataBackupAddRecType

#endif

/*!
 * \brief 添加备份数据的通道
 * \param hDataBackup 数据备份器的句柄
 * \param nChannel 通道号
 * \return 成功返回0，失败返回小于0的错误码。
 */
#ifdef __PKGSTREAM_STORTAGE__
#define Macro_DataBackupChanAdd HY_PkgStream_DataBackupChanAdd
#elif defined(__JFS_STORAGE__)

#define Macro_DataBackupChanAdd

#endif



/*!
 * \brief 获得待备份数据的总容量(单位：KB)
 * \param hDataBackup 数据备份器的句柄
 * \param pSize 总容量
 * \return 成功返回0，失败返回小于0的错误码。
 */
#ifdef __PKGSTREAM_STORTAGE__
#define Macro_GetDataBackupSize HY_PkgStream_GetDataBackupSize
#elif defined(__JFS_STORAGE__)

#define Macro_GetDataBackupSize(x...)  (-1)

#endif



/*!
 * \brief 开始数据备份
 * \param hDataBackup 数据备份器的句柄
 * \return 成功返回0，失败返回小于0的错误码。
 */
#ifdef __PKGSTREAM_STORTAGE__
#define Macro_DataBackupStart HY_PkgStream_DataBackupStart
#elif defined(__JFS_STORAGE__)

#define Macro_DataBackupStart(x...) -1

#endif



/*!
 * \brief 停止数据备份
 * \param hDataBackup 数据备份器的句柄
 * \return 成功返回0，失败返回小于0的错误码。
 */
#ifdef __PKGSTREAM_STORTAGE__
#define Macro_DataBackupStop HY_PkgStream_DataBackupStop
#elif defined(__JFS_STORAGE__)

#define Macro_DataBackupStop(x...) -1

#endif



/*!
 * \brief 释放数据弹出器
 * \param hDataBackup 数据备份器的句柄
 * \return 成功返回0，失败返回小于0的错误码。
 */
#ifdef __PKGSTREAM_STORTAGE__
#define Macro_DataBackupRelease HY_PkgStream_DataBackupRelease
#elif defined(__JFS_STORAGE__)

#define Macro_DataBackupRelease(x...) -1

#endif




/*!
 * \brief 添加弹出数据的录像类型
 * \param hDataPopper 数据弹出器的句柄
 * \param nRecType 录像类型
 * \return 成功返回0，失败返回小于0的错误码。
 */
#ifdef __PKGSTREAM_STORTAGE__
#define Macro_DataPopRecTypeAdd HY_PkgStream_DataPopRecTypeAdd
#elif defined(__JFS_STORAGE__)

#define Macro_DataPopRecTypeAdd  CRecordFile::DataPopRecTypeAdd

#endif



/*!
 * \brief 移除弹出数据的录像类型
 * \param hDataPopper 数据弹出器的句柄
 * \param nRecType 录像类型
 * \return 成功返回0，失败返回小于0的错误码。
 */
#ifdef __PKGSTREAM_STORTAGE__
#define Macro_DataPopRecTypeDel HY_PkgStream_DataPopRecTypeDel
#elif defined(__JFS_STORAGE__)

#define Macro_DataPopRecTypeDel  CRecordFile::DataPopRecTypeDel

#endif


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
#ifdef __PKGSTREAM_STORTAGE__
#define  Macro_DataPopGetOneKeyFrame HY_PkgStream_DataPopGetOneKeyFrame
#elif defined(__JFS_STORAGE__)

#define Macro_DataPopGetOneKeyFrame  CRecordFile::DataPopGetOneKeyFrame

#endif


/*!
 * \brief 获取指定分区所含有的录像数据的开始时间和结束时间
 * \param hDiskGroup 硬盘分组句柄
 * \param pcDevPart 分区节点号
 * \param tBeginTime 存放开始时间
 * \param tEndTime 存放结束时间
 * \return 成功返回0，失败返回小于0的错误码。
 */
#ifdef __PKGSTREAM_STORTAGE__
#define  Macro_GetDiskPartDataInfo HY_PkgStream_GetDiskPartDataInfo
#elif defined(__JFS_STORAGE__)

#define Macro_GetDiskPartDataInfo(x...)    -1

#endif

/*!
 * \brief 获取指定分区所含有的录像数据的开始时间和结束时间
 * \param pcDevPart 分区节点号
 * \param tBeginTime 存放开始时间
 * \param tEndTime 存放结束时间
 * \return 成功返回0，失败返回小于0的错误码。
 */
#ifdef __PKGSTREAM_STORTAGE__
#define  Macro_GetDiskPartDataInfoEx HY_PkgStream_GetDiskPartDataInfoEx
#elif defined(__JFS_STORAGE__)

#define Macro_GetDiskPartDataInfoEx(x...)  -1

#endif

/*!
 * \brief 获取指定分区所含有的录像数据的大小(MB为单位)
 * \param pcDevPart 分区节点号
 * \param DataSize 存放录像数据的大小
 * \return 成功返回0，失败返回小于0的错误码。
 */
#ifdef __PKGSTREAM_STORTAGE__
#define Macro_GetDiskPartDataSize HY_PkgStream_GetDiskPartDataSize
#elif defined(__JFS_STORAGE__)

#define Macro_GetDiskPartDataSize(x...)  -1

#endif


/*!
 * \brief 获取指定分组类型是否有指定时间段的录像数据
 * \param GroupType 指定的分组类型
 * \param tBeginTime 指定时间段的开始时间
 * \param tEndTime 指定时间段的结束时间
 * \return 有录像数据返回0，其他返回小于0的错误码。
 */
#ifdef __PKGSTREAM_STORTAGE__
#define Macro_JudgeIFHaveDataByTime HY_PkgStream_JudgeIFHaveDataByTime
#elif defined(__JFS_STORAGE__)

#define Macro_JudgeIFHaveDataByTime

#endif



/*!
 * \brief 获取弹出器备份数据的总容量(单位：KB)
 * \param hDataPopper 数据弹出器的句柄
 * \param pSize 总容量
 * \return 成功返回0，失败返回小于0的错误码。
 */
#ifdef __PKGSTREAM_STORTAGE__
#define Macro_DataPopGetDataSize HY_PkgStream_DataPopGetDataSize
#elif defined(__JFS_STORAGE__)

#define Macro_DataPopGetDataSize  CRecordFile::GetSize

#endif


 /*!
  * \brief 分区后更新分组信息.
  * \param sDevName.硬盘结点名称.
  * \return 无.
  */
#ifdef __PKGSTREAM_STORTAGE__
#define Macro_UpdateGroupInfo HY_PkgStream_UpdateGroupInfo
#elif defined(__JFS_STORAGE__)

#define Macro_UpdateGroupInfo

#endif


 /*!
  * \brief 分区后更新分组信息.
  * \param sDevName.硬盘结点名称.
  * \return 无.
  */
#ifdef __PKGSTREAM_STORTAGE__
#define Macro_SetDiskGroupCycleMode HY_PkgStream_SetDiskGroupCycleMode
#elif defined(__JFS_STORAGE__)

#define Macro_SetDiskGroupCycleMode  CRecordFile::SetDiskFull

#endif



/*!
 * \brief 获取指定盘组的剩余容量(单位：MB)
 * \param hDiskGroup 磁盘分组句柄
 * \param pSize 总容量
 * \return 成功返回0，失败返回小于0的错误码。
 */
#ifdef __PKGSTREAM_STORTAGE__
#define Macro_GetDiskGroupRemainSize HY_PkgStream_GetDiskGroupRemainSize
#elif defined(__JFS_STORAGE__)

#define Macro_GetDiskGroupRemainSize  CRecordFile::GetDiskGroupRemainSize

#endif

/*!
 * \brief 获取指定分组类型是否有指定天数(一般用于一个月)的录像数据
 * \param GroupType 指定的分组类型
 * \param tBeginTime 指定时间段的开始时间
 * \param numOfdays 从起始天开始的天数(24小时)
 * \param pResult 返回结果，采用位与方式bit0-bit30分别对应31天
 * \return 成功返回0，其他返回小于0的错误码。
 */
#ifdef __PKGSTREAM_STORTAGE__
#define Macro_JudgeIFHaveDataByDays HY_PkgStream_JudgeIFHaveDataByDays
#elif defined(__JFS_STORAGE__)

#define Macro_JudgeIFHaveDataByDays

#endif

#endif
