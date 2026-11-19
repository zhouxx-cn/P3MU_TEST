#ifndef __PTZ_CODE_H_
#define __PTZ_CODE_H_

//!\brief 云台基本控制码
//##ModelId=49B866580222
enum TTY_PTZ_CODE
{
	//!初始化
	//##ModelId=49B866F502BF
	TTY_PTZ_INITIAL = 0x01,

	//!向上
	//##ModelId=49B866F502EE
	TTY_PTZ_UP = 0x02,

	//!自动向上
	//##ModelId=49B866F5032C
	TTY_PTZ_UP_AUTO = 0x03,

	//!向下
	//##ModelId=49B866F5035B
	TTY_PTZ_DOWN = 0x04,

	//##ModelId=49B866F5038A
	TTY_PTZ_DOWN_AUTO = 0x05,

	//!向左
	//##ModelId=49B866F503C8
	TTY_PTZ_LEFT = 0x06,

	//##ModelId=49B866F6001F
	TTY_PTZ_LEFT_AUTO = 0x07,

	//!向右
	//##ModelId=49B866F6005D
	TTY_PTZ_RIGHT = 0x08,

	//暂不实现
	//##ModelId=49B866F6009C
	TTY_PTZ_RIGHT_AUTO = 0x09,

	//!左上
	//##ModelId=49B866F600DA
	TTY_PTZ_LEFTUP = 0x10,

	//!右上
	//##ModelId=49B866F60119
	TTY_PTZ_RIGHTUP = 0x11,

	//!左下
	//##ModelId=49B866F60157
	TTY_PTZ_LEFTDOWN = 0x12,

	//!右下
	//##ModelId=49B866F60196
	TTY_PTZ_RIGHTDOWN = 0x13,

	//!上停
	//##ModelId=49B866F601D4
	TTY_PTZ_UP_STOP = 0x14,

	//!下停
	//##ModelId=49B866F60222
	TTY_PTZ_DOWN_STOP = 0x15,

	//!左停
	//##ModelId=49B866F60261
	TTY_PTZ_LEFT_STOP = 0x16,

	//!右停
	//##ModelId=49B866F6029F
	TTY_PTZ_RIGHT_STOP = 0x17,

	//!停止
	//##ModelId=49B866F602EE
	TTY_PTZ_STOP = 0x1C,

	//!变倍大
	//##ModelId=49B866F6033C
	TTY_PTZ_ZOOMIN = 0x21,

	//!变倍小
	//##ModelId=49B866F6037A
	TTY_PTZ_ZOOMOUT = 0x22,

	//!自动变倍
	//##ModelId=49B866F603C8
	TTY_PTZ_ZOOM_AUTO = 0x23,

	//!停止变倍
	//##ModelId=49B866F7002E
	TTY_PTZ_ZOOM_STOP = 0x24,

	//!光圈大
	//##ModelId=49B866F7007D
	TTY_PTZ_IRIS_LARGE = 0x25,

	//!光圈小
	//##ModelId=49B866F700CB
	TTY_PTZ_IRIS_SMALL = 0x26,

	//!自动光圈
	//##ModelId=49B866F70119
	TTY_PTZ_IRIS_AUTO = 0x27,

	//!停止光圈调节
	//##ModelId=49B866F70167
	TTY_PTZ_IRIS_STOP = 0x28,

	//!焦距近
	//##ModelId=49B866F701C5
	TTY_PTZ_FOCUS_NEAR = 0x29,

	//!焦距远
	//##ModelId=49B866F70213
	TTY_PTZ_FOCUS_FAR = 0x2a,

	//!自动调焦
	//##ModelId=49B866F70271
	TTY_PTZ_FOCUS_AUTO = 0x2b,

	//!停止调焦
	//##ModelId=49B866F702CE
	TTY_PTZ_FOCUS_STOP = 0x2c,

	//!
	//##ModelId=49B866F7031C
	TTY_PTZ_CAMERA_MANUAL = 0x2d,

	//!\brief 左上停
	TTY_PTZ_LEFTUP_STOP = 0x2e,

	//!\brief 左下停
	TTY_PTZ_LEFTDOWN_STOP = 0x2f,

	//!\brief 右上停
	TTY_PTZ_RIGHTUP_STOP = 0x30,

	//!\brief 右下停
	TTY_PTZ_RIGHTDOWN_STOP = 0x31,

	//!\brief 设置坐标
	TTY_PTZ_SET_POS = 0x32,

	//!\brief 向左走一步
	TTY_PTZ_LEFT_ONESTEP = 0x33,

	//!\brief 向右走一步
	TTY_PTZ_RIGHT_ONESTEP = 0x34,

	//!\brief 向下走一步
	TTY_PTZ_DOWN_ONESTEP =  0x35, 

	//!\brief 向上走一步
	TTY_PTZ_UP_ONESTEP   = 0x36,
};

//!\brief 预置点控制码
//##ModelId=49B8668C0280
enum TTY_PRESET_CODE
{
	//!设置预置点
	//##ModelId=49B866F9032C
	TTY_PTZ_PRESETSET = 0x01,

	//!删除预置点
	//##ModelId=49B866F9036B
	TTY_PTZ_PRESETMOVE = 0x02,

	//!删除所有预置点
	//##ModelId=49B866F903A9
	TTY_PTZ_PRESETCLRALL = 0x03,

	//!调用预置点
	//##ModelId=49B866FA0000
	TTY_PTZ_PRESETCALL = 0x04,

	//!设置轨迹
	//##ModelId=49B866FA003E
	TTY_PTZ_TRACKSET = 0x05,

	//!设置轨迹结束
	//##ModelId=49B866FA006D
	TTY_PTZ_TRACKEND = 0x06,

	//!开始设置巡航
	//##ModelId=49B866FA00DA
	TTY_PTZ_ROUTESET = 0x08,

	//!添加巡航点
	//##ModelId=49B866FA0119
	TTY_PTZ_ROUTEADDPOINT = 0x09,

	//!删除巡航点
	//##ModelId=49B866FA0157
	TTY_PTZ_ROUTEREMOVEPOINT = 0x10,

	//!结束巡航设置
	//##ModelId=49B866FA0196
	TTY_PTZ_ROUTEEND = 0x11,

	//!删除巡航序列
	//##ModelId=49B866FA01E4
	TTY_PTZ_ROUTEREMOVE = 0x12,

	//!设置线扫开始点
	TTY_PTZ_AUTOSET = 0x13,

	//!设置线扫结束点
	TTY_PTZ_AUTOEND = 0x14,

	//!设置看守动作
	TTY_PTZ_GUARDSETACTION = 0x15,

	//!设置看守动作周期
	TTY_PTZ_GUARDSETCYCLE = 0x16,

	//!一键回到看守位
	TTY_PTZ_WATCHCAREGOTO = 0x17,
};

//!\brief 自动控制码
//##ModelId=49B8666D0232
enum TTY_AUTO_CODE
{
	//!自动扫描
	//##ModelId=49B866F900CB
	TTY_PTZ_AUTO = 0x01,

	//!手动
	//##ModelId=49B866F900FA
	TTY_PTZ_MANUAL = 0x02,

	//!\brief 开始自动巡航
	//!bScan = 1开始
	//!bScan = 0结束
	//##ModelId=49B866F90128
	TTY_PTZ_SETROUTE = 0x03,

	//!调用轨迹
	//##ModelId=49B866FA00AB
	TTY_PTZ_TRACKCALL = 0x04,


	//!设置云台速度
	//##ModelId=49B866F90157
	TTY_PTZ_SPEED = 0x05,

	//!上下速度
	//##ModelId=49B866F90196
	TTY_PTZ_UPDOWN_SPEED = 0x06,

	//!左右速度
	//##ModelId=49B866F901C5
	TTY_PTZ_LEFTRIGHT_SPEED = 0x07,

	//!停留时间
	//##ModelId=49B866F90203
	TTY_PTZ_DWELL = 0x08,

	TTY_PTZ_GUARD = 0x09
};

//!\brief 云台辅助控制码
//##ModelId=49B8666202CE
enum TTY_AUX_CODE
{
	//!灯光
	//##ModelId=49B866F8030D
	TTY_AUX_LIGHT = 0x01,

	//!加热
	//##ModelId=49B866F8033C
	TTY_AUX_HEATER = 0x02,

	//!制冷
	//##ModelId=49B866F8037A
	TTY_AUX_COOLER = 0x03,

	//!雨刮
	//##ModelId=49B866F803A9
	TTY_AUX_WIPER = 0x04,

	//!电源
	//##ModelId=49B866F803D8
	TTY_AUX_POWER = 0x05,

	//!恢复电子PTZ所有参数
	TTY_AUX_RESETPARAM = 0x20
};

//!\brief 菜单控制码
//##ModelId=49B8667803C8
enum TTY_MENU_CODE
{
	//!进入菜单
	//##ModelId=49B869800261
	TTY_PTZ_MENU_ENTER = 0x01,

	//!菜单焦点向上
	//##ModelId=49B8699D004E
	TTY_PTZ_MENU_UP = 0x02,

	//!菜单焦点向下
	//##ModelId=49B869AB00EA
	TTY_PTZ_MENU_DOWN = 0x03,

	//!菜单左
	TTY_PTZ_MENU_LEFT = 0x04,

	//!菜单右
	TTY_PTZ_MENU_RIGHT = 0x05,

	//!菜单确定
	TTY_PTZ_MENU_OK = 0x06,

	//!菜单取消
	TTY_PTZ_MENU_CANEL = 0x07,

	//!退出菜单
	//##ModelId=49B869B9037A
	TTY_PTZ_MENU_EXIT = 0x08
};

typedef enum
{
	//!数字变倍
	ePropDigitZoom,

	//!低照度
	ePropLowLum,

	//!背光补偿
	ePropBackLight,

	//!自动快门
	ePropAutoShutter,

	//!白平衡
	ePropWhiteBalance,

	//!红增益
	ePropRedGain,

	//!蓝增益
	ePropBlueGain,

	//!锐度
	ePropAcutance,

	//!曝光模式
	ePropExpMode,

	//!快门速度
	ePropShutterSpeed,

	//!光圈
	ePropIrisSpeed,

	//!宽动态
	ePropWideDyn,

	//!3D降噪
	eProp3DFilter,

	//!镜像
	ePropMirror,

	//!翻转
	ePropReverse,

	//!高清视频模式(分辨率，制式频率，高宽比)
	ePropHDVideoMode,

	//!长焦限速
	eProplimitS,
	
	//低功耗
	eLowPowerConsumption,	
	
	//雨刷		
	eScreenWiper,	
	
	//夜视设备
	eNightVision,
	
	//除雾
	eDemist,
	
	//!重启
	ePropreboot,
	
	//!北位设置
	ePropnorth,

	ePropEnd
	
} eEletronicProp;

typedef enum
{
	ePtzBlackMode = 8,
	ePtzShimmerMode = 9,
	ePtzEnableSmartTrack = 10,	//hsab用
	ePtzDisableSmartTrack = 11,	//hsab用

	ePtzAILinkAlarmOutOpen = 12,
	ePtzAILinkAlarmOutClose = 13,
	
	ePtzAntiflickerOpen = 54,
	ePtzAntiflickerClose = 55,

	ePtzEnableSmartTrackStd = 75,	//标准用
	ePtzDisableSmartTrackStd = 76,	//标准用
	ePtzMotorToWatch60S = 77,
	ePtzMotorToWatch30S = 78,
	ePtzMotorRemovePreset = 79,
	ePtzMotorSelfCheck = 80,

	ePtzMotorCruise = 97,
	
	ePtzIrcIntelligentNight = 111,
	ePtzIrcIntelligentRed = 112,	
	ePtzIrcIntelligentWhite = 113,	

	ePtzTurnOffWhiteForce = 229,
	ePtzTurnOnWhite = 230,	
	ePtzTurnOnWhiteForce = 231,	
	
	ePtzAILinkAudioOpen = 232,
	ePtzAILinkAudioClose = 233,
	ePtzHumanOverlayOpen = 234,
	ePtzHumanOverlayClose = 235,
	ePtzDetectAreaOpen = 999,//暂时不需要操作布防框
	ePtzDetectAreaClose = 999,
} ePtzPresetFunc;

typedef enum
{
	// 机芯类型
	eStateType = 0x100,
	// 温度
	eStateTemperature,
	// 坐标
	eStateCoordinate,
	// 485键盘信息
	eState485Com,
} eEletronicState;

typedef enum
{
	eNone,
	// 索尼
	eSony = 1,
	// 日立
	eHitachi = 2,
	// 三洋
	eSanyo = 7
} eCameraType;

typedef enum
{
	ePTZMoving = 0,
	ePTZStop   = 1
} ePTZMoveStatus;

typedef int (*CbSendData) (unsigned int nChannel, void *pBuf, int nLen);

typedef int (*CbRecvData) (unsigned int nChannel, void *pBuf, int nLen);

typedef int (*CbStartCruise) (unsigned int nChannel, unsigned int nCruiseIndex);

typedef int (*CbStopCruise) (unsigned int nChannel);

#endif
