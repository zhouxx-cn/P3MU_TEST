
/*!
*\file   CDPManager.h 
*\brief  数据处理管理器
*\author cwf
*\date	  2009/03/24
*\version 0.1
*/

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CDPManager_49A3744A00ED_INCLUDED
#define _INC_CDPManager_49A3744A00ED_INCLUDED

#include <pthread.h>
#include <time.h>
//#include "CDPopM.h"
//#include "CDSegM.h"
#include "tagTime.h"
#include "HANDLE.h"
#include "dp_callbackfun.h"

#ifndef HotplugInfo
typedef struct _HOTPLUGINFO HotplugInfo;
#endif

#ifndef HY_REPLAY_DATABACKUP_CALLBACK

/*!
 * \brief 数据备份回调函数
 * \param hDataBackup 数据备份器句柄
 * \param BackupSize 已备份大小(单位：KB,全F表示结束)
 * \param BackupFrameTime 当前绝对时间
 * \param EventID 发生的事件类型，如果是正常的数据备份，EventID为DATABACKUP_EVENT_NONE，其他事件时，帧相关的参数无效
 * \return
 */
typedef int (*HY_REPLAY_DATABACKUP_CALLBACK) (HANDLE hDataBackup, unsigned long long BackupSize, time_t BackupFrameTime, unsigned long EventID);
#endif

class CPNodeTest;

struct tagRecSeg;

//!\brief 1、管理数据的信息的获取、数据的获取、数据的记录等
//!\brief 2、提供成员方法的interface
//!\brief 
//3、利用不同的连接上下文不一致，来区分不用的对象，这样在外层
//!\brief 不需要做任何对象的映射。
//##ModelId=49A3744A00ED
class CDPManager
{
  public:


	//!\brief 功能：打开指定磁盘分组类型的可查读属性.
	//!\param GroupType 磁盘分组类型.
	//!\return  成功为0，失败为-1
	int DPM_SetDiskGroupReadable(int GroupType);

	//!\brief 功能：设置数据弹出器属性。
	//!\param int DiskGroupMask 0表示查询普通分组+报警分组、1表示冗余分组、2表示备份分组
	//!\param hHandle:CDPopM 弹出器句柄
	//!\param nRefChan : 参考通道(可以不设置)。
	//!\param time_t tStartSeekTime 开始时间
	//!\param time_t tEndSeekTime    结束时间
	//!\param PopCallBack CallBack   弹出器数据回调
	//!\return  成功为0，失败为-1
	//##ModelId=49AF966D00AF
	int DPM_SetAttr(void *hHandle, int DiskGroupMask, int nRefChan, time_t tStartSeekTime, time_t tEndSeekTime, DATAPOP_CALLBACK PopCallBack);


	//!\brief 功能：设置数据弹出器属性。
	//!\param int DiskGroupMask 0表示查询普通分组+报警分组、1表示冗余分组、2表示备份分组
	//!\param hHandle:CDPopM 弹出器句柄
	//!\param nRefChan : 参考通道(可以不设置)。
	//!\param time_t tStartSeekTime 开始时间
	//!\param time_t tEndSeekTime    结束时间
	//!\param PopCallBack CallBack   弹出器数据回调
	//!\param int bDynPop            是否动态弹出
	//!\return  成功为0，失败为-1
	//##ModelId=49AF966D00AF
	int DPM_SetAttr(void *hHandle, int DiskGroupMask, int nRefChan, time_t tStartSeekTime, time_t tEndSeekTime, DATAPOP_CALLBACK PopCallBack,
					int bDynPop);

	//!\brief 创建一个弹出器
	//!\param void
	//!\return  成功为弹出器句柄，失败为NULL
	//##ModelId=49AF96C702E9
	void *DPM_Create();

	//!\brief 销毁一个弹出器
	//!\param void* hHandle 弹出器句柄
	//!\return  成功为0，失败为-1
	//##ModelId=49AF96EC009E
	int DPM_Destroy(void *hHandle);

	//功能：
	//向弹出器添加一个通道
	//参数：
	//hHandle : void* 弹出器句柄
	//int nChannel 通道
	//int bDynPop  新的数据是否动态弹出
	//返回值：
	//成功为0，失败为-1
	//##ModelId=49AF972800B8
	int DPM_AddCh(void *hHandle, int NChannel, int bDynPop);

	//功能：
	//从删除一个通道
	//参数：
	//hHandle : void* 弹出器句柄
	//int nChannel 通道
	//返回值：
	//成功为0，失败为-1
	//##ModelId=49AF974300FD
	int DPM_DelCh(void *hHandle, int nChannel);

	//!\brief 设置弹出器弹出的位置
	//!\param void* hHandle 弹出器句柄
	//!\param time_t SeekTime 弹出器的位置
	//!\return  成功为0，失败为-1
	//##ModelId=49AF975602F9
	int DPM_Seek(void *hHandle, time_t SeekTime);


	//!\brief 获取一帧关键帧
	//!\param hDataPopper 数据弹出器的句柄
	//!\param iChannel 通道号
	//!\param tSeekTime 指定时间簇
	//!\param precision 时间簇误差
	//!\param pbuf 保存该关键帧的缓冲区指针
	//!\param bufsize 输入时为缓冲区大小，输出时为关键帧大小
	//!\return 成功返回0，失败返回小于0的错误码。
	int DPM_GetOneKeyFrame(HANDLE hHandle, int iChannel, time_t tSeekTime, unsigned int precision, unsigned char *pbuf, unsigned int *bufsize);

	//!\brief 开始弹出
	//!\param void* hHandle 弹出器句柄
	//!\return  成功为0，失败为-1
	//##ModelId=49AF97650387
	int DPM_StartPop(void *hHandle);

	//!\brief 结束弹出
	//!\param void* hHandle 弹出器句柄
	//!\return  成功为0，失败为-1
	//##ModelId=49AF97750307
	int DPM_StopPop(void *hHandle);

	//!\brief 向弹出器添加一种录像类型
	//!\param void* hHandle 弹出器句柄
	//!\param int RecType 录像类型（见录像模块定义）
	//!\return  成功为0，失败为-1   
	int DPM_AddRecType(void *hHandle, int RecType);


	//!\brief 删除弹出器中一种录像类型
	//!\param void* hHandle 弹出器句柄
	//!\param int RecType 录像类型（见录像模块定义）
	//!\return  成功为0，失败为-1
	int DPM_DelRecType(void *hHandle, int RecType);

	//!\brief 设置弹出器弹出数据类型
	//!\param void* hHandle 弹出器句柄
	//!\param int nDataType 数据类型。0为所有帧，1为I帧
	//!\param PopKeyInterval 当只弹出关键帧时，该参数有效，表示通道弹出每隔多少关键帧弹出一个，此值限制在0-3，0表示全部弹出
	//!\return  成功为0，失败为-1
	//##ModelId=49B8C58401F7
	int DPM_SetPopDataType(void *hHandle, int nDataType, unsigned int PopKeyInterval);

	//!\brief 设置弹出器弹出数据方向
	//!\param void* hHandle 弹出器句柄
	//!\param bool bFront 0为向前，1为向后
	//!\return  成功为0，失败为-1
	//##ModelId=49B8C59C0274
	int DPM_SetPopDataDirection(void *hHandle, bool bFront);


	//!\brief 获取弹出器中的数据大小（KB）
	//!\return  成功为数据大小，失败为-1
	//##ModelId=49B8C59C0274
	long long DPM_GetSize(void *hHandle);

	//!\brief 创建一个数据片段信息弹出器
	//!\param void
	//!\return  成功为弹出器句柄，失败为NULL
	//##ModelId=49B8AC3801FF
	void *DSM_Create();

	//!\brief 销毁一个数据片段信息弹出器
	//!\param void* hHandle 弹出器句柄
	//!\return  成功为0，失败为-1
	//##ModelId=49B8AD7C016F
	int DSM_Destroy(void *hHandle);

	//##ModelId=49AF97A10062
	//!\brief 设置弹出器属性。之后才能进行弹出器的其他操作
	//!\param int DiskGroupMask 0表示查询普通分组+报警分组、1表示冗余分组、2表示备份分组
	//!\param void* hHandle 弹出器句柄
	//!\return  成功为0，失败为-1
	int DSM_SetAttr(void *hHandle, int DiskGroupMask, time_t tStartSeekTime, time_t tEndSeekTime, SEGMENT_CALLBACK CallBack);

	//##ModelId=49AF97CC0078
	int DSM_AddCh(void *hHandle, int nChannel, int bDynReport);

	//!\brief 删除一个通道
	//!\param void* hHandle 弹出器句柄
	//!\param int NChannel 通道
	//!\return  成功为0，失败为-1
	//##ModelId=49AF97DA038F
	int DSM_DelCh(void *hHandle, int nChannel);

	//!\brief 开始弹出
	//!\param void* hHandle 弹出器句柄
	//!\return  成功为0，失败为-1
	//##ModelId=49AF97F900B8
	int DSM_StartPop(void *hHandle);

	//!\brief 结束弹出
	//!\param void* hHandle 弹出器句柄
	//!\return  成功为0，失败为-1
	//##ModelId=49AF980600D5
	int DSM_StopPop(void *hHandle);

	//!\brief 获取录像片段信息
	//!\param int DiskGroupMask 0表示查询普通分组+报警分组、1表示冗余分组、2表示备份分组
	//!\param int nChannel 通道
	//!\param time_t tBeginTime 开始时间
	//!\param time_t tEndTime 结束时间  
	//!\param int RecType 录像类型
	//!\param RECSEG ** pRecSegHead 录像片段指针
	//!\param nCount 录像片段的个数
	//!\return  成功为0，失败为-1
	//##ModelId=49AF98CD009F
	int DSM_GetRecSegList(int DiskGroupMask, int nChannel, time_t tBeginTime, time_t tEndTime, int RecType, tagRecSeg ** pRecSegHead, int *nCount);

	//!\brief 释放录像片段信息
	//!\tagRecSeg *pRecSegHead 释放链表指针
	//!\return  成功为0，失败为-1
	//##ModelId=49AF98D80308
	int DSM_ReleaseRecSegList(tagRecSeg * pRecSegHead);

	//!\brief 设置录像类型
	int DR_SetRecordType(int nCh, int nRecType);
	
    /* 触发录像完毕之后，发送消息的回调函数 */
    int DR_SetTrigRecordOverCallBack(int nCh, int (*CallBack)(unsigned char *));
    
    //!\brief 设置预录时间(单位: 秒)
    int DR_SetPrevTimeSec(int nCh, unsigned int nPrevTimeSec);
    
    //!\brief 获取预录时间(单位: 秒)
    unsigned int DR_GetPrevTimeSec(int nCh);

	//!\brief 设置录像延迟时间（秒）
	int DR_SetRecordDelayTime (int nCh, unsigned int nDelaySec);

	//!\brief 获取录像延迟时间（秒）
	int DR_GetRecordDelayTime (int nCh, unsigned int *pnDelaySec);

    //!\brief 设置录像写入状态 0:暂停  1:写入
    int DR_SetRecordWriteStatus(int nCh, int record_write_status);

	//!\brief 开始记录数据
	//!\param int nChannel 通道
	//!\return  成功为0，失败为-1
	//##ModelId=49AF9963016D
	int DR_StartRecordData(int nCh, int record_type, int bH265);

    int DR_UploadPictureToSdCard(const char *pic_name, unsigned int trig_type);

	//!\brief 暂停记录数据
	//!\param 无
	//!\return  成功返回0，失败为-1
	//##ModelId=49A645FD00D0

	int PauseRecord(int nCh, int bPause);

	//!\brief 删除录像数据
	//!\param int nDiskGroupType 盘组类型
	//!\param int nChannel 通道
	//!\param int nRecType 录像类型
	//!\param time_t tBeginTime 开始时间
	//!\param time_t tEndTime 结束时间
	//!\return  成功为0，失败为-1
	//##ModelId=49AF996A0181
	int DR_DelRecordData(int nDiskGroupType, int nCh, int nRecType, time_t tStartTime, time_t tEndTime);


	//!\brief 删除录像数据
	//!\param int nDiskGroupType 盘组类型
	//!\param time_t tBeginTime 开始时间
	//!\param time_t tEndTime 结束时间
	//!\return  成功为0，失败为-1
	//##ModelId=49AF996A0181
	int DR_DelRecordData(int nDiskGroupType, time_t tStartTime, time_t tEndTime);
    
    /*@breif DR_SetCurrentAviNumPerPage 设置每页显示多少个AVI文件
     *@param [IN]  avi_num_per_page 每页显示的AVI文件个数
     *@return 成功返回0
     *@       失败返回-1
     *@note  每页显示的AVI文件个数不能超过当前有效的AVI个数g_read_curr_avi_num，
     *@      每页显示的AVI文件个数不能超过最大能显示的AVI个数MAX_AVI_NUM_ONE_PAGE
     */
    int DR_SetCurrentAviNumPerPage(int avi_num_per_page);

    /*@breif DR_GetCurrentAviTotalNum 获取当前AVI总个数
     *@return 成功返回当前总的AVI文件个数
     *@       失败返回-1
     */
    int DR_GetCurrentAviTotalNum(void);

    /*@breif DR_SetSwitchPageIsChoose 设置当前选中的是哪一个页面和当前录像路径类型
     *@param [IN]  choose_page 选中的页面(从1开始)
     *@param [IN]  path_type 当前录像路径类型 0: http路径  1:绝对路径
     *@return 成功返回0
     *@      失败返回-1
     */
    int DR_SetSwitchPageIsChoose(int choose_page, int path_type);

    /*@breif DR_UpdateAviListByTime 通过给定的时间戳更新AVI文件列表
     *@param [IN]  start_time 要搜索的起始时间
     *@param [IN]  stop_time 要搜索的结束时间(为0表示搜索全部文件)
     *@param [OUT] p_search_avi_count 搜索到符合条件的AVI文件总个数
     *@return 成功返回0
     *@      失败返回-1
     */
    int DR_UpdateAviListByTime(unsigned int start_time, 
                                      unsigned int stop_time, 
                                      int *p_search_avi_count);

    /*@breif DR_GetCurrentAviListInfo 通过给定的时间戳获取对应的AVI文件列表
     *@param [IN]  read_avi_page 需要获取第几页的AVI信息(从1开始)
     *@param [OUT] p_avi_list 获取到的AVI文件列表
     *@param [OUT] p_avi_num_curr_page 当前页面有多少个AVI文件
     *@return 成功返回0
     *@      失败返回-1
     *@note  需要先调用SetCurrentAviNumPerPage设置当前用户需要设置每页显示多少个AVI文件
     */
    int DR_GetCurrentAviListInfo(int read_avi_page, void **p_avi_list, int *p_avi_num_curr_page);

    /*@breif DR_SetCurrentPicNumPerPage 设置每页显示多少个图片文件
     *@param [IN]  pic_num_per_page 每页显示的图片文件个数
     *@return 成功返回0
     *@       失败返回-1
     *@note  每页显示的图片文件个数不能超过当前有效的图片个数g_read_curr_pic_num，
     *@      每页显示的图片文件个数不能超过最大能显示的图片个数MAX_PIC_NUM_ONE_PAGE
     */
    int DR_SetCurrentPicNumPerPage(int pic_num_per_page);

    /*@breif DR_GetCurrentPicTotalNum 获取当前图片总个数
     *@return 成功返回当前总的图片文件个数
     *@       失败返回-1
     */
    int DR_GetCurrentPicTotalNum(void);

    /*@breif DR_UpdatePicListByTime 通过给定的时间戳更新PIC文件列表
     *@param [IN]  start_time 要搜索的起始时间
     *@param [IN]  stop_time 要搜索的结束时间(为0表示搜索全部文件)
     *@param [OUT] p_search_pic_count 搜索到符合条件的PIC文件总个数
     *@return 成功返回0
     *@      失败返回-1
     */
    int DR_UpdatePicListByTime(unsigned int start_time, 
                                      unsigned int stop_time, 
                                      int *p_search_pic_count);

    /*@breif DR_GetCurrentPicListInfo 通过给定的时间戳获取对应的图片文件列表
     *@param [IN]  read_pic_page 需要获取第几页的图片信息(从1开始)
     *@param [OUT] p_pic_list 获取到的图片文件列表
     *@param [OUT] p_pic_num_curr_page 当前页面有多少个图片文件 
     *@return 成功返回0
     *@      失败返回-1
     *@note  需要先调用GetPicGlobalHeadInfoFromGlobalFile将需要的数据放到全局结构体里面
     *@      需要先调用SetCurrentPicNumPerPage设置当前用户需要设置每页显示多少个图片文件
     */
    int DR_GetCurrentPicListInfo(int read_pic_page, void **p_pic_list, int *p_pic_num_curr_page);

    /* 获取磁盘资源占用情况(单位: MB) */
    int DR_GetDiskResourceSize(int *pRecordMB, int *pPictureMB);

    /* 获取录像存放的比例(范围0~100) */
    int DR_GetVideoUsedSpaceRate(void);

    /* 设置录像存放的比例(范围0~100) */
    int DR_SetVideoUsedSpaceRate(int nVideoSpace);

    /* 设置录像尺寸 */
    int DR_SetRecordSize(int width, int height, int bH265);

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
    int DR_GetFileReadWriteStatus(const char *file_name, 
                                          unsigned int *read_write_status,
                                          unsigned int *file_size);

    /*@breif DR_GetRecordDataSize 获取指定时间段的录像数据大小
     *@param [IN] start_time 开始时间
     *@param [IN] stop_time  结束时间
     *@param [IN] record_type 筛选的录像类型(当前无效)
     *@param [OUT] p_size 获取到的帧数据大小
     *@return 返回  0 表示获取成功
     *@       返回 -1 表示获取失败
     *@note   获取到的是数据帧大小，包括帧头
     */
    int DR_GetRecordDataSize(unsigned int start_time, 
                                        unsigned int stop_time, 
                                        int record_type, 
                                        unsigned int * p_size);

    /* 获取当前回放的录像信息 */
    int DR_GetRecordInfo(int *pCurrentRecordStartTime);

    /* 获取文件对应的文件信息 */
    int DR_GetRecordFileInfoByFileName(const char* pFileName, void* pFileInfo);

    /* 设置获取时间偏移回调 */
    int DR_SetGetTimeOffsetCallBack(int (*CallBack)(int *, int *, int *));

	//!\brief 输入录像数据。必需在调用开始记录之后
	//!\param int nChannel 通道
	//!\param const char* Buff 数据buff
	//!\param int nLen 数据长度
	//!\param int nFramType 数据类型
	//!\return  成功为0，失败为-1
	//##ModelId=49B8C5F90304
	int DR_InputData(int nCh, time_t tFrameTime, unsigned long tFrameTimeInUs, const char *Buff, int nLen, int nFramType);

    int DR_StopRecordAndUinitMemRecord(int nCh);

    int DR_InputDataToRingBuffer(int nCh, time_t tFrameTime, unsigned long tFrameTimeInUs, const char *Buff, int nLen, int nFramType);

	//!\brief 设置备份时间计划。不保存，有自动备份模块保存，需要自动备份模块设置
	//!\param int DiskGroupMask 0表示查询普通分组+报警分组、1表示冗余分组、2表示备份分组
	//!\param itStartTime:time_t 备份的开始时间
	//!\param tStopTime:time_t 备份的结束时间   
	//!\HY_REPLAY_DATABACKUP_CALLBACK callBack 回调函数接口
	//!\return  成功返回备份器句柄，失败为NULL
	//##ModelId=49BDAF450063
	HANDLE DP_BackupCreate(int DiskGroupMask, time_t tStartTime, time_t tStopTime, HY_REPLAY_DATABACKUP_CALLBACK callBack);

	//!\brief 释放备份器
	//!\param HANDLE handle 备份器句柄
	int DP_BackupDestroy(HANDLE handle);

	//!\brief 添加备份通道
	//!\param HANDLE handle 备份器句柄
	//!\param int nCh 备份通道
	//!\return  成功返回0，失败错误号
	int DP_BackupAddCh(HANDLE handle, int nCh);

	//!\brief 添加备份录像类型
	//!\param HANDLE handle 备份器句柄
	//!\param int nRecType 录像类型
	//!\return  成功返回0，失败错误号
	int DP_BackupAddRecType(HANDLE handle, int nRecType);


	//!\brief 开始备份
	//!\param HANDLE handle 备份器句柄
	//!\return  成功返回0，失败为-1
	//##ModelId=49BDAF45029D
	int DP_BackupStart(HANDLE handle);

	//!\brief 停止备份
	//!\param HANDLE handle 备份器句柄
	//!\return  成功返回0，失败为-1
	//##ModelId=49BDAF450316
	int DP_BackupStop(HANDLE handle);

	//!\brief 获取此次备份的数据大小
	//!\param HANDLE handle 备份器句柄
	//!\param unsigned long *nSize 数据大小
	//!\return  成功返回0，失败为-1
	int DP_BackupGetSize(HANDLE handle, unsigned long long *nSize);

	//!\brief 初始化
	//!\return  成功返回0，失败为-1
	//##ModelId=49BF19D80049
	int Init();

	//!\brief 释放资源
	//!\return  成功返回0，失败为-1
	//##ModelId=49BF19D903A8
	int UnInit();

	//!\brief   判断某天是否有录像数据
	//!\param int DiskGroupMask 0表示查询普通分组+报警分组、1表示冗余分组、2表示备份分组
	//!\param   int nYear
	//!\param   int nMon
	//!\param   int nDay
	//!\return  有数据返回true,无数据返回false
	//##ModelId=49A646100324
	static bool IsFileExist(int DiskGroupMask, int nYear, int nMon, int nDay);

	//!\brief   判断某月是否有录像数据
	//!\param int DiskGroupMask 0表示查询普通分组+报警分组、1表示冗余分组、2表示备份分组
	//!\param   int nYear
	//!\param   int nMon
	//!\return  0-30分别代表1日制31日
	//##ModelId=49A646100324
	static int IsFileExist(int DiskGroupMask, int nYear, int nMon);

	//!\brief 获取实例指针
	//!\param void
	//!\return  成功返回指针，失败为NULL
	//##ModelId=49BDA6B000D2
	static CDPManager *GetInstance();

	//!\brief 释放实例指针
	//!\return  成功返回0，失败为-1
	//##ModelId=49BDA6C50244
	static int ReleaseInstance();

  private:

	//!\brief 获取编码通道的个数
	//!\param 无
	//!\return  成功返回0，失败为-1
	int GetEncChannel();

	//!\brief 获取数据管理器的操作权
	//!\param 无
	//!\return  成功返回0，失败为-1
	int Capture();

	//!\brief 释放数据管理器的操作权
	//!\param 无
	//!\return  成功返回0，失败为-1
	int Release();

	//备份句柄
	HANDLE m_hBackUp;

	static CDPManager *theCDPManager;

	int m_EnChannel;
	//数据管理器操作原子变量
	pthread_mutex_t mutex;

	friend class CDiskGroupManger;
	friend class CDiskManger;

	CPNodeTest *theChildObj;

	friend void *STR_NewDisk(HotplugInfo hpInfo);
	friend void *STR_DelDisk(HotplugInfo hpInfo);
	friend void Sata_AddCallBack(HotplugInfo hpInfo);
	friend void Sata_RemoveCallBack(HotplugInfo hpInfo);
#ifndef TEST_MEM_INFO
	//##ModelId=49BE1F2A03D3
	  CDPManager();
	class CDRecord *m_CDRecord;
	//##ModelId=49BE1F2D0098
	 ~CDPManager();
#else
public:
	//##ModelId=49BE1F2A03D3
	  CDPManager();
	class CDRecord *m_CDRecord;
	//##ModelId=49BE1F2D0098
	 ~CDPManager();
#endif

};

#endif							/* _INC_CDPManager_49A3744A00ED_INCLUDED */
