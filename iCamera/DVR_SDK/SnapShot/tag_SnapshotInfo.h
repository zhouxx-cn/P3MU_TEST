// Copyright (C) 1991 - 1999 Rational Software Corporation

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_TAG_SNAPSHOTINFO_49A78C120167_INCLUDED
#define _INC_TAG_SNAPSHOTINFO_49A78C120167_INCLUDED

//##ModelId=49A78C120167
struct tag_SnapshotInfo
{
  public:
	//!<快照名称
	//##ModelId=49A78C120172
	char m_szFileName[256];

	//!<所属通道
	//##ModelId=49A7969A002D
	int m_nChannel;

	//!<抓拍时间
	//##ModelId=49A7969A002E
	int m_nTime;

	//!<数据时间
	//##ModelId=49A7969A002E
	int m_nDateTime;

	//##ModelId=49A7969A002F
	bool m_bLocked;
	bool m_bMark;
};
#endif							/* _INC_TAG_SNAPSHOTINFO_49A78C120167_INCLUDED */
