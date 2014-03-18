#ifndef __LED_H__
#define __LED_H__

#define CONTROL_WKUP_PAD0_FREF_CLK4_OUT_PAD1_SYS_32K		0x4a31e05c
#define CONTROL_CORE_PAD0_SDMMC1_DAT7_PAD1_ABE_MCBSP2_CLKX	0x4a1000f4

#define GPIO1_CTRL	0x4a310130
#define	GPIO4_CTRL	0x48059130

#define GPIO1_OE	0x4a310134
#define GPIO4_OE	0x48059134

#define	GPIO1_DATAOUT	0x4a31013c
#define GPIO4_DATAOUT	0x4805913c

void init_gpio_clk(void);
void init_pad_control(void);
void init_gpio_led(void);
void led_on(int nr);
void led_off(int nr);





#endif
