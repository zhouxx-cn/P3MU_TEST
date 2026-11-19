#ifndef __MPU_LCD_API_H__
#define __MPU_LCD_API_H__

/* 输出调试信息 */
int MPU_LCD_DumpInfo();

/* status: 0提醒开始 1提醒结束 */
int MPU_LCD_SetScheduleRemindStatus(int nStatus);

/* status: 0连接中 1绑定中 2添加成功 3添加失败 */
int MPU_LCD_SetDevBindConnetStatus(int nStatus);

/* status: 0呼叫中 1视频通话中 2语音通话中 3呼叫未接听 4呼叫挂断 5等待接听 */
int MPU_LCD_SetCallStatus(int nStatus);

/* 视频通话喂流接口 */
int MPU_LCD_SetVideoCallData(const char *DataBuf, unsigned int BufSize);

/**
 * @brief 获取熄屏超时时间
 * @param seconds 超时秒数（0表示永不休眠）
 */
int MPU_LCD_GetSleepTimeout(unsigned int *seconds);

/**
 * @brief 设置熄屏超时时间
 * @param seconds 超时秒数（0表示永不休眠）
 */
int MPU_LCD_SetSleepTimeout(unsigned int seconds);

/**
 * @brief 获取屏幕亮度
 * @param level 亮度等级0-100
 */
int MPU_LCD_GetBrightness(unsigned int *level);

/**
 * @brief 设置屏幕亮度
 * @param level 亮度等级0-100
 */
int MPU_LCD_SetBrightness(unsigned int level);

/**
 * @brief 获取背景图序号
 * @param number 序号0-9
 */
int MPU_LCD_GetBackgroundImageNumber(unsigned int *number);

/**
 * @brief 设置背景图序号
 * @param number 序号0-9
 */
int MPU_LCD_SetBackgroundImageNumber(unsigned int number);

/**
 * @brief 设置背景图序号
 * @param number 序号0-9
 */
int MPU_LCD_SetBackgroundImageData(const char* imageData, unsigned int dataLen);

/**
 * @brief 唤醒LCD显示屏
 * @return 1则成功唤醒
 */
int MPU_LCD_WakeUp();

#endif
