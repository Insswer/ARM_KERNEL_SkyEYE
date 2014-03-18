#ifndef     __EXCEPTION_H__
#define     __EXCEPTION_H__



void debug_exception(unsigned long, int);
void do_irq(void);
void swi_exception(int);
void debug(void);

#endif
