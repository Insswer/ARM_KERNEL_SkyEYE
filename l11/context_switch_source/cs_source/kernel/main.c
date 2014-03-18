#include "arch/asm.h"
#include "arch/io.h"
#include "arch/mmu.h"
#include "swi.h"
#include "arch/int_control.h"
#include "arch/timer.h"
#include "init.h"
#include "list.h"
#include "page.h"
#include "task.h"
#include "slab.h"

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

static void test_mem(void)
{
    /*
    void *addrs[10];
    int i;

    for (i = 0; i < 10; i++) {
        addrs[i] = kmalloc(1 << (i+1));
        printf("addrs[%d]->0x%08x\n",i,addrs[i]);
    }
    */
    void *p,*q;
    p = kmalloc(2 * 1024 * 1024);
    q = kmalloc(2 * 1024 * 1024);
    printf("p is 0x%08x\n", p);
    printf("q is 0x%08x\n", q);
    kfree(p);
    kfree(q);
}

int  task1(void *args)
{
	printf("in task1...\n");
	for (;;);
	return 0;
}

int task2(void *args)
{
	printf("int task2...\n");
	for (;;);
	return 0;
}

void delay(void){
		volatile unsigned long time = 0xffff;
		while (time--);
}

int test_process(void *p)
{
	while(1) {
			kernel_disable_irq();
			printf("test process %d\n", (int)p);
			kernel_enable_irq();
			delay();
	}
	return 0;
}

void  start_kernel(void)
{
    unsigned long i;
    init_mmu();
    test_list();
    init_arch();
    init_subsys();
    kernel_disable_irq();
    task_init();
//  test_mem();
    i = do_fork((unsigned long)test_process, (void *)0x1);
    if (i == -1)
    	ERROR();
    i = do_fork((unsigned long)test_process, (void *)0x2);
    if (i == -1)
    	ERROR();
    i = do_fork((unsigned long)test_process, (void *)0x3);
    if (i == -1)
    	ERROR();
    init_sys_timer();
    kernel_enable_irq();

    for(;;);
}
