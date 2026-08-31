/*
 * Vectra Kernel
 * Path: arch/riscv/bootstrap.c
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
#include <drivers/timer.hpp>
#include <mm/mm.hpp>
#include <libkern/klog.hpp>

extern char __heap_start[];
extern char __heap_end[];
extern char __text_start[];
extern char __text_end[];

void kernel_main(boot_info_t *boot_info);

static boot_info_t early_info;
static uint64_t bootstrap_cpu = -1;
static riscv_hart_info_t boot_hart_info[MAX_CPUS];
spinlock_t boot_spinlock = SPINLOCK_INIT;

void boot_main(uint64_t hartid)
{
    cpu_set_tp((uint64_t)&boot_hart_info[hartid]);
    boot_hart_info->hartid = hartid;
    asm volatile("mv %0, sp" : "=r"(boot_hart_info->stack));

    if (bootstrap_cpu != -1) {
        sbi_send_ipi_all();
        spin_lock(&boot_spinlock);
        klog_info("early", "hello from cpu %d", hartid);
        spin_unlock(&boot_spinlock);
        for (;;)
            ;
    }

    klog_info("early", "booting from cpu %d", hartid);
    klog_info("boot", "starting Vectra Kernel");

    klog_info("boot", "found boot heap to be : start=0x%x, end=0x%x", (uint8_t *)__heap_start, (uint8_t *)__heap_end);

    early_info.early_heap_start = (uint8_t *)__heap_start;
    early_info.early_heap_end = (uint8_t *)__heap_end;
    early_info.kernel_image_start = (uint8_t *)__text_start;
    early_info.kernel_image_end = (uint8_t *)__text_end;
    early_info.machine_ram_base = cpu_get_membase();
    early_info.machine_ram_total = cpu_get_memsize();

    klog_info("boot", "memory map: kernel base=0x%x, kernel end=0x%x", (uint8_t *)__text_start, (uint8_t *)__text_end);

    interrupts_init();
    timer_init();

    spin_lock_init(&boot_spinlock);

    klog_info("boot", "booting secondary cpus..");
    bootstrap_cpu = hartid;
    for (uint64_t hart = 0; hart < 0; hart++) {
        if (hart != bootstrap_cpu) {
            sbi_start_hart(hart, (uint64_t)__text_start, PRIV_SUPERVISOR);
        }
    }

    kernel_main(&early_info);
    while (1);
}