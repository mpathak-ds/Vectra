/*
 * Vectra Kernel
 * Path: libkern/klog.cpp
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
#include <drivers/uart.hpp>
#include <libkern/klog.hpp>
#include <stdarg.h>

static const char *level_name(KLogLevel level)
{
    switch (level) {
        case KLogLevel::Debug:    return "debug   ";
        case KLogLevel::Info:     return "info    ";
        case KLogLevel::Notice:   return "notice  ";
        case KLogLevel::Warning:  return "warning ";
        case KLogLevel::Error:    return "error   ";
        case KLogLevel::Critical: return "critical";
    }

    return "unknown ";
}

static void put_unsigned(uint64_t value, unsigned base)
{
    char buf[32];
    unsigned i = 0;

    if (value == 0) {
        uart_putc('0');
        return;
    }

    while (value) {
        unsigned digit = value % base;

        if (digit < 10)
            buf[i++] = '0' + digit;
        else
            buf[i++] = 'a' + (digit - 10);

        value /= base;
    }

    while (i)
        uart_putc(buf[--i]);
}

static void put_signed(int value)
{
    if (value < 0) {
        uart_putc('-');

        // Avoid overflow for INT64_MIN.
        uint64_t magnitude = (uint64_t)(-(value + 1)) + 1;
        put_unsigned(magnitude, 10);
    } else {
        put_unsigned((uint64_t)value, 10);
    }
}

static void klog_vprintf(const char *format, va_list args)
{
    while (*format) {

        if (*format != '%') {
            uart_putc(*format++);
            continue;
        }

        format++;

        switch (*format) {

            case '%':
                uart_putc('%');
                break;

            case 'c':
                uart_putc((char)va_arg(args, int));
                break;

            case 's': {
                const char *str = va_arg(args, const char *);

                if (!str)
                    str = "(null)";

                uart_puts(str);
                break;
            }

            case 'd':
                put_signed(va_arg(args, int));
                break;

            case 'u':
                put_unsigned(va_arg(args, unsigned int), 10);
                break;

            case 'x':
                put_unsigned(va_arg(args, unsigned int), 16);
                break;

            case 'X': {
                uint64_t value = va_arg(args, unsigned int);

                char buf[16];
                unsigned i = 0;

                if (value == 0) {
                    uart_putc('0');
                    break;
                }

                while (value) {
                    unsigned digit = value % 16;

                    if (digit < 10)
                        buf[i++] = '0' + digit;
                    else
                        buf[i++] = 'A' + (digit - 10);

                    value /= 16;
                }

                while (i)
                    uart_putc(buf[--i]);

                break;
            }

            case 'p':
                uart_puts("0x");
                put_unsigned(
                    (uint64_t)va_arg(args, void *),
                    16
                );
                break;

            default:
                // Preserve unknown format specifiers.
                uart_putc('%');
                uart_putc(*format);
                break;
        }

        format++;
    }
}

void klog(KLogLevel level,
          const char *subsystem,
          const char *format, ...)
{
    uart_puts("[    0.000] ");
    uart_puts(level_name(level));
    uart_puts(" ");
    uart_puts(subsystem);
    uart_puts(": ");

    va_list args;
    va_start(args, format);
    klog_vprintf(format, args);
    va_end(args);

    uart_puts("\n");
}

#define DEFINE_KLOG_FUNCTION(name, level)       \
void name(const char *subsystem, const char *format, ...) \
{                                               \
    va_list args;                               \
    va_start(args, format);                     \
                                                \
    uart_puts("[    0.000] ");                  \
    uart_puts(level_name(level));               \
    uart_puts(" ");                             \
    uart_puts(subsystem);                       \
    uart_puts(": ");                            \
    klog_vprintf(format, args);                 \
    uart_puts("\n");                            \
                                                \
    va_end(args);                               \
}

DEFINE_KLOG_FUNCTION(klog_info,     KLogLevel::Info)
DEFINE_KLOG_FUNCTION(klog_notice,   KLogLevel::Notice)
DEFINE_KLOG_FUNCTION(klog_warn,     KLogLevel::Warning)
DEFINE_KLOG_FUNCTION(klog_error,    KLogLevel::Error)
DEFINE_KLOG_FUNCTION(klog_critical, KLogLevel::Critical)
DEFINE_KLOG_FUNCTION(klog_debug,    KLogLevel::Debug)

#undef DEFINE_KLOG_FUNCTION

void klog_init()
{
    klog_info("log", "logging initialized");
}