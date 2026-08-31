.section ".text.boot"

.equ STACK_TOP, 0x82000000
.equ STACK_SIZE, 65536

.global _start
_start:
    j master_core

park_loop:
    wfi
    j park_loop

master_core:
    # separate stack for each new cpu
    li t0, STACK_TOP
    li t1, STACK_SIZE
    mul t1, a0, t1
    add t0, t0, t1
    mv sp, t0

    la t0, __bss_start
    la t1, __bss_end

clear_bss:
    bgeu t0, t1, jump_c

    sd zero, 0(t0)
    addi t0, t0, 8
    j clear_bss

jump_c:
    tail boot_main