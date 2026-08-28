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
#include <boot/cpu.hpp>
#include <drivers/uart.hpp>
#include <libkern/klog.hpp>
#include <stdarg.h>

static const char *level_name(KLogLevel level)
{
    switch (level) {
        case KLogLevel::Debug:    return COLOR_CYAN "[debug]" COLOR_RESET;
        case KLogLevel::Info:     return COLOR_CYAN "[info]" COLOR_RESET;
        case KLogLevel::Notice:   return COLOR_BLUE "[notice]" COLOR_RESET;
        case KLogLevel::Warning:  return COLOR_YELLOW "[warning]" COLOR_RESET;
        case KLogLevel::Error:    return COLOR_RED "[error]" COLOR_RESET;
        case KLogLevel::Critical: return COLOR_RED "[critical]" COLOR_RESET;
        case KLogLevel::Panic:    return COLOR_RED "[panic]" COLOR_RESET;
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
    uart_puts(level_name(level));
    uart_puts(" ");
    uart_puts(COLOR_MAGENTA);
    uart_puts(subsystem);
    uart_puts(COLOR_RESET);
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
    uart_puts(level_name(level));               \
    uart_puts(" ");                             \
    uart_puts(COLOR_MAGENTA);                   \
    uart_puts(subsystem);                       \
    uart_puts(COLOR_RESET);                     \
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
DEFINE_KLOG_FUNCTION(klog_panic,    KLogLevel::Panic)

#undef DEFINE_KLOG_FUNCTION

void panic(const char *subsystem,
          const char *format, ...)
{
    uart_puts(level_name(KLogLevel::Panic));
    uart_puts(" ");
    uart_puts(COLOR_MAGENTA);
    uart_puts(subsystem);
    uart_puts(COLOR_RESET);
    uart_puts(": ");

    va_list args;
    va_start(args, format);
    klog_vprintf(format, args);
    va_end(args);

    uart_puts("\n");

    arm64_registers_t *regs = dump_registers();

    klog_panic("cpu", 
        "x0 =0x%x  x1 =0x%x "
        "x2 =0x%x  x3 =0x%x\n"
        "x4 =0x%x  x5 =0x%x "
        "x6 =0x%x  x7 =0x%x\n"
        "x8 =0x%x  x9 =0x%x "
        "x10=0x%x  x11=0x%x\n"
        "x12=0x%x  x13=0x%x "
        "x14=0x%x  x15=0x%x\n"
        "x16=0x%x  x17=0x%x "
        "x18=0x%x  x19=0x%x\n"
        "x20=0x%x  x21=0x%x "
        "x22=0x%x  x23=0x%x\n"
        "x24=0x%x  x25=0x%x "
        "x26=0x%x  x27=0x%x\n"
        "x28=0x%x  fp =0x%x "
        "lr =0x%x  sp =0x%x", 
        regs->x0,  regs->x1,  regs->x2,  regs->x3,
        regs->x4,  regs->x5,  regs->x6,  regs->x7,
        regs->x8,  regs->x9,  regs->x10, regs->x11,
        regs->x12, regs->x13, regs->x14, regs->x15,
        regs->x16, regs->x17, regs->x18, regs->x19,
        regs->x20, regs->x21, regs->x22, regs->x23,
        regs->x24, regs->x25, regs->x26, regs->x27,
        regs->x28, regs->x29, regs->lr,  regs->sp
    );

    halt();
}

void klog_init()
{
    klog_info("log", "logging initialized");
}