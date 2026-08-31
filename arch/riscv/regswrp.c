/*
 * Vectra Kernel
 * Path: arch/riscv/regs.c
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

#include <boot/cpu.hpp>
#include <libkern/klog.hpp>
#include <libkern/string.hpp>

riscv_registers_t regframe;

riscv_registers_t *regs_dump(riscv_registers_t *regs)
{
    memcpy(&regframe, &regs, sizeof(riscv_registers_t));
    return &regframe;
}