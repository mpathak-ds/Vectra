/*
 * Vectra Kernel
 * Path: arch/riscv/bootstrap.cpp
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
#include <libkern/klog.hpp>

extern "C" char __heap_start[];
extern "C" char __heap_end[];
extern "C" char __text_start[];

void kernel_main(boot_info_t *boot_info);

static boot_info_t early_info;

extern "C" void boot_main(void)
{
    klog_info("boot", "starting Vectra Kernel");

    interrupts_init();

    klog_info("boot", "found boot heap to be : start=0x%x, end=0x%x", (uint8_t *)__heap_start, (uint8_t *)__heap_end);

    early_info.early_heap_start = (uint8_t *)__heap_start;
    early_info.early_heap_end = (uint8_t *)__heap_end;
    early_info.kernel_image_start = (uint8_t*)__text_start;
    early_info.machine_ram_base = cpu_get_membase();
    early_info.machine_ram_total = cpu_get_memsize();

    kernel_main(&early_info);
    while (1);
}