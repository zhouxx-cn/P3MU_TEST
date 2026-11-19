#include <stdio.h>
#include <string.h>
#include<pthread.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <unistd.h>
#include <stdarg.h>

#include "watch_api.h"
#include "sys_banned.h"

char szVersion_WATCH[] = "1.0.4.4014 Build 20081210";


typedef struct t_TaskMgr
{
	pthread_mutex_t m_lock;
	int m_nMsgID;
	unsigned char m_nIDList[MAX_WATCH_TASK_ID];
}
T_TASKMGR;


typedef struct t_MsgSendBuf
{
	int m_nType;
	int m_nMsg;
	int m_nTaskID;
	int m_nTimeSpace;
	char m_szTaskName[128];
}
T_MSGSBUF;

#define MSGBUF_LEN	(sizeof(T_MSGSBUF))


static int g_bWatchModInited = 0;

static T_TASKMGR g_tTaskMgr;


static void TerminateWatch(void)
{
	//1 销毁互斥锁
	pthread_mutex_destroy(&g_tTaskMgr.m_lock);

	g_bWatchModInited = 0;
}


static int InitWatch(void)
{
	key_t key;

	if (0 != g_bWatchModInited)
	{
		return 0;
	}

	memset(&g_tTaskMgr, 0, sizeof (g_tTaskMgr));

	//1 创建互斥锁
	pthread_mutex_init(&g_tTaskMgr.m_lock, NULL);

	//1 获取其消息队列Key
	key = ftok("/linux/msgqueue/higheasy/WatchTask", 'a');

	//1 打开消息队列
	g_tTaskMgr.m_nMsgID = msgget(key, IPC_CREAT | IPC_EXCL | 00666);
	if (-1 == g_tTaskMgr.m_nMsgID)
	{
		g_tTaskMgr.m_nMsgID = msgget(key, IPC_EXCL | 00666);
		if (-1 == g_tTaskMgr.m_nMsgID)
		{
			perror("[Watch Daemon] msgget");
			printf("[Watch Daemon] msgqueue open failed\n");

			TerminateWatch();
			return -1;
		}
	}

	g_bWatchModInited = 1;
	return 0;
}


/*! \fn int WATCH_AddTask (int nMaxNotifyTimeSpace, int *pnTaskID)
 *  \brief	添加一个监控任务
 *  \param nMaxNotifyTimeSpace		输入参数，通知消息最大时间间隔，
 超过该时间没有收到消息，设备将被重启， 单位为秒。
 *  \param pnTaskID						输出参数，分配的任务ID
 *  \param szTaskName		任务名字,以供后面输出提供给使用者看
 *  \exception 无
 *  \return 0;成功;
 *  \return 其它,   对应各种错误码
 */
int WATCH_AddTask(int nMaxNotifyTimeSpace, int *pnTaskID, const char *szTaskName)
{
	int i, reval;

	T_MSGSBUF msg_sbuf;

	if (0 != InitWatch())
	{
		printf("[WATCH] WATCH_AddTask() err, InitWatch() failed\n");
		return -1;
	}

	if (0 >= nMaxNotifyTimeSpace)
	{
		printf("[WATCH] WATCH_AddTask() err, nMaxNotifyTimeSpace = %d\n", nMaxNotifyTimeSpace);
		return -2;
	}

	if (NULL == pnTaskID)
	{
		printf("[WATCH] WATCH_AddTask() err, pnTaskID = %d\n", (int) pnTaskID);
		return -3;
	}

	if (0 == strcmp(szTaskName, ""))
	{
		printf("[WATCH] WATCH_AddTask() err, szTaskName is NULL\n");
		return -4;
	}

	pthread_mutex_lock(&g_tTaskMgr.m_lock);

	for (i = 0; i < MAX_WATCH_TASK_ID; ++i)
	{
		if (0 == g_tTaskMgr.m_nIDList[i])
		{
			break;
		}
	}

	if (i >= MAX_WATCH_TASK_ID)
	{
		printf("[WATCH] WATCH_AddTask() err, Task full, MaxID = %d\n", MAX_WATCH_TASK_ID);

		pthread_mutex_unlock(&g_tTaskMgr.m_lock);
		return -4;
	}

	//1 添加任务，消息为1
	msg_sbuf.m_nType = 10;
	msg_sbuf.m_nMsg = 1;
	msg_sbuf.m_nTaskID = i;
	msg_sbuf.m_nTimeSpace = nMaxNotifyTimeSpace;
	strncpy(msg_sbuf.m_szTaskName, szTaskName, sizeof(msg_sbuf.m_szTaskName));

	reval = msgsnd(g_tTaskMgr.m_nMsgID, &msg_sbuf, MSGBUF_LEN, IPC_NOWAIT);
	if (-1 == reval)
	{
		perror("[WATCH] WATCH_AddTask() msgsnd");
		printf("[WATCH] WATCH_AddTask() err, message send failed, nTaskID = %d\n", i);

		pthread_mutex_unlock(&g_tTaskMgr.m_lock);
		return -5;
	}

	*pnTaskID = i;
	g_tTaskMgr.m_nIDList[i] = 1;

	printf("[WATCH] WATCH_AddTask() ok, TaskID = %d\n", i);

	pthread_mutex_unlock(&g_tTaskMgr.m_lock);
	return 0;
}


/*! \fn int WATCH_RemoveTask(int nTaskID)
 *  \brief			删除一个监控任务
 *  \param 		nTaskID	被监控任务的ID，该ID为任务添加时，
 								由守护进程自动分配
 *  \exception 	无
 *  \return 		0;成功;
 *  \return 		其它,   对应各种错误码
 */
int WATCH_RemoveTask(int nTaskID)
{
	int reval;

	T_MSGSBUF msg_sbuf;

	if (0 != InitWatch())
	{
		printf("[WATCH] WATCH_RemoveTask() err, InitWatch() failed\n");
		return -1;
	}

	if ((nTaskID < 0) || (nTaskID >= MAX_WATCH_TASK_ID))
	{
		printf("[WATCH] WATCH_RemoveTask() err, nMaxTaskID = %d, nTaskID = %d\n", MAX_WATCH_TASK_ID, nTaskID);
		return -2;
	}

	pthread_mutex_lock(&g_tTaskMgr.m_lock);

	if (0 == g_tTaskMgr.m_nIDList[nTaskID])
	{
		printf("[WATCH] WATCH_RemoveTask() err, invalid nTaskID = %d\n", nTaskID);

		pthread_mutex_unlock(&g_tTaskMgr.m_lock);
		return -3;
	}

	//1 删除任务，消息为2
	msg_sbuf.m_nType = 10;
	msg_sbuf.m_nMsg = 2;
	msg_sbuf.m_nTaskID = nTaskID;
	msg_sbuf.m_nTimeSpace = 0;
	memset(msg_sbuf.m_szTaskName, 0, sizeof (msg_sbuf.m_szTaskName));

	reval = msgsnd(g_tTaskMgr.m_nMsgID, &msg_sbuf, MSGBUF_LEN, IPC_NOWAIT);
	if (-1 == reval)
	{
		perror("[WATCH] WATCH_RemoveTask() msgsnd");
		printf("[WATCH] WATCH_RemoveTask() err, message send failed, nTaskID = %d\n", nTaskID);

		pthread_mutex_unlock(&g_tTaskMgr.m_lock);
		return -4;
	}

	g_tTaskMgr.m_nIDList[nTaskID] = 0;

	printf("[WATCH] WATCH_RemoveTask() ok, nTaskID = %d\n", nTaskID);

	pthread_mutex_unlock(&g_tTaskMgr.m_lock);
	return 0;
}


/*! \fn int WATCH_NotifyTaskLive(int nTaskID)
 *  \brief 	由被监控任务调用，通知守护进程，被监控
 				任务还在正常运行
 *  \param 		nTaskID	被监控任务的ID，该ID为任务添加时，
 								由守护进程自动分配
 *  \exception	无
 *  \return		 0;成功;
 *  \return		其它,   对应各种错误码
 */
int WATCH_NotifyTaskLive(int nTaskID)
{
	int reval;
//printf("WATCH_NotifyTaskLive\n");
	T_MSGSBUF msg_sbuf;

	if (0 != InitWatch())
	{
		printf("[WATCH] WATCH_RemoveTask() err, InitWatch() failed\n");
		return -1;
	}

	if ((nTaskID < 0) || (nTaskID >= MAX_WATCH_TASK_ID))
	{
		printf("[WATCH] WATCH_NotifyTaskLive() err, nMaxTaskID = %d, nTaskID = %d\n", MAX_WATCH_TASK_ID, nTaskID);
		return -2;
	}

	pthread_mutex_lock(&g_tTaskMgr.m_lock);

	if (0 == g_tTaskMgr.m_nIDList[nTaskID])
	{
		//printf("[WATCH] WATCH_NotifyTaskLive() err, invalid nTaskID = %d\n", nTaskID);

		pthread_mutex_unlock(&g_tTaskMgr.m_lock);
		return -3;
	}

	//1 通知，消息为3
	msg_sbuf.m_nType = 10;
	msg_sbuf.m_nMsg = 3;
	msg_sbuf.m_nTaskID = nTaskID;
	msg_sbuf.m_nTimeSpace = 0;

	reval = msgsnd(g_tTaskMgr.m_nMsgID, &msg_sbuf, MSGBUF_LEN, IPC_NOWAIT);
	if (-1 == reval)
	{ 
		//perror("[WATCH] WATCH_NotifyTaskLive() msgsnd");
		printf("[WATCH] WATCH_NotifyTaskLive() err, message send failed, nTaskID = %d\n", nTaskID);
		
		pthread_mutex_unlock(&g_tTaskMgr.m_lock);
		return -4;
	}

	pthread_mutex_unlock(&g_tTaskMgr.m_lock);
	return 0;
}


int WATCH_LOG(__const char *__restrict __format, ...)
{
	char s[300];

	va_list argptr;

	int cnt;

	memset(s, 0, sizeof (s));

	va_start(argptr, __format);
	cnt = vsnprintf(s, sizeof(s), __format, argptr);
	va_end(argptr);

	printf("%s", s);

	return (cnt);
}
