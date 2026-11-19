
/*!
 *\file		STREAM_TYPE.h
 *\brief	数据流类型定义
 *\author	yuk@higheasyrd.com
 *\date		2009/04/16

说明：
-#	定义了数据流类型,在备份时使用
*/

#ifndef __STREAM_TYPE_H_
#define __STREAM_TYPE_H_

//!\brief 拟支持的数据流格式
typedef enum
{
	//!\brief avi格式,暂时不支持
	STREAM_TYPE_H264RAW = 0,

	//!\brief 纯H264的数据格式
	STREAM_TYPE_H264MP4,

	//!\brief AVI格式
	STREAM_TYPE_H264AVI,

	STREAM_TYPE_COUNT
} STREAM_TYPE;

//!\brief 拟支持的备份类型
typedef enum
{
	//brief 备份盘组
	BAK_BACKUPGROUP = 0x0,

	//!\brief U盘备份 
	BAK_USBDISK,

	//!\brief CDROM备份
	BAK_USBCDROM,

	BAK_TYPE_COUNT
} IDVR_BAK_TYPE;

#endif /**/
