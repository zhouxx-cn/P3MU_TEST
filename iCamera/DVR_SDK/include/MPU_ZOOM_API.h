#ifndef __MPU_ZOOM_API_H__
#define __MPU_ZOOM_API_H__

#include "zwsdk.h"

#ifdef __DIGITAL_ZOOM_DRAG_SUPPORT__
//后续对外接口提供
int ZOOM_Init();
int ZOOM_Uninit();
int MPU_ZOOM_SetRatio(double ratio,int eOpreator);

int MPU_ZOOM_GetRatio(double *ratio);

int MPU_ZOOM_SetPresetCfg(int index, double ratio);

int MPU_ZOOM_GetPresetCfg(int index, double *ratio);

int MPU_ZOOM_RemovePresetCfg(int index);

int MPU_ZOOM_SetDragZoomCmd(DRAG_ZOOM_CMD nCmd, int eOpreator);

int MPU_ZOOM_GetZoomLens(int *Lens);
#if defined __IS_SUPPORT_LONG_SHORT_FOCUS_ZOOM__
int MPU_ZOOM_SetZoomLens(int Lens);
#endif
#endif

#endif