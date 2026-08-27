/*
 * Vectra Kernel
 * Path: arch/arm64/bootstrap.cpp
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
#include <libkern/klog.hpp>

void kernel_main(void);

extern "C" void boot_main(void)
{
    klog_info("boot", "starting Vectra Kernel");
    klog_info("cpu", "running at EL%d", cpu_get_el_num());

    interrupts_init();
    //test
    //asm("udf #0xdead");

    kernel_main();
    while (1);
}