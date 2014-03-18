#ifndef     __TIMER_H__
#define     __TIMER_H__

/* we use timer04 ignore PCLK... */

#define TIMER_BASE  (0x51000000)
#define TCFG0   ((volatile unsigned int *)(TIMER_BASE+0x0))
#define TCFG1   ((volatile unsigned int *)(TIMER_BASE+0x4))
#define TCON    ((volatile unsigned int *)(TIMER_BASE+0x8))
#define TCONB4  ((volatile unsigned int *)(TIMER_BASE+0x3c))


void  init_sys_timer(void);

#endif
