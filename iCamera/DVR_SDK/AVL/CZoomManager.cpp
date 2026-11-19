//#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/types.h>
#include "zwsdk.h"
#include "global_macro.h"
#include "CConfig.h"
#include "CZoomManager.h"
#include "common_pub.h"
#include "PTZ/CPtzManager.h"




#if defined(__DIGITAL_ZOOM_DRAG_SUPPORT__)

#if defined(__PRODUCT_G43TA__)
//检测次数
#define DRAG_ZOOM_CHECK_COUNT						(1)

//放大缩小步长
#define DRAG_ZOOM_TRACK_STEP						(0.4)

//宽度比 < 0.25时需要放大
#define DRAG_ZOOM_TRACKER_WIDTH_RATIO_ENLARGE        (0.23f)
//高度比 < 0.3时需要放大
#define DRAG_ZOOM_TRACKER_HEIGHT_RATIO_ENLARGE       (0.28f)
//高度比 < 0.33时需要放大
#define DRAG_ZOOM_TRACKER_HEIGHT_RATIO_ENLARGE_MAX   (0.32f)

//宽度比 > 0.38时需要缩小
#define DRAG_ZOOM_TRACKER_WIDTH_RATIO_LESSEN         (0.30f)
//高度比 > 0.66时需要缩小
#define DRAG_ZOOM_TRACKER_HEIGHT_RATIO_LESSEN        (0.40f)
//高度比 > 0.75 时需要缩小
#define DRAG_ZOOM_TRACKER_HEIGHT_RATIO_LESSEN_MAX    (0.48f)

#else
//检测次数
#define DRAG_ZOOM_CHECK_COUNT						(1)

//放大缩小步长
#define DRAG_ZOOM_TRACK_STEP						(0.4)

//宽度比 < 0.25时需要放大
#define DRAG_ZOOM_TRACKER_WIDTH_RATIO_ENLARGE        (0.25f)
//高度比 < 0.3时需要放大
#define DRAG_ZOOM_TRACKER_HEIGHT_RATIO_ENLARGE       (0.3f)
//高度比 < 0.33时需要放大
#define DRAG_ZOOM_TRACKER_HEIGHT_RATIO_ENLARGE_MAX   (0.33f)

//宽度比 > 0.38时需要缩小
#define DRAG_ZOOM_TRACKER_WIDTH_RATIO_LESSEN         (0.30f)
//高度比 > 0.66时需要缩小
#define DRAG_ZOOM_TRACKER_HEIGHT_RATIO_LESSEN        (0.40f)
//高度比 > 0.75 时需要缩小
#define DRAG_ZOOM_TRACKER_HEIGHT_RATIO_LESSEN_MAX    (0.45f)
#endif
//宽或者高变化超过step ,才做变倍处理
#define DRAG_CHANGE_STEP							(50)		

#define HUMAN_DETECT_LIMIT_WIDTH                    (10000)
#define HUMAN_DETECT_LIMIT_HEIGHT                   (10000)


static const char *ZoomSection = "ZoomCfg";
static const char *ZoomPresetKey = "ZoomCurRatioPreset";

static const char *ZoomLensSection = "ZoomLensCfg";
//变倍镜头组合：0-4+12mm组合，1-4+8mm组合 2-4+6mm组合
static const char *ZoomLensComKey = "ZoomLensCombination";

CZoomManager *CZoomManager::m_pInstance = NULL;

static double g_zoom_max_multiple = DRAG_ZOOM_MAX_MULTIPLE;
static double g_zoom_min_multiple = DRAG_ZOOM_MIN_MULTIPLE;

CZoomManager::CZoomManager()
{
    int index = 0;

    m_pInstance = NULL;
    pCConfig = NULL;

    for(index = 0; index < MAX_ZOOM_PRESET_COUNT; index++)
    {
        m_iZoomPresetRatio[index] = 0;
    }

    m_iZoomScaleMagnification = 0;
	m_iZoomRatio = 1.0f;
}

CZoomManager::~CZoomManager()
{
    UnInit();
}

CZoomManager *CZoomManager::GetInstance()
{
    if (NULL == m_pInstance)
    {
        m_pInstance = new CZoomManager;

        if (NULL == m_pInstance)
        {
            printf("CZoomManager::GetInstance failed\n");
            return NULL;
        }
    }
    return (CZoomManager *) m_pInstance;
}

int CZoomManager::ReleaseInstance()
{
    if (NULL != m_pInstance)
    {
        delete m_pInstance;
        m_pInstance = NULL;
    }

    return (int) 0;
}

int CZoomManager::Init()
{
    int iRet = SUCCESS;
    char key[32] = {0};
	char get_value[32] = {0};
    int index = 0;

    //loadParam
    pCConfig = CConfig::GetInstance();
    if (NULL == pCConfig)
    {
        UnInit();
        return -1;
    }

	m_bSupport = 1;

	m_iZoomLens = pCConfig->GetValue(ZoomLensSection, ZoomLensComKey, 0);

    //读取刻度轴放大倍数
    m_iZoomScaleMagnification = 12.0;

#if defined (__PRODUCT_G85T__) || defined (__PRODUCT_W85T__)
    m_iZoomScaleMagnification = 18.0;
#elif defined (__PRODUCT_G86T__)
    m_iZoomScaleMagnification = 36.0;
#elif defined (__PRODUCT_G43TA__)
	if(m_iZoomLens == ForAndTwelve)
	{
		m_iZoomScaleMagnification = 18;
	}
	else if(m_iZoomLens == ForAndSix)
	{
		m_iZoomScaleMagnification = 12.0;
	}
#endif

    if (m_iZoomScaleMagnification < 1)
    {
        m_iZoomScaleMagnification = 1.0;
    }

	iRet = ZWSys_SetDragZoomScaleMagnification(m_iZoomScaleMagnification);
    if (SUCCESS != iRet)
    {
        printf("[ZOOM] set zoom scale magification:%d failed!!! Return:%d\n", m_iZoomScaleMagnification, iRet);
    }

	ZWSys_GetDragZoomRange(&g_zoom_min_multiple,&g_zoom_max_multiple);

    for (index = 0; index < MAX_ZOOM_PRESET_COUNT; ++index)
    {
        memset(key, 0, sizeof(key));
        snprintf(key,sizeof(key)-1 ,"%s_%d", ZoomPresetKey, index);
		pCConfig->GetValue(ZoomSection, key, "0.0",get_value,sizeof(get_value)-1);
        str2double(&m_iZoomPresetRatio[index], get_value);
    }
	#if defined __IS_SUPPORT_LONG_SHORT_FOCUS_ZOOM__
	ZWSys_SetZoomLens(m_iZoomLens);
	#endif

    printf("[ZOOM] init ratio:%f, scale magnification:%d m_bSupport:%d\n", m_iZoomRatio, m_iZoomScaleMagnification, m_bSupport);

    return SUCCESS;
}

void CZoomManager::UnInit()
{
    return ;
}

int CZoomManager::SetRatio(double ratio,int eOpreator)
{
    int iRet = SUCCESS;
	CPtzManager *pPtz = NULL;

	if (0 == m_bSupport)
	{
		return SUCCESS;
	}

    if ((ratio < g_zoom_min_multiple) || (ratio > g_zoom_max_multiple))
    {
        printf("[ZOOM] set zoom ratio:%f failed!!!\n", ratio);
        return -1;
    }

	if (DRAG_CLIENT_RADIO == eOpreator)
	{
		pPtz = CPtzManager::PTZ_GetInstance();
		pPtz->PTZ_SetPTZCtrlStatue(TIME_AFTER_NORMAL_CTRL);
	}

    iRet = ZWSys_SetDragZoomRatio(ratio,eOpreator);
    if (SUCCESS != iRet)
    {
        printf("[ZOOM] set zoom ratio:%f failed!!!Return:%d\n", ratio, iRet);
        return iRet;
    }

    m_iZoomRatio = ratio;

    return SUCCESS;
}

int CZoomManager::GetRatio(double *ratio)
{
    ZWSys_GetDragZoomRatio(ratio);
    return SUCCESS;
}

int CZoomManager::SetPresetCfg(int index, double ratio)
{
    int iRet = SUCCESS;
    char key[32] = {0};
	char setValue[32] = {0};

	if (0 == m_bSupport)
	{
		return SUCCESS;
	}

    if (index < 0 || index > MAX_ZOOM_PRESET_COUNT)
    {
        printf("[ZOOM] Preset index:%d error!!!\n", index);
        return -1;
    }

    if ((ratio < g_zoom_min_multiple) || (ratio > g_zoom_max_multiple))
    {
        printf("[ZOOM] Preset index:%d set zoom ratio:%f error!!!\n", index, ratio);
        return -1;
    }

    m_iZoomPresetRatio[index] = ratio;

    memset(key, 0, sizeof(key));
    snprintf(key,sizeof(key)-1, "%s_%d", ZoomPresetKey, index);
	snprintf(setValue,sizeof(setValue)-1, "%.1f", ratio);
    pCConfig->SetValue(ZoomSection, key, setValue);
    pCConfig->SaveFile();

    return iRet;
}

int CZoomManager::GetPresetCfg(int index, double *ratio)
{
    *ratio = m_iZoomPresetRatio[index];
    return 0;
}

int CZoomManager::RemovePresetCfg(int index)
{
    if (0 == m_bSupport)
	{
		return SUCCESS;
	}

    if (index < 0 || index > MAX_ZOOM_PRESET_COUNT)
    {
        printf("[ZOOM] Preset index:%d error!!!\n", index);
        return -1;
    }
    char key[32] = {0};
	char setValue[32] = {0};

    m_iZoomPresetRatio[index] = DRAG_ZOOM_PRESET_INIT_MULTIPLE;

    memset(key, 0, sizeof(key));
	snprintf(key,sizeof(key)-1, "%s_%d", ZoomPresetKey, index);
	snprintf(setValue,sizeof(setValue)-1, "%.1f", DRAG_ZOOM_PRESET_INIT_MULTIPLE);
    pCConfig->SetValue(ZoomSection, key, setValue);
    pCConfig->SaveFile();

    return 0;
}

int CZoomManager::SetDragZoomCmd(DRAG_ZOOM_CMD nCmd, int eOpreator)
{
    int ret = 0;
	CPtzManager *pPtz = NULL;

	printf("[ZOOM] nCmd(%d) eOpreator(%d) m_bSupport(%d)\n",nCmd,eOpreator,m_bSupport);

	if (0 == m_bSupport)
	{
		return SUCCESS;
	}

	pPtz = CPtzManager::PTZ_GetInstance();
	pPtz->PTZ_SetPTZCtrlStatue(TIME_AFTER_NORMAL_CTRL);

    ret = ZWSys_SetDragZoomCmd(nCmd,eOpreator);
    if (SUCCESS != ret)
    {
        printf("[ZOOM] set zoom cmd:%d failed!!!Return:%d\n", nCmd, ret);
    }

    return ret;
}

int CZoomManager::GetZoomScaleMagnification(int *ratio)
{
    if (ratio == NULL)
    {
        printf("[ZOOM] scale ratio is NULL\n");
        return -1;
    }

    *ratio = m_iZoomScaleMagnification;
    return 0;
}

int CZoomManager::IsSupport()
{
	return m_bSupport;
}

int CZoomManager::GetZoomLens(int *nLens)
{
    if (nLens == NULL)
    {
        printf("[ZOOM] nLensflag is NULL\n");
        return -1;
    }

    *nLens = m_iZoomLens;
    return 0;
}

#if defined __IS_SUPPORT_LONG_SHORT_FOCUS_ZOOM__
int CZoomManager::SetZoomLens(int nLens)
{
    if (nLens < 0 ||nLens >3)
    {
        printf("[ZOOM] nLensflag is err\n");
        return -1;
    }
     m_iZoomLens = nLens;
	ZWSys_SetZoomLens(m_iZoomLens);
	pCConfig->SetValue(ZoomLensSection, ZoomLensComKey, m_iZoomLens);
	
	if(m_iZoomLens == ForAndTwelve)
	{
		m_iZoomScaleMagnification = 18;
	}
	else if(m_iZoomLens == ForAndSix)
	{
		m_iZoomScaleMagnification = 12.0;
	}
	int iRet = ZWSys_SetDragZoomScaleMagnification(m_iZoomScaleMagnification);
    if (SUCCESS != iRet)
    {
        printf("[ZOOM] set zoom scale magification:%d failed!!! Return:%d\n", m_iZoomScaleMagnification, iRet);
    }
	ZWSys_GetDragZoomRange(&g_zoom_min_multiple,&g_zoom_max_multiple);
    return 0;
}
#endif


//计算x支持的最大变倍
static double CalcBoundaryMaxRadio(int x,double realMaxRadio,double usrMaxRadio,double curRadio)
{
	double fRatio = 1.0f;
	double fCurOneSideRatio = 1.0f;

	//当前已生效的单边倍数
	fCurOneSideRatio = sqrt(curRadio);

	//计算当前可以变化最大倍数
	fRatio = (double)(HUMAN_DETECT_LIMIT_WIDTH/2) / (abs(HUMAN_DETECT_LIMIT_WIDTH/2 - x));

	//整体可变化倍数
	fRatio = fRatio*fRatio;

	//转换成定制倍数
	fRatio = (fRatio-1.0f) * ((usrMaxRadio-1.0f) / (realMaxRadio-1.0f)) + 1.0f;

	//转换成基于起始倍数的可变倍数
	fRatio = fRatio * fCurOneSideRatio;

	return fRatio;
}

//计算实际可以放大的倍数
static double CalcEnlargeRadio(double fCurRatio,int x,int y,int width,int heigth)
{
	double fNewRatio = fCurRatio + DRAG_ZOOM_TRACK_STEP;
	double fRealMaxRatio = 1.0f;
	double fsureMaxRatio = 1.0f;
	double ftop = 1.0f;
	double fbottom = 1.0f;
	double fleft = 1.0f;
	double fright = 1.0f;
	double fOneSideUserMaxMul = 1.0f;
	double fOneSideRealMaxMul = 1.0f;
    
    if (fNewRatio > g_zoom_max_multiple)
    {
        fNewRatio = g_zoom_max_multiple;
    }
    ZWSys_GetRealMaxRatio(&fRealMaxRatio);
    fOneSideUserMaxMul = sqrt(g_zoom_max_multiple);
    fOneSideRealMaxMul = sqrt(fRealMaxRatio);

	//计算可以放大的最大倍数
	ftop = CalcBoundaryMaxRadio(y,fOneSideRealMaxMul,fOneSideUserMaxMul,fCurRatio);
	fbottom = CalcBoundaryMaxRadio(y+heigth,fOneSideRealMaxMul,fOneSideUserMaxMul,fCurRatio);
	fleft = CalcBoundaryMaxRadio(x,fOneSideRealMaxMul,fOneSideUserMaxMul,fCurRatio);
	fright = CalcBoundaryMaxRadio(x+width,fOneSideRealMaxMul,fOneSideUserMaxMul,fCurRatio);
	
	fsureMaxRatio = ftop;
	if (fsureMaxRatio > fbottom)
	{
		fsureMaxRatio = fbottom;
	}
	if (fsureMaxRatio > fleft)
	{
		fsureMaxRatio = fleft;
	}
	if (fsureMaxRatio > fright)
	{
		fsureMaxRatio = fright;
	}

	printf("[ZOOM] fbottom(%.1f) ftop(%.1f) fleft(%.1f) fright(%.1f) fsureMaxRatio(%.1f)\n",fbottom,ftop,fleft,fright,fsureMaxRatio);

	if (fNewRatio > fsureMaxRatio)
	{
		fNewRatio = fCurRatio;
	}
	
	return fNewRatio;

}

int CZoomManager::TrackMultipleCalc(int x,int y,int width,int heigth)
{
	static int nEnlargeCnt = 0;
    static int nLessenCnt = 0;
	double fWidthRatio = 0.0f;
	double fHeightRatio = 0.0f;
	int nOpreate = 0;
	double fCurRatio = 1.0f;
	double fNewRatio = 1.0f;

	if ((0 == x) && (0 == y) && (0 == width) && (0 == heigth))
	{
		return 0;
	}

	fWidthRatio = (double)width / HUMAN_DETECT_LIMIT_WIDTH;
	fHeightRatio = (double)heigth / HUMAN_DETECT_LIMIT_HEIGHT;

	//printf("[ZOOM] x(%d) y(%d) width(%d) heigth(%d)\n",x,y,width,heigth);
	//printf("[ZOOM] fWidthRatio(%.1f) fHeightRatio(%.1f)\n",fWidthRatio,fHeightRatio);
    if ( (fWidthRatio < DRAG_ZOOM_TRACKER_WIDTH_RATIO_ENLARGE && fHeightRatio < DRAG_ZOOM_TRACKER_HEIGHT_RATIO_ENLARGE)
        || (fHeightRatio < DRAG_ZOOM_TRACKER_HEIGHT_RATIO_ENLARGE_MAX) )
    {
        nEnlargeCnt++;
        nLessenCnt = 0;
    }
    else if ( (fWidthRatio > DRAG_ZOOM_TRACKER_WIDTH_RATIO_LESSEN && fHeightRatio > DRAG_ZOOM_TRACKER_HEIGHT_RATIO_LESSEN)
        || (fHeightRatio > DRAG_ZOOM_TRACKER_HEIGHT_RATIO_LESSEN_MAX) )
    {
        nEnlargeCnt = 0;
        nLessenCnt++;
    }
    else
    {
        nEnlargeCnt = 0;
        nLessenCnt = 0;        
    }
    if (nEnlargeCnt >= DRAG_ZOOM_CHECK_COUNT)
    {//增大倍数
        nOpreate = 1;
        nEnlargeCnt = 0;
    }
    else if (nLessenCnt >= DRAG_ZOOM_CHECK_COUNT)
    {//缩小倍数
        nOpreate = 2;
        nLessenCnt = 0;
    }

	ZWSys_GetDragZoomRatio(&fCurRatio);

	if (1 == nOpreate)
	{
		//放大时，保证目标在画面内
		fNewRatio = CalcEnlargeRadio(fCurRatio,x,y,width,heigth);

		//printf("[ZOOM]In fNewRatio(%.1f)\n",fNewRatio);
		ZWSys_SetDragZoomRatio(fNewRatio,DRAG_TRACK_RADIO);
	}
	else if (2 == nOpreate)
	{//缩小不用做限制，目标必然在画面内
		fNewRatio = fCurRatio - DRAG_ZOOM_TRACK_STEP;
		if (fNewRatio < g_zoom_min_multiple)
		{
			fNewRatio = g_zoom_min_multiple;
		}

		//printf("[ZOOM]Out fNewRatio(%.1f)\n",fNewRatio);
		ZWSys_SetDragZoomRatio(fNewRatio,DRAG_TRACK_RADIO);
	}

	return 0;
}

#endif

