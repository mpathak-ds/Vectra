/*
 * Vectra Kernel
 * Path: include/libkern/hash.hpp
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

#ifndef KERN_HASH_H
#define KERN_HASH_H

#include <osdef.hpp>
#include <sync/atomic.hpp>
#include <mm/mm.hpp>

typedef struct hash_node {
    uint64_t key;
    void *value;
    struct hash_node *next; //chaining for collisions
} hash_node_t;

typedef struct hashtable {
    hash_node_t **buckets;
    size_t nbuckets;
    atomic_uint32_t count;
    slab_cache_t *node_cache;
} hashtable_t;

void hashtable_init(hashtable_t *ht, size_t nbuckets);
void hashtable_insert(hashtable_t *ht, uint64_t key, void *value);
void *hashtable_lookup(hashtable_t *ht, uint64_t key);
void hashtable_remove(hashtable_t *ht, uint64_t key);

#endif