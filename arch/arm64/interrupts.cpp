/*
 * Vectra Kernel
 * Path: arch/arm64/interrupts.cpp
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
 * Author: Mayank Pathak (mpathak)
 */

#include <osdef.hpp>
#include <boot/cpu.hpp>
#include <libkern/klog.hpp>

extern "C" void enable_exceptions(void);
extern "C" void vectors(void); //declaring it as a function gives you the address pointer
extern "C" void interrupt_exception_handler(uint32_t exc_type, arm64_registers_t *regs);
extern void gic_handle_interrupts(arm64_registers_t *regs);

void interrupts_set_vbar(uint64_t val)
{
    uint32_t el = cpu_get_el_num();

    //this is a must have due to cross platform support
    //u boot uses el2
    if (el == 1) {
        cpu_set_vbar_el1(val);
    } else if (el == 2) {
        cpu_set_vbar_el2(val);
    } else {
        klog_critical("cpu", "setting vbar failed! unsupported exception level=%d", el);
    }
}

void interrupts_enable(bool irq, bool fiq)
{
    uint32_t daif;
    uint32_t value = 0;

    if (irq) {
        value |= 1;
    }

    if (fiq) {
        value |= 2;
    }

    asm volatile("mrs %0, daif" : "=r"(daif));
    daif &= ~value;
    asm volatile("msr daif, %0" : : "r"(daif));
}

void interrupts_init(void)
{
    //initialize exceptions first
    klog_info("boot", "enabling exceptions...");
    interrupts_set_vbar((uint64_t)vectors);
    enable_exceptions();
}

extern "C" void interrupt_exception_handler(uint32_t exc_type, arm64_registers_t *regs)
{
    uint32_t el = cpu_get_el_num();
    uint64_t esr = 0;
    uint64_t elr = 0;

    if (el == 1) {
        esr = read_esr_el1();
        elr = read_elr_el1();
    } else if (el == 2) {
        esr = read_esr_el2();
        elr = read_elr_el2();
    }

    switch(exc_type) {
        case EXC_TYPE_FIQ: {
            gic_handle_interrupts(regs);
            break;
        }
        case EXC_TYPE_SERR: {
            klog_critical("cpu", 
                "EL%d EXCEPTION %d: ESR=0x%x, ELR=0x%x\n"
                "x0 =0x%x  x1 =0x%x "
                "x2 =0x%x  x3 =0x%x\n"
                "x4 =0x%x  x5 =0x%x "
                "x6 =0x%x  x7 =0x%x\n"
                "x8 =0x%x  x9 =0x%x "
                "x10=0x%x  x11=0x%x\n"
                "x12=0x%x  x13=0x%x "
                "x14=0x%x  x15=0x%x\n"
                "x16=0x%x  x17=0x%x "
                "x18=0x%x  x19=0x%x\n"
                "x20=0x%x  x21=0x%x "
                "x22=0x%x  x23=0x%x\n"
                "x24=0x%x  x25=0x%x "
                "x26=0x%x  x27=0x%x\n"
                "x28=0x%x  fp =0x%x "
                "lr =0x%x  sp =0x%x", 
                el, exc_type, esr, elr,
                regs->x0,  regs->x1,  regs->x2,  regs->x3,
                regs->x4,  regs->x5,  regs->x6,  regs->x7,
                regs->x8,  regs->x9,  regs->x10, regs->x11,
                regs->x12, regs->x13, regs->x14, regs->x15,
                regs->x16, regs->x17, regs->x18, regs->x19,
                regs->x20, regs->x21, regs->x22, regs->x23,
                regs->x24, regs->x25, regs->x26, regs->x27,
                regs->x28, regs->x29, regs->lr,  regs->sp
            );

            for (;;) asm volatile("wfe");
            break;
        }
        default: {
            klog_warn("cpu", "unknown exception type %d", exc_type);
        }
    }
}