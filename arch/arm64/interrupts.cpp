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
 */

#include <osdef.hpp>
#include <boot/cpu.hpp>

__attribute__((aligned(2048)))
static void *vector_table[256];

bool interrupt_install(uint8_t irq, void* vector)
{
    //TODO
}

void interrupt_uninstall(uint8_t irq, void* vector)
{
    //TODO
}

void interrupts_init(void)
{
    //TODO
}

