#ifndef _ASM_GENERIC_IOCTL_H
#define _ASM_GENERIC_IOCTL_H

#include <uapi/asm-generic/ioctl.h>

<<<<<<< HEAD
=======
#ifdef __CHECKER__
#define _IOC_TYPECHECK(t) (sizeof(t))
#else
>>>>>>> android-3.18
/* provoke compile error for invalid uses of size argument */
extern unsigned int __invalid_size_argument_for_IOC;
#define _IOC_TYPECHECK(t) \
	((sizeof(t) == sizeof(t[1]) && \
	  sizeof(t) < (1 << _IOC_SIZEBITS)) ? \
	  sizeof(t) : __invalid_size_argument_for_IOC)
<<<<<<< HEAD
=======
#endif

>>>>>>> android-3.18
#endif /* _ASM_GENERIC_IOCTL_H */
