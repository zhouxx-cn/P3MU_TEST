/*****************************************************************************
* Copyright (C) 2017 All rights reserved.
*  文件名称：aviapi.h
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
*               由于采用静态数组的形式存放，故当前AVI的大小限定为6~7MB，可以通过宏MAX_6M_AVI_FILE_I_FRAME_NUM来调节当
*               前AVI里面I帧的个数用以存放更大或者更小的AVI文件；
*            7. AVI回放定位以视频帧为主，支持音视频在非全帧率下面的实时同步；
*  更新日志：
*  
******************************************************************************/

#ifndef __AVI_API__
#define __AVI_API__

#include "avilib.h"

#ifdef __cplusplus
extern "C" {
#endif

/*@breif InitAviActionMode 初始化AVI操作的模式
 *@param[in]  avi_ops_file 操作的AVI文件名
 *@param[in] bReadAviMode 0:写AVI模式 1:读AVI模式 
 *@param[in] read_inx  读的通道，写模式无视
 *@return 成功返回0 失败返回-1
 */
int InitAviActionMode(const char *avi_ops_file, int bReadAviMode, int read_inx);

/* 设置帧索引的文件名 */
int SetFrameFileName(const char *p_frame_files_name);

/*@breif GetIFrameFileInfo 从I帧的索引文件里面获取I帧信息
 *@param[out] tmp_file_name 获取到的TMP文件名
 *@param[out] p_iframeinfos 获取到的I帧信息
 *@param[out] p_iframe_count 获取到的有效的I帧个数
 *@return 成功返回0 失败返回-1
 */
int GetIFrameFileInfo(char *tmp_file_name, void **p_iframeinfos, int *p_iframe_count);

/*@breif WriteFrameToAvi 将264文件逐帧写入AVI文件
 *@param[in]  frame_size 写入的帧数据大小
 *@param[in]  frame_buffer 写入的帧数据缓冲区
 *@param[in]  frame_head 写入的私有帧数据头
 *@param[out] file_pos  当前文件读写位置
 *@return 成功返回0 失败返回-1
 *@note   仅在写AVI操作模式下面工作
 */
int WriteFrameToAvi(int frame_size, unsigned char * frame_buffer, unsigned char *frame_head, unsigned int *file_pos, int force_flush);

/*@breif WriteRawFrameToAvi 将原始264文件逐帧写入AVI文件
 *@param[in]  frame_size 写入的帧数据大小
 *@param[in]  frame_buffer 写入的帧数据缓冲区
 *@param[in]  frame_head 写入的私有帧数据头
 *@return 成功返回0 失败返回-1
 *@note   仅在写AVI操作模式下面工作
 *@note   传入的音频数据需要是原始的PCM格式的音频数据
 */
int WriteRawFrameToAvi(int frame_size, unsigned char * frame_buffer, int bAudio, int keyframe);

/*@breif SetAviIndexPosition 设置AVI索引的帧位置
 *@param[in]  read_inx 读的通道
 *@param[in]  value 当为AVI格式的时候，value是索引帧号；当未PRV格式的时候，value是位置偏移pos
 *@return 成功返回0 失败返回-1
 *@note   仅在读操作模式下面工作
 */
int SetAviIndexPosition(int read_inx, int value);

/*@breif SetAviRecSize 设置录像尺寸
 *@param[in]  width 录像宽
 *@param[in]  height 录像高
 *@param[in]  bH265 当前是否是265编码
 *@return 成功返回0 失败返回-1
 *@note 仅在写操作模式下面工作，且编码参数只有在创建录像文件时才能有效设置，其他情况设置无效
 */
int SetAviRecSize(int width, int height, int bH265);

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
int ReadAviFrame(int read_inx, char *FrameData, int max_frame_size, int *framesize, int *frametype, int *keyframe);

/*@breif ReadPrvFrame 读取PRV帧数据
  *@breif ReadPrvFrame 读取PRV帧数据
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
                        int *file_offset);

/*@breif GetAviVideoParam 获取AVI视频参数
 *@param[in]   read_inx  读的通道
 *@param[out]  pWidth 当前AVI视频的宽度
 *@param[out]  pHight 当前AVI视频的高度
 *@param[out]  pFramerate 当前AVI视频的帧率
 *@param[out]  pTotalFrames 当前AVI视频的总帧数
 *@return 成功返回0 失败返回-1
 *@note   仅在读AVI操作模式下面工作
 */
int GetAviVideoParam(int read_inx, int *pWidth, int *pHight, double *pFramerate, int *pTotalFrames);

/*@breif GetAviAudioParam 获取AVI音频参数
 *@param[in]   read_inx  读的通道
 *@param[out]  pAudioMp3Rate 获取音频mp3码率(kbs)
 *@param[out]  pBits 获取音频每秒的采样数据位宽(8bit, 16bit, ...)
 *@param[out]  pRate 获取音频采样率(8k,16k,...)
 *@param[out]  pBytes 获取音频字节数
 *@return 成功返回0 失败返回-1
 *@note   仅在读AVI操作模式下面工作
 */
int GetAviAudioParam(int read_inx, long *pAudioMp3Rate, int *pBits, long *pRate, long *pBytes);

/*@breif StopAviAction 停止相关AVI操作
 *@param[in]  bStopReading 0表示停止写AVI操作 =1表示停止读AVI操作
 *@param[in]  read_inx 读的通道, 写模式无视
 *@return 成功返回0 失败返回-1
 */
int StopAviAction(int bStopReading, int read_inx);

#ifdef __cplusplus
}
#endif
#endif
