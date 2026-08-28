/*
 * Vectra Kernel
 * Path: platform/virt/gic.cpp
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

#include <drivers/gic.hpp>
#include <boot/cpu.hpp>
#include <libkern/framework.hpp>
#include <libkern/klog.hpp>

#define MACHINE_QEMU_VIRT

#define GICD_BASE 0x08000000
#define GICC_BASE 0x08010000

#define GICD_CTLR            0x000
#define GICD_TYPER           0x004
#define GICD_IIDR            0x008
#define GICD_ISENABLER(n)    (0x100 + (n) * 4)
#define GICD_IPRIORITYR(n)   (0x400 + (n) * 4)

#define GICC_CTLR            0x000
#define GICC_PMR             0x004
#define GICC_IAR             0x00C
#define GICC_EOIR            0x010

static interrupt_handler_t gic_handlers[GIC_MAX_IRQS];

static uint32_t gicc_iar = 0;

//PRIVATE FUNCTIONS

static inline uint32_t gic_interrupt_ack()
{
    gicc_iar = io_read32(GICC_BASE, GICC_IAR);
    //extract irq id
    return gicc_iar & 0x3FF;
}

static inline void gic_eoi()
{
    io_write32(GICC_BASE, GICC_EOIR, gicc_iar);
}

//PUBLIC FUNCTIONS

//must be used only in exception handler
void gic_handle_interrupts(arm64_registers_t *regs)
{
    uint32_t irq_id = gic_interrupt_ack();
    int32_t ret;

    if (gic_handlers[irq_id]) {
        ret = gic_handlers[irq_id](regs);
    } else {
        ret = -1;
    }

    if (ret == -1) {
        panic("gic", "interrupt handler exited with error");
    }
}

void gic_enable_interrupt(uint8_t irq)
{
    io_write32(GICD_BASE, GICD_ISENABLER(irq / 32), (1 << (irq % 32)));
}

void gic_set_interrupt_priority(uint8_t irq, uint8_t priority)
{
    uint32_t prio;

    prio = io_read32(GICD_BASE, GICD_IPRIORITYR(irq / 4));
    prio &= ~(0xFF << ((irq % 4) * 8));
    prio |= (priority << ((irq % 4) * 8));
    io_write32(GICD_BASE, GICD_IPRIORITYR(irq / 4), prio);
}

void gic_allow_interrupts()
{
    io_write32(GICC_BASE, GICC_PMR, GIC_PRIORITY_ALL);
}

void gic_init()
{
    uint32_t gicd_ctlr;
    uint32_t gicc_ctlr;
    uint32_t gicd_iidr;
    uint32_t gicd_typer;

    gicd_ctlr = io_read32(GICD_BASE, GICD_CTLR);
    gicd_ctlr |= GIC_ENABLE_GRP0;
    io_write32(GICD_BASE, GICD_CTLR, gicd_ctlr);
    gicc_ctlr = io_read32(GICC_BASE, GICC_CTLR);
    gicc_ctlr |= GIC_ENABLE_GRP0;
    io_write32(GICC_BASE, GICC_CTLR, gicc_ctlr);

    gicd_iidr = io_read32(GICD_BASE, GICD_IIDR);
    gicd_typer = io_read32(GICD_BASE, GICD_TYPER);

    klog_info("gic", "GIC model is %s, implementer %s, %d max irqs", ((gicd_iidr >> 24) & 0x03) == GIC_PRODUCT_GIC_400 ? "GIC-400" : "Unknown", 
        (gicd_iidr & 0xfff) == GIC_IMPLEMENTER_ARM ? "ARM" : "Unknown", (32 * ((gicd_typer & 0x1F) + 1)));

    gic_allow_interrupts();
}