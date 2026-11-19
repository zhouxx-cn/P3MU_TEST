// Copyright (C) 1991 - 1999 Rational Software Corporation

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CCAPTUREPIC_49B5C6AD006C_INCLUDED
#define _INC_CCAPTUREPIC_49B5C6AD006C_INCLUDED

#include "global_macro.h"
#include "CConfig.h"
#include "CSnapshotManager.h"
#include <time.h>

#include "zwsdk.h"

#include "AVL_global.h"
#include "pthread.h"

typedef struct stCapResolution_U
{
    unsigned short int m_iCapWidth;
    unsigned short int m_iCapHeight;
}stCapResolution;

//预览抓图
//配置文件为全部对象共有
//##ModelId=49B5C6AD006C
class CCapturePic
{
  public:
	CCapturePic(int nCh, void *hHandle);
	 ~CCapturePic();
	//!\brief 从流里面获取一帧数据作为抓图数据
	//!\note  时间精确到ms，图片大小不超过MAX_PICTURE_BUF_SIZE，可以和远程抓图同时进行
	//!\return  成功为0，失败为-1
	int Capture();
	//!\brief 抓图
	//!\param int bAlarm 
	//是否为联动抓图。false为手动抓图，true为联动抓图
	//!\param int AlarmType 触发源类型
	//!\param int AlarmCh 触发源通道
	//!\return  成功为0，失败为-1
	
	
	int Capture(int bAlarm, int AlarmType, int AlarmCh);

	//!\brief 输入数据
	//!\param void * buff1 buff1
	//!\param int buflen1 长度
	//!\param void * buff2
	//!\param int buflen2
	//!\param int FrameType 帧类型
	//!\return  成功为0，失败为-1
	int InputData(void *buff, int buflen, int FrameType, time_t tFrameTime, time_t tFrameTime_usec);

	//!\brief 循环抓图线程
    	void *CircleCaptureThreadProcess();
	


	//!\brief 获取抓拍参数。手动抓图不受该参数影响
	//!\praram int nCount 连续抓拍多少张
	//!\praram int nIntervalTime 间隔时间
	//!\return  成功为0，失败为-1
	//##ModelId=49BDEFE00263
	int GetCaptureCfg(int *nCount, int *nIntervalTime, int *nFormat = NULL, int *nWidth = NULL, int *nHeight = NULL, int *nQuality = NULL);

	//!\brief 设置抓拍参数。手动抓图不受该参数影响
	//!\praram int nCount 连续抓拍多少张
	//!\praram int nIntervalTime 间隔时间
	//!\return  成功为0，失败为-1
	//##ModelId=49BDEFE002DB
	int SetCaptureCfg(int nCount, int nIntervalTime, int nFormat = 1, int nWidth = 352, int nHeight = 288, int nQuality = 0);

	//!\brief	获取抓码流类型
	//!\param	char nStreamType 0表示主码流，1表示第一子码流，2表示第二子码流
	//!\return	成功为0，失败为-1
	int GetCaptureStreamType(char *nStreamType);

	//!\brief	设置抓码流类型
	//!\param	char nStreamType 0表示主码流，1表示第一子码流，2表示第二子码流
	//!\return	成功为0，失败为-1
	int SetCaptureCfgV2(int nQualitychar, char nStreamType);

	//检查拍照
	int CheckCapturePic();
  private:
	//通道号
	int m_iChannel;
	//编码句柄
	HANDLE m_henc;

	//锁
	pthread_mutex_t m_Capturelock;
	int use_cnt;
	//是否连抓
	int m_bAlarm;
	//抓拍张数
	int m_iCount;

	//时间间隔
	int m_iIntervalTime;	//秒
	
    int m_iFormat;

	//抓图码流类型  ，  0表示主码流，1表示第一子码流，2表示第二子码流
	char m_nStreamType;
	
    stCapResolution m_stCapRes;

	int m_iQuality;	//抓拍质量

	//关于帧缓冲问题
    int m_buflen;                             // 帧缓冲大小(最大暂定为MAX_PICTURE_BUF_SIZE)
    int m_FrameType;                          // 帧类型
    time_t m_tFrameTime;                      // 时间戳(秒)
    time_t m_tFrameTime_usec;                 // 时间戳(微秒)
    char *m_pFrameBuffer; // 帧缓冲区(最大暂定为MAX_PICTURE_BUF_SIZE)
    pthread_mutex_t m_frame_mutex;

    pthread_t m_CircleCaptureThread;                 // 循环抓图线程
	//当前已抓张数
	int m_iCount_already;
	//上次拍照时间
	time_t m_iLastCaptureTime;	//秒
	//照片缓冲
	char *m_pBuff;
	//照片缓冲大小
	unsigned int m_isize;

	//上次报警时间
	time_t m_last_AlarmTime;

	//报警触发计数 0~(MAX_CHN_NUM-1) - 报警输入 
	//MAX_CHN_NUM~(2*MAX_CHN_NUM-1) - 移动侦测
	//2*MAX_CHN_NUM~(3*MAX_CHN_NUM-1) -人形检测
	int m_AlarmAlart[ALARM_ALART_SIZE];

	//配置文件
	CConfig *pCConfig;
	//快照管理
	#ifndef DEBUG_NOT_USE_SNP_LIB
	CSnapshotManager *pCSnapshotManager;
	#endif
  private:
	int LoadParam();
};

#endif							/* _INC_CCAPTUREPIC_49B5C6AD006C_INCLUDED */
