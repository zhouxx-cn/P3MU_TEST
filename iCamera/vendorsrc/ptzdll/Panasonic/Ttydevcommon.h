#ifndef _TTYDEVCOMMON_H_
#define _TTYDEVCOMMON_H_


//∂®“Â∑µªÿ÷µ
#define	DLL_ERROR	(-1)		//≤Œ ˝ªÚ’ﬂ√¸¡Ó¥ÌŒÛ
#define	DLL_INPUT_OK	(0)		//≤Œ ˝ªÚ’ﬂ√¸¡Ó’˝»∑£¨µ´ «÷¥––¥ÌŒÛ
#define	DLL_OUTPUT_OK		(1)	//∑µªÿ’˝»∑
#define	DLL_NEED_REMAIN	(2)		//–Ë“™±£≥÷≤Ÿ◊˜

typedef int BOOL;

#define FALSE   0
#define TRUE    1

//÷ß≥÷µƒ…Ë±∏
#define DRIVER_PTZ			0x01	//ÂÆö‰πâ‰∫îÁßçËÆæÂ§á ‰∫ëÂè∞
#define DRIVER_IO			0x02	//ÂÆö‰πâ Êä•Ë≠¶iO
#define DRIVER_MATRIX		0x04	//ÂÆö‰πâ Áü©Èòµ
#define DRIVER_WATCHDOG		0x08	//ÂÆö‰πâ ÁúãÈó®Áãó
#define DRIVER_CFDOG		0x10	//ÂÆö‰πâ Êô∫ËÉΩÂç°
#define DRIVER_ADC			0x20	//ÂÆö‰πâ Ê®°ÊãüÈáèËæìÂÖ•

//÷ß≥÷µƒ≤®Ãÿ¬ 
#define	BD1200	0x01
#define	BD2400	0x02
#define	BD4800	0x04
#define	BD9600	0x08
#define	BD19200	0x10
#define	BD38400	0x20
#define	BD57600	0x40
#define	BD115200	0x80

//PTZ  
#define MAX_PTZ_SPEED_LEVEL	8
#define MAX_TYPE_PTZTYPE	8

#define PTZ_TYPE_NO			0	// no ptz function
#define PTZ_TYPE_NORMAL		1	// normal ptz
#define PTZ_TYPE_FAST		2	// speed dome
#define PTZ_TYPE_PARITY  	4	//

// PTZ FLAG 
#define PTZFLAG_PANTITLT	0x01
#define PTZFLAG_ZOOM		0x02
#define PTZFLAG_FOCUS		0x04
#define PTZFLAG_IRIS		0x08


//PTZ AUXFLAG
#define PTZFLAG_AUXLIGHT		0x01	// have light function
#define PTZFLAG_AUXHEATER		0x02	// have heater function
#define PTZFLAG_AUXCOOLER		0x04	// have cooler function
#define PTZFLAG_AUXWIPER		0x08	// have wiper function
#define PTZFLAG_AUXPOWER		0x10	// have power function
#define PTZFLAG_AUXPRESET		0x20
#define PTZFLAG_AUXTRACK	0x40	// have track function
#define PTZFLAG_AUXAUTO		0x80	// have ◊‘∂Ø…®√Ë function
#define PTZFLAG_AUXCRUISE		0x100	// have  cruise function
#define PTZFLAG_AUXTIMECRUISE	0x200	// have  function

//PTZ TIMERFLAG
#define TIMERFLAG_PANTITLT		0x01
#define TIMERFLAG_ZOOM			0x02
#define TIMERFLAG_FOCUS		0x04
#define TIMERFLAG_IRIS			0x08
#define TIMERFLAG_AUXLIGHT		0x10
#define TIMERFLAG_AUXHEATER		0x20
#define TIMERFLAG_AUXCOOLER		0x40
#define TIMERFLAG_AUXWAPER		0x80
#define TIMERFLAG_AUXPOWER		0x100

//TTYDEV …Ë±∏–≈œ¢
typedef struct tag_TTYINFO
{
//  char * strTtyDriverName;    // Device name , can't be empty
	int dwDevType;				// 
	int dwPortType;				// PORT_ISA, PORT_SERIAL can't combine
	int dwPtzType;				// PTZ_NO, PTZ_NORMAL, PTZ_FAST

	int dwBaudrate;				//÷ß≥÷µƒ≤®Ãÿ¬ 
	int dwTTYAuxFlags;			// PTZFLAG_AUXLIGHT ... or combine
	int dwPTZFlags;				// PTZFLAG_PANTITLT ... or combine
	int dwPTZTimerFlags;		//timer for some protocols need send data circlely
	int dwTimerPeriod;			//timer period for application program
	int dwPresetCount;			// ÷ß≥÷µƒ‘§÷√µ„ ˝,wType == PTZ_FAST
	int dwCruiseCount;			//÷ß≥÷µƒ—≤∫Ω ˝
	int dwTrackCount;			//÷ß≥÷µƒπÏº£ ˝
	int dwReserve1;				// ‰øùÁïôÂ≠ó 1
	int dwReserve2;				// ‰øùÁïôÂ≠ó 2
	int dwReserve3;				// ‰øùÁïôÂ≠ó 3
	int dwReserve4;				// ‰øùÁïôÂ≠ó 4
} TTYINFO;

//for PtzDevNormalCtrl function
typedef enum
{
	TTY_PTZ_INITIAL = 0x01,
	TTY_PTZ_UP = 0x02,
	TTY_PTZ_UP_AUTO = 0x03,
	TTY_PTZ_DOWN = 0x04,
	TTY_PTZ_DOWN_AUTO = 0x05,
	TTY_PTZ_LEFT = 0x06,
	TTY_PTZ_LEFT_AUTO = 0x07,
	TTY_PTZ_RIGHT = 0x08,
	TTY_PTZ_RIGHT_AUTO = 0x09,
	TTY_PTZ_LEFTUP = 0x10,
	TTY_PTZ_RIGHTUP = 0x11,
	TTY_PTZ_LEFTDOWN = 0x12,
	TTY_PTZ_RIGHTDOWN = 0x13,

	TTY_PTZ_UP_STOP = 0x14,
	TTY_PTZ_DOWN_STOP = 0x15,
	TTY_PTZ_LEFT_STOP = 0x16,
	TTY_PTZ_RIGHT_STOP = 0x17,
	TTY_PTZ_STOP = 0x1C,

	TTY_PTZ_ZOOMIN = 0x21,
	TTY_PTZ_ZOOMOUT = 0x22,
	TTY_PTZ_ZOOM_AUTO = 0x23,
	TTY_PTZ_ZOOM_STOP = 0x24,
	TTY_PTZ_IRIS_LARGE = 0x25,
	TTY_PTZ_IRIS_SMALL = 0x26,
	TTY_PTZ_IRIS_AUTO = 0x27,
	TTY_PTZ_IRIS_STOP = 0x28,
	TTY_PTZ_FOCUS_NEAR = 0x29,
	TTY_PTZ_FOCUS_FAR = 0x2a,
	TTY_PTZ_FOCUS_AUTO = 0x2b,
	TTY_PTZ_FOCUS_STOP = 0x2c,

	TTY_PTZ_CAMERA_MANUAL = 0x40,
} TTY_PTZ_CODE;

//for PtzDevAuxCtrl function
typedef enum
{
	TTY_AUX_LIGHT = 0x01,		//µ∆π‚
	TTY_AUX_HEATER = 0x02,		//º”»»
	TTY_AUX_COOLER = 0x03,		//÷∆¿‰
	TTY_AUX_WIPER = 0x04,		//”ÍπŒ
	TTY_AUX_POWER = 0x05,		//µÁ‘¥
} TTY_AUX_CODE;


//for PtzDevAutoCtrl function
typedef enum
{
	TTY_PTZ_AUTO = 0x01,		//◊‘∂Ø
	TTY_PTZ_MANUAL = 0x02,		// ÷∂Ø
	TTY_PTZ_SETROUTE = 0x03,	//ø™ º◊‘∂Ø—≤∫Ω
	TTY_PTZ_SPEED = 0x04,		//…Ë÷√‘∆Ã®ÀŸ∂»
	TTY_PTZ_DWELL = 0x05,		// …Ë÷√Õ£¡Ù ±º‰
} TTY_AUTO_CODE;


//for PtzDevPresetCtrl function
typedef enum
{
	TTY_PTZ_PRESETSET = 0x01,	//…Ë÷√‘§÷√µ„
	TTY_PTZ_PRESETMOVE = 0x02,	//…æ≥˝‘§÷√µ„
	TTY_PTZ_PRESETCLRALL = 0x03,	//…æ≥˝À˘”–‘§÷√µ„
	TTY_PTZ_PRESETCALL = 0x04,	//µ˜”√‘§÷√µ„
	TTY_PTZ_TRACKSET = 0x05,	//…Ë÷√πÏº£
	TTY_PTZ_TRACKEND = 0x06,	//…Ë÷√πÏº£Ω· ¯
	TTY_PTZ_TRACKCALL = 0x07,	//πÏº£ªÿµ˜
	TTY_PTZ_ROUTESET = 0x08,	//ø™ º…Ë÷√—≤∫Ω
	TTY_PTZ_ROUTEADDPOINT = 0x09,	//ÃÌº”—≤∫Ωµ„
	TTY_PTZ_ROUTEREMOVEPOINT = 0x10,	//…æ≥˝—≤∫Ωµ„
	TTY_PTZ_ROUTEEND = 0x11,	//Ω· ¯—≤∫Ω…Ë÷√
	TTY_PTZ_ROUTEREMOVE = 0x12,	//…æ≥˝—≤∫Ω–Ú¡–
} TTY_PRESET_CODE;

#endif
