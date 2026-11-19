
/*********************************************************************
* 版权所有 (C)2008, 武汉恒亿电子科技发展有限公司。
* 
* 文件名称： BuildDD.cpp
* 文件标识： 暂无
* 内容摘要： 内嵌设备描述信息
* 其它说明： 当增加新的设备型号时， 需手动把设备的信息录入到此文件中。
* 当前版本： 1.0.0
* 作    者： 张林 (zl@higheasyrd.com)
* 完成日期： 2008.6.3
* 
* 修改记录1：
*    修改日期：
*    版 本 号：
*    修 改 人：
*    修改内容： 
* 修改记录2：…
**********************************************************************/
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>
#include <unistd.h>

#include "ddt_so.h"
#include "inifile.h"
#include "../include/zwsdk.h"
#include "sys_banned.h"
#include "PUB_BoradInfo.h"
#include "include/global_FilePath.h"

int BuildDeviceDescriptionByID(LPDeviceDescription pDD, const ULONG nBoardID)
{
	int iChannel = 0;
	char value[64] = {0};
    int SnsCnt = ZWSys_GetMaxSensorCnt();

	pDD->dwBoardID = nBoardID;
	strncpy(pDD->szTypeName, PUB_FindBoardTypeInfoById(nBoardID), sizeof(pDD->szTypeName) - 1);

    if (1 == read_profile_string("SystemFunction", "FunctionFullType", value, sizeof(value), "", CUSTOM_CONFIG_FILE))
    {
        memset(pDD->szTypeName,0, sizeof(pDD->szTypeName));
        snprintf(pDD->szTypeName, sizeof(pDD->szTypeName), "%.31s", value);
    }
    else
    {
        // 优先使用产测工具定制
    	if (1 == read_profile_string("SystemFunction", "FunctionType", value, sizeof(value), "", CUSTOM_CONFIG_FILE) && (strlen(value) > 0))
    	{
            memset(pDD->szTypeName, 0, MAX_NAME_LEN);
			strncpy(pDD->szTypeName, value, sizeof(pDD->szTypeName) - 1);
    	}
    }

	for (iChannel = 0; iChannel < MAX_CHANNEL_NUM; ++iChannel)
	{
		pDD->bytVideoSteamType[iChannel] = STREAM_MAIN | STREAM_SUB; /*支持的码流类型， STREAM_MAIN/STREAM_SUB */
		/* 主码流支持的压缩尺寸 */
		pDD->bytMainVideoSreamResolutionCap[iChannel] = CR_QXGA | CR_HD1080;
		/* 子码流支持的压缩尺寸 */
		pDD->bytSubVideoSreamResolutionCap[iChannel] = CR_4CIF | CR_640x360;
	}

	pDD->bytCoreFileType = FILE_MP4;	/*媒体文件封装格式	FILE_MP4/FILE_ASF/FILE_AVI */
	pDD->bytDecChannels = 1;			/*解码通道个数-----暂时保留，使用系统SDK的返回值 */
	pDD->bytAudioInChannels = 1;		/*音频输入路数 */
	pDD->bytVideoInChannels = SnsCnt;	/*视频输入路数-----暂时保留，使用系统SDK的返回值 */
	pDD->bytAudioOutChannels = 1;		/*音频输出路数 */
	pDD->bytVoiceInChannels = 1;		/*语音对讲输入路数 */
	pDD->bytSpotOutChannles = 0;		/*SPOT输出路数 */
	pDD->bytLoopBackOutChannels = 0;	/*环通输出路数 */
	pDD->bytKeyBoardInChannels = 0; 	/*键盘输入口路数 */
	pDD->bytAlarmInChannels = 0;		/*报警输入路数 */
	pDD->bytAlarmOutChannels = 1;		/*报警输出路数 */

	//只有1个串口
	for (iChannel = 0; iChannel < MAX_RS_CHANNEL; iChannel++)
	{
		pDD->bytRS232Channels[iChannel] = 0; /*RS232个数 ：Bit7 ；Bit6 ~ Bit0 节点号  */
		pDD->bytRS422Channels[iChannel] = 0; /*RS422个数 ：Bit7 ；Bit6 ~ Bit0 节点号  */
		pDD->bytRS485Channels[iChannel] = 0; /*RS485个数 ：Bit7 ；Bit6 ~ Bit0 节点号  */
	}

	pDD->bytVGAChannels = 0;	/*VGA接口个数 */
	pDD->bytUSBChannels = 0;	/*USB接口个数 */
	pDD->bytCVBSPorts = 0;		/*复合视频输出端口个数 */
	pDD->bytSVideoPorts = 0;	/*S-Video视频输出端口个数 */
	pDD->bytIDEType = IDE_SD;	/*硬盘类型， IDE_ATA/IDE_SATA */
	pDD->bytMaxDiskNums = 1;	/*支持的最大硬盘个数 */
	pDD->bytAimDetectSupport = 0;	/*是否支持目标检测: Bit7; 区域个数： Bit6 ～　Bit0 */
	pDD->bytMotionDetectSupport = 1;	/*是否支持运动检测 */
	pDD->bytMosaicSupport = 0;	/*是否支持马赛克: Bit7; 区域个数： Bit6 ～　Bit0 */
	pDD->dwReserved1 = 0;
	pDD->dwReserved2 = 0;
	pDD->dwReserved3 = 0;
	pDD->dwReserved4 = 0;
	
	return ERR_DDT_SUCCESSS;
}

/*!
 * \brief
 * 打印设备能力信息
 * 
 * \param pDD
 * 设备能力描述结构体
 */
VOID DumpDeviceInfo(const LPDeviceDescription pDD)
{
	int iCnt;

	printf("\n");
	printf("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
	printf("++++++                         Device Info Dump                       ++++++\n");
	printf("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
	printf("BoardID = 0x%x\n", (int) pDD->dwBoardID);
	printf("BoardTypeName = %s\n", pDD->szTypeName);

	printf("bytVideoSteamType:\n");
	for (iCnt = 0; iCnt < MAX_CHANNEL_NUM; iCnt++)
		printf("%d ", pDD->bytVideoSteamType[iCnt]);
	printf("\n");

	printf("bytMainVideoSreamResolutionCap:\n");
	for (iCnt = 0; iCnt < MAX_CHANNEL_NUM; iCnt++)
		printf("%lu ", pDD->bytMainVideoSreamResolutionCap[iCnt]);
	printf("\n");


	printf("bytSubVideoSreamResolutionCap:\n");
	for (iCnt = 0; iCnt < MAX_CHANNEL_NUM; iCnt++)
		printf("%lu ", pDD->bytSubVideoSreamResolutionCap[iCnt]);
	printf("\n");

	printf("bytCoreFileType = %d\n", pDD->bytCoreFileType);

	printf("bytDecChannels = %d\n", pDD->bytDecChannels);

	printf("bytAudioInChannels = %d\n", pDD->bytAudioInChannels);

	printf("bytVideoInChannels = %d\n", pDD->bytVideoInChannels);

	printf("bytAudioOutChannels = %d\n", pDD->bytAudioOutChannels);

	printf("bytVoiceInChannels = %d\n", pDD->bytVoiceInChannels);

	printf("bytSpotOutChannles = %d\n", pDD->bytSpotOutChannles);

	printf("bytLoopBackOutChannels = %d\n", pDD->bytLoopBackOutChannels);

	printf("bytAlarmInChannels = %d\n", pDD->bytAlarmInChannels);

	printf("bytAlarmOutChannels =%d\n", pDD->bytAlarmOutChannels);

	printf("bytRS422Channels:\n");
	for (iCnt = 0; iCnt < MAX_RS_CHANNEL; iCnt++)
		printf("%d ", pDD->bytRS422Channels[iCnt]);
	printf("\n");

	printf("bytRS232Channels:\n");
	for (iCnt = 0; iCnt < MAX_RS_CHANNEL; iCnt++)
		printf("%d ", pDD->bytRS232Channels[iCnt]);
	printf("\n");

	printf("bytRS485Channels:\n");
	for (iCnt = 0; iCnt < MAX_RS_CHANNEL; iCnt++)
		printf("%d ", pDD->bytRS485Channels[iCnt]);
	printf("\n");

	printf("bytVGAChannels = %d\n", pDD->bytVGAChannels);

	printf("bytUSBChannels = %d\n", pDD->bytUSBChannels);

	printf("bytCVBSPorts = %d\n", pDD->bytCVBSPorts);

	printf("bytSVideoPorts = %d\n", pDD->bytSVideoPorts);

	printf("bytIDEType = %d\n", pDD->bytIDEType);

	printf("bytMaxDiskNums = %d\n", pDD->bytMaxDiskNums);

	printf("bytAimDetectSupport = %d\n", pDD->bytAimDetectSupport);

	printf("bytMotionDetectSupport = %d\n", pDD->bytMotionDetectSupport);

	printf("bytMosaicSupport = %d\n", pDD->bytMosaicSupport);

	printf("dwReserved1 = %lu\n", pDD->dwReserved1);
	printf("dwReserved2 = %lu\n", pDD->dwReserved2);
	printf("dwReserved3 = %lu\n", pDD->dwReserved3);
	printf("dwReserved4 = %lu\n", pDD->dwReserved4);

	printf("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
	printf("++++++                        End                                     ++++++\n");
	printf("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
}

