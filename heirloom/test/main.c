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

/*
 * Copyright (c) 2005 Gunnar Ritter, Freiburg i. Br., Germany
 *
 * Sccsid @(#)main.c	1.2 (gritter) 6/29/05
 */

#include "defs.h"
#include <locale.h>

int	mb_cur_max;
int	ucb_builtins;

int
main(int argc, char **argv)
{
	extern int	func(int, char **);
	extern int	sysv3;
	char	*pp;

	if (sysv3)
		putenv("SYSV3=set");
	setlocale(LC_CTYPE, "");
	mb_cur_max = MB_CUR_MAX;
	if ((pp = getenv("PATH")) != NULL) {
		while (*pp) {
			while (*pp == ':')
				pp++;
			if (strncmp(pp, "/usr/ucb", 8) == 0 &&
					(pp[8] == ':' || pp[8] == '\0')) {
				ucb_builtins = 1;
				break;
			} else if (strncmp(pp, "/bin", 4) == 0 &&
					(pp[4] == ':' || pp[4] == '\0')) {
				ucb_builtins = 0;
				break;
			} else if (strncmp(pp, "/usr/bin", 8) == 0 &&
					(pp[8] == ':' || pp[8] == '\0')) {
				ucb_builtins = 0;
				break;
			} else if (strncmp(pp, "/usr/5bin", 9) == 0 &&
					(pp[9] == ':' || pp[9] == '\0')) {
				ucb_builtins = 0;
				break;
			}
			while (*pp && *pp != ':')
				pp++;
		}
	}
	return func(argc, argv);
}

struct namnod *
findnam(const char *name)
{
	static struct namnod	n;
	char	*cp;

	if ((cp = getenv(name)) != NULL) {
		n.value = cp;
		n.namflg = N_EXPORT|N_ENVNAM;
		return &n;
	}
	return NULL;
}
