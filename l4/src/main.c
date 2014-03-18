#include "asm.h"



#define UFCON0	((volatile unsigned int *)(0x50000020))

const char *str="helloworld\n";


void puts(const char *p)
{

        while(*p) {
                *UFCON0=*p++;
        };
}


void    main(void)
{
    puts(str);
}
