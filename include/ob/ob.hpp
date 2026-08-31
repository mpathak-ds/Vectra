/*
 * Vectra Kernel
 * Path: include/ob/ob.hpp
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

#ifndef KERN_OB_H
#define KERN_OB_H

#include <osdef.hpp>
#include <sync/atomic.hpp>
#include <libkern/hash.hpp>

#define MAX_OBJECT_TYPES 64

struct object_header;

typedef struct object_type_ops
{
    //lifecycle never called directly
    int  (*create)(struct object_header *obj, void *create_args);
    void (*destroy)(struct object_header *obj);

    //optional generic
    ssize_t (*read)   (struct object_header *obj, uint64_t off, void *buf, size_t len);
    ssize_t (*write)  (struct object_header *obj, uint64_t off, const void *buf, size_t len);
    int     (*wait)   (struct object_header *obj, uint64_t timeout_ns);
    int     (*ioop)   (struct object_header *obj, uint32_t opcode, void *arg);
    void    (*on_revoke)(struct object_header *obj); //wake anyone blocked in wait
} object_type_ops_t;

typedef struct object_type
{
    const char *name;
    size_t instance_size; //header+payload
    object_type_ops_t ops;
    slab_cache_t *slab;
    uint32_t default_rights;
} object_type_t;

typedef struct object_manager
{
    object_type_t types[MAX_OBJECT_TYPES];
    atomic_uint32_t type_count;
    hashtable_t id_table;
    atomic_uint32_t next_object_id; //uint64 later?
    spinlock_t table_lock;
} object_manager_t;

typedef struct object_header
{
    uint32_t type_id;
    atomic_uint32_t refcount;
    atomic_uint32_t handle_count;
    uint32_t generation;
    uint64_t object_id;
    spinlock_t lock;
    bool destroyed;
    list_node_t type_list_link;
    uint8_t payload[];
} object_header_t;

#define OBJ_PAYLOAD(obj, T) ((T *)((obj)->payload))

void ob_init(void);
object_header_t *object_create(object_type_t *type, void *create_args);
object_type_t *object_type_register(const char *name, size_t instance_size, object_type_ops_t ops);
void object_ref(object_header_t *obj);
void object_deref(object_header_t *obj);
void object_test(void);
void object_maybe_destroy(object_header_t *obj);

object_type_t *ob_type_for(uint32_t type_id);

#define CAP_TABLE_MAX 4096
#define CAP_INVALID   ((cap_t)UINT32_MAX)

typedef uint32_t cap_t;

#define RIGHT_READ  (1u << 0)
#define RIGHT_WRITE (1u << 1)
#define RIGHT_WAIT  (1u << 2)
#define RIGHT_IOOP  (1u << 3)
#define RIGHT_MAP   (1u << 4)
#define RIGHT_GRANT (1u << 5)

typedef struct cap_entry
{
    object_header_t *object;
    uint32_t rights;
    uint32_t generation; //snapshot
} cap_entry_t;

typedef struct cap_table
{
    cap_entry_t entries[CAP_TABLE_MAX];
    atomic_uint32_t next_free_hint;
    spinlock_t lock;
} cap_table_t;

cap_table_t *cap_table_create(void);
cap_t object_grant(cap_table_t *table, object_header_t *obj, uint32_t rights);
int  object_close(cap_table_t *table, cap_t handle);
object_header_t *cap_lookup(cap_table_t *table, cap_t handle, uint32_t required_rights);

#endif