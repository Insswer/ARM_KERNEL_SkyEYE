#include "int_control.h"
#include "asm.h"
#include "io.h"
#include "exception.h"
#include "init.h"
#include "list.h"

/* 我们先不考虑共享中断的情况 */
struct irq_desc irq_descs[32];

/* 由于没有动态内存管理此处我们先简单的静态分配内存 */
struct irq_action irq_act;

static void init_int_cont(void)
{
    unsigned long tmp;

    printf("init system tick...\n");

    /* disable all interrupt in core */
    __disable_int();

    /* clear the pending int */
    write32(0xffffffff, SRCPND);

    /* use irq only */
    tmp = read32(INTMOD);
    tmp &=  ~(0xffffffff);
    write32(tmp, INTMOD);

    /* disable all interrupt in int_cont */
    tmp = read32(INTMSK);
    tmp |=  (0xffffffff);
    write32(tmp, INTMSK);
}

arch_initcall(init_int_cont);

void ack_int(unsigned int num)
{
    if (num > 31)
        return;

    /* clear SRCPND */
    write32((1 << num), SRCPND);

    /* clear INTPEND */
    write32((1 << num), INTPEND);
}

int read_int(void)
{
    unsigned long tmp, tmp2;
    tmp = read32(INTOFFSET);
    tmp2 = read32(INTPEND);

    if (!(tmp2 & (1 << tmp))) {
        printf("pony interrupt...\n");
        return -1;
    }

    return (int)tmp;
}

void enable_int(unsigned int irq_num)
{
    unsigned long tmp;
    tmp = read32(INTMSK);
    tmp &= ~(1 << irq_num);
    write32(tmp, INTMSK);
}

void disable_int(unsigned int irq_num)
{
    unsigned long tmp;
    tmp = read32(INTMSK);
    tmp |= (1 << irq_num);
    write32(tmp, INTMSK);
}


int request_irq(unsigned int irq_num, int (*irq)(void *),unsigned int flag, void *private_data)
{
    struct irq_desc *pIrq_desc;

    if (irq_num > 31) {
        printf("bad irq num...\n");
        goto failed;
    }

    if (flag != IRQF_NON_SHARED && flag != IRQF_SHARED && flag != IRQF_DISABLED) {
        printf("bad flag...\n");
        goto failed;
    }

    pIrq_desc = &irq_descs[irq_num];

    if (pIrq_desc->irq_exist == 1) {
        printf("overwrite irq%d \n",irq_num);
        goto failed;
    }

    INIT_LIST_HEAD(&pIrq_desc->irq_head);

    pIrq_desc->irq_exist = 1;
    pIrq_desc->irq_num = irq_num;
    pIrq_desc->flag = flag;

    /* 有动态内存管理后，此处应该被替换为动态分配内存 */
    irq_act.irq_isr = irq;
    list_add(&irq_act.irq_share_lists, &pIrq_desc->irq_head);

    if (private_data != NULL)
        pIrq_desc->private_data = private_data;

    enable_int(irq_num);
    return 0;
failed:
    return -1;
}
