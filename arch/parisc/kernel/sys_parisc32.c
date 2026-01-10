/*
 * sys_parisc32.c: Conversion between 32bit and 64bit native syscalls.
 *
 * Copyright (C) 2000-2001 Hewlett Packard Company
 * Copyright (C) 2000 John Marvin
 * Copyright (C) 2001 Matthew Wilcox
 * Copyright (C) 2014 Helge Deller <deller@gmx.de>
 *
 * These routines maintain argument size conversion between 32bit and 64bit
 * environment. Based heavily on sys_ia32.c and sys_sparc32.c.
 */

#include <linux/compat.h>
#include <linux/kernel.h>
#include <linux/syscalls.h>

<<<<<<< HEAD
#include <asm/types.h>
#include <asm/uaccess.h>
#include <asm/mmu_context.h>

#include "sys32.h"

#undef DEBUG

#ifdef DEBUG
#define DBG(x)	printk x
#else
#define DBG(x)
#endif

/*
 * sys32_execve() executes a new program.
 */

asmlinkage int sys32_execve(struct pt_regs *regs)
{
	int error;
	struct filename *filename;

	DBG(("sys32_execve(%p) r26 = 0x%lx\n", regs, regs->gr[26]));
	filename = getname((const char __user *) regs->gr[26]);
	error = PTR_ERR(filename);
	if (IS_ERR(filename))
		goto out;
	error = compat_do_execve(filename->name, compat_ptr(regs->gr[25]),
				 compat_ptr(regs->gr[24]), regs);
	putname(filename);
out:

	return error;
}
=======
>>>>>>> android-3.18

asmlinkage long sys32_unimplemented(int r26, int r25, int r24, int r23,
	int r22, int r21, int r20)
{
    printk(KERN_ERR "%s(%d): Unimplemented 32 on 64 syscall #%d!\n", 
    	current->comm, current->pid, r20);
    return -ENOSYS;
}

asmlinkage long sys32_fanotify_mark(compat_int_t fanotify_fd, compat_uint_t flags,
	compat_uint_t mask0, compat_uint_t mask1, compat_int_t dfd,
	const char  __user * pathname)
{
	return sys_fanotify_mark(fanotify_fd, flags,
			((__u64)mask1 << 32) | mask0,
			 dfd, pathname);
}
