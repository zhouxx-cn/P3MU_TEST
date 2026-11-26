/**
 * @file    thumcam_hsm.c
 * @brief   拇指相机 HFSM 框架核心：消息队列 + 单线程派发 + 系统定时器
 *
 * 线程/变量安全说明：
 *   1. 消息队列：pthread_mutex + pthread_cond 保护，多生产者单消费者，线程安全；
 *   2. hsm 引擎（current/last/saved）：仅在派发线程内通过 hsm_dispatch() 修改，
 *      其他线程通过状态切换回调发布的快照（last_state）只读访问；
 *   3. 定时器配置/活动时间戳：由 PostEvent/NotifyActivity/SetTimeouts 从任意
 *      线程调用，均在 q_lock 内读写；
 *   4. 状态机初始化在派发线程内完成，所有 ENTRY/EXIT 都在派发线程上下文执行；
 *   5. 用户回调 user_cb：注册/注销时加锁，调用时快照到局部变量再执行，
 *      避免"注销后悬空指针"竞争。
 */

#include <string.h>
#include <time.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include "thumcam_hsm_internal.h"

/* 定时器轮询 tick 周期（毫秒）：派发线程无事件时的最大等待时长 */
#define HFSM_TICK_MS         200
/* 倒计时拍照 tick 间隔（毫秒） */
#define HFSM_DELAY_TICK_MS   1000
/* 默认超时配置 */
#define HFSM_DEFAULT_SAVER_MS      (30 * 1000)
#define HFSM_DEFAULT_SLEEP_MS      (60 * 1000)
#define HFSM_DEFAULT_AUTOOFF_MS    0
#define HFSM_DEFAULT_DELAY_PHOTO_S 3

static thumcam_hfsm_t g_hfsm = {
    .saver_ms = HFSM_DEFAULT_SAVER_MS,
    .sleep_ms = HFSM_DEFAULT_SLEEP_MS,
    .autooff_ms = HFSM_DEFAULT_AUTOOFF_MS,
    .delay_photo_sec = HFSM_DEFAULT_DELAY_PHOTO_S,
    .q_lock = PTHREAD_MUTEX_INITIALIZER,
    .q_cond = PTHREAD_COND_INITIALIZER,
    .init_cond = PTHREAD_COND_INITIALIZER,
    .state_lock = PTHREAD_MUTEX_INITIALIZER,
};

/* ---- 状态快照原子读写（供派发线程外的线程无锁只读访问） ----
 * 使用 GCC 原子内建函数：无需 volatile，跨线程可见性与顺序性由
 * ACQUIRE/RELEASE 语义保证，且不触发 -Wcast-qual/-Wdiscarded-qual。 */
static inline const hsm_t *hsm_state_load(void)
{
    return __atomic_load_n(&g_hfsm.last_state, __ATOMIC_ACQUIRE);
}

static inline void hsm_state_store(const hsm_t *s)
{
    __atomic_store_n(&g_hfsm.last_state, s, __ATOMIC_RELEASE);
}

static inline const hsm_t *hsm_saved_load(void)
{
    return __atomic_load_n(&g_hfsm.last_saved, __ATOMIC_ACQUIRE);
}

static inline void hsm_saved_store(const hsm_t *s)
{
    __atomic_store_n(&g_hfsm.last_saved, s, __ATOMIC_RELEASE);
}

/* 编译期确认基类布局约束：super 必须位于结构体首地址（hsm 引擎强转依赖） */
_Static_assert(offsetof(thumcam_hfsm_t, super) == 0,
               "hsm_obj_t super must be the first member");

/* ==================================================================================
 * 工具函数
 * ================================================================================== */
static long long now_ms(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (long long)ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
}

static void publish_state_snapshot(void)
{
    hsm_state_store(g_hfsm.super.current);
    /* 叠加态期间同步发布被保留的业务状态（非叠加期为 NULL），
     * 供 ThumCam_HSM_IsInState 做"叠加感知"判断 */
    hsm_saved_store(g_hfsm.super.saved);
}

/* 状态切换通知：必须在事件派发完成之后调用。
 * 这样回调触发的时刻，目标状态的 ENTRY 已经执行完毕、旧状态的 EXIT 已经收尾，
 * 业务层/UI 在回调里查询到的状态与实际硬件动作是一致的。
 * 原实现在 hsm_tran 内部（Entry 之前）就通知，UI 会先看到"新状态"再看到硬件就绪。 */
static void notify_state_change(const hsm_t *before, const hsm_t *after)
{
    void (*cb)(const char *, const char *);

    /* 先发布快照：回调里 GetStateName()/IsInState() 能读到新状态 */
    publish_state_snapshot();

    if (before == after)
    {
        return;                                 /* 状态未变化不通知 */
    }

    THUMCAM_HSM_LOG("state: %s -> %s",
                    before ? before->name : "NULL",
                    after ? after->name : "NULL");

    pthread_mutex_lock(&g_hfsm.state_lock);
    cb = g_hfsm.user_cb;
    pthread_mutex_unlock(&g_hfsm.state_lock);

    if (cb != NULL)
    {
        cb(before ? before->name : "NULL", after ? after->name : "NULL");
    }
}

/* 用户活动类事件：派发时自动重置超时计时器。
 * 覆盖所有"用户/外部主动操作"，包括硬件插拔与业务层的停止录像通知，
 * 漏掉任何一个都会让对应的超时定时器一直保持"已触发"状态而不再重新武装。 */
static bool is_user_activity(int sig)
{
    switch (sig)
    {
        case MSG_POWER_KEY_SHORT:
        case MSG_POWER_KEY_LONG:
        case MSG_REC_KEY:
        case MSG_PHOTO_KEY:
        case MSG_PHOTO_DELAY:
        case MSG_PLAYBACK_KEY:
        case MSG_SETTING_KEY:
        case MSG_MODE_KEY:
        case MSG_TOUCH_NEXT:
        case MSG_TOUCH_PREV:
        case MSG_TOUCH_DELETE:
        case MSG_TOUCH_BACK:
        case MSG_USB_MODE_SELECT:
        case MSG_REMOTE_REC_START:
        case MSG_REMOTE_REC_STOP:
        case MSG_REMOTE_SNAP:
        case MSG_REMOTE_PREVIEW:
        case MSG_REC_STOP:
        case MSG_USB_INSERT:
        case MSG_USB_REMOVE:
            return true;
        default:
            return false;
    }
}

/* ==================================================================================
 * 消息队列（多生产者单消费者，q_lock 全程保护）
 * ================================================================================== */
/* 核心 push：调用者必须已持有 q_lock（供定时器等已持锁路径复用，避免重入死锁） */
static int queue_push_locked(int sig, void *param)
{
    if (!g_hfsm.running)
    {
        return -1;                                  /* 已停止，拒绝投递 */
    }
    if (g_hfsm.q_count >= HFSM_QUEUE_DEPTH)
    {
        THUMCAM_HSM_LOG("[WARN] queue full, drop msg %d", sig);
        return -1;                                  /* 队列满，丢弃 */
    }
    g_hfsm.queue[g_hfsm.q_tail].sig = sig;
    g_hfsm.queue[g_hfsm.q_tail].param = param;
    g_hfsm.q_tail = (g_hfsm.q_tail + 1) % HFSM_QUEUE_DEPTH;
    g_hfsm.q_count++;
    pthread_cond_signal(&g_hfsm.q_cond);
    return 0;
}

static int queue_push(int sig, void *param)
{
    int ret;

    pthread_mutex_lock(&g_hfsm.q_lock);
    ret = queue_push_locked(sig, param);
    pthread_mutex_unlock(&g_hfsm.q_lock);
    return ret;
}

/* 带超时出队：有事件立即返回 1，超时返回 0 */
static int queue_pop(hsm_event_t *out, int timeout_ms)
{
    struct timespec ts;
    int ret = 0;

    pthread_mutex_lock(&g_hfsm.q_lock);
    if (g_hfsm.q_count == 0)
    {
        clock_gettime(CLOCK_REALTIME, &ts);
        ts.tv_sec  += timeout_ms / 1000;
        ts.tv_nsec += (long)(timeout_ms % 1000) * 1000000L;
        if (ts.tv_nsec >= 1000000000L)
        {
            ts.tv_sec += 1;
            ts.tv_nsec -= 1000000000L;
        }
        pthread_cond_timedwait(&g_hfsm.q_cond, &g_hfsm.q_lock, &ts);
    }

    if (g_hfsm.q_count > 0)
    {
        *out = g_hfsm.queue[g_hfsm.q_head];
        g_hfsm.q_head = (g_hfsm.q_head + 1) % HFSM_QUEUE_DEPTH;
        g_hfsm.q_count--;
        ret = 1;
    }
    pthread_mutex_unlock(&g_hfsm.q_lock);
    return ret;
}

/* ==================================================================================
 * 系统定时器：在互斥锁内检查超时并投递事件（q_lock 保护所有定时变量）
 * ================================================================================== */
static void timer_check_locked(void)
{
    long long now = now_ms();

    /* 超时事件按"截止时刻"周期性重投：
     * 原实现是一次性 latch（投递即置位，直到下次用户活动才复位），一旦某个状态
     * 拒绝该事件（例如录像态拒绝休眠），定时器就会永久静默——设备再也不休眠。
     * 改为周期性重投后，拒绝只影响当次，下一个周期自动重新武装。 */

    /* 屏保超时：任意业务状态可进（由各状态决定是否响应） */
    if (g_hfsm.saver_ms > 0 && now >= g_hfsm.next_saver_ms)
    {
        g_hfsm.next_saver_ms = now + g_hfsm.saver_ms;
        (void)queue_push_locked(MSG_SAVER_TIMEOUT, NULL);
    }

    /* 休眠超时：仅预览态响应；录像等状态收到后自行丢弃 */
    if (g_hfsm.sleep_ms > 0 && now >= g_hfsm.next_sleep_ms)
    {
        g_hfsm.next_sleep_ms = now + g_hfsm.sleep_ms;
        (void)queue_push_locked(MSG_SLEEP_TIMEOUT, NULL);
    }

    /* 自动关机超时 */
    if (g_hfsm.autooff_ms > 0 && now >= g_hfsm.next_autooff_ms)
    {
        g_hfsm.next_autooff_ms = now + g_hfsm.autooff_ms;
        (void)queue_push_locked(MSG_AUTO_POWEROFF, NULL);
    }

    /* 倒计时拍照 tick：仅在倒计时拍照状态下周期投递 */
    if (now - g_hfsm.last_tick_ms >= HFSM_DELAY_TICK_MS)
    {
        g_hfsm.last_tick_ms = now;
        if (hsm_state_load() == &photo_delay_state)
        {
            (void)queue_push_locked(MSG_DELAY_PHOTO_TICK, NULL);
        }
    }
}

static void timer_reset_locked(void)
{
    long long now = now_ms();

    g_hfsm.last_activity_ms = now;
    g_hfsm.next_saver_ms = now + g_hfsm.saver_ms;
    g_hfsm.next_sleep_ms = now + g_hfsm.sleep_ms;
    g_hfsm.next_autooff_ms = now + g_hfsm.autooff_ms;
}

/* ==================================================================================
 * 派发线程主循环（唯一调用 hsm_dispatch 的线程）
 * ================================================================================== */
static void *hfsm_thread_loop(void *arg)
{
    hsm_event_t e;

    (void)arg;

    /* 状态机初始化放在派发线程内完成：保证所有 ENTRY/EXIT 都在派发线程上下文，
     * 业务层的 Enter/Exit 钩子不会跨线程调用（原实现在调用者线程上做初始化）。 */
    hsm_init(&g_hfsm.super, g_hfsm.init_state);
    publish_state_snapshot();

    pthread_mutex_lock(&g_hfsm.q_lock);
    g_hfsm.inited = true;
    pthread_cond_broadcast(&g_hfsm.init_cond);
    pthread_mutex_unlock(&g_hfsm.q_lock);

    while (g_hfsm.running)
    {
        if (queue_pop(&e, HFSM_TICK_MS) == 1)
        {
            const hsm_t *before = g_hfsm.super.current;

            /* 用户活动事件：先重置定时器再派发 */
            if (is_user_activity(e.sig))
            {
                pthread_mutex_lock(&g_hfsm.q_lock);
                timer_reset_locked();
                pthread_mutex_unlock(&g_hfsm.q_lock);
            }

            hsm_dispatch(&g_hfsm.super, &e);

            /* 派发完成后再通知状态变化：此时目标状态的 ENTRY 已执行完毕 */
            notify_state_change(before, g_hfsm.super.current);
        }

        /* 定时器轮询：每轮循环都检查，避免事件繁忙时定时器饿死 */
        pthread_mutex_lock(&g_hfsm.q_lock);
        timer_check_locked();
        pthread_mutex_unlock(&g_hfsm.q_lock);
    }

    g_hfsm.running = false;
    return NULL;
}

/* ==================================================================================
 * 公共 API 实现
 * ================================================================================== */
int ThumCam_HSM_Start(int product_test)
{
    pthread_mutex_lock(&g_hfsm.q_lock);
    if (g_hfsm.started)
    {
        pthread_mutex_unlock(&g_hfsm.q_lock);
        return -1;
    }

    g_hfsm.inited = false;
    g_hfsm.init_state = product_test ? &product_test_state : &power_off_state;
    memset(&g_hfsm.queue, 0, sizeof(g_hfsm.queue));
    g_hfsm.q_head = 0;
    g_hfsm.q_tail = 0;
    g_hfsm.q_count = 0;
    g_hfsm.last_tick_ms = now_ms();
    timer_reset_locked();
    g_hfsm.running = true;
    pthread_mutex_unlock(&g_hfsm.q_lock);

    if (pthread_create(&g_hfsm.thread, NULL, hfsm_thread_loop, NULL) != 0)
    {
        g_hfsm.running = false;
        return -1;
    }

    /* 等待派发线程完成状态机初始化后返回，保证调用后状态已就绪 */
    pthread_mutex_lock(&g_hfsm.q_lock);
    while (!g_hfsm.inited)
    {
        pthread_cond_wait(&g_hfsm.init_cond, &g_hfsm.q_lock);
    }
    g_hfsm.started = true;
    pthread_mutex_unlock(&g_hfsm.q_lock);
    return 0;
}

void ThumCam_HSM_Stop(void)
{
    pthread_t tid;

    pthread_mutex_lock(&g_hfsm.q_lock);
    if (!g_hfsm.started)
    {
        pthread_mutex_unlock(&g_hfsm.q_lock);
        return;
    }
    tid = g_hfsm.thread;
    /* 置 running=false 并唤醒等待中的派发线程 */
    g_hfsm.running = false;
    pthread_cond_signal(&g_hfsm.q_cond);
    pthread_mutex_unlock(&g_hfsm.q_lock);

    pthread_join(tid, NULL);

    pthread_mutex_lock(&g_hfsm.q_lock);
    g_hfsm.started = false;
    pthread_mutex_unlock(&g_hfsm.q_lock);
}

int ThumCam_HSM_PostEvent(int sig, void *param)
{
    return queue_push(sig, param);
}

void ThumCam_HSM_NotifyActivity(void)
{
    pthread_mutex_lock(&g_hfsm.q_lock);
    timer_reset_locked();
    pthread_mutex_unlock(&g_hfsm.q_lock);
}

void ThumCam_HSM_SetTimeouts(int saver_ms, int sleep_ms, int autooff_ms)
{
    pthread_mutex_lock(&g_hfsm.q_lock);
    g_hfsm.saver_ms = (saver_ms > 0) ? saver_ms : 0;
    g_hfsm.sleep_ms = (sleep_ms > 0) ? sleep_ms : 0;
    g_hfsm.autooff_ms = (autooff_ms > 0) ? autooff_ms : 0;
    timer_reset_locked();               /* 新配置生效前重置计时 */
    pthread_mutex_unlock(&g_hfsm.q_lock);
}

void ThumCam_HSM_SetDelayPhotoSeconds(int seconds)
{
    if (seconds <= 0)
    {
        seconds = 1;
    }
    pthread_mutex_lock(&g_hfsm.q_lock);
    g_hfsm.delay_photo_sec = seconds;
    pthread_mutex_unlock(&g_hfsm.q_lock);
}

int ThumCam_HSM_GetDelayPhotoSeconds(void)
{
    int sec;
    pthread_mutex_lock(&g_hfsm.q_lock);
    sec = g_hfsm.delay_photo_sec;
    pthread_mutex_unlock(&g_hfsm.q_lock);
    return sec;
}

void ThumCam_HSM_RegisterStateChangeCb(void (*cb)(const char *from, const char *to))
{
    pthread_mutex_lock(&g_hfsm.state_lock);
    g_hfsm.user_cb = cb;
    pthread_mutex_unlock(&g_hfsm.state_lock);
}

const char *ThumCam_HSM_GetStateName(void)
{
    const hsm_t *s = hsm_state_load();      /* 原子读快照 */
    return s ? s->name : "none";
}

bool ThumCam_HSM_IsInState(const hsm_t *state)
{
    const hsm_t *s = hsm_state_load();      /* 叶子状态快照（叠加期为叠加态自身） */
    const hsm_t *sv = hsm_saved_load();     /* 叠加期被保留的业务状态快照 */

    if (state == NULL)
    {
        return false;
    }

    /* 叠加感知：叠加期间（如屏保盖在录像上）业务状态仍在运行，
     * 询问 rec_group 等业务状态时应返回 true，因此同时匹配 saved 链 */
    if (s != NULL && ((s == state) || hsm_is_child(state, s)))
    {
        return true;
    }
    return (sv != NULL) && ((sv == state) || hsm_is_child(state, sv));
}
