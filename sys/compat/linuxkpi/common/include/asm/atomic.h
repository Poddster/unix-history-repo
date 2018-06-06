/*-
 * Copyright (c) 2010 Isilon Systems, Inc.
 * Copyright (c) 2010 iX Systems, Inc.
 * Copyright (c) 2010 Panasas, Inc.
 * Copyright (c) 2013-2017 Mellanox Technologies, Ltd.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice unmodified, this list of conditions, and the following
 *    disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * $FreeBSD$
 */

#ifndef _ASM_ATOMIC_H_
#define	_ASM_ATOMIC_H_

#include <linux/compiler.h>
#include <sys/types.h>
#include <machine/atomic.h>

#define	ATOMIC_INIT(x)	{ .counter = (x) }

typedef struct {
	volatile int counter;
} atomic_t;

/*------------------------------------------------------------------------*
 *	32-bit atomic operations
 *------------------------------------------------------------------------*/

#define	atomic_add(i, v)		atomic_add_return((i), (v))
#define	atomic_sub(i, v)		atomic_sub_return((i), (v))
#define	atomic_inc_return(v)		atomic_add_return(1, (v))
#define	atomic_add_negative(i, v)	(atomic_add_return((i), (v)) < 0)
#define	atomic_add_and_test(i, v)	(atomic_add_return((i), (v)) == 0)
#define	atomic_sub_and_test(i, v)	(atomic_sub_return((i), (v)) == 0)
#define	atomic_dec_and_test(v)		(atomic_sub_return(1, (v)) == 0)
#define	atomic_inc_and_test(v)		(atomic_add_return(1, (v)) == 0)
#define	atomic_dec_return(v)		atomic_sub_return(1, (v))
#define	atomic_inc_not_zero(v)		atomic_add_unless((v), 1, 0)

static inline int
atomic_add_return(int i, atomic_t *v)
{
	return i + atomic_fetchadd_int(&v->counter, i);
}

static inline int
atomic_sub_return(int i, atomic_t *v)
{
	return atomic_fetchadd_int(&v->counter, -i) - i;
}

static inline void
atomic_set(atomic_t *v, int i)
{
	WRITE_ONCE(v->counter, i);
}

static inline void
atomic_set_release(atomic_t *v, int i)
{
	atomic_store_rel_int(&v->counter, i);
}

static inline void
atomic_set_mask(unsigned int mask, atomic_t *v)
{
	atomic_set_int(&v->counter, mask);
}

static inline int
atomic_read(const atomic_t *v)
{
	return READ_ONCE(v->counter);
}

static inline int
atomic_inc(atomic_t *v)
{
	return atomic_fetchadd_int(&v->counter, 1) + 1;
}

static inline int
atomic_dec(atomic_t *v)
{
	return atomic_fetchadd_int(&v->counter, -1) - 1;
}

static inline int
atomic_add_unless(atomic_t *v, int a, int u)
{
	int c;

	for (;;) {
		c = atomic_read(v);
		if (unlikely(c == u))
			break;
		if (likely(atomic_cmpset_int(&v->counter, c, c + a)))
			break;
	}
	return (c != u);
}

static inline void
atomic_clear_mask(unsigned int mask, atomic_t *v)
{
	atomic_clear_int(&v->counter, mask);
}

static inline int
atomic_xchg(atomic_t *v, int i)
{
#if defined(__i386__) || defined(__amd64__) || \
    defined(__arm__) || defined(__aarch64__) || \
    defined(__powerpc__)
	return (atomic_swap_int(&v->counter, i));
#else
	int ret;
	for (;;) {
		ret = READ_ONCE(v->counter);
		if (atomic_cmpset_int(&v->counter, ret, i))
			break;
	}
	return (ret);
#endif
}

static inline int
atomic_cmpxchg(atomic_t *v, int old, int new)
{
	int ret = old;

	for (;;) {
		if (atomic_cmpset_int(&v->counter, old, new))
			break;
		ret = READ_ONCE(v->counter);
		if (ret != old)
			break;
	}
	return (ret);
}

#define	cmpxchg(ptr, old, new) ({					\
	union {								\
		__typeof(*(ptr)) val;					\
		u8 u8[0];						\
		u16 u16[0];						\
		u32 u32[0];						\
		u64 u64[0];						\
	} __ret = { .val = (old) }, __new = { .val = (new) };		\
									\
	CTASSERT(sizeof(__ret.val) == 1 || sizeof(__ret.val) == 2 ||	\
	    sizeof(__ret.val) == 4 || sizeof(__ret.val) == 8);		\
									\
	switch (sizeof(__ret.val)) {					\
	case 1:								\
		while (!atomic_fcmpset_8((volatile u8 *)(ptr),		\
		    __ret.u8, __new.u8[0]) && __ret.val == (old))	\
			;						\
		break;							\
	case 2:								\
		while (!atomic_fcmpset_16((volatile u16 *)(ptr),	\
		    __ret.u16, __new.u16[0]) && __ret.val == (old))	\
			;						\
		break;							\
	case 4:								\
		while (!atomic_fcmpset_32((volatile u32 *)(ptr),	\
		    __ret.u32, __new.u32[0]) && __ret.val == (old))	\
			;						\
		break;							\
	case 8:								\
		while (!atomic_fcmpset_64((volatile u64 *)(ptr),	\
		    __ret.u64, __new.u64[0]) && __ret.val == (old))	\
			;						\
		break;							\
	}								\
	__ret.val;							\
})

#define	cmpxchg_relaxed(...)	cmpxchg(__VA_ARGS__)

#define	xchg(ptr, new) ({						\
	union {								\
		__typeof(*(ptr)) val;					\
		u8 u8[0];						\
		u16 u16[0];						\
		u32 u32[0];						\
		u64 u64[0];						\
	} __ret, __new = { .val = (new) };				\
									\
	CTASSERT(sizeof(__ret.val) == 1 || sizeof(__ret.val) == 2 ||	\
	    sizeof(__ret.val) == 4 || sizeof(__ret.val) == 8);		\
									\
	switch (sizeof(__ret.val)) {					\
	case 1:								\
		__ret.val = READ_ONCE(*ptr);				\
		while (!atomic_fcmpset_8((volatile u8 *)(ptr),		\
	            __ret.u8, __new.u8[0]))				\
			;						\
		break;							\
	case 2:								\
		__ret.val = READ_ONCE(*ptr);				\
		while (!atomic_fcmpset_16((volatile u16 *)(ptr),	\
	            __ret.u16, __new.u16[0]))				\
			;						\
		break;							\
	case 4:								\
		__ret.u32[0] = atomic_swap_32((volatile u32 *)(ptr),	\
		    __new.u32[0]);					\
		break;							\
	case 8:								\
		__ret.u64[0] = atomic_swap_64((volatile u64 *)(ptr),	\
		    __new.u64[0]);					\
		break;							\
	}								\
	__ret.val;							\
})

static inline int
atomic_dec_if_positive(atomic_t *v)
{
	int retval;
	int curr;

	do {
		curr = atomic_read(v);
		retval = curr - 1;
		if (unlikely(retval < 0))
			break;
	} while (!likely(atomic_cmpset_int(&v->counter, curr, retval)));

	return (retval);
}

#define	LINUX_ATOMIC_OP(op, c_op)				\
static inline void atomic_##op(int i, atomic_t *v)		\
{								\
	int c, old;						\
								\
	c = v->counter;						\
	while ((old = atomic_cmpxchg(v, c, c c_op i)) != c)	\
		c = old;					\
}

#define	LINUX_ATOMIC_FETCH_OP(op, c_op)				\
static inline int atomic_fetch_##op(int i, atomic_t *v)		\
{								\
	int c, old;						\
								\
	c = v->counter;						\
	while ((old = atomic_cmpxchg(v, c, c c_op i)) != c)	\
		c = old;					\
								\
	return (c);						\
}

LINUX_ATOMIC_OP(or, |)
LINUX_ATOMIC_OP(and, &)
LINUX_ATOMIC_OP(andnot, &~)
LINUX_ATOMIC_OP(xor, ^)

LINUX_ATOMIC_FETCH_OP(or, |)
LINUX_ATOMIC_FETCH_OP(and, &)
LINUX_ATOMIC_FETCH_OP(andnot, &~)
LINUX_ATOMIC_FETCH_OP(xor, ^)

#endif					/* _ASM_ATOMIC_H_ */
