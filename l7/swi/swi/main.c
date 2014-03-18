#include "asm.h"
#include "io.h"
#include "mmu.h"
#include "swi.h"
#include "init.h"



void  start_kernel(void)
{
    init_mmu();

    for (;;);
}
