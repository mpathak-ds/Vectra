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

static volatile uint64_t global_ticks;

static int32_t timer_handler(uint32_t iar, arm64_registers_t *regs)
{
    uint64_t freq, calculated_freq;

    timer_mask();

    global_ticks++;

    freq = timer_get_core_frequency();
    calculated_freq = freq / TIMER_FREQ;
    timer_set_frequency(10000000);
    timer_enable();

    klog_debug("timer", "interrupt");

    dsbsy();

    timer_unmask();

    return 0;
}

uint64_t timer_get_core_frequency()
{
    uint64_t freq;

    asm("mrs %0, cntfrq_el0" : "=r"(freq));

    return freq;
}

void timer_set_frequency(uint64_t freq)
{
    asm("msr cntp_tval_el0, %0" : : "r"(freq));
    wfi();
}

void timer_disable()
{
    uint64_t value;

    asm("mrs %0, cntp_ctl_el0" : "=r"(value));
    value &= ~TIMER_ENABLE;
    asm("msr cntp_ctl_el0, %0" : : "r"(value));
}

void timer_enable()
{
    uint64_t value;

    asm("mrs %0, cntp_ctl_el0" : "=r"(value));
    value |= TIMER_ENABLE;
    asm("msr cntp_ctl_el0, %0" : : "r"(value));
    wfi();
}

void timer_unmask()
{
    uint64_t ctrl;

    asm volatile("mrs %0, cntp_ctl_el0" : "=r"(ctrl));
    ctrl &= ~(1 << 1);
    asm volatile("msr cntp_ctl_el0, %0" : : "r"(ctrl));
}

void timer_mask()
{
    uint64_t ctrl;

    asm volatile("mrs %0, cntp_ctl_el0" : "=r"(ctrl));
    ctrl |= (1 << 1);
    asm volatile("msr cntp_ctl_el0, %0" : : "r"(ctrl));
}

void timer_wait(uint64_t ticks)
{
    uint64_t end = global_ticks + ticks;

    while (global_ticks < end) {
        ;
    }
}

void timer_init()
{
    uint64_t freq, calculated_freq;

    freq = timer_get_core_frequency();
    calculated_freq = freq / TIMER_FREQ;

    klog_info("timer", "core frequency: %d kHz", TO_KHZ(freq));

    gic_register_interrupt_handler(TIMER_IRQ_ID, timer_handler);
    gic_enable_interrupt(TIMER_IRQ_ID);
    gic_set_interrupt_priority(TIMER_IRQ_ID, GIC_PRIORITY_MEDIUM);

    timer_set_frequency(10000000);
    timer_enable();
}