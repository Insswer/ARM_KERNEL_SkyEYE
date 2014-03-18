
	.extern	helloworld
	.global _start
	.text

_start:
	ldr	sp, =0x30700000
	bl	helloworld
