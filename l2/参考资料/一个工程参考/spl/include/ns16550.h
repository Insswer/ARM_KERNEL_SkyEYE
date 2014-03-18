#ifndef __NS16550_H__
#define __NS16550_H__


#define MODE_X_DIV 16
#define NS16550_CLK (48000000)
#define BAURD 115200


#define UART3_RBR	0x48020000
#define UART3_IER	0x48020004
#define UART3_FCR	0x48020008
#define UART3_LCR	0x4802000c
#define	UART3_MCR	0x48020010
#define UART3_LSR	0x48020014
#define UART3_MSR	0x48020018
#define UART3_SCR	0x4802001c
#define UART3_MDR1	0x48020020

#define UART3_THR UART3_RBR
#define UART3_IIR UART3_FCR
#define UART3_DLL UART3_RBR
#define UART3_DLM UART3_IER


struct NS16550 {
	unsigned char rbr;		/* 0 */
	int pad1:24;
	unsigned char ier;		/* 1 */
	int pad2:24;
	unsigned char fcr;		/* 2 */
	int pad3:24;
	unsigned char lcr;		/* 3 */
	int pad4:24;
	unsigned char mcr;		/* 4 */
	int pad5:24;
	unsigned char lsr;		/* 5 */
	int pad6:24;
	unsigned char msr;		/* 6 */
	int pad7:24;
	unsigned char scr;		/* 7 */
	int pad8:24;
	unsigned char mdr1;
} __attribute__ ((packed));

#define thr rbr
#define iir fcr
#define dll rbr
#define dlm ier


void puts(const char *s);
void putc(char c);

void ns16550_init(int clock_divisor);
void ns16550_putc(char c); 
char ns16550_getc(void);
void setup_console(void);


#endif

