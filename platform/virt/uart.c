/*
 * Vectra Kernel
 * Path: platform/virt/uart.c
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

#define UART_BASE 0x09000000UL

#define UART_DR (*(volatile unsigned int *)(UART_BASE + 0x00))
#define UART_FR (*(volatile unsigned int *)(UART_BASE + 0x18))

#define UART_FR_TXFF (1 << 5)

void uart_putc(char c)
{
    volatile unsigned int* dr = (volatile unsigned int*)0x09000000;
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