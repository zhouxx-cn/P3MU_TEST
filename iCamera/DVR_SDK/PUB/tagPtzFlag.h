#ifndef __PTZ_FLAG_H_
#define __PTZ_FLAG_H_

//支持的设备
#define DRIVER_PTZ			0x01
#define DRIVER_IO			0x02
#define DRIVER_MATRIX		0x04
#define DRIVER_WATCHDOG		0x08
#define DRIVER_CFDOG		0x10
#define DRIVER_ADC			0x20

//支持的波特率
#define	BD1200		0x01
#define	BD2400		0x02
#define	BD4800		0x04
#define	BD9600		0x08
#define	BD19200		0x10
#define	BD38400		0x20
#define	BD57600		0x40
#define	BD115200	0x80

//PTZ
#define MAX_PTZ_SPEED_LEVEL	8
#define MAX_TYPE_PTZTYPE	8

#define PTZ_TYPE_NO			0
#define PTZ_TYPE_NORMAL		1
#define PTZ_TYPE_FAST		2
#define PTZ_TYPE_PARITY  	4

//定义返回值
#define	DLL_ERROR		(-1)	//参数或者命令错误
#define	DLL_INPUT_OK	(0)		//参数或者命令正确，但是执行错误
#define	DLL_OUTPUT_OK	(1)		//返回正确

//!云台协议功能标志
//##ModelId=49B86DE603B9
enum PTZ_FLAG
{

	//##ModelId=49BA217B003E
	PTZ_FLAG_PANTITLT = 0x01,

	//!变倍
	//##ModelId=49BA218303B9
	PTZ_FLAG_ZOOM = 0x02,

	//!焦距
	//##ModelId=49BA219D0203
	PTZ_FLAG_FOCUS = 0x04,

	//!光圈
	//##ModelId=49BA21A400CB
	PTZ_FLAG_IRIS = 0x08,

	//!灯光
	//##ModelId=49BB2EE30213
	PTZ_FLAG_AUXLIGHT = 0x10,

	//!加热器
	//##ModelId=49BB2EE30232
	PTZ_FLAG_AUXHEATER = 0x20,

	//!制冷
	//##ModelId=49BB2EE30251
	PTZ_FLAG_AUXCOOLER = 0x40,

	//!雨刷
	//##ModelId=49BB2EE30280
	PTZ_FLAG_AUXWIPER = 0x80,

	//!电源
	//##ModelId=49BB2EE3029F
	PTZ_FLAG_AUXPOWER = 0x100,

	//!预置点
	//##ModelId=49BB2EE302CE
	PTZ_FLAG_AUXPRESET = 0x200,

	//!轨迹
	//##ModelId=49BB2EE302EE
	PTZ_FLAG_AUXTRACK = 0x400,

	//!自动扫描
	//##ModelId=49BB2EE3031C
	PTZ_FLAG_AUXAUTO = 0x800,

	//!巡航
	//##ModelId=49BB2EE3034B
	PTZ_FLAG_AUXCRUISE = 0x1000,

	//!软巡航
	//##ModelId=49BB2EE3036B
	PTZ_FLAG_AUXSOFTWARECRUISE = 0x2000,

	//!
	//##ModelId=49BB2EE30399
	PTZ_FLAG_AUXTIMECRUISE = 0x4000,

	//!\菜单控制
	PTZ_FLAG_AUXMENUCTRL = 0x8000,

	//!\左上、左下、右上、右下
	PTZ_FLAG_AUXPANTITLT = 0x10000,

	//!\brief 预置点间自动扫
	PTZ_FLAG_AUXPRESETAUTO = 0x20000,

	//!\brief 支持电子PTZ参数设置
	PTZ_FLAG_AUXELETRONIC = 0x40000,

	PTZ_FLAG_AUXHWGUARD = 0x80000,

	PTZ_FLAG_RecvData = 0x100000
};
#endif
