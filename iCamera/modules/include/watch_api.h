#ifndef __LIBWATCH_H__
#define __LIBWATCH_H__


/*!
 *\file			watch_api.h
 *\brief			EDVR软件守护进程相关接口定义
 *\author		林云
 *\date			2008/07/26
 *\version		0.0.1
 */


#ifdef __cplusplus
extern "C"
{
#endif							/* __cplusplus */


//1 被监控任务总数为10，有效ID为0-9
#define MAX_WATCH_TASK_ID	(10)


/*! \fn int WATCH_AddTask (int nMaxNotifyTimeSpace, int *pnTaskID)
 *  \brief	添加一个监控任务
 *  \param nMaxNotifyTimeSpace		输入参数，通知消息最大时间间隔，
 超过该时间没有收到消息，设备将被重启， 单位为秒。
 *  \param pnTaskID						输出参数，分配的任务ID
 *  \param szTaskName		任务名字,以供后面输出提供给使用者看,字符长度不超过128
 *  \exception 无
 *  \return 0;成功;
 *  \return 其它,   对应各种错误码
 */
	int WATCH_AddTask(int nMaxNotifyTimeSpace, int *pnTaskID, const char *szTaskName);


/*! \fn int WATCH_RemoveTask(int nTaskID)
 *  \brief			删除一个监控任务
 *  \param 		nTaskID	被监控任务的ID，该ID为任务添加时，
 								由守护进程自动分配
 *  \exception 	无
 *  \return 		0;成功;
 *  \return 		其它,   对应各种错误码
 */
	int WATCH_RemoveTask(int nTaskID);


/*! \fn int WATCH_NotifyTaskLive(int nTaskID)
 *  \brief 	由被监控任务调用，通知守护进程，被监控
 				任务还在正常运行
 *  \param 		nTaskID	被监控任务的ID，该ID为任务添加时，
 								由守护进程自动分配
 *  \exception	无
 *  \return		 0;成功;
 *  \return		其它,   对应各种错误码
 */
	int WATCH_NotifyTaskLive(int nTaskID);


/*! \fn int WATCH_LOG (__const char *__restrict __format, ...)
 *  \brief 	由被监控任务调用，通知守护进程，被监控
 				任务还在正常运行
 *  \param 		nTaskID	被监控任务的ID，该ID为任务添加时，
 								由守护进程自动分配
 *  \exception	无
 *  \return		 0;成功;
 *  \return		其它,   对应各种错误码
 */
	int WATCH_LOG(__const char *__restrict __format, ...);


#ifdef __cplusplus
}
#endif							/* __cplusplus */


#endif							/* __LIBWATCH_H__ */
