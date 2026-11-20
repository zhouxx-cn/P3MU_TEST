#ifndef _INC_MPU_CIU_PREVIEW_H_
#define _INC_MPU_CIU_PREVIEW_H_

//!\brief  语音对讲回调函数
//!\param   int iState    0，开始；1， 结束
//!\return  unsigned long dwChannel 通道
typedef int (*Talk_Callback) (int iState, unsigned long dwChannel);

int MPU_CIU_PreView_GetPreviewStream();

#if defined (__IS_SUPPORT_LCD_MANAGER__)
void GetExternPreviewCallState(int *state);
#endif

#ifdef __USE_CIRCLEQUEUE__
/* 前置声明, 避免头文件强依赖 MemRecord.h (RecordInterface 完整定义见 MemRecord.h) */
struct tagRecordInterface;

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * @brief 获取子码流(AUX, 240x320)循环缓冲区消费接口。
 * @details 用法参考录像写卡(MemRecord)：拿到接口后先 ifRecordUserAdd 添加消费者,
 *          再循环 ifRecordGetData 取子码流 H264 裸流(内部已剥离私有帧头),
 *          处理完调用 ifRecordReleaseData 释放; 退出时 ifRecordUserDelete。
 * @param pInterface [out] 取流回调集合
 * @return 0 成功, -1 失败
 */
int CIU_GetSubStreamRecordInterface(struct tagRecordInterface *pInterface);

#ifdef __cplusplus
}
#endif
#endif /* __USE_CIRCLEQUEUE__ */

#endif							/* #ifndef _INC_MPU_CIU_PREVIEW_H_ */
