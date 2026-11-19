#include <stdio.h>   
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>   
#include <sys/ipc.h>   
#include <sys/msg.h>   
#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>

#include "msg_net.h"
#include "global_gather.h"

#if 0
/* 清空整个消息队列 */
static int ClearMsg(stuMsgTask *pstuMsg)
{
    while(msgrcv(pstuMsg->msgid, pstuMsg, sizeof(stuMsgTask), 0, IPC_NOWAIT) != -1);
    
    return 0;
}

/* 销毁消息队列 */
static int MsgDestory(int msgid)
{
    int ret = 0;
    
    ret = msgctl(msgid, IPC_RMID, 0);
    if (ret)
    {
        fprintf(stderr, "Destory Message Queue Error：%s\n", strerror(errno));  
        return ret;
    }
    
	return ret;
}
#endif

/* 将inputbuf塞入消息队列msg_key里面 */
int SendInfoToMsg(const char *msg_key, const char *inputbuf)
{
  	int ret = 0;
    static int timeout_cnt = 0;
    stuMsgTask stuMsg;

    if (!msg_key || !inputbuf)
    {
		printf("NULL POINTER EXCEPTION\n");
        return -1;
    }

    memset(&stuMsg, 0, sizeof(stuMsg));

    key_t key = ftok(msg_key, 'a'); 
    if(key == -1)  
	{
		fprintf(stderr,"Creat Key Error：%s\n", strerror(errno));  
		return -1;
	}

	if((stuMsg.msgid=msgget(key, IPC_CREAT|IPC_EXCL|00666))==-1)  
	{  
		stuMsg.msgid = msgget(key, IPC_EXCL|00666);
		if (-1 == stuMsg.msgid)
		{
			perror("msgget");
			return -1;
		}
	}
	stuMsg.mtype = 2;	
	strncpy(stuMsg.buffer, inputbuf, strlen(stuMsg.buffer) - 1);	
	ret = msgsnd(stuMsg.msgid, &stuMsg, sizeof(stuMsg)-sizeof(long), IPC_NOWAIT);
	if (ret)
	{
	    /* 超时计数开始 */
	    timeout_cnt ++;
	    /* 如果消息队列满了，则需要清空消息队列，重新发送 */
	    perror("msgsnd");
	    usleep(500000);
        while(msgrcv(stuMsg.msgid, &stuMsg, sizeof(stuMsg), 0, IPC_NOWAIT) != -1);  //    ClearMsg(&stuMsg);
        msgsnd(stuMsg.msgid, &stuMsg, sizeof(stuMsg)-sizeof(long), IPC_NOWAIT);
	}
    
    /* 如果超时计数达到3次，则需要重建消息队列 */
    if (timeout_cnt == 3)
    {
        printf("Message Queue is timeout over 3 times! rebuild it!\n");
        msgctl(stuMsg.msgid, IPC_RMID, 0);                                           //   MsgDestory(msg_key);
        timeout_cnt = 0;
    }
	
	return 0;
}

/* 从消息队列msg_key获取信息放到outputbuf里面 */  
int GetInfoFromMsg(const char *msg_key, char *outputbuf)
{
  	int ret = 0;
    stuMsgTask stuMsg;

    if (!msg_key || !outputbuf)
    {
		printf("NULL POINTER EXCEPTION\n");
        return -1;
    }

    memset(&stuMsg, 0, sizeof(stuMsg));

    key_t key = ftok(msg_key, 'a'); 
    if(key == -1)  
	{
		fprintf(stderr,"Creat Key Error：%s\n", strerror(errno));  
		return -1;
	}

    stuMsg.msgid = msgget(key, IPC_EXCL|00666);
    if (-1 == stuMsg.msgid)
    {
        perror("msgget");
        return -1;
    }

	/* 接收消息队列 */  
	ret = msgrcv(stuMsg.msgid, &stuMsg, sizeof(stuMsg)-sizeof(long), 0, IPC_NOWAIT);  
	if (ret == -1)
	{
//		perror("msgrcv error!");
		return ret;
	}
	
	strncpy(outputbuf, stuMsg.buffer, strlen(stuMsg.buffer)+1);
//	printf("text=[%s] pid=[%d]\n", g_tMsgTask.buffer, getpid()); 

	return 0;  
}

