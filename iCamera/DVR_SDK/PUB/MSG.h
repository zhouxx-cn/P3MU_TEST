
/*!
 *\file		MSG.h
 *\brief	iDVR内部消息结构定义
 *\author	林云
 *\date		2009/03/20
 */

#ifndef _INC_MSG_H_
#define _INC_MSG_H_


//MSG_STRUCT结构定义
struct tag_MsgStruct
{
	unsigned long nMsgLen;		//本消息结构的长度
	int nMsg;					//消息名
	int nParam1;				//参数1
	int nParam2;				//参数2
	int nParam3;				//参数3
	int nParam4;				//参数4
	//消息扩展项 bOpt = 1表示有效;
	int bOpt;					//消息扩展项 bOpt = 1表示有效;
	int nOptlen;				//扩展项字节长度
	unsigned char *pOpt;		//扩展项指针 

	  tag_MsgStruct()
	{
		nMsgLen = sizeof (struct tag_MsgStruct);
		  nMsg = 0;
		  nParam1 = 0;
		  nParam2 = 0;
		  nParam3 = 0;
		  nParam4 = 0;
		  bOpt = 0;
		  nOptlen = 0;
		  pOpt = 0;
	}
};

//EMSG类型定义
typedef struct tag_MsgStruct EMSG;

//EMSG指针类型定义
typedef EMSG *PEMSG;

//EMSG回调函数类型定义
typedef int (*MSG_CALLBACK) (PEMSG pMessage, void *pContext);


//通知消息
enum
{
	MSG_AVP_BASE,				//回放消息起始

	/*
	   播放结束,需上层异步调用停止回放
	   nParam1  为结束原因
	   0表示文件结束
	   1表示结束时间到
	   2表示播放已达到最新的录像数据时间
	   3表示硬盘上数据异常
	 */
	MSG_AVP_NOTITY_STOP,

	/*
	   查询下一个浏览点
	   nParam1  为当前浏览时间点
	   nParam2  为当前播放方向
	   返回值
	   没有数据则返回开始时间或结束时间(看方向)
	 */
	MSG_AVP_BROWSE_NEXT,
};


#endif							//_INC_MSG_H_
