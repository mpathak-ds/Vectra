/*
 * Vectra Kernel
 * Path: arch/riscv/paging.c
 *
 * Copyright (c) 2026 Driftless Software. All rights reserved.
 * Property of Driftless Software.
 *
 * NOTICE: This software is governed by a license agreement. 
 * Redistribution, modification, or use of this file, in whole or in part,
 * is strictly restricted to the terms specified in the 'LICENSE' file 
 * located at the root directory of this project repository.
 *
 * Author: Daniil Dunaeff (dan7)
 */

#include <mm/rvpaging.hpp>
#include <mm/mm.hpp>
#include <osdef.hpp>
#include <boot/cpu.hpp>
#include <libkern/klog.hpp>
#include <libkern/string.hpp>

__attribute__((aligned(4096)))
static page_table_t kernel_page_table;

static inline pte_t pa_to_pte_ppn(uint64_t pa)
{
    return (pa >> 12) << 10;
}

static inline phys_addr_t pte_to_pa(pte_t pte)
{
    return (pte >> 10) << 12;
}

static inline void vmm_install_page_table(page_table_t page_table)
{
    uint64_t pt_addr = (uint64_t)page_table;

    klog_info("vmm", "root page table physical address: 0x%x", pt_addr);

    asm volatile(
        "srli t0, %0, 12\n\t"
        "li   t1, 8\n\t"
        "li   t2, 60\n\t"
        "sll  t1, t1, t2\n\t"
        "or   t0, t0, t1\n\t"
        "csrw satp, t0\n\t"
        "sfence.vma zero, zero"
        :
        : "r"(pt_addr)
        : "t0", "t1", "t2", "memory"
    );
}

pte_t *vmm_walk(page_table_t page_table, virt_addr_t va, BOOL alloc)
{
    uint32_t vpn2 = (va >> VA_VPN2_SHIFT) & VA_INDEX_MASK;
    uint32_t vpn1 = (va >> VA_VPN1_SHIFT) & VA_INDEX_MASK;
    uint32_t vpn0 = (va >> VA_VPN0_SHIFT) & VA_INDEX_MASK;

    pte_t *pte2 = &page_table[vpn2];
    page_table_t *l1_table;

    if (!(*pte2 & PTE_V)) {
        if (alloc) {
            l1_table = (page_table_t *)pmm_alloc_frame();
            if (l1_table == NULL) {
                return NULL;
            }
            memset(l1_table, 0, PAGE_SIZE);

            *pte2 = pa_to_pte_ppn((uint64_t)l1_table) | PTE_V;
        } else {
            return NULL;
        }
    } else {
        l1_table = (page_table_t *)pte_to_pa(*pte2); 
    }

    pte_t *pte1 = &(*l1_table)[vpn1];
    page_table_t *l0_table;

    if (!(*pte1 & PTE_V)) {
        if (alloc) {
            l0_table = (page_table_t *)pmm_alloc_frame();
            if (l0_table == NULL) {
                return NULL;
            }
            memset(l1_table, 0, PAGE_SIZE);

            *pte1 = pa_to_pte_ppn((uint64_t)l0_table) | PTE_V;
        } else {
            return NULL;
        }
    } else {
        l0_table = (page_table_t *)pte_to_pa(*pte1); 
    }

    pte_t *pte0 = &(*l0_table)[vpn0];

    return pte0;
}

phys_addr_t vmm_virt_to_phys(page_table_t page_table, virt_addr_t va)
{
    pte_t *pte;
    phys_addr_t addr;

    pte = vmm_walk(page_table, va, FALSE);
    addr = pte_to_pa(*pte);

    return addr | (va & 0xFFF);
}

bool vmm_map_page(page_table_t page_table, virt_addr_t va, phys_addr_t pa, uint64_t flags)
{
    pte_t *pte;
    
    pte = vmm_walk(page_table, va, TRUE);
    if (pte == NULL) {
        return FALSE;
    }

    if (va % PAGE_SIZE != 0 || pa % PAGE_SIZE != 0) {
        return FALSE;
    }

    *pte = pa_to_pte_ppn(pa) | PTE_V | flags;

    return TRUE;
}

phys_addr_t vmmk_virt_to_phys(virt_addr_t va)
{
    return vmm_virt_to_phys(kernel_page_table, va);
}

bool vmmk_map_page(virt_addr_t va, phys_addr_t pa, uint64_t flags)
{
    return vmm_map_page(kernel_page_table, va, pa, flags);
}

void vmm_init(boot_info_t *boot_info)
{
    memset(kernel_page_table, 0, sizeof(kernel_page_table));

    uint64_t kernel_start = (phys_addr_t)boot_info->kernel_image_start; 
    
    for (uint64_t addr = ALIGN(kernel_start, PAGE_SIZE); addr < ALIGN_UP((uint64_t)(kernel_start + boot_info->kernel_image_end), PAGE_SIZE); addr += PAGE_SIZE) {
        if (vmmk_map_page(addr, addr, PAGE_KERNEL_EXEC) == FALSE) {
            panic("vmm", "unable to map kernel page at 0x%x", addr);
        }
    }

    klog_info("vmm", "installing page table");

    vmm_install_page_table(kernel_page_table);

    klog_info("vmm", "paging initialized, page table at 0x%x", (uint64_t)kernel_page_table);
}