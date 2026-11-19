/*****************************************************************************
* Copyright (C) 2017 All rights reserved.
*  文件名称：aviapi.c
*  创 建 者：樊聪
*  邮    箱：fancong20@163.com
*  创建日期：2017年08月24日 星期四
*  描    述：实现如下功能：
*            1. 封装AVILIB库，实现H264和AVI文件的互相转化；
*            2. H264转化为AVI文件采用写AVI的方式实现，当前只支持一路转化，即多线程里面只允许一个写AVI的功能实习；
*            3. AVI转化为H264帧数据采用读AVI方式实现，当前最大支持四路转化，即一个AVI文件同时支持四个读AVI操作；
*            4. 读AVI操作的最大路数可以由宏MAX_READ_CHN_NUM控制；
*            5. 支持G711x和PCM音频转化；
*            6. 支持读AVI模式下面的SEEK定位，以视频帧为主，即当前写成的AVI组成形式为：
*               AVI帧头 + AVI帧数据 + AVI关键帧信息索引
*               其中AVI关键帧信息索引里面存放的是当前AVI每个I帧在视频帧里面的帧号和其对应的时间戳，主要是方便索引定位；
*               由于采用静态数组的形式存放，故当前AVI的大小限定为6~7MB，可以通过宏MAX_AVI_FILE_I_FRAME_NUM来调节当
*               前AVI里面I帧的个数用以存放更大或者更小的AVI文件；
*            7. AVI回放定位以视频帧为主，支持音视频在非全帧率下面的实时同步；
*  更新日志：
*            2019年12月13日 10:20:07
*            1. 修改如果设备支持128G的SD卡，则必须是14MB一个文件，否则是7MB一个文件；
*  
******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <pthread.h>
#include <math.h>
#include <libgen.h>
#include <sys/stat.h> 
#include <sys/types.h>
#if defined (__MUSL__)
#include <sys/time.h>
#endif

#include "g711_law.h"
#include "aviapi.h"
#include "mp4.h"
#include "sys_banned.h"
#include "LogManager.h"


#define AVI_ERR(format...)              LOG_E(LOG_MODULE_SD, format)
#define AVI_WRAN(format...)             LOG_W(LOG_MODULE_SD, format)
#define AVI_INFO(format...)             LOG_I(LOG_MODULE_SD, format)
#define AVI_DBG(format...)              LOG_D(LOG_MODULE_SD, format)

#define CALC_FRAMERATE(start_t, stop_t, frames)   ((float)(frames)*90000.0/abs((stop_t)-(start_t)))

#define MAX_READ_CHN_NUM                (4)
#define MAX_PATH_STR_NUM                (128)
#define MAX_REC_FILE_I_FRAME_NUM        (2048)
#define MAX_IFRAME_INTAL_FRAME_NUM      (500)  // 10seconds * 50frames(25fps+25fps)

#define I_FRAME                         (0x01)
#define P_FRAME                         (0x02)
#define MJPG_FRAME                      (0x03)
#define AUDIO_FRAME                     (0x08)
#define QCIFIFrames                     (0x0010)
#define QCIFPFrames                     (0x0040)

#define G726_AUDIO_FRAME                (0x0)
#define G711A_AUDIO_FRAME               (0x1)
#define G711U_AUDIO_FRAME               (0x2)
#define MP3_AUDIO_FRAME                 (0x5)
#define ADPCMI_AUDIO_FRAME              (0x6)
#define ADPCMD_AUDIO_FRAME              (0x7)
#define AMR_AUDIO_FRAME                 (0x8)

/* 该宏用来控制AVI调试打印信息 */
#define AVI_DBG_INF

/* 该宏用来控制当前录制的AVI是264数据还是265数据(265数据只能用VLC和完美解码播放)录像，在Makefile对应的profile里面定义 */
#ifdef __IS_FAKE_H265__
#define RECORD_COMPRESSOR   "HEVC"
#else
#define RECORD_COMPRESSOR   "h264"
#endif

#if defined (__USE_EMMC__)
#define DEFAULE_FRAME_INFO_FILE    "/tmp/app/disk/mmcblk1p1/video/.frames_index"
#else
#define DEFAULE_FRAME_INFO_FILE    "/tmp/app/disk/mmcblk0p1/video/.frames_index"
#endif

#define STREAM_TYPE_VIDEO     (1) // 单独的视频流
#define STREAM_TYPE_AUDIO     (2)
#define STREAM_TYPE_AVSYNC    (3) // 混合流(视频流+音频流)

#define FILE_START_CODE       (0xB6010000)
#define FILE_TYPE_ID_H264     (0x34363248)
#define FILE_TYPE_ID_H265     (0x35363248)
#define FILE_VERSION_2_0      (0x02)

#define G711A_AUDIO_FRAME     (0x1)
#define G711U_AUDIO_FRAME     (0x2)
#define AAC_AUDIO_FRAME       (0x9)


#define LOADTYPE_H264_ENC     (0x00)
#define LOADTYPE_H265_ENC     (0x04)

#define LOADTYPE_G726_ENC     (0x00)
#define LOADTYPE_G711A_ENC    (0x01)
#define LOADTYPE_G711U_ENC    (0x02)

#define DATA_PAKT_START_CODE (0xB7010000)

#define FRAME_INDEX_START_CODE    (0x494E4458) // indx

#define MAX_MP4_EXTRA_SIZE   (64*1024)

/* 文件头信息 */
typedef struct tagMP4NewFileHeader
{
    unsigned int FileStartCode;		        //固定为0xB6010000(用作校验)
    unsigned int FileID;					//FILE_TYPE_ID_H264和FILE_TYPE_ID_H265
    unsigned int FileHeadLen;				//媒体文件头的长度(包含本身4Byte)
    unsigned char Version;					//媒体文件头的版本信息.初始版本为1
    /*
    2Bytes, 表示媒体数据包及媒体索引信息
    MSB B15 		  0:没有索引 1:有索引
    	B1			  0:no Private Data 1:with Private Data
    	B0			  0: Video Only 1:Video&Audio
    */
    short int	DataFlag;		  
    short int	dwImageWidth;			    //视频媒体数据的图像宽度
    short int	dwImageHight; 		        //视频媒体数据的图像高度
}__attribute__((packed)) MPEG4FILEHEADER;

/* 帧头信息 */
typedef struct tagH264DataPacketHeader
{
    unsigned int        DataPacketStartCode;    //固定为0xB7010000
    unsigned int        FrameNo;                //媒体数据包的序号(音视频独立编号)
    unsigned char       FrameType;
    unsigned char       byLoadType;             //媒体数据包的信息,具体含义依赖于帧类型
    unsigned int        ShowTime;               //表示媒体数据包相对于上一帧的偏差时间(频率90Khz)
    unsigned long       u32RealTimeSec;
    unsigned long       u32RealTimeUSec;
    unsigned short      uReserved;
    unsigned int        nOverloadLen;           //表示媒体数据包的数据长度
}__attribute__((packed)) H264DATAPACKETHEADER, *PH264DATAPACKETHEADER;

typedef struct FILE_inf
{
    char   avi_file[MAX_PATH_STR_NUM];  // 当前操作的AVI
    avi_t *avi_lib_handle;              // 标明文件(-1为没有)
    int    filesize;                    // 文件大小
    int    start_time;                  // 文件开始时间
    int    end_time;                    // 文件结束时间
    int    frame_count;                 // 当前数据总帧数
}File_info,*p_file_info;

/* 私有数据的操作信息 */
typedef struct tagPrvInfo
{
    char prv_file[MAX_PATH_STR_NUM];    // 当前操作的PRV
    long fdes;                          // 私有文件的文件描述符
    long mode;                          // 模式: 0 正在写文件, 1 正在读文件
    long read_frame;                    // 当前读到了第几帧数据(读模式起效)
    long write_frame;                   // 当前写到了第几帧数据(写模式起效)
    off_t curr_read_pos;                // 读模式：当前帧开始的偏移
    off_t next_read_pos;                // 读模式：下一帧开始的偏移
    off_t curr_write_pos;               // 写模式：当前帧开始的偏移
    off_t next_write_pos;               // 写模式：下一帧开始的偏移
}stuPrvInfo, *lpPrvInfo;

/* 每个AVI的I帧信息，固定为16个字节 */
typedef struct tagIFrameInfo
{
    unsigned int CurIFrameTime;     // 当前时间
    unsigned int ShowTime;          // 当前I帧的相对时间戳
    unsigned int IFrameNum;         // 当前I帧编号在总视频帧里面的编号
    unsigned int CurrFramePos;      // 当前帧在文件中的偏移地址(PRV格式有效)
}__attribute__((packed))IFrameInfo, *pIFrameInfo;

static File_info  g_file_read_info[MAX_READ_CHN_NUM];
static File_info  g_file_write_info = {0};
static IFrameInfo g_struIFameInfo[MAX_REC_FILE_I_FRAME_NUM];

/* 帧信息 */
static int  g_nFramesInfoFileFd = -1;
static char g_szFrameFileName[MAX_PATH_STR_NUM] = {0};

static stuPrvInfo g_prv_file_read_info[MAX_READ_CHN_NUM];
static stuPrvInfo g_prv_file_write_info;

static MPEG4FILEHEADER g_prv_file_read_header[MAX_READ_CHN_NUM];
static MPEG4FILEHEADER g_prv_file_write_header;

static int g_nCurrentVideoFrameNum  = 0;
static int g_nCurrentAudioFrameNum  = 0;

static int  g_bStartReadAviMode[MAX_READ_CHN_NUM] = {0};
static int  g_bStartWriteAviMode = 0;

static int g_nRecordWidth  = 1920;
static int g_nRecordHeight = 1080;
static int g_nEncodeType    = FILE_TYPE_ID_H264;
static int g_nMp4EncType    = ENC_H265;

#if defined (__PRODUCT_G85T__) || defined (__PRODUCT_w85T__) || defined (__PRODUCT_G84F__) || defined (__PRODUCT_W84F__) || defined (__PRODUCT_G63F__)|| defined (__PRODUCT_W63F__)
#define MAX_WRITE_BUF_SIZE              (2 * 1024 * 1024)                        // write写入缓冲区大小
#elif defined (__PRODUCT_G43TA__) 
#define MAX_WRITE_BUF_SIZE              (512 * 1024) 
#else
#define MAX_WRITE_BUF_SIZE              (1 * 1024 * 1024)                        // write写入缓冲区大小
#endif

#ifdef __SUPPORT_128G_DISK__
#define MAX_AVI_FILE_I_FRAME_NUM        (2048)  //14MB // (14 * 1024 / 8) // 1024 > 768个I帧(64Kb => 64/8=8KB
#define AVIAPI_VIDEO_FILESIZE           (14 * 1024 * 1024)  // 视频文件最大大小
#define AVIAPI_VIDEO_TYPE_INFO_SIZE     (32 * 1024)                 // 录像类型信息大小，这里要与sd_ops中SD_OPS_RecordTypeInfo保持一致
#define AVIAPI_VIDEO_FRAME_INFO_SIZE    (32 * 1024)         // 帧索引数据信息大小，
#else
#define MAX_AVI_FILE_I_FRAME_NUM        (1024)  // (6 * 1024 / 8) // 1024 > 768个I帧(64Kb => 64/8=8KB
#define AVIAPI_VIDEO_FILESIZE           (7 * 1024 * 1024)  // 视频文件最大大小
#define AVIAPI_VIDEO_TYPE_INFO_SIZE     (32 * 1024)         // 录像类型信息大小，这里要与sd_ops中SD_OPS_RecordTypeInfo保持一致
#define AVIAPI_VIDEO_FRAME_INFO_SIZE    (16 * 1024)        // 帧索引数据信息大小，
#endif
#define AVIAPI_VIDEO_ADD_INFO_SIZE      (AVIAPI_VIDEO_TYPE_INFO_SIZE + AVIAPI_VIDEO_FRAME_INFO_SIZE)     // 尾部录像附加信息大小
static IFrameInfo g_iframe_info[MAX_AVI_FILE_I_FRAME_NUM];
static int g_nCurrentIFrameNum = 0;

static int WriteAviFrame(avi_t *avi_out_fd, int frame_size, unsigned char * frame_buffer, int keyframe, unsigned int *file_pos)
{
    if (NULL==avi_out_fd || NULL==frame_buffer)
    {
        AVI_ERR("NULL POINTER EXCEPTION\n");
        return -1;
    }
    
    /* 针对音频帧，需要针对前面初始化时给入的音频格式参数进行处理，这里是G711U，不需要处理 */
    if (AUDIO_FRAME == keyframe)
    {
        if (AVI_write_audio(avi_out_fd, (char *)frame_buffer, frame_size) < 0)
        {
            AVI_print_error("Fail to write avi audio");
            return -1;
        }
    }
    else
    {
        if(AVI_write_frame(avi_out_fd, (char *)frame_buffer, frame_size, (I_FRAME==keyframe)?1:0) < 0)
        {
            AVI_print_error("Fail to write avi video");
            return -1;
        }        
    }
    
    *file_pos = avi_out_fd->pos;
    
    return 0;
}

/* write写入缓冲区信息 */
typedef struct tagWiteBufInfo
{
    unsigned int FrameCnt;                  //已缓存帧个数
    unsigned int DataBufLen;                //已缓存大小
    char *pDataBuf;                         //数据缓冲区
}stuWiteBufInfo, *lpWiteBufInfo;

/* 使用TFWriteCache需特别注意，因为当前在同一线程中，因此未加锁处理 */
static stuWiteBufInfo TFWriteCache;

/* 刷新I帧位置及音视频计数（在文件后面定义，前面需要使用因此前置声明） */
void RefreshFrameCntAndIFrameInfo(H264DATAPACKETHEADER *pframe_head, unsigned int write_pos);

static int WritePrvFrame(H264DATAPACKETHEADER *frame_head, unsigned char * frame_data, int force_flush)
{
    if (NULL==frame_head || NULL==frame_data || NULL==TFWriteCache.pDataBuf)
    {
        AVI_ERR("NULL POINTER EXCEPTION\n");
        return -1;
    }
    unsigned int frame_szie = frame_head->nOverloadLen+sizeof(H264DATAPACKETHEADER);
    /* 1、缓冲区实际大小+当前帧大小 < 缓冲区大小, 正常写入缓冲区 */
    if ((force_flush == 0) && (TFWriteCache.DataBufLen+frame_szie < MAX_WRITE_BUF_SIZE))
    {
        memcpy((TFWriteCache.pDataBuf+TFWriteCache.DataBufLen), frame_data, frame_szie);
        TFWriteCache.FrameCnt++;
        TFWriteCache.DataBufLen += frame_szie;
    }
    /* 2、缓冲区实际大小+当前帧大小 >= 缓冲区大小 or 需要强制刷新缓冲区 */
    else
    {
    	
        unsigned int write_pos = 0;
        unsigned int write_frame = 0;

        /* 2.1、将缓冲区数据写入文件 */
        if (g_prv_file_write_info.fdes < 0)
        {
            AVI_ERR("invalid prv fd\n");
            return -1;
        }

        /* 写之前记录当前帧的开始位置 */
        g_prv_file_write_info.curr_write_pos = lseek(g_prv_file_write_info.fdes, 0, SEEK_CUR);
        write_pos = g_prv_file_write_info.curr_write_pos;
		write_frame++;
        /* 写帧数据 */
        if (TFWriteCache.FrameCnt > 0)
        {
            unsigned int i = 0;
            unsigned int npos = 0;
            H264DATAPACKETHEADER *pframe_head = NULL;
        
            write_frame = TFWriteCache.FrameCnt;
            write(g_prv_file_write_info.fdes, TFWriteCache.pDataBuf, TFWriteCache.DataBufLen);
            TFWriteCache.FrameCnt = 0;
            TFWriteCache.DataBufLen = 0;

            /* 刷新I帧位置及音视频计数 */
            for (i=0; i<write_frame; i++)
            {
                pframe_head = (H264DATAPACKETHEADER *)(TFWriteCache.pDataBuf+npos);
                RefreshFrameCntAndIFrameInfo(pframe_head, write_pos);
                write_pos += pframe_head->nOverloadLen+sizeof(H264DATAPACKETHEADER);
                npos += pframe_head->nOverloadLen+sizeof(H264DATAPACKETHEADER);
            }
        }

        /* 2.2、当前帧大小 < 缓冲区大小, 正常写入缓冲区 */
        if (frame_szie < MAX_WRITE_BUF_SIZE && force_flush != 1)
        {
            memcpy((TFWriteCache.pDataBuf+TFWriteCache.DataBufLen), frame_data, frame_szie);
            TFWriteCache.FrameCnt++;
            TFWriteCache.DataBufLen += frame_szie;
        }
        /* 2.3、当前帧大小 >= 缓冲区大小, 把当前帧写入文件*/
        else
        {

            write(g_prv_file_write_info.fdes, frame_data, frame_szie);
			
            /* 刷新I帧位置及音视频计数 */
            RefreshFrameCntAndIFrameInfo(frame_head, write_pos);
            write_pos += frame_szie;
        }

        /* 记录当前文件偏移和写入的帧数 */
        g_prv_file_write_info.write_frame += write_frame;
        g_prv_file_write_info.next_write_pos = lseek(g_prv_file_write_info.fdes, 0, SEEK_CUR);
    }

    return 0;

}


__attribute__((unused)) static avi_t * OpenAviFileToWrite(const char* avi_file)
{
    avi_t *avi_out_fd = NULL; 

    if (avi_file) 
    {
        if ((avi_out_fd=AVI_open_output_file(avi_file)) == NULL) 
        {
            AVI_print_error("Fail to AVI_open_output_file");
            AVI_ERR("cannot open write file %s\n", avi_file);
            return NULL;
        }
    }
    else
    {
        AVI_ERR("there is no outputfile\n");
        return NULL;
    }

    /* 以下帧的长宽会随着写入帧的改变而改变，帧率会进行自我统计，故以下值均是初始化的时候用 */
    AVI_set_video(avi_out_fd, g_nRecordWidth, g_nRecordHeight, 25.0, RECORD_COMPRESSOR);

    /* channels: 0表示无音频
     * rate: 采样率(8K, 16K, ...)
     * bits: 采样位数(8位，16位, ...)
     * format: 格式(1:pcm , ..., 7:g711u, ...见avilib.h宏定义)
     * mp3rate: G711U格式则为64, PCM则为128
     */
    AVI_set_audio(avi_out_fd, 1, 8000, 16, WAVE_FORMAT_MULAW, 64);

    return avi_out_fd;
}

__attribute__((unused)) static avi_t * OpenAviFileToRead(const char* avi_file)
{
    avi_t *avi_in_fd = NULL; 

    if (avi_file) 
    {
        if ((avi_in_fd=AVI_open_input_file(avi_file, 1)) == NULL) 
        {
            AVI_print_error("Fail to AVI_open_input_file");
            AVI_ERR("cannot open write file %s\n", avi_file);
            return NULL;
        }
    }
    else
    {
        AVI_ERR("there is no outputfile\n");
        return NULL;
    }


    if (AVI_seek_start(avi_in_fd)) // 寻找文件开始
    {
        AVI_print_error("Fail to AVI_seek_start");
        return NULL;
    }

    return avi_in_fd;
}

__attribute__((unused)) static void CloseAviFile(p_file_info p_file_info, int b_reading_mode)
{
    if (NULL == p_file_info)
    {
        AVI_ERR("NULL POINTER EXCEPTION!\n");
        return;
    }

    if(p_file_info->avi_lib_handle!=NULL && p_file_info->avi_lib_handle>0)
    {
        /* 对于写模式需要统计最后获取的帧率，然后写到AVI文件里面，读模式则不需要 */
        if (0 == b_reading_mode)
        {
            /* 获取录像片段延续时间 */
            if (p_file_info->frame_count > 0)
            {
                /* 相对时间戳到了0x40000000就会溢出 */
                if(p_file_info->end_time < p_file_info->start_time)
                {
                    p_file_info->avi_lib_handle->fps = CALC_FRAMERATE(p_file_info->start_time, p_file_info->end_time+0x40000000, p_file_info->frame_count);
                }
                else
                {
                    p_file_info->avi_lib_handle->fps = CALC_FRAMERATE(p_file_info->start_time, p_file_info->end_time, p_file_info->frame_count);
                }
            }
            else
            {
                p_file_info->avi_lib_handle->fps = 25.0;
            }

#ifdef AVI_DBG_INF
            AVI_DBG("p_file_info->avi_lib_handle->fps = %f\n", p_file_info->avi_lib_handle->fps);
#endif
        }
        
        AVI_close(p_file_info->avi_lib_handle);
        p_file_info->avi_lib_handle = NULL;
    }
}

__attribute__((unused)) static int SaveFramesInfo(const char *p_frame_info_file, const pIFrameInfo p_frames_info, int frame_count)
{
    if (NULL==p_frame_info_file || NULL==p_frames_info)
    {
        AVI_ERR("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    int file_fdes = -1;
    file_fdes = open(p_frame_info_file, O_RDWR | O_CREAT | O_TRUNC, 0777);
    if (file_fdes < 0)
    {
        AVI_WRAN("Fail to open %s!\n", p_frame_info_file);
        return -1;
    }

    write(file_fdes, p_frames_info, frame_count*sizeof(IFrameInfo));

    close(file_fdes);
    file_fdes = -1;

    return 0;
}

/* 设置I帧索引的文件名 */
int SetFrameFileName(const char *p_frame_files_name)
{
    if (NULL == p_frame_files_name)
    {
        AVI_ERR("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    strncpy(g_szFrameFileName, p_frame_files_name, sizeof(g_szFrameFileName) - 1);
    
    return 0;
}

/*@breif InitAviActionMode 初始化AVI操作的模式
 *@param[in]  avi_ops_file 操作的AVI文件名
 *@param[in] bReadAviMode 0:写AVI模式 1:读AVI模式 
 *@param[in] read_inx  读的通道，写模式无视
 *@return 成功返回0 失败返回-1
 */
int InitAviActionMode(const char *avi_ops_file, int bReadAviMode, int read_inx)
{
    if (NULL == avi_ops_file)
    {
        AVI_ERR("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    /* 读模式和写模式不能相互影响 */
    /* 读取模式需要AVI文件存在 */
    if (bReadAviMode)
    {
        read_inx = (read_inx >= MAX_READ_CHN_NUM) ? MAX_READ_CHN_NUM-1 : read_inx;

        memset(&g_prv_file_read_info[read_inx], 0, sizeof(stuPrvInfo));

        if (access(avi_ops_file, F_OK))
        {
            AVI_ERR("Read prv mode but file %s not exist!\n", avi_ops_file);
            return -1;
        }

        int prv_file_header_size = sizeof(MPEG4FILEHEADER);

        g_prv_file_read_info[read_inx].fdes = open(avi_ops_file, O_RDONLY);
        if (g_prv_file_read_info[read_inx].fdes < 0)
        {
            AVI_WRAN("[ReadMode] Fail to open prv file %s[%d]!\n", avi_ops_file, read_inx);
            return -1;
        }

        /* 文件指针移动到文件头 */
        lseek(g_prv_file_read_info[read_inx].fdes, 0, SEEK_SET);

        /* 读取当前PRV的文件头 */
        if (read(g_prv_file_read_info[read_inx].fdes, &g_prv_file_read_header[read_inx], prv_file_header_size) != prv_file_header_size)        
        {
            AVI_WRAN("Fail to read prv file header!\n");
        }

#ifdef AVI_DBG_INF
        AVI_INFO("---%s info---\n", avi_ops_file);
        AVI_INFO("----------------Frame width: %04d-------------------\n", g_prv_file_read_header[read_inx].dwImageWidth);
        AVI_INFO("----------------Frame heigh: %04d-------------------\n", g_prv_file_read_header[read_inx].dwImageHight);
        AVI_INFO("----------------------------------------------------\n");
#endif

        strncpy(g_prv_file_read_info[read_inx].prv_file, avi_ops_file, MAX_PATH_STR_NUM);
        g_prv_file_read_info[read_inx].mode = 1;
        g_prv_file_read_info[read_inx].read_frame = 0;
        g_prv_file_read_info[read_inx].next_read_pos = prv_file_header_size;

        g_bStartReadAviMode[read_inx] = 1;
    }
    else
    {
        g_nCurrentIFrameNum = 0;
        g_nCurrentVideoFrameNum = 0;
        g_nCurrentAudioFrameNum = 0;

        memset(&g_prv_file_write_info, 0, sizeof(stuPrvInfo));
            memset(g_iframe_info, 0, MAX_AVI_FILE_I_FRAME_NUM*sizeof(IFrameInfo));

        int mask = 0;
        mask = umask(0);
        umask(mask);

        g_prv_file_write_info.fdes = open(avi_ops_file, O_RDWR|O_CREAT|O_TRUNC, (S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH) &~ mask);
        if (g_prv_file_write_info.fdes < 0)
        {
            AVI_WRAN("[WriteMode] Fail to open prv file %s!\n", avi_ops_file);
            return -1;
        }

        /* 初始化文件头结构体 */
		memset(&g_prv_file_write_header, 0, sizeof(MPEG4FILEHEADER));
        g_prv_file_write_header.FileStartCode = FILE_START_CODE;          
        g_prv_file_write_header.FileID = g_nEncodeType;
        g_prv_file_write_header.FileHeadLen = sizeof(MPEG4FILEHEADER);
        g_prv_file_write_header.Version = FILE_VERSION_2_0;
        g_prv_file_write_header.DataFlag = STREAM_TYPE_AVSYNC;
        g_prv_file_write_header.dwImageWidth = g_nRecordWidth;
        g_prv_file_write_header.dwImageHight = g_nRecordHeight;

		/* 文件指针定位到头，写文件头 */
		lseek(g_prv_file_write_info.fdes, 0, SEEK_SET);
		write(g_prv_file_write_info.fdes, &g_prv_file_write_header, sizeof(MPEG4FILEHEADER));
    
		/* 将文件指针定位到文件头的后面，开始写帧头和帧数据 */
		lseek(g_prv_file_write_info.fdes, sizeof(MPEG4FILEHEADER), SEEK_SET);
        
		/* 初始化stuPrvInfo */
        strncpy(g_prv_file_write_info.prv_file, avi_ops_file, MAX_PATH_STR_NUM);
        g_prv_file_write_info.mode = 0;
        g_prv_file_write_info.write_frame = 0;
        g_prv_file_write_info.next_write_pos = sizeof(MPEG4FILEHEADER);

        /* 帧信息文件的保存 */
        if (0 == strlen(g_szFrameFileName))
        {
            strncpy(g_szFrameFileName, DEFAULE_FRAME_INFO_FILE, sizeof(g_szFrameFileName));
        }

        g_nFramesInfoFileFd = open(g_szFrameFileName, O_RDWR|O_CREAT|O_TRUNC, (S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH) &~ mask);
        if (g_nFramesInfoFileFd < 0)
        {
            AVI_WRAN("[WriteMode] Fail to open frame info file! %s\n", g_szFrameFileName);
        }
        else
        {
            /* 写文件头(当前操作的文件名) */
            write(g_nFramesInfoFileFd, g_prv_file_write_info.prv_file, MAX_PATH_STR_NUM);
        }
        
        close(g_nFramesInfoFileFd); //暂时处理避免AOV模式进出TF卡占用无法挂载卸载
        g_nFramesInfoFileFd = -1;

        g_bStartWriteAviMode = 1;
    }

    return 0;
}

/*@breif GetAviVideoParam 获取AVI视频参数
 *@param[in]   read_inx  读的通道
 *@param[out]  pWidth 当前AVI视频的宽度
 *@param[out]  pHight 当前AVI视频的高度
 *@param[out]  pFramerate 当前AVI视频的帧率
 *@param[out]  pTotalFrames 当前AVI视频的总帧数
 *@return 成功返回0 失败返回-1
 *@note   仅在读AVI操作模式下面工作
 */
int GetAviVideoParam(int read_inx, int *pWidth, int *pHight, double *pFramerate, int *pTotalFrames)
{
    if (!pWidth || !pHight || !pFramerate || !pTotalFrames)
    {
        AVI_ERR("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    read_inx = (read_inx >= MAX_READ_CHN_NUM) ? MAX_READ_CHN_NUM-1 : read_inx;

    if (0 == g_bStartReadAviMode[read_inx])
    {
        AVI_ERR("Avi read mode not start!\n");
        return -1;
    }

    if (NULL == g_file_read_info[read_inx].avi_lib_handle)
    {
        AVI_ERR("avi_lib_handle[read] is NULL!\n");
        return -1;
    }

    *pWidth = AVI_video_width(g_file_read_info[read_inx].avi_lib_handle);
    *pHight = AVI_video_height(g_file_read_info[read_inx].avi_lib_handle);
    *pFramerate = AVI_frame_rate(g_file_read_info[read_inx].avi_lib_handle);
    *pTotalFrames = AVI_video_frames(g_file_read_info[read_inx].avi_lib_handle);
    
    return 0;
}

/*@breif GetAviAudioParam 获取AVI音频参数
 *@param[in]   read_inx  读的通道
 *@param[out]  pAudioMp3Rate 获取音频mp3码率(kbs)
 *@param[out]  pBits 获取音频每秒的采样数据位宽(8bit, 16bit, ...)
 *@param[out]  pRate 获取音频采样率(8k,16k,...)
 *@param[out]  pBytes 获取音频字节数
 *@return 成功返回0 失败返回-1
 *@note   仅在读AVI操作模式下面工作
 */
int GetAviAudioParam(int read_inx, long *pAudioMp3Rate, int *pBits, long *pRate, long *pBytes)
{
    if (!pAudioMp3Rate || !pBits || !pRate || !pBytes)
    {
        AVI_ERR("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    read_inx = (read_inx >= MAX_READ_CHN_NUM) ? MAX_READ_CHN_NUM-1 : read_inx;

    if (0 == g_bStartReadAviMode[read_inx])
    {
        AVI_ERR("Avi read mode not start!\n");
        return -1;
    }

    if (NULL == g_file_read_info[read_inx].avi_lib_handle)
    {
        AVI_ERR("avi_lib_handle[read] is NULL!\n");
        return -1;
    }

    *pAudioMp3Rate = AVI_audio_mp3rate(g_file_read_info[read_inx].avi_lib_handle);
    *pBits = AVI_audio_bits(g_file_read_info[read_inx].avi_lib_handle);
    *pRate = AVI_audio_rate(g_file_read_info[read_inx].avi_lib_handle);
    *pBytes = AVI_audio_bytes(g_file_read_info[read_inx].avi_lib_handle);

    return 0;
}

/*@breif SetAviIndexPosition 设置AVI索引的帧位置
 *@param[in]  read_inx 读的通道
 *@param[in]  value 当为AVI格式的时候，value是索引帧号；当未PRV格式的时候，value是位置偏移pos
 *@return 成功返回0 失败返回-1
 *@note   仅在读操作模式下面工作
 */
int SetAviIndexPosition(int read_inx, int value)
{
    if (0 == value)
    {
        AVI_DBG("Seek none!\n");
        return 0;
    }

    read_inx = (read_inx >= MAX_READ_CHN_NUM) ? MAX_READ_CHN_NUM-1 : read_inx;

    if (0 == g_bStartReadAviMode[read_inx])
    {
        AVI_ERR("Avi read mode not start!\n");
        return -1;
    }

    if (g_prv_file_read_info[read_inx].fdes < 0)
    {
        AVI_ERR("Invalid prv file fd [%d]!\n", read_inx);
        return -1;
    }

    lseek(g_prv_file_read_info[read_inx].fdes, value, SEEK_SET);

    return 0;
}

/*@breif SetAviRecSize 设置录像尺寸
 *@param[in]  width 录像宽
 *@param[in]  height 录像高
 *@param[in]  bH265 当前是否是265编码
 *@return 成功返回0 失败返回-1
 *@note 仅在写操作模式下面工作，且编码参数只有在创建录像文件时才能有效设置，其他情况设置无效
 */
int SetAviRecSize(int width, int height, int bH265)
{   
    if ((!width) || (!height))
    {
        AVI_ERR("Fail to called SetAviRecSize\n");
        return -1;
    }

    g_nRecordWidth = width;
    g_nRecordHeight = height;
    g_nMp4EncType = (bH265) ? ENC_H265 : ENC_H264;
    g_nEncodeType = (bH265) ? FILE_TYPE_ID_H265 : FILE_TYPE_ID_H264;

    return 0;
}

/*@breif ReadAviFrame 读取AVI帧数据
 *@param[in]  read_inx 读的通道
 *@param[out] FrameData 帧数据缓冲区 
 *@param[in]  max_frame_size 帧数据缓冲区FrameData的最大尺寸
 *@param[out] framesize 获取到的帧数据大小
 *@param[out] frametype 获取到的帧数据类型: =1视频帧 =0音频帧 
 *@param[out] keyframe 是否为关键帧I帧: =1为I帧 =0其他视频帧
 *@return 成功返回0 失败返回-1 文件读完返回1
 *@note   仅在读AVI操作模式下面工作, 参数keyframe仅支持视频帧
 */
int ReadAviFrame(int read_inx, char *FrameData, int max_frame_size, int *framesize, int *frametype, int *keyframe)
{
    if (NULL==FrameData || NULL==framesize || NULL==frametype)
    {
        AVI_ERR("NULL POINTER EXCEPTION\n");
        return -1;
    }

    read_inx = (read_inx >= MAX_READ_CHN_NUM) ? MAX_READ_CHN_NUM-1 : read_inx;

    if (0 == g_bStartReadAviMode[read_inx])
    {
//      AVI_ERR("Avi read mode not start!\n");
        return -1;
    }

    if (NULL == g_file_read_info[read_inx].avi_lib_handle)
    {
        AVI_ERR("avi_lib_handle[%d] is NULL!\n", read_inx);
        return -1;
    }

    /* Return codes:
    *    1 = 读的是视频
    *    2 = 读的是音频
    *    0 = 到达文件尾
    *   -1 = 视频缓冲区太小
    *   -2 = 音频缓冲区太小
    */
    int ret_val = AVI_read_data(g_file_read_info[read_inx].avi_lib_handle, FrameData, max_frame_size, (long *)framesize, keyframe);
    if (1 == ret_val)           // 读的是视频
    {
        *frametype = 1;
    }
    else if (2 == ret_val)      // 读的是音频
    {    
        *frametype = 0;
    }
    else if (0 == ret_val)      // 到达文件尾
    {
        return 1;
    }
    else                        // 出错
    {
        return -1;
    }

    return 0;
}

/*@breif ReadPrvFrame 读取PRV帧数据
 *@param[in]  read_inx 读的通道
 *@param[out] frame_head 帧头缓冲区
 *@param[out] frame_data 帧数据缓冲区
 *@param[in] frame_size 获取到的帧数据大小
 *@return 成功返回0 失败返回-1 文件读完返回1
 *@note   仅在读PRV操作模式下面工作
 */
int ReadPrvFrame(int read_inx, 
                       void *frame_head, 
                       unsigned char *frame_data, 
                       unsigned int frame_size,
                       int *file_offset)
{
    if (NULL==frame_head || NULL==frame_data || NULL==file_offset)
    {
        AVI_ERR("NULL POINTER EXCEPTION\n");
        return -1;
    }

    H264DATAPACKETHEADER *p_frame_head = frame_head;

    read_inx = (read_inx >= MAX_READ_CHN_NUM) ? MAX_READ_CHN_NUM-1 : read_inx;

    if (0 == g_bStartReadAviMode[read_inx])
    {
        *file_offset = 0;
        return -1;
    }

    if (g_prv_file_read_info[read_inx].fdes < 0)
    {
        *file_offset = 0;
        AVI_ERR("Invalid prv file fd [%d]!\n", read_inx);
        return -1;
    }

    int read_count = 0;

    /* 读帧头(28字节) */
    read_count = read (g_prv_file_read_info[read_inx].fdes, p_frame_head, sizeof(H264DATAPACKETHEADER));

    /* 读取长度为0表示文件读完毕 */
    if (0 == read_count)
    {
        goto READ_OVER;
    }

    /* 注意：
     *     由于无法区分文件读完或者读错(文件后面有16K的I帧信息)，
     *     这里一律按照读完处理，为了保证数据帧的完整性(即没有破
     *     坏的数据)，这里当由于帧头读错或者帧数据读错了之后，需
     *     要通过lseek将文件指针还原到前一帧数据的尾部
     */

    /* 判断读的长度是否足够 */
    if (sizeof(H264DATAPACKETHEADER) != read_count)
    {
        /* 文件指针还原到前一帧数据的尾部 */
        lseek(g_prv_file_read_info[read_inx].fdes, -read_count, SEEK_CUR);
        AVI_DBG("Read [%d] framehead len [%d != 28]!\n", read_inx, read_count);
        goto READ_OVER;
    }

    /* 判断接收的缓冲区是否足够 */
    if (frame_size < p_frame_head->nOverloadLen)
    {
        /* 文件指针还原到前一帧数据的尾部 */
        lseek(g_prv_file_read_info[read_inx].fdes, -read_count, SEEK_CUR);
        AVI_DBG("Recv data buffer len [%d < %d]!\n", frame_size, p_frame_head->nOverloadLen);
        goto READ_OVER;
    }

    /* 校验帧头(当读到文件尾的时候，也会校验头出错) */
    if (p_frame_head->DataPacketStartCode != DATA_PAKT_START_CODE)
    {
        /* 文件指针还原到前一帧数据的尾部 */
        lseek(g_prv_file_read_info[read_inx].fdes, -read_count, SEEK_CUR);
        AVI_DBG("Framehead start code [%#x < %#x]!\n", p_frame_head->DataPacketStartCode, DATA_PAKT_START_CODE);
        goto READ_OVER;
    }

    /* 读取帧数据 */
    read_count = read (g_prv_file_read_info[read_inx].fdes, frame_data, p_frame_head->nOverloadLen);

    /* 读取长度为0表示文件读完毕 */
    if (0 == read_count)
    {
        /* 文件指针还原到前一帧数据的尾部 */
        lseek(g_prv_file_read_info[read_inx].fdes, -read_count, SEEK_CUR);
        goto READ_OVER;
    }

    /* 判断读的长度是否足够 */
    if (p_frame_head->nOverloadLen != read_count)
    {
        /* 文件指针还原到前一帧数据的尾部 */
        lseek(g_prv_file_read_info[read_inx].fdes, -(read_count+sizeof(H264DATAPACKETHEADER)), SEEK_CUR);
        AVI_DBG("Read [%d] framedata len [%d != %d]!\n", read_inx, p_frame_head->nOverloadLen, read_count);
        goto READ_OVER;
    }

    *file_offset = lseek(g_prv_file_read_info[read_inx].fdes, 0, SEEK_CUR);
    return 0;

READ_OVER:

    *file_offset = lseek(g_prv_file_read_info[read_inx].fdes, 0, SEEK_CUR);
    return 1;
}

/*@breif GetIFrameFileInfo 从I帧的索引文件里面获取I帧信息
 *@param[out] tmp_file_name 获取到的TMP文件名
 *@param[out] p_iframeinfos 获取到的I帧信息
 *@param[out] p_iframe_count 获取到的有效的I帧个数
 *@return 成功返回0 失败返回-1
 */
int GetIFrameFileInfo(char *tmp_file_name, void **p_iframeinfos, int *p_iframe_count)
{
    if (!tmp_file_name || !p_iframeinfos || !p_iframe_count)
    {
        AVI_ERR("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    if (0 == strlen(g_szFrameFileName))
    {
        strncpy(g_szFrameFileName, DEFAULE_FRAME_INFO_FILE, sizeof(g_szFrameFileName) - 1);
    }

    if (access(g_szFrameFileName, F_OK))
    {
        AVI_WRAN("iframes file %s not exist!\n", g_szFrameFileName);
        return -1;
    }

    int i = 0;
    int fd = -1;
    int read_count = -1;
    int iframe_count = 0;

    fd = open(g_szFrameFileName, O_RDONLY);
    if (fd < 0)
    {
        AVI_WRAN("Fail to open %s\n", g_szFrameFileName);
        return -1;
    }

    *p_iframeinfos = g_struIFameInfo;

    /* 读取文件名(MAX_PATH_STR_NUM字节) */
    read_count = read(fd, tmp_file_name, MAX_PATH_STR_NUM);
    if (read_count < MAX_PATH_STR_NUM)
    {
        AVI_WRAN("Fail to read %s name\n", g_szFrameFileName);
        close(fd);
        fd = -1;
        return -1;
    }

    /* 读取I帧信息(32KB) */
    memset(g_struIFameInfo, 0, sizeof(g_struIFameInfo));
    read_count = read(fd, g_struIFameInfo, sizeof(g_struIFameInfo));

#if 0
    if (read_count < sizeof(g_struIFameInfo))
    {
        AVI_SYS("Fail to read %s iframe infos\n", IFRAME_INFO_FILE);
        close(fd);
        fd = -1;
        return -1;
    }
#endif

    close(fd);
    fd = -1;

    /* 有效的I帧数量 */
    for (i=0; i<MAX_REC_FILE_I_FRAME_NUM; i++)
    {
        if (g_struIFameInfo[i].CurIFrameTime>0 && g_struIFameInfo[i].ShowTime>0)
        {
            iframe_count ++;
        }
    }

    *p_iframe_count = iframe_count;

    /* 文件简单校验 */
    if (access(tmp_file_name, F_OK))
    {
        AVI_ERR("tmp file %s not exist!\n", tmp_file_name);
        return -1;
    }

#if 0
    struct stat statbuff;
    memset(&statbuff, 0, sizeof(statbuff));
    stat(tmp_file_name, &statbuff);
    if (statbuff.st_size < )
    {
        
    }
#endif

    return 0;
}

/*@breif WriteFrameToAvi 将264文件逐帧写入AVI文件
 *@param[in]  frame_size 写入的帧数据大小
 *@param[in]  frame_buffer 写入的帧数据缓冲区
 *@param[in]  frame_head 写入的私有帧数据头
 *@param[out] file_pos  当前文件读写位置
 *@return 成功返回0 失败返回-1
 *@note   仅在写AVI操作模式下面工作
 */
int WriteFrameToAvi(int frame_size, unsigned char * frame_buffer, unsigned char *frame_head, unsigned int *file_pos, int force_flush)
{
    if (0 == g_bStartWriteAviMode)
    {
        AVI_ERR("Avi write mode not start!\n");
        return -1;
    }

    if (NULL==frame_buffer || NULL==frame_head)
    {
        AVI_ERR("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    H264DATAPACKETHEADER *pPack = (H264DATAPACKETHEADER *)frame_head;

    /* 只有I帧和P帧才算是有效帧，才会进行统计(注意：还有其它种类的I帧,这里暂不处理) */
    if (I_FRAME==pPack->FrameType || P_FRAME==pPack->FrameType || AUDIO_FRAME==pPack->FrameType)
    {
        /* 直接将私有数据写入打开的文件 */
       WritePrvFrame((H264DATAPACKETHEADER *)frame_head, (unsigned char*)frame_buffer,force_flush);
        *file_pos = g_prv_file_write_info.next_write_pos;
    }

    return 0;
}

void RefreshFrameCntAndIFrameInfo(H264DATAPACKETHEADER *pframe_head, unsigned int write_pos)
{
    if (AUDIO_FRAME == pframe_head->FrameType)
    {
        g_nCurrentAudioFrameNum ++;
    }
    else
    {
        /* 如果当前是I帧，则需要记录该I帧的位置(注意：还有其它种类的I帧,这里暂不处理) */
        if (I_FRAME == pframe_head->FrameType)
        {
            g_iframe_info[g_nCurrentIFrameNum].CurIFrameTime = pframe_head->u32RealTimeSec;
            g_iframe_info[g_nCurrentIFrameNum].ShowTime         = pframe_head->ShowTime;
            g_iframe_info[g_nCurrentIFrameNum].IFrameNum        = g_nCurrentVideoFrameNum;
            g_iframe_info[g_nCurrentIFrameNum].CurrFramePos     = write_pos;
            g_nCurrentIFrameNum ++;
        }
        g_nCurrentVideoFrameNum ++;
    }

    return;
}

int InitAviWriteBuf()
{
    memset(&TFWriteCache, 0, sizeof(stuWiteBufInfo));
    TFWriteCache.pDataBuf = (char*) malloc (MAX_WRITE_BUF_SIZE);
    if (TFWriteCache.pDataBuf == NULL)
    {
        AVI_WRAN("[WriteMode] Fail to malloc write buf\n");
        return -1;
    }

    return 0;
}

int UnInitAviWriteBuf()
{
    TFWriteCache.FrameCnt = 0;
    TFWriteCache.DataBufLen = 0;

    if (TFWriteCache.pDataBuf != NULL)
    {
        free(TFWriteCache.pDataBuf);
        TFWriteCache.pDataBuf = NULL;
    }
    return 0;
}


/*@breif StopAviAction 停止相关AVI操作
 *@param[in]  bStopReading 0表示停止写AVI操作 =1表示停止读AVI操作 
 *@param[in]  read_inx 读的通道, 写模式无视
 *@return 成功返回0 失败返回-1
 */
int StopAviAction(int bStopReading, int read_inx)
{
    /* 读模式和写模式不能相互影响 */
    if (bStopReading)
    {
        read_inx = (read_inx >= MAX_READ_CHN_NUM) ? MAX_READ_CHN_NUM-1 : read_inx;
        if (g_prv_file_read_info[read_inx].fdes <= 0)
        {
        }
        else
        {
            close(g_prv_file_read_info[read_inx].fdes);
            g_prv_file_read_info[read_inx].fdes = 0;
        }
        memset(&g_prv_file_read_info[read_inx], 0, sizeof(stuPrvInfo));
        g_bStartReadAviMode[read_inx] = 0;
    }
    else
    {
        if(g_prv_file_write_info.fdes <= 0)
        {
        }
        else
        {
            if (1)
            {
                /**
                 * 1.注意写文件时，文件可能已经存在（也即当前写指针不一定在文件末尾）
                 * 2.注意文件的大小不能超过14M，附加信息必须要写在文件的尾部
                 * 3.录像类型的附加信息这个模块没有数据。文件只能顺序写，因此要留空间
                 */
                const char bufTmpForType[AVIAPI_VIDEO_TYPE_INFO_SIZE] = {};  // 用于写预留的类型数据
                const off_t nSeekCur = lseek (g_prv_file_write_info.fdes, 0, SEEK_CUR);
                const off_t nSeekTail = lseek (g_prv_file_write_info.fdes, 0, SEEK_END);

                #if 0
                if (nSeekCur > (AVIAPI_VIDEO_FILESIZE - AVIAPI_VIDEO_ADD_INFO_SIZE) ||
                    nSeekTail > AVIAPI_VIDEO_FILESIZE)
                {
                    /* 这里如果出错，则下面的逻辑可能有问题 */
                    AVI_ERR ("inner error, cur=%d,size=end=%d\n", nSeekCur, nSeekTail);
                }
                #endif

                /* nSeekCur:  当前文件指针的位置，指的是VideoData最后的数据
                 * nSeekTail: 当前文件的尾部位置，有以下几种：
                 *            1. 当前是新建文件: nSeekTail == nSeekCur;
                 *            2. 当前是覆盖写文件，但是旧文件小于等于现在的文件: nSeekTail == nSeekCur;
                 *            3. 当前是覆盖写文件，但是旧文件大于现在的文件: nSeekTail > nSeekCur;
                 */
                if (nSeekCur < (nSeekTail - AVIAPI_VIDEO_ADD_INFO_SIZE)) // 当前的位置+需要填充的信息<原来文件大小
                {
                    lseek (g_prv_file_write_info.fdes, 0 - AVIAPI_VIDEO_ADD_INFO_SIZE, SEEK_END);
                    if (sizeof(bufTmpForType) > 0)
                    {
                        write (g_prv_file_write_info.fdes, bufTmpForType, sizeof(bufTmpForType));
                    }
                }
                else 
                {
                    lseek (g_prv_file_write_info.fdes, nSeekCur, SEEK_SET);
                    if (sizeof(bufTmpForType) > 0)
                    {
                        write (g_prv_file_write_info.fdes, bufTmpForType, sizeof(bufTmpForType));
                    }
                }
            }

            /* 在文件后面写AVIAPI_VIDEO_FRAME_INFO_SIZE的I帧信息 */
            write(g_prv_file_write_info.fdes, g_iframe_info, sizeof(g_iframe_info));

            /* 关闭文件 */
            close(g_prv_file_write_info.fdes);
            g_prv_file_write_info.fdes = -1;
        }
        g_bStartWriteAviMode = 0;
        g_nCurrentVideoFrameNum = 0;
        g_nCurrentAudioFrameNum = 0;
    }
    
    return 0;
}

/*@breif WriteRawFrameToAvi 将原始264文件逐帧写入AVI文件
 *@param[in]  frame_size 写入的帧数据大小
 *@param[in]  frame_buffer 写入的帧数据缓冲区
 *@param[in]  frame_head 写入的私有帧数据头
 *@return 成功返回0 失败返回-1
 *@note   仅在写AVI操作模式下面工作
 */
int WriteRawFrameToAvi(int frame_size, unsigned char * frame_buffer, int bAudio, int keyframe)
{
    if (0 == g_bStartWriteAviMode)
    {
        AVI_ERR("Avi write mode not start!\n");
        return -1;
    }

    if (NULL == frame_buffer)
    {
        AVI_ERR("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    if (NULL == g_file_write_info.avi_lib_handle)
    {
        AVI_ERR("avi_lib_handle[write] is NULL!\n");
        return -1;
    }

    unsigned int file_pos = 0;
    /* 需要将纯净的H264数据写入前面打开的AVI文件(传入的是原始的PCM格式的音频帧) */
    if (WriteAviFrame(g_file_write_info.avi_lib_handle, frame_size, (unsigned char*)frame_buffer, ((bAudio==1)?AUDIO_FRAME:keyframe), &file_pos))
    {
        AVI_ERR("Fail to called WriteAviFrame\n");
        return -1;
    }

    return 0;
}


