#ifndef _MQUEUE_H_
#define _MQUEUE_H_

/* mqueue.h: POSIX.4 Message Queues */

/*-
 * Copyright (c) 1996, 1997
 *	HD Associates, Inc.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by HD Associates, Inc
 * 4. Neither the name of the author nor the names of any co-contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY HD ASSOCIATES AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL HD ASSOCIATES OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

#ifdef _POSIX4_INCLUDE_MAYBES
#include <sys/types.h>
#include <fcntl.h>
#include <time.h>
#include <signal.h>
#else
struct sigevent;
#endif

typedef int mqd_t;		/* message queue descriptors */

struct mq_attr {
	long mq_flags;		/* message queue flags */
	long mq_maxmsg;		/* maximum number of messages */
	long mq_msgsize;	/* maximum message size */
	long mq_curmsgs;	/* number of messages currently queued */
};

#ifndef KERNEL

#include <sys/cdefs.h>

__BEGIN_DECLS
mqd_t mq_open __P((const char *, int oflag, ...));
int mq_close __P((mqd_t));
int mq_unlink __P((const char *));
int mq_send __P((mqd_t, const char *, size_t, unsigned int));
ssize_t mq_receive __P((mqd_t, char *, size_t, unsigned int *));
int mq_notify __P((mqd_t, const struct sigevent *));
int mq_setattr __P((mqd_t, const struct mq_attr *, struct mq_attr *));
int mq_getattr __P((mqd_t, struct mq_attr *));
__END_DECLS

#endif /* KERNEL */

#endif /* _MQUEUE_H_ */
