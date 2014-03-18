#include "int_control.h"
#include "asm.h"
#include "io.h"
#include "exception.h"
#include "init.h"

/* 我们先不考虑共享中断的情况 */
struct irq_desc irq_descs[32];

/* 初始化中断控制器 */
static void init_int_cont(void)
{

}


/* 编写中断应答函数 */
void ack_int(unsigned int num)
{

}


/* 读取中断类型 */
int read_int(void)
{
	return 0;
}


/* 使能某个中断 */
void enable_int(unsigned int irq_num)
{

}


/* 失能某个中断 */
void disable_int(unsigned int irq_num)
{

}


/* 注册一个中断 */
int request_irq(unsigned int irq_num, int (*irq)(void *), void *private_data)
{
   return 0;
}

/* 声明初始化调用函数 */
arch_initcall(init_int_cont);
