#ifndef __COMMON_H__
#define	__COMMON_H__
#include <syslog.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/prctl.h>
#include <sys/time.h>
#include "defs.h"
#include "hisdk.h"
#include "mysystem.h"
#include "Authority.h"
#include "AIFrameType.h"
#include "vcomm.h"
#include "utils.h"
#include "LogManager.h"

#ifdef SYSTEM_SUPPORT_CHANNEL
//最大通道个数
#define MAX_CH_NUM                 (SYSTEM_SUPPORT_CHANNEL)
#else
#define MAX_CH_NUM					(1)
#endif

#if defined __ISP_IMAGE_DEBUG__
#define USE_ALGO	(0)
#define USE_VDA		(0)
#else
#define USE_ALGO (0)
#define USE_VDA (0)
#endif
#define USE_VISP	(0)

//收光参数调试宏
#define USE_BLACK_DEBUG (0)

//DEBUG 宏控制
/*
开启后需要在配置目录下创建SUPPORT_ALGO_DEBUG
亮度控制算法的灵敏度，同时亮度控制失效
饱和度控制算法的trackID多少帧检测不到认为是Die，同时饱和度控制失效
*/
#define SUPPORT_ALGO_DEBUG
#define ALGO_DEBUG_FLAG_FILE "AlgoDebugFlag"

#define AI_CHECK_DELAY_MS   (5 * 1000)  // 刚切换到夜晚时，延迟一段时间再查看AI状态，防止灯光切换带来的误检

#define BRIGHTNESS_CHECK_FREQ 	(1 * 1000 * 1000)
#define	SDK_INIT_MAGIC		    0x13145200

//Drag Zoom 倍数
#define DRAG_ZOOM_MIN_RATIO                     (1.0)
#if defined(__IS_SUPPORT_LONG_SHORT_FOCUS_ZOOM__)
#if defined (__PRODUCT_G43TA__)
//4+12mm镜头真实倍数
#define DRAG_ZOOM_Z_TO_S_RATIO                  (9.0)  /*这是真是的变倍倍数，不可修改 短*/
#define DRAG_ZOOM_S_TO_M_RATIO                  (4.0)  /*这是真是的变倍倍数，不可修改 中*/
#define DRAG_ZOOM_M_TO_L_RATIO                  (4.0)  /*这是真是的变倍倍数，不可修改 长*/
//4+6mm镜头真实倍数
#define DRAG_ZOOM_Z_TO_S_RATIO_4A6				(1.69)  /*这是真是的变倍倍数，不可修改 短*/
#define DRAG_ZOOM_S_TO_M_RATIO_4A6             	(4.0)  /*这是真是的变倍倍数，不可修改 中*/
#define DRAG_ZOOM_M_TO_L_RATIO_4A6           	(4.0)  /*这是真是的变倍倍数，不可修改 长*/
#else
//4+12mm镜头真实倍数
#define DRAG_ZOOM_Z_TO_S_RATIO                  (9.0)  /*这是真是的变倍倍数，不可修改 */
#define DRAG_ZOOM_S_TO_M_RATIO                  (4.0)  /*这是真是的变倍倍数，不可修改 */
#define DRAG_ZOOM_M_TO_L_RATIO                  (4.0)  /*这是真是的变倍倍数，不可修改 */
//4+6mm镜头真实倍数
#define DRAG_ZOOM_Z_TO_S_RATIO_4A6				(9.0)  /*这是真是的变倍倍数，不可修改 */
#define DRAG_ZOOM_S_TO_M_RATIO_4A6             	(4.0)  /*这是真是的变倍倍数，不可修改 */
#define DRAG_ZOOM_M_TO_L_RATIO_4A6           	(4.0)  /*这是真是的变倍倍数，不可修改 */

#endif
#endif

#if defined (__PRODUCT_P3MU__)
#define DRAG_ZOOM_MAX_RATIO                     (2.0)
#else
#define DRAG_ZOOM_MAX_RATIO                     (4.0)
#endif

#define DRAG_ZOOM_DEFAULT_RATIO             (6.0)  /* 未定制时候的默认值 */
#define DRAG_CMD_WAIT_TIME					(5000)  //命令执行完成后，清除状态等待时间 ms
#define DRAG_ZOOM_MAX_TIME					(2000000)	//最大倍数至最小倍数耗时
#define DRAG_ZOOM_RATIO_STEP                (0.2)

#define ALGO_MODELS_DIR						"/mnt/model/models"        //算法模型目录
#define UNAUTHORIZED_OSD_PATH               "/mnt/app/data/unauthorizedOsd.bin"

#ifdef __SUPPORT_PRODUCT_TEST_CUSTOM_SPEECH_FILES__
#define SPEECH_FILE_PATH				    "/mnt/speech"
#define SPEECH_FILE_PATH_OLD			    "/mnt/app/custom/speech/"
#endif
#define SPEECH_FILE_OTHER_PATH				"/mnt/app/custom/speech/other"

#define SPEECH_PREFIX                       ".amr"
#define SPEECH_MOTION_ALARM_CUSTOM_FILE			"/mnt/mtd/config/motionCustomAlarm.amr"
#define SPEECH_MOTION_ALARM_CUSTOM_FILE_TEMP	"/tmp/app/motionCustomAlarm.amr"

#define SPEECH_REMIND_SCHEDULE_CUSTOM_FILE1			"/mnt/mtd/config/remind_custom1.amr"
#define SPEECH_REMIND_SCHEDULE_CUSTOM_FILE1_TMP		"/tmp/app/remind_custom1.amr"
#define SPEECH_REMIND_SCHEDULE_CUSTOM_FILE2			"/mnt/mtd/config/remind_custom2.amr"
#define SPEECH_REMIND_SCHEDULE_CUSTOM_FILE2_TMP		"/tmp/app/remind_custom2.amr"
#define SPEECH_REMIND_SCHEDULE_CUSTOM_FILE3			"/mnt/mtd/config/remind_custom3.amr"
#define SPEECH_REMIND_SCHEDULE_CUSTOM_FILE3_TMP		"/tmp/app/remind_custom3.amr"

#define G711U_FRAME_SIZE 160 // G711U帧大小(20ms@8kHz)
#define AMRNB_FRAME_SIZE 32  // AMR最大帧大小

#define SPEECH_ALARM_CUSTOM_FILE_DIR			"/mnt/mtd/config/"
#define SPEECH_ALARM_CUSTOM_FILE_TEMP_DIR		"/tmp/app/"

#define CUSTOM_CONFIG_EX				"/mnt/mtd/config/custominfo.ini"
#define CUSTOM_CONFIG_EX_1				"/mnt/app/custom/info/custominfo.ini"

#define CUSTOM_CONFIG_FILE              "/mnt/mtd/config/customcfg.ini"
#define CUSTOM_ABILITY_INI_FILE			"/mnt/mtd/config/customability.ini"
#define AUTHORIZATION_CFG	            "/mnt/mtd/config/authorization_cfg.ini"

//产测标识文件(工厂模式=产测模式)
#define PRODUCTTEST_FILE            "/tmp/app/disk/mmcblk0p1/output/ProductTest"
#define PRODUCTTEST_WIFI_FILE		"/tmp/app/disk/mmcblk0p1/SdTypeNetCfg.ini"

//P2P发送数据失败标志文件
#define P2P_SEND_DATA_FAIL_FLAG     "/mnt/mtd/.P2PSendDataFail"

#define AMR_MAGIC_NUMBER			 	"#!AMR\n"

#define BLACK_LOGIC_TO_DAY_MS	10 * 60 * 1000  //黑光逻辑强制切换成白天 10Min
#define ALGO_THRESHOLD_NORMAL (0.5)             //检测灵敏度
#define ALGO_THRESHOLD_NIGHT (0.2)              //检测灵敏度，夜晚下

#define DID_CONF                 "/mnt/mtd/config/tutkuid.conf"
#define DID_CONF_BAK             "/mnt/mtd/config/tutkuid.conf_bak"
#define CUSTOM_CONFIG            "/mnt/mtd/config/config.ini"

/* 产生一个随机的序列号 */
#define GET_RANDOM_SERIAL_NUM   "cat /proc/sys/kernel/random/uuid| cksum | cut -f1 -d\" \""

// 海思 scene 路径
#define HISI_SCENEAUTO_PATH "/etc/sensor/scene"
#define HISI_SCENEAUTO_PATH_F38PA "/etc/sensor/scene/F38PA"
#define HISI_SCENEAUTO_PATH_JXK06A "/etc/sensor/scene/JXK06A"
#define HISI_SCENEAUTO_PATH_GC20C3 "/etc/sensor/scene/GC20C3"

#define SMARTP_VIFrame 10000

/*!< P2PShell可上传下载任意文件标记 协议发送开启Telnet命令后创建*/
#define P2PUDAnyFile "/tmp/.P2PShellUpDownLoadFile"

/*!< 停止图像的 autoScene*/
#define AutoSceneStopFlag "/mnt/mtd/auto_scene_stop"

#define CHECK_AUTHEN
#define GET_TUTK_UID

#define MAX_SPEECH_CACHE_NUM    (5)
#define MAX_SPEECH_FILE_SIZE    (128)

#define HI_FAILURE (-1)

#define SUPPORT_TEXT_OSD_TWO_LINE

#ifdef SUPPORT_TEXT_OSD_TWO_LINE
#define NEW_LINE_CHAR 0X5E			//换行符
#define ASC_WIDTH		8
#define HZ_WIDTH		16
#endif
#define TEXT_OSD_MAX_LEN 20
#define MAX_OSD_FLOW_DATA 99999 //客流统计最大叠加数据

#define 	HI_SUCCESS				(0)
#if defined(__PRODUCT_G84F__)||defined (__PRODUCT_W84F__) || defined (__PRODUCT_G63F__)|| defined (__PRODUCT_W63F__)
#define     AUDIO_AI_PTZ_MOVE       (65)    //云台转动过程中的音量
#else
#define     AUDIO_AI_PTZ_MOVE       (75)    //云台转动过程中的音量
#endif

#define		INIT_BPS_360P			(512)				//初始化360P码率(K)
#define		MAX_BPS_360P			(2048)				//最大360P码率(K)
#define		MAX_FPS_360P			(30)				//最大360P帧率

#define		INIT_BPS_D1			    (512)				//初始化D1码率(K)
#define		MAX_BPS_D1			    (2048)				//最大D1码率(K)
#define		MAX_FPS_D1			    (30)				//最大D1帧率

#define		INIT_BPS_512P			(2500)				//初始化512P码率(K)
#define		MAX_BPS_512P			(12288)				//最大512P码率(K)
#define		MAX_FPS_512P			(30)				//最大512P帧率

#define		INIT_BPS_HD720P			(2560)				//初始化720P码率(K)

#define		MAX_BPS_HD720P			(4096)				//最大720P码率(K)

#define		MAX_FPS_HD720P			(30)				//最大720P帧率

#define		AUX_MAX_FPS_HD720P		(25)				//子码流最大720P帧率

#define		INIT_BPS_QuadVGA		(2560)				//初始化960P码率(K)

#define		MAX_BPS_QuadVGA		    (12288)				//最大960P码率(K)

#define		MAX_FPS_QuadVGA		    (30)				//最大960P帧率

#define		INIT_BPS_HD1080P		(4096)				//初始化HD1080P码率(K)

#define		MAX_BPS_HD1080P		    (4096)				//最大HD1080P码率(K)

#define		MAX_FPS_HD1080P		    (30)				//最大HD1080P帧率

#define		INIT_BPS_QXGA			(4096)				//初始化QXGA码率(K)

#define		MAX_BPS_QXGA			(12288)				//最大QXGA码率(K)

#define		MAX_FPS_QXGA			(30)				//最大QXGA帧率

#define		INIT_BPS_UWXGA			(4096)				//初始化UWXGA码率(K)

#define		MAX_BPS_UWXGA			(4096)				//最大UWXGA码率(K) 300W

#define		MAX_FPS_UWXGA			(20)				//最大UWXGA帧率	300W

#define		INIT_BPS_5M				(4096)				//初始化5M码率(K)

#define		MAX_BPS_5M				(8192)				//最大5M码率(K)
#define		MAX_FPS_5M				(30)				//最大5M帧率

#define		MAX_FPS_5M_UNAUTH		(20)				//未激活时最大5M帧率

#define		INIT_BPS_4M				(4096)				//初始化4M码率(K)

#define		MAX_BPS_4M				(8192)				//最大4M码率(K)
#define		MAX_FPS_4M				(30)				//最大4M帧率
#define		MAX_FPS_4M_UNAUTH		(30)				//未激活时最大4M帧率

#define		MAX_BPS_8M				(8192)				//最大8M码率(K)
#define		MAX_FPS_8M				(20)				//最大8M帧率

#define		INIT_MAIN_BPS			(2048)				//初始化主码流码率(K)
#define		MAX_MAIN_BPS			(2048)				//最大主码流码率(K)

#define		MAX_MAIN_BPS_SPEC		(4096)				//最大主码流码率(K)

#define		INIT_AUX_BPS			(368)				//初始化子码流码率(K)
#define		MAX_AUX_BPS				(2048)				//最大子码流码率(K)

#define		INIT_THIRD_BPS			(368)				//初始化第三码流码率(K)
#define		MAX_THIRD_BPS			(2048)				//最大第三码流码率(K)
#define 	MAX_THIRD_FPS			(30)				//最大第三码流帧率

#define		CAPTURE_STREAM_MAX_FPS  (5)

#define		ENC_STREAM_BUFSIZE			(1024 * 512)	//编码流数据缓冲空间
#define		ENC_AUDIOSTREAM_BUFSIZE		(1024 * 1)		//音频编码流数据缓冲空间
#define		DEC_STREAM_BUFSIZE			(1024 * 256)	//解码流数据缓冲空间
#define		DEC_STREAM_BUFSIZE_SLAVE	(1024 * 256)	//解码流数据缓冲空间
#define		MAX_FRMAESIZE				(1024 * 200)	//最大的一帧Size

#define     CAP_OF_HDAUX_LEAVE      (640*360*8)        //如果子码流能力不够，则留360p的8fps给第三码流

#define     MAX_FIRST_STREAM_I_FRAME_SIZE    (700 * 1024)     
#define     MAX_SECOND_STREAM_I_FRAME_SIZE    (120 * 1024)     
#define     MAX_THIRD_STREAM_I_FRAME_SIZE    (120 * 1024)     

#define     MAIN_800W_FRAME_BUFFER_SIZE    	(1024 * 1024)//800W分辨率使用  
#define     AUX_800W_FRAME_BUFFER_SIZE    	(256 * 1024)

#define     MAIN_500W_FRAME_BUFFER_SIZE    	(768 * 1024)//500W分辨率使用  
#define     AUX_D1_FRAME_BUFFER_SIZE    	(128 * 1024)

#define     MAIN_400W_FRAME_BUFFER_SIZE    	(1024 * 1024)//4K暂时定为1M,后续再调
#define     AUX_400W_FRAME_BUFFER_SIZE    	(192 * 1024)

#define     THIRD_FRAME_BUFFER_SIZE  		(128 * 1024)//第二子码流
#define     MJPEG_FRAME_BUFFER_SIZE  		(128 * 1024)//MJEP流

#define     MAX_MAIN_FRAME          (30)

#define     MIN_AUX_WIDTH           (352)
#define     MIN_AUX_HEIGHT          (288)
#define     MAX_AUX_FRAME           (30)
#define     MAX_THIRD_WIDTH         (704)
#define     MAX_THIRD_HEIGH         (576)
#define     MAX_THIRD_FRAME         (30)

#define     MAX_FOURTH_FRAME         (30)

#define     MD_AREA_WIDTH           (352)
#define     MD_AREA_HEIGH           (288)

#define     VPE2_MAX_WIDTH           (800)
#define     VPE2_MAX_HEIGH           (448)

#define     VSIZE_4K_WIDTH           (3840)
#define     VSIZE_4K_HEIGHT          (2160)

#define     VSIZE_6M_WIDTH           (3200)
#define     VSIZE_6M_HEIGHT          (1800)

#define     VSIZE_5M_WIDTH           (2880)
#define     VSIZE_5M_HEIGHT          (1620)

#define     VSIZE_4M_WIDTH           (2560)
#define     VSIZE_4M_HEIGHT          (1440)

#define     VSIZE_3M_WIDTH           (2304)
#define     VSIZE_3M_HEIGHT          (1296)

#define     VSIZE_1080P_WIDTH        (1920)
#define     VSIZE_1080P_HEIGHT       (1080)

#define     VSIZE_1600_WIDTH        (1600)
#define     VSIZE_900_HEIGHT        (900)

#define     VSIZE_1408_WIDTH        (1408)
#define     VSIZE_792_HEIGHT        (792)

#define     VSIZE_720P_WIDTH         (1280)
#define     VSIZE_720P_HEIGHT        (720)

#define     VSIZE_D1_WIDTH         	 (704)
#define     VSIZE_D1_HEIGHT        	 (576)

#define     VSIZE_448P_WIDTH         (800)
#define     VSIZE_448P_HEIGHT        (448)

#define     VSIZE_360P_WIDTH         (640)
#define     VSIZE_360P_HEIGHT        (360)

#define     MAIN_STREAM_WIDTH       (VSIZE_5M_WIDTH) 
#define     MAIN_STREAM_HEIGHT      (VSIZE_5M_HEIGHT)
#define     AUX_STREAM_WIDTH        (VSIZE_448P_WIDTH)
#define     AUX_STREAM_HEIGHT       (VSIZE_448P_HEIGHT)

#if defined (__PRODUCT_P4__)||defined (__PRODUCT_G43TA__)||defined (__PRODUCT_P4U__)||defined (__PRODUCT_P3MU__)
#define     ALGO_STREAM_WIDTH       (320)
#define     ALGO_STREAM_HEIGHT      (192)
#define     ALGO_STREAM_FPS         (20)
#define     ALGO_CHANGE_THR_FRM     (15)//调整检测灵敏度，慢快门帧率阈值
#else
#define     ALGO_STREAM_WIDTH       (768)
#define     ALGO_STREAM_HEIGHT      (448)
#define     ALGO_STREAM_FPS         (20)
#define     ALGO_CHANGE_THR_FRM     (15)//调整检测灵敏度，慢快门帧率阈值
#endif

#ifdef SYSTEM_SUPPORT_HD_ALGO_CHANNEL
//最大通道个数
#define ALGO_CHANNEL                 (SYSTEM_SUPPORT_HD_ALGO_CHANNEL)
#else
#define ALGO_CHANNEL					(1)
#endif

//子码流抓图质量各等级对应的值
#define     AUX_CAPTURE_QUALITY_HIGH   (75)
#define     AUX_CAPTURE_QUALITY_MID    (50)
#define     AUX_CAPTURE_QUALITY_LOW    (25)

//主码流抓图质量各等级对应的值
#define     MAIN_CAPTURE_QUALITY_HIGH   (30)
#define     MAIN_CAPTURE_QUALITY_MID    (20)
#define     MAIN_CAPTURE_QUALITY_LOW    (10)

#define     ENC_SUB_STREAM_BUF_SIZE   	(128 * 1024)

#if defined(__PRODUCT_P3MU__)
#define SENSOR_REAL_WIDTH (VSIZE_1080P_WIDTH)
#define SENSOR_REAL_HEIGHT (VSIZE_1080P_HEIGHT)
#define MAIN_STREAM_WIDTH_MAX (VSIZE_3M_WIDTH)
#define MAIN_STREAM_HEIGHT_MAX (VSIZE_3M_HEIGHT)
#define AUX_STREAM_WIDTH_MAX (240)
#define AUX_STREAM_HEIGHT_MAX (320)
#define MAX_FPS (20)
#define ENC_MAIN_STREAM_BUF_SIZE (512 * 1024)
#define MAIN_STREAM_VBR_MAX_BITRATE (1024)
#define AUX_STREAM_VBR_MAX_BITRATE (512)
#endif

#define 	MAX_MAIN_FPS				(30)
#define 	MAX_AUX_FPS					(30)
#define 	MAX_FPS_WIFI				(15)    //wifi下最大帧率

#define     SLOWFRAME_ENCODER_RATE      5
#define     SLOWFRAME_SWITCH_INTERVAL   240
#define     SLOWFRAME_GOP_RATIO         8

//qp相关(需要保证min_iqp < min_qpvenc)
#define     MAIN_STREAM_MAX_QP     		(51)
#define     MAIN_STREAM_MAX_IQP     	(51)
#define     AUX_STREAM_MAX_QP     		(51)
#define     AUX_STREAM_MAX_IQP     	    (51)


#if defined (__PRODUCT_P4__) || defined (__PRODUCT_P3MU__)||defined (__PRODUCT_P4U__)
#define     MAIN_STREAM_MIN_QP     		(30)
#define     MAIN_STREAM_MIN_IQP     	(29)
#define     AUX_STREAM_MIN_QP     		(30)
#define     AUX_STREAM_MIN_IQP     	    (29)
#else
#define     MAIN_STREAM_MIN_QP     		(32)
#define     MAIN_STREAM_MIN_IQP     	(30)
#define     AUX_STREAM_MIN_QP     		(31)
#define     AUX_STREAM_MIN_IQP     	    (29)

#endif

//主子码流限制在4G移动卡下的minQP
#define     MAIN_STREAM_MIN_QP_IN_CHINA_MOBILE     		(32)
#define     MAIN_STREAM_MIN_IQP_IN_CHINA_MOBILE     	(31)
#define     AUX_STREAM_MIN_QP_IN_CHINA_MOBILE     		(32)
#define     AUX_STREAM_MIN_IQP_IN_CHINA_MOBILE     	    (30)
//主子码流在4G移动卡下的最大码率（KB）
#define     MAIN_STREAM_MAX_BITRATE_IN_CHINA_MOBILE     (350)
#define     AUX_STREAM_MAX_BITRATE_IN_CHINA_MOBILE      (250)

//主子码流限制在产测模式下的minQP
#define     MAIN_STREAM_MIN_QP_IN_PRODUCT_MODE     		(32)
#define     MAIN_STREAM_MIN_IQP_IN_PRODUCT_MODE     	(31)
#define     AUX_STREAM_MIN_QP_IN_PRODUCT_MODE     		(32)
#define     AUX_STREAM_MIN_IQP_IN_PRODUCT_MODE     	    (30)
//主子码流在产测模式下的最大码率（KB）
#define     MAIN_STREAM_MAX_BITRATE_IN_PRODUCT_MODE     (350)
#define     AUX_STREAM_MAX_BITRATE_IN_PRODUCT_MODE      (250)
#define     FOURTH_STREAM_FPS       (5)

//人形探测使用的分辨率
#define     HUMAN_DETECT_W           (ALGO_STREAM_WIDTH)
#define     HUMAN_DETECT_H           (ALGO_STREAM_HEIGHT)
#define     MIN(a,b) ((a) < (b) ? (a) : (b))

#define     VIDEO_WIDTH             (352)
#define     VIDEO_HEIGHT            (288)
#define     TALKINGSIZE             (320 * 4)

#define MAIN_STREAM_LINE_THICK	4
#define SUB_STREAM_LINE_THICK	2  

/***** 灯板类型 *****/
/* 普通光敏控红外灯板，(完全靠光敏）*/
/* 高电平有效 */
#define LDR_2PIN_INFRARED_LIGHT_BOARD			1

/* 普通光敏控红外灯板加红外可开关（光敏+AGC控制并且可以关闭红外）*/
/* 高电平有效 */
#define LDR_2PIN_INFRARED_SWITCH_LIGHT_BOARD	2

/* 无光敏   		无极白光灯板 */
/* 高电平有效 */
#define NO_LDR_STEPLESS_WHITE_BOARD				3

/* 无光敏			无极红外灯板 */
/* 高电平有效 */
#define NO_LDR_STEPLESS_INFRARED_BOARD			4

/* 智能光敏控制双光源灯板（2根线与模组接，灯板一根IRCUT 输出，一根白光灯控制输入 */
/* 飞扬灯板低电平有效  				  华视安邦灯板高电平有效 					创安源灯板高电平有效 */
#define LDR_2PIN_LIGHT_BOARD					5

/* 智能光敏控制双光源灯板（3根线与模组接，灯板一根IRCUT 输出，一根白光灯控制输入，一根红外灯控制输入 ）触发可能是低/高，必须兼容普通红外模式 */
/* 安柯达灯板（带光敏）低电平有效 */
#define LDR_3PIN_LIGHT_BOARD					6

/* 无光敏控制双光源灯板，（2根线与模组接，一根白光灯控制输入，一根红外灯控制输入 ）触发可能是低/高，必须兼容普通红外模式 */
/* 安柯达灯板低电平有效 */
#define NO_LDR_2PIN_LIGHT_BOARD					7

/* 无光敏控制双光源灯板，（2根线与模组接，一根白光灯控制输入，一根红外灯控制输入 ）触发可能是低/高，必须兼容普通红外模式 */
/* 伟创达灯板高电平有效 */
#define NO_LDR_WCD_2PIN_LIGHT_BOARD				8

/* 摇头机灯板     	暂不定义 */
#define REVERSE_LIGHT_BOARD						9

/* 无光敏控制控制白灯，只有亮和灭 */
/* 贝斯德灯板高电平有效 */
#define NO_LDR_NORMAL_WHITE_LIGHT_BOARD			10

/* 无光敏控制双光源灯板，（2根线与模组接，一根白光灯控制输入，一根红外灯控制输入 ）触发是高 */
/* 华视安邦灯板高电平有效 */
#define NO_LDR_2PIN_LIGHT_BOARD_HSAB    11

#define SPEECH_BIT      (0x0)
#define TALK_BIT        (0x1)
#define SPEECH_ENABLE   (0x1<<(SPEECH_BIT))
#define TALK_ENABLE     (0x1<<(TALK_BIT))

#define AUDIO_G711_ONE_FRAME_SIZE 320

#define PRTCOLOR
//#define DEBUG
#define ERROR
//#define ASSERT

//网卡模式
typedef enum {
	eNicWireless=0,	//无线
	eNicWire=1,	//有线
	eNic4G=2,	//4G
	eNicend,
}nic_mode_t;

typedef enum
{
	FIRST_SENSOR = 0,
#if (SYSTEM_SUPPORT_CHANNEL >= 2)
	SEC_SENSOR = 1,
#endif
#if (SYSTEM_SUPPORT_CHANNEL >= 3)
	THIRD_SENSOR = 2,
#endif
#if (SYSTEM_SUPPORT_CHANNEL >= 4)
	FOURTH_SENSOR = 3,
#endif
    SUPPORT_MAX_SENSOR_COUNT
}DEV_SUPPORT_SENSOR;

typedef enum {
	ZW_ERROR_LEVEL = 0,
	ZW_INFO_LEVEL  = 1,	
	ZW_DEBUG_LEVEL = 2,
} zw_log_level;

typedef enum hiENCODE_STREAM_E
{
    FIRST_STREAM = 0,
    SECOND_STREAM,
    THIRD_STREAM,
    FOURTH_STREAM,
    //CAPTURE_STREAM,
    AI_STREAM,
    MAX_STREAM_COUNT,
} ENCODE_STREAM_E;

typedef enum
{
    STREAM_CALLBACK_MAIN        		= 0,
    STREAM_CALLBACK_SUB         		= 1,
    STREAM_CALLBACK_MJPEG       		= 2,
    STREAM_CALLBACK_SNAP        		= 3,

    STREAM_CALLBACK_MAIN_SEC    		= 4,
    STREAM_CALLBACK_SUB_SEC    			= 5,
    STREAM_CALLBACK_MJPEG_SEC  	 		= 6,
    STREAM_CALLBACK_SNAP_SEC    		= 7,

    STREAM_CALLBACK_MAIN_THIRD  		= 8,
    STREAM_CALLBACK_SUB_THIRD   		= 9,
    STREAM_CALLBACK_MJPEG_THIRD 		= 10,
    STREAM_CALLBACK_SNAP_THIRD  		= 11,

    STREAM_CALLBACK_MAIN_FOURTH 		= 12,
    STREAM_CALLBACK_SUB_FOURTH  		= 13,
    STREAM_CALLBACK_MJPEG_FOURTH 		= 14,
    STREAM_CALLBACK_SNAP_FOURTH  		= 15,

    STREAM_CALLBACK_AUDIO       		= 16,
    STREAM_CALLBACK_ALGO        		= 17,
    STREAM_CALLBACK_HUMANSHAPE  		= 18,
    STREAM_CALLBACK_HUMANSHAPE_SEC  	= 19,
    STREAM_CALLBACK_HUMANSHAPE_THIRD  	= 20,
    STREAM_CALLBACK_HUMANSHAPE_FOURTH  	= 21,
    STREAM_CALLBACK_MOTION      		= 22,
    STREAM_CALLBACK_MOTION_SEC  		= 23,
    STREAM_CALLBACK_MOTION_THIRD  		= 24,
    STREAM_CALLBACK_MOTION_FOURTH  		= 25,

    STREAM_CALLBACK_MAX 				= 0xFF,
} STREAM_CALLBACK_E;

typedef enum _XRS_FRAME_TYPE_E
{
    I_FRAME = 0,
    P_FRAME = 1,
    B_FRAME = 2,
} ENCODE_FRAME_TYPE_E;

typedef enum _XRS_BITRATE_CTRLMODE_E
{
	ENCODE_BITRATE_CTRLMODE_VBR  = 0,
	ENCODE_BITRATE_CTRLMODE_CBR  = 1,
	ENCODE_BITRATE_CTRLMODE_AVBR = 2,
} ENCODE_BITRATE_CTRLMODE_E;

typedef enum _XRS_ENCODE_TYPE_E
{
    ENCODE_TYPE_H264 = 0,
    ENCODE_TYPE_H265 = 1,
    ENCODE_TYPE_MJPEG = 2,
    ENCODE_TYPE_JPEG = 3,
} ENCODE_TYPE_E;

//jpeg质量等级
typedef enum _XRS_JPEG_QUALITY_E
{
    JPEG_QUALITY_HIGH = 0,
    JPEG_QUALITY_MID = 1,
    JPEG_QUALITY_LOW = 2,
} JPEG_QUALITY_E;

typedef enum
{
    HI_FALSE = 0,
    HI_TRUE = 1,
} HI_BOOL;

#ifdef PRTCOLOR
#define NONE         "\033[m"  
#define RED          "\033[0;32;31m"
#define LIGHT_RED    "\033[1;31m"
#define GREEN        "\033[0;32;32m"
#define LIGHT_GREEN  "\033[1;32m"
#define BLUE         "\033[0;32;34m"
#define LIGHT_BLUE   "\033[1;34m"
#define DARY_GRAY    "\033[1;30m"
#define CYAN         "\033[0;36m"
#define LIGHT_CYAN   "\033[1;36m"
#define PURPLE       "\033[0;35m"
#define LIGHT_PURPLE "\033[1;35m"
#define BROWN        "\033[0;33m"
#define YELLOW       "\033[1;33m"
#define LIGHT_GRAY   "\033[0;37m"
#define WHITE        "\033[1;37m"
#else
#define NONE
#define RED
#define LIGHT_RED
#define GREEN
#define LIGHT_GREEN
#define BLUE
#define LIGHT_BLUE
#define DARY_GRAY
#define CYAN
#define LIGHT_CYAN
#define PURPLE
#define LIGHT_PURPLE
#define BROWN
#define YELLOW
#define LIGHT_GRAY
#define WHITE
#endif

#ifndef PRT_TID
#define PRT_TID()\
           ({prctl(PR_SET_NAME, __FUNCTION__, 0, 0, 0);\
             FILE *tid_log_fp = NULL;\
             tid_log_fp = fopen("/tmp/.tid_log", "a");\
             fprintf(tid_log_fp, "[%s:%d] thread: %s tid: %ld\n", __FILE__, __LINE__, __FUNCTION__, syscall(SYS_gettid));\
             fclose(tid_log_fp);\
             tid_log_fp = NULL;\
           })

#define PRT_TID_NAME(_NAME_)\
           ({prctl(PR_SET_NAME, _NAME_, 0, 0, 0);\
             FILE *tid_log_fp = NULL;\
             tid_log_fp = fopen("/tmp/.tid_log", "a");\
             fprintf(tid_log_fp, "[%s:%d] thread: %s tid: %ld\n", __FILE__, __LINE__, __FUNCTION__, syscall(SYS_gettid));\
             fclose(tid_log_fp);\
             tid_log_fp = NULL;\
           })
#endif

#define ZW_ERROR(format...)  LOG_E(LOG_MODULE_ZWSDK, format)
#define ZW_DEBUG(format...)  LOG_D(LOG_MODULE_ZWSDK, format)
#define ZW_INFO(format...)   LOG_I(LOG_MODULE_ZWSDK, format)
#define ZW_PRINTF(format...) LOG_W(LOG_MODULE_ZWSDK, format)
#define SDKPRINT(format...)  LOG_I(LOG_MODULE_OPEN, format)

/* 断言判断: DBG_ASSERT(条件,返回值,打印)
 * 条件成立直接运行，否则则表示断言成功，打印相关信息和返回值，直接返回返回值
 */
#ifdef ASSERT
#define DBG_ASSERT(condition, ret, fmt, args...) CHECK_RTNP(!condition, ret, fmt, ##args)
#else
#define DBG_ASSERT(condition, ret, fmt, args...) NULL
#endif

#define	HIEASY_FREE(a)				\
{									\
    if ((a) != NULL)				\
    {								\
        free((a));					\
        (a) = NULL;					\
    }								\
}

#define FRAME_START_CODE 0xB7010000

typedef struct tagH264DataPacketHeader
{
    unsigned int        uDataPacketStartCode; //0xB7010000
    unsigned int        uFrameNo;
    unsigned char       byFrameType;
    unsigned char       byLoadType;
    unsigned int        uShowTime;
    unsigned long       u32RealTimeSec;
    unsigned long       u32RealTimeUSec;
    unsigned short      uReserved;
    unsigned int        uOverloadLen;
}__attribute__((packed)) H264DATAPACKETHEADER, *PH264DATAPACKETHEADER;

/* 自定义读写锁实现 */
typedef enum 
{
    HY_UNLOCK		= 0,
    HY_RDLOCK		= 1,
    HY_WRLOCK		= 2,
}HY_RWLockState_t;

typedef struct
{
    pthread_mutex_t		m_Lock;
    UINT32				m_ReadLockNum;
    UINT32				m_WriteLockNum;
    HY_RWLockState_t	m_LockState;
}HY_RWLodk_t;

typedef enum
{
	ISNIGHT = 0,//夜晚
	ISDAY = 1,//白天
}DAYNIGHT_T;

typedef enum                               
{                                          
    IRCUT_STATUS_OFF   = 0, /*关闭 IRCUT*/ 
    IRCUT_STATUS_DAY   = 1, /*白天模式 */  
    IRCUT_STATUS_NIGHT = 2, /*夜晚模式*/   
}IRCUT_STATUS_E;

typedef enum                               
{                                          
    AUDIO_DATA_TYPE_VALID    = 0, /*有效数据*/
    AUDIO_DATA_TYPE_INVALID  = 1, /*无效数据*/
    AUDIO_DATA_TYPE_FILE_END = 2, /*音频文件尾*/
}AUDIO_DATA_TYPE_E;
	
typedef enum
{
	EXTINGUISH_LOW = 0,//弱收光
	EXTINGUISH_MIDDLE = 1,//中等收光
	EXTINGUISH_HIGH = 2,//强收光
	EXTINGUISH_END = 3,//END
}ISPAELEVEL_T;

/* 线程管理资源 */
typedef struct
{
    int				m_Exit;
    pthread_t		m_ThreadId;			//线程ID
    int				m_Pri;
}ThreadInfo_t;

/* 流通道信息管理 */
typedef struct
{
    int				m_PortNum;		//通道编号
    char			*pBuf;			//缓冲区
    int				BufSize;		//缓冲区大小
    int				ReadPos;        // 读取位置，当为普通缓冲区时为0
    int				WritePos;       // 写入位置，当为普通缓冲区使用时为当前缓冲区数据大小
    int				LastReadSize;	//上次读取的数据Size
}StreamInfo_t;

typedef struct hiRECT_S
{
    unsigned int s32X;
    unsigned int s32Y;
    unsigned int u32Width;
    unsigned int u32Height;
} RECT_S;

typedef enum
{
	STATUS_NORMAL = 0,
	STATUS_PRODUCT_TEST,            //产测
	STATUS_CHINAMOBILE,             //4G移动网络下
	STATUS_SEND_DATA_FAIL,          //协议告知发送数据失败
}ADJUST_ENCPRAM_STATUS;

//---------------------------------------------------------------------------------------------------
//	12时制：2008-06-06 AM 09:28:32			24时制：2008-06-06 09:28:32
//	每个字母看成一个宏块，
//---------------------------------------------------------------------------------------------------
typedef struct
{
    RECTH			rect;					//显示区域
    int				FontWidth;				//字符宽度
    int				FontHeight;				//字符高度
    int				bHour24;				//是否24小时制
    OSDTimeMode_t	DisMod;					//0,表示年月日, =1，表示月日年
    char			*pBlackFontInfo;		//15个关键字的点阵(黑色)
    char			*pWhiteFontInfo;		//15个关键字的点阵(白色)
}SysTimeOSD_t;

typedef struct
{
    RECTH			rect;					//显示区域
    int             nFontNum;               //字符个数
    int				FontWidth;				//字符宽度
    int				FontHeight;				//字符高度
    char			*pBlackFontInfo;		//13个关键字的点阵(黑色)
    char			*pWhiteFontInfo;		//13个关键字的点阵(白色)
}ZoomOSD_t;

#ifdef __DIGITAL_ZOOM_DRAG_SUPPORT__
typedef struct __PIXEL_ZOOM_OSD_T_
{
	unsigned char w;			//宽的像素数
	unsigned char h;			//高的像素数
	unsigned char bytes;		//宽的字节数
	unsigned char pixel_buf[12 * 2 * 32];	//点阵 16x32最大
} PIXEL_ZOOM_OSD_T;

static const PIXEL_ZOOM_OSD_T DEFAULT_ZOOMOSD_PIXEL = {
	/* 0 */

		 /*------------------------------------------------------------------------------
        ;  若数据乱码，请检查字模格式设置，注意选择正确的取模方向和字节位顺序。
        ;  源文件 / 文字 : 0
        ;  宽×高（像素）: 8×16
        ;  字模格式/大小 : 单色点阵液晶字模，横向取模，字节正序/16字节
        ;  数据转换日期  : 2009-4-22 15:26:12   0x00, 0x00, 0x00, 0x38, 0x44, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x44, 0x38, 0x00, 0x00,
        ------------------------------------------------------------------------------*/
	0x08, 0x10, 0x01,			//宽的像素数,高的像素数，宽的字节数，参数设置可选
	{
	0x0, 0x0, 0x38, 0x6c, 0xc6, 0xc6, 0xd6, 0xd6, 0xc6, 0xc6, 0x6c, 0x38, 0x0, 0x0, 0x0, 0x0,
	/* 1 */

		 /*------------------------------------------------------------------------------
        ;  若数据乱码，请检查字模格式设置，注意选择正确的取模方向和字节位顺序。
        ;  源文件 / 文字 : 1
        ;  宽×高（像素）: 8×16
        ;  字模格式/大小 : 单色点阵液晶字模，横向取模，字节正序/16字节
        ;  数据转换日期  : 2009-4-22 15:26:12
        ------------------------------------------------------------------------------*/
	0x0, 0x0, 0x18, 0x38, 0x78, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x7e, 0x0, 0x0, 0x0, 0x0,
	/* 2 */

		 /*------------------------------------------------------------------------------
        ;  若数据乱码，请检查字模格式设置，注意选择正确的取模方向和字节位顺序。
        ;  源文件 / 文字 : 2
        ;  宽×高（像素）: 8×16
        ;  字模格式/大小 : 单色点阵液晶字模，横向取模，字节正序/16字节
        ;  数据转换日期  : 2009-4-22 15:26:12
        ------------------------------------------------------------------------------*/
	0x0, 0x0, 0x7c, 0xc6, 0x6, 0xc, 0x18, 0x30, 0x60, 0xc0, 0xc6, 0xfe, 0x0, 0x0, 0x0, 0x0,
	/* 3 */

		 /*------------------------------------------------------------------------------
        ;  若数据乱码，请检查字模格式设置，注意选择正确的取模方向和字节位顺序。
        ;  源文件 / 文字 : 3
        ;  宽×高（像素）: 8×16
        ;  字模格式/大小 : 单色点阵液晶字模，横向取模，字节正序/16字节
        ;  数据转换日期  : 2009-4-22 15:26:12
        ------------------------------------------------------------------------------*/
	0x0, 0x0, 0x7c, 0xc6, 0x6, 0x6, 0x3c, 0x6, 0x6, 0x6, 0xc6, 0x7c, 0x0, 0x0, 0x0, 0x0,
	/* 4 */

		 /*------------------------------------------------------------------------------
        ;  若数据乱码，请检查字模格式设置，注意选择正确的取模方向和字节位顺序。
        ;  源文件 / 文字 : 4
        ;  宽×高（像素）: 8×16
        ;  字模格式/大小 : 单色点阵液晶字模，横向取模，字节正序/16字节
        ;  数据转换日期  : 2009-4-22 15:26:12
        ------------------------------------------------------------------------------*/
	0x0, 0x0, 0xc, 0x1c, 0x3c, 0x6c, 0xcc, 0xfe, 0xc, 0xc, 0xc, 0x1e, 0x0, 0x0, 0x0, 0x0,
	/* 5 */

		 /*------------------------------------------------------------------------------
        ;  若数据乱码，请检查字模格式设置，注意选择正确的取模方向和字节位顺序。
        ;  源文件 / 文字 : 5
        ;  宽×高（像素）: 8×16
        ;  字模格式/大小 : 单色点阵液晶字模，横向取模，字节正序/16字节
        ;  数据转换日期  : 2009-4-22 15:26:12
        ------------------------------------------------------------------------------*/
	0x0, 0x0, 0xfe, 0xc0, 0xc0, 0xc0, 0xfc, 0x6, 0x6, 0x6, 0xc6, 0x7c, 0x0, 0x0, 0x0, 0x0,
	/* 6 */

		 /*------------------------------------------------------------------------------
        ;  若数据乱码，请检查字模格式设置，注意选择正确的取模方向和字节位顺序。
        ;  源文件 / 文字 : 6
        ;  宽×高（像素）: 8×16
        ;  字模格式/大小 : 单色点阵液晶字模，横向取模，字节正序/16字节
        ;  数据转换日期  : 2009-4-22 15:26:12
        ------------------------------------------------------------------------------*/
	0x0, 0x0, 0x38, 0x60, 0xc0, 0xc0, 0xfc, 0xc6, 0xc6, 0xc6, 0xc6, 0x7c, 0x0, 0x0, 0x0, 0x0,
	/* 7 */

		 /*------------------------------------------------------------------------------
        ;  若数据乱码，请检查字模格式设置，注意选择正确的取模方向和字节位顺序。
        ;  源文件 / 文字 : 7
        ;  宽×高（像素）: 8×16
        ;  字模格式/大小 : 单色点阵液晶字模，横向取模，字节正序/16字节
        ;  数据转换日期  : 2009-4-22 15:26:12
        ------------------------------------------------------------------------------*/
	0x0, 0x0, 0xfe, 0xc6, 0x6, 0x6, 0xc, 0x18, 0x30, 0x30, 0x30, 0x30, 0x0, 0x0, 0x0, 0x0,
	/* 8 */

		 /*------------------------------------------------------------------------------
        ;  若数据乱码，请检查字模格式设置，注意选择正确的取模方向和字节位顺序。
        ;  源文件 / 文字 : 8
        ;  宽×高（像素）: 8×16
        ;  字模格式/大小 : 单色点阵液晶字模，横向取模，字节正序/16字节
        ;  数据转换日期  : 2009-4-22 15:26:12
        ------------------------------------------------------------------------------*/
	0x0, 0x0, 0x7c, 0xc6, 0xc6, 0xc6, 0x7c, 0xc6, 0xc6, 0xc6, 0xc6, 0x7c, 0x0, 0x0, 0x0, 0x0,
	/* 9 */

		/*------------------------------------------------------------------------------
        ;  若数据乱码，请检查字模格式设置，注意选择正确的取模方向和字节位顺序。
        ;  源文件 / 文字 : 9
        ;  宽×高（像素）: 8×16
        ;  字模格式/大小 : 单色点阵液晶字模，横向取模，字节正序/16字节
        ;  数据转换日期  : 2009-4-22 15:26:12
        ------------------------------------------------------------------------------*/
	0x0, 0x0, 0x7c, 0xc6, 0xc6, 0xc6, 0x7e, 0x6, 0x6, 0x6, 0xc, 0x78, 0x0, 0x0, 0x0, 0x0,
	/* X */

		/*------------------------------------------------------------------------------
        ;  若数据乱码，请检查字模格式设置，注意选择正确的取模方向和字节位顺序。
        ;  源文件 / 文字 : X
        ;  宽×高（像素）: 8×16
        ;  字模格式/大小 : 单色点阵液晶字模，横向取模，字节正序/16字节
        ;  数据转换日期  : 2009-4-22 15:26:12
        ------------------------------------------------------------------------------*/
	0x0, 0x0, 0xc6, 0xc6, 0x6c, 0x7c, 0x38, 0x38, 0x7c, 0x6c, 0xc6, 0xc6, 0x0, 0x0, 0x0, 0x0,
    /* . */

        /*------------------------------------------------------------------------------
        ;  若数据乱码，请检查字模格式设置，注意选择正确的取模方向和字节位顺序。
        ;  源文件 / 文字 : .
        ;  宽×高（像素）: 8×16
        ;  字模格式/大小 : 单色点阵液晶字模，横向取模，字节正序/16字节
        ;  数据转换日期  : 2009-4-22 15:26:12
        ------------------------------------------------------------------------------*/
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x18, 0x18, 0x0, 0x0, 0x0, 0x0,
	
	/*   */
	
		 /*------------------------------------------------------------------------------
		;  若数据乱码，请检查字模格式设置，注意选择正确的取模方向和字节位顺序。
		;  源文件 / 文字 : 
		;  宽×高（像素）: 8×16
		;  字模格式/大小 : 单色点阵液晶字模，横向取模，字节正序/16字节
		;  数据转换日期  : 2009-4-22 15:26:12
		------------------------------------------------------------------------------*/
	0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
	}
};
#endif

/*编码切换时暂停用户功能标志*/
typedef struct
{
	int restartMdFlag;
	int restartTimeOsdFlag;
	int restartTextOsdFlag;
	int restartZoomOsdFlag;
}UserSetParamFlag_t;

typedef enum{
    SLOWFRAME_SCENE_NONE = 0,
    SLOWFRAME_SCENE_FORCESLOW = 1,
    SLOWFRAME_SCENE_TRIGGERALARM = 2,
    SLOWFRAME_SCENE_OPENSTREAM = 3,
    SLOWFRAME_SCENE_CLOSESTREAM = 4,
}eSLOWFRAMESCENE;

typedef enum 
{
    SameAsMainStream   	= 0x00,	
    SameAsAuxStream     = 0x01,
	SameAsThirdStream   = 0x02,
} CaptureType_t;

/* 系统资源 */
typedef struct
{
    /* 基本信息(软件不可配置) */
    int                 m_BoardInfo;
    char                pTUTKUID[64];
    int					m_TotalAudioEncChannel;	//支持的音频编码通道总数
    int					m_TotalAudioDecChannel;	//支持的音频解码通道总数
    int					m_TotalTalkingChannel;	//支持的语音对讲的通道总数
    unsigned char       m_szSerialNum[MAX_SERIAL_NUM_LEN]; //序列号

    /* 线程资源 */
    ThreadInfo_t		m_OSDUpdateThread;		//OSD刷新线程
    ThreadInfo_t		m_AuioDecThread;		 //音频解码线程
    ThreadInfo_t		m_AuioInThread;		 	//音频输入线程
    ThreadInfo_t		m_IRCUTSwitchThread;	//IRCUT切换线程   
    ThreadInfo_t 		m_CheckBrightnessThread;//亮度检测线程
    ThreadInfo_t		m_CheckSecretPinStatusThread; // IP复位检测
    ThreadInfo_t		m_TimerThread;
    ThreadInfo_t		m_CheckDayAndNightThread;//日夜转换线程，根据白天黑夜状态加载不同的图像参数
#if (USE_ALGO)
    ThreadInfo_t		m_HDThread;
#endif
    ThreadInfo_t        m_DrawLineThread;
    ThreadInfo_t        m_MiscTaskThread;               //杂项任务线程
    ThreadInfo_t        m_SlowFrameModeSwitchThread;    //切换低帧模式线程
    
#if defined(__DIGITAL_ZOOM_DRAG_SUPPORT__)
		ThreadInfo_t		m_TrackerZoomThread;	//跟踪变倍线程
#endif

#if defined __ONE_CLICK_CALL__
			ThreadInfo_t		m_KeytocallThread;				//一键呼叫gpio检测
#endif

    pthread_mutex_t		m_WriteFileLock;		//压缩数据传递互斥

    STREAM_READ_CALLBACK m_pStreamReadCallback;	//编码函数
    HY_RWLodk_t			m_SysTemConfigLock;

    /*VI 的配置信息*/
    int                 m_VIChnWidth[SUPPORT_MAX_SENSOR_COUNT];           // 视频输入通道的图像实际大小
    int                 m_VIChnHeight[SUPPORT_MAX_SENSOR_COUNT];          // 视频输入通道的图像实际大小
    int                 m_VIChnFrmRate[SUPPORT_MAX_SENSOR_COUNT];         // 视频输入通道的图像实际帧率

    /* 系统时间OSD信息 */
    pthread_mutex_t		m_SysOSDLock;

    /* ENC资源管理 */
    char				*pVideoBuffer;	//视频码流缓冲区
#ifndef __USE_CIRCLEQUEUE__
    int					m_VideoBufferSize;	//视频码流缓冲大小
#endif
    char				*pMJPEGBuffer;		//视频码流缓冲区
    int					m_MJPEGBufferSize;	//视频码流缓冲大小
    int					m_TotalEncCap;		//总资源
    pthread_mutex_t     m_StreamCallbackLock;
    pthread_mutex_t     m_AudioInLock;
#ifdef __MODULE_4G_SUPPORT__
    ADJUST_ENCPRAM_STATUS   nCurAdustEncPramStatus; //当前自适应编码状态
#endif

    //二维码相关
    int 				m_bIsQrScanEnable;
    QR_SCAN_CALLBACK 	m_pQrScanCallback;
    XRS_NETCARD_TYPE_E 	m_NicStatus;            //网卡状态
    
    //云台坐标回调
	Moto_SavePos_CALLBACK 	m_pMotoSavePosCallback;

    /* 语音对讲相关 */
    TALKING_READ_CALLBACK   m_pTalkingCallBack;      //语音对讲回调函数
    int                     m_TalkingMode;           //语音对讲模式(G726 or G711)
    int                     m_SpeechMode;            //语音对讲模式(G726 or G711)
    int                     m_AudioFrameSize;        //语音对讲一帧大小
    int                     m_SpeechAudioFrameSize;  //语音对讲一帧大小
    int						m_AudioInMode;           //0:MIC   1:LINE_IN
    int                     m_SpeechTalkingMode;     //0 空闲模式 bit0=1 语音输出模式 bit1=1处于对讲模式
    void                    *m_pAmrToPcmDecoder;     //amr解码至pcm指针

    CmosFrequencyType_t m_Frequ_HZ;
    BOOL                m_bAudioInited;
    BOOL                m_bAduioInStart;

    pthread_mutex_t     m_SecretResetLock;			//IP复位

    /* 设备能力相关 */	
    BOOL                m_bSupportWifi;                        // 是否支持wifi
    BOOL                m_bSupportSDCard;                      // 是否支持SD卡

    /* 是否强制支持双光源功能 */
    int                 m_nFuncActiveStatusCode;               // 功能激活状态码
    BOOL                m_bActiveStatus;                       // 功能激活状态
    int                 m_LightBoardTypeID;                    // 定制灯板ID

    BOOL                m_bSupportAudioAlarm;                  // 是否支持音频报警
    BOOL                m_bSupportAudioBroadcast;              // 是否支持音频播报
    BOOL                m_bSupportCloudIPCSlowFrame;           // 是否支持云摄像机低帧模式
#ifdef __ONE_CLICK_CALL__
    BOOL				m_bSupportKeyToCall;				   // 是否支持一键呼叫
    BOOL        		m_bSupportAlternateResetButton;         //支持备用复位按键（一键呼叫按键）
#endif
    BOOL                m_bSupportAlarmOut;                    // 是否支持报警输出
    BOOL 				m_bSupportMotor;					   // 是否支持云台
	BOOL				m_bSupportZoom;					   // 是否支持云台
    int  				m_bSptSwapLRViewChannel;             /*支持左右球/枪的sensor通道交换 0:不交换 1:交换*/

    /* 授权相关 */
    BOOL                m_bAuthEncryptOK;                      // 授权验证成功
    char                m_szAuthLicense[LICENSE_CODE_MAX_LEN]; // 授权码
    char                m_szMacAddr[MAC_ADDR_LEN];             // MAC地址
    char 				m_sTutkUID[MAX_DID_NUM_LEN + 1];
    char				m_sUniqueID[UNIQUEID_BUF_LEN];		   // 唯一的硬件ID

    /* 声音侦测 */
    AudioDetect_CALLBACK m_pAudioDetectCallback;
    void *m_pAudioDetectCallbackData;
    pthread_mutex_t m_muAudioDetectCallbackLock;

    /* 其他 */
    int                 m_iCalibrationStatus;                  // 校准状态
    unsigned char *     pDormancyIFrameBuffer;                 // 休眠视频I帧缓存Buf
    unsigned char *     pDormancyPFrameBuffer;                 // 休眠视频P帧缓存Buf
    unsigned int        m_DormancyIFrameBufferSize;            // 休眠视频I帧缓存Buf大小
    unsigned int        m_DormancyPFrameBufferSize;            // 休眠视频P帧缓存Buf大小
    int                 m_iDormancyState;                      // 休眠状态 ZWSDK_DROMANCY_STATE
#ifdef __MODULE_4G_SUPPORT__
    int			        m_BroadCastEnable;                      // 绑定后是否播报语音
    e4GModuleType       m_4Gmoduletype; 						// 4g模组类型
    int					m_CloudCardmode;						//4g是否支持云卡 0-不支持 1-支持
    e4GTelecomOperatorType m_4GTelecomOperatorType;             // 4g运营商类型
#endif
    unsigned int        m_LensOrientation;           			// 镜头安装方向，由低到高每个字节，代表镜头安装时是否反向，1为反向
    HumanTracker_CALLBACK m_pHumanTrackerCallback;              // 人形跟踪处理回调
#if defined(__DIGITAL_ZOOM_DRAG_SUPPORT__)
		double				m_CurRatio; 							//送检前电子放大倍数	
#endif
    unsigned int        m_P2PSendDataFailCnt;                   // P2P发送数据失败计数
}SystemInfo_t;

typedef enum
{
    NormalMode = 0,     //普通模式(编解码)
    UpgradeMode = 1,    //升级模式
}enSdkMode;

typedef struct 
{
    pthread_t   thThreadId;
    STREAM_READ_CALLBACK cbStreamReadCallback;
    int bExit;
}stBuleScreenThreadInfo;

/* SDK接口资源 */
typedef struct
{
    HY_RWLodk_t			m_Lock;				//SDK出口和其它接口互斥
    int					m_bSDKInit;			//SDK是否初始化
    BOOL				m_bSDKInitLock;		//SDK入口与出口函数互斥
    enSdkMode           m_enSdkModeCur;     //SDK当前模式
    stBuleScreenThreadInfo m_stBuleScreenThreadInfo;    //蓝屏帧线程信息
}SDK_ResInfo_t;

#ifdef __DIGITAL_ZOOM_DRAG_SUPPORT__
#define OSD_ZOOM_MAX_HEIGH					(1)
#define OSD_ZOOM_MAX_LENGTH	(5)//空格100X空格 前后加空格便于描边
#define OSD_ZOOM_UNICODE_H (16) //点阵高
#define OSD_ZOOM_UNICODE_W (8) //点阵宽
#define OSD_ZOOM_PIC_WIDTH (OSD_ZOOM_UNICODE_W * OSD_ZOOM_MAX_LENGTH) //像素宽度 8*num
#define OSD_ZOOM_PIC_HEIGHT (16) //像素高度
#endif

typedef struct
{
    unsigned short int				m_PicWidth;
    unsigned short int              m_PicHeight;
}ResolutionType_t ;

typedef enum enENCODE_OSD_TYPE
{
    TEXT_TYPE = 0,
    TIME_TYPE = 1,
    ZOOM_TYPE = 2,
    MENU_TYPE = 3,
    DRAW_LINE_TYPE = 4,
    NAME_TYPE = 5,    
    BUTT_TYPE
} ENCODE_OSD_TYPE;

#define RTMP_SUPPORT       (0)
#define AF_SUPPORT         (1)
#define DOUBLE_LIGHT_SUPPORT    (3)
#define PEOPLE_DETECT_TRACK_SUPPORT (6)
#define EXTERNSIMCARD_SUPPORT 		(15)

#define CAPABILITY_RTMP   		(1 << RTMP_SUPPORT)
#define CAPABILITY_AF     		(1 << AF_SUPPORT)
#define CAPABILITY_DL	  	 	(1 << DOUBLE_LIGHT_SUPPORT)
#define CAPABILITY_PD_TRACK		(1 << PEOPLE_DETECT_TRACK_SUPPORT)
#define CAPABILITY_EXTERNSIMCARD (1 << EXTERNSIMCARD_SUPPORT)

//----------------------------------------- 公用部分子函数 --------------------------------------------------
int HY_Res_SysInitSensorCnt();
int HY_Res_GetMaxSensorCnt();
int HY_Res_GetSensorConfCorrect(int CheckCnt);

int HY_Res_GetInfo(char *pInfo);
int get_uid(char *tutkuid);

void    HY_Res_SDK_Usleep(unsigned int ntime);
int HY_Res_RWLock_init(HY_RWLodk_t *pLockInfo);
int HY_Res_RWLock_destory(HY_RWLodk_t *pLockInfo);
int HY_Res_RD_Lock(HY_RWLodk_t *pLockInfo);
int HY_Res_RD_UnLock(HY_RWLodk_t *pLockInfo);
int HY_Res_WR_Lock(HY_RWLodk_t *pLockInfo);
int HY_Res_WR_UnLock(HY_RWLodk_t *pLockInfo);

//--------------------------------- SDK 接口加锁 ------------------------
int HY_Res_SDK_Check();
int HY_Res_SDK_Lock();
int HY_Res_SDK_UnLock();

//--------------------------------- 获取相关资源地址 ------------------------
int HY_Res_InitSDKInfo();
int HY_Res_InitSystemInfo();

int HY_Res_GetSDKInfo(SDK_ResInfo_t **pInfo);
int HY_Res_GetSystemInfo(SystemInfo_t **pInfo);

int HY_Res_SetPresetConfig (ZWSDK_PresetConfig PresetConfig);
int HY_Res_GetPresetConfig (ZWSDK_PresetConfig *pPresetConfig);

//--------------------------------- 资源状态诊断函数 ------------------------
int HY_Res_CreatThread(ThreadInfo_t *pThInfo, int Pri, void *(*start_routine)(void*));

int HY_Res_DestroyThread(ThreadInfo_t *pThInfo);

int HY_Res_GetFormatSize(int width, int height, unsigned long *puMaxRate);

int system_time(char * time_str, size_t str_len);
int GetCurTimeHour();

unsigned long long get_ms();

unsigned long long get_s();

int HY_Res_SDK_GetTimeSinceBoot();

int get_integer_from_file(const char *filename, int *value);

extern unsigned G711U_DECODE_PCM(signed short* dst, const unsigned char* src, size_t srcSize);
int convert_g711u_to_amr(const char* input_file, const char* output_file);

int GetSpeechNamePath(char *SpeechFilePath);

void SpeechRemindTypeMap(SpeechType_T nType, eSpeechRemindType *nSpeechType);
void SpeechRemindNameMap(SpeechType_T nType, char *pCustomSpeechName);

#endif	//end of __COMMON_H__

