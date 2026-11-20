/**
 * @file    ui_lang.h
 * @brief   拇指相机 UI 多语言模块：语言枚举 + 文案 ID + 翻译接口。
 *
 * @details
 *  纯静态表驱动，无动态内存。所有用户可见文案一律通过 ui_lang_tr() 取得，
 *  页面在创建/刷新时调用，不缓存指针长期持有（切语言后指针会变）。
 *
 *  支持语言（UI_LANG_MAX = 11）：英语 / 简体中文 / 繁体中文 / 日语 / 俄语 /
 *  泰语 / 越南语 / 德语 / 法语 / 意大利语 / 葡萄牙语。
 *
 *  扩展新语言步骤：
 *    1. ui_lang.h   增加 UI_LANG_XXX 枚举（插在 UI_LANG_MAX 之前）；
 *    2. ui_lang.c   增加 ui_str_xxx[] 表（顺序必须与 ui_str_id_t 完全一致，
 *                   缺失条目填 NULL，运行时自动回退英语）；
 *    3. 运行 tools/font_gen.py 重新生成 ui_font 下字库（脚本自动收集
 *                   ui_lang.c 所有表格字符），重新编译。
 *
 *  已知限制：LVGL 8.3 无复杂文本整形（shaping），泰语元音/声调组合符、
 *  阿拉伯语等无法精确定位，渲染为"基字符 + 组合符近似叠加"，可读但非出版级；
 *  如需出版级泰语，升级 LVGL 9.x（内置 HarfBuzz 简化整形）或走位图整行方案。
 */
#ifndef __UI_LANG_H__
#define __UI_LANG_H__

#include "ui_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ==================== 语言 ==================== */
typedef enum {
    UI_LANG_EN = 0,     /**< English */
    UI_LANG_ZH_CN,      /**< 简体中文（默认） */
    UI_LANG_ZH_TW,      /**< 繁體中文 */
    UI_LANG_JA,         /**< 日本語 */
    UI_LANG_RU,         /**< Русский */
    UI_LANG_TH,         /**< ไทย */
    UI_LANG_VI,         /**< Tiếng Việt */
    UI_LANG_DE,         /**< Deutsch */
    UI_LANG_FR,         /**< Français */
    UI_LANG_IT,         /**< Italiano */
    UI_LANG_PT,         /**< Português */
    UI_LANG_MAX,
} ui_lang_id_t;

/* ==================== 文案 ID ==================== */
typedef enum {
    UI_STR_REC = 0,           /**< 录像 */
    UI_STR_PHOTO,             /**< 拍照 */
    UI_STR_TIMELAPSE,         /**< 延时摄影 */
    UI_STR_PLAYBACK,          /**< 回放 */
    UI_STR_SETTINGS,          /**< 设置 */
    UI_STR_MODE_MENU,         /**< 模式菜单 */
    UI_STR_RESOLUTION,        /**< 分辨率 */
    UI_STR_LOOP_RECORD,       /**< 循环录像 */
    UI_STR_WDR,               /**< WDR宽动态 */
    UI_STR_FLIP,              /**< 画面翻转 */
    UI_STR_IR_NIGHT,          /**< 红外夜视 */
    UI_STR_MIC,               /**< 录音 */
    UI_STR_WATERMARK,         /**< 时间水印 */
    UI_STR_WIFI,              /**< WiFi */
    UI_STR_WIFI_SETUP,        /**< WiFi配网 */
    UI_STR_FORMAT,            /**< 格式化 */
    UI_STR_FACTORY_RESET,     /**< 恢复出厂 */
    UI_STR_LANGUAGE,          /**< 语言 */
    UI_STR_ON,                /**< 开 */
    UI_STR_OFF,               /**< 关 */
    UI_STR_DISABLED,          /**< 关闭（翻转/红外选项用） */
    UI_STR_FLIP_V,            /**< 上下翻 */
    UI_STR_FLIP_H,            /**< 左右镜像 */
    UI_STR_FLIP_180,          /**< 旋转180 */
    UI_STR_IR_AUTO,           /**< 自动 */
    UI_STR_IR_ALWAYS,         /**< 常开 */
    UI_STR_FORMATTING,        /**< 格式化中... */
    UI_STR_FORMAT_DONE,       /**< 格式化完成 */
    UI_STR_FACTORY_DONE,      /**< 已恢复出厂设置 */
    UI_STR_CONFIRM_FORMAT,    /**< 确定格式化存储?\n所有文件将被删除 */
    UI_STR_CONFIRM_FACTORY,   /**< 确定恢复出厂设置? */
    UI_STR_SCAN_QR,           /**< 请用APP扫码配网 */
    UI_STR_DELETED,           /**< 已删除 */
    UI_STR_PLAYING,           /**< 播放中... */
    UI_STR_NO_FILES,          /**< 暂无文件 */
    UI_STR_REC_FAIL,          /**< 录像启动失败 */
    UI_STR_REC_START,         /**< 开始录像 */
    UI_STR_REC_SAVED,         /**< 录像已保存 */
    UI_STR_PHOTO_FAIL,        /**< 拍照失败 */
    UI_STR_PHOTO_SAVED,       /**< 已保存照片 */
    UI_STR_TL_FAIL,           /**< 延时启动失败 */
    UI_STR_TL_RUNNING,        /**< 延时摄影中 */
    UI_STR_TL_SAVED,          /**< 延时已保存 */
    UI_STR_SD_FULL,           /**< 存储已满 */
    UI_STR_CONFIRM_TITLE,     /**< 确认操作? */
    UI_STR_CANCEL,            /**< 取消 */
    UI_STR_CONFIRM_OK,        /**< 确认 */
    UI_STR_PROCESSING,        /**< 处理中... */
    UI_STR_ACT_TIMELAPSE,     /**< 延时（预览页提示短词） */
    UI_STR_HINT_SWITCH,       /**< 切模式 */
    UI_STR_HINT_MENU,         /**< 菜单 */
    UI_STR_MAX,
} ui_str_id_t;

/**
 * @brief 取当前语言的文案（表缺失或条目为 NULL 时回退英语，永不返回 NULL）。
 * @param id 文案 ID
 * @return UTF-8 字符串（静态存储，勿修改/勿释放）
 */
const char *ui_lang_tr(ui_str_id_t id);

/**
 * @brief 切换语言并重绘全部可见文案（经消息队列投递，线程安全可任意线程调用）。
 * @param lang 目标语言
 */
void ui_lang_set(ui_lang_id_t lang);

/**
 * @brief 当前语言。
 */
ui_lang_id_t ui_lang_get(void);

/**
 * @brief 语言自称（母语写法，设置页语言选项展示用）。
 * @param lang 语言 ID
 * @return 静态字符串，勿修改
 */
const char *ui_lang_name(ui_lang_id_t lang);

#ifdef __cplusplus
}
#endif

#endif /* __UI_LANG_H__ */
