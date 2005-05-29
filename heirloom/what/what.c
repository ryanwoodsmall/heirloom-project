/*
 * Changes by Gunnar Ritter, Freiburg i. Br., Germany, July 2003.
 */
/*
 * Copyright (c) 1980, 1988, 1993
 *	The Regents of the University of California.  All rights reserved.
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
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/* from what.c	8.1 (Berkeley) 6/6/93 */

#if __GNUC__ >= 3 && __GNUC_MINOR__ >= 4 || __GNUC__ >= 4
#define	USED	__attribute__ ((used))
#elif defined __GNUC__
#define	USED	__attribute__ ((unused))
#else
#define	USED
#endif
static const char sccsid[] USED = "@(#)what.sl	1.8 (gritter) 5/29/05";

#include <stdio.h>
#include <stdlib.h>

#if defined (__GLIBC__)
#if defined (_IO_get_unlocked)
#undef	getchar
#define	getchar()	_IO_getc_unlocked(stdin)
#endif
#if defined (_IO_putc_unlocked)
#undef	putchar
#define	putchar(c)	_IO_putc_unlocked(c, stdout)
#endif
#endif

static void search(void);

/*
 * what
 */
int
main(int argc, char **argv)
{
	int	status = 0;

	if (argc <= 1)
		search();
	else do {
		if (!freopen(argv[1], "r", stdin)) {
			perror(argv[1]);
			status = 1;
			continue;
		}
		printf("%s:\n", argv[1]);
		search();
	} while(++argv, --argc > 1);
	exit(status);
}

static void
search(void)
{
	register int	c;

	while ((c = getchar()) != EOF) {
loop:		if (c != '@')
			continue;
		if ((c = getchar()) != '(')
			goto loop;
		if ((c = getchar()) != '#')
			goto loop;
		if ((c = getchar()) != ')')
			goto loop;
		putchar('\t');
		while ((c = getchar()) != EOF && c && c != '"' &&
		    c != '>' && c != '\n')
			putchar(c);
		putchar('\n');
	}
}
