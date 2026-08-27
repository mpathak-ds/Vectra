.section ".text.boot"

#ifdef ORANGE_PI
.equ STACK_TOP, 0x50000000
#else
.equ STACK_TOP, 0x47F00000
#endif

.global _start
_start:
    mrs x0, mpidr_el1
    and x0, x0, #0xFF
    cbz x0, master_core

park_loop:
    wfe
    b park_loop

master_core:
    ldr x0, =STACK_TOP
    mov sp, x0

    ldr x0, =__bss_start
    ldr x1, =__bss_end

clear_bss:
    cmp x0, x1
    b.hs jump_c

    str xzr, [x0], #8
    b clear_bss

jump_c:
    bl boot_main