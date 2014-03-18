

    .global __get_cp15_control
    .global __set_cp15_control
    .global __set_ttb_reg
    .global __set_domain_reg

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


