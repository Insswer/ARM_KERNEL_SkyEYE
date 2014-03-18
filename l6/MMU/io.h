#ifndef     __IO_H__
#define     __IO_H__

//#define MMU



#define UTXH0	    (0xe0000020)
#define URXH0       (0xe0000024)
#define UTRSTAT0    (0xe0000010)



#define RAW_UTXH0	    (0x50000020)
#define RAW_URXH0       (0x50000024)
#define RAW_UTRSTAT0    (0x50000010)



#define read8(addr)    ({ unsigned char __c = (*(volatile unsigned char *)(addr)); __c; })
#define read16(addr)   ({ unsigned short __c = (*(volatile unsigned short *)(addr)); __c; })
#define read32(addr)    ({ unsigned long __c = (*(volatile unsigned long *)(addr)); __c; })

#define write8(val,addr) ({ unsigned char __v = val; (*(volatile unsigned char *)(addr)) = (__v); })
#define write16(val,addr) ({ unsigned short __v = val; (*(volatile unsigned short *)(addr)) = (__v); })
#define write32(val,addr) ({ unsigned long __v = val; (*(volatile unsigned long *)(addr)) = (__v); })

/*
    typedef     char*   va_list;
    #define _INTSIZEOF(n)   ((sizeof(n) + sizeof(int) - 1) & ~(sizeof(int)-1))
    #define va_start(ap,v)  ( ap = (va_list)&v + _INTSIZEOF(v))
    #define va_arg(ap,t)    (*(t *)((ap += _INTSIZEOF(t)) - _INTSIZEOF(t)))
    #define va_end(ap)      (ap = (va_list)0)
*/

void putc(char c);
void putc_raw(char c);
void puts(char *s);
void puts_raw(char *s);
char getc(void);
int printf(const char *fmt, ...);
int printf_raw(const char *fmt, ...);

#endif
