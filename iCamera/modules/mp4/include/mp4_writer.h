#ifndef __MP4_WRITER_H__
#define __MP4_WRITER_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct MP4Writer MP4Writer;

typedef struct
{
    int          width;      /* 编码宽度，0=从 SPS 自动解析 */
    int          height;     /* 编码高度，0=从 SPS 自动解析 */
    int          fps;        /* 标称帧率，用于异常时间戳兜底，默认 25 */
    unsigned int start_time; /* 首帧绝对时间(秒)，0=使用当前系统时间 */
} MP4WriterInfo;

typedef struct
{
    int max_samples;      /* 单分片最多缓存帧数，默认 30 */
    int max_bytes;        /* 单分片 mdat 最大字节数，默认 512KB */
    int max_duration_ms;  /* 单分片最大时长，默认 2000ms */
    int sync_interval_ms; /* 强制 fdatasync 周期，默认 2000ms；<=0 表示每片都刷盘 */
} MP4WriterConfig;

/*
 * 创建 MP4 写入句柄。
 * @note 内部使用 fMP4(分片 MP4) 布局：ftyp + moov + 循环(moof + mdat)。
 *       每个分片都是自包含索引，异常掉电后已落盘的分片可以直接播放。
 * @return 成功返回句柄，失败返回 NULL
 */
MP4Writer *MP4Writer_Create(const char *path, const MP4WriterInfo *info,
                            const MP4WriterConfig *cfg);

/*
 * 写入一帧 H.264 Annex-B 裸流（不做二次编码）。
 * @param pts_us   显示时间戳(微秒)
 * @param dts_us   解码时间戳(微秒)
 * @param keyframe 1=I帧，0=非I帧，-1=根据码流自动判断
 * @return 0=成功（首帧前若尚未拿到 SPS/PPS 会丢弃并返回 0），-1=失败
 */
int MP4Writer_WriteFrame(MP4Writer *w, const uint8_t *data, int size,
                         int64_t pts_us, int64_t dts_us, int keyframe);

/* 立即把当前缓存帧写成一个分片并刷盘 */
int MP4Writer_Flush(MP4Writer *w);

/* 结束录像：写完剩余帧、追加 mfra 随机访问索引、fsync 并关闭文件 */
int MP4Writer_Close(MP4Writer *w);

/* 销毁句柄（未 Close 时会自动 Close） */
void MP4Writer_Destroy(MP4Writer *w);

/* 当前文件大小（字节），失败返回 -1 */
int MP4Writer_GetFileSize(MP4Writer *w);

/* 最近一次错误描述，无错误返回空字符串 */
const char *MP4Writer_GetLastError(MP4Writer *w);

#ifdef __cplusplus
}
#endif

#endif /* __MP4_WRITER_H__ */
