/*
 * Derived from code:
 * (C) Copyright 2013 David Feng <fenghua@phytium.com.cn>
 * (C) Copyright 2002 Gary Jennejohn, DENX Software Engineering, <garyj@denx.de>
 *
 * SPDX-License-Identifier: GPL-2.0
 */

OUTPUT_FORMAT(elf64-littleaarch64)
OUTPUT_ARCH(aarch64)
ENTRY(_start)
SECTIONS
{
    /* start.S - the low-level CPU init, which eventually jumps to C code in .text
     * the firmware puts ATAGs or some other data in 0x100-0x4000, so the
     * init section must no exceed 256 bytes */
    . = 0x0;
    .init : {
        *(.init)
        . = 0x100;
    }

    . = 0x100;
    .atags : { FILL(0); }

    /* C code goes here */
    . = 0x4000;
    .text : { *(.text*) }

    . = ALIGN(8);
    .rodata : { *(SORT_BY_ALIGNMENT(SORT_BY_NAME(.rodata*))) }

    . = ALIGN(8);
    .data : { *(.data*) }

    . = ALIGN(8);
    .rel_dyn_start : { *(.__rel_dyn_start) }
    .rela.dyn : { *(.rela*) }
    .rel_dyn_end : { *(.__rel_dyn_end) }

    _end = .;

    . = ALIGN(8);
    .bss_start : { KEEP(*(.__bss_start)); }
    .bss : {
        *(.bss*)
         . = ALIGN(8);
    }
    .bss_end : { KEEP(*(.__bss_end)); }

    /DISCARD/ : { *(.dynsym) }
    /DISCARD/ : { *(.dynstr*) }
    /DISCARD/ : { *(.dynamic*) }
    /DISCARD/ : { *(.plt*) }
    /DISCARD/ : { *(.interp*) }
    /DISCARD/ : { *(.gnu*) }
}
