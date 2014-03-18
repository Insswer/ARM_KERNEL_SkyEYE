#ifndef     __ASM_H__
#define     __ASM_H__




#define     MEM_SIZE    0x10000000
#define     MEM_BASE    0x30000000

#define     SVC_STACK   (MEM_BASE + MEM_SIZE - 4)
#define     IRQ_STACK   (SVC_STACK - 0x1000000)
#define     FIQ_STACK   (IRQ_STACK - 0x1000000)
#define     ABT_STACK   (FIQ_STACK - 0x1000000)
#define     UND_STACK   (ABT_STACK - 0x1000000)
#define     SYS_STACK   (UND_STACK - 0x1000000)



#endif
