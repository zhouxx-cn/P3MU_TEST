
/*!
*\file   CDPopM.cpp 
*\brief  数据记录
*\author fc
*\date	  2009/03/24
*\version 0.1
*/
#ifdef __SUPPORT_SD_CARD__
#include "CDRecord.h"
#include "pkg_interface.h"
#include "pthreadmacro.h"
// #include "sys_banned.h"
#include "global_gather.h"
int CDRecord::SetRecordType(int nCh, int nRecType)
{
    LOCK(m_mutex);
	
    //ZW_Pkg_SetSegType(nRecType);
    
    UNLOCK(m_mutex);
	
	return 0;
}

/* 触发录像完毕之后，发送消息的回调函数 */
int CDRecord::SetTrigRecordOverCallBack(int (*CallBack)(unsigned char *))
{
    ZW_Pkg_SetTrigRecordOverCallBack(CallBack);

	return 0;
}

//!\brief 设置预录时间(单位: 秒)
int CDRecord::SetPrevTimeSec(int nCh, unsigned int prev_time_sec)
{
    LOCK(m_mutex);
	
    ZW_Pkg_SetPrevTimeSec(prev_time_sec);
    
    UNLOCK(m_mutex);
	
	return 0;
}

/*@breif SetCurrentAviNumPerPage 设置每页显示多少个AVI文件
 *@param [IN]  avi_num_per_page 每页显示的AVI文件个数
 *@return 成功返回0
 *@       失败返回-1
 *@note  每页显示的AVI文件个数不能超过当前有效的AVI个数g_read_curr_avi_num，
 *@      每页显示的AVI文件个数不能超过最大能显示的AVI个数MAX_AVI_NUM_ONE_PAGE
 */
int CDRecord::SetCurrentAviNumPerPage(int avi_num_per_page)
{
    LOCK(m_mutex);
	
    int ret = ZW_PkgSd_SetCurrentAviNumPerPage(avi_num_per_page);
    
    UNLOCK(m_mutex);

    return ret;
}

/*@breif GetCurrentAviTotalNum 获取当前AVI总个数
 *@return 成功返回当前总的AVI文件个数
 *@       失败返回-1
 */
int CDRecord::GetCurrentAviTotalNum(void)
{
    int nCount = 0;
    
    LOCK(m_mutex);
	
    nCount = ZW_PkgSd_GetCurrentAviTotalNum();
    
    UNLOCK(m_mutex);

    return nCount;
}

/*@breif SetSwitchPageIsChoose 设置当前选中的是哪一个页面和当前录像路径类型
 *@param [IN]  choose_page 选中的页面(从1开始)
 *@param [IN]  path_type 当前录像路径类型 0: http路径  1:绝对路径
 *@return 成功返回0
 *@      失败返回-1
 */
int CDRecord::SetSwitchPageIsChoose(int choose_page, int path_type)
{
    LOCK(m_mutex);

    int ret = ZW_PkgSd_SetSwitchPageIsChoose(choose_page, path_type);

    UNLOCK(m_mutex);

    return ret;
}


/*@breif UpdateAviListByTime 通过给定的时间戳更新AVI文件列表
 *@param [IN]  start_time 要搜索的起始时间
 *@param [IN]  stop_time 要搜索的结束时间(为0表示搜索全部文件)
 *@param [OUT] p_search_avi_count 搜索到符合条件的AVI文件总个数
 *@return 成功返回0
 *@      失败返回-1
 */
int CDRecord::UpdateAviListByTime(unsigned int start_time, 
                                        unsigned int stop_time, 
                                        int *p_search_avi_count)
{
    LOCK(m_mutex);

    int ret = 0;
    ZW_PkgSd_UpdateAviListByTime(start_time,
                                           stop_time,
                                           p_search_avi_count);

    UNLOCK(m_mutex);

    return ret;
}

/*@breif GetCurrentAviListInfo 通过给定的时间戳获取对应的AVI文件列表
 *@param [IN]  read_avi_page 需要获取第几页的AVI信息(从1开始)
 *@param [OUT] p_avi_list 获取到的AVI文件列表
 *@param [OUT] p_avi_num_curr_page 当前页面有多少个AVI文件
 *@return 成功返回0
 *@      失败返回-1
 *@note  需要先调用SetCurrentAviNumPerPage设置当前用户需要设置每页显示多少个AVI文件
 */
int CDRecord::GetCurrentAviListInfo(int read_avi_page, void **p_avi_list, int *p_avi_num_curr_page)
{
    LOCK(m_mutex);
	
    int ret = ZW_PkgSd_GetCurrentAviListInfo(   read_avi_page, p_avi_list, p_avi_num_curr_page);
    
    UNLOCK(m_mutex);

    return ret;
}

/*@breif SetCurrentPicNumPerPage 设置每页显示多少个图片文件
 *@param [IN]  pic_num_per_page 每页显示的图片文件个数
 *@return 成功返回0
 *@       失败返回-1
 *@note  每页显示的图片文件个数不能超过当前有效的图片个数g_read_curr_pic_num，
 *@      每页显示的图片文件个数不能超过最大能显示的图片个数MAX_PIC_NUM_ONE_PAGE
 */
int CDRecord::SetCurrentPicNumPerPage(int pic_num_per_page)
{
    LOCK(m_mutex);
	
    int ret = ZW_PkgSd_SetCurrentPicNumPerPage(pic_num_per_page);
    
    UNLOCK(m_mutex);

    return ret;
}

/*@breif GetCurrentPicTotalNum 获取当前图片总个数
 *@return 成功返回当前总的图片文件个数
 *@       失败返回-1
 */
int CDRecord::GetCurrentPicTotalNum(void)
{
    int nCount = 0;
    
    LOCK(m_mutex);
	
    nCount = ZW_PkgSd_GetCurrentPicTotalNum();
    
    UNLOCK(m_mutex);

    return nCount;
}

/*@breif UpdatePicListByTime 通过给定的时间戳更新PIC文件列表
 *@param [IN]  start_time 要搜索的起始时间
 *@param [IN]  stop_time 要搜索的结束时间(为0表示搜索全部文件)
 *@param [OUT] p_search_pic_count 搜索到符合条件的PIC文件总个数
 *@return 成功返回0
 *@      失败返回-1
 */
int CDRecord::UpdatePicListByTime(unsigned int start_time, 
                                        unsigned int stop_time, 
                                        int *p_search_pic_count)
{
    LOCK(m_mutex);

    int ret = ZW_PkgSd_UpdatePicListByTime(start_time,
                                          stop_time,
                                          p_search_pic_count);

    UNLOCK(m_mutex);

    return ret;
}

/*@breif GetCurrentPicListInfo 通过给定的时间戳获取对应的图片文件列表
 *@param [IN]  read_pic_page 需要获取第几页的图片信息(从1开始)
 *@param [OUT] p_pic_list 获取到的图片文件列表
 *@param [OUT] p_pic_num_curr_page 当前页面有多少个图片文件 
 *@return 成功返回0
 *@      失败返回-1
 *@note  需要先调用GetPicGlobalHeadInfoFromGlobalFile将需要的数据放到全局结构体里面
 *@      需要先调用SetCurrentPicNumPerPage设置当前用户需要设置每页显示多少个图片文件
 */
int CDRecord::GetCurrentPicListInfo(int read_pic_page, void **p_pic_list, int *p_pic_num_curr_page)
{
    LOCK(m_mutex);
	
    int ret = ZW_PkgSd_GetCurrentPicListInfo(read_pic_page, p_pic_list, p_pic_num_curr_page);
    
    UNLOCK(m_mutex);
    
    return ret;
}

/* 获取磁盘资源占用情况(单位: MB) */
int CDRecord::GetDiskResourceSize(int *pRecordMB, int *pPictureMB)
{
    LOCK(m_mutex);

    int ret = ZW_PkgSd_GetDiskResourceSize(pRecordMB, pPictureMB);

    UNLOCK(m_mutex);

    return ret;
}

/* 获取录像存放的比例(范围0~100) */
int CDRecord::GetVideoUsedSpaceRate(void)
{
    LOCK(m_mutex);

    int video_rate = ZW_PkgSd_GetVideoUsedSpaceRate();

    UNLOCK(m_mutex);

    return video_rate;
}

/* 设置录像存放的比例(范围0~100) */
int CDRecord::SetVideoUsedSpaceRate(int nVideoSpace)
{
    LOCK(m_mutex);

    int ret = ZW_PkgSd_SetVideoUsedSpaceRate(nVideoSpace);

    UNLOCK(m_mutex);

    return ret;
}

/* 设置录像尺寸 */
int CDRecord::SetRecordSize(int width, int height, int bH265)
{
    LOCK(m_mutex);

    int ret = ZW_PkgSd_SetRecordSize(width, height, bH265);

    UNLOCK(m_mutex);

    return ret;
}

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
int CDRecord::GetFileReadWriteStatus(const char *file_name, 
                                            unsigned int *read_write_status,
                                            unsigned int *file_size)
{
    return ZW_PkgSd_GetFileReadWriteStatus(file_name, read_write_status, file_size);
}

/*@breif GetFileReadWriteStatus 获取指定时间段的录像数据大小
 *@param [IN] start_time 开始时间
 *@param [IN] stop_time  结束时间
 *@param [IN] record_type 筛选的录像类型(当前无效)
 *@param [OUT] p_size 获取到的帧数据大小
 *@return 返回  0 表示获取成功
 *@       返回 -1 表示获取失败
 *@note   获取到的是数据帧大小，包括帧头
 */
int CDRecord::GetRecordDataSize(unsigned int start_time, 
                                         unsigned int stop_time, 
                                         int record_type, 
                                         unsigned int * p_size)
{
    return ZW_PkgSd_SDOPS_GetRecordDataSize(start_time, stop_time, record_type, p_size);
}

/* 获取当前回放的录像信息 */
int CDRecord::GetRecordInfo(int *pCurrentRecordStartTime)
{
    return ZW_PkgSd_SDOPS_GetRecordInfo(pCurrentRecordStartTime);
}

/* 获取文件对应的文件信息 */
int CDRecord::GetRecordFileInfoByFileName(const char *pFileName, void* pFileInfo)
{
    return ZW_PkgSd_SDOPS_GetRecordFileInfoByFileName(pFileName, pFileInfo);
}

/* 设置获取时间偏移回调 */
int CDRecord::SetGetTimeOffsetCallBack(int (*CallBack)(int *, int *, int *))
{
    return ZW_PkgSd_SetGetTimeOffsetCallBack(CallBack);
}

/* 传入的图片名字组成pic_name: 时间戳_标号，如:1532477851_01需要改为1532477851 */
int CDRecord::UploadPictureToSdCard(const char *pic_name, unsigned int trig_type)
{
    LOCK(m_mutex);

    int ret = ZW_PkgSd_UploadPictureToSdCard(pic_name, trig_type);
    UNLOCK(m_mutex);

    return ret;
}

//!\brief 获取预录时间(单位: 秒)
unsigned int CDRecord::GetPrevTimeSec(int nCh)
{
    unsigned int prev_time_sec = 0;
    
    LOCK(m_mutex);
    
    prev_time_sec = ZW_Pkg_GetPrevTimeSec();
    
    UNLOCK(m_mutex);
	
	return prev_time_sec;
}

//!\brief 设置录像延迟时间（单位：秒）
int CDRecord::SetRecordDelayTime (int nCh, unsigned int nDelaySecs)
{
    LOCK (m_mutex);
    int ret = ZW_Pkg_SetRecordDelayTime (nDelaySecs);
    UNLOCK(m_mutex);

    return ret;
}

//!\brief 获取录像延迟时间（单位：秒）
int CDRecord::GetRecordDelayTime (int nCh, unsigned int *pnDelaySecs)
{
    LOCK (m_mutex);
    int ret = ZW_Pkg_GetRecordDelayTime (pnDelaySecs);
    UNLOCK(m_mutex);

    return ret;
}

//!\brief 暂停记录数据
//!\param 无
//!\return  成功返回0，失败为-1
int CDRecord::PauseRecord(int nCh, int bPause)
{
	int ret = 0;
	return ret;
}

int CDRecord::DelRecordData(HANDLE Dev, int nCh, int nRecType, time_t tStartTime, time_t tEndTime)
{
	return 0;
}

int CDRecord::DelRecordData(HANDLE Dev, time_t tStartTime, time_t tEndTime)
{
	return 0;
}

int CDRecord::StartRecordData(int nCh, int record_type, int bH265)
{
    LOCK(m_mutex);

    ZW_Pkg_StartRecord(record_type, bH265);

    UNLOCK(m_mutex);
	
	return (int) 0;
}

int CDRecord::InputDataToRingBuffer(const char *Buff, int nLen)
{
    int ret = ZW_PkgSd_DataInput(Buff, nLen);
    
    return (int) ret;
}

int CDRecord::StopRecordAndUinitMemRecord(void)
{
  LOCK(m_mutex);
    ZW_PkgSd_StopRecordAndUinitMemRecord();
  UNLOCK(m_mutex);

    return (int) 0;
}

CDRecord::CDRecord()
{
	INIT_MUTEX(m_mutex);

	m_nRecordType = -1;
}

int CDRecord::InputData(int nCh, time_t tFrameTime, unsigned long tFrameTimeInUs, const char *Buff, int nLen, int nFramType)
{
	return 0;
}
#endif