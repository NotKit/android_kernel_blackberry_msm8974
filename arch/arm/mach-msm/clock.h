<<<<<<< HEAD
/*
 * Copyright (c) 2012-2013, The Linux Foundation. All rights reserved.
=======
/* arch/arm/mach-msm/clock.h
 *
 * Copyright (C) 2007 Google, Inc.
 * Copyright (c) 2007-2012, The Linux Foundation. All rights reserved.
>>>>>>> android-3.18
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

#ifndef __ARCH_ARM_MACH_MSM_CLOCK_H
#define __ARCH_ARM_MACH_MSM_CLOCK_H

<<<<<<< HEAD
#include <linux/clkdev.h>

/**
 * struct clock_init_data - SoC specific clock initialization data
 * @table: table of lookups to add
 * @size: size of @table
 * @pre_init: called before initializing the clock driver.
 * @post_init: called after registering @table. clock APIs can be called inside.
 * @late_init: called during late init
 */
struct clock_init_data {
	struct list_head list;
	struct clk_lookup *table;
	size_t size;
	void (*pre_init)(void);
	void (*post_init)(void);
	int (*late_init)(void);
};

extern struct clock_init_data msm9615_clock_init_data;
extern struct clock_init_data msm9625_clock_init_data;
extern struct clock_init_data apq8064_clock_init_data;
extern struct clock_init_data fsm9xxx_clock_init_data;
extern struct clock_init_data msm7x01a_clock_init_data;
extern struct clock_init_data msm7x27_clock_init_data;
extern struct clock_init_data msm7x27a_clock_init_data;
extern struct clock_init_data msm7x30_clock_init_data;
extern struct clock_init_data msm8960_clock_init_data;
extern struct clock_init_data msm8x60_clock_init_data;
extern struct clock_init_data qds8x50_clock_init_data;
extern struct clock_init_data msm8625_dummy_clock_init_data;
extern struct clock_init_data msm8930_clock_init_data;
extern struct clock_init_data msm8930_pm8917_clock_init_data;
extern struct clock_init_data msm8974_clock_init_data;
extern struct clock_init_data msm8974_rumi_clock_init_data;
extern struct clock_init_data msm8610_clock_init_data;
extern struct clock_init_data msm8610_rumi_clock_init_data;
extern struct clock_init_data msm8226_clock_init_data;
extern struct clock_init_data msm8226_rumi_clock_init_data;
extern struct clock_init_data msm8084_clock_init_data;
extern struct clock_init_data mpq8092_clock_init_data;
extern struct clock_init_data msmkrypton_clock_init_data;

int msm_clock_init(struct clock_init_data *data);
int find_vdd_level(struct clk *clk, unsigned long rate);

#ifdef CONFIG_DEBUG_FS
int clock_debug_register(struct clk_lookup *t, size_t s);
void clock_debug_print_enabled(void);
#else
static inline int clock_debug_register(struct clk_lookup *t, size_t s)
{
	return 0;
}
static inline void clock_debug_print_enabled(void) { return; }
#endif
=======
#include <linux/clk-provider.h>
#include <mach/clk.h>

#define CLK_FIRST_AVAILABLE_FLAG	0x00000100
#define CLKFLAG_AUTO_OFF		0x00000200
#define CLKFLAG_MIN			0x00000400
#define CLKFLAG_MAX			0x00000800

#define OFF CLKFLAG_AUTO_OFF
#define CLK_MIN CLKFLAG_MIN
#define CLK_MAX CLKFLAG_MAX
#define CLK_MINMAX (CLK_MIN | CLK_MAX)

struct msm_clk {
	int (*reset)(struct clk_hw *hw, enum clk_reset_action action);
	struct clk_hw hw;
};

static inline struct msm_clk *to_msm_clk(struct clk_hw *hw)
{
	return container_of(hw, struct msm_clk, hw);
}
>>>>>>> android-3.18

#endif
