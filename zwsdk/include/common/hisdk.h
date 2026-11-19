#ifndef  _HISDK_H_
#define  _HISDK_H_
#include "zwsdk.h"
#include "defs.h"

#define HI_SDK_SUCESS                   0               //成功

#define ERR_NO                          0x00000000      //无错误
#define ERR_MALLOC_ERROR                0xc0000000      //内存分配失败
#define ERR_INVALID_HANDLE              0xc0000001      //句柄错误
#define ERR_WAIT_TIMEOUT                0xc0000002      //等待超时
#define ERR_INVALID_ARGUMENT            0xc0000003      //参数错误
#define ERR_NOT_SUPPORT                 0xc0000004      //不支持功能
#define ERR_SDK_INITFAIL                0xc0000005      //SDK初始化失败
#define ERR_FILE_READ_ERROR             0xC0000006      //文件读取失败
#define ERR_BUFFER_LEN_SHORT            0xC0000007      //内存大小不够
#define ERR_PACKET_NO_DATA              0xC0000008      //包里没有数据
#define ERR_SYS_BUSY                    0xC0000009      //系统忙
#define ERR_ENCODENOTSTART              0xC000000A      //未启动录像

/* ROI错误码 */
#define ERR_SDK_SETUP_ROI_ERROR         0xd0000027
#define ERR_SDK_CLEAR_ROI_ERROR         0xd0000027
#define ERR_SDK_NOINIT                  0xFFFFFFFF      //SDK没有初始化

#ifndef ERROR_MAX_THRDS_REACHED
#define ERROR_MAX_THRDS_REACHED         0x000000A4				//No more threads can be created in the system
#endif

#ifdef __cplusplus
extern "C" {
#endif

int 	MP4Sys_SwitchToUpgradeMode(int bInto, unsigned int *pu32PhyAddr, void **ppVirtAddr, unsigned int *puMmzAllocSize);

int     MP4Sys_InitDSPs();
int     MP4Sys_DeInitDSPs();

int     MP4Sys_GetBoardInfo( );
int     MP4Sys_GetTUTKUID(char *puid);

int 	MP4Sys_GetSensorType(BoardSensorType_t *SensorType);

int     MP4Sys_GetSerialNum(unsigned char *pszSerialNum);
int     MP4Sys_GetSecretPinStatus(int *pResetStatus);
int     MP4Sys_GetTotalEncChannels( );

void*   MP4Sys_EncChannelOpen(int ChannelNum);
int     MP4Sys_EncChannelClose(void *hChannelHandle);
int     MP4Sys_EncSetCurrentTime( );
int     MP4Sys_EncSetCurrentTimeEx(int tNewTime);
int     MP4Sys_EncSetVideoPara(void *hChannelHandle, int Brightness, int Contrast, int Saturation, int Hue, int Sharpness);
int     MP4Sys_EncRegisterStreamReadCallback(STREAM_READ_CALLBACK StreamReadCallback);
int     MP4Sys_EncStartVideoCapture(void *hChannelHandle);
int     MP4Sys_EncStopVideoCapture(void *hChannelHandle);
int     MP4Sys_EncReadStreamData(void *hChannelHandle, char **pDataBuf, int *Length, int *FrameType, unsigned int *RealTime_Sec, unsigned int *RealTime_USec, int *StreamType);
int     MP4Sys_EncCaptureIFrame(void *hChannelHandle, int nSensorIndex);
int     MP4Sys_EncSetStreamType(void *hChannelHandle, unsigned short Type);
int     MP4Sys_EncGetStreamType(void *hChannelHandle, unsigned short *StreamType);
int     MP4Sys_EncSetCodecPara(void *hChannelHandle, int bAux, CODEC_PARA_S *pCodecPara, EncodeType_t encType);
int     MP4Sys_EncSetCodecParaExt(void *hChannelHandle, int stream, CODEC_PARA_S_EXT *ppCodecPara);
int     MP4Sys_EncGetCodecPara(void *hChannelHandle, int bAux, CODEC_PARA_S * pCodecPara);

int     MP4Sys_EncStartAuxCodec(void *hChannelHandle);
int     MP4Sys_EncStopAuxCodec(void *hChannelHandle);
int     MP4Sys_EncStartThirdCodec(void *hChannelHandle);
int     MP4Sys_EncStopThirdCodec(void *hChannelHandle);
int     MP4Sys_EncSetOsdText(void *hChannelHandle, TextOSDInfo_t *pOSDTextInfo);
int     MP4Sys_StartSysStatusLed(void *hChannelHandle, int freq);
int     MP4Sys_EncSetOsdTime(void *hChannelHandle, TimeOSDInfo_t *pOSDTimeInfo);
int 	MP4Sys_EncSetOsdV2Name(void *hChannelHandle, TextOSDV2Info_t *pOSDV2NameInfo);
int     MP4Sys_EncSetOsd(void *hChannelHandle, int EnableTime, int EnableText);
int 	MP4Sys_StopROI(void *hChannelHandle, int stream_id);

//人形检测使能开关
int     MP4Sys_EncSetHumanDetectEnable(void *hChannelHandle, int bEnable);

//人形跟踪使能开关
int     MP4Sys_EncSetHumanTrackEn(void *hChannelHandle, int bEnable);

//人形检测配置
int     MP4Sys_EncSetHumanShapeSchedule(void *hChannelHandle, LpAiScehdule Schedule);
int     MP4Sys_EncSetHumanShapeCfgV2(void *hChannelHandle, LpHumanShapeCfgV2 pCfg);
int     MP4Sys_EncSetDetectSchedule(void *hChannelHandle, int enable);

int     MP4Sys_EncSetupMotionDetection( void *hChannelHandle, unsigned char *listOfAreas);
int     MP4Sys_EncSetMotionSchedule(void *hChannelHandle, LpAiScehdule Schedule);
int     MP4Sys_EncStartMotionDetection(void *hChannelHandle);
int     MP4Sys_EncStopMotionDetection(void *hChannelHandle);
int     MP4Sys_EncAdjustMotionDetectPrecision(void *hChannelHandle, int iGrade, int iMotionDetectFps);

int     MP4Sys_EncSetSnapPara(void *hChannelHandle, ResolutionParam_t *PictureRes, int nQuality);
int     MP4Sys_EncSetSnapParaV2(void *hChannelHandle, int nQuality, char nStreamType);

int     MP4Sys_AlarmOutSupport(int nSupport);
int     MP4Sys_AlarmOutDefaultStatus(int nStatus);
int     MP4Sys_AlarmOut (long nPort, int bOpen);
int     MP4Sys_LightAlarm (int nMode, int bOpen);
unsigned int    MP4Sys_AlarmIn(int *pAlarmIndata);

int     MP4Sys_SetTalkingMode(int Mode);
int     MP4Sys_SetAudioInMode(int Mode);
//-----------------------------------------------------------------------------
//    说明：      设置语音播报的数据
//    输入参数：    bEnable
//    输入参数： dataFormat 0：pcm格式，1：G711u格式
//    返回	：  成功为 0 , 0x102：当前为对讲模式
//-----------------------------------------------------------------------------
int     MP4Sys_SetSpeechingEnable(int bEnable, int dataFormat);
int     MP4Sys_SetTalkingEnable(int bEnable);
int     MP4Sys_ReadTalkingData(char *DataBuf, unsigned int* BufSize);
int     MP4Sys_SetTalkingData(char *DataBuf, unsigned int DataSize);
//-----------------------------------------------------------------------------
//    说明：      设置语音播报的数据
//    输入参数：    DataBuf
//    输入参数：    DataSize
//    返回	：  成功为 0 , 0x101：已恢复为了空闲，0x102：当前为对讲模式
//-----------------------------------------------------------------------------
int     MP4Sys_SetSpeechData(char *DataBuf, unsigned long DataSize);
int     MP4Sys_RegisterTalkingReadCallback(TALKING_READ_CALLBACK TalkingReadCallback);
void    MP4Sys_SetSerialPortDirection(int iTransmit);

//与日夜切换相关接口
int		MP4Sys_EncSetIrCut(void * hChannelHandle, int nDayNightSensity);
int		MP4Sys_EncSetIrcutInitiativeModeSensity(void * hChannelHandle, int Sensity);
int 	MP4Sys_EncSetDayNightMode(void * hChannelHandle, DAYNIGHTMODE_T mode, LP_IrcutTimeModeInfo pIrcutTimeModeInfo);
int 	MP4Sys_EncSetVarWhiteMode(void * hChannelHandle, NIGHTVISIONWORKMODE_T mode, NIGHTVISIONCUSTOMMODE_T manualMode,  int bright);
int 	MP4Sys_EncSetVarWhiteModeInfo(void *hChannelHandle, LP_IrcutNightVisionTimeInfo pVarWhiteModeInfo, int nightThr, int dayThr);
int 	MP4Sys_EncSetColorNightMode(void *hChannelHandle, NIGHTVISIONWORKMODE_T mode, LP_IrcutNightVisionTimeInfo pColorNightModeInfo);

int 	MP4Sys_EncSetIrcutDirection(void * hChannelHandle, int mode);
int 	MP4Sys_EncSetDayNightSwitchTime(void * hChannelHandle, int time);
int 	MP4Sys_EncSetDayNightInitiativeSwitchTime(void * hChannelHandle, int time);
int     MP4Sys_SetNormalWhiteLightThreshold(void *hChannelHandle, int C2Blkthreshold, int Blk2Cthreshold);
int     MP4Sys_SetColorNightThreshold(void *hChannelHandle, int threshold);
int     MP4Sys_SetIntelligentThreshold(void *hChannelHandle, int threshold);

/* PTZ设置IRCUT的日夜转换模式        dayNight:0 day,1 night*/
int		MP4Sys_EncPTZSetIrcutPassivity(void *hChannelHandle, int dayNight);

/**
 * @description: 获取当前得日夜状态
 * @param {void} *hChannelHandle 通道句柄
 * @param {int} *isDay 是否是白天
 * @return {int} 0：成功 <0：失败
 */
int MP4Sys_EncGetDayNightStatus(void *hChannelHandle, int *isDay);

//与sensor设置相关的函数待定
int		MP4Sys_EncSetBacklightMode(void *hChannelHandle,  int enable);
int 	MP4Sys_SetWhiteBalance(void *hChannelHandle, WHITEBALANCE_T *WhiteBalance);
int 	MP4Sys_SetAEParam(void *hChannelHandle, AE_T *aeParam);
//宽动态
int 	MP4Sys_SetLocalExpMode(void *hChannelHandle, int ExpMode);

//3D降噪
int	 	MP4Sys_SetMCTFStrength(void *hChannelHandle, int MCTFStrength);

int MP4Sys_SetLensOrientation(unsigned int nLensOrientation);

//旋转
int 	MP4Sys_SetFlipRotateMode(void *hChannelHandle, FLIP_ROTATE_MODE_E flipRotate);
//pc/tv模式
int 	MP4Sys_SetChromaScale(void *hChannelHandle, CHROMA_SCALE_E chromaScale);

int		MP4Sys_SetDcIris(void *hChannelHandle, int enable);
int 	MP4Sys_SetDcIrisDuty(void *hChannelHandle, int duty);

int     MP4Sys_SetAudioInVol(void * hChannelHandle, int vol);

int     MP4Sys_SetAudioOutVol(void * hChannelHandle, int vol, int IsUpdateVoiceCache,  int m_IntercomMode);

int	MP4Sys_SetAudioMappingVol(void * hChannelHandle, int vol, int Type);

// 对讲模式： 全双工（0） / 半双工（1）
int MP4Sys_EncGetIntercomMode(int *iIntercomMode);
int MP4Sys_EncSetIntercomMode(int iIntercomMode);

int     MP4Sys_GetBoardTypeNoSDK(char *pInfo);

/* 设置按键状态获取回调函数 */
int MP4Sys_RegisterKeyStautsCallBack(KEY_STATUS_CALLBACK getKeyStateCallback);

/* 设置串口回调函数 */
int MP4Sys_SetSerialDataCallBack(SERIALDATA_READ_CALLBACK SerailDataReadCallBack);

int MP4Sys_SetLinkStatus(int status);
int MP4Sys_SetWifiStatusLed(WIFI_STATUS_T wifiStatus);
//设置网卡状态 status0:网络异常 1：无线状态 2：有线状态
int MP4Sys_SetCurrNetCardType(XRS_NETCARD_TYPE_E status);

int MP4Sys_SetAlarmLightStayTime (int nEnable, int nStayTime);
int MP4Sys_SetAlarmLightStart();
int MP4Sys_SetSpeechContent(SpeechType_T speech);
int MP4Sys_SetAlarmSpeechCfg(void *hChannelHandle, SpeechAlarmInfo_t *speechCfg);
int MP4Sys_SetRemindSpeechCfg(void *hChannelHandle, int TypeIdx, SpeechRemindInfo_t *speechCfg);
int MP4Sys_SetRemindHangupState(int iState);
int MP4Sys_SetAudioCustomVoice(void *hChannelHandle, int customtype, char *pBuff, int len);
//-----------------------------------------------------------------------------
//	说明：		设备外设控制
//	输入参数：AssistantCtrlInfo	pCtrlInfo	控制信息
//	返回:		成功为 0 , 其它为定义的错误号；
//-----------------------------------------------------------------------------
int MP4Sys_AssistantControl(AssistantCtrlInfo *pCtrlInfo);

// 读取唯一的硬件ID
int MP4Sys_GetUniqueID(char *uniqueid, int len);

/**
 * @description: 初始化KS HalMod模块
 * @return {int} 正常返回0，异常返回<0
 */
SINT32 MP4Sys_HalModInit();

/**
 * @description: 反初始化KS HalMod
 * @return {SINT32} 正常返回0，异常返回<0
 */
SINT32 MP4Sys_HalModDeInit();

/**
 * @description: 设置系统启动状态标志
 * @param {int} Stat 启动状态标记
 * @return {int} 正常返回0，异常返回<0
 */
SINT32 MP4Sys_SetSysStartTag(int Stat);

int MP4Sys_LightBoardType(int nLightType);

int MP4Sys_SetSpeechLanType(eSpeechLanguage speech_language);

int MP4Sys_GetAdcStatus(int *nAdcStatus);

#ifdef __MODULE_4G_SUPPORT__
int  MP4Sys_GetModuleType(e4GModuleType *moduletype);

int  MP4Sys_SetModuleType(e4GModuleType moduletype);

int MP4Sys_Set4GTelecomOperatorType(e4GTelecomOperatorType telecomOperatorType);

//4G模块复位
int MP4Sys_4GModuleReset();

int MP4Sys_GetBroadCastEnable(int *nEnable);

int MP4Sys_SetCloudCardmode(int CloudCardmode);
#endif

//设置声音侦测配置
int MP4Sys_SetAudioDetectCfg(void *hChannelHandle, LpAudioDetectCfg pCfg);
//注册声音侦测报警回调
int MP4Sys_RegisterAudioDetectCallback(AudioDetect_CALLBACK pCallback, void *UserData);

int MP4Sys_GetSpeechPlayingStatus(void);
int MP4Sys_UpdatePtzStatus(void *hChannelHandle, ePtzStatus enStatus);
int MP4Sys_PtzForTrackBacktoPreset(void *hChannelHandle, ePtz2PresetStatus toPresetStat);
int MP4Sys_SetColorNightCfg(void *hChannelHandle, int lastTime);
int MP4Sys_SetInfraredThreshold(void *hChannelHandle,  int time);
int MP4Sys_SetInfraredWorkMode(void *hChannelHandle,  int mode);
int MP4Sys_SetAuthInfo(const unsigned char *pszAuthLicense);
int MP4Sys_GetAuthInfo(unsigned char *pszAuthLicense, unsigned int *pAuthState);
#ifdef __AGINGTEST__
/* 强制开启夜晚状态开关（老化测试） */
int MP4Sys_SetForceNightStatus(void *hChannelHandle, int enable);
#endif
int MP4Sys_RemoveAuthLicense(void);
int MP4Sys_SetActiveStatusCode(int nActiveStatusCode);
int MP4Sys_GetActiveStatusCode(int *pActiveStatusCode);
int MP4Sys_GetBitrateInfo(void *hChannelHandle, int dataArr[], int arrlen);
int MP4Sys_EnableQrScan(int bEnable);
int MP4Sys_RegisterQrScanCallback(QR_SCAN_CALLBACK pCallback);
int MP4Sys_RegisterMotoSavePosCallback(QR_SCAN_CALLBACK pCallback);

int MP4Sys_SetAudioCodec(int ACodec);
int MP4Sys_SetInfraredBrightness(void *hChannelHandle,  int brightness);
int MP4Sys_SetWhiteLightThreshold(void *hChannelHandle,  int time);
int MP4Sys_SetImageMode(void *hChannelHandle,  int mode);
int MP4Sys_SetDefogStrength(void *hChannelHandle,  int strength);
int MP4Sys_SetAntiFlickerStrength(void *hChannelHandle, int strength);
int MP4Sys_SetNoAuthInfoOSDDisplay(void);
int MP4Sys_EncSetOsdTextEx(void *hChannelHandle, TextOSDInfoEx_t *pOSDTextExInfo);
int MP4Sys_EncSetOsdV2Text(void *hChannelHandle, TextOSDV2Info_t *pOSDV2TextInfo);
int MP4Sys_SetHumanOverlayInfo(void *hChannelHandle, char nMainStreamHumanOverlay, char nSubStreamHumanOverlay);
int MP4Sys_SetDormancyState(int iState);
int MP4Sys_SetLightOpenStatus(int Status);
// 设置产测模式，目前给灯光检测判断使用
int MP4Sys_SetProductMode(int nProductMode);
int MP4Sys_SwitchAecMode(int AecMode);

int MP4Sys_SetPauseAudioAlarm();

int MP4Sys_SetRecoveryAudioAlarm();

#ifdef __ONE_CLICK_CALL__

int MP4Sys_SetAlternateResetButton(int enable);
int MP4Sys_SetKey2callhanupState(int iState);
int MP4Sys_GetKey2callDuSpeechStatus(int *status);
int MP4Sys_GetCallPinStatus(int *pCallStatus, int *pHangupStatus);
int MP4Sys_GetCallPinStatus_Test(int *pCallStatus, int *pHangupStatus);
int MP4Sys_SetCallPinStatus_Test(int pCallStatus, int pHangupStatus);
int MP4Sys_SetKeyToCall(int enable);
int MP4Sys_SetKeyToCallHardwareType(int type);
#endif

int MP4Sys_EncSetSlowFrameMode( int SlowFrameMode);
int MP4Sys_EncSetNightLedDuration( int LedDuration );
int MP4Sys_EncSetNightLedMode( NightLedCommand_T NightLedFlag );
int MP4Sys_SysInitSensorCnt();
int MP4Sys_GetMaxSensorCnt();
int MP4Sys_GetSensorConfCorrect(int CheckCnt);
int MP4Sys_HumanTrackerCallBack(HumanTracker_CALLBACK pFn_HumanTracker);
int MP4Sys_GetWiFiInfo(void *wifiInfo);
int MP4Sys_P2PNotifySendDataFail(const char * pBuffer, UINT32 dwBufLen);

#if defined(__DIGITAL_ZOOM_DRAG_SUPPORT__)
	int MP4Sys_SetDragZoomRatio(double ratio,int eOpreator);

	int MP4Sys_GetDragZoomRatio(double *ratio);

	int MP4Sys_GetEachFocusZoom(double *shortZoom, double *middleZoom, double *longZoom);

	int MP4Sys_SetDragZoomCmd(DRAG_ZOOM_CMD nCmd,int eOpreator);

	int MP4Sys_GetDragZoomChgStatus(int *status);

	int MP4Sys_SetDragZoomScaleMagnification(double ratio);

	int MP4Sys_GetDragZoomRange(double *min,double *max);

	int MP4Sys_HumanTrackerCallBack(HumanTracker_CALLBACK pFn_HumanTracker);
	double MP4Sys_GetCurRatio();
#if defined(__IS_SUPPORT_LONG_SHORT_FOCUS_ZOOM__)
	int MP4Sys_GetCurrentSensor();
	int MP4Sys_SetZoomLens(int nLens);
#endif
#endif

#if defined(__IS_SUPPORT_LCD_MANAGER__)
int MP4Sys_LCDRefreshFrame(char *pData, unsigned int DataSize);

int MP4Sys_LCDSetBackLightLevel(int level);

int MP4Sys_LCDCreateVdecChn(int nType);

int MP4Sys_LCDSendStreamToChn(int nType, char *pData, unsigned int DataSize);

int MP4Sys_LCDDestoryVdecChn(int nType);
#endif

int MP4Sys_DownSdPower();
int MP4Sys_GetRealKeyFrameInterval(int nChannel,int *nFrameRateInterval ,int *nKeyFrameInterval);
int MP4Sys_GetSensorName(char *SensorType);

int MP4Sys_SetSptSwapLRPTZChannel(int bEnable);
int MP4Sys_GetSptSwapLRPTZChannel(int *pbEnable);


#ifdef __cplusplus
}
#endif
#endif

