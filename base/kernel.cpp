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

//delete this unit test later
struct test_struct_t {
    uint64_t id;
    uint64_t data[3];
};

void mm_test_slab(void)
{
    klog_info("test", "starting slab allocator test suite...");

    slab_cache_t *cache = slab_cache_create(sizeof(test_struct_t));
    if (!cache) {
        panic("test", "slab_cache_create failed");
    }
    klog_info("test", "created slab cache for obj_size=%u", cache->obj_size);

    test_struct_t *obj1 = (test_struct_t *)slab_alloc(cache);
    test_struct_t *obj2 = (test_struct_t *)slab_alloc(cache);

    if (!obj1 || !obj2 || obj1 == obj2) {
        panic("test", "basic allocation failed or returned duplicate pointers");
    }

    if (((uintptr_t)obj1 % alignof(void *)) != 0 || ((uintptr_t)obj2 % alignof(void *)) != 0) {
        panic("test", "allocated pointer is misaligned");
    }

    obj1->id = 0xDEADBEEF;
    obj2->id = 0xCAFEBABE;
    klog_info("test", "alloc test passed: obj1=%p (id=0x%x), obj2=%p (id=0x%x)", 
              obj1, obj1->id, obj2, obj2->id);

    slab_free(cache, obj1);
    test_struct_t *obj3 = (test_struct_t *)slab_alloc(cache);

    if (obj3 != obj1) {
        panic("test", "freelist did not reuse recently freed object");
    }
    klog_info("test", "memory reuse test passed: recycled ptr=%p", obj3);

    const size_t BULK_COUNT = 300;
    test_struct_t *ptrs[BULK_COUNT];
    ptrs[0] = obj2;
    ptrs[1] = obj3;

    for (size_t i = 2; i < BULK_COUNT; i++) {
        ptrs[i] = (test_struct_t *)slab_alloc(cache);
        if (!ptrs[i]) {
            panic("test", "failed bulk allocation at index %u", i);
        }
        ptrs[i]->id = i;
    }
    klog_info("test", "bulk allocation passed: allocated %u objects across frames", BULK_COUNT);

    for (size_t i = 0; i < BULK_COUNT; i++) {
        slab_free(cache, ptrs[i]);
    }

    test_struct_t *obj_after_free = (test_struct_t *)slab_alloc(cache);
    if (!obj_after_free) {
        panic("test", "allocation failed after bulk free operations");
    }
    slab_free(cache, obj_after_free);

    klog_info("test", "ALL SLAB ALLOCATOR TESTS PASSED SUCCESSFULLY!");
}

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

    uint32_t value = pmm_alloc_frame();
    klog_debug("kern", "pmm_alloc_frame returned 0x%x", &value);

    mm_test_slab();

    while (1);
}