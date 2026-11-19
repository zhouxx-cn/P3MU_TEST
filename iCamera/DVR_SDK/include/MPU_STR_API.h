
/*!
 *\file		MPU_STR_API.h
 *\brief	MPU模块，存储管理相关接口定义
 *\author	林云
 *\date		2009/03/20

功能：
-	盘组：
	-#	提供磁盘个数的获取
	-#	提供磁盘的操作（格式化、分区、卸载、挂载）
	-#	提供磁盘分组的管理（建立、删除、更改、设置组的属性）
	-#	提供磁盘属性（容量、剩余容量、类型、状态、物理位置）的获取
	-#	盘组、数据的绑定，数据的查询、存储、读取等

-	音视频数据：
	-#	音视频数据的写入与查找（按时间段），信息的获取。
	-#	快照的文件名的生成与查找，信息的获取。
	-#	按照文件查找与读取。
	-#	录像文件提供完整的路径，有上层决定是否使用路径。流式存储无此项功能
	-#	提供硬盘错误信息的报警联动功能
 */

#ifndef _INC_MPU_STR_49A5F8B70324_INCLUDED
#define _INC_MPU_STR_49A5F8B70324_INCLUDED


#include <time.h>
#include "HANDLE.h"
#include "STR/tagDiskGroupInfo.h"
#include "STR/tagDiskInfo.h"
#include "STR/tagPartition.h"
#include "STR/tagRecSeg.h"
#include "STR/dp_callbackfun.h"
#include "SnapShot/SNAPSHOTINFO.h"
#include "CBAlert.h"			//报警回调函数
#include "CBDisk.h"				//报警回调函数


/**************************************************************************
 *                     盘组管理                                           *
 **************************************************************************/

//!\brief   获取工作模式
//!\return  成功返回工作模式，失败为-1
int MPU_STR_DiskGroup_GetWorkMode();

//!\brief   获取工作模式
//!\param   int nWorkMode 工作模式
//!\return  成功返回工作模式，失败为-1
int MPU_STR_DiskGroup_SetWorkMode(int nWorkMode);

//!\brief   提供操作盘组的接口函数
//!\brief   创建硬盘分组，主要是注册该新硬盘分组的基本信息。
//!\param   int nDiskGroupType  盘组类型。0为普通盘组，1为报警盘组，2为冗余盘组，3为备份盘组
//!\param   int nCycleMode 盘组满时是覆盖还是停止
//!\return  成功返回0，失败为-1
int MPU_STR_DiskGroup_Create(int nDiskGroupType, int nCycleMode);

//!\brief   销毁硬盘分组。
//!\param   int nDiskGroupType  盘组类型。0为普通盘组，1为报警盘组，2为冗余盘组，3为备份盘组
//!\return  成功返回0，失败为-1
int MPU_STR_DiskGroup_Destroy(int nDiskGroupType);

//!\brief   获取磁盘组信息
//!\param   int nDiskGroupType  盘组类型。0为普通盘组，1为报警盘组，2为冗余盘组，3为备份盘组
//!\param   tagDiskGroupInfo* Info 磁盘组信息结构体指针
//!\return  成功返回0，失败为-1
int MPU_STR_DiskGroup_GetInfo(int nDiskGroupType, tagDiskGroupInfo * Info);

//!\brief       获取指定分区所含有的录像数据的开始时间和结束时间
//!\param   pcDevPart 分区节点号
//!\param   tBeginTime 存放开始时间
//!\param   tEndTime 存放结束时间
//!\return  成功返回0，失败返回小于0的错误码。
int MPU_STR_DiskGroup_GetDiskPartDataInfoEx(char *pcDevPart, time_t * tBeginTime, time_t * tEndTime);

//!\brief   解除绑定某通道数据至该盘组
//!\param   int nDiskGroupType  盘组类型。0为普通盘组，1为报警盘组，2为冗余盘组，3为备份盘组
//!\param   int nCh 通道
//!\return  成功返回0，失败为-1
int MPU_STR_DiskGroup_UnBindDataCH(int nDiskGroupType, int nCh, int RecType);

//!\brief   绑定某通道数据至该盘组
//!\param   int nDiskGroupType  盘组类型。0为普通盘组，1为报警盘组，2为冗余盘组，3为备份盘组
//!\param   int nCh 通道
//!\param   int RecType录像类型
//!\return  成功返回0，失败为-1
int MPU_STR_DiskGroup_BindDataCH(int nDiskGroupType, int nCh, int RecType);

//!\brief   添加分区至该盘组
//!\param   int nDiskGroupType  盘组类型。0为普通盘组，1为报警盘组，2为冗余盘组，3为备份盘组
//!\param char *Partition 分区
//!\return  成功返回0，失败为-1
int MPU_STR_DiskGroup_AddPartition(int nDiskGroupType, const char *Partition);

//!\brief 删除分区至该盘组
//!\param int nDiskGroupType    盘组类型。0为普通盘组，1为报警盘组，2为冗余盘组，3为备份盘组
//!\param char *Partition 分区
//!\return  成功返回0，失败为-1
int MPU_STR_DiskGroup_DelPartition(int nDiskGroupType, const char *Partition);

//!\brief   获取指定硬盘组中块坏分布信息，含硬盘位置，分区位置。盘组属性及数据保持时间
//!\param   int nDiskGroupType  盘组类型。0为普通盘组，1为报警盘组，2为冗余盘组，3为备份盘组
//!\param PBADBLOCKINFO pBadBlockInfo   坏块链表表头指针。
//!\return  成功返回0，失败为-1
int MPU_STR_DiskGroup_GetBadBlockInfo(int nDiskGroupType, void *pBadBlockInfo);

//!\brief   设置数据的保持时间
//!\param   int nDiskGroupType  盘组类型。0为普通盘组，1为报警盘组，2为冗余盘组，3为备份盘组
//!\param   int nDataKeepTime 数据保持时间
//!\return  成功返回0，失败返回小于0的错误码。
int MPU_STR_DiskGroup_DataKeepTime(int nDiskGroupType, int nDataKeepTime);

//!\brief       设置盘组满回调函数
//!\param   int nType                   盘组满返回的类型
//!\param   CBAlert pCallback       回调函数句柄
//!\return  成功返回0，失败为-1
int MPU_STR_DiskGroup_SetFullCallBack(int nAlarmType, CBAlert pCallback);

//!\brief 设置盘组异常回调函数
//!\param int nAlarmType 盘组满返回的类型
//!\param CBAlert CallBack 回调函数句柄
//!\return  成功返回0，失败为-1
int MPU_STR_DiskGroup_SetErrorCallBack(int nAlarmType, CBAlert pCallback);

//!\brief 开启/禁止某个报警联动动作
//!\praram int nAlarmType 报警类型。VIDEO_LOST 
//!\praram int nActionType
//!      ACTION_SCREEN 屏幕显示
//!      ACTION_BEEP 声音提示
//!      ACTION_ALARMOUT 报警输出
//!      ACTION_SENDTOSERVER 上传报警服务器
//!      ACTION_EMAIL 联动Email
//!      ACTION_SMS 联动短信
//!\bool bEnahle true为开启，false为禁止
//!\return 成功为0，失败为错误码
int MPU_STR_DiskGroup_EnableAction(int nAlarmType, tagActionType nActionType, bool bEnahle);

//!\brief 判断某个报警联动动作是否开启
//!\praram int nAlarmType 报警类型。
//!\praram int nActionType
//!      ACTION_SCREEN 屏幕显示
//!      ACTION_BEEP 声音提示
//!      ACTION_ALARMOUT 报警输出
//!      ACTION_SENDTOSERVER 上传报警服务器
//!      ACTION_EMAIL 联动Email
//!      ACTION_SMS 联动短信
//!\return false 为禁止联动 true 为开启联动
bool MPU_STR_DiskGroup_IsEnableAction(int nAlarmType, tagActionType nActionType);

//!\brief 设置某个联动动作涉及的通道。无通道概念的不需要设置
//!\param int nCh 通道
//!\praram int nAlarmType 报警类型
//!\praram tagActionType nActionType   动作类型
//!\praram int nAlarmCh 联动动作涉及到通道。
//!\praram bool bEnable 启用/不启用该通道的配置
//!\praram int dwReserved 保留字。对于预置点、轨迹、巡航线路输入上述编号
//!\return  成功为0，失败为-1
int MPU_STR_DiskGroup_SetActionChParam(int nAlarmType, tagActionType nActionType, int nAlarmCh, bool bEnable, int nReserved = 0);

//!\brief 获取某个联动动作涉及的通道或者编号
//!\praram int AlarmType 报警类型
//!\praram tagActionType nActionType 动作类型
//!\praram bool *szCh 通道数组，true为联动了该通道，false为没有联动该通道
//!\praram int *szReserved 对应通道涉及到的子类型数据。目前仅相对于预置点编号等
//!\return  成功为0，失败为-1
int MPU_STR_DiskGroup_GetActionChParam(int nAlarmType, tagActionType nActionType, bool * szCh, int *szReserved);

//!\brief 获取指定盘组的剩余容量(单位：MB)
//!\param int nDiskGroupType 盘组类型。0为普通盘组，1为报警盘组，2为冗余盘组，3为备份盘组
//!\param hDiskGroup 磁盘分组句柄
//!\param pnSize 总容量
//!\return 成功返回0，失败返回小于0的错误码。
int MPU_STR_DiskGroup_GetRemainSize(const unsigned int nDiskGroupType, int *pnSize);


/**************************************************************************
 *                     磁盘管理                                           *
 **************************************************************************/

//!\brief   获取硬盘个数
//!\param   int *pnNum  返回磁盘个数
//!\return  成功返回0，其它为错误码
int MPU_STR_Disk_GetNum(int *pnNum);

//!\brief   获取磁盘物理位置编号
//!\param   int nIndex      硬盘索引号
//!\param   int *pBusNo     硬盘物理位置编号
//!\return  成功返回0，其它为错误码
int MPU_STR_Disk_GetBusNumber(const unsigned int nIndex, int *pBusNo);

//!\brief   判断硬盘是否为新硬盘
//!\param   int nIndex  硬盘索引号
//!\param   bool *pbNew 返回是否为新硬盘
//!\return  成功返回0，其它为错误码
int MPU_STR_Disk_IsNew(const unsigned int nIndex, bool * pbNew);

//!\brief   获取磁盘信息
//!\param   tagDiskInfo *Info 硬盘信息结构体指针    
//!\param   int nIndex 硬盘索引号
//!\return  成功返回0，其它为错误码
int MPU_STR_Disk_GetInfo(int nIndex, tagDiskInfo * Info);

//!\brief   获取对应磁盘对应分区的信息
//!\param   tagPartition *Info 硬盘信息结构体指针   
//!\param   int nPartitionIndex 分区在硬盘中的索引值
//!\param   int nDiskIndex 硬盘索引号
//!\return  成功返回0，其它为错误码
int MPU_STR_Disk_GetPartitionInfo(int nDiskIndex, int nPartitionIndex, tagPartition * Info);

//!\brief   格式化一个分区
//!\param   int nPartitionIndex 分区在硬盘中的索引值
//!\param   int nDiskIndex 硬盘索引号
//!\return  成功返回0，其它为错误码
int MPU_STR_Disk_FormatPartition(int nDiskIndex, int nPartitionIndex);

//!\brief   卸载该硬盘
//!\param   int nPartitionIndex 分区在硬盘中的索引值
//!\param   int nDiskIndex 硬盘索引号
//!\return  成功返回0，其它为错误码
int MPU_STR_Disk_UnMount(int nIndex);

//!\brief   创建一个分区
//!\param   int nDiskIndex 硬盘索引号
//!\param   unsigned long nSize 分区大小
//!\return  成功返回0，其它为错误码
int MPU_STR_Disk_CreatePartition(int nDiskIndex, unsigned long nSize);

//!\brief   创建一个分区
//!\param   int nDiskIndex 硬盘索引号
//!\param   int nPartitionIndex 分区在硬盘中的索引值
//!\return  成功返回0，其它为错误码
int MPU_STR_Disk_DestroyPartition(int nDiskIndex, int nIndexPartition);

//!\brief   设置硬盘异常通知回调函数
//!\param   int nAlarmType      硬盘错误类型
//!\param   CBAlert pCallback   回调指针
//!\return  成功返回0，其它为错误码
int MPU_STR_Disk_SetErrorNotifyCallback(int nAlarmType, CBAlert pCallback);

//!\brief       当系统插入一个无法识别（没有保留分区）的硬盘时，提示用户
//!\param   pCallBack   回调函数指针
//!\return  成功为0，失败为错误码
int MPU_STR_Disk_SetNewDiskCallBack(NEWDISKCALLBACK pCallback);

//!\brief       设置硬盘被卸载或者插入的回调函数
//!\param   pCallBack   回调指针
//!\return  成功为0，失败为错误码
int MPU_STR_Disk_SetDiskStatusCallBack(DISKSTATUSCALLBACK pCallBack);

//!\brief       开启/禁止某个报警联动动作
//!\param   int AlarmType   报警类型。
//!\param   int Action
//!      ACTION_SCREEN 屏幕显示
//!     ACTION_BEEP 声音提示
//!     ACTION_ALARMOUT 报警输出
//!     ACTION_SENDTOSERVER 上传报警服务器
//!     ACTION_EMAIL 联动Email
//!     ACTION_SMS 联动短信
//!\param   bool bEnahle    true为开启，false为禁止
//!\return  成功为0，失败为错误码
int MPU_STR_Disk_EnableAction(int nAlarmType, tagActionType Type, bool bEnahle);

//!\brief       判断某个报警联动动作是否开启
//!\param   int AlarmType   报警类型。
//!\param int Action
//!     ACTION_SCREEN                       屏幕显示
//!     ACTION_BEEP                         声音提示
//!     ACTION_ALARMOUT                 报警输出
//!     ACTION_SENDTOSERVER         上传报警服务器
//!     ACTION_EMAIL                            联动Email
//!     ACTION_SMS                          联动短信
//!\return  false 为禁止联动 true 为开启联动
bool MPU_STR_Disk_IsEnableAction(int nAlarmType, tagActionType Type);

//!\brief       设置某个联动动作涉及的通道。无通道概念的不需要设置
//!\param   int nCh                     通道
//!\param   int AlarmType               报警类型
//!\param   tagActionType Action    动作类型
//!\param   int nAlarmCh                联动动作涉及到通道。
//!\param   bool bEnable                启用/不启用该通道的配置
//!\param   int dwReserved          保留字。对于预置点、轨迹、巡航线路输入上述编号
//!\return   成功为0，失败为-1
int MPU_STR_Disk_SetActionChParam(int nAlarmType, tagActionType Action, int nAlarmCh, bool bEnable, int nReserved = 0);

//!\brief       获取某个联动动作涉及的通道或者编号
//!\param   int nAlarmType              报警类型
//!\param   tagActionType Action        动作类型
//!\param   bool *pbChEnable            通道数组，true为联动了该通道，false为没有联动该通道
//!\param   int *pnReserved         对应通道涉及到的子类型数据。目前仅相对于预置点编号等
//!\return  成功为0，失败为-1
int MPU_STR_Disk_GetActionChParam(int nAlarmType, tagActionType Action, bool * pbChEnable, int *pnReserved = NULL);

/*@brief MPU_STR_Disk_GetDiskStatus 获取磁盘状态
 *@return 见下表
 eZWDiskStatus_Idle = 0,
 eZWDiskStatus_Writting = 1,
 eZWDiskStatus_Reading,
 eZWDiskStatus_Exception,
 eZWDiskStatus_Error,
 eZWDiskStatus_Readonly,
 eZWDiskStatus_Formatting = 6,
 eZWDiskStatus_Warn,
 eZWDiskStatus_ReadWrite = 8,
 eZWDiskStatus_NoDisk = 9, // 无磁盘
 eZWDiskStatus_ExpNeedFormat = 10, // 磁盘异常需要格式化
 eZWDiskStatus_ExpNeedChangeDisk = 11, // 磁盘异常需要换卡
 eZWDiskStatus_FormatFailNeedReboot = 12 // 磁盘格式化失败需要重启

 */
int MPU_STR_Disk_GetDiskStatus(const unsigned int nIndex);

//!\brief 获取某硬盘当前的改动验证码
//!\param int nDiskIndex 硬盘索引号
//!\return 返回验证码
unsigned int MPU_STR_Disk_GetVerifyMark(int nDiskIndex);

//!\brief       初始化硬盘
//!\param   int nBusNo  硬盘物理位置编号
//!\return  成功为0，失败为-1
int MPU_STR_Disk_InitDisk(const unsigned int nBusNo);

/*@ brief 获取硬盘当前格式化的状态
 *@ param int nIndex 硬盘索引号
 *@ return  格式化状态:
 *@                  0: 未格式化;
 *@                  1: 格式化中
 *@                  2: 格式化成功
 *@                  3: 格式化失败
 *@                  4: 卸载失败
 *@                  5: 设备忙
 *@                  6: 无SD卡
 */
int MPU_STR_Disk_GetDiskFormatStatus(const unsigned int nIndex);

//!\brief 获取当前硬盘写的状态
//!\param int nIndex 硬盘索引号
//!\return  当前硬盘写状态: 0.SD卡空闲, 1.SD卡正在写, 2.SD卡写完成, 3.SD卡写失败
int MPU_STR_Disk_GetDiskWriteStatus(const unsigned int nIndex);

/**************************************************************************
 *                     录像信息查询                                       *
 **************************************************************************/

//!\brief   创建一个数据片段信息弹出器
//!\param   void
//!\return  成功为弹出器句柄，失败为NULL
void *MPU_STR_DSM_Create();

//!\brief   销毁一个数据片段信息弹出器
//!\param   void* hHandle       弹出器句柄
//!\return  成功为0，其它为错误码
int MPU_STR_DSM_Destroy(void *hHandle);

//!\brief   设置弹出器属性
//!\param   void* hHandle       弹出器句柄
//!\param   int nDiskGroupMask  0表示查询普通分组+报警分组、1表示冗余分组、2表示备份分组
//!\param   time_t tStartTime   开始时间
//!\param   time_t tStopTime    结束时间
//!\param   SEGMENT_CALLBACK pCallback  录像片段信息回调函数
//!\return  成功为0，其它为错误码
int MPU_STR_DSM_SetAttr(void *hHandle, int nDiskGroupMask, time_t tStartTime, time_t tEndTime, SEGMENT_CALLBACK pCallback);

//!\brief   添加一个通道
//!\param   void* hHandle       弹出器句柄
//!\param   int nChannel        通道号
//!\param   int bDynPop         新片段是否动态弹出
//!\return  成功为0，其它为错误码
int MPU_STR_DSM_AddCh(void *hHandle, int nChannel, int bDynPop);

//!\brief   删除一个通道
//!\param   void* hHandle       弹出器句柄
//!\param   int nChannel        通道号
//!\return  成功为0，其它为错误码
int MPU_STR_DSM_DelCh(void *hHandle, int nChannel);

//!\brief   开始弹出
//!\param   void* hHandle       弹出器句柄
//!\return  成功为0，其它为错误码
int MPU_STR_DSM_StartPop(void *hHandle);

//!\brief   结束弹出
//!\param   void* hHandle       弹出器句柄
//!\return  成功为0，其它为错误码
int MPU_STR_DSM_StopPop(void *hHandle);

//!\brief   获取录像片段信息
//!\param   int nDiskGroupMask      0表示查询普通分组+报警分组、1表示冗余分组、2表示备份分组
//!\param   int nChannel            通道
//!\param   time_t tStartTime       开始时间
//!\param   time_t tStopTime        结束时间    
//!\param   int nRecType            录像类型
//!\param   RECSEG **pRecSegHead    返回录像片段指针
//!\param   int *pnCount            返回录像片段的个数
//!\return  成功为0，其它为错误码
int MPU_STR_DSM_GetRecSegList(int nDiskGroupMask, int nChannel, time_t tStartTime, time_t tStopTime, int nRecType, tagRecSeg ** pRecSegHead,
							  int *pnCount);

//!\brief   释放录像片段信息
//!\param   tagRecSeg *pRecSegHead      链表指针
//!\return  成功为0，其它为错误码
int MPU_STR_DSM_ReleaseRecSegList(tagRecSeg * pRecSegHead);


/**************************************************************************
 *                     录像备份管理                                       *
 **************************************************************************/

//!\brief        判断某天是否有录像数据
//!\param   int nDiskGroupMask 0表示查询普通分组+报警分组、1表示冗余分组、2表示备份分组
//!\param   int nYear
//!\param   int nMon
//!\param   int nDay
//!\return  有数据返回true,无数据返回false
bool MPU_STR_DP_IsFileExist(int nDiskGroupMask, int nYear, int nMon, int nDay);

//!\brief   判断某月是否有录像数据
//!\param   int DiskGroupMask 0表示查询普通分组+报警分组、1表示冗余分组、2表示备份分组
//!\param   int nYear
//!\param   int nMon
//!\return  0-30分别代表1日制31日
int MPU_STR_DP_IsFileExist(int nDiskGroupMask, int nYear, int nMon);

/**************************************************************************
 *                     快照管理                                           *
 **************************************************************************/

//!\brief   创建一个快照查询
//!\param   time_t tStartTime   开始时间
//!\param   time_t tStopTime    结束时间
//!\param   int nChannelMask    需要查询的通道mask
//!\return  返回一个快照查询句柄。NULL表示创建失败，其他表示创建成功
HANDLE MPU_STR_SnapShotM_CreateList(time_t tStartTime, time_t tStopTime, int nChannelMask);

//!\brief   获取符合文件的个数
//!\param   hHandle : HANDLE 符合文件的个数
//!\return  文件的个数
int MPU_STR_SnapShotM_GetCount(HANDLE hHandle);

//!\brief   获取指定快照信息
//!\param   hHandle : HANDLE 查询的句柄
//!\param   int nIndex 文件在总个数中的索引。从0开始
//!\param   SNAPSHOTINFO* Info 快照信息
//!\return  成功为0，其它为错误码
int MPU_STR_SnapShotM_GetInfo(HANDLE hHandle, int nIndex, SNAPSHOTINFO * Info);

//!\brief   锁定文件
//!\param   hHandle : HANDLE 查询的句柄
//!\param   int nIndex 文件在总个数中的索引。从0开始
//!\return  成功为0，其它为错误码
int MPU_STR_SnapShotM_Lock(HANDLE hHandle, int nIndex);

//!\brief   解锁文件
//!\param   hHandle : HANDLE 查询的句柄
//!\param   int nIndex 文件在总个数中的索引。从0开始
//!\return  成功为0，其它为错误码
int MPU_STR_SnapShotM_Unlock(HANDLE hHandle, int nIndex);


//!\brief   锁定文件
//!\param   char *FileName : 文件全路径
//!\return  成功为0，其它为错误码
int MPU_STR_SnapShotM_Lock(char *FileName);

//!\brief   解锁文件
//!\param   char *FileName : 文件全路径
//!\return  成功为0，其它为错误码
int MPU_STR_SnapShotM_Unlock(char *FileName);

//!\brief   释放一个查找句柄
//!\param   hHandle : HANDLE 查询的句柄
//!\return 无
void MPU_STR_SnapShotM_ReleaseList(HANDLE hHandle);

//!\brief   生成一个快照文件
//!\param   char* pBuff     快照的数据，将该buffer直接写入文件。
//!\param   int nChannel    通道
//!\param   int tCreateTime 时间
//!\param   int nLen        快照buffer的长度
//!\param   enumFileType eFileType  
//!\return  成功为0，其它为错误码
int MPU_STR_SnapShotM_CapturePic(int nChannel, time_t tCreateTime, char *pBuff, int nLen, enumFileType eFileType = BMP);

//!\brief 设置选择标记
//!\param int nStartIndex 开始索引
//!\param int nStopIndex  结束索引
//!\param int bMark;TRUE为标记，FALSE取消标记
//!\return 成功为0，失败为-1
int MPU_STR_SnapShotM_SetMark(HANDLE hHandle, int nStartIndex, int nStopIndex, unsigned int bMark);


//!\brief       获取选择标记总数
//!\param   hHandle : HANDLE 查询的句柄
//!\return  成功为标记的总数，失败为-1
int MPU_STR_SnapShotM_GetMarkSum(HANDLE hHandle);


//!\brief 获取是否被标记
//!\param   hHandle : HANDLE 查询的句柄
//!\param   int nIndex 文件在总个数中的索引。从0开始
//!\return 标记范围ture，没有标记范围0，失败返回-1
int MPU_STR_SnapShotM_IsMark(HANDLE hHandle, int nIndex);

//!\brief       判断某个月份的每一天是否有图片文件
//!\param   int nCh
//!\param   int nYear
//!\param   int nMon
//!\param   int *data
//!\return  成功为0，其它为错误码
int MPU_STR_SnapShotM_IsFileExist(int nCh, int nYear, int nMon, int *pnData);

//!\brief   获取最后一次抓图的文件名(文件有可能被淘汰)
//!\param   int nCh 通道
//!\param   char *szFileName 输出文件名称(根据触发事件类型不同，获取到的名字组成不同)
//!\return  成功为0，失败为-1
int MPU_STR_SnapShotM_GetLastPic(int nCh, int nalarm_type, char *szFileName);

//!\brief   触发搬图
//!\param   nalarm_type  触发联动事件类型(0:Email 1:存储介质(SD, SAMBA或FTP里的一种))
//!\param   ntime  事件触发的时间(秒)
//!\param   utime  事件触发的时间(微秒)
//!\return  0表示成功，其它为错误码
int MPU_STR_TrigEventCapturePic(int nalarm_type, time_t ntime, time_t utime);

/*@breif MPU_STR_SetSdSnapShotParam 设置SD卡抓图参数
 *@param [IN] bEnable 是否使能SD卡抓图
 *@param [IN] nTimeInterval 抓图的时间间隔
 *@return 成功返回0
 *@       失败返回-1
 *@note   抓图时间间隔范围在MIN_SNAPSHOT_TIME_INTERVAL和MAX_SNAPSHOT_TIME_INTERVAL之间
 */
int MPU_STR_SetSdSnapShotParam(int bEnable, int nTimeInterval);

/*@breif MPU_STR_GetSdSnapShotParam 获取SD卡抓图参数
 *@param [OUT] pEnable 是否使能SD卡抓图
 *@param [OUT] pTimeInterval 抓图的时间间隔
 *@return 成功返回0
 *@       失败返回-1
 *@note   抓图时间间隔范围在MIN_SNAPSHOT_TIME_INTERVAL和MAX_SNAPSHOT_TIME_INTERVAL之间
 */
int MPU_STR_GetSdSnapShotParam(int *pEnable, int *pTimeInterval);

/*@breif MPU_STR_SetFTPSnapShotParam 设置FTP抓图参数
 *@param [IN] bEnable 是否使能FTP抓图
 *@param [IN] nTimeInterval 抓图的时间间隔
 *@return 成功返回0
 *@       失败返回-1
 *@note   抓图时间间隔范围在MIN_SNAPSHOT_TIME_INTERVAL和MAX_SNAPSHOT_TIME_INTERVAL之间
 */
int MPU_STR_SetFTPSnapShotParam(int bEnable, int nTimeInterval);

/*@breif MPU_STR_GetFTPSnapShotParam 获取FTP抓图参数
 *@param [OUT] pEnable 是否使能FTP抓图
 *@param [OUT] pTimeInterval 抓图的时间间隔
 *@return 成功返回0
 *@       失败返回-1
 *@note   抓图时间间隔范围在MIN_SNAPSHOT_TIME_INTERVAL和MAX_SNAPSHOT_TIME_INTERVAL之间
 */
int MPU_STR_GetFTPSnapShotParam(int *pEnable, int *pTimeInterval);

int MPU_STR_SetTimedCaptureCallBack(int (*CallBack)(int capture_type));

//!\brief    恢复SD卡录像
//!\return  成功为0，失败为-1
int MPU_STR_Disk_ResumeSdRecord();

//!\brief    暂停SD卡录像
//!\return  成功为0，失败为-1
int MPU_STR_Disk_PauseSdRecord();

#endif							/* _INC_MPU_STR_49A5F8B70324_INCLUDED */
