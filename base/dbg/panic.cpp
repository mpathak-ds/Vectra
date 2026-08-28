/*
 * Vectra Kernel
 * Path: base/dbg/panic.cpp
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

#include <osdef.hpp>
#include <libkern/klog.hpp>

void panic(const char *reason)
{
    klog_critical("panic", "!!! critical system failed : %s !!!", reason);
    klog_critical("panic", "hanging");

    while(1);
}