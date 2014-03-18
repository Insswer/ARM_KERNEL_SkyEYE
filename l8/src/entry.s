

    .global __get_cp15_control
    .global __set_cp15_control
    .global __set_ttb_reg
    .global __set_domain_reg
    .global __enable_int
    .global __disable_int

__get_cp15_control:
    mrc     p15, 0, r0, c1, c0, 0
    mov     pc, lr

__set_cp15_control:
    mcr     p15, 0, r0, c1, c0, 0
    mov     pc, lr

__set_ttb_reg:
    mcr     p15, 0, r0, c2, c0, 0
    mov     pc, lr

__set_domain_reg:
    mcr     p15, 0, r0, c3, c0, 0
    mov     pc, lr

__enable_int:
    mrs     r0,     cpsr
    bic     r0,     r0,#0x80
    msr     cpsr,   r0
    mov     pc, lr

__disable_int:
    mrs     r0,     cpsr
    orr     r0,     r0,#0xc0
    msr     cpsr,   r0
    mov     pc, lr

