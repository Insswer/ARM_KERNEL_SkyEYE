#ifndef _IO_H_
#define _IO_H_

#include "ktype.h"

#define __getb(a)			(*(vu8 *)(a))
#define __getw(a)			(*(vu16 *)(a))
#define __getl(a)			(*(vu32 *)(a))

#define __putb(v,a)		(*(vu8 *)(a) = (v))
#define __putw(v,a)		(*(vu16 *)(a) = (v))
#define __putl(v,a)		(*(vu32 *)(a) = (v))

/* basic memory barrier for bootloader*/
#define dmb()		__asm__ __volatile__ ("" : : : "memory")
#define __iormb()	dmb()
#define __iowmb()	dmb()

#define writeb(v,c)	({ u8  __v = v; __iowmb(); __putb(__v,c); __v; })
#define writew(v,c)	({ u16 __v = v; __iowmb(); __putw(__v,c); __v; })
#define writel(v,c)	({ u32 __v = v; __iowmb(); __putl(__v,c); __v; })

#define readb(c)	({ u8  __v = __getb(c); __iormb(); __v; })
#define readw(c)	({ u16 __v = __getw(c); __iormb(); __v; })
#define readl(c)	({ u32 __v = __getl(c); __iormb(); __v; })

#define out_8(a,v)	writeb(v,a)
#define in_8(a)		readb(a)

#define out_16(a,v)	writew(v,a)
#define in_16(a)	readw(a)

#define out_32(a,v)	writel(v,a)
#define in_32(a)	readl(a)

#define clrbits(addr, clear) \
	out_32((addr), in_32(addr) & ~(clear))

#define setbits(addr, set) \
	out_32((addr), in_32(addr) | (set))

#define clrsetbits(addr, clear, set) \
	out_32((addr), (in_32(addr) & ~(clear)) | (set))

#define BIT0  (1<<0)
#define BIT1  (1<<1)
#define BIT2  (1<<2)
#define BIT3  (1<<3)
#define BIT4  (1<<4)
#define BIT5  (1<<5)
#define BIT6  (1<<6)
#define BIT7  (1<<7)
#define BIT8  (1<<8)
#define BIT9  (1<<9)
#define BIT10 (1<<10)
#define BIT11 (1<<11)
#define BIT12 (1<<12)
#define BIT13 (1<<13)
#define BIT14 (1<<14)
#define BIT15 (1<<15)
#define BIT16 (1<<16)
#define BIT17 (1<<17)
#define BIT18 (1<<18)
#define BIT19 (1<<19)
#define BIT20 (1<<20)
#define BIT21 (1<<21)
#define BIT22 (1<<22)
#define BIT23 (1<<23)
#define BIT24 (1<<24)
#define BIT25 (1<<25)
#define BIT26 (1<<26)
#define BIT27 (1<<27)
#define BIT28 (1<<28)
#define BIT29 (1<<29)
#define BIT30 (1<<30)
#define BIT31 (1<<31)





static inline void spin_delay(u32 loops)
{
	__asm__ volatile ("1:\n" "subs %0, %1, #1\n"
			  "bne 1b" : "=r" (loops) : "0"(loops));
}




int printk(const char *fmt, ...);
int raise (int signum);
u32 wait_on_value(u32 read_bit_mask, u32 match_value, u32 read_addr,
			  u32 bound);
void sr32(u32 addr, u32 start_bit, u32 num_bits, u32 value);



#endif /*_IO_H_*/

