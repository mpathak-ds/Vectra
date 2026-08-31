/*
 * Vectra Kernel
 * Path: base/mm/kheap.c
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
#include <sync/atomic.hpp>
#include <stdbool.h>

static slab_cache_t *kmalloc_caches[NUM_SLAB_BUCKETS];
static bool kheap_initialized = false;

static inline int get_bucket_index(size_t size)
{
    if (size <= 16)   return 0;
    if (size <= 32)   return 1;
    if (size <= 64)   return 2;
    if (size <= 128)  return 3;
    if (size <= 256)  return 4;
    if (size <= 512)  return 5;
    if (size <= 1024) return 6;
    if (size <= 2048) return 7;
    return -1;
}

void kheap_init(void)
{
    if (kheap_initialized) return;

    size_t bucket_size = KMALLOC_MIN_SIZE;
    for (int i = 0; i < NUM_SLAB_BUCKETS; i++) {
        kmalloc_caches[i] = slab_cache_create(bucket_size + sizeof(alloc_header_t));
        if (!kmalloc_caches[i]) {
            panic("mm", "kheap_init: failed to initialize slab bucket");
        }
        bucket_size <<= 1;
    }

    kheap_initialized = true;
    klog_info("mm", "kernel heap initialized");
}

void *kmalloc(size_t size)
{
    if (size == 0) return NULL;
    if (!kheap_initialized) {
        panic("mm", "kmalloc called before kheap_init");
        return NULL;
    }

    size_t total_size = size + sizeof(alloc_header_t);
    int bucket = get_bucket_index(size);

    alloc_header_t *header = NULL;

    if (bucket != -1) {
        slab_cache_t *cache = kmalloc_caches[bucket];
        header = (alloc_header_t*)slab_alloc(cache);
        if (!header) return NULL;

        header->size = size;
        header->cache = cache;
    } else {
        //large alloc goes to pmm
        uint64_t num_pages = ALIGN_UP(total_size, PAGE_SIZE) / PAGE_SIZE;
        
        uintptr_t phys = pmm_alloc_frame();
        if (!phys) return NULL;

        for (uint64_t i = 1; i < num_pages; i++) {
            uintptr_t next_phys = pmm_alloc_frame();
            if (next_phys != phys + (i * PAGE_SIZE)) {
                panic("mm", "kmalloc: non-contiguous frame allocation for large request");
            }
        }

        header = (alloc_header_t*)phys;
        header->size = size;
        header->cache = NULL;
    }

    return (void*)(header + 1);
}

void kfree(void *ptr)
{
    if (!ptr) return;

    alloc_header_t *header = ((alloc_header_t*)ptr) - 1;

    if (header->cache != NULL) {
        slab_free(header->cache, (void*)header);
    } else {
        //direct large pmm
        size_t total_size = header->size + sizeof(alloc_header_t);
        uint64_t num_pages = ALIGN_UP(total_size, PAGE_SIZE) / PAGE_SIZE;
        uintptr_t phys = (uintptr_t)header;

        for (uint64_t i = 0; i < num_pages; i++) {
            pmm_free_frame(phys + (i * PAGE_SIZE));
        }
    }
}

void *kcalloc(size_t num, size_t size)
{
    size_t total = num * size;
    //multiplication overflow?
    if (num != 0 && total / num != size) return NULL; //uh oh

    void *ptr = kmalloc(total);
    if (ptr) {
        uint8_t *byte_ptr = (uint8_t*)ptr;
        for (size_t i = 0; i < total; i++) {
            byte_ptr[i] = 0;
        }
    }
    return ptr;
}

void *krealloc(void *ptr, size_t new_size)
{
    if (!ptr) return kmalloc(new_size);
    if (new_size == 0) {
        kfree(ptr);
        return NULL;
    }

    alloc_header_t *header = ((alloc_header_t*)ptr) - 1;
    size_t old_size = header->size;

    if (new_size <= old_size) {
        return ptr;
    }

    void *new_ptr = kmalloc(new_size);
    if (!new_ptr) return NULL;

    uint8_t *src = (uint8_t*)ptr;
    uint8_t *dst = (uint8_t*)new_ptr;
    for (size_t i = 0; i < old_size; i++) {
        dst[i] = src[i];
    }

    kfree(ptr);
    return new_ptr;
}