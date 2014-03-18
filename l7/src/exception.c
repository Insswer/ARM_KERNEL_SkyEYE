#include "exception.h"
#include "io.h"
#include "int_control.h"

#define    UND_EXCEPTION             0x1
#define    PREFETCH_ABORT_EXCEPTION  0x2
#define    NOTUSED_EXCEPTION         0x3
#define    DATA_ABORT_EXCEPTION      0x4
#define    FIQ_EXCEPTION             0x5

void debug_exception(unsigned long addr, int kind)
{
    switch(kind) {
    case UND_EXCEPTION:
        printf("UNDEFINE EXCEPTION: at 0x%08x\n", addr);
        break;
    case PREFETCH_ABORT_EXCEPTION:
        printf("PREFETCH ABORT: at 0x%08x\n", addr);
        break;
    case NOTUSED_EXCEPTION:
        printf("how can this happen...\n");
        break;
    case DATA_ABORT_EXCEPTION:
        printf("DATA ABORT EXCEPTION: at 0x%08x\n", addr);
        break;
    case FIQ_EXCEPTION:
        printf("FIQ EXCEPTION...\n");
        break;
    default:
        printf("UNKNOWN EXCEPTION...\n");
    }
}


void do_irq(void)
{
    unsigned int int_num;
    int ret;
    struct irq_desc *pIrq_desc;
    int_num = read_int();
    if (int_num == -1)
        return;
    ack_int(int_num);
    pIrq_desc = &irq_descs[int_num];
    if (pIrq_desc->irq_exist)
        ret =  pIrq_desc->irq_isr(pIrq_desc->private_data);

    if (ret != 0)
        printf("isr error...\n");
}

void debug(void)
{
    printf_raw("hello\n");
    for(;;);
}


void swi_exception(int swi_num)
{
    printf("swi %d\n",swi_num);
}
