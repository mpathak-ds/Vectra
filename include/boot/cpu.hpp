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

#define EXC_TYPE_SYNC 1
#define EXC_TYPE_IRQ  2
#define EXC_TYPE_FIQ  3
#define EXC_TYPE_SERR 4

typedef struct {
    uint64_t x0,  x1,  x2,  x3,  x4,  x5,  x6,  x7;
    uint64_t x8,  x9,  x10, x11, x12, x13, x14, x15;
    uint64_t x16, x17, x18, x19, x20, x21, x22, x23;
    uint64_t x24, x25, x26, x27, x28, x29; // x29 is fp
    uint64_t lr;  // x30
    uint64_t sp;  // stack pointer
} arm64_registers_t;

typedef int32_t (*interrupt_handler_t)(uint32_t, arm64_registers_t*);

void cpu_set_vbar_el1(uint64_t val);
void cpu_set_vbar_el2(uint64_t val);
void cpu_set_vaif(uint64_t val);

void halt();

void wai();

uint64_t cpu_get_el();
uint64_t cpu_get_el_num();

uint64_t read_esr_el2();
uint64_t read_elr_el2();
uint64_t read_esr_el1();
uint64_t read_elr_el1();

uint64_t cpu_get_membase();
uint64_t cpu_get_memsize();

void interrupts_enable();
void interrupts_init();

extern "C" arm64_registers_t *dump_registers();

#endif