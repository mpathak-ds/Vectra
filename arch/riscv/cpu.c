/*
 * Vectra Kernel
 * Path: arch/riscv/cpu.c
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

extern void enter_usermode(uint64_t user_pc, uint64_t user_sp);

void cpu_set_stvec(uint64_t addr)
{
    asm volatile("csrw stvec, %0" : : "r"(addr));
}

void cpu_set_tp(uint64_t addr)
{
    register unsigned long tp asm("tp");
    tp = addr;
}

uint64_t cpu_get_sstatus()
{
    uint64_t sstatus;

    asm volatile("csrr %0, sstatus" : "=r"(sstatus));
    return sstatus;
}

uint64_t cpu_get_sie()
{
    uint64_t sie;

    asm volatile("csrr %0, sie" : "=r"(sie));
    return sie;
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

uint64_t cpu_get_tp()
{
    register unsigned long tp asm("tp");
    return tp;
}

uint64_t cpu_get_cpu_id()
{
    riscv_hart_info_t *hart_info = (riscv_hart_info_t *)cpu_get_tp();

    return hart_info->hartid;
}

void cpu_halt_cpu_id(uint64_t cpu_id)
{
    sbi_send_ipi_all();
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