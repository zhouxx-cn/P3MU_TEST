// Copyright (C) 1991 - 1999 Rational Software Corporation

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CRECORD_49A25F780253_INCLUDED
#define _INC_CRECORD_49A25F780253_INCLUDED
#include "global_macro.h"
#include "tagEncCfg.h"
#include "enumRecordMode.h"
#include "tagRecordSchedule.h"
#include "tagAdvancEncCfg.h"
#ifdef __SUPPORT_SD_CARD__
#include "CDPManager.h"
#endif
#include "CConfig.h"
#include "CCoder.h"
#include "enumRecType.h"
#include "AVL_global.h"
#ifdef DO_SATA
#include "hisdk_sata.h"
#else
//#include "hisdk.h"
#include "zwsdk.h"
#endif



class CDPManger;

class CConfig;

//预录数据结构
typedef struct _PREV_DATA_T
{
	int i_pos;					//偏移
	int i_end;					//结束偏移
	int i_type;					//帧类型
	int i_len;					//帧长度
	int i_time;					//时间戳
	int i_time_usec;			//微秒数
	struct _PREV_DATA_T *next;	//下一帧结构
	char *p_buf;				//当前帧缓冲

} __attribute__ ((packed)) PREV_DATA_T;


//!\brief 处理录像业务、有关的操作及配置
//##ModelId=49A25F780253
class CRecord
{
  public:

	~CRecord();

	//!\brief 录像实例化
	//!\param int nCh               通道号
	//!\param HANDLE hEnc           编码器句柄
	//!\return void
	//##ModelId=49A260A10246
	CRecord(int nCh, HANDLE hEnc);

	//!\brief 录像初始化
	//!\param void。
	//!\return  成功为0，失败为-1
	int Init();

	//!\brief 获取制式
	//!\param int *pStandard,接受制式,1-NTSC,2-PAL,4-SECAM,0x80000000-none
	//!\return 成功0,否则失败
	//##ModelId=49B4856A0139
	static int GetStandard(int *pStandard);

	//!\brief 设置制式
	//!\param int Standard,1-NTSC,2-PAL,4-SECAM
	//!\return 成功0,否则失败
	//##ModelId=49B4856A0139
	static int SetStandard(int Standard);
	
	//!\brief 开启编码
	//!\return 成功0,其它失败
	int StartEncCapture();

	//!\brief 停止编码
	//!\return 成功0,其它失败
	int StopEncCapture();

    //!\breif 开始内存录像
    int StartAviRecord(int record_type);

	//!\brief 暂停/继续录像
	//!\param int bPause 暂停或者继续的标志。
	//!\return  成功为0，失败为-1
	//##ModelId=49A25F8E02EB
	int Pause(int bPause);

	//!\brief 启动录像模式：手动、自动、停止三种模式
	//!\param enumRecordeMode model 录像模式的一种
	//!\return  上次录像模式，失败为-1
	//##ModelId=49A25F9D017A
	int StartMode(enumRecordMode model);

	//!\brief 获取录像模式：手动、自动、停止三种模式
	//!\return  录像模式enumRecordeMode model 录像模式的一种
	enumRecordMode GetRecMode();

	//!\brief   设置录像码流索引
	//!\param   int nIndex  0表示主码流，1表示子码流，2表示高清子码流
	//!\return 成功为0，其它为错误码
	int SetRecStreamIndex(int nIndex);

	//!\brief       获取录像码率索引
	//!\return   录像码流索引(0表示主码流，1表示子码流，2表示高清子码流)
	int GetRecStreamIndex();
	
    /* 触发录像完毕之后，发送消息的回调函数 */
    int SetTrigRecordOverCallBack(int nCh, int (*CallBack)(unsigned char *));

	//!\brief 触发报警录像
	//!\return  成功为0，失败为-1
	int Motion_SetAlarmRec();
	int Ai_SetAlarmRec();
	
	int AlarmIn_SetAlarmRec();

	void *GetCurrentResolutionThreadProcess();

	//!\brief 获取当前录像的状态
	//!\param void
	//!\return  返回当前录像的状态，失败为-1
	//##ModelId=49A25FB00286
	int GetState();


	//!\brief 获取编码参数
	//!\param tagEncCfg *Info 编码参数结构体指针
	//!\param int nEnctype 录像的编码类型 0-一般,1-手动,2-事件
	//!\return 成功为0，失败为-1
	//##ModelId=49B714EC03AF
	int GetEncCfg(tagEncCfg * Info, int nEnctype);

	//!\brief 设置编码参数
	//!\param tagEncCfg Info 编码参数结构体
	//!\param int nEnctype 录像的编码类型 0-一般,1-手动,2-事件
	//!\return 成功为0，失败为-1
	//##ModelId=49A263B601BF
	int SetEncCfg(tagEncCfg * Info, int nEnctype);

	//!\brief   恢复编码参数到出厂设置
	//!\param   int nEnctype        录像的编码类型
	//!\return 成功为0，失败为-1
	int ResumeEncCfg(int nEnctype);

	//!\brief 获取预录时间
	//!\param void
	//!\return 预录时间
	//##ModelId=49B715AC003A
	int GetPreTime();

	//!\brief 设置预录时间
	//!\param int nTime 预录时间, 0-表示不限时间
	//!\return 成功为0，失败为-1
	//##ModelId=49B715DB02CC
	int SetPreTime(int nTime);

	//!\brief 获取延迟时间
	//!\param void
	//!\return 延迟时间
	//##ModelId=49B715E9001A
	int GetDelayTime();

	//!\brief 设置延迟时间
	//!\param int nTime 延迟时间
	//!\return 成功为0，失败为-1
	//##ModelId=49B715F802F6
	int SetDelayTime(int nTime);

    //!\brief 设置录像写入状态 0:暂停  1:写入
    int SetRecordWriteStatus(int record_write_status);

	//!\brief 获取录像的时间计划
	//!\param tagRecordSchedule* cfg 时间计划结构体
	//!\return 成功为0，失败为-1
	//##ModelId=49B7164C0058
	int GetSchedule(tagRecordSchedule * cfg);

	//!\brief 设置录像的时间计划
	//!\param ttagSchedule cfg 时间计划结构体
	//!\return 成功为0，失败为-1
	//##ModelId=49B716980242
	int SetSchedule(const tagRecordSchedule * cfg);


	//!\brief 获取新的录像的时间计划
	//!\param tagRecordScheduleV2* cfg 时间计划结构体
	//!\return 成功为0，失败为-1
	//##ModelId=49B7164C0058
	int GetScheduleV2(tagRecordScheduleV2 * cfg);

	//!\brief 设置新的录像的时间计划
	//!\param tagRecordScheduleV2 cfg 时间计划结构体
	//!\return 成功为0，失败为-1
	//##ModelId=49B716980242
	int SetScheduleV2(tagRecordScheduleV2 * cfg);

	//!\brief 输入数据
	//!\param void * buff1 buff1
	//!\param int buflen1 长度
	//!\param void * buff2
	//!\param int buflen2
	//!\param int FrameType 帧类型
	//!\return  成功为0，失败为-1
	//##ModelId=49AC9E1D039C
	int InputData(void *buff, int buflen, int FrameType, time_t tFrameTime, time_t tFrameTime_usec);

	//!\brief 定制录像模式
	//!\param int nCustomMode 0为一般模式，1为高级模式
	//!\return 成功为0，失败为-1
	//##ModelId=49B864580125
	static int SetRecordCustomMode(int nCustomMode);

	//!\brief 获取定制的信息
	//!\param int* nCustomMode 0为一般模式，1为高级模式
	//!\return 成功为0，失败为-1
	//##ModelId=49B8653002CA
	static int GetRecordCustomMode(int *nCustomMode);

	//!\brief 检测时间计划
	//!\param void
	//!\return 成功为0，失败为-1
	//##ModelId=49B732E400CC
	int Schedule();

	//!\brief 检测时间计划V2
	//!\param void
	//!\return 成功为0，失败为-1
	int ScheduleV2();

	//!\brief 强迫切换片段
	int ForceSwicth();

	PictureFormat_t SwitchPicFormat(int PicFormat);

	int GetAdvanceEnc(int *NormalAdvEncType, int *EventAdvEncType, tagVIDEO_SHELTER *pCfg);

	int SetAdvanceEnc(int NormalAdvEncType, int EventAdvEncType, tagVIDEO_SHELTER *pCfg);

    int HttpTriggerRecord(void);
    
    int StopRecordAndUinitMemRecord();

   /*@breif SetCurrentAviNumPerPage 设置每页显示多少个AVI文件
    *@param [IN]  avi_num_per_page 每页显示的AVI文件个数
    *@return 成功返回0
    *@       失败返回-1
    *@note  每页显示的AVI文件个数不能超过当前有效的AVI个数g_read_curr_avi_num，
    *@      每页显示的AVI文件个数不能超过最大能显示的AVI个数MAX_AVI_NUM_ONE_PAGE
    */
    int SetCurrentAviNumPerPage(int avi_num_per_page);

   /*@breif GetCurrentAviNumPerPage 获取显示多少个AVI文件
    *@return 成功返回AVI文件个数
    *@       失败返回-1
    */
    int GetCurrentAviNumPerPage(void);

   /*@breif GetCurrentAviTotalNum 获取当前AVI总个数
    *@return 成功返回当前总的AVI文件个数
    *@       失败返回-1
    */
    int GetCurrentAviTotalNum(void);

   /*@breif SetSwitchPageIsChoose 设置当前选中的是哪一个页面和当前录像路径类型
    *@param [IN]  choose_page 选中的页面(从1开始)
    *@param [IN]  path_type 当前录像路径类型 0: http路径  1:绝对路径
    *@return 成功返回0
    *@       失败返回-1
    */
   int SetSwitchPageIsChoose(int choose_page, int path_type);

   /*@breif GetSwitchPageIsChoose 获取当前选中的是哪一个页面
    *@return 成功返回当前选中的是哪一个页面
    *@       失败返回-1
    */
   int GetSwitchPageIsChoose(void);

   /*@breif GetCurrentAviListInfo 从给定的页面里面获取当前AVI文件列表
    *@param [OUT] p_avi_list 获取到的AVI文件列表
    *@param [OUT] p_avi_num_curr_page 当前页面有多少个AVI文件
    *@return 成功返回0
    *@       失败返回-1
    *@note   需要先调用SetSwitchPageIsChoose设置当前选中哪一页
    *@       需要先调用SetCurrentAviNumPerPage设置当前用户需要设置每页显示多少个AVI文件
    */
    int GetCurrentAviListInfo(void **p_avi_list, int *p_avi_num_curr_page);

    /* 设置当前要寻找的AVI的时间段 */
    int SetCurrentAviSearchTimeSeg(DateTime start_time, DateTime stop_time);

    /* 设置当前要寻找的PIC的时间段 */
    int SetCurrentPicSearchTimeSeg(DateTime start_time, DateTime stop_time);

    /*@breif SetCurrentPicNumPerPage 设置每页显示多少个图片文件
     *@param [IN]  pic_num_per_page 每页显示的图片文件个数
     *@return 成功返回0
     *@       失败返回-1
     *@note  每页显示的图片文件个数不能超过当前有效的图片个数g_read_curr_pic_num，
     *@      每页显示的图片文件个数不能超过最大能显示的图片个数MAX_PIC_NUM_ONE_PAGE
     */
    int SetCurrentPicNumPerPage(int pic_num_per_page);
    
    /*@breif GetCurrentAviNumPerPage 获取显示多少个图片文件
     *@return 成功返回图片文件个数
     *@       失败返回-1
     */
    int GetCurrentPicNumPerPage(void);
    
    /*@breif GetCurrentPicTotalNum 获取当前图片总个数
     *@return 成功返回当前总的图片文件个数
     *@       失败返回-1
     */
    int GetCurrentPicTotalNum(void);
    
    /*@breif SetSwitchPicPageIsChoose 设置当前选中的是哪一个页面和当前图片路径类型
     *@param [IN]  choose_page 选中的页面(从1开始)
     *@param [IN]  path_type 当前图片路径类型 0: http路径  1:绝对路径
     *@return 成功返回0
     *@       失败返回-1
     */
    int SetSwitchPicPageIsChoose(int choose_page, int path_type);
    
    /*@breif GetSwitchPageIsChoose 获取当前选中的是哪一个页面
     *@return 成功返回当前选中的是哪一个页面
     *@       失败返回-1
     */
    int GetSwitchPicPageIsChoose(void);
    
    /*@breif GetCurrentPicListInfo 从给定的页面里面获取当前图片文件列表
     *@param [OUT] p_pic_list 获取到的图片文件列表
     *@param [OUT] p_pic_num_curr_page 当前页面有多少个图片文件
     *@return 成功返回0
     *@       失败返回-1
     *@note   需要先调用SetSwitchPicPageIsChoose设置当前选中哪一页
     *@       需要先调用SetCurrentPicNumPerPage设置当前用户需要设置每页显示多少个图片文件
     */
    int GetCurrentPicListInfo(void **read_pic_page, int *p_pic_num_curr_page);

    /* 获取文件对应的文件信息 */
    int GetRecordFileInfoByFileName(const char *file_name, void* p_file_info);

    /* 获取磁盘资源占用情况(单位: MB) */
    int GetDiskResourceSize(int *pRecordMB, int *pPictureMB);

    /* 获取录像存放的比例(范围0~100) */
    int GetVideoUsedSpaceRate(void);

    /* 设置录像存放的比例(范围0~100) */
    int SetVideoUsedSpaceRate(int nVideoSpace);

    /*@breif GetFileReadWriteStatus 获取当前文件的读写状态
     *@param [IN]  file_name 需要操作的文件名(绝对路径)
     *@param [OUT] read_write_status 
     *@            0:  当前文件空闲
     *@            1:  当前文件正在读
     *@            2:  当前文件正在写
     *@param [OUT] file_size 文件大小(单位: 字节) 
     *@return 返回0表示成功
     *@      返回-1表示失败
     */
    int GetFileReadWriteStatus(const char *file_name, 
                                      unsigned int *read_write_status, 
                                      unsigned int *file_size);
    
	/* 设置获取夏令时回调 */
    int SetGetTimeOffsetCallBack(int (*CallBack)(int *, int *, int *));

    /* 传入的图片名字组成pic_name: 时间戳_标号，如:1532477851_01需要改为1532477851 */
    int UploadPictureToSdCard(const char *pic_name, unsigned int trig_type);

  private:

	//!\brief 缺省编码参数
	//!\return 成功为0，失败为-1

	int DefEncCfg();

	//!\brief 检测编码参数，进行编码控制
	//!\param void
	//!\return 成功为0，失败为-1

	int CheckEncCfg();


	//!\brief 开始预录
	//!\param void
	//!\return 成功为0，失败为-1
	//##ModelId=49B8653002CA
	int StartPrevRec();

	//!\brief 处理预录转存
	//!\param void
	//!\return 成功为0，失败为-1
	//##ModelId=49B8653002CA
	int DealPrevRec();

	//!\brief 停止预录
	//!\param void
	//!\return 成功为0，失败为-1
	//##ModelId=49B8653002CA
	int StopPrevRec();

	//初始化
	int InitPrevData();
	//释放资源
	int UnInitPrevData();
	//插入一数据到尾
	int AddPrevData(void *buff, int buflen, int FrameType, time_t tFrameTime, time_t tFrameTime_usec);
	//取得一数据
	PREV_DATA_T *GetPrevData();
	//取得一块空数据块
	PREV_DATA_T *GetPrevFreeData(int len);
	//删除所有预录数据
	int DelAllPrevData();
	//!\brief 从头删除一帧，在必要时删除到下一个I帧处
	void DelPrevData();


  private:
	//通道号
	int m_channel;
	//编码句柄
	HANDLE m_hEnc;
	//制式
	static int m_iStandard;
	//编码配置类型,普通/手动/事件/
	int m_iEncCfgtype;
	//##ModelId=49BF4B8202A5
	tagEncCfg RecordCfg[3];		//0-一般,1-手动,2-事件

	//高级编码参数配置(H264, H265)
    tagAdvanceEncCfg m_AdvanceEncCfg;
	
	//当前使用的配置
	tagEncCfg *pRecordCfg;
	//配置有更新
	int m_bChangeCfg;

	//录像计划
	tagRecordSchedule m_ScheduleCfg;

	//新的录像计划
	tagRecordScheduleV2 m_ScheduleCfgV2;
		
	//录像状态
	enumRecType m_iRecState;
	//当前计划录像类型,将要录像的
	enumRecType m_iRecSchedulType;
	//录像模式
	enumRecordMode m_iRecMode;
	//录像码流索引
	int m_iRecStreamIndex;
	//是否关掉自动录像模式
	int m_iCloseAutoMode;

	//需要切换录像类型
	int m_bNeedSwitch;

    //预录时间
    unsigned int m_iPreTime;

	//延迟录像时间
	int m_iDelayTime;
	//定制录像模式
	static int m_iCustomMode;
	//是否有移动报警录像
	int m_bMotionAlarmRec;
	//是否有智能检测报警录像
	int m_bAiAlarmRec;
	//是否有移动报警录像
	int m_bAlarmInAlarmRec;
	//报警录像启动时间
	int m_iAlarmRecTime;
	//预录状态,0-停止,1-预录，2-转存
	int m_bPrevRecord;
	//预录缓冲
	char *m_sPrevBuff;
	//预录缓冲大小
	int m_iPrevBuffLen;
	//预录记录头
	PREV_DATA_T *m_pHead;
	//预录记录尾
	PREV_DATA_T *m_pTail;
	//预录可以定改数据
	int m_bPrevRecReady;

	//是否暂停录像
	int m_bPause;
	
	//每页显示的AVI个数
    unsigned int m_iShowAviNumPerPage;

    //当前显示哪一页
    unsigned int m_iSwitchPageIsChoose;

    //每页显示的图片个数
    unsigned int m_iShowPicNumPerPage;

    //当前图片显示哪一页
    unsigned int m_iSwitchPicPageIsChoose;

    //当前录像的路径类型 0: http路径  1:绝对路径
    unsigned int m_iRecordPathType;

    //当前图片的路径类型 0: http路径  1:绝对路径
    unsigned int m_iPicturePathType;

    //要寻找的avi的起始时间
    unsigned int m_nSearchAviStartTime;

    //要寻找的avi的结束时间
    unsigned int m_nSearchAviStopTime;

    //要寻找的pic的起始时间
    unsigned int m_nSearchPicStartTime;

    //要寻找的pic的结束时间
    unsigned int m_nSearchPicStopTime;

    //寻找到的AVI的总数
    int m_nSearchAviTotalNum;

    //寻找到的PIC的总数
    int m_nSearchPicTotalNum;

    //当前SD卡里面视频录像占用整个SD卡容量的比例
    unsigned int m_iVideoRate;

	//判断是否支持该编码格式,返回支持的编码格式
	int IsSupport(int EncFormat);

    int IsSupportV2(int width, int height);

	//支持最大的编码格式
	int GetMaxEncCap();

	//是否强制切换片段
	int m_bForceSwitch;

	//上次暂停时录像类型
	enumRecType m_iLastRecType;
	enumRecordMode m_iLastRecMode;

    unsigned int m_nH264Width;
    unsigned int m_nH264Height;

	//锁
	pthread_mutex_t m_lock;

	pthread_t m_GetCurrentResolutionThread;
#ifdef __SUPPORT_SD_CARD__
	//存储管理
	CDPManager *pCDPManager;
#endif	
	//配置文件
	CConfig *pCConfig;

	//板型
	char m_szType[9][8];

	//sensor 输入频率
	CmosFrequencyType_t m_SensorFrequency;

    int  m_bHttpEventIsComming;  // HTTP Event触发到来  
    int  m_iHttpEventRecTime;    // http event录像启动时间

  private:
	int LoadParam();

	int StartRec(enumRecType recType);

};

#endif							/* _INC_CRECORDE_49A25F780253_INCLUDED */
