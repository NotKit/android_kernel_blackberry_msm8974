/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LEDS_TRIGGER_KBD_H
#define _LEDS_TRIGGER_KBD_H

#ifdef CONFIG_LEDS_TRIGGER_KBD
void kbd_trigger_display_off(void);
void kbd_trigger_display_on(void);
void kbd_trigger_manual_off(void);
void kbd_trigger_set_brightness_scale(u8 value);
#else
static inline void kbd_trigger_display_off(void) {}
static inline void kbd_trigger_display_on(void) {}
static inline void kbd_trigger_manual_off(void) {}
static inline void kbd_trigger_set_brightness_scale(u8 value) {}
#endif

#endif /* _LEDS_TRIGGER_KBD_H */
