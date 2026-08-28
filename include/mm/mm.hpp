/*
 * Vectra Kernel
 * Path: include/mm/mm.hpp
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

#ifndef KERN_MM_H
#define KERN_MM_H

#include <osdef.hpp>

#define PAGE_SIZE 4096
#define PAGE_SHIFT 12
#define BITMAP_BIT_SIZE 8

extern "C" void mm_boot_init(boot_info_t *boot_info);
void *mm_boot_alloc(uint64_t size);

extern "C" void pmm_init(uintptr_t ram_base, uint64_t ram_size);
void pmm_free_region(uintptr_t base, uint64_t size);
void pmm_reserve_region(uintptr_t base, uint64_t size);
uintptr_t pmm_alloc_frame(void);
void pmm_free_frame(uintptr_t phys_addr);

#endif