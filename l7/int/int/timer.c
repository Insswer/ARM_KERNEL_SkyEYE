#include "timer.h"
#include "io.h"
#include "int_control.h"
#include "exception.h"
#include "init.h"

/* 时钟中断处理函数 */
static int sys_tick(void *private_data)
{
    printf("SYS TICK\n");
    return 0;
}

/* 初始化时钟中断 */
static void  init_sys_timer(void)
{

}

arch_initcall(init_sys_timer);


