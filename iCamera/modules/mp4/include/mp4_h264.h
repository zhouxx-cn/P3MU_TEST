#ifndef __MP4_H264_H__
#define __MP4_H264_H__

#include <stdint.h>

#include "mp4_common.h"

#define MP4_MAX_SPS_COUNT   4
#define MP4_MAX_PPS_COUNT   4
#define MP4_MAX_SPS_SIZE    256
#define MP4_MAX_PPS_SIZE    128

/* H.264 参数集：写入时从码流中提取，存入 avcC；回放时用于补齐关键帧 */
typedef struct
{
    uint8_t sps[MP4_MAX_SPS_COUNT][MP4_MAX_SPS_SIZE];
    int     sps_len[MP4_MAX_SPS_COUNT];
    int     sps_count;

    uint8_t pps[MP4_MAX_PPS_COUNT][MP4_MAX_PPS_SIZE];
    int     pps_len[MP4_MAX_PPS_COUNT];
    int     pps_count;

    int     length_size; /* avcC lengthSizeMinusOne+1，正常为 4 */
} MP4H264Extra;

typedef struct
{
    const uint8_t *data; /* 指向 NAL 负载(含 NAL header) */
    int            size;
} MP4Nal;

/*
 * 遍历 Annex-B 码流。
 * @param pos [IN/OUT] 起始/下一次扫描位置
 * @return 1=找到一个 NAL，0=遍历结束，-1=参数错误
 * @note 同时兼容 3 字节(00 00 01)与 4 字节(00 00 00 01)起始码，
 *       会自动裁掉 NAL 尾部的 0x00 填充。
 */
int MP4_H264IterateNal(const uint8_t *buf, int size, int *pos, MP4Nal *nal);

/*
 * 扫描一帧 Annex-B 数据：提取 SPS/PPS，并判断是否包含 IDR。
 * extra 会被增量更新（相同 SPS/PPS 自动去重）。
 * @return 0=成功，-1=没有找到任何合法 NAL
 */
int MP4_H264Scan(const uint8_t *buf, int size, MP4H264Extra *extra, int *is_key);

/* 从 SPS NAL(含 1 字节 NAL header) 解析宽高。成功返回 0 */
int MP4_H264GetSpsSize(const uint8_t *sps, int sps_len, int *width, int *height);

/* Annex-B -> AVCC(4 字节长度前缀)，用于写入 mdat */
int MP4_H264AvccSize(const uint8_t *in, int in_size, int *out_size, int *nal_count);
int MP4_H264AnnexBToAvcc(const uint8_t *in, int in_size,
                         uint8_t *out, int out_cap, int *out_size);

/* 计算 AVCC -> Annex-B 所需输出大小（关键帧会自动补 SPS/PPS） */
int MP4_H264AvccRequiredSize(const uint8_t *in, int in_size,
                             const MP4H264Extra *extra, int keyframe,
                             int *required);

/* AVCC -> Annex-B，用于回放抛出裸流 */
int MP4_H264AvccToAnnexB(const uint8_t *in, int in_size,
                         const MP4H264Extra *extra, int keyframe,
                         uint8_t *out, int out_cap, int *out_size);

#endif /* __MP4_H264_H__ */
