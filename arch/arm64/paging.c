/*
 * Vectra Kernel
 * Path: arch/arm64/paging.c
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

#include <boot/cpu.hpp>
#include <libkern/klog.hpp>
#include <libkern/string.hpp>
#include <sync/atomic.hpp>
#include <mm/mm.hpp>
#include <stdbool.h>

//fixed pool no recycling yet
#define ASID_POOL_SIZE 256
static bool        g_asid_used[ASID_POOL_SIZE];
static spinlock_t  g_asid_lock = SPINLOCK_INIT;

static int16_t asid_alloc()
{
    spin_lock(&g_asid_lock);

    for (int i = 1; i < ASID_POOL_SIZE; i++) {
        if (!g_asid_used[i]) {
            g_asid_used[i] = true;
            spin_unlock(&g_asid_lock);
            return (int16_t)i;
        }
    }

    spin_unlock(&g_asid_lock);

    return -1; //exhausted
}

static void asid_free(uint16_t asid)
{
    spin_lock(&g_asid_lock);

    g_asid_used[asid]=false;

    spin_unlock(&g_asid_lock);
}

static uint64_t alloc_table_page()
{
    uintptr_t phys = pmm_alloc_frame();
    if (!phys) return 0;
    memset(PHYS_TO_VIRT((void *)(phys)), 0, PAGE_SIZE);
    return phys;
}

static void free_table_page(uintptr_t phys)
{
    pmm_free_frame(phys);
}

static inline uint64_t level_index(uint64_t va, int level)
{
    int shift = PAGE_SHIFT + VA_BITS_PER_LEVEL * (PT_LEVELS - 1 - level);
    return (va >> shift) & (PT_ENTRIES - 1);
}

static inline void tlb_invalidate_va_asid(uint64_t va, uint16_t asid)
{
    uint64_t arg = ((uint64_t)asid << 48) | ((va >> 12) & 0xFFFFFFFFFFFUL);
    asm volatile("dsb ishst");
    asm volatile("tlbi vae1is, %0" :: "r"(arg));
    asm volatile("dsb ish");
    asm volatile("isb");
}

pagetable_root_t *arch_pagetable_create(void)
{
    pagetable_root_t *root = (pagetable_root_t *)kmalloc(sizeof(pagetable_root_t));
    if (!root) return NULL;

    uint64_t l0_phys = alloc_table_page();
    if (!l0_phys) {
        kfree_sized(root, sizeof(*root)); 
        return NULL;
    }

    int16_t asid = asid_alloc();
    if (asid < 0) {
        free_table_page(l0_phys);
        kfree_sized(root, sizeof(*root));
        return NULL;
    }

    root->ttbr0_phys = l0_phys;
    root->asid = (uint16_t)asid;
    return root;
}

static void free_table_recursive(uint64_t table_phys, int level)
{
    uint64_t *table = (uint64_t *)PHYS_TO_VIRT((table_phys));

    if (level < PT_LEVELS - 1) {
        for (int i = 0; i < PT_ENTRIES; i++) {
            uint64_t entry = table[i];
            if ((entry & PTE_VALID) && (entry & PTE_TABLE)) {
                free_table_recursive(entry & PTE_ADDR_MASK, level + 1);
            }
        }
    }

    free_table_page(table_phys);
}

void arch_pagetable_destroy(pagetable_root_t *root)
{
    free_table_recursive(root->ttbr0_phys, 0);
    asid_free(root->asid);
    kfree_sized(root, sizeof(*root));
}

static uint64_t *walk_create(uint64_t table_phys, uint64_t va, int level)
{
    uint64_t *table = (uint64_t *)PHYS_TO_VIRT((table_phys));
    uint64_t idx = level_index(va, level);

    if (level == PT_LEVELS - 1) return &table[idx];

    if (!(table[idx] & PTE_VALID)) {
        uint64_t child_phys = alloc_table_page();
        if (!child_phys) return NULL;
        table[idx] = PTE_ADDR(child_phys) | PTE_TABLE | PTE_VALID;
    }
    return walk_create(table[idx] & PTE_ADDR_MASK, va, level + 1);
}

int arch_map_page(pagetable_root_t *root, uint64_t va, uint64_t phys, uint32_t prot)
{
    uint64_t *pte = walk_create(root->ttbr0_phys, va, 0);
    if (!pte) return -1;

    uint64_t attrs = PTE_ATTR_IDX(MAIR_IDX_NORMAL) | PTE_SH_INNER | PTE_AF | PTE_NG;

    attrs |= (prot & PROT_WRITE) ? PTE_AP_RW_ALL : PTE_AP_RO_ALL;
    if (!(prot & PROT_EXEC)) attrs |= PTE_UXN | PTE_PXN;
    else                     attrs |= PTE_PXN;

    *pte = PTE_ADDR(phys) | PTE_TABLE | PTE_VALID | attrs;

    tlb_invalidate_va_asid(va, root->asid);
    return 0;
}

int arch_unmap_page(pagetable_root_t *root, uint64_t va)
{
    uint64_t *pte = walk_create(root->ttbr0_phys, va, 0);
    if (!pte || !(*pte & PTE_VALID)) return -1;

    *pte = 0;
    tlb_invalidate_va_asid(va, root->asid);
    return 0;
}