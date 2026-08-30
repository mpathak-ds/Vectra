/*
 * Vectra Kernel
 * Path: include/libkern/framework.hpp
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

#ifndef KERN_STRING_H
#define KERN_STRING_H

#include <osdef.hpp>

#ifdef __cplusplus
extern "C" {
#endif

void *memset(void *dst, int value, size_t size);
void *memcpy(void *dst, const void *src, size_t size);
#ifdef __cplusplus
}
#endif

void *memmove(void *dest, const void *src, size_t n);
int memcmp(const void *s1, const void *s2, size_t n);
size_t strlen(const char *str);

#endif