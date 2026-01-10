#ifndef _ASM_SOCKET_H
#define _ASM_SOCKET_H

#include <uapi/asm/socket.h>

#define SO_MAX_PACING_RATE	44

#ifdef __KERNEL__
/* O_NONBLOCK clashes with the bits used for socket types.  Therefore we
 * have to define SOCK_NONBLOCK to a different value here.
 */
#define SOCK_NONBLOCK	0x40000000
<<<<<<< HEAD
#endif /* __KERNEL__ */

=======
>>>>>>> android-3.18
#endif /* _ASM_SOCKET_H */
