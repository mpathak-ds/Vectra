#ifndef RISCV_LOCK_H
#define RISCV_LOCK_H

#include <osdef.hpp>

typedef struct spinlock {
    volatile uint32_t locked; // 0 = free, 1 = held
} spinlock_t;

#define SPINLOCK_INIT { .locked = 0 }

static inline void cpu_relax(void) {
    __asm__ volatile("wfi" ::: "memory");
}

static inline uint32_t riscv_atomic_exchange_acquire(volatile uint32_t *ptr, uint32_t new_val) {
    uint32_t old_val;

    __asm__ volatile (
        "amoswap.w.aq %0, %1, (%2)"
        : "=&r" (old_val)
        : "r" (new_val), "r" (ptr)
        : "memory"
    );

    return old_val;
}

static inline void riscv_atomic_store_release(volatile uint32_t *ptr, uint32_t new_val) {
    __asm__ volatile (
        "amoswap.w.rl zero, %1, (%0)"
        :
        : "r" (ptr), "r" (new_val)
        : "memory"
    );
}

#endif