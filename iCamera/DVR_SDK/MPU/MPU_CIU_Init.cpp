
/*!
 *\file		MPU_CIU_Preview.cpp
 *\brief	MPU模块，远程预览的实现
 *\author	林云
 *\date		2009/06/15

功能：
-#	
-#	远程预览

说明：
-#	本文件目前用老版本网络SDK实现
 */


#include "MPU_CIU_global.h"
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include "common_pub.h"
#include "MPU_LCD_API.h"
/**************************************************************************
 *                            重启网络服务处理                                                                     *
 **************************************************************************/

//重启网络服务线程
static void *reset_hieserver_thread(void *param);

extern int MPU_AVL_SetFaceEventCallBack(int nCh, int (*CallBack) (struct tagAlertInfo *pAlertInfo));

//如果远程改变了网络参数，则需要异步重启网络服务
typedef struct tag_reset_server_mgt
{
	pthread_t m_thrdID;			//线程ID
	pthread_mutex_t m_Mutex;	//互斥锁

	//是否正在进行异步重启过程
	volatile bool m_bReset;

#ifdef CIU_RESET_THREAD_CREATE_JOINABLE
	//是否创建了线程
	volatile bool m_bRun;

	//休眠时间
	volatile int m_nSleepSecond;

	//!\brief   唤醒线程信号
	sem_t m_semWake;
#endif

	void Lock()
	{
		pthread_mutex_lock(&m_Mutex);
	}

	void Unlock()
	{
		pthread_mutex_unlock(&m_Mutex);
	}

	void Init()
	{
		int nSDKRet;

		//创建线程互斥锁
		nSDKRet = pthread_mutex_init(&m_Mutex, NULL);
		if (0 != nSDKRet)
		{
			MPU_ERR("pthread_mutex_init( m_Mutex ) return %d\n", nSDKRet);
			MPU_CHK(0 == nSDKRet);
		}

		m_bReset = false;

#ifdef CIU_RESET_THREAD_CREATE_JOINABLE
		m_bRun = true;
		m_nSleepSecond = 0;

		//创建线程唤醒信号
		nSDKRet = sem_init(&m_semWake, 0, 0);
		if (0 != nSDKRet)
		{
			MPU_ERR("sem_init( m_semWake ) return %d\n", nSDKRet);
			MPU_CHK(0 == nSDKRet);
		}

		//创建重启线程
		nSDKRet = pthread_create(&m_thrdID, NULL, reset_hieserver_thread, NULL);
		if (0 != nSDKRet)
		{
			MPU_ERR("pthread_create( reset_hieserver_thread ) JOINABLE return %d\n", nSDKRet);
			MPU_CHK(0 == nSDKRet);
		}
#endif
	}

	void Uninit()
	{
		int nSDKRet;

#ifdef CIU_RESET_THREAD_CREATE_JOINABLE

		//设置标志，停止线程
		Lock();
		m_bRun = false;
		Unlock();

		//激活线程
		sem_post(&m_semWake);

		//等待线程退出
		nSDKRet = pthread_join(m_thrdID, NULL);
		if (0 != nSDKRet)
		{
			MPU_ERR("pthread_join( m_thrdID ) return %d\n", nSDKRet);
			MPU_CHK(0 == nSDKRet);
		}

		//销毁浏览线程同步资源
		nSDKRet = sem_destroy(&m_semWake);
		if (0 != nSDKRet)
		{
			MPU_ERR("sem_destroy( m_semWake ) return %d\n", nSDKRet);
			MPU_CHK(0 == nSDKRet);
		}

#endif

		nSDKRet = pthread_mutex_destroy(&m_Mutex);
		if (0 != nSDKRet)
		{
			MPU_ERR("pthread_mutex_destroy( m_Mutex ) return %d\n", nSDKRet);
			MPU_CHK(0 == nSDKRet);
		}
	}

	int Start(int nSecond)
	{
		int nRet = IDVR_OK;

		MPU_DBG("need reboot HieServer after %d s\n", nSecond);

		Lock();

		if (!m_bReset)
		{
#ifdef CIU_RESET_THREAD_CREATE_JOINABLE

			m_nSleepSecond = nSecond;
			m_bReset = true;

			Unlock();

			MPU_DBG("notify reboot HieServer\n");

			//激活重启线程
			sem_post(&m_semWake);

#else

			int nSDKRet;

			pthread_attr_t attr;

			nSDKRet = pthread_attr_init(&attr);
			if (0 != nSDKRet)
			{
				MPU_ERR("pthread_attr_init() return %d\n", nSDKRet);
				MPU_CHK(0 == nSDKRet);
			}

			//第二个参数可选为分离线程
			nSDKRet = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
			if (0 != nSDKRet)
			{
				MPU_ERR("pthread_attr_setdetachstate( PTHREAD_CREATE_DETACHED ) return %d\n", nSDKRet);
				MPU_CHK(0 == nSDKRet);
			}

			MPU_DBG("create reboot HieServer thread\n");

			nSDKRet = pthread_create(&m_thrdID, &attr, reset_hieserver_thread, (void *) nSecond);
			if (0 == nSDKRet)
			{
				m_bReset = true;
			}
			else
			{
				MPU_ERR("pthread_create( reset_hieserver_thread ) DETACHED return %d\n", nSDKRet);
				MPU_CHK(0 == nSDKRet);

				nRet = -1;
			}

			//清除属性
			nSDKRet = pthread_attr_destroy(&attr);
			if (0 != nSDKRet)
			{
				MPU_ERR("pthread_attr_destroy() return %d\n", nSDKRet);
				MPU_CHK(0 == nSDKRet);
			}

			Unlock();
#endif
		}
		else
		{
			Unlock();

			MPU_DBG("already reboot HieServer\n");
		}

		return nRet;
	}

	void Reset(int nSecond)
	{
		if (nSecond > 0)
		{
			MPU_DBG("delay %d seconds\n", nSecond);
			sleep(nSecond);
		}
MPU_INFO("----------------------------reboot-----------------------------\n");
		#ifdef __SUPPORT_SD_CARD__
		MPU_AVI_StopRecordAndUinitMemRecord(0);
		#endif
		mysystem("reboot");
		#if 0
		//停止自动发现服务
		nSDKRet = HieServer_DeviceProbeStop();
		MPU_CHK(0 == nSDKRet);
		MPU_INFO("HieServer_DeviceProbeStop() return %d\n", nSDKRet);

		//停止远程服务
		MPU_INFO("HieServer_Stop() enter\n");
		nSDKRet = HieServer_Stop();
		MPU_CHK(0 == nSDKRet);
		MPU_INFO("HieServer_Stop() return %d\n", nSDKRet);

		sleep(1);

		//启动远程服务
		MPU_INFO("HieServer_Start() enter\n");
		nSDKRet = HieServer_Start();
		MPU_CHK(0 == nSDKRet);
		MPU_INFO("HieServer_Start() return %d\n", nSDKRet);

		//启动自动发现服务
		nSDKRet = HieServer_DeviceProbeStart();
		MPU_CHK(0 == nSDKRet);
		MPU_INFO("HieServer_DeviceProbeStart() return %d\n", nSDKRet);

		//启动自动注册
		NMS_Reset();
		#endif
	}

	void ThreadProcess(void *param)
	{
#ifdef CIU_RESET_THREAD_CREATE_JOINABLE

		bool bRun;

		do
		{
			//等待唤醒
			MPU_DBG("sleep\n");
			sem_wait(&m_semWake);
			MPU_DBG("wake up\n");

			Lock();				//加锁

			//判断是否需要重启网络服务
			if (m_bRun && m_bReset)
			{
				int nSecond = m_nSleepSecond;

				Unlock();		//解锁

				//重启网络服务
				Reset(nSecond);

				Lock();			//加锁

				m_bReset = false;
			}

			bRun = m_bRun;

			Unlock();			//解锁
		}
		while (bRun);

#else

		int nSecond = (int) param;

		//重启网络服务
		Reset(nSecond);

		Lock();
		m_bReset = false;
		Unlock();

#endif
	}
}

reboot_hieserver_mgt_t;

static reboot_hieserver_mgt_t g_reset;


static void *reset_hieserver_thread(void *param)
{
    PRT_TID();

	BEGINRUN_THREAD;
	g_reset.ThreadProcess(param);
	ENDRUN_THREAD;

	return NULL;
}


/**************************************************************************
 *                                    报警事件异步通知处理                                                 *
 **************************************************************************/

//报警事件异步通知线程
static void *refresh_alarm_thread(void *param);

//如果远程改变了网络参数，则需要异步重启网络服务
typedef struct tag_refresh_alarm_mgt
{
	pthread_t m_thrdID;			//线程ID

	pthread_mutex_t m_mutex;	//互斥锁

	sem_t m_semExit;			//退出信号
	sem_t m_semWake;			//激活信号

	int m_nMaxAlarmInCh;		//最大报警通道数
	int m_nMaxVideoInCh;		//最大视频通道数

	int m_bAlarmIn[DVR_MAX_ALARMIN_NUM];
	int m_dwAlarmInReserved[DVR_MAX_ALARMIN_NUM];

	int m_bMotion[DVR_MAX_VIDEOIN_NUM];
	int m_dwMotionReserved[DVR_MAX_VIDEOIN_NUM];

	int m_bHumanShape[DVR_MAX_VIDEOIN_NUM];
	int m_dwHumanShapeReserved[DVR_MAX_VIDEOIN_NUM];

	int m_bSound[DVR_MAX_VIDEOIN_NUM];
	int m_dwSoundReserved[DVR_MAX_VIDEOIN_NUM];

	int m_bVideoLost[DVR_MAX_VIDEOIN_NUM];
	int m_dwVideoLostReserved[DVR_MAX_VIDEOIN_NUM];

	int m_bEncodeError[DVR_MAX_VIDEOIN_NUM];
	int m_dwEncodeErrorReserved[DVR_MAX_VIDEOIN_NUM];

	int m_bVideoException[DVR_MAX_VIDEOIN_NUM];
	int m_dwVideoExceptionReserved[DVR_MAX_VIDEOIN_NUM];

	//硬盘异常事件
	int m_bDiskError;
	int m_dwDiskErrorReserved;

	//硬盘满事件
	int m_bDiskFull;
	int m_dwDiskFullReserved;

	//IP冲突事件
	int m_bIpConflict;
	int m_dwIpConflictReserved;

	//非法访问事件
	int m_bIllegeAccess;
	int m_dwIllegeAccessReserved;

	//盘组异常事件
	int m_bDiskGroupError;
	int m_dwDiskGroupErrorReserved;

	//Imode
	int m_bImode;
	int m_dwImodeReserved;

	//IP更新
	int m_bIpUpdate;
	int m_bIpUpdateReserved;

	void Init()
	{
		int nSDKRet;

#if SYSTEM_SUPPORT_HD_ALGO_CHANNEL >=0
		//最大视频通道数
		m_nMaxVideoInCh = SYSTEM_SUPPORT_HD_ALGO_CHANNEL;
#elif
		//最大视频通道数
		m_nMaxVideoInCh = MAX_ENC_CHANNEL_NUM;
#endif

		//创建互斥锁
		nSDKRet = pthread_mutex_init(&m_mutex, NULL);
		if (0 != nSDKRet)
		{
			MPU_ERR("pthread_mutex_init( m_mutex  ) return %d\n", nSDKRet);
			MPU_CHK(0 == nSDKRet);
		}

		//创建线程同步资源
		nSDKRet = sem_init(&m_semExit, 0, 0);
		if (0 != nSDKRet)
		{
			MPU_ERR("sem_init( m_semExit ) return %d\n", nSDKRet);
			MPU_CHK(0 == nSDKRet);
		}

		nSDKRet = sem_init(&m_semWake, 0, 0);
		if (0 != nSDKRet)
		{
			MPU_ERR("sem_init( m_semWake ) return %d\n", nSDKRet);
			MPU_CHK(0 == nSDKRet);
		}

		//加锁
		Lock();

		//初始化数据

		//报警输入
		memset(m_bAlarmIn, 0, sizeof (m_bAlarmIn));
		memset(m_dwAlarmInReserved, 0, sizeof (m_dwAlarmInReserved));

		//移动侦测
		memset(m_bMotion, 0, sizeof (m_bMotion));
		memset(m_dwMotionReserved, 0, sizeof (m_dwMotionReserved));

		//人形检测
		memset(m_bHumanShape, 0, sizeof (m_bHumanShape));
		memset(m_dwHumanShapeReserved, 0, sizeof (m_dwHumanShapeReserved));

        //声音侦测
		memset(m_bSound, 0, sizeof (m_bSound));
		memset(m_dwSoundReserved, 0, sizeof (m_dwSoundReserved));

		//视频丢失
		memset(m_bVideoLost, 0, sizeof (m_bVideoLost));
		memset(m_dwVideoLostReserved, 0, sizeof (m_dwVideoLostReserved));

		//编码 错误
		memset(m_bEncodeError, 0, sizeof (m_bEncodeError));
		memset(m_dwEncodeErrorReserved, 0, sizeof (m_dwEncodeErrorReserved));

		//视频异常
		memset(m_bVideoException, 0, sizeof (m_bEncodeError));
		memset(m_dwVideoExceptionReserved, 0, sizeof (m_dwEncodeErrorReserved));

		//硬盘异常事件
		m_bDiskError = FALSE;
		m_dwDiskErrorReserved = 0;

		//硬盘满事件
		m_bDiskFull = FALSE;
		m_dwDiskFullReserved = 0;

		//IP冲突事件
		m_bIpConflict = FALSE;
		m_dwIpConflictReserved = 0;

		//非法访问事件
		m_bIllegeAccess = FALSE;
		m_dwIllegeAccessReserved = 0;

		//盘组异常事件
		m_bDiskGroupError = FALSE;
		m_dwDiskGroupErrorReserved = 0;
		
		//Imode
		 m_bImode = FALSE;
		 m_dwImodeReserved = 0;

		//解锁
		Unlock();

		//创建报警信息异步发送线程
		nSDKRet = pthread_create(&m_thrdID, NULL, refresh_alarm_thread, this);
		if (0 != nSDKRet)
		{
			MPU_ERR("pthread_create( refresh_alarm_thread ) JOINABLE return %d\n", nSDKRet);
			MPU_CHK(0 == nSDKRet);
		}
	}

	void Lock()
	{
		pthread_mutex_lock(&m_mutex);
	}

	void Unlock()
	{
		pthread_mutex_unlock(&m_mutex);
	}

	void Uninit()
	{
		int nSDKRet;

		//激活并退出线程
		sem_post(&m_semExit);
		sem_post(&m_semWake);

		//等待线程退出完毕
		nSDKRet = pthread_join(m_thrdID, NULL);
		if (0 != nSDKRet)
		{
			MPU_ERR("pthread_join( m_thrdID ) return %d\n", nSDKRet);
			MPU_CHK(0 == nSDKRet);
		}

		//销毁线程同步资源
		nSDKRet = sem_destroy(&m_semExit);
		if (0 != nSDKRet)
		{
			MPU_ERR("sem_destroy( m_semExit ) return %d\n", nSDKRet);
			MPU_CHK(0 == nSDKRet);
		}

		nSDKRet = sem_destroy(&m_semWake);
		if (0 != nSDKRet)
		{
			MPU_ERR("sem_destroy( m_semWake ) return %d\n", nSDKRet);
			MPU_CHK(0 == nSDKRet);
		}

		//销毁互斥锁
		nSDKRet = pthread_mutex_destroy(&m_mutex);
		if (0 != nSDKRet)
		{
			MPU_ERR("pthread_mutex_destroy( m_mutex ) return %d\n", nSDKRet);
			MPU_CHK(0 == nSDKRet);
		}
	}

	//记录信息到本地
	int Notify(struct tagAlertInfo *pAlertInfo)
	{
		int nRet = IDVR_OK;

		if (NULL == pAlertInfo)
		{
			MPU_ERR("NULL == pAlertInfo\n");
			return IDVR_ERROR_PARAMETER_INVALID_POINTER;
		}

		Lock();					//加锁

		switch (pAlertInfo->AlertType)
		{
			case ALERT_TYPE_ALARMIN:
				{
					//报警输入事件
					if (pAlertInfo->nChannel >= 0 && pAlertInfo->nChannel < DVR_MAX_ALARMIN_NUM)
					{
						m_bAlarmIn[pAlertInfo->nChannel] = TRUE;
						m_dwAlarmInReserved[pAlertInfo->nChannel] = pAlertInfo->dwReserved;
					}
					else
					{
						MPU_ERR("Alarm In Channel Number = %d is overflow\n", pAlertInfo->nChannel);
						nRet = IDVR_ERROR_CHANNELNO;
					}
				}
				break;

			case ALERT_TYPE_MOTION:
				{
					//移动侦测事件
					if (pAlertInfo->nChannel >= 0 && pAlertInfo->nChannel < DVR_MAX_VIDEOIN_NUM)
					{
						m_bMotion[pAlertInfo->nChannel] = TRUE;
						m_dwMotionReserved[pAlertInfo->nChannel] = pAlertInfo->dwReserved;
					}
					else
					{
						MPU_ERR("Motion Channel Number = %d is overflow\n", pAlertInfo->nChannel);
						nRet = IDVR_ERROR_CHANNELNO;
					}
				}
				break;

			case ALERT_TYPE_HUMANSHAPE:
				{
					//移动侦测事件
					if (pAlertInfo->nChannel >= 0 && pAlertInfo->nChannel < DVR_MAX_VIDEOIN_NUM)
					{
						m_bHumanShape[pAlertInfo->nChannel] = TRUE;
						m_dwHumanShapeReserved[pAlertInfo->nChannel] = pAlertInfo->dwReserved;
					}
					else
					{
						MPU_ERR("Human Shape Channel Number = %d is overflow\n", pAlertInfo->nChannel);
						nRet = IDVR_ERROR_CHANNELNO;
					}
				}
				break;

			case ALERT_TYPE_SOUNDDETECT:
				{
					//声音侦测事件
					if (pAlertInfo->nChannel >= 0 && pAlertInfo->nChannel < DVR_MAX_VIDEOIN_NUM)
					{
						m_bSound[pAlertInfo->nChannel] = TRUE;
						m_dwSoundReserved[pAlertInfo->nChannel] = pAlertInfo->dwReserved;
					}
					else
					{
						MPU_ERR("Sound Channel Number = %d is overflow\n", pAlertInfo->nChannel);
						nRet = IDVR_ERROR_CHANNELNO;
					}
				}
				break;

			case ALERT_TYPE_VIDEOLOST:
				{
					//视频丢失事件
					if (pAlertInfo->nChannel >= 0 && pAlertInfo->nChannel < DVR_MAX_VIDEOIN_NUM)
					{
						m_bVideoLost[pAlertInfo->nChannel] = TRUE;
						m_dwVideoLostReserved[pAlertInfo->nChannel] = pAlertInfo->dwReserved;
					}
					else
					{
						MPU_ERR("Video Lost Channel Number = %d is overflow\n", pAlertInfo->nChannel);
						nRet = IDVR_ERROR_CHANNELNO;
					}
				}
				break;

			case ALERT_TYPE_ENCODEERROR:
				{
					//编码错误事件
					if (pAlertInfo->nChannel >= 0 && pAlertInfo->nChannel < DVR_MAX_VIDEOIN_NUM)
					{
						m_bEncodeError[pAlertInfo->nChannel] = TRUE;
						m_dwEncodeErrorReserved[pAlertInfo->nChannel] = pAlertInfo->dwReserved;
					}
					else
					{
						MPU_ERR("Encode Error Channel Number = %d is overflow\n", pAlertInfo->nChannel);
						nRet = IDVR_ERROR_CHANNELNO;
					}
				}
				break;

			case ALERT_TYPE_VIDEOEXCEPTION:
				{
					//视频异常事件
					if (pAlertInfo->nChannel >= 0 && pAlertInfo->nChannel < DVR_MAX_VIDEOIN_NUM)
					{
						m_bVideoException[pAlertInfo->nChannel] = TRUE;
						m_dwVideoExceptionReserved[pAlertInfo->nChannel] = pAlertInfo->dwReserved;
					}
					else
					{
						MPU_ERR("Video Exception Channel Number = %d is overflow\n", pAlertInfo->nChannel);
						nRet = IDVR_ERROR_CHANNELNO;
					}
				}
				break;

			case ALERT_TYPE_DISKERROR:
				{
					//硬盘异常事件
					m_bDiskError = TRUE;
					m_dwDiskErrorReserved = pAlertInfo->dwReserved;
				}
				break;

			case ALERT_TYPE_DISKFULL:
				{
					//硬盘满事件
					m_bDiskFull = TRUE;
					m_dwDiskFullReserved = pAlertInfo->dwReserved;
				}
				break;

			case ALERT_TYPE_IPCONFLICT:
				{
					//IP冲突事件
					m_bIpConflict = TRUE;
					m_dwIpConflictReserved = pAlertInfo->dwReserved;
				}
				break;

			case ALERT_TYPE_ILLEGEACCESS:
				{
					//非法访问事件
					m_bIllegeAccess = TRUE;
					m_dwIllegeAccessReserved = pAlertInfo->dwReserved;
				}
				break;

			case ALERT_TYPE_DISKGROUP_ERROR:
				{
					//盘组异常事件
					m_bDiskGroupError = TRUE;
					m_dwDiskGroupErrorReserved = pAlertInfo->dwReserved;
				}
				break;

			case ALERT_TYPE_IMODE:
				{
					m_bImode = TRUE;
					m_dwImodeReserved |= pAlertInfo->dwReserved;
				}
				break;
			case ALERT_TYPE_IPCHANGED:
				{
					m_bIpUpdate = TRUE;
					m_bIpUpdateReserved = pAlertInfo->dwReserved;
				}
				break;

			case ALERT_TYPE_RETICLEDISCONNECT:
			default:
				{
					//网线断不需要通知

					MPU_ERR("unsupported AlertType = %d\n", pAlertInfo->AlertType);
					nRet = IDVR_ERROR_PARAMETER_OVERFLOW;
				}
				break;
		};

		Unlock();				//解锁

		if (IDVR_OK == nRet)
		{
			int sem_value;

			//激活异步线程处理
			sem_getvalue(&m_semWake, &sem_value);
			if (sem_value == 0)
			{
				//MPU_DBG("refresh_alarm_thread  need wake up\n");
				sem_post(&m_semWake);
			}
		}

		return nRet;
	}
	
typedef enum eEventType
{
	UNKNOW_EVENT		= 0,
	START_EVENT 		= 2,
	CONTINUOUS_EVENT	= 3,
	END_EVENT			= 4,
	EVENT_MAX			= 5,
} eEventType;

	//发送报警通知到网络
	void Send(int nAlertType, int nChannel, unsigned long dwReserved)
	{
		Unlock();				//解锁

		/*
		推送事件逻辑：若有连续的智能事件触发，每10秒发送一次事件到网络层
		推送图片逻辑：若有连续的智能事件触发，中途需要连续10秒以上不再触发事件后，下次才会触发推图
		*/
		
		if (ALERT_TYPE_HUMANSHAPE == nAlertType)
		{
			//兼容新旧接口
			HieServer_RefreshEvent(USEREVENT_ALARM_NOTICE, ALERT_HUMANSHAPE, nChannel, dwReserved);
#if 0
			PEOPLE_FRAME_HEAD *pFrameHead = (PEOPLE_FRAME_HEAD *)pBuff;
			MPU_DBG("u32AIType=%#x u32Size=%d u32Time_sec=%d u32Time_usec=%d u32BgPicSize=%d u32BgPicOffset=%d sizeof=%d\n", 
				pFrameHead->u32AIType,
				pFrameHead->u32Size,
				pFrameHead->u32Time_sec,
				pFrameHead->u32Time_usec,
				pFrameHead->u32BgPicSize,
				pFrameHead->u32BgPicOffset,
				sizeof(PEOPLE_FRAME_HEAD));
			for (int i = 0; i < pFrameHead->u32Num; i++)
			{
				MPU_DBG("rect[%d]=[%d, %d, %d, %d]\n", 
					i, pFrameHead->rcPeople[i].x,
					pFrameHead->rcPeople[i].y,
					pFrameHead->rcPeople[i].w,
					pFrameHead->rcPeople[i].h);
			}
#endif
			int myRet = HieServer_RefreshEventEx(USEREVENT_AI_EVENT_PUSH, EVENT_AI_PEOPLE_DETECT, ALERT_HUMANSHAPE, nChannel, dwReserved, NULL, 0);
			MPU_DBG("HieServer_RefreshEventEx myRet=%#x\n", myRet);
		}
		else if (ALERT_TYPE_MOTION == nAlertType)
		{
			static eEventType lastEvent = UNKNOW_EVENT;
			time_t curTime = 0;					//当前时间
			static time_t lastSendEventTime = 0;//上次发送事件的时间	

			curTime = time(NULL);

			if (lastSendEventTime > curTime)//时间反转需要把上次时间进行赋值
			{
				lastSendEventTime = curTime;
			}
			
			//开始事件
			if((UNKNOW_EVENT == lastEvent) || (END_EVENT == lastEvent) || 
				(dwReserved && (curTime - lastSendEventTime > (SEND_END_EVNET_TIME_INTERNAL + 3))))
			{
				//兼容新旧接口
				HieServer_RefreshEvent(USEREVENT_ALARM_NOTICE, ALERT_MOTION, nChannel, dwReserved);

				HieServer_RefreshEventEx(USEREVENT_ALARM_NOTICE, 0, ALERT_MOTION, nChannel, START_EVENT, NULL, 0);
				
				lastEvent = START_EVENT;
				lastSendEventTime = time(NULL);

				MPU_DBG("ALERT_TYPE_MOTION START_EVENT dwReserved=%#x\n", dwReserved);
			}
			else if ((START_EVENT == lastEvent) || (CONTINUOUS_EVENT == lastEvent))
			{
				if(0 == dwReserved) //结束事件，由功能模块触发
				{
					HieServer_RefreshEventEx(USEREVENT_ALARM_NOTICE, 0, 
						nAlertType, nChannel, END_EVENT, NULL, 0);
					
					lastEvent = END_EVENT;
					lastSendEventTime = time(NULL);

					MPU_DBG("ALERT_TYPE_MOTION END_EVENT dwReserved=%#x\n", dwReserved);
				}
				else if (curTime - lastSendEventTime >= SEND_MOTION_CONTINUOUS_EVNET_TIME_INTERNAL)
				{
					//兼容新旧接口，老的发送事件接口是一直发送
					HieServer_RefreshEvent(USEREVENT_ALARM_NOTICE, ALERT_MOTION, nChannel, dwReserved);

					HieServer_RefreshEventEx(USEREVENT_ALARM_NOTICE, 0, ALERT_MOTION, nChannel, CONTINUOUS_EVENT, NULL, 0);	

					lastEvent = CONTINUOUS_EVENT;
					lastSendEventTime = time(NULL);

					MPU_DBG("ALERT_TYPE_MOTION CONTINUOUS_EVENT dwReserved=%#x\n", dwReserved);
				}
			}
		}
		else
		{
            MPU_ERR("######### HieServer_RefreshEvent nAlertType=%d not support\n", nAlertType);
			//兼容新旧接口
			/*HieServer_RefreshEvent(USEREVENT_ALARM_NOTICE, nAlertType, nChannel, dwReserved);

			HieServer_RefreshEventEx(USEREVENT_ALARM_NOTICE, 0, 
				nAlertType, nChannel, dwReserved, NULL, 0);*/
		}

		//发送完毕，休眠10ms
		usleep(10000);

		Lock();					//加锁
	}

	//检测所有报警信息，并发送通知
	int Refresh()
	{
		int bContinue = FALSE;
		int nCh;

		//报警输入事件
		for (nCh = 0; nCh < m_nMaxAlarmInCh; ++nCh)
		{
			if (m_bAlarmIn[nCh])
			{
				Send(ALERT_TYPE_ALARMIN, nCh, m_dwAlarmInReserved[nCh]);

				m_bAlarmIn[nCh] = FALSE;
				m_dwAlarmInReserved[nCh] = 0;

				bContinue = TRUE;
			}
		}

		//移动侦测事件
		for (nCh = 0; nCh < m_nMaxVideoInCh; ++nCh)
		{
			if (m_bMotion[nCh])
			{
				Send(ALERT_TYPE_MOTION, nCh, m_dwMotionReserved[nCh]);

				m_bMotion[nCh] = FALSE;
				m_dwMotionReserved[nCh] = 0;

				bContinue = TRUE;
			}
		}
		
		//人形检测事件
		for (nCh = 0; nCh < m_nMaxVideoInCh; ++nCh)
		{
			if (m_bHumanShape[nCh])
			{
				Send(ALERT_TYPE_HUMANSHAPE, nCh, m_dwHumanShapeReserved[nCh]);

				m_bHumanShape[nCh] = FALSE;
				m_dwHumanShapeReserved[nCh] = 0;

				bContinue = TRUE;
			}
		}

		//声音侦测事件
		for (nCh = 0; nCh < m_nMaxVideoInCh; ++nCh)
		{
			if (m_bSound[nCh])
			{
				Send(ALERT_TYPE_SOUNDDETECT, nCh, m_dwSoundReserved[nCh]);

				m_bSound[nCh] = FALSE;
				m_dwSoundReserved[nCh] = 0;

				bContinue = TRUE;
			}
		}

		//视频丢失事件
		for (nCh = 0; nCh < m_nMaxVideoInCh; ++nCh)
		{
			if (m_bVideoLost[nCh])
			{
				Send(ALERT_TYPE_VIDEOLOST, nCh, m_dwVideoLostReserved[nCh]);

				m_bVideoLost[nCh] = FALSE;
				m_dwVideoLostReserved[nCh] = 0;

				bContinue = TRUE;
			}
		}

		//编码错误事件
		for (nCh = 0; nCh < m_nMaxVideoInCh; ++nCh)
		{
			if (m_bEncodeError[nCh])
			{
				Send(ALERT_TYPE_ENCODEERROR, nCh, m_dwEncodeErrorReserved[nCh]);

				m_bEncodeError[nCh] = FALSE;
				m_dwEncodeErrorReserved[nCh] = 0;

				bContinue = TRUE;
			}
		}

		//视频异常事件
		for (nCh = 0; nCh < m_nMaxVideoInCh; ++nCh)
		{
			if (m_bVideoException[nCh])
			{
				Send(ALERT_TYPE_VIDEOEXCEPTION, nCh, m_dwVideoExceptionReserved[nCh]);

				m_bVideoException[nCh] = FALSE;
				m_dwVideoExceptionReserved[nCh] = 0;

				bContinue = TRUE;
			}
		}

		//硬盘异常事件
		if (m_bDiskError)
		{
			Send(ALERT_TYPE_DISKERROR, 0, m_dwDiskErrorReserved);

			m_bDiskError = FALSE;
			m_dwDiskErrorReserved = 0;

			bContinue = TRUE;
		}

		//硬盘满事件
		if (m_bDiskFull)
		{
			Send(ALERT_TYPE_DISKFULL, 0, m_dwDiskFullReserved);

			m_bDiskFull = FALSE;
			m_dwDiskFullReserved = 0;

			bContinue = TRUE;
		}

		//IP冲突事件
		if (m_bIpConflict)
		{
			Send(ALERT_TYPE_IPCONFLICT, 0, m_dwIpConflictReserved);

			m_bIpConflict = FALSE;
			m_dwIpConflictReserved = 0;

			bContinue = TRUE;
		}

		//非法访问事件
		if (m_bIllegeAccess)
		{
			Send(ALERT_TYPE_ILLEGEACCESS, 0, m_dwIllegeAccessReserved);

			m_bIllegeAccess = FALSE;
			m_dwIllegeAccessReserved = 0;

			bContinue = TRUE;
		}

		//盘组异常事件
		if (m_bDiskGroupError)
		{
			Send(ALERT_TYPE_DISKGROUP_ERROR, 0, m_dwDiskGroupErrorReserved);

			m_bDiskGroupError = FALSE;
			m_dwDiskGroupErrorReserved = 0;

			bContinue = TRUE;
		}

		//imode
		if (m_bImode)
		{
			Send(ALERT_TYPE_IMODE, 0, m_dwImodeReserved);

			m_bImode = FALSE;
			m_dwImodeReserved = 0;

			bContinue = TRUE;
		}

		//ipUpdate
		if (m_bIpUpdate)
		{
			Send(ALERT_TYPE_IPCHANGED, 0, m_bIpUpdateReserved);

			m_bIpUpdate = FALSE;
			m_bIpUpdateReserved = 0;

			bContinue = TRUE;
		}

		return bContinue;
	}

	void ThreadProcess()
	{
		while (0 != sem_trywait(&m_semExit))
		{
			//等待唤醒
			//MPU_DBG("refresh_alarm_thread  is asleep\n");
			sem_wait(&m_semWake);
			//MPU_DBG("refresh_alarm_thread  is wake up\n");

			Lock();				//加锁

			//检测所有报警，通知网络库
			while (Refresh())
			{
				//发送成功，则继续下一轮
			}


			Unlock();			//解锁
		}
	}

}

refresh_alarm_mgt_t;

static refresh_alarm_mgt_t g_alarm;


static void *refresh_alarm_thread(void *param)
{
    PRT_TID();

	BEGINRUN_THREAD;
	g_alarm.ThreadProcess();
	ENDRUN_THREAD;

    return NULL;
}

#ifdef __ONE_CLICK_CALL__
static void *check_callstatus_thread(void *param);

typedef enum _KEY_TO_CALL_STATUS {
    IDLE_CALL_STATUS,   //空闲
    START_CALL_STATUS,  //发起通话
    WAIT_CALL_STATUS,   //等待接听
    BUSY_CALL_STATUS,   //通话中
    END_CALL_STATUS,    //结束通话
    FACI_END_CALL_STATUS, //设备端挂断
} KEY_TO_CALL_STATUS;

typedef struct tag_check_callevent_mgt
{
    pthread_t m_thrdID;			//线程ID
    sem_t m_semExit;			//退出信号
    char PicBuffer[MAX_PICTURE_BUF_SIZE] = {0};

    KEY_TO_CALL_STATUS call_status = IDLE_CALL_STATUS;  //一键呼叫通话状态
    pthread_mutex_t m_mutex;	                        //互斥锁
    int recv_call_state = 0;                            //外部动作（0：未收到动作 1：接听 2：挂断 3：直播流开启 4:直播流关闭）
    int is_offline = 0;                                 //设备不在线
    int is_noanswer = 0;                                //无人接听
    DWORD pdwConnectionId = 0;                          //记录一键呼叫开流的ID
    int RecordID =0;                                    //记录一次标志位
	int m_bSupportKeyToCall =0;							//是否支持一键呼叫

	void Init()
    {
        int s32Ret = 0;
        //创建互斥锁
        s32Ret = XTRACE_MUTEX_INIT(&m_mutex, NULL);
        if (0 != s32Ret)
        {
            MPU_ERR("pthread_mutex_init( m_mutex  ) return %d\n", s32Ret);
            MPU_CHK(0 == s32Ret);
        }

        //创建线程同步资源
        s32Ret = sem_init(&m_semExit, 0, 0);
        if (0 != s32Ret)
        {
            MPU_ERR("sem_init( m_semExit ) return %d\n", s32Ret);
            MPU_CHK(0 == s32Ret);
        }

        //创建一键呼叫检测发送线程
        s32Ret = pthread_create(&m_thrdID, NULL, check_callstatus_thread, this);
        if (0 != s32Ret) {
            MPU_ERR("pthread_create( check_callstatus_thread ) JOINABLE return %d\n", s32Ret);
            MPU_CHK(0 == s32Ret);
        }
    }

    void Uninit()
    {
        int s32Ret = 0;

        //激活并退出线程
        sem_post(&m_semExit);

        //等待线程退出完毕
        pthread_cancel(m_thrdID);
        s32Ret = pthread_join(m_thrdID, NULL);
        if (0 != s32Ret)
        {
            MPU_ERR("pthread_join( m_thrdID ) return %d\n", s32Ret);
            MPU_CHK(0 == s32Ret);
        }

        //销毁线程同步资源
        s32Ret = sem_destroy(&m_semExit);
        if (0 != s32Ret)
        {
            MPU_ERR("sem_destroy( m_semExit ) return %d\n", s32Ret);
            MPU_CHK(0 == s32Ret);
        }
        
        //销毁互斥锁
        s32Ret = XTRACE_MUTEX_DESTORY(&m_mutex);
        if (0 != s32Ret)
        {
            MPU_ERR("pthread_mutex_destroy( m_mutex ) return %d\n", s32Ret);
            MPU_CHK(0 == s32Ret);
        }
    }

    void SetRecvCallState(int state)
    {
        pthread_mutex_lock(&m_mutex);
        recv_call_state = state;
        pthread_mutex_unlock(&m_mutex);
    }

    void GetRecvCallState(int *state)
    {
        pthread_mutex_lock(&m_mutex);
        *state = recv_call_state;
        recv_call_state = 0;
        pthread_mutex_unlock(&m_mutex);
    }

    void GetCallConnectionId(DWORD ConnectionId)
    {
#if !defined (__IS_SUPPORT_LCD_MANAGER__)
		pthread_mutex_lock(&m_mutex);
		if((BUSY_CALL_STATUS == call_status)&&(0 == RecordID))
		{
			recv_call_state=3;
			pdwConnectionId = ConnectionId;
			RecordID=1;
		}
		pthread_mutex_unlock(&m_mutex);
#endif
    }

    void GetHangupConnectionId(DWORD ConnectionId)
    {
#if !defined (__IS_SUPPORT_LCD_MANAGER__)
		pthread_mutex_lock(&m_mutex);
		if(pdwConnectionId == ConnectionId)
		{
			recv_call_state = 4;
			pdwConnectionId = 0;
			RecordID=0;
		}
        pthread_mutex_unlock(&m_mutex);
#endif
    }

    void StartCall()
    {
        int fd = -1;
        int counter = 0;
        int nTimeZone = 0;
        int wifi_gprs_status = 0;
        int  UiEnableAI = 0;
        int  UiFC = 0;
        int  UiOP = 0;
        int  UiAI = 0;
        unsigned int nFileSize = 0;
        char szTmp[256] = {0};
        char szFileName[64] = {0};
        EventKey2Call CallState = {0};
        struct timeval now_tv = {0, 0};
        ZWP2P_PUSH_EVENT_INFO struEventInfo = {0};

        CConfig *pConfig = NULL;
        pConfig = CConfig::GetInstance();

        CConfig *pCfg = CConfig::NewInstance();
        if (pCfg)
        {
            if(0 == pCfg->LoadFileRO(CUSTOM_INFO))
            {
                UiEnableAI = pCfg->GetValue("PushAI", "UiEnableAI", 0);
                UiFC = pCfg->GetValue("PushAI", "UiFC", 0);
                UiOP = pCfg->GetValue("PushAI", "UiOP", 0);
                UiAI = pCfg->GetValue("PushAI", "UiAI", 0);
            }
            pCfg->ReleaseInstance(pCfg); 
            pCfg = NULL;
        }
#if defined (__IS_SUPPORT_WLAN__)
        int net_status = 0;
        net_status = MPU_NET_GetStatus();
        wifi_gprs_status = MPU_WLAN_GetStatus();
#elif defined (__MODULE_4G_SUPPORT__)
        MPU_Net_CheckIfGPRSAccessToInernet(&wifi_gprs_status);
#endif
        // 设备没联网，播放设备不在线提示音
#if defined (__IS_SUPPORT_WLAN__)
		if ((0 != wifi_gprs_status) && (net_status != 0))
#elif defined (__MODULE_4G_SUPPORT__)
		if (1 != wifi_gprs_status)
#endif
        {
            ZWSys_SetSpeechContent(SPEECH_KEY2CALL_OFFLINE);
            while(1)
            {
                sleep(1);
                if (eSpeechPlayIdle == ZWSys_GetSpeechPlayingStatus())
                {
                    break;
                }
                if (++counter > SPEECH_PLAY_TIME_OUT_SEC)
                {
                    counter = 0;
                    break;
                }
            }
            is_offline = 1;
            return;
        }
        MPU_LCD_SetCallStatus(0);   //LCD显示发起通话
        ZWSys_SetPauseAudioAlarm(); //暂停声光报警
        ZWSys_SetSpeechContent(SPEECH_KEY2CALL_START); //发起呼叫
        while(1)
        {
            sleep(1);
            if (eSpeechPlayIdle == ZWSys_GetSpeechPlayingStatus())
            {
                break;
            }
            if (++counter > SPEECH_PLAY_TIME_OUT_SEC)
            {
                counter = 0;
                break;
            }
        }

        //处理上报
        memset(&now_tv, 0, sizeof(now_tv));
        gettimeofday(&now_tv,NULL);// 获取时间
        nTimeZone = MPU_SYS_GetTimeZone(); // 获取时区

        //私有
        CallState.dwID++;
        CallState.dwTimeSec = now_tv.tv_sec;
        CallState.dwTimeUSec = now_tv.tv_sec*1000000 + now_tv.tv_usec;
        HieServer_RefreshEventEx(USEREVENT_AI_EVENT_PUSH, EVENT_AI_DEVICE_KEY2CALL, 0, 0, 0, (char *)(&CallState), sizeof(EventKey2Call));

        //尚云
        memset(&struEventInfo, 0, sizeof(struEventInfo));
        if (NULL != pConfig)
        {
            pConfig->GetValue("iDVR_Alias", "NameUTF8","IPC", struEventInfo.cDeviceName, sizeof(struEventInfo.cDeviceName));
            pConfig->GetValue("ChannelAlias_0", "NameUTF8", "IPC", struEventInfo.cChannelName, sizeof(struEventInfo.cChannelName));
        }

        /* 触发抓图 */
        MPU_STR_TrigEventCapturePic(3, now_tv.tv_sec, now_tv.tv_usec);

        /* 获取抓图的文件名 */
        MPU_STR_SnapShotM_GetLastPic(0, 3, szFileName); 

        memset(szTmp, 0, sizeof(szTmp));
        snprintf(szTmp, sizeof(szTmp), "/tmp/picture/pic_for_push/%s", szFileName);

        /* 读取图片文件到缓冲区 */
        fd = open(szTmp, O_RDONLY);
        if (-1 == fd)
        {
            MPU_ERR("Fail to open %s\n", szTmp);
        }
        else
        {
            nFileSize = read(fd, this->PicBuffer, sizeof(this->PicBuffer));
            close(fd);
            fd = -1;
        }
        /* 不管成功与否，都要删除文件 */
        unlink(szTmp);

        /* 如果读取的图片有效，则填充struEventInfo结构体 */
        if (nFileSize > 0)
        {
            struEventInfo.uiResourceType = 1;
            struEventInfo.pMediaData = this->PicBuffer;
            struEventInfo.nMediaDataLen = nFileSize;
        }

        ISO8601TimeGet(now_tv.tv_sec, nTimeZone, struEventInfo.cEventCreateTime, sizeof(struEventInfo.cEventCreateTime));

        struEventInfo.uiEnableAI = UiEnableAI;
        struEventInfo.uiFC = UiFC;
        struEventInfo.uiOP = UiOP;
        struEventInfo.uiAI = UiAI;
        strncpy(struEventInfo.cPushTitle, "Key2Call Alert", sizeof(struEventInfo.cPushTitle)-1);
        MPU_Net_ZwP2pPushEvent(eDeviceEventTypeKeyToCall, 1, &struEventInfo);

        MPU_WARN("[Key_to_Call] [%s]\n", __FUNCTION__);
    }

    void WaitAnswer()
    {
        int status = 0;
        int counter = 0;
        static int timecnt = 0;

        timecnt += 100; //外层循环为100ms

        ZWSys_GetKey2callDuSpeechStatus(&status);
        if (status == 1) //嘟嘟提示音播放中
        {
            if (timecnt >= 30*1000) //30S超时未接听
            {            
                //停止播放嘟嘟提示音
                ZWSys_SetKey2callhanupState(2);
                while (1)
                {
                    sleep(1);
                    ZWSys_GetKey2callDuSpeechStatus(&status);
                    if (0 == status)
                    {
                        timecnt = 0;
                        break;
                    }
                }

                //播放暂时无人接听语音
                ZWSys_SetSpeechContent(SPEECH_KEY2CALL_NO_ANSWER);
                while (1)
                {
                    sleep(1);
                    if (eSpeechPlayIdle == ZWSys_GetSpeechPlayingStatus())
                    {
                        break;
                    }
                    if (++counter > SPEECH_PLAY_TIME_OUT_SEC)
                    {
                        counter = 0;
                        break;
                    }
                }
                //设置无人接听标记，方便切换通话状态为结束
                is_noanswer = 1;
            }
            return;
        }
        else if (status == 0) // 没有播放嘟嘟提示音
        {
            //等待开始播放嘟嘟提示音退出
            ZWSys_SetSpeechContent(SPEECH_KEY2CALL_DU);
            while (1)
            {
                sleep(1);
                ZWSys_GetKey2callDuSpeechStatus(&status);
                if (1 == status)
                {
                    timecnt = 0;
                    break;
                }
                if (++counter > SPEECH_PLAY_TIME_OUT_SEC)
                {
                    counter = 0;
                    timecnt = 0;
                    break;
                }
            }
        }
        MPU_WARN("[Key_to_Call] [%s] du_status[%d]\n", __FUNCTION__, status);
    }

	void StopCall()
	{
		ZWSys_SetRecoveryAudioAlarm(); //恢复声光报警
		MPU_WARN("[Key_to_Call] [%s]\n", __FUNCTION__);
	}

	void StopCall_Device()
	{
		// 推送挂断消息
		int fd = -1;
			int nTimeZone = 0;
		int wifi_gprs_status = 0;
		int  UiEnableAI = 0;
		int  UiFC = 0;
		int  UiOP = 0;
		int  UiAI = 0;
		unsigned int nFileSize = 0;
		char szTmp[256] = {0};
		char szFileName[64] = {0};
		EventKey2Call CallState = {0};
		struct timeval now_tv = {0, 0};
		ZWP2P_PUSH_EVENT_INFO struEventInfo = {0};

		CConfig *pConfig = NULL;
		pConfig = CConfig::GetInstance();

		CConfig *pCfg = CConfig::NewInstance();
		if (pCfg)
		{
			if(0 == pCfg->LoadFileRO(CUSTOM_INFO))
			{
				UiEnableAI = pCfg->GetValue("PushAI", "UiEnableAI", 0);
				UiFC = pCfg->GetValue("PushAI", "UiFC", 0);
				UiOP = pCfg->GetValue("PushAI", "UiOP", 0);
				UiAI = pCfg->GetValue("PushAI", "UiAI", 0);
			}
			pCfg->ReleaseInstance(pCfg); 
			pCfg = NULL;
		}
#if defined (__IS_SUPPORT_WLAN__)
		int net_status = 0;
		net_status = MPU_NET_GetStatus();
		wifi_gprs_status = MPU_WLAN_GetStatus();
#elif defined (__MODULE_4G_SUPPORT__)
		MPU_Net_CheckIfGPRSAccessToInernet(&wifi_gprs_status);
#endif

#if defined (__IS_SUPPORT_WLAN__)
		// wifi配网不成功或者4g没联网或设备已休眠，播放设备不在线提示音
		if (((0 != wifi_gprs_status) && (net_status != 0)) || (!MPU_SYS_AwakeSchedule_GetDevSleepStatus())) 
#elif defined (__MODULE_4G_SUPPORT__)
		if ((1 != wifi_gprs_status) || (!MPU_SYS_AwakeSchedule_GetDevSleepStatus())) 
#endif
		{
			//恢复声光报警
			ZWSys_SetRecoveryAudioAlarm();
			MPU_WARN("[Key_to_Call] [%s]\n", __FUNCTION__);

			return;
		}

		//处理上报
		memset(&now_tv, 0, sizeof(now_tv));
		gettimeofday(&now_tv,NULL);// 获取时间
		nTimeZone = MPU_SYS_GetTimeZone(); // 获取时区

		//私有
		CallState.dwID++;
		CallState.dwTimeSec = now_tv.tv_sec;
		CallState.dwTimeUSec = now_tv.tv_sec*1000000 + now_tv.tv_usec;
		HieServer_RefreshEventEx(USEREVENT_AI_EVENT_PUSH, EVENT_AI_STOP_KEY2CALL, 0, 0, 0, (char *)(&CallState), sizeof(EventKey2Call));

		//尚云
		memset(&struEventInfo, 0, sizeof(struEventInfo));
		if (NULL != pConfig)
		{
			pConfig->GetValue("iDVR_Alias", "NameUTF8","IPC", struEventInfo.cDeviceName, sizeof(struEventInfo.cDeviceName));
			pConfig->GetValue("ChannelAlias_0", "NameUTF8", "IPC", struEventInfo.cChannelName, sizeof(struEventInfo.cChannelName));
		}

		/* 触发抓图 */
		MPU_STR_TrigEventCapturePic(3, now_tv.tv_sec, now_tv.tv_usec);

		/* 获取抓图的文件名 */
		MPU_STR_SnapShotM_GetLastPic(0, 3, szFileName); 

		memset(szTmp, 0, sizeof(szTmp));
		snprintf(szTmp, sizeof(szTmp), "/tmp/picture/pic_for_push/%s", szFileName);

		/* 读取图片文件到缓冲区 */
		fd = open(szTmp, O_RDONLY);
		if (-1 == fd)
		{
			MPU_ERR("Fail to open %s\n", szTmp);
		}
		else
		{
			nFileSize = read(fd, this->PicBuffer, sizeof(this->PicBuffer));
			close(fd);
			fd = -1;
		}
		/* 不管成功与否，都要删除文件 */
		unlink(szTmp);

		/* 如果读取的图片有效，则填充struEventInfo结构体 */
		if (nFileSize > 0)
		{
			struEventInfo.uiResourceType = 1;
			struEventInfo.pMediaData = this->PicBuffer;
			struEventInfo.nMediaDataLen = nFileSize;
		}

		ISO8601TimeGet(now_tv.tv_sec, nTimeZone, struEventInfo.cEventCreateTime, sizeof(struEventInfo.cEventCreateTime));

		struEventInfo.uiEnableAI = UiEnableAI;
		struEventInfo.uiFC = UiFC;
		struEventInfo.uiOP = UiOP;
		struEventInfo.uiAI = UiAI;
		strncpy(struEventInfo.cPushTitle, "HangUp Key2Call Alert", sizeof(struEventInfo.cPushTitle)-1);
		MPU_Net_ZwP2pPushEvent(eDeviceCustomEventTypeStopKey2Call, 1, &struEventInfo);

		//恢复声光报警
		ZWSys_SetRecoveryAudioAlarm();
		MPU_WARN("[Key_to_Call] [%s]\n", __FUNCTION__);
	}

    void StopCall_Before()
    {
        int status = 0;
        ZWSys_GetKey2callDuSpeechStatus(&status);
        if (status != 0)
        {
            ZWSys_SetKey2callhanupState(1);
            sleep(1);
            MPU_WARN("[Key_to_Call] [%s]  du status:%d   Stop Call In Before \n", __FUNCTION__, status);
        }
    }

#if defined (__IS_SUPPORT_LCD_MANAGER__)
    /* 支持视频通话，当前用LCD宏定义区分，例如P3MU设备 */
    void RefreshCallStatus()
    {
        int keyCallStatus = 0;      //呼叫按键状态
        int keyHangupStatus = 0;    //挂断按键状态
        int ActExternCallState = 0; //主动呼叫--外部动作
        int PasExternCallState = 0; //被动通话--外部动作
        KEY_TO_CALL_STATUS CallStatus = call_status;

        GetRecvCallState(&ActExternCallState);
        GetExternPreviewCallState(&PasExternCallState);
        ZWSys_GetCallPinStatus(&keyCallStatus, &keyHangupStatus);

        if (keyCallStatus || keyHangupStatus) {
            //按键唤醒LCD显示屏幕
            if (MPU_LCD_WakeUp() == 1) {
                keyCallStatus = 0;
                keyHangupStatus = 0;
            }
#if defined (__IS_SUPPORT_REMINDER_SCHEDULE__)
            //按键打断定时提醒
            if (MPU_SYS_RemindSchedule_InterruptReminder() == 1) {
                keyCallStatus = 0;
                keyHangupStatus = 0;
            }
#endif
            //呼叫按键立即唤醒(退出休眠模式)
            if (keyCallStatus && MPU_SYS_AwakeSchedule_GetDevSleepStatus() == 0) {
                MPU_SYS_AwakeSchedule_DorEnable(1);
            }
        }

        switch (call_status)
        {
            case IDLE_CALL_STATUS:  //空闲
            {
                if (PasExternCallState) //外部通话中
                {
                    CallStatus = BUSY_CALL_STATUS;
                }
                else if (keyCallStatus == 1) //一键呼叫按键按下
                {
                    CallStatus = START_CALL_STATUS;
                }
                break;
            }
            case START_CALL_STATUS: //发起通话
            {
                if (is_offline == 1) //设备不在线
                {
                    is_offline = 0;
                    CallStatus = IDLE_CALL_STATUS; //恢复空闲状态
                }
                else
                {
                    CallStatus = WAIT_CALL_STATUS;
                    MPU_LCD_SetCallStatus(5);
                }
                break;
            }
            case WAIT_CALL_STATUS: //等待接听
            {
                if (keyHangupStatus || keyCallStatus) //设备挂断
                {
                    MPU_LCD_SetCallStatus(4);
                    ZWSys_SetKey2callhanupState(1);
                    sleep(1);
                    CallStatus = FACI_END_CALL_STATUS;
                }
                else if (PasExternCallState) //外部通话中
                {
                    ZWSys_SetKey2callhanupState(1);
                    CallStatus = BUSY_CALL_STATUS;
                }
#if 0
                /* 通话状态全靠PasExternCallState决定 */
                else if (ActExternCallState == 1) //远程接听
                {
                    ZWSys_SetKey2callhanupState(1);
                    CallStatus = BUSY_CALL_STATUS;
                }
#endif
                else if (ActExternCallState == 2) //远程挂断
                {
                    MPU_LCD_SetCallStatus(4);
                    ZWSys_SetKey2callhanupState(1);
                    CallStatus = END_CALL_STATUS;
                }
                else if (is_noanswer == 1) //无人接听
                {
                    MPU_LCD_SetCallStatus(3);
                    ZWSys_SetKey2callhanupState(1);
                    is_noanswer = 0;
                    CallStatus = FACI_END_CALL_STATUS;
                }
                break;
            }
            case BUSY_CALL_STATUS: //通话中
            {
                if (keyHangupStatus || keyCallStatus) //设备挂断
                {
                    CallStatus = FACI_END_CALL_STATUS;
                    if (HieServer_RefreshEvent(USEREVENT_ALARM_NOTICE, ALERT_HANGUP_VIDEO_CALL, 0, 0) == 0) {
                        MPU_WARN("RefreshEvent HANGUP_VIDEO_CALL Success\n");
                    }
                    else {
                        MPU_WARN("RefreshEvent HANGUP_VIDEO_CALL Failed\n");
                    }
                }
                else if (PasExternCallState == 0) //外部挂断
                {
                    CallStatus = END_CALL_STATUS;
                }
                break;
            }
            case END_CALL_STATUS: //结束通话
            {
                CallStatus = IDLE_CALL_STATUS;
                break;
            }
            case FACI_END_CALL_STATUS: //结束通话
            {
                CallStatus = IDLE_CALL_STATUS;
                break;
            }
            default:
                 break; 
        }

        if (call_status != CallStatus)
        {
            MPU_WARN("[Key_to_Call] CallStatus:[%d]->[%d] ExternCallState:[%d] [%d]\n", call_status, CallStatus, ActExternCallState, PasExternCallState);
        }
        call_status = CallStatus;
    }
#else
    void RefreshCallStatus()
    {
        int keyCallStatus = 0;      //呼叫按键状态
        int keyHangupStatus = 0;    //挂断按键状态
        int RecvCallState = 0;
        KEY_TO_CALL_STATUS CallStatus = call_status;

        GetRecvCallState(&RecvCallState);
        ZWSys_GetCallPinStatus(&keyCallStatus, &keyHangupStatus);
        if (keyCallStatus || keyHangupStatus) {
#if defined (__IS_SUPPORT_REMINDER_SCHEDULE__)
            //呼叫按键打断定时提醒
            if (MPU_SYS_RemindSchedule_InterruptReminder() == 1) {
                keyCallStatus = 0;
                keyHangupStatus = 0;
            }
#endif
        }
		
		//呼叫按键立即唤醒(退出休眠模式)
		if (keyCallStatus &&MPU_SYS_AwakeSchedule_GetDevSleepStatus() == 0) 
		{
			//退出睡眠状态
			MPU_SYS_AwakeSchedule_DorEnable(1);
		} 

        switch (call_status)
        {
            case IDLE_CALL_STATUS:  //空闲
            {
                if (keyCallStatus == 1) //一键呼叫按键按下
                {
                    CallStatus = START_CALL_STATUS;
                }
                break;
            }
            case START_CALL_STATUS: //发起通话
            {
                if (is_offline == 1) //设备不在线
                {
                    is_offline = 0;
                    CallStatus = IDLE_CALL_STATUS; //恢复空闲状态
                }
                else
                {
                    CallStatus = WAIT_CALL_STATUS;
                }
                break;
            }
            case WAIT_CALL_STATUS: //等待接听
            {
                if (keyHangupStatus == 1) //设备挂断
                {
                    ZWSys_SetKey2callhanupState(1);
                    sleep(1);
                    CallStatus = FACI_END_CALL_STATUS;
                }
                else if (RecvCallState == 1) //远程接听
                {
                    ZWSys_SetKey2callhanupState(1);
                    CallStatus = BUSY_CALL_STATUS;
                }
                else if (RecvCallState == 2) //远程挂断
                {
                    ZWSys_SetKey2callhanupState(1);
                    CallStatus = END_CALL_STATUS;
                }
                else if (RecvCallState == 3) //远程开启直播流
                {
                    ZWSys_SetKey2callhanupState(1);
                    CallStatus = BUSY_CALL_STATUS;
                }
                else if (is_noanswer == 1) //无人接听
                {
					ZWSys_SetKey2callhanupState(1);
                    is_noanswer = 0;
                    CallStatus = FACI_END_CALL_STATUS;
                }
                break;
            }
            case BUSY_CALL_STATUS: //通话中
            {
                if (keyHangupStatus == 1) //设备挂断
                {
                    CallStatus = FACI_END_CALL_STATUS;
                }
                else if (RecvCallState == 4) //远程关闭直播流，关闭当前通话
                {
                    CallStatus = END_CALL_STATUS;
                }
                break;
            }
            case END_CALL_STATUS: //结束通话
            {
                CallStatus = IDLE_CALL_STATUS;
                break;
            }
			case FACI_END_CALL_STATUS: //结束通话
            {
                CallStatus = IDLE_CALL_STATUS;
                break;
            }
            default:
                 break; 
        }

        if (call_status != CallStatus)
        {
            MPU_INFO("[Key_to_Call] CallStatus:[%d]->[%d] RecvCallState:[%d]\n", call_status, CallStatus, RecvCallState);
        }
        call_status = CallStatus;
    }
#endif

	void ThreadProcess()
	{        
        while (0 != sem_trywait(&m_semExit))
        {
            usleep(100000);

            // 刷新一键呼叫通话状态
            RefreshCallStatus();
            // 执行一键呼叫通话动作
            switch (call_status)
            {
                case IDLE_CALL_STATUS:  //空闲
                {
                    break;
                }
                case START_CALL_STATUS: //发起通话
                {
                    StartCall();
                    break;
                }
                case WAIT_CALL_STATUS: //等待接听
                {
                    WaitAnswer();
                    break;
                }
                case BUSY_CALL_STATUS: //通话中
                {
                    break;
                }
                case END_CALL_STATUS: //结束通话
                {
                    StopCall();
                    break;
                }
                case FACI_END_CALL_STATUS: //设备端在接听前挂断通话
                {
                    StopCall_Device();
                    break;
                }
                default:
                     break; 
            }
        }
    }
}check_callevent_mgt_t;

static check_callevent_mgt_t g_callevent;

static void *check_callstatus_thread(void *param)
{
    PRT_TID();
    
	BEGINRUN_THREAD;

	g_callevent.ThreadProcess();

	ENDRUN_THREAD;
    
    return NULL;
}

// 外部设置一键呼叫状态(1：接听 2：挂断 3：直播流开启 4:直播流关闭)
int CIU_CALL_SetRecvCallState(int state)
{
    if (state < 1 || state > 4)
    {
        return -1;
    }
    g_callevent.SetRecvCallState(state);
    return 0;
}

// 外部获取接听后开启的码流id
int CIU_CALL_GetCallConnectionId(DWORD ConnectionId)
{
    if (ConnectionId < 0)
    {
        return -1;
    }
    g_callevent.GetCallConnectionId(ConnectionId);
    return 0;
}
// 外部获取挂断后关闭的码流id
int CIU_CALL_GetHangupConnectionId(DWORD ConnectionId)
{
    if (ConnectionId < 0)
    {
        return -1;
    }
    g_callevent.GetHangupConnectionId(ConnectionId);
    return 0;
}
#endif


/**************************************************************************
 *                            模块内部接口                                *
 **************************************************************************/
//!\brief       初始化远程接入模块
//!\return  0表示成功，其它为错误码
int CIU_Init()
{
	int nRet = IDVR_OK, nSDKRet;

//////////////////////////////////////////////////////////////////////////
//防止在客户端关掉后收到broken pipe信号，致使程序退出。
	int ret;
	MPU_DBG("\n---MPU_CIU_Init.cpp-----CIU_Init()-----------初始化远程接入模块..........\n");
	static bool bSys_Signal = true;

	if (bSys_Signal)
	{
		struct sigaction sigact;//结构体 sigaction【查询或设置信号处理方式】

		memset(&sigact, 0, sizeof (sigact));
		sigact.sa_handler = SIG_IGN;
		if ((ret = sigaction(SIGPIPE, &sigact, 0)) != 0)
		{
			MPU_ERR("Unable to turn off SIGPIPE: %s\n", strerror(ret));
			return FALSE;
		}
		bSys_Signal = true;
	}
//////////////////////////////////////////////////////////////////////////

	CIU_UpdatePortConfig();

#ifdef CIU_USE_OLD_LIB

	CIU_RegisterPreview();

#else


	//注册远程系统配置接口
	MPU_DBG("\n---MPU_CIU_Init.cpp-----CIU_Init()----...CIU_RegisterConfig().....注册远程系统配置接口......\n");
	CIU_RegisterConfig();

	// 注册预览
	MPU_DBG("\n---MPU_CIU_Init.cpp-----CIU_Init()----...CIU_RegisterPreview();......注册预览..........\n");
	CIU_RegisterPreview();

	// 注册登陆
	MPU_DBG("\n---MPU_CIU_Init.cpp-----CIU_Init()----...CIU_RegisterLogin();........注册登陆........\n");
	CIU_RegisterLogin();

	// 注册下载
	MPU_DBG("\n---MPU_CIU_Init.cpp-----CIU_Init()----...CIU_RegisterDownload();.......注册下载.......\n");
	CIU_RegisterDownload();

	// 注册升级
	MPU_DBG("\n---MPU_CIU_Init.cpp-----CIU_Init()----...CIU_RegisterUpload(); ... 注册升级..........\n");
	CIU_RegisterUpload();

	// 注册日志查询
	MPU_DBG("\n---MPU_CIU_Init.cpp-----CIU_Init()----...CIU_RegisterLogQuery();..........注册日志查询..........\n");
	CIU_RegisterLogQuery();

	//远程回放
	MPU_DBG("\n---MPU_CIU_Init.cpp-----CIU_Init()----...CIU_Playback_init();..........远程回放..........\n");
	CIU_Playback_init();
	
	//远程图片管理
	MPU_DBG("\n---MPU_CIU_Init.cpp-----CIU_Init()----...CIU_PictureManager_init();.........远程图片管理..........\n");
	CIU_PictureManager_init();

	//远程命令
	MPU_DBG("\n---MPU_CIU_Init.cpp-----CIU_Init()----...CIU_RemoteCommand_init();.......远程命令.......\n");
	CIU_RemoteCommand_init();

    //启动远程服务
	MPU_DBG("\n---MPU_CIU_Init.cpp-----CIU_Init()----...HieServer_Start();........启动远程服务............\n");
	nSDKRet = HieServer_Start();
	MPU_DBG("HieServer_Start() return %d\n", nSDKRet);

	//创建私有协议启动标记
	mysystem("touch /tmp/.hieserverStarted");

	//启动自动探测服务
	MPU_DBG("\n---MPU_CIU_Init.cpp-----CIU_Init()----...HieServer_DeviceProbeStart();......启动自动探测服务.......\n");
	nSDKRet = HieServer_DeviceProbeStart();
	MPU_DBG("HieServer_DeviceProbeStart() return %d\n", nSDKRet);

	g_reset.Init();
	g_alarm.Init();
#ifdef __ONE_CLICK_CALL__
	g_callevent.Init();
#endif


#endif

	return nRet;
}

//!\brief       逆初始化远程接入
//!\return  0表示成功，其它为错误码
int CIU_Uninit()
{
	int nSDKRet;

#ifndef CIU_USE_OLD_LIB

	//启动自动探测服务
	nSDKRet = HieServer_DeviceProbeStop();
	MPU_DBG("HieServer_DeviceProbeStop() return %d\n", nSDKRet);

	//停止远程服务
	nSDKRet = HieServer_Stop();
	MPU_DBG("HieServer_Stop() return %d\n", nSDKRet);

	//取消注册远程配置回调
	//CIU_UnRegisterConfig();

	//销毁网络库重启线程
	g_reset.Uninit();

	//销毁报警事件异步通知线程
	g_alarm.Uninit();

#endif

	return IDVR_OK;
}

//重启网络服务
int CIU_ResetServer(int nSecond)
{
	return g_reset.Start(nSecond);
}

//!\brief       异步报警事件通知
//!\param   pAlertInfo      报警事件信息
//!\return  0表示成功，其它为错误码
int CIU_RefreshAlarmEvent(struct tagAlertInfo *pAlertInfo)
{
	return g_alarm.Notify(pAlertInfo);
}

