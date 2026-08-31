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

#ifndef PHYS_TO_VIRT
#define PHYS_TO_VIRT(pa) ((void*)(uint64_t)(pa))
#endif

#ifndef VIRT_TO_PHYS
#define VIRT_TO_PHYS(va) ((phys_addr_t)(uint64_t)(va))
#endif

//config
#ifndef SATP_MODE_SV39
#define SATP_MODE_SV39 (8ULL << 60)
#endif

#ifndef MAKE_SATP_VAL
#define MAKE_SATP_VAL(pagetable_phys) (SATP_MODE_SV39 | ((uint64_t)(pagetable_phys) >> 12))
#endif

__attribute__((aligned(4096)))
static pte_t kernel_page_table[PT_ENTRIES];

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
    phys_addr_t pt_phys = VIRT_TO_PHYS(page_table);

    klog_info("vmm", "root page table physical address: 0x%x", (uint64_t)pt_phys);

    uint64_t satp_val = MAKE_SATP_VAL(pt_phys);

    asm volatile(
        "csrw satp, %0\n\t"
        "sfence.vma zero, zero"
        :
        : "r"(satp_val)
        : "memory"
    );
}

pte_t *vmm_walk(page_table_t page_table, virt_addr_t va, BOOL alloc)
{
    uint32_t vpn2 = (va >> VA_VPN2_SHIFT) & VA_INDEX_MASK;
    uint32_t vpn1 = (va >> VA_VPN1_SHIFT) & VA_INDEX_MASK;
    uint32_t vpn0 = (va >> VA_VPN0_SHIFT) & VA_INDEX_MASK;

    pte_t *pte2 = &page_table[vpn2];
    pte_t *l1_table;

    if (!(*pte2 & PTE_V)) {
        if (alloc) {
            phys_addr_t frame_pa = (phys_addr_t)pmm_alloc_frame();
            if (!frame_pa) {
                return NULL;
            }
            l1_table = (pte_t *)PHYS_TO_VIRT(frame_pa);
            memset(l1_table, 0, PAGE_SIZE);

            //pte v bit
            *pte2 = pa_to_pte_ppn(frame_pa) | PTE_V;
        } else {
            return NULL;
        }
    } else {
        l1_table = (pte_t *)PHYS_TO_VIRT(pte_to_pa(*pte2)); 
    }

    pte_t *pte1 = &l1_table[vpn1];
    pte_t *l0_table;

    if (!(*pte1 & PTE_V)) {
        if (alloc) {
            phys_addr_t frame_pa = (phys_addr_t)pmm_alloc_frame();
            if (!frame_pa) {
                return NULL;
            }
            l0_table = (pte_t *)PHYS_TO_VIRT(frame_pa);
            memset(l0_table, 0, PAGE_SIZE);

            //only pte v bit
            *pte1 = pa_to_pte_ppn(frame_pa) | PTE_V;
        } else {
            return NULL;
        }
    } else {
        l0_table = (pte_t *)PHYS_TO_VIRT(pte_to_pa(*pte1)); 
    }

    pte_t *pte0 = &l0_table[vpn0];

    return pte0;
}

phys_addr_t vmm_virt_to_phys(page_table_t page_table, virt_addr_t va)
{
    pte_t *pte = vmm_walk(page_table, va, FALSE);
    if (!pte || !(*pte & PTE_V)) {
        return 0;
    }

    phys_addr_t addr = pte_to_pa(*pte);
    klog_info("vmm", "0x%x, 0x%x", *pte, addr);

    return addr | (va & 0xFFF);
}

bool vmm_map_page(page_table_t page_table, virt_addr_t va, phys_addr_t pa, uint64_t flags)
{
    if (va % PAGE_SIZE != 0 || pa % PAGE_SIZE != 0) {
        return FALSE;
    }

    pte_t *pte = vmm_walk(page_table, va, TRUE);
    if (pte == NULL) {
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
    memset(kernel_page_table, 0, PAGE_SIZE);

    //kern mapping
    for (uint64_t addr = 0x80000000; addr < 0x8F000000; addr += PAGE_SIZE) {
        if (vmmk_map_page(addr, addr, PAGE_KERNEL_EXEC) == FALSE) {
            panic("vmm", "unable to map kernel page at 0x%x", addr);
        }
    }

    //map uart
    for (uint64_t mmio_addr = 0x10000000; mmio_addr < 0x10002000; mmio_addr += PAGE_SIZE) {
        vmmk_map_page(mmio_addr, mmio_addr, PTE_R | PTE_W);
    }

    klog_info("vmm", "installing page table");

    klog_info("vmm", "virt to phys returned 0x%x", vmm_virt_to_phys(kernel_page_table, 0x80000000));
    
    //activate
    vmm_install_page_table(kernel_page_table);

    klog_info("vmm", "paging initialized, page table at 0x%x", (uint64_t)kernel_page_table);
}