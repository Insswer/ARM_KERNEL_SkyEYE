#ifndef     __TIMER_H__
#define     __TIMER_H__

/* we use timer04 ignore PCLK... */

#define TIMER_BASE  (0x51000000)
#define TCFG0   ((volatile unsigned int *)(TIMER_BASE+0x0))
#define TCFG1   ((volatile unsigned int *)(TIMER_BASE+0x4))
#define TCON    ((volatile unsigned int *)(TIMER_BASE+0x8))
#define TCONB4  ((volatile unsigned int *)(TIMER_BASE+0x3c))

#define PRESCALE  0x255

void sys_tick_start(void);

#endif
