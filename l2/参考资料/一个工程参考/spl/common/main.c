#include <ns16550.h>
#include <asm/cpu.h>
#include <io.h>
#include <led.h>

/*****************************************
 * Routine: board_init
 * Description: Early hardware init.
 *****************************************/
void board_init(void)
{
	unsigned int rev = omap_revision();
	if (rev != OMAP4430_ES1_0) {
		if (readl(0x4805D138) & (1<<22)) {
			/* enable software ioreq */
			sr32(0x4A30a31C, 8, 1, 0x1);
			/* set for sys_clk (38.4MHz) */
			sr32(0x4A30a31C, 1, 2, 0x0);
			/* set divisor to 2 */
			sr32(0x4A30a31C, 16, 4, 0x1);
			/* set the clock source to active */
			sr32(0x4A30a110, 0, 1, 0x1);
			/* enable clocks */
			sr32(0x4A30a110, 2, 2, 0x3);
		} else {
			/* enable software ioreq */
			sr32(0x4A30a314, 8, 1, 0x1);
			/* set for PER_DPLL */
			sr32(0x4A30a314, 1, 2, 0x2);
			/* set divisor to 16 */
			sr32(0x4A30a314, 16, 4, 0xf);
			/* set the clock source to active */
			sr32(0x4A30a110, 0, 1, 0x1);
			/* enable clocks */
			sr32(0x4A30a110, 2, 2, 0x3);
		}
	}
}



void(*init_funcs[])(void) = {
	cpu_init,		/* basic cpu dependent setup */
	board_init,		/* basic board dependent setup */
// 	setup_console,		
	NULL,
};

void main(void)
{

	void (**init_func_ptr)(void);
	for (init_func_ptr = init_funcs; *init_func_ptr; ++init_func_ptr) {
		(*init_func_ptr)();
	}
	putc('H');
	printk("hello world\n");
	led_on(2);
	while(1);
}
