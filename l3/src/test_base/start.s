

	.extern	main
	.global _start
	.text

_start:
	ldr sp, =0x38000000
    bl  main

halt:
    b   halt



