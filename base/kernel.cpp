/*
 * Vectra Kernel
 * Path: base/kernel.cpp
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
#include <drivers/timer.hpp>
#include <libkern/klog.hpp>
#include <mm/mm.hpp>

void kernel_main(boot_info_t *boot_info)
{
    klog_info("kern", "initializing kernel");
    mm_boot_init(boot_info);

    pmm_init(boot_info->machine_ram_base, boot_info->machine_ram_total);

    //free all of memory, mark unused
    pmm_free_region(boot_info->machine_ram_base, boot_info->machine_ram_total);

    uintptr_t kern_start = (uintptr_t)boot_info->kernel_image_start;
    uintptr_t kern_end   = (uintptr_t)boot_info->early_heap_end;
    uint64_t  kern_size  = kern_end - kern_start;
    pmm_reserve_region(kern_start, kern_size); //reserve kernel image and boot heap

    klog_info("kern", "physical memory initialized (reserved 0x%x - 0x%x, size: %u KB)", 
              kern_start, kern_end, kern_size / 1024);

    timer_wait(10);

    klog_debug("kern", "sleeped for 10 ticks");

    while (1);
}