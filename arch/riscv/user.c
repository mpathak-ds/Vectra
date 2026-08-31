/*
 * Vectra Kernel
 * Path: arch/riscv/user.c
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

#include <osdef.hpp>
#include <boot/cpu.hpp>
#include <mm/rvpaging.hpp>
#include <mm/mm.hpp>
#include <libkern/klog.hpp>
#include <libkern/string.hpp>

extern uint8_t __trampoline_start[];
extern void enter_usermode(uint64_t user_pc, uint64_t user_sp, uint64_t user_page_table);

const unsigned char usercode[] = {
    0x37, 0x05, 0x00, 0x10, 0x93, 0x05, 0x10, 0x06, 
    0x23, 0x00, 0xb5, 0x00, 0x23, 0x80, 0xb5, 0x00
};

void create_user_process()
{
    __attribute__((aligned(4096)))
    pte_t user_page_table[PT_ENTRIES];

    void *code = (void *)pmm_alloc_frame();
    memcpy(code, usercode, sizeof(usercode));
    vmm_map_page(user_page_table, (virt_addr_t)code, (phys_addr_t)code, PAGE_KERNEL_EXEC | PTE_U);
    vmm_map_page(user_page_table, (virt_addr_t)__trampoline_start, (phys_addr_t)__trampoline_start, PAGE_KERNEL_EXEC);
    vmm_map_page(user_page_table, 0x10000000UL, 0x10000000UL, PTE_R | PTE_W | PTE_U);

    klog_info("user", "switching to user mode, trampoline address=0x%llx", (virt_addr_t)__trampoline_start);
    enter_usermode((uint64_t)code, (uint64_t)(code + 1024), (uint64_t)user_page_table);
}