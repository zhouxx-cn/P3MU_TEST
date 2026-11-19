// Copyright (C) 1991 - 1999 Rational Software Corporation

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CSNAPSHOT_49B0B4170037_INCLUDED
#define _INC_CSNAPSHOT_49B0B4170037_INCLUDED

#include <time.h>
#include "SNAPSHOTINFO.h"
#include "PAR/CConfig.h"

#if 0
#ifdef TEST_MEM_INFO
#define MODULE_NAME "SnapShot"
#include "MemoryInterface.h"
#include "TCreateObject.h"

//#include "InternalCommon.h"

#endif
#endif

//!\brief  快照
//##ModelId=49B0B4170037
class CSnapShot
{
  public:

	CSnapShot();
	
	//构造函数
	CSnapShot(time_t tStartTime, time_t tStopTime, int nChannel);


	//!\brief   初始化
	//!\return 0-成功;其它为错误码
	int Init();

	//!\brief   释放资源
	//!\return 0-成功;其它为错误码
	int UnInit();

    CConfig * GetConfig();

    //!\brief 生成一个快照文件
    //!\param char *buffer_data 快照的数据，将该buff直接写入文件。
    //!\param time_t ntime 时间(秒)
    //!\param time_t utime 时间(微秒)
    //!\param int nsize    快照buff的长度
    //!\return  成功为0，失败为-1
    int CapturePic(time_t ntime, time_t utime, char * buffer_data, int nsize);

    //!\brief   触发搬图
    //!\param   nalarm_type  触发联动事件的类型(0:Email 1:FTP 2:SD 3:PUSH)
    //!\param   ntime  事件触发的时间(秒)
    //!\param   utime  事件触发的时间(微秒)
    //!\return  0表示成功，其它为错误码
    int TrigEvent(int nalarm_type, time_t ntime, time_t utime);

    //!\brief   获取操作的文件名
    //!\param   nalarm_type  触发联动事件的类型(0:Email 1:FTP 2:SD 3:PUSH)
    //!\return  0表示成功，其它为错误码
    int GetOpsPicFileName(int nalarm_type, char *pic_file);

    /*@breif SetSnapShotParam 设置SD卡抓图参数
     *@param [IN] bEnable 是否使能SD卡抓图
     *@param [IN] nTimeInterval 抓图的时间间隔
     *@return 成功返回0
     *@       失败返回-1
     *@note   抓图时间间隔范围在MIN_SNAPSHOT_TIME_INTERVAL和MAX_SNAPSHOT_TIME_INTERVAL之间
     */
    int SetSdSnapShotParam(int bEnable, int nTimeInterval);

    /*@breif GetSdSnapShotParam 获取SD卡抓图参数
     *@param [OUT] pEnable 是否使能SD卡抓图
     *@param [OUT] pTimeInterval 抓图的时间间隔
     *@return 成功返回0
     *@       失败返回-1
     *@note   抓图时间间隔范围在MIN_SNAPSHOT_TIME_INTERVAL和MAX_SNAPSHOT_TIME_INTERVAL之间
     */
    int GetSdSnapShotParam(int *pEnable, int *pTimeInterval);

    /*@breif SetFTPSnapShotParam 设置FTP抓图参数
     *@param [IN] bEnable 是否使能FTP抓图
     *@param [IN] nTimeInterval 抓图的时间间隔
     *@return 成功返回0
     *@       失败返回-1
     *@note   抓图时间间隔范围在MIN_SNAPSHOT_TIME_INTERVAL和MAX_SNAPSHOT_TIME_INTERVAL之间
     */
    int SetFTPSnapShotParam(int bEnable, int nTimeInterval);

    /*@breif GetFTPSnapShotParam 获取FTP抓图参数
     *@param [OUT] pEnable 是否使能FTP抓图
     *@param [OUT] pTimeInterval 抓图的时间间隔
     *@return 成功返回0
     *@       失败返回-1
     *@note   抓图时间间隔范围在MIN_SNAPSHOT_TIME_INTERVAL和MAX_SNAPSHOT_TIME_INTERVAL之间
     */
    int GetFTPSnapShotParam(int *pEnable, int *pTimeInterval);

    int SetTimedCaptureCallBack(int (*CallBack)(int capture_type));

    /* 定时抓图线程 */
    int TimedCaptureThreadProcess();

    /* 计时器线程 */
    int CapSecondTimerThreadProcess();

    //!\brief 获取实例指针
    //!\param void
    //!\return  成功返回指针，失败为NULL
    //##ModelId=49BDA6B000D2
    static CSnapShot *GetInstance();

    //!\brief 释放实例指针
    //!\return  成功返回0，失败为-1
    //##ModelId=49BDA6C50244
    static int ReleaseInstance();

     //快照句柄
    static CSnapShot *theCSnapShot;

  private:

	//配置文件
	CConfig *m_pCConfig;
  
    //导入缺省值
    int LoadParam();

    // 定时抓图回调函数
    int (*m_TimedCaptureCallBack)(int capture_type);

    // 使能SD卡定时抓图
    int m_bEnableSdTimedCapture;

    // SD卡定时抓图时间间隔
    int m_nSdTimedCapTimeInterval;

    // 使能FTP定时抓图
    int m_bEnableFTPTimedCapture;

    // FTP定时抓图时间间隔
    int m_nFTPTimedCapTimeInterval;
    
    unsigned int m_nTimeCount;

    pthread_t m_timed_capture_id;
    pthread_t m_sencond_timer_id;

    pthread_mutex_t  m_cap_mutex;
    pthread_mutex_t  m_operate_pic_mutex;
};

#endif							/* _INC_CSNAPSHOT_49B0B4170037_INCLUDED */
