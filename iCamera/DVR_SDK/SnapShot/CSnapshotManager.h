// Copyright (C) 1991 - 1999 Rational Software Corporation

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CSNAPSHOTMANAGER_49A6406E0367_INCLUDED
#define _INC_CSNAPSHOTMANAGER_49A6406E0367_INCLUDED

#include <time.h>
#include <pthread.h>
#include "HANDLE.h"
#include "SNAPSHOTINFO.h"
#include "CSnapShot.h"

//!\brief  快照管理器
//##ModelId=49A6406E0367
class CSnapshotManager
{
  public:
	//!\brief   生成一个快照文件
	//!\param   char* buffer_data 快照的数据，将该buffer直接写入文件。
	//!\param   time_t ntime   时间(秒)
	//!\param   time_t utime   时间(微秒)
	//!\param   int nsize    快照buffer的长度
	//!\return  成功为0，失败为-1
    int CapturePic(time_t ntime, time_t utime, char *buffer_data, int nsize);
    
    //!\brief   触发搬图
    //!\param   nalarm_type  触发联动事件的类型(0:Email 1:FTP 2:SD)
    //!\param   ntime  事件触发的时间(秒)
    //!\param   utime  事件触发的时间(微秒)
    //!\return  0表示成功，其它为错误码
    int TrigEvent(int nalarm_type, time_t ntime, time_t utime);

    //!\brief   获取操作的文件名
    //!\param   nalarm_type  触发联动事件的类型(0:Email 1:FTP 2:SD)
    //!\return  0表示成功，其它为错误码
    int GetNeedSendPicFileName(int nalarm_type, char *pic_file);

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

	//!\brief 获取快照管理实例指针
	//!\return  成功为为快照句柄，失败为NULL
	static CSnapshotManager *GetInstance();

	//!\brief 销毁实例
	//!\return  成功为为快照句柄，失败为NULL
	static int ReleaseInstance();

#ifndef TEST_MEM_INFO
  private:

	//快照管理器句柄
	static CSnapshotManager *theCSnapshotManager;
    
    CSnapShot *pCSnapShot;

	//##ModelId=49A6460E0321
	  CSnapshotManager();

	//##ModelId=49A646100324
	  virtual ~ CSnapshotManager();
#else
    //快照管理器句柄
    static CSnapshotManager *theCSnapshotManager;
    
    CSnapShot *pCSnapShot;
  
    //##ModelId=49A6460E0321
  	CSnapshotManager();
  
    //##ModelId=49A646100324
  	virtual ~ CSnapshotManager();
#endif

};

#endif							/* _INC_CSNAPSHOTMANAGER_49A6406E0367_INCLUDED */
