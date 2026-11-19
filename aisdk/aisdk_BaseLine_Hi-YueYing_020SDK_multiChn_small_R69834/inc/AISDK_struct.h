/*

*/
#ifndef __AISDK_STRUCT_H__
#define __AISDK_STRUCT_H__


//特征值最大长度定义
#define  MAX_FEATURE_LEN (320)
//同帧最大人脸数量
#define  MAX_FACE_CNT_ONCE   (8)   
//同帧最大混合对象数量
#define  MAX_OBJ_CNT_ONCE     (64)
#define  MAX_OBJ_CNT_ONCE_LITE (8)







#define AISDK_API 

typedef unsigned char           uint8_t;
typedef unsigned short          uint16_t;
typedef unsigned int            uint32_t;
typedef signed char             int8_t;
typedef short                   int16_t;
typedef int                     int32_t;
typedef unsigned long long      uint64_t;
typedef long long               int64_t;




//AI场景枚举 
typedef enum
{
    AI_SCENE_INVALID   = 0x0,
    AI_SCENE_FACECAP   = 0x1,      //人脸抓拍
    AI_SCENE_FACEEVENT = 0x2,      //人脸上报
    AI_SCENE_FACERECO  = 0x4,      //人脸比对
    AI_SCENE_PLATE     = 0x8,      //车牌识别[此项目未用]
    AI_SCENE_MD        = 0x10,     //移动侦测
    AI_SCENE_HD        = 0x20,     //人形区域算法检测
/*    
    AI_SCENE_ELECTRONIC_FENCE,//电子围栏
    AI_SCENE_CROSS_BOARD_DETECT,//越界检测
    AI_SCENE_OFF_DUTY_DETECT,//离岗检测
    AI_SCENE_PASSENGER_FLOW,//客流统计
*/
    AI_SCENE_VEHICLE    = 0x40,
    AI_SCENE_REGION     = 0x80,     //区域功能，附庸与其他场景
    AI_SCENE_TUMBLE    = 0x100,     //跌倒检测[此项目未用]
    AI_SCENE_PET       = 0x200,     //宠物检测[此项目未用]
    AI_SCENE_FACERECO_VEHICLE   = 0x400,      //人脸比对+车辆检测[此项目未用]
    AI_SCENE_FLAME     = 0x800,    //火焰检测[此项目未用]
    AI_SCENE_PARKINGSPACE   = 0x1000,    //车位
    AI_SCENE_FLAME_SMOG         = 0x2000,    //烟火检测
    AI_SCENE_GESTURE            = 0x4000,    //手势检测[此项目未用]
}E_AI_SCENE;

//配置数据类型枚举
typedef enum
{
    AICFG_TYPE_STRUCT = 0,
    AICFG_TYPE_JSON,
    AICFG_TYPE_XML,
}E_AISDK_CFG_TYPE;

//分析帧输入数据结构体
typedef struct YUV_FRAME_INFO_T
{
    int  nStatus;           // 0:空闲，1:正用于缓存数据，2：人脸算法检测中，3：正在抠图
    uint32_t nFrameId;      // 帧ID
    uint32_t uYuvFormat;    // YUV格式 0:NV12/YUV420SP  1:NV21/YVU420SP  2:yuv400 
    uint32_t nWidth;        // YUV宽度
    uint32_t nHeight;       // YUV长度
    uint32_t u32RealTimeSec;  // 帧时间
    uint32_t u32RealTimeUSec;
    uint64_t uFrameBufVir; // 帧缓冲区-虚拟地址
    uint64_t uFrameBufPhy; // 帧缓冲区-物理地址
    uint32_t uDataSourceType;   // 0 meida PPL 获取直接输入   1 Rmem内存区输入 
                                // 对应的释放方式：
                                // 0： 外部注册的释放方法，根据送检结果处理调用已注册的方式释放
                                // 1： 目前内部不释放，任何情况都由外部释放。
    uint32_t uCompressMode; //OT_COMPRESS_MODE_NONE = 0,      /* no compress */
                            //OT_COMPRESS_MODE_SEG,           /* compress unit is 256x1 bytes as a segment. */
                            //OT_COMPRESS_MODE_SEG_COMPACT,   /* compact compress unit is 256x1 bytes as a segment. */
                            //OT_COMPRESS_MODE_TILE,          /* compress unit is a tile. */
                            //OT_COMPRESS_MODE_LINE,          /* compress unit is the whole line. */
                            //OT_COMPRESS_MODE_FRAME,         /* compress unit is the whole frame. YUV for 3DNR */
                            //OT_COMPRESS_MODE_FMAP,          /* compress unit is the whole line, based on mapping transformation. */
}YUV_FRAME_INFO_S;

//
typedef struct DEF_RECT_T
{
    uint32_t    left;
    uint32_t    top;
    uint32_t    right;
    uint32_t    bottom;
}S_RECT;

//目标类型定义
typedef enum 
{
    AI_TARGET_PERSON  = 0x1,     //人形
    AI_TARGET_CAR     = 0x2,     //车辆
    AI_TARGET_BUS     = 0x4,
    AI_TARGET_TRUCK   = 0x8,
    AI_TARGET_PET     = 0x10,    //宠物：猫狗
    AI_TARGET_TUMBLE  = 0x20,    //事件 跌倒
    AI_TARGET_CYCLE   = 0x40,    //非机动车
    AI_TARGET_FLAME	  = 0x80,    //火焰
    AI_TARGET_SMOG	  = 0x100,   //烟雾
    AI_TARGET_FACE	  = 0x200,   //人脸
    AI_TARGET_PLATE	  = 0x400,   //车牌
}E_AI_TARGET_TYPE;

//算法类型定义
typedef enum 
{
    AI_ALGO_INVALID_TYPE         = 0x0,
    AI_ALGO_MEG_FACEHUMAN        = 0x1,     //脸人
    AI_ALGO_MEG_HVCFP            = 0x2,     //机非人
    AI_ALGO_JZ_PERSONVEHICLE     = 0x3,     //人形车形
    AI_ALGO_MEG_PET              = 0x4,     //宠物：猫狗
    AI_ALGO_MEG_TUMBLE           = 0x5,     //跌倒
    AI_ALGO_MEG_FLAME            = 0x6,     //火焰
    AI_ALGO_MEG_FLAMESMOG        = 0x7,     //烟火
    AI_ALGO_MEG_HVCFPVIETNAM     = 0x8,     //机非人-越南
}E_AI_ALGO_TYPE;

//单个授权文件信息结构体
typedef struct 
{
    E_AI_ALGO_TYPE eType;
    char strFileName[128];
    unsigned long long uStartTime;
    unsigned long long uDueTime;
    unsigned int uValidCh;
}ST_AISDK_DEF_AUTH_FILE_INFO;

//授权文件信息结构体
typedef struct 
{
    unsigned int uFileNum;
    ST_AISDK_DEF_AUTH_FILE_INFO* pFileInfoList;
}ST_AISDK_DEF_AUTH_INFO;








/*************************************
 * 配置数据（AICFG_TYPE_STRUCT方式）定义 *************************
**************************************/

typedef struct ST_AISDK_DEF_INIT_PARAM
{
    unsigned int ai_stream_w;  //AI流尺寸-宽
    unsigned int ai_stream_h;  //AI流尺寸-高
    unsigned int nmem_size_jz_pv; //目前仅使用jz算法有效 - jz人车算法使用nmem大小  默认 3.125*1024*1024
    unsigned int nmem_size_jz_hg; //目前仅使用jz算法有效 - jz手势算法使用nmem大小  默认 4.375*1024*1024
    
    char model_path[192];   //模型目录
    char auth_path[128];    //授权文件目录   目前仅ks算法有效   默认 /mnt/mtd/AlgoAuth
    char auth_backup_path[128]; //授权文件备份目录   目前仅ks算法有效   默认 /mnt/mtd/mall_algo/AlgoAuth
    char device_name[64];  //设备型号名   目前仅ks算法商城有效

    unsigned int nmem_size_total;   //指定最大的可用的nmem大小。 注意：必须指定！

    char reserve[56];
}AISDK_DEF_INIT_PARAM;















/////////////  Image  ////////////////////////
#define ______IMAGE______
/*Image type*/
typedef enum IMAGE_TYPE_ENUM
{
    IMAGE_TYPE_YUV420SP       =  0x0,       /*YUV420 SemiPlanar*/  //NV12
    IMAGE_TYPE_YUV422SP       =  0x1,       /*YUV422 SemiPlanar*/
    IMAGE_TYPE_YUV420P        =  0x2,       /*YUV420 Planar */
    IMAGE_TYPE_YUV422P        =  0x3,       /*YUV422 planar */

    IMAGE_TYPE_YVU420SP       =  0x4,       /*YVU420 SemiPlanar*/ //NV21
    IMAGE_TYPE_YVU422SP       =  0x5,       /*YVU422 SemiPlanar*/
    IMAGE_TYPE_YVU420P        =  0x6,       /*YVU420 Planar */
    IMAGE_TYPE_YVU422P        =  0x7,       /*YVU422 planar */

    IMAGE_TYPE_YUYV422I = 0x8, /* INTERLEAVED_YUYV : YUYVYUYV...              */
    IMAGE_TYPE_UYVY422I = 0x9, /* INTERLEAVED_UYVY : UYVYUYVY...              */

    IMAGE_TYPE_GRAY8          = 0xA,

    IMAGE_TYPE_RGBA_8888       = 0x10,
    IMAGE_TYPE_RGB_888_PLANAR  = 0x11,
    IMAGE_TYPE_RGB_888_PACKAGE = 0x12,
    IMAGE_TYPE_RGB_565         = 0x13,

    IMAGE_TYPE_BGRA_8888       = 0x20,
    IMAGE_TYPE_BGR_888_PLANAR  = 0x21,
    IMAGE_TYPE_BGR_888_PACKAGE = 0x22,

    IMAGE_TYPE_BUTT

}ALGO_IMAGE_TYPE_E;

typedef enum IMAGE_MEM_TYPE_ENUM
{
    IMAGE_MEM_TYPE_NMEM   = 0,
    IMAGE_MEM_TYPE_RMEM,
    IMAGE_MEM_TYPE_OSMEM, /*暂不支持 同nmem*/
    IMAGE_MEM_TYPE_POOL,
    IMAGE_MEM_TYPE_MMZ,
}ALGO_IMAGE_MEM_TYPE_E;


typedef enum 
{
    Aisdk_FuncCb_Type_Invalid = 0,              //无效的
    Aisdk_FuncCb_Type_OsdFaceCapFace,           //外部叠加OSD回调。 对应func： AISDK_FuncCb_Osd   - 人脸抓拍-人脸   【暂未实现】
    Aisdk_FuncCb_Type_OsdFaceCapBg,             //外部叠加OSD回调。 对应func： AISDK_FuncCb_Osd   - 人脸抓拍-背景   【暂未实现】
    Aisdk_FuncCb_Type_OsdFaceRecoFace,          //外部叠加OSD回调。 对应func： AISDK_FuncCb_Osd   - 人脸识别-人脸
    Aisdk_FuncCb_Type_OsdFaceRecoBg,            //外部叠加OSD回调。 对应func： AISDK_FuncCb_Osd   - 人脸识别-背景
    Aisdk_FuncCb_Type_OsdMaskFace,              //外部叠加OSD回调。 对应func： AISDK_FuncCb_Osd   - 口罩-人脸
    Aisdk_FuncCb_Type_OsdMaskBg,                //外部叠加OSD回调。 对应func： AISDK_FuncCb_Osd   - 口罩-背景
    Aisdk_FuncCb_Type_OsdElectronicFenceBg,     //外部叠加OSD回调。 对应func： AISDK_FuncCb_Osd   - 围栏-背景
    Aisdk_FuncCb_Type_OsdCrossBoardBg,          //外部叠加OSD回调。 对应func： AISDK_FuncCb_Osd   - 越界-背景
    Aisdk_FuncCb_Type_OsdOffDutyBg,             //外部叠加OSD回调。 对应func： AISDK_FuncCb_Osd   - 离岗-背景
    Aisdk_FuncCb_Type_OsdPassengerFlowBg,       //外部叠加OSD回调。 对应func： AISDK_FuncCb_Osd   - 客流-背景
    Aisdk_FuncCb_Type_OsdVehicleVehicle,        //外部叠加OSD回调。 对应func： AISDK_FuncCb_Osd   - 车辆-车形
    Aisdk_FuncCb_Type_OsdVehiclePlate,          //外部叠加OSD回调。 对应func： AISDK_FuncCb_Osd   - 车辆-车牌
    Aisdk_FuncCb_Type_OsdVehicleBg,             //外部叠加OSD回调。 对应func： AISDK_FuncCb_Osd   - 车辆-背景
    Aisdk_FuncCb_Type_Max,
}AISDK_DEF_E_FUNC_CB_TYPE;

typedef struct 
{
    int ch;                         //通道号 [暂无效]
    ALGO_IMAGE_TYPE_E eImageType;   //图像格式
    unsigned int uImageSizeW;   //图像宽
    unsigned int uImageSizeH;   //图像高
    unsigned long long uImageBufVir;         //图像buf-虚拟地址
    unsigned long long uImageBufPhy;         //图像buf-物理地址
}AISDK_DEF_ST_FUNC_CB_DATA_OSD;

typedef int (*AISDK_FuncCb_Osd)(AISDK_DEF_E_FUNC_CB_TYPE eFuncType, AISDK_DEF_ST_FUNC_CB_DATA_OSD* pData, void* userData);

typedef union 
{
    AISDK_FuncCb_Osd funcCb_Osd;
}AISDK_DEF_FUNC_CB;









/** 事件帧相关的定义 **/
typedef struct 
{
    uint8_t nValid;      // 是否有效
    uint64_t faceId;        // 人脸id
    // float    confidence;    // 评分 
    // uint32_t feature_data_size;                 // 特征值数据长度   0则没有
    // uint8_t  feature_data[MAX_FEATURE_LEN];     // 特征值数据
    // int      face_pic_len;       // 人脸图jpg长度   0则没有
    // char*    face_pic_data;      // 人脸图jpg数据 [内部alloc]
    // int      face_pic_w;         // 人脸图 宽
    // int      face_pic_h;         // 人脸图 高
    uint32_t uRecoNum;           //匹配数量， 0：没有  1：有  >1：有多个（无效值 目前不支持）
    float    fRecoScores;        //匹配度   (0-100) 
    // uint64_t uRecoGroupId;       //匹配人脸组id
    char     uuidGroup[64];/*人脸组唯一标识符，全局唯一，使用uuid（16Byte）生成，转换成字符串存储*/
    // char	 groupName[32];		//脸库名称
    char     uuidSubgroup[64];/*人脸子组唯一标识符，全局唯一，使用uuid（16Byte）生成，转换成字符串存储*/
    // char 	 subGroupName[64]; // 组织名称
    // uint64_t uRecoFaceId;        //匹配人脸id
    // char     uuidFace[64];/*人脸信息唯一标识符，全局唯一，使用uuid（16Byte）生成，转换成字符串存储*/
    // char     people_name[32];/*人脸信息名称，32个字节的字符串*/
}AISDK_DEF_S_LOCAL_FACEINFO;


/*************************************
 * 事件帧定义 *************************
**************************************/

//事件类型定义
typedef enum
{
    AI_EVENT_FACECAP,              // 人脸抓拍      对应事件帧： AISDK_DEF_S_EVENT_FACECAP
    AI_EVENT_FACEDETECT,           // 人脸检测      对应事件帧： AISDK_DEF_S_EVENT_FACEDETECT
    AI_EVENT_FACERECO,             // 人脸比对      对应事件帧： AISDK_DEF_S_EVENT_FACERECO
    AI_EVENT_MOTIONDETECT,         // 移动侦测      对应事件帧： AISDK_DEF_S_EVENT_MOTION
    AI_EVENT_MASKDETECT,           // 口罩检测      对应事件帧： AISDK_DEF_S_EVENT_MASK_DETECT
    AI_EVENT_HUMANDETECT,          // 人形侦测      对应事件帧： AISDK_DEF_S_EVENT_HUMAN
    AI_EVENT_VEHICLEDETECT,          //【暂无】车形侦测      对应事件帧： AISDK_DEF_S_EVENT_VEHICLE
    AI_EVENT_ELECTRONIC_FENCE,     // 电子围栏      对应事件帧： AISDK_DEF_S_EVENT_ELECTRONIC_FENCE
    AI_EVENT_CROSS_BOARD_DETECT,   // 越界检测      对应事件帧： AISDK_DEF_S_EVENT_CROSS_BOARD_DETECT
    AI_EVENT_OFF_DUTY_DETECT,      // 离岗检测      对应事件帧： AISDK_DEF_S_EVENT_OFF_DUTY_DETECT
    AI_EVENT_PASSENGER_FLOW,       // 客流统计      对应事件帧： AISDK_DEF_S_EVENT_PASSENGER_FLOW
    AI_EVENT_CYCLE_PASSENGER_FLOW, // 客流周期统计   对应事件帧： AISDK_DEF_S_EVENT_CYCLE_PASSENGER_FLOW
    AI_EVENT_TARGET_DIE,           // 目标消失      对应事件帧： AISDK_DEF_S_EVENT_TARGET_DIE
    AI_EVENT_OBJDETECT,            // 混合对象检测  对应事件帧： AISDK_DEF_S_EVENT_OBJDETECT
    AI_EVENT_PETDETECT,            // 【暂无】宠物检测
    AI_EVENT_TUMBLEDETECT,         // 【暂无】跌倒检测
    AI_EVENT_VEHICLERECO,          // 车辆识别      对应事件帧： AISDK_DEF_S_EVENT_VEHICLERECO
    AI_EVENT_VEHICLECAP,           // 车辆抓拍      对应事件帧： AISDK_DEF_S_EVENT_VEHICLECAP
    AI_EVENT_FLAMEDETECT,          // 【暂无】火焰检测      对应事件帧： AISDK_DEF_S_EVENT_FLAME_DETECT
    AI_EVENT_PARKING_SPACE_STATUS, // 车位状态      对应事件帧： AISDK_DEF_S_EVENT_PARKING_SPACE_STATUS
    AI_EVENT_OBJDETECT_LITE,         //混合对象检测-轻量     对应事件帧： AISDK_DEF_S_EVENT_OBJDETECT_LITE
    AI_EVENT_GESTUREDETECT,          //【暂无】手势检测             对应事件帧： AISDK_DEF_S_EVENT_GESTUREDETECT
    AI_EVENT_PASSENGER_FLOW_LINE,       // 客流统计-拌线      对应事件帧： AISDK_DEF_S_EVENT_PASSENGER_FLOW_LINE
    AI_EVENT_CYCLE_PASSENGER_FLOW_LINE, // 客流周期统计-拌线     对应事件帧： AISDK_DEF_S_EVENT_CYCLE_PASSENGER_FLOW
}E_AI_EVENT_TYPE;

//人脸检测事件帧 
typedef struct ST_AISDK_DEF_EVENT_FACEDETECT
{
    uint32_t nFrameId;      // 帧ID
    uint32_t nSrcW;         // 原始帧宽
    uint32_t nSrcH;         // 原始帧高
    uint32_t u32TimeSec;    // 发生时间s
    uint32_t u32TimeUSec;   // 发生时间us
    uint32_t uFaceNum;      // 人脸数量
    S_RECT   stRect[MAX_FACE_CNT_ONCE];        // 人脸区域
    uint64_t faceId[MAX_FACE_CNT_ONCE];        // 人脸id
    AISDK_DEF_S_LOCAL_FACEINFO faceInfo[MAX_FACE_CNT_ONCE]; // 【目前无效】人脸信息   #定义__FACEDETECT_HAVE_FACEINFO__时数据有效 
    // float    confidence;    // 评分 
    // uint32_t nRuleID;            // 规则ID
    // char acOrganizationID[64];   // 组织id
    // char acOrganizationName[64]; // 组织名称
}AISDK_DEF_S_EVENT_FACEDETECT;

//人脸抓拍事件帧 
typedef struct ST_AISDK_DEF_EVENT_FACECAP
{
    uint32_t nFrameId;      // 帧ID
    uint32_t nSrcW;         // 原始帧宽
    uint32_t nSrcH;         // 原始帧高
    uint32_t u32TimeSec;    // 发生时间s
    uint32_t u32TimeUSec;   // 发生时间us
    uint64_t faceId;        // 人脸id
    S_RECT   stRect;        // 人脸区域
    float    confidence;    // 评分 
    uint32_t feature_data_size;                 // 特征值数据长度   0则没有
    char     feature_data[MAX_FEATURE_LEN];     // 特征值数据
    int      face_pic_len;       // 人脸图jpg长度   0则没有
    char*    face_pic_data;      // 人脸图jpg数据 [内部alloc]
    int      bg_pic_len;         // 背景图jpg长度   0则没有
    char*    bg_pic_data;        // 背景图jpg数据 [内部alloc]
    uint32_t nRuleID;            // 规则ID
    char acOrganizationID[64];   // 组织id
    char acOrganizationName[64]; // 组织名称
}AISDK_DEF_S_EVENT_FACECAP;

//人脸比对事件帧 
typedef struct ST_AISDK_DEF_EVENT_FACERECO
{
    uint32_t nFrameId;      // 帧ID
    uint32_t nSrcW;         // 原始帧宽
    uint32_t nSrcH;         // 原始帧高
    uint32_t u32TimeSec;    // 发生时间s
    uint32_t u32TimeUSec;   // 发生时间us
    uint64_t faceId;        // 人脸id
    S_RECT   stRect;        // 人脸区域
    float    confidence;    // 评分 
    uint32_t feature_data_size;                 // 特征值数据长度   0则没有
    uint8_t  feature_data[MAX_FEATURE_LEN];     // 特征值数据
    int      face_pic_len;       // 人脸图jpg长度   0则没有
    char*    face_pic_data;      // 人脸图jpg数据 [内部alloc]
    int      bg_pic_len;         // 背景图jpg长度   0则没有
    char*    bg_pic_data;        // 背景图jpg数据 [内部alloc]
    // uint32_t nRuleID;            // 规则ID
    //比对结果及人脸数据库信息
    uint32_t uRecoNum;           //匹配数量， 0：没有  1：有  >1：有多个（无效值 目前不支持）
    uint64_t uRecoGroupId;       //匹配人脸组id
    uint64_t uRecoFaceId;        //匹配人脸id
    float    fRecoScores;        //匹配度   (0-100) 
    char     uuidGroup[64];/*人脸组唯一标识符，全局唯一，使用uuid（16Byte）生成，转换成字符串存储*/
    char     uuidSubgroup[64];/*人脸子组唯一标识符，全局唯一，使用uuid（16Byte）生成，转换成字符串存储*/
    char     uuidFace[64];/*人脸信息唯一标识符，全局唯一，使用uuid（16Byte）生成，转换成字符串存储*/
    char     people_name[32];/*人脸信息名称，32个字节的字符串*/
    // int gender;/*性别，0-男，1-女，2-未知*/
    // char phone_number[20];/*手机号*/
    // int certificates_type;/*证件类型，0-身份证，1-护照，2-其他*/
    // char certificates_id[20];/*证件号*/
    
    int      face_pic_w;         // 人脸图 宽
    int      face_pic_h;         // 人脸图 高
    char	 groupName[32];		//脸库名称
    char 	 subGroupName[64]; // 组织名称
    int      bg_pic_w;           // 背景图 宽
    int      bg_pic_h;           // 背景图 高

    float faceinfo_blur;
    float faceinfo_yaw;
    float faceinfo_roll;
}AISDK_DEF_S_EVENT_FACERECO;

//移动侦测事件帧
typedef struct ST_AISDK_DEF_EVENT_MOTION
{
    uint32_t nFrameId;      // 帧ID
    uint32_t u32TimeSec;    // 发生时间s
    uint32_t u32TimeUSec;   // 发生时间us
    uint32_t u32EventID;    // 事件ID    目前无效
    uint32_t u32EventStatus;// 事件状态  0 首次报警  1 报警持续中  2 报警结束    目前无效
    uint32_t uSensitivity;  // 触发灵敏度 
    uint8_t  uModeType;     // 哪种方式  0：宏块方式   1：区域方式 
    union 
    {
        /*宏块方式*/
        uint16_t  uActiviMacroBlock[12]; //活跃的宏块数据 12*16 （1bit表示一个宏块）
        /*区域方式*/
        //..
    };
    uint32_t u32BakPicL;	// 背景图jpg长度    目前无效
    uint32_t u32BakPicW;    // 背景图jpg宽    目前无效
    uint32_t u32BakPicH;    // 背景图jpg高    目前无效
    uint8_t  *u8BakPicData;	// 背景图jpg数据 [内部malloc]    目前无效
}AISDK_DEF_S_EVENT_MOTION;


//口罩报警事件帧
typedef struct ST_AISDK_DEF_EVENT_MASK_DETECT
{
	uint32_t nFrameId; 			// 帧ID
	uint32_t nSrcW; 			// 原始帧宽
	uint32_t nSrcH; 			// 原始帧高
	uint32_t u32TimeSec; 		// 发生时间s
	uint32_t u32TimeUSec; 		// 发生时间us
	uint64_t faceId; 			// 人脸id
	S_RECT stRect; 				// 人脸区域
	uint32_t face_pic_len; 		// 人脸图jpg长度 0则没有
	char* face_pic_data; 		// 人脸图jpg数据 [内部alloc]
    uint32_t face_pic_w;        // 人脸图 宽
    uint32_t face_pic_h;        // 人脸图 高
	uint32_t bg_pic_len; 		// 背景图jpg长度 0则没有
	char* bg_pic_data; 			// 背景图jpg数据 [内部alloc]
    uint32_t bg_pic_w;          // 背景图 宽
    uint32_t bg_pic_h;          // 背景图 高
	uint32_t maskStatus;		//口罩状态:0-无口罩；1-有口罩
}AISDK_DEF_S_EVENT_MASK_DETECT;

//人形侦测事件帧
typedef struct ST_AISDK_DEF_EVENT_HUMAN
{
    uint32_t nFrameId;                     // 帧ID
	uint32_t nSrcW;         // 原始帧宽    目前无效
	uint32_t nSrcH; 		// 原始帧高    目前无效
    uint32_t u32TimeSec;                   // 发生时间s
    uint32_t u32TimeUSec;                  // 发生时间us
    uint32_t u32Peoplenum;                 // 人形数量
    uint64_t u64TargetId[MAX_OBJ_CNT_ONCE]; // 人形id
    S_RECT stRect[MAX_OBJ_CNT_ONCE];     // 人形区域
    uint8_t  uStatusActionless[MAX_OBJ_CNT_ONCE]; // 人形静止状态  0 运动中 1 运动目标归于静止 2 未决中 
} AISDK_DEF_S_EVENT_HUMAN;

//电子围栏事件帧
typedef struct ST_AISDK_DEF_EVENT_ELECTRONIC_FENCE
{
    uint32_t nFrameId;      // 帧ID
    uint32_t nSrcW;         // 原始帧宽
    uint32_t nSrcH;         // 原始帧高
    uint32_t u32TimeSec;    // 发生时间s
    uint32_t u32TimeUSec;   // 发生时间us
    uint32_t u32EventID;    // 事件ID    目前无效
    uint32_t u32EventStatus;// 事件状态  0 首次报警  1 报警持续中  2 报警结束
    uint32_t u32HaveTagetType; // 包含哪些类型目标  多种目标可与  参考 E_AI_TARGET_TYPE     ##事件状态2时此数据无效
    uint32_t u32InTime;     // 进入时长 0 首次报警  x 持续时长  单位：ms
    uint32_t u32Peoplenum;	// 围栏内人形数量
    S_RECT   stRect[MAX_OBJ_CNT_ONCE];    // 围栏内人形坐标
    uint64_t u64ObjId[MAX_OBJ_CNT_ONCE];     // 对象 track id 
    uint8_t  uObjCategory[MAX_OBJ_CNT_ONCE]; // 对象类型  0 unknow   1 face   2 body   3 vehicle   4 cycle   5 plate   6 pet   7 flame 
    uint8_t  uStatusActionless[MAX_OBJ_CNT_ONCE]; // 人形静止状态  0 运动中 1 运动目标归于静止 
    uint8_t  uStatusTrigger[MAX_OBJ_CNT_ONCE]; // 触发状态  0 未知/无效  1 方向-进入  2 方向-离开  3 位置
    uint32_t bg_pic_len; 		// 背景图jpg长度 0则没有     ##事件状态1、2时此数据无效
	char* bg_pic_data; 			// 背景图jpg数据 [内部alloc]     ##事件状态1、2时此数据无效
    uint32_t bg_pic_w;          // 背景图 宽     ##事件状态1、2时此数据无效
    uint32_t bg_pic_h;          // 背景图 高     ##事件状态1、2时此数据无效
}AISDK_DEF_S_EVENT_ELECTRONIC_FENCE;

//越界检测事件帧
typedef struct ST_AISDK_DEF_EVENT_CROSS_BOARD_DETECT
{
    uint32_t nFrameId;      // 帧ID
    uint32_t nSrcW;         // 原始帧宽
    uint32_t nSrcH;         // 原始帧高
    uint32_t u32TimeSec;    // 发生时间s
    uint32_t u32TimeUSec;   // 发生时间us
    uint32_t u32EventID;    // 事件ID    目前无效
    uint32_t u32EventStatus;// 事件状态  0 首次报警  1 报警持续中  2 报警结束
    uint32_t u32HaveTagetType; // 包含哪些类型目标   多种目标可与  参考 E_AI_TARGET_TYPE     ##事件状态2时此数据无效
    uint32_t u32ObjNum;	    // 目标对象数量
    uint64_t u64ObjId[MAX_OBJ_CNT_ONCE];      // 对象track id 
    uint8_t  uObjCategory[MAX_OBJ_CNT_ONCE];  // 对象类型  0 unknow   1 face   2 body   3 vehicle   4 cycle   5 plate   6 pet   7 flame 
    S_RECT   stObjRect[MAX_OBJ_CNT_ONCE];     // 对象坐标
    uint32_t bg_pic_len; 		// 背景图jpg长度 0则没有
	char* bg_pic_data; 			// 背景图jpg数据 [内部alloc]
    uint32_t bg_pic_w;          // 背景图 宽
    uint32_t bg_pic_h;          // 背景图 高
}AISDK_DEF_S_EVENT_CROSS_BOARD_DETECT;

//离岗检测事件帧
typedef struct ST_AISDK_DEF_EVENT_OFF_DUTY_DETECT
{
    uint32_t nFrameId;      // 帧ID
    uint32_t nSrcW;         // 原始帧宽
    uint32_t nSrcH;         // 原始帧高
    uint32_t u32TimeSec;    // 发生时间s
    uint32_t u32TimeUSec;   // 发生时间us
    uint32_t u32EventID;    // 事件ID    目前无效
    uint32_t u32EventStatus;// 事件状态  0 首次报警  1 报警持续中  2 报警结束
    uint32_t u32HaveTagetType; // 包含哪些类型目标  多种目标可与  参考 E_AI_TARGET_TYPE     ##事件状态2时此数据无效
    uint32_t u32Peoplenum;	// 区域内人形数量
    S_RECT   stRect[MAX_OBJ_CNT_ONCE];    // 区域内人形坐标
    uint64_t u64ObjId[MAX_OBJ_CNT_ONCE];     // 对象 track id 
    uint8_t  uStatusActionless[MAX_OBJ_CNT_ONCE]; // 人形静止状态  0 运动中 1 运动目标归于静止 
	uint32_t bg_pic_len; 		// 背景图jpg长度 0则没有
	char* bg_pic_data; 			// 背景图jpg数据 [内部alloc]
    uint32_t bg_pic_w;          // 背景图 宽
    uint32_t bg_pic_h;          // 背景图 高
}AISDK_DEF_S_EVENT_OFF_DUTY_DETECT;

//客流统计出入事件帧
typedef struct ST_AISDK_DEF_EVENT_PASSENGER_FLOW
{
    uint32_t nFrameId;      // 帧ID
    uint32_t nSrcW;         // 原始帧宽
    uint32_t nSrcH;         // 原始帧高
    uint32_t u32TimeSec;    // 发生时间s
    uint32_t u32TimeUSec;   // 发生时间us
    uint32_t u32EventID;    // 事件ID    目前无效
    uint32_t u32EventStatus;// 事件状态  0 首次报警  1 报警持续中  2 报警结束
    uint32_t u32HaveTagetType; // 包含哪些类型目标  [目前无效]  多种目标可与  参考 E_AI_TARGET_TYPE     ##事件状态2时此数据无效
    uint32_t u32InoutStatus;	//出入状态:1-进入; 2-离开
    //uint64_t u64InNum;		// 进入人数
    //uint64_t u64OutNum;		// 出去人数
    uint32_t bg_pic_len; 		// 背景图jpg长度 0则没有
	char* bg_pic_data; 			// 背景图jpg数据 [内部alloc]
    uint32_t bg_pic_w;          // 背景图 宽
    uint32_t bg_pic_h;          // 背景图 高
}AISDK_DEF_S_EVENT_PASSENGER_FLOW;

//客流统计周期事件帧
typedef struct ST_AISDK_DEF_EVENT_CYCLE_PASSENGER_FLOW
{
    uint32_t u32TimeSec;    // 发送时间s(周期统计结束时间)
    uint32_t u32TimeUSec;   // 发送时间us
    uint64_t u64InNum;		// 统计的进入人数
	uint64_t u64OutNum;		// 统计的出去人数
}AISDK_DEF_S_EVENT_CYCLE_PASSENGER_FLOW;

//客流统计-拌线 出入（瞬时）事件帧
typedef struct ST_AISDK_DEF_EVENT_PASSENGER_FLOW_LINE
{
    uint32_t nFrameId;      // 帧ID
    uint32_t nSrcW;         // 原始帧宽
    uint32_t nSrcH;         // 原始帧高
    uint32_t u32TimeSec;    // 发生时间s
    uint32_t u32TimeUSec;   // 发生时间us
    uint32_t u32EventID;    // [目前无效] 事件ID    
    uint32_t u32HaveTagetType;      // [目前无效] 包含哪些类型目标    多种目标可与  参考 E_AI_TARGET_TYPE 
    uint32_t u32HaveInOutStatus;    // 出入状态:1-有进入; 2-有离开; 3-有进入也有离开
}AISDK_DEF_S_EVENT_PASSENGER_FLOW_LINE;

//区域人数统计出入事件帧
typedef struct ST_AISDK_DEF_EVENT_GATHERING_FLOW
{
    uint32_t nFrameId;      // 帧ID
    uint32_t u32TimeSec;    // 发生时间s
    uint32_t u32TimeUSec;   // 发生时间us
    int      s32InoutNum;	//出入人数:  -2：离开2人  1：进入1人
    uint32_t      u32CurNum;    //当前人数
    uint32_t bg_pic_len; 		// 背景图jpg长度 0则没有
	char* bg_pic_data; 			// 背景图jpg数据 [内部alloc]
    uint32_t bg_pic_w;          // 背景图 宽
    uint32_t bg_pic_h;          // 背景图 高
}AISDK_DEF_S_EVENT_GATHERING_FLOW;

//区域人数统计周期事件帧
typedef struct ST_AISDK_DEF_EVENT_CYCLE_GATHERING_FLOW
{
    uint32_t u32TimeSec;    // 发送时间s(周期统计结束时间)
    uint32_t u32TimeUSec;   // 发送时间us
    uint64_t u64InNum;		// 统计的进入人数
	uint64_t u64OutNum;		// 统计的出去人数
    uint32_t u32CurNum;     // 当前人数
}AISDK_DEF_S_EVENT_CYCLE_GATHERING_FLOW;

//宠物检测事件帧 
typedef struct ST_AISDK_DEF_EVENT_PETDETECT
{
    uint32_t nFrameId;      // 帧ID
    uint32_t nSrcW;         // 原始帧宽
    uint32_t nSrcH;         // 原始帧高
    uint32_t u32TimeSec;    // 发生时间s
    uint32_t u32TimeUSec;   // 发生时间us
    uint32_t uTargetNum;    // 目标数量
    S_RECT   stTargetRect[MAX_FACE_CNT_ONCE];   // 目标区域
    uint64_t u64TargetId[MAX_FACE_CNT_ONCE];    // 目标id
}AISDK_DEF_S_EVENT_PETDETECT;

//跌倒检测事件帧 
typedef struct ST_AISDK_DEF_EVENT_TUMBLE_DETECT
{
    uint32_t nFrameId;      // 帧ID
    uint32_t nSrcW;         // 原始帧宽
    uint32_t nSrcH;         // 原始帧高
    uint32_t u32TimeSec;    // 发生时间s
    uint32_t u32TimeUSec;   // 发生时间us
    uint32_t uTargetNum;    // 目标数量
    S_RECT   stTargetRect[MAX_FACE_CNT_ONCE];   // 目标区域
    uint64_t u64TargetId[MAX_FACE_CNT_ONCE];    // 目标id
    uint32_t u32TargetStatus[MAX_FACE_CNT_ONCE];// 目标状态（注：有检测事件出来，状态起码是有效的）   0 无效  1 unknown  2 no tumble  3 tumble 
}AISDK_DEF_S_EVENT_TUMBLE_DETECT;

//对象检测事件帧 
typedef struct ST_AISDK_DEF_EVENT_OBJDETECT
{
    uint32_t nFrameId;      // 帧ID
    uint32_t nSrcW;         // 原始帧宽
    uint32_t nSrcH;         // 原始帧高
    uint32_t u32TimeSec;    // 发生时间s
    uint32_t u32TimeUSec;   // 发生时间us
    uint32_t uObjNum;       // 对象数量  <= MAX_OBJ_CNT_ONCE
    S_RECT   stObjRect[MAX_OBJ_CNT_ONCE];      // 对象区域
    uint64_t u64ObjId[MAX_OBJ_CNT_ONCE];     // 对象 track id 
    uint8_t  uObjCategory[MAX_OBJ_CNT_ONCE]; // 对象类型  0 unknow   1 face   2 body   3 vehicle   4 cycle   5 plate   6 pet   7 flame 
    uint8_t  uStatusActionless[MAX_OBJ_CNT_ONCE]; // 人形静止状态  0 运动中 1 运动目标归于静止 2 未决中 
    // float    confidence;    // 评分 
}AISDK_DEF_S_EVENT_OBJDETECT;

//对象检测事件帧-轻量 
typedef struct ST_AISDK_DEF_EVENT_OBJDETECT_LITE
{
    uint32_t nFrameId;      // 帧ID
    uint32_t nSrcW;         // 原始帧宽
    uint32_t nSrcH;         // 原始帧高
    uint32_t u32TimeSec;    // 发生时间s
    uint32_t u32TimeUSec;   // 发生时间us
    uint32_t uObjNum;       // 对象数量  <= MAX_OBJ_CNT_ONCE
    S_RECT   stObjRect[MAX_OBJ_CNT_ONCE_LITE];      // 对象区域
    uint64_t u64ObjId[MAX_OBJ_CNT_ONCE_LITE];     // 对象 track id 
    uint8_t  uObjCategory[MAX_OBJ_CNT_ONCE_LITE]; // 对象类型  0 unknow   1 face   2 body   3 vehicle   4 cycle   5 plate   6 pet   7 flame 
    uint8_t  uStatusActionless[MAX_OBJ_CNT_ONCE_LITE]; // 【目前无效】人形静止状态  0 运动中 1 运动目标归于静止 
    uint32_t u32ObjStatus[MAX_OBJ_CNT_ONCE_LITE];// 目标状态 [保留]
    // float    confidence;    // 评分 
}AISDK_DEF_S_EVENT_OBJDETECT_LITE;

//车辆抓拍事件帧 
typedef struct ST_AISDK_DEF_EVENT_VEHICLECAP
{
    uint32_t nFrameId;      // 帧ID
    uint32_t nSrcW;         // 原始帧宽
    uint32_t nSrcH;         // 原始帧高
    uint32_t u32TimeSec;    // 发生时间s
    uint32_t u32TimeUSec;   // 发生时间us
    uint64_t trackId;       // 对象 tarck id 
    S_RECT   stRect;        // 对象区域
    S_RECT   stDetectRect;  // 对象检测区域-debug使用
    float    confidence;    // 评分 
    uint32_t pic_len;       // 对象图jpg长度   0则没有
    uint32_t pic_w;         // 对象图 宽
    uint32_t pic_h;         // 对象图 高
    char*    pic_data;      // 对象图jpg数据 [内部alloc]
    uint32_t bg_pic_len;    // 背景图jpg长度   0则没有
    uint32_t bg_pic_w;      // 背景图 宽
    uint32_t bg_pic_h;      // 背景图 高
    char*    bg_pic_data;   // 背景图jpg数据 [内部alloc]    
    uint8_t  uHavePlate;        // 是否有车牌信息：1 有
    uint64_t uPlateTrackId;     // 车牌对象id
    S_RECT   stPlateRect;       // 车牌对象区域
    S_RECT   stPlateDetectRect; // 车牌对象实际检测区域-debug使用
    float    fPlateConfidence;  // 车牌对象评分 
    uint32_t uPlatePicLen;      // 车牌图jpg长度   0则没有
    uint32_t uPlatePicW;        // 车牌图 宽
    uint32_t uPlatePicH;        // 车牌图 高
    char*    pPlatePicData;     // 车牌图jpg数据 [内部alloc]
    uint64_t uTrackIdSub;   // 对象子id  进一步区分不同的车辆
    uint8_t  reserved[40];
}AISDK_DEF_S_EVENT_VEHICLECAP;

//车辆识别事件帧 
typedef struct ST_AISDK_DEF_EVENT_VEHICLERECO
{
    uint32_t nFrameId;      // 帧ID
    uint32_t nSrcW;         // 原始帧宽
    uint32_t nSrcH;         // 原始帧高
    uint32_t u32TimeSec;    // 发生时间s
    uint32_t u32TimeUSec;   // 发生时间us
    uint64_t trackId;       // 对象 tarck id 
    S_RECT   stRect;        // 对象区域
    S_RECT   stDetectRect;  // 对象检测区域-debug使用
    float    confidence;    // 评分 
    uint8_t  uColor;        // 对象颜色  0 invalid  
    /*  0 is unknow
        "color.black_score", 
        "color.white_score", 
        "color.silvery_score", 
        "color.gray_score", 
        "color.brown_score", 
        "color.red_score", 
        "color.orange_score", 
        "color.yellow_score", 
        "color.green_score", 
        "color.cyan_score", 
        "color.blue_score", 
        "color.pink_score", 
        "color.purple_score", 
        "color.champagne_score"           */
    uint8_t  uOrientation;  // 对象方向  0 invalid  1 正向  2 背向  3 其他 
    uint8_t  uType;         // 对象类型  0 invalid  
    /*  0 is unknow
        "vehicle_type.car_score", 
        "vehicle_type.suv_score", 
        "vehicle_type.microbus_score",
        "vehicle_type.midibus_score", 
        "vehicle_type.pickup_score", 
        "vehicle_type.truck_score"           */
    uint32_t pic_len;       // 对象图jpg长度   0则没有
    uint32_t pic_w;         // 对象图 宽
    uint32_t pic_h;         // 对象图 高
    char*    pic_data;      // 对象图jpg数据 [内部alloc]
    uint32_t bg_pic_len;    // 背景图jpg长度   0则没有
    uint32_t bg_pic_w;      // 背景图 宽
    uint32_t bg_pic_h;      // 背景图 高
    char*    bg_pic_data;   // 背景图jpg数据 [内部alloc]    
    uint8_t  uHavePlate;        // 是否有车牌信息：1 有
    uint64_t uPlateTrackId;     // 车牌对象id
    S_RECT   stPlateRect;       // 车牌对象区域
    S_RECT   stPlateDetectRect; // 车牌对象实际检测区域-debug使用
    float    fPlateConfidence;  // 车牌对象评分 
    uint8_t  uPlateColor;       // 车牌颜色  0 invalid 
        /*  0 is unknow
        blue = 1
        yellow
        black
        white
        green
        small_new_energy
        large_new_energy
        absence           */
    uint8_t  uPlateStyle;       // 车牌风格  0 invalid  1 双行  2 单行
    uint8_t  uPlateCategory;    // 车牌种类  0 invalid  1 小车牌  2 大车牌
    char     szPlateCode[48];   // 车牌号码 
    float    fPlateCodeConfidence;  // 车牌号码评分 
    uint32_t uPlatePicLen;      // 车牌图jpg长度   0则没有
    uint32_t uPlatePicW;        // 车牌图 宽
    uint32_t uPlatePicH;        // 车牌图 高
    char*    pPlatePicData;     // 车牌图jpg数据 [内部alloc]
    uint64_t uTrackIdSub;   // 对象子id  进一步区分不同的车辆
    uint8_t  reserved[40];
}AISDK_DEF_S_EVENT_VEHICLERECO;

//车位状态事件帧
typedef struct ST_AISDK_DEF_EVENT_PARKING_SPACE_STATUS
{
    uint32_t nFrameId;      // 帧ID
    uint32_t nSrcW;         // 原始帧宽
    uint32_t nSrcH;         // 原始帧高
    uint32_t u32TimeSec;    // 发生时间s
    uint32_t u32TimeUSec;   // 发生时间us
    uint32_t u32EventID;    // 事件ID    目前无效
    uint32_t u32RegId;      // 区域 id 
    uint32_t u32RegHaveVehicle; // 区域是否有车
    S_RECT   stObjRect;     // 对象坐标
    uint8_t  uHavePlate;        // 是否有车牌信息：1 有
    uint8_t  uPlateColor;       // 车牌颜色  0 invalid 
        /*  0 is unknow
        blue = 1
        yellow
        black
        white
        green
        small_new_energy
        large_new_energy
        absence           */
    uint8_t  uPlateStyle;       // 车牌风格  0 invalid  1 双行  2 单行
    uint8_t  uPlateCategory;    // 车牌种类  0 invalid  1 小车牌  2 大车牌
    char     szPlateCode[48];   // 车牌号码 
    float    fPlateCodeConfidence;  // 车牌号码评分 
    uint32_t uPlatePicLen;      // 车牌图jpg长度   0则没有
    uint32_t uPlatePicW;        // 车牌图 宽
    uint32_t uPlatePicH;        // 车牌图 高
    char*    pPlatePicData;     // 车牌图jpg数据 [内部alloc]
    uint32_t bg_pic_len; 		// 背景图jpg长度 0则没有
	char* bg_pic_data; 			// 背景图jpg数据 [内部alloc]
    uint32_t bg_pic_w;          // 背景图 宽
    uint32_t bg_pic_h;          // 背景图 高
}AISDK_DEF_S_EVENT_PARKING_SPACE_STATUS;

//车位状态变化报警事件帧
//...

//目标消失事件帧
typedef struct ST_AISDK_DEF_EVENT_TARGET_DIE
{
    uint32_t nFrameId;      // 帧ID
    uint32_t u32TimeSec;    // 发生时间s
    uint32_t u32TimeUSec;   // 发生时间us
    uint32_t u32TargetNum;	// 目标数量
    uint64_t u64ObjId[MAX_OBJ_CNT_ONCE];     // 对象 track id 
    uint8_t  uObjCategory[MAX_OBJ_CNT_ONCE];  // 对象类型  0 unknow   1 face   2 body   3 vehicle   4 cycle   5 plate   6 pet   7 flame 
    uint8_t  uStatusActionless[MAX_OBJ_CNT_ONCE]; // 静止状态  0 运动中 1 运动目标归于静止 
    E_AI_EVENT_TYPE eRelatedEventType[MAX_OBJ_CNT_ONCE];     // 相关联的事件类型
}AISDK_DEF_S_EVENT_TARGET_DIE;

//手势检测事件帧
typedef struct ST_AISDK_DEF_EVENT_GESTUREDETECT
{
    uint32_t nFrameId;      // 帧ID
    uint32_t nSrcW;         // 原始帧宽
    uint32_t nSrcH;         // 原始帧高
    uint32_t u32TimeSec;    // 发生时间s
    uint32_t u32TimeUSec;   // 发生时间us
    uint32_t uObjNum;       // 对象数量  <= MAX_OBJ_CNT_ONCE_LITE
    S_RECT   stObjRect[MAX_OBJ_CNT_ONCE_LITE];      // 对象区域
    uint8_t  uObjCategory[MAX_OBJ_CNT_ONCE_LITE];   // 对象类型  0:call 1:dislike 2:fist 3:four 4:like 5:ok 6:one 7:palm 8:peace 9:rock 10:stop 11:stop_inverted 12:three 13:peace_inverted 14:no_gesture
    float    fObjConfidence[MAX_OBJ_CNT_ONCE_LITE]; // 置信度
}AISDK_DEF_S_EVENT_GESTUREDETECT;










/*************************************
 * JSON配置说明 ***********************
**************************************/


#define JSON_FACERECO_FILT_ACTIONLESS_TARGET_DISTANCELEN    "FiltActionlessTargetDistancelenRatio"
#define	JSON_FACERECO_FILT_ACTIONLESS_TARGET_ZOOM_W         "FiltActionlessTargetZoomWidthRatio"
#define	JSON_FACERECO_FILT_ACTIONLESS_TARGET_ZOOM_H         "FiltActionlessTargetZoomHeightRatio"

#define JSON_BODY_DETECT             "body_detect"
#define JSON_BODY_DETECT_MIN_WIDTH   "min_width"
#define JSON_BODY_DETECT_MIN_HEIGHT  "min_height"
#define JSON_BODY_DETECT_QUALITY     "quality"

#define JSON_FACE_DETECT             "face_detect"
#define JSON_FACE_DETECT_MIN_WIDTH   "min_width"
#define JSON_FACE_DETECT_MIN_HEIGHT  "min_height"
#define JSON_FACE_DETECT_FACE_POSE   "face_pose"
#define JSON_FACE_DETECT_POSE_PITCH  "pitch"
#define JSON_FACE_DETECT_POSE_YAW    "yaw"
#define JSON_FACE_DETECT_POSE_ROLL   "roll"
#define JSON_FACE_DETECT_POSE_BLUR   "blur"

#define JSON_FACE_CAPTURE             "face_capture"
#define JSON_FACE_CAPTURE_MIN_WIDTH   "min_width"
#define JSON_FACE_CAPTURE_MIN_HEIGHT  "min_height"
#define JSON_FACE_CAPTURE_QUALITY     "quality"
#define JSON_FACE_CAPTURE_FACE_POSE   "face_pose"
#define JSON_FACE_CAPTURE_SENSITIVE   "sensitive"
#define	JSON_FACE_CAPTURE_POLYGON     "polygon"
#define	JSON_FACE_CAPTURE_POLYGON_NUM "num"
#define	JSON_FACE_CAPTURE_POLYGON_POINTS "points"
#define JSON_FACE_CAPTURE_POSE_PITCH  "pitch"
#define JSON_FACE_CAPTURE_POSE_YAW    "yaw"
#define JSON_FACE_CAPTURE_POSE_ROLL   "roll"
#define JSON_FACE_CAPTURE_POSE_BLUR   "blur"

#define JSON_FACE_PUSH                 "face_push"
#define JSON_FACE_PUSH_MIN_WIDTH       "min_width"
#define JSON_FACE_PUSH_MIN_HEIGHT      "min_height"
#define JSON_FACE_PUSH_QUALITY         "quality"
#define JSON_FACE_PUSH_MODE            "push_mode"
#define JSON_FACE_PUSH_INTERVAL_TIMES  "interval_times"
#define JSON_FACE_PUSH_COUNTS          "push_counts"
#define JSON_FACE_PUSH_POSE_PITCH      "pitch"
#define JSON_FACE_PUSH_FACE_POSE       "face_pose"
#define JSON_FACE_PUSH_POSE_YAW        "yaw" 
#define JSON_FACE_PUSH_POSE_ROLL       "roll"
#define JSON_FACE_PUSH_POSE_BLUR       "blur"
#define JSON_FACE_PUSH_FRAME_CACHE_DEPTH "frame_cache_depth"
#define JSON_FACE_PUSH_CROP_CONFIG       "crop_config"
#define JSON_FACE_PUSH_CROP_SCALE_LEFT   "scale_left" 
#define JSON_FACE_PUSH_CROP_SCALE_RIGHT  "scale_right"
#define JSON_FACE_PUSH_CROP_SCALE_TOP    "scale_top"
#define JSON_FACE_PUSH_CROP_SCALE_BOTTOM "scale_bottom"
#define JSON_FACE_PUSH_CROP_JPEG_QUALITY "jpeg_quality"
#define JSON_FACE_PUSH_CROP_JPEG_QPLEVEL "jpeg_qplevel"

#define JSON_FACE_PUSH_PANORAMA_CONFIG       "panorama_config"
#define JSON_FACE_PUSH_PANORAMA_WIDTH        "width" 
#define JSON_FACE_PUSH_PANORAMA_HEIGHT       "height"
#define JSON_FACE_PUSH_PANORAMA_JPEG_QUALITY "jpeg_quality"
#define JSON_FACE_PUSH_PANORAMA_JPEG_QPLEVEL "jpeg_qplevel"

#define JSON_PLATE_DETECT             "plate_detect"
#define JSON_PLATE_DETECT_MIN_WIDTH   "min_width"
#define JSON_PLATE_DETECT_MIN_HEIGHT  "min_height"
#define JSON_PLATE_DETECT_QUALITY     "quality"

#define JSON_PLATE_CAPTURE             "plate_capture"
#define JSON_PLATE_CAPTURE_MIN_WIDTH   "min_width"
#define JSON_PLATE_CAPTURE_MIN_HEIGHT  "min_height"
#define JSON_PLATE_CAPTURE_QUALITY     "quality"

#define JSON_PLATE_PUSH                 "plate_push"
#define JSON_PLATE_PUSH_MIN_WIDTH       "min_width"
#define JSON_PLATE_PUSH_MIN_HEIGHT      "min_height"
#define JSON_PLATE_PUSH_QUALITY         "quality"
#define JSON_PLATE_PUSH_MODE            "push_mode"
#define JSON_PLATE_PUSH_INTERVAL_TIMES  "interval_times"
#define JSON_PLATE_PUSH_COUNTS          "push_counts"
#define JSON_PLATE_PUSH_FRAME_CACHE_DEPTH "frame_cache_depth"
#define JSON_PLATE_PUSH_CROP_CONFIG       "crop_config"
#define JSON_PLATE_PUSH_CROP_SCALE_LEFT   "scale_left" 
#define JSON_PLATE_PUSH_CROP_SCALE_RIGHT  "scale_right"
#define JSON_PLATE_PUSH_CROP_SCALE_TOP    "scale_top"
#define JSON_PLATE_PUSH_CROP_SCALE_BOTTOM "scale_bottom"
#define JSON_PLATE_PUSH_CROP_JPEG_QUALITY "jpeg_quality"
#define JSON_PLATE_PUSH_CROP_JPEG_QPLEVEL "jpeg_qplevel"

#define JSON_PLATE_PUSH_PANORAMA_CONFIG       "panorama_config"
#define JSON_PLATE_PUSH_PANORAMA_WIDTH        "width" 
#define JSON_PLATE_PUSH_PANORAMA_HEIGHT       "height"
#define JSON_PLATE_PUSH_PANORAMA_JPEG_QUALITY "jpeg_quality"
#define JSON_PLATE_PUSH_PANORAMA_JPEG_QPLEVEL "jpeg_qplevel"

#define JSON_ATTR_PLATE_COLOR           "color"
#define JSON_ATTR_PLATE_STYLE           "style"
#define JSON_ATTR_PLATE_SCORE           "score"
#define JSON_ATTR_PLATE_NAME            "name"
#define JSON_ATTR_PLATE_STATE           "state"
#define JSON_ATTR_PLATE_CODE_CONFIDENCE "code_confidence"
#define JSON_ATTR_PLATE_CODE_KILLED     "code_killed"
#define JSON_ATTR_PLATE_CODE_RESULT     "code_result"

#define JSON_FACE_SNAPSHOT             	"face_snapshot"
#define JSON_FACE_SNAPSHOT_ENABLE       "enable"
#define JSON_FACE_SNAPSHOT_CAP_MODE   	"capMode"
#define JSON_FACE_SNAPSHOT_QUALITY  	"faceQuality"
#define JSON_FACE_SNAPSHOT_IS_CAP_BACK  "isCapBack"

#define JSON_MASK_DETECT             	"mask_detect"
#define JSON_MASK_DETECT_ENABLE   		"enable"
#define JSON_MASK_DETECT_ALARM_TYPE   	"alarmType"

#define JSON_FACE_RECO             		"face_reco"
#define JSON_FACE_RECO_ENABLE       	"enable"

#define JSON_FACE_JPG_DETECT             	"face_jpg_detect"
#define JSON_FACE_JPG_DETECT_YUV_SIZE_MAX   "maxYuvSize"


#define JSON_ENABLE_TRUE   		"true"
#define JSON_ENABLE_FALSE   	"false"



#define  ___AISDK_JSON_CFG_FACECAP___  
/*
*/
#define  ___AISDK_JSON_CFG_FACERECO___  
/* 参照协议接口文档和产品功能更新 ！！！！！！！！！！
"aisdk_cfg_facereco":
{
    "FiltActionlessTargetDistancelenRatio": 0.15,   ## 过滤静止目标-中心点偏差的比例的阈值 [0,1] 未传值默认0.15
    "FiltActionlessTargetZoomWidthRatio": 0.2,     ## 过滤静止目标-缩放宽度方向偏差的比例的阈值 [0,1] 未传值默认0.2
    "FiltActionlessTargetZoomHeightRatio": 0.15,    ## 过滤静止目标-缩放高度方向偏差的比例的阈值 [0,1] 未传值默认0.15
    "body_detect":
    {
        "min_width": 0,
        "min_height": 0,
        "quality": 70
    },
    "face_detect":
    {
        "min_width": 0,
        "min_height": 0,
        "face_pose":
        {
            "pitch": 180,
            "yaw":   180,
            "roll":  180,
            "blur":  0.9
        }
    },
    "face_capture":
    {
        "min_width": 30,
        "min_height": 30,
        "quality": 50,
        "face_pose":
        {
            "pitch": 180,
            "yaw":   180,
            "roll":  180,
            "blur":  0.9
        }
    },
    "face_snapshot":
    {
		"enable": "true",
		"capMode": 0,
		"faceQuality": 80,
		"isCapBack": 1
    },
    "mask_detect":
    {
    	"enable": "true",
		"alarmType": 0
    },
    "face_push":
    {
        "min_width": 30,
        "min_height": 30,
        "quality": 20,
        "push_mode": 2,
        "interval_times": 2000,
        "push_counts": 10,
        "frame_cache_depth": 15.0,
        "face_pose":
        {
            "pitch": 180,
            "yaw":   180,
            "roll":  180,
            "blur":  0.9
        },
        "crop_config":
        {
            "scale_left": 0.5,
            "scale_right": 0.5,
            "scale_top": 0.5,
            "scale_bottom": 0.5,
            "jpeg_quality": 70.0,
            "jpeg_qplevel": 5.0
        },
        "panorama_config":
        {
            "width": 1280,
            "height": 720,
            "jpeg_quality": 60.0,
            "jpeg_qplevel": 5.0
        }
    }
    rule
    {
        ...
    }
}
*/
#define  ___AISDK_JSON_CFG_PARKING_SPACE___  
/*
"aisdk_cfg_parking_space":
{
    "enable": 1,                                    ## 使能
    "vehicle_detect":                               ## 车辆检测
    {
        "enable": 1,                                    ## 使能
        "enFiltActionlessTarget": 0,                    ## 车辆检测事件是否过滤静止目标  有效依赖于上层的enFiltActionlessTarget开启；未传值默认1；
        "min_width": 0,                                 ## 车辆检测对象最小宽度 单位：像素 【暂无效】
        "min_height": 0,                                ## 车辆检测对象最小高度 单位：像素 【暂无效】
        "min_quality": 0.001                            ## 车辆检测对象最小质量 (0,1) 【暂无效】
    },
    "plate_detect":                                 ## 车牌检测
    {
        "enable": 1,                                    ## 使能
        "enFiltActionlessTarget": 0,                    ## 车牌检测事件是否过滤静止目标  有效依赖于上层的enFiltActionlessTarget开启；未传值默认1；
        "min_width": 0,                                 ## 车牌检测对象最小宽度 单位：像素 【暂无效】
        "min_height": 0,                                ## 车牌检测对象最小高度 单位：像素 【暂无效】
        "min_quality": 0.001                            ## 车牌检测对象最小质量 (0,1) 【暂无效】
    },
    "parking_space":                                ## 车位检测
    {
        "enable": 1,                                    ## 使能
        "min_width": 0,                                 ## 车辆识别对象最小宽度 单位：像素 【暂无效】
        "min_height": 0,                                ## 车辆识别对象最小高度 单位：像素 【暂无效】
        "min_quality": 0.001                            ## 车辆识别对象最小质量 (0,1) 【暂无效】
        "polygon": [                                    ## 车辆识别多边形区域  【目前只支持最多2个】
            {
                "id": 1,                                        ## 多边形区域id
                "enable": 1,                                    ## 使能
                "num": 4,                                       ## 多边形区域点个数
                "pic_width": 1920,                             ## 坐标系宽度 【暂无效】
                "pic_height": 1080,                            ## 坐标系高度 【暂无效】
                "points": [                                     ## 多边形区域点坐标
                    [50, 40],
                    [900, 40],
                    [900, 1000],
                    [50, 1000]
                ]
            },
            {
                "id": 2,                                        ## 多边形区域id
                "enable": 1,                                    ## 使能
                "num": 4,                                       ## 多边形区域点个数
                "pic_width": 1920,                             ## 坐标系宽度 【暂无效】
                "pic_height": 1080,                            ## 坐标系高度 【暂无效】
                "points": [                                     ## 多边形区域点坐标
                    [1020, 40],
                    [1860, 40],
                    [1860, 1000],
                    [1020, 1000]
                ]
            }
        ],
        "status_event_time_interval": 5000,             ## 状态事件推送时间间隔   单位ms   0为关闭
        "en_cap_image_vehicle": 1,                      ## 是否使能车形图 
        "en_cap_image_plate": 1,                        ## 是否使能车牌图 
        "cap_image_crop_qp": 99,                        ## 抓图编码质量 1-99 默认99 【暂无效】
        "en_cap_image_bg": 0,                           ## 是否使能背景图 
        "cap_image_bg_qp": 25,                          ## 背景图编码质量 1-99 默认25 【暂无效】
    }
}
*/
#define  ___AISDK_JSON_CFG_VEHICLE_RECO___  
/*
"aisdk_cfg_vehicle_reco":
{
    "----comment----start_param": "启动参数，只有Start时传入才有效",
    "----comment----enable": "使能",
    "----comment----enFiltActionlessTarget": "过滤静止目标  过滤范围：识别事件、车辆/车牌检测事件",
    "----comment----FiltActionlessTargetDistancelenRatio": "过滤静止目标-中心点偏差的比例的阈值 [0,1] 未传值默认0.15",
    "----comment----FiltActionlessTargetZoomWidthRatio": "过滤静止目标-缩放宽度方向偏差的比例的阈值 [0,1] 未传值默认0.2",
    "----comment----FiltActionlessTargetZoomHeightRatio": "过滤静止目标-缩放高度方向偏差的比例的阈值 [0,1] 未传值默认0.15",
    "----comment----enTrackDiePush": "目标消失推送使能",
    "----comment----vehicle_detect": "车辆检测",
    "----comment----FastActionlessOfRegion": "根据目标出现位置快速归类为静止目标 功能",
    "start_param": {
        "----comment----RegionalVer": "0 中国（默认）  1 越南",
        "RegionalVer": 0
    },
    "enable": 1, 
    "enFiltActionlessTarget": 1, 
    "FiltActionlessTargetDistancelenRatio": 0.15, 
    "FiltActionlessTargetZoomWidthRatio": 0.2, 
    "FiltActionlessTargetZoomHeightRatio": 0.15, 
    "FastActionlessOfRegion": {
        "----comment----RecListNumTotal": "记录列表空间个数长度 0则关闭功能。注意：开-关-开-关 顺序调用，用完关闭以释放",
        "----comment----RecListTimeLen": "记录列表记录时间长度（单位：s），默认1800",
        "RecListNumTotal": 0,
        "RecListTimeLen": 1800
    },
    "enTrackDiePush":   1, 
    "vehicle_detect": 
    {
        "----comment----enable": "使能",
        "----comment----enFiltActionlessTarget": "车辆检测事件是否过滤静止目标  有效依赖于上层的enFiltActionlessTarget开启；未传值默认1；",
        "----comment----min_width": "车辆检测对象最小宽度 单位：像素 【暂无效】",
        "----comment----min_height": "车辆检测对象最小高度 单位：像素 【暂无效】",
        "----comment----min_quality": "车辆检测对象最小质量 (0,1) 【暂无效】",
        "enable": 1, 
        "enFiltActionlessTarget": 0, 
        "min_width": 0, 
        "min_height": 0, 
        "min_quality": 0.001 
    },
    "----comment----plate_detect": "车牌检测",
    "plate_detect": 
    {
        "----comment----enable": "使能",
        "----comment----enFiltActionlessTarget": "车牌检测事件是否过滤静止目标  有效依赖于上层的enFiltActionlessTarget开启；未传值默认1；",
        "----comment----min_width": "车牌检测对象最小宽度 单位：像素 【暂无效】",
        "----comment----min_height": "车牌检测对象最小高度 单位：像素 【暂无效】",
        "----comment----min_quality": "车牌检测对象最小质量 (0,1) 【暂无效】",
        "enable": 1, 
        "enFiltActionlessTarget": 0, 
        "min_width": 0, 
        "min_height": 0, 
        "min_quality": 0.001 
    },
    "----comment----vehicle_reco": "车辆识别",
    "vehicle_reco": 
    {
        "----comment----enable": "使能",
        "----comment----min_width": "车辆识别对象最小宽度 单位：像素 【暂无效】",
        "----comment----min_height": "车辆识别对象最小高度 单位：像素 【暂无效】",
        "----comment----min_quality": "车辆识别对象最小质量 (0,1) 【暂无效】",
        "----comment----polygon": "车辆识别多边形区域",
        "enable": 1, 
        "min_width": 0, 
        "min_height": 0, 
        "min_quality": 0.001, 
        "polygon": { 
            "----comment----num": "多边形区域点个数",
            "----comment----points": "多边形区域点坐标",
            "num": 4, 
            "points": [ 
                [360, 180],
                [9640, 180],
                [9640, 9820],
                [360, 9820]
            ]
        },
        "----comment----chooes_best_mode": "车辆识别选优模式 0:质量 1:快速  默认0",
        "----comment----en_cap_image_vehicle": "车辆识别是否使能车形图",
        "----comment----en_cap_image_plate": "车辆识别是否使能车牌图",
        "----comment----cap_image_crop_qp": "车辆识别抓图编码质量 1-99 默认99",
        "----comment----en_cap_image_bg": "车辆识别是否使能背景图",
        "----comment----cap_image_bg_qp": "车辆识别背景图编码质量 1-99 默认25",
        "----comment----track_continue_time": "跟踪中断的接续时间 单位：毫秒 默认4000",
        "----comment----repetitive_push": "重复推送",
        "chooes_best_mode": 0, 
        "en_cap_image_vehicle": 1, 
        "en_cap_image_plate": 1, 
        "cap_image_crop_qp": 99, 
        "en_cap_image_bg": 0, 
        "cap_image_bg_qp": 25, 
        "track_continue_time": 4000, 
        "repetitive_push": 
        {
            "----comment----enable": "使能",
            "----comment----time_interval": "时间间隔 单位：秒 默认120",
            "enable": 0, 
            "time_interval": 120 
        }        
    },
    "----comment----region_taget": "区域功能检测的目标",
    "region_taget": 
    {
        "----comment----en_human": "人",
        "----comment----en_non_motor": "非机动车",
        "----comment----en_vehicle": "车",
        "en_human": 1, 
        "en_cycle": 1, 
        "en_vehicle": 1 
    },
    "----comment----ElectronicFence": "电子围栏。 --参考下面完整版说明",
    "ElectronicFence": { 
        "enable": false
    },
    "----comment----CrossBoardDetect": "拌线检测。 --参考下面完整版说明",
    "CrossBoardDetect": { 
        "enable": "true",
        "sensitive": 3,
        "orientation": 2,
        "detectTarget": 3, 
        "enFiltActionlessTarget": 0, 
        "times_die": 15,
        "line": [
            [0,300],
            [800,300]
            ]
    },
    "----comment----OffDutyDetect": "离岗检测。 --参考下面完整版说明",
    "OffDutyDetect": { 
        "enable":       "false"
    },
    "----comment----PassengerFlow": "乘客流量。 --参考下面完整版说明",
    "PassengerFlow": { 
        "enable":       "false"
    }
}
*/
#define  ___AISDK_JSON_CFG_HUMANDETECT___  
/*
{
    "----comment----Scene": "场景说明：最大支持4路流检测，实际运行时支持的路数取决于启动参数的配置",
    "----comment----start_param": "启动参数，只有Start时传入才有效",
    "----comment----TargetTrackMode": "目标跟踪模式：0 算法track(默认)； 1 xrs track(注意需libhumanTrack.so库在可链接位置启动才有效)。 注意：切换后会清理所有track相关业务的数据缓存",
    "----comment----enFiltActionlessTarget": "过滤静止使能",
    "----comment----ClearRecWhenEnable": "使能开启时是否清理静止过滤的记录数据；默认1(是)",
    "----comment----FiltActionlessTargetDistancelenRatio": "过滤静止目标-中心点偏差的比例的阈值 [0,1] 未传值默认0.15", 
    "----comment----FiltActionlessTargetZoomWidthRatio": "过滤静止目标-缩放宽度方向偏差的比例的阈值 [0,1] 未传值默认0.2", 
    "----comment----FiltActionlessTargetZoomHeightRatio": "过滤静止目标-缩放高度方向偏差的比例的阈值 [0,1] 未传值默认0.15", 
    "----comment----FiltActionlessTargetDistancelen": "中心点偏差的阈值 (0,送检分辨率对角长度] 未传值默认25", 
    "----comment----FiltActionlessTargetZoomWidth": "缩放宽度方向偏差的阈值 (0,送检分辨率宽度] 未传值默认20", 
    "----comment----FiltActionlessTargetZoomHeight": "缩放高度方向偏差的阈值 (0,送检分辨率高度] 未传值默认20", 
    "----comment----FastActionlessOfRegion": "根据目标出现位置快速归类为静止目标 功能",
    "----comment----PushHumanDetect": "推送人形检测事件。",
    "----comment----PushVehicleDetect": "推送车形检测事件。",
    "----comment----AlgoParam": "算法层的运行参数",
    "start_param": {
        "----comment----AiChnNum": "AI通道数 [0,4(最大支持)]。可以不配置，默认1",
        "AiChnNum": 1
    },
    "TargetTrackMode": 0,
    "enFiltActionlessTarget":       1,
    "ClearRecWhenEnable": 1,
    "FiltActionlessTargetDistancelenRatio": 0.15, 
    "FiltActionlessTargetZoomWidthRatio": 0.2, 
    "FiltActionlessTargetZoomHeightRatio": 0.15, 
    "FiltActionlessTargetDistancelen": 25,
    "FiltActionlessTargetZoomWidth": 20,
    "FiltActionlessTargetZoomHeight": 20,
    "FastActionlessOfRegion": {
        "----comment----RecListNumTotal": "记录列表空间个数长度 0则关闭功能。注意：开-关-开-关 顺序调用(用完关闭以释放)",
        "----comment----RecListTimeLen": "记录列表记录时间长度（单位：s），默认1800",
        "RecListNumTotal": 0,
        "RecListTimeLen": 1800
    },
    "PushHumanDetect": {
        "----comment----enable": "使能 1：开启 0：关闭。 默认1",
        "----comment----filt_mode": "过滤模式 0:filt by any one (所有条件皆超过min阈值才为有效目标，有任意一个小于min则过滤)  1:filt by all (有任意一个条件超过min阈值则为有效目标，所有条件皆小于min才过滤)   默认0",
        "----comment----min_width": "对象最小宽度 单位：像素。不需要生效则设置最低值0即可",
        "----comment----min_height": "对象最小高度 单位：像素。不需要生效则设置最低值0即可",
        "----comment----min_area": "对象最小面积 单位：像素的平方。不需要生效则设置最低值0即可",
        "----comment----min_confidence": "对象最小质量 浮点型(0,1)。不需要生效则设置最低值0.0即可",
        "enable": 1, 
        "filt_mode": 0,
        "min_width": 0, 
        "min_height": 0, 
        "min_area": 0, 
        "min_confidence": 0.11 
    },
    "PushVehicleDetect": {
        "----comment----enable": "使能 1：开启 0：关闭。  默认1",
        "----comment----filt_mode": "过滤模式 0:filt by any one (所有条件皆超过min阈值才为有效目标，有任意一个小于min则过滤)  1:filt by all (有任意一个条件超过min阈值则为有效目标，所有条件皆小于min才过滤)   默认0",
        "----comment----min_width": "对象最小宽度 单位：像素。不需要生效则设置最低值0即可",
        "----comment----min_height": "对象最小高度 单位：像素。不需要生效则设置最低值0即可",
        "----comment----min_area": "对象最小面积 单位：像素的平方。不需要生效则设置最低值0即可",
        "----comment----min_confidence": "对象最小质量 浮点型(0,1)。不需要生效则设置最低值0.0即可",
        "enable": 1, 
        "filt_mode": 0,
        "min_width": 0, 
        "min_height": 0, 
        "min_area": 0, 
        "min_confidence": 0.11 
    },
    "AlgoParam": {
        "----comment----human": "人形目标相关 【要配置就一整组完整的配置】",
        "----comment----vehicle": "车形目标相关 【要配置就一整组完整的配置】",
        "human": {
            "----comment----threshold": "阈值(0.0,1.0)。 hisi算法有效范围(0.5,1.0)",
            "----comment----track_die_framenum": "多少帧检测不到目标了就认为这个目标的track die了",
            "threshold": 0.57,
            "track_die_framenum": 8
        },
        "vehicle": {
            "----comment----threshold": "阈值(0.0,1.0)。 hisi算法有效范围(0.5,1.0)",
            "----comment----track_die_framenum": "多少帧检测不到目标了就认为这个目标的track die了",
            "threshold": 0.57,
            "track_die_framenum": 8
        }
    }
    "ElectronicFence": {
            "enable": true,
            "sensitives": {
                "----comment----filt_mode": "过滤模式 0:filt by any one (所有条件皆超过min阈值才为有效目标，有任意一个小于min则过滤)  1:filt by all (有任意一个条件超过min阈值则为有效目标，所有条件皆小于min才过滤)   默认0",
                "----comment----min_width": "对象最小宽度 单位：像素。不需要生效则设置最低值0即可",
                "----comment----min_height": "对象最小高度 单位：像素。不需要生效则设置最低值0即可",
                "----comment----min_area": "对象最小面积 单位：像素的平方。不需要生效则设置最低值0即可",
                "----comment----min_confidence": "对象最小质量 浮点型(0,1)。不需要生效则设置最低值0.0即可",
                "filt_mode": 0,
                "min_width": 0, 
                "min_height": 0, 
                "min_area": 0, 
                "min_confidence": 0.11 
            },
            "alarmRules": [
                {
                    "----comment----careTarget": "多种目标可与  参考 E_AI_TARGET_TYPE",
                    "----comment----alarmMode": "0-按方向，1-按位置",
                    "----comment----direction": "alarmMode为按方向时有效；0-无效，1-双向，2-进入，3-离开",
                    "----comment----intrusionDuration": "入侵时长配置，入侵区域多少ms开始报警",
                    "careTarget": 67, 
                    "alarmMode": 1,
                    "direction": 0,
                    "intrusionDuration": 300
                }
            ],
            "alarm_interval":300,
            "active_sensitive_ms":	120000,
            "----comment----detectTarget": "多种目标可与  参考 E_AI_TARGET_TYPE  # [“people”,“car”,“...”],",
            "detectTarget": 67, 
            "----comment----enFiltActionlessTarget": "过滤静止目标 class_id按位与使能",
            "enFiltActionlessTarget": 1, 
            "times_die": 15,
            "polygon":{
            "num": 4,
            "points": [
                [0,0],
                [1000,0],
                [1000,500],
                [0,500]
                ]
            }
    },
    "CrossBoardDetect":     {
            "enable":       "false"
    },
    "OffDutyDetect":        {
            "enable":       "false"
    },
    "PassengerFlow":        {
            "enable":       "false"
    },
    "PassengerFlowLine":        {
            "enable":       "false"
    }
}
*/

/*
{
  "ElectronicFence": {
        "----comment----sensitives": "灵敏度",
        "----comment----alarmRules": "报警模规则，可配置多个（目前最多6个）",
        "----comment----alarm_interval": "连续报警的事件上报时间间隔。 单位ms，0则每帧都报。 可不配置则默认5000",
        "----comment----active_sensitive_ms": "事件的中断忽略值 单位：ms。  例如值500，2次触发中间中断<500ms则忽略中断，认为这2次触发为一次连续触发。 可不配置则默认500",
        "----comment----detectTarget": "多种目标可与  参考 E_AI_TARGET_TYPE  # [“people”,“car”,“...”],",
        "----comment----enFiltActionlessTarget": "过滤静止目标 class_id按位与使能  <人形 0:person,车型 1:car,2:bus,3:truck,4:cycle 5:pet ，事件 6:跌倒 >",
        "----comment----ToActionlessAlarmDuration": "运动变为静止状态的目标还能持续触发报警的时长 单位s。 0则不限制（可一直持续触发）。 默认0",
        "----comment----times_die": "丢失多少帧判断为track丢失（非主动die时才有效）",
        "enable": true,
        "sensitives": {
            "----comment----filt_mode": "过滤模式 0:filt by any one (所有条件皆超过min阈值才为有效目标，有任意一个小于min则过滤)  1:filt by all (有任意一个条件超过min阈值则为有效目标，所有条件皆小于min才过滤)   默认0",
            "----comment----min_width": "对象最小宽度 单位：像素。不需要生效则设置最低值0即可",
            "----comment----min_height": "对象最小高度 单位：像素。不需要生效则设置最低值0即可",
            "----comment----min_area": "对象最小面积 单位：像素的平方。不需要生效则设置最低值0即可",
            "----comment----min_confidence": "对象最小质量 浮点型(0,1)。不需要生效则设置最低值0.0即可",
            "filt_mode": 0,
            "min_width": 0, 
            "min_height": 0, 
            "min_area": 0, 
            "min_confidence": 0.11 
        },
        "alarmRules": [
            {
                "----comment----careTarget": "多种目标可与  参考 E_AI_TARGET_TYPE",
                "----comment----alarmMode": "0-按方向，1-按位置",
                "----comment----direction": "alarmMode为按方向时有效；0-无效，1-双向，2-进入，3-离开",
                "----comment----intrusionDuration": "入侵时长配置，入侵区域多少ms开始报警",
                "careTarget": 1, 
                "alarmMode": 1,
                "direction": 0,
                "intrusionDuration": 300
            },
            {
                "----comment----careTarget": "多种目标可与  参考 E_AI_TARGET_TYPE",
                "----comment----alarmMode": "0-按方向，1-按位置",
                "----comment----direction": "alarmMode为按方向时有效；0-无效，1-双向，2-进入，3-离开",
                "----comment----intrusionDuration": "入侵时长配置，入侵区域多少ms开始报警",
                "careTarget": 67, 
                "alarmMode": 0,
                "direction": 1,
                "intrusionDuration": 100
            }
        ],
        "alarm_interval":300,
        "active_sensitive_ms":	120000,
        "detectTarget": 67,
        "enFiltActionlessTarget": 1, 
        "ToActionlessAlarmDuration": 0, 
        "times_die": 15,
        "polygon":{
          "----comment----points": "以送检尺寸1：1的坐标",
          "num": 4,
          "points": [
            [0,0],
            [1000,0],
            [1000,500],
            [0,500]
            ]
        },
        "en_bg_pic":    false
  },
  "CrossBoardDetect": {
        "enable": "false",
        "sensitives": {
            "----comment----filt_mode": "过滤模式 0:filt by any one (所有条件皆超过min阈值才为有效目标，有任意一个小于min则过滤)  1:filt by all (有任意一个条件超过min阈值则为有效目标，所有条件皆小于min才过滤)   默认0",
            "----comment----min_width": "对象最小宽度 单位：像素。不需要生效则设置最低值0即可",
            "----comment----min_height": "对象最小高度 单位：像素。不需要生效则设置最低值0即可",
            "----comment----min_area": "对象最小面积 单位：像素的平方。不需要生效则设置最低值0即可",
            "----comment----min_confidence": "对象最小质量 浮点型(0,1)。不需要生效则设置最低值0.0即可",
            "filt_mode": 0,
            "min_width": 0, 
            "min_height": 0, 
            "min_area": 0, 
            "min_confidence": 0.11 
        },
        "orientation": 2,
        "detectTarget": 3,              ## 多种目标可与  参考 E_AI_TARGET_TYPE  # ["people","car","..."],
        "enFiltActionlessTarget": 1,            ## 过滤静止目标 class_id按位与使能
        "times_die": 15,
        "line": [
            [0,300],
            [800,300]
            ],
        "en_bg_pic":    false
  },
  "OffDutyDetect": {
        "enable": "false",
        "sensitives": {
            "----comment----filt_mode": "过滤模式 0:filt by any one (所有条件皆超过min阈值才为有效目标，有任意一个小于min则过滤)  1:filt by all (有任意一个条件超过min阈值则为有效目标，所有条件皆小于min才过滤)   默认0",
            "----comment----min_width": "对象最小宽度 单位：像素。不需要生效则设置最低值0即可",
            "----comment----min_height": "对象最小高度 单位：像素。不需要生效则设置最低值0即可",
            "----comment----min_area": "对象最小面积 单位：像素的平方。不需要生效则设置最低值0即可",
            "----comment----min_confidence": "对象最小质量 浮点型(0,1)。不需要生效则设置最低值0.0即可",
            "filt_mode": 0,
            "min_width": 0, 
            "min_height": 0, 
            "min_area": 0, 
            "min_confidence": 0.11 
        },
        "offDutyTime": 300,
        "enFiltActionlessTarget": 1,            ## 过滤静止目标 class_id按位与使能
        "times_die": 15,
        "polygon":{
          "num": 4,
          "points": [
            [0,0],
            [1000,0],
            [1000,500],
            [0,500]
            ]
        },
        "OnDutyPeople":1,
        "en_bg_pic":    false
  },
  "PassengerFlow": {
        "enable": "false",
        "census_delay": 60,
        "sensitives": {
            "----comment----filt_mode": "过滤模式 0:filt by any one (所有条件皆超过min阈值才为有效目标，有任意一个小于min则过滤)  1:filt by all (有任意一个条件超过min阈值则为有效目标，所有条件皆小于min才过滤)   默认0",
            "----comment----min_width": "对象最小宽度 单位：像素。不需要生效则设置最低值0即可",
            "----comment----min_height": "对象最小高度 单位：像素。不需要生效则设置最低值0即可",
            "----comment----min_area": "对象最小面积 单位：像素的平方。不需要生效则设置最低值0即可",
            "----comment----min_confidence": "对象最小质量 浮点型(0,1)。不需要生效则设置最低值0.0即可",
            "filt_mode": 0,
            "min_width": 0, 
            "min_height": 0, 
            "min_area": 0, 
            "min_confidence": 0.11 
        },
        "detectTarget": 1,              ## 多种目标可与  参考 E_AI_TARGET_TYPE  # ["people","car","..."],
        "enFiltActionlessTarget": 1,            ## 过滤静止目标 class_id按位与使能
        "----comment----uDecisionPointOfTargetTopPercent": "目标判定点为中线的上百分之多少 [1,100)  可不配置 默认25",
        "uDecisionPointOfTargetTopPercent": 25, 
        "times_die": 15,
        "polygon":{
          "num": 4,
          "points": [
            [0,0],
            [1000,0],
            [1000,1000],
            [0,1000]
            ]
        },
        "en_bg_pic":    false
  },
  "PassengerFlowLine": {
        "----comment----enable": "使能",
        "----comment----census_delay_mode": "周期统计的模式： 0-相对时间周期（已功能启动时间计时）； 1-墙上时间整点半小时周期（以现实时间每半小时为周期，例如1点半、2点、2点半上报统计）",
        "----comment----census_delay": "相对时间周期模式时有效，单位s",
        "----comment----sensitives": "灵敏度配置",
        "----comment----detectTarget": "多种目标可与  参考 E_AI_TARGET_TYPE",
        "----comment----enFiltActionlessTarget": "过滤静止目标 class_id按位与使能",
        "----comment----uDecisionPointOfTargetTopPercent": "目标判定点为中线的上百分之多少 [1,100)  可不配置 默认25",
        "----comment----direction": "拌线方向，以line的点0->点1为矢量。可选： line_L2R_in（目标从线的左到右为进入）/line_R2L_in（目标从线的右到左为进入）",
        "enable": "false",
        "census_delay_mode": 0,
        "census_delay": 60,
        "sensitives": {
            "----comment----filt_mode": "过滤模式 0:filt by any one (所有条件皆超过min阈值才为有效目标，有任意一个小于min则过滤)  1:filt by all (有任意一个条件超过min阈值则为有效目标，所有条件皆小于min才过滤)   默认0",
            "----comment----min_width": "对象最小宽度 单位：像素。不需要生效则设置最低值0即可",
            "----comment----min_height": "对象最小高度 单位：像素。不需要生效则设置最低值0即可",
            "----comment----min_area": "对象最小面积 单位：像素的平方。不需要生效则设置最低值0即可",
            "----comment----min_confidence": "对象最小质量 浮点型(0,1)。不需要生效则设置最低值0.0即可",
            "filt_mode": 0,
            "min_width": 0, 
            "min_height": 0, 
            "min_area": 0, 
            "min_confidence": 0.11 
        },
        "detectTarget": 1, 
        "enFiltActionlessTarget": 1, 
        "uDecisionPointOfTargetTopPercent": 25, 
        "times_die": 15,
        "polygon":{
          "num": 4,
          "points": [
            [0,0],
            [1000,0],
            [1000,1000],
            [0,1000]
            ]
        },
        "direction":"line_R2L_in",
        "line":{
            "points": [
              [0,0],
              [1000,0]
            ]
        },
        "en_bg_pic":    false
  },
  "GatheringFlow": {
        "enable": "false",
        "cycle_time": 300,
        "polygon":{
          "num": 4,
          "points": [
            [0,0],
            [1000,0],
            [1000,1000],
            [0,1000]
            ]
        },
        "alarm_num":30,
        "alarm_interval":300
  }

}
*/

#define  ___AISDK_JSON_CFG_FLAME_SMOG___
/*
"scene_flame_smog":	{
    "----comment----ElectronicFence": "围栏配置| 视围栏enable为总开关，关闭后不会送检",
    "ElectronicFence":	{
        "enable":	false,
        "sensitive":	2,
        "alarmRules": [
            {
                "----comment----careTarget": "多种目标可与  参考 E_AI_TARGET_TYPE",
                "----comment----alarmMode": "0-按方向，1-按位置",
                "----comment----direction": "alarmMode为按方向时有效；0-无效，1-双向，2-进入，3-离开",
                "----comment----intrusionDuration": "入侵时长配置，入侵区域多少ms开始报警",
                "careTarget": 384, 
                "alarmMode": 1,
                "direction": 2,
                "intrusionDuration": 0
            }
        ],
        "alarm_interval":	120000,
        "active_sensitive_ms":	120000,
        "----comment----detectTarget": "多种目标可与  参考 E_AI_TARGET_TYPE  # [\"flame\"],",
        "detectTarget":	384,
        "times_die":	15,
        "polygon":	{
            "num":	4,
            "pic_width":	1280,
            "pic_height":	720,
            "points":	[[0, 0], [1280, 0], [1280, 720], [0, 720]]
        }
    },
    "----comment----flame_alarm": "火焰报警相关配置",
    "flame_alarm": {
        "----comment----sensitivity": "灵敏度[0-100] 0:关闭",
        "sensitivity": 100,
        "----comment----duration": "报警触发需要的持续时间 单位：秒",
        "duration": 1,
        "alertor_configs":	{
            "----comment----cooldown_duration": "KS参数（调试）- 报警间隔时间 单位：秒",
            "cooldown_duration": 3
        }
    },
    "----comment----smog_alarm": "烟雾报警相关配置",
    "smog_alarm": {
        "----comment----sensitivity": "灵敏度[0-100] 0:关闭",
        "sensitivity": 50,
        "----comment----duration": "报警触发需要的持续时间 单位：秒",
        "duration": 1,
        "alertor_configs":	{
            "----comment----cooldown_duration": "KS参数（调试）- 报警间隔时间 单位：秒",
            "cooldown_duration": 3
        }
    }
}
*/






/*************************************
 * 人脸比对功能相关定义 ****************
**************************************/

/*人脸组结构体定义*/
typedef struct {
uint64_t groupid;/*人脸组标识符，唯一主键，自增型，用于底库使用*/
char uuid[64];/*人脸组唯一标识符，全局唯一，使用uuid（16Byte）生成，转换成字符串存储*/
char name[32];/*人脸组名称，32个字节的字符串*/
float threshold;/*阈值，浮点数 [0-100]*/
uint64_t version;/*版本号，8个字节的整数*/
int status;/*状态值，0-正常，1-删除*/
int nValidPersonCount;		//有效人数
int nInvalidPersonCount;	//无效人数
int nPersonInfoCount;		//作为输出参数,脸库人员信息个数
}AISDK_ST_FACERECO_GROUP;

/*人脸子组结构体定义*/
typedef struct {
char uuid[64];/*人脸子组唯一标识符，全局唯一，使用uuid（16Byte）生成，转换成字符串存储*/
char name[64];/*人脸子组名称，32个字节的字符串*/
uint64_t version;/*版本号，8个字节的整数*/
int status;/*状态值，0-正常，1-删除*/
char l1uuid[64];/*第0层uuid，人脸组表的uuid*/
char l2uuid[64];/*第1层uuid，人脸子组表的uuid*/
char l3uuid[64];/*第2层uuid，人脸子组表的uuid*/
char l4uuid[64];/*第3层uuid，人脸子组表的uuid*/
char l5uuid[64];/*第4层uuid，人脸子组表的uuid*/
char l6uuid[64];/*第5层uuid，人脸子组表的uuid*/
char l7uuid[64];/*第6层uuid，人脸子组表的uuid*/
int  memo_int1;/*备注-整型1   应用自定义使用*/
char memo[124];/*备注信息，用于扩展   【备用-未使用】*/
}AISDK_ST_FACERECO_GROUP_SUB;

/*人脸信息表结构*/
typedef struct {
uint64_t faceId;/*人脸信息标识符，唯一主键，自增型，用于底库使用*/
char uuid[64];/*人脸信息唯一标识符，全局唯一，使用uuid（16Byte）生成，转换成字符串存储*/
char name[32];/*人脸信息名称，32个字节的字符串*/
int gender;/*性别，0-男，1-女，2-未知*/
char phone_number[20];/*手机号*/
int certificates_type;/*证件类型，0-身份证，1-护照，2-其他*/
char certificates_id[20];/*证件号*/
int  nModelState;		//作为输出参数，建模状态  -1：无效 
char feature[2048];/*特征值*/
char feature_version[8];/*特征值版本*/
char group_uuid[64];/*人脸组唯一标识符，全局唯一，使用uuid（16Byte）生成，转换成字符串存储*/
char subgroup_uuid[64];/*人脸子组唯一标识符，全局唯一，使用uuid（16Byte）生成，转换成字符串存储*/
char pic_face_pathname[128];/*人脸底图jpg文件的pathname*/
uint64_t version;/*版本号，8个字节的整数*/
uint64_t update_time;/*人脸信息更新时间戳[使用者管理]，8个字节的整数*/
int status;/*状态值，0-更新，1-删除*/
uint64_t faceGroupId;/*人脸组标识符*/
char memo[120];/*备注信息，用于扩展*/
}AISDK_ST_FACERECO_FACEINFO;

/*人脸组结构体定义*/
typedef struct {
uint64_t groupid;/*人脸组标识符，唯一主键，自增型，用于底库使用*/
char uuid[64];/*人脸组唯一标识符，全局唯一，使用uuid（16Byte）生成，转换成字符串存储*/
char name[32];/*人脸组名称，32个字节的字符串*/
float threshold;/*阈值，浮点数 [0-100]*/
uint64_t version;/*版本号，8个字节的整数*/
int status;/*状态值，0-正常，1-删除*/
int nValidPersonCount;		//有效人数
int nInvalidPersonCount;	//无效人数
int nPersonInfoCount;		//作为输出参数,脸库人员信息个数
}AISDK_ST_FACERECO_GROUPINFO;

typedef struct{
char uuid[64];/*人脸子组唯一标识符，全局唯一，使用uuid（16Byte）生成，转换成字符串存储*/
char name[64];/*人脸子组名称，32个字节的字符串*/
uint64_t version;/*版本号，8个字节的整数*/
int status;/*状态值，0-正常，1-删除*/
char l1uuid[64];/*第0层uuid，人脸组表的uuid*/
char l2uuid[64];/*第1层uuid，人脸子组表的uuid*/
char l3uuid[64];/*第2层uuid，人脸子组表的uuid*/
char l4uuid[64];/*第3层uuid，人脸子组表的uuid*/
char l5uuid[64];/*第4层uuid，人脸子组表的uuid*/
char l6uuid[64];/*第5层uuid，人脸子组表的uuid*/
char l7uuid[64];/*第6层uuid，人脸子组表的uuid*/
char memo[128];/*备注信息，用于扩展*/
}AISDK_ST_FACERECO_SUBGROUPINFO;

//jpg人脸检测结果结构体
typedef struct
{
    char feature_version[8];/*特征值版本*/
    char feature[2048];/*特征值*/
    int  feature_len;/*特征值长度*/
    S_RECT rect;        /*人脸位置 【暂无效】*/
    char **p_crop_jpg_buf;/*人脸的jpg数据。可为NULL 则不输出。  外部指针接收，内部申请，外部自行按需释放*/
    int  crop_jpg_len;/*人脸的jpg数据的长度*/
}AISDK_ST_FACERECO_JPG_RET;

typedef enum
{
	FACEINFO_QUERY_ORDER_NOT = 0,			    //不排序
	FACEINFO_QUERY_ORDER_UPDATETIME_ASC,	//按更新时间排序 升序 时间值小->大
    FACEINFO_QUERY_ORDER_UPDATETIME_DESC,	//按更新时间排序 降序 时间值大->小
}E_AISDK_FACE_INFO_QUERY_ORDER_MODE;





//人脸抓拍配置 
typedef struct 
{
    uint32_t nEnable;      	// 抓拍使能
    uint32_t nCapMode;     	// 抓拍模式：0-质量；1-快速
    uint32_t nFaceQuality;  // 人脸质量（五个档位）[20,40,60,80,100]
    uint32_t nIsCapBack;   	// 是否抓拍背景图：0-关闭；1-启用
}AISDK_DEF_S_SNAPSHOT_CFG;

typedef struct 
{
    uint32_t nEnable;      	// 口罩检测使能,0:关闭,1:启用
    uint32_t nAlarmType;    // 口罩检测报警类型,0:无口罩报警,1:有口罩报警
}AISDK_DEF_S_MASK_DETECT_CFG;

typedef struct 
{
    uint32_t nEnable;      	// 人脸识别使能,0:关闭,1:启用
}AISDK_DEF_S_FACERECO_CFG;


#if 1

#define	MOTION_DETECT       					"MotionDetect"
#define	MOTION_DETECT_ENABLE       				"enable"
#define	MOTION_DETECT_SENSITIVE   				"sensitive"
#define	MOTION_DETECT_ALARM_INTERVAL			"alarm_interval"
#define	MOTION_DETECT_ACTIVE_SENSITIVE          "active_sensitive_ms"
#define	MOTION_DETECT_ERGION_MODE     			"region_mode"
#define	MOTION_DETECT_BIT_REGION     			"bit_region"
#define	MOTION_DETECT_POLYGON					"polygon"
#define	MOTION_DETECT_POLYGON_NUM				"num"
#define	MOTION_DETECT_POLYGON_POINTS			"points"
#define	MOTION_DETECT_POLYGON_PIC_W 			"pic_width"
#define	MOTION_DETECT_POLYGON_PIC_H 			"pic_height"

#define HUMAN_DETECT_TARGET_TRACK_MODE          "TargetTrackMode"
#define HUMAN_DETECT_START_PARAM                "start_param"
#define HUMAN_DETECT_START_PARAM_AICHN_NUM      "AiChnNum"
#define HUMAN_DETECT_QPS                        "QPS"
#define HUMAN_DETECT_EN_FILT_ACTIONLESS         "enFiltActionlessTarget"
#define HUMAN_DETECT_FILT_ACTIONLESS_CLEAR_REC_WHEN_ENABLE "ClearRecWhenEnable"
#define HUMAN_DETECT_FILT_ACTIONLESS_TARGET_DISTANCELEN    "FiltActionlessTargetDistancelenRatio"
#define	HUMAN_DETECT_FILT_ACTIONLESS_TARGET_ZOOM_W         "FiltActionlessTargetZoomWidthRatio"
#define	HUMAN_DETECT_FILT_ACTIONLESS_TARGET_ZOOM_H         "FiltActionlessTargetZoomHeightRatio"
#define	HUMAN_DETECT_FILT_ACTIONLESS_PARAM_DISTANCELEN_AVALUE    "FiltActionlessTargetDistancelen"
#define	HUMAN_DETECT_FILT_ACTIONLESS_PARAM_ZOOM_W_AVALUE         "FiltActionlessTargetZoomWidth"
#define	HUMAN_DETECT_FILT_ACTIONLESS_PARAM_ZOOM_H_AVALUE         "FiltActionlessTargetZoomHeight"
#define	HUMAN_DETECT_FILT_ACTIONLESS_FASTACTIONLESS        "FastActionlessOfRegion"
#define	HUMAN_DETECT_FILT_ACTIONLESS_FASTACTIONLESS_RECLIST_NUMTOTAL "RecListNumTotal"
#define	HUMAN_DETECT_FILT_ACTIONLESS_FASTACTIONLESS_RECLIST_TIMELEN  "RecListTimeLen"
#define HUMAN_DETECT_PUSH_HUMAN_DETECT          "PushHumanDetect"
#define HUMAN_DETECT_PUSH_VEHICLE_DETECT        "PushVehicleDetect"
#define HUMAN_DETECT_PUSH_DETECT_EN             "enable"
#define HUMAN_DETECT_PUSH_DETECT_FILT_MODE      "filt_mode"
#define HUMAN_DETECT_PUSH_DETECT_MIN_WIDTH      "min_width"
#define HUMAN_DETECT_PUSH_DETECT_MIN_HEIGHT     "min_height"
#define HUMAN_DETECT_PUSH_DETECT_MIN_AREA       "min_area"
#define HUMAN_DETECT_PUSH_DETECT_MIN_CONFIDENCE "min_confidence"
#define HUMAN_DETECT_ALGO_PARAM                 "AlgoParam"
#define HUMAN_DETECT_ALGO_PARAM_HUMAN           "human"
#define HUMAN_DETECT_ALGO_PARAM_HUMAN_THRESHOLD "threshold"
#define HUMAN_DETECT_ALGO_PARAM_HUMAN_TRACK_DIE_FRAMENUM "track_die_framenum"
#define HUMAN_DETECT_ALGO_PARAM_VEHICLE           "vehicle"
#define HUMAN_DETECT_ALGO_PARAM_VEHICLE_THRESHOLD "threshold"
#define HUMAN_DETECT_ALGO_PARAM_VEHICLE_TRACK_DIE_FRAMENUM "track_die_framenum"

#define	ELECTRONIC_FENCE                        "ElectronicFence"
#define	ELECTRONIC_FENCE_ENABLE                 "enable"
#define	ELECTRONIC_FENCE_SENSITIVES             "sensitives"
#define ELECTRONIC_FENCE_SENSITIVE_FILT_MODE    "filt_mode"
#define	ELECTRONIC_FENCE_SENSITIVE_MIN_WIDTH    "min_width"
#define	ELECTRONIC_FENCE_SENSITIVE_MIN_HEIGHT   "min_height"
#define ELECTRONIC_FENCE_SENSITIVE_MIN_AREA     "min_area"
#define	ELECTRONIC_FENCE_SENSITIVE_MIN_CONFIDENCE "min_confidence"
#define	ELECTRONIC_FENCE_ALARM_RULES            "alarmRules"
#define	ELECTRONIC_FENCE_ALARM_RULES_CARE_TARGET "careTarget"
#define	ELECTRONIC_FENCE_ALARM_RULES_ALARM_MODE "alarmMode"
#define	ELECTRONIC_FENCE_ALARM_RULES_DIRECTION  "direction"
#define	ELECTRONIC_FENCE_ALARM_RULES_INTRUSION_DURATION "intrusionDuration"
#define	ELECTRONIC_FENCE_ALARM_INTERVAL         "alarm_interval"
#define	ELECTRONIC_FENCE_ACTIVE_SENSITIVE       "active_sensitive_ms"
#define	ELECTRONIC_FENCE_DETECT_TARGET          "detectTarget"
#define ELECTRONIC_FENCE_EN_FILT_ACTIONLESS     "enFiltActionlessTarget"
#define ELECTRONIC_FENCE_TOACTIONLESS_ALARM_DURATION "ToActionlessAlarmDuration"
#define	ELECTRONIC_FENCE_TIME_DIE       		"times_die"
#define	ELECTRONIC_FENCE_POLYGON                "polygon"
#define	ELECTRONIC_FENCE_POLYGON_NUM            "num"
#define	ELECTRONIC_FENCE_POLYGON_POINTS         "points"
#define	ELECTRONIC_FENCE_EN_BG_PIC              "en_bg_pic"

#define	CROSS_BOARD_DETECT       				"CrossBoardDetect"
#define	CROSS_BOARD_DETECT_ENABLE       		"enable"
#define	CROSS_BOARD_DETECT_SENSITIVES   		"sensitives"
#define CROSS_BOARD_DETECT_SENSITIVE_FILT_MODE  "filt_mode"
#define	CROSS_BOARD_DETECT_SENSITIVE_MIN_WIDTH  "min_width"
#define	CROSS_BOARD_DETECT_SENSITIVE_MIN_HEIGHT "min_height"
#define CROSS_BOARD_DETECT_SENSITIVE_MIN_AREA   "min_area"
#define	CROSS_BOARD_DETECT_SENSITIVE_MIN_CONFIDENCE "min_confidence"
#define	CROSS_BOARD_DETECT_ORIENTATION       	"orientation"
#define	CROSS_BOARD_DETECT_DETECT_TARGET     	"detectTarget"
#define CROSS_BOARD_EN_FILT_ACTIONLESS          "enFiltActionlessTarget"
#define	CROSS_BOARD_DETECT_TIME_DIE       		"times_die"
#define	CROSS_BOARD_DETECT_LINE      			"line"
#define	CROSS_BOARD_DETECT_EN_BG_PIC            "en_bg_pic"

#define	OFF_DUTY_DETECT       					"OffDutyDetect"
#define	OFF_DUTY_DETECT_ENABLE       			"enable"
#define	OFF_DUTY_DETECT_SENSITIVES   			"sensitives"
#define OFF_DUTY_DETECT_SENSITIVE_FILT_MODE    "filt_mode"
#define	OFF_DUTY_DETECT_SENSITIVE_MIN_WIDTH    "min_width"
#define	OFF_DUTY_DETECT_SENSITIVE_MIN_HEIGHT   "min_height"
#define OFF_DUTY_DETECT_SENSITIVE_MIN_AREA     "min_area"
#define	OFF_DUTY_DETECT_SENSITIVE_MIN_CONFIDENCE "min_confidence"
#define	OFF_DUTY_DETECT_OFF_DUTY_TIME     		"offDutyTime"
#define	OFF_DUTY_DETECT_DETECT_TARGET           "detectTarget"
#define OFF_DUTY_DETECT_EN_FILT_ACTIONLESS      "enFiltActionlessTarget"
#define	OFF_DUTY_DETECT_TIME_DIE       			"times_die"
#define	OFF_DUTY_DETECT_POLYGON					"polygon"
#define	OFF_DUTY_DETECT_POLYGON_NUM				"num"
#define	OFF_DUTY_DETECT_POLYGON_POINTS			"points"
#define	OFF_DUTY_DETECT_EN_BG_PIC               "en_bg_pic"

#define	PASSENGER_FLOW       					"PassengerFlow"
#define	PASSENGER_FLOW_ENABLE       			"enable"
#define	PASSENGER_FLOW_CENSUS_DELAY       		"census_delay"
#define	PASSENGER_FLOW_SENSITIVES   			"sensitives"
#define PASSENGER_FLOW_SENSITIVE_FILT_MODE      "filt_mode"
#define	PASSENGER_FLOW_SENSITIVE_MIN_WIDTH      "min_width"
#define	PASSENGER_FLOW_SENSITIVE_MIN_HEIGHT     "min_height"
#define PASSENGER_FLOW_SENSITIVE_MIN_AREA       "min_area"
#define	PASSENGER_FLOW_SENSITIVE_MIN_CONFIDENCE "min_confidence"
#define PASSENGER_FLOW_EN_FILT_ACTIONLESS       "enFiltActionlessTarget"
#define PASSENGER_FLOW_DECISION_POINT_OF_TARGET_TOP_PERCENT "uDecisionPointOfTargetTopPercent"
#define	PASSENGER_FLOW_TIME_DIE       			"times_die"
#define	PASSENGER_FLOW_POLYGON					"polygon"
#define	PASSENGER_FLOW_POLYGON_NUM				"num"
#define	PASSENGER_FLOW_POLYGON_POINTS			"points"
#define	PASSENGER_FLOW_EN_BG_PIC                "en_bg_pic"

#define	PASSENGER_FLOW_LINE       				"PassengerFlowLine"
#define	PASSENGER_FLOW_LINE_ENABLE       		"enable"
#define	PASSENGER_FLOW_LINE_SENSITIVES   			"sensitives"
#define PASSENGER_FLOW_LINE_SENSITIVE_FILT_MODE    "filt_mode"
#define	PASSENGER_FLOW_LINE_SENSITIVE_MIN_WIDTH      "min_width"
#define	PASSENGER_FLOW_LINE_SENSITIVE_MIN_HEIGHT     "min_height"
#define PASSENGER_FLOW_LINE_SENSITIVE_MIN_AREA     "min_area"
#define	PASSENGER_FLOW_LINE_SENSITIVE_MIN_CONFIDENCE "min_confidence"
#define	PASSENGER_FLOW_LINE_CENSUS_DELAY_MODE      	 "census_delay_mode"
#define	PASSENGER_FLOW_LINE_CENSUS_DELAY       	"census_delay"
#define	PASSENGER_FLOW_LINE_TIME       		    "time"
#define	PASSENGER_FLOW_LINE_TIME_DIE       		"times_die"
#define PASSENGER_FLOW_LINE_EN_FILT_ACTIONLESS  "enFiltActionlessTarget"
#define PASSENGER_FLOW_LINE_DECISION_POINT_OF_TARGET_TOP_PERCENT "uDecisionPointOfTargetTopPercent"
#define	PASSENGER_FLOW_LINE_POLYGON				"polygon"
#define	PASSENGER_FLOW_LINE_POLYGON_NUM			"num"
#define	PASSENGER_FLOW_LINE_POLYGON_POINTS		"points"
#define	PASSENGER_FLOW_LINE_DIRECTION			"direction"
#define	PASSENGER_FLOW_LINE_LINE				"line"
#define	PASSENGER_FLOW_LINE_LINE_POINTS			"points"
#define	PASSENGER_FLOW_LINE_DIRECTION_L2R_IN    "line_L2R_in"
#define	PASSENGER_FLOW_LINE_DIRECTION_R2L_IN    "line_R2L_in"
#define	PASSENGER_FLOW_LINE_EN_BG_PIC           "en_bg_pic"


#define ENABLE_TRUE   		"true"
#define ENABLE_FALSE   		"false"

typedef struct
{
    uint8_t enable;   /* 是否使能移动侦测 */
    uint8_t sen;      /* 侦测灵敏度(1-5)  VDA_SENSI_LEVEL_E */
    uint8_t MbCnt[2]; /* MB个数 wcnt每行块数, hcnt每列块数 */
    uint32_t Blk[18];    /* 32*32为单位,最大(704*576)共22*18,每位代表一个单位是否有效 */
} AISDK_VDA_MD_PARAM_S;

typedef struct
{
    uint16_t BlkNum;              /* 移动块个数 */
    uint8_t MArea[50]; /* 移动区块位置标识 */
} AISDK_VDA_MD_AREADATA_S;


typedef struct
{
    uint8_t TgId;     /* 芯片目标ID(TGID_E) */
    uint8_t ChipNum;  /* 芯片个数(CHIP_NUM_E) */
    uint8_t Resvd[2]; /* 保留位 */
} AISDK_VIDEO_CHIP_CONF_S;

typedef struct
{
    uint8_t vmode;        /* 视频制式(VI_MODE_E) */
    uint8_t MaxVdaChn;    /* VDA最大通道数 */
    uint8_t Resvd[2];     /* 保留位 */
} AISDK_VDA_CONF_S;

typedef struct
{
    uint8_t VdaChnIdx;    /* vda通道号 */
    uint8_t VdaSrcChn;    /* 源设备设备号 */
    uint8_t Rotation;     /* ROTATION_E */ 
    uint8_t CropRatioH;    /* VPSS通道水平裁减比例 */
    uint8_t CropRatioV;    /* VPSS通道垂直裁减比例 */
    uint8_t Resvd[3];
    uint16_t  Width;
    uint16_t  Height;
} AISDK_VDA_CHN_ATTR_S;

typedef struct
{
    uint16_t x;   /* X轴坐标 */
    uint16_t y;   /* Y轴坐标 */
    uint16_t w;   /* X轴宽度 */
    uint16_t h;   /* Y轴高度 */
} AISDK_VRECT_S;


/*mosaic信息结构体*/
typedef struct
{
	uint8_t BlkSize;			/*马赛克块大小*/
    uint8_t RectCnt;  /* 马赛克区域个数*/
	AISDK_VRECT_S Rect[4];	/*马赛克区域*/
} AISDK_VMOSAIC_REGION_S;


#endif


















#endif   // __AISDK_STRUCT_H__
