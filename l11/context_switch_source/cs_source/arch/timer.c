#include "arch/timer.h"
#include "arch/io.h"
#include "arch/int_control.h"
#include "arch/exception.h"
#include "arch/asm.h"
#include "init.h"
#include "task.h"

static unsigned long tick = 0;
static int sys_timer_init_complete = 0;

extern void __sched_in_irq(void);

static void ack_timer(void)
{
	ack_int(IRQ_TIMER4);
}

static int sys_tick(void *private_data)
{
	ack_timer();
    printf("SYS TICK %lu\n", ++tick);
    __sched_in_irq();
    return 0;
}


void  init_sys_timer(void)
{
    unsigned long tmp;

    tmp = read32(TCFG0);
    tmp &= ~(0xff << 8);
    tmp |= 0x800;
    write32(tmp, TCFG0);

    tmp = read32(TCON);
    tmp &= ~(0x7 << 20);
    tmp |= (1 << 22);
    tmp |= (1 << 21);
    write32(tmp, TCON);
    write32(0x50000000, TCONB4);

    sys_timer_init_complete = 1;

    if (sys_timer_init_complete) {

        tmp = read32(TCON);
        tmp |= (0x1 << 20);
        tmp &= ~(0x1 << 21);
        write32(tmp, TCON);

        if (0 != request_irq(IRQ_TIMER4, sys_tick, IRQF_DISABLED, NULL))
            printf("regist tick irq failed...\n");
    	} else
    		printf("system timer not init...\n");

    printf("system timer init done...\n");

}




