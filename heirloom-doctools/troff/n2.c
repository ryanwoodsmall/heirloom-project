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
 * Copyright 1989 Sun Microsystems, Inc.  All rights reserved.
 * Use is subject to license terms.
 */

/*	Copyright (c) 1984, 1986, 1987, 1988, 1989 AT&T	*/
/*	  All Rights Reserved  	*/


/*	from OpenSolaris "n2.c	1.9	05/06/08 SMI"	*/

/*
 * Portions Copyright (c) 2005 Gunnar Ritter, Freiburg i. Br., Germany
 *
 * Sccsid @(#)n2.c	1.6 (gritter) 8/8/05
 */

/*
 * University Copyright- Copyright (c) 1982, 1986, 1988
 * The Regents of the University of California
 * All Rights Reserved
 *
 * University Acknowledgment- Portions of this document are derived from
 * software developed by the University of California, Berkeley, and its
 * contributors.
 */

/*
 * n2.c
 *
 * output, cleanup
 */

#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include "tdef.h"
#ifdef NROFF
#include "tw.h"
#endif
#include "proto.h"
#include <setjmp.h>
#include "ext.h"
#ifdef EUC
#ifdef NROFF
#include <stddef.h>
#ifdef	__sun
#include <widec.h>
#else
#include <wchar.h>
#endif
#include <limits.h>
#include <ctype.h>
#include <unistd.h>

char mbobuf[MB_LEN_MAX] = {0};
wchar_t wchar;
int	nmb1 = 0;
#endif /* NROFF */
#endif /* EUC */

extern	jmp_buf	sjbuf;
int	toolate;
int	error;

void
pchar(register tchar i)
{
	register int j;
	static int hx = 0;	/* records if have seen HX */

	if (hx) {
		hx = 0;
		j = absmot(i);
		if (isnmot(i)) {
			if (j > dip->blss)
				dip->blss = j;
		} else {
			if (j > dip->alss)
				dip->alss = j;
			ralss = dip->alss;
		}
		return;
	}
	if (ismot(i)) {
		pchar1(i); 
		return;
	}
	switch (j = cbits(i)) {
	case 0:
	case IMP:
	case RIGHT:
	case LEFT:
		return;
	case HX:
		hx = 1;
		return;
	case PRESC:
		if (dip == &d[0])
			j = eschar;	/* fall through */
	default:
#ifndef EUC
		setcbits(i, trtab[j]);
#else
#ifndef NROFF
		setcbits(i, trtab[j]);
#else
		if (!multi_locale || (!(j & CSMASK) && !(j & MBMASK1)))
			setcbits(i, trtab[j]);
#endif /* NROFF */
#endif /* EUC */
	}
	pchar1(i);
}


void
pchar1(register tchar i)
{
	register int j;
	extern void ptout(tchar);

	j = cbits(i);
	if (dip != &d[0]) {
		wbf(i);
		dip->op = offset;
		return;
	}
	if (!tflg && !print) {
		if (j == '\n')
			dip->alss = dip->blss = 0;
		return;
	}
	if (no_out || j == FILLER)
		return;
	if (tflg) {	/* transparent mode, undiverted */
		fdprintf(ptid, "%c", j);
		return;
	}
#ifndef NROFF
	if (ascii)
		outascii(i);
	else
#endif
		ptout(i);
}

void
outascii (	/* print i in best-guess ascii */
    tchar i
)
{
	int j = cbits(i);

	if (ismot(i)) {
		oput(' ');
		return;
	}
	if (j < 0177 && j >= ' ' || j == '\n') {
		oput(j);
		return;
	}
	if (j == DRAWFCN)
		oputs("\\D");
	else if (j == HYPHEN || j == MINUS)
		oput('-');
	else if (j == XON)
		oputs("\\X");
	else if (j == LIG_FI)
		oputs("fi");
	else if (j == LIG_FL)
		oputs("fl");
	else if (j == LIG_FF)
		oputs("ff");
	else if (j == LIG_FFI)
		oputs("ffi");
	else if (j == LIG_FFL)
		oputs("ffl");
	else if (j == WORDSP)
		;	/* nothing at all */
	else if (j > 0177) {
		oput('\\');
		oput('(');
		oput(chname[chtab[j-128]]);
		oput(chname[chtab[j-128]+1]);
	}
}


/*
 * now a macro
oput(i)
	register int	i;
{
	*obufp++ = i;
	if (obufp >= &obuf[OBUFSZ])
		flusho();
}
*/

void
oputs(register char *i)
{
	while (*i != 0)
		oput(*i++);
}


void
flusho(void)
{
	if (obufp == obuf)
		return;
	if (no_out == 0) {
		if (!toolate) {
			toolate++;
#ifdef NROFF
			set_tty();
			{
				char	*p = t.twinit;
				while (*p++)
					;
				if (p - t.twinit > 1)
					write(ptid, t.twinit, p - t.twinit - 1);
			}
#endif
		}
		toolate += write(ptid, obuf, obufp - obuf);
	}
	obufp = obuf;
}


void
done(int x)
{
	register int i;

	error |= x;
	app = ds = lgf = 0;
	if (i = em) {
		donef = -1;
		em = 0;
		if (control(i, 0))
			longjmp(sjbuf, 1);
	}
	if (!nfo)
		done3(0);
	mflg = 0;
	dip = &d[0];
	if (woff)
		wbt((tchar)0);
	if (pendw)
		getword(1);
	pendnf = 0;
	if (donef == 1)
		done1(0);
	donef = 1;
	ip = 0;
	frame = stk;
	nxf = frame + 1;
	if (!ejf)
		tbreak();
	nflush++;
	eject((struct s *)0);
	longjmp(sjbuf, 1);
}


void
done1(int x) 
{
	error |= x;
	if (numtab[NL].val) {
		trap = 0;
		eject((struct s *)0);
		longjmp(sjbuf, 1);
	}
	if (nofeed) {
		ptlead();
		flusho();
		done3(0);
	} else {
		if (!gflag)
			pttrailer();
		done2(0);
	}
}


void
done2(int x) 
{
	ptlead();
#ifndef NROFF
	if (!ascii)
		ptstop();
#endif
	flusho();
	done3(x);
}

void
done3(int x)
{
	error |= x;
	signal(SIGINT, SIG_IGN);
	signal(SIGTERM, SIG_IGN);
	unlink(unlkp);
#ifdef NROFF
	twdone();
#endif
	if (ascii)
		mesg(1);
	exit(error);
}


void
edone(int x)
{
	frame = stk;
	nxf = frame + 1;
	ip = 0;
	done(x);
}



void
casepi(void)
{
	register int i;
	int	id[2];

	if (toolate || skip() || !getname() || pipe(id) == -1 || (i = fork()) == -1) {
		errprint("Pipe not created.");
		return;
	}
	ptid = id[1];
	if (i > 0) {
		close(id[0]);
		toolate++;
		pipeflg++;
		return;
	}
	close(0);
	dup(id[0]);
	close(id[1]);
	execl(nextf, nextf, NULL);
	errprint("Cannot exec %s", nextf);
	exit(-4);
}
