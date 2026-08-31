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

#define MAX_CPUS 32

#define EXC_TYPE_SYNC   1
#define EXC_TYPE_IRQ    2
#define EXC_TYPE_FIQ    3
#define EXC_TYPE_SERR   4

#define EXC_TYPE_SSI    1
#define EXC_TYPE_STI    5
#define EXC_TYPE_SEI    9

#define PRIV_USER       0
#define PRIV_SUPERVISOR 1

#define SCAUSE_IS_IRQ(scause) ((scause & 0x8000000000000000ULL) >> 63)

typedef struct {
    uint64_t x0,  x1,  x2,  x3,  x4,  x5,  x6,  x7;
    uint64_t x8,  x9,  x10, x11, x12, x13, x14, x15;
    uint64_t x16, x17, x18, x19, x20, x21, x22, x23;
    uint64_t x24, x25, x26, x27, x28, x29; // x29 is fp
    uint64_t lr;  // x30
    uint64_t sp;  // stack pointer
} arm64_registers_t;

typedef struct {
    uint64_t zero; // x0
    uint64_t ra;   // x1
    uint64_t sp;   // x2
    uint64_t gp;   // x3
    uint64_t tp;   // x4
    uint64_t t0;   // x5
    uint64_t t1;   // x6
    uint64_t t2;   // x7
    uint64_t s0;   // x8 / fp
    uint64_t s1;   // x9
    uint64_t a0;   // x10
    uint64_t a1;   // x11
    uint64_t a2;   // x12
    uint64_t a3;   // x13
    uint64_t a4;   // x14
    uint64_t a5;   // x15
    uint64_t a6;   // x16
    uint64_t a7;   // x17
    uint64_t s2;   // x18
    uint64_t s3;   // x19
    uint64_t s4;   // x20
    uint64_t s5;   // x21
    uint64_t s6;   // x22
    uint64_t s7;   // x23
    uint64_t s8;   // x24
    uint64_t s9;   // x25
    uint64_t s10;  // x26
    uint64_t s11;  // x27
    uint64_t t3;   // x28
    uint64_t t4;   // x29
    uint64_t t5;   // x30
    uint64_t t6;   // x31
} riscv_registers_t;

typedef struct {
    uint64_t stack;
    uint64_t hartid;
} riscv_hart_info_t;

typedef struct {
    long error;
    long value;
} sbiret_t;

typedef int32_t (*interrupt_handler_t)(uint32_t, arm64_registers_t*);

void cpu_set_vbar_el1(uint64_t val);
void cpu_set_vbar_el2(uint64_t val);
void cpu_set_vaif(uint64_t val);

void enter_usermode(uint64_t user_pc, uint64_t user_sp);

void cpu_set_stvec(uint64_t addr);
void cpu_set_tp(uint64_t addr);

void halt();

void wfi();

void dsbsy();

void dmbsy();

uint64_t cpu_get_el();
uint64_t cpu_get_el_num();

uint64_t read_esr_el2();
uint64_t read_elr_el2();
uint64_t read_esr_el1();
uint64_t read_elr_el1();

uint64_t cpu_get_sstatus();
uint64_t cpu_get_sie();
uint64_t cpu_get_scause();
uint64_t cpu_get_sepc();
uint64_t cpu_get_stval();
uint64_t cpu_get_tp();
uint64_t cpu_get_cpu_id();

void cpu_halt_cpu_id(uint64_t cpu_id);

uint64_t cpu_get_membase();
uint64_t cpu_get_memsize();

sbiret_t sbi_ecall(int ext, int fid, unsigned long arg0, unsigned long arg1, unsigned long arg2, unsigned long arg3);
void sbi_putchar(char c);
sbiret_t sbi_start_hart(uint64_t hartid, uint64_t pc, uint64_t priv);
void sbi_send_ipi_all();

void interrupts_enable(bool irq, bool fiq);
void interrupts_init();

#ifdef ARCH_SPEC_ARM64
extern arm64_registers_t *dump_registers();
#elif defined(ARCH_SPEC_RISCV)
extern riscv_registers_t *dump_registers();
#endif

#endif