/*
 * Vectra Kernel
 * Path: arch/riscv/timer.c
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
#include <drivers/timer.hpp>
#include <libkern/klog.hpp>

static volatile uint64_t global_ticks;

void timer_handler(riscv_registers_t *regs)
{
    global_ticks++;

    uint64_t cmp_value = timer_get_cmp();

    // klog_debug("timer", "interrupt %d, %d", cmp_value, timer_get_time());

    cmp_value += TIMER_FREQ * 100;
    timer_set_cmp(cmp_value);
    asm volatile("fence.i" ::: "memory");
    asm volatile("fence" ::: "memory");
}

void timer_set_cmp(uint64_t cmp_value)
{
    asm volatile("csrw stimecmp, %0" : : "r"(cmp_value));
}

uint64_t timer_get_time()
{
    uint64_t timer_value;

    asm volatile("csrr %0, time" : "=r"(timer_value));
    return timer_value;
}

uint64_t timer_get_cmp()
{
    uint64_t timer_cmp_value;

    asm volatile("csrr %0, stimecmp" : "=r"(timer_cmp_value));
    return timer_cmp_value;
}

void timer_disable()
{
    asm volatile("csrc sie, %0" : : "r"(0x20));
}

void timer_enable()
{
    asm volatile("csrs sie, %0" : : "r"(0x20));
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
    uint64_t timer_value = timer_get_time();
    
    global_ticks = 0;

    timer_value += TIMER_FREQ * 100;
    timer_set_cmp(timer_value);
    timer_enable();

    klog_info("timer", "enabled timer: time=%d", timer_value);
}