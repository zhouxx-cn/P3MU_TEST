
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>
#include <utime.h>
#include <pthread.h>
#include <semaphore.h>
#include <assert.h>
#include <fcntl.h>
#include <dirent.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/statvfs.h>

#include "mov-format.h"
#include "mpeg4-avc.h"
#include "mpeg4-hevc.h"
#include "mpeg4-aac.h"
#include "mov-reader.h"

#include "mp4_packet.h"
#include "mp4.h"

#define mp4_dbg(format, args...) do{printf("\033[1;32m[MP4][%s-%d-%s]\033[m" format, __FILE__, __LINE__, __FUNCTION__, ##args);}while(0)
#define mp4_inf(format, args...) do{printf("\033[1;35m[MP4][%s-%d-%s]\033[m" format, __FILE__, __LINE__, __FUNCTION__, ##args);}while(0)
#define mp4_err(format, args...) do{printf("\033[1;31m[MP4][%s-%d-%s]\033[m" format, __FILE__, __LINE__, __FUNCTION__, ##args);}while(0)
#define mp4_sys(format, args...) do{printf("\033[1;31m[MP4][%s-%d-%s](%s) \033[m" format, __FILE__, __LINE__, __FUNCTION__, strerror(errno), ##args);}while(0)

#define safe_fclose(x)   do{if (x) fclose(x);x = NULL;}while(0);
#define safe_free(x)     do{if (x) free(x);x = NULL;}while(0);

#define MAX_EXTRA_DATA_SIZE (64*1024)

/* 打印精确到毫秒的时间 */
#define _debug_timeprint() \
    do { \
        struct timeval tvTmp = {}; \
        struct tm stmTmp = {}; \
        char szDTime[64] = {}; \
        char szResult[64] = {}; \
        \
        gettimeofday (& tvTmp, NULL); \
        gmtime_r (& tvTmp.tv_sec, & stmTmp); \
        strftime (szDTime, sizeof(szDTime), "%F %T", &stmTmp); \
        snprintf (szResult, sizeof(szResult), "%s.%03d", szDTime, (int)(tvTmp.tv_usec / 1000)); \
        \
        mp4_dbg("timestamp[%s %d %s] %s\n", __FILE__,__LINE__,__FUNCTION__, szResult); \
    } while (0)

/*
 *  @enum   eNALUnitFlag
 *  @brief  NAL片段标识
 */
typedef enum
{
    eNALUnitIDR       = 0x05,  /*!< H264 IDR  */
    eNALUnitSEI       = 0x06,  /*!< H264 SEI  */
    eNALUnitSPS       = 0x07,  /*!< H264 SPS  */
    eNALUnitPPS       = 0x08,  /*!< H264 PPS  */
    eNALUnitHVEVCIDR  = 0x13,  /*!< HEVC IDR  */
    eNALUnitHVEVCVPS  = 0x20,  /*!< HEVC VPS  */
    eNALUnitHVEVCSPS  = 0x21,  /*!< HEVC SPS  */
    eNALUnitHVEVCPPS  = 0x22,  /*!< HEVC PPS  */
    eNALUnitHVEVCSEI  = 0x27,  /*!< HEVC SEI  */
}eNALUnitFlag;

struct h26x_nalu_unit_t
{
    uint8_t type;
    size_t size;
    uint8_t *data;
};

typedef struct h26x_data_info_s{
    size_t data_size;
    size_t pos_offset;
    uint8_t is_key_frame;    
}h26x_frame_info_t;

typedef struct fd_av_s
{
    FILE *fd; // first field;
    int stime;
    int size;
    int b_raw_mov_data;

    // track;
    int vtrack;
    int atrack;

    union { 
        mp4_writer_t *w;
        mp4_reader_t *r;
    };

    // mp4-frame buffer;
    int s_length; 
    uint8_t *s_buffer;

    // out-mp4-frame-info;
    int type;                            // FRM_TYPE
    int64_t pts_ms;                      // pts ms;
    int64_t dts_ms;                      // dts ms;
    rec_v_info_t v;                      // media info;
    rec_a_info_t a;                      // media info;

    // out-mp4-frame-buffer;
    int *out_size;                       // out size;
    uint8_t *out_buffer;                 // out buffer;

    int vobject, aobject;                // MOV_OBJECT;
    union {
        struct mpeg4_hevc_t s_hevc;       // extra
        struct mpeg4_avc_t s_avc;         // extra
    };
    union {
        struct mpeg4_aac_t s_aac;         // extra
    };
}fd_av_t;

typedef struct fd_rebuild_s
{
    FILE *fp;
    int flag;
    mp4_rebuilder_t *b;
}fd_rebuild_t;


/* 单帧数据里面最大的杂糅的帧数 */
#define MAX_MIX_FRAME_NUM   (5)

#define READ_SIZE           (8)
#define SWAP_EDIAN_U32(X)   (((0x000000ff&X)<<24)|((0x0000ff00&X)<<8)|((0x00ff0000&X)>>8)|((0xff000000&X)>>24))
#define READ_MAX_BUF   (64*1024) // 64KB读缓冲区大小
static int mov_file_read(void* fp, void* data, uint64_t bytes)
{
    if (bytes == fread(data, 1, bytes, (FILE*)fp))
    {
        return 0;
    }
	return (0 != ferror((FILE*)fp) ? ferror((FILE*)fp) : -1) /*EOF*/;
}

static int mov_file_write(void* fp, const void* data, uint64_t bytes)
{
	return (bytes == fwrite(data, 1, bytes, (FILE*)fp) ? 0 : ferror((FILE*)fp));
}

static int mov_file_seek(void* fp, uint64_t offset)
{
	return (fseek((FILE*)fp, offset, SEEK_SET));
}

static int mov_file_seek_end(void* fp, uint64_t offset)
{
	return (fseek((FILE*)fp, offset, SEEK_END));
}

static uint64_t mov_file_tell(void* fp)
{
	return (ftell((FILE*)fp));
}

/* 由于只会存在一个写MP4的操作，故可以使用全局变量 */
static struct mov_buffer_t s_io = {
    mov_file_read,
    mov_file_write,
    mov_file_seek,
    mov_file_seek_end,
    mov_file_tell,
};

static const struct mov_buffer_t* mov_file_buffer(void)
{
	return &s_io;
}

typedef struct _TMp4FileWriteOpsPriv
{
    fd_av_t *_hdl;
    uint32_t u32_video_extra_data_size;
    uint32_t u32_audio_extra_data_size;
    uint8_t  u8_video_extra_data_buf[MAX_EXTRA_DATA_SIZE];
    uint8_t  u8_audio_extra_data_buf[MAX_EXTRA_DATA_SIZE];
}TMp4FileWriteOpsPriv;

typedef struct _TMp4FileReadOpsPriv
{
    fd_av_t *_hdl;
}TMp4FileReadOpsPriv;

typedef struct _TMp4FileRebuildOpsPriv
{
    fd_rebuild_t *_hdl;
}TMp4FileRebuildOpsPriv;

static int h26x_read_one_nalu(uint8_t is_hevc, const uint8_t *buffer, size_t bytes, size_t offSet, struct h26x_nalu_unit_t* nalu)
{
    if (!buffer || !nalu)
    {
        mp4_err("NULL POINTER EXCEPTION!\n");
        return -1;
    }

	size_t i = offSet;
	while (i < (bytes-3))
	{
		if (buffer[i + 0] == 0x00 && buffer[i + 1] == 0x00 && buffer[i + 2] == 0x00 && buffer[i + 3] == 0x01)
		{
			size_t pos = i + 4;
			while (pos < (bytes-3))
			{  
				if(buffer[pos + 0] == 0x00 && buffer[pos + 1] == 0x00 && buffer[pos + 2] == 0x00 && buffer[pos + 3] == 0x01)
				{
					break;  
				}
				pos = pos + 1;
			}

			if (pos == (bytes-3))
			{  
				nalu->size = bytes - i;
			}
            else
            {
				nalu->size = (pos - 4) - i;
			}

			if (is_hevc)
			{
                nalu->type = buffer[i + 4] & 0x7e;
                nalu->type >>= 1;
                nalu->data = (unsigned char*)&buffer[i + 4];
			}
			else
			{
                nalu->type = buffer[i + 4] & 0x1f;
                nalu->data = (uint8_t*)&buffer[i + 4];
			}
			return (nalu->size + i + 4 - offSet);
		}
		i += 1;
	}  
	return 0;
}

/* 从给定的帧缓冲里面，获取当前真实存在的H26X信息 */
static int find_frames_info(int32_t is_hevc, const uint8_t *buffer, size_t buffer_size, h26x_frame_info_t *frames_info, int32_t *frame_num)
{
    if (!buffer || !frames_info || !frame_num)
    {
        mp4_err("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    *frame_num = 0;
    size_t pos_offset = 0;
    size_t read_len = 0;

    struct h26x_nalu_unit_t nalu = {0, 0, NULL};

    while ((read_len = h26x_read_one_nalu(is_hevc, buffer, buffer_size, pos_offset, &nalu)))
    {
        if (is_hevc)
        {
            if(nalu.type != eNALUnitHVEVCVPS 
            && nalu.type != eNALUnitHVEVCSPS 
            && nalu.type != eNALUnitHVEVCPPS 
            && nalu.type != eNALUnitHVEVCSEI)
            {
                if (0 == (*frame_num))
                {
                    frames_info[*frame_num].pos_offset = 0x0;
                }
                else
                {
                    frames_info[*frame_num].pos_offset = pos_offset;
                }

                if (eNALUnitHVEVCIDR == nalu.type)
                {
                    frames_info[*frame_num].is_key_frame = 0x1;
                }
                (*frame_num) ++;
            }
        }
        else
        {
            if(nalu.type != eNALUnitSPS 
            && nalu.type != eNALUnitPPS 
            && nalu.type != eNALUnitSEI)
            {
                if (0 == (*frame_num))
                {
                    frames_info[*frame_num].pos_offset = 0x0;
                }
                else
                {
                    frames_info[*frame_num].pos_offset = pos_offset;
                }

                if (eNALUnitIDR == nalu.type)
                {
                    frames_info[*frame_num].is_key_frame = 0x1;
                }
                (*frame_num) ++;
            }
        }
        pos_offset += read_len;
    }

    /* 计算H26X数据帧的长度 */
    if ((*frame_num) > 0)
    {    
        int32_t i = 0;
        int32_t frame_index = (*frame_num) - 1;
        if (0 == frame_index)
        {
            frames_info[frame_index].data_size = buffer_size;
        }
        else
        {
            frames_info[0].data_size = frames_info[1].pos_offset;
            for (i=1; i<(*frame_num); i++)
            {
                frames_info[i].data_size = i + 1 < (*frame_num) 
                                            ? (frames_info[i+1].pos_offset - frames_info[i].pos_offset) 
                                            : (buffer_size-frames_info[i].pos_offset);
            }
        }
    }

    return (*frame_num);
}

/*@ breif Mp4WriteOpen 打开需要写的MP4视频，不存在则创建
 *@ param [in] This 句柄
 *@ param [in] name MP4录像文件
 *@ param [in] b_fmp4 是否为FMP4格式 0:标准MP4格式 1:FMP4格式
 *@ param [in] b_raw_mov_data 写入的数据是否是纯的MP4数据流(用作修复MP4使用)
 *@ param [in] b_fast_start 是否是快速MP4模式(moov提前，仅标准MP4有效)
 *@ return 成功返回0，失败返回-1
 */
static int Mp4WriteOpen(struct _TMp4FileWriteOps *This, const char *name, int b_fmp4, int b_raw_mov_data, int b_fast_start)
{
    if (!This || !name)
    {
        mp4_err("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    This->Priv->_hdl = (fd_av_t*)calloc(1, sizeof(fd_av_t));

    fd_av_t *_hdl = This->Priv->_hdl;
    if (!_hdl)
    {
        mp4_sys("Fail to calloc\n");
        return -1;
    }

    _hdl->vtrack = -1;
    _hdl->atrack = -1;    
    _hdl->b_raw_mov_data = b_raw_mov_data;
    _hdl->fd = fopen(name, "wb+");
    if (!_hdl->fd)
    {
        mp4_sys("Fail to fopen %s\n", name);
        safe_free(_hdl);
        return -1;
    }

    int _b_fast_start  = b_fast_start ? 0x1 : 0x0;

    _hdl->w = mp4_writer_create(b_fmp4, mov_file_buffer(), _hdl->fd, _b_fast_start);
    _hdl->s_length = 400*1024;  // init size;
    _hdl->s_buffer = (uint8_t*)calloc(_hdl->s_length, 1);
    if (!_hdl->s_buffer)
    {
        mp4_sys("Fail to calloc!\n");
        safe_fclose(_hdl->fd);
        safe_free(_hdl);
        return -1;
    }

    return 0;
}

/*@ breif Mp4WriteClose 关闭写录像句柄
 *@ param [in] This 句柄
 *@ return 成功返回0，失败返回-1
 */
static int Mp4WriteClose(struct _TMp4FileWriteOps *This)
{
    if (!This || !This->Priv || !This->Priv->_hdl)
    {
        mp4_err("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    mp4_writer_destroy(This->Priv->_hdl->w);

    safe_fclose(This->Priv->_hdl->fd);
    safe_free(This->Priv->_hdl->s_buffer);
    safe_free(This->Priv->_hdl);

    return 0;
}

/*@ breif Mp4WriteFlush 将数据刷新到磁盘上(暂未实现)
 *@ param [in] This 句柄
 *@ return 成功返回0，失败返回-1
 */
static int Mp4WriteFlush(struct _TMp4FileWriteOps *This)
{
    if (!This || !This->Priv || !This->Priv->_hdl)
    {
        mp4_err("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    // TODO ..
    return 0;
}

/*@ breif Mp4WriteFrame 写一帧视频数据
 *@ param [in] This 句柄
 *@ param [in] buf 纯流数据buffer
 *@ param [in] size 流数据buffer大小
 *@ param [in] info 流信息
 *@ return 成功返回0，失败返回-1
 */
static int Mp4WriteFrame(struct _TMp4FileWriteOps *This, const char *buf, int size, const rec_rw_info_t *info)
{
    if (!This || !This->Priv || !This->Priv->_hdl || !buf || !info)
    {
        mp4_err("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    int ret = 0;

    fd_av_t *_hdl = This->Priv->_hdl;

    if (_hdl->fd)
    {
        if (FRM_VIDEO_P == info->type || FRM_VIDEO_I == info->type)
        {
            struct mpeg4_avc_t avc = {0};
            struct mpeg4_hevc_t hevc = {0};
            h26x_frame_info_t frames_info[MAX_MIX_FRAME_NUM] = {0};
            int32_t i = 0;
            int32_t mix_frame_num = 0;
            
            if(size > _hdl->s_length)
            {
                _hdl->s_length = size;
                _hdl->s_buffer = realloc(_hdl->s_buffer, _hdl->s_length);
            }

            //mp4_inf("type:%d, enc:%d, s_length:%d\n", info->type, info->enc, _hdl->s_length);
            if(ENC_H264 == info->enc)
            {
                if (_hdl->b_raw_mov_data)
                {
                    if(_hdl->vtrack == -1)
                    {
                        _hdl->vtrack = mp4_writer_add_video(_hdl->w, MOV_OBJECT_H264, info->v.w, info->v.h, This->Priv->u8_video_extra_data_buf, This->Priv->u32_video_extra_data_size);
                    }

                    if(_hdl->vtrack != -1)
                    {
                        mp4_writer_write(_hdl->w, _hdl->vtrack, buf, size, info->pts_ms, info->dts_ms, (info->type == FRM_VIDEO_I)? MOV_AV_FLAG_KEYFREAME : 0);
                    }
                }
                else
                {
                    find_frames_info(0, buf, size, frames_info, &mix_frame_num);

                    for (i=0; i<mix_frame_num; i++)
                    {
                        size_t n = h264_annexbtomp4(&avc, buf+frames_info[i].pos_offset, frames_info[i].data_size, _hdl->s_buffer, _hdl->s_length, NULL, NULL);

                        if(_hdl->vtrack == -1)
                        {
                            if (avc.nb_sps < 1 || avc.sps[0].bytes < 4) 
                            {
                                mp4_dbg("video wait for key frame [idc:%d, nb_sps:%d, sps:%d]\n", 
                                        avc.chroma_format_idc, avc.nb_sps, avc.sps[0].bytes);
                                return 0;
                            }
                            This->Priv->u32_video_extra_data_size = 0;
                            memset(This->Priv->u8_video_extra_data_buf, 0, sizeof(This->Priv->u8_video_extra_data_buf));
                            This->Priv->u32_video_extra_data_size = mpeg4_avc_decoder_configuration_record_save(&avc, This->Priv->u8_video_extra_data_buf, sizeof(This->Priv->u8_video_extra_data_buf));
                            assert(This->Priv->u32_video_extra_data_size > 0); // check buffer length
                            _hdl->vtrack = mp4_writer_add_video(_hdl->w, MOV_OBJECT_H264, info->v.w, info->v.h, This->Priv->u8_video_extra_data_buf, This->Priv->u32_video_extra_data_size);
                        }
                        if(_hdl->vtrack != -1)
                        {
                            //if(info->type == FRM_VIDEO_I) mp4_dbg("video write vtrack.\n");
                            mp4_writer_write(_hdl->w, _hdl->vtrack, _hdl->s_buffer, n, info->pts_ms, info->dts_ms, (frames_info[i].is_key_frame)? MOV_AV_FLAG_KEYFREAME : 0);
                        }
                    }
                }
            }
            else if(ENC_H265 == info->enc)
            {
                if (_hdl->b_raw_mov_data)
                { 
                    if(_hdl->vtrack == -1)
                    {
                        _hdl->vtrack = mp4_writer_add_video(_hdl->w, MOV_OBJECT_HEVC, info->v.w, info->v.h, This->Priv->u8_video_extra_data_buf, This->Priv->u32_video_extra_data_size);
                    }

                    if(_hdl->vtrack != -1)
                    {
                        mp4_writer_write(_hdl->w, _hdl->vtrack, buf, size, info->pts_ms, info->dts_ms, (info->type == FRM_VIDEO_I)? MOV_AV_FLAG_KEYFREAME : 0);
                    }
                }
                else
                {
                    find_frames_info(1, buf, size, frames_info, &mix_frame_num);

					//海思I帧尾部有未知的nalu,会导致花屏，暂认为是无用数据
					if (mix_frame_num > 1)
					{
						mix_frame_num = 1;
					}

                    for (i=0; i<mix_frame_num; i++)
                    {
                        size_t n = h265_annexbtomp4(&hevc, buf+frames_info[i].pos_offset, frames_info[i].data_size, _hdl->s_buffer, _hdl->s_length, NULL, NULL);
                        if(_hdl->vtrack == -1)
                        {
                            if (hevc.numOfArrays < 1)
                            {
                                return 0; // wait for key frame
                            }

                            This->Priv->u32_video_extra_data_size = 0;
                            memset(This->Priv->u8_video_extra_data_buf, 0, sizeof(This->Priv->u8_video_extra_data_buf));
                            This->Priv->u32_video_extra_data_size = mpeg4_hevc_decoder_configuration_record_save(&hevc, This->Priv->u8_video_extra_data_buf, sizeof(This->Priv->u8_video_extra_data_buf));
                            assert(This->Priv->u32_video_extra_data_size > 0); // check buffer length
                            _hdl->vtrack = mp4_writer_add_video(_hdl->w, MOV_OBJECT_HEVC, info->v.w, info->v.h, This->Priv->u8_video_extra_data_buf, This->Priv->u32_video_extra_data_size);
                        }
                        if(_hdl->vtrack != -1)
                        {
                            mp4_writer_write(_hdl->w, _hdl->vtrack, _hdl->s_buffer, n, info->pts_ms, info->dts_ms, (frames_info[i].is_key_frame)? MOV_AV_FLAG_KEYFREAME : 0);
                        }
                    }
                }
            }
            else
            {
                if(-1 == _hdl->vtrack)
                {
                    _hdl->vtrack = mp4_writer_add_video(_hdl->w, MOV_OBJECT_JPEG, info->v.w, info->v.h, "null", strlen("null"));
                }
                mp4_writer_write(_hdl->w, _hdl->vtrack, buf, size, info->pts_ms, info->dts_ms, (info->type == FRM_VIDEO_I)? MOV_AV_FLAG_KEYFREAME : 0);
            }
        }
        else if(FRM_AUDIO_1 == info->type)
        {
            if(ENC_G711A==info->enc || ENC_G711U==info->enc)
            {
                if(_hdl->atrack == -1)
                {
                    _hdl->atrack = mp4_writer_add_audio(_hdl->w, (info->enc==ENC_G711A)?MOV_OBJECT_G711a:MOV_OBJECT_G711u, info->a.chs, info->a.ss, info->a.sp, NULL, 0);
                }

                if(_hdl->atrack != -1)
                {
                    ret = mp4_writer_write(_hdl->w, _hdl->atrack, buf, size, info->pts_ms, info->dts_ms, 0);
                }
            }
            else if(ENC_AAC == info->enc)
            {
                int rate = 1;
                struct mpeg4_aac_t aac = {0};
                mpeg4_aac_adts_load((const uint8_t*)buf, size, &aac);

                if (-1 == _hdl->atrack)
                {
                    if (_hdl->b_raw_mov_data)
                    {
                    }
                    else
                    {
                        This->Priv->u32_audio_extra_data_size = 0;
                        memset(This->Priv->u8_audio_extra_data_buf, 0, sizeof(This->Priv->u8_audio_extra_data_buf));
                        This->Priv->u32_audio_extra_data_size = mpeg4_aac_audio_specific_config_save(&aac, This->Priv->u8_audio_extra_data_buf, sizeof(This->Priv->u8_audio_extra_data_buf));
                    }
                    rate = mpeg4_aac_audio_frequency_to((enum mpeg4_aac_frequency)aac.sampling_frequency_index);
                    _hdl->atrack = mp4_writer_add_audio(_hdl->w, MOV_OBJECT_AAC, aac.channel_configuration, 16, rate, This->Priv->u8_audio_extra_data_buf, This->Priv->u32_audio_extra_data_size);
                }
                if(-1 != _hdl->atrack)
                {
                    int framelen = ((buf[3] & 0x03) << 11) | (buf[4] << 3) | (buf[5] >> 5);
                    mp4_writer_write(_hdl->w, _hdl->atrack, buf + 7, framelen - 7, info->pts_ms, info->dts_ms, 0);
                }
            }
        }
        else
        {
            return -1;
        }
    }

    if (ret != 0)
    {
        mp4_dbg("info->type = %d, info->enc = %d, ret = %d\n", info->type, info->enc, ret);
    }

    //_hdl->size += size;
    mp4_writer_pos(_hdl->w, &_hdl->size);

    return ret;
}

/*@ breif Mp4WriteGetExtraData 获取文件的EXTRA数据(用于MP4的修复)
 *@ param [in] This 句柄
 *@ param [in] b_video 是否为视频的EXTRA, 1:video 0:audio
 *@ param [out] extra_data EXTRA的buffer
 *@ param [in] extra_data_size EXTRA的buffer大小
 *@ return 成功返回大小，失败返回-1
 */
static int Mp4WriteGetExtraData(struct _TMp4FileWriteOps *This, int b_video, uint8_t* extra_data, uint32_t extra_data_size)
{
    if (!This || !This->Priv || !extra_data)
    {
        mp4_err("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    memset(extra_data, 0, extra_data_size);

    if (b_video)
    {
        memcpy(extra_data, This->Priv->u8_video_extra_data_buf, This->Priv->u32_video_extra_data_size);
        return This->Priv->u32_video_extra_data_size;
    }
    else
    {
        memcpy(extra_data, This->Priv->u8_audio_extra_data_buf, This->Priv->u32_audio_extra_data_size);
        return This->Priv->u32_audio_extra_data_size;
    }
}

/*@ breif Mp4WriteSetExtraData 设置文件的EXTRA数据(用于修复MP4的extra数据)
 *@ param [in] This 句柄
 *@ param [in] b_video 是否为视频的EXTRA, 1:video 0:audio
 *@ param [in] extra_data EXTRA的buffer
 *@ param [in] extra_data_size EXTRA的buffer大小
 *@ return 成功返回0，失败返回-1
 */
static int Mp4WriteSetExtraData(struct _TMp4FileWriteOps *This, int b_video, const uint8_t* extra_data, uint32_t extra_data_size)
{
    if (!This || !This->Priv || !extra_data)
    {
        mp4_err("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    if (extra_data_size > MAX_EXTRA_DATA_SIZE)
    {
        mp4_err("extra_data_size %lu over limit size %d!\n", extra_data_size, MAX_EXTRA_DATA_SIZE);
        return -1;
    }

    if (b_video)
    {
        memset(This->Priv->u8_video_extra_data_buf, 0, sizeof(This->Priv->u8_video_extra_data_buf));
        memcpy(This->Priv->u8_video_extra_data_buf, extra_data, extra_data_size);
        This->Priv->u32_video_extra_data_size = extra_data_size;
    }
    else
    {
        memset(This->Priv->u8_audio_extra_data_buf, 0, sizeof(This->Priv->u8_audio_extra_data_buf));
        memcpy(This->Priv->u8_audio_extra_data_buf, extra_data, extra_data_size);
        This->Priv->u32_audio_extra_data_size = extra_data_size;
    }

    return 0;
}

/*@ breif Mp4WriteSubtitle MP4文件添加字幕
 *@ param [in] This 句柄
 *@ param [in] object 字幕对象，详见E_MP4_OBJ
 *@ param [in] object 字幕对象buffer
 *@ param [in] object 字幕对象buffer大小
 *@ return 成功返回添加的track数，失败返回-1
 */
static int Mp4WriteSubtitle(struct _TMp4FileWriteOps *This, uint8_t object, const void* extra_data, size_t extra_data_size)
{
    if (!This || !This->Priv || !This->Priv->_hdl || !extra_data)
    {
        mp4_err("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    return mp4_writer_add_subtitle(This->Priv->_hdl->w, object, extra_data, extra_data_size);
}

/*@ breif Mp4WriteCover MP4文件添加封面(图片只支持PNG和JPG)
 *@ param [in] This 句柄
 *@ param [in] cover_file 图片文件
 *@ return 成功返回0，失败返回-1
 */
static int Mp4WriteCover(struct _TMp4FileWriteOps *This, const char *cover_file)
{
    if (!This || !This->Priv || !This->Priv->_hdl || !cover_file)
    {
        mp4_err("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    return mp4_writer_add_cover(This->Priv->_hdl->w, cover_file);
}

/*@ breif Mp4WriteGetFileSize 获取文件大小
 *@ param [in] This 句柄
 *@ return 成功返回大小，失败返回-1
 */
static int Mp4WriteGetFileSize(struct _TMp4FileWriteOps *This)
{
    if (!This || !This->Priv || !This->Priv->_hdl)
    {
        mp4_err("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    return This->Priv->_hdl->size;
}

/*@ breif Mp4FileWriteOpsCreate 创建写MP4相关操作句柄
 *@ return 成功返回操作句柄，失败返回NULL
 */
TMp4FileWriteOps* Mp4FileWriteOpsCreate(void)
{
    TMp4FileWriteOps *This = (TMp4FileWriteOps *)calloc(1, sizeof(TMp4FileWriteOps));
    if (NULL == This)
    {
        mp4_sys("Fail to calloc TMp4FileWriteOps handler!\n");
        return NULL;
    }

    This->Priv = (struct _TMp4FileWriteOpsPriv *)calloc(1, sizeof(struct _TMp4FileWriteOpsPriv));
    if (NULL == This->Priv)
    {
        mp4_sys("Fail to calloc TMp4FileWriteOps Priv handler!\n");
        safe_free(This);
        return NULL;
    }
    This->Mp4WriteOpen = Mp4WriteOpen;
    This->Mp4WriteClose = Mp4WriteClose;
    This->Mp4WriteFlush = Mp4WriteFlush;
    This->Mp4WriteGetExtraData = Mp4WriteGetExtraData;
    This->Mp4WriteSetExtraData = Mp4WriteSetExtraData;
    This->Mp4WriteFrame = Mp4WriteFrame;
    This->Mp4WriteCover = Mp4WriteCover;
    This->Mp4WriteSubtitle = Mp4WriteSubtitle;
    This->Mp4WriteGetFileSize = Mp4WriteGetFileSize;

    return This;
}

/*@ breif Mp4FileWriteOpsDestroy 销毁MP4写操作句柄
 *@ return 成功返回0，失败返回-1
 */
int Mp4FileWriteOpsDestroy(TMp4FileWriteOps* This)
{
    if (!This || !This->Priv)
    {
        mp4_err("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    safe_free(This->Priv);
    safe_free(This);

    return 0;
}

/*************************************************************************************************************/

static void mov_video_info(void* param, uint32_t track, uint8_t object, int width, int height, const void* extra, size_t bytes)
{
    if (!param || !extra)
    {
        mp4_err("NULL POINTER EXCEPTION!\n");
        return;
    }

    fd_av_t *_hdl = (fd_av_t*)param;

    _hdl->vtrack = track;
    _hdl->vobject = object;
    if (MOV_OBJECT_H264 == object)
    {
        mpeg4_avc_decoder_configuration_record_load((const uint8_t*)extra, bytes, &_hdl->s_avc);
    }
    else if (MOV_OBJECT_HEVC == object)
    {
        mpeg4_hevc_decoder_configuration_record_load((const uint8_t*)extra, bytes, &_hdl->s_hevc);
    }
    else
    {
        ;//assert(0);
    }

    _hdl->v.w = width;
    _hdl->v.h = height;
    _hdl->v.fps = 0;
}

static void mov_audio_info(void* param, uint32_t track, uint8_t object, int channel_count, int bit_per_sample, int sample_rate, const void* extra, size_t bytes)
{
    if (!param)
    {
        mp4_err("NULL POINTER EXCEPTION!\n");
        return;
    }

    fd_av_t *_hdl = (fd_av_t*)param;
    _hdl->atrack = track;
    _hdl->aobject = object;

    if (MOV_OBJECT_G711a == _hdl->aobject || MOV_OBJECT_G711u == _hdl->aobject)
    {
        ;
    }
    else if (MOV_OBJECT_AAC == _hdl->aobject)
    {
        _hdl->s_aac.profile = MPEG4_AAC_LC;
        _hdl->s_aac.channel_configuration = channel_count;
        _hdl->s_aac.sampling_frequency_index = mpeg4_aac_audio_frequency_from(sample_rate);
    }

    _hdl->a.sp  = sample_rate;
    _hdl->a.ss  = bit_per_sample;
    _hdl->a.chs = channel_count;
}

static void onread(void* param, uint32_t track, const void* buffer, size_t bytes, int64_t pts, int64_t dts, int flags)
{
    if (!param || !buffer)
    {
        mp4_err("NULL POINTER EXCEPTION!\n");
        return;
    }

    fd_av_t *_hdl = (fd_av_t*)param;

    _hdl->type = 0;
    _hdl->pts_ms = pts;
    _hdl->dts_ms = dts;

    if(_hdl->vtrack == track)
    {
        if (MOV_OBJECT_H264 == _hdl->vobject)
        {
            int n = h264_mp4toannexb(&_hdl->s_avc, buffer, bytes, _hdl->out_buffer, *_hdl->out_size);
            *_hdl->out_size = n;
            _hdl->type = flags ? FRM_VIDEO_I : FRM_VIDEO_P; // I.P
        }
        else if(MOV_OBJECT_HEVC == _hdl->vobject)
        {
            int n = h265_mp4toannexb(&_hdl->s_hevc, buffer, bytes, _hdl->out_buffer, *_hdl->out_size);
            *_hdl->out_size = n;
            _hdl->type = flags ? FRM_VIDEO_I : FRM_VIDEO_P; // I.P
        }
        else if(MOV_OBJECT_JPEG == _hdl->vobject)
        {
            int n = bytes;
            memcpy(_hdl->out_buffer, buffer, n);
            *_hdl->out_size = n;
            _hdl->type = FRM_VIDEO_I; // JPEG
        }
    }
    else if(_hdl->atrack == track)
    {
        _hdl->type = FRM_AUDIO_1; // A

        if (MOV_OBJECT_G711a == _hdl->aobject || MOV_OBJECT_G711u == _hdl->aobject)
        {
            int n = bytes;
            memcpy(_hdl->out_buffer, buffer, n);
            *_hdl->out_size = n;
        }
        else if (MOV_OBJECT_AAC == _hdl->aobject)
        {
            uint8_t adts[32] = {0};
            int n = mpeg4_aac_adts_save(&_hdl->s_aac, bytes, adts, sizeof(adts));

            memcpy(_hdl->out_buffer, adts, n);
            memcpy(_hdl->out_buffer+n, buffer, bytes);
            *_hdl->out_size = n+bytes;
        }
    }
    else
    {
        ;//assert(0);
    }
}

/*@ breif Mp4ReadOpen 打开一个MP4文件，用作回放
 *@ param [in] This 句柄
 *@ param [in] name MP4文件
 *@ param [in] type 用作扩展视频文件格式(暂未用到)
 *@ param [out] media 当前MP4录像文件的信息
 *@ return 成功返回0, 失败返回-1
 */
static int Mp4ReadOpen(struct _TMp4FileReadOps *This, const char *name, int type, rec_media_info_t *media)
{
    if (!This || !This->Priv || !name || !media)
    {
        mp4_err("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    struct stat st = {0};
    fd_av_t *_hdl = NULL;
    This->Priv->_hdl = (fd_av_t*)calloc(1, sizeof(fd_av_t));
    _hdl = This->Priv->_hdl;

    if (!_hdl)
    {
        mp4_sys("Fail to calloc!\n");
        return -1;
    }
    
    _hdl->vtrack = -1;
    _hdl->atrack = -1;
    _hdl->fd = fopen(name, "rb");
    if (!_hdl->fd)
    {
        mp4_sys("Fail to fopen %s\n", name);
        safe_free(_hdl);
        return -1;
    }

    _hdl->r = mp4_reader_create(mov_file_buffer(), _hdl->fd);

    _hdl->s_length = 400*1024;  // init size;
    _hdl->s_buffer = (uint8_t*)calloc(_hdl->s_length, 1);
    if (!_hdl->s_buffer)
    {
        mp4_sys("Fail to calloc!\n");
        safe_fclose(_hdl->fd);
        safe_free(_hdl);
        return -1;
    }

    struct mov_reader_trackinfo_t info = { mov_video_info, mov_audio_info };
    mp4_reader_getinfo(_hdl->r, &info, _hdl);

    media->venc = (_hdl->vobject == MOV_OBJECT_H264)?ENC_H264:
                   (_hdl->vobject == MOV_OBJECT_HEVC)?ENC_H265:
                   (_hdl->vobject == MOV_OBJECT_JPEG)?ENC_MJPEG:
                   ENC_BUTT;

    media->aenc = (_hdl->aobject == MOV_OBJECT_G711a)?ENC_G711A:
                   (_hdl->aobject == MOV_OBJECT_G711u)?ENC_G711U:
                   (_hdl->aobject == MOV_OBJECT_AAC)?ENC_AAC:
                   ENC_BUTT;

    memcpy(&media->v, &_hdl->v, sizeof(rec_v_info_t));
    memcpy(&media->a, &_hdl->a, sizeof(rec_a_info_t));

    if(fstat(fileno(_hdl->fd), &st) == 0)
    {
        _hdl->size = st.st_size;
    }

    return 0;
}

/*@ breif Mp4ReadClose 关闭读录像句柄
 *@ param [in] This 读录像句柄
 *@ return 成功返回0，失败返回-1
 */
static int Mp4ReadClose(struct _TMp4FileReadOps *This)
{
    if (!This || !This->Priv)
    {
        mp4_err("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    fd_av_t *_hdl = This->Priv->_hdl;

    mp4_reader_destroy(_hdl->r);

    safe_fclose(_hdl->fd);
    safe_free(_hdl->s_buffer);
    safe_free(_hdl);

    return 0;
}

/*@ breif Mp4ReadFrame 读一帧视频数据
 *@ param [in] This 读录像句柄
 *@ param [out] buf 获取到的一帧数据
 *@ param [in] size 当前buffer的大小
 *@ param [out] get_size 获取到的实际buffer大小
 *@ param [out] info 当前帧数据的信息
 *@ return 成功返回0，错误返回其他
 */
static int Mp4ReadFrame(struct _TMp4FileReadOps *This, char *buf, int buf_size, int *get_size, rec_rw_info_t *info)
{
    if (!This || !This->Priv || !buf || !get_size || !info)
    {
        mp4_err("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    fd_av_t *_hdl = This->Priv->_hdl;
    int ret = 0;
    int buff_size_tmp = buf_size;

    _hdl->out_buffer = (uint8_t *)buf;
    _hdl->out_size  = &buff_size_tmp;

    while((ret = mp4_reader_read(_hdl->r, _hdl->s_buffer, _hdl->s_length, onread, _hdl)) == ENOMEM)
    {
        _hdl->s_length *= 2; 
        _hdl->s_buffer = realloc(_hdl->s_buffer, _hdl->s_length);
    }

    *get_size = buff_size_tmp;

    info->type = _hdl->type;
    info->enc = (_hdl->type == FRM_VIDEO_I || _hdl->type == FRM_VIDEO_P)?
                (_hdl->vobject == MOV_OBJECT_H264)?ENC_H264:
                (_hdl->vobject == MOV_OBJECT_HEVC)?ENC_H265:  
                (_hdl->vobject == MOV_OBJECT_JPEG)?ENC_MJPEG: ENC_BUTT:
                (_hdl->type == FRM_AUDIO_1)?
                (_hdl->aobject == MOV_OBJECT_G711a)?ENC_G711A:
                (_hdl->aobject == MOV_OBJECT_G711u)?ENC_G711U:
                (_hdl->aobject == MOV_OBJECT_AAC)?ENC_AAC: ENC_BUTT:
                ENC_BUTT;
    info->pts_ms = _hdl->pts_ms;
    info->dts_ms = _hdl->dts_ms;
    memcpy(&info->v, &_hdl->v, sizeof(rec_v_info_t));
    memcpy(&info->a, &_hdl->a, sizeof(rec_a_info_t));

    return ret;
}

/*@ breif Mp4ReadGetFileSize 获取文件大小
 *@ param [in] This 读录像句柄
 *@ return 成功返回大小，失败返回-1
 */
static int Mp4ReadGetFileSize(struct _TMp4FileReadOps *This)
{
    if (!This || !This->Priv)
    {
        mp4_err("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    fd_av_t *_hdl = This->Priv->_hdl;
    return _hdl->size;
}

/*@ breif Mp4ReadSeekSec 进行seek定位(单位: 秒)
 *@ param [in] This 读录像句柄
 *@ param [in] seek_sec seek的秒数
 *@ param [out] get_seek_sec 获取到实际定位的时间点
 *@ return 成功返回0，失败返回-1
 *@ note 由于MP4的定位，只能用相对时间定位，故传入的时间不能超过整个录像的时间
 */
static int Mp4ReadSeekSec(struct _TMp4FileReadOps *This, int seek_sec, int *get_seek_sec)
{
    if (!This || !This->Priv || !get_seek_sec)
    {
        mp4_err("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    if (0 == seek_sec)
    {
        return 0;
    }

    uint64_t seek_time_msec = 0;
    uint64_t duration = 0;
    fd_av_t *_hdl = This->Priv->_hdl;

    duration = mp4_reader_getduration(_hdl->r);
    seek_time_msec = (int64_t)(seek_sec * 1000);
    seek_time_msec = seek_time_msec > duration ? duration : seek_time_msec;

    mp4_reader_seek(_hdl->r, &seek_time_msec);

    *get_seek_sec = (int)(seek_time_msec/1000);

    return 0;
}

/*@ breif Mp4ReadSeekMSec 进行seek定位(单位: 毫秒)
 *@ param [in] This 读录像句柄
 *@ param [in] seek_msec seek的毫秒数
 *@ param [out] get_seek_msec 获取到实际定位的时间点
 *@ return 成功返回0，失败返回-1
 *@ note 由于MP4的定位，只能用相对时间定位，故传入的时间不能超过整个录像的时间
 */
static int Mp4ReadSeekMSec(struct _TMp4FileReadOps *This, int seek_msec, int *get_seek_msec)
{
    if (!This || !This->Priv || !get_seek_msec)
    {
        mp4_err("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    if (0 == seek_msec)
    {
        return 0;
    }

    uint64_t seek_time_msec = 0;
    uint64_t duration = 0;
    fd_av_t *_hdl = This->Priv->_hdl;

    duration = mp4_reader_getduration(_hdl->r);
    seek_time_msec = (int64_t)(seek_msec);
    seek_time_msec = seek_time_msec > duration ? duration : seek_time_msec;

    mp4_reader_seek(_hdl->r, &seek_time_msec);

    *get_seek_msec = (int)(seek_time_msec);

    return 0;
}

/*@ breif Mp4ReadGetDurationMs 获取当前MP4录像的实际录像时间(单位:毫秒)
 *@ param [in] This 读录像句柄
 *@ return 成功返回当前录像文件的时长(毫秒)，失败返回-1
 */
static int Mp4ReadGetDurationMs(struct _TMp4FileReadOps *This)
{
    if (!This || !This->Priv)
    {
        mp4_err("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    fd_av_t *_hdl = This->Priv->_hdl;

    return mp4_reader_getduration(_hdl->r);
}

/*@ breif Mp4ReadGetSampleCount 获取当前MP4录像的帧数
 *@ param [in] This 读录像句柄
 *@ return 成功返回当前录像文件帧数，失败返回-1
 */
static int Mp4ReadGetSampleCount(struct _TMp4FileReadOps *This)
{
    if (!This || !This->Priv)
    {
        mp4_err("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    fd_av_t *_hdl = This->Priv->_hdl;

    return mp4_reader_get_sample_count(_hdl->r);
}

/*@ breif Mp4ReadFindContentBox 寻找MP4的BOX
 *@ param [in] This 句柄
 *@ param [in] mp4_file MP4文件 
 *@ param [in/out] box box信息(传入box里面的name信息，获取偏移和长度)
 *@ return 成功返回1，失败返回-1
 */
static int Mp4ReadFindContentBox(struct _TMp4FileReadOps *This, const char *mp4_file, mp4_box_t *box)
{
    if (!This || !mp4_file)
    {
        mp4_err("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    int ret = 0;
    size_t rlen = 0;
    size_t flen = 0;
    size_t boxLen = 0;

    FILE *vfp = fopen(mp4_file, "rb");
    if(NULL == vfp)
    {
        mp4_sys("Fail to fopen %s!\n", mp4_file);
        return -1;
    }

    fseek(vfp, 0, SEEK_END);
    flen = ftell(vfp);
    fseek(vfp, 0, SEEK_SET);

    while(rlen < flen)
    {
        mp4_box_t boxtmp = {0};
        if(READ_SIZE != fread((uint8_t *)&boxtmp, 1, READ_SIZE, vfp))
        {
            mp4_sys("Fail to fread %s!\n", mp4_file);
            ret = -1;
            break;
        }

        char srcname[5] = {0};
        char dstname[5] = {0};
        memcpy(srcname, boxtmp.name, 4);
        memcpy(dstname, box->name, 4);

#if 0
        mp4_inf("rlen = %u\n",rlen);
        int t = 0;
        uint8_t* ppp = (unsigned char *)&boxtmp;
        for(t=0; t<READ_SIZE; t++)
        {
            printf("%#02x ", ppp[t]);
        }
        printf("\n");

#endif

        boxLen = SWAP_EDIAN_U32(boxtmp.len); // ntohl(boxtmp.len);
        if(boxLen < READ_SIZE)
        {
            ret = -1;
 //         break;
        }

        if(0 == strcmp(srcname, dstname))
        {
            memcpy((unsigned char *)box, (unsigned char *)&boxtmp, READ_SIZE);
            if(rlen + boxLen > flen)
            {
                ret = 0;
                break;
            }
            box->offset = ftell(vfp) - READ_SIZE;
            ret = 1;
            break;
        }
        else
        {
            if(rlen + boxLen > flen)
            {
                mp4_err("Box length beyond file!\n");
                ret = 0;
                break;
            }
            fseek(vfp, boxLen-READ_SIZE, SEEK_CUR);
            rlen += boxLen;
        }
    }
    safe_fclose(vfp);

    return ret;
}

/*@ breif Mp4FileReadOpsCreate 创建读MP4相关操作句柄
 *@ return 成功返回操作句柄，失败返回NULL
 */
TMp4FileReadOps* Mp4FileReadOpsCreate(void)
{
    TMp4FileReadOps *This = (TMp4FileReadOps *)calloc(1, sizeof(TMp4FileReadOps));
    if (NULL == This)
    {
        mp4_sys("Fail to calloc TMp4FileReadOps handler!\n");
        return NULL;
    }

    This->Priv = (struct _TMp4FileReadOpsPriv *)calloc(1, sizeof(struct _TMp4FileReadOpsPriv));
    if (NULL == This->Priv)
    {
        mp4_sys("Fail to calloc TMp4FileReadOps Priv handler!\n");
        safe_free(This);
        return NULL;
    }

    This->Mp4ReadOpen = Mp4ReadOpen;
    This->Mp4ReadClose = Mp4ReadClose;
    This->Mp4ReadFrame = Mp4ReadFrame;
    This->Mp4ReadGetFileSize = Mp4ReadGetFileSize;
    This->Mp4ReadSeekSec = Mp4ReadSeekSec;
    This->Mp4ReadSeekMSec = Mp4ReadSeekMSec;
    This->Mp4ReadGetDurationMs = Mp4ReadGetDurationMs;
    This->Mp4ReadGetSampleCount = Mp4ReadGetSampleCount;
    This->Mp4ReadFindContentBox = Mp4ReadFindContentBox;

    return This;
}

/*@ breif Mp4FileReadOpsDestroy 销毁MP4读操作句柄
 *@ return 成功返回0，失败返回-1
 */
int Mp4FileReadOpsDestroy(TMp4FileReadOps* This)
{
    if (!This || !This->Priv)
    {
        mp4_err("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    safe_free(This->Priv);
    safe_free(This);

    return 0;
}

/*************************************************************************************************************/

/*@ breif Mp4RebuildOpen 打开一个需要修复(重建的)MP4文件
 *@ param [in] This 句柄
 *@ param [in] name MP4文件 
 *@ param [in] flag 标志位
 *@ return 成功返回0  失败返回-1
 */
static int Mp4RebuildOpen(struct _TMp4FileRebuildOps *This, const char *name, int flag)
{
    if (!This || !This->Priv)
    {
        mp4_err("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    This->Priv->_hdl = (fd_rebuild_t*)calloc(1, sizeof(fd_rebuild_t));
    fd_rebuild_t *_hdl = This->Priv->_hdl;
    if (!_hdl)
    {
        mp4_sys("Fail to calloc!\n");
        return -1;
    }

    _hdl->flag = flag;
    _hdl->fp= fopen(name, "rb+");
    if (!_hdl->fp)
    {
        mp4_sys("Fail to fopen %s\n", name);
        safe_free(_hdl);
        return -1;
    }
    _hdl->b = mp4_rebuilder_create(&s_io, (void*)_hdl->fp, _hdl->flag);    

    return 0;
}

/*@ breif Mp4RebuildClose 关闭一个需要修复(重建的)MP4文件
 *@ param [in] This 句柄
 *@ return 成功返回0，失败-1
 */
static int Mp4RebuildClose(struct _TMp4FileRebuildOps *This)
{
    if (!This || !This->Priv)
    {
        mp4_err("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    fd_rebuild_t *_hdl = This->Priv->_hdl;
    mp4_rebuilder_destroy(_hdl->b);
    safe_fclose(_hdl->fp);
    safe_free(_hdl);

    return 0;
}

/*@ breif Mp4RebuildAddAudio 添加音频track */
static int Mp4RebuildAddAudio(struct _TMp4FileRebuildOps *This, uint8_t object, int channel_count, int bits_per_sample, int sample_rate, const void* extra_data, size_t extra_data_size)
{
    if (!This || !This->Priv)
    {
        mp4_err("NULL POINTER EXCEPTION!\n");
        return -1;
    }
    fd_rebuild_t *_hdl = This->Priv->_hdl;
    return mp4_rebuilder_add_audio(_hdl->b, object, channel_count, bits_per_sample, sample_rate, extra_data, extra_data_size);
}

/*@ breif Mp4RebuildAddVideo 添加视频track */
static int Mp4RebuildAddVideo(struct _TMp4FileRebuildOps *This, uint8_t object, int width, int height, const void* extra_data, size_t extra_data_size)
{
    if (!This || !This->Priv)
    {
        mp4_err("NULL POINTER EXCEPTION!\n");
        return -1;
    }
    fd_rebuild_t *_hdl = This->Priv->_hdl;
    return  mp4_rebuilder_add_video(_hdl->b, object, width, height, extra_data, extra_data_size);
}

/*@ breif Mp4RebuildAddSubtitle 添加字幕 */
static int Mp4RebuildAddSubtitle(struct _TMp4FileRebuildOps *This, uint8_t object, const void* extra_data, size_t extra_data_size)
{
    if (!This || !This->Priv)
    {
        mp4_err("NULL POINTER EXCEPTION!\n");
        return -1;
    }
    fd_rebuild_t *_hdl = This->Priv->_hdl;
    return mp4_rebuilder_add_subtitle(_hdl->b, object, extra_data, extra_data_size);
}

/*@ breif Mp4RebuildWriteFrameNoData 写帧数据(数据为空) */
static int Mp4RebuildWriteFrameNoData(struct _TMp4FileRebuildOps *This, int track, size_t data_bytes, int64_t pts, int64_t dts, int flags)
{
    if (!This || !This->Priv)
    {
        mp4_err("NULL POINTER EXCEPTION!\n");
        return -1;
    }
    fd_rebuild_t *_hdl = This->Priv->_hdl;
    return mp4_rebuilder_write(_hdl->b, track, data_bytes, pts, dts, flags);
}

/*@ breif Mp4FileRebuildOpsCreate 创建修复MP4相关操作句柄
 *@ return 成功返回操作句柄，失败返回NULL
 */
TMp4FileRebuildOps* Mp4FileRebuildOpsCreate(void)
{
    TMp4FileRebuildOps *This = (TMp4FileRebuildOps *)calloc(1, sizeof(TMp4FileRebuildOps));
    if (NULL == This)
    {
        mp4_sys("Fail to calloc TMp4FileRebuildOps handler!\n");
        return NULL;
    }

    This->Priv = (struct _TMp4FileRebuildOpsPriv *)calloc(1, sizeof(struct _TMp4FileRebuildOpsPriv));
    if (NULL == This->Priv)
    {
        mp4_sys("Fail to calloc TMp4FileRebuildOps Priv handler!\n");
        safe_free(This);
        return NULL;
    }

    This->Mp4RebuildOpen = Mp4RebuildOpen;
    This->Mp4RebuildClose = Mp4RebuildClose;
    This->Mp4RebuildAddAudio = Mp4RebuildAddAudio;
    This->Mp4RebuildAddVideo = Mp4RebuildAddVideo;
    This->Mp4RebuildAddSubtitle = Mp4RebuildAddSubtitle;
    This->Mp4RebuildWriteFrameNoData = Mp4RebuildWriteFrameNoData;

    return This;
}

/*@ breif Mp4FileRebuildOpsDestroy 销毁MP4修复操作句柄
 *@ return 成功返回0，失败返回-1
 */
int Mp4FileRebuildOpsDestroy(TMp4FileRebuildOps* This)
{
    if (!This || !This->Priv)
    {
        mp4_err("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    safe_free(This->Priv);
    safe_free(This);

    return 0;
}
