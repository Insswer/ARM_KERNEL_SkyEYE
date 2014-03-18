#include <ns16550.h>
#include <asm/clocks.h>
#include <asm/cpu.h>
#include <config.h>
#include <io.h>

#define CONTROL_CORE_PAD0_UART3_RX_IRRX_PAD1_UART3_TX_IRTX 0x4a100144

static void setup_clocks_for_console(void)
{
	unsigned long tmp;
	tmp = readl(CM_L4PER_UART3_CLKCTRL);
	tmp &= ~(0x3);
	tmp |= 0x2;
	writel(tmp, CM_L4PER_UART3_CLKCTRL);

	tmp = readl(CONTROL_CORE_PAD0_UART3_RX_IRRX_PAD1_UART3_TX_IRTX);
	tmp &= ~(0xffffffff);
	tmp |= ((0x0 << 0)|(0x1 << 8)|(0x0 << 16));
	writel(tmp, CONTROL_CORE_PAD0_UART3_RX_IRRX_PAD1_UART3_TX_IRTX);
}

void puts(const char *s)
{
	while(*s){
		ns16550_putc(*s++);
	}
}

void putc(char c)
{
	ns16550_putc(c);
}


void setup_console(void)
{
	int clock_divisor;
	setup_clocks_for_console();

	/* clock_divisor should be 26 */
	clock_divisor = ((NS16550_CLK + (BAURD * (MODE_X_DIV/2)))/(MODE_X_DIV * BAURD));
	ns16550_init(clock_divisor);
}

/* ROM code will init uart3 in operation mode */
void ns16550_init(int clock_divisor)
{
	/* clear all features in ier */
	unsigned long tmp;

	tmp = readl(UART3_IER);
	tmp = 0x0;
	writel(tmp, UART3_IER);

	tmp = readl(UART3_MDR1);
	tmp = 0x7;
	writel(tmp, UART3_MDR1);

	tmp = readl(UART3_LCR);
	tmp = 0x80|0x03;
	writel(tmp, UART3_LCR);

	tmp = readl(UART3_DLL);
	tmp = clock_divisor & 0xff;
	writel(tmp, UART3_DLL);

	tmp = readl(UART3_DLM);
	tmp = (clock_divisor >> 8) & 0xff;
	writel(tmp, UART3_DLM);

	tmp = readl(UART3_LCR);
	tmp = 0x03;
	writel(tmp, UART3_LCR);

	tmp = readl(UART3_MCR);
	tmp = 0x01 | 0x02;
	writel(tmp, UART3_MCR);

	tmp = readl(UART3_FCR);
	tmp = 0x01 | 0x02 | 0x04;
	writel(tmp, UART3_FCR);

	tmp = readl(UART3_MDR1);
	tmp = 0;
	writel(tmp, UART3_MDR1);

}

void ns16550_putc(char c)
{
	while((readl(UART3_LSR) & 0x20) == 0);
	writeb(c,UART3_THR);

	if (c == '\n')
		ns16550_putc('\r');
}

char ns16550_getc(void)
{
	while((readl(UART3_LSR) & 0x01) == 0);
	return readb(UART3_RBR);
}
