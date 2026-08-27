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
extern "C" void interrupt_exception_handler();

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

void interrupts_init(void)
{
    //initialize exceptions first
    klog_info("boot", "enabling exceptions...");
    interrupts_set_vbar((uint64_t)vectors);
    enable_exceptions();
}

extern "C" void interrupt_exception_handler()
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

    klog_critical("cpu", "EL%d EXCEPTION: ESR=0x%x ELR=0x%x", el, esr, elr);

    for (;;) asm volatile("wfe");
}