.section ".text.boot"

.global vectors
.global vector_entry_stub
.global enable_exceptions

.extern interrupt_exception_handler

//128 byte alignment macro
.macro vector label
    .p2align 7
    b \label
.endm

enable_exceptions:
    msr daif, xzr //unmask all
    ret

.p2align 11
vectors:
    vector vector_entry_stub
    vector vector_entry_stub
    vector vector_entry_stub
    vector vector_entry_stub

    vector vector_entry_stub
    vector vector_entry_stub
    vector vector_entry_stub
    vector vector_entry_stub

    vector vector_entry_stub
    vector vector_entry_stub
    vector vector_entry_stub
    vector vector_entry_stub

    vector vector_entry_stub
    vector vector_entry_stub
    vector vector_entry_stub
    vector vector_entry_stub

vector_entry_stub:
    //preserve registers
    stp x0, x1, [sp, #-16]!
    stp x2, x3, [sp, #-16]!

    bl interrupt_exception_handler //handler in CPP

    ldp x2, x3, [sp], #16
    ldp x0, x1, [sp], #16

    eret //similar to intel iret