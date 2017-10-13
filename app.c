/*
 * Derived from code:
 *
 * Copyright (C) 2012 Vikram Narayananan <vikram186@gmail.com>
 * (C) Copyright 2012-2016 Stephen Warren
 * Copyright (C) 1996-2000 Russell King
 *
 * SPDX-License-Identifier: GPL-2.0
 */

#include <stdint.h>
#include <limits.h>
#include "unlzma.h"

#ifndef NULL
#define NULL ((void*)0)
#endif

#define BIT(x) (1 << (x))

#define BCM2835_MU_BASE         0x3f215040

struct bcm283x_mu_regs {
    uint32_t io;
    uint32_t iir;
    uint32_t ier;
    uint32_t lcr;
    uint32_t mcr;
    uint32_t lsr;
    uint32_t msr;
    uint32_t scratch;
    uint32_t cntl;
    uint32_t stat;
    uint32_t baud;
};

/* This actually means not full, but is named not empty in the docs */
#define BCM283X_MU_LSR_TX_EMPTY     BIT(5)
#define BCM283X_MU_LSR_RX_READY     BIT(0)

#define __arch_getl(a)          (*(volatile unsigned int *)(a))
#define __arch_putl(v,a)        (*(volatile unsigned int *)(a) = (v))

#define dmb()       __asm__ __volatile__ ("" : : : "memory")
#define __iormb()   dmb()
#define __iowmb()   dmb()

#define readl(c)    ({ uint32_t __v = __arch_getl(c); __iormb(); __v; })
#define writel(v,c) ({ uint32_t __v = v; __iowmb(); __arch_putl(__v,c); __v; })

extern uint64_t _app_start;

static void bcm283x_mu_serial_putc(const char data)
{
    struct bcm283x_mu_regs *regs = (struct bcm283x_mu_regs *)BCM2835_MU_BASE;

    /* Wait until there is space in the FIFO */
    while (!(readl(&regs->lsr) & BCM283X_MU_LSR_TX_EMPTY))
        ;

    /* Send the character */
    writel(data, &regs->io);
}

void dbg_puts(char *s)
{
    while (*s)
    {
        if (*s == '\n')
            bcm283x_mu_serial_putc('\r');
        bcm283x_mu_serial_putc(*s++);
    }
}

void dbg_puthex4(int val)
{
    int c;

    if (val < 10)
        c = val + '0';
    else
        c = val - 10 + 'A';

    bcm283x_mu_serial_putc(c);
}

void dbg_puthex32(uint32_t val)
{
    for (int i = 28; i >= 0; i -= 4)
        dbg_puthex4((val >> i) & 0xf);
}

void dbg_puthex64(uint64_t val)
{
    dbg_puthex32(val >> 32);
    dbg_puthex32(val & 0xffffffffU);
}

uint64_t read_mpidr(void)
{
    uint64_t v;
    __asm__ __volatile__("mrs %0, mpidr_el1" : "=r" (v) : : );
    return v;
}

uint64_t read_currentel(void)
{
    uint64_t v;
    __asm__ __volatile__("mrs %0, currentel" : "=r" (v) : : );
    return v;
}

uint64_t read_spsel(void)
{
    uint64_t v;
    __asm__ __volatile__("mrs %0, spsel" : "=r" (v) : : );
    return v;
}

void unlzma_and_print()
{
    uint32_t kern_addr_32 = *(volatile uint32_t *)0xfc;
    unsigned char *kern_addr = (unsigned char *)(0L | kern_addr_32);
    unsigned char *outbuf = (unsigned char*)0x00280000;
    long sz;

    dbg_puts("decompress from ");
    dbg_puthex64((uint64_t)kern_addr);
    dbg_puts(" to ");
    dbg_puthex64((uint64_t)outbuf);
    dbg_puts("\n");

    unlzma(kern_addr, LONG_MAX, NULL, NULL, outbuf, &sz, dbg_puts);

    dbg_puts("Decompressed ");
    dbg_puthex64(sz);
    dbg_puts(" bytes:\n");

    outbuf[sz] = '\0';
    dbg_puts((char*)outbuf);
}

void app(uint64_t r0, uint64_t r1, uint64_t r2, uint64_t r3)
{
    static int this_cpuid = -1;
    uint64_t v;

    this_cpuid++;

    /* LF after line noise */
    if (!this_cpuid)
        dbg_puts("\n");

    dbg_puts("Hello, world!\n");

    dbg_puts("this_cpuid:");
    dbg_puthex4(this_cpuid);
    dbg_puts("\n");

    dbg_puts("MPIDR:");
    v = read_mpidr();
    dbg_puthex64(v);
    dbg_puts("\n");

    dbg_puts("r0:");
    dbg_puthex64(r0);
    dbg_puts("\n");

    dbg_puts("r1:");
    dbg_puthex64(r1);
    dbg_puts("\n");

    dbg_puts("r2:");
    dbg_puthex64(r2);
    dbg_puts("\n");

    dbg_puts("r3:");
    dbg_puthex64(r3);
    dbg_puts("\n");

    dbg_puts("CurrentEL:");
    v = read_currentel();
    dbg_puthex4(v);
    dbg_puts("\n");

    dbg_puts("SPSel:");
    v = read_spsel();
    dbg_puthex4(v);
    dbg_puts("\n");

    dbg_puts("dtb_ptr32:");
    dbg_puthex32(*(volatile uint32_t *)0xf8);
    dbg_puts("\n");

    dbg_puts("kernel_entry32:");
    dbg_puthex32(*(volatile uint32_t *)0xfc);
    dbg_puts("\n");

    dbg_puts("_app_start:");
    dbg_puthex64((uint64_t)&_app_start);
    dbg_puts("\n");

    dbg_puts("Decompressing 'kernel' data...\n");
    unlzma_and_print();

#if 0
    if (this_cpuid < 3) {
        void **spin_table = (void*)0xd8;
        spin_table[this_cpuid + 1] = &_app_start;
        __asm__ __volatile__("sev");
    }
#endif

    while (1) {
    }
}
