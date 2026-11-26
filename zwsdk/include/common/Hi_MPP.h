#ifndef _HI_MPP_H_
#define _HI_MPP_H_

#include <sys/mman.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <math.h>
#include <unistd.h>
#include <regex.h>

#include "Common.h"
#include "defs.h"

/* 平台相关头文件 */
#include "vcomm.h"
#include "vi.h"
#include "vpss.h"
#include "visp.h"
#include "msstd.h"
#include "lightctrl.h"
#include "ao.h"
#include "ai.h"
#include "aenc.h"
#include "adec.h"
#include "venc.h"
#include "vosd.h"
#include "vda.h"
#include "RingBuffer.h"

#define AUDIO_PERIOD_SIZE 320
#define AUDIO_CHANNEL	2

#define AVBR_MOTION_STEP 4
#define MAIN_SUPER_IFRM_BITS_THR    (200*1024*8)
#define AUX_SUPER_IFRM_BITS_THR     (50*1024*8)

#define FORCE_RC_TO_CBR_HOLD_TIME 0	//5
#define MAX_STATISTIC_TIMES		(10)
#define _VIDEO_TEST_MAX_CHN 1
#define ISP_EFFECT_FILE_PATH		"/mnt/app/data/sensor/usr/"
#define ALGO_JSON_PATH 			"/mnt/app/bin/algo_config.json"
#define _VIDEO_TEST_DATA_NAME_STR   "myvideo_test.h264"
#define _VIDEO_TEST_DATA_NAME_STR_SUB   "myvideo_test_sub.h264"

#define _VIDEO_TEST_CROP_RATIO_00_PERCENT  0
#define _VIDEO_TEST_CROP_RATIO_10_PERCENT  20


#define IVE_MD_IMAGE_NUM 2

#define MAX_RGN_COVER_W 8192
#define MAX_RGN_COVER_H 8192

#define WAV_G711A (0x06)
#define WAV_G711U (0x07)
#define WAV_G726 (0x45)
#define WAV_PCM (0x1)

#define G726_16 (2)
#define G726_24 (3)
#define G726_32 (4)
#define G726_40 (5)

#define AI_DMIC_CHN_MAX (2)

#define AI_AMIC_CHN_MAX (2)
#define AI_I2S_NOR_CHN (2)

#define AI_FRM_MIN_INTERVAL  (10)
#define AI_FRM_MS_INTERVAL (3600)

#define AI_DEV_ID_MAX (3)
#define AO_DEV_ID_MAX (2)

#define AI_DEV_AMIC (0)
#define AI_DEV_DMIC (1)
#define AI_DEV_I2S_RX (2)
#define AI_DEV_LineIn (3)

#define AO_DEV_LineOut (0)
#define AO_DEV_I2S_TX (1)

#define AI_VOLUME_AMIC_MIN (0)
#define AI_VOLUME_AMIC_MAX (3)
#define AI_VOLUME_LINEIN_MAX (31)

#define AI_VOLUME_DMIC_MIN (-60)
#define AI_VOLUME_DMIC_MAX (30)

#define AO_VOLUME_MIN (0)
#define AO_VOLUME_MAX (165)      /*调整音频输出 DAC 音量*/

#define MI_AUDIO_SAMPLE_PER_FRAME (1024)

#define DMA_BUF_SIZE_8K (8000)
#define DMA_BUF_SIZE_16K (16000)
#define DMA_BUF_SIZE_32K (32000)
#define DMA_BUF_SIZE_48K (48000)

#define AI_DMA_BUFFER_MAX_SIZE (256 * 1024)
#define AI_DMA_BUFFER_MID_SIZE (128 * 1024)
#define AI_DMA_BUFFER_MIN_SIZE (64 * 1024)

#define AO_DMA_BUFFER_MAX_SIZE (256 * 1024)
#define AO_DMA_BUFFER_MID_SIZE (128 * 1024)
#define AO_DMA_BUFFER_MIN_SIZE (64 * 1024)

#define MIU_WORD_BYTE_SIZE (8)
#define TOTAL_BUF_DEPTH (16)

#define BASE_FONT_WIDTH (8)
#define BASE_FONT_HEIGHT (16)

#define MAX_OSD_TARGET_NUM (8)

//##############################人形  start##################################
#define HCFD_FRMRATE 5 //人形检测的帧率
#define USE_IVE 1
#define SAD_BLOCK_SIZE 8
#define MD_ROI_MAX 50
#define MD_PIXEL_DIFF 15   //夜晚推荐5，白天推荐10
#define HCHD_DETECT_MAX 32 //人形检测的最多人形个数
//##############################人形  end##################################
#define MD_CD_CHECK_INTERVAL (100) /*25毫秒检测一次,FHAdv_MD_CD_Check函数同时检测MD/MD_EX/CD, \
									 如果用户对于MD/MD_EX/CD配置了不同的检测间隔,         \
									 advapi内部会取他们的最小检测间隔*/
#define PASSENGER_FLOW_SEND_EVENT_PERIOD_MS (1000)
#define ELECTRONIC_FENCE_SEND_EVENT_PERIOD_MS (1000)

typedef enum
{
	E_AI_SOUND_MODE_MONO = 0,
	E_AI_SOUND_MODE_STEREO,
	E_AI_SOUND_MODE_QUEUE,
} AiSoundMode_e;

typedef struct HI_MPP_VPSS_VENC_BIND_S
{
	int m_bValid;
	int m_VpssGrp;
	int m_VpssChn;
	int m_VenGrp;
	int m_VenChn;
} HI_MPP_VPSS_VENC_BIND;

typedef struct HI_MPP_RC_FORCE_TO_CBR_S
{
	BOOL m_bBitrateOverLoad;
	BOOL m_bIFrameOverLoad;
	BOOL m_bMotionOverLoad;
} HI_MPP_RC_FORCE_TO_CBR;

typedef struct HI_MPP_RC_INFO_S
{
	int m_bit_rate;
	BOOL m_frame_rate;
	int m_bit_rate_10s;
} HI_MPP_RC_INFO;

/* 人脸检测相关定义 */
#define MAX_FACE_NUM 10
#define FACE_TRACK_DISTANCE_MULTI (1.5 * 1.5)
#define FACE_TRACK_NO_DIE_HOLD_TIMES 2

typedef enum
{
	FACE_CROP_MODE_FACE = 0,
	FACE_CROP_MODE_HBODY,
	FACE_CROP_MODE_BODY,
} FACE_CROP_MODE_E;

typedef struct
{
	int x; /* 区域的x坐标 */
	int y; /* 区域的y坐标 */
	int w; /* 区域的宽 */
	int h; /* 区域的高 */
} FACE_RECTH;

typedef struct
{
	UINT32 u32X;
	UINT32 u32Y;
	UINT32 u32Width;
	UINT32 u32Height;
} Detect_Rect;

struct buf_desc
{
	SINT32 valid; /*buf缓冲区有效标志,1:已有一帧有效数据,可以做人形检测, 0:buf缓冲区无效*/
	//MEM_DESC buf;	 /*从VMM分配的缓冲区,用来存储用于人形检测的一帧原始图像的Y数据*/

	Detect_Rect detect_rect; /*人形检测的区域,可以全屏检测,也可以是基于运动或BGM的一个子区域的检测*/
};

typedef struct
{
	unsigned int id;	  /* 人脸追踪ID */
	FACE_RECTH rect;	  /* 检测算法输出的区域坐标 */
	FACE_RECTH cropRect;  /* 抠图区域的坐标，可能根据抠图方式的不同会有变换和缩放 */
	unsigned int quality; /* 人脸质量 */
	int noDie;			  /* 发送维持帧，保留中间层数据 */
} FACE_INFO;

typedef struct
{
	unsigned int count;
	FACE_INFO faces[MAX_FACE_NUM];
} FACE_DETECT_RESULT;

typedef struct
{
	int used;			   /*该结构是否使用*/
	int id;				   /*人脸跟踪ID*/
	int index;			   /*最新的人脸坐标索引，>=10 说明数组被环形使用 */
	FACE_RECTH rects[10];  /*缓存的人脸坐标*/
	FACE_RECTH prediction; /*预测坐标*/
	int noFaceTimes;	   /*未匹配到人脸的持续帧数*/
	int flag;			   /*当前检测周期中是否已经找到匹配的人脸*/
} FACE_TRACK_INFO;

typedef struct
{
	unsigned int max_id;
	FACE_TRACK_INFO track_info[MAX_FACE_NUM];
} FACE_TRACK;

typedef struct _XRS_UserSetParaInfo
{
	int SetEncParaMode; 					   //0--用户设置编码参数  1--线程调用
	int force_to_cbr;		   				   //各通道强制CBR标记
	int ForceRcModeHoldTimes;				   //各通道强制CBR持续时间，默认60
} XRS_UserSetParaInfo_t;

/**
 * 平台相关的编码结构体采用 _XRS_XX来封装
 * XRS_StreamAttr:分辨率相关
 * XRS_StreamData:流数据的BUFFER相关
 * XRS_VENC_ChnAttr:编码参数相关
 */
typedef struct _XRS_StreamAttr
{
	BOOL bEnable;

	UINT32 u32Width;
	UINT32 u32Height;

	/* OSD */
	BOOL bUseOSD;
	SINT32 s32TextOsdID;	// 文本OSD ID
	SINT32 s32TimeOsdID;	// 时间OSD ID
#ifdef __DIGITAL_ZOOM_DRAG_SUPPORT__
	SINT32 s32ZoomOsdID;	// 变倍OSD ID
#endif

	RECTH TextOSDRect;
	RECTH TimeOSDRect;
#ifdef __DIGITAL_ZOOM_DRAG_SUPPORT__
	RECTH ZoomOSDRect;
#endif
	BOOL m_bTimeOSDCreated;	//时间OSD区域是否创建，因为osd使能跟配置不同步，需要标记底层状态，防止未创建初始化
	BOOL m_bTextOSDCreated;	//文本OSD区域是否创建，因为osd使能跟配置不同步，需要标记底层状态，防止未创建初始化
#ifdef __DIGITAL_ZOOM_DRAG_SUPPORT__
	BOOL m_bZoomOSDCreated; //文本OSD区域是否创建，因为osd使能跟配置不同步，需要标记底层状态，防止未创建初始化
#endif

	unsigned char *pTimeBitmap;      //时间信息实际点阵
	unsigned char *pTextBitmap;		//文本信息实际点阵
#ifdef __DIGITAL_ZOOM_DRAG_SUPPORT__
	unsigned char *pZoomBitmap; 	//变倍信息实际点阵
#endif

} XRS_StreamAttr;

typedef struct _XRS_StreamData
{
	BOOL	bFinish;
	VENC_CHNTYPE_E eStream;
	unsigned int u32VencChn;
	ENCODE_TYPE_E eStreamType;
	ENCODE_FRAME_TYPE_E eFrameType;

	UINT32 u32FrameInterval;	
	UINT32 TimeSec;
    UINT16 TimeMSec; 
	UINT32 SYS_TimeSec;
    UINT16 SYS_TimeMSec; 
	char *data;
	unsigned int datasz;
	int framerate;
	int SecFrmType;
} XRS_StreamData;

typedef struct _XRS_VENC_ChnAttr
{
	VENC_ENCODEPARAM_S EncParam;
} XRS_VENC_ChnAttr;

typedef struct _XRS_VPSS_ChnAttr
{
	VPSS_CHN_ATTR_S VpssParam;
} XRS_VPSS_ChnAttr;

//#########################################################

/**
 * 平台相关的ISP结构体采用 _XRS_XX来封装
 * XRS_ISP_PARAM:ISP相关参数
 */
typedef struct _XRS_ISP_PARAM
{
	ISP_TUNE_AE_PARAM_S stAeParam;
} XRS_ISP_PARAM;

//#########################################################

//##############################人形  start##################################
typedef struct faces_s
{
	int valid; //人形结果是否有更新，产生数据线程更新后置一，画框线程取完后清零
	RECT_S rectInfo[HCHD_DETECT_MAX];
	int num;
	float pScore[HCHD_DETECT_MAX];
	unsigned int trackId[HCHD_DETECT_MAX];
    unsigned char IsMove[HCHD_DETECT_MAX];
	unsigned int nFrameId;
    unsigned int nSnsId;
} human_struct;

typedef struct coexistEventTrack_s
{
	int valid;//是否有效
	int trackNum;//track个数
	unsigned int trackId[HCHD_DETECT_MAX];
} coexistEventTrack_struct;

typedef struct _NNA_Result
{
	int num;
	struct
	{
		int type;
		RECT_S rect;
		float score;
	} rsts[HCHD_DETECT_MAX];
} NNA_Result;

typedef struct _NNA_Rst
{
	int type;
	RECT_S rect;
	float score;
} NNA_Rst;

typedef struct ive_methon
{
	int width;
	int align_height;
	// MI_IVE_HANDLE ive_handle;
	// MI_IVE_SrcImage_t *Y_image0, *Y_image1;
	// MI_IVE_Image_t *SadResult, *ThdResult;
	// MI_IVE_SadCtrl_t *sad_ctrl;
} SAD_METHOD_handle;

typedef struct
{
	int x_min;
	int x_max;
	int y_min;
	int y_max;
} BBOX;

typedef struct
{
	int max_num;
	int used_num;
	BBOX box[MD_ROI_MAX];
} ROI;

typedef struct
{
	unsigned int x;
	unsigned int y;
} HM_POINT;

typedef struct
{
	int nSenstive;
	double dScoreThr; // 置信度阈值
	int nDetectRegionNum;
	POLYGON rcDetectRegion[MAX_HUMAN_SHAPE_REGION_NUM];
} XRS_AICfg;

typedef enum
{
	I2_PF_COLOR_NULL = 0,
	I2_PF_COLOR_RED = 1,
	I2_PF_COLOR_GREEN = 2,
	I2_PF_COLOR_WHITE = 3,
} I2_PIXEL_FORMATE_COLOR_E;

typedef struct _XRS_STATISTICS
{
	unsigned int io1Brihtness;
	unsigned int io2Brihtness;
	unsigned int softLdr;
	unsigned int gain;
	unsigned int luma;
	unsigned int viFps;

	unsigned int inFlow;
	unsigned int outFlow;
	unsigned int offDutyFrequency;
	unsigned int offDutyTotalTimes;
} XRS_STATISTICS;

typedef enum
{
	ELECTRONIC_FENCE = 0,
	CROSS_BOARD_DETECT = 1,
	OFF_DUTY_DETECT = 2,
	PASSENGER_FLOW_STATICS = 3,
} EVENT_TYPE;

//##############################人形  end################################

typedef enum
{
	AI_DETECT_OFF 						= 0,		//无，均为开启任何功能
	AI_DETECT_ELECTRONIC_FENCE_ON 		= 1,		//电子围栏
	AI_DETECT_CROSS_BOARD_DETECT_ON 	= 2,		//越界检测
	AI_DETECT_OFF_DUTY_DETECT_ON 		= 3,		//离岗检测
	AI_DETECT_PASSENGER_FLOW_ON 		= 4,		//客流统计
	AI_DETECT_FIRE_DETECT_ON 			= 5,		//火焰检测
	AI_DETECT_SMART_TRACK_ON			= 6,		//智能跟踪
	AI_DETECT_ALERT_AREA_MAX,						//警戒区域类功能最大值，前面的功能互斥
	AI_DETECT_FACE_SNAPSHOT_ON 			= 10,		//人脸抓拍
	AI_DETECT_MASK_DETECT_ON 			= 11,		//口罩检测
	AI_DETECT_MAX,									//功能最大值
} AI_DETECT_FUNCTION_TYPE;

typedef enum
{
	AREA_TYPE_ALERT_AREA				= 0,		//警戒区域类功能
	AREA_TYPE_FACE						= 1,		//人脸类功能
	AREA_TYPE_MAX,
} AI_DETECT_DETECT_AREA_TYPE;

typedef enum
{
	AI_DETECT_TARGET_HUMAN = 0,
	AI_DETECT_TARGET_CAR = 1,
	AI_DETECT_TARGET_FIRE = 2,
	AI_DETECT_TARGET_FACE = 3,
} AI_DETECT_TARGET_TYPE;

typedef struct
{
	int nValidNum;
	VECTOR point[8];
} AI_DETECT_POLYGON;

//!\brief MPP层管理电子围栏，越界检测，离岗检测，客流统计配置的结构体
typedef struct
{
	//提取出来方便线程里取判断条件，不然需要挨个配置去遍历。但每个功能的配置更新需要同时更新下面三个状态
	AI_DETECT_FUNCTION_TYPE whichFunction; // 0:均为开启任何功能，1：电子围栏，2：越界检测，3：离岗检测，4：客流统计，5：火焰检测
	AI_DETECT_TARGET_TYPE detectTarget[AREA_TYPE_MAX];	   // 0:人形，1：车形 2.:火焰
	float sensitivity;					   //目标的灵敏度
	unsigned int trackEnable;			   //跟踪使能
	AI_DETECT_POLYGON detectArea[AREA_TYPE_MAX];		   //检测区域，用多边形结构体可表示,2个点是拌线，3个点以上是多边形

	// ELECTRONIC_FENCE_CFG		m_stElectronicFenceCfg; 	//电子围栏配置
	// CROSS_BOARD_EDTECT_CFG 	m_stCrossBoardDetectCfg;	//越界检测配置
	// OFF_DUTY_DETECT_CFG		m_stOffDutyDetectCfg;		//离岗检测配置
	// PASSENGER_FLOW_CFG		m_stPassengerFlowCfg;//客流统计配置
} XRS_AIDetectCfg;

typedef enum audio_bitwidth
{
	AUDIO_BITWIDTH_8 = 0,
	AUDIO_BITWIDTH_16,
} XRS_AUDIO_BITWIDTH_E;

typedef enum audio_sample_rate
{
	AUDIO_SAMPLE_RATE_8K = 0,
	AUDIO_SAMPLE_RATE_16K,
} XRS_AUDIO_SAMPLE_RATE_E;

typedef struct audio_data
{
	AudioEncodeMode_t 		eMode;			// 音频编码格式（暂未使用）
	XRS_AUDIO_BITWIDTH_E eBitWidth;			// 位宽（暂未使用）
	XRS_AUDIO_SAMPLE_RATE_E eSampleRate;	// 采样率（暂未使用）
	UINT32 u32DataSz;		// 音频帧数据大小（暂未使用）
	BYTE *pData;			// 音频数据地址，使用后KS释放
} XRS_AUDIO_DATA_S, *PXRS_AUDIO_DATA_S;

typedef SINT32 (*AudioInData_Callback)(PXRS_AUDIO_DATA_S pstData);

typedef struct _DETECTOR_PUSH_STRATEGY_T {
    UINT8 nPushMode;
    UINT32 nInterval; // only for MCV_PUSH_MODE_INTERVAL or MCV_PUSH_MODE_FAST
    UINT32 nPushCounts; // only for MCV_PUSH_MODE_INTERVAL or MCV_PUSH_MODE_FAST
    BOOL bPushSameFrame; // AX_FALSE: push cross frame AX_TRUE: push same frame
} DETECTOR_PUSH_STRATEGY_T;

typedef struct _DETECTOR_STREAM_PARAM_T {
    char algo_type[128];
    char config_path[256];
} DETECTOR_STREAM_PARAM_T;

typedef struct _AI_Detection_Box_t {
    float fX, fY, fW, fH;
} AI_Detection_Box_t;

typedef struct _AI_Detection_BodyResult_t {
    AI_Detection_Box_t tBox;  // 0-1 relative
} AI_Detection_BodyResult_t;

typedef struct _AI_Detection_Result_t {
    SINT32 nErrorCode;
    UINT32 nFrameId;
    AI_Detection_BodyResult_t* pBodys;
    UINT32 nBodySize;
} AI_Detection_Result_t;

typedef struct _AI_Result_Black_Mode_t {
	pthread_mutex_t m_humanResultsBlackModeLock; //人形结果数据锁 权重用
	human_struct m_humanResultsBlackMode;		 //人形结果,修改权重用
	int isStartGetHumanResult; //是否开始获取人形结果
} AI_Result_Black_Mode_t;

typedef struct _AI_Result_Smart_Track_t {
	pthread_mutex_t m_humanResultsSmartTrackLock;   //人形结果数据锁
	human_struct m_humanResultsSmartTrack;		 	//人形结果，智能跟踪用
} AI_Result_Smart_Track_t;

typedef struct mem_desc {
	unsigned int base;
	void        *vbase;
	unsigned int size;
	unsigned int align;
} MEM_DESC;


/* 编码通道句柄信息 */
typedef struct
{
    BOOL   m_bHandInit; /*标识此句柄是否初始化*/
    SINT32 m_BoardInfo;
    HI_MPP_RC_INFO m_MppRCInfo[MAX_STREAM_COUNT];

    SINT32              		m_MaxViChn;  /*当前视频最大输入数*/
    VIDEO_CHIP_CONF_S   		m_ChipConf;  /*芯片信息*/

    /*Encode*/
    BOOL m_bEncoderIsStart;    /*1-开始编码 0-结束编码*/
    int m_VIOutMaxWidth;	   /*当前 VI 通道支持的最大分辨率宽*/
    int m_VIOutMaxHeight;	   /*当前 VI 通道支持的最大分辨率高*/

    int m_VPSSGroupNum[SUPPORT_MAX_SENSOR_COUNT];                   //对应的VPSS组编号
    int m_VPSSChnNum[SUPPORT_MAX_SENSOR_COUNT][MAX_STREAM_COUNT];   //对应的VPSS通道编号
    int m_MppEncChNum[SUPPORT_MAX_SENSOR_COUNT][MAX_STREAM_COUNT];  //编码通道号
    int m_FrameNo[SUPPORT_MAX_SENSOR_COUNT];            //主码流压缩数据帧数
    int m_FrameNoAux[SUPPORT_MAX_SENSOR_COUNT];         //辅码流压缩数据帧数
    int m_FrameNoFourth[SUPPORT_MAX_SENSOR_COUNT];      //四码流压缩数据帧数
    int m_FrameNoAudio;                                 //音频流压缩数据帧数

    XRS_StreamData m_stStreamData;                                                  /* 一帧编码数据缓存 */
    XRS_StreamAttr   m_stStreamAttr[SUPPORT_MAX_SENSOR_COUNT][MAX_STREAM_COUNT];    /* 用户设置的分辨率 */
    XRS_VENC_ChnAttr m_stVencChnAttr[SUPPORT_MAX_SENSOR_COUNT][MAX_STREAM_COUNT];   /* Venc参数 */
    XRS_VPSS_ChnAttr m_stVpssChnAttr[SUPPORT_MAX_SENSOR_COUNT][MAX_STREAM_COUNT];   /* VPSS参数 */

    /*ISP*/
    BOOL m_bISPInitIsOver;      /* ISP初始化是否成功 */

    /*Day Night Mode*/
    int m_DayNightMode;
    int m_ImageMode;

    /*Human*/
    int m_bAiAlgoInited;			                //人形算法是否初始化
    int m_bAiUseingAlgo;			                //当前使用的算法类型
    int m_bHumanEnable;				                //人形的使能
    int m_bHumanTrackEn;                            //人形跟踪使能
    int m_bHumanInited;				                //人形算法是否初始化
    pthread_mutex_t m_humanWorklock;                //人形工作的锁，包括对流的获取，移动判断，算法。为了跟切换编码互斥
    pthread_mutex_t m_humanResultsLock;	            //人形结果数据锁
    XRS_AICfg m_HumanDetectCfg;		                //人形探测算法的配置
    human_struct m_humanResults[ALGO_CHANNEL];		//人形结果,给画框用
    AI_Result_Black_Mode_t m_humanResult2Black;     //人形结果,黑光逻辑用

    /*Audio*/
    int m_AudioInVol;
    int m_AudioOutVol;
    int m_AudioDecMode; // 0:解码G711,1:解码PCM
	int m_FullAudioInVol;//全双工MIC映射值
	int m_FullAudioOutVol;//全双工SPK映射值
	int m_HalfAudioInVol;//半双工MIC映射值
	int m_HalfAudioOutVol;//半双工PSK映射值
	
    /* OSD相关 */
    int TimeBitmapWidth[MAX_STREAM_COUNT];		   //时间信息最终位图宽度(放大后)
    int TimeBitmapHeight[MAX_STREAM_COUNT];		   //时间信息最终位图高度(放大后)

#ifdef __DIGITAL_ZOOM_DRAG_SUPPORT__
		int ZoomBitmapWidth[MAX_STREAM_COUNT];		   //时间信息最终位图宽度(放大后)
		int ZoomBitmapHeight[MAX_STREAM_COUNT]; 	   //时间信息最终位图高度(放大后)
#endif

    XRS_STATISTICS m_stStatistics; //数据统计
    ISP_TUNE_AE_PARAM_S m_AeParam;

    /* 音频流数据缓冲 */
    RingBuffer *m_pstAudioRb;
    /* 对讲数据缓冲 */
    RingBuffer *m_pstTalkAudioRb;
    UINT32 audio_sys_TimeSec;
    UINT16 audio_sys_TimeUSec;

    SINT32 m_AIAudioDataNeadClearSize;  //因调整音频配置后需要清零的AI数据长度；
} CPlatformChannelContext;

typedef enum
{
	FULLIN,
	FULLOUT,
	HALFIN,
	HALFOUT,
} AudioMappingType;


typedef enum sample_vi_mode_e
{
	SMART_SC4236_CMOS_1296P_20FPS,
	SMART_SC4236_CMOS_1080P_30FPS,
	F23_CMOS_1080P_30FPS,
	IMX307_CMOS_1080P_30FPS,
} SAMPLE_VI_MODE_E;

typedef struct AFSoftIrParam_s{
	int m_SensorGain;    /*当前Sensor的增益值*/
	int m_IrCutStatus;   /*IRCUT状态*/
	int m_IrCutCtlMode;  /*IRCUT控制模式*/
	int m_IrLight;       /*红外灯亮度*/
	int m_WhiteLight;    /*白光灯亮度*/
}AFSOFTIRPARAM_T;

typedef enum
{
	H264_ENC = 0,
	H265_ENC,
	MJPEG_ENC,
	H264_CBR,
	H264_VBR,
	H264_AVBR,
	H265_CBR,
	H265_VBR,
	H265_AVBR,
	H265_PLUS_CBR,
	H265_PLUS_VBR,

	GOP_ATTR
} ENC_MODULE;

typedef enum
{
	LIGHT_CLOSE				= 0,
	LIGHT_OPEN_WITH_FACE	= 1,
	LIGHT_OPEN_WITHOUT_FACE = 2,
	BLC_MODE 				= 3,/* 背光补偿 */
	AE_STATUS_MAX,
} AE_STATUS;
#if defined (__PRODUCT_T4P__) || defined (__PRODUCT_T4P_W__)
typedef enum sensor_fps 
{
	__15FPS,
	__20FPS,
	__25FPS,
}SENSOR_INIT_FPS;
#elif defined (__PRODUCT_T4S__) || defined (__PRODUCT_T4C__)
//t4s 暂只用支持一个模式(0=30帧)
typedef enum sensor_fps 
{
	__30FPS,
	__25FPS,
	__20FPS,
	__15FPS,
}SENSOR_INIT_FPS;
#endif

int Hi_MPP_InitVO(void *hChannelHandle);

int Hi_MPP_StopENC(void *hChannelHandle, int stream);

int Hi_MPP_GetRealRunRcInfo(void *hChannelHandle);

int Hi_MPP_ReadStream(void *hChannelHandle,
					  char **pFirstDataBuf,
					  int *pFirstLength,
					  int *pRealTime,
					  int *pRealTime_Usec,
					  int *pFrameType,
					  int encType,
					  int stream,
					  int  SensorIndex);

 //!\brief 获取当前需要跟踪的目标；选取一个最大人形跟踪直到消失，再重新选取下个目标跟踪
 //!\praram in srcResult 算法给的所有目标
 //!\praram out maxResult 需要跟踪的目标
 //!\return 当前最大的人形的面积
int Hi_MPP_GetMaxTargetResult(void *hChannelHandle, human_struct *srcResult, human_struct *maxResult);
 
int Hi_MPP_SetAudioInVol(void *hChannelHandle, int vol, int IsClearCache, int m_IntercomMode);
int Hi_MPP_SetAudioOutVol(void *hChannelHandle, int vol, int IsUpdateVoiceCache, int m_IntercomMode);

int Hi_MPP_SetAudioMappingVol(void *hChannelHandle, int vol, int Type);

int Hi_MPP_ReadAudioStream(void *hChannelHandle, StreamInfo_t *pstAudioInfo, StreamInfo_t *pstTalkingInfo, int *audioSyncFlag);

/***
 * @description: 向音频解码通道发送一帧数据
 * @param {void} *hChannelHandle 平台句柄
 * @param {char} *pbuf 帧数据地址
 * @param {int} framesize 帧大小
 * @param {int} audioDataType 音频数据类型
 * @param {int} audioDecMode 音频数据编码类型
 * @return {int} 0-成功 !0-失败
 */
int Hi_MPP_SendFrameToAdev(void *hChannelHandle, char *pbuf, int framesize, int audioDataType, int audioDecMode);

int Hi_MPP_GetTimeBitmap(void *hChannelHandle, time_t *pSysTime, DEV_SUPPORT_SENSOR nSenIndex, ENCODE_STREAM_E eStm);

int Hi_MPP_GetOSDV2Bitmap(void *hChannelHandle);

/**
 * @description: 初始化OSD模块
 * @param {void} *hChannelHandle 平台句柄
 * @return {SINT32} 0-成功，其他-失败
 */
SINT32 Hi_MPP_InitOsd(void *hChannelHandle);

int Hi_MPP_DeInitOsd(void);

/**
 * @description: 启动OSD模块并设置相关词参数
 * @param {void} *hChannelHandle 平台句柄
 * @return {SINT32} 0-成功，其他-失败
 */
SINT32 Hi_MPP_StartOSD(void *hChannelHandle);

/***
 * @description: 设置OSD的叠加内容和使能
 * @param {void} *hChannelHandle 平台句柄
 * @param {STREAM_TYPE_E} eStm 码流类型
 * @param {ENCODE_OSD_TYPE} enOsdType OSD类型
 * @param {BOOL} bEn 是否使能
 * @return {SINT32}
 */
SINT32 Hi_MPP_SetOSD(void *hChannelHandle, DEV_SUPPORT_SENSOR nSenIndex, ENCODE_STREAM_E eStm, ENCODE_OSD_TYPE enOsdType, BOOL bEn);

int Hi_MPP_SetBitMapAttrToGetAddrFromPlat(void *hChannelHandle, DEV_SUPPORT_SENSOR nSenIndex, ENCODE_STREAM_E eStm, ENCODE_OSD_TYPE eOsdType, VOSD_OVERYLAY_S *stOverylay, int update);

int GetOSDFontSizeByRes(int width, int height, int *scaleWidth, int *scaleHeight);

/**
 * @description: 打开 VDA 模块
 * @param {void} *hChannelHandle 平台适配层句柄
 * @return {SINT32} 正常返回0，异常返回<0
 */
SINT32 Hi_MPP_InitVda(void *hChannelHandle);

/**
 * @description: 关闭 VDA 模块
 * @param {void}
 * @return {SINT32} 正常返回0，异常返回<0
 */
SINT32 Hi_MPP_DeInitVda(void);

/**
 * @description: 创建移动检测和遮挡检测的通道
 * @param {void} *hChannelHandle 平台适配层句柄
 * @return {SINT32} 正常返回0，异常返回<0
 */
SINT32 Hi_MPP_InitVdaChn(void *hChannelHandle);

/**
 * @description: 销毁移动侦测和遮挡检测的通道
 * @param {void}
 * @return {SINT32} 正常返回0，异常返回<0
 */
SINT32 Hi_MPP_DeInitVdaChn(void);

/**
 * @description: 使能移动侦测
 * @param {void} *hChannelHandle 平台适配层句柄
 * @return {SINT32} 正常返回0，异常返回<0
 */
SINT32 Hi_MPP_StartMD(void *hChannelHandle);

/**
 * @description: 关闭移动侦测
 * @param {void} *hChannelHandle 平台适配层句柄
 * @return {SINT32} 正常返回0，异常返回<0
 */
SINT32 Hi_MPP_StopMD(void *hChannelHandle);

/**
 * @description: 设置移动侦测灵敏度
 * @param {void} *hChannelHandle 平台适配层句柄
 * @param {int} MDIntervel 检测时间间隔(无效)
 * @param {int} motionSensitivityLev 灵敏度（2 - 高，3 - 中， 4 - 低）
 * @return {SINT32} 正常返回0，异常返回<0
 */
SINT32 Hi_MPP_MDSetPrecision(void *hChannelHandle, int MDIntervel, int motionSensitivityLev);

/**
 * @description: 获取移动侦测结果
 * @param {void} *hChannelHandle 平台适配层句柄
 * @param {int} MotionAreas 检测区域总数(无效)
 * @param {int} MotionGrade 检测灵敏度(无效)
 * @param {int} MotionRectStride 宏块个数
 * @param {char} *MotionRect 设置的宏块是否启动检测
 * @param {char} *pMotion 用户设置的宏块个数
 * @param {int} *motion 移动检测的结果(1:有运动,0:无运动)
 * @return {SINT32} 正常返回0，异常返回<0
 */
SINT32 Hi_MPP_MotionAnalyzer(void *hChannelHandle, int MotionAreas, int MotionGrade, int MotionRectStride, char *pMotion, int *motion,int *Chn);

int Hi_MPP_ReadTalkingStream(char *pAudioBuf, int *pAudioLen, int AeChn);

int Hi_MPP_SetLocalIrcutMode(void *hChannelHandle, int mode);

int Hi_MPP_AudioInit(void *hChannelHandle);


int Hi_MPP_AudioUninit(CPlatformChannelContext *pChContext);

int Hi_MPP_SetAntiFlickerStrengthParam(void *hChannelHandle, int strength);

int Hi_MPP_SimulateColor2Grey(void *hChannelHandle, int enable);

/* VO相关 */
int Hi_MPP_VO_StopDev(UINT32 VoDev);
int Hi_MPP_VO_StopLayer(UINT32 VoLayer);
int Hi_MPP_VO_StopChn(UINT32 VoLayer, VO_MODE_E enMode);

/* 卸载VPSS和VO相关相关 */
int Hi_MPP_VENC_Stop(void *hChannelHandle, UINT32 VencChn);

/**
 * @description: 修改编码的帧率(降帧时不进行帧控, 则编码帧率默认跟随 sensor 帧率; 不降帧时设置回用户的配置)
 * @param {void} *hChannelHandle 平台适配层句柄
 * @param {int} bCtrlFps  是否进行帧控
 * @return {SINT32} 正常返回0，异常返回<0
 */
SINT32 Hi_MPP_ChangeEncFps(void *hChannelHandle, int bCtrlFps);

int Hi_MPP_InitIVP(void *hChannelHandle);

int Hi_MPP_AlgoInit(void *hChannelHandle);
int Hi_MPP_AlgounInit();

int Hi_MPP_SetHumanDetectEnable(void *hChannelHandle, int bEnable);

int Hi_MPP_SetHumanTrackEn(void *hChannelHandle, int bEnable);

/**
 * @description: 关闭人形算法并反初始化
 * @param {void} * hChannelHandle 平台适配层句柄
 * @return {SINT32} 正常返回0，异常返回<0
 */
SINT32 Hi_MPP_SetHumanDetectDisEnable(void *hChannelHandle);

int Hi_MPP_SetHumanShapeCfg();
int Hi_MPP_GetBitrateInfo(void *hChannelHandle, int dataArr[], int arrlen);
int Hi_MPP_SwitchAOMode(CPlatformChannelContext *pChContext, int mode);
int Hi_MPP_SetHumanShapeCfgV2();

/**
 * @description: 人形功能检测线程
 * @param {void} *lpParameter 线程参数
 * @return {void *} 
 */
void *Hi_MPP_HDThread(void * lpParameter);

int Hi_MPP_LoadAlgoParame(void *hChannelHandle);

/**
 * @description: 子码流采用在线的方式进行画线画框(主子码流清除画框都在这里处理)
 * @param {void} *hChannelHandle 平台适配层句柄
 * @param {int} *pHumanAreaOverlayEnable 人形目标框叠加使能
 * @param {int} *pDetectAreaOverlayEnable 布防框叠加使能
 * @param {int} *pCarAreaOverlayEnable 车型框叠加使能
 * @param {int} *pFireAreaOverlayEnable 火焰框叠加使能
 * @param {int} motionDetectEnable 移动侦测使能
 * @param {int} motioning 是否正在移动
* @param {int}  callCycleMs 调用的周期
 * @return {SINT32} 正常返回0，异常返回<0
 */
int Hi_MPP_ReadyDrawLineInfo(void *hChannelHandle,
							 int *pHumanAreaOverlayEnable,
							 int motionDetectEnable,
							 int motioning,
							 int callCycleMs);

int Hi_MPP_ClearDrawLineInfo(int ClearType);

/*************** 编码相关封装接口 *************/
/**
 * @description: 初始化编码通道句柄
 * @param {void *} *hChannelHandle 平台适配层句柄
 * @return {SINT32} 正常返回0，异常返回<0
 */
SINT32 Hi_MPP_initHandle(void **hChannelHandle);

/**
 * @description: 销毁编码通道句柄
 * @param {void *} *hChannelHandle 平台适配层句柄
 * @return {SINT32} 正常返回0，异常返回<0
 */
SINT32 Hi_MPP_DeinitHandle(void **hChannelHandle);

/**
 * @description: VI模块初始化,内部自动分配系统资源
 * @param {void} *hChannelHandle 平台适配层句柄
 * @return {SINT32} 正常返回0，异常返回<0
 */
SINT32 Hi_MPP_InitVi(void *hChannelHandle);

/**
 * @description: VI模块反初始化
 * @param {void}
 * @return {SINT32} 正常返回0，异常返回<0
 */
SINT32 Hi_MPP_DeInitVi(void);

/**
 * @description: VPSS模块初始化,内部自动分配系统资源
 * @param {void} *hChannelHandle 平台适配层句柄
 * @return {SINT32} 正常返回0，异常返回<0
 */
SINT32 Hi_MPP_InitVpss(void *hChannelHandle);

/**
 * @brief  VPSS通道初始化
 * @param  hChannelHandle 编码通道句柄
 * @return SINT32 正常返回0
 */
SINT32 Hi_MPP_InitVpssChn(void *hChannelHandle);

/**
 * @description: VPSS模块反初始化
 * @param {void}
 * @return {SINT32} 正常返回0，异常返回<0
 */
SINT32 Hi_MPP_DeInitVpss(void);

/**
 * @description: Venc模块初始化,内部自动分配系统资源
 * @param {void} *hChannelHandle 平台适配层句柄
 * @return {SINT32} 正常返回0，异常返回<0
 */
SINT32 Hi_MPP_InitVenc(void *hChannelHandle);

/**
 * @description: Venc模块反初始化
 * @param {void}
 * @return {SINT32} 正常返回0，异常返回<0
 */
SINT32 Hi_MPP_DeInitVenc(void);

/**
 * @description: ISP模块初始化,内部自动分配系统资源
 * @param {void} *hChannelHandle 平台适配层句柄
 * @return {SINT32} 正常返回0，异常返回<0
 */
SINT32 Hi_MPP_InitIsp(void *hChannelHandle);

/**
 * @description: 启动各个模块
 * @param {void}
 * @return {SINT32} 正常返回0，异常返回<0
 */
SINT32 Hi_MPP_DeInitIsp(void);

/**
 * @description: 创建视频输入通道
 * @param {void} *hChannelHandle 平台适配层句柄
 * @return {SINT32} 正常返回0，异常返回<0
 */
SINT32 Hi_MPP_InitViChn(void *hChannelHandle);

/**
 * @description: 销毁视频输入通道
 * @param {void} *hChannelHandle 平台适配层句柄
 * @return {SINT32} 正常返回0，异常返回<0
 */
SINT32 Hi_MPP_DeInitViChn(void);

/**
 * @description: 创建编码通道，并进行Venc通道绑定
 * @param {SINT32} StreamNum 编码通道号 >= 0
 * @param {void} *hChannelHandle 平台适配层句柄
 * @return {SINT32} 正常返回0，异常返回<0
 */
SINT32 Hi_MPP_InitEncChn(void *hChannelHandle);

/**
 * @description: 销毁编码通道
 * @param {void}
 * @return {SINT32} 正常返回0，异常返回<0
 */
SINT32 Hi_MPP_DeInitEncChn(void);

/**
 * @description: 强制I帧
 * @param {void} *hChannelHandle 平台句柄
 * @return {SINT32} 正常返回0，异常返回<0
 */
SINT32 Hi_MPP_ForceIFrame(void *hChannelHandle, int nSensorIndex);

/**
 * @description: 设置编码参数(主要是设置编码参数，不涉及分辨率)
 * @param {void} *hChannelHandle 平台适配层句柄
 * @param {CODEC_PARA_S} *pParam 用户设置的配置
 * @param {int} Chn 编码通道号
 * @param {EncodeType_t} encType 编码类型
 * @param {int} iSetFrom :1 修改码率  0:还原码率
 * @param {int} nRateTurnDir :2 调大码率  1:调小码率 0:码率用用户设置的。
 * @param {int} SlowFrameMode :低帧率模式
 * @return {SINT32} 正常返回0，异常返回<0
 */
SINT32 Hi_MPP_SetEncPara(void *hChannelHandle, CODEC_PARA_S *pParam, ENCODE_STREAM_E Chn, int iSetFrom, int SlowFrameMode);

/**
 * @description: 改变编码分辨率
 * @param {void} *hChannelHandle 平台适配层句柄
 * @param {ENCODE_STREAM_E} eStream编码通道号
 * @return {SINT32} 正常返回0，异常返回<0
 */
SINT32 Hi_MPP_SwitchPicFormat(void *hChannelHandle, ENCODE_STREAM_E eStream);

/**
 * @description: 更新用户设置的分辨率到 XRS_StreamAttr
 * @param {void} *hChannelHandle 平台适配层句柄
 * @param {ResolutionParam_t} *m_EncAttr用户设置的主码流分辨率
 * @param {ResolutionParam_t} *m_EncAttrAux用户设置的子码流分辨率
 * @param {int} ifCaptureThird是否支持第三码流
 * @param {ResolutionParam_t} *m_EncAttrThird用户设置的第三码流分辨率
 * @param {int} ifCaptureFourth是否支持第四码流
 * @param {ResolutionParam_t} *m_EncAttrFourth用户设置的第四码流分辨率
 * @return {SINT32} 成功 0, 失败 -1;
 */
SINT32 Hi_MPP_UpdatePictureFormat(void *hChannelHandle,
										  ResolutionParam_t *m_EncAttr, 
										  ResolutionParam_t *m_EncAttrAux,
					int ifCaptureFourth,  ResolutionParam_t *m_EncAttrFourth);

/**
 * @description: 将用户设置编码参数 XRS_VENC_ChnAttr 中
 * @param {void} *hChannelHandle 平台适配层句柄
 * @param {CODEC_PARA_S} *m_EncAttr用户设置的主码流编码参数
 * @param {CODEC_PARA_S} *m_EncAttrAux用户设置的子码流编码参数
 * @param {int} ifCaptureThird是否支持第三码流
 * @param {CODEC_PARA_S} *m_EncAttrThird用户设置的第三码流编码参数
 * @param {int} ifCaptureFourth是否支持第四码流
 * @param {CODEC_PARA_S} *m_EncAttrFourth用户设置的第四码流编码参数
 * @return {SINT32} 成功 0, 失败 -1;
 */
SINT32 Hi_MPP_InitUserEncodeParam(void *hChannelHandle,
										  CODEC_PARA_S *m_EncAttr, 
										  CODEC_PARA_S *m_EncAttrAux,
					int ifCaptureFourth,  CODEC_PARA_S *m_EncAttrFourth);

/**
 * @description: 设置用户参数
 * @param {void} *hChannelHandle 平台适配层句柄
 * @param {int} boardInfo 版型
 * @param {EncodeType_t} mainEncType 主码流编码类型
 * @param {CODEC_PARA_S} *m_EncAttr用户设置的主码流编码参数
 * @param {EncodeType_t} auxEncType 子码流编码类型
 * @param {CODEC_PARA_S} *m_EncAttrAux用户设置的子码流编码参数
 * @param {int} ifCaptureThird是否支持第三码流
 * @param {EncodeType_t} thirdEncType 第三码流编码类型
 * @param {CODEC_PARA_S} *m_EncAttrThird用户设置的第三码流编码参数
 * @param {int} ifCaptureFourth是否支持第四码流
 * @param {EncodeType_t} fourthEncType 第四码流编码类型
 * @param {CODEC_PARA_S} *m_EncAttrFourth用户设置的第四码流编码参数
 * @return {SINT32} 正常返回0，异常返回<0
 */
SINT32 Hi_MPP_SetUserParam(void *hChannelHandle, int boardInfo,
              					  CODEC_PARA_S *m_EncAttr, 
              					  CODEC_PARA_S *m_EncAttrAux,
              int ifCaptureFourth,CODEC_PARA_S *m_EncAttrFourth);

/**
 * @description: 4K20帧采用离线方式,将VPSS的YUV数据处理后发送给编码
 * @param {void}
 * @return {SINT32} 正常返回0，异常返回<0
 */
SINT32 Hi_MPP_SendMainFrameToEncode(void *hChannelHandle);

/**
 * @description: 4K 5M 模型切换时重启编码器
 * @param {void} *hChannelHandle 平台适配层句柄
 * @return {SINT32} 正常返回0，异常返回<0
 */
SINT32 Hi_MPP_RestartEnc(void *hChannelHandle);

/********* 遮挡检测 *********/
/**
 * @description: 使能遮挡检测
 * @param {void} *hChannelHandle 平台适配层句柄
 * @return {SINT32} 正常返回0，异常返回<0
 */
SINT32 Hi_MPP_StartOD(void *hChannelHandle);

/**
 * @description: 关闭遮挡检测
 * @param {void} *hChannelHandle 平台适配层句柄
 * @return {SINT32} 正常返回0，异常返回<0
 */
SINT32 Hi_MPP_StopOD(void *hChannelHandle);

/**
 * @description: 设置遮挡检测灵敏度
 * @param {void} *hChannelHandle 平台适配层句柄
 * @param {int} OcclusionSensitivityLev 灵敏度（2 - 高，3 - 中， 4 - 低）
 * @return {SINT32} 正常返回0，异常返回<0
 */
SINT32 Hi_MPP_ODSetPrecision(void *hChannelHandle, int OcclusionSensitivityLev);

/**
 * @description: 获取遮挡检测结果
 * @param {void} *hChannelHandle 平台适配层句柄
 * @param {int} *OdStat 遮挡检测的结果(1:有遮挡,0:无遮挡)
 * @return {SINT32} 正常返回0，异常返回<0
 */
SINT32 Hi_MPP_OcclusionDetectAnalyzer(void *hChannelHandle, int *OdStat);

/********* ISP 模块相关封装接口 ************/
/**
 * @description: 设置去雾强度
 * @param {void} *hChannelHandle 平台句柄
 * @param {int} strength 用户设置的去雾强度
 * @return {SINT32} 正常返回0，异常返回<0
 */
SINT32 Hi_MPP_SetDefogStrengthParam(void *hChannelHandle, int strength);

/**
 * @description: 设置图像参数
 * @param {void} *hChannelHandle 平台句柄
 * @param {int} luma 亮度
 * @param {int} Contrast 对比度
 * @param {int} hue 色度
 * @param {int} Saturation 饱和度
 * @param {int} Sharpness 锐度
 * @return {SINT32} 正常返回0，异常返回<0
 */
SINT32  Hi_MPP_SetImgPara(void *hChannelHandle, int Luma, int Contrast, int Hue, int Saturation, int Sharpness);

/**
 * @description: 设置白平衡参数
 * @param {void} *hChannelHandle 平台句柄
 * @param {int} enable 是否使能白平衡(除了自动白平衡为0,其他全为1)
 * @param {int} wbMode 白平衡模式 0:白织灯,1:暖光灯,2:日光灯,3:自然光,4:手动白平衡,5:自动白平衡,6:锁定白平衡
 * @param {int} redGain 红增益  0-100
 * @param {int} blueGain 蓝增益  0-100
 * @return {SINT32} 正常返回0，异常返回<0
 */
SINT32 Hi_MPP_SetWhiteBalance(void *hChannelHandle, int enable, int wbMode, int redGain, int blueGain);

/**
 * @description: 设置降噪参数
 * @param {void} *hChannelHandle 平台句柄
 * @param {int} MCTFStrength 降噪强度0:自动,1:弱,2:一般,3:强
 * @return {SINT32} 正常返回0，异常返回<0
 */
SINT32 Hi_MPP_SetMCTFStrength(void *hChannelHandle, int MCTFStrength);

/**
 * @description: 设置背光补偿使能
 * @param {void} *hChannelHandle 平台句柄
 * @param {int} enable 使能 0:关,1:开
 * @return {SINT32} 正常返回0，异常返回<0
 */
SINT32 Hi_MPP_EnableBacklightMode(void *hChannelHandle, int enable);

/**
 * @description: 设置宽动态参数
 * @param {void} *hChannelHandle 平台句柄
 * @param {int} ExpMode 宽动态强度0:自动,1:弱,2:一般,3:强
 * @return {SINT32} 正常返回0，异常返回<0
 */
SINT32 Hi_MPP_SetLocalExpMode(void *hChannelHandle, int ExpMode);

/**
 * @description: 设置曝光参数
 * @param {void} *hChannelHandle 平台句柄
 * @param {SINT32} antiFlicker 防闪烁模式(0:50Hz, 1:60Hz, 2:关闭)
 * @param {UINT32} minShutterTime 曝光最低时间(无效参数)
 * @param {UINT32} maxShutterTime 曝光时间:10~10000
 * @param {UINT32} sensorGain 增益:0~42
 * @param {UINT32} ae_mode 曝光模式(0:手动,1:自动)
 * @return {SINT32} 正常返回0，异常返回<0
 */
SINT32 Hi_MPP_SetAEParam(void *hChannelHandle, SINT32 antiFlicker, UINT32 minShutterTime, UINT32 maxShutterTime, UINT32 sensorGain, SINT32 ae_mode);

/**
 * @brief  获取AE参数
 * @param  hChannelHandle 编码通道句柄
 * @param  pstAeParam AE参数结构体
 * @return SINT32 正常返回0，异常返回<0
 */
SINT32 Hi_MPP_GetAEParam(void *hChannelHandle, ISP_TUNE_AE_PARAM_S *pstAeParam);

/**
 * @description: 设置本地图像模式
 * @param {void} *hChannelHandle 平台句柄
 * @param {int} mode 0:标准模式,1:人脸无曝光模式,2:车牌模式
 * @return {SINT32} 正常返回0，异常返回<0
 */
SINT32 Hi_MPP_SetLocalImageMode(void *hChannelHandle, int mode);

/**
 * @description: 设置镜像翻转
 * @param {void} *hChannelHandle 平台适配层句柄
 * @param {int} flipRotate 翻转角度 0:正常, 1:90, 2:180,3:270(暂时只支持0度和180度)
 * @return {SINT32} 正常返回0，异常返回<0
 */
SINT32 Hi_MPP_SetFlipRotateMode(void *hChannelHandle, int flipRotate);

/**
 * @description: 画面切换到黑白或者彩色
 * @param {int} DayOrNight 0:晚上 1:白天(黑白效果或者彩色效果)
 * @return {SINT32} 正常返回0，异常返回<0
 */
SINT32 Hi_MPP_SwitchScreenColor(int DayOrNight);

/**
 * @description: 通过实时亮度获取当前的日夜模式
 * @param {void}
 * @return {SINT32} 正常返回0，异常返回<0
 */
SINT32 Hi_MPP_GetCurrDayNightStatus(IR_ISP_INFO_S *LightIrInfo);

/**
 * @description: 设置 sensor 宽动态模式 
 * @param {int} SensorWdrMode Sensor 宽动态强度 0: SDR, 1:WDR
 * @return {SINT32} 正常返回0，异常返回<0
 */
SINT32 Hi_MPP_SetSensorWDRMode(int SensorWdrMode);

/**
 * @description: 设置 ISP 数字宽动态参数
 * @param {int} IspWdrMode 数字宽动态强度0:关闭,1:弱,2:一般,3:强,4:自动
 * @return {SINT32} 正常返回0，异常返回<0
 */
SINT32 Hi_MPP_SetDigitalWDRMode(int IspWdrMode);

/**
 * @description: 4K 5M 模型切换时销毁 ISP 后需要更新用户设置
 * @param {void} 
 * @return {SINT32} 正常返回0，异常返回<0
 */
SINT32 Hi_MPP_ResetIspParam(void);

/* 硬件相关封装接口 */
/**
 * @description: 初始化KS HalMod模块
 * @return {int} 正常返回0，异常返回<0
 */
SINT32 Hi_MPP_HalModInit();

/**
 * @description: 反初始化KS HalMod
 * @return {int} 正常返回0，异常返回<0
 */
SINT32 Hi_MPP_HalModDeInit();

/**
 * @description: 设置系统启动状态标志
 * @param {int} Stat 启动状态标记 0: 正常, 1: 错误
 * @return {int} 正常返回0，异常返回<0
 */
SINT32 Hi_MPP_SetSysStartTag(int Stat);

/**
 * @description: 获取复位按键状态
 * @param {UINT8} u8KeySt
 * @return {SINT32} 正常返回0，异常返回<0
 */
SINT32 Hi_MPP_GetRestKeyStatus(KEY_STATUS_E* eKeySt);

/**
 * @description: 设置IRCUT模式，会直接切换IR-CUT
 * @param {void} *hChannelHandle 平台适配层句柄
 * @param {IRCUT_MODE_E} eMode IRCUT模式
 * @return {SINT32} 正常返回0，异常返回<0
 */
SINT32 Hi_MPP_SetIRCutMode(IRCUT_MODE_E eMode);

/* 灯光控制脚 */
/**
 * @description: 设置白光补光灯
 * @param {UINT32} u32Duty 占空比，0-关闭，100-最亮
 * @param {UINT32} u32FrqKHZ PWM频率
 * @return {SINT32} 正常返回0，异常返回<0
 */
SINT32 Hi_MPP_SetWhiteLight(UINT32 u32Duty, UINT32 u32FrqKHZ);

/**
 * @description: 设置红光补光灯
 * @param {UINT32} u32Duty 占空比，0-关闭，100-最亮
 * @param {UINT32} u32FrqKHZ PWM频率
 * @return {SINT32} 正常返回0，异常返回<0
 */
SINT32 Hi_MPP_SetRedLight(UINT32 u32Duty, UINT32 u32FrqKHZ);

// IR 
/**
 * @description: 获取硬光敏电平状态
 * @param {SINT32} *pDayNightStatus 1:白天 0:夜晚
 * @return {SINT32} 正常返回0，异常返回<0
 */
SINT32 Hi_MPP_GetIRStateLevel(UINT32 *pDayNightStatus);

/* 报警输出 */
/**
 * @description: 设置报警输出
 * @param {UINT32} u32Port 报警输出端口
 * @param {BOOL} bAlarm	是否报警
 * @return {SINT32} 0-成功 !=0 失败
 */
SINT32 Hi_MPP_SetAlarmOut(UINT32 u32Port, BOOL bAlarm);

/* 报警输入 */
/**
 * @description: 获取报警输入状态
 * @param {BOOL} *bAlarm 输出报警输入状态，TRUE-报警，FLASE-正常
 * @return {SINT32} 0-成功，!0-失败
 */
SINT32 Hi_MPP_GetAlarmIn(BOOL *bAlarm);

/* 喇叭输出使能 */
/**
 * @description: 扩展板喇叭输出使能控制
 * @param {BOOL} bSpkEn TRUE-使能，FLASE-失能
 * @return {SINT32} 0-成功，!0-失败
 */
SINT32 Hi_MPP_SetSpeakerEn(BOOL bSpkEn);

/**
 * @description: 获取设备得唯一CHIP ID
 * @param {SINT8} *buffer 缓冲区
 * @param {SINT32} len 缓冲区长度
 * @return {SINT32} 0-成功， 非0-失败
 */
SINT32 Hi_MPP_GetDevChipID(SINT8 *buffer, SINT32 len);

/**
 * @brief  统计平均码率
 * @param  hChannelHandle 平台通道句柄
 * @param  eFrameType 码流类型
 * @param  s32FrameSize 帧大小
 * @return int 返回0为正常，返回其他值为异常
 */
int Hi_MPP_GetAvgBitrate(void *hChannelHandle, STREAM_TYPE_E eFrameType, int s32FrameSize);

int Hi_MPP_GetMaxFrameRateByAntiflashAndNetType(int *pMaxFrameRate);
int Hi_MPP_GetGOPValueByNetType(int frameRate, UINT16 *pGop);
SINT32 Hi_MPP_ChangeSensorFps(void *hChannelHandle, int nChn, FLOAT32 SensorFps);
SINT32 Hi_MPP_SetVencFrameRateV2(void *hChannelHandle, ENCODE_STREAM_E ChnType, int FrmRate, int slowframemode);
int Hi_MPP_DeInit(void);
int Hi_MPP_ChangeAudioEnc(CPlatformChannelContext *pPlatCtx);
SINT32 Hi_MPP_GetIRStateLevel(UINT32 *pDayNightStatus);
int Hi_MPP_SetIVPCfgV2(void *hChannelHandle, HumanShapeCfgV2 *pHumanDetectUserCfg);

int Hi_MPP_SetAudioDetectCfg(void *hChannelHandle, LpAudioDetectCfg pCfg);

int Hi_MPP_SwitchAecMode(void *hChannelHandle, int AecMode,int m_IntercomMode);

void GetCurSensorFrame(UINT32 *sensorFrame);

int Hi_MPP_GetWiFiInfo(void *hi_wifiInfo);

int Hi_MPP_GetRealKeyFrameInterval(int nChannel,int *nFrameRateInterval, int *nKeyFrameInterval);

#ifdef __DIGITAL_ZOOM_DRAG_SUPPORT__
int Hi_MPP_GetZoomBitmap(void *hChannelHandle, DEV_SUPPORT_SENSOR nSenIndex, ENCODE_STREAM_E eStm);
#endif
#ifdef __ONE_CLICK_CALL__
SINT32 Hi_MPP_GetCallKeyStatus(KEY_STATUS_E* eCallKeySt, KEY_STATUS_E* eHangupKeySt);
#endif

#endif
