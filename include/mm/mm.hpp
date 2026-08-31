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

#ifdef ARCH_SPEC_RISCV
#include <mm/rvpaging.hpp>
#endif

#define PAGE_SIZE 4096
#define PAGE_SHIFT 12
#define BITMAP_BIT_SIZE 8

typedef uint64_t phys_addr_t;
typedef uint64_t virt_addr_t;

void mm_boot_init(boot_info_t *boot_info);
void *mm_boot_alloc(uint64_t size);

void pmm_init(uintptr_t ram_base, uint64_t ram_size);
void pmm_free_region(uintptr_t base, uint64_t size);
void pmm_reserve_region(uintptr_t base, uint64_t size);
uintptr_t pmm_alloc_frame(void);
void pmm_free_frame(uintptr_t phys_addr);

typedef struct free_node {
    struct free_node *next;
} free_node;

typedef struct list_node {
    struct list_node *prev, *next;
} list_node_t;

typedef struct frame {
    uintptr_t phys_addr;
    struct frame *next;
} frame_t;

typedef struct list {
    free_node *head;
} list_t;

typedef struct slab_cache {
    size_t obj_size;
    frame_t *slabs;
    list_t free_objects;
    spinlock_t lock;
} slab_cache_t;

void *slab_alloc(slab_cache_t *cache);
void slab_free(slab_cache_t *cache, void *obj);
slab_cache_t *slab_cache_create(size_t obj_size);

#define KMALLOC_MIN_SIZE 16
#define KMALLOC_MAX_SLAB 2048
#define NUM_SLAB_BUCKETS 8 //16, 32, 64, 128, 256, 512, 1024, 2048

typedef struct alloc_header {
    size_t size; //total usable
    slab_cache_t *cache; //null for large direct pmm allocs
} alloc_header_t;

void kheap_init(void);
void *kmalloc(size_t size);
void kfree(void *ptr);
void *kcalloc(size_t num, size_t size);
void *krealloc(void *ptr, size_t new_size);

void mm_init(boot_info_t *boot_info);

phys_addr_t vmm_virt_to_phys(page_table_t page_table, virt_addr_t va);
bool vmm_map_page(page_table_t page_table, virt_addr_t va, phys_addr_t pa, uint64_t flags);

phys_addr_t vmmk_virt_to_phys(virt_addr_t va);
bool vmmk_map_page(virt_addr_t va, phys_addr_t pa, uint64_t flags);

void vmm_init(boot_info_t *boot_info);

#endif