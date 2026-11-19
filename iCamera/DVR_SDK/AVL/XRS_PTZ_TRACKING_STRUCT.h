/*******************************************************************************
*
* 版权信息：版权所有 (c) 202X, 武汉芯睿视科技有限公司, 保留所有权利
*
* 文件名称：XRS_PTZ_Tracking_Struct.h
* 摘    要：芯睿视 球机跟踪结构体头文件
*
* 当前版本：0.0.0.1
* 作    者：吴胜斌
* 日    期：20211209
* 备    注：
*          
********************************************************************************/
#ifndef _XRS_PTZ_TRACKING_STRUCT_H_
#define _XRS_PTZ_TRACKING_STRUCT_H_
/*****************点结构体***********************/
typedef struct XRS_POINT_ST
{
	float  x;
	float  y;
} POINT_ST;

/**************PTZ水平方向标志位******************/
typedef enum XRS_OBJ_PAN_DIRECTION_ENUM
{
	OBJ_PAN_CENTER		= 0,						// 目标在视野中心
	OBJ_PAN_LEFT		= 1,						// 目标在视野中心左边
	OBJ_PAN_RIGHT		= 2,						// 目标在视野中心右边

}OBJ_PAN_DIRECTION_E;

/**************PTZ垂直方向标志位******************/
typedef enum XRS_OBJ_TILT_DIRECTION_ENUM
{
	OBJ_TILT_CENTER		= 0,						// 目标在视野中心
	OBJ_TILT_UP			= 1,						// 目标在视野中心上方
	OBJ_TILT_DOWN		= 2,						// 目标在视野中心下方

}OBJ_TILT_DIRECTION_E;


/**************PTZ调整结构体*********************/
typedef struct XRS_ROTATION_ST
{
	bool initPTZ;									// 是否回到预置点；
	OBJ_PAN_DIRECTION_E objPanDirection;			// 目标相对于水平视野中心的方向
	OBJ_TILT_DIRECTION_E objTiltDierction;			// 目标相对于垂直视野中心的方向
	double panAngle;								// 目标在水平方向上需要调整的角度
	double tiltAngle;								// 目标在垂直方向上需要调整的角度

}ROTATION_ST;


/****************坐标结构体*********************/
typedef struct XRS_RECT_ST
{

	int x;											// 左上角横坐标
	int y;											// 左上角纵坐标
	int wide;										// 宽
	int height;										// 高

}RECT_ST;

/************系统检测的人形结构体****************/
typedef struct XRS_PTZ_TRACK_TASK_ST
{
	RECT_ST *human_list;							// 外部传入的人形队列
	int human_list_size;							// 外部传入的人形队列长度

}PTZ_TRACK_TASK_S;


/**************input Params******************/
typedef struct XRS_PTZ_TRACK_INPUT_PARAM_ST
{
	// 图像基本参数
	int imgWide;									// 图像的分辨率宽，像素
	int imgHeight;									// 图像的分辨率高，像素
	double panThr;									// 目标在画面中允许偏离视野中心的水平距离，像素
	double tiltThr;									// 目标在画面中允许偏离视野中心的垂直距离，像素
	int max_skip_tracking_frame;					// 目标脱离的最大帧数；

	// 根据预置点计算
	double ptz_scale_left;							// 当前云台水平往左旋转的最大角度，单位°
	double ptz_scale__right;						// 当前云台水平往右旋转的最大角度，单位°
	double ptz_scale_up;							// 当前云台垂直向上旋转的最大角度，单位°
	double ptz_scale_down;							// 当前云台垂直向下旋转的最大角度，单位°

	// 传感器基本参数
	double Ws;										// 传感器的宽度，毫米
	double Hs;										// 传感器的高度，毫米
	double currentFocus;							// 镜头焦距

	// 放置人形漏检抖动
	int distanceThr;								// 距离阈值
	int gravityChangeThr;							// 重心变化次数阈值

	int debug = 0;									// 调试模式

}PTZ_TRACK_INPUT_PARAM_ST;

#endif