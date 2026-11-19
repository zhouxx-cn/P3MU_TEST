// Copyright (C) 1991 - 1999 Rational Software Corporation

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_ENUMRECTYPE_49AC9E8200DA_INCLUDED
#define _INC_ENUMRECTYPE_49AC9E8200DA_INCLUDED

//录像类型：
//包括：无录像、报警、普通、移动侦测
//##ModelId=49AC9E8200DA
enum enumRecType
{
	//无录像
	//##ModelId=49AC9E9F00AA
	RECORD_TYPE_NO = 0,

	//报警录像类型
	//##ModelId=49ACC4790003
	RECORD_TYPE_ALARM,

	//普通类型
	//##ModelId=49AC9F23003B
	RECORD_TYPE_GENERAL,

	//手动录像类型
	//##ModelId=49AC9F5902F6
	RECORD_TYPE_MANUAL,

	//移动录像
	//##ModelId=49AC9F8E01EE
	RECORD_TYPE_MOTION,
	
	RECORD_TYPE_HTTP_TRIG,

	//智能录像(人形/人脸/...)
	RECORD_TYPE_AI,
};

#endif							/* _INC_ENUMRECTYPE_49AC9E8200DA_INCLUDED */
