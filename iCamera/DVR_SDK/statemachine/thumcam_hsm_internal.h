/**
 * @file    thumcam_hsm_internal.h
 * @brief   HFSM 框架内部结构（仅 thumcam_hsm.c / thumcam_states.c 使用，
 *          业务层请勿 include，对外接口一律走 thumcam_hsm.h）
 */

#ifndef __THUMCAM_HSM_INTERNAL_H__
#define __THUMCAM_HSM_INTERNAL_H__

#include <stdbool.h>
#include <pthread.h>
#include "thumcam_hsm.h"        /* 消息枚举、状态对象声明、公共 API */

/* 消息队列深度：状态机事件频率低（按键/定时器/业务完成），64 足够 */
#define HFSM_QUEUE_DEPTH     64

typedef struct
{
    hsm_obj_t super;                /* 必须放第一位（hsm 引擎按基类指针访问） */

    /* ---- 派发线程 ---- */
    volatile bool running;          /* 停止标志，配合 mutex/cond 保证可见性 */
    bool started;
    bool inited;                    /* 派发线程已完成状态机初始化 */
    hsm_t *init_state;              /* 初始状态（派发线程初始化时使用） */
    pthread_t thread;

    /* ---- 消息队列（多生产者单消费者，q_lock 保护） ---- */
    hsm_event_t queue[HFSM_QUEUE_DEPTH];
    int q_head;                     /* 出队下标 */
    int q_tail;                     /* 入队下标 */
    int q_count;                    /* 当前元素个数 */
    pthread_mutex_t q_lock;
    pthread_cond_t  q_cond;         /* 队列非空/超时唤醒 */
    pthread_cond_t  init_cond;      /* 初始化完成握手 */

    /* ---- 系统定时器（q_lock 保护） ----
     * 超时事件按"截止时刻"周期性重投：到点投递一次，若被当前状态拒绝
     * （如录像态拒绝休眠），下一个周期会自动重新武装，不会永久静默。 */
    int saver_ms;
    int sleep_ms;
    int autooff_ms;
    long long last_activity_ms;     /* 最近一次用户活动时刻 */
    long long next_saver_ms;        /* 下次投递 MSG_SAVER_TIMEOUT 的时刻 */
    long long next_sleep_ms;        /* 下次投递 MSG_SLEEP_TIMEOUT 的时刻 */
    long long next_autooff_ms;      /* 下次投递 MSG_AUTO_POWEROFF 的时刻 */
    long long last_tick_ms;         /* 倒计时拍照 tick 计时 */

    /* ---- 倒计时拍照秒数（q_lock 保护；PHOTO_DELAY 进入时读取） ---- */
    int delay_photo_sec;

    /* ---- 状态快照与通知（state_lock 保护 user_cb；两个快照用原子读写） ---- */
    pthread_mutex_t state_lock;
    void (*user_cb)(const char *from, const char *to);
    const hsm_t *last_state;        /* 派发线程每次 dispatch 后原子发布叶子状态快照 */
    const hsm_t *last_saved;        /* 叠加态期间被保留的业务状态快照，
                                     * 非叠加期恒为 NULL；供 IsInState 叠加感知 */
} thumcam_hfsm_t;

#endif /* __THUMCAM_HSM_INTERNAL_H__ */
