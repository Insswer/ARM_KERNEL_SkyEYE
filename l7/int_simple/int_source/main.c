#include "asm.h"
#include "io.h"
#include "mmu.h"
#include "swi.h"
#include "int_control.h"
#include "timer.h"
#include "init.h"

extern initcall_t	__init_start_arch[], __init_end_arch[];
extern initcall_t   __init_start_subsys[],__init_end_subsys[];

static void init_arch(void)
{
    initcall_t *call;
	for (call = __init_start_arch; call < __init_end_arch; call++) {
		(*call)();
	}
}

void  start_kernel(void)
{
    init_mmu();
    init_arch();
    __enable_int();
    for (;;);
}
