#ifndef RISCV_LOCK_H
#define RISCV_LOCK_H

#include <osdef.hpp>

typedef volatile uint32_t atomic_uint32_t;
typedef volatile int32_t  atomic_int32_t;

typedef struct spinlock {
    volatile uint32_t locked; // 0 = free, 1 = held
} spinlock_t;

#define SPINLOCK_INIT { .locked = 0 }

static inline void cpu_relax(void) {
    __asm__ volatile("fence rw, rw" ::: "memory");
}

static inline uint32_t atomic_exchange_acquire(volatile uint32_t *ptr, uint32_t new_val) {
    uint32_t old_val;

    __asm__ volatile (
        "amoswap.w.aq %0, %1, (%2)"
        : "=&r" (old_val)
        : "r" (new_val), "r" (ptr)
        : "memory"
    );

    return old_val;
}

static inline void atomic_store_release(volatile uint32_t *ptr, uint32_t new_val) {
    __asm__ volatile (
        "fence rw, w\n\t"
        "sw zero, 0(%0)"
        :
        : "r" (ptr)
        : "memory"
    );

}

static inline uint32_t atomic_load(const volatile uint32_t *ptr) {
    uint32_t val;
    __asm__ volatile (
        "lw %0, 0(%1)\n\t"
        "fence r, rw"
        : "=r" (val)
        : "r" (ptr)
        : "memory"
    );
    return val;
}

static inline uint32_t atomic_fetch_add(volatile uint32_t *ptr, uint32_t val) {
    uint32_t old_val;
    __asm__ volatile (
        "    amoadd.w.aqrl %0, %2, 0(%1) \n"
        : "=r" (old_val)
        : "r" (ptr), "r" (val)
        : "memory"
    );
    return old_val;
}

static inline uint32_t atomic_fetch_sub(volatile uint32_t *ptr, uint32_t val) {
    uint32_t old_val;
    uint32_t tmp;
    __asm__ volatile (
        "    neg           %1, %3        \n"
        "    amoadd.w.aqrl %0, %1, 0(%2) \n"
        : "=&r" (old_val), "=&r" (tmp)
        : "r" (ptr), "r" (val)
        : "memory"
    );
    return old_val;
}

#endif