#ifndef _XT_SOCKET_H
#define _XT_SOCKET_H

#include <linux/types.h>

enum {
	XT_SOCKET_TRANSPARENT = 1 << 0,
<<<<<<< HEAD
=======
	XT_SOCKET_NOWILDCARD = 1 << 1,
>>>>>>> android-3.18
};

struct xt_socket_mtinfo1 {
	__u8 flags;
};
<<<<<<< HEAD

void xt_socket_put_sk(struct sock *sk);
=======
#define XT_SOCKET_FLAGS_V1 XT_SOCKET_TRANSPARENT

struct xt_socket_mtinfo2 {
	__u8 flags;
};
#define XT_SOCKET_FLAGS_V2 (XT_SOCKET_TRANSPARENT | XT_SOCKET_NOWILDCARD)

>>>>>>> android-3.18
struct sock *xt_socket_get4_sk(const struct sk_buff *skb,
			       struct xt_action_param *par);
struct sock *xt_socket_get6_sk(const struct sk_buff *skb,
			       struct xt_action_param *par);

#endif /* _XT_SOCKET_H */
