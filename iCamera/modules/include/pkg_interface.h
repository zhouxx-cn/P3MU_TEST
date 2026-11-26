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

#define USE_SD_PKG

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
typedef int (*ZW_REPLAY_DATAPOP_CALLBACK) (HANDLE hDataPopper,
                                                    BOOL bIndexFrame,
                                                    unsigned short wFrameType,
                                                    time_t tFrameTime,
                                                    unsigned char *pBuf,
                                                    unsigned long dwSize,
                                                    unsigned long EventID,
                                                    void *pContext);

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
	int pop_method;
    char pop_file[256];
} POP_SS, *pPOP_SS;

typedef struct {
	int maxRingBufSize;
	int maxFrameBufSize;
}ZW_PKG_PARAM_T;

/*!
 * \brief
 * 存储系统的初始化.
 * 
 * \param void
 * 
 * \return
 * 成功返回0，失败返回-1.
 */
PSS_API int API_CALL ZW_PkgSD_Init(ZW_PKG_PARAM_T *s);

/**
 * SD卡模块功能暂停
 * 暂停后，所有功能不可用，并释放占用的内存
 */
PSS_API int API_CALL ZW_PkgSD_Pause();

/**
 * SD卡模块功能继续
 * 恢复后，重置暂停之前的操作，重新获取需要的内存
 */
PSS_API int API_CALL ZW_PkgSD_Resume();

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

/*
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

//!\brief 获取当前SD卡格式化状态
//!\return  格式化状态: 0未格式化 1正在格式化 2格式化成功 3格式化失败
PSS_API int API_CALL ZW_PkgSd_GetSdFormatStatus(void);

PSS_API int API_CALL ZW_PkgSd_GetDiskWriteStatus(int *writeStatus);

/*!
 * \brief 将数据写入SD卡
 * \param pFirstBuf 写入的一帧数据
 * \param iFirstLength 写入的一帧数据长度
 * \return 成功返回0，失败返回小于0的错误码
 */
PSS_API int API_CALL ZW_PkgSd_DataInput(const char *pFirstBuf, int iFirstLength);		

PSS_API int API_CALL ZW_PkgSd_UploadPictureToSdCard(const char *pic_name, unsigned int trig_type);

PSS_API int API_CALL ZW_PkgSd_StopRecordAndUinitMemRecord(void);

PSS_API void API_CALL ZW_PkgSd_UpdateInfo(void);

PSS_API int API_CALL ZW_PkgSd_IsFileExist(int iYear, int iMonth, int iDay, int *ifExist);

/*!
 * \brief 获得指定通道在指定时间段内指定录像类型的录像片断
 * \param tBeginTime 起始时间
 * \param tEndTime 结束时间
 * \param iRecType 录像类型
 * \param pRecSegCount 保存返回获取片断的个数
 * \return 成功返回0，失败返回小于0的错误码。
 */
PSS_API int API_CALL ZW_PkgSd_GetRecSegList(HANDLE hDataPopper, time_t tBeginTime, time_t tEndTime, int iRecType, PSS_REGSEG_T ** pRecSegHead, int *pRecSegCount);

/*!
 * \brief 释放录像片断信息列表所占内存
 * \return 成功返回0，失败返回小于0的错误码。
 */
PSS_API int API_CALL ZW_PkgSd_ReleaseRecSegList(int linkhandle,PSS_REGSEG_T * pRecSegHead);

PSS_API int API_CALL ZW_PkgSd_GetDataSize( time_t tBeginTime, time_t tEndTime, int iRecType, unsigned long long *size);

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

PSS_API int API_CALL ZW_Pkg_SetSegType(int seg_rec_type);

/* 开启录像 */
PSS_API int API_CALL ZW_Pkg_StartRecord(int record_type, int bH265);

/* 设置预录时间(单位: 秒) */
PSS_API int API_CALL ZW_Pkg_SetPrevTimeSec(unsigned int prev_time_sec);

/* 获取预录时间(单位: 秒) */
PSS_API unsigned int API_CALL ZW_Pkg_GetPrevTimeSec(void);

/* 设置录像延迟时间（单位：秒） */
PSS_API int API_CALL ZW_Pkg_SetRecordDelayTime (unsigned int nDelaySecs);

/* 获取录像延迟时间（单位：秒） */
PSS_API int API_CALL ZW_Pkg_GetRecordDelayTime (unsigned int *pnDelaySecs);

/*@breif ZW_PkgSd_SetCurrentAviNumPerPage 设置每页显示多少个AVI文件
 *@param [IN]  avi_num_per_page 每页显示的AVI文件个数
 *@return 成功返回0
 *@       失败返回-1
 *@note  每页显示的AVI文件个数不能超过当前有效的AVI个数g_read_curr_avi_num，
 *@      每页显示的AVI文件个数不能超过最大能显示的AVI个数MAX_AVI_NUM_ONE_PAGE
 */
PSS_API int API_CALL ZW_PkgSd_SetCurrentAviNumPerPage(int avi_num_per_page);

/*@breif ZW_PkgSd_GetCurrentAviTotalNum 获取当前AVI总个数
 *@return 成功返回当前总的AVI文件个数
 *@       失败返回-1
 */
PSS_API int API_CALL ZW_PkgSd_GetCurrentAviTotalNum(void);

/*@breif ZW_PkgSd_SetSwitchPageIsChoose 设置当前选中的是哪一个页面和当前录像路径类型
 *@param [IN]  choose_page 选中的页面(从1开始)
 *@param [IN]  path_type 当前录像路径类型 0: http路径  1:绝对路径
 *@return 成功返回0
 *@      失败返回-1
 */
PSS_API int API_CALL ZW_PkgSd_SetSwitchPageIsChoose(int choose_page, int path_type);

/*@breif ZW_PkgSd_GetCurrentAviListInfo 通过给定的时间戳获取对应的AVI文件列表
 *@param [IN]  read_avi_page 需要获取第几页的AVI信息(从1开始)
 *@param [OUT] p_avi_list 获取到的AVI文件列表
 *@param [OUT] p_avi_num_curr_page 当前页面有多少个AVI文件
 *@return 成功返回0
 *@      失败返回-1
 *@note  需要先调用SetCurrentAviNumPerPage设置当前用户需要设置每页显示多少个AVI文件
 */
PSS_API int API_CALL ZW_PkgSd_GetCurrentAviListInfo(int read_avi_page, void **p_avi_list, int *p_avi_num_curr_page);

/*@breif ZW_PkgSd_UpdateAviListByTime 通过给定的时间戳获取符合时间段的文件总数
 *@param [IN]  start_time 要搜索的起始时间
 *@param [IN]  stop_time 要搜索的结束时间(为0表示搜索全部文件)
 *@param [OUT] p_search_avi_count 符合当前时间段的文件总数
 *@return 成功返回0
 *@      失败返回-1
 */
PSS_API int API_CALL ZW_PkgSd_UpdateAviListByTime(unsigned int start_time, 
                                                           unsigned int stop_time, 
                                                           int * p_search_avi_count);



/*@breif ZW_PkgSd_SetCurrentPicNumPerPage 设置每页显示多少个图片文件
 *@param [IN]  pic_num_per_page 每页显示的图片文件个数
 *@return 成功返回0
 *@       失败返回-1
 *@note  每页显示的图片文件个数不能超过当前有效的图片个数g_read_curr_pic_num，
 *@      每页显示的图片文件个数不能超过最大能显示的图片个数MAX_PIC_NUM_ONE_PAGE
 */
PSS_API int API_CALL ZW_PkgSd_SetCurrentPicNumPerPage(int pic_num_per_page);

/*@breif ZW_PkgSd_GetCurrentPicTotalNum 获取当前图片总个数
 *@return 成功返回当前总的图片文件个数
 *@       失败返回-1
 */
PSS_API int API_CALL ZW_PkgSd_GetCurrentPicTotalNum(void);

/*@breif ZW_PkgSd_UpdatePicListByTime 通过给定的时间戳获取符合时间段的文件总数
 *@param [IN]  start_time 要搜索的起始时间
 *@param [IN]  stop_time 要搜索的结束时间(为0表示搜索全部文件)
 *@param [OUT] p_search_pic_count 符合当前时间段的文件总数
 *@return 成功返回0
 *@      失败返回-1
 */
PSS_API int API_CALL ZW_PkgSd_UpdatePicListByTime(unsigned int start_time,
                                                           unsigned int stop_time,
                                                           int *p_search_pic_count);

/*@breif ZW_PkgSd_GetCurrentPicListInfo 通过给定的时间戳获取对应的图片文件列表
 *@param [IN]  read_pic_page 需要获取第几页的图片信息(从1开始)
 *@param [OUT] p_pic_list 获取到的图片文件列表
 *@param [OUT] p_pic_num_curr_page 当前页面有多少个图片文件 
 *@return 成功返回0
 *@      失败返回-1
 *@note  需要先调用GetPicGlobalHeadInfoFromGlobalFile将需要的数据放到全局结构体里面
 *@      需要先调用SetCurrentPicNumPerPage设置当前用户需要设置每页显示多少个图片文件
 */
PSS_API int API_CALL ZW_PkgSd_GetCurrentPicListInfo(int read_pic_page, void **p_pic_list, int *p_pic_num_curr_page);

/* 获取磁盘资源占用情况(单位: MB) */
PSS_API int API_CALL ZW_PkgSd_GetDiskResourceSize(int *pRecordMB, int *pPictureMB);

/*@breif ZW_PkgSd_GetFileReadWriteStatus 获取当前文件的读写状态
 *@param [IN]  file_name 需要操作的文件名(绝对路径)
 *@param [OUT] read_write_status 
 *@            0:  当前文件空闲
 *@            1:  当前文件正在读
 *@            2:  当前文件正在写
 *@param [OUT] file_size 文件大小(单位: 字节) 
 *@return 返回0表示成功
 *@      返回-1表示失败
 */
PSS_API int API_CALL ZW_PkgSd_GetFileReadWriteStatus(const char *file_name,
                                                               unsigned int *read_write_status,
                                                               unsigned int *file_size);

/* 设置获取时间偏移回调 */
PSS_API int API_CALL ZW_PkgSd_SetGetTimeOffsetCallBack(int (*CallBack)(int *, int *, int *));

/*@breif ZW_PkgSd_GetDiskInfo 获取磁盘信息
 *@param [OUT] sutats        SD卡状态
 *@param [OUT] total_space;     SD(分区)总大小(单位: MB)
 *@param [OUT] available_space  SD(分区)可用空间(单位: MB)
 *@return 返回0表示成功
 *@      返回-1表示失败
 */
PSS_API int API_CALL ZW_PkgSd_GetDiskInfo(int *sutats, unsigned long *total_space, unsigned long *available_space);

/*@breif ZW_PkgSd_SDOPS_GetRecordDataSize 获取指定时间段的录像数据大小
 *@param [IN] start_time 开始时间
 *@param [IN] stop_time  结束时间
 *@param [IN] record_type 筛选的录像类型(当前无效)
 *@param [OUT] p_size 获取到的帧数据大小
 *@return 返回  0 表示获取成功
 *@       返回 -1 表示获取失败
 *@note   获取到的是数据帧大小，包括帧头
 */
PSS_API int API_CALL ZW_PkgSd_SDOPS_GetRecordDataSize(unsigned int start_time, 
                                                                       unsigned int stop_time, 
                                                                       int record_type, 
                                                                       unsigned int * p_size);

/* 获取当前回放的录像信息 */
PSS_API int ZW_PkgSd_SDOPS_GetRecordInfo(int *pCurrentRecordStartTime);

/**
 * @brief 关闭SD卡录像模块
 */
PSS_API int API_CALL ZW_PkgSd_UnInitSdCard(void);

/* 获取文件对应的文件信息 */
PSS_API int ZW_PkgSd_SDOPS_GetRecordFileInfoByFileName(const char* pFileName, void* pFileInfo);

#endif



