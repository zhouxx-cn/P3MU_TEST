/*!
* @file
*  ZWP2PPUSHVNClient_Common.h
* \brief
*  客户端SDK接口结构与类型定义
* @author
*  zw
* @date
*  2017年07月29日
* @version
*  1.2.0
*/

#ifndef __ZWP2P_PUSH_VNCLIENT_COMMON_H__

#define __ZWP2P_PUSH_VNCLIENT_COMMON_H__

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
typedef enum tageZWP2P_ERR_CODE
{
	eZWP2P_PUSH_ERR_SUCCESS = 0,						// 成功
	eZWP2P_PUSH_ERR_FAILED,								// 失败

	eZWP2P_PUSH_ERR_FAILED_SDK_NOT_INIT = 100,			// 客户端sdk未初始化
	eZWP2P_PUSH_ERR_FAILED_SDK_ALREADY_INIT,			// 客户端以及初始化
	eZWP2P_PUSH_ERR_FAILED_UNSUPPORTED,                 // 不支持的操作
	eZWP2P_PUSH_ERR_FAILED_UNSUPPORT_PUSH_DEVICE,       // 不支持推送的设备
	eZWP2P_PUSH_ERR_FAILED_START_PUSH_THREAD,			// 启动推送线程失败
	eZWP2P_PUSH_ERR_FAILED_BUFFER_FULL,					// 缓冲区满
	eZWP2P_PUSH_ERR_FAILED_INVALID_PARAMETER,			// 参数无效
	eZWP2P_PUSH_ERR_FAILED_INVALID_HANDLE,				// 句柄值无效
	eZWP2P_PUSH_ERR_FAILED_NO_RESOURCE,					// 资源不足
	eZWP2P_PUSH_ERR_FAILED_MEMORY_ERROR,				// 内存不足
	eZWP2P_PUSH_ERR_FAILED_JSON_ERROR,					// JSON错误
	eZWP2P_PUSH_ERR_FAILED_SERVER_NO_RESPONSE,			// 服务器未响应
	eZWP2P_PUSH_ERR_FAILED_GET_UTCT,					// 获取UTCT时间错误
	eZWP2P_PUSH_ERR_FAILED_GENERAL_SUBKEY,				// 生成subkey错误
	eZWP2P_PUSH_ERR_FAILED_SET_SUBKEY,					// 设置subkey失败
	eZWP2P_PUSH_ERR_FAILED_CLEAR_SUBKEY,				// 清除subkey失败
}eZWP2P_ERR_CODE;

//设备事件类型
typedef enum tageDeviceEventType
{
	eDeviceEventTypeDeviceOffline = 0,					//设备下线
	eDeviceEventTypeDeviceOnline,						//设备在线
	eDeviceEventTypeDeviceDiskError,					//磁盘错误
	eDeviceEventTypeDeviceDiskFull,						//磁盘满
	eDeviceEventTypeDeviceIlleagalAccess,				//非法访问
	eDeviceEventTypeDeviceRestoretodefault,				//设备恢复到默认
	eDeviceEventTypeDeviceEncodeError,					//编码错误
	eDeviceEventTypeDeviceIPConflict,					//IP冲突
	eDeviceEventTypeDeviceCustomAll,					//自定义所有字段

	eDeviceChannelEventTypeMotion = 100,				//移动侦测事件
	eDeviceChannelEventTypeVideoLost,					//视频丢失
	eDeviceChannelEventTypeLineCross,					//跨线监测
	eDeviceChannelEventTypeVideoException,				//视频异常
	eDeviceChannelEventTypeAlarmIn,						//报警输入

	eDeviceChannelEventTypeAIFace = 200,				//画面中有人脸出现就触发一条该类型的推送消息
	eDeviceChannelEventTypeAITraversePlane,				//越界提醒推送消息
	eDeviceChannelEventTypeAIFieldInvade,				//区域入侵提醒推送消息(NVR:电子围栏消息)
	eDeviceChannelEventTypeAIPIR,						//PIR信号提醒（APP显示移动侦测），X81（低功耗）设备专用
	eDeviceChannelEventTypeAIPeopleDetect,				//人形检测，画面中出现人形就触发一条该消息
	eDeviceEventTypePeopleExceptionTemperature,			//体温异常推送
	eDeviceEventTypeBlackListPeople,					//黑名单推送
	eDeviceEventTypeTripWire,							//伴线
    eDeviceEventTypeDoorRing,							//门铃消息推送
	eDeviceEventTypeLowPower,							//低电量告警通知
	eDeviceEventTypeTamperProof,						//防拆告警通知
    eDeviceEventTypePowerLowerThan10,                   //电量低于 10%
    eDeviceEventTypePowerLowerThan20,                   //电量低于 20%
	eDeviceEventTypeSystemMessage = 500,				//系统消息
	eDeviceChannelEventTypeAICarDetect = 600,           //车型检测
	eDeviceChannelEventTypeAIFlameDetect = 601,			//火焰检测
	eDeviceChannelEventTypeAIQiutPostDetect = 602,      //离岗检测
	eDeviceChannelEventTypeAICarFlowCount = 603,      //车流统计
	
	eDeviceEventTypeLowVoltage = 700,                   //电压低告警
	eDeviceEventTypePoisonAir  = 701,                   //有毒气体
	eDeviceEventTypeKeyToCall  = 702,                   //一键呼叫
	eDeviceEventTypeSleepWakeUp = 703,                  //休眠唤醒
	eDeviceEventPassengerFlowUpperLimit = 704,          //客流总量超上限
	eDeviceEventPassengerFlowBackToNormal = 705,        //客流总量恢复正常
	eDeviceEventVideoShelter = 706,                     //视频遮挡事件
	eDeviceEventCrying = 707,                           //哭叫声
	eDeviceEventPassengerFlowCount = 708,               //客流统计汇总事件
	eDeviceEventQuitPostCount = 709,                    //离岗汇总事件
	eDeviceEventTypeAIFence = 710,			            //电子围栏提醒推送消息(IPC:电子围栏消息)
	eDeviceEventIntelligentTracking = 711,			    //智能跟踪
	eDeviceEventFaceRecognition = 712,			   		//人脸识别事件
    eDeviceEventDeviceGoToSleep = 713,			   		//设备开始休眠
    eDeviceEventHumanFaceSnapshot = 714,			   	//人脸抓拍（做天地伟业增加的，已经确认了人脸检测和人脸抓拍不是同一个事件类型）

}eDeviceEventType;

//设备事件类型
typedef enum tageDeviceCustomEventType
{
    eDeviceCustomEventTypeCryDetect = 10100,            //哭声检测事件
    eDeviceCustomEventTypeFallDetect = 10101,           //跌倒检测事件
    eDeviceCustomEventTypePetDetect = 10102,            //宠物检测事件
    eDeviceCustomEventTypeCarLicenseSnapshot = 10103,   //车牌抓拍事件（通用事件，非运营商）
    eDeviceCustomEventTypePetDetectForR2 = 10104,       //宠物检测事件(对应结构体, R2型号产品, 运营商项目)
    eDeviceCustomEventTypePetSnapshotForR2 = 10105,     //宠物抓拍事件(对应结构体 EventAIExPetSnapshotR2, R2型号产品, 运营商项目) 
	eDeviceCustomEventTypeShutdownAlarm = 10106, 		//关机警告事件
	eDeviceCustomEventTypeLowBatteryTenPersentAlarm = 10107,  //低电量百分之10事件
	eDeviceCustomEventTypeLowBatteryTwentyPersentAlarm = 10108, //低电量百分之20事件
	eDeviceCustomEventTypeRechargeFailAlarm = 10109,	//回冲失败事件
	eDeviceCustomEventTypeStrangerFace = 10110,			//陌生人脸事件 (对应结构体 EventFaceRecognitionV4, T5AI首次使用（刘海林）) 
	eDeviceCustomEventTypeFaceComparison = 10111,		//人脸比对事件(对应结构体 EventFaceRecognitionV4, T5AI首次使用（刘海林）)
    eDeviceCustomEventTypeChannelOffLine = 10112,		//通道掉线事件
    eDeviceCustomEventTypeRecordDataLost = 10113,		//录像数据丢失事件
    eDeviceCustomEventTypeDiskFault = 10114,			//硬盘故障事件
    eDeviceCustomEventTypeFaceComparisonAll = 10115, 	//人脸对比事件(包含了陌生人和脸库的人)(对应结构体 EventFaceRecognitionV4, T5AI首次使用（刘海林）) 
    eDeviceCustomEventTypeUrgentAssistance = 10116, 	//紧急求助（带一张背景图，首次使用（张辉）) 
    eDeviceCustomEventTypeFlameAlarm = 10117, 			//火焰报警(带一张背景图，T5AI首次使用（张辉）) 
    eDeviceCustomEventTypeTimedSnapshot = 10118, 		//定时抓拍(带一张背景图，T5AI首次使用（钟奎，P6SEvent）) 
    eDeviceCustomEventTypeGroundlockStatusChanged = 10119, //地锁状态变化(T5AI首次使用（钟奎，P6SEvent）) 
    eDeviceCustomEventTypeStartupReport = 10120, 		//开机上报(带一张背景图，T5AI首次使用（钟奎，P6SEvent）) 
    eDeviceCustomEventTypeAbnormalEvent = 10121, 		//异常事件(带一张背景图，T5AI首次使用（钟奎，P6SEvent）) 
    eDeviceCustomEventTypeNonWhiteListCar = 10122, 		//非白名单车辆(T5AI首次使用（钟奎，P6SEvent）) 
	eDeviceCustomEventTypePowerOnInfoReport = 10123,       // 上电信息上报事件（XT5N首次使用）
	eDeviceCustomEventTypeCPUPersistentlyHigh = 10124,       // CPU持续高事件 (NVR专用事件，IPC不用关注)
	eDeviceCustomEventTypeDiskIOKeepHigh = 10125,       // 硬盘IO持续高事件 (NVR专用事件，IPC不用关注)
	eDeviceCustomEventTypeWidebandOverrun = 10126,       // 宽带超限事件 (NVR专用事件，IPC不用关注)
	eDeviceCustomEventTypeCarLicenseCompare = 10127,       // 车牌对比事件 (暂时只有NVR使用)
	eDeviceCustomEventTypeLicensePlateSurveillance = 10132, // 车牌布控事件(T5AI首次首次使用，对接人是钟奎)，细分事件类型：在pCustomPayload中param参数之前加一个字段SEVT，取值为0，陌生车牌，1.已注册车牌，2.非绿色车牌，3.黄色车牌，4.无车牌
	eDeviceCustomEventTypeElectricBike = 10133,                        // 电瓶车事件(T5AI首次使用)
    eDeviceCustomEventTypeGasContainer = 10134,             // 煤气罐事件(T5AI首次使用)
    eDeviceCustomEventTypeStopKey2Call = 10135,             // 一键呼叫挂断(W42U首次使用)
	eDeviceCustomEventTypeElectronicDefenceV2 = 10136,             // 电子围栏V2（T5AI首次使用）
	eDeviceCustomEventTypeLongAway = 10139,				//长久离家事件（P3MU首次使用）
	eDeviceCustomEventTypeVideoCover = 20001, 			// 视频遮挡事件 （用于 T5AI）
}eDeviceCustomEventType;

//事件类别，大类，将上面的事件区分为几个类别（具体哪个类别中间层决定）
typedef enum tageDeviceEventCategory
{
	eDeviceEventCategorySimple = 1,            		//简单事件
    eDeviceEventCategoryPrecision = 2,           	//精确事件
    eDeviceEventCategorySystem = 3,            		//系统事件
}eDeviceEventCategory;

// 尚云推送 事件对应得额外消息得格式（#param=%s）,需要将json数据转换为字符串，事件对应得json得字段如下
/*
eDeviceCustomEventTypeFaceComparison
{
	"userName" : "111"
}

*/

//云消息推送 pP6SMSGBuffer 对应得字符串 变量名=值#变量名=值#变量名=值 如果变量名与值中有#设备需要将#转换成%23，然后服务器解析到%23后再转换成#

/*
事件类型：eDeviceCustomEventTypeFaceComparison = 10111,		//人脸比对事件
字符串为：userName=111#feature=111#personid=1111


事件类型：eDeviceCustomEventTypeFaceComparison = 10111,		//人脸比对事件 
字符串为：feature=111

*/


#define ZWP2P_PUSH_MAX_TITLE_LENGTH						32
#define ZWP2P_PUSH_MAX_DEVICE_NAME_LENGTH				40
#define ZWP2P_PUSH_MAX_CHANNEL_NAME_LENGTH				40
#define ZWP2P_PUSH_MAX_EVENT_CREATE_TIME_LENGTH	 		20
#define ZWP2P_PUSH_MAX_DID_LENGTH  						64
#define ZWP2P_PUSH_MAX_CUSTOM_CONTENT					80
#define ZWP2P_PUSH_MAX_CUSTOM_PAYLOAD					80
#define ZWP2P_PUSH_MAX_MAC_LEN							20
#define ZWP2P_PUSH_MAX_SUBKEY_DIR						256
#define ZWP2P_PUSH_MAX_EVENT_CREATE_TIME_LENGTH_V2	 	32
#define ZWP2P_PUSH_MAX_DOMAIN_LEN                       128
#define ZWP2P_PUSH_MAX_DOMAIN_LIST_NUM                  16

#if !defined(USE_C_LANGUAGE)
typedef int (*IF_GetNetworkStatus)(int& nStatus);
#else
typedef int (*IF_GetNetworkStatus)(int* nStatus);
#endif


////////////////////////////////////////////////////////////////////////////


typedef struct tagZWP2P_PUSH_INIT_PUSH_PARAMETERS
{
	ZWP2P_CHAR  cDeviceDid[ZWP2P_PUSH_MAX_DID_LENGTH];			//设备的DID号，比如:
																//"PPCS-014039-EKVEL,GPNSWM",
																//"PPCS-014039-EKVEL,GPNSWM#ZWBDID"：支持推送的DID
																//协议内部根据该DID号参数，库里面可以直接监测到设备是否支持推送服务，进而控制是否启动推送服务
	ZWP2P_UINT32  nPushInterval;    							//最大推送的间隔，单位是秒。
	ZWP2P_UINT32  nMaxEventCount;								//最大保存的推送事件个数。
}ZWP2P_PUSH_INIT_PUSH_PARAMETERS,*LPZWP2P_PUSH_INIT_PUSH_PARAMETERS;

typedef struct tagZWP2P_PUSH_SUBKEY_INPUT_PARAMS
{
	ZWP2P_CHAR  cMac[ZWP2P_PUSH_MAX_MAC_LEN];							//设备的MAC地址，格式是，每一个char表示MAC地址的一个字节，内部用来生成唯一的subkey
	ZWP2P_CHAR  SubkeyParamDir[ZWP2P_PUSH_MAX_SUBKEY_DIR];				//存储标记的目录，传输一个可读可写的永久保存的绝对路径，用来库内部的临时文件存储
	ZWP2P_UINT32 nIsSupportPushEventV2;    								//0，使用先进先出的推送逻辑，1.使用新的推送过滤逻辑
	ZWP2P_UINT32 nIsSupportAOV;											//是否支持AOV模式
	ZWP2P_UINT32 nIsSupportMultiChannel;								//是否支持区分通道发送
	ZWP2P_CHAR  cReserve[256 - 4 - 4 - 4];    									//保留字段
}ZWP2P_PUSH_SUBKEY_INPUT_PARAMS,*LPZWP2P_PUSH_SUBKEY_INPUT_PARAMS;


//中间层主动设置的参数（中途可以配置的参数）
typedef struct tagZWP2P_PUSH_EVENT_PARAMS
{
	ZWP2P_UINT32 nRunMode;											//0.常电模式，1.低功耗，2.省电，3.AOV
	ZWP2P_CHAR  cReserve[256];    									//保留字段
}ZWP2P_PUSH_EVENT_PARAMS,*LPZWP2P_PUSH_EVENT_PARAMS;

/*自定义payload的说明
1.当事件是有毒气体时：洁净:clean air，轻度污染:slightly polluted，中度污染:moderately polluted，重度污染:heavily polluted
2.当事件是休眠唤醒:摄像头休眠状态：sleep    摄像头唤醒状态：awaken
3.当事件是客户统计汇总事件时:eg:{StartTm:2022090510,EndTm:2022090512,Count:%d,oCount:%d,sTM:121212,eTM:131313};分别表示开始时间（单位：小时），结束时间，客流进入人数，客流流离开人数,开始时间（时分秒），结束时间（时分秒）
4.当事件是离岗检测汇总事件时:eg:{StartTm:2022090510,EndTm:2022090512,Count:%d,Time:%d};分别表示开始时间（单位：小时），结束时间，离岗次数和总时长
5.当事件是人脸识别事件时：自定义payload的param为 {organization:"xrs",name:"xrs",faceType:"white"}，其中 faceType，取值为 black （黑名单），white （白名单），visitor （访客），图片由pMediaData提供
6.当事件为eDeviceEventTypeDeviceCustomAll时，bSupportCustomize必须为2 ，字段uCategory，pP6SMSGData，uiDeviceTime，dwTimeZone，pCustomContent，
7.当事件是车流统计汇总事件时:eg:{StartTm:2022090510,EndTm:2022090512,Count:%d,oCount:%d,sTM:121212,eTM:131313};分别表示开始时间（单位：小时），结束时间，车流进入数，车流离开数，开始时间（时分秒），结束时间（时分秒）
8.当事件是车牌抓拍事件（10103）时：自定义payload的param为 {carLicenseColor:"blue",carLicense:"鄂A76898"},其中 carLicenseColor取值为车牌颜色，取值为 black （黑车），white （白车）,carLicense，车牌号
pCustomPayload，pMediaData，nMediaDataLen有效，其余字段无效`
*/
typedef struct tagZWP2P_PUSH_EVENT_INFO
{
	ZWP2P_CHAR  cPushTitle[ZWP2P_PUSH_MAX_TITLE_LENGTH];					//推送标题
	ZWP2P_CHAR	cDeviceName[ZWP2P_PUSH_MAX_DEVICE_NAME_LENGTH];				//设备名称（UTF8编码）,bSupportCustomize = 0时，可以为空,如果和ChannelName同时为空,那么自定义的内容有效
	ZWP2P_CHAR cChannelName[ZWP2P_PUSH_MAX_CHANNEL_NAME_LENGTH];			//通道名称（UTF8编码）,bSupportCustomize = 0时，可以为空,如果和cDeviceName同时为空,那么自定义的内容有效
	ZWP2P_CHAR cEventCreateTime[ZWP2P_PUSH_MAX_EVENT_CREATE_TIME_LENGTH];	//事件创建时间(ISO 8601标准格式的时间,例如：20040503T173008+480，20040503T173008是本地时间，+480是时区)
	ZWP2P_CHAR cCustomContent[ZWP2P_PUSH_MAX_CUSTOM_CONTENT];				//自定义内容，当bSupportCustomize为1时，该字段有效，为0的话，需要判断ChannelName和DeviceName同时为空，那么此参数有效
	ZWP2P_CHAR cCustomPayload[ZWP2P_PUSH_MAX_CUSTOM_PAYLOAD];				//自定义透传给APP的数据，当bSupportCustomize为1时，该字段有效，为0的话，当事件类型为
																			//eDeviceEventTypePoisonAir,
																			//eDeviceEventTypeSleepWakeUp
																			//eDeviceEventPassengerFlowCount
																			//eDeviceEventQuitPostCount
																			//eDeviceEventFaceRecognition，
																			//此定义有效,且只适用于接口ZWP2PPUSHVNClient_PushEvent_V2
	ZWP2P_UINT32 uiResourceType;											//资源类型1，图片，2，视频
	ZWP2P_UINT32 uiResourceNumber;											//资源号
	ZWP2P_CHAR * pMediaData;												//媒体数据
	ZWP2P_UINT32 nMediaDataLen;												//媒体数据长度
	ZWP2P_UINT32 uiEnableAI;												//是否支持AI功能，0，表示不支持，1，支持，支持的话，uiFC,uiOP,uiAI有效
	ZWP2P_UINT32 uiFC;														//0,无效数据，使用默认方式，默认为1
																			//1，仅下载 SPS 上的原图。且 AID 在 SPS_Upload 时的 Payload 参数需添加 "Src" 原图地址，
																			//2，下载并删除 SPS 上的原图。AID 在 SPS_Upload 时的 Payload 参数不不需要添加 "Src" 原图地址。															
	ZWP2P_UINT32 uiOP;														//0,无效数据，使用默认方式，默认为2
																			//1:不不上传图，单纯通知AI辨识结果
																			//2:通知AI辨识结果且上传AI辨识之后的加框图。
	ZWP2P_UINT32 uiAI;														//0,无效数据，使用默认方式，默认为人形
																			//1:人形侦测
																			//2:人脸侦测
																			//3.人脸侦测+人形侦测
																			//4.婴儿哭声检测
	ZWP2P_CHAR bSupportCustomize;											//是否支持自定义内容，0，不支持，1支持，如果为1，内部只关注除cDeviceName，cChannelName的其他字段，如果为0的话，兼容以前的逻辑，2.表示完全自定义
	ZWP2P_CHAR bSupportGetMediaByFile;										//是否支持通过文件的方式来获取媒体数据 0 不支持，1，支持,改参数只针对IOTB及以后申请的平台
	ZWP2P_CHAR cMediaFilePath[128];											//文件路径，当bSupportGetMediaByFile为1时有效	
	ZWP2P_CHAR bSupportCreateTimeV2;										//是否支持第二版本时间创建参数
	ZWP2P_CHAR cEventCreateTimeV2[ZWP2P_PUSH_MAX_EVENT_CREATE_TIME_LENGTH_V2];	//第二版本时间创建参数
	ZWP2P_CHAR uCategory; 										             //0:不用这个字段。1.push_normal  2.push_siren
    ZWP2P_CHAR * pP6SMSGData;                                               // P6SMSG 数据（json 格式数据。例：{"name": "qwe"}）设备直接透传给 APP 的数据，服务器不处理,该字段为pCustomPayload中param=的内容
    ZWP2P_UINT32 uiDeviceTime;                                              // 设备时间，仅云消息使用，该时间是透传给服务器，目前服务器没有使用
    ZWP2P_UINT32 dwTimeZone;                                                // 时区，该时区暂时没有使用
	ZWP2P_CHAR * pCustomContent;											// 当bSupportCustomize为2时有效，最大长度是256，参考[CreateTime][DeviceName][ChannelName] has a [Event] event
	ZWP2P_CHAR * pCustomPayload;											// 当bSupportCustomize为2时有效，最大长度是180,参考MT=0#TM=%s#DID=%s#ET=%d#CH=%d#RT=%d#RN=%d#ET=%d#SEVT=%d#param=%s#（
																															//MT表示消息类型，0，表示系统消息，1，表示通用消息，100.表示自定义消息）
																															//TM 表示事件创建时间
																															//DID 表示设备DID
																															//ET 表示事件类型
																															//RT 表示资源类型，0，表示文字，1，表示图片，2.表示视频
																															//RN 表示资源号
																															//SEVT 表示子事件类型
																															//param 表示参数 param=json字符串 对应得字段名参考eDeviceCustomEventType对应得json格式
    ZWP2P_UINT32 dwEventCH;	                                                // 事件通道，100为普通事件通道，200为实时事件通道
	ZWP2P_UINT32 uiHasSecondPicture;										//是否有第二张图片，0：没有，1：有，置为1时，pPictureData才有效
	ZWP2P_CHAR * pPictureData;												//图片数据
	ZWP2P_UINT32 nPictureDataLen;											//图片数据长度
	ZWP2P_UINT32 uiHasOtherInfo;											//对应得事件是否有其他信息（只对P6SMSG有用，对于尚云推送无用）（0：没有，1：有）
	ZWP2P_CHAR * pP6SMSGBuffer;												//P6SMSG需要传递得其他信息，设备传给服务器的数据（格式为：变量名=值#变量名=值#变量名=值，对应得字段名参考eDeviceCustomEventType对应得字符串格式, 如果变量名与值中有#设备需要将#转换成%23，然后服务器解析到%23后再转换成#）
	ZWP2P_UINT32 nP6SMSGBufferLen;											//其他信息得长度
    ZWP2P_UINT32 nP6SMSGTimeStampMS;                                        //云消息毫秒级时间戳，当前时间的毫秒数值。例：now_tv.tv_usec / 1000
	ZWP2P_UINT32 nSendMode;													//发送模式 0:尚云推送与云消息都发送，1:只发送云消息，2:只发送尚云推送 
    ZWP2P_UINT32 nP6SMSGDataLen;                                            //P6SMSGData 长度
	ZWP2P_UINT32* pStructInfo;                                              //云消息元数据结构体的指针
} ZWP2P_PUSH_EVENT_INFO,*LPZWP2P_PUSH_EVENT_INFO;


/*自定义payload的说明
1.当事件是有毒气体时：洁净:clean air，轻度污染:slightly polluted，中度污染:moderately polluted，重度污染:heavily polluted
2.当事件是休眠唤醒:摄像头休眠状态：sleep    摄像头唤醒状态：awaken
3.当事件是客户统计汇总事件时:eg:{StartTm:2022090510,EndTm:2022090512,Count:%d};分别表示开始时间（单位：小时），结束时间，客流人数
4.当事件是离岗检测汇总事件时:eg:{StartTm:2022090510,EndTm:2022090512,Count:%d,Time:%d};分别表示开始时间（单位：小时），结束时间，离岗次数和总时长
5.当事件是人脸识别事件时：自定义payload为 {organization:"xrs",name:"xrs",faceType:"white"}，其中 faceType，取值为 black （黑名单），white （白名单），visitor （访客），图片由pMediaData提供
6.当事件为eDeviceEventTypeDeviceCustomAll时，bSupportCustomize必须为2 ，字段uCategory，pP6SMSGData，uiDeviceTime，dwTimeZone，pCustomContent，
pCustomPayload，pMediaData，nMediaDataLen有效，其余字段无效`
7.当事件是车流统计汇总事件时:eg:{StartTm:2022090510,EndTm:2022090512,Count:%d,oCount:%d,sTM:121212,eTM:131313};分别表示开始时间（单位：小时），结束时间，车流进入数，车流离开数，开始时间（时分秒），结束时间（时分秒）
8.当事件是车牌抓拍事件（10103）时：自定义payload的param为 {carLicenseColor:"blue",carLicense:"鄂A76898"},其中 carLicenseColor表示车牌颜色，取值为 black （黑车），white （白车）,carLicense，车牌号
*/
typedef struct tagZWP2P_PUSH_EVENT_INFO_EX
{
	ZWP2P_CHAR  cPushTitle[ZWP2P_PUSH_MAX_TITLE_LENGTH];					//推送标题
	ZWP2P_CHAR	cDeviceName[ZWP2P_PUSH_MAX_DEVICE_NAME_LENGTH];				//设备名称（UTF8编码）,bSupportCustomize = 0时，可以为空,如果和ChannelName同时为空,那么自定义的内容有效
	ZWP2P_CHAR cChannelName[ZWP2P_PUSH_MAX_CHANNEL_NAME_LENGTH];			//通道名称（UTF8编码）,bSupportCustomize = 0时，可以为空,如果和cDeviceName同时为空,那么自定义的内容有效
	ZWP2P_CHAR cEventCreateTime[ZWP2P_PUSH_MAX_EVENT_CREATE_TIME_LENGTH];	//事件创建时间(ISO 8601标准格式的时间,例如：20040503T173008+480，20040503T173008是本地时间，+08是时区)，尚云推送和云消息公用
	ZWP2P_CHAR cCustomContent[ZWP2P_PUSH_MAX_CUSTOM_CONTENT];				//自定义内容，当bSupportCustomize为1时，该字段有效，为0的话，需要判断ChannelName和DeviceName同时为空，那么此参数有效
	ZWP2P_CHAR cCustomPayload[ZWP2P_PUSH_MAX_CUSTOM_PAYLOAD];				//自定义透传给APP的数据，当bSupportCustomize为1时，该字段有效，为0的话，当事件类型为
																			//eDeviceEventTypePoisonAir,
																			//eDeviceEventTypeSleepWakeUp
																			//eDeviceEventPassengerFlowCount
																			//eDeviceEventQuitPostCount
																			//eDeviceEventFaceRecognition，
																			//此定义有效,且只适用于接口ZWP2PPUSHVNClient_PushEvent_V2
	ZWP2P_UINT32 uiResourceType;											//资源类型1，图片，2，视频
	ZWP2P_UINT32 uiResourceNumber;											//资源号
	ZWP2P_CHAR * pMediaData;												//媒体数据
	ZWP2P_UINT32 nMediaDataLen;												//媒体数据长度
	ZWP2P_UINT32 uiEnableAI;												//是否支持AI功能，0，表示不支持，1，支持，支持的话，uiFC,uiOP,uiAI有效
	ZWP2P_UINT32 uiFC;														//0,无效数据，使用默认方式，默认为1
																			//1，仅下载 SPS 上的原图。且 AID 在 SPS_Upload 时的 Payload 参数需添加 "Src" 原图地址，
																			//2，下载并删除 SPS 上的原图。AID 在 SPS_Upload 时的 Payload 参数不不需要添加 "Src" 原图地址。															
	ZWP2P_UINT32 uiOP;														//0,无效数据，使用默认方式，默认为2
																			//1:不不上传图，单纯通知AI辨识结果
																			//2:通知AI辨识结果且上传AI辨识之后的加框图。
	ZWP2P_UINT32 uiAI;														//0,无效数据，使用默认方式，默认为人形
																			//1:人形侦测
																			//2:人脸侦测
																			//3.人脸侦测+人形侦测
																			//4.婴儿哭声检测
	ZWP2P_CHAR bSupportCustomize;											//是否支持自定义内容，0，不支持，1支持，如果为1，内部只关注除cDeviceName，cChannelName的其他字段，如果为0的话，兼容以前的逻辑，2.表示完全自定义
	ZWP2P_CHAR bSupportGetMediaByFile;										//是否支持通过文件的方式来获取媒体数据 0 不支持，1，支持,改参数只针对IOTB及以后申请的平台
	ZWP2P_CHAR cMediaFilePath[128];											//文件路径，当bSupportGetMediaByFile为1时有效	
	ZWP2P_CHAR bSupportCreateTimeV2;										//是否支持第二版本时间创建参数
	ZWP2P_CHAR cEventCreateTimeV2[ZWP2P_PUSH_MAX_EVENT_CREATE_TIME_LENGTH_V2];	//第二版本时间创建参数
	ZWP2P_CHAR uCategory; 										             //0:不用这个字段。1.push_normal  2.push_siren
    ZWP2P_CHAR * pP6SMSGData;                                               // P6SMSG 数据（json 格式数据。例：{"name": "qwe"}）设备直接透传给 APP 的数据，服务器不处理，该字段为pCustomPayload中param=的内容
    ZWP2P_UINT32 uiDeviceTime;                                              // 设备时间，仅云消息使用，该时间是透传给服务器，目前服务器没有使用
    ZWP2P_UINT32 dwTimeZone;                                                // 时区，该时区暂时没有使用
	ZWP2P_CHAR * pCustomContent;											// 当bSupportCustomize为2时有效，最大长度是256，参考[CreateTime][DeviceName][ChannelName] has a [Event] event
	ZWP2P_CHAR * pCustomPayload;											// 当bSupportCustomize为2时有效，最大长度是180,参考MT=0#TM=%s#DID=%s#ET=%d#CH=%d#RT=%d#RN=%d#ET=%d#SEVT=%d#param=%s#（
																															//MT表示消息类型，0，表示系统消息，1，表示通用消息，100.表示自定义消息）
																															//TM 表示事件创建时间
																															//DID 表示设备DID
																															//ET 表示事件类型
																															//RT 表示资源类型，0，表示文字，1，表示图片，2.表示视频
																															//RN 表示资源号
																															//SEVT 表示子事件类型
																															//param 表示参数 param=json字符串 对应得字段名参考eDeviceCustomEventType对应得json格式
    ZWP2P_UINT32 dwEventCH;	                                                // 事件通道，100为普通事件通道，200为实时事件通道
	ZWP2P_UINT32 uiHasSecondPicture;										//是否有第二张图片，0：没有，1：有，置为1时，pPictureData才有效
	ZWP2P_CHAR * pPictureData;												//图片数据
	ZWP2P_UINT32 nPictureDataLen;											//图片数据长度
	ZWP2P_UINT32 uiHasOtherInfo;											//对应得事件是否有其他信息（只对P6SMSG有用，对于尚云推送无用）（0：没有，1：有）
	ZWP2P_CHAR * pP6SMSGBuffer;												//P6SMSG需要传递得其他信息，设备传给服务器的数据（格式为：变量名=值#变量名=值#变量名=值，对应得字段名参考eDeviceCustomEventType对应得字符串格式, 如果变量名与值中有#设备需要将#转换成%23，然后服务器解析到%23后再转换成#）
	ZWP2P_UINT32 nP6SMSGBufferLen;											//其他信息得长度
    ZWP2P_UINT32 nP6SMSGTimeStampMS;                                        //云消息毫秒级时间戳，当前时间的毫秒数值。例：now_tv.tv_usec / 1000
	ZWP2P_UINT32 nSendMode;													//发送模式 0:尚云推送与云消息都发送，1:只发送云消息，2:只发送尚云推送 
    ZWP2P_UINT32 nP6SMSGDataLen;                                            //P6SMSGData 长度
	ZWP2P_CHAR cSendType;													//发送事件类别 0：该参数无效，1：简单事件，2：精确事件，3系统事件 ,用来区分事件的发送频率（nvr使用）
	ZWP2P_CHAR bSecondPictureSupportByFile;									//第二张图片是否支持通过文件的方式来获取媒体数据 0 不支持，1，支持云消息使用
	ZWP2P_CHAR cSecondMediaFilePath[128];									//第二张图片的文件路径，当bSupportGetMediaByFile为1时有效	
	ZWP2P_CHAR cReserve[512];
} ZWP2P_PUSH_EVENT_INFO_EX,*LPZWP2P_PUSH_EVENT_INFO_EX;


//中间层主动设置给云消息的参数
typedef struct tagZWP2P_PUSH_P6SMSG_PARAM
{
    ZWP2P_UINT32 nRetransmissionMode; //重传模式 0 协议控制重传，1 中间层控制重传。中间层控制重传时，由中间层保存数据，协议控制重传时，由协议保存数据，缓冲区满了之后正常淘汰，没满之前一直重试上报事件
    ZWP2P_UINT32 isSupportDynamicGraphics; //是否支持动图（不启用中间层重传才生效），0:不支持，1：支持
    ZWP2P_UINT32 isSupportPrerecorded; //是否支持预录动图（支持动图才生效），0:不支持，1：支持
    ZWP2P_UINT32 nImageLength; //抓图图片 buffer 的最大长度（默认大小为 64K）

    ZWP2P_CHAR cReserve[256 - 12]; //保留字段
}ZWP2P_PUSH_P6SMSG_PARAM,*LPZWP2P_PUSH_P6SMSG_PARAM;


typedef struct tagZWP2P_PUSH_P6SMSG_PICTURE_INFO
{
    int nValue;    //结构体是否有效
    int imageStorageMode;                       //图片存储模式,0:直接根据strPicBuff传递，1:使用文件路径方式传递cMediaFilePath
    char* strPicBuff;                           //图片的buff，协议会copy一份，中间层自己释放内存
    int nPicLen;                                //图片的长度
    char cMediaFilePath[128];                   //文件路径，当bSupportGetMediaByFile为1时有效
    char cReserve[128];
}ZWP2P_PUSH_P6SMSG_PICTURE_INFO,*LPZWP2P_PUSH_P6SMSG_PICTURE_INFO;


typedef struct tagZWP2P_PUSH_P6SMSG_DYNAMIC_GRAPHICS_INFO
{
    int nValid; //结构体是否有效,支持动态图并且启用中间层重传策略的时候才生效
    int picType; //图片类型,0:关键图（主图），1：辅图，2:预录图
    int imageSerialNumber; //图片的序列号,用来给平台排序用的
    char cReserve[32];
}ZWP2P_PUSH_P6SMSG_DYNAMIC_GRAPHICS_INFO,*LPZWP2P_PUSH_P6SMSG_DYNAMIC_GRAPHICS_INFO;


typedef struct tagP6SMSG_EVENT_INFO
{
    char cDeviceDID[64];
    unsigned long eventID;                      //事件ID,给中间层回调事件状态时，用来确认哪个事件
    int nChannel;                               //通道号
    unsigned long long  nEventTime;             //事件产生的时间，格式：时间戳，如：1754472507028，中间层的时间如果是带时区的需要处理一下时区
    unsigned long long  nDeviceTime;            //设备的时间，格式：时间戳，如：1754472507028，中间层的时间如果是带时区的需要处理一下时区
    int nEventType;                             //事件的类型
    ZWP2P_PUSH_P6SMSG_PICTURE_INFO stCaptureInfo;          //抓拍图
    ZWP2P_PUSH_P6SMSG_PICTURE_INFO stBackgroundInfo;      //背景图
    int* pEventStructInfo;                      //事件对应的结构体的指针，协议内部组装json，这个json字段服务器不解析，直接透传给客户端的
    unsigned int uiHasOtherContent;             //对应得事件是否有其他信息（0：没有， 1：有），这个是用来给服务器解析筛选用的字段
    char* pOtherContentBuffer;                  //P6SMSG需要传递得其他信息（格式为：变量名=值#变量名=值#变量名=值，如username=aaa#password=111111, 如果变量名与值中有#需要将#转换成%23）
    unsigned int nOtherContentBufferLen;        //其他信息得长度
    ZWP2P_PUSH_P6SMSG_DYNAMIC_GRAPHICS_INFO dynamicGraphicsinfo; //动态图信息
    char strUUID[128];
    ZWP2P_CHAR * pP6SMSGData;                   // P6SMSG 数据（json 格式数据。例：{"name": "qwe"}）设备直接透传给 APP 的数据，服务器不处理，该字段为pCustomPayload中param=的内容
    ZWP2P_UINT32 nP6SMSGDataLen;                                            //P6SMSGData 长度
    char cReserve[512 - 44 - 128 - 8];
} ZWP2P_PUSH_P6SMSG_EVENT_INFO,*LPZWP2P_PUSH_P6SMSG_EVENT_INFO;


typedef struct tagZWP2P_PUSH_P6SMSG_EVENT_STATUS
{
    unsigned long eventID;                      //事件ID,事件唯一标识
    int nP6SEventPublishStatus;                 //云消息事件发送状态;1：成功;2：失败;
    int imageSerialNumber;                      //图片的序列号,动图时，多个事件的id是一样的，需要根据这个区分
    char cReserve[124];
}ZWP2P_PUSH_P6SMSG_EVENT_STATUS,*LPZWP2P_PUSH_P6SMSG_EVENT_STATUS;

//事件发送状态回调
typedef int (*IF_SendEventStatus)(ZWP2P_PUSH_P6SMSG_EVENT_STATUS sendEventStatus);


typedef struct tagZWP2P_PUSH_P6SMSG_IMAGE_CAPTURE_PARAM
{
    char* buffer;                       //存放图片地址
    int nMaxBufferSize;                 //传入最大buffer大小，超过这个返回失败，小于则返回的是实际图片长度
    int nPicLen;                        //图片实际大小
    char cReserve[128];
}ZWP2P_PUSH_P6SMSG_IMAGE_CAPTURE_PARAM,*LPZWP2P_PUSH_P6SMSG_IMAGE_CAPTURE_PARAM;

//抓图回调
typedef int (*IF_ImageCapture)(ZWP2P_PUSH_P6SMSG_IMAGE_CAPTURE_PARAM* imageCaptureParam);


typedef struct tagZWP2P_PUSH_P6SMSG_PACKSGE_STATUS
{
    int isCloudRecognitionPackage;          // 是否开通云识套餐0:未开通 1:开通
    char cReserve[128];
}ZWP2P_PUSH_P6SMSG_PACKSGE_STATUS,*LPZWP2P_PUSH_P6SMSG_PACKSGE_STATUS;

//套餐状态回调
typedef int (*IF_PackageStatus)(ZWP2P_PUSH_P6SMSG_PACKSGE_STATUS* packageStatusParam);


typedef struct tagZWP2P_PUSH_P6SMSG_CALLBACK
{
    IF_SendEventStatus sendEventStatusCallback;
    IF_ImageCapture imageCaptureCallback;
    IF_PackageStatus packageStatusCallback;
    char cReserve[120];
}ZWP2P_PUSH_P6SMSG_CALLBACK;


typedef struct tagZWP2P_EVENT_ATTRIBUTE{
	
	int nSend2ServerTimeout;		//同一通道同一个事件类型的事件超时时间，默认值是5分钟
	int nVirtualStopSignalTimeOut;	//虚拟停止信号超时时间,单位是秒,默认是30秒
	int nAddWeightByTriggerStop;	//触发停止信号增加的权值，默认是增加8
	int nAddWeightByHighPriority;   //高优先级事件增加的权值，默认是增加16
    int nEnablePushP6SMSG;          //是否发送 P6SMessage 数据，默认不发送
	char cReserve[60];				/*!保留字段*/
}ZWP2P_EVENT_ATTRIBUTE;

typedef struct tagZWPush_DOMAIN_INFO
{
	int nValid;
	char cDomain[64];
}ZWPush_DOMAIN_INFO;

typedef struct tagZWPush_DOMAIN_INFO_LIST
{
	int nCount;									//域名 信息实际个数
	ZWPush_DOMAIN_INFO stDomainInfoList[32];  //域名信息列表
}ZWPush_DOMAIN_INFO_LIST;


typedef struct tagZWP2P_PUSH_DOMAIN_INFO
{
	ZWP2P_UINT32 nValid;
	ZWP2P_CHAR  cDomain[ZWP2P_PUSH_MAX_DOMAIN_LEN];                 //域名
	ZWP2P_CHAR  cReserve[256];    					                //保留字段
}ZWP2P_PUSH_DOMAIN_INFO;

typedef struct tagZWP2P_PUSH_DOMAIN_PARAMS
{
    ZWP2P_CHAR  cDID[ZWP2P_PUSH_MAX_DID_LENGTH];                                    //输入参数，DID
	ZWP2P_CHAR  cDomainSuffix[ZWP2P_PUSH_MAX_DOMAIN_LEN];                           //输入参数，域名后缀
	ZWP2P_CHAR  cReserve[256];    							                        //保留字段
}ZWP2P_PUSH_DOMAIN_PARAMS;


typedef struct tagZWPush_P6SMSG_SERVER_INFO
{
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
    char reserved[64];          // 保留
}ZWPush_P6SMSG_SERVER_INFO;

typedef struct tagZWP2P_PUSH_P6SMSG_POLICY_PARAMS
{
	ZWP2P_UINT32  nMaxEventCount;								//最大保存的推送事件个数。
    ZWP2P_UINT32  nRetryCount;								    //失败重传次数。此字段无效，协议重传逻辑现在均改为发送失败时若队列未满就一直发送
    ZWP2P_UINT32  nPushInterval;    							//最大推送的间隔，单位是秒。
}ZWP2P_PUSH_P6SMSG_POLICY_PARAMS,*LPZWP2P_PUSH_P6SMSG_POLICY_PARAMS;

typedef struct tagZWP2P_PUSH_P6SMSG_POLICY_PARAMS_V2
{
	ZWP2P_UINT32  nMaxEventCount;								//最大保存的推送事件个数。
    ZWP2P_UINT32  nRetryCount;								    //失败重传次数。此字段无效，协议重传逻辑现在均改为发送失败时若队列未满就一直发送
    ZWP2P_UINT32  nPushInterval;    							//最大推送的间隔，单位是秒。
    ZWP2P_UINT32  nLimitMode;    							    //限制模式。0 - 默认值，按推送事件的个数限制（兼容以前的逻辑，以前没有模式的概念，默认就是以数量做限制）；
                                                                //1 - 按推送事件的个数限制； 2 - 按内存大小限制； 3 - 自动模式（个数和内存哪个先到限定值就判断哪个）
    ZWP2P_UINT32  nMaxMemorySize;    							//最大内存大小。默认值：1024 * 1024

    char reserved[256];                                         // 保留

}ZWP2P_PUSH_P6SMSG_POLICY_PARAMS_V2,*LPZWP2P_PUSH_P6SMSG_POLICY_PARAMS_V2;

typedef struct tagZWP2P_PUSH_P6SMSG_SERVER_STATICS
{
    int nPushMode;                      // 事件推送模式。0 ： 不推图，1 ： 推图
    int nPushStatus;                    // 事件推送状态。0 ： 不能推送，1 ： 可以推送

    char szReserved[256];		        // 保留
}ZWP2P_PUSH_P6SMSG_SERVER_STATICS, *LPZWP2P_PUSH_P6SMSG_SERVER_STATICS;

#endif
