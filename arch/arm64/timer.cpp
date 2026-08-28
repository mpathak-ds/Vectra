/*
 * Vectra Kernel
 * Path: arch/arm64/timer.cpp
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

#include <drivers/timer.hpp>
#include <boot/cpu.hpp>
#include <drivers/gic.hpp>
#include <libkern/klog.hpp>

uint64_t timer_get_core_frequency()
{
    uint64_t freq;

    asm("mrs %0, cntfrq_el0" : "=r"(freq));

    return freq;
}

void timer_set_frequency(uint64_t freq)
{
    asm("msr cntp_tval_el0, %0" : : "r"(freq));
}

void timer_enable()
{
    uint64_t value;

    asm("mrs %0, cntp_ctl_el0" : "=r"(value));

    value |= TIMER_ENABLE;

    asm("msr cntp_ctl_el0, %0" : : "r"(value));
}

void timer_init()
{
    uint64_t freq, calculated_freq;

    freq = timer_get_core_frequency();
    calculated_freq = freq / TIMER_FREQ;

    klog_info("timer", "core frequency: %d kHz", TO_KHZ(freq));

    timer_set_frequency(calculated_freq);
    timer_enable();
    interrupts_enable();
    gic_enable_interrupt(TIMER_IRQ_ID);
    gic_set_interrupt_priority(TIMER_IRQ_ID, GIC_PRIORITY_MEDIUM);
}