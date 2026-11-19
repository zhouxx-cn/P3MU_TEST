/* 函数接口的简单封装 */

#ifndef __MP4_PACKET_H__
#define __MP4_PACKET_H__

#include "mov-reader.h"
#include "mov-buffer.h"
#include "mov-rebuilder.h"

typedef struct mp4_writer_t mp4_writer_t;
typedef struct mp4_reader_t mp4_reader_t;
typedef struct mp4_rebuilder_t mp4_rebuilder_t;

#ifdef __cplusplus
extern "C" {
#endif

// writer
mp4_writer_t* mp4_writer_create(int is_fmp4, const struct mov_buffer_t *buffer, void* param, int flags);
void mp4_writer_destroy(mp4_writer_t* mp4);
int mp4_writer_add_audio(mp4_writer_t* mp4, uint8_t object, int channel_count, int bits_per_sample, int sample_rate, const void* extra_data, size_t extra_data_size);
int mp4_writer_add_video(mp4_writer_t* mp4, uint8_t object, int width, int height, const void* extra_data, size_t extra_data_size);
int mp4_writer_add_subtitle(mp4_writer_t* mp4, uint8_t object, const void* extra_data, size_t extra_data_size);
int mp4_writer_add_cover(struct mp4_writer_t* mp4, const int8_t* cover);
int mp4_writer_write(mp4_writer_t* mp4, int track, const void* data, size_t bytes, int64_t pts, int64_t dts, int flags);
int mp4_writer_save_segment(mp4_writer_t* mp4);
int mp4_writer_init_segment(mp4_writer_t* mp4);
int mp4_writer_pos(struct mp4_writer_t* mp4, size_t *pos);

// reader
mp4_reader_t* mp4_reader_create(const struct mov_buffer_t* buffer, void* param);
void mp4_reader_destroy(mp4_reader_t* mp4);
int mp4_reader_getinfo(mp4_reader_t* mp4, struct mov_reader_trackinfo_t *ontrack, void* param);
uint64_t mp4_reader_getduration(mp4_reader_t* mp4);
uint32_t mp4_reader_get_sample_count(mp4_reader_t* mp4);
int mp4_reader_read(mp4_reader_t* mp4, void* buffer, size_t bytes, mov_reader_onread onread, void* param);
int mp4_reader_seek(mp4_reader_t* mp4, int64_t* timestamp);

// repair and rebuilder
struct mp4_rebuilder_t* mp4_rebuilder_create(const struct mov_buffer_t *buffer, void* param, int flags);
void mp4_rebuilder_destroy(struct mp4_rebuilder_t* mp4);
int mp4_rebuilder_add_audio(struct mp4_rebuilder_t* mp4, uint8_t object, int channel_count, int bits_per_sample, int sample_rate, const void* extra_data, size_t extra_data_size);
int mp4_rebuilder_add_video(struct mp4_rebuilder_t* mp4, uint8_t object, int width, int height, const void* extra_data, size_t extra_data_size);
int mp4_rebuilder_add_subtitle(struct mp4_rebuilder_t* mp4, uint8_t object, const void* extra_data, size_t extra_data_size);
int mp4_rebuilder_write(struct mp4_rebuilder_t* mp4, int track, size_t data_bytes, int64_t pts, int64_t dts, int flags);

#ifdef __cplusplus
}
#endif

#endif // end of __MP4_PACKET_H__
