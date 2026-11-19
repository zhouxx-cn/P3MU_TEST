// Copyright (C) 1991 - 1999 Rational Software Corporation

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_TAGCHARBITMAP_49B492EB01A8_INCLUDED
#define _INC_TAGCHARBITMAP_49B492EB01A8_INCLUDED

//##ModelId=49B492EB01A8
struct tagCharBitMap
{

	//##ModelId=49B492FB03E5
	//点阵图片宽
	int nWidth;

	//##ModelId=49B4930A0301
	//点阵图片高
	int nHeight;

	//每点占bits
	int nBitNum;

	//##ModelId=49B493140156
	//点阵缓冲
	char *pBuff;

};

#endif							/* _INC_TAGCHARBITMAP_49B492EB01A8_INCLUDED */
