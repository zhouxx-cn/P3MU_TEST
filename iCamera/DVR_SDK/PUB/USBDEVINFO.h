
/*!
 *\file		USBDEVINFO.h
 *\brief	USB设备信息结构定义
 *\author	林云 yuk@higheasyrd.com
 *\date		2009/04/16

功能：
-#	定义USB设备信息（设备个数、分区信息、文件信息等）
 */


#ifndef __USBDEVINFO_H_
#define __USBDEVINFO_H_

//!\brief 支持的USB设备最大分区数
const int MAX_USB_PARTITIONS = 10;

//\brief 描述USB设备信息结构定义
//##ModelId=49A6070B03C6
struct tag_UsbDevInfo
{
  public:
	//!\brief usb设备名
	//##ModelId=49A607D60238
	char m_szDeviceName[30];

	//!\brief usb设备类型
	//##ModelId=49A607D6024C
	char m_szDeviceType[15];

	//!\brief usb 设备信息
	//##ModelId=49A607D60260
	char m_szDeviceVendor[30];

	//!\brief USB设备分区数
	//##ModelId=49A607D6026A
	int m_nPartitions;

	//!\brief USB设备容量
	//##ModelId=49A607D6027E
	unsigned long m_ulTotalPartitionsSpace[MAX_USB_PARTITIONS];

	//!\brief USB设备可用空间
	//##ModelId=49A607D60292
	unsigned long m_ulAvailablePartitionsSpace[MAX_USB_PARTITIONS];

	//!\brief USB设备挂载点
	//##ModelId=49A607D602A6
	char m_szMountPoint[MAX_USB_PARTITIONS][60];

	//!\brief USB设备节点
	//##ModelId=49A607D602C4
	char m_szDevName[MAX_USB_PARTITIONS][60];

};

//!\brief   USB设备信息结构类型定义
typedef struct tag_UsbDevInfo USBDEVINFO;


//!\brief   USB设备信息结构指针类型定义
typedef USBDEVINFO *PUSBDEVINFO;
#endif
