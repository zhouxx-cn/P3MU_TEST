#ifndef  _ZWSDK_H_
#define  _ZWSDK_H_

/*******************************************
 武汉卓威视讯
 yongc
 20120816
 本文件属于SDK，是SDK接口定义
 *******************************************/
#define UNIQUEID_BUF_LEN (16+1)

typedef enum 
{
    StandardNone    = 0x80000000,
    StandardNTSC    = 0x00000001,
    StandardPAL     = 0x00000002,
    StandardSECAM   = 0x00000004,
} VideoStandard_t, VideoOutStandard_t;

typedef struct
{
    long    left;     /* 区域的左边与图像的最左边的距离 */
    long    top;      /* 区域的上边与图像的最上边的距离 */
    long    right;    /* 区域的右边与图像的最左边的距离 */
    long    bottom;   /* 区域的下边与图像的最上边的距离 */
} RECTH, *PRECTH, *LPRECTH;

typedef struct
{
    long    x;
    long    y;
} VECTOR, *PVECTOR, *LPVECTOR;

typedef struct
{
    int nValidNum;
    VECTOR point[8];
} POLYGON, *PPOLYGON, *LPPOLYGON;

typedef struct
{
    VECTOR point[4];
} POLYGON4, *PPOLYGON4, *LPPOLYGON4;

typedef struct
{
	VECTOR BeginPoint;
	VECTOR EndPoint;
} LINE, *PLINE, *LPLINE;

typedef struct
{
    float    x;
    float    y;
} VECTOR_FLOAT, *PVECTOR_FLOAT, *LPVECTOR_FLOAT;

typedef struct
{
    int nValidNum;
    VECTOR_FLOAT point[8];
} POLYGON_FLOAT, *PPOLYGON_FLOAT, *LPPOLYGON_FLOAT;

/* 视频帧 */
typedef enum 
{
	PktError            = 0x00,	
    PktIFrames          = 0x01, //sensor1 main I
    PktPFrames          = 0x02, //sensor1 main P
    PktMJPEG            = 0x03,
    PktBPFrames         = 0x20,
    PktBBPFrames        = 0x04,
    PktAudioFrames      = 0x08,
    PktQCIFIFrames      = 0x10, //sensor1 aux I
    PktQCIFPFrames      = 0x40, //sensor1 aux P
    PktBIFrames         = 0x90,
    PktBBIFrames        = 0xC0,
    PktSQCIFIFrames		= 0x11, //sensor1 hdaux I
    PktSQCIFPFrames		= 0x41, //sensor1 hdaux P
    PktSysHeader        = 0x80,
    PktSysHeaderAux		= 0x81,
    PktSysHeaderHDAux 	= 0x82,
    PktMotionDetection  = 0x83,
    PktAimDetection     = 0x84,
    PktPrivateData      = 0x85,
    PktIAFrames         = 0x86,//人形帧，就是一张jpg图
    PktIAFramesForStream = 0x86+0xff,   //供设备内部使用，用于区分智能事件帧与智能码流帧
    PktOrigImage        = 0x87,
	PktFourthIFrames	= 0x42,
	PktFourthPFrames	= 0x43,
	PktCaptureIFrames	= 0x46,
	PktCapturePFrames	= 0x47,
	PktAdvFaceFrames	= 0xA0,
    PktIACarFrames      = 0xA1,//车形帧，就是一张jpg图
    PktIAElectronicFenceFrames    = 0xA2,//电子围栏
	PktIACrossBoardDetectFrames   = 0xA3,//越界侦测
	PktIAOffDutyDetectFrames      = 0xA4,//离岗侦测
	PktIAPassengerFlowFrames      = 0xA5,//客流统计
    PktIAFireDetectFrames         = 0xA6,//火焰
    PktIAPassengerFlowHumanEventFrames = 0xA7,//客流统计人形事件
    PktIAVideoShelterDetectEventFrames = 0xA8,//视频遮挡检测事件
	PktIAFaceSnapshotEventFrames = 0xA9,//人脸抓拍检测事件
	PktIAMaskDetectEventFrames = 0xAA,//口罩检测事件
    PktIFramesForS1     = 0xB0, //sensor2 main I
    PktPFramesForS1     = 0xB1, //sensor2 main P
    PktQCIFIFramesForS1 = 0xB2, //sensor2 aux I
    PktQCIFPFramesForS1 = 0xB3, //sensor2 aux P
	PktFourthIFramesForS1 = 0xB4, //sensor2 mjpeg I
	PktFourthPFramesForS1 = 0xB5, //sensor2 mjpeg P
    PktIFramesForS2     = 0xB6, //sensor3 main I
    PktPFramesForS2     = 0xB7, //sensor3 main P
    PktQCIFIFramesForS2 = 0xB8, //sensor3 aux I
    PktQCIFPFramesForS2 = 0xB9, //sensor3 aux P
	PktFourthIFramesForS2 = 0xBA, //sensor3 mjpeg I
	PktFourthPFramesForS2 = 0xBB, //sensor3 mjpeg P
    PktIFramesForS3     = 0xBC, //sensor4 main I
    PktPFramesForS3     = 0xBD, //sensor4 main P
    PktQCIFIFramesForS3 = 0xBE, //sensor4 aux I
    PktQCIFPFramesForS3 = 0xBF, //sensor4 aux P
	PktFourthIFramesForS3 = 0xC0, //sensor4 mjpeg I
	PktFourthPFramesForS3 = 0xC1, //sensor4 mjpeg P
} FrameType_t;

/* 视频帧编码类型 */
typedef enum 
{
	H264    = 0x00,
	H265	= 0x04,
	MJPEG   = 0x05
} VideoEncodeType_t;   

//编码设置类型
typedef enum 
{
    LtH264Video         = 0x00,	
    LtH265Video         = 0x01,
	LtMjpegVideo        = 0x02,
    LtH265PlusVideo     = 0x03,
    LtH264PlusVideo     = 0x04,
	LtEncTypeEnd,
} EncodeType_t;

/* 音频编码类型 */
typedef enum 
{
    LtG726Audio24Kbps   = 0x00,	
    LtG711AudioALaw     = 0x01,  
    LtG711AudioULaw     = 0x02,  
    LtG729Audio         = 0x03,  
    LtG723Audio1        = 0x04,  
    LtMP3Audio          = 0x05,  
    LtAdpcmAudioIma     = 0x06,  
    LtAdpcmAudioDvi4    = 0x07,  
    LtAmrAudio          = 0x08,  
    LtAacAudio          = 0x09,
} Audio_LoadType_t;

/* OSD显示时间模式 */
typedef enum 
{
    Mode_YYYY_MM_DD     = 0,
    Mode_MM_DD_YYYY     = 1,
    Mode_DD_MM_YYYY     = 2,
} OSDTimeMode_t;

typedef struct TimeOSDInfo_t
{
    int             FontWidth;              //字符宽度
    int             FontHeight;             //字符高度
    int             Start_X;                //起始X坐标
    int             Start_Y;                //起始Y坐标
    int             bTranslant;             //是否透明
    int             bHour24;                //是否24小时制
    OSDTimeMode_t   DisMod;                 //0,表示年月日, =1，表示月日年
    const char      *pFontPixinfo;          //15个关键字的点阵信息：数字0-9,字母"A"，"P"，"M"，"："，"-"
} TimeOSDInfo_t;

typedef struct	TextOSDInfoEx_s
{
	int				Flag;					//0:对text的扩展，1：多行OSD
    int             TextNum;                //文本字符个数
    int             Start_X;                //起始X坐标
    int             Start_Y;                //起始Y坐标
    char            *pPixinfo;              //点阵信息(每个字符的宽、高各占一个字节)
    unsigned short  strOSD[256];
    unsigned short	strGB2312[256];			//GB2312字符信息，不带换行符
} TextOSDInfoEx_t;

// OSDV2 包含中括号的长度
#define OSDV2_TEXT_KEYWORD_MAX_LENGTH	(6)
#define OSDV2_TEXT_KEYWORD_MIN_LENGTH	(3)

#define OSDV2_TEXT_MAX_LENGTH			(180)
#define OSDV2_TEXT_LENGTH				(512)
#define OSDV2_TEXT_TOKEN_LEN			(32)
#define OSDV2_TEXT_CNT					(4)
// 5行文字，1行附加信息
#define OSDV1_TEXT_MAX_LINE             (3) //单行OSD兼容^换行逻辑，最大支持5行，不支持附加信息
#define OSDV2_TEXT_MAX_LINE				(5+1)

#define OSDV2_TEXT_MAX_LINE_WIDTH		(30)
#define OSDV2_TEXT_KEYWORD_MAX_ONE_LINE	(2)

#ifdef __DIGITAL_ZOOM_DRAG_SUPPORT__
/* 手动变倍命令 */
typedef enum {
	DRAG_CMD_ZOOMST = 0x00,          //拖动变倍枚举起始
	DRAG_CMD_ZOOMIN = 0x01,          //Zoom放大
	DRAG_CMD_ZOOMOUT = 0x02,         //Zoom缩小
	DRAG_CMD_ZOOM_STOP = 0x03,       //停止变倍
	DRAG_CMD_ZOOM_RECOVER = 0x04,    //恢复变倍
} DRAG_ZOOM_CMD;

/* 变倍操作者 */
typedef enum {
	DRAG_NONE = 0x00,          		//无操作者
	DRAG_CLIENT_RADIO = 0x01,          //客户端下发倍数
	DRAG_CLIENT_ZOOM = 0x02,       //客户端下发变倍命令
	DRAG_CRUISE_RADIO = 0x03,          //巡航下发倍数
	DRAG_TRACK_RADIO = 0x04,         //跟踪下发倍数
} DRAG_ZOOM_OPERATOR;
#endif

typedef enum OSDV2TextKeywordType_e
{
	OSDV2_KW_NONE = 0,
	OSDV2_KW_RES,	// 分辨率
	OSDV2_KW_BPS,	// 码率
	OSDV2_KW_LGT,	// 灯板相关
	OSDV2_KW_FLOW,
	OSDV2_KW_TMP,
	OSDV2_KW_MAX,
}OSDV2TextKeywordType_t;

typedef struct OSDV2TextKeyword_s
{
	int type;
	int pixWidth;
	int pos;
	int wordLen;
	unsigned short word[OSDV2_TEXT_KEYWORD_MAX_LENGTH];
} OSDV2TextKeyword_t;

typedef struct OSDV2TextLineInfo_s
{
	int pixWidth;
	int keywordNum;
	OSDV2TextKeyword_t keyWords[OSDV2_TEXT_KEYWORD_MAX_ONE_LINE];
} OSDV2TextLineInfo_t;

typedef struct	TextOSDV2Info_s
{
	int				Flag;					//0:对text的扩展，1：多行OSD
    int             Start_X;                //起始X坐标
    int             Start_Y;                //起始Y坐标
    char            *pPixinfo;              //点阵信息(每个字符的宽、高各占一个字节)
	int 				lineNum;
	OSDV2TextLineInfo_t lineInfo[OSDV2_TEXT_MAX_LINE];
	int             keyWordFontWidth;              //字符宽度
    int             keyWordFontHeight;             //字符高度
	const char      *pKeywordPixInfo;	           //附加信息可能用到的点阵信息
	unsigned short	text[OSDV2_TEXT_MAX_LENGTH];   //GB2312 OSD字符串
}TextOSDV2Info_t;

typedef struct
{
	int				valid;
	int				FontWidth;				//字符宽度
    int				FontHeight;				//字符高度
	char			*pBlackFontInfo;		//关键字的点阵(黑色)
    char			*pWhiteFontInfo;		//关键字的点阵(白色)
    int 				lineNum;
	OSDV2TextLineInfo_t lineInfo[OSDV2_TEXT_MAX_LINE];
	unsigned short	text[OSDV2_TEXT_MAX_LENGTH];
} SysTextOSDV2_t;

typedef struct	TextOSDInfo_t
{
    int             TextNum;                //文本字符个数
    int             Start_X;                //起始X坐标
    int             Start_Y;                //起始Y坐标
    char            *pPixinfo;              //点阵信息(每个字符的宽、高各占一个字节)
	int 			lineNum;
	OSDV2TextLineInfo_t lineInfo[OSDV2_TEXT_MAX_LINE];
	unsigned short  strOSD[21];
    unsigned short	strGB2312[21];			//GB2312字符信息，不带换行符
}TextOSDInfo_t;

typedef enum 
{
	ENC_CIF_FORMAT		= 0,
	ENC_QCIF_FORMAT 	= 1,
	ENC_2CIF_FORMAT 	= 2,
	ENC_MD_FORMAT		= 3,
	ENC_D1_FORMAT		= 4,
	ENC_DCIF_FORMAT 	= 5,
	ENC_SD1_FORMAT		= 6,
	ENC_360P_FORMAT 	= 7,		// 640x360
	ENC_QVGA_FORMAT 	= 8,		//QVGA(320*240)
	ENC_VGA_FORMAT		= 9,		//VGA(640*480)
	ENC_SVGA_FORMAT 	= 10,		//SVGA(800*600)
	ENC_960H_FORMAT 	= 11,		//(960*576)
	ENC_XVGA_FORMAT 	= 12,		//XVGA(1024*768)
	ENC_HD720_FORMAT	= 13,		//HD720(1280*720)
	ENC_QuadVGA_FORMAT	= 14,		//QuadVGA(1280*960)
	ENC_SXGA_FORMAT 	= 15,		//SXGA(1280*1024)
	ENC_UXGA_FORMAT 	= 16,		//UXGA(1600*1200)
	ENC_HD1080_FORMAT	= 17,		//HD1080(1920*1080)
	ENC_QXGA_FORMAT 	= 18,		//QXGA(2304*1296)
	ENC_UWXGA_FORMAT	= 19,		//UWXGA(2592×1520)
	ENC_USXGA_FORMAT	= 20,		//USXGA(2592×1944)
	ENC_FORMAT_NUMS
}PictureFormat_t;

typedef struct
{
    unsigned short int				m_PicWidth;
    unsigned short int              m_PicHeight;
}ResolutionParam_t ;

typedef struct CODEC_PARA_S_t
{
    union {
        PictureFormat_t PictureFormat;       //编码尺寸
        ResolutionParam_t PictureRes;        //编码真实分辨率
    };

    int             BitRate;                //码率
    int             FrameRate;              //帧率
    int             PicQualityLever;        //编码图像质量等级，vbr有效0~5, 0最高
    int             BitRateCtlMode;         //码率控制类型：1: CBR; 0: VBR; 
    int             KeyFrameInterval;       //关键帧间隔
    EncodeType_t 	EncType;				//编码类型
}CODEC_PARA_S;

typedef struct CODEC_PARA_S_ext
{
    int             KeyFrameInterval;       //关键帧间隔
}CODEC_PARA_S_EXT;

typedef struct tagMotionData
{
    PictureFormat_t PicFormat;
    unsigned long   HorizeBlocks;
    unsigned long   VerticalBlocks;
    unsigned long   Stride;
}MOTION_DATA_HEADER, *PMOTION_DATA_HEADER;

typedef enum {
	FR_NORMAL = 0,
	FR_ROTATE_180 = 1,//	
	FR_HORI_FLIP = 2,//水平镜像
	FR_VERT_FLIP = 3,//垂直镜像
	FR_MAX = FR_VERT_FLIP,
} FLIP_ROTATE_MODE_E;

typedef enum {
	CS_TV_STYLE_VOUT = 0,
	CS_PC_STYLE_VOUT = 1,
} CHROMA_SCALE_E;

typedef enum
{
    VO_MODE_1MUX = 0,
    VO_MODE_2MUX = 1,
    VO_MODE_BUTT
}VO_MODE_E;
    
typedef enum
{
    SAMPLE_RC_CBR = 0,
    SAMPLE_RC_VBR,
    SAMPLE_RC_FIXQP
}RC_E;

typedef struct
{
    int s32ViDevCnt;         // VI Dev Total Count
    int s32ViDevInterval;    // Vi Dev Interval
    int s32ViChnCnt;         // Vi Chn Total Count
    int s32ViChnInterval;    // VI Chn Interval
}VI_PARAM_S;

typedef struct{
	unsigned int protocol;
	unsigned int addr;
	unsigned int baudRate;
}PTZParam_t;

/* PTZ状态 */
typedef enum tagPtzStatus
{
    ePtzStatus_Stop = 0,    // PTZ静止状态
    ePtzStatus_Moving,      // PTZ运动状态
}ePtzStatus;

/* PTZ回预置点状态 */
typedef enum tagPtz2PresetStatus
{
	ePtz2Preset_backing = 0, // 正在回
	ePtz2Preset_fulfil = 1,	 // 完成
}ePtz2PresetStatus;

typedef enum
{
	CMOS_FREQUENCY_50HZ  = 0,	//50HZ
	CMOS_FREQUENCY_60HZ  = 1,	//60HZ
	CMOS_FREQUENCY_CLOSE = 2,
} CmosFrequencyType_t;

typedef enum
{
	AE_MODE_AUTO    = 0,	
	AE_MODE_MANUAL  = 1,	
} AE_MODE_E;

typedef enum
{
	WB_MODE_INCANDS  = 0, /*白炽灯*/
	WB_MODE_WARM     = 1, /*暖光灯*/
	WB_MODE_FLUORES  = 2, /*日光灯*/
	WB_MODE_NATURAL  = 3, /*自然光*/
	WB_MODE_MANUAL   = 4, /*手动白平衡*/
	WB_MODE_AUTO     = 5, /*自动白平衡*/
	WB_MODE_LOCK     = 6, /*锁定白平衡*/	
}WBWorkMode_E;

typedef struct whiteBalance_s
{
	int WBEnable;
	int WBWorkMode;
	unsigned char RedGain;
	unsigned char BlueGain;
}WHITEBALANCE_T;

typedef struct ae_s
{
	CmosFrequencyType_t	antiFlicker;//sensor input frequency
	unsigned int MinShutterTime;//快门时间
	unsigned int MaxShutterTime;
	unsigned int sensorGain;//sensor 增益
//	u32 slow_shutter_enable;
}AE_T;

//4G模组型号
typedef enum {
	UNKNOW_MODULE,			//未知模组
	QUECTEL_EC800,			//移远EC800
	ZHONGYUN_ZX800, 		//中云信安 ZX800		
	QUECTEL_EG800,			//移远EG800
	ZHONGYUN_ZX1602,        //中云信安 ZX1602		
	ZHONGYUN_ZX800SG,       //中云信安 ZX800-SG
	ZHONGYUN_ZX800SG_S,     //中云信安 ZX800-SG 欧版
	ZHONGYUN_ZX800SG_A,     //中云信安 ZX800-SG 美版
	ZHONGYUN_ZX1605,        //中云信安 ZX1605	
	ZHONGYUN_ZX800HG,		//中云信安 ZX800-HG 海思4G模组
	ZHONGYUN_ZX800HG_S,		//中云信安 ZX800-HG 欧版
	ZHONGYUN_ZX800HG_A,     //中云信安 ZX800-HG 美版
}e4GModuleType;

//4G运营商类型
typedef enum {
	eUnknowOperator,			//未知运营商
	eChinaUnicom,				//中国联通
	eChinaMobile, 				//中国移动		
	eChinaTelecom,				//中国电信
	eOtherOperator,        		//其他运营商	
}e4GTelecomOperatorType;

#define FORWARD_4G_NUMBER	"IOTDBB"		//4G运营模式号段
#define FORWARD_4G_NUMBER1	"IOTDEE"		//4G运营模式号段 森亦杨
#define FORWARD_4G_NUMBER2	"IOTDFF"		//4G运营模式号段 云瑞
#define FORWARD_4G_NUMBER3	"IOTDGG"		//4G运营模式号段 隆图
#define FORWARD_4G_NUMBER4	"IOTGAA"		//4G运营模式号段 畅享

#define SUPPORT_4G_IOTF     "IOTF"          //4G只支持该号段为普通号段
#define SUPPORT_4G_YLTB     "YLTB"          //4G只支持该号段为伊朗,也是普通号段
#define SUPPORT_4G_IOTH	    "IOTH"			//4G运营模式号段非洲号段 

//4G客户类型
typedef enum {
	CUSTOMTYPE_NOTFLOW = 0,				//非绑卡号段
	CUSTOMTYPE_XINRUISHI,				//对应IOTDBB    
	CUSTOMTYPE_SUNYIYANG,				//对应IOTDEE    森亦杨  
	CUSTOMTYPE_YUNRUI,					//对应IOTDFF    云瑞
	CUSTOMTYPE_LONGTU,					//对应IOTDGG    隆图
	CUSTOMTYPE_XINRUISHI_NORMAL,		//对应普通号段
}eFORWARD_4G_CUSTOMTYPE;

/**************************************************************/
/*!
*  @enum	eVoiceMode，保持和中间层一致
*  @brief	语音编解码模式
*/

typedef enum 
{
	eCodecBegin = 0,			/*!< 起始值						 */
	eCodecG726, 				/*!< G726编解码模式				 */
	eCodecG711U,				/*!< G711U编解码模式				 */
	eCodecMPEG4AAC, 			/*!< MPEG4 AAC编解码模式			   */
	eCodecAutoDiscern,			/*!< 自动识别编解码模式				   */
	eCodecADPCMima, 			/*!< ADPCMima编解码模式			   */
	eCodecG711A,				/*!< G711A编解码模式				 */
	eCodecPCM,					/*!< PCM编解码模式					        */
	eCodecModeEnd,				/*!< 结束值						 */ 
} AudioEncodeMode_t;

typedef enum{
	IMG_STANDARD_TYPE =0,
	IMG_SOFT_TYPE,
	IMG_SHARP_TYPE,
	IMG_CUSTOM_TYPE
}color_type_E;

/* 音频输入类型 */
typedef enum 
{
	MIC_IN		    = 0,
	LINE_IN			= 1,
} AudioChannelType_t;

/* 动作等级枚举 */
typedef enum {
   LBR_MOTION_NONE = 0,
   LBR_MOTION_LOW  = 1,
   LBR_MOTION_HIGH = 2,     //默认
} LBR_MOTION_LEVEL;

/* 噪声等级枚举 */
typedef enum {
  LBR_NOISE_NONE = 0,
  LBR_NOISE_LOW  = 1,
  LBR_NOISE_HIGH = 2,       //默认
} LBR_NOISE_LEVEL;

/* LBR等级枚举 */
typedef enum{
    LBR_STYLE_FPS_KEEP_BITRATE_AUTO      = 0,  //try to keep max fps,  bitrate auto drop when possible (default, consumer style)
    LBR_STYLE_QUALITY_KEEP_FPS_AUTO_DROP = 1,  //similar to default, but fps can auto drop when no budget to keep quality  (Quality keeping)
    LBR_STYLE_FPS_KEEP_CBR_ALIKE         = 2,  //FPS keep, CBR alike.  (traditional professional IPCAM style)
}LBR_STYLE;

typedef struct LBR_Info
{
	int				 autoMode;
	int 			 ibitrate_target;       // 码率上限,单位bps   [64k, 20M]
	LBR_STYLE        iLbr_style;            // SmartAVC风格:   0;1;2  默认为0
	LBR_MOTION_LEVEL iMotion_level;         // 移动等级    :   0;1;2  默认为2
	LBR_NOISE_LEVEL  iNoise_level;	        // 噪声等级    :   0;1;2  默认为2
}LBR_INFO, *LPLBR_INFO;

typedef enum WIFIFREQ{
    tow_fourGener = 1,
    fifthGener = 3,
}WifiFreqMode;



typedef enum 
{
	LOGO_SYSTEM        = 0,			//系统Logo(9画面时显示)
	LOGO_DECNORECORD   = 1,			//回放无录像文件时显示
	LOGO_NOVIDEO	   = 2,			//无视频信号时显示图片
	LOGO_LOCK	       = 3,			//锁屏使用的LOGO
}LogoType_t; 

/* 传感器类型 */
typedef enum {
     CMOS_SENSOR_NULL   = 0,
	 CMOS_SENSOR_GC2063 = 1,
	 CMOS_SENSOR_GC2083 = 2,
	 CMOS_SENSOR_GC4653 = 3,
     CMOS_SENSOR_JXF38P = 4,
}BoardSensorType_t;

typedef struct MotionDetectGridToi_s
{
	unsigned char *	listOfAreas;		//指向24个字节的区域信息
	int 			nBlockNum;
	unsigned short	threshold;		//0~200
	unsigned short	sensitivity;    //5    20    50
} MotionDetectGridToi_t;

typedef struct	PointOSDInfo_s
{
	unsigned short 	x;
	unsigned short 	y;
	unsigned short 	width;
	unsigned short 	height;
	unsigned char	fontSize;
	unsigned char   *data;
}PointOSDInfo_t;

typedef enum {
    eSpeechPlayIdle   = 0, // 播放器空闲
    eSpeechPlaying    = 1, // 正在播放
} E_SpeechPlayStatus;

//语音播报语言,定义已WIKI上为准，新添语言需要申请
typedef enum tagSpeechLanguage
{
    eSpeechLanguage_Didi    = 0, //滴滴
    eSpeechLanguage_Chinese = 1, //中
    eSpeechLanguage_English = 2, //英
    eSpeechLanguage_Vietnamese = 3, //越南语
    eSpeechLanguage_Indonesian = 4, //印尼语
    eSpeechLanguage_Spanish = 5, //西班牙语
    eSpeechLanguage_Portuguese = 6, //葡萄牙语
    eSpeechLanguage_Korean = 7, //韩语
    eSpeechLanguage_Thai = 8, // 泰语
    eSpeechLanguage_Japanese = 9, //日语
    eSpeechLanguage_German = 10, //德语
    eSpeechLanguage_Italian = 11, //意大利语
    eSpeechLanguage_Turkish = 12, //土耳其语
    eSpeechLanguage_Polish = 13, //波兰语
    eSpeechLanguage_French = 14, //法语
    eSpeechLanguage_Czech = 15, //捷克语
    eSpeechLanguage_Greek = 16, //希腊语
    eSpeechLanguage_Persian = 17, //波斯语
    eSpeechLanguage_Hebrew = 18, //希伯来
    eSpeechLanguage_Arabic = 19, //阿拉伯
    eSpeechLanguage_Russian = 20, //俄语
    eSpeechLanguage_Other = 21, // 自定义
#ifdef __SUPPORT_PRODUCT_TEST_CUSTOM_SPEECH_FILES__
    eSpeechLanguage_None = 22, // 忽略
#endif
    eSpeechLanguage_Num
} eSpeechLanguage;

typedef enum _XRS_NETCARD_TYPE_E
{
	eXRS_NETCARD_WIRED_Unknown = 0,
	eXRS_NETCARD_WIRED_1 = 1,		// 有线网卡1
	eXRS_NETCARD_WIRED_2 = 2,			// 有线网卡2
	eXRS_NETCARD_WIFI = 3,			// 无线网卡
	eXRS_NETCARD_3G = 4,			// 3G网络
	eXRS_NETCARD_4G = 5,			// 4G网络
	eXRS_NETCARD_5G = 6		    	// 5G网络
}XRS_NETCARD_TYPE_E;

/* wifi状态 */
typedef enum {
  IN_AP_STATUS = 0,//wifi处于AP 模式
  IN_WIFI_CONNECTING  = 1,//wifi处于STA模式，正在连接
  IN_WIFI_CONNECTED = 2,//wifi处于STA模式，已连接上
  IN_WIFI_KEY_STATUS_AP_VALUE = 3,//按键状态已经达到ap功能
  IN_WIFI_KEY_STATUS_RESET_VALUE = 4,//按键状态已经达到reset功能
  IN_WIFI_NULL,
} WIFI_STATUS_T;

typedef enum
{
	SPEECH_WIFI_IN_AP_STATUS = 1,                       //wifi处于AP 模式
	SPEECH_WIFI_CONNECTING  = 2,						//WIFI连接中请稍等
	SPEECH_WIFI_CONNECT_SUCCESS = 3,					//WIFI连接成功
	SPEECH_WIFI_CONNECT_FAIL = 4,						//WIFI连接失败
	SPEECH_WIFI_CONNECT_FAIL_PWD_WRONG = 5,				//wifi连接失败，密码错误
	SPEECH_WIFI_AP_BUILD = 6,							//热点已建立
	SPEECH_WIFI_CONNECT_FAIL_PLEASE_AGAIN = 7,          //配网失败请重试
    SPEECH_WIFI_PLEASE_CONNECT = 8,                     //请使用扫码连接路由器
    SPEECH_NET_CONNECT_SUCCESS = 9,                     //设备配网成功

	SPEECH_ALARM_MOTION = 10,							//移动报警
	SPEECH_ALARM_HUMAN = 11,							//人形报警
	SPEECH_REMINDER_SCHEDULE_1 = 12,                    //定时提醒语音1
	SPEECH_REMINDER_SCHEDULE_2 = 13,                    //定时提醒语音2
	SPEECH_REMINDER_SCHEDULE_3 = 14,                    //定时提醒语音3
	SPEECH_REMINDER_SCHEDULE_TEST = 15,                 //定时提醒语音测试

	SPEECH_UPGRADE_START = 20,							//开始升级请勿断电
	SPEECH_UPGRADE_END_REBOOT = 21,						//升级完成即将重启
	SPEECH_UPGRADE_FAIL = 22,							//升级失败请重新升级
	SPEECH_UPGRADE_FAIL_ERROR_VERSION = 23,				//升级失败请检查升级包

	SPEECH_SYS_RESET = 30,								//系统正在复位	  
	SPEECH_SYS_STARTING = 31,							//设备启动中
	SPEECH_SYS_START_OVER = 32,							//设备启动完成
    SPEECH_SYS_QRCODE_SCAN_OVER = 33,                   //二维码扫描完成
    SPEECH_SYS_RESET_SUCCESS = 34,						//复位成功
	SPEECH_FACTORY_AUDIO_TEST = 35,                     //生产测试用-音频测试

    SPEECH_KEY2CALL_START = 40,                         //你好，摄像机正在呼叫家人
    SPEECH_KEY2CALL_OFFLINE = 41,                       //设备不在线
    SPEECH_KEY2CALL_DU = 42,                            //嘟嘟
    SPEECH_KEY2CALL_NO_ANSWER = 43,                     //暂时无人接听，已为你推送视频

    SPEECH_4G_SYS_RESET = 90,							//4G 系统正在复位
	SPEECH_4G_SYS_RESET_SUCCESS = 91,					//4G 系统复位成功
	SPEECH_4G_SYS_STARTING = 92,						//4G 设备启动中
	SPEECH_4G_SYS_START_OVER = 93,						//4G 设备启动完成
	SPEECH_4G_USE_WIRED = 94,							//4G 使用有线网络
	SPEECH_4G_CONNECT_SUCCESS = 95,						//4G 网络连接成功
	SPEECH_4G_CONNECT_FAILED = 96,						//4G 网络连接失败
	SPEECH_4G_MODULE_CHECK_SUCCESS = 97,				//4G 模块检测成功
	SPEECH_4G_MODULE_CHECK_FAILED = 98,					//4G 模块检测失败
	SPEECH_4G_SIM_CHECK_SUCCESS = 99,					//4G 内置卡检测成功
	SPEECH_4G_SIM_CHECK_FAILED = 100,					//4G 内置卡检测失败
	SPEECH_4G_SIM_NOT_ORIGINAL = 101,					//非原厂卡，网络连接会失败
	SPEECH_4G_SIGNAL_QUALITY = 102,
	SPEECH_4G_SIGNAL_0 = 103,
	SPEECH_4G_SIGNAL_1 = 104,
	SPEECH_4G_SIGNAL_2 = 105,
	SPEECH_4G_SIGNAL_3 = 106,
	SPEECH_4G_SIGNAL_4 = 107,
	SPEECH_4G_SIGNAL_5 = 108,	
	SPEECH_4G_SIGNAL_6 = 109,
	SPEECH_4G_SIGNAL_7 = 110,
	SPEECH_4G_SIGNAL_8 = 111,
	SPEECH_4G_SIGNAL_9 = 112,
	SPEECH_4G_SIGNAL_10 = 113,
	SPEECH_4G_SIGNAL_MAXNUM = (SPEECH_4G_SIGNAL_0+99),  //202 4G信号质量最大为99
    SPEECH_4G_CONNECT_FAILED_CHECK_SIMCARD = 210,       // 入网失败，请检查sim卡
	SPEECH_4G_CARDTYPE_ERROR = 211, 				    // 4G卡查询失败,错误码X
	SPEECH_4G_CARDTYPE_ERRORCODE1 = 212,
	SPEECH_4G_CARDTYPE_ERRORCODE2 = 213,
	SPEECH_4G_CARDTYPE_ERRORCODE3 = 214,
	SPEECH_4G_CARDTYPE_ERRORCODE4 = 215,
	SPEECH_4G_CARDTYPE_ERRORCODE5 = 216,
	SPEECH_4G_CARDTYPE_ERRORCODE6 = 217,
	SPEECH_4G_CONNECTING_PLEASE_WAIT = 218,             // creg入网过程中： 网络连接中请稍候
	SPEECH_4G_INSUFFICIENT_BALANCE = 219,               //设备余额不足，网络连接失败

	SPEECH_AGING_TEST = 250,							 //老化测试
	SPEECH_MAX
} SpeechType_T;

#define IS_ALARM_SPEECH_TYPE(type) (type >= SPEECH_ALARM_MOTION && type <= SPEECH_ALARM_HUMAN)
#define IS_REMIND_SPEECH_TYPE(type) (type >= SPEECH_REMINDER_SCHEDULE_1 && type <= SPEECH_REMINDER_SCHEDULE_TEST)

typedef enum tagSpeechRemindType
{
	eSpeechType_Remind_1 = 0, //定时提醒语音1
	eSpeechType_Remind_2 = 1, //定时提醒语音2
	eSpeechType_Remind_3 = 2, //定时提醒语音3
	eSpeechType_Remind_TEST = 3, //定时提醒语音测试
	eSpeechTypeRemindMax
} eSpeechRemindType;

typedef struct tagSpeechRemindTypeMap{
	eSpeechRemindType 	nSpeechType;				//语音播报类型
	SpeechType_T 	    eAlarmType;					//声音报警类型
	char 			    sCustomSpeechFileName[64];	//自定义语音名称
}stuSpeechRemindTypeMap;

//定时提醒语音配置整合
typedef struct	tagSpeechRemindInfo_s
{
	/* 0:自定义1, 1:卡农, 2:爱丽丝, 3:电话铃声, 4:闹铃, 5:鸟鸣 6:自定义2, 7:自定义3 */
	unsigned char 	remindSpeechType;	//报警播报类型
	unsigned char 	enable;             //语音播报的输出使能
	unsigned char 	broadCastTimes;		//播报次数
	unsigned char 	broadCastVolume;	//播报音量 1~100
}SpeechRemindInfo_t;

//报警语音配置整合
typedef struct	tagSpeechAlarmInfo_s
{
    /* 0:自定义, 1:警号, 2:犬吠 */
	unsigned char 	alarmSpeechType;    //报警播报类型
	unsigned char 	enable;             //语音播报的输出使能
	unsigned short 	alarmOutDelay;      //报警延时输出的时间
	unsigned char 	broadCastVolume;    //播报音量 1~100
	unsigned char   broadCastTimes;     //播报次数
	unsigned char	reserve[7];
}SpeechAlarmInfo_t;

typedef enum
{
	IRCUT_PASSIVITY = 0,//自动(被动模式)
	IRCUT_DAY = 1,//白天
	IRCUT_NIGHT = 2,//夜晚
	IRCUT_TIME = 3,//定时
	IRCUT_INITIATIVE = 4,//自动(主动模式)
	IRCUT_COLOR_NIGHT = 5,//夜视全彩
	IRCUT_INTELLIGENT_NIGHT = 6,//智能全彩
	IRCUT_INTELLIGENT_REDRAY  =7, //智能红外(强光抑制)
	IRCUT_INTELLIGENT_WHITELIGHT = 8,//智能白光PWM
	IRCUT_NORMAL_WHITELIGHT = 9 //普通白光
}DAYNIGHTMODE_T;

typedef enum {
	SDK_ERROR_LEVEL = 0,
	SDK_DEBUG_LEVEL = 1,
	SDK_INFO_LEVEL  = 2,
}sdk_log_level,PrintLogLevel_t;

/**
 * 夜灯标志位枚举
 * 初始值0，收到开启命令1，收到关闭命令2
 */
typedef enum
{
    NIGNTLED_NULL  = 0,//默认
	NIGNTLED_START = 1,//开启夜灯
	NIGNTLED_STOP  = 2,//关闭夜灯
	NIGNTLED_ON    = 3,
}NightLedCommand_T;

typedef struct tagIrcutTimeModeInfo
{
	unsigned char nStartHour;		/*!< 当nMode等于3时有效, 夜晚开始小时,范围[0,23] */
	unsigned char nStartMin;		/*!< 当nMode等于3时有效, 夜晚开始分钟,范围[0,59] */
	unsigned char nStartSec;		/*!< 当nMode等于3时有效, 夜晚开始秒,  范围[0,59] */

	unsigned char nStopHour;		/*!< 当nMode等于3时有效, 夜晚结束小时,范围[0,23] */
	unsigned char nStopMin;		    /*!< 当nMode等于3时有效, 夜晚结束分钟,范围[0,59] */
	unsigned char nStopSec;		    /*!< 当nMode等于3时有效, 夜晚结束秒,  范围[0,59] */
}IrcutTimeModeInfo,*LP_IrcutTimeModeInfo;

typedef enum
{
	WORKMODE_CUSTOM = 0,//自定义
	WORKMODE_AUTO = 1   //自动
}NIGHTVISIONWORKMODE_T;

typedef enum
{
	CUSTOMMODE_TIME = 0,//定时
	CUSTOMMODE_THR = 1   //手动阈值
}NIGHTVISIONCUSTOMMODE_T;

typedef struct tagIrcutColorNightInfo
{
	unsigned char nStartHour;		/*!< 自定义开始小时,范围[0,23] */
	unsigned char nStartMin;		/*!< 自定义开始分钟,范围[0,59] */
	unsigned char nStartSec;		/*!< 自定义开始秒,  范围[0,59] */

	unsigned char nStopHour;		/*!< 自定义结束小时,范围[0,23] */
	unsigned char nStopMin;		    /*!< 自定义结束分钟,范围[0,59] */
	unsigned char nStopSec;		    /*!< 自定义结束秒,  范围[0,59] */
}IrcutNightVisionTimeInfo,*LP_IrcutNightVisionTimeInfo;	

typedef enum
{
	KEYCLICK_START, //开始
	KEYCLICK_LIGHTMODE, //切换灯板模式
	KEYCLICK_RESUME, //恢复出厂
	KEYCLICK_END //结束
}KEYCLICK_T;

typedef struct tagIPC_TONE_RGB2YUV
{
	int matrix_values[9];		// 范围[-1024-1023]
	int y_offset;				// 范围［-128-255］
	int u_offset;				// 范围［-128-255］
	int v_offset;				// 范围［-128-255］
}IPC_TONE_RGB2YUV,*LP_IPC_TONE_RGB2YUV;

typedef int	(*STREAM_READ_CALLBACK )(unsigned long ChannelNumber);
typedef int	(*TALKING_READ_CALLBACK )(unsigned long HINumber);
typedef int (*SERIALDATA_READ_CALLBACK)(unsigned char* pSerailData,unsigned int uiLength);
typedef int (*SERIALDATA_WRITE_CALLBACK)(unsigned char* pSerailData,unsigned int uiLength);
typedef int (*KEY_STATUS_CALLBACK)(KEYCLICK_T status);
typedef int (*AUDIOALARM_ACTION_CALLBACK)(int action);
typedef int (*QR_SCAN_CALLBACK)(unsigned char *pDataBuf, unsigned int uiDataLen);
typedef int (*HumanTracker_CALLBACK)(char *sBuff, int nLen);
typedef int (*Moto_SavePos_CALLBACK)();

#define MAX_HUMAN_SHAPE_SENSTIVE_NUM 6
#define MAX_HUMAN_SHAPE_REGION_NUM 1
#define MAX_HUMAN_SHAPE_REGION_WIDTH 704
#define MAX_HUMAN_SHAPE_REGION_HEIGHT 576

//!布防时间表
#define SEH_MAX_DAYS 7
#define SEH_MAX_SEG_DAYS 48
typedef struct tAiSchedule
{
	//!0为布防 1为不布防
	unsigned char dwTimeSeg[SEH_MAX_DAYS][SEH_MAX_SEG_DAYS];

	//!状态,0为禁用,1为启用,2为不支持
	unsigned char state;
}AiScehdule, *LpAiScehdule;

//人形侦测配置
typedef struct tagHumanShapeCfg
{
	int nSenstive;          //人形侦测灵敏度, 0 - (MAX_HUMAN_SHAPE_SENSTIVE_NUM-1),越低越灵敏
	int nDetectRegionNum;   //人形区域个数
	RECTH rcDetectRegion[MAX_HUMAN_SHAPE_REGION_NUM];   //人形检测区域(范围:[MAX_HUMAN_SHAPE_REGION_WIDTH, MAX_HUMAN_SHAPE_REGION_HEIGHT])
} HumanShapeCfg, *LpHumanShapeCfg;

typedef struct tagHumanShapeCfgV2
{
	int nSenstive;          //人形侦测灵敏度, 0 - (MAX_HUMAN_SHAPE_SENSTIVE_NUM-1),越低越灵敏
	int nDetectRegionNum;   //人形区域个数
	POLYGON rcDetectRegion[MAX_HUMAN_SHAPE_REGION_NUM];   //人形检测区域(范围:[MAX_HUMAN_SHAPE_REGION_WIDTH, MAX_HUMAN_SHAPE_REGION_HEIGHT])
} HumanShapeCfgV2, *LpHumanShapeCfgV2;

//声音侦测
#define MAX_AUDIO_DETECT_SENSTIVE_NUM 3

//声音侦测配置
typedef struct tagAudioDetectCfg
{
    int nEnable;    //使能
	int nSenstive;  //灵敏度（0~MAX_AUDIO_DETECT_SENSTIVE_NUM），值越小，灵敏度越高
} AudioDetectCfg, *LpAudioDetectCfg;

//声音侦测报警回调参数
typedef struct tagAudioDetectCbData
{
    //是否触发了报警
	char bTrigger;
} AudioDetectCbData, *LpAudioDetectCbData;

//声音侦测报警回调
typedef int (*AudioDetect_CALLBACK)(LpAudioDetectCbData pData, void *UserData);

//设备外设类型
typedef enum enPeripheralDevType
{
    PDEV_WHITE_LED = 0,		//白光灯
    PDEV_INFRARED_LED,		//红外灯
    PDEV_RB_WARN_LED,		//红蓝爆闪灯
} PeripheralDevType;

//设备外设控制配置
typedef struct tagAssistantCtrlInfo
{
	//外设类型
	PeripheralDevType enAssisstantType;
	//控制动作类型 1-开启	0-关闭
	int nCtrlStatus;
} AssistantCtrlInfo;

/**
 * SDK 需要提前设置的一些参数。由外部给定。
 * 如果不给定，则使用固定的默认值。
 * 这里传递的值，只应该在SDK初始化的时候用到
 */
typedef struct
{
    int bLangFlag;              // 语音类型是否有效
    eSpeechLanguage eLang;      // SDK语音语言类型
	int nAlgoType;				// 算法类型 1:人脸算法, 2:结构化算法
	char szReverse[256];		// 保留字段
} ZWSDK_PresetConfig;

typedef struct DEVWIFIINFO
{
    unsigned char Support;                    /* 是否支持wifi功能 */
    signed char Model[32];                    /* WIFI型号 */
    unsigned int Freq;                        /* HALMOD_WIFI_FREQ_E */
    unsigned int Gen;                         /* HALMOD_WIFI_GEN_E */
}DEV_WIFI_INFO_S, *PDEV_WIFI_INFO_S;

//客流统计事件帧
typedef struct tagPassengerFlowEventPacketHeader
{
    unsigned int        uDataPacketStartCode; //0xB7010000
    unsigned char       byFrameType;
    unsigned long       u32RealTimeSec;
    unsigned short      inFlow;                   /*!< 有多少组数据 				*/
    unsigned short      outFlow;                   /*!< 有多少组数据 				*/
}__attribute__((packed)) PassengerFlowEventPacketHeader, *PPassengerFlowEventPacketHeader;
#define	PASSENGER_FLOW_EVENT_PACKET_LEN	sizeof(PassengerFlowEventPacketHeader)

#define STREAM_TYPE_VIDEO   1 // 单独的视频流
#define STREAM_TYPE_AUDIO   2
#define STREAM_TYPE_AVSYNC  3 // 混合流(视频流+音频流)

#define BITRATE_VBR     0
#define BITRATE_CBR     1
#define BITRATE_HBR     2

#define SDK_SUCESS                              0x00000000               //成功 
#define ERR_SDK_NO                        	 	0x00000000	//无错误
#define ERR_SDK_MALLOC_ERROR                	0xd0000000	//内存分配失败
#define ERR_SDK_INVALID_HANDLE              	0xd0000001	//句柄错误
#define ERR_SDK_WAIT_TIMEOUT              	 	0xd0000002	//等待超时
#define ERR_SDK_INVALID_ARGUMENT          		0xd0000003	//参数错误
#define ERR_SDK_NOT_SUPPORT                 	0xd0000004	//不支持功能
#define ERR_SDK_INIT_FAIL                    	0xd0000005	//SDK初始化失败
#define ERR_SDK_FILE_READ_ERROR             	0xd0000006	//文件读取失败
#define ERR_SDK_FILE_WRITE_ERROR				0xd0000007
#define ERR_SDK_BUFFER_LEN_SHORT                0xd0000008	//内存大小不够
#define ERR_SDK_PACKET_NO_DATA                  0xd0000009	//包里没有数据
#define ERR_SDK_SYS_BUSY                        0xd000000A	//系统忙 
#define ERR_SDK_ENCODE_NOTSTART                 0xd000000B

#define ERR_SDK_CLEAR_OSD_ERROR                 0xd0000021
#define ERR_SDK_START_MD_ERROR                  0xd0000022
#define ERR_SDK_STOP_MD_ERROR                   0xd0000022
#define ERR_SDK_CLEAR_MD_ERROR                  0xd0000022

#define ERR_SDK_SETUP_MD_ERROR                  0xd0000023
#define ERR_SDK_SETUP_CVBS_STANDAR_ERROR        0xd0000024
#define ERR_SDK_SETUP_STILL_CAPTURE_ERROR       0xd0000025
#define ERR_SDK_FORCE_IFRAME_ERROR              0xd0000026
#define ERR_SDK_SETUP_MOSAIC_ERROR              0xd0000027
#define ERR_SDK_CLEAR_MOSAIC_ERROR              0xd0000027
#define ERR_SDK_STOP_MOSAIC_ERROR               0xd0000027

/* ROI错误码 */ 
#define ERR_SDK_SETUP_ROI_ERROR             	0xd0000027
#define ERR_SDK_CLEAR_ROI_ERROR         	    0xd0000027

/* Smart AVC错误码 */
#define ERR_SDK_SETUP_LBR_ERROR 				0xd0000027
#define ERR_SDK_STOP_LBR_ERROR 				    0xd0000027


#define ERR_SDK_ENABLE_WATCHDOG_ERROR           0xd0000027
#define ERR_SDK_CLEAR_WATCHDOG_ERROR            0xd0000027

#define ERR_SDK_NOINIT                          0xFFFFFFFF	//SDK没有初始化 

//还需要细分的话在书写函数时添加
#define ERROR_MAX_THRDS_REACHED                 0x000000A4

#ifdef __cplusplus
extern "C" {
#endif
//系统信息函数
int     ZWSys_Init(int mode);
int     ZWSys_DeInit( );
int     ZWSys_SwitchToUpgradeMode(int bInto, unsigned int *pu32PhyAddr, void **ppVirtAddr, unsigned int *puMmzAllocSize);
int     ZWSys_GetBoardInfo(int *pBoardInfo );
int		ZWSys_GetStreamCount(void *hChannelHandle, int *stream);
int     ZWSys_GetTUTKUID(char *puid);
int     ZWSys_GetSerialNum(unsigned char * pSerialNum);
int     ZWSys_GetResetPinStatus(int *pResetStatus);
int     ZWSys_StartSysStatusLed(void *hChannelHandle, int freq);
int     ZWSys_Set485WriteOrRead(int rw);

int     ZWSys_GetTotalEncChannels( );
//与编码相关的函数  
void*   ZWSys_EncChannelOpen(int ChannelNum);
int     ZWSys_EncChannelClose(void *hChannelHandle);
int     ZWSys_EncSetCurrentTime( );//don't know values
int     ZWSys_EncSetCurrentTimeEx(int tNewTime);

int     ZWSys_EncRegisterStreamReadCallback(STREAM_READ_CALLBACK StreamReadCallback);
int     ZWSys_EncStartVideoCapture(void *hChannelHandle);
int     ZWSys_EncStopVideoCapture(void *hChannelHandle);
int 	ZWSys_EncReadStreamData(void *hChannelHandle, char **pDataBuf, int *Length, 
	    int *FrameType, unsigned int *RealTime_Sec, unsigned int *RealTime_USec, int *nSensorChn);
int 	ZWSys_EncForceIFrame(void *hChannelHandle, int nSensorIndex);
int     ZWSys_EncSetStreamType(void *hChannelHandle, unsigned short Type);
int     ZWSys_EncGetStreamType(void *hChannelHandle, unsigned short *StreamType);
int     ZWSys_EncSetCodecPara(void *hChannelHandle, int bAux, CODEC_PARA_S *pCodecPara, EncodeType_t encType);
int     ZWSys_EncGetCodecPara(void *hChannelHandle, int bAux, CODEC_PARA_S * pCodecPara);
int 	ZWSys_EncSetCodecParaExt(void *hChannelHandle, int stream, CODEC_PARA_S_EXT *pCodecPara);

int     ZWSys_EncStartAuxCodec(void *hChannelHandle);
int     ZWSys_EncStopAuxCodec(void *hChannelHandle);
int     ZWSys_EncSetOsdText(void *hChannelHandle, TextOSDInfo_t *pOSDTextInfo);
int		ZWSys_EncSetOsdTextEx(void *hChannelHandle, TextOSDInfoEx_t *pOSDTextInfoEx);
int		ZWSys_EncSetOsdV2Text(void *hChannelHandle, TextOSDV2Info_t *pOSDTextV2Info, int index);
int     ZWSys_EncSetOsdTime(void *hChannelHandle, TimeOSDInfo_t *pOSDTimeInfo);
int 	ZWSys_EncSetOsd(void *hChannelHandle, int EnableText1, int EnableTime, int EnableText);

int 	ZWSys_EncSetupMotionDetection( void *hChannelHandle, MotionDetectGridToi_t *pMotionDetectToiInfo);
int		ZWSys_EncSetMotionSchedule(void *hChannelHandle, LpAiScehdule Schedule);
int     ZWSys_EncStartMotionDetection(void *hChannelHandle);
int     ZWSys_EncStopMotionDetection(void *hChannelHandle);

//人形检测使能开关
int 	ZWSys_EncSetPedestrianFiltering(void *hChannelHandle, int bEnable);

//人形跟踪使能开关
int 	ZWSys_EncSetHumanTrackEn(void *hChannelHandle, int bEnable);

//人形检测配置
int 	ZWSys_EncSetHumanShapeCfgV2(void *hChannelHandle, LpHumanShapeCfgV2 pCfg);
int		ZWSys_EncSetHumanShapeSchedule(void *hChannelHandle, LpAiScehdule Schedule);
int 	ZWSys_EncSetDetectSchedule(void *hChannelHandle, int enable);

//人形叠加信息处理
int 	ZWSys_SetHumanOverlayInfo(void *hChannelHandle, char nMainStreamHumanOverlay, char nSubStreamHumanOverlay);

//是否支持人形推图
int 	ZWSys_SetIsSupportPushPeopleImage(void *hChannelHandle, char bPushPeopleImage);

/*设置日夜自动(被动模式)的IRCUT切换时间*/
int		ZWSys_EncSetDayNightSwitchTime(void *hChannelHandle,  int time);
/*设置日夜自动(被动模式)的灵敏度*/
int		ZWSys_EncSetIrCut(void *hChannelHandle,  int nDayNightSensity);

/*设置日夜自动(主动模式)的IRCUT切换时间*/
int		ZWSys_EncSetDayNightInitiativeSwitchTime(void *hChannelHandle,  int time);
/*设置日夜自动(主动模式)的灵敏度*/
int		ZWSys_EncSetIrcutInitiativeModeSensity(void *hChannelHandle,  int Sensity);
/* 获取ircut 自检状态 */
int ZWSys_GetIrcutCheckStatus(int * ircutCheckStatus);

int 	ZWSys_SetAudioInVol(void *hChannelHandle, int vol);
int 	ZWSys_SetAudioOutVol(void *hChannelHandle, int vol, int IsUpdateVoiceCache);
int 	ZWSys_SetAudioMappingVol(void *hChannelHandle, int vol, int Type);

// 对讲模式： 全双工（0） / 半双工（1）
int ZWSys_EncGetIntercomMode(int *iIntercomMode);
int ZWSys_EncSetIntercomMode(int iIntercomMode);

//获取I帧间隔
int ZWSys_GetRealKeyFrameInterval(int nChannel, int *nFrameRateInterval,int *nKeyFrameInterval);

//未使用
int     ZWSys_EncAdjustMotionDetectPrecision(void *hChannelHandle, int iGrade, int iMotionDetectFps);
int     ZWSys_EncMotionAnalyzer(void *hChannelHandle, char *MotionData, int *iResult);

int 	ZWSys_AlarmOutSupport(int nSupport);
int 	ZWSys_AlarmOutDefaultStatus(int nStatus);
int     ZWSys_AlarmOut (long nPort, int bOpen);
int 	ZWSys_AlarmIn(int *pAlarmIndata );
int 	ZWSys_LightAlarm(int nMode, int bOpen);

int 	ZWSys_GetAudioEncodeMode(AudioEncodeMode_t *audioEncodeMode);
int 	ZWSys_SetAudioChannel(AudioChannelType_t channel);
int 	ZWSys_SetAudioCodec(AudioEncodeMode_t ACodec);
int     ZWSys_SetTalkingMode(int Mode);
int     ZWSys_SetTalkingEnable(int bEnable);
int     ZWSys_ReadTalkingData(char *DataBuf, unsigned int* BufSize);
int     ZWSys_SetTalkingData(char *DataBuf, unsigned int DataSize);
int     ZWSys_RegisterTalkingReadCallback(TALKING_READ_CALLBACK TalkingReadCallback);
int     ZWSys_SetSerialPortDirection(int iTransmit);
int		ZWSys_SetSerialCfg(PTZParam_t *pParam);
int     ZWSys_SetSerialDataCallBack(SERIALDATA_READ_CALLBACK SerailDataReadCallBack);
#if defined (__IS_SOPPORT_ZOOM_OSD__)
int     ZWSys_SetSerialDataWriteCallBack(SERIALDATA_WRITE_CALLBACK SerailDataWriteCallBack);
#endif
int     ZWSys_GetSensorType(BoardSensorType_t *SensorType); 
int     ZWSys_GetCVBSOutStandard(VideoOutStandard_t *VideoStandard);
int     ZWSys_SetCVBSOutStandard(void *hChannelHandle, VideoOutStandard_t VideoStandard);
int 	ZWSys_stillCapture(void *hChannelHandle, char *jpeg_filename, int jpeg_w, 
	int jpeg_h, int quality);

//与sensor设置相关的函数待定
int 	ZWSys_EncSetVideoPara(void *hChannelHandle, int Brightness, 
		int Contrast, int Saturation, int Hue, int Sharpness);
int 	ZWSys_EncSetAutoChangeBright(void *hChannelHandle,  int enable);
int		ZWSys_EncSetBacklightMode(void *hChannelHandle,  int enable);
int   	ZWSys_SetWhiteBalance(void *hChannelHandle, WHITEBALANCE_T *WhiteBalance);
int 	ZWSys_SetAEParam(void *hChannelHandle, AE_T *aeParam);
#if defined (__IR_CUT_AUTO_MANUAL__)
int     ZWSys_EncSetColorNightMode(void *hChannelHandle, NIGHTVISIONWORKMODE_T mode, LP_IrcutNightVisionTimeInfo pColorNightModeInfo);
int     ZWSys_EncSetVarWhiteMode(void *hChannelHandle, NIGHTVISIONWORKMODE_T mode, NIGHTVISIONCUSTOMMODE_T manualMode, int bright);
int     ZWSys_EncSetVarWhiteModeInfo(void *hChannelHandle, LP_IrcutNightVisionTimeInfo pVarWhiteModeInfo, int nightThr, int dayThr);
#endif
int		ZWSys_EncSetDayNightMode(void *hChannelHandle, DAYNIGHTMODE_T mode, LP_IrcutTimeModeInfo pIrcutTimeModeInfo);
int 	ZWSys_EncSetIrcutDirection(void *hChannelHandle,  int mode);

#ifdef __AGINGTEST__
int 	ZWSys_SetForceNightStatus(void *hChannelHandle, int enable);
#endif

/*** 
 * @description: 获取当前得日夜状态
 * @param {void} *hChannelHandle 通道句柄
 * @param {int} isDay 白天1，夜晚0
 * @return {int} 0:成功 <0:失败
 */
int     ZWSys_EncSetNightLedMode(NightLedCommand_T NightLedFlag);
int     ZWSys_EncSetNightLedDuration(int LedDuration);
int		ZWsys_EncGetDayNightStatus(void *hChannelHandle, int *isDay);
int     ZWSys_SetColorNightCfg(void *hChannelHandle, int lastTime);
int 	ZWSys_SetImageMode(void *hChannelHandle, int mode);
/*图像中去雾设置*/
int     ZWSys_SetDefogStrength(void *hChannelHandle, int strength);
/*抗闪强度 strength 0-关  50-弱        100-强*/
int     ZWSys_SetAntiFlickerStrength(void *hChannelHandle, int strength);
int     ZWSys_SetInfraredThreshold(void *hChannelHandle, int threshold);
int		ZWSys_SetInfraredWorkMode(void *hChannelHandle, int mode);
int 	ZWSys_SetInfraredBrightness(void *hChannelHandle, int Brightness);
int 	ZWSys_SetWhiteLightThreshold(void *hChannelHandle, int threshold);
int 	ZWSys_SetNormalWhiteLightThreshold(void *hChannelHandle, int C2Blkthreshold, int Blk2Cthreshold);

#if defined (__DOUBLE_LIGHT_NO_LDR_CUSTOM__)
int 	ZWSys_SetColorNightThreshold(void *hChannelHandle, int threshold);
int 	ZWSys_SetIntelligentThreshold(void *hChannelHandle, int threshold);
#endif
//宽动态
int 	ZWSys_SetLocalExpMode(void *hChannelHandle, int ExpMode);

//3D降噪
int 	ZWSys_SetMCTFStrength(void *hChannelHandle, int MCTFStrength);

int ZWSys_SetLensOrientation(unsigned int nLensOrientation);

//旋转
int 	ZWSys_SetFlipRotateMode(void *hChannelHandle, FLIP_ROTATE_MODE_E flipRotate);
//pc/tv模式
int 	ZWSys_SetChromaScale(void *hChannelHandle, CHROMA_SCALE_E chromaScale);
int     ZWSys_SetColorType(void *hChannelHandle, color_type_E type);
int 	ZWSys_SetDcIris(void *hChannelHandle, int enable);
int 	ZWSys_SetDcIrisDuty(void *hChannelHandle, int duty);

//image test
int ZWSys_SetTuneToneCurve(void *hChannelHandle, int isDefault, const char *toneCurveFile);
int ZWSys_SetRGB2YUVMatrix(void *hChannelHandle, int isDefault, LP_IPC_TONE_RGB2YUV dataArray);
int ZWSys_GetRGB2YUVMatrix(void *hChannelHandle, LP_IPC_TONE_RGB2YUV dataArray);
int ZWSys_GetWifiExist(int *exist);
int ZWSys_EnableDspMemToUpgrade(void *hChannelHandle, int *dspAddr, int *length, int bEnable);
int ZWSys_EncSetSnapPara(void *hChannelHandle, int nFormat, int nQuality);
int ZWSys_EncSetSnapParaV2(void *hChannelHandle, int nQuality, char nStreamType);

//二维码扫描功能使能
int ZWSys_EnableQrScan(int bEnable);
int ZWSys_RegisterQrScanCallback(QR_SCAN_CALLBACK pCallback);
int ZWSys_SetCurrNetCardType(XRS_NETCARD_TYPE_E status);

int ZWSys_RegisterMotoSavePosCallback(Moto_SavePos_CALLBACK pCallback);

/* 设置功能激活码 */
int ZWSys_SetActiveStatusCode(int nActiveStatusCode);

/* 设置授权信息，成功返回授权错误码，失败返回-1 */
int ZWSys_SetAuthInfo(const unsigned char *pszAuthLicense);

/* 获取授权码和授权状态，成功返回0，失败返回-1 */
int ZWSys_GetAuthInfo(unsigned char *pszAuthLicense, unsigned int *pAuthState);

/* 移除(取消)授权，直接返回REMOVE_AUTH_SUCCESS */
int ZWSys_RemoveAuthLicense(void);

int ZWSys_SetSpeechContent(SpeechType_T speech);

int ZWSys_GetPresetConfig(ZWSDK_PresetConfig *pPresetConfig);

int ZWSys_SetPresetConfig (ZWSDK_PresetConfig PresetConfig); /* 模块初始化前调用 */

int ZWSys_SetAlarmLightStayTime(int nEnable, int nStayTime);

int ZWSys_SetAlarmLightStart();

int ZWSys_SetAlarmSpeechCfg(void *hChannelHandle, SpeechAlarmInfo_t *speechCfg);

int ZWSys_SetRemindSpeechCfg(void *hChannelHandle, int TypeIdx, SpeechRemindInfo_t *speechCfg);

int ZWSys_SetRemindHangupState(int iState);

/* 设置自定义语音 */
int ZWSys_SetAudioCustomVoice(void *hChannelHandle, int customtype, char *pBuff, int len);

/* 获取码流统计信息数据 */
int ZWSys_GetBitrateInfo(void *hChannelHandle, int dataArr[], int arrlen);

/* 设置按键状态获取回调函数 */
int ZWSys_RegisterKeyStautsCallBack(KEY_STATUS_CALLBACK getKeyStateCallback);

int ZWSys_GetActiveStatusCode(int *pbActiveStatus);
int ZWSys_SetLinkStatus(int status);
int ZWSys_SetWifiStatusLed(WIFI_STATUS_T wifiStatus);

//-----------------------------------------------------------------------------
//	说明：		设备外设控制
//	输入参数：AssistantCtrlInfo	pCtrlInfo	控制信息
//	返回:		成功为 0 , 其它为定义的错误号；
//-----------------------------------------------------------------------------
int ZWSys_AssistantControl(AssistantCtrlInfo *pCtrlInfo);

int ZWSys_GetUniqueID(char *uniqueid, int len);

int ZWSys_SetCalibrationStatue(int nStatue);
/*更新PTZ状态*/
int	ZWSys_UpdatePtzStatus(void *hChannelHandle, ePtzStatus enStatus);
/*跟踪是否回到了看守位*/
int	ZWSys_PtzForTrackBacktoPreset(void *hChannelHandle, ePtz2PresetStatus toPresetStat);
/**
 * @description: 初始化KS HalMod模块
 * @return {int} 正常返回0，异常返回<0
 */
int ZWSys_HalModInit();

/**
 * @description: 反初始化KS HalMod
 * @return {int} 正常返回0，异常返回<0
 */
int ZWSys_HalModDeInit();

/**
 * @description: 设置系统启动状态标志
 * @param {int} Stat 启动状态标记
 * @return {int} 正常返回0，异常返回<0
 */
int ZWSys_SetSysStartTag(int Stat);

int ZWSys_SetSpeechLanType(eSpeechLanguage speech_language);

int ZWSys_LightBoardType(int nLightType);

int ZWSys_GetAdcStatus(int *nAdcStatus);

int ZWSys_GetSpeechPlayingStatus(void);

/**
 * @brief 设置休眠状态
 * 
 * @param iState: 0表示唤醒，1表示休眠
 * @return int 返回 0
 */
int ZWSys_SetDormancyState(int iState);

int ZWSys_EncSetSlowFrameMode( int iSlowFrameMode);

//设置灯板高低开模式
int ZWSys_SetLightOpenStatus(int Status);

//设置工厂模式 1-代表工厂模式 0-代表出厂模式
int ZWSys_SetProductMode(int nProductMode);

//设置声音侦测配置
int ZWSys_SetAudioDetectCfg(void *hChannelHandle, LpAudioDetectCfg pCfg);
//注册声音侦测报警回调
int ZWSys_RegisterAudioDetectCallback(AudioDetect_CALLBACK pCallback, void* pCBUserData);

int ZWSys_SwitchAecMode(int AecMode);
int ZWSys_SetRecoveryAudioAlarm();

int ZWSys_SysInitSensorCnt();

int ZWSys_GetMaxSensorCnt();

int ZWSys_GetSensorConfCorrect(int CheckCnt);

int ZWSys_HumanTrackerCallBack(HumanTracker_CALLBACK pFn_HumanTracker);

int ZWSys_GetWiFiInfo(DEV_WIFI_INFO_S *wifiInfo);

int ZWSys_P2PNotifySendDataFail(const char * pBuffer, unsigned int dwBufLen);

#ifdef __MODULE_4G_SUPPORT__
/* 设置4G模组的类型 */
int ZWSys_SetModuleType(e4GModuleType moduletype);

/* 设置4G模组云卡的类型 */
int ZWSys_SetCloudCardmode(int CloudCardmode);

//设置4G运营商的类型
int ZWSys_Set4GTelecomOperatorType(e4GTelecomOperatorType telecomOperatorType);

//复位4G模组
int ZWSys_4GModuleReset();

//判断当前是否是运营号段 返回值 1：是运营号段  0：非运营号段 -1:异常
int ZWSys_CheckDidIsFlowOpreate(char *pDid);

//判断当前是否支持的号段 返回值 1：支持  0：不支持 -1:异常
int ZWSys_CheckDidIsSupport(char *pDid);

//根据号段获取客户类型 nCustomer: 客户类型
int ZWSys_DidForCustomerType(char *pDid,int *nCustomer);

// 设置了IOTDBB 号段的DID，未激活运营模式，显示 “未激活”（GB2312）
int ZWSys_SetNoActiveOperModeOSDDisplay(void);

//当前网卡是否是4G
int ZWSys_Get4GSupport(int *bSupport);

//当前网卡是否是4G,不管是否识别到wifi
int ZWSys_Get4GSupportV2(int *bSupport);

//获取是否支持绑定后播报语音
int ZWSys_GetBroadCastEnable(int *nEnable);

int ZWSys_SetModuleType(e4GModuleType moduletype);

int ZWSys_GetModuleType(e4GModuleType *moduletype);
#endif

#ifdef __ONE_CLICK_CALL__
int ZWSys_SetPauseAudioAlarm();

int ZWSys_SetKey2callhanupState(int iState);

int ZWSys_GetKey2callDuSpeechStatus(int *status);

int ZWSys_GetCallPinStatus(int *pCallStatus, int *pHangupStatus);

int ZWSys_GetCallPinStatus_Test(int *pCallStatus, int *pHangupStatus);

int ZWSys_SetCallPinStatus_Test(int pCallStatus, int pHangupStatus);

int ZWSys_SetAlternateResetButton(int enable);

int ZWSys_SetKeyToCall(int enable);

int ZWSys_SetKeyToCallHardwareType(int type);
#endif

#if defined(__DIGITAL_ZOOM_DRAG_SUPPORT__)
//设置拖动变倍倍数
int ZWSys_SetDragZoomRatio(double ratio,int eOpreator);

int ZWSys_GetDragZoomRatio(double *ratio);

int ZWSys_GetEachFocusZoom(double *shortZoom, double *middleZoom, double *longZoom);

int ZWSys_SetDragZoomCmd(DRAG_ZOOM_CMD nCmd,int eOpreator);

int ZWSys_GetDragZoomChgStatus(int *status);

int ZWSys_SetDragZoomScaleMagnification(double ratio);

int ZWSys_GetDragZoomRange(double *min,double *max);

//获取实际最大倍数
int ZWSys_GetRealMaxRatio(double *ratio);

int ZWSys_GetMotorPos(int *hpos,int *vpos);

//获取送检时真实倍数
int ZWSys_GetCheckRealRatio(double *curRatio);

double ZWSys_GetCurRatio();

#if defined __IS_SUPPORT_LONG_SHORT_FOCUS_ZOOM__
int ZWSys_SetZoomLens(int nLens);
int ZWSys_GetCurrentSensor();

#endif
#endif

#if defined(__IS_SUPPORT_LCD_MANAGER__)
int ZWSys_LCDRefreshFrame(char *pData, unsigned int DataSize);

int ZWSys_LCDSetBackLightLevel(int level);

int ZWSys_LCDCreateVdecChn(int nType);

int ZWSys_LCDSendStreamToChn(int nType, char *pData, unsigned int DataSize);

int ZWSys_LCDDestoryVdecChn(int nType);
#endif

int ZWSys_DownSdPower();

int ZWSys_SetSptSwapLRPTZChannel(int bEnable);

int ZWSys_GetSptSwapLRPTZChannel(int *pbEnable);

#ifdef __cplusplus
}
#endif
#endif

