#ifndef _AUDIO_COMM_H_
#define _AUDIO_COMM_H_

#include "xrsstd.h"
#include "msstd.h"

#define MAX_AUDIO_CHN   17  /* 最大可能通道数 */
#define MAX_ADEC_CHN    3   /* 解码通道数目 */
#define AO_CHN_NUM      2   /* 每一个AO有2个Chn */
#define MAX_AI_DEV_NUMS 4
#define MAX_AI_CHN_NUMS 32
#define AENC_CHN_NUM    2

#define MIN_AUDIO_VOLUME    0
#define MAX_AUDIO_VOLUME    100

/* 音频编码格式(目前支持PCM和G711) */
typedef enum
{
    AUDIO_PCM   = MSSTD_ALGORITHM_AUDIO_PCM8_16, 
    AUDIO_G711A = MSSTD_ALGORITHM_AUDIO_G711A, 
    AUDIO_G722  = MSSTD_ALGORITHM_AUDIO_G722,   /* 暂不支持 */
    AUDIO_G726  = MSSTD_ALGORITHM_AUDIO_G726, 
    AUDIO_PCM16 = MSSTD_ALGORITHM_AUDIO_PCM16,
    AUDIO_G711U = MSSTD_ALGORITHM_AUDIO_G711U,
    AUDIO_MP3   = MSSTD_ALGORITHM_AUDIO_MP3,
    AUDIO_AAC   = MSSTD_ALGORITHM_AUDIO_AAC,
    AUDIO_TYPE_BUTT
} AUDIO_TYPE_E;

/* 音频物理设备类型AI分为本地和对讲 */
typedef enum
{
    AUDIO_DEV_AI_LOCAL = 0, 
    AUDIO_DEV_AI_TALK, 
    AUDIO_DEV_AENC, 
    AUDIO_DEV_ADEC, 
    AUDIO_DEV_AO, 
    AUDIO_DEV_BUTT
} AUDIO_DEV_TYPE_E;

typedef enum
{
	AI_SRC_ANALOG,   /* 音频通道为模拟音频 */
	AI_SRC_DIGIT,    /* 音频通道为数字音频 */
	AI_SRC_BUTT
} AI_SRC_TYPE_E;

typedef enum
{
	AI_CHTYPE_LOCAL,    /* 本地音频 */
	AI_CHTYPE_TALK,     /* 对讲音频 */
	AI_CHTYPE_BUTT
} AI_CHTYPE_E;

typedef struct
{
    SINT32 enable;
    SINT32 ChnMapTbl[MAX_AI_CHN_NUMS];
} AI_PHY_MAP_CFG_S, *PAI_PHY_MAP_CFG_S;

typedef enum MvAUDIO_SAMPLE_RATE_E {
    XRS_AUDIO_SAMPLE_RATE_8000   = 8000,    /* 8K samplerate */
    XRS_AUDIO_SAMPLE_RATE_12000  = 12000,   /* 12K samplerate */
    XRS_AUDIO_SAMPLE_RATE_11025  = 11025,   /* 11.025K samplerate */
    XRS_AUDIO_SAMPLE_RATE_16000  = 16000,   /* 16K samplerate */
    XRS_AUDIO_SAMPLE_RATE_22050  = 22050,   /* 22.050K samplerate */
    XRS_AUDIO_SAMPLE_RATE_24000  = 24000,   /* 24K samplerate */
    XRS_AUDIO_SAMPLE_RATE_32000  = 32000,   /* 32K samplerate */
    XRS_AUDIO_SAMPLE_RATE_44100  = 44100,   /* 44.1K samplerate */
    XRS_AUDIO_SAMPLE_RATE_48000  = 48000,   /* 48K samplerate */
    XRS_AUDIO_SAMPLE_RATE_64000  = 64000,   /* 64K samplerate */
    XRS_AUDIO_SAMPLE_RATE_96000  = 96000,   /* 96K samplerate */
    XRS_AUDIO_SAMPLE_RATE_BUTT,
} AUDIO_SAMPLE_RATE_E;

typedef enum MvAUDIO_BITWIDTH_E {
    XRS_AUDIO_BIT_WIDTH_8   = 0,   /* 8bit width */
    XRS_AUDIO_BIT_WIDTH_16  = 1,   /* 16bit width */
    XRS_AUDIO_BIT_WIDTH_24  = 2,   /* 24bit width */
    XRS_AUDIO_BIT_WIDTH_BUTT,
} AUDIO_BITWIDTH_E;

typedef enum MvAUDIO_NRLEVEL_E {
    AUDIO_NR_LEVEL_1  = 1,   /* LOW */
    AUDIO_NR_LEVEL_2  = 2,   /* MIDDLE */
    AUDIO_NR_LEVEL_3  = 3,   /* HIGH */
    AUDIO_NR_LEVEL_BUTT,
} AUDIO_NRLEVEL_E;

typedef struct
{
    AUDIO_SAMPLE_RATE_E enRate; /* 采样率 */
    AUDIO_BITWIDTH_E enBiW;     /* 位宽 */
    AUDIO_NRLEVEL_E enNrLevel;  /* 降噪等级: 数值越大降噪等级越强(AUDIO_NRLEVEL_E)*/  
	UINT32 u32NumPerFrm;        /* 采样点，每帧采样点数目 */
	UINT32 u32CacheNum;		/* 音频帧缓存数量 */
} AUDIO_CONF_S, *PAUDIO_CONF_S;
#define AUDIO_CONF_S_LEN   sizeof(AUDIO_CONF_S)

typedef struct
{
    UINT32 MertSize;		/* 君正音频算法Mert使用nmem内存大小,单位字节；多进程时需传入该进程使用nmem内存之和 */
	UINT32 MertEnable;		/* 君正音频算法Mert使能开关，关闭时使用SDK自带回声消除。项目使用mert功能需基线配置支持*/
    UINT32 Resvd[2];     	/* 保留位 */
} AI_JZ_PLAT_CFG_S, *PAI_JZ_PLAT_CFG_S;

typedef struct
{
    UINT32 VqeV2Enable;		/* 海思音频talk vqe v2算法使能开关 ：关闭时使用talk vqe算法*/
    UINT32 Resvd[3];     	/* 保留位 */
} AI_HS_PLAT_CFG_S, *PAI_HS_PLAT_CFG_S;

typedef struct
{
	union{
		AI_JZ_PLAT_CFG_S JzPlat;	/*!< 君正平台配置*/
		AI_HS_PLAT_CFG_S HsPlat;	/*!< 海思平台配置*/
	};
	union{
		UINT8 Resvd[16];				/*!< 预留位*/				
	};
} AI_PLAT_CFG_S, *PAI_PLAT_CFG_S;

typedef struct
{
    UINT8 MaxLocalChn;  /* 音频本地通道个数 */
    UINT8 MaxTalkChn;   /* 音频对讲通道个数 */
    UINT8 Resvd[2];     /* 保留位 */
    AI_PHY_MAP_CFG_S UsrAiPhyMap;
	AI_PLAT_CFG_S PlatCfg;	/* 平台配置 */
} AI_CONF_S, *PAI_CONF_S;
#define AI_CONF_S_LEN   sizeof(AI_CONF_S)

typedef struct
{
    SINT16 MaxGain;   /* 最大增益 */
    SINT16 Resvd[3]; /* 保留位 */
} AI_AGC_CONF_S, *PAI_AGC_CONF_S;
#define AI_AGC_CONF_S_LEN sizeof(AI_AGC_CONF_S)
ASSERT_SIZEOF_TYPEDEF_STRUCT(AI_AGC_CONF_S, 8);


typedef struct
{   
    UINT8 AencSrcChn;   /* 待编码数据通道号(如AI通道号) */
    UINT8 SrcDevType;   /* 源设备类型(AUDIO_DEV_TYPE_E) */
    UINT8 AencDstChn;   /* 音频编码通道号 */
    UINT8 Resvd;        /* 保留位 */
} AENC_BIND_CONF_S, *PAENC_BIND_CONF_S;
#define AENC_BIND_CONF_S_LEN  sizeof(AENC_BIND_CONF_S)

typedef enum
{
    ADEC_SMODE_FRAME = 0,   /* 帧模式 */
    ADEC_SMODE_STREAM,      /* 流模式 */
    ADEC_SMODE_BUTT
} ADEC_SMODE_E;

typedef enum
{
    ADEC_WRITE_NONBLOCK = 0,    /* 解码写入非阻塞模式 */
    ADEC_WRITE_BLOCKING = 1,    /* 解码写入阻塞模式 */
    ADEC_WRITE_TIMEOUT  = 1000, /* 解码写入超时模式(默认是为1000毫秒,此值可自指定) */
} ADEC_WRITE_MODE_E;

typedef enum
{
    ADEC_WRITE_HDMI0 = 0,   /* 主实时音频 */
    ADEC_WRITE_HDMI1 = 1,   /* 辅实时音频 */
    ADEC_WRITE_TALK  = 2,   /* 对讲音频 */
    ADEC_WRITE_BUTT,        /* */
} ADEC_WRITE_TYPE_E;

typedef enum
{
    ADEC_WRITE_ING = 0,		/* 数据传输中 */
    ADEC_WRITE_END = 1,   	/* 数据传输完 */
} ADEC_WRITE_FINISH_E;

typedef enum
{
    AO_WRITE_ING = 0,		/* 数据传输中 */
    AO_WRITE_END = 1,   	/* 数据传输完 */
} AO_WRITE_FINISH_E;

typedef struct
{
    UINT8 enable;   /* 是否使能 */
    UINT8 Chn;      /* 通道号 */
    UINT8 Resvd[2]; /* 保留位 */
} AENC_CONF_S, *PAENC__CONF_S;
#define AENC_DEC_CONF_S_LEN sizeof(AENC_CONF_S)
ASSERT_SIZEOF_TYPEDEF_STRUCT(AENC_CONF_S, 4);

typedef enum
{
	AO_DEV_CVBS,    /* CVBS音频输出 */
	AO_DEV_HDMI0,   /* HDMI0音频输出(主) */
	AO_DEV_HDMI1,   /* HDMI1音频输出(从) */
	AO_DEV_TALK,    /* 对讲音频输出 */
	AO_DEV_BUTT
} AO_DEV_E;

#define MAX_AO_DEV_NUMS AO_DEV_BUTT

/* 设备AUDIO配置模式,与媒体模块匹配AUDIO的配置信息 */
typedef enum
{
    AUDIO_DEV_CFG_MODE_A,   /* AHD(5104E-C,5104E-D,5108E-C) */
    AUDIO_DEV_CFG_MODE_B,   /* AHD(5104E-D-V2,5108E-D,5116E-C,5108E-D-V2) */
    AUDIO_DEV_CFG_MODE_C,   /* AHD(5216E-D,5416E-D) */
    AUDIO_DEV_CFG_MODE_D,   /* 混合机型使用此配置模式(HVR8832) */
    AUDIO_DEV_CFG_MODE_E,   /* HVR8816 */
    AUDIO_DEV_CFG_MODE_F,	/* NVR9632 */
    AUDIO_DEV_CFG_MODE_G,	/* NVR9634 */
    AUDIO_DEV_CFG_MODE_M,	/* 3798M,3100 */
    AUDIO_DEV_CFG_MODE_N,	/* 5216E-D_V2 */
    AUDIO_DEV_CFG_MODE_O,   /* 新混合机型使用此配置模式(新HVR8832) */
    AUDIO_DEV_CFG_MODE_P,   /* 新混合机型使用此配置模式(新HVR8816) */

    AUDIO_DEV_CFG_MODE_R,   /* 5寸面板机 */
    AUDIO_DEV_CFG_MODE_S,   /* 7寸面板机 */
    AUDIO_DEV_CFG_MODE_T,   /* 3寸面板机 、 IPC*/
    AUDIO_DEV_CFG_MODE_BUTT
} AUDIO_CFG_MODE_E;

/* 解码通道配置信息 */
typedef struct
{
    UINT8 AudioType;    /* 音频类型(AUDIO_TYPE_E) */
    UINT8 AdecSMode;    /* 解码流模式(ADEC_SMODE_E) */
    UINT8 Resvd[2];     /* 保留位 */
} ADEC_CHN_CONF_S, *PADEC_CHN_CONF_S;
#define ADEC_CHN_CONF_S_LEN sizeof(ADEC_CHN_CONF_S)
ASSERT_SIZEOF_TYPEDEF_STRUCT(ADEC_CHN_CONF_S, 4);

typedef struct
{
    UINT8 AoSrcChn;     /* 源设备通道号(一般为VDEC) */
    UINT8 SrcDevType;   /* 源设备类型(AUDIO_DEV_TYPE_E) */
    SINT8 AoDev;        /* AO输出的逻辑设备(AO_DEV_E) */
    UINT8 Resvd;        /* 保留位 */
} AO_BIND_CONF_S, *PAO_BIND_CONF_S;
#define AO_BIND_CONF_S_LEN  sizeof(AO_BIND_CONF_S)

/* WAVE音频头部格式 */
typedef struct
{
	char Riff[4];           /* = "RIFF" */
	SINT32 SizeM8;          /* = FileSize - 8 */
	char Wave[4];           /* = "WAVE" */
	char Fmt[4];            /* = "fmt " */
	SINT32 FmtSize;         /* = 下一个结构体的大小: 16 */

	SINT16 FmtTag;          /* = PCM: 1 */
	SINT16 Chns;            /* = 通道数: 1 */
	SINT32 SamplesPerSec;   /* = 采样率: 8000 | 6000 | 11025 | 16000 */
	SINT32 AvgBytesPerSec;  /* = 每秒字节数: SamplesPerSec * BitsPerSample / 8 */
	SINT16 BlockAlign;      /* = 每采样点字节数: BitsPerSample / 8 */
	SINT16 BitsPerSample;   /* = 量化比特数: 8 | 16 */

	char Data[4];           /* = "data" */
	SINT32 DataSize;        /* = 纯数据长度: FileSize - 44  */
} ADEC_WAVE_PCM_HDR_S, *PADEC_WAVE_PCM_HDR_S;
#define ADEC_WAVE_PCM_HDR_S_LEN sizeof(ADEC_WAVE_PCM_HDR_S)
ASSERT_SIZEOF_TYPEDEF_STRUCT(ADEC_WAVE_PCM_HDR_S, 44);

typedef struct
{
	SINT32 bitrate;              /**<output bitrate*/
	SINT32 nChans;               /**<output channels,range:1,2*/
	SINT32 samprate;             /**<output samplerate*/
	SINT32 bitsPerSample;        /**<output bitwidth*/
	SINT32 outputSamps;          /**<output samples,range:nChans*SamplePerFrame*/
	SINT32 layer;                /**<output layer*/
	SINT32 version;              /**<output version*/
	SINT32 len;                  /**<output len*/

} AIUDIO_USER_FRAME_INFO_S;
#define AIUDIO_USER_FRAME_INFO_S_LEN sizeof(AIUDIO_USER_FRAME_INFO_S)
ASSERT_SIZEOF_TYPEDEF_STRUCT(AIUDIO_USER_FRAME_INFO_S, 32);

typedef struct
{
    UINT8 AudioType;    /* 音频类型(AUDIO_TYPE_E) */
    UINT64 FrmPts;      /* 音频帧pts */
	BUFF_EXT_S FrmBuff;
} AUDIO_FRM_INFO_S, *PAUDIO_FRM_INFO_S;
#define AUDIO_FRM_INFO_S_LEN  sizeof(AUDIO_FRM_INFO_S)

typedef struct {
    UINT32 totalNum; /* total number of channel buffer */
    UINT32 freeNum;  /* free number of channel buffer */
    UINT32 busyNum;  /* busy number of channel buffer */
} AO_CHN_BUF_STATUS_S, *PAO_CHN_BUF_STATUS_S ;


#endif /* _AUDIO_COMM_H_ */
