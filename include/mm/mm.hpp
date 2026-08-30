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
#include <sync/atomic.hpp>

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

struct free_node {
    free_node *next;
};

struct frame {
    uintptr_t phys_addr;
    frame *next;
};
typedef struct frame frame_t;

struct list {
    free_node *head;
};
typedef struct list list_t;

typedef struct slab_cache {
    size_t obj_size;
    frame_t *slabs;
    list_t free_objects;
    spinlock_t lock;
} slab_cache_t;

void *slab_alloc(slab_cache_t *cache);
void slab_free(slab_cache_t *cache, void *obj);
slab_cache_t *slab_cache_create(size_t obj_size);

#endif