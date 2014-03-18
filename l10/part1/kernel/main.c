#include "arch/asm.h"
#include "arch/io.h"
#include "arch/mmu.h"
#include "swi.h"
#include "arch/int_control.h"
#include "arch/timer.h"
#include "init.h"
#include "list.h"

extern initcall_t	__init_start_arch[], __init_end_arch[];
extern initcall_t   	__init_start_subsys[],__init_end_subsys[];

struct list_group {
    int id;
    struct list_head list;
};

LIST_HEAD(list_group_head);
struct list_group list_groups[10];

static test_list()
{
    int i;
    struct list_group *pList;
    for (i = 0;i < 10; i++) {
        list_groups[i].id = i;
        list_add(&list_groups[i].list, &list_group_head);
    }

    list_for_each_entry(pList, &list_group_head, list) {
        printf("LIST ID : %d\n", pList->id);
    }

    printf("TEST DELETE LIST\n");
    list_del(&list_groups[0].list);

    list_for_each_entry(pList, &list_group_head, list) {
        printf("LIST ID : %d\n", pList->id);
    }

    printf("test done...\n");
}

static void init_arch(void)
{
    initcall_t *call;
	for (call = __init_start_arch; call < __init_end_arch; call++) {
		(*call)();
	}
}

static void init_subsys(void)
{
    initcall_t *call;
	for (call = __init_start_subsys; call < __init_end_subsys; call++) {
		(*call)();
	}
}

void  start_kernel(void)
{
    unsigned long i;
    init_mmu();
    test_list();
    init_arch();
    init_subsys();
#if 0
    __enable_int();
#endif

    for(;;);
}
