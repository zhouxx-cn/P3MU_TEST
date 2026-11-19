#ifndef __CDPM_RECORD_H__
#include <pthread.h>
#include <semaphore.h>

#include "pkgstreamstorage.h"
#include "CRecordFile.h"
#include "HY_FileBackup.h"

#define __CDPM_RECORD_H__

class CDPM_Record
{
  public:
	/*!
	 * \brief 数据弹出时是否只弹出关键帧
	 * \param hDataPopper 数据弹出器的句柄
	 * \param bKeyFrame 是否只弹出关键帧，如果为TRUE则只弹出关键帧，如果为FALSE则弹出所有帧。
	 * \param PopKeyInterval 当只弹出关键帧时，该参数有效，
	 *  表示通道弹出每隔多少关键帧弹出一个，此值限制在0-3，0表示全部弹出
	 * \return 成功返回0，失败返回小于0的错误码。
	 */
	int DataPopSetKeyFrame(BOOL bKeyFrame, unsigned long PopKeyInterval);



	/*!
	 * \brief 设置数据弹出方向
	 * \param hDataPopper 数据弹出器的句柄
	 * \param bKeyFrame 数据弹出方向，如果为TRUE则向后弹出，如果为FALSE则向前弹出
	 * \return 成功返回0，失败返回小于0的错误码。
	 */
	int DataPopSetDirect(BOOL bSequential);


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
	int DataPopGetOneKeyFrame(int iChannel, time_t tSeekTime, unsigned int precision, unsigned char *pbuf, unsigned int *bufsize);


	/*!
	 * \brief 数据弹出器定位(设定了参考通道，按参考通道定位，未设参考通道，定位为时序上第一个满足条件的通道)
	 * \param hDataPopper 数据弹出器的句柄
	 * \param SeekTime 设置的弹出数据的时间
	 * \return 成功返回0，失败返回小于0的错误码。
	 */
	int DataPopTimeSeek(time_t SeekTime);


	/*!
	 * \brief 开始数据弹出
	 * \param hDataPopper 数据弹出器的句柄
	 * \return 成功返回0，失败返回小于0的错误码。
	 */
	int DataPopStart();



	/*!
	 * \brief 停止数据弹出
	 * \param hDataPopper 数据弹出器的句柄
	 * \return 成功返回0，失败返回小于0的错误码。
	 */
	int DataPopStop();

	long long GetSize();
	CRecordFile *m_RecordFile;

	int m_bThreadRun;

	//片段弹出器用的变量
	HY_REPLAY_DATAPOP_CALLBACK m_cbGetIndexCallback;
	void *m_pChContext;
	unsigned long m_nChMask;
	unsigned long m_nTypeMask;
	HANDLE m_Dev[__REAL__CH__];
	time_t m_nBeginTime;
	time_t m_nEndTime;
	time_t m_nPlayTime;

	static int m_iNum;

	  CDPM_Record()
	{
		int i;
		  m_RecordFile = NULL;
		  m_nChMask = 0;
		  m_pChContext = NULL;
		  m_cbGetIndexCallback = NULL;
		  m_bThreadRun = 0;
		  m_bSequential = 1;
		  m_iNum++;
		  m_nPlayTime = 0;
		  pthread_mutex_init(&m_mutex, NULL);

		for (i = 0; i < __REAL__CH__; i++)
		{
			m_Dev[i] = NULL;
			m_OpenFileIndex[i] = -1;
			m_OpenFileDev[i] = (HANDLE) - 1;
		}
		sem_init(&m_sem, 0, 0);
	}

	~CDPM_Record()
	{
		DataPopStop();
		m_nChMask = 0;
		m_pChContext = NULL;
		m_cbGetIndexCallback = NULL;
		int i;

		for (i = 0; i < __REAL__CH__; i++)
		{
			if (NULL != m_Dev[i])
			{
				m_RecordFile->ReleaseList(m_Dev[i]);
			}

			if (m_OpenFileDev[i] != (HANDLE) - 1)
			{
				CloseFile(m_OpenFileDev[i]);
			}

			m_OpenFileIndex[i] = 0;
			m_OpenFileDev[i] = (HANDLE) - 1;
		}

		m_RecordFile = NULL;
		pthread_mutex_destroy(&m_mutex);
		m_iNum--;
		sem_destroy(&m_sem);
	}

  private:

	int GetFileName(int nCh, time_t tTime, char *FileName);

	int GetNextFileName(int nCh, char *FileName);

	int GetBeforeFileName(int nCh, char *FileName);

	pthread_mutex_t m_mutex;

	int m_bKeyFrame;

	int m_PopKeyInterval;

	int m_bSequential;

	void lock()
	{
		pthread_mutex_lock(&m_mutex);
	}
	void unlock()
	{
		pthread_mutex_unlock(&m_mutex);
	}

	HANDLE m_OpenFileDev[__REAL__CH__];

	int m_OpenFileIndex[__REAL__CH__];

	static unsigned char m_nPortFlag[MAX_PLAY_PORT];

	void OpenPort(int n)
	{
		m_nPortFlag[n] = 1;
	}
	void ClosePort(int n)
	{
		m_nPortFlag[n] = 0;
	}

	int GetPort()
	{
		for (int i = 0; i < MAX_PLAY_PORT; i++)
		{
			if (0 == m_nPortFlag[i])
			{
				return i;
			}
		}
		return -1;
	}

	HANDLE OpenFile(char *szFileName, int bFirstFile)
	{
		int nPort = GetPort();

		if (nPort == -1)
		{
			TRACE("nPort = -1\n");
			return (HANDLE) - 1;
		}
		TRACE(".....nPort = %d\n", nPort);

		OpenPort(nPort);
		char IndexFileName[256];

		CRecordFile::GetIndexFileName(szFileName, IndexFileName);
		int ret = MP4Sys_DecOpenBackUpFile(nPort, szFileName, IndexFileName, bFirstFile);

		if (ret != 1)
		{
			ClosePort(nPort);
			TRACE("nPort = %d, szFileName = %s, bFirstFile = %d\n", nPort, szFileName, bFirstFile);
			return (HANDLE) - 1;
		}
		TRACE("SDK----:open %d:%s\n", nPort, szFileName);

		return (HANDLE) nPort;
	}
	int CloseFile(HANDLE _this)
	{
		int nPort = (int) _this;

		if (nPort == -1)
		{
			DEBUG_MSG("nPort = %d\n", nPort);
			return -1;
		}

		TRACE(".....nPort = %d\n", nPort);

		DEBUG_MSG("nPort = %d,m_nPortFlag[nPort] = %d\n", nPort, m_nPortFlag[nPort]);

		if (m_nPortFlag[nPort] == 1)
		{
			ClosePort(nPort);
			DEBUG_MSG("nPort = %d,m_nPortFlag[nPort] = %d\n", nPort, m_nPortFlag[nPort]);
			TRACE(".....nPort = %d\n", nPort);
			return !MP4Sys_DecCloseBackUpFile(nPort);
		}

		return 0;
	}

	int Seek(HANDLE _this, unsigned int TimeSec)
	{
		int nPort = (int) _this;

		return !MP4Sys_DecSetBackUpFileTime(nPort, TimeSec);
	}

	int ReadOneFrame(HANDLE _this,	//通道编号
					 int nReadFrameType,	//读取帧类型，0为所有帧，1为关键帧
					 int nReadDirection,	//读取方向，0为正向，1为反向
					 int bThrowAudio,	//是否扔掉音频，0读取音频，1丢弃音频
					 char **pFrameHead,	//帧头地址
					 int *pnFrameHeadLenth,	//帧头长度
					 int *pnFrameType,	//帧类型
					 char **pFrameData,	//帧数据地址
					 int *pnFrameDataLenth,	//帧数据长度
					 unsigned int *FrameTime)	//该帧相对于文件第一个I帧的时间(视频帧有效，单位为毫秒)
	{
		int nPort = (int) _this;

		return !MP4Sys_DecReadOneFrame(nPort, nReadFrameType, nReadDirection, bThrowAudio, pFrameHead, pnFrameHeadLenth, pnFrameType, pFrameData,
									   pnFrameDataLenth, FrameTime);
	}

	static void *work_thread(void *_this);

	sem_t m_sem;

};
#endif
