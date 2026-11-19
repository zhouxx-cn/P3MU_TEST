#ifndef	__USR_ZOOM_H__
#define	__USR_ZOOM_H__

#include "defs.h"
#include "Hi_MPP.h"
#include "EncoderLib.h"
#include "zwsdk.h"

typedef enum
{
	ForAndTwelve = 0,
	ForAndEight,
	ForAndSix
}ZoomLensType;


#if defined(__DIGITAL_ZOOM_DRAG_SUPPORT__)
int Zoom_SetDragZoomSupport(int bSupport);
int Zoom_SetTrackZoomEn(int TrackZoomEn);
int Zoom_SetDragZoomRatio(double ratio,int eOpreator);
int Zoom_GetDragZoomRatio(double *ratio);
int Zoom_SetDragZoomCmd(DRAG_ZOOM_CMD nCmd,int eOpreator);
int Zoom_GetDragZoomChgStatus(int *status);
int Zoom_SetDragZoomScaleMagnification(double ratio);
int Zoom_GetDragZoomRange(double *min,double *max);
double Zoom_AnalyseDragCmdEx(DRAG_ZOOM_CMD nCmdEx, double nCurRatio, double nMaxRatio);
void Zoom_StopDragZoom();
int Zoom_StartDragZoom();

#if defined(__IS_SUPPORT_LONG_SHORT_FOCUS_ZOOM__)
void Zoom_SwitchFocusTiming(double curRatio, double newRatio, int *needSwitch, int *targetSensor);
int Zoom_GetEachFocusZoom(double *shortZoom, double *middleZoom, double *longZoom);
int Zoom_SwicthFocusOpt(int switchFlg, int targetSensor, int *updRatio);
int Zoom_SetZoomLens(int nLens);
int Zoom_GetCurrentSensor();
#endif
double Zoom_GetCurRatio();


void Zoom_RatioConverseAdapt(double newRatio, double *relRatio);
#endif

#endif
