#ifndef __MSG_H__
#define __MSG_H__

/*@ 
 *@ file	 msg_net.h
 *@ brief	 消息队列相关操作对外接口
 *@ author	 樊聪
 *@ date	 2016年3月17日 20:31:08
 *@ version	 0.0.3
 */

#define BUFFER    (32)

typedef struct msgtype 
{  
	int  msgid;
	long mtype;  
	char buffer[BUFFER+1];  
}stuMsgTask;  

#ifdef __cplusplus
extern "C" {
#endif

/* 将inputbuf塞入消息队列msg_key里面 */
int SendInfoToMsg(const char *msg_key, const char *inputbuf);

/* 从消息队列msg_key获取信息放到outputbuf里面 */
int GetInfoFromMsg(const char *msg_key, char *outputbuf);

#ifdef __cplusplus
}
#endif

#endif // end of __MSG_H__
