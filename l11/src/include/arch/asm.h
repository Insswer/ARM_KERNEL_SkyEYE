#ifndef     __ASM_H__
#define     __ASM_H__

#define     ARMv4

#define     MEM_SIZE        0x10000000
#define     MEM_BASE        0x30000000
#define     PAGE_TABLE      0x38000000
#define     AVA_PAGE_END    ((PAGE_TABLE - 1024*4 - 1) & ~(0x1000 - 1))

/* 4MB */
#define     SVC_STACK   (PAGE_TABLE - 4)
/* 4MB */
#define     IRQ_STACK   (SVC_STACK - 0x400000)
/* 4MB */
#define     FIQ_STACK   (IRQ_STACK - 0x400000)
/* 4MB */
#define     ABT_STACK   (FIQ_STACK - 0x400000)
/* 4MB */
#define     UND_STACK   (ABT_STACK - 0x400000)
/* 4MB */
#define     SYS_STACK   (UND_STACK - 0x400000)
/* 16MB */
#define		USR_STACK	(SYS_STACK - 0x1000000)
/* 16MB */
#define     STACK_END   (USR_STACK - 0x1000000)
/* SAFE_STACK_LIMIT */
#define     STACK_SAFE  ((STACK_END - 0x1000) & (~(0x1000 - 1)))


unsigned long __get_cp15_control(void);
void          __set_cp15_control(unsigned long);
void          __set_ttb_reg(unsigned long);
void          __set_domain_reg(unsigned long);
void          __swi(int);
void          __enable_int(void);
void          __disable_int(void);
#endif
