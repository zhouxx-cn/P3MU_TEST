/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 */
#ifndef OT_AUTORATE_H
#define OT_AUTORATE_H

#include "ot_type.h"
#include "ot_common_venc.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#define OT_AUTORATE_VENC_MAX_CHN_NUM 4  /* 支持的最大可调的编码通道个数 */

#define OT_ID_AUTORATE 0x41
/* GENERAL STRUCTURES */
/*
 * AUTORATE Error Code
 * 0x40 : AUTORATE_NOT_INIT
 * 0x41 : AUTORATE_CHN_FULL
 * 0x42 : AUTORATE_INVALARG
 * 0x43 : AUTORATE_NO_RESOURCES
 * 0x44 : AUTORATE_CHN_NEED_REMOV
 * 0x45 : AUTORATE_PATH_NOT_EXIST
 */
typedef enum {
    OT_ERR_AUTORATE_NOT_INIT                = 0x40, /* AUTORATE */
    OT_ERR_AUTORATE_CHN_FULL                = 0x41, /* AUTORATE chn full */
    OT_ERR_AUTORATE_INVALARG                = 0x42, /* AUTORATE invalarg */
    OT_ERR_AUTORATE_NO_RESOURCES            = 0x43, /* AUTORATE no resources */
    OT_ERR_AUTORATE_CHN_NEED_REMOV          = 0x44, /* AUTORATE chn need remove */
    OT_ERR_AUTORATE_PATH_NOT_EXIST          = 0x45, /* AUTORATE path not exist */
} ot_autorate_err_code;

#define OT_ERR_AUTORATE_NULL_PTR       OT_DEFINE_ERR(OT_ID_AUTORATE, OT_ERR_LEVEL_ERROR, OT_ERR_NULL_PTR)
#define OT_ERR_AUTORATE_NOT_SUPPORT    OT_DEFINE_ERR(OT_ID_AUTORATE, OT_ERR_LEVEL_ERROR, OT_ERR_NOT_SUPPORT)

#define OT_ERR_AUTORATE_NOT_INIT       OT_DEFINE_ERR(OT_ID_AUTORATE, OT_ERR_LEVEL_ERROR, OT_ERR_AUTORATE_NOT_INIT)
#define OT_ERR_AUTORATE_CHN_FULL       OT_DEFINE_ERR(OT_ID_AUTORATE, OT_ERR_LEVEL_ERROR, OT_ERR_AUTORATE_CHN_FULL)
#define OT_ERR_AUTORATE_INVALARG       OT_DEFINE_ERR(OT_ID_AUTORATE, OT_ERR_LEVEL_ERROR, OT_ERR_AUTORATE_INVALARG)
#define OT_ERR_AUTORATE_NO_RESOURCES   OT_DEFINE_ERR(OT_ID_AUTORATE, OT_ERR_LEVEL_ERROR, OT_ERR_AUTORATE_NO_RESOURCES)
#define OT_ERR_AUTORATE_CHN_NEED_REMOV OT_DEFINE_ERR(OT_ID_AUTORATE, OT_ERR_LEVEL_ERROR, OT_ERR_AUTORATE_CHN_NEED_REMOV)
#define OT_ERR_AUTORATE_PATH_NOT_EXIST OT_DEFINE_ERR(OT_ID_AUTORATE, OT_ERR_LEVEL_ERROR, OT_ERR_AUTORATE_PATH_NOT_EXIST)

/**
  * @brief 码率自适应的属性配置
  */
typedef struct {
    td_u32 run_period;             /**<  码率自适应运行的周期，值越小调整越快 取值范围[300, 2000] 单位:ms 建议500 */
    td_u32 default_video_rate;     /**<  不在运动场景下，期望码率上限 取值范围[20, 5000] 单位kbps */
    td_u32 max_video_rate;         /**<  最大视频编码码率 取值范围[20, 5000] 单位kbps */
    td_u32 min_video_rate;         /**<  最小视频编码码率 取值范围[20, max_video_rate] 单位kbps */
    td_u32 step_rate;              /**<  单次调整的视频码率 取值范围[10, 1000] 单位kbps 建议64或者100 */
    td_u32 adjust_dir;             /**<  调整的方向，值越大画质优先，值越小流畅优先 取值范围[0, 8] 建议4 */
} ot_autorate_attr;

/**
  * @brief 通知事件类型
  */
typedef enum {
    OT_EVENT_UP,                   /**< 网络质量好转，参考建议目标码率调整 */
    OT_EVENT_DOWN,                 /**< 网络质量变差，参考建议目标码率调整 */
    OT_EVENT_HQ_NET,               /**< 网络质量优，已调到最大码率，空口仍无压力 */
    OT_EVENT_BQ_NET,               /**< 网络质量差，已调到最低码率，空口仍然有压力，如有需要可降帧、降分辨率等 */
    OT_EVENT_HQ_VIDEO,             /**< 瞬时码率超过空口速率。*/
} ot_autorate_event;

/**
  * @brief 通知事件消息
  */
typedef struct {
    td_u32 target_rate;
    td_u32 venc_count;
    td_s32 venc_hdl[OT_AUTORATE_VENC_MAX_CHN_NUM];
} ot_autorate_event_para;

typedef struct {
    td_s32 step_rate;
    td_u32 judge_level;
} ot_autorate_table;

/**
  * @brief 回调用户接口
  */
typedef struct {
    td_s32  (*ot_autorate_get_modem_info)(char *buf, td_u32 buf_len);          /**< 获取4G模组空口预估速率 */
    td_s32  (*ot_autorate_get_modem_curr_rate)(char *buf, td_u32 buf_len);             /**< 获取4G模组空口实时速率 */
    td_void (*ot_autorate_event_report)(ot_autorate_event ot_event, ot_autorate_event_para *para);/**< 码率调整事件上报 */
    td_void (*ot_autorate_get_user_table)(td_u32 curr_rate, td_u32 buff_used, ot_autorate_table *table);
} ot_autorate_call_back;

/**
  * @brief 码率控制视频通道配置
  */
typedef struct {
    td_u32 count;
    td_s32 venc_hdl[OT_AUTORATE_VENC_MAX_CHN_NUM];
    td_u32 ref_count[OT_AUTORATE_VENC_MAX_CHN_NUM];
} ot_autorate_venc_chn;

/**
  * @brief 码率视频帧数信息
  */
typedef struct {
    td_u32 stream_size;
    ot_venc_ref_type ref_type;
} ot_autorate_stream;

/**
 * @brief       码率自适应初始化
 * @par         Sync/Async: Synchronous
 * @par         Reentrancy: Reentrant
 * @retval      TD_SUCCESS: 获取成功， TD_FAILURE：获取失败
 */
td_s32 ot_autorate_init(const ot_autorate_attr *rate_attr, const ot_autorate_call_back *cb);

/**
 * @brief       码率自适应去初始化
 * @par         Sync/Async: Synchronous
 * @par         Reentrancy: Reentrant
 * @retval      TD_SUCCESS: 获取成功， TD_FAILURE：获取失败
 */
td_s32 ot_autorate_deinit(td_void);

/**
 * @brief       设置当前的码率自适应接口属性参数
 * @par         Sync/Async: Synchronous
 * @par         Reentrancy: Reentrant
 * @param[in]  rate_attr:  码率自适应配置属性
 * @retval      TD_SUCCESS: 获取成功， TD_FAILURE：获取失败
 */
td_s32 ot_autorate_set_attr(const ot_autorate_attr *rate_attr);

/**
 * @brief       获取当前的码率自适应接口属性参数
 * @par         Sync/Async: Synchronous
 * @par         Reentrancy: Reentrant
 * @param[out]  rate_attr:  码率自适应配置属性
 * @retval      TD_SUCCESS: 获取成功， TD_FAILURE：获取失败
 */
td_s32 ot_autorate_get_attr(ot_autorate_attr *rate_attr);

/**
 * @brief       向码率自适应增加一个编码通道
 * @par         Sync/Async: Synchronous
 * @par         Reentrancy: Reentrant
 * @param[in]  venc_hdl:  编码通道号
 * @retval      TD_SUCCESS: 增加成功， TD_FAILURE：增加失败
 */
td_s32 ot_autorate_add_venc_chn(td_s32 venc_hdl);

/**
 * @brief       从码率自适应删除一个编码通道
 * @par         Sync/Async: Synchronous
 * @par         Reentrancy: Reentrant
 * @param[in]  venc_hdl:  编码通道号
 * @retval      TD_SUCCESS: 删除成功， TD_FAILURE：删除失败
 */
td_s32 ot_autorate_remove_venc_chn(td_s32 venc_hdl);

/**
 * @brief       查询生效的编码通道
 * @par         Sync/Async: Synchronous
 * @par         Reentrancy: Reentrant
 * @param[out]  venc_chn:  编码通道配置
 * @retval      TD_SUCCESS: 查询成功， TD_FAILURE：查询失败
 */
td_s32 ot_autorate_query_venc_chn(ot_autorate_venc_chn *venc_chn);

/**
 * @brief       获取到码流时通知到码率自适应
 * @par         Sync/Async: Synchronous
 * @par         Reentrancy: Reentrant
 * @param[in]   chn:  编码通道号
 * @param[in]   stream:  码流结构体
 * @retval      无
 */
td_void ot_autorate_notify_on_stream(ot_venc_chn chn, const ot_autorate_stream *stream);

/**
 * @brief       设置调试日志的开关和路径
 * @par         Sync/Async: Synchronous
 * @par         Reentrancy: Reentrant
 * @param[in]   enable:  是否保存log
 * @param[in]   file_path:  保存log的目录
 * @retval      无
 */
td_s32 ot_autorate_set_log_file(td_bool enable, const td_char *file_path);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif