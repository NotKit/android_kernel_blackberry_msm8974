/*
 *
 * Copyright (C) 2007 Google, Inc.
 * Author: Brian Swetland <swetland@google.com>
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#define CONFIG_MSM_HAS_DEBUG_UART_HS
#define CONFIG_MSM_HAS_DEBUG_UART_HS_V14

#include <mach/hardware.h>
#include <mach/msm_iomap.h>
#include <mach/msm_serial_hsl_regs.h>

#ifdef CONFIG_DEBUG_UART_PHYS
    .macro  addruart, rp, rv, tmp
	ldr     \rp, =CONFIG_DEBUG_UART_PHYS
	ldr     \rv, =CONFIG_DEBUG_UART_VIRT
	.endm

	.macro	senduart,rd,rx
	ldr	r2, [\rx]
	cmp	r2, #0
	moveq	r2, #16
	strb	\rd, [\rx, r2]
	add	r2, r2, #1
	str	r2, [\rx]
	.endm

	.macro	waituart,rd,rx
	.endm

#else

	.macro  addruart, rp, rv
	.endm

	.macro	senduart,rd,rx
	.endm

	.macro	waituart,rd,rx
	.endm
#endif

	.macro	busyuart,rd,rx
	.endm
