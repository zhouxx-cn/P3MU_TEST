#ifndef __MP4_COMMON_H__
#define __MP4_COMMON_H__

#include <stddef.h>
#include <stdint.h>

/*
 * MP4 模块内部公共定义。
 * 说明：本模块不依赖 ffmpeg / libav，也不依赖项目里的 libmov，
 *       全部使用标准 POSIX 接口 + 手写 ISO BMFF 封装/解析。
 */

#define MP4_TIMESCALE                 90000      /* 视频时基，90kHz 是 MP4 常用精度 */
#define MP4_EPOCH_1904_OFFSET         2082844800LL /* 1970->1904 秒偏移，MP4 时间戳从 1904 起算 */

#define MP4_MAX_FRAME_SIZE            (4 * 1024 * 1024) /* 单帧上限，适配 1080P/4Mbps 场景 */
#define MP4_MAX_FRAGMENT_BYTES        (4 * 1024 * 1024) /* 单个分片 mdat 上限 */
#define MP4_MAX_SAMPLES_PER_FRAG      512               /* 单个 moof 内最多 sample 数 */
#define MP4_MAX_MOOV_SIZE             (8 * 1024 * 1024) /* moov 防御性上限 */
#define MP4_MAX_MOOF_SIZE             (1 * 1024 * 1024) /* moof 防御性上限 */
#define MP4_MAX_FRAGMENT_INDEX        65536             /* 分片索引上限，2s/片约可覆盖 36 小时 */

#define MP4_FRAME_TYPE_I              0x01
#define MP4_FRAME_TYPE_P              0x02

/* ISO/IEC 14496-12 sample flags 的常用组合 */
#define MP4_SAMPLE_FLAG_SYNC          0x02000000u /* sample_depends_on=2(I帧)，非 non-sync */
#define MP4_SAMPLE_FLAG_NON_SYNC      0x01010000u /* sample_depends_on=1，non-sync=1 */

#define MP4_FOURCC(a, b, c, d) \
    (((uint32_t)(uint8_t)(a) << 24) | ((uint32_t)(uint8_t)(b) << 16) | \
     ((uint32_t)(uint8_t)(c) << 8)  | ((uint32_t)(uint8_t)(d)))

/* 大端读写：嵌入式 ARM 通常是小端，这里统一按网络字节序手工拼装，避免依赖 htonl */
static inline uint16_t mp4_rd16(const uint8_t *p)
{
    return (uint16_t)(((uint16_t)p[0] << 8) | (uint16_t)p[1]);
}

static inline uint32_t mp4_rd24(const uint8_t *p)
{
    return ((uint32_t)p[0] << 16) | ((uint32_t)p[1] << 8) | (uint32_t)p[2];
}

static inline uint32_t mp4_rd32(const uint8_t *p)
{
    return ((uint32_t)p[0] << 24) | ((uint32_t)p[1] << 16) |
           ((uint32_t)p[2] << 8)  | (uint32_t)p[3];
}

static inline uint64_t mp4_rd64(const uint8_t *p)
{
    return ((uint64_t)mp4_rd32(p) << 32) | (uint64_t)mp4_rd32(p + 4);
}

static inline void mp4_wr16(uint8_t *p, uint16_t v)
{
    p[0] = (uint8_t)(v >> 8);
    p[1] = (uint8_t)v;
}

static inline void mp4_wr24(uint8_t *p, uint32_t v)
{
    p[0] = (uint8_t)(v >> 16);
    p[1] = (uint8_t)(v >> 8);
    p[2] = (uint8_t)v;
}

static inline void mp4_wr32(uint8_t *p, uint32_t v)
{
    p[0] = (uint8_t)(v >> 24);
    p[1] = (uint8_t)(v >> 16);
    p[2] = (uint8_t)(v >> 8);
    p[3] = (uint8_t)v;
}

static inline void mp4_wr64(uint8_t *p, uint64_t v)
{
    mp4_wr32(p, (uint32_t)(v >> 32));
    mp4_wr32(p + 4, (uint32_t)v);
}

/* 可增长的字节缓冲区：用于组装 moov/moof，所有扩容都做溢出检查 */
typedef struct
{
    uint8_t *data;
    size_t   size;
    size_t   cap;
    int      err;
} MP4Buf;

int    MP4Buf_Init(MP4Buf *b, size_t cap);
void   MP4Buf_Free(MP4Buf *b);
int    MP4Buf_Reserve(MP4Buf *b, size_t extra);
int    MP4Buf_Append(MP4Buf *b, const void *data, size_t len);
int    MP4Buf_Put8(MP4Buf *b, uint8_t v);
int    MP4Buf_Put16(MP4Buf *b, uint16_t v);
int    MP4Buf_Put24(MP4Buf *b, uint32_t v);
int    MP4Buf_Put32(MP4Buf *b, uint32_t v);
int    MP4Buf_Put64(MP4Buf *b, uint64_t v);
size_t MP4Buf_BeginBox(MP4Buf *b, const char type[4]);
int    MP4Buf_EndBox(MP4Buf *b, size_t box_start);

#endif /* __MP4_COMMON_H__ */
