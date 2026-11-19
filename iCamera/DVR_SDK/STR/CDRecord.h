
/*!
*\file   CDRecord.h 
*\brief  数据记录声明
*\author cwf
*\date	  2009/03/24
*\version 0.1
*/

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CDRECORDE_49A645BB0392_INCLUDED
#define _INC_CDRECORDE_49A645BB0392_INCLUDED
#include <time.h>
#include <stdio.h>
#include "pthread.h"
#include "HANDLE.h"

//数据记录
//该类成员函数为虚函数，不提供实现方法，具体到不同的文件系统需
//要不同的实现
//##ModelId=49A645BB0392
class CDRecord
{
  public:
  
    //!\brief 设置录像类型
    //!\param int nRecType 
    //数据类型。0为普通、1为报警、2、移动侦测、3为手动
    int SetRecordType(int nCh, int nRecType);

    /* 传入的图片名字组成pic_name: 时间戳_标号，如:1532477851_01需要改为1532477851 */
    int UploadPictureToSdCard(const char *pic_name, unsigned int trig_type);

    /* 触发录像完毕之后，发送消息的回调函数 */
    int SetTrigRecordOverCallBack(int (*CallBack)(unsigned char *));

    //!\brief 设置预录时间(单位: 秒)
    int SetPrevTimeSec(int nCh, unsigned int prev_time_sec);

    //!\brief 获取预录时间(单位: 秒)
    unsigned int GetPrevTimeSec(int nCh);

    //!\brief 设置录像延迟时间（单位：秒）
    int SetRecordDelayTime (int nCh, unsigned int nDelaySecs);

    //!\brief 获取录像延迟时间（单位：秒）
    int GetRecordDelayTime (int nCh, unsigned int *pnDelaySecs);

    //!\brief 设置录像写入状态 0:暂停  1:写入
    int SetRecordWriteStatus(int record_status);

	//!\brief 暂停记录数据
	//!\param 无
	//!\return  成功返回0，失败为-1
	int PauseRecord(int nCh, int bPause);

	//!\brief 删除指定通道在指定时间段内的数据，并不影响轮转过程，只是将指定时间段某通道的数据索引项屏蔽；相对应帧数据清0(此项也可不做)。
	//!\param HANDLE dev 盘组句柄
	//!\param time_t TStartTime 时间段之起始时间。
	//!\param time_t tEndTime 时间段之结束时间。
	//!\return  成功返回0，失败为-1
	int DelRecordData(HANDLE dev, int nCh, int nRecType, time_t tStartTime, time_t tEndTime);

	//!\brief 删除指定通道在指定时间段内的数据，并不影响轮转过程，只是将指定时间段某通道的数据索引项屏蔽；相对应帧数据清0(此项也可不做)。
	//!\param HANDLE dev 盘组句柄
	//!\param time_t TStartTime 时间段之起始时间。
	//!\param time_t tEndTime 时间段之结束时间。
	//!\return  成功返回0，失败为-1
	static int DelRecordData(HANDLE dev, time_t tStartTime, time_t tEndTime);

	//!\brief 开始记录数据
	//!\return  成功返回0，失败为-1
	int StartRecordData(int nCh, int record_type, int bH265);

	//!\brief灌入数据
	//!\param const char* Buff 数据Buff
	//!\param  int nLen 数据长度
	//!\param int nFramType 数据类型
	//!\return  成功返回0，失败为-1
	int InputData(int nCh, time_t tFrameTime, unsigned long tFrameTimeInUs, const char *Buff, int nLen, int nFramType);
    
    int InputDataToRingBuffer(const char *Buff, int nLen);

    int StopRecordAndUinitMemRecord(void);

    /*@breif SetCurrentAviNumPerPage 设置每页显示多少个AVI文件
     *@param [IN]  avi_num_per_page 每页显示的AVI文件个数
     *@return 成功返回0
     *@       失败返回-1
     *@note  每页显示的AVI文件个数不能超过当前有效的AVI个数g_read_curr_avi_num，
     *@      每页显示的AVI文件个数不能超过最大能显示的AVI个数MAX_AVI_NUM_ONE_PAGE
     */
    int SetCurrentAviNumPerPage(int avi_num_per_page);

    /*@breif GetCurrentAviTotalNum 获取当前AVI总个数
     *@return 成功返回当前总的AVI文件个数
     *@       失败返回-1
     */
    int GetCurrentAviTotalNum(void);

    /*@breif UpdateAviListByTime 通过给定的时间戳更新AVI文件列表
     *@param [IN]  start_time 要搜索的起始时间
     *@param [IN]  stop_time 要搜索的结束时间(为0表示搜索全部文件)
     *@param [OUT] p_search_avi_count 搜索到符合条件的AVI文件总个数
     *@return 成功返回0
     *@      失败返回-1
     */
    int UpdateAviListByTime(unsigned int start_time, 
                                  unsigned int stop_time, 
                                  int *p_search_avi_count);

   /*@breif SetSwitchPageIsChoose 设置当前选中的是哪一个页面和当前录像路径类型
    *@param [IN]  choose_page 选中的页面(从1开始)
    *@param [IN]  path_type 当前录像路径类型 0: http路径  1:绝对路径
    *@return 成功返回0
    *@      失败返回-1
    */
    int SetSwitchPageIsChoose(int choose_page, int path_type);

    /*@breif GetCurrentAviListInfo 通过给定的时间戳获取对应的AVI文件列表
     *@param [IN]  read_avi_page 需要获取第几页的AVI信息(从1开始)
     *@param [OUT] p_avi_list 获取到的AVI文件列表
     *@param [OUT] p_avi_num_curr_page 当前页面有多少个AVI文件
     *@return 成功返回0
     *@      失败返回-1
     *@note  需要先调用SetCurrentAviNumPerPage设置当前用户需要设置每页显示多少个AVI文件
     */
    int GetCurrentAviListInfo(int read_avi_page, void **p_avi_list, int *p_avi_num_curr_page);

    /*@breif SetCurrentPicNumPerPage 设置每页显示多少个图片文件
     *@param [IN]  pic_num_per_page 每页显示的图片文件个数
     *@return 成功返回0
     *@       失败返回-1
     *@note  每页显示的图片文件个数不能超过当前有效的图片个数g_read_curr_pic_num，
     *@      每页显示的图片文件个数不能超过最大能显示的图片个数MAX_PIC_NUM_ONE_PAGE
     */
    int SetCurrentPicNumPerPage(int pic_num_per_page);

    /*@breif GetCurrentPicTotalNum 获取当前图片总个数
     *@return 成功返回当前总的图片文件个数
     *@       失败返回-1
     */
    int GetCurrentPicTotalNum(void);

    /*@breif UpdatePicListByTime 通过给定的时间戳更新PIC文件列表
     *@param [IN]  start_time 要搜索的起始时间
     *@param [IN]  stop_time 要搜索的结束时间(为0表示搜索全部文件)
     *@param [OUT] p_search_pic_count 搜索到符合条件的PIC文件总个数
     *@return 成功返回0
     *@      失败返回-1
     */
    int UpdatePicListByTime(unsigned int start_time, 
                                  unsigned int stop_time, 
                                  int *p_search_pic_count);

    /*@breif GetCurrentPicListInfo 通过给定的时间戳获取对应的图片文件列表
     *@param [IN]  read_pic_page 需要获取第几页的图片信息(从1开始)
     *@param [OUT] p_pic_list 获取到的图片文件列表
     *@param [OUT] p_pic_num_curr_page 当前页面有多少个图片文件 
     *@return 成功返回0
     *@      失败返回-1
     *@note  需要先调用GetPicGlobalHeadInfoFromGlobalFile将需要的数据放到全局结构体里面
     *@      需要先调用SetCurrentPicNumPerPage设置当前用户需要设置每页显示多少个图片文件
     */
    int GetCurrentPicListInfo(int read_pic_page, void **p_pic_list, int *p_pic_num_curr_page);

    /* 获取磁盘资源占用情况(单位: MB) */
    int GetDiskResourceSize(int *pRecordMB, int *pPictureMB);

    /* 获取录像存放的比例(范围0~100) */
    int GetVideoUsedSpaceRate(void);
    
    /* 设置录像存放的比例(范围0~100) */
    int SetVideoUsedSpaceRate(int nVideoSpace);

    /* 设置录像尺寸 */
    int SetRecordSize(int width, int height, int bH265);

    /*@breif GetFileReadWriteStatus 获取当前文件的读写状态
     *@param [IN]  file_name 需要操作的文件名(绝对路径)
     *@param [OUT] read_write_status 
     *@            0:  当前文件空闲
     *@            1:  当前文件正在读
     *@            2:  当前文件正在写
     *@param [OUT] file_size 文件大小(单位: 字节) 
     *@return 返回0表示成功
     *@      返回-1表示失败
     */
    int GetFileReadWriteStatus(const char *file_name, 
                                      unsigned int *read_write_status,
                                      unsigned int *file_size);

    /*@breif GetFileReadWriteStatus 获取指定时间段的录像数据大小
     *@param [IN] start_time 开始时间
     *@param [IN] stop_time  结束时间
     *@param [IN] record_type 筛选的录像类型(当前无效)
     *@param [OUT] p_size 获取到的帧数据大小
     *@return 返回  0 表示获取成功
     *@       返回 -1 表示获取失败
     *@note   获取到的是数据帧大小，包括帧头
     */
    int GetRecordDataSize(unsigned int start_time, 
                                   unsigned int stop_time, 
                                   int record_type, 
                                   unsigned int * p_size);

    /* 获取当前回放的录像信息 */
    int GetRecordInfo(int *pCurrentRecordStartTime);

    /* 获取文件对应的文件信息 */
    int GetRecordFileInfoByFileName(const char *pFileName, void* pFileInfo);

    /* 设置获取时间偏移回调 */
    int SetGetTimeOffsetCallBack(int (*CallBack)(int *, int *, int *));

    CDRecord();
    
  private:
	  pthread_mutex_t m_mutex;
	int m_nRecordType;


};

#endif							/* _INC_CDRECORDE_49A645BB0392_INCLUDED */
