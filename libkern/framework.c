/*
 * Vectra Kernel
 * Path: libkern/framework.cpp
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

#include <libkern/framework.hpp>

uint32_t io_read32(uint64_t address, uint64_t offset)
{
    return *((volatile uint32_t*)(address + offset));
}

void io_write32(uint64_t address, uint64_t offset, uint32_t value)
{
    *((volatile uint32_t*)(address + offset)) = value;
}

uint64_t io_read64(uint64_t address, uint64_t offset)
{
    return *((volatile uint64_t*)(address + offset));
}

void io_write64(uint64_t address, uint64_t offset, uint64_t value)
{
    *((volatile uint64_t*)(address + offset)) = value;
}