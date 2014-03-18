

    .global __swi_1
__swi_1:
    swi     #0x1
    mov     pc,     lr
