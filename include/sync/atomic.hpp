/*
 * Vectra Kernel
 * Path: include/sync/atomic.hpp
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

#ifndef KERN_BASE_ATOMIC_H
#define KERN_BASE_ATOMIC_H

#include <osdef.hpp>
#include <sync/a64lock.hpp>

static inline void spin_lock_init(spinlock_t *lock) {
    lock->locked = 0;
}

static inline void spin_lock(spinlock_t *lock) {
#ifdef ARCH_SPEC_ARM64
    while (arm64_atomic_exchange_acquire(&lock->locked, 1) != 0) {
        while (lock->locked != 0) {
            cpu_relax();
        }
    }
#endif
}

static inline void spin_unlock(spinlock_t *lock) {
#ifdef ARCH_SPEC_ARM64
    arm64_atomic_store_release(&lock->locked, 0);
#endif
}

#endif