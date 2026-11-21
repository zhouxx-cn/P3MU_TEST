/**
 * @file    ui_cam.h
 * @brief   拇指相机业务控制器：模式/录制状态机 + 可注入的相机动作钩子。
 *
 * @details
 *  UI 页面只调用本控制器的语义接口（开始录像、拍照、切模式…），
 *  真正的 VENC 录像/抓拍/延时逻辑通过 ui_cam_ops_t 钩子注入，
 *  样机默认用桩实现（仅改状态+发 toast），真机把钩子接到
 *  DVR_SDK 的 SnapShot/录像接口即可。业务与界面解耦，便于迁移。
 */
#ifndef __UI_CAM_H__
#define __UI_CAM_H__

#include "ui_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @brief 相机动作钩子（真机注入；样机可全 NULL 用桩）。 */
typedef struct {
    int (*start_record)(void);           /**< 开始录像（主码流） */
    int (*stop_record)(void);            /**< 停止录像 */
    int (*take_photo)(void);             /**< 拍照一张 */
    int (*start_timelapse)(int interval);/**< 开始延时，interval 秒/帧 */
    int (*stop_timelapse)(void);         /**< 停止延时 */
    int (*get_free_minutes)(void);       /**< 查询剩余可录分钟 */
} ui_cam_ops_t;

/** @brief 初始化控制器（默认录像模式、未录制）。 */
void ui_cam_init(void);

/** @brief 注入相机动作实现。 @param ops 钩子集合，NULL 恢复桩。 */
void ui_cam_set_ops(const ui_cam_ops_t *ops);

/** @brief 获取当前工作模式。 */
ui_work_mode_t ui_cam_mode(void);

/** @brief 切换工作模式（同时广播 UI_BIZ_MODE 给状态栏）。 */
void ui_cam_set_mode(ui_work_mode_t mode);

/**
 * @brief 从回放/设置页退出：恢复进入前的拍摄模式（3 键布局无返回键时的统一出口）。
 */
void ui_cam_mode_back(void);

/** @brief 是否正在录制。 */
int ui_cam_is_recording(void);

/**
 * @brief 录像开始/停止切换（录像模式下按 OK 触发）。
 * @return UI_OK / UI_ERR
 */
int ui_cam_toggle_record(void);

/**
 * @brief 拍照（拍照模式下按 OK 触发）。
 * @return UI_OK / UI_ERR
 */
int ui_cam_snap(void);

/**
 * @brief 延时摄影开始/停止切换。
 * @param interval_sec 间隔秒数
 * @return UI_OK / UI_ERR
 */
int ui_cam_toggle_timelapse(int interval_sec);

/**
 * @brief 周期性业务滴答：累加录制时长、刷新剩余容量，广播给状态栏。
 * @note  由预览页的 1Hz 定时器调用。
 */
void ui_cam_tick_1hz(void);

#ifdef __cplusplus
}
#endif

#endif /* __UI_CAM_H__ */
