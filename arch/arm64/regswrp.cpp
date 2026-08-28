/*
 * Vectra Kernel
 * Path: arch/arm64/regs.cpp
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

arm64_registers_t regframe;

extern "C" arm64_registers_t *regs_dump(arm64_registers_t *regs)
{
    memcpy(&regframe, &regs, sizeof(arm64_registers_t));
    return &regframe;
}