/*
 * Vectra Kernel
 * Path: libkern/klog.c
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
#include <libkern/hash.hpp>
#include <libkern/klog.hpp>
#include <libkern/string.hpp>
#include <mm/mm.hpp>

static inline size_t hash_u64(uint64_t key, size_t nbuckets) {
    key ^= key >> 33; key *= 0xff51afd7ed558ccdULL; //murmur64 finalizer cheap and good enough
    key ^= key >> 33;
    return key % nbuckets;
}

void hashtable_init(hashtable_t *ht, size_t nbuckets)
{
    ht->buckets = (hash_node_t**)(kmalloc(sizeof(hash_node_t*) * nbuckets)); //welcome to cpp i guess
    memset(ht->buckets, 0, sizeof(hash_node_t *) * nbuckets);
    ht->nbuckets   = nbuckets;
    ht->count      = 0;
    ht->node_cache = slab_cache_create(sizeof(hash_node_t));
}

void hashtable_insert(hashtable_t *ht, uint64_t key, void *value)
{
    hash_node_t *n = (hash_node_t*)(slab_alloc(ht->node_cache));
    n->key = key; n->value = value;
    size_t b = hash_u64(key, ht->nbuckets);
    n->next = ht->buckets[b]; //caller must hold whatever lock guards this table
    ht->buckets[b] = n;       //hashtable_t itself has no internal lock by design
    atomic_fetch_add(&ht->count, 1);
}

void *hashtable_lookup(hashtable_t *ht, uint64_t key)
{
    hash_node_t *n = ht->buckets[hash_u64(key, ht->nbuckets)];
    while (n) { if (n->key == key) return n->value; n = n->next; }
    return NULL;
}

void hashtable_remove(hashtable_t *ht, uint64_t key)
{
    hash_node_t **pp = &ht->buckets[hash_u64(key, ht->nbuckets)];
    while (*pp) {
        if ((*pp)->key == key) {
            hash_node_t *dead = *pp;
            *pp = dead->next;
            slab_free(ht->node_cache, dead);
            atomic_fetch_sub(&ht->count, 1);
            return;
        }
        pp = &(*pp)->next;
    }
}