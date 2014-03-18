
/**
 * omap_smc_rom() - This function is the entry point to services
 *		    exposed by ROM code
 * Prototype:
 * unsigned int omap_smc_rom(unsigned int rom_svc,
 *		unsigned int arg);
 * @rom_svc: Application ID of ROM api (input in r0 will move to r12)
 * @arg: Argument passed to ROM api (input in r1 will move to r0)
 */
.globl omap_smc_rom
.arch_extension sec
omap_smc_rom:
	push {r4-r11,lr}
	mov	r12,	r0
	mov	r0,	r1
	dsb
	isb
	dmb
	smc	#0
	pop {r4-r11,pc}

