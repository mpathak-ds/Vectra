/*
 * Vectra Kernel
 * Path: include/drivers/timer.hpp
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

#ifndef KERN_TIMER_H
#define KERN_TIMER_H

#include <osdef.hpp>

#define TO_MHZ(hz) ((hz) / 1000000)
#define TO_KHZ(hz) ((hz) / 1000)

uint64_t timer_get_core_frequency(void);
void timer_set_frequency(uint64_t freq);

void timer_enable(void);

void timer_init(void);

#endif