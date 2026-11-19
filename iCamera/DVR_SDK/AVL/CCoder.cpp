// Copyright (C) 1991 - 1999 Rational Software Corporation

#include <string.h>
#include "CCoder.h"
#include "ddt_so.h"
#include <unistd.h>
#include "DeviceAbility.h"
#include "mysystem.h"
#include "MemRecord.h"
#include "PUB_BoradInfo.h"

#define MAX_CUSTOM_AUDIO_SIZE	(128 * 1024)

extern int bSupportRtmp;

#ifndef TRACE_CODER
#define TRACE_CODER 0?1:printf
#endif

int CCoder::m_AlarmType = 0;

ALARMCALLBACK CCoder::m_FxnCallBack = NULL;

VideoOutStandard_t CCoder::m_iStandard = StandardPAL;

//##ModelId=49A2637500AD
CCoder::CCoder(int nCh, void *hHandle)
{
	m_iChannel = nCh;
	m_AlarmType = 0;
	m_henc = hHandle;
	m_AuxAdvEncType = LtH264Video;
	m_HDAuxAdvEncType = LtH264Video;
    m_FourthStreamAdvEncType = LtMjpegVideo;
	m_CodecStreamType = 0;
	m_FxnCallBack = NULL;
	m_SensorFrequency = CMOS_FREQUENCY_50HZ;	
    m_nMainKeyFrameInterval = 0;
    m_nAuxKeyFrameInterval  = 0;
    m_nHDAuxKeyFrameInterval = 0;	
	m_nAudioInVol = 80;
	m_nAudioOutVol = 80;
}

CCoder::~CCoder()
{
	UnInit();
}

static int bitRate2quality(int bitrate)
{
	unsigned int quality = 3;

	quality = bitrate/1000 - 512;
	quality = (quality < 0 ? 0 : quality) > 5 ? 5 : quality;
	
	return quality;
}

//初始化
int CCoder::Init(int nMaxCh, int nMaxAlarmOut)
{
	int result;
	CODEC_PARA_S Aux_CodecParam;

	m_pActionAlarm = new CActionAlarm;
	if (m_pActionAlarm == NULL)
	{
		UnInit();
		AVL_ERR("[CCoder:%s]new CActionAlarm failed\n", __FUNCTION__);
		return -1;
	}
	m_pActionAlarm->Init(m_iChannel, CoderErrorCfgSection, "CoderErrorAction", nMaxCh, nMaxAlarmOut);
	pCConfig = CConfig::GetInstance();

	//MP4Sys_GetSystemStandard(&m_iStandard);
	if (0 != ZWSys_GetCVBSOutStandard(&m_iStandard))
	{
		UnInit();
		AVL_ERR("[CCoder:%s]Get_CVBSOutStandard failed\n", __FUNCTION__);
		return -1;
	}
	
	LoadParam();
	//设置参数
	
	memcpy(&Aux_CodecParam, &m_CodecParam, sizeof(CODEC_PARA_S));

	if(LtMjpegVideo == m_AuxAdvEncType)
	{
		Aux_CodecParam.PicQualityLever = bitRate2quality(Aux_CodecParam.BitRate);
	}

	//printf("[%s] ZWSys_EncSetCodecPara(Aux) picFormat=%d BitRateCtlMode=%d KeyFrameInterval=%d BitRate=%d FrameRate=%d\n", __FUNCTION__,
	//	  m_CodecParam.PictureFormat, 
	//	  m_CodecParam.BitRateCtlMode, m_CodecParam.KeyFrameInterval, m_CodecParam.BitRate, m_CodecParam.FrameRate);
	if(1 == bSupportRtmp)
	{
		if(Aux_CodecParam.FrameRate < 15)
		{
			Aux_CodecParam.FrameRate = 15;
		}
	}
	
	result = ZWSys_EncSetCodecPara(m_henc, 1, &Aux_CodecParam, m_AuxAdvEncType);
	if (result)
	{
		AVL_ERR("[CCoder:%s]call ZWSys_EncSetCodecPara(Aux) failed(%d)\n", __FUNCTION__, result);
		return result;
	}

	//使能
	AuxCoder_Enable(m_bAuxEnable);

#ifdef USE_HDAUX
	HDAuxCoder_SetEncCfg(&m_HDCodecParam);

	//使能高清子码流
	HDAuxCoder_Enable(m_bHDAuxEnable);
#endif

#ifdef USE_FOURTH_STREAM
    FourthStream_SetEncCfg(&m_FourthStreamParam);
#endif

	SetKeyFrameInterval(0, m_nMainKeyFrameInterval);
	
	SetKeyFrameInterval(1, m_nAuxKeyFrameInterval);

#ifdef USE_HDAUX
	SetKeyFrameInterval(2, m_nHDAuxKeyFrameInterval);
#endif

	return 0;
}

//释放
int CCoder::UnInit()
{
	if (m_pActionAlarm != NULL)
	{
		delete m_pActionAlarm;

		m_pActionAlarm = NULL;
	}
	return 0;
}

//!\brief 通知制式
//!\return 成功0,其它失败
int CCoder::SetStandard(int standard)
{
	m_iStandard = (VideoOutStandard_t) standard;
	return 0;
}

//!\brief 开启编码
//!\return 成功0,其它失败
int CCoder::StartAuxEncCapture()
{
	int result;
	BoardSensorType_t Sensortype;
	char szSectionComm[30];
	int ivalue_sensor;
	CODEC_PARA_S Aux_CodecParam;

	if (!m_bAuxEnable)
	{
		return 0;
	}

	ZWSys_GetSensorType(&Sensortype);
	snprintf(szSectionComm, sizeof(szSectionComm),"ISP_S%d",Sensortype);
	ivalue_sensor = pCConfig->GetValue(szSectionComm, "InputFrequency", 0);
	if (0 == ivalue_sensor)
		m_SensorFrequency = CMOS_FREQUENCY_50HZ;
	else{
		if (1 == ivalue_sensor)
			m_SensorFrequency = CMOS_FREQUENCY_60HZ;
		else
			m_SensorFrequency = CMOS_FREQUENCY_50HZ;
		}
	
	m_CodecParam.KeyFrameInterval = m_CodecParam.FrameRate;	//关键帧间隔等于帧率
	if (m_CodecParam.KeyFrameInterval == 0xFF)
	{
		if (m_SensorFrequency == CMOS_FREQUENCY_50HZ)
		{
			m_CodecParam.KeyFrameInterval = 25;
		}
		else{
			if (m_SensorFrequency == CMOS_FREQUENCY_60HZ)
					m_CodecParam.KeyFrameInterval = 30;
			else
				m_CodecParam.KeyFrameInterval = 25;
		}
		m_CodecParam.FrameRate=m_CodecParam.KeyFrameInterval;
	}
	
	memcpy(&Aux_CodecParam, &m_CodecParam, sizeof(CODEC_PARA_S));

	if(LtMjpegVideo == m_AuxAdvEncType)
	{
		Aux_CodecParam.PicQualityLever = bitRate2quality(Aux_CodecParam.BitRate);
	}

	TRACE_AVL("ZWSys_EncSetCodecPara(Aux) picFormat=%d BitRateCtlMode=%d KeyFrameInterval=%d BitRate=%d FrameRate=%d\n",
		  m_CodecParam.PictureFormat,
		  m_CodecParam.BitRateCtlMode, m_CodecParam.KeyFrameInterval, m_CodecParam.BitRate, m_CodecParam.FrameRate);
	if(1 == bSupportRtmp)
	{
		if(Aux_CodecParam.FrameRate < 15)
			Aux_CodecParam.FrameRate = 15;
	}
	result = ZWSys_EncSetCodecPara(m_henc, 1, &Aux_CodecParam, m_AuxAdvEncType);
	if (result)
	{
		AVL_ERR("[CCoder:%s]call ZWSys_EncSetCodecPara(Aux) failed(%d)\n", __FUNCTION__, result);
		return result;
	}

	result = ZWSys_EncStartAuxCodec(m_henc);
	if (result)
	{
		AVL_ERR("[CCoder:%d]ZWSys_EncStartAuxCodec failed (%d)", m_iChannel, result);
		return result;
	}

	return 0;
}

//!\brief 停止编码
//!\return 成功0,其它失败
int CCoder::StopAuxEncCapture()
{
	int result;

	if (m_bAuxEnable)
	{
		return 0;
	}
	result = ZWSys_EncStopAuxCodec(m_henc);
	if (result)
	{
		AVL_ERR("[CCoder:%d]ZWSys_EncStopAuxCodec failed (%d)", m_iChannel, result);
		return result;
	}
	return 0;
}

//##ModelId=49B6224401A7
int CCoder::EncCaptureIFrame(int nSensorIndex)
{
	return ZWSys_EncForceIFrame(m_henc, nSensorIndex);
}

//!\brief 启用某个报警联动动作
//!\praram int Action    !      ACTION_SCREEN 屏幕显示
//  !      ACTION_BEEP 声音提示
//  !      ACTION_ALARMOUT 报警输出
//  !      ACTION_SENDTOSERVER 上传报警服务器
//  !      ACTION_EMAIL 联动Email
//  !      ACTION_SMS 联动短信
//!\praram int bEnable false 为禁止联动 true 为开启联动
//对于联动屏幕显示0为不启用，1为联动主显,2为联动辅显,3为联动主显和辅显
//!\return  成功为0，失败为-1
//##ModelId=49BDEFDF0009
int CCoder::EnableAction(int Action, int bEnable)
{
	return m_pActionAlarm->EnableAction(Action, bEnable);
}

//!\brief 判断某个报警联动动作是否开启
//!\praram int Action    !      ACTION_SCREEN 屏幕显示
//  !      ACTION_BEEP 声音提示
//  !      ACTION_ALARMOUT 报警输出
//  !      ACTION_SENDTOSERVER 上传报警服务器
//  !      ACTION_EMAIL 联动Email
//  !      ACTION_SMS 联动短信
//!\return false 为禁止联动 true 为开启联动,
//对于联动屏幕显示0为不启用，1为联动主显,2为联动辅显,3为联动主显和辅显
//##ModelId=49BDEFDF0081
int CCoder::IsEnableAction(int Action)
{
	return m_pActionAlarm->IsEnableAction(Action);
}

//!\brief 获取某个报警联动录像参数
//!\param dwChannelFlag 联动的通道标记 
//每一位表示联动一个通道 1表示联动通道1 
//2表示联动通道2录像 3表示联动通道1和2录像,以此类推
//!\param nPort 某个报警通道号
//!\return 0表示获取成功，-1表示获取失败
//##ModelId=49B7187602BF
int CCoder::GetRecordParam(int *dwChannelFlag)
{
	return m_pActionAlarm->GetRecordParam(dwChannelFlag);
}

//!\brief 设置某个报警联动录像参数
//!\param dwChannelFlag 联动录像通道标记
//!\param nPort 某个报警通道号
//!\return 0表示设置成功 -1表示设置失败
//##ModelId=49B718640232
int CCoder::SetRecordParam(int dwChannelFlag)
{
	return m_pActionAlarm->SetRecordParam(dwChannelFlag);
}

//!\brief 
//获取某个报警联动拍照参数,拍照张数和间隔由CCapturePic::GetCfg获取
//!\param dwChannelFlag 存储联动拍照通道标记的缓冲区
//!\param nPort 报警联动输入通道号
//!\return 0表示成功 -1表示失败
//##ModelId=49B5CE9A030D
int CCoder::GetSnapshotParam(int *dwChannelFlag)
{
	return m_pActionAlarm->GetSnapshotParam(dwChannelFlag);
}

//!\brief 
//设置某个报警联动拍照参数，拍照张数和拍照间隔由CCapturePi
//c::SetCfg设定
//!\param dwChannelFlag 联动拍照通道标记
//!\param nPort 某个报警通道号
//!\return 0表示设置成功 -1表示失败
//##ModelId=49B5CDFE00BB
int CCoder::SetSnapshotParam(int dwChannelFlag)
{
	return m_pActionAlarm->SetSnapshotParam(dwChannelFlag);
}


//!\brief 获取某一某个报警通道联动某一云台的动作参数
//!\param nAction 联动动作 
//*nAction=0表示无动作，1联动预置点，2联动巡航，3联动轨迹
//!\param nIndex 联动预置点/巡航/轨迹的编号
//!\param nChannel 云台对应的通道号
//!\param nPort 某个报警通道号
//!\return 0表示获取成功, -1表示获取失败
//##ModelId=49B718B202DE

int CCoder::GetPtzParam(int nChannel, int *nAction, int *nIndex)
{
	return m_pActionAlarm->GetPtzParam(nChannel, nAction, nIndex);
}


//!\brief 设置某个报警通道联动某一个云台的动作
//!\param nAction 联动的动作 0表示无动作 1联动预置点 
//2联动巡航 3联动轨迹
//!\param nIndex 联动的预置点/巡航/轨迹的编号
//!\param nChannel 云台对应的通道号
//!\param nPort 某个报警通道号
//!\return 0表示成功 -1表示失败
//##ModelId=49B718BD0261
int CCoder::SetPtzParam(int nChannel, int nAction, int nIndex)
{
	return m_pActionAlarm->SetPtzParam(nChannel, nAction, nIndex);
}

//!\brief 获取某个报警联动报警输出的参数
//!\param dwOutPortFlag 存储输出通道标记的缓冲区
//!\param nPort 某个报警通道号
//!\return 0表示成功 -1表示失败
//##ModelId=49B718DD030D
int CCoder::GetDopParam(int *dwOutPortFlag)
{
	return m_pActionAlarm->GetDopParam(dwOutPortFlag);
}

//!\brief 设置某个报警联动报警输出的参数
//!\param dwOutPortFlag 设置输出通道标记
//!\param nPort 某个报警通道号
//!\return 0表示成功 -1表示失败
//##ModelId=49B718ED00AB
int CCoder::SetDopParam(int dwOutPortFlag)
{
	return m_pActionAlarm->SetDopParam(dwOutPortFlag);
}

//##ModelId=49BDEDDA03DB
int CCoder::AuxCoder_Enable(int bEnable)
{
	char key[100];

	int ret;
	m_bAuxEnable = bEnable;
	if (bEnable)
	{
		ret = ZWSys_EncStartAuxCodec(m_henc);
	}
	else
	{
		ret = ZWSys_EncStopAuxCodec(m_henc);
	}
	snprintf(key, sizeof(key), "AuxCoderEnable%02d", m_iChannel);
	pCConfig->SetValue(AuxCoderCfgSection, key, bEnable);

	if (ret != 0)
	{
		AVL_ERR("[CCoder:%s]EnableAuxCodec failed!ch=%d  enable= %d errorno=%d\n", __FUNCTION__, m_iChannel, bEnable, ret);
	}


	return (int) ret;
}

//##ModelId=49BDEDDB004D
int CCoder::AuxCoder_IsEnable()
{
	return m_bAuxEnable;

}

//##ModelId=49BDEDDB006B
int CCoder::AuxCoder_SetFrameRate(int nFrameRate)
{
	int result;

	char key[100];
	BoardSensorType_t Sensortype;
	char szSectionComm[30];
	int ivalue_sensor;
	CODEC_PARA_S Aux_CodecParam;

	TRACE_AVL("[CCoder:%s]SetFrameRate =%d\n", __FUNCTION__, nFrameRate);
	if (nFrameRate != 0xFF && (nFrameRate < 1 || nFrameRate > 30))
	{
		AVL_ERR("[CCoder:%s]nFrameRate=%d\n", __FUNCTION__, nFrameRate);
		return -1;
	}
	if(1 == bSupportRtmp)
		{
			if(nFrameRate < 15)
				{
					nFrameRate = 15;
					printf("ctrl 15fps\n");
				}
		}
	if (m_CodecParam.FrameRate == nFrameRate)
	{
		TRACE_AVL("[CCoder:%s]SetFrameRate no change\n", __FUNCTION__);
		return 0;
	}

	ZWSys_GetSensorType(&Sensortype);
	snprintf(szSectionComm, sizeof(szSectionComm),"ISP_S%d",Sensortype);
	ivalue_sensor = pCConfig->GetValue(szSectionComm, "InputFrequency", 0);
	if (0 == ivalue_sensor)
		m_SensorFrequency = CMOS_FREQUENCY_50HZ;
	else{
		if (1 == ivalue_sensor)
			m_SensorFrequency = CMOS_FREQUENCY_60HZ;
		else
			m_SensorFrequency = CMOS_FREQUENCY_50HZ;
		}
	
	m_CodecParam.FrameRate = nFrameRate;
	m_CodecParam.KeyFrameInterval = nFrameRate;
	if (m_CodecParam.KeyFrameInterval == 0xFF)
	{
		if (m_SensorFrequency == CMOS_FREQUENCY_50HZ)
		{
			m_CodecParam.KeyFrameInterval = 25;
		}
		else
		{
		if (m_SensorFrequency == CMOS_FREQUENCY_60HZ)
				m_CodecParam.KeyFrameInterval = 30;
		else
			m_CodecParam.KeyFrameInterval = 25;
		}
		m_CodecParam.FrameRate=m_CodecParam.KeyFrameInterval;
	}
	snprintf(key, sizeof(key), "AuxCoderFrameRate%02d", m_iChannel);
	result = pCConfig->SetValue(AuxCoderCfgSection, key, nFrameRate);
	if (result == 0 && pCConfig->m_bReadOnlyCfgFlag == 1)
	{
		m_CodecParam.FrameRate = pCConfig->GetValue(AuxCoderCfgSection, key, 0);
	}

	memcpy(&Aux_CodecParam, &m_CodecParam, sizeof(CODEC_PARA_S));
	if(LtMjpegVideo == m_AuxAdvEncType)
	{
		Aux_CodecParam.PicQualityLever = bitRate2quality(Aux_CodecParam.BitRate);
	}

	TRACE_AVL("[CCoder:%s]m_CodecParam.BitRate =%d===SetFrameRate=%d\n", __FUNCTION__, m_CodecParam.BitRate, m_CodecParam.FrameRate);
	result = ZWSys_EncSetCodecPara(m_henc, 1, &Aux_CodecParam, m_AuxAdvEncType);
	if (result)
	{
		AVL_ERR("[CCoder:%s]call ZWSys_EncSetCodecPara failed(%d)\n", __FUNCTION__, result);
		return result;
	}
	return (int) 0;
}

//##ModelId=49BDEDDB00BB
int CCoder::AuxCoder_GetFrameRate()
{
	int ivalue;
	char key[100];

	snprintf(key, sizeof(key), "AuxCoderFrameRate%02d", m_iChannel);
	ivalue = pCConfig->GetValue(AuxCoderCfgSection, key, 255);

	return ivalue;
}

//##ModelId=49BDEDDB00D9
int CCoder::AuxCoder_SetBitRate(int bBitRate)
{
	int result = 0;
	CODEC_PARA_S Aux_CodecParam;
	char key[100] = {0};
	
	TRACE_AVL("[CCoder:%s]SetBitRate =%d\n", __FUNCTION__, bBitRate);
	if (m_CodecParam.BitRate == bBitRate)
	{
		TRACE_AVL("[CCoder:%s]SetBitRate no change\n", __FUNCTION__);
		return 0;
	}
	
	bBitRate = (bBitRate < 64) ? 64 : bBitRate;
	snprintf(key, sizeof(key), "AuxCoderBitRate%02d", m_iChannel);
	result = pCConfig->SetValue(AuxCoderCfgSection, key, bBitRate);
	if (result == 0 && pCConfig->m_bReadOnlyCfgFlag == 1)
	{
		bBitRate = pCConfig->GetValue(AuxCoderCfgSection, key, 0);
	}
	m_CodecParam.BitRate = bBitRate * 1000;
	
	memcpy(&Aux_CodecParam, &m_CodecParam, sizeof(CODEC_PARA_S));

	if(LtMjpegVideo == m_AuxAdvEncType)
	{
		Aux_CodecParam.PicQualityLever = bitRate2quality(Aux_CodecParam.BitRate);
	}

	TRACE_AVL("[CCoder:%s]m_CodecParam.BitRate =%d==SetBitRate\n", __FUNCTION__, m_CodecParam.BitRate);
	if(1 == bSupportRtmp)
	{
		if(Aux_CodecParam.FrameRate < 15)
			Aux_CodecParam.FrameRate = 15;
	}
	result = ZWSys_EncSetCodecPara(m_henc, 1, &Aux_CodecParam, m_AuxAdvEncType);
	if (result)
	{
		AVL_ERR("[CCoder:%s]call ZWSys_EncSetCodecPara failed(%d)\n", __FUNCTION__, result);
		return result;
	}
	
	return (int) result;
}

//##ModelId=49BDEDDB0133
int CCoder::AuxCoder_GetBitRate()
{
	int bitrate=  (m_CodecParam.BitRate / 1000);//>> 10);
	TRACE_AVL("[CCoder:%s]m_CodecParam.BitRate =%d==GetBitRate\n", __FUNCTION__, m_CodecParam.BitRate);
	TRACE_AVL("[CCoder:%s]bitrate =%d==GetBitRate\n", __FUNCTION__, bitrate);
	return bitrate;
}

/*
*brief:
*return:-1 not support Resolution ;nozero return board type
*
*/
int CCoder::isSupport(int byResolution)
{ 
	static int nBoardID = 0;
	static DeviceDescription ddt;

	if (nBoardID == 0)
	{
		ZWSys_GetBoardInfo(&nBoardID);
		EDVRS_DDT_GetDeviceDescriptionByID(&ddt, nBoardID);
	}
	if (byResolution >= 0)
	{
		if ((ddt.bytSubVideoSreamResolutionCap[m_iChannel] & CR_CIF) && (byResolution == 0))
		{
			return byResolution;
		}
		else if ((ddt.bytSubVideoSreamResolutionCap[m_iChannel] & CR_4CIF) && (byResolution == 1))
		{
			return byResolution;
		}
		else if ((ddt.bytSubVideoSreamResolutionCap[m_iChannel] & CR_2CIF) && (byResolution == 2))
		{
			return byResolution;
		}
		/*
		else if (byResolution == 3)
		{
			return byResolution;
		}
		else if (byResolution == 4)
		{
			return byResolution;
		}
		*/
		else if ((ddt.bytSubVideoSreamResolutionCap[m_iChannel] & CR_QCIF) && (byResolution == 5))
		{		
			return byResolution;
		}
		//else if ((ddt.bytSubVideoSreamResolutionCap[m_iChannel] & CR_640x360) && (byResolution == 10))
		else if (ddt.bytSubVideoSreamResolutionCap[m_iChannel] & CR_640x360)
		{
			return byResolution;
		}
		else if (ddt.bytSubVideoSreamResolutionCap[m_iChannel] & CR_QVGA)
		{
			return byResolution;
		}
		else if (ddt.bytSubVideoSreamResolutionCap[m_iChannel] & CR_800x448)
		{
			return byResolution;
		}
		else
		{
			AVL_ERR("my add: don't support format %d\n",byResolution);
			return -1;
		}
		
	}	
	return -1;
}
//add end

int CCoder::AuxCoder_SetFormat(int byResolution, int width, int height)
{
	int result = 0;
	char key[128] = {0};
	CODEC_PARA_S Aux_CodecParam;
	char acResolution[32] = {0};

    fprintf(stderr, "[%s][%d] ********zxr sub width=[%d] height=[%d]*****\r\n",__FUNCTION__,__LINE__, width, height);

	if (width > 0 && height > 0)
	{
		snprintf(acResolution, sizeof(acResolution), "%dX%d", width, height);
		result = IsSupportDeviceAbility(1, 1, 0, acResolution);
		if(100 != result)
		{
			fprintf(stderr, "[%s][%d] Aux unsupport format.w=%d h=%d*****\r\n",__FUNCTION__,__LINE__, width, height);
			return -1;
		}
		m_CodecParam.PictureRes.m_PicWidth = width;
		m_CodecParam.PictureRes.m_PicHeight = height;
	}
	else
	{
        fprintf(stderr, "[%s][%d] invalid param.w=%d h=%d*****\r\n",__FUNCTION__,__LINE__, width, height);
	    return -1;
	}

	snprintf(key, sizeof(key), "AuxCoderFormatWidth%02d", m_iChannel);
	result = pCConfig->SetValue(AuxCoderCfgSection, key, m_CodecParam.PictureRes.m_PicWidth);
	if (result == 0 && pCConfig->m_bReadOnlyCfgFlag == 1)
	{
		m_CodecParam.PictureRes.m_PicWidth = pCConfig->GetValue(AuxCoderCfgSection, key, 0);
	}

	snprintf(key, sizeof(key), "AuxCoderFormatHeight%02d", m_iChannel);
	result = pCConfig->SetValue(AuxCoderCfgSection, key, m_CodecParam.PictureRes.m_PicHeight);
	if (result == 0 && pCConfig->m_bReadOnlyCfgFlag == 1)
	{
		m_CodecParam.PictureRes.m_PicHeight = pCConfig->GetValue(AuxCoderCfgSection, key, 0);
	}

	memcpy(&Aux_CodecParam, &m_CodecParam, sizeof(CODEC_PARA_S));

	if(LtMjpegVideo == m_AuxAdvEncType)
	{
		Aux_CodecParam.PicQualityLever = bitRate2quality(Aux_CodecParam.BitRate);
	}
#ifdef DO_SATA
	result = MP4Sys_EncSetAuxCodecMode(m_henc, m_CodecParam.KeyFrameInterval, m_CodecParam.FrameRate, m_CodecParam.BitRate);
#else
	if(1 == bSupportRtmp)
	{
		if(Aux_CodecParam.FrameRate < 15)
		{
			Aux_CodecParam.FrameRate = 15;
			printf("ctrl 15fps\n");
		}
	}
	result = ZWSys_EncSetCodecPara(m_henc, 1, &Aux_CodecParam, m_AuxAdvEncType);
#endif

	if (result)
	{
		AVL_ERR("[CCoder:%s]call ZWSys_EncSetCodecPara failed(%d)\n", __FUNCTION__, result);
		return result;
	}

	//snprintf(key, sizeof(key), "AuxCoderFormat%02d", m_iChannel);
	//pCConfig->SetValue(AuxCoderCfgSection, key, m_CodecParam.PictureFormat);
	//pCConfig->SetValue(AuxCoderCfgSection, key, ResolutionType);

	return (int) result;
}

int CCoder::AuxCoder_GetFormat(int *width, int *height)
{
	int byResolution = 0;

    //在这里直接返回分辨率,这里是为了兼容V2版本编码参数
    if ((width != NULL) && (height != NULL))
    {
        *width = m_CodecParam.PictureRes.m_PicWidth; 
        *height = m_CodecParam.PictureRes.m_PicHeight;
    }

	return (int)byResolution;
}

int CCoder::GetAudioInVol()
{
	return m_nAudioInVol;
}

int CCoder::SetAudioInVol(int vol)
{
	int ivalue = 0, nSDKRet = 0;
	char szSectionComm[30] = {0};

	if (vol < 0 || vol > 100)
	{
		printf("[CCoder:%s] invalid vol value\n", __FUNCTION__);
	}
	
	void *hChannelHandle=ZWSys_EncChannelOpen(0);
	
	if (hChannelHandle == NULL)
	{
		printf("ChannelOpen fail, !!!!!!!!!\n");
		return -1;
	}

	snprintf(szSectionComm, sizeof(szSectionComm),"%s", "AudioIn");
	ivalue = pCConfig->SetValue(szSectionComm, "AudioVol", vol);
	if (ivalue == 0 && pCConfig->m_bReadOnlyCfgFlag == 1)
	{
		vol = pCConfig->GetValue(szSectionComm, "AudioVol", 0);
	}

	m_nAudioInVol = vol;
	
	nSDKRet = ZWSys_SetAudioInVol(hChannelHandle, m_nAudioInVol);
	if (nSDKRet)
	{
		printf("[CCoder:%s]call ZWSys_SetAudioInVol failed(%d)\n", __FUNCTION__, nSDKRet);
		return nSDKRet;
	}

	return ivalue;
}

int CCoder::GetAudioOutVol()
{
	return m_nAudioOutVol;
}

int CCoder::SetAudioOutVol(int vol)
{
	int ivalue, nSDKRet;
	char szSectionComm[36];

	if (vol < 0 || vol > 100)
	{
		printf("[CCoder:%s] invalid vol value\n", __FUNCTION__);
	}
	
	void *hChannelHandle=ZWSys_EncChannelOpen(0);

	snprintf(szSectionComm, sizeof(szSectionComm),"%s", "AudioOut");
	ivalue = pCConfig->SetValue(szSectionComm, "AudioVol", vol);
	if (ivalue == 0 && pCConfig->m_bReadOnlyCfgFlag == 1)
	{
		vol = pCConfig->GetValue(szSectionComm, "AudioVol", 0);
	}

	m_nAudioOutVol = vol;
	
	if (hChannelHandle == NULL)
	{
		printf("ChannelOpen fail, !!!!!!!!!\n");
		return -1;
	}
	
	nSDKRet = ZWSys_SetAudioOutVol(hChannelHandle, m_nAudioOutVol, 1);
	if (nSDKRet)
	{
		printf("[CCoder:%s]call ZWSys_SetAudioOutVol failed(%d)\n", __FUNCTION__, nSDKRet);
		return nSDKRet;
	}

	return ivalue;
}

int CCoder::SetAudioMappingVol(int vol,int Type)
{
	int nSDKRet;
	if (vol < 0 || vol > 100 || Type < 0 || Type > 3)
	{
		printf("[CCoder:%s] invalid vol value\n", __FUNCTION__);
	}
	
	void *hChannelHandle=ZWSys_EncChannelOpen(0);

	if (hChannelHandle == NULL)
	{
		printf("ChannelOpen fail, !!!!!!!!!\n");
		return -1;
	}
	
	nSDKRet = ZWSys_SetAudioMappingVol(hChannelHandle, vol, Type);
	if (nSDKRet)
	{
		printf("[CCoder:%s]call ZWSys_SetAudioOutVol failed(%d)\n", __FUNCTION__, nSDKRet);
		return nSDKRet;
	}

	return 0;
}


int CCoder::SetAudioCustomVoice(int nCh, int customtype, char *pBuff, int len)
{
	int nSDKRet;

	if(NULL == pBuff)
	{
		AVL_ERR("Buffer is NULL!\n");
		return -1;
	}

	//检测长度
	if(len > MAX_CUSTOM_AUDIO_SIZE)//超过128K
	{
		printf("Amr data size %d is too big! must be less than %d\n", len, MAX_CUSTOM_AUDIO_SIZE);
		return -1;
	}

	nSDKRet = ZWSys_SetAudioCustomVoice(m_henc, customtype, pBuff, len);
	if (nSDKRet)
	{
		printf("[CCoder:%s]call SetAudioAlarmVoice failed(%d)\n", __FUNCTION__, nSDKRet);
		return nSDKRet;
	}

	return 0;
}

int CCoder::SetAudioAlarmConfig(int nCh, int enable, int audioType, int audioDelayOutput, int audioVolume, int iAudioPlayTimes)
{
	int nSDKRet;

	if ((enable != 0) && (enable != 1))
	{
		AVL_ERR("param error! enable=%d\n", enable);
		return -1;
	}
	
	if ((audioType != 0) && (audioType != 1) && (audioType != 2))
	{
		AVL_ERR("param error! audioType=%d\n", audioType);
		return -1;
	}

	if ((audioDelayOutput < 5) || (audioDelayOutput > 60))
	{
		AVL_ERR("param error! audioDelayOutput=%d\n", audioDelayOutput);
		return -1;
	}

	if ((audioVolume < 0) || (audioVolume > 100))
	{
		AVL_ERR("param error! audioVolume=%d\n", audioVolume);
		return -1;
	}

	if ((iAudioPlayTimes < 1) || (iAudioPlayTimes > 10))
	{
		AVL_ERR("param error! audioVolume=%d\n", audioVolume);
		return -1;
	}

	if((m_audioAlarmCfg.enable == enable) && (m_audioAlarmCfg.alarmSpeechType == audioType)
		&& (m_audioAlarmCfg.alarmOutDelay == audioDelayOutput) && (m_audioAlarmCfg.broadCastVolume == audioVolume)
		&& (m_audioAlarmCfg.broadCastTimes == iAudioPlayTimes))
	{
		return 0;
	}

	m_audioAlarmCfg.enable = enable;
	m_audioAlarmCfg.alarmSpeechType = audioType;
	m_audioAlarmCfg.alarmOutDelay = audioDelayOutput;
	m_audioAlarmCfg.broadCastVolume = audioVolume;
	m_audioAlarmCfg.broadCastTimes = iAudioPlayTimes;

	pCConfig->SetValue(AudioAlarmCfgSection, "enable", enable);
	pCConfig->SetValue(AudioAlarmCfgSection, "audioType", audioType);
	pCConfig->SetValue(AudioAlarmCfgSection, "audioDelayOutput", audioDelayOutput);
	pCConfig->SetValue(AudioAlarmCfgSection, "audioVolume", audioVolume);
	pCConfig->SetValue(AudioAlarmCfgSection, "iAudioPlayTimes", iAudioPlayTimes);
	
	nSDKRet = ZWSys_SetAlarmSpeechCfg(m_henc, &m_audioAlarmCfg);

	return nSDKRet;
}

//!\brief 设置声音报警的时间计划
//!\param ttagSchedule cfg 时间计划结构体
//!\return 成功为0，失败为-1
int CCoder::SetAudioAlarmSchedule(const tagAudioAlarmSchedule * cfg)
{
	char value[255];

	char key_str[30];

	int i, j;

	if (NULL == cfg)
	{
		printf("[CCoder:%s]cfg==NULL\n", __FUNCTION__);
		return -1;
	}
	if (cfg->bytState < 0 || cfg->bytState > 2)
	{
		printf("[CCoder:%s]cfg->bytState=%d\n", __FUNCTION__, cfg->bytState);
		return -1;
	}


	//布防计划--
	pCConfig->SetValue(AudioAlarmCfgSection, "ScheduleCfgEnable", cfg->bytState);
	pCConfig->SetValue(AudioAlarmCfgSection, "ScheduleCfgEnableDN", cfg->nDayMode);
	
	for (i = 0; i < 7; i++)
	{
		for (j = 0; j < 48; j++)
		{
			value[j] = cfg->dwTimeSeg[i][j] + '0';
		}
		value[j] = 0;
		snprintf(key_str, sizeof(key_str), "ScheduleCfg_%01d", i);

		pCConfig->SetValue(AudioAlarmCfgSection, key_str, value);
		TRACE_AVL("[CCoder:%s]cfg->bytState=%d,cfg->nDayMode=%d,Write:%s\n", __FUNCTION__, cfg->bytState, cfg->nDayMode, value);
	}
	//更新计划
	memcpy(&m_AudioAlarmScheduleCfg, cfg, sizeof (tagAudioAlarmSchedule));	
	return 0;
}

//!\brief 获取声音报警的时间计划
//!\param tagAudioAlarmSchedule* cfg 时间计划结构体
//!\return 成功为0，失败为-1
int CCoder::GetAudioAlarmSchedule(tagAudioAlarmSchedule * cfg)
{
	if (NULL == cfg)
	{
		AVL_ERR("[CCoder:%s]cfg==NULL\n", __FUNCTION__);
		return -1;
	}
	memcpy(cfg, &m_AudioAlarmScheduleCfg, sizeof (tagAudioAlarmSchedule));
	return 0;
}

int CCoder::CheckAudioAlarmInSchedule()
{
	int day = 0;
	int seg = 0;
	time_t curr_time = 0;
	struct tm curr_tm = {0};
	int nResult = 0;
	
	if (1 == m_AudioAlarmScheduleCfg.bytState)//自定义
	{
		//获取当前时间段信息
		curr_time = time(NULL);
		localtime_r(&curr_time, &curr_tm);
		day = curr_tm.tm_wday;
		seg = (curr_tm.tm_hour << 1) + (curr_tm.tm_min >= 30);
		TRACE_AVL("wday=%d hour=%d min=%d---\n", day, curr_tm.tm_hour, curr_tm.tm_min);
		
		if (m_AudioAlarmScheduleCfg.dwTimeSeg[day][seg])
		{
			nResult = 1;
		}
	}
    else
    {
        nResult = 1;
    }

	return nResult;
}


int CCoder::TryListenAlarmVoice(int nCh, int enable, int audioType, int audioDelayOutput, int audioVolume, int alarmType)
{
	SpeechType_T curSpeechType = SPEECH_ALARM_HUMAN;

	return ZWSys_SetSpeechContent(curSpeechType);
}

int CCoder::GetAudioAlarmConfig(int nCh, int *enable, int *audioType, int *audioDelayOutput, int *audioVolume, int *iAudioPlayTimes)
{
	if((NULL == enable) || (NULL == audioType) || (NULL == audioDelayOutput) || (NULL == audioVolume))
	{
		AVL_ERR("param point is NULL\n");
		return -1;
	}

	*enable = m_audioAlarmCfg.enable;
	*audioType = m_audioAlarmCfg.alarmSpeechType;
	*audioDelayOutput = m_audioAlarmCfg.alarmOutDelay;
	*audioVolume = m_audioAlarmCfg.broadCastVolume;
	*iAudioPlayTimes = m_audioAlarmCfg.broadCastTimes;

	return 0;
}

int CCoder::GetGetRealKeyFrameInterval(int nChannel,int *nFrameRateInterval, int *nKeyFrameInterval)
{
	if( NULL == nFrameRateInterval && NULL == nKeyFrameInterval)
	{
		return -1;
	}
	
	return ZWSys_GetRealKeyFrameInterval(nChannel, nFrameRateInterval,nKeyFrameInterval);
}

int CCoder::GetKeyFrameInterval(int stream, int *value)
{
	if (0 == stream)
	{
		*value = m_nMainKeyFrameInterval;
	}
	else if (1 == stream)
	{
		*value = m_nAuxKeyFrameInterval;
	}
	else if (2 == stream)
	{	
#ifdef USE_HDAUX
		*value = m_nHDAuxKeyFrameInterval;
#endif
	}
	
	return 0;
}

int CCoder::SetKeyFrameInterval(int stream, int value)
{
	int result = -1;
	char key[100] = {0};
	CODEC_PARA_S_EXT stCodecParam;
	
	char keyResult[256] = {0};
	int frameRate = 0;
	int null = 0;

	if (value <= 0 || value > 200)
	{
		fprintf(stderr, "[%s][%d] invalid param.KeyFrameInterval=[%d]\r\n",__FUNCTION__,__LINE__, value);
		return -1;
	}

	if (0 == stream)
	{
		m_nMainKeyFrameInterval = value;
		snprintf(key, sizeof(key), "MainCoderKeyFrameInterval%02d", m_iChannel);
		result = pCConfig->SetValue(MainCoderCfgSection, key, value);
		if (result == 0 && pCConfig->m_bReadOnlyCfgFlag == 1)
		{
			m_nMainKeyFrameInterval = pCConfig->GetValue(MainCoderCfgSection, key, 0);
			value = m_nMainKeyFrameInterval;
		}
	}
	else if (1 == stream)
	{
		m_nAuxKeyFrameInterval = m_CodecParam.FrameRate * 4;
		snprintf(key, sizeof(key), "AuxCoderKeyFrameInterval%02d", m_iChannel);
		result = pCConfig->SetValue(AuxCoderCfgSection, key, value);
		if (result == 0 && pCConfig->m_bReadOnlyCfgFlag == 1)
		{
			m_nAuxKeyFrameInterval = pCConfig->GetValue(AuxCoderCfgSection, key, 0);
			value = m_nAuxKeyFrameInterval;
		}
	}
	else if (2 == stream)
	{
#ifdef USE_HDAUX
		snprintf(key, sizeof(key), "HDAuxEncCfg%02d", m_iChannel);
		pCConfig->GetValue(HDAuxCoderCfgSection, key, "10:15:1:0:0:2:384:0", keyResult, 256);
		sscanf(keyResult, "%d:%d:%d:%d:%d:%d:%d:%d",
			   &frameRate, &null, &null, &null, &null, &null, &null, &null);
		m_nHDAuxKeyFrameInterval = frameRate * 4;

		snprintf(key, sizeof(key), "HDAuxEncCfgKeyFrameInterval%02d", m_iChannel);
		result = pCConfig->SetValue(HDAuxCoderCfgSection, key, value);
		if (result == 0 && pCConfig->m_bReadOnlyCfgFlag == 1)
		{
			m_nHDAuxKeyFrameInterval = pCConfig->GetValue(HDAuxCoderCfgSection, key, 0);
			value = m_nHDAuxKeyFrameInterval;
		}
#endif
	}

	memset(&stCodecParam, 0, sizeof(stCodecParam));
	stCodecParam.KeyFrameInterval = value;

	result = ZWSys_EncSetCodecParaExt(m_henc, stream, &stCodecParam);
	if (result)
	{
		printf("[CCoder:%s]call MP4Sys_EncSetCodecParaExt failed(%d)\n", __FUNCTION__, result);
		return result;
	}

	return (int) result;
}

int CCoder::AuxCoder_GetQuality(void)
{
	return m_CodecParam.PicQualityLever;
}

int CCoder::AuxCoder_SetQuality(int dwQuality)
{
	int result = -1;
	char key[100] = {0};

	printf("[CCoder:%s]SetQuptiety = %d\n", __FUNCTION__, dwQuality);
	if (m_CodecParam.PicQualityLever == dwQuality)
	{
		printf("[CCoder:%s]SetQuptiety no change\n", __FUNCTION__);
		return 0;
	}

    m_CodecParam.PicQualityLever = dwQuality;

	printf("[CCoder:%s]m_CodecParam.PicQualityLever=%d==SetQuptiety\n", __FUNCTION__, m_CodecParam.PicQualityLever);

 	if(1 == bSupportRtmp)
	{
		if(m_CodecParam.FrameRate < 15)
			m_CodecParam.FrameRate = 15;
	} 

	snprintf(key, sizeof(key), "AuxCoderQuptiety%02d", m_iChannel);
	result = pCConfig->SetValue(AuxCoderCfgSection, key, dwQuality);
	if (result == 0 && pCConfig->m_bReadOnlyCfgFlag == 1)
	{
		m_CodecParam.PicQualityLever = pCConfig->GetValue(AuxCoderCfgSection, key, 0);
	}
	
	result = ZWSys_EncSetCodecPara(m_henc, 1, &m_CodecParam, m_AuxAdvEncType);
	if (result)
	{
		printf("[CCoder:%s]call ZWSys_EncSetCodecPara failed(%d)\n", __FUNCTION__, result);
		return result;
	}

	return (int) result;
}

int CCoder::AuxCoder_GetBitrateType(void)
{
	return m_CodecParam.BitRateCtlMode;
}

int CCoder::AuxCoder_SetBitrateType(int dwBitrateType)
{
	int result = -1;
	char key[100] = {0};

    if (dwBitrateType>2 || dwBitrateType<0)
    {
		printf("[CCoder:%s]dwBitrateType(%d) out of [0,2]\n", __FUNCTION__, dwBitrateType);
		return -1;
    }
    
    /* 参数能力级检测 */
    if (dwBitrateType != IsSupportDeviceAbility(1, 2, dwBitrateType, NULL))
    {
        AVL_ERR("Device not support ability(BitrateType)\n");
        return -1;
    }

	printf("[CCoder:%s]SetBitrateType = %d\n", __FUNCTION__, dwBitrateType);
	if (m_CodecParam.BitRateCtlMode == dwBitrateType)
	{
		printf("[CCoder:%s]SetBitrateType no change\n", __FUNCTION__);
		return 0;
	}
	m_CodecParam.BitRateCtlMode = dwBitrateType;

	printf("[CCoder:%s]m_CodecParam.BitRateCtlMode =%d==SetBitrateType\n", __FUNCTION__, m_CodecParam.BitRateCtlMode);

 	if(1 == bSupportRtmp)
	{
		if(m_CodecParam.FrameRate < 15)
			m_CodecParam.FrameRate = 15;
	}

	snprintf(key, sizeof(key), "AuxCoderBitrateType%02d", m_iChannel);
	result = pCConfig->SetValue(AuxCoderCfgSection, key, dwBitrateType);
	if (result == 0 && pCConfig->m_bReadOnlyCfgFlag == 1)
	{
		m_CodecParam.BitRateCtlMode = pCConfig->GetValue(AuxCoderCfgSection, key, 0);
	}
	
	result = ZWSys_EncSetCodecPara(m_henc, 1, &m_CodecParam, m_AuxAdvEncType);
	if (result)
	{
		AVL_ERR("[CCoder:%s]call ZWSys_EncSetCodecPara failed(%d)\n", __FUNCTION__, result);
		return result;
	}

	return (int) result;
}

int CCoder::AuxCoder_GetStreamType(void)
{
	return m_CodecStreamType;
}

int CCoder::AuxCoder_SetStreamType(int dwStreamType)
{
	int result = 0;
	char key[100] = {0};

    if (dwStreamType>1 || dwStreamType<0)
    {
		AVL_ERR("[CCoder:%s]dwStreamType(%d) out of [0,1]\n", __FUNCTION__, dwStreamType);
		return -1;
    }
    
	printf("[CCoder:%s]SetStreamType=%d\n", __FUNCTION__, dwStreamType);
	if (m_CodecStreamType == dwStreamType)
	{
		printf("[CCoder:%s]SetStreamType no change\n", __FUNCTION__);
		return 0;
	}

    m_CodecStreamType = dwStreamType;

	printf("[CCoder:%s]m_CodecStreamType =%d==SetStreamType\n", __FUNCTION__, m_CodecStreamType);

	snprintf(key, sizeof(key), "AuxCoderStreamType%02d", m_iChannel);
	result = pCConfig->SetValue(AuxCoderCfgSection, key, dwStreamType);
	if (result == 0 && pCConfig->m_bReadOnlyCfgFlag == 1)
	{
		m_CodecStreamType = pCConfig->GetValue(AuxCoderCfgSection, key, 0);
	}
	mysystem("sync");
	return (int) result;
}

/* 设置子码流高级编码参数(H264/H265/MJPEG) */
int CCoder::SetAuxAdvEnc(int AuxAdvEncType)
{
    int s32Ret = 0;
    EncodeType_t encType;
	char key[100] = {0};

    printf("[%s:%d]---AuxAdvEncType=%d---\n", __FUNCTION__, __LINE__, AuxAdvEncType);
    
    /* 参数能力级检测 */
    if (AuxAdvEncType != IsSupportDeviceAbility(1, 0, AuxAdvEncType, NULL))
    {
        AVL_ERR("Device not support ability(VideoCode)\n");
        return -1;
    }

    if (AuxAdvEncType == 0)
	{
        encType = LtH264Video;
    }
    else if (AuxAdvEncType == 1)
	{
        encType = LtH265Video;
    }
    else if (AuxAdvEncType == 2)
	{
        encType = LtMjpegVideo;
    }
	else if (AuxAdvEncType == 3)
	{
        encType = LtH265PlusVideo;
    }
	else if (AuxAdvEncType == 4)
	{
        encType = LtH264PlusVideo;
    }
	else
	{
        encType = LtH264Video;
    }

	snprintf(key, sizeof(key), "AuxFormatType%02d", m_iChannel);
	if(encType != (pCConfig->GetValue(AuxCoderCfgSection, key, LtH264Video)))
	{
	    m_AuxAdvEncType = encType;
		if(1 == bSupportRtmp)
		{
			if(m_CodecParam.FrameRate < 15)
			{
				m_CodecParam.FrameRate = 15;
				printf("ctrl 15fps\n");
			}
		}	
		
		s32Ret = pCConfig->SetValue(AuxCoderCfgSection, key, encType);
		if (s32Ret == 0 && pCConfig->m_bReadOnlyCfgFlag == 1)
		{
			m_AuxAdvEncType = (EncodeType_t)(pCConfig->GetValue(AuxCoderCfgSection, key, 0));
			encType = m_AuxAdvEncType;
		}
        s32Ret = ZWSys_EncSetCodecPara(m_henc, 1, &m_CodecParam, encType);
        if (s32Ret)
        {
            AVL_ERR("[CCoder:%s]call ZWSys_EncSetCodecPara failed(%d)\n", __FUNCTION__, s32Ret);
            return s32Ret;
        }
	}

    return s32Ret;
}


/* 获取子码流高级编码参数(H264/H265/MJPEG) */
int CCoder::GetAuxAdvEnc(int *pAuxAdvEncType)
{
	if (m_AuxAdvEncType == LtH264Video)
	{
        *pAuxAdvEncType = 0;
    }
    else if (m_AuxAdvEncType == LtH265Video)
	{
        *pAuxAdvEncType = 1;
    }
    else if (m_AuxAdvEncType == LtMjpegVideo)
	{
        *pAuxAdvEncType = 2;
    }
	else if (m_AuxAdvEncType == LtH265PlusVideo)
	{
        *pAuxAdvEncType = 3;
    }
	else if (m_AuxAdvEncType == LtH264PlusVideo)
	{
        *pAuxAdvEncType = 4;
    }
	else
	{
        *pAuxAdvEncType = 0;
    }

    printf("[%s:%d]---AuxAdvEncType=%d---\n", __FUNCTION__, __LINE__, *pAuxAdvEncType);

    return 0;
}

/* 设置高清子码流高级编码参数(H264/H265/MJPEG) */
int CCoder::SetHDAuxAdvEnc(int HDAuxAdvEncType)
{
    int s32Ret = 0;
    BoardSensorType_t Sensortype;
    char szSectionComm[30];
	int ivalue_sensor;
    
#ifdef USE_HDAUX
    EncodeType_t encType;
    CODEC_PARA_S codec;
	char key[100] = {0};

    printf("[%s:%d]---HDAuxAdvEncType=%d---\n", __FUNCTION__, __LINE__, HDAuxAdvEncType);

    /* 参数能力级检测 */
    if (HDAuxAdvEncType != IsSupportDeviceAbility(2, 0, HDAuxAdvEncType, NULL))
    {
        AVL_ERR("Device not support ability(VideoCode)\n");
        return -1;
    }

	if (HDAuxAdvEncType == 0)
	{
        encType = LtH264Video;
    }
    else if (HDAuxAdvEncType == 1)
	{
        encType = LtH265Video;
    }
    else if (HDAuxAdvEncType == 2)
	{
        encType = LtMjpegVideo;
    }
	else if (HDAuxAdvEncType == 3)
	{
        encType = LtH265PlusVideo;
    }
	else if (HDAuxAdvEncType == 4)
	{
        encType = LtH264PlusVideo;
    }
	else
	{
        encType = LtH264Video;
    }
        
	snprintf(key, sizeof(key), "HDAuxFormatType%02d", m_iChannel);
	if(encType != (pCConfig->GetValue(HDAuxCoderCfgSection, key, LtH264Video)))
	{
        m_HDAuxAdvEncType = encType;

        ZWSys_GetSensorType(&Sensortype);
		snprintf(szSectionComm, sizeof(szSectionComm),"ISP_S%d",Sensortype);
		ivalue_sensor = pCConfig->GetValue(szSectionComm, "InputFrequency", 0);
		if (1 == ivalue_sensor)
			m_SensorFrequency = CMOS_FREQUENCY_60HZ;
		else
			m_SensorFrequency = CMOS_FREQUENCY_50HZ;
		
		codec.FrameRate = m_HDCodecParam.dwVideoFrameRate;
		codec.KeyFrameInterval = codec.FrameRate;
		if (codec.KeyFrameInterval == 0xFF)
		{
			if (m_SensorFrequency == CMOS_FREQUENCY_50HZ)
				codec.KeyFrameInterval = 25;
			else
				codec.KeyFrameInterval = 30;
		}
		
		codec.PictureFormat = SwitchPicFormat(m_HDCodecParam.byResolution);
		codec.BitRate = (m_HDCodecParam.dwVideoBitrateH < 64) ? 64 : m_HDCodecParam.dwVideoBitrateH;
		codec.BitRate = codec.BitRate * 1000;//<< 10;
		codec.PicQualityLever = m_HDCodecParam.dwQuotiety;
		codec.BitRateCtlMode = m_HDCodecParam.byBitrateType;

		codec.PictureRes.m_PicWidth = m_HDCodecParam.dwResWidth;
		codec.PictureRes.m_PicHeight = m_HDCodecParam.dwResHeight;

		TRACE_AVL("SDK SET STREAM 2\n\
			encType=%d\n\
			codec.BitRate=%d\n\
			codec.BitRateCtlMode%d\n\
			codec.FrameRate=%d\n\
			codec.KeyFrameInterval=%d\n\
			codec.PicQualityLever=%d\n\
			codec.PictureFormat=%d\n\
			\n\n",encType,
			codec.BitRate,
			codec.BitRateCtlMode,
			codec.FrameRate,
			codec.KeyFrameInterval,
			codec.PicQualityLever,
			codec.PictureFormat);
	 	if(1 == bSupportRtmp)
		{
			if(codec.FrameRate < 15)
				codec.FrameRate = 15;
		} 		

		s32Ret = pCConfig->SetValue(HDAuxCoderCfgSection, key, encType);
		if (s32Ret == 0 && pCConfig->m_bReadOnlyCfgFlag == 1)
		{
			m_HDAuxAdvEncType = (EncodeType_t)(pCConfig->GetValue(HDAuxCoderCfgSection, key, 0));
			encType = m_HDAuxAdvEncType;
		}
		
        s32Ret = ZWSys_EncSetCodecPara(m_henc, 2, &codec, encType);
        if (s32Ret)
        {
            AVL_ERR("[CCoder:%s]call ZWSys_EncSetCodecPara failed(%d)\n", __FUNCTION__, s32Ret);
            return s32Ret;
        }
	}
#endif

    return s32Ret;
}

/* 获取高清子码流高级编码参数(H264/H265/MJPEG) */
int CCoder::GetHDAuxAdvEnc(int *pHDAuxAdvEncType)
{
    int s32Ret = 0;
    
#ifdef USE_HDAUX
	if (m_HDAuxAdvEncType == LtH264Video)
	{
        *pHDAuxAdvEncType = 0;
    }
    else if (m_HDAuxAdvEncType == LtH265Video)
	{
        *pHDAuxAdvEncType = 1;
    }
    else if (m_HDAuxAdvEncType == LtMjpegVideo)
	{
        *pHDAuxAdvEncType = 2;
    }
	else if (m_HDAuxAdvEncType == LtH265PlusVideo)
	{
        *pHDAuxAdvEncType = 3;
    }
	else if (m_HDAuxAdvEncType == LtH264PlusVideo)
	{
        *pHDAuxAdvEncType = 4;
    }
	else
	{
        *pHDAuxAdvEncType = 0;
    }

    printf("[%s:%d]---HDAuxAdvEncType=%d---\n", __FUNCTION__, __LINE__, *pHDAuxAdvEncType);
#endif

    return s32Ret;
}

/* 设置第四码流高级编码参数(H264/H265/MJPEG) */
int CCoder::SetFourthStreamAdvEnc(int FourthStreamAdvEncType)
{
    int s32Ret = 0;

#ifdef USE_FOURTH_STREAM
    EncodeType_t encType;
	char key[100];

	printf("[%s:%d]---FourthStreamAdvEncType=%d---\n", __FUNCTION__, __LINE__, FourthStreamAdvEncType);

    /* 参数能力级检测 */
    if (FourthStreamAdvEncType != IsSupportDeviceAbility(3, 0, FourthStreamAdvEncType, NULL))
    {
        AVL_ERR("Device not support ability(VideoCode)\n");
        return -1;
    }

	if (FourthStreamAdvEncType == 0)
	{
        encType = LtH264Video;
    }
    else if (FourthStreamAdvEncType == 1)
	{
        encType = LtH265Video;
    }
    else if (FourthStreamAdvEncType == 2)
	{
        encType = LtMjpegVideo;
    }
	else if (FourthStreamAdvEncType == 3)
	{
        encType = LtH265PlusVideo;
    }
	else if (FourthStreamAdvEncType == 4)
	{
        encType = LtH264PlusVideo;
    }
	else
	{
        encType = LtH264Video;
    }
		
	snprintf(key, sizeof(key), "FourthStreamFormatType%02d", m_iChannel);
	if(encType != (pCConfig->GetValue(HDFourthStreamCfgSection, key, LtH264Video)))
	{
	    m_FourthStreamAdvEncType = encType;
 //     s32Ret = ZWSys_EncSetCodecPara(m_henc, 1, &m_CodecParam, encType);
        if (s32Ret)
        {
            AVL_ERR("[CCoder:%s]call ZWSys_EncSetCodecPara failed(%d)\n", __FUNCTION__, s32Ret);
            return s32Ret;
        }
        pCConfig->SetValue(HDFourthStreamCfgSection, key, encType);
	}
#endif

    return s32Ret;
}

/* 获取第四码流高级编码参数(H264/H265/MJPEG) */
int CCoder::GetFourthStreamAdvEnc(int *pFourthStreamAdvEncType)
{
    int s32Ret = 0;

#ifdef USE_FOURTH_STREAM
	if (m_FourthStreamAdvEncType == LtH264Video)
	{
        *pFourthStreamAdvEncType = 0;
    }
    else if (m_FourthStreamAdvEncType == LtH265Video)
	{
        *pFourthStreamAdvEncType = 1;
    }
    else if (m_FourthStreamAdvEncType == LtMjpegVideo)
	{
        *pFourthStreamAdvEncType = 2;
    }
	else if (m_FourthStreamAdvEncType == LtH265PlusVideo)
	{
        *pFourthStreamAdvEncType = 3;
    }
	else if (m_FourthStreamAdvEncType == LtH264PlusVideo)
	{
        *pFourthStreamAdvEncType = 4;
    }
	else
	{
        *pFourthStreamAdvEncType = 0;
    }
        
    printf("[%s:%d]---FourthStreamAdvEncType=%d---\n", __FUNCTION__, __LINE__, *pFourthStreamAdvEncType);
    
#endif

    return s32Ret;
}

EncodeType_t CCoder::SwitchAdvCodeType(int nAdvCodeType)
{
    switch(nAdvCodeType)
    {
        case 0:return LtH264Video;
        case 1:return LtH265Video;
        case 2:return LtMjpegVideo;
		case 3:return LtH265PlusVideo;
		case 4:return LtH264PlusVideo;
        default:
        AVL_ERR("Not support Advance Codec %d\n", nAdvCodeType);
    }
    return (EncodeType_t)-1;
}

int CCoder::ReverseSwitchAdvCodeType(EncodeType_t eAdvCodeType)
{
    switch(eAdvCodeType)
    {
        case LtH264Video:return 0;
        case LtH265Video:return 1;
        case LtMjpegVideo:return 2;
		case LtH265PlusVideo:return 3;
		case LtH264PlusVideo:return 4;
        default:
        AVL_ERR("Not support Advance Codec %d\n", eAdvCodeType);
    }
    return -1;
}


int CCoder::ReverseSwitchFormat(int PicFormat)
{
    eResolutionIndex ePicFormat = eResolutionIndex_CIF;

    switch (PicFormat)
    {
        case ENC_CIF_FORMAT:     ePicFormat = eResolutionIndex_CIF; break;
        case ENC_D1_FORMAT:      ePicFormat = eResolutionIndex_D1; break;
        case ENC_360P_FORMAT:    ePicFormat = eResolutionIndex_640x360; break;
        case ENC_QVGA_FORMAT:    ePicFormat = eResolutionIndex_QVGA; break;
        case ENC_VGA_FORMAT:     ePicFormat = eResolutionIndex_VGA; break;
        case ENC_HD720_FORMAT:   ePicFormat = eResolutionIndex_720P; break;
        case ENC_QuadVGA_FORMAT: ePicFormat = eResolutionIndex_QuadVGA; break;
        case ENC_SXGA_FORMAT:    ePicFormat = eResolutionIndex_SXGA; break;
        case ENC_UXGA_FORMAT:    ePicFormat = eResolutionIndex_UXGA; break;
        case ENC_HD1080_FORMAT:  ePicFormat = eResolutionIndex_1080P; break;        
        case ENC_QXGA_FORMAT:    ePicFormat = eResolutionIndex_QXGA; break;        
        case ENC_UWXGA_FORMAT:   ePicFormat = eResolutionIndex_UWXGA; break;        
        case ENC_USXGA_FORMAT:   ePicFormat = eResolutionIndex_USXGA; break;        
        default :AVL_ERR("Not support Picture Format %d\n", PicFormat); break;
    }
    
    return (int)ePicFormat;
}

int CCoder::SetAuxHDAdvEnc(int AuxAdvEncType, int HDAuxAdvEncType)
{
    int s32Ret = 0;
    CODEC_PARA_S codec;
	char key[100];
    
	snprintf(key, sizeof(key), "AuxFormatType%02d", m_iChannel);
	if(AuxAdvEncType != (pCConfig->GetValue(AuxCoderCfgSection, key, LtH264Video)))
	{
		memcpy(&codec, &m_CodecParam, sizeof(CODEC_PARA_S));
		codec.PicQualityLever = bitRate2quality(codec.BitRate);
	 	if(1 == bSupportRtmp)
		{
			if(m_CodecParam.FrameRate < 15)
				m_CodecParam.FrameRate = 15;
		} 			

		s32Ret = pCConfig->SetValue(AuxCoderCfgSection, key, AuxAdvEncType);
		if (s32Ret == 0 && pCConfig->m_bReadOnlyCfgFlag == 1)
		{
			m_AuxAdvEncType = (EncodeType_t)(pCConfig->GetValue(AuxCoderCfgSection, key, 0));
			AuxAdvEncType = m_AuxAdvEncType;
		}
        s32Ret = ZWSys_EncSetCodecPara(m_henc, 1, &m_CodecParam, (EncodeType_t)AuxAdvEncType);
        if (s32Ret)
        {
            AVL_ERR("[CCoder:%s]call ZWSys_EncSetCodecPara failed(%d)\n", __FUNCTION__, s32Ret);
            return s32Ret;
        }
		m_AuxAdvEncType = (EncodeType_t)AuxAdvEncType;
        pCConfig->SetValue(AuxCoderCfgSection, key, m_AuxAdvEncType);
	}

	snprintf(key, sizeof(key), "HDAuxFormatType%02d", m_iChannel);
	if (HDAuxAdvEncType != (pCConfig->GetValue(HDAuxCoderCfgSection, key, LtH264Video)))
	{
        codec.PictureFormat = SwitchPicFormat(m_HDCodecParam.byResolution);
        codec.BitRate = (m_HDCodecParam.dwVideoBitrateH < 64) ? 64 : m_HDCodecParam.dwVideoBitrateH;
        codec.BitRate = codec.BitRate * 1000;
        codec.FrameRate = m_HDCodecParam.dwVideoFrameRate;
        codec.KeyFrameInterval = m_HDCodecParam.dwVideoFrameRate;
        codec.PicQualityLever = m_HDCodecParam.dwQuotiety;
        codec.BitRateCtlMode = m_HDCodecParam.byBitrateType;
		codec.PictureRes.m_PicWidth = m_HDCodecParam.dwResWidth;
		codec.PictureRes.m_PicHeight = m_HDCodecParam.dwResHeight;

	 	if (1 == bSupportRtmp)
		{
			if(codec.FrameRate < 15)
				codec.FrameRate = 15;
		} 		
		s32Ret = ZWSys_EncSetCodecPara(m_henc, 2, &codec, (EncodeType_t)HDAuxAdvEncType);
        if (s32Ret)
        {
            AVL_ERR("[CCoder:%s]call ZWSys_EncSetCodecPara failed(%d)\n", __FUNCTION__, s32Ret);
            return s32Ret;
        }
        m_HDAuxAdvEncType = (EncodeType_t)HDAuxAdvEncType;
        pCConfig->SetValue(HDAuxCoderCfgSection, key, m_HDAuxAdvEncType);
	}
	
    return s32Ret;
}

int CCoder::GetAuxHDAdvEnc(int *pAuxAdvEncType, int *pHDAuxAdvEncType)
{
    *pAuxAdvEncType = m_AuxAdvEncType;
    *pHDAuxAdvEncType = m_HDAuxAdvEncType;

    return 0;
}

PictureFormat_t CCoder::SwitchPicFormat(int PicFormat)
{
    PictureFormat_t SdkPicFormat = ENC_CIF_FORMAT;

    switch (PicFormat)
    {
        case eResolutionIndex_CIF:      
            SdkPicFormat = ENC_CIF_FORMAT; break;
        case eResolutionIndex_D1:       
            SdkPicFormat = ENC_D1_FORMAT; break;  
        case eResolutionIndex_640x360:  
            SdkPicFormat = ENC_360P_FORMAT; break;
        default :
            printf("Sub stream Not support Picture Format %d\n", PicFormat); SdkPicFormat = ENC_CIF_FORMAT; break;
    }

    return SdkPicFormat;
}

int CCoder::TUTK_GetMedia(int *whichMedia)
{
	*whichMedia = tutkUseWhitchStream;
	return 0;
}

tutk_boardinfo_t boardid_quality_para[] = {

	{
		HS610_G85T_BOARD_ID,
		{{1, 16,  10,  512,  2},	//高清
		{2,	  4,  10,  384,  2},
		{3,	  4,  25,  256,  2},
		{4,	 10,  15,  128,  2},
		{5,	 16,  10,  256,  2}},	//标清
		{{1, 2},
		{2,	 2},
		{3,	 2},
		{4,	 3},
		{5,	 4}},
	},
    {
        HS610_W85T_BOARD_ID,
        {{1, 16,  10,  512,  2},    //高清                                                                                                                            
        {2,   4,  10,  384,  2},
        {3,   4,  25,  256,  2},
        {4,  10,  15,  128,  2},
        {5,  16,  10,  256,  2}},   //标清
        {{1, 2},
        {2,  2},
        {3,  2},
        {4,  3},
        {5,  4}},
    },
	{
		HS608_P4_BOARD_ID,
		{{1, 16,  12,  500,  2},	//高清
		{2,	  4,  12,  384,  2},
		{3,	  4,  25,  256,  2},
		{4,	 10,  15,  128,  2},
		{5,	 16,  12,  250,  2}},	//标清
		{{1, 2},
		{2,	 2},
		{3,	 2},
		{4,	 3},
		{5,	 4}},
	},
	{
		HS608_P3MU_BOARD_ID,
		{{1, 16,  12,  676,  2},	//高清
		{2,	  4,  12,  384,  2},
		{3,	  4,  25,  256,  2},
		{4,	 10,  15,  128,  2},
		{5,	 16,  12,  256,  2}},	//标清
		{{1, 2},
		{2,	 2},
		{3,	 2},
		{4,	 3},
		{5,	 4}},
	},
	{
		HS608_G43TA_BOARD_ID,
		{{1, 16,  10,  512,  2},	//高清
		{2,	  4,  10,  384,  2},
		{3,	  4,  25,  256,  2},
		{4,	 10,  15,  128,  2},
		{5,	 10,  10,  256,  2}},	//标清
		{{1, 2},
		{2,	 2},
		{3,	 2},
		{4,	 3},
		{5,	 4}},
	},
	{
		HS608_P4U_BOARD_ID,
		{{1, 16,  12,  500,  2},	//高清
		{2,	  4,  12,  384,  2},
		{3,	  4,  25,  256,  2},
		{4,	 10,  15,  128,  2},
		{5,	 16,  12,  250,  2}},	//标清
		{{1, 2},
		{2,	 2},
		{3,	 2},
		{4,	 3},
		{5,	 4}},
	},
	{
		HS610_G84F_BOARD_ID,
		{{1, 16,  10,  512,  2},	//高清
		{2,	  4,  10,  384,  2},
		{3,	  4,  25,  256,  2},
		{4,	 10,  15,  128,  2},
		{5,	 16,  10,  256,  2}},	//标清
		{{1, 2},
		{2,	 2},
		{3,	 2},
		{4,	 3},
		{5,	 4}},
	},
	{
		HS610_W84F_BOARD_ID,
		{{1, 16,  10,  512,  2},	//高清
		{2,	  4,  10,  384,  2},
		{3,	  4,  25,  256,  2},
		{4,	 10,  15,  128,  2},
		{5,	 16,  10,  256,  2}},	//标清
		{{1, 2},
		{2,	 2},
		{3,	 2},
		{4,	 3},
		{5,	 4}},
	},
	{
		HS610_G63F_BOARD_ID,
		{{1, 16,  10,  512,  2},	//高清
		{2,   4,  10,  384,  2},
		{3,   4,  25,  256,  2},
		{4,  10,  15,  128,  2},
		{5,  16,  10,  256,  2}},	//标清
		{{1, 2},
		{2,  2},
		{3,  2},
		{4,  3},
		{5,  4}},
	},
	{
		HS610_W63F_BOARD_ID,
		{{1, 16,  10,  512,  2},	//高清
		{2,   4,  10,  384,  2},
		{3,   4,  25,  256,  2},
		{4,  10,  15,  128,  2},
		{5,  16,  10,  256,  2}},	//标清
		{{1, 2},
		{2,  2},
		{3,  2},
		{4,  3},
		{5,  4}},
	},


};

/* 根据分辨率获取尺寸 */
static int AvlResolutionToSize(eResolutionIndex eResolution, int *width, int *heigh)
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
		case eResolutionIndex_800X448 : *width = 800;  *heigh = 448;  break;    
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

#define NUM_OF_BOARD   (sizeof(boardid_quality_para) / sizeof(tutk_boardinfo))
int	CCoder::TUTK_SetQualityCfg(int tutkQuality,tagEncCfg *info)
{
	char key[100];
	int nFrameRate, nBitRate, byResolution;
	int nBoardID;
	int wifiIfExist=0;
	tagEncCfg Info;
	ULONG iItemIndex = 0;
	ULONG LevelIndex = 0;
	int streamnum = 0;//流的个数
    int width = 0, height = 0;
    
	ZWSys_GetBoardInfo(&nBoardID);

	AVL_DBG("[%s] Set tutk quality is %d\n",__FUNCTION__,tutkQuality);
	if(tutkQuality>QualitylevelAll)//高清的质量是6
	{
		AVL_DBG("%d Quality don't in table that use mainstream!\n",tutkQuality);
		tutkUseWhitchStream = 1;	//传输主码流	
		return 0;					//不保存主码流，也不设置编码配置		
	}
	else
	{
		for(iItemIndex = 0; iItemIndex < NUM_OF_BOARD; iItemIndex++)		//find board 
		{
			if (nBoardID == boardid_quality_para[iItemIndex].nboardID)
			{
				break;
			}
		}
		
		
		for(LevelIndex = 0; LevelIndex < QualitylevelAll; LevelIndex++)		//match tutkQuality
		{
			if (tutkQuality == boardid_quality_para[iItemIndex].coder_para[LevelIndex].nQuality_level)
			{
				break;
			}
		}
        
		m_tutkQuality = tutkQuality;
		//根据实际的映射表
		tutkUseWhitchStream = boardid_quality_para[iItemIndex].coder_para[LevelIndex].tutkUseWhitchStream;		
		byResolution = boardid_quality_para[iItemIndex].coder_para[LevelIndex].byResolution;
		nFrameRate = boardid_quality_para[iItemIndex].coder_para[LevelIndex].nFrameRate;
		nBitRate = boardid_quality_para[iItemIndex].coder_para[LevelIndex].nBitRate;		

		if(tutkQuality <= 3)//质量等级为清晰以上
		{
			streamnum = boardid_quality_para[iItemIndex].coder_para[4].tutkUseWhitchStream;//最低质量与流个数有对应关系
			if(3 == streamnum)//支持第三码流
			{
				tutkUseWhitchStream = 3;
				if (nBoardID == 0x2063 || nBoardID == 0x2069 || nBoardID == 0x2072 || nBoardID == 0x2071 \
					|| nBoardID == 0x207E || nBoardID == 0x2094 || nBoardID == 0x2095 || nBoardID == 0x2092)
				{
					byResolution = eResolutionIndex_640x360;	//修改为360P	
				}
				else if (nBoardID == 0x207D)
				{
					byResolution = eResolutionIndex_720P;
					tutkUseWhitchStream = 2;
				}
				else
				{
					byResolution = eResolutionIndex_D1;	//修改为D1
				}
			}
			else	//只有两个流
			{
				//分辨率0-CIF, 1-D1 ,2 2CIF , 3 DCIF, 4 MD 5QCIF
				if(-1 != isSupport(eResolutionIndex_800X448))//子码流支持D1
				{
					tutkUseWhitchStream = 2;
					byResolution = eResolutionIndex_800X448;	//修改为D1					
				}
				else if(-1 != isSupport(eResolutionIndex_D1))//子码流支持D1
				{
					tutkUseWhitchStream = 2;
					byResolution = eResolutionIndex_D1;	//修改为D1					
				}
				else if(-1 != isSupport(eResolutionIndex_640x360))//支持360P
				{
					tutkUseWhitchStream = 2;
					byResolution = eResolutionIndex_640x360;	//修改为360
				}	
				else
				{
					printf("Use boardid_quality_para table\n");
				}
			}
		}
		else if((tutkQuality > 3)&&(tutkQuality <= 5))
		{
			printf("Use boardid_quality_para table\n");
		}
	}
	
#ifdef USE_HDAUX
	if(wifiIfExist)
	{
		tutkUseWhitchStream = 2;
	}
	else
	{
		tutkUseWhitchStream = boardid_quality_para[iItemIndex].coder_para[LevelIndex].tutkUseWhitchStream;
	}	

	AVL_DBG("tutkQuality %d \t iItemIndex %lu\n",tutkQuality,iItemIndex);
	AVL_DBG("levelindex %lu \t FrameRate %d\n",\
	LevelIndex,boardid_quality_para[iItemIndex].coder_para[LevelIndex].nFrameRate);
	AVL_DBG("Borad ID %d,Resolution %d, FrameRate %d, BitRate %d, tutkUseWhitchStream %d\n",\
	boardid_quality_para[iItemIndex].nboardID,\
	byResolution,\
	nFrameRate,\
	nBitRate,\
	tutkUseWhitchStream\
	);
	
	if(3 == tutkUseWhitchStream)
	{
		AVL_DBG("HDAuxCoder_SetEncCfg ___%s\t%d____\n",__FILE__,__LINE__);
		memcpy(&Info, &m_HDCodecParam, sizeof(tagEncCfg));
		Info.dwVideoFrameRate = nFrameRate;
		Info.dwVideoBitrateH = nBitRate;
		Info.byResolution = (eResolutionIndex)byResolution;

        AvlResolutionToSize((eResolutionIndex)byResolution, &width, &height);
        Info.dwResWidth = width;
        Info.dwResHeight = height;

		if(0 == Info.byBitrateType)
		{
			Info.dwQuotiety = boardid_quality_para[iItemIndex].vbrLevel_para[LevelIndex].picQualityLevel;
		}
		HDAuxCoder_SetEncCfg(&Info);		
	}
	else
	{
		AVL_DBG("AuxCoder_SetFormat ___%s\t%d____\n",__FILE__,__LINE__);

		if(0 == AuxCoder_GetBitrateType())
		{
			AuxCoder_SetQuality(boardid_quality_para[iItemIndex].vbrLevel_para[LevelIndex].picQualityLevel);
		}
		AuxCoder_SetFrameRate(nFrameRate);
		AuxCoder_SetBitRate(nBitRate);
		AvlResolutionToSize((eResolutionIndex)byResolution, &width, &height);
		AuxCoder_SetFormat(byResolution, width, height);	
	}
#else
	if(0 == AuxCoder_GetBitrateType())
	{
		AuxCoder_SetQuality(boardid_quality_para[iItemIndex].vbrLevel_para[LevelIndex].picQualityLevel);
	}
	AuxCoder_SetFrameRate(nFrameRate);
	AuxCoder_SetBitRate(nBitRate);
    AvlResolutionToSize((eResolutionIndex)byResolution, &width, &height);
    AuxCoder_SetFormat(byResolution, width, height);    
	tutkUseWhitchStream = 2;
#endif

	AVL_DBG("[%s]tutkUseWhitchStream = %d byResolution=%d\n",__FUNCTION__,tutkUseWhitchStream,byResolution);
	snprintf(key, sizeof(key), "tutkUseWhitchStream%02d", m_iChannel);
	pCConfig->SetValue(AuxCoderCfgSection, key, tutkUseWhitchStream);

	snprintf(key, sizeof(key), "tutkQuality%02d", m_iChannel);
	pCConfig->SetValue(AuxCoderCfgSection, key, m_tutkQuality);
	
	return 0;
}

int	CCoder::TUTK_GetQualityCfg(int *tutkQuality)
{
	*tutkQuality = m_tutkQuality;

	return 0;
}

int	CCoder::TUTK_GetQualityboardCfg(int count, int *dwStreamType)
{
	ULONG iItemIndex = 0;	
	int nBoardID = 0;
	
	ZWSys_GetBoardInfo(&nBoardID);
	for(iItemIndex = 0; iItemIndex < NUM_OF_BOARD; iItemIndex++)		//find board 
	{
		if (nBoardID == boardid_quality_para[iItemIndex].nboardID)
		{
			break;
		}
	}

	*dwStreamType = boardid_quality_para[iItemIndex].coder_para[count-1].tutkUseWhitchStream;
	if(count > QualitylevelAll)
	{
		TRACE_AVL("---TUTK_GetQualityCfg---map failed!-- line %d-\n" ,__LINE__);
		return -1;
	}
	
	return 0;
}

int CCoder::LoadParam()
{
	char key[100] = {0};
	char value[100] = {0};
	//默认永远是关闭的
	m_bAuxEnable = 0;			//pCConfig->GetValue(AuxCoderCfgSection,key,0);
	//辅码流参数
	m_CodecParam.BitRateCtlMode = 1;		//定码率
	BoardSensorType_t Sensortype;
	char szSectionComm[30] = {0};
	int ivalue_sensor;
	int nBoardID = 0;
	char boardIDSection[24] = {0};
	int SecondStreamDefault = 0;
	int tmp;
	tagEncCfg Info;
	int nRet = 0;

    int nCurFormat    = 0;
    int nWidth = 0, nHeight = 0;
    char acResolution[32] = {0};
    int nMaxFramerate = 0;
    int nMaxBitrate   = 0;

	int i, j;
	tagAudioAlarmSchedule *cfg;
	
	//根据版型来修改默认配置参数,为了解决不同版型默认编码参数不同的问题
	ZWSys_GetBoardInfo(&nBoardID);
	memset(boardIDSection, 0, sizeof(boardIDSection));
	snprintf(boardIDSection, sizeof(boardIDSection), "BoardID-%x", nBoardID);
	SecondStreamDefault = pCConfig->GetValue(boardIDSection, "SecondStreamDefault", 0);
	if(1 == SecondStreamDefault)
	{
		pCConfig->SetValue(boardIDSection, "SecondStreamDefault", 0);
		TRACE_AVL("\n[software] [%s-%d]===second stream enc goto default==\n",__FUNCTION__,__LINE__);
		
		snprintf(key, sizeof(key), "DeAuxCoderFrameRate%02d", m_iChannel);
		tmp = pCConfig->GetValue(boardIDSection, key, 255);
		snprintf(key, sizeof(key), "AuxCoderFrameRate%02d", m_iChannel);
		pCConfig->SetValue(AuxCoderCfgSection, key, tmp);

		snprintf(key, sizeof(key), "DeAuxCoderKeyFrameInterval%02d", m_iChannel);
		tmp = pCConfig->GetValue(boardIDSection, key, 80);
		snprintf(key, sizeof(key), "AuxCoderKeyFrameInterval%02d", m_iChannel);
		pCConfig->SetValue(AuxCoderCfgSection, key, tmp);

		snprintf(key, sizeof(key), "DeAuxCoderBitRate%02d", m_iChannel);
		tmp = pCConfig->GetValue(boardIDSection, key, 1024);
		snprintf(key, sizeof(key), "AuxCoderBitRate%02d", m_iChannel);
		pCConfig->SetValue(AuxCoderCfgSection, key, tmp);

		snprintf(key, sizeof(key), "DeAuxCoderFormat%02d", m_iChannel);
		tmp = pCConfig->GetValue(boardIDSection, key, 10);
		snprintf(key, sizeof(key), "AuxCoderFormat%02d", m_iChannel);
		pCConfig->SetValue(AuxCoderCfgSection, key, tmp);

		snprintf(key, sizeof(key), "DeAuxCoderFormatWidth%02d", m_iChannel);
		tmp = pCConfig->GetValue(boardIDSection, key, 800);
		snprintf(key, sizeof(key), "AuxCoderFormatWidth%02d", m_iChannel);
		pCConfig->SetValue(AuxCoderCfgSection, key, tmp);

		snprintf(key, sizeof(key), "DeAuxCoderFormatHeight%02d", m_iChannel);
		tmp = pCConfig->GetValue(boardIDSection, key, 448);
		snprintf(key, sizeof(key), "AuxCoderFormatHeight%02d", m_iChannel);
		pCConfig->SetValue(AuxCoderCfgSection, key, tmp);
	}

#ifdef DO_SATA
	m_CodecParam.IQuantVal = m_CodecParam.PQuantVal = m_CodecParam.BQuantVal = 16;
#else
	m_CodecParam.PicQualityLever = 3;
#endif

	m_CodecParam.PictureFormat = ENC_CIF_FORMAT;
	
	ZWSys_GetSensorType(&Sensortype);
	snprintf(szSectionComm, sizeof(szSectionComm),"ISP_S%d",Sensortype);
	ivalue_sensor = pCConfig->GetValue(szSectionComm, "InputFrequency", 0);
	if (0 == ivalue_sensor)
		m_SensorFrequency = CMOS_FREQUENCY_50HZ;
	else
	{
		if (1 == ivalue_sensor)
			m_SensorFrequency = CMOS_FREQUENCY_60HZ;
		else
			m_SensorFrequency = CMOS_FREQUENCY_50HZ;
	}

	snprintf(key, sizeof(key), "AuxCoderFrameRate%02d", m_iChannel);
	m_CodecParam.FrameRate = pCConfig->GetValue(AuxCoderCfgSection, key, 15);
	snprintf(key, sizeof(key), "AuxCoderBitRate%02d", m_iChannel);
	m_CodecParam.BitRate = pCConfig->GetValue(AuxCoderCfgSection, key, 32) * 1000;//<< 10;

	snprintf(key, sizeof(key), "AuxCoderFormatWidth%02d", m_iChannel);
	m_CodecParam.PictureRes.m_PicWidth = pCConfig->GetValue(AuxCoderCfgSection, key, 800);

	snprintf(key, sizeof(key), "AuxCoderFormatHeight%02d", m_iChannel);
	m_CodecParam.PictureRes.m_PicHeight = pCConfig->GetValue(AuxCoderCfgSection, key, 448);

	memset(key, 0, sizeof(key));
	snprintf(key, sizeof(key), "AuxCoderQuptiety%02d", m_iChannel);
	m_CodecParam.PicQualityLever = pCConfig->GetValue(AuxCoderCfgSection, key, 1);

	memset(key, 0, sizeof(key));
	snprintf(key, sizeof(key), "AuxCoderStreamType%02d", m_iChannel);
	m_CodecStreamType = pCConfig->GetValue(AuxCoderCfgSection, key, 0);

	memset(key, 0, sizeof(key));
	snprintf(key, sizeof(key), "AuxCoderKeyFrameInterval%02d", m_iChannel);
	m_CodecParam.KeyFrameInterval = pCConfig->GetValue(AuxCoderCfgSection, key, 40);

	memset(key, 0, sizeof(key));
	snprintf(key, sizeof(key), "AuxCoderBitrateType%02d", m_iChannel);
	m_CodecParam.BitRateCtlMode = pCConfig->GetValue(AuxCoderCfgSection, key, 1);

    /* 从XML里面获取能力，看是否支持 */
    nRet = GetSupportMaxFramerate(1);
	if(-1 != nRet) 
	{
		nMaxFramerate = nRet;
    	m_CodecParam.FrameRate = m_CodecParam.FrameRate > nMaxFramerate ? nMaxFramerate : m_CodecParam.FrameRate;
	}

	nRet = GetSupportMaxBitrate(1);
	if(-1 != nRet)
	{
		nMaxBitrate = 1000 * nRet;
	    m_CodecParam.BitRate = m_CodecParam.BitRate > nMaxBitrate ? nMaxBitrate : m_CodecParam.BitRate;		
	}

    /* 分辨率 */
    snprintf(acResolution, sizeof(acResolution), "%dX%d", m_CodecParam.PictureRes.m_PicWidth, m_CodecParam.PictureRes.m_PicHeight);
    
	nRet = IsSupportDeviceAbility(1, 1, 0, acResolution);
	if(100 != nRet)
	{	    
        GetDeviceDefaultAbility(1, 1, &nCurFormat, &nWidth, &nHeight);
        printf("Not Support Resolution %d:%d! Use default%d:%d(Aux Stream)!\n", m_CodecParam.PictureRes.m_PicWidth, m_CodecParam.PictureRes.m_PicHeight, nWidth, nHeight);
        
        m_CodecParam.PictureRes.m_PicWidth = nWidth;
        m_CodecParam.PictureRes.m_PicHeight = nHeight;
	}

	nRet = IsSupportDeviceAbility(1, 2, m_CodecParam.BitRateCtlMode, NULL);
	if(-1 != nRet)
	{
	    /* 码率类型 */
	    if (m_CodecParam.BitRateCtlMode != nRet)
	    {
	        GetDeviceDefaultAbility(1, 2, &m_CodecParam.BitRateCtlMode, 0, 0);
	        AVL_ERR("Not Support BitrateType! Use default %d(Aux Stream)!\n", m_CodecParam.BitRateCtlMode);
	    }
	}

	nRet = IsSupportDeviceAbility(1, 3, m_CodecStreamType, NULL);
	if(-1 != nRet)
	{
	    /* 流类型 */
	    if (m_CodecStreamType != nRet)
	    {
	        GetDeviceDefaultAbility(1, 3, &m_CodecStreamType, 0, 0);
	        AVL_ERR("Not Support StreamType! Use default %d(Aux Stream)!\n", m_CodecStreamType);
	    }	
	}
	m_CodecParam.KeyFrameInterval = m_CodecParam.FrameRate;//关键帧间隔等于帧率
	if (m_CodecParam.KeyFrameInterval == 0xFF)
	{
		if (m_SensorFrequency == CMOS_FREQUENCY_50HZ)
		{
			m_CodecParam.KeyFrameInterval = 25;
		}
		else
		{
			if (m_SensorFrequency == CMOS_FREQUENCY_60HZ)
				m_CodecParam.KeyFrameInterval = 30;
			else
				m_CodecParam.KeyFrameInterval = 25;
		}
		m_CodecParam.FrameRate=m_CodecParam.KeyFrameInterval;
	}
#ifdef USE_HDAUX
	m_bHDAuxEnable = 0;
	snprintf(key, sizeof(key), "HDAuxEncCfg%02d", m_iChannel);

	pCConfig->GetValue(HDAuxCoderCfgSection, key, DefHDAuxEncCfgStr, value, sizeof(value));
	sscanf(value, "%d:%d:%d:%d:%d:%d:%d:%d",
		   (int *)&Info.byResolution, &Info.dwVideoFrameRate, &Info.byStreamType, &Info.byBitrateType,
		   &Info.byPicQuality, &Info.dwQuotiety, &Info.dwVideoBitrateH, &Info.dwVideoBitrateL);

	snprintf(key, sizeof(key), "HDAuxCoderFormatWidth%02d", m_iChannel);
	Info.dwResWidth = pCConfig->GetValue(HDAuxCoderCfgSection, key, 640);

	snprintf(key, sizeof(key), "HDAuxCoderFormatHeight%02d", m_iChannel);
	Info.dwResHeight = pCConfig->GetValue(HDAuxCoderCfgSection, key, 360);

	if (Info.dwVideoFrameRate != 0xFF && (Info.dwVideoFrameRate < 1 || Info.dwVideoFrameRate > 29))
	{
		Info.dwVideoFrameRate = 0xFF;

	}
	if (Info.dwQuotiety < 0 || Info.dwQuotiety > 5)
	{
		Info.dwQuotiety = 5;
	}
	if (Info.byStreamType > 1 || Info.byStreamType < 0)
	{
		Info.byStreamType = 1;
	}
	if (Info.byPicQuality > 3 || Info.byPicQuality < 0)
	{
		Info.byPicQuality = 1;
	}
	if (Info.byBitrateType > 2 || Info.byBitrateType < 0)
	{
		Info.byBitrateType = 1;
	}
	
	memcpy(&m_HDCodecParam, &Info, sizeof(tagEncCfg));
#endif

	//从配置文件中获取传输哪个流
	unsigned int iItemIndex = 0;
	int tutkUseWhitchStreamRead = 0;
	snprintf(key, sizeof(key), "tutkUseWhitchStream%02d", m_iChannel);
	tutkUseWhitchStreamRead = pCConfig->GetValue(AuxCoderCfgSection, key, 2);	
	
	for(iItemIndex = 0; iItemIndex < NUM_OF_BOARD; iItemIndex++)		//find board 
	{
		if (nBoardID == boardid_quality_para[iItemIndex].nboardID)
		{
			break;
		}
	}

	tutkUseWhitchStream = boardid_quality_para[iItemIndex].coder_para[4].tutkUseWhitchStream; //最低等级tutkUseWhitchStream的值最大
	if((tutkUseWhitchStreamRead <= tutkUseWhitchStream)&&(tutkUseWhitchStreamRead > 0)) //表示tutkUseWhitchStreamRead在能力范围内
	{
		tutkUseWhitchStream = tutkUseWhitchStreamRead;
	}

	snprintf(key, sizeof(key), "tutkQuality%02d", m_iChannel);
	m_tutkQuality = pCConfig->GetValue(AuxCoderCfgSection, key, 5);

    snprintf(key, sizeof(key), "AuxFormatType%02d", m_iChannel);
    m_AuxAdvEncType = (EncodeType_t)pCConfig->GetValue(AuxCoderCfgSection, key, LtH264Video);
    
    snprintf(key, sizeof(key), "HDAuxFormatType%02d", m_iChannel);
    m_HDAuxAdvEncType = (EncodeType_t)pCConfig->GetValue(HDAuxCoderCfgSection, key, LtH264Video);

    // 关键帧间隔
	snprintf(key, sizeof(key), "MainCoderKeyFrameInterval%02d", m_iChannel);
    m_nMainKeyFrameInterval = pCConfig->GetValue(MainCoderCfgSection, key, 40);

	snprintf(key, sizeof(key), "AuxCoderKeyFrameInterval%02d", m_iChannel);
    m_nAuxKeyFrameInterval = pCConfig->GetValue(AuxCoderCfgSection, key, 40);

	snprintf(key, sizeof(key), "HDAuxEncCfgKeyFrameInterval%02d", m_iChannel);
    m_nHDAuxKeyFrameInterval = pCConfig->GetValue(HDAuxCoderCfgSection, key, 40);

   	m_audioAlarmCfg.enable = pCConfig->GetValue(AudioAlarmCfgSection, "enable", 0);
    
	m_audioAlarmCfg.alarmOutDelay = pCConfig->GetValue(AudioAlarmCfgSection, "audioDelayOutput", 5);
	m_audioAlarmCfg.broadCastVolume = pCConfig->GetValue(AudioAlarmCfgSection, "audioVolume", 50);
	m_audioAlarmCfg.alarmSpeechType = pCConfig->GetValue(AudioAlarmCfgSection, "audioType", 1);
	m_audioAlarmCfg.broadCastTimes = pCConfig->GetValue(AudioAlarmCfgSection, "iAudioPlayTimes", 1);
	
	if (access("/mnt/mtd/config/AudioOut_95",F_OK) != 0 || m_audioAlarmCfg.broadCastVolume == 50)
	{
		mysystem("touch /mnt/mtd/config/AudioOut_95");
		m_audioAlarmCfg.broadCastVolume = 95;
		pCConfig->SetValue(AudioAlarmCfgSection, "audioVolume", 95);
		printf(" SET audioVolume = 95\n");
	}

	ZWSys_SetAlarmSpeechCfg(m_henc, &m_audioAlarmCfg);

	/* 设置音频输入音量 */
	m_nAudioInVol = pCConfig->GetValue("AudioIn", "AudioVol", 80);
	ZWSys_SetAudioInVol(m_henc, m_nAudioInVol);

    /* 设置音频输出音量 */
    m_nAudioOutVol = pCConfig->GetValue("AudioOut", "AudioVol", 80);
	ZWSys_SetAudioOutVol(m_henc, m_nAudioOutVol, 1);

	cfg = &m_AudioAlarmScheduleCfg;

	cfg->bytState = pCConfig->GetValue(AudioAlarmCfgSection, "ScheduleCfgEnable", 0);//自定义、全天
	cfg->nDayMode = pCConfig->GetValue(AudioAlarmCfgSection, "ScheduleCfgEnableDN", 2);//白天，夜晚
	for (i = 0; i < 7; i++)
	{
		snprintf(key, sizeof(key), "ScheduleCfg_%01d", i);
		pCConfig->GetValue(AudioAlarmCfgSection, key, SCHEDULE_TIME_ONE, value, sizeof(value));
		for (j = 0; j < 48; j++)
		{
			cfg->dwTimeSeg[i][j] = value[j] - '0';
		}
	}

	return 0;
}

//##ModelId=49BDEDDB0198
int CCoder::SetAlarmCallBack(int AlarmType, int (*CallBack) (struct tagAlertInfo * pAlertInfo, struct tagActionInfo * pActionInfo))
{
	if (CallBack == NULL)
	{
		AVL_ERR("[CCoder:%s]CallBack==NULL\n", __FUNCTION__);
		return -1;
	}
	m_AlarmType = AlarmType;
	m_FxnCallBack = CallBack;
	return (int) 0;
}

int CCoder::CheckCoderError(int bError)
{
	if (!bError)
	{
		return 0;
	}
	
	return m_pActionAlarm->CheckAction(0,m_AlarmType, m_FxnCallBack, 0);
}

#ifdef USE_HDAUX
//!\brief   开启高清子码流编码
//!\return  成功0,其它失败
int CCoder::StartHDAuxEncCapture()
{
	return 0;
}

//!\brief   停止高清子码流编码
//!\return  成功0,其它失败
int CCoder::StopHDAuxEncCapture()
{
	return 0;
}

//!\brief   启用/不启用高清子码流
//!\param   int bEnable false为不启用，true为启用
//!\return  成功为0，失败为-1
int CCoder::HDAuxCoder_Enable(int bEnable)
{
	m_bHDAuxEnable = bEnable;
	return 0;
}

//!\brief   判断是否启用高清子码流
//!\return  false为不启用，true为启用
int CCoder::HDAuxCoder_IsEnable()
{
	return m_bHDAuxEnable;
}

//!\brief   设置高清子码流参数
//!\return  成功为0，失败为-1
int CCoder::HDAuxCoder_SetEncCfg(tagEncCfg * Info)
{
	int result = 0;
	char key[100] = {0};
	char value[100] = {0};
	BoardSensorType_t Sensortype;
	char szSectionComm[30] = {0};
	int ivalue_sensor;
	tagEncCfg tmp_info;
	DeviceDescription ddt;
	int nBoardID = 0;
	CODEC_PARA_S HDCodecParam;
	char acResolution[32] = {0};

	if (NULL == Info)
	{
		AVL_ERR("[CRecord:%s]Info==NULL\n", __FUNCTION__);
		return -1;
	}

	TRACE_AVL("[CCoder:%s]HDAuxCoder_SetEncCfg --frameRate=%d--bitRate=%d--Quotiety=%d--resolution=%d--StreamType=%d--BitrateType=%d--\n",
		__FUNCTION__, Info->dwVideoFrameRate,Info->dwVideoBitrateH,Info->dwQuotiety,Info->byResolution,
		Info->byStreamType,Info->byBitrateType);

	// 判断设置下来的分辨率是否支持
	snprintf(acResolution, sizeof(acResolution), "%dX%d", Info->dwResWidth, Info->dwResHeight);
	result = IsSupportDeviceAbility(2, 1, 0, acResolution);
	if(100 != result)
	{
		fprintf(stderr, "[%s][%d] HDAux unsupport format.w=%d h=%d*****\r\n",__FUNCTION__,__LINE__, Info->dwResWidth, Info->dwResHeight);
		return -1;
	}

 	if(1 == bSupportRtmp)
	{
		if(Info->dwVideoFrameRate < 15)
			Info->dwVideoFrameRate = 15;
	} 
	
	memcpy(&tmp_info, Info, sizeof(tagEncCfg));

	if (Info->dwVideoFrameRate != 0xFF && (Info->dwVideoFrameRate < 1 || Info->dwVideoFrameRate > 30))
	{
		AVL_ERR("[CRecord:%s]Info->dwVideoFrameRate=%d\n", __FUNCTION__, Info->dwVideoFrameRate);
		return -1;
	}
	if (Info->dwQuotiety < 0 || Info->dwQuotiety > 5)
	{
		AVL_ERR("[CRecord:%s]Info->dwQuotiety=%d\n", __FUNCTION__, Info->dwQuotiety);
		return -1;
	}
	if (Info->byStreamType > 1 || Info->byStreamType < 0)
	{
		AVL_ERR("[CRecord:%s]Info->byStreamType=%d\n", __FUNCTION__, Info->byStreamType);
		return -1;
	}
	if (Info->byPicQuality > 3 || Info->byPicQuality < 0)
	{
		AVL_ERR("[CRecord:%s]Info->byPicQuality=%d\n", __FUNCTION__, Info->byPicQuality);
		return -1;
	}
	if (Info->byBitrateType > 2 || Info->byBitrateType < 0)
	{
		AVL_ERR("[CRecord:%s]Info->byBitrateType=%d\n", __FUNCTION__, Info->byBitrateType);
		return -1;
	}

	tmp_info.dwVideoBitrateH = (Info->dwVideoBitrateH < 64) ? 64 : Info->dwVideoBitrateH;

	if (nBoardID == 0)
	{
		ZWSys_GetBoardInfo(&nBoardID);
		EDVRS_DDT_GetDeviceDescriptionByID(&ddt, nBoardID);
	}

	if (ddt.bytAudioInChannels <= m_iChannel)
	{
		Info->byStreamType = 0;
	}

	snprintf(key, sizeof(key), "HDAuxEncCfg%02d", m_iChannel);
	snprintf(value, sizeof(value), "%d:%d:%d:%d:%d:%d:%d:%d",
			tmp_info.byResolution, tmp_info.dwVideoFrameRate, tmp_info.byStreamType, tmp_info.byBitrateType,
			tmp_info.byPicQuality, tmp_info.dwQuotiety, tmp_info.dwVideoBitrateH, tmp_info.dwVideoBitrateL);
	result = pCConfig->SetValue(HDAuxCoderCfgSection, key, value);
	if (result == 0 && pCConfig->m_bReadOnlyCfgFlag == 1)
	{
		pCConfig->GetValue(HDAuxCoderCfgSection, key, "", value, sizeof(value));
		sscanf(value, "%d:%d:%d:%d:%d:%d:%d:%d",
				(int *)&tmp_info.byResolution, &tmp_info.dwVideoFrameRate, &tmp_info.byStreamType, &tmp_info.byBitrateType,
				&tmp_info.byPicQuality, &tmp_info.dwQuotiety, &tmp_info.dwVideoBitrateH, &tmp_info.dwVideoBitrateL);
		sscanf(value, "%d:%d:%d:%d:%d:%d:%d:%d",
				(int *)&Info->byResolution, &Info->dwVideoFrameRate, &Info->byStreamType, &Info->byBitrateType,
				&Info->byPicQuality, &Info->dwQuotiety, &Info->dwVideoBitrateH, &Info->dwVideoBitrateL);
	}

	snprintf(key, sizeof(key), "HDAuxCoderFormatWidth%02d", m_iChannel);
	result = pCConfig->SetValue(HDAuxCoderCfgSection, key, Info->dwResWidth);
	if (result == 0 && pCConfig->m_bReadOnlyCfgFlag == 1)
	{
		Info->dwResWidth = pCConfig->GetValue(HDAuxCoderCfgSection, key, 0);
	}

	snprintf(key, sizeof(key), "HDAuxCoderFormatHeight%02d", m_iChannel);
	result = pCConfig->SetValue(HDAuxCoderCfgSection, key, Info->dwResHeight);
	if (result == 0 && pCConfig->m_bReadOnlyCfgFlag == 1)
	{
		Info->dwResHeight = pCConfig->GetValue(HDAuxCoderCfgSection, key, 0);
	}

	//分辨率转换
	switch (tmp_info.byResolution)
	{
		case eResolutionIndex_CIF: 
			HDCodecParam.PictureFormat = ENC_CIF_FORMAT;
			break;
		case eResolutionIndex_D1: 
			HDCodecParam.PictureFormat = ENC_D1_FORMAT;
			break;
		case eResolutionIndex_640x360: 
			HDCodecParam.PictureFormat = ENC_360P_FORMAT;
			break;

		default:
			TRACE_AVL("HDAux only support CIF!\n");
			HDCodecParam.PictureFormat = ENC_CIF_FORMAT;
	}

	ZWSys_GetSensorType(&Sensortype);
	snprintf(szSectionComm, sizeof(szSectionComm),"ISP_S%d",Sensortype);
	ivalue_sensor = pCConfig->GetValue(szSectionComm, "InputFrequency", 0);
	if (1 == ivalue_sensor)
		m_SensorFrequency = CMOS_FREQUENCY_60HZ;
	else
		m_SensorFrequency = CMOS_FREQUENCY_50HZ;
	
	HDCodecParam.FrameRate = tmp_info.dwVideoFrameRate;
	HDCodecParam.KeyFrameInterval = HDCodecParam.FrameRate;
	if (HDCodecParam.KeyFrameInterval == 0xFF)
	{
		if (m_SensorFrequency == CMOS_FREQUENCY_50HZ)
			HDCodecParam.KeyFrameInterval = 25;
		else
			HDCodecParam.KeyFrameInterval = 30;
	}

	
	HDCodecParam.BitRate = (tmp_info.dwVideoBitrateH < 64) ? 64 : tmp_info.dwVideoBitrateH;
	HDCodecParam.BitRate = HDCodecParam.BitRate * 1000;//<< 10;
	HDCodecParam.PicQualityLever = Info->dwQuotiety;
	HDCodecParam.BitRateCtlMode = Info->byBitrateType;

	HDCodecParam.PictureRes.m_PicWidth = Info->dwResWidth;
	HDCodecParam.PictureRes.m_PicHeight = Info->dwResHeight;

	result = ZWSys_EncSetCodecPara(m_henc, 2, &HDCodecParam, m_HDAuxAdvEncType);

	if (result)
	{
		AVL_ERR("[CCoder:%s]call ZWSys_EncSetCodecPara failed(%d)\n", __FUNCTION__, result);
		return result;
	}
	memcpy(&m_HDCodecParam, Info, sizeof(tagEncCfg));

	return (int) result;
}


//!\brief   获取高清子码流参数
//!\return  成功为0，失败为-1
int CCoder::HDAuxCoder_GetEncCfg(tagEncCfg * Info)
{
	int exist=0;
	int stream=0;
	if (NULL == Info)
	{
		AVL_ERR("[CRecord:%s]Info==NULL\n", __FUNCTION__);
		return -1;
	}

	ZWSys_GetStreamCount(m_henc, &stream);

	if(!(stream & (1<<2)))
		return 1;//无第二子码流
	
	ZWSys_GetWifiExist(&exist);
	if(exist)
		return 1;

	memcpy(Info, &m_HDCodecParam, sizeof (tagEncCfg));
	
	TRACE_AVL("\n[software] [%s-%d]=HDAux==GetEncCfg====%d:%d:%d:%d:%d:%d:%d:%d=======\n",__FUNCTION__,__LINE__,
				Info->byResolution, Info->dwVideoFrameRate, Info->byStreamType, Info->byBitrateType,
				Info->byPicQuality, Info->dwQuotiety, Info->dwVideoBitrateH, Info->dwVideoBitrateL);
	return (int) 0;
}


int CCoder::FourthStream_SetEncCfg(tagEncCfg * Info)
{
	int result = 0;	
	int ivalue_sensor = 0;
	char key[100] = {0};
	char value[100] = {0};
	char szSectionComm[30] = {0};
	BoardSensorType_t Sensortype;
	tagEncCfg tmp_info;
	CODEC_PARA_S FourthStreamParam;

	if (NULL == Info)
	{
		AVL_ERR("[CRecord:%s]Info==NULL\n", __FUNCTION__);
		return -1;
	}

	printf("[CCoder:%s]FourthStream_SetEncCfg --frameRate=%d--bitRate=%d--Quotiety=%d--resolution=%d--StreamType=%d--BitrateType=%d--\n",
		__FUNCTION__, Info->dwVideoFrameRate,Info->dwVideoBitrateH,Info->dwQuotiety,Info->byResolution,
		Info->byStreamType,Info->byBitrateType);
    
	memcpy(&tmp_info, Info, sizeof(tagEncCfg));

	if (Info->dwVideoFrameRate != 0xFF && (Info->dwVideoFrameRate < 1 || Info->dwVideoFrameRate > 30))
	{
		AVL_ERR("[CRecord:%s]Info->dwVideoFrameRate=%d\n", __FUNCTION__, Info->dwVideoFrameRate);
		return -1;
	}
	if (Info->dwQuotiety < 0 || Info->dwQuotiety > 5)
	{
		AVL_ERR("[CRecord:%s]Info->dwQuotiety=%d\n", __FUNCTION__, Info->dwQuotiety);
		return -1;
	}
	if (Info->byStreamType > 1 || Info->byStreamType < 0)
	{
		AVL_ERR("[CRecord:%s]Info->byStreamType=%d\n", __FUNCTION__, Info->byStreamType);
		return -1;
	}
	if (Info->byPicQuality > 3 || Info->byPicQuality < 0)
	{
		AVL_ERR("[CRecord:%s]Info->byPicQuality=%d\n", __FUNCTION__, Info->byPicQuality);
		return -1;
	}
	if (Info->byBitrateType > 2 || Info->byBitrateType < 0)
	{
		AVL_ERR("[CRecord:%s]Info->byBitrateType=%d\n", __FUNCTION__, Info->byBitrateType);
		return -1;
	}

	tmp_info.dwVideoBitrateH = (Info->dwVideoBitrateH < 64) ? 64 : Info->dwVideoBitrateH;

  
    if (Info->byResolution != IsSupportDeviceAbility(3, 1, Info->byResolution, NULL))
    {
        AVL_ERR("Device not support ability(Resolution)\n");
        return -1;
    }
    if (Info->byBitrateType != IsSupportDeviceAbility(3, 2, Info->byBitrateType, NULL))
    {
        AVL_ERR("Device not support ability(BitrateType)\n");
        return -1;
    }
    if (Info->byStreamType != IsSupportDeviceAbility(3, 3, Info->byStreamType, NULL))
    {
        AVL_ERR("Device not support ability(StreamType)\n");
        return -1;
    }
    if (Info->dwVideoFrameRate > GetSupportMaxFramerate(3))
    {
		AVL_ERR("dwVideoFrameRate %d over MaxFrameRate!\n", Info->dwVideoFrameRate);
        return -1;
    }
    if (Info->dwVideoBitrateH > GetSupportMaxBitrate(3))
    {
		AVL_ERR("dwVideoBitrateH %d over MaxFrameRate!\n", Info->dwVideoBitrateH);
        return -1;
    }
    
	snprintf(key, sizeof(key), "FourthStreamEncCfg%02d", m_iChannel);
	snprintf(value, sizeof(value), "%d:%d:%d:%d:%d:%d:%d:%d",
			tmp_info.byResolution, tmp_info.dwVideoFrameRate, tmp_info.byStreamType, tmp_info.byBitrateType,
			tmp_info.byPicQuality, tmp_info.dwQuotiety, tmp_info.dwVideoBitrateH, tmp_info.dwVideoBitrateL);
	pCConfig->SetValue(HDFourthStreamCfgSection, key, value);

    snprintf(key, sizeof(key), "FourthStreamFormatWidth%02d", m_iChannel);
    pCConfig->SetValue(HDAuxCoderCfgSection, key, Info->dwResWidth);

    snprintf(key, sizeof(key), "FourthStreamFormatHeight%02d", m_iChannel);
    pCConfig->SetValue(HDAuxCoderCfgSection, key, Info->dwResHeight);

	switch (tmp_info.byResolution)
	{
		case eResolutionIndex_QVGA: 
			FourthStreamParam.PictureFormat = ENC_QVGA_FORMAT;
			break;
	    
        case eResolutionIndex_CIF:
			FourthStreamParam.PictureFormat = ENC_CIF_FORMAT;
			break;
	
        case eResolutionIndex_640x360: 
            FourthStreamParam.PictureFormat = ENC_360P_FORMAT;
            break;
            
		case eResolutionIndex_D1: 
			FourthStreamParam.PictureFormat = ENC_D1_FORMAT;
			break;
			
		default:
			AVL_ERR("Fourth Stream not support %d! default: CIF\n", tmp_info.byResolution);
			FourthStreamParam.PictureFormat = ENC_CIF_FORMAT;
	}

    FourthStreamParam.PictureRes.m_PicWidth = Info->dwResWidth;
    FourthStreamParam.PictureRes.m_PicWidth = Info->dwResHeight;

	ZWSys_GetSensorType(&Sensortype);
	snprintf(szSectionComm, sizeof(szSectionComm),"ISP_S%d",Sensortype);
	ivalue_sensor = pCConfig->GetValue(szSectionComm, "InputFrequency", 0);
	if (1 == ivalue_sensor)
		m_SensorFrequency = CMOS_FREQUENCY_60HZ;
	else
		m_SensorFrequency = CMOS_FREQUENCY_50HZ;
	
	FourthStreamParam.FrameRate = tmp_info.dwVideoFrameRate;
	FourthStreamParam.KeyFrameInterval = FourthStreamParam.FrameRate;
	if (FourthStreamParam.KeyFrameInterval == 0xFF)
	{
		if (m_SensorFrequency == CMOS_FREQUENCY_50HZ)
			FourthStreamParam.KeyFrameInterval = 25;
		else
			FourthStreamParam.KeyFrameInterval = 30;
	}

	FourthStreamParam.BitRate = (tmp_info.dwVideoBitrateH < 64) ? 64 : tmp_info.dwVideoBitrateH;
	FourthStreamParam.BitRate = FourthStreamParam.BitRate * 1000;//<< 10;
	FourthStreamParam.PicQualityLever = Info->dwQuotiety;
	FourthStreamParam.BitRateCtlMode = Info->byBitrateType;

//	result = MP4Sys_EncSetCodecPara(m_henc, 2, &FourthStreamParam, m_HDAuxAdvEncType);
	if (result)
	{
		AVL_ERR("[CCoder:%s]call ZWSys_EncSetCodecPara failed(%d)\n", __FUNCTION__, result);
		return result;
	}
	memcpy(&m_FourthStreamParam, Info, sizeof(tagEncCfg));

	return (int) result;
}
int CCoder::FourthStream_GetEncCfg(tagEncCfg * Info)
{
	if (NULL == Info)
	{
		AVL_ERR("[CRecord:%s]Info==NULL\n", __FUNCTION__);
		return -1;
	}

	memcpy(Info, &m_FourthStreamParam, sizeof (tagEncCfg));
	
	printf("\n[software] [%s-%d]=FourthStream==GetEncCfg====%d:%d:%d:%d:%d:%d:%d:%d=======\n",__FUNCTION__,__LINE__,
				Info->byResolution, Info->dwVideoFrameRate, Info->byStreamType, Info->byBitrateType,
				Info->byPicQuality, Info->dwQuotiety, Info->dwVideoBitrateH, Info->dwVideoBitrateL);
	return (int) 0;
}

#endif

