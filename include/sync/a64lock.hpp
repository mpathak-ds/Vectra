/*
 * Vectra Kernel
 * Path: include/sync/a64lock.hpp
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

#ifndef ARM64_LOCK_H
#define ARM64_LOCK_H

#include <osdef.hpp>

typedef volatile uint32_t atomic_uint32_t;
typedef volatile int32_t  atomic_int32_t;

typedef struct spinlock {
    volatile uint32_t locked; // 0 = free, 1 = held
} spinlock_t;

#define SPINLOCK_INIT { .locked = 0 }

static inline void cpu_relax(void) {
    __asm__ volatile("wfe" ::: "memory");
}

static inline uint32_t arm64_atomic_exchange_acquire(volatile uint32_t *ptr, uint32_t new_val) {
    uint32_t old_val;
    uint32_t status;

    __asm__ volatile (
        "1:  ldaxr   %w0, [%2]        \n"
        "    stxr    %w1, %w3, [%2]   \n"
        "    cbnz    %w1, 1b          \n"
        : "=&r" (old_val), "=&r" (status)
        : "r" (ptr), "r" (new_val)
        : "memory"
    );

    return old_val;
}

static inline void arm64_atomic_store_release(volatile uint32_t *ptr, uint32_t new_val) {
    __asm__ volatile (
        "    stlr    %w1, [%0]        \n"
        "    sev                      \n"
        :
        : "r" (ptr), "r" (new_val)
        : "memory"
    );
}

static inline uint32_t atomic_load(const volatile uint32_t *ptr) {
    uint32_t val;

    __asm__ volatile (
        "    ldar    %w0, [%1]        \n"
        : "=r" (val)
        : "r" (ptr)
        : "memory"
    );

    return val;
}

static inline uint32_t atomic_fetch_add(volatile uint32_t *ptr, uint32_t val) {
    uint32_t old_val;
    uint32_t new_val;
    uint32_t status;

    __asm__ volatile (
        "1:  ldaxr   %w0, [%3]        \n"
        "    add     %w1, %w0, %w4    \n"
        "    stlxr   %w2, %w1, [%3]   \n"
        "    cbnz    %w2, 1b          \n"
        : "=&r" (old_val), "=&r" (new_val), "=&r" (status)
        : "r" (ptr), "r" (val)
        : "memory", "cc"
    );

    return old_val;
}

static inline uint32_t atomic_fetch_sub(volatile uint32_t *ptr, uint32_t val) {
    uint32_t old_val;
    uint32_t new_val;
    uint32_t status;

    __asm__ volatile (
        "1:  ldaxr   %w0, [%3]        \n"
        "    sub     %w1, %w0, %w4    \n"
        "    stlxr   %w2, %w1, [%3]   \n"
        "    cbnz    %w2, 1b          \n"
        : "=&r" (old_val), "=&r" (new_val), "=&r" (status)
        : "r" (ptr), "r" (val)
        : "memory", "cc"
    );

    return old_val;
}

#endif