// Copyright (C) 2018 - 2025 Rational Software Corporation
#include <stdio.h>

#include <string.h>
#include <time.h>
#include <sys/time.h>

#include <math.h>
#include "global_macro.h"

#include "AIFrameType.h"
#include "CAIStream.h"
#include "PTZ/CPtzManager.h"
#include "zwsdk.h"
#include "MPU_API.h"
#include "common_pub.h"
#include "AVL_global.h"
#if defined(__DIGITAL_ZOOM_DRAG_SUPPORT__)
#include "CZoomManager.h"
#endif
#include "mysystem.h"
#define  USE_SIMPLI_HUMAN_TRACK 

#define TRACK_STATUS_CNT (3)

static time_t time_begin = 0;
static int s_TrackMoveFlag = 0;//云台跟踪标志
static int TrackStayTimeS = 0;

#ifndef __IS_SUPPORT_MAS_CALIBRATION__
static int s_nAutoCalibrationTime = 0;
#endif
#if defined(__DIGITAL_ZOOM_DRAG_SUPPORT__)
static int s_WaitRecoveryFlay = 0; 		//恢复倍数
#endif

//##ModelId=49B0E4200207
CAIStream::CAIStream(int nCh, void *hHandle)
{
	m_iChannel = nCh;
	m_Track_Status = 0;
	m_nHorizonMaxStep = 0;
	m_nVerticalMaxStep = 0;
	m_horizontalTotalFieldAngle = 0;
	m_VerticalTotalFieldAngle = 0;
	m_HorizonviewingAngle = 0.0;
	m_VorizonviewingAngle = 0.0;
	
	m_linkAction = 1;//默认允许围栏/越界响应开启的所有联动项。在跟踪时只能响应录像
	m_linkStart = 0;

	m_henc = hHandle;
	pTrackHandle = NULL;

    /*初始化变量*/
    memset(static_cast<void*>(&param), 0, sizeof(XRS_PTZ_TRACK_INPUT_PARAM_ST));
}

CAIStream::~CAIStream()
{
	UnInit();
}

int CAIStream::Init(int nMaxCh, int nMaxAlarmOut)
{
	char szSectionName[20] = {0};
	char szTrackSectionName[20] = {0};
	char szSensorWs[20] = {0};
	char szSensorHs[20] = {0};
	char szcurrentFocus[20] = {0};
	char szpanThr[20] = {0};
	char sztiltThr[20] = {0};
	int s32Ret = 0;
    int nBoardID = 0;
	pCConfig = CConfig::GetInstance();
	if (NULL == pCConfig)
	{
		UnInit();
		return -1;
	}
	
    snprintf(szSectionName, sizeof(szSectionName), "PTZ_Set_%d", 0);
    m_nHorizonMaxStep   = pCConfig->GetValue(szSectionName, "HorizontalStep", 396);
    m_nVerticalMaxStep = pCConfig->GetValue(szSectionName, "VerticalStep", 128);

    /* 人形跟踪使能 */
    m_trackSwitch =  pCConfig->GetValue("HumanShapeCfg", "TrackSwitch00", 0);
    m_trackSwitchBak = m_trackSwitch;
    ZWSys_EncSetHumanTrackEn(m_henc, m_trackSwitch);
	
#if defined(__DIGITAL_ZOOM_DRAG_SUPPORT__)
		/* 跟踪变倍使能 */
		m_TrackZoomEn = pCConfig->GetValue("HumanShapeCfg", "TrackZoomEnable", 0);
#else
		m_TrackZoomEn = 0;
#endif
	
	snprintf(szTrackSectionName, sizeof(szTrackSectionName), "PTZ_Track_%d", 0);
	m_horizontalTotalFieldAngle = pCConfig->GetValue(szTrackSectionName, "HTotalFieldAngle", 270);//球机水平可转动的角度，(从定制文件获取)
	m_VerticalTotalFieldAngle = pCConfig->GetValue(szTrackSectionName, "VTotalFieldAngle", 100);//球机垂直可转动的角度，(从定制文件获取)
	TRACE_AVL("szTrackSectionName = %s\n", szTrackSectionName);
	TRACE_AVL("%s %d m_horizontalTotalFieldAngle = %d m_VerticalTotalFieldAngle = %d\n", __FUNCTION__, __LINE__, m_horizontalTotalFieldAngle, m_VerticalTotalFieldAngle);
#if defined (__PRODUCT_G43TA__)
	int sZoomLen = pCConfig->GetValue("ZoomLensCfg", "ZoomLensCombination", 2);
	if(sZoomLen == 0)
	{
		//G43TA 球机跟踪放大参数区分摇头机
		sprintf(szSensorWs,"%s","5.50");
		sprintf(szSensorHs,"%s","3.00");
	}
	else
	{
		pCConfig->GetValue(szTrackSectionName, "SensorWs", "5.12", szSensorWs, sizeof(szSensorWs));
		pCConfig->GetValue(szTrackSectionName, "SensorHs", "2.88", szSensorHs, sizeof(szSensorHs));
	}
#else
	pCConfig->GetValue(szTrackSectionName, "SensorWs", "5.12", szSensorWs, sizeof(szSensorWs));
	pCConfig->GetValue(szTrackSectionName, "SensorHs", "2.88", szSensorHs, sizeof(szSensorHs));
#endif
	pCConfig->GetValue(szTrackSectionName, "currentFocus", "3.6", szcurrentFocus, sizeof(szcurrentFocus));
	pCConfig->GetValue(szTrackSectionName, "panThr", "1500", szpanThr, sizeof(szpanThr));
	pCConfig->GetValue(szTrackSectionName, "tiltThr", "1500", sztiltThr, sizeof(sztiltThr));

	TRACE_AVL("szSensorWs = %s, szSensorHs = %s szcurrentFocus = %s\n", szSensorWs, szSensorHs, szcurrentFocus);
	TRACE_AVL("szpanThr = %s, sztiltThr = %s\n", szpanThr, sztiltThr);
	param.imgWide = IMAGE_TOTAL_WIDE;//图像的宽度
	param.imgHeight = IMAGE_TOTAL_HIGHT;//图像的高度
	s32Ret |= str2double(&param.panThr, szpanThr);//目标水平允许偏差，(从定制文件获取)
	s32Ret |= str2double(&param.tiltThr, sztiltThr);//目标垂直允许偏差，(从定制文件获取)
	m_panThr = 400;
	m_tiltThr = 1200;
	
	ZWSys_GetBoardInfo(&nBoardID);
    
	param.max_skip_tracking_frame = 10;//算法认为多少帧没有目标了就是丢失
	
	//当前视野距离边界角度这4参数算法弃用了
	param.ptz_scale_left = 0;
	param.ptz_scale__right = 0;
	param.ptz_scale_up = 0;
	param.ptz_scale_down = 0;
	
	s32Ret = str2double(&param.Ws, szSensorWs);//sensor霸面的宽(单位毫米)，(从定制文件获取) 例：2560*2/1000.0
	s32Ret |= str2double(&param.Hs, szSensorHs);//sensor霸面的高(单位毫米)，(从定制文件获取)例：1440*2/1000.0
	s32Ret |= str2double(&param.currentFocus, szcurrentFocus);//镜头焦距，(从定制文件获取)  例：3.6

	TRACE_AVL("panThr = %lf tiltThr = %lf\n", param.panThr, param.tiltThr);
	TRACE_AVL("Ws = %lf Hs = %lf currentFocus = %lf\n", param.Ws, param.Hs, param.currentFocus);

	//以下两个参数会照成跟踪目标群中间变化较大时相应慢的问题，但是可以解决因人形短暂漏检照成的误调整
	param.distanceThr = pCConfig->GetValue(szTrackSectionName, "distanceThr", 3300);//当前中心点跟上次偏离多少时启动该逻辑，(从定制文件获取)
	param.gravityChangeThr = pCConfig->GetValue(szTrackSectionName, "gravityChangeThr", 2);//达到几次都是上面中心偏离值时再启动调整。如果不想启动该逻辑，将该值给0，(从定制文件获取)

	TRACE_AVL("distanceThr = %d gravityChangeThr = %d s32Ret = %d\n", param.distanceThr, param.gravityChangeThr, s32Ret);

	//算法调试开关
	param.debug = 1;

#ifndef USE_SIMPLI_HUMAN_TRACK	
	s32Ret = XRS_PTZ_Tracking_Init(&pTrackHandle);
	if(s32Ret)
	{
		AVL_ERR("XRS_PTZ_Tracking_Init fail!!! ret =%d\n", s32Ret);
	}
	else
	{
		XRS_PTZ_Tracking_SetParam(pTrackHandle, param);
	}
#else
	printf("Use USE_SIMPLI_HUMAN_TRACK!\n");	
#endif
	//计算出水平垂直视场角
	m_HorizonviewingAngle = 2 * atan((param.Ws / 2) /param.currentFocus) * 180 / MATH_PI;
	m_VorizonviewingAngle = 2 * atan((param.Hs / 2) /param.currentFocus) * 180 / MATH_PI;

	m_nHorizonMaxStepInPic = m_HorizonviewingAngle * m_nHorizonMaxStep / m_horizontalTotalFieldAngle;
	m_nVerticalMaxStepInPic = m_VorizonviewingAngle * m_nVerticalMaxStep / m_VerticalTotalFieldAngle;
	
#if defined __PRODUCT_G85T__
	m_nHorizonMaxStepInPic = 306;
	m_nHorizonMaxStepInPic_Long = 107;
	m_nVerticalMaxStepInPic_Long = m_nVerticalMaxStepInPic/3;
#endif

#if defined __PRODUCT_W63F__
		m_nHorizonMaxStepInPic = 325;
#endif


	TRACE_AVL("m_HorizonviewingAngle=%f, m_VorizonviewingAngle=%f\n", m_HorizonviewingAngle,m_VorizonviewingAngle);
	TRACE_AVL("m_nHorizonMaxStepInPic=%d, m_nVerticalMaxStepInPic=%d\n", m_nHorizonMaxStepInPic, m_nVerticalMaxStepInPic);	
	return 0;
}

int CAIStream::UnInit()
{
	return 0;
}

int CAIStream::SetTrackStayTime(int nStayTimeSec)
{
	CPtzManager *pPtz = NULL;
	pPtz = CPtzManager::PTZ_GetInstance();

	if (pPtz != NULL)
	{
		pPtz->SetTrackStayTime(nStayTimeSec);
	}

	return 0;
}

int CAIStream::GetTrackStayTime(int *nStayTimeSec)
{
	CPtzManager *pPtz = NULL;
	pPtz = CPtzManager::PTZ_GetInstance();

	if (pPtz != NULL)
	{
		pPtz->GetTrackStayTime(nStayTimeSec);
	}

	return 0;
}

int CAIStream::GetTrackStatCfg(int *trackEnable, int *linkAction)
{
	int nSmartTrackEnable = 0;

	if (trackEnable != NULL && linkAction != NULL)
	{
		*trackEnable = nSmartTrackEnable;
		*linkAction = m_linkAction;
	}

	return 0;
}

int CAIStream::SetTrackLinkStart(int linkStart)
{
	m_linkStart = linkStart;

	return 0;
}

//!\brief 获取人形跟踪使能
//!\return 1表示开启 0表示关闭 -1表示失败
int CAIStream::GetTrackSwitchParam()
{
	return m_trackSwitch;
}

//!\brief 设置人形跟踪使能
//!\return 0表示成功 -1表示失败
int CAIStream::SetTrackSwitchParam(int iEnable)
{
    if (iEnable != m_trackSwitch)
    {
        char key[100] = {0};
        m_trackSwitch = iEnable;
        m_trackSwitchBak = m_trackSwitch;
        snprintf(key, sizeof(key), "TrackSwitch%02d", m_iChannel);
        pCConfig->SetValue("HumanShapeCfg", key, m_trackSwitch);

        if (iEnable == 0)
        {
            /* 恢复云台速度 */
            int nSpeed = MPU_PTZ_GetSpeed(m_iChannel);
            MPU_PTZ_SetSpeed(nSpeed, m_iChannel);
#if defined(__DIGITAL_ZOOM_DRAG_SUPPORT__)
            RecoverSaveTrackZoomRatio();
#endif
        }
        ZWSys_EncSetHumanTrackEn(m_henc, m_trackSwitch);

        TRACE_AVL("---CHumanShape Set TrackSwitchParam[%d]---\n",m_trackSwitch);
    }

    return 0;
}

#if defined(__DIGITAL_ZOOM_DRAG_SUPPORT__)
int CAIStream::SetTrackZoomEnable(int nEnable)
{
	if ((nEnable != 0) && (nEnable != 1))
	{
		return -1;
	}

	if (nEnable == m_TrackZoomEn)
	{
		return 0;
	}
	m_TrackZoomEn = nEnable;
	m_panThr = 400;
	m_tiltThr = 1200;
    if (nEnable == 0)
    {
        RecoverSaveTrackZoomRatio();
    }

	pCConfig->SetValue("HumanShapeCfg", "TrackZoomEnable", m_TrackZoomEn);
	return 0;
}

int CAIStream::GetTrackZoomEnable(int *nEnable)
{
	if (NULL == nEnable)
	{
		return -1;
	}

	*nEnable = m_TrackZoomEn;
	return 0;
}

//恢复人形跟踪变倍前的倍数,1x
int CAIStream::RecoverSaveTrackZoomRatio()
{
    int ret = 0;

    double iCurRatio = 1.0f;
    CZoomManager *pZoom = NULL;

    pZoom = CZoomManager::GetInstance();
    ret = pZoom->SetRatio(iCurRatio,DRAG_TRACK_RADIO);

    return ret;
}

#endif

int CAIStream::DealAIStreamData(char *sBuff, int nLen)
{
	int nRectCenterX = 0;
	int nRectCenterY = 0;
	int nHMotoStep = 0;
	int nVMotoStep = 0;
	int nTopAdjust = 0;
	int nPTZCtrlStatue = 0;

	CPtzManager *pPtz = NULL;
		
	struct timeval NowTime;
    int s32Ret = 0;
	int IfInit = 0;
	static int speedH=1;
	static int speedV=1;
	int maxSpeedH=9, minSpeedH=0;//水平跟踪允许的云台速度
	int maxSpeedV=5, minSpeedV=0;//垂直跟踪允许的云台速度
    int iCuriseState = 0;
	int nHorizonMaxStepInPic = 0;
	int nVerticalMaxStepInPic = 0;

#if defined(__DIGITAL_ZOOM_DRAG_SUPPORT__)    
    RECT_ST humanRect[16] = {0};
    double fRatio = 1.0f;
	CZoomManager *cZoom = CZoomManager::GetInstance();
#endif
#if defined(__IS_SUPPORT_LONG_SHORT_FOCUS_ZOOM__)
	int CurrentSensor = 0;//当前sensor倍数
	CurrentSensor = ZWSys_GetCurrentSensor();
	if(CurrentSensor == ZOOM_WIDE_FOCUS)
	{
		nHorizonMaxStepInPic = m_nHorizonMaxStepInPic;
		nVerticalMaxStepInPic = m_nVerticalMaxStepInPic;
	}
	else if(CurrentSensor == ZOOM_MIDDLE_FOCUS)
	{
		nHorizonMaxStepInPic = m_nHorizonMaxStepInPic_Long;
		nVerticalMaxStepInPic = m_nVerticalMaxStepInPic_Long;
	}
#else
	nHorizonMaxStepInPic = m_nHorizonMaxStepInPic;
	nVerticalMaxStepInPic = m_nVerticalMaxStepInPic;
#endif
	

	pPtz = CPtzManager::PTZ_GetInstance();
	if (NULL == sBuff || nLen != sizeof(OverlayRectangle))
	{
		return -1;
	}

    if ((NULL == pPtz) || (0 == pPtz->IsSupportBuiltInMotor()) || (!m_trackSwitch))
    {
        return 0;
    }

    /* 二维码扫描模式下，一段时间内不响应人形跟踪，以优化扫码体验 */
    if (0 != GetQRScanStatus())
    {
        return 0;
    }

#if defined (__IS_SUPPORT_LCD_MANAGER__)
    int CallState = 0;
    GetExternPreviewCallState(&CallState);
    if (CallState != 0) //通话状态下，不进行跟踪操作
    {
        return 0;
    }
#endif

    /* 必须在自动巡航结束后才可跟踪 */
    pPtz->PTZ_GetCuiseStatue(&iCuriseState);
    if (iCuriseState != 0)
    {
        return 0;
    }
	
	POverlayRectangle pOverlayRectangle = (POverlayRectangle)sBuff;

#if defined(__IS_SUPPORT_TRACK_HUMAN_FACE__)
	if ((pOverlayRectangle->m_Bottom - pOverlayRectangle->m_Top)>(pOverlayRectangle->m_Right - pOverlayRectangle->m_Left))
	{	
        #if defined(__DIGITAL_ZOOM_DRAG_SUPPORT__)
    	humanRect[0].x = pOverlayRectangle->m_Left;
    	humanRect[0].y = pOverlayRectangle->m_Top;
    	humanRect[0].wide = pOverlayRectangle->m_Right - pOverlayRectangle->m_Left;
        humanRect[0].height = pOverlayRectangle->m_Right - pOverlayRectangle->m_Left;
        #endif
    	nRectCenterX = (pOverlayRectangle->m_Right + pOverlayRectangle->m_Left) / 2;
        nRectCenterY = (pOverlayRectangle->m_Top*2 + humanRect[0].height) / 2;
	}
    else
    {
        #if defined(__DIGITAL_ZOOM_DRAG_SUPPORT__)
    	humanRect[0].x = pOverlayRectangle->m_Left;
    	humanRect[0].y = pOverlayRectangle->m_Top;
    	humanRect[0].wide = pOverlayRectangle->m_Right - pOverlayRectangle->m_Left;
        humanRect[0].height = pOverlayRectangle->m_Bottom - pOverlayRectangle->m_Top;
        #endif
    	nRectCenterX = (pOverlayRectangle->m_Right + pOverlayRectangle->m_Left) / 2;
		nRectCenterY = (pOverlayRectangle->m_Top + pOverlayRectangle->m_Bottom) / 2;
    }
#else
    #if defined(__DIGITAL_ZOOM_DRAG_SUPPORT__)
	humanRect[0].x = pOverlayRectangle->m_Left;
	humanRect[0].y = pOverlayRectangle->m_Top;
	humanRect[0].wide = pOverlayRectangle->m_Right - pOverlayRectangle->m_Left;
	humanRect[0].height = pOverlayRectangle->m_Bottom - pOverlayRectangle->m_Top;
    #endif
	nRectCenterX = (pOverlayRectangle->m_Right + pOverlayRectangle->m_Left) / 2;
	nRectCenterY = (pOverlayRectangle->m_Top + pOverlayRectangle->m_Bottom) / 2;
#endif

	// 人形位置的上边界离最大大边界小于20%，向上移动10%
	if (pOverlayRectangle->m_Top < 2000)
	{
		nTopAdjust = -1000;
	}

	// 有人形结果时，将回看守位的时间清零
	gettimeofday(&NowTime, NULL);
	time_begin = NowTime.tv_sec;
	s_TrackMoveFlag = 1;

	if (m_linkStart)
	{
		m_linkAction = 0; // 在跟踪时只能响应录像
	}

    // 电机还未开始转动
    if (nRectCenterX && nRectCenterY)
    {
        pPtz->PTZ_GetPTZCtrlStatue(&nPTZCtrlStatue);
        if (0 != nPTZCtrlStatue)
        {
            return 0;
        }

        pPtz->PTZ_GetIfInit(&IfInit); // 需要等待云台初始化校准完成
        if (IfInit)
		{
			//在允许的偏差范围内，则不启动跟踪
			if(abs(nRectCenterX - 5000) > m_panThr)
			{
				nHMotoStep = ((nRectCenterX - 5000)*(nHorizonMaxStepInPic)/10000);
			}
			
			if(abs(nRectCenterY + nTopAdjust - 5000) > m_tiltThr)
			{
				nVMotoStep = ((nRectCenterY + nTopAdjust - 5000)*nVerticalMaxStepInPic/10000);
			}

		#if defined(__DIGITAL_ZOOM_DRAG_SUPPORT__)
			fRatio = ZWSys_GetCurRatio();
		#if defined (__IS_SUPPORT_LONG_SHORT_FOCUS_ZOOM__)
			if(CurrentSensor == ZOOM_MIDDLE_FOCUS)
			{
				fRatio = (DRAG_ZOOM_MIN_MULTIPLE - (fRatio - DRAG_ZOOM_MIN_MULTIPLE)/80)*fRatio;//长焦偏差系数换算公式
			}
		#endif
            if (nHMotoStep)
			{
				float fHMotoStep = nHMotoStep*1.0f/fRatio;
				// 限制绝对值，如果小于1则设为1（保持符号）
				if (ABS(fHMotoStep) < 1.0f && fHMotoStep != 0.0f)
				{
					fHMotoStep = (fHMotoStep > 0) ? 1.0f : -1.0f;
				}
				nHMotoStep = (int)fHMotoStep;
			}
			if (nVMotoStep)
			{
				float fVMotoStep = nVMotoStep*1.0f/fRatio;
				// 限制绝对值，如果小于1则设为1（保持符号）
				if (ABS(fVMotoStep) < 1.0f && fVMotoStep != 0.0f)
				{
					fVMotoStep = (fVMotoStep > 0) ? 1.0f : -1.0f;
				}
				nVMotoStep = (int)fVMotoStep;
			}
			/*根据运行步长占总步长的比例换算速度*/
		#endif
		
		speedH = abs(nHMotoStep)*2* (maxSpeedH - minSpeedH) /(m_nHorizonMaxStepInPic)  + minSpeedH;
		speedV = (abs(nVMotoStep)*2 * (maxSpeedV - minSpeedV)/ m_nVerticalMaxStepInPic)	 + minSpeedV;
		

			if(nHMotoStep || nVMotoStep)
			{
				//为了不影响手动控制云台的速度，那么就需要手动控制云台前需要把速度换过来。
				pPtz->PTZ_SetTrackSpeed(speedH, speedV, m_iChannel);
				//printf("nHMotoStep=%d, nVMotoStep=%d, speedH=%d, speedV=%d\n", nHMotoStep, nVMotoStep, speedH, speedV);
				s32Ret = pPtz->PTZ_NormalCtrlToPosExt(m_iChannel, nHMotoStep, nVMotoStep);
				//判断电机是否真的转动
				if(s32Ret != 1)
				{
					if (!m_Track_Status)
					{
						ZWSys_UpdatePtzStatus(m_henc,ePtzStatus_Moving);
					}
					m_Track_Status = TRACK_STATUS_CNT;
					//人形跟踪进行中，电机正在移动......
				}

			#if defined(__DIGITAL_ZOOM_DRAG_SUPPORT__)
				s_WaitRecoveryFlay = 0;
			#endif
			}

		}

#if defined(__DIGITAL_ZOOM_DRAG_SUPPORT__)
        //跟踪放大开关控制跟踪放大逻辑
        if (1 == m_TrackZoomEn)
        {
            cZoom->TrackMultipleCalc(humanRect[0].x,humanRect[0].y,humanRect[0].wide,humanRect[0].height);
        }
#endif	
    }    
	return 0;
}


int CAIStream::CheckAIStream()
{	
    int iCuriseState = 0;
	int nPTZCtrlStatue = 0;
	int nSelfTestStatus = 0;
	int nTrackStayTimeS = 0;
	static int bGetMotoStat = 0;
	int motoStatus = 0;
	static int motoStatusCount = 0;
	
	struct timeval NowTime;
	gettimeofday(&NowTime, NULL);
	CPtzManager *pPtz = NULL;
	pPtz = CPtzManager::PTZ_GetInstance();

	if (pPtz == NULL || (0 == pPtz->IsSupportBuiltInMotor()))
	{
		return 0;
	}

	//手动控制停止十秒后才能开始跟踪
    pPtz->PTZ_GetPTZCtrlStatue(&nPTZCtrlStatue);
	if (nPTZCtrlStatue > 0)
	{
		nPTZCtrlStatue--;
		pPtz->PTZ_SetPTZCtrlStatue(nPTZCtrlStatue);
		return 0;
	}

#if defined (__IS_SUPPORT_LCD_MANAGER__)
    int CallState = 0;
    GetExternPreviewCallState(&CallState);
    if (CallState != 0) //通话状态下，不进行回预置位操作
    {
        if (m_Track_Status > 0)
        {
            m_Track_Status = 0;
            ZWSys_UpdatePtzStatus(m_henc, ePtzStatus_Stop);
        }
        return 0;
    }
#endif

#if defined(__DIGITAL_ZOOM_DRAG_SUPPORT__)
        int moto_state = 0;
        //回到看守位后，恢复倍数
        if (1 == s_WaitRecoveryFlay)
        {
            pPtz->PTZ_GetMotorState(&moto_state);
            if (0 == moto_state)
            {
                s_WaitRecoveryFlay = 0;
                RecoverSaveTrackZoomRatio();
            }
        }
#endif

	pPtz->GetTrackStayTime(&nTrackStayTimeS);
    pPtz->PTZ_GetCuiseStatue(&iCuriseState);

	if(nTrackStayTimeS != TrackStayTimeS)
	{
		TrackStayTimeS = nTrackStayTimeS;
		time_begin = NowTime.tv_sec;
	}
	
	//人形检测停留的时间到达，且开启跟踪，回到预置位
	if (m_trackSwitch)
	{
		if (s_TrackMoveFlag)
		{
			if (ABS(NowTime.tv_sec - time_begin) > 30)
	        {
	    		time_begin = NowTime.tv_sec;
	            if (iCuriseState == 0)
	            {
        			//将云台正在回预置位的状态给到sdk
        			ZWSys_PtzForTrackBacktoPreset(m_henc, ePtz2Preset_backing);
        			pPtz->PTZ_NormalCtrlToPreset(m_iChannel);
        			s_TrackMoveFlag = 0;
        			bGetMotoStat = 1;
                #if defined(__DIGITAL_ZOOM_DRAG_SUPPORT__)
    				//回到看守位后，恢复倍数
                    if (1 == m_TrackZoomEn)
                    {
    					s_WaitRecoveryFlay = 1;
                    }
    			#endif
	            }
	        }
		}
		else
		{
			if (ABS(NowTime.tv_sec - time_begin) > nTrackStayTimeS)
			{
				time_begin = NowTime.tv_sec;
				if (iCuriseState == 0)
				{
					//将云台正在回预置位的状态给到sdk
					ZWSys_PtzForTrackBacktoPreset(m_henc, ePtz2Preset_backing);
					pPtz->PTZ_NormalCtrlToPreset(m_iChannel);
					s_TrackMoveFlag = 0;
					bGetMotoStat = 1;
                #if defined(__DIGITAL_ZOOM_DRAG_SUPPORT__)
					//回到看守位后，恢复倍数
					if (1 == m_TrackZoomEn)
					{
						s_WaitRecoveryFlay = 1;
					}
                #endif
				}
			}
		}
	}
    else
    {
        //停止巡航时,执行手动控制云台回到看守位逻辑
        if (iCuriseState == 0)
        {
        	if (ABS(NowTime.tv_sec - pPtz->PTZ_GetManualMoveTimeBegin()) > nTrackStayTimeS)
        	{
        		pPtz->PTZ_SetManualMoveTimeBegin(NowTime.tv_sec);
        		if (pPtz->PTZ_GetManualMovePtzFlag())
        		{
        			pPtz->PTZ_NormalCtrlToPreset(m_iChannel);
        			pPtz->PTZ_ClearManualMovePtzFlag();
        		}
        	}
        }
    }

    // 获取云台当前状态，0静止 1运动
    pPtz->PTZ_IsMotoStatus(&motoStatus);

	//使用当前电机的状态来判断人形跟踪是否结束了
	if (m_Track_Status > 0)
	{
	    m_Track_Status--;
        if ((0 == m_Track_Status) && 0 == motoStatus)
        {
    		ZWSys_UpdatePtzStatus(m_henc, ePtzStatus_Stop);
        }
	}

	if ((1 == bGetMotoStat) && (0 == motoStatus))
	{
		motoStatusCount ++;
		//跟踪回到预置位后，延时1-2S将云台动作完成的状态给到sdk。供sdk做区域功能的处理
		if (motoStatusCount >= 2)
		{
			ZWSys_PtzForTrackBacktoPreset(m_henc, ePtz2Preset_fulfil);
			motoStatusCount = 0;
			bGetMotoStat = 0;

			m_linkAction = 1;//跟踪完成、回到预置位后，允许围栏/越界响应开启的所有联动项。
			m_linkStart = 0;
		}
	}

	/*清除校准状态*/
	if (pPtz->PTZ_IsCalibrationStart())
	{
		pPtz->PTZ_GetSelfCheckState(&nSelfTestStatus);
		if (nSelfTestStatus == 2)
		{
			pPtz->PTZ_SetCalibrationReset();
			ZWSys_SetCalibrationStatue(0);	
		}
	}
#ifndef __IS_SUPPORT_MAS_CALIBRATION__
	if (pPtz->PTZ_IsAdaptProtocol(0))
	{
		//自动校准,在人形跟踪开启的情况下才启动6小时自检
		if (m_trackSwitch)
		{
			s_nAutoCalibrationTime++;
		}
		else
		{
			s_nAutoCalibrationTime = 0;
		}

		if (s_nAutoCalibrationTime > (6 * 60 * 60))
		{
			s_nAutoCalibrationTime = 0;
			pPtz->PTZ_MotoPosCalibration(0);
		}
	}
#endif

	return 0;
}
