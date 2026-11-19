#ifndef	_HY_FILEREAD_H_
#define	_HY_FILEREAD_H_

#include "Commen.h"

typedef struct TagFrame
{
	int FramePos;
	unsigned int FrameNum;		//帧号(相对于第一帧)
	unsigned int FrameTime;		//帧时间(相对于第一帧)
} TagFrame_t;

int Higheasy_GetFileLen(FILE * file);

int Higheasy_FR_FindNextFrame(FILE * file, int nFrameType);

int Higheasy_FR_FindFirstFrame(FILE * file, unsigned int *pFramNo, unsigned int *pFrameTime, int *pFramePos);

int Higheasy_FR_FindFrameByTime(FILE * file, unsigned int FileStartTimeStamp, unsigned int PosTime, TagFrame_t * pFrameInfo);

int Higheasy_FR_ReadHead(H264DATAPACKETHEADER * pHead, FILE * file, int Type);

int Higheasy_FR_FindFrameByTime_New(FILE * file, unsigned int FileStartTimeStamp, unsigned int PosTime, TagFrame_t * pFrameInfo);

#endif
