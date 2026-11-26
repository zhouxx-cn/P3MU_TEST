#include <limits.h>
#include <string.h>

#include "mp4_h264.h"

#define H264_NAL_IDR  5
#define H264_NAL_SEI  6
#define H264_NAL_SPS  7
#define H264_NAL_PPS  8
#define H264_NAL_AUD  9

/* ------------------------------------------------------------------------- */
/* Annex-B NAL 遍历                                                          */
/* ------------------------------------------------------------------------- */

static int h264_find_start_code(const uint8_t *buf, int size, int from,
                                int *sc_start, int *nal_start)
{
    int i = from;

    while (i + 3 <= size)
    {
        if (buf[i] == 0x00 && buf[i + 1] == 0x00)
        {
            if (buf[i + 2] == 0x01)
            {
                *sc_start  = i;
                *nal_start = i + 3;
                return 1;
            }

            if (i + 4 <= size && buf[i + 2] == 0x00 && buf[i + 3] == 0x01)
            {
                *sc_start  = i;
                *nal_start = i + 4;
                return 1;
            }
        }
        i++;
    }

    return 0;
}

int MP4_H264IterateNal(const uint8_t *buf, int size, int *pos, MP4Nal *nal)
{
    int sc_start = 0;
    int nal_start = 0;
    int next_sc = 0;
    int end = 0;

    if (!buf || size <= 0 || !pos || !nal)
    {
        return -1;
    }

    while (1)
    {
        if (!h264_find_start_code(buf, size, *pos, &sc_start, &nal_start))
        {
            *pos = size;
            return 0;
        }

        /* 查找下一个起始码，作为当前 NAL 的结束位置 */
        if (h264_find_start_code(buf, size, nal_start, &next_sc, &end))
        {
            end = next_sc;
        }
        else
        {
            end = size;
        }

        /* 去除 NAL 尾部填充的 0x00（部分编码器会补零对齐） */
        while (end > nal_start && buf[end - 1] == 0x00)
        {
            end--;
        }

        *pos = (next_sc > 0) ? next_sc : size;

        if (end > nal_start)
        {
            nal->data = buf + nal_start;
            nal->size = end - nal_start;
            return 1;
        }

        /* 空 NAL（连续起始码）继续向后找 */
        if (*pos >= size)
        {
            return 0;
        }
    }
}

/* ------------------------------------------------------------------------- */
/* SPS/PPS 提取与宽高解析                                                    */
/* ------------------------------------------------------------------------- */

static int h264_extra_add_sps(MP4H264Extra *extra, const uint8_t *data, int len)
{
    int i = 0;

    if (len <= 0 || len > MP4_MAX_SPS_SIZE || extra->sps_count >= MP4_MAX_SPS_COUNT)
    {
        return 0;
    }

    for (i = 0; i < extra->sps_count; i++)
    {
        if (extra->sps_len[i] == len && memcmp(extra->sps[i], data, (size_t)len) == 0)
        {
            return 0; /* 去重 */
        }
    }

    memcpy(extra->sps[extra->sps_count], data, (size_t)len);
    extra->sps_len[extra->sps_count] = len;
    extra->sps_count++;

    return 1;
}

static int h264_extra_add_pps(MP4H264Extra *extra, const uint8_t *data, int len)
{
    int i = 0;

    if (len <= 0 || len > MP4_MAX_PPS_SIZE || extra->pps_count >= MP4_MAX_PPS_COUNT)
    {
        return 0;
    }

    for (i = 0; i < extra->pps_count; i++)
    {
        if (extra->pps_len[i] == len && memcmp(extra->pps[i], data, (size_t)len) == 0)
        {
            return 0;
        }
    }

    memcpy(extra->pps[extra->pps_count], data, (size_t)len);
    extra->pps_len[extra->pps_count] = len;
    extra->pps_count++;

    return 1;
}

int MP4_H264Scan(const uint8_t *buf, int size, MP4H264Extra *extra, int *is_key)
{
    int pos = 0;
    int found = 0;
    int key = 0;
    MP4Nal nal;

    if (!buf || size <= 0 || !extra)
    {
        return -1;
    }

    if (extra->length_size == 0)
    {
        extra->length_size = 4;
    }

    while (MP4_H264IterateNal(buf, size, &pos, &nal) == 1)
    {
        int type = 0;

        found = 1;
        if (nal.size <= 0)
        {
            continue;
        }

        type = nal.data[0] & 0x1F;

        if (type == H264_NAL_IDR)
        {
            key = 1;
        }
        else if (type == H264_NAL_SPS)
        {
            (void)h264_extra_add_sps(extra, nal.data, nal.size);
        }
        else if (type == H264_NAL_PPS)
        {
            (void)h264_extra_add_pps(extra, nal.data, nal.size);
        }
        else
        {
            /* SEI/AUD/普通 slice 无需处理 */
        }
    }

    if (!found)
    {
        return -1;
    }

    if (is_key)
    {
        *is_key = key;
    }

    return 0;
}

/* 去掉 H.264 防竞争字节 00 00 03 -> 00 00 */
static int h264_remove_epb(const uint8_t *in, int in_len, uint8_t *out, int out_cap)
{
    int i = 0;
    int n = 0;
    int zeros = 0;

    if (!in || !out || in_len <= 0 || out_cap <= 0)
    {
        return -1;
    }

    for (i = 0; i < in_len; i++)
    {
        if (zeros >= 2 && in[i] == 0x03)
        {
            zeros = 0;
            continue;
        }

        if (n >= out_cap)
        {
            return -1;
        }

        out[n++] = in[i];

        if (in[i] == 0x00)
        {
            zeros++;
        }
        else
        {
            zeros = 0;
        }
    }

    return n;
}

typedef struct
{
    const uint8_t *data;
    int            bytes;
    int            bitpos;
} MP4BitReader;

static int br_read_bit(MP4BitReader *br, int *bit)
{
    if (!br || br->bitpos >= br->bytes * 8)
    {
        return -1;
    }

    *bit = (br->data[br->bitpos >> 3] >> (7 - (br->bitpos & 7))) & 1;
    br->bitpos++;

    return 0;
}

static int br_read_bits(MP4BitReader *br, int n, uint32_t *value)
{
    uint32_t v = 0;
    int i = 0;
    int bit = 0;

    if (n < 0 || n > 32)
    {
        return -1;
    }

    for (i = 0; i < n; i++)
    {
        if (br_read_bit(br, &bit))
        {
            return -1;
        }
        v = (v << 1) | (uint32_t)bit;
    }

    *value = v;
    return 0;
}

static int br_read_ue(MP4BitReader *br, uint32_t *value)
{
    int zeros = 0;
    int bit = 0;
    uint32_t suffix = 0;

    while (1)
    {
        if (br_read_bit(br, &bit))
        {
            return -1;
        }

        if (bit)
        {
            break;
        }

        zeros++;
        if (zeros > 31)
        {
            return -1;
        }
    }

    if (zeros > 0 && br_read_bits(br, zeros, &suffix))
    {
        return -1;
    }

    *value = ((uint32_t)1 << zeros) - 1u + suffix;
    return 0;
}

static int br_read_se(MP4BitReader *br, int32_t *value)
{
    uint32_t ue = 0;

    if (br_read_ue(br, &ue))
    {
        return -1;
    }

    if (ue & 1u)
    {
        *value = (int32_t)((ue + 1u) / 2u);
    }
    else
    {
        *value = -(int32_t)(ue / 2u);
    }

    return 0;
}

static int h264_skip_scaling_list(MP4BitReader *br, int size)
{
    int last = 8;
    int next = 8;
    int j = 0;

    for (j = 0; j < size; j++)
    {
        if (next != 0)
        {
            int32_t delta = 0;

            if (br_read_se(br, &delta))
            {
                return -1;
            }

            next = (last + delta + 256) % 256;
        }

        last = (next == 0) ? last : next;
    }

    return 0;
}

int MP4_H264GetSpsSize(const uint8_t *sps, int sps_len, int *width, int *height)
{
    uint8_t rbsp[MP4_MAX_SPS_SIZE * 2];
    MP4BitReader br;
    int rbsp_len = 0;
    uint32_t profile = 0;
    uint32_t ue = 0;
    uint32_t chroma_format_idc = 1;
    uint32_t separate_colour_plane = 0;
    uint32_t frame_mbs_only = 1;
    uint32_t frame_cropping = 0;
    uint32_t crop_left = 0;
    uint32_t crop_right = 0;
    uint32_t crop_top = 0;
    uint32_t crop_bottom = 0;
    uint32_t pic_width_in_mbs = 0;
    uint32_t pic_height_in_map_units = 0;
    int width_px = 0;
    int height_px = 0;
    int crop_unit_x = 1;
    int crop_unit_y = 1;
    int bit = 0;
    int i = 0;

    if (!sps || sps_len <= 1 || !width || !height)
    {
        return -1;
    }

    /* sps[0] 是 NAL header，真正的 RBSP 从 sps[1] 开始 */
    rbsp_len = h264_remove_epb(sps + 1, sps_len - 1, rbsp, (int)sizeof(rbsp));
    if (rbsp_len < 4)
    {
        return -1;
    }

    memset(&br, 0, sizeof(br));
    br.data   = rbsp;
    br.bytes  = rbsp_len;
    br.bitpos = 0;

    /* profile_idc, constraint flags, level_idc */
    if (br_read_bits(&br, 8, &profile) ||
        br_read_bits(&br, 8, &ue) ||
        br_read_bits(&br, 8, &ue))
    {
        return -1;
    }

    if (br_read_ue(&br, &ue)) /* seq_parameter_set_id */
    {
        return -1;
    }

    if (profile == 100 || profile == 110 || profile == 122 || profile == 244 ||
        profile == 44  || profile == 83  || profile == 86  || profile == 118 ||
        profile == 128 || profile == 138 || profile == 139 || profile == 134 ||
        profile == 135)
    {
        if (br_read_ue(&br, &chroma_format_idc))
        {
            return -1;
        }

        if (chroma_format_idc == 3)
        {
            if (br_read_bit(&br, &bit))
            {
                return -1;
            }
            separate_colour_plane = (uint32_t)bit;
        }

        if (br_read_ue(&br, &ue) || br_read_ue(&br, &ue)) /* bit_depth_luma/chroma */
        {
            return -1;
        }

        if (br_read_bit(&br, &bit)) /* qpprime_y_zero_transform_bypass_flag */
        {
            return -1;
        }

        if (br_read_bit(&br, &bit)) /* seq_scaling_matrix_present_flag */
        {
            return -1;
        }

        if (bit)
        {
            int lists = (chroma_format_idc != 3) ? 8 : 12;

            for (i = 0; i < lists; i++)
            {
                int present = 0;

                if (br_read_bit(&br, &present))
                {
                    return -1;
                }

                if (present && h264_skip_scaling_list(&br, (i < 6) ? 16 : 64))
                {
                    return -1;
                }
            }
        }
    }

    if (br_read_ue(&br, &ue)) /* log2_max_frame_num_minus4 */
    {
        return -1;
    }

    if (br_read_ue(&br, &ue)) /* pic_order_cnt_type */
    {
        return -1;
    }

    if (ue == 0)
    {
        if (br_read_ue(&br, &ue)) /* log2_max_pic_order_cnt_lsb_minus4 */
        {
            return -1;
        }
    }
    else if (ue == 1)
    {
        int32_t se = 0;

        if (br_read_bit(&br, &bit) || /* delta_pic_order_always_zero_flag */
            br_read_se(&br, &se)  || /* offset_for_non_ref_pic */
            br_read_se(&br, &se))    /* offset_for_top_to_bottom_field */
        {
            return -1;
        }

        if (br_read_ue(&br, &ue)) /* num_ref_frames_in_pic_order_cnt_cycle */
        {
            return -1;
        }

        if (ue > 256)
        {
            return -1;
        }

        for (i = 0; i < (int)ue; i++)
        {
            if (br_read_se(&br, &se))
            {
                return -1;
            }
        }
    }
    else
    {
        /* type 2，无需额外字段 */
    }

    if (br_read_ue(&br, &ue)) /* max_num_ref_frames */
    {
        return -1;
    }

    if (br_read_bit(&br, &bit)) /* gaps_in_frame_num_value_allowed_flag */
    {
        return -1;
    }

    if (br_read_ue(&br, &pic_width_in_mbs) ||
        br_read_ue(&br, &pic_height_in_map_units))
    {
        return -1;
    }

    if (br_read_bit(&br, &bit)) /* frame_mbs_only_flag */
    {
        return -1;
    }
    frame_mbs_only = (uint32_t)bit;

    if (!frame_mbs_only)
    {
        if (br_read_bit(&br, &bit)) /* mb_adaptive_frame_field_flag */
        {
            return -1;
        }
    }

    if (br_read_bit(&br, &bit)) /* direct_8x8_inference_flag */
    {
        return -1;
    }

    if (br_read_bit(&br, &bit)) /* frame_cropping_flag */
    {
        return -1;
    }
    frame_cropping = (uint32_t)bit;

    if (frame_cropping)
    {
        if (br_read_ue(&br, &crop_left) ||
            br_read_ue(&br, &crop_right) ||
            br_read_ue(&br, &crop_top) ||
            br_read_ue(&br, &crop_bottom))
        {
            return -1;
        }
    }

    /* 计算 Cropping Unit：4:2:0 -> 2,2；4:2:2 -> 2,1；4:4:4/单色 -> 1,1 */
    if (separate_colour_plane || chroma_format_idc == 0)
    {
        crop_unit_x = 1;
        crop_unit_y = 1;
    }
    else if (chroma_format_idc == 1)
    {
        crop_unit_x = 2;
        crop_unit_y = 2;
    }
    else if (chroma_format_idc == 2)
    {
        crop_unit_x = 2;
        crop_unit_y = 1;
    }
    else
    {
        crop_unit_x = 1;
        crop_unit_y = 1;
    }

    if (pic_width_in_mbs > 4096 || pic_height_in_map_units > 4096)
    {
        return -1;
    }

    width_px  = (int)((pic_width_in_mbs + 1) * 16);
    height_px = (int)((pic_height_in_map_units + 1) * 16 * (2 - frame_mbs_only));

    width_px  -= (int)((crop_left + crop_right) * (uint32_t)crop_unit_x);
    height_px -= (int)((crop_top + crop_bottom) * (uint32_t)crop_unit_y);

    if (width_px <= 0 || height_px <= 0 || width_px > 16384 || height_px > 16384)
    {
        return -1;
    }

    *width  = width_px;
    *height = height_px;

    return 0;
}

/* ------------------------------------------------------------------------- */
/* Annex-B <-> AVCC                                                          */
/* ------------------------------------------------------------------------- */

int MP4_H264AvccSize(const uint8_t *in, int in_size, int *out_size, int *nal_count)
{
    int pos = 0;
    int total = 0;
    int count = 0;
    MP4Nal nal;

    if (!in || in_size <= 0 || !out_size || !nal_count)
    {
        return -1;
    }

    while (MP4_H264IterateNal(in, in_size, &pos, &nal) == 1)
    {
        if (nal.size <= 0 || nal.size > INT_MAX - 4)
        {
            return -1;
        }

        if (total > INT_MAX - 4 - nal.size)
        {
            return -1;
        }

        total += 4 + nal.size;
        count++;
    }

    if (count == 0)
    {
        return -1;
    }

    *out_size  = total;
    *nal_count = count;

    return 0;
}

int MP4_H264AnnexBToAvcc(const uint8_t *in, int in_size,
                         uint8_t *out, int out_cap, int *out_size)
{
    int pos = 0;
    int written = 0;
    MP4Nal nal;

    if (!in || in_size <= 0 || !out || out_cap <= 0 || !out_size)
    {
        return -1;
    }

    while (MP4_H264IterateNal(in, in_size, &pos, &nal) == 1)
    {
        if (nal.size <= 0)
        {
            continue;
        }

        if (written > out_cap - 4 || nal.size > out_cap - written - 4)
        {
            return -1;
        }

        mp4_wr32(out + written, (uint32_t)nal.size);
        written += 4;
        memcpy(out + written, nal.data, (size_t)nal.size);
        written += nal.size;
    }

    if (written <= 0)
    {
        return -1;
    }

    *out_size = written;
    return 0;
}

/* 读取一个 AVCC NAL。返回 1=成功，0=结束，-1=非法 */
static int avcc_next(const uint8_t *in, int in_size, int *pos, int length_size,
                     const uint8_t **nal, int *nal_size)
{
    uint32_t len = 0;
    int p = *pos;
    int i = 0;

    if (!in || !pos || !nal || !nal_size || length_size <= 0 || p < 0 || p > in_size)
    {
        return -1;
    }

    if (p == in_size)
    {
        return 0;
    }

    if (p > in_size - length_size)
    {
        return -1;
    }

    for (i = 0; i < length_size; i++)
    {
        len = (len << 8) | in[p + i];
    }

    p += length_size;

    if (len == 0 || len > 0x7FFFFFFFu || (int)len > in_size - p)
    {
        return -1;
    }

    *nal      = in + p;
    *nal_size = (int)len;
    *pos      = p + (int)len;

    return 1;
}

int MP4_H264AvccRequiredSize(const uint8_t *in, int in_size,
                             const MP4H264Extra *extra, int keyframe,
                             int *required)
{
    int pos = 0;
    int total = 0;
    int has_sps = 0;
    int has_pps = 0;
    int count = 0;
    int i = 0;
    int r = 0;
    const uint8_t *nal = NULL;
    int nal_size = 0;
    int length_size = 4;

    if (!in || in_size <= 0 || !required)
    {
        return -1;
    }

    if (extra && extra->length_size > 0)
    {
        length_size = extra->length_size;
    }

    while ((r = avcc_next(in, in_size, &pos, length_size, &nal, &nal_size)) == 1)
    {
        int type = nal[0] & 0x1F;

        if (type == H264_NAL_SPS)
        {
            has_sps = 1;
        }
        else if (type == H264_NAL_PPS)
        {
            has_pps = 1;
        }

        if (total > INT_MAX - 4 - nal_size)
        {
            return -1;
        }

        total += 4 + nal_size;
        count++;
    }

    if (r < 0 || count == 0)
    {
        return -1;
    }

    if (keyframe && extra)
    {
        if (!has_sps)
        {
            for (i = 0; i < extra->sps_count; i++)
            {
                if (extra->sps_len[i] > 0)
                {
                    total += 4 + extra->sps_len[i];
                }
            }
        }

        if (!has_pps)
        {
            for (i = 0; i < extra->pps_count; i++)
            {
                if (extra->pps_len[i] > 0)
                {
                    total += 4 + extra->pps_len[i];
                }
            }
        }
    }

    *required = total;
    return 0;
}

int MP4_H264AvccToAnnexB(const uint8_t *in, int in_size,
                         const MP4H264Extra *extra, int keyframe,
                         uint8_t *out, int out_cap, int *out_size)
{
    int pos = 0;
    int written = 0;
    int has_sps = 0;
    int has_pps = 0;
    int i = 0;
    int r = 0;
    const uint8_t *nal = NULL;
    int nal_size = 0;
    int length_size = 4;
    static const uint8_t start_code[4] = {0x00, 0x00, 0x00, 0x01};

    if (!in || in_size <= 0 || !out || out_cap <= 0 || !out_size)
    {
        return -1;
    }

    if (extra && extra->length_size > 0)
    {
        length_size = extra->length_size;
    }

    /* 先判断 sample 内部是否已经带 SPS/PPS，避免重复插入 */
    while ((r = avcc_next(in, in_size, &pos, length_size, &nal, &nal_size)) == 1)
    {
        int type = nal[0] & 0x1F;

        if (type == H264_NAL_SPS)
        {
            has_sps = 1;
        }
        else if (type == H264_NAL_PPS)
        {
            has_pps = 1;
        }
    }

    if (r < 0)
    {
        return -1;
    }

    if (keyframe && extra)
    {
        if (!has_sps)
        {
            for (i = 0; i < extra->sps_count; i++)
            {
                int len = extra->sps_len[i];

                if (len <= 0)
                {
                    continue;
                }

                if (written > out_cap - 4 || len > out_cap - written - 4)
                {
                    return -1;
                }

                memcpy(out + written, start_code, 4);
                written += 4;
                memcpy(out + written, extra->sps[i], (size_t)len);
                written += len;
            }
        }

        if (!has_pps)
        {
            for (i = 0; i < extra->pps_count; i++)
            {
                int len = extra->pps_len[i];

                if (len <= 0)
                {
                    continue;
                }

                if (written > out_cap - 4 || len > out_cap - written - 4)
                {
                    return -1;
                }

                memcpy(out + written, start_code, 4);
                written += 4;
                memcpy(out + written, extra->pps[i], (size_t)len);
                written += len;
            }
        }
    }

    pos = 0;
    while ((r = avcc_next(in, in_size, &pos, length_size, &nal, &nal_size)) == 1)
    {
        if (written > out_cap - 4 || nal_size > out_cap - written - 4)
        {
            return -1;
        }

        memcpy(out + written, start_code, 4);
        written += 4;
        memcpy(out + written, nal, (size_t)nal_size);
        written += nal_size;
    }

    if (r < 0 || written <= 0)
    {
        return -1;
    }

    *out_size = written;
    return 0;
}
