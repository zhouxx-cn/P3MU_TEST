/**
 * @file    ui_page.h
 * @brief   页面栈管理 + 页面统一接口。
 *
 * @details
 *  每个业务页面（预览/菜单/设置/回放）实现一个 ui_page_t 接口对象。
 *  页面栈负责 push/pop/replace，并在切换时调用页面的 show/hide，
 *  把事件分发给栈顶页面。页面只管自己的 LVGL 对象树，不关心导航细节。
 *
 *  约定：所有页面创建的根对象应挂在 lv_scr_act() 的"内容层"，视频在底层、
 *  状态栏/弹窗在顶层，三者互不干扰（见 ui_main 的分层说明）。
 */
#ifndef __UI_PAGE_H__
#define __UI_PAGE_H__

#include "ui_common.h"
#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @brief 页面标识。 */
typedef enum {
    UI_PAGE_PREVIEW = 0,   /**< 预览页（录像/拍照/延时的实时取景） */
    UI_PAGE_MENU,          /**< 模式主菜单 */
    UI_PAGE_SETTING,       /**< 参数设置 */
    UI_PAGE_PLAYBACK,      /**< 文件回放 */
    UI_PAGE_MAX,
} ui_page_id_t;

struct ui_page;
typedef struct ui_page ui_page_t;

/**
 * @brief 页面接口。各页面在自己的 .c 中实例化并通过 ui_page_register 注册。
 */
struct ui_page {
    ui_page_id_t id;
    const char  *name;

    /** 创建页面 LVGL 对象（首次进入时惰性调用）；返回页面根对象。parent 为内容层。 */
    lv_obj_t *(*create)(ui_page_t *self, lv_obj_t *parent);
    /** 页面显示（入栈/恢复到栈顶）：刷新数据、获取输入焦点组。 */
    void (*on_show)(ui_page_t *self);
    /** 页面隐藏（被覆盖或出栈）：暂停动画/定时器。 */
    void (*on_hide)(ui_page_t *self);
    /** 处理统一事件（按键/点击/业务）。返回 1 表示已消费，0 表示未处理。 */
    int  (*on_event)(ui_page_t *self, const ui_event_t *evt);
    /** 销毁页面对象（可选，样机通常常驻不销毁）。 */
    void (*destroy)(ui_page_t *self);

    /* 运行时字段（由 page 框架维护，页面实现勿手改） */
    lv_obj_t    *root;     /**< 页面根对象 */
    lv_group_t  *group;    /**< 该页输入焦点组 */
    void        *priv;     /**< 页面私有数据 */
};

/**
 * @brief 初始化页面栈框架。
 * @param content_layer 内容层父对象（页面根对象挂到这里）
 * @return UI_OK / UI_ERR
 */
int ui_page_stack_init(lv_obj_t *content_layer);

/**
 * @brief 注册一个页面实现。
 * @param page 页面接口对象（静态生命周期）
 * @return UI_OK / UI_ERR
 */
int ui_page_register(ui_page_t *page);

/**
 * @brief 入栈并显示指定页面。
 * @param id 页面标识
 * @return UI_OK / UI_ERR
 */
int ui_page_push(ui_page_id_t id);

/**
 * @brief 出栈当前页面，回到上一页；栈底页面不出栈。
 * @return UI_OK / UI_ERR
 */
int ui_page_pop(void);

/**
 * @brief 用指定页面替换栈顶（不增加栈深，用于模式切换）。
 * @param id 页面标识
 * @return UI_OK / UI_ERR
 */
int ui_page_replace(ui_page_id_t id);

/**
 * @brief 获取当前栈顶页面。
 * @return 栈顶页面指针，空栈返回 NULL
 */
ui_page_t *ui_page_top(void);

/**
 * @brief 把事件分发给栈顶页面处理。
 * @param evt 事件
 * @return 1 已被页面消费，0 未处理
 */
int ui_page_dispatch(const ui_event_t *evt);

#ifdef __cplusplus
}
#endif

#endif /* __UI_PAGE_H__ */
