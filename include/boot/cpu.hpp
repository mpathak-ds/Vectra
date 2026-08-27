/*
 * Vectra Kernel
 * Path: include/boot/cpu.hpp
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

#ifndef BOOT_CPU_H
#define BOOT_CPU_H

#include <osdef.hpp>

void cpu_set_vbar(uint64_t val);
void cpu_set_vaif(uint64_t val);

uint64_t cpu_get_el();
uint64_t cpu_get_el_num();

#endif