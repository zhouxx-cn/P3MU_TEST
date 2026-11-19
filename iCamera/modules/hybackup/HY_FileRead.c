
/*!
 *\file   HY_Fileread.c
 *\brief  文件备份的操作
 *\author jtx
 *\date	  2010/06/11
 *\version 0.1
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <sys/time.h>

#include "HY_FileRead.h"
#include "sys_banned.h"
/* 搜寻指定Buf中第一个头,BytesLen为0xFF时候，只搜寻有效帧头，否则搜寻指定类型的帧头 */
int Higheasy_SerchHead(char *pBuf, int BufSize, int *pFindPos, char Type)
{
	int nSerchLen = 0;

	int nCmpLen;

	int i, j;
	unsigned char ToFindBytes[5] = { 0x00, 0x00, 0x01, 0xB7, 0x00 };//根据帧头结构体成员顺序而定

	*pFindPos = 0;

	if (Type == 0xFF)			//不管帧类型
	{
		nSerchLen = 4;
		nCmpLen = 4;
	}
	else if (Type == 0xFE)		//I帧或者P帧
	{
		nSerchLen = 5;
		nCmpLen = 4;
	}
	else
	{
		nSerchLen = 5;
		nCmpLen = 5;
		ToFindBytes[4] = Type;	//搜索帧类型
	}

	if (BufSize < nSerchLen)
		return -1;

	for (i = 0; i < BufSize - (nSerchLen - 1); i++)
	{
		for (j = 0; j < nCmpLen; j++)
		{
			if(4 == j)//新流格式中，帧头里的成员顺序改变了
			{
				if( i < BufSize - (nSerchLen - 1) - 4)
					if (pBuf[i + 4 + j] != ToFindBytes[j])
						break;
			}
			else
				if (pBuf[i + j] != ToFindBytes[j])
					break;
		}

		if (j == nCmpLen)
		{
			if (Type == 0xFE)
			{
				if( i < BufSize - (nSerchLen - 1) - 4)
					if ((pBuf[i + j + 4] == I_FRAME) || (pBuf[i + j + 4] == P_FRAME))//加4是根据帧头中成员位置定的
					{
						*pFindPos = i;
						return 0;
					}
			}
			else
			{
				*pFindPos = i;
				return 0;
			}
			printf("my add: find frame type 0x%x ########################\n",Type);
		}
	}
	return -1;
}

 /*
    获取文件大小 
  */
int Higheasy_GetFileLen(FILE * file)
{
	int dwCurrentPos = ftell(file);

	fseek(file, 0, SEEK_END);
	int filesize = ftell(file);

	fseek(file, dwCurrentPos, SEEK_SET);
	return filesize;
}

/* 从当前文件指针开始搜寻下一个指定类型帧头,搜寻成功后文件指针落在新搜到的帧头位置 */
int Higheasy_FR_FindNextFrame(FILE * file, int nFrameType)
{
	char DataBuffer[4096];

	int lFindDataStartPos = 0;

	int dwRet = 0;

	while (1)
	{
		dwRet = fread(DataBuffer, 1, 4096, file);

		if (Higheasy_SerchHead(DataBuffer, dwRet, &lFindDataStartPos, nFrameType) != 0)
			fseek(file, -FRAME_HEAD_LENTH, SEEK_CUR);
		else
		{
			fseek(file, lFindDataStartPos - dwRet, SEEK_CUR);
			return 0;
		}

		if (dwRet < 4096)
			return -1;
	}
	return -1;
}

/* 搜寻第一个I帧的信息 */
int Higheasy_FR_FindFirstFrame(FILE * file, unsigned int *pFramNo, unsigned int *pFrameTime, int *pFramePos)
{
	int InitFilePos;

	int dwRet;

	H264DATAPACKETHEADER PacketHead;

	InitFilePos = ftell(file);	//保存文件指针的起始位置

	/*
	   从文件头结束之后开始查找 
	 */
	fseek(file, FILE_HEAD_LENTH, SEEK_SET);
	dwRet = fread(&PacketHead, 1, FRAME_HEAD_LENTH, file);
	if (dwRet != FRAME_HEAD_LENTH)
	{
		fseek(file, InitFilePos, SEEK_SET);
		return -1;
	}
	if ((PacketHead.DataPacketStartCode != STARTCODE) || (PacketHead.FrameType != I_FRAME))
	{
		fseek(file, -(long) dwRet, SEEK_CUR);
		if (Higheasy_FR_FindNextFrame(file, I_FRAME) != 0)
		{
			fseek(file, InitFilePos, SEEK_SET);
			return -1;
		}
		fread(&PacketHead, 1, FRAME_HEAD_LENTH, file);
	}

	*pFramNo = PacketHead.FrameNo;
	*pFrameTime = PacketHead.ShowTime;
	*pFramePos = ftell(file) - FRAME_HEAD_LENTH;
	fseek(file, InitFilePos, SEEK_SET);
	return 0;
}

/* 搜寻指定时间点的I帧信息 */
#if 1
int Higheasy_FR_FindFrameByTime_New(FILE * file, unsigned int FileStartTimeStamp, unsigned int PosTime, TagFrame_t * pFrameInfo)
{
	int dwMinPos, dwMaxPos, dwCurPos;

	int FileSize;

	char DataBuffer[4096];

	int lFindDataStartPos = 0;

	H264DATAPACKETHEADER DataPacket;

	H264DATAPACKETHEADER NewDataPacket;

	int i, j;

	int dwRet = 0;

	unsigned int nTimeTemp = PosTime;

	int InitFilePos;

	InitFilePos = ftell(file);	//保存文件指针的起始位置

	dwMinPos = 0;
	dwMaxPos = Higheasy_GetFileLen(file);
	FileSize = dwMaxPos;
	/*
	   确定定位搜索的次数 
	 */
	for (j = 1; j < 32; j++)
	{
		if ((FileSize >> j) < 4096)
			break;
	}

	/*
	   初步定位视频帧的位置 
	 */
	for (i = 0; i < j; i++)
	{
		dwCurPos = (dwMinPos + dwMaxPos) / 2;	//采用二分法来定位
		fseek(file, dwCurPos, SEEK_SET);
		while (dwCurPos < FileSize)
		{
			dwRet = fread(DataBuffer, 1, 4096, file);
			if (dwRet < 4096)
				break;

			if (Higheasy_SerchHead(DataBuffer, dwRet, &lFindDataStartPos, 0xFE) != 0)
			{
				fseek(file, -FRAME_HEAD_LENTH, SEEK_CUR);
				dwCurPos = ftell(file);
			}
			else
			{
				fseek(file, lFindDataStartPos - dwRet, SEEK_CUR);
				dwCurPos = ftell(file);
				fread(&NewDataPacket, 1, FRAME_HEAD_LENTH + 4, file);
				//  COUNT_REAL_INTERVAL(FrameInterVal, NewDataPacket.Vsecond, FileStartTimeStamp);
				//if (NewDataPacket.Vsecond >= nTimeTemp)
				if (NewDataPacket.u32RealTimeSec >= nTimeTemp)
					dwMaxPos = dwCurPos;
				else
					dwMinPos = dwCurPos;
				break;
			}
		}
		if (dwRet < 4096)
			break;
	}
	/*
	   经过上面搜寻之后，指定时间点的视频帧位于dwMinPos和dwMaxPos之间 
	 */
	/*
	   从dwMinPos开始往后搜，搜到第一个I帧返回 
	 */
	fseek(file, dwMaxPos, SEEK_SET);
	while (1)
	{
		dwRet = fread(DataBuffer, 1, 4096, file);
		if (Higheasy_SerchHead(DataBuffer, dwRet, &lFindDataStartPos, I_FRAME) != 0)
		{
			if (dwRet != 4096)
				break;
			else
				fseek(file, -FRAME_HEAD_LENTH, SEEK_CUR);
		}
		else
		{
			fseek(file, lFindDataStartPos - dwRet, SEEK_CUR);
			dwCurPos = ftell(file);
			dwRet = fread(&DataPacket, 1, FRAME_HEAD_LENTH, file);
			pFrameInfo->FramePos = dwCurPos;	//搜寻到的I帧的位置
			pFrameInfo->FrameNum = DataPacket.FrameNo;	//搜寻到的I帧的时间
			pFrameInfo->FrameTime = DataPacket.ShowTime;	//搜寻到的I帧的时间
			fseek(file, InitFilePos, SEEK_SET);
			return 0;
		}
	}

	/*
	   从dwMinPos开始往前搜，搜到第一个I帧返回 
	 */
	if (dwMinPos > 4096)
		fseek(file, dwMinPos - 4096, SEEK_SET);
	else
		fseek(file, 0, SEEK_SET);
	while (1)
	{
		dwRet = fread(DataBuffer, 1, 4096, file);
		if (Higheasy_SerchHead(DataBuffer, dwRet, &lFindDataStartPos, I_FRAME) != 0)
		{
			dwCurPos = ftell(file);
			if (dwCurPos > (4096 * 2 - FRAME_HEAD_LENTH))
				fseek(file, -(4096 * 2 - FRAME_HEAD_LENTH), SEEK_CUR);
			else if (dwCurPos > 4096)
				fseek(file, 0, SEEK_SET);
			else
				break;
		}
		else
		{
			fseek(file, lFindDataStartPos - dwRet, SEEK_CUR);
			dwCurPos = ftell(file);
			dwRet = fread(&DataPacket, 1, FRAME_HEAD_LENTH, file);
			pFrameInfo->FramePos = dwCurPos;	//搜寻到的I帧的位置
			pFrameInfo->FrameNum = DataPacket.FrameNo;	//搜寻到的I帧的时间
			pFrameInfo->FrameTime = DataPacket.ShowTime;	//搜寻到的I帧的时间
			fseek(file, InitFilePos, SEEK_SET);
			return 0;
		}
	}
	fseek(file, InitFilePos, SEEK_SET);
	return -1;
}
#else
#endif

int Higheasy_FR_FindFrameByTime(FILE * file, unsigned int FileStartTimeStamp, unsigned int PosTime, TagFrame_t * pFrameInfo)
{
	int dwMinPos, dwMaxPos, dwCurPos;

	int FileSize;

	char DataBuffer[4096];

	int lFindDataStartPos = 0;

	H264DATAPACKETHEADER DataPacket;

	int i, j;

	int dwRet = 0;

	unsigned int nTimeTemp = PosTime * 90;

	unsigned int FrameInterVal;

	int InitFilePos;

	InitFilePos = ftell(file);	//保存文件指针的起始位置

	dwMinPos = 0;
	dwMaxPos = Higheasy_GetFileLen(file);
	FileSize = dwMaxPos;

//      SDK_DPF(("[SDK] FileSize = %d\n", FileSize));

	/*
	   确定定位搜索的次数 
	 */
	for (j = 1; j < 32; j++)
	{
		if ((FileSize >> j) < 4096)
			break;
	}

//      SDK_DPF(("[SDK] j = %d\n", j));

	/*
	   初步定位视频帧的位置 
	 */
	for (i = 0; i < j; i++)
	{
		dwCurPos = (dwMinPos + dwMaxPos) / 2;	//采用二分法来定位
		fseek(file, dwCurPos, SEEK_SET);

//              SDK_DPF(("[SDK] i = %d, dwCurPos = %d, dwMinPos = %d, dwMaxPos = %d\n", i, dwCurPos, dwMinPos, dwMaxPos));

		while (dwCurPos < FileSize)
		{
			dwRet = fread(DataBuffer, 1, 4096, file);
			if (dwRet < 4096)
				break;

			if (Higheasy_SerchHead(DataBuffer, dwRet, &lFindDataStartPos, 0xFE) != 0)
			{
//                              SDK_DPF(("[SDK] Higheasy_SerchHead Fail dwRet = %d\n", dwRet));
				fseek(file, -FRAME_HEAD_LENTH, SEEK_CUR);
				dwCurPos = ftell(file);
			}
			else
			{
				fseek(file, lFindDataStartPos - dwRet, SEEK_CUR);
				dwCurPos = ftell(file);
				fread(&DataPacket, 1, FRAME_HEAD_LENTH, file);
				COUNT_REAL_INTERVAL(FrameInterVal, DataPacket.ShowTime, FileStartTimeStamp);

				if (FrameInterVal >= nTimeTemp)
					dwMaxPos = dwCurPos;
				else
					dwMinPos = dwCurPos;
				break;
			}
		}

		if (dwRet < 4096)
			break;
	}

//      SDK_DPF(("[SDK] dwMinPos = %d, dwMaxPos = %d\n", dwMinPos, dwMaxPos));

	/*
	   经过上面搜寻之后，指定时间点的视频帧位于dwMinPos和dwMaxPos之间 
	 */
	/*
	   从dwMinPos开始往后搜，搜到第一个I帧返回 
	 */
	fseek(file, dwMinPos, SEEK_SET);
	while (1)
	{
		dwRet = fread(DataBuffer, 1, 4096, file);
		if (Higheasy_SerchHead(DataBuffer, dwRet, &lFindDataStartPos, I_FRAME) != 0)
		{
//                      SDK_DPF(("[SDK] Curpos = %d, Higheasy_SerchHead I_FRAME Fail 0\n", ftell(file)));               
			if (dwRet != 4096)
				break;
			else
				fseek(file, -FRAME_HEAD_LENTH, SEEK_CUR);
		}
		else
		{
			fseek(file, lFindDataStartPos - dwRet, SEEK_CUR);
			dwCurPos = ftell(file);
			dwRet = fread(&DataPacket, 1, FRAME_HEAD_LENTH, file);
			pFrameInfo->FramePos = dwCurPos;	//搜寻到的I帧的位置
			pFrameInfo->FrameNum = DataPacket.FrameNo;	//搜寻到的I帧的时间
			pFrameInfo->FrameTime = DataPacket.ShowTime;	//搜寻到的I帧的时间
			fseek(file, InitFilePos, SEEK_SET);
//                      SDK_DPF(("[SDK] Curpos = %d, Higheasy_SerchHead I_FRAME OK 0\n", dwCurPos));
			return 0;
		}
	}

	/*
	   从dwMinPos开始往前搜，搜到第一个I帧返回 
	 */
	if (dwMinPos > 4096)
		fseek(file, dwMinPos - 4096, SEEK_SET);
	else
		fseek(file, 0, SEEK_SET);
	while (1)
	{
		dwRet = fread(DataBuffer, 1, 4096, file);
		if (Higheasy_SerchHead(DataBuffer, dwRet, &lFindDataStartPos, I_FRAME) != 0)
		{
//                      SDK_DPF(("[SDK] Curpos = %d, Higheasy_SerchHead I_FRAME Fail 1\n", ftell(file)));
			dwCurPos = ftell(file);
			if (dwCurPos > (4096 * 2 - FRAME_HEAD_LENTH))
				fseek(file, -(4096 * 2 - FRAME_HEAD_LENTH), SEEK_CUR);
			else if (dwCurPos > 4096)
				fseek(file, 0, SEEK_SET);
			else
				break;
		}
		else
		{
			fseek(file, lFindDataStartPos - dwRet, SEEK_CUR);
			dwCurPos = ftell(file);
			dwRet = fread(&DataPacket, 1, FRAME_HEAD_LENTH, file);
			pFrameInfo->FramePos = dwCurPos;	//搜寻到的I帧的位置
			pFrameInfo->FrameNum = DataPacket.FrameNo;	//搜寻到的I帧的时间
			pFrameInfo->FrameTime = DataPacket.ShowTime;	//搜寻到的I帧的时间
			fseek(file, InitFilePos, SEEK_SET);
//                      SDK_DPF(("[SDK] Curpos = %d, Higheasy_SerchHead I_FRAME OK 1\n", dwCurPos));
			return 0;
		}
	}


//      SDK_DPF(("[SDK] Higheasy_FR_FindFrameByTime Fail!!\n"));
	fseek(file, InitFilePos, SEEK_SET);
	return -1;
}

/* 读取有效的帧头 */
int Higheasy_FR_ReadHead(H264DATAPACKETHEADER * pHead, FILE * file, int Type)
{
	int dwRet;

	while (1)
	{
		/*
		   读取帧头 
		 */
		dwRet = fread(pHead, 1, FRAME_HEAD_LENTH, file);
		if (dwRet != FRAME_HEAD_LENTH)
		{
			printf("MP4Sys_DecReadOneFrame dwret=%x\n", dwRet);
			return -1;
		}

		/*
		   如果读出的不是帧头则搜寻下一个帧头 
		 */
		if (pHead->DataPacketStartCode != STARTCODE)
		{
			/*
			   软件处理录像文件时按照16K作对其,文件结束时可能会有很多0 
			 */
			if ((pHead->DataPacketStartCode == 0) && (pHead->FrameNo == 0) && (pHead->ShowTime == 0) && (pHead->nOverloadLen == 0))
			{
				return -2;
			}
			else
			{
				fseek(file, -FRAME_HEAD_LENTH, SEEK_CUR);
				if (Higheasy_FR_FindNextFrame(file, 0xFF) != 0)
					return -3;
				fread(pHead, 1, FRAME_HEAD_LENTH, file);
			}
		}

		/*
		   如果不是视频数据，跳过，重新读 
		 */
		if (Type == 0)
		{
			if ((pHead->FrameType != I_FRAME) && (pHead->FrameType != P_FRAME))
			{
				fseek(file, pHead->nOverloadLen, SEEK_CUR);
				continue;
			}
		}
		else if (Type == 1)
		{
			if (pHead->FrameType != AUDIO_FRAME)
			{
				fseek(file, pHead->nOverloadLen, SEEK_CUR);
				continue;
			}
		}
		break;
	}
	return 0;
}
