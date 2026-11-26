#ifndef _MM_RECORD_H_
#define _MM_RECORD_H_

#ifdef __USE_CIRCLEQUEUE__
//!\brief 录像添加循环缓冲区消费者
//!\param DWORD dwChannel 通道，从0开始
//!\param unsigned int dwStreamType   码流类型
//!\param DWORD *pdwConnectionId 输出参数，连接ID
//!\return 
typedef int (*IF_RecordUserAdd) (unsigned int dwChannel,unsigned int dwStreamType,unsigned int *pdwConnectionId);

//!\brief 录像删除循环缓冲区消费者
//!\param unsigned int pdwConnectionId 连接ID
//!\return 
typedef int (*IF_RecordUserdelete)(unsigned int pdwConnectionId);

//!\brief 录像获取循环缓冲区数据
//!\param unsigned int pdwConnectionId 连接ID
//!\param char **pBuffer 流数据
//!\param unsigned int *pLen 数据长度
//!\return 
typedef int (*IF_RecordGetData)(unsigned int dwConnectionId, char **pBuffer,unsigned int *pLen);

//!\brief 录像释放循环缓冲区数据
//!\param unsigned int pdwConnectionId 连接ID
//!\param char *pBuffer 流数据
//!\param unsigned int pLen 数据长度
//!\return 
typedef int (*IF_RecordReleaseData)(unsigned int dwConnectionId, char *pBuffer,unsigned int pLen);


/*!
*  @struct    tagRecordInterface
*  @brief    录像获取帧数据接口合集
*/
typedef struct tagRecordInterface
{
    IF_RecordUserAdd ifRecordUserAdd;    /*!< 消费者添加请求接口*/
    IF_RecordUserdelete ifRecordUserDelete;    /*!< 消费者删除请求接口*/
    IF_RecordGetData ifRecordGetData;    /*!< 消费者获取数据请求接口       */
    IF_RecordReleaseData ifRecordReleaseData;   /*!< 消费者释放数据*/
} RecordInterface, *LPRecordInterface;
#endif

#ifdef __cplusplus
extern "C" {
#endif

	/*!
	* @brief
	*	设置录像取流数据回调
	* @param pInterface
	*	拉流方式接口信息
	* @return
	*	返回0表示成功, 否则表示错误码
	* @note
	*	接口类型: 阻塞式
	*/
	int REC_SetRecordInterface(const LPRecordInterface pInterface);	

    typedef struct tagMemRecordInitParam
    {
        int nMaxFrameSize;
        int nRingBufSize;
    } MemRecordInitParam;

     /* 初始化 */
	int MemRecordInit(void);

    int MemRecordInitV2(MemRecordInitParam *pInitParam);

    /* 反初始化 */
    void MemRecordExit(void);

    /**
     * @brief 暂停模块功能
     */
    int MemRecord_Pause ();

    /**
     * @brief 恢复模块功能
     */
    int MemRecord_Resume ();

    /* 设置系统预录时间(单位: 秒) */
    void SetPrevTimeSec(unsigned int prev_time_sec);    

    /* 获取系统预录时间 */
    int GetPrevTimeSec(void);

    /**
     * 设置录像延迟时间
     */
    int MEMRECORD_SetDelayTime (unsigned int nDelaySecs);

    /**
     * 获取录像延迟时间
     */
    int MEMRECORD_GetDelayTime (unsigned int *pnDelaySecs);

    /* 获取触发录像类型 */
    int GetTrigRecordType(void);
    
    /* 设置触发录像类型 */
    int SetTrigRecordType(int record_type);
    
    /* 开始录像 */
    int REC_Start (int record_type, int bH265);
	
    /* 停止录像 */
    int REC_Stop ();

    /*@breif SetRecordStatus 设置录像的写入状态
     *@param IN status 写入状态 0:暂停  1:继续写入 参见enumRecStatus
     */
    int SetRecordWriteStatus(int status);

    /* 触发录像完毕之后，发送消息的回调函数 */
    int SetTrigRecordOverCallBack(int (*CallBack)(unsigned char *));

    /*@breif 从环形缓冲区里面获取一帧数据写成AVI到SD卡
     *@param IN frame_size   帧大小
     *@param IN frame_buffer 帧缓冲
     *@note 不可以塞死
     */
    int  RawDataWirteProcess(int frame_size, const char * frame_buffer);

	/* 设置用户录像类型 */
	void SetUsrRecordType(int type,int enable);

    /* 获取录像类型和使能*/
    void GetUsrRecordType(int *type,int *enable);
    
#ifdef __cplusplus
}
#endif
#endif
