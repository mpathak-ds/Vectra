/*
 * Vectra Kernel
 * Path: platform/virt_rv/uart.cpp
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

#include <drivers/uart.hpp>

#define UART_BASE 0x10000000UL

extern "C" void uart_putc(char c)
{
    volatile unsigned int* dr = (volatile unsigned int*)UART_BASE;
    *dr = c;
}

void uart_puts(const char *str)
{
    while (*str) {
        if (*str == '\n')
            uart_putc('\r');

        uart_putc(*str++);
    }
}