/**
 * @file    ui_msgq.c
 * @brief   UI 消息队列实现：环形缓冲 + 互斥量 + 条件变量。
 */
#include "ui_msgq.h"
#include <pthread.h>
#include <errno.h>
#include <time.h>

typedef struct {
    ui_event_t      buf[UI_MSGQ_CAPACITY];
    int             head;      /* 读位置 */
    int             tail;      /* 写位置 */
    int             count;     /* 当前元素数 */
    pthread_mutex_t lock;
    pthread_cond_t  not_empty;
    int             inited;
} ui_msgq_t;

static ui_msgq_t s_q;

int ui_msgq_init(void)
{
    if (s_q.inited) {
        return UI_OK;
    }
    memset(&s_q, 0, sizeof(s_q));
    if (pthread_mutex_init(&s_q.lock, NULL) != 0) {
        return UI_ERR;
    }
    if (pthread_cond_init(&s_q.not_empty, NULL) != 0) {
        pthread_mutex_destroy(&s_q.lock);
        return UI_ERR;
    }
    s_q.inited = 1;
    return UI_OK;
}

void ui_msgq_deinit(void)
{
    if (!s_q.inited) {
        return;
    }
    pthread_mutex_destroy(&s_q.lock);
    pthread_cond_destroy(&s_q.not_empty);
    s_q.inited = 0;
}

/* 队列满时的降级策略：若新来的是视频帧事件，直接丢弃（反正下一帧马上来）；
 * 其它关键事件（按键/业务/退出）则丢弃队首最旧的视频帧为其腾位，
 * 若队列里没有可丢的视频帧，则丢弃最旧事件，保证关键交互不卡死。 */
static int try_make_room_locked(void)
{
    int i, idx;
    for (i = 0; i < s_q.count; i++) {
        idx = (s_q.head + i) % UI_MSGQ_CAPACITY;
        if (s_q.buf[idx].type == UI_EVT_VIDEO_FRAME) {
            /* 将该视频帧之前的元素整体后移一格覆盖它（简单起见直接标记为 NONE 并跳过） */
            s_q.buf[idx].type = UI_EVT_NONE;
            return UI_OK;
        }
    }
    /* 没有视频帧可丢：丢弃最旧一个 */
    s_q.head = (s_q.head + 1) % UI_MSGQ_CAPACITY;
    s_q.count--;
    return UI_OK;
}

int ui_msgq_post(const ui_event_t *evt)
{
    if (!s_q.inited || !evt) {
        return UI_ERR_PARAM;
    }

    pthread_mutex_lock(&s_q.lock);

    if (s_q.count >= UI_MSGQ_CAPACITY) {
        if (evt->type == UI_EVT_VIDEO_FRAME) {
            /* 实时流：满了就丢当前帧，不阻塞推流线程 */
            pthread_mutex_unlock(&s_q.lock);
            return UI_ERR;
        }
        try_make_room_locked();
    }

    s_q.buf[s_q.tail] = *evt;
    s_q.tail = (s_q.tail + 1) % UI_MSGQ_CAPACITY;
    s_q.count++;

    pthread_cond_signal(&s_q.not_empty);
    pthread_mutex_unlock(&s_q.lock);
    return UI_OK;
}

int ui_msgq_poll(ui_event_t *evt, int timeout_ms)
{
    int rc = UI_ERR;

    if (!s_q.inited || !evt) {
        return UI_ERR_PARAM;
    }

    pthread_mutex_lock(&s_q.lock);

    if (s_q.count == 0) {
        if (timeout_ms == 0) {
            pthread_mutex_unlock(&s_q.lock);
            return UI_ERR;
        } else if (timeout_ms < 0) {
            while (s_q.count == 0) {
                pthread_cond_wait(&s_q.not_empty, &s_q.lock);
            }
        } else {
            struct timespec ts;
            clock_gettime(CLOCK_REALTIME, &ts);
            ts.tv_sec  += timeout_ms / 1000;
            ts.tv_nsec += (long)(timeout_ms % 1000) * 1000000L;
            if (ts.tv_nsec >= 1000000000L) {
                ts.tv_sec  += 1;
                ts.tv_nsec -= 1000000000L;
            }
            while (s_q.count == 0) {
                if (pthread_cond_timedwait(&s_q.not_empty, &s_q.lock, &ts) == ETIMEDOUT) {
                    break;
                }
            }
        }
    }

    /* 跳过被降级标记为 NONE 的空洞 */
    while (s_q.count > 0 && s_q.buf[s_q.head].type == UI_EVT_NONE) {
        s_q.head = (s_q.head + 1) % UI_MSGQ_CAPACITY;
        s_q.count--;
    }

    if (s_q.count > 0) {
        *evt = s_q.buf[s_q.head];
        s_q.head = (s_q.head + 1) % UI_MSGQ_CAPACITY;
        s_q.count--;
        rc = UI_OK;
    }

    pthread_mutex_unlock(&s_q.lock);
    return rc;
}
