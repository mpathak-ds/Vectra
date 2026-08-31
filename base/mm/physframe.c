/*
 * Vectra Kernel
 * Path: base/mm/physframe.c
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

//
// Basic physical frame allocator via bitmap
//

static uint8_t *frame_bitmap = NULL;
static uint64_t total_frames = 0;
static uint64_t free_frames = 0;
static uintptr_t mem_phys_base = 0;
static uint64_t last_free_index = 0;

static inline void bitmap_set(uint64_t bit)
{
    frame_bitmap[bit / 8] |= (1 << (bit % 8));
}

static inline void bitmap_clear(uint64_t bit)
{
    frame_bitmap[bit / 8] &= ~(1 << (bit % 8));
}

static inline bool bitmap_test(uint64_t bit)
{
    return (frame_bitmap[bit / 8] & (1 << (bit % 8))) != 0;
}

void pmm_init(uintptr_t ram_base, uint64_t ram_size)
{
    mem_phys_base = ram_base;
    total_frames = ram_size / PAGE_SIZE;
    free_frames = total_frames;

    uint64_t bitmap_size = ALIGN_UP(total_frames / BITMAP_BIT_SIZE, 8);

    klog_info("mm", "allocating %u bytes for frame bitmap (%u total frames)", bitmap_size, total_frames);

    frame_bitmap = (uint8_t*)mm_boot_alloc(bitmap_size); //boot heap allocator
    if (!frame_bitmap) panic("mm", "failed to allocate physical memory bitmap");

    for (uint64_t i = 0; i < bitmap_size; i++) {
        frame_bitmap[i] = 0xFF; //USED
    }

    klog_info("mm", "physical frame allocator initialized successfully");
}

void pmm_free_region(uintptr_t base, uint64_t size)
{
    uint64_t start_frame = (base - mem_phys_base) / PAGE_SIZE;
    uint64_t count = size / PAGE_SIZE;

    for (uint64_t i=0; i < count; i++) {
        if (bitmap_test(start_frame+i)) {
            bitmap_clear(start_frame+i);
            free_frames++;
        }
    }
}

void pmm_reserve_region(uintptr_t base, uint64_t size)
{
    uint64_t start_frame = (base - mem_phys_base) / PAGE_SIZE;
    uint64_t count = ALIGN_UP(size, PAGE_SIZE) / PAGE_SIZE;

    for (uint64_t i = 0; i < count; i++) {
        if (!bitmap_test(start_frame + i)) {
            bitmap_set(start_frame + i);
            if (free_frames > 0) free_frames--;
        }
    }
}

uintptr_t pmm_alloc_frame(void)
{
    if (free_frames == 0) {
        panic("mm", "out of physical memory frames");
        return 0;
    }

    for (uint64_t i = last_free_index; i < total_frames; i++) {
        if (!bitmap_test(i)) {
            bitmap_set(i);
            free_frames--;
            last_free_index = i + 1;
            return mem_phys_base + (i * PAGE_SIZE);
        }
    }

    //wrap around
    for (uint64_t i = 0; i < last_free_index; i++) {
        if (!bitmap_test(i)) {
            bitmap_set(i);
            free_frames--;
            last_free_index = i + 1;
            return mem_phys_base + (i * PAGE_SIZE);
        }
    }

    return 0;
}

void pmm_free_frame(uintptr_t phys_addr)
{
    if (phys_addr < mem_phys_base) return;

    uint64_t frame_idx = (phys_addr - mem_phys_base) / PAGE_SIZE;
    if (frame_idx >= total_frames) return;

    if (bitmap_test(frame_idx)) {
        bitmap_clear(frame_idx);
        free_frames++;
        if (frame_idx < last_free_index) {
            last_free_index = frame_idx;
        }
    }
}