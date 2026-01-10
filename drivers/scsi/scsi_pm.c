/*
 *	scsi_pm.c	Copyright (C) 2010 Alan Stern
 *
 *	SCSI dynamic Power Management
 *		Initial version: Alan Stern <stern@rowland.harvard.edu>
 */

#include <linux/pm_runtime.h>
#include <linux/export.h>
#include <linux/async.h>

#include <scsi/scsi.h>
#include <scsi/scsi_device.h>
#include <scsi/scsi_driver.h>
#include <scsi/scsi_host.h>

#include "scsi_priv.h"

<<<<<<< HEAD
static int scsi_dev_type_suspend(struct device *dev, int (*cb)(struct device *))
{
=======
#ifdef CONFIG_PM_SLEEP

static int do_scsi_suspend(struct device *dev, const struct dev_pm_ops *pm)
{
	return pm && pm->suspend ? pm->suspend(dev) : 0;
}

static int do_scsi_freeze(struct device *dev, const struct dev_pm_ops *pm)
{
	return pm && pm->freeze ? pm->freeze(dev) : 0;
}

static int do_scsi_poweroff(struct device *dev, const struct dev_pm_ops *pm)
{
	return pm && pm->poweroff ? pm->poweroff(dev) : 0;
}

static int do_scsi_resume(struct device *dev, const struct dev_pm_ops *pm)
{
	return pm && pm->resume ? pm->resume(dev) : 0;
}

static int do_scsi_thaw(struct device *dev, const struct dev_pm_ops *pm)
{
	return pm && pm->thaw ? pm->thaw(dev) : 0;
}

static int do_scsi_restore(struct device *dev, const struct dev_pm_ops *pm)
{
	return pm && pm->restore ? pm->restore(dev) : 0;
}

static int scsi_dev_type_suspend(struct device *dev,
		int (*cb)(struct device *, const struct dev_pm_ops *))
{
	const struct dev_pm_ops *pm = dev->driver ? dev->driver->pm : NULL;
>>>>>>> android-3.18
	int err;

	/* flush pending in-flight resume operations, suspend is synchronous */
	async_synchronize_full_domain(&scsi_sd_pm_domain);

	err = scsi_device_quiesce(to_scsi_device(dev));
	if (err == 0) {
<<<<<<< HEAD
		if (cb) {
			err = cb(dev);
			if (err)
				scsi_device_resume(to_scsi_device(dev));
		}
=======
		err = cb(dev, pm);
		if (err)
			scsi_device_resume(to_scsi_device(dev));
>>>>>>> android-3.18
	}
	dev_dbg(dev, "scsi suspend: %d\n", err);
	return err;
}

<<<<<<< HEAD
static int scsi_dev_type_resume(struct device *dev, int (*cb)(struct device *))
{
	int err = 0;

	if (cb)
		err = cb(dev);
=======
static int scsi_dev_type_resume(struct device *dev,
		int (*cb)(struct device *, const struct dev_pm_ops *))
{
	const struct dev_pm_ops *pm = dev->driver ? dev->driver->pm : NULL;
	int err = 0;

	err = cb(dev, pm);
>>>>>>> android-3.18
	scsi_device_resume(to_scsi_device(dev));
	dev_dbg(dev, "scsi resume: %d\n", err);

	if (err == 0) {
		pm_runtime_disable(dev);
		pm_runtime_set_active(dev);
		pm_runtime_enable(dev);
	}

	return err;
}

<<<<<<< HEAD
#ifdef CONFIG_PM_SLEEP

static int
scsi_bus_suspend_common(struct device *dev, int (*cb)(struct device *))
=======
static int
scsi_bus_suspend_common(struct device *dev,
		int (*cb)(struct device *, const struct dev_pm_ops *))
>>>>>>> android-3.18
{
	int err = 0;

	if (scsi_is_sdev_device(dev)) {
		/*
		 * All the high-level SCSI drivers that implement runtime
		 * PM treat runtime suspend, system suspend, and system
<<<<<<< HEAD
		 * hibernate identically.
=======
		 * hibernate nearly identically. In all cases the requirements
		 * for runtime suspension are stricter.
>>>>>>> android-3.18
		 */
		if (pm_runtime_suspended(dev))
			return 0;

		err = scsi_dev_type_suspend(dev, cb);
	}

	return err;
}

<<<<<<< HEAD
static int
scsi_bus_resume_common(struct device *dev, int (*cb)(struct device *))
=======
static void async_sdev_resume(void *dev, async_cookie_t cookie)
>>>>>>> android-3.18
{
	scsi_dev_type_resume(dev, do_scsi_resume);
}

static void async_sdev_thaw(void *dev, async_cookie_t cookie)
{
	scsi_dev_type_resume(dev, do_scsi_thaw);
}

static void async_sdev_restore(void *dev, async_cookie_t cookie)
{
	scsi_dev_type_resume(dev, do_scsi_restore);
}

static int scsi_bus_resume_common(struct device *dev,
		int (*cb)(struct device *, const struct dev_pm_ops *))
{
	async_func_t fn;

	if (!scsi_is_sdev_device(dev))
		fn = NULL;
	else if (cb == do_scsi_resume)
		fn = async_sdev_resume;
	else if (cb == do_scsi_thaw)
		fn = async_sdev_thaw;
	else if (cb == do_scsi_restore)
		fn = async_sdev_restore;
	else
		fn = NULL;

	if (fn) {
		async_schedule_domain(fn, dev, &scsi_sd_pm_domain);

		/*
		 * If a user has disabled async probing a likely reason
		 * is due to a storage enclosure that does not inject
		 * staggered spin-ups.  For safety, make resume
		 * synchronous as well in that case.
		 */
<<<<<<< HEAD
		pm_runtime_get_sync(dev->parent);
		err = scsi_dev_type_resume(dev, cb);
		pm_runtime_put_sync(dev->parent);
	}

	if (err == 0) {
=======
		if (strncmp(scsi_scan_type, "async", 5) != 0)
			async_synchronize_full_domain(&scsi_sd_pm_domain);
	} else {
>>>>>>> android-3.18
		pm_runtime_disable(dev);
		pm_runtime_set_active(dev);
		pm_runtime_enable(dev);
	}
	return 0;
}

static int scsi_bus_prepare(struct device *dev)
{
	if (scsi_is_sdev_device(dev)) {
		/* sd probing uses async_schedule.  Wait until it finishes. */
		async_synchronize_full_domain(&scsi_sd_probe_domain);

	} else if (scsi_is_host_device(dev)) {
		/* Wait until async scanning is finished */
		scsi_complete_async_scans();
	}
	return 0;
}

static int scsi_bus_suspend(struct device *dev)
{
<<<<<<< HEAD
	const struct dev_pm_ops *pm = dev->driver ? dev->driver->pm : NULL;
	return scsi_bus_suspend_common(dev, pm ? pm->suspend : NULL);
=======
	return scsi_bus_suspend_common(dev, do_scsi_suspend);
>>>>>>> android-3.18
}

static int scsi_bus_resume(struct device *dev)
{
<<<<<<< HEAD
	const struct dev_pm_ops *pm = dev->driver ? dev->driver->pm : NULL;
	return scsi_bus_resume_common(dev, pm ? pm->resume : NULL);
=======
	return scsi_bus_resume_common(dev, do_scsi_resume);
>>>>>>> android-3.18
}

static int scsi_bus_freeze(struct device *dev)
{
<<<<<<< HEAD
	const struct dev_pm_ops *pm = dev->driver ? dev->driver->pm : NULL;
	return scsi_bus_suspend_common(dev, pm ? pm->freeze : NULL);
=======
	return scsi_bus_suspend_common(dev, do_scsi_freeze);
>>>>>>> android-3.18
}

static int scsi_bus_thaw(struct device *dev)
{
<<<<<<< HEAD
	const struct dev_pm_ops *pm = dev->driver ? dev->driver->pm : NULL;
	return scsi_bus_resume_common(dev, pm ? pm->thaw : NULL);
=======
	return scsi_bus_resume_common(dev, do_scsi_thaw);
>>>>>>> android-3.18
}

static int scsi_bus_poweroff(struct device *dev)
{
<<<<<<< HEAD
	const struct dev_pm_ops *pm = dev->driver ? dev->driver->pm : NULL;
	return scsi_bus_suspend_common(dev, pm ? pm->poweroff : NULL);
=======
	return scsi_bus_suspend_common(dev, do_scsi_poweroff);
>>>>>>> android-3.18
}

static int scsi_bus_restore(struct device *dev)
{
<<<<<<< HEAD
	const struct dev_pm_ops *pm = dev->driver ? dev->driver->pm : NULL;
	return scsi_bus_resume_common(dev, pm ? pm->restore : NULL);
=======
	return scsi_bus_resume_common(dev, do_scsi_restore);
>>>>>>> android-3.18
}

#else /* CONFIG_PM_SLEEP */

#define scsi_bus_prepare		NULL
#define scsi_bus_suspend		NULL
#define scsi_bus_resume			NULL
#define scsi_bus_freeze			NULL
#define scsi_bus_thaw			NULL
#define scsi_bus_poweroff		NULL
#define scsi_bus_restore		NULL

#endif /* CONFIG_PM_SLEEP */

#ifdef CONFIG_PM_RUNTIME

<<<<<<< HEAD
static int sdev_blk_runtime_suspend(struct scsi_device *sdev,
					int (*cb)(struct device *))
{
	int err;
=======
static int sdev_runtime_suspend(struct device *dev)
{
	const struct dev_pm_ops *pm = dev->driver ? dev->driver->pm : NULL;
	struct scsi_device *sdev = to_scsi_device(dev);
	int err = 0;
>>>>>>> android-3.18

	err = blk_pre_runtime_suspend(sdev->request_queue);
	if (err)
		return err;
<<<<<<< HEAD
	if (cb)
		err = cb(&sdev->sdev_gendev);
=======
	if (pm && pm->runtime_suspend)
		err = pm->runtime_suspend(dev);
>>>>>>> android-3.18
	blk_post_runtime_suspend(sdev->request_queue, err);

	return err;
}

<<<<<<< HEAD
static int sdev_runtime_suspend(struct device *dev)
{
	const struct dev_pm_ops *pm = dev->driver ? dev->driver->pm : NULL;
	int (*cb)(struct device *) = pm ? pm->runtime_suspend : NULL;
	struct scsi_device *sdev = to_scsi_device(dev);
	int err;

	if (sdev->request_queue->dev)
		return sdev_blk_runtime_suspend(sdev, cb);

	err = scsi_dev_type_suspend(dev, cb);
	if (err == -EAGAIN)
		pm_schedule_suspend(dev, jiffies_to_msecs(
					round_jiffies_up_relative(HZ/10)));
	return err;
}

=======
>>>>>>> android-3.18
static int scsi_runtime_suspend(struct device *dev)
{
	int err = 0;

	dev_dbg(dev, "scsi_runtime_suspend\n");
	if (scsi_is_sdev_device(dev))
		err = sdev_runtime_suspend(dev);

	/* Insert hooks here for targets, hosts, and transport classes */

	return err;
}

<<<<<<< HEAD
static int sdev_blk_runtime_resume(struct scsi_device *sdev,
					int (*cb)(struct device *))
{
	int err = 0;

	blk_pre_runtime_resume(sdev->request_queue);
	if (cb)
		err = cb(&sdev->sdev_gendev);
=======
static int sdev_runtime_resume(struct device *dev)
{
	struct scsi_device *sdev = to_scsi_device(dev);
	const struct dev_pm_ops *pm = dev->driver ? dev->driver->pm : NULL;
	int err = 0;

	blk_pre_runtime_resume(sdev->request_queue);
	if (pm && pm->runtime_resume)
		err = pm->runtime_resume(dev);
>>>>>>> android-3.18
	blk_post_runtime_resume(sdev->request_queue, err);

	return err;
}

<<<<<<< HEAD
static int sdev_runtime_resume(struct device *dev)
{
	struct scsi_device *sdev = to_scsi_device(dev);
	const struct dev_pm_ops *pm = dev->driver ? dev->driver->pm : NULL;
	int (*cb)(struct device *) = pm ? pm->runtime_resume : NULL;

	if (sdev->request_queue->dev)
		return sdev_blk_runtime_resume(sdev, cb);
	else
		return scsi_dev_type_resume(dev, cb);
}

=======
>>>>>>> android-3.18
static int scsi_runtime_resume(struct device *dev)
{
	int err = 0;

	dev_dbg(dev, "scsi_runtime_resume\n");
	if (scsi_is_sdev_device(dev))
		err = sdev_runtime_resume(dev);

	/* Insert hooks here for targets, hosts, and transport classes */

	return err;
}

static int scsi_runtime_idle(struct device *dev)
{
	dev_dbg(dev, "scsi_runtime_idle\n");

	/* Insert hooks here for targets, hosts, and transport classes */

	if (scsi_is_sdev_device(dev)) {
<<<<<<< HEAD
		struct scsi_device *sdev = to_scsi_device(dev);

		if (sdev->request_queue->dev) {
			pm_runtime_mark_last_busy(dev);
			err = pm_runtime_autosuspend(dev);
		} else {
			err = pm_runtime_suspend(dev);
		}
	} else {
		err = pm_runtime_suspend(dev);
	}
	return err;
=======
		pm_runtime_mark_last_busy(dev);
		pm_runtime_autosuspend(dev);
		return -EBUSY;
	}

	return 0;
>>>>>>> android-3.18
}

int scsi_autopm_get_device(struct scsi_device *sdev)
{
	int	err;

	err = pm_runtime_get_sync(&sdev->sdev_gendev);
	if (err < 0 && err !=-EACCES)
		pm_runtime_put_sync(&sdev->sdev_gendev);
	else
		err = 0;
	return err;
}
EXPORT_SYMBOL_GPL(scsi_autopm_get_device);

void scsi_autopm_put_device(struct scsi_device *sdev)
{
	pm_runtime_put_sync(&sdev->sdev_gendev);
}
EXPORT_SYMBOL_GPL(scsi_autopm_put_device);

void scsi_autopm_get_target(struct scsi_target *starget)
{
	pm_runtime_get_sync(&starget->dev);
}

void scsi_autopm_put_target(struct scsi_target *starget)
{
	pm_runtime_put_sync(&starget->dev);
}

int scsi_autopm_get_host(struct Scsi_Host *shost)
{
	int	err;

	err = pm_runtime_get_sync(&shost->shost_gendev);
	if (err < 0 && err !=-EACCES)
		pm_runtime_put_sync(&shost->shost_gendev);
	else
		err = 0;
	return err;
}

void scsi_autopm_put_host(struct Scsi_Host *shost)
{
	pm_runtime_put_sync(&shost->shost_gendev);
}

#else

#define scsi_runtime_suspend	NULL
#define scsi_runtime_resume	NULL
#define scsi_runtime_idle	NULL

#endif /* CONFIG_PM_RUNTIME */

const struct dev_pm_ops scsi_bus_pm_ops = {
	.prepare =		scsi_bus_prepare,
	.suspend =		scsi_bus_suspend,
	.resume =		scsi_bus_resume,
	.freeze =		scsi_bus_freeze,
	.thaw =			scsi_bus_thaw,
	.poweroff =		scsi_bus_poweroff,
	.restore =		scsi_bus_restore,
	.runtime_suspend =	scsi_runtime_suspend,
	.runtime_resume =	scsi_runtime_resume,
	.runtime_idle =		scsi_runtime_idle,
};
