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


/*	from OpenSolaris "hashmake.c	1.11	05/06/08 SMI"	 SVr4.0 1.2		*/

/*
 * Portions Copyright (c) 2005 Gunnar Ritter, Freiburg i. Br., Germany
 *
 * Sccsid @(#)hashmake.c	2.3 (gritter) 6/22/05
 */

#if __GNUC__ >= 3 && __GNUC_MINOR__ >= 4 || __GNUC__ >= 4
#define	USED	__attribute__ ((used))
#elif defined __GNUC__
#define	USED	__attribute__ ((unused))
#else
#define	USED
#endif
static const char sccsid[] USED = "@(#)hashmake.c	2.3 (gritter) 6/22/05";

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <locale.h>
#include <limits.h>
#include <string.h>

#include "hash.h"

/* ARGSUSED */
int
main(int argc, char **argv)
{
	char word[LINE_MAX];
	int n;

	hashinit();
	while (fgets(word, sizeof word, stdin)) {
		n = strlen(word);
		if (word[n-1] == '\n')
			word[n-1] = '\0';
		printf("%.*lo\n", (HASHWIDTH+2)/3, (long)hash(word));
	}
	return 0;
}
