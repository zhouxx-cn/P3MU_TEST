#ifndef __REMOTE_ACTION_H_
#define __REMOTE_ACTION_H_
//!\brief 远程操作代码 
//!系统设置、关闭系统、串口输出、报警输出、远程升级、语音对讲

typedef enum
{
	//!远程系统设置
	//##ModelId=49ADDEFD01E4
	REMOTE_SETTINGS = 0x00,

	//!远程关机/重启
	//##ModelId=49ADDEFD0203
	REMOTE_HALT = 0x01,

	//!语音对讲
	//##ModelId=49ADDEFD0242
	REMOTE_VOICE = 0x02,

	//!透明通道
	//##ModelId=49ADDEFD0271
	REMOTE_COM = 0x03,

	//!报警输出
	//##ModelId=49ADDEFD029F
	REMOTE_ALARMOUT = 0x04,

	//!远程升级
	//##ModelId=49ADDEFD02DE
	REMOTE_UPDATE = 0x05
} REMOTE_ACTION;

typedef enum
{
	//!语音对讲
	REMOTE_VOICE_INTERCOM = 0x00,

	//!透明通道
	REMOTE_SERIAL_COM = 0x01,

	//!人脸配置
	REMOTE_FACE = 0x02,

	//!体温配置
	REMOTE_TEMP = 0x03,

	//!口罩配置
	REMOTE_MASK = 0x04,

	//!隐私配置
	REMOTE_PRIVACY = 0x05,

	//!开门配置
	REMOTE_OEPNDOOR = 0x06,

	//!影像配置
	REMOTE_IMAGE = 0x07,

	//!基础网络设置
	REMOTE_BASIC_NETWORK = 0x08,

	//!高级网络设置
	REMOTE_ADVANCE_NETWORK = 0x09,

	//!编码设置
	REMOTE_CODE = 0x0a,

	//!报警设置
	REMOTE_ALARM = 0x0b,

	//!基本系统设置
	REMOTE_BASIC_SYSTEM = 0x0c,

	//!高级系统设置 
	REMOTE_ADVANCE_SYSTEM = 0x0d,

	//!系统升级
	REMOTE_SYSTEM_UPDATE = 0x0e,
	
} REMOTE_ACTION_V1;
#endif
