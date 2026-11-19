#ifndef __CHANNEL_ACTION_H_
#define __CHANNEL_ACTION_H_

//!\brief 通道操作代码，包含预览，回放、云台控制、手动录像
//##ModelId=49ADDE5C01D4
typedef enum
{
	//!\brief 通道预览
	//##ModelId=49ADDEFD0000
	CHANNEL_PREVIEW = 0x00,

	//!\brief 通道回放
	//##ModelId=49ADDEFD002E
	CHANNEL_PLAYBACK = 0x01,

	//!\brief 云台控制
	//##ModelId=49ADDEFD004E
	CHANNEL_PTZCTRL = 0x02,

	//!\brief 手动录像
	//##ModelId=49ADDEFD007D
	CHANNEL_MANUALRECORD = 0x03
} CHANNEL_ACTION;
#endif
