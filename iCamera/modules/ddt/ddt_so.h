
/*********************************************************************
* 版权所有 (C)2008, 武汉恒亿电子科技发展有限公司。
* 
* 文件名称： ddt_so.h
* 文件标识： 暂无
* 内容摘要： 描述设备描述库的接口函数
* 其它说明： 当增加新的设备型号时， 需手动把设备的信息录入到libddt.so文件中。
* 当前版本： 1.0.0
* 作    者： 张林 (zl@higheasyrd.com)
* 完成日期： 2008.6.2
* 
* 修改记录1：
*    修改日期：
*    版 本 号：
*    修 改 人：
*    修改内容： 
* 修改记录2：…
**********************************************************************/

#ifndef __HIGHEASY_EDVRS_DDT_20080602__
#define __HIGHEASY_EDVRS_DDT_20080602__

/*设备型谱名称的长度*/
#define MAX_NAME_LEN          32

/*设备音、视频通道的最大路数*/
#define MAX_CHANNEL_NUM       2

/*设备RS232、RS483、RS42的最大个数*/
#define MAX_RS_CHANNEL        4

/*输入参数宏*/
#define IN

/*输出参数宏*/
#define OUT

/*输入/输出参数宏*/
#define IN_OUT

/*SDK返回码*/
#define ERR_DDT_SUCCESSS         0	/*成功返回值 */
#define ERR_DDT_EOF_OF_TABLE    -1	/*枚举时遇到表尾 */
#define ERR_DDT_INVALID_PARAM   -2	/*输入参数非法 */
#define ERR_DDT_NOT_SUPPORTED   -3	/*输入的设备信息不支持 */
#define ERR_DDT_NOT_IMPLEMENT   -4	/*接口函数功能待实现 */

typedef unsigned char UCHAR;

typedef unsigned long ULONG;

typedef char CHAR;

typedef int INT;

//typedef   void             VOID;
#ifndef VOID
#define VOID	void
#endif

/*定义视频压缩格式*/
#define VIDEO_MPEG4       0x01
#define VIDEO_H264        0x02
#define VIDEO_ST          0x04

/*定义音频编码格式*/
#define AUDIO_G726        0x01
#define AUDIO_G729        0x02
#define AUDIO_G711        	0x04
#define AUDIO_MPEG4AAC	0x08

/*支持的码流类型*/
#define STREAM_MAIN       0x01
#define STREAM_SUB        0x02
#define STREAM_HDSUB        0x04

/*压缩尺寸*/
#define CR_QCIF	0x01
#define CR_CIF	0x02
#define CR_2CIF	0x04
#define CR_4CIF	0x08 	
#define CR_4CIF_SIM   0x10		//非实时D1
#define CR_2CIF_SIM   0x20		//非实时2CIF

#define CR_QVGA		0x40		//320*240
#define CR_VGA		0x80		//640*480
#define CR_SVGA		0x100		//800*600
#define CR_XVGA		0x200		//1024*768
#define CR_HD720	0x400		//1280*720
#define CR_QuadVGA	0x800		//1280*960
#define CR_SXGA		0x1000		//1280*1024
#define CR_UXGA		0x2000		//1600*1200
#define CR_HD1080	0x4000		//1920*1080
#define CR_HD576	0x8000		//960*576
#define CR_QXGA		0x10000		//2048*1536
#define CR_640x360	0x20000		//640*360
#define CR_800x448	0x30000		//800*448

#define CR_UWXGA    0x40000    //2592*1520   400W
#define CR_USXGA    0x80000    //2592*1944   500W

//#define CR_500MP      0x8000  //2560*1920

#define CR_D1	CR_2CIF
#define CR_SD	CR_4CIF

/*媒体文件封装格式*/
#define FILE_MP4		0x01
#define FILE_ASF		0x02
#define FILE_AVI		0x04

/*硬盘类型*/
#define IDE_ATA		0x01
#define IDE_SATA          0x02
#define IDE_SD		0x04
#define IDE_USB		0x08

#if defined( __cplusplus)
#define DDT_API  extern "C"
#else
#define DDT_API
#endif


typedef struct tagDeviceDescription
{
	ULONG dwBoardID;			/*设备板型ID */
	CHAR szTypeName[MAX_NAME_LEN];	/*设备型谱名称，如HY-2016HC */
	UCHAR bytVideoSteamType[MAX_CHANNEL_NUM];	/*支持的码流类型， STREAM_MAIN/STREAM_SUB */
	ULONG bytMainVideoSreamResolutionCap[MAX_CHANNEL_NUM];	/*主码流支持的压缩尺寸CR_CIF/CR_2CIF/CR_4CIF */
	ULONG bytSubVideoSreamResolutionCap[MAX_CHANNEL_NUM];	/*子码流支持的压缩尺寸CR_CIF/CR_2CIF/CR_4CIF */
	UCHAR bytCoreFileType;		/*媒体文件封装格式  FILE_MP4/FILE_ASF/FILE_AVI */
	UCHAR bytDecChannels;		/*解码通道个数 */
	UCHAR bytAudioInChannels;	/*音频输入路数 */
	UCHAR bytVideoInChannels;	/*视频输入路数 */
	UCHAR bytAudioOutChannels;	/*音频输出路数 */
	UCHAR bytVoiceInChannels;	/*语音对讲输入路数 */
	UCHAR bytSpotOutChannles;	/*SPOT输出路数 */
	UCHAR bytLoopBackOutChannels;	/*环通输出路数 */
	UCHAR bytKeyBoardInChannels;	/*键盘输入口路数 */
	UCHAR bytAlarmInChannels;	/*报警输入路数 */
	UCHAR bytAlarmOutChannels;	/*报警输出路数 */
	UCHAR bytRS422Channels[MAX_RS_CHANNEL];	/*RS422个数 ：Bit7 ；Bit6 ~ Bit0 节点号  */
	UCHAR bytRS232Channels[MAX_RS_CHANNEL];	/*RS232个数 ：Bit7 ；Bit6 ~ Bit0 节点号  */
	UCHAR bytRS485Channels[MAX_RS_CHANNEL];	/*RS485个数 ：Bit7 ；Bit6 ~ Bit0 节点号  */
	UCHAR bytVGAChannels;		/*VGA接口个数 */
	UCHAR bytUSBChannels;		/*USB接口个数 */
	UCHAR bytCVBSPorts;			/*复合视频输出端口个数 */
	UCHAR bytSVideoPorts;		/*S-Video视频输出端口个数 */
	UCHAR bytIDEType;			/*硬盘类型， IDE_ATA/IDE_SATA */
	UCHAR bytMaxDiskNums;		/*支持的最大硬盘个数 */
	UCHAR bytAimDetectSupport;	/*是否支持目标检测: Bit7; 区域个数： Bit6 ～　Bit0 */
	UCHAR bytMotionDetectSupport;	/*是否支持运动检测 */
	UCHAR bytMosaicSupport;		/*是否支持马赛克: Bit7; 区域个数： Bit6 ～　Bit0 */
	ULONG dwReserved1;
	ULONG dwReserved2;
	ULONG dwReserved3;
	ULONG dwReserved4;
} DeviceDescription, *LPDeviceDescription;

/*!
 * \brief
 *  根据设备型号名称来获取设备能力描述表
 * 
 * \param pDD
 *  存放设备能力的缓冲区
 * \param szTypeName
 *  设备型号名称
 *
 * \return
 *  返回0表示成功， 其它值表示失败
 * 
 * \see
 *  EDVRS_DDT_GetDeviceDescriptionByID.
 */
DDT_API INT EDVRS_DDT_GetDeviceDescriptionByName(OUT LPDeviceDescription pDD, IN const CHAR * szTypeName);

/*!
 * \brief
 *  根据BoardID来获取设备能力描述表
 * 
 * \param pDD
 *  存放设备能力的缓冲区
 * \param BoardID
 *  设备板型ID
 * 
 * \return
 *  返回0表示成功， 其它值表示失败
 * 
 * \see
 *  EDVRS_DDT_GetDeviceDescriptionByName.
 */
DDT_API INT EDVRS_DDT_GetDeviceDescriptionByID(OUT LPDeviceDescription pDD, IN const ULONG BoardID);

/*!
* \brief
*  启动设备描述表枚举功能
* 
* \see
*  EDVRS_DDT_EnumDeviceDescripiontTable.
*/
DDT_API VOID EDVRS_DDT_EnumDeviceDescriptionTableStart();

/*!
 * \brief
 *  获取设备描述表枚举信息
 * 
 * \param
 *  存放设备能力的缓冲区
 *
 * \return
 *  返回0表示成功， 其它值表示失败
 */
DDT_API INT EDVRS_DDT_EnumDeviceDescripiontTable(OUT LPDeviceDescription pDD);

/*!
* \brief
*  停止设备描述表枚举功能
* 
* \see
*  EDVRS_DDT_EnumDeviceDescripiontTable.
*/
DDT_API VOID EDVRS_DDT_EnumDeviceDescriptionTableStop();

#endif
