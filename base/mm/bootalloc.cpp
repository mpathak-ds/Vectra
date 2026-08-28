/*
 * Vectra Kernel
 * Path: base/mm/bootalloc.cpp
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

//
// This is the dumbest possible allocator, a simple bump pointer over the boot region.
// The reason behind it's existence is otherwise we would have a chicken and egg paradox, as the memory manager
// requires objects for allocation, but the object manager requires the memory manager for allocation of objects.
//

static uint8_t *boot_heap_ptr = NULL;
static uint8_t *boot_heap_end = NULL;

extern "C" void mm_boot_init(boot_info_t *boot_info)
{
    klog_info("mm", "initializing boot memory");

    boot_heap_ptr = boot_info->early_heap_start;
    boot_heap_end = boot_info->early_heap_end;

    klog_info("mm", "initialized boot heap at 0x%x - 0x%x", boot_heap_ptr, boot_heap_end);
}

void *mm_boot_alloc(uint64_t size)
{
    if (!boot_heap_ptr) {
        klog_critical("mm", "uninitialized boot heap");
        return NULL;
    }

    size = ALIGN_UP(size, 16);
    if (boot_heap_ptr + size > boot_heap_end) {
        panic("boot heap ran out of memory");
        return NULL;
    }

    void *p = boot_heap_ptr;
    boot_heap_ptr += size;
    return p;
}