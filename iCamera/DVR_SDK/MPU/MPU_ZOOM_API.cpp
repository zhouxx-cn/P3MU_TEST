#include "MPU_global.h"
#include "CZoomManager.h"

#ifdef __DIGITAL_ZOOM_DRAG_SUPPORT__

/**************************************************************************
 *                            局部宏定义                                  *
 **************************************************************************/

//调用报警输出管理器执行动作
#define MPU_ZOOM_CALL(x)											\
do																\
{																\
	int nRet;														\
																\
	CZoomManager *pMgr = CZoomManager::GetInstance();		\
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
		MPU_ERR("Get CZoomManager instance fail!\n");				\
		MPU_CHK(NULL != pMgr);									\
		nRet = IDVR_MPU_DOP_NO_MANAGER_INSTANCE;					\
	}															\
																\
	return nRet;													\
}																\
while (0)


//调用报警输出管理器获取参数
#define MPU_ZOOM_GET(DataType, DefaultValue, Function)				\
do																\
{																\
	DataType retValue;												\
																\
	CZoomManager *pMgr = CZoomManager::GetInstance();		\
																\
	if (NULL != pMgr)												\
	{															\
		retValue = pMgr->Function;									\
	}															\
	else															\
	{															\
		MPU_ERR("Get CZoomManager instance fail!\n");				\
		MPU_CHK(NULL != pMgr);									\
		retValue = DefaultValue;									\
	}															\
																\
	return retValue;												\
}																\
while (0)


//!\brief   ZOOM初始化
//!\return  0表示成功，其它为错误码
int ZOOM_Init()
{
    CZoomManager *pMg = CZoomManager::GetInstance();

    if (NULL != pMg)
    {
        pMg->Init();
        MPU_DBG("CZoomManager init ok\n");
    }
    else
    {
        MPU_ERR("no CZoomManager instance\n");
        MPU_CHK(NULL != pMg);
        return IDVR_MPU_STR_NO_DISK_MANAGER_INSTANCE;
    }

    return IDVR_OK;
}

//!\brief   逆初始化变倍管理
//!\return  0表示成功，其它为错误码
int ZOOM_Uninit()
{
    CZoomManager::ReleaseInstance();

    return IDVR_OK;
}

int MPU_ZOOM_SetRatio(double ratio,int eOpreator)
{
    MPU_ZOOM_CALL(SetRatio(ratio,eOpreator));
}

int MPU_ZOOM_GetRatio(double *ratio)
{
    MPU_ZOOM_CALL(GetRatio(ratio));
}

int MPU_ZOOM_SetPresetCfg(int index, double ratio)
{
    MPU_ZOOM_CALL(SetPresetCfg(index, ratio));
}

int MPU_ZOOM_GetPresetCfg(int index, double *ratio)
{
    MPU_ZOOM_CALL(GetPresetCfg(index, ratio));
}

int MPU_ZOOM_RemovePresetCfg(int index)
{
    MPU_ZOOM_CALL(RemovePresetCfg(index));
}

int MPU_ZOOM_SetDragZoomCmd(DRAG_ZOOM_CMD nCmd, int eOpreator)
{
    MPU_ZOOM_CALL(SetDragZoomCmd(nCmd, eOpreator));
}

int MPU_ZOOM_GetZoomLens(int *Lens)
{
    MPU_ZOOM_CALL(GetZoomLens(Lens));
}

#if defined __IS_SUPPORT_LONG_SHORT_FOCUS_ZOOM__
int MPU_ZOOM_SetZoomLens(int Lens)
{
    MPU_ZOOM_CALL(SetZoomLens(Lens));
}
#endif

#endif

