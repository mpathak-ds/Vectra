/*
 * Vectra Kernel
 * Path: arch/riscv/interrupts.c
 *
 * Copyright (c) 2026 Driftless Software. All rights reserved.
 * Property of Driftless Software.
 *
 * NOTICE: This software is governed by a license agreement. 
 * Redistribution, modification, or use of this file, in whole or in part,
 * is strictly restricted to the terms specified in the 'LICENSE' file 
 * located at the root directory of this project repository.
 *
 * Author: Daniil Dunaeff (dan7)
 */

#include <osdef.hpp>
#include <boot/cpu.hpp>
#include <libkern/klog.hpp>

extern void trap_vector(void); //declaring it as a function gives you the address pointer
extern void interrupt_exception_handler(uint32_t usermode, riscv_registers_t *regs);
extern void timer_handler(riscv_registers_t *regs);

void interrupts_set_tvec(uint64_t val)
{
    cpu_set_stvec(val);
}

void interrupts_enable(bool irq, bool fiq)
{
    asm volatile("csrs sie, %0" : : "r"(0x02));
    asm volatile("csrs sstatus, %0" : : "r"(0x02));
}

void interrupts_init(void)
{
    //initialize exceptions first
    klog_info("boot", "enabling exceptions...");
    interrupts_set_tvec((uint64_t)trap_vector);
    interrupts_enable(FALSE, FALSE);
}

void interrupt_exception_handler(uint32_t usermode, riscv_registers_t *regs)
{
    asm volatile("csrc sip, %0" : : "r"(0x02));
    uint64_t scause = cpu_get_scause();
    uint64_t sepc = cpu_get_sepc();
    uint64_t stval = cpu_get_stval();

    if (!SCAUSE_IS_IRQ(scause)) {
        if ((scause & 0x7FFFFFFFFFFFFFFF) == 15 || (scause & 0x7FFFFFFFFFFFFFFF) == 13 || (scause & 0x7FFFFFFFFFFFFFFF) == 12) {
            klog_critical("cpu", "page fault at 0x%llx", stval);
        }

        klog_critical("cpu", 
            "%c-MODE EXCEPTION %d: SCAUSE=0x%llx, SEPC=0x%llx, STVAL=0x%llx\n"
            "x0 =0x%llx  ra =0x%llx "
            "sp =0x%llx  gp =0x%llx\n"
            "tp =0x%llx  t0 =0x%llx "
            "t1 =0x%llx  t2 =0x%llx\n"
            "s0 =0x%llx  s1 =0x%llx "
            "a0 =0x%llx  a1 =0x%llx\n"
            "a2 =0x%llx  a3 =0x%llx "
            "a4 =0x%llx  a5 =0x%llx\n"
            "a6 =0x%llx  a7 =0x%llx "
            "s2 =0x%llx  s3 =0x%llx\n"
            "s4 =0x%llx  s5 =0x%llx "
            "s6 =0x%llx  s7 =0x%llx\n"
            "s8 =0x%llx  s9 =0x%llx "
            "s10=0x%llx  s11=0x%llx\n"
            "t3 =0x%llx  t4 =0x%llx "
            "t5 =0x%llx  t6 =0x%llx", 
            usermode == 1 ? 'U' : 'S', (scause & 0x7FFFFFFFFFFFFFFF), scause, sepc, stval,
            regs->zero, regs->ra,  regs->sp,  regs->gp,
            regs->tp,   regs->t0,  regs->t1,  regs->t2,
            regs->s0,   regs->s1,  regs->a0,  regs->a1,
            regs->a2,   regs->a3,  regs->a4,  regs->a5,
            regs->a6,   regs->a7,  regs->s2,  regs->s3,
            regs->s4,   regs->s5,  regs->s6,  regs->s7,
            regs->s8,   regs->s9,  regs->s10, regs->s11,
            regs->t3,   regs->t4,  regs->t5,  regs->t6
        );

        panic("cpu", "exception");
    } else {
        switch(scause & 0x7FFFFFFFFFFFFFFF) {
            case EXC_TYPE_SSI: {
                halt();
                break;
            }
            case EXC_TYPE_STI: {
                timer_handler(regs);
                break;
            }
        }
    }
}