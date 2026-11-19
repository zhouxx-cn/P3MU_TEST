#if defined(__DIGITAL_ZOOM_DRAG_SUPPORT__)
#include <math.h>
#include <pthread.h>
// #include <stddef.h>

#include "Zoom.h"
#include "halmod.h"
#include "Common.h"
#include "EncoderLib.h"
#include "Hi_MPP.h"
// #include "vi.h"
// #include "visp.h"
#include "vpss.h"

static int GetRatioOpreatorPriority(int eOpreator)
{
	int nPriority = 0xff;

	if (DRAG_CLIENT_RADIO == eOpreator)
	{
		nPriority = 0;
	}
	else if (DRAG_CLIENT_ZOOM == eOpreator)
	{
		nPriority = 0;
	}
	else if (DRAG_CRUISE_RADIO == eOpreator)
	{
		nPriority = 1;
	}
	else if (DRAG_TRACK_RADIO == eOpreator)
	{
		nPriority = 2;
	}
	else if (DRAG_NONE == eOpreator)
	{
		nPriority = 0xff;
	}

	return nPriority;
}

int Zoom_SetTrackZoomEn(int TrackZoomEn)
{
    CChannelContext *pChannelContext = NULL;
    HY_Enc_GetEncChInfo(&pChannelContext);
    if (pChannelContext == NULL)
    {
        ZW_ERROR("Param Err!\n");
        return -1;
    }

    if (TrackZoomEn != 0 && TrackZoomEn != 1)
    {
        ZW_ERROR("Zoom TrackZoomEn:%d is error!\n", TrackZoomEn);
        return -1;
    }

	pthread_mutex_lock(&pChannelContext->m_DragZoomLock);
    pChannelContext->m_ZoomDragEn  = TrackZoomEn;
    pthread_mutex_unlock(&pChannelContext->m_DragZoomLock);

    return 0;
}

int Zoom_SetDragZoomRatio(double ratio,int eOpreator)
{
    CChannelContext *pChannelContext = NULL;
    HY_Enc_GetEncChInfo(&pChannelContext);
    if (pChannelContext == NULL)
    {
        ZW_ERROR("Param Err!\n");
        return -1;
    }

	pthread_mutex_lock(&pChannelContext->m_DragZoomLock);

	if (GetRatioOpreatorPriority(eOpreator) > GetRatioOpreatorPriority(pChannelContext->m_ZoomOpreate))
	{//优先级低于当前正在操作的优先级
		ZW_ERROR("eOpreator(%d) less m_ZoomOpreate(%d)! ratio(%f)\n", eOpreator,pChannelContext->m_ZoomOpreate,ratio);

		pthread_mutex_unlock(&pChannelContext->m_DragZoomLock);
		return 0;
	}

    if (ratio < DRAG_ZOOM_MIN_RATIO || ratio > pChannelContext->m_ZoomUserMaxRatio)
    {
        ZW_ERROR("Zoom ratio:%f is error!\n", ratio);

		pthread_mutex_unlock(&pChannelContext->m_DragZoomLock);
        return -1;
    }


    pChannelContext->m_ZoomTrueRatio  = ratio;
	pChannelContext->m_ZoomOpreate = eOpreator;
    pthread_mutex_unlock(&pChannelContext->m_DragZoomLock);

    ZW_DEBUG("Drag m_ZoomOpreate(%d) ZoomRatio:[%0.1f, %0.1f]\n", eOpreator,pChannelContext->m_ZoomCurRatio, pChannelContext->m_ZoomTrueRatio);
    return 0;
}

int Zoom_GetDragZoomRatio(double *ratio)
{
    CChannelContext *pChannelContext = NULL;
    HY_Enc_GetEncChInfo(&pChannelContext);
    if (pChannelContext == NULL)
    {
        ZW_ERROR("Param Err!\n");
        return -1;
    }

    pthread_mutex_lock(&pChannelContext->m_DragZoomLock);
    *ratio = pChannelContext->m_ZoomCurRatio;
    pthread_mutex_unlock(&pChannelContext->m_DragZoomLock);

    return 0;
}

//设置变倍命令
int Zoom_SetDragZoomCmd(DRAG_ZOOM_CMD nCmd,int eOpreator)
{
    CChannelContext *pChannelContext = NULL;
    HY_Enc_GetEncChInfo(&pChannelContext);
    if (pChannelContext == NULL)
    {
        ZW_ERROR("Param Err!\n");
        return -1;
    }

	if (GetRatioOpreatorPriority(eOpreator) > GetRatioOpreatorPriority(pChannelContext->m_ZoomOpreate))
	{//优先级低于当前正在操作的优先级
		ZW_ERROR("eOpreator(%d) less m_ZoomOpreate(%d)!\n", eOpreator,pChannelContext->m_ZoomOpreate);
		return 0;
	}

    pthread_mutex_lock(&pChannelContext->m_DragZoomLock);

	if ((DRAG_CMD_ZOOM_STOP == nCmd) && (DRAG_CLIENT_ZOOM != pChannelContext->m_ZoomOpreate))
	{
		pthread_mutex_unlock(&pChannelContext->m_DragZoomLock);
		//已经停止了，不响应停止指令
		ZW_ERROR("Stoped!\n");
		return 0;
	}

	if (DRAG_CMD_ZOOM_STOP == nCmd)
	{
		pChannelContext->m_ZoomDragCmdStop = 1;
	}
	else
	{
    	pChannelContext->m_ZoomDragCmd = nCmd;
		pChannelContext->m_ZoomOpreate = eOpreator;
		pChannelContext->m_ZoomDragCmdStop = 0;
		pChannelContext->m_ClickFirst = 0;
		pChannelContext->m_ZoomExecFlag = 0;
	}
    pthread_mutex_unlock(&pChannelContext->m_DragZoomLock);

    ZW_DEBUG("Drag ZoomCmd:%d  nCmd:%d  m_ZoomDragCmdStop:%d\n", pChannelContext->m_ZoomDragCmd,nCmd,pChannelContext->m_ZoomDragCmdStop);

    return 0;
}

//获取变倍状态
int Zoom_GetDragZoomChgStatus(int *status)
{
    CChannelContext *pChannelContext = NULL;
    HY_Enc_GetEncChInfo(&pChannelContext);
    if (pChannelContext == NULL)
    {
        ZW_ERROR("Param Err!\n");
        return -1;
    }

    *status = pChannelContext->m_ZoomChgStatus;

    return 0;
}

int Zoom_SetDragZoomScaleMagnification(double ratio)
{
    CChannelContext *pChannelContext = NULL;
    HY_Enc_GetEncChInfo(&pChannelContext);
    if (pChannelContext == NULL)
    {
        ZW_ERROR("Param Err!\n");
        return -1;
    }

    pChannelContext->m_ZoomUserMaxRatio = ratio;

    return 0;
}

int Zoom_GetDragZoomRange(double *min,double *max)
{
	CChannelContext *pChannelContext = NULL;
    HY_Enc_GetEncChInfo(&pChannelContext);
    if (pChannelContext == NULL)
    {
        ZW_ERROR("Param Err!\n");
        return -1;
    }

	*min = DRAG_ZOOM_MIN_RATIO;
	*max = pChannelContext->m_ZoomUserMaxRatio;

	return 0;
}

double Zoom_AnalyseDragCmdEx(DRAG_ZOOM_CMD nCmdEx, double nCurRatio, double nMaxRatio)
{
    double nNewRatio = 1.0f;

    if (DRAG_CMD_ZOOMIN == nCmdEx)
    {
    	nNewRatio = (nCurRatio + DRAG_ZOOM_RATIO_STEP > nMaxRatio) ? nMaxRatio : (nCurRatio + DRAG_ZOOM_RATIO_STEP);
    }
    else if ( DRAG_CMD_ZOOMOUT == nCmdEx)
    {
    	nNewRatio = (nCurRatio - DRAG_ZOOM_RATIO_STEP < DRAG_ZOOM_MIN_RATIO) ? DRAG_ZOOM_MIN_RATIO : (nCurRatio - DRAG_ZOOM_RATIO_STEP);
    }
    else
    {
        nNewRatio = nCurRatio;
    }
    return nNewRatio;
}

void Zoom_StopDragZoom()
{
	CChannelContext *pChContext = NULL;

	HY_Enc_GetEncChInfo(&pChContext);

	pthread_mutex_lock(&pChContext->m_DragZoomLock);
	pChContext->m_DragRunFlag = 0;
	pthread_mutex_unlock(&pChContext->m_DragZoomLock);

	ZW_DEBUG("m_DragRunFlag(%d)\n",pChContext->m_DragRunFlag);
}

int Zoom_StartDragZoom()
{
	CChannelContext *pChContext = NULL;
	int ret = 0;

	VPSS_CENTER_ZOOM_ATTR_S zoomAttr = {0};

	HY_Enc_GetEncChInfo(&pChContext);

	CPlatformChannelContext *pPlatCtx = (CPlatformChannelContext *)pChContext->pPlatformChannelhandle;
	double tmpRadio = 1.0f;
#if defined(__IS_SUPPORT_LONG_SHORT_FOCUS_ZOOM__)
    int changeRatio = 0;
    int needSensorSwitch = 0;
    int targetSensor = 0;
#endif

	pthread_mutex_lock(&pChContext->m_DragZoomLock);

#if defined(__IS_SUPPORT_LONG_SHORT_FOCUS_ZOOM__)
    #if defined(__PRODUCT_G85T__) || defined (__PRODUCT_W85T__) ||defined (__PRODUCT_G43TA__)
    targetSensor = pChContext->m_ZoomCurRatio < pChContext->m_WideMaxRatio ? 0 : 1;
    #elif defined(__PRODUCT_G86T__)
    targetSensor = pChContext->m_ZoomCurRatio < pChContext->m_WideMaxRatio ? 0 :
    pChContext->m_ZoomCurRatio < pChContext->m_WideMaxRatio + pChContext->m_MiddleMaxRatio ? 1 : 2;
    #endif
    if (targetSensor != 0) needSensorSwitch = 1;
    ret = Zoom_SwicthFocusOpt(needSensorSwitch, targetSensor, &changeRatio);
    if (0 != ret)
	{
		ZW_ERROR("SwicthFocusOpt failed! needSensorSwitch(%d) targetSensor(%d) HeightRatio(%d)\n",needSensorSwitch,targetSensor,changeRatio);
	}
#endif

    Zoom_RatioConverseAdapt(pChContext->m_ZoomCurRatio, &tmpRadio);

	tmpRadio = sqrt(tmpRadio);
	zoomAttr.Enable = 1;
	zoomAttr.WidthRatio = 1.0f / tmpRadio;
	zoomAttr.HeightRatio = 1.0f / tmpRadio;

	ret = Vpss_Grp_Set_CenterZoom(0, pPlatCtx->m_VPSSGroupNum[FIRST_SENSOR], zoomAttr);
	if (0 != ret)
	{
		ZW_ERROR("Vpss_Grp_Set_CenterZoom failed! ret = %d  Enable(%d) WidthRatio(%f) HeightRatio(%f)\n",ret,zoomAttr.Enable,zoomAttr.WidthRatio,zoomAttr.HeightRatio);
	}

	pChContext->m_DragRunFlag = 1;

    pChContext->m_ZoomOsdValue = pChContext->m_ZoomCurRatio;

	pChContext->m_DragUseOsd = 1;
	//HY_Enc_SetZoomOSDBitMap(pChContext, MAX_STREAM_COUNT, 1);

	pthread_mutex_unlock(&pChContext->m_DragZoomLock);

	ZW_DEBUG("m_DragRunFlag(%d)\n",pChContext->m_DragRunFlag);
    return 0;
}

#if defined(__IS_SUPPORT_LONG_SHORT_FOCUS_ZOOM__)

#define ZOOM_EPSILON 1e-5f
#define ZOOM_WIDE_FOCUS 0
#define ZOOM_MIDDLE_FOCUS 1
#define ZOOM_TELE_FOCUS 2

int Zoom_GetEachFocusZoom(double *shortZoom, double *middleZoom, double *longZoom)
{
    CChannelContext *pChannelContext = NULL;
    HY_Enc_GetEncChInfo(&pChannelContext);
    if (pChannelContext == NULL)
    {
        ZW_ERROR("Param Err!\n");
        return -1;
    }

    pthread_mutex_lock(&pChannelContext->m_DragZoomLock);
    *shortZoom = 1.0;
    *middleZoom = pChannelContext->m_WideMaxRatio;
    *longZoom = pChannelContext->m_WideMaxRatio + pChannelContext->m_MiddleMaxRatio;
    pthread_mutex_unlock(&pChannelContext->m_DragZoomLock);

    return 0;
}

void Zoom_SwitchFocusTiming(double curRatio, double newRatio, int *needSwitch, int *targetSensor)
{
    CChannelContext *pChCtx = NULL;
	HY_Enc_GetEncChInfo(&pChCtx);
    const double WIDE_MIDDLE_SWITCH_RATIO = pChCtx->m_WideMaxRatio;
    #if defined(__PRODUCT_G86T__)
    const double MIDDLE_TELE_SWITCH_RATIO = pChCtx->m_WideMaxRatio + pChCtx->m_MiddleMaxRatio;
    #endif

    if (pChCtx->m_CurrentSensor == ZOOM_WIDE_FOCUS)
    {
        if (newRatio >= WIDE_MIDDLE_SWITCH_RATIO - ZOOM_EPSILON && curRatio < WIDE_MIDDLE_SWITCH_RATIO - ZOOM_EPSILON)
        {
            *needSwitch = 1;
            *targetSensor = ZOOM_MIDDLE_FOCUS;
        }
    }
    else if (pChCtx->m_CurrentSensor == ZOOM_MIDDLE_FOCUS)
    {
        if (newRatio <= WIDE_MIDDLE_SWITCH_RATIO - ZOOM_EPSILON && curRatio >= WIDE_MIDDLE_SWITCH_RATIO - ZOOM_EPSILON)
        {
            *needSwitch = 1;
            *targetSensor = ZOOM_WIDE_FOCUS;
        }

    #if defined(__PRODUCT_G86T__)
        if (newRatio >= MIDDLE_TELE_SWITCH_RATIO - ZOOM_EPSILON && curRatio < MIDDLE_TELE_SWITCH_RATIO - ZOOM_EPSILON)
        {
            *needSwitch = 1;
            *targetSensor = ZOOM_TELE_FOCUS;
        }
    #endif
    }
    #if defined(__PRODUCT_G86T__)
    else
    {
        if (newRatio <= MIDDLE_TELE_SWITCH_RATIO - ZOOM_EPSILON && curRatio >= MIDDLE_TELE_SWITCH_RATIO - ZOOM_EPSILON)
        {
            *needSwitch = 1;
            *targetSensor = ZOOM_MIDDLE_FOCUS;
        }
    }
    #endif

    return;
}

int Zoom_SwicthFocusOpt(int switchFlg, int targetSensor, int *updRatio)
{
    CChannelContext *pChCtx = NULL;
    HY_Enc_GetEncChInfo(&pChCtx);
    SINT32 bUseViFuncForSel0 = 1;
    if (pChCtx == NULL) {
        ZW_ERROR("GetEncChInfo failed!\n");
        goto err_exit;
    }

    if (!switchFlg || targetSensor == pChCtx->m_CurrentSensor) {
        return 0;
    }

    if ((access(PRODUCTTEST_FILE, F_OK) == 0) || HY_Enc_GetProductMode())
        bUseViFuncForSel0 = 0;
#if !defined(__PRODUCT_G43TA__)
    static double dummyRatio = 0.0;
    const double RATIO_STEP = 0.2;
    dummyRatio = pChCtx->m_FocusCurRatio;
	Visp_Get_VdTimeOut(0, 1000);
#endif

	//G43TA无法使用mipi切镜头，强制走GPIO切
#if defined(__PRODUCT_G43TA__)
	bUseViFuncForSel0 = 0;
	for(int i = 0;i <= SECOND_STREAM;i++)
	{
		int Ret =Venc_Chn_Stop(0,i,0);
		if(Ret != 0)
			ZW_ERROR("[Zoom_SwicthFocusOpt] Venc_Chn_Stop(0,%d,0); ERR!!!\n",i);
	}
#endif

    switch (targetSensor) {
        case 0:
            if (bUseViFuncForSel0) {
                if (Vi_Chn_Set_Switch_Sensor_Attr(0, 0, 0) < 0) {
                    ZW_ERROR("Vi_Chn_Set_Switch_Sensor_Attr failed!\n");
                    goto err_exit;
                }
            }
			else {
				if (HalMod_Set_ZoomLens_Sel(0, 0) < 0) {
	                ZW_ERROR("HalMod_Set_ZoomLens_Sel failed!\n");
	                goto err_exit;
	            }
			}
            break;
        case 1:
        case 2:
			if (bUseViFuncForSel0){
				if (Vi_Chn_Set_Switch_Sensor_Attr(0, 0, 1) < 0) {
					ZW_ERROR("Vi_Chn_Set_Switch_Sensor_Attr failed!\n");
					goto err_exit;
				}
			}
			else {
				if (HalMod_Set_ZoomLens_Sel(0, 1) < 0){
					ZW_ERROR("HalMod_Set_ZoomLens_Sel failed!\n");
					goto err_exit;
				}
			}
            #if defined(__PRODUCT_G86T__)
            if (HalMod_Set_ZoomLens_Sel(1, targetSensor - 1) < 0) {
                ZW_ERROR("HalMod_Set_ZoomLens_Sel failed!\n");
                goto err_exit;
            }
            #endif
            break;
        default:
            ZW_ERROR("unsupported targetSensor, %d\n", targetSensor);
            goto err_exit;
    }

#if !defined(__PRODUCT_G43TA__)
    int currentSensor = pChCtx->m_CurrentSensor;
    double diffRatio = (currentSensor < targetSensor) ? RATIO_STEP : -RATIO_STEP;
    int maxDelayCnt = ((currentSensor == 1 && targetSensor == 2) ||
                       (currentSensor == 2 && targetSensor == 1)) ? 1 : 2;
    VPSS_CENTER_ZOOM_ATTR_S zoomAttr = {0};
    zoomAttr.Enable = 1;
    for (int i = 0; i < maxDelayCnt; i++) {
			Visp_Get_VdTimeOut(0, 1000);

        dummyRatio = (dummyRatio + diffRatio) < DRAG_ZOOM_MIN_RATIO ? DRAG_ZOOM_MIN_RATIO : (dummyRatio + diffRatio);

        double sqrtRatio = sqrt(dummyRatio);
        zoomAttr.WidthRatio = 1.0f / sqrtRatio;
        zoomAttr.HeightRatio = 1.0f / sqrtRatio;

        int ret = Vpss_Grp_Set_CenterZoom(0, 0, zoomAttr);
        if (ret != 0) {
            ZW_ERROR("Vpss_Grp_Set_CenterZoom failed! ret=%d Enable=%d WidthRatio=%.3f HeightRatio=%.3f\n",
                     ret, zoomAttr.Enable, zoomAttr.WidthRatio, zoomAttr.HeightRatio);
        }
    }
#endif

#if defined(__PRODUCT_G43TA__)
	for(int i = 0;i <= SECOND_STREAM;i++)
	{
		int Ret =Venc_Chn_Start(0,i,0,0);
		if(Ret != 0)
			ZW_ERROR("[Zoom_SwicthFocusOpt] Venc_Chn_Start(0,%d,0); ERR!!!\n",i);
		if(Ret != 0)
		printf("[Zoom_SwicthFocusOpt] Venc_Chn_Start(0,0,0); ERR!!!\n");
		else
		printf("[Zoom_SwicthFocusOpt] Venc_Chn_Start(0,0,0); SUEESE!!!\n");
	}
	ZW_INFO("Frame Time Out !!!\n");
	Visp_Get_VdTimeOut(0, 1000);
#endif

    pChCtx->m_CurrentSensor = targetSensor;
    ZW_INFO("Sensor switched to: %d\n", targetSensor);

#if 1
    SINT32 nStatus0 = 0;
    SINT32 nStatus1 = 0;
    if (bUseViFuncForSel0) {
        Vi_Chn_Get_Switch_Sensor_Attr(0, 0, &nStatus0);
    } else {
        HalMod_Get_ZoomLens_Sel(0, &nStatus0);
    }
#if (SUPPORT_LONG_SHORT_FOCUS_ZOOM_SENSOR_NUM == 3)
    HalMod_Get_ZoomLens_Sel(1, &nStatus1);
#endif
    ZW_INFO("Current ZoomLens MIPI Switch Status: %d, %d, %s (User Set: %d)\n", nStatus0, nStatus1,
        (!nStatus0) ? "Wide Lens" : ((!nStatus1) ? "Medium Lens" : "Telephoto Lens"), targetSensor);
#endif
    return 0;

err_exit:
    if (updRatio != NULL) {
        *updRatio = 0;
    }

    return -1;
}

int Zoom_GetCurrentSensor()
{
	CChannelContext *pChCtx = NULL;
	HY_Enc_GetEncChInfo(&pChCtx);
	return	pChCtx->m_CurrentSensor;
}

int Zoom_SetZoomLens(int nLens)
{
    CChannelContext *pChannelContext = NULL;
    HY_Enc_GetEncChInfo(&pChannelContext);
    if (pChannelContext == NULL)
    {
        ZW_ERROR("Param Err!\n");
        return -1;
    }

	pthread_mutex_lock(&pChannelContext->m_DragZoomLock);

    if (nLens < ForAndTwelve || nLens > ForAndSix)
    {
        ZW_ERROR("Zoom nLens:%d is error!\n", nLens);

		pthread_mutex_unlock(&pChannelContext->m_DragZoomLock);
        return -1;
    }

    pChannelContext->m_ZoomLens  = nLens;
	if(pChannelContext->m_ZoomLens == ForAndSix)
	{
		pChannelContext->m_WideMaxRatio = 6.0;
		pChannelContext->m_MiddleMaxRatio = 6.0;
		pChannelContext->m_TeleMaxRatio = 12.0;
		pChannelContext->m_FocusCurRatio = 1.0;
	}
	else
	{
		pChannelContext->m_WideMaxRatio = 6.0;
		pChannelContext->m_MiddleMaxRatio = 12.0;
		pChannelContext->m_TeleMaxRatio = 18.0;
		pChannelContext->m_FocusCurRatio = 1.0;
	}
    pthread_mutex_unlock(&pChannelContext->m_DragZoomLock);

    ZW_DEBUG("Drag m_ZoomnLens(%d) \n", nLens);
    return 0;
}


#endif

double Zoom_GetCurRatio()
{
	CChannelContext *pChCtx = NULL;
	HY_Enc_GetEncChInfo(&pChCtx);
	return  sqrt(pChCtx->m_FocusCurRatio);
}

void Zoom_RatioConverseAdapt(double newRatio, double *relRatio)
{
    CChannelContext *pChCtx = NULL;
	HY_Enc_GetEncChInfo(&pChCtx);

#if defined(__IS_SUPPORT_LONG_SHORT_FOCUS_ZOOM__)
	double Zoom_Z_to_S_ratio = 0;
	double Zoom_S_to_M_ratio = 0;
	double Zoom_M_to_L_ratio = 0;

    const double WIDE_MIDDLE_SWITCH_RATIO = pChCtx->m_WideMaxRatio;
    const double MIDDLE_TELE_SWITCH_RATIO = pChCtx->m_WideMaxRatio + pChCtx->m_MiddleMaxRatio;

	if(pChCtx->m_ZoomLens == ForAndTwelve)
	{
		Zoom_Z_to_S_ratio = DRAG_ZOOM_Z_TO_S_RATIO;
		Zoom_S_to_M_ratio = DRAG_ZOOM_S_TO_M_RATIO;
		Zoom_M_to_L_ratio = DRAG_ZOOM_M_TO_L_RATIO;
	}
	else if(pChCtx->m_ZoomLens == ForAndSix)
	{
		Zoom_Z_to_S_ratio = DRAG_ZOOM_Z_TO_S_RATIO_4A6;
		Zoom_S_to_M_ratio = DRAG_ZOOM_S_TO_M_RATIO_4A6;
		Zoom_M_to_L_ratio = DRAG_ZOOM_M_TO_L_RATIO_4A6;
	}
    double sensorMaxRatio = (pChCtx->m_CurrentSensor == ZOOM_WIDE_FOCUS) ?
                           pChCtx->m_WideMaxRatio : (pChCtx->m_CurrentSensor == ZOOM_MIDDLE_FOCUS) ?
                           pChCtx->m_MiddleMaxRatio : pChCtx->m_TeleMaxRatio;
    if (pChCtx->m_CurrentSensor == ZOOM_WIDE_FOCUS)
    {
        *relRatio = (newRatio - DRAG_ZOOM_MIN_RATIO) * (Zoom_Z_to_S_ratio - DRAG_ZOOM_MIN_RATIO) /
                  (sensorMaxRatio - DRAG_ZOOM_MIN_RATIO) + DRAG_ZOOM_MIN_RATIO;
    }
    else if (pChCtx->m_CurrentSensor == ZOOM_MIDDLE_FOCUS)
    {
        *relRatio = (newRatio - WIDE_MIDDLE_SWITCH_RATIO) * (Zoom_S_to_M_ratio - DRAG_ZOOM_MIN_RATIO) /
                  (sensorMaxRatio - DRAG_ZOOM_MIN_RATIO) + DRAG_ZOOM_MIN_RATIO;
    }
    else
    {
        *relRatio = (newRatio - MIDDLE_TELE_SWITCH_RATIO) * (Zoom_M_to_L_ratio - DRAG_ZOOM_MIN_RATIO) /
                  (sensorMaxRatio - DRAG_ZOOM_MIN_RATIO) + DRAG_ZOOM_MIN_RATIO;
    }
    pChCtx->m_FocusCurRatio = *relRatio;
#else
	//计算设置到基线的倍数
	*relRatio =  (newRatio - DRAG_ZOOM_MIN_RATIO) * (DRAG_ZOOM_MAX_RATIO-DRAG_ZOOM_MIN_RATIO) / (pChCtx->m_ZoomUserMaxRatio-DRAG_ZOOM_MIN_RATIO) + DRAG_ZOOM_MIN_RATIO;
	pChCtx->m_FocusCurRatio = *relRatio;
#endif

    return;
}
#endif

