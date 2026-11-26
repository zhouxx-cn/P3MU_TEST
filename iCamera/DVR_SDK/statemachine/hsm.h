/**
 * @file    hsm.h
 * @brief   Hierarchical state machines for designing event-driven systems.
 * @copyright  (c) 2024, yezenghui, <y956370541@163.com>
 * @license MIT
 * @note    本文件基于 doc/hsm-master 开源框架拷贝适配，改动点：
 *          1. HSM_ASSERT 由 while(1) 死循环改为可覆盖的日志宏（量产安全）；
 *          2. 注释转 UTF-8；
 *          3. 新增"正交区域/浅历史"跳转（HSM_TRAN_KEEP / HSM_RESUME）：
 *             进入叠加态（如屏保）时不退出源业务状态链，源业务（如录像）不中断；
 *             配套 hsm_obj_t.saved 保存被保留的业务状态。
 *          其余逻辑与原版保持一致，勿随意改动引擎行为。
 */

#ifndef __HSM_H__
#define __HSM_H__
#include "stdint.h"
#include "stdbool.h"
#include "stddef.h"
#include "stdio.h"
#ifdef __cplusplus
extern "C" {
#endif

/*状态机支持的最大嵌套层数*/
#define HSM_MAX_LEVEL 10

/*前置声明*/
typedef struct hsm hsm_t;
typedef struct hsm_obj hsm_obj_t;

/*状态机默认事件信号*/
enum{
	HSM_ENTRY_SIG = -5,  //进入事件信号，进入状态时执行触发一次,内部信号
	HSM_EXIT_SIG = -4,   //退出事件信号,退出状态时触发一次，内部信号
    HSM_INIT_SIG = -3,   //初始化事件，内部信号
    //
    HSM_CYCLE_SIG = -2,  //周期事件信号，需用户自行触发
    //
    HSM_DEF_SIG = -1,
    //用户自定义事件在外部定义，需大于HSM_DEF_SIG
};
typedef int HSM_SIG_T;


/*状态机事件结构体*/
typedef struct{
	HSM_SIG_T sig;//信号
	void *param;  //事件参数
}hsm_event_t;


/*状态机事件处理函数返回值*/
typedef enum{
	HSM_RET_HANDLED,  //事件已处理，不再向上传播到父状态
	HSM_RET_UNHANDLED, //事件继续向上传播给父状态
	HSM_RET_TRAN,    //状态转换（退出源状态链 + 进入目标状态链）
	HSM_RET_TRAN_KEEP, //状态转换（保留源状态链，只进入目标状态链）
	HSM_RET_RESUME,  //恢复被保留的业务状态（只退出叠加态，不重入业务链）
}HSM_RET_T;


typedef struct hsm_obj{
    hsm_t *current;
    hsm_t *last;
    hsm_t *saved; //保留跳转时保存的业务状态，正常跳转后为 NULL

    //状态转换回调函数，可用于调试信息打印
    void(*state_tran_cb)(hsm_t *source, hsm_t *target);
}hsm_obj_t;

/*状态函数类型*/
typedef HSM_RET_T(*state_func_t)(hsm_obj_t *me, hsm_event_t *e);

typedef struct hsm{
    const char *name;
    const struct hsm *parent;
    state_func_t state_func;
}hsm_t;


/*状态机事件处理结果*/
#define HSM_HANDLED() (HSM_RET_HANDLED)
#define HSM_UNHANDLED() (HSM_RET_UNHANDLED)
/* 常规跳转。注意自跳转语义：target == 当前状态时引擎不发送 EXIT/ENTRY
 * （经典 HSM 语义是退出再重入），需要重置场景请自行先跳到中间状态。 */
#define HSM_TRAN(target) (((hsm_obj_t*)(me))->last = ((hsm_obj_t*)(me))->current, \
                          ((hsm_obj_t*)(me))->current = (target), HSM_RET_TRAN)

/* 保留源状态跳转（正交区域 / 浅历史）：
 * 进入 target，但不对源状态链发送 HSM_EXIT_SIG —— 源业务（如录像）保持运行。
 * 被保留的业务状态存放在 me->saved，离开叠加态时由引擎负责退出该业务链。
 * 典型用法：屏保叠加在业务态之上，只关背光，录像不中断。
 * 说明：已在叠加态中再次请求保留跳转时，保留原有的 me->saved，避免业务链被覆盖丢失。 */
#define HSM_TRAN_KEEP(target) ( \
    (((hsm_obj_t*)(me))->saved = (((hsm_obj_t*)(me))->saved != NULL) ? \
                                 ((hsm_obj_t*)(me))->saved : ((hsm_obj_t*)(me))->current), \
    ((hsm_obj_t*)(me))->last = ((hsm_obj_t*)(me))->current, \
    ((hsm_obj_t*)(me))->current = (target), \
    HSM_RET_TRAN_KEEP)

/* 恢复被 HSM_TRAN_KEEP 保留的业务状态：
 * 只对当前叠加态发送 HSM_EXIT_SIG，不重发业务链的 HSM_ENTRY_SIG，
 * 因此被保留的业务（如录像）是无缝续上的，不会重启。
 * 【禁令】只能在 HSM_TRAN_KEEP 叠加期间调用；无叠加态时调用属于误用，
 * 引擎会断言告警并保持原状态不变（不会崩溃）。 */
#define HSM_RESUME() (((hsm_obj_t*)(me))->last = ((hsm_obj_t*)(me))->current, \
                      ((hsm_obj_t*)(me))->current = ((hsm_obj_t*)(me))->saved, \
                      ((hsm_obj_t*)(me))->saved = NULL, HSM_RET_RESUME)

#define HSM_STATE_FUNC(func) (state_func_t)(func)

/* 断言：默认仅打印日志（原版为 while(1) 死循环，量产不安全）。
 * 业务侧可在编译选项中自定义覆盖，例如接入 DVR_SDK 日志模块。 */
#ifndef HSM_ASSERT
#define HSM_ASSERT(cond)  \
    do{ if(!(cond)){ fprintf(stderr, "[HSM][ASSERT] %s failed at %s:%d\n", \
                     #cond, __FILE__, __LINE__); } }while(0)
#endif

#if   defined ( __CC_ARM )
	#define __STATIC_INLINE static __inline
#else
	#define __STATIC_INLINE static inline
#endif

/* 注意：内部使用 static 事件缓存，仅允许在 HFSM 单线程（派发线程）上下文中使用。
 * 多线程投递事件请使用上层封装的消息队列接口（见 thumcam_hsm.h）。 */
__STATIC_INLINE hsm_event_t* hsm_event_make(HSM_SIG_T sig, void *param){
    static hsm_event_t _event = {0, NULL};
    _event.sig = sig;
    _event.param = param;
	return &_event;
}


/*顶层状态声明*/
extern const hsm_t top_state;

void hsm_init(hsm_obj_t *me, hsm_t *init_state);
void hsm_dispatch(hsm_obj_t *me, hsm_event_t *e);
bool hsm_is_child(const hsm_t *parent, const hsm_t *child);

#ifdef __cplusplus
}
#endif
#endif
