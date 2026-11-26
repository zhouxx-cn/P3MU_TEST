/**
 * @file    hsm.c
 * @brief   Hierarchical state machines for designing event-driven systems.
 * @copyright  (c) 2024, yezenghui, <y956370541@163.com>
 * @license MIT
 * @note    基于 doc/hsm-master 开源框架拷贝，引擎逻辑与原版保持一致。
 *          仅允许 HFSM 单线程（见 thumcam_hsm.c 派发线程）调用 hsm_dispatch，
 *          多线程并发派发会破坏 current/last 一致性。
 *
 *          适配改动（相对原版）：
 *          1. 新增"正交区域/浅历史"跳转：HSM_TRAN_KEEP / HSM_RESUME，
 *             进入叠加态（屏保）时不退出源业务状态链，源业务（录像）不中断；
 *          2. 事件上浮时增加业务链拼接：叠加态未处理的事件先交给被保留的
 *             业务链，避免业务事件（分片结束、停止录像等）丢失；
 *          3. ENTRY/EXIT 回调内禁止发起状态跳转（否则破坏 current/last 一致性）。
 */

#include "hsm.h"

/*状态跳转模式*/
#define TRAN_NORMAL 0  /*常规跳转：退出源状态链 + 进入目标状态链*/
#define TRAN_KEEP   1  /*保留跳转：不退出任何状态链，只进入目标状态链*/
#define TRAN_RESUME 2  /*恢复跳转：只退出叠加态链，不重入被保留的业务链*/


static HSM_RET_T top_state_func(hsm_obj_t *me, hsm_event_t *e){
    (void)me;
    (void)e;
    /* 根状态兜底：吞掉一切未处理事件，防止指针越界到 NULL 父状态 */
    return HSM_RET_HANDLED;
}

const hsm_t top_state = {
    .name = "top_state",
    .parent = NULL,
    .state_func = top_state_func
};


/*求 source 与 target 的最近公共祖先（source 的某个祖先，同时也是 target 的祖先）*/
static const hsm_t *hsm_lca(const hsm_t *source, const hsm_t *target){
    const hsm_t *p = source;

    if(source == target){
        /*同一个状态：公共祖先就是它自己（hsm_is_child 不把自身视为自己的祖先）*/
        return source;
    }

    while(p != NULL && p != &top_state && !hsm_is_child(p, target)){
        p = p->parent;
    }

    return p;
}

/*状态在状态树中的深度（top_state 深度为 1）*/
static int hsm_depth(const hsm_t *s){
    int d = 0;

    while(s != NULL){
        d++;
        s = s->parent;
    }

    return d;
}

/*收集本次需要退出的状态：
 *   source 链（当前状态链）+ saved 链（被保留的业务状态链）的并集，
 *   去掉仍属于目标状态链（target 本身或其祖先）的状态，
 *   再按"深→浅"排序，保证先退子状态、后退父状态，且同一状态只退一次。
 * 说明：两条链可能有公共祖先（例如都挂在 WORKING 下），若分别求退出链会
 *      导致公共祖先被退出两次（父状态先于子状态退出），故这里统一去重排序。*/
static int hsm_collect_exit(const hsm_t *source, const hsm_t *saved, const hsm_t *target,
                            const hsm_t **out, int max){
    const hsm_t *chains[2];
    int nchain = 0;
    int n = 0;
    int i, j;

    chains[nchain++] = source;
    if(saved != NULL){
        chains[nchain++] = saved;
    }

    for(i = 0; i < nchain; i++){
        const hsm_t *p = chains[i];

        while(p != NULL){
            int dup = 0;

            for(j = 0; j < n; j++){
                if(out[j] == p){
                    dup = 1;
                    break;
                }
            }

            /*属于目标状态链的状态保持运行，不退出*/
            if(!dup && p != target && !hsm_is_child(p, target)){
                if(n < max){
                    out[n++] = p;
                }
            }

            p = p->parent;
        }
    }

    /*按深度从深到浅排序（稳定插入排序）：父状态必须在子状态之后退出*/
    for(i = 1; i < n; i++){
        const hsm_t *key = out[i];
        int kd = hsm_depth(key);

        j = i - 1;
        while(j >= 0 && hsm_depth(out[j]) < kd){
            out[j + 1] = out[j];
            j--;
        }
        out[j + 1] = key;
    }

    return n;
}

/*发送 ENTRY/EXIT 事件，并防止回调内非法发起状态跳转*/
static void hsm_call_state(hsm_obj_t *me, const hsm_t *s, HSM_SIG_T sig){
    hsm_t *guard = me->current;

    s->state_func(me, hsm_event_make(sig, NULL));

    if(me->current != guard){
        /* ENTRY/EXIT 回调内禁止 HSM_TRAN，否则会破坏当前转换的 current/last 一致性 */
        HSM_ASSERT(0);
        me->current = guard;
        me->last = guard;
    }
}


static void hsm_tran_ex(hsm_obj_t *me, int mode){

    int idx = 0;
    const hsm_t *chain[HSM_MAX_LEVEL] = {NULL};
    hsm_t *source = me->last;
    hsm_t *target = me->current;
    hsm_t *origin;         /*实际仍在运行的最深层状态*/
    const hsm_t *lca_in;   /*进入链的公共祖先*/

    if(target == NULL){
        /* 防护：无叠加态时调用了 HSM_RESUME()（HSM_RESUME 宏把 current 置成了
         * saved=NULL）。回退为保持原状态，避免后续派发/查询空指针崩溃。
         * 约束：HSM_RESUME() 只允许在 HSM_TRAN_KEEP 叠加期间使用。 */
        HSM_ASSERT(0);
        me->last = source;
        me->current = source;
        return;
    }

    if(me->state_tran_cb){
        me->state_tran_cb(me->last, me->current);
    }

    if(source == target){
        /*自跳转：不退出也不进入*/
        return;
    }

    /*存在保留跳转时，被保留的业务链仍然在运行，进入链要相对它计算*/
    origin = (me->saved != NULL) ? me->saved : source;
    lca_in = hsm_lca(origin, target);

    /* 1. 退出链：source 链 + saved 链的并集（去重、深→浅，保证父状态最后退出） */
    if(mode != TRAN_KEEP){

        const hsm_t *exit_list[HSM_MAX_LEVEL * 2] = {NULL};
        int n = hsm_collect_exit(source, me->saved, target, exit_list, HSM_MAX_LEVEL * 2);
        int i;

        for(i = 0; i < n; i++){
            hsm_call_state(me, exit_list[i], HSM_EXIT_SIG);
        }

        me->saved = NULL;
    }

    if(mode == TRAN_RESUME){
        /*恢复：被保留的业务链仍在运行，不重发 ENTRY_SIG*/
        return;
    }

    /* 2. 收集 target 侧需要进入的状态链（自顶向下入栈） */
    {
        const hsm_t *p = target;
        while(p != NULL && p != lca_in){

            HSM_ASSERT(idx < HSM_MAX_LEVEL);

            chain[idx++] = p;

            p = p->parent;
        }
    }

    /* 3. 自顶向下依次进入（先父后子） */
    while (idx > 0) {
        hsm_call_state(me, chain[--idx], HSM_ENTRY_SIG);
    }

    /* 4. 最内层状态若返回 TRAN，则继续下钻到其默认子状态 */
    if(chain[0]){
        if(HSM_RET_TRAN == chain[0]->state_func(me, hsm_event_make(HSM_INIT_SIG, NULL))){
            hsm_tran_ex(me, mode);
        }
    }
}

void hsm_init(hsm_obj_t *me, hsm_t *init_state) {

    const hsm_t *chain[HSM_MAX_LEVEL] = {NULL};
    const hsm_t *h = init_state;
    int idx = 0;

    me->current = init_state;
    me->last = init_state;
    me->saved = NULL;

    /* 自底向上收集初始状态链，再自顶向下进入 */
    for (; h != NULL; h = h->parent) {
        HSM_ASSERT(idx < HSM_MAX_LEVEL);
        chain[idx++] = h;
    }

    while (idx > 0) {
        hsm_call_state(me, chain[--idx], HSM_ENTRY_SIG);
    }

    if(HSM_RET_TRAN == me->current->state_func(me, hsm_event_make(HSM_INIT_SIG, NULL))){
        hsm_tran_ex(me, TRAN_NORMAL);
    }
}

bool hsm_is_child(const hsm_t *parent, const hsm_t *child) {
    const hsm_t *h = child->parent;
    for (; h != NULL; h = h->parent) {
        if(h == parent) {
            return true;
        }
    }

    return false;
}

void hsm_dispatch(hsm_obj_t *me, hsm_event_t *e){
    HSM_RET_T ret = HSM_RET_HANDLED;
    const hsm_t *h = me->current;

    if(!e){
        return;
    }

    if(h == NULL){
        /* 防护：current 为空说明叠加态被破坏（如无叠加态时误调 HSM_RESUME），
         * 丢弃本条事件，等待引擎自愈（见 hsm_tran_ex 的回退逻辑） */
        HSM_ASSERT(0);
        return;
    }

    /* 事件上浮：当前状态返回 UNHANDLED 则交给父状态，直到根状态 */
    for(;;){
        ret = h->state_func(me, e);
        if(ret != HSM_RET_UNHANDLED){
            break;
        }

        h = h->parent;
        if(h == NULL){
            break;
        }

        /* 叠加态（屏保）场景：走到叠加态的父状态之前，先让被保留的业务链有机会处理，
         * 否则 MSG_REC_SEGMENT_DONE / MSG_REC_STOP 等业务事件会被上层吞掉丢失。 */
        if(me->saved != NULL && h == me->current->parent){
            const hsm_t *s = me->saved;
            while(s != NULL && s != h){
                ret = s->state_func(me, e);
                if(ret != HSM_RET_UNHANDLED){
                    break;
                }
                s = s->parent;
            }
            if(ret != HSM_RET_UNHANDLED){
                break;
            }
        }
    }

    if(ret == HSM_RET_TRAN) {
        hsm_tran_ex(me, TRAN_NORMAL);
    } else if(ret == HSM_RET_TRAN_KEEP) {
        hsm_tran_ex(me, TRAN_KEEP);
    } else if(ret == HSM_RET_RESUME) {
        hsm_tran_ex(me, TRAN_RESUME);
    }
}
