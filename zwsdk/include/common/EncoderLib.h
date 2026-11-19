#ifndef	_ENCODERLIB_H_
#define	_ENCODERLIB_H_

#include "Common.h"
#include "lightctrl.h"

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MID(a, min, max) (MAX(MIN((a), (max)), (min)))

#define SD_AGING_TEST_LIST_FILE_NAME    "/tmp/aging_test.list"
#define SD_AGING_TEST_AUDIO_FILE_NAME   "/tmp/aging_test.g711u"
#define AUTH_DID_NUM_FLAG               "/tmp/.auth_did_num_flag"

/* 编码通道句柄信息 */
typedef struct
{
	/* 编码参数句柄 */
	void                *pPlatformChannelhandle;

	/* 编码相关资源 */
	BOOL			    m_bSupportFourthStream;  //支持第四码流
	BOOL				m_bRecord;			    //通道是否启动编码
	BOOL				m_bRecordAux;		    //通道是否启动辅码流编码
	BOOL				m_bRecordFourth;	    //通道是否启动四码流编码
	BOOL				m_bChangeCodePara;      // 记录编码参数修改；
    BOOL                bMainStreamOK[SUPPORT_MAX_SENSOR_COUNT];
	BOOL                bAuxStreamOK[SUPPORT_MAX_SENSOR_COUNT];
    BOOL                bUpgradeStreamOK[SUPPORT_MAX_SENSOR_COUNT];
	BOOL                bFourthStreamOK[SUPPORT_MAX_SENSOR_COUNT];
	BOOL                bAudioStreamOK;
	BOOL				bMotionStateOK[ALGO_CHANNEL];
	int					m_LastCaptureQuality;   //抓图质量
    int                 m_RealTime;
	int                 m_RealTime_USec;
	short				m_streamType;			//通道的编码类型
	CaptureType_t  		m_CaptureType;          //抓图类型
	EncodeType_t        m_EncType;              // 用户设置的主码流编码类型
    EncodeType_t        m_AuxEncType;           // 用户设置的子码流编码类型
	EncodeType_t		m_FourthEncType; 		// 用户设置的第四码流编码类型
    CODEC_PARA_S        m_EncAttr;              //用户设置的主码流参数
	CODEC_PARA_S        m_EncAttrAux;           //用户设置的子码流参数
	CODEC_PARA_S        m_EncAttrFourth;        //用户设置的四码流参数
    ResolutionType_t    m_PictureRes[MAX_STREAM_COUNT];      //用户设置的码流的当前编码分辨率,用来取代m_CurPictureFormat

 	/* 音频相关 */
    int					m_AudioCodec;                   //6:G711_A_LAW   2:G711_MU_LAW
	int					m_AudioChNum;			        //音频通道编号(5008,5016均只有前4路支持音频) 	
	int					m_SpeechNum;			        //有几条待播报的语音
	int					m_SpeechWrite;			        //在队列里哪个位置存放播报语音
	int					m_SpeechRead;			        //在队列里哪个位置取待播报语音
	int					m_SpeechType[MAX_SPEECH_CACHE_NUM];	//语音播报的类型
	int                 m_nSpeechPlayStatus;            //当前播放器的播放状态
	int					m_SpeechNow;			        //当前正在播放的播报类型
	int					m_SpeechAlarmDelayClear;		//报警播报的累积延时输出时间清空标记
	int 				m_BoardcastSpeechFirst;			//语音播报优先（比报警播报优先）
	int                 m_SpeechPlayEnd;                //文件播报结束标志
	int 				m_nAudioInVolNormal;            //音频输入音量(常规状态下)
    int 				m_audioSyncFlag;
	char				m_SpeechFileName[MAX_SPEECH_CACHE_NUM][MAX_SPEECH_FILE_SIZE];	//语音播报的文件名
	char				m_SpeechFilePath[128];
    eSpeechLanguage     m_enSpeechLanguage;		        //播报的语言，0：滴滴，1：中，2：英，3：其他语言
	SpeechAlarmInfo_t	m_AlarmSpeechCfg;               //报警语音配置
	SpeechRemindInfo_t	m_RemindSpeechCfg[eSpeechTypeRemindMax];    //定时提醒语音配置
	StreamInfo_t		m_AudioStreamInfo;		        //音频流通道
	int                 m_IntercomMode;                 // 对讲模式: 全双工（0） / 半双工（1）

	/* ISP图像调整   */
	BOOL			ResetUserPic;
	AE_T            AEParam;
	int				Contrast;				/*对比度*/
	int				Bright;					/*亮度*/
	int				Saturation;				/*饱和度*/
	int				Hue;					/*流明度*/
	int				Sharpness;				/*锐度*/
    int             m_ae_mode;
	int 			FogStrength;            /*去雾强度*/
	int 			MCTFStrength;           /*降噪参数*/
	int             m_antiFlickerStrength;  /*抗闪强度*/  
	int 			m_WDRLevel;             /*宽动态*/
	int             m_LocalImageMode;
	WHITEBALANCE_T  WhiteBalance;           /*白平衡参数*/
	FLIP_ROTATE_MODE_E flipRotate;          /*图像镜像配置*/

    /* 日夜切换 */
	DAYNIGHT_T		m_DayNightStutas;               //1当前是白天,0是夜晚
	BOOL 			m_bLightAlarmIsOpen;	        //0-关闭灯光报警	 		1-开启灯光报警
	int 			m_nLightAlarmMode;		        //0-常亮模式		1-闪烁模式
    int             VarWhiteCustomBright;           //亮度值
	int				m_VarWhiteNightThreshold;		//晚上阀值
	int				m_VarWhiteDayThreshold;		    //白天阀值
	int				m_PtzCtrlIrcut;
	int				m_PtzSetDayOrNight;		        //0 day,1 night
	int				m_colorNightLastTime;
	int				m_IrcutState;                   //非零表示IRCUT需要切换
	int				m_IrcutStateHis;                //保存当前IRCUT的切换状态，改变IRCUT切换正反时恢复用                     1->白天 2->晚上
	int				m_IrcutSwitchFlag;
	int				m_IrcutDeriction;               //IRCUT切换方向
	int				m_DayNightSwitchTime;
	int				m_DayNightInitiativeSwitchTime;
	int				m_InfraredThreshold;		    //无极红外生效阀值
	int				m_WhiteLightThreshold;		    //无极白光生效阀值
	int				m_InfraredWorkMode;			    //无极红外工作模式 1:自动 0:手动
	int				m_InfraredBrightness;		    //无极红外亮度值 0 - 100
	int				m_ColorNightThreshold;		    //全彩白光生效阀值
	int				m_IntelligentThreshold;		    //智能双光生效阀值
	int				m_NormalWhiteC2BlkThreshold;    //普通白光彩转黑生效阀值
	int				m_NormalWhiteBlk2CThreshold;    //普通白光黑转彩生效阀值
	int				m_ImageMode;				    //0:normal   1:人脸无曝光模式   2:车牌模式
    int 			m_BacklightEnable;
	int 			m_InitiativeModeSensity;        //主动模式灵敏度
	int				m_IrPin3Default;	            //0:表示给高电平开灯，1：表示给低电平开灯
	DAYNIGHTMODE_T              DayNightMode;       //当前用户设置的日夜模式
	IrcutTimeModeInfo           m_ircutTimeModeInfo;
	IrcutNightVisionTimeInfo    m_ircutColorNightModeInfo;
	NIGHTVISIONWORKMODE_T       ColorNightMode;             //当前用户设置的全彩白的工作模式
    NIGHTVISIONWORKMODE_T       VarWhiteMode;               //工作模式 手动、自动
    NIGHTVISIONCUSTOMMODE_T     VarWhiteCustomMode;         //自动下的控制方式 定时、手动阈值
	IrcutNightVisionTimeInfo    m_ircutVarWhiteModeInfo;    //时间段
	LIGHT_CTX_S		            m_stLightCtx;               // 灯板控制上下文

	/* 运动检测相关 */
	BOOL			m_bStartMD;				//是否启动运动检测
	BOOL			m_bStartMidMotion;		//中间层移动侦测状态(真实反映客户端对移动侦测的使能状态)
	BOOL			m_bStartMDForDL;		//给智能双光用
	BOOL           	m_bStartHuman;		
	BOOL			m_bMotionStateIsValue;	//运动状态是否有效，给夜视全彩使用
	BOOL			m_bMotionStateEx;		//是否检测到运动，给夜视全彩使用
	BOOL			m_bMotionState;			//是否检测到运动
	BOOL			m_bIsMotion;
	int				MotionFps;				//运动检测间隔
	int				MotionGrade;			//分析SAD时的灵敏度（0 - 高，1 - 中， 2 - 低）
	int				m_MotionRectStride;		//编码尺寸不同，宏块的个数不同
	int				MotionAreas;			//检测区域总数
	char			pUsrBlock[192];			//用户设置的运动检测宏块
	char			pUsrBlockState[192];	//用户设置的运动检测宏块
	AiScehdule 		m_stMotionSchedule;     //移动侦测布防配置

	/* 人形相关参数 */
	BOOL			m_bHumanStateIsValue;	//人形状态是否有效，给智能双光使用
	BOOL			m_bHumanStateEx;		//是否检测到人形，给智能双光使用
	BOOL			m_bHumanShapeState[ALGO_CHANNEL];     //是否检测到人形,用来发送通知，如果置位了则需要发送一帧JPG图
	double 			m_AlgoThr;				//算法检测灵敏度
	int             m_HumanDetectSchedule;  //布防时间画框判断
	int				m_HumanAreaOverlayEnable[MAX_STREAM_COUNT];	//人形框叠加使能
	HumanShapeCfgV2 m_stHumanShapeCfgV2;    //人形配置
	AiScehdule		m_stHumanShapeSchedule; //人形布防配置

    /* 声音侦测 */
    AudioDetectCfg  m_stAudioDetectCfg;     //声音侦测配置

	/* 时间OSD相关 */
	BOOL			m_bCreatTimeOSDRegion;	                //时间OSD区域是否已经创建
	BOOL			m_bEnableTimeOSD;		                //启动时间OSD显示
	int				TimeBlockNum[MAX_STREAM_COUNT];			//时间宏块数目
	RECTH			m_TimeOSDRect[MAX_STREAM_COUNT];        //时间OSD区域
	char 			BlockAscValue[MAX_STREAM_COUNT][22];    //宏块对应的ASC值(最多22个宏块)
	SysTimeOSD_t	m_TimeOSDInfo;

	/* 文本OSD相关 */
	BOOL			m_bCreatTextOSDRegion;	                //文本OSD区域是否已经创建
	BOOL			m_bEnableTextOSD;		                //启动文本OSD显示
 	int				TextBitmapWidth[MAX_STREAM_COUNT];		//文本信息最终位图宽度
	int				TextBitmapHeight[MAX_STREAM_COUNT];		//文本信息最终位图高度
 	RECTH			m_TextOSDRect[MAX_STREAM_COUNT];	    //文本OSD区域
	RECTH			TextOSDRect_Src;		                //软件设置的原始文本OSD区域(非实时D1需要)
	char			*pTextOSDBit_Src;		                //软件设置的原始文本OSD点阵(非实时D1需要)
	SysTextOSDV2_t	m_TextOSDV2Info;		                //OSDV2相关信息

    /* 资源互斥 */
	pthread_mutex_t m_IrcutLock;
	pthread_mutex_t m_DayNightLock;
	pthread_mutex_t m_SensorLock;
	pthread_mutex_t	m_CheckLumLock;
	pthread_mutex_t m_VencLock;
	pthread_mutex_t m_InitVPSSLock;
	pthread_mutex_t m_AudioLock;
	pthread_mutex_t	m_EncLock;
	pthread_mutex_t m_IVPLock;
	pthread_mutex_t	m_OSDLock;
	pthread_mutex_t	m_MDLock;
	pthread_mutex_t	m_SpeechLock;
	pthread_mutex_t	m_SpeechCustomAMRFileLock;
    pthread_mutex_t m_HumanShapeLock;
    pthread_mutex_t m_SlowFrameLock;            // 低帧模式资源互斥

	int                 m_recvPauseAudioLightAlarm; //暂停声光报警,用于打断报警音频播放
	int 				m_whiteLightFlickeredEnable;    //白光灯闪烁报警能力
    int                 m_NightLedFlag;                 //触发一键夜灯标志
    int                 m_NightLedTime;                 //一键夜灯保持时长，单位s

    /* 系统杂项控制 */
	int 			m_FinishingGrade;           //当前收光等级
	int 			m_HighTempFlag;             //高温标记
	int				m_SysStarted;			    //系统是否已启动完成
	int				m_AlarmOutDefault;	        //0:表示开启报警给高电平，1：表示开启报警给低电平	
    int				m_nAssistantControlTest;    //是否正在辅助控制测试	        	大于0-则正在测试    		等于	0-没有测试
    ePtzStatus 			m_enPtzStatus;          //PTZ状态
	ePtz2PresetStatus 	m_iPtzTrackBacktoPreset;//跟踪是否回到预置位
	KEY_STATUS_CALLBACK m_GetKeyStatusCallBack;

    /* 低帧模式 */
    int             m_iSlowFrameMode;                       // 低帧模式: 0-处于低帧模式 1-处于正常帧模式
    int             m_iSlowFrameResetFlag;                  // 重置时间标志
    int             m_isRuRemotepreview;                    //是否正在直播   0-无直播 1-正在直播
#if defined(__DIGITAL_ZOOM_DRAG_SUPPORT__)
    //OSD相关
    double              m_ZoomRealValue;                        //实际生效的倍数
    double              m_ZoomOsdValue;                         //用于叠加OSD的倍数 1.0-12.0
    ZoomOSD_t		    m_ZoomOsdInfo;							//ZOOM OSD相关信息
    int				    ZoomBlockNum[MAX_STREAM_COUNT];			//时间宏块数目
    RECTH			    m_ZoomOSDRect[MAX_STREAM_COUNT]; 		//变倍 OSD区域
    //ZOOM相关
    int                 m_ZoomDragEn;					        //跟踪放大使能
    DRAG_ZOOM_CMD       m_ZoomDragCmd;                          //拖动变倍命令
    int                 m_ZoomDragCmdStop;					    //zoom停止标记
    double              m_ZoomCurRatio;                         //Zoom当前倍数
    double              m_ZoomTrueRatio;                        //Zoom用户期望倍数
    double	            m_ZoomUserMaxRatio;					    //用户定制的最大倍数
    int                 m_ZoomChgStatus;                        //变倍状态
    int                 m_ZoomOpreate;                          //操作者
	int 	            m_ZoomExecFlag;						    //zoom执行标记
	int                 m_ClickFirst;
	unsigned long       m_UserChangeRatioTime;				    //上一次用户操作变倍时间
	int 	            m_DragRunFlag;
	int 	            m_DragUseOsd;
	double 				m_FocusCurRatio;         				// 当前实际生效倍数
#if defined(__IS_SUPPORT_LONG_SHORT_FOCUS_ZOOM__)
    int 				m_CurrentSensor;           				// 当前传感器 0:短焦 1:中焦 2:远焦
    double				m_WideMaxRatio;         				// 短焦最大倍数
    double 				m_MiddleMaxRatio;       				// 中焦最大倍数
    double 				m_TeleMaxRatio;         				// 长焦最大倍数
    int 				m_ZoomLens;           					// 长短焦组合 0:4+12mm--1:4+8mm--2:4+6mm
#endif
    pthread_mutex_t     m_DragZoomLock;
    pthread_mutex_t     m_ZoomValueLock;
    pthread_mutex_t     m_ZoomOsdLock;
#endif

#ifdef __ONE_CLICK_CALL__
	/* 一键呼叫 */
	int 			m_key2callDuSpeechStatus;		//一键呼叫嘟嘟提示音状态 0：未播放 1：播放中
	int 			m_recvkey2callhanup;			//一键呼叫挂断,用于打断音频播放(0:不打断 1:立即打断 2：播放完成再打断)
#endif
	int 			m_recvRemindHangUp;			    //定时提醒挂断,用于打断音频播放(0:不打断 1:立即打断)
}CChannelContext;

/* 语音对讲通道 */
typedef struct
{
	BOOL			m_bStart;
	pthread_mutex_t	m_Lock;

	/* 海思相关的资源 */
	int				m_AudioEncCh;			//音频编码通道
	int				m_AudioDecCh;			//音频解码通道
	StreamInfo_t	m_TalkingIn;			//对讲采集数据缓冲
	StreamInfo_t	m_TalkingOut;			//对讲采集数据缓冲
}CTalkingChannel_t;

void* UpdateOSDThread( void * lpParameter);
void* LargeBitRateCheckThread( void * lpParameter);
void* BuleScreenThread(void * lpParameter);
void* AudioDecThread( void * lpParameter);
void* AudioInThread( void * lpParameter);
void* IRCUTSwitchThread(void * lpParameter);
void* CheckDayAndNightThread(void *lpParameter);
void* CheckBrightnessThread(void * lpParameter);
void* CheckSecretPinStatusThread(void * lpParameter);
#if defined __ONE_CLICK_CALL__
void* CheckKeytocallPinStatusThread(void * lpParameter);
#endif
void* CheckAveLumThread(void * lpParameter);
void* DrawLineThread(void * lpParameter);

int	HY_Enc_GetEncChInfo(CChannelContext **pInfo);

/* 获取语音对讲通道信息句柄 */
int	HY_Enc_GetTalkingChInfo(CTalkingChannel_t **pInfo);

int HY_Enc_IsEncHandleOk(void * hChannelHandle);

int HY_Enc_InitEncCap();
int HY_Enc_PutEncCap(int CapNum);
int HY_Enc_GetEncCap(int CapNum);
int HY_Enc_AdjustEncCap();
int HY_Enc_SetFrameRate(void *hChannelHandle, ENCODE_STREAM_E chn_type,int frmRate, int slowframemode);
/**
 * @description: 初始化灯板控制模块
 * @return {int} 0: 成功 <0: 失败
 */
int HY_LightCtlInit();

int HY_Enc_PlatformInit();

int HY_Enc_UnInitAISDK();

int HY_Enc_PlatformDeInit();

int	HY_Enc_DeInitEncCap();

int HY_Enc_StartEnc(CChannelContext *pChannelContext, int stream);

int HY_Enc_StopEnc(CChannelContext *pChannelContext, int stream);

int HY_Enc_ForceIFrame(CChannelContext *pChannelContext, int nSensorIndex);

int HY_Enc_SwitchPicFormat(CChannelContext *pChannelContext, int EncChannel);

int HY_Enc_ReadOneFrame(CChannelContext *pChannelContext,
							 char **pFirstDataBuf,
						  	 int 	*pFirstLength,
						  	 int 	*pFrameType, 
						  	 int CallBackType);

/**
 * @description: 设置时间OSD的ARGB1555点阵信息
 * @param {CChannelContext} *pChCtx 通道句柄
 * @param {SysTimeOSD_t} *pSysTime 时间OSD配置
 * @param {STREAM_TYPE_E} eStmTyp 流类型，MAX_STREAM_COUNT指所有通道
 * @param {BOOL} bEn 是否使能显示
 * @return {SINT32} 0-成功，非0-失败
 */
int HY_Enc_SetTimeOSDBitMap(CChannelContext *pChCtx, SysTimeOSD_t *pSysTime, ENCODE_STREAM_E eStmTyp, BOOL bEn);

/**
 * @description: 设置文本OSD ARGB1555点阵信息
 * @param {CChannelContext} *pChCtx 通道句柄
 * @param {STREAM_TYPE_E} eStmTyp 流类型，MAX_STREAM_COUNT指所有通道
 * @param {BOOL} bEn 是否使能显示
 * @return {SINT32} 0-成功，非0-失败
 */
SINT32 HY_Enc_SetTextOSDBitMap(CChannelContext *pChCtx, ENCODE_STREAM_E eStmTyp, BOOL bEn);

int HY_Enc_SetZoomOSDBitMap(CChannelContext *pChannelContext, ENCODE_STREAM_E eStream, BOOL bEn);

int HY_Enc_GetZoomBitMap(CChannelContext *hChannelHandle, ENCODE_STREAM_E eStream);

int HY_changeValue(unsigned char recvBuf);

int HY_Enc_StartOsd(CChannelContext *pChannelContext, ENCODE_OSD_TYPE enOsdType, ENCODE_STREAM_E eStream);

int HY_Enc_StopOsd(CChannelContext *pChannelContext, ENCODE_OSD_TYPE enOsdType, ENCODE_STREAM_E eStream);

int HY_Enc_MD_Start(CChannelContext *pChannelContext);

int HY_Enc_MD_Stop(CChannelContext *pChannelContext);

int HY_Enc_MD_SetPrecision(CChannelContext *pChannelContext);

int HY_Enc_SetCurrentTime(int tNewTime);

int HY_Enc_SetVideoPara(CChannelContext *pChannelContext);

int HY_Enc_SetAEParam(CChannelContext *pChannelContext, AE_T *aeParam, int ae_mode);

int HY_Enc_SetAntiFlickerStrengthParam(CChannelContext *pChannelContext, int strength);

int HY_Enc_SetDefogStrengthParam(CChannelContext *pChannelContext, int strength);

int HY_Enc_SetAudioInVol(CChannelContext *pChannelContext, int vol, int IsClearCache);

int HY_Enc_SetAudioOutVol(CChannelContext *pChannelContext, int vol, int IsUpdateVoiceCache, int m_IntercomMode);

int HY_Enc_SetAudioMappingVol(CChannelContext *pChannelContext, int vol, int Type);

int HY_Enc_SetBacklightMode(CChannelContext *pChannelContext,  int enable);

int HY_Enc_SetWhiteBalance(CChannelContext *pChannelContext, WHITEBALANCE_T *WhiteBalance);

int HY_Enc_SetDayNightMode(CChannelContext * pChannelContext, int enable);

void *HY_Switch_Ircut_Twice(void *arg);

int HY_Enc_GetIRCutCheckStatus(int *pIRCutCheckStatus);

int HY_Enc_SetLocalImageMode(CChannelContext *pChannelContext, int Mode);

int HY_Enc_SetLocalExpMode(CChannelContext * pChannelContext, int ExpMode);

int HY_Enc_SetMCTFStrength(CChannelContext * pChannelContext, int MCTFStrength);

int HY_Enc_SetLocalIrcutMode(CChannelContext *pChannelContext, int Mode);

int HY_Enc_SetFlipRotateMode(CChannelContext * pChannelContext, FLIP_ROTATE_MODE_E flipRotate);

int HY_Enc_SetDayNightMode(CChannelContext * pChannelContext, int enable);

int HY_Enc_SetDcIris(CChannelContext *pChannelContext, BOOL enable);

int HY_Enc_EnableFaceDetect();

int HY_Enc_SetHumanDetectEnable(CChannelContext *pChannelContext, int bEnable);

int HY_Enc_SetHumanTrackEn(CChannelContext *pChannelContext, int bEnable);

int HY_Enc_SetHumanShapeCfgV2(CChannelContext *pChannelContext);
int HY_Enc_SetFaceCapCfgV2(CChannelContext *pChannelContext);

int HY_Enc_SetCarDetectEnable(CChannelContext *pChannelContext, int bEnable);
int HY_Enc_SetCarDetectCfg(CChannelContext *pChannelContext);

int HY_Enc_SetFaceSnapshotEnable(CChannelContext *pChannelContext, int bEnable);

int HY_Enc_GetBitrateInfo(CChannelContext *pChannelContext, int dataArr[], int arrlen);

/* 注册获取按键状态回调函数 */
int HY_Enc_RegisterKeyStautsCallBack(KEY_STATUS_CALLBACK getKeyStateCallback);

int HY_Res_SetLinkStatus(int status);

int HY_Res_SetWifiStatusLed(WIFI_STATUS_T wifiStatus);

int HY_Res_SetCurrNetCardType(XRS_NETCARD_TYPE_E status);

int HY_EnableQrScan(int bEnable);

/* 设置报警播放配置 */
int HY_Enc_SetAlarmPlayCfg(eSpeechLanguage speech_language);

/*
1.如果当前正在播放报警声，又触发了报警，则不再新添报警播报事件，但需重置播报延时
2.如果报警事件列表里已有报警事件，又触发了报警，则不再新添报警事件
*/
int HY_Enc_SetSpeechContent(SpeechType_T speech);
int HY_Enc_SwitchAOMode(int mode);
int HY_Enc_SetAudioCustomVoice(CChannelContext *hChannelHandle, int customtype, char *pBuff, int len);
/* 设置声光报警灯光持续时间*/
int HY_Enc_SetAlarmLightStayTime(int Enable, int nStayTime);
/* 开始声光报警中的灯光报警*/
int HY_Enc_SetAlarmLightStart();

//扬声器硬件使能
int HY_Enc_SetSpeakerHardwareEnable(unsigned char bEnable);

// 说明: 计算当前扬声器模式
// 返回: 扬声器模式 0=>关闭 1=>播报模式 2=>对讲模式
int HY_Speaker_CalcCurrMode(SystemInfo_t *pSysInfo);

// \brief 计算当前扬声器的下一个模式
// \param SystemInfo_t *pSysInfo 系统信息
// \param nMode 要打开/关闭的模式
// \param bEnable 打开/关闭
// \return 预计切换模式后的扬声器模式 0=>关闭 1=>播报模式 2=>对讲模式
int HY_Speaker_CalcNextMode(SystemInfo_t *pSysInfo, int nMode, int bEnable);

// 说明: 切换扬声器模式
// 输入参数: nMode 要打开/关闭的模式 bEnable 打开/关闭
// 返回: 0表示成功 其他表示失败
int HY_Speaker_ChangeMode(SystemInfo_t *pSysInfo, int nMode, int bEnable);
//-----------------------------------------------------------------------------
//	说明：		设备外设控制
//	输入参数：AssistantCtrlInfo	pCtrlInfo	控制信息
//	返回:		成功为 0 , 其它为定义的错误号；
//-----------------------------------------------------------------------------
int HY_AssistantControl(AssistantCtrlInfo *pCtrlInfo);

//-----------------------------------------------------------------------------
//	说明：		设置灯光报警
//	输入参数：nMode 灯光报警类型，0-常亮，1-闪烁        			  bOpen 打开/关闭
//	返回:		成功为 0 , 其它为定义的错误号；
//-----------------------------------------------------------------------------
int HY_Enc_SetLightAlarm(int nMode, int bOpen);

/**
 * @description: 初始化KS HalMod模块
 * @return {int} 正常返回0，异常返回<0
 */
SINT32 HY_HalModInit();

/**
 * @description: 反初始化KS HalMod
 * @return {SINT32} 正常返回0，异常返回<0
 */
SINT32 HY_HalModDeInit();

/**
 * @description: 设置系统启动状态标志
 * @param {int} Stat 启动状态标记
 * @return {int} 正常返回0，异常返回<0
 */
SINT32 HY_SetSysStartTag(int Stat);

/**
 * @description: 获取设备得唯一CHIP ID
 * @param {SINT8} *buffer 缓冲区
 * @param {SINT32} len 缓冲区长度
 * @return {SINT32} 0-成功， 非0-失败
 */
SINT32 HY_GetDevChipID(SINT8 *buffer, SINT32 len);

int HY_Enc_GetAdcStatus(int *nAdcStatus);

int HY_Enc_RestartOsd(CChannelContext *pChannelContext);

int HY_Res_GetAuthInfo(unsigned char *pszAuthLicense, unsigned int *pAuthState);

int HY_Res_SetAuthInfo(const unsigned char *pszAuthLicense);

int HY_ChangeSensorFrameRate(CChannelContext * pChannelContext, CmosFrequencyType_t antiFlickerNew);

#ifdef __ONE_CLICK_CALL__
int HY_GetCallPinStatus(int *pCallStatus, int *pHangupStatus);
int HY_GetCallPinStatus_Test(int *pCallStatus, int *pHangupStatus);
int HY_SetCallPinStatus_Test(int pCallStatus, int pHangupStatus);
int HY_Enc_SetPauseAudioAlarm();
int HY_Enc_SetAlternateResetButton(int enable);
int HY_Enc_SetRecoveryAudioAlarm();
int HY_Enc_SetKey2callhanupState(int iState);
int HY_Enc_SetKeyToCall(int enable);
int HY_Enc_SetKeyToCallHardwareType(int type);
#endif

int HY_Enc_SetAlarmOutSupport(int nSupport);
int HY_Enc_SetAlarmOutDefaultStatus(int nStatus);
int HY_Enc_SetAlarmOut(long nPort, int bOpen);

int HY_Enc_GetAlarmIn(int *pAlarmIndata);

int HY_Enc_SetAudioCodec(CChannelContext *pChCtx);

int	HY_Enc_InitEncChInfo();
int HY_Enc_InitTalkingChInfo();

int HY_GetSecretPinStatus(int *pResetStatus);

int HY_Enc_SetDormancyState(int iState);

void *SlowFrameModeSwitchThread(void * lpParameter);

int HY_Enc_GetProductMode();
int HY_Enc_SetProductMode(int nProductMode);

int HY_Enc_SetAudioDetectCfg(CChannelContext *pChannelContext, LpAudioDetectCfg pCfg);

int HY_Enc_SwitchAecMode(int AecMode);

int HY_Enc_GetWiFiInfo(void *lib_wifiInfo);

#if defined(__IS_SUPPORT_LCD_MANAGER__)
int HY_Enc_LCDRefreshFrame(char *pData, unsigned int DataSize);

int HY_Enc_LCDSetBackLightLevel(int level);

int HY_Enc_LCDCreateVdecChn(int nType);

int HY_Enc_LCDSendStreamToChn(int nType, char *pData, unsigned int DataSize);

int HY_Enc_LCDDestoryVdecChn(int nType);
#endif

SINT32 HY_Res_P2PNotifySendDataFail(const char * pBuffer, UINT32 dwBufLen);

int HY_Enc_GetRealKeyFrameInterval(int nChannel,int *nFrameRateInterval,int *nKeyFrameInterval);

int HY_Enc_StopUserSet(CChannelContext *pChannelContext, ENCODE_STREAM_E eStream, UserSetParamFlag_t *m_UserSetParam);

int HY_Enc_StartUserSet(CChannelContext *pChannelContext, ENCODE_STREAM_E eStream, UserSetParamFlag_t *m_UserSetParam);

#endif

