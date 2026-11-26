#ifndef __H_SD_OPS__
#define __H_SD_OPS__

/*系统错误码定义*/
#define ERR_NO                    0x00000000

#define ERR_HANDLE_ALLOC_ERROR    0xc0000100
#define ERR_DDRAW_CREATE_FAILED   0xc0000101
#define ERR_RUNTIME_ERROR         0xc0000102
#define ERR_INVALID_HANDLE_PSS	  0xc0000103
#define ERR_WAIT_TIMEOUT          0xc0000104
#define ERR_INVALID_ARGUMENT      0xc0000105
#define ERR_INVALID_FILENAME      0xc0000108
#define ERR_TMMAN_FAILURE         0xC0000109
#define ERR_OUTOF_MEMORY          0xc000010A
#define ERR_NOT_SUPPORT           0xc000010C
#define ERR_SYS_NOT_INIT          0xc000010D
#define ERR_CHANNEL_NOT_REC       0xc000010E
#define ERR_DISK_OPEN             0xc000010F
#define ERR_DISK_SEEK             0xc0000110
#define ERR_DISK_FULL             0xc0000111

#define ERR_GET_BUF                      0xc0000112
#define ERR_REACH_MOST_NEW_POS           0xc0000113
#define USER_KEYPKG_GET_FINISH           0xc0000114	//用来表示用户关键帧查询结束
#define USER_KEYPKG_FIND_OK              0xc0000115	//用于在用户关键帧查询回调中控制查询结束
#define ERR_DISK_NOT_EXIST               0xc0000116	//硬盘分区可以OPEN但是无法访问
#define ERR_DISK_HEADAREA_NOT_USEABLE    0xc0000117	//硬盘分区的头部区域有坏扇区，无法使用
#define ERR_DISK_INDEXAREA_TOO_MUCH      0xc0000118	//硬盘分区的所有大块索引区都有坏块，无法使用

#define ERR_DISK_OPEN_FILE_FAIL 		 0xC0000002
#define ERR_DISK_HAND_FULL 			     0xC0000004
#define ERR_DISK_MALLOC_FAIL 			 0xC0000005
#define ERR_DISK_WRITE_FAIL 			 0xC000000A
#define ERR_DISK_READ_ONLY               0xC000000B	

//分区表中的文件系统类型值
typedef enum {
	FS_FAT32_VALUE = 0x0B
}FS_TYPE;

/* 触发类型 */
typedef enum{
    eHttpEvent = 0, // Http事件触发
    eMotion    = 1, // 移动侦测触发
    eAlarmIn_1 = 2, // 报警输入1触发
    eAlarmIn_2 = 3, // 报警输入0触发
    eVideoOcc  = 4, // 视频遮盖触发
    NONE_ALARM = 5  // 没有触发
}eTriggerType;

/* 传输类型 */
typedef enum{
    None     = 0, // 无传输
    ePicture = 1, // 传输图片
    eVideo   = 2, // 传输视频
    All      = 3, // 传输图片和视频
}eTransferType;

/* SD卡状态 */
typedef enum{    
    NoSdCard         = 0, // 无SD卡(启动检测)
    SdCardErr        = 1, // SD卡错误(启动/运行检测)
    SdCardReadOnly   = 2, // SD卡只读(启动/运行检测)
    SdCardReadWrite  = 3, // SD卡读写(启动/运行检测)
    SdCardFatfsErr   = 4, // SD卡FATFS错误(运行检测)
    SdCardMountErr   = 5, // SD卡挂载错误(启动检测)
    SdCardAddPartErr = 6, // SD卡添加分区错误(启动检测)
}eSdCardStatus;

/* SD卡格式化状态 */
typedef enum{
    UnFormatted  = 0, // 未格式化
    Formatting   = 1, // 格式化中
    FormatOk     = 2, // 格式化成功
    FormatErr    = 3, // 格式化失败
    UmountFail   = 4, // 卸载失败
    DeviceIsBusy = 5, // 设备忙
    NoDiskFound  = 6, // 无SD卡
}eSdCardFormatStatus;

/* SD卡写状态 */
typedef enum{
    SdCardIsIDEL    = 0, // SD卡空闲
    SdCardIsWriting = 1, // SD卡正在写
    SdCardWriteOk   = 2, // SD卡写完成
    SdCardWriteFail = 3, // SD卡写失败
}eSdCardWriteStatus;

typedef enum{
    /* 系统启动时进行磁盘检查(必须和checkSDCard一同修改) */
    SdCardBootCheck_NoNode          = 0,
    SdCardBootCheck_MountFail       = 1,
    SdCardBootCheck_DiskExp         = 2,
    SdCardBootCheck_AddPartFail     = 3,
    SdCardBootCheck_ReadWriteFail   = 4,
    SdCardBootCheck_DiskIsNormal    = 5,

    /* 系统运行时进行磁盘检查(必须和MonitorProcess一同修改) */
    SdCardRunCheck_DiskReadOnly     = 6,
    SdCardRunCheck_DiskIsError      = 7,
    SdCardRunCheck_FatfsIsError     = 8,
    SdCardRunCheck_DiskIsWritting   = 9,
    SdCardRunCheck_DiskIsIdle       = 10,

    /* 格式化状态 */
    SdCardFormatStatus_UmountFail   = 11,
    SdCardFormatStatus_FormatFail   = 12,
    SdCardFormatStatus_IsFormatting = 13,
    SdCardFormatStatus_FormatOK     = 14
}eSdCardTotalInfoStatus;

/* 视频循环录像选择 */
typedef enum{
    DeleteFileAfter24H     = 0, // SD卡满了之后删除24小时之后的录像
    DeleteFileAfter48H     = 1, // SD卡满了之后删除48小时之后的录像
    DelOldFileWhenDiskFull = 2, // SD卡满了之后循环录像
    NoRecordWhenDiskFull   = 3, // SD卡满了之后不再录像
    NoneOperation          = 4, // 无
}eRecordCircleType;

/* 停止操作AVI的模式选择 */
typedef enum{
    STOP_WRITE_AVI = 0,    // 停止写入AVI文件
    STOP_READ_AVI  = 1,    // 停止读取AVI文件
}eSTOP_AVI_MODE;
typedef struct tagEventInfo{
    char picture_name[32];      // 欲传输图片名(可以为空)
    char video_name[32];        // 欲传输录像(可以为空)
    eTriggerType  trig_type;    // 触发类型
    eTransferType trans_type;   // 传输类型
    char resolved[952];         // 保留字段
}stuEventInfo, *LPEventInfo;

typedef struct tagSdCardStatus{
    eSdCardStatus        eSdcardStatus;      // SD卡状态
    unsigned long        dwTotalSpace;       // SD(分区)总大小(单位: MB)
    unsigned long        dwAvailableSpace;   // SD(分区)可用空间(单位: MB)
}stuSdCardStatus, *LPSdCardStatus;

/* 每个AVI的I帧信息，固定为16个字节 */
typedef struct tagIFrameInfo
{
    unsigned int CurIFrameTime;     // 当前时间
    unsigned int ShowTime;          // 当前I帧的相对时间戳
    unsigned int IFrameNum;         // 当前I帧编号在总视频帧里面的编号
    unsigned int CurrFramePos;      // 当前帧在文件中的偏移地址(PRV格式有效)
}__attribute__((packed))IFrameInfo, *pIFrameInfo;

/*
 * 单个图片的信息，固定为16个字节, 注意: 
 * 想要获取当前图片的实际名字，需要通过将name_time_stamp时间戳转化为可识别的
 * 年月日时分秒模式，然后，利用trig_pic_type加上前缀进行组合而成，用时间戳的
 * 方式是为了减小结构体，节约栈的空间开销
 */
typedef struct tagPicFileInfo{
    unsigned int  file_size;          /* 当前图片文件的大小           */
    unsigned int  name_time_stamp;    /* 当前文件名组成的时间戳 */
    unsigned int  trig_pic_type;      /* 当前图片的抓图触发类型 */
    unsigned int  struct_xor_crc;     /* 结构体按位异或校验结果 */
}__attribute__((packed)) stuPicFileInfo;

/* 检测单个图片的信息，固定为16个字节 */
typedef struct tagCheckPicFileInfo{
    int  file_size;          /* 当前图片文件的大小           */
    int  name_time_stamp;    /* 当前文件名组成的时间戳 */
    int  trig_pic_type;      /* 当前图片的抓图触发类型 */
    int  struct_xor_crc;     /* 结构体按位异或校验结果 */
}__attribute__((packed)) stuCheckPicFileInfo;

typedef struct tagAviFilesList
{
    unsigned int  is_available;     /* AVI文件是否有效       */
    unsigned char file_name[32];    /* AVI文件的文件名字     */
    unsigned int  start_time;       /* AVI文件开始录制的时间 */
    unsigned int  stop_time;        /* AVI文件结束录制的时间 */
    unsigned int  record_type;      /* AVI文件的文件类型     */
}AviFilesList, *pAviFilesList;

typedef struct tagPopHandle
{
    unsigned int start_time;       /* 片段开始录制的时间 */
    unsigned int stop_time;        /* 片段结束录制的时间 */
    void  *read_link_handle;       /* 片段链表句柄       */
    unsigned int record_type;      /* 片段录像类型       */
}PopHandle, *pPopHandle;

/* 搜索的AVI文件片段链表 */
typedef struct tagAviSegLinkList
{
    void         *prev_link;        /* 链表前指针            */
    unsigned char file_name[64];    /* AVI文件的文件名字     */
    unsigned int  start_time;       /* AVI文件开始录制的时间 */
    unsigned int  stop_time;        /* AVI文件结束录制的时间 */
    unsigned int  record_type;      /* AVI文件的文件类型     */
    void         *next_link;        /* 链表尾指针            */
}AviSegLinkList, *pAviSegLinkList;

#if 0
/* 搜索的I帧信息链表 */
typedef struct tagIFrameLinkList
{
    void         *prev_link;        /* 链表前指针                      */
    unsigned int  CurIFrameTime;    /* 当前时间                        */
    unsigned int  IFrameNum;        /* 当前I帧编号在总视频帧里面的编号 */
    void         *next_link;        /* 链表尾指针                      */
}IFrameLinkList, *pIFrameLinkList;
#endif

#ifdef __cplusplus
extern "C" {
#endif

extern AviSegLinkList  *SEG_LINK[4];
extern PopHandle       *pop_handle;

extern int Disk_InitV2(int nMaxFrameSize);

extern int Disk_Init(void);

extern int Disk_Uinit(void);

/**
 * @brief 暂停模块功能
 */
extern int SDOPS_Disk_Pause ();

/**
 * @brief 恢复模块功能
 */
extern int SDOPS_Disk_Resume ();

/*@breif SetCurrentAviNumPerPage 设置每页显示多少个AVI文件
 *@param [IN]  avi_num_per_page 每页显示的AVI文件个数
 *@return 成功返回0
 *@       失败返回-1
 *@note  每页显示的AVI文件个数不能超过当前有效的AVI个数g_read_curr_avi_num，
 *@      每页显示的AVI文件个数不能超过最大能显示的AVI个数MAX_AVI_NUM_ONE_PAGE
 */
extern int SetCurrentAviNumPerPage(int avi_num_per_page);

/*@breif GetCurrentAviTotalNum 获取当前AVI总个数
 *@return 成功返回当前总的AVI文件个数
 *@       失败返回-1
 */
extern int GetCurrentAviTotalNum(void);

/*@breif SetSwitchPageIsChoose 设置当前选中的是哪一个页面和当前录像路径类型
 *@param [IN]  choose_page 选中的页面(从1开始)
 *@param [IN]  path_type 当前录像路径类型 0: http路径  1:绝对路径
 *@return 成功返回0
 *@      失败返回-1
 */
extern int SetSwitchPageIsChoose(int choose_page, int path_type);

/*@breif UpdateAviListByTime 通过给定的时间戳更新AVI文件列表
 *@param [IN]  start_time 要搜索的起始时间
 *@param [IN]  stop_time 要搜索的结束时间(为0表示搜索全部文件)
 *@param [OUT] p_search_avi_count 搜索到符合条件的AVI文件总个数
 *@return 成功返回0
 *@      失败返回-1
 */
extern int UpdateAviListByTime(unsigned int start_time, 
                                     unsigned int stop_time, 
                                     int *p_search_avi_count);

/*@breif SetSwitchPageIsChoose 设置当前选中的是哪一个页面和当前录像路径类型
 *@param [IN]  choose_page 选中的页面(从1开始)
 *@param [IN]  path_type 当前录像路径类型 0: http路径  1:绝对路径
 *@return 成功返回0
 *@      失败返回-1
*/
extern int SetSwitchPageIsChoose(int choose_page, int path_type);

/*@breif GetAviListInfoByTimeStamp 通过给定的时间戳获取对应的AVI文件列表
 *@param [IN]  read_avi_page 需要获取第几页的AVI信息(从1开始)
 *@param [OUT] p_avi_list 获取到的AVI文件列表
 *@param [OUT] p_avi_num_curr_page 当前页面有多少个AVI文件
 *@return 成功返回0
 *@      失败返回-1
 *@note  需要先调用SetCurrentAviNumPerPage设置当前用户需要设置每页显示多少个AVI文件
 *@note  然后调用UpdateAviListByTime
 */
extern int GetAviListInfoByTimeStamp(int read_avi_page, void **p_avi_list, int *p_avi_num_curr_page);

/*@breif SetCurrentPicNumPerPage 设置每页显示多少个图片文件
 *@param [IN]  pic_num_per_page 每页显示的图片文件个数
 *@return 成功返回0
 *@       失败返回-1
 *@note  每页显示的图片文件个数不能超过当前有效的图片个数g_read_curr_pic_num，
 *@      每页显示的图片文件个数不能超过最大能显示的图片个数MAX_PIC_NUM_ONE_PAGE
 */
extern int SetCurrentPicNumPerPage(int pic_num_per_page);

/*@breif GetCurrentPicTotalNum 获取当前图片总个数
 *@return 成功返回当前总的图片文件个数
 *@       失败返回-1
 */
extern int GetCurrentPicTotalNum(void);

/*@breif UpdatePicListByTime 通过给定的时间戳更新PIC文件列表
 *@param [IN]  start_time 要搜索的起始时间
 *@param [IN]  stop_time 要搜索的结束时间(为0表示搜索全部文件)
 *@param [OUT] p_search_pic_count 搜索到符合条件的PIC文件总个数
 *@return 成功返回0
 *@      失败返回-1
 */
extern int UpdatePicListByTime(unsigned int start_time, 
                                     unsigned int stop_time, 
                                     int *p_search_pic_count);

/*@breif GetPictureListInfoByTimeStamp 通过给定的时间戳获取对应的图片文件列表
 *@param [IN]  read_pic_page 需要获取第几页的图片信息(从1开始)
 *@param [OUT] p_pic_list 获取到的图片文件列表
 *@param [OUT] p_pic_num_curr_page 当前页面有多少个图片文件 
 *@return 成功返回0
 *@      失败返回-1
 *@note  需要先调用GetPicGlobalHeadInfoFromGlobalFile将需要的数据放到全局结构体里面
 *@      需要先调用SetCurrentPicNumPerPage设置当前用户需要设置每页显示多少个图片文件
 */
extern int GetPictureListInfoByTimeStamp(int read_pic_page, void **p_pic_list, int *p_pic_num_curr_page);

/* 将全局索引结构体里面的数据拷贝到当前需要操作的结构体里面，并排序，以便后面用到 */
extern void UpdateGlobalAviFileSet(void);

/* 初始化双向链表 */
extern AviSegLinkList  *InitNewAviSegLinklist(int b_get_link_list);

/* 添加一个双向链表节点 */
extern AviSegLinkList *AddNewAviSegLinkList(AviSegLinkList *plink);

/* 释放整个链表 */
extern void FreeAviSegLinkLists(int b_get_link_list, AviSegLinkList *link_head);

/* 初始化弹出句柄 */
extern void InitPopHandle(void);

/* 创建一个弹出句柄(只允许创建两个弹出句柄) */
extern void  *CreateOnePopHandle(int *curr_handle);

/* 销毁一个弹出句柄 */
extern int DestroyOnePopHandle(int handle);

/* 释放一个弹出句柄 */
extern void FreeOnePopHandle(int handle);

extern void printsegtime(int FrameTime);

int CreateDirent(const char *sPathName);

/* 获取磁盘资源占用情况(单位: MB) */
extern int GetDiskResourceSize(int *pRecordMB, int *pPictureMB);

/* 格式化SD卡(返回值: -1:失败; 0:成功 1:没有SD卡) */
extern int FormatSdCard(void);

/* 获取SD卡状态 */
extern int GetSdCardFormatStatus(void);

/* 获取写SD卡的状态 */
extern int GetSdCardWriteStatus(void);

/* 获取SD卡相关信息 */
extern int GetSdCardInfo(LPSdCardStatus pSdCardStatus);

/*@breif GetFileReadWriteStatus 获取当前文件的读写状态
 *@param [IN]  file_name 需要操作的文件名(绝对路径)
 *@param [OUT] read_write_status 
 *@            0:  当前文件空闲
 *@            1:  当前文件正在读
 *@            2:  当前文件正在写
 *@param [OUT] file_size 文件大小 (单位: 字节)
 *@return 返回0表示成功
 *@      返回-1表示失败
 */
extern int GetFileReadWriteStatus(const char *file_name, 
                                         unsigned int *read_write_status,
                                         unsigned int *file_size);


/* ============================================================================
 * MP4 循环录像配置接口 (readme 新需求)
 * ----------------------------------------------------------------------------
 * SD_OPS 内部已有静态全局 g_mp4_segment_duration_sec 控制切片时长，
 * 这里通过对外函数暴露给上层应用动态调整。
 * ==========================================================================*/

/* 设置 MP4 录像容器切片时长阈值（秒），<=0 时回退到默认 300 */
void         SDOPS_SetMp4SegmentDuration(unsigned int sec);
/* 获取当前 MP4 录像容器切片时长阈值（秒） */
unsigned int SDOPS_GetMp4SegmentDuration(void);

/* 切换录像容器：1=MP4(新，默认)，0=AVI(旧，用于回退) */
void         SDOPS_SetRecordContainerToMp4(int use_mp4);

/*@breif SDOPS_GetRecordDataSize 获取指定时间段的录像数据大小
 *@param [IN] start_time 开始时间
 *@param [IN] stop_time  结束时间
 *@param [IN] record_type 筛选的录像类型(当前无效)
 *@param [OUT] p_size 获取到的帧数据大小
 *@return 返回  0 表示获取成功
 *@       返回 -1 表示获取失败
 *@note   获取到的是数据帧大小，包括帧头
 */
extern int SDOPS_GetRecordDataSize (unsigned int start_time, 
                                                 unsigned int stop_time, 
                                                 int record_type,
                                                 unsigned int *p_size);

/*@breif SDOPS_GetRecordInfo 获取当前录像时间的录像信息
 *@param [OUT] p_curr_rec_start_time 获取到的录像文件起始时间
 *@return 返回  0 表示获取成功
 *@       返回 -1 表示获取失败
 */
extern int SDOPS_GetRecordInfo(int *p_curr_rec_start_time);

/********************************************************************************************************/

/*@breif SDOPS_GetRecordFileInfoByFileName 获取文件名对应的文件信息
 *@param[in]  file_name 文件名
 *@param[out]  file_info 文件信息
 *@return 成功返回0 失败返回-1
 */
extern int SDOPS_GetRecordFileInfoByFileName(const char* file_name, void* file_info);

#ifdef __cplusplus
}
#endif

#endif

