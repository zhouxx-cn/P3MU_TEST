
/*!
 *\file	 MPU_CIU_Config_Ptz.cpp
 *\brief	MPU模块，远程配置，云台设置
 *\author	林云
 *\date	2009/8/17

功能：
-#	远程配置，视频设置

说明：
-#	本文件使用新的网络库实现
 */

#include <stdio.h>

#include "MPU_CIU_global.h"
#include "tagActionInfo.h"
#include "tagPtzCode.h"


static int m_PtzEletronicChConfig_bInit = 0;

#define PTZ_SetEletronicProp(value,cmd,ch)  			\
	if ( 0 == m_PtzEletronicChConfig_bInit  || MPU_PTZ_GetEletronicProp(cmd,ch) != value)	\
	{												\
		MPU_PTZ_SetEletronicProp(value,cmd,ch);		\
	}												\

//获取云台配置串口信息，需外部将pcfg清零
static int GetPtzSerialConfig(int nCh, ZW_CAMERA_PTZ_CFG * pcfg)
{
	int nRet = IDVR_OK;

	//! 本结构体是否有效，=0表示无效
	pcfg->bValid = 1;
	//当前除通道0外其他通道的PTZ只做SpeedGET
	if(nCh == 0)
	{
		/* 波特率(bps)，0－50，1－75，2－110，3－150，4－300，5－600，6－1200，7－2400，8－4800，9－9600，10－19200， 11－38400，12－57600，13－76800，14－115.2k; */
		pcfg->dwBandrate = MPU_PTZ_GetBaudrate(nCh);

		/*解码器地址:0 - 255 */
		pcfg->dwAddress = MPU_PTZ_GetAddress(nCh);

		/*云台复位周期 */
		pcfg->dwWatchDelayBySecond = MPU_PTZ_GetResetCycle(nCh);

		/*云台守望功能开启*/
		pcfg->bWatch = MPU_PTZ_IsEnableKeeper(nCh);
	
		/*云台动作设置*/
		unsigned long dwDefAction;
		MPU_PTZ_GetDefaultAction(&dwDefAction, &pcfg->dwPTZActionID, nCh);
		pcfg->dwPTZActionType  = (eZWPTZActionType)(dwDefAction - ACTION_PTZ_PRESET);
	}
	else
	{
		pcfg->dwBandrate = 0;
		pcfg->dwAddress=0;
		pcfg->dwWatchDelayBySecond= 0;
		pcfg->bWatch = 0;
		pcfg->dwPTZActionID = 0;
		pcfg->dwPTZActionType = (eZWPTZActionType)0;
	}
#if 1
			/*云台功能开启*/
			pcfg->dwEnable = MPU_PTZ_IsEnable(nCh);
		MPU_DBG("--GetPtzSerialConfig--nCh=%d--dwEnable=%d--------------\n",nCh,pcfg->dwEnable);

#endif
    pcfg->nSpeedValid = 1;
	pcfg->nSpeed = MPU_PTZ_GetSpeed(nCh);
//  MPU_DBG("nCh = %d, nDecoderAddress = %d\n", nCh, pcfg->nDecoderAddress);

	return nRet;
}


//设置云台配置串口信息
static int SetPtzSerialConfig(int nCh, ZW_CAMERA_PTZ_CFG * pcfg)
{
	int nRet = IDVR_OK, nSDKRet;

	//! 本结构体是否有效，=0表示无效
	if (0 == pcfg->bValid)
	{
		MPU_DBG("0 == pcfg->bValid, nCh = %d\n", nCh);
		return IDVR_CIU_STRUCT_UNVALID;
	}
	//当前除通道0外其他通道的PTZ只做SPeed设置
	if(nCh == 0)
	{
		/* 波特率(bps)，0－50，1－75，2－110，3－150，4－300，5－600，6－1200，7－2400，8－4800，9－9600，10－19200， 11－38400，12－57600，13－76800，14－115.2k; */
		//nSDKRet = MPU_PTZ_SetBaudrate(pcfg->nBaudRate, nCh);
		nSDKRet = MPU_PTZ_SetBaudrate(pcfg->dwBandrate, nCh);
		MPU_CHK(0 == nSDKRet);

		/*解码器地址:0 - 255 */
		//nSDKRet = MPU_PTZ_SetAddress(pcfg->nDecoderAddress, nCh);
		nSDKRet = MPU_PTZ_SetAddress(pcfg->dwAddress, nCh);
		MPU_CHK(0 == nSDKRet);

		/*云台复位周期 */
		nSDKRet = MPU_PTZ_SetResetCycle(pcfg->dwWatchDelayBySecond, nCh);
		MPU_CHK(0 == nSDKRet);

		/*开启云台守望功能*/
		nSDKRet = MPU_PTZ_EnableKeeper(pcfg->bWatch, nCh);
		MPU_CHK(0 == nSDKRet);


		/*云台 动作接口*/
		nSDKRet = MPU_PTZ_SetDefaultAction((pcfg->dwPTZActionType + ACTION_PTZ_PRESET), pcfg->dwPTZActionID, nCh);
		MPU_CHK(0 == nSDKRet);
	}
#if 1
	/*开启云台*/
	nSDKRet = MPU_PTZ_Enable(pcfg->dwEnable, nCh);
	MPU_CHK(0 == nSDKRet);
#endif
    if(pcfg->nSpeedValid)
	{
		nSDKRet = MPU_PTZ_SetSpeed(pcfg->nSpeed, nCh);
		MPU_CHK(0 == nSDKRet);
	}
//  MPU_DBG("nCh %d, BaudRate %d\n", nCh, pcfg->nBaudRate);
//  MPU_DBG("nCh %d, nDecoderAddress %d\n", nCh, pcfg->nDecoderAddress);
	return nRet;
}
#if 0
static int GetPtzChannelAction(int nCh, LPHY_DVR_PTZACTION pcfg)
{
	int nRet = IDVR_OK, nSDKRet;

	//! 本结构体是否有效，=0表示无效
	pcfg->bValid = 1;

	// 是否启用
	pcfg->nUseable = MPU_PTZ_IsEnableKeeper(nCh);
	// 守望周期
	pcfg->nCycle = MPU_PTZ_GetResetCycle(nCh);
	// 守望动作
	unsigned long dwAction = 0;

	unsigned long dwIndex = 0;

	nSDKRet = MPU_PTZ_GetDefaultAction(&dwAction, &dwIndex, nCh);
	if (ACTION_PTZ_PRESET > dwAction)
	{
		pcfg->nAction = 0;
		pcfg->nIndex = 0;
	}
	else
	{
		pcfg->nAction = (int) dwAction - ACTION_PTZ_PRESET;
		pcfg->nIndex = (int) dwIndex;
	}
	MPU_DBG("Get PTZ Guard Action, nUseable=%d, Cycle=%d, Action=%d, Index=%d\n", pcfg->nUseable, pcfg->nCycle, pcfg->nAction, pcfg->nIndex);
	MPU_CHK(0 == nSDKRet);

	return nRet;
}

static int SetPtzChannelAction(int nCh, LPHY_DVR_PTZACTION pcfg)
{
	int nRet = IDVR_OK, nSDKRet;

	if (!pcfg->bValid)
	{
		return IDVR_CIU_STRUCT_UNVALID;
	}

	// 启用
	MPU_PTZ_EnableKeeper(pcfg->nUseable, nCh);
	// 设置周期
	MPU_PTZ_SetResetCycle(pcfg->nCycle, nCh);
	// 设置守望动作
	nSDKRet = MPU_PTZ_SetDefaultAction(ACTION_PTZ_PRESET + pcfg->nAction, pcfg->nIndex, nCh);
	MPU_DBG("Set PTZ Guard Action, nUseable=%d, Cycle=%d, Action=%d, Index=%d\n", pcfg->nUseable, pcfg->nCycle, pcfg->nAction, pcfg->nIndex);

	return nRet;
}
#endif

static int GetPtzChannelReplenish(int nCh, LPHY_DVR_PTZ_REPLENISH_CONTENT_1 pcfg)
{
	int nRet = IDVR_OK;
	
	pcfg->bValid = 1;
	pcfg->iLowPowerConsumption = MPU_PTZ_GetEletronicProp(eLowPowerConsumption,nCh);
	pcfg->iScreenWiper = MPU_PTZ_GetEletronicProp(eScreenWiper,nCh);
	pcfg->iNightVision = MPU_PTZ_GetEletronicProp(eNightVision,nCh);
	pcfg->iDemist = MPU_PTZ_GetEletronicProp(eDemist,nCh);
	MPU_DBG("GetPtzReplenish\n");
	return nRet;	
}

static int SetPtzChannelReplenish(int nCh, LPHY_DVR_PTZ_REPLENISH_CONTENT_1 pcfg)
{
	int nRet = IDVR_OK;

	//! 本结构体是否有效，=0表示无效
	if (0 == pcfg->bValid)
	{
		MPU_DBG("0 == pcfg->bValid, nCh = %d\n", nCh);
		return IDVR_CIU_STRUCT_UNVALID;
	}

	if (pcfg->bValid)
	{

		PTZ_SetEletronicProp(pcfg->iLowPowerConsumption,eLowPowerConsumption,nCh);
		PTZ_SetEletronicProp(pcfg->iScreenWiper,eScreenWiper,nCh);
		PTZ_SetEletronicProp(pcfg->iNightVision,eNightVision,nCh);
		PTZ_SetEletronicProp(pcfg->iDemist,eDemist,nCh);
	}
	else
	{
	}

	return nRet;	
}

static int GetPtzReplenish(LPHY_DVR_PTZ_REPLENISH_1 pcfg)
{
	int nRet = IDVR_OK, nCh, nCount;

	//! 本结构体是否有效，=0表示无效
	pcfg->bValid = 1;

	//获取视频通道数
	nCount = MAX_ENC_CHANNEL_NUM;

	for (nCh = 0; nCh < nCount; ++nCh)
	{
		GetPtzChannelReplenish(nCh, &pcfg->struContent[nCh]);
	}
	
	return nRet;

}

static int SetPtzReplenish(LPHY_DVR_PTZ_REPLENISH_1 pcfg)
{
	int nRet = IDVR_OK, nCh, nCount;

	//! 本结构体是否有效，=0表示无效
	if (0 == pcfg->bValid)
	{
		MPU_DBG("0 == pcfg->bValid\n");
		return IDVR_CIU_STRUCT_UNVALID;
	}

	//获取视频通道数
	nCount = MAX_ENC_CHANNEL_NUM;

	for (nCh = 0; nCh < nCount; ++nCh)
	{
		SetPtzChannelReplenish(nCh, &pcfg->struContent[nCh]);
	}

	return nRet;

}

static int MPU_PTZ_GetPtzPresetListConfig(int nCh, ZW_CAMERA_PRESET_LIST *pcfg)
{
	int i;

	char defaultValue[32];
	char section[32];
	int nBoardID = 0;
	
	CConfig *cfg = CConfig::GetInstance();

	pcfg->bValid = 1;

	ZWSys_GetBoardInfo(&nBoardID);

	for(i=0;i<256;i++)
	{
		memset(section, 0, sizeof(section));
		snprintf(section, sizeof(section), "PPL_%d", i);//裁剪配置，这里太占大小，把PTZ_PRESET_LIST缩写为PPL

		//T4P T4S默认支持全部预置点，不需要先设置再获取
		if(0x2110 == nBoardID || 0x211e == nBoardID || 0x2115 == nBoardID || 0x212D == nBoardID)
		{
			pcfg->struPresetList[i].bValid = cfg->GetValue(section, "valid", 1);
		}
		else
		{
			pcfg->struPresetList[i].bValid = cfg->GetValue(section, "valid", 0);
		}
		
		pcfg->struPresetList[i].nPresetIndex = cfg->GetValue(section, "Index", i);
		memset(defaultValue, 0, sizeof(defaultValue));
		memset(&pcfg->struPresetList[i].strPresetToken, 0, 32);
		snprintf(defaultValue, sizeof(defaultValue), "%d", i+1);
		cfg->GetValue(section, "Token", defaultValue, pcfg->struPresetList[i].strPresetToken, 32);
		cfg->GetValue(section, "Name", defaultValue, pcfg->struPresetList[i].strPresetName, 32);
	}

	return 0;
}

static int MPU_PTZ_SetPtzPresetListConfig(int nCh, ZW_CAMERA_PRESET_LIST *pcfg)
{
	int i;

	int nRet = IDVR_OK;
	char section[32];
	
	CConfig *cfg = CConfig::GetInstance();

	//! 本结构体是否有效，=0表示无效
	if (0 == pcfg->bValid)
	{
		MPU_DBG("0 == pcfg->bValid, nCh = %d\n", nCh);
		return IDVR_CIU_STRUCT_UNVALID;
	}

	for(i=0;i<256;i++)
	{
		memset(section, 0, sizeof(section));
		snprintf(section, sizeof(section), "PPL_%d", i);
		cfg->SetValue(section, "valid", pcfg->struPresetList[i].bValid);
		cfg->SetValue(section, "Index", pcfg->struPresetList[i].nPresetIndex);
		cfg->SetValue(section, "Token", pcfg->struPresetList[i].strPresetToken);
		cfg->SetValue(section, "Name", pcfg->struPresetList[i].strPresetName);
	}
		
	return nRet;
}

static int GetPTZPresetList(ZW_DVR_PTZ_PRESET_LIST * pcfg)
{
	int nRet = IDVR_OK, nCh, nCount;

	//! 本结构体是否有效，=0表示无效
	pcfg->bValid = 1;

	//获取视频通道数
	nCount = 1;

	for (nCh = 0; nCh < nCount; ++nCh)
	{
		MPU_PTZ_GetPtzPresetListConfig(nCh, &pcfg->struPTZPresetList[nCh]);
	}
	
	return nRet;
}

static int SetPTZPresetList(ZW_DVR_PTZ_PRESET_LIST * pcfg)
{
	int nRet = IDVR_OK, nCh = 0, nCount;
	
	//! 本结构体是否有效，=0表示无效
	if (0 == pcfg->bValid)
	{
		MPU_DBG("0 == pcfg->bValid, nCh = %d\n", nCh);
		return IDVR_CIU_STRUCT_UNVALID;
	}

	//获取视频通道数
	nCount = 1;

	for (nCh = 0; nCh < nCount; ++nCh)
	{
		MPU_PTZ_SetPtzPresetListConfig(nCh, &pcfg->struPTZPresetList[nCh]);
	}
	
	return nRet;
}


//获取云台配置通道信息，需外部将pcfg清零
static int GetPtzChannelConfig(int nCh, LPHY_DVR_DECODER_CHANNEL pcfg)
{
	int nRet = IDVR_OK, nSDKRet;

	//! 本结构体是否有效，=0表示无效
	pcfg->bValid = 1;


	/*PTZ协议名称 */
	nSDKRet = MPU_PTZ_GetProtocol(pcfg->szPTZName, sizeof (pcfg->szPTZName), nCh);
	MPU_CHK(0 == nSDKRet);

	//串口配置
//	GetPtzSerialConfig(nCh, &pcfg->struSerial);
	GetPtzSerialConfig(nCh, &pcfg->struPTZcfg);

	return nRet;
}

//设置云台配置通道信息
static int SetPtzChannelConfig(int nCh, LPHY_DVR_DECODER_CHANNEL pcfg)
{
	int nRet = IDVR_OK, nSDKRet;

	//! 本结构体是否有效，=0表示无效
	if (0 == pcfg->bValid)
	{
		MPU_DBG("0 == pcfg->bValid, nCh = %d\n", nCh);
		return IDVR_CIU_STRUCT_UNVALID;
	}

	/*PTZ协议名称 */
	nSDKRet = MPU_PTZ_SetProtocol(pcfg->szPTZName, nCh);
	MPU_CHK(0 == nSDKRet);

	MPU_DBG("--nCh=%d--szPTZName=%s--dwEnable=%d--dwAddress=%d--dwBandrate=%d--dwProtocol_index=%d--" \
		"bWatch=%d--dwWatchDelayBySecond=%d--dwPTZActionType=%d--dwPTZActionID=%d--\n", 
		nCh, 
		pcfg->szPTZName,
		pcfg->struPTZcfg.dwEnable,
		pcfg->struPTZcfg.dwAddress,
		pcfg->struPTZcfg.dwBandrate,
		pcfg->struPTZcfg.dwProtocol_index,
		pcfg->struPTZcfg.bWatch,
		pcfg->struPTZcfg.dwWatchDelayBySecond,
		pcfg->struPTZcfg.dwPTZActionType,
		pcfg->struPTZcfg.dwPTZActionID);

	//串口配置
//	SetPtzSerialConfig(nCh, &pcfg->struSerial);
	SetPtzSerialConfig(nCh, &pcfg->struPTZcfg);

	return nRet;
}

//获取电子PTZ参数配置信息需外部将pcfg清零
static int GetPtzEletronicChConfig(int nCh, LPHY_DVR_PTZ_ELCTRONIC_CHANNEL pcfg)
{
	int nRet = IDVR_OK, value;
	pcfg->bValid = 1;
	pcfg->struPtzElcSony.bValid = 1;
	pcfg->struPtzElcSony.nDigitZoom = MPU_PTZ_GetEletronicProp(ePropDigitZoom, nCh);
	pcfg->struPtzElcSony.nLowLum = MPU_PTZ_GetEletronicProp(ePropLowLum, nCh);
	pcfg->struPtzElcSony.nBackLight = MPU_PTZ_GetEletronicProp(ePropBackLight, nCh);
	pcfg->struPtzElcSony.nAutoShutter = MPU_PTZ_GetEletronicProp(ePropAutoShutter, nCh);
	pcfg->struPtzElcSony.nWhiteBalance = MPU_PTZ_GetEletronicProp(ePropWhiteBalance, nCh);
	pcfg->struPtzElcSony.nRedGain = MPU_PTZ_GetEletronicProp(ePropRedGain, nCh);
	pcfg->struPtzElcSony.nBlueGain = MPU_PTZ_GetEletronicProp(ePropBlueGain, nCh);
	pcfg->struPtzElcSony.nAcutance = MPU_PTZ_GetEletronicProp(ePropAcutance, nCh);
	pcfg->struPtzElcSony.nExpMode = MPU_PTZ_GetEletronicProp(ePropExpMode, nCh);
	pcfg->struPtzElcSony.nShutterSpeed = MPU_PTZ_GetEletronicProp(ePropShutterSpeed, nCh);
	pcfg->struPtzElcSony.nIrisSpeed = MPU_PTZ_GetEletronicProp(ePropIrisSpeed, nCh);
	pcfg->struPtzElcSony.nWideDyn = MPU_PTZ_GetEletronicProp(ePropWideDyn, nCh);
	pcfg->struPtzElcSony.n3DFilter = MPU_PTZ_GetEletronicProp(eProp3DFilter, nCh);
	pcfg->struPtzElcSony.nMirror = MPU_PTZ_GetEletronicProp(ePropMirror, nCh);
	pcfg->struPtzElcSony.nReverse = MPU_PTZ_GetEletronicProp(ePropReverse, nCh);
	pcfg->struPtzElcSony.nLongRestrict= MPU_PTZ_GetEletronicProp(eProplimitS, nCh);
	pcfg->struPtzElcSony.nNorthSet = 0;
	value = MPU_PTZ_GetEletronicProp(ePropHDVideoMode, nCh);
	pcfg->struPtzElcSony.nInputFrequency = value ;

	return nRet;
}

//设置电子PTZ参数配置信息
static int SetPtzEletronicChConfig(int nCh, LPHY_DVR_PTZ_ELCTRONIC_CHANNEL pcfg)
{
	int nRet = IDVR_OK;

	//! 本结构体是否有效，=0表示无效
	if (0 == pcfg->bValid)
	{
		MPU_DBG("0 == pcfg->bValid, nCh = %d\n", nCh);
		return IDVR_CIU_STRUCT_UNVALID;
	}

	if (pcfg->struPtzElcSony.bValid)
	{
		PTZ_SetEletronicProp(pcfg->struPtzElcSony.nDigitZoom, ePropDigitZoom, nCh);
		PTZ_SetEletronicProp(pcfg->struPtzElcSony.nLowLum, ePropLowLum, nCh);
		PTZ_SetEletronicProp(pcfg->struPtzElcSony.nBackLight, ePropBackLight, nCh);
		PTZ_SetEletronicProp(pcfg->struPtzElcSony.nAutoShutter, ePropAutoShutter, nCh);
		PTZ_SetEletronicProp(pcfg->struPtzElcSony.nWhiteBalance, ePropWhiteBalance, nCh);
		PTZ_SetEletronicProp(pcfg->struPtzElcSony.nRedGain, ePropRedGain, nCh);
		PTZ_SetEletronicProp(pcfg->struPtzElcSony.nBlueGain, ePropBlueGain, nCh);
		PTZ_SetEletronicProp(pcfg->struPtzElcSony.nAcutance, ePropAcutance, nCh);
		PTZ_SetEletronicProp(pcfg->struPtzElcSony.nExpMode, ePropExpMode, nCh);
		PTZ_SetEletronicProp(pcfg->struPtzElcSony.nShutterSpeed, ePropShutterSpeed, nCh);
		PTZ_SetEletronicProp(pcfg->struPtzElcSony.nIrisSpeed, ePropIrisSpeed, nCh);
		PTZ_SetEletronicProp(pcfg->struPtzElcSony.nWideDyn, ePropWideDyn, nCh);
		PTZ_SetEletronicProp(pcfg->struPtzElcSony.n3DFilter, eProp3DFilter, nCh);
		PTZ_SetEletronicProp(pcfg->struPtzElcSony.nMirror, ePropMirror, nCh);
		PTZ_SetEletronicProp(pcfg->struPtzElcSony.nReverse, ePropReverse, nCh);
		PTZ_SetEletronicProp(pcfg->struPtzElcSony.nLongRestrict, eProplimitS, nCh);
		
		if (pcfg->struPtzElcSony.nNorthSet)
		{
			MPU_PTZ_SetEletronicProp(0, ePropnorth, nCh);
		}
		
	//	MP4Sys_SetVideoFrequency((VideoFrequencyType_t) !pcfg->struPtzElcSony.nInputFrequency );
		PTZ_SetEletronicProp(pcfg->struPtzElcSony.nInputFrequency, ePropHDVideoMode, nCh);
	}
	else
	{
	}

	return nRet;
}


//获取电子PTZ参数配置信息需外部将pcfg清零
static int GetPtzEletronicChConfig(int nCh, LPHY_DVR_PTZ_ELCTRONIC_CH_TEMPERATURE pcfg)
{
	CConfig *cfg = CConfig::GetInstance();
	
	int nRet = IDVR_OK;
	
	pcfg->bValid = 1;
	pcfg->struPtzFanDevice.bValid = 1;
	pcfg->struPtzFanDevice.iWorkMode = cfg->GetValue("PtzEletronicChConfig", "fanworkmode", 0);
	pcfg->struPtzFanDevice.iManualSwitch = cfg->GetValue("PtzEletronicChConfig", "faniManualSwitch", 0);
	pcfg->struPtzFanDevice.iOpenThreshold = cfg->GetValue("PtzEletronicChConfig", "faniOpenThreshold", 35);
	pcfg->struPtzFanDevice.iCloseThreshold = cfg->GetValue("PtzEletronicChConfig", "faniCloseThreshold", 30);
	
	pcfg->struPtzHeaterDevice.bValid = 1;
	pcfg->struPtzHeaterDevice.iWorkMode = cfg->GetValue("PtzEletronicChConfig", "Heaterworkmode", 0);
	pcfg->struPtzHeaterDevice.iManualSwitch = cfg->GetValue("PtzEletronicChConfig", "HeateriManualSwitch", 0);
	pcfg->struPtzHeaterDevice.iOpenThreshold = cfg->GetValue("PtzEletronicChConfig", "HeateriOpenThreshold", 0);
	pcfg->struPtzHeaterDevice.iCloseThreshold = cfg->GetValue("PtzEletronicChConfig", "HeateriCloseThreshold", 10);

	return nRet;
}

//设置电子PTZ参数配置信息
static int SetPtzEletronicChConfig(int nCh, LPHY_DVR_PTZ_ELCTRONIC_CH_TEMPERATURE pcfg)
{
	int nRet = IDVR_OK;
	CConfig *cfg = CConfig::GetInstance();

	//! 本结构体是否有效，=0表示无效
	if (0 == pcfg->bValid)
	{
		MPU_DBG("0 == pcfg->bValid, nCh = %d\n", nCh);
		return IDVR_CIU_STRUCT_UNVALID;
	}

	if (pcfg->bValid)
	{
		if (pcfg->struPtzFanDevice.bValid)
		{
			cfg->SetValue("PtzEletronicChConfig", "fanworkmode", pcfg->struPtzFanDevice.iWorkMode);
			cfg->SetValue("PtzEletronicChConfig", "faniManualSwitch", pcfg->struPtzFanDevice.iManualSwitch);
			cfg->SetValue("PtzEletronicChConfig", "faniOpenThreshold", pcfg->struPtzFanDevice.iOpenThreshold);
			cfg->SetValue("PtzEletronicChConfig", "faniCloseThreshold", pcfg->struPtzFanDevice.iCloseThreshold);
		}
		
		if (pcfg->struPtzHeaterDevice.bValid)
		{
			cfg->SetValue("PtzEletronicChConfig", "Heaterworkmode", pcfg->struPtzHeaterDevice.iWorkMode);
			cfg->SetValue("PtzEletronicChConfig", "HeateriManualSwitch", pcfg->struPtzHeaterDevice.iManualSwitch);
			cfg->SetValue("PtzEletronicChConfig", "HeateriOpenThreshold", pcfg->struPtzHeaterDevice.iOpenThreshold);
			cfg->SetValue("PtzEletronicChConfig", "HeateriCloseThreshold", pcfg->struPtzHeaterDevice.iCloseThreshold);
		}
	}
	else
	{
	}

	return nRet;
}

static HY_DVR_PTZ_ELCTRONIC m_Dvr_Ptz_Elctronic ;

int Init_PtzEletronic()
{
	int ret = 0;
/*
	BoardSensorType_t SensorType=BOARD_SENSOR_NORM5150;
	//MP4Sys_GetSensorType(&SensorType);
	if (SensorType <= BOARD_SENSOR_MT9D131||SensorType>=BOARD_SENSOR_EFFIOE_E)
	{
		return -1;
	}
	
	HY_DVR_PTZ_ELCTRONIC_CHANNEL info;
	ret = GetPtzEletronicChConfig( 0, &info);
	if ( 0 == ret )
	{
		ret = SetPtzEletronicChConfig( 0, &info);
	}
	memcpy(&m_Dvr_Ptz_Elctronic,&info,sizeof(HY_DVR_PTZ_ELCTRONIC_CHANNEL));
	m_PtzEletronicChConfig_bInit = 1;
	*/
	return ret;
}


int Init_Replenish()
{
	int ret = 0;
/*
	BoardSensorType_t SensorType;
	//MP4Sys_GetSensorType(&SensorType);
	SensorType=BOARD_SENSOR_NORM5150;
	if (SensorType <= BOARD_SENSOR_MT9D131||SensorType>=BOARD_SENSOR_EFFIOE_E)
	{
		return -1;
	}
	
	HY_DVR_PTZ_REPLENISH_CONTENT_1 info;
	ret = GetPtzChannelReplenish( 0, &info);
	if ( 0 == ret )
	{
		ret = SetPtzChannelReplenish( 0, &info);
	}
*/
	return ret;	
}

//获取云台配置信息，需外部将pcfg清零
LPHY_DVR_PTZ_ELCTRONIC GetPtzEletronicConfig()
{
	return &m_Dvr_Ptz_Elctronic;
}

//获取云台配置信息，需外部将pcfg清零
static int GetPtzEletronicConfig(LPHY_DVR_PTZ_ELCTRONIC pcfg)
{
	int nRet = IDVR_OK, nCh, nCount;

	//! 本结构体是否有效，=0表示无效
	pcfg->bValid = 1;

	//获取视频通道数
	nCount = MAX_ENC_CHANNEL_NUM;

	for (nCh = 0; nCh < nCount; ++nCh)
	{
		//获取各通道配置信息
		GetPtzEletronicChConfig(nCh, &pcfg->struPtzElcChannel[nCh]);
		GetPtzEletronicChConfig(nCh, &pcfg->struPtzElcChTemperature[nCh]);
	}

	return nRet;
}

/*设置云台配置信息*/
static int SetPtzEletronicConfig(LPHY_DVR_PTZ_ELCTRONIC pcfg)
{
	int nRet = IDVR_OK, nCh, nCount;

	//! 本结构体是否有效，=0表示无效
	if (0 == pcfg->bValid)
	{
		MPU_DBG("0 == pcfg->bValid\n");
		return IDVR_CIU_STRUCT_UNVALID;
	}

	//获取视频通道数
	nCount = MAX_ENC_CHANNEL_NUM;

	for (nCh = 0; nCh < nCount; ++nCh)
	{
		//获取各通道配置信息
		nRet = SetPtzEletronicChConfig(nCh, &pcfg->struPtzElcChannel[nCh]);
		nRet = SetPtzEletronicChConfig(nCh, &pcfg->struPtzElcChTemperature[nCh]);
	}
	
	memcpy(&m_Dvr_Ptz_Elctronic,pcfg,sizeof(HY_DVR_PTZ_ELCTRONIC));

	return nRet;
}

#if 0
static int ResetPtzEletronicConfig(LPHY_DVR_PTZ_ELCTRONIC pcfg)
{
	int nRet = IDVR_OK, nCh, nCount;

	//! 本结构体是否有效，=0表示无效
	if (0 == pcfg->bValid)
	{
		MPU_DBG("0 == pcfg->bValid\n");
		return IDVR_CIU_STRUCT_UNVALID;
	}

	//获取视频通道数
	nCount = MAX_ENC_CHANNEL_NUM;

	for (nCh = 0; nCh < nCount; ++nCh)
	{
		if (pcfg->struPtzElcChannel[nCh].bValid)
		{
			MPU_PTZ_ResetAllEletronicProp(nCh);
		}
	}

	return nRet;
}
#endif

//获取云台配置信息，需外部将pcfg清零
static int GetPtzConfig(LPHY_DVR_DECODER_CFG pcfg)
{
	int nRet = IDVR_OK, nCh, nCount;

	//! 本结构体是否有效，=0表示无效
	pcfg->bValid = 1;

	//获取视频通道数
	nCount = MAX_PTZ_CHANNEL_NUM;

	for (nCh = 0; nCh < nCount; ++nCh)
	{
		//获取各通道配置信息
		GetPtzChannelConfig(nCh, &pcfg->struDecoder[nCh]);
	}

	return nRet;
}

/*设置云台配置信息*/
static int SetPtzConfig(LPHY_DVR_DECODER_CFG pcfg)
{
	int nRet = IDVR_OK, nCh, nCount;
	//! 本结构体是否有效，=0表示无效
	if (0 == pcfg->bValid)
	{
		MPU_DBG("0 == pcfg->bValid\n");
		return IDVR_CIU_STRUCT_UNVALID;
	}

	//获取视频通道数
	nCount = MAX_PTZ_CHANNEL_NUM;
	if (nCount > DVR_MAX_PTZ_NUM)
	{
		MPU_ERR("remote config ptz %d, real %d\n", DVR_MAX_PTZ_NUM, nCount);
		nCount = DVR_MAX_PTZ_NUM;
	}
	for (nCh = 0; nCh < nCount; ++nCh)
	{	
		//设置对应通道配置信息
		SetPtzChannelConfig(nCh, &pcfg->struDecoder[nCh]);
#if 0
		// 守望动作配置
		SetPtzChannelAction(nCh, &pcfg->struPtzAction.struPTZAction[nCh]);
		SetPtzChannelReplenish(nCh, &pcfg->struReplenish1.struContent[nCh]);
#endif
	}
#if 0
	SetPtzEletronicConfig(&pcfg->struPtzElc);
#endif
	return nRet;
}

//获取云台协议信息，需外部将pcfg清零
static int GetPtzProtocol(LPHY_DVR_PTZPRO_CFG pcfg)
{
	int nRet = IDVR_OK, i, j, nCount;

	char szProtocolList[DVR_MAX_PTZPRO_NUM][256];

	int bExternal[DVR_MAX_PTZPRO_NUM];


	//! 本结构体是否有效，=0表示无效
	pcfg->bValid = 1;


	//获取可用的云台协议
	nCount = MPU_PTZ_GetProtocolList(szProtocolList, DVR_MAX_PTZPRO_NUM, TRUE, bExternal);

	//填写可用的协议信息
	if (nCount > 0)
	{
		for (i = 0; i < nCount; ++i)
		{
			LPHY_DVR_PTZPRO pPro = &pcfg->struPTZProtocol[i];

			//! 本结构体是否有效，=0表示无效
			pPro->bValid = 1;
#if 0
			/*PTZ协议的状态（0：未安装，1：已安装） */
			pPro->nUseable = 1;

			/*PTZ协议的状态（0：非系统固有的，可删除，1：系统固有的，不能删除） */
			pPro->bSystem = !bExternal[i];
#endif
			/*PTZ协议名称 */
			strncpy(pPro->szPTZName, szProtocolList[i], sizeof(pPro->szPTZName) - 1);
		}

		pcfg->nPtzCount = nCount;
	}

	if (DVR_MAX_PTZPRO_NUM == pcfg->nPtzCount)
	{
		return nRet;
	}


	//获取不可用的云台协议
	nCount = MPU_PTZ_GetProtocolList(szProtocolList, DVR_MAX_PTZPRO_NUM, FALSE, bExternal);

	//填写不可用的协议信息
	if (nCount > 0)
	{
		if ((nCount + pcfg->nPtzCount) > DVR_MAX_PTZPRO_NUM)
		{
			nCount = DVR_MAX_PTZPRO_NUM - pcfg->nPtzCount;
		}

		for (i = 0, j = pcfg->nPtzCount; i < nCount; ++i, ++j)
		{
			LPHY_DVR_PTZPRO pPro = &pcfg->struPTZProtocol[j];

			//! 本结构体是否有效，=0表示无效
			pPro->bValid = 1;
#if 0
			/*PTZ协议的状态（0：未安装，1：已安装） */
			pPro->nUseable = 0;

			/*PTZ协议的状态（0：非系统固有的，可删除，1：系统固有的，不能删除） */
			pPro->bSystem = !bExternal[i];
#endif
			/*PTZ协议名称 */
			strncpy(pPro->szPTZName, szProtocolList[i], sizeof(pPro->szPTZName) - 1);
		}

		pcfg->nPtzCount += nCount;
	}

	return nRet;
}

//设置新版本看守位配置
static int SetWatchCarePreset(HY_DVR_WATCH_CARE_PRESET *ptWatchCfg)
{
    int nRet = IDVR_OK;

    if(NULL == ptWatchCfg)
    {
        MPU_ERR("input param is error\n");
        return IDVR_CIU_BUF_SMALL;
    }

    //! 本结构体是否有效，=0表示无效
	if (0 == ptWatchCfg->nValid)
	{
		MPU_ERR("0 == pcfg->bValid, nCh = %d\n", ptWatchCfg->nChannelID);
		return IDVR_CIU_STRUCT_UNVALID;
	}

    MPU_WARN("SetWatchCarePreset cType %d nPreset %d cWatchCareEnable %d\n", ptWatchCfg->cType, ptWatchCfg->nPreset, ptWatchCfg->cWatchCareEnable);
    if (0 == ptWatchCfg->cType)
    {
        //旧版本看守位设置
        MPU_PTZ_SetNormalCtrlPreset(ptWatchCfg->nChannelID, ptWatchCfg->nPreset);
    }
    else
    {
        //新版本看守位设置
        MPU_PTZ_SetWatchCareEnable(ptWatchCfg->nChannelID, ptWatchCfg->cWatchCareEnable);
    }

    return nRet;
}

static int GetWatchCarePreset(HY_DVR_WATCH_CARE_PRESET *ptWatchCfg)
{
    int nRet = IDVR_OK;    
    int iWatchCareType = 0;
    int iWatchCareEnable = 0;

    if(NULL == ptWatchCfg)
    {
        MPU_ERR("input param is error\n");
        return IDVR_CIU_BUF_SMALL;
    }

    ptWatchCfg->nValid = 1;
    ptWatchCfg->nChannelID = 0;

    MPU_PTZ_GetWatchCareType(ptWatchCfg->nChannelID,  &iWatchCareType);
    if (iWatchCareType == 0)
    {
        //旧版本看守位设置
        MPU_PTZ_GetNormalCtrlPreset(ptWatchCfg->nChannelID, &ptWatchCfg->nPreset);
    }
    else
    {    
        //新版本看守位设置
        MPU_PTZ_GetWatchCareEnable(ptWatchCfg->nChannelID, &iWatchCareEnable);
        ptWatchCfg->cWatchCareEnable = iWatchCareEnable;
        ptWatchCfg->cType = iWatchCareType;
    }

    MPU_WARN("GetWatchCarePreset cType %d nPreset %d cWatchCareEnable %d\n", ptWatchCfg->cType, ptWatchCfg->nPreset, ptWatchCfg->cWatchCareEnable);
    return nRet;
}

static int SetWatchCareAttribute(HY_DVR_WATCH_CARE_PRESET_ATTRIBUTE *ptAttrCfg)
{
    int nRet = IDVR_OK;

    if (NULL == ptAttrCfg)
    {
        MPU_ERR("input param is error\n");
        return IDVR_CIU_BUF_SMALL;
    }

    //! 本结构体是否有效，=0表示无效
	if (0 == ptAttrCfg->iValid)
	{
		MPU_ERR("0 == pcfg->bValid, nCh = %d\n", ptAttrCfg->iChannelID);
		return IDVR_CIU_STRUCT_UNVALID;
	}

    MPU_PTZ_SetWatchCareReTime(ptAttrCfg->iChannelID, ptAttrCfg->iReturnTime);
    return nRet;
}

static int GetWatchCareAttribute(HY_DVR_WATCH_CARE_PRESET_ATTRIBUTE *ptAttrCfg)
{
    int nRet = IDVR_OK;

    if(NULL == ptAttrCfg)
    {
        MPU_ERR("input param is error\n");
        return IDVR_CIU_BUF_SMALL;
    }

    ptAttrCfg->iValid = 1;
    ptAttrCfg->iChannelID = 0;

    MPU_PTZ_GetWatchCareReTime(ptAttrCfg->iChannelID, &ptAttrCfg->iReturnTime);
    return nRet;
}

/*设置云台协议信息*/
static int SetPtzProtocol(DWORD dwUserID, LPHY_DVR_PTZPRO_CFG pcfg)
{
	int nRet = IDVR_OK, i;

	bool bInstall = false;

	bool bUninstall = false;

	//! 本结构体是否有效，=0表示无效
	if (0 == pcfg->bValid)
	{
		MPU_DBG("0 == pcfg->bValid\n");
		return IDVR_CIU_STRUCT_UNVALID;
	}

	for (i = 0; i < pcfg->nPtzCount; ++i)
	{
		LPHY_DVR_PTZPRO pPro = &pcfg->struPTZProtocol[i];

		if (0 == pPro->bValid)
		{
			continue;
		}
#if 0
		if (pPro->nUseable)
		{
			nSDKRet = MPU_PTZ_InstallProtocol(pPro->szPTZName);
			if (0 == nSDKRet)
			{
				bInstall = true;
			}
		}
		else
		{
			nSDKRet = MPU_PTZ_UninstallProtocol(pPro->szPTZName, NULL);
			if (0 == nSDKRet)
			{
				bUninstall = true;
			}
		}
#endif
//		MPU_DBG("%d: %s, nUseable = %d, nSDKRet = %d\n", i, pPro->szPTZName, pPro->nUseable, nSDKRet);
	}

	if (bInstall)
	{
		//记录安装协议日志
		CIU_OPL_AddLog(dwUserID, EDVR_EVENT_SETTING, EDVR_EVENT_SETTING_DETAIL_IMPORT_PTZ, 0, 0);
	}

	if (bUninstall)
	{
		//记录卸载协议日志
		CIU_OPL_AddLog(dwUserID, EDVR_EVENT_SETTING, EDVR_EVENT_SETTING_DETAIL_EXPORT_PTZ, 0, 0);
	}

	return nRet;
}

int CIU_GetConfigure_Ptz(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen)
{
	int nRet = IDVR_OK;

	MPU_DBG("ID %d, Major %d, Minor %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType);

	switch (dwMinorType)
	{
			/*获取云台协议信息 */
		case PTZCFG_ALL:
			{
				LPHY_DVR_PTZPRO_CFG pcfg = (LPHY_DVR_PTZPRO_CFG) pBuffer;

				//判断buf空间是否足够
				if (*pdwBufLen < PTZPRO_CFG_LEN)
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", PTZPRO_CFG_LEN, *pdwBufLen);
					break;
				}

				memset(pcfg, 0, PTZPRO_CFG_LEN);
				*pdwBufLen = PTZPRO_CFG_LEN;

				nRet = GetPtzProtocol(pcfg);
			}
			break;
			
		case PRESET_LIST_CFG:
			{
				ZW_DVR_PTZ_PRESET_LIST * pcfg = (ZW_DVR_PTZ_PRESET_LIST *) pBuffer;
		
				//判断buf空间是否足够
				if (*pdwBufLen < sizeof(ZW_DVR_PTZ_PRESET_LIST))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(ZW_DVR_PTZ_PRESET_LIST), *pdwBufLen);
					break;
				}
		
				memset(pcfg, 0, sizeof (ZW_DVR_PTZ_PRESET_LIST));
				*pdwBufLen = sizeof (ZW_DVR_PTZ_PRESET_LIST);
		
				nRet = GetPTZPresetList(pcfg);
			}
			break;

		case PTZCFG_WATCH_CARE_PRESET:
    		{
    			HY_DVR_WATCH_CARE_PRESET *pcfg = (HY_DVR_WATCH_CARE_PRESET *) pBuffer;
    		
    			//判断buf空间是否足够
    			if (*pdwBufLen < sizeof(HY_DVR_WATCH_CARE_PRESET))
    			{
    				nRet = IDVR_CIU_BUF_SMALL;
    				MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_WATCH_CARE_PRESET), *pdwBufLen);
    				break;
    			}
    	
    			memset(pcfg, 0, sizeof (HY_DVR_WATCH_CARE_PRESET));
    			*pdwBufLen = sizeof (HY_DVR_WATCH_CARE_PRESET);
                nRet = GetWatchCarePreset(pcfg);
    		}
    		break;

		case WATCH_CARE_PRESET_ATTRIBUTE:
			{
				HY_DVR_WATCH_CARE_PRESET_ATTRIBUTE *pcfg = (HY_DVR_WATCH_CARE_PRESET_ATTRIBUTE *) pBuffer;
			
				//判断buf空间是否足够
				if (*pdwBufLen < sizeof(HY_DVR_WATCH_CARE_PRESET_ATTRIBUTE))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_WATCH_CARE_PRESET_ATTRIBUTE), *pdwBufLen);
					break;
				}
		
				memset(pcfg, 0, sizeof (HY_DVR_WATCH_CARE_PRESET_ATTRIBUTE));
				*pdwBufLen = sizeof (HY_DVR_WATCH_CARE_PRESET_ATTRIBUTE);
				nRet = GetWatchCareAttribute(pcfg);
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

int CIU_SetConfigure_Ptz(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, const BYTE * pBuffer, DWORD dwBufLen)
{
	int nRet = IDVR_OK;

	MPU_DBG("ID %d, Major %d, Minor %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType);

	switch (dwMinorType)
	{
			/*设置云台协议信息 */
		case PTZCFG_ALL:
			{
				///////////////////////////////////////////////////////////////////////////////
				//判断buf空间是否足够
				if (dwBufLen < PTZPRO_CFG_LEN)
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", PTZPRO_CFG_LEN, dwBufLen);
					break;
				}

				LPHY_DVR_PTZPRO_CFG pcfg = (LPHY_DVR_PTZPRO_CFG) pBuffer;

				nRet = SetPtzProtocol(dwUserID, pcfg);
			}
			break;
			
		case PRESET_LIST_CFG:
		{
			ZW_DVR_PTZ_PRESET_LIST *pcfg = (ZW_DVR_PTZ_PRESET_LIST *) pBuffer;
		
			//判断buf空间是否足够
			if (dwBufLen < sizeof(ZW_DVR_PTZ_PRESET_LIST))
			{
				nRet = IDVR_CIU_BUF_SMALL;
				MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(ZW_DVR_PTZ_PRESET_LIST), dwBufLen);
				break;
			}
		
			dwBufLen = sizeof(ZW_DVR_PTZ_PRESET_LIST);
		
			nRet = SetPTZPresetList(pcfg);
		}
		break;

		case PTZCFG_WATCH_CARE_PRESET:
		{
			HY_DVR_WATCH_CARE_PRESET *pcfg = (HY_DVR_WATCH_CARE_PRESET *) pBuffer;
		
			//判断buf空间是否足够
			if (dwBufLen < sizeof(HY_DVR_WATCH_CARE_PRESET))
			{
				nRet = IDVR_CIU_BUF_SMALL;
				MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_WATCH_CARE_PRESET), dwBufLen);
				break;
			}
		
			dwBufLen = sizeof(HY_DVR_WATCH_CARE_PRESET);
            nRet = SetWatchCarePreset(pcfg);
		}
		break;

    	case WATCH_CARE_PRESET_ATTRIBUTE:
		{
			HY_DVR_WATCH_CARE_PRESET_ATTRIBUTE *pcfg = (HY_DVR_WATCH_CARE_PRESET_ATTRIBUTE *) pBuffer;
		
			//判断buf空间是否足够
			if (dwBufLen < sizeof(HY_DVR_WATCH_CARE_PRESET_ATTRIBUTE))
			{
				nRet = IDVR_CIU_BUF_SMALL;
				MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_WATCH_CARE_PRESET_ATTRIBUTE), dwBufLen);
				break;
			}
		
			dwBufLen = sizeof(HY_DVR_WATCH_CARE_PRESET_ATTRIBUTE);
			nRet = SetWatchCareAttribute(pcfg);
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

//!\brief   远程配置，获取串口参数
int CIU_GetConfigure_Serial(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen)
{
	int nRet = IDVR_OK;

	MPU_DBG("ID %d, Major %d, Minor %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType);

	switch (dwMinorType)
	{
			/*获取云台配置信息 */
		case SERIALCFG_ALL:
			{
				LPHY_DVR_DECODER_CFG pcfg = (LPHY_DVR_DECODER_CFG) pBuffer;

				//判断buf空间是否足够
				if (*pdwBufLen < DECODER_CFG_LEN)
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", DECODER_CFG_LEN, *pdwBufLen);
					break;
				}

				memset(pcfg, 0, DECODER_CFG_LEN);
				*pdwBufLen = DECODER_CFG_LEN;

				nRet = GetPtzConfig(pcfg);
			}
			break;

		case SERIALCFG_PTZ_ELCTRONIC_CFG:
			{
				LPHY_DVR_PTZ_ELCTRONIC pcfg = (LPHY_DVR_PTZ_ELCTRONIC) pBuffer;

				//判断buf空间是否足够
				if (*pdwBufLen < DECODER_CFG_LEN)
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", DECODER_CFG_LEN, *pdwBufLen);
					break;
				}

				memset(pcfg, 0, sizeof (HY_DVR_PTZ_ELCTRONIC));
				*pdwBufLen = sizeof (HY_DVR_PTZ_ELCTRONIC);

				nRet = GetPtzEletronicConfig(pcfg);
			}
			break;
			
		case SERIALCFG_PTZ_REPLENISH_1_CFG:
			{
				LPHY_DVR_PTZ_REPLENISH_1 pcfg = (LPHY_DVR_PTZ_REPLENISH_1) pBuffer;

				//判断buf空间是否足够
				if (*pdwBufLen < DECODER_CFG_LEN)
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", DECODER_CFG_LEN, *pdwBufLen);
					break;
				}

				memset(pcfg, 0, sizeof (HY_DVR_PTZ_REPLENISH_1));
				*pdwBufLen = sizeof (HY_DVR_PTZ_REPLENISH_1);

				nRet = GetPtzReplenish(pcfg);
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

//!\brief   远程配置，设置串口参数
int CIU_SetConfigure_Serial(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, const BYTE * pBuffer, DWORD dwBufLen)
{
	int nRet = IDVR_OK;

	MPU_DBG("ID %d, Major %d, Minor %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType);

	switch (dwMinorType)
	{
			/*设置云台配置信息 */
		case SERIALCFG_ALL:
			{
				///////////////////////////////////////////////////////////////////////////////
				//判断buf空间是否足够
				if (dwBufLen < DECODER_CFG_LEN)
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", DECODER_CFG_LEN, dwBufLen);
					break;
				}

				LPHY_DVR_DECODER_CFG pcfg = (LPHY_DVR_DECODER_CFG) pBuffer;

				nRet = SetPtzConfig(pcfg);
			}
			break;
		case SERIALCFG_PTZ_ELCTRONIC_CFG:
			{
				///////////////////////////////////////////////////////////////////////////////
				//判断buf空间是否足够
				if (dwBufLen < sizeof (HY_DVR_PTZ_ELCTRONIC))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof (HY_DVR_PTZ_ELCTRONIC), dwBufLen);
					break;
				}

				LPHY_DVR_PTZ_ELCTRONIC pcfg = (LPHY_DVR_PTZ_ELCTRONIC) pBuffer;

				nRet = SetPtzEletronicConfig(pcfg);
			}
			break;
		case SERIALCFG_PTZ_REPLENISH_1_CFG:
		{
			LPHY_DVR_PTZ_REPLENISH_1 pcfg = (LPHY_DVR_PTZ_REPLENISH_1) pBuffer;

			//判断buf空间是否足够
			if (dwBufLen < sizeof (HY_DVR_PTZ_REPLENISH_1))
			{
				nRet = IDVR_CIU_BUF_SMALL;
				MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof (HY_DVR_PTZ_REPLENISH_1), dwBufLen);
				break;
			}

			dwBufLen = sizeof (HY_DVR_PTZ_REPLENISH_1);

			nRet = SetPtzReplenish(pcfg);
		}
			break;
#if 0
		case SERIALCFG_PTZ_ELCTRONIC_RESET:
			{
				///////////////////////////////////////////////////////////////////////////////
				//判断buf空间是否足够
				if (dwBufLen < sizeof (HY_DVR_PTZ_ELCTRONIC))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof (HY_DVR_PTZ_ELCTRONIC), dwBufLen);
					break;
				}

				LPHY_DVR_PTZ_ELCTRONIC pcfg = (LPHY_DVR_PTZ_ELCTRONIC) pBuffer;

				ResetPtzEletronicConfig(pcfg);
			}
			break;
#endif

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

static int GetDragDigitalMultiple(HY_DVR_DEVICE_DIGITAL_MULTIPLE *pcfg)
{
    int nRet = IDVR_OK;

    pcfg->bValid = 1;
#if defined(__DIGITAL_ZOOM_DRAG_SUPPORT__)

	//使用小数
	pcfg->iDeviceDigitalMultipleType = 1;
    
    nRet = MPU_ZOOM_GetRatio(&pcfg->dDeviceDigitalMultiple);
	pcfg->nDeviceDigitalMultiple = (int)pcfg->dDeviceDigitalMultiple;
	MPU_CHK(0 == nRet);
#endif

    return nRet;
}

static int SetDragDigitalMultiple(HY_DVR_DEVICE_DIGITAL_MULTIPLE *pcfg)
{
    int nRet = IDVR_OK;

    //! 本结构体是否有效，=0表示无效
    if ((0 == pcfg->bValid))
    {
        MPU_DBG("0 == pcfg->bValid\n");
        return IDVR_CIU_STRUCT_UNVALID;
    }

#if defined(__DIGITAL_ZOOM_DRAG_SUPPORT__)
	if (1 == pcfg->iDeviceDigitalMultipleType)
	{
		//使用小数
    	nRet = MPU_ZOOM_SetRatio(pcfg->dDeviceDigitalMultiple,DRAG_CLIENT_RADIO);
	}
	else
	{
		//使用整数
    	nRet = MPU_ZOOM_SetRatio((double)pcfg->nDeviceDigitalMultiple,DRAG_CLIENT_RADIO);
	}
	MPU_CHK(0 == nRet);
#endif

    return nRet;
}

int CIU_GetConfigure_DeviceDragDigitalMultiple(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen)
{
	int nRet = IDVR_OK;
	MPU_INFO("ID %d, Major %d, Minor %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType);

	switch (dwMinorType)
	{
		case DEVICE_DIGITAL_MULTIPLE:
			{
				///////////////////////////////////////////////////////////////////////////////
				//判断buf空间是否足够
				if (*pdwBufLen < sizeof(HY_DVR_DEVICE_DIGITAL_MULTIPLE))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_DEVICE_DIGITAL_MULTIPLE), *pdwBufLen);
					break;
				}

				HY_DVR_DEVICE_DIGITAL_MULTIPLE *pcfg = (HY_DVR_DEVICE_DIGITAL_MULTIPLE *) pBuffer;
				memset(pcfg, 0, sizeof(HY_DVR_DEVICE_DIGITAL_MULTIPLE));
				nRet = GetDragDigitalMultiple(pcfg);
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

int CIU_SetConfigure_DeviceDragDigitalMultiple(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, const BYTE * pBuffer, DWORD dwBufLen)
{
	int nRet = IDVR_OK;
	MPU_INFO("ID %d, Major %d, Minor %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType);

	switch (dwMinorType)
	{
		case DEVICE_DIGITAL_MULTIPLE:
			{
				///////////////////////////////////////////////////////////////////////////////
				//判断buf空间是否足够
				if (dwBufLen < sizeof(HY_DVR_DEVICE_DIGITAL_MULTIPLE))
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", sizeof(HY_DVR_DEVICE_DIGITAL_MULTIPLE), dwBufLen);
					break;
				}
				
				HY_DVR_DEVICE_DIGITAL_MULTIPLE *pcfg = (HY_DVR_DEVICE_DIGITAL_MULTIPLE *) pBuffer;
				nRet = SetDragDigitalMultiple(pcfg);
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
