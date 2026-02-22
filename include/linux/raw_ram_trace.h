/*
 * raw_ram_trace.h - Lock-free direct-to-RAM tracing
 *
 * Writes directly to the persistent RAM (ram_console) buffer using
 * atomic operations only. Safe to call from ANY context including:
 * - Inside raw_spin_lock sections
 * - Inside IRQ handlers
 * - With IRQs disabled
 * - NMI context
 *
 * Bypasses printk, console framework, and all locks.
 */

#ifndef _LINUX_RAW_RAM_TRACE_H
#define _LINUX_RAW_RAM_TRACE_H

#ifdef CONFIG_ANDROID_RAM_CONSOLE

/*
 * raw_ram_trace - Write a string directly to the ram_console buffer.
 * @s: null-terminated string to write
 *
 * Lock-free, NMI-safe. Uses atomic_cmpxchg to advance the buffer pointer.
 * No ECC update (trading reliability for safety in hard contexts).
 */
void notrace raw_ram_trace(const char *s);

/*
 * raw_ram_trace_fmt - Formatted version (limited to 256 chars).
 * Uses a per-CPU buffer to avoid stack overflow in IRQ context.
 * Still lock-free for the actual RAM write.
 */
void notrace raw_ram_trace_fmt(const char *fmt, ...)
	__attribute__((format(printf, 1, 2)));

#else

static inline void raw_ram_trace(const char *s) {}
static inline void raw_ram_trace_fmt(const char *fmt, ...) {}

#endif /* CONFIG_ANDROID_RAM_CONSOLE */

#endif /* _LINUX_RAW_RAM_TRACE_H */
