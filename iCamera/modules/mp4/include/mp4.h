#ifndef __MP4_H__
#define __MP4_H__

#include <stdint.h>

/* 帧类型 */
typedef enum {
    FRM_VIDEO_P = 0, /* P  */
    FRM_VIDEO_I = 1, /* I  */
    FRM_VIDEO_B = 2, /* B  */
    FRM_AUDIO_1 = 3, /* 1  */
    FRM_BUTT,
}E_FRM_TYPE;

/* 编码类型 */
typedef enum {
    ENC_H264  = 0,
    ENC_H265  = 1,
    ENC_MJPEG = 2,
    ENC_JPEG  = 3,
    ENC_MPEG4 = 4,
    ENC_ADPCM = 5,
    ENC_G711A = 6,
    ENC_G711U = 7,
    ENC_G726  = 8,
    ENC_AAC   = 9,
    ENC_BUTT,
}E_ENC_TYPE;

// ISO/IEC 14496-1:2010(E) 7.2.6.6 DecoderConfigDescriptor (p48)
// MPEG-4 systems ObjectTypeIndication
// http://www.mp4ra.org/object.html
typedef enum {
    MP4_OBJECT_TEXT     = 0x08, // Text Stream
    MP4_OBJECT_MP4V     = 0x20, // Visual ISO/IEC 14496-2 (c)
    MP4_OBJECT_H264     = 0x21, // Visual ITU-T Recommendation H.264 | ISO/IEC 14496-10 
    MP4_OBJECT_HEVC     = 0x23, // Visual ISO/IEC 23008-2 | ITU-T Recommendation H.265
    MP4_OBJECT_AAC      = 0x40, // Audio ISO/IEC 14496-3
    MP4_OBJECT_MP2V     = 0x60, // Visual ISO/IEC 13818-2 Simple Profile
    MP4_OBJECT_AAC_MAIN = 0x66, // MPEG-2 AAC Main
    MP4_OBJECT_AAC_LOW  = 0x67, // MPEG-2 AAC Low
    MP4_OBJECT_AAC_SSR  = 0x68, // MPEG-2 AAC SSR
    MP4_OBJECT_MP3      = 0x69, // Audio ISO/IEC 13818-3
    MP4_OBJECT_MP1V     = 0x6A, // Visual ISO/IEC 11172-2
    MP4_OBJECT_MP1A     = 0x6B, // Audio ISO/IEC 11172-3
    MP4_OBJECT_JPEG     = 0x6C, // Visual ISO/IEC 10918-1 (JPEG)
    MP4_OBJECT_PNG      = 0x6D, // Portable Network Graphics (f)
    MP4_OBJECT_JPEG2000 = 0x6E, // Visual ISO/IEC 15444-1 (JPEG 2000)
    MP4_OBJECT_VC1      = 0xA3, // SMPTE VC-1 Video
    MP4_OBJECT_DIRAC    = 0xA4, // Dirac Video Coder
    MP4_OBJECT_AC3      = 0xA5, // AC-3
    MP4_OBJECT_EAC3     = 0xA5, // Enhanced AC-3
    MP4_OBJECT_G719     = 0xA8, // ITU G.719 Audio
    MP4_OBJECT_DTS      = 0xA9, // Core Substream
    MP4_OBJECT_OPUS     = 0xAD, // Opus audio
    MP4_OBJECT_VP9      = 0xB1, // VP9 Video
    MP4_OBJECT_FLAC     = 0xC1, // nonstandard from FFMPEG
    MP4_OBJECT_VP8      = 0xC2, // nonstandard
    MP4_OBJECT_G711a    = 0xFD, // ITU G.711 alaw
    MP4_OBJECT_G711u    = 0xFE, // ITU G.711 ulaw
    MP4_OBJECT_AV1      = 0xFF, // AV1: https://aomediacodec.github.io/av1-isobmff
}E_MP4_OBJ;

// MP4 flags
#define MP4_FLAG_FASTSTART (0x00000001)
#define MP4_FLAG_SEGMENT    (0x00000002) // fmp4_writer only

// MP4 av stream flag
#define MP4_AV_FLAG_KEYFREAME (0x0001)

typedef struct rec_v_info_s
{
    int w;      // 宽度
    int h;      // 高度
    int fps;    // 帧率
}rec_v_info_t;

typedef struct rec_a_info_s
{
    int sp;     // 采样率
    int ss;     // 帧长度
    int chs;    // 通道数
}rec_a_info_t;

typedef struct rec_rw_info_s
{
    E_FRM_TYPE type;
    E_ENC_TYPE enc;
    long time;       // localtime sec
    int64_t pts_ms;  // pts ms
    int64_t dts_ms;  // dts ms
    union {
        rec_v_info_t v;
        rec_a_info_t a;
    };
}rec_rw_info_t;

typedef struct _rec_media_info_s
{
    E_ENC_TYPE aenc;
    E_ENC_TYPE venc;
    rec_v_info_t v;
    rec_a_info_t a;
}rec_media_info_t;

#pragma pack(1)
typedef struct mp4_box_s
{
    uint32_t len;
    int8_t name[4];
    uint32_t offset;
}mp4_box_t;
#pragma pack()

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************MP4写操作接口*****************************************************/

struct _TMp4FileWriteOpsPriv;
typedef struct _TMp4FileWriteOps
{
    struct _TMp4FileWriteOpsPriv *Priv;

    /*@ breif Mp4WriteOpen 打开需要写的MP4视频，不存在则创建
     *@ param [in] This 句柄
     *@ param [in] name MP4录像文件
     *@ param [in] b_fmp4 是否为FMP4格式 0:标准MP4格式 1:FMP4格式
     *@ param [in] b_raw_mov_data 写入的数据是否是纯的MP4数据流(用作修复MP4使用)
     *@ param [in] b_fast_start 是否是快速MP4模式(moov提前，仅标准MP4有效)
     *@ return 成功返回0，失败返回-1
     */
    int (*Mp4WriteOpen)(struct _TMp4FileWriteOps *This, const char *name, int b_fmp4, int b_raw_mov_data, int b_fast_start);

    /*@ breif Mp4WriteClose 关闭写录像句柄
     *@ param [in] This 句柄
     *@ return 成功返回0，失败返回-1
     */
    int (*Mp4WriteClose)(struct _TMp4FileWriteOps *This);

    /*@ breif Mp4WriteFlush 将数据刷新到磁盘上(暂未实现)
     *@ param [in] This 句柄
     *@ return 成功返回0，失败返回-1
     */
    int (*Mp4WriteFlush)(struct _TMp4FileWriteOps *This);

    /*@ breif Mp4WriteFrame 写一帧视频数据
     *@ param [in] This 句柄
     *@ param [in] buf 纯流数据buffer
     *@ param [in] size 流数据buffer大小
     *@ param [in] info 流信息
     *@ return 成功返回0，失败返回-1
     */
    int (*Mp4WriteFrame)(struct _TMp4FileWriteOps *This, const char *buf, int size, const rec_rw_info_t *info);

    /*@ breif Mp4WriteGetExtraData 获取文件的EXTRA数据(用于MP4的修复)
     *@ param [in] This 句柄
     *@ param [in] b_video 是否为视频的EXTRA, 1:video 0:audio
     *@ param [out] extra_data EXTRA的buffer
     *@ param [in] extra_data_size EXTRA的buffer大小
     *@ return 成功返回0，失败返回-1
     */
    int (*Mp4WriteGetExtraData)(struct _TMp4FileWriteOps *This, int b_video, uint8_t* extra_data, uint32_t extra_data_size);

    /*@ breif Mp4WriteSetExtraData 设置文件的EXTRA数据(用于修复MP4的extra数据)
     *@ param [in] This 句柄
     *@ param [in] b_video 是否为视频的EXTRA, 1:video 0:audio
     *@ param [in] extra_data EXTRA的buffer
     *@ param [in] extra_data_size EXTRA的buffer大小
     *@ return 成功返回0，失败返回-1
     */
    int (*Mp4WriteSetExtraData)(struct _TMp4FileWriteOps *This, int b_video, const uint8_t* extra_data, uint32_t extra_data_size);

    /*@ breif Mp4WriteSubtitle MP4文件添加字幕
     *@ param [in] This 句柄
     *@ param [in] object 字幕对象，详见E_MP4_OBJ
     *@ param [in] object 字幕对象buffer
     *@ param [in] object 字幕对象buffer大小
     *@ return 成功返回添加的track数，失败返回-1
     */
    int (*Mp4WriteSubtitle)(struct _TMp4FileWriteOps *This, uint8_t object, const void* extra_data, size_t extra_data_size);

    /*@ breif Mp4WriteCover MP4文件添加封面(图片只支持PNG和JPG)
     *@ param [in] This 句柄
     *@ param [in] cover_file 图片文件
     *@ return 成功返回0，失败返回-1
     */
    int (*Mp4WriteCover)(struct _TMp4FileWriteOps *This, const char *cover_file);

    /*@ breif Mp4WriteGetFileSize 获取文件大小
     *@ param [in] This 句柄
     *@ return 成功返回大小，失败返回-1
     */
    int (*Mp4WriteGetFileSize)(struct _TMp4FileWriteOps *This);
}TMp4FileWriteOps;

/*@ breif Mp4FileWriteOpsCreate 创建写MP4相关操作句柄
 *@ return 成功返回操作句柄，失败返回NULL
 */
TMp4FileWriteOps* Mp4FileWriteOpsCreate(void);

/*@ breif Mp4FileWriteOpsDestroy 销毁MP4写操作句柄
 *@ return 成功返回0，失败返回-1
 */
int Mp4FileWriteOpsDestroy(TMp4FileWriteOps* This);

/*****************************************************MP4读操作接口*****************************************************/

struct _TMp4FileReadOpsPriv;
typedef struct _TMp4FileReadOps
{
    struct _TMp4FileReadOpsPriv *Priv;

    /*@ breif Mp4ReadOpen 打开一个MP4文件，用作回放
     *@ param [in] This 句柄
     *@ param [in] name MP4文件
     *@ param [in] type 用作扩展视频文件格式(暂未用到)
     *@ param [out] media 当前MP4录像文件的信息
     *@ return 成功返回0，失败返回-1
     */
    int (*Mp4ReadOpen)(struct _TMp4FileReadOps *This, const char *name, int type, rec_media_info_t *media);

    /*@ breif Mp4ReadClose 关闭读录像句柄
     *@ param [in] This 读录像句柄
     *@ return 成功返回0，失败返回-1
     */
    int (*Mp4ReadClose)(struct _TMp4FileReadOps *This);

    /*@ breif Mp4ReadFrame 读一帧视频数据
     *@ param [in] This 读录像句柄
     *@ param [out] buf 获取到的一帧数据
     *@ param [in] size 当前buffer的大小
     *@ param [out] get_size 获取到的实际buffer大小
     *@ param [out] info 当前帧数据的信息
     *@ return 成功返回0，错误返回其他
     */
    int (*Mp4ReadFrame)(struct _TMp4FileReadOps *This, char *buf, int buf_size, int *get_size, rec_rw_info_t *info);

    /*@ breif Mp4ReadGetFileSize 获取文件大小
     *@ param [in] This 读录像句柄
     *@ return 成功返回大小，失败返回-1
     */
    int (*Mp4ReadGetFileSize)(struct _TMp4FileReadOps *This);

    /*@ breif Mp4ReadSeekSec 进行seek定位(单位: 秒)
     *@ param [in] This 读录像句柄
     *@ param [in] seek_sec seek的秒数
     *@ param [out] get_seek_sec 获取到实际定位的时间点
     *@ return 成功返回0，失败返回-1
     *@ note 由于MP4的定位，只能用相对时间定位，故传入的时间不能超过整个录像的时间
     */
    int (*Mp4ReadSeekSec)(struct _TMp4FileReadOps *This, int seek_sec, int *get_seek_sec);

	/*@ breif Mp4ReadSeekMSec 进行seek定位(单位: 毫秒)
     *@ param [in] This 读录像句柄
     *@ param [in] seek_msec seek的毫秒数
     *@ param [out] get_seek_msec 获取到实际定位的时间点
     *@ return 成功返回0，失败返回-1
     *@ note 由于MP4的定位，只能用相对时间定位，故传入的时间不能超过整个录像的时间
     */
    int (*Mp4ReadSeekMSec)(struct _TMp4FileReadOps *This, int seek_msec, int *get_seek_msec);

    /*@ breif Mp4ReadGetDurationMs 获取当前MP4录像的实际录像时间(单位:毫秒)
     *@ param [in] This 读录像句柄
     *@ return 成功返回当前录像文件的时长(毫秒)，失败返回-1
     */
    int (*Mp4ReadGetDurationMs)(struct _TMp4FileReadOps *This);

    /*@ breif Mp4ReadGetSampleCount 获取当前MP4录像的帧数
     *@ param [in] This 读录像句柄
     *@ return 成功返回当前录像文件帧数，失败返回-1
     */
    int (*Mp4ReadGetSampleCount)(struct _TMp4FileReadOps *This);

    /*@ breif Mp4ReadFindContentBox 寻找MP4的BOX
     *@ param [in] This 句柄
     *@ param [in] mp4_file MP4文件 
     *@ param [in/out] box box信息(传入box里面的name信息，获取偏移和长度)
     *@ return 成功返回1，失败返回-1
     */
    int (*Mp4ReadFindContentBox)(struct _TMp4FileReadOps *This, const char *mp4_file, mp4_box_t *box);
}TMp4FileReadOps;

/*@ breif Mp4FileReadOpsCreate 创建读MP4相关操作句柄
 *@ return 成功返回操作句柄，失败返回NULL
 */
TMp4FileReadOps* Mp4FileReadOpsCreate(void);

/*@ breif Mp4FileReadOpsDestroy 销毁MP4读操作句柄
 *@ return 成功返回0，失败返回-1
 */
int Mp4FileReadOpsDestroy(TMp4FileReadOps* This);

/*************************************************MP4修复接口(仅支持标准MP4)*************************************************/

struct _TMp4FileRebuildOpsPriv;
typedef struct _TMp4FileRebuildOps
{
    struct _TMp4FileRebuildOpsPriv *Priv;

    /*@ breif Mp4RebuildOpen 打开一个需要修复(重建的)MP4文件
     *@ param [in] This 句柄
     *@ param [in] name MP4文件 
     *@ param [in] flag 标志位
     *@ return 成功返回0     失败返回-1
     */
    int (*Mp4RebuildOpen)(struct _TMp4FileRebuildOps *This, const char *name, int flag);

    /*@ breif Mp4RebuildClose 关闭一个需要修复(重建的)MP4文件
     *@ param [in] This 句柄
     *@ return 成功返回0，失败-1
     */
    int (*Mp4RebuildClose)(struct _TMp4FileRebuildOps *This);

    /*@ breif Mp4RebuildAddAudio 添加音频track */
    int (*Mp4RebuildAddAudio)(struct _TMp4FileRebuildOps *This, uint8_t object, int channel_count, int bits_per_sample, int sample_rate, const void* extra_data, size_t extra_data_size);

    /*@ breif Mp4RebuildAddVideo 添加视频track */
    int (*Mp4RebuildAddVideo)(struct _TMp4FileRebuildOps *This, uint8_t object, int width, int height, const void* extra_data, size_t extra_data_size);

    /*@ breif Mp4RebuildAddSubtitle 添加字幕 */
    int (*Mp4RebuildAddSubtitle)(struct _TMp4FileRebuildOps *This, uint8_t object, const void* extra_data, size_t extra_data_size);

    /*@ breif Mp4RebuildWriteFrameNoData 写帧数据(数据为空) */
    int (*Mp4RebuildWriteFrameNoData)(struct _TMp4FileRebuildOps *This, int track, size_t data_bytes, int64_t pts, int64_t dts, int flags);
}TMp4FileRebuildOps;

/*@ breif Mp4FileRebuildOpsCreate 创建修复MP4相关操作句柄
 *@ return 成功返回操作句柄，失败返回NULL
 */
TMp4FileRebuildOps* Mp4FileRebuildOpsCreate(void);

/*@ breif Mp4FileRebuildOpsDestroy 销毁MP4修复操作句柄
 *@ return 成功返回0，失败返回-1
 */
int Mp4FileRebuildOpsDestroy(TMp4FileRebuildOps* This);

#ifdef __cplusplus
}
#endif

#endif //__MP4_H__
