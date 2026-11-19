/**
 * @file    ui_menu.h
 * @brief   拇指相机模式主菜单对外声明。
 */
#ifndef __UI_MENU_H__
#define __UI_MENU_H__

#include "ui_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @brief 注册预览页（录像/拍照/延时实时取景）。 */
void ui_page_preview_register(void);

/** @brief 注册模式主菜单页。 */
void ui_page_menu_register(void);

/** @brief 注册文件回放页。 */
void ui_page_playback_register(void);

#ifdef __cplusplus
}
#endif

#endif /* __UI_MENU_H__ */
