#ifndef _LINUX_VIRTIO_H
#define _LINUX_VIRTIO_H
/* Everything a virtio driver needs to work with any particular virtio
 * implementation. */
#include <linux/types.h>
#include <linux/scatterlist.h>
#include <linux/spinlock.h>
#include <linux/device.h>
#include <linux/mod_devicetable.h>
#include <linux/gfp.h>
#include <linux/vringh.h>

/**
 * virtqueue - a queue to register buffers for sending or receiving.
 * @list: the chain of virtqueues for this device
 * @callback: the function to call when buffers are consumed (can be NULL).
 * @name: the name of this virtqueue (mainly for debugging)
 * @vdev: the virtio device this queue was created for.
 * @vq_ops: the operations for this virtqueue (see below).
 * @priv: a pointer for the virtqueue implementation to use.
 * @index: the zero-based ordinal number for this queue.
 * @num_free: number of elements we expect to be able to fit.
 *
 * A note on @num_free: with indirect buffers, each buffer needs one
 * element in the queue, otherwise a buffer will need one element per
 * sg element.
 */
struct virtqueue {
	struct list_head list;
	void (*callback)(struct virtqueue *vq);
	const char *name;
	struct virtio_device *vdev;
	unsigned int index;
	unsigned int num_free;
	void *priv;
};

int virtqueue_add_outbuf(struct virtqueue *vq,
			 struct scatterlist sg[], unsigned int num,
			 void *data,
			 gfp_t gfp);

int virtqueue_add_inbuf(struct virtqueue *vq,
			struct scatterlist sg[], unsigned int num,
			void *data,
			gfp_t gfp);

int virtqueue_add_sgs(struct virtqueue *vq,
		      struct scatterlist *sgs[],
		      unsigned int out_sgs,
		      unsigned int in_sgs,
		      void *data,
		      gfp_t gfp);

bool virtqueue_kick(struct virtqueue *vq);

/**
 * virtqueue_disable_cb - disable callbacks
 * @vq: the struct virtqueue we're talking about.
 *
 * Note that this is not necessarily synchronous, hence unreliable and only
 * useful as an optimization.
 *
 * Unlike other operations, this need not be serialized.
 */
static inline void virtqueue_disable_cb(struct virtqueue *vq)
{
	vq->vq_ops->disable_cb(vq);
}

bool virtqueue_notify(struct virtqueue *vq);

/**
 * virtqueue_enable_cb_delayed - restart callbacks after disable_cb.
 * @vq: the struct virtqueue we're talking about.
 *
 * This re-enables callbacks but hints to the other side to delay
 * interrupts until most of the available buffers have been processed;
 * it returns "false" if there are many pending buffers in the queue,
 * to detect a possible race between the driver checking for more work,
 * and enabling callbacks.
 *
 * Caller must ensure we don't call this with other virtqueue
 * operations at the same time (except where noted).
 */
static inline bool virtqueue_enable_cb_delayed(struct virtqueue *vq)
{
	return vq->vq_ops->enable_cb_delayed(vq);
}

unsigned virtqueue_enable_cb_prepare(struct virtqueue *vq);

bool virtqueue_poll(struct virtqueue *vq, unsigned);

unsigned virtqueue_enable_cb_prepare(struct virtqueue *vq);

bool virtqueue_poll(struct virtqueue *vq, unsigned);

bool virtqueue_enable_cb_delayed(struct virtqueue *vq);

/**
 * virtqueue_detach_unused_buf - detach first unused buffer
 * @vq: the struct virtqueue we're talking about.
 *
 * Returns NULL or the "data" token handed to virtqueue_add_buf().
 * This is not valid on an active queue; it is useful only for device
 * shutdown.
 */
static inline void *virtqueue_detach_unused_buf(struct virtqueue *vq)
{
	return vq->vq_ops->detach_unused_buf(vq);
}

/**
 * virtqueue_get_impl_size - return the size of the virtqueue's implementation
 * @vq: the struct virtqueue containing the implementation of interest.
 *
 * Returns the size of the virtqueue implementation.  This is mainly used
 * for boasting to userspace.  Unlike other operations, this need not
 * be serialized.
 */
static inline unsigned int virtqueue_get_impl_size(struct virtqueue *vq)
{
	return vq->vq_ops->get_impl_size(vq);
}

bool virtqueue_is_broken(struct virtqueue *vq);

/**
 * virtio_device - representation of a device using virtio
 * @index: unique position on the virtio bus
 * @failed: saved value for CONFIG_S_FAILED bit (for restore)
 * @config_enabled: configuration change reporting enabled
 * @config_change_pending: configuration change reported while disabled
 * @config_lock: protects configuration change reporting
 * @dev: underlying device.
 * @id: the device type identification (used to match it with a driver).
 * @config: the configuration ops for this device.
 * @vringh_config: configuration ops for host vrings.
 * @vqs: the list of virtqueues for this device.
 * @features: the features supported by both driver and device.
 * @priv: private pointer for the driver's use.
 */
struct virtio_device {
	int index;
	bool failed;
	bool config_enabled;
	bool config_change_pending;
	spinlock_t config_lock;
	struct device dev;
	struct virtio_device_id id;
	const struct virtio_config_ops *config;
	const struct vringh_config_ops *vringh_config;
	struct list_head vqs;
	/* Note that this is a Linux set_bit-style bitmap. */
	unsigned long features[1];
	void *priv;
};

static inline struct virtio_device *dev_to_virtio(struct device *_dev)
{
	return container_of(_dev, struct virtio_device, dev);
}

int register_virtio_device(struct virtio_device *dev);
void unregister_virtio_device(struct virtio_device *dev);

void virtio_break_device(struct virtio_device *dev);

void virtio_config_changed(struct virtio_device *dev);
#ifdef CONFIG_PM_SLEEP
int virtio_device_freeze(struct virtio_device *dev);
int virtio_device_restore(struct virtio_device *dev);
#endif

#define virtio_device_for_each_vq(vdev, vq) \
	list_for_each_entry(vq, &vdev->vqs, list)

/**
 * virtio_driver - operations for a virtio I/O driver
 * @driver: underlying device driver (populate name and owner).
 * @id_table: the ids serviced by this driver.
 * @feature_table: an array of feature numbers supported by this driver.
 * @feature_table_size: number of entries in the feature table array.
 * @probe: the function to call when a device is found.  Returns 0 or -errno.
 * @remove: the function to call when a device is removed.
 * @config_changed: optional function to call when the device configuration
 *    changes; may be called in interrupt context.
 */
struct virtio_driver {
	struct device_driver driver;
	const struct virtio_device_id *id_table;
	const unsigned int *feature_table;
	unsigned int feature_table_size;
	int (*probe)(struct virtio_device *dev);
	void (*scan)(struct virtio_device *dev);
	void (*remove)(struct virtio_device *dev);
	void (*config_changed)(struct virtio_device *dev);
#ifdef CONFIG_PM
	int (*freeze)(struct virtio_device *dev);
	int (*restore)(struct virtio_device *dev);
#endif
};

static inline struct virtio_driver *drv_to_virtio(struct device_driver *drv)
{
	return container_of(drv, struct virtio_driver, driver);
}

int register_virtio_driver(struct virtio_driver *drv);
void unregister_virtio_driver(struct virtio_driver *drv);

/* module_virtio_driver() - Helper macro for drivers that don't do
 * anything special in module init/exit.  This eliminates a lot of
 * boilerplate.  Each module may only use this macro once, and
 * calling it replaces module_init() and module_exit()
 */
#define module_virtio_driver(__virtio_driver) \
	module_driver(__virtio_driver, register_virtio_driver, \
			unregister_virtio_driver)
#endif /* _LINUX_VIRTIO_H */
