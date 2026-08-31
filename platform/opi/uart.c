/*
 * Vectra Kernel
 * Path: platform/opi/uart.c
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

#include <drivers/uart.hpp>

#define UART2_BASE 0xFE660000
#define UART_THR   (*(volatile unsigned int *)(UART2_BASE + 0x00))
#define UART_LSR   (*(volatile unsigned int *)(UART2_BASE + 0x14))

void uart_putc(char c) 
{
    while (!(UART_LSR & (1 << 5)));
    UART_THR = c;
}

void uart_puts(const char *str)
{
    while (*str) {
        if (*str == '\n') uart_putc('\r');
        uart_putc(*str++);
    }
}