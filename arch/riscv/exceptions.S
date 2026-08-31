.section ".text.boot"
.global trap_vector
.align 4

.extern interrupt_exception_handler

trap_vector:
    addi sp, sp, -272

    sd x5,  40(sp)
    sd x6,  48(sp)
    sd x7,  56(sp)

    addi t0, sp, 272
    sd t0,  16(sp)

    csrrw sp, sscratch, sp
    bnez sp, from_usermode

    csrrw sp, sscratch, sp
    li t0, 0
    j stackframe_create

from_usermode:
    li t0, 1

stackframe_create:
    sd x0,  0(sp)         # zero (always 0, but filled for structure matching)
    sd x1,  8(sp)         # ra
    sd x3,  24(sp)        # gp
    sd x4,  32(sp)        # tp
    sd x8,  64(sp)        # s0/fp
    sd x9,  72(sp)        # s1
    sd x10, 80(sp)        # a0
    sd x11, 88(sp)        # a1
    sd x12, 96(sp)        # a2
    sd x13, 104(sp)       # a3
    sd x14, 112(sp)       # a4
    sd x15, 120(sp)       # a5
    sd x16, 128(sp)       # a6
    sd x17, 136(sp)       # a7
    sd x18, 144(sp)       # s2
    sd x19, 152(sp)       # s3
    sd x20, 160(sp)       # s4
    sd x21, 168(sp)       # s5
    sd x22, 176(sp)       # s6
    sd x23, 184(sp)       # s7
    sd x24, 192(sp)       # s8
    sd x25, 200(sp)       # s9
    sd x26, 208(sp)       # s10
    sd x27, 216(sp)       # s11
    sd x28, 224(sp)       # t3
    sd x29, 232(sp)       # t4
    sd x30, 240(sp)       # t5
    sd x31, 248(sp)       # t6

    csrr t1, sstatus
    sd t1, 256(sp)
    csrr t2, sepc
    sd t2, 264(sp)

    csrw sscratch, zero

    mv a0, t0
    mv a1, sp
    call interrupt_exception_handler

    ld t1, 256(sp)
    csrw sstatus, t1
    ld t2, 264(sp)
    csrw sepc, t2

    ld x31, 248(sp)       # t6
    ld x30, 240(sp)       # t5
    ld x29, 232(sp)       # t4
    ld x28, 224(sp)       # t3
    ld x27, 216(sp)       # s11
    ld x26, 208(sp)       # s10
    ld x25, 200(sp)       # s9
    ld x24, 192(sp)       # s8
    ld x23, 184(sp)       # s7
    ld x22, 176(sp)       # s6
    ld x21, 168(sp)       # s5
    ld x20, 160(sp)       # s4
    ld x19, 152(sp)       # s3
    ld x18, 144(sp)       # s2
    ld x17, 136(sp)       # a7
    ld x16, 128(sp)       # a6
    ld x15, 120(sp)       # a5
    ld x14, 112(sp)       # a4
    ld x13, 104(sp)       # a3
    ld x12, 96(sp)        # a2
    ld x11, 88(sp)        # a1
    ld x10, 80(sp)        # a0
    ld x9,  72(sp)        # s1
    ld x8,  64(sp)        # s0/fp
    ld x7,  56(sp)        # t2
    ld x6,  48(sp)        # t1
    ld x5,  40(sp)        # t0
    ld x4,  32(sp)        # tp
    ld x3,  24(sp)        # gp
    ld x1,  8(sp)         # ra

    addi sp, sp, 272

    sret