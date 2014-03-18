#ifndef     __LIST_H__
#define     __LIST_H__

#include "io.h"

struct list_head{
    struct list_head *next, *prev;
};

#define     LIST_HEAD_INIT(name) {&(name), &(name)}

/*
    stuct list_head name = {&(name), &(name)}
*/

#define     LIST_HEAD(name) \
    struct list_head name = LIST_HEAD_INIT(name)

static inline void INIT_LIST_HEAD(struct list_head *list_head)
{
    list_head->prev = list_head;
    list_head->next = list_head;
}

static inline void __list_add(struct list_head *new, struct list_head *prev, struct list_head *next)
{
    next->prev = new;
    new->next = next;
    prev->next = new;
    new->prev = prev;
}

static inline void list_add(struct list_head *new, struct list_head *head)
{
    __list_add(new, head, head->next);
}

static inline void list_add_tail(struct list_head *new, struct list_head *head)
{
    __list_add(new, head->prev, head);
}

static inline void __list_del(struct list_head *prev, struct list_head *next)
{
    prev->next = next;
    next->prev = prev;
}

static inline void list_del(struct list_head *entry)
{
    __list_del(entry->prev, entry->next);
    entry->next = (void *)0;
    entry->prev = (void *)0;
}

static inline int list_is_last(struct list_head *entry, struct list_head *head)
{
    return entry->next == head;
}

static inline int list_empty(struct list_head *head)
{
    return head->next == head;
}


#define list_entry(ptr, type, member)   \
    container_of(ptr, type, member)

/* do not use this macro to delete list entry */
#define list_for_each(pos, head)    \
    for (pos = (head)->next;pos != (head); pos = pos->next)

#define list_for_each_safe(pos, n, head)      \
    for (pos = (head)->next, n = pos->next; pos != (head);pos = n, n = pos -> next)

#define list_for_each_entry(pos, head, member)  \
    for (pos = list_entry((head)->next, typeof(*pos), member); &pos->member != (head); \
        pos = list_entry(pos->member.next, typeof(*pos), member))

#define list_for_each_entry_safe(pos, n, head, member) \
    for (pos = list_entry((head)->next, typeof(*pos), member), n = list_entry(pos->member.next, typeof(*pos), member); \
        &pos->member != (head);\
        pos = n, n = list_entrt(pos->member.next, typeof(*pos), member))
#endif
