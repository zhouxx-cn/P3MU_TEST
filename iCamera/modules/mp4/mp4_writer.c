#ifndef _FILE_OFFSET_BITS
#define _FILE_OFFSET_BITS 64
#endif

#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>

#include "mp4_common.h"
#include "mp4_h264.h"
#include "mp4_writer.h"

#define MP4_PUT8(b, v)  do { if (MP4Buf_Put8((b), (v)))  return -1; } while (0)
#define MP4_PUT16(b, v) do { if (MP4Buf_Put16((b), (v))) return -1; } while (0)
#define MP4_PUT32(b, v) do { if (MP4Buf_Put32((b), (v))) return -1; } while (0)
#define MP4_PUT64(b, v) do { if (MP4Buf_Put64((b), (v))) return -1; } while (0)
#define MP4_END(b, o)   do { if (MP4Buf_EndBox((b), (o))) return -1; } while (0)

typedef struct
{
    uint64_t time_ticks;
    uint64_t moof_offset;
} MP4FragIndex;

struct MP4Writer
{
    int              fd;
    int              closed;
    int              error;
    char             errmsg[160];
    char             path[512];

    MP4WriterInfo    info;
    MP4WriterConfig  cfg;

    int              header_written;
    int              have_base;
    int64_t          base_dts_us;
    int64_t          last_dts_ticks;
    uint64_t         creation_time_1904;
    int              track_id;

    MP4H264Extra     extra;

    /* 当前分片缓存：小批量帧缓存，避免每帧一次 write/fsync */
    MP4Buf           payload;      /* 已转换为 AVCC 的 sample 数据 */
    uint32_t         sample_size[MP4_MAX_SAMPLES_PER_FRAG];
    uint32_t         sample_dur[MP4_MAX_SAMPLES_PER_FRAG];
    int32_t          sample_ctts[MP4_MAX_SAMPLES_PER_FRAG];
    uint32_t         sample_flags[MP4_MAX_SAMPLES_PER_FRAG];
    int              sample_count;
    int64_t          frag_base_dts_ticks;

    uint32_t         sequence;
    uint64_t         file_bytes;
    uint64_t         last_sync_ms;

    MP4FragIndex    *index;
    int              index_count;
    int              index_cap;
    int              index_disabled;
};

/* ------------------------------------------------------------------------- */
/* 基础工具                                                                  */
/* ------------------------------------------------------------------------- */

static void writer_set_error(MP4Writer *w, const char *msg)
{
    if (!w)
    {
        return;
    }

    w->error = 1;
    snprintf(w->errmsg, sizeof(w->errmsg), "%s", msg ? msg : "error");
}

static void writer_set_io_error(MP4Writer *w, const char *msg)
{
    int e = errno;

    if (!w)
    {
        return;
    }

    w->error = 1;
    snprintf(w->errmsg, sizeof(w->errmsg), "%s: %s",
             msg ? msg : "io error", strerror(e));
}

static int write_all(int fd, const void *buf, size_t len)
{
    const uint8_t *p = (const uint8_t *)buf;
    size_t off = 0;

    while (off < len)
    {
        ssize_t n = write(fd, p + off, len - off);

        if (n < 0)
        {
            if (errno == EINTR)
            {
                continue;
            }
            return -1;
        }

        if (n == 0)
        {
            return -1;
        }

        off += (size_t)n;
    }

    return 0;
}

static uint64_t now_ms(void)
{
    struct timeval tv;

    if (gettimeofday(&tv, NULL) == 0)
    {
        return (uint64_t)tv.tv_sec * 1000ULL + (uint64_t)tv.tv_usec / 1000ULL;
    }

    return (uint64_t)time(NULL) * 1000ULL;
}

static int64_t us_to_ticks(int64_t us)
{
    if (us <= 0)
    {
        return 0;
    }

    return (us * (int64_t)MP4_TIMESCALE + 500000LL) / 1000000LL;
}

static int64_t writer_nominal_ticks(const MP4Writer *w)
{
    int fps = 25;

    if (w && w->info.fps > 0 && w->info.fps <= 120)
    {
        fps = w->info.fps;
    }

    return (int64_t)MP4_TIMESCALE / fps;
}

static void put_matrix(MP4Buf *b)
{
    (void)MP4Buf_Put32(b, 0x00010000u);
    (void)MP4Buf_Put32(b, 0);
    (void)MP4Buf_Put32(b, 0);
    (void)MP4Buf_Put32(b, 0);
    (void)MP4Buf_Put32(b, 0x00010000u);
    (void)MP4Buf_Put32(b, 0);
    (void)MP4Buf_Put32(b, 0);
    (void)MP4Buf_Put32(b, 0);
    (void)MP4Buf_Put32(b, 0x40000000u);
}

static void put_zeros(MP4Buf *b, int n)
{
    int i = 0;

    for (i = 0; i < n; i++)
    {
        (void)MP4Buf_Put8(b, 0);
    }
}

/* ------------------------------------------------------------------------- */
/* moov 组装                                                                 */
/* ------------------------------------------------------------------------- */

static int write_ftyp(MP4Buf *b)
{
    size_t box = MP4Buf_BeginBox(b, "ftyp");

    MP4_PUT32(b, MP4_FOURCC('i', 's', 'o', 'm'));
    MP4_PUT32(b, 0x00000200u);
    MP4_PUT32(b, MP4_FOURCC('i', 's', 'o', 'm'));
    MP4_PUT32(b, MP4_FOURCC('i', 's', 'o', '6'));
    MP4_PUT32(b, MP4_FOURCC('a', 'v', 'c', '1'));
    MP4_PUT32(b, MP4_FOURCC('m', 'p', '4', '1'));

    MP4_END(b, box);

    return 0;
}

static int write_mvhd(MP4Buf *b, uint32_t ct)
{
    size_t box = MP4Buf_BeginBox(b, "mvhd");

    MP4_PUT32(b, 0); /* version 0, flags 0 */
    MP4_PUT32(b, ct);
    MP4_PUT32(b, ct);
    MP4_PUT32(b, 1000); /* movie timescale */
    MP4_PUT32(b, 0);    /* duration 0，分片 MP4 由 moof 表达时长 */
    MP4_PUT32(b, 0x00010000u); /* rate 1.0 */
    MP4_PUT16(b, 0x0100);      /* volume 1.0 */
    MP4_PUT16(b, 0);
    MP4_PUT32(b, 0);
    MP4_PUT32(b, 0);
    put_matrix(b);
    put_zeros(b, 24); /* pre_defined[6] */
    MP4_PUT32(b, 2);  /* next_track_ID */

    MP4_END(b, box);
    return 0;
}

static int write_tkhd(MP4Buf *b, uint32_t ct, int track_id, int width, int height)
{
    size_t box = MP4Buf_BeginBox(b, "tkhd");

    MP4_PUT32(b, 0x00000007u); /* version 0, flags: enabled|in_movie|in_preview */
    MP4_PUT32(b, ct);
    MP4_PUT32(b, ct);
    MP4_PUT32(b, (uint32_t)track_id);
    MP4_PUT32(b, 0); /* reserved */
    MP4_PUT32(b, 0); /* duration 0 */
    MP4_PUT32(b, 0);
    MP4_PUT32(b, 0);
    MP4_PUT16(b, 0); /* layer */
    MP4_PUT16(b, 0); /* alternate_group */
    MP4_PUT16(b, 0); /* volume: video = 0 */
    MP4_PUT16(b, 0); /* reserved */
    put_matrix(b);
    MP4_PUT32(b, (uint32_t)(width & 0xFFFF) << 16);
    MP4_PUT32(b, (uint32_t)(height & 0xFFFF) << 16);

    MP4_END(b, box);
    return 0;
}

static int write_mdhd(MP4Buf *b, uint32_t ct)
{
    size_t box = MP4Buf_BeginBox(b, "mdhd");

    MP4_PUT32(b, 0);
    MP4_PUT32(b, ct);
    MP4_PUT32(b, ct);
    MP4_PUT32(b, MP4_TIMESCALE);
    MP4_PUT32(b, 0);      /* duration 0 */
    MP4_PUT16(b, 0x55C4); /* language = und */
    MP4_PUT16(b, 0);

    MP4_END(b, box);
    return 0;
}

static int write_hdlr(MP4Buf *b)
{
    size_t box = MP4Buf_BeginBox(b, "hdlr");
    static const char name[] = "VideoHandler";

    MP4_PUT32(b, 0);
    MP4_PUT32(b, 0);
    MP4_PUT32(b, MP4_FOURCC('v', 'i', 'd', 'e'));
    MP4_PUT32(b, 0);
    MP4_PUT32(b, 0);
    MP4_PUT32(b, 0);
    (void)MP4Buf_Append(b, name, sizeof(name)); /* 含结尾 '\0' */

    MP4_END(b, box);
    return 0;
}

static int write_avcc(MP4Buf *b, const MP4H264Extra *extra)
{
    size_t box = MP4Buf_BeginBox(b, "avcC");
    int i = 0;

    if (extra->sps_count <= 0 || extra->pps_count <= 0)
    {
        return -1;
    }

    MP4_PUT8(b, 1); /* configurationVersion */
    MP4_PUT8(b, extra->sps[0][1]);
    MP4_PUT8(b, extra->sps[0][2]);
    MP4_PUT8(b, extra->sps[0][3]);
    MP4_PUT8(b, 0xFC | 0x03); /* lengthSizeMinusOne = 3 -> 4 字节长度前缀 */
    MP4_PUT8(b, (uint8_t)(0xE0 | (extra->sps_count & 0x1F)));

    for (i = 0; i < extra->sps_count; i++)
    {
        MP4_PUT16(b, (uint16_t)extra->sps_len[i]);
        (void)MP4Buf_Append(b, extra->sps[i], (size_t)extra->sps_len[i]);
    }

    MP4_PUT8(b, (uint8_t)extra->pps_count);
    for (i = 0; i < extra->pps_count; i++)
    {
        MP4_PUT16(b, (uint16_t)extra->pps_len[i]);
        (void)MP4Buf_Append(b, extra->pps[i], (size_t)extra->pps_len[i]);
    }

    MP4_END(b, box);
    return 0;
}

static int write_stsd(MP4Buf *b, const MP4H264Extra *extra, int width, int height)
{
    size_t stsd = MP4Buf_BeginBox(b, "stsd");
    size_t avc1 = 0;

    MP4_PUT32(b, 0);
    MP4_PUT32(b, 1); /* entry_count */

    avc1 = MP4Buf_BeginBox(b, "avc1");

    put_zeros(b, 6); /* reserved */
    MP4_PUT16(b, 1); /* data_reference_index */
    MP4_PUT16(b, 0); /* pre_defined */
    MP4_PUT16(b, 0); /* reserved */
    MP4_PUT32(b, 0); /* pre_defined[0] */
    MP4_PUT32(b, 0); /* pre_defined[1] */
    MP4_PUT32(b, 0); /* pre_defined[2] */
    MP4_PUT16(b, (uint16_t)width);
    MP4_PUT16(b, (uint16_t)height);
    MP4_PUT32(b, 0x00480000u); /* 72 dpi */
    MP4_PUT32(b, 0x00480000u);
    MP4_PUT32(b, 0); /* reserved */
    MP4_PUT16(b, 1); /* frame_count */
    put_zeros(b, 32); /* compressorname */
    MP4_PUT16(b, 0x0018); /* depth */
    MP4_PUT16(b, 0xFFFF); /* pre_defined */

    if (write_avcc(b, extra))
    {
        return -1;
    }

    MP4_END(b, avc1);
    MP4_END(b, stsd);

    return 0;
}

static int write_stbl(MP4Buf *b, const MP4H264Extra *extra, int width, int height)
{
    size_t stbl = MP4Buf_BeginBox(b, "stbl");
    size_t box = 0;

    if (write_stsd(b, extra, width, height))
    {
        return -1;
    }

    /* 分片 MP4 的 sample 表为空，实际索引在 moof/trun 中 */
    box = MP4Buf_BeginBox(b, "stts");
    MP4_PUT32(b, 0);
    MP4_PUT32(b, 0);
    MP4_END(b, box);

    box = MP4Buf_BeginBox(b, "stsc");
    MP4_PUT32(b, 0);
    MP4_PUT32(b, 0);
    MP4_END(b, box);

    box = MP4Buf_BeginBox(b, "stsz");
    MP4_PUT32(b, 0);
    MP4_PUT32(b, 0); /* sample_size */
    MP4_PUT32(b, 0); /* sample_count */
    MP4_END(b, box);

    box = MP4Buf_BeginBox(b, "stco");
    MP4_PUT32(b, 0);
    MP4_PUT32(b, 0);
    MP4_END(b, box);

    MP4_END(b, stbl);
    return 0;
}

static int write_minf(MP4Buf *b, const MP4H264Extra *extra, int width, int height)
{
    size_t minf = MP4Buf_BeginBox(b, "minf");
    size_t box = 0;

    box = MP4Buf_BeginBox(b, "vmhd");
    MP4_PUT32(b, 0x00000001u);
    MP4_PUT16(b, 0); /* graphicsmode */
    MP4_PUT16(b, 0);
    MP4_PUT16(b, 0);
    MP4_PUT16(b, 0);
    MP4_END(b, box);

    box = MP4Buf_BeginBox(b, "dinf");
    {
        size_t dref = MP4Buf_BeginBox(b, "dref");
        size_t url = 0;

        MP4_PUT32(b, 0);
        MP4_PUT32(b, 1);
        url = MP4Buf_BeginBox(b, "url ");
        MP4_PUT32(b, 0x00000001u); /* self-contained */
        MP4_END(b, url);
        MP4_END(b, dref);
    }
    MP4_END(b, box);

    if (write_stbl(b, extra, width, height))
    {
        return -1;
    }

    MP4_END(b, minf);
    return 0;
}

static int write_mdia(MP4Buf *b, uint32_t ct, const MP4H264Extra *extra,
                      int width, int height)
{
    size_t mdia = MP4Buf_BeginBox(b, "mdia");

    if (write_mdhd(b, ct) || write_hdlr(b) ||
        write_minf(b, extra, width, height))
    {
        return -1;
    }

    MP4_END(b, mdia);
    return 0;
}

static int write_trak(MP4Buf *b, uint32_t ct, int track_id,
                      const MP4H264Extra *extra, int width, int height)
{
    size_t trak = MP4Buf_BeginBox(b, "trak");

    if (write_tkhd(b, ct, track_id, width, height) ||
        write_mdia(b, ct, extra, width, height))
    {
        return -1;
    }

    MP4_END(b, trak);
    return 0;
}

static int write_mvex(MP4Buf *b, int track_id)
{
    size_t mvex = MP4Buf_BeginBox(b, "mvex");
    size_t trex = MP4Buf_BeginBox(b, "trex");

    MP4_PUT32(b, 0);
    MP4_PUT32(b, (uint32_t)track_id);
    MP4_PUT32(b, 1); /* default_sample_description_index */
    MP4_PUT32(b, 0); /* default_sample_duration */
    MP4_PUT32(b, 0); /* default_sample_size */
    MP4_PUT32(b, MP4_SAMPLE_FLAG_NON_SYNC);

    MP4_END(b, trex);
    MP4_END(b, mvex);

    return 0;
}

static int writer_write_header(MP4Writer *w)
{
    MP4Buf b;
    uint32_t ct = (uint32_t)w->creation_time_1904;
    int width = w->info.width;
    int height = w->info.height;
    size_t moov = 0;
    int ret = -1;

    if (width <= 0 || height <= 0)
    {
        if (w->extra.sps_count <= 0 ||
            MP4_H264GetSpsSize(w->extra.sps[0], w->extra.sps_len[0],
                               &width, &height))
        {
            writer_set_error(w, "cannot get width/height from SPS");
            return -1;
        }
    }

    if (width <= 0 || height <= 0 || width > 16384 || height > 16384)
    {
        writer_set_error(w, "invalid video size");
        return -1;
    }

    if (MP4Buf_Init(&b, 4096))
    {
        writer_set_error(w, "alloc header buffer failed");
        return -1;
    }

    if (write_ftyp(&b))
    {
        goto out;
    }

    moov = MP4Buf_BeginBox(&b, "moov");
    if (write_mvhd(&b, ct) ||
        write_trak(&b, ct, w->track_id, &w->extra, width, height) ||
        write_mvex(&b, w->track_id))
    {
        goto out;
    }
    if (MP4Buf_EndBox(&b, moov))
    {
        goto out;
    }

    if (b.err || write_all(w->fd, b.data, b.size))
    {
        writer_set_io_error(w, "write mp4 header failed");
        goto out;
    }

    /* moov 必须在任何 sample 之前落盘，保证掉电时文件结构完整 */
    if (fsync(w->fd))
    {
        writer_set_io_error(w, "fsync header failed");
        goto out;
    }

    w->file_bytes += (uint64_t)b.size;
    w->header_written = 1;
    ret = 0;

out:
    if (ret != 0 && !w->error)
    {
        writer_set_error(w, "write mp4 header failed");
    }
    MP4Buf_Free(&b);
    return ret;
}

/* ------------------------------------------------------------------------- */
/* 分片写入                                                                  */
/* ------------------------------------------------------------------------- */

static int writer_index_add(MP4Writer *w, uint64_t ticks, uint64_t offset)
{
    MP4FragIndex *p = NULL;
    int new_cap = 0;

    if (w->index_disabled)
    {
        return 0;
    }

    if (w->index_count >= MP4_MAX_FRAGMENT_INDEX)
    {
        w->index_disabled = 1;
        return 0;
    }

    if (w->index_count >= w->index_cap)
    {
        new_cap = (w->index_cap > 0) ? (w->index_cap * 2) : 256;

        if (new_cap > MP4_MAX_FRAGMENT_INDEX)
        {
            new_cap = MP4_MAX_FRAGMENT_INDEX;
        }

        p = (MP4FragIndex *)realloc(w->index, (size_t)new_cap * sizeof(MP4FragIndex));
        if (!p)
        {
            /* 索引只影响 mfra 随机访问，不影响录像继续写入 */
            w->index_disabled = 1;
            return 0;
        }

        w->index = p;
        w->index_cap = new_cap;
    }

    w->index[w->index_count].time_ticks  = ticks;
    w->index[w->index_count].moof_offset = offset;
    w->index_count++;

    return 0;
}

static int writer_flush_fragment(MP4Writer *w)
{
    MP4Buf moof;
    uint8_t mdat_hdr[8];
    size_t moof_box = 0;
    size_t mfhd = 0;
    size_t traf = 0;
    size_t tfhd = 0;
    size_t tfdt = 0;
    size_t trun = 0;
    size_t data_off_pos = 0;
    uint32_t data_offset = 0;
    uint32_t nominal = (uint32_t)writer_nominal_ticks(w);
    uint64_t moof_offset = w->file_bytes;
    int i = 0;
    int64_t sync_elapsed = 0;
    uint64_t now = 0;
    int ret = -1;

    if (w->sample_count <= 0)
    {
        return 0;
    }

    if (w->sample_dur[w->sample_count - 1] == 0)
    {
        w->sample_dur[w->sample_count - 1] = nominal;
    }

    if (MP4Buf_Init(&moof, 1024))
    {
        writer_set_error(w, "alloc moof buffer failed");
        return -1;
    }

    moof_box = MP4Buf_BeginBox(&moof, "moof");

    /* mfhd: 分片序号 */
    mfhd = MP4Buf_BeginBox(&moof, "mfhd");
    if (MP4Buf_Put32(&moof, 0) || MP4Buf_Put32(&moof, w->sequence) ||
        MP4Buf_EndBox(&moof, mfhd))
    {
        goto out;
    }

    traf = MP4Buf_BeginBox(&moof, "traf");

    /* tfhd: default-base-is-moof + default-sample-flags */
    tfhd = MP4Buf_BeginBox(&moof, "tfhd");
    if (MP4Buf_Put32(&moof, 0x00020000u | 0x00000020u) ||
        MP4Buf_Put32(&moof, (uint32_t)w->track_id) ||
        MP4Buf_Put32(&moof, MP4_SAMPLE_FLAG_NON_SYNC) ||
        MP4Buf_EndBox(&moof, tfhd))
    {
        goto out;
    }

    /* tfdt: 分片首个 sample 的解码时间 */
    tfdt = MP4Buf_BeginBox(&moof, "tfdt");
    if (MP4Buf_Put32(&moof, 0x01000000u) ||
        MP4Buf_Put64(&moof, (uint64_t)w->frag_base_dts_ticks) ||
        MP4Buf_EndBox(&moof, tfdt))
    {
        goto out;
    }

    /* trun v1: data_offset/duration/size/flags/ctts 全部按 sample 写入 */
    trun = MP4Buf_BeginBox(&moof, "trun");
    if (MP4Buf_Put32(&moof, 0x01000000u |
                             0x00000001u | 0x00000100u |
                             0x00000200u | 0x00000400u | 0x00000800u) ||
        MP4Buf_Put32(&moof, (uint32_t)w->sample_count))
    {
        goto out;
    }

    data_off_pos = moof.size;
    if (MP4Buf_Put32(&moof, 0)) /* 占位，moof 结束后回填 */
    {
        goto out;
    }

    for (i = 0; i < w->sample_count; i++)
    {
        if (MP4Buf_Put32(&moof, w->sample_dur[i]) ||
            MP4Buf_Put32(&moof, w->sample_size[i]) ||
            MP4Buf_Put32(&moof, w->sample_flags[i]) ||
            MP4Buf_Put32(&moof, (uint32_t)w->sample_ctts[i]))
        {
            goto out;
        }
    }

    if (MP4Buf_EndBox(&moof, trun) || MP4Buf_EndBox(&moof, traf) ||
        MP4Buf_EndBox(&moof, moof_box))
    {
        goto out;
    }

    /* data_offset 相对 moof 起始；+8 是 mdat 的 box header */
    data_offset = (uint32_t)(moof.size + 8);
    mp4_wr32(moof.data + data_off_pos, data_offset);

    mp4_wr32(mdat_hdr, (uint32_t)(8 + w->payload.size));
    memcpy(mdat_hdr + 4, "mdat", 4);

    /*
     * 先写 moof，再写 mdat。即使 mdat 写到一半掉电，回放侧会通过
     * trun 中的 size 与文件实际长度校验，把不完整分片整体丢弃，
     * 之前已经 fsync 的分片不受影响。
     */
    if (write_all(w->fd, moof.data, moof.size) ||
        write_all(w->fd, mdat_hdr, sizeof(mdat_hdr)) ||
        write_all(w->fd, w->payload.data, w->payload.size))
    {
        writer_set_io_error(w, "write fragment failed");
        goto out;
    }

    w->file_bytes += (uint64_t)moof.size + sizeof(mdat_hdr) + w->payload.size;

    (void)writer_index_add(w, (uint64_t)w->frag_base_dts_ticks, moof_offset);

    w->sequence++;
    w->payload.size = 0;
    w->sample_count = 0;

    now = now_ms();
    sync_elapsed = (int64_t)(now - w->last_sync_ms);
    if (w->cfg.sync_interval_ms <= 0 ||
        w->last_sync_ms == 0 ||
        sync_elapsed >= (int64_t)w->cfg.sync_interval_ms)
    {
        /* 周期强制刷盘：兼顾掉电可靠性与 SD 卡寿命 */
        if (fsync(w->fd))
        {
            writer_set_io_error(w, "fsync fragment failed");
            goto out;
        }
        w->last_sync_ms = now;
    }

    ret = 0;

out:
    MP4Buf_Free(&moof);
    return ret;
}

/* ------------------------------------------------------------------------- */
/* mfra 随机访问索引                                                         */
/* ------------------------------------------------------------------------- */

static int writer_write_mfra(MP4Writer *w)
{
    MP4Buf b;
    size_t mfra = 0;
    size_t tfra = 0;
    size_t mfro = 0;
    size_t mfro_size_pos = 0;
    int i = 0;
    int ret = -1;

    if (w->index_disabled || w->index_count <= 0)
    {
        return 0;
    }

    if (MP4Buf_Init(&b, (size_t)w->index_count * 20 + 64))
    {
        writer_set_error(w, "alloc mfra buffer failed");
        return -1;
    }

    mfra = MP4Buf_BeginBox(&b, "mfra");

    tfra = MP4Buf_BeginBox(&b, "tfra");
    if (MP4Buf_Put32(&b, 0x01000000u) || /* version 1: 64bit time/offset */
        MP4Buf_Put32(&b, (uint32_t)w->track_id) ||
        MP4Buf_Put32(&b, 0) || /* reserved */
        MP4Buf_Put32(&b, 0) || /* traf/trun/sample 字段长度均为 1 字节 */
        MP4Buf_Put32(&b, (uint32_t)w->index_count))
    {
        goto out;
    }

    for (i = 0; i < w->index_count; i++)
    {
        if (MP4Buf_Put64(&b, w->index[i].time_ticks) ||
            MP4Buf_Put64(&b, w->index[i].moof_offset) ||
            MP4Buf_Put8(&b, 1) || /* traf_number */
            MP4Buf_Put8(&b, 1) || /* trun_number */
            MP4Buf_Put8(&b, 1))   /* sample_number */
        {
            goto out;
        }
    }
    if (MP4Buf_EndBox(&b, tfra))
    {
        goto out;
    }

    mfro = MP4Buf_BeginBox(&b, "mfro");
    if (MP4Buf_Put32(&b, 0))
    {
        goto out;
    }
    mfro_size_pos = b.size;
    if (MP4Buf_Put32(&b, 0) || /* 占位，回填整个 mfra 大小 */
        MP4Buf_EndBox(&b, mfro) || MP4Buf_EndBox(&b, mfra))
    {
        goto out;
    }

    /* mfro.mfra_size 包含 mfra box header */
    mp4_wr32(b.data + mfro_size_pos, (uint32_t)(b.size - mfra));

    if (b.err || write_all(w->fd, b.data, b.size))
    {
        writer_set_io_error(w, "write mfra failed");
        goto out;
    }

    w->file_bytes += (uint64_t)b.size;
    ret = 0;

out:
    MP4Buf_Free(&b);
    return ret;
}

/* ------------------------------------------------------------------------- */
/* 对外接口                                                                  */
/* ------------------------------------------------------------------------- */

MP4Writer *MP4Writer_Create(const char *path, const MP4WriterInfo *info,
                            const MP4WriterConfig *cfg)
{
    MP4Writer *w = NULL;

    if (!path || !path[0])
    {
        return NULL;
    }

    w = (MP4Writer *)calloc(1, sizeof(MP4Writer));
    if (!w)
    {
        return NULL;
    }

    w->fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (w->fd < 0)
    {
        free(w);
        return NULL;
    }

    snprintf(w->path, sizeof(w->path), "%s", path);

    if (info)
    {
        w->info = *info;
    }
    if (w->info.fps <= 0 || w->info.fps > 120)
    {
        w->info.fps = 25;
    }

    if (cfg)
    {
        w->cfg = *cfg;
    }
    if (w->cfg.max_samples <= 0 || w->cfg.max_samples > MP4_MAX_SAMPLES_PER_FRAG)
    {
        w->cfg.max_samples = 30;
    }
    if (w->cfg.max_bytes <= 0)
    {
        w->cfg.max_bytes = 512 * 1024;
    }
    if (w->cfg.max_duration_ms <= 0)
    {
        w->cfg.max_duration_ms = 2000;
    }
    if (w->cfg.sync_interval_ms < 0)
    {
        w->cfg.sync_interval_ms = 2000;
    }

    if (MP4Buf_Init(&w->payload, 64 * 1024))
    {
        close(w->fd);
        free(w);
        return NULL;
    }

    w->track_id = 1;
    w->sequence = 0;
    w->last_dts_ticks = -1;
    w->extra.length_size = 4;
    w->creation_time_1904 = (uint64_t)(w->info.start_time ? w->info.start_time
                                                          : (unsigned int)time(NULL)) +
                            (uint64_t)MP4_EPOCH_1904_OFFSET;
    w->last_sync_ms = now_ms();

    return w;
}

int MP4Writer_WriteFrame(MP4Writer *w, const uint8_t *data, int size,
                         int64_t pts_us, int64_t dts_us, int keyframe)
{
    int scan_key = 0;
    int is_key = 0;
    int avcc_size = 0;
    int nal_count = 0;
    int written = 0;
    int64_t rel_dts_us = 0;
    int64_t rel_pts_us = 0;
    int64_t dts_ticks = 0;
    int64_t pts_ticks = 0;
    int64_t ctts = 0;
    int64_t nominal = 0;
    int limit_samples = 0;
    int64_t limit_bytes = 0;
    int64_t limit_ticks = 0;
    int limit_hit = 0;

    if (!w || !data || size <= 0 || size > MP4_MAX_FRAME_SIZE)
    {
        return -1;
    }

    if (w->error || w->closed)
    {
        return -1;
    }

    if (MP4_H264Scan(data, size, &w->extra, &scan_key))
    {
        writer_set_error(w, "invalid h264 frame");
        return -1;
    }

    is_key = (keyframe > 0 || scan_key) ? 1 : 0;

    if (!w->header_written)
    {
        /* 录像从第一个 I 帧开始，首帧之前的数据直接丢弃 */
        if (!is_key)
        {
            return 0;
        }

        if (w->extra.sps_count <= 0 || w->extra.pps_count <= 0)
        {
            writer_set_error(w, "missing SPS/PPS before first keyframe");
            return -1;
        }

        if (writer_write_header(w))
        {
            return -1;
        }
    }

    /*
     * 带 B 帧的码流 DTS 可能为负（编码器重排延迟），不能按帧截断为 0，
     * 否则时间轴会被压缩。这里只做防御性范围限制，实际归一化以首帧 DTS 为基准。
     */
    if (dts_us < -1000000000000LL)
    {
        dts_us = -1000000000000LL;
    }
    if (dts_us > 1000000000000LL)
    {
        dts_us = 1000000000000LL;
    }
    if (pts_us < -1000000000000LL)
    {
        pts_us = -1000000000000LL;
    }
    if (pts_us > 1000000000000LL)
    {
        pts_us = 1000000000000LL;
    }

    if (!w->have_base)
    {
        w->base_dts_us = dts_us;
        w->have_base = 1;
    }

    rel_dts_us = dts_us - w->base_dts_us;
    rel_pts_us = pts_us - w->base_dts_us;
    if (rel_dts_us < 0)
    {
        rel_dts_us = 0;
    }
    if (rel_pts_us < 0)
    {
        rel_pts_us = 0;
    }

    dts_ticks = us_to_ticks(rel_dts_us);
    pts_ticks = us_to_ticks(rel_pts_us);
    nominal = writer_nominal_ticks(w);

    /* 时间戳异常时保证 dts 单调，避免生成非法 stts */
    if (w->last_dts_ticks >= 0 && dts_ticks <= w->last_dts_ticks)
    {
        dts_ticks = w->last_dts_ticks + nominal;
    }

    if (MP4_H264AvccSize(data, size, &avcc_size, &nal_count))
    {
        writer_set_error(w, "convert frame to avcc failed");
        return -1;
    }

    if (avcc_size <= 0 || avcc_size > MP4_MAX_FRAME_SIZE)
    {
        writer_set_error(w, "frame too large");
        return -1;
    }

    /* 先根据当前帧补齐上一帧 duration，再判断是否需要切分片 */
    if (w->sample_count > 0)
    {
        int64_t dur = dts_ticks - w->last_dts_ticks;

        if (dur <= 0 || dur > (int64_t)0xFFFFFFFFu)
        {
            dur = nominal;
        }
        w->sample_dur[w->sample_count - 1] = (uint32_t)dur;

        limit_samples = (w->cfg.max_samples > 0) ? w->cfg.max_samples
                                                 : MP4_MAX_SAMPLES_PER_FRAG;
        if (limit_samples > MP4_MAX_SAMPLES_PER_FRAG)
        {
            limit_samples = MP4_MAX_SAMPLES_PER_FRAG;
        }

        limit_bytes = (w->cfg.max_bytes > 0) ? (int64_t)w->cfg.max_bytes
                                             : (int64_t)MP4_MAX_FRAGMENT_BYTES;
        limit_ticks = (int64_t)((w->cfg.max_duration_ms > 0)
                                    ? w->cfg.max_duration_ms : 2000) *
                      (int64_t)MP4_TIMESCALE / 1000;

        if (is_key)
        {
            limit_hit = 1; /* 每个分片从 I 帧开始，seek 可直接落到关键帧 */
        }
        else if (w->sample_count >= limit_samples)
        {
            limit_hit = 1;
        }
        else if ((int64_t)w->payload.size + (int64_t)avcc_size > limit_bytes)
        {
            limit_hit = 1;
        }
        else if (dts_ticks - w->frag_base_dts_ticks >= limit_ticks)
        {
            limit_hit = 1;
        }
        else
        {
            limit_hit = 0;
        }

        if (limit_hit && writer_flush_fragment(w))
        {
            return -1;
        }
    }

    if (w->sample_count == 0)
    {
        w->frag_base_dts_ticks = dts_ticks;
    }

    if (MP4Buf_Reserve(&w->payload, (size_t)avcc_size))
    {
        writer_set_error(w, "fragment buffer overflow");
        return -1;
    }

    if (MP4_H264AnnexBToAvcc(data, size,
                             w->payload.data + w->payload.size,
                             (int)(w->payload.cap - w->payload.size),
                             &written))
    {
        writer_set_error(w, "annexb->avcc failed");
        return -1;
    }

    if (written != avcc_size)
    {
        writer_set_error(w, "avcc size mismatch");
        return -1;
    }

    w->payload.size += (size_t)written;

    ctts = pts_ticks - dts_ticks;
    if (ctts > 0x7FFFFFFFLL)
    {
        ctts = 0x7FFFFFFFLL;
    }
    if (ctts < -0x80000000LL)
    {
        ctts = -0x80000000LL;
    }

    w->sample_size[w->sample_count]  = (uint32_t)written;
    w->sample_dur[w->sample_count]   = 0;
    w->sample_ctts[w->sample_count]  = (int32_t)ctts;
    w->sample_flags[w->sample_count] = is_key ? MP4_SAMPLE_FLAG_SYNC
                                              : MP4_SAMPLE_FLAG_NON_SYNC;
    w->sample_count++;
    w->last_dts_ticks = dts_ticks;

    return 0;
}

int MP4Writer_Flush(MP4Writer *w)
{
    if (!w || w->error || w->closed)
    {
        return -1;
    }

    if (writer_flush_fragment(w))
    {
        return -1;
    }

    if (fsync(w->fd))
    {
        writer_set_io_error(w, "fsync flush failed");
        return -1;
    }

    w->last_sync_ms = now_ms();
    return 0;
}

int MP4Writer_Close(MP4Writer *w)
{
    int ret = 0;

    if (!w || w->closed)
    {
        return 0;
    }

    if (writer_flush_fragment(w))
    {
        ret = -1;
    }

    /* mfra 失败不影响已写分片的可播放性，但需要向上层报告 */
    if (writer_write_mfra(w))
    {
        ret = -1;
    }

    if (fsync(w->fd))
    {
        writer_set_io_error(w, "fsync close failed");
        ret = -1;
    }

    if (close(w->fd))
    {
        writer_set_io_error(w, "close failed");
        ret = -1;
    }

    w->fd = -1;
    w->closed = 1;

    return ret;
}

void MP4Writer_Destroy(MP4Writer *w)
{
    if (!w)
    {
        return;
    }

    if (!w->closed)
    {
        (void)MP4Writer_Close(w);
    }

    MP4Buf_Free(&w->payload);
    free(w->index);
    free(w);
}

int MP4Writer_GetFileSize(MP4Writer *w)
{
    if (!w)
    {
        return -1;
    }

    if (w->file_bytes > 0x7FFFFFFFu)
    {
        return 0x7FFFFFFF;
    }

    return (int)w->file_bytes;
}

const char *MP4Writer_GetLastError(MP4Writer *w)
{
    if (!w)
    {
        return "null writer";
    }

    return w->errmsg;
}
