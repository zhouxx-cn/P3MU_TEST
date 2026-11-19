
/*!
 *\file		DEVICEINFO.h
 *\brief	设备信息结构定义
 *\author	林云
 *\date		2009/05/05

功能：
-#	定义DVR设备基本信息（设备名称、自动锁屏时间、系统语言等）
 */


#ifndef _INC_DEVICEINFO_H_
#define _INC_DEVICEINFO_H_

//\brief    描述DVR设备信息结构定义
struct tag_DeviceInfo
{
	//!\brief   自动锁屏时间（单位为秒）
	int m_nAutoLockScreenTime;	// 0 - 从不

	//!\brief   语言
	int m_nMenuLanguage;		// 0(Default) - 简体中文 1 - 繁体中文 2 - 英文

	//!\brief   日期格式
	int m_nDateFormat;			// 0(Default) - YYYY-MM-DD W hh:mm:ss, 1 - YYYY-MM-DD hh:mm:ss, 2 - MM-DD-YYYY W hh:mm:ss, 3 - MM-DD-YYYY hh:mm:ss

	//!\brief   时间格式（0 - 12小时制，1 - 24小时制）
	int m_nTimeFormat;

};


typedef struct tag_DeviceInfo DEVICEINFO;

typedef DEVICEINFO *PDEVICEINFO;

#endif							/* #ifndef _INC_DEVICEINFO_H_ */
