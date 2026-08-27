/*
 * Vectra Kernel
 * Path: include/drivers/uart.hpp
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

#define MACHINE_QEMU_VIRT
//#define MACHINE_OPI

void uart_putc(char c);
void uart_puts(const char *s);

#endif