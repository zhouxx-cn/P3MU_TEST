#ifndef __VAUTORATE_H__
#define __VAUTORATE_H__

#include "xrsdef.h"
#ifdef __cplusplus
extern "C" {
#endif  //@__cplusplus
// #include "xrsstd.h"
#include "vcomm.h"

#define VAUTORATE_VENC_MAX_CHN_NUM 4

typedef enum {
    VAUTORATE_ERROR_START = COMP_ERROR_START, /* 用户模块起始错误号 */
    VAUTORATE_ERROR_UNINIT,                   /* 模块未初始化*/
    VAUTORATE_ERROR_REINIT,                   /* 模块重复初始化*/
    VAUTORATE_ERROR_SDK_API,                  /* SDK API调用失败*/  
} VAUTORATEERROR_E;
#define VAUTORATE_ERRNO(no) ERRNO(no, COMP_VAUTORATE)

#define VAUTORATE_ERR(str, args...) TRACE_ERR(COMP_VAUTORATE, str, ##args)
#define VAUTORATE_WARN(str, args...) TRACE_WARN(COMP_VAUTORATE, str, ##args)
#define VAUTORATE_FLOW(str, args...) TRACE_FLOW(COMP_VAUTORATE, str, ##args)
#define VAUTORATE_INFO(str, args...) TRACE_INFO(COMP_VAUTORATE, str, ##args)
#define VAUTORATE_DBG0(str, args...) TRACE_DBG0(COMP_VAUTORATE, str, ##args)

/**
  * @brief 码率自适应的属性配置
  */
typedef struct {
    UINT32 RunPeriod;             /**<  码率自适应运行的周期，值越小调整越快 取值范围[300, 2000] 单位:ms 建议500 */
    UINT32 DefaultVideoRate;     /**<  不在运动场景下，期望码率上限 取值范围[20, 5000] 单位kbps */
    UINT32 MaxVideoRate;         /**<  最大视频编码码率 取值范围[20, 5000] 单位kbps */
    UINT32 MinVideoRate;         /**<  最小视频编码码率 取值范围[20, max_video_rate] 单位kbps */
    UINT32 StepRate;              /**<  单次调整的视频码率 取值范围[10, 1000] 单位kbps 建议64或者100 */
    UINT32 AdjustDir;             /**<  调整的方向，值越大画质优先，值越小流畅优先 取值范围[0, 8] 建议4 */
} VAUTORATE_ATTR_S;

/**
  * @brief 通知事件类型
  */
typedef enum {
    VAUTORATE_EVENT_UP,                   /**< 网络质量好转，参考建议目标码率调整 */
    VAUTORATE_EVENT_DOWN,                 /**< 网络质量变差，参考建议目标码率调整 */
    VAUTORATE_EVENT_HQ_NET,               /**< 网络质量优，已调到最大码率，空口仍无压力 */
    VAUTORATE_EVENT_BQ_NET,               /**< 网络质量差，已调到最低码率，空口仍然有压力，如有需要可降帧、降分辨率等 */
    VAUTORATE_EVENT_HQ_VIDEO,             /**< 瞬时码率超过空口速率。*/
} VAUTORATE_EVENT_E;

/**
  * @brief 通知事件消息
  */
typedef struct {
    UINT32 TargetRate;
    UINT32 VencCount;
    SINT32 VencHdl[VAUTORATE_VENC_MAX_CHN_NUM];
} VAUTORATE_EVENT_PARA_S;

typedef struct {
    SINT32 StepRate;
    UINT32 JudgeLevel;
} VAUTORATE_TABLE_S;

/**
  * @brief 回调用户接口
  */
typedef struct {
    SINT32  (*VAutoRate_Get_ModemInfo)(SINT8 *Buf, UINT32 BufLen);          /**< 获取4G模组空口预估速率 */
    SINT32  (*VAutoRate_Get_ModemCurrRate)(SINT8 *Buf, UINT32 BufLen);             /**< 获取4G模组空口实时速率 */
    void (*VAutoRate_Event_Report)(VAUTORATE_EVENT_E Event, VAUTORATE_EVENT_PARA_S *Para);/**< 码率调整事件上报 */
    void (*VAutoRate_Get_UserTable)(UINT32 CurrRate, UINT32 BuffUsed, VAUTORATE_TABLE_S *Table);
} VAUTORATE_CALLBACK_S;

/**
  * @brief 码率控制视频通道配置
  */
typedef struct {
    UINT32 Count;
    SINT32 VencHdl[VAUTORATE_VENC_MAX_CHN_NUM];
    UINT32 RefCount[VAUTORATE_VENC_MAX_CHN_NUM];
} VAUTORATE_VENC_CHN_ATTR_S;

/******************************************************************************
 * 函数功能: 打开模块,分配系统资源,模块初始化
 * 输入参数: RateAttr: 码率自适应属性配置
 *           Callback: 回调用户接口
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 VAutoRate_Open(const VAUTORATE_ATTR_S *RateAttr, const VAUTORATE_CALLBACK_S *Callback);

/******************************************************************************
 * 函数介绍: 关闭模块,释放系统资源
 * 输入参数: 无
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 VAutoRate_Close(void);

/******************************************************************************
 * 函数介绍: 设置当前的码率自适应接口属性参数
 * 输入参数: RateAttr: 码率自适应属性
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 VAutoRate_Set_Attr(const VAUTORATE_ATTR_S *RateAttr);

/******************************************************************************
 * 函数介绍: 设置当前的码率自适应接口属性参数
 * 输入参数: 无
 * 输出参数: RateAttr: 码率自适应属性
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 VAutoRate_Get_Attr(VAUTORATE_ATTR_S *RateAttr);

/******************************************************************************
 * 函数介绍: 向码率自适应增加一个编码通道
 * 输入参数: VencChn: 编码通道号
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 VAutoRate_Add_VencChn(SINT32 VencChn);

/******************************************************************************
 * 函数介绍: 从码率自适应删除一个编码通道
 * 输入参数: VencChn: 编码通道号
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 VAutoRate_Remove_VencChn(SINT32 VencChn);

/******************************************************************************
 * 函数介绍: 获取生效的编码通道属性
 * 输入参数: ChnAttr：编码通道属性
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 VAutoRate_Get_VencChnAttr(VAUTORATE_VENC_CHN_ATTR_S *ChnAttr);

/******************************************************************************
 * 函数介绍: 获取到码流时通知到码率自适应
 * 输入参数: Chn: 编码通道号
 *           Data: 码流结构体指针
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 VAutoRate_Notify_On_Stream(SINT32 Chn, const SINT8 *Data);

/******************************************************************************
 * 函数介绍: 设置调试日志的开关和路径
 * 输入参数: Enable: 是否保存log
 *           FilePath: 保存log的目录
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 VAutoRate_Set_LogFile(XRS_BOOL Enable, const UINT8 *FilePath);

#ifdef __cplusplus
}
#endif  //@__cplusplus
#endif  /* __VAUTORATE_H__ */
