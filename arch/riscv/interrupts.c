/*
 * Vectra Kernel
 * Path: arch/riscv/interrupts.cpp
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
extern void interrupt_exception_handler(riscv_registers_t *regs);

void interrupts_set_tvec(uint64_t val)
{
    cpu_set_stvec(val);
}

void interrupts_init(void)
{
    //initialize exceptions first
    klog_info("boot", "enabling exceptions...");
    interrupts_set_tvec((uint64_t)trap_vector);
}

void interrupt_exception_handler(riscv_registers_t *regs)
{
    uint64_t scause = cpu_get_scause();
    uint64_t sepc = cpu_get_sepc();
    uint64_t stval = cpu_get_stval();

    if (!SCAUSE_IS_IRQ(scause)) {
        klog_critical("cpu", 
            "S-MODE EXCEPTION %d: SCAUSE=0x%x, SEPC=0x%x, STVAL=0x%x\n"
            "x0 =0x%x  ra =0x%x "
            "sp =0x%x  gp =0x%x\n"
            "tp =0x%x  t0 =0x%x "
            "t1 =0x%x  t2 =0x%x\n"
            "s0 =0x%x  s1 =0x%x "
            "a0 =0x%x  a1 =0x%x\n"
            "a2 =0x%x  a3 =0x%x "
            "a4 =0x%x  a5 =0x%x\n"
            "a6 =0x%x  a7 =0x%x "
            "s2 =0x%x  s3 =0x%x\n"
            "s4 =0x%x  s5 =0x%x "
            "s6 =0x%x  s7 =0x%x\n"
            "s8 =0x%x  s9 =0x%x "
            "s10=0x%x  s11=0x%x\n"
            "t3 =0x%x  t4 =0x%x "
            "t5 =0x%x  t6 =0x%x", 
            (scause & 0x7FFFFFFFFFFFFFFF), scause, sepc, stval,
            regs->zero, regs->ra,  regs->sp,  regs->gp,
            regs->tp,   regs->t0,  regs->t1,  regs->t2,
            regs->s0,   regs->s1,  regs->a0,  regs->a1,
            regs->a2,   regs->a3,  regs->a4,  regs->a5,
            regs->a6,   regs->a7,  regs->s2,  regs->s3,
            regs->s4,   regs->s5,  regs->s6,  regs->s7,
            regs->s8,   regs->s9,  regs->s10, regs->s11,
            regs->t3,   regs->t4,  regs->t5,  regs->t6
        );

        for (;;) asm volatile("wfi");
    }
}