/**
 * @file    ui_setting.h
 * @brief   拇指相机参数设置页：数据模型 + 注册接口。
 *
 * @details
 *  设置项与业务解耦：页面只负责展示/编辑 ui_settings_t，并通过可注入的
 *  ui_settings_ops_t 把"持久化/生效"动作交给真机实现（样机用桩）。
 *  设置项严格对齐拇指相机：分辨率、循环录像、WDR、翻转、红外、音频、
 *  时间水印、格式化、恢复出厂、WiFi 配网。
 */
#ifndef __UI_SETTING_H__
#define __UI_SETTING_H__

#include "ui_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @brief 录像分辨率档位（主码流）。 */
typedef enum {
    UI_RES_2304x1296 = 0,   /**< 默认，对应主码流 */
    UI_RES_1920x1080,
    UI_RES_1280x720,
    UI_RES_MAX,
} ui_resolution_t;

/** @brief 红外夜视模式。 */
typedef enum {
    UI_IR_AUTO = 0,
    UI_IR_ON,
    UI_IR_OFF,
    UI_IR_MAX,
} ui_ir_mode_t;

/** @brief 画面翻转。 */
typedef enum {
    UI_FLIP_NONE = 0,
    UI_FLIP_V,      /**< 上下翻转 */
    UI_FLIP_H,      /**< 左右镜像 */
    UI_FLIP_HV,     /**< 旋转180 */
    UI_FLIP_MAX,
} ui_flip_t;

/** @brief 设置数据模型。 */
typedef struct {
    ui_resolution_t resolution;     /**< 分辨率 */
    uint8_t         loop_record;    /**< 循环录像 0/1 */
    uint8_t         wdr;            /**< 宽动态 0/1 */
    ui_flip_t       flip;          /**< 翻转 */
    ui_ir_mode_t    ir;            /**< 红外 */
    uint8_t         audio;         /**< 录音 0/1 */
    uint8_t         watermark;     /**< 时间水印 0/1 */
    uint8_t         wifi_on;       /**< WiFi 开关 0/1 */
    int             language;      /**< UI 语言（ui_lang_id_t，随 save 持久化；choice 项用 int* 访问，须为 int） */
} ui_settings_t;

/** @brief 设置动作钩子（真机注入；样机用桩）。 */
typedef struct {
    int (*load)(ui_settings_t *out);            /**< 读取持久化设置 */
    int (*save)(const ui_settings_t *in);       /**< 保存设置 */
    int (*apply)(const ui_settings_t *in);      /**< 使设置生效（下发 ISP/编码等） */
    int (*format_storage)(void);                /**< 格式化存储 */
    int (*factory_reset)(void);                 /**< 恢复出厂 */
    int (*wifi_config_start)(void);             /**< 进入 WiFi 配网 */
} ui_settings_ops_t;

/** @brief 注入设置动作实现。 */
void ui_setting_set_ops(const ui_settings_ops_t *ops);

/** @brief 注册设置页。 */
void ui_page_setting_register(void);

#ifdef __cplusplus
}
#endif

#endif /* __UI_SETTING_H__ */
