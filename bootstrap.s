	.arch armv8-a
	.file	"bootstrap.cpp"
	.text
	.align	2
	.global	boot_main
	.type	boot_main, %function
boot_main:
	sub	sp, sp, #16
	mov	x0, 150994944
	str	x0, [sp, 8]
	ldr	x0, [sp, 8]
	mov	w1, 88
	str	w1, [x0]
.L2:
	nop
	b	.L2
	.size	boot_main, .-boot_main
	.ident	"GCC: (Arm GNU Toolchain 14.3.Rel1 (Build arm-14.174)) 14.3.1 20250623"
