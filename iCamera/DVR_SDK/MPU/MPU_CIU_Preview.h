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

#endif							/* #ifndef _INC_MPU_CIU_PREVIEW_H_ */
