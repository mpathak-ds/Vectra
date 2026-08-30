.global enter_usermode

enter_usermode:
    li t0, 0x100
    csrc sstatus, t0

    csrw sscratch, sp

    csrw sepc, a0
    mv sp, a1

    mv t0, zero
    mv t1, zero
    mv t2, zero
    mv t3, zero
    mv t4, zero
    mv t5, zero
    mv t6, zero
    mv a0, zero
    mv a1, zero
    mv a2, zero

    sret