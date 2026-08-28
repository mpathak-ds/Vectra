/*
 * Vectra Kernel
 * Path: include/drivers/gic.hpp
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

#ifndef KERN_GIC_H
#define KERN_GIC_H

#include <osdef.hpp>

#define GIC_ENABLE_GRP0 0x01
#define GIC_ENABLE_GRP1 0x02

#define GIC_PRODUCT_GIC_400 0x002
#define GIC_IMPLEMENTER_ARM 0x43B

void gic_init(void);

#endif