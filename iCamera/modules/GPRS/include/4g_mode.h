/*****************************************************************************
 * Copyright (C) 2017 All rights reserved.
 *  文件名称：
 *  创 建 者：
 *  邮    箱：
 *  创建日期：
 *  描    述：
 *   
 ******************************************************************************/
#ifndef __H_4G_MODE__
#define __H_4G_MODE__

#ifdef __MODULE_4G_SUPPORT__

#include <pthread.h>


#define PING_HOST_ADDR		"8.8.8.8"						//ping地址
#define PING_HOST_ADDR_BAK	"114.114.114.114"				//ping地址
#define PING_HOST_ADDR_YL		"217.218.155.155"			//ping地址
#define PING_HOST_ADDR_YL_BAK	"217.217.127.127"			//ping地址
#define TTY_DEVICE          "/dev/ttyUSB1"					//与模组通讯串口设备结点
#define TTY_DEVICE_H        "/dev/ttyUSB3"					//与模组通讯串口设备结点
#define TTY_DEVICE_H_1      "/dev/ttyUSB4"					//与模组通讯串口设备结点

//移远EG800Q-EU
#define EG800QEU_TTY_DEVICE "/dev/ttyUSB2"					//与模组通讯串口设备结点
#define TTY_DEVICE1         "/dev/ttyUSB0"					//与模组通讯串口设备结点
#define MEIG_MODULE_USBID	"0903"							//usb id
#define ZX800_MODULE_USBID  "0001"							//中云信安ZX800
#define EG800QEU_MODULE_USBID	"6007"						//移远EG800Q-EU usb id
#define ZX1602_MODULE_USBID "3010"							//中云信安1602
#define ZX800SG_MODULE_USBID  "3010"						//中云信安ZX800-SG
#define ZX800HG_MODULE_USBID  "3361"						//中云信安ZX800-HG

#define MEIG_SLM322M_ETH	"usb0"							// 模组 网卡
#define MODULE_4G_LOG       "/tmp/.4g_module_log" 			//日志文件
#define DNS_4G_FILE			"/mnt/mtd/etc/resolv_gprs.conf"			//4G DNS临时文件
#define DEVICE_RESET_FLAG	"/mnt/mtd/config/.module4gresetdevice"			//因4g模组有无复位设备标识文件
#define DEVICE_RESET_COM_FLAG	"/mnt/mtd/config/.module4gresetdevicecomerror"	//因4g模组通讯失败复位设备标识文件
#define MODULE_UPDATE_PATH	"/mnt/app/data/4GUpdate"	//4G模块升级文件路径
#define MODULE_UPDATE_FILE_MD5 "/mnt/app/data/4GUpdate/updatefile_md5.txt"	//4G模块升级文件md5值
#define SCODE_FILE			"/mnt/mtd/config/scode.conf"  //scode 文件
#define UPDATE_RESET_FLAG	"/mnt/mtd/config/.update4gresetdevice"			//因升级4g失败复位设备标识文件

#define AT_PING_RESP_STR	"Reply from %s: bytes="			//ping返回字符串
#define AT_RESP_OK_STR		"OK"				//at命令处理正常返回值
#define AT_RESP_ERR_STR		"ERROR"				//at命令处理错误返回值
#define AI_RESET_MODULE     "AT+CFUN=1,1"		//at复位命令处理错误返回值 
#define AT_GET_DNS			"AT*NETDNS=1"       //at获取dns命令返回值
#define AT_SIM_SWITCH       "AT+SWITCHSIM="		//SIM卡切换指令 
#define AT_MSG_SEPARATOR	"\r\n"				//消息分割符
#define AT_REPORT_SEPARATOR	"+"					//上报消息标识
#define AI_UC_MODULE     "AT^UCWKMODE?"     //at获取云卡命令处理错误返回值 
#define AI_QSIMSTAT     "AT+QSIMSTAT?"     //获取是否插外置卡，此命令在516版本才支持

#define DIAL_MAX_TIME			(30)			//等待拨号状态
#define CREG_BROADCAST_TIME	    (68)			//播报网络连接中，请稍后语音
#define CREG_MAX_TIME			(5*60)			//等待creg注册时间,某些sim卡首次入网非常慢
#define DNS_ADDR_NUM			(4)				//DNS 地址最多个数
#define DNS_ADDR_LEN			(32)			//DNS 地址长度

#define MAX_RECV_DATA_LEN		(1024)			//接收缓冲区大小

#define MAX_ACT_LEN         	(64)
#define MAX_OPER_LEN        	(64)
#define MAX_BAND_LEN        	(64)
#define HARDWARE_VERSION_LEN    (128)

#define MAX_IMSI_LEN        	(16)			//imsi长度
#define MAX_IMEI_LEN        	(16)			//imei长度
#define MAX_ICCID_LEN       	(32)			//iccid长度
#define MAX_VERSION_LEN       	(128)			//模组固件版本长度
#define	MAX_UCWORSTATUS_LEN		(16)			//云卡状态数据长度

#define SEND_WAIT_TIME			(2)				//发送at命令等待超时时间 s
#define SEND_RETRY_SLEEP_TIME	(1)				//重发at命令休眠时间 s
#define AIR_WAIT_TIME			(8)				//进入飞行模式等待时间
#define MAX_USB_RESET_TIME		(5)				//USB复位等待时间
#define MAX_MODULEPOWER_RESET_TIME	(10)		//断电复位模组等待时间
#define MAX_MODULESOFT_RESET_TIME	(10)		//软件复位模组最长等待时间
#define PING_PERIOD				(30)			//ping 周期 s
#define MICRO_POWER_PING_PERIOD	(300)			//微功耗ping 周期 s
#define PING_WAIT_TIME			(10)			//ping 最长等待时间 s
#define ERROR_SLEEP_TIME		(10)			//模组检测错误睡眠时间
#define ERROR_NOMODULE_SLEEP_TIME	(10)		//检测无模组错误睡眠时间
#define ERROR_COMREBOOT_SLEEP_TIME	(1800)		//因读写模组失败复位一次后下次再复位时间，改后暂未用上
#define MAX_UPDATE_RESET_WAIT_TIME  (30)        //升级过程中模组重启等待时长
#define MAX_UPDATE_TIME				(600)		//最长升级时间，超过该时间认为升级失败
#define WATCHDOX_MAX_TIME			(600)		//看门狗最大时长  s
#define WATCHDOG_FEED				(10)		//喂狗间隔s
#define WAIT_MODULE_ONLINE			(10)		//等待模组上线s
#define CHECK_MODULE_UPDATE_TIME	(3)			//检测1602模组是否在升级模式，s



#define MAX_SERIAL_RETRY_COUNT	(3)				//串口通讯无响应重试次数
#define MAX_PING_FAILED_COUNT	(5)				//ping外网无响应次数，认为无网络连接
#define MAX_PING_FAILED_COUNT_MODIFY	(60)	//ping外网无响应次数，认为无网络连接,2sping一次，连续ping60次，2min
#define SWITCH_BIND_WAIT_TIME	(10)			//切换频段等待时间 s

#define MAX_AIRPLANE_COUNT		(20)				//切换飞行模式最大次数
#define MAX_USB_RESET_COUNT		(1)				//usb复位次数
#define MAX_MODULE_RESET_COUNT	(2)				//未检测到模式硬件复位模组次数

/*4G切卡错误码*/
#define IDVR_CIU_COMMAND_4G_SIMSWITCH_NOSUPPORT	    0X091390009 /*不支持切卡返回错误码*/
#define IDVR_CIU_COMMAND_4G_SIMSWITCH_UPGRADING   0X091290005 /*升级中不允许切卡返回错误码*/
#define IDVR_CIU_COMMAND_4G_SIMSWITCH_ING	        0X09139000A /*正在切卡返回错误码*/
#define IDVR_CIU_COMMAND_4G_SIMSWITCH_INVALIDPARAM   0X09139000B /*切卡参数无效返回错误码*/
#define IDVR_CIU_COMMAND_4G_SIMSWITCH_STATES_FAILD	        0X09139000C /*4g状态不满足切卡返回错误码*/

//SIM卡最大支持个数
#define MAX_SIMCARD_CNT (2)

//SIM卡最大支持个数
#define MAX_SIMCARD_CNT (2)

//4G模组工作模式
typedef enum 
{
	RSLPMode_NORMAL = 0,        //常电
	RSLPMode_LOWPOWER = 1,      //低功耗
	RSLPMode_AOV = 2,           //AOV
}eRSLPModeType;


//正常异常定义
typedef enum {
    ABNORMAL    		= 0x0,		//异常
    NORMAL    			= 0x1,		//正常
    MAX_STATUS			= 0x2,
}eLinkStatus;

//4G工作模式
typedef enum {
	UNBINDING_MODE    	= 0x0,		//标准模式
    BINDING_MODE    	= 0x1,		//运营模式
    MAX_WORKMODE		= 0x2,
}eWorkMode;

//网络连接语音播报
typedef enum {
	LINKSPEECH_WAIT    			= 0x0,		//暂不播报语音
    LINKSPEECH_ORIGINAL 		= 0x1,		//绑定卡(金睿卡、北纬卡、云仓库)
    LINKSPEECH_NOTORIGINAL_BIND	= 0x2,		//非绑定卡，未解绑
    LINKSPEECH_NOTORIGINAL_UNBIND	= 0x3,		//非绑定卡，已解绑
}eLinkSpeechControl;

//通讯错误处理
typedef enum {
	COMERROR_RESET_STATE,		//复位模组
	COMERROR_WAIT_STATE,		//睡眠等待
	COMERROR_TEST_STATE,		//读写测试
	COMERROR_REBOOT_STATE,		//复位设备
}eComErrorHandlerState;


//任务状态
typedef enum {
	TASK_USE4G_STATE = 0,			//使用4G状态
	TASK_PRODUCTTEST_STATE = 2,		//产测状态
	TASK_CHECK_ERROR_STATE = 3,		//检测错误状态
	TASK_SLEEP_STATE = 4,			//关闭4G状态
	TASK_UPDATE_MODULE_STATE = 5,	//4G模组升级固件状态
}eTaskState;

//4g模组处理下各状态
typedef enum {
	CHECK_MODULE_STATE,			//检测模组, 		0
	MODULE_INIT_STATE,			//初始化, 			1
	CHECK_SIM_STATE,			//检测sim卡, 		2
	GET_INFO_STATE,				//获取模组sim卡信息, 3
	ENTER_AIR_STATE,			//进入飞行模式, 	4
	QUIT_AIR_STATE,				//退出飞行模式, 	5
	DIAL_STATE,					//拨号, 			6
	CHECK_DIAL_STATE,			//检测拨号状态, 	7
	CHECK_CREG_STATE,			//检测入网状态, 	8
	CHECK_CSQ_STATE,			//获取信号质量, 	9
	CHECK_PING_STATE,			//检测与外网连接状态, 10
	COMMUNITE_ERROR_STATE,		//通讯异常状态, 	11
	CLOUD_CARD_STATE,			//云卡模式 			12
	SIMSWITCH_CARDS_STATE,			//切卡状态
}eBINDTASK_STATE;


//产测各状态
typedef enum {
	PRODUCTTEST_CHECK_STATE,			//开始检测
	PRODUCTTEST_CHECKFINISH_STATE,		//检测结束
	PRODUCTTEST_CLOUD_CARD_STATE,		//云卡模式检测
}ePRODUCTTESTTASK_STATE;


//4G升级各状态
typedef enum {
	UPDATE_CHECK_UPDATING_STATE,//检测是否正在升级，因上次断电未升级完成
	UPDATE_CHECK_STATE,			//检测是否需要升级，通过对比固件和升级文件版本
	UPDATE_SENDFILE_STATE,		//下发升级文件，下发完后4G模组会进行重启一次
	UPDATE_WAIT_FINISH_STATE,   //等待升级完成，升级完成后4G模组会再重启一次
}eUPDATETASK_STATE;

// 4G 网络异常情况分类
enum{
	SimCardErr = 0,
	CregErr = 1,
	DialErr = 2,
	DhcpErr = 3,
//	PingErr = 4,
	MaxErr,
};


//产测数据结构体
typedef struct {
	int 	uModuleStatus;				                /*模组检测状态，有或者无*/
	int 	uSimCardStatus[MAX_SIMCARD_CNT];            /*sim卡状态，有或者没有*/
	int 	uCregStatus[MAX_SIMCARD_CNT];               /*入网状态*/
	int 	uInternetConnStatus;                        /*与外网通讯状态*/
	char 	sImei[MAX_IMEI_LEN];                        /*imei*/
	char 	sImsi[MAX_SIMCARD_CNT][MAX_IMSI_LEN];		/*imsi*/
	char 	sIccid[MAX_SIMCARD_CNT][MAX_ICCID_LEN];		/*iccid*/
	char 	sACT[MAX_SIMCARD_CNT][MAX_ACT_LEN];			/*act*/
    char 	sOper[MAX_SIMCARD_CNT][MAX_OPER_LEN];		/*oper*/
    char 	sBand[MAX_SIMCARD_CNT][MAX_BAND_LEN];		/*band*/
	int 	iCsq[MAX_SIMCARD_CNT];                      /*csq 信号质量*/
	char	sVersion[MAX_VERSION_LEN];				    /*模组固件版本*/
}Module4G_ProductTest_Data_t;

/*AT 管理体结构定义*/
typedef struct
{
	int 	iSerial_fd;								/*与模组通讯串口句柄*/
	int 	uState;									/*主任务状态*/
	int		uSubState;								/*子任务状态*/
	int 	nPingControltime;						/*ping 周期*/
	int 	nPingFailedCount;						/*与外网ping失败次数*/
	int 	nAirPlaneTestCount;						/*飞行模式尝试次数*/
	int 	nUsbResetCount;							/*USB复位次数*/
	int 	nModuleResetCount;						/*模组检测异常断电复位次数*/
	int 	uErrorSleepTime;						/*检测错误睡眠时间*/
	int		nDeviceResetFlag;						/*设备复位标志*/
	int 	uCheckErrorFlag;						/*检测错误标识*/
    int     uErrorNotSleepFlag;                     /*无需睡眠，直接对模组断上电标识*/
	int 	nProductTestFinish;						/*产测结束标识*/
	int 	nHasCutCardFlag;						/*自动切卡操作标识*/
	int 	nNetFailedResetModuleFlag;				/*网络不通上下电复位设备标识*/
	int 	nModuleOpenFlag;						/*模组上电标识*/
	int 	nProductTestFlag;						/*产测标识，标识是否执行产测流程*/
	int 	nUpdate4GFlag;							/*升级标识，标识是否执行升级4G模组流程*/
	int		nNeedCheckUpdateAgain;					/*重新检测标识，标识是否再次检测4G模组升级*/
	int 	eLinkSpeechControl;						/*网络连接语音播报控制*/
	int 	uUpdateCheckThreadExitFlag;				/*升级检测线程退出标识*/
	int		uNeedSpeechSimSuccess;					/*播报sim卡检测成功标识*/
	int 	nCregCheckCount;						/*creg检测控制*/
	int 	nDialState;								/*拨号状态*/
	int 	nDialStateCheckCount;					/*拨号状态检测计数*/
	int 	nDonotSendStartUpdateFlag;				/*不下发开始升级指令标识*/	
    int 	nDonotSendRndisFlag;				    /*不下发设置rndis指令标识*/	

	int 	uComErrorState;							/*通讯错误处理控制状态*/
	int		nComErrorResetSleepCount;				/*休眠计数器*/
	int 	uComErrorRebootFlag;					/*复位标识*/

	int 	uWorkMode;								/*4G工作模式，运营模式，标准模式*/
	int 	uWireConnStatus;						/*有线连接状态 0 有线连接 1 有线断开*/
	int 	uProductMode;							/*产测模式 0 正常模式 1 产测模式*/
	int 	uAllowFlag;								/*允许使用4G标识 0 不允许 1 允许*/
	int 	uAllowUseEth0Flag;						/*允许使用有线标识 0 不允许 1 允许*/
	int 	nTutkStatus;							/*P2P连接状态 0 离线 2 在线*/
	int     uBindMode;                              /*设备是否已被绑定， 0未绑定，1已绑定*/ 
	
	int 	uModuleStatus;							/*模组检测状态，有或者无*/
	int 	uSimCardStatus[MAX_SIMCARD_CNT];        /*sim卡状态，有或者没有*/
	int 	uCregStatus[MAX_SIMCARD_CNT];           /*入网状态*/
	int 	uInternetConnStatus;					/*与外网通讯状态*/
    int     uRealNetConnStatus;					    /*与外网通讯真实状态*/
	char 	sImei[MAX_IMEI_LEN];					/*imei*/
	char 	sImsi[MAX_SIMCARD_CNT][MAX_IMSI_LEN];   /*imsi*/
	char 	sIccid[MAX_SIMCARD_CNT][MAX_ICCID_LEN]; /*iccid*/
	char 	sACT[MAX_SIMCARD_CNT][MAX_ACT_LEN];     /*act*/
    char 	sOper[MAX_SIMCARD_CNT][MAX_OPER_LEN];   /*oper*/
    char 	sBand[MAX_SIMCARD_CNT][MAX_BAND_LEN];   /*band*/
	int 	iCsq[MAX_SIMCARD_CNT];                  /*csq 信号质量*/

	char	sVersion[MAX_VERSION_LEN];				/*模组固件版本*/
	int		iSignal4G;				/*4G信号强度，0-4*/
	int		iModuleMode;             				/*4G模组业务形态： 0:常电, 1:低功耗, 2:AOV*/

	char 	szDnsAddr[DNS_ADDR_NUM][DNS_ADDR_LEN];	/*DNS 地址*/
	int     rndistype;								/*协议类型*/
	int     netcardtype;							/*网卡模式*/
	int     nCurSimCardIndex;                       /*当前正在使用的sim卡号，0-SIM1 1-SIM2*/
	char 	sHardVersion[HARDWARE_VERSION_LEN];		/*模组硬件版本*/
    int 	nmifiFlag;								/*是否为公网ip    	0:公网dns 1：局域网dns*/ 
	char 	szATDnsAddr[DNS_ADDR_NUM][DNS_ADDR_LEN];/*DNS 地址*/
    int     nSetModuleTypeEnd;                      /*是否设置完ModuleType*/
    int 	ucWorkMode;								/*云卡工作模式*/
    int     ucOpenusStatus;                         /*云卡启用状态*/
    int     ucOpenusProcess;                        /*云卡启用进度*/
    int     ucOpenusErrno;                          /*云卡启用错误码*/
	int		IsSupportOnlyCloudCard;					/*是否启用只使用云卡模式*/
    unsigned int     ucDetectTimes;                 /*云卡错误码检测时长*/

	int 	s_SimCardType;							//SIM卡运营商类型
	int 	s_chinaMobileBand; 						//移动卡的频段 1: FDD 0: 其他频段
	int 	s_ltebandlBand; 						//ltebandl 频段值
	int 	s_4GSimCardCount;//4g模组真正支持的sim卡数量

	int 	s_4GNetErrTimes[MaxErr];
	int 	s_ResetError_State;//检测不到模组或读写错误，复位模组的状态
	int		nRSLPMode;								/*模组工作模式 eRSLPModeType 0:常电, 1:低功耗, 2:AOV*/
	int 	s_IsSgOrVg4gModule;						/*模组类型 0:SG 1:VG*/
	int     device_did_info;						/*1:YL号段*/
	int 	sim_01_card_status;						/*sim1 卡槽是否插卡 0:不插卡 1:插卡             2:AT+QSIMSTAT?命令不支持,4G模组版本小于516*/ 
	
	
	int s_SwitchCardsState;//切卡状态 0:未切卡 1：切卡中
	int s_SwitchCardsResult;//切卡结果 -1为无效状态（说明没有切卡）0：切卡成功，1：卡不存在 2：入网失败 3：信号小于40 4：网络不通
	Module4G_ProductTest_Data_t s_ProductTestData; /* 产测数据结构 */

	int (*GetSCode)(char *, int *); 		//int MPU_SYS_DeviceBind_GetSCode(char *code, int *len);



	pthread_mutex_t MutexParaAccess;
	pthread_mutex_t MutexProductDataAccess;
}G4_Control_Manager_t;


//设置云卡能力集回调
typedef int (*SETCLOUDCARD_CAP_CB)(int iSupport);

#ifdef __cplusplus
extern "C" {
#endif


/*@breif Module4G_Init GPRS模块初始化
 *@return 成功返回 0,更改模式返回1
 *@      失败返回-1
 */
void Module4G_Init(void);

/*@breif Module4G_UnInit GPRS模块反初始化 */
void Module4G_UnInit(void);

/*@breif Module4G_GetModuleSignalQuality 检查Sim卡的信号强度
 *@param [OUT] signal_quality 当前模块的信号强度
 *@return 成功返回 0
 *@       失败返回-1
 */
int Module4G_GetModuleSignalQuality(int *signal_quality);


/*@breif Module4G_GetModule4GSignalQuality 获取4G信号强度
 *@param [OUT] signal_quality 当前模块的信号强度
 *@return 成功返回 0
 *@       失败返回-1
 */
int Module4G_GetModule4GSignalQuality(int *signal_quality);


/*@breif Module4G_GetSimIndexSignalQuality 获取指定sim卡一些信息
 *@param [OUT] nIsInUse 指定sim卡:是否在用，szOperator:运营商，nSignalStrength:信号强度，szICCID:iccid, szIMEI:imei szVersion:hardversion
 *@return 成功返回 0
 *@       失败返回-1
 */
int Module4G_GetSimIndexInfo(int nSim, int* nIsInUse, char *szOperator, int *nSignalStrength, char *szICCID, char *szIMEI);

/*@breif Module4G_GetModuleSimStatus 获取sim卡状态
 *@param [OUT] CregStatus 
 *@return 成功返回 0
 *@       失败返回-1
 */
int Module4G_GetModuleSimStatus(int *SimStatus);



/*@breif Module4G_GetModuleInternetConnStatus 获取当前模块的联网状态
 *@param [OUT] status 当前模块的联网状态
 *@return 成功返回 0
 *@       失败返回-1
 */
int Module4G_GetModuleInternetConnStatus(int *status);

int Module4G_GetModuleRealInternetConnStatus(int *status);

/*@breif Module4G_GetIccid 获取当前卡的iccid
 *@param [OUT] Iccid 当前卡的iccid
 *@return 成功返回 0
 *@       失败返回-1
 */
int Module4G_GetIccid(char *Iccid);


/*@breif Module4G_GetVersion 获取模组当前版本
 *@param [OUT] Iccid 当前卡的iccid
 *@return 成功返回 0
 *@       失败返回-1
 */
int Module4G_GetVersion(char *Version);

/*@breif Module4G_SetWorkMode 设置工作模式
 *@param [OUT] mode 是否自动切卡
 *@return 成功返回 0
 *@       失败返回-1
 */
int Module4G_SetWorkMode(int mode);

/*@breif Module4G_SetBindMode 设置绑定模式
 *@param [OUT] mode 设备是否已绑定 0未绑定，1 已绑定
 *@return 成功返回 0
 *@       失败返回-1
 */
int Module4G_SetBindMode(int mode);


/*@breif Module4G_SetWireConnStatus 设置当前有线连接状态
 *@param [In] status 有线连接状态
 *@return 成功返回 0
 *@       失败返回-1
 */
int Module4G_SetWireConnStatus(int status);


/*@breif Module4G_GetProductTestData 获取产测数据
 *@param [OUT] out_buf 
 *@return 成功返回 0
 *@       失败返回-1
 *@       正在测试-2
 */
int Module4G_GetProductTestData(Module4G_ProductTest_Data_t *out_buf);


/*@breif Module4G_SetProductTest 产测开始
 *@param [In] TestStart 设置产测模式
 *@return 成功返回 0
 *@       失败返回-1
 */
int Module4G_SetProductTest(int TestStart);


/*@breif Module4G_SetProductMode 产测开始
 *@param [In] TestStart 设置产测开始
 *@return 成功返回 0
 *@       失败返回-1
 */
int Module4G_SetProductMode(int nMode);

/*@breif Module4G_SetCloudCardOnly 设置是否只使用云卡
 *@param [In] TestStart 设置只使用云卡模式
 *@return 成功返回 0
 *@       失败返回-1
 */
int Module4G_SetCloudCardOnly(int CloudCardStatus);

/*@breif Module4G_SetAllowUseEth0Flag 设置是否允许使用有线标识
 *@param [In] uAllowUseEth0Flag 
 *@return 成功返回 0
 *@       失败返回-1
 */
int Module4G_SetAllowUseEth0Flag(int uAllowUseEth0Flag);


/*@breif Module4G_SetTutkStatus 设置tutk状态
 *@param [In] nTutkStatus tutk状态
 *@return 成功返回 0
 *@       失败返回-1
 */
int Module4G_SetTutkStatus(int nTutkStatus);


/*@breif Module4G_SetAllowFlag 设置是否允许使用4G
 *@param [In] flag 标识
 *@return 成功返回 0
 *@       失败返回-1
 */
int Module4G_SetAllowFlag(int flag);


/*@breif Module4G_LinkSpeechControl 设置是否允许使用4G
 *@param [In] flag 标识
 *@return 成功返回 0
 *@       失败返回-1
 */
int Module4G_LinkSpeechControl(int eControl);

//获取云卡设备工作模式
int Module4G_GetUcWorkMode(int *piUcWorkMode);

//获取是否是云卡设备
int Module4G_GetIsCloudCardDev(void);


/*@breif Module4G_GetSimInfo 获取当前卡的信息
 *@param [OUT] Iccid 当前卡的信息
 *@return 成功返回 0
 *@       失败返回-1
 */
int Module4G_GetSimInfo(char *imsi, 
								char *imei, 
								char *act, 
								char *oper, 
								char *band,
								char *iccid,
								char *Version);

/*@breif Module4G_IsModuleTypeSet 模组类型是否设置完毕
 *@param 空
 *@return 返回模组类型是否设置完毕
 */
int Module4G_IsModuleTypeSet        ();

/*@breif Module4G_ManagerProcessThread 4G模组处理主处理线程
 *@param [In] param 线程传入参数
 *@return 成功返回 0
 *@       失败返回-1
 */
void *Module4G_ManagerProcessThread(void *param);

//设置云卡能力集回调
int Module4G_SetCloudCardCapCallback(SETCLOUDCARD_CAP_CB pCallback);

int Set4GRealSimCardCount(int simCardCount);
int Get4GRealSimCardCount();

int Module4G_Set4GControlConfig(int nValid, int nCardID);
int Module4G_Get4GControlConfig(int *nValid,int *nCardID,char *cICCID,int *nAutoSwitchCard);
int Module4G_Get4GSwitchResult(int *nValid,int *iSelectErrCode);

//设置设备did类型
int Module4G_SetDevDidType(int DiDType);

#ifdef __cplusplus
}
#endif

#endif

#endif

