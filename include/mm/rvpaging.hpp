/*
 * Vectra Kernel
 * Path: include/mm/rvpaging.hpp
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

#ifndef KERN_RVPAGING_H
#define KERN_RVPAGING_H

#include <osdef.hpp>

#define PTE_V             (1ULL << 0)   // Valid
#define PTE_R             (1ULL << 1)   // Read
#define PTE_W             (1ULL << 2)   // Write
#define PTE_X             (1ULL << 3)   // Execute
#define PTE_U             (1ULL << 4)   // User
#define PTE_G             (1ULL << 5)   // Global
#define PTE_A             (1ULL << 6)   // Accessed
#define PTE_D             (1ULL << 7)   // Dirty

#define PAGE_KERNEL_EXEC  (PTE_V | PTE_R | PTE_X | PTE_A | PTE_D)
#define PAGE_KERNEL_DATA  (PTE_V | PTE_R | PTE_W | PTE_A | PTE_D)
#define PAGE_TABLE_NODE   (PTE_V)

#define MAKE_SATP(pagetable) ((SATP_MODE_SV39 << 60) | (((uint64_t)(pagetable)) >> 12))

#define PAGESIZE          4096
#define PT_ENTRIES        512

typedef uint64_t pte_t;
typedef pte_t page_table_t[PT_ENTRIES];

#define VA_VPN2_SHIFT     30
#define VA_VPN1_SHIFT     21
#define VA_VPN0_SHIFT     12
#define VA_INDEX_MASK     0x1FF

#endif