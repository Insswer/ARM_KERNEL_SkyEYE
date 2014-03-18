
.global _start
.extern main
.extern s_init
_start:
	b	reset
	ldr	pc,	_hang
	ldr	pc,	_hang
	ldr	pc,	_hang
	ldr	pc,	_hang
	ldr	pc,	_hang
	ldr	pc,	_hang
	ldr	pc,	_hang

_hang:
	.word	do_hang

	.word	0x12345678
	.word	0x12345678
	.word	0x12345678
	.word	0x12345678
	.word	0x12345678
	.word	0x12345678
	.word	0x12345678

do_hang:
	b	do_hang

.global _end_vect
_end_vect:
	
	.balignl 16, 0xdddddddd


/*
	Startup Code (reset vector)
	we suppose to setup uart3 as soon as possible.
	we don't need to relocate our code. we start at 0x40300000 and we set Stack Pointer to 0x4030dffc.
	we need to clear bss.
*/


.global _bss_start

_bss_start:
	.word	__bss_start

.global _bss_end
_bss_end:
	.word	__bss_end



/*
	we are always in FAT mode here. So no need to check boot params
*/
reset:
/*
	set the cpu to SVC32 mode. No interrupt is supposed to be happened here...
*/
	mrs	r0,	cpsr
	bic	r0,	r0,	#0x1f
	orr	r0,	r0,	#0xd3
	msr	cpsr,	r0

/*
	ignore Vector, we will relocate vector table in OS init.
*/

	bl	cpu_init_crit

.global hang
hang:
	b	hang


cpu_init_crit:
/*
	Invalidate L1 I/D and branch predictor array	
*/

	mov	r0,	#0
	mcr	p15,	0,	r0,	c8,	c7,	0	/* invalidate TLBs */
	mcr	p15,	0,	r0,	c7,	c5, 	0	/* invalidate icache */
	mcr	p15,	0,	r0,	c7,	c5,	6	/* invalidate BP array*/
	
/*
	disable MMU and caches
*/

	mrc	p15,	0,	r0,	c1,	c0,	0
	bic	r0,	r0,	#0x00002000			/* clear bits 13  ----> using low execption vector */
	bic	r0,	r0,	#0x00000007			/* clear bits 2:0 ----> Data Cache disabled/Alignment check disabled/MMU disabled */
	orr	r0,	r0,	#0x00000002			/* set bit 1      ----> setup Alignment Check */
	orr	r0,	r0,	#0x00000800			/* set bit 11     ----> branch prediction disabled */
	orr	r0,	r0,	#0x00001000			/* set bit 12     ----> enable I-cache */
	mcr	p15,	0,	r0,	c1,	c0,	0

/*
	clear BSS and jump to board specific initialization...
*/

	ldr	r0,	_bss_start
	ldr	r1,	_bss_end
	cmp	r0,	r1
	beq	after_clr_bss
	mov	r2,	#0x0

clrbss_1:
	
	str	r2,	[r0],	#4
	cmp	r0,	r1
	bne	clrbss_1

after_clr_bss:

	ldr		sp,	=0x4030dff0
	bl s_init
	bl main
		





