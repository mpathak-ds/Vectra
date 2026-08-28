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
#include <libkern/framework.hpp>
#include <libkern/klog.hpp>

#define MACHINE_QEMU_VIRT

#define GICD_BASE 0x08000000
#define GICC_BASE 0x08010000

#define GICD_CTLR            0x000
#define GICD_IIDR            0x008
#define GICD_ISENABLER(n)    (0x100 + (n) * 4)
#define GICD_IPRIORITYR(n)   (0x400 + (n) * 4)

#define GICC_CTLR            0x000
#define GICC_PMR             0x004
#define GICC_IAR             0x00C
#define GICC_EOIR            0x010

void gic_init(void)
{
    uint64_t gicd_ctlr;
    uint64_t gicd_iidr;

    gicd_ctlr = io_read32(GICD_BASE, GICD_CTLR);
    gicd_ctlr |= GIC_ENABLE_GRP0;
    io_write32(GICD_BASE, GICD_CTLR, gicd_ctlr);

    gicd_iidr = io_read32(GICD_BASE, GICD_IIDR);

    klog_info("gic", "GIC model is %s, implementer %s", ((gicd_iidr >> 24) & 0x03) == GIC_PRODUCT_GIC_400 ? "GIC-400" : "Unknown", 
        (gicd_iidr & 0xfff) == GIC_IMPLEMENTER_ARM ? "ARM" : "Unknown");
}