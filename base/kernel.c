/*
 * Vectra Kernel
 * Path: base/kernel.c
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
#include <ob/ob.hpp>

void kernel_main(boot_info_t *boot_info)
{
    klog_info("kern", "initializing kernel");
    mm_init(boot_info);
#ifdef ARCH_SPEC_RISCV
    vmm_init(boot_info);
#endif
    ob_init();
    object_test();
    
    while (1);
}