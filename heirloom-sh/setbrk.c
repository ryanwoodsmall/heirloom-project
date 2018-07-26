/*
 * CDDL HEADER START
 *
 * The contents of this file are subject to the terms of the
 * Common Development and Distribution License, Version 1.0 only
 * (the "License").  You may not use this file except in compliance
 * with the License.
 *
 * You can obtain a copy of the license at usr/src/OPENSOLARIS.LICENSE
 * or http://www.opensolaris.org/os/licensing.
 * See the License for the specific language governing permissions
 * and limitations under the License.
 *
 * When distributing Covered Code, include this CDDL HEADER in each
 * file and include the License file at usr/src/OPENSOLARIS.LICENSE.
 * If applicable, add the following below this CDDL HEADER, with the
 * fields enclosed by brackets "[]" replaced with your own identifying
 * information: Portions Copyright [yyyy] [name of copyright owner]
 *
 * CDDL HEADER END
 */
/*	Copyright (c) 1984, 1986, 1987, 1988, 1989 AT&T	*/
/*	  All Rights Reserved  	*/


/*
 * Copyright (c) 1996, by Sun Microsystems, Inc.
 * All rights reserved.
 */

/*
 * Portions Copyright (c) 2005 Gunnar Ritter, Freiburg i. Br., Germany
 *
 * Sccsid @(#)setbrk.c	1.4 (gritter) 6/15/05
 */
/* from OpenSolaris "setbrk.c	1.10	05/06/08 SMI"	 SVr4.0 1.8.1.1 */
/*
 *	UNIX shell
 */

#include	"defs.h"

/*
 * fudge a version of sbrk() from old musl code
 *   http://git.musl-libc.org/cgit/musl/commit/src/linux/sbrk.c?id=7a995fe706e519a4f55399776ef0df9596101f93
 */
#include	<sys/syscall.h>
#include	<errno.h>

void *muslsbrk(intptr_t inc)
{
	unsigned long cur = syscall(SYS_brk, 0);
	if (inc && syscall(SYS_brk, cur+inc) != cur+inc) {
		errno = ENOMEM;
		return (void *)-1;
	}
	return (void *)cur;
}

unsigned char *
setbrk(int incr)
{

	// XXX - using sbrk as a growable stack (i think)
	// XXX - won't work on musl or with straight malloc replacement?
	register unsigned char *a = (unsigned char *)muslsbrk(incr);

	brkend = a + incr;
	return(a);
}
