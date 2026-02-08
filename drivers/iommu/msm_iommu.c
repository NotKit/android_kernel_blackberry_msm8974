/* Copyright (c) 2013, The Linux Foundation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#define pr_fmt(fmt)	KBUILD_MODNAME ": " fmt
#include <linux/errno.h>
#include <linux/err.h>
#include <linux/list.h>
#include <linux/mutex.h>
#include <linux/platform_device.h>
#include <linux/export.h>
#include <linux/iommu.h>
<<<<<<< HEAD
#include <mach/iommu.h>
=======
#include <linux/clk.h>

#include <asm/cacheflush.h>
#include <asm/sizes.h>

#include "msm_iommu_hw-8xxx.h"
#include "msm_iommu.h"
>>>>>>> android-3.18

static DEFINE_MUTEX(iommu_list_lock);
static LIST_HEAD(iommu_list);

static struct iommu_access_ops *iommu_access_ops;

struct bus_type msm_iommu_sec_bus_type = {
	.name = "msm_iommu_sec_bus",
};

void msm_set_iommu_access_ops(struct iommu_access_ops *ops)
{
	iommu_access_ops = ops;
}

struct iommu_access_ops *msm_get_iommu_access_ops()
{
	BUG_ON(iommu_access_ops == NULL);
	return iommu_access_ops;
}
EXPORT_SYMBOL(msm_get_iommu_access_ops);

void msm_iommu_add_drv(struct msm_iommu_drvdata *drv)
{
	mutex_lock(&iommu_list_lock);
	list_add(&drv->list, &iommu_list);
	mutex_unlock(&iommu_list_lock);
}

void msm_iommu_remove_drv(struct msm_iommu_drvdata *drv)
{
	mutex_lock(&iommu_list_lock);
	list_del(&drv->list);
	mutex_unlock(&iommu_list_lock);
}

static int find_iommu_ctx(struct device *dev, void *data)
{
	struct msm_iommu_ctx_drvdata *c;

	c = dev_get_drvdata(dev);
	if (!c || !c->name)
		return 0;

<<<<<<< HEAD
	return !strcmp(data, c->name);
=======
	INIT_LIST_HEAD(&priv->list_attached);
	priv->pgtable = (unsigned long *)__get_free_pages(GFP_KERNEL,
							  get_order(SZ_16K));

	if (!priv->pgtable)
		goto fail_nomem;

	memset(priv->pgtable, 0, SZ_16K);
	domain->priv = priv;

	domain->geometry.aperture_start = 0;
	domain->geometry.aperture_end   = (1ULL << 32) - 1;
	domain->geometry.force_aperture = true;

	return 0;

fail_nomem:
	kfree(priv);
	return -ENOMEM;
>>>>>>> android-3.18
}

static struct device *find_context(struct device *dev, const char *name)
{
	return device_find_child(dev, (void *)name, find_iommu_ctx);
}

struct device *msm_iommu_get_ctx(const char *ctx_name)
{
	struct msm_iommu_drvdata *drv;
	struct device *dev = NULL;

	mutex_lock(&iommu_list_lock);
	list_for_each_entry(drv, &iommu_list, list) {
		dev = find_context(drv->dev, ctx_name);
		if (dev)
			break;
	}
	mutex_unlock(&iommu_list_lock);

	put_device(dev);

	if (!dev || !dev_get_drvdata(dev)) {
		pr_debug("Could not find context <%s>\n", ctx_name);
		dev = ERR_PTR(-EPROBE_DEFER);
	}

	return dev;
}
EXPORT_SYMBOL(msm_iommu_get_ctx);

<<<<<<< HEAD
=======
static int msm_iommu_map(struct iommu_domain *domain, unsigned long va,
			 phys_addr_t pa, size_t len, int prot)
{
	struct msm_priv *priv;
	unsigned long flags;
	unsigned long *fl_table;
	unsigned long *fl_pte;
	unsigned long fl_offset;
	unsigned long *sl_table;
	unsigned long *sl_pte;
	unsigned long sl_offset;
	unsigned int pgprot;
	int ret = 0, tex, sh;

	spin_lock_irqsave(&msm_iommu_lock, flags);

	sh = (prot & MSM_IOMMU_ATTR_SH) ? 1 : 0;
	tex = msm_iommu_tex_class[prot & MSM_IOMMU_CP_MASK];

	if (tex < 0 || tex > NUM_TEX_CLASS - 1) {
		ret = -EINVAL;
		goto fail;
	}

	priv = domain->priv;
	if (!priv) {
		ret = -EINVAL;
		goto fail;
	}

	fl_table = priv->pgtable;

	if (len != SZ_16M && len != SZ_1M &&
	    len != SZ_64K && len != SZ_4K) {
		pr_debug("Bad size: %d\n", len);
		ret = -EINVAL;
		goto fail;
	}

	if (!fl_table) {
		pr_debug("Null page table\n");
		ret = -EINVAL;
		goto fail;
	}

	if (len == SZ_16M || len == SZ_1M) {
		pgprot = sh ? FL_SHARED : 0;
		pgprot |= tex & 0x01 ? FL_BUFFERABLE : 0;
		pgprot |= tex & 0x02 ? FL_CACHEABLE : 0;
		pgprot |= tex & 0x04 ? FL_TEX0 : 0;
	} else	{
		pgprot = sh ? SL_SHARED : 0;
		pgprot |= tex & 0x01 ? SL_BUFFERABLE : 0;
		pgprot |= tex & 0x02 ? SL_CACHEABLE : 0;
		pgprot |= tex & 0x04 ? SL_TEX0 : 0;
	}

	fl_offset = FL_OFFSET(va);	/* Upper 12 bits */
	fl_pte = fl_table + fl_offset;	/* int pointers, 4 bytes */

	if (len == SZ_16M) {
		int i = 0;
		for (i = 0; i < 16; i++)
			*(fl_pte+i) = (pa & 0xFF000000) | FL_SUPERSECTION |
				  FL_AP_READ | FL_AP_WRITE | FL_TYPE_SECT |
				  FL_SHARED | FL_NG | pgprot;
	}

	if (len == SZ_1M)
		*fl_pte = (pa & 0xFFF00000) | FL_AP_READ | FL_AP_WRITE | FL_NG |
					    FL_TYPE_SECT | FL_SHARED | pgprot;

	/* Need a 2nd level table */
	if ((len == SZ_4K || len == SZ_64K) && (*fl_pte) == 0) {
		unsigned long *sl;
		sl = (unsigned long *) __get_free_pages(GFP_ATOMIC,
							get_order(SZ_4K));

		if (!sl) {
			pr_debug("Could not allocate second level table\n");
			ret = -ENOMEM;
			goto fail;
		}

		memset(sl, 0, SZ_4K);
		*fl_pte = ((((int)__pa(sl)) & FL_BASE_MASK) | FL_TYPE_TABLE);
	}

	sl_table = (unsigned long *) __va(((*fl_pte) & FL_BASE_MASK));
	sl_offset = SL_OFFSET(va);
	sl_pte = sl_table + sl_offset;


	if (len == SZ_4K)
		*sl_pte = (pa & SL_BASE_MASK_SMALL) | SL_AP0 | SL_AP1 | SL_NG |
					  SL_SHARED | SL_TYPE_SMALL | pgprot;

	if (len == SZ_64K) {
		int i;

		for (i = 0; i < 16; i++)
			*(sl_pte+i) = (pa & SL_BASE_MASK_LARGE) | SL_AP0 |
			    SL_NG | SL_AP1 | SL_SHARED | SL_TYPE_LARGE | pgprot;
	}

	ret = __flush_iotlb(domain);
fail:
	spin_unlock_irqrestore(&msm_iommu_lock, flags);
	return ret;
}

static size_t msm_iommu_unmap(struct iommu_domain *domain, unsigned long va,
			    size_t len)
{
	struct msm_priv *priv;
	unsigned long flags;
	unsigned long *fl_table;
	unsigned long *fl_pte;
	unsigned long fl_offset;
	unsigned long *sl_table;
	unsigned long *sl_pte;
	unsigned long sl_offset;
	int i, ret = 0;

	spin_lock_irqsave(&msm_iommu_lock, flags);

	priv = domain->priv;

	if (!priv)
		goto fail;

	fl_table = priv->pgtable;

	if (len != SZ_16M && len != SZ_1M &&
	    len != SZ_64K && len != SZ_4K) {
		pr_debug("Bad length: %d\n", len);
		goto fail;
	}

	if (!fl_table) {
		pr_debug("Null page table\n");
		goto fail;
	}

	fl_offset = FL_OFFSET(va);	/* Upper 12 bits */
	fl_pte = fl_table + fl_offset;	/* int pointers, 4 bytes */

	if (*fl_pte == 0) {
		pr_debug("First level PTE is 0\n");
		goto fail;
	}

	/* Unmap supersection */
	if (len == SZ_16M)
		for (i = 0; i < 16; i++)
			*(fl_pte+i) = 0;

	if (len == SZ_1M)
		*fl_pte = 0;

	sl_table = (unsigned long *) __va(((*fl_pte) & FL_BASE_MASK));
	sl_offset = SL_OFFSET(va);
	sl_pte = sl_table + sl_offset;

	if (len == SZ_64K) {
		for (i = 0; i < 16; i++)
			*(sl_pte+i) = 0;
	}

	if (len == SZ_4K)
		*sl_pte = 0;

	if (len == SZ_4K || len == SZ_64K) {
		int used = 0;

		for (i = 0; i < NUM_SL_PTE; i++)
			if (sl_table[i])
				used = 1;
		if (!used) {
			free_page((unsigned long)sl_table);
			*fl_pte = 0;
		}
	}

	ret = __flush_iotlb(domain);

fail:
	spin_unlock_irqrestore(&msm_iommu_lock, flags);

	/* the IOMMU API requires us to return how many bytes were unmapped */
	len = ret ? 0 : len;
	return len;
}

static phys_addr_t msm_iommu_iova_to_phys(struct iommu_domain *domain,
					  dma_addr_t va)
{
	struct msm_priv *priv;
	struct msm_iommu_drvdata *iommu_drvdata;
	struct msm_iommu_ctx_drvdata *ctx_drvdata;
	unsigned int par;
	unsigned long flags;
	void __iomem *base;
	phys_addr_t ret = 0;
	int ctx;

	spin_lock_irqsave(&msm_iommu_lock, flags);

	priv = domain->priv;
	if (list_empty(&priv->list_attached))
		goto fail;

	ctx_drvdata = list_entry(priv->list_attached.next,
				 struct msm_iommu_ctx_drvdata, attached_elm);
	iommu_drvdata = dev_get_drvdata(ctx_drvdata->pdev->dev.parent);

	base = iommu_drvdata->base;
	ctx = ctx_drvdata->num;

	ret = __enable_clocks(iommu_drvdata);
	if (ret)
		goto fail;

	/* Invalidate context TLB */
	SET_CTX_TLBIALL(base, ctx, 0);
	SET_V2PPR(base, ctx, va & V2Pxx_VA);

	par = GET_PAR(base, ctx);

	/* We are dealing with a supersection */
	if (GET_NOFAULT_SS(base, ctx))
		ret = (par & 0xFF000000) | (va & 0x00FFFFFF);
	else	/* Upper 20 bits from PAR, lower 12 from VA */
		ret = (par & 0xFFFFF000) | (va & 0x00000FFF);

	if (GET_FAULT(base, ctx))
		ret = 0;

	__disable_clocks(iommu_drvdata);
fail:
	spin_unlock_irqrestore(&msm_iommu_lock, flags);
	return ret;
}

static bool msm_iommu_capable(enum iommu_cap cap)
{
	return false;
}

static void print_ctx_regs(void __iomem *base, int ctx)
{
	unsigned int fsr = GET_FSR(base, ctx);
	pr_err("FAR    = %08x    PAR    = %08x\n",
	       GET_FAR(base, ctx), GET_PAR(base, ctx));
	pr_err("FSR    = %08x [%s%s%s%s%s%s%s%s%s%s]\n", fsr,
			(fsr & 0x02) ? "TF " : "",
			(fsr & 0x04) ? "AFF " : "",
			(fsr & 0x08) ? "APF " : "",
			(fsr & 0x10) ? "TLBMF " : "",
			(fsr & 0x20) ? "HTWDEEF " : "",
			(fsr & 0x40) ? "HTWSEEF " : "",
			(fsr & 0x80) ? "MHF " : "",
			(fsr & 0x10000) ? "SL " : "",
			(fsr & 0x40000000) ? "SS " : "",
			(fsr & 0x80000000) ? "MULTI " : "");

	pr_err("FSYNR0 = %08x    FSYNR1 = %08x\n",
	       GET_FSYNR0(base, ctx), GET_FSYNR1(base, ctx));
	pr_err("TTBR0  = %08x    TTBR1  = %08x\n",
	       GET_TTBR0(base, ctx), GET_TTBR1(base, ctx));
	pr_err("SCTLR  = %08x    ACTLR  = %08x\n",
	       GET_SCTLR(base, ctx), GET_ACTLR(base, ctx));
	pr_err("PRRR   = %08x    NMRR   = %08x\n",
	       GET_PRRR(base, ctx), GET_NMRR(base, ctx));
}

irqreturn_t msm_iommu_fault_handler(int irq, void *dev_id)
{
	struct msm_iommu_drvdata *drvdata = dev_id;
	void __iomem *base;
	unsigned int fsr;
	int i, ret;

	spin_lock(&msm_iommu_lock);

	if (!drvdata) {
		pr_err("Invalid device ID in context interrupt handler\n");
		goto fail;
	}

	base = drvdata->base;

	pr_err("Unexpected IOMMU page fault!\n");
	pr_err("base = %08x\n", (unsigned int) base);

	ret = __enable_clocks(drvdata);
	if (ret)
		goto fail;

	for (i = 0; i < drvdata->ncb; i++) {
		fsr = GET_FSR(base, i);
		if (fsr) {
			pr_err("Fault occurred in context %d.\n", i);
			pr_err("Interesting registers:\n");
			print_ctx_regs(base, i);
			SET_FSR(base, i, 0x4000000F);
		}
	}
	__disable_clocks(drvdata);
fail:
	spin_unlock(&msm_iommu_lock);
	return 0;
}

static const struct iommu_ops msm_iommu_ops = {
	.capable = msm_iommu_capable,
	.domain_init = msm_iommu_domain_init,
	.domain_destroy = msm_iommu_domain_destroy,
	.attach_dev = msm_iommu_attach_dev,
	.detach_dev = msm_iommu_detach_dev,
	.map = msm_iommu_map,
	.unmap = msm_iommu_unmap,
	.iova_to_phys = msm_iommu_iova_to_phys,
	.pgsize_bitmap = MSM_IOMMU_PGSIZES,
};

static int __init get_tex_class(int icp, int ocp, int mt, int nos)
{
	int i = 0;
	unsigned int prrr = 0;
	unsigned int nmrr = 0;
	int c_icp, c_ocp, c_mt, c_nos;

	RCP15_PRRR(prrr);
	RCP15_NMRR(nmrr);

	for (i = 0; i < NUM_TEX_CLASS; i++) {
		c_nos = PRRR_NOS(prrr, i);
		c_mt = PRRR_MT(prrr, i);
		c_icp = NMRR_ICP(nmrr, i);
		c_ocp = NMRR_OCP(nmrr, i);

		if (icp == c_icp && ocp == c_ocp && c_mt == mt && c_nos == nos)
			return i;
	}

	return -ENODEV;
}

static void __init setup_iommu_tex_classes(void)
{
	msm_iommu_tex_class[MSM_IOMMU_ATTR_NONCACHED] =
			get_tex_class(CP_NONCACHED, CP_NONCACHED, MT_NORMAL, 1);

	msm_iommu_tex_class[MSM_IOMMU_ATTR_CACHED_WB_WA] =
			get_tex_class(CP_WB_WA, CP_WB_WA, MT_NORMAL, 1);

	msm_iommu_tex_class[MSM_IOMMU_ATTR_CACHED_WB_NWA] =
			get_tex_class(CP_WB_NWA, CP_WB_NWA, MT_NORMAL, 1);

	msm_iommu_tex_class[MSM_IOMMU_ATTR_CACHED_WT] =
			get_tex_class(CP_WT, CP_WT, MT_NORMAL, 1);
}

static int __init msm_iommu_init(void)
{
	setup_iommu_tex_classes();
	bus_set_iommu(&platform_bus_type, &msm_iommu_ops);
	return 0;
}

subsys_initcall(msm_iommu_init);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Stepan Moskovchenko <stepanm@codeaurora.org>");
>>>>>>> android-3.18
