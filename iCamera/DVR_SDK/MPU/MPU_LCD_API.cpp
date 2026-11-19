#include "MPU_global.h"

#if defined(__IS_SUPPORT_LCD_MANAGER__)
#include "LCD/ScreenManager.h"

//调用LCD管理器执行动作
#define MPU_LCD_CALL(x)											\
do																\
{																\
	int nRet;														\
																\
	ScreenManager *pMgr = ScreenManager::GetInstance();		\
																\
	if (NULL != pMgr)												\
	{															\
		nRet = pMgr->x;											\
																\
		if (IDVR_OK != nRet)										\
		{														\
			MPU_ERR("%s, nRet = %d\n", #x, nRet);					\
		}														\
	}															\
	else															\
	{															\
		MPU_ERR("Get ScreenManager instance fail!\n");				\
		MPU_CHK(NULL != pMgr);									\
		nRet = IDVR_MPU_TPC_NO_MANAGER_INSTANCE;					\
	}															\
																\
	return nRet;													\
}																\
while (0)

//调用LCD管理器获取参数
#define MPU_LCD_GET(DataType, DefaultValue, Function)				\
do																\
{																\
	DataType retValue;												\
																\
	ScreenManager *pMgr = ScreenManager::GetInstance();		\
																\
	if (NULL != pMgr)												\
	{															\
		retValue = pMgr->Function;									\
	}															\
	else															\
	{															\
		MPU_ERR("Get ScreenManager instance fail!\n");				\
		MPU_CHK(NULL != pMgr);									\
		retValue = DefaultValue;									\
	}															\
																\
	return retValue;												\
}																\
while (0)

//!\brief   LCD初始化
//!\return  0表示成功，其它为错误码
int LCD_Init()
{
    ScreenManager *pMg = ScreenManager::GetInstance();

    if (NULL != pMg)
    {
        MPU_DBG("ScreenManager init ok\n");
    }
    else
    {
        MPU_ERR("no ScreenManager instance\n");
        MPU_CHK(NULL != pMg);
        return IDVR_MPU_STR_NO_DISK_MANAGER_INSTANCE;
    }
    return IDVR_OK;
}

//!\brief   逆初始化LCD管理
//!\return  0表示成功，其它为错误码
int LCD_Uninit()
{
    ScreenManager::ReleaseInstance();
    return IDVR_OK;
}
#else
//调用系统管理器执行动作
#define MPU_LCD_CALL(x)									\
do														\
{														\
	return IDVR_OK;										\
}														\
while (0)

//调用系统管理器获取参数
#define MPU_LCD_GET(DataType, DefaultValue, Function)		\
do														\
{														\
	return IDVR_OK;										\
}														\
while (0)
#endif

int MPU_LCD_DumpInfo()
{
	MPU_LCD_CALL(DumpInfo());
}

int MPU_LCD_SetScheduleRemindStatus(int nStatus)
{
	MPU_LCD_CALL(ExternSetScheduleRemindStatus(nStatus));
}

int MPU_LCD_SetDevBindConnetStatus(int nStatus)
{
	MPU_LCD_CALL(ExternSetDevBindConnetStatus(nStatus));
}

int MPU_LCD_SetCallStatus(int nStatus)
{
	MPU_LCD_CALL(ExternSetCallStatus(nStatus));
}

int MPU_LCD_SetVideoCallData(const char *DataBuf, unsigned int BufSize)
{
	MPU_LCD_CALL(SetVideoCallStreamData(DataBuf, BufSize));
}

int MPU_LCD_GetSleepTimeout(unsigned int *seconds)
{
	MPU_LCD_CALL(GetSleepTimeout(seconds));
}

int MPU_LCD_SetSleepTimeout(unsigned int seconds)
{
	MPU_LCD_CALL(SetSleepTimeout(seconds));
}

int MPU_LCD_GetBrightness(unsigned int *level)
{
	MPU_LCD_CALL(GetBrightness(level));
}

int MPU_LCD_SetBrightness(unsigned int level)
{
	MPU_LCD_CALL(SetBrightness(level));
}

int MPU_LCD_GetBackgroundImageNumber(unsigned int *number)
{
	MPU_LCD_CALL(GetBackgroundImageNumber(number));
}

int MPU_LCD_SetBackgroundImageNumber(unsigned int number)
{
	MPU_LCD_CALL(SetBackgroundImageNumber(number));
}

int MPU_LCD_SetBackgroundImageData(const char* imageData, unsigned int dataLen)
{
	MPU_LCD_CALL(SetBackgroundImageData(imageData, dataLen));
}

int MPU_LCD_WakeUp()
{
	MPU_LCD_GET(int, 0, WakeUp());
}

