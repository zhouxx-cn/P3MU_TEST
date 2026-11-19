// Copyright (C) 1991 - 1999 Rational Software Corporation

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CCODER_49A2634B035F_INCLUDED
#define _INC_CCODER_49A2634B035F_INCLUDED
#include "global_macro.h"
#include "tagAlertInfo.h"
#include "tagActionInfo.h"
#include "tagEncCfg.h"
#include "CConfig.h"
#ifdef DO_SATA
#include "hisdk_sata.h"
#else
#include "zwsdk.h"
//#include "hisdk.h"
#endif
#include "AVL_global.h"
#include "CActionAlarm.h"
#include "tagMotionSchedule.h"

/*  图像等级总数  */
#define QualitylevelAll 5

/*  图像质量与编码参数对应关系  */
typedef struct quality_coding_para {
	int nQuality_level;
	int byResolution;
	int nFrameRate;
	int nBitRate;
	int	tutkUseWhitchStream;	//传输哪种码流 2:子码流  3:第二子码流
} quality_coding_para_t;

typedef struct quality_in_VBR {
	int nQuality_level;
	int picQualityLevel;		//当编码为变码率时使用的编码率等级
} quality_in_VBR_t;

/*  TUTK与板型对应关系  */
typedef struct tutk_boardinfo {
	int nboardID;
	quality_coding_para_t coder_para[QualitylevelAll];
	quality_in_VBR_t vbrLevel_para[QualitylevelAll];
} tutk_boardinfo_t;

typedef struct tagProtocolSpeechMap{
	/* 0:人形  1：车形,2:火焰,3:报警输入(AlarmIn),4:视频遮挡(VideoShelter),5:移动侦测(Motion),6:电子围栏(ElectronicDence),
	7:越界侦测(CrossBorderDetect),8:离岗侦测(OffDutyDetect),9:哭声叫声(CryScreamAudio),10:客流进入(PassengerIn),
	11:客流离开(PassengerOut),12:智能跟踪(IntelligenceTrack),13:人脸抓拍(FaceCapture),14:口罩检测(MaskDetection) */
	int 			nProtocolSpeechType;
	SpeechType_T 	eAlarmType;
}stuProtocolSpeechMap;

#ifdef DO_SATA
/*#ifndef CODEC_PARA__
#define CODEC_PARA__
typedef struct CODEC_PARA_S_t
{
	PictureFormat_t PictureFormat;	//设置编码格式
	int BitRate;				//Bit率
	int FrameRate;				//帧率
	int IQuantVal;				//量化系数I，CBR时无效
	int PQuantVal;				//量化系数P，CBR时无效
	int BQuantVal;				//量化系数B，CBR时无效
	int BitRateCtlMode;					//码率控制类型：0为VBR，1为CBR
	int KeyFrameInterval;		//关键帧间隔
} CODEC_PARA_S;
#endif
*/
#endif
////////////////////

//处理编码器有关的配置信息
//主要是辅助码流信息
//##ModelId=49A2634B035F
class CCoder
{
  public:
	//!\brief 获取辅助编码参数
	//!\praram int nCh 通道
	//!\praram void* hHandle编码通道句柄
	//!\return  void
	//##ModelId=49A2637500AD
	CCoder(int nCh, void *hHandle);

	 ~CCoder();
	//初始化
	//!\param int nMaxCh, int nMaxAlarmOut分别为最大通道数、最大报警输出数
	int Init(int nMaxCh, int nMaxAlarmOut);

	//释放
	int UnInit();

	//!\brief 开启编码
	//!\return 成功0,其它失败
	int StartAuxEncCapture();

	//!\brief 通知制式
	//!\return 成功0,其它失败
	static int SetStandard(int standard);

	//!\brief 停止编码
	//!\return 成功0,其它失败
	int StopAuxEncCapture();

	//!\brief 强制一个I帧
	//!\return  成功为0，失败为-1
	//##ModelId=49B6224401A7
	int EncCaptureIFrame(int nSensorIndex);

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
	int EnableAction(int Action, int bEnable);

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
	int IsEnableAction(int Action);

	//!\brief 获取某个报警联动录像参数
	//!\param dwChannelFlag 联动的通道标记 
	//每一位表示联动一个通道 1表示联动通道1 
	//2表示联动通道2录像 3表示联动通道1和2录像,以此类推
	//!\param nPort 某个报警通道号
	//!\return 0表示获取成功，-1表示获取失败
	//##ModelId=49B7187602BF
	int GetRecordParam(int *dwChannelFlag);

	//!\brief 设置某个报警联动录像参数
	//!\param dwChannelFlag 联动录像通道标记
	//!\param nPort 某个报警通道号
	//!\return 0表示设置成功 -1表示设置失败
	//##ModelId=49B718640232
	int SetRecordParam(int dwChannelFlag);

	//!\brief 
	//获取某个报警联动拍照参数,拍照张数和间隔由CCapturePic::GetCfg获取
	//!\param dwChannelFlag 存储联动拍照通道标记的缓冲区
	//!\param nPort 报警联动输入通道号
	//!\return 0表示成功 -1表示失败
	//##ModelId=49B5CE9A030D
	int GetSnapshotParam(int *dwChannelFlag);

	//!\brief 
	//设置某个报警联动拍照参数，拍照张数和拍照间隔由CCapturePi
	//c::SetCfg设定
	//!\param dwChannelFlag 联动拍照通道标记
	//!\param nPort 某个报警通道号
	//!\return 0表示设置成功 -1表示失败
	//##ModelId=49B5CDFE00BB
	int SetSnapshotParam(int dwChannelFlag);


	//!\brief 获取某一某个报警通道联动某一云台的动作参数
	//!\param nAction 联动动作 
	//*nAction=0表示无动作，1联动预置点，2联动巡航，3联动轨迹
	//!\param nIndex 联动预置点/巡航/轨迹的编号
	//!\param nChannel 云台对应的通道号
	//!\param nPort 某个报警通道号
	//!\return 0表示获取成功, -1表示获取失败
	//##ModelId=49B718B202DE

	int GetPtzParam(int nChannel, int *nAction, int *nIndex);


	//!\brief 设置某个报警通道联动某一个云台的动作
	//!\param nAction 联动的动作 0表示无动作 1联动预置点 
	//2联动巡航 3联动轨迹
	//!\param nIndex 联动的预置点/巡航/轨迹的编号
	//!\param nChannel 云台对应的通道号
	//!\param nPort 某个报警通道号
	//!\return 0表示成功 -1表示失败
	//##ModelId=49B718BD0261
	int SetPtzParam(int nChannel, int nAction, int nIndex = 0);

	//!\brief 获取某个报警联动报警输出的参数
	//!\param dwOutPortFlag 存储输出通道标记的缓冲区
	//!\param nPort 某个报警通道号
	//!\return 0表示成功 -1表示失败
	//##ModelId=49B718DD030D
	int GetDopParam(int *dwOutPortFlag);

	//!\brief 设置某个报警联动报警输出的参数
	//!\param dwOutPortFlag 设置输出通道标记
	//!\param nPort 某个报警通道号
	//!\return 0表示成功 -1表示失败
	//##ModelId=49B718ED00AB
	int SetDopParam(int dwOutPortFlag);


	//!\brief 启用/不启用子码流
	//!\praram int bEnable false 为不启用，true为启用
	//!\return  成功为0，失败为-1
	//##ModelId=49BDEDDA03DB
	int AuxCoder_Enable(int bEnable);

	//!\brief 判断是否启用码流
	//!\return  int bEnable false 为不启用，true为启用
	//##ModelId=49BDEDDB004D
	int AuxCoder_IsEnable();

	//!\brief 设置子码流帧率
	//!\return  成功为0，失败为-1
	//##ModelId=49BDEDDB006B
	int AuxCoder_SetFrameRate(int nFrameRate);

	//!\brief 获取子码流帧率
	//!\return  子码流帧率，失败为-1
	//##ModelId=49BDEDDB00BB
	int AuxCoder_GetFrameRate();

	//!\brief 设置子码流码率
	//!\return  成功为0，失败为-1
	//##ModelId=49BDEDDB00D9
	int AuxCoder_SetBitRate(int nBitRate);

	//!\brief 获取子码流码率
	//!\return  成功为子码流码率，失败为-1
	//##ModelId=49BDEDDB0133
	int AuxCoder_GetBitRate();

	//add by lxc ,检查板型是不是支持该子码流
	int isSupport(int byResolution);
	
	int AuxCoder_SetFormat(int byResolution, int width, int height);

	int AuxCoder_GetFormat(int *width, int *height);
	
    int AuxCoder_GetQuality(void);
    
    int AuxCoder_SetQuality(int dwQuality);
    
    int AuxCoder_GetBitrateType(void);
    
    int AuxCoder_SetBitrateType(int dwBitrateType);
    
    int AuxCoder_GetStreamType(void);
    
    int AuxCoder_SetStreamType(int dwStreamType);

	int GetAudioInVol();
	
	int SetAudioInVol(int vol);
	
	int SetAudioMappingVol(int vol,int Type);

	int GetAudioOutVol();

	int SetAudioOutVol(int vol);

    int SetAudioCustomVoice(int nCh, int customtype, char *pBuff, int len);

	int SetFireAudioAlarmCustomVoice(int nCh, char *pBuff, int len);
    
	int SetAudioAlarmConfig(int nCh, int enable, int audioType, int audioDelayOutput, int audioVolume, int iAudioPlayTimes);
    
	//!\brief 获取声音报警的时间计划
	//!\param tagAudioAlarmSchedule* cfg 时间计划结构体
	//!\return 成功为0，失败为-1
	int GetAudioAlarmSchedule(tagAudioAlarmSchedule * cfg);

	//!\brief 设置声音报警的时间计划
	//!\param tagAudioAlarmSchedule cfg 时间计划结构体
	//!\return 成功为0，失败为-1
	int SetAudioAlarmSchedule(const tagAudioAlarmSchedule * cfg);

	int CheckAudioAlarmInSchedule();

	int TryListenAlarmVoice(int nCh, int enable, int audioType, int audioDelayOutput, int audioVolume, int alarmType);
    
	int GetAudioAlarmConfig(int nCh, int *enable, int *audioType, int *audioDelayOutput, int *audioVolume,int *iAudioPlayTimes);

	int GetKeyFrameInterval(int stream, int *value);
	
	int SetKeyFrameInterval(int stream, int value);

    int SetAuxAdvEnc(int AuxAdvEncType);

    int GetAuxAdvEnc(int *pAuxAdvEncType);

    int SetHDAuxAdvEnc(int HDAuxAdvEncType);

    int GetHDAuxAdvEnc(int *pHDAuxAdvEncType);

    int SetFourthStreamAdvEnc(int FourthStreamAdvEncType);

    int GetFourthStreamAdvEnc(int *pFourthStreamAdvEncType);
	
    int SetAuxHDAdvEnc(int AuxAdvEncType, int HDAuxAdvEncType);

    int GetAuxHDAdvEnc(int *pAuxAdvEncType, int *pHDAuxAdvEncType);

    PictureFormat_t SwitchPicFormat(int PicFormat);
		
    int ReverseSwitchFormat(int picformat);

    EncodeType_t SwitchAdvCodeType(int nAdvCodeType);
    
    int ReverseSwitchAdvCodeType(EncodeType_t eAdvCodeType);
	int TUTK_GetMedia(int *whichMedia);

	int	TUTK_SetQualityCfg(int tutkQuality,tagEncCfg *info);
	
	int	TUTK_GetQualityCfg(int *tutkQuality);

	int TUTK_GetQualityboardCfg(int count, int *dwStreamType);

	int GetGetRealKeyFrameInterval(int nChannel ,int * nFrameRateInterval , int *nKeyFrameInterval);

#ifdef USE_HDAUX
	//!\brief   开启高清子码流编码
	//!\return  成功0,其它失败
	int StartHDAuxEncCapture();

	//!\brief   停止高清子码流编码
	//!\return  成功0,其它失败
	int StopHDAuxEncCapture();

	//!\brief   启用/不启用高清子码流
	//!\param   int bEnable false为不启用，true为启用
	//!\return  成功为0，失败为-1
	int HDAuxCoder_Enable(int bEnable);

	//!\brief   判断是否启用高清子码流
	//!\return  false为不启用，true为启用
	int HDAuxCoder_IsEnable();

	//!\brief   设置高清子码流参数
	//!\return  成功为0，失败为-1
	int HDAuxCoder_SetEncCfg(tagEncCfg * pInfo);

	//!\brief   获取高清子码流参数
	//!\return  成功为0，失败为-1
	int HDAuxCoder_GetEncCfg(tagEncCfg * pInfo);
#endif

#ifdef USE_FOURTH_STREAM

	//!\brief   获取第四码流参数
	//!\return  成功为0，失败为-1
    int FourthStream_GetEncCfg(tagEncCfg * Info);
    
	//!\brief   设置第四码流参数
	//!\return  成功为0，失败为-1
    int FourthStream_SetEncCfg(tagEncCfg * Info);

#endif

	//!\brief 设置报警联动回调函数
	//!\praram int(*CallBack)(struct tAlertInfo *pAlertInfo,  
	//struct tActionInfo *pActionInfo)) 报警回调指针
	//!\return  成功为0，失败为-1
	//##ModelId=49BDEDDB0198
	static int SetAlarmCallBack(int AlarmType, int (*CallBack) (struct tagAlertInfo * pAlertInfo, struct tagActionInfo * pActionInfo));
  public:

	//检测编码错误
	int CheckCoderError(int bError);
  private:
	//通道号
	int m_iChannel;
	//编码句柄
	HANDLE m_henc;
	//辅码流使能
	int m_bAuxEnable;
	//辅码流参数
	CODEC_PARA_S m_CodecParam;

    int m_CodecStreamType;
	
	int tutkUseWhitchStream;
	int m_tutkQuality;

    //关键帧间隔
    int m_nMainKeyFrameInterval;
    int m_nAuxKeyFrameInterval;
    int m_nHDAuxKeyFrameInterval;

    //子码流编码类型
    EncodeType_t m_AuxAdvEncType;

    //高清子码流编码类型
    EncodeType_t m_HDAuxAdvEncType;    

    //第四码流编码类型
    EncodeType_t m_FourthStreamAdvEncType;    

#ifdef USE_HDAUX
	//高清辅码流使能
	int m_bHDAuxEnable;
	//高清辅码流参数
	//CODEC_PARA_S m_HDCodecParam;
	tagEncCfg m_HDCodecParam;
#endif

#ifdef USE_FOURTH_STREAM
	//第四码流参数
	tagEncCfg m_FourthStreamParam;
#endif

	//报警类型
	static int m_AlarmType;
	//动作处理
	CActionAlarm *m_pActionAlarm;

	//制式
	static VideoOutStandard_t m_iStandard;

	//配置文件
	CConfig *pCConfig;

	//回调函数
	static ALARMCALLBACK m_FxnCallBack;

	//sensor 输入频率
	CmosFrequencyType_t m_SensorFrequency;

	SpeechAlarmInfo_t m_audioAlarmCfg;

	//输入输出音量
    int m_nAudioInVol;
    int m_nAudioOutVol;

	//声音报警计划
	tagAudioAlarmSchedule m_AudioAlarmScheduleCfg;

  private:
	//!\brief 载入设置
	int LoadParam();

};

#endif							/* _INC_CCODER_49A2634B035F_INCLUDED */
