#ifndef     __EXCEPTION_H__
#define     __EXCEPTION_H__

#define    UND_EXCEPTION             0x1
#define    PREFETCH_ABORT_EXCEPTION  0x2
#define    NOTUSED_EXCEPTION         0x3
#define    DATA_ABORT_EXCEPTION      0x4
#define    FIQ_EXCEPTION             0x5


void debug_exception(unsigned long, int);
void do_irq(unsigned int);
void swi_exception(int);
void debug(void);
unsigned int identify_and_clear(void);

#endif
