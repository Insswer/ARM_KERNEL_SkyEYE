#include "exception.h"
#include "io.h"
#include "int_control.h"
#include "list.h"
#include "asm.h"

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

unsigned int identify_and_clear(void)
{
    unsigned int int_num;
    int_num = read_int();
    ack_int(int_num);
    return int_num;
}

void do_irq(unsigned int int_num)
{
    struct irq_desc *pIrq_desc;
    struct irq_action *pIrq_act;
    struct list_head *lst_pointer;
    int ret;

    pIrq_desc = &irq_descs[int_num];

    if (!(pIrq_desc->irq_exist))
        ret = -1;

    if (pIrq_desc->flag & IRQF_DISABLED)
        __disable_int();


    list_for_each_entry (pIrq_act, &pIrq_desc->irq_head, irq_share_lists) {

        if (pIrq_act == NULL) {
            printf("irq_act not exist!\n");
            ret = -1;
        }

        ret = pIrq_act->irq_isr(pIrq_desc->private_data);

        if (ret == IRQ_HANDLED)
            break;
        else {
            if (pIrq_desc->flag & IRQF_SHARED)
                continue;
            else {
                ret = -1;
                break;
            }
        }
    }

    if (pIrq_desc->flag & IRQF_DISABLED)
        __enable_int();

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
