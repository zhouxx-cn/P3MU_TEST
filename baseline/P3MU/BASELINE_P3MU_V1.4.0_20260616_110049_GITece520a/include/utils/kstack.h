#ifndef __KSTACK_H__
#define __KSTACK_H__

#include <stdio.h>
#include "klist.h"

static inline void kstack_init(struct list_head *stack)
{
    INIT_LIST_HEAD(stack);
}

static inline void kstack_push(struct list_head *stack, struct list_head *node)
{
    list_add(node, stack);
}

static inline struct list_head* kstack_pop(struct list_head *stack)
{
    struct list_head *node = NULL;

    if (!list_empty(stack))
    {
        node = stack->next;
        list_del(node);
    }

    return node;
}

static inline int kstack_size(struct list_head *stack)
{
    struct list_head *pos = NULL;
    int size = 0;

    list_for_each(pos, stack)
    {
        size++;
    }

    return size;
}

#endif