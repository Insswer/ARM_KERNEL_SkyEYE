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

/* 开辟一个新的空间来存放我们的thread_info，相当于创建一个任务的pcb */
struct thread_info *copy_thread_info (void)
{
	return NULL;
}


/* 初始化任务，这个任务基本没啥用，一半操作系统中当作idle任务使用 */
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


/* 创建init任务，我们不关心这个 */
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

/* 任务管理初始化 */
int task_init(void)
{
	set_init_task();
	return 0;
}


/* 这个函数内部会跳转到任务的入口处去运行任务 */
int do_tsk(void *args)
{

}


/* 创建一个任务，需要两个参数，一个入口函数，一个参数 */
int do_fork(unsigned long entry_point, void *args)
{

	return 0;
}
/* 超简单的调度函数，以后我们会丰富它 */
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

