/*
 * xhci-plat.c - xHCI host controller driver platform Bus Glue.
 *
 * Copyright (C) 2012 Texas Instruments Incorporated - http://www.ti.com
 * Author: Sebastian Andrzej Siewior <bigeasy@linutronix.de>
 *
 * A lot of code borrowed from the Linux xHCI driver.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 */

<<<<<<< HEAD
#include <linux/platform_device.h>
#include <linux/pm_runtime.h>
=======
#include <linux/clk.h>
#include <linux/dma-mapping.h>
>>>>>>> android-3.18
#include <linux/module.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
<<<<<<< HEAD
#include <linux/usb/otg.h>
#include <linux/usb/msm_hsusb.h>
=======
#include <linux/usb/xhci_pdriver.h>
>>>>>>> android-3.18

#include "xhci.h"
#include "xhci-mvebu.h"
#include "xhci-rcar.h"

static struct hc_driver __read_mostly xhci_plat_hc_driver;

#define SYNOPSIS_DWC3_VENDOR	0x5533

static struct usb_phy *phy;

static void xhci_plat_quirks(struct device *dev, struct xhci_hcd *xhci)
{
	struct xhci_plat_data *pdata = dev->platform_data;

	/*
	 * As of now platform drivers don't provide MSI support so we ensure
	 * here that the generic code does not try to make a pci_dev from our
	 * dev struct in order to setup MSI
	 */
	xhci->quirks |= XHCI_PLAT;
<<<<<<< HEAD

	if (!pdata)
		return;

	if (pdata->vendor == SYNOPSIS_DWC3_VENDOR && pdata->revision < 0x230A)
		xhci->quirks |= XHCI_PORTSC_DELAY;

	if (pdata->vendor == SYNOPSIS_DWC3_VENDOR && pdata->revision == 0x250A)
		xhci->quirks |= XHCI_RESET_DELAY;
=======
>>>>>>> android-3.18
}

/* called during probe() after chip reset completes */
static int xhci_plat_setup(struct usb_hcd *hcd)
{
	struct device_node *of_node = hcd->self.controller->of_node;
	int ret;

	if (of_device_is_compatible(of_node, "renesas,xhci-r8a7790") ||
	    of_device_is_compatible(of_node, "renesas,xhci-r8a7791")) {
		ret = xhci_rcar_init_quirk(hcd);
		if (ret)
			return ret;
	}

	return xhci_gen_setup(hcd, xhci_plat_quirks);
}

<<<<<<< HEAD
static void xhci_plat_phy_autosuspend(struct usb_hcd *hcd,
						int enable_autosuspend)
{
	if (!phy || !phy->set_phy_autosuspend)
		return;

	usb_phy_set_autosuspend(phy, enable_autosuspend);

	return;
}

static const struct hc_driver xhci_plat_xhci_driver = {
	.description =		"xhci-hcd",
	.product_desc =		"xHCI Host Controller",
	.hcd_priv_size =	sizeof(struct xhci_hcd *),

	/*
	 * generic hardware linkage
	 */
	.irq =			xhci_irq,
	.flags =		HCD_MEMORY | HCD_USB3 | HCD_SHARED,

	/*
	 * basic lifecycle operations
	 */
	.reset =		xhci_plat_setup,
	.start =		xhci_run,
	.stop =			xhci_stop,
	.shutdown =		xhci_shutdown,

	/*
	 * managing i/o requests and associated device resources
	 */
	.urb_enqueue =		xhci_urb_enqueue,
	.urb_dequeue =		xhci_urb_dequeue,
	.alloc_dev =		xhci_alloc_dev,
	.free_dev =		xhci_free_dev,
	.alloc_streams =	xhci_alloc_streams,
	.free_streams =		xhci_free_streams,
	.add_endpoint =		xhci_add_endpoint,
	.drop_endpoint =	xhci_drop_endpoint,
	.endpoint_reset =	xhci_endpoint_reset,
	.check_bandwidth =	xhci_check_bandwidth,
	.reset_bandwidth =	xhci_reset_bandwidth,
	.address_device =	xhci_address_device,
	.update_hub_device =	xhci_update_hub_device,
	.reset_device =		xhci_discover_or_reset_device,
=======
static int xhci_plat_start(struct usb_hcd *hcd)
{
	struct device_node *of_node = hcd->self.controller->of_node;
>>>>>>> android-3.18

	if (of_device_is_compatible(of_node, "renesas,xhci-r8a7790") ||
	    of_device_is_compatible(of_node, "renesas,xhci-r8a7791"))
		xhci_rcar_start(hcd);

<<<<<<< HEAD
	/* Root hub support */
	.hub_control =		xhci_hub_control,
	.hub_status_data =	xhci_hub_status_data,
	.bus_suspend =		xhci_bus_suspend,
	.bus_resume =		xhci_bus_resume,
	.set_autosuspend =	xhci_plat_phy_autosuspend,
};
=======
	return xhci_run(hcd);
}
>>>>>>> android-3.18

static int xhci_plat_probe(struct platform_device *pdev)
{
	struct device_node	*node = pdev->dev.of_node;
	struct usb_xhci_pdata	*pdata = dev_get_platdata(&pdev->dev);
	const struct hc_driver	*driver;
	struct xhci_hcd		*xhci;
	struct resource         *res;
	struct usb_hcd		*hcd;
	struct clk              *clk;
	int			ret;
	int			irq;

	if (usb_disabled())
		return -ENODEV;

	driver = &xhci_plat_hc_driver;

	irq = platform_get_irq(pdev, 0);
	if (irq < 0)
		return irq;

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!res)
		return -ENODEV;

	/* Initialize dma_mask and coherent_dma_mask to 32-bits */
	ret = dma_set_coherent_mask(&pdev->dev, DMA_BIT_MASK(32));
	if (ret)
		return ret;
	if (!pdev->dev.dma_mask)
		pdev->dev.dma_mask = &pdev->dev.coherent_dma_mask;
	else
		dma_set_mask(&pdev->dev, DMA_BIT_MASK(32));

	hcd = usb_create_hcd(driver, &pdev->dev, dev_name(&pdev->dev));
	if (!hcd)
		return -ENOMEM;

	hcd_to_bus(hcd)->skip_resume = true;
	hcd->rsrc_start = res->start;
	hcd->rsrc_len = resource_size(res);

	hcd->regs = devm_ioremap_resource(&pdev->dev, res);
	if (IS_ERR(hcd->regs)) {
		ret = PTR_ERR(hcd->regs);
		goto put_hcd;
	}

<<<<<<< HEAD
	hcd->regs = ioremap_nocache(hcd->rsrc_start, hcd->rsrc_len);
	if (!hcd->regs) {
		dev_dbg(&pdev->dev, "error mapping memory\n");
		ret = -EFAULT;
		goto release_mem_region;
=======
	/*
	 * Not all platforms have a clk so it is not an error if the
	 * clock does not exists.
	 */
	clk = devm_clk_get(&pdev->dev, NULL);
	if (!IS_ERR(clk)) {
		ret = clk_prepare_enable(clk);
		if (ret)
			goto put_hcd;
	} else if (PTR_ERR(clk) == -EPROBE_DEFER) {
		ret = -EPROBE_DEFER;
		goto put_hcd;
	}

	if (of_device_is_compatible(pdev->dev.of_node,
				    "marvell,armada-375-xhci") ||
	    of_device_is_compatible(pdev->dev.of_node,
				    "marvell,armada-380-xhci")) {
		ret = xhci_mvebu_mbus_init_quirk(pdev);
		if (ret)
			goto disable_clk;
>>>>>>> android-3.18
	}

	pm_runtime_set_active(&pdev->dev);
	pm_runtime_enable(&pdev->dev);
	pm_runtime_get_sync(&pdev->dev);

	ret = usb_add_hcd(hcd, irq, IRQF_SHARED);
	if (ret)
		goto disable_clk;

	device_wakeup_enable(hcd->self.controller);

	/* USB 2.0 roothub is stored in the platform_device now. */
	hcd = platform_get_drvdata(pdev);
	xhci = hcd_to_xhci(hcd);
	xhci->clk = clk;
	xhci->shared_hcd = usb_create_shared_hcd(driver, &pdev->dev,
			dev_name(&pdev->dev), hcd);
	if (!xhci->shared_hcd) {
		ret = -ENOMEM;
		goto dealloc_usb2_hcd;
	}

<<<<<<< HEAD
	hcd_to_bus(xhci->shared_hcd)->skip_resume = true;
=======
	if ((node && of_property_read_bool(node, "usb3-lpm-capable")) ||
			(pdata && pdata->usb3_lpm_capable))
		xhci->quirks |= XHCI_LPM_SUPPORT;
>>>>>>> android-3.18
	/*
	 * Set the xHCI pointer before xhci_plat_setup() (aka hcd_driver.reset)
	 * is called by usb_add_hcd().
	 */
	*((struct xhci_hcd **) xhci->shared_hcd->hcd_priv) = xhci;

	if (HCC_MAX_PSA(xhci->hcc_params) >= 4)
		xhci->shared_hcd->can_do_streams = 1;

	ret = usb_add_hcd(xhci->shared_hcd, irq, IRQF_SHARED);
	if (ret)
		goto put_usb3_hcd;

	phy = usb_get_transceiver();
	/* Register with OTG if present, ignore USB2 OTG using other PHY */
	if (phy && phy->otg && !(phy->flags & ENABLE_SECONDARY_PHY)) {
		dev_dbg(&pdev->dev, "%s otg support available\n", __func__);
		ret = otg_set_host(phy->otg, &hcd->self);
		if (ret) {
			dev_err(&pdev->dev, "%s otg_set_host failed\n",
				__func__);
			usb_put_transceiver(phy);
			goto put_usb3_hcd;
		}
	} else {
		pm_runtime_no_callbacks(&pdev->dev);
	}

	pm_runtime_put(&pdev->dev);

	return 0;

put_usb3_hcd:
	usb_put_hcd(xhci->shared_hcd);

dealloc_usb2_hcd:
	usb_remove_hcd(hcd);

disable_clk:
	if (!IS_ERR(clk))
		clk_disable_unprepare(clk);

put_hcd:
	usb_put_hcd(hcd);

	return ret;
}

static int xhci_plat_remove(struct platform_device *dev)
{
	struct usb_hcd	*hcd = platform_get_drvdata(dev);
	struct xhci_hcd	*xhci = hcd_to_xhci(hcd);
	struct clk *clk = xhci->clk;

	xhci->xhc_state |= XHCI_STATE_REMOVING;


	usb_remove_hcd(xhci->shared_hcd);
	usb_put_hcd(xhci->shared_hcd);

	usb_remove_hcd(hcd);
<<<<<<< HEAD
	iounmap(hcd->regs);
	release_mem_region(hcd->rsrc_start, hcd->rsrc_len);
=======
	if (!IS_ERR(clk))
		clk_disable_unprepare(clk);
>>>>>>> android-3.18
	usb_put_hcd(hcd);
	kfree(xhci);

	if (phy && phy->otg) {
		otg_set_host(phy->otg, NULL);
		usb_put_transceiver(phy);
	}

	return 0;
}

<<<<<<< HEAD
#ifdef CONFIG_PM_RUNTIME
static int xhci_msm_runtime_idle(struct device *dev)
{
	dev_dbg(dev, "xhci msm runtime idle\n");
	return 0;
}

static int xhci_msm_runtime_suspend(struct device *dev)
{
	dev_dbg(dev, "xhci msm runtime suspend\n");
	/*
	 * Notify OTG about suspend.  It takes care of
	 * putting the hardware in LPM.
	 */
	if (phy)
		return usb_phy_set_suspend(phy, 1);

	return 0;
}

static int xhci_msm_runtime_resume(struct device *dev)
{
	dev_dbg(dev, "xhci msm runtime resume\n");

	if (phy)
		return usb_phy_set_suspend(phy, 0);

	return 0;
}
#endif

#ifdef CONFIG_PM_SLEEP
static int xhci_msm_pm_suspend(struct device *dev)
{
	dev_dbg(dev, "xhci-msm PM suspend\n");

	if (phy)
		return usb_phy_set_suspend(phy, 1);

	return 0;
}

static int xhci_msm_pm_resume(struct device *dev)
{
	dev_dbg(dev, "xhci-msm PM resume\n");

	if (pm_runtime_suspended(dev))
		return 0;

	if (phy)
		return usb_phy_set_suspend(phy, 0);

	return 0;
}
#endif

static const struct dev_pm_ops xhci_msm_dev_pm_ops = {
	SET_SYSTEM_SLEEP_PM_OPS(xhci_msm_pm_suspend, xhci_msm_pm_resume)
	SET_RUNTIME_PM_OPS(xhci_msm_runtime_suspend, xhci_msm_runtime_resume,
				xhci_msm_runtime_idle)
};
=======
#ifdef CONFIG_PM_SLEEP
static int xhci_plat_suspend(struct device *dev)
{
	struct usb_hcd	*hcd = dev_get_drvdata(dev);
	struct xhci_hcd	*xhci = hcd_to_xhci(hcd);

	/*
	 * xhci_suspend() needs `do_wakeup` to know whether host is allowed
	 * to do wakeup during suspend. Since xhci_plat_suspend is currently
	 * only designed for system suspend, device_may_wakeup() is enough
	 * to dertermine whether host is allowed to do wakeup. Need to
	 * reconsider this when xhci_plat_suspend enlarges its scope, e.g.,
	 * also applies to runtime suspend.
	 */
	return xhci_suspend(xhci, device_may_wakeup(dev));
}

static int xhci_plat_resume(struct device *dev)
{
	struct usb_hcd	*hcd = dev_get_drvdata(dev);
	struct xhci_hcd	*xhci = hcd_to_xhci(hcd);

	return xhci_resume(xhci, 0);
}

static const struct dev_pm_ops xhci_plat_pm_ops = {
	SET_SYSTEM_SLEEP_PM_OPS(xhci_plat_suspend, xhci_plat_resume)
};
#define DEV_PM_OPS	(&xhci_plat_pm_ops)
#else
#define DEV_PM_OPS	NULL
#endif /* CONFIG_PM */

#ifdef CONFIG_OF
static const struct of_device_id usb_xhci_of_match[] = {
	{ .compatible = "generic-xhci" },
	{ .compatible = "xhci-platform" },
	{ .compatible = "marvell,armada-375-xhci"},
	{ .compatible = "marvell,armada-380-xhci"},
	{ .compatible = "renesas,xhci-r8a7790"},
	{ .compatible = "renesas,xhci-r8a7791"},
	{ },
};
MODULE_DEVICE_TABLE(of, usb_xhci_of_match);
#endif
>>>>>>> android-3.18

static struct platform_driver usb_xhci_driver = {
	.probe	= xhci_plat_probe,
	.remove	= xhci_plat_remove,
	.shutdown = usb_hcd_platform_shutdown,
	.driver	= {
		.name = "xhci-hcd",
<<<<<<< HEAD
		.pm = &xhci_msm_dev_pm_ops,
=======
		.pm = DEV_PM_OPS,
		.of_match_table = of_match_ptr(usb_xhci_of_match),
>>>>>>> android-3.18
	},
};
MODULE_ALIAS("platform:xhci-hcd");

static int __init xhci_plat_init(void)
{
	xhci_init_driver(&xhci_plat_hc_driver, xhci_plat_setup);
	xhci_plat_hc_driver.start = xhci_plat_start;
	return platform_driver_register(&usb_xhci_driver);
}
module_init(xhci_plat_init);

static void __exit xhci_plat_exit(void)
{
	platform_driver_unregister(&usb_xhci_driver);
}
module_exit(xhci_plat_exit);

MODULE_DESCRIPTION("xHCI Platform Host Controller Driver");
MODULE_LICENSE("GPL");
