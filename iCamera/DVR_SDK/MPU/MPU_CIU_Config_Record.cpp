
/*!
 *\file	 MPU_CIU_Config_Record.cpp
 *\brief	MPU模块，远程配置，录像参数
 *\author	林云
 *\date	2009/8/17

功能：
-#	远程配置，编码参数
-#	远程配置，录像计划

说明：
-#	本文件使用新的网络库实现
 */

#include "MPU_CIU_global.h"

extern int GetDevCustom_Record(LPHY_DVR_WORKMODE pcfg);

#ifdef __USE_RTMP__
static int Notif_Creat_Channel(int *nRet)
{
	MPU_AVL_Record_Notif_Creat_Channel(nRet);
	return 0;
}

static int SetActionCode(char *pCode,int unAllActivedFunctionMask)
{
	MPU_AVL_Record_SetActionCode(pCode,unAllActivedFunctionMask);

	return 0;
}

static int GetActionCode(char *pCode)
{
	MPU_AVL_Record_GetActionCode(pCode);

	return 0;
}

static int SetPushAddressCfg(char enable ,char *PushURL)
{	
	int ret =0;
	ret = MPU_AVL_Record_SetPushAddressCfg(enable,PushURL);

	return ret;
}	

static int GetPushAddressCfg(char *bsupport, char *enable, char *pushurl )
{	
	int ret =0;
	ret = MPU_AVL_Record_GetPushAddressCfg(bsupport,enable,pushurl);

	return ret;
}	

static int SetConnect_param(int tpye, int nCh, HY_DVR_RTMP_PUSH_CONFIG *pcfg)
{
	if(NULL == pcfg)
	{
		return IDVR_ERROR_INVALID_POINTER;
	}
	switch(tpye)
		{
			case 0:
				if(0 == pcfg->struMainRTMPConfig[nCh].bValid)
				{
					MPU_DBG("0 == pcfg->bValid, nCh = %d\n", nCh);
					return IDVR_CIU_STRUCT_UNVALID;					
				}
				
				MPU_AVL_Record_SetRtmpParam(0, nCh, pcfg->struMainRTMPConfig[nCh].szUserName,pcfg->struMainRTMPConfig[nCh].szPassword,
				pcfg->struMainRTMPConfig[nCh].szStreamPullingAddress,pcfg->struMainRTMPConfig[nCh].dwEnable,\
				pcfg->struMainRTMPConfig[nCh].dwStatus, pcfg->struMainRTMPConfig[nCh].cIsPushAudioEnable);
				break;
			case 1:
				if(0 == pcfg->struSub1RTMPConfig[nCh].bValid)
				{
					MPU_DBG("0 == pcfg->bValid, nCh = %d\n", nCh);
					return IDVR_CIU_STRUCT_UNVALID;					
				}
				MPU_AVL_Record_SetRtmpParam(1, nCh,  pcfg->struSub1RTMPConfig[nCh].szUserName,pcfg->struSub1RTMPConfig[nCh].szPassword,
				pcfg->struSub1RTMPConfig[nCh].szStreamPullingAddress,pcfg->struSub1RTMPConfig[nCh].dwEnable,\
				pcfg->struSub1RTMPConfig[nCh].dwStatus, pcfg->struMainRTMPConfig[nCh].cIsPushAudioEnable);
				break;
			case 2:
				if(0 == pcfg->struSub2RTMPConfig[nCh].bValid)
				{
					MPU_DBG("0 == pcfg->bValid, nCh = %d\n", nCh);
					return IDVR_CIU_STRUCT_UNVALID;					
				}
				MPU_AVL_Record_SetRtmpParam(2, nCh,  pcfg->struSub2RTMPConfig[nCh].szUserName,pcfg->struSub2RTMPConfig[nCh].szPassword,
				pcfg->struSub2RTMPConfig[nCh].szStreamPullingAddress,pcfg->struSub2RTMPConfig[nCh].dwEnable,\
				pcfg->struSub2RTMPConfig[nCh].dwStatus, pcfg->struMainRTMPConfig[nCh].cIsPushAudioEnable);
				break;
			case 3:
				if(0 == pcfg->struSub3RTMPConfig[nCh].bValid)
				{
					MPU_DBG("0 == pcfg->bValid, nCh = %d\n", nCh);
					return IDVR_CIU_STRUCT_UNVALID;					
				}
				MPU_AVL_Record_SetRtmpParam(3, nCh,  pcfg->struSub3RTMPConfig[nCh].szUserName,pcfg->struSub3RTMPConfig[nCh].szPassword,
				pcfg->struSub3RTMPConfig[nCh].szStreamPullingAddress,pcfg->struSub3RTMPConfig[nCh].dwEnable,\
				pcfg->struSub3RTMPConfig[nCh].dwStatus, pcfg->struMainRTMPConfig[nCh].cIsPushAudioEnable);
				break;
			case 4:
				if(0 == pcfg->struSub4RTMPConfig[nCh].bValid)
				{
					MPU_DBG("0 == pcfg->bValid, nCh = %d\n", nCh);
					return IDVR_CIU_STRUCT_UNVALID;					
				}
				MPU_AVL_Record_SetRtmpParam(4, nCh,  pcfg->struSub4RTMPConfig[nCh].szUserName,pcfg->struSub4RTMPConfig[nCh].szPassword,
				pcfg->struSub4RTMPConfig[nCh].szStreamPullingAddress,pcfg->struSub4RTMPConfig[nCh].dwEnable,\
				pcfg->struSub4RTMPConfig[nCh].dwStatus, pcfg->struMainRTMPConfig[nCh].cIsPushAudioEnable);
				break;
			case 5:
				if(0 == pcfg->struSub5RTMPConfig[nCh].bValid)
				{
					MPU_DBG("0 == pcfg->bValid, nCh = %d\n", nCh);
					return IDVR_CIU_STRUCT_UNVALID;					
				}
				MPU_AVL_Record_SetRtmpParam(5, nCh,  pcfg->struSub5RTMPConfig[nCh].szUserName,pcfg->struSub5RTMPConfig[nCh].szPassword,
				pcfg->struSub5RTMPConfig[nCh].szStreamPullingAddress,pcfg->struSub5RTMPConfig[nCh].dwEnable,\
				pcfg->struSub5RTMPConfig[nCh].dwStatus, pcfg->struMainRTMPConfig[nCh].cIsPushAudioEnable);
				break;
			default:
				MPU_DBG("Don Not support line = %d\n", __LINE__);

				break;				
		}
}

static int GetConnect_param(int tpye, int nCh, HY_DVR_RTMP_PUSH_CONFIG *pcfg)
{
	if(NULL == pcfg)
	{
		return IDVR_ERROR_INVALID_POINTER;
	}
	switch(tpye)
		{
			case 0:
				pcfg->struMainRTMPConfig[nCh].bValid = 1;
				
				MPU_AVL_Record_GetRtmpParam(0, nCh,  pcfg->struMainRTMPConfig[nCh].szUserName, sizeof(pcfg->struMainRTMPConfig[nCh].szUserName), pcfg->struMainRTMPConfig[nCh].szPassword, sizeof(pcfg->struMainRTMPConfig[nCh].szPassword), \
				pcfg->struMainRTMPConfig[nCh].szStreamPullingAddress, sizeof(pcfg->struMainRTMPConfig[nCh].szStreamPullingAddress), &pcfg->struMainRTMPConfig[nCh].dwEnable,\
				&pcfg->struMainRTMPConfig[nCh].dwStatus,pcfg->struMainRTMPConfig[nCh].szPlayURL, sizeof(pcfg->struMainRTMPConfig[nCh].szPlayURL), &pcfg->struMainRTMPConfig[nCh].cIsPushAudioEnable);
				break;
			case 1:
				pcfg->struSub1RTMPConfig[nCh].bValid = 1;
				
				MPU_AVL_Record_GetRtmpParam(1, nCh,  pcfg->struSub1RTMPConfig[nCh].szUserName, sizeof(pcfg->struSub1RTMPConfig[nCh].szUserName), pcfg->struSub1RTMPConfig[nCh].szPassword, sizeof(pcfg->struSub1RTMPConfig[nCh].szPassword), 
				pcfg->struSub1RTMPConfig[nCh].szStreamPullingAddress, sizeof(pcfg->struSub1RTMPConfig[nCh].szStreamPullingAddress), &pcfg->struSub1RTMPConfig[nCh].dwEnable,\
				&pcfg->struSub1RTMPConfig[nCh].dwStatus,pcfg->struSub1RTMPConfig[nCh].szPlayURL, sizeof(pcfg->struSub1RTMPConfig[nCh].szPlayURL), &pcfg->struMainRTMPConfig[nCh].cIsPushAudioEnable);
				break;
			case 2:
				pcfg->struSub2RTMPConfig[nCh].bValid = 1;
				
				MPU_AVL_Record_GetRtmpParam(2, nCh,  pcfg->struSub2RTMPConfig[nCh].szUserName,sizeof(pcfg->struSub2RTMPConfig[nCh].szUserName), pcfg->struSub2RTMPConfig[nCh].szPassword, sizeof(pcfg->struSub2RTMPConfig[nCh].szPassword),
				pcfg->struSub2RTMPConfig[nCh].szStreamPullingAddress,sizeof(pcfg->struSub2RTMPConfig[nCh].szStreamPullingAddress), &pcfg->struSub2RTMPConfig[nCh].dwEnable,\
				&pcfg->struSub2RTMPConfig[nCh].dwStatus,pcfg->struSub2RTMPConfig[nCh].szPlayURL,sizeof(pcfg->struSub2RTMPConfig[nCh].szPlayURL), &pcfg->struMainRTMPConfig[nCh].cIsPushAudioEnable);
				break;
			case 3:
				pcfg->struSub3RTMPConfig[nCh].bValid = 1;
				
				MPU_AVL_Record_GetRtmpParam(3, nCh,  pcfg->struSub3RTMPConfig[nCh].szUserName, sizeof(pcfg->struSub3RTMPConfig[nCh].szUserName), pcfg->struSub3RTMPConfig[nCh].szPassword, sizeof(pcfg->struSub3RTMPConfig[nCh].szPassword),
				pcfg->struSub3RTMPConfig[nCh].szStreamPullingAddress, sizeof(pcfg->struSub3RTMPConfig[nCh].szStreamPullingAddress), &pcfg->struSub3RTMPConfig[nCh].dwEnable,\
				&pcfg->struSub3RTMPConfig[nCh].dwStatus,pcfg->struSub3RTMPConfig[nCh].szPlayURL, sizeof(pcfg->struSub3RTMPConfig[nCh].szPlayURL), &pcfg->struMainRTMPConfig[nCh].cIsPushAudioEnable);
				break;
			case 4:
				pcfg->struSub4RTMPConfig[nCh].bValid = 1;
				
				MPU_AVL_Record_GetRtmpParam(4, nCh,  pcfg->struSub4RTMPConfig[nCh].szUserName,sizeof(pcfg->struSub4RTMPConfig[nCh].szUserName), pcfg->struSub4RTMPConfig[nCh].szPassword, sizeof(pcfg->struSub4RTMPConfig[nCh].szPassword),
				pcfg->struSub4RTMPConfig[nCh].szStreamPullingAddress, sizeof(pcfg->struSub4RTMPConfig[nCh].szStreamPullingAddress), &pcfg->struSub4RTMPConfig[nCh].dwEnable,\
				&pcfg->struSub4RTMPConfig[nCh].dwStatus,pcfg->struSub4RTMPConfig[nCh].szPlayURL, sizeof(pcfg->struSub4RTMPConfig[nCh].szPlayURL), &pcfg->struMainRTMPConfig[nCh].cIsPushAudioEnable);
				break;
			case 5:
				pcfg->struSub5RTMPConfig[nCh].bValid = 1;
				 
				MPU_AVL_Record_GetRtmpParam(5, nCh,  pcfg->struSub5RTMPConfig[nCh].szUserName,sizeof(pcfg->struSub5RTMPConfig[nCh].szUserName), pcfg->struSub5RTMPConfig[nCh].szPassword,sizeof(pcfg->struSub5RTMPConfig[nCh].szPassword),
				pcfg->struSub5RTMPConfig[nCh].szStreamPullingAddress,sizeof(pcfg->struSub5RTMPConfig[nCh].szStreamPullingAddress),&pcfg->struSub5RTMPConfig[nCh].dwEnable,\
				&pcfg->struSub5RTMPConfig[nCh].dwStatus,pcfg->struSub5RTMPConfig[nCh].szPlayURL, sizeof(pcfg->struSub5RTMPConfig[nCh].szPlayURL), &pcfg->struMainRTMPConfig[nCh].cIsPushAudioEnable);
				break;
			default:
				MPU_DBG("Don Not support line = %d\n", __LINE__);

				break;				
		}
}

//设置设备缓冲区信息，需外部将pcfg清零
static int SetRtmpPushCfg(HY_DVR_RTMP_PUSH_CONFIG *pcfg)
{
	int nRet = IDVR_OK;

	int nCh = 0;
	int nCount = 0;
	///////////////////////////////////////////////////////////////////////////////
	//本结构体是否有效，=0表示无效
	if (0 == pcfg->bValid)
	{
		MPU_DBG("0 == pcfg->bValid, nCh = %d\n", nCh);
		return IDVR_CIU_STRUCT_UNVALID;
	}
	
	//设置录像通道总数
	nCount = MAX_ENC_CHANNEL_NUM;
	if (nCount > DVR_MAX_VIDEOIN_NUM)
	{
		MPU_ERR("RtmpPushCfg %d, real %d\n", DVR_MAX_VIDEOIN_NUM, nCount);
		nCount = DVR_MAX_VIDEOIN_NUM;
	}

	//设置是否从服务器获取推流地址
	SetPushAddressCfg(pcfg->bRetrievePushAddressEnable,pcfg->szPushURLServer);
	
	//设置激活码,换位置了，放到系统设置里了
//	SetActionCode(pcfg->szActiveCode);
	///////////////////////////////////////////////////////////////////////////////
	//设置各通道参数配置
	for (nCh = 0; nCh < nCount; ++nCh)
	{
		//各通参数
		SetConnect_param(0, nCh, pcfg);
		SetConnect_param(1, nCh, pcfg);
		SetConnect_param(2, nCh, pcfg);
		SetConnect_param(3, nCh, pcfg);
		SetConnect_param(4, nCh, pcfg);
		SetConnect_param(5, nCh, pcfg);
	}
	
	//上面参数不一致的时候，就已经尝试重连，所以这里没有必要再重新连吧
	//Notif_Creat_Channel(&nRet);

	return nRet;
}

//获取设备缓冲区信息，需外部将pcfg清零
static int GetRtmpPushCfg(HY_DVR_RTMP_PUSH_CONFIG *pcfg)
{
	int nRet = IDVR_OK;

	int nCh = 0;
	int nCount = 0;

	if(NULL == pcfg)
	{
		return IDVR_CIU_STRUCT_UNVALID;
	}

	pcfg->bValid = 1;
//	GetActionCode(pcfg->szActiveCode);

	//设置是否从服务器获取推流地址
	//brief:设备是否支持，是否使能从服务器获取，获取到的服务器URL
	GetPushAddressCfg(&pcfg->bSupportDynRetrievePushAddress, &pcfg->bRetrievePushAddressEnable, pcfg->szPushURLServer);
	
	//获取录像通道总数
	nCount = MAX_ENC_CHANNEL_NUM;
	if (nCount > DVR_MAX_VIDEOIN_NUM)
	{
		MPU_ERR("RtmpPushCfg %d, real %d\n", DVR_MAX_VIDEOIN_NUM, nCount);
		nCount = DVR_MAX_VIDEOIN_NUM;
	}
	///////////////////////////////////////////////////////////////////////////////
	//获取各通道参数配置
	for (nCh = 0; nCh < nCount; ++nCh)
	{
		//各通参数
		GetConnect_param(0, nCh, pcfg);
		GetConnect_param(1, nCh, pcfg);
		GetConnect_param(2, nCh, pcfg);
		GetConnect_param(3, nCh, pcfg);
		GetConnect_param(4, nCh, pcfg);
		GetConnect_param(5, nCh, pcfg);
	}

	return nRet;
}
#endif

//获取编码能力，需外部将缓冲区清零
static int GetCompressCap(int nCh, LPHY_DVR_COMPRESS_CAP pcfg)
{
	int nRet = IDVR_OK;

	DEVICECAP dc;

	///////////////////////////////////////////////////////////////////////////////
	//本结构体是否有效，=0表示无效
	pcfg->bValid = 1;

	//获取设备能力
	MPU_PUB_GetDeviceCap(&dc);

	/*支持的码流格式， 所有支持的格式相或的结果 */
	pcfg->nStreamFormatCap = DVR_CAP_NORMAL_H264 | DVR_CAP_ADVANCED_H264;

	/*每种编码格式下支持的分辨率格式,支持所有的格式相或的结果 */
	if (dc.m_bytVideoSteamType[nCh] & STREAM_MAIN)
	{
		pcfg->dwResolutionCap[0] = dc.m_dwMainVideoSreamResolutionCap[nCh];
	}

	if (dc.m_bytVideoSteamType[nCh] & STREAM_SUB)
	{
		//子码流
		pcfg->dwResolutionCap[1] = dc.m_dwSubVideoSreamResolutionCap[nCh];
	}

	return nRet;
}

//获取各通道子码流参数，需外部将缓冲区清零
static int GetCompressInfo_Aux(int nCh, LPHY_DVR_AUX_COMPRESSION_INFO pcfg)
{
	int nRet = IDVR_OK;

	///////////////////////////////////////////////////////////////////////////////
	//本结构体是否有效，=0表示无效
	pcfg->bValid = 1;

	/* 是否启用子码流 */
	//pcfg->bEnalbe = MPU_AVL_AuxCoder_IsEnable(nCh);
	pcfg->bEnalbe = 1;

	//帧率 1-20，ff为全帧率
	pcfg->nVideoFrameRate = MPU_AVL_AuxCoder_GetFrameRate(nCh);

	//码率
	pcfg->nVideoBitrate = MPU_AVL_AuxCoder_GetBitRate(nCh);
#if 1
	//分辨率
	pcfg->nResolution = MPU_AVL_AuxCoder_GetPictureFormat(nCh, NULL, NULL);
#endif
  MPU_DBG("nCh = %d, bEnalbe = %d, nVideoFrameRate = %d, nVideoBitrate = %d, nResolution = %d\n", 
  nCh, pcfg->bEnalbe, pcfg->nVideoFrameRate, pcfg->nVideoBitrate, pcfg->nResolution);

	return nRet;
}

//设置各通道子码流参数
static int SetCompressInfo_Aux(int nCh, LPHY_DVR_AUX_COMPRESSION_INFO pcfg)
{
	int nRet = IDVR_OK;

	///////////////////////////////////////////////////////////////////////////////
	//本结构体是否有效，=0表示无效
	if (0 == pcfg->bValid)
	{
		MPU_DBG("0 == pcfg->bValid, nCh = %d\n", nCh);
		return IDVR_CIU_STRUCT_UNVALID;
	}


	/* 是否启用子码流 */
	//MPU_AVL_AuxCoder_Enable(nCh, pcfg->bEnalbe);

	//帧率 1-20，ff为全帧率
	MPU_AVL_AuxCoder_SetFrameRate(nCh, pcfg->nVideoFrameRate);

	//码率
	MPU_AVL_AuxCoder_SetBitRate(nCh, pcfg->nVideoBitrate);

	//分辨率
	MPU_AVL_AuxCoder_SetPictureFormat(nCh, pcfg->nResolution, 0, 0);

	// 辅码流的I帧间隔
	MPU_AVL_SetKeyFrameInterval(nCh, 1, pcfg->nVideoFrameRate * 4);	

    MPU_DBG("nCh = %d, bEnalbe = %d, nVideoFrameRate = %d, nVideoBitrate = %d, nResolution = %d\n", 
    nCh, pcfg->bEnalbe, pcfg->nVideoFrameRate, pcfg->nVideoBitrate, pcfg->nResolution);

	return nRet;
}

static int TutkGetWhichMedia(LPZW_TUTK_STREAMINDEX pcfg)
{
	int whichMedia;

	pcfg->bValid = 1;
	pcfg->dwMaxVideoInCnt = 1;
	MPU_AVL_TUTK_GetMedia(0, &whichMedia);
	pcfg->stream[0].bValid = 1;
	pcfg->stream[0].dwWhichStream = whichMedia;
	
	MPU_DBG("--TutkGetWhichMedia----whichMedia=%d----\n",whichMedia);
	
	return 0;
}

static int TutkSetQualityCfg(DWORD dwUserID, LPZW_TUTK_QUALITY pcfg)
{

	if (0 == pcfg->bValid)
	{
		MPU_DBG("0 == pcfg->bValid\n");
		return IDVR_CIU_STRUCT_UNVALID;
	}
	/*if (pcfg->dwMaxVieoInCnt > 0)
	{
		MPU_DBG("pcfg->dwMaxVieoInCnt <= 0\n");
		return IDVR_CIU_STRUCT_UNVALID;
	}*/

	if(pcfg->quality[0].bValid)
	{
		MPU_DBG("--TutkSetQualityCfg---quality=%d----\n",(int)(pcfg->quality[0].dwVideoQulity));
		MPU_AVL_TUTK_SetQualityCfg(0, (int)(pcfg->quality[0].dwVideoQulity));
	}
	else
		MPU_DBG("--TutkSetQualityCfg-----pcfg->quality[0].bValid==0------\n");

	return 0;
}

static int TutkGetQualityCfg(LPZW_TUTK_QUALITY pcfg)
{
	int TUTKQuality;
		
	pcfg->bValid = 1;
	pcfg->dwMaxVieoInCnt = 1;
	pcfg->quality[0].bValid = 1;
	
	MPU_AVL_TUTK_GetQualityCfg(0, &TUTKQuality);
	
	switch (TUTKQuality)
	{
		case 1:
			pcfg->quality[0].dwVideoQulity = zwAVIOCTRL_QUALITY_MAX;break;
		case 2:
			pcfg->quality[0].dwVideoQulity = zwAVIOCTRL_QUALITY_HIGH;break;
		case 3:
			pcfg->quality[0].dwVideoQulity = zwAVIOCTRL_QUALITY_MIDDLE;break;
		case 4:
			pcfg->quality[0].dwVideoQulity = zwAVIOCTRL_QUALITY_LOW;break;
		case 5:
			pcfg->quality[0].dwVideoQulity = zwAVIOCTRL_QUALITY_MIN;break;
		default:
			pcfg->quality[0].dwVideoQulity = zwAVIOCTRL_QUALITY_UNKNOWN;
	}

	MPU_DBG("--TutkGetQualityCfg---quality=%d----\n", TUTKQuality);
	
	return 0;
}


static int TutkGetQualityMap(LPHY_DVR_COMPRESSION_QUALITY_AND_STREAM_TYPE_TABLE pcfg)
{
	int ret;
	int i;
	pcfg->bValid = 1;
	pcfg->struQualityAndStreamTypeTableChannel[0].nValid = 1;
	int nCount = 0;
	for(i=1; i<=MAX_QUALITY_STREAM_MAP_COUNT; i++)
		{
			pcfg->struQualityAndStreamTypeTableChannel[0].struQualityAndStreamType[i-1].dwQuality = i;
			ret = MPU_AVL_TUTK_GetQualityboardCfg(0, i, &pcfg->struQualityAndStreamTypeTableChannel[0].struQualityAndStreamType[i-1].dwStreamType);
			if(ret<0)
				{
					pcfg->struQualityAndStreamTypeTableChannel[0].nValid = 0;
					break;
				}
			nCount++;
			MPU_DBG("--TutkGetQualityMap---quality=%d  StreamType=%d----\n", \
			pcfg->struQualityAndStreamTypeTableChannel[0].struQualityAndStreamType[i-1].dwQuality,\
			pcfg->struQualityAndStreamTypeTableChannel[0].struQualityAndStreamType[i-1].dwStreamType);
		}	
	pcfg->struQualityAndStreamTypeTableChannel[0].dwRealCount = nCount;
	
	return 0;
}

//获取各通道主码流参数，需外部将缓冲区清零
static int GetCompressInfo_Main(int nCh, int nType, LPHY_DVR_COMPRESSION_INFO pcfg, int *nVideoW, int *nVideoH)
{
	int nRet = IDVR_OK, nSDKRet;
	static int isSupport6MP = 0;
	int nBoardID = 0;

	tagEncCfg EncCfg;

	ZWSys_GetBoardInfo(&nBoardID);
	///////////////////////////////////////////////////////////////////////////////
	//本结构体是否有效，=0表示无效
	pcfg->bValid = 1;

	nSDKRet = MPU_AVL_Record_GetEncCfg(nCh, &EncCfg, nType);
	MPU_CHK(0 == nSDKRet);

	//帧率 1-20，ff为全帧率
	pcfg->nVideoFrameRate = EncCfg.dwVideoFrameRate;

	//码率上限。单位K，
	pcfg->nVideoBitrate = EncCfg.dwVideoBitrateH;

	//码率下限
	//pcfg->nVideoBitrateL = EncCfg.dwVideoBitrateL;

	//量化系数:0-6
	pcfg->nQuotiety = EncCfg.dwQuotiety;

	//分辨率0-CIF, 1-D1
    if (nVideoW != NULL && nVideoH != NULL)
    {		
		if((0 == isSupport6MP) && (3200 == EncCfg.dwResWidth && 1800 == EncCfg.dwResHeight))
		{
			//避免设置成6MP后关闭定制或者升级其它的定制版本导致web显示的分辨率为空
			*nVideoW = 2880;
        	*nVideoH = 1624;
		}
		else
		{
			*nVideoW = EncCfg.dwResWidth;
        	*nVideoH = EncCfg.dwResHeight;
		}
    }

	//码流类型0-视频流,1-复合流
	pcfg->nStreamType = EncCfg.byStreamType;

	//录像质量 0-高质量 1-标准 2-低 3-自定义 
	//	IE层已去掉该项 GUI和中间层已固定为3
	//pcfg->nPicQuality = EncCfg.byPicQuality;

	//码率类型0: 变码率率，1: 定码，2：限定码率
	pcfg->nBitrateType = EncCfg.byBitrateType;

	return nRet;
}

//设置各通道主码流参数
static int SetCompressInfo_Main(int nCh, int nType, LPHY_DVR_COMPRESSION_INFO pcfg, int nVideoW, int nVideoH)
{
	int nRet = IDVR_OK, nSDKRet;

	tagEncCfg EncCfg;

	///////////////////////////////////////////////////////////////////////////////
	//本结构体是否有效，=0表示无效
	if (0 == pcfg->bValid)
	{
		MPU_DBG("0 == pcfg->bValid, nCh = %d, nType = %d\n", nCh, nType);
		return IDVR_CIU_STRUCT_UNVALID;
	}
	MPU_DBG("--FrameRate %d--Bitrate %d--nQuotiety %d--nResolution %d--nStreamType %d--nBitrateType %d----\n",
		 pcfg->nVideoFrameRate, pcfg->nVideoBitrate, pcfg->nQuotiety, pcfg->nResolution,
		 pcfg->nStreamType, pcfg->nBitrateType);

	memset(&EncCfg, 0, sizeof (EncCfg));
	//帧率 1-20，ff为全帧率
	EncCfg.dwVideoFrameRate = pcfg->nVideoFrameRate;

	//码率上限。单位K，
	if(pcfg->nVideoBitrate >= 64000)//如果码率超过这个，则认为是客户端把单位搞错了，做下兼容
	{
		EncCfg.dwVideoBitrateH = pcfg->nVideoBitrate / 1000;
	}
	else
	{
		EncCfg.dwVideoBitrateH = pcfg->nVideoBitrate;
	}

	//码率下限
	//EncCfg.dwVideoBitrateL = pcfg->nVideoBitrateL;
	EncCfg.dwVideoBitrateL	 = 0;

	//量化系数:0-6
	EncCfg.dwQuotiety = pcfg->nQuotiety;

	//分辨率0-CIF, 1-D1
    fprintf(stderr, "[%s][%d] ********zxr main nVideoW=[%d] nVideoH=[%d]*****\r\n",__FUNCTION__,__LINE__, nVideoW, nVideoH);
    if (nVideoW > 0 && nVideoH > 0)
    {
        EncCfg.dwResWidth = nVideoW;        
        EncCfg.dwResHeight = nVideoH;
    }
    else
    {
        fprintf(stderr, "[%s][%d] invalid param.w=%d h=%d*****\r\n",__FUNCTION__,__LINE__, nVideoW, nVideoH);
        return IDVR_CIU_STRUCT_UNVALID;
    }

	//码流类型0-视频流,1-复合流
	EncCfg.byStreamType = pcfg->nStreamType;

	//录像质量 0-高质量 1-标准 2-低 3-自定义
	//	IE层已去掉该项  GUI和中间层已固定为3
//	EncCfg.byPicQuality = pcfg->nPicQuality;
	EncCfg.byPicQuality = 3;

	//码率类型0: 变码率率，1: 定码，2：限定码率
	EncCfg.byBitrateType = pcfg->nBitrateType;

	/* 设置下主码流I帧间隔 */
	nSDKRet = MPU_AVL_SetKeyFrameInterval(nCh, 0, pcfg->nVideoFrameRate * 4);	
	MPU_CHK(0 == nSDKRet);

	nSDKRet = MPU_AVL_Record_SetEncCfg(nCh, &EncCfg, nType);
	MPU_CHK(0 == nSDKRet);

	return nRet;
}

#ifdef USE_HDAUX

//获取各通道高清子码流参数，需外部将缓冲区清零
static int GetCompressInfo_HDAux(int nCh, LPHY_DVR_COMPRESSION_INFO pcfg, int *nVideoW, int *nVideoH)
{
	int nRet = IDVR_OK, nSDKRet;

	tagEncCfg EncCfg;

	///////////////////////////////////////////////////////////////////////////////
	//本结构体是否有效，=0表示无效
	pcfg->bValid = 1;

	memset(&EncCfg, 0, sizeof (EncCfg));
	nSDKRet = MPU_AVL_HDAuxCoder_GetEncCfg(nCh, &EncCfg);
	MPU_CHK(0 == nSDKRet);
	if(1 == nSDKRet)//无第二子码流
	{
		pcfg->bValid = 0;
		return IDVR_OK;
	}

	//帧率 1-20，ff为全帧率
	pcfg->nVideoFrameRate = EncCfg.dwVideoFrameRate;

	//码率上限。单位K，
	pcfg->nVideoBitrate = EncCfg.dwVideoBitrateH;


	//量化系数:0-6
	pcfg->nQuotiety = EncCfg.dwQuotiety;

	//分辨率0-CIF, 1-D1
    if (nVideoW != NULL && nVideoH != NULL)
    {
        *nVideoW = EncCfg.dwResWidth;
        *nVideoH = EncCfg.dwResHeight;
    }

	//码流类型0-视频流,1-复合流
	pcfg->nStreamType = EncCfg.byStreamType;

	//码率类型0: 变码率率，1: 定码，2：限定码率
	pcfg->nBitrateType = EncCfg.byBitrateType;

	MPU_DBG("get--HDAux--FrameRate %d--Bitrate %d--nQuotiety %d--nResolution %d--nStreamType %d--nBitrateType %d----\n",
		 pcfg->nVideoFrameRate, pcfg->nVideoBitrate, pcfg->nQuotiety, pcfg->nResolution,
		 pcfg->nStreamType, pcfg->nBitrateType);

	return nRet;
}

//设置各通道高清子码流参数
static int SetCompressInfo_HDAux(int nCh, LPHY_DVR_COMPRESSION_INFO pcfg, int nVideoW, int nVideoH)
{
	int nRet = IDVR_OK, nSDKRet;

	tagEncCfg EncCfg;

	MPU_DBG("set--HDAux--FrameRate %d--Bitrate %d--nQuotiety %d--nResolution %d--nStreamType %d--nBitrateType %d----\n",
		 pcfg->nVideoFrameRate, pcfg->nVideoBitrate, pcfg->nQuotiety, pcfg->nResolution,
		 pcfg->nStreamType, pcfg->nBitrateType);

	///////////////////////////////////////////////////////////////////////////////
	//本结构体是否有效，=0表示无效
	if (0 == pcfg->bValid)
	{
		MPU_DBG("0 == pcfg->bValid, nCh = %d\n", nCh);
		return IDVR_CIU_STRUCT_UNVALID;
	}

	memset(&EncCfg, 0, sizeof (EncCfg));

	//帧率 1-20，ff为全帧率
	EncCfg.dwVideoFrameRate = pcfg->nVideoFrameRate;

	//码率上限。单位K，
	EncCfg.dwVideoBitrateH = pcfg->nVideoBitrate;

	//码率下限
	//EncCfg.dwVideoBitrateL = pcfg->nVideoBitrateL;
	EncCfg.dwVideoBitrateL	 = 0;


	//量化系数:0-6
	EncCfg.dwQuotiety = pcfg->nQuotiety;

	//分辨率0-CIF, 1-D1
    if (nVideoW > 0 && nVideoH > 0)
    {
        EncCfg.dwResWidth = nVideoW;        
        EncCfg.dwResHeight = nVideoH;
    }
    else
    {      
        fprintf(stderr, "[%s][%d] invalid param.w=%d h=%d*****\r\n",__FUNCTION__,__LINE__, nVideoW, nVideoH);
        return IDVR_CIU_STRUCT_UNVALID;
    }

	//码流类型0-视频流,1-复合流
	EncCfg.byStreamType = pcfg->nStreamType;

	//码率类型0: 变码率率，1: 定码，2：限定码率
	EncCfg.byBitrateType = pcfg->nBitrateType;

	nSDKRet = MPU_AVL_SetKeyFrameInterval(nCh, 2, pcfg->nVideoFrameRate * 4);	
	MPU_CHK(0 == nSDKRet);

	nSDKRet = MPU_AVL_HDAuxCoder_SetEncCfg(nCh, &EncCfg);
	MPU_CHK(0 == nSDKRet);

	return nRet;
}

#endif

//获取各通道编码参数，需外部将缓冲区清零
static int GetCompressChannelCfg(int nCh, LPHY_DVR_COMPRESSION_CHANNEL pcfg)
{
	int nRet = IDVR_OK;

	///////////////////////////////////////////////////////////////////////////////
	//本结构体是否有效，=0表示无效
	pcfg->bValid = 1;

	/* 录象延迟时间长度 */
	pcfg->nDelayRecordTime = MPU_AVL_Record_GetDelayTime(nCh);

	/* 预录时间 0-不预录 1-5秒 2-10秒 3-15秒 4-20秒 5-25秒 */
	pcfg->nPreRecordTime = MPU_AVL_Record_GetPreTime(nCh);

	//MPU_DBG("nCh %d: nDelayRecordTime = %d, nPreRecordTime = %d\n", nCh, pcfg->nDelayRecordTime, pcfg->nPreRecordTime);

	///////////////////////////////////////////////////////////////////////////////
	/*压缩能力， 仅在获取参数时有效（只读） */
	GetCompressCap(nCh, &pcfg->struCompressCap);

	/* 子码流 */
	GetCompressInfo_Aux(nCh, &pcfg->struNetPara);
	
#ifdef USE_HDAUX
		GetCompressInfo_HDAux(nCh, &pcfg->struVGANetPara, NULL, NULL);
#endif

	/*0基本配置，2事件录像，1手动录像 */
	GetCompressInfo_Main(nCh, 0, &pcfg->struNormalRecordPara, NULL, NULL);
	GetCompressInfo_Main(nCh, 2, &pcfg->struEventRecordPara, NULL, NULL);

	return nRet;
}

//获取各通道编码参数，需外部将缓冲区清零
static int GetCompressChannelCfgEx(int nCh, LPHY_DVR_COMPRESSION_CHANNEL_EX pcfg)
{
	int nRet = IDVR_OK;
	int value = 0;

	///////////////////////////////////////////////////////////////////////////////
	//本结构体是否有效，=0表示无效
	pcfg->bValid = 1;

	//主码流
	pcfg->struNormalRecordPara.bValid = 1;
	nRet = MPU_AVL_GetKeyFrameInterval(nCh, 0, &value);	
	MPU_CHK(0 == nRet);
	pcfg->struNormalRecordPara.nKeyFrameInterval = value;

	//子码流
	pcfg->struNetPara.bValid = 1;	
	nRet = MPU_AVL_GetKeyFrameInterval(nCh, 1, &value);	
	MPU_CHK(0 == nRet);
	pcfg->struNetPara.nKeyFrameInterval = value;

	//invalid
	pcfg->struEventRecordPara.bValid = 0;

	//第二子码流
	pcfg->struVGANetPara.bValid = 1;
	nRet = MPU_AVL_GetKeyFrameInterval(nCh, 2, &value);	
	MPU_CHK(0 == nRet);
	pcfg->struVGANetPara.nKeyFrameInterval = value;

	//audio in vol
	pcfg->cVolume = MPU_AVL_GetAudioInVol(nCh);

	//audio out vol
	pcfg->cEnableAudioOutVolume = 1;
	pcfg->cAudioOutVolume = MPU_AVL_GetAudioOutVol(nCh);

	return nRet;
}

static int SetCompressChannelCfgEx(int nCh, LPHY_DVR_COMPRESSION_CHANNEL_EX pcfg)
{
	int nSDKRet;

	///////////////////////////////////////////////////////////////////////////////
	//本结构体是否有效，=0表示无效
	if (0 == pcfg->bValid)
	{
		MPU_DBG("0 == pcfg->bValid, nCh = %d\n", nCh);
		return IDVR_CIU_STRUCT_UNVALID;
	}

	if (pcfg->cVolume < 0 || pcfg->cVolume > 100)
	{
		MPU_DBG("0 == pcfg->bValid, nCh = %d\n", nCh);
		return IDVR_ERROR_PARAMETER;
	}		

	nSDKRet = MPU_AVL_SetAudioInVol(nCh, pcfg->cVolume);
	MPU_CHK(0 == nSDKRet);

	nSDKRet = MPU_AVL_SetAudioOutVol(nCh, pcfg->cAudioOutVolume);
	MPU_CHK(0 == nSDKRet);

	return nSDKRet;
}

//设置各通道编码参数
static int SetCompressChannelCfg(int nCh, LPHY_DVR_COMPRESSION_CHANNEL pcfg)
{
	int nRet = IDVR_OK, nSDKRet;

	///////////////////////////////////////////////////////////////////////////////
	//本结构体是否有效，=0表示无效
	if (0 == pcfg->bValid)
	{
		MPU_DBG("0 == pcfg->bValid, nCh = %d\n", nCh);
		return IDVR_CIU_STRUCT_UNVALID;
	}


	/* 录象延迟时间长度 */
	nSDKRet = MPU_AVL_Record_SetDelayTime(nCh, pcfg->nDelayRecordTime);
	MPU_CHK(0 == nSDKRet);

	/* 预录时间 0-最大预录时间，其它为实际时间 */
	nSDKRet = MPU_AVL_Record_SetPreTime(nCh, pcfg->nPreRecordTime);
	MPU_CHK(0 == nSDKRet);

	MPU_DBG("nCh %d: nDelayRecordTime = %d, nPreRecordTime = %d\n", nCh, pcfg->nDelayRecordTime, pcfg->nPreRecordTime);

	/* 子码流 */
	SetCompressInfo_Aux(nCh, &pcfg->struNetPara);

	/*0基本配置,2事件录像,1手动录像 */
	SetCompressInfo_Main(nCh, 0, &pcfg->struNormalRecordPara, 0, 0);
	SetCompressInfo_Main(nCh, 2, &pcfg->struEventRecordPara, 0, 0);
	
#ifdef USE_HDAUX
	SetCompressInfo_HDAux(nCh, &pcfg->struVGANetPara, 0, 0);
#endif

	return nRet;
}

//获取编码参数，需外部将缓冲区清零
static int GetCompressCfg(LPHY_DVR_COMPRESSION_CFG pcfg)
{
	int nRet = IDVR_OK, nCh, nCount;

	///////////////////////////////////////////////////////////////////////////////
	//本结构体是否有效，=0表示无效
	pcfg->bValid = 1;


	///////////////////////////////////////////////////////////////////////////////
	//公共参数
	//GetDevCustom_Record(&pcfg->struWorkMode);


	//获取录像通道总数
	nCount = MAX_ENC_CHANNEL_NUM;
	if (nCount > DVR_MAX_VIDEOIN_NUM)
	{
		MPU_ERR("remote config record %d, real %d\n", DVR_MAX_VIDEOIN_NUM, nCount);
		nCount = DVR_MAX_VIDEOIN_NUM;
	}
	
	for (nCh = 0; nCh < nCount; ++nCh)
	{
		//各通道压缩参数
		GetCompressChannelCfg(nCh, &pcfg->struCompressChannel[nCh]);
	}

	return nRet;
}

//获取编码参数，需外部将缓冲区清零
static int GetCompressCfgEx(LPHY_DVR_COMPRESSION_CFG_EX pcfg)
{
	int nCh, nCount;

	///////////////////////////////////////////////////////////////////////////////
	//本结构体是否有效，=0表示无效
	pcfg->bValid = 1;

	nCount = MAX_ENC_CHANNEL_NUM;
	if (nCount > DVR_MAX_VIDEOIN_NUM)
	{
		MPU_ERR("remote config record %d, real %d\n", DVR_MAX_VIDEOIN_NUM, nCount);
		nCount = DVR_MAX_VIDEOIN_NUM;
	}
	
	for (nCh = 0; nCh < nCount; ++nCh)
	{
		//各通道压缩参数
		GetCompressChannelCfgEx(nCh, &pcfg->struCompressChannelEx[nCh]);
	}

	return 0;
}

static int SetCompressCfgEx(DWORD dwUserID,LPHY_DVR_COMPRESSION_CFG_EX pcfg)
{
	int nRet = IDVR_OK, nCh, nCount, nSDKRet;

	if (0 == pcfg->bValid)
	{
		MPU_DBG("0 == pcfg->bValid\n");
		return IDVR_CIU_STRUCT_UNVALID;
	}

	nCount = MAX_ENC_CHANNEL_NUM;
	if (nCount > DVR_MAX_VIDEOIN_NUM)
	{
		MPU_ERR("remote config record %d, real %d\n", DVR_MAX_VIDEOIN_NUM, nCount);
		nCount = DVR_MAX_VIDEOIN_NUM;
	}
	
	for (nCh = 0; nCh < nCount; ++nCh)
	{
		nSDKRet = SetCompressChannelCfgEx(nCh, &pcfg->struCompressChannelEx[nCh]);
		CIU_ERR(nSDKRet, nRet);

		if (IDVR_OK == nSDKRet)
		{
			//记录日志，设置编码参数
			CIU_OPL_AddLog(dwUserID, EDVR_EVENT_SETTING, EDVR_EVENT_SETTING_DETAIL_CODEC_SETTING, nCh, 0);
		}
	}
	
	return nRet;
}

//接口定义
//插入数据接口
//1.插入指定通道主码流的分辨率和最大帧率数
int InsertMainResolutionAndMaxFrameRate(HY_DVR_CODEC_CAP_V3* pstCodecCapV3, int nChannel,HY_DVR_RESOLUTION_AND_FRAMERATE_INFO&  struResolutionAndFrameInfo)
{
    HY_DVR_MAIN_STREAM_RESOLUTION_V3 * pMainStreamResolution = new HY_DVR_MAIN_STREAM_RESOLUTION_V3();
    if(NULL == pMainStreamResolution)
    {
        printf("memory malloc failed\n");
        return -1;
    }
    
	pstCodecCapV3->bValid = 1;
	pstCodecCapV3->codec_cap[nChannel].bValid = 1;

    pMainStreamResolution->bValid = 1;
    pMainStreamResolution->struMainStreamResolutionAndFramerate = struResolutionAndFrameInfo;
    pMainStreamResolution->pNext = NULL;
    pMainStreamResolution->pSubStreamHead = NULL;

    HY_DVR_MAIN_STREAM_RESOLUTION_V3 * pTemp = pstCodecCapV3->codec_cap[nChannel].pMainStreamResolutionHead;

    if(NULL == pTemp)
    {
        pstCodecCapV3->codec_cap[nChannel].pMainStreamResolutionHead = pMainStreamResolution;
    }
    else
    {
        while(pTemp->pNext)
        {
            pTemp = pTemp->pNext;
        }
        pTemp->pNext = pMainStreamResolution;
    }
    return 0;
	
}

//2.插入指定通道，指定主码流对应支持的子码流的数
int InsertSubStreamByMainResolution(HY_DVR_CODEC_CAP_V3* pstCodecCapV3, int nChannel,int nMainStreamResWidth,int nMainStreamResHeight)
{
    HY_DVR_MAIN_STREAM_RESOLUTION_V3 * pMainStreamReolutionTemp = pstCodecCapV3->codec_cap[nChannel].pMainStreamResolutionHead;
    if(NULL == pMainStreamReolutionTemp)
    {
       printf("not Main Resolution Element\n");
       return -1; 
    }
    HY_DVR_MAIN_STREAM_RESOLUTION_V3 * pMainStreamResolution = NULL;
    while(pMainStreamReolutionTemp)
    {
        if(pMainStreamReolutionTemp->struMainStreamResolutionAndFramerate.nResolutionWidth == nMainStreamResWidth&&
           pMainStreamReolutionTemp->struMainStreamResolutionAndFramerate.nResolutionHeight == nMainStreamResHeight
           )
        {
            pMainStreamResolution = pMainStreamReolutionTemp;
        }
        pMainStreamReolutionTemp = pMainStreamReolutionTemp->pNext;
    }
    if(NULL == pMainStreamResolution)
    {
       printf("not find Main Resolution[%dX%d] Element\n",nMainStreamResWidth,nMainStreamResHeight);
       return -1; 
    }
    
    HY_DVR_SUB_STREAM * pSubStreamTemp = NULL;
    pSubStreamTemp = pMainStreamResolution->pSubStreamHead;

    HY_DVR_SUB_STREAM * pSubStream = new HY_DVR_SUB_STREAM();
    if(NULL == pSubStream)
    {
        printf("memory malloc failed\n");
        return -1;
    }
    pSubStream->bValid = 1;
    pSubStream->pSubStreamResolutionHead = NULL;
    pSubStream->pNext = NULL;

    if(NULL == pSubStreamTemp)
    {
        pMainStreamResolution->pSubStreamHead = pSubStream;
    }
    else
    {
        while(pSubStreamTemp->pNext)
        {
            pSubStreamTemp = pSubStreamTemp->pNext;
        }
        pSubStreamTemp->pNext = pSubStream;
    }
    return 0;
}

//3.插入指定通道，指定主码流对应的指定的子码流的分辨率和最大帧率的数据
int InsertSubStreamResolutionAndMaxFrameRateByMainResolution(HY_DVR_CODEC_CAP_V3* pstCodecCapV3, int nChannel,int nMainStreamResWidth,int nMainStreamResHeight,int nSubStreamType,HY_DVR_RESOLUTION_AND_FRAMERATE_INFO&  struResolutionAndFrameInfo)
{
    HY_DVR_MAIN_STREAM_RESOLUTION_V3 * pMainStreamReolutionTemp = pstCodecCapV3->codec_cap[nChannel].pMainStreamResolutionHead;
    if(NULL == pMainStreamReolutionTemp)
    {
       printf("not Main Resolution Element\n");
       return -1; 
    }
    HY_DVR_MAIN_STREAM_RESOLUTION_V3 * pMainStreamResolution = NULL;
    while(pMainStreamReolutionTemp)
    {
        if(pMainStreamReolutionTemp->struMainStreamResolutionAndFramerate.nResolutionWidth == nMainStreamResWidth&&
           pMainStreamReolutionTemp->struMainStreamResolutionAndFramerate.nResolutionHeight == nMainStreamResHeight
           )
        {
            pMainStreamResolution = pMainStreamReolutionTemp;
        }
        pMainStreamReolutionTemp = pMainStreamReolutionTemp->pNext;
    }
    if(NULL == pMainStreamResolution)
    {
       printf("not find Main Resolution[%dX%d] Element\n",nMainStreamResWidth,nMainStreamResHeight);
       return -1; 
    }
    
    HY_DVR_SUB_STREAM * pSubStreamTemp = NULL;
    pSubStreamTemp = pMainStreamResolution->pSubStreamHead;

    if(NULL == pSubStreamTemp)
    {
       printf("not find Sub Stream Type[%d] Element\n",nSubStreamType);
       return -1; 
    }
    int nInterSubStreamType = 0;
    HY_DVR_SUB_STREAM * pSubStream = NULL;
    while(pSubStreamTemp)
    {
        nInterSubStreamType++;
        if(nInterSubStreamType == nSubStreamType)
        {
            pSubStream = pSubStreamTemp;
        }
        pSubStreamTemp = pSubStreamTemp->pNext;
    }
    if(NULL == pSubStream)
    {
       printf("not find Sub Stream Type[%d] Element\n",nSubStreamType);
       return -1; 
    }

    HY_DVR_SUB_STREAM_RESOLUTION_V3 * pSubStreamResolution = new HY_DVR_SUB_STREAM_RESOLUTION_V3();
    if(NULL == pSubStreamResolution)
    {
        printf("memory malloc failed\n");
        return -1;
    }

    pSubStreamResolution->bValid = 1;
    pSubStreamResolution->pNext = NULL;
    pSubStreamResolution->struResolutionAndFrameInfo = struResolutionAndFrameInfo;

    HY_DVR_SUB_STREAM_RESOLUTION_V3 * pSubStreamResolutionTemp = pSubStream->pSubStreamResolutionHead;

    if(NULL == pSubStreamResolutionTemp)
    {
        pSubStream->pSubStreamResolutionHead = pSubStreamResolution;
    }
    else
    {
        while(pSubStreamResolutionTemp->pNext)
        {
            pSubStreamResolutionTemp = pSubStreamResolutionTemp->pNext;
        }
        pSubStreamResolutionTemp->pNext = pSubStreamResolution;
    }
    return 0; 
}

static int GetCompressCfgNewV3(HY_DVR_CODEC_CAP_V3 *pcfg)
{
	int max_framerate = 25;
	int nRet = IDVR_OK;

	HY_DVR_RESOLUTION_AND_FRAMERATE_INFO struMainResolutionAndFrameRateInfo;//主码流信息结构体
	HY_DVR_RESOLUTION_AND_FRAMERATE_INFO struSubResolutionAndFrameRateInfo;//子码流信息结构体

	/* 设置对外可调帧率最大为10帧 */
#if defined(__IS_SUPPORT_12FPS__)
	max_framerate = 12;
#else
	max_framerate = 10;
#endif

#if defined (__PRODUCT_P4__)
	//插入主码流支持的分辨率
	memset(&struMainResolutionAndFrameRateInfo, 0, sizeof(struMainResolutionAndFrameRateInfo));
	struMainResolutionAndFrameRateInfo.nResolutionWidth =  2560;
	struMainResolutionAndFrameRateInfo.nResolutionHeight = 1440;
	struMainResolutionAndFrameRateInfo.nMaxFrameRate = max_framerate;
	InsertMainResolutionAndMaxFrameRate(pcfg, 0, struMainResolutionAndFrameRateInfo);
#endif

#if defined(__PRODUCT_P4U__) 
		//插入主码流支持的分辨率
		memset(&struMainResolutionAndFrameRateInfo, 0, sizeof(struMainResolutionAndFrameRateInfo));
		struMainResolutionAndFrameRateInfo.nResolutionWidth =  2880;
		struMainResolutionAndFrameRateInfo.nResolutionHeight = 1620;
		struMainResolutionAndFrameRateInfo.nMaxFrameRate = max_framerate;
		InsertMainResolutionAndMaxFrameRate(pcfg, 0, struMainResolutionAndFrameRateInfo);
#endif


	//插入主码流支持的分辨率
#if defined (__PRODUCT_P3MU__)
    memset(&struMainResolutionAndFrameRateInfo, 0, sizeof(struMainResolutionAndFrameRateInfo));
    struMainResolutionAndFrameRateInfo.nResolutionWidth =  2304;
    struMainResolutionAndFrameRateInfo.nResolutionHeight = 1296;
    struMainResolutionAndFrameRateInfo.nMaxFrameRate = max_framerate;
    InsertMainResolutionAndMaxFrameRate(pcfg, 0, struMainResolutionAndFrameRateInfo);
#endif

	memset(&struMainResolutionAndFrameRateInfo, 0, sizeof(struMainResolutionAndFrameRateInfo));
	struMainResolutionAndFrameRateInfo.nResolutionWidth =  1920;
	struMainResolutionAndFrameRateInfo.nResolutionHeight = 1080;
	struMainResolutionAndFrameRateInfo.nMaxFrameRate = max_framerate;
	InsertMainResolutionAndMaxFrameRate(pcfg, 0, struMainResolutionAndFrameRateInfo);

	memset(&struMainResolutionAndFrameRateInfo, 0, sizeof(struMainResolutionAndFrameRateInfo));
	struMainResolutionAndFrameRateInfo.nResolutionWidth =  1280;
	struMainResolutionAndFrameRateInfo.nResolutionHeight = 720;
	struMainResolutionAndFrameRateInfo.nMaxFrameRate = max_framerate;
	InsertMainResolutionAndMaxFrameRate(pcfg, 0, struMainResolutionAndFrameRateInfo);

#if defined (__PRODUCT_P4__)
	//插入子码流支持的分辨率和最大码率的情况 
	//1920*1080
	//在主码流1920*1080分辨率下插入第一个子码流下所有的分辨率选项和最大帧率
	InsertSubStreamByMainResolution(pcfg, 0, 2560, 1440);//插入第一子码流节点
	memset(&struSubResolutionAndFrameRateInfo, 0, sizeof(struSubResolutionAndFrameRateInfo));
	struSubResolutionAndFrameRateInfo.nResolutionWidth =  800; 
	struSubResolutionAndFrameRateInfo.nResolutionHeight = 448;
	struSubResolutionAndFrameRateInfo.nMaxFrameRate = max_framerate;//max_framerate;
	InsertSubStreamResolutionAndMaxFrameRateByMainResolution(pcfg, 0, 2560, 1440, 1, struSubResolutionAndFrameRateInfo);

	InsertSubStreamByMainResolution(pcfg, 0, 1920, 1080);//插入第一子码流节点
	memset(&struSubResolutionAndFrameRateInfo, 0, sizeof(struSubResolutionAndFrameRateInfo));
	struSubResolutionAndFrameRateInfo.nResolutionWidth =  800;
	struSubResolutionAndFrameRateInfo.nResolutionHeight = 448;
	struSubResolutionAndFrameRateInfo.nMaxFrameRate = max_framerate;//max_framerate;
	InsertSubStreamResolutionAndMaxFrameRateByMainResolution(pcfg, 0, 1920, 1080, 1, struSubResolutionAndFrameRateInfo);

	//1280*720
	//在主码流1280*720分辨率下插入第一个子码流下所有的分辨率选项和最大帧率
	InsertSubStreamByMainResolution(pcfg, 0, 1280, 720);//插入第一子码流节点
	memset(&struSubResolutionAndFrameRateInfo, 0, sizeof(struSubResolutionAndFrameRateInfo));
	struSubResolutionAndFrameRateInfo.nResolutionWidth =  800; 
	struSubResolutionAndFrameRateInfo.nResolutionHeight = 448;
	struSubResolutionAndFrameRateInfo.nMaxFrameRate = max_framerate;//max_framerate;
	InsertSubStreamResolutionAndMaxFrameRateByMainResolution(pcfg, 0, 1280, 720, 1, struSubResolutionAndFrameRateInfo);

#elif defined(__PRODUCT_P4U__) 
	//插入子码流支持的分辨率和最大码率的情况 
	//在主码流1920*1080分辨率下插入第一个子码流下所有的分辨率选项和最大帧率
	InsertSubStreamByMainResolution(pcfg, 0, 2880, 1620);//插入第一子码流节点
	memset(&struSubResolutionAndFrameRateInfo, 0, sizeof(struSubResolutionAndFrameRateInfo));
	struSubResolutionAndFrameRateInfo.nResolutionWidth =  800; 
	struSubResolutionAndFrameRateInfo.nResolutionHeight = 448;
	struSubResolutionAndFrameRateInfo.nMaxFrameRate = max_framerate;//max_framerate;
	InsertSubStreamResolutionAndMaxFrameRateByMainResolution(pcfg, 0, 2880, 1620, 1, struSubResolutionAndFrameRateInfo);

	InsertSubStreamByMainResolution(pcfg, 0, 1920, 1080);//插入第一子码流节点
	memset(&struSubResolutionAndFrameRateInfo, 0, sizeof(struSubResolutionAndFrameRateInfo));
	struSubResolutionAndFrameRateInfo.nResolutionWidth =  800;
	struSubResolutionAndFrameRateInfo.nResolutionHeight = 448;
	struSubResolutionAndFrameRateInfo.nMaxFrameRate = max_framerate;//max_framerate;
	InsertSubStreamResolutionAndMaxFrameRateByMainResolution(pcfg, 0, 1920, 1080, 1, struSubResolutionAndFrameRateInfo);

	//1280*720
	//在主码流1280*720分辨率下插入第一个子码流下所有的分辨率选项和最大帧率
	InsertSubStreamByMainResolution(pcfg, 0, 1280, 720);//插入第一子码流节点
	memset(&struSubResolutionAndFrameRateInfo, 0, sizeof(struSubResolutionAndFrameRateInfo));
	struSubResolutionAndFrameRateInfo.nResolutionWidth =  800; 
	struSubResolutionAndFrameRateInfo.nResolutionHeight = 448;
	struSubResolutionAndFrameRateInfo.nMaxFrameRate = max_framerate;//max_framerate;
	InsertSubStreamResolutionAndMaxFrameRateByMainResolution(pcfg, 0, 1280, 720, 1, struSubResolutionAndFrameRateInfo);

#else
	//插入子码流支持的分辨率和最大码率的情况 
#if defined (__PRODUCT_P3MU__)
    InsertSubStreamByMainResolution(pcfg, 0, 2304, 1296);//插入第一子码流节点
    memset(&struSubResolutionAndFrameRateInfo, 0, sizeof(struSubResolutionAndFrameRateInfo));
    struSubResolutionAndFrameRateInfo.nResolutionWidth =  800; 
    struSubResolutionAndFrameRateInfo.nResolutionHeight = 448;
    struSubResolutionAndFrameRateInfo.nMaxFrameRate = max_framerate;//max_framerate;
    InsertSubStreamResolutionAndMaxFrameRateByMainResolution(pcfg, 0, 2304, 1296, 1, struSubResolutionAndFrameRateInfo);
#endif

	InsertSubStreamByMainResolution(pcfg, 0, 1920, 1080);//插入第一子码流节点
	memset(&struSubResolutionAndFrameRateInfo, 0, sizeof(struSubResolutionAndFrameRateInfo));
	struSubResolutionAndFrameRateInfo.nResolutionWidth =  800;
	struSubResolutionAndFrameRateInfo.nResolutionHeight = 448;
	struSubResolutionAndFrameRateInfo.nMaxFrameRate = max_framerate;//max_framerate;
	InsertSubStreamResolutionAndMaxFrameRateByMainResolution(pcfg, 0, 1920, 1080, 1, struSubResolutionAndFrameRateInfo);

	memset(&struSubResolutionAndFrameRateInfo, 0, sizeof(struSubResolutionAndFrameRateInfo));
	struSubResolutionAndFrameRateInfo.nResolutionWidth =  640; 
	struSubResolutionAndFrameRateInfo.nResolutionHeight = 360;
	struSubResolutionAndFrameRateInfo.nMaxFrameRate = max_framerate;//max_framerate;
	InsertSubStreamResolutionAndMaxFrameRateByMainResolution(pcfg, 0, 1920, 1080, 1, struSubResolutionAndFrameRateInfo);

	//1280*720
	//在主码流1280*720分辨率下插入第一个子码流下所有的分辨率选项和最大帧率
	InsertSubStreamByMainResolution(pcfg, 0, 1280, 720);//插入第一子码流节点
	memset(&struSubResolutionAndFrameRateInfo, 0, sizeof(struSubResolutionAndFrameRateInfo));
	struSubResolutionAndFrameRateInfo.nResolutionWidth =  800; 
	struSubResolutionAndFrameRateInfo.nResolutionHeight = 448;
	struSubResolutionAndFrameRateInfo.nMaxFrameRate = max_framerate;//max_framerate;
	InsertSubStreamResolutionAndMaxFrameRateByMainResolution(pcfg, 0, 1280, 720, 1, struSubResolutionAndFrameRateInfo);

	memset(&struSubResolutionAndFrameRateInfo, 0, sizeof(struSubResolutionAndFrameRateInfo));
	struSubResolutionAndFrameRateInfo.nResolutionWidth =  640; 
	struSubResolutionAndFrameRateInfo.nResolutionHeight = 360;
	struSubResolutionAndFrameRateInfo.nMaxFrameRate = max_framerate;//max_framerate;
	InsertSubStreamResolutionAndMaxFrameRateByMainResolution(pcfg, 0, 1280, 720, 1, struSubResolutionAndFrameRateInfo);
#endif

	printf("GetCompressCfgNewV3_end\n");
	return nRet;
}

//获取编码能力及对应的帧率范围，需外部将缓冲区清零
static int GetCompressCfgNew(HY_DVR_CODEC_CAP * pcfg)
{
	int nRet = IDVR_OK, nBoardID;
	int max_framerate = 25;

	ZWSys_GetBoardInfo(&nBoardID);

	pcfg->codec_cap[0].bValidAudioCap = 0x1;		 //onvif音频支持能力
	pcfg->codec_cap[0].nSupportAudioCodecMask = 0x1;  //onvif音频编码类型


    max_framerate = 10;

	pcfg->bValid = 1;
	pcfg->codec_cap[0].bValid = 1;
	pcfg->codec_cap[0].resolution_cnt = 2;

	pcfg->codec_cap[0].nSupportCorridorMode = 0;	//支持走廊模式 
	pcfg->codec_cap[0].nSupportCodecMask = 5;		//支持H265	H265+
	pcfg->codec_cap[0].nSupportShelterCheck = 1;	//支持视频遮挡

	pcfg->codec_cap[0].bValidAudioCap = 0x1;		 //onvif音频支持能力
	pcfg->codec_cap[0].nSupportAudioCodecMask = 0x1;  //onvif音频编码类型

	pcfg->codec_cap[0].framerate_cap[0].bValid = 1;
	pcfg->codec_cap[0].framerate_cap[0].cur_cap.bValid = 1;
	pcfg->codec_cap[0].framerate_cap[0].cur_cap.resolution = CR_HD1080;
	pcfg->codec_cap[0].framerate_cap[0].cur_cap.max_framerate = max_framerate;
	pcfg->codec_cap[0].framerate_cap[0].nValidFramerateCnt[0] = 1;
	pcfg->codec_cap[0].framerate_cap[0].cap[0][0].bValid = 1;
	pcfg->codec_cap[0].framerate_cap[0].cap[0][0].resolution = CR_800x448;
	pcfg->codec_cap[0].framerate_cap[0].cap[0][0].max_framerate = max_framerate;
	
	pcfg->codec_cap[0].framerate_cap[1].bValid = 1;
	pcfg->codec_cap[0].framerate_cap[1].cur_cap.bValid = 1;
	pcfg->codec_cap[0].framerate_cap[1].cur_cap.resolution = CR_HD720;
	pcfg->codec_cap[0].framerate_cap[1].cur_cap.max_framerate = max_framerate;
	pcfg->codec_cap[0].framerate_cap[1].nValidFramerateCnt[0] = 1;
	pcfg->codec_cap[0].framerate_cap[1].cap[0][0].bValid = 1;
	pcfg->codec_cap[0].framerate_cap[1].cap[0][0].resolution = CR_800x448;
	pcfg->codec_cap[0].framerate_cap[1].cap[0][0].max_framerate = max_framerate;


	printf("GetCompressCfgNew_end\n");

	return nRet;
}

//设置编码参数
static int SetCompressCfg(DWORD dwUserID, LPHY_DVR_COMPRESSION_CFG pcfg)
{
	int nRet, nSDKRet, nCh, nCount;

	///////////////////////////////////////////////////////////////////////////////
	//本结构体是否有效，=0表示无效
	if (0 == pcfg->bValid)
	{
		MPU_DBG("0 == pcfg->bValid\n");
		return IDVR_CIU_STRUCT_UNVALID;
	}


	///////////////////////////////////////////////////////////////////////////////
	//公共参数
	//SetDevCustom_Record(&pcfg->struWorkMode);


	//默认返回设置成功
	nRet = IDVR_OK;


	//获取录像通道总数
	nCount = MAX_ENC_CHANNEL_NUM;
	if (nCount > DVR_MAX_VIDEOIN_NUM)
	{
		MPU_ERR("remote config record %d, real %d\n", DVR_MAX_VIDEOIN_NUM, nCount);
		nCount = DVR_MAX_VIDEOIN_NUM;
	}
	for (nCh = 0; nCh < nCount; ++nCh)
	{
		//各通道压缩参数
		nSDKRet = SetCompressChannelCfg(nCh, &pcfg->struCompressChannel[nCh]);
		CIU_ERR(nSDKRet, nRet);

		if (IDVR_OK == nSDKRet)
		{
			//记录日志，设置编码参数
			CIU_OPL_AddLog(dwUserID, EDVR_EVENT_SETTING, EDVR_EVENT_SETTING_DETAIL_CODEC_SETTING, nCh, 0);
		}
	}
	return nRet;
}

/* 根据分辨率获取尺寸 */
static int ResolutionToSize(eResolutionIndex eResolution, int *width, int *heigh)
{
    if (NULL==width || NULL==heigh)
    {
        printf("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    switch(eResolution)
    {    
        case eResolutionIndex_320X176 : *width = 320;  *heigh = 176;  break;
        case eResolutionIndex_QVGA    : *width = 320;  *heigh = 240;  break;
        case eResolutionIndex_CIF     : *width = 352;  *heigh = 288;  break;
        case eResolutionIndex_480X256 : *width = 480;  *heigh = 256;  break;
        case eResolutionIndex_640x360 : *width = 640;  *heigh = 360;  break;
        case eResolutionIndex_VGA     : *width = 640;  *heigh = 480;  break;
        case eResolutionIndex_D1      : *width = 704;  *heigh = 576;  break;
        case eResolutionIndex_SD      : *width = 704;  *heigh = 576;  break;        
        case eResolutionIndex_720P    : *width = 1280; *heigh = 720;  break;
        case eResolutionIndex_QuadVGA : *width = 1280; *heigh = 960;  break;
        case eResolutionIndex_SXGA    : *width = 1280; *heigh = 1024; break;
        case eResolutionIndex_UXGA    : *width = 1600; *heigh = 1200; break;
        case eResolutionIndex_1080P   : *width = 1920; *heigh = 1080; break;     
        case eResolutionIndex_QXGA    : *width = 2304; *heigh = 1296; break;
        case eResolutionIndex_UWXGA   : *width = 2592; *heigh = 1520; break;
        case eResolutionIndex_USXGA   : *width = 2592; *heigh = 1944; break;
		case eResolutionIndex_8M      : *width = 3840; *heigh = 2160; break;
        default: printf("Not support resolution %d!\n", eResolution); return -1;
    }

    return 0;
}

/* 根据尺寸获取分辨率 */
static int SizeToResolution(int width, int heigh)
{
	if (320==width  && 176==heigh)  {return eResolutionIndex_320X176;}
    else if (320==width  && 240==heigh)  {return eResolutionIndex_QVGA;}
    else if (352==width  && 288==heigh)  {return eResolutionIndex_CIF;}
    else if (480==width  && 256==heigh)  {return eResolutionIndex_480X256;}
    else if (640==width  && 360==heigh)  {return eResolutionIndex_640x360;}
    else if (640==width  && 480==heigh)  {return eResolutionIndex_VGA;}
    else if (704==width  && 576==heigh)  {return eResolutionIndex_D1;}    
    else if (1280==width && 720==heigh)  {return eResolutionIndex_720P;}
    else if (1280==width && 960==heigh)  {return eResolutionIndex_QuadVGA;}
    else if (1280==width && 1024==heigh) {return eResolutionIndex_SXGA;}
    else if (1600==width && 1200==heigh) {return eResolutionIndex_SXGA;}
    else if (1920==width && 1080==heigh) {return eResolutionIndex_1080P;}
    else if (2304==width && 1296==heigh) {return eResolutionIndex_QXGA;}
    else if (2048==width && 1536==heigh) {return eResolutionIndex_QXGA;}
    else if (2592==width && 1520==heigh) {return eResolutionIndex_UWXGA;}
    else if (2592==width && 1944==heigh) {return eResolutionIndex_USXGA;}
    else {printf("width=%d, heigh=%d, not support!\n", width, heigh); return -1;}

    return 0;
}

/*-----------------------------------------------------------------------*/
//获取各通道子码流参数(第二版)
static int GetCompressInfo_Aux_V2(int nCh, LPHY_DVR_COMPRESSION_INFO_V2 pcfg)
{
	int nRet = IDVR_OK;
	int nRes = 0;
    int nAdvanceEncType = 0;
    
	//本结构体是否有效，=0表示无效
	pcfg->bValid = 1;

    nRet = MPU_AVL_GetAuxAdvEncParam(nCh, &nAdvanceEncType);
    if (nRet)
    {
        MPU_ERR("Fail to call MPU_AVL_GetAuxAdvEncParam, ret = %d\n", nRet);
        return nRet;
    }

    pcfg->nVideoCodecID     = nAdvanceEncType;
    pcfg->nVideoFrameRate   = MPU_AVL_AuxCoder_GetFrameRate(nCh);
    pcfg->nVideoBitrate     = MPU_AVL_AuxCoder_GetBitRate(nCh);
    pcfg->nVideoFrameRate   = MPU_AVL_AuxCoder_GetFrameRate(nCh);
    pcfg->nVideoBitrate     = MPU_AVL_AuxCoder_GetBitRate(nCh);
    pcfg->nQuotiety         = MPU_AVL_AuxCoder_GetQuality(nCh);
    pcfg->nStreamType       = MPU_AVL_AuxCoder_GetStreamType(nCh);
    pcfg->nBitrateType      = MPU_AVL_AuxCoder_GetBitrateType(nCh);

    nRes = MPU_AVL_AuxCoder_GetPictureFormat(nCh, &pcfg->nResolutionWidth, &pcfg->nResolutionHeight);
    if (pcfg->nResolutionWidth == 0 || pcfg->nResolutionHeight == 0)
    {
        nRet = ResolutionToSize((eResolutionIndex)nRes, &pcfg->nResolutionWidth, &pcfg->nResolutionHeight);
        if (nRet)
        {
            MPU_ERR("Fail to call ResolutionToSize, ret = %d\n", nRet);
            return nRet;
        }
    }
    
	return nRet;
}

//设置各通道子码流参数(第二版)
static int SetCompressInfo_Aux_V2(int nCh, LPHY_DVR_COMPRESSION_INFO_V2 pcfg)
{
	int nRet = IDVR_OK;

    if (pcfg->bValid == 0)
    {
        printf("[%s:%d] pcfg->bValid == 0\n", __FUNCTION__, __LINE__);
        return 0;
    }

    /* 设置子码流下的高级编码参数 */
    //nRet = MPU_AVL_SetHDAuxAdvEncParam(nCh, pcfg->nVideoCodecID);
    if (nRet)
    {
        MPU_ERR("Fail to call MPU_AVL_SetHDAuxAdvEncParam, ret = %d\n", nRet);
        return nRet;
    }

    nRet = MPU_AVL_AuxCoder_SetFrameRate(nCh, pcfg->nVideoFrameRate);
    if (nRet)
    {
        MPU_ERR("Fail to call MPU_AVL_AuxCoder_SetFrameRate, ret = %d\n", nRet);
        return nRet;
    }

    nRet = MPU_AVL_AuxCoder_SetBitRate(nCh, pcfg->nVideoBitrate);
    if (nRet)
    {
        MPU_ERR("Fail to call MPU_AVL_AuxCoder_SetBitRate, ret = %d\n", nRet);
        return nRet;
    }

    nRet = MPU_AVL_AuxCoder_SetQuality(nCh, pcfg->nQuotiety);
    if (nRet)
    {
        MPU_ERR("Fail to call MPU_AVL_AuxCoder_SetQuality, ret = %d\n", nRet);
        return nRet;
    }

    nRet = MPU_AVL_AuxCoder_SetStreamType(nCh, pcfg->nStreamType);
    if (nRet)
    {
        MPU_ERR("Fail to call MPU_AVL_AuxCoder_SetStreamType, ret = %d\n", nRet);
        return nRet;
    }
    
    nRet = MPU_AVL_AuxCoder_SetBitrateType(nCh, pcfg->nBitrateType);
    if (nRet)
    {
        MPU_ERR("Fail to call MPU_AVL_AuxCoder_SetBitrateType, ret = %d\n", nRet);
        return nRet;
    }

    nRet = MPU_AVL_AuxCoder_SetPictureFormat(nCh, 0, pcfg->nResolutionWidth, pcfg->nResolutionHeight);
    if (nRet)
    {
        MPU_ERR("Fail to call MPU_AVL_AuxCoder_SetPictureFormat, ret = %d\n", nRet);
        return nRet;
    }

	return nRet;
}

/*-----------------------------------------------------------------------*/
// 1是主码流
static int GetEncParam(int nCh, int nStream, LPHY_DVR_COMPRESSION_INFO_V2 pcfg)
{
	int nRet = IDVR_OK;
    int width = 0;
    int heigh = 0;
    int nAdvanceEncType = 0;
    HY_DVR_COMPRESSION_INFO compression_cfg;
    
    memset(&compression_cfg, 0, sizeof(compression_cfg));

    if (NULL == pcfg)
    {
        MPU_ERR("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    if (0!=nCh || nStream<1 || nStream>4)
    {
        MPU_ERR("nChn=%d(0), nStream=%d[1-4]\n", nCh, nStream);
        return -1;
    }

	printf("\n***GetEncParam func:nCh=%d,nStream=%d\n\n",nCh,nStream);
    switch(nStream)
    {
        case 1: // 主码流
        {
            int nEventAdvanceEncType = 0;
            VIDEO_SHELTER stuVideoShelter;
            memset(&stuVideoShelter, 0, sizeof(stuVideoShelter));
            
            /* 获取主码流高级编码参数 */
            nRet = MPU_AVL_GetAdvanceEncParam(nCh, &nAdvanceEncType, &nEventAdvanceEncType, &stuVideoShelter);
            if (nRet)
            {
                MPU_ERR("Fail to call MPU_AVL_GetAdvanceEncParam, ret = %d\n", nRet);
                return nRet;
            }

            /* 获取主码流编码参数 */
            nRet = GetCompressInfo_Main(nCh, 0, &compression_cfg, &width, &heigh);
            if (nRet)
            {
                MPU_ERR("Fail to call GetCompressInfo_Main, ret = %d\n", nRet);
                return nRet;
            }

            pcfg->bValid = 1;
            pcfg->nVideoCodecID     = nAdvanceEncType;
            pcfg->nVideoFrameRate   = compression_cfg.nVideoFrameRate;
            pcfg->nVideoBitrate     = compression_cfg.nVideoBitrate;
            pcfg->nQuotiety         = compression_cfg.nQuotiety;
            pcfg->nResolutionWidth  = width;
            pcfg->nResolutionHeight = heigh;
            pcfg->nStreamType       = compression_cfg.nStreamType;
            pcfg->nBitrateType      = compression_cfg.nBitrateType;
        }
        break;
        
        case 2: // 子码流
        {    
            HY_DVR_COMPRESSION_INFO_V2 compression_cfg_v2;
            memset(&compression_cfg_v2, 0, sizeof(compression_cfg_v2));
            
            nRet = GetCompressInfo_Aux_V2(nCh, &compression_cfg_v2);
            if (nRet)
            {
                MPU_ERR("Fail to call GetCompressInfo_Aux_V2, ret = %d\n", nRet);
                return nRet;
            }
            
            memcpy(pcfg, &compression_cfg_v2, sizeof(compression_cfg_v2));
        }
        break;
        
        case 3: // 高清子码流
        {
            nRet = MPU_AVL_GetHDAuxAdvEncParam(nCh, &nAdvanceEncType);
            if (nRet)
            {
                MPU_ERR("Fail to call MPU_AVL_GetHDAuxAdvEncParam, ret = %d\n", nRet);
                return nRet;
            }
            
            nRet = GetCompressInfo_HDAux(nCh, &compression_cfg, &width, &heigh);
            if (nRet)
            {
                MPU_ERR("Fail to call GetCompressInfo_HDAux, ret = %d\n", nRet);
                return nRet;
            }

            pcfg->bValid 			= compression_cfg.bValid;
			printf("============= pcfg->bValid = %d===========\n", pcfg->bValid);
            pcfg->nVideoCodecID     = nAdvanceEncType;
            pcfg->nVideoFrameRate   = compression_cfg.nVideoFrameRate;
            pcfg->nVideoBitrate     = compression_cfg.nVideoBitrate;
            pcfg->nQuotiety         = compression_cfg.nQuotiety;
            pcfg->nResolutionWidth  = width;
            pcfg->nResolutionHeight = heigh;
            pcfg->nStreamType       = compression_cfg.nStreamType;
            pcfg->nBitrateType      = compression_cfg.nBitrateType;
        }
        break;

        case 4: // 第四码流
        {
            tagEncCfg EncCfg;
            memset(&EncCfg, 0, sizeof(EncCfg));
            
            nRet = MPU_AVL_GetFourthStreamAdvEncParam(nCh, &nAdvanceEncType);
            if (nRet)
            {
                MPU_ERR("Fail to call MPU_AVL_GetFourthStreamAdvEncParam, ret = %d\n", nRet);
                return nRet;
            }
            
            nRet = MPU_AVL_FourthStream_GetEncCfg(nCh, &EncCfg);
            if (nRet)
            {
                MPU_ERR("Fail to call MPU_AVL_FourthStream_GetEncCfg, ret = %d\n", nRet);
                return nRet;
            }
            
            nRet = ResolutionToSize((eResolutionIndex)EncCfg.byResolution, &width, &heigh);
            if (nRet)
            {
                MPU_ERR("Fail to call ResolutionToSize, ret = %d\n", nRet);
                return nRet;
            }
            
            pcfg->bValid = 1;
            pcfg->nVideoCodecID     = nAdvanceEncType;
            pcfg->nVideoFrameRate   = EncCfg.dwVideoFrameRate;
            pcfg->nVideoBitrate     = EncCfg.dwVideoBitrateH;
            pcfg->nQuotiety         = EncCfg.dwQuotiety;
            pcfg->nResolutionWidth  = width;
            pcfg->nResolutionHeight = heigh;
            pcfg->nStreamType       = EncCfg.byStreamType;
            pcfg->nBitrateType      = EncCfg.byBitrateType;
        }
        break;
        
        default:
        {
            pcfg->bValid = 0; 
            MPU_ERR("nStream = %d, out of [1, 4]\n", nStream);
            return -1;
        }
    }

	MPU_DBG("\
nCh=%d\n\
nStream=%d\n\
pcfg->bValid=%d\n\
pcfg->nVideoCodecID=%d\n\
pcfg->nVideoFrameRate=%d\n\
pcfg->nVideoBitrate=%d\n\
pcfg->nQuotiety=%d\n\
pcfg->nResolutionWidth=%d\n\
pcfg->nResolutionHeight=%d\n\
pcfg->nStreamType=%d\n\
pcfg->nBitrateType=%d\n",
nCh, 
nStream,
pcfg->bValid,
pcfg->nVideoCodecID,
pcfg->nVideoFrameRate,
pcfg->nVideoBitrate,
pcfg->nQuotiety,
pcfg->nResolutionWidth,
pcfg->nResolutionHeight,
pcfg->nStreamType,
pcfg->nBitrateType
);
	return nRet;
}

static int SetEncParam(int nCh, int nStream, LPHY_DVR_COMPRESSION_INFO_V2 pcfg, int *nNeedReboot)
{
	int nRet = IDVR_OK;
    int nAdvanceEncType = 0;
    CConfig *pCfgFile = NULL;
    HY_DVR_COMPRESSION_INFO compression_cfg;
    
    memset(&compression_cfg, 0, sizeof(compression_cfg));

    if (NULL == pcfg)
    {
        MPU_ERR("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    if (0!=nCh || nStream<1 || nStream>4)
    {
        MPU_ERR("nChn=%d(0), nStream=%d[1-4]\n", nCh, nStream);
        return -1;
    }
	
    if(pcfg->nVideoCodecID == 0) //不支持264编码
	{
		MPU_ERR("no suport Code:%d\n", pcfg->nVideoCodecID);
		return -1;
	}
	
    if (pcfg->bValid == 0)
    {
        printf("[%s:%d] pcfg->bValid == 0\n", __FUNCTION__, __LINE__);
        return 0;
    }

    *nNeedReboot = 0;

MPU_DBG("\
nCh=%d\n\
nStream=%d\n\
pcfg->bValid=%d\n\
pcfg->nVideoCodecID=%d\n\
pcfg->nVideoFrameRate=%d\n\
pcfg->nVideoBitrate=%d\n\
pcfg->nQuotiety=%d\n\
pcfg->nResolutionWidth=%d\n\
pcfg->nResolutionHeight=%d\n\
pcfg->nStreamType=%d\n\
pcfg->nBitrateType=%d\n",
nCh, 
nStream,
pcfg->bValid,
pcfg->nVideoCodecID,
pcfg->nVideoFrameRate,
pcfg->nVideoBitrate,
pcfg->nQuotiety,
pcfg->nResolutionWidth,
pcfg->nResolutionHeight,
pcfg->nStreamType,
pcfg->nBitrateType
);

    switch(nStream)
    {
        case 1: // 主码流
        {
            int nEventAdvanceEncType = 0;
            VIDEO_SHELTER stuVideoShelter;
            memset(&stuVideoShelter, 0, sizeof(stuVideoShelter));

            compression_cfg.bValid          = pcfg->bValid;
            compression_cfg.nVideoFrameRate = pcfg->nVideoFrameRate;
            compression_cfg.nVideoBitrate   = pcfg->nVideoBitrate;
            compression_cfg.nQuotiety       = pcfg->nQuotiety;
            compression_cfg.nStreamType     = pcfg->nStreamType;
            compression_cfg.nBitrateType    = pcfg->nBitrateType;
            compression_cfg.nResolution     = SizeToResolution(pcfg->nResolutionWidth, pcfg->nResolutionHeight);

            /* 获取主码流高级编码参数 */
            nRet = MPU_AVL_GetAdvanceEncParam(nCh, &nAdvanceEncType, &nEventAdvanceEncType, &stuVideoShelter);
            if (nRet)
            {
                MPU_ERR("Fail to call MPU_AVL_GetAdvanceEncParam, ret = %d\n", nRet);
                return nRet;
            }
            /* 设置主码流高级编码参数-编码类型 */
            nRet = MPU_AVL_SetAdvanceEncParam(nCh, pcfg->nVideoCodecID, nEventAdvanceEncType, &stuVideoShelter);
            if (nRet)
            {
                MPU_ERR("Fail to call MPU_AVL_SetAdvanceEncParam, ret = %d\n", nRet);
                return nRet;
            }

            int nBoardID;
            HY_DVR_COMPRESSION_INFO stCompressInfo;
            int nMainVideoWidth = 0, nMainVideoHeight = 0;
            int nSmartWorkMode = 0;

            ZWSys_GetBoardInfo(&nBoardID);

            if (nBoardID == 0x2099 || nBoardID == 0x208F || nBoardID == 0x2074 || nBoardID == 0x2080)
            {
                memset(&stCompressInfo, 0, sizeof(stCompressInfo));
                GetCompressInfo_Main(nCh, 0, &stCompressInfo, &nMainVideoWidth, &nMainVideoHeight);
                pCfgFile = CConfig::GetInstance();
                if (NULL != pCfgFile)
                {
                    nSmartWorkMode = pCfgFile->GetValue("PUB", "SmartAIWorkMode", 0);
                }

                if (nSmartWorkMode == 0)  //当前是标准模式
                {
                    if (nMainVideoWidth != pcfg->nResolutionWidth && nMainVideoHeight != pcfg->nResolutionHeight)
                    {
                        fprintf(stderr, "[%s][%d] **zxr need reboot**\r\n",__FUNCTION__,__LINE__);
                        *nNeedReboot = 1;
                    }
                }
                else  //人形/人脸模式
                {
                    //从1080P和720P切换到3M时，需要重启
                    if (nMainVideoWidth != 2304 && nMainVideoHeight != 1296 
                    && pcfg->nResolutionWidth == 2304 && pcfg->nResolutionHeight == 1296)
                    {
                        *nNeedReboot = 1;
                        fprintf(stderr, "[%s][%d] **zxr need reboot**\r\n",__FUNCTION__,__LINE__);
                    }  
                }
            }

            /* 设置主码流编码参数 */
            nRet = SetCompressInfo_Main(nCh, 0, &compression_cfg, pcfg->nResolutionWidth, pcfg->nResolutionHeight);
            if (nRet)
            {
                *nNeedReboot = 0;
                MPU_ERR("Fail to call SetCompressInfo_Main, ret = %d\n", nRet);
                return nRet;
            }
        }
        break;
        
        case 2: // 子码流
        {
            nRet = SetCompressInfo_Aux_V2(nCh, pcfg);
            if (nRet)
            {
                MPU_ERR("Fail to call SetCompressInfo_Aux_V2, ret = %d\n", nRet);
                return nRet;
            }
        }
        break;
        
        case 3: // 高清子码流
        {
            compression_cfg.bValid          = pcfg->bValid;
            compression_cfg.nVideoFrameRate = pcfg->nVideoFrameRate;
            compression_cfg.nVideoBitrate   = pcfg->nVideoBitrate;
            compression_cfg.nQuotiety       = pcfg->nQuotiety;
            compression_cfg.nStreamType     = pcfg->nStreamType;  
            compression_cfg.nBitrateType    = pcfg->nBitrateType;
            compression_cfg.nResolution     = SizeToResolution(pcfg->nResolutionWidth, pcfg->nResolutionHeight);

			//ODM设置MJPEG会崩溃，所以当子码流或者第二子码流得编码格式为MJPEG则返回
			if (2 == pcfg->nVideoCodecID)
			{
				return nRet;
			}
			
            nRet = MPU_AVL_SetHDAuxAdvEncParam(nCh, pcfg->nVideoCodecID);//编码类型
            if (nRet)
            {
                MPU_ERR("Fail to call MPU_AVL_SetHDAuxAdvEncParam, ret = %d\n", nRet);
                return nRet;
            }
            nRet = SetCompressInfo_HDAux(nCh, &compression_cfg, pcfg->nResolutionWidth, pcfg->nResolutionHeight);
            if (nRet)
            {
                MPU_ERR("Fail to call SetCompressInfo_HDAux, ret = %d\n", nRet);
                return nRet;
            }
        }
        break;

        case 4: // 第四码流
        {
            tagEncCfg EncCfg;
            memset(&EncCfg, 0, sizeof(EncCfg));

            EncCfg.dwVideoFrameRate = pcfg->nVideoFrameRate;
            EncCfg.dwVideoBitrateH  = pcfg->nVideoBitrate;
            EncCfg.dwQuotiety       = pcfg->nQuotiety;
            EncCfg.byStreamType     = pcfg->nStreamType;
            EncCfg.byBitrateType    = pcfg->nBitrateType;

printf("[...4...Set]:EncCfg.dwVideoFrameRate=%d\n\
EncCfg.dwVideoBitrateH=%d\n\
EncCfg.dwQuotiety=%d\n\
EncCfg.byStreamType=%d\n\
EncCfg.byBitrateType=%d\n\
pcfg->nResolutionWidth=%d\n\
pcfg->nResolutionHeight=%d\n\
pcfg->nVideoCodecID=%d\n",
EncCfg.dwVideoFrameRate,
EncCfg.dwVideoBitrateH, 
EncCfg.dwQuotiety,      
EncCfg.byStreamType,   
EncCfg.byBitrateType,
pcfg->nResolutionWidth,
pcfg->nResolutionHeight,
pcfg->nVideoCodecID);

            EncCfg.byResolution = (eResolutionIndex)SizeToResolution(pcfg->nResolutionWidth, pcfg->nResolutionHeight);
            EncCfg.dwResWidth = pcfg->nResolutionWidth;
            EncCfg.dwResHeight = pcfg->nResolutionHeight;
            nRet = MPU_AVL_SetFourthStreamAdvEncParam(nCh, pcfg->nVideoCodecID);
            if (nRet)
            {
                MPU_ERR("Fail to call SetCompressInfo_HDAux, ret = %d\n", nRet);
                return nRet;
            }
            nRet = MPU_AVL_FourthStream_SetEncCfg(nCh, &EncCfg);
            if (nRet)
            {
                MPU_ERR("Fail to call SetCompressInfo_HDAux, ret = %d\n", nRet);
                return nRet;
            }
        }
        break;
    }

	return nRet;
}

static int GetAllCompressCfg(LPHY_DVR_COMPRESSION_CFG_V2 pcfg)
{
	int nRet = IDVR_OK;
	int i    = 0;
	
    HY_DVR_COMPRESSION_INFO_V2 compression_info_v2;

    memset(&compression_info_v2, 0, sizeof(compression_info_v2));

    pcfg->bValid = 1;
	
    for (i=0; i<MAX_STREAM_COUNT; i++)
    {
        if (pcfg->struCompressChannelV2[i].bValid)
        {
            nRet = GetEncParam(
                        pcfg->struCompressChannelV2[i].nChannelID, 
                        pcfg->struCompressChannelV2[i].nStreamID, 
                       &pcfg->struCompressChannelV2[i].struStreamTypeV2);             
        }
    }
    
	return nRet;
}

static int SetAllCompressCfg(LPHY_DVR_COMPRESSION_CFG_V2 pcfg, int *nNeedReboot)
{
	int nRet = IDVR_OK;
	int i    = 0;
	
	//本结构体是否有效，=0表示无效
	if (0 == pcfg->bValid)
	{
		MPU_DBG("0 == pcfg->bValid\n");
		return IDVR_CIU_STRUCT_UNVALID;
	}

    if (pcfg->bValid)
    {
        for (i=0; i<MAX_STREAM_COUNT; i++)
        {
            if (pcfg->struCompressChannelV2[i].bValid)
            {
                nRet = SetEncParam(
                            pcfg->struCompressChannelV2[i].nChannelID, 
                            pcfg->struCompressChannelV2[i].nStreamID, 
                           &pcfg->struCompressChannelV2[i].struStreamTypeV2,
                           nNeedReboot);
				printf("======SetEncParam nRet =%d ==========================\n",nRet);
				if(nRet)
				{
					MPU_ERR("SetEncParam Error nChannelID = %d nStreamID = %d struStreamTypeV2 = %d\n",pcfg->struCompressChannelV2[i].nChannelID,\
						pcfg->struCompressChannelV2[i].nStreamID,pcfg->struCompressChannelV2[i].struStreamTypeV2);
					return nRet;
				}				
            }
        }
    }
	return nRet;
}
static int GetSmallCompressCfg(LPHY_DVR_COMPRESSION_CHANNEL_V2 pcfg)
{
	int nRet = IDVR_OK; 

    if (pcfg->bValid)
    {
        nRet = GetEncParam(pcfg->nChannelID, pcfg->nStreamID, &pcfg->struStreamTypeV2);
        if (nRet)
        {
            MPU_ERR("Fail to call GetEncParam, ret = %d\n", nRet);
            return nRet;
        }
    }  
	
	return nRet;
}

static int GetFrameRateRange(HY_DVR_FRAME_RATE_RANGE * pcfg)
{
	int nRet = IDVR_OK; 
    int width = 0;
    int heigh = 0;
	CConfig *pCfgFile = NULL;
	int nValue = 0;
	int max_framerate = 25;
    HY_DVR_COMPRESSION_INFO compression_cfg;
	BoardSensorType_t SensorType;
    
    memset(&compression_cfg, 0, sizeof(compression_cfg));
	
	ZWSys_GetSensorType(&SensorType);
	//获取配置文件管理器实例指针
	pCfgFile = CConfig::GetInstance();
	
	if (NULL != pCfgFile)
	{
		char szSection[30]; 				
		snprintf(szSection, sizeof(szSection), "ISP_S%d", SensorType);
		nValue = pCfgFile->GetValue(szSection, "InputFrequency", 0);
		if (1 == nValue)
			max_framerate = 30;  //30 FPS in 60HZ, 暂不支持抗闪
		else
			max_framerate = 25;
	}

    if (pcfg->nValid)
    {
        /* 获取主码流编码参数 */
        nRet = GetCompressInfo_Main(pcfg->nChannelID, 0, &compression_cfg, &width, &heigh);
        if (nRet)
        {
            MPU_ERR("Fail to call GetCompressInfo_Main, ret = %d\n", nRet);
            return nRet;
        }

        pcfg->nMinFrameRate = 1;

		if ((3840 == width && 2160 == heigh) || (2880 == width && 1620 == heigh))
        {
            pcfg->nMaxFrameRate = 15;
        }
        if ((2560 == width && 1920 == heigh) || (2880 == width && 1624 == heigh))
        {
            pcfg->nMaxFrameRate = 20;
        }
		else if (2560 == width && 1440 == heigh)
        {
            pcfg->nMaxFrameRate = 25;
        }
        else
        {
            pcfg->nMaxFrameRate = max_framerate;
        }
    }

	return nRet;
}

static int SetSmallCompressCfg(LPHY_DVR_COMPRESSION_CHANNEL_V2 pcfg, int *nNeedReboot)
{
	int nRet = IDVR_OK;

    if (pcfg->bValid)
    {
        nRet = SetEncParam(pcfg->nChannelID, pcfg->nStreamID, &pcfg->struStreamTypeV2, nNeedReboot);
        if (nRet)
        {
            MPU_ERR("Fail to call SetEncParam, ret = %d\n", nRet);
            return nRet;
        }
    }
	return nRet;
}


//设置录像计划时间表，需要在外部对pcfg清零
static int GetSchedule_Record(int nCh, LPHY_DVR_SCHEDTIME pcfg)
{
	int nRet = IDVR_OK, nDay, i, num;
#if defined (__SUPPORT_SD_CARD__)
	tagRecordSchedule sched;

	//! 本结构体是否有效，=0表示无效
	pcfg->bValid = 1;


	//获取录像计划时间表
	MPU_AVL_Record_GetSchedule(nCh, &sched);

	num = DVR_MAX_TIMESEGMENT / HY_DVR_RECORDSEG_DAY;

	//时间表需要扩展
	for (nDay = 0; nDay < DVR_MAX_DAYS; ++nDay)
	{
		for (i = 0; i < DVR_MAX_TIMESEGMENT; ++i)
		{
			pcfg->bySetType[nDay][i] = sched.bytRecordTime[nDay][i / num];
		}
	}

	//! 状态,0为禁用(全天布防),1为启用,2为不支持
	pcfg->byState = sched.bytState;

	pcfg->byReserve[0] = sched.bytReserve[0];
	pcfg->byReserve[1] = sched.bytReserve[1];
	pcfg->byReserve[2] = sched.bytReserve[2];
#endif
	return nRet;
}

//设置录像计划时间表
static int SetSchedule_Record(int nCh, LPHY_DVR_SCHEDTIME pcfg)
{
	int nSDKRet = 0, nDay, i, num;
#if defined (__SUPPORT_SD_CARD__)
	tagRecordSchedule sched;

	///////////////////////////////////////////////////////////////////////////////
	//本结构体是否有效，=0表示无效
	if (0 == pcfg->bValid)
	{
		MPU_DBG("0 == pcfg->bValid, nCh = %d\n", nCh);
		return IDVR_CIU_STRUCT_UNVALID;
	}

	num = DVR_MAX_TIMESEGMENT / HY_DVR_RECORDSEG_DAY;

	//时间表需要扩展
	for (nDay = 0; nDay < DVR_MAX_DAYS; ++nDay)
	{
		for (i = 0; i < HY_DVR_ALARMOUTSEG_DAY; ++i)
		{
			sched.bytRecordTime[nDay][i] = pcfg->bySetType[nDay][i * num];
		}
	}

	//! 状态,0为禁用(全天布防),1为启用,2为不支持
	sched.bytState = pcfg->byState;

	sched.bytReserve[0] = pcfg->byReserve[0];
	sched.bytReserve[1] = pcfg->byReserve[1];
	sched.bytReserve[2] = pcfg->byReserve[2];

	//设置报警输入布防时间计划
	nSDKRet = MPU_AVL_Record_SetSchedule(nCh, &sched);
#endif
	return (0 == nSDKRet) ? IDVR_OK : IDVR_CIU_CALLSDK_FAIL;
}

static int GetRecordMode(LPHY_DVR_RECORD_CTRLSTATE pcfg)
{
	int nRet = IDVR_OK, nCh, nCount;
#if defined (__SUPPORT_SD_CARD__)
	pcfg->bValid = 1;

	nCount = MAX_ENC_CHANNEL_NUM;

	for (nCh = 0; nCh < nCount; ++nCh)
	{
		pcfg->uRecordMode[nCh] = MPU_AVL_Record_GetRecMode(nCh);
	}
#endif
	return nRet;
}

static int SetRecordMode(DWORD dwUserID, LPHY_DVR_RECORD_CTRLSTATE pcfg)
{
	int nRet = IDVR_OK, nCh, nCount;
#if defined (__SUPPORT_SD_CARD__)
	if (0 != pcfg->bValid)
	{
		nCount = MAX_ENC_CHANNEL_NUM;

		for (nCh = 0; nCh < nCount; ++nCh)
		{
			unsigned int nMode = pcfg->uRecordMode[nCh];

			if (MPU_AVL_Record_GetRecMode(nCh) != (enumRecordMode) nMode)
			{
				MPU_AVL_Record_StartMode(nCh, (enumRecordMode) nMode);
				//记录日志，远程录像控制
				CIU_OPL_AddLog(dwUserID, EDVR_EVENT_RECORD, (EDVR_EVENT_RECORD_DETAIL) nMode, nCh, 0);
			}
		}
	}
#endif
	return nRet;
}

static int GetRecordStream(LPHY_DVR_RECORD_STREAM pcfg)
{
	int nRet = IDVR_OK, nCh, nCount;
#if defined (__SUPPORT_SD_CARD__)
	pcfg->bValid = 1;

	nCount = MAX_ENC_CHANNEL_NUM;

	for (nCh = 0; nCh < nCount; ++nCh)
	{
		pcfg->uStreamIndex[nCh] = MPU_AVL_Record_GetRecStreamIndex(nCh);

//      MPU_INFO("ch %d, uStreamIndex = %u\n", nCh, pcfg->uStreamIndex[nCh]);
	}
#endif
	return nRet;
}

static int SetRecordStream(LPHY_DVR_RECORD_STREAM pcfg)
{
	int nRet = IDVR_OK, nCh, nCount;
#if defined (__SUPPORT_SD_CARD__)
	if (0 != pcfg->bValid)
	{
		nCount = MAX_ENC_CHANNEL_NUM;

		for (nCh = 0; nCh < nCount; ++nCh)
		{
			int nIndex = pcfg->uStreamIndex[nCh];

//          MPU_INFO("ch %d, uStreamIndex = %u\n", nCh, pcfg->uStreamIndex[nCh]);

			if (MPU_AVL_Record_GetRecStreamIndex(nCh) != nIndex)
			{
				MPU_AVL_Record_SetRecStreamIndex(nCh, nIndex);
#if 0
				if (MPU_CIU_PreView_GetPreviewStream() != eMediaCodeAssist && nIndex != 1)
				{
					//预览和录像均不为子码流
					MPU_AVL_AuxCoder_Enable(nCh, 0);
				}
#endif
#ifdef USE_HDAUX
				if (MPU_CIU_PreView_GetPreviewStream() != eMediaCodeHDAssist && nIndex != 2)
				{
					//预览和录像均不为高清子码流
					MPU_AVL_HDAuxCoder_Enable(nCh, 0);
				}
#endif
			}
		}
	}
#endif
	return nRet;
}

//获取录像配置，需外部将缓冲区清零
static int GetRecordCfg(LPHY_DVR_RECORD_SCHED pcfg)
{
	int nRet = IDVR_OK, nCh, nCount;
#if defined (__SUPPORT_SD_CARD__)
	///////////////////////////////////////////////////////////////////////////////
	//本结构体是否有效，=0表示无效
	pcfg->bValid = 1;


	//获取录像通道总数
	nCount = MAX_ENC_CHANNEL_NUM;
	if (nCount > DVR_MAX_VIDEOIN_NUM)
	{
		MPU_ERR("remote config record %d, real %d\n", DVR_MAX_VIDEOIN_NUM, nCount);
		nCount = DVR_MAX_VIDEOIN_NUM;
	}
	//获取各通道录像计划配置
	for (nCh = 0; nCh < nCount; ++nCh)
	{
		GetSchedule_Record(nCh, &pcfg->struRecordTime[nCh]);
	}
#if 0
	GetRecordMode(&pcfg->struRecordCtlState);
#endif

	GetRecordStream(&pcfg->struRecordStream);
#endif
	return nRet;
}

//设置录像配置
static int SetRecordCfg(DWORD dwUserID, LPHY_DVR_RECORD_SCHED pcfg)
{
	int nRet = IDVR_OK, nCh, nCount, nSDKRet;
#if defined (__SUPPORT_SD_CARD__)
	///////////////////////////////////////////////////////////////////////////////
	//本结构体是否有效，=0表示无效
	if (0 == pcfg->bValid)
	{
		MPU_DBG("0 == pcfg->bValid\n");
		return IDVR_CIU_STRUCT_UNVALID;
	}


	//获取录像通道总数
	nCount = MAX_ENC_CHANNEL_NUM;
	if (nCount > DVR_MAX_VIDEOIN_NUM)
	{
		MPU_ERR("remote config record %d, real %d\n", DVR_MAX_VIDEOIN_NUM, nCount);
		nCount = DVR_MAX_VIDEOIN_NUM;
	}
	//默认返回设置成功
	nRet = IDVR_OK;

	//设置各通道录像计划配置
	for (nCh = 0; nCh < nCount; ++nCh)
	{
		nSDKRet = SetSchedule_Record(nCh, &pcfg->struRecordTime[nCh]);
		CIU_ERR(nSDKRet, nRet);

		if (IDVR_OK == nSDKRet)
		{
			//记录日志，设置录像计划
			CIU_OPL_AddLog(dwUserID, EDVR_EVENT_SETTING, EDVR_EVENT_SETTING_DETAIL_TIMER_SETTING, nCh, 0);
		}
	}
#if 0
	SetRecordMode(dwUserID, &pcfg->struRecordCtlState);
#endif

	SetRecordStream(&pcfg->struRecordStream);
#endif
	return nRet;
}

//获取新的录像计划时间表，需要在外部对pcfg清零
static int GetSchedule_RecordV2(LPHY_DVR_RECORD_SCHEDULE_V2 pcfg)
{
	int nRet = IDVR_OK;
	tagRecordScheduleV2 sched;
#if defined (__SUPPORT_SD_CARD__)
	//! 本结构体是否有效，=0表示无效
	pcfg->nValid = 1;

	//获取录像计划时间表
	nRet = MPU_AVL_Record_GetNewSchedule(&sched);
    if(nRet != 0)
    {
        MPU_DBG("Get New Record Schedule Fail\n");
        return -1;
    }
    //录像状态
    pcfg->nEnable = sched.nEnable;
	printf("[GetSchedule_RecordV2] Get nEnable =%d\n", pcfg->nEnable);
	//录像类型,0-定时录像, 1-事件录像
	pcfg->nRecordType = sched.nRecordType;
	MPU_DBG("[GetSchedule_RecordV2] Get nRecordType =%d\n", pcfg->nRecordType);
    //录像开始时间
	pcfg->struStartTime[0].nHour = sched.startTime.nHour;
	MPU_DBG("[GetSchedule_RecordV2] Get startTime Hour =%d\n", pcfg->struStartTime[0].nHour);
	pcfg->struStartTime[0].nMinute = sched.startTime.nMinute;
	MPU_DBG("[GetSchedule_RecordV2] Get startTime Minute =%d\n", pcfg->struStartTime[0].nMinute);
	pcfg->struStartTime[0].nSecond = sched.startTime.nSecond;
	MPU_DBG("[GetSchedule_RecordV2] Get startTime Second =%d\n", pcfg->struStartTime[0].nSecond);
	//录像结束时间
	pcfg->struStopTime[0].nHour = sched.stopTime.nHour;
	MPU_DBG("[GetSchedule_RecordV2] Get stopTime Hour =%d\n", pcfg->struStopTime[0].nHour);
	pcfg->struStopTime[0].nMinute = sched.stopTime.nMinute;
	MPU_DBG("[GetSchedule_RecordV2] Get stopTime Minute =%d\n", pcfg->struStopTime[0].nMinute);
	pcfg->struStopTime[0].nSecond = sched.stopTime.nSecond;
	MPU_DBG("[GetSchedule_RecordV2] Get stopTime Minute =%d\n", pcfg->struStopTime[0].nSecond);

	pcfg->iStreamType = MPU_AVL_Record_GetRecStreamIndex(0);
#endif
	return nRet;
}

//设置新的录像计划时间表
static int SetSchedule_Record_V2(int nCh, LPHY_DVR_RECORD_SCHEDULE_V2 pcfg)
{
	int nSDKRet = IDVR_OK;
	tagRecordScheduleV2 sched;
#if defined (__SUPPORT_SD_CARD__)
    //录像状态,0-不开启，1-开启录像
    if (1 != pcfg->nEnable)
    {
        sched.nEnable = 1;
        printf("[%s, %5d] Set Record ScheduleV2 force set 1!\n", __FUNCTION__, __LINE__);
    }
    else
    {
        //录像状态,0-不开启，1-开启录像
        sched.nEnable = pcfg->nEnable;
    }
	MPU_DBG("[Schedule_Record_V2] Set nEnable =%d\n", sched.nEnable);
	//录像类型, 0-定时录像, 1-事件录像
	sched.nRecordType = pcfg->nRecordType;
	MPU_DBG("[Schedule_Record_V2] Set nRecordType=%d\n", sched.nRecordType);

	//录像开始时间
	sched.startTime.nHour = pcfg->struStartTime[0].nHour;
	sched.startTime.nMinute = pcfg->struStartTime[0].nMinute;
	sched.startTime.nSecond = pcfg->struStartTime[0].nSecond;
	//录像结束时间
	sched.stopTime.nHour = pcfg->struStopTime[0].nHour;
	sched.stopTime.nMinute = pcfg->struStopTime[0].nMinute;
	sched.stopTime.nSecond = pcfg->struStopTime[0].nSecond;

	MPU_DBG("[Schedule_Record_V2] Set startTime.nHour=%d\n", sched.startTime.nHour);
	MPU_DBG("[Schedule_Record_V2] Set startTime.nMinute=%d\n", sched.startTime.nMinute);
	MPU_DBG("[Schedule_Record_V2] Set startTime.nSecond=%d\n", sched.startTime.nSecond);

	MPU_DBG("[Schedule_Record_V2] Set stopTime.nHour=%d\n", sched.stopTime.nHour);
	MPU_DBG("[Schedule_Record_V2] Set stopTime.nMinute=%d\n", sched.stopTime.nMinute);
	MPU_DBG("[Schedule_Record_V2] Set stopTime.nSecond=%d\n", sched.stopTime.nSecond);

	//设置报警输入布防时间计划
	nSDKRet = MPU_AVL_Record_SetNewSchedule(nCh, &sched);

	//设置录像码流
	if (MPU_AVL_Record_GetRecStreamIndex(nCh) != pcfg->iStreamType)
	{
		MPU_AVL_Record_SetRecStreamIndex(nCh, pcfg->iStreamType);
	}
#endif
	return nSDKRet;
}

//设置新的录像计划配置
static int SetRecordCfgV2(LPHY_DVR_RECORD_SCHEDULE_V2 pcfg)
{
	int nRet = IDVR_OK;
#if defined (__SUPPORT_SD_CARD__)
	//本结构体是否有效，=0表示无效
	if (0 == pcfg->nValid)
	{
		MPU_DBG("0 == pcfg->bValid\n");
		return IDVR_CIU_STRUCT_UNVALID;
	}

	//设置各通道录像计划配置
    nRet = SetSchedule_Record_V2(pcfg->nChannelID, pcfg);
    MPU_CHK(0 == nRet);
#endif
	return nRet;
}

//获取新的录像配置，需外部将缓冲区清零
static int GetRecordCfgV2(LPHY_DVR_RECORD_SCHEDULE_V2 pcfg)
{
	int nRet = IDVR_OK;
#if defined (__SUPPORT_SD_CARD__)
	//获取各通道录像计划配置
	nRet = GetSchedule_RecordV2(pcfg);
	if (nRet != 0)
	{
        MPU_DBG("Get New Record Schedule Fail\n");
        pcfg->nValid = 0;
        return -1;
	}

	pcfg->nValid=1;
#endif
	return nRet;
}

/* 获取录像文件列表属性(获取当前总共文件记录有多少条和每页有多少条) */
static int GetRecordFileListAttr(HY_DVR_RECORD_FILE_LIST_ATTRIBUTE *pcfg)
{
    MPU_DBG("Into %s-%s-%d ...\n", __FILE__, __FUNCTION__, __LINE__);

	int nRet = 0;
#if defined (__SUPPORT_SD_CARD__)
    pcfg->bValid = 1;

    pcfg->nSumRecordFileCount = MPU_AVL_GetCurrentAviTotalNum(0);
    pcfg->nRecordFileCountPerPage = MPU_AVL_GetCurrentAviNumPerPage(0);
    MPU_DBG("pcfg->nSumRecordFileCount = %d, pcfg->nRecordFileCountPerPage = %d\n",
             pcfg->nSumRecordFileCount, pcfg->nRecordFileCountPerPage);
#endif
    return nRet;
}

/* 设置录像文件列表属性(设置每一页显示多少条记录) */
static int SetRecordFileListAttr(HY_DVR_RECORD_FILE_LIST_ATTRIBUTE *pcfg)
{
    MPU_DBG("Into %s-%s-%d ... pcfg->nRecordFileCountPerPage = %d\n", __FILE__, __FUNCTION__, __LINE__,
            pcfg->nRecordFileCountPerPage);

	int nRet = 0;
#if defined (__SUPPORT_SD_CARD__)
	//本结构体是否有效，=0表示无效
	if (0 == pcfg->bValid)
	{
		MPU_DBG("0 == pcfg->bValid\n");
		return IDVR_CIU_STRUCT_UNVALID;
	}

    nRet = MPU_AVL_SetCurrentAviNumPerPage(0, pcfg->nRecordFileCountPerPage);
#endif
	return nRet;
}

/* 设置要寻找录像的时间段 */
static int SetRecordSearchTimeSeg(HY_DVR_FILE_LIST_ATTRIBUTE_SEARCH_BY_TIME *pcfg)
{
    MPU_DBG("Into %s-%s-%d ...\n", __FILE__, __FUNCTION__, __LINE__);

	int nRet = 0;
#if defined (__SUPPORT_SD_CARD__)
	//本结构体是否有效，=0表示无效
	if (0 == pcfg->bValid)
	{
		MPU_DBG("0 == pcfg->bValid\n");
		return IDVR_CIU_STRUCT_UNVALID;
	}

    DateTime start_time;
    DateTime stop_time;

    memcpy(&start_time, &pcfg->struStartTime, sizeof(DateTime));
    memcpy(&stop_time, &pcfg->struStopTime, sizeof(DateTime));

#if 0
printf("Record start: %d-%d-%d %d-%d-%d\n",
        pcfg->struStartTime.nYear,
        pcfg->struStartTime.nMonth,
        pcfg->struStartTime.nDay,
        pcfg->struStartTime.nHour,
        pcfg->struStartTime.nMinute,
        pcfg->struStartTime.nSecond);

printf("Record stop: %d-%d-%d %d-%d-%d\n",
        pcfg->struStopTime.nYear,
        pcfg->struStopTime.nMonth,
        pcfg->struStopTime.nDay,
        pcfg->struStopTime.nHour,
        pcfg->struStopTime.nMinute,
        pcfg->struStopTime.nSecond);

#endif

    nRet = MPU_AVL_SetCurrentAviSearchTimeSeg(0, start_time, stop_time);
#endif
	return nRet;
}

/* 设置要寻找抓图的时间段 */
static int SetSnapshotSearchTimeSeg(HY_DVR_FILE_LIST_ATTRIBUTE_SEARCH_BY_TIME *pcfg)
{
    MPU_DBG("Into %s-%s-%d ...\n", __FILE__, __FUNCTION__, __LINE__);

	int nRet = 0;

	//本结构体是否有效，=0表示无效
	if (0 == pcfg->bValid)
	{
		MPU_DBG("0 == pcfg->bValid\n");
		return IDVR_CIU_STRUCT_UNVALID;
	}

    DateTime start_time;
    DateTime stop_time;

    memcpy(&start_time, &pcfg->struStartTime, sizeof(DateTime));
    memcpy(&stop_time, &pcfg->struStopTime, sizeof(DateTime));

    MPU_DBG("Picture start: %d-%d-%d %d-%d-%d\n",
    pcfg->struStartTime.nYear,
    pcfg->struStartTime.nMonth,
    pcfg->struStartTime.nDay,
    pcfg->struStartTime.nHour,
    pcfg->struStartTime.nMinute,
    pcfg->struStartTime.nSecond);

    MPU_DBG("Picture stop: %d-%d-%d %d-%d-%d\n",
    pcfg->struStopTime.nYear,
    pcfg->struStopTime.nMonth,
    pcfg->struStopTime.nDay,
    pcfg->struStopTime.nHour,
    pcfg->struStopTime.nMinute,
    pcfg->struStopTime.nSecond);

    nRet = MPU_AVL_SetCurrentPicSearchTimeSeg(0, start_time, stop_time);

	return nRet;
}

/* 获取当前选中的是哪一页 */
static int GetRecordFileCurrentPage(HY_DVR_RECORD_FILE_CURRENT_PAGE *pcfg)
{
    MPU_DBG("Into %s-%s-%d ...\n", __FILE__, __FUNCTION__, __LINE__);

    int nRet = 0;

    pcfg->bValid = 1;

    pcfg->nIndexPage = MPU_AVL_GetSwitchPageIsChoose(0);

    MPU_DBG("pcfg->nIndexPage = %d\n", pcfg->nIndexPage);

    return nRet;
}

/* 设置当前需要选中哪一页 */
static int SetRecordFileCurrentPage(HY_DVR_RECORD_FILE_CURRENT_PAGE *pcfg)
{
    MPU_DBG("Into %s-%s-%d ...\n", __FILE__, __FUNCTION__, __LINE__);

	int nRet = 0;

	//本结构体是否有效，=0表示无效
	if (0 == pcfg->bValid)
	{
		MPU_DBG("0 == pcfg->bValid\n");
		return IDVR_CIU_STRUCT_UNVALID;
	}

    MPU_DBG("pcfg->nIndexPage=%d, pcfg->nPathType=%d\n", pcfg->nIndexPage, pcfg->nPathType);

    nRet = MPU_AVL_SetSwitchPageIsChoose(0, pcfg->nIndexPage, pcfg->nPathType);

	return nRet;
}

/* 从给定的页面里面获取当前AVI文件列表
 * 需要先调用MPU_AVL_SetSwitchPageIsChoose设置当前选中哪一页
 * 需要先调用MPU_AVL_SetCurrentAviNumPerPage设置当前用户需要设置每页显示多少个AVI文件
 */
static int GetRecordFileList(HY_DVR_RECORD_FILE_LIST *pcfg)
{
    MPU_DBG("Into %s-%s-%d ...\n", __FILE__, __FUNCTION__, __LINE__);
	int nRet = 0;
	int nAviCount = 0;
#if defined (__SUPPORT_SD_CARD__)
    pcfg->bValid = 1;

    tagRecordAviReadList *pRecordAviReadList = NULL;

    nRet = MPU_AVL_GetCurrentAviListInfo(0, (void**)&pRecordAviReadList, &nAviCount);
    if (nRet)
    {
        MPU_ERR("Fail to called MPU_AVL_GetCurrentAviListInfo\n");
        return nRet;
    }

    for (int i=0; i<nAviCount; i++)
    {
        pcfg->struRecordFile[i].bValid = 1;
        strncpy(pcfg->struRecordFile[i].strPathName, (char *)pRecordAviReadList[i].file_path, sizeof(pcfg->struRecordFile[i].strPathName) - 1);
        strncpy(pcfg->struRecordFile[i].strFileName, (char *)pRecordAviReadList[i].file_name, sizeof(pcfg->struRecordFile[i].strFileName) - 1);
        memcpy(&pcfg->struRecordFile[i].struStartTime, &pRecordAviReadList[i].start_time, sizeof(HY_DVR_TIME));
        memcpy(&pcfg->struRecordFile[i].struStopTime, &pRecordAviReadList[i].stop_time, sizeof(HY_DVR_TIME));
#if 0
        printf("%d: pcfg->struRecordFile[i].strPathName = %s\n", i, pcfg->struRecordFile[i].strPathName);
        printf("%d: pcfg->struRecordFile[i].strFileName = %s\n", i, pcfg->struRecordFile[i].strFileName);
        printf("%d: pcfg->struRecordFile[i].struStartTime.nYear   = %d\n", i, pcfg->struRecordFile[i].struStartTime.nYear);
        printf("%d: pcfg->struRecordFile[i].struStartTime.nDay    = %d\n", i, pcfg->struRecordFile[i].struStartTime.nMonth);
        printf("%d: pcfg->struRecordFile[i].struStartTime.nHour   = %d\n", i, pcfg->struRecordFile[i].struStartTime.nDay);
        printf("%d: pcfg->struRecordFile[i].struStartTime.nHour   = %d\n", i, pcfg->struRecordFile[i].struStartTime.nHour);
        printf("%d: pcfg->struRecordFile[i].struStartTime.nMinute = %d\n", i, pcfg->struRecordFile[i].struStartTime.nMinute);
        printf("%d: pcfg->struRecordFile[i].struStartTime.nSecond = %d\n", i, pcfg->struRecordFile[i].struStartTime.nSecond);
        printf("%d: pcfg->struRecordFile[i].struStopTime.nYear   = %d\n", i, pcfg->struRecordFile[i].struStopTime.nYear);
        printf("%d: pcfg->struRecordFile[i].struStopTime.nDay    = %d\n", i, pcfg->struRecordFile[i].struStopTime.nMonth);
        printf("%d: pcfg->struRecordFile[i].struStopTime.nHour   = %d\n", i, pcfg->struRecordFile[i].struStopTime.nDay);
        printf("%d: pcfg->struRecordFile[i].struStopTime.nHour   = %d\n", i, pcfg->struRecordFile[i].struStopTime.nHour);
        printf("%d: pcfg->struRecordFile[i].struStopTime.nMinute = %d\n", i, pcfg->struRecordFile[i].struStopTime.nMinute);
        printf("%d: pcfg->struRecordFile[i].struStopTime.nSecond = %d\n", i, pcfg->struRecordFile[i].struStopTime.nSecond);
#endif
    }
#endif
    return nRet;
}


/* 从给定的页面里面获取当前AVI文件列表
 * 需要先调用MPU_AVL_SetSwitchPageIsChoose设置当前选中哪一页
 * 需要先调用MPU_AVL_SetCurrentAviNumPerPage设置当前用户需要设置每页显示多少个AVI文件
 */
static int GetRecordFileListEX(HY_DVR_RECORD_FILE_LIST_EX *pcfg)
{
    MPU_DBG("Into %s-%s-%d ...\n", __FILE__, __FUNCTION__, __LINE__);
	int nRet = 0;
	int nAviCount = 0;
#if defined (__SUPPORT_SD_CARD__)
    pcfg->bValid = 1;

    tagRecordAviReadList *pRecordAviReadList = NULL;

    nRet = MPU_AVL_GetCurrentAviListInfo(0, (void**)&pRecordAviReadList, &nAviCount);
    if (nRet)
    {
        MPU_ERR("Fail to called MPU_AVL_GetCurrentAviListInfo\n");
        return nRet;
    }

    for (int i=0; i<nAviCount; i++)
    {
        pcfg->struRecordFile[i].bValid = 1;
        pcfg->struRecordFile[i].nFileSize = pRecordAviReadList[i].file_size;
        strncpy(pcfg->struRecordFile[i].strPathName, (char *)pRecordAviReadList[i].file_path, sizeof(pcfg->struRecordFile[i].strPathName) - 1);
        strncpy(pcfg->struRecordFile[i].strFileName, (char *)pRecordAviReadList[i].file_name, sizeof(pcfg->struRecordFile[i].strFileName) - 1);
        memcpy(&pcfg->struRecordFile[i].struStartTime, &pRecordAviReadList[i].start_time, sizeof(HY_DVR_TIME));
        memcpy(&pcfg->struRecordFile[i].struStopTime, &pRecordAviReadList[i].stop_time, sizeof(HY_DVR_TIME));
#if 0
        printf("%d: pcfg->struRecordFile[i].strPathName = %s\n", i, pcfg->struRecordFile[i].strPathName);
        printf("%d: pcfg->struRecordFile[i].strFileName = %s\n", i, pcfg->struRecordFile[i].strFileName);
        printf("%d: pcfg->struRecordFile[i].nFileSize   = %d\n", i, pcfg->struRecordFile[i].nFileSize);
        printf("%d: pcfg->struRecordFile[i].struStartTime.nYear   = %d\n", i, pcfg->struRecordFile[i].struStartTime.nYear);
        printf("%d: pcfg->struRecordFile[i].struStartTime.nDay    = %d\n", i, pcfg->struRecordFile[i].struStartTime.nMonth);
        printf("%d: pcfg->struRecordFile[i].struStartTime.nHour   = %d\n", i, pcfg->struRecordFile[i].struStartTime.nDay);
        printf("%d: pcfg->struRecordFile[i].struStartTime.nHour   = %d\n", i, pcfg->struRecordFile[i].struStartTime.nHour);
        printf("%d: pcfg->struRecordFile[i].struStartTime.nMinute = %d\n", i, pcfg->struRecordFile[i].struStartTime.nMinute);
        printf("%d: pcfg->struRecordFile[i].struStartTime.nSecond = %d\n", i, pcfg->struRecordFile[i].struStartTime.nSecond);
        printf("%d: pcfg->struRecordFile[i].struStopTime.nYear   = %d\n", i, pcfg->struRecordFile[i].struStopTime.nYear);
        printf("%d: pcfg->struRecordFile[i].struStopTime.nDay    = %d\n", i, pcfg->struRecordFile[i].struStopTime.nMonth);
        printf("%d: pcfg->struRecordFile[i].struStopTime.nHour   = %d\n", i, pcfg->struRecordFile[i].struStopTime.nDay);
        printf("%d: pcfg->struRecordFile[i].struStopTime.nHour   = %d\n", i, pcfg->struRecordFile[i].struStopTime.nHour);
        printf("%d: pcfg->struRecordFile[i].struStopTime.nMinute = %d\n", i, pcfg->struRecordFile[i].struStopTime.nMinute);
        printf("%d: pcfg->struRecordFile[i].struStopTime.nSecond = %d\n", i, pcfg->struRecordFile[i].struStopTime.nSecond);
#endif
    }
#endif
    return nRet;
}

/* 从给定的页面里面获取当前AVI文件列表
 * 需要先调用MPU_AVL_SetSwitchPageIsChoose设置当前选中哪一页
 * 需要先调用MPU_AVL_SetCurrentAviNumPerPage设置当前用户需要设置每页显示多少个AVI文件
 */
static int GetRecordFileListEX_V1(HY_DVR_RECORD_FILE_LIST_V1 *pcfg)
{
    MPU_DBG("Into %s-%s-%d ...\n", __FILE__, __FUNCTION__, __LINE__);
	int nRet = 0;
	int nAviCount = 0;

    pcfg->bValid = 1;

    tagRecordAviReadList *pRecordAviReadList = NULL;

    nRet = MPU_AVL_GetCurrentAviListInfo(0, (void**)&pRecordAviReadList, &nAviCount);
    if (nRet)
    {
        MPU_ERR("Fail to called MPU_AVL_GetCurrentAviListInfo\n");
        return nRet;
    }

    for (int i=0; i<nAviCount; i++)
    {
        pcfg->struRecordFile[i].bValid = 1;
        pcfg->struRecordFile[i].nFileSize = pRecordAviReadList[i].file_size;
        strncpy(pcfg->struRecordFile[i].strPathName, (char *)pRecordAviReadList[i].file_path, sizeof(pcfg->struRecordFile[i].strPathName) - 1);
        strncpy(pcfg->struRecordFile[i].strFileName, (char *)pRecordAviReadList[i].file_name, sizeof(pcfg->struRecordFile[i].strFileName) - 1);
        memcpy(&pcfg->struRecordFile[i].struStartTime, &pRecordAviReadList[i].start_time, sizeof(HY_DVR_TIME));
        memcpy(&pcfg->struRecordFile[i].struStopTime, &pRecordAviReadList[i].stop_time, sizeof(HY_DVR_TIME));
#if 0
        printf("%d: pcfg->struRecordFile[i].strPathName = %s\n", i, pcfg->struRecordFile[i].strPathName);
        printf("%d: pcfg->struRecordFile[i].strFileName = %s\n", i, pcfg->struRecordFile[i].strFileName);
        printf("%d: pcfg->struRecordFile[i].nFileSize   = %d\n", i, pcfg->struRecordFile[i].nFileSize);
        printf("%d: pcfg->struRecordFile[i].struStartTime.nYear   = %d\n", i, pcfg->struRecordFile[i].struStartTime.nYear);
        printf("%d: pcfg->struRecordFile[i].struStartTime.nDay    = %d\n", i, pcfg->struRecordFile[i].struStartTime.nMonth);
        printf("%d: pcfg->struRecordFile[i].struStartTime.nHour   = %d\n", i, pcfg->struRecordFile[i].struStartTime.nDay);
        printf("%d: pcfg->struRecordFile[i].struStartTime.nHour   = %d\n", i, pcfg->struRecordFile[i].struStartTime.nHour);
        printf("%d: pcfg->struRecordFile[i].struStartTime.nMinute = %d\n", i, pcfg->struRecordFile[i].struStartTime.nMinute);
        printf("%d: pcfg->struRecordFile[i].struStartTime.nSecond = %d\n", i, pcfg->struRecordFile[i].struStartTime.nSecond);
        printf("%d: pcfg->struRecordFile[i].struStopTime.nYear   = %d\n", i, pcfg->struRecordFile[i].struStopTime.nYear);
        printf("%d: pcfg->struRecordFile[i].struStopTime.nDay    = %d\n", i, pcfg->struRecordFile[i].struStopTime.nMonth);
        printf("%d: pcfg->struRecordFile[i].struStopTime.nHour   = %d\n", i, pcfg->struRecordFile[i].struStopTime.nDay);
        printf("%d: pcfg->struRecordFile[i].struStopTime.nHour   = %d\n", i, pcfg->struRecordFile[i].struStopTime.nHour);
        printf("%d: pcfg->struRecordFile[i].struStopTime.nMinute = %d\n", i, pcfg->struRecordFile[i].struStopTime.nMinute);
        printf("%d: pcfg->struRecordFile[i].struStopTime.nSecond = %d\n", i, pcfg->struRecordFile[i].struStopTime.nSecond);
#endif
    }

    return nRet;
}

static int GetRecordFileInfoByFileName(HY_DVR_RECORD_FILE_V1 *pcfg)
{
	int nRet = 0;
    tagRecordAviReadList struRecordFileInfo = {0};

    pcfg->bValid = 1;

    nRet = MPU_AVL_GetRecordFileInfoByFileName(0, pcfg->strFileName, (tagRecordAviReadList *)&struRecordFileInfo);
    if (0 == nRet)
    {
        memcpy(&pcfg->struStartTime, (HY_DVR_TIME *)&struRecordFileInfo.start_time, sizeof(HY_DVR_TIME));
        memcpy(&pcfg->struStopTime, (HY_DVR_TIME *)&struRecordFileInfo.stop_time, sizeof(HY_DVR_TIME));
        strncpy(pcfg->strPathName, (char *)struRecordFileInfo.file_path, sizeof(pcfg->strPathName) - 1);
        pcfg->nFileSize = struRecordFileInfo.file_size;
#if 0
        printf("start_time: %04d-%02d-%02d %02d:%02d:%02d\n", 
                pcfg->struStartTime.nYear, pcfg->struStartTime.nMonth, pcfg->struStartTime.nDay,
                pcfg->struStartTime.nHour, pcfg->struStartTime.nMinute, pcfg->struStartTime.nSecond);
        printf("stop_time: %04-%02-%02 %02:%02:%02\n", 
                pcfg->struStopTime.nYear, pcfg->struStopTime.nMonth, pcfg->struStopTime.nDay,
                pcfg->struStopTime.nHour, pcfg->struStopTime.nMinute, pcfg->struStopTime.nSecond);
        printf("file_name: %s\n", pcfg->strFileName);
        printf("file_path: %s\n", pcfg->strPathName);
        printf("file_size: %d bytes\n", pcfg->nFileSize);
#endif
    }

    return nRet;
}

/* 获取抓图文件列表属性(获取当前总共文件记录有多少条和每页有多少条) */
static int GetSnapshotFileListAttr(HY_DVR_SNAPSHOT_FILE_LIST_ATTRIBUTE *pcfg)
{
    MPU_DBG("Into %s-%s-%d ...\n", __FILE__, __FUNCTION__, __LINE__);

	int nRet = 0;

    pcfg->bValid = 1;

    pcfg->nSumSnapshotFileCount = MPU_AVL_GetCurrentPicTotalNum(0);
    pcfg->nSnapshotFileCountPerPage = MPU_AVL_GetCurrentPicNumPerPage(0);

    return nRet;
}

/* 设置抓图文件列表属性(设置每一页显示多少条记录) */
static int SetSnapshotFileListAttr(HY_DVR_SNAPSHOT_FILE_LIST_ATTRIBUTE *pcfg)
{
    MPU_DBG("Into %s-%s-%d ...pcfg->nSnapshotFileCountPerPage = %d\n", __FILE__, __FUNCTION__, __LINE__,
             pcfg->nSnapshotFileCountPerPage);

	int nRet = 0;

	//本结构体是否有效，=0表示无效
	if (0 == pcfg->bValid)
	{
		MPU_DBG("0 == pcfg->bValid\n");
		return IDVR_CIU_STRUCT_UNVALID;
	}

    nRet = MPU_AVL_SetCurrentPicNumPerPage(0, pcfg->nSnapshotFileCountPerPage);

	return nRet;
}

/* 获取当前选中的是哪一页 */
static int GetSnapshotFileCurrentPage(HY_DVR_SNAPSHOT_FILE_CURRENT_PAGE *pcfg)
{
    MPU_DBG("Into %s-%s-%d ...\n", __FILE__, __FUNCTION__, __LINE__);

	int nRet = 0;

    pcfg->bValid = 1;

    pcfg->nIndexPage = MPU_AVL_GetSwitchPicPageIsChoose(0);

    MPU_DBG("pcfg->nIndexPage = %d\n", pcfg->nIndexPage);

    return nRet;
}

/* 设置当前需要选中哪一页 */
static int SetSnapshotFileCurrentPage(HY_DVR_SNAPSHOT_FILE_CURRENT_PAGE *pcfg)
{
    MPU_DBG("Into %s-%s-%d ...\n", __FILE__, __FUNCTION__, __LINE__);

	int nRet = 0;

	//本结构体是否有效，=0表示无效
	if (0 == pcfg->bValid)
	{
		MPU_DBG("0 == pcfg->bValid\n");
		return IDVR_CIU_STRUCT_UNVALID;
	}

    MPU_DBG("pcfg->nIndexPage=%d, pcfg->nPathType=%d\n", pcfg->nIndexPage, pcfg->nPathType);

    nRet = MPU_AVL_SetSwitchPicPageIsChoose(0, pcfg->nIndexPage, pcfg->nPathType);
	nRet = MPU_AVL_SetSwitchPageIsChoose(0, pcfg->nIndexPage, pcfg->nPathType);

	return nRet;
}

/* 从给定的页面里面获取当前图片文件列表
 * 需要先调用MPU_AVL_SetSwitchPicPageIsChoose设置当前选中哪一页
 * 需要先调用MPU_AVL_SetCurrentPicNumPerPage设置当前用户需要设置每页显示多少个图片文件
 */
static int GetSnapshotFileList(HY_DVR_SNAPSHOT_FILE_LIST *pcfg)
{
    MPU_DBG("Into %s-%s-%d ...\n", __FILE__, __FUNCTION__, __LINE__);

	int nRet = 0;
	int nPicCount = 0;

    pcfg->bValid = 1;

    tagPicReadList *pPicReadList = NULL;

    nRet = MPU_AVL_GetCurrentPicListInfo(0, (void**)&pPicReadList, &nPicCount);
    if (nRet)
    {
        MPU_ERR("Fail to called MPU_AVL_GetCurrentPicListInfo\n");
        return nRet;
    }

    for (int i=0; i<nPicCount; i++)
    {
        pcfg->struSnapshotFile[i].bValid = 1;
        strncpy(pcfg->struSnapshotFile[i].strPathName, (char *)pPicReadList[i].file_path, sizeof(pcfg->struSnapshotFile[i].strPathName) - 1);
        strncpy(pcfg->struSnapshotFile[i].strFileName, (char *)pPicReadList[i].file_name, sizeof(pcfg->struSnapshotFile[i].strFileName) - 1);
        memcpy(&pcfg->struSnapshotFile[i].struSnapshotTime, &pPicReadList[i].cap_time, sizeof(HY_DVR_TIME));
#if 0
        printf("%d: pcfg->struSnapshotFile[i].strPathName = %s\n", i, pcfg->struSnapshotFile[i].strPathName);
        printf("%d: pcfg->struSnapshotFile[i].strFileName = %s\n", i, pcfg->struSnapshotFile[i].strFileName);
        printf("%d: pcfg->struSnapshotFile[i].struSnapshotTime.nYear   = %d\n", i, pcfg->struSnapshotFile[i].struSnapshotTime.nYear);
        printf("%d: pcfg->struSnapshotFile[i].struSnapshotTime.nDay    = %d\n", i, pcfg->struSnapshotFile[i].struSnapshotTime.nMonth);
        printf("%d: pcfg->struSnapshotFile[i].struSnapshotTime.nHour   = %d\n", i, pcfg->struSnapshotFile[i].struSnapshotTime.nDay);
        printf("%d: pcfg->struSnapshotFile[i].struSnapshotTime.nHour   = %d\n", i, pcfg->struSnapshotFile[i].struSnapshotTime.nHour);
        printf("%d: pcfg->struSnapshotFile[i].struSnapshotTime.nMinute = %d\n", i, pcfg->struSnapshotFile[i].struSnapshotTime.nMinute);
        printf("%d: pcfg->struSnapshotFile[i].struSnapshotTime.nSecond = %d\n", i, pcfg->struSnapshotFile[i].struSnapshotTime.nSecond);
#endif
    }

    return nRet;
}

/* 从给定的页面里面获取当前图片文件列表
 * 需要先调用MPU_AVL_SetSwitchPicPageIsChoose设置当前选中哪一页
 * 需要先调用MPU_AVL_SetCurrentPicNumPerPage设置当前用户需要设置每页显示多少个图片文件
 */
static int GetSnapshotFileListEX(HY_DVR_SNAPSHOT_FILE_LIST_EX *pcfg)
{
    MPU_DBG("Into %s-%s-%d ...\n", __FILE__, __FUNCTION__, __LINE__);

	int nRet = 0;
	int nPicCount = 0;

    pcfg->bValid = 1;

    tagPicReadList *pPicReadList = NULL;

    nRet = MPU_AVL_GetCurrentPicListInfo(0, (void**)&pPicReadList, &nPicCount);
    if (nRet)
    {
        MPU_ERR("Fail to called MPU_AVL_GetCurrentPicListInfo\n");
        return nRet;
    }

    for (int i=0; i<nPicCount; i++)
    {
        pcfg->struSnapshotFile[i].bValid = 1;
        pcfg->struSnapshotFile[i].nFileSize = pPicReadList[i].file_size;
        strncpy(pcfg->struSnapshotFile[i].strPathName, (char *)pPicReadList[i].file_path, sizeof(pcfg->struSnapshotFile[i].strPathName) - 1);
        strncpy(pcfg->struSnapshotFile[i].strFileName, (char *)pPicReadList[i].file_name, sizeof(pcfg->struSnapshotFile[i].strFileName) - 1);
        memcpy(&pcfg->struSnapshotFile[i].struSnapshotTime, &pPicReadList[i].cap_time, sizeof(HY_DVR_TIME));
#if 0
        printf("%d: pcfg->struSnapshotFile[i].strPathName = %s\n", i, pcfg->struSnapshotFile[i].strPathName);
        printf("%d: pcfg->struSnapshotFile[i].nFileSize   = %d\n", i, pcfg->struSnapshotFile[i].nFileSize);
        printf("%d: pcfg->struSnapshotFile[i].strFileName = %s\n", i, pcfg->struSnapshotFile[i].strFileName);
        printf("%d: pcfg->struSnapshotFile[i].struSnapshotTime.nYear   = %d\n", i, pcfg->struSnapshotFile[i].struSnapshotTime.nYear);
        printf("%d: pcfg->struSnapshotFile[i].struSnapshotTime.nDay    = %d\n", i, pcfg->struSnapshotFile[i].struSnapshotTime.nMonth);
        printf("%d: pcfg->struSnapshotFile[i].struSnapshotTime.nHour   = %d\n", i, pcfg->struSnapshotFile[i].struSnapshotTime.nDay);
        printf("%d: pcfg->struSnapshotFile[i].struSnapshotTime.nHour   = %d\n", i, pcfg->struSnapshotFile[i].struSnapshotTime.nHour);
        printf("%d: pcfg->struSnapshotFile[i].struSnapshotTime.nMinute = %d\n", i, pcfg->struSnapshotFile[i].struSnapshotTime.nMinute);
        printf("%d: pcfg->struSnapshotFile[i].struSnapshotTime.nSecond = %d\n", i, pcfg->struSnapshotFile[i].struSnapshotTime.nSecond);
#endif
    }

    return nRet;
}

static int SetExtentOSDCfg(HY_DVR_OSD_EX *pcfg)
{
	int i;

	///////////////////////////////////////////////////////////////////////////////
	//本结构体是否有效，=0表示无效
	if ((0 == pcfg->bValid) || (0 != pcfg->nChannel))
	{
		MPU_ERR("pcfg->bValid = %d or nChannel != 0\n", pcfg->bValid);
		return IDVR_CIU_STRUCT_UNVALID;
	}

	//数组0设置的是扩展的Text osd，跟Token名osd_token_1绑定
	//数组1设置的是多行OSD
	for(i=0; i < DVR_MAX_EXTENT_OSD_CNT; i++)
	{
		if(pcfg->struOSD[i].bValid)
		{
			MPU_AVL_OSD_SetTextEx(pcfg->struOSD[i].szOSDToken, pcfg->struOSD[i].strText, pcfg->struOSD[i].nCharSet);
		}
	}
	
	return 0;
}

static int GetOSDV2Cfg(HY_DVR_OSD_V2 * pcfg)
{
	int i;
	int ret;
	int showType = 0;
	
	if (0 != pcfg->nChannel)
	{
		MPU_ERR("nChannel != 0 \n");
		return IDVR_CIU_STRUCT_UNVALID;
	}
	pcfg->bValid = 1;

	//数组0设置的是扩展的Text osd，跟Token名osd_token_1绑定
	//数组1设置的是多行OSD
	for(i=0; i < DVR_MAX_OSD_V2_CNT; i++)
	{
		ret = MPU_AVL_OSD_GetOSDV2TextByNum(i, &pcfg->struOSD[i].bEnable, pcfg->struOSD[i].szToken, pcfg->struOSD[i].strText, &pcfg->struOSD[i].nX, &pcfg->struOSD[i].nY);
		if(!ret)
		{
			pcfg->struOSD[i].bValid = 1;
		}
	}
	
	// 获取时间配置
	pcfg->struOSDTime.bValid = 1;
	MPU_AVL_OSD_GetType(pcfg->nChannel, &showType);
	pcfg->struOSDTime.bEnable = ((showType & 0x2) != 0);
	MPU_AVL_OSD_GetPosition(pcfg->nChannel, 0, &pcfg->struOSDTime.nX, &pcfg->struOSDTime.nY);
	
	return 0;
}


static int SetOSDV2Cfg(HY_DVR_OSD_V2 *pcfg)
{
	int i;
	int showType = 0;
	int ret = 0, nSDKRet = 0;

	///////////////////////////////////////////////////////////////////////////////
	//本结构体是否有效，=0表示无效
	if ((0 == pcfg->bValid) || (0 != pcfg->nChannel))
	{
		MPU_ERR("pcfg->bValid = %d or nChannel != 0\n", pcfg->bValid);
		return IDVR_CIU_STRUCT_UNVALID;
	}
	
	if(pcfg->struOSD[1].bValid)
	{
		showType |= ((pcfg->struOSD[1].bEnable != 0) << 2);
	}
	// 保存时间配置及设置显示类型
	if(pcfg->struOSDTime.bValid)
	{
		showType |= ((pcfg->struOSDTime.bEnable != 0) << 1);
	}
	if(pcfg->struOSD[0].bValid)
	{
		showType |= (pcfg->struOSD[0].bEnable != 0);
	}
	
	nSDKRet = MPU_AVL_OSD_SetType(pcfg->nChannel, showType);
	if (0 != nSDKRet)
	{
		MPU_ERR("set channel %d osd tpye %d fail, return %d\n", pcfg->nChannel, showType, nSDKRet);
		ret = IDVR_CIU_CALLSDK_FAIL;
	}
	
 	MPU_DBG("TimeValid: %d, TextOSDValid: %d, ShowType: %d, TimeX: %d, TimeY: %d\n", pcfg->struOSDTime.bEnable,pcfg->struOSD[0].bEnable, showType, pcfg->struOSDTime.nX, pcfg->struOSDTime.nY);
 	MPU_AVL_OSD_SetPosition(0, 0, pcfg->struOSDTime.nX, pcfg->struOSDTime.nY);

	for(i=0; i < DVR_MAX_OSD_V2_CNT; i++)
	{
		if(i > 1)
		{
			//暂时支持两个文本OSD
			break;
		}
		if(pcfg->struOSD[i].bValid)
		{
			MPU_AVL_OSD_SetOSDV2(i, pcfg->struOSD[i].bEnable, pcfg->struOSD[i].szToken, pcfg->struOSD[i].strText, pcfg->struOSD[i].nX, pcfg->struOSD[i].nY);
		}
	}

	CIU_NotifyRemoteControl(0, eRemoteMoveVideoInAlias, pcfg->nChannel);
	
	return ret;
}

static int GetExtentOSDCfg(HY_DVR_OSD_EX * pcfg)
{
	int i;
	int ret;
	
	if (0 != pcfg->nChannel)
	{
		MPU_ERR("nChannel != 0 \n");
		return IDVR_CIU_STRUCT_UNVALID;
	}
	pcfg->bValid = 1;

	//数组0设置的是扩展的Text osd，跟Token名osd_token_1绑定
	//数组1设置的是多行OSD
	for(i=0; i < DVR_MAX_EXTENT_OSD_CNT; i++)
	{
		ret = MPU_AVL_OSD_GetTextExByNum(i, pcfg->struOSD[i].szOSDToken, sizeof(pcfg->struOSD[i].szOSDToken), pcfg->struOSD[i].strText, &pcfg->struOSD[i].nCharSet);
		if(!ret)
		{
			pcfg->struOSD[i].bValid = 1;
		}
	}

	return 0;
}

static int GetExtentOSDNodeCfg(HY_DVR_OSD_NODE_EX *pcfg)
{
	int ret=0;
	
	if (0 != pcfg->nChannel)
	{
		MPU_ERR("nChannel != 0 \n");
		return IDVR_CIU_STRUCT_UNVALID;
	}

	//Token = Camera表示设置的是扩展Text，即struOSD[0]节点。其他Token暂时不支持
	ret = MPU_AVL_OSD_GetTextExByToken(pcfg->struOSD.szOSDToken, pcfg->struOSD.strText, &pcfg->struOSD.nCharSet);

	if(0 == ret)
	{
		pcfg->bValid = 1;
		pcfg->struOSD.bValid = 1;
		return 0;
	}
	else
		{
			return -1;
		}
}

static int SetExtentOSDNodeCfg(HY_DVR_OSD_NODE_EX *pcfg)
{
	///////////////////////////////////////////////////////////////////////////////
	//本结构体是否有效，=0表示无效
	if ((0 == pcfg->bValid) || (0 != pcfg->nChannel) || (0 == pcfg->struOSD.bValid))
	{
		MPU_ERR("pcfg->bValid = %d or nChannel != 0 or struOSD.bValid = %d\n", pcfg->bValid, pcfg->struOSD.bValid);
		return IDVR_CIU_STRUCT_UNVALID;
	}
	printf("[%s:%d], szOSDToken=%s, nCharSet=%d\n", __FUNCTION__, __LINE__, pcfg->struOSD.szOSDToken, pcfg->struOSD.nCharSet);

	//Token = Camera表示设置的是扩展Text，即struOSD[0]节点。其他Token暂时不支持
	MPU_AVL_OSD_SetTextEx(pcfg->struOSD.szOSDToken, pcfg->struOSD.strText, pcfg->struOSD.nCharSet);

	return 0;

}

/* 获取定时抓图配置 */
int GetTimedCaptureCfg(HY_TIMED_CAPTURE_CFG *pcfg)
{
	int nRet = 0;

    pcfg->bValid = 1;
    pcfg->struSDTimedCaptureInfo.bValid = 1;
    MPU_STR_GetSdSnapShotParam(&(pcfg->struSDTimedCaptureInfo.bEnable), &(pcfg->struSDTimedCaptureInfo.uTimeInterval));

    pcfg->struFtpTimedCaptureInfo.bValid = 1;
    MPU_STR_GetFTPSnapShotParam(&(pcfg->struFtpTimedCaptureInfo.bEnable), &(pcfg->struFtpTimedCaptureInfo.uTimeInterval));

    return nRet;
}

/* 设置定时抓图配置 */
int SetTimedCaptureCfg(HY_TIMED_CAPTURE_CFG *pcfg)
{
	int nRet = 0;

	//本结构体是否有效，=0表示无效
	if (0 == pcfg->bValid)
	{
		MPU_DBG("0 == pcfg->bValid\n");
		return IDVR_CIU_STRUCT_UNVALID;
	}

    if (pcfg->struSDTimedCaptureInfo.bValid)
    {
        MPU_STR_SetSdSnapShotParam(pcfg->struSDTimedCaptureInfo.bEnable, pcfg->struSDTimedCaptureInfo.uTimeInterval);
    }

    if (pcfg->struFtpTimedCaptureInfo.bValid)
    {
        MPU_STR_SetFTPSnapShotParam(pcfg->struFtpTimedCaptureInfo.bEnable, pcfg->struFtpTimedCaptureInfo.uTimeInterval);
    }

	return nRet;
}

static int SetAudioAlarmCfg(HY_AUDIO_ALARM_CONFIG *pcfg)
{
	int nRet = 0;

	//本结构体是否有效，=0表示无效
	if (0 == pcfg->bValid)
	{
		MPU_DBG("0 == pcfg->bValid\n");
		return IDVR_CIU_STRUCT_UNVALID;
	}
	//弃用
	//nRet = MPU_AVL_SetAudioAlarmCfg(0, pcfg->nEnable, pcfg->nAudioType, pcfg->nAudioDelayOutput, pcfg->nAudioVolume);
	
	return nRet;
}

static int SetAudioAlarmCfgV1(HY_AUDIO_ALARM_CONFIG_V1 *pcfg)
{
	int nRet = 0;
	return nRet;
}

static int TryListenAlarmAudio(HY_AUDIO_ALARM_CONFIG *pcfg)
{
	int nRet = 0;

	//本结构体是否有效，=0表示无效
	if (0 == pcfg->bValid)
	{
		MPU_DBG("0 == pcfg->bValid\n");
		return IDVR_CIU_STRUCT_UNVALID;
	}
	nRet = MPU_AVL_TryListenAlarmAudio(0, pcfg->nEnable, pcfg->nAudioType, pcfg->nAudioDelayOutput, pcfg->nAudioVolume, 5);

	return nRet;
}

static int TryListenAlarmAudioV1(HY_AUDIO_ALARM_CONFIG_V1 *pcfg)
{
	int nRet = 0;

	//本结构体是否有效，=0表示无效
	if (0 == pcfg->nValid)
	{
		MPU_DBG("0 == pcfg->nValid\n");
		return IDVR_CIU_STRUCT_UNVALID;
	}

	if (0 == pcfg->nTryListenType)
	{
		nRet = MPU_AVL_TryListenAlarmAudio(0, pcfg->nEnable, pcfg->nPeopleAudioType, pcfg->nAudioDelayOutput, pcfg->nAudioVolume, pcfg->nTryListenType);
	}
	else if (1 == pcfg->nTryListenType)
	{
		nRet = MPU_AVL_TryListenAlarmAudio(0, pcfg->nEnable, pcfg->nCarAudioType, pcfg->nAudioDelayOutput, pcfg->nAudioVolume, pcfg->nTryListenType);
	}
	else if (2 == pcfg->nTryListenType)
	{
		nRet = MPU_AVL_TryListenAlarmAudio(0, pcfg->nEnable, pcfg->nFireAudioType, pcfg->nAudioDelayOutput, pcfg->nAudioVolume, pcfg->nTryListenType);
	}
	else if (3 == pcfg->nTryListenType)
	{
		nRet = MPU_AVL_TryListenAlarmAudio(0, pcfg->nEnable, pcfg->iAlarmInExAudioType, pcfg->nAudioDelayOutput, pcfg->nAudioVolume, pcfg->nTryListenType);
	}
	else if (4 == pcfg->nTryListenType)
	{
		nRet = MPU_AVL_TryListenAlarmAudio(0, pcfg->nEnable, pcfg->iVideoShelterAudioType, pcfg->nAudioDelayOutput, pcfg->nAudioVolume, pcfg->nTryListenType);
	}
	else if (5 == pcfg->nTryListenType)
	{
		nRet = MPU_AVL_TryListenAlarmAudio(0, pcfg->nEnable, pcfg->iMotionAudioType, pcfg->nAudioDelayOutput, pcfg->nAudioVolume, pcfg->nTryListenType);
	}
	else if (6 == pcfg->nTryListenType)
	{
		nRet = MPU_AVL_TryListenAlarmAudio(0, pcfg->nEnable, pcfg->iElectronicDenceAudioType, pcfg->nAudioDelayOutput, pcfg->nAudioVolume, pcfg->nTryListenType);
	}
	else if (7 == pcfg->nTryListenType)
	{
		nRet = MPU_AVL_TryListenAlarmAudio(0, pcfg->nEnable, pcfg->iCrossBorderDetectAudioType, pcfg->nAudioDelayOutput, pcfg->nAudioVolume, pcfg->nTryListenType);
	}
	else if (8 == pcfg->nTryListenType)
	{
		nRet = MPU_AVL_TryListenAlarmAudio(0, pcfg->nEnable, pcfg->iOffDutyDetectAudioType, pcfg->nAudioDelayOutput, pcfg->nAudioVolume, pcfg->nTryListenType);
	}
	else if (10 == pcfg->nTryListenType)
	{
		nRet = MPU_AVL_TryListenAlarmAudio(0, pcfg->nEnable, pcfg->iPassengerInAudioType, pcfg->nAudioDelayOutput, pcfg->nAudioVolume, pcfg->nTryListenType);
	}
	else if (11 == pcfg->nTryListenType)
	{
		nRet = MPU_AVL_TryListenAlarmAudio(0, pcfg->nEnable, pcfg->iPassengerOutAudioType, pcfg->nAudioDelayOutput, pcfg->nAudioVolume, pcfg->nTryListenType);
	}
	else if (12 == pcfg->nTryListenType)
	{
		nRet = MPU_AVL_TryListenAlarmAudio(0, pcfg->nEnable, pcfg->iIntelligenceTrackAudioType, pcfg->nAudioDelayOutput, pcfg->nAudioVolume, pcfg->nTryListenType);
	}
	else if (13 == pcfg->nTryListenType)
	{
		nRet = MPU_AVL_TryListenAlarmAudio(0, pcfg->nEnable, pcfg->iFaceCaptureAudioType, pcfg->nAudioDelayOutput, pcfg->nAudioVolume, pcfg->nTryListenType);
	}
	else if (14 == pcfg->nTryListenType)
	{
		nRet = MPU_AVL_TryListenAlarmAudio(0, pcfg->nEnable, pcfg->iMaskDetectionAudioType, pcfg->nAudioDelayOutput, pcfg->nAudioVolume, pcfg->nTryListenType);
	}
	else if (15 == pcfg->nTryListenType)
	{
		nRet = MPU_AVL_TryListenAlarmAudio(0, pcfg->nEnable, pcfg->iCarPlateSnapAudioType, pcfg->nAudioDelayOutput, pcfg->nAudioVolume, pcfg->nTryListenType);
	}
	else if (16 == pcfg->nTryListenType)
	{
		nRet = MPU_AVL_TryListenAlarmAudio(0, pcfg->nEnable, pcfg->iFaceRecoAudioType, pcfg->nAudioDelayOutput, pcfg->nAudioVolume, pcfg->nTryListenType);
	}
    
	return nRet;
}

static int GetAudioAlarmCfg(HY_AUDIO_ALARM_CONFIG *pcfg)
{
	int nRet = 0;

    pcfg->bValid = 1;
	//弃用
	//nRet = MPU_AVL_GetAudioAlarmCfg(0, &pcfg->nEnable, &pcfg->nAudioType, &pcfg->nAudioDelayOutput, &pcfg->nAudioVolume);

	return nRet;
}

static int GetAudioAlarmCfgV1(HY_AUDIO_ALARM_CONFIG_V1 *pcfg)
{
	int nRet = 0;
	return nRet;
}

#ifdef __USE_RTMP__
int CIU_GetConfigure_RTMP_PUSH(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen)
{
	int nRet = IDVR_OK;

	MPU_INFO("ID %d, Major %d, Minor %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType);

	switch (dwMinorType)
	{
			//获取BUFFER SIZE的信息
		case RTMP_PUSH_CONFIG_ALL:
			{
				///////////////////////////////////////////////////////////////////////////////
				//判断buf空间是否足够
				if (*pdwBufLen < HY_DVR_RTMP_PUSH_CONFIG_LEN)
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", HY_DVR_RTMP_PUSH_CONFIG_LEN, *pdwBufLen);
					break;
				}

				HY_DVR_RTMP_PUSH_CONFIG *pcfg = (HY_DVR_RTMP_PUSH_CONFIG *) pBuffer;

				///////////////////////////////////////////////////////////////////////////////
				//初始化输出缓冲区
				memset(pcfg, 0, HY_DVR_RTMP_PUSH_CONFIG_LEN);
				*pdwBufLen = HY_DVR_RTMP_PUSH_CONFIG_LEN;
				
				nRet = GetRtmpPushCfg(pcfg);
			}
			break;


		default:
			{
				MPU_ERR("unsupported dwMinorType = %lu\n", dwMinorType);
				nRet = IDVR_CIU_UNSUPPORTED_MINORTYPE;
			}
			break;
	}


	MPU_INFO("ID %d, Major %d, Minor %d, return %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType, nRet);
	return nRet;    
}

int CIU_SetConfigure_RTMP_PUSH(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, const BYTE * pBuffer, DWORD  dwBufLen)
{
	int nRet = IDVR_OK;

	MPU_INFO("ID %d, Major %d, Minor %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType);

	#if 1
	switch (dwMinorType)
	{
			//获取BUFFER SIZE的信息
		case RTMP_PUSH_CONFIG_ALL:
			{
				///////////////////////////////////////////////////////////////////////////////
				//判断buf空间是否足够
				if (dwBufLen < HY_DVR_RTMP_PUSH_CONFIG_LEN)
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", HY_DVR_RTMP_PUSH_CONFIG_LEN, dwBufLen);
					break;
				}

				HY_DVR_RTMP_PUSH_CONFIG *pcfg = (HY_DVR_RTMP_PUSH_CONFIG *) pBuffer;

				nRet = SetRtmpPushCfg(pcfg);
			}
			break;


		default:
			{
				MPU_ERR("Unsupported dwMinorType = %lu\n", dwMinorType);
				nRet = IDVR_CIU_UNSUPPORTED_MINORTYPE;
			}
			break;
	}
	#endif

	MPU_INFO("ID %d, Major %d, Minor %d, return %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType, nRet);
	return nRet;    
}
#endif

int CIU_GetConfigure_Compress(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen)
{
	int nRet = IDVR_OK;
	
	printf("ID %d, Major %d, Minor %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType);
	switch (dwMinorType)
	{
			/*设置编解码配置 */
		case COMPRESSCFG_ALL:
			{				
				///////////////////////////////////////////////////////////////////////////////
				//判断buf空间是否足够
				if (*pdwBufLen < COMPRESS_CFG_LEN)
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", COMPRESS_CFG_LEN, *pdwBufLen);
					break;
				}

				LPHY_DVR_COMPRESSION_CFG pcfg = (LPHY_DVR_COMPRESSION_CFG) pBuffer;


				///////////////////////////////////////////////////////////////////////////////
				//初始化输出缓冲区
				memset(pcfg, 0, COMPRESS_CFG_LEN);
				*pdwBufLen = COMPRESS_CFG_LEN;

				nRet = GetCompressCfg(pcfg);
			}
			break;

		//编码能力的扩展
		case COMPRESSCFG_ALL_EX:
		{
			//判断buf空间是否足够
			if (*pdwBufLen < sizeof(HY_DVR_COMPRESSION_CFG_EX))
			{
				nRet = IDVR_CIU_BUF_SMALL;
				printf("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_COMPRESSION_CFG_EX), *pdwBufLen);
				break;
			}
		
			LPHY_DVR_COMPRESSION_CFG_EX pcfg = (LPHY_DVR_COMPRESSION_CFG_EX) pBuffer;
			/* 在获取的时候，会传入一些值加以判断，故此时不可以清空结构体 */

			*pdwBufLen = COMPRESS_CFG_LEN_EX;
			memset(pcfg, 0, COMPRESS_CFG_LEN_EX);
			nRet = GetCompressCfgEx(pcfg);			
		}
		break;

		case COMPRESSCFG_CAPABILITY :
			{		
				///////////////////////////////////////////////////////////////////////////////
				//判断buf空间是否足够
				if (*pdwBufLen < COMPRESS_NEW_CFG_LEN)
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", COMPRESS_NEW_CFG_LEN, *pdwBufLen);
					break;
				}

				HY_DVR_CODEC_CAP * pcfg = (HY_DVR_CODEC_CAP *) pBuffer;

				///////////////////////////////////////////////////////////////////////////////
				//初始化输出缓冲区
				memset(pcfg, 0, COMPRESS_NEW_CFG_LEN);
				*pdwBufLen = COMPRESS_NEW_CFG_LEN;

				nRet = GetCompressCfgNew(pcfg);
			}
			break;

		case COMPRESSCFG_CAPABILITY_V3 :
			{				
				///////////////////////////////////////////////////////////////////////////////
				//判断buf空间是否足够
				if (*pdwBufLen < COMPRESS_NEW_CFG_LEN_V3)
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", COMPRESS_NEW_CFG_LEN_V3, *pdwBufLen);
					break;
				}

				HY_DVR_CODEC_CAP_V3 * pcfg = (HY_DVR_CODEC_CAP_V3 *) pBuffer;

				///////////////////////////////////////////////////////////////////////////////
				//初始化输出缓冲区
				memset(pcfg, 0, COMPRESS_NEW_CFG_LEN_V3);
				*pdwBufLen = COMPRESS_NEW_CFG_LEN_V3;

				nRet = GetCompressCfgNewV3(pcfg);
			}
			break;
		
		case COMPRESSCFG_TUTK_LEVEL :
			{
				///////////////////////////////////////////////////////////////////////////////
				//判断buf空间是否足够
				if (*pdwBufLen < sizeof(ZW_TUTK_QUALITY))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(ZW_TUTK_QUALITY), *pdwBufLen);
					return nRet;
				}

				LPZW_TUTK_QUALITY pcfg = (LPZW_TUTK_QUALITY) pBuffer;

				///////////////////////////////////////////////////////////////////////////////
				//初始化输出缓冲区
				memset(pcfg, 0, sizeof(ZW_TUTK_QUALITY));
				*pdwBufLen = sizeof(ZW_TUTK_QUALITY);

				nRet = TutkGetQualityCfg(pcfg);
			}
			break;
			
		case COMPRESSION_QUALITY_AND_STREAM_TYPE_TABLE:
			{
				///////////////////////////////////////////////////////////////////////////////
				//判断buf空间是否足够
				if (*pdwBufLen < COMPRESS_CFG_LEN_EX)
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", COMPRESS_CFG_LEN_EX, *pdwBufLen);
					break;
				}
				LPHY_DVR_COMPRESSION_QUALITY_AND_STREAM_TYPE_TABLE pcfg = \
					(LPHY_DVR_COMPRESSION_QUALITY_AND_STREAM_TYPE_TABLE) pBuffer;
				///////////////////////////////////////////////////////////////////////////////
				//初始化输出缓冲区
				memset(pcfg, 0, sizeof(HY_DVR_COMPRESSION_QUALITY_AND_STREAM_TYPE_TABLE));
				*pdwBufLen = sizeof(HY_DVR_COMPRESSION_QUALITY_AND_STREAM_TYPE_TABLE);				
				nRet = TutkGetQualityMap(pcfg);
			}
			break;	
		
		case COMPRESSCFG_TUTK_STREAMINDEX :
			{
				///////////////////////////////////////////////////////////////////////////////
				//判断buf空间是否足够
				if (*pdwBufLen < sizeof(ZW_TUTK_STREAMINDEX))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(ZW_TUTK_STREAMINDEX), *pdwBufLen);
					return nRet;
				}

				LPZW_TUTK_STREAMINDEX pcfg = (LPZW_TUTK_STREAMINDEX) pBuffer;


				///////////////////////////////////////////////////////////////////////////////
				//初始化输出缓冲区
				memset(pcfg, 0, sizeof(ZW_TUTK_STREAMINDEX));
				*pdwBufLen = sizeof(ZW_TUTK_STREAMINDEX);

				nRet = TutkGetWhichMedia(pcfg);
			}
			break;
		default:
			{
				nRet = IDVR_CIU_UNSUPPORTED_MINORTYPE;
				MPU_ERR("unsupported dwMinorType = %lu\n", dwMinorType);
			}
			break;
	}

	printf("ID %d, Major %d, Minor %d, return %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType, nRet);
	return nRet;
}

int CIU_SetConfigure_Compress(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, const BYTE * pBuffer, DWORD dwBufLen)
{
	int nRet = IDVR_OK;

	MPU_DBG("ID %d, Major %d, Minor %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType);

	switch (dwMinorType)
	{
			/*设置编解码配置 */
		case COMPRESSCFG_ALL:
			{
				///////////////////////////////////////////////////////////////////////////////
				//判断buf空间是否足够
				if (dwBufLen < COMPRESS_CFG_LEN)
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", COMPRESS_CFG_LEN, dwBufLen);
					break;
				}

				LPHY_DVR_COMPRESSION_CFG pcfg = (LPHY_DVR_COMPRESSION_CFG) pBuffer;


				nRet = SetCompressCfg(dwUserID, pcfg);
			}
			break;
			
		case COMPRESSCFG_ALL_EX:
		{
			///////////////////////////////////////////////////////////////////////////////
			//判断buf空间是否足够
			if (dwBufLen < COMPRESS_CFG_LEN_EX)
			{
				nRet = IDVR_CIU_BUF_SMALL;
				MPU_ERR("buffer is too small, need %d, real %lu\n", COMPRESS_CFG_LEN_EX, dwBufLen);
				break;
			}

			LPHY_DVR_COMPRESSION_CFG_EX pcfg = (LPHY_DVR_COMPRESSION_CFG_EX)pBuffer;

			nRet = SetCompressCfgEx(dwUserID, pcfg);
		}
		break;

		case COMPRESSCFG_TUTK_LEVEL:
			{
				///////////////////////////////////////////////////////////////////////////////
				//判断buf空间是否足够
				if (dwBufLen < sizeof(ZW_TUTK_QUALITY))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(ZW_TUTK_QUALITY), dwBufLen);
					break;
				}

				LPZW_TUTK_QUALITY pcfg = (LPZW_TUTK_QUALITY) pBuffer;

				nRet = TutkSetQualityCfg(dwUserID, pcfg);
			}

			break;
		default:
			{
				nRet = IDVR_CIU_UNSUPPORTED_MINORTYPE;
				MPU_ERR("unsupported dwMinorType = %lu\n", dwMinorType);
			}
			break;
	}

	MPU_DBG("ID %d, Major %d, Minor %d, return %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType, nRet);
	return nRet;
}

#ifdef __IS_SUPPORT_CLOUDWORLD__
int CIU_GetConfigure_CurrentFrame_Param(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen)
{
	int nRet = IDVR_OK;

	MPU_DBG("ID %d, Major %d, Minor %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType);

	/*设置编解码配置 */
	
	switch(dwMinorType)
	{
		case CURRENT_FRAME_PARAM_INFO:
		{
			///////////////////////////////////////////////////////////////////////////////
			//判断buf空间是否足够
			if (*pdwBufLen < sizeof(HY_DVR_FRAME_INFO))
			{
				nRet = IDVR_CIU_BUF_SMALL;
				MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_FRAME_INFO), *pdwBufLen);
				break;
			}
	
			LP_HY_DVR_FRAME_INFO pcfg = (LP_HY_DVR_FRAME_INFO) pBuffer;
	
			///////////////////////////////////////////////////////////////////////////////
			//初始化输出缓冲区
			memset(pcfg, 0, sizeof(HY_DVR_FRAME_INFO));
			*pdwBufLen = sizeof(HY_DVR_FRAME_INFO);

			int nKeyFrameInterval = 40;
			int nFrameRateInterval = 10;

			if(MPU_AVL_GetGetRealKeyFrameInterval(0, 0, &nFrameRateInterval,&nKeyFrameInterval))
			{
				return -1;
			}

			pcfg->bValid = 1;
			pcfg->nStreamID = 1;
			pcfg->nChannelID = 0;
			pcfg->currentVideoFrameRate = nFrameRateInterval;
			pcfg->currentKeyFrameInterval = nKeyFrameInterval;
			MPU_DBG("nFrameRateInterval:%d nKeyFrameInterval:%d \n",nFrameRateInterval,nKeyFrameInterval);
		}
		break;

		default:
		{
			nRet = IDVR_CIU_UNSUPPORTED_MINORTYPE;
			MPU_ERR("unsupported dwMinorType = %lu\n", dwMinorType);
		}
		break;

	}

	return nRet;
}
#endif


int CIU_GetConfigure_Compress_V2(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen)
{
	int nRet = IDVR_OK;


	MPU_DBG("CIU_GetConfigure_Compress_V2 ID %d, Major %d, Minor %d\n\n",(int) dwUserID, (int) dwMajorType, (int) dwMinorType);

	switch (dwMinorType)
	{
		/* 获取全部的编码参数配置(大结构体) */
		case COMPRESSCFG_ALL_V2:
		{
			//判断buf空间是否足够
			if (*pdwBufLen < sizeof(HY_DVR_COMPRESSION_CFG_V2))
			{
				nRet = IDVR_CIU_BUF_SMALL;
				printf("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_COMPRESSION_CFG_V2), *pdwBufLen);
				break;
			}

			LPHY_DVR_COMPRESSION_CFG_V2 pcfg = (LPHY_DVR_COMPRESSION_CFG_V2) pBuffer;
            /* 在获取的时候，会传入一些值加以判断，故此时不可以清空结构体 */
			nRet = GetAllCompressCfg(pcfg);
		}
		break;

	    /* 获取单个通道的单个流的参数配置 */
		case COMPRESSCFG_ONE_CHANNEL_V2 :
		{
			//判断buf空间是否足够
			if (*pdwBufLen < sizeof(HY_DVR_COMPRESSION_CHANNEL_V2))
			{
				nRet = IDVR_CIU_BUF_SMALL;
				MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_COMPRESSION_CHANNEL_V2), *pdwBufLen);
				break;
			}

			LPHY_DVR_COMPRESSION_CHANNEL_V2 pcfg = (LPHY_DVR_COMPRESSION_CHANNEL_V2) pBuffer;
            /* 在获取的时候，会传入一些值加以判断，故此时不可以清空结构体 */
			nRet = GetSmallCompressCfg(pcfg);
		}
		break;

		case FRAME_RATE_RANGE:
        {
			//判断buf空间是否足够
			if (*pdwBufLen < sizeof(HY_DVR_FRAME_RATE_RANGE))
			{
				nRet = IDVR_CIU_BUF_SMALL;
				MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_FRAME_RATE_RANGE), *pdwBufLen);
				break;
			}

			HY_DVR_FRAME_RATE_RANGE * pcfg = (HY_DVR_FRAME_RATE_RANGE *) pBuffer;
            /* 在获取的时候，会传入一些值加以判断，故此时不可以清空结构体 */
			nRet = GetFrameRateRange(pcfg);

        }
		break;

		default:
		{
			nRet = IDVR_CIU_UNSUPPORTED_MINORTYPE;
			MPU_ERR("unsupported dwMinorType = %lu\n", dwMinorType);
		}
		break;
	}

	MPU_DBG("CIU_GetConfigure_Compress_V2 ID %d, Major %d, Minor %d return.\n\n",(int) dwUserID, (int) dwMajorType, (int) dwMinorType);
	return nRet;
}
int CIU_SetConfigure_Compress_V2(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, const BYTE * pBuffer, DWORD dwBufLen, int *nNeedReboot)
{
	int nRet = IDVR_OK;
	
	MPU_DBG("ID %d, Major %d, Minor %d\n\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType);

	switch (dwMinorType)
	{
		/* 设置全部的编码参数配置(大结构体) */
		case COMPRESSCFG_ALL_V2:
			{
				///////////////////////////////////////////////////////////////////////////////
				//判断buf空间是否足够
				if (dwBufLen < sizeof(HY_DVR_COMPRESSION_CFG_V2))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_COMPRESSION_CFG_V2), dwBufLen);
					break;
				}

				LPHY_DVR_COMPRESSION_CFG_V2 pcfg = (LPHY_DVR_COMPRESSION_CFG_V2) pBuffer;

				nRet = SetAllCompressCfg(pcfg, nNeedReboot);
			}
			break;
		
	    /* 设置单个通道的单个流的参数配置 */		
		case COMPRESSCFG_ONE_CHANNEL_V2:
			{
				///////////////////////////////////////////////////////////////////////////////
				//判断buf空间是否足够
				if (dwBufLen < sizeof(HY_DVR_COMPRESSION_CHANNEL_V2))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_COMPRESSION_CHANNEL_V2), dwBufLen);
					break;
				}

				LPHY_DVR_COMPRESSION_CHANNEL_V2 pcfg = (LPHY_DVR_COMPRESSION_CHANNEL_V2) pBuffer;

				nRet = SetSmallCompressCfg(pcfg, nNeedReboot);
			}

			break;
		default:
			{
				nRet = IDVR_CIU_UNSUPPORTED_MINORTYPE;
				MPU_ERR("unsupported dwMinorType = %lu\n", dwMinorType);
			}
			break;
	}

	MPU_DBG("ID %d, Major %d, Minor %d, return %d\n\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType, nRet);
	return nRet;
}
int CIU_GetConfigure_Record(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen)
{
	int nRet = IDVR_OK;

	printf("ID %d, Major %d, Minor %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType);

	switch (dwMinorType)
	{
			/*获取录像配置 */
		case RECORDCFG_ALL:
			{
				///////////////////////////////////////////////////////////////////////////////
				//判断buf空间是否足够
				if (*pdwBufLen < RECORD_SCHED_LEN)
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", RECORD_SCHED_LEN, *pdwBufLen);
					return nRet;
				}

				LPHY_DVR_RECORD_SCHED pcfg = (LPHY_DVR_RECORD_SCHED) pBuffer;


				///////////////////////////////////////////////////////////////////////////////
				//初始化输出缓冲区
				memset(pcfg, 0, RECORD_SCHED_LEN);
				*pdwBufLen = RECORD_SCHED_LEN;

				nRet = GetRecordCfg(pcfg);
			}
			break;
		
		/*获取新的录像配置 */
		case RECORDCFG_ALL_V2:
			{
				///////////////////////////////////////////////////////////////////////////////
				//判断buf空间是否足够
				if (*pdwBufLen < sizeof(HY_DVR_RECORD_SCHEDULE_V2))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof (HY_DVR_RECORD_SCHEDULE_V2), *pdwBufLen);
					return nRet;
				}

				LPHY_DVR_RECORD_SCHEDULE_V2 pcfg = (LPHY_DVR_RECORD_SCHEDULE_V2) pBuffer;
				//初始化输出缓冲区
				memset(pcfg, 0, sizeof (HY_DVR_RECORD_SCHEDULE_V2));
				*pdwBufLen = sizeof (HY_DVR_RECORD_SCHEDULE_V2);

				nRet = GetRecordCfgV2(pcfg);
			}
			break;

		case RECORD_CTRL_STATE:
			{
				///////////////////////////////////////////////////////////////////////////////
				//判断buf空间是否足够
				if (*pdwBufLen < sizeof (HY_DVR_RECORD_CTRLSTATE))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof (HY_DVR_RECORD_CTRLSTATE), *pdwBufLen);
					return nRet;
				}

				LPHY_DVR_RECORD_CTRLSTATE pcfg = (LPHY_DVR_RECORD_CTRLSTATE) pBuffer;

				*pdwBufLen = sizeof (HY_DVR_RECORD_CTRLSTATE);
				nRet = GetRecordMode(pcfg);
			}
			break;

		case RECORD_STREAM:
			{
				///////////////////////////////////////////////////////////////////////////////
				//判断buf空间是否足够
				if (*pdwBufLen < sizeof (HY_DVR_RECORD_STREAM))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof (HY_DVR_RECORD_STREAM), *pdwBufLen);
					return nRet;
				}

				LPHY_DVR_RECORD_STREAM pcfg = (LPHY_DVR_RECORD_STREAM) pBuffer;

				*pdwBufLen = sizeof (HY_DVR_RECORD_STREAM);
				nRet = GetRecordStream(pcfg);
			}
			break;

		default:
			{
				nRet = IDVR_CIU_UNSUPPORTED_MINORTYPE;
				MPU_ERR("unsupported dwMinorType = %lu\n", dwMinorType);
			}
			break;
	}

	printf("ID %d, Major %d, Minor %d, return %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType, nRet);
	return nRet;
}

int CIU_SetConfigure_Record(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, const BYTE * pBuffer, DWORD dwBufLen)
{
	int nRet = IDVR_OK;

	MPU_DBG("ID %d, Major %d, Minor %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType);

	switch (dwMinorType)
	{
			/*设置录像配置 */
		case RECORDCFG_ALL:
			{
				///////////////////////////////////////////////////////////////////////////////
				//判断buf空间是否足够
				if (dwBufLen < RECORD_SCHED_LEN)
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", RECORD_SCHED_LEN, dwBufLen);
					break;
				}

				LPHY_DVR_RECORD_SCHED pcfg = (LPHY_DVR_RECORD_SCHED) pBuffer;

				nRet = SetRecordCfg(dwUserID, pcfg);
			}
			break;
		
		/* 设置新的录像计划配置*/
		case RECORDCFG_ALL_V2:
    	{
    		///////////////////////////////////////////////////////////////////////////////
    		//判断buf空间是否足够
    		if (dwBufLen < sizeof (HY_DVR_RECORD_SCHEDULE_V2))
    		{
    			nRet = IDVR_CIU_BUF_SMALL;
    			MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof (HY_DVR_RECORD_SCHEDULE_V2), dwBufLen);
    			break;
    		}

    		LPHY_DVR_RECORD_SCHEDULE_V2 pcfg = (LPHY_DVR_RECORD_SCHEDULE_V2) pBuffer;

            nRet = SetRecordCfgV2(pcfg);
    	}
	    break;

		case RECORD_CTRL_STATE:
			{
				///////////////////////////////////////////////////////////////////////////////
				//判断buf空间是否足够
				if (dwBufLen < sizeof (HY_DVR_RECORD_CTRLSTATE))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof (HY_DVR_RECORD_CTRLSTATE), dwBufLen);
					break;
				}

				LPHY_DVR_RECORD_CTRLSTATE pcfg = (LPHY_DVR_RECORD_CTRLSTATE) pBuffer;

				nRet = SetRecordMode(dwUserID, pcfg);
			}

		case RECORD_STREAM:
			{
				///////////////////////////////////////////////////////////////////////////////
				//判断buf空间是否足够
				if (dwBufLen < sizeof (HY_DVR_RECORD_STREAM))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof (HY_DVR_RECORD_STREAM), dwBufLen);
					break;
				}

				LPHY_DVR_RECORD_STREAM pcfg = (LPHY_DVR_RECORD_STREAM) pBuffer;

				nRet = SetRecordStream(pcfg);
			}

		default:
			{
				nRet = IDVR_CIU_UNSUPPORTED_MINORTYPE;
				MPU_ERR("unsupported dwMinorType = %lu\n", dwMinorType);
			}
			break;
	}

	MPU_DBG("ID %d, Major %d, Minor %d, return %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType, nRet);
	return nRet;
}

int CIU_GetExtentOSD(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen)
{
	int nRet = IDVR_OK;

	MPU_INFO("ID %d, Major %d, Minor %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType);

	switch (dwMinorType)
	{
			//获取BUFFER SIZE的信息
		case EXTENT_OSD_LIST:
			{
				///////////////////////////////////////////////////////////////////////////////
				//判断buf空间是否足够
				if (*pdwBufLen < sizeof(HY_DVR_OSD_EX))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_OSD_EX), *pdwBufLen);
					break;
				}

				HY_DVR_OSD_EX *pcfg = (HY_DVR_OSD_EX *) pBuffer;
				
				nRet = GetExtentOSDCfg(pcfg);
			}
			break;

		case EXTENT_GET_OSD_NODE:
			{
				///////////////////////////////////////////////////////////////////////////////
				//判断buf空间是否足够
				if (*pdwBufLen < sizeof(HY_DVR_OSD_NODE_EX))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_OSD_NODE_EX), *pdwBufLen);
					break;
				}

				HY_DVR_OSD_NODE_EX *pcfg = (HY_DVR_OSD_NODE_EX *) pBuffer;

				nRet = GetExtentOSDNodeCfg(pcfg);
			}
			break;

		default:
			{
				MPU_ERR("unsupported dwMinorType = %lu\n", dwMinorType);
				nRet = IDVR_CIU_UNSUPPORTED_MINORTYPE;
			}
			break;
	}


	MPU_INFO("ID %d, Major %d, Minor %d, return %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType, nRet);
	return nRet;    
}

int CIU_SetExtentOSD(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, const BYTE * pBuffer, DWORD  dwBufLen)
{
	int nRet = IDVR_OK;

	MPU_INFO("ID %d, Major %d, Minor %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType);

	switch (dwMinorType)
	{
			//获取BUFFER SIZE的信息
		case EXTENT_OSD_LIST:
			{
				///////////////////////////////////////////////////////////////////////////////
				//判断buf空间是否足够
				if (dwBufLen < sizeof(HY_DVR_OSD_EX))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_OSD_EX), dwBufLen);
					break;
				}

				HY_DVR_OSD_EX *pcfg = (HY_DVR_OSD_EX *) pBuffer;

				nRet = SetExtentOSDCfg(pcfg);
			}
			break;

		case EXTENT_DEL_OSD_NODE:
		case EXTENT_ADD_OSD_NODE:
		case EXTENT_SET_OSD_NODE:
			{
				///////////////////////////////////////////////////////////////////////////////
				//判断buf空间是否足够
				if (dwBufLen < sizeof(HY_DVR_OSD_NODE_EX))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_OSD_NODE_EX), dwBufLen);
					break;
				}

				HY_DVR_OSD_NODE_EX *pcfg = (HY_DVR_OSD_NODE_EX *) pBuffer;

				nRet = SetExtentOSDNodeCfg(pcfg);
			}
			break;

		default:
			{
				MPU_ERR("Unsupported dwMinorType = %lu\n", dwMinorType);
				nRet = IDVR_CIU_UNSUPPORTED_MINORTYPE;
			}
			break;
	}

	MPU_INFO("ID %d, Major %d, Minor %d, return %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType, nRet);
	return nRet;    
}


int CIU_GetOSDV2(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen)
{
	int nRet = IDVR_OK;
	MPU_INFO("ID %d, Major %d, Minor %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType);

	switch (dwMinorType)
	{
			//获取BUFFER SIZE的信息
		case OSD_INFO_V2:
			{
				///////////////////////////////////////////////////////////////////////////////
				//判断buf空间是否足够
				if (*pdwBufLen < sizeof(HY_DVR_OSD_V2))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_OSD_V2), *pdwBufLen);
					break;
				}

				HY_DVR_OSD_V2 *pcfg = (HY_DVR_OSD_V2 *) pBuffer;
				
				nRet = GetOSDV2Cfg(pcfg);
			}
			break;

		default:
			{
				MPU_ERR("unsupported dwMinorType = %lu\n", dwMinorType);
				nRet = IDVR_CIU_UNSUPPORTED_MINORTYPE;
			}
			break;
	}


	MPU_INFO("ID %d, Major %d, Minor %d, return %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType, nRet);
	return nRet;
}


int CIU_SetOSDV2(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, const BYTE * pBuffer, DWORD  dwBufLen)
{
	int nRet = IDVR_OK;
	MPU_INFO("ID %d, Major %d, Minor %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType);
	switch (dwMinorType)
	{
			//获取BUFFER SIZE的信息
		case OSD_INFO_V2:
			{
				///////////////////////////////////////////////////////////////////////////////
				//判断buf空间是否足够
				if (dwBufLen < sizeof(HY_DVR_OSD_V2))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_OSD_V2), dwBufLen);
					break;
				}

				HY_DVR_OSD_V2 *pcfg = (HY_DVR_OSD_V2 *) pBuffer;
				nRet = SetOSDV2Cfg(pcfg);
			}
			break;

		case OSD_INFO_V4:
			{
				//FIXME:本设备只支持V2，协议这里多设置了一次V4，这里不给返回0会导致Web提示保存失败，待协议兼容后删掉
				nRet = IDVR_OK;
			}
			break;

		default:
			{
				MPU_ERR("Unsupported dwMinorType = %lu\n", dwMinorType);
				nRet = IDVR_CIU_UNSUPPORTED_MINORTYPE;
			}
			break;
	}

	MPU_INFO("ID %d, Major %d, Minor %d, return %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType, nRet);
	return nRet;
}

/* 获取录像文件列表属性(获取当前总共文件记录有多少条和每页有多少条) */
int CIU_GetConfigure_RecordFileListAttr(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen)
{
	int nRet = IDVR_OK;

	MPU_DBG("ID %d, Major %d, Minor %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType);

	switch (dwMinorType)
	{
		case RECORD_FILE_LIST_ATTRIBUTE_ALL:
			{
				///////////////////////////////////////////////////////////////////////////////
				//判断buf空间是否足够
				if (*pdwBufLen < sizeof(HY_DVR_RECORD_FILE_LIST_ATTRIBUTE))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_RECORD_FILE_LIST_ATTRIBUTE), *pdwBufLen);
					break;
				}

				HY_DVR_RECORD_FILE_LIST_ATTRIBUTE *pcfg = (HY_DVR_RECORD_FILE_LIST_ATTRIBUTE *) pBuffer;

				///////////////////////////////////////////////////////////////////////////////
				//初始化输出缓冲区
				memset(pcfg, 0, sizeof(HY_DVR_RECORD_FILE_LIST_ATTRIBUTE));
				*pdwBufLen = sizeof(HY_DVR_RECORD_FILE_LIST_ATTRIBUTE);

				nRet = GetRecordFileListAttr(pcfg);
			}
			break;

		default:
			{
				nRet = IDVR_CIU_UNSUPPORTED_MINORTYPE;
				MPU_ERR("unsupported dwMinorType = %lu\n", dwMinorType);
			}
			break;
	}

	MPU_DBG("ID %d, Major %d, Minor %d, return %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType, nRet);
	return nRet;
}

/* 设置录像文件列表属性(设置每一页显示多少条记录) */
int CIU_SetConfigure_RecordFileListAttr(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, const BYTE * pBuffer, DWORD dwBufLen)
{
	int nRet = IDVR_OK;

	MPU_DBG("ID %d, Major %d, Minor %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType);

	switch (dwMinorType)
	{
		case RECORD_FILE_LIST_ATTRIBUTE_ALL:
			{
				///////////////////////////////////////////////////////////////////////////////
				//判断buf空间是否足够
				if (dwBufLen < sizeof(HY_DVR_RECORD_FILE_LIST_ATTRIBUTE))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_RECORD_FILE_LIST_ATTRIBUTE), dwBufLen);
					break;
				}

				HY_DVR_RECORD_FILE_LIST_ATTRIBUTE *pcfg = (HY_DVR_RECORD_FILE_LIST_ATTRIBUTE *) pBuffer;

				nRet = SetRecordFileListAttr(pcfg);
			}
			break;

        case FILE_LIST_ATTR_SEARCHTIME:
            {
                ///////////////////////////////////////////////////////////////////////////////
                //判断buf空间是否足够
                if (dwBufLen < sizeof(HY_DVR_FILE_LIST_ATTRIBUTE_SEARCH_BY_TIME))
                {
                    nRet = IDVR_CIU_BUF_SMALL;
                    MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_FILE_LIST_ATTRIBUTE_SEARCH_BY_TIME), dwBufLen);
                    break;
                }
            
                HY_DVR_FILE_LIST_ATTRIBUTE_SEARCH_BY_TIME *pcfg = (HY_DVR_FILE_LIST_ATTRIBUTE_SEARCH_BY_TIME *) pBuffer;
            
                nRet = SetRecordSearchTimeSeg(pcfg);
            }
            break;

		default:
			{
				nRet = IDVR_CIU_UNSUPPORTED_MINORTYPE;
				MPU_ERR("unsupported dwMinorType = %lu\n", dwMinorType);
			}
			break;
	}


	MPU_DBG("ID %d, Major %d, Minor %d, return %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType, nRet);
	return nRet;
}

/* 获取录像文件当前显示的是哪一页 */
int CIU_GetConfigure_RecordFileCurrentPage(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen)
{
	int nRet = IDVR_OK;

	MPU_DBG("ID %d, Major %d, Minor %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType);

	switch (dwMinorType)
	{
		case RECORD_FILE_CURRENT_PAGE_ALL:
			{
				///////////////////////////////////////////////////////////////////////////////
				//判断buf空间是否足够
				if (*pdwBufLen < sizeof(HY_DVR_RECORD_FILE_CURRENT_PAGE))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_RECORD_FILE_CURRENT_PAGE), *pdwBufLen);
					break;
				}

				HY_DVR_RECORD_FILE_CURRENT_PAGE *pcfg = (HY_DVR_RECORD_FILE_CURRENT_PAGE *) pBuffer;

				///////////////////////////////////////////////////////////////////////////////
				//初始化输出缓冲区
				memset(pcfg, 0, sizeof(HY_DVR_RECORD_FILE_CURRENT_PAGE));
				*pdwBufLen = sizeof(HY_DVR_RECORD_FILE_CURRENT_PAGE);

				nRet = GetRecordFileCurrentPage(pcfg);
			}
			break;

		default:
			{
				nRet = IDVR_CIU_UNSUPPORTED_MINORTYPE;
				MPU_ERR("unsupported dwMinorType = %lu\n", dwMinorType);
			}
			break;
	}

	MPU_DBG("ID %d, Major %d, Minor %d, return %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType, nRet);
	return nRet;
}

/* 设置录像文件当前页(设置当前需要显示哪一页) */
int CIU_SetConfigure_RecordFileCurrentPage(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, const BYTE * pBuffer, DWORD dwBufLen)
{
	int nRet = IDVR_OK;

	MPU_DBG("ID %d, Major %d, Minor %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType);

	switch (dwMinorType)
	{
		case RECORD_FILE_CURRENT_PAGE_ALL:
			{
				///////////////////////////////////////////////////////////////////////////////
				//判断buf空间是否足够
				if (dwBufLen < sizeof(HY_DVR_RECORD_FILE_CURRENT_PAGE))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_RECORD_FILE_CURRENT_PAGE), dwBufLen);
					break;
				}

				HY_DVR_RECORD_FILE_CURRENT_PAGE *pcfg = (HY_DVR_RECORD_FILE_CURRENT_PAGE *) pBuffer;

				nRet = SetRecordFileCurrentPage(pcfg);
			}
			break;

		default:
			{
				nRet = IDVR_CIU_UNSUPPORTED_MINORTYPE;
				MPU_ERR("unsupported dwMinorType = %lu\n", dwMinorType);
			}
			break;
	}


	MPU_DBG("ID %d, Major %d, Minor %d, return %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType, nRet);
	return nRet;
}

/* 获取录像文件列表 */
int CIU_GetConfigure_RecordFileList(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen)
{
	int nRet = IDVR_OK;

	MPU_DBG("ID %d, Major %d, Minor %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType);

	switch (dwMinorType)
	{
		case RECORD_FILE_LIST_ALL:
			{
				///////////////////////////////////////////////////////////////////////////////
				//判断buf空间是否足够
				if (*pdwBufLen < sizeof(HY_DVR_RECORD_FILE_LIST))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_RECORD_FILE_LIST), *pdwBufLen);
					break;
				}

				HY_DVR_RECORD_FILE_LIST *pcfg = (HY_DVR_RECORD_FILE_LIST *) pBuffer;

				///////////////////////////////////////////////////////////////////////////////
				//初始化输出缓冲区
				memset(pcfg, 0, sizeof(HY_DVR_RECORD_FILE_LIST));
				*pdwBufLen = sizeof(HY_DVR_RECORD_FILE_LIST);

				nRet = GetRecordFileList(pcfg);
			}
			break;

		case RECORD_FILE_LIST_ALL_EX:
			{
				///////////////////////////////////////////////////////////////////////////////
				//判断buf空间是否足够
				if (*pdwBufLen < sizeof(HY_DVR_RECORD_FILE_LIST_EX))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_RECORD_FILE_LIST_EX), *pdwBufLen);
					break;
				}

				HY_DVR_RECORD_FILE_LIST_EX *pcfg = (HY_DVR_RECORD_FILE_LIST_EX *) pBuffer;

				///////////////////////////////////////////////////////////////////////////////
				//初始化输出缓冲区
				memset(pcfg, 0, sizeof(HY_DVR_RECORD_FILE_LIST_EX));
				*pdwBufLen = sizeof(HY_DVR_RECORD_FILE_LIST_EX);

				nRet = GetRecordFileListEX(pcfg);
			}
			break;
	   case RECORD_FILE_LIST_ALL_V1:
			{
				///////////////////////////////////////////////////////////////////////////////
				//判断buf空间是否足够
				if (*pdwBufLen < sizeof(HY_DVR_RECORD_FILE_LIST_V1))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_RECORD_FILE_LIST_V1), *pdwBufLen);
					break;
				}

				HY_DVR_RECORD_FILE_LIST_V1 *pcfg = (HY_DVR_RECORD_FILE_LIST_V1 *) pBuffer;

				///////////////////////////////////////////////////////////////////////////////
				//初始化输出缓冲区
				memset(pcfg, 0, sizeof(HY_DVR_RECORD_FILE_LIST_V1));
				*pdwBufLen = sizeof(HY_DVR_RECORD_FILE_LIST_V1);

				nRet = GetRecordFileListEX_V1(pcfg);
			}
			break;

        case RECORD_FILE_INFO_BY_FILENAME:
			{
				///////////////////////////////////////////////////////////////////////////////
				//判断buf空间是否足够
				if (*pdwBufLen < sizeof(HY_DVR_RECORD_FILE_V1))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_RECORD_FILE_V1), *pdwBufLen);
					break;
				}

				HY_DVR_RECORD_FILE_V1 *pcfg = (HY_DVR_RECORD_FILE_V1 *) pBuffer;

				///////////////////////////////////////////////////////////////////////////////
				//初始化输出缓冲区
				//memset(pcfg, 0, sizeof(HY_DVR_RECORD_FILE_V1));
				*pdwBufLen = sizeof(HY_DVR_RECORD_FILE_V1);

				nRet = GetRecordFileInfoByFileName(pcfg);
			}
			break; 

		default:
			{
				nRet = IDVR_CIU_UNSUPPORTED_MINORTYPE;
				MPU_ERR("unsupported dwMinorType = %lu\n", dwMinorType);
			}
			break;
	}

	MPU_DBG("ID %d, Major %d, Minor %d, return %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType, nRet);
	return nRet;
}


/* 设置录像文件列表(无效) */
int CIU_SetConfigure_RecordFileList(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, const BYTE * pBuffer, DWORD dwBufLen)
{
	int nRet = IDVR_OK;

	MPU_DBG("ID %d, Major %d, Minor %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType);

	switch (dwMinorType)
	{
		case RECORD_FILE_LIST_ALL:
			{
			    /* 无效，不支持设置 */
			}
			break;

		default:
			{
				nRet = IDVR_CIU_UNSUPPORTED_MINORTYPE;
				MPU_ERR("unsupported dwMinorType = %lu\n", dwMinorType);
			}
			break;
	}

	MPU_DBG("ID %d, Major %d, Minor %d, return %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType, nRet);
	return nRet;
}

/*------------------------------------------------------------------------------------------------------------------------------*/

/* 获取抓图文件列表属性(获取当前总共文件记录有多少条和每页有多少条) */
int CIU_GetConfigure_SnapshotFileListAttr(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen)
{
	int nRet = IDVR_OK;

	MPU_DBG("ID %d, Major %d, Minor %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType);

	switch (dwMinorType)
	{
		case SNAPSHOT_FILE_LIST_ATTRIBUTE_ALL:
			{
				///////////////////////////////////////////////////////////////////////////////
				//判断buf空间是否足够
				if (*pdwBufLen < sizeof(HY_DVR_SNAPSHOT_FILE_LIST_ATTRIBUTE))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_SNAPSHOT_FILE_LIST_ATTRIBUTE), *pdwBufLen);
					break;
				}

				HY_DVR_SNAPSHOT_FILE_LIST_ATTRIBUTE *pcfg = (HY_DVR_SNAPSHOT_FILE_LIST_ATTRIBUTE *) pBuffer;

				///////////////////////////////////////////////////////////////////////////////
				//初始化输出缓冲区
				memset(pcfg, 0, sizeof(HY_DVR_SNAPSHOT_FILE_LIST_ATTRIBUTE));
				*pdwBufLen = sizeof(HY_DVR_SNAPSHOT_FILE_LIST_ATTRIBUTE);

				nRet = GetSnapshotFileListAttr(pcfg);
			}
			break;

		default:
			{
				nRet = IDVR_CIU_UNSUPPORTED_MINORTYPE;
				MPU_ERR("unsupported dwMinorType = %lu\n", dwMinorType);
			}
			break;
	}

	MPU_DBG("ID %d, Major %d, Minor %d, return %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType, nRet);
	return nRet;
}

/* 设置录像文件列表属性(设置每一页显示多少条记录) */
int CIU_SetConfigure_SnapshotFileListAttr(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, const BYTE * pBuffer, DWORD dwBufLen)
{
	int nRet = IDVR_OK;

	MPU_DBG("ID %d, Major %d, Minor %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType);

	switch (dwMinorType)
	{
		case RECORD_FILE_LIST_ATTRIBUTE_ALL:
			{
				///////////////////////////////////////////////////////////////////////////////
				//判断buf空间是否足够
				if (dwBufLen < sizeof(HY_DVR_SNAPSHOT_FILE_LIST_ATTRIBUTE))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_SNAPSHOT_FILE_LIST_ATTRIBUTE), dwBufLen);
					break;
				}

				HY_DVR_SNAPSHOT_FILE_LIST_ATTRIBUTE *pcfg = (HY_DVR_SNAPSHOT_FILE_LIST_ATTRIBUTE *) pBuffer;

				nRet = SetSnapshotFileListAttr(pcfg);
			}
			break;

        case FILE_LIST_ATTR_SEARCHTIME:
            {
                ///////////////////////////////////////////////////////////////////////////////
                //判断buf空间是否足够
                if (dwBufLen < sizeof(HY_DVR_FILE_LIST_ATTRIBUTE_SEARCH_BY_TIME))
                {
                    nRet = IDVR_CIU_BUF_SMALL;
                    MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_FILE_LIST_ATTRIBUTE_SEARCH_BY_TIME), dwBufLen);
                    break;
                }
            
                HY_DVR_FILE_LIST_ATTRIBUTE_SEARCH_BY_TIME *pcfg = (HY_DVR_FILE_LIST_ATTRIBUTE_SEARCH_BY_TIME *) pBuffer;
            
                nRet = SetSnapshotSearchTimeSeg(pcfg);
            }
            break;

		default:
			{
				nRet = IDVR_CIU_UNSUPPORTED_MINORTYPE;
				MPU_ERR("unsupported dwMinorType = %lu\n", dwMinorType);
			}
			break;
	}


	MPU_DBG("ID %d, Major %d, Minor %d, return %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType, nRet);
	return nRet;
}

/* 获取抓图文件当前显示的是哪一页 */
int CIU_GetConfigure_SnapshotFileCurrentPage(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen)
{
	int nRet = IDVR_OK;

	MPU_DBG("ID %d, Major %d, Minor %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType);

	switch (dwMinorType)
	{
		case SNAPSHOT_FILE_CURRENT_PAGE_ALL:
			{
				///////////////////////////////////////////////////////////////////////////////
				//判断buf空间是否足够
				if (*pdwBufLen < sizeof(HY_DVR_SNAPSHOT_FILE_CURRENT_PAGE))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_SNAPSHOT_FILE_CURRENT_PAGE), *pdwBufLen);
					break;
				}

				HY_DVR_SNAPSHOT_FILE_CURRENT_PAGE *pcfg = (HY_DVR_SNAPSHOT_FILE_CURRENT_PAGE *) pBuffer;

				///////////////////////////////////////////////////////////////////////////////
				//初始化输出缓冲区
				memset(pcfg, 0, sizeof(HY_DVR_SNAPSHOT_FILE_CURRENT_PAGE));
				*pdwBufLen = sizeof(HY_DVR_SNAPSHOT_FILE_CURRENT_PAGE);

				nRet = GetSnapshotFileCurrentPage(pcfg);
			}
			break;

		default:
			{
				nRet = IDVR_CIU_UNSUPPORTED_MINORTYPE;
				MPU_ERR("unsupported dwMinorType = %lu\n", dwMinorType);
			}
			break;
	}

	MPU_DBG("ID %d, Major %d, Minor %d, return %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType, nRet);
	return nRet;
}

/* 设置抓图文件当前页(设置当前需要显示哪一页) */
int CIU_SetConfigure_SnapshotFileCurrentPage(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, const BYTE * pBuffer, DWORD dwBufLen)
{
	int nRet = IDVR_OK;

	MPU_DBG("ID %d, Major %d, Minor %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType);

	switch (dwMinorType)
	{
		case SNAPSHOT_FILE_CURRENT_PAGE_ALL:
			{
				///////////////////////////////////////////////////////////////////////////////
				//判断buf空间是否足够
				if (dwBufLen < sizeof(HY_DVR_SNAPSHOT_FILE_CURRENT_PAGE))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_SNAPSHOT_FILE_CURRENT_PAGE), dwBufLen);
					break;
				}

				HY_DVR_SNAPSHOT_FILE_CURRENT_PAGE *pcfg = (HY_DVR_SNAPSHOT_FILE_CURRENT_PAGE *) pBuffer;

				nRet = SetSnapshotFileCurrentPage(pcfg);
			}
			break;

		default:
			{
				nRet = IDVR_CIU_UNSUPPORTED_MINORTYPE;
				MPU_ERR("unsupported dwMinorType = %lu\n", dwMinorType);
			}
			break;
	}


	MPU_DBG("ID %d, Major %d, Minor %d, return %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType, nRet);
	return nRet;
}

/* 获取抓图文件列表 */
int CIU_GetConfigure_SnapshotFileList(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen)
{
	int nRet = IDVR_OK;

	MPU_DBG("ID %d, Major %d, Minor %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType);

	switch (dwMinorType)
	{
		case SNAPSHOT_FILE_LIST_ALL:
			{
				///////////////////////////////////////////////////////////////////////////////
				//判断buf空间是否足够
				if (*pdwBufLen < sizeof(HY_DVR_SNAPSHOT_FILE_LIST))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_SNAPSHOT_FILE_LIST), *pdwBufLen);
					break;
				}

				HY_DVR_SNAPSHOT_FILE_LIST *pcfg = (HY_DVR_SNAPSHOT_FILE_LIST *) pBuffer;

				///////////////////////////////////////////////////////////////////////////////
				//初始化输出缓冲区
				memset(pcfg, 0, sizeof(HY_DVR_SNAPSHOT_FILE_LIST));
				*pdwBufLen = sizeof(HY_DVR_SNAPSHOT_FILE_LIST);

				nRet = GetSnapshotFileList(pcfg);
			}
			break;

		case SNAPSHOT_FILE_LIST_ALL_EX:
			{
				///////////////////////////////////////////////////////////////////////////////
				//判断buf空间是否足够
				if (*pdwBufLen < sizeof(HY_DVR_SNAPSHOT_FILE_LIST_EX))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_SNAPSHOT_FILE_LIST_EX), *pdwBufLen);
					break;
				}

				HY_DVR_SNAPSHOT_FILE_LIST_EX *pcfg = (HY_DVR_SNAPSHOT_FILE_LIST_EX *) pBuffer;

				///////////////////////////////////////////////////////////////////////////////
				//初始化输出缓冲区
				memset(pcfg, 0, sizeof(HY_DVR_SNAPSHOT_FILE_LIST_EX));
				*pdwBufLen = sizeof(HY_DVR_SNAPSHOT_FILE_LIST_EX);

				nRet = GetSnapshotFileListEX(pcfg);
			}
			break;

		default:
			{
				nRet = IDVR_CIU_UNSUPPORTED_MINORTYPE;
				MPU_ERR("unsupported dwMinorType = %lu\n", dwMinorType);
			}
			break;
	}

	MPU_DBG("ID %d, Major %d, Minor %d, return %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType, nRet);
	return nRet;
}

/* 设置抓图文件列表(无效) */
int CIU_SetConfigure_SnapshotFileList(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, const BYTE * pBuffer, DWORD dwBufLen)
{
	int nRet = IDVR_OK;

	MPU_DBG("ID %d, Major %d, Minor %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType);

	switch (dwMinorType)
	{
		case SNAPSHOT_FILE_LIST_ALL:
			{
			    /* 无效，不支持设置 */
			}
			break;

		default:
			{
				nRet = IDVR_CIU_UNSUPPORTED_MINORTYPE;
				MPU_ERR("unsupported dwMinorType = %lu\n", dwMinorType);
			}
			break;
	}

	MPU_DBG("ID %d, Major %d, Minor %d, return %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType, nRet);
	return nRet;
}

/* 获取定时抓图配置 */
int CIU_GetConfigure_TimedCaptureCfg(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen)
{
	int nRet = IDVR_OK;

	MPU_DBG("ID %d, Major %d, Minor %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType);

	switch (dwMinorType)
	{
		case TIMED_CAPTURE_CFG_ALL:
			{
				///////////////////////////////////////////////////////////////////////////////
				//判断buf空间是否足够
				if (*pdwBufLen < sizeof(HY_TIMED_CAPTURE_CFG))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_TIMED_CAPTURE_CFG), *pdwBufLen);
					break;
				}

				HY_TIMED_CAPTURE_CFG *pcfg = (HY_TIMED_CAPTURE_CFG *) pBuffer;

				///////////////////////////////////////////////////////////////////////////////
				//初始化输出缓冲区
				memset(pcfg, 0, sizeof(HY_TIMED_CAPTURE_CFG));
				*pdwBufLen = sizeof(HY_TIMED_CAPTURE_CFG);

				nRet = GetTimedCaptureCfg(pcfg);
			}
			break;

		default:
			{
				nRet = IDVR_CIU_UNSUPPORTED_MINORTYPE;
				MPU_ERR("unsupported dwMinorType = %lu\n", dwMinorType);
			}
			break;
	}

	MPU_DBG("ID %d, Major %d, Minor %d, return %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType, nRet);
	return nRet;
}

/* 设置定时抓图配置 */
int CIU_SetConfigure_TimedCaptureCfg(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, const BYTE * pBuffer, DWORD dwBufLen)
{
	int nRet = IDVR_OK;

	MPU_DBG("ID %d, Major %d, Minor %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType);

	switch (dwMinorType)
	{
		case TIMED_CAPTURE_CFG_ALL:
			{
				//判断buf空间是否足够
				if (dwBufLen < sizeof(HY_TIMED_CAPTURE_CFG))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_TIMED_CAPTURE_CFG), dwBufLen);
					break;
				}

				HY_TIMED_CAPTURE_CFG *pcfg = (HY_TIMED_CAPTURE_CFG *) pBuffer;

				nRet = SetTimedCaptureCfg(pcfg);
			}
			break;

		default:
			{
				nRet = IDVR_CIU_UNSUPPORTED_MINORTYPE;
				MPU_ERR("unsupported dwMinorType = %lu\n", dwMinorType);
			}
			break;
	}

	MPU_DBG("ID %d, Major %d, Minor %d, return %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType, nRet);
	return nRet;
}


static int GetAudioAlarmCfgEX(HY_AUDIO_ALARM_CONFIG_EX *pcfg)
{
	int nRet = IDVR_OK, nDay, i, num;
    int nCh = 0;

    pcfg->bValid = 1;
	nRet = MPU_AVL_GetAudioAlarmCfg(0, &pcfg->nEnable, &pcfg->nAudioType, &pcfg->nAudioDelayOutput, &pcfg->nAudioVolume, &pcfg->iAudioPlayTimes);
    
	AudioAlarmSchedule sched;

	//! 本结构体是否有效，=0表示无效
	pcfg->struSchedTime.bValid = 1;

	nRet = MPU_AVL_AudioAlarm_GetSchedule(nCh, &sched);

	num = DVR_MAX_TIMESEGMENT / HY_DVR_ALARMOUTSEG_DAY;

	//时间表需要扩展
	for (nDay = 0; nDay < DVR_MAX_DAYS; ++nDay)
	{
		for (i = 0; i < DVR_MAX_TIMESEGMENT; ++i)
		{
			pcfg->struSchedTime.bySetType[nDay][i] = sched.dwTimeSeg[nDay][i / num];
		}
	}

	//! 状态,0为禁用(全天布防),1为启用,2为不支持
	pcfg->struSchedTime.byState = sched.bytState;

	pcfg->struSchedTime.byReserve[0] = sched.bytReserve[0];
	pcfg->struSchedTime.byReserve[1] = sched.bytReserve[1];
	pcfg->struSchedTime.byReserve[2] = sched.bytReserve[2];

	return nRet;
}

static int SetAudioAlarmCfgEX(HY_AUDIO_ALARM_CONFIG_EX *pcfg)
{
	int nRet = IDVR_OK, nDay, i, num;
    AudioAlarmSchedule sched;
    int nCh = 0;

    ///////////////////////////////////////////////////////////////////////////////
	//本结构体是否有效，=0表示无效
	if (0 == pcfg->bValid)
	{
		MPU_DBG("0 == pcfg->bValid\n");
		return IDVR_CIU_STRUCT_UNVALID;
	}
    nRet = MPU_AVL_SetAudioAlarmCfg(0, pcfg->nEnable, pcfg->nAudioType, pcfg->nAudioDelayOutput, pcfg->nAudioVolume,pcfg->iAudioPlayTimes);
	
	//本结构体是否有效，=0表示无效
	if (0 == pcfg->struSchedTime.bValid)
	{
		MPU_DBG("0 == pcfg->struSchedTime.bValid\n");
		return IDVR_CIU_STRUCT_UNVALID;
	}

	num = DVR_MAX_TIMESEGMENT / HY_DVR_ALARMOUTSEG_DAY;

	//时间表需要扩展
	for (nDay = 0; nDay < DVR_MAX_DAYS; ++nDay)
	{
		for (i = 0; i < HY_DVR_ALARMOUTSEG_DAY; ++i)
		{
			sched.dwTimeSeg[nDay][i] = pcfg->struSchedTime.bySetType[nDay][i * num];
		}
	}

	//! 状态,0为禁用(全天布防),1为启用,2为不支持
	sched.bytState = pcfg->struSchedTime.byState;

	sched.bytReserve[0] = pcfg->struSchedTime.byReserve[0];
	sched.bytReserve[1] = pcfg->struSchedTime.byReserve[1];
	sched.bytReserve[2] = pcfg->struSchedTime.byReserve[2];

	nRet = MPU_AVL_AudioAlarm_SetSchedule(nCh, &sched);
	
	return nRet;
}


int CIU_SetConfigure_AUDIO_ALARM_ADVANCE_CONFIG(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, const BYTE * pBuffer, DWORD dwBufLen)
{
	int nRet = IDVR_OK;

	MPU_DBG("ID %d, Major %d, Minor %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType);

	switch (dwMinorType)
	{
		case AUDIO_ALARM_CONFIG_ALL:
			{
				//判断buf空间是否足够
				if (dwBufLen < sizeof(HY_AUDIO_ALARM_CONFIG))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_AUDIO_ALARM_CONFIG), dwBufLen);
					break;
				}

				HY_AUDIO_ALARM_CONFIG *pcfg = (HY_AUDIO_ALARM_CONFIG *) pBuffer;

				nRet = SetAudioAlarmCfg(pcfg);
			}
			break;
		
        case AUDIO_ALARM_CONFIG_ALL_EX:
			{
				//判断buf空间是否足够
				if (dwBufLen < sizeof(HY_AUDIO_ALARM_CONFIG_EX))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_AUDIO_ALARM_CONFIG_EX), dwBufLen);
					break;
				}

				HY_AUDIO_ALARM_CONFIG_EX *pcfg = (HY_AUDIO_ALARM_CONFIG_EX *) pBuffer;

                nRet = SetAudioAlarmCfgEX(pcfg);
			}
			break;

		case AUDIO_ALARM_CONFIG_ALL_CHECK:
			{
				//判断buf空间是否足够
				if (dwBufLen < sizeof(HY_AUDIO_ALARM_CONFIG))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_AUDIO_ALARM_CONFIG), dwBufLen);
					break;
				}

				HY_AUDIO_ALARM_CONFIG *pcfg = (HY_AUDIO_ALARM_CONFIG *) pBuffer;

				nRet = TryListenAlarmAudio(pcfg);
			}
			break;

		case AUDIO_ALARM_CONFIG_ALL_CHECK_V1:
			{
				//判断buf空间是否足够
				if (dwBufLen < sizeof(HY_AUDIO_ALARM_CONFIG_V1))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_AUDIO_ALARM_CONFIG_V1), dwBufLen);
					break;
				}

				HY_AUDIO_ALARM_CONFIG_V1 *pcfg = (HY_AUDIO_ALARM_CONFIG_V1 *) pBuffer;

				nRet = TryListenAlarmAudioV1(pcfg);
			}
			break;

		case AUDIO_ALARM_CONFIG_V1:
			{
				//判断buf空间是否足够
				if (dwBufLen < sizeof(HY_AUDIO_ALARM_CONFIG_V1))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_AUDIO_ALARM_CONFIG_V1), dwBufLen);
					break;
				}

				HY_AUDIO_ALARM_CONFIG_V1 *pcfg = (HY_AUDIO_ALARM_CONFIG_V1 *) pBuffer;

				nRet = SetAudioAlarmCfgV1(pcfg);
			}
			break;

		default:
			{
				nRet = IDVR_CIU_UNSUPPORTED_MINORTYPE;
				MPU_ERR("unsupported dwMinorType = %lu\n", dwMinorType);
			}
			break;
	}

	MPU_DBG("ID %d, Major %d, Minor %d, return %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType, nRet);
	return nRet;
}

int CIU_GetConfigure_AUDIO_ALARM_ADVANCE_CONFIG(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen)
{
	int nRet = IDVR_OK;

	MPU_DBG("ID %d, Major %d, Minor %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType);

	switch (dwMinorType)
	{
		case AUDIO_ALARM_CONFIG_ALL:
			{
				///////////////////////////////////////////////////////////////////////////////
				//判断buf空间是否足够
				if (*pdwBufLen < sizeof(HY_AUDIO_ALARM_CONFIG))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_AUDIO_ALARM_CONFIG), *pdwBufLen);
					break;
				}

				HY_AUDIO_ALARM_CONFIG *pcfg = (HY_AUDIO_ALARM_CONFIG *) pBuffer;

				///////////////////////////////////////////////////////////////////////////////
				//初始化输出缓冲区
				*pdwBufLen = sizeof(HY_AUDIO_ALARM_CONFIG);

				nRet = GetAudioAlarmCfg(pcfg);
			}
			break;

		case AUDIO_ALARM_CONFIG_ALL_EX:
			{
				///////////////////////////////////////////////////////////////////////////////
				//判断buf空间是否足够
				if (*pdwBufLen < sizeof(HY_AUDIO_ALARM_CONFIG_EX))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_AUDIO_ALARM_CONFIG_EX), *pdwBufLen);
					break;
				}

				HY_AUDIO_ALARM_CONFIG_EX *pcfg = (HY_AUDIO_ALARM_CONFIG_EX *) pBuffer;

				///////////////////////////////////////////////////////////////////////////////
				//初始化输出缓冲区
				*pdwBufLen = sizeof(HY_AUDIO_ALARM_CONFIG_EX);
                nRet = GetAudioAlarmCfgEX(pcfg);
			}
			break;
		
		case AUDIO_ALARM_CONFIG_V1:
			{
				///////////////////////////////////////////////////////////////////////////////
				//判断buf空间是否足够
				if (*pdwBufLen < sizeof(HY_AUDIO_ALARM_CONFIG_V1))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_AUDIO_ALARM_CONFIG_V1), *pdwBufLen);
					break;
				}

				HY_AUDIO_ALARM_CONFIG_V1 *pcfg = (HY_AUDIO_ALARM_CONFIG_V1 *) pBuffer;

				///////////////////////////////////////////////////////////////////////////////
				//初始化输出缓冲区
				*pdwBufLen = sizeof(HY_AUDIO_ALARM_CONFIG_V1);

				nRet = GetAudioAlarmCfgV1(pcfg);
			}
			break;

		default:
			{
				nRet = IDVR_CIU_UNSUPPORTED_MINORTYPE;
				MPU_ERR("unsupported dwMinorType = %lu\n", dwMinorType);
			}
			break;
	}

	MPU_DBG("ID %d, Major %d, Minor %d, return %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType, nRet);
	return nRet;
}

int CIU_SetConfigure_FaceCap(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, const BYTE * pBuffer, DWORD dwBufLen)
{
	int nRet = IDVR_OK;
	MPU_DBG("ID %d, Major %d, Minor %d, return %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType, nRet);
	return nRet;
}

int CIU_GetConfigure_FaceCap(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen)
{
	int nRet = IDVR_OK;
	MPU_DBG("ID %d, Major %d, Minor %d, return %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType, nRet);
	return nRet;
}
