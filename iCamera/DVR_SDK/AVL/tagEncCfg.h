// Copyright (C) 1991 - 1999 Rational Software Corporation

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_TAGENCCFG_49BF4C150101_INCLUDED
#define _INC_TAGENCCFG_49BF4C150101_INCLUDED

#include "newhie/HieServer_Common.h"

//录像结构体
//##ModelId=49BF4C150101
struct tagEncCfg
{

	//帧率 1-20，ff为全帧率
	//##ModelId=49BF4C3300AA
	int dwVideoFrameRate;

	//码率上限。单位K，
	//##ModelId=49BF4C3300BE
	int dwVideoBitrateH;

	//码率下限
	//##ModelId=49BF4C3300DC
	int dwVideoBitrateL;

	//量化系数:0-6
	//##ModelId=49BF4C330104
	int dwQuotiety;

	//分辨率0-CIF, 1-D1 ,2 2CIF , 3 DCIF, 4 MD 5QCIF
	//分辨率0-4CIF, 1-H720 ,2-H1080 
	//##ModelId=49BF4C330122
	eResolutionIndex byResolution;

    //分辨率的宽
    int dwResWidth;

    //分辨率的高
    int dwResHeight;
    
	//码流类型0-视频流,1-复合流
	//##ModelId=49BF4C33014A
	int byStreamType;

	//录像质量 0-高质量 1-标准 2-低 3-自定义
	//##ModelId=49BF4C330168
	int byPicQuality;

	//码率类型0: 变码率率，1: 定码，2：限定码率
	//##ModelId=49BF4C330190
	int byBitrateType;

};

#endif							/* _INC_TAGENCCFG_49BF4C150101_INCLUDED */
