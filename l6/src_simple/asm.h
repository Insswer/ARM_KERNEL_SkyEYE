#ifndef     __ASM_H__
#define     __ASM_H__


#define     ARMv4

#define     MEM_SIZE    0x10000000
#define     MEM_BASE    0x30000000
#define     PAGE_TABLE  0x38000000

#define     SVC_STACK   (MEM_BASE + MEM_SIZE - 4)
#define     IRQ_STACK   (SVC_STACK - 0x1000000)
#define     FIQ_STACK   (IRQ_STACK - 0x1000000)
#define     ABT_STACK   (FIQ_STACK - 0x1000000)
#define     UND_STACK   (ABT_STACK - 0x1000000)
#define     SYS_STACK   (UND_STACK - 0x1000000)

unsigned long __get_cp15_control(void);
void          __set_cp15_control(unsigned long);
void          __set_ttb_reg(unsigned long);
void          __set_domain_reg(unsigned long);


#endif
