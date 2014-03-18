#include "timer.h"
#include "io.h"
#include "int_control.h"
#include "exception.h"
#include "init.h"

static int sys_tick(void *private_data)
{
    printf("SYS TICK\n");
    return 0;
}


static void  init_sys_timer(void)
{
    unsigned long tmp;

    tmp = read32(TCFG0);
    tmp &= ~(0xff << 8);
    tmp |= (PRESCALE << 8);
    write32(tmp, TCFG0);

    tmp = read32(TCON);
    tmp &= ~(0x7 << 20);
    tmp |= (1 << 22);
    tmp |= (1 << 21);
    write32(tmp, TCON);
    write32(10000, TCONB4);

    tmp = read32(TCON);
    tmp |= (0x1 << 20);
    tmp &= ~(0x1 << 21);
    write32(tmp, TCON);

    if (0 != request_irq(IRQ_TIMER4, sys_tick, NULL))
        printf("regist tick irq failed...\n");

    printf("system timer init done...\n");

}

arch_initcall(init_sys_timer);


