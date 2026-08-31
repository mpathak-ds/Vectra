/*
 * Vectra Kernel
 * Path: base/ob/cap.c
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

extern object_manager_t g_om_ref(void); //visible

cap_table_t *cap_table_create(void)
{
    cap_table_t *t = (cap_table_t *)kmalloc(sizeof(cap_table_t));
    if (!t) return NULL;

    memset(t, 0, sizeof(*t));
    t->lock = (spinlock_t)SPINLOCK_INIT;
    return t;
}

cap_t object_grant(cap_table_t *table, object_header_t *obj, uint32_t rights)
{
    object_type_t *type = ob_type_for(obj->type_id);
    if ((rights & type->default_rights) != rights) {
        klog_info("cap", "grant rejected: rights exceed type default_rights");
        return CAP_INVALID;
    }

    spin_lock(&table->lock);
    cap_t slot = CAP_INVALID;
    uint32_t hint = atomic_load(&table->next_free_hint);

    for (uint32_t i = 0; i < CAP_TABLE_MAX; i++) {
        uint32_t idx = (hint + i) % CAP_TABLE_MAX;
        if (table->entries[idx].object == NULL) {
            table->entries[idx] = (cap_entry_t){
                .object = obj,
                .rights = rights,
                .generation = obj->generation
            };
            atomic_store_release(&table->next_free_hint, idx + 1);
            slot = idx;
            break;
        }
    }
    spin_unlock(&table->lock);

    if (slot != CAP_INVALID) {
        atomic_fetch_add(&obj->handle_count, 1);
    } else {
        klog_info("cap", "grant failed: table full");
    }

    return slot;
}

int object_close(cap_table_t *table, cap_t handle)
{
    if (handle >= CAP_TABLE_MAX) return -1;

    spin_lock(&table->lock);
    object_header_t *obj = table->entries[handle].object;
    if (!obj) {
        spin_unlock(&table->lock);
        return -1;
    }
    table->entries[handle].object = NULL;
    spin_unlock(&table->lock);

    atomic_fetch_sub(&obj->handle_count, 1);
    object_maybe_destroy(obj);
    return 0;
}

object_header_t *cap_lookup(cap_table_t *table, cap_t handle, uint32_t required_rights)
{
    if (handle >= CAP_TABLE_MAX) return NULL;

    spin_lock(&table->lock);
    cap_entry_t *e = &table->entries[handle];
    object_header_t *obj = e->object;

    if (!obj) { spin_unlock(&table->lock); return NULL; }

    if (obj->generation != e->generation) {
        //stale
        table->entries[handle].object = NULL;
        spin_unlock(&table->lock);
        atomic_fetch_sub(&obj->handle_count, 1);
        object_maybe_destroy(obj);
        return NULL;
    }

    if ((e->rights & required_rights) != required_rights) {
        spin_unlock(&table->lock);
        return NULL;
    }

    object_ref(obj);
    spin_unlock(&table->lock);
    return obj;
}