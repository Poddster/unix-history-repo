/*
 * Copyright (c) 2000-2001 Sendmail, Inc. and its suppliers.
 *	All rights reserved.
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the sendmail distribution.
 *
 *	$Id: sm_os_next.h,v 1.1.1.1 2002/02/17 21:56:43 gshapiro Exp $
 */

/*
**  Platform definitions for NeXT
*/

#define SM_OS_NAME	"next"

#define SM_CONF_SIGSETJMP	0
#define SM_CONF_SSIZE_T		0
#define SM_CONF_FORMAT_TEST	0

/* doesn't seem to work on NeXT 3.x */
#define SM_DEAD(proto) proto
#define SM_UNUSED(decl) decl

/* try LLONG tests in libsm/t-types.c? */
#ifndef SM_CONF_TEST_LLONG
# define SM_CONF_TEST_LLONG	0
#endif /* !SM_CONF_TEST_LLONG */
