#ifndef __GLOBAL_CONSTANT_H__
#define __GLOBAL_CONSTANT_H__



/* 数字宏 */
//!\brief 支持的最大用户数
#ifdef USE_ICAMERA
#define IDVR_MAX_USER  17
#else
#define IDVR_MAX_USER  16
#endif

//!包含结束符:用户名和密码
//!\brief 用户名字符串最大长度
#define MAX_USERNAME_LEN 17
//!\brief 密码字符串最大长度
#define MAX_PASSWORD_LEN  17
//!\brief email最大长度
#define MAX_EMAIL_LEN  248
//!\brief MAC地址字节数
#define MAC_ADDRESS_BYTES  6
//!\brief 云台协议名长度
#define MAX_PROTONAME_LEN 48
//!\brief 内部/外部云台协议各256
#define MAX_PROTOCOL_NUM  256
#define	NET_MAX_DOMAINNAME_LEN	        (256)
#define	NET_MAX_EMAIL_LEN			    (256)
#define NET_MAX_USER_NAME_LEN           (17)
#define	NET_MAX_PASSWORD_LEN		    (64)
#define NET_MAX_SUBJECT_LEN         	(32)
#define NET_MAX_EMAIL_BODY_LEN      	(128)
#define SEND_END_EVNET_TIME_INTERNAL	(15)

//!\brief IP白名单个数
#define MAX_WHITEIP_NUM  10
#define LICENSE_CODE_MAX_LEN (256)
#define MAX_SERIAL_NUM_LEN	   (32)
#define MAC_ADDR_LEN		   (6)
#define MAX_DID_NUM_LEN		   (63)
#define MAX_DID_LENGTH      (64)    //UID总长度
#define MAX_PUSH_ID_LENGTH     (8)       //推送ID总长度
#define MAX_PUSH_ID_SFD        ('#')     //推送ID起始定界符
#define DATA_IS_EVEN(x) ((x) % 2 == 0)

#define  MAX_AREADEFENSE_NUM 1
#define  MAX_POLYGON_LINE_NUM 8
// //缺省灵敏度  
#define  DEFAULT_GRADE_FCS   (1)     	//  CFaceCapSimplify
#define  DEFAULT_GRADE_HS    (1)     	//  CHumanShape
#define  DEFAULT_GRADE_M     (1)     	//  CMotion
#define  OFF_DUTY_FILE_FORMAT_VERSION (2)
#define  PASSENGER_FLOW_FILE_FORMAT_VERSION (2)
#define	 MAX_OFF_DUTY_NUMBER_OF_DAYS (7)	//离岗统计保存的最大天数
#define	 MAX_PASSENGER_FLOW_NUMBER_OF_DAYS (7)	//客流统计保存的最大天数
#define	 ONE_DAY_HOURS              (24)
#define  WRITE_FILE_INTERVAL_SECOND (30)

#define	 OFF_DUTY_DATE_DATA_LENTH (8)  //离岗统计日期占的字节长度
#define  OFF_DUTY_FILE_START_CODE (0x5852533A)   //"XRS:"
#define	 OFF_DUTY_REGION_POINT_NUM (4)

#define	 PASSENGER_FLOW_DATE_DATA_LENTH (8)  //客流统计日期占的字节长度
#define  PASSENGER_FLOW_FILE_START_CODE (0x5852533A)   //"XRS:"
#define	 PASSENGER_FLOW_REGION_POINT_NUM (4)

//移动侦测区域最大数目
#define  MAX_MOTION_REGION_NUM	16
// 人形侦测区域最大数目
#define  MAX_HUMANSHAPE_REGION_NUM	1

//移动侦测触发联动动作间隔默认值
#define  MOTION_ACTION_LINKAGE_INTERVAL_DEF (120)
//人形侦测触发联动动作间隔默认值
#define  HUMAN_SHAPE_ACTION_LINKAGE_INTERVAL_DEF (120)
//报警输入联动推送动作间隔默认值
#define  ALARMIN_ACTION_LINKAGE_INTERVAL_DEF (120)
//电子围栏触发联动动作间隔默认值
#define  ELEC_ACTION_LINKAGE_INTERVAL_DEF (120)
//越界侦测触发联动动作间隔默认值
#define  CROSSBOARD_ACTION_LINKAGE_INTERVAL_DEF (120)
//离岗侦测联动推送动作间隔默认值
#define  OFFDUTY_ACTION_LINKAGE_INTERVAL_DEF (120)
//客流统计触发联动动作间隔默认值
#define  PASSENGER_ACTION_LINKAGE_INTERVAL_DEF (120)
//智能跟踪联动推送动作间隔默认值
#define  SMARTTRACK_ACTION_LINKAGE_INTERVAL_DEF (120)

#define NO_TRIGGER_TIME         (10)
//!\brief  动作种类数
#define   MAX_ACTION_NUM        (14)
//!\brief  每种动作的最大子动作数
#define  MAX_ACTION_NUM_SUB     (5)
//!\brief  触发动作的最大通道数
#define  MAX_ACTION_CH_NUM      (32)

#ifndef ABS
#define ABS(x)                  (((x)<0) ? -(x) : (x))
#endif
#define IMAGE_TOTAL_WIDE        (10000)
#define IMAGE_TOTAL_HIGHT       (10000)
#define MATH_PI                 (3.1415926)
#define HORIZON_SPEED_DAMPING   (1.2)
#define VORIZON_SPEED_DAMPING   (1.2)

#define MAX_CHN_NUM 1
// 报警输入		0	ALERT_TYPE_ALARMIN
// 移动侦测		1	ALERT_TYPE_MOTION
// 人形检测 	2	ALERT_TYPE_HUMANSHAPE
// 人脸检测		3	ALERT_TYPE_FACEDETECT
// 车形检测		4	ALERT_TYPE_CAR
// 电子围栏		5	ALERT_TYPE_FENCEDETECT
// 越界检测		6	ALERT_TYPE_TRIPWIRE
// 离岗检测		7	ALERT_TYPE_OFFDUTY_DETECT
// 客流统计		8	ALERT_TYPE_HUMANCOUNT
// 火焰侦测		9	ALERT_TYPE_FIRE_DETECT
// 视频遮挡检测	10	ALERT_TYPE_VIDEO_SHELTER
// 人脸抓拍		11	ALERT_TYPE_FACE_SNAPSHOT
// 口罩检测		12	ALERT_TYPE_MASK_DETECT
// 车牌抓拍		13	ALERT_TYPE_LICENSE_PLATE_CAP
// 人脸识别		14	ALERT_TYPE_FACERECO
#define MAX_ALARM_TYPE          (15)
#define ALARM_ALART_SIZE (MAX_CHN_NUM * MAX_ALARM_TYPE)

//流类型
typedef enum {
	STREAM_TYPE_MAIN,
	STREAM_TYPE_AUX,
	STREAM_TYPE_HDAUX,
	STREAM_TYPE_FOURTH,
	STREAM_TYPE_CAPTURE,
	STREAM_TYPE_MAIN_C1,		//第二主码流
	STREAM_TYPE_AUX_C1,			//第二子码流
	STREAM_TYPE_MAX,
}STREAM_TYPE;

#define AI_STREAM_BUFFER_LEN	(16 * 1024)
#define AVL_HEADERMARK0         (0xB6010000)
#define AVL_HEADERMARK1         (0x34363248)
#define INDEXMASK			    (0x80)
#define PRIVATEDATEMASK         (0X02)
#define STREAMTYPEMASK		    (0X01)

#define DEBUG_WATCH
#define MAXNUM_VATARGET                 (32)  //最多目标点数目

#ifdef SYSTEM_SUPPORT_CHANNEL
//最大通道个数
#define MAX_CH_NUM                 (SYSTEM_SUPPORT_CHANNEL)
#else
#define MAX_CH_NUM					(1)
#endif


//最大流个数
#define DVR_MAX_STREAM_NUM				(6)
//最大秒数
#define MAX_SECOND_NUM					(5)
//看门狗重启5S内帧率阈值
#define FRAME_THRESHOLD					(1)
//码流缓冲大小 500k
#define MAX_BUFF_LEN                    (500000)
#define Stat_Time_Interval              (5)
#define MAX_CUSTOM_AUDIO_SIZE	        (128 * 1024)

#define DATABASE_MAX_NUM			        (1000)
#define PARTITION_MAX_USED			        (95.0)
#define INVALID_RESULT 				        (-999)
#define BUFFER_STRINGS_MAX_NUM 		        (64)
#define MAX_BUFFER_SIZE				        (512 * 1024)
#define USER_CUSTOM_TEMPLATE  		        (5)
#define  MAX_FACE_REGION_NUM	            (1)
#define  MAX_CHECK_FACE_COUNT	            (10)
#define ERR_CODE_SAFETY_CODE_NOT_MATCH		(4208)

#define MAX_ALARMOUT_NUM                    (32)
#define MAX_DOP_NAME_LEN                    (20)
#define MAX_MOSAIC_RECT                     (4)
#define  MAX_MAGIC_BLOCK_WIDTH      (704/16)
#define  MAX_MAGIC_BLOCK_HEIGHT     (576/12)
//移动块单元个数
#define   MAX_MAGIC_BLOCK_NUM       (16*12)
//缺省检测帧率
#define   DEFAULT_FPS               (10)
#define   DEFAULT_THRESHOLD         (30)
/* COsd */
#define SUPPORT_TEXT_OSD_TWO_LINE
#define NEW_LINE_CHAR               (0X5E)			//换行符

#define STRING_OSD
#define DVR_MAX_CHANNELNAME_LEN			(20 + 1)	//点阵通道名最大长度,含结束符
#define OSD_TEXT_EX_MAX_EXTENT_OSD_CNT		(8)
#define OSD_TEXT_EX_MAX_EXTENT_OSD_TEXT_LENGTH			(256)
#define OSD_TEXT_EX_MAX_EXTENT_OSD_TOKEN_LENGTH     	(32)
#define OSD_SAVE_COUNT_ONE_LINE						(20)
#define OSD_TEXT_EX_MAX_CHAR_ONE_LINE				(60)
#define OSD_TEXT_EX_MAX_LINE_NUM					(2)
#define OSD_TEXT_EX_MAX_CHAR						(OSD_TEXT_EX_MAX_CHAR_ONE_LINE * OSD_TEXT_EX_MAX_LINE_NUM)
#define OSD_TEXT_EX_MAX_COORD_LEN				    (3)
#define OSDV2_TEXT_CNT_SAVE_ONE_LINE	            (32)
#define OSDV2_TEXT_OVERLAY	                        (12)

#define MAIN_STREAM_RECORD_INDEX                    (0)
#define AUX_STREAM_RECORD_INDEX                     (1)
#define HDAUX_STREAM_RECORD_INDEX                   (2)
#define FOURTH_STREAM_RECORD_INDEX                  (3)

#define MAX_STREAM_NUM              (6)
#define MAX_STRING_CNT              (32)
#define MAX_RESOLUTION_CNT          (32)
#define MAX_PICFUNC_CNT             (3)	//最多支持的图像功能计数
#define MAX_CAP_FACE_NUM 1
#define MAX_CAP_FACE_CACHE_NUM (MAX_CAP_FACE_NUM + 1)
#ifndef SYSTEM_SUPPORT_CHANNEL
#define DVR_MAX_VIDEOIN_NUM				(1)
#else
#define DVR_MAX_VIDEOIN_NUM				(SYSTEM_SUPPORT_CHANNEL)
#endif

/** @brief 人脸脸库ID最大长度													           */
#define MAX_FACE_LIB_ID_LENGTH				(48)
/** @brief 人脸ID最大长度														           */
#define MAX_FACE_ID_LENGTH					(48)
/** @brief 人脸脸库名称最大长度												          		*/
#define MAX_FACE_LIB_NAME_LENGTH			(32)
/** @brief 人脸名称最大长度												          	 		*/
#define MAX_FACE_NAME_LENGTH				(32)
/** @brief 人脸脸库ID最大长度V2											          			*/
#define MAX_FACE_LIB_ID_LENGTH_V2			(64)
/** @brief 人脸ID最大长度V2														          	*/
#define MAX_FACE_ID_LENGTH_V2				(64)
/** @brief 人脸脸库名称最大长度V2												          	 */
#define MAX_FACE_LIB_NAME_LENGTH_V2			(64)
/** @brief 人脸名称最大长度V2												          	 	 */
#define MAX_FACE_NAME_LENGTH_V2				(64)

#define  AVL_HY_DVR_MAX_DELETE_PERSON_INFO_COUNT  100
#define AVL_HY_DVR_MAX_GROUP_FACE_COUNT_V1	100
#define AVL_HY_DVR_RECO_THRESHOLD_SCHEDULE_COUNT 4


#define MAX_DAYS                   (7)
#define HY_DVR_MOTIONSEG_DAY       (48)
#define	HY_DVR_RECORDSEG_DAY       (48)
#define HY_DVR_VIDEOLOSTSEG_DAY    (48)
#define HY_DVR_ALARMINSEG_DAY      (48)
#define HY_DVR_ALARMOUTSEG_DAY     (48)
#define MAX_POLYGON_SIZE           (8)
#define TIMEOUT_USEC               (0)
#define MAX_ALARMIN_NUM            (32)
#define MAX_DIP_NAME_LEN           (20)


#define __SEARCH_ALL_TYPE__        (0xff)
#define ARCH_MPC_FM
#define NANOSLEEP
#define CUR_DISK_FULL              (0)
#define ALL_DISK_FULL              (1)
#define MAXFINDTHREADNUM           (100)
#define FASTFIND_PREV              (-1)
#define FASTFIND_NEXT              (1)
#define DELETEFILENUM              (1)
#define CALLBACK
#define MODE_CHECK_ALL_DISK_FULL    1
#define MODE_CHECK_CUR_DISK_FULL    0

/*设备型谱名称的长度-----  25 和 32 的值 */
#define MAX_NAME_LEN          32
/*设备RS232、RS483、RS42的最大个数*/
#define MAX_RS_CHANNEL        4
#ifndef MAX_LOG_LEN
#define MAX_LOG_LEN     512
#endif
/*定义视频压缩格式*/
#define VIDEO_MPEG4       0x01
#define VIDEO_H264        0x02
#define VIDEO_ST          0x04
/*定义音频编码格式*/
#define AUDIO_G726        0x01
#define AUDIO_G729        0x02
#define AUDIO_G711        	0x04
#define AUDIO_MPEG4AAC	0x08
/*支持的码流类型*/
#define STREAM_MAIN       0x01
#define STREAM_SUB        0x02
#define STREAM_HDSUB        0x04
/*压缩尺寸*/
#define CR_QCIF	        0x01
#define CR_CIF	        0x02
#define CR_2CIF	        0x04
#define CR_4CIF	        0x08
#define CR_4CIF_SIM     0x10		//非实时D1
#define CR_2CIF_SIM     0x20		//非实时2CIF
#define CR_QVGA		    0x40		//320*240
#define CR_VGA		    0x80		//640*480
#define CR_SVGA		    0x100		//800*600
#define CR_XVGA		    0x200		//1024*768
#define CR_HD720	    0x400		//1280*720
#define CR_QuadVGA	    0x800		//1280*960
#define CR_SXGA		    0x1000		//1280*1024
#define CR_UXGA		    0x2000		//1600*1200
#define CR_HD1080	    0x4000		//1920*1080
#define CR_HD576	    0x8000		//960*576
#define CR_QXGA		    0x10000		//2048*1536
#define CR_640x360	    0x20000		//640*360
#define CR_800x448	    0x30000		//800*448
#define CR_UWXGA        0x40000     //2592*1520   400W
#define CR_USXGA        0x80000     //2592*1944   500W
//#define CR_500MP      0x8000      //2560*1920
#define CR_D1	CR_2CIF
#define CR_SD	CR_4CIF
/*媒体文件封装格式*/
#define FILE_MP4		0x01
#define FILE_ASF		0x02
#define FILE_AVI		0x04
/*硬盘类型*/
#define IDE_ATA		0x01
#define IDE_SATA          0x02
#define IDE_SD		0x04
#define IDE_USB		0x08

#define ATTR_DIR	0x01
#define ATTR_FILE	0x02

#define DVR_MAX_HOST_LENTH	        (128)
/** @brief 最大用户名长度														          		 */
#define DVR_MAX_USERNAME_LEN 			(64)
/** @brief 最大密码长度															          		 */
#define DVR_MAX_PASSWORD_LEN 			(64)
#define PANEL_CMD_GETID		1
#define PANEL_CMD_SETID		2
#define NORMAL_HZ           10
#define IPRESET_HZ          20
#define DISKERROR_HZ        2

#ifdef __USE_CIRCLEQUEUE__
#define MAX_MAIN_ACCESS_NUM       (2)   //主sensor主码流最大预览流数量
#if defined (__PRODUCT_P4__)||defined(__PRODUCT_P4U__)
#define MAX_SECOND_ACCESS_NUM     (4)   //主sensor次码流最大预览流数量
#else
#define MAX_SECOND_ACCESS_NUM     (3)   //主sensor次码流最大预览流数量
#endif

#define MAX_MAIN_ACCESS_NUM_S1    (2)   //次sensor主码流最大预览流数量
#define MAX_SECOND_ACCESS_NUM_S1  (3)   //次sensor次码流最大预览流数量
#define MAX_MAIN_ACCESS_NUM_S2    (2)   //次sensor主码流最大预览流数量
#define MAX_SECOND_ACCESS_NUM_S2  (3)   //次sensor次码流最大预览流数量
#define MAX_MAIN_ACCESS_NUM_S3    (2)   //次sensor主码流最大预览流数量
#define MAX_SECOND_ACCESS_NUM_S3  (3)   //次sensor次码流最大预览流数量
#define MAX_THIRD_ACCESS_NUM        (3)
#else
#define MAX_MAIN_ACCESS_NUM         (3)
#define MAX_SECOND_ACCESS_NUM       (3)
#define MAX_THIRD_ACCESS_NUM        (3)
#endif

#define MAIN_SUB_QUEUE_SIZE	(1.5*1024*1024)
#define MJPEG_QUEUE_SIZE	(128*1024)
#define SECOND_MAIN_SUB_QUEUE_SIZE	(1.5*1024*1024)
#define SECOND_MJPEG_QUEUE_SIZE	(128*1024)
#define QUEUE_DURATION	(5*1000)

#define MAX_PICTURE_BUF_SIZE   (128*1024)
#define MAX_EVETN_PICTURE_BUF_SIZE   (128*1024)    //事件图片是算法流编码的，非MJPEG流的图

#define BUFFER_SIZE (4 * 1024)
#define SEND_MOTION_CONTINUOUS_EVNET_TIME_INTERNAL		(1)
#define SEND_CONTINUOUS_EVNET_TIME_INTERNAL				(5)
#define PICTURE_HANDLEMARK   (8)
#define MAX_PICTURESUPPORT_SEARCH (100)
#define PICTURE_MARK_SHIFT  (8)
#define PICTUREINDEX_MASK  (~((-1)<<PICTURE_MARK_SHIFT))
#define PICTURE_FAILED_HANDLE ((HANDLE)0x800000FF)
#define MAX_PLAYBACK_SUPPORT (1)
#define MAX_DATA_BUFFER_SIZE (1024 *1024)
#define MAX_SUPPORT_RECTYPE (6)
#define PLAYBACK_MARK_SHIFT  (8)
#define PLAYBACK_INDEX_MASK  (~((-1)<<PLAYBACK_MARK_SHIFT))
#define PLAYBACK_FAILED_HANDLE ((HANDLE)0xC00000FF)


#define     TypeDiskGroupNormal     0	/*!<    普通盘组        */
#define     TypeDiskGroupAlarm      0	/*!<报警盘组        */
#define     TypeDiskGroupRedundance 1	/*!< 冗余盘组           */
#define     TypeDiskGroupBackup     2	/*!< 备份盘组       */
#define     NEW_QUERY
#define     MAX_SEG_TIME            (24*3600)
#define     TypeDiskGroupNormal_RC     0	/*!<    普通盘组        */
#define     TypeDiskGroupAlarm_RC      1	/*!<报警盘组        */
#define     TypeDiskGroupRedundance_RC 2	/*!< 冗余盘组           */
#define     TypeDiskGroupBackup_RC     3	/*!< 备份盘组       */
//获取盘组的大小，0表示查询普通分组+报警分组、1表示冗余分组、2表示备份分组
#define     Size_TypeDiskGroupNormal     0	/*!<    普通盘组 + 报警盘组       */
#define     Size_TypeDiskGroupRedundance 1	/*!< 冗余盘组           */
#define     Size_TypeDiskGroupBackup     2	/*!< 备份盘组 */

#define KEY_ID_MEM_UPGRADE     		(0x18)
#define KEY_ID_MEM_AUTH     	(0x6666)
#define UPLOAD_TIMEOUT_DETECTION   		 (300) //超时检测时间
#define UPLOAD_TIMEOUT_DETECTION_WIFI    (600) //超时检测时间
#define UPGRADE_MAX_MEM_NUM     16
#define CLOUD_UPGRADE_RET_DELAY (5)    //云升级返回延时
#define CLOUD_UPGRADE_REBOOT_MONITOR_DELAY	(10*60)	//重启监控倒计时
#define CLOUD_UPGRADE_LIST_DOWNLOAD_CONNECT_TIMEOUT			(10)
#define CLOUD_UPGRADE_LIST_DOWNLOAD_TIMEOUT					(10)
#define CLOUD_UPGRADE_LIST_DOWNLOAD_TRIES_CNT				(5)
#define CLOUD_UPGRADE_UPGRADE_FILE_DOWNLOAD_CONNECT_TIMEOUT	(10)
#define CLOUD_UPGRADE_UPGRADE_FILE_DOWNLOAD_TIMEOUT			(8*60)
#define CLOUD_UPGRADE_UPGRADE_FILE_DOWNLOAD_TRIES_CNT		(5)
#define CLOUD_UPGRADE_DOWNLOAD_LOW_SPEED_LIMIT				(1L)
#define CLOUD_UPGRADE_DOWNLOAD_LOW_SPEED_TIME				(6L)

#define MPU_MAX_SUPPORT_DISKNUM   (20)
#define SEGSIZE		4096
#define MAX_USB_DEV		16
#define MAX_IDE_DEV		8
#define MAX_SYSTEM_CMD_NUM      32
#define SYSTEM_TIME_OUT         (10*60*100)	//10ms in unit
#define SUCCESS 0
#define FAILURE -1
#define DEV_NAME_POS	2

#define DEV_NAME_INDEX	(5 + DEV_NAME_POS)
#define USB_SHM_SIGNUM	36
#define USB_REMOVE_SIGNUM	37
#define USB_ADD_SIGNUM		38
#define USB_REP_SIGNUM		39
#define USB_IND_REV (MAX_IDE_DEV - 2)
#define USB_IND_ADD	(MAX_IDE_DEV - 3)
#define USB_REP_ADD	(MAX_IDE_DEV - 4)

#define SYS_TIMEOUT (-2)
#define TIME_IN_SEC		10//3*60	// how long client will wait for server response in non-blocking mode
// #define BUFFER_SIZE		10240	// SendData and RecvData buffers sizes
#define MSG_SIZE_IN_MB	25		// the maximum size of the message with all attachments
#define COUNTER_VALUE	100		// how many times program will try to receive data

#define KEY_SIZE_50                 (50)
#define MAX_HTTP_RESPONSE_LENGTH    (16 * 1024)
#define AMS_MAX_DOMAINNAME_LEN 		(256)
#define DDNS_STATE_STOP				(0)
#define DDNS_STATE_CONNECTING		(1)
#define DDNS_STATE_CONNECTED		(2)
#define DDNS_STATE_CONNECT_FAILED	(3)
#define CHECKEXTERNIPTIME           (60) //检测外网改变的时间--60秒
#define	NET_EMAIL_QUEUE_LEN	        (16 + 16 + 16 + 5)
#define MINEMAILTIMEINTERVAL        (60)

#define  MAX_DDNS_PROTO             (32)
#define  DOMAINNAMELEN              (256)
#define  PROTOCOLNAMELEN            (128)
#define  DDNS_USERNAME_LEN          (64)
#define  DDNS_PASSWORD_LEN          (64)
#define NMS_MAX_DOMAINNAME_LEN      (256)

#if defined  (__USE_FTP__)
#define FTP_SEND_PIC_ONLY 1
#endif

#if defined (__AUTH_V2__)
#define PLATFORM_NUM		(9)
#define PLATFORM_LENTH		(5)
#endif
#define IP_DAT4_MAX         (254)   //网段下可用的最大IP 
#define IP_DAT4_MIN         (2)     //网段下可用的最小IP 
#define RAND_SAME_MAX       (60) //随机产生的值与之前相同的限制值,到达后就去表中查找可用IP。经验值，可随实际现象修改其值
#define IP_NUM              (253)       //网段下总共可用的IP个数


/* iNet */
#define DHCP_FAILE_TIMES        (3)
#define WLAN0_INDEX             (1)
#define DEL_MULTICAST_ROUTE
#define MAC_BCAST_ADDR          (unsigned char *) "\xff\xff\xff\xff\xff\xff"
#define MAX_UID                 (64)
#define MAX_IP_STR_LEN          (16)
#define MAX_MAC_STR_LENGTH      (18)
#define DETECT_MAX_NUM		    (5)
#define IP_MAX_LEN			    (16)
#define ADDRESS_DEFAULT_SIZE    (20)
#define MULTI_DETECT_CYCLE	    (30)	//多探测源修改周期
#define PROBE_OVER_TIME		    (30)	//探测超时时间
#define MAX_REQUEST_URL_LEN     (256)
#define MIX_REQUEST_URL_LEN     (10)
#define WIFI_LIST_MAX   30

/* INTP */
#define NTP_STATE_STOP				        (0)
#define NTP_STATE_CONNECTING		        (1)
#define NTP_STATE_CONNECTED			        (2)
#define NTP_STATE_CONNECT_FAILED	        (3)
#define NTP_UPDATE_INTERVAL_DECOND_MIN      (60)
#define NTP_MAX_DOMAINNAME_LEN 			    (256)
/* IPPPOE */
#define  MSG_PPPOE_LOCAL_NAME               (0x5001)
#define  PPPOE_USERNAME_LEN                 (64)
#define  PPPOE_PASSWORD_LEN                 (64)

#define ICMP_DATA_LEN                       (20)        /* ICMP负载长度 */
#define ICMP_ECHO_MAX                       (4)         /* ECHO-REQUEST报文发送次数 */
#define ICMP_REQUEST_TIMEOUT                (2)         /* 超时时间间隔 */
#define MAX_ALLOC_SIZE                      (256)   //notice-fjz

#define CHECK_SQL_FILE
//!\brief 第一次分配section的个数
#define DEF_SEC_NUM                         (15)
//!\brief 递增的section的个数
#define ADD_SEC_NUM                         (5)
//!\brief 第一次分配的key的个数
#define DEF_KEY_NUM                         (5)
//!\brief 递增的key的个数
#define ADD_KEY_NUM                         (5)
//!\brief ini文件每行的字符数
#define INI_LINE_LENGTH                     (320)
//!\brief ini section 名称最大长度
#define INI_MAX_SECTION_NAME_LENGTH         (48)
//!\brief ini key 名称最大长度
#define INI_MAX_KEY_NAME_LENGTH             (48)
//!\brief ini key 值最大长度
#define INI_MAX_KEY_VALUE_LENGTH            (255)
#define CONFIG_INSTANCE_WAINING_THRESHOLD   (6)

#define DEFAUTLAUDIOTYPE                    (1)
#define JSON_SUCCESS_CODE       			(200)
#define JSON_FORMAT_ERROR 					(201)
#define JSON_CMDINFO_ERROR      			(202)
#define JSON_RESPOND_ERROR      			(203)
#define JSON_TESTMODE_ERROR     			(204)
#define RESPONDMAXLENTH                     (2048)
#define MAXMAINCMD       	                (30)
#define FIRSTPARAMLENTH                     (32)
#define SECPARAMLENTH                       (32)
#define PTZ_SPEED_BASE                      (0)
#define PTZ_SPEED_LEV_MAX                   (10)
#define MAX_CRUISE_COUNT                    (16)
#define MAX_CRUISE_POINT                    (128)
#define PTZ_STOP_MOTO_CRUISE_TIME            (15 *60)



#define RECV_BUF_LEN                        (512)
#define SUPPORT_OSD_MAX_LINE                (14)//11
#define OSD_MAX_STR_LEN                     (31)//20
#define RECV_COMMAND_MIN_LEN                (4)//6
#define COMMAND_HEAD1_ASC                   (0x51)//0xAA
#define COMMAND_HEAD2_ASC                   (0x01)//0x5A
#define COMMAND_HEAD3_ASC                   (0x04)
#define COMMAND_HEAD4_ASC                   (0x78)//读(收)
#define COMMAND_TAIL_ASC                    (0xFF)
#define OSD_SET_COMMAND_INFO_LEN            (2)//(6+2) //OSD设置命令中除了设置的字符外的字符所占长度
#define IS_NIGHT(x)                         ((x) & (0x1))
#define IS_EXTERN_CTL(x)                    ((x) & (0x2))
//OSD 操作类：
#define OSDOPS_TITLPRINT                    (1) //OSD 打印
#define OSDOPS_TITLCLEAN                    (2) //OSD 清屏
#define SUPPORT_MAX_OSD_LINE                (14)//11
#define SUPPORT_LINE_MAX_CHAR               (31)//20
#define PROTOCOL_NAME_MAX                   (32)
#define PROTOCOL_PATH_MAX                   (128)
//!\brief 手动控制云台后多长时间后跟踪继续
#define TIME_AFTER_NORMAL_CTRL              (10)
#define TRANSPARENT_BUFF_SIZE               (1024)
#define TIMEOUT_SEC_2(datalen, baudrate)    ((datalen) * 20 / (baudrate) + 2)	//接收超时

#define HAS_8_BIT						    (8)
#define ASC_WIDTH						    (8)	//隐形条件------HZK宽是ASC的两倍，高相等
#define ASC_HEIGTH						    (16)
#define ONE_LINE_MAX_PIX				    (SUPPORT_LINE_MAX_CHAR * ASC_WIDTH)
#define OSD_STRIDE						    ((ONE_LINE_MAX_PIX + 7) >> 3)//一整行所占的字节数
#define ASC_STRIDE						    ((ASC_WIDTH + 7) >> 3)//ASC一行所占的字节数
#define ONE_ASC_CHAR_BYTE				    (ASC_STRIDE * ASC_HEIGTH)//一个asc占的字节
#define ALL_PIX_MAX_BYTE				    (ONE_ASC_CHAR_BYTE * SUPPORT_LINE_MAX_CHAR)//所有点阵占的字节

//第一部分为中文
#define HECS1_FirstChar_1		            (0x4E00)
#define HECS1_LastChar_1		            (0x9FA5)
#define HECS1_CharCount_1	                (HECS1_LastChar_1 - HECS1_FirstChar_1 + 1)
//第二部分为拉丁语1
#define HECS1_FirstChar_2		            (0x0080)
#define HECS1_LastChar_2		            (0x023F)
#define HECS1_CharCount_2	                (HECS1_LastChar_2 - HECS1_FirstChar_2 + 1)

#define     MAX_TMP_PICTURE_COUNT           (3)
/* 最小的定时抓图时间间隔 */
#define     MIN_SNAPSHOT_TIME_INTERVAL      (10)
/* 最大的定时抓图时间间隔 */
#define     MAX_SNAPSHOT_TIME_INTERVAL      (86400)
#define MAX_TEMP_PIC_NUM_TYPE               (2)
#define SAVE_PIC_IN_MEM
#define SUPPORT_PIC_NUM                     (3)

#define ONE_PIC_MAX_SIZE   MAX_PICTURE_BUF_SIZE
#define PIC_BUFF_MAX_SIZE  (ONE_PIC_MAX_SIZE*SUPPORT_PIC_NUM)

// 语音播放超时时间
#define SPEECH_PLAY_TIME_OUT_SEC    (10)

#define ACTIONTYPE(ActionType)                  ((ActionType>>16))
#define MAKEACTIONTYPE(ActionType)              ((ActionType<<16))
#define ACTIONCHILDTYPE(ActionType)             ((ActionType & 0xff) - 1)
#define PTZ_ACTIONTYPE(action)                  ((action) & 0xFFFFFF)
#define ACTIONINDEX(action)                     ((action) >> 24)

#define DOCNAME_MAX 		                    (255)

#define KEEP_FOREVER                            (0x80000000)
#define MAX_RECORD_TYPE                         (8)
#define ERROR_RECORD_TYPE                       (-1)
#define H264DATAPACKETHEADER_REAL_TIME_OFFSET   (14)
#define MAX_PLAY_PORT                           (16)


#define SEARCH_RECORD_ALL                       (0x8f)
#define __REAL__CH__                            (1)
#define FULL_DISK_SIZE                          (50*__REAL__CH__)
#define MINI_DISK_SIZE                          (10*__REAL__CH__)

#define IS_NANDWRITE (1)

#define OPT_p  (1 << 0) /* nandwrite only */
#define OPT_o  (1 << 0) /* nanddump only */
#define OPT_s  (1 << 1)
#define OPT_b  (1 << 2)
#define OPT_f  (1 << 3)
#define OPT_l  (1 << 4)
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX_WAIT_SEC			(1 * 60)
#define BOARD_TYPE_MAX_LENTH    64
#define MAX_BOARD_NUM           50
#ifdef __UPGRADE_V2__
#define EXT_INFO_MAGIC_CODE		"EXTMAGIC"
#define VERSION_FIELD_LEN		32
#define CHECK_FIELD_LEN			32
#endif

#define UPGRADE_UNSTARTED           0			//未开启
#define UPGRADE_CHECKING            1			//校验升级包
#define UPGRADE_CHECKING_FAIL       2		    //校验失败
#define UPGRADE_WRITING             4			//写入中
#define UPGRADE_SOFTWARE_EXC        5		    //软件错误
#define UPGRADE_COMPLETE            9			//升级完成
#define NO_USE_BUZZER
#define USE_HASLASTWEEK	                        // 加上最后一周
#define MON_MIN		                1
#define MON_MAX		                12
#if !defined(USE_HASLASTWEEK)
#define WEEK_MIN                    1
#else
#define WEEK_MIN	                0
#endif
#define WEEK_MAX	                (4)
#define WDAY_MIN	                (0)
#define WDAY_MAX	                (6)
#define MDAY_MIN	                (1)
#define MDAY_MAX	                (31)
#define HOUR_MIN	                (0)
#define HOUR_MAX	                (23)
#define MIN_MIN		                (0)
#define MIN_MAX		                (59)
#define SEC_MIN		                (0)
#define SEC_MAX		                (59)
#define ADJ_MIN		                (1)
#define ADJ_MAX		                (60)
#define TPYE_TUTK	                (0)
#define TPYE_ULK	                (1)
#define TPYE_SHYUN	                (2)
#define EMAIL_LIMIT			        (256)
#define XDSL_STATE_DISCONNECTED		(0)
#define XDSL_STATE_CONNECTING		(1)
#define XDSL_STATE_CONNECTED		(2)
#define XDSL_STATE_DISCONNECTING	(3)
#define MSG_USER                    (0x0800)
#define MSG_CHECKDISK			    (MSG_USER + 1)
#define MSG_SHUTDOWN			    (MSG_USER + 2)
#define MSG_SHUTDOWN_TIMER		    (MSG_USER + 3)
#define MSG_REMOTE_SETID		    (MSG_USER + 4)
#define MSG_REMOTE_UPDATEOVER	    (MSG_USER + 5)
#define MSG_NETINITED			    (MSG_USER + 6)
#define MSG_SETOSDPOSITION		    (MSG_USER + 7)
#define MSG_IPCHANGED			    (MSG_USER + 8)
#define LOCAL_USER_ID	            (0xFFFFFFFF)
#define SYSTEM_STATUS_LED           (35)
#define LED_HZ                      (4)
#define MIN_SD_CAPTURE_SECOND_INTERVAL    (10)    // 最小抓图间隔
#define MIN_FTP_CAPTURE_SECOND_INTERVAL   (10)    // 最小FTP传图间隔
#define USR_BEEP_KEY		        (0)
#define USR_SERVER_KEY		        (1)
#define USR_EMAIL_KEY		        (2)
#define USR_GUI_KEY			        (3)
#define USR_ALARMOUT_KEY	        (4)
#define USR_PUSH_KEY		        (5)
#define USR_LIGHTALARM_KEY	        (6)
#define PASSWDLENGHT                (16)
#define BUFFERLEN                   (10)
#define MAXTIMELIMIT                ((6) * (60) * (60))
#define HOURTOSEC                   ((60) * (60))
#define DEBUGER_MODE	            (0)
#define PRINTFMODE	                (0)
#define true                        (1)
#define false                       (0)

#define DECRYPT_ZERO (0)

#define ENCRYPT_ONE                     (1)

#define INI_MAX_LINE                (512)               // the max characters in one line for ini file
#define AGING_TEST_MAX_TIME         (30*24*60)          // 最长老化时间30天
#define AGING_TEST_DEFAULT_TIME     (24*60)             // 默认老化时间1天
#define AGING_MIN_PERIODIC_TIME 	(24)				//老化周期最小时间（单位：s）
#define AGING_MAX_PERIODIC_TIME 	(60)				//老化周期最大时间（单位：s）

#define MAX_ENC_CHANNEL_NUM		    (1)
#define MAX_DEC_CHANNEL_NUM		    (1)
#define ALARM_NUM		            (16)

#define MAX_PTZ_CHANNEL_NUM	SYSTEM_SUPPORT_CHANNEL

#if (DEVICE_SUPPORT_PTZ_NUM >=3) 
//Multi Ptz Dev Manager
#define PTZ_MULTIDEV_CFG_SECTION         "MultiPtzDevCfg"
#define PTZ_L_HORZ_REVERSE               "PtzLeftHorzReverse"
#define PTZ_L_VERT_REVERSE               "PtzLeftVertReverse"
#define PTZ_R_HORZ_REVERSE               "PtzRightHorzReverse"
#define PTZ_R_VERT_REVERSE               "PtzRightVertReverse"
#define PTZ_LR_HORZ_MAXSTEP              "PtzLRHorzMaxStep"
#define PTZ_LR_VERT_MAXSTEP              "PtzLRVertMaxStep"
#define PTZ_LR_HORZ_MAXSPEED             "PtzLRHorzMaxSpeed"
#define PTZ_LR_VERT_MAXSPEED             "PtzLRVertMaxSpeed"
	#if (SYSTEM_SUPPORT_CHANNEL == 4)
	#define PTZ_MID		1
	#define PTZ_LERF 	2
	#define PTZ_RIGHT 	3
	#elif (SYSTEM_SUPPORT_CHANNEL == 3)
	#define PTZ_LERF 	1
	#define PTZ_RIGHT 	2
	#endif
#endif

#define PTZ_MAIN 	0



/***** 灯板类型 *****/
/* 普通光敏控红外灯板，(完全靠光敏）*/
// /* 高电平有效 */
#define LDR_2PIN_INFRARED_LIGHT_BOARD			(1)
/* 普通光敏控红外灯板加红外可开关（光敏+AGC控制并且可以关闭红外）*/
/* 高电平有效 */
#define LDR_2PIN_INFRARED_SWITCH_LIGHT_BOARD	(2)
/* 无光敏   		无极白光灯板 */
/* 高电平有效 */
#define NO_LDR_STEPLESS_WHITE_BOARD				(3)
/* 无光敏			无极红外灯板 */
/* 高电平有效 */
#define NO_LDR_STEPLESS_INFRARED_BOARD			(4)
/* 智能光敏控制双光源灯板（2根线与模组接，灯板一根IRCUT 输出，一根白光灯控制输入 */
/* 飞扬灯板低电平有效  				  华视安邦灯板高电平有效 					创安源灯板高电平有效 */
#define LDR_2PIN_LIGHT_BOARD					(5)
/* 智能光敏控制双光源灯板（3根线与模组接，灯板一根IRCUT 输出，一根白光灯控制输入，一根红外灯控制输入 ）触发可能是低/高，必须兼容普通红外模式 */
/* 安柯达灯板（带光敏）低电平有效 */
#define LDR_3PIN_LIGHT_BOARD					(6)
/* 无光敏控制双光源灯板，（2根线与模组接，一根白光灯控制输入，一根红外灯控制输入 ）触发可能是低/高，必须兼容普通红外模式 */
/* 安柯达灯板低电平有效 */
#define NO_LDR_2PIN_LIGHT_BOARD					(7)
/* 无光敏控制双光源灯板，（2根线与模组接，一根白光灯控制输入，一根红外灯控制输入 ）触发可能是低/高，必须兼容普通红外模式 */
/* 伟创达灯板高电平有效 */
#define NO_LDR_WCD_2PIN_LIGHT_BOARD				(8)
/* 摇头机灯板     	暂不定义 */
#define REVERSE_LIGHT_BOARD						(9)
/* 无光敏控制控制白灯，只有亮和灭 */
/* 贝斯德灯板高电平有效 */
#define NO_LDR_NORMAL_WHITE_LIGHT_BOARD			(10)
/* 无光敏控制双光源灯板，（2根线与模组接，一根白光灯控制输入，一根红外灯控制输入 ）触发是高 */
/* 华视安邦灯板高电平有效 */
#define NO_LDR_2PIN_LIGHT_BOARD_HSAB            (11)
//!< 桌面时间的ID
#define ID_TIMER_STATE		                    (201)
#define ID_TIMER_REBOOT		                    (202)
#define ID_TIMER_OSD			                (203)
#define REBOOT_WAIT_SECOND	                    (10)
//!< 桌面定时器速度
#define DESK_TIMER_SPEED	                    (50)
//!< 开机OSD显示的时间
#define OSD_SHOW_TIME		                    (3000)
#define IDC_BOTTOM_LAYER	                    (0x0FF)
#define IDC_LABEL_IP			                (0x100)
#define IDC_STATIC_IP		                    (0x101)
#define IDC_LABEL_NETMASK	                    (0x102)
#define IDC_STATIC_NETMASK	                    (0x103)
#define IDC_LABEL_GW		                    (0x104)
#define IDC_STATIC_GW		                    (0x105)
#define IDC_LABEL_MAC		                    (0x106)
#define IDC_STATIC_MAC		                    (0x107)
#define IDC_LABEL_HTTP		                    (0x108)
#define IDC_STATIC_HTTP		                    (0x109)
#define IDC_LABEL_WIFI		                    (0x10A)
#define IDC_STATIC_WIFI		                    (0x10B)
#define IDC_LABEL_PTZ_ADDR		                (IDC_LABEL_WIFI)
#define IDC_STATIC_PTZ_ADDR		                (IDC_LABEL_PTZ_ADDR + 1)
#define IDC_LABEL_PTZ_PROTOCOL	                (IDC_LABEL_PTZ_ADDR + 2)
#define IDC_STATIC_PTZ_PROTOCOL	                (IDC_LABEL_PTZ_ADDR + 3)
#define IDC_LABEL_PTZ_BAUDRATE	                (IDC_LABEL_PTZ_ADDR + 4)
#define IDC_STATIC_PTZ_BAUDRATE	                (IDC_LABEL_PTZ_ADDR + 5)
#define IDC_BORDER_W		                    (20)
#define IDC_LINE_H			                    (30)
#define IDC_BOTTOM_H1		                    (IDC_LINE_H * 6 + (IDC_BORDER_W << 1))
#define IDC_BOTTOM_H2		                    (IDC_LINE_H * 8 + (IDC_BORDER_W << 1))
#define IDC_LABEL_W			                    (100)
#define IDC_STATIC_W			                (120)
#define IDC_BOTTOM_W		                    (IDC_LABEL_W + IDC_STATIC_W + (IDC_BORDER_W << 1))
#define IDC_BOTTOM_X(sw)	                    (((sw) - IDC_BOTTOM_W) >> 1)
#define IDC_BOTTOM_Y(sh, bh)	                (((sh) - (bh)) >> 1)
#define MAX_INTERVAL_TIME                       (3)
#define MAXWORDLEN	                            (1024)
#define JAN_1970                                (2208988800UL)             // 1970 - 1900 in seconds

#define PROTOCOL_SETDNS_WAIT_DHCPSUCC_INTERVAL (10)


#endif
