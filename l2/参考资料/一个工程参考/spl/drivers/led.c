#include <led.h>
#include <asm/cpu.h>
#include <io.h>


void init_gpio_clk(void)
{
	unsigned long tmp;

	tmp = readl(CM_WKUP_GPIO1_CLKCTRL);
	tmp |=	((0x1 << 8)|(0x1 << 0));
	writel(tmp, CM_WKUP_GPIO1_CLKCTRL);

	tmp = readl(CM_L4PER_CLKSTCTRL);
	tmp |= (0x2 << 0);
	writel(tmp, CM_L4PER_CLKSTCTRL);

	tmp = readl(CM_L4PER_GPIO4_CLKCTRL);
	tmp |= (0x1 << 0);
	writel(tmp, CM_L4PER_GPIO4_CLKCTRL);
	
}
void init_pad_control(void)
{
	unsigned long tmp;

	tmp = readl(CONTROL_WKUP_PAD0_FREF_CLK4_OUT_PAD1_SYS_32K);
	tmp &= ~(0x7 << 0);
	tmp |= (0x3 << 0);
	tmp &= ~(0x1 << 3);
	writel(tmp, CONTROL_WKUP_PAD0_FREF_CLK4_OUT_PAD1_SYS_32K);

	tmp = readl(CONTROL_CORE_PAD0_SDMMC1_DAT7_PAD1_ABE_MCBSP2_CLKX);
	tmp &= ~(0x7 << 16);
	tmp |= (0x3 << 16);
	tmp &= ~(0x1 << 19);
	writel(tmp, CONTROL_CORE_PAD0_SDMMC1_DAT7_PAD1_ABE_MCBSP2_CLKX);
}
void init_gpio_led(void)
{
	init_gpio_clk();
	init_pad_control();

	unsigned long tmp;
	tmp = readl(GPIO1_CTRL);
	tmp &= ~(0x1 << 0);
	writel(tmp, GPIO1_CTRL);

	tmp = readl(GPIO4_CTRL);
	tmp &= ~(0x1 << 0);
	writel(tmp, GPIO4_CTRL);

	tmp = readl(GPIO1_OE);
	tmp &= ~(0x1 << 8);
	writel(tmp, GPIO1_OE);

	tmp = readl(GPIO4_OE);
	tmp &= ~(0x1 << 14);
	writel(tmp, GPIO4_OE);

	tmp = readl(GPIO1_DATAOUT);
	tmp &= ~(0x1 << 8);
	writel(tmp, GPIO1_DATAOUT);

	tmp = readl(GPIO4_DATAOUT);
	tmp &= ~(0x1 << 14);
	writel(tmp, GPIO4_DATAOUT);

}

void led_on(int nr)
{
	unsigned long tmp;
	if (nr == 1) {
		tmp = readl(GPIO4_DATAOUT);
		tmp &= ~(1 << 14);
		tmp |= (1 << 14);
		writel(tmp, GPIO4_DATAOUT);
	} else if (nr == 2) {
		tmp = readl(GPIO1_DATAOUT);
		tmp &= ~(1 << 8);
		tmp |= (1 << 8);
		writel(tmp, GPIO1_DATAOUT);
	}
}

void led_off(int nr)
{
	unsigned long tmp;
	if (nr == 1) {
		tmp = readl(GPIO4_DATAOUT);
		tmp &= ~(1 << 14);
		writel(tmp, GPIO4_DATAOUT);
	} else if (nr == 2) {
		tmp = readl(GPIO1_DATAOUT);
		tmp &= ~(1 << 8);
		writel(tmp, GPIO1_DATAOUT);
	}
}

