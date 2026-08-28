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
#include <libkern/klog.hpp>
#include <mm/mm.hpp>

void kernel_main(boot_info_t *boot_info)
{
    klog_info("kern", "initializing kernel");
    mm_boot_init(boot_info);

    //test block, remove later
    void *test_ptr = mm_boot_alloc(128);
    if (!test_ptr) panic("kern", "boot alloc test failed");

    klog_info("kern", "alloc successful at 0x%x", test_ptr);

    while (1);
}