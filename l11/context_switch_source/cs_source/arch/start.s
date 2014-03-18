

    .equ    MODE_MASK,   0x1f
    .equ    SYS_MODE,    0x1f
    .equ    IRQ_MODE,    0x12
    .equ    FIQ_MODE,    0x11
    .equ    SVC_MODE,    0x13
    .equ    USR_MODE,    0x10
    .equ    MON_MODE,    0x16
    .equ    ABT_MODE,    0x17
    .equ    HYP_MODE,    0x1a
    .equ    UND_MODE,    0x1b

    .equ    IRQ_MASK,    0x80
    .equ    FIQ_MASK,    0x40

    .equ     MEM_SIZE,    0x10000000
    .equ     MEM_BASE,    0x30000000

    /* 16MB */
    .equ     SVC_STACK,   (MEM_BASE + MEM_SIZE - 4)
    .equ     SVC_IRQ_STACK, (SVC_STACK - 0x200000)
    /* 16MB */
    .equ     IRQ_STACK,   (SVC_STACK - 0x400000)
    /* 16MB */
    .equ     FIQ_STACK,   (IRQ_STACK - 0x400000)
    /* 16MB */
    .equ     ABT_STACK,   (FIQ_STACK - 0x400000)
    /* 16MB */
    .equ     UND_STACK,   (ABT_STACK - 0x400000)
    /* 16MB */
    .equ     SYS_STACK,   (UND_STACK - 0x400000)
    /* 32MB */
    .equ     STACK_END,   (SYS_STACK - 0x2000000)

    .equ     UND_EXCEPTION,             0x1
    .equ     PREFETCH_ABORT_EXCEPTION,  0x2
    .equ     NOTUSED_EXCEPTION,         0x3
    .equ     DATA_ABORT_EXCEPTION,      0x4
    .equ     FIQ_EXCEPTION,             0x5

    .equ 	DISABLE_IRQ,				0x80
	.equ 	DISABLE_FIQ,				0x40
	.equ 	SYS_MOD,					0x1f
	.equ 	IRQ_MOD,					0x12
	.equ 	FIQ_MOD,					0x11
	.equ 	SVC_MOD,					0x13
	.equ 	ABT_MOD,					0x17
	.equ 	UND_MOD,					0x1b
	.equ 	MOD_MASK,					0x1f


.macro CHANGE_TO_SVC
        msr     cpsr_c,#(DISABLE_FIQ|DISABLE_IRQ|SVC_MOD)
.endm

.macro CHANGE_TO_IRQ
        msr     cpsr_c,#(DISABLE_FIQ|DISABLE_IRQ|IRQ_MOD)
.endm

.macro CHANGE_TO_SYS
        msr     cpsr_c,#(DISABLE_FIQ|DISABLE_IRQ|SYS_MOD)
.endm



	.extern	start_kernel
	.extern init_mmu
	.extern __bss_start__
	.extern __bss_end__

	.global _start
	.section .vector
	.align 0
_start:
    ldr     pc,     _reset
    /* 该异常发生在译码阶段 */
    ldr     pc,     _undefined_instruction
    ldr     pc,     _soft_interrupt
    /* 该异常发生于取址阶段，如果目标指令位于非法地址则触发该异常 */
    ldr     pc,     _prefetch_abort
    /* 发生在访存阶段，如果地址不在内存则触发该异常 */
    ldr     pc,     _data_abort
    ldr     pc,     _not_used
    ldr     pc,     _irq
    ldr     pc,     _fiq

    .align 4
_reset:
    .word       reset

_undefined_instruction:
    .word       undefine_instruction

_soft_interrupt:
    .word       soft_interrupt

_prefetch_abort:
    .word       prefetch_abort

_data_abort:
    .word       data_abort

/* virtualization call */
_not_used:
    .word       not_used

_irq:
    .word       irq

_fiq:
    .word       fiq

    .text
/* arm will reset in svc mode */
reset:

/* 我们需要切换到每个模式，然后设置他们的堆栈指针sp */
    mrs r0,     cpsr
    bic r0,     r0,     #MODE_MASK
    orr r0,     r0,     #(SVC_MODE|IRQ_MASK|FIQ_MASK)
    msr cpsr,   r0
/* 我们现在位于SVC模式下，并且中断都DISABLE */
    ldr sp,     =SVC_STACK

    mrs r0,     cpsr
    bic r0,     r0,     #MODE_MASK
    orr r0,     r0,     #(IRQ_MODE|IRQ_MASK|FIQ_MASK)
    msr cpsr,   r0
    ldr sp,     =IRQ_STACK

    mrs r0,     cpsr
    bic r0,     r0,     #MODE_MASK
    orr r0,     r0,     #(FIQ_MODE|IRQ_MASK|FIQ_MASK)
    msr cpsr,   r0
    ldr sp,     =FIQ_STACK

    mrs r0,     cpsr
    bic r0,     r0,     #MODE_MASK
    orr r0,     r0,     #(ABT_MODE|IRQ_MASK|FIQ_MASK)
    msr cpsr,   r0
    ldr sp,     =ABT_STACK

    mrs r0,     cpsr
    bic r0,     r0,     #MODE_MASK
    orr r0,     r0,     #(UND_MODE|IRQ_MASK|FIQ_MASK)
    msr cpsr,   r0
    ldr sp,     =UND_STACK

    mrs r0,     cpsr
    bic r0,     r0,     #MODE_MASK
    orr r0,     r0,     #(SYS_MODE|IRQ_MASK|FIQ_MASK)
    msr cpsr,   r0
    ldr sp,     =SYS_STACK

/* 我们现在处于系统模式下 */
_clear_bss:
    ldr r0,     _bss_start
    ldr r1,     _bss_end
    mov r2,     #0x0

1:
    cmp r0,     r1
    beq __main
    str r2,     [r0],#0x4
/* b means before , f means front */
    b   1b

__main:
/* never return       */
/*	ldr pc, 	_main */
    bl  start_kernel

halt:
    b   halt


_bss_start:
    .word   __bss_start__

_bss_end:
    .word   __bss_end__

    .extern debug_exception
    .extern do_irq
    .extern swi_exception
    .extern identify_and_clear

/* 未定义指令在ALU中发生，此时PC没有更新，当前PC指向未定以指令的地址加8的位置，系统
   将PC-4的值保存到LR当中。出错指令的地址为PC-8，不需要修正返回地址.
 */
undefine_instruction:
    sub     lr,     lr, #0x4
    mov     r0,     lr
    mov     r1,     #UND_EXCEPTION
    bl      debug_exception
    b       e_halt

/* 由SWI指令触发，为当前指令触发，情况同未定义指令，发生在译码阶段。此时PC没有更新，
   PC = PC + 8，处理器将PC-4的值保存在lr中，返回地址不要修改.注意v7以后该指令改名为了SVC
   我们可以通过LR-4拿到SVC指令，SVC指令的低24位保存了系统调用号
*/
soft_interrupt:
    stmfd   sp!,    {r0 - r12, lr}
    ldr     r0,     [lr,#-4]
    bic     r0,     r0, #0xff000000
    bl      swi_exception
    ldmfd   sp!,    {r0 - r12, pc}^

/* 发生在流水线取址, 此时PC并没有更新，仍然指向当前异常地址加8的位置。
   处理器将PC-4的地址存放到lr中。失败的指令为PC-8
 */
prefetch_abort:
    sub     lr,     lr, #0x4
    mov     r0,     lr
    mov     r1,     #PREFETCH_ABORT_EXCEPTION
    bl      debug_exception
    b       e_halt

/* 读取数据访存失败会触发该异常，异常触发时PC已经更新（执行阶段），此时
   PC位于PC+12的位置。处理器将PC-4的地址存放到lr中，失败访存指令位于PC-8处
*/
data_abort:
    sub     lr,     lr, #0x8
    mov     r0,     lr
    mov     r1,     #DATA_ABORT_EXCEPTION
    bl      debug_exception
    b       e_halt

not_used:
    mov     r0,     #0x0
    mov     r1,     #NOTUSED_EXCEPTION
    bl      debug_exception
    b       e_halt
/* 简化一下 */
/*				IRQ_STACK
	lr			-4
	r12			-8
	r11			-c
	r10			-10
	r9			-14
	r8			-18
	r7			-1c
	r6			-20
	r5			-24
	r4			-28
	r3			-2c
	r2			-30
	r1			-34
	r0	<-sp	-38
*/
irq:
	/* 设置堆栈指针 */
	ldr		sp,		=IRQ_STACK
	/* 修正返回地址 */
    sub     lr,     lr, #0x4
	/* 保存上下文寄存器 */
	stmfd	sp!,	{r0-r12, lr}

	/* 保证恢复的瞬间中断必须被关掉！ */
	mrs     r0,     spsr
    orr     r0,     r0,#0xc0
    msr     spsr,   r0

	bl      do_irq

	/* 读取上下文，这个时候中断返回，执行这一条代码的环境是
	   do_irq中不进行调度，也就是本次中断不需要进行调度。
	*/
    ldmfd   sp!,    {r0-r12, pc}^


	.global __sched_in_irq
	.extern __common_schedule
__sched_in_irq:
	/* 重新平衡中断堆栈，这是必要的 */
	ldr		sp,		=IRQ_STACK
	/* 现在sp指向r1 */
	sub		sp,		sp,#0x34
	/* 恢复r1到r12 */
	ldmfd	sp!,	{r1-r12}
	/* 再次平衡堆栈 */
	ldr		sp,		=IRQ_STACK
	/* 恢复lr */
	ldr		lr,		[sp, #-0x4]
	/* 将lr保存到r0中 */
	mov		r0,		lr
	/* 切换到系统模式 */
	CHANGE_TO_SYS
	/* 保存中断返回地址 */
	/* 我们任务的堆栈应该是这样的：
		中断返回地址(pc)
		某个函数时的返回地址(lr)<---sp
		r12
		r11
		r10
		r9
		r8
		r7
		r6
		r5
		r4
		r3
		r2
		r1
		r0
		cpsr
	*/
	stmfd	sp!,	{r0}
	/* 保存正常流程中，上一个进程的返回地址 */
	stmfd	sp!,	{lr}
	/* 保存LR，也就是返回地址 */
	/* 再次切换到中断模式 */
	CHANGE_TO_IRQ
	/* 再次切换中断栈 */
	ldr		sp,		=IRQ_STACK
	/* sp指到r0 */
	sub		sp,		sp,#0x38
	/* 恢复了r0 */
	ldr		r0,		[sp]
	/* 将立即要跳转的目标保存到lr */
	ldr		lr,		=__asm_schedule
	/* 保存，这个语句是为了下面使用^ */
	stmfd	sp!,	{lr}
	/* 跳转到__asm_schedule,并且切换回系统模式，同时还将spsr写给cpsr，此时中断被禁止 */
	ldmfd	sp!,	{pc}^
__asm_schedule:
	/* 现在仿佛和刚进中断之前的任务一样 */
	/*
		中断返回地址(pc)
		某个函数时的返回地址(lr)<---sp
		r12
		r11
		r10
		r9
		r8
		r7
		r6
		r5
		r4
		r3
		r2
		r1
		r0
		cpsr
	*/
	msr		cpsr_c,	#0xdf
	/* 保存r0到r12 */
	stmfd	sp!,	{r0-r12}
	mrs		r1,		cpsr
	/* 保存cpsr */
	stmfd	sp!,	{r1}

	/* 通过小技巧拿到thread_info的指针 */
	mov		r1,		sp
	bic		r1,		#0xff0
	bic		r1,		#0xf
	mov		r0,		sp
	/* save sp to thread_info，保存堆栈，恢复的时候我们可以通过这个sp拿到上下文 */
	str		r0,		[r1]

	/* 获取下一个任务的thread_info */
	bl		__common_schedule
	/* 加载堆栈，堆栈现在指向需要恢复的r0 */
	ldr		sp,		[r0]
	/* 加载cpsr */
	ldmfd	sp!,	{r1}
	msr		cpsr,	r1
	/* 还原上下文 */
	ldmfd	sp!,	{r0-r12,lr,pc}

/* 快中断发生时，处理器已经执行完指令，这时PC已经更新，PC = PC + 12
   处理器将PC-4的值保存到lr中，返回地址需要将lr减去4才行，我们不支持FIQ
 */
fiq:
    mov     r0,     #0x0
    mov     r1,     #FIQ_EXCEPTION
    bl      debug_exception
    b       e_halt

e_halt:
    b   e_halt


