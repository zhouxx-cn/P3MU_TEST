
/*!
 *\file   HY_FileBackUp.c
 *\brief  文件备份的操作
 *\author jtx
 *\date	  2010/06/11
 *\version 0.1
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include "Commen.h"
#include "HY_FileBackup.h"
#include "HY_FileRead.h"
#include "sys_banned.h"
SDK_ResInfo_t g_BackupSDKResInfo[MAX_CHANNEL_CNT];

CFileBackUp_t g_hFileBackUp[MAX_CHANNEL_CNT];	//文件备份句柄信息

Index_File I_Frame_Index[I_Frame_Index_Num];	//暂定一个文件最多包含1800个I帧

Index_File Temp_Index[I_Frame_Index_Num];

Index_File Rebuild_Index[I_Frame_Index_Num];

static int I_cnt = 0;

static char Index_File_Name[256];

//要求将索引写入录像文件的尾部
//如果写文件的时候断电，则该文件没有索引，在打开文件的时候进行判断，然后将索引写入文件尾部


//-----------------------------------------------------------------------------
//      说明：          建立文件索引
//      输入参数：      FrameHead     帧头信息
//      输入参数：      FilePos     当前帧将要在文件中占据的位置
//      输入参数：      flag        0 开始写文件   1 正在写文件  2 写文件完毕
//      输入参数：      Index_Len   索引的长度
//      返回:           0 成功 －1 失败 其它地址表示索引建立完毕 
//-----------------------------------------------------------------------------
int MP4Sys_CreateFileIndex(char *FrameHead, int FilePos, int flag, int *Index_Len)
{
	H264DATAPACKETHEADER PacketHead = {0};

	if (flag == 0)				//刚开始写文件，上端传入的数据不是正确的一帧，而是文件头
	{
		memset(&I_Frame_Index[0], 0, 1800 * sizeof (Index_File));
		strncpy(Index_File_Name, FrameHead, sizeof (Index_File_Name));
		Index_File_Name[sizeof (Index_File_Name) - 1] = '\0';
		I_cnt = 0;				//第一个I帧，将计数器清零
		return 0;
	}
	if (flag == 2)				//结束，由于上端不知道何时结束，所以这一帧也没有数据
	{
		I_Frame_Index[I_cnt].I_Frame_Num = I_cnt + 1;
		I_Frame_Index[I_cnt].I_FRAME_Pos = Index_End_Flag;
		*Index_Len = I_cnt * 2 * 4 + 8;	//索引所占用的长度，以char来计量
		return (int)&I_Frame_Index[0];
	}
	if (flag == 1)
	{
		memcpy(&PacketHead, FrameHead, sizeof (H264DATAPACKETHEADER));
	}
	if (PacketHead.DataPacketStartCode == STARTCODE && PacketHead.FrameType == I_FRAME)
	{
		if (flag == 1)
		{
			I_Frame_Index[I_cnt].I_Frame_Num = I_cnt + 1;
			I_Frame_Index[I_cnt].I_FRAME_Pos = FilePos;
			I_cnt++;
			return 0;
		}
		else
			return -1;
	}
	return -1;
}



//-----------------------------------------------------------------------------
//      说明：          建立文件索引
//      输入参数：      nPort       对应通道编号
//      输入参数：      szFileName     文件名
//      输入参数：      Index_Len   索引的长度
//      返回:           0 不需要重建  -1 表示错误 其它地址表示索引建立完毕 
//-----------------------------------------------------------------------------
int MP4Sys_RebuildFileIndex(int nPort, char *szFileName, int *R_Index_Len)
{
	int RetValue;

	int StartCodeTmp, i;

	int CurPos, SearchIPos, IFramePos;

	int Index_flag, Index_Len;

	FILE *m_File = NULL;

	MPEG4FILEHEADER m_FileHead;

	unsigned int m_FirstIFrameNumStamp, m_FirstIFrameTimeStamp;
	int m_nFirstIFramePos;

	H264DATAPACKETHEADER m_FrameHead;

	int nIFrameNumber = 0;

	*R_Index_Len = 0;

	/*
	   参数检查 
	 */
	if ((nPort < 0) || (nPort >= MAX_CHANNEL_CNT))
	{
		return -1;
	}

	if (szFileName == NULL)
	{
		return -1;
	}
	m_File = fopen(szFileName, "rb");
	if (m_File == NULL)
	{
		fclose(m_File);
		return -1;
	}

	fread(&m_FileHead, 1, FILE_HEAD_LENTH, m_File);
	if (m_FileHead.FileStartCode != 0xB6010000)
	{
		fclose(m_File);
		return -1;
	}

	CurPos = ftell(m_File);		//得到当前文件指针位置
	fseek(m_File, -8, SEEK_END);
	//Index_Len 索引的长度，包含了最后2个标志的长度
	fread(&Index_Len, 1, 4, m_File);
	fread(&Index_flag, 1, 4, m_File);
	if (Index_flag == Index_End_Flag)
	{
		fclose(m_File);
		return 0;
	}
	else
	{
		i = 0;
		fseek(m_File, CurPos, SEEK_SET);
		CurPos = ftell(m_File);
		/*
		   搜寻第一个I帧
		 */
		RetValue = Higheasy_FR_FindFirstFrame(m_File, &m_FirstIFrameNumStamp, &m_FirstIFrameTimeStamp, &m_nFirstIFramePos);
		if (RetValue != 0)
		{
			fclose(m_File);
			return -1;
		}
		while (1)
		{
			if (Higheasy_FR_FindNextFrame(m_File, I_FRAME) < 0)
				break;
			IFramePos = ftell(m_File);
			fread((char *) &m_FrameHead, 1, sizeof (H264DATAPACKETHEADER), m_File);
			fseek(m_File, m_FrameHead.nOverloadLen, SEEK_CUR);
			fread((char *) &StartCodeTmp, 1, 4, m_File);
			if (StartCodeTmp != STARTCODE)
			{
				SearchIPos = ftell(m_File) - 4;
				fseek(m_File, -(m_FrameHead.nOverloadLen + sizeof (H264DATAPACKETHEADER)), SEEK_CUR);
				if (Higheasy_FR_FindNextFrame(m_File, 0xFF) == 0)
				{
					CurPos = ftell(m_File);
					if (CurPos < SearchIPos)
					{
						continue;
					}
				}
				fseek(m_File, SearchIPos, SEEK_SET);
			}
			else
				fseek(m_File, -4, SEEK_CUR);

			Rebuild_Index[i].I_Frame_Num = nIFrameNumber + 1;
			Rebuild_Index[i].I_FRAME_Pos = IFramePos;
			nIFrameNumber += 1;
			i++;
		}
		Rebuild_Index[i].I_Frame_Num = nIFrameNumber + 1;
		Rebuild_Index[i].I_FRAME_Pos = Index_End_Flag;
		*R_Index_Len = (nIFrameNumber + 1) * 8;
		fclose(m_File);
		return (int)&Rebuild_Index[0];
	}
}

//-----------------------------------------------------------------------------
//      说明：          打开需要进行备份的文件、检察文件头是否正确
//      输入参数：      nPort                   对应通道编号
//      输入参数：      szFileName               文件名
//      输入参数：      bFirstFile              是否第一个文件
//      返回:           成功或失败
//      注意：          为了兼容回放库，引入nPort参数
//-----------------------------------------------------------------------------
int MP4Sys_DecOpenBackUpFile(int nPort, char *szFileName, char *IndexFileName, int bFirstFile)
{
	CFileBackUp_t *pBackCh;

	int RetValue,ret;

	int StartCodeTmp, i;

	int CurPos, SearchIPos, IFramePos;

	int Index_flag, Index_Len, temp_cnt, temp_pos, File_Flag = 1;

	unsigned int *I_Index_Buf, *Buf_Temp;

	struct FramePosAdd
	{
		long nFrameNum;
		long nFilePos;
		struct FramePosAdd *pNext;
	} *FramePosHead, *FramePosTail, *FramePosTemp;
	printf("my add: --into MP4Sys_DecOpenBackUpFile--1--nPort %d--szFileName %s--IndexFileName %s--bFirstFile %d\n",
		nPort,szFileName,IndexFileName,bFirstFile);
	/*
	   参数检查 
	 */
	if ((nPort < 0) || (nPort >= MAX_CHANNEL_CNT))
	{
		SDK_DPF("[SDK] MP4Sys_DecCloseBackUpFile nPort = %d Err!\n", nPort);
		return FALSE;
	}

	while (g_BackupSDKResInfo[nPort].m_bSDKInitLock)
	{
		usleep(1000000);
		//SDK_DPF("[SDK] MP4Sys_InitDSPs: wait m_bSDKInitLock !!!!!!!!!!!\n");
		SDK_DPF("[SDK] ZWSys_Init: wait m_bSDKInitLock !!!!!!!!!!!\n");
	}
	g_BackupSDKResInfo[nPort].m_bSDKInitLock = TRUE;
	if (g_BackupSDKResInfo[nPort].m_bSDKInit == SDK_INIT_MAGIC)
	{
		g_BackupSDKResInfo[nPort].m_bSDKInitLock = FALSE;
		return FALSE;
	}

	if (szFileName == NULL)
	{
		SDK_DPF("[SDK] MP4Sys_DecOpenBackUpFile szFileName == NULL\n");
		g_BackupSDKResInfo[nPort].m_bSDKInit = 0;
		g_BackupSDKResInfo[nPort].m_bSDKInitLock = FALSE;
		return FALSE;
	}
	pBackCh = &g_hFileBackUp[nPort];
	memset((char *) pBackCh, 0, sizeof (CFileBackUp_t));
	pthread_mutex_init(&pBackCh->m_Lock, NULL);

	pthread_mutex_lock(&pBackCh->m_Lock);

	/*
	   分配资源，纯粹的数据缓冲空间，没有必要初始化 
	 */
	if (pBackCh->pFrameData == NULL)
		pBackCh->pFrameData = (char *) malloc(MAX_FRMAESIZE);	//帧数据空间
	if (pBackCh->pFrameData == NULL)
	{
		SDK_DPF("[SDK] MP4Sys_DecOpenBackUpFile malloc Fail\n");
		goto MP4Sys_DecOpenBackUpFile_Err;
	}

	/*
	   关闭先前文件 
	 */
	pBackCh->m_bFileOK = FALSE;
	HIEASY_FCLOSE(pBackCh->m_File);

	/*
	   打开文件
	 */
	pBackCh->m_File = fopen(szFileName, "rb");
	if (pBackCh->m_File == NULL)
	{
		SDK_DPF("[SDK] MP4Sys_DecOpenBackUpFile open %s Fail\n", szFileName);
		goto MP4Sys_DecOpenBackUpFile_Err;
	}

	/*
	   检察文件Size 
	 */
	pBackCh->m_dwFileSize = Higheasy_GetFileLen(pBackCh->m_File);
	if (pBackCh->m_dwFileSize < 4096)
	{
		SDK_DPF("[SDK] MP4Sys_DecOpenBackUpFile FileSize too small\n");
		goto MP4Sys_DecOpenBackUpFile_Err;
	}

	/*
	   检察文件头 
	 */
	ret = fread(&pBackCh->m_FileHead, 1, FILE_HEAD_LENTH, pBackCh->m_File);
	if(ret != FILE_HEAD_LENTH)
	{
		SDK_DPF("[SDK] fread %s fail \n",szFileName);
		goto MP4Sys_DecOpenBackUpFile_Err;
	}
	if (pBackCh->m_FileHead.FileStartCode != 0xB6010000)
	{
		SDK_DPF("[SDK] MP4Sys_DecOpenBackUpFile FileStartCode Err\n");
		goto MP4Sys_DecOpenBackUpFile_Err;
	}

	/*
	   先检查有没有索引，如果有则直接使用，如果没有则重建索引
	 */
	//当前打开文件是否是正在录像文件
	File_Flag = strcmp(szFileName, Index_File_Name);
	SDK_DPF("my add: File_Flag = %d\n",File_Flag);
	if (File_Flag == 0)			//当前文件还没有索引，正在录像
	{
		CurPos = ftell(pBackCh->m_File);	//得到当前文件指针位置
		memcpy(Temp_Index, I_Frame_Index, 1800 * sizeof (Index_File));
		FramePosHead = NULL;
		FramePosTail = NULL;
		FramePosTemp = NULL;
		i = 0;
		while (1)
		{
			FramePosTemp = malloc(sizeof (struct FramePosAdd));
			if (FramePosHead == NULL)
			{
				FramePosHead = FramePosTemp;
				FramePosTail = FramePosHead;
			}
			else
			{
				FramePosTail->pNext = FramePosTemp;
				FramePosTail = FramePosTemp;
			}
			temp_cnt = Temp_Index[i].I_Frame_Num;	//第几个I帧，与之前的索引相比要减1
			temp_pos = Temp_Index[i].I_FRAME_Pos;	//I帧对应在文件中的pos
			FramePosTail->nFrameNum = pBackCh->nIFrameNumber;	//temp_cnt-1;//pBackCh->nIFrameNumber;
			FramePosTail->nFilePos = temp_pos;
			FramePosTail->pNext = NULL;
			pBackCh->nIFrameNumber += 1;
			if (temp_cnt == 1)
				pBackCh->m_nFirstIFramePos = temp_pos;
			i++;
			if (Temp_Index[i].I_Frame_Num == 0 || Temp_Index[i].I_FRAME_Pos == 0)
			{
				break;
			}
		}
		fseek(pBackCh->m_File, CurPos, SEEK_SET);
		/*
		   搜寻第一个I帧,确认代码正确之后将这里修改成fseek，直接读出时间戳 
		 */
		RetValue =
			Higheasy_FR_FindFirstFrame(pBackCh->m_File, &pBackCh->m_FirstIFrameNumStamp, &pBackCh->m_FirstIFrameTimeStamp,
									   &pBackCh->m_nFirstIFramePos);
	}
	else						//分为已经录像文件有没有索引
	{
		CurPos = ftell(pBackCh->m_File);	//得到当前文件指针位置
		fseek(pBackCh->m_File, -8, SEEK_END);
		//Index_Len 索引的长度，包含了最后2个标志的长度
		fread(&Index_Len, 1, 4, pBackCh->m_File);
		fread(&Index_flag, 1, 4, pBackCh->m_File);
		SDK_DPF("my add:Index_Len %d,Index_flag = 0x%x\n", Index_Len, Index_flag);
		if ((Index_flag == Index_End_Flag) && (Index_Len > 1))
		{
			FramePosHead = NULL;
			FramePosTail = NULL;
			FramePosTemp = NULL;
			I_Index_Buf = malloc(I_Frame_Index_Num * 2 * 4);
			Buf_Temp = I_Index_Buf;
			Index_Len *= 8;
			fseek(pBackCh->m_File, -Index_Len, SEEK_END);
			//将视频文件中的索引取出
			fread((void *) I_Index_Buf, 1, Index_Len - 8, pBackCh->m_File);
			Index_Len -= 8;
			while (1)
			{
				if (Index_Len <= 0)
				{
					break;
				}
				FramePosTemp = malloc(sizeof (struct FramePosAdd));
				if (FramePosHead == NULL)
				{
					FramePosHead = FramePosTemp;
					FramePosTail = FramePosHead;
				}
				else
				{
					FramePosTail->pNext = FramePosTemp;
					FramePosTail = FramePosTemp;
				}
				temp_cnt = *I_Index_Buf;	//第几个I帧，与之前的索引相比要减1
				temp_pos = *(I_Index_Buf + 1);	//I帧对应在文件中的pos
				FramePosTail->nFrameNum = pBackCh->nIFrameNumber;	//temp_cnt-1;//pBackCh->nIFrameNumber;
				FramePosTail->nFilePos = temp_pos;
				FramePosTail->pNext = NULL;
				pBackCh->nIFrameNumber += 1;	// for test
				if (temp_cnt == 1)
					pBackCh->m_nFirstIFramePos = temp_pos;
				I_Index_Buf += 2;	//每取出一个I帧的信息，则自动加2
				Index_Len -= 8;	//每取出一个I帧信息，则总长度减8
			}
			fseek(pBackCh->m_File, CurPos, SEEK_SET);
			free(Buf_Temp);
			/*
			   搜寻第一个I帧 
			 */
			RetValue = Higheasy_FR_FindFirstFrame(pBackCh->m_File, &pBackCh->m_FirstIFrameNumStamp, &pBackCh->m_FirstIFrameTimeStamp,
										   &pBackCh->m_nFirstIFramePos);
		}
		else
		{
			FramePosHead = NULL;
			FramePosTail = NULL;
			FramePosTemp = NULL;
			fseek(pBackCh->m_File, CurPos, SEEK_SET);
			CurPos = ftell(pBackCh->m_File);
			/*
			   搜寻第一个I帧 
			 */
			RetValue = Higheasy_FR_FindFirstFrame(pBackCh->m_File, &pBackCh->m_FirstIFrameNumStamp, &pBackCh->m_FirstIFrameTimeStamp,
										   &pBackCh->m_nFirstIFramePos);
			SDK_DPF("my add: Higheasy_FR_FindFirstFrame return %d \n",RetValue);
			if (RetValue != 0)
			{
				SDK_DPF("[SDK] MP4Sys_DecOpenBackUpFile 搜寻第一个I帧失败\n");
				goto MP4Sys_DecOpenBackUpFile_Err;
			}

			while (1)
			{
				if (Higheasy_FR_FindNextFrame(pBackCh->m_File, I_FRAME) < 0)
					break;
				IFramePos = ftell(pBackCh->m_File);
				fread((char *) &pBackCh->m_FrameHead, 1, sizeof (H264DATAPACKETHEADER), pBackCh->m_File);
				fseek(pBackCh->m_File, pBackCh->m_FrameHead.nOverloadLen, SEEK_CUR);
				fread((char *) &StartCodeTmp, 1, 4, pBackCh->m_File);
				if (StartCodeTmp != STARTCODE)
				{
					SearchIPos = ftell(pBackCh->m_File) - 4;
					fseek(pBackCh->m_File, -(pBackCh->m_FrameHead.nOverloadLen + sizeof (H264DATAPACKETHEADER)), SEEK_CUR);
					if (Higheasy_FR_FindNextFrame(pBackCh->m_File, 0xFF) == 0)
					{
						CurPos = ftell(pBackCh->m_File);
						if (CurPos < SearchIPos)
						{
							continue;
						}
					}
					fseek(pBackCh->m_File, SearchIPos, SEEK_SET);
				}
				else
					fseek(pBackCh->m_File, -4, SEEK_CUR);
				FramePosTemp = malloc(sizeof (struct FramePosAdd));
				if (FramePosHead == NULL)
				{
					FramePosHead = FramePosTemp;
					FramePosTail = FramePosHead;
				}
				else
				{
					FramePosTail->pNext = FramePosTemp;
					FramePosTail = FramePosTemp;
				}
				FramePosTail->nFrameNum = pBackCh->nIFrameNumber;
				FramePosTail->nFilePos = IFramePos;
				FramePosTail->pNext = NULL;
				pBackCh->nIFrameNumber += 1;
			}
		}
	}
	SDK_DPF("my add: nIFrameNumber = %ld \n",pBackCh->nIFrameNumber);
	if (pBackCh->nIFrameNumber != 0)
	{
		pBackCh->IFrameIndex = (int *) malloc(pBackCh->nIFrameNumber * 4);
		if (pBackCh->IFrameIndex == NULL)
			goto MP4Sys_DecOpenBackUpFile_Err;
	}
	while (FramePosHead != NULL)
	{
		pBackCh->IFrameIndex[FramePosHead->nFrameNum] = FramePosHead->nFilePos;
		FramePosTemp = FramePosHead;
		FramePosHead = FramePosTemp->pNext;
		HIEASY_FREE(FramePosTemp);
	}

	/*
	   将文件指针指向第一个I帧 
	 */
	fseek(pBackCh->m_File, pBackCh->m_nFirstIFramePos, SEEK_SET);

	pBackCh->m_bValidIFrame = FALSE;
	pBackCh->m_bDiscardErrFrame = FALSE;
	pBackCh->m_bFileEnd = FALSE;
	pBackCh->m_bFileOK = TRUE;

	/*
	   为了让不同文件无缝连接起来，新生成时间戳 
	 */
	if (bFirstFile)
	{
		pBackCh->m_CurAudioTimeStamp = 40 * 90;	//所有备份文件的音频累积时间戳
		pBackCh->m_CurVideoTimeStamp = 40 * 90;	//所有备份文件的视频累积时间戳

		pBackCh->m_CurAudioNumStamp = 1000;	//所有备份文件的音频累积帧号戳
		pBackCh->m_CurVideoNumStamp = 1000;	//所有备份文件的视频累积帧号戳
	}
	else
	{
		pBackCh->m_CurAudioTimeStamp += 40 * 90;	//所有备份文件的音频累积时间戳
		pBackCh->m_CurVideoTimeStamp += 40 * 90;	//所有备份文件的视频累积时间戳

		pBackCh->m_CurAudioNumStamp++;	//所有备份文件的音频累积帧号戳
		pBackCh->m_CurVideoNumStamp++;	//所有备份文件的视频累积帧号戳
	}

	pBackCh->m_LastVideoTimeStamp = 0;
	pBackCh->m_LastAudioTimeStamp = 0;
	pBackCh->m_LastVideoNumStamp = 0;
	pBackCh->m_LastAudioNumStamp = 0;

	pthread_mutex_unlock(&pBackCh->m_Lock);
	g_BackupSDKResInfo[nPort].m_bSDKInit = SDK_INIT_MAGIC;
	g_BackupSDKResInfo[nPort].m_bSDKInitLock = FALSE;
	return TRUE;

  MP4Sys_DecOpenBackUpFile_Err:
  	SDK_DPF("my add: -----------MP4Sys_DecOpenBackUpFile-----------\n");
	HIEASY_FCLOSE(pBackCh->m_File) HIEASY_FREE(pBackCh->pFrameData) HIEASY_FREE(pBackCh->IFrameIndex) pthread_mutex_unlock(&pBackCh->m_Lock);
	g_BackupSDKResInfo[nPort].m_bSDKInit = 0;
	g_BackupSDKResInfo[nPort].m_bSDKInitLock = FALSE;
	return FALSE;
}

//-----------------------------------------------------------------------------
//      说明：          关闭需要备份的文件
//      输入参数：      nPort                   对应通道编号
//      返回:           成功或失败
//      注意：          为了兼容回放库，引入nPort参数
//-----------------------------------------------------------------------------
int MP4Sys_DecCloseBackUpFile(int nPort)
{
	CFileBackUp_t *pBackCh;

	/*
	   参数检查 
	 */
	if ((nPort < 0) || (nPort >= MAX_CHANNEL_CNT))
	{
		SDK_DPF("[SDK] MP4Sys_DecCloseBackUpFile nPort = %d Err!\n", nPort);
		return FALSE;
	}
	if (g_BackupSDKResInfo[nPort].m_bSDKInit != SDK_INIT_MAGIC)
		return FALSE;

	pBackCh = &g_hFileBackUp[nPort];

	pthread_mutex_lock(&pBackCh->m_Lock);

	HIEASY_FCLOSE(pBackCh->m_File);
	HIEASY_FREE(pBackCh->pFrameData);
	HIEASY_FREE(pBackCh->IFrameIndex) pBackCh->m_bFileOK = FALSE;
	pthread_mutex_unlock(&pBackCh->m_Lock);
	pthread_mutex_destroy(&pBackCh->m_Lock);
	memset((char *) pBackCh, 0, sizeof (CFileBackUp_t));
	g_BackupSDKResInfo[nPort].m_bSDKInit = 0;
	return TRUE;
}

//-----------------------------------------------------------------------------
//      说明：          将文件指针移到指定时间前的I帧
//      输入参数：      nPort                   对应通道编号
//      输入参数：      TimeSec                 相对于文件第一个I帧的时间(单位为秒)
//      返回:           成功或失败
//      注意：          当time大于文件时间长度时，文件指针指向最后一个I帧
//-----------------------------------------------------------------------------
int MP4Sys_DecSetBackUpFileTime(int nPort, unsigned int TimeSec)
{
	TagFrame_t FrameInfo;

	CFileBackUp_t *pBackCh;

	int RetValue = TRUE;

	int i, CurPos;

	/*
	   参数检查 
	 */
	if ((nPort < 0) || (nPort >= MAX_CHANNEL_CNT))
	{
		SDK_DPF("[SDK] MP4Sys_DecCloseBackUpFile nPort = %d Err!\n", nPort);
		return FALSE;
	}
	if (g_BackupSDKResInfo[nPort].m_bSDKInit != SDK_INIT_MAGIC)
		return FALSE;

	pBackCh = &g_hFileBackUp[nPort];

	pthread_mutex_lock(&pBackCh->m_Lock);
	if (!pBackCh->m_bFileOK)
	{
		SDK_DPF("[SDK] CH_%d MP4Sys_DecSetBackUpFileTime File Is not OK\n", nPort);
		RetValue = FALSE;
		goto MP4Sys_DecSetBackUpFileTime_Tag0;
	}

	if (TimeSec == 0)
		fseek(pBackCh->m_File, pBackCh->m_nFirstIFramePos, SEEK_SET);
	else
	{
		if (Higheasy_FR_FindFrameByTime(pBackCh->m_File, pBackCh->m_FirstIFrameTimeStamp, TimeSec * 1000, &FrameInfo) == 0)
		{
			fseek(pBackCh->m_File, FrameInfo.FramePos, SEEK_SET);
		}
		else
		{
			SDK_DPF("[SDK] MP4Sys_DecSetBackUpFileTime Find I Frame Fail\n");
			fseek(pBackCh->m_File, pBackCh->m_nFirstIFramePos, SEEK_SET);
		}
	}

	pBackCh->m_bValidIFrame = FALSE;
	pBackCh->m_bDiscardErrFrame = FALSE;

	CurPos = ftell(pBackCh->m_File);
	for (i = 0; i < pBackCh->nIFrameNumber; i++)
	{
		if (pBackCh->IFrameIndex[i] > CurPos)
			break;
	}
	pBackCh->m_PreIFrameNo = i - 1;

  MP4Sys_DecSetBackUpFileTime_Tag0:
  	SDK_DPF("my add: -----------MP4Sys_DecSetBackUpFileTime-----------\n");
	pthread_mutex_unlock(&pBackCh->m_Lock);
	return RetValue;
}

//-----------------------------------------------------------------------------
//      说明：          获取备份文件的文件头
//      输入参数：      nPort                   对应通道编号
//      输出参数：      pFileHead               文件头地址
//      输出参数：      FileHeadLenth   文件头长度
//      返回:           成功或失败
//      注意：          为了兼容回放库，引入nPort参数
//-----------------------------------------------------------------------------
int MP4Sys_DecGetFileHead(int nPort, char **pFileHead, int *FileHeadLenth)
{
	CFileBackUp_t *pBackCh;

	int RetValue = TRUE;

	/*
	   参数检查 
	 */
	if ((nPort < 0) || (nPort >= MAX_CHANNEL_CNT))
	{
		SDK_DPF("[SDK] MP4Sys_DecCloseBackUpFile nPort = %d Err!\n", nPort);
		return FALSE;
	}
	if (g_BackupSDKResInfo[nPort].m_bSDKInit != SDK_INIT_MAGIC)
		return FALSE;

	pBackCh = &g_hFileBackUp[nPort];

	pthread_mutex_lock(&pBackCh->m_Lock);
	if (!pBackCh->m_bFileOK)
	{
		*pFileHead = NULL;
		*FileHeadLenth = 0;
		SDK_DPF("[SDK] MP4Sys_DecGetFileHead File Fail, Please OpenBackUpFile First\n");
		RetValue = FALSE;
	}
	else
	{
		*pFileHead = (char *) &pBackCh->m_FileHead;
		*FileHeadLenth = FILE_HEAD_LENTH;
	}

	pthread_mutex_unlock(&pBackCh->m_Lock);
	return RetValue;
}

//-----------------------------------------------------------------------------
//      说明：          获取指定时间段的文件大小，用来计算备份文件的大小
//      输入参数：      sFileName               文件名
//      输入参数：      StartSec                起始时间(相对于文件的第一个I帧)
//      输入参数：      StopSec                 结束时间(相对于文件的第一个I帧，如果需要直接计算文件尾可以设置为-1)
//      输出参数：      pBackSize               需要备份的数据大小
//      返回:           成功或失败
//      注意：          为了比较准确的计算备份文件的大小引入该函数，不能保证绝对精确
//-----------------------------------------------------------------------------
int MP4Sys_DecGetFrameSize(char *sFileName, unsigned int StartSec, unsigned int StopSec, int *pBackSize)
{
	FILE *file = NULL;

	int FileSize;

	MPEG4FILEHEADER FileHead;	//文件头

	H264DATAPACKETHEADER NewFrameHead;

	unsigned int FirstIFrameTimeStamp, FirstIFrameNumStamp;
	int FirstIFramePos;

	TagFrame_t FrameInfo;

	int StartPos, dwRet, len;

	unsigned int FrameInterVal;

	/*
	   参数检查 
	 */
	if (sFileName == NULL)
	{
		SDK_DPF("[SDK] MP4Sys_DecGetFramePos sFileName = NULL\n");
		return FALSE;
	}
	if (pBackSize == NULL)
	{
		SDK_DPF("[SDK] MP4Sys_DecGetFramePos pBackSize = NULL\n");
		return FALSE;
	}
	if (StopSec <= StartSec)
	{
		*pBackSize = 0;
		SDK_DPF("[SDK] MP4Sys_DecGetFramePos StopSec <= StartSec\n");
		return FALSE;
	}

	/*
	   打开文件
	 */
	file = fopen(sFileName, "rb");
	if (file == NULL)
	{
		SDK_DPF("[SDK] MP4Sys_DecGetFramePos open %s Fail\n", sFileName);
		goto MP4Sys_DecGetFrameSize_Err;
	}

	/*
	   检察文件Size 
	 */
	FileSize = Higheasy_GetFileLen(file);
	if (FileSize < 4096)
	{
		SDK_DPF("[SDK] MP4Sys_DecGetFramePos FileSize too small\n");
		goto MP4Sys_DecGetFrameSize_Err;
	}

	/*
	   检察文件头 
	 */
	fread(&FileHead, 1, FILE_HEAD_LENTH, file);
	if (FileHead.FileStartCode != 0xB6010000)
	{
		SDK_DPF("[SDK] MP4Sys_DecGetFramePos FileStartCode Err\n");
		goto MP4Sys_DecGetFrameSize_Err;
	}

	/*
	   搜寻第一个I帧 
	 */
	if (Higheasy_FR_FindFirstFrame(file, &FirstIFrameNumStamp, &FirstIFrameTimeStamp, &FirstIFramePos) != 0)
	{
		SDK_DPF("[SDK] MP4Sys_DecGetFrameSize 搜寻第一个I帧失败\n");
		goto MP4Sys_DecGetFrameSize_Err;
	}
	fseek(file, FirstIFramePos, SEEK_SET);

	/*
	   搜寻起始备份的I帧 
	 */
	if (StartSec == 0)
		StartPos = FirstIFramePos;
	else
	{
		if (Higheasy_FR_FindFrameByTime_New(file, FirstIFrameTimeStamp, StartSec, &FrameInfo) != 0)
		{
			SDK_DPF("[SDK] MP4Sys_DecSetBackUpFileTime Find Start I Frame Fail\n");
			StartPos = FirstIFramePos;
		}
		else
			StartPos = FrameInfo.FramePos;
	}
	/*
	   搜寻结束备份的视频帧 
	 */
	if (StopSec == -1)
		*pBackSize = FileSize - StartPos;
	else
	{
		if (Higheasy_FR_FindFrameByTime_New(file, FirstIFrameTimeStamp, StopSec, &FrameInfo) != 0)
		{
			SDK_DPF("[SDK] MP4Sys_DecSetBackUpFileTime Find Stop I Frame Fail\n");
			*pBackSize = FileSize - StartPos;
		}
		else
		{
			/*
			   搜寻离结束时间最近的视频帧位置 
			 */
			if (FrameInfo.FramePos > StartPos)
				fseek(file, FrameInfo.FramePos, SEEK_SET);
			else
				fseek(file, StartPos, SEEK_SET);
			while (1)
			{
				len = FRAME_HEAD_LENTH + 4;
				dwRet = fread(&NewFrameHead, 1, len, file);

				/*
				   读到文件尾 
				 */
				if (dwRet != len)
					break;

				/*
				   搜寻有效的帧头 
				 */
				if (NewFrameHead.DataPacketStartCode != STARTCODE)
				{
					fseek(file, -len, SEEK_CUR);
					if (Higheasy_FR_FindNextFrame(file, 0xFF) != 0)
						break;

					dwRet = fread(&NewFrameHead, 1, len, file);
				}

				if ((NewFrameHead.FrameType != I_FRAME) && (NewFrameHead.FrameType != P_FRAME))
				{
					fseek(file, NewFrameHead.nOverloadLen, SEEK_CUR);
					continue;
				}

				/*
				   比较和结束时间的差值 
				 */
				//  COUNT_REAL_INTERVAL(FrameInterVal, FrameHead.ShowTime, FirstIFrameTimeStamp);
				//FrameInterVal = NewFrameHead.Vsecond;
				FrameInterVal = NewFrameHead.u32RealTimeSec;
				if (FrameInterVal > (StopSec))
					break;
				else
					fseek(file, NewFrameHead.nOverloadLen, SEEK_CUR);
			}
			*pBackSize = ftell(file) - StartPos;
		}
	}

	HIEASY_FCLOSE(file);

	return TRUE;

  MP4Sys_DecGetFrameSize_Err:
  	SDK_DPF("my add: -----------MP4Sys_DecGetFrameSize-----------\n");
	*pBackSize = 0;
	HIEASY_FCLOSE(file);
	return FALSE;
}

//-----------------------------------------------------------------------------
//      说明：          从备份文件中读取一帧数据
//      返回:           成功或失败(失败表示读到文件尾)
//      注意：          为了兼容回放库，引入nPort参数
//-----------------------------------------------------------------------------
int MP4Sys_DecReadOneFrame(int nPort,	//通道编号
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
	int dwRet;

	unsigned int FrameInterval;

	H264DATAPACKETHEADER DataPacketNext;

	H264DATAPACKETHEADER *pNextHead;

	H264DATAPACKETHEADER *pCurHead;

	int CurFramePos, NextFramePos;

	CFileBackUp_t *pBackCh;

	int bDropFrame, ret;
//printf("my add: --1--\n");
	/*
	   参数检查 
	 */
	if ((nPort < 0) || (nPort >= MAX_CHANNEL_CNT))
	{
		SDK_DPF("[SDK] MP4Sys_DecCloseBackUpFile nPort = %d Err!\n", nPort);
		return FALSE;
	}
	if (g_BackupSDKResInfo[nPort].m_bSDKInit != SDK_INIT_MAGIC)
	{
		return FALSE;
	}

	pBackCh = &g_hFileBackUp[nPort];

	pthread_mutex_lock(&pBackCh->m_Lock);
	if (!pBackCh->m_bFileOK)
	{
		goto MP4Sys_DecReadOneFrame_Tag0;
	}
	if (pBackCh->m_bFileEnd)
	{
		goto MP4Sys_DecReadOneFrame_Tag0;
	}
	//	printf("my add: --2--\n");
	if (NULL == pBackCh->IFrameIndex)
	{
		printf("my add: IFrameIndex == NULL \n");
	}
	if (nReadDirection == 1)
	{
		if (pBackCh->m_PreIFrameNo > 0)
			fseek(pBackCh->m_File, pBackCh->IFrameIndex[pBackCh->m_PreIFrameNo - 1], SEEK_SET);
		else
		{
			goto MP4Sys_DecReadOneFrame_Tag0;
		}
	}
	else if (nReadFrameType == 1)
	{
		if (pBackCh->m_PreIFrameNo < (pBackCh->nIFrameNumber - 1))
			fseek(pBackCh->m_File, pBackCh->IFrameIndex[pBackCh->m_PreIFrameNo + 1], SEEK_SET);
		else
		{
			goto MP4Sys_DecReadOneFrame_Tag0;
		}
	}
//	printf("my add: --3--\n");

	pCurHead = &pBackCh->m_FrameHead;

  BACKUP_READ_HEAD:
	ret = Higheasy_FR_ReadHead(pCurHead, pBackCh->m_File, 2);
	if (ret != 0)
	{
		goto MP4Sys_DecReadOneFrame_Tag0;
	}
//	printf("my add: --4--\n");

  BACKUP_CHECK_HEAD:
	/*
	   检测帧头的长度信息是否合法，只要出错就搜寻下一个I帧，宁缺勿滥 
	 */
	if ((pCurHead->nOverloadLen <= 0) || (pCurHead->nOverloadLen > MAX_FRMAESIZE))
	{
		SDK_DPF("[SDK] CH_%d MP4Sys_DecReadOneFrame FrameSize = %d Err, Pos = %d\n", (int) nPort, (int) pCurHead->nOverloadLen,
				(int) ftell(pBackCh->m_File) - FRAME_HEAD_LENTH);
		/*
		   只要出错就搜寻下一个I帧，宁缺勿滥 
		 */
		pBackCh->m_bDiscardErrFrame = TRUE;
		goto BACKUP_READ_HEAD;
	}
	CurFramePos = ftell(pBackCh->m_File) - FRAME_HEAD_LENTH;	//当前帧位置
	NextFramePos = 0;			//下一帧位置
	pNextHead = NULL;			//下一个帧头

	/*
	   检测下一帧的帧头信息是否合法，由于出错的机率比较小，预先读取当前帧的有效数据 
	 */
	dwRet = fread(pBackCh->pFrameData + FRAME_HEAD_LENTH, 1, pCurHead->nOverloadLen, pBackCh->m_File);
	if (dwRet == pCurHead->nOverloadLen)
	{
		dwRet = fread(&DataPacketNext, 1, FRAME_HEAD_LENTH, pBackCh->m_File);
		if (dwRet == FRAME_HEAD_LENTH)
		{
			NextFramePos = CurFramePos + FRAME_HEAD_LENTH + pCurHead->nOverloadLen;
			pNextHead = &DataPacketNext;
		}
	}
	else
	{
		goto MP4Sys_DecReadOneFrame_Tag0;
	}
//	printf("my add: --5--\n");

	/*
	   如果下一帧头信息非法，说明当前帧长度异常，重新搜寻下一个I帧，宁缺勿滥 
	 */
	if (pNextHead != NULL)
	{
		if (pNextHead->DataPacketStartCode != STARTCODE)
		{
			/*
			   软件处理录像文件时按照16K作对其,文件结束时可能会有很多0 
			 */
			if ((pNextHead->DataPacketStartCode == 0) && (pNextHead->FrameNo == 0) && (pNextHead->ShowTime == 0) && (pNextHead->nOverloadLen == 0))
			{
				SDK_DPF("[SDK] CH_%d: File End All zero !!, haha !!\n", nPort);
				pNextHead = NULL;
				pBackCh->m_bFileEnd = TRUE;
			}
			else
			{
				SDK_DPF("[SDK] CH_%d: FrameErr, FramePos1 = %d, FramePos2 = %d\n", nPort, CurFramePos, NextFramePos);
				SDK_DPF("[SDK] ErrType = %x, ErrNum = %x, ErrTime = %x\n", pCurHead->FrameType, pCurHead->FrameNo, pCurHead->ShowTime);
				fseek(pBackCh->m_File, CurFramePos + FRAME_HEAD_LENTH, SEEK_SET);
				pBackCh->m_bDiscardErrFrame = TRUE;
				goto BACKUP_READ_HEAD;
			}
		}
	}
//	printf("my add: --6--\n");

	if (pCurHead->FrameType == I_FRAME)
	{
		int CurIFrame = pBackCh->m_PreIFrameNo;

		int CurPos = ftell(pBackCh->m_File);

		if (nReadDirection == 1)
			CurIFrame -= 1;
		else
			CurIFrame += 1;
		printf("my add: --a--CurIFrame=%d CurPos=%d \n",CurIFrame, CurPos);
		while (pBackCh->IFrameIndex[CurIFrame + 1] < CurPos)
		{
			CurIFrame += 1;
			if (CurIFrame >= (pBackCh->nIFrameNumber - 1))
				break;
		}
		printf("my add: --b--\n");
		pBackCh->m_PreIFrameNo = CurIFrame;
		pBackCh->m_bValidIFrame = TRUE;
		pBackCh->m_bDiscardErrFrame = FALSE;
		pBackCh->m_LastValidIFrameNum = pCurHead->FrameNo;
		pBackCh->m_LastValidIFrameTime = pCurHead->ShowTime;
	}
//	printf("my add: --7--\n");

	bDropFrame = FALSE;
	if (pCurHead->FrameType != AUDIO_FRAME)	//没有找到合法I帧，继续丢
	{
		if (!pBackCh->m_bValidIFrame || pBackCh->m_bDiscardErrFrame)
		{
			SDK_DPF("[SDK] CH_%d: Back File Discard This Frame Until Next I Frame, Pos = %d\n", (int) nPort, (int) CurFramePos);
			bDropFrame = TRUE;
		}
	}
	else if (bThrowAudio)
		bDropFrame = TRUE;

	if (bDropFrame)
	{
		if (pNextHead != NULL)
		{
			memcpy((char *) pCurHead, (char *) pNextHead, sizeof (H264DATAPACKETHEADER));
			goto BACKUP_CHECK_HEAD;
		}
		else					//当前帧错误,又没有下一帧,认为文件结束
		{
			goto MP4Sys_DecReadOneFrame_Tag0;
		}
	}
//	printf("my add: --8--\n");

	/*
	   修改音频时间戳、帧号戳 
	 */
	if (pCurHead->FrameType == AUDIO_FRAME)
	{
		/*
		   修改时间戳 
		 */
		if (pBackCh->m_LastAudioTimeStamp != 0)
		{
			COUNT_REAL_INTERVAL(FrameInterval, pCurHead->ShowTime, pBackCh->m_LastAudioTimeStamp);	//计算和上一帧的时间差
			pBackCh->m_CurAudioTimeStamp += FrameInterval;
		}
		pBackCh->m_LastAudioTimeStamp = pCurHead->ShowTime;
		pCurHead->ShowTime = pBackCh->m_CurAudioTimeStamp | 1;	//(不能为0，否则PC回放库认为错误)

		/*
		   修改帧号戳 
		 */
		if (pBackCh->m_LastAudioNumStamp != 0)
		{
			COUNT_REAL_INTERVAL(FrameInterval, pCurHead->FrameNo, pBackCh->m_LastAudioNumStamp);	//计算和上一帧的帧号差
			pBackCh->m_CurAudioNumStamp += FrameInterval;
		}
		pBackCh->m_LastAudioNumStamp = pCurHead->FrameNo;
		pCurHead->FrameNo = pBackCh->m_CurAudioNumStamp;
	}
//	printf("my add: --9--\n");

	/*
	   如果是视频帧，计算该帧相对于文件第一个I帧的时间之后再修改时间戳、帧号戳 
	 */
	if ((pCurHead->FrameType == I_FRAME) || (pCurHead->FrameType == P_FRAME))
	{
		/*
		   计算和文件第一个I帧的时间差 
		 */
		COUNT_REAL_INTERVAL(FrameInterval, pCurHead->ShowTime, pBackCh->m_FirstIFrameTimeStamp);
		*FrameTime = FrameInterval / 90;

		/*
		   修改时间戳 
		 */
		if (pBackCh->m_LastVideoTimeStamp != 0)
		{
			COUNT_REAL_INTERVAL(FrameInterval, pCurHead->ShowTime, pBackCh->m_LastVideoTimeStamp);	//计算和上一帧的时间差
			pBackCh->m_CurVideoTimeStamp += FrameInterval;
		}
		pBackCh->m_LastVideoTimeStamp = pCurHead->ShowTime;
		pCurHead->ShowTime = pBackCh->m_CurVideoTimeStamp | 1;	//(不能为0，否则PC回放库认为错误)

		/*
		   修改帧号戳 
		 */
		if (pBackCh->m_LastVideoNumStamp != 0)
		{
			COUNT_REAL_INTERVAL(FrameInterval, pCurHead->FrameNo, pBackCh->m_LastVideoNumStamp);	//计算和上一帧的帧号差
			pBackCh->m_CurVideoNumStamp += FrameInterval;
		}
		pBackCh->m_LastVideoNumStamp = pCurHead->FrameNo;
		pCurHead->FrameNo = pBackCh->m_CurVideoNumStamp;
	}
//	printf("my add: --10--\n");

	*pFrameHead = (char *) &pBackCh->m_FrameHead;	//帧头地址
	*pnFrameHeadLenth = FRAME_HEAD_LENTH;	//帧头长度
	*pnFrameType = pBackCh->m_FrameHead.FrameType;	//帧类型

	*pFrameData = pBackCh->pFrameData;	//帧数据地址
	memcpy(*pFrameData, (void *) &pBackCh->m_FrameHead, FRAME_HEAD_LENTH);

	*pnFrameDataLenth = pBackCh->m_FrameHead.nOverloadLen + FRAME_HEAD_LENTH;	//帧数据长度
	fseek(pBackCh->m_File, CurFramePos + FRAME_HEAD_LENTH + pCurHead->nOverloadLen, SEEK_SET);

	pthread_mutex_unlock(&pBackCh->m_Lock);
	return TRUE;
	printf("my add: --11--\n");

  MP4Sys_DecReadOneFrame_Tag0:
  	SDK_DPF("my add: -----------MP4Sys_DecReadOneFrame-----------\n");
	pBackCh->m_bFileEnd = TRUE;
	pthread_mutex_unlock(&pBackCh->m_Lock);
	return FALSE;
}
