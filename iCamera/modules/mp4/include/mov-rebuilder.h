#ifndef __MOV_REBUILDER_H__
#define __MOV_REBUILDER_H__

typedef struct mov_rebuilder_t mov_rebuilder_t;

#ifdef __cplusplus
extern "C" {
#endif

/// @param[in] flags mov flags, such as: MOV_FLAG_FASTSTART, see more @mov-format.h
mov_rebuilder_t* mov_rebuilder_create(const struct mov_buffer_t* buffer, void* param, int flags);
void mov_rebuilder_destroy(struct mov_rebuilder_t* rebuilder);

int mov_rebuilder_add_audio(struct mov_rebuilder_t* rebuilder, uint8_t object, int channel_count, int bits_per_sample, int sample_rate, const void* extra_data, size_t extra_data_size);
int mov_rebuilder_add_video(struct mov_rebuilder_t* rebuilder, uint8_t object, int width, int height, const void* extra_data, size_t extra_data_size);
int mov_rebuilder_add_subtitle(struct mov_rebuilder_t* rebuilder, uint8_t object, const void* extra_data, size_t extra_data_size);

/// Write audio/video stream
/// raw AAC data, don't include ADTS/AudioSpecificConfig
/// H.264/H.265 MP4 format, replace start code(0x00000001) with NALU size
/// @param[in] track return by mov_writer_add_audio/mov_writer_add_video
/// @param[in] bytes data buffer size
/// @param[in] pts timestamp in millisecond
/// @param[in] dts timestamp in millisecond
/// @param[in] flags MOV_AV_FLAG_XXX, such as: MOV_AV_FLAG_KEYFREAME, see more @mov-format.h
/// @return 0-ok, other-error
int mov_rebuilder_write(struct mov_rebuilder_t* rebuilder, int track, size_t data_bytes, int64_t pts, int64_t dts, int flags);

#ifdef __cplusplus
}
#endif

#endif // end of __MOV_REBUILDER_H__

