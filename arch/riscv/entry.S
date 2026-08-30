.section ".text.boot"

.equ STACK_TOP, 0x82000000

.global _start
_start:
    # csrr t0, mhartid
    # bne t0, zero, park_loop
    j master_core

park_loop:
    wfi
    j park_loop

master_core:
    li sp, STACK_TOP

    la t0, __bss_start
    la t1, __bss_end

clear_bss:
    bgeu t0, t1, jump_c

    sd zero, 0(t0)
    addi t0, t0, 8
    j clear_bss

jump_c:
    tail boot_main