
/*!
 *\file		DEVICECAP.h
 *\brief		设备能力结构定义
 *\author		林云
 *\date		2009/06/18

功能：
-#	定义DVR设备能力信息
 */


#ifndef _INC_DEVICECAP_H_
#define _INC_DEVICECAP_H_

#include "ddt_so.h"

//\brief    描述DVR设备信息结构定义
struct tag_DeviceCap
{
	//!\brief   设备型谱名称，如HY-2016HC
	char m_szTypeName[MAX_NAME_LEN];

	unsigned char m_bytVideoInChannels;	/*视频输入路数 */
	unsigned char m_bytAudioInChannels;	/*音频输入路数 */
	unsigned char m_bytAlarmInChannels;	/*报警输入路数 */
	unsigned char m_bytAlarmOutChannels;	/*报警输出路数 */

	unsigned char m_bytVGAChannels;	/*VGA接口个数 */

	unsigned char m_bytSpotOutChannles;	/*SPOT输出路数 */
	unsigned char m_bytVoiceInChannels;	/*语音对讲输入路数 */

	unsigned char m_bytMaxDiskNums;	/*支持的最大硬盘个数 */

	unsigned char m_bytVideoSteamType[MAX_CHANNEL_NUM];	/*支持的码流类型， STREAM_MAIN/STREAM_SUB */
	unsigned long m_dwMainVideoSreamResolutionCap[MAX_CHANNEL_NUM];	/*主码流支持的压缩尺寸CR_CIF/CR_2CIF/CR_4CIF */
	unsigned long m_dwSubVideoSreamResolutionCap[MAX_CHANNEL_NUM];	/*子码流支持的压缩尺寸CR_CIF/CR_2CIF/CR_4CIF */
};


typedef struct tag_DeviceCap DEVICECAP;

typedef DEVICECAP *PDEVICECAP;

#endif							/* #ifndef _INC_DEVICECAP_H_ */
