/*
 * Vectra Kernel
 * Path: arch/riscv/opensbi.c
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

sbiret_t sbi_ecall(int ext, int fid, unsigned long arg0, unsigned long arg1, unsigned long arg2, unsigned long arg3)
{
    sbiret_t ret;
    register unsigned long a0 __asm__("a0") = arg0;
    register unsigned long a1 __asm__("a1") = arg1;
    register unsigned long a2 __asm__("a2") = arg2;
    register unsigned long a3 __asm__("a3") = arg3;
    register unsigned long a6 __asm__("a6") = fid;
    register unsigned long a7 __asm__("a7") = ext;

    asm volatile (
        "ecall"
        : "+r" (a0), "+r" (a1), "+r" (a2), "+r" (a3)
        : "r" (a6), "r" (a7)
        : "memory"
    );

    ret.error = a0;
    ret.value = a1;
    return ret;
}

void sbi_putchar(char c)
{
    sbi_ecall(0x01, 0, (unsigned long)c, 0, 0, 0);
}

sbiret_t sbi_start_hart(uint64_t hartid, uint64_t pc, uint64_t priv)
{
    return sbi_ecall(0x48534D, 0, hartid, pc, priv, 0);
}

void sbi_send_ipi_all()
{
    sbi_ecall(0x04, 0, 0, 0, 0, 0);
}