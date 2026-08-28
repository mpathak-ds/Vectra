/*
 * Vectra Kernel
 * Path: arch/arm64/cpu.cpp
 *
 * Copyright (c) 2026 Driftless Software. All rights reserved.
 * Property of Driftless Software.
 *
 * NOTICE: This software is governed by a license agreement. 
 * Redistribution, modification, or use of this file, in whole or in part,
 * is strictly restricted to the terms specified in the 'LICENSE' file 
 * located at the root directory of this project repository.
 *
 * Author: Mayank Pathak (mpathak)
 */

#include <osdef.hpp>
#include <boot/cpu.hpp>

//x0 are used in these functions as the convention uses that for first arg
void cpu_set_vbar_el1(uint64_t val)
{
    asm("msr vbar_el1, x0");
}

void cpu_set_vbar_el2(uint64_t val)
{
    asm("msr vbar_el2, x0");
}

void cpu_set_vaif(uint64_t val)
{
    asm("msr daif, x0");
}

//returns raw encoding only
uint64_t cpu_get_el()
{
    uint64_t el;

    asm("mrs %0, CurrentEL" : "=r"(el));
    return el;
}

//returns actual EL
uint64_t cpu_get_el_num()
{
    return cpu_get_el() >> 2;
}

uint64_t read_esr_el2()
{
    uint64_t v;
    asm volatile("mrs %0, esr_el2" : "=r"(v));
    return v;
}

uint64_t read_elr_el2()
{
    uint64_t v;
    asm volatile("mrs %0, elr_el2" : "=r"(v));
    return v;
}

uint64_t read_esr_el1()
{
    uint64_t v;
    asm volatile("mrs %0, esr_el1" : "=r"(v));
    return v;
}

uint64_t read_elr_el1()
{
    uint64_t v;
    asm volatile("mrs %0, elr_el1" : "=r"(v));
    return v;
}

void halt()
{
    for (;;) asm volatile("wfe");
}

void wai()
{
    asm volatile("isb" : : : "memory");
}

uint64_t cpu_get_membase()
{
#ifdef MACHINE_OPI
    return 0x00200000;
#else
    return 0x40080000;
#endif
}

uint64_t cpu_get_memsize()
{
    return MACHINE_RAM_SIZE; //hardcoded... 128mb
}