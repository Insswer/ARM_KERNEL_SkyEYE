#ifndef     __INT_CONTROL_H__
#define     __INT_CONTROL_H__


#define S3C2410_IRQ(num)    (num + 0)

/* main cpu interrupts */
#define IRQ_EINT0      S3C2410_IRQ(0)	    /* 16 */
#define IRQ_EINT1      S3C2410_IRQ(1)
#define IRQ_EINT2      S3C2410_IRQ(2)
#define IRQ_EINT3      S3C2410_IRQ(3)
#define IRQ_EINT4t7    S3C2410_IRQ(4)	    /* 20 */
#define IRQ_EINT8t23   S3C2410_IRQ(5)
#define IRQ_RESERVED6  S3C2410_IRQ(6)	    /* for s3c2410 */
#define IRQ_CAM        S3C2410_IRQ(6)	    /* for s3c2440,s3c2443 */
#define IRQ_BATT_FLT   S3C2410_IRQ(7)
#define IRQ_TICK       S3C2410_IRQ(8)	    /* 24 */
#define IRQ_WDT	       S3C2410_IRQ(9)	    /* WDT/AC97 for s3c2443 */
#define IRQ_TIMER0     S3C2410_IRQ(10)
#define IRQ_TIMER1     S3C2410_IRQ(11)
#define IRQ_TIMER2     S3C2410_IRQ(12)
#define IRQ_TIMER3     S3C2410_IRQ(13)
#define IRQ_TIMER4     S3C2410_IRQ(14)
#define IRQ_UART2      S3C2410_IRQ(15)
#define IRQ_LCD	       S3C2410_IRQ(16)	    /* 32 */
#define IRQ_DMA0       S3C2410_IRQ(17)	    /* IRQ_DMA for s3c2443 */
#define IRQ_DMA1       S3C2410_IRQ(18)
#define IRQ_DMA2       S3C2410_IRQ(19)
#define IRQ_DMA3       S3C2410_IRQ(20)
#define IRQ_SDI	       S3C2410_IRQ(21)
#define IRQ_SPI0       S3C2410_IRQ(22)
#define IRQ_UART1      S3C2410_IRQ(23)
#define IRQ_RESERVED24 S3C2410_IRQ(24)	    /* 40 */
#define IRQ_NFCON      S3C2410_IRQ(24)	    /* for s3c2440 */
#define IRQ_USBD       S3C2410_IRQ(25)
#define IRQ_USBH       S3C2410_IRQ(26)
#define IRQ_IIC	       S3C2410_IRQ(27)
#define IRQ_UART0      S3C2410_IRQ(28)	    /* 44 */
#define IRQ_SPI1       S3C2410_IRQ(29)
#define IRQ_RTC	       S3C2410_IRQ(30)
#define IRQ_ADCPARENT  S3C2410_IRQ(31)

#define SRCPND          0x4a000000
#define INTMOD          0x4a000004
#define INTMSK          0x4a000008
#define INTPEND         0x4a000010
#define INTOFFSET       0x4a000014

void ack_int(unsigned int num);
int read_int(void);

void disable_int(unsigned int irq_num);
void enable_int(unsigned int irq_num);

struct irq_desc {
    /* 实现这个结构体 */
};

extern struct irq_desc irq_descs[];
int request_irq(unsigned int irq_num, int (*irq)(void *), void *private_data);

#endif
