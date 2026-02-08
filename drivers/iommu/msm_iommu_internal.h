/*
 * MSM IOMMU internal definitions
 */
#ifndef MSM_IOMMU_INTERNAL_H
#define MSM_IOMMU_INTERNAL_H

#include <linux/list.h>

struct msm_iommu_pt {
	unsigned long *fl_table;
	int redirect;
};

struct msm_iommu_priv {
	struct msm_iommu_pt pt;
	struct list_head list_attached;
};

#endif
