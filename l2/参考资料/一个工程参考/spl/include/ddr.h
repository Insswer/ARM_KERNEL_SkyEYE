#ifndef	__DDR_H__
#define	__DDR_H__


#include <asm/cpu.h>


void ddr_init(void);
void do_ddr_init(struct ddr_regs *emif1_ddr_regs,struct ddr_regs *emif2_ddr_regs);

#endif
