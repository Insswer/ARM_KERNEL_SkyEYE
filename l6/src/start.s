

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

    .equ     SVC_STACK,   (MEM_BASE + MEM_SIZE - 4)
    .equ     IRQ_STACK,   (SVC_STACK - 0x1000000)
    .equ     FIQ_STACK,   (IRQ_STACK - 0x1000000)
    .equ     ABT_STACK,   (FIQ_STACK - 0x1000000)
    .equ     UND_STACK,   (ABT_STACK - 0x1000000)
    .equ     SYS_STACK,   (UND_STACK - 0x1000000)


	.extern	start_kernel
	.extern init_mmu
	.extern __bss_start__
	.extern __bss_end__
	.global _start
	.text

_start:
    b       reset
    ldr     pc,     _undefined_instruction
    ldr     pc,     _soft_interrupt
    ldr     pc,     _prefetch_abort
    ldr     pc,     _data_abort
    ldr     pc,     _not_used
    ldr     pc,     _irq
    ldr     pc,     _fiq

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


undefine_instruction:
    nop

soft_interrupt:
    nop

prefetch_abort:
    nop

data_abort:
    nop

not_used:
    nop

irq:
    nop

fiq:
    nop
