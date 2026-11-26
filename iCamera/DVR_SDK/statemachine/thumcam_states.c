/**
 * @file    thumcam_states.c
 * @brief   拇指相机全部状态实现（层级结构见同目录 README.md）
 *
 * 约定：
 *   1. 本文件只做状态流转与 Enter/Exit 回调，不调用任何 UI / 媒体 / 硬件驱动；
 *      硬件与媒体动作全部以 TODO(业务层) 占位注释标注，由上层实现；
 *   2. 事件返回值：
 *      - HSM_HANDLED()   本状态已处理，事件不再上浮；
 *      - HSM_UNHANDLED() 本状态不处理，事件上浮给父状态（层次屏蔽的关键机制）；
 *   3. 增加新状态的方法：
 *      a. thumcam_hsm.h 中 extern 声明状态对象；
 *      b. 本文件定义 hsm_t 并实现状态函数；
 *      c. 挂到对应父状态（parent 指针），在父状态的 HSM_INIT_SIG 或其他状态
 *         的转换逻辑中 HSM_TRAN 进入；
 *      d. 新增消息 ID 加到 thumcam_hsm.h 的 thumcam_msg_e。
 *
 * 状态层级：
 *   top_state
 *   ├── power_off_state            关机
 *   ├── product_test_state         产测（壳）
 *   └── working_state              整机工作超级父状态
 *       ├── idle_preview_state     待机预览（ISP 常开，切录像快起）
 *       ├── screen_saver_state     屏保（只关背光，业务不停）
 *       ├── sys_sleep_state        休眠（关 ISP/VENC/音频）
 *       ├── rec_group_state        录像父状态（屏蔽休眠，允许屏保）
 *       │   ├── rec_normal_state / rec_timelapse_state / rec_slow_motion_state
 *       ├── photo_group_state      拍照父状态
 *       │   ├── photo_single_state / photo_delay_state
 *       ├── playback_state         文件回放
 *       ├── setting_menu_state     设置菜单
 *       └── usb_group_state        USB 插入父状态（禁录像拍照）
 *           ├── usb_chg_only_state / usb_uvc_state / usb_mass_storage_state
 */

#include <stdint.h>
#include "thumcam_hsm.h"
#include "thumcam_hsm_internal.h"

/* ==================================================================================
 * 辅助宏：硬件/媒体占位日志，统一前缀便于全局检索 TODO(业务层)
 * ================================================================================== */
#define HWSLOT(fmt, ...) THUMCAM_HSM_LOG("[业务层占位] " fmt, ##__VA_ARGS__)

/* ==================================================================================
 * SYS_ROOT 直接子状态
 * ================================================================================== */

/* ---------------------------------- 关机状态 ---------------------------------- */
static HSM_RET_T power_off_state_func(hsm_obj_t *me, hsm_event_t *e)
{
    switch (e->sig)
    {
        case HSM_ENTRY_SIG:
            /* TODO(业务层): 全部媒体硬件下电（ISP/VENC/AUDIO/LCD）；DDR 低功耗 */
            HWSLOT("POWER_OFF enter: system power off");
            return HSM_HANDLED();

        case HSM_EXIT_SIG:
            HWSLOT("POWER_OFF exit: booting");
            return HSM_HANDLED();

        case MSG_POWER_KEY_SHORT:
            /* 开机：SYS_WORKING 的 HSM_INIT_SIG 会继续进入 IDLE_PREVIEW */
            return HSM_TRAN(&working_state);

        case MSG_USB_INSERT:
            /* 关机状态插 USB：仅充电，不自动开机 */
            HWSLOT("POWER_OFF: usb insert, charge only");
            return HSM_HANDLED();

        default:
            return HSM_UNHANDLED();
    }
}

hsm_t power_off_state = {
    .name = "POWER_OFF",
    .parent = &top_state,
    .state_func = HSM_STATE_FUNC(power_off_state_func),
};

/* ---------------------------------- 产测模式 ---------------------------------- */
static HSM_RET_T product_test_state_func(hsm_obj_t *me, hsm_event_t *e)
{
    (void)me;
    switch (e->sig)
    {
        case HSM_ENTRY_SIG:
            /* TODO(业务层): 关预览；加载产测私有 WiFi；启动产测指令服务。
             * 当前阶段仅搭建状态壳，具体产测业务后续实现。 */
            HWSLOT("PRODUCT_TEST enter");
            return HSM_HANDLED();

        case HSM_EXIT_SIG:
            HWSLOT("PRODUCT_TEST exit");
            return HSM_HANDLED();

        case MSG_PRODUCT_TEST_EXIT:
            /* 产测标记消失 / 收到退出指令 → 关机 */
            return HSM_TRAN(&power_off_state);

        default:
            return HSM_UNHANDLED();
    }
}

hsm_t product_test_state = {
    .name = "PRODUCT_TEST",
    .parent = &top_state,
    .state_func = HSM_STATE_FUNC(product_test_state_func),
};

/* ==================================================================================
 * SYS_WORKING 超级父状态：SD 拔出 / 低电 / 过热 / 关机 全局事件统一处理
 * ================================================================================== */
static HSM_RET_T working_state_func(hsm_obj_t *me, hsm_event_t *e)
{
    switch (e->sig)
    {
        case HSM_ENTRY_SIG:
            /* TODO(业务层): 初始化电池管理；启动基础外设（不启动 ISP） */
            HWSLOT("WORKING enter");
            return HSM_HANDLED();

        case HSM_INIT_SIG:
            /* 工作状态下的默认子状态：待机预览 */
            return HSM_TRAN(&idle_preview_state);

        case HSM_EXIT_SIG:
            /* TODO(业务层): 停止全部定时器/外设 */
            HWSLOT("WORKING exit: shutdown");
            return HSM_HANDLED();

        /* ---- 以下为全局事件（子状态不处理时上浮到这里） ---- */

        case MSG_SD_REMOVE:
            /* SD 拔出：录像家族自身会先收到（其父态未拦截则到此），
             * 统一停录像/弹窗/回预览；其他状态弹窗提示后停留原状态 */
            HWSLOT("WORKING: sd removed, popup warn");
            return HSM_TRAN(&idle_preview_state);

        case MSG_USB_INSERT:
            /* USB 插入：停止当前业务（录像则停录）后切 USB_GROUP。
             * 本兜底让"非预览态插 USB"在任意子状态下都生效：
             * 跳转时源业务状态链的 EXIT 会被执行，业务层在 EXIT 里停录/停拍。 */
            HWSLOT("WORKING: usb insert, stop current business");
            return HSM_TRAN(&usb_group_state);

        case MSG_BAT_LOW:
            /* TODO(业务层): 弹低电提示；业务层监测到临界电量再投关机事件 */
            HWSLOT("WORKING: battery low, popup warn");
            return HSM_HANDLED();

        case MSG_OVER_TEMP:
            /* TODO(业务层): 停止编码并弹过热提示；严重过热由业务投递关机 */
            HWSLOT("WORKING: over temperature, stop encode");
            return HSM_HANDLED();

        case MSG_POWER_KEY_LONG:
            /* 长按电源：关机 */
            return HSM_TRAN(&power_off_state);

        case MSG_AUTO_POWEROFF:
            /* 自动关机 */
            return HSM_TRAN(&power_off_state);

        case MSG_SLEEP_TIMEOUT:
            /* 休眠超时兜底：能上浮到这里说明子状态（录像家族等）拒绝休眠，
             * 直接丢弃 */
            return HSM_HANDLED();

        case MSG_SAVER_TIMEOUT:
            /* 屏保超时兜底：能上浮到这里说明所有子状态都拒绝屏保，丢弃 */
            return HSM_HANDLED();

        default:
            return HSM_UNHANDLED();
    }
}

hsm_t working_state = {
    .name = "WORKING",
    .parent = &top_state,
    .state_func = HSM_STATE_FUNC(working_state_func),
};

/* ==================================================================================
 * IDLE_PREVIEW 待机预览（常态）
 * ================================================================================== */
static HSM_RET_T idle_preview_state_func(hsm_obj_t *me, hsm_event_t *e)
{
    (void)me;
    switch (e->sig)
    {
        case HSM_ENTRY_SIG:
            /* TODO(业务层):
             * 1. 打开 ISP/VPSS 通路，YUV 直送 LCD；
             * 2. 开启 3A / EIS / 重力传感器；
             * 3. VENC、AAC 保持关闭；
             * 4. 点亮屏幕。
             * 注意：EXIT 不关 ISP，保证切录像快起。 */
            HWSLOT("IDLE_PREVIEW enter");
            return HSM_HANDLED();

        case HSM_EXIT_SIG:
            /* 切录像/拍照等复用 ISP，不关；只有切休眠/关机才真正关 */
            return HSM_HANDLED();

        /* ---- 业务入口 ---- */
        case MSG_REC_KEY:
        case MSG_REMOTE_REC_START:
            return HSM_TRAN(&rec_normal_state);

        case MSG_PHOTO_KEY:
        case MSG_REMOTE_SNAP:
            return HSM_TRAN(&photo_single_state);

        case MSG_PHOTO_DELAY:
            /* 倒计时定时拍照（GUI / APP / 长按拍照键触发） */
            return HSM_TRAN(&photo_delay_state);

        case MSG_PLAYBACK_KEY:
            return HSM_TRAN(&playback_state);

        case MSG_SETTING_KEY:
            return HSM_TRAN(&setting_menu_state);

        /* ---- 超时/硬件 ---- */
        case MSG_SAVER_TIMEOUT:
            /* 屏保用"保留跳转"进入：只关背光，预览业务不中断 */
            return HSM_TRAN_KEEP(&screen_saver_state);

        case MSG_SLEEP_TIMEOUT:
            /* 预览态允许休眠 */
            return HSM_TRAN(&sys_sleep_state);

        case MSG_USB_INSERT:
            return HSM_TRAN(&usb_group_state);

        default:
            return HSM_UNHANDLED();
    }
}

hsm_t idle_preview_state = {
    .name = "IDLE_PREVIEW",
    .parent = &working_state,
    .state_func = HSM_STATE_FUNC(idle_preview_state_func),
};

/* ==================================================================================
 * SCREEN_SAVER 屏保：只关背光，ISP/VENC 不动；业务（录像）无缝续跑
 *
 * 屏保是"叠加态"：进入时用 HSM_TRAN_KEEP，引擎不会退出源业务状态链，
 * 因此录像态进屏保时 REC_NORMAL/REC_GROUP 的 EXIT 不会被执行 —— 录像不中断。
 * 唤醒时用 HSM_RESUME，只发屏保的 EXIT，不重发业务链的 ENTRY —— 录像不重启。
 * 被保留的业务状态保存在引擎的 me->saved 里。
 * ================================================================================== */
static HSM_RET_T screen_saver_state_func(hsm_obj_t *me, hsm_event_t *e)
{
    switch (e->sig)
    {
        case HSM_ENTRY_SIG:
            /* me->saved 由 HSM_TRAN_KEEP 保存，即进入屏保前的业务状态 */
            /* TODO(业务层): 关闭 LCD 背光；ISP/VENC/AAC 保持现状（业务未退出） */
            HWSLOT("SCREEN_SAVER enter, keep=%s",
                   me->saved ? me->saved->name : "NULL");
            return HSM_HANDLED();

        case HSM_EXIT_SIG:
            /* TODO(业务层): 打开 LCD 背光 */
            HWSLOT("SCREEN_SAVER exit");
            return HSM_HANDLED();

        /* ---- 唤醒类事件：恢复进入屏保前的业务状态（业务链不重启） ---- */
        case MSG_POWER_KEY_SHORT:
        case MSG_REC_KEY:
        case MSG_PHOTO_KEY:
        case MSG_PLAYBACK_KEY:
        case MSG_SETTING_KEY:
        case MSG_MODE_KEY:
        case MSG_TOUCH_NEXT:
        case MSG_TOUCH_PREV:
        case MSG_TOUCH_BACK:
        case MSG_REMOTE_PREVIEW:
            if (me->saved == NULL)
            {
                /* 兜底：理论上不会发生（屏保只能由 HSM_TRAN_KEEP 进入） */
                return HSM_TRAN(&idle_preview_state);
            }
            return HSM_RESUME();

        case MSG_SAVER_TIMEOUT:
            /* 已在屏保，吞掉 */
            return HSM_HANDLED();

        case MSG_SLEEP_TIMEOUT:
            /* 屏保中休眠超时：只有"进入前是预览态"才允许休眠；
             * 录像等业务态屏保中不允许休眠（录像继续） */
            if (me->saved == &idle_preview_state)
            {
                return HSM_TRAN(&sys_sleep_state);
            }
            return HSM_HANDLED();

        default:
            /* 其余业务事件（分片结束、停止录像等）由引擎上浮给被保留的业务链 */
            return HSM_UNHANDLED();
    }
}

hsm_t screen_saver_state = {
    .name = "SCREEN_SAVER",
    .parent = &working_state,
    .state_func = HSM_STATE_FUNC(screen_saver_state_func),
};

/* ==================================================================================
 * SYS_SLEEP 休眠：关 ISP/VENC/音频，任意按键唤醒回预览
 * ================================================================================== */
static HSM_RET_T sys_sleep_state_func(hsm_obj_t *me, hsm_event_t *e)
{
    (void)me;
    switch (e->sig)
    {
        case HSM_ENTRY_SIG:
            /* TODO(业务层): LCD 熄屏；关闭 ISP/VENC/AAC；
             * 维持电池检测、按键检测 */
            HWSLOT("SYS_SLEEP enter");
            return HSM_HANDLED();

        case HSM_EXIT_SIG:
            /* TODO(业务层): 唤醒由 IDLE_PREVIEW 的 ENTRY 重新开 ISP 通路 */
            return HSM_HANDLED();

        case MSG_POWER_KEY_SHORT:
        case MSG_REC_KEY:
        case MSG_PHOTO_KEY:
        case MSG_PLAYBACK_KEY:
        case MSG_SETTING_KEY:
        case MSG_TOUCH_BACK:
        case MSG_REMOTE_PREVIEW:
            /* 任意键唤醒，回待机预览 */
            return HSM_TRAN(&idle_preview_state);

        case MSG_SLEEP_TIMEOUT:
        case MSG_SAVER_TIMEOUT:
            /* 已休眠，吞掉超时事件 */
            return HSM_HANDLED();

        default:
            return HSM_UNHANDLED();
    }
}

hsm_t sys_sleep_state = {
    .name = "SYS_SLEEP",
    .parent = &working_state,
    .state_func = HSM_STATE_FUNC(sys_sleep_state_func),
};

/* ==================================================================================
 * REC_GROUP 录像父状态：屏蔽休眠、允许屏保、停止录像统一入口、模式键循环切换
 * ================================================================================== */
static HSM_RET_T rec_group_state_func(hsm_obj_t *me, hsm_event_t *e)
{
    (void)me;
    switch (e->sig)
    {
        case HSM_ENTRY_SIG:
            /* TODO(业务层): 打开 AAC 音频编码；初始化 MP4 封装上下文 */
            HWSLOT("REC_GROUP enter");
            return HSM_HANDLED();

        case HSM_EXIT_SIG:
            /* TODO(业务层): 停止 AAC；关闭 MP4 句柄；释放媒体资源 */
            HWSLOT("REC_GROUP exit");
            return HSM_HANDLED();

        case MSG_REC_KEY:
        case MSG_REC_STOP:
        case MSG_REMOTE_REC_STOP:
            /* 停止录像统一入口（含 APP 远程停止），回待机预览 */
            return HSM_TRAN(&idle_preview_state);

        case MSG_MODE_KEY:
            /* 模式键：在录像子状态间循环切换 普通 → 延时 → 慢动作 → 普通
             * （兄弟状态互切，框架自动执行 EXIT/ENTRY，业务侧完成重配） */
            if (ThumCam_HSM_IsInState(&rec_normal_state))
            {
                return HSM_TRAN(&rec_timelapse_state);
            }
            else if (ThumCam_HSM_IsInState(&rec_timelapse_state))
            {
                return HSM_TRAN(&rec_slow_motion_state);
            }
            return HSM_TRAN(&rec_normal_state);

        case MSG_SAVER_TIMEOUT:
            /* 录像中允许屏保：用保留跳转进入，录像状态链不被退出，录像不停 */
            return HSM_TRAN_KEEP(&screen_saver_state);

        case MSG_SLEEP_TIMEOUT:
            /* 录像家族拒绝休眠：上浮由 SYS_WORKING 丢弃 */
            return HSM_UNHANDLED();

        case MSG_SD_REMOVE:
            /* SD 拔出：录像中必须停机，交给 SYS_WORKING 统一处理 */
            return HSM_UNHANDLED();

        case MSG_PHOTO_KEY:
        case MSG_REMOTE_SNAP:
            /* 录像中不允许拍照（如需录像抓拍功能后续扩展，由业务实现） */
            HWSLOT("REC_GROUP: photo ignored while recording");
            return HSM_HANDLED();

        default:
            return HSM_UNHANDLED();
    }
}

hsm_t rec_group_state = {
    .name = "REC_GROUP",
    .parent = &working_state,
    .state_func = HSM_STATE_FUNC(rec_group_state_func),
};

/* ---------------------------- REC_NORMAL 普通录像 ---------------------------- */
static HSM_RET_T rec_normal_state_func(hsm_obj_t *me, hsm_event_t *e)
{
    (void)me;
    switch (e->sig)
    {
        case HSM_ENTRY_SIG:
            /* TODO(业务层):
             * 1. 读取配置（分辨率/编码格式/分段参数/循环录像/重力旋转）；
             * 2. 启动主码流 VENC；
             * 3. 初始化预录制环形缓存；
             * 4. 启动 MP4 分片封装。 */
            HWSLOT("REC_NORMAL enter: start record");
            return HSM_HANDLED();

        case HSM_EXIT_SIG:
            /* TODO(业务层): 停止 VENC；结束 MP4 写入；回收预录缓存 */
            HWSLOT("REC_NORMAL exit: stop record");
            return HSM_HANDLED();

        case MSG_REC_SEGMENT_DONE:
            /* 分段完成：TODO(业务层) 关闭当前 MP4 段、开启新段（循环录制），
             * 状态不变 */
            HWSLOT("REC_NORMAL: segment rollover");
            return HSM_HANDLED();

        default:
            return HSM_UNHANDLED();
    }
}

hsm_t rec_normal_state = {
    .name = "REC_NORMAL",
    .parent = &rec_group_state,
    .state_func = HSM_STATE_FUNC(rec_normal_state_func),
};

/* --------------------------- REC_TIMELAPSE 延时录像 --------------------------- */
static HSM_RET_T rec_timelapse_state_func(hsm_obj_t *me, hsm_event_t *e)
{
    (void)me;
    switch (e->sig)
    {
        case HSM_ENTRY_SIG:
            /* TODO(业务层): 加载缩时间隔参数；配置帧抽取；
             * 时间戳逻辑由媒体工作队列处理（勿阻塞 HFSM 线程） */
            HWSLOT("REC_TIMELAPSE enter");
            return HSM_HANDLED();

        case HSM_EXIT_SIG:
            /* TODO(业务层): 停止编码，完成缩时 MP4 输出 */
            HWSLOT("REC_TIMELAPSE exit");
            return HSM_HANDLED();

        case MSG_REC_SEGMENT_DONE:
            HWSLOT("REC_TIMELAPSE: segment rollover");
            return HSM_HANDLED();

        default:
            return HSM_UNHANDLED();
    }
}

hsm_t rec_timelapse_state = {
    .name = "REC_TIMELAPSE",
    .parent = &rec_group_state,
    .state_func = HSM_STATE_FUNC(rec_timelapse_state_func),
};

/* --------------------------- REC_SLOW_MOTION 慢动作 --------------------------- */
static HSM_RET_T rec_slow_motion_state_func(hsm_obj_t *me, hsm_event_t *e)
{
    (void)me;
    switch (e->sig)
    {
        case HSM_ENTRY_SIG:
            /* TODO(业务层): Sensor 切高帧率；配置 VENC；
             * 时间戳压缩（慢放）由媒体层实现 */
            HWSLOT("REC_SLOW_MOTION enter");
            return HSM_HANDLED();

        case HSM_EXIT_SIG:
            /* TODO(业务层): 关高帧率，恢复预览帧率 */
            HWSLOT("REC_SLOW_MOTION exit");
            return HSM_HANDLED();

        case MSG_REC_SEGMENT_DONE:
            HWSLOT("REC_SLOW_MOTION: segment rollover");
            return HSM_HANDLED();

        default:
            return HSM_UNHANDLED();
    }
}

hsm_t rec_slow_motion_state = {
    .name = "REC_SLOW_MOTION",
    .parent = &rec_group_state,
    .state_func = HSM_STATE_FUNC(rec_slow_motion_state_func),
};

/* ==================================================================================
 * PHOTO_GROUP 拍照父状态
 * ================================================================================== */
static HSM_RET_T photo_group_state_func(hsm_obj_t *me, hsm_event_t *e)
{
    (void)me;
    switch (e->sig)
    {
        case HSM_ENTRY_SIG:
            /* TODO(业务层): 初始化 JPEG 抓拍资源 */
            HWSLOT("PHOTO_GROUP enter");
            return HSM_HANDLED();

        case HSM_EXIT_SIG:
            /* TODO(业务层): 释放抓拍临时资源 */
            HWSLOT("PHOTO_GROUP exit");
            return HSM_HANDLED();

        case MSG_PHOTO_DONE:
            /* 抓拍完成：立刻回预览（拇指相机使用习惯，不驻留拍照态） */
            return HSM_TRAN(&idle_preview_state);

        case MSG_SD_REMOVE:
            /* 拍照依赖 SD 卡写文件：拔卡时交 SYS_WORKING 统一处理 */
            return HSM_UNHANDLED();

        default:
            return HSM_UNHANDLED();
    }
}

hsm_t photo_group_state = {
    .name = "PHOTO_GROUP",
    .parent = &working_state,
    .state_func = HSM_STATE_FUNC(photo_group_state_func),
};

/* ---------------------------- PHOTO_SINGLE 普通单拍 ---------------------------- */
static HSM_RET_T photo_single_state_func(hsm_obj_t *me, hsm_event_t *e)
{
    (void)me;
    switch (e->sig)
    {
        case HSM_ENTRY_SIG:
            /* TODO(业务层): 触发 Snapshot；取 ISP YUV 编码 JPEG 写 SD；
             * 完成后由媒体线程投递 MSG_PHOTO_DONE */
            HWSLOT("PHOTO_SINGLE enter: snapshot");
            return HSM_HANDLED();

        case HSM_EXIT_SIG:
            return HSM_HANDLED();

        default:
            /* MSG_PHOTO_DONE 由父状态统一处理 */
            return HSM_UNHANDLED();
    }
}

hsm_t photo_single_state = {
    .name = "PHOTO_SINGLE",
    .parent = &photo_group_state,
    .state_func = HSM_STATE_FUNC(photo_single_state_func),
};

/* --------------------------- PHOTO_DELAY 倒计时拍照 --------------------------- */
static HSM_RET_T photo_delay_state_func(hsm_obj_t *me, hsm_event_t *e)
{
    (void)me;
    static int countdown = 0;       /* 仅派发线程访问，无并发 */

    switch (e->sig)
    {
        case HSM_ENTRY_SIG:
            countdown = ThumCam_HSM_GetDelayPhotoSeconds();
            HWSLOT("PHOTO_DELAY enter: countdown %ds", countdown);
            return HSM_HANDLED();

        case HSM_EXIT_SIG:
            countdown = 0;
            return HSM_HANDLED();

        case MSG_DELAY_PHOTO_TICK:
            countdown--;
            if (countdown > 0)
            {
                /* TODO(业务层): GUI 显示剩余秒数 */
                HWSLOT("PHOTO_DELAY: %d ...", countdown);
                return HSM_HANDLED();
            }
            /* 倒计时结束：执行抓拍（同单拍），完成由 MSG_PHOTO_DONE 收口 */
            /* TODO(业务层): 触发 Snapshot 抓拍 */
            HWSLOT("PHOTO_DELAY: countdown done, snapshot");
            return HSM_TRAN(&photo_single_state);

        case MSG_PHOTO_KEY:
            /* 倒计时期间再按一次拍照键：取消抓拍，直接回预览 */
            HWSLOT("PHOTO_DELAY: canceled by user");
            return HSM_TRAN(&idle_preview_state);

        case MSG_TOUCH_BACK:
            return HSM_TRAN(&idle_preview_state);

        /* 倒计时期间不进屏保：屏保会挂起倒计时 tick，导致读秒停住。
         * 超时事件上浮到 WORKING 被丢弃，下一个周期会自动重新武装。 */

        default:
            return HSM_UNHANDLED();
    }
}

hsm_t photo_delay_state = {
    .name = "PHOTO_DELAY",
    .parent = &photo_group_state,
    .state_func = HSM_STATE_FUNC(photo_delay_state_func),
};

/* ==================================================================================
 * PLAYBACK 文件回放
 * ================================================================================== */
static HSM_RET_T playback_state_func(hsm_obj_t *me, hsm_event_t *e)
{
    (void)me;
    switch (e->sig)
    {
        case HSM_ENTRY_SIG:
            /* TODO(业务层): 关 VENC；初始化解码器；读取 SD 相册列表；
             * 关闭重力画面旋转（回放不需要旋转） */
            HWSLOT("PLAYBACK enter");
            return HSM_HANDLED();

        case HSM_EXIT_SIG:
            /* TODO(业务层): 销毁解码器资源，释放内存 */
            HWSLOT("PLAYBACK exit");
            return HSM_HANDLED();

        case MSG_TOUCH_NEXT:
            /* TODO(业务层): 下一个文件；到最后一个再翻 → 回到第一个（循环） */
            HWSLOT("PLAYBACK: next (cycle)");
            return HSM_HANDLED();

        case MSG_TOUCH_PREV:
            /* TODO(业务层): 上一个文件；到第一个再翻 → 回到最后一个（循环） */
            HWSLOT("PLAYBACK: prev (cycle)");
            return HSM_HANDLED();

        case MSG_TOUCH_DELETE:
            /* TODO(业务层): 删除当前文件并刷新列表 */
            HWSLOT("PLAYBACK: delete file");
            return HSM_HANDLED();

        case MSG_TOUCH_BACK:
        case MSG_PLAYBACK_KEY:
            /* 触摸返回退出回放 */
            return HSM_TRAN(&idle_preview_state);

        case MSG_SLEEP_TIMEOUT:
            /* 回放属于用户观看场景，不允许自动休眠（用户主动返回预览后才休眠） */
            return HSM_HANDLED();

        case MSG_SAVER_TIMEOUT:
            /* 回放态也允许屏保（只关背光，回放继续） */
            return HSM_TRAN_KEEP(&screen_saver_state);

        default:
            return HSM_UNHANDLED();
    }
}

hsm_t playback_state = {
    .name = "PLAYBACK",
    .parent = &working_state,
    .state_func = HSM_STATE_FUNC(playback_state_func),
};

/* ==================================================================================
 * SETTING_MENU 设置菜单
 * 原则：只改内存配置，不操作硬件；配置在录像/预览的 ENTRY 中加载生效
 * ================================================================================== */
static HSM_RET_T setting_menu_state_func(hsm_obj_t *me, hsm_event_t *e)
{
    (void)me;
    switch (e->sig)
    {
        case HSM_ENTRY_SIG:
            /* TODO(业务层): UI 切换设置页面 */
            HWSLOT("SETTING_MENU enter");
            return HSM_HANDLED();

        case HSM_EXIT_SIG:
            HWSLOT("SETTING_MENU exit");
            return HSM_HANDLED();

        case MSG_TOUCH_BACK:
        case MSG_SETTING_KEY:
            return HSM_TRAN(&idle_preview_state);

        case MSG_SAVER_TIMEOUT:
            /* 菜单页也允许屏保（只关背光） */
            return HSM_TRAN_KEEP(&screen_saver_state);

        case MSG_SLEEP_TIMEOUT:
            /* 菜单操作态不允许自动休眠，防止改配置改一半被挂起 */
            return HSM_HANDLED();

        default:
            return HSM_UNHANDLED();
    }
}

hsm_t setting_menu_state = {
    .name = "SETTING_MENU",
    .parent = &working_state,
    .state_func = HSM_STATE_FUNC(setting_menu_state_func),
};

/* ==================================================================================
 * USB_GROUP USB 插入父状态：插入后弹 UI 选模式；家族内禁止录像拍照
 * ================================================================================== */
static HSM_RET_T usb_group_state_func(hsm_obj_t *me, hsm_event_t *e)
{
    (void)me;
    switch (e->sig)
    {
        case HSM_ENTRY_SIG:
            /* TODO(业务层): 停止所有录像/抓拍业务；弹出模式选择 UI，
             * 等待用户通过 MSG_USB_MODE_SELECT 选择 */
            HWSLOT("USB_GROUP enter: popup mode select ui");
            return HSM_HANDLED();

        case HSM_INIT_SIG:
            /* 默认进入"仅充电"（安全模式）；用户选择后由 MSG_USB_MODE_SELECT
             * 跳到对应子状态 */
            return HSM_TRAN(&usb_chg_only_state);

        case HSM_EXIT_SIG:
            /* USB 拔出，回预览 */
            HWSLOT("USB_GROUP exit: usb removed");
            return HSM_HANDLED();

        case MSG_USB_MODE_SELECT:
            switch ((intptr_t)e->param)
            {
                case THUMCAM_USB_UVC:
                    return HSM_TRAN(&usb_uvc_state);
                case THUMCAM_USB_MASS_STORAGE:
                    return HSM_TRAN(&usb_mass_storage_state);
                case THUMCAM_USB_CHG_ONLY:
                default:
                    return HSM_TRAN(&usb_chg_only_state);
            }

        case MSG_USB_REMOVE:
            return HSM_TRAN(&idle_preview_state);

        case MSG_USB_INSERT:
            /* 已在 USB 家族：忽略重复的插入事件（避免重进 USB_GROUP 重弹模式选择） */
            return HSM_HANDLED();

        /* ---- 家族级屏蔽：USB 插入期间禁止本地录像/拍照/远程录像 ---- */
        case MSG_REC_KEY:
        case MSG_PHOTO_KEY:
        case MSG_MODE_KEY:
        case MSG_REMOTE_REC_START:
        case MSG_REMOTE_REC_STOP:
        case MSG_REMOTE_SNAP:
            HWSLOT("USB_GROUP: rec/photo blocked");
            return HSM_HANDLED();

        case MSG_SAVER_TIMEOUT:
            /* USB 模式允许屏保（只关背光） */
            return HSM_TRAN_KEEP(&screen_saver_state);

        case MSG_SLEEP_TIMEOUT:
            /* USB 插入（如 U 盘传输/UVC）期间不自动休眠 */
            return HSM_HANDLED();

        default:
            return HSM_UNHANDLED();
    }
}

hsm_t usb_group_state = {
    .name = "USB_GROUP",
    .parent = &working_state,
    .state_func = HSM_STATE_FUNC(usb_group_state_func),
};

/* ------------------------------ USB 仅充电模式 ------------------------------ */
static HSM_RET_T usb_chg_only_state_func(hsm_obj_t *me, hsm_event_t *e)
{
    (void)me;
    switch (e->sig)
    {
        case HSM_ENTRY_SIG:
            /* TODO(业务层): 充电图标显示；相机业务（预览/回放/设置）可用 */
            HWSLOT("USB_CHG_ONLY enter");
            return HSM_HANDLED();

        case HSM_EXIT_SIG:
            return HSM_HANDLED();

        default:
            /* 预览/回放/设置类交互由 GUI 层在本模式下处理；
             * 录像/拍照已被 USB_GROUP 屏蔽 */
            return HSM_UNHANDLED();
    }
}

hsm_t usb_chg_only_state = {
    .name = "USB_CHG_ONLY",
    .parent = &usb_group_state,
    .state_func = HSM_STATE_FUNC(usb_chg_only_state_func),
};

/* -------------------------------- USB UVC 模式 -------------------------------- */
static HSM_RET_T usb_uvc_state_func(hsm_obj_t *me, hsm_event_t *e)
{
    (void)me;
    switch (e->sig)
    {
        case HSM_ENTRY_SIG:
            /* TODO(业务层): ISP 切 UVC 输出通路；关闭本地 VENC 与 SD 存储 */
            HWSLOT("USB_UVC enter");
            return HSM_HANDLED();

        case HSM_EXIT_SIG:
            /* TODO(业务层): 关 UVC，恢复本地预览通路 */
            HWSLOT("USB_UVC exit");
            return HSM_HANDLED();

        default:
            return HSM_UNHANDLED();
    }
}

hsm_t usb_uvc_state = {
    .name = "USB_UVC",
    .parent = &usb_group_state,
    .state_func = HSM_STATE_FUNC(usb_uvc_state_func),
};

/* --------------------------------- USB U盘模式 --------------------------------- */
static HSM_RET_T usb_mass_storage_state_func(hsm_obj_t *me, hsm_event_t *e)
{
    (void)me;
    switch (e->sig)
    {
        case HSM_ENTRY_SIG:
            /* TODO(业务层): SD 卡挂载给 USB 主机；本地禁止读写 SD 卡 */
            HWSLOT("USB_MASS_STORAGE enter");
            return HSM_HANDLED();

        case HSM_EXIT_SIG:
            /* TODO(业务层): 卸载 Mass Storage，SD 卡归还本地 */
            HWSLOT("USB_MASS_STORAGE exit");
            return HSM_HANDLED();

        case MSG_SD_REMOVE:
            /* U 盘模式下 SD 被拔出：交 SYS_WORKING 统一弹窗处理 */
            return HSM_UNHANDLED();

        default:
            return HSM_UNHANDLED();
    }
}

hsm_t usb_mass_storage_state = {
    .name = "USB_MASS_STORAGE",
    .parent = &usb_group_state,
    .state_func = HSM_STATE_FUNC(usb_mass_storage_state_func),
};
