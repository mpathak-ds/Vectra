.section ".text.boot"

.global _start
_start:
    /* Read CPU ID from MPIDR_EL1 */
    mrs     x0, mpidr_el1
    and     x0, x0, #0xFF
    cbz     x0, master_core

park_loop:
    wfe
    b       park_loop

master_core:
    /* Set stack pointer to safe memory below 0x00200000 */
    ldr     x0, =_start
    mov     sp, x0

    /* Clear BSS section */
    ldr     x0, =__bss_start
    ldr     x1, =__bss_end
clear_bss:
    cmp     x0, x1
    b.ge    jump_c
    str     xzr, [x0], #8
    b       clear_bss

jump_c:
    bl      kernel_main
halt:
    wfi
    b       halt