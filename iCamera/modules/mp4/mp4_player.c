#ifndef _FILE_OFFSET_BITS
#define _FILE_OFFSET_BITS 64
#endif

#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif

#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "mp4_common.h"
#include "mp4_h264.h"
#include "mp4_player.h"

typedef struct
{
    uint64_t offset;    /* sample 数据在文件中的绝对偏移 */
    uint32_t size;      /* sample 字节数 */
    uint64_t dts;       /* 解码时间(90kHz) */
    int64_t  ctts;      /* pts = dts + ctts */
    uint32_t flags;     /* sample flags */
} MP4SampleEntry;

typedef struct
{
    uint64_t moof_offset;
    uint32_t moof_size;
    uint64_t base_dts;
    uint32_t sample_count;
    uint64_t data_end;
    int      first_key;
} MP4Fragment;

struct MP4Player
{
    int      fd;
    char     path[512];
    uint64_t file_size;

    uint32_t timescale;
    uint32_t creation_time;
    int64_t  file_start_sec;
    int      track_id;
    int      width;
    int      height;

    MP4H264Extra extra;

    MP4Fragment *frags;
    int          frag_count;
    int          frag_cap;

    int cur_frag;
    int cur_sample;
    int loaded;
    int sample_count;
    MP4SampleEntry samples[MP4_MAX_SAMPLES_PER_FRAG];

    uint8_t *sample_buf;
    int      sample_cap;
    uint8_t *out_buf;
    int      out_cap;

    char errmsg[160];
};

typedef struct
{
    const uint8_t *data;
    size_t         size;
    size_t         pos;
} MP4Walker;

/* ------------------------------------------------------------------------- */
/* 内存 box 遍历                                                             */
/* ------------------------------------------------------------------------- */

static int walker_next(MP4Walker *w, uint32_t *type, size_t *payload_off,
                       size_t *payload_size)
{
    uint32_t size32 = 0;
    uint64_t box_size = 0;
    size_t hdr = 8;

    if (!w || !type || !payload_off || !payload_size || w->pos + 8 > w->size)
    {
        return 0;
    }

    size32 = mp4_rd32(w->data + w->pos);
    *type  = mp4_rd32(w->data + w->pos + 4);
    box_size = size32;

    if (size32 == 1)
    {
        if (w->pos + 16 > w->size)
        {
            return -1;
        }
        box_size = mp4_rd64(w->data + w->pos + 8);
        hdr = 16;
    }
    else if (size32 == 0)
    {
        box_size = w->size - w->pos;
    }

    if (box_size < hdr || box_size > w->size - w->pos)
    {
        return -1;
    }

    *payload_off  = w->pos + hdr;
    *payload_size = (size_t)box_size - hdr;
    w->pos += (size_t)box_size;

    return 1;
}

/* ------------------------------------------------------------------------- */
/* 文件顶层 box 头读取                                                       */
/* ------------------------------------------------------------------------- */

/* @return 1=成功，0=到文件尾，-1=非法，-2=box 超出文件尾(掉电截断) */
static int read_box_header(int fd, uint64_t off, uint64_t file_size,
                           uint32_t *type, uint64_t *box_size, uint64_t *hdr)
{
    uint8_t h[16];
    uint32_t size32 = 0;
    ssize_t n = 0;

    if (!type || !box_size || !hdr)
    {
        return -1;
    }

    if (off >= file_size || file_size - off < 8)
    {
        return 0;
    }

    n = pread(fd, h, 8, (off_t)off);
    if (n != 8)
    {
        return -1;
    }

    size32 = mp4_rd32(h);
    *type  = mp4_rd32(h + 4);
    *hdr   = 8;
    *box_size = size32;

    if (size32 == 1)
    {
        if (file_size - off < 16)
        {
            return -2;
        }

        n = pread(fd, h + 8, 8, (off_t)(off + 8));
        if (n != 8)
        {
            return -1;
        }

        *box_size = mp4_rd64(h + 8);
        *hdr = 16;
    }
    else if (size32 == 0)
    {
        *box_size = file_size - off;
    }
    else
    {
        /* size32 正常 */
    }

    if (*box_size < *hdr)
    {
        return -1;
    }

    if (*box_size > file_size - off)
    {
        return -2;
    }

    return 1;
}

/* ------------------------------------------------------------------------- */
/* moov 解析                                                                 */
/* ------------------------------------------------------------------------- */

static int parse_avcc(MP4H264Extra *extra, const uint8_t *data, size_t size)
{
    int length_size = 0;
    int num_sps = 0;
    int num_pps = 0;
    int i = 0;
    size_t pos = 0;
    uint32_t len = 0;

    if (!extra || !data || size < 7)
    {
        return -1;
    }

    length_size = (data[4] & 0x03) + 1;
    if (length_size == 3)
    {
        return -1; /* 规范不允许 3 字节长度前缀 */
    }

    num_sps = data[5] & 0x1F;
    pos = 6;

    extra->length_size = length_size;
    extra->sps_count = 0;
    extra->pps_count = 0;

    for (i = 0; i < num_sps; i++)
    {
        if (pos + 2 > size)
        {
            return -1;
        }

        len = mp4_rd16(data + pos);
        pos += 2;

        if (len == 0 || pos + len > size)
        {
            return -1;
        }

        if (extra->sps_count < MP4_MAX_SPS_COUNT && len <= MP4_MAX_SPS_SIZE)
        {
            memcpy(extra->sps[extra->sps_count], data + pos, len);
            extra->sps_len[extra->sps_count] = (int)len;
            extra->sps_count++;
        }

        pos += len;
    }

    if (pos + 1 > size)
    {
        return -1;
    }

    num_pps = data[pos];
    pos++;

    for (i = 0; i < num_pps; i++)
    {
        if (pos + 2 > size)
        {
            return -1;
        }

        len = mp4_rd16(data + pos);
        pos += 2;

        if (len == 0 || pos + len > size)
        {
            return -1;
        }

        if (extra->pps_count < MP4_MAX_PPS_COUNT && len <= MP4_MAX_PPS_SIZE)
        {
            memcpy(extra->pps[extra->pps_count], data + pos, len);
            extra->pps_len[extra->pps_count] = (int)len;
            extra->pps_count++;
        }

        pos += len;
    }

    return 0;
}

static int parse_stsd(MP4H264Extra *extra, int *width, int *height,
                      const uint8_t *data, size_t size)
{
    uint32_t entry_count = 0;
    size_t pos = 8;

    if (!extra || !width || !height || !data || size < 8)
    {
        return -1;
    }

    entry_count = mp4_rd32(data + 4);

    while (entry_count > 0 && pos + 8 <= size)
    {
        uint32_t entry_size = mp4_rd32(data + pos);
        uint32_t entry_type = mp4_rd32(data + pos + 4);

        if (entry_size < 8 || pos + entry_size > size)
        {
            return -1;
        }

        if (entry_type == MP4_FOURCC('a', 'v', 'c', '1') ||
            entry_type == MP4_FOURCC('a', 'v', 'c', '3'))
        {
            const uint8_t *entry = data + pos;

            if (entry_size >= 86)
            {
                if (*width <= 0)
                {
                    *width = (int)mp4_rd16(entry + 32);
                }
                if (*height <= 0)
                {
                    *height = (int)mp4_rd16(entry + 34);
                }
            }

            /* VisualSampleEntry 固定 86 字节，之后是 avcC 等子 box */
            if (entry_size > 86)
            {
                MP4Walker w;
                uint32_t type = 0;
                size_t poff = 0;
                size_t psize = 0;

                w.data = entry + 86;
                w.size = entry_size - 86;
                w.pos  = 0;

                while (walker_next(&w, &type, &poff, &psize) == 1)
                {
                    if (type == MP4_FOURCC('a', 'v', 'c', 'C'))
                    {
                        if (parse_avcc(extra, w.data + poff, psize) == 0)
                        {
                            return 0;
                        }
                        return -1;
                    }
                }
            }

            return -1;
        }

        pos += entry_size;
        entry_count--;
    }

    return -1;
}

static int parse_minf(MP4H264Extra *extra, int *width, int *height,
                      const uint8_t *data, size_t size)
{
    MP4Walker w;
    uint32_t type = 0;
    size_t poff = 0;
    size_t psize = 0;
    int ret = 0;

    w.data = data;
    w.size = size;
    w.pos  = 0;

    while ((ret = walker_next(&w, &type, &poff, &psize)) == 1)
    {
        if (type == MP4_FOURCC('s', 't', 'b', 'l'))
        {
            MP4Walker sw;
            uint32_t stype = 0;
            size_t soff = 0;
            size_t ssize = 0;
            int sr = 0;

            sw.data = w.data + poff;
            sw.size = psize;
            sw.pos  = 0;

            while ((sr = walker_next(&sw, &stype, &soff, &ssize)) == 1)
            {
                if (stype == MP4_FOURCC('s', 't', 's', 'd'))
                {
                    return parse_stsd(extra, width, height, sw.data + soff, ssize);
                }
            }

            return (sr < 0) ? -1 : 0;
        }
    }

    return (ret < 0) ? -1 : 0;
}

static int parse_mdia(MP4H264Extra *extra, int *width, int *height,
                      uint32_t *timescale, int *is_video,
                      const uint8_t *data, size_t size)
{
    MP4Walker w;
    uint32_t type = 0;
    size_t poff = 0;
    size_t psize = 0;
    int ret = 0;
    int have_handler = 0;

    w.data = data;
    w.size = size;
    w.pos  = 0;

    while ((ret = walker_next(&w, &type, &poff, &psize)) == 1)
    {
        const uint8_t *p = w.data + poff;

        if (type == MP4_FOURCC('m', 'd', 'h', 'd') && psize >= 20)
        {
            uint8_t version = p[0];

            if (version == 1 && psize >= 32)
            {
                *timescale = mp4_rd32(p + 20);
            }
            else
            {
                *timescale = mp4_rd32(p + 12);
            }
        }
        else if (type == MP4_FOURCC('h', 'd', 'l', 'r') && psize >= 12)
        {
            uint32_t handler = mp4_rd32(p + 8);

            *is_video = (handler == MP4_FOURCC('v', 'i', 'd', 'e'));
            have_handler = 1;
        }
        else if (type == MP4_FOURCC('m', 'i', 'n', 'f'))
        {
            if (parse_minf(extra, width, height, p, psize))
            {
                return -1;
            }
        }
        else
        {
            /* 其他 box 忽略 */
        }
    }

    if (ret < 0)
    {
        return -1;
    }

    return have_handler ? 0 : -1;
}

static int parse_trak(MP4Player *p, int *is_video,
                      const uint8_t *data, size_t size)
{
    MP4Walker w;
    uint32_t type = 0;
    size_t poff = 0;
    size_t psize = 0;
    int ret = 0;
    int track_id = 0;
    int width = 0;
    int height = 0;
    uint32_t timescale = 0;
    int video = 0;
    MP4H264Extra extra;
    int have_avcc = 0;

    memset(&extra, 0, sizeof(extra));
    extra.length_size = 4;

    w.data = data;
    w.size = size;
    w.pos  = 0;

    while ((ret = walker_next(&w, &type, &poff, &psize)) == 1)
    {
        const uint8_t *q = w.data + poff;

        if (type == MP4_FOURCC('t', 'k', 'h', 'd'))
        {
            uint8_t version = (psize > 0) ? q[0] : 0;

            if (psize < 24)
            {
                return -1;
            }

            if (version == 1)
            {
                /* v1: creation/modification/duration 都是 64bit */
                if (psize < 96)
                {
                    return -1;
                }
                track_id = (int)mp4_rd32(q + 20);
                width    = (int)mp4_rd32(q + 88) >> 16;
                height   = (int)mp4_rd32(q + 92) >> 16;
            }
            else
            {
                if (psize < 84)
                {
                    return -1;
                }
                track_id = (int)mp4_rd32(q + 12);
                width    = (int)mp4_rd32(q + 76) >> 16;
                height   = (int)mp4_rd32(q + 80) >> 16;
            }
        }
        else if (type == MP4_FOURCC('m', 'd', 'i', 'a'))
        {
            if (parse_mdia(&extra, &width, &height, &timescale, &video, q, psize))
            {
                return -1;
            }

            if (extra.sps_count > 0 || extra.pps_count > 0)
            {
                have_avcc = 1;
            }
        }
        else
        {
            /* 其他 box 忽略 */
        }
    }

    if (ret < 0)
    {
        return -1;
    }

    if (track_id <= 0 || timescale == 0 || !video)
    {
        return 0;
    }

    /*
     * 即使 avcC 中 SPS/PPS 为空（avc3），只要关键帧自带参数集也能回放；
     * 这里优先采用第一个视频轨。
     */
    p->track_id  = track_id;
    p->timescale = timescale;
    p->width     = width;
    p->height    = height;
    p->extra     = extra;
    p->extra.length_size = (extra.length_size > 0) ? extra.length_size : 4;

    *is_video = have_avcc ? 2 : 1;

    return 0;
}

static int parse_moov(MP4Player *p, const uint8_t *data, size_t size)
{
    MP4Walker w;
    uint32_t type = 0;
    size_t poff = 0;
    size_t psize = 0;
    int ret = 0;

    w.data = data;
    w.size = size;
    w.pos  = 0;

    while ((ret = walker_next(&w, &type, &poff, &psize)) == 1)
    {
        const uint8_t *q = w.data + poff;

        if (type == MP4_FOURCC('m', 'v', 'h', 'd') && psize >= 20)
        {
            uint8_t version = q[0];

            if (version == 1 && psize >= 28)
            {
                p->creation_time = (uint32_t)mp4_rd64(q + 4);
            }
            else
            {
                p->creation_time = mp4_rd32(q + 4);
            }
        }
        else if (type == MP4_FOURCC('t', 'r', 'a', 'k'))
        {
            int is_video = 0;

            if (p->track_id == 0 && parse_trak(p, &is_video, q, psize) == 0 &&
                is_video)
            {
                /* 已采用该视频轨 */
            }
        }
        else
        {
            /* 其他 box 忽略 */
        }
    }

    if (ret < 0)
    {
        return -1;
    }

    if (p->track_id <= 0 || p->timescale == 0)
    {
        return -1;
    }

    return 0;
}

/* ------------------------------------------------------------------------- */
/* moof/mdat 解析                                                            */
/* ------------------------------------------------------------------------- */

static int parse_moof(MP4Player *p, uint64_t moof_offset,
                      const uint8_t *data, size_t size,
                      MP4Fragment *frag, MP4SampleEntry *samples,
                      int *sample_count)
{
    MP4Walker w;
    uint32_t type = 0;
    size_t poff = 0;
    size_t psize = 0;
    int ret = 0;
    int total = 0;
    uint64_t data_end = 0;
    uint64_t min_data = 0;

    if (!p || !frag || !samples || !sample_count || !data)
    {
        return -1;
    }

    memset(frag, 0, sizeof(*frag));
    frag->moof_offset = moof_offset;
    frag->moof_size   = (uint32_t)(size + 8);

    /* sample 数据必须位于 moof 之后，防止损坏文件把偏移指回 moov/moof */
    min_data = moof_offset + frag->moof_size;

    w.data = data;
    w.size = size;
    w.pos  = 0;

    while ((ret = walker_next(&w, &type, &poff, &psize)) == 1)
    {
        if (type != MP4_FOURCC('t', 'r', 'a', 'f'))
        {
            continue;
        }

        {
            MP4Walker tw;
            uint32_t ttype = 0;
            size_t toff = 0;
            size_t tsize = 0;
            int tr = 0;
            int track_id = 0;
            int skip = 0;
            uint32_t default_dur = 0;
            uint32_t default_size = 0;
            uint32_t default_flags = 0;
            uint32_t first_flags = 0;
            int have_first_flags = 0;
            uint64_t base_data = moof_offset;
            uint64_t dts = 0;
            uint64_t data_pos = moof_offset;
            int have_data_pos = 0;

            tw.data = data + poff;
            tw.size = psize;
            tw.pos  = 0;

            while ((tr = walker_next(&tw, &ttype, &toff, &tsize)) == 1)
            {
                const uint8_t *q = tw.data + toff;

                if (ttype == MP4_FOURCC('t', 'f', 'h', 'd'))
                {
                    uint32_t flags = 0;
                    size_t pos = 8;

                    if (tsize < 8)
                    {
                        return -1;
                    }

                    flags = mp4_rd32(q) & 0x00FFFFFFu;
                    track_id = (int)mp4_rd32(q + 4);
                    skip = (track_id != p->track_id);

                    if (flags & 0x000001u) /* base-data-offset-present */
                    {
                        if (pos + 8 > tsize)
                        {
                            return -1;
                        }
                        base_data = mp4_rd64(q + pos);
                        pos += 8;
                    }

                    if (flags & 0x000002u) /* sample-description-index-present */
                    {
                        if (pos + 4 > tsize)
                        {
                            return -1;
                        }
                        pos += 4;
                    }

                    if (flags & 0x000008u) /* default-sample-duration-present */
                    {
                        if (pos + 4 > tsize)
                        {
                            return -1;
                        }
                        default_dur = mp4_rd32(q + pos);
                        pos += 4;
                    }

                    if (flags & 0x000010u) /* default-sample-size-present */
                    {
                        if (pos + 4 > tsize)
                        {
                            return -1;
                        }
                        default_size = mp4_rd32(q + pos);
                        pos += 4;
                    }

                    if (flags & 0x000020u) /* default-sample-flags-present */
                    {
                        if (pos + 4 > tsize)
                        {
                            return -1;
                        }
                        default_flags = mp4_rd32(q + pos);
                        pos += 4;
                    }
                }
                else if (ttype == MP4_FOURCC('t', 'f', 'd', 't'))
                {
                    if (tsize < 8)
                    {
                        return -1;
                    }

                    if (q[0] == 1)
                    {
                        if (tsize < 12)
                        {
                            return -1;
                        }
                        dts = mp4_rd64(q + 4);
                    }
                    else
                    {
                        dts = mp4_rd32(q + 4);
                    }
                }
                else if (ttype == MP4_FOURCC('t', 'r', 'u', 'n'))
                {
                    uint32_t full = 0;
                    uint32_t flags = 0;
                    int version = 0;
                    uint32_t count = 0;
                    size_t pos = 0;
                    int32_t signed_off = 0;
                    uint32_t i = 0;

                    if (skip)
                    {
                        continue;
                    }

                    if (tsize < 8)
                    {
                        return -1;
                    }

                    full = mp4_rd32(q);
                    version = (int)(full >> 24);
                    flags = full & 0x00FFFFFFu;
                    count = mp4_rd32(q + 4);
                    pos = 8;

                    if (count == 0 || total + (int)count > MP4_MAX_SAMPLES_PER_FRAG)
                    {
                        return -1;
                    }

                    have_first_flags = 0;
                    if (flags & 0x000004u) /* first-sample-flags-present */
                    {
                        if (pos + 4 > tsize)
                        {
                            return -1;
                        }
                        first_flags = mp4_rd32(q + pos);
                        have_first_flags = 1;
                        pos += 4;
                    }

                    if (flags & 0x000001u) /* data-offset-present */
                    {
                        if (pos + 4 > tsize)
                        {
                            return -1;
                        }
                        signed_off = (int32_t)mp4_rd32(q + pos);
                        pos += 4;
                        data_pos = (uint64_t)((int64_t)base_data + signed_off);
                        have_data_pos = 1;
                    }
                    else if (!have_data_pos)
                    {
                        data_pos = base_data;
                        have_data_pos = 1;
                    }
                    else
                    {
                        /* 沿用上一个 trun 的结束位置 */
                    }

                    for (i = 0; i < count; i++)
                    {
                        uint32_t dur = default_dur;
                        uint32_t ssize = default_size;
                        uint32_t sflags = default_flags;
                        int64_t ctts = 0;
                        MP4SampleEntry *e = NULL;

                        if (flags & 0x000100u) /* sample-duration-present */
                        {
                            if (pos + 4 > tsize)
                            {
                                return -1;
                            }
                            dur = mp4_rd32(q + pos);
                            pos += 4;
                        }

                        if (flags & 0x000200u) /* sample-size-present */
                        {
                            if (pos + 4 > tsize)
                            {
                                return -1;
                            }
                            ssize = mp4_rd32(q + pos);
                            pos += 4;
                        }

                        if (flags & 0x000400u) /* sample-flags-present */
                        {
                            if (pos + 4 > tsize)
                            {
                                return -1;
                            }
                            sflags = mp4_rd32(q + pos);
                            pos += 4;
                        }
                        else if (i == 0 && have_first_flags)
                        {
                            sflags = first_flags;
                        }
                        else
                        {
                            /* 使用 default_flags */
                        }

                        if (flags & 0x000800u) /* composition-time-offset-present */
                        {
                            uint32_t raw = 0;

                            if (pos + 4 > tsize)
                            {
                                return -1;
                            }
                            raw = mp4_rd32(q + pos);
                            pos += 4;

                            if (version == 0)
                            {
                                ctts = (int64_t)raw;
                            }
                            else
                            {
                                ctts = (int64_t)(int32_t)raw;
                            }
                        }

                        if (ssize == 0 || ssize > MP4_MAX_FRAME_SIZE)
                        {
                            return -1;
                        }

                        if (data_pos < min_data || data_pos > p->file_size ||
                            (uint64_t)ssize > p->file_size - data_pos)
                        {
                            return -2; /* 分片被掉电截断或偏移非法 */
                        }

                        e = &samples[total];
                        e->offset = data_pos;
                        e->size   = ssize;
                        e->dts    = dts;
                        e->ctts   = ctts;
                        e->flags  = sflags;

                        data_pos += ssize;
                        dts += dur;
                        total++;

                        if (data_pos > data_end)
                        {
                            data_end = data_pos;
                        }
                    }
                }
                else
                {
                    /* 其他 traf 子 box 忽略 */
                }
            }

            if (tr < 0)
            {
                return -1;
            }
        }
    }

    if (ret < 0)
    {
        return -1;
    }

    if (total <= 0)
    {
        return -1;
    }

    frag->base_dts     = samples[0].dts;
    frag->sample_count = (uint32_t)total;
    frag->data_end     = data_end;
    frag->first_key    = ((samples[0].flags & 0x00010000u) == 0) ? 1 : 0;
    *sample_count      = total;

    if (data_end > p->file_size)
    {
        return -2;
    }

    return 0;
}

/* ------------------------------------------------------------------------- */
/* 打开/索引                                                                 */
/* ------------------------------------------------------------------------- */

static int player_add_fragment(MP4Player *p, const MP4Fragment *frag)
{
    MP4Fragment *q = NULL;
    int new_cap = 0;

    if (!p || !frag)
    {
        return -1;
    }

    if (p->frag_count >= MP4_MAX_FRAGMENT_INDEX)
    {
        return 0; /* 超出索引上限，后面分片不再索引，已索引部分仍可回放 */
    }

    if (p->frag_count >= p->frag_cap)
    {
        new_cap = (p->frag_cap > 0) ? (p->frag_cap * 2) : 256;

        if (new_cap > MP4_MAX_FRAGMENT_INDEX)
        {
            new_cap = MP4_MAX_FRAGMENT_INDEX;
        }

        q = (MP4Fragment *)realloc(p->frags,
                                   (size_t)new_cap * sizeof(MP4Fragment));
        if (!q)
        {
            return -1;
        }

        p->frags = q;
        p->frag_cap = new_cap;
    }

    p->frags[p->frag_count] = *frag;
    p->frag_count++;

    return 0;
}

static int player_index_fragments(MP4Player *p)
{
    uint64_t off = 0;
    uint8_t *buf = NULL;

    while (off + 8 <= p->file_size)
    {
        uint32_t type = 0;
        uint64_t box_size = 0;
        uint64_t hdr = 0;
        int r = read_box_header(p->fd, off, p->file_size, &type, &box_size, &hdr);

        if (r == 0)
        {
            break;
        }
        if (r == -2)
        {
            /* 文件尾部被掉电截断，停止索引，之前完整分片仍可用 */
            break;
        }
        if (r < 0)
        {
            break;
        }

        if (type == MP4_FOURCC('m', 'o', 'o', 'f'))
        {
            MP4Fragment frag;
            MP4SampleEntry samples[MP4_MAX_SAMPLES_PER_FRAG];
            int count = 0;
            size_t payload = (size_t)(box_size - hdr);

            if (payload == 0 || payload > MP4_MAX_MOOF_SIZE)
            {
                break;
            }

            buf = (uint8_t *)malloc(payload);
            if (!buf)
            {
                return -1;
            }

            if (pread(p->fd, buf, payload, (off_t)(off + hdr)) != (ssize_t)payload)
            {
                free(buf);
                return -1;
            }

            r = parse_moof(p, off, buf, payload, &frag, samples, &count);
            free(buf);
            buf = NULL;

            if (r != 0)
            {
                /* 不完整/非法分片，丢弃当前及之后数据 */
                break;
            }

            if (player_add_fragment(p, &frag))
            {
                return -1;
            }
        }

        off += box_size;
    }

    return 0;
}

MP4Player *MP4Player_Open(const char *path)
{
    MP4Player *p = NULL;
    struct stat st;
    uint64_t off = 0;
    int found_moov = 0;
    uint8_t *moov = NULL;

    if (!path || !path[0])
    {
        return NULL;
    }

    p = (MP4Player *)calloc(1, sizeof(MP4Player));
    if (!p)
    {
        return NULL;
    }

    p->fd = open(path, O_RDONLY);
    if (p->fd < 0)
    {
        free(p);
        return NULL;
    }

    snprintf(p->path, sizeof(p->path), "%s", path);
    p->cur_frag = -1;

    if (fstat(p->fd, &st) != 0 || st.st_size <= 0)
    {
        MP4Player_Close(p);
        return NULL;
    }

    p->file_size = (uint64_t)st.st_size;

    /* 第一遍：找到 moov 并解析 track/avcC */
    while (off + 8 <= p->file_size)
    {
        uint32_t type = 0;
        uint64_t box_size = 0;
        uint64_t hdr = 0;
        int r = read_box_header(p->fd, off, p->file_size, &type, &box_size, &hdr);

        if (r != 1)
        {
            break;
        }

        if (type == MP4_FOURCC('m', 'o', 'o', 'v'))
        {
            size_t payload = (size_t)(box_size - hdr);

            if (payload == 0 || payload > MP4_MAX_MOOV_SIZE)
            {
                break;
            }

            moov = (uint8_t *)malloc(payload);
            if (!moov)
            {
                MP4Player_Close(p);
                return NULL;
            }

            if (pread(p->fd, moov, payload, (off_t)(off + hdr)) != (ssize_t)payload)
            {
                free(moov);
                MP4Player_Close(p);
                return NULL;
            }

            if (parse_moov(p, moov, payload))
            {
                free(moov);
                MP4Player_Close(p);
                return NULL;
            }

            free(moov);
            moov = NULL;
            found_moov = 1;
            break;
        }

        off += box_size;
    }

    if (!found_moov)
    {
        MP4Player_Close(p);
        return NULL;
    }

    p->file_start_sec = (p->creation_time > 0)
                            ? ((int64_t)p->creation_time - MP4_EPOCH_1904_OFFSET)
                            : 0;

    if (p->extra.length_size <= 0)
    {
        p->extra.length_size = 4;
    }

    /* 第二遍：索引所有 moof；掉电产生的半截分片会被自动排除 */
    if (player_index_fragments(p))
    {
        MP4Player_Close(p);
        return NULL;
    }

    return p;
}

void MP4Player_Close(MP4Player *p)
{
    if (!p)
    {
        return;
    }

    if (p->fd >= 0)
    {
        close(p->fd);
        p->fd = -1;
    }

    free(p->frags);
    free(p->sample_buf);
    free(p->out_buf);
    free(p);
}

/* ------------------------------------------------------------------------- */
/* 读帧                                                                      */
/* ------------------------------------------------------------------------- */

static int ensure_cap(uint8_t **buf, int *cap, int need)
{
    uint8_t *q = NULL;
    int new_cap = 0;

    if (!buf || !cap || need <= 0)
    {
        return -1;
    }

    if (*cap >= need)
    {
        return 0;
    }

    new_cap = (*cap > 0) ? *cap : 4096;
    while (new_cap < need)
    {
        if (new_cap > INT_MAX / 2)
        {
            new_cap = need;
            break;
        }
        new_cap *= 2;
    }

    q = (uint8_t *)realloc(*buf, (size_t)new_cap);
    if (!q)
    {
        return -1;
    }

    *buf = q;
    *cap = new_cap;

    return 0;
}

static int load_fragment(MP4Player *p, int index)
{
    uint8_t *buf = NULL;
    MP4Fragment frag;
    MP4SampleEntry samples[MP4_MAX_SAMPLES_PER_FRAG];
    int count = 0;
    int r = 0;

    if (!p || index < 0 || index >= p->frag_count)
    {
        return -1;
    }

    buf = (uint8_t *)malloc(p->frags[index].moof_size);
    if (!buf)
    {
        return -1;
    }

    if (pread(p->fd, buf, p->frags[index].moof_size,
              (off_t)p->frags[index].moof_offset) !=
        (ssize_t)p->frags[index].moof_size)
    {
        free(buf);
        return -1;
    }

    r = parse_moof(p, p->frags[index].moof_offset, buf + 8,
                   (size_t)p->frags[index].moof_size - 8,
                   &frag, samples, &count);
    free(buf);

    if (r != 0 || count <= 0 || count > MP4_MAX_SAMPLES_PER_FRAG)
    {
        return -1;
    }

    memcpy(p->samples, samples, (size_t)count * sizeof(MP4SampleEntry));
    p->sample_count = count;
    p->cur_sample   = 0;
    p->loaded       = 1;

    return 0;
}

static int player_read_next(MP4Player *p, int *out_size, int *is_key,
                            int64_t *pts_ticks)
{
    MP4SampleEntry *e = NULL;
    int key = 0;
    int required = 0;
    ssize_t n = 0;

    if (!p || !out_size || !is_key || !pts_ticks)
    {
        return -1;
    }

    while (1)
    {
        if (p->cur_frag < 0)
        {
            p->cur_frag = 0;
            p->loaded = 0;
        }

        if (p->cur_frag >= p->frag_count)
        {
            return 1;
        }

        if (!p->loaded)
        {
            if (load_fragment(p, p->cur_frag))
            {
                return -1;
            }
        }

        if (p->cur_sample >= p->sample_count)
        {
            p->cur_frag++;
            p->loaded = 0;
            continue;
        }

        break;
    }

    e = &p->samples[p->cur_sample];

    if (e->size == 0 || e->size > MP4_MAX_FRAME_SIZE)
    {
        return -1;
    }

    if (ensure_cap(&p->sample_buf, &p->sample_cap, (int)e->size))
    {
        return -1;
    }

    n = pread(p->fd, p->sample_buf, e->size, (off_t)e->offset);
    if (n != (ssize_t)e->size)
    {
        return -1;
    }

    key = ((e->flags & 0x00010000u) == 0) ? 1 : 0;

    if (MP4_H264AvccRequiredSize(p->sample_buf, (int)e->size, &p->extra,
                                 key, &required))
    {
        return -1;
    }

    if (required <= 0 ||
        ensure_cap(&p->out_buf, &p->out_cap, required))
    {
        return -1;
    }

    if (MP4_H264AvccToAnnexB(p->sample_buf, (int)e->size, &p->extra, key,
                             p->out_buf, p->out_cap, out_size))
    {
        return -1;
    }

    *is_key = key;
    *pts_ticks = (int64_t)e->dts + e->ctts;
    if (*pts_ticks < 0)
    {
        *pts_ticks = 0;
    }

    p->cur_sample++;

    return 0;
}

int MP4Player_ReadFrame(MP4Player *p, uint8_t **frame, int *size,
                        int *is_key, uint64_t *pts_us)
{
    int out_size = 0;
    int key = 0;
    int64_t ticks = 0;
    int r = 0;

    if (!p || !frame || !size || !is_key || !pts_us)
    {
        return -1;
    }

    r = player_read_next(p, &out_size, &key, &ticks);
    if (r != 0)
    {
        return r;
    }

    *frame  = p->out_buf;
    *size   = out_size;
    *is_key = key;
    *pts_us = (uint64_t)((ticks * 1000000LL) / (int64_t)p->timescale);

    return 0;
}

int MP4Player_SeekUS(MP4Player *p, uint64_t rel_us)
{
    uint64_t ticks = 0;
    int lo = 0;
    int hi = 0;
    int best = -1;
    int mid = 0;

    if (!p || p->timescale == 0)
    {
        return -1;
    }

    ticks = (rel_us * (uint64_t)MP4_TIMESCALE) / 1000000ULL;

    if (p->frag_count <= 0)
    {
        p->cur_frag = 0;
        p->loaded = 0;
        p->cur_sample = 0;
        return 0;
    }

    lo = 0;
    hi = p->frag_count - 1;

    /* 找最后一个 base_dts <= ticks 的分片；分片首帧必为 I 帧 */
    while (lo <= hi)
    {
        mid = lo + (hi - lo) / 2;

        if (p->frags[mid].base_dts <= ticks)
        {
            best = mid;
            lo = mid + 1;
        }
        else
        {
            hi = mid - 1;
        }
    }

    if (best < 0)
    {
        best = 0;
    }

    p->cur_frag = best;
    p->loaded = 0;
    p->cur_sample = 0;

    return 0;
}

/* ------------------------------------------------------------------------- */
/* 模板接口                                                                  */
/* ------------------------------------------------------------------------- */

int MP4PopHandle_Init(MP4PopHandle *h, const char *path,
                      unsigned int start_time, unsigned int stop_time)
{
    if (!h)
    {
        return -1;
    }

    memset(h, 0, sizeof(*h));

    if (path && path[0])
    {
        snprintf(h->szFilePath, sizeof(h->szFilePath), "%s", path);
    }

    h->start_time = start_time;
    h->stop_time  = stop_time;
    h->priv       = NULL;

    return 0;
}

void MP4PopHandle_Close(MP4PopHandle *h)
{
    if (!h)
    {
        return;
    }

    if (h->priv)
    {
        MP4Player_Close((MP4Player *)h->priv);
        h->priv = NULL;
    }
}

int pop_one_frame_from_sector_MP4(MP4PopHandle *PoPhandle,
                                  int rec_type,
                                  char **p_frame_buffer,
                                  int *pSize,
                                  int *pFrametype,
                                  int *pKeyframe,
                                  unsigned int *pRealTimeSec,
                                  unsigned int *pRealTimeUSec,
                                  int *pNeedSeek,
                                  int *VipFrame)
{
    MP4Player *p = NULL;
    char *path = NULL;
    int out_size = 0;
    int key = 0;
    int r = 0;
    int64_t ticks = 0;
    int64_t rel_us = 0;
    uint64_t abs_us = 0;
    unsigned int sec = 0;
    unsigned int usec = 0;

    (void)rec_type;

    if (!PoPhandle || !pSize || !pFrametype || !pKeyframe ||
        !pRealTimeSec || !pRealTimeUSec || !pNeedSeek)
    {
        return -1;
    }

    path = PoPhandle->szFilePath;
    if (!path[0])
    {
        return -1;
    }

    /*
     * 关键约定：文件不存在直接返回 2，并且绝不能操作 p_frame_buffer。
     * 这里在解引用 p_frame_buffer 之前就返回。
     */
    if (access(path, F_OK) != 0)
    {
        return 2;
    }

    if (!p_frame_buffer)
    {
        return -1;
    }

    p = (MP4Player *)PoPhandle->priv;

    /* 支持上层直接改写 szFilePath 切换到另一个录像文件 */
    if (p && strcmp(p->path, path) != 0)
    {
        MP4Player_Close(p);
        PoPhandle->priv = NULL;
        p = NULL;
    }

    if (!p)
    {
        p = MP4Player_Open(path);
        if (!p)
        {
            return -1;
        }

        PoPhandle->priv = p;
        *pNeedSeek = 1; /* 首次打开按 start_time 定位 */
    }

    if (*pNeedSeek)
    {
        if (PoPhandle->start_time > 0 && p->file_start_sec > 0 &&
            (int64_t)PoPhandle->start_time > p->file_start_sec)
        {
            rel_us = ((int64_t)PoPhandle->start_time - p->file_start_sec) *
                     1000000LL;
        }
        else
        {
            rel_us = 0;
        }

        if (MP4Player_SeekUS(p, (uint64_t)rel_us))
        {
            return -1;
        }

        *pNeedSeek = 0;
    }

    r = player_read_next(p, &out_size, &key, &ticks);
    if (r != 0)
    {
        return r;
    }

    rel_us = (ticks * 1000000LL) / (int64_t)p->timescale;
    if (rel_us < 0)
    {
        rel_us = 0;
    }

    abs_us = (uint64_t)rel_us;
    if (p->file_start_sec > 0)
    {
        abs_us += (uint64_t)p->file_start_sec * 1000000ULL;
    }

    sec  = (unsigned int)(abs_us / 1000000ULL);
    usec = (unsigned int)(abs_us % 1000000ULL);

    /* 到达指定回放结束时间 */
    if (PoPhandle->stop_time > 0 && sec > PoPhandle->stop_time)
    {
        return 1;
    }

    *p_frame_buffer = (char *)p->out_buf;
    *pSize          = out_size;
    *pFrametype     = key ? MP4_FRAME_TYPE_I : MP4_FRAME_TYPE_P;
    *pKeyframe      = key;
    *pRealTimeSec   = sec;
    *pRealTimeUSec  = usec;

    if (VipFrame)
    {
        *VipFrame = 0;
    }

    return 0;
}
