#ifndef	_INIT_H_
#define	_INIT_H_

#include "Common.h"
#include "EncoderLib.h"
#include "Hi_MPP.h"


#define BLEND_SHIFT  6 
#define ALPHA        64           // 1 
#define BELTA        48           // 0.85 
#define AF_BLOCK_8X8 1

//#include "DecoderLib.h"

/* 全局资源初始化 */
int Zview_Res_Init(char *pInfo);

/* 全局资源反初始化 */
int Zview_Res_DeInit();

int Zview_SDK_Init();

int Zview_SDK_DeInit();

int Zview_SlaveDev_Init(SystemInfo_t *pSysInfo);

int Zview_SlaveDev_DeInit(SystemInfo_t *pSysInfo);

/* 语音对讲通道初始化 */
int Zview_TalkingChInfo_Init(SystemInfo_t *pSysInfo);

int Zview_TalkingChInfo_DeInit();

/* 创建SDK需要的线程 */
int Zview_CreatThread();

/* 退出SDKZview */
int Zview_ExitThread();

int Zview_EncChInfo_Init(SystemInfo_t *pSysInfo);

int Zview_EncChInfo_DeInit();

int AF_Send_MCU_data(CChannelContext *pChannelContext, void * para);

#endif
