/*
 * Vectra Kernel
 * Path: include/klog.hpp
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

#ifndef LIBK_LOG_H
#define LIBK_LOG_H

#include <osdef.hpp>

enum class KLogLevel {
    Debug,
    Info,
    Notice,
    Warning,
    Error,
    Critical
};

void klog_init();

void klog(KLogLevel level, const char *subsystem, const char *format, ...);

void klog_info(const char *subsystem, const char *format, ...);
void klog_notice(const char *subsystem, const char *format, ...);
void klog_warn(const char *subsystem, const char *format, ...);
void klog_error(const char *subsystem, const char *format, ...);
void klog_critical(const char *subsystem, const char *format, ...);
void klog_debug(const char *subsystem, const char *format, ...);

#endif