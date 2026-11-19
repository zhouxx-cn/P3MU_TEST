#ifndef __LOCAL_ACTION_H_
#define __LOCAL_ACTION_H_

//!\brief 本地操作代码，包含系统设置、关闭系统、email联动、短信联动
//##ModelId=49ADDE8001F4
typedef enum
{
	//!\brief 系统设置
	//##ModelId=49ADDEFD00FA
	LOCAL_SETTINGS = 0x00,

	//!\关闭系统
	//##ModelId=49ADDEFD0128
	LOCAL_HALT = 0x01,

	//!\发送邮件通知
	//##ModelId=49ADDEFD0157
	LOCAL_EMAIL = 0x02,

	//!\发送短信通知
	//##ModelId=49ADDEFD0177
	LOCAL_SMS = 0x03,

	LOCAL_FILE = 0x04,

	LOCAL_BACKUP = 0x05
} LOCAL_ACTION;

#endif
