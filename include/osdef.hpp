/*
 * Vectra Kernel
 * Path: include/osdef.hpp
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

#ifndef KERN_BASE_H
#define KERN_BASE_H

#define NULL 0
#define ALIGN_UP(num, align) (((num) + ((align) - 1)) & ~((align) - 1))

#define MACHINE_QEMU_VIRT
//#define MACHINE_OPI

#define MACHINE_RAM_SIZE 0x8000000 //128mb

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long uint64_t;

typedef char int8_t;
typedef short int16_t;
typedef int int32_t;
typedef long int64_t;

typedef uint64_t size_t;
typedef uint64_t uintptr_t;

/* Note, no need to define bool, we are using C++ and it already has it :3 */

typedef uint32_t OSSTATUS;

typedef struct boot_info 
{
    uint8_t *early_heap_start;
    uint8_t *early_heap_end;
    uint8_t *kernel_image_start;

    uint64_t machine_ram_base;
    uint64_t machine_ram_total;
} boot_info_t;

void panic(const char *reason);

#endif