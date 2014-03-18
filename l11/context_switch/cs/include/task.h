#ifndef   __TASK_H__
#define   __TASK_H__

#include "arch/exception.h"




struct thread_info{
	unsigned long sp;
	unsigned long entry_point;
	struct thread_info *next;
};

#define	TASK_SIZE	4096

#define disable_schedule()	kernel_disable_irq()
#define enable_schedule()	kernel_enable_irq()

struct thread_info *current_thread_info(void);
struct thread_info *copy_thread_info (void);
#define current	current_thread_info()

#define DO_INIT_SP(sp, fn, args, lr, cpsr, pt_base)				\
	do {														\
		(sp) = (sp) - 4;										\
		*(volatile unsigned long *)(sp) = (unsigned long)(fn);	\
		(sp) = (sp) - 4;										\
		*(volatile unsigned long *)(sp) = (unsigned long)(lr);	\
		(sp) = (sp) - 4*13;										\
		*(volatile unsigned long *)(sp) = (unsigned long)(args);\
		(sp) = (sp) - 4;										\
		*(volatile unsigned long *)(sp) = (unsigned long)(cpsr);\
	}while(0)

unsigned long get_cpsr(void);

void *__common_schedule(void);
int do_fork(unsigned long entry_point, void *args);
int task_init(void);


#endif
