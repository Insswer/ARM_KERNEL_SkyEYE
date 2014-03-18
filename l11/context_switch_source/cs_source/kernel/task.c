#include "task.h"
#include "arch/io.h"
#include "arch/exception.h"
#include "slab.h"
#include "arch/asm.h"

unsigned long task_stack_base = (STACK_END + 0x1000 - 1 ) & (~ (0x1000 - 1));
static struct thread_info *init_task;
static struct thread_info *cur_task;

/* 巧妙的通过当前任务堆栈找到任务
 *
 * ----------------------- <--sp
 *
 *
 *
 *
 * ----------------------- <--thread_info
 *
 * */
struct thread_info *current_thread_info(void) {
	register unsigned long sp asm ("sp");
	return (struct thread_info *)(sp &~(4096 - 1));
}

struct thread_info *copy_thread_info (void)
{
	struct thread_info *tmp = (struct thread_info *) task_stack_base;
	task_stack_base += TASK_SIZE;
	return tmp;
}

void init_task_entry(void)
{
	int i;
	for(;;) {
		kernel_disable_irq();
		printf("init task..\n");
		kernel_enable_irq();
		for(i = 0; i < 0xffff; i++);
	}
}

int do_tsk(void *args);

void set_init_task(void)
{
	struct thread_info *tsk, *tmp;
	if ((tsk = copy_thread_info()) == (void *)0)
			return ;
	tsk->sp = ((unsigned long)(tsk) + TASK_SIZE);
	tsk->entry_point = (unsigned long)init_task_entry;
	DO_INIT_SP(tsk->sp, do_tsk, 0, 0, 0xdf|get_cpsr(), 0);
	cur_task = tsk;
	tsk->next = tsk;
	printf("tsk: 0x%08x 0x%08x 0x%08x\n",tsk->sp, tsk->next, tsk->entry_point);
}


int task_init(void)
{
	set_init_task();
	return 0;
}

int do_tsk(void *args)
{
	struct thread_info *task;
	int (*ptr)(void *);
	int ret;
	task = current_thread_info();
	ptr = (int (*)(void *))(task->entry_point);
	printf("in do tsk, args is %d task is 0x%08x 0x%08x 0x%08x\n", (int)args, task, task->next, task->entry_point);
	kernel_enable_irq();
	ret = (*ptr)(args);
	return 0;
}

int do_fork(unsigned long entry_point, void *args)
{
	struct thread_info *tsk, *tmp;
	if (entry_point == 0)
		return -1;
	if ((tsk = copy_thread_info()) == (void *)0)
		return -1;
	tsk->sp = ((unsigned long)(tsk) + TASK_SIZE);
	tsk->entry_point = entry_point;
	DO_INIT_SP(tsk->sp, do_tsk, args, 0, 0xdf|get_cpsr(), 0);

	tmp = cur_task->next;
	cur_task->next = tsk;
	tsk->next = tmp;
	printf("tsk: 0x%08x 0x%08x 0x%08x\n",tsk, tsk->next, tsk->entry_point);
	return 0;
}

void *__common_schedule(void)
{
	int i;
//	unsigned long sp;
	struct thread_info *task;
//	sp = cur_task->sp;
/*
	for (i = 0; i < 16; i++) {
		printf("0: 0x%08x\n",*(unsigned long *)sp);
		sp+= 4;
	}
*/

	task = cur_task->next;
	cur_task = task;
//	printf("jmp to task 0x%08x\n",cur_task);
/*	sp = cur_task->sp;
	for (i = 0; i < 16; i++) {
		printf("0: 0x%08x\n",*(unsigned long *)sp);
		sp+= 4;
	}
*/
	return (void *)(task);
}

