/*所有 (C)2012, zviewtech.com

* 文件名称： pkgsdfile.h
* 文件标识： 
* 内容摘要： 存储管理系统接口实现定义说明文件
* 其它说明： 
* 当前版本： 1.0.0
* 作    者： yong cao
* 完成日期： 
* 
* 修改记录1：// 修改历史记录，包括修改日期、修改者及修改内容
*    修改日期：
*    版 本 号：
*    修 改 人：
*    修改内容： 
* 修改记录2：…
**********************************************************************/
#ifndef PKGSDSDK_H
#define PKGSDSDK_H

#ifdef WIN32
#ifdef __cplusplus
#if defined( MAVS_EXPORTS)
#define PSS_API extern "C" __declspec(dllexport)
#else
#define PSS_API extern "C"
#endif
#else
#define PSS_API
#endif
#define API_CALL __stdcall
#else							/*Linux */
#ifdef __cplusplus
#define PSS_API extern "C"
#else
#define PSS_API
#endif
#define API_CALL
#endif

#include "time.h"

//#define USE_SD_PKG

typedef enum pkgsdErr
{
	SD_INIT_ERR = 0x01,			//初始化出错
	SD_DISK_ERR,				    //SD卡出错
	//往后添加
} PkgSdERR;



/* 主分区系统类型 
enum MainType
{
	Empty = 0x00,
	Fat12 = 0x01,
	Fat16 = 0x06,
	NTFS = 0x07,
	Fat32 = 0x0b,
	Linux = 0x83
};*/


#ifndef HANDLE
typedef void *HANDLE;
#endif

#ifndef BOOL
typedef int BOOL;
#endif

#ifndef DWORD
typedef unsigned long DWORD;
#endif

#define TRUE 		1
#define FALSE 		0





/*定义帧类型*/
#define PSS_FRAMETYPE_AV          0x8000	//音视频数据帧
#define PSS_FRAMETYPE_PICTURE     0x8001	//图片数据帧
#define PSS_FRAMETYPE_USER        0x8002	//用户自定义数据帧
#define PSS_FRAMETYPE_RECSTART    0x8003	//通道启动录像控制帧
#define PSS_FRAMETYPE_RECSTOP     0x8004	//通道停止录像控制帧
#define PSS_FRAMETYPE_POWERON     0x8005	//掉电控制帧



/*磁盘分组轮转类型*/
#define PSS_CYCLEMODE_STOP        0x0	//硬盘分组写满后停止
#define PSS_CYCLEMODE_COVER       0x1	//硬盘分组写满后覆盖

/*查询器数限制*/
//#define PSS_MAX_RECSEGREPORTER    2	//最多支持的录像片段查询器个数
 

/*弹出器数限制*/
//#define PSS_MAX_DATAPOPER           2	//最多支持的数据弹出器个数

/*字符个数最大定义*/
#define MAX_STRING_LEN            24	// 字符串最多字符个数



/*数据弹出器回调函数中事件类型*/
#define DATAPOP_EVENT_NONE      0x00	//无通知事件
#define DATAPOP_EVENT_TIME_JUMP 0x01	//跳到新的时间片
#define DATAPOP_EVENT_TIME_OVER 0x02	//指定的时间片查询回调结束
#define DATAPOP_EVENT_TIME_CURR 0x03	//当前数据查询完毕，即播放时间已经到达最新录像的数据时间
#define DATAPOP_EVENT_DISK_ERR  0x04	//异常结束事件，比如read或者seek硬盘失败，把该异常回调通知用户

/*数据备份器回调函数中事件类型*/
#define DATABACKUP_EVENT_NONE           0x00	//无通知事件
#define DATABACKUP_EVENT_DISK_READ_ERR  0x04	//异常结束事件，比如read或者seek硬盘失败，把该异常回调通知用户
#define DATABACKUP_EVENT_DISK_WRITE_ERR 0x05	//异常结束事件，比如write或者seek硬盘失败，把该异常回调通知用户



/* 磁盘分区信息相关的结构体 
typedef struct _DISK_PARTITION_INFO
{
	char acPartName[12];
	unsigned long dwStart;		// 分区的起始逻辑扇区
	unsigned long dwPartSize;	// 分区的总空间(扇区数)
} T_DISKPARTITIONINFO;*/

/* 磁盘信息相关的结构体 */
typedef struct _SD_DEVICE_INFO
{
	char acModelName[MAX_STRING_LEN];	// 磁盘的型号
	char acSerialNo[MAX_STRING_LEN];	// 磁盘的序列号
	unsigned long dwTotal_Disk_Space;	// 总的磁盘空间	
} T_SDINFO;



/*录像片断信息结构体*/
typedef struct _SD_PSS_REGSEG
{
	time_t tBeginTime;			//起始时间点
	time_t tEndTime;			//结束时间点
	int iRecType;				//该时间段的录像类型
	struct _PSS_REGSEG *ptNext;
} PSS_REGSEG_T, *pPPS_REGSEG_T;








/*系统错误码定义*/
#define ERR_NO                    0x00000000

#define ERR_HANDLE_ALLOC_ERROR    0xc0000100
#define ERR_DDRAW_CREATE_FAILED   0xc0000101
#define ERR_RUNTIME_ERROR         0xc0000102
#define ERR_INVALID_HANDLE_PSS	  0xc0000103
#define ERR_WAIT_TIMEOUT          0xc0000104
#define ERR_INVALID_ARGUMENT      0xc0000105
#define ERR_INVALID_FILENAME      0xc0000108
#define ERR_TMMAN_FAILURE         0xC0000109
#define ERR_OUTOF_MEMORY          0xc000010A
#define ERR_NOT_SUPPORT           0xc000010C
#define ERR_SYS_NOT_INIT          0xc000010D
#define ERR_CHANNEL_NOT_REC       0xc000010E
#define ERR_DISK_OPEN             0xc000010F
#define ERR_DISK_SEEK             0xc0000110
#define ERR_DISK_FULL             0xc0000111

#define ERR_GET_BUF               0xc0000112
#define ERR_REACH_MOST_NEW_POS    0xc0000113
#define USER_KEYPKG_GET_FINISH    0xc0000114	//用来表示用户关键帧查询结束
#define USER_KEYPKG_FIND_OK       0xc0000115	//用于在用户关键帧查询回调中控制查询结束
#define ERR_DISK_NOT_EXIST        0xc0000116	//硬盘分区可以OPEN但是无法访问
#define ERR_DISK_HEADAREA_NOT_USEABLE    0xc0000117	//硬盘分区的头部区域有坏扇区，无法使用
#define ERR_DISK_INDEXAREA_TOO_MUCH      0xc0000118	//硬盘分区的所有大块索引区都有坏块，无法使用

#define ERR_DISK_OPEN_FILE_FAIL 			0xC0000002
#define ERR_DISK_HAND_FULL 			0xC0000004
#define ERR_DISK_MALLOC_FAIL 			0xC0000005
#define ERR_DISK_WRITE_FAIL 			0xC000000A
#define ERR_DISK_READ_ONLY        0xC000000B	//

/* 报告出错类型，出错文件名，报错代码所在行 */
typedef int (API_CALL * ZW_PKG_SYSERR_CALLBACK) (PkgSdERR nPkgErr, char *sFileName, char *Function, int nLine);

/*!
 * \brief 存储库访问异常错误回调函数
 * \param dev 分区节点
 * \param errtype 异常错误类型
 * \return
 */
typedef int (*ZW_PKG_DISKERR_CALLBACK) (char *dev, int errtype);

/*!
 * \brief 录像片段回调函数
 * \param hRecSegReporter 录像片段报告器句柄
 * \param iChannel 该录像片段对应的通道
 * \param tBeginTime 该录像片段对应的起始时间
 * \param tEndTime 该录像片段对应的结束时间
 * \param RecType 该录像片段对应的录像类型
 * \param iChannel 该录像片段对应的通道
 * \return
 */
typedef int (*ZW_REC_SEGMENT_CALLBACK) (HANDLE hRecSegReporter, int iChannel, time_t tBeginTime, time_t tEndTime, unsigned short RecType
										);

/*!
 * \brief 数据弹出回调函数
 * \param hDataPopper 数据弹出器句柄
 * \param bIndexFrame 是否是建立索引的帧
 * \param wFrameType 数据对应的类型
 * \param tFrameTime 数据帧的绝对时间簇，精确度为秒
 * \param pBuf 数据对应的缓冲区地址
 * \param dwSize 数据对应的大小
 * \param EventID 发生的事件类型，如果是正常的数据弹出，EventID为DATAPOP_EVENT_NONE，其他事件时，帧相关的参数无效
 * \param pContext 创建弹出器时传入的上下文
 * \return
 */
typedef int (*ZW_REPLAY_DATAPOP_CALLBACK) (HANDLE hDataPopper, BOOL bIndexFrame, unsigned short wFrameType, time_t tFrameTime,
										   unsigned char *pBuf, unsigned long dwSize, unsigned long EventID, void *pContext);

/*!
 * \brief 用户索引信息判别定位条件的回调函数
 * \param iChannel 该用户数据对应的通道
 * \param UserIndex 该用户数据帧对应的索引值
 * \param FrameTime 该用户数据帧对应的时间
 * \return
 */
typedef int (*ZW_SEEK_USERINFOFILTER_CALLBACK) (HANDLE hDataPopper, int UserIndex, time_t FrameTime);

/*
 * \brief 磁盘满回调函数
 * \param hDisk 磁盘句柄
 * \return
 */
typedef int (*ZW_DATAINPUT_DISKGROUPFULL_CALLBACK) (HANDLE hDisk);



/*POP信息结构体*/
typedef struct _POP_SS
{
	time_t tSeekBeginTime;			//起始时间点
	time_t tSeekEndTime;			//结束时间点
	ZW_REPLAY_DATAPOP_CALLBACK popcallback;
	int handle;
	void *pContext;
	int *plink;
	int recmask;
	int bpopstop;
	int bkeyframe;
	 
} POP_SS, *pPOP_SS;


/*!
 * \brief
 * 存储系统的初始化.
 * 
 * \param void
 * 
 * \return
 * 成功返回0，失败返回-1.
 */
PSS_API int API_CALL ZW_PkgSD_Init(char *dev);



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
PSS_API int API_CALL ZW_PkgSd_SysErrorCallbackRegister(ZW_PKG_SYSERR_CALLBACK cbSysErrCallback);





/*!
 * \brief
 * 获取磁盘信息, 存入ptDiskInfo.
 *

 * \param ptDiskInfo
 * [out]磁盘信息结构体指针.
 *
 * \return
 * 成功返回0, 失败返回-1.
 */
PSS_API int API_CALL ZW_PkgSd_GetDiskInfo(T_SDINFO * ptDiskInfo);

PSS_API int API_CALL ZW_PkgSd_GetMountPointSpace(const char *pszMountPoint, int *pbMounted, unsigned long *pdwAvailableSize);


/*!
 * \brief
 * 获取磁盘写状态.
 *

 * \param writeStatus
 * [out]正在写返回1，空闲返回0
 *
 * \return
 * 成功返回0, 失败返回-1.
 */
PSS_API int API_CALL ZW_PkgSd_GetDiskWriteStatus(int *writeStatus);


/*!
 * \brief
 * 设置指定磁盘分组的轮转属性
 *
 * \param ptDisk
 * 磁盘句柄
 *
 * \param iCycleMode
 * 轮转属性值
 *
 * \return
 * 成功返回0, 失败返回-1.
 */
PSS_API int API_CALL ZW_PkgSd_SetDiskGroupCycleMode( int iCycleMode);







/*!
 * \brief
 * 对指定磁盘分区格式化.
 * \return
 * 成功返回0, 失败返回-1.
 */
PSS_API int API_CALL ZW_PkgSd_DiskPartFormat(const char *sDeviceSerial);




/*!
 * \brief 将数据写入SD卡
 *  * \param wFrameType 数据帧类型  *
 * \param pFirstBuf 写入的一帧数据
 * \param iFirstLength 写入的一帧数据长度
 * \return 成功返回0，失败返回小于0的错误码
 */
PSS_API int API_CALL ZW_PkgSd_DataInput( unsigned short wFrameType, void *pFirstBuf,
											int iFirstLength);



PSS_API int API_CALL ZW_PkgSd_IsFileExist(int iYear, int iMonth, int iDay, int *ifExist);


/*!
 * \brief 获得指定通道在指定时间段内指定录像类型的录像片断
 * \param tBeginTime 起始时间
 * \param tEndTime 结束时间
 * \param iRecType 录像类型
 * \param pRecSegCount 保存返回获取片断的个数
 * \return 成功返回0，失败返回小于0的错误码。
 */
PSS_API int API_CALL ZW_PkgSd_GetRecSegList(HANDLE hDataPopper,
                                                        time_t tBeginTime, time_t tEndTime, int iRecType,
                                                        PSS_REGSEG_T ** pRecSegHead, int *pRecSegCount);
            


/*!
 * \brief 释放录像片断信息列表所占内存
 * \return 成功返回0，失败返回小于0的错误码。
 */
PSS_API int API_CALL ZW_PkgSd_ReleaseRecSegList(int linkhandle,PSS_REGSEG_T * pRecSegHead);




PSS_API int API_CALL ZW_PkgSd_GetDataSize( time_t tBeginTime, time_t tEndTime, int iRecType,
												unsigned long long *size);



/*!
 * \brief 创建数据弹出器

 * \param tStartSeekTime 初始弹出数据的时间
 * \param tEndSeekTime 弹出数据的结束时间。当此时间为0时，数据进行动态弹出。如果结束时间是一个未来时间，则到达当前时间时，通过回调接口告之该事件。
 * \param cbReplayCallback 设定数据弹出的回调函数接口，弹出的数据按弹出器句柄、通道、帧类型、时间簇、事件类型
 *       （跳到新的时间片/查询结束/当前数据查询完毕等）等参数形式送回放线程。此回调与DSP上送编码数据接口相当
 * \param bDynPop 是否动态定位弹出，即播放完毕后，是否主动重新定位继续弹出。
 *        当用于回放时，设置为TRUE，用于备份时，设置为FALSE
 * \param pContext 上下文，上层软件可以传入此参数，在弹出回调中会给出该参数
 * \return 成功返回创建的数据弹出器的句柄,失败返回NULL。
 */
PSS_API HANDLE API_CALL  ZW_PkgSd_DataPopCreate(time_t tStartSeekTime, 
                                                            time_t tEndSeekTime, 
                                                            int recType,
                                                            ZW_REPLAY_DATAPOP_CALLBACK cbReplayCallback, 
                                                            void *pContext);






/*!
 * \brief 添加弹出数据的录像类型
 * \param hDataPopper 数据弹出器的句柄
 * \param nRecType 录像类型
 * \return 成功返回0，失败返回小于0的错误码。
 */
PSS_API int API_CALL ZW_PkgSd_DataPopRecTypeAdd(HANDLE hDataPopper, int nRecType);



/*!
 * \brief 移除弹出数据的录像类型
 * \param hDataPopper 数据弹出器的句柄
 * \param nRecType 录像类型
 * \return 成功返回0，失败返回小于0的错误码。
 */
PSS_API int API_CALL ZW_PkgSd_DataPopRecTypeDel(HANDLE hDataPopper, int nRecType);



/*!
 * \brief 数据弹出器定位(设定了参考通道，按参考通道定位，未设参考通道，定位为时序上第一个满足条件的通道)
 * \param hDataPopper 数据弹出器的句柄
 * \param SeekTime 设置的弹出数据的时间
 * \return 成功返回0，失败返回小于0的错误码。
 */
PSS_API int API_CALL ZW_PkgSd_DataPopTimeSeek(HANDLE hDataPopper, time_t SeekTime);





/*!
 * \brief 开始数据弹出
 * \param hDataPopper 数据弹出器的句柄
 * \return 成功返回0，失败返回小于0的错误码。
 */
PSS_API int API_CALL ZW_PkgSd_DataPopStart(HANDLE hDataPopper);

/*!
 * \brief 阻塞数据弹出
 * \param hDataPopper 数据弹出器的句柄
 * \param tSeekTime 新的开始时间
 * \return 成功返回0，失败返回小于0的错误码。
 */
PSS_API int API_CALL ZW_PkgSd_DataPopSeek(HANDLE hDataPopper,time_t tSeekTime);


/*!
 * \brief 停止数据弹出
 * \param hDataPopper 数据弹出器的句柄
 * \return 成功返回0，失败返回小于0的错误码。
 */
PSS_API int API_CALL ZW_PkgSd_DataPopStop(HANDLE hDataPopper);

/*!
 * \brief 阻塞数据弹出
 * \param hDataPopper 数据弹出器的句柄
 * \param pause 暂停给1，继续给0
 * \return 成功返回0，失败返回小于0的错误码。
 */
PSS_API int API_CALL ZW_PkgSd_DataPopPause(HANDLE hDataPopper,int pause);

/*!
 * \brief 释放数据弹出器
 * \param hDataPopper 数据弹出器的句柄
 * \return 成功返回0，失败返回小于0的错误码。
 */
PSS_API int API_CALL ZW_PkgSd_DataPopRelease(HANDLE hDataPopper);







/*!
 * \brief 数据弹出时是否只弹出关键帧
 * \param hDataPopper 数据弹出器的句柄
 * \param bKeyFrame 是否只弹出关键帧，如果为TRUE则只弹出关键帧，如果为FALSE则弹出所有帧。
 * \param PopKeyInterval 当只弹出关键帧时，该参数有效，
 *  表示通道弹出每隔多少关键帧弹出一个，此值限制在0-3，0表示全部弹出
 * \return 成功返回0，失败返回小于0的错误码。
 */
	PSS_API int API_CALL ZW_PkgSd_DataPopSetKeyFrame(HANDLE hDataPopper, BOOL bKeyFrame);





PSS_API int API_CALL ZW_PkgSd_stopWrite(int enable);


/*!
 * \brief 存储系统硬盘访问异常错误号
 * \param cbDiskErrCallback 回调用函数.
 * \return 成功返回0，失败返回-1.
 */
PSS_API int API_CALL ZW_PkgSd_DiskErrorGetErrorCode();



PSS_API int API_CALL ZW_Pkg_SetSegType(int  seg_rec_type,int f_create_new_seg);



#endif



