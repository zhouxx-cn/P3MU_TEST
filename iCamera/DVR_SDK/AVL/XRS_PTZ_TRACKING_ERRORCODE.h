/*******************************************************************************
*
* 版权信息：版权所有 (c) 202X, 武汉芯睿视科技有限公司, 保留所有权利
*
* 文件名称：XRS_PTZ_TRACKING_ErrorCode.h
* 摘    要：芯睿视 球机跟踪错误码定义头文件
*
* 当前版本：0.0.0.1
* 作    者：吴胜斌
* 日    期：20211209
* 备    注：
********************************************************************************/
#ifndef _XRS_PTZ_TRACKING_ERRORCODE_H_
#define _XRS_PTZ_TRACKING_ERRORCODE_H_

#define XRS_PTZ_TRACKING_ERROR_SUCESS									0				// 成功
#define XRS_PTZ_TRACKING_ERROR_FAILED									-1              // 失败

#define XRS_PTZ_TRACKING_ERROR_BASE									(40000)			    // 错误码起始位置
#define XRS_ERROR(idx)							((XRS_PTZ_TRACKING_ERROR_BASE ) + idx)  // 定义错误码函数

#define XRS_PTZ_TRACKING_ERROR_CV_EXCEPTION                   XRS_ERROR(1)			    // opencv抛出异常
#define XRS_PTZ_TRACKING_ERROR_MEMERY_ALLOC		              XRS_ERROR(2)				// 分配内存异常
#define XRS_PTZ_TRACKING_ERROR_EMPTY_POINT			          XRS_ERROR(3)				// 空指针
#define XRS_PTZ_TRACKING_ERROR_GRABIMG_NULL			          XRS_ERROR(4)				// 获取图像为空
#define XRS_PTZ_TRACKING_ERROR_FORCE_STOP			          XRS_ERROR(5)				// 程序强制停止
#define XRS_PTZ_TRACKING_ERROR_IMAGE_FORMAT                   XRS_ERROR(6)				// 图像格式错误
#define XRS_PTZ_TRACKING_ERROR_IMGTRANS_WRONG                 XRS_ERROR(7)				// 图像转换错误
#define XRS_PTZ_TRACKING_ERROR_IMAGE_KERNELSIZE			      XRS_ERROR(8)				// 图像掩膜尺寸不正确
#define XRS_PTZ_TRACKING_ERROR_INPUT_ID_WRONG				  XRS_ERROR(9)				// 传入的目标ID错误
#define XRS_PTZ_TRACKING_ERROR_INPUT_PARAMS_WRONG			  XRS_ERROR(10)			    // 传入参数错误
#define XRS_PTZ_TRACKING_ERROR_INPUT_PARAMS_LENGTH_WRONG      XRS_ERROR(11)			    // 传入参数长度错误



// 参数检查
#define CHECK_PARAM(val, low, hight, code) if (val > hight || val < low) {return code;}

#endif // _XRS_ERRORCODE_H_
