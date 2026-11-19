#ifndef HIE_SERVER_COMMON_H
#define HIE_SERVER_COMMON_H

#include <time.h>

//////////////////////////////////////////////////////////////////////////
//宏定义
//////////////////////////////////////////////////////////////////////////
//HieServer通用信息定义文件

//参数属性

/** @brief 入参																			*/
#define IN

/** @brief 出参																			*/
#define OUT

/** @brief 入出参																		*/
#define IN_OUT

/** @brief 实时流最大客户数																*/
#define REAL_STREAM_LIMIT					(8)

/** @brief 语音流最大客户端																*/
#define VOICE_STREAM_LIMIT					REAL_STREAM_LIMIT

/** @brief 用户名长度																	*/
#define USERNAME_LEN						(32)

/** @brief 日期时间长度																	*/
#define DATE_TIME_LEN						(15)

/** @brief 用户密码长度																	*/
#define USERPASS_LEN						(32)

/** @brief 超级用户密码长度																*/
#define SUPER_USERPASS_LEN					(128)

/** @brief 设备ID长度																	*/
#define MAX_DEVICE_ID_LEN					(32)
/** @brief 设备ID长度																	*/
#define MAX_DEVICE_ID_LEN_FOR_CLOUD			(64)

/** @brief IP地址长度(含结束符)															*/
#define IP_ADDRESS_LEN						(16)

/** @brief MAC地址长度(含结束符)														*/
#define MAC_ADDRESS_LEN						(18)

/** @brief MAC地址二进制长度															*/
#define MAC_BINARY_ADDRESS_LEN				(6)

/** @brief 版本信息长度(含结束符)														*/
#define VERSION_INFO_LEN					(20)

/** @brief 最大文件名长度																*/
#define MAX_FILENAME						(256)

/** @brief 域名长度(含结束符)															*/
#define DOMAIN_ADDRESS_LEN					(256)

/** @brief 地址长度(含结束符)																		*/
#define MAX_ADDRESS_LEN				(DOMAIN_ADDRESS_LEN)

/** @brief 默认保留字段个数																*/
#define DEF_RESERVE_NUM						(3)

/** @brief 配置信息最大长度											                    */
#define MAX_CFG_LEN_512BYTE					(512)

/** @brief 配置信息最大长度											                    */
#define MAX_CFG_LEN							(128 * 1024 * 4)

/** @brief 配置信息最大长度											                    */
#define MAX_CFG_LEN_256K					(128 * 1024 * 2)

/** @brief 配置信息最大长度											                    */
#define MAX_CFG_LEN_1K						(1 * 1024)

/** @brief 配置信息最大长度											                    */
#define MAX_CFG_LEN_8K						(8 * 1024)

/** @brief 配置信息最大长度											                    */
#define MAX_CFG_LEN_16K						(16 * 1024)

/** @brief 配置信息最大长度											                    */
#define MAX_CFG_LEN_32K						(32 * 1024)

/** \brief 透明传输数据最大长度												               			*/
#define MAX_TRANSPARENT_DATA_LEN			(128 * 1024 * 4)

/** @brief 描述信息最大长度											                    */
#define MAX_DESCRIPTION_INFO_LEN			(100)

/** @brief 扩展信令Method的最大长度															*/
#define MAX_METHOD_LEN						(32)

/** @brief 扩展信令Answer的最大长度															*/
#define MAX_ANSWER_LEN						(32)

/** @brief 扩展信令最大设备类型长度															*/
#define MAX_TYPE_LEN						(128)

/** @brief 扩展信令Usage的最大长度															*/
#define MAX_USAGE_LEN						(128)

/** @brief 扩展信息的最大长度																*/
#define MAX_EX_INFO_LEN						(32 * 1024)

/** @brief 错误描述信息的最大长度															*/
#define MAX_ERROE_MESSAGE_LEN				(256)

/** @brief Hxht日期时间长度																	*/
#define HXHT_DATA_TIME_LEN					(32)

/** @brief Hxht流类型长度																	*/
#define HXHT_STREAM_TYPE_LEN				(32)

/** @brief Hxht普通字符串长度																*/
#define HXHT_COMMON_STRING_LEN				(32)

/** @brief Hxht最大流时间信息个数															*/
#define MAX_STREAM_TIME_INFO_ITEN			(50)

/** @brief Hxht最大录像片段个数																*/
#define MAX_SEGMENT_ITEM					(50)

/** @brief Hxht最大预置点个数																*/
#define MAX_PRESET_POINT_NUM				(128)

/** @brief Hxht最大辅助开关个数																*/
#define MAX_AUX_SWITCH_NUM					(16)

/** @brief Hxht最大支持的云台控制命令号个数													*/
#define MAX_SUPPORT_OP_CMD_NUM				(64)

/** @brief Hxht最大十六进制485数据的长度													*/
#define MAX_485_DATA_LEN					(128)

/** @brief Hxht支持的最大能力查询数															*/
#define MAX_QUERY_SUPPORT_NUM				(9)

/** @brief Hxht最大功能名称长度																*/
#define MAX_SUPPORT_NAME_LEN				(128)

/** @brief Hxht最大时间计划节点数															*/
#define HXHT_MAX_ITEM_COUNT					(32)

/** @brief Hxht最大字符串长度																*/
#define HXHT_MAX_XML_STRING_LEN				(180)

/** @brief 单服务器最大解码通道数											*/
#define MAX_DEC_CHANNEL_NUMBER		(64) 

/** @brief 播放控制参数个数													*/
#define MAX_PLAY_CONTROL_PARAMETER	(5)

/** @brief 最大解码通道轮转视频源个数																			*/
#define MAX_DECODER_CHANNEL_SOURCE	(16)

/** @brief 屏幕控制参数个数													*/
#define MAX_SCREEN_CONTROL_PARAMETER	(5)

/** @brief 单服务器最大分屏数																		*/
#define MAX_DISPLAY_SCREEN_NUMBER	(64)

/** @brief LOGO图片参数个数													*/
#define MAX_LOGO_PICTURE_PARAMETER	(5)

/** @brief OSD文本字符串													*/
#define MAX_OSD_TEXT_STRING_LENGTH	(64)

/** @brief RTSP源字符串													*/
#define MAX_RTSP_SOURCE_STRING_LENGTH	(128)

/** @brief 设备版本名称长度												          	 */
#define DVR_MAX_DEVICE_VERSION_LEN				(32)

/** @brief P2P UUID 长度												          	 */
#define DVR_MAX_P2P_UUID_LEN				(32)

/** @brief 人脸脸库ID最大长度													          			*/
#define MAX_FACE_LIB_ID_LENGTH				(48)

/** @brief 人脸ID最大长度														          			*/
#define MAX_FACE_ID_LENGTH					(48)

/** @brief 人脸脸库名称最大长度												          				*/
#define MAX_FACE_LIB_NAME_LENGTH			(32)

/** @brief 人脸名称最大长度												          	 				*/
#define MAX_FACE_NAME_LENGTH				(32)

/** @brief 身份ID最大长度											          						*/
#define MAX_CERTIFICATE_ID_LENGTH			(32)

/** @brief 卡片ID最大长度												          					*/
#define MAX_CARD_ID_LENGTH					(32)

/** @brief 门禁远程控制ID最大长度												          			*/
#define MAX_GATE_REMOTE_ID_LENGTH			(32)

/** @brief 人脸脸库ID最大长度V2											          			*/
#define MAX_FACE_LIB_ID_LENGTH_V2			(64)

/** @brief 人脸ID最大长度V2														          			*/
#define MAX_FACE_ID_LENGTH_V2				(64)

/** @brief 人脸脸库名称最大长度V2												          				*/
#define MAX_FACE_LIB_NAME_LENGTH_V2			(64)

/** @brief 人脸名称最大长度V2												          	 				*/
#define MAX_FACE_NAME_LENGTH_V2				(64)

/** @brief 最大授权文件数量												          	 				*/
#define MAX_AUTH_FILE_COUNT				(32)

//////////////////////////////////////////////////////////////////////////
//历史流帧宏定义
//////////////////////////////////////////////////////////////////////////

/** @brief 历史流异常结束																*/
#define HISTORY_STREAM_EXCEPTION	(0x01000000)

/** @brief 历史流切换帧宏定义															*/
#define	HISTORY_STREAM_SWITCH_FRAME	(0x02000000)

/** @brief 历史流跳到新的时间片															*/
#define HISTORY_STREAM_TIME_JUMP	(0x03000000)

/** @brief 历史流结束帧宏定义															*/
#define	HISTORY_STREAM_TIME_END		(0x04000000)

/** @brief 历史流数据查询完毕															*/
#define HISTORY_STREAM_TIME_CURR	(0x05000000)

//////////////////////////////////////////////////////////////////////////
//远程事件类型
//////////////////////////////////////////////////////////////////////////

/** @brief 报警事件(子类型为eAlertType)																		*/
#define USEREVENT_ALARM_NOTICE			(0x00000001)

/** @brief 心跳丢失,网络断开															*/
#define USEREVENT_HEARTBEAT_LOST		(0x00000002)

/** @brief 网络重连成功																	*/
#define USEREVENT_NET_RECOVER			(0x00000004)

/** @brief 远程用户断开																	*/
#define USEREVENT_USER_DISCONNECT		(0x00000008)

/** @brief 远程流媒体断开																*/
#define USEREVENT_STREAM_DISCONNECT		(0x00000010)

/** @brief 硬盘组管理事件																*/
#define USEREVENT_DISKGROUP_MANAGE		(0x00000020)

/** @brief 历史流事件通知																*/
#define USEREVENT_HISTORYSTREAM_NOTICE	(0x00000040)

/** @brief 实时流启动连接ID通知															*/
#define USEREVENT_REALSTREAM_STARTLINK	(0x00000080)

/** @brief 实时流停止连接ID通知															*/
#define USEREVENT_REALSTREAM_STOPLINK	(0x00000100)

/** @brief 语音流启动连接ID通知															*/
#define USEREVENT_VOICESTREAM_STARTLINK	(0x00000200)

/** @brief 语音流停止连接ID通知															*/
#define USEREVENT_VOICESTREAM_STOPLINK	(0x00000400)

/** @brief 历史流销毁事件通知															*/
#define USEREVENT_HISTORYSTREAM_DESTROYLINK		(0X00000800)

/** @brief 历史流启动事件通知															*/
#define USEREVENT_HISTORYSTREAM_STARTLINK		(0X00001000)

/** @brief 历史流停止事件通知															*/
#define USEREVENT_HISTORYSTREAM_STOPLINK		(0X00002000)

/** @brief 历史流创建事件通知															*/
#define USEREVENT_HISTORYSTREAM_CREATELINK		(0X00004000)

/** @brief 设备配置修改事件通知（参数param1是配置的主命令，参数param2是配置的次命令）	*/
#define USEREVENT_DEVICE_CONFIG_CHANGED			(0X00008000)

/** \brief 智能推送事件（子类型见eEventAIType）											*/
#define USEREVENT_AI_EVENT_PUSH					(0X00010000)

/** \brief 中间层通知协议网络连接状态	（子类型见eEventNetType）							*/
#define USEREVENT_DEVICE_NET_CONNECT_STATUS		(0X00020000)

/** \brief 中间层通知协议九霄事件	（子类型见eEventMeglinkType）							*/
#define USEREVENT_MEGLINK_EVENT				(0X00040000)

/** \brief P2P通知FlowClient客户端登录		（子类型见eEventloginedType）						*/
#define USEREVENT_P2P_LOGIN_EVENT				(0X00080000)

/** \brief 中间层通知天翼协议日志上报							*/
#define USEREVENT_TIANYI_LOG_REPORT		 (0X00100000)

/** \brief 中间层通知协议天翼V2事件	（子类型见eEventTianYiV2Type）							*/
#define USEREVENT_TIANYIV2_EVENT				(0X00200000)

/** \brief 中间层通知狮子王事件	（子类型见eEventLionKingType）							*/
#define USEREVENT_LIONKING_EVENT				(0X00400000)

/** \brief 中间层通知二维码事件								*/
#define USEREVENT_QR_CODE_EVENT				(0X00800000)

/** \brief 中间层通知一键呼叫事件								*/
#define USEREVENT_TO_CALL_EVENT				(0X00900000)
// 在eAlertType枚举中新增设备休眠唤醒类型，仅用于HieServer_RefreshEventEx接口通知协议层台灯状态变更事件
#define USEREVENT_TO_LAMP_EVENT 			(0X00A00000)

//////////////////////////////////////////////////////////////////////////
//历史流录像类型宏定义
//////////////////////////////////////////////////////////////////////////
/** @brief 普通录像类型（录像的子类型是eAlertType，子类型只用于录像查询的返回接口，只用于区分20000以上的子类型，后续eAlertType新增子类型也是20000以上，其余都视作普通录像类型）																				*/
#define GENERAL_RECORD				(0X00000001)

/** @brief 手动录像类型																				*/
#define MANUAL_RECORD				(0X00000002)

/** @brief 报警录像类型																				*/
#define ALARM_RECORD				(0X00000004)

/** @brief 移动录像类型																				*/
#define MOTION_RECORD				(0X00000008)

/** \brief 智能事件录像类型	(录像的子类型是eEventAIType，子类型只用于录像查询返回结果，用于区分不同的AI录像类型)																			*/
#define AI_RECORD					(0X00000010)

/** \brief PIR录像类型																				*/
#define PIR_RECORD					(0X00000020)

/** @brief 所有录像类型																				*/
#define ALL_STREAM_MEDIA			(GENERAL_RECORD | MANUAL_RECORD | MOTION_RECORD | ALARM_RECORD|AI_RECORD|PIR_RECORD)


//////////////////////////////////////////////////////////////////////////
//基本类型
//////////////////////////////////////////////////////////////////////////

#if defined(__XRS_X64__)
typedef unsigned int	DWORD;
#else
typedef unsigned long	DWORD;
#endif
typedef unsigned long HIE_ULONG;
typedef unsigned short WORD;
typedef unsigned char BYTE;
typedef unsigned long VNS_USERDATA;
#ifndef HANDLE_DEFINED
#define HANDLE_DEFINED
typedef void *HANDLE;
#endif

//////////////////////////////////////////////////////////////////////////
//枚举
//////////////////////////////////////////////////////////////////////////

#ifndef DEF_ENUM_ALERT_TYPE
#define DEF_ENUM_ALERT_TYPE

/*!
*  @enum	eAlertType
*  @brief	报警和异常警告类型（普通事件子类型/普通录像子类型）
*/
typedef enum eAlertType
{
	ALERT_ALARMIN,					/*!< 报警输入                          (EventAlarmIn)         */
	ALERT_MOTION,					/*!< 移动侦测                                   */
	ALERT_ENCODEERROR,				/*!< 编码异常                                   */
	ALERT_DISKERROR,				/*!< 硬盘错误                                   */
	ALERT_DISKFULL,					/*!< 硬盘满                                     */
	ALERT_IPCONFLICT,				/*!< IP冲突                                     */
	ALERT_ILLEGEACCESS,				/*!< 非法访问                                   */
	ALERT_RETICLEDISCONNECT,		/*!< 网线断                                     */
	ALERT_VIDEOLOST,				/*!< 视频丢失                                   */
	ALERT_VIDEOEXCEPTION,			/*!< 视频异常                                   */
	ALERT_DISKGROUP_ERROR,			/*!< 盘组异常                                   */
	ALERT_IMODE,					/*!< 待商议(内部使用)                           */
	ALERT_IPCHANGED,				/*!< IP地址变化,用于发送HELLO消息(内部使用)     */
	ALERT_REMOTE_ALARMIN,			/*!< 远程报警输入                               */
	ALERT_HUMANSHAPE,				/*!< 人形检测                                   */
	ALERT_FACEDETECT,				/*!< 人脸检测                                   */
	ALERT_FACERECO,					/*!< 人脸识别                                   */
	ALERT_PIR,						/*!< PIR                              (EventPIR)     */
	ALERT_FACETEMP,					/*!< 人脸体温                                   */
	ALERT_FACEMASK,					/*!< 人脸口罩                                   */
	ALERT_TRIPWIRE,					/*!< 绊线检测                                   */
	ALERT_FENCEDETECT,				/*!< 电子围栏                                   */
	ALERT_HUMANCOUNT,				/*!< 客流统计                                   */
	ALERT_CAR_LICENSE,				/*!< 车牌识别                                   */
	ALERT_SOUNDDETECT,				/*!< 声音侦测                   (EventVoice)      */
	ALERT_HARDWARERESET,			/*!< 硬件复位                                   */
	ALERT_CAR,						/*!< 车形                                       */
	ALERT_ALARMOUT,					/*!< 报警输出事件(Onvif需要)                    */
	ALERT_ABNORMAL,			/*!< 设备异常                                   */
	ALERT_CHANNEL_STATES,		/*!< 通道状态变更（EventOVDChannelState）                                   */
	ALERT_IRCUT,					/*!< 光敏事件                                   */
	ALERT_OPENDOORBYKEY = 0x1000,	/*!< 按键开门信号                               */
	ALERT_OPENDOORBYPASSWD, 		/*!< 密码开门信号                               */
	ALERT_OPENDOORBYCARD,			/*!< 刷卡开门信号                                   */
	ALERT_OPENDOORBYQR,				/*!< 磁卡开门信号                               */
	ALERT_EBIKE_DETECT,				/*!< 电动车                                     */
	ALERT_BICICLE_DETECT,			/*!< 自行车                                     */
	ALERT_OFFDUTY_DETECT,			/*!< 离岗侦测                                   */
	ALERT_VEHICLE_DETECT,			/*!< 机动车侦测                                   */
	ALERT_REMOTE_UPDATE_NOTIFY,		/*!< 远程升级通知                                  */
	ALERT_FIRE_DETECT,				/*!< 火焰侦测(中间层内部使用)                      */
	ALERT_OPENDOORBYHEALTH,      	/*!< 健康检测开门信号           */
	ALERT_VIDEO_SHELTER,            /*!< 视频遮挡事件                      */
	ALERT_FALLING_DETECT,           /*!< 高空抛物                      */
	ALERT_INTELLIGENCE_TRACK,       /*!< 智能跟踪                      */
	ALERT_ANDLINK_UNBIND,           /*!< Andlink解绑                      */
	ALERT_REG_PEOPLE_ALARM,         /*!< 区域人数统计事件                      */
	ALERT_REG_PEOPLE_REPORT,        /*!< 区域人数统计上报                     */
	ALERT_TRANSGRESS_REPORT,        /*!< 越界侦测事件上报                     */
	ALERT_DEBUG_FILE,       		/*!< 调试文件上报（added by tb,转为AISDK使用，紧内部使用）  */
									/*!< 对应的buffer格式为：文件个数 + 结构体HY_DVR_FILE_GUIDE ..... + HY_DVR_FILE_GUIDE结构体描述的文件合集，总大小不能大于媒体发送buffer大小以及客户端接收buffer的大小 */

	//后续再新增普通事件的事件类型时，统一定义为20000以上的值，主要解决录像类型/事件类型的解耦
    ALERT_DEVICE_GOTO_SLEEP = 20000,      /*!< 设备进入休眠状态                     */
    ALERT_VIDEO_COVER,                    /*!< 视频遮挡事件（用于 T5AI）(EventVideoCover)                     */
    ALERT_HANGUP_VIDEO_CALL,              /*!< 视频通话挂断事件                     */
}eAlertType;


/*!
*  @enum	eEventNetType
*  @brief	网络状态事件
*/
typedef enum eEventNetType
{
	EVENT_NET_CONNECTED = 0,			/*!< 网络已连接                          	*/
	EVENT_NET_DISCONNECT,				/*!< 网络已断开					*/
	EVENT_NET_OVD_RECONFIG_NETWORK,			//OVD事件，二维码重新配网
}eEventNetType;

/*!
*  @enum	eEventloginedType
*  @brief	网络状态事件
*/
typedef enum eEventloginedType
{
	EVENT_P2P_CLIENT_LOGINED = 0,		/*!< 客户端已接入               */
	EVENT_P2P_CLIENT_LOGINOUT,				/*!< 客户端已断开					*/
}eEventloginedType;

/*!
*  @enum	eEventTianYiV2Type
*  @brief	天翼事件
*/
typedef enum eEventTianYiV2Type
{
	EVENT_TIANYIV2_REBOOT = 0,		/*!< 中间层打算重启设备   */
	EVENT_TIANYIV2_FACE_RECOGNITION,	/*!< 中间层给协议人脸识别通知  */
	EVENT_TIANYIV2_FACE_LABEL_INITED,   /*!< 中间层通知协议人脸数据库初始化完成  */
	EVENT_TIANYIV2_NO_MASK,           /*!< 中间层通知协议没有带口罩  */
	EVENT_TIANYIV2_BATTERY_BIKE,      /*!< 中间层通知协议电动车事件  */	
}eEventTianYiV2Type;

/*!
*  @enum	eEventTianYiV2Type
*  @brief	狮子王事件
*/
typedef enum eEventLionKingType
{
	EVENT_LIONKING_SET_GUARD = 0,		/*!< 布放   */
	EVENT_LIONKING_RESET_GUARD,				/*!< 撤防  */
	EVENT_LIONKING_ALARM,   				/*!< 报警  */
	EVENT_LIONKING_RESERVE,   				/*!< 预留  */
}eEventLionKingType;

/*!
*  @enum	eEventAIType
*  @brief	智能事件类型（（AI事件子类型/AI录像子类型））
*/
typedef enum eEventAIType
{
	EVENT_AI_PEOPLE_DETECT = 0,/*!< 人体检测事件                          	*/
	EVENT_AI_FACE_DETECT_LITE,	/*!< 基础版本人脸检测事件					*/
	EVENT_AI_FACE_DETECT,		/*!< 高阶版本人脸检测事件（EventFaceRecognition）*/
	EVENT_AI_FACE_RECOGNITION,	/*!< 人脸识别事件（EventFaceRecognition）   */
	EVENT_AI_FACE_RECOGNITION_V2,	/*!< 人脸识别事件V2（EventFaceRecognitionV2）   */
	EVENT_AI_FACE_RECOGNITION_V3,	/*!< 人脸识别事件V3 适配济工网平台*/
	EVENT_AI_PEOPLE_DETECT_OVD,		//CMOVD人形事件（EventAiPeople）
	EVENT_AI_PANORAMIC_SNAPSHOT,    //全景抓拍图片事件
	EVENT_AI_OUT_OF_BOUND,			//越界侦测（使用 EventCrossBorderDetect, P6SEvent使用 EventP6SEventCrossBorderDetect,通过结构体EventCrossBorderDetect的uDataPacketStartCode参数传递 ）
	EVENT_AI_LEAVE_POST,				//离岗侦测（使用 EventOffDutyDetect, P6SEvent使用 EventP6SEventOffDutyDetect,通过结构体EventOffDutyDetect的uDataPacketStartCode参数传递 ）
	EVENT_AI_ATTENDANCE_OPERAT_TYPE,	//考勤操作类型（EventAttendanceOperatType）
	EVENT_AI_EXPORT_ATTENDANCE_RECORD,	//导出考勤记录（EventExportAttendanceRecord）
	EVENT_AI_FIRE_DETECT = 12,			//火焰侦测(EventFireDetectInfo)
	EVENT_AI_PEOPLE_DETECT_MESSAGEPUSH,		//云消息人形事件（EventAiMessagePushPeople）	
	EVENT_AI_PERSON_LEAVE,					//人员离开事件(EventPersonLeave)
	EVENT_AI_DEVICE_SLEEP,					//睡眠模式事件（EventDevSleepState）	
	EVENT_AI_POISON_AIR,					//毒害气体报警事件（EventPoisonAir）	
	EVENT_AI_DEVICE_KEY2CALL,				//一键呼叫事件(EventKey2Call)
	EVENT_AI_HEALTHCODE_INFO,				/*!< 健康码信息事件(EventHealthCodeInfo)*/
	EVENT_AI_INTELLIGENCE_TRACK,			/*!< 智能跟踪事件(EventIntelligenceTrack)*/
	EVENT_AI_FACE_RECOGNITION_V4,			/*!< 人脸识别事件V4 (适配人脸前抓前比功能 EventFaceRecognitionV4)*/
	EVENT_AI_BREATH_MASK,					/*!< 口罩事件 (适配人脸前抓前比功能 EventBreathMask)*/
	EVENT_AI_DAJIANGIOT_UNLOCK_RECORD,   	/*!< 开锁记录事件(大匠智慧社区项目定制事件,结构体EventDaJiangIOTUnlockRecord)*/
	EVENT_AI_PERSON_PASS_EXPIRE,   			/*!< 人员通行失效事件(结构体 EventPersonPassExpire)*/
	EVENT_AI_CMDOT_FACE_RECOGNITION,	    /*!< 移动人脸识别事件（HY_DVR_CMDOT_FACE_RECO_INFO）   */	

	EVENT_GATE_FACERECOGNITION_OPEN = 100,	/*!< 人脸比对开门事件（EventFaceRecognition）*/
	EVENT_GATE_KEYBOARD_OPEN,				/*!< 密码开门（EventGateKeyboard）		*/
	EVENT_GATE_CARD_OPEN,					/*!< 刷卡开门（EventGateCard）			*/
	EVENT_GATE_BUTTON_OPEN,					/*!< 按键开门（无）						*/
	EVENT_GATE_REMOTE_OPEN,					/*!< 远程开门（EventGateRemote）		*/
	EVENT_GATE_CARD_OPEN_WIEGAND,			/*!< 面板机韦根刷卡开门（EventGateCardV2）		*/
	EVENT_GATE_REMOTE_OPEN_CGI,				/*!< 远程开门CGI（EventGateRemote）		*/

	EVENT_GATEWAY_CHECKIN = 200,			/*!< 打卡		*/
	EVENT_GATEWAY_PEOPLE_RECORD,			/*!< 人脸录入		*/

	EVENT_MOTION_EVENT = 300,				// 移动侦测（带抓拍图）					(EventAiMotion) 

	EVENT_MOTION_FENCE_DETECT = 400,		/*!< 电子围栏事件  （ EventFenceDetectAlarm(电信天翼使用该结构体)  标准版本使用 EventElectronicFence, P6SEvent使用EventP6SEventElectronicDefence,通过事件通知接口的param3参数传递*/
	EVENT_MOTION_HUMCOUNT = 500,			/*!< 客流统计  （EventHumCount）*/
	EVENT_CAR_LICENSE_RECOGNITION = 600,	/*!< 车牌识别  （EventCarLicenseRecognition）*/
	EVENT_CAR_DETECT,						/*!< 车形识别 */
	EVENT_TRAFFIC_STATISTICS,				/*!< 车流统计  (EventTrafficStatistics) */

	EVENT_NFC_CARD_DETECT = 700,			/*!< 刷NFC卡 */
	EVENT_VEHICLE_DETECT = 800,				/*!< 机动车检测 */
	EVENT_PASSENGER_FLOW_STAT_PEOPLE = 850,	  /*!< 客流统计为人形 EventAIPassengerFlowStatPeople*/
	EVENT_PASSENGER_FLOW_STAT_PASSENGER = 851,/*!< 客流统计为客流 EventAIPassengerFlowStatPassenger*/
	EVENT_AI_FACE_RECOGNITION_FOR_OVDSDK,   /*!< 人脸识别(HY_DVR_OVDSDK_FACE_INFO_LIST) */
	EVENT_CAR_LICENSE_RECOGNITION_FOR_OVDSDK,   /*!< 车牌识别 (HY_DVR_OVDSDK_VEHICLE_INFO_LIST)*/
	EVENT_NON_CAR_FOR_OVDSDK,   				/*!< 非机动车上传 (HY_DVR_OVDSDK_NONMOTOR_VEHICLE_INFO_LIST)*/
	EVENT_PASSENGER_FLOW_FOR_OVDSDK,        /*!< 客流统计 (HY_DVR_OVDSDK_PASSENGER_FLOW_INFO_LIST)*/
	EVENT_REGIONAL_PEOPLE_FOR_OVDSDK,        /*!< 区域人数统计 (HY_DVR_OVDSDK_REGIONAL_PEOPLE_INFO_LIST)*/
	EVENT_AI_FACE_UNMASK_FOR_OVDSDK,            /*人脸口罩事件(HY_DVR_OVDSDK_FACE_INFO_LIST)*/
	EVENT_REGIONAL_PEOPLE_ALARM_FOR_OVDSDK,        /*!< 区域人数事件 (HY_DVR_OVDSDK_REGIONAL_PEOPLE_ALARM)*/
    EVENT_TRANSGRESS_ALARM_FOR_OVDSDK,           /*!< 越界侦测事件 (HY_DVR_OVDSDK_TRANSGRESS_ALARM)*/
    EVENT_OFF_DUTY_ALARM_FOR_OVDSDK,           /*!< 离岗检测事件 (HY_DVR_OVDSDK_OFF_DUTY_ALARM)*/
	EVENT_AERA_ALERT_ALARM_FOR_OVDSDK,           /*!< 区域入侵事件 (HY_DVR_OVDSDK_AERA_ALERT_ALARM)*/
	EVENT_OFF_DUTY_SEND1400_FOR_OVDSDK,    /*!< 离岗上报1400事件 (HY_DVR_OVDSDK_OFF_DUTY_ALARM_V2)*/
	EVENT_OFF_DUTY_FOR_OVDSDK,           /*!< 离岗检测上报 (HY_DVR_OVDSDK_OFF_DUTY_INFO_LIST)*/
	EVENT_TRAFFIC_STATISTICS_CARSHAPE,       /*!< 车流统计车形事件  (EventTrafficStatisticsCarShape) */
	
	EVENT_FALLING_OBJECT_DETECT = 900,	   // 高空抛物（带抓拍图）					(EventFallingObjectDetectAlarm)
	EVENT_ALERT_VIDEO_SHELTER = 950,            /*!< 视频遮挡事件 (EventVideoShade)                   */
	EVENT_AI_DEVICE_KEY2CALL_FOR_VOIP,		//一键呼叫，移动运营商voip专用
	
	EVENT_AI_EX_CRYING_DETECT = 10100,				/*!< 哭声检测检测事件  (EventAICryScreamDetect)                        	*/
	EVENT_AI_EX_OLD_MAN_FALL_DETECT,				/*!< 老人跌倒检测事件                          	*/
	EVENT_AI_EX_PET_DETECT,				   			/*!< 宠物检测事件                          		*/
	EVENT_CAR_LICENSE_SNAPSHOT,						/*!< 车牌抓拍事件（通用事件，非运营商） （tagEventCarLicenseSnapshotRecognition） */
	EVENT_AI_EX_PET_SNAPSHOT = 10105,				/*!< 宠物抓拍事件(对应结构体 EventAIExPetSnapshotR2, R2型号产品, 运营商项目) */
	EVENT_AI_EX_SHUTDOWN_ALARM,						/*!< 关机告警事件(EventShutDownAlarm) */
	EVENT_AI_EX_LOW_BATTERY_TEN_PERSENT_ALARM,		/*!< 低电量百分之10事件(EventLowBatteryTenPercent) */
	EVENT_AI_EX_LOW_BATTERY_TWENTY_PERSENT_ALARM,	/*!< 低电量百分之20事件(EventLowBatteryTwentyPercent) */
	EVENT_AI_EX_RECHARGE_FAIL_ALARM,				/*!< 回充失败事件(EventRechargeFail) */
	EVENT_AI_EX_STRANGE_FACE,						/*!< 陌生人脸事件(对应结构体 EventFaceRecognitionV4, T5AI首次使用（刘海林）) */
	EVENT_AI_EX_FACE_COMPARISON,					/*!< 人脸对比事件(对应结构体 EventFaceRecognitionV4, T5AI首次使用（刘海林）) */
	EVENT_CHANNEL_OFFLINE,							/*!< 通道掉线事件（NVR专用事件，IPC不用关注） */
	EVENT_RECORD_DATA_LOST,							/*!< 录像数据丢失事件（NVR专用事件，IPC不用关注） */
	EVENT_DISK_FAULT,								/*!< 磁盘故障事件（NVR专用事件，IPC不用关注） */
	EVENT_AI_EX_FACE_COMPARISON_ALL,				/*!< 人脸对比事件(包含了陌生人和脸库的人)(对应结构体 EventFaceRecognitionV4, T5AI首次使用（刘海林）) */
	EVENT_AI_URGENT_ASSISTANCE,						/*!< 紧急求助(对应结构体 无, P5A首次使用（张辉）) */
	EVENT_AI_FLAME_ALARM,							/*!< 火焰报警(包含了陌生人和脸库的人)(对应结构体 无, P5A首次使用（张辉）) */
    EVENT_AI_TIMED_SNAPSHOT,                        /*!< 定时抓拍事件（P6SEvent使用EventP6SEventTimedSnapshot,通过事件通知接口的param3参数传递）*/
    EVENT_AI_GROUND_LOCK_STATUS_CHANGED,            /*!< 地锁状态变化事件（P6SEvent使用EventP6SEventGroundLockStatusChanged,通过事件通知接口的param3参数传递）*/
    EVENT_AI_STARTUP_REPORT,                        /*!< 开机上报事件（P6SEvent使用EventP6SEventStartupReport,通过事件通知接口的param3参数传递）*/
    EVENT_AI_ABNORMAL_EVENT,                        /*!< 异常事件（P6SEvent使用EventP6SEventAbnormalEvent,通过事件通知接口的param3参数传递）*/
    EVENT_AI_NON_WHITELIST_CAR_EVENT,               /*!< 非白名单车辆事件（P6SEvent使用EventP6SEventNonWhitelistCarEvent,通过事件通知接口的param3参数传递）*/
	EVENT_AI_POWERON_INFO_REPORT_EVENT,             /*!< 上电信息上报事件 (首次使用XT5N)*/ 
	EVENT_AI_CPU_PERSISTENTLY_HIGH_EVENT,           /*!< CPU持续高事件 (NVR专用事件，IPC不用关注)*/ 
	EVENT_AI_DISK_IO_KEEP_HIGH_EVENT,       		/*!< 硬盘IO持续高事件 (NVR专用事件，IPC不用关注)*/ 
	EVENT_AI_WIDEBAND_OVERRUN_EVENT,       			/*!< 宽带超限事件 (NVR专用事件，IPC不用关注)*/ 
	EVENT_AI_CAR_LICENSE_COMPARE_EVENT,       		/*!< 车牌对比事件 (暂时只有NVR使用)*/ 
    EVENT_AI_AUTH_REQUEST,                          /*!< 鉴权请求事件（丁丁地锁）（P6SEvent使用EventP6SEventAuthRequest,通过事件通知接口的param3参数传递）*/
    EVENT_AI_DEVICE_STATUS_REPORT,                  /*!< 设备运行状态事件（丁丁地锁）（P6SEvent使用EventP6SEventDeviceStatusReport,通过事件通知接口的param3参数传递）*/
    EVENT_AI_CAR_PLATE_RECOGNITION,                 /*!< 车牌识别事件（丁丁地锁）（P6SEvent使用EventP6SEventCarPlateRecognition,通过事件通知接口的param3参数传递）*/
    EVENT_AI_ACTIVE_SNAPSHOT,                       /*!< 主动抓拍事件（丁丁地锁）（P6SEvent使用EventP6SEventActiveSnapshot,通过事件通知接口的param3参数传递）*/
    EVENT_AI_LICENSE_PLATE_SURVEILLANCE_EVENT,      /*!< 车牌布控事件(T5AI首次首次使用，对接人是钟奎)（通用型事件，使用EventLicensePlateSurveillance结构体）*/
    EVENT_AI_ELECTRIC_BIKE,                         /*!< 电瓶车事件(T5AI首次使用)（通用型事件，使用EventElectricBike结构体）*/
    EVENT_AI_GAS_CONTAINER,                         /*!< 煤气罐事件(T5AI首次使用)（通用型事件，使用EventGasContainer结构体）*/
    EVENT_AI_STOP_KEY2CALL,                         /*!< 挂断一键呼叫(W42U首次使用)（通用型事件，无对应结构体）*/
	EVENT_AI_ELECTRONIC_DEFENCE_V2,					/*!< 电子围栏事件V2  (通用EventElectronicDefenceV2) */
	EVENT_AI_PEOPLE_NUMBER_OVERLIMIT,				/*!< 人数超限报警事件(通用 EventAIPeopleNumberOverLimit) */
	EVENT_AI_PEOPLE_NUMBER_STATISTICS,				/*!< 人数统计事件（周期性上报）(通用 EventAIPeopleNumberStatistics) */
}eEventAIType;

typedef enum eEventMeglinkType
{
	EVENT_MEGLINK_STORAGE_OVER_90_PERCENT,		//存储空间超过90%
	EVENT_MEGLINK_STORAGE_OVER_90_PERCENT_CANCEL,		//存储空间超过90%解除
	EVENT_MEGLINK_DISMANTLE_ALERT,						//防拆报警
	EVENT_MEGLINK_DISMANTLE_ALERT_CANCLE,		//防拆报警解除
	EVENT_MEGLINK_PASSWORD_CRACK,					//密码破解报警
	EVENT_MEGLINK_PASSWORD_CRACK_CANCLE,		//密码破解报警接触
	EVENT_MEGLINK_DOOR_OPEN_TIMEOUT,				//门禁开放超时报警
	EVENT_MEGLINK_DOOR_OPEN_TIMEOUT_CANCLE,	//门禁开放超时报警解除
	EVENT_MEGLINK_TEMPERATURE_MODULE_OFFLINE,		//测温模块离线报警
	EVENT_MEGLINK_TEMPERATURE_MODULE_OFFLINE_CANCLE,		//测温模块重新上线
	EVENT_MEGLINK_ALARM_IN,			//报警输入报警
	EVENT_MEGLINK_FACE_FEATURE_ANALYSIS_FAIL,		//人脸特征抽取失败
	EVENT_MEGLINK_FIRE_ALERT,			//消防报警
	EVENT_MEGLINK_FEATURE_UPLOAD,			//特征上报
	EVENT_MEGLINK_RECOGNIZE_RECORD_UPLOAD,		//识别记录上报
}eEventMeglinkType;

#endif//DEF_ENUM_ALERT_TYPE

//////////////////////////////////////////////////////////////////////////
//协议给中间层通知事件类型
//////////////////////////////////////////////////////////////////////////
//九霄
/** @brief 九霄OTA升级信息															*/
#define PROTOCOL_EVENT_MEGLINK_OTA						1
/** @brief 九霄配置同步信息															*/
#define PROTOCOL_EVENT_MEGLINK_CONFIG_SYNC		2
/** @brief 九霄服务器连接状态															*/
#define PROTOCOL_EVENT_MEGLINK_SERVER_CONNECTION_STATE		3
/** @brief 九霄人员同步信息															*/
#define PROTOCOL_EVENT_MEGLINK_PERSON_SYNC		4

//天翼
#define PROTOCOL_EVENT_TIANYI_STREAM_CONFIG			5

//移动OVD
#define PROTOCOL_EVENT_CMCC_OVD_CONNECT_STATUS	6
#define PROTOCOL_EVENT_CMCC_OVD_KEY_TO_CAll_STATUS	7


/*!
*  @enum	eProtocolID
*  @brief	协议类型
*/
typedef enum eProtocolID
{
	eProtocolBegin = -1,	//协议开始

	eProtocolHYPrivate,		//私有协议
	eProtocolTideMobile,	//TideMobile手机协议
	eProtocolMegaEyes,		//互信互通（全球眼）
	eProtocolRTSP,          //RTSP协议
	eProtocolOnvif,         //Onvif协议
	eProtocolTUTK,			// TUTK协议
	eProtocolJSTP,			// JSTP协议
	eProtocolCGI,			//CGI协议
	eProtocolGooLink,		//GooLink
	eProtocolCloudStorage,	//云存储协议
	eProtocolLongSee,	//长视协议适配层
	eProtocolMqtt,		//MQTT协议
	eProtocolWXGateway,		//微信考勤机协议
	eProtocolTianYi,		//天翼看家
	eProtocolMegLink,	//旷视九霄
	eProtocolFlowClient,		//金睿协议
	eProtocolGB28181,		//GB28181
	eProtocolCMDot,			//中国移动
	eProtocolTianYiV2,		//天翼2.0看家
	eProtocolJiGongPlatform,	//济工网平台
	eProtocolLionKing,		//LionKing
	eProtocolCMCC_OVD,		//CMCC_OVD
	eProtocolCUIOT_LTYF,	//ZW_CUIOT_LTYF
	eProtocolZWebSocket,		//ZWebSocket
	eProtocolP6SMessagePush,	//P6SMessagePush	
	eProtocolCloudStorageWorld,		//国际云存储
	eProtocolCUDot,			//联通协议
	eProtocolHealthCode,	//健康码协议
	eProtocolDaJiangIOT,	//大匠定制协议
	eProtocolP6SEvent,		//P6SEvent协议
    eProtocolHoloLink,      //HoloLink协议
	eProtocolRTMP,      //rtmp协议
	eProtocolRtmpRegister,  //rtmp register协议
	eProtocolP6SCMS,		//P6SCMS协议
	eProtocolGA1400,		//GA1400协议
	eProtocolEnd,			//协议结束
}eProtocolID;

/*!
*  @enum	eErrorValue
*  @brief	错误值类型（建议添加错误码时，在最后面添加）
*/
typedef enum eErrorValue
{
	eErrorNone = 0,				/*!< 无错误                                 */
	eErrorFailed,				/*!< 失败                                   */
	eErrorNoInitializeSDK,		/*!< 未初始化SDK                            */
	eErrorHandle,				/*!< 句柄错误                               */
	eErrorParameter,			/*!< 参数错误                               */
	eErrorMemory,				/*!< 内存错误                               */
	eErrorSystemFailed,			/*!< 操作系统错误                           */
	eErrorNoIdleResource,		/*!< 无可用资源                             */
	eErrorTimeOut,				/*!< 超时错误                               */
	eErrorFunctionVersionLow,	/*!< 功能版本低于服务器                     */
	eErrorFunctionVersionHigh,	/*!< 功能版本高于服务器                     */
	eErrorTaskNoRun,			/*!< 任务未启动                             */
	eErrorAlreadyRun,			/*!< 重复启动                               */
	eErrorConnectFailed,		/*!< 连接失败                               */
	eErrorSessionDisconnect,	/*!< 连接断开                               */
	eErrorCommandSendFailed,	/*!< 命令发送失败                           */
	eErrorServerReject,			/*!< 服务器拒绝                             */
	eErrorInterfaceUnset,		/*!< 接口未设置                             */
	eErrorHYPrivate,			/*!< 私有协议库不存在                       */
	eErrorMegaEyes,				/*!< 全球眼协议库不存在                     */
	eErrorFunction,				/*!< 协议中函数不存在                       */

	eErrorUserName,				/*!< 用户名错误                             */
	eErrorUserPass,				/*!< 用户密码错误                           */
	eErrorIPLimited,			/*!< 用户IP限制                             */
	eErrorMACLimited,			/*!< 用MAC限制                              */
	eErrorUserNumOverflow,		/*!< 登陆用户过多                           */
	eErrorUserHeartBeat,		/*!< 用户心跳订阅失败                       */
	eErrorGetConfigurationPort,	/*!< 登陆时获取端口配置信息失败             */
	eErrorServiceReseting,		/*!< 网络服务重启中                         */


	eErrorHistoryStreamCreate,	/*!< 历史流创建的指针为NULL					*/
	eErrorGetDataSize,			/*!< 获取历史数据大小的指针为NULL			*/
	eErrorHistoryQueryCreate,	/*!< 历史查询创建的指针为NULL				*/
	eErrorHistoryQueryNext,		/*!< 历史查询下一条的指针为NULL				*/
	eErrorTideMobile,			/*!< TideMobile协议不存在					*/
	eErrorHxht,					/*!< MegaEyes协议不存在(Hxht是该协议公司的名字)	*/
	eErrorRTSP,					/*!< RTSP协议不存在                     	*/
	eErrorOnvif,				/*!< Onvif协议不存在                     	*/
	eErrorImplement,			/*!< Implement协议不存在                     	*/
	eErrorNoSupportCommand,		/*!< 不支持的命令	                        */
	eErrorUnserialize,			/*!< 反序列化失败							*/
	eErrorCodecTypeChanged,		/*!< 编码算法变化,应该主动断开连接，以便客户端快速重连	*/

	eErrorUnsupportSubstreamPlayback,		/*!< 不支持子码流进行回放		*/
	eNeedResetUserPass,			/*!< 不支持空密码，需要重新设置密码         */

	eErrorLongSee,		//加载长视协议库失败
	eErrorWXGateway,	//微信考勤协议加载失败

	eErrorNotSupport,		//私有协议现在分为标准版和small版，small版的某些功能被屏蔽，如果尝试进行这些功能的操作会返回该错误码
	eErrorWeakPassword,		//弱密码
	eErrorUserHasBeenDisabled,		//用户被禁用

    eErrorUnsupportedLanguageType,      //不支持的语言类型
    eErrorLanguageTypeIsCustomized,     //语言类型被定制

	eErrorP2PFailed = -1859583999, 			/*!< 执行失败 */
	eErrorP2PChargeLessThan10 = -1859583998, 	/*!<电量低于10% */
	eErrorP2PChargeLessThan20 = -1859583997, 	/*!< 电量低于20% */
	eErrorP2PFunctionInUse = -1859583996, 		/*!< 功能正在使用 */
	eErrorP2PDeviceIsUpgrading = -1859583995, /*!< 设备正在升级 */
	eErrorCGINightVisionConfigError = -1858535324, /*!< 夜视配置设置不生效 */

	eErrorCGIMidCertFileCountError = -1858600933,	/* 上传证书—上传文件个数不对 */
	eErrorCGIMidCertPrivateDERFailed = -1858535403,	/* 创建证书—私钥DER导出失败 */
	eErrorCGIMidCertDERFileGenFailed = -1858535402,	/* 创建证书—DER格式证书生成失败 */
	eErrorCGIMidCertPEMFileGenFailed = -1858535401, /* 创建证书—PEM格式证书生成失败 */
	eErrorCGIMidCertPrivateKeyGenFailed = -1858535400, /* 创建证书—私钥生成失败 */
	eErrorCGIMidCertIPNotMatch = -1858535399,	/* 创建证书/上传证书—输入IP和设备IP不匹配 */
	eErrorCGIMidCertFileTypeError = -1858535398,	/* 上传证书—文件格式不对 */
	eErrorCGIMidPortIsUsedError = -1858535396,	/* 修改端口--端口配占用 */

	eZWP2P_ERR_FAILED_SCODE_VERIFY = -1876426751, // scode验证错误

    eErrorP6SEventGroundlockOperationFail = -1847000988, /*!< 地锁操作失败 */
	eErrorGroundlockOperationing = -1847000987, /*!< 地锁操作中 */

	eErrorActiveFileAndDeviceIsNotMatch = -1858535423, /*!< 激活文件与设备不匹配 */

	eErrorCGIMidActiveDIDIsInvalid = -1858535422,/*!< 激活了4G流量运营，但是did不是“运营”号段 */
	eErrorCGIMidActiveCodeParseFailed = -1858535421,/*!< 激活码解码失败 */
	eErrorCGIMidActiveFailed = -1858535420,/*!< 激活失败（因设备原因未成功） */
	eErrorCGIMidActiveDeviceNotSupportSomeFunc = -1858535419,/*!< 设备不支持激活某个功能 */
	eErrorCGIMidActiveCodeTooLong = -1858535418,/*!< 激活码长度过长	 */
	eErrorCGIMidBasicVersionNotMatch = -1858535417,/*!< 使用CGI升级时，固件会检测版本号，提示基础版本不匹配错误 */
	eErrorCGIMidAuthCodeTooLong = -1858535416,/*!< 授权时授权码过长错误 */
	eErrorCGIMidNotSupportSwitchSimCard = -1858535415,/*!< 不支持切卡 */
	eErrorCGIMidSwitchingSimCard = -1858535414,/*!< 正在切卡中 */
	eErrorCGIMidSwitchSimCardParamInvalid = -1858535413,/*!< 切卡参数无效 */
	eErrorCGIMid4GStatusNotAllowSwitchSimCard = -1858535412,/*!< 4G状态不满足切卡 */

	eErrorCommonPasswordLacksRequiredFields  = -1812398064, // 密码缺少必含字段
	eErrorCommonPasswordInvalidString  = -1812398065, // 密码包含不合法字符串
	eErrorCommonPasswordLenInvalid = -1812398066, // 密码长度不合法
	eErrorCommonBasicVersionNotMatch = -1812398067, /*!< 进行升级时，固件会检测版本号，提示基础版本不匹配错误 */
    eErrorP2PMidDevIsBusy = -1859583971, /*!< 视频通话/语音对讲--设备忙线中 */

}eErrorValue;

/*!
*  @enum	eConfigureType
*  @brief	配置信息类型
*/
typedef enum eConfigureType
{
	eConfigureTypeBegin = 0,	/*!< 起始值                                 */
	eConfigureDevice,			/*!< 设备信息                               */
	eConfigureNetWork,			/*!< 网络信息                               */
	eConfigureManagerServer,	/*!< 管理服务器信息                         */
	eConfigureTime,				/*!< 时间信息                               */
	eConfigureChannel,			/*!< 通道信息                               */
	eConfigureCompression,		/*!< 压缩信息                               */
	eConfigureTypeEnd,			/*!< 结束值                                 */
}eConfigureType;

/*!
*  @enum	eGetServerStateType
*  @brief	获取服务状态类型
*/
typedef enum eGetServerStateType
{
	eGetServerStateTypeBegin = 0,	/*!< 起始值                                 */
	eRealStreamState,			/*!< 实时流状态                             */
	eRemoteUser,				/*!< 用户状态                               */
	eHistroyState,				/*!< 历史流状态                             */
	eGetServerStateTypeEnd,		/*!< 结束值                                 */
}eGetServerStateType;

/*!
*  @enum	eManagerServer
*  @brief	管理服务器类型
*/
typedef enum eManagerServerType
{
	eManagerServerTypeBegin = 0,	/*!< 起始值                                 */
	eManagerCenter,				/*!< 中心管理服务器                         */
	eManagerMediaStream,		/*!< 流媒体服务器                           */
	eManagerStorage,			/*!< 存储服务器                             */
	eManagerAlarm,				/*!< 报警服务器                             */
	eManagerServerTypeEnd,		/*!< 结束值                                 */
}eManagerServerType;

/*!
*  @enum	eRemoteUserState
*  @brief	用户状态类型
*/
typedef enum eRemoteUserState
{
	eRemoteUserStateBegin = 0,	/*!< 起始值                                 */
	eUserTimeOut,				/*!< 超时(心跳超时)                         */
	eUserNetWorkError,			/*!< 连接异常                               */
	eRemoteUserStateEnd,		/*!< 结束值                                 */
}eRemoteUserState;

/*!
*  @enum	eMediaCodeType
*  @brief	流媒体码流类型
*/
typedef enum eMediaCodeType
{
	eMediaCodeTypeBegin = 0,	/*!< 起始值                                 */
	eMediaCodeMain,				/*!< 主码流									*/
	eMediaCodeAssist,			/*!< 辅码流			                        */
	eMediaCodeHDAssist,			/*!< 高清辅码流			                    */
	eMediaCodefourthStream,		/*!< 第四码流								*/
	eMediaCodeTenStream = 10,	/*!< 第10码流(远程桌面码流)					*/
	eMediaCodeTypeEnd,			/*!< 结束值                                 */
}eMediaCodeType;

/*!
*  @enum	eStreamMediaType
*  @brief	流媒体类型
*/
typedef enum eStreamMediaType
{
	eStreamMediaTypeBegin = 0,	/*!< 起始值                                 */
	eMainVideoAndSound,			/*!< 主码流音视频                           */
	eMainVideo,					/*!< 主码流视频                             */
	eMainSound,					/*!< 主码流音频                             */
	eAssistVideo,				/*!< 辅码流视频                             */
	eAssistHDVideo,				/*!< 高清子码流视频							*/
	eRemoteDesktopVideo,		/*!< 远程桌面流								*/
	eStreamMediaTypeEnd,		/*!< 结束值                                 */
}eStreamMediaType;


/*!
*  @enum	eVideoCallMediaType
*  @brief	视频通话APP->Device流媒体类型
*/
typedef enum eVideoCallMediaType
{
	eVideoCallSMediaTypeBegin = 0,				/*!< 起始值                                	*/
	eVideoCallMediaTypeVideoAndAudio,			/*!< 视频流和音视流                          */
	eVideoCallMediaTypeVideo,					/*!< 视频流                             	*/
	eVideoCallMediaTypeAudio,					/*!< 音频流                          */
	eVideoCallSMediaTypeEnd
}eVideoCallMediaType;

/*!
*  @enum	eHistoryStreamType
*  @brief	历史流类型
*/
typedef enum eHistoryStreamType
{
	eHistoryStreamTypeBegin = 0,	/*!< 起始值                                 */
	eAllStreamMedia,			/*!< 所有流媒体                             */
	eGeneralRecord,				/*!< 普通录像                               */
	eManualRecord,				/*!< 手动录像                               */
	eMotionRecord,				/*!< 移动录像                               */
	eAlarmRecord,				/*!< 报警录像                               */
	eAIRecord,					/*!< 智能录像                               */
	ePIRRecord,					/*!< PIR录像                               */
	eHistoryStreamTypeEnd,		/*!< 结束值                                 */
}eHistoryStreamType;

/*!
*  @enum	eStreamActionType
*  @brief	流事件类型
*/
typedef enum eStreamActionType
{
	eStreamActionTypeBegin = 0,	/*!< 起始值                                 */
	eRequestConnect,			/*!< 请求连接                               */
	eRequestDisConnect,			/*!< 请求断开                               */
	eConnected,					/*!< 已连接                                 */
	eDisconnected,				/*!< 已断开                                 */
	eStreamActionTypeEnd,		/*!< 结束值                                 */
}eStreamActionType;

/*!
*  @enum	eRemoteDownloadState
*  @brief	远程下载状态
*/
typedef enum eRemoteDownloadState
{
	eRemoteDownloadStateBegin = 0,	/*!< 起始值                                 */
	eTransferOver,				/*!< 文件传输结束                           */
	eTransfer,					/*!< 文件传输中                             */
	eRemoteDownloadStateEnd,	/*!< 结束值                                 */
}eRemoteDownloadState;

/*!
*  @enum	eTransferSpeed
*  @brief	数据传输速度类型
*/
typedef enum eTransferSpeed
{
	eTransferSpeedBegin = 0,	/*!< 起始值                                 */
	eQuickSpeed,				/*!< 最快速度                               */
	eNormalSpeed,				/*!< 普通速度                               */
	eSlowSpeed,					/*!< 较慢速度                               */
	eTransferSpeedEnd,			/*!< 结束值                                 */
}eTransferSpeed;

/*!
*  @enum	eDetectAction
*  @brief	远程探测事件类型
*/
typedef enum eDetectAction
{
	eDetectActionBegin = 0,		/*!< 起始值                                 */
	eDetectGetCfg,				/*!< 获取配置信息                           */
	eDetectSetCfg,				/*!< 设置配置信息                           */
	eDetectActionEnd,			/*!< 结束值                                 */
}eDetectAction;

/*!
*  @enum	eFileLockOperationCode
*  @brief	文件锁操作
*/
typedef enum eFileLockOperationCode
{
	eUnlockFile = 0,			/*!< 解锁                                   */
	eLockFile,					/*!< 开锁                                   */
}eFileLockOperationCode;


/*!
*  @enum	eImageFormat
*  @brief	抓图文件格式
*/
typedef enum eImageFormat
{
	eImageFormatBegin = 0,		/*!< 起始值                                 */
	eBmpFormat,					/*!< 位图格式                               */
	eJpgFormat,					/*!< jpg格式                                */
	eJpgFormatV2,				/*!< jpgV2格式 ,用于使用流方式来抓图		*/
	eJPGFormatUI,			    /*!< DVR/NVR/IPD等有本地界面的设备适用		*/
	eJPGFormatV3,			    /*!< jpg格式，用于获取缩略图				*/
	eImageFormatEnd,			/*!< 结束值                                 */
}eImageFormat;

/*!
*  @enum	eQueryStatus
*  @brief	查询状态
*/
typedef enum eQueryStatus
{
	eQueryOK = 0,				/*!< 查询成功                               */
	eQueryFailed,				/*!< 查询失败                               */
	eQueryBusy,					/*!< 查询忙                                 */
	eQueryFinished,				/*!< 查询结束                               */
}eQueryStatus;

/*!
*  @enum	eRemoteDeviceControl
*  @brief	远程设备控制
*/
typedef enum eRemoteDeviceControl
{
	eRemoteDeviceControlBegin = 0,	/*!< 起始值                                 */
	eDeviceReboot,					/*!< 重启动                                 */
	eDeviceHalt,					/*!< 关机                                   */
	eDeviceStandby,					/*!< 待机                                   */
	eDeviceSetDefault,				/*!< 恢复系统默认                           */
	eDeviceSendTestEmail,			/*!< 发送测试邮件							*/
	eDeviceSetDefense,              /*!< 布防                                   */
	eDeviceUnsetDefense,            /*!< 撤防                                   */
	eDeviceDefaultISPCfg,			/*!< ISP默认配置                            */
	eDeviceAdminDefaultPassword,	/*!< 恢复admin的默认密码                    */
	eDeviceDetectBadPoint,		    /*!< 检测坏点								*/
	eDeviceStartDetect,				/*!< 开始探测								*/
	eDeviceStopDetect,			    /*!< 停止探测								*/
	eDeviceRebootV1,				/*!< 重启动V1接口								*/

	eRemotePreviewCruiseStart = 20,	/*!< 启动解码器预览轮转                     */
	eRemotePreviewCruiseStop,	    /*!< 停止解码器预览轮转                     */

	eDeviceResetImage,				/*!< 重置图像,重启高速球                    */
	eDeviceEnterAPMode,				/*!< 进入AP模式			                    */
	eDeviceStartVoice,				/*!< 启动对讲								*/
	eDeviceStopVoice,				/*!< 停止对讲								*/
	eDeviceRestoreFactory,			/*!< 恢复出厂设置                           */
	eDeviceSendTestFTP,				/*!< 发送测试FTP							*/
	eDeviceGammaDefault,			/*!< Gamma曲线恢复默认 						*/
	eDeviceRestoreFactory_CMCCOVD,		/*!< CMCC OVD恢复出厂设置       设备收到该请求后，应该将所有配置恢复到出厂状态（包括wifi配置），但不能断开当前网络连接，并返回成功应答。 OVC稍后会再下发一个重启指令将设备重启，默认配置生效。                    */
	eDeviceEnterSTAMode = 40,		/*!< 进入STA模式			                */
	eDevice4GCardTest,				/*!< 发送4G网络测试		                    */
	eDeviceSaveHardwareConfig,		/*!< 固化硬件配置		                    */
	eDeviceCancelHardwareConfig,	/*!< 取消硬件配置		                    */
	eDeviceClearFaceOverlayInfo,	/*!< 清除人脸叠加信息	                    */
	eDeviceTryListenAlarmAudio,		/*!< 试听报警音频信息	                    */
	eDeviceReversePacket,			/*!< 反向打包			                    */
	eDeviceLightWarningByCurrentConfig,		/*!< 根据当前灯光配置进行灯光报警			                    */
	eDeviceUpgradeSignal,			/*!< 设备升级（升级包已经在设备上）			                    */

	eDeviceOpenDoor = 100,					/*!< 开门			 						*/
	eDeviceIndicatorLightOpen,		/*!< 指示灯开			                    */
	eDeviceIndicatorLightClose,		/*!< 指示灯关			                    */

	eDeviceSimulateHardReset = 120,				/*!< 模拟硬件复位			 						*/
	eDeviceLeaveFactory = 121,			/*!< 出厂			 						*/

	eRemoteDeviceControlEnd,		/*!< 结束值                                 */
}eRemoteDeviceControl;


/*!
*  @enum	eDiskGroupType
*  @brief	 盘组类型
*/
typedef enum eDiskGroupType
{
	eDiskGroupTypeBegin = 0,	/*!< 起始值                                 */
	eDiskGroupNormal,			/*!< 普通盘组                               */
	eDiskGroupAlarm,			/*!< 报警盘组                               */
	eDiskGroupRedundance,		/*!< 冗余盘组                               */
	eDiskGroupBackup,			/*!< 备份盘组                               */
	eDiskGroupTypeEnd,			/*!< 结束值                                 */
}eDiskGroupType;


/*!
*  @enum	eDiskGroupOperation
*  @brief	硬盘组操作　
*/
typedef enum eDiskGroupOperation
{
	eDiskGroupOperationBegin = 0,	/*!< 起始值                                 */
	eUnMountDisk,				/*!< 卸载分区                               */
	eCreateNewPartition,		/*!< 建立新分区                             */
	eDeletePartition,			/*!< 删除分区                               */
	eFormatPartition,			/*!< 格式化分区                             */
	eDiskGroupKeepTime,			/*!< 分组保持数据时间                       */
	eDiskGroupBindChannel,		/*!< 分组绑定通道                           */
	eDiskGroupAddPartition,		/*!< 添加分区至分组                         */
	eDiskGroupDelPartition,		/*!< 从分组删除分区                         */
	eInitializeDisk,			/*!< 初始化硬盘								*/
	eDiskGroupOperationEnd,		/*!< 结束值                                 */
}eDiskGroupOperation;

/*!
*  @enum	ePTZControlCodeType
*  @brief	PTZ云台控制码类型
*/
typedef enum ePTZControlCmdCode
{
	ePTZControlCodeAllOff = 0,	//－   关闭(或断开)所有开关    (Param1: 无效, Param2: 无效, Param3: 无效, Param4: 无效)        
	ePTZControlCodeCameraPower = 1,	//－   接通摄像机电源      (Param1: 1-开始/0-停止, Param2: 无效, Param3: 无效, Param4: 无效)
	ePTZControlCodeLightPower = 2,	//－   接通灯光电源            (Param1: 1-开始/0-停止, Param2: 无效, Param3: 无效, Param4: 无效)
	ePTZControlCodeWiper = 3,	//－   接通雨刷开关            (Param1: 1-开始/0-停止, Param2: 无效, Param3: 无效, Param4: 无效)
	ePTZControlCodeFans = 4,	//－   接通风扇开关            (Param1: 1-开始/0-停止, Param2: 无效, Param3: 无效, Param4: 无效)
	ePTZControlCodeHeater = 5,	//－   接通加热器开关      (Param1: 1-开始/0-停止, Param2: 无效, Param3: 无效, Param4: 无效)
	ePTZControlCodeAuxEquiment = 6,	//－   接通辅助设备开关        (Param1: 1-开始/0-停止, Param2: 无效, Param3: 无效, Param4: 无效)

	ePTZControlCodeStopContinue = 10,	//－   停止所有连续量(镜头,云台)动作   (Param1: 无效, Param2: 无效, Param3: 无效, Param4: 无效)
	ePTZControlCodeZoomIn = 11,	//－   焦距变大(倍率变大)      (Param1: 1-开始/0-停止, Param2: 速度 [0-10,0表示默认速度,1-10表示速度级别], Param3: 无效, Param4: 倍率)
	ePTZControlCodeZoomOut = 12,	//－   焦距变小(倍率变小)      (Param1: 1-开始/0-停止, Param2: 速度 [0-10,0表示默认速度,1-10表示速度级别], Param3: 无效, Param4: 倍率)
	ePTZControlCodeFocusNear = 13,	//－   焦点前调                (Param1: 1-开始/0-停止, Param2: 速度 [0-10,0表示默认速度,1-10表示速度级别], Param3: 无效, Param4: 无效)
	ePTZControlCodeFocusFar = 14,	//－   焦点后调                (Param1: 1-开始/0-停止, Param2: 速度 [0-10,0表示默认速度,1-10表示速度级别], Param3: 无效, Param4: 无效)
	ePTZControlCodeApertureUp = 15,	//－   光圈扩大                (Param1: 1-开始/0-停止, Param2: 速度 [0-10,0表示默认速度,1-10表示速度级别], Param3: 无效, Param4: 无效)
	ePTZControlCodeApertureDown = 16,	//－   光圈缩小                (Param1: 1-开始/0-停止, Param2: 速度 [0-10,0表示默认速度,1-10表示速度级别], Param3: 无效, Param4: 无效)
	ePTZControlCodeAutoZoom = 17,	//－   开自动焦距(自动倍率)    (Param1: 1-开始/0-停止, Param2: 无效, Param3: 无效, Param4: 无效)
	ePTZControlCodeAutoFocus = 18,	//－   开自动调焦          (Param1: 1-开始/0-停止, Param2: 无效, Param3: 无效, Param4: 无效)
	ePTZControlCodeAutoAperture = 19,	//－   开自动光圈          (Param1: 1-开始/0-停止, Param2: 无效, Param3: 无效, Param4: 无效)
	ePTZControlCodeUp = 21,		//－   云台上仰                (Param1: 1-开始/0-停止, Param2: 速度 [0-10,0表示默认速度,1-10表示速度级别], Param3: 1表示电子PTZ,0表示传统PTZ, Param4: 无效)
	ePTZControlCodeDown = 22,	//－   云台下俯                (Param1: 1-开始/0-停止, Param2: 速度 [0-10,0表示默认速度,1-10表示速度级别], Param3: 1表示电子PTZ,0表示传统PTZ, Param4: 无效)
	ePTZControlCodeLeft = 23,	//－   云台左转                (Param1: 1-开始/0-停止, Param2: 速度 [0-10,0表示默认速度,1-10表示速度级别], Param3: 1表示电子PTZ,0表示传统PTZ, Param4: 无效)
	ePTZControlCodeRight = 24,	//－   云台右转                (Param1: 1-开始/0-停止, Param2: 速度 [0-10,0表示默认速度,1-10表示速度级别], Param3: 1表示电子PTZ,0表示传统PTZ, Param4: 无效)
	ePTZControlCodeUpLeft = 25,	//－   云台上仰和左转      (Param1: 1-开始/0-停止, Param2: 速度 [0-10,0表示默认速度,1-10表示速度级别], Param3: 无效, Param4: 无效)
	ePTZControlCodeUpRight = 26,	//－   云台上仰和右转      (Param1: 1-开始/0-停止, Param2: 速度 [0-10,0表示默认速度,1-10表示速度级别], Param3: 无效, Param4: 无效)
	ePTZControlCodeDownLeft = 27,	//－   云台下俯和左转      (Param1: 1-开始/0-停止, Param2: 速度 [0-10,0表示默认速度,1-10表示速度级别], Param3: 无效, Param4: 无效)
	ePTZControlCodeDownRight = 28,	//－   云台下俯和右转      (Param1: 1-开始/0-停止, Param2: 速度 [0-10,0表示默认速度,1-10表示速度级别], Param3: 无效, Param4: 无效)
	ePTZControlCodeAutoLeftRight = 29,	//－   云台左右自动扫描        (Param1: 1-开始/0-停止, Param2: 速度 [0-10,0表示默认速度,1-10表示速度级别], Param3: 无效, Param4: 无效)
	ePTZControlCodeLeftOneStep = 30,		//云台左转一步		(Param1: 无效, Param2: 无效, Param3: 无效, Param4: 无效)
	ePTZControlCodeUpOneStep = 31,		//云台上仰一步			(Param1: 无效, Param2: 无效, Param3: 无效, Param4: 无效)
	ePTZControlCodeRightOneStep = 32,		//云台右转一步			(Param1: 无效, Param2: 无效, Param3: 无效, Param4: 无效)
	ePTZControlCodeDownOneStep = 33,		//云台下俯一步			(Param1: 无效, Param2: 无效, Param3: 无效, Param4: 无效)
	ePTZControlCodeZoomInStep = 34,			//焦距变大一步			(Param1: 无效, Param2: 速度 [0-10,0表示默认速度,1-10表示速度级别], Param3: 无效, Param4: 无效)
	ePTZControlCodeZoomOutStep = 35,				//焦距变小一步			(Param1: 无效, Param2: 速度 [0-10,0表示默认速度,1-10表示速度级别], Param3: 无效, Param4: 无效)
 	ePTZControlCodeFocusNearStep = 36,  //－   焦点前调 (Param1: 无效, Param2: 无效, Param3: 无效, Param4: 无效)
    ePTZControlCodeFocusFarStep = 37,   //－   焦点后调 (Param1: 无效, Param2: 无效, Param3: 无效, Param4: 无效)
	ePTZControlCodePTZReset = 38,   //－   云台复位(AF专用云台项目增加) (Param1: 无效, Param2: 无效, Param3: 无效, Param4: 无效)
	
	ePTZControlCodePresetSet = 40,	//－   设置预置点          (Param1: 预置点序号[>=0], Param2: 无效, Param3: 无效, Param4: 无效)
	ePTZControlCodePresetClear = 41,	//－   清除预置点          (Param1: 预置点序号[>=0], Param2: 无效, Param3: 无效, Param4: 无效)
	ePTZControlCodePresetCall = 42,	//－   转到预置点          (Param1: 预置点序号[>=0], Param2: 无效, Param3: 无效, Param4: 无效)

	ePTZControlCodeCuriserSetStart = 51,	//－   启动巡航记忆            (Param1: 巡航路线号[>=0], Param2: 无效, Param3: 无效, Param4: 无效)
	ePTZControlCodeCuriserSetStop = 52,	//－   关闭巡航记忆            (Param1: 巡航路线号[>=0], Param2: 无效, Param3: 无效, Param4: 无效)
	ePTZControlCodeCuriserAddPreset = 53,	//－   将预置点加入巡航序列    (Param1: 巡航路线号[>=0], Param2: 预置点序号[>=0], Param3: 停顿时间[秒,>=0], Param4: 巡航速度[1-10])
	ePTZControlCodeCuriserRunStart = 54,	//－   开始巡航                (Param1: 巡航路线号[>=0], Param2: 无效, Param3: 无效, Param4: 无效)
	ePTZControlCodeCuriserRunStop = 55,	//－   停止巡航                (Param1: 巡航路线号[>=0], Param2: 无效, Param3: 无效, Param4: 无效)
	ePTZControlCodeCruiseTrackStart = 56,	//－   巡航跟踪开始(AF专用云台项目增加)      (Param1: 巡航路线号[>=0], Param2: 无效, Param3: 无效, Param4: 无效)
	ePTZControlCodeCruiseTrackStop = 57,	//－   巡航跟踪停止(AF专用云台项目增加)      (Param1: 巡航路线号[>=0], Param2: 无效, Param3: 无效, Param4: 无效)

	ePTZControlCodeTraceSetStart = 61,	//－   启动轨迹记忆            (Param1: 轨迹号[>=0], Param2: 无效, Param3: 无效, Param4: 无效)
	ePTZControlCodeTraceSetStop = 62,	//－   关闭轨迹记忆            (Param1: 轨迹号[>=0], Param2: 无效, Param3: 无效, Param4: 无效)
	ePTZControlCodeTraceRunStart = 63,	//－   开始轨迹                (Param1: 轨迹号[>=0], Param2: 无效, Param3: 无效, Param4: 无效)
	ePTZControlCodeTraceRunStop = 64,	//－   停止轨迹                (Param1: 轨迹号[>=0], Param2: 无效, Param3: 无效, Param4: 无效)

	ePTZControlCodeClearCruise = 65,	//清除
	ePTZControlCodeWatchRunStart = 66,	//－   开始守望                (Param1: 巡航路线号[>=0], Param2: 无效, Param3: 无效, Param4: 无效)
	ePTZControlCodeWatchRunStop = 67,	//－   停止守望                (Param1: 巡航路线号[>=0], Param2: 无效, Param3: 无效, Param4: 无效)
	ePTZControlCodeTrackRunStart = 68,	// -	 开始跟踪
	ePTZControlCodeTrackRunStop = 69,	// -   停止跟踪

	ePTZControlCodeBrightnessLeft = 80,
	ePTZControlCodeBrightnessRight = 81,
	ePTZControlCodeContrastLeft = 82,
	ePTZControlCodeContrastRight = 83,
	ePTZControlCodeSaturationLeft = 84,
	ePTZControlCodeSaturationRight = 85,

	ePTZWindowsOpen = 90,
	ePTZWindowsClose = 91,
	ePTZWindowsStop = 92,

	ePTZControlCodeSystemReset = 99,	//－   系统复位                (Param1: 无效, Param2: 无效, Param3: 无效, Param4: 无效)
	ePTZControlCodeElctronicReset = 100,//－   电子PTZ系统复位         (Param1: 无效, Param2: 无效, Param3: 无效, Param4: 无效)

	ePTZControlCodeCalibration = 110,		//校准

	ePTZControlCodeCameraLensReset = 120, 		//－ 镜头复位(AF专用云台项目增加)  (Param1: 无效, Param2: 无效, Param3: 无效, Param4: 无效)

	ePTZControlCodeRangeScanSetLeftBoundary = 130, 		//－ 区域扫描设置左边界(AF专用云台项目增加)  (Param1: 左边界预置点序号[>=0], Param2: 无效, Param3: 无效, Param4: 无效)
	ePTZControlCodeRangeScanSetRightBoundary = 131, 	//－ 区域扫描设置右边界(AF专用云台项目增加)  (Param1: 右边界预置点序号[>=0], Param2: 无效, Param3: 无效, Param4: 无效)
	ePTZControlCodeStartRangeScan = 132, 				//－ 启动扫描(AF专用云台项目增加)  (Param1: 无效, Param2: 无效, Param3: 无效, Param4: 无效)
	ePTZControlCodeStopRangeScan = 133, 				//－ 停止扫描(AF专用云台项目增加)  (Param1: 无效, Param2: 无效, Param3: 无效, Param4: 无效)
	ePTZControlCodeStartRangeScan360 = 134, 			//－ 启动360度区域扫描(AF专用云台项目增加)  (Param1: 无效, Param2: 无效, Param3: 无效, Param4: 无效)
	ePTZControlCodeStopRangeScan360 = 135, 				//－ 停止360度区域扫描(AF专用云台项目增加)  (Param1: 无效, Param2: 无效, Param3: 无效, Param4: 无效)
	ePTZControlCodeStartRangeScanTrack = 136, 			//－ 启动区域扫描跟踪(AF专用云台项目增加)  (Param1: 无效, Param2: 无效, Param3: 无效, Param4: 无效)
	ePTZControlCodeStopRangeScanTrack = 137, 			//－ 停止区域扫描跟踪(AF专用云台项目增加)  (Param1: 无效, Param2: 无效, Param3: 无效, Param4: 无效)
    ePTZControlCodeAbsoluteMove = 138, 			        //－ 绝对移动  (Param1: 无效, Param2: 无效, Param3: 绝对移动的坐标，通过结构体 HY_DVR_PTZ_POS_INFO 传递, Param4: 无效)
    ePTZControlCodeRelativeMove = 139, 			        //－ 相对移动  (Param1: 无效, Param2: 无效, Param3: 绝对移动的坐标，通过结构体 HY_DVR_PTZ_POS_INFO 传递, Param4: 无效)

	ePTZControlCodeWatchCareGoto = 160,			/* － 转到看守位(Param1: 看守位序号[>=0], Param2: 无效, Param3: 无效, Param4: 无效);
													说明:以前设备没有分开预置位和看守位，如果预置点的坐标修改，看守位的坐标也修改了，为避免这种问题将与预置位和看守位分开 */

	ePTZControlCodeCalibrationSelfCheck = 180,			//－ 校准自检(H82-50POM-SWBT海思枪球项目增加)  (Param1: 无效, Param2: 无效, Param3: 无效, Param4: 无效)
    ePTZControlCodeCalibrate = 181,						//－ 标定(H82-50POM-SWBT海思枪球项目增加)  (Param1: 无效, Param2: 无效, Param3: 无效, Param4: 无效)
}ePTZControlCmdCode;

/*!
*  @enum	ePTZControlCmdType
*  @brief	PTZ控制命令类型(针对Hxht协议)
*/
typedef enum ePTZControlCmdType
{
	ePTZCmdTypeBegin = 0,		// PTZ控制命令起始值
	ePTZCmdTypeGetStatus,				// 获取PTZ状态
	ePTZCmdTypeControl,					// PTZ控制
	ePTZCmdTypeSet485Param,				// 设置485参数
	ePTZCmdTypeWrite485Data,			// 直写485数据
	ePTZCmdTypeEnd,						// PTZ控制命令结束值
}ePTZControlCmdType;

/*!
*  @enum	tagPTZControlOperation
*  @brief	PTZ控制码(针对Hxht协议)
*/
typedef enum ePTZControlOperation
{
	ePTZOperationaIRISControl = 8300,				/*!< 光圈调节											*/
	ePTZOperationIRISStop = 8306,				/*!< 停止光圈调节										*/
	ePTZOperationDigitalControl = 8302,				/*!< 数字变倍											*/
	ePTZOperationDigitalStop = 8308,				/*!< 停止数字变倍										*/
	ePTZOperationStart = 8200,				/*!< 云台开始动作										*/
	ePTZOperationStop = 8202,				/*!< 云台停止动作										*/
	ePTZOperationScanStart = 8204,				/*!< 开始随机线扫										*/
	ePTZOperationScanStop = 8204,				/*!< 停止随机线扫										*/
	ePTZOperationPresetSetting = 8206,				/*!< 预置点设置											*/
	ePTZOperationPresetClear = 8208,				/*!< 预置点清除											*/
	ePTZOperationPresetCall = 8210,				/*!< 预置点调用											*/
	ePTZOperationPresetClearAll = 8212,				/*!< 清除所有											*/
	ePTZOperationFocusControl = 8304,				/*!< 自动聚焦控制										*/
	ePTZOperationFocusStop = 8310,				/*!< 停止自动聚焦										*/
	ePTZOperationLightOn = 13,				/*!< 灯光开												*/
	ePTZOperationLightOff = 14,				/*!< 灯光关												*/
	ePTZOperationRainBrushOn = 15,				/*!< 雨刷开												*/
	ePTZOperationRainBrushOff = 16,				/*!< 雨刷开												*/
	ePTZOperationDetectionOn = 17,				/*!< 探头开												*/
	ePTZOperationDetectionOff = 18,				/*!< 探头开												*/
	ePTZOperationCameraOn = 19,				/*!< 摄头开												*/
	ePTZOperationCameraOff = 20,				/*!< 摄头开												*/
}ePTZControlOperation;

/*!
*  @enum	eAppRefreshEvent
*  @brief	应用更新事件通知类型
*/
typedef enum eAppRefreshEvent
{
	eAppRefreshEventBegin = 0,	/*!< 起始值                                 */
	eManagerServerRefresh,		/*!< 管理服务器状态                         */
	eUserActionRefresh,			/*!< 用户动作状态                           */
	eRealStreamRefresh,			/*!< 实时流事件                             */
	eVoiceRefresh,				/*!< 语音流事件                             */
	eRemoteDownloadRefresh,		/*!< 远程下载状态                           */
	eRemoteDetectRefresh,		/*!< 远程探测                               */
	eAppRefreshEventEnd,		/*!< 结束值                                 */
}eAppRefreshEvent;

/*!
*  @enum	eStreamTransferMode
*  @brief	流传输方式
*/
typedef enum eStreamTransferMode
{
	eStreamTransferModeBegin = 0,	/*!< 起始值                                 */
	eGeneralTCP,				/*!< 普通的TCP                              */
	eGeneralRTP,				/*!< 普通的RTP                              */
#if 0
	eGeneralRTCP,				/*!< 普通的RTCP                             */
	eGeneralRTSP,				/*!< 普通的RTSP                             */
	eGeneralUDP,				/*!< 普通的UDP                              */
	eGeneralMulticast,			/*!< 普通的多播                             */
#endif
	eStreamTransferModeEnd,		/*!< 结束值                                 */
}eStreamTransferMode;

/*!
*  @enum	eVoiceMode
*  @brief	语音编解码模式
*/
typedef enum eVoiceMode
{
	eVoiceModeBegin = 0,		/*!< 起始值                                 */
	eVoiceG726,					/*!< G726编解码模式                         */
	eVoiceG711U,				/*!< G711U编解码模式                        */
	eVoiceMPEG4AAC,				/*!< MPEG4 AAC编解码模式					*/
	eVoiceAutoDiscern,			/*!< 自动识别编解码模式						*/
	eVoiceADPCMima,										/*!< ADPCMima编解码模式						*/
	eVoiceG711A,										/*!< G711A编解码模式                        */
	eVoicePCM,										/*!< PCM编解码模式					        */
	eVoiceAAC,                                      /*!< AAC编解码模式 (H5SV(H5SVX)-50PSX_V1.1.2（3516CV610 20S+IMX335）项目新增)                  */
	eVoiceModeEnd,				/*!< 结束值                                 */
}eVoiceMode;

/*!
*  @enum	eVideoMode
*  @brief	视频编解码模式
*/
typedef enum eVideoMode
{
	eVideoModeBegin = 0,		/*!< 起始值                                 */
	eVoiceH264,					/*!< G726编解码模式                         */
	eVoiceH265,				/*!< G711U编解码模式                        */
	eVideoModeEnd,				/*!< 结束值                                 */
}eVideoMode;

/*!
*  @enum	eVideoCodecMode
*  @brief	视频编解码模式
*/
typedef enum eVideoCodecMode
{
	eVideoCodecModeBegin = 0,	/*!< 起始值									*/
	eVideoCodecH264,			/*!< 标准H264编解码模式						*/
	eVideoCodecHYMpeg4,			/*!< HYMpeg4编解码模式                      */
	eVideoCodecHYH264,			/*!< HYH264编解码模式						*/
	eVideoCodecAutoDiscern,		/*!< 自动识别编解码模式						*/
	eVideoCodecModeEnd,			/*!< 结束值									*/
}eVideoCodecMode;

/*!
*  @enum	eTaskStatus
*  @brief	任务状态
*/
typedef enum eTaskStatus
{
	eTaskStatusBegin = 0,		/*!< 起始值                                 */
	eTaskStatusNone,			/*!< 任务无状态                             */
	eTaskStatusInitialize,		/*!< 任务初始化                             */
	eTaskStatusDestroy,			/*!< 任务销毁                               */
	eTaskStatusStart,			/*!< 任务启动                               */
	eTaskStatusStop,			/*!< 任务停止                               */
	eTaskStatusRun,				/*!< 任务运行中                             */
	eTaskStatusRecover,			/*!< 任务恢复                               */
	eTaskStatusEnd,				/*!< 结束值                                 */
}eTaskStatus;

/*!
*  @enum	eMediaFunctionType
*  @brief	媒体功能类型
*/
typedef enum eMediaFunctionType
{
	eMediaFunctionTypeBegin = 0,	/*!< 起始值                                 */
	eMediaFunctionRealStream,	/*!< 实时流                                 */
	eMediaFunctionVoiceStream,	/*!< 语音流                                 */
	eMediaFunctionHistoryStream,	/*!< 历史流                                 */
	eMediaFunctionTransparent,	/*!< 透明通道                               */
	eMediaFunctionFileUpload,	/*!< 文件上传                               */
	eMediaFunctionFileDownload,	/*!< 文件下载                               */
	eMediaFunctionTypeEnd,		/*!< 结束值                                 */
}eMediaFunctionType;

/*!
*  @enum	eFrameType
*  @brief	帧类型
*/
typedef enum eFrameType
{
	eFrameError = 0x0000,
	eFrameIFrames = 0x0001,			//主码流的I帧
	eFramePFrames = 0x0002,
	eFrameBPFrames = 0x0020,
	eFrameBBPFrames = 0x0004,
	eFrameAudioFrames = 0x0008,
	eFrameQCIFIFrames = 0x0010,		//QCIF的I帧
	eFramePktSQCIFIFrames = 0x0011, //高清子码流I帧
	eFrameQCIFPFrames = 0x0040,
	eFramePktSQCIFPFrames = 0x0041, //高清子码流P帧
	eFrameBIFrames = 0x0090,
	eFrameBBIFrames = 0x00C0,
	eFrameSysHeader = 0x0080,			//主码流系统头
	eFramePktSysHeaderAux = 0x0081,		//子码流系统头
	eFramePktSysHeaderHDAux = 0x0082,	//高清子码流系统头
	eFramePktIAFrames = 0x86,		// AI 分析帧
	eFrameSFrames = 0x0200,
	eFrameDspStatus = 0x0100,
	eFrameAimDetection = 0x0400,
	eFrameOrigImage = 0x0800,
	eFrameMotionDetection = 0x1000,
	eFrameAdvFaceFrames  = 0xA0,
    eFrameIACarFrames      = 0xA1,//车形帧，就是一张jpg图
    eFrameIAElectronicFenceFrames    = 0xA2,//电子围栏
  	eFrameIACrossBoardDetectFrames   = 0xA3,//越界侦测
  	eFrameIAOffDutyDetectFrames      = 0xA4,//离岗侦测
  	eFrameIAPassengerFlowFrames      = 0xA5,//客流统计
    eFrameIAFireDetectFrames         = 0xA6,//火焰
    eFrameIAPassengerFlowHumanEventFrames = 0xA7,//客流统计人形事件
    eFrameIAVideoShelterDetectEventFrames = 0xA8,//视频遮挡检测事件
}eFrameType;

/*!
*  @enum	eSerialType
*  @brief	串口类型
*/
typedef enum eSerialType
{
	eTTY232 = 0,							/*!< RS232串口                               */
	eTTY485,								/*!< RS485串口                               */
	eTTY422,								/*!< RS422串口                               */
}eSerialType;

/*!
*  @enum	eDevType
*  @brief	设备类型
*/
typedef enum eDevType
{
	eDevTypeTerminal = 2,				/*!< 前端(视频服务器) */
	eDevTypeStore = 4,					/*!< 存储服务器 */
	eDevTypeCamera = 5,					/*!< 摄像头 */
	eDevTypeAlarmin = 6,				/*!< 报警输入 */
	eDevTypeAlarmout = 8,				/*!< 报警输出 */
	eDevTypeAnalyse = 20,				/*!< 智能分析单元 */
}eDevType;

/*!
*  @enum	eAlarmEventType
*  @brief	报警输入事件类型
*/
typedef enum eAlarmEventType
{
	eAlarmEventTypeOverflow = 0,		/*!< 越界检测 */
	eAlarmEventTypeResort = 1,			/*!< 滞留检测 */
	eAlarmEventTypeCarSpeed = 2,		/*!< 车速检测 */
}eAlarmEventType;

/*!
*  @enum	eFaultType
*  @brief	故障类型
*/
typedef enum eFaultType
{
	eFaultTypeDevice = 1,				/*!< 设备告警 */
	eFaultTypeCapability = 2,			/*!< 性能告警 */
	eFaultTypeProcess = 3,				/*!< 处理出错告警 */
	eFaultTypeCommunication = 4,		/*!< 通讯告警 */
	eFaultTypeEnvironment = 5,			/*!< 环境告警 */
}eFaultType;

/*!
*  @enum	eFaultID
*  @brief	故障编号
*/
typedef enum eFaultID
{
	//设备故障编号
	eFaultTypeFlash = 7,				/*!< 操作Flash失败报警 */
	eFaultTypeDisk = 8,					/*!< 硬盘读写失败告警或者硬盘故障报警 */
	eFaultTypeDiskSpace = 501,			/*!< 硬盘空间剩余容量低于阈值报警 */
	eFaultTypeDiskFull = 502,			/*!< 硬盘空间满报警 */
	eFaultTypeStoreServer = 503,		/*!< 向中心存储FTP文件失败*/

	//性能故障编号
	eFaultTypeCpuUseRatio = 1,			/*!< 操作Flash失败报警 */
	eFaultTypeDiskUseRatio = 2,			/*!< 硬盘读写失败告警或者硬盘故障报警 */
	eFaultTypeMemoryUseRatio = 3,		/*!< 硬盘空间剩余容量低于阈值报警 */

	//通讯故障编号
	eFaultTypeVideoLoss = 4,			/*!< 视频信号中断报警 */
}eFaultID;

/*!
*  @enum	eFaultState
*  @brief	故障状态
*/
typedef enum eFaultState
{
	New = 0,			/*!< 新故障 */
	Resure = 1,			/*!< 故障恢复 */
}eFaultState;

/*!
*  @enum	eConnectType
*  @brief	流连接方式
*/
typedef enum eConnectType
{
	eConnectTypeBegin = 0,
	eConnectActive,
	eConnectPassive,
	eConnectTypeEnd,
}eConnectType;

/*!
*  @struct	ePanelKeyBoardCodeType
*  @brief	面板键盘值类型
*/
typedef enum ePanelKeyBoardCodeType
{
	ePanelKeyBoardTypeBegin = 0x0,		/*!< 起始值									*/
	ePanelKeyBoardCode_0 = 0x1,		/*!< 对应键:0								*/
	ePanelKeyBoardCode_1 = 0x2,		/*!< 对应键:1								*/
	ePanelKeyBoardCode_2 = 0x3,		/*!< 对应键:2								*/
	ePanelKeyBoardCode_3 = 0x4,		/*!< 对应键:3								*/
	ePanelKeyBoardCode_4 = 0x5,		/*!< 对应键:4								*/
	ePanelKeyBoardCode_5 = 0x6,		/*!< 对应键:5								*/
	ePanelKeyBoardCode_6 = 0x7,		/*!< 对应键:6								*/
	ePanelKeyBoardCode_7 = 0x8,		/*!< 对应键:7								*/
	ePanelKeyBoardCode_8 = 0x9,		/*!< 对应键:8								*/
	ePanelKeyBoardCode_9 = 0xA,		/*!< 对应键:9								*/
	ePanelKeyBoardCode_10 = 0xB,		/*!< 对应键:10								*/
	ePanelKeyBoardCode_11 = 0xC,		/*!< 对应键:11								*/
	ePanelKeyBoardCode_12 = 0xD,		/*!< 对应键:12								*/
	ePanelKeyBoardCode_13 = 0xE,		/*!< 对应键:13								*/
	ePanelKeyBoardCode_14 = 0xF,		/*!< 对应键:14								*/
	ePanelKeyBoardCode_15 = 0x10,		/*!< 对应键:15								*/
	ePanelKeyBoardCode_16 = 0x11,		/*!< 对应键:16								*/
	ePanelKeyBoardCode_PTZ = 0x40,		/*!< 对应键:云台							*/
	ePanelKeyBoardCode_Copy = 0x41,		/*!< 对应键:备份							*/
	ePanelKeyBoardCode_Multi = 0x42,		/*!< 对应键:多画面							*/
	ePanelKeyBoardCode_Switch = 0x43,		/*!< 对应键:切换							*/
	ePanelKeyBoardCode_Function = 0x44,		/*!< 对应键:辅助							*/
	ePanelKeyBoardCode_Play = 0x45,		/*!< 对应键:播放							*/
	ePanelKeyBoardCode_Backward = 0x46,		/*!< 对应键:倒放							*/
	ePanelKeyBoardCode_Record = 0x47,		/*!< 对应键:录像							*/
	ePanelKeyBoardCode_ESC = 0x48,		/*!< 对应键:退出							*/
	ePanelKeyBoardCode_Left = 0x49,		/*!< 对应键:左								*/
	ePanelKeyBoardCode_Right = 0x4A,		/*!< 对应键:右								*/
	ePanelKeyBoardCode_Up = 0x4B,		/*!< 对应键:上								*/
	ePanelKeyBoardCode_Down = 0x4C,		/*!< 对应键:下								*/
	ePanelKeyBoardCode_Enter = 0x4D,		/*!< 对应键:确认							*/
	ePanelKeyBoardCode_Shutdown = 0x4E,		/*!< 对应键:关机							*/
	ePanelKeyBoardCode_TV_VGA = 0x4F,		/*!< 对应键:TV/VGA							*/
	ePanelKeyBoardCode_Edit = 0x50,		/*!< 对应键:编辑							*/
	ePanelKeyBoardCode_Language = 0x51,		/*!< 对应键:语言							*/
	ePanelKeyBoardCode_Mute = 0x52,		/*!< 对应键:静音							*/
	ePanelKeyBoardCode_Pause = 0x53,		/*!< 对应键:暂停							*/
	ePanelKeyBoardCode_Stop = 0x54,		/*!< 对应键:停止							*/
	ePanelKeyBoardCode_AlarmClear = 0x55,		/*!< 对应键:消警							*/
	ePanelKeyBoardCode_Defence = 0x56,		/*!< 对应键:布防撤防						*/
	ePanelKeyBoardCode_Capture = 0x57,		/*!< 对应键:抓图							*/
	ePanelKeyBoardCode_LightAdd = 0x58,		/*!< 对应键:亮度增加						*/
	ePanelKeyBoardCode_LightSub = 0x59,		/*!< 对应键:亮度减少						*/
	ePanelKeyBoardCode_ContrastAdd = 0x5A,		/*!< 对应键:对比度增加						*/
	ePanelKeyBoardCode_ContrastSub = 0x5B,		/*!< 对应键:对比度减少						*/
	ePanelKeyBoardCode_SpeedAdd = 0x5C,		/*!< 对应键:速度增加						*/
	ePanelKeyBoardCode_SpeedSub = 0x5D,		/*!< 对应键:速度减少						*/
	ePanelKeyBoardCode_Set = 0x5E,		/*!< 对应键:设置							*/
	ePanelKeyBoardCode_F1 = 0x5F,		/*!< 对应键:F1								*/
	ePanelKeyBoardTypeEnd,									/*!< 结束值									*/
}ePanelKeyBoardCodeType;

/*!
*  @struct	ePanelControlType
*  @brief	面板控制类型
*/
typedef enum ePanelControlType
{
	ePanelControlBegin = 0,		/*!< 起始值									*/
	ePanelControlKeyDown,									/*!< 按下									*/
	ePanelControlKeyUp,										/*!< 弹起									*/
	ePanelControlClick,										/*!< 点击(按下并弹起)						*/
	ePanelControlEnd,										/*!< 结束值									*/
}ePanelControlType;

/*!
*  @struct	eDecoderChannelStatus
*  @brief	解码通道状态
*/
typedef enum eDecoderChannelStatus
{
	eDecoderChannelStatusBegin = 0,		/*!< 起始值									*/
	eDecoderChannelStatusOpen,								/*!< 打开									*/
	eDecoderChannelStatusClose,								/*!< 关闭									*/
	eDecoderChannelStatusContinue,							/*!< 保持									*/
	eDecoderChannelStatusEnd,								/*!< 结束值									*/
}eDecoderChannelStatus;

/*!
*  @struct	eStreamSourceType
*  @brief	流媒体源类型
*/
typedef enum eStreamSourceType
{
	eStreamSourceTypeBegin = 0,		/*!< 起始值									*/
	eStreamSourceTypeReal,									/*!< 实时流									*/
	eStreamSourceTypeHistory,								/*!< 历史流									*/
	eStreamSourceTypeEnd,									/*!< 结束值									*/
}eStreamSourceType;

/*!
*  @struct	eStreamPlayControlStatus
*  @brief	流媒体播放控制状态
*/
typedef enum eStreamPlayControlStatus
{
	eStreamPlayControlBegin = 0,		/*!< 起始值									*/
	eStreamPlayControlPlay,									/*!< 播放									*/
	eStreamPlayControlSuspend,								/*!< 暂停									*/
	eStreamPlayControlResume,								/*!< 恢复									*/
	eStreamPlayControlStop,									/*!< 停止									*/
	eStreamPlayControlFast,									/*!< 快速播放								*/
	eStreamPlayControlSlow,									/*!< 慢速播放								*/
	eStreamPlayControlNormal,								/*!< 正常速度								*/
	eStreamPlayControlOneByOne,								/*!< 单帧播放								*/
	eStreamPlayControlOpenAudio,							/*!< 打开声音								*/
	eStreamPlayControlCloseAudio,							/*!< 关闭声音								*/
	eStreamPlayControlSetPos,								/*!< 设置位置								*/
	eStreamPlayControlClearBuffer,							/*!< 清空缓冲区								*/
	eStreamPlayControlEnd,									/*!< 结束值									*/
}eStreamPlayControlStatus;

/*!
*  @struct	eDisplayControlStatus
*  @brief	显示控制状态
*/
typedef enum eDisplayControlStatus
{
	eDisplayControlStatusBegin = 0,		/*!< 起始值									*/
	eDisplayControlStatusEnlarge,							/*!< 放大显示								*/
	eDisplayControlStatusRenew,								/*!< 还原									*/
	eDisplayControlStatusEnd,								/*!< 结束值									*/
}eDisplayControlStatus;

typedef enum eResolutionIndex
{
	eResolutionIndex_CIF,			//352*288
	eResolutionIndex_D1,			//704*576

	eResolutionIndex_SD,
	eResolutionIndex_QuadVGA,		//1280*960
	eResolutionIndex_SXGA,			//1280*1024
	eResolutionIndex_UXGA,			//1600*1200

	eResolutionIndex_720P,			//1280*720
	eResolutionIndex_1080P,			//1920*1080

	eResolutionIndex_VGA,			//640*480

	eResolutionIndex_QXGA,			//2048x1536
	eResolutionIndex_640x360,		//640*360
	eResolutionIndex_QVGA,			//320*240
	eResolutionIndex_UWXGA,			//2592*1520
	eResolutionIndex_USXGA,			//2592*1944
	eResolutionIndex_480X256,		//480*256
	eResolutionIndex_320X176,		//320*176
	eResolutionIndex_800X448,		//800*448

	eResolutionIndex_960H = 20,		//960*576
	eResolutionIndex_1024x1536,		//1024*1536
	eResolutionIndex_QCIF,			//176*144
	eResolutionIndex_WQXGA,			//2304*1728
	eResolutionIndex_QSXGA,			//2592*1920
	eResolutionIndex_1080N = 30,		//960*1080
	eResolutionIndex_4M_N,			//1280*1440
	eResolutionIndex_5M_N,			//1296*1944
	eResolutionIndex_4M,			//2560*1440
	eResolutionIndex_8M_N,			//1920*2160
	eResolutionIndex_8M,			//3840*2160

	eResolutionIndex_640x512,		//640*512

	eResolutionIndex_3M,		//2304*1296
	eResolutionIndex_5M,		//2560*1920
	eResolutionIndex_4K,		//4096*2160

	eResolutionIndex_2880x1616 = 50, //2880x1616
	eResolutionIndex_2880x1620 = 51, //2880x1620
	eResolutionIndex_2592x1944 = 52, //2592x1944
	eResolutionIndex_3200x1800 = 60, //3200x1800
}eResolutionIndex;

typedef enum eSystemLanguage
{
	eLanguage_Chinese,				//简体中文
	eLanguage_English,				//英语
	eLanguage_Korean,				//韩国
	eLanguage_Japanese,				//日语
	eLanguage_Italian,				//意大利语
	eLanguage_Russian,				//俄罗斯语
	eLanguage_Spanish,				//西班牙语
	eLanguage_Portuguese,			//葡萄牙语
	eLanguage_Persian,				//波斯语
	eLanguage_Arabic,				//阿拉伯文
	eLanguage_Bulgarian,			//保加利亚语
	eLanguage_Chinese_Traditional,		//繁体中文
	eLanguage_Custom,		//定制语言
	eLanguage_Croation,			//克罗地亚语
	eLanguage_Czech,				//捷克语
	eLanguage_Danish,				//丹麦语
	eLanguage_Dutch,				//荷兰（德语）
	eLanguage_German,				//德语
	eLanguage_Estonian,			//爱沙尼亚语
	eLanguage_Finnish,				//芬兰语
	eLanguage_French,				//法语
	eLanguage_Greek,				//希腊语
	eLanguage_Hebrew,				//希伯来语
	eLanguage_Hungarian,			//匈牙利语
	eLanguage_Latvian,				//拉脱维亚语
	eLanguage_Lithuanian,			//立陶宛语
	eLanguage_Norwegian,			//挪威语
	eLanguage_Polish,				//波兰语
	eLanguage_Romanian,			//罗马尼亚语
	eLanguage_Slovak,				//斯洛伐克语
	eLanguage_Slovenian,			//斯洛维尼亚语
	eLanguage_Swedish,				//瑞典语
	eLanguage_Thai,					//泰语
	eLanguage_Turkish,				//土耳其语
    eLanguage_Vietnamese,           //越南语
    eLanguage_Indonesian,           //印尼语
    eLanguage_KoreanLanguage,       //朝鲜语
}eSystemLanguage;

//该字符串的顺序严格按照枚举eSystemLanguage的顺序来定义，对于的字符串的定义由服务器提供
static const char *SystemLanguage[] __attribute__((unused)) = {"SimpChinese","English","Korean","Japanese","Italian","Russian",
                                    "Spanish","Portuguese","Persian","Arabic","Bulgarian","TranChinese",
                                    "Custom","Croation","Czech","Danish","Dutch","German",
                                    "Estonian","Finnish","French","Greek","Hebrew","Hungarian",
                                    "Latvian","Lithuanian","Norwegian","Polish","Romanian","Slovak",
                                    "Slovenian","Swedish","Thai","Turkish","Vietnamese","Indonesian",
                                    "KoreanLanguage"};

/**
 * @brief 通用车型，目前车牌抓拍事件使用
 * 
 */
typedef enum tagVehicleType
{
    VEHICLE_TYPE_INVALID,        // 起始无效值
    VEHICLE_TYPE_CAR,            // 小轿车
    VEHICLE_TYPE_SUV,            // suv
    VEHICLE_TYPE_MICROBUS,       // 面包车
    VEHICLE_TYPE_MIDIBUS,        // 中巴/大巴
    VEHICLE_TYPE_PICKUP          // 皮卡
}VehicleType;

/***
 * @brief 通用车辆颜色，目前车牌抓拍事件使用
*/
typedef enum tagVehicleColor
{
    VEHICLE_COLOR_INVALID,      // 起始无效值
    VEHICLE_COLOR_BLACK,        // 黑色
    VEHICLE_COLOR_WHITE,        // 白色
    VEHICLE_COLOR_SILVERY,      // 银灰
    VEHICLE_COLOR_GRAY,         // 深灰
    VEHICLE_COLOR_BROWN,        // 棕色
    VEHICLE_COLOR_RED,          // 红色
    VEHICLE_COLOR_ORANGE,       // 橙色
    VEHICLE_COLOR_YELLOW,       // 黄色
    VEHICLE_COLOR_GREEN,        // 绿色
    VEHICLE_COLOR_CYAN,         // 青色
    VEHICLE_COLOR_BLUE,         // 蓝色
    VEHICLE_COLOR_PINK,        // 粉色
    VEHICLE_COLOR_PURPLE,      // 紫色
    VEHICLE_COLOR_CHAMPAGNE    // 香槟色
}VehicleColor;

/**
 * @brief 通用车牌颜色，目前车牌抓拍事件使用
 * 
 */
typedef enum tagPlateColor
{
    PLATE_COLOR_INVALID,    // 起始无效值
    PLATE_COLOR_BLUE,       // 蓝牌
    PLATE_COLOR_YELLOW,     // 黄牌
    PLATE_COLOR_BLACK,      // 黑牌
    PLATE_COLOR_WHITE,      // 白牌
    PLATE_COLOR_GREEN,      // 绿牌
    PLATE_COLOR_SMALL_NEW_GREEN,    // 渐变绿
    PLATE_COLOR_LARGE_NEW_GREEN,    // 黄绿
    PLATE_COLOR_ABENSE       // 缺省
}PlateColor;

//////////////////////////////////////////////////////////////////////////
//结构
//////////////////////////////////////////////////////////////////////////

/*!
*  @struct	tagTimeInfo
*  @brief	时间信息
*/
typedef struct tagTimeInfo
{
	WORD wYear;					/*!< 年                                     */
	WORD wMonth;				/*!< 月                                     */
	WORD wDay;					/*!< 日                                     */
	WORD wHour;					/*!< 时                                     */
	WORD wMinute;				/*!< 分                                     */
	WORD wSecond;				/*!< 秒                                     */
} TimeInfo, *LPTimeInfo;

/*!
*  @struct	tagAbsoluteTime
*  @brief	绝对时间信息
*/
typedef struct tagAbsoluteTime
{
	WORD wYear;					/*!< 年                                     */
	WORD wMonth;				/*!< 月                                     */
	WORD wWeek;					/*!< 周                                     */
	WORD wDay;					/*!< 日                                     */
	WORD wHour;					/*!< 时                                     */
	WORD wMinute;				/*!< 分                                     */
	WORD wSecond;				/*!< 秒                                     */
	WORD wMillisecond;			/*!< 毫秒                                   */
} AbsoluteTime, *LPAbsoluteTime;

/*!
*  @struct	tagBuffer
*  @brief	缓冲区信息
*/
typedef struct tagBuffer
{
	BYTE *pBuffer;				/*!< 媒体数据缓冲区                         */
	DWORD dwBufLen;				/*!< 媒体数据长度                           */
} Buffer, *LPBuffer;

/*!
*  @struct	tagManagerServerState
*  @brief	管理服务器状态
*/
typedef struct tagManagerServerState
{
	eManagerServerType eType;	/*!< 管理类型                               */
	DWORD dwState;				/*!< 管理状态                               */
	char sServer[IP_ADDRESS_LEN];	/*!< 管理主机IP地址信息                     */
	DWORD dwServerDomainEnable;	/*!<管理主机域名信息有效标志                */
	char sServerDomain[DOMAIN_ADDRESS_LEN];	/*!<管理主机域名信息                      */
	DWORD dwServerPort;			/*!<管理主机端口                            */
	DWORD dwReserve[DEF_RESERVE_NUM];	/*!<保留                                    */
} ManagerServerState, *LPManagerServerState;

/*!
*  @struct	tagRealStreamMedia
*  @brief	实时流媒体
*/
typedef struct tagRealStreamMedia
{
	eStreamMediaType eType;		/*!< 流类型                                 */
	Buffer cBuffer;				/*!< 缓冲区信息                             */
	AbsoluteTime cTime;			/*!< 帧时间                                 */
	DWORD dwFrameType;			/*!< 帧类型                                 */
	DWORD dwReserve[DEF_RESERVE_NUM];	/*!< 保留， dwReserve[0]的高16位表示通道，低16位表示码流类型,dwReserve[1]的值为0表示普通编码器编码，1，表示smartp编码器编码 */
} RealStreamMedia, *LPRealStreamMedia;

/*!
*  @struct	tagVideoStreamMedia
*  @brief	实时流媒体
*/
typedef struct tagVideoStreamMedia
{
	eVideoMode eMode;					/*!< 视频通话视频流编码类型                                 */
	eVoiceMode eVoiceCodec;				/*!< 视频通话语音流编码类型                                 */
	eVideoCallMediaType eMediaType;		/*!< 视频通话码流类型（只传语音，只传视频，既传语音又传视频）  */
	char cReserve[32];	/*!< 保留*/
} VideoStreamMedia, *LPVideoStreamMedia;


/*!
*  @struct	tagVideoCallMediaControl
*  @brief	视频通话媒体控制参数
*/
typedef struct tagVideoCallMediaControl
{
	eVideoCallMediaType eMediaType;		/*!< 视频通话码流类型（只传语音，只传视频，既传语音又传视频）  */
	char cReserve[32];	/*!< 保留*/
} VideoCallMediaControl, *LPVideoCallMediaControl;

/*!
*  @struct	tagUserData
*  @brief	拉流保留字段
*/
typedef struct tagUserData
{
	DWORD dwSeq;
	void * pIter;
	char cReserve[64];
} UserData, *LPUserData;



/*!
*  @struct	tagStreamTransferFrame
*  @brief	流转发帧参数
*/
typedef struct tagStreamTransferFrame
{
	eFrameType	eFrameTypeInfo;							/*!< 帧类型信息								*/
	DWORD	dwTimeStamp;								/*!< 帧时间戳								*/
	AbsoluteTime cAbsoluteTime;							/*!< 帧绝对时间戳							*/
	DWORD	dwFrameNumber;								/*!< 帧编号									*/
	Buffer	cFrameBuffer;								/*!< 帧缓冲区								*/
	DWORD	dwReserve[DEF_RESERVE_NUM];					/*!< 保留									*/
}StreamTransferFrame, *LPStreamTransferFrame;

/*!
*  @struct	tagStreamFlowState
*  @brief	流流量状态
*/
typedef struct tagStreamFlowState
{
	DWORD dwBitrate;			/*!< 码率(kbps)                             */
	DWORD dwWorkTime;			/*!< 流工作时间                             */
	DWORD dwTotalFlow;			/*!< 总流量                                 */
	DWORD dwReserve[DEF_RESERVE_NUM];	/*!< 保留                                   */
} StreamFlowState, *LPStreamFlowState;

/*!
*  @struct	tagStreamState
*  @brief	流状态
*/
typedef struct tagStreamState
{
	DWORD dwClientNum;			/*!< 客户数                                 */
	DWORD dwUserID[REAL_STREAM_LIMIT];	/*!< 客户信息                               */
	StreamFlowState cStreamFlowState[REAL_STREAM_LIMIT];	/*!< 流量状态                   */
	DWORD dwReserve[DEF_RESERVE_NUM];	/*!< 保留                                   */
} StreamState, *LPStreamState;

/*!
*  @struct	tagRemoteUserInfo
*  @brief	远程用户信息
*/
typedef struct tagRemoteUserInfo
{
	DWORD dwUserID;				/*!< 用户ID                                 */
	char sUserName[USERNAME_LEN];	/*!< 用户名                                 */
	char sUserPass[USERPASS_LEN];	/*!< 用户密码                               */
	char sUserIPAddress[IP_ADDRESS_LEN];	/*!< 用户IP地址                             */
	char sClientIPAddress[IP_ADDRESS_LEN];	/*!< 客户端IP地址 */
	char sClientMACAddress[MAC_ADDRESS_LEN];	/*!< 客户端MAC地址                           */
	DWORD dwReserve[DEF_RESERVE_NUM];	/*!< 保留                                   */
} RemoteUserInfo, *LPRemoteUserInfo;

/*!
*  @struct	tagRealStreamAction
*  @brief	实时流事件通知
*/
typedef struct tagRealStreamAction
{
	DWORD dwChannel;			/*!< 通道号                                 */
	eStreamActionType eActionType;	/*!< 事件类型                               */
	DWORD dwReserve[DEF_RESERVE_NUM];	/*!< 保留                                   */
} RealStreamAction, *LPRealStreamAction;

/*!
*  @struct	tagVoiceStreamAction
*  @brief	语音流事件通知
*/
typedef struct tagVoiceStreamAction
{
	DWORD dwChannel;			/*!< 通道号                                 */
	eStreamActionType eActionType;	/*!< 事件类型                               */
	DWORD dwReserve[DEF_RESERVE_NUM];	/*!< 保留                                   */
} VoiceStreamAction, *LPVoiceStreamAction;

/*!
*  @struct	tagAlarmNotify
*  @brief	报警通知信息
*/
typedef struct tagAlarmNotify
{
	DWORD dwType;				/*!< 报警类型                               */
	DWORD dwDepict;				/*!< 报警信息描述                           */
	DWORD dwReserve[DEF_RESERVE_NUM];	/*!< 保留                                   */
} AlarmNotify, *LPAlarmNotify;

/*!
*  @struct	tagAppLogQuery
*  @brief	应用层日志查询信息
*/
typedef struct tagAppLogQuery
{
	BYTE bytQueryMode;			/*!< 查询模式                               */
	BYTE bytMajorType;			/*!< 主类型                                 */
	BYTE bytMinorType;			/*!< 次类型                                 */
	BYTE bytReserve[1];			/*!< 保留                                   */
	DWORD dwChannel;			/*!< 通道号                                 */
	TimeInfo cBeginTime;		/*!< 开始时间                               */
	TimeInfo cEndTime;			/*!< 结束时间                               */
	DWORD dwReserve[DEF_RESERVE_NUM];	/*!< 保留                                   */
} AppLogQuery, *LPAppLogQuery;

/*!
*  @struct	tagAppLogInfo
*  @brief	应用层日志信息
*/
typedef struct tagAppLogInfo
{
	TimeInfo cLogTime;			/*!< 日志时间                               */
	BYTE bytMajorType;			/*!< 主类型                                 */
	BYTE bytMinorType;			/*!< 次类型                                 */
	BYTE bytDetail;				/*!< 详细信息                               */
	BYTE bytReserve[1];			/*!< 保留                                   */
	DWORD dwChannel;			/*!< 通道                                   */
	char sUserName[USERNAME_LEN];	/*!< 操作用户                               */
	DWORD dwUserIPAddress;		/*!< 操作用户IP地址                         */
	DWORD dwReserve[DEF_RESERVE_NUM];	/*!< 保留                                   */
} AppLogInfo, *LPAppLogInfo;

/*!
*  @struct	tagHistoryQueryInfo
*  @brief	历史查询信息
*/
typedef struct tagHistoryQueryInfo
{
	DWORD dwChannel;			/*!< 通道号                                 */
	DWORD dwDiskGroup;			/*!< 盘组信息                               */
	eHistoryStreamType eStreamType;	/*!< 历史流类型                             */
	char cBeginTime[15];		/*!< 起始时间                               */
	char cEndTime[15];			/*!< 结束时间                               */
	char cTicket[HXHT_COMMON_STRING_LEN]; /*!< 验证信息，针对Hxht           */
} HistoryQueryInfo, *LPHistoryQueryInfo;

/*!
*  @struct	tagHistoryQueryInfo
*  @brief	历史查询信息
*/
typedef struct tagHistoryQueryInfoMultiType
{
	DWORD dwChannel;			/*!< 通道号                                 */
	DWORD dwDiskGroup;			/*!< 盘组信息                               */
	DWORD dwStreamType;			/*!< 历史流类型                             */
	char cBeginTime[15];		/*!< 起始时间                               */
	char cEndTime[15];			/*!< 结束时间                               */
} HistoryQueryInfoMultiType, *LPHistoryQueryInfoMultiType;

/*!
*  @struct	tagFileQueryInfo
*  @brief	文件查询信息
*/
typedef struct tagFileQueryInfo
{
	DWORD dwChannel;			/*!< 通道号                                 */
	DWORD dwFileType;			/*!< 文件流类型                             */
	char cBeginTime[DATE_TIME_LEN];	/*!< 起始时间                               */
	char cEndTime[DATE_TIME_LEN];	/*!< 结束时间                               */
} FileQueryInfo, *LPFileQueryInfo;

/*!
*  @struct	tagHistoryQueryResult
*  @brief	历史查询结果信息
*/
typedef struct tagHistoryQueryResultMultiType
{
	DWORD dwStreamType;			/*!< 历史流类型                             */
	DWORD dwChannel;			/*!< 通道号                                 */
	char cBeginTime[15];		/*!< 开始时间                               */
	char cEndTime[15];			/*!< 结束时间                               */
	DWORD dwDataSize;			/*!< 数据大小                               */
} HistoryQueryResultMultiType, *LPHistoryQueryResultMultiType;

/*!
*  @struct	tagHistoryQueryResult
*  @brief	历史查询结果信息
*/
typedef struct tagHistoryQueryResult
{
	eHistoryStreamType eStreamType;			/*!< 历史流类型                             */
	DWORD dwChannel;			/*!< 通道号                                 */
	char cBeginTime[15];		/*!< 开始时间                               */
	char cEndTime[15];			/*!< 结束时间                               */
	DWORD dwDataSize;			/*!< 数据大小                               */
} HistoryQueryResult, *LPHistoryQueryResult;

/*!
*  @struct	tagFileQueryResult
*  @brief	文件查询结果信息
*/
typedef struct tagFileQueryResult
{
	DWORD dwChannel;			/*!< 通道号                                 */
	DWORD dwLock;				/*是否锁定                                  */
	DWORD dwDataSize;			/*!< 数据大小                               */
	DWORD dwFileType;			/*!< 图片类型                               */
	char cCreateTime[DATE_TIME_LEN];	/*!< 图片时间                               */
	char cFileName[MAX_FILENAME];	/*!< 图片设备端路径                         */
} FileQueryResult, *LPFileQueryResult;

/*!
*  @struct	tagHistoryQueryResultSet
*  @brief	历史查询结果集
*/
typedef struct tagHistoryQueryResultSet
{
	HistoryQueryResult cHistoryQueryResult;	/*!< 历史查询结果信息                       */
	struct tagHistoryQueryResultSet *pNext;	/*!< 下一个结果                             */
} HistoryQueryResultSet, *LPHistoryQueryResultSet;



/*!
*  @enum	eHistoryLogMajorType
*  @brief	日志查询主类型
*/
typedef enum eHistoryLogMajorType
{
	eHistoryLogMajorTypeBegin = 0,	/*!< 起始值                                 */
	eHistoryLogSysOperation,	/*!< 系统管理日志                           */
	eHistoryLogSysSetting,		/*!< 系统配置日志                           */
	eHistoryLogRecord,			/*!< 录像日志                               */
	eHistoryLogUserManage,		/*!< 用户管理日志                           */
	eHistoryLogAlarm,			/*!< 设备报警日志                           */
	eHistoryLogException,		/*!< 设备异常日志                           */
	eHistoryLogStorage,			/*!< 存储管理日志                           */
	eHistoryLogNetWork,			/*!< 网络事件								*/
	eHistoryLogMajorAll,		/*!< 忽略主类型                             */
	eHistoryLogAIGate,			/*!< 智能门禁事件日志						*/
	eHistoryLogMajorTypeEnd,	/*!< 结束值                                 */
}eHistoryLogMajorType;

/*!
*  @enum	eHistoryLogMinorType
*  @brief	日志查询子类型
*/
typedef enum eHistoryLogMinorType
{
	eHistoryLogSysOp = 0,		/*!< 系统操作所有子项                       */
	eHistoryLogSysOpUpdate,		/*!< 系统升级                               */
	eHistoryLogSysOpPTZControl,	/*!< 云台控制                               */
	eHistoryLogSysOpIllShutdown,	/*!< 非法关机                               */
	eHistoryLogSysOpPowerOff,	/*!< 正常关机                               */
	eHistoryLogSysOpPowerOn,	/*!< 开机                                   */
	eHistoryLogSysOpRemoteReset,	/*!< 远程重启                               */
	eHistoryLogSysOpClearAlarm,	/*!< 消警                                   */
	eHistoryLogLocalPlayback,							/*!< 本地回放								*/
	eHistoryLogLocalBackup,								/*!< 本地备份								*/
	eHistoryLogRemotePlaybackAndBackup,					/*!< 远程备份和回放							*/

	eHistoryLogSysSetAll = 100,	/*!< 系统设置所有子项                       */
	eHistoryLogSysSetTimeSetting,	/*!< 录像时间表设置                         */
	eHistoryLogSysSetCommonSetting,	/*!< 普通设置                               */
	eHistoryLogSysSetCodecSetting,	/*!< 编码设置                               */
	eHistoryLogSysSetNetSetting,	/*!< 网络设置                               */
	eHistoryLogSysSetServerSetting,	/*!< 服务器设置                             */
	eHistoryLogSysSetMotionSetting,	/*!< 移动侦测设置                           */
	eHistoryLogSysSetPTZSetting,	/*!< 云台解码器设置                         */
	eHistoryLogSysSetDefaultSetting,	/*!< 恢复默认设置                           */
	eHistoryLogSysSetDisplayDeviceSetting,	/*!< 设置显示设备参数                       */
	eHistoryLogSysSetDisplayModeSetting,	/*!< 显示模式设置                           */
	eHistoryLogSysSetPictureSetting,	/*!< 图像颜色设置                           */
	eHistoryLogSysSetMaintainSetting,	/*!< 自动维护设置                           */
	eHistoryLogSysSetAlarmOutSetting,	/*!< 报警输出设置                           */
	eHistoryLogSysSetAlarmInSetting,	/*!< 报警输入设置                           */
	eHistoryLogSysSetCustomizeFunction,	/*!< 功能定制设置                           */
	eHistoryLogSysSetImportPTZProtocol,	/*!< 导入云台协议                           */
	eHistoryLogSysSetExportPTZProtocol,	/*!< 导出云台协议                           */
	eHistoryLogSysSetImportSettings,	/*!< 导入参数配置                           */
	eHistoryLogSysSetExportSettings,	/*!< 导出参数配置                           */
	eHistoryLogSysSetExceptionSetting,	/*!< 异常参数配置                           */
	eHistoryLogSysSetIpConflick = 122,	/*!< IP冲突									*/
	eHistoryLogSysSetSysTimeSetting,	/*!< 系统时间参数配置                           */
	eHistoryLogSysSetSysFaceSnapSetting,	/*!< 人脸抓拍参数配置                           */

	eHistoryLogRecordAll = 200,	/*!< 录像控制所有子项                       */
	eHistoryLogRecordStartManual,	/*!< 启动手动录像                           */
	eHistoryLogRecordStartAuto,	/*!< 启动自动录像                           */
	eHistoryLogRecordStop,		/*!< 停止结束                               */

	eHistoryLogUserAll = 300,	/*!< 用户管理所有子项                       */
	eHistoryLogUserAdd,			/*!< 添加用户                               */
	eHistoryLogUserDelete,		/*!< 删除用户                               */
	eHistoryLogUserEdit,		/*!< 修改用户资料                           */
	eHistoryLogUserLogin,		/*!< 用户登录                               */
	eHistoryLogUserLogout,		/*!< 用户登出                               */
	eHistoryLogUserLockOut,		/*!< 用户锁定                               */
	eHistoryLogUserEditPrivilege,		/*!< 修改用户权限                           */

	eHistoryLogAlarmAll = 400,	/*< 报警所有子项                            */
	eHistoryLogAlarmMotionStart,	/*!< 移动侦测开始                           */
	eHistoryLogAlarmMotionStop,	/*!< 移动侦测结束                           */
	eHistoryLogAlarmInputAlarm,	/*!< 输入报警开始                           */
	eHistoryLogFaceDetectStart,	/*!< 人脸检测开始                           */
	eHistoryLogFaceDetectStop,	/*!< 人脸检测结束                           */

	eHistoryLogExceptionAll = 500,	/*!< 异常所有子项                           */
	eHistoryLogExceptionIpConflict,	/*!< IP冲突                                 */
	eHistoryLogExceptionHardDiskFull,	/*!< 硬盘满                                 */
	eHistoryLogExceptionHardDiskError,	/*!< 硬盘出错                               */
	eHistoryLogExceptionIllegalAccess,	/*!< 非法访问                               */
	eHistoryLogExceptionSignalLost,	/*!< 信号丢失                               */
	eHistoryLogExceptionSignalRecover,	/*!< 信号恢复                               */
	eHistoryLogExceptionNetDisconnect,	/*!< 网线断                                 */
	eHistoryLogExceptionNetReconnect,	/*!< 网线恢复                                 */

	eHistoryLogStorageAll = 600,	/*!< 存储管理所有子项                       */
	eHistoryLogStorageFormatPartition,	/*!< 分区格式化                             */
	eHistoryLogStorageAddPartition,	/*!< 添加分区                               */
	eHistoryLogStorageRemovePartition,	/*!< 删除分区　                             */
	eHistoryLogStorageUnmountDisk,	/*!< 卸载硬盘                               */
	eHistoryLogStorageDiskGroupOperation,	/*!< 磁盘分组管理                           */

	eHistoryLogNetworkAll = 700,				/*!< 网络事件所有子项						*/
	eHistoryLogNetworkConnectSuccess,					/*!< 网络连接成功							*/
	eHistoryLogNetworkConnectFailed,					/*!< 网络连接失败							*/
	eHistoryLogNetworkDisconnect,						/*!< 网络断开时间　							*/
	eHistoryLogNetworkRecover,							/*!< 网络恢复								*/
	eHistoryLogNetworkActiveDisconnect,					/*!< 网络主动断开							*/

	eHistoryLogAIGateAll = 800,				/*!< 智能门禁事件所有子项					*/
	eHistoryLogAIGateFaceRecognitionOpen,				/*!< 人脸比对开门（EventFaceRecognition）*/
	eHistoryLogAIGateKeyboardOpen,						/*!< 密码开门（EventGateKeyboard）			*/
	eHistoryLogAIGateCardOpen,							/*!< 刷卡开门（EventGateCard）				*/
	eHistoryLogAIGateButtonOpen,						/*!< 按键开门（无）							*/
	eHistoryLogAIGateRemoteOpen,						/*!< 远程开门（EventGateRemote）			*/
}eHistoryLogMinorType;


/*!
*  @struct	tagLogQueryInfo
*  @brief	历史查询信息
*/
typedef struct tagLogQueryInfo
{
	DWORD m_dwQueryMode;		/*!< 查询方式                               */
	eHistoryLogMajorType m_eMajorType;	/*!< 主类型                                 */
	eHistoryLogMinorType m_eMinorType;	/*!< 次类型                                 */
	DWORD m_dwChannel;			/*!< 通道号                                 */
	char m_sStartTime[DATE_TIME_LEN];	/*!< 起始时间                               */
	char m_sStopTime[DATE_TIME_LEN];	/*!< 结束时间                               */
} LogQueryInfo, *LPLogQueryInfo;

/*!
*  @struct	tagLogQueryResult
*  @brief	历史查询结果信息
*/
typedef struct tagLogQueryResult
{
	eHistoryLogMajorType m_eMajorType;	/*!< 主类型                                 */
	eHistoryLogMinorType m_eMinorType;	/*!< 次类型                                 */
	DWORD m_dwDetailInfo;		/*!< 详细信息                               */
	char m_sUserName[USERNAME_LEN];	/*!< 操作用户                               */
	char m_sUserIP[IP_ADDRESS_LEN];	/*!< 用户IP地址                             */
	char m_sLogTime[DATE_TIME_LEN];	/*!< 操作时间                               */
} LogQueryResult, *LPLogQueryResult;

/*!
*  @struct	tagLogQueryResultV1
*  @brief	历史查询结果信息
*/
typedef struct tagLogQueryResultV1
{
	eHistoryLogMajorType m_eMajorType;		/*!< 主类型                                 */
	eHistoryLogMinorType m_eMinorType;		/*!< 次类型，日志描述结构体见具体类型注释   */
	DWORD m_dwDetailInfo;					/*!< 详细信息                               */
	char m_sUserName[USERNAME_LEN];			/*!< 操作用户                               */
	char m_sUserIP[IP_ADDRESS_LEN];			/*!< 用户IP地址                             */
	char m_sLogTime[DATE_TIME_LEN];			/*!< 操作时间                               */
	char m_cReserve[1];						/*!< 保留字段								*/
	BYTE m_cDepict[MAX_CFG_LEN_512BYTE];	/*!< 日志描述，根据次类型对应的结构体解析	*/
} LogQueryResultV1, *LPLogQueryResultV1;

/*!
*  @struct	tagHistoryQueryResultSet
*  @brief	历史查询结果集
*/
typedef struct tagLogQueryResultSet
{
	LogQueryResult cLogQueryResult;	/*!< 历史查询结果信息                       */
	struct tagLogQueryResultSet *pNext;	/*!< 下一个结果                             */
} LogQueryResultSet, *LPLogQueryResultSet;

/*!
*  @struct	tagHistoryChannelParaInfo
*  @brief	历史通道启动参数信息
*/
typedef struct tagHistoryChannelParaInfo
{
	DWORD dwEnableEndTime;		/*!< 结束时间是否有效                       */
	TimeInfo cBeginTime;		/*!< 开始时间                               */
	TimeInfo cEndTime;			/*!< 结束时间                               */
	DWORD dwChannel;			/*!< 录像通道                               */
	DWORD dwDiskGroup;			/*!< 盘组信息                               */
	eHistoryStreamType eType;	/*!< 历史流类型                             */
	DWORD dwTaskType;			/*!< 任务类型                               */
	char	cStreamMediaType;							/*!< 媒体流类型 0表示主码流，1表示子码流	*/
	char	cReserve[3];								/*!< 保留									*/
	DWORD	dwReserve[DEF_RESERVE_NUM - 1];				/*!< 保留									*/
} HistoryChannelParaInfo, *LPHistoryChannelParaInfo;

/*!
*  @struct	tagHistoryChannelParaInfoMultiType
*  @brief	历史通道启动参数信息(扩展)
*/
typedef struct tagHistoryChannelParaInfoMultiType
{
	DWORD dwEnableEndTime;		/*!< 结束时间是否有效                       */
	TimeInfo cBeginTime;		/*!< 开始时间                               */
	TimeInfo cEndTime;			/*!< 结束时间                               */
	DWORD dwChannel;			/*!< 录像通道                               */
	DWORD dwDiskGroup;			/*!< 盘组信息                               */
	DWORD dwType;				/*!< 历史流类型                             */
	DWORD dwTaskType;			/*!< 任务类型                               */
	char	cStreamMediaType;							/*!< 媒体流类型 0表示主码流，1表示子码流	*/
	char    cSinglePlayback;       //0,表示单通道模式，1，表示多通道模式
	char	cReserve[3-1]	;								/*!< 保留									*/
	DWORD	dwReserve[DEF_RESERVE_NUM - 1];				/*!< 保留									*/
} HistoryChannelParaInfoMultiType, *LPHistoryChannelParaInfoMultiType;

/*!
*  @struct	tagHistoryChannelParaInfoMultiTypeV1
*  @brief	历史通道启动参数信息(扩展)
*/
typedef struct tagHistoryChannelParaInfoMultiTypeV1
{
	DWORD dwMethod;				/*!< 启动历史流方法，0，tagHistoryChannelParaInfoMultiType结构体对应的方式，1.使用文件名的方式 */
	DWORD dwEnableEndTime;		/*!< 结束时间是否有效                       */
	TimeInfo cBeginTime;		/*!< 开始时间                               */
	TimeInfo cEndTime;			/*!< 结束时间                               */
	DWORD dwChannel;			/*!< 录像通道                               */
	DWORD dwDiskGroup;			/*!< 盘组信息                               */
	DWORD dwType;				/*!< 历史流类型                             */
	DWORD dwTaskType;			/*!< 任务类型                               */
	char	cStreamMediaType;							/*!< 媒体流类型 0表示主码流，1表示子码流	*/
	char 	cFileName[MAX_FILENAME];					/*!< 文件名，当dwMethod为1时有效			*/
	char	cReserve[3];								/*!< 保留									*/
	DWORD	dwReserve[DEF_RESERVE_NUM - 1];				/*!< 保留									*/
} HistoryChannelParaInfoMultiTypeV1, *LPHistoryChannelParaInfoMultiTypeV1;

/*!
*  @struct tagHistoryChannelParaInfoSrtMode
*  @brief	历史通道启动参数信息
*/
typedef struct tagHistoryChannelParaInfoSrtMode
{
	DWORD dwChannel;

	char szReserved[64];
}HistoryChannelParaInfoSrtMode,*LPHistoryChannelParaInfoSrtMode;

/*!
*  @struct tagHistoryChannelFrameInfoSrtMode
*  @brief	历史通道帧数据信息（Srt协议找中间层拉取历史流数据信息）
*/
typedef struct tagHistoryChannelVideoFrameInfoSrtMode
{
	int nIsKeyFrame;		//是否是I帧
	short nWidth;
	short nHeight;
	short nFramerate;		//帧率
	short nGop;

	char szReserved[64];		//预留
}HistoryChannelVideoFrameInfoSrtMode,*LPHistoryChannelVideoFrameInfoSrtMode;

typedef struct tagHistoryChannelAudioFrameInfoSrtMode 
{
	int nSamplesRate; //采样率
	short nChannelNumber; //声道数
	short nBitsPerSample; //位深
	int nSamplePerFrame; //每帧采样点数

	char szReserved[64]; //预留
}HistoryChannelAudioFrameInfoSrtMode,*LPHistoryChannelAudioFrameInfoSrtMode;

typedef struct tagHistoryChannelFrameInfoSrtMode
{
	short nFrameType;	//帧类型，0:视频，1:音频
	short nCodecType;	//编码类型，0：H264，1：AAC with ADTS，4：H265
	long long nTimestamp; //绝对时间戳
	HistoryChannelVideoFrameInfoSrtMode struVideoInfo;
	HistoryChannelAudioFrameInfoSrtMode struAudioInfo;
	char *pFrameData; //帧内容
	int nFrameLen; //帧长度

	char szReserved[64]; //预留
}HistoryChannelFrameInfoSrtMode,*LPHistoryChannelFrameInfoSrtMode;


/*!
*  @struct	tagHistoryChannelStream
*  @brief	历史通道流信息
*/
typedef struct tagHistoryChannelStream
{
	DWORD dwChannel;			/*!< 通道号                                 */
	Buffer cBuffer;				/*!< 帧缓冲区                               */
	AbsoluteTime cTime;			/*!< 帧时间                                 */
	/*!<
	   帧类型：
	   当帧的格式变换时，将帧类型置为 HISTORY_STREAM_SWITCH_FRAME；
	   当帧为结束帧时，将帧类型置为 HISTORY_STREAM_TIME_END
	 */
	DWORD dwFrameType;
	DWORD dwReserve[DEF_RESERVE_NUM];	/*!< 保留                                   */
} HistoryChannelStream, *LPHistoryChannelStream;

/*!
*  @struct	tagHistoryChannelPositionInfo
*  @brief	历史通道定位参数信息
*/
typedef struct tagHistoryChannelPositionInfo
{
	TimeInfo cSeekTime;			/*!< 定位时间                               */
} HistoryChannelPositionInfo, *LPHistoryChannelPositionInfo;

/*!
*  @struct	tagHistoryChannelPositionInfoV1
*  @brief	历史通道定位参数信息
*/
typedef struct tagHistoryChannelPositionInfoV1
{
	unsigned int uiSeektimestamp;			/*!< 定位时间                               */
} HistoryChannelPositionInfoV1, *LPHistoryChannelPositionInfoV1;

/*!
*  @struct	 tagHistoryChannelPositionInfo
*  @brief	 历史通道暂停参数信息
*/
typedef struct tagHistoryChannelPauseInfo
{
	int nCommand;		//0：暂停		1：继续

	char szReserved[32];
}HistoryChannelPauseInfo, *LPHistoryChannelPauseInfo;

/*!
*  @struct	 tagHistoryChannelFileFormatInfo
*  @brief	 历史通道录像文件信息
*/

typedef struct tagHistoryChannelRecordFileParam
{
	int nChannel;
	char szFileName[MAX_FILENAME];					//录像文件的绝对路径

	char szReserved[1024];
}HistoryChannelRecordFileParam, *LPHistoryChannelRecordFileParam;

/*!
*  @struct	 tagHistoryChannelRecordFileVideoFormat
*  @brief	 历史通道录像文件视频格式信息
*/
typedef struct tagHistoryChannelRecordFileVideoFormat
{
	int nVideoCodecID;											/*!< 0,H264,1,H265,2,MJPEG */
	int nVideoBitrate;
	int nResolutionWidth;										/*!< 分辨率的宽 */
	int nResolutionHeight;										/*!< 分辨率的高 */
	int nVideoFrameRate;										/*!< 帧率*/
	int nColorDepth;
	int nKeyframeinterval;										/*!< 帧间隔*/

	int nQuality;						//OVD用，视频质量, 0低清 1标清 2高清 3超高清
	int nEncrypt;						//OVD用，是否是加密的

	char szReserved[128];
}HistoryChannelRecordFileVideoFormat, *LPHistoryChannelRecordFileVideoFormat;

/*!
*  @struct	 tagHistoryChannelRecordFileAudioFormat
*  @brief	 历史通道录像文件音频格式信息
*/
typedef struct tagHistoryChannelRecordFileAudioFormat
{
	int nEncoder;			//0:G711U	1:G711A	2:ACC
	int nSampleRate;		//采样率
	int nBitRate;				//码流比特率
	int nWaveFormat;
	int nChannel;			//声道数
	int nBlockAlign;
	int nBitPerSample;	//位宽
	int nSamplePerFrame;	//每一帧中包含的sample数，AAC算法标准固定为1024
	int nFrameInterval;

	char szReserved[128];
}HistoryChannelRecordFileAudioFormat, *LPHistoryChannelRecordFileAudioFormat;


typedef struct tagServerInformation
{
	char cAddress[IP_ADDRESS_LEN];	/*!< 地址                                   */
	DWORD dwCommandPort;		/*!< 信令端口                               */
	DWORD dwMediaPort;			/*!< 媒体端口                               */
} ServerInformation, *LPServerInformation;


/*!
*  @struct	tagTransparentParaInfo
*  @brief	透明传输参数信息
*/
typedef struct tagTransparentParaInfo
{
	DWORD dwMajorType;			/*!< 主类型信息                             */
	DWORD dwMinorType;			/*!< 子类型信息                             */
	Buffer cBuffer;				/*!< 参数信息                               */
	DWORD dwReserve[DEF_RESERVE_NUM];	/*!< 保留                                   */
} TransparentParaInfo, *LPTransparentParaInfo;


/*!
*  @struct	tagDeviceRegisterNetConfig
*  @brief	设备注册网络设置
*/
typedef struct tagDeviceRegisterNetConfig
{
	char cCentralIP[IP_ADDRESS_LEN];	/*!< 注册中心IP地址                         */
	DWORD dwCentralPort;		/*!< 注册中心端口                           */
	DWORD dwInterval;			/*!< 注册时间间隔(s                         */
} DeviceRegisterNetConfig, *LPDeviceRegisterNetConfig;

/*!
*  @struct	tagDeviceRegisterInfo
*  @brief	设备注册信息（私有协议用，设备ID是DWORD型）
*/
typedef struct tagDeviceRegisterInfo
{
	DWORD dwDeviceID;							/*!< 设备ID								*/
	char cDeviceIP[IP_ADDRESS_LEN];				/*!< 设备IP地址                         */
	char cDeviceMAC[MAC_ADDRESS_LEN];			/*!< 设备MAC地址                        */
	DWORD dwDeviceCmdPort;						/*!< 设备命令端口                       */
	DWORD dwHTTPPort;							/*!< HTTP端口                           */
	DWORD dwDeviceType;							/*!< 设备类型                           */
	char cDeviceVersion[VERSION_INFO_LEN];		/*!< 设备软件版本号                     */
	DWORD dwDeviceMaxConnect;					/*!< 视频的最大连接数                   */
} DeviceRegisterInfo, *LPDeviceRegisterInfo;

/*!
*  \struct	tagDeviceRegisterInfoV1
*  \brief	设备注册信息
*/
typedef struct tagDeviceRegisterInfoV1
{
	DWORD	m_bytDevType;								/*!< 设备类型(Read)							*/
	DWORD	m_bytBoardID;								/*!< 设备型号(Read)							*/

	WORD	m_bytDevDetectPort;							/*!< 设备自动发现监听端口					*/
	WORD	m_wCmdPort;									/*!< 设备命令端口							*/
	WORD	m_wMediaPort;								/*!< 设备媒体端口							*/
	WORD	m_wHttpPort;								/*!< 设备HTTP端口							*/
	WORD	m_wRTSPPort;								/*!< 设备RTSP端口							*/
	BYTE	m_bytPadding1[2];							/*!< 保留字段								*/

	BYTE	m_bytDevChan;								/*!< 设备通道个数(Read)						*/
	BYTE	m_bytDevRegInterval;						/*!< 设备注册间隔时间						*/
	BYTE	m_bytDevMac[MAC_BINARY_ADDRESS_LEN];		/*!< 设备MAC地址(Read)						*/

	char	m_bytDevIP[IP_ADDRESS_LEN];					/*!< 设备当前IP地址							*/
	char	m_bytDevMask[IP_ADDRESS_LEN];				/*!< 设备IP地址掩码							*/
	char	m_bytDevGateway[IP_ADDRESS_LEN];			/*!< 设备网关地址							*/
	char	m_bytDeviceVersion[DVR_MAX_DEVICE_VERSION_LEN]; /*!< 设备版本信息(只读，不占用下面的有效数据)*/
	char	m_bytP2PUUID[DVR_MAX_P2P_UUID_LEN];			/*!< P2P UUID(只读，不占用下面的有效数据)	*/

	char 	m_szTypeName[32];							/*!< 设备型谱名称(ReadOnly)					*/
	char 	m_szSerialNum[32];							/*!< 序列号(ReadOnly)						*/
	char 	m_szHardwareVersion[32];					/*!< 硬件版本(ReadOnly) 					*/
	char 	m_szSoftwareVersion[32];					/*!< 软件版本(ReadOnly) 					*/
	char 	m_szAlgorithmVersion[32];					/*!< 算法版本(ReadOnly) 					*/
	char 	m_szDspVersion[32];							/*!< 编码版本(ReadOnly) 					*/

	BYTE	m_byVideoInChannels;						/*!< 视频输入路数(ReadOnly)  				*/
	BYTE	m_byAudioInChannels;						/*!< 音频输入路数(ReadOnly)  				*/
	BYTE	m_byAlarmInChannels;						/*!< 报警输入路数(ReadOnly)  				*/
	BYTE	m_byAlarmOutChannels;						/*!< 报警输出路数(ReadOnly)  				*/

	BYTE	m_byVGAChannels;							/*!< VGA接口个数(ReadOnly)  				*/
	BYTE	m_bySpotOutChannles;						/*!< SPOT输出路数(ReadOnly)  				*/
	BYTE	m_byVoiceInChannels;						/*!< 语音对讲输入路数(ReadOnly)  			*/
	BYTE	m_byP2PType;								/*!< P2P类型，0，不支持，1，tutk,2,ulk		*/

	BYTE	m_bytNetworkType;							/*!< 网卡类型, 参见eWhichNetcardUsed		*/
	BYTE	m_bytDHCP;									/*!< DHCP开关(ReadOnly)
																 0, 表示本字段无效(老设备都会是0)
																 1，静态设置
																 2，动态获取						*/
	BYTE	m_bytAutoGenIP;								/*!< IP自适应开关(ReadOnly)
																 0, 表示本字段无效(老设备都会是0)
																 1，关
																 2，开								*/
	BYTE	m_cAuthState;								/*!< 授权状态0，表示未授权，1，表示已授权	*/

	char	m_bytPushNum[8];							/*!< 推送号									*/
	char	m_cDeviceName[64];							/*!< 设备名字 UTF8							*/
	DWORD	m_nCurrentTime;								/*!< 当前时间    							*/
	long	m_nTimeZone;								/*!< 当前时区，以分钟为单位，    			*/
	DWORD	m_dwMaxVideoConnect;						/*!< 视频的最大连接数						*/
	char	m_Reserve[256];								/*!< 保留字段								*/
}DeviceRegisterInfoV1, *LPDeviceRegisterInfoV1;

/*!
*  @struct	tagDeviceRegisterInfoHxht
*  @brief	设备注册信息（Hxht协议用，设备ID是字符串）
*/
typedef struct tagDeviceRegisterInfoHxht
{
	char cDeviceID[MAX_DEVICE_ID_LEN];			/*!< 设备序列号							*/
	char cDeviceIP[IP_ADDRESS_LEN];				/*!< 设备IP地址                         */
	char cDeviceMAC[MAC_ADDRESS_LEN];			/*!< 设备MAC地址                        */
	DWORD dwDeviceCmdPort;						/*!< 设备命令端口                       */
	DWORD dwHTTPPort;							/*!< HTTP端口                           */
	DWORD dwDeviceType;							/*!< 设备类型                           */
	char cDeviceVersion[VERSION_INFO_LEN];		/*!< 设备软件版本号                     */
	DWORD dwDeviceMaxConnect;					/*!< 视频的最大连接数                   */
} DeviceRegisterInfoHxht, *LPDeviceRegisterInfoHxht;

/*!
*  @struct	tagDeviceStoreRegisterInfo
*  @brief	设备存储注册信息
*/
typedef struct tagDeviceStoreRegisterInfo
{
	char cDeviceID[MAX_DEVICE_ID_LEN];				/*!< 设备序列号＋"_T"					*/
	char cDeviceLinkType;							/*!< 设备连接类型						*/
	char cDeviceIP[IP_ADDRESS_LEN];					/*!< 设备IP地址							*/
	char cDeviceIP2[IP_ADDRESS_LEN];				/*!< 设备IP地址							*/
	DWORD dwDeviceMsgPort;							/*!< 设备消息端口						*/
	DWORD dwDeviceVideoPort;						/*!< 设备视频连接端口					*/
} DeviceStoreRegisterInfo, *LPDeviceStoreRegisterInfo;

/*!
*  @struct	tagCentralRetInfo
*  @brief	中心平台注册信息
*/
typedef struct tagCentralRetInfo
{
	char cCentralTime[DATE_TIME_LEN];	/*!< 注册平台中心时间                   */
} CentralRetInfo, *LPCentralRetInfo;

typedef struct tagRegisterInfo
{
	DWORD dwDeviceID;			/*!< 设备ID                                 */
	BYTE cDeviceIP[IP_ADDRESS_LEN];	/*!< 设备IP                                 */
	DWORD dwDevicePort;			/*!< 设备注册端口                           */
	BYTE cDeviceMAC[MAC_ADDRESS_LEN];	/*!< 设备MAC地址                            */
	DWORD dwHttpPort;			/*!< Http端口                               */
	DWORD dwDeviceType;			/*!< 设备IP                                 */
	char cDeviceVersion[VERSION_INFO_LEN];	/*!< 设备版本信息                           */
	DWORD dwDeviceMaxConnect;	/*!< 设备最大连接数                         */
} RegisterInfo, *LPRegisterInfo;

/*!
*  @enum	eTransferFileType
*  @brief	传输文件类型
*/
typedef enum eTransferFileType
{
	eTransferFileTypeBegin = 0,		/*!< 起始值                                 */
	eFileSystemUpdate,				/*!< 升级包文件                             */
	ePtzProtocol,					/*!< 云台协议文件                           */
	eDeviceConfig,					/*!< 设备配置文件                           */
	ePicture,						/*!< 图片文件								*/
	eRecordSetFile,					/*!< 历史记录查询文件						*/
	eDeviceDetectListFile,			/*!< 设备探测列表文件						*/
	eIspColorConfig,					/*!< 图像tone curve 数据文件>               */
	eTransferFileTypeAuthFile,				/*!< 授权文件				                */
	eTransferFileTypeConfigAndCap,			/*!< 配置和能力          	       	        */
	eTransferFileTypeAudioData,				/*!< 语音数据包          	       	        */
	eTransferFileTypeFaceReco,				/*!< 人脸识别	          	       	        */
	eTransferFileTypeStartupLogo,				/*!< 开机LOGO                         */
	eTransferFileTypeChannelLogo,					/*!< 通道LOGO                         */
	eTestSDCard,							/*!< 测试SD卡                         */
	eTransferFileTypeCloudUpdate,			/*!< 云升级                         */
	eTransferFileTypeCustomConfigFileAll,			/*!< 灯板定制文件                        */
	eTransferFileTypeMenuLogo,					/*!< 菜单Logo							 */
	eTransferFileTypeScreenSaverLogo,			/*!< 屏保Logo							 */
	eTransferFileTypeAuthFileCar,        /*!<车牌Auth文件 */
	eTransferFileTypeSetDidCfgManul,        /*!<手动设置DID配置 */
	eTransferFileTypeCustomFileUpload,        /*!<定制文件上传 */
	eTransferFileAllLogFile,            	/*!<全部日志文件上传 */
    eTransferFileSerialLogFileUpload,   	/*!<串口日志文件上传 */
    eTransferFileProcLogFileUpload,     	/*!<proc日志文件上传 */
    eTransferFileAxLogFileUpload,       	/*!<axlog日志文件上传 */
    eAuthorizationFile,               /*!<授权文件信息 */
    eTransferWebBackgroundPicture,          /*!< Web 背景图片 */
    eTransferWebLogoPicture,                /*!< 网址标签logo */
	eTransferWebLoginLogoPicture,			/*!< Web 登录Logo 图片 */
	eAFBoardFileSystemUpdate,           /*!< AF机芯文件系统升级文件 */
	eTransferFileTypeAllAudioData,			/*!< 上传所有语音数据包(默认支持的语音包,eg:警报,犬吠,警戒区域... + 客户自定义的语音内容)      	        */
	eTransferTextOsd1Bmp,                   /*!< 定制的多国OSD1文件(多国OSD定制功能) 0通道第一个OSD */
    eTransferTextOsd2Bmp,                   /*!< 定制的多国OSD2文件(多国OSD定制功能,暂未使用) 0通道第二个OSD */
	eTransferFileTypeMainStreamBmpPicture,			/*!< 主码流BMP图片  */
    eTransferFileTypeSubStreamBmpPicture,			/*!< 子码流BMP图片  */
	eTransferHTTPSCertificationFile,				/*!< HTTPS证书文件(pem后缀)  */
    eTransferHTTPSCertificationKeyFile,				/*!< HTTPS密钥文件(key后缀)  */
	eTransferHTTPSCertificationCerFile,				/*!< HTTPS证书文件(cer后缀) ，证书下载*/
	eTransferTextOsd11Bmp,                   /*!< 图片OSD文件,1通道第一个OSD*/
    eTransferTextOsd12Bmp,                   /*!< 图片OSD文件,1通道第二个OSD  */
	eTransferFileTypeEnd,			/*!< 结束值                                 */
}eTransferFileType;


/*!
*  @struct	tagRemoteDownloadParaInfo
*  @brief	远程文件下载启动参数信息
*/
typedef struct tagRemoteDownloadParaInfo
{
	eTransferSpeed eSpeed;		/*!< 传输速度类型                           */
	eTransferFileType eFileType;	/*!< 文件类型                               */
	char cFileName[MAX_FILENAME];	/*!< 文件名                                 */
	DWORD dwFileTotalLen;		/*!< 文件总大小                             */
} RemoteDownloadParaInfo, *LPRemoteDownloadParaInfo;

/*!
*  @struct	tagRemoteDownloadState
*  @brief	远程文件下载状态信息
*/
typedef struct tagRemoteDownloadState
{
	eTransferSpeed eSpeed;		/*!< 传输速度类型                           */
	DWORD dwFileTransferLen;	/*!< 文件已传输大小                         */
	DWORD dwFileTotalLen;		/*!< 文件总大小                             */
	eRemoteDownloadState eState;	/*!< 文件传输状态                           */
} RemoteDownloadState, *LPRemoteDownloadState;

/*!
*  @struct	tagFileUploadParaInfo
*  @brief	远程文件上传启动参数信息
*/
typedef struct tagFileUploadParaInfo
{
	eTransferSpeed eSpeed;		/*!< 传输速度类型                           */
	eTransferFileType eFileType;	/*!< 文件类型                               */
	char cFileName[MAX_FILENAME];	/*!< 文件名                                 */
	DWORD dwFileTotalLen;		/*!< 文件总大小                             */
} FileUploadParaInfo, *LPFileUploadParaInfo;

/*!
*  @struct	tagFileUploadState
*  @brief	远程文件上传状态信息
*/
typedef struct tagFileUploadState
{
	eTransferSpeed eSpeed;		/*!< 传输速度类型                           */
	DWORD dwFileTransferLen;	/*!< 文件已传输大小                         */
	DWORD dwFileTotalLen;		/*!< 文件总大小                             */
	eRemoteDownloadState eState;	/*!< 文件传输状态                           */
} FileUploadState, *LPFileUploadState;



/*!
*  @struct	tagFileDownloadParaInfo
*  @brief	远程文件下载启动参数信息
*/
typedef struct tagFileDownloadParaInfo
{
	eTransferSpeed eSpeed;		/*!< 传输速度类型                           */
	DWORD dwFileType;			/*!< 文件类型                               */
	char cFileName[MAX_FILENAME];	/*!< 文件名                                 */
	DWORD dwFileTotalLen;		/*!< 文件总大小                             */
} FileDownloadParaInfo, *LPFileDownloadParaInfo;

/*!
*  @struct	tagFileDownloadState
*  @brief	远程文件下载状态信息
*/
typedef struct tagFileDownloadState
{
	eTransferSpeed eSpeed;		/*!< 传输速度类型                           */
	DWORD dwFileTransferLen;	/*!< 文件已传输大小                         */
	DWORD dwFileTotalLen;		/*!< 文件总大小                             */
	eRemoteDownloadState eState;	/*!< 文件传输状态                           */
} FileDownloadState, *LPFileDownloadState;

/*!
*  @struct	tagImageFileInfo
*  @brief	 抓图文件信息
*/
typedef struct tagImageFileInfo
{
	DWORD m_dwFileSize;			/*!< 文件大小                               */
	char m_sFileName[MAX_FILENAME];	/*!< 文件名                                 */
	char m_sCreateTime[DATE_TIME_LEN];	/*!< 文件创建时间                           */
} ImageFileInfo, *LPImageFileInfo;

/*!
*  \struct	tagRecordSetFileInfo
*  \brief	 历史记录查询文件信息
*/
typedef struct tagRecordSetFileInfo
{
	DWORD				m_dwFileSize;					/*!< 文件大小								*/
	char				m_sFileName[MAX_FILENAME];		/*!< 文件名									*/
	char				m_sCreateTime[DATE_TIME_LEN];	/*!< 文件创建时间							*/
}RecordSetFileInfo, *LPRecordSetFileInfo;

/*!
*  @struct	tagDataPacketType
*  @brief	文件传送媒体包类型
*/
typedef enum tagDataPacketType
{
	eDataTypeBegin = 0,
	eDataRequest,
	eDataPacket,
	eDataReply,
	eDataFinished,
	eDataCanceled,
	eDataError,
	eWriteError,
	eFlashFull,
	eFileCheckError,
	eFileAliveKeep,
	eDataTypeEnd,
} eDataPacketType;

/*!
*  @struct	tagFileTransferDataPacket
*  @brief	文件传送媒体包定义
*/
typedef struct tagFileTransferDataPacket
{
	eDataPacketType m_dwPacketType;	/* 0:数据请求（数据传输开始）   1:数据包　2:回应包　 3:结束包 */
	DWORD m_dwPacketSequence;
	DWORD m_dwPacketLen;
} FileTransferDataPacket, *LPFileTransferDataPacket;


/*!
*  @struct	tagDetectActionInfo
*  @brief	探测事件信息
*/
typedef struct tagDetectActionInfo
{
	eDetectAction eType;		/*!< 事件类型                               */
	char cServerIP[IP_ADDRESS_LEN];	/*!< 远程服务器地址                         */
	DWORD dwServerPort;			/*!< 远程服务器端口                         */
} DetectActionInfo, *LPDetectActionInfo;

/*!
*  @struct	tagDevAlarmInfo
*  @brief	设备报警信息
*/
typedef struct tagDevAlarmInfo
{
	char szEventName[HXHT_MAX_XML_STRING_LEN];				/*!< 事件名 */
	char szDevName[HXHT_MAX_XML_STRING_LEN];				/*!< 报警设备名 */
	DWORD dwDevType;										/*!< 报警设备类型（5：摄像头，6：报警输入，20：智能分析单元） */
	char szAlarmTime[HXHT_MAX_XML_STRING_LEN];				/*!< 报警触发时间 */
	DWORD dwAlarmType;										/*!< 越界检测、滞留检测、车速检测 */
	char szInfo[HXHT_MAX_XML_STRING_LEN];					/*!< 自定义的关于些报警事件的说明 */
// 	eDevType eDeviceType;								/*!< 报警设备类型 */
// 	eAlarmEventType eAlarmType;							/*!< 报警时间类型 */
// 	TimeInfo cAlarmTime;								/*!< 报警触发时间 */
// 	DWORD dwChannel;									/*!< 通道号 */
// 	char cDescriptionInfo[MAX_DESCRIPTION_INFO_LEN];	/*!< 报警信息描述*/
} DevAlarmInfo, *LPDevAlarmInfo;

/*!
*  @struct	tagDevFaultInfo
*  @brief	设备故障信息
*/
typedef struct tagDevFaultInfo
{
	char szDevIP[IP_ADDRESS_LEN];							/*!< 前端设备IP */
	char szCurrentTime[HXHT_MAX_XML_STRING_LEN];			/*!< 当前时间 */
	char szDevID[HXHT_COMMON_STRING_LEN];					/*!< 设备ID */
	DWORD dwDevType;										/*!< 设备类型编号 */
	DWORD dwFaultType;										/*!< 故障类型编号 */
	DWORD dwFaultID;										/*!< 故障编号 */
	char szFaultTime[HXHT_COMMON_STRING_LEN];				/*!< 故障产生时间 */
	DWORD dwIsClear;										/*!< 0（新故障） or 1（故障恢复） */
	//eDevType dwDeviceType;			/*!< 故障设备类型 */
	//eFaultType dwFaultType;			/*!< 故障类型 */
	//eFaultID dwFaultID;				/*!< 故障编号ID */
	//TimeInfo cAlarmTime;				/*!< 报警触发时间 */
	//DWORD dwChannel;					/*!< 通道号 */
	//eFaultState dwFaultState;			/*!< 故障状态 */
} DevFaultInfo, *LPDevFaultInfo;

/*!
*  @struct	tagQuerySupport
*  @brief	能力查询信息
*/
typedef struct tagQuerySupportInfo
{
	char acIsSupport[MAX_QUERY_SUPPORT_NUM][MAX_SUPPORT_NAME_LEN]; /*!< 功能名称						*/
}QuerySupportInfo, *LPQuerySupportInfo;

/*!
*  @struct	tagQuerySupportResult
*  @brief	能力查询结果
*/
typedef struct tagQuerySupportResult
{
	char acSupport[MAX_QUERY_SUPPORT_NUM][MAX_SUPPORT_NAME_LEN];	/*!< 支持的功能						*/
	char acUnSupport[MAX_QUERY_SUPPORT_NUM][MAX_SUPPORT_NAME_LEN];	/*!< 不支持的功能					*/
}QuerySupportResult, *LPQuerySupportResult;

/*!
*  @struct	tagSystemUpdateParam
*  @brief	系统升级参数
*/
typedef struct tagSystemUpdateParam
{
	char szFTPIP[IP_ADDRESS_LEN];									/*!< FTP服务器IP */
	DWORD dwFTPPort;												/*!< FTP服务器端口 */
	char szFTPUserName[USERNAME_LEN];								/*!< FTP服务器用户名 */
	char szFTPPassword[USERPASS_LEN];								/*!< FTP服务器用户密码 */
	char szFTPFilePath[MAX_FILENAME];								/*!< FTP服务器升级文件路径 */
	char szFTPFileName[MAX_FILENAME];								/*!< FTP服务器升级文件名 */
}SystemUpdateParam, *LPSystemUpdateParam;

/*!
*  @struct	tagThresholdParam
*  @brief	网管告警阀值
*/
// typedef struct tagThresholdParam
// {
// 	float fCPUUseRatio;												/*!< CPU利用率 */
// 	float fMemoryUseRatio;											/*!< 内存利用率 */
// 	float fDiskSpaceRatio;											/*!< 硬盘利用率 */
// 	DWORD dwDiskSpaceBalance;										/*!< 硬盘剩余空间 */
// }ThresholdParam, *LPThresholdParam;

/*!
* @struct tagTimeSchemeItem
* @brief 时间段
*/
typedef struct tagTimeSchemeItem
{
	char szItemDay[HXHT_COMMON_STRING_LEN];							/*!< 表示起止日期 */
	char szItemTime[HXHT_COMMON_STRING_LEN];						/*!< 表示起止时间 */
	/*举例：<Item Day=”1-3” Time="08:01:02-09:03:04"/>表示录像计划周期内的第一天到第三天，每天从8点1分2秒开始录像，
	到9点3分4秒结束。Item项数可以有多个，但最多不超过32个。多个Item指定的录像时间范围可能有重复，
	重复的时间范围能保证录像即可*/
}TimeSchemeItem, *LPTimeSchemeItem;

/*!
* @struct tagScheme
* @brief 布防时间
*/
typedef struct tagScheme
{
	char szSchemeCycle[HXHT_COMMON_STRING_LEN];						/*!< 计划周期：day按天为周期，week按周为周期，month按月为周期 */
	TimeSchemeItem cTimeScheme[HXHT_MAX_ITEM_COUNT];				/*!< 时间计划节点 */
	int nValidItem;													/*!< TimeSchemeItem有效的个数 */
}TimeScheme, *LPTimeScheme;

////////////////////////////////////Hxht//////////////////////////////////////

/*!
*  @struct	tagHxhtPTZSwitchStatus
*  @brief	Hxht PTZ开关状态
*/
typedef struct tagHxhtPTZSwitchStatus
{
	char acRandomScan[HXHT_COMMON_STRING_LEN];			/*!< 随机线扫开关 on--开 off-关												*/
	char acLight[HXHT_COMMON_STRING_LEN];				/*!< 灯光开关 on--开 off-关													*/
	char acWiper[HXHT_COMMON_STRING_LEN];				/*!< 雨刷开关 on--开 off-关													*/
	char acProbe[HXHT_COMMON_STRING_LEN];				/*!< 探头开关 on--开 off-关													*/
	char acPower[HXHT_COMMON_STRING_LEN];				/*!< 电源开关 on--开 off-关													*/
	char acAutoFocus[HXHT_COMMON_STRING_LEN];			/*!< 自动聚焦开关 on--开 off-关												*/
	char acAutoBright[HXHT_COMMON_STRING_LEN];			/*!< 自动光圈开关 on--开 off-关												*/
	char acBackLight[HXHT_COMMON_STRING_LEN];			/*!< 背光补偿开关 on--开 off-关												*/
	char acAutoNightVision[HXHT_COMMON_STRING_LEN];		/*!< 自动夜视开关 on--开 off-关												*/
	char acNightVision[HXHT_COMMON_STRING_LEN];			/*!< 夜视开关 on--开 off-关													*/
	char acWatchPoint[HXHT_COMMON_STRING_LEN];			/*!< 看首位开关	 on--开 off-关												*/
	char acAid1[HXHT_COMMON_STRING_LEN];				/*!< 辅助1开关	 on--开 off-关												*/
	char acAid2[HXHT_COMMON_STRING_LEN];				/*!< 辅助2开关	 on--开 off-关												*/
}HxhtPTZSwitchStatus, *LPHxhtPTZSwitchStatus;

/*!
*  @struct	tagHxhtPTZAuxInfo
*  @brief	Hxht 辅助开关信息
*/
typedef struct tagHxhtPTZAuxInfo
{
	DWORD dwNum;										/*!< 辅助开关编号															*/
	char acName[HXHT_COMMON_STRING_LEN];				/*!< 辅助开关名称															*/
	char acSwitch[HXHT_COMMON_STRING_LEN];				/*!< on/off																	*/
}HxhtPTZAuxInfo, *LPHxhtPTZAuxInfo;

/*!
*  @struct	taHxhtPTZAuxSwitch
*  @brief	Hxht 辅助开关
*/
typedef struct tagHxhtPTZAuxSwitch
{
	DWORD dwCount;										/*!< 支持的辅助开关数														*/
	HxhtPTZAuxInfo sAuxInfo[MAX_AUX_SWITCH_NUM];		/*!< 辅助开关信息															*/
}HxhtPTZAuxSwitch, *LPHxhtPTZAuxSwitch;

/*!
*  @struct	tagHxhtPresetPoint
*  @brief	Hxht预置点信息
*/
typedef struct tagHxhtPresetPoint
{
	DWORD dwNum;										/*!< 预置点编号																*/
	char acName[HXHT_COMMON_STRING_LEN];				/*!< 预置点名称																*/
}HxhtPresetPoint, *LPHxhtPresetPoint;

/*!
*  @struct	tagHxhtPresetPointSetting
*  @brief	Hxht预置点设置信息
*/
typedef struct tagHxhtPresetPointSetting
{
	DWORD dwCount;										/*!< 支持预置点数															*/
	HxhtPresetPoint sPoint[MAX_PRESET_POINT_NUM];		/*!< 预置点信息																*/
}HxhtPresetPointSetting, *LPHxhtPresetPointSetting;

/*!
*  @struct	tagHxhtPTZSupport
*  @brief	Hxht PTZ支持功能
*  @note	dwOpCmd说明：8300--光圈大/小 8306--光圈停止 8302--数组变倍大/小 8308--数字变倍停止
			8200--云台开始动作 8202--云台停止动作 8204--随机线扫开/关 8206--预置点设置
			8208--预置点清除 8210--预置点调用 8212--预置点清除所有 8304--聚焦控制大/小
			8310--聚焦停止 8400--辅助开关 0--停止 1--云台上 2--云台下 3--云台左 4--云台右
			5--光圈大 6--光圈小 7--变倍大 8--变倍小 9--聚焦大 10--聚焦小 11--随机线扫开 12--随机线扫关
			13--灯光开 14--灯光关 15--雨刷开 16--雨刷关 17--探头电源开 18--探头电源关
			19--摄头电源开 20--摄头电源关
*/
typedef struct tagHxhtPTZSupport
{
	char acDirect[HXHT_COMMON_STRING_LEN];				/*!< 是否支持方向控制 y--支持 n--不支持										*/
	char acBright[HXHT_COMMON_STRING_LEN];				/*!< 是否支持光圈(亮度)控制	y--支持 n--不支持								*/
	char acZoom[HXHT_COMMON_STRING_LEN];				/*!< 是否支持变倍控制 y--支持 n--不支持										*/
	char acFocus[HXHT_COMMON_STRING_LEN];				/*!< 是否支持聚焦控制 y--支持 n--不支持										*/
	char acFlip180[HXHT_COMMON_STRING_LEN];				/*!< 是否支持180度翻转 y--支持 n--不支持									*/
	char acOriginalSetting[HXHT_COMMON_STRING_LEN];		/*!< 是否支持恢复原厂设置 y--支持 n--不支持									*/
	char acEnterMenu[HXHT_COMMON_STRING_LEN];			/*!< 是否支持进入菜单模式 y--支持 n--不支持									*/
	DWORD dwOpCmd[MAX_SUPPORT_OP_CMD_NUM];				/*!< 支持的云台控制命令号 													*/
}HxhtPTZSupport, *LPHxhtPTZSupport;

/*!
*  @struct	tagHxhtPTZStatusInfo
*  @brief	Hxht PTZ状态信息
*/
typedef struct tagPTZStatusInfo
{
	char acStatus[HXHT_COMMON_STRING_LEN];				/*!< 云台状态,长度为5的'0'、'1' 字符串，该字符串的5个字符依次代表随机线扫、灯光、雨刷、探头、摄头的状态，'0'为关闭，'1'为打开*/
	HxhtPTZSwitchStatus sSwitchStatus;					/*!< 开关状态																*/
	HxhtPTZAuxSwitch sAuxSwitch;						/*!< 辅助开关信息															*/
	HxhtPresetPointSetting sPresetSetting;				/*!< 预置点设置																*/
	HxhtPTZSupport sSupport;							/*!< 支持的TPZ功能															*/
}HxhtPTZStatusInfo, *LPHxhtPTZStatusInfo;

/*!
*  @struct	tagPanelStatusInfo
*  @brief	面板状态信息
*/
typedef struct tagPanelStatusInfo
{
	DWORD dwStatus;										/*!< 面板状态,0为未锁定,1为已锁定											*/
	DWORD dwReserve[DEF_RESERVE_NUM];					/*!< 保留																	*/
}PanelStatusInfo, *LPPanelStatusInfo;

/*!
*  @struct	tagPanelControlParameter
*  @brief	面板控制参数
*/
typedef struct tagPanelControlParameter
{
	DWORD dwControl;									/*!< 面板控制,参见ePanelControlType值										*/
	DWORD dwKeyBoardCode;								/*!< 面板键值,参见ePanelKeyBoardCodeType值									*/
	DWORD dwReserve[DEF_RESERVE_NUM];					/*!< 保留																	*/
}PanelControlParameter, *LPPanelControlParameter;

/*!
*  @enum	eRTPStreamTransferMode
*  @brief	RTP流传输方式
*/
typedef enum eRTPStreamTransferMode
{
	eRTPStreamTransferModeBegin = 0,				/*!< 起始值									*/
	eRTPGeneralTCP,										/*!< 普通的TCP								*/
	eRTPGeneralUDP,										/*!< 普通的UDP								*/
	eRTPStreamTransferModeEnd,							/*!< 结束值									*/
}eRTPStreamTransferMode;

/*!
*  @struct	eScreenControlStatus
*  @brief	屏幕控制状态
*/
typedef enum eScreenControlStatus
{
	eScreenControlStatusBegin = 0,	/*!< 起始值									*/
	eScreenControlStatusLoopStart,						/*!< 开始轮转								*/
	eScreenControlStatusLoopStop,						/*!< 停止轮转								*/
	eScreenControlStatusSplitSingle,					/*!< 单画面									*/
	eScreenControlStatusSplitThree,						/*!< 三画面									*/
	eScreenControlStatusSplitFour,						/*!< 四画面									*/
	eScreenControlStatusSplitSix,						/*!< 六画面									*/
	eScreenControlStatusSplitNine,						/*!< 九画面									*/
	eScreenControlStatusSplitTen,						/*!< 十画面									*/
	eScreenControlStatusSplitTwelve,					/*!< 十二画面								*/
	eScreenControlStatusSplitThirteen,					/*!< 十三画面								*/
	eScreenControlStatusSplitFifteen,					/*!< 十五画面								*/
	eScreenControlStatusSplitSixteen,					/*!< 十六画面								*/
	eScreenControlStatusEnd,							/*!< 结束值									*/
}eScreenControlStatus;

/*!
*  @enum	eResolutionMode
*  @brief	分辨率模式
*/
typedef enum eResolutionMode
{
	eResolutionModeBegin = 0,			/*!< 起始值									*/
	eResolutionModeAutoDiscern,							/*!< 自动识别分辨率								*/
	eResolutionModeQCIF,									/*!< QCIF (176*144)							*/
	eResolutionModeCIF,										/*!< CIF (352*288 / 320*240 / 352*240)			*/
	eResolutionModeHalfD1,									/*!< HalfD1 (704*288 / 640*240 / 704*240)		*/
	eResolutionModeD1,										/*!< D1	(704*576 / 640*480 / 704*480)			*/
	eResolutionModeDCIF,									/*!< DCIF (528*384 / 480*240)					*/
	eResolutionModeMD,										/*!< MD	(512*288 / 416*240)					*/
	eResolutionModeVGA,										/*!< VGA (640*480)							*/
	eResolutionModeHD720,									/*!< HD720 (1280*720)						*/
	eResolutionModeQuadVGA,									/*!< QuadVGA (1280*960)						*/
	eResolutionModeUXGA,									/*!< UXGA (1600*1200)						*/
	eResolutionModeHD1080,									/*!< HD1080 (1920*1080)						*/
	eResolutionModeEnd,										/*!< 结束值									*/
}eResolutionMode;

/*!
*  @enum	eVGAResolutionMode
*  @brief	VGA分辨率
*/
typedef enum eVGAResolutionMode
{
	VGA_SVGA_800x600_60HZ = 0,
	VGA_SVGA_800x600_75HZ,
	VGA_XGA_1024x768_60HZ,
	VGA_XGA_1024x768_70HZ,
	VGA_SXGA_1280x1024_60HZ,
	VGA_1280x720P_60HZ,
	VGA_1920x1080i_60HZ,
	VGA_1920x1080p_60HZ
}eVGAResolutionMode;

/*!
*  @struct	eLogoPictureType
*  @brief	Logo图片类型
*/
typedef enum eLogoPictureType
{
	eLogoPictureTypeBegin = 0,		/*!< 起始值									*/
	eLogoPictureTypeDecoderChannel,						/*!< 解码通道LOGO							*/
	eLogoPictureTypeEnd,								/*!< 结束值									*/
}eLogoPictureType;

/*!
*  @enum	eDeviceChannelControl
*  @brief	设备通道控制
*/
typedef enum eDeviceChannelControl
{
	eDeviceChannelControlBegin = 0,				/*!< 起始值									*/
	eDeviceChannelControlShowLogo,						/*!< 显示LOGO								*/
	eDeviceChannelControlHideLogo,						/*!< 隐藏LOGO								*/
	eDeviceChannelControlEnd,							/*!< 结束值									*/
}eDeviceChannelControl;

/*!
*  @enum	eDeviceProtocolType
*  @brief	设备协议类型
*/
typedef enum eDeviceProtocolType
{
	eDeviceProtocolTypeBegin = 0,				/*!< 起始值									*/
	eDeviceProtocolTypeAdapter,							/*!< 自适应版本								*/
	eDeviceProtocolTypeClassics,						/*!< 经典版本								*/
	eDeviceProtocolTypeNewborn,							/*!< 新版本									*/
	eDeviceProtocolTypeEnd,								/*!< 结束值									*/
}eDeviceProtocolType;


/*!
*  @struct	tagRealStreamPara
*  @brief	实时流连接参数
*/
typedef struct tagRealStreamPara
{
	DWORD	dwChannel;									/*!< 流媒体通道号							*/
	eStreamTransferMode	eTransferMode;					/*!< 传输模式								*/
	eStreamMediaType	eMediaType;						/*!< 流媒体类型								*/
	DWORD	dwReserve[DEF_RESERVE_NUM];					/*!< 保留									*/
}RealStreamPara, *LPRealStreamPara;

/*!
*  @struct	tagUserLoginPara
*  @brief	用户登录参数
*/
typedef struct tagUserLoginPara
{
	char	sServerIP[MAX_ADDRESS_LEN];					/*!< 服务器端IP地址							*/
	DWORD	dwCommandPort;								/*!< 登录连接的信令端口						*/
	char	sUName[USERNAME_LEN];						/*!< 登录用户的用户名称						*/
	char	sUPass[USERPASS_LEN];						/*!< 登录用户的用户密码						*/
	DWORD	dwReserve[DEF_RESERVE_NUM];					/*!< 保留									*/
}UserLoginPara, *LPUserLoginPara;

/*!
*  @struct	tagHistoryStreamPara
*  @brief	历史流连接参数
*/
typedef struct tagHistoryStreamPara
{
	DWORD	dwDiskGroup;								/*!< 盘组									*/
	DWORD	dwChannel;									/*!< 流媒体通道号							*/
	DWORD		dwEnableEndTime;						/*!< 结束时间是否有效						*/
	TimeInfo	cBeginTime;								/*!< 历史流起始时间							*/
	TimeInfo	cEndTime;								/*!< 历史流结束时间							*/
	eHistoryStreamType	eStreamType;					/*!< 历史流类型								*/
	eStreamTransferMode	eTransferMode;					/*!< 传输模式								*/
	char	cStreamMediaType;							/*!< 媒体流类型 0表示主码流，1表示子码流	*/
	char	cReserve[3];									/*!< 保留									*/
	DWORD	dwReserve[DEF_RESERVE_NUM - 1];				/*!< 保留									*/
}HistoryStreamPara, *LPHistoryStreamPara;

/*!
*  @struct	tagChannelOSDInformation
*  @brief	通道OSD信息
*/
typedef struct tagChannelOSDInformation
{
	DWORD	dwEnable;									/*!< 启用: 0为不启用, 1为启用				*/
	char	cOSDText[MAX_OSD_TEXT_STRING_LENGTH];		/*!< OSD信息								*/
	DWORD	dwReserve[DEF_RESERVE_NUM];					/*!< 保留									*/
}ChannelOSDInformation, *LPChannelOSDInformation;

/*!
*  @struct	tagStreamTransferPara
*  @brief	流转发连接参数
*/
typedef struct tagStreamTransferPara
{
	eStreamTransferMode	eTransferMode;					/*!< 传输模式								*/
	eStreamSourceType eStreamSourceTypeValue;			/*!< 流媒体源类型							*/
	eVideoCodecMode	eVideoCodecType;					/*!< 视频编解码模式							*/
	eVoiceMode	eVoiceCoderType;						/*!< 音频编解码模式							*/
	eResolutionMode	eResolutionValue;					/*!< 图像分辨率模式							*/
	ChannelOSDInformation	cOSDInfo;					/*!< OSD信息结构							*/
	DWORD	dwReserve[DEF_RESERVE_NUM];					/*!< 保留									*/
}StreamTransferPara, *LPStreamTransferPara;

/*!
*  @struct	tagMediaServerInformation
*  @brief	流媒体服务器信息
*/
typedef struct tagMediaServerInformation
{
	DWORD	dwEnable;									/*!< 启用: 0为不启用, 1为启用				*/
	char	sServerIP[MAX_ADDRESS_LEN];					/*!< 服务器端IP地址							*/
	DWORD	dwServerPort;								/*!< 登录连接的服务器端口					*/
	char	sUName[USERNAME_LEN];						/*!< 登录用户的用户名称						*/
	char	sUPass[USERPASS_LEN];						/*!< 登录用户的用户密码						*/
	eRTPStreamTransferMode	eRTPStreamTransferValue;	/*!< RTP流传输模式							*/
	char	sChannel[MAX_RTSP_SOURCE_STRING_LENGTH];	/*!< 视频通道								*/
	DWORD	dwReserve[DEF_RESERVE_NUM];					/*!< 保留									*/
}MediaServerInformation, *LPMediaServerInformation;

/*!
*  @struct	tagHYPrivateServerInformation
*  @brief	HY私有协议服务器信息
*/
typedef struct tagHYPrivateServerInformation
{
	DWORD	dwEnable;									/*!< 启用: 0为不启用, 1为启用				*/
	eDeviceProtocolType	eDeviceProtocolValue;			/*!< 设备协议版本类型						*/
	eStreamSourceType	eStreamSourceValue;				/*!< 流媒体源类型							*/
	UserLoginPara	cUserLoginPara;						/*!< 登录参数信息							*/
	RealStreamPara	cRealStreamPara;					/*!< 实时流参数信息							*/
	HistoryStreamPara cHistoryStreamPara;				/*!< 历史流参数信息							*/
	DWORD	dwReserve[DEF_RESERVE_NUM];					/*!< 保留									*/
}HYPrivateServerInformation, *LPHYPrivateServerInformation;

/*!
*  @struct	tagLocalHistoryInformation
*  @brief	本地历史流信息
*/
typedef struct tagLocalHistoryInformation
{
	DWORD		dwEnable;								/*!< 启用: 0为不启用, 1为启用				*/
	DWORD		dwEnableEndTime;						/*!< 结束时间是否有效						*/
	TimeInfo	cBeginTime;								/*!< 历史流起始时间							*/
	TimeInfo	cEndTime;								/*!< 历史流结束时间							*/
	eHistoryStreamType	eStreamType;					/*!< 历史流类型								*/
	DWORD	dwReserve[DEF_RESERVE_NUM];					/*!< 保留									*/
}LocalHistoryInformation, *LPLocalHistoryInformation;

/*!
*  @struct	tagDecoderDynamicInformation
*  @brief	动态解码服务器信息
*/
typedef struct tagDecoderDynamicInformation
{
	MediaServerInformation	cMediaServerInfo;			/*!< 流媒体服务器信息						*/
	HYPrivateServerInformation	cHYPrivateServerInfo;	/*!< HY私有协议服务器信息					*/
	LocalHistoryInformation	cLocalHistoryInformation;	/*!< 本地历史流信息							*/
	DWORD	dwReserve[DEF_RESERVE_NUM];					/*!< 保留									*/
}DecoderDynamicInformation, *LPDecoderDynamicInformation;

/*!
*  @struct	tagDecoderChannelSourceConfig
*  @brief	解码器通道视频源配置
*/
typedef struct tagDecoderChannelSourceConfig
{
	MediaServerInformation	cMediaServerInfo;			/*!< 流媒体服务器信息							*/
	HYPrivateServerInformation	cHYPrivateServerInfo;	/*!< HY私有协议服务器信息						*/
	DWORD	dwReserve[DEF_RESERVE_NUM];				/*!< 保留									*/
}DecoderChannelSourceConfig, *LPDecoderChannelSourceConfig;

/*!
*  @struct	tagDecoderChannelLoopConfig
*  @brief	解码器通道轮转配置
*/
typedef struct tagDecoderChannelLoopConfig
{
	DWORD	dwEnable;								/*!< 是否启用  								*/
	DWORD	dwPoolTime;							/*!< 轮巡时间  								*/
	DecoderChannelSourceConfig cSource[MAX_DECODER_CHANNEL_SOURCE];/*!< 通道视频源配置						*/
	DWORD	dwReserve[DEF_RESERVE_NUM];				/*!< 保留									*/
}DecoderChannelLoopConfig, *LPDecoderChannelLoopConfig;

/*!
*  @struct	tagDecoderLoopStatus
*  @brief	解码器通道轮询状态
*/
typedef struct tagDecoderLoopStatus
{
	DWORD	dwChannelNumber;							/*!< 通道数									*/
	eDecoderChannelStatus	eDecChStatus[MAX_DEC_CHANNEL_NUMBER];/*!< 通道状态						*/
	DWORD dwReserve[DEF_RESERVE_NUM];					/*!< 保留									*/
}DecoderLoopStatus, *LPDecoderLoopStatus;

/*!
*  @struct	tagDecoderDeviceConfig
*  @brief	解码器设备配置
*/
typedef struct tagDecoderDeviceConfig
{
	DWORD	dwDecoderChannelNumber;					/*!< 解码通道个数,根据解码模式决定个数,只读配置项	*/
	eResolutionMode	eDecodeMode;					/*!< 解码模式								*/
	DWORD	dwReserve[DEF_RESERVE_NUM];				/*!< 保留									*/
}DecoderDeviceConfig, *LPDecoderDeviceConfig;

/*!
*  @struct	tagDecoderChannelInformation
*  @brief	解码器通道信息
*/
typedef struct tagDecoderChannelInformation
{
	DWORD	dwDecoderState;						/*!< 解码通道状态：0-动态解码 1－循环解码			*/
	DWORD	dwGetStreamMode;						/*!< 取流模式：0-未解码，1-主动，2-被动			*/
	MediaServerInformation	cMediaServerInfo;			/*!< 流媒体服务器信息							*/
	HYPrivateServerInformation	cHYPrivateServerInfo;	/*!< HY私有协议服务器信息						*/
	LocalHistoryInformation	cLocalHistoryInformation;		/*!< 本地历史流信息							*/
	StreamTransferPara 	cStreamTransferPara;				/*!< 被动解码流转发信息						*/
	DWORD	dwReserve[DEF_RESERVE_NUM];				/*!< 保留									*/
}DecoderChannelInformation, *LPDecoderChannelInformation;

/*!
*  @struct	tagDecoderChannelStatusInfo
*  @brief	解码器通道状态信息
*/
typedef struct tagDecoderChannelStatusInfo
{
	eDecoderChannelStatus	eDecChStatus;				/*!< 通道状态					*/
	eStreamSourceType	eStreamSourceTypeValue;			/*!< 流媒体源类型				*/
	DWORD	dwStreamBitrate;						/*!< 流媒体码率(kbps)			*/
	DWORD	dwVideoWidth;							/*!< 视频图像宽度				*/
	DWORD	dwVideoHeight;						/*!< 视频图像高度				*/
	DWORD	dwReserve[DEF_RESERVE_NUM];					/*!< 保留									*/
}DecoderChannelStatusInfo, *LPDecoderChannelStatusInfo;

/*!
*  @struct	tagDecoderChannelStatus
*  @brief	解码器通道状态
*/
typedef struct tagDecoderChannelStatus
{
	DWORD	dwChannelNumber;							/*!< 通道数				*/
	DecoderChannelStatusInfo	cDecChStatus[MAX_DEC_CHANNEL_NUMBER];/*!< 解码器通道状态信*/
	DWORD dwReserve[DEF_RESERVE_NUM];					/*!< 保留					*/
}DecoderChannelStatus, *LPDecoderChannelStatus;

/*!
*  @struct	tagDecoderChannelControl
*  @brief	解码器通道控制
*/
typedef struct tagDecoderChannelControl
{
	eDecoderChannelStatus	eDecChStatus[MAX_DEC_CHANNEL_NUMBER];/*!< 解码器通道状态*/
	DWORD	dwReserve[DEF_RESERVE_NUM];					/*!< 保留									*/
}DecoderChannelControl, *LPDecoderChannelControl;

/*!
*  @struct	tagDecoderSetPlayStatusParameter
*  @brief	设置解码器通道播放状态参数
*/
typedef struct tagDecoderSetPlayStatusParameter
{
	eStreamPlayControlStatus	ePlayControlStatus;		/*!< 流播放状态控制		*/
	DWORD dwParameter[MAX_PLAY_CONTROL_PARAMETER];			/*!< 流播放状态控制参数		*/
	DWORD dwReserve[DEF_RESERVE_NUM];					/*!< 保留					*/
}DecoderSetPlayStatusParameter, *LPDecoderSetPlayStatusParameter;

/*!
*  @struct	tagDecoderPlayStatusParameter
*  @brief	解码器通道播放状态参数
*/
typedef struct tagDecoderPlayStatusParameter
{
	eStreamSourceType	eStreamSourceTypeValue;				/*!< 流媒体源类型			*/
	DWORD		dwEnableEndTime;						/*!< 结束时间是否有效		*/
	TimeInfo	cBeginTime;								/*!< 历史流起始时间		*/
	TimeInfo	cEndTime;									/*!< 历史流结束时间		*/
	TimeInfo	cCurrentTime;								/*!< 当前播放的历史流时间	*/
	DWORD dwReserve[DEF_RESERVE_NUM];					/*!< 保留					*/
}DecoderPlayStatusParameter, *LPDecoderPlayStatusParameter;

/*!
*  @struct	tagDecoderGetPlayStatusParameter
*  @brief	获取解码器通道播放状态参数
*/
typedef struct tagDecoderGetPlayStatusParameter
{
	DWORD	dwChannelNumber;							/*!< 通道数				*/
	DecoderPlayStatusParameter	cDecChPlayStatus[MAX_DEC_CHANNEL_NUMBER];/*!< 解码器通道播放状态信息*/
	DWORD dwReserve[DEF_RESERVE_NUM];					/*!< 保留					*/
}DecoderGetPlayStatusParameter, *LPDecoderGetPlayStatusParameter;

/*!
*  @struct	tagDecoderSetDisplayStatusParameter
*  @brief	设置解码通道显示状态参数
*/
typedef struct tagDecoderSetDisplayStatusParameter
{
	eDisplayControlStatus	eDisplayControlStatusValue;		/*!< 显示控制状态信息		*/
	DWORD dwReserve[DEF_RESERVE_NUM];					/*!< 保留					*/
}DecoderSetDisplayStatusParameter, *LPDecoderSetDisplayStatusParameter;

/*!
*  @struct	tagDecoderDisplayConfig
*  @brief	解码显示配置
*/
typedef struct tagDecoderDisplayConfig
{
	eVGAResolutionMode	eVGAResolution;				/*!<	分辨率								*/
	eScreenControlStatus	eScreenControlStatusValue;		/*!< 显示控制状态信息							*/
	DWORD	dwDecoderChannel[MAX_DISPLAY_SCREEN_NUMBER];	/*!< 绑定的解码通道号							*/
	DWORD	dwEnlargeWindow;						/*!< 窗口放大, 0xFFFFFFFF表示不放大,其它表示窗口编号	*/
	DWORD dwReserve[DEF_RESERVE_NUM];					/*!< 保留									*/
}DecoderDisplayConfig, *LPDecoderDisplayConfig;

/*!
*  @struct	tagChannelChromaConfig
*  @brief	解码通道光学参数
*/
typedef struct tagChannelChromaConfig
{
	DWORD dwBrightness;								/*!< 亮度,0-255 							*/
	DWORD dwContrast;								/*!< 对比度,0-255							*/
	DWORD dwSaturation;								/*!< 饱和度,0-255 							*/
	DWORD dwHue;									/*!< 色调,0-255 							*/
	DWORD dwReserve[DEF_RESERVE_NUM];					/*!< 保留									*/
}ChannelChromaConfig, *LPChannelChromaConfig;

/*!
*  @struct	tagDecoderChannelConfig
*  @brief	解码器通道配置
*/
typedef struct tagDecoderChannelConfig
{
	DWORD	dwDecChanScaleStatus;					/*!< 解码通道显示缩放控制，1-缩放显示，0-真实显示 	*/
	ChannelChromaConfig	cChannelChromaConfig;			/*!< 光学参数								*/
	DWORD	dwReserve[DEF_RESERVE_NUM];				/*!< 保留									*/
}DecoderChannelConfig, *LPDecoderChannelConfig;

/*!
*  @struct	tagDecoderVideoStandardConfig
*  @brief	解码器通道视频制式
*/
typedef struct tagDecoderVideoStandardConfig
{
	DWORD	dwVideoStandard;						/*!< 解码器视频制式: 0-PAL，1-NTSC  				*/
	DWORD	dwReserve[DEF_RESERVE_NUM];				/*!< 保留									*/
}DecoderVideoStandardConfig, *LPDecoderVideoStandardConfig;

/*!
*  @struct	tagUploadLogoParameter
*  @brief	上传Logo参数
*/
typedef struct tagUploadLogoParameter
{
	eLogoPictureType	eLogoPictureValue;				/*!< Logo图片类型							*/
	DWORD dwParameter[MAX_LOGO_PICTURE_PARAMETER];		/*!< Logo图片参数							*/
	Buffer				cBuffer;						/*!< Logo图片缓冲区							*/
	DWORD dwReserve[DEF_RESERVE_NUM];					/*!< 保留									*/
}UploadLogoParameter, *LPUploadLogoParameter;

/*!
*  @struct	tagDecoderDisplayStatus
*  @brief	设置解码通道显示状态
*/
typedef struct tagDecoderDisplayStatus
{
	DWORD	dwWindow;								/*!< 窗口编号								*/
	eDisplayControlStatus	eDisplayControlStatusValue;	/*!< 显示控制状态信息							*/
	DWORD dwReserve[DEF_RESERVE_NUM];					/*!< 保留									*/
}DecoderDisplayStatus, *LPDecoderDisplayStatus;

/*!
*  @struct	tagHistoryStreamMultiTypeQueryFactor
*  @brief	历史流查询条件
*/
typedef struct tagHistoryStreamMultiTypeQueryFactor
{
	DWORD				dwChannel;						/*!< 通道号									*/
	DWORD				dwDiskGroup;					/*!< 盘组									*/
	DWORD				dwStreamType;					/*!< 历史流类型								*/
	char				cBeginTime[15];					/*!< 起始时间								*/
	char				cEndTime[15];					/*!< 结束时间								*/
}HistoryStreamMultiTypeQueryFactor, *LPHistoryStreamMultiTypeQueryFactor;

/*!
*  @struct	tagTransmitHistoryStreamQueryFactor
*  @brief	转发历史流查询条件
*/
typedef struct tagTransmitHistoryStreamQueryFactor
{
	UserLoginPara	cUserLoginPara;					/*!< 登录参数信息							*/
	eDeviceProtocolType	eDeviceProtocolValue;			/*!< 设备协议版本类型							*/
	HistoryStreamMultiTypeQueryFactor cFactor;			/*!< 历史查询条件							*/
	DWORD dwReserve[DEF_RESERVE_NUM];					/*!< 保留									*/
}TransmitHistoryStreamQueryFactor, *LPTransmitHistoryStreamQueryFactor;

/*!
*  @struct	tagHistoryStreamQueryResultEx
*  @brief	历史流查询结果
*/
typedef struct tagHistoryStreamMultiTypeQueryResult
{
	DWORD				dwChannel;						/*!< 通道号									*/
	DWORD				dwStreamType;					/*!< 历史流类型								*/
	char				cBeginTime[DATE_TIME_LEN];		/*!< 起始时间								*/
	char				cEndTime[DATE_TIME_LEN];		/*!< 结束时间								*/
	DWORD				dwStreamSize;					/*!< 流数据长度								*/
}HistoryStreamMultiTypeQueryResult, *LPHistoryStreamMultiTypeQueryResult;

/*!
*  @struct	tagEventGateKeyboard
*  @brief	门禁密码
*/
typedef struct tagEventGateKeyboard
{
	char	m_id[USERPASS_LEN];						/*!< 添加密码返回的ID								*/
	int nEventID;
	int nAuthResult;									/*!< 0，表示验证正确，1.表示验证失败								*/
	char szPassword[32];
	int nPasswordLen;
	char	m_Reserve[MAX_CFG_LEN_512BYTE - 32 - 4 - 4 - 32 - 4];		/*!< 保留字段							*/
}EventGateKeyboard, *LPEventGateKeyboard;

/*!
*  @struct	tagEventGateCard
*  @brief	门禁刷卡
*/
typedef struct tagEventGateCard
{
	char	m_id[MAX_CARD_ID_LENGTH];				/*!< 添加卡片返回的ID								*/
	char szCardNumber[32];
	int nCardNumberLen;
	char	m_Reserve[MAX_CFG_LEN_512BYTE - 32 - 32 - 4];		/*!< 保留字段							*/
}EventGateCard, *LPEventGateCard;

typedef struct tagEventPeopleInfo
{
	char strName[32];				//作为输入参数,姓名
	int nSex;						//作为输入参数,性别,0,male,1,female
	int nCertificateType;			//作为输入参数,身份证类型 0，passport,1,身份证，0xff，其他
	char strCertificateID[32];		//作为输入参数,ID
	char strCountry[32];			//作为输入参数,国家
	char strAddress[64];			//作为输入参数,地址
	TimeInfo struBirthday;		//作为输入参数,生日
	char strUniqueID[32];			//作为输入参数
	long long nPersonID;			//作为输入参数
	char strCardNum[32];			//作为输入参数,卡号
	char szReserved[MAX_CFG_LEN_512BYTE - 264];
}EventPeopleInfo, *LPEventPeopleInfo;

typedef struct tagEventGateCardV2
{
	char szID[MAX_CARD_ID_LENGTH];
	long long nGroupID;										/*!< 脸库ID								*/
	AbsoluteTime struTime;									/*!< 刷卡时间								*/
	EventPeopleInfo struPeopleInfo;					/*!< 刷卡人员信息								*/
	int nEventID;
	int nAuthority;									/*!< 0,白名单，1，黑名单，2，未注册				*/
	char szReserved[MAX_CFG_LEN_512BYTE - 4 - 4];
}EventGateCardV2, *LPEventGateCardV2;

/*!
*  @struct	tagEventGateRemote
*  @brief	门禁远程操作
*/
typedef struct tagEventGateRemote
{
	char	m_ControlID[MAX_GATE_REMOTE_ID_LENGTH];		/*!< 远程控制码							*/
	int 	m_nEventId;									/*!< 事件ID							*/
	char	m_Reserve[MAX_CFG_LEN_512BYTE - 32 - 4];	/*!< 保留字段							*/
}EventGateRemote, *LPEventGateRemote;

/*!
*  @struct	tagObjectRect
*  @brief	目标位置
*/
typedef struct tagObjectRect
{
	WORD				wLeft;							/*!< 左									*/
	WORD				wRight;							/*!< 右									*/
	WORD				wTop;							/*!< 上									*/
	WORD				wBottom;						/*!< 下									*/
}ObjectRect, *LPObjectRect;

/*!
*  @struct	tagImageInfo
*  @brief	图片信息
*/
typedef struct tagImageInfo
{
	WORD				wWidth;							/*!< 图片宽度								*/
	WORD				wHeight;						/*!< 图片高度								*/
	DWORD				dwLength;						/*!< 文件大小								*/
	char				cReserve[8];					/*!< 保留数据								*/
}ImageInfo, *LPImageInfo;

/*!
*  @struct	tagQRCodeEventInfo
*  @brief	二维码事件信息
*/
typedef struct tagQRCodeEventInfo
{
	float				fBodyTemperature;						/*!< 体温数据 */
	DWORD				dwTemperatureStatus;					/*!< 体温状态（0-正常、1-异常） */
    DWORD				dwQRCodeBufferLength;					/*!< 二维码数据长度 */
    
	char				cReserve[1024];							/*!< 保留字段 */
}QRCodeEventInfo, *LPQRCodeEventInfo;

/*!
*  @struct	tagFaceFeatureInfo
*  @brief	人脸识别特征信息
*/
typedef struct tagFaceFeatureInfo
{
    unsigned char        cAge;                            /*!< 年龄：0-未知，其它表示年龄                */
    unsigned char        cSex;                            /*!< 性别：0-未知，1-男性，2-女性            */
    unsigned char        cHat;                            /*!< 帽子：0-未知，1-戴帽子，2-无帽子,3.安全帽,4.其他帽子       */
    unsigned char        cHair;                            /*!< 头发长度：0-未知，1-短发，2-普通，3-长发,4.光头，5.少量头发（包含秃顶）*/
    unsigned char        cHairColor;                        /*!< 头发颜色：0-未知，1-黑色，2-白色，3-金色，4-其它*/
    unsigned char        cEye;                            /*!< 眼睛：0-未知，1-正常，2-闭眼，3-遮挡    */
    unsigned char        cGlasses;                        /*!< 眼镜：0-未知，1-戴眼镜，2-无眼镜,3.墨镜，4.其他眼镜        */
    unsigned char        cBeard;                            /*!< 胡须：0-未知，1-有胡须（嘴唇上的胡子），2-无胡须(无胡子或者胡子不明显) 3. 络腮胡       */
    unsigned char        cMask;                            /*!< 口罩：0-未知，1-有口罩，2-无口罩，3.捂住嘴巴，4.捂住口鼻        */
    unsigned char        cSkin;                            /*!< 肤色：0-未知，1-黄色，2-白色，3-黑色，4-棕色*/
    unsigned char        cSmile;                            /*!< 表情：0-未知，1-普通，2-微笑            */
    unsigned char        cHuman;                            /*!< 活体：0-未知，1-活体，2-假体            */
    unsigned char        cQuality;                        /*!< 清晰度：值域范围（0-100）                */
    float                cYaw;                            /*!< 侧向角度                                */
    float                cRoll;                            /*!< 俯仰角度                                */
    float                cPitch;                            /*!< 斜脸角度                                */
    char                cReserve[64];                    /*!< 保留数据                                */
}FaceFeatureInfo, *LPFaceFeatureInfo;


/*!
*  @struct	tagFaceCaptureInfo
*  @brief	脸库抓拍信息
*/
#ifdef _WIN32
#pragma pack(1)
#endif
typedef struct tagFaceCaptureInfo
{
	FaceFeatureInfo		cFaceFeature;					/*!< 人脸识别特征信息：描述检测到的目标特征信息	*/
	ImageInfo			cFaceImage;						/*!< 抓拍图片信息：实时抓拍图片信息			*/
	ObjectRect			cFaceRect;						/*!< 人脸位置：描述人脸图像区域位置信息		*/
	unsigned long long nTrackID;					/*!< 同一个人脸一直在画面中连续抓的图trackid是同一个，表示是同一个人脸，即trackid没断。如果人离开画面后再出现则重新分配id，即trackid断了。								*/
	int				nTrackDie;							/*!< trackid即将不再连续，下一次事件的trackid将会与本次的trackid不同，本次推送为同一个trackid的最后一张图								*/
	char				cReserve[100 - 8 - 4];					/*!< 保留数据	-8:nTrackID		-4:nTrackDie								*/
}
#ifdef _WIN32
FaceCaptureInfo, *LPFaceCaptureInfo;
#pragma pack()
#endif
#ifdef __linux__
__attribute__((packed)) FaceCaptureInfo, *LPFaceCaptureInfo;
#endif

/*!
*  @struct	tagFaceMatchInfo
*  @brief	脸库匹配信息
*/
typedef struct tagFaceMatchInfo
{
	float				fMatch;							/*!< 匹配度：值域范围（0~100.0）			*/
	DWORD				dwSerial;						/*!< 匹配序号：一次人脸识别过程中的连续编号	*/
	char				cLibID[MAX_FACE_LIB_ID_LENGTH];	/*!< 脸库ID：黑名单，白名单，其它等			*/
	char				cFaceID[MAX_FACE_ID_LENGTH];	/*!< 人脸ID									*/
	unsigned char		cFaceLibName[MAX_FACE_LIB_NAME_LENGTH];/*!< 脸库名称，Unicode编码			*/
	unsigned char		cFaceName[MAX_FACE_NAME_LENGTH];/*!< 人脸名称，Unicode编码					*/
	ImageInfo			cImageInfo;						/*!< 人脸图片信息							*/
	char				szUniqueID[32];					/*!< 人脸UID							*/
	//char				cReserve[32];					/*!< 保留数据								*/
}FaceMatchInfo, *LPFaceMatchInfo;

/*!
*  @struct	tagHealthCodeData
*  @brief	健康码信息
*/
typedef struct tagHealthCodeData
{
	DWORD				dwHealthCodeStatus;						/*!< 健康码状态	*/
	char				acIDCardDesen[32];						/*!< 身份证（脱敏）	*/
	char				acNameDesen[32];						/*!< 姓名（脱敏）	*/
	char				acNucleicData[128];						/*!< 核酸数据 */
    char				acTravelData[192];						/*!< 行程数据	*/
	char				cReserve[1024];							/*!< 保留数据	*/
}HealthCodeData, *LPHealthCodeData;

/*!
*  @struct	tagEventFaceRecognition
*  @brief	人脸识别事件
*/
/*事件负载缓冲区内容视图
格式分布：[EventFaceRecognition][Image0]
EventFaceRecognition：表示人脸识别事件主结构
Image0：表示底图或者实时人脸抓拍图片，图片信息见cFaceImage
一次人脸识别匹配事件流程示例：
1. 发送背景图事件消息，消息类型为背景图，此时cFace人脸详情等描述信息无效，但cCaptureInfo.cImageInfo有效
2. 发送人脸识别事件消息，消息类型为人脸抓拍，附带抓拍人脸源图片
3. 发送人脸识别事件消息，消息类型为人脸比对，附带脸库匹配目标图片
4. 发送剩余人脸比对事件消息至结束
*/
#ifdef _WIN32
#pragma pack(1)
#endif
typedef struct tagEventFaceRecognition
{
	DWORD				dwID;							/*!< 事件ID：同一事件在多片段发送时使用相同ID标识*/
	DWORD				dwType;							/*!< 类型：0-人脸抓拍：实时抓拍到的目标人脸
																1-人脸比对：脸库中的匹配目标人脸
																2-背景图：智能视频分析背景图		*/
	DWORD				dwTimeSec;						/*!< 绝对时间戳（单位：秒）					*/
	DWORD				dwTimeUSec;						/*!< 绝对时间戳（单位：微秒）				*/
	DWORD				dwFaceNumber;					/*!< 人脸数量：当前实时图像中检测到的人脸数量*/
	DWORD				dwImageCodec;					/*!< 图片格式：0-JPEG;1-PNG						*/
	DWORD				dwMatchNumber;					/*!< 匹配数量：0-未匹配到目标，否则为匹配目标数量*/
	union {
		FaceCaptureInfo		cCaptureInfo;				/*!< 抓拍图片信息：描述抓拍图片基本信息		*/
		FaceMatchInfo		cMatchInfo;					/*!< 匹配图片信息：描述目标图片基本信息		*/
	}cFace;												/*!< 人脸详情：dwType为0时使用cCaptureInfo，为1时使用cMatchInfo*/
	DWORD				dwRuleID;						/*!< 规则ID：触发事件的源规则，抓拍事件时填充0xFFFFFFFF*/
	float						fFaceTemperature;		/*!< 人脸温度						*/
	char						cTemperatureUnit;		/*!< 人脸温度(0：摄氏度	1：华氏度)						*/
	char						cIsNormalFaceTemperature;		/*!< 温度是否正常(0：不支持    1：温度正常	2：温度异常)						*/
	char						cHealthCodeInfoValid;	/*!< 健康码信息是否有效(0：无效，1：有效)	*/
	char				cReserve[64 - 11];				/*!< 保留数据								*/
}
#ifdef _WIN32
EventFaceRecognition, *LPEventFaceRecognition;
#pragma pack()
#endif
#ifdef __linux__
__attribute__((packed)) EventFaceRecognition, *LPEventFaceRecognition;
#endif

/*!
*  @struct	tagFaceCaptureInfoV2
*  @brief	脸库抓拍信息V2
*/
#ifdef _WIN32
#pragma pack(1)
#endif
typedef struct tagFaceCaptureInfoV2
{
	FaceFeatureInfo		cFaceFeature;
	/*!< 人脸识别特征信息：描述检测到的目标特征信息	*/
	ImageInfo			cFaceImage;
	/*!< 抓拍图片信息：实时抓拍图片信息			*/
	ObjectRect			cFaceRect;
	/*!< 人脸位置：描述人脸图像区域位置信息		*/
	unsigned long long nTrackID;
	/*!< 同一个人脸一直在画面中连续抓的图trackid是同一个，表示是同一个人脸，即trackid没断。如果人离开画面后再出现则重新分配id，即trackid断了。								*/
	int				nTrackDie;
	/*!< trackid即将不再连续，下一次事件的trackid将会与本次的trackid不同，本次推送为同一个trackid的最后一张图								*/
	char 				szPicMd5sum[64];
	/*!< 抓拍图片的md5sum							*/
	char				cReserve[64];
	/*!< 保留数据									*/
}
#ifdef _WIN32
FaceCaptureInfoV2, *LPFaceCaptureInfoV2;
#pragma pack()
#endif
#ifdef __linux__
__attribute__((packed)) FaceCaptureInfoV2, *LPFaceCaptureInfoV2;
#endif

/*!
*  @struct	tagFaceCaptureInfoV3
*  @brief	脸库抓拍信息V3
*/
#ifdef _WIN32
#pragma pack(1)
#endif
typedef struct tagFaceCaptureInfoV3
{
	FaceFeatureInfo		cFaceFeature;
	/*!< 人脸识别特征信息：描述检测到的目标特征信息	*/
	ImageInfo			cFaceImage;
	/*!< 抓拍图片信息：实时抓拍图片信息			*/
	ObjectRect			cFaceRect;
	/*!< 人脸位置：描述人脸图像区域位置信息		*/
	unsigned long long nTrackID;
	/*!< 同一个人脸一直在画面中连续抓的图trackid是同一个，表示是同一个人脸，即trackid没断。如果人离开画面后再出现则重新分配id，即trackid断了。								*/
	int				nTrackDie;
	/*!< trackid即将不再连续，下一次事件的trackid将会与本次的trackid不同，本次推送为同一个trackid的最后一张图								*/
	char 				szPicMd5sum[64];
	/*!< 抓拍图片的md5sum						*/
	int      iHasFeatureValue;
	/*!< 是否有特征值信息;0-没有,1-有 			*/
    int      iFeatureValueLength;
	/*!< 特征值有效长度 						*/
    char     acFeatureValue[2048];
	/*!< 特征值 								*/
	char				cReserve[64];
	/*!< 保留数据								*/
}
#ifdef _WIN32
FaceCaptureInfoV3, *LPFaceCaptureInfoV3;
#pragma pack()
#endif
#ifdef __linux__
__attribute__((packed)) FaceCaptureInfoV3, *LPFaceCaptureInfoV3;
#endif

/*!
*  @struct	tagFaceMatchInfoV2
*  @brief	脸库匹配信息V2
*/
typedef struct tagFaceMatchInfoV2
{
	float				fMatch;
	/*!< 匹配度：值域范围（0~100.0）*/
	DWORD				dwSerial;
	/*!< 匹配序号：一次人脸识别过程中的连续编号	*/
	char				cLibID[MAX_FACE_LIB_ID_LENGTH];
	/*!< 脸库ID：黑名单，白名单，其它等		*/
	char				cFaceID[MAX_FACE_ID_LENGTH];
	/*!< 人脸ID						   */
	unsigned char		cFaceLibName[MAX_FACE_LIB_NAME_LENGTH];
	/*!< 脸库名称，Unicode编码			*/
	unsigned char		cFaceName[MAX_FACE_NAME_LENGTH];
	/*!< 人脸名称，Unicode编码					*/
	ImageInfo			cImageInfo;
	/*!< 人脸图片信息							*/
	char				szUniqueID[32];
	/*!< 人脸UID							*/
	char 				szPicMd5sum[64];
	/*!< 匹配图片的md5sum					*/
	char				cReserve[32];
	/*!< 保留数据							*/
}FaceMatchInfoV2, *LPFaceMatchInfoV2;

/*!
*  @struct	tagFaceMatchInfoV3
*  @brief	脸库匹配信息V3
*/
#ifdef _WIN32
#pragma pack(1)
#endif
typedef struct tagFaceMatchInfoV3
{
	float				fMatch;
	/*!< 匹配度：值域范围（0~100.0）*/
	DWORD				dwSerial;
	/*!< 匹配序号：一次人脸识别过程中的连续编号	*/
	char				cLibID[MAX_FACE_LIB_ID_LENGTH_V2];
	/*!< 脸库ID：黑名单，白名单，其它等		*/
	char				cFaceID[MAX_FACE_ID_LENGTH_V2];
	/*!< 人脸ID						   */
	unsigned char		cFaceLibName[MAX_FACE_LIB_NAME_LENGTH_V2];
	/*!< 脸库名称，Unicode编码			*/
	unsigned char		cFaceName[MAX_FACE_NAME_LENGTH_V2];
	/*!< 人脸名称，Unicode编码					*/
	ImageInfo			cImageInfo;
	/*!< 人脸图片信息							*/
	char				szUniqueID[64];
	/*!< 人脸UID							*/
	char 				szPicMd5sum[64];
	/*!< 匹配图片的md5sum					*/
    char     			scOrganizationID[64];
	/*!< 组织ID 							*/
    char     			scOrganizationName[64];
	/*!< 组织名称 							*/
    char     			scFaceUUID[64];
	/*!< FaceUUID 							*/
	char				cReserve[32];
	/*!< 保留数据							*/
}
#ifdef _WIN32
FaceMatchInfoV3, *LPFaceMatchInfoV3;
#pragma pack()
#endif
#ifdef __linux__
__attribute__((packed)) FaceMatchInfoV3, *LPFaceMatchInfoV3;
#endif

/*!
*  @struct	tagEventPersonPassExpire
*  @brief	人员通行失效信息
*/
#ifdef _WIN32
#pragma pack(1)
#endif
typedef struct tagEventPersonPassExpire
{
    DWORD                dwID;                          /* 事件ID：同一事件在多片段发送时使用相同ID标识*/
    DWORD                dwTimeSec;                     /*!< 绝对时间戳（单位：秒）                    */
    DWORD                dwTimeUSec;                    /*!< 绝对时间戳（单位：微秒）                */
    long long            llGroupID;                        /*!< 作为输入参数和输出参数 */
    long long            llPersonID;                    /*!< 作为输入参数和输出参数 */
    char                 acUniqueID[32];                /*!< 作为输出参数 */

    char                acReserve[128];                 /*!< 保留数据                                */
}
#ifdef _WIN32
EventPersonPassExpire, *LPEventPersonPassExpire;
#pragma pack()
#endif
#ifdef __linux__
__attribute__((packed)) EventPersonPassExpire, *LPEventPersonPassExpire;
#endif

/*!
*  @struct    tagEventPeopleDetectInfo
*  @brief     人形检测事件信息
*/
typedef struct tagEventPeopleDetectInfo
{
    DWORD    dwID;                                /*!< 事件ID：同一事件在多片段发送时使用相同ID标识    */
    DWORD    dwTimeSec;                           /*!< 绝对时间戳(单位：秒)    */
    DWORD    dwTimeUSec;                          /*!< 绝对时间戳(单位：微秒)    */
    DWORD    dwImageCodec;                        /*!< 图片格式：0-JPEG,1-PNG   */
    ImageInfo   cPeopleImage;                     /*!< 人形图片信息：实时抓拍图片信息            */

    char    cReserve[256];                        /*!< 保留数据    */
}EventPeopleDetectInfo, *LPEventPeopleDetectInfo;

/*!
*  @struct	tagEventFireDetectInfo
*  @brief	火焰检测事件信息
*/
typedef struct tagEventFireDetectInfo
{
	DWORD	dwID; 								/*!< 事件ID：同一事件在多片段发送时使用相同ID标识	*/
	DWORD	dwTimeSec;							/*!< 绝对时间戳(单位：秒)	*/
	DWORD	dwTimeUSec;							/*!< 绝对时间戳(单位：微秒)	*/
	DWORD	dwImageCodec;						/*!< 图片格式：0-JPEG,1-png	*/
    ImageInfo	cFireImage;						/*!< 火焰图片信息：实时抓拍图片信息			*/
	ObjectRect	cFireRect;						/*!< 火焰位置：描述人脸图像区域位置信息		*/
	float	fFireTemperature;					/*!< 火焰温度	*/
	char	cTemperatureUnit;					/*!< 火焰温度单位(0：摄氏度,1：华氏度)	*/
	char	cFireTemperatureIntensity;			/*!< 火焰温度强度(0：不支持    1：常温	2：高温)	*/
	
	char	cReserve[256];						/*!< 保留数据	*/
}EventFireDetectInfo, *LPEventFireDetectInfo;



/*!
*  @struct	tagEventVideoCover
*  @brief	视频遮挡事件信息
*/
typedef struct tagEventVideoCover
{
	DWORD	dwID; 								/*!< 事件ID：同一事件在多片段发送时使用相同ID标识	*/
	DWORD	dwTimeSec;							/*!< 绝对时间戳(单位：秒)	*/
	DWORD	dwTimeUSec;							/*!< 绝对时间戳(单位：微秒)	*/
	DWORD	dwImageCodec;						/*!< 图片格式：0-JPEG,1-png	*/
    ImageInfo	cImageInfo;						/*!< 图片信息：实时抓拍图片信息			*/
	ObjectRect	cRect;						    /*!< 位置：描述图像区域位置信息		*/
	
	char	cReserve[256];						/*!< 保留数据	*/
}EventVideoCover, *LPEventVideoCover;


/*!
*  @struct	tagEventElectricBike
*  @brief	电瓶车事件信息
*/
typedef struct tagEventElectricBike
{
	DWORD	dwID; 								/*!< 事件ID：同一事件在多片段发送时使用相同ID标识	*/
	DWORD	dwTimeSec;							/*!< 绝对时间戳(单位：秒)	*/
	DWORD	dwTimeUSec;							/*!< 绝对时间戳(单位：微秒)	*/
	DWORD	dwImageCodec;						/*!< 图片格式：0-JPEG,1-png	*/
    ImageInfo	cImageInfo;						/*!< 图片信息：实时抓拍图片信息			*/
	ObjectRect	cRect;						    /*!< 位置：描述人脸图像区域位置信息		*/
	
	char	cReserve[256];						/*!< 保留数据	*/
}EventElectricBike, *LPEventElectricBike;


/*!
*  @struct	tagEventGasContainer
*  @brief	煤气罐事件信息
*/
typedef struct tagEventGasContainer
{
	DWORD	dwID; 								/*!< 事件ID：同一事件在多片段发送时使用相同ID标识	*/
	DWORD	dwTimeSec;							/*!< 绝对时间戳(单位：秒)	*/
	DWORD	dwTimeUSec;							/*!< 绝对时间戳(单位：微秒)	*/
	DWORD	dwImageCodec;						/*!< 图片格式：0-JPEG,1-png	*/
    ImageInfo	cImageInfo;						/*!< 图片信息：实时抓拍图片信息			*/
	ObjectRect	cRect;						    /*!< 位置：描述人脸图像区域位置信息		*/
	
	char	cReserve[256];						/*!< 保留数据	*/
}EventGasContainer, *LPEventGasContainer;


/*!
*  @struct	tagEventFaceRecognitionV2
*  @brief	人脸识别事件V2
*/
/*事件负载缓冲区内容视图
格式分布：[EventFaceRecognitionV2][Image0]
EventFaceRecognition：表示人脸识别事件主结构
Image0：表示人脸抓拍图，人脸底库图以md5sum的方式给出，图片信息见cFaceImage
一次人脸识别匹配事件只发送一次，既包括了人脸抓拍信息以及图片，也包括了人脸识别信息
*/
#ifdef _WIN32
#pragma pack(1)
#endif
typedef struct tagEventFaceRecognitionV2
{
	DWORD				dwID;
	/*!< 事件ID：同一事件在多片段发送时使用相同ID标识*/
	DWORD				dwTimeSec;
	/*!< 绝对时间戳（单位：秒）					*/
	DWORD				dwTimeUSec;
	/*!< 绝对时间戳（单位：微秒）				*/
	DWORD				dwFaceNumber;
	/*!< 人脸数量：当前实时图像中检测到的人脸数量*/
	DWORD				dwImageCodec;
	/*!< 图片格式：0-JPEG						*/
	DWORD				dwMatchNumber;
	/*!< 匹配数量：0-未匹配到目标，否则为匹配目标数量*/
	FaceCaptureInfo		cCaptureInfo;
	/*!< 抓拍图片信息：描述抓拍图片基本信息		*/
	FaceMatchInfoV2		cMatchInfo;
	/*!< 匹配图片信息：描述目标图片基本信息		*/
	DWORD				dwRuleID;
	/*!< 规则ID：触发事件的源规则，抓拍事件时填充0xFFFFFFFF*/
	float						fFaceTemperature;
	/*!< 人脸温度						*/
	char						cTemperatureUnit;
	/*!< 人脸温度(0：摄氏度	1：华氏度)			*/
	char						cIsNormalFaceTemperature;
	/*!< 温度是否正常(0：不支持    1：温度正常	2：温度异常)						*/
	char						cHealthCodeInfoValid;
	/*!< 健康码信息是否有效(0：无效，1：有效)					*/
	char				cReserve[63];
	/*!< 保留数据								*/
}
#ifdef _WIN32
EventFaceRecognitionV2, *LPEventFaceRecognitionV2;
#pragma pack()
#endif
#ifdef __linux__
__attribute__((packed)) EventFaceRecognitionV2, *LPEventFaceRecognitionV2;
#endif

/*!
*  @struct    tagBreathBackgroundInfo
*  @brief    背景图片信息
*/
typedef struct tagFaceBackgroundInfo
{
    int         iHasBackground;            /*!< 是否有背景图片信息;0-没有,1-有 */
    ImageInfo   cImageInfo;                /*!< 背景图片信息 */

    char        cReserve[128];             /*!< 保留数据 */
}FaceBackgroundInfo, *LPFaceBackgroundInfo;

#ifdef _WIN32
#pragma pack(1)
#endif
typedef struct tagEventFaceRecognitionV4
{
    DWORD    dwID;                         /*!< 事件ID：同一事件在多片段发送时使用相同ID标识 */
	DWORD	 dwType;					   /*!< 类型：0-人脸抓拍：实时抓拍到的目标人脸
												1-人脸比对：脸库中的匹配目标人脸
												2-背景图：智能视频分析背景图		*/
    DWORD    dwTimeSec;                    /*!< 绝对时间戳（单位：秒） */
    DWORD    dwTimeUSec;                   /*!< 绝对时间戳（单位：微秒） */
    DWORD    dwFaceNumber;                 /*!< 人脸数量：当前实时图像中检测到的人脸数量 */
    DWORD    dwImageCodec;                 /*!< 图片格式：0-JPEG  */
    DWORD    dwMatchNumber;                /*!< 匹配数量：0-未匹配到目标，否则为匹配目标数量 */
    FaceCaptureInfoV3  cCaptureInfo;       /*!< 抓拍图片信息：描述抓拍图片基本信息  */
    FaceMatchInfoV3    cMatchInfo;         /*!< 匹配图片信息：描述目标图片基本信息  */
    DWORD    dwRuleID;                     /*!< 规则ID：触发事件的源规则，抓拍事件时填充0xFFFFFFFF */
    float    fFaceTemperature;             /*!< 人脸温度  */
    char     cTemperatureUnit;             /*!< 人脸温度(0：摄氏度 1：华氏度) */
    char     cIsNormalFaceTemperature;     /*!< 温度是否正常(0：不支持 1：温度正常 2：温度异常)   */
    char     cHasFaceCaptureInfo;          /*!< 是否有抓拍图(0：无，1：有)  */
    char     cHasFaceMatchInfoV2;          /*!< 是否有匹配图(0：无，1：有)  */
    char     cHealthCodeInfoValid;         /*!< 健康码信息是否有效(0：无效，1：有效)  */
    HealthCodeData    stHealthCodeData;    /*!< 健康码数据信息  */
    FaceBackgroundInfo stBackgroundInfo;   /*!< 背景图片信息：描述目标图片基本信息  */

    char     cReserve[512];
    /*!< 保留数据                                */
}
#ifdef _WIN32
EventFaceRecognitionV4, *LPEventFaceRecognitionV4;
#pragma pack()
#endif
#ifdef __linux__
__attribute__((packed)) EventFaceRecognitionV4, *LPEventFaceRecognitionV4;
#endif

/*!
*  @struct    tagBreathBackgroundInfo
*  @brief    背景图片信息
*/
typedef struct tagBreathBackgroundInfo
{
    int         iHasBackground;            /*!< 是否有背景图片信息:0-没有,1-有 */
    ImageInfo   cImageInfo;                /*!< 背景图片信息 */

    char        cReserve[128];             /*!< 保留数据 */
}BreathBackgroundInfo, *LPBreathBackgroundInfo;

typedef struct tagEventBreathMask
{
    DWORD                dwID;
    /*!< 事件ID：同一事件在多片段发送时使用相同ID标识*/
    DWORD                dwTimeSec;
    /*!< 绝对时间戳（单位：秒）                    */
    DWORD                dwTimeUSec;
    /*!< 绝对时间戳（单位：微秒）                */
    DWORD                dwImageCodec;
    /*!< 图片格式：0-JPEG                        */
    DWORD                dwRuleID;
    /*!< 规则ID：触发事件的源规则，抓拍事件时填充0xFFFFFFFF*/
    int                  iHasFaceMask;            
    /*!< 是否带了口罩:0-没有,1-有 */
	FaceCaptureInfo        cCaptureInfo;
    /*!< 抓拍图片信息：描述抓拍图片基本信息        */
    BreathBackgroundInfo stBackgroundInfo;
    /*!< 背景图片信息：描述背景图片基本信息        */

    char                cReserve[256];
    /*!< 保留数据                                */
}EventBreathMask, *LPEventBreathMask;

/*!
*  @struct	tagEventPanoramicSnapshot
*  @brief	全景抓拍图片事件
*/
/*事件负载缓冲区内容视图
格式分布：[EventPanoramicSnapshot][Image0]
EventPanoramicSnapshot：表示全景抓拍图片事件主结构
ImageInfo：表示抓拍图，图片信息见cCaptureImageInfo
*/
#ifdef _WIN32
#pragma pack(1)
#endif
typedef struct tagEventPanoramicSnapshot
{
    DWORD				dwID;					/*!< 事件ID：同一事件在多片段发送时使用相同ID标识*/
	DWORD				dwTimeSec;				/*!< 绝对时间戳（单位：秒）*/
	DWORD				dwTimeUSec;				/*!< 绝对时间戳（单位：微秒）*/
	DWORD				dwImageCodec;			/*!< 图片格式：0-JPEG;1-PNG	*/
	ImageInfo			cCaptureImageInfo;		/*!< 抓拍图片信息：描述抓拍图片基本信息	*/
	DWORD 				dwEventType;			/*!< 事件主类型,对应sendEvent/sendEventEx接口传入的dwEventType */
    DWORD 				dwSubEventType;			/*!< 事件次类型,对应sendEvent/sendEventEx接口传入的dwSubEventType */
	
    char				cReserve[64];	/*!< 保留数据	*/
}
#ifdef _WIN32
EventPanoramicSnapshot, *LPEventPanoramicSnapshot;
#pragma pack()
#endif
#ifdef __linux__
__attribute__((packed)) EventPanoramicSnapshot, *LPEventPanoramicSnapshot;
#endif

/*!
*  @struct	tagCheckInMatchInfo
*  @brief	人脸考勤信息结构体（适配济工网平台）
*/
typedef struct tagFaceCheckInMatchInfo
{
	float	fMatch;						/*!< 匹配度：值域范围（0~100.0）*/
	char	szCertID[32];				/*!< 匹配人员身份证ID*/
	char	szName[32];					/*!< 匹配人员姓名*/
	int		nSex;						/*!< 匹配人员性别*/
	char	szCheckInTime[32];			/*!< 签入时间 2018-09-19 16:29:59"*/
	int		nIOFlag;					/*!< 签入方向		2进 3出*/
	char	cReserve[32];				/*!< 保留数据							*/
}FaceCheckInMatchInfo, *LPFaceCheckInMatchInfo;

/*!
*  @struct	tagEventAttendanceOperatType
*  @brief	考勤操作类型事件结构体
*/
typedef struct tagEventAttendanceOperatType
{
	DWORD				dwID;							/*!< 事件ID：同一事件在多片段发送时使用相同ID标识*/
	DWORD				dwTimeSec;						/*!< 绝对时间戳（单位：秒）					*/
	DWORD				dwTimeUSec;						/*!< 绝对时间戳（单位：微秒）				*/
    DWORD				dwOperatType; 					/*!< 考勤操作类型,0-出勤,1-退勤,2-外出,3-返回*/
	char				cReserve[256];					/*!< 保留字段 */
}EventAttendanceOperatType, *LPEventAttendanceOperatType;

/*!
*  @struct	tagEventExportAttendanceRecord
*  @brief	导出考勤记录事件结构体
*/
typedef struct tagEventExportAttendanceRecord
{
	DWORD				dwID;							/*!< 事件ID：同一事件在多片段发送时使用相同ID标识*/
	DWORD				dwTimeSec;						/*!< 绝对时间戳（单位：秒）					*/
	DWORD				dwTimeUSec;						/*!< 绝对时间戳（单位：微秒）				*/
	char				cReserve[256];					/*!< 保留字段 */
}EventExportAttendanceRecord, *LPEventExportAttendanceRecord;

/*!
*  @struct	tagEventPersonLeave
*  @brief	人员离开事件结构体
*/
typedef struct tagEventPersonLeave
{
	DWORD				dwID;							/*!< 事件ID：同一事件在多片段发送时使用相同ID标识 */
	DWORD				dwTimeSec;						/*!< 绝对时间戳（单位：秒） */
	DWORD				dwTimeUSec;						/*!< 绝对时间戳（单位：微秒） */
    
	char				cReserve[256];					/*!< 保留字段 */
}EventPersonLeave, *LPEventPersonLeave;

typedef struct tagEventDevSleepState
{
    DWORD					dwID;						/*!< 事件ID：同一事件在多片段发送时使用相同ID标识*/
    int nStatus;		//0：设备为睡眠状态  1设备为唤醒状态
    char szReserved[32];
}EventDevSleepState, *LPEventDevSleepState;

typedef struct tagEventPoisonAir
{
    DWORD					dwID;						/*!< 事件ID：同一事件在多片段发送时使用相同ID标识*/
    DWORD					dwTimeSec;					/*!< 绝对时间戳（单位：秒）*/
    DWORD					dwTimeUSec;					/*!< 绝对时间戳（单位：微秒）*/
    int nStatus;		// 当前环境气体质量等级  0：洁净  1：轻度污染  2：中度污染 3：重度污染
    char szReserved[32];
}EventPoisonAir, *LPEventPoisonAir;

typedef struct tagEventKey2Call
{
    DWORD               dwID;                           /*!< 事件ID：同一事件在多片段发送时使用相同ID标识   */
    DWORD               dwTimeSec;                      /*!< 绝对时间戳(单位:秒)    */
    DWORD               dwTimeUSec;                     /*!< 绝对时间戳(单位:微秒)   */
    DWORD               dwImageCodec;                   /*!< 图片格式：0-JPEG;1-PNG  */
    ImageInfo           cImageInfo;                     /*!< 图片信息   */

    char reserved[512];
}EventKey2Call, *LPEventKey2Call;

/*!
*  @struct	tagEventRemoteUpdateNotifyInfo
*  @brief	远程升级通知事件结构体
*/
typedef struct tagEventRemoteUpdateNotifyInfo
{
	DWORD				dwID;									/*!< 事件ID：同一事件在多片段发送时使用相同ID标识*/
	DWORD				dwTimeSec;								/*!< 绝对时间戳（单位：秒）					*/
	DWORD				dwTimeUSec;								/*!< 绝对时间戳（单位：微秒）				*/
	char				cReserve[256];							/*!< 保留字段 */
}EventRemoteUpdateNotifyInfo, *LPEventRemoteUpdateNotifyInfo;

/*!
*  @struct	tagEventFaceRecognitionV3
*  @brief	人脸识别事件V3
*/
/*事件负载缓冲区内容视图
格式分布：[EventFaceRecognitionV3][Image0]
EventFaceRecognition：表示人脸识别事件主结构
Image0：带时间格式的人脸背景图
*/
#ifdef _WIN32
#pragma pack(1)
#endif
typedef struct tagEventFaceRecognitionV3
{
	DWORD					dwID;						/*!< 事件ID：同一事件在多片段发送时使用相同ID标识*/
	DWORD					dwTimeSec;					/*!< 绝对时间戳（单位：秒）					*/
	DWORD					dwTimeUSec;					/*!< 绝对时间戳（单位：微秒）				*/
	DWORD					dwImageCodec;				/*!< 图片格式：默认是 0-JPEG						*/
	DWORD					dwPicLength;				/*!< 图片长度 */
	FaceCheckInMatchInfo	cMatchInfo;					/*!< 人脸考勤信息		*/
	float					fFaceTemperature;			/*!< 人脸温度，默认使用摄氏度单位			*/
	char					cTemperatureUnit;			/*!< 人脸温度(0：摄氏度	1：华氏度)			*/
	char					cIsNormalFaceTemperature;	/*!< 温度是否正常(0：不支持    1：温度正常	2：温度异常)						*/
	char					cReserve[64];				/*!< 保留数据								*/
}
#ifdef _WIN32
EventFaceRecognitionV3, *LPEventFaceRecognitionV3;
#pragma pack()
#endif
#ifdef __linux__
__attribute__((packed)) EventFaceRecognitionV3, *LPEventFaceRecognitionV3;
#endif


/*!
*  @struct	tagCheckInMatchInfo
*  @brief	NFC卡考勤信息结构体（适配济工网平台）
*/
typedef struct tagNfcCardCheckInMatchInfo
{
	char	szCertID[32];				/*!< 卡ID*/
	char	szCheckInTime[32];			/*!< 签入时间 2018-09-19 16:29:59"*/
	int		nIOFlag;					/*!< 签入方向		2进 3出*/
	char	cReserve[32];				/*!< 保留数据							*/
}NfcCardCheckInMatchInfo, *LPNfcCardCheckInMatchInfo;


/*!
*  @struct	tagNfcCardRecognition
*  @brief	NFC刷卡事件
*/
#ifdef _WIN32
#pragma pack(1)
#endif
typedef struct tagNfcCardRecognition
{
	DWORD					dwID;						/*!< 事件ID：同一事件在多片段发送时使用相同ID标识*/
	DWORD					dwTimeSec;					/*!< 绝对时间戳（单位：秒）					*/
	DWORD					dwTimeUSec;					/*!< 绝对时间戳（单位：微秒）				*/
	NfcCardCheckInMatchInfo	cMatchInfo;					/*!< 卡考勤信息		*/
	char					cReserve[64];				/*!< 保留数据								*/
}
#ifdef _WIN32
EventNfcCardRecognition, *LPEventNfcCardRecognition;
#pragma pack()
#endif
#ifdef __linux__
__attribute__((packed)) EventNfcCardRecognition, *LPEventNfcCardRecognition;
#endif


/*!
*  @struct	tagEbikeCaptureInfo
*  @brief	梯控报警抓图信息结构体
*/
typedef struct tagEbikeCaptureInfo
{
	DWORD			dwPicQuality;				/*!< 抓图质量								*/
	DWORD			dwType;						/*!< 图片格式 0-JPG，目前只支持JPEG				*/
	DWORD			dwWidth;					/*!< 图片宽度								*/
	DWORD			dwHeight;					/*!< 图片高度								*/
	DWORD			dwLength;					/*!< 文件大小								*/
	char			cReserve[32];				/*!< 保留数据								*/
}EbikeCaptureInfo, *LPEbikeCaputreInfo;


/*!
*  @struct	tagEbikeSnapshort
*  @brief	梯控报警抓图（结构体+图片）
*/
#ifdef _WIN32
#pragma pack(1)
#endif
typedef struct tagEbikeCaputre
{
	DWORD					dwID;						/*!< 事件ID：同一事件在多片段发送时使用相同ID标识			*/
	DWORD					dwTimeSec;					/*!< 绝对时间戳（单位：秒）									*/
	DWORD					dwTimeUSec;					/*!< 绝对时间戳（单位：微秒）									*/
	EbikeCaptureInfo		cCaptureInfo;				/*!< 抓图信息											*/
	ObjectRect				cCaptureRect;				/*!< 电动车在背景图中的位置									*/
	char					cReserve[64];				/*!< 保留数据											*/
}
#ifdef _WIN32
EventEbikeCaputre, *LPEventEbikeCaputre;
#pragma pack()
#endif
#ifdef __linux__
__attribute__((packed)) EventEbikeCaputre, *LPEventEbikeCaputre;
#endif

/*!
*  @enum	eCMCCVehicleType
*  @brief	CMCC车辆类型
*/
typedef enum tagCMCCVehicleType
{
	CMCC_VEHICLE_TYPE_K10 = 10,				/*!< 大客车									*/
	CMCC_VEHICLE_TYPE_K20 = 20,				/*!< 中客车									*/
	CMCC_VEHICLE_TYPE_H10 = 100,			/*!< 大货车									*/
	CMCC_VEHICLE_TYPE_H20 = 200,			/*!< 中货车									*/
	CMCC_VEHICLE_TYPE_H22 = 220,			/*!< 厢式货车									*/
	CMCC_VEHICLE_TYPE_H30 = 300,			/*!< 小货车									*/
	CMCC_VEHICLE_TYPE_X1 = 1100,			/*!< 小轿车									*/
	CMCC_VEHICLE_TYPE_X2 = 1200,			/*!< 面包车									*/
	CMCC_VEHICLE_TYPE_X3 = 1300,			/*!< SUV									*/
	CMCC_VEHICLE_TYPE_X4 = 1400,			/*!< MPV									*/
	CMCC_VEHICLE_TYPE_X5 = 1500,			/*!< 皮卡										*/
	CMCC_VEHICLE_TYPE_X6 = 1600,			/*!< 微型车									*/
	CMCC_VEHICLE_TYPE_X7 = 1700			/*!< 公交车									*/
}eCMCCVehicleType;

/*!
*  @enum	eCMCCVehicleType
*  @brief	CMCC车辆和车牌颜色
*/
typedef enum tagCMCCVehicleColor
{
	CMCC_VEHICLE_COLOR_BLACK = 1,				/*!< 黑色									*/
	CMCC_VEHICLE_COLOR_WHITE,					/*!< 白色									*/
	CMCC_VEHICLE_COLOR_ASH,						/*!< 灰色									*/
	CMCC_VEHICLE_COLOR_RED,						/*!< 红色									*/
	CMCC_VEHICLE_COLOR_BLUE,					/*!< 蓝色									*/
	CMCC_VEHICLE_COLOR_YELLOW,					/*!< 黄色									*/
	CMCC_VEHICLE_COLOR_ORANGE,					/*!< 橘色									*/
	CMCC_VEHICLE_COLOR_BROWN,					/*!< 棕色									*/
	CMCC_VEHICLE_COLOR_GREEN,					/*!< 绿色									*/
	CMCC_VEHICLE_COLOR_PURPLE,					/*!< 紫色									*/
	CMCC_VEHICLE_COLOR_YOUNG,
	CMCC_VEHICLE_COLOR_POWDER,
	CMCC_VEHICLE_COLOR_TRANSPARENT,				/*!< 透明色								*/
	CMCC_VEHICLE_COLOR_OTHER = 99,
	CMCC_VEHICLE_COLOR_SILVERY = 1114			/*!< 银色									*/
}eCMCCVehicleColor;

/*!
*  @enum	eCMCCPlateType
*  @brief	CMCC车牌类型
*/
typedef enum tagCMCCPlateType
{
	CMCC_PLATE_TYPE_LARGE_CAR = 1,					/*!< 大型汽车号牌									*/
	CMCC_PLATE_TYPE_SMALL_CAR,						/*!< 小型汽车号牌									*/
	CMCC_PLATE_TYPE_EMABSSY_CAR,					/*!< 使馆汽车号牌									*/
	CMCC_PLATE_TYPE_CONSULATE_CAR,					/*!< 领馆汽车号牌									*/
	CMCC_PLATE_TYPE_OVERSEAS_CAR,					/*!< 境外汽车号牌									*/
	CMCC_PLATE_TYPE_FOREIGN_CAR,					/*!< 外籍汽车号牌									*/
	CMCC_PLATE_TYPE_ORDINARY_MOTORCYCLE,			/*!< 普通摩托车号牌								*/
	CMCC_PLATE_TYPE_MOPED_MOTORCYCLE,				/*!< 轻便摩托车号牌								*/
	CMCC_PLATE_TYPE_EMBASSY_MOTORCYCLE,				/*!< 使馆摩托车号牌								*/
	CMCC_PLATE_TYPE_CONSULATE_MOTORCYCLE,			/*!< 领馆摩托车号牌								*/
	CMCC_PLATE_TYPE_OVERSEAS_MOTORCYCLE,			/*!< 境外摩托车号牌								*/
	CMCC_PLATE_TYPE_FOREIGN_MOTORCYCLE,				/*!< 外籍摩托车号牌								*/
	CMCC_PLATE_TYPE_LOW_SPEED_VEHICLE,				/*!< 低速车号牌									*/
	CMCC_PLATE_TYPE_TRACTOR,						/*!< 拖拉机号牌									*/
	CMCC_PLATE_TYPE_TRAILER,						/*!< 挂车号牌									*/
	CMCC_PLATE_TYPE_COACH_CAR,						/*!< 教练汽车号牌									*/
	CMCC_PLATE_TYPE_COACH_MOTORCYCLE,				/*!< 教练摩托车号牌								*/
	CMCC_PLATE_TYPE_TEMPORARY_ENTRY_VEHICLE = 20,	/*!< 临时入境汽车号牌								*/
	CMCC_PLATE_TYPE_TEMPORARY_ENTRY_MOTORCYCLE,		/*!< 临时入境摩托车号牌								*/
	CMCC_PLATE_TYPE_TEMPORARY_DRIVING_VEHICLE,		/*!< 临时行驶车号牌								*/
	CMCC_PLATE_TYPE_POLICE_CAR,						/*!< 警用汽车号牌									*/
	CMCC_PLATE_TYPE_POLICE_MOTORCYCLE,				/*!< 警用摩托车号牌								*/
	CMCC_PLATE_TYPE_AGRICULTURAL_MACHINERY,			/*!< 原农机号牌									*/
	CMCC_PLATE_TYPE_HONGKONG,						/*!< 香港入出境号牌								*/
	CMCC_PLATE_TYPE_MACAO,							/*!< 澳门入出境号牌								*/
	CMCC_PLATE_TYPE_ARMED_POLICE = 31,					/*!< 武警号牌									*/
	CMCC_PLATE_TYPE_PLA,							/*!< 军队号牌									*/
	CMCC_PLATE_TYPE_OTHER = 99,						/*!< 其他号牌									*/
	CMCC_PLATE_TYPE_BLUE = 1133,					/*!< 蓝牌										*/
	CMCC_PLATE_TYPE_BLACK,							/*!< 黑牌										*/
	CMCC_PLATE_TYPE_YELLOW,							/*!< 黄牌										*/
	CMCC_PLATE_TYPE_NEW_ENERGY,						/*!< 新能源									*/
	CMCC_PLATE_TYPE_NO_PLATE,						/*!< 无车牌									*/
	CMCC_PLATE_TYPE_COVERD_PLATE					/*!< 无法识别车牌									*/
}eCMCCPlateType;

/*!
*  @struct	tagVehicleCapturePictureInfo
*  @brief	CMCC机动车报警抓图图片信息结构体
*/
typedef struct tagVehicleCapturePictureInfo
{
	DWORD			dwPicQuality;				/*!< 抓图质量								*/
	DWORD			dwPicType;					/*!< 图片格式 0-JPG，目前只支持JPEG				*/
	DWORD			dwPicWidth;					/*!< 图片宽度								*/
	DWORD			dwPicHeight;				/*!< 图片高度								*/
	DWORD			dwPicLength;				/*!< 文件大小								*/
	char*			pPicData;					/*!< 图片地址								*/
	char			cReserve[32];				/*!< 保留数据								*/
}VehicleCapturePictureInfo, *LPVehicleCapturePictureInfo;

/*!
*  @struct	tagVehicleCaptureAtrributeCommonInfo
*  @brief	CMCC机动车报警抓图图片信息结构体
*/
typedef struct tagVehicleCaptureAttributeCommonInfo
{
	DWORD			dwAttriClass;					/*!< 类别									*/
	DWORD			dwAttriColor;					/*!< 颜色									*/
	DWORD			dwAttriAccuracy;				/*!< 识别精度（0-100）						*/
	char			cReserve[32];					/*!< 保留数据								*/
}VehicleCaptureAtrributeCommonInfo, *LPVehicleCaptureAtrributeCommonInfo;

/*!
*  @struct	tagVehicleCaptureAttributeInfo
*  @brief	CMCC机动车报警抓图属性信息结构体
*/
typedef struct tagVehicleCaptureAttributeInfo
{
	VehicleCaptureAtrributeCommonInfo		cAttriCommon;					/*!< 公共属性								*/
	DWORD									dwAttriInOut;					/*!< 车辆进出方向（0进1出）						*/
	char									cReserve[32];					/*!< 保留数据								*/
}VehicleCaptureAttributeInfo, *LPVehicleCaptureAttributeInfo;

/*!
*  @struct	tagVehicleCaptureInfo
*  @brief	CMCC机动车报警抓图信息结构体
*/
typedef struct tagVehicleCaptureInfo
{
	VehicleCapturePictureInfo			cPicInfo;		/*!< 抓图信息									*/
	VehicleCaptureAttributeInfo			cAttriInfo;		/*!< 属性信息									*/
	char								cReserve[32];	/*!< 保留数据									*/
}VehicleCaptureInfo, *LPVehicleCaputreInfo;

/*!
*  @struct	tagPlateCaptureAttributeInfo
*  @brief	CMCC机动车车牌抓图属性信息结构体
*/
typedef struct tagPlateCaptureAttributeInfo
{
	VehicleCaptureAtrributeCommonInfo		cAttriCommon;					/*!< 公共属性								*/
	char									cNumber[32];					/*!< 车牌号码								*/
	char									cReserve[32];					/*!< 保留数据								*/
}PlateCaptureAttributeInfo, *LPPlateCaptureAttributeInfo;


/*!
*  @struct	tagPlateCaptureInfo
*  @brief	CMCC机动车报警抓图信息结构体
*/
typedef struct tagPlateCaptureInfo
{
	VehicleCapturePictureInfo			cPicInfo;		/*!< 抓图信息									*/
	PlateCaptureAttributeInfo			cAttriInfo;		/*!< 属性信息									*/
	char								cReserve[32];	/*!< 保留数据									*/
}PlateCaptureInfo, *LPPlateCaptureInfo;



/*!
*  @struct	tagVehicleSnapshort
*  @brief	CMCC机动车报警抓图（结构体+大图图片+小图图片）
*/
#ifdef _WIN32
#pragma pack(1)
#endif
typedef struct tagEventVehicleCaputre
{
	DWORD					dwID;						/*!< 事件ID：同一事件在多片段发送时使用相同ID标识			*/
	DWORD					dwTimeSec;					/*!< 绝对时间戳（单位：秒）									*/
	DWORD					dwTimeUSec;					/*!< 绝对时间戳（单位：微秒）									*/
	VehicleCaptureInfo		cCaptureBackgroundInfo;		/*!< 大图机动车抓图信息										*/
	PlateCaptureInfo		cCapturePlateInfo;			/*!< 小图车牌抓图信息										*/
	DWORD                   dwRegionId;                 /* 检测区域ID  0表示无区域   1-n  表区域序号 */
	char					cReserve[64-4];				/*!< 保留数据											*/
	void*					pReserved;					/*!< 保留字段，后续多车牌用									*/
}
#ifdef _WIN32
EventVehicleCaputre, *LPEventVehicleCaputre;
#pragma pack()
#endif
#ifdef __linux__
__attribute__((packed)) EventVehicleCaputre, *LPEventVehicleCaputre;
#endif




#ifdef _WIN32
#pragma pack(1)
#endif
typedef struct tagEventFenceDetectAlarm
{
	DWORD				dwIndex;					/*!< 电子围栏的索引*/
	DWORD				dwFenceDir;					/*!< 类型:0 向内(进入)或1 向外(离开)、2 逗留*/
	char*               pJpgBuff;					/*!< 上报1080P的图片*/	
	DWORD               dwJpgLen;					/*!< 图片长度*/	
	time_t              dwEventTime;                /*!< 事件的时间，要与图片上面OSD的时间要一致*/
	DWORD               dwEventType;                /*!< 类型:0 人行或 1 机动车、2 非机动车*/
	char				cReserve[64 - 8 - 8];				/*!< 保留数据*/
}
#ifdef _WIN32
EventFenceDetectAlarm, *LPEventFenceDetectAlarm;
#pragma pack()
#endif
#ifdef __linux__
__attribute__((packed)) EventFenceDetectAlarm, *LPEventFenceDetectAlarm;
#endif


#ifdef _WIN32
#pragma pack(1)
#endif
typedef struct tagEventMaskDetectAlarm
{
	DWORD				dwIndex;					/*!< 口罩检测的索引*/
	char*               pJpgBuff;					/*!< 上报1080P的图片*/	
	DWORD               dwJpgLen;					/*!< 图片长度*/	
	time_t              dwEventTime;                /*!< 事件的时间，要与图片上面OSD的时间要一致*/
	char				cReserve[64];				/*!< 保留数据*/
}
#ifdef _WIN32
EventMaskDetectAlarm, *LPEventMaskDetectAlarm;
#pragma pack()
#endif
#ifdef __linux__
__attribute__((packed)) EventMaskDetectAlarm, *LPEventMaskDetectAlarm;
#endif


#ifdef _WIN32
#pragma pack(1)
#endif
typedef struct tagEventTianYiV2BatteryBikeAlarm
{
	DWORD				dwIndex;					/*!< 电动车检测的索引*/
	DWORD               dwForcePush;                /*!< 是否为新电动车事件   1:是新事件强推 0:不是新事件*/ 
	char*               pJpgBuff;					/*!< 上报1080P的图片*/	
	DWORD               dwJpgLen;					/*!< 图片长度*/	
	DWORD               dwEventSec;                /*!< 事件的时间，要与图片上面OSD的时间要一致  秒*/
	char				cReserve[64];				/*!< 保留数据*/
}
#ifdef _WIN32
EventTianYiV2BatteryBikeAlarm, *LPEventTianYiV2BatteryBikeAlarm;
#pragma pack()
#endif
#ifdef __linux__
__attribute__((packed)) EventTianYiV2BatteryBikeAlarm, *LPEventTianYiV2BatteryBikeAlarm;
#endif

#ifdef _WIN32
#pragma pack(1)
#endif
typedef struct tagEventFallingObjectDetectAlarm
{
	DWORD				dwIndex;					/*!< 图片的索引*/
	unsigned long long		dwAlarmStartTime;		/*!< 事件开始的时间*/
	char*               pJpgBuff;					/*!< 上报图片*/	
	DWORD               dwJpgLen;					/*!< 图片长度*/	
	char				cReserve[64];				/*!< 保留数据*/
}
#ifdef _WIN32
EventFallingObjectDetectAlarm, *LPEventFallingObjectDetectAlarm;
#pragma pack()
#endif
#ifdef __linux__
__attribute__((packed)) EventFallingObjectDetectAlarm, *LPEventFallingObjectDetectAlarm;
#endif


#ifdef _WIN32
#pragma pack(1)
#endif
typedef struct tagEventHumCount
{
	DWORD dwID;
	DWORD nHumInNum;     //进入人数
	DWORD nHumOutNum;    //出来人数
	char strStartTime[32];  //开始时间  格式:"202103170404"
	char strEndTime[32];    //结束时间
	DWORD dwTimeSec;			/*!< 绝对时间戳(单位:秒)    */
    DWORD dwTimeUSec;			/*!< 绝对时间戳(单位:微秒)   */

	char cReserve[56];				/*!< 保留数据*/
}
#ifdef _WIN32
EventHumCount, *LPEventHumCount;
#pragma pack()
#endif
#ifdef __linux__
__attribute__((packed)) EventHumCount, *LPEventHumCount;
#endif

#ifdef _WIN32
#pragma pack(1)
#endif
typedef struct tagEventCarLicenseRecognition
{
	DWORD dwID;
	char strCarNum[16];   //车牌字符串
	DWORD nCarNumLen;   //车牌图的长度
	char* pCarNum;     //车牌图的地址
	DWORD nCarBackLen;   //背景图长度
	char* pCarBack;     //背景图的地址   

	char cReserve[64];				/*!< 保留数据*/
}
#ifdef _WIN32
EventCarLicenseRecognition, *LPEventCarLicenseRecognition;
#pragma pack()
#endif
#ifdef __linux__
__attribute__((packed)) EventCarLicenseRecognition, *LPEventCarLicenseRecognition;
#endif


typedef struct tagEventAlarmIn
{
	DWORD dwID;
	char cAlarmInChannel;
	char cAlarmInName[32];
	char szReserved[220];
}EventAlarmIn, *LPEventAlarmIn;

/*!
*  @struct    tagEventAIMotion
*  @brief    P6SEvent 移动侦测事件结构体
*  @illustrate  原先定义的结构体只有ID字段和256的保留字节,不能满足P6SEvent功能需求,
*				同时hieserver加载的协议和非hieserver加载协议都没有用到该结构体,所以直接修改使用
*/
#ifdef _WIN32
#pragma pack(1)
#endif
typedef struct tagEventAIMotion
{
    DWORD               dwID;                           /*!< 事件ID：同一事件在多片段发送时使用相同ID标识   */
    DWORD               dwTimeSec;                      /*!< 绝对时间戳(单位:秒)    */
    DWORD               dwTimeUSec;                     /*!< 绝对时间戳(单位:微秒)   */
    DWORD               dwImageCodec;                   /*!< 图片格式：0-JPEG;1-PNG  */
    DWORD               dwTarget;                       /*!< 检测目标：0-人形,1-车形  */
    ImageInfo           cImageInfo;                     /*!< 图片信息   */

    char reserved[512];
}
#ifdef _WIN32
EventAiMotion, *LPEventAIMotion;
#pragma pack()
#endif
#ifdef __linux__
__attribute__((packed)) EventAiMotion, *LPEventAIMotion;
#endif

typedef struct tagEventAiPeople
{
	DWORD dwID;

	char szReserved[256];
}EventAiPeople, *LPEventAiPeople;

typedef struct tagEventAIPassengerFlowStatPassenger
{
	DWORD dwID;

	char szReserved[256];
}EventAIPassengerFlowStatPassenger, *LPEventAIPassengerFlowStatPassenger;

typedef struct tagEventAiMessagePushPeople
{
	DWORD dwID;

	char szReserved[256];
}EventAiMessagePushPeople, *LPEventAiMessagePushPeople;

typedef struct tagEventPIR
{
	DWORD dwID;

	char szReserved[256];
}EventPIR, *LPEventPIR;

typedef struct tagEventVoice
{
	DWORD dwID;

	char szReserved[256];
}EventVoice, *LPEventVoice;

typedef struct tagEventOVDChannelState
{
	DWORD dwID;

	int nChannelID;
	int nStatus;		//0：离线    1：在线    3：异常    4：不使能

	char szReserved[32];
}EventOVDChannelState,*LPEventOVDChannelState;

typedef struct tagEventAiOutOfBound
{
	DWORD dwID;

	char szReserved[64];
}EventAiOutOfBound,*LPEventAiOutOfBound;


/*!
*  @struct    tagEventP6SEventGroundLockStatusChanged
*  @brief    P6SEvent定时抓拍事件结构体
*  @illustrate  原事件对应的结构体不能满足P6SEvent功能需求,新增该结构体,在发送事件时将结构体指针通过事件回调接口的参数 dwParam3 传递,
*               实时事件参考HieServer_RefreshEventEx,重传事件参考HieServer_RefreshEventExV2
*/
#ifdef _WIN32
#pragma pack(1)
#endif
typedef struct tagEventP6SEventGroundLockStatusChanged
{
    DWORD               dwID;                           /*!< 事件ID：同一事件在多片段发送时使用相同ID标识   */
    DWORD               dwTimeSec;                      /*!< 绝对时间戳(单位:秒)    */
    DWORD               dwTimeUSec;                     /*!< 绝对时间戳(单位:微秒)   */
    DWORD               dwGroundLockStatus;             /*!< 地锁状态。0 - 落下； 1 - 升起 */
    DWORD               dwParkingSpaceNum;              /*!< 车位序号   */
    char                acParkingSpaceName[32];         /*!< 车位名称   */
    char                acNumberPlate[32];              /*!< 车牌号码   */
    DWORD               dwImageCodec;                   /*!< 图片格式：0-JPEG;1-PNG  */
    ImageInfo           cImageInfo;                     /*!< 图片信息   */
    DWORD               dwForce;                        /*!< 是否强制操作。0 - 否； 1 - 是 */

    char reserved[508];
}
#ifdef _WIN32
EventP6SEventGroundLockStatusChanged, *LPEventP6SEventGroundLockStatusChanged;
#pragma pack()
#endif
#ifdef __linux__
__attribute__((packed)) EventP6SEventGroundLockStatusChanged, *LPEventP6SEventGroundLockStatusChanged;
#endif


/*!
*  @struct    tagEventP6SEventTimedSnapshot
*  @brief    P6SEvent定时抓拍事件结构体
*  @illustrate  原事件对应的结构体不能满足P6SEvent功能需求,新增该结构体,在发送事件时将结构体指针通过事件回调接口的参数 dwParam3 传递,
*               实时事件参考HieServer_RefreshEventEx,重传事件参考HieServer_RefreshEventExV2
*/
#ifdef _WIN32
#pragma pack(1)
#endif
typedef struct tagEventP6SEventTimedSnapshot
{
    DWORD               dwID;                           /*!< 事件ID：同一事件在多片段发送时使用相同ID标识   */
    DWORD               dwTimeSec;                      /*!< 绝对时间戳(单位:秒)    */
    DWORD               dwTimeUSec;                     /*!< 绝对时间戳(单位:微秒)   */
    DWORD               dwImageCodec;                   /*!< 图片格式：0-JPEG;1-PNG  */
    ImageInfo           cImageInfo;                     /*!< 图片信息   */

    char reserved[512];
}
#ifdef _WIN32
EventP6SEventTimedSnapshot, *LPEventP6SEventTimedSnapshot;
#pragma pack()
#endif
#ifdef __linux__
__attribute__((packed)) EventP6SEventTimedSnapshot, *LPEventP6SEventTimedSnapshot;
#endif


/*!
*  @struct	tagCarPlateWhiteList
*  @brief	车牌白名单
*/
typedef struct tagCarPlateWhiteList
{
	char                acCarPlate[16];                 /*!< 车牌白名单， “44_AG12345”， 44:表示省级行政区域代码								*/
	char				cReserve[8];					/*!< 保留数据								*/
}CarPlateWhiteList, *LPCarPlateWhiteList;


/*!
*  @struct    tagEventP6SEventStartupReport
*  @brief    P6SEvent开机上报事件结构体
*  @illustrate  原事件对应的结构体不能满足P6SEvent功能需求,新增该结构体,在发送事件时将结构体指针通过事件回调接口的参数 dwParam3 传递,
*               实时事件参考HieServer_RefreshEventEx,重传事件参考HieServer_RefreshEventExV2
*/
#ifdef _WIN32
#pragma pack(1)
#endif
typedef struct tagEventP6SEventStartupReport
{
    DWORD               dwID;                           /*!< 事件ID：同一事件在多片段发送时使用相同ID标识   */
    DWORD               dwTimeSec;                      /*!< 绝对时间戳(单位:秒)    */
    DWORD               dwTimeUSec;                     /*!< 绝对时间戳(单位:微秒)   */
    char                acVersion[32];                  /*!< 固件版本号   */
    DWORD               dwCsq;                          /*!< 信号质量。 0 - 30   */
    char                acIP[32];                       /*!< IP 地址   */
    DWORD               dwHeartbeatInterval;            /*!< 心跳上报频率间隔（秒），默认 15s，范围：15-180   */
    DWORD               dwBearOffLiftLockDuration;      /*!< 停车离开后升锁时长（秒）,车辆驶离后，设定多少秒执行升锁，默认 20，范围0-60   */
    DWORD               dwLockEnabledDate0;             /*!< 地锁启用时间段(0 点-8点) 。地锁启用时间段，每半个小时为 1 个单元，每 8 小时为一组；启用：升起地锁，停用：降下地锁示例：65535 转 2 进制为1111111111111111，则表示0 点-8 点为全开启    */
    DWORD               dwLockEnabledDate1;             /*!< 同上   */
    DWORD               dwLockEnabledDate2;             /*!< 同上   */
    char                acDID[32];                      /*!< 设备 DID    */
    char                acTimeStamp[16];                /*!< 设备端时间戳毫秒    */
    CarPlateWhiteList*  pstWhiteList;                   /*!< 车牌白名单   */
    DWORD               dwWhiteNum;                     /*!< 车牌白名单数量   */
    DWORD               dwAuthMode;                     /*!< 鉴权模式: 0 - 自动模式（默认）（摄像头判断）;1 - 手动模式（除了本地的白名单车牌之外，每个都要向平台鉴权）; 2 - 特殊规则（除了本地的白名单车牌之外，只鉴权蓝牌）    */
    char                acMAC[32];                      /*!< MAC 地址。无分割符：112233AABBCC   */
    char                acICCID[64];                    /*!< ICCID   */
    int                 nBuzzer;                        /*!< 蜂鸣器（0关闭，1开启）   */
    int                 nTaskPhotosDuration;            /*!< 定时抓拍间隔（单位：秒，最小值：10）    */
    DWORD               dwImageCodec;                   /*!< 图片格式：0-JPEG;1-PNG  */
    ImageInfo           cImageInfo;                     /*!< 上报全屏照   */

    char reserved[504];
}
#ifdef _WIN32
EventP6SEventStartupReport, *LPEventP6SEventStartupReport;
#pragma pack()
#endif
#ifdef __linux__
__attribute__((packed)) EventP6SEventStartupReport, *LPEventP6SEventStartupReport;
#endif


/*!
*  @struct    tagEventP6SEventAbnormalEvent
*  @brief    P6SEvent异常事件结构体
*  @illustrate  原事件对应的结构体不能满足P6SEvent功能需求,新增该结构体,在发送事件时将结构体指针通过事件回调接口的参数 dwParam3 传递,
*               实时事件参考HieServer_RefreshEventEx,重传事件参考HieServer_RefreshEventExV2
*/
#ifdef _WIN32
#pragma pack(1)
#endif
typedef struct tagEventP6SEventAbnormalEvent
{
    DWORD               dwID;                           /*!< 事件ID：同一事件在多片段发送时使用相同ID标识   */
    DWORD               dwTimeSec;                      /*!< 绝对时间戳(单位:秒)    */
    DWORD               dwTimeUSec;                     /*!< 绝对时间戳(单位:微秒)   */
    DWORD               dwEventType;                    /*!< 事件类型。0 - 地锁异常， 1 - 设备异常    */
    char                acEventDescription[32];         /*!< 事件描述。“TF卡异常”，“超低电”，“低电”，“过流”，“扳动下压”，“扳动后推”，“动力故障”，“运动超时”    */

    char reserved[512];
}
#ifdef _WIN32
EventP6SEventAbnormalEvent, *LPEventP6SEventAbnormalEvent;
#pragma pack()
#endif
#ifdef __linux__
__attribute__((packed)) EventP6SEventAbnormalEvent, *LPEventP6SEventAbnormalEvent;
#endif



/*!
*  @struct    tagEventP6SEventNonWhitelistCarEvent
*  @brief    P6SEvent非白名单车辆事件结构体
*  @illustrate  原事件对应的结构体不能满足P6SEvent功能需求,新增该结构体,在发送事件时将结构体指针通过事件回调接口的参数 dwParam3 传递,
*               实时事件参考HieServer_RefreshEventEx,重传事件参考HieServer_RefreshEventExV2
*/
#ifdef _WIN32
#pragma pack(1)
#endif
typedef struct tagEventP6SEventNonWhitelistCarEvent
{
    DWORD               dwID;                           /*!< 事件ID：同一事件在多片段发送时使用相同ID标识   */
    DWORD               dwTimeSec;                      /*!< 绝对时间戳(单位:秒)    */
    DWORD               dwTimeUSec;                     /*!< 绝对时间戳(单位:微秒)   */
    char                acNumberPlate[32];              /*!< 车牌号码   */
    DWORD               dwImageCodec;                   /*!< 图片格式：0-JPEG;1-PNG  */
    ImageInfo           cImageInfo;                     /*!< 图片信息   */

    char reserved[512];
}
#ifdef _WIN32
EventP6SEventNonWhitelistCarEvent, *LPEventP6SEventNonWhitelistCarEvent;
#pragma pack()
#endif
#ifdef __linux__
__attribute__((packed)) EventP6SEventNonWhitelistCarEvent, *LPEventP6SEventNonWhitelistCarEvent;
#endif


/*!
*  @struct    tagEventP6SEventAuthRequest
*  @brief    P6SEvent非白名单车辆事件结构体
*  @illustrate  原事件对应的结构体不能满足P6SEvent功能需求,新增该结构体,在发送事件时将结构体指针通过事件回调接口的参数 dwParam3 传递,
*               实时事件参考HieServer_RefreshEventEx,重传事件参考HieServer_RefreshEventExV2
*/
#ifdef _WIN32
#pragma pack(1)
#endif
typedef struct tagEventP6SEventAuthRequest
{
    DWORD               dwID;                           /*!< 事件ID：同一事件在多片段发送时使用相同ID标识   */
    DWORD               dwTimeSec;                      /*!< 绝对时间戳(单位:秒)    */
    DWORD               dwTimeUSec;                     /*!< 绝对时间戳(单位:微秒)   */
    DWORD               dwParkingNum;                   /*!< 车位号  */
    char                acNumberPlate[32];              /*!< 车牌号码   */

    char reserved[512];
}
#ifdef _WIN32
EventP6SEventAuthRequest, *LPEventP6SEventAuthRequest;
#pragma pack()
#endif
#ifdef __linux__
__attribute__((packed)) EventP6SEventAuthRequest, *LPEventP6SEventAuthRequest;
#endif


/*!
*  @struct    tagEventP6SEventDeviceStatusReport
*  @brief    P6SEvent设备运行状态事件结构体
*  @illustrate  原事件对应的结构体不能满足P6SEvent功能需求,新增该结构体,在发送事件时将结构体指针通过事件回调接口的参数 dwParam3 传递,
*               实时事件参考HieServer_RefreshEventEx,重传事件参考HieServer_RefreshEventExV2
*/
#ifdef _WIN32
#pragma pack(1)
#endif
typedef struct tagEventP6SEventDeviceStatusReport
{
    DWORD               dwID;                           /*!< 事件ID：同一事件在多片段发送时使用相同ID标识   */
    DWORD               dwTimeSec;                      /*!< 绝对时间戳(单位:秒)    */
    DWORD               dwTimeUSec;                     /*!< 绝对时间戳(单位:微秒)   */
    DWORD               dwParkingNum;                   /*!< 车位号  */
    char                acNumberPlate[32];              /*!< 车牌号码   */
    DWORD               dwParkingStatus;                /*!< 停车状态。0–无车；1–有车   */
    DWORD               dwLockOnlineStatus;             /*!< 车位锁在线状态。0：未知；1：在线；2：离线   */
    DWORD               dwGroundLockStatus;             /*!< 车位锁状态。0 - 落下状态；1 - 升起状态 */
    char                acTransparentData[256];         /*!< 车位锁上报的透传数据（需要做 BASE64 编码）   */

    char reserved[512];
}
#ifdef _WIN32
EventP6SEventDeviceStatusReport, *LPEventP6SEventDeviceStatusReport;
#pragma pack()
#endif
#ifdef __linux__
__attribute__((packed)) EventP6SEventDeviceStatusReport, *LPEventP6SEventDeviceStatusReport;
#endif



/*!
*  @struct   tagEventP6SEventCarPlateRecognition
*  @brief    P6SEvent设备运行状态事件结构体
*  @illustrate  原事件对应的结构体不能满足P6SEvent功能需求,新增该结构体,在发送事件时将结构体指针通过事件回调接口的参数 dwParam3 传递,
*               实时事件参考HieServer_RefreshEventEx,重传事件参考HieServer_RefreshEventExV2
*/
#ifdef _WIN32
#pragma pack(1)
#endif
typedef struct tagEventP6SEventCarPlateRecognition
{
    DWORD               dwID;                           /*!< 事件ID：同一事件在多片段发送时使用相同ID标识   */
    DWORD               dwTimeSec;                      /*!< 绝对时间戳(单位:秒)    */
    DWORD               dwTimeUSec;                     /*!< 绝对时间戳(单位:微秒)   */
    DWORD               dwParkingNum;                   /*!< 车位号  */
    char                acNumberPlate[32];              /*!< 车牌号码   */
    PlateColor          dwCarLicenseAttriColor;         /*!< 车牌颜色  */
    float               fMatch;						    /*!< 车牌结果置信度  */
    VehicleColor        dwCarAttriColor;                /*!< 车辆颜色  */
    DWORD               dwCarDirection;                 /*!< 驶入方向（1-车辆驶入；2-车辆离开）  */
    DWORD               dwImageCodec;                   /*!< 图片格式：0-JPEG;1-PNG  */
    ImageInfo           cImageInfo;                     /*!< 图片信息   */	

    char reserved[512];
}
#ifdef _WIN32
EventP6SEventCarPlateRecognition, *LPEventP6SEventCarPlateRecognition;
#pragma pack()
#endif
#ifdef __linux__
__attribute__((packed)) EventP6SEventCarPlateRecognition, *LPEventP6SEventCarPlateRecognition;
#endif




/*!
*  @struct   tagEventP6SEventActiveSnapshot
*  @brief    P6SEvent设备运行状态事件结构体
*  @illustrate  原事件对应的结构体不能满足P6SEvent功能需求,新增该结构体,在发送事件时将结构体指针通过事件回调接口的参数 dwParam3 传递,
*               实时事件参考HieServer_RefreshEventEx,重传事件参考HieServer_RefreshEventExV2
*/
#ifdef _WIN32
#pragma pack(1)
#endif
typedef struct tagEventP6SEventActiveSnapshot
{
    DWORD               dwID;                           /*!< 事件ID：同一事件在多片段发送时使用相同ID标识   */
    DWORD               dwTimeSec;                      /*!< 绝对时间戳(单位:秒)    */
    DWORD               dwTimeUSec;                     /*!< 绝对时间戳(单位:微秒)   */
    DWORD               dwImageCodec;                   /*!< 图片格式：0-JPEG;1-PNG  */
    ImageInfo           cImageInfo;                     /*!< 图片信息   */	
    DWORD               dwParkingSpaceNum;               /*!< 车位号   */	

    char reserved[508];
}
#ifdef _WIN32
EventP6SEventActiveSnapshot, *LPEventP6SEventActiveSnapshot;
#pragma pack()
#endif
#ifdef __linux__
__attribute__((packed)) EventP6SEventActiveSnapshot, *LPEventP6SEventActiveSnapshot;
#endif



/*!
*  @struct    tagEventP6SEventElectronicDefence
*  @brief    P6SEvent电子围栏事件结构体
*  @illustrate  原事件对应的结构体不能满足P6SEvent功能需求,新增该结构体,在发送事件时将结构体指针通过事件回调接口的参数 dwParam3 传递,
*               实时事件参考HieServer_RefreshEventEx,重传事件参考HieServer_RefreshEventExV2
*/
#ifdef _WIN32
#pragma pack(1)
#endif
typedef struct tagEventP6SEventElectronicDefence
{
    DWORD               dwID;                           /*!< 事件ID：同一事件在多片段发送时使用相同ID标识   */
    DWORD               dwTimeSec;                      /*!< 绝对时间戳(单位:秒)    */
    DWORD               dwTimeUSec;                     /*!< 绝对时间戳(单位:微秒)   */
    DWORD               dwImageCodec;                   /*!< 图片格式：0-JPEG;1-PNG  */
    DWORD               dwTarget;                       /*!< 检测目标：0-人形,1-车形  */
    ImageInfo           cImageInfo;                     /*!< 图片信息   */

    char reserved[512];
}
#ifdef _WIN32
EventP6SEventElectronicDefence, *LPEventP6SEventElectronicDefence;
#pragma pack()
#endif
#ifdef __linux__
__attribute__((packed)) EventP6SEventElectronicDefence, *LPEventP6SEventElectronicDefence;
#endif


/*!
*  @struct    tagEventP6SEventCrossBorderDetect
*  @brief    P6SEvent越界侦测事件信息
*  @illustrate  原事件对应的结构体不能满足P6SEvent功能需求,新增该结构体,在发送事件时将结构体指针通过事件回调接口的参数 dwParam3 传递,
*               实时事件参数HieServer_RefreshEventEx,重传事件参考HieServer_RefreshEventExV2
*/
#ifdef _WIN32
#pragma pack(1)
#endif
typedef struct tagEventP6SEventCrossBorderDetect
{
    DWORD               dwID;                           /*!< 事件ID：同一事件在多片段发送时使用相同ID标识   */
    DWORD               dwTimeSec;                      /*!< 绝对时间戳(单位:秒)    */
    DWORD               dwTimeUSec;                     /*!< 绝对时间戳(单位:微秒)   */
    DWORD               dwImageCodec;                   /*!< 图片格式：0-JPEG;1-PNG  */
    DWORD               dwTarget;                       /*!< 检测目标：0-人形,1-车形  */
    ImageInfo           cImageInfo;                     /*!< 图片信息   */
	ObjectRect          cTargetRect;                    /*!< 目标坐标   */

    char reserved[504];
}
#ifdef _WIN32
EventP6SEventCrossBorderDetect, *LPEventP6SEventCrossBorderDetect;
#pragma pack()
#endif
#ifdef __linux__
__attribute__((packed)) EventP6SEventCrossBorderDetect, *LPEventP6SEventCrossBorderDetect;
#endif


/*!
*  @struct    tagEventP6SEventOffDutyDetect
*  @brief    P6SEvent离岗侦测事件信息
*  @illustrate  原事件对应的结构体不能满足P6SEvent功能需求,新增该结构体,在发送事件时将结构体指针通过事件回调接口的参数 dwParam3 传递,
*               实时事件参数HieServer_RefreshEventEx,重传事件参考HieServer_RefreshEventExV2
*/
#ifdef _WIN32
#pragma pack(1)
#endif
typedef struct tagEventP6SEventOffDutyDetect
{
    DWORD               dwID;                           /*!< 事件ID：同一事件在多片段发送时使用相同ID标识   */
    DWORD               dwTimeSec;                      /*!< 绝对时间戳(单位:秒)    */
    DWORD               dwTimeUSec;                     /*!< 绝对时间戳(单位:微秒)   */
    DWORD               dwImageCodec;                   /*!< 图片格式：0-JPEG;1-PNG  */
    DWORD				dwOffdutySustain;				/*!< 离岗持续时间,单位:秒  */
    ImageInfo           cImageInfo;                     /*!< 图片信息   */

    char reserved[512];
}
#ifdef _WIN32
EventP6SEventOffDutyDetect, *LPEventP6SEventOffDutyDetect;
#pragma pack()
#endif
#ifdef __linux__
__attribute__((packed)) EventP6SEventOffDutyDetect, *LPEventP6SEventOffDutyDetect;
#endif


#ifdef _WIN32
#pragma pack(1)
#endif
typedef struct tagEventCrossBorderDetect
{
	unsigned int		uDataPacketStartCode; //0xB7010000
    unsigned char		byFrameType;        //参考eFrameType类型
    unsigned long		u32RealTimeSec;		//时间戳秒
	int 				needAlarm;		//是否需要报警
}
#ifdef _WIN32
EventCrossBorderDetect,*LPEventCrossBorderDetect;
#pragma pack()
#endif
#ifdef __linux__
__attribute__((packed)) EventCrossBorderDetect,*LPEventCrossBorderDetect;
#endif

#ifdef _WIN32
#pragma pack(1)
#endif
typedef struct tagEventElectronicFence
{
	unsigned int		uDataPacketStartCode; //0xB7010000
    unsigned char		byFrameType;		//参考eFrameType类型
    unsigned long		u32RealTimeSec;		//时间戳秒
	int 				needAlarm;		//是否需要报警
}
#ifdef _WIN32
EventElectronicFence,*LPEventElectronicFence;
#pragma pack()
#endif
#ifdef __linux__
__attribute__((packed)) EventElectronicFence,*LPEventElectronicFence;
#endif

#ifdef _WIN32
#pragma pack(1)
#endif
typedef struct tagEventOffDutyDetect
{
	unsigned int		uDataPacketStartCode; //0xB7010000
    unsigned char		byFrameType;		//参考eFrameType类型
    unsigned long		u32RealTimeSec;		//时间戳秒
	int 				needAlarm;		//是否需要报警
}
#ifdef _WIN32
EventOffDutyDetect,*LPEventOffDutyDetect;
#pragma pack()
#endif
#ifdef __linux__
__attribute__((packed)) EventOffDutyDetect,*LPEventOffDutyDetect;
#endif

#ifdef _WIN32
#pragma pack(1)
#endif
typedef struct tagEventAIPassengerFlowStatPeople
{
    unsigned int        uDataPacketStartCode; //0xB7010000
    unsigned char        byFrameType;        //参考eFrameType类型
    unsigned long        u32RealTimeSec;        //时间戳秒
    int                 needAlarm;        //是否需要报警
}
#ifdef _WIN32
EventAIPassengerFlowStatPeople,*LPEventAIPassengerFlowStatPeople;
#pragma pack()
#endif
#ifdef __linux__
__attribute__((packed)) EventAIPassengerFlowStatPeople,*LPEventAIPassengerFlowStatPeople;
#endif

#ifdef _WIN32
#pragma pack(1)
#endif
typedef struct tagEventVideoShade
{
	unsigned int		uDataPacketStartCode; //0xB7010000
    unsigned char		byFrameType;		//参考eFrameType类型
    unsigned long		u32RealTimeSec;		//时间戳秒
	int 				needAlarm;		//是否需要报警
}
#ifdef _WIN32
EventVideoShade,*LPEventVideoShade;
#pragma pack()
#endif
#ifdef __linux__
__attribute__((packed)) EventVideoShade,*LPEventVideoShade;
#endif

#ifdef _WIN32
#pragma pack(1)
#endif
typedef struct tagEventIntelligenceTrack
{
	unsigned int		uDataPacketStartCode; //0xB7010000
    unsigned char		byFrameType;		//参考eFrameType类型
    unsigned long		u32RealTimeSec;		//时间戳秒
	int 				needAlarm;		//是否需要报警
}
#ifdef _WIN32
EventIntelligenceTrack,*LPEventIntelligenceTrack;
#pragma pack()
#endif
#ifdef __linux__
__attribute__((packed)) EventIntelligenceTrack,*LPEventIntelligenceTrack;
#endif

/*!
*  @struct    tagEventAICryScreamDetect
*  @brief    P6SEvent 哭声叫声检测事件结构体
*  @illustrate 
*/
#ifdef _WIN32
#pragma pack(1)
#endif
typedef struct tagEventAICryScreamDetect
{
    DWORD               dwID;                           /*!< 事件ID：同一事件在多片段发送时使用相同ID标识   */
    DWORD               dwTimeSec;                      /*!< 绝对时间戳(单位:秒)    */
    DWORD               dwTimeUSec;                     /*!< 绝对时间戳(单位:微秒)   */
    DWORD               dwImageCodec;                   /*!< 图片格式：0-JPEG;1-PNG  */
    ImageInfo           cImageInfo;                     /*!< 图片信息   */
 
    char reserved[512];
}
#ifdef _WIN32
EventAICryScreamDetect, *LPEventAICryScreamDetect;
#pragma pack()
#endif
#ifdef __linux__
__attribute__((packed)) EventAICryScreamDetect, *LPEventAICryScreamDetect;
#endif

/*!
*  @struct    tagEventTrafficStatistics
*  @brief    P6SEvent车流统计事件结构体
*  @illustrate 
*/
#ifdef _WIN32
#pragma pack(1)
#endif
typedef struct tagEventTrafficStatistics
{
    DWORD               dwID;                           /*!< 事件ID：同一事件在多片段发送时使用相同ID标识   */
    DWORD               dwTimeSec;                      /*!< 绝对时间戳(单位:秒)    */
    DWORD               dwTimeUSec;                     /*!< 绝对时间戳(单位:微秒)   */
    DWORD               dwCarInNum;                     /*!< 进入车数  */
    DWORD               dwCarOutNum;                    /*!< 离开车数  */
    char                acStartTime[32];               /*!< 开始时间,格式:YYYY-MM-DD HH:MM:SS  */
    char                acEndTime[32];                 /*!< 结束时间,格式:YYYY-MM-DD HH:MM:SS  */
    DWORD               dwImageCodec;                   /*!< 图片格式：0-JPEG;1-PNG  */
   ImageInfo           cImageInfo;                     /*!< 图片信息   */
 
    char reserved[512];
}
#ifdef _WIN32
EventTrafficStatistics, *LPEventTrafficStatistics;
#pragma pack()
#endif
#ifdef __linux__
__attribute__((packed)) EventTrafficStatistics, *LPEventTrafficStatistics;
#endif

/*!
*  @struct    tagEventTrafficStatisticsCarShape
*  @brief    P6SEvent车流统计车形事件结构体
*  @illustrate 
*/
#ifdef _WIN32
#pragma pack(1)
#endif
typedef struct tagEventTrafficStatisticsCarShape
{
    DWORD               dwID;                           /*!< 事件ID：同一事件在多片段发送时使用相同ID标识   */
    DWORD               dwTimeSec;                      /*!< 绝对时间戳(单位:秒)    */
    DWORD               dwTimeUSec;                     /*!< 绝对时间戳(单位:微秒)   */
    DWORD               dwNeedAlarm;                    /*!< 是否需要报警：0-否;1-是  */
    DWORD               dwImageCodec;                   /*!< 图片格式：0-JPEG;1-PNG  */
    ImageInfo           cImageInfo;                     /*!< 图片信息   */
 
    char reserved[512];
}
#ifdef _WIN32
EventTrafficStatisticsCarShape, *LPEventTrafficStatisticsCarShape;
#pragma pack()
#endif
#ifdef __linux__
__attribute__((packed)) EventTrafficStatisticsCarShape, *LPEventTrafficStatisticsCarShape;
#endif

typedef struct tagEventAiLeavePost
{
	DWORD dwID;

	char szReserved[64];
}EventAiLeavePost,*LPEventAiLeavePost;

typedef struct tagEventKeyToCall
{
	DWORD dwID;

    char callee[64];
    int call_type;      //0: 音频呼叫类型         1: 视频呼叫类型
}EventKeyToCall,*LPEventKeyToCall;

//查询人员信息
typedef struct tagHealthCodePersonData
{
	int iValid;

    float fTemperature;         //体温，摄氏度，北京测温版必传
    char acName[32];            //姓名(脱敏,证件号码查询方式必传)
    char acZjhm[64];            //证件号码(脱敏,证件号码查询方式必传)
    char acQRCode[2048];        //健康码码字(传码字就可以不用传姓名+证件号码)
    int iKeyType;               //通行方式 0:健康码code;1:身份证号码;99:其它
    char acMobile[16];          //手机号,缺省为空
    char *pcBase64String;       //人员通过时截屏的图片对应的base64字符串，刷身份证通过并人证比对一致时可传
    int iBase64StringLen;       //
	int iBase64StringType;       //

    char reserved[1024];
}HealthCodePersonData;
//设备信息
typedef struct tagHealthCodeDeviceInfo
{
	int iValid;
	
    char acSerialNo[32];          //机器序列号
    char acMacAddress[32];        //机器MAC地址
    char acSceneAccount[64];      //场所编码，支持终端侧可配置
    char acSceneCategory[8];      //场所一级分类，支持终端侧可配置，详见SZSB平台接入规范文档
    char acSceneType[8];          //场所二级分类，支持终端侧可配置，详见SZSB平台接入规范文档
    char acLongitude[16];          //经度，WGS84坐标系
    char acLatitude[16];          //纬度，WGS84坐标系
    char acMsi[32];              //机器sim信息
    char acSceneName[256];        //场所名称

    char reserved[1024];
}HealthCodeDeviceInfo;

//健康码状态信息
typedef struct tagHealthCodeStateData
{
    int iValid;

    bool bIsHaveHealthCode;        //是否有健康码
    int iState;                    //健康码等级：0，绿色；1，黄色；2，红色；5,异常,缺省为-1
    char acStateInfo[128];         //健康码描述,缺省为空,非绿码时，返回健康状态异常原因
    char acChangeTime[32];         //上次码色变更时间。时间格式yyyymmddhhmmss,缺省为空,平台返回
    char acExt[128];               //扩展描述,缺省为空

    char reserved[128];
}HealthCodeStateData;
//核酸信息
typedef struct tagHealthCodeNucleicData
{
    int iValid;

    int iNatTestState;                          //核酸状态:枚举值0-没有，1-有并且为阴性，2-有并且为阳性
    int iDateRange;                             //核酸状态查询范围，单位天，eg:0表示1天内，1表示2天内,-1表示没有时间范围限制,缺省为-2
    unsigned long long ullNatTestLastTime;      //最近一次核酸检测时间,单位时间戳，精确到毫秒,缺省为0,本地时间,本地时间
    unsigned long long ullNatSimpleLastTime;    //最近一次核酸采样时间,单位时间戳，精确到毫秒,缺省为0,本地时间
    char acTestPlace[128];                      //检测地点(可能为空)
    char acSimplePlace[128];                    //采样地点(可能为空)
    int ihsExpired;                             //核酸有效期,如:12、24、48、72. result(核酸状态)为0时有效(是否必须:否,缺省为-1)
    int iAntigenCode;                           //抗原状态,包括-1(阴性),0(无效结果),1(阳性) (是否必须:否,缺省为0xff)
    char acCreatedTime[32];                     //抗原上报时间,格式:yyyy-MM-dd HH:mm:ss ,本地时间(是否必须:否,缺省为空) 
	char acExt[128];               				//扩展描述,缺省为空

    char reserved[128];
}HealthCodeNucleicData;
//疫苗历史信息
typedef struct tagHealthCodeVaccineHistory
{
    int iValid;

    char acName[64];                           //疫苗名称,缺省为空
    char acFactoryName[64];                    //厂家名称,缺省为空
    char acCompanyName[64];                    //接种单位,缺省为空
    char acNumber[32];                         //接种批号,缺省为空
    int iTimes;                                //接种针次,缺省为-1,1:第一针;2:第二针;以此类推
    unsigned long long ullVaccineDate;         //接种时间，单位时间戳，精确到毫秒,缺省为0,本地时间

    char reserved[128];
}HealthCodeVaccineHistory;

//疫苗信息
typedef struct tagHealthCodeVaccineData
{
    int iValid;

    char acVaccineInfo[32];                            //疫苗接种信息描述（未授权；未接种；部分接种；完成接种；暂无数据）
    int iVaccineState;                                 //接种疫苗状态:枚举值0-未打疫苗，1-打了一针，2-打了两针
    int iHistoryListNum;                               //疫苗历史信息数,缺省为0
    HealthCodeVaccineHistory *pstHistoryList;          //疫苗历史信息 (协议层解析数据时会malloc,使用完后需free)
    char acYmLastCheckTime[32];                        //疫苗最近一次的检测时间,VaccineState非 0 时有效,缺省为空,本地时间

    char reserved[256];
}HealthCodeVaccineData;

//城市信息
typedef struct tagHealthCodeCityInfo
{
    int iValid;

    char acCityCode[32];                    //城市码,缺省为空
    char acCityName[32];                    //eg:湖北省武汉 或者湖北省荆门市* ,缺省为空

    char reserved[128];
}HealthCodeCityInfo;

//行程信息
typedef struct tagHealthCodeTravelData
{
    int iValid;


    int iRouteStatus;                            /*!< 行程卡状态，-1：X天内到过中高风险地区，0：无数据，1：X天没到过中高风险地区(是否必须:否,缺省为0xff) */
    int iAddressListNum;                         /*!< 经过的城市数,缺省为0 */
    HealthCodeCityInfo *pstAddressList;          /*!< 经过的城市 (协议层解析数据时会malloc,使用完后需free) */
    int iDateRange;                              /*!< 行程时间范围，单位天，eg:14表示返回的是14天内行程,缺省为-1 */
    char acExt[128];                             /*!< 对行程信息的扩展描述，eg：(注：*表示当前改城市存在中风险或高风险地区，并不表示用户实际到访过这些中高风险地区),缺省为空 */

    char reserved[256];
}HealthCodeTravelData;

//通行信息
typedef struct tagHealthCodePassResultData
{
    int iValid;

    bool bPassRecordResult;            //通行结果

    char reserved[256];
}HealthCodePassResultData;

typedef struct tagEventHealthCodeInfo
{
    DWORD                dwID;                               /*!< 事件ID：同一事件在多片段发送时使用相同ID标识 */
    DWORD                dwTimeSec;                          /*!< 绝对时间戳（单位：秒）                    */
    DWORD                dwTimeUSec;                         /*!< 绝对时间戳（单位：微秒）                */
    int                 iMsgSendType;                        /*!< 消息发送类型，0:实时;1:重传  */
    int                 iPassType;                           /*!< 通行方式，0:人脸;1:身份证;2:健康码  */
    char                acPlatform[32];                      /*!< 平台类型,HQP_MegEye,HQP_JKMHealthCode,HQP_HeNanHealthCode,HQP_AnHuiHealthCode,HQP_OneNetPlatform  */
    HealthCodePersonData stPersonData;                       //人员信息
    HealthCodeDeviceInfo stDeviceData;                       //设备信息
    HealthCodeStateData stStateData;                         //健康码状态信息
    HealthCodeNucleicData stNucleicData;                     //核酸信息
    HealthCodeVaccineData stVaccineData;                     //疫苗信息
    HealthCodeTravelData stTravelData;                       //行程信息
    HealthCodePassResultData stPassResultData;               //通行结果信息

    char                cReserve[1024];                      /*!< 保留字段 */
}EventHealthCodeInfo, *LPEventHealthCodeInfo;

//大匠智慧社区项目定制事件结构体
typedef struct tagEventDaJiangIOTUnlockRecordUpload
{
    DWORD                dwID;       		/*!< 事件ID：同一事件在多片段发送时使用相同ID标识*/
    DWORD                dwTimeSec;  		/*!< 绝对时间戳（单位：秒）  */
    DWORD                dwTimeUSec; 		/*!< 绝对时间戳（单位：微秒） */
    DWORD                dwRuleID;   		/*!< 规则ID：触发事件的源规则，抓拍事件时填充0xFFFFFFFF*/
    DWORD                dwImageCodec; 		/*!< 图片格式：0-JPEG  */
    DWORD      			 dwPicLength; 		/*!< 图片长度  */
    char                 acDeviceCd[64];  	/*!< 门禁设备编码 */
    char                 acFaceInfoID[64];	/*!< 人脸信息ID */
    int                  iUnlockType;     	/*!< 开锁类型：1-卡,2-手机,3-密码,4-刷脸。 */
    double               dSimilarity;     	/*!< 相似度：开锁类型为 4-刷脸时,不为空。 */
    int                  iIsUnlockSucc;   	/*!< 开锁是否成功：1成功，0失败。 */
    char                 acErrInfo[64];   	/*!< 开锁失败原因 */
    float                fBodyTemperature;	/*!< 体温温度  */
    char                 cTemperatureUnit;	/*!< 温度单位(0：摄氏度,1：华氏度) */

    char                cReserve[512]; 		/*!< 保留数据  */
}EventDaJiangIOTUnlockRecord, *LPEventDaJiangIOTUnlockRecord;

// 车牌抓拍事件背景图结构体
typedef struct tagCarLicenseSnapshotBackgroundInfo
{
    int         iHasBackground;            // 是否有背景图片信息;0-没有,1-有 
    ImageInfo   cImageInfo;                // 背景图片信息
    char        cReserve[128];             // 保留数据 
}CarLicenseSnapshotBackgroundInfo,*LPCarLicenseSnapshotBackgroundInfo;

// 车牌抓拍对比事件结构体
typedef struct tagCarLicenseMatchInfo
{
    char cCarLicenseLibname;                    // 车牌库名(0-白名单；1-黑名单)
    char  dwCarOwners[64];                      // 车主
    char  OwnersPhone[64];                      // 车主电话
    char cReserve[256];                          //保留
}CarLicenseMatchInfo,*LPCarLicenseMatchInfo;

// 车牌属性结构体
typedef struct tagCarLicenseAttributeInfo
{
    VehicleType dwAttriClass;             // 车辆类型
    VehicleColor dwCarAttriColor;         // 车辆颜色
    DWORD dwAttriOrientation;             // 车辆方向（0-下行（车头面对摄像头）；1-上行（车尾面对摄像头）；其他值无效）
    char cNumber[32];                     // 车牌号码
    PlateColor dwCarLicenseAttriColor;    // 车牌颜色
    char dwCarLicenseRowNum;              // 车牌单双排（1-单排；2-双排;其他值无效）
    ObjectRect dwCarLicenseRect;          // 车牌坐标
    ObjectRect dwCarRect;                 // 车型坐标
    char cDriveEntryDrection;             // 驶入方向（0-下行驶入；1-上行驶入；其他值无效）
	float fMatch;						  // 车牌结果置信度
	float fScore;						  // 车牌图评分
    char cReserve[64 - 4 - 4];                    //保留    
}CarLicenseAttributeInfo, *LPCarLicenseAttributeInfo;

// 车牌抓拍的车牌信息
typedef struct tagCarLicenseCaptureInfo
{
    CarLicenseAttributeInfo cAttriInfo;    // 车牌属性
    ImageInfo cCarLicenseImage;            // 抓拍图片信息
    unsigned long long nTrackID;          
    // 同一辆车一直在画面中连续抓的图trackid是同一个，表示是同一辆车，即trackid没断。如果车离开画面后再出现则重新分配id，即trackid断了。
    int	nTrackDie;
    // trackid即将不再连续，下一次事件的trackid将会与本次的trackid不同，本次推送为同一个trackid的最后一张图
    char szPicMd5sum[64];                  // 抓拍图片的md5值
    char cReserve[64];                     //保留
}CarLicenseCaptureInfo, *LPCarLicenseCaptureInfo;

// 车牌抓拍事件结构体 （抓拍图数据跟在结构体后）
typedef struct tagEventCarLicenseSnapshotRecognition
{
    DWORD dwID;               // 事件ID：同一事件在多片段发送时使用相同ID标识
    DWORD dwType;             // 类型:0-车牌抓拍事件：抓拍到的车牌号
                              // 1-车牌管理事件：跟车牌库比对的事件
    DWORD dwTimeSec;          // 绝对时间戳（单位秒）
    DWORD dwTimeUSec;         // 绝对时间戳（单位：微秒）  

    CarLicenseCaptureInfo cCaptureInfo;    // 抓拍图片车牌信息
    CarLicenseMatchInfo cMatchInfo;    // 匹配到的车辆信息
    char cHasCaptureInfo;  // 是否有抓拍图(0：无，1：有)
    char cHasMatchInfo;    // 是否有匹配信息(0：无，1：有)

    CarLicenseSnapshotBackgroundInfo stBackgroundInfo; // 背景图信息

    DWORD dwCarDrivingStatus;           // 车辆行驶状态。1 - 车辆驶入；2 - 车辆离开
	DWORD dwIsNoRecoCarLicenseFlag;	   // 不识别车牌标记，0---》势必车牌，1---》不识别车牌
    char cReserve[512 - 8];        //保留
} EventCarLicenseSnapshotRecognition, *LPEventCarLicenseSnapshotRecognition;


// 车牌布控事件结构体 （抓拍图数据跟在结构体后）
typedef struct tagEventLicensePlateSurveillance
{
    DWORD dwID;               // 事件ID：同一事件在多片段发送时使用相同ID标识
    DWORD dwType;             // 类型:0-车牌抓拍事件：抓拍到的车牌号
                              // 1-车牌管理事件：跟车牌库比对的事件
    DWORD dwTimeSec;          // 绝对时间戳（单位秒）
    DWORD dwTimeUSec;         // 绝对时间戳（单位：微秒）  

    CarLicenseCaptureInfo cCaptureInfo;    // 抓拍图片车牌信息
    CarLicenseMatchInfo cMatchInfo;    // 匹配到的车辆信息
    char cHasCaptureInfo;  // 是否有抓拍图(0：无，1：有)
    char cHasMatchInfo;    // 是否有匹配信息(0：无，1：有)

    CarLicenseSnapshotBackgroundInfo stBackgroundInfo; // 背景图信息

    DWORD dwCarDrivingStatus;           // 车辆行驶状态。1 - 车辆驶入；2 - 车辆离开
	DWORD dwIsNoRecoCarLicenseFlag;	   // 不识别车牌标记，0---》识别车牌，1---》不识别车牌
	DWORD dwLicensePlateNumStatus;	   // 0，陌生车牌，1.已注册车牌，2,非绿色车牌; 3,黄色车牌; 4,无车牌
    char cReserve[500];        //保留
} EventLicensePlateSurveillance, *LPEventLicensePlateSurveillance;


// 宠物抓拍图片信息
typedef struct tagPetSnapShotCaptureInfo
{
    ImageInfo cCarLicenseImage;             /*!< 抓拍图片信息 */

    ObjectRect  cFaceRect;                  /*!< 宠物位置：描述宠物图像区域位置信息      */
    unsigned long long nTrackID;            /*!< 同一个宠物一直在画面中连续抓的图trackid是同一个，表示是同一个宠物，即trackid没断。如果宠物离开画面后再出现则重新分配id，即trackid断了。                             */
    int nTrackDie;                          /*!< trackid即将不再连续，下一次事件的trackid将会与本次的trackid不同，本次推送为同一个trackid的最后一张图   */
    char szPicMd5sum[64];                   /*!< 抓拍图片的md5值 */

    char cReserve[256];                         /*!< 保留 */
}PetSnapShotCaptureInfo, *LPPetSnapShotCaptureInfo;


/*!
*  @struct  tagEventShutDownAlarm
*  @brief   关机告警事件(EVENT_AI_EX_SHUTDOWN_ALARM)
*/
typedef struct tagEventShutDownAlarm
{
    DWORD   dwID;               /*!< 事件ID：同一事件在多片段发送时使用相同ID标识   */
    DWORD   dwType;             /*!< 类型:1-关机告警事件 */
    DWORD   dwTimeSec;          /*!< 绝对时间戳（单位：秒）        */
    DWORD   dwTimeUSec;         /*!< 绝对时间戳（单位：微秒）   */
    int     iIsShutDown;        /*!< 是否关机:0-否,1-是 */

    char    cReserve[256]; /*!< 保留数据    */
}EventShutDownAlarm, *LPEventShutDownAlarm;


/*!
*  @struct  tagEventLowBatteryTenPercent
*  @brief   低电量百分之10告警事件(EVENT_AI_EX_LOW_BATTERY_TEN_PERSENT_ALARM)
*/
typedef struct tagEventLowBatteryTenPercent
{
    DWORD   dwID;               /*!< 事件ID：同一事件在多片段发送时使用相同ID标识   */
    DWORD   dwType;             /*!< 类型:1-低电量百分之10告警事件 */
    DWORD   dwTimeSec;          /*!< 绝对时间戳（单位：秒）        */
    DWORD   dwTimeUSec;         /*!< 绝对时间戳（单位：微秒）   */
    float   fBatteryPercent;    /*!< 电量百分比      */


    char    cReserve[256]; /*!< 保留数据    */
}EventLowBatteryTenPercent, *LPEventLowBatteryTenPercent;


/*!
*  @struct  tagEventLowBatteryTwentyPercent
*  @brief   低电量百分之20事件(EVENT_AI_EX_LOW_BATTERY_TWENTY_PERSENT_ALARM)
*/
typedef struct tagEventLowBatteryTwentyPercent
{
    DWORD   dwID;               /*!< 事件ID：同一事件在多片段发送时使用相同ID标识   */
    DWORD   dwType;             /*!< 类型:1-低电量百分之20告警事件 */
    DWORD   dwTimeSec;          /*!< 绝对时间戳（单位：秒）        */
    DWORD   dwTimeUSec;         /*!< 绝对时间戳（单位：微秒）   */
    float   fBatteryPercent;    /*!< 电量百分比      */

    char    cReserve[256]; /*!< 保留数据    */
}EventLowBatteryTwentyPercent, *LPEventLowBatteryTwentyPercent;


/*!
*  @struct  tagEventRechargeFail
*  @brief   回充失败事件(EVENT_AI_EX_RECHARGE_FAIL_ALARM)
*/
typedef struct tagEventRechargeFail
{
    DWORD   dwID;               /*!< 事件ID：同一事件在多片段发送时使用相同ID标识   */
    DWORD   dwType;             /*!< 类型:1-回充失败事件 */
    DWORD   dwTimeSec;          /*!< 绝对时间戳（单位：秒）        */
    DWORD   dwTimeUSec;         /*!< 绝对时间戳（单位：微秒）   */

    char    cReserve[256]; /*!< 保留数据    */
}EventRechargeFail, *LPEventRechargeFail;


/*!
*  @struct  tagEventAIExStrangeFace
*  @brief   陌生人脸事件
*/
/*  事件负载缓冲区内容视图
    格式分布：[EventAIExStrangeFace][Image0]
    EventAIExStrangeFace：表示陌生人脸事件主结构
    Image0：表示人脸抓拍图，人脸底库图以md5sum的方式给出，图片信息见cFaceImage
*/
#ifdef _WIN32
#pragma pack(1)
#endif
typedef struct tagEventAIExStrangeFace
{
    DWORD dwID; /*!< 事件ID：同一事件在多片段发送时使用相同ID标识*/
    DWORD dwTimeSec;/*!< 绝对时间戳（单位：秒）                    */
    DWORD dwTimeUSec;/*!< 绝对时间戳（单位：微秒）               */
    DWORD dwFaceNumber;/*!< 人脸数量：当前实时图像中检测到的人脸数量*/
    DWORD dwImageCodec;/*!< 图片格式：0-JPEG                        */
    FaceCaptureInfo cCaptureInfo;/*!< 抓拍图片信息：描述抓拍图片基本信息      */
    DWORD dwRuleID;/*!< 规则ID：触发事件的源规则，抓拍事件时填充0xFFFFFFFF*/
    float fFaceTemperature;/*!< 人脸温度                       */
    char cTemperatureUnit;/*!< 人脸温度(0：摄氏度 1：华氏度)          */
    char cIsNormalFaceTemperature;/*!< 温度是否正常(0：不支持    1：温度正常 2：温度异常)                     */

    char acReserve[512];/*!< 保留数据                               */
}
#ifdef _WIN32
EventAIExStrangeFace, *LPEventAIExStrangeFace;
#pragma pack()
#endif
#ifdef __linux__
__attribute__((packed)) EventAIExStrangeFace, *LPEventAIExStrangeFace;
#endif


/*!
*  @struct  tagEventAIExFaceComparison
*  @brief   人脸对比事件
*/
/*  事件负载缓冲区内容视图
    格式分布：[EventAIExFaceComparison]
    EventAIExFaceComparison：表示人脸对比事件主结构
*/
#ifdef _WIN32
#pragma pack(1)
#endif
typedef struct tagEventAIExFaceComparison
{
    DWORD dwID;                           /*!< 事件ID：同一事件在多片段发送时使用相同ID标识*/
    DWORD dwTimeSec;                      /*!< 绝对时间戳（单位：秒）                    */
    DWORD dwTimeUSec;                     /*!< 绝对时间戳（单位：微秒）               */
    DWORD dwImageCodec;                   /*!< 图片格式：0-JPEG                        */
    DWORD dwMatchNumber;                  /*!< 匹配数量：0-未匹配到目标，否则为匹配目标数量*/
    FaceMatchInfoV2 cMatchInfo;           /*!< 匹配图片信息：描述目标图片基本信息      */
    DWORD dwRuleID;                       /*!< 规则ID：触发事件的源规则，抓拍事件时填充0xFFFFFFFF*/
    float fFaceTemperature;               /*!< 人脸温度                       */
    char cTemperatureUnit;                /*!< 人脸温度(0：摄氏度 1：华氏度)          */
    char cIsNormalFaceTemperature;        /*!< 温度是否正常(0：不支持    1：温度正常 2：温度异常)                     */

    char acReserve[512];/*!< 保留数据                               */
}
#ifdef _WIN32
EventAIExFaceComparison, *LPEventAIExFaceComparison;
#pragma pack()
#endif
#ifdef __linux__
__attribute__((packed)) EventAIExFaceComparison, *LPEventAIExFaceComparison;
#endif

/*!
*  @struct    tagEventElectronicDefenceV2
*  @brief    电子围栏V2事件结构体
*  @illustrate  实时事件参考HieServer_RefreshEventEx,重传事件参考HieServer_RefreshEventExV2
*/
#ifdef _WIN32
#pragma pack(1)
#endif
typedef struct tagEventElectronicDefenceV2
{
    DWORD               dwID;                           /*!< 事件ID：同一事件在多片段发送时使用相同ID标识   */
    DWORD               dwTimeSec;                      /*!< 绝对时间戳(单位:秒)    */
    DWORD               dwTimeUSec;                     /*!< 绝对时间戳(单位:微秒)   */
    DWORD               dwTarget;                       /*!< 检测目标：0-人形，1-机动车, 2-非机动车  */
    DWORD               dwTargetStatus;                 /*!< 检测目标状态：0-进入,1-离开,2-滞留  */
    DWORD               dwImageCodec;                   /*!< 图片格式：0-JPEG;1-PNG  */
    ImageInfo           cImageInfo;                     /*!< 图片信息   */
	ObjectRect          cTargetRect;                    /*!< 目标坐标   */

    char reserved[120];
}
#ifdef _WIN32
EventElectronicDefenceV2, *LPEventElectronicDefenceV2;
#pragma pack()
#endif
#ifdef __linux__
__attribute__((packed)) EventElectronicDefenceV2, *LPEventElectronicDefenceV2;
#endif


#define PEOPLE_STATISTICS_AREA_MAX_COUNT 5
#define PEOPLE_STATISTICS_AREA_MAX_COUNT_RESERVE    10

/*!
*  @struct	tagPeopleNumberStatisticsAreaInfo
*  @brief	人数统计事件(周期性上报)区域信息
*/
typedef struct tagPeopleNumberStatisticsAreaInfo
{

	DWORD dwAreaNum;				/*!< 区域序号，取值1，2，3	*/
	DWORD dwPersonNumber;			/*!< 人员数量	*/

	char	cReserve[64];								/*!< 保留	*/
} PeopleNumberStatisticsAreaInfo, *LPPeopleNumberStatisticsAreaInfo;

/*!
 *  @struct    tagEventAIPeopleNumberStatistics
 *  @brief    P6SEvent人数统计事件(周期性上报)结构体
 *  @illustrate
 */
#ifdef _WIN32
#pragma pack(1)
#endif
typedef struct tagEventAIPeopleNumberStatistics
{
	DWORD dwID;			  /*!< 事件ID：同一事件在多片段发送时使用相同ID标识   */
	DWORD dwTimeSec;	  /*!< 绝对时间戳(单位:秒)    */
	DWORD dwTimeUSec;	  /*!< 绝对时间戳(单位:微秒)   */
	DWORD dwValidAreaCount;  /*!< 有效区域个数  */
	PeopleNumberStatisticsAreaInfo stAreaInfo[PEOPLE_STATISTICS_AREA_MAX_COUNT]; /*!< 区域信息   */
	PeopleNumberStatisticsAreaInfo stAreaInfoReserve[PEOPLE_STATISTICS_AREA_MAX_COUNT_RESERVE]; /*!< 保留区域信息   */
	char acStartTime[32]; /*!< 开始时间,格式:YYYYMMDDTHHMMSS+08,例：20040503T093008+08  */
	char acEndTime[32];	  /*!< 结束时间,格式:YYYYMMDDTHHMMSS+08,例：20040503T093008+08  */
	DWORD dwHasImage;	  /*!< 是否有图片：0-否;1-是  */
	DWORD dwImageCodec;	  /*!< 图片格式：0-JPEG;1-PNG  */
	ImageInfo cImageInfo; /*!< 图片信息   */

	char reserved[512];
}
#ifdef _WIN32
EventAIPeopleNumberStatistics, *LPEventAIPeopleNumberStatistics;
#pragma pack()
#endif
#ifdef __linux__
__attribute__((packed)) EventAIPeopleNumberStatistics, *LPEventAIPeopleNumberStatistics;
#endif


/*!
*  @struct	tagPeopleNumberOverLimitAreaInfo
*  @brief	人数超限报警事件区域信息
*/
typedef struct tagPeopleNumberOverLimitAreaInfo
{

	DWORD dwAreaNum;				/*!< 区域序号，取值1，2，3	*/
	DWORD dwPersonStatus;			/*!< 人员状态;0,人数超员，1.人数过少	*/

	char	cReserve[64];								/*!< 保留	*/
} PeopleNumberOverLimitAreaInfo, *LPPeopleNumberOverLimitAreaInfo;
/*!
 *  @struct    tagEventAIPeopleNumberOverLimit
 *  @brief    P6SEvent人数超限报警事件结构体
 *  @illustrate
 */
#ifdef _WIN32
#pragma pack(1)
#endif
typedef struct tagEventAIPeopleNumberOverLimit
{
	DWORD dwID;			  /*!< 事件ID：同一事件在多片段发送时使用相同ID标识   */
	DWORD dwTimeSec;	  /*!< 绝对时间戳(单位:秒)    */
	DWORD dwTimeUSec;	  /*!< 绝对时间戳(单位:微秒)   */
	DWORD dwValidAreaCount;  /*!< 有效区域个数  */
	PeopleNumberOverLimitAreaInfo stAreaInfo[PEOPLE_STATISTICS_AREA_MAX_COUNT]; /*!< 区域信息   */
	PeopleNumberOverLimitAreaInfo stAreaInfoReserve[PEOPLE_STATISTICS_AREA_MAX_COUNT_RESERVE]; /*!< 保留区域信息   */
	DWORD dwHasImage;	  /*!< 是否有图片：0-否;1-是  */
	DWORD dwImageCodec;	  /*!< 图片格式：0-JPEG;1-PNG  */
	ImageInfo cImageInfo; /*!< 图片信息   */

	char reserved[512];
}
#ifdef _WIN32
EventAIPeopleNumberOverLimit, *LPEventAIPeopleNumberOverLimit;
#pragma pack()
#endif
#ifdef __linux__
__attribute__((packed)) EventAIPeopleNumberOverLimit, *LPEventAIPeopleNumberOverLimit;
#endif
//////////////////////////////////////////////////////////////////////////
//接口
//////////////////////////////////////////////////////////////////////////
/////////////////////////////配置信息/////////////////////////////////////

/*
* @brief
*	获取设备配置信息回调接口
* @param dwUserID
*	用户ID,有效用户ID由1开始,0表示SDK操作配置信息
* @param dwMajorType
*        0: 设备注册信息
*		 1:
*        2:
*	配置信息主类型
*	高16位用于表示通道号
* @param dwMinorType
*	配置信息子类型
*	dwMajorType==0时, dwMinorType == 0:设备注册网络信息
*				                     1:设备信息
*
* @param pBuffer
*	配置信息缓冲区地址
* @param dwBufLen
*	配置信息缓冲区长度
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
typedef int(*IF_GetConfigure) (eProtocolID eProtocol, DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * dwBufLen);

/*
* @brief
*	设置设备配置信息回调接口
* @param dwUserID
*	用户ID,有效用户ID由1开始,0表示SDK操作配置信息
* @param dwMajorType
*	配置信息主类型
*	高16位用于表示通道号
* @param dwMinorType
*	配置信息子类型
* @param pBuffer
*	配置信息缓冲区地址
* @param dwBufLen
*	配置信息缓冲区长度
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
typedef int(*IF_SetConfigure) (DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, const BYTE * pBuffer, DWORD dwBufLen);

/*
* @brief
*	设置设备配置信息回调接口
* @param eProtocol
*	协议ID
* @param dwUserID
*	用户ID,有效用户ID由1开始,0表示SDK操作配置信息
* @param dwMajorType
*	配置信息主类型
*	高16位用于表示通道号
* @param dwMinorType
*	配置信息子类型
* @param pBuffer
*	配置信息缓冲区地址
* @param dwBufLen
*	配置信息缓冲区长度
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
typedef int(*IF_SetConfigureV1) (eProtocolID eProtocol, DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, const BYTE * pBuffer, DWORD dwBufLen);

/*
* @brief
*	协议对外提供的HTTP请求处理接口
* @param dwUserId
*	用户ID,有效用户ID由1开始,0表示SDK操作配置信息
* @param byHttpRequest
*	HTTP请求内容
* @param dwRequestLen
*	HTTP请求长度
* @param byHttpResponse
*	HTTP回应内容
* @param dwResponseLen
*	HTTP回应长度
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
typedef int(*IF_HttpRequest)(DWORD dwUserId, BYTE * byHttpRequest, DWORD dwRequestLen, BYTE * byHttpResponse, DWORD * dwResponseLen);
///////////////////////////更新事件通知///////////////////////////////////

/*
* @brief
*	更新事件通知启动接口
* @param dwEventType
*	事件类型
* @param pBuffer
*	参数信息
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
typedef int(*IF_RefreshEventStart) (DWORD dwEventType, const LPBuffer pBuffer);

/*
* @brief
*	更新事件通知停止接口
* @param dwEventType
*	事件类型
* @param pBuffer
*	参数信息
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
typedef int(*IF_RefreshEventStop) (DWORD dwEventType, const LPBuffer pBuffer);

/*
* @brief
*	更新事件通知接口
* @param dwEventType
*	事件类型
* @param pBuffer
*	参数信息
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 非阻塞式
*/
typedef int(*IF_RefreshEvent) (DWORD dwEventType, const LPBuffer pBuffer);

/////////////////////////////用户动作/////////////////////////////////////

/*
* @brief
*	用户登录回调接口
* @param dwUserID
*	用户ID
* @param pRemoteUserInfo
*	用户信息
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
typedef int(*IF_UserLogin) (eProtocolID eProtocol, DWORD * pUserID, const LPRemoteUserInfo pRemoteUserInfo);

/*
* @brief
*	用户注销回调接口
* @param dwUserID
*	用户ID
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
typedef int(*IF_UserLogout) (DWORD dwUserID);

/*
* @brief
*	用户登录时，由用户名获取密码接口
* @param pUserName
*	用户名
* @param pUserPass
*	用户密码
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
typedef int(*IF_UserGetPassByName) (eProtocolID eProtocol, const char *pUserName, char *pUserPass);

///////////////////////////////流捕获/////////////////////////////////////

/*
* @brief
*	流捕获启动接口
* @param eType
*	流类型
* @param dwChannel
*	通道号
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
typedef int(*IF_StreamStartCapture) (DWORD dwUserID, eMediaCodeType eType, DWORD dwChannel);

/*
* @brief
*	实时流捕获停止接口
* @param dwUserID
*	用户ID
* @param eType
*	流类型
* @param dwChannel
*	通道号
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
typedef int(*IF_StreamStopCapture) (DWORD dwUserId, eMediaCodeType eType, DWORD dwChannel);

/*
* @brief
*	流转发连接接口
* @param dwStream
*	流媒体ID
* @param dwUserID
*	用户ID
* @param dwChannel
*	解码通道
* @param cStreamPara
*	连接参数信息
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
typedef int(*IF_StreamTransferConnect) (DWORD *dwStream, DWORD dwUserId, DWORD dwChannel, const LPStreamTransferPara cStreamPara);

/*
* @brief
*	流转发接收接口
* @param dwStream
*	流媒体ID
* @param cFrame
*	帧信息
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 非阻塞
*/
typedef int(*IF_StreamTransferRecv) (DWORD dwStream, const LPStreamTransferFrame cFrame);

/*
* @brief
*	流转发断开接口
* @param dwChannel
*	流媒体ID
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
typedef int(*IF_StreamTransferDisconnect) (DWORD dwStream);

/////////////////////////////语音流读取///////////////////////////////////

/*
* @brief
*	语音流创建接口
* @param dwUserId
*	用户ID
* @param dwChannel
*	通道号
* @param eMode
*	语音编解码模式
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
typedef int(*IF_VoiceStreamCreate) (DWORD dwUserId, DWORD dwChannel, eVoiceMode eMode);

/*
* @brief
*	语音流销毁接口
* @param dwUserId
*	用户ID
* @param dwChannel
*	通道号
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
typedef int(*IF_VoiceStreamDestroy) (DWORD dwUserId, DWORD dwChannel);

/*
* @brief
*	语音流读取接口
* @param dwUserId
*	用户ID
* @param dwChannel
*	通道号
* @param pBuffer
*	语音流缓冲区
* @param dwBufLen
*	语音流长度
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 非阻塞
*/
typedef int(*IF_VoiceStreamRead) (DWORD dwUserId, DWORD dwChannel, const BYTE * pBuffer, DWORD dwBufLen);

/////////////////////////////历史查询/////////////////////////////////////
/*
* @brief
*	历史查询创建接口
* @param hIndex
*	历史查询句柄
* @param dwUserID
*	用户ID
* @param pQuery
*	历史查询参数
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
typedef int(*IF_HistoryQueryCreate) (HANDLE *hIndex, DWORD dwUserID, const LPHistoryQueryInfo pQuery);

/*
* @brief
*	历史查询创建接口
* @param hIndex
*	历史查询句柄
* @param dwUserID
*	用户ID
* @param pQuery
*	历史查询参数
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
typedef int(*IF_HistoryQueryCreateMultiType) (HANDLE * hIndex, DWORD dwUserID, const LPHistoryQueryInfoMultiType pQuery);

/*
* @brief
*	历史查询创建接口(以文件方式传输查询结果)
* @param hIndex
*	历史查询句柄
* @param dwUserID
*	用户ID
* @param pQuery
*	历史查询参数
* @param rsinfo
*	查询结果文件基本信息（用于客户端下载）
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
typedef int(*IF_HistoryQueryCreateMultiTypeRSFile) (HANDLE * hIndex, DWORD dwUserID, const LPHistoryQueryInfoMultiType pQuery, LPRecordSetFileInfo rsinfo);

/*
* @brief
*	历史查询销毁接口
* @param hIndex
*	历史查询句柄
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
typedef int(*IF_HistoryQueryDestroy) (HANDLE hIndex);

/*
* @brief
*	查询下一条历史接口
* @param hIndex
*	历史查询句柄
* @param pNext
*	下一条历史结果
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
typedef int(*IF_HistoryQueryNext) (HANDLE hIndex, LPHistoryQueryResult pNext);

/*
* @brief
*	查询下一条历史接口
* @param hIndex
*	历史查询句柄
* @param pNext
*	下一条历史结果
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
typedef int(*IF_HistoryQueryNextMultiType) (HANDLE hIndex, LPHistoryQueryResultMultiType pNext);

/////////////////////////////文件查询/////////////////////////////////////

/*
* @brief
*	文件查询创建接口
* @param hIndex
*	文件查询句柄
* @param dwUserID
*	用户ID
* @param pQuery
*	文件查询参数
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
typedef int(*IF_FileQueryCreate) (HANDLE * hIndex, DWORD dwUserID, const LPFileQueryInfo pQuery);

/*
* @brief
*	文件查询销毁接口
* @param hIndex
*	文件查询句柄
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
typedef int(*IF_FileQueryDestroy) (HANDLE hIndex);

/*
* @brief
*	查询下一条文件接口
* @param hIndex
*	文件查询句柄
* @param pNext
*	下一条文件结果
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
typedef int(*IF_FileQueryNext) (HANDLE hIndex, LPFileQueryResult pNext);

/////////////////////////////日志查询/////////////////////////////////////

/*
* @brief
*	日志查询创建接口
* @param hIndex
*	日志查询句柄
* @param dwUserID
*	用户ID
* @param pQuery
*	日志查询参数
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
typedef int(*IF_LogQueryCreate) (HANDLE * hIndex, DWORD dwUserID, const LPLogQueryInfo pQuery);

/*
* @brief
*	日志查询销毁接口
* @param hIndex
*	日志查询句柄
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
typedef int(*IF_LogQueryDestroy) (HANDLE hIndex);

/*
* @brief
*	查询下一条历史接口
* @param hIndex
*	日志查询句柄
* @param pNext
*	下一条历史结果
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
typedef int(*IF_LogQueryNext) (HANDLE hIndex, LPLogQueryResult pNext);

/*
* @brief
*	查询V1下一条历史接口
* @param hIndex
*	日志查询句柄
* @param pNext
*	下一条历史结果
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
typedef int(*IF_LogQueryNextV1) (HANDLE hIndex, LPLogQueryResultV1 pNext);

/////////////////////////////历史通道/////////////////////////////////////

/*
* @brief
*	历史通道创建接口
* @param hIndex
*	历史通道句柄
* @param dwUserID
*	用户ID
* @param pHistoryChannelParaInfo
*	历史通道参数信息
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
typedef int(*IF_HistoryChannelCreate) (HANDLE * hIndex, DWORD dwUserID, const LPHistoryChannelParaInfo pHistoryChannelParaInfo);

/*
* @brief
*	历史通道创建接口
* @param hIndex
*	历史通道句柄
* @param dwUserID
*	用户ID
* @param pHistoryChannelParaInfo
*	历史通道参数信息
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
typedef int(*IF_HistoryChannelCreateMultiType) (HANDLE * hIndex, DWORD dwUserID, const LPHistoryChannelParaInfoMultiType pHistoryChannelParaInfo);

/*
* @brief
*	历史通道创建接口
* @param hIndex
*	历史通道句柄
* @param dwUserID
*	用户ID
* @param pHistoryChannelParaInfo
*	历史通道参数信息
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
typedef int(*IF_HistoryChannelCreateMultiTypeV1) (HANDLE * hIndex, DWORD dwUserID, const LPHistoryChannelParaInfoMultiTypeV1 pHistoryChannelParaInfo);

/*
* @brief
*	历史通道创建接口，Srt拉流式，协议找中间层读取历史流帧数据
* @param hIndex
*	历史通道句柄
* @param dwUserID
*	用户ID
* @param dwChannel
*	历史通道参数信息
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
typedef int(*IF_HistoryChannelCreateSrtMode)(HANDLE *hIndex, DWORD dwUserID, const LPHistoryChannelParaInfoSrtMode pParaInfo);

/*
* @brief
*	历史通道读取历史流数据接口，Srt拉流式
* @param hIndex
*	历史通道句柄
* @param dwUserID
*	用户ID
* @param dwChannel
*	历史通道参数信息
* @return
*	返回0表示成功, 201, //读帧重试错误码
							202, //卡录像读取完毕
* @note
*	接口类型: 阻塞式
*/
typedef int(*IF_HistoryChannelReadFrame)(HANDLE hIndex, DWORD dwUserID, LPHistoryChannelFrameInfoSrtMode pFrameInfo);

/*
* @brief
*	历史通道销毁接口
* @param hIndex
*	历史通道句柄
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
typedef int(*IF_HistoryChannelDestroy) (HANDLE hIndex);

/*
* @brief
*	历史通道添加流通道号接口
* @param hIndex
*	历史通道句柄
* @param dwChannel
*	流通道号
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
typedef int(*IF_HistoryChannelInsert) (HANDLE hIndex, DWORD dwChannel);

/*
* @brief
*	历史通道移除流通道号接口
* @param hIndex
*	历史通道句柄
* @param dwChannel
*	流通道号
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
typedef int(*IF_HistoryChannelRemove) (HANDLE hIndex, DWORD dwChannel);

/*
* @brief
*	历史通道开始捕获接口
* @param hIndex
*	历史通道句柄
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
typedef int(*IF_HistoryChannelStartCapture) (HANDLE hIndex);

/*
* @brief
*	历史通道停止捕获接口
* @param hIndex
*	历史通道句柄
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
typedef int(*IF_HistoryChannelStopCapture) (HANDLE hIndex);

/*
* @brief
*	历史通道快放，仅传送I帧
* @param hIndex
*	历史通道句柄
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
typedef int(*IF_HistoryChannelSetSpeed) (HANDLE hIndex, DWORD dwSpeed);

/*
* @brief
*	历史通道定位接口
* @param hIndex
*	历史通道句柄
* @param pHistoryChannelPositionInfo
*	定位信息
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
typedef int(*IF_HistoryChannelPosition) (HANDLE hIndex, const LPHistoryChannelPositionInfo pHistoryChannelPositionInfo);

typedef int(*IF_HistoryChannelPositionV1) (HANDLE hIndex, const LPHistoryChannelPositionInfoV1 pHistoryChannelPositionInfo);

/*
* @brief
*	历史通道暂停接口
* @param hIndex
*	历史通道句柄
* @param pHistoryChannelPauseInfo
*	暂停信息
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
typedef int(*IF_HistoryChannelPause)(HANDLE hIndex, const LPHistoryChannelPauseInfo pHistoryChannelPauseInfo);

/*
* @brief
*	历史通道暂停接口
* @param hIndex
*	历史通道句柄
* @param pHistoryChannelPauseInfo
*	暂停信息
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
typedef int(*IF_HistoryChannelRecordFileInfo)(const LPHistoryChannelRecordFileParam pRecordFileParam, LPHistoryChannelRecordFileVideoFormat pVideoFormat, LPHistoryChannelRecordFileAudioFormat pAudioFormat);

/////////////////////////////透明通道/////////////////////////////////////

/*
* @brief
*	透明通道创建接口
* @param hIndex
*	透明通道句柄
* @param dwUserID
*	用户ID
* @param pTransparentParaInfo
*	透明通道启动参数
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
typedef int(*IF_TransparentChannelCreate) (HANDLE * hIndex, DWORD dwUserID, const LPTransparentParaInfo pTransparentParaInfo);

/*
* @brief
*	透明通道销毁接口
* @param hIndex
*	透明通道句柄
* @param pTransparentParaInfo
*	透明通道停止参数
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
typedef int(*IF_TransparentChannelDestroy) (HANDLE hIndex, const LPTransparentParaInfo pTransparentParaInfo);

/*
* @brief
*	透明通道数据读取接口
* @param hIndex
*	透明通道句柄
* @param pTransparentParaInfo
*	透明通道数据信息
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 非阻塞
*/
typedef int(*IF_TransparentChannelRead) (HANDLE hIndex, const LPTransparentParaInfo pTransparentParaInfo);

/////////////////////////////远程下载/////////////////////////////////////

/*
* @brief
*	远程下载创建接口
* @param hIndex
*	远程下载句柄
* @param dwUserID
*	用户ID
* @param pRemoteDownloadParaInfo
*	远程下载参数信息
* @param pFileName
*	存储完整文件名(含路径)
* @param dwFileNameLen
*	文件名最大长度
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
typedef int(*IF_RemoteDownloadCreate) (HANDLE * hIndex, DWORD dwUserID, const LPRemoteDownloadParaInfo pRemoteDownloadParaInfo);

/*
* @brief
*	远程下载创建接口
* @param hIndex
*	远程下载句柄
* @param pBuffer
*	数据缓冲区指针
* @return
*	返回0表示成功, 小于0表示出错,1表示已经读取完成
* @note
*	接口类型: 阻塞式
*/
typedef int(*IF_RemoteDownloadRead) (HANDLE hIndex, Buffer * pBuffer);


/*
* @brief
*	远程下载销毁接口
* @param hIndex
*	远程下载句柄
* @param pState
*	远程下载文件状态
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
typedef int(*IF_RemoteDownloadDestroy) (HANDLE hIndex, const LPRemoteDownloadState pState);


/////////////////////////////获取Buffer/////////////////////////////////////
typedef enum eGetBufferType
{
	eGetBufferTypeBegin,
	eGetBufferTypePicture,			/*缩略图*/
	eGetBufferTypeEnd
}eGetBufferType;



/*
* @brief
*	获取Buffer创建接口
* @param hIndex
*	获取Buffer句柄,回调函数要给出唯一的值
* @param dwUserID
*	用户ID
* @param etype
*	类型，用于指定想要获取哪一种类型的Buffer
* @param pRequest
*	创建参数
* @param pResponse
*
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
typedef int(*IF_RemoteGetBufferCreate) (HANDLE * hIndex, DWORD dwUserID, eGetBufferType etype, const char * pRequest, char * pResponse);

/*
* @brief
*	获取Buffer读取接口
* @param hIndex
*	获取Buffer句柄
* @param pBuffer
*	数据缓冲区指针
* @return
*	返回0表示成功, 小于0表示出错,1表示已经读取完成
* @note
*	接口类型: 阻塞式
*/
typedef int(*IF_RemoteGetBufferRead) (HANDLE hIndex, Buffer * pBuffer);


/*
* @brief
*	获取Buffer销毁接口
* @param hIndex
*	获取Buffer句柄
* @param pState
*	获取Buffer文件状态
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
typedef int(*IF_RemoteGetBufferDestroy) (HANDLE hIndex);


/////////////////////////////远程上传/////////////////////////////////////

/*
* @brief
*	远程上传创建接口
* @param hIndex
*	远程下载句柄
* @param dwUserID
*	用户ID
* @param pRemoteUploadParaInfo
*	远程上传参数信息
* @param pFileName
*	存储完整文件名(含路径)
* @param dwFileNameLen
*	文件名最大长度
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
typedef int(*IF_RemoteUploadCreate) (HANDLE * hIndex, DWORD dwUserID, const LPFileUploadParaInfo pFileUploadParaInfo);

/*
* @brief
*	远程下载创建接口
* @param hIndex
*	远程上传句柄
* @param pBuffer
*	数据缓冲区指针
* @return
*	返回0表示成功, 1表示写失败,2表示取消,3表示flash已经满,4表示文件版本不对
* @note
*	接口类型: 阻塞式
*/
typedef int(*IF_RemoteUploadWrite) (HANDLE hIndex, Buffer * pBuffer);


/*
* @brief
*	远程上传销毁接口
* @param hIndex
*	远程下载句柄
* @param pState
*	远程上传文件状态
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
typedef int(*IF_RemoteUploadDestroy) (HANDLE hIndex, const LPFileUploadState pState);

/*
* @brief
*	写log接口
* @param hIndex
*	句柄
* @param pBuffer
*	数据缓冲区指针
* @return
*	返回0表示成功, 1表示写失败
* @note
*	接口类型: 阻塞式
*/
typedef int(*IF_WriteLogFile) (int nLogLevel, char * strLogContent, int nLogLength);

///////////////////////////远程命令///////////////////////////////////

/*
* @brief
*	远程设备控制
* @param dwUserID
*	用户ID
* @param eOperationCode
*	操作码
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
typedef int(*IF_RemoteDeviceControl) (DWORD dwUserID, eRemoteDeviceControl eOperationCode);

/*
* @brief
*	远程设备控制
* @param dwUserID
*	用户ID
* @param eOperationCode
*	操作码
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
typedef int(*IF_RemoteDeviceControlV1) (DWORD dwUserID, eRemoteDeviceControl eOperationCode, char *pBuffer, DWORD nBufferSize);

/*
* @brief
*	远程强制I帧命令
* @param dwUserID
*	用户ID
* @param dwChannel
*	通道号
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
typedef int(*IF_RemoteForceIFrame) (DWORD dwUserID, DWORD dwChannel);

/*
* @brief
*	远程清除报警命令
* @param dwUserID
*	用户ID
* @param dwChannel
*	通道号
* @param dwAlarmType
*	报警类型
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
typedef int(*IF_RemoteAlarmClear) (DWORD dwUserID, DWORD dwChannel, DWORD dwAlarmType);

/*
* @brief
*	远程抓图命令
* @param dwUserID
*	用户ID
* @param dwChannel
*	通道号
* @param eFormat
*	图片格式
* @param pImageFileInfo
*	图片文件信息
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
typedef int(*IF_RemoteImageCapture) (DWORD dwUserID, DWORD dwChannel, eImageFormat eFormat, LPImageFileInfo pImageFileInfo);


/*
* @brief
*	远程锁定文件
* @param dwUserID
*	用户ID
* @param szFileName
*	文件名
* @param eOperationCode
*	操作码
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
typedef int(*IF_RemoteFileLock) (DWORD dwUserID, char *szFileName, eFileLockOperationCode eOperationCode);

/*
* @brief
*	远程录像控制
* @param dwUserID
*	用户ID
* @param dwChannel
*	通道号
* @param dwRecordType
*	录像类型
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
typedef int(*IF_RemoteRecordControl) (DWORD dwUserID, DWORD dwChannel, DWORD dwRecordType);

/*
* @brief
*	触发录像事件
* @param dwUserID
*	用户ID
* @param dwChannel
*	通道号
* @param dwRecordType
*	录像类型
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
typedef int(*IF_TriggerRecordEvent) (DWORD dwUserID, DWORD dwChannel, DWORD dwRecordType, VNS_USERDATA dwUserData);


typedef struct tagRemoteDesktopParam
{
	DWORD dwChannel;
	char reserve[1024];
} REMOTE_DESKTOP_PARAM, *LPREMOTE_DESKTOP_PARAM;

/*
* @brief
*	远程桌面连接请求
* @param dwUserID
*	用户ID
* @param dwChannel
*	通道号
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
typedef int(*IF_RemoteDesktopConnect) (DWORD dwUserID, LPREMOTE_DESKTOP_PARAM pStruRemoteDesktopParam);



typedef struct tagRemoteDesktopOperateParam
{
	DWORD dwOprateType;		//0,表示左键，1，表示中键，2表示右键
	DWORD dwAction;			//0,up,1,down,2,move,3,wheelup,4,wheeldown
	DWORD dwPosX;
	DWORD dwPosY;
	DWORD dwWheel;
	char reserve[1024];
} REMOTE_DESKTOP_OPERATE_PARAM, *LPREMOTE_DESKTOP_OPERATE_PARAM;
/*
* @brief
*	远程桌面操作请求
* @param dwUserID
*	用户ID
* @param dwChannel
*	通道号
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
typedef int(*IF_RemoteDesktopOperate) (DWORD dwUserID, LPREMOTE_DESKTOP_OPERATE_PARAM pStruRemoteDesktopOperateParam);

/*
* @brief
*	远程桌面断开连接请求
* @param dwUserID
*	用户ID
* @param dwChannel
*	通道号
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
typedef int(*IF_RemoteDesktopDisconnect) (DWORD dwUserID, LPREMOTE_DESKTOP_PARAM pStruRemoteDesktopParam);

/*
* @brief
*	远程报警输出控制
* @param dwUserID
*	用户ID
* @param dwChannel
*	通道号
* @param dwSwitch
*	开关状态
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
typedef int(*IF_RemoteAlarmoutControl) (DWORD dwUserID, DWORD dwChannel, DWORD dwSwitch);

/*
* @brief
*   远程报警输出控制(支持报警输出号)
* @param dwUserID
*   用户ID
* @param dwChannel
*   通道号
* @param dwStateBits
*   0-31位分别表示报警输入通道0-31的状态
* @return
*   返回0表示成功, 否则表示错误码
* @note
*   接口类型: 阻塞式
*/
typedef int(*IF_RemoteAlarmoutChannelControl) (DWORD dwUserID, DWORD dwChannel, DWORD dwStateBits);

/*
* @brief
*	远程报警输出状态获取
* @param dwUserID
*	用户ID
* @param dwChanNum
*	实际有效报警输入通道个数
* @param dwStateBits
*	0-31位分别表示报警输入通道0-31的状态
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
typedef int(*IF_RemoteAlarmoutStateGet) (DWORD dwUserID, DWORD* dwChanNum, DWORD* dwStateBits);

/*
* @brief
*	远程报警输入状态获取
* @param dwUserID
*	用户ID
* @param dwChanNum
*	实际有效报警输入通道个数
* @param dwStateBits
*	0-31位分别表示报警输入通道0-31的状态
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
typedef int(*IF_RemoteAlarminStateGet) (DWORD dwUserID, DWORD* dwChanNum, DWORD* dwStateBits);

/*
* @brief
*	云台控制命令
* @param dwUserID
*	用户ID
* @param dwChannel
*	通道号
* @param eCommandCode
*	命令字
* @param dwParam1-dwParam4
*	参数1-4
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
typedef int(*IF_PtzControl) (DWORD dwUserID, DWORD dwChannel, ePTZControlCmdCode eCommandCode, DWORD dwParam1, DWORD dwParam2, HIE_ULONG dwParam3,
	DWORD dwParam4);

/*
* @brief
*	Hxht云台控制命令
* @param dwUserID
*	用户ID
* @param dwChannel
*	通道号
* @param eCommandCode
*	命令字
* @param pBuffer
*	命令缓冲区
* @param dwBufLen
*	命令缓冲区长度
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
#ifndef USE_LOAD_C_COMPILER
typedef int(*IF_HxhtPtzControl) (DWORD dwUserID, DWORD dwChannel, ePTZControlCmdType eCommandType, BYTE *pBuffer, DWORD &dwBufLen);
#else
typedef int(*IF_HxhtPtzControl) (DWORD dwUserID, DWORD dwChannel, ePTZControlCmdType eCommandType, BYTE *pBuffer, DWORD * pdwBufLen);
#endif

/*
* @brief
*	Hxht能力查询接口
* @param dwUserId
*	用户Id
* @param dwChannel
*	通道号
* @param pQueryInfo
*	能力查询信息
* @param pResult
*	能力查询结果
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
typedef int(*IF_HxhtQuerySupport)(DWORD dwUserId, DWORD dwChannel, QuerySupportInfo *pQueryInfo, QuerySupportResult *pResult);

/*
* @brief
*	数据存在性检测命令
* @param dwUserID
*	用户ID
* @param dwChannel
*	通道号
* @param dwCommandId
*	命令字
* @param dwParam1-dwParam4
*	参数1-4
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
typedef int(*IF_DataExistCheck) (DWORD dwUserID, DWORD dwChannel, DWORD dwMajorType, DWORD dwMinorType, char *szYearMonth, DWORD * dwResult);


///////////////////////////应用事件处理///////////////////////////////////

/*
* @brief
*	应用层控制请求接口
* @param dwUserID
*	用户ID
* @param pRequest
*	控制请求信息
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
typedef int(*IF_AppControlRequest) (DWORD dwUserID, const LPTransparentParaInfo pRequest);

/*
* @brief
*	硬盘分组管理请求接口
* @param dwUserID
*	用户ID
* @param eOperationCode
*	操作码
* @param dwParam1-3
*	参数列表
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
typedef int(*IF_DiskGroupManage) (DWORD dwUserID, eDiskGroupOperation eOperationCode, DWORD dwParam1, DWORD dwParam2, DWORD dwParam3);

/*
* @brief
*	获取时间片段数据大小
* @param dwUserID
*	用户ID
* @param dwChannelBits
*	按位选定通道号
* @param eStreamType
*	录像类型
* @param szStartTime
*	开始时间
* @param szEndTime
*	结束时间
* @param dwDataSize
*	数据大小
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
typedef int(*IF_GetDataSize) (DWORD dwUserID, DWORD dwChannelBits, eDiskGroupType eDskGroupType, eHistoryStreamType eStreamType, char *szStartTime,
	char *szEndTime, VNS_USERDATA dwUserData, DWORD * dwDataSize);

/*
* @brief
*	获取时间片段数据大小
* @param dwUserID
*	用户ID
* @param dwChannelBits
*	按位选定通道号
* @param eStreamType
*	录像类型
* @param szStartTime
*	开始时间
* @param szEndTime
*	结束时间
* @param dwDataSize
*	数据大小
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
typedef int(*IF_GetDataSizeMultiType) (DWORD dwUserID, DWORD dwChannelBits, eDiskGroupType eDskGroupType, DWORD dwStreamType, char *szStartTime,
	char *szEndTime, VNS_USERDATA dwUserData, DWORD * dwDataSize);

/*
* @brief
*	系统升级通知
* @param dwUserID
*	用户ID
* @param cFTPParam
*	FTP服务器的升级文件参数
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
typedef int(*IF_SystemUpdateNotify) (DWORD dwUserID, SystemUpdateParam* pFTPParam);

/*
* @brief
*	分屏模式控制接口
* @param dwUserID
*	用户ID
* @param dwScreenDev
*	用于区分主显和辅显（0表示主显，1表示辅显）
* @param dwSplitNum
*	分屏数，支持1-16分屏
* @param pChanList
*	通道列表（pChanList表示一个DWORD的数组，包含16个DWORD成员。例如：dwSplitNum为4时，总共有4个分屏，
*	如果pChanList[0]为2，表示第一个分屏预览通道2的视频信号。）
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
typedef int(*IF_SetScreenSplitMode) (DWORD dwUserID, DWORD dwScreenDev, DWORD dwSplitNum, DWORD* pChanList);

/*
* @brief
*	远程恢复色度默认参数命令
* @param dwUserID
*	用户ID
* @param dwChannel
*	通道号
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
typedef int(*IF_RemoteSetChromaDefault) (DWORD dwUserID, DWORD dwChannel, DWORD dwTimeSection);

/*
* @brief
*	设置视频移动侦测布撤防时间计划
* @param dwUserID
*	用户ID
* @param pTimeScheme
*	布撤防时间
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
typedef int(*IF_SetMotionScheme) (DWORD dwUserID, TimeScheme *pTimeScheme);

/*
* @brief
*	设置报警输入布撤防时间计划
* @param dwUserID
*	用户ID
* @param pTimeScheme
*	布撤防时间
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
typedef int(*IF_SetAlarmINScheme) (DWORD dwUserID, TimeScheme *pTimeScheme);

/*
* @brief
*	获取视频移动侦测布撤防时间计划
* @param dwUserID
*	用户ID
* @param pTimeScheme
*	布撤防时间
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
typedef int(*IF_GetMotionScheme) (DWORD dwUserID, TimeScheme *pTimeScheme);

/*
* @brief
*	获取报警输入布撤防时间计划
* @param dwUserID
*	用户ID
* @param pTimeScheme
*	布撤防时间
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
typedef int(*IF_GetAlarmINScheme) (DWORD dwUserID, TimeScheme *pTimeScheme);

/*
* @brief
*	设置报警输出布撤防时间计划
* @param dwUserID
*	用户ID
* @param pTimeScheme
*	布撤防时间
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
typedef int(*IF_SetAlarmOUTScheme) (DWORD dwUserID, TimeScheme *pTimeScheme);

/*
* @brief
*	获取报警输出布撤防时间计划
* @param dwUserID
*	用户ID
* @param pTimeScheme
*	布撤防时间
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
typedef int(*IF_GetAlarmOUTScheme) (DWORD dwUserID, TimeScheme *pTimeScheme);

/*
* @brief
*	获取面板状态
* @param dwUserID
*	用户ID
* @param cStatus
*	面板状态
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
typedef int(*IF_GetPanelStatus) (DWORD dwUserID, LPPanelStatusInfo cStatus);

/*
* @brief
*	设置面板状态
* @param dwUserID
*	用户ID
* @param cStatus
*	面板状态
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
typedef int(*IF_SetPanelStatus) (DWORD dwUserID, LPPanelStatusInfo cStatus);

/*
* @brief
*	面板控制
* @param dwUserID
*	用户ID
* @param cControl
*	面板控制参数
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
typedef int(*IF_PanelControl) (DWORD dwUserID, LPPanelControlParameter cControl);

/*!
* @brief
*	动态主动流媒体连接
* @param dwUserID
*	用户句柄,通过用户登录获取的句柄
* @param dwChannel
*	解码通道号
* @param cStreamPara
*	连接参数信息
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*	动态的媒体连接不会保存,设备重启后该流媒体不会自动连接
*/
// typedef int (*IF_DecoderDynamicConnect)(IN DWORD dwUserID, IN DWORD dwChannel, IN const LPDecoderDynamicInformation cStreamPara);

/*!
* @brief
*	动态主动流媒体断开
* @param dwUserID
*	用户句柄,通过用户登录获取的句柄
* @param dwChannel
*	解码通道号
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
// typedef int (*IF_DecoderDynamicDisconnect)(IN DWORD dwUserID, IN DWORD dwChannel);

/*!
* @brief
*	设置解码通道轮转配置
* @param dwUserID
*	用户句柄,通过用户登录获取的句柄
* @param dwChannel
*	解码通道号
* @param cConfig
*	配置
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
// typedef int (*IF_DecoderSetChannelLoopConfig)(IN DWORD dwUserID, IN DWORD dwChannel, IN const LPDecoderChannelLoopConfig cConfig);

/*!
* @brief
*	获取解码通道轮转配置
* @param dwUserID
*	用户句柄,通过用户登录获取的句柄
* @param dwChannel
*	解码通道号
* @param cConfig
*	配置
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
// typedef int (*IF_DecoderGetChannelLoopConfig)(IN DWORD dwUserID, IN DWORD dwChannel, IN LPDecoderChannelLoopConfig cConfig);

/*!
* @brief
*	设置解码通道轮询状态
* @param dwUserID
*	用户句柄,通过用户登录获取的句柄
* @param dwChannel
*	解码通道号
* @param eStatus
*	状态
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
// typedef int (*IF_DecoderSetChannelLoopStatus)(IN DWORD dwUserID, IN DWORD dwChannel, IN eDecoderChannelStatus eStatus);

/*!
* @brief
*	获取解码通道轮询状态
* @param dwUserID
*	用户句柄,通过用户登录获取的句柄
* @param dwChannel
*	解码通道号
* @param eStatus
*	状态
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
// typedef int (*IF_DecoderGetChannelLoopStatus)(IN DWORD dwUserID, IN DWORD dwChannel, IN eDecoderChannelStatus *eStatus);

/*!
* @brief
*	获取解码通道轮询状态
* @param hUser
*	用户句柄,通过用户登录获取的句柄
* @param cStatus
*	状态
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
// typedef int (*IF_DecoderGetLoopStatus)(IN DWORD dwUserID, IN LPDecoderLoopStatus cStatus);

/*!
* @brief
*	获取解码器通道状态
* @param dwUserID
*	用户句柄,通过用户登录获取的句柄
* @param cStatus
*	状态
* @return
*	返回表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
// typedef int (*IF_DecoderChannelGetStatus)(IN DWORD dwUserID, IN LPDecoderChannelStatus cStatus);

/*!
* @brief
*	获取解码器通道播放状态
* @param dwUserID
*	用户句柄,通过用户登录获取的句柄
* @param cControl
*	控制参数
* @return
*	返回表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
// typedef int (*IF_DecoderChannelGetPlayStatus)(IN DWORD dwUserID, IN LPDecoderGetPlayStatusParameter cControl);

/*!
* @brief
*	设置解码器通道播放状态
* @param dwUserID
*	用户句柄,通过用户登录获取的句柄
* @param dwChannel
*	解码通道号
* @param cControl
*	控制参数
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
// typedef int (*IF_DecoderChannelSetPlayStatus)(IN DWORD dwUserID, IN DWORD dwChannel, IN LPDecoderSetPlayStatusParameter cControl);

/*!
* @brief
*	控制解码通道显示状态
* @param dwUserID
*	用户句柄,通过用户登录获取的句柄
* @param dwDisplayChannel
*	显示通道号
* @param cControl
*	控制参数
* @return
*	返回表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
// typedef int (*IF_DecoderChannelDisplayControl)(IN DWORD dwUserID, IN DWORD dwDisplayChannel, IN LPDecoderDisplayStatus cControl);

/*!
* @brief
*	设置解码配置信息
* @param dwUserID
*	用户句柄,通过用户登录获取的句柄
* @param cConfig
*	配置信息
* @return
*	返回表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
// typedef int (*IF_DecoderSetConfig)(IN DWORD dwUserID, IN const LPDecoderDeviceConfig cConfig);

/*!
* @brief
*	获取解码配置信息
* @param dwUserID
*	用户句柄,通过用户登录获取的句柄
* @param dwUserID
*	配置信息
* @return
*	返回表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
// typedef int (*IF_DecoderGetConfig)(IN DWORD dwUserID, IN LPDecoderDeviceConfig cConfig);

/*!
* @brief
*	获取解码通道信息
* @param dwUserID
*	用户句柄,通过用户登录获取的句柄
* @param dwChannel
*	解码通道号
* @param cInformation
*	状态
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
// typedef int (*IF_DecoderGetChannelInformation)(IN DWORD dwUserID, IN DWORD dwChannel, IN const LPDecoderChannelInformation cInformation);

/*!
* @brief
*	设置显示配置
* @param dwUserID
*	用户句柄,通过用户登录获取的句柄
* @param dwDisplay
*	显示设备号
* @param cConfig
*	参数
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
// typedef int (*IF_DecoderSetDisplayConfig)(IN DWORD dwUserID, IN DWORD dwChannel, IN const LPDecoderDisplayConfig cInformation);

/*!
* @brief
*	获取显示配置
* @param dwUserID
*	用户句柄,通过用户登录获取的句柄
* @param dwDisplay
*	显示设备号
* @param cConfig
*	参数
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
// typedef int (*IF_DecoderGetDisplayConfig)(IN DWORD dwUserID, IN DWORD dwDisplay, IN LPDecoderDisplayConfig cConfig);

/*!
* @brief
*	获取解码通道配置信息
* @param dwUserID
*	用户句柄,通过用户登录获取的句柄
* @param dwChannel
*	解码通道号
* @param cConfig
*	配置
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
// typedef int (*IF_DecoderGetChannelConfig)(IN DWORD dwUserID, IN DWORD dwChannel, IN LPDecoderChannelConfig cConfig);

/*!
* @brief
*	设置解码通道配置信息
* @param dwUserID
*	用户句柄,通过用户登录获取的句柄
* @param dwChannel
*	解码通道号
* @param cConfig
*	配置
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
// typedef int (*IF_DecoderSetChannelConfig)(IN DWORD dwUserID, IN DWORD dwChannel, IN const LPDecoderChannelConfig cConfig);

/*!
* @brief
*	获取解码通道视频制式
* @param dwUserID
*	用户句柄,通过用户登录获取的句柄
* @param dwChannel
*	解码通道号
* @param cConfig
*	配置
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
// typedef int (*IF_DecoderGetVideoStandard)(IN DWORD dwUserID, IN DWORD dwChannel, IN LPDecoderVideoStandardConfig cConfig);

/*!
* @brief
*	设置解码通道视频制式
* @param dwUserID
*	用户句柄,通过用户登录获取的句柄
* @param dwChannel
*	解码通道号
* @param cConfig
*	配置
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
// typedef int (*IF_DecoderSetVideoStandard)(IN DWORD dwUserID, IN DWORD dwChannel, IN const LPDecoderVideoStandardConfig cConfig);

/*!
* @brief
*	上传LOGO图片
* @param dwUserID
*	用户句柄,通过用户登录获取的句柄
* @param dwChannel
*	通道号
* @param cParameter
*	LOGO图片参数
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
// typedef int (*IF_UploadLogo)(IN DWORD dwUserID, IN DWORD dwChannel, IN LPUploadLogoParameter cParameter);

/*!
* @brief
*	切换解码通道LOGO图片
* @param dwUserID
*	用户句柄,通过用户登录获取的句柄
* @param dwChannel
*	通道号
* @param eOperationCode
*	操作码　
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
// typedef int (*IF_DecoderChannelLogoSwitch)(IN DWORD dwUserID, IN DWORD dwChannel, IN eDeviceChannelControl eOperationCode);

/*!
* @brief
*	转发历史流查询连接操作（支持多种录像类型的组合）
* @param hStreamQuery
*	历史流查询句柄,连接后接口填充该句柄值
* @param dwUserID
*	用户句柄,通过用户登录获取的句柄
* @param cStreamQueryFactor
*	历史流查询条件
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
// typedef int (*IF_TransmitHistoryStreamQueryConnect)(OUT DWORD *hStreamQuery, IN DWORD dwUserID, IN LPTransmitHistoryStreamQueryFactor cStreamQueryFactor);

/*!
* @brief
*	转发历史流查询断开操作
* @param hStreamQuery
*	历史流查询句柄,通过连接历史流查询获取
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
// typedef int (*IF_TransmitHistoryStreamQueryDisconnect)(IN DWORD hStreamQuery);

/*!
* @brief
*	转发历史流查询下一条（仅支持单一录像类型和所有录像类型）
* @param hStreamQuery
*	历史流查询句柄,通过连接历史流查询获取
* @param pStreamQueryResult
*	历史流查询的一条记录
* @return
*	返回0表示成功, 2为查询忙,3为查询已经完成,其它值为错误码
* @note
*	接口类型: 阻塞式
*/
// typedef int (*IF_TransmitHistoryStreamQueryNext)(IN DWORD hStreamQuery, OUT LPHistoryStreamMultiTypeQueryResult pStreamQueryResult);
/*
* @brief
*    实时流连接添加请求接口
* @param eProtocol
*    协议类型
* @param dwUserID
*    用户ID
* @param dwChannel
*    通道，从0开始
* @param eType
*    码流类型
* @param vReserve
*    保留字段指针
* @param pdwConnectionId
*    输出参数，连接ID
* @return
*    返回0表示成功, 否则表示错误码
* @note
*    接口类型: 阻塞式
*/
typedef int (*IF_RealStreamConnectionAdd) (eProtocolID eProtocol,DWORD dwUserID, DWORD dwChannel, eMediaCodeType eType, void * vReserve,HIE_ULONG *pdwConnectionId);

/*
* @brief
*    实时流连接删除请求接口
* @param dwConnectionId
*    输入参数，连接ID
* @param dwUserID
*    用户ID
* @param vReserve
*    保留字段指针
* @return
*    返回0表示成功, 否则表示错误码
* @note
*    接口类型: 阻塞式
*/
typedef int (*IF_RealStreamConnectionDelete) (HIE_ULONG dwConnectionId, void * vReserve);

/*
* @brief
*    实时流连接获取数据请求接口
* @param dwConnectionId
*    输入参数，连接ID
* @param pRealStreamMedia
*    实时流数据信息
* @param vReserve
*    保留字段指针
* @return
*    返回0表示成功, 否则表示错误码
* @note
*    接口类型: 阻塞式
*/
typedef int (*IF_RealStreamConnectionGetData) (HIE_ULONG dwConnectionId, LPRealStreamMedia pRealStreamMedia,void * vReserve);

/*
* @brief
*    实时流连接释放数据请求接口
* @param dwConnectionId
*    输入参数，连接ID
* @param pRealStreamMedia
*    实时流数据信息
* @param vReserve
*    保留字段指针
* @return
*    返回0表示成功, 否则表示错误码
* @note
*    接口类型: 阻塞式
*/
typedef int (*IF_RealStreamConnectionReleaseData) (HIE_ULONG dwConnectionId, LPRealStreamMedia pRealStreamMedia,void * vReserve);



/////////////////////////////视频通话流读取///////////////////////////////////

/*
* @brief
*	视频通话流创建接口
* @param dwUserId
*	用户ID
* @param dwChannel
*	通道号
* @param eMode
*	语音编解码模式
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
typedef int(*IF_VideoStreamCreate) (DWORD dwUserId, DWORD dwChannel, LPVideoStreamMedia VideoStreamMedia);

/*
* @brief
*	视频通话流销毁接口
* @param dwUserId
*	用户ID
* @param dwChannel
*	通道号
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
typedef int(*IF_VideoStreamDestroy) (DWORD dwUserId, DWORD dwChannel);

/*
* @brief
*	视频通话流读取接口
* @param dwUserId
*	用户ID
* @param dwChannel
*	通道号
* @param pBuffer
*	语音流缓冲区
* @param dwBufLen
*	语音流长度
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 非阻塞
*/
typedef int(*IF_VideoStreamRead) (DWORD dwUserId, DWORD dwChannel, const BYTE * pBuffer, DWORD dwBufLen);


/*
* @brief
*	视频通话控制接口
* @param dwUserId
*	用户ID
* @param dwChannel
*	通道号
* @param pBuffer
*	语音流缓冲区
* @param dwBufLen
*	语音流长度
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 非阻塞
*/
typedef int(*IF_VideoStreamControl) (DWORD dwUserId, DWORD dwChannel, LPVideoCallMediaControl VideoCallMediaControl);



///////////////////////////接口结构信息///////////////////////////////////

/*!
*  @struct	tagConfigureInterface
*  @brief	配置操作接口信息
*/
typedef struct tagConfigureInterface
{
	IF_GetConfigure ifGetConfigure;	/*!< 获取配置信息                        */
	IF_SetConfigure ifSetConfigure;	/*!< 设置配置信息                        */
} ConfigureInterface, *LPConfigureInterface;

/*!
*  @struct	tagConfigureInterfaceV1
*  @brief	配置操作接口信息
*/
typedef struct tagConfigureInterfaceV1
{
	IF_GetConfigure ifGetConfigure;		/*!< 获取配置信息                        */
	IF_SetConfigureV1 ifSetConfigure;	/*!< 设置配置信息                        */
} ConfigureInterfaceV1, *LPConfigureInterfaceV1;

/*!
*  @struct	tagRefreshEventInterface
*  @brief	更新事件操作接口
*/
typedef struct tagRefreshEventInterface
{
	IF_RefreshEventStart ifRefreshEventStart;	/*!< 更新事件通知启动接口           */
	IF_RefreshEventStop ifRefreshEventStop;	/*!< 更新事件通知停止接口            */
	IF_RefreshEvent ifRefreshEvent;	/*!< 更新事件通知接口                   */
} RefreshEventInterface, *LPRefreshEventInterface;

/*!
*  @struct	tagUserActionInterface
*  @brief	用户动作接口信息
*/
typedef struct tagUserActionInterface
{
	IF_UserLogin ifUserLogin;	/*!< 用户登录接口                       */
	IF_UserLogout ifUserLogout;	/*!< 用户注销接口                       */
	IF_UserGetPassByName ifUserGetPassByName;	/*!< 用户登录时，由用户名获取密码       */
} UserActionInterface, *LPUserActionInterface;

/*!
*  @struct	tagStreamCaptureInterface
*  @brief	流捕获控制接口信息
*/
typedef struct tagStreamCaptureInterface
{
	IF_StreamStartCapture ifStreamStartCapture;	/*!< 启动流捕获接口                    */
	IF_StreamStopCapture ifStreamStopCapture;	/*!< 停止流捕获接口                     */
} StreamCaptureInterface, *LPStreamCaptureInterface;

/*
* @brief
*	流捕获启动接口
* @param eType
*	流类型
* @param dwChannel
*	通道号
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
typedef int(*IF_StreamStartCaptureV2) (eProtocolID dwProtocolID, DWORD dwUserID, eMediaCodeType eType, DWORD dwChannel);

/*
* @brief
*	实时流捕获停止接口
* @param dwUserID
*	用户ID
* @param eType
*	流类型
* @param dwChannel
*	通道号
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
typedef int(*IF_StreamStopCaptureV2) (eProtocolID dwProtocolID, DWORD dwUserId, eMediaCodeType eType, DWORD dwChannel);

/*!
*  @struct	tagStreamCaptureInterfaceV2
*  @brief	流捕获控制接口信息
*/
typedef struct tagStreamCaptureInterfaceV2
{
	IF_StreamStartCaptureV2 ifStreamStartCaptureV2;	/*!< 启动流捕获接口                    */
	IF_StreamStopCaptureV2 ifStreamStopCaptureV2;	/*!< 停止流捕获接口                     */
} StreamCaptureInterfaceV2, *LPStreamCaptureInterfaceV2;

/*!
*  @struct	tagVoiceStreamInterface
*  @brief	语音流接口信息
*/
typedef struct tagVoiceStreamInterface
{
	IF_VoiceStreamCreate ifVoiceStreamCreate;	/*!< 语音流创建接口                 */
	IF_VoiceStreamDestroy ifVoiceStreamDestroy;	/*!< 语音流销毁接口                */
	IF_VoiceStreamRead ifVoiceStreamRead;	/*!< 语音流读取接口                   */
} VoiceStreamInterface, *LPVoiceStreamInterface;

/*!
*  @struct	tagHistoryQueryInterface
*  @brief	历史查询接口信息
*/
typedef struct tagHistoryQueryInterface
{
	IF_HistoryQueryCreate ifHistoryQueryCreate;	/*!< 创建历史查询接口              */
	IF_HistoryQueryDestroy ifHistoryQueryDestroy;	/*!< 销毁历史查询接口             */
	IF_HistoryQueryNext ifHistoryQueryNext;	/*!< 历史查询下一条接口              */
} HistoryQueryInterface, *LPHistoryQueryInterface;

/*!
*  @struct	tagHistoryQueryInterfaceMultiType
*  @brief	历史查询接口信息
*/
typedef struct tagHistoryQueryInterfaceMultiType
{
	IF_HistoryQueryCreateMultiType ifHistoryQueryCreate;	/*!< 创建历史查询接口              */
	IF_HistoryQueryDestroy ifHistoryQueryDestroy;	/*!< 销毁历史查询接口             */
	IF_HistoryQueryNextMultiType ifHistoryQueryNext;	/*!< 历史查询下一条接口              */
} HistoryQueryInterfaceMultiType, *LPHistoryQueryInterfaceMultiType;


/*!
*  @struct	tagHistoryQueryInterfaceMultiTypeEx
*  @brief	历史查询接口信息（扩展）
*/
typedef struct tagHistoryQueryInterfaceMultiTypeEx
{
	IF_HistoryQueryCreateMultiTypeRSFile ifHistoryQueryCreate;
} HistoryQueryInterfaceMultiTypeEx, *LPHistoryQueryInterfaceMultiTypeEx;

/*!
*  @struct	tagFileQueryInterface
*  @brief	文件查询接口信息
*/
typedef struct tagFileQueryInterface
{
	IF_FileQueryCreate ifFileQueryCreate;	/*!< 创建文件查询接口               */
	IF_FileQueryDestroy ifFileQueryDestroy;	/*!< 销毁文件查询接口               */
	IF_FileQueryNext ifFileQueryNext;	/*!< 文件查询下一条接口             */
} FileQueryInterface, *LPFileQueryInterface;

/*!
*  @struct	tagLogQueryInterface
*  @brief	日志查询接口信息
*/
typedef struct tagLogQueryInterface
{
	IF_LogQueryCreate ifLogQueryCreate;	/*!< 创建日志查询接口               */
	IF_LogQueryDestroy ifLogQueryDestroy;	/*!< 销毁日志查询接口               */
	IF_LogQueryNext ifLogQueryNext;	/*!< 日志查询下一条接口             */
} LogQueryInterface, *LPLogQueryInterface;

/*!
*  @struct	tagLogQueryInterfaceV1
*  @brief	日志查询V1接口信息
*/
typedef struct tagLogQueryInterfaceV1
{
	IF_LogQueryCreate ifLogQueryCreate;	/*!< 创建日志查询接口               */
	IF_LogQueryDestroy ifLogQueryDestroy;	/*!< 销毁日志查询接口               */
	IF_LogQueryNextV1 ifLogQueryNext;	/*!< 日志查询下一条接口             */
} LogQueryInterfaceV1, *LPLogQueryInterfaceV1;


/*!
*  @struct	tagHistoryChannelInterface
*  @brief	历史通道接口信息
*/
typedef struct tagHistoryChannelInterface
{
	IF_HistoryChannelCreate ifHistoryChannelCreate;	/*!< 创建历史通道接口            */
	IF_HistoryChannelDestroy ifHistoryChannelDestroy;	/*!< 销毁历史通道接口           */
	IF_HistoryChannelInsert ifHistoryChannelInsert;	/*!< 添加流通道号接口            */
	IF_HistoryChannelRemove ifHistoryChannelRemove;	/*!< 移除流通道号接口            */
	IF_HistoryChannelStartCapture ifHistoryChannelStartCapture;	/*!< 开始捕获接口      */
	IF_HistoryChannelStopCapture ifHistoryChannelStopCapture;	/*!< 停止捕获接口       */
	IF_HistoryChannelSetSpeed ifHistoryChannelSetSpeed;	/*!< 历史流快放，仅发送I帧   */
	IF_HistoryChannelPosition ifHistoryChannelPosition;	/*!< 历史通道定位接口      */
} HistoryChannelInterface, *LPHistoryChannelInterface;

/*!
*  @struct	tagHistoryChannelInterfaceMultiType
*  @brief	历史通道接口信息
*/
typedef struct tagHistoryChannelInterfaceMultiType
{
	IF_HistoryChannelCreateMultiType ifHistoryChannelCreate;	/*!< 创建历史通道接口            */
	IF_HistoryChannelDestroy ifHistoryChannelDestroy;	/*!< 销毁历史通道接口           */
	IF_HistoryChannelInsert ifHistoryChannelInsert;	/*!< 添加流通道号接口            */
	IF_HistoryChannelRemove ifHistoryChannelRemove;	/*!< 移除流通道号接口            */
	IF_HistoryChannelStartCapture ifHistoryChannelStartCapture;	/*!< 开始捕获接口      */
	IF_HistoryChannelStopCapture ifHistoryChannelStopCapture;	/*!< 停止捕获接口       */
	IF_HistoryChannelSetSpeed ifHistoryChannelSetSpeed;	/*!< 历史流快放，仅发送I帧   */
	IF_HistoryChannelPosition ifHistoryChannelPosition;	/*!< 历史通道定位接口      */
} HistoryChannelInterfaceMultiType, *LPHistoryChannelInterfaceMultiType;

/*!
*  @struct	tagHistoryChannelInterfaceMultiType
*  @brief	历史通道接口信息
*/
typedef struct tagHistoryChannelInterfaceMultiTypeV1
{
	IF_HistoryChannelCreateMultiTypeV1 ifHistoryChannelCreate;	/*!< 创建历史通道接口            */
	IF_HistoryChannelDestroy ifHistoryChannelDestroy;	/*!< 销毁历史通道接口           */
	IF_HistoryChannelInsert ifHistoryChannelInsert;	/*!< 添加流通道号接口            */
	IF_HistoryChannelRemove ifHistoryChannelRemove;	/*!< 移除流通道号接口            */
	IF_HistoryChannelStartCapture ifHistoryChannelStartCapture;	/*!< 开始捕获接口      */
	IF_HistoryChannelStopCapture ifHistoryChannelStopCapture;	/*!< 停止捕获接口       */
	IF_HistoryChannelSetSpeed ifHistoryChannelSetSpeed;	/*!< 历史流快放，仅发送I帧   */
	IF_HistoryChannelPositionV1 ifHistoryChannelPosition;	/*!< 历史通道定位接口      */
} HistoryChannelInterfaceMultiTypeV1, *LPHistoryChannelInterfaceMultiTypeV1;

typedef struct tagHistoryChannelInterfaceMultiTypeV2
{
	IF_HistoryChannelCreateMultiTypeV1 ifHistoryChannelCreate;	/*!< 创建历史通道接口            */
	IF_HistoryChannelDestroy ifHistoryChannelDestroy;	/*!< 销毁历史通道接口           */
	IF_HistoryChannelInsert ifHistoryChannelInsert;	/*!< 添加流通道号接口            */
	IF_HistoryChannelRemove ifHistoryChannelRemove;	/*!< 移除流通道号接口            */
	IF_HistoryChannelStartCapture ifHistoryChannelStartCapture;	/*!< 开始捕获接口      */
	IF_HistoryChannelStopCapture ifHistoryChannelStopCapture;	/*!< 停止捕获接口       */
	IF_HistoryChannelSetSpeed ifHistoryChannelSetSpeed;	/*!< 历史流快放，仅发送I帧   */
	IF_HistoryChannelPositionV1 ifHistoryChannelPosition;	/*!< 历史通道定位接口      */
	IF_HistoryChannelPause ifHistoryChannelPause;/*!< 历史通道暂停接口      */
	IF_HistoryChannelRecordFileInfo ifHistoryChannelRecordFileInfo;/*!< 历史通道录像文件编码格式查询接口      */

	IF_HistoryChannelCreateSrtMode ifHistoryChannelCreateSrtMode;	//创建历史通道接口(Srt模式，协议找中间层拉历史流)
	IF_HistoryChannelReadFrame ifHistoryChannelReadFrame;//历史通道帧读取接口(Srt模式，协议找中间层拉历史流)

	char szReserved[64];
}HistoryChannelInterfaceMultiTypeV2, *LPHistoryChannelInterfaceMultiTypeV2;

/*!
*  @struct	tagTransparentChanInterface
*  @brief	透明通道接口信息
*/
typedef struct tagTransparentChanInterface
{
	IF_TransparentChannelCreate ifTransparentChannelStart;	/*!< 透明通道创建接口         */
	IF_TransparentChannelDestroy ifTransparentChannelDestroy;	/*!< 透明通道销毁接口   */
	IF_TransparentChannelRead ifTransparentChannelRead;	/*!< 透明通道数据读取接口      */
} TransparentChanInterface, *LPTransparentChanInterface;

/*!
*  @struct	tagRemoteDownloadInterface
*  @brief	远程下载接口信息
*/
typedef struct tagRemoteDownloadInterface
{
	IF_RemoteDownloadCreate ifRemoteDownloadCreate;	/*!< 远程下载创建接口               */
	IF_RemoteDownloadDestroy ifRemoteDownloadDestroy;	/*!< 远程下载销毁接口               */
	IF_RemoteDownloadRead ifRemoteDownloadRead;	/*!< 远程下载Read接口               */
} RemoteDownloadInterface, *LPRemoteDownloadInterface;

/*!
*  @struct	tagRemoteGetBufferInterface
*  @brief	获取Buffer接口信息
*/
typedef struct tagRemoteGetBufferInterface
{
	IF_RemoteGetBufferCreate ifRemoteGetBufferCreate;	/*!< 获取Buffer创建接口               */
	IF_RemoteGetBufferDestroy ifRemoteGetBufferDestroy;	/*!< 获取Buffer销毁接口               */
	IF_RemoteGetBufferRead ifRemoteGetBufferRead;	/*!< 获取Buffer Read接口               */
} RemoteGetBufferInterface, *LPRemoteGetBufferInterface;

/*!
*  @struct	tagRemoteUploadInterface
*  @brief	远程上传接口信息
*/
typedef struct tagRemoteUploadInterface
{
	IF_RemoteUploadCreate ifRemoteUploadCreate;	/*!< 远程文件上传创建接口               */
	IF_RemoteUploadDestroy ifRemoteUploadDestroy;	/*!< 远程文件上传销毁接口               */
	IF_RemoteUploadWrite ifRemoteUploadWrite;	/*!< 远程文件上传Read接口               */
} RemoteUploadInterface, *LPRemoteUploadInterface;

/*!
*  @struct	tagWriteLogFile
*  @brief	协议将log写给中间层的接口
*/
typedef struct tagWriteLogFileInterface
{
	IF_WriteLogFile ifWriteLogFile;	/*!< 写log至中间层接口             */
	char cReserve[128];	/*!< 保留*/
} WriteLogFileInterface, *LPWriteLogFileInterface;

/*!
*  @struct	tagOtherFunctionInterface
*  @brief	其它功能接口信息
*/
typedef struct tagOtherFunctionInterface
{
	IF_AppControlRequest ifAppControlRequest;	/*!< 应用层控制请求接口             */
} OtherFunctionInterface, *LPOtherFunctionInterface;

/*!
*  @struct	tagDiskGroupManageInterface
*  @brief	远程硬盘分组管理接口
*/
typedef struct tagDiskGroupManageInterface
{
	IF_DiskGroupManage ifDiskGroupManage;	/*!< 硬盘分组管理               */
	IF_GetDataSize ifGetDataSize;	/*!< 获取时间片段数据大小       */
} DiskGroupManageInterface, *LPDiskGroupManageInterface;

/*!
*  @struct	tagDiskGroupManageInterface
*  @brief	远程硬盘分组管理接口
*/
typedef struct tagDiskGroupManageInterfaceMultiType
{
	IF_DiskGroupManage ifDiskGroupManage;	/*!< 硬盘分组管理               */
	IF_GetDataSizeMultiType ifGetDataSize;	/*!< 获取时间片段数据大小       */
} DiskGroupManageInterfaceMultiType, *LPDiskGroupManageInterfaceMultiType;

///////////////////////////应用结构信息///////////////////////////////////
typedef struct tagRemoteCommandInterface
{
	IF_RemoteDeviceControl ifRemoteDeviceControl;	/*!< 远程重启动接口             */
	IF_RemoteForceIFrame ifRemoteForceIFrame;	/*!< 远程强制I帧接口            */
	IF_RemoteAlarmClear ifRemoteAlarmClear;	/*!< 远程清除报警接口           */
	IF_RemoteImageCapture ifRemoteImageCapture;	/*!< 远程抓图接口               */
	IF_RemoteFileLock ifRemoteFileLock;	/*!< 远程锁定文件               */
	IF_RemoteRecordControl ifRemoteRecordControl;	/*!< 远程录像控制               */
	IF_RemoteAlarmoutControl ifRemoteAlarmoutControl;	/*!< 远程报警输出控制 */
	IF_RemoteAlarmoutStateGet ifRemoteAlarmoutStateGet;	/*!< 远程报警输出状态获取 */
	IF_PtzControl ifPtzControl;	/*!< 远程云台控制接口           */
	IF_DataExistCheck ifDataExistCheck;	/*!< 数据存在性检测接口         */
	IF_HxhtQuerySupport ifHxhtSupportQuery; /*!< Hxht能力查询接口           */
	IF_SystemUpdateNotify ifSystemUpdateNotify; /*!< Hxht升级请求接口 */
	IF_SetScreenSplitMode ifSetScreenSplitMode;	/*!< 分屏模式控制接口 */
	IF_RemoteSetChromaDefault ifSetChromaDefault; /*!< 远程设置色度默认参数 */
	IF_RemoteAlarminStateGet ifRemoteAlarminStateGet;	/*!< 远程报警输入状态获取           */
	IF_TriggerRecordEvent	ifTriggerRecordEvent;		/*!< 触发录像事件		           */

	//IF_DecoderDynamicConnect	ifDecoderDynamicConnect;	/*!< 配置解码器主动动态连接 */
	//IF_DecoderDynamicDisconnect	ifDecoderDynamicDisconnect;	/*!< 配置解码器主动动态连接断开 */

	//IF_DecoderSetChannelLoopConfig	ifDecoderSetChannelLoopConfig;
	//IF_DecoderGetChannelLoopConfig	ifDecoderGetChannelLoopConfig;
	//IF_DecoderSetChannelLoopStatus	ifDecoderSetChannelLoopStatus;
	//IF_DecoderGetChannelLoopStatus	ifDecoderGetChannelLoopStatus;
	//IF_DecoderGetLoopStatus			ifDecoderGetLoopStatus;

	//IF_DecoderSetConfig				ifDecoderSetConfig;
	//IF_DecoderGetConfig				ifDecoderGetConfig;
	//IF_DecoderGetChannelInformation	ifDecoderGetChannelInformation;
	//IF_DecoderSetDisplayConfig		ifDecoderSetDisplayConfig;
	//IF_DecoderGetDisplayConfig		ifDecoderGetDisplayConfig;

	//IF_DecoderGetChannelConfig		ifDecoderGetChannelConfig;
	//IF_DecoderSetChannelConfig		ifDecoderSetChannelConfig;
	//IF_DecoderGetVideoStandard		ifDecoderGetVideoStandard;
	//IF_DecoderSetVideoStandard		ifDecoderSetVideoStandard;
	//IF_DecoderChannelGetStatus		ifDecoderChannelGetStatus;
	//IF_DecoderChannelGetPlayStatus	ifDecoderChannelGetPlayStatus;
	//IF_DecoderChannelSetPlayStatus	ifDecoderChannelSetPlayStatus;
	//IF_UploadLogo					ifUploadLogo;
	//IF_DecoderChannelLogoSwitch		ifDecoderChannelLogoSwitch;
	//IF_DecoderChannelDisplayControl	ifDecoderChannelDisplayControl;

	//以下接口是Hxht用的，但是与9000软件组协商，将Hxht翻译成私有协议的，这样对于设备来说就没有协议的区分了。
	//以下这些接口都暂时没有使用
// 	IF_HxhtPtzControl			ifHxhtPtzControl;		/*!< Hxht远程云台控制接口           */
// 	IF_SetMotionScheme			ifSetMotionScheme;		/*!< 设置移动侦测布撤防时间接口 */
// 	IF_GetMotionScheme			ifGetMotionScheme;		/*!< 获取移动侦测布撤防时间接口 */
// 	IF_SetAlarmINScheme			ifSetAlarmINScheme;		/*!< 设置报警输入布撤防时间接口 */
// 	IF_GetAlarmINScheme			ifGetAlarmINScheme;		/*!< 获取报警输入布撤防时间接口 */
// 	IF_SetAlarmOUTScheme		ifSetAlarmOUTScheme;	/*!< 设置报警输出布撤防时间接口 */
// 	IF_GetAlarmOUTScheme		ifGetAlarmOUTScheme;	/*!< 获取报警输出布撤防时间接口 */
} RemoteCommandInterface, *LPRemoteCommandInterface;

typedef struct tagRemoteCommandInterfaceV1
{
	int nValid;

	IF_RemoteDeviceControl ifRemoteDeviceControl;	/*!< 远程重启动接口             */
	IF_RemoteForceIFrame ifRemoteForceIFrame;	/*!< 远程强制I帧接口            */
	IF_RemoteAlarmClear ifRemoteAlarmClear;	/*!< 远程清除报警接口           */
	IF_RemoteImageCapture ifRemoteImageCapture;	/*!< 远程抓图接口               */
	IF_RemoteFileLock ifRemoteFileLock;	/*!< 远程锁定文件               */
	IF_RemoteRecordControl ifRemoteRecordControl;	/*!< 远程录像控制               */
	IF_RemoteAlarmoutControl ifRemoteAlarmoutControl;	/*!< 远程报警输出控制 */
	IF_RemoteAlarmoutStateGet ifRemoteAlarmoutStateGet;	/*!< 远程报警输出状态获取 */
	IF_PtzControl ifPtzControl;	/*!< 远程云台控制接口           */
	IF_DataExistCheck ifDataExistCheck;	/*!< 数据存在性检测接口         */
	IF_HxhtQuerySupport ifHxhtSupportQuery; /*!< Hxht能力查询接口           */
	IF_SystemUpdateNotify ifSystemUpdateNotify; /*!< Hxht升级请求接口 */
	IF_SetScreenSplitMode ifSetScreenSplitMode;	/*!< 分屏模式控制接口 */
	IF_RemoteSetChromaDefault ifSetChromaDefault; /*!< 远程设置色度默认参数 */
	IF_RemoteAlarminStateGet ifRemoteAlarminStateGet;	/*!< 远程报警输入状态获取           */
	IF_TriggerRecordEvent	ifTriggerRecordEvent;		/*!< 触发录像事件		           */
	IF_RemoteDeviceControlV1 ifRemoteDeviceControlV1;	/*!< 设备远程控制接口V1             */
	IF_RemoteAlarmoutChannelControl ifRemoteAlarmoutChannelControl;	/*!< 远程报警输出控制(支持报警输出号)             */

	char szReserved[28];
}RemoteCommandInterfaceV1, *LPRemoteCommandInterfaceV1;

typedef struct tagRemoteDesktopInterface
{
	IF_RemoteDesktopConnect ifRemoteDesktopConnect;		/*!< 远程桌面连接		           */
	IF_RemoteDesktopDisconnect ifRemoteDesktopDisconnect;	/*!< 远程桌面断开连接		   */
	IF_RemoteDesktopOperate	   ifRemoteDesktopOperate;		/*!< 远程桌面操作		       */
} RemoteDesktopInterface, *LPRemoteDesktopInterface;
/*!
*  @struct	tagPanelControlInterface
*  @brief	面板控制接口
*/
typedef struct tagPanelControlInterface
{
	IF_GetPanelStatus	ifPanelGetStatus;					/*!< 获取面板状态				*/
	IF_SetPanelStatus	ifPanelSetStatus;					/*!< 设置面板状态				*/
	IF_PanelControl		ifPanelControl;						/*!< 面板控制					*/
}PanelControlInterface, *LPPanelControlInterface;

/*!
*  @struct	tagStreamTransferInterface
*  @brief	流转发接口信息
*/
//typedef struct tagStreamTransferInterface
//{
//	IF_StreamTransferConnect	ifStreamTransferConnect;	/*!< 流转发连接接口             */
//	IF_StreamTransferRecv		ifStreamTransferRecv;		/*!< 流转发接收接口             */
//	IF_StreamTransferDisconnect ifStreamTransferDisconnect;	/*!< 流转发断开接口             */
//}StreamTransferInterface, *LPStreamTransferInterface;

/*!
*  @struct	tagTransmitHistoryStreamQueryInterface
*  @brief	转发历史流查询接口信息
*/
//typedef struct tagTransmitHistoryStreamQueryInterface
//{
//	IF_TransmitHistoryStreamQueryConnect	ifTransmitHistoryStreamQueryConnect;	/*!< 转发历史流查询连接接口             */
//	IF_TransmitHistoryStreamQueryNext		ifTransmitHistoryStreamQueryNext;		/*!< 转发历史流查询接口					*/
//	IF_TransmitHistoryStreamQueryDisconnect ifTransmitHistoryStreamQueryDisconnect;	/*!< 转发历史流查询断开接口             */
//}TransmitHistoryStreamQueryInterface, *LPTransmitHistoryStreamQueryInterface;

/*!
*  @struct	tagRealStreamAccess
*  @brief	历史查询信息
*/
typedef struct tagRealStreamAccess
{
	DWORD 					m_dwChannel;	//通道号
	DWORD 					m_dwAction;		//操作，0：连接，1：断开
	eMediaCodeType			m_eStreamType;	//流类型
	eProtocolID 			m_eProtocol;		//协议类型
	char 					m_cReserve[32];	//保留
} RealStreamAccess, *LPRealStreamAccess;

/*
* @brief
*	实时流访问控制
* @param dwUserID
*	用户ID
* @param cRealStreamAccess
*	实时流访问参数
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
typedef int(*IF_RealStreamAccess) (DWORD dwUserID, LPRealStreamAccess cRealStreamAccess);

/*!
*  @struct	tagRealStreamAccessInterface
*  @brief	流访问
*/
typedef struct tagRealStreamAccessInterface
{
	IF_RealStreamAccess ifStreamAccess;	/*!< 流访问接口                    */
	char cReserve[32];					//保留字段
} RealStreamAccessInterface, *LPRealStreamAccessInterface;

/*!
*  @struct	tagTransparentInterface
*  @brief	透传请求接口
*/
typedef struct tagTransparentInterface
{
	IF_HttpRequest		ifHttpRequest;	/*!< HTTP请求处理接口              */
	char cReserve[32];					//保留字段
} TransparentInterface, *LPTransparentInterface;

typedef int(*IF_HieServerGetConfig)(DWORD dwProtocol, DWORD dwMajorCommand, DWORD dwMinorCommand, char *buffer, DWORD dwBufferLength);
typedef int(*IF_HieServerSetConfig)(DWORD dwProtocol, DWORD dwMajorCommand, DWORD dwMinorCommand, char *buffer, DWORD dwBufferLength);

typedef struct tagHieServerConfigInterface
{
	IF_HieServerGetConfig ifGetConfig;			/*!< 配置获取接口              */
	IF_HieServerSetConfig ifSetConfig;			/*!< 配置设置接口              */

	char cReserve[32];
}HieServerConfigInterface, *LPHieServerConfigInterface;


/*!
*  @struct    tagPullRealStreamInterface
*  @brief    拉流方式接口合集
*/
typedef struct tagPullRealStreamInterface
{
    IF_RealStreamConnectionAdd ifRealStreamConnectionAdd;    /*!< 实时流消费者添加请求接口*/
    IF_RealStreamConnectionDelete ifRealStreamConnectionDelete;    /*!< 实时流消费者删除请求接口*/
    IF_RealStreamConnectionGetData ifRealStreamConnectionGetData;    /*!< 实时流消费者获取数据请求接口       */
    IF_RealStreamConnectionReleaseData ifRealStreamConnectionReleaseData;   /*!< 实时流消费者释放数据*/
} PullRealStreamInterface, *LPPullRealStreamInterface;


/*!
*  @struct	tagVideoStreamInterface
*  @brief	语音流接口信息
*/
typedef struct tagVideoStreamInterface
{
	IF_VideoStreamCreate ifVideoStreamCreate;	/*!< 视频通话流创建接口                 */
	IF_VideoStreamDestroy ifVideoStreamDestroy;	/*!< 视频通话流销毁接口                */
	IF_VideoStreamRead ifVideoStreamRead;	/*!< 视频通话流读取接口                   */
	IF_VideoStreamControl ifVideoStreamControl;	/*!< 视频通话控制接口                   */
} VideoStreamInterface, *LPVideoStreamInterface;

#endif
