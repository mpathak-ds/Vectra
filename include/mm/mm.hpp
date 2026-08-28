/*
 * Vectra Kernel
 * Path: include/mm/mm.hpp
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

#ifndef KERN_MM_H
#define KERN_MM_H

#include <osdef.hpp>

extern "C" void mm_boot_init(boot_info_t *boot_info);
void *mm_boot_alloc(uint64_t size);

#endif