#ifndef __EMIF_H__
#define __EMIF_H__
 
#include <asm/cpu.h>
 
void reset_phy(unsigned int base);
void emif_config(unsigned int base, const struct ddr_regs *ddr_regs);


#endif
