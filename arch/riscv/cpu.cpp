/*
 * Vectra Kernel
 * Path: arch/riscv/cpu.cpp
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

void cpu_set_stvec(uint64_t addr)
{
    asm volatile("csrw stvec, %0" : : "r"(addr));
}

uint64_t cpu_get_scause()
{
    uint64_t scause;

    asm volatile("csrr %0, scause" : "=r"(scause));
    return scause;
}

uint64_t cpu_get_sepc()
{
    uint64_t sepc;

    asm volatile("csrr %0, sepc" : "=r"(sepc));
    return sepc;
}

uint64_t cpu_get_stval()
{
    uint64_t stval;

    asm volatile("csrr %0, stval" : "=r"(stval));
    return stval;
}

void halt()
{
    for (;;) asm volatile("wfi");
}

uint64_t cpu_get_membase()
{
    return 0x80000000;
}

uint64_t cpu_get_memsize()
{
    return MACHINE_RAM_SIZE; //hardcoded... 128mb
}