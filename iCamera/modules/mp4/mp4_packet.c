#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#include "mov-reader.h"
#include "mov-writer.h"
#include "mov-rebuilder.h"
#include "mov-udta.h"
#include "fmp4-writer.h"
#include "mp4_packet.h"

#define MP4_DBG(format, args...) do{printf("\033[1;32m[MP4][%s-%d-%s]\033[m" format, __FILE__, __LINE__, __FUNCTION__, ##args);}while(0)
#define MP4_INF(format, args...) do{printf("\033[1;35m[MP4][%s-%d-%s]\033[m" format, __FILE__, __LINE__, __FUNCTION__, ##args);}while(0)
#define MP4_ERR(format, args...) do{printf("\033[1;31m[MP4][%s-%d-%s]\033[m" format, __FILE__, __LINE__, __FUNCTION__, ##args);}while(0)
#define MP4_SYS(format, args...) do{printf("\033[1;31m[MP4][%s-%d-%s](%s) \033[m" format, __FILE__, __LINE__, __FUNCTION__, strerror(errno), ##args);}while(0)

#define MP4_FCLOSE(x)   do{if (x) fclose(x);x = NULL;}while(0);
#define MP4_FREE(x)     do{if (x) free(x);x = NULL;}while(0);

#define MAX_COVER_IMAGE_SIZE   (512*1024)

#define USE_ALIGN_BUFFER

static const uint8_t g_s_jpg_file_head[] = {0xff, 0xd8, 0xff};
static const uint8_t g_s_png_file_head[] = {0x89, 0x50, 0x4e, 0x47, 0x0d, 0xa0, 0x1a, 0x0a};
//static const uint8_t g_s_bmp_file_head[] = {0x42, 0x4d};

struct mp4_writer_t
{
    int is_fmp4;
    union
    {
        fmp4_writer_t *fmp4;  // fmp4_write
        mov_writer_t *mov;    // mp4_write
    } u;

    uint8_t *udat;
    size_t udat_size;
};

struct mp4_reader_t          // mp4_reader
{
    mov_reader_t *mov;
};

struct mp4_rebuilder_t          // mp4_builder
{
    mov_rebuilder_t *rebuilder_mov;
};

struct mp4_writer_t* mp4_writer_create(int is_fmp4, const struct mov_buffer_t *buffer, void* param, int flags)
{
    if (!buffer || !param)
    {
        MP4_ERR("NULL POINTER EXCEPTION!\n");
        return NULL;
    }

    struct mp4_writer_t *mp4 = (struct mp4_writer_t *)calloc(1, sizeof(struct mp4_writer_t));    
    mp4->is_fmp4 = is_fmp4;    
    if (is_fmp4)
    {
        mp4->u.fmp4 = fmp4_writer_create(buffer, param, flags);
    }
    else
    {
#ifdef USE_ALIGN_BUFFER
        mp4->u.mov = mov_writer_align_create(buffer, param, flags);
#else
        mp4->u.mov = mov_writer_create(buffer, param, flags);
#endif
    }
    return mp4;
}

void mp4_writer_destroy(struct mp4_writer_t* mp4)
{
    if (!mp4)
    {
        MP4_ERR("NULL POINTER EXCEPTION!\n");
        return;
    }

    if (mp4->is_fmp4)
    {
        fmp4_writer_destroy(mp4->u.fmp4);
    }
    else
    {
#ifdef USE_ALIGN_BUFFER
        mov_writer_align_destroy(mp4->u.mov);
#else
        mov_writer_destroy(mp4->u.mov);
#endif
    }
    MP4_FREE(mp4->udat);
    MP4_FREE(mp4);
}

int mp4_writer_add_audio(struct mp4_writer_t* mp4, uint8_t object, int channel_count, int bits_per_sample, int sample_rate, const void* extra_data, size_t extra_data_size)
{
    if (!mp4)
    {
        MP4_ERR("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    if (mp4->is_fmp4)
    {
        return fmp4_writer_add_audio(mp4->u.fmp4, object, channel_count, bits_per_sample, sample_rate, extra_data, extra_data_size);
    }
    else
    {
        return mov_writer_add_audio(mp4->u.mov, object, channel_count, bits_per_sample, sample_rate, extra_data, extra_data_size);
    }
}

int mp4_writer_add_video(struct mp4_writer_t* mp4, uint8_t object, int width, int height, const void* extra_data, size_t extra_data_size)
{
    if (!mp4)
    {
        MP4_ERR("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    if (mp4->is_fmp4)
    {
        return fmp4_writer_add_video(mp4->u.fmp4, object, width, height, extra_data, extra_data_size);
    }
    else
    {
        return mov_writer_add_video(mp4->u.mov, object, width, height, extra_data, extra_data_size);
    }
}

int mp4_writer_add_subtitle(struct mp4_writer_t* mp4, uint8_t object, const void* extra_data, size_t extra_data_size)
{
    if (!mp4)
    {
        MP4_ERR("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    if (mp4->is_fmp4)
    {
        return fmp4_writer_add_subtitle(mp4->u.fmp4, object, extra_data, extra_data_size);
    }
    else
    {
        return mov_writer_add_subtitle(mp4->u.mov, object, extra_data, extra_data_size);
    }
}

int mp4_writer_add_cover(struct mp4_writer_t* mp4, const int8_t* cover)
{
    if (!mp4 || !cover)
    {
        MP4_ERR("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    int ret = -1;

    FILE* cover_fp = fopen(cover, "rb");
	if (!cover_fp)
	{
	    MP4_SYS("Fail to open %s\n", cover);
		return -1;
	}

    // cover file size is limite MAX_COVER_IMAGE_SIZE
    fseek(cover_fp, 0, SEEK_END);
    size_t file_len = ftell(cover_fp);
    fseek(cover_fp, 0, SEEK_SET);
    if (file_len > MAX_COVER_IMAGE_SIZE)
    {
	    MP4_ERR("Cover file %s size is %u over %d bytes!\n", cover, file_len, MAX_COVER_IMAGE_SIZE);
        MP4_FCLOSE(cover_fp);
        return -1;
    }

    uint8_t* cover_buff = (uint8_t *)calloc(MAX_COVER_IMAGE_SIZE, sizeof(uint8_t));
    if (NULL == cover_buff)
    {
        MP4_SYS("Fail to calloc cover buff!\n");
        MP4_FCLOSE(cover_fp);
        return -1;
    }

	size_t n = fread(cover_buff, 1, MAX_COVER_IMAGE_SIZE, cover_fp);
    MP4_FCLOSE(cover_fp);

    // cover only support jpg and png format
    if (!memcmp(cover_buff, g_s_jpg_file_head, 3) 
     && !memcmp(cover_buff, g_s_png_file_head, 4))
    {
        MP4_ERR("Cover format error! current mp4 file cover image is only support jpg and png!\n");
        MP4_FREE(cover_buff);
        return -1;
    }

    if (NULL == mp4->udat)
    {
        mp4->udat = (uint8_t *)calloc(MAX_COVER_IMAGE_SIZE, sizeof(uint8_t));
        if (NULL == mp4->udat)
        {
            MP4_SYS("Fail to calloc cover buff!\n");
            MP4_FREE(cover_buff);
            return -1;
        }
    }

    struct mov_udta_meta_t meta = {0};
    meta.cover = cover_buff;
    meta.cover_size = n;
    n = mov_udta_meta_write(&meta, mp4->udat, MAX_COVER_IMAGE_SIZE);
    MP4_FREE(cover_buff);

    if (n > MAX_COVER_IMAGE_SIZE)
    {
        MP4_ERR("Cover image write size %u over buff max size %d\n", n, MAX_COVER_IMAGE_SIZE);
        MP4_FREE(mp4->udat);
        return -1;
    }

    mp4->udat_size = n;

    if (mp4->is_fmp4)
    {
        ret = fmp4_writer_add_udta(mp4->u.fmp4, mp4->udat, n);
    }
    else
    {
        ret = mov_writer_add_udta(mp4->u.mov, mp4->udat, n);
    }

    return ret;
}

int mp4_writer_write(struct mp4_writer_t* mp4, int track, const void* data, size_t bytes, int64_t pts, int64_t dts, int flags)
{
    if (!mp4)
    {
        MP4_ERR("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    if (mp4->is_fmp4)
    {
        return fmp4_writer_write(mp4->u.fmp4, track, data, bytes, pts, dts, flags);
    }
    else
    {
#ifdef USE_ALIGN_BUFFER
        return mov_writer_align_write(mp4->u.mov, track, data, bytes, pts, dts, flags);
#else
        return mov_writer_write(mp4->u.mov, track, data, bytes, pts, dts, flags);
#endif
    }
}

int mp4_writer_save_segment(struct mp4_writer_t* mp4)
{
    if (!mp4)
    {
        MP4_ERR("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    if (mp4->is_fmp4)
    {
        return fmp4_writer_save_segment(mp4->u.fmp4);
    }
    else
    {
        return -1;
    }
}

int mp4_writer_init_segment(struct mp4_writer_t* mp4)
{
    if (!mp4)
    {
        MP4_ERR("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    if (mp4->is_fmp4)
    {
        return fmp4_writer_init_segment(mp4->u.fmp4);
    }
    else
    {
        return -1;
    }
}

int mp4_writer_pos(struct mp4_writer_t* mp4, size_t *pos)
{
    if (!mp4 || !pos)
    {
        MP4_ERR("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    if (mp4->is_fmp4)
    {
        return fmp4_writer_pos(mp4->u.fmp4, pos);
    }
    else
    {
#ifdef USE_ALIGN_BUFFER
        return mov_writer_align_pos(mp4->u.mov, pos);
#else
        return mov_writer_pos(mp4->u.mov, pos);
#endif
    }
}

/*---------------------------------------------------------------------------------------------*/

struct mp4_reader_t* mp4_reader_create(const struct mov_buffer_t* buffer, void* param)
{
    if (!buffer || !param)
    {
        MP4_ERR("NULL POINTER EXCEPTION!\n");
        return NULL;
    }

    struct mp4_reader_t *mp4 = (struct mp4_reader_t *)calloc(1, sizeof(struct mp4_reader_t));
    mp4->mov = mov_reader_create(buffer, param);
    return mp4;
}

void mp4_reader_destroy(struct mp4_reader_t* mp4)
{
    if (mp4)
    {
        mov_reader_destroy(mp4->mov);
        MP4_FREE(mp4);
    }
    mp4 = NULL;
}

int mp4_reader_getinfo(struct mp4_reader_t* mp4, struct mov_reader_trackinfo_t *ontrack, void* param)
{
    if (!mp4)
    {
        MP4_ERR("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    return mov_reader_getinfo(mp4->mov, ontrack, param);
}

uint64_t mp4_reader_getduration(struct mp4_reader_t* mp4)
{
    if (!mp4)
    {
        MP4_ERR("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    return mov_reader_getduration(mp4->mov);
}

uint32_t mp4_reader_get_sample_count(struct mp4_reader_t* mp4)
{
    if (!mp4)
    {
        MP4_ERR("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    return mov_reader_get_sample_count(mp4->mov);
}

int mp4_reader_read(struct mp4_reader_t* mp4, void* buffer, size_t bytes, mov_reader_onread onread, void* param)
{
    if (!mp4 || !buffer)
    {
        MP4_ERR("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    return mov_reader_read(mp4->mov, buffer, bytes, onread, param);
}

int mp4_reader_seek(struct mp4_reader_t* mp4, int64_t *timestamp)
{
    if (!mp4 || !timestamp)
    {
        MP4_ERR("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    return mov_reader_seek(mp4->mov, timestamp);
}

/*---------------------------------------------------------------------------------------------*/

struct mp4_rebuilder_t* mp4_rebuilder_create(const struct mov_buffer_t *buffer, void* param, int flags)
{
    if (!buffer || !param)
    {
        MP4_ERR("NULL POINTER EXCEPTION!\n");
        return NULL;
    }

    struct mp4_rebuilder_t *mp4 = (struct mp4_rebuilder_t *)calloc(1, sizeof(struct mp4_rebuilder_t));    
    mp4->rebuilder_mov = mov_rebuilder_create(buffer, param, flags);
    return mp4;
}

void mp4_rebuilder_destroy(struct mp4_rebuilder_t* mp4)
{
    if (!mp4)
    {
        MP4_ERR("NULL POINTER EXCEPTION!\n");
        return;
    }

    mov_rebuilder_destroy(mp4->rebuilder_mov);
    MP4_FREE(mp4);
}

int mp4_rebuilder_add_audio(struct mp4_rebuilder_t* mp4, uint8_t object, int channel_count, int bits_per_sample, int sample_rate, const void* extra_data, size_t extra_data_size)
{
    if (!mp4)
    {
        MP4_ERR("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    return mov_rebuilder_add_audio(mp4->rebuilder_mov, object, channel_count, bits_per_sample, sample_rate, extra_data, extra_data_size);
}

int mp4_rebuilder_add_video(struct mp4_rebuilder_t* mp4, uint8_t object, int width, int height, const void* extra_data, size_t extra_data_size)
{
    if (!mp4)
    {
        MP4_ERR("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    return mov_rebuilder_add_video(mp4->rebuilder_mov, object, width, height, extra_data, extra_data_size);
}

int mp4_rebuilder_add_subtitle(struct mp4_rebuilder_t* mp4, uint8_t object, const void* extra_data, size_t extra_data_size)
{
    if (!mp4)
    {
        MP4_ERR("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    return mov_rebuilder_add_subtitle(mp4->rebuilder_mov, object, extra_data, extra_data_size);
}

int mp4_rebuilder_write(struct mp4_rebuilder_t* mp4, int track, size_t data_bytes, int64_t pts, int64_t dts, int flags)
{
    if (!mp4)
    {
        MP4_ERR("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    return mov_rebuilder_write(mp4->rebuilder_mov, track, data_bytes, pts, dts, flags);
}
