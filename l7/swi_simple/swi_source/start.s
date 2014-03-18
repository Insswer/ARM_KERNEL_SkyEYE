

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

    .equ     UND_EXCEPTION,             0x1
    .equ     PREFETCH_ABORT_EXCEPTION,  0x2
    .equ     NOTUSED_EXCEPTION,         0x3
    .equ     DATA_ABORT_EXCEPTION,      0x4
    .equ     FIQ_EXCEPTION,             0x5



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

irq:
    sub     lr,     lr, #0x4
    stmfd   sp!,    {r0 - r12, lr}
    /* call do_irq */
    ldmfd   sp!,    {r0 - r12, pc}^


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
