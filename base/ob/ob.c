/*
 * Vectra Kernel
 * Path: base/ob/ob.c
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
#include <ob/ob.hpp>
#include <libkern/hash.hpp>
#include <sync/atomic.hpp>
#include <libkern/string.hpp>
#include <stdbool.h>

static object_manager_t g_om;

void ob_init(void)
{
    hashtable_init(&g_om.id_table, 4096);
    g_om.table_lock = (spinlock_t)SPINLOCK_INIT;
    g_om.next_object_id = 1; //reserve 0 as invalid sentinel
    g_om.type_count = 0;
}

/* This function registers a specific object type like a file */
object_type_t *object_type_register(const char *name, size_t instance_size, object_type_ops_t ops)
{
    uint32_t idx = atomic_fetch_add(&g_om.type_count, 1);
    if (idx >= MAX_OBJECT_TYPES) panic("ob", "object type table full!");

    object_type_t *t = &g_om.types[idx];
    t->name = name;
    t->instance_size = sizeof(object_header_t) + instance_size; //header+payload
    t->ops = ops;
    t->slab = slab_cache_create(t->instance_size);

    return t;
}

/* This function actually creates an object of some specific type */
object_header_t *object_create(object_type_t *type, void *create_args)
{
    void *mem = slab_alloc(type->slab);
    if (!mem) return NULL;

    object_header_t *obj = (object_header_t*)mem;
    obj->type_id = type - g_om.types; //index
    obj->refcount = 1;
    obj->handle_count = 0;
    obj->generation = 0;
    obj->object_id = atomic_fetch_add(&g_om.next_object_id, 1);
    obj->lock = (spinlock_t)SPINLOCK_INIT;

    if (type->ops.create && type->ops.create(obj, create_args) != 0)
    {
        slab_free(type->slab, mem);
        return NULL;
    }

    spin_lock(&g_om.table_lock);
    hashtable_insert(&g_om.id_table, obj->object_id, obj);
    spin_unlock(&g_om.table_lock);

    return obj;
}

/* This function references an object atomically*/
void object_ref(object_header_t *obj)
{
    atomic_fetch_add(&obj->refcount, 1);
}

/* This function dereferences an object atomically */
void object_deref(object_header_t *obj)
{
    if (atomic_fetch_sub(&obj->refcount, 1) == 1) {
        object_maybe_destroy(obj);
    }
}

/* This function destroys an object if its reference and handle counts are zero */
void object_maybe_destroy(object_header_t *obj)
{
    spin_lock(&obj->lock);
    //both has to be zero
    bool go = (atomic_load(&obj->refcount) == 0 && atomic_load(&obj->handle_count) && !obj->destroyed);
    if (go) obj->destroyed = true;
    spin_unlock(&obj->lock);

    if (!go) return; //not actually dead yet

    object_type_t *type = &g_om.types[obj->type_id];
    if (type->ops.destroy) type->ops.destroy(obj);

    spin_lock(&g_om.table_lock);
    hashtable_remove(&g_om.id_table, obj->object_id);
    spin_unlock(&g_om.table_lock);

    slab_free(type->slab, obj);
}

/* Test callbacks */

static int test_object_create(object_header_t *obj, void *args)
{
    klog_info("test", "object create callback");
    return 0;
}

static void test_object_destroy(object_header_t *obj)
{
    klog_info("test", "object destroy callback");
}

/* Tests */

void object_test(void)
{
    klog_info("test", "[ob] starting");

    object_type_ops_t ops = {};
    ops.create = test_object_create;
    ops.destroy = test_object_destroy;

    object_type_t *type =
        object_type_register("test", 32, ops);

    if (!type) {
        klog_info("test", "[ob] FAIL: type registration");
        return;
    }

    klog_info("test", "[ob] PASS: type registration");

    object_header_t *obj =
        object_create(type, NULL);

    if (!obj) {
        klog_info("test", "[ob] FAIL: object creation");
        return;
    }

    klog_info("test", "[ob] PASS: object creation");

    if (atomic_load(&obj->refcount) != 1) {
        klog_info("test", "[ob] FAIL: initial refcount");
        return;
    }

    klog_info("test", "[ob] PASS: initial refcount");

    object_ref(obj);

    if (atomic_load(&obj->refcount) != 2) {
        klog_info("test", "[ob] FAIL: object_ref");
        return;
    }

    klog_info("test", "[ob] PASS: object_ref");

    object_deref(obj);

    if (atomic_load(&obj->refcount) != 1) {
        klog_info("test", "[ob] FAIL: first deref");
        return;
    }

    klog_info("test", "[ob] PASS: first deref");

    object_deref(obj);

    klog_info("test", "[ob] PASS: final deref/destroy");
    klog_info("test", "[ob] ALL TESTS PASSED");
}