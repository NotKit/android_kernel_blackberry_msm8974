<<<<<<< HEAD
#ifndef ASM_DMA_CONTIGUOUS_H
#define ASM_DMA_CONTIGUOUS_H

#ifdef __KERNEL__
#ifdef CONFIG_CMA

#include <linux/device.h>
#include <linux/dma-contiguous.h>

static inline struct cma *dev_get_cma_area(struct device *dev)
{
	if (dev && dev->cma_area)
		return dev->cma_area;
	return dma_contiguous_def_area;
}

static inline void dev_set_cma_area(struct device *dev, struct cma *cma)
{
	if (dev)
		dev->cma_area = cma;
}

#endif
#endif
=======
#ifndef _ASM_GENERIC_DMA_CONTIGUOUS_H
#define _ASM_GENERIC_DMA_CONTIGUOUS_H

#include <linux/types.h>

static inline void
dma_contiguous_early_fixup(phys_addr_t base, unsigned long size) { }
>>>>>>> android-3.18

#endif
