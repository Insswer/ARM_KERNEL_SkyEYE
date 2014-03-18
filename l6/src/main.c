#include "asm.h"
#include "io.h"
#include "mmu.h"

void  start_kernel(void)
{
    init_mmu();
    printf("hello world\n");
}
