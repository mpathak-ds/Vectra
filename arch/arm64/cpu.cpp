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
void cpu_set_vbar(uint64_t val)
{
    asm("msr vbar_el1, x0");
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