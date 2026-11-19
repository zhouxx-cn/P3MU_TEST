
/*!
 *\file		DEVICEVERSION.h
 *\brief		设备 版本结构定义
 *\author		林云
 *\date		2009/06/18

功能：
-#	定义DVR设备版本信息
 */


#ifndef _INC_DEVICEVERSION_H_
#define _INC_DEVICEVERSION_H_

//\brief    描述DVR设备信息结构定义
struct tag_DeviceVersion
{
	//!\brief   序列号
	unsigned char m_szSerialNum[32];

	//!\brief   硬件版本
	char m_szHardwareVersion[32];

	//!\brief   软件版本
	char m_szSoftwareVersion[32];

	//!\brief   软件版本(只包含版本号和日期，不包含后缀)
	char m_szSoftwareVersionPure[32];

	//!\brief   编码版本
	char m_szDspVersion[32];
};


typedef struct tag_DeviceVersion DEVICEVERSION;

typedef DEVICEVERSION *PDEVICEVERSION;

#endif							/* #ifndef _INC_DEVICEVERSION_H_ */
