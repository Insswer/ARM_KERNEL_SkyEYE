#include "asm.h"
#include "io.h"
#include "mmu.h"
#include "swi.h"
#include "int_control.h"
#include "timer.h"
#include "init.h"


void  start_kernel(void)
{
    init_mmu();
    init_arch();
    __enable_int();
    for (;;);
}
