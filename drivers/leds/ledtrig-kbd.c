// SPDX-License-Identifier: GPL-2.0
#include <linux/module.h>
#include <linux/leds.h>
#include <linux/input.h>
#include <linux/slab.h>
#include <linux/timer.h>
#include <linux/jiffies.h>
#include <linux/display_state.h>
#include <linux/ledtrig-kbd.h>
#include <linux/export.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>
#include <linux/mutex.h>
#include <linux/delay.h>
#include <linux/string.h>

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))
#endif

/* --------------------------------------------------------------------- */
/* State global                                                          */
/* --------------------------------------------------------------------- */

static struct kobject *kbd_kobj;

static struct led_trigger *kbd_trigger;
static struct input_handler kbd_input_handler;
static struct timer_list kbd_timer;
static struct timer_list fade_timer;

static bool led_on;
static int  fade_dir = 0; /* +1 = fade in, -1 = fade out */
static int  fade_level = 0;

static u8  user_brightness_scale = 255;
static bool kbd_update_in_progress;
static unsigned int user_timeout_sec      = 10;
static unsigned int user_fade_steps       = 20;
static unsigned int user_fade_interval_ms = 25;
static bool manual_mode = false;
static bool no_backlight = false;

static struct timer_list kbd_feedback_timer;
static bool kbd_feedback_in_progress = false;

/* 0 = triggered by physical kb, 1 = triggered by a capacitive touch input */
static unsigned int work_mode = 0;

struct kbd_client_ctx {
	bool is_touch_keypad; /* true = capacitive tp, false = keyboard */
    int  last_tracking_id;   /* -1 = no contact */
};


/* --------------------------------------------------------------------- */
/* Sysfs: manual_mode                                                    */
/* --------------------------------------------------------------------- */

static ssize_t no_bkl_show(struct kobject *kobj,
				struct kobj_attribute *attr, char *buf)
{
	return scnprintf(buf, PAGE_SIZE, "%u\n", no_backlight ? 1 : 0);
}


static ssize_t no_bkl_store(struct kobject *kobj,
				 struct kobj_attribute *attr,
				 const char *buf, size_t count)
{
	unsigned int v;
	int ret;

	ret = kstrtouint(buf, 0, &v);
	if (ret)
		return ret;

	if (v == 1) {
		no_backlight = true;
	}
	else {
		no_backlight = false;
	}

	return count;
}

static struct kobj_attribute no_bkl_attr =
	__ATTR(no_bkl, 0664, no_bkl_show, no_bkl_store);

static ssize_t manual_mode_show(struct kobject *kobj,
				struct kobj_attribute *attr, char *buf)
{
	return scnprintf(buf, PAGE_SIZE, "%u\n", manual_mode ? 1 : 0);
}

static ssize_t manual_mode_store(struct kobject *kobj,
				 struct kobj_attribute *attr,
				 const char *buf, size_t count)
{
	unsigned int v;
	int ret;

	ret = kstrtouint(buf, 0, &v);
	if (ret)
		return ret;

	if (v == 0) {
		if (manual_mode) {
			pr_info("kbd-trigger: exit manual mode -> restoring auto control\n");
			manual_mode = false;

			led_trigger_event(kbd_trigger, LED_OFF);
			led_on = false;
			fade_level = 0;
			fade_dir = 0;

			del_timer_sync(&fade_timer);
			del_timer_sync(&kbd_timer);

			/* restart timeout */
			mod_timer(&kbd_timer, jiffies + user_timeout_sec * HZ);
		}
	} else {
		if (!manual_mode) {
			pr_info("kbd-trigger: entering manual mode (auto control disabled)\n");
			manual_mode = true;
			del_timer_sync(&fade_timer);
			del_timer_sync(&kbd_timer);
		}

		/* applying brightness scale */
		{
			u8 scaled = user_brightness_scale;
			if (scaled == 0)
				scaled = 1;

			led_trigger_event(kbd_trigger, scaled);
			led_on = true;
			fade_level = scaled;

			pr_info("kbd-trigger: manual mode ON (brightness=%u)\n", scaled);
		}
	}

	return count;
}

static struct kobj_attribute manual_mode_attr =
	__ATTR(manual_mode, 0664, manual_mode_show, manual_mode_store);

/* --------------------------------------------------------------------- */
/* Sysfs: fade_steps                                                     */
/* --------------------------------------------------------------------- */

static ssize_t fade_steps_show(struct kobject *kobj,
			       struct kobj_attribute *attr, char *buf)
{
	return scnprintf(buf, PAGE_SIZE, "%u\n", user_fade_steps);
}

static ssize_t fade_steps_store(struct kobject *kobj,
				struct kobj_attribute *attr,
				const char *buf, size_t count)
{
	unsigned int v;
	int ret = kstrtouint(buf, 0, &v);
	if (ret)
		return ret;
	if (v < 1)
		v = 1;
	if (v > 100)
		v = 100;

	user_fade_steps = v;
	pr_info("kbd-trigger: fade_steps set to %u\n", v);
	return count;
}

static struct kobj_attribute fade_steps_attr =
	__ATTR(fade_steps, 0664, fade_steps_show, fade_steps_store);

/* --------------------------------------------------------------------- */
/* Sysfs: fade_interval_ms                                               */
/* --------------------------------------------------------------------- */

static ssize_t fade_interval_show(struct kobject *kobj,
				  struct kobj_attribute *attr, char *buf)
{
	return scnprintf(buf, PAGE_SIZE, "%u\n", user_fade_interval_ms);
}

static ssize_t fade_interval_store(struct kobject *kobj,
				   struct kobj_attribute *attr,
				   const char *buf, size_t count)
{
	unsigned int v;
	int ret = kstrtouint(buf, 0, &v);
	if (ret)
		return ret;
	if (v < 1)
		v = 1;
	if (v > 1000)
		v = 1000;

	user_fade_interval_ms = v;
	pr_info("kbd-trigger: fade_interval_ms set to %u\n", v);
	return count;
}

static struct kobj_attribute fade_interval_attr =
	__ATTR(fade_interval_ms, 0664, fade_interval_show, fade_interval_store);

/* --------------------------------------------------------------------- */
/* Sysfs: kbd_timeout                                                    */
/* --------------------------------------------------------------------- */

static ssize_t kbd_timeout_show(struct kobject *kobj,
				struct kobj_attribute *attr, char *buf)
{
	return scnprintf(buf, PAGE_SIZE, "%u\n", user_timeout_sec);
}

static ssize_t kbd_timeout_store(struct kobject *kobj,
				 struct kobj_attribute *attr,
				 const char *buf, size_t count)
{
	unsigned int v;
	int ret = kstrtouint(buf, 0, &v);
	if (ret)
		return ret;

	if (v < 1)
		v = 1;        /* we don't want 0 */
	if (v > 600)
		v = 600;      /* prevent an 'infinite' timeout - safeguard to user */

	user_timeout_sec = v;
	pr_info("kbd-trigger: timeout set to %u seconds\n", v);
	return count;
}

static struct kobj_attribute kbd_timeout_attr =
	__ATTR(kbd_timeout, 0664, kbd_timeout_show, kbd_timeout_store);

/* --------------------------------------------------------------------- */
/* Sysfs: kbd_scale                                                      */
/* --------------------------------------------------------------------- */

static DEFINE_MUTEX(kbd_scale_lock);

static ssize_t kbd_scale_show(struct kobject *kobj,
			      struct kobj_attribute *attr, char *buf)
{
	return scnprintf(buf, PAGE_SIZE, "%u\n", user_brightness_scale);
}

static ssize_t kbd_scale_store(struct kobject *kobj,
			       struct kobj_attribute *attr,
			       const char *buf, size_t count)
{
	unsigned int v;
	int ret;

	ret = kstrtouint(buf, 0, &v);
	if (ret)
		return ret;
	if (v > 255)
		v = 255;

	user_brightness_scale = (u8)v;
	pr_info("kbd-trigger: brightness scale set to %u\n", v);

	if (!is_display_on())
		return count;

	if (!mutex_trylock(&kbd_scale_lock))
		return count;

	/* no change if there's already a fade in/out in progress */
	if (fade_dir != 0 || kbd_update_in_progress) {
    	//pr_info("kbd-trigger: scale updated silently (fade in progress)\n");
    	mutex_unlock(&kbd_scale_lock);
    	return count;
	}

	/* action feedback */
    led_trigger_event(kbd_trigger, user_brightness_scale);

	if (timer_pending(&kbd_feedback_timer)) {
		mod_timer(&kbd_feedback_timer, jiffies + msecs_to_jiffies(300));
	} else {
		mod_timer(&kbd_feedback_timer, jiffies + msecs_to_jiffies(300));
		kbd_feedback_in_progress = true;
	}

	mod_timer(&kbd_timer, jiffies + user_timeout_sec * HZ);
    mutex_unlock(&kbd_scale_lock);
    return count;
}

static void kbd_feedback_timeout(struct timer_list *t)
{
	led_trigger_event(kbd_trigger, LED_OFF);
	led_on = false;
	kbd_feedback_in_progress = false;
}

static struct kobj_attribute kbd_scale_attr =
	__ATTR(kbd_scale, 0664, kbd_scale_show, kbd_scale_store);

/* --------------------------------------------------------------------- */
/* Sysfs: work_mode                                                      */
/* --------------------------------------------------------------------- */

static ssize_t work_mode_show(struct kobject *kobj,
			      struct kobj_attribute *attr, char *buf)
{
	return scnprintf(buf, PAGE_SIZE, "%u\n", work_mode);
}

static ssize_t work_mode_store(struct kobject *kobj,
			       struct kobj_attribute *attr,
			       const char *buf, size_t count)
{
	unsigned int v;
	int ret = kstrtouint(buf, 0, &v);
	if (ret)
		return ret;
	if (v > 1)
		v = 1;

	work_mode = v;
	pr_info("kbd-trigger: work_mode set to %u (%s)\n",
		work_mode, work_mode ? "touch-sensitive" : "keypress");
	return count;
}

static struct kobj_attribute work_mode_attr =
	__ATTR(work_mode, 0664, work_mode_show, work_mode_store);

/* --------------------------------------------------------------------- */
/* API : set scale (export)                    		                     */
/* --------------------------------------------------------------------- */

void kbd_trigger_set_brightness_scale(u8 value)
{
	if (kbd_update_in_progress)
		return;

	if (value > 255)
		value = 255;

	user_brightness_scale = value;
}
EXPORT_SYMBOL_GPL(kbd_trigger_set_brightness_scale);

/* --------------------------------------------------------------------- */
/* Detection				                                             */
/* --------------------------------------------------------------------- */

static bool has_typical_keyboard_keys(struct input_dev *dev)
{
	static const unsigned short required_keys[] = {
		KEY_A, KEY_Z, KEY_Q, KEY_W, KEY_E, KEY_R, KEY_T,
		KEY_Y, KEY_U, KEY_I, KEY_O, KEY_P,
		KEY_S, KEY_D, KEY_F, KEY_G, KEY_H, KEY_J, KEY_K, KEY_L,
		KEY_X, KEY_C, KEY_V, KEY_B, KEY_N, KEY_M,
		KEY_SPACE, KEY_ENTER, KEY_BACKSPACE
	};
	int i;

	/* physical kb */
#ifdef INPUT_PROP_DIRECT
	if (test_bit(INPUT_PROP_DIRECT, dev->propbit))
		return false;
#endif

	for (i = 0; i < ARRAY_SIZE(required_keys); i++) {
		if (!test_bit(required_keys[i], dev->keybit))
			return false;
	}
	return true;
}

static bool is_touch_keypad_dev(const struct input_dev *dev)
{
    /* Oonly the touch keypad - we don't want the screen */
    if (dev->name && strcmp(dev->name, "touch_keypad") == 0)
        return true;
    return false;
}

/* --------------------------------------------------------------------- */
/* Fade step                                                             */
/* --------------------------------------------------------------------- */

static void kbd_fade_step(unsigned long data)
{
	u8 scaled_value;

	if (!fade_dir)
		return;

	fade_level += fade_dir * (255 / user_fade_steps);

	if (fade_level >= 255) {
		fade_level = 255;
		fade_dir = 0;
	} else if (fade_level <= 0) {
		fade_level = 0;
		fade_dir = 0;
	}

	if (kbd_update_in_progress)
		return;

	kbd_update_in_progress = true;
	scaled_value = (fade_level * user_brightness_scale) / 255;
	led_trigger_event(kbd_trigger, scaled_value);
	kbd_update_in_progress = false;

	if (fade_dir)
		mod_timer(&fade_timer, jiffies + msecs_to_jiffies(user_fade_interval_ms));
}

/* --------------------------------------------------------------------- */
/* Timeout                                               */
/* --------------------------------------------------------------------- */

static void kbd_led_timeout(unsigned long data)
{
	/* On screen off - immediate fadeout */
	if (!is_display_on()) {
		fade_level = 0;
		fade_dir = 0;
		led_trigger_event(kbd_trigger, LED_OFF);
		led_on = false;
		return;
	}

	fade_dir = -1;
	mod_timer(&fade_timer, jiffies + 1);
	led_on = false;
}

/* --------------------------------------------------------------------- */
/* Kb events		                                                     */
/* --------------------------------------------------------------------- */
static int kbd_input_event(struct input_handle *handle,
			   unsigned int type, unsigned int code, int value)
{
	if (no_backlight)
		return;
	
		struct kbd_client_ctx *ctx = handle ? handle->private : NULL;
	static unsigned long last_touch_jiffies;
	unsigned long next_timeout;

	if (!ctx)
		return 0;

	pr_debug("kbd-trig: dev=%s type=%u code=%u val=%d touch=%d mode=%u\n",
		handle->dev->name,
		type, code, value,
		ctx ? ctx->is_touch_keypad : -1,
		work_mode);

	/* on manual mode (manual bkl control) we avoid any action */
	if (manual_mode)
		return 0;

	/* touchpad event */
    if (ctx->is_touch_keypad) {
        bool touch_start = false;

        if (work_mode != 1)
            return 0;

        /* Case 1 -> key */
        if (type == EV_KEY && code == BTN_TOUCH && value == 1) {
            touch_start = true;
        }

        /* Case 2 -> tap on capacitive kb */
        if (type == EV_ABS && code == ABS_MT_TRACKING_ID) {
            if (value >= 0 && ctx->last_tracking_id < 0)
                touch_start = true;
            ctx->last_tracking_id = value;
        }

        if (!touch_start)
            return 0;

        pr_info("kbd-trig: TOUCH START (mode=%u)\n", work_mode);

        if (!is_display_on()) {
            if (led_on) {
                led_trigger_event(kbd_trigger, LED_OFF);
                led_on = false;
            }
            return 0;
        }

        if (!led_on) {
            fade_dir = +1;
            mod_timer(&fade_timer, jiffies + 1);
            led_on = true;
        }

        mod_timer(&kbd_timer, jiffies + user_timeout_sec * HZ);
        return 0;
    } else {
		if (work_mode != 0)
			return 0;

		if (!(type == EV_KEY && value))
			return 0;

		if (code >= BTN_MISC)
			return 0;
	}

	if (!is_display_on()) {
		if (led_on) {
			led_trigger_event(kbd_trigger, LED_OFF);
			led_on = false;
		}
		return 0;
	}

	if (!led_on) {
		fade_dir = +1;
		mod_timer(&fade_timer, jiffies + 1);
		led_on = true;
		pr_info("kbd-trig: fade in started\n");
	}

	/* extinction timeout start */
	next_timeout = jiffies + user_timeout_sec * HZ;
	mod_timer(&kbd_timer, next_timeout);

    if (ctx->is_touch_keypad && type == EV_ABS && code == ABS_MT_TRACKING_ID && value < 0) {
    ctx->last_tracking_id = -1;
}
	return 0;
}

/* --------------------------------------------------------------------- */
/* Connexion						                                     */
/* --------------------------------------------------------------------- */

static int kbd_input_connect(struct input_handler *handler,
			     struct input_dev *dev,
			     const struct input_device_id *id)
{
	struct input_handle *handle;
	struct kbd_client_ctx *ctx;
	int error;

	/* connect to both kb + touch keypad */
	if (!has_typical_keyboard_keys(dev) && !is_touch_keypad_dev(dev))
		return -ENODEV;

	handle = kzalloc(sizeof(*handle), GFP_KERNEL);
	if (!handle)
		return -ENOMEM;

	ctx = kzalloc(sizeof(*ctx), GFP_KERNEL);
	if (!ctx) {
		kfree(handle);
		return -ENOMEM;
	}

	ctx->is_touch_keypad = is_touch_keypad_dev(dev);
    ctx->last_tracking_id = -1;

	handle->dev = dev;
	handle->handler = handler;
	handle->name = "kbd_ledtrig";
	handle->private = ctx;

	error = input_register_handle(handle);
	if (error)
		goto err_free;

	error = input_open_device(handle);
	if (error)
		goto err_unregister;

	pr_info("ledtrig-kbd: attached to input '%s'%s\n",
		dev->name ? dev->name : "?",
		ctx->is_touch_keypad ? " (touch_keypad)" : " (physical)");
	return 0;

err_unregister:
	input_unregister_handle(handle);
err_free:
	kfree(ctx);
	kfree(handle);
	return error;
}

static void kbd_input_disconnect(struct input_handle *handle)
{
	if (handle && handle->private)
		kfree(handle->private);
	input_close_device(handle);
	input_unregister_handle(handle);
	kfree(handle);
}

/* --------------------------------------------------------------------- */
/* Devices													             */
/* --------------------------------------------------------------------- */

static const struct input_device_id kbd_ids[] = {
	/* devices with EV_KEY events (kb) */
	{
		.flags = INPUT_DEVICE_ID_MATCH_EVBIT,
		.evbit = { BIT_MASK(EV_KEY) },
	},
	/* devices with EV_ABS tactile events (touchpad) */
	{
		.flags = INPUT_DEVICE_ID_MATCH_EVBIT,
		.evbit = { BIT_MASK(EV_ABS) },
	},
	{ },
};

/* --------------------------------------------------------------------- */
/* public API									                         */
/* --------------------------------------------------------------------- */

void kbd_trigger_display_off(void)
{
	if (no_backlight)
		return;

	pr_info("kbd-trigger: display off (notified by mdss driver)\n");

	if (!led_on)
		return;

	manual_mode = false;

	/* Immediate stop */
	del_timer_sync(&kbd_timer);
	del_timer_sync(&fade_timer);
	del_timer_sync(&kbd_feedback_timer);

	/* nothing to do here */
	if (!led_on || fade_level == 0) {
		led_trigger_event(kbd_trigger, LED_OFF);
		led_on = false;
		fade_level = 0;
		fade_dir = 0;
		return;
	}

	/* dafe-out */
	fade_dir = -1;
	led_on = false;
	mod_timer(&fade_timer, jiffies + 1);

	pr_info("kbd-trigger: display turned off → fading out keyboard backlight\n");
}
EXPORT_SYMBOL_GPL(kbd_trigger_display_off);

void kbd_trigger_manual_off(void)
{
	pr_info("kbd-trigger: manual off (notified by userspace)\n");

	if (!led_on)
		return;

	/* immediate reinit */
	del_timer_sync(&kbd_timer);
	del_timer_sync(&fade_timer);

	fade_level = 0;
	fade_dir = 0;
	led_on = false;
	kbd_update_in_progress = false;
}
EXPORT_SYMBOL_GPL(kbd_trigger_manual_off);

void kbd_trigger_display_on(void)
{
	
}
EXPORT_SYMBOL_GPL(kbd_trigger_display_on);

/* --------------------------------------------------------------------- */
/* Init / Exit                                                           */
/* --------------------------------------------------------------------- */

static int __init kbd_ledtrig_init(void)
{
	int ret, err;

	led_trigger_register_simple("kbd-trigger", &kbd_trigger);

	setup_timer(&kbd_timer, kbd_led_timeout, 0);
	setup_timer(&fade_timer, kbd_fade_step, 0);
	setup_timer(&kbd_feedback_timer, kbd_feedback_timeout, 0);

	fade_level = 0;
	fade_dir = 0;
	led_on = false;

	kbd_input_handler.event      = kbd_input_event;
	kbd_input_handler.connect    = kbd_input_connect;
	kbd_input_handler.disconnect = kbd_input_disconnect;
	kbd_input_handler.name       = "kbd_ledtrig_handler";
	kbd_input_handler.id_table   = kbd_ids;

	ret = input_register_handler(&kbd_input_handler);
	if (ret)
		led_trigger_unregister_simple(kbd_trigger);

	pr_info("ledtrig-kbd: registered trigger 'kbd-trigger' (timeout=%us, fade_steps=%u, fade_interval=%ums)\n",
		user_timeout_sec, user_fade_steps, user_fade_interval_ms);

	kbd_kobj = kobject_create_and_add("kbd_trigger", kernel_kobj);
	if (!kbd_kobj) {
		pr_err("ledtrig-kbd: failed to create kbd_trigger sysfs node\n");
		return ret ? ret : -ENOMEM;
	}

	err = sysfs_create_file(kbd_kobj, &kbd_scale_attr.attr);
	if (err) pr_err("failed to create kbd_scale\n");

	err = sysfs_create_file(kbd_kobj, &kbd_timeout_attr.attr);
	if (err) pr_err("failed to create kbd_timeout\n");

	err = sysfs_create_file(kbd_kobj, &fade_steps_attr.attr);
	if (err) pr_err("failed to create fade_steps\n");

	err = sysfs_create_file(kbd_kobj, &fade_interval_attr.attr);
	if (err) pr_err("failed to create fade_interval_ms\n");

	err = sysfs_create_file(kbd_kobj, &manual_mode_attr.attr);
	if (err) pr_err("failed to create manual_mode\n");

	err = sysfs_create_file(kbd_kobj, &work_mode_attr.attr);
	if (err) pr_err("failed to create work_mode\n");

	err = sysfs_create_file(kbd_kobj, &no_bkl_attr.attr);
	if (err) pr_err("failed to create no_bkl\n");

	return ret;
}

static void __exit kbd_ledtrig_exit(void)
{
	del_timer_sync(&fade_timer);
	del_timer_sync(&kbd_timer);
	del_timer_sync(&kbd_feedback_timer);
	input_unregister_handler(&kbd_input_handler);
	led_trigger_unregister_simple(kbd_trigger);

	if (kbd_kobj) {
		sysfs_remove_file(kbd_kobj, &kbd_scale_attr.attr);
		sysfs_remove_file(kbd_kobj, &kbd_timeout_attr.attr);
		sysfs_remove_file(kbd_kobj, &fade_steps_attr.attr);
		sysfs_remove_file(kbd_kobj, &fade_interval_attr.attr);
		sysfs_remove_file(kbd_kobj, &manual_mode_attr.attr);
		sysfs_remove_file(kbd_kobj, &work_mode_attr.attr);
		sysfs_remove_file(kbd_kobj, &no_bkl_attr.attr);
		kobject_put(kbd_kobj);
	}
}

module_init(kbd_ledtrig_init);
module_exit(kbd_ledtrig_exit);

MODULE_AUTHOR("guizmox");
MODULE_DESCRIPTION("LED trigger for keyboard activity (physical & capacitive), with fade, timeout, display state");
MODULE_LICENSE("GPL");
