/*
 * Vectra Kernel
 * Path: base/mm/slab.c
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

static slab_cache_t cache_metadata_cache;
static bool cache_meta_initialized = false;

static void slab_grow(slab_cache_t *cache)
{
    uintptr_t phys = pmm_alloc_frame();
    if (!phys) {
        panic("mm", "slab_grow: out of physical memory");
        return;
    }

    //assuming identity but need to change after we do the vmm
    uint8_t *virt = (uint8_t*)phys; 

    frame_t *frame_meta = (frame_t*)virt;
    frame_meta->phys_addr = phys;
    frame_meta->next = cache->slabs;
    cache->slabs = frame_meta;

    uintptr_t start = ALIGN_UP((uintptr_t)(virt + sizeof(frame_t)), alignof(void*));
    uintptr_t end = (uintptr_t)virt + PAGE_SIZE;

    for (uintptr_t ptr = start; ptr + cache->obj_size <= end; ptr += cache->obj_size) {
        free_node *node = (free_node*)ptr;
        node->next = cache->free_objects.head;
        cache->free_objects.head = node;
    }
}

slab_cache_t *slab_cache_create(size_t obj_size)
{
    if (obj_size < sizeof(free_node)) {
        obj_size = sizeof(free_node);
    }
    obj_size = ALIGN_UP(obj_size, alignof(void*));

    slab_cache_t *cache = NULL;

    if (!cache_meta_initialized) {
        //bootstrap
        cache_metadata_cache.obj_size = sizeof(slab_cache_t);
        cache_metadata_cache.slabs = NULL;
        cache_metadata_cache.free_objects.head = NULL;
        spin_lock_init(&cache_metadata_cache.lock);
        cache_meta_initialized = true;
        
        cache = (slab_cache_t*)slab_alloc(&cache_metadata_cache);
    } else {
        cache = (slab_cache_t*)slab_alloc(&cache_metadata_cache);
    }

    if (!cache) return NULL;

    cache->obj_size = obj_size;
    cache->slabs = NULL;
    cache->free_objects.head = NULL;
    spin_lock_init(&cache->lock);

    return cache;
}

void *slab_alloc(slab_cache_t *cache)
{
    if (!cache) return NULL;

    spin_lock(&cache->lock);

    if (!cache->free_objects.head) {
        slab_grow(cache);
    }

    free_node *node = cache->free_objects.head;
    if (node) {
        cache->free_objects.head = node->next;
    }

    spin_unlock(&cache->lock);
    return (void*)node;
}

void slab_free(slab_cache_t *cache, void *obj)
{
    if (!cache || !obj) return;

    spin_lock(&cache->lock);

    free_node *node = (free_node*)obj;
    node->next = cache->free_objects.head;
    cache->free_objects.head = node;

    spin_unlock(&cache->lock);
}