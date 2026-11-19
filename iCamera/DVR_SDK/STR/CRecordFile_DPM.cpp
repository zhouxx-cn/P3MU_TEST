#include "debug.h"
#include "CRecordFile_DPM.h"
#include "global_macro.h"
#include "macro_pthread.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "../PAR/CPubFunc.h"
#define GETBIT(y,x)	((y) & (1<<(x)))



#define SWAP16(x) (test_lite_big()?(x):(((((x)&0xff)<<8)|(((x)>>8)&0xff))))
#define SWAP32(x)  (test_lite_big()?(x):(((SWAP16((x)&0xffff)<<16)|(SWAP16((x)>>16)))))
#if (MAX_PLAY_PORT == 16)
unsigned char CDPM_Record::m_nPortFlag[MAX_PLAY_PORT] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
#endif

/*!
 * \brief 数据弹出时是否只弹出关键帧
 * \param hDataPopper 数据弹出器的句柄
 * \param bKeyFrame 是否只弹出关键帧，如果为TRUE则只弹出关键帧，如果为FALSE则弹出所有帧。
 * \param PopKeyInterval 当只弹出关键帧时，该参数有效，
 *  表示通道弹出每隔多少关键帧弹出一个，此值限制在0-3，0表示全部弹出
 * \return 成功返回0，失败返回小于0的错误码。
 */
int CDPM_Record::DataPopSetKeyFrame(BOOL bKeyFrame, unsigned long PopKeyInterval)
{
	DEBUG_MSG("here\n");
	m_bKeyFrame = bKeyFrame;
	m_PopKeyInterval = PopKeyInterval;
	DEBUG_MSG("here\n");
	return 0;
}



/*!
 * \brief 设置数据弹出方向
 * \param hDataPopper 数据弹出器的句柄
 * \param bKeyFrame 数据弹出方向，如果为TRUE则向后弹出，如果为FALSE则向前弹出
 * \return 成功返回0，失败返回小于0的错误码。
 */
int CDPM_Record::DataPopSetDirect(BOOL bSequential)
{
	DEBUG_MSG("here\n");
	m_bSequential = bSequential;
	DEBUG_MSG("here\n");
	return 0;
}


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
int CDPM_Record::DataPopGetOneKeyFrame(int iChannel, time_t tSeekTime, unsigned int precision, unsigned char *pbuf, unsigned int *bufsize)
{
	int i = iChannel;

	DEBUG_MSG("here\n");
	CDPM_Record *S = this;

	int ret;

	char *pFrameHead;

	int nFrameHeadLenth;

	int nFrameType;

	char *pFrameData;

	int nFrameDataLenth;

	unsigned int FrameTime;

	if (!GETBIT(S->m_nChMask, i))
	{
		TRACE("通道%d不在范围内:%x\n", i, S->m_nChMask);
	}

	tagTime T;

	T.MkTag(tSeekTime);
	T.print();

	char FileName[256];

	S->CloseFile(S->m_OpenFileDev[i]);
	ret = S->GetFileName(i, tSeekTime, FileName);

	if (ret == 0)
	{
		DEBUG_MSG("DATAPOP_EVENT_TIME_JUMP:%d\n", i);
		S->m_OpenFileDev[i] = S->OpenFile(FileName, 0);

	}
	else
	{
		DEBUG_MSG("DATAPOP_EVENT_TIME_OVER:%d\n", i);
		return -1;
	}

	if ((HANDLE) - 1 != S->m_OpenFileDev[i])
	{
		DEBUG_MSG("ReadOneFrame %d S->m_bKeyFrame = %d ,S->m_bSequential = %d\n", i, S->m_bKeyFrame, S->m_bSequential);
		ret = S->ReadOneFrame(S->m_OpenFileDev[i], 1, 0, 0, &pFrameHead, &nFrameHeadLenth, &nFrameType, &pFrameData, &nFrameDataLenth, &FrameTime);
		if (0 == ret)
		{
			if (nFrameDataLenth < 4)
			{
				TRACE("Data len = %d error \n", nFrameDataLenth);
				return -1;
			}

			memcpy(pbuf, pFrameHead, nFrameHeadLenth);
			memcpy(pbuf + nFrameHeadLenth, (void *) pFrameData, nFrameDataLenth);
			*bufsize = nFrameHeadLenth + nFrameDataLenth;
			//DEBUG_MSG("read success\n");
			time_t tTime;

			if (nFrameType != 0x8)
			{
				memcpy(&tTime, pFrameData, 4);
				SWAP32(tTime);
				tagTime T;

				T.MkTag(tTime);
				T.print();
			}

		}
		else
		{
			DEBUG_MSG("next File over\n");
		}
		S->CloseFile(S->m_OpenFileDev[i]);
		DEBUG_MSG("here\n");
		return 0;
	}
	DEBUG_MSG("here\n");
	return -1;
}


/*!
 * \brief 数据弹出器定位(设定了参考通道，按参考通道定位，未设参考通道，定位为时序上第一个满足条件的通道)
 * \param hDataPopper 数据弹出器的句柄
 * \param SeekTime 设置的弹出数据的时间
 * \return 成功返回0，失败返回小于0的错误码。
 */
int CDPM_Record::DataPopTimeSeek(time_t SeekTime)
{
	int i, ret;

	int old;

	char FileName[256];

	DEBUG_MSG("here time = %ld\n", time(NULL));
	lock();

	DEBUG_MSG("here\n");

	DEBUG_MSG("here time = %ld\n", time(NULL));
	DEBUG_MSG("here\n");
	for (i = 0; i < __REAL__CH__; i++)
	{
		if (!GETBIT(m_nChMask, i))
		{
			DEBUG_MSG("CH:%d is no in:%x\n", i, m_nChMask);
			continue;
		}
		old = m_OpenFileIndex[i];

		ret = GetFileName(i, SeekTime, FileName);

		if (m_OpenFileIndex[i] == old && m_OpenFileDev[i] != (HANDLE) - 1)
		{
			TRACE("same file\n");
			tag_RecordFileInfo Info;

			m_RecordFile->GetInfo(m_Dev[i], m_OpenFileIndex[i], &Info);
			m_nPlayTime = SeekTime;
			TRACE("SeekTime = %ld Info.m_nDateTime = %ld\n", SeekTime, Info.m_nDateTime);
			if (SeekTime > Info.m_nDateTime)
			{
				Seek(m_OpenFileDev[i], SeekTime - Info.m_nDateTime);
			}
		}
		else if (0 == ret)
		{
			DEBUG_MSG("here time = %ld\n", time(NULL));
			CloseFile(m_OpenFileDev[i]);

			m_OpenFileDev[i] = OpenFile(FileName, 0);
			if ((HANDLE) - 1 == m_OpenFileDev[i])
			{
				while (1)
				{
					if (m_bSequential == 0)
					{
						ret = GetBeforeFileName(i, FileName);
					}
					else
					{
						ret = GetNextFileName(i, FileName);
					}

					if (0 == ret)
					{
						m_OpenFileDev[i] = OpenFile(FileName, 0);
					}
					else
					{
						TRACE("No Data....\n");
						unlock();
						return -1;
					}

					if ((HANDLE) - 1 != m_OpenFileDev[i])
					{
						break;
					}
					else
					{
						TRACE("open %s failed\n", FileName);
					}

				}

			}
			DEBUG_MSG("Open file %s success\n", FileName);
			tag_RecordFileInfo Info;

			m_RecordFile->GetInfo(m_Dev[i], m_OpenFileIndex[i], &Info);
			m_nPlayTime = SeekTime;

			DEBUG_MSG("here time = %ld\n", time(NULL));
			TRACE("SeekTime = %ld Info.m_nDateTime = %ld\n", SeekTime, Info.m_nDateTime);
			if (SeekTime > Info.m_nDateTime)
			{
				Seek(m_OpenFileDev[i], SeekTime - Info.m_nDateTime);
			}
		}
		else
		{
			TRACE("没有文件\n");
		}
	}

	unlock();
	DEBUG_MSG("here time = %ld\n", time(NULL));
	return 0;
}


/*!
 * \brief 开始数据弹出
 * \param hDataPopper 数据弹出器的句柄
 * \return 成功返回0，失败返回小于0的错误码。
 */
int CDPM_Record::DataPopStart()
{
	DEBUG_MSG("here\n");
	int nRet;

	pthread_t thread_t;

	pthread_attr_t new_attr;

	pthread_attr_init(&new_attr);
	pthread_attr_setdetachstate(&new_attr, PTHREAD_CREATE_DETACHED);

	DataPopStop();
	m_bThreadRun = 1;
	DataPopTimeSeek(m_nPlayTime);

	nRet = pthread_create(&thread_t, &new_attr, CDPM_Record::work_thread, this);
	if (nRet != 0)
	{
		TRACE("create  thread error.....\n");
		return -1;
	}

	DEBUG_MSG("here\n");
	return 0;
}



/*!
 * \brief 停止数据弹出
 * \param hDataPopper 数据弹出器的句柄
 * \return 成功返回0，失败返回小于0的错误码。
 */
int CDPM_Record::DataPopStop()
{
	int bThreadRun = m_bThreadRun;

	DEBUG_MSG("here\n");
	lock();
	m_bThreadRun = 0;
	unlock();
	if (bThreadRun)
	{
		DEBUG_MSG("等待结束\n");
		sem_wait(&m_sem);
	}
	DEBUG_MSG("here\n");
	return 0;
}

/*!
*\brief 得到文件的大小,返回的为字节数
*
*
*/
//!\brief 得到文件的大小,返回的为字节数
//!\praram char *path 文件路径
//!\return  文件的大小：（单位：字节）
static long file_size(char *path)
{
	struct stat buf;

	int fd;

	fd = open(path, O_RDONLY);
	if (fd == -1)
		return 0;
	fstat(fd, &buf);
	close(fd);
	return buf.st_size;
}

/*!
 * \brief 开始数据弹出
 * \param hDataPopper 数据弹出器的句柄
 * \return 成功返回0，失败返回小于0的错误码。
 */
long long CDPM_Record::GetSize()
{
	int nCh = 0;

	long long m_nSize = 0;

	int nSize = 0;

	int Num = m_RecordFile->GetCount(m_Dev[nCh]);

	tag_RecordFileInfo Info;

	int i;

	for (i = 0; i < Num; i++)
	{
		if (GETBIT(m_nChMask, nCh))
		{
			m_RecordFile->GetInfo(m_Dev[nCh], i, &Info);

			if (!GETBIT(m_nTypeMask, Info.m_Type))
			{
				continue;
			}

			//TRACE("Info.m_nDateTime  = %d,m_nBeginTime  = %d,Info.m_nTime  = %d,m_nEndTime = %d\n",Info.m_nDateTime ,m_nBeginTime ,Info.m_nTime ,m_nEndTime);
			if (Info.m_nDateTime >= m_nBeginTime && Info.m_nTime <= m_nEndTime)
			{
				m_nSize += file_size(Info.m_szFileName);
			}
			else if (Info.m_nDateTime >= m_nEndTime || Info.m_nTime < m_nBeginTime )
			{
			}
			else
			{
				//m_nSize += file_size(Info.m_szFileName);
				nSize = 0;
				MP4Sys_DecGetFrameSize(Info.m_szFileName, m_nBeginTime, m_nEndTime, &nSize);
				TRACE("Info.m_szFileName= %s Info.m_nDateTime  = %d,m_nBeginTime  = %d,Info.m_nTime  = %d,m_nEndTime = %d ,nSize = %d\n",
					  Info.m_szFileName, Info.m_nDateTime, m_nBeginTime, Info.m_nTime, m_nEndTime, nSize);
				m_nSize += nSize;
			}

		}
	}
	return m_nSize / 1024;
}


int CDPM_Record::GetFileName(int nCh, time_t tTime, char *FileName)
{
	int num;

	int i;

	tag_RecordFileInfo Info;

	if (!GETBIT(m_nChMask, nCh))
	{
		DEBUG_MSG("nCh = %d is not in\n", nCh);
		return -1;
	}

	if (NULL != m_Dev[nCh] && NULL != m_RecordFile)
	{
		num = m_RecordFile->GetCount(m_Dev[nCh]);
		for (i = 0; i < num; i++)
		{
			m_RecordFile->GetInfo(m_Dev[nCh], i, &Info);

			if (!GETBIT(m_nTypeMask, Info.m_Type))
			{
				DEBUG_MSG("Info.m_Type = %d m_nTypeMask = %dis not in \n", Info.m_Type, m_nTypeMask);
				continue;
			}

			if (Info.m_nTime > tTime)
			{
 				tagTime T;

				T.MkTag(Info.m_nDateTime);
				T.print();
				T.MkTag(Info.m_nTime);
				T.print();
 				m_OpenFileIndex[nCh] = i;
				return 0;
			}
		}
	}
	else
	{
		TRACE("m_Dev[%d] = %p, m_RecordFile = %p\n", nCh, m_Dev[nCh], m_RecordFile);
		tagTime T;

		T.MkTag(tTime);
		T.print();
	}

	return -1;
}


int CDPM_Record::GetNextFileName(int nCh, char *FileName)
{
	int num;

	int i;

	tag_RecordFileInfo Info;

	if (!GETBIT(m_nChMask, nCh))
	{
		TRACE("nCh = %d is not in\n", nCh);
		return -1;
	}

	if (m_OpenFileIndex[nCh] == -1)
	{
		return -1;
	}

	if (NULL != m_Dev[nCh] && NULL != m_RecordFile)
	{
		num = m_RecordFile->GetCount(m_Dev[nCh]);
		for (i = m_OpenFileIndex[nCh] + 1; i < num; i++)
		{
			m_RecordFile->GetInfo(m_Dev[nCh], i, &Info);

			if (!GETBIT(m_nTypeMask, Info.m_Type))
			{
				TRACE("Info.m_Type = %d is not in\n", Info.m_Type);
				return -1;
			}

			if (Info.m_nTime > m_nBeginTime && Info.m_nTime <= m_nEndTime)
			{
				memcpy(FileName, Info.m_szFileName, strlen(Info.m_szFileName)+1);
				m_OpenFileIndex[nCh] = i;
				return 0;
			}
		}
	}
	else
	{
		TRACE("m_Dev[%d] = %p, m_RecordFile = %p\n", nCh, m_Dev[nCh], m_RecordFile);
	}

	m_OpenFileIndex[nCh] = -1;

	return -1;
}

int CDPM_Record::GetBeforeFileName(int nCh, char *FileName)
{
	int i;

	tag_RecordFileInfo Info;

	if (!GETBIT(m_nChMask, nCh))
	{
		DEBUG_MSG("nCh = %d is not in\n", nCh);
		return -1;
	}

	if (NULL != m_Dev[nCh] && NULL != m_RecordFile)
	{
		m_RecordFile->GetCount(m_Dev[nCh]);
		for (i = m_OpenFileIndex[nCh] - 1; i >= 0; i--)
		{
			m_RecordFile->GetInfo(m_Dev[nCh], i, &Info);

			if (!GETBIT(m_nTypeMask, Info.m_Type))
			{
				DEBUG_MSG("Info.m_Type = %d is not in\n", Info.m_Type);
				return -1;
			}

			if (Info.m_nTime > m_nBeginTime && Info.m_nTime <= m_nEndTime)
			{
				memcpy(FileName, Info.m_szFileName, strlen(Info.m_szFileName)+1);
				m_OpenFileIndex[nCh] = i;
				return 0;
			}
		}
	}
	else
	{
		DEBUG_MSG("m_Dev[%d] = %p, m_RecordFile = %p\n", nCh, m_Dev[nCh], m_RecordFile);
	}

	m_OpenFileIndex[nCh] = -1;

	return -1;
}

void *CDPM_Record::work_thread(void *_this)
{
	prctl(PR_SET_NAME,"work_thread");
    PRT_TID();

	BEGINRUN_THREAD;
	int i;

	char *pFrameHead;

	int nFrameHeadLenth;

	int nFrameType;

	char *pFrameData;

	int nFrameDataLenth;

	unsigned int FrameTime;

	int ret;

	int bPopData = 0;

	CDPM_Record *S = (CDPM_Record *) _this;

	time_t tTime;
	tag_RecordFileInfo Info;

	while (S->m_bThreadRun)
	{
		S->lock();

		if (S->m_bThreadRun == 0)
		{
			for (i = 0; i < __REAL__CH__; i++)
			{
				S->CloseFile(S->m_OpenFileDev[i]);
			}
			S->unlock();
			break;
		}

		bPopData = 0;
		//DEBUG_MSG("locked\n");
		for (i = 0; i < __REAL__CH__; i++)
		{
			if (GETBIT(S->m_nChMask, i))
			{
				if ((HANDLE) - 1 != S->m_OpenFileDev[i])
				{
					bPopData = 1;
					//TRACE("ReadOneFrame %d S->m_bKeyFrame = %d ,S->m_bSequential = %d\n",i,S->m_bKeyFrame ,S->m_bSequential);
 					ret = S->ReadOneFrame(S->m_OpenFileDev[i], S->m_bKeyFrame, 0, 0, &pFrameHead, &nFrameHeadLenth, &nFrameType, &pFrameData,
										&nFrameDataLenth, &FrameTime);
					if (0 == ret)
					{
						/*!
						 * \brief 数据弹出回调函数
						 * \param hDataPopper 数据弹出器句柄
						 * \param iChannel 数据对应的通道
						 * \param bIndexFrame 是否是建立索引的帧
						 * \param wFrameType 数据对应的类型
						 * \param tFrameTime 数据帧的绝对时间簇，精确度为秒
						 * \param pBuf 数据对应的缓冲区地址
						 * \param dwSize 数据对应的大小
						 * \param EventID 发生的事件类型，如果是正常的数据弹出，EventID为DATAPOP_EVENT_NONE，其他事件时，帧相关的参数无效
						 * \param pContext 创建弹出器时传入的上下文
						 * \return
						 */
						//typedef int (*HY_REPLAY_DATAPOP_CALLBACK)(HANDLE hDataPopper, int iChannel, BOOL bIndexFrame, unsigned short wFrameType, time_t tFrameTime, unsigned char *pBuf, unsigned long dwSize, unsigned long EventID, void *pContext);

						if (nFrameDataLenth < 4)
						{
							TRACE("Data len = %d error \n", nFrameDataLenth);
							continue;
						}

						//memcpy(Data+nFrameHeadLenth,(void *)pFrameData,nFrameDataLenth);
						//DEBUG_MSG("read success\n");
						if (nFrameType != 0x8)
						{
							memcpy(&tTime, pFrameData + H264DATAPACKETHEADER_REAL_TIME_OFFSET, 4);

							S->m_nPlayTime = SWAP32(tTime);
#ifndef _RNL_
							tagTime T;

							T.MkTag(S->m_nPlayTime);
							T.print();
#endif
						}

						if (S->m_nPlayTime > S->m_nEndTime)
						{
							TRACE("DATAPOP_EVENT_TIME_OVER:%d\n", i);

							if (NULL != S->m_cbGetIndexCallback)
							{
								S->m_cbGetIndexCallback(S, i, 0, nFrameType, 10000, (unsigned char *) pFrameData, nFrameDataLenth,
														DATAPOP_EVENT_TIME_OVER, S->m_pChContext);
							}

							for (i = 0; i < __REAL__CH__; i++)
							{
								S->CloseFile(S->m_OpenFileDev[i]);
								S->m_OpenFileDev[i] = (HANDLE) - 1;
							}

							sem_post(&S->m_sem);
							S->unlock();
							ENDRUN_THREAD;
							return NULL;
						}

						/*!
						 * \brief 数据弹出回调函数
						 * \param hDataPopper 数据弹出器句柄
						 * \param iChannel 数据对应的通道
						 * \param bIndexFrame 是否是建立索引的帧
						 * \param wFrameType 数据对应的类型
						 * \param tFrameTime 数据帧的绝对时间簇，精确度为秒
						 * \param pBuf 数据对应的缓冲区地址
						 * \param dwSize 数据对应的大小
						 * \param EventID 发生的事件类型，如果是正常的数据弹出，EventID为DATAPOP_EVENT_NONE，其他事件时，帧相关的参数无效
						 * \param pContext 创建弹出器时传入的上下文
						 * \return
						 */

						if (NULL != S->m_cbGetIndexCallback)
						{
							S->m_cbGetIndexCallback(S, i, (nFrameType == 1), PSS_FRAMETYPE_AV, tTime, (unsigned char *) pFrameData, nFrameDataLenth,
													DATAPOP_EVENT_NONE, S->m_pChContext);
						}
						//DEBUG_MSG("write data\n");

					}
					else
					{
						//记录上次回放完的文件的最后时间
						S->m_RecordFile->GetInfo(S->m_Dev[i], S->m_OpenFileIndex[i], &Info);
						tTime = Info.m_nTime;
						
						while (1)
						{
							TRACE("ReadOneFrame %d next file m_bSequential  = %d\n", i, S->m_bSequential);
							char FileName[256];

							S->CloseFile(S->m_OpenFileDev[i]);
							if (S->m_bSequential == 0)
							{
								ret = S->GetBeforeFileName(i, FileName);
							}
							else
							{
								ret = S->GetNextFileName(i, FileName);
							}


							if (ret == 0)
							{
								TRACE("DATAPOP_EVENT_TIME_JUMP:%d\n", i);
								S->m_OpenFileDev[i] = S->OpenFile(FileName, 0);

								if (S->m_OpenFileDev[i] == (HANDLE) - 1)
								{
									TRACE("open file :%s failed\n", FileName);
									continue;
								}

								if (S->m_bSequential == 0)
								{

									S->m_RecordFile->GetInfo(S->m_Dev[i], S->m_OpenFileIndex[i], &Info);
									TRACE("S->m_nPlayTime = %ld Info.m_nDateTime = %ld\n", S->m_nPlayTime, Info.m_nDateTime);
									S->Seek(S->m_OpenFileDev[i], Info.m_nTime - Info.m_nDateTime);
								}

								if (NULL != S->m_cbGetIndexCallback)
								{
									S->m_cbGetIndexCallback(S, i, 0, nFrameType, 10000, (unsigned char *) pFrameData, nFrameDataLenth,
															DATAPOP_EVENT_TIME_JUMP, S->m_pChContext);
								}
								break;
							}
							else if (ret != 0)
							{
								if (S->m_Dev[i] != NULL)
								{
									S->m_RecordFile->ReleaseList(S->m_Dev[i]);
									tagTime T;

									T.MkTag(S->m_nBeginTime);
									T.print();
									T.MkTag(S->m_nEndTime);
									T.print();

									S->m_Dev[i] = S->m_RecordFile->CreateList(i, S->m_nBeginTime, S->m_nEndTime, __SEARCH_ALL_TYPE__);
								}

								//
								S->m_nPlayTime =  tTime;
								ret = S->GetFileName(i, S->m_nPlayTime + 1, FileName);

								if (0 != ret)
								{
									TRACE("DATAPOP_EVENT_TIME_OVER:%d\n", i);

									if (NULL != S->m_cbGetIndexCallback)
									{
										S->m_cbGetIndexCallback(S, i, 0, nFrameType, 10000, (unsigned char *) pFrameData, nFrameDataLenth,
																DATAPOP_EVENT_TIME_OVER, S->m_pChContext);
									}

									break;
								}


								S->m_RecordFile->GetInfo(S->m_Dev[i], S->m_OpenFileIndex[i], &Info);

								TRACE("DATAPOP_EVENT_TIME_JUMP:%d\n", i);
								S->m_OpenFileDev[i] = S->OpenFile(Info.m_szFileName, 0);

								if (S->m_OpenFileDev[i] == (HANDLE) - 1)
								{
									TRACE("open file :%s failed\n", FileName);
									continue;
								}
								
								if (S->m_nPlayTime > Info.m_nDateTime)
								{
									TRACE("S->m_nPlayTime = %ld Info.m_nDateTime = %ld\n", S->m_nPlayTime, Info.m_nDateTime);
									S->Seek(S->m_OpenFileDev[i], S->m_nPlayTime - Info.m_nDateTime);
								}

								TRACE("DATAPOP_EVENT_TIME_JUMP:%d\n", i);
								if (NULL != S->m_cbGetIndexCallback)
								{
									S->m_cbGetIndexCallback(S, i, 0, nFrameType, 10000, (unsigned char *) pFrameData, nFrameDataLenth,
															DATAPOP_EVENT_TIME_JUMP, S->m_pChContext);
								}
								break;
							}
							TRACE("next File over\n");
						}
					}
				}
				else
				{
					TRACE("CH:%d is anot replay\n", i);
				}
			}
		}
		//DEBUG_MSG("unlock\n");
		S->unlock();

		if (bPopData == 0)
		{
			usleep(100000);
		}
	}

	for (i = 0; i < __REAL__CH__; i++)
	{
		S->CloseFile(S->m_OpenFileDev[i]);
		S->m_OpenFileDev[i] = (HANDLE) - 1;
	}

	ENDRUN_THREAD;
	sem_post(&S->m_sem);
	return NULL;
}
