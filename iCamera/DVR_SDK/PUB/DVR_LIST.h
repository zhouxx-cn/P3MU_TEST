
/*!
 *\file		DVR_LIST.h
 *\brief	双向链表定义
 *\author	yuk@higheasyrd.com
 *\date		2009/04/27

说明：双向链表,用于一些对象的管理
*/

#ifndef _DVR_LIST_H
#define _DVR_LIST_H

//!\brief 双向链表头
struct dvr_list_head
{
	struct dvr_list_head *next, *prev;
};

//!\brief 初始化链表
#define DVR_LIST_HEAD_INIT(name) { &(name), &(name) }

//!\brief 声明一个链表并初始化
#define DVR_LIST_HEAD(name) \
	struct dvr_list_head name = LIST_HEAD_INIT(name)

#define INIT_DVR_LIST_HEAD(ptr) do { \
	(ptr)->next = (ptr); (ptr)->prev = (ptr); \
} while (0)

//!\brief 在两个节点之间添加一个新的节点
//!\param entry 要添加的节点的指针
//!\param prev 前节点的指针
//!\param next 后节点的指针
//!\return 无
static __inline__ void __dvr_list_add(struct dvr_list_head *entry, struct dvr_list_head *prev, struct dvr_list_head *next)
{
	next->prev = entry;
	entry->next = next;
	entry->prev = prev;
	prev->next = entry;
}

//!\brief 在头结点后面插入节点
//!\param 要插入的节点
//!\param head 头结点
//!\return 无
static __inline__ void dvr_list_add(struct dvr_list_head *entry, struct dvr_list_head *head)
{
	__dvr_list_add(entry, head, head->next);
}

//!\brief 在链表尾部插入节点
//!\param 要插入的节点
//!\param head 头结点
//!\return 无
static __inline__ void dvr_list_add_tail(struct dvr_list_head *entry, struct dvr_list_head *head)
{
	__dvr_list_add(entry, head->prev, head);
}

//!\brief 删除在两个节点之间的节点
//!\param prev 前节点的指针
//!\param next 后节点的指针
//!\return 无
//!\note 中间的节点的资源并未释放
static __inline__ void __dvr_list_del(struct dvr_list_head *prev, struct dvr_list_head *next)
{
	next->prev = prev;
	prev->next = next;
}

//!\brief 删除指定的节点
//!\param entry 要删除的节点的指针
//!\return 无
//!\note 删除的节点的资源并未释放
static __inline__ void dvr_list_del(struct dvr_list_head *entry)
{
	__dvr_list_del(entry->prev, entry->next);
	entry->next = entry->prev = 0;
}

//!\brief 删除节点，重新初始化
//!\param entry 要删除的节点
static __inline__ void dvr_list_del_init(struct dvr_list_head *entry)
{
	__dvr_list_del(entry->prev, entry->next);
	INIT_DVR_LIST_HEAD(entry);
}

//!\brief 判断链表是否为空
//!\param head 链表头
//!\return 1表示链表为空 0表示不为空
static __inline__ int dvr_list_empty(struct dvr_list_head *head)
{
	return (head->next == head);
}

//!\brief 合并两个链表
//!\param list 要添加的新的链表
//!\param head 要添加到的链表
//!\return 无
static __inline__ void dvr_list_splice(struct dvr_list_head *list, struct dvr_list_head *head)
{
	struct dvr_list_head *first = list->next;

	if (first != list)
	{
		struct dvr_list_head *last = list->prev;

		struct dvr_list_head *at = head->next;

		first->prev = head;
		head->next = first;

		last->next = at;
		at->prev = last;
	}
}

//!\brief 获取包含链表头结构的结构体指针
#define dvr_list_entry(ptr, type, member) \
	((type *)((char *)(ptr)-(unsigned long)(&((type *)0)->member)))

//!\brief 遍历链表
#define dvr_list_for_each(pos, head) \
	for (pos = (head)->next; pos != (head); \
        	pos = pos->next)

//!\brief 安全遍历链表
#define dvr_list_for_each_safe(pos, n, head) \
	for (pos = (head)->next, n = pos->next; pos != (head); \
		pos = n, n = pos->next)

#endif
