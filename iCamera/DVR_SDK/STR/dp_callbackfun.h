#ifndef __DP_CALLBACKFUN_H__
#define __DP_CALLBACKFUN_H__



/*定义帧类型*/
#define PSS_FRAMETYPE_AV          0x8000	//音视频数据帧
#define PSS_FRAMETYPE_PICTURE     0x8001	//图片数据帧
#define PSS_FRAMETYPE_USER        0x8002	//用户自定义数据帧
#define PSS_FRAMETYPE_RECSTART    0x8003	//通道启动录像控制帧
#define PSS_FRAMETYPE_RECSTOP     0x8004	//通道停止录像控制帧
#define PSS_FRAMETYPE_POWERON     0x8005	//掉电控制帧


/*数据弹出器回调函数中事件类型*/
#define DATAPOP_EVENT_NONE      0x00	//无通知事件
#define DATAPOP_EVENT_TIME_JUMP 0x01	//跳到新的时间片
#define DATAPOP_EVENT_TIME_OVER 0x02	//指定的时间片查询回调结束
#define DATAPOP_EVENT_TIME_CURR 0x03	//当前数据查询完毕，即播放时间已经到达最新录像的数据时间
#define DATAPOP_EVENT_DISK_ERR  0x04	//异常结束事件，比如read或者seek硬盘失败，把该异常回调通知用户


/*!
 * \brief 数据弹出回调函数
 * \param hDataPopper 数据弹出器句柄
 * \param iChannel 数据对应的通道
 * \param BOOL bIndexFrame 是否为索引帧，true是，false不是
 * \param wFrameType 数据对应的类型
 * \param FrameTime 数据对应的时间
 * \param pBuf 数据对应的缓冲区地址
 * \param dwSize 数据对应的大小
 * \param EventID 发生的事件类型，如果是正常的数据弹出，EventID为DATAPOP_EVENT_NONE，其他事件时，帧相关的参数无效
 * \return
*/
#ifndef DATAPOP_CALLBACK
typedef int (*DATAPOP_CALLBACK) (HANDLE hDataPopper, int iChannel, int bIndexFrame, unsigned short wFrameType, time_t FrameTime, unsigned char *pBuf,
								 unsigned long dwSize, unsigned long EventID);
#endif

/*!
 * \brief 录像片段回调函数
 * \param hRecSegReporter 录像片段报告器句柄
 * \param iChannel 该录像片段对应的通道
 * \param tBeginTime 该录像片段对应的起始时间
 * \param tEndTime 该录像片段对应的结束时间
 * \param RecType 该录像片段对应的录像类型
 * \param iChannel 该录像片段对应的通道
 * \param pChContext 暂时没用
 * \return
 */
#ifndef SEGMENT_CALLBACK
typedef int (*SEGMENT_CALLBACK) (HANDLE hRecSegReporter, int iChannel, time_t tBeginTime, time_t tEndTime, unsigned short RecType, void *pChContext);

#endif
#endif
