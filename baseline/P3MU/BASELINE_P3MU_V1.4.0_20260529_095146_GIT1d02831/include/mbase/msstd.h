#ifndef _MSSTD_H_
#define _MSSTD_H_

#include "xrsstd.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/******************************************************************************
 * MSSTD内存存储结构示意图
 * +----------------------------+
 * |    MSSTD INFO              |
 * +----------------------------+
 * |    MSSTD_SEG_S[0]          |
 * +----------------------------+
 * |          ......            |
 * +----------------------------+
 * |    MSSTD_SEG_S[n-1]        |
 * +----------------------------+
 * |                            |
 * |  Audio Data or Video Data  |
 * |                            |
 * +----------------------------+
 * 一个典型的音频帧包括 : MSSTD_S和Audio Data;
 * 一个典型的视频I帧包括: MSSTD_S,若干MSSTD_SEG_S和Video Data;
 * 一个典型的视频P帧包括: MSSTD_S,Video Data;
 *****************************************************************************/
#define MSSTD_FLAG  0x5647454D  /* 帧头标志(MEGV) */

#define MSSTD_MIN_FRAME_RATE    1   /* 最小帧率 */
#define MSSTD_MAX_FRAME_RATE    255 /* 最大帧率 */

#define MSSTD_HEAD_ALIGN_SIZE   4   /* MSSTD媒体头对齐长度 */
#define MSSTD_DATA_ALIGN_SIZE   4   /* MSSTD媒体数据对齐长度 */

#define MSSTD_MIN_HEAD_SIZE MSSTD_S_LEN                                     /* 媒体头最小长度(MSSTD_HEAD_ALIGN_SIZE字节对齐) */
#define MSSTD_MAX_HEAD_SIZE ALIGN_BACK(0xFFFF, MSSTD_HEAD_ALIGN_SIZE)       /* 媒体头最大长度(MSSTD_HEAD_ALIGN_SIZE字节对齐) */
#define MSSTD_MAX_DATA_SIZE ALIGN_BACK(0xFFFFFFFF, MSSTD_DATA_ALIGN_SIZE)   /* 媒体数据最大长度(MSSTD_DATA_ALIGN_SIZE字节对齐) */

typedef enum
{
    MSSTD_DATA = 0,    /* 音视频数据以包模式 */
    MSSTD_HEAD = 1,    /* 音视频数据以帧模式 */
} MSSTD_DATA_MODE_E;

/* Video And Audio ALGORITHM */
typedef enum
{
    MSSTD_ALGORITHM_VIDEO_H264_HISILICON    = 0,    /* Hisilicon H.264 */
    MSSTD_ALGORITHM_VIDEO_H264_AMBARELLA    = 1,    /* Ambarella H.264 */
    MSSTD_ALGORITHM_VIDEO_H264_TECHWELL     = 2,    /* Techwell  H.264 */
    MSSTD_ALGORITHM_VIDEO_H264_STANDARD     = 3,    /* Standard  H.264 */
    MSSTD_ALGORITHM_VIDEO_H265_HISILICON    = 4,    /* Hisilicon H.265 */
    MSSTD_ALGORITHM_VIDEO_H265_GENERAL      = 5,    /* General   H.265 */
    MSSTD_ALGORITHM_VIDEO_JPEG_STANDARD     = 29,   /* Standard  JPEG */
    MSSTD_ALGORITHM_VIDEO_MPEG4             = 30,   /* MPEG4 */
    MSSTD_ALGORITHM_VIDEO_MPEG4_ISO         = 31,   /* MPEG4 ISO */
    MSSTD_ALGORITHM_VIDEO_MAX               = 128,   

    MSSTD_ALGORITHM_AUDIO_PCM8_16           = 128,  /* 16-bit PCM只保留高8位(解码后为,1通道,8000Hz采样率,16-bit的PCM数据) */
    MSSTD_ALGORITHM_AUDIO_G711A             = 129,  /* G.711 Alaw(解码后为,1通道,8000Hz采样率,16-bit的PCM数据) */
    MSSTD_ALGORITHM_AUDIO_G722              = 130,  /* G.722(解码后为,1通道,8000Hz采样率,16-bit的PCM数据) */
    MSSTD_ALGORITHM_AUDIO_G726              = 131,  /* G.726(解码后为,1通道,8000Hz采样率,16-bit的PCM数据) */
    MSSTD_ALGORITHM_AUDIO_PCM16             = 132,  /* 16-bit PCM(不需要解码,1通道,8000Hz采样率,16-bit的PCM数据)*/
    MSSTD_ALGORITHM_AUDIO_G711U             = 133,  /* G.711 Ulaw(解码后为,1通道,8000Hz采样率,16-bit的PCM数据) */
    MSSTD_ALGORITHM_AUDIO_MP3               = 134,  /* MP3 */
    MSSTD_ALGORITHM_AUDIO_AAC               = 135,  /* AAC */
    MSSTD_ALGORITHM_AUDIO_MAX               = 256
} MSSTD_ALGORITHM_TYPE_E;

/* 帧类型 */
typedef enum
{
    MSSTD_STREAM_FRAME_AUDIO    = 0,    /* 音频帧 */
    MSSTD_STREAM_FRAME_VIDEO_I  = 1,    /* 视频I帧 */
    MSSTD_STREAM_FRAME_VIDEO_P  = 2,    /* 视频P帧 */
    MSSTD_STREAM_FRAME_VIDEO_B  = 3,    /* 视频B帧 */
    MSSTD_STREAM_FRAME_VIDEO_E  = 4,    /* 视频E帧 */
} MSSTD_STREAM_FRAME_TYPE_E;

/* 帧次类型 */
typedef enum
{
    MSSTD_STREAM_FRAME_IFRM_REAL    = 0,    /* 视频真实I帧 */
    MSSTD_STREAM_FRAME_IFRM_VIRT    = 1,    /* 视频虚拟I帧 */
    MSSTD_STREAM_FRAME_PFRM_REAL    = 2,    /* 视频真实P帧 */
} MSSTD_STREAM_FRAME_IFRM_TYPE_E;

#define ISMSSTD(fd)         (MSSTD_FLAG == ((PMSSTD_S)(fd))->flag)
#define ISAUDIOFRAME(fd)    (((PMSSTD_S)(fd))->Algorithm & 0x80)
#define ISVIDEOFRAME(fd)    (0 == (((PMSSTD_S)(fd))->Algorithm & 0x80))
#define ISKEYFRAME(fd)      (MSSTD_STREAM_FRAME_VIDEO_I == ((PMSSTD_S)(fd))->FrameType)
#define ISREALKEYFRAME(fd)  (MSSTD_STREAM_FRAME_IFRM_REAL == ((PMSSTD_S)(fd))->SecFrmType)
#define ISVIRTKEYFRAME(fd)  (MSSTD_STREAM_FRAME_IFRM_VIRT == ((PMSSTD_S)(fd))->SecFrmType)

#define ISVALIDFRAME(fd, s) /* 判断是否是完整的帧 */                                            \
    (ISMSSTD(fd) && (MSSTD_GETMSHSIZE(fd) >= MSSTD_S_LEN) && (MSSTD_GETFRAMESIZE(fd) <= (s)) && \
    (ISVIDEOFRAME(fd) || ISAUDIOFRAME(fd)) && (MSSTD_GETMSDSIZE(fd) > 0))

#define MSSTD_SEGP(fd)  (((POINTER_TYPE)fd) + MSSTD_S_LEN)              /* 获取媒体SEG段指针 */
#define MSSTD_DATAP(fd) (((POINTER_TYPE)fd) + MSSTD_GETMSHSIZE(fd))     /* 获取媒体数据指针 */
#define MSSTD_NEXTP(fd) (((POINTER_TYPE)fd) + MSSTD_GETFRAMESIZE(fd))   /* 获取下一个媒体头指针 */

#define MSSTD_SEGP_OFFSET(fd)   MSSTD_S_LEN /* 获取媒体OSD段指针偏移大小 */

#define MSSTD_GET_FCOUNT(fd)    (((PMSSTD_S)(fd))->FrmCnt)

#define MSSTD_GET_WIDTH(fd)     (((PMSSTD_S)(fd))->width)
#define MSSTD_SET_WIDTH(fd, w)  (((PMSSTD_S)(fd))->width = w)

#define MSSTD_GET_HEIGHT(fd)    (((PMSSTD_S)(fd))->height)
#define MSSTD_SET_HEIGHT(fd, h) (((PMSSTD_S)(fd))->height = h)

#define MSSTD_GETMSHSIZE(fd)    (((PMSSTD_S)(fd))->MsHSize) /* 获取媒体头大小 */
#define MSSTD_GETMSDSIZE(fd)    (((PMSSTD_S)(fd))->MsDSize) /* 获取媒体数据大小(不包含媒体头大小) */

/* 获取媒体数据帧大小(包含媒体头大小) */
#define MSSTD_GETFRAMESIZE(fd)  (MSSTD_GETMSHSIZE(fd) + MSSTD_GETMSDSIZE(fd))

#define MSSTD_GET_MAX_MSHSIZE(fd)   (((PMSSTD_S)(fd))->MaxMsHSize) /* 获取媒体头最大支持大小 */

#define MSSTD_GET_TIMESTAMP_SEC(fd)     (((PMSSTD_S)(fd))->TimeSec)
#define MSSTD_SET_TIMESTAMP_SEC(fd, s)  (((PMSSTD_S)(fd))->TimeSec = s)

#define MSSTD_GET_TIMESTAMP_MSEC(fd)    (((PMSSTD_S)(fd))->TimeMSec)
#define MSSTD_SET_TIMESTAMP_MSEC(fd, m) (((PMSSTD_S)(fd))->TimeMSec = m)

#define MSSTD_GET_TIMESTAMP(fd, s, m)   \
    (((s) = MSSTD_GET_TIMESTAMP_SEC(fd)), ((m) = MSSTD_GET_TIMESTAMP_MSEC(fd)))
#define MSSTD_SET_TIMESTAMP(fd, s, m)   \
    (MSSTD_SET_TIMESTAMP_SEC(fd, s), MSSTD_SET_TIMESTAMP_MSEC(fd, m))

/* 设置媒体头最大支持大小 */
#define MSSTD_SET_MAX_MSHSIZE(fd, s)                                            \
    MUX(((s) > MSSTD_MAX_HEAD_SIZE), (fd)->MaxMsHSize = MSSTD_MAX_HEAD_SIZE,   \
        (fd)->MaxMsHSize = ALIGN_N(s, MSSTD_HEAD_ALIGN_SIZE))

/* 设置MSSTD中mshsize大小,如果超过最大值按最大值处理 */
#define MSSTD_SET_MSHSIZE_MAX(fd, s)                                        \
    MUX(((s) > MSSTD_MAX_HEAD_SIZE), (fd)->MsHSize = MSSTD_MAX_HEAD_SIZE,   \
        (fd)->MsHSize = ALIGN_N(s, MSSTD_HEAD_ALIGN_SIZE))

/* 设置MSSTD中mshsize大小,如果超过最大值按错误处理 */
#define MSSTD_SET_MSHSIZE_ERR(fd, s)                                        \
    MUX(((s) > MSSTD_MAX_HEAD_SIZE), ERRNO(COMMON_ERROR_PARAM, COMP_MSSTD), \
        (fd)->MsHSize = ALIGN_N(s, MSSTD_HEAD_ALIGN_SIZE))

/* 设置MSSTD中msdsize大小,如果超过最大值按最大值处理,H265不按照4字节对其 */
#define MSSTD_SET_MSDSIZE_MAX(fd, s)                                        \
    MUX(((s) > MSSTD_MAX_DATA_SIZE), (fd)->MsDSize = MSSTD_MAX_DATA_SIZE,   \
        MUX((MSSTD_ALGORITHM_VIDEO_H265_HISILICON == (fd)->Algorithm) ||    \
            (MSSTD_ALGORITHM_VIDEO_H265_GENERAL == (fd)->Algorithm),        \
            (fd)->MsDSize = s, (fd)->MsDSize = ALIGN_N(s, MSSTD_DATA_ALIGN_SIZE)))

/* 设置MSSTD中msdsize大小,如果超过最大值按错误处理,H265不按照4字节对其 */
#define MSSTD_SET_MSDSIZE_ERR(fd, s)                                        \
    MUX(((s) > MSSTD_MAX_DATA_SIZE), ERRNO(COMMON_ERROR_PARAM, COMP_MSSTD), \
        MUX((MSSTD_ALGORITHM_VIDEO_H265_HISILICON == (fd)->Algorithm) ||    \
            (MSSTD_ALGORITHM_VIDEO_H265_GENERAL == (fd)->Algorithm),        \
            (fd)->MsDSize = s, (fd)->MsDSize = ALIGN_N(s, MSSTD_DATA_ALIGN_SIZE)))

/* 段类型 */
typedef enum
{
    MSSTD_SEG_CUSTOM_DEFINE = 0,    /* 自定义类型,实现data透传,具体结构自行解析 */
    MSSTD_SEG_RESERVE_0     = 1,    /* 保留类型 */
    MSSTD_SEG_RESERVE_1     = 2,    /* 保留类型 */
    MSSTD_SEG_MVBLK_TBL     = 3,    /* 移动侦测分块位表信息 */
    MSSTD_SEG_ENCYPT_INFO   = 4,    /* 加密信息(MSSTD_ENCYPT_DATA_S) */
    MSSTD_SEG_PICTURE_INFO  = 5,    /* 图片信息(MSSTD_SEG_PICTURE_S) */
    MSSTD_SEG_MULTI_INFO    = 6,    /* 复合通道信息(MSSTD_MULTI_S) */
    MSSTD_SEG_FACESNAP      = 7,    /* 人脸抓拍数据信息(MSSTD_SEG_FACESNAP_S) */
    MSSTD_SEG_PTS           = 8,    /* PTS数据信息(UINT64) */
    MSSTD_SEG_MAX           = 31    /* 占用存储空间5位,最大值为31 */
} MSSTD_SEG_TYPE_E;

/* 移动侦测分块位表信息 */
typedef struct
{
    UINT8 row;      /* 移动侦测区域行 */
    UINT8 col;      /* 移动侦测区域列 */
    UINT16 size;    /* MdArea大小(根据平台客户端定义最大支持2047字节) */
    char MdArea[];  /* 移动侦测区域移动状态(按位表示) */
} MSSTD_MVBLK_S, *PMSSTD_MVBLK_S;
#define MSSTD_MVBLK_S_LEN   sizeof(MSSTD_MVBLK_S)
ASSERT_SIZEOF_TYPEDEF_STRUCT(MSSTD_MVBLK_S, 4);

#if 0
/* 加密类型 */
typedef enum
{
    ENCRYPT_TYPE_NONE = 0,  /* 未加密 */
    ENCRYPT_TYPE_BLOWFISH,  /* BLOWFISH加解密 */
    ENCRYPT_TYPE_RC4,       /* RC4加解密 */
    ENCRYPT_TYPE_MD5,       /* MD5加密 */
    ENCRYPT_TYPE_BASE64,    /* BASE64加解密 */
    ENCRYPT_TYPE_SMS4,      /* SMS4加解密 */
} ENCRYPT_TYPE_E;
#endif

/* 数据加密类型 */
typedef enum
{
    DATA_ENCRYPT_NONE   = 0x00,     /* 未加密 */
    DATA_ENCRYPT_VIDEO  = BIT32(0), /* 视频加密 */
    DATA_ENCRYPT_AUDIO  = BIT32(1), /* 音频加密 */
    DATA_ENCRYPT_ALL    = (DATA_ENCRYPT_VIDEO | DATA_ENCRYPT_AUDIO)
} DATA_ENCRYPT_STAT_E;

/* MSSTD_SEG_ENCYPT_INFO加密段数据区扩展信息 */
typedef struct
{
    UINT8 PwdEncryptType;   /* 密码加密类型(ENCRYPT_TYPE_E) */
    UINT8 PwdEncryptLen;    /* 加密后的密码大小 */
    UINT8 DataEncryptType;  /* 数据加密类型(ENCRYPT_TYPE_E) */
    UINT8 DataEncryptStat;  /* 数据加密类型(DATA_ENCRYPT_STAT_E) */
    UINT32 DataEncryptSize; /* 数据加密大小 */
    char data[];            /* 保留(便于后续扩展) */
} MSSTD_ENCYPT_DATA_S, *PMSSTD_ENCYPT_DATA_S;
#define MSSTD_ENCYPT_DATA_S_LEN sizeof(MSSTD_ENCYPT_DATA_S)
ASSERT_SIZEOF_TYPEDEF_STRUCT(MSSTD_ENCYPT_DATA_S, 8);

/* 图片帧头信息 */
typedef struct
{
    UINT8 Chn;
    UINT8 VencSnap; /* 抓拍类型(VENC_SNAP_TYPT_E) */
    UINT8 Resvd[2]; /* 保留 */
} MSSTD_PICTURE_S, *PMSSTD_PICTURE_S;
#define MSSTD_PICTURE_S_LEN sizeof(MSSTD_PICTURE_S)
ASSERT_SIZEOF_TYPEDEF_STRUCT(MSSTD_PICTURE_S, 4);

/* 复合通道信息 */
typedef struct
{
    UINT8 Screen;       /* 复合通道分割方式(VO_SPLIT_E) */
    UINT8 Resvd[3];     /* 保留位 */
    UINT8 SplitIdx[];   /* 多画面时,每个位置显示的图像通道(从0开始) */
} MSSTD_MULTI_SCREEN_S, *PMSSTD_MULTI_SCREEN_S;
#define MSSTD_MULTI_SCREEN_S_LEN    sizeof(MSSTD_MULTI_SCREEN_S)
ASSERT_SIZEOF_TYPEDEF_STRUCT(MSSTD_MULTI_SCREEN_S, 4);

#define MSSTD_FACE_FEATURE_LEN  512

/* 人脸性别信息类型 */
typedef enum
{
    MSSTD_FACE_GENDER_UNKNOWN,  /* 未知 */
    MSSTD_FACE_GENDER_MALE,     /* 男性 */
    MSSTD_FACE_GENDER_FEMALE,   /* 女性 */
    MSSTD_FACE_GENDER_BUTT
} MSSTD_FACE_GENDER_E;

/* 人脸少数民族信息类型 */
typedef enum
{
    MSSTD_FACE_MINORITY_UNKNOWN,    /* 未知 */
    MSSTD_FACE_MINORITY_NOT,        /* 非少数名族 */
    MSSTD_FACE_MINORITY_YES,        /* 少数名族 */
    MSSTD_FACE_MINORITY_BUTT
} MSSTD_FACE_MINORITY_E;

/* 人脸抓拍段信息 */
typedef struct
{
    UINT32 size;        /* 图片大小 */
    UINT32 data_offset; /* 相对data起始位置的偏移量 */
    SINT32 face_idx;    /* 比对成功的底库人脸索引 */

    float yaw;          /* 左右旋转角度(正脸侧脸)[-90(左), 90(右)] */
    float pitch;        /* 抬头低头角度(俯仰角)[-90(上), 90(下)] */
    float roll;         /* 左右倾斜角度 [-180(逆时针), 180(顺时针)] */
    float blur;         /* 人脸模糊度(0-表示最清晰,值越大越模糊) */

    UINT16 width;       
    UINT16 height;      /* 人脸图宽高 */

    UINT8 threshold;    /* 人脸质量评分值[0 - 100] */
    SINT8 feature_stat; /* 人脸feature状态(包含未提取(MV_FALSE),提取失败(MV_INVALID),提取成功(MV_TRUE)) */
    UINT8 gender;       /* 性别(MSSTD_FACE_GENDER_E) */
    UINT8 age;          /* 年龄 */

    UINT8 glass;        /* 眼镜 */
    UINT8 mask;         /* 口罩 */
    UINT8 minority;     /* 少数民族(MSSTD_FACE_MINORITY_E) */
    UINT8 verify;       /* 是否比对成功 */

    float score;        /* 比对成功时的比分 */
    SINT32 track_id;    /* 人脸ID */
    UINT8 Resvd[16];    /* 保留位(未使用时填0) */ 
} MSSTD_FACE_CROP_S, *PMSSTD_FACE_CROP_S;
#define MSSTD_FACE_CROP_S_LEN   sizeof(MSSTD_FACE_CROP_S)
ASSERT_SIZEOF_TYPEDEF_STRUCT(MSSTD_FACE_CROP_S, 64);

typedef struct
{
    UINT16 face_cnt;    /* 单张背景图(大图)中人脸个数 */
    UINT16 feature_len; /* 人脸图特征值长度 */    
    UINT32 size;        /* 背景图(大图)大小 */
    char data[];        /* 段数据 */
} MSSTD_SEG_FACESNAP_S, *PMSSTD_SEG_FACESNAP_S;
#define MSSTD_SEG_FACESNAP_S_LEN sizeof(MSSTD_SEG_FACESNAP_S)
ASSERT_SIZEOF_TYPEDEF_STRUCT(MSSTD_SEG_FACESNAP_S, 8);

/* MSSTD段信息 */
typedef struct
{
    UINT32 size: 16;    /* 段大小,以字节为单位,取值范围[sizeof(MSSTD_SEG_S), 64k),
                         * 包括联合体u中扩展内容的长度,当扩展数据为字符串时,包括结束符'\0' 
                         * 注意:总段大小最大64k,单个最大也是近64k,使用时需合理规划段数和大小 */
    UINT32 type: 5;     /* 段类型,指定data数据的类型(MSSTD_SEG_TYPE_E),
                         * 当段类型是MSSTD_SEG_CUSTOM_DEFINE时,实现透传,数据结构自行解析
                         * 当段类型是MSSTD_SEG_MVBLK_TBL时,MSSTD_MVBLK_S有效 
                         * 当段类型是MSSTD_SEG_FACESNAP时,MSSTD_SEG_FACESNAP_S有效 */
    UINT32 Resvd: 11;   /* 保留 */
    char data[];
} MSSTD_SEG_S, *PMSSTD_SEG_S;
#define MSSTD_SEG_S_LEN sizeof(MSSTD_SEG_S)
ASSERT_SIZEOF_TYPEDEF_STRUCT(MSSTD_SEG_S, 4);

/* 流媒体标准头结构体信息(单位:字节,标准大小32字节,可宽展,要求4字节对齐) */
typedef struct
{
    UINT32 flag;        /* 流媒体标准头结构体信息标志(MSSTD_FLAG) */
    UINT32 MsDSize;     /* 媒体数据流大小,以字节为单位(MSSTD_MAX_DATA_SIZE) */
    UINT16 MsHSize;     /* 媒体头信息大小,以字节为单位(MAX size MSSTD_MAX_HEAD_SIZE) */
    UINT16 MaxMsHSize;  /* 媒体头信息最大长度,以字节为单位,其中包含MSSTD结构体长度 */
    UINT32 FrmCnt;      /* 用于码流的帧计数 */
    UINT16 width;       /* 如为视频,则表示宽度(单位1像素);如为音频,则表示采样位宽(单位bit) */
    UINT16 height;      /* 如为视频,则表示高度(单位1像素);如为音频,则表示采样率(单位K[1000]) */
    UINT32 TimeSec;     /* 当前帧时间(单位1秒);表示从公元1970年1月1日0时0分0秒算起至今的UTC时间所经过的秒数 */
    UINT16 TimeMSec;    /* 当前帧时间(单位1毫秒) */
    UINT8 Algorithm;    /* 音频或视频编码算法(MSSTD_ALGORITHM_TYPE_E) */
    UINT8 Segment;      /* MSSTD_SEG_S结构体的数量,取值范围[0, 255] */
    UINT8 FrateRate;    /* 帧率,以赫兹(Hz)为单位,取值范围[1, 255] */
    UINT8 FrameType;    /* 媒体帧类型(MSSTD_STREAM_FRAME_TYPE_E) */
    UINT8 SecFrmType;   /* 媒体帧次类型(MSSTD中FrameType为MSSTD_STREAM_FRAME_VIDEO_I时,
                         * SecFrmType为MSSTD_STREAM_FRAME_IFRM_TYPE_E有效；smart p模式下，
                         * 补充P帧实际的功能类型(虚拟I帧和普通P帧)) */
    UINT8 Resvd;        /* 保留位 */
    SINT8 data[];       /* 紧跟着为段信息或者媒体数据 */
} MSSTD_S, *PMSSTD_S;
#define MSSTD_S_LEN  sizeof(MSSTD_S)
ASSERT_SIZEOF_TYPEDEF_STRUCT(MSSTD_S, 32);

/******************************************************************************
 * 函数介绍: 创建包头,此处将为句柄分配max_mshsize字节内存,
 *           其中包含MSSTD结构体长度,长度函数内部会自动MSSTD_HEAD_ALIGN_SIZE字节对齐
 * 输入参数: Algorithm: 编码标准
 *           MaxMsHSize: 媒体头信息最大长度,大小最大不能超过MSSTD_MAXHEADSIZE
 *           ppMsStd: 存储媒体头信息结构体指针的指针
 * 输出参数: ppMsStd: 存储媒体头信息结构体指针
 * 返回值  : <0-错误代码,0-成功
 *****************************************************************************/
SINT32 MSSTD_Open(SINT32 Algorithm, SINT32 MaxMsHSize, PMSSTD_S *ppMsStd);

/******************************************************************************
 * 函数介绍: 根据已存在的内存地址基础上创建媒体头信息
 * 输入参数: Algorithm: 编码标准
 *           MaxMsHSize: 媒体头信息最大长度,长度要求MSSTD_HEAD_ALIGN_SIZE字节对齐,
 *                       大小最小不能小于MSSTD_LEN,长度超过MSSTD_MAXHEADSIZE部分将被截断
 *           pMsStd: 媒体头信息结构体指针
 * 输出参数: 无
 * 返回值  : <0-错误代码,0-成功
 *****************************************************************************/
SINT32 MSSTD_OpenExt(SINT32 Algorithm, SINT32 MaxMsHSize, PMSSTD_S pMsStd);

/******************************************************************************
 * 函数介绍: 写入帧信息数据,编码数据不进行拷贝
 * 输入参数: pMsStd: MSSTD句柄
 *           FrmType: 帧类型(MSSTD_STREAM_FRAME_TYPE_E)
 *           IFrmType: 帧次类型(MSSTD_STREAM_FRAME_IFRM_TYPE_E)
 *           data: 音视频数据缓冲区(实际未使用)
 *           size: 音视频数据长度
 *           sec: 只对视频有效,当前帧时间的秒数,表示从公元1970年1月1日0时0分0秒算起,
 *                至今的UTC时间所经过的秒数(注意: 当该值为0时,时间戳由内部产生)
 *           msec: 只对视频有效,当前帧时间(单位: 毫秒)
 *           width: 只对视频有效,则表示宽度(单位1像素)
 *           height: 只对视频有效,则表示高度(单位1像素)
 *           FrmRate: 视频帧率,只有视频帧是有效,最大值为MSSTD_MAX_VIDEO_FRAME_RATE
 * 输出参数: 无
 * 返回值  : <0-错误代码,>0-成功,表示写入的数据长度
 *  (注意: 长度不够MSSTD_DATA_ALIGN_SIZE字节对齐将自动加长后对齐,H265不按照MSSTD_DATA_ALIGN_SIZE字节对其)
 *****************************************************************************/
SINT32 MSSTD_WriteExt(PMSSTD_S pMsStd, MSSTD_STREAM_FRAME_TYPE_E FrmType, MSSTD_STREAM_FRAME_IFRM_TYPE_E IFrmType, 
    SINT8 *data, UINT32 size, UINT32 sec, UINT32 msec, UINT32 width, UINT32 height, UINT32 FrmRate);

/******************************************************************************
 * 函数介绍: 写入帧信息数据,编码数据不进行拷贝
 * 输入参数: pMsStd: MSSTD句柄
 *           FrmType: 帧类型对于视频,对于音频无效
 *           IFrmType: 帧次类型(MSSTD_STREAM_FRAME_IFRM_TYPE_E)
 *           data: 音视频数据缓冲区,实际未使用
 *           size: 音视频数据长度
 *           width: 只对视频有效,则表示宽度(单位1像素)
 *           height: 只对视频有效,则表示高度(单位1像素)
 *           FrmRate: 视频帧率,只有视频帧是有效,最大值为MSSTD_MAX_VIDEO_FRAME_RATE
 * 输出参数: 无
 * 返回值  : <0-错误代码,>0-成功,表示写入的数据长度
 *  (注意: 长度不够MSSTD_DATA_ALIGN_SIZE字节对齐将自动加长后对齐,H265不按照MSSTD_DATA_ALIGN_SIZE字节对其)
 *****************************************************************************/
SINT32 MSSTD_Write(PMSSTD_S pMsStd, MSSTD_STREAM_FRAME_TYPE_E FrmType, MSSTD_STREAM_FRAME_IFRM_TYPE_E IFrmType, 
    SINT8 *data, UINT32 size, UINT32 width, UINT32 height, UINT32 FrmRate);

/******************************************************************************
 * 函数介绍: 读取音视频数据,获取数据指针,以及长度
 * 输入参数: pMsStd: MSSTD句柄
 * 输出参数: ppData: 音视频数据缓冲区地址指针
 *           size: 音视频数据长度指针
 * 返回值  : <0-错误代码,0-成功
 *****************************************************************************/
SINT32 MSSTD_Read(PMSSTD_S pMsStd, SINT8 **ppData, UINT32 *pSize);

/******************************************************************************
 * 函数介绍: 写入短信息数据MSSTD_SEG
 * 输入参数: pMsStd: MSSTD句柄
 *           Seg: 段头信息,除size,data其余数据由用户输入
 *           data: 用户数据
 *           size: 用户数据长度,如为字符串应包含尾部'\0'的长度
 * 输出参数: 无
 * 返回值  : <0-错误代码,>0-成功,表示当前包头长度(注意: 长度不够MSSTD_HEAD_ALIGN_SIZE字节对齐将自动加长后对齐)
 *****************************************************************************/
SINT32 MSSTD_Write_Seg(PMSSTD_S pMsStd, MSSTD_SEG_S Seg, SINT8 *data, UINT32 size);

/******************************************************************************
 * 函数介绍: 获取包头信息中的一个段信息
 * 输入参数: pMsStd: MSSTD句柄
 *           Index: MSSTD_SEG段索引序号
 * 返回值: <0-错误代码,0-句柄中无可用信息,>0-则指向获取的段信息指针PMSSTD_SEG
 *****************************************************************************/
PMSSTD_SEG_S MSSTD_Read_Seg(PMSSTD_S pMsStd, UINT32 Index);

/******************************************************************************
 * 函数介绍: MSSTD复位当前句柄
 * 输入参数: pMsStd: MSSTD句柄
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 MSSTD_Reset(PMSSTD_S pMsStd);

/******************************************************************************
 * 函数介绍: 关闭MSSTD,该函数将清除句柄所有数据,并释放相关资源
 * 输入参数: pMsStd: MSSTD句柄
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 MSSTD_Close(PMSSTD_S pMsStd);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* _MSSTD_H_ */
