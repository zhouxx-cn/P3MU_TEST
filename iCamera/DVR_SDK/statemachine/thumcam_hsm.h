/**
 * @file    thumcam_hsm.h
 * @brief   拇指相机整机层次化状态机（HFSM）公共接口
 *
 * 架构分层原则：
 *   1. 按键 / LVGL GUI / APP 远程协议 / 硬件检测线程 只负责产生事件，
 *      通过 ThumCam_HSM_PostEvent() 投递消息；
 *   2. 状态机变更状态后，通过 ThumCam_HSM_RegisterStateChangeCb() 注册的
 *      回调通知 GUI 刷新界面；
 *   3. 状态机不直接调用 UI 和媒体驱动。硬件/媒体动作在 Enter/Exit 回调里
 *      只写占位注释（TODO(业务层)），由上层业务实现。
 *
 * 线程模型：
 *   - 多个生产者线程调用 ThumCam_HSM_PostEvent()（内部互斥锁保护）；
 *   - 唯一的 HFSM 派发线程从消息队列取事件，单线程调用 hsm_dispatch()，
 *     保证 hsm 引擎的 current/last 状态无并发竞争；
 *   - ThumCam_HSM_GetStateName() / ThumCam_HSM_IsInState() 可在任意线程调用，
 *     读取的是派发线程发布的快照。
 */

#ifndef __THUMCAM_HSM_H__
#define __THUMCAM_HSM_H__

#include <stdint.h>
#include <stdbool.h>
#include <pthread.h>
#include "hsm.h"

#ifdef __cplusplus
extern "C" {
#endif

/* 模块日志宏：默认输出到 stderr，业务侧可在编译选项中覆盖以接入 DVR_SDK 日志。
 * 例如：-DTHUMCAM_HSM_LOG(fmt,...) do{ my_log(fmt, ##__VA_ARGS__); }while(0) */
#ifndef THUMCAM_HSM_LOG
#define THUMCAM_HSM_LOG(fmt, ...) fprintf(stderr, "[HSM] " fmt "\n", ##__VA_ARGS__)
#endif

/* ==================================================================================
 * 消息 ID 定义（用户自定义信号，必须大于 HSM_DEF_SIG）
 * ================================================================================== */
typedef enum
{
    /* ---- 物理按键（GPIO 按键检测线程产生） ---- */
    MSG_POWER_KEY_SHORT = HSM_DEF_SIG + 1,  /* 电源键短按：开机/休眠唤醒 */
    MSG_POWER_KEY_LONG,                     /* 电源键长按：关机 */
    MSG_REC_KEY,                            /* 录像键：预览态=开始录像；录像态=停止录像 */
    MSG_PHOTO_KEY,                          /* 拍照键：预览态=单拍；倒计时拍照中=取消 */
    MSG_PHOTO_DELAY,                        /* 触发倒计时拍照（GUI/APP/长按拍照键产生） */
    MSG_PLAYBACK_KEY,                       /* 回放键 */
    MSG_SETTING_KEY,                        /* 设置键 */
    MSG_MODE_KEY,                           /* 模式键：录像家族内循环切换 普通/延时/慢动作 */

    /* ---- 触摸屏（LVGL GUI 事件回调产生） ---- */
    MSG_TOUCH_NEXT,                         /* 回放：下一个（循环翻页） */
    MSG_TOUCH_PREV,                         /* 回放：上一个（循环翻页） */
    MSG_TOUCH_DELETE,                       /* 回放：删除文件 */
    MSG_TOUCH_BACK,                         /* 触摸返回 */
    MSG_USB_MODE_SELECT,                    /* USB 模式选择（param = thumcam_usb_mode_e 值） */

    /* ---- APP 远程指令（WiFi AP 模式协议线程产生） ---- */
    MSG_REMOTE_REC_START,                   /* APP 远程开始录像 */
    MSG_REMOTE_REC_STOP,                    /* APP 远程停止录像 */
    MSG_REMOTE_SNAP,                        /* APP 远程抓拍 */
    MSG_REMOTE_PREVIEW,                     /* APP 远程请求实时预览 */

    /* ---- 系统定时器（HFSM 内部定时器产生，见 ThumCam_HSM_SetTimeouts） ---- */
    MSG_SAVER_TIMEOUT,                      /* 屏保超时：任意业务状态可进屏保（只关背光） */
    MSG_SLEEP_TIMEOUT,                      /* 休眠超时：仅预览态生效；录像家族屏蔽 */
    MSG_AUTO_POWEROFF,                      /* 自动关机超时 */
    MSG_DELAY_PHOTO_TICK,                   /* 倒计时拍照 tick（仅 PHOTO_DELAY 态收到） */

    /* ---- 硬件中断 / 检测线程 ---- */
    MSG_USB_INSERT,                         /* USB 插入 */
    MSG_USB_REMOVE,                         /* USB 拔出 */
    MSG_SD_REMOVE,                          /* SD 卡拔出 */
    MSG_BAT_LOW,                            /* 低电量告警 */
    MSG_OVER_TEMP,                          /* 过热保护 */

    /* ---- 内部业务完成通知（媒体线程产生） ---- */
    MSG_PHOTO_DONE,                         /* 抓拍完成（param 无） */
    MSG_REC_SEGMENT_DONE,                   /* 分段录像：当前段写完 */
    MSG_REC_STOP,                           /* 录像停止指令（业务层备用入口，等价录像键） */
    MSG_PRODUCT_TEST_EXIT,                  /* 产测退出指令 */
} thumcam_msg_e;

/* USB 工作模式（MSG_USB_MODE_SELECT 的 param 使用） */
typedef enum
{
    THUMCAM_USB_CHG_ONLY = 0,               /* 仅充电：相机业务可用，禁止录像拍照 */
    THUMCAM_USB_UVC,                        /* UVC 摄像头 */
    THUMCAM_USB_MASS_STORAGE,               /* U 盘大容量存储 */
} thumcam_usb_mode_e;

/* ==================================================================================
 * 状态对象声明（实现在 thumcam_states.c，层级见同目录 README.md）
 * ================================================================================== */
extern const hsm_t top_state;               /* hsm 框架内置根状态，勿修改 */

/* SYS_ROOT 直接子状态 */
extern hsm_t power_off_state;               /* 关机 */
extern hsm_t product_test_state;            /* 产测模式（当前仅状态壳） */
extern hsm_t working_state;                 /* 整机工作超级父状态 */

/* SYS_WORKING 子状态 */
extern hsm_t idle_preview_state;            /* 待机预览（常态） */
extern hsm_t screen_saver_state;            /* 屏保（只关背光，业务不停） */
extern hsm_t sys_sleep_state;               /* 休眠（关 ISP/VENC/音频） */
extern hsm_t rec_group_state;               /* 录像父状态 */
extern hsm_t photo_group_state;             /* 拍照父状态 */
extern hsm_t playback_state;                /* 文件回放 */
extern hsm_t setting_menu_state;            /* 设置菜单 */
extern hsm_t usb_group_state;               /* USB 插入父状态 */

/* REC_GROUP 子状态 */
extern hsm_t rec_normal_state;              /* 普通录像（预录+分段+循环） */
extern hsm_t rec_timelapse_state;           /* 延时缩时录像 */
extern hsm_t rec_slow_motion_state;         /* 慢动作录像 */

/* PHOTO_GROUP 子状态 */
extern hsm_t photo_single_state;            /* 普通单拍 */
extern hsm_t photo_delay_state;             /* 倒计时定时拍照 */

/* USB_GROUP 子状态（互斥三选一） */
extern hsm_t usb_chg_only_state;            /* 仅充电 */
extern hsm_t usb_uvc_state;                 /* UVC */
extern hsm_t usb_mass_storage_state;        /* U 盘 */

/* ==================================================================================
 * 公共 API
 * ================================================================================== */

/* 状态切换通知回调：状态机每次跳转后调用（在派发线程上下文执行，
 * 回调里不要做重活/直接刷 GUI，GUI 框架一般要求在自己的线程刷新，
 * 建议回调里只 post 一个 LVGL 刷新任务）。
 * @from/@to 为状态名字符串（来自 hsm_t.name）。 */
typedef void (*thumcam_state_change_cb)(const char *from, const char *to);

/**
 * @brief  启动状态机（创建派发线程 + 初始化状态机到初始状态）
 * @param  product_test 非 0 表示上电检测到产测标记，直接进入产测状态
 * @return 0 成功，-1 失败（重复启动/线程创建失败）
 */
int ThumCam_HSM_Start(int product_test);

/**
 * @brief  停止状态机（退出派发线程；不会主动执行当前状态的 EXIT 收尾，
 *         关机收尾应由业务通过事件驱动完成后再调用本接口）
 */
void ThumCam_HSM_Stop(void);

/**
 * @brief  投递事件到状态机（线程安全，任意线程可调用）
 * @param  sig   thumcam_msg_e 中的消息 ID
 * @param  param 事件参数指针。注意生命周期：param 指向的内存在事件被处理前
 *         必须保持有效；传值场景请转换为指针，如 (void *)(intptr_t)mode。
 *         队列满时丢弃事件并返回错误。
 * @return 0 成功，-1 队列满或未启动
 */
int ThumCam_HSM_PostEvent(int sig, void *param);

/**
 * @brief  用户活动通知（重置屏保/休眠/自动关机计时器）。
 *         按键/触摸/远程指令类事件在投递时会自动调用；GUI 若有其他
 *         "算作用户操作"的行为（如滑动动画持续），可显式调用本接口。
 */
void ThumCam_HSM_NotifyActivity(void);

/**
 * @brief  配置系统定时器超时时间（毫秒），0 表示禁用对应定时器
 * @param  saver_ms   屏保超时（默认 30s）
 * @param  sleep_ms   休眠超时（默认 60s，仅预览态生效）
 * @param  autooff_ms 自动关机超时（默认 0 禁用）
 */
void ThumCam_HSM_SetTimeouts(int saver_ms, int sleep_ms, int autooff_ms);

/**
 * @brief  设置倒计时拍照秒数（PHOTO_DELAY 态进入时读取），默认 3 秒
 */
void ThumCam_HSM_SetDelayPhotoSeconds(int seconds);

/**
 * @brief  获取倒计时拍照秒数（线程安全）
 */
int ThumCam_HSM_GetDelayPhotoSeconds(void);

/**
 * @brief  注册状态切换通知回调（GUI 刷新入口）；传 NULL 注销
 */
void ThumCam_HSM_RegisterStateChangeCb(thumcam_state_change_cb cb);

/**
 * @brief  获取当前状态名（线程安全，读快照；未启动返回 "none"）
 */
const char *ThumCam_HSM_GetStateName(void);

/**
 * @brief  判断当前是否处于某状态或其子状态（线程安全）
 * @note   基于 hsm_is_child 的严格父子判断，state 本身相等也算 true
 * @example ThumCam_HSM_IsInState(&rec_group_state) —— 判断"是否正在录像家族"
 */
bool ThumCam_HSM_IsInState(const hsm_t *state);

#ifdef __cplusplus
}
#endif
#endif /* __THUMCAM_HSM_H__ */
