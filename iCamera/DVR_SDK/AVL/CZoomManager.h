#ifndef __CZOOM__H__
#define __CZOOM__H__

#include <pthread.h>
#include "global_macro.h"
#include "zwsdk.h"

#if defined(__DIGITAL_ZOOM_DRAG_SUPPORT__)

#define DRAG_ZOOM_MAX_MULTIPLE          10
#define DRAG_ZOOM_MIN_MULTIPLE          1
#define DRAG_ZOOM_SWITCH_MULTIPLE       5

#define DRAG_ZOOM_SCALE_MAGNIFY_2X_MULTIPLE    2

#define DRAG_ZOOM_PRESET_INIT_MULTIPLE  (0.0f)

#define MAX_ZOOM_PRESET_COUNT    6

#define ZOOM_WIDE_FOCUS 0
#define ZOOM_MIDDLE_FOCUS 1
#define ZOOM_TELE_FOCUS 2
typedef enum
{
	ForAndTwelve = 0,
	ForAndEight,
	ForAndSix
}ZoomLensType;
	
class CZoomManager
{

public:
    #define SUCCESS             0
    #define FAILED              (-1)

public:
    CZoomManager();
    ~CZoomManager();

    int Init();
    void UnInit(void);

    //!\brief 获取实例指针
    //!\param void
    //!\return  成功返回指针，失败为NULL
    static CZoomManager *GetInstance();

    //!\brief 释放实例指针
    //!\return  成功返回0，失败为-1
    static int ReleaseInstance();

    //设置Zoom倍数
    int SetRatio(double ratio,int eOpreator);

    //获取Zoom倍数
    int GetRatio(double *ratio);

    //设置预置点的倍数
    int SetPresetCfg(int index, double ratio);

    int GetPresetCfg(int index, double *ratio);

    int RemovePresetCfg(int index);

    int SetDragZoomCmd(DRAG_ZOOM_CMD nCmd, int eOpreator);

    int GetZoomScaleMagnification(int *ratio);

	int IsSupport();
	
	int GetZoomLens(int *lens);
	#if defined __IS_SUPPORT_LONG_SHORT_FOCUS_ZOOM__
	int SetZoomLens(int lens);
	#endif
	//计算跟踪倍数
	int TrackMultipleCalc(int x,int y,int width,int heigth);


private:

    static CZoomManager *m_pInstance;

    //配置文件
    CConfig *pCConfig;

    //当前倍数
    double m_iZoomRatio;

    //预置点保存的倍数
    double m_iZoomPresetRatio[MAX_ZOOM_PRESET_COUNT];
	
    //长焦镜头 6mm-8mm-12mm标志，0-12mm，1-8mm，2-6mm
    int m_iZoomLens;
	
    //刻度轴最大倍数
    int m_iZoomScaleMagnification;

	//是否支持
	int m_bSupport;
};

#endif

#endif // __CZOOM__H__