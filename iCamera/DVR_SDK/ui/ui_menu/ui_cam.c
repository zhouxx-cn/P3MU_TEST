/**
 * @file    ui_cam.c
 * @brief   拇指相机业务控制器实现（状态机 + 桩钩子 + 状态广播）。
 */
#include "ui_cam.h"
#include "ui_msgq.h"
#include "ui_popup.h"

static ui_work_mode_t s_mode = UI_MODE_RECORD;
static int            s_recording = 0;
static int            s_timelapse_on = 0;
static int            s_rec_seconds = 0;
static int            s_free_minutes = 120;   /* 桩：假设 2 小时余量 */
static ui_cam_ops_t  s_ops = {0};

/* 广播业务状态给状态栏/页面 */
static void biz_post(ui_biz_type_t type, int32_t value, int32_t aux)
{
    ui_event_t evt;
    evt.type = UI_EVT_BIZ;
    evt.u.biz.type  = type;
    evt.u.biz.value = value;
    evt.u.biz.aux   = aux;
    ui_msgq_post(&evt);
}

void ui_cam_init(void)
{
    s_mode = UI_MODE_RECORD;
    s_recording = 0;
    s_timelapse_on = 0;
    s_rec_seconds = 0;
    biz_post(UI_BIZ_MODE, s_mode, 0);
    biz_post(UI_BIZ_SDCARD, s_free_minutes, 1);
}

void ui_cam_set_ops(const ui_cam_ops_t *ops)
{
    if (ops) {
        s_ops = *ops;
    } else {
        memset(&s_ops, 0, sizeof(s_ops));
    }
}

ui_work_mode_t ui_cam_mode(void) { return s_mode; }
int ui_cam_is_recording(void)    { return s_recording; }

void ui_cam_set_mode(ui_work_mode_t mode)
{
    if (mode >= UI_MODE_MAX || mode == s_mode) {
        return;
    }
    /* 切模式前若在录制/延时，先停 */
    if (s_recording) {
        ui_cam_toggle_record();
    }
    if (s_timelapse_on) {
        ui_cam_toggle_timelapse(0);
    }
    s_mode = mode;
    biz_post(UI_BIZ_MODE, s_mode, 0);
    UI_LOGI("mode -> %d", s_mode);
}

int ui_cam_toggle_record(void)
{
    if (!s_recording) {
        if (s_ops.start_record && s_ops.start_record() != 0) {
            ui_popup_toast("录像启动失败", 1500);
            return UI_ERR;
        }
        s_recording = 1;
        s_rec_seconds = 0;
        biz_post(UI_BIZ_REC_STATE, 1, 0);
        ui_popup_toast("开始录像", 1000);
    } else {
        if (s_ops.stop_record) {
            s_ops.stop_record();
        }
        s_recording = 0;
        biz_post(UI_BIZ_REC_STATE, 0, 0);
        ui_popup_toast("录像已保存", 1200);
    }
    return UI_OK;
}

int ui_cam_snap(void)
{
    if (s_ops.take_photo && s_ops.take_photo() != 0) {
        ui_popup_toast("拍照失败", 1200);
        return UI_ERR;
    }
    ui_popup_toast("已保存照片", 1000);
    return UI_OK;
}

int ui_cam_toggle_timelapse(int interval_sec)
{
    if (!s_timelapse_on) {
        if (s_ops.start_timelapse && s_ops.start_timelapse(interval_sec) != 0) {
            ui_popup_toast("延时启动失败", 1500);
            return UI_ERR;
        }
        s_timelapse_on = 1;
        biz_post(UI_BIZ_REC_STATE, 1, 0);
        ui_popup_toast("延时摄影中", 1000);
    } else {
        if (s_ops.stop_timelapse) {
            s_ops.stop_timelapse();
        }
        s_timelapse_on = 0;
        biz_post(UI_BIZ_REC_STATE, 0, 0);
        ui_popup_toast("延时已保存", 1200);
    }
    return UI_OK;
}

void ui_cam_tick_1hz(void)
{
    if (s_recording || s_timelapse_on) {
        s_rec_seconds++;
        biz_post(UI_BIZ_REC_STATE, 1, s_rec_seconds);

        /* 每分钟递减一次剩余容量（桩逻辑；真机用 get_free_minutes） */
        if (s_rec_seconds % 60 == 0) {
            if (s_ops.get_free_minutes) {
                s_free_minutes = s_ops.get_free_minutes();
            } else if (s_free_minutes > 0) {
                s_free_minutes--;
            }
            biz_post(UI_BIZ_SDCARD, s_free_minutes, s_free_minutes > 0 ? 1 : 2);
            if (s_free_minutes == 0) {
                ui_popup_toast("存储已满", 1500);
                if (s_recording) ui_cam_toggle_record();
                if (s_timelapse_on) ui_cam_toggle_timelapse(0);
            }
        }
    }
}
