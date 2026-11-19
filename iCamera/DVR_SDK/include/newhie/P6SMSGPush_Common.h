/*!
* @file
*  P6SMSGPush_Common.h
* \brief
*  客户端SDK接口结构与类型定义
* @author
*  zw
* @date
*  2017年07月29日
* @version
*  1.2.0
*/

#ifndef __P6S_MSG_PUSH_COMMON_H__

#define __P6S_MSG_PUSH_COMMON_H__

// 参数属性
/** \brief 入参																						*/
#define IN

/** \brief 出参																						*/
#define OUT

/** \brief 入出参																					*/
#define IN_OUT

/** \brief 通用数据类型																				*/

typedef unsigned int			    ZWP2P_UINT32;
typedef int							ZWP2P_INT32;
typedef unsigned short				ZWP2P_UINT16;
typedef unsigned char				ZWP2P_UCHAR;
typedef char						ZWP2P_CHAR;


#if defined(_WIN32)
#ifndef SYSTEM_WIN32
#define SYSTEM_WIN32
#endif
#elif defined(_LINUX)
#define SYSTEM_LINUX
#else
#error "undefined system!"
#endif

/** \brief 通用数据类型																				*/
#ifndef CALLBACK
#if defined(_LINUX)
#define CALLBACK
#else
#define CALLBACK					__stdcall
#endif
#endif

#ifndef HANDLE
typedef void*						HANDLE;
#endif


/// 错误码定义
typedef enum tageP6SMSG_ERR_CODE
{
	eP6SMSG_PUSH_ERR_SUCCESS = 0,						// 成功
	eP6SMSG_PUSH_ERR_FAILED,								// 失败

	eP6SMSG_PUSH_ERR_FAILED_SDK_NOT_INIT = 100,			// 客户端sdk未初始化
	eP6SMSG_PUSH_ERR_FAILED_SDK_ALREADY_INIT,			// 客户端以及初始化
	eP6SMSG_PUSH_ERR_FAILED_UNSUPPORTED,                 // 不支持的操作
	eP6SMSG_PUSH_ERR_FAILED_UNSUPPORT_PUSH_DEVICE,       // 不支持推送的设备
	eP6SMSG_PUSH_ERR_FAILED_START_PUSH_THREAD,			// 启动推送线程失败
	eP6SMSG_PUSH_ERR_FAILED_BUFFER_FULL,					// 缓冲区满
	eP6SMSG_PUSH_ERR_FAILED_INVALID_PARAMETER,			// 参数无效
	eP6SMSG_PUSH_ERR_FAILED_INVALID_HANDLE,				// 句柄值无效
	eP6SMSG_PUSH_ERR_FAILED_NO_RESOURCE,					// 资源不足
	eP6SMSG_PUSH_ERR_FAILED_MEMORY_ERROR,				// 内存不足
	eP6SMSG_PUSH_ERR_FAILED_JSON_ERROR,					// JSON错误
	eP6SMSG_PUSH_ERR_FAILED_SERVER_NO_RESPONSE,			// 服务器未响应
	eP6SMSG_PUSH_ERR_FAILED_GET_UTCT,					// 获取UTCT时间错误
	eP6SMSG_PUSH_ERR_FAILED_GENERAL_SUBKEY,				// 生成subkey错误
	eP6SMSG_PUSH_ERR_FAILED_SET_SUBKEY,					// 设置subkey失败
	eP6SMSG_PUSH_ERR_FAILED_CLEAR_SUBKEY,				// 清除subkey失败
}eP6SMSG_ERR_CODE;



#define P6SMSG_PUSH_MAX_TITLE_LENGTH						32
#define P6SMSG_PUSH_MAX_DEVICE_NAME_LENGTH				40
#define P6SMSG_PUSH_MAX_CHANNEL_NAME_LENGTH				40
#define P6SMSG_PUSH_MAX_EVENT_CREATE_TIME_LENGTH	 		20
#define P6SMSG_PUSH_MAX_DID_LENGTH  						64
#define P6SMSG_PUSH_MAX_CUSTOM_CONTENT					80
#define P6SMSG_PUSH_MAX_CUSTOM_PAYLOAD					80
#define P6SMSG_PUSH_MAX_MAC_LEN							20
#define P6SMSG_PUSH_MAX_SUBKEY_DIR						256
#define P6SMSG_PUSH_MAX_EVENT_CREATE_TIME_LENGTH_V2	 	32
#define P6SMSG_PUSH_MAX_DOMAIN_LEN                       128
#define P6SMSG_PUSH_MAX_DOMAIN_LIST_NUM                  16



////////////////////////////////////////////////////////////////////////////
// 事件相关结构体，用于云消息中元数据


typedef struct tagP6SMSG_PUSH_DEVICE_INFO
{
    char          strOperator[64];                                    // 事件类型，当类型是电子围栏时，该字段填 ElectronicDefence; 当类型是越界侦测时，该字段填 CrossBorderDetect; 
                                                                      // 当类型是人脸抓拍时，该字段填 FaceSnapshot; 当类型是人脸识别时，该字段填 FaceReco; 
                                                                      // 当类型是车牌抓拍时，该字段填 CarLicenseSnapshot; 当类型是车牌布控时，该字段填 LicensePlateSurveillance;
                                                                      // 当类型是视频遮挡时，该字段填 VideoCoverEvent; 当类型是电瓶车时，该字段填 ElectricBikeEvent; 
                                                                      // 当类型是煤气罐时，该字段填 GasContainerEvent; 当类型是一键呼叫挂断时，该字段填 StopKey2Call; 
                                                                      // 当类型是电子围栏V2时，该字段填 ElectronicDefenceV2; 当类型是人数统计触发事件时，该字段填 PeopleNumberOverLimit
                                                                      // 当类型是人数统计事件（周期性上报）时，该字段填PeopleNumberStatistics; 当类型是长久离家时，该字段填LongAway
    char          strSerialNumber[64];                                // 设备序列号
    char          strPlatformID[32];                                  // 平台 ID
    int           nDeviceType;                                        // 设备类型； 0 - IPC， 1 - NVR

    char          cReserve[128];                                      //保留字段

}P6SMSG_PUSH_DEVICE_INFO, *LPP6SMSG_PUSH_DEVICE_INFO;

// 电子围栏
typedef struct tagP6SMSG_PUSH_ELECTRONIC_DEFENCE
{
    P6SMSG_PUSH_DEVICE_INFO     stDeviceInfo;                                       // 设备信息
    int                         nEventID;                                           // 事件 ID
    char                        strTime[32];                                        // 事件时间，格式为 20200331T101001+08
    char                        strDetectTarget[16];                                // 检测目标, "car"-车形, "person"-人形

    char                        cReserve[128];                                      //保留字段
}P6SMSG_PUSH_ELECTRONIC_DEFENCE, *LPP6SMSG_PUSH_ELECTRONIC_DEFENCE;


// 越界侦测
typedef struct tagP6SMSG_PUSH_CROSS_BORDER_DETECT
{
    P6SMSG_PUSH_DEVICE_INFO     stDeviceInfo;                                        // 设备信息
    int                         nEventID;                                            // 事件 ID
    char                        strTime[32];                                         // 事件时间，格式为 20200331T101001+08
    char                        strDetectTarget[16];                                 // 检测目标, "car"-车形, "person"-人形

    char                        cReserve[128];                                       //保留字段
}P6SMSG_PUSH_CROSS_BORDER_DETECT, *LPP6SMSG_PUSH_CROSS_BORDER_DETECT;


// 人脸抓拍事件结构体

/*!
*  @struct	tagP6SMSG_PUSH_FACE_FEATURE_INFO
*  @brief	人脸识别特征信息
*/
typedef struct tagP6SMSG_PUSH_FACE_FEATURE_INFO
{
    char        cAge;                                 /*!< 年龄：0-未知，其它表示年龄                */
    char        cSex;                                 /*!< 性别：0-未知，1-男性，2-女性            */
    char        cHat;                                 /*!< 帽子：0-未知，1-戴帽子，2-无帽子,3.安全帽,4.其他帽子       */
    char        cHair;                                /*!< 头发长度：0-未知，1-短发，2-普通，3-长发,4.光头，5.少量头发（包含秃顶）*/
    char        cHairColor;                           /*!< 头发颜色：0-未知，1-黑色，2-白色，3-金色，4-其它*/
    char        cEye;                                 /*!< 眼睛：0-未知，1-正常，2-闭眼，3-遮挡    */
    char        cGlasses;                             /*!< 眼镜：0-未知，1-戴眼镜，2-无眼镜,3.墨镜，4.其他眼镜        */
    char        cBeard;                               /*!< 胡须：0-未知，1-有胡须（嘴唇上的胡子），2-无胡须(无胡子或者胡子不明显) 3. 络腮胡       */
    char        cMask;                                /*!< 口罩：0-未知，1-有口罩，2-无口罩，3.捂住嘴巴，4.捂住口鼻        */
    char        cSkin;                                /*!< 肤色：0-未知，1-黄色，2-白色，3-黑色，4-棕色*/
    char        cSmile;                               /*!< 表情：0-未知，1-普通，2-微笑            */
    char        cHuman;                               /*!< 活体：0-未知，1-活体，2-假体            */
    char        cQuality;                             /*!< 清晰度：值域范围（0-100）                */
    float       fYaw;                                 /*!< 侧向角度                                */
    float       fRoll;                                /*!< 俯仰角度                                */
    float       fPitch;                               /*!< 斜脸角度                                */
    char        cReserve[64];                         /*!< 保留数据                                */
}P6SMSG_PUSH_FACE_FEATURE_INFO, *LPP6SMSG_PUSH_FACE_FEATURE_INFO;


// 人脸抓拍
typedef struct tagP6SMSG_PUSH_SNAP_SHOT
{
    P6SMSG_PUSH_DEVICE_INFO                   stDeviceInfo;                                        // 设备信息
    int                                       nEventID;                                            // 事件 ID
    char                                      strTime[32];                                         // 事件时间，格式为 20200331T101001+08
    char                                      strDirection[16];                                    // 出入口方向，取值enter：进入,exit：离开,unknown：未知方向
    int                                       nTrackID;                                            // 唯一标记某一张图里面的某一个人脸信息
    int                                       nFaceNumber;                                         // 图片中的人脸个数
    P6SMSG_PUSH_FACE_FEATURE_INFO             stFaceFeature;                                       // 人脸识别特征信息：描述检测到的目标特征信息

    char                        cReserve[128];                                       //保留字段
}P6SMSG_PUSH_SNAP_SHOT, *LPP6SMSG_PUSH_SNAP_SHOT;


// 人脸识别事件结构体
/*!
*  @struct	tagPersonInfo
*  @brief	人脸识别人员信息
*/
typedef struct tagP6SMSG_PUSH_PERSON_INFO_CONTENT
{
    char          strName[32];                                         // 姓名，最大字节长度32个字节
    char          strSex[8];                                           // 性别，取值female，male，unknown
    char          strCardType[16];                                     // 证件类型，取值IC:身份证，passport：护照，custom：自定义或其他类型
    char          strCardID[32];                                       // 证件ID，最大字节长度32个字节, 实际只支持19个字节
    char          strCountry[32];                                      // 国家，最大字节长度32个字节
    char          strAddress[64];                                      // 具体地址，最大字节长度64个字节，省，市，区，县之间使用“#”分割
    char          strBirthday[32];                                     // 生日，格式为 20200331T101001+08
    float         fMatch;                                              // 识别匹配度，取值：0-100
    int           nSerial;                                             // 匹配序号：一次人脸识别过程中的连续编号
    int           nGroupID;                                            // 组织结构ID
    char          strGroupName[128];                                   // 实际的组织（格式“节点1-节点2”）（UTF8编码）
    char          strPersonID[64];                                     // 用于单个设备的唯一标记人员信息的标记
    char          strUniqueID[64];                                     // 用于多个设备的唯一标记人员信息的标记
    char          strFaceLibID[64];                                    // 人脸所在的脸库类型（白名单，黑名单，访客）
    
    char                cReserve[64];                                        // 保留数据 
}P6SMSG_PUSH_PERSON_INFO_CONTENT, *LPP6SMSG_PUSH_PERSON_INFO_CONTENT;


// 人脸识别
typedef struct tagP6SMSG_PUSH_PERSON_INFO
{
    P6SMSG_PUSH_DEVICE_INFO                         stDeviceInfo;                                        // 设备信息
    int                                             nEventID;                                            // 事件 ID
    char                                            strTime[32];                                         // 事件时间，格式为 20200331T101001+08
    P6SMSG_PUSH_FACE_FEATURE_INFO                   stFaceFeature;                                       // 人脸识别特征信息：描述检测到的目标特征信息
    P6SMSG_PUSH_PERSON_INFO_CONTENT                 stPersonInfo;                                        // 人脸识别特征信息：描述检测到的目标特征信息

    char                        cReserve[128];                                       //保留字段
}P6SMSG_PUSH_PERSON_INFO, *LPP6SMSG_PUSH_PERSON_INFO;


// 车牌抓拍事件结构体

/**
 * @brief 通用车型，目前车牌抓拍事件使用
 * 
 */
typedef enum tagP6SMSG_PUSH_VEHICLE_TYPE
{
    P6SMSG_PUSH_VEHICLE_TYPE_INVALID,        // 起始无效值
    P6SMSG_PUSH_VEHICLE_TYPE_CAR,            // 小轿车
    P6SMSG_PUSH_VEHICLE_TYPE_SUV,            // suv
    P6SMSG_PUSH_VEHICLE_TYPE_MICROBUS,       // 面包车
    P6SMSG_PUSH_VEHICLE_TYPE_MIDIBUS,        // 中巴/大巴
    P6SMSG_PUSH_VEHICLE_TYPE_PICKUP          // 皮卡
}P6SMSG_PUSH_VEHICLE_TYPE;

/***
 * @brief 通用车辆颜色，目前车牌抓拍事件使用
*/
typedef enum tagP6SMSG_PUSH_VEHICLE_COLOR
{
    P6SMSG_PUSH_VEHICLE_COLOR_INVALID,      // 起始无效值
    P6SMSG_PUSH_VEHICLE_COLOR_BLACK,        // 黑色
    P6SMSG_PUSH_VEHICLE_COLOR_WHITE,        // 白色
    P6SMSG_PUSH_VEHICLE_COLOR_SILVERY,      // 银灰
    P6SMSG_PUSH_VEHICLE_COLOR_GRAY,         // 深灰
    P6SMSG_PUSH_VEHICLE_COLOR_BROWN,        // 棕色
    P6SMSG_PUSH_VEHICLE_COLOR_RED,          // 红色
    P6SMSG_PUSH_VEHICLE_COLOR_ORANGE,       // 橙色
    P6SMSG_PUSH_VEHICLE_COLOR_YELLOW,       // 黄色
    P6SMSG_PUSH_VEHICLE_COLOR_GREEN,        // 绿色
    P6SMSG_PUSH_VEHICLE_COLOR_CYAN,         // 青色
    P6SMSG_PUSH_VEHICLE_COLOR_BLUE,         // 蓝色
    P6SMSG_PUSH_VEHICLE_COLOR_PINK,        // 粉色
    P6SMSG_PUSH_VEHICLE_COLOR_PURPLE,      // 紫色
    P6SMSG_PUSH_VEHICLE_COLOR_CHAMPAGNE    // 香槟色
}P6SMSG_PUSH_VEHICLE_COLOR;

/**
 * @brief 通用车牌颜色，目前车牌抓拍事件使用
 * 
 */
typedef enum tagP6SMSG_PUSH_PLATE_COLOR
{
    P6SMSG_PUSH_PLATE_COLOR_INVALID,    // 起始无效值
    P6SMSG_PUSH_PLATE_COLOR_BLUE,       // 蓝牌
    P6SMSG_PUSH_PLATE_COLOR_YELLOW,     // 黄牌
    P6SMSG_PUSH_PLATE_COLOR_BLACK,      // 黑牌
    P6SMSG_PUSH_PLATE_COLOR_WHITE,      // 白牌
    P6SMSG_PUSH_PLATE_COLOR_GREEN,      // 绿牌
    P6SMSG_PUSH_PLATE_COLOR_SMALL_NEW_GREEN,    // 渐变绿
    P6SMSG_PUSH_PLATE_COLOR_LARGE_NEW_GREEN,    // 黄绿
    P6SMSG_PUSH_PLATE_COLOR_ABENSE       // 缺省
}P6SMSG_PUSH_PLATE_COLOR;

/*!
*  @struct	tagObjectRect
*  @brief	目标位置
*/
typedef struct tagP6SMSG_PUSH_OBJECT_RECT
{
	int				nLeft;							/*!< 左									*/
	int				nRight;							/*!< 右									*/
	int				nTop;							/*!< 上									*/
	int				nBottom;						/*!< 下									*/
}P6SMSG_PUSH_OBJECT_RECT, *LPP6SMSG_PUSH_OBJECT_RECT;

// 车牌属性结构体
typedef struct tagP6SMSG_PUSH_CAR_LICENSE_ATTRIBUTE_INFO
{
    P6SMSG_PUSH_VEHICLE_TYPE             dwAttriClass;                               // 车辆类型
    P6SMSG_PUSH_VEHICLE_COLOR            dwCarAttriColor;                            // 车辆颜色
    int                                  dwAttriOrientation;                         // 车辆方向（0-下行（车头面对摄像头）；1-上行（车尾面对摄像头）；其他值无效）
    char                                 cNumber[32];                                // 车牌号码
    P6SMSG_PUSH_PLATE_COLOR              dwCarLicenseAttriColor;                     // 车牌颜色
    char                                 dwCarLicenseRowNum;                         // 车牌单双排（1-单排；2-双排;其他值无效）
    P6SMSG_PUSH_OBJECT_RECT              dwCarLicenseRect;                           // 车牌坐标
    P6SMSG_PUSH_OBJECT_RECT              dwCarRect;                                  // 车型坐标
    char                                 cDriveEntryDrection;                        // 驶入方向（0-下行驶入；1-上行驶入；其他值无效）
	float                                fMatch;                                     // 车牌结果置信度
	float                                fScore;                                     // 车牌图评分
    char                                 cReserve[64];                               // 保留    
}P6SMSG_PUSH_CAR_LICENSE_ATTRIBUTE_INFO, *LPP6SMSG_PUSH_CAR_LICENSE_ATTRIBUTE_INFO;


// 车牌抓拍
typedef struct tagP6SMSG_PUSH_CAR_LICENSE_SNAPSHOT
{
    P6SMSG_PUSH_DEVICE_INFO                      stDeviceInfo;                                        // 设备信息
    int                                          nEventID;                                            // 事件 ID
    char                                         strTime[32];                                         // 事件时间，格式为 20200331T101001+08
    P6SMSG_PUSH_CAR_LICENSE_ATTRIBUTE_INFO       stAttributeInfo;                                     // 车牌属性

    char                                         cReserve[128];                                       //保留字段
}P6SMSG_PUSH_CAR_LICENSE_SNAPSHOT, *LPP6SMSG_PUSH_CAR_LICENSE_SNAPSHOT;


// 车牌布控事件
typedef struct tagP6SMSG_PUSH_LICENSE_PLATE_SURVEILLANCE
{
    P6SMSG_PUSH_DEVICE_INFO                      stDeviceInfo;                                        // 设备信息
    int                                          nEventID;                                            // 事件 ID
    char                                         strTime[32];                                         // 事件时间，格式为 20200331T101001+08
    P6SMSG_PUSH_CAR_LICENSE_ATTRIBUTE_INFO       stAttributeInfo;                                     // 车牌属性
	int dwLicensePlateNumStatus;	                                                                  // 0，陌生车牌; 1,已注册车牌; 2,非绿色车牌; 3,黄色车牌; 4,无车牌
    
    char cReserve[256];        //保留
} P6SMSG_PUSH_LICENSE_PLATE_SURVEILLANCE, *LPP6SMSG_PUSH_LICENSE_PLATE_SURVEILLANCE;


// 视频遮挡事件
typedef struct tagP6SMSG_PUSH_VIDEO_COVER
{
    P6SMSG_PUSH_DEVICE_INFO                  stDeviceInfo;                                       // 设备信息
    int                                      nEventID;                                           // 事件 ID
    char                                     strTime[32];                                        // 事件时间，格式为 20200331T101001+08
    P6SMSG_PUSH_OBJECT_RECT                  dwRect;                                             // 坐标

    char                                     cReserve[128];                                      //保留字段
}P6SMSG_PUSH_VIDEO_COVER, *LPP6SMSG_PUSH_VIDEO_COVER;


// 电瓶车事件
typedef struct tagP6SMSG_PUSH_ELECTRIC_BIKE
{
    P6SMSG_PUSH_DEVICE_INFO                  stDeviceInfo;                                       // 设备信息
    int                                      nEventID;                                           // 事件 ID
    char                                     strTime[32];                                        // 事件时间，格式为 20200331T101001+08
    P6SMSG_PUSH_OBJECT_RECT                  dwRect;                                             // 坐标

    char                                     cReserve[128];                                      //保留字段
}P6SMSG_PUSH_ELECTRIC_BIKE, *LPP6SMSG_PUSH_ELECTRIC_BIKE;


// 煤气罐事件
typedef struct tagP6SMSG_PUSH_GAS_CONTAINER
{
    P6SMSG_PUSH_DEVICE_INFO                  stDeviceInfo;                                       // 设备信息
    int                                      nEventID;                                           // 事件 ID
    char                                     strTime[32];                                        // 事件时间，格式为 20200331T101001+08
    P6SMSG_PUSH_OBJECT_RECT                  dwRect;                                             // 坐标

    char                                     cReserve[128];                                      //保留字段
}P6SMSG_PUSH_GAS_CONTAINER, *LPP6SMSG_PUSH_GAS_CONTAINER;


// 一键呼叫挂断事件
typedef struct tagP6SMSG_PUSH_STOP_KEY2CALL
{
    P6SMSG_PUSH_DEVICE_INFO                  stDeviceInfo;                                       // 设备信息
    int                                      nEventID;                                           // 事件 ID
    char                                     strTime[32];                                        // 事件时间，格式为 20200331T101001+08

    char                                     cReserve[128];                                      //保留字段
}P6SMSG_PUSH_STOP_KEY2CALL, *LPP6SMSG_PUSH_STOP_KEY2CALL;



// 电子围栏 V2 事件
typedef struct tagP6SMSG_PUSH_ELECTRONIC_DEFENCE_V2
{
    P6SMSG_PUSH_DEVICE_INFO                  stDeviceInfo;                                       // 设备信息
    int                                      nEventID;                                           // 事件 ID
    char                                     strTime[32];                                        // 事件时间，格式为 20200331T101001+08
    char                                     strDetectTarget[32];                                // 检测目标;,"person"-人形，"MotorVehicle" 机动车, "NonMotorVehicle" 非机动车
    char                                     strDetectTargetStatus[32];                          // 检测目标状态；"enter" 进入,"leave" 离开,"retention"滞留

    char                                     cReserve[128];                                      //保留字段
}P6SMSG_PUSH_ELECTRONIC_DEFENCE_V2, *LPP6SMSG_PUSH_ELECTRONIC_DEFENCE_V2;



//人数统计触发事件

// 人数超限报警事件区域信息
typedef struct tagP6SMSG_PUSH_PEOPLE_NUMBER_OVERLIMIT_AREA_INFO
{

	int nAreaNum;				/*!< 区域序号，取值1，2，3	*/
	int nPersonStatus;			/*!< 人员状态;0,人数超员，1.人数过少	*/

	char	cReserve[64];								/*!< 保留	*/
} P6SMSG_PUSH_PEOPLE_NUMBER_OVERLIMIT_AREA_INFO, *LPP6SMSG_PUSH_PEOPLE_NUMBER_OVERLIMIT_AREA_INFO;


// 人数超限报警事件结构体
typedef struct tagP6SMSG_PUSH_PEOPLE_NUMBER_OVERLIMIT
{
    P6SMSG_PUSH_DEVICE_INFO                         stDeviceInfo;                                       // 设备信息
    int                                             nEventID;                                           // 事件 ID
    char                                            strTime[32];                                        // 事件时间，格式为 20200331T101001+08
    int                                             nValidAreaCount;                                    /*!< 有效区域个数  */
    P6SMSG_PUSH_PEOPLE_NUMBER_OVERLIMIT_AREA_INFO   stAreaInfo[5];                                      /*!< 区域信息   */
    P6SMSG_PUSH_PEOPLE_NUMBER_OVERLIMIT_AREA_INFO   stAreaInfoReserve[3];                               /*!< 区域信息保留字段   */

    char                                            cReserve[128];                                      //保留字段
}P6SMSG_PUSH_PEOPLE_NUMBER_OVERLIMIT, *LPP6SMSG_PUSH_PEOPLE_NUMBER_OVERLIMIT;


// 人数统计事件（周期性上报）区域信息
typedef struct tagP6SMSG_PUSH_PEOPLE_NUMBER_STATISTICS_AREA_INFO
{

	int nAreaNum;				/*!< 区域序号，取值1，2，3	*/
	int nPersonNumber;			/*!< 人员数量	*/

	char	cReserve[64];								/*!< 保留	*/
} P6SMSG_PUSH_PEOPLE_NUMBER_STATISTICS_AREA_INFO, *LPP6SMSG_PUSH_PEOPLE_NUMBER_STATISTICS_AREA_INFO;


// 人数统计事件（周期性上报）结构体
typedef struct tagP6SMSG_PUSH_PEOPLE_NUMBER_STATISTICS
{
    P6SMSG_PUSH_DEVICE_INFO                         stDeviceInfo;                                       // 设备信息
    int                                             nEventID;                                           // 事件 ID
    char                                            strTime[32];                                        // 事件时间，格式为 20200331T101001+08
    int                                             nValidAreaCount;                                    /*!< 有效区域个数  */
    char                                            strStartTime[32];                                   /*!< 开始时间,格式:YYYYMMDDTHHMMSS+08,例：20040503T093008+08  */
    char                                            strEndTime[32];                                     /*!< 结束时间,格式:YYYYMMDDTHHMMSS+08,例：20040503T093008+08  */
    P6SMSG_PUSH_PEOPLE_NUMBER_STATISTICS_AREA_INFO  stAreaInfo[5];                                      /*!< 区域信息   */
    P6SMSG_PUSH_PEOPLE_NUMBER_STATISTICS_AREA_INFO  stAreaInfoReserve[3];                               /*!< 区域信息保留字段   */

    char                                            cReserve[128];                                      //保留字段
}P6SMSG_PUSH_PEOPLE_NUMBER_STATISTICS, *LPP6SMSG_PUSH_PEOPLE_NUMBER_STATISTICS;



// 长久离家
typedef struct tagP6SMSG_PUSH_LONG_AWAY
{
    P6SMSG_PUSH_DEVICE_INFO     stDeviceInfo;                                       // 设备信息
    int                         nEventID;                                           // 事件 ID
    char                        strTime[32];                                        // 事件时间，格式为 20200331T101001+08
    char                        strStartTime[32];                                   // 离家开始时间, 格式为YYYYMMDDTHHMMSS+08
    char                        strEndTime[32];                                     // 离家结束时间, 格式为YYYYMMDDTHHMMSS+08

    char                        cReserve[128];                                      //保留字段
}P6SMSG_PUSH_LONG_AWAY, *LPP6SMSG_PUSH_LONG_AWAY;

////////////////////////////////////////////////////////////////////////////



#if !defined(USE_C_LANGUAGE)
typedef int (*IF_GetNetworkStatus)(int& nStatus);
#else
typedef int (*IF_GetNetworkStatus)(int* nStatus);
#endif



typedef struct tagP6SMSG_DYNAMIC_GRAPHICS_INFO
{
    int nValid; //结构体是否有效,支持动态图并且启用中间层重传策略的时候才生效
    int picType; //图片类型,0:关键图（主图），1：辅图，2:预录图
    int imageSerialNumber; //图片的序列号,用来给平台排序用的
    char cReserve[32];
}P6SMSG_DYNAMIC_GRAPHICS_INFO,*LPP6SMSG_DYNAMIC_GRAPHICS_INFO;



typedef struct tagP6SMSG_PUSH_EVENT_INFO
{
	char cDeviceDID[64];
    int nChannel;                               //通道号
	unsigned long long  nEventTime;             //事件产生的时间
    unsigned long long  nDeviceTime;            //设备当前时间
	int nEventType;                             //事件的类型
	char* strPicBuff;                           //图片的buff
	int nPicLen;                                //图片的长度
    char bSupportGetMediaByFile;		        //是否支持通过文件的方式来获取媒体数据 0 不支持，1，支持,改参数只针对IOTB及以后申请的平台
	char cMediaFilePath[128];			        //文件路径，当bSupportGetMediaByFile为1时有效
    unsigned int uiHasSecondPicture;			//是否有第二张图片，0：没有，1：有，置为1时，pPictureData才有效
	char* pPictureData;						    //图片数据
	unsigned int nPictureDataLen;				//图片数据长度
	unsigned int uiHasOtherInfo;				//对应得事件是否有其他信息（0：没有，1：有）
	char* pP6SMSGBuffer;						//P6SMSG需要传递得其他信息（格式为：变量名=值#变量名=值#变量名=值，如username=aaa#password=111111, 如果变量名与值中有#需要将#转换成%23）
	unsigned int nP6SMSGBufferLen;				//其他信息得长度
    char* pP6SMSGData;                          // P6SMSGData（json 格式数据。例：{"name": "qwe"}）
    unsigned int nP6SMSGDataLen;                // P6SMSGData 长度
    char bSecondPictureSupportByFile;			//第二张图片是否支持通过文件的方式来获取媒体数据 0 不支持，1，支持
	char cSecondMediaFilePath[128];				//第二张图片的文件路径，当bSupportGetMediaByFile为1时有效	
    int* pStructInfo;                           //云消息元数据结构体的指针
    int nEventID;                               //事件ID
    P6SMSG_DYNAMIC_GRAPHICS_INFO stImageInfo;   //动图信息
    char strUUID[128];                          //事件UUID
    char cReserve[172];
} P6SMSG_PUSH_EVENT_INFO,*LPP6SMSG_PUSH_EVENT_INFO;



typedef struct tagP6SMSG_SERVER_INFO
{
    char did[64];
    char softwareVersion[128];   // 固件版本
    char deviceType[128];        // 设备型号
    char boardId[64];            // boardId
    int  customer;               // 客户id
    char versionId[64];          // 客户版本id
    char mac[64];                // mac地址
    char sn[64];                 // sn号
	char chipId[64];			// chip id
	unsigned char videoChannels; //通道数 
	unsigned char audioChannels; //音频通道数 
	unsigned char alarmInChannels; //报警输入通道数 
	unsigned char alarmOutChannels; //报警输入通道数 
	unsigned char voiceInChannels; //报警输入通道数 
    char url[64];                // 平台 url
    int nIsAOVMode;               // AOV 模式
    char reserved[60];          // 保留
}P6SMSG_SERVER_INFO, *LPP6SMSG_SERVER_INFO;

typedef struct 	tagP6SMSG_PUSH_EVENT
{
	int nValid;	
    char cDeviceDID[64];
	unsigned long long  nEventTime;     //事件产生的时间
    unsigned long long  nDeviceTime;    //事件产生的时间
	int nEventType;                     //事件的类型
	char *strPicBuff;                   //图片的buff
	int nPicLen;                        //图片的长度
	char szReserved[128];		        //保留
}P6SMSG_PUSH_EVENT, *LPP6SMSG_PUSH_EVENT;

typedef struct tagP6SMSG_POLICY_PARAMS
{
	int  nMaxEventCount;								//最大保存的推送事件个数。
                                                        //(支持动态图且不支持中间层重传时，该数值为队列的实际大小，即事件的个数+图片的个数，
                                                        //例：若该参数设置为 10，动图需要5张图片，则队列中只会保存两个事件）
    int  nRetryCount;								    //失败重传次数。
    int  nPushInterval;    							    //最大推送的间隔，单位是秒。
}P6SMSG_POLICY_PARAMS,*LPP6SMSG_POLICY_PARAMS;


typedef struct tagP6SMSG_POLICY_PARAMS_V2
{
	int  nMaxEventCount;								//最大保存的推送事件个数。
    int  nRetryCount;								    //失败重传次数。
    int  nPushInterval;    							    //最大推送的间隔，单位是秒。
    int  nLimitMode;    							    //限制模式。0 - 默认值，按推送事件的个数限制（兼容以前的逻辑，以前没有模式的概念，默认就是以数量做限制）；
                                                        //1 - 按推送事件的个数限制； 2 - 按内存大小限制； 3 - 自动模式（个数和内存哪个先到限定值就判断哪个）
    int  nMaxMemorySize;    							//最大内存大小。默认值：1024 * 1024

    char reserved[256];                                 // 保留
}P6SMSG_POLICY_PARAMS_V2,*LPP6SMSG_POLICY_PARAMS_V2;

typedef struct tagP6SMSG_SERVER_STATICS
{
    int nPushMode;                      // 事件推送模式。0 ： 不推图，1 ： 推图
    int nPushStatus;                    // 事件推送状态。0 ： 可以推送，1 ： 不能推送

    char szReserved[256];		        // 保留
}P6SMSG_SERVER_STATICS, *LPP6SMSG_SERVER_STATICS;


typedef struct tagP6SMSG_EVENT_STATUS
{
    unsigned long eventID;                      //事件ID,事件唯一标识
    int nP6SEventPublishStatus;                 //云消息事件发送状态;1：成功;2：失败;
    int imageSerialNumber;                      //图片的序列号,动图时，多个事件的id是一样的，需要根据这个区分
    char cReserve[124];
}P6SMSG_EVENT_STATUS,*LPP6SMSG_EVENT_STATUS;

//事件发送状态回调
typedef int (*IF_P6SMSGSendEventStatus)(P6SMSG_EVENT_STATUS sendEventStatus);

typedef struct tagP6SMSG_IMAGE_CAPTURE_PARAM
{
    char* buffer;                       //存放图片地址
    int nMaxBufferSize;                 //传入最大buffer大小，超过这个返回失败，小于则返回的是实际图片长度（输入参数）
    int nPicLen;                        //图片的实际大小（输出参数）
    char cReserve[128];
}P6SMSG_IMAGE_CAPTURE_PARAM,*LPP6SMSG_IMAGE_CAPTURE_PARAM;

//抓图回调
typedef int (*IF_P6SMSGImageCapture)(P6SMSG_IMAGE_CAPTURE_PARAM* imageCaptureParam);


typedef struct tagP6SMSG_PACKSGE_STATUS
{
    int isCloudRecognitionPackage;          // 是否开通云识套餐0:未开通 1:开通
    char cReserve[128];
}P6SMSG_PACKSGE_STATUS,*LPP6SMSG_PACKSGE_STATUS;

//套餐状态回调
typedef int (*IF_P6SMSGPackageStatus)(P6SMSG_PACKSGE_STATUS* packageStatusParam);


typedef struct tagP6SMSG_CALLBACK
{
    IF_P6SMSGSendEventStatus sendEventStatusCallback;
    IF_P6SMSGImageCapture imageCaptureCallback;
    IF_P6SMSGPackageStatus packageStatusCallback;
    char cReserve[120];
}P6SMSG_CALLBACK;


//中间层主动设置给云消息的参数
typedef struct tagP6SMSG_PARAM
{
    int nRetransmissionMode; //重传模式 0 协议控制重传，1 中间层控制重传。中间层控制重传时，由中间层保存数据，协议控制重传时，由协议保存数据，缓冲区满了之后正常淘汰，没满之前一直重试上报事件
    int isSupportDynamicGraphics; //是否支持动图（不启用中间层重传才生效），0:不支持，1：支持
    int isSupportPrerecorded; //是否支持预录动图（支持动图才生效），0:不支持，1：支持
    int nImageLength; //抓图图片 buffer 的最大长度（默认大小为 64K）
    char cReserve[256 - 12]; //保留字段
}P6SMSG_PARAM,*LPP6SMSG_PARAM;


#endif