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
 * Copyright 2003 Sun Microsystems, Inc.  All rights reserved.
 * Use is subject to license terms.
 */

/*	Copyright (c) 1984, 1986, 1987, 1988, 1989 AT&T	*/
/*	  All Rights Reserved  	*/


/*	from OpenSolaris "n5.c	1.10	05/06/08 SMI"	*/

/*
 * Portions Copyright (c) 2005 Gunnar Ritter, Freiburg i. Br., Germany
 *
 * Sccsid @(#)n5.c	1.25 (gritter) 12/6/05
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

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <ctype.h>
#ifdef 	EUC
#ifdef	NROFF
#include <stddef.h>
#ifdef	__sun
#include <widec.h>
#else
#include <wchar.h>
#endif
#endif	/* NROFF */
#endif	/* EUC */
#include <string.h>
#include <unistd.h>
#include "tdef.h"
#include "ext.h"
#ifdef	NROFF
#include "tw.h"
#endif
#include "proto.h"

extern void mchbits(void);

/*
 * troff5.c
 * 
 * misc processing requests
 */

static int	*iflist;
static int	ifx;

static void
growiflist(void)
{
	int	nnif = NIF + 15;

	if ((iflist = realloc(iflist, nnif * sizeof *iflist)) == NULL) {
		errprint("if-else overflow.");
		ifx = 0;
		edone(040);
	}
	memset(&iflist[NIF], 0, (nnif-NIF) * sizeof *iflist);
	NIF = nnif;
}

void
casead(void)
{
	register int i;

	ad = 1;
	/*leave admod alone*/
	if (skip())
		return;
	switch (i = cbits(getch())) {
	case 'r':	/*right adj, left ragged*/
		admod = 2;
		break;
	case 'l':	/*left adj, right ragged*/
		admod = ad = 0;	/*same as casena*/
		break;
	case 'c':	/*centered adj*/
		admod = 1;
		break;
	case 'b': 
	case 'n':
		admod = 0;
		break;
	case '0': 
	case '2': 
	case '4':
		ad = 0;
	case '1': 
	case '3': 
	case '5':
		admod = (i - '0') / 2;
	}
}


void
casena(void)
{
	ad = 0;
}


void
casefi(void)
{
	tbreak();
	fi++;
	pendnf = 0;
	lnsize = LNSIZE;
}


void
casenf(void)
{
	tbreak();
	fi = 0;
}


void
casers(void)
{
	dip->nls = 0;
}


void
casens(void)
{
	dip->nls++;
}


void
casespreadwarn(void)
{
	if (skip())
		spreadwarn = !spreadwarn;
	else {
		dfact = EM;
		spreadlimit = inumb(&spreadlimit);
		spreadwarn = 1;
	}
}


int 
chget(int c)
{
	tchar i = 0;

	if (skip() || ismot(i = getch()) || cbits(i) == ' ' || cbits(i) == '\n') {
		ch = i;
		return(c);
	} else 
		return(i & BYTEMASK);
}


void
casecc(void)
{
	cc = chget('.');
}


void
casec2(void)
{
	c2 = chget('\'');
}


void
casehc(void)
{
	ohc = chget(OHC);
}


void
casetc(void)
{
	tabc = chget(0);
}


void
caselc(void)
{
	dotc = chget(0);
}


void
casehy(void)
{
	register int i;

	hyf = 1;
	if (skip())
		return;
	noscale++;
	i = atoi();
	noscale = 0;
	if (nonumb)
		return;
	hyf = max(i, 0);
}


void
casenh(void)
{
	hyf = 0;
}


int 
max(int aa, int bb)
{
	if (aa > bb)
		return(aa);
	else 
		return(bb);
}

int 
min(int aa, int bb)
{
	if (aa < bb)
		return(aa);
	else 
		return(bb);
}


void
casece(void)
{
	register int i;

	noscale++;
	skip();
	i = max(atoi(), 0);
	if (nonumb)
		i = 1;
	tbreak();
	ce = i;
	noscale = 0;
}


void
casein(void)
{
	register int i;

	if (skip())
		i = in1;
	else 
		i = max(hnumb(&in), 0);
	tbreak();
	in1 = in;
	in = i;
	if (!nc) {
		un = in;
		setnel();
	}
}


void
casell(void)
{
	register int i;

	if (skip())
		i = ll1;
	else 
		i = max(hnumb(&ll), INCH / 10);
	ll1 = ll;
	ll = i;
	setnel();
}


void
caselt(void)
{
	register int i;

	if (skip())
		i = lt1;
	else 
		i = max(hnumb(&lt), 0);
	lt1 = lt;
	lt = i;
}


void
caseti(void)
{
	register int i;

	if (skip())
		return;
	i = max(hnumb(&in), 0);
	tbreak();
	un1 = i;
	setnel();
}


void
casels(void)
{
	register int i;

	noscale++;
	if (skip())
		i = ls1;
	else 
		i = max(inumb(&ls), 1);
	ls1 = ls;
	ls = i;
	noscale = 0;
}


void
casepo(void)
{
	register int i;

	if (skip())
		i = po1;
	else 
		i = max(hnumb(&po), 0);
	po1 = po;
	po = i;
#ifndef NROFF
	if (!ascii)
		esc += po - po1;
#endif
}


void
casepl(void)
{
	register int i;

	skip();
	if ((i = vnumb(&pl)) == 0)
		pl = defaultpl ? defaultpl : 11 * INCH; /*11in*/
	else 
		pl = i;
	if (numtab[NL].val > pl)
		numtab[NL].val = pl;
}


void
casewh(void)
{
	register int i, j, k;

	lgf++;
	skip();
	i = vnumb((int *)0);
	if (nonumb)
		return;
	skip();
	j = getrq();
	if (j >= 256)
		j = maybemore(j, 0);
	if ((k = findn(i)) != NTRAP) {
		mlist[k] = j;
		return;
	}
	for (k = 0; k < NTRAP; k++)
		if (mlist[k] == 0)
			break;
	if (k == NTRAP) {
		flusho();
		errprint("cannot plant trap.");
		return;
	}
	mlist[k] = j;
	nlist[k] = i;
}


void
casech(void)
{
	register int i, j, k;

	lgf++;
	skip();
	if (!(j = getrq()))
		return;
	else  {
		if (j >= 256)
			j = maybemore(j, 0);
		for (k = 0; k < NTRAP; k++)
			if (mlist[k] == j)
				break;
	}
	if (k == NTRAP)
		return;
	skip();
	i = vnumb((int *)0);
	if (nonumb)
		mlist[k] = 0;
	nlist[k] = i;
}


int 
findn(int i)
{
	register int k;

	for (k = 0; k < NTRAP; k++)
		if ((nlist[k] == i) && (mlist[k] != 0))
			break;
	return(k);
}


void
casepn(void)
{
	register int i;

	skip();
	noscale++;
	i = max(inumb(&numtab[PN].val), 0);
	noscale = 0;
	if (!nonumb) {
		npn = i;
		npnflg++;
	}
}


void
casebp(void)
{
	register int i;
	register struct s *savframe;

	if (dip != d)
		return;
	savframe = frame;
	skip();
	if ((i = inumb(&numtab[PN].val)) < 0)
		i = 0;
	tbreak();
	if (!nonumb) {
		npn = i;
		npnflg++;
	} else if (dip->nls)
		return;
	eject(savframe);
}


static void
tmtmcwr(int ab, int tmc, int wr)
{
	register int i;
	char	tmbuf[NTM];

	lgf++;
	copyf++;
	if (skip() && ab)
		errprint("User Abort");
	for (i = 0; i < NTM - 2; )
		if ((tmbuf[i++] = getch()) == '\n')
			break;
	if (i == NTM - 2)
		tmbuf[i++] = '\n';
	if (tmc)
		i--;
	tmbuf[i] = 0;
	if (ab)	/* truncate output */
		obufp = obuf;	/* should be a function in n2.c */
	if (wr < 0) {
		flusho();
		fdprintf(stderr, "%s", tmbuf);
	} else if (i)
		write(wr, tmbuf, i);
	copyf--;
	lgf--;
}

void
casetm(int ab)
{
	tmtmcwr(ab, 0, -1);
}

void
casetmc(void)
{
	tmtmcwr(0, 1, -1);
}

static struct stream {
	char	*name;
	int	fd;
} *streams;
static int	nstreams;

static void
open1(int flags)
{
	int	ns = nstreams;

	lgf++;
	if (skip() || !getname() || skip())
		return;
	streams = realloc(streams, sizeof *streams * ++nstreams);
	streams[ns].name = malloc(NS);
	strcpy(streams[ns].name, nextf);
	getname();
	if ((streams[ns].fd = open(nextf, flags, 0666)) < 0) {
		errprint("can't open file %s", nextf);
		done(02);
	}
}

void
caseopen(void)
{
	open1(O_WRONLY|O_CREAT|O_TRUNC);
}

void
caseopena(void)
{
	open1(O_WRONLY|O_CREAT|O_APPEND);
}

static int
getstream(const char *name)
{
	int	i;

	for (i = 0; i < nstreams; i++)
		if (strcmp(streams[i].name, name) == 0)
			return i;
	errprint("no such stream %s", name);
	return -1;
}

static void
write1(int writec)
{
	int	i;

	lgf++;
	if (skip() || !getname())
		return;
	if ((i = getstream(nextf)) < 0)
		return;
	tmtmcwr(0, writec, streams[i].fd);
}

void
casewrite(void)
{
	write1(0);
}

void
casewritec(void)
{
	write1(1);
}

void
caseclose(void)
{
	int	i;

	lgf++;
	if (skip() || !getname())
		return;
	if ((i = getstream(nextf)) < 0)
		return;
	free(streams[i].name);
	memmove(&streams[i], &streams[i+1], sizeof *streams);
	nstreams--;
}


void
casesp(int a)
{
	register int i, j, savlss;

	tbreak();
	if (dip->nls || trap)
		return;
	i = findt1();
	if (!a) {
		skip();
		j = vnumb((int *)0);
		if (nonumb)
			j = lss;
	} else 
		j = a;
	if (j == 0)
		return;
	if (i < j)
		j = i;
	savlss = lss;
	if (dip != d)
		i = dip->dnl; 
	else 
		i = numtab[NL].val;
	if ((i + j) < 0)
		j = -i;
	lss = j;
	newline(0);
	lss = savlss;
}


void
casert(void)
{
	register int a, *p;

	skip();
	if (dip != d)
		p = &dip->dnl; 
	else 
		p = &numtab[NL].val;
	a = vnumb(p);
	if (nonumb)
		a = dip->mkline;
	if ((a < 0) || (a >= *p))
		return;
	nb++;
	casesp(a - *p);
}


void
caseem(void)
{
	lgf++;
	skip();
	em = getrq();
	if (em >= 256)
		em = maybemore(em, 1);
}


void
casefl(void)
{
	tbreak();
	flusho();
}


static struct evnames {
	int	number;
	char	*name;
} *evnames;
static struct env	*evp;
static int	*evlist;
static int	evi;
static int	evlsz;
static int	Nev = NEV;

static struct env *
findev(int *number, char *name)
{
	int	i;

	if (*number < 0)
		return &evp[-1 - (*number)];
	else if (name) {
		for (i = 0; i < Nev-NEV; i++)
			if (evnames[i].name != NULL &&
					strcmp(evnames[i].name, name) == 0) {
				*number = -1 - i;
				return &evp[i];
			}
		*number = -1 - i;
		return NULL;
	} else if (*number >= NEV) {
		for (i = 0; i < Nev-NEV; i++)
			if (evnames[i].name == NULL &&
					evnames[i].number == *number)
				return &evp[i];
		*number = -1 - i;
		return NULL;
	} else {
#ifdef	INCORE
		extern tchar *corebuf;
		return &((struct env *)corebuf)[*number];
#else
		return (struct env *)-1;
#endif
	}
}

static int
getev(int *nxevp, char **namep)
{
	char	*name = NULL;
	int nxev = 0;
	char	c;
	int	i = 0, sz = 0;

	*namep = NULL;
	*nxevp = 0;
	if (skip())
		return 0;
	c = cbits(ch);
	if (xflag == 0 || isdigit(c) || c == '(') {
		noscale++;
		nxev = atoi();
		noscale = 0;
		if (nonumb) {
			flushi();
			return 0;
		}
	} else {
		do {
			c = getach();
			if (i >= sz)
				name = realloc(name, (sz += 8) * sizeof *name);
			name[i++] = c;
		} while (c);
	}
	flushi();
	*namep = name;
	*nxevp = nxev;
	return 1;
}

void
caseev(void)
{
	char	*name;
	int nxev;
	struct env	*np, *op;

	if (getev(&nxev, &name) == 0) {
		if (evi == 0)
			return;
		nxev =  evlist[--evi];
		goto e1;
	}
	if (xflag == 0 && ((nxev >= NEV) || (nxev < 0) || (evi >= EVLSZ)))
		goto cannot;
	if (evi >= evlsz) {
		evlsz = evi + 1;
		if ((evlist = realloc(evlist, evlsz * sizeof *evlist)) == NULL)
			goto cannot;
	}
	if (name && findev(&nxev, name) == NULL || nxev >= Nev) {
		if ((evp = realloc(evp, (Nev-NEV+1) * sizeof *evp)) == NULL ||
				(evnames = realloc(evnames,
				   (Nev-NEV+1) * sizeof *evnames)) == NULL)
			goto cannot;
		evnames[Nev-NEV].number = nxev;
		evnames[Nev-NEV].name = name;
		evp[Nev-NEV] = initenv;
		Nev++;
	}
	if (name == NULL && nxev < 0) {
		flusho();
	cannot:	errprint("cannot do ev.");
		if (error)
			done2(040);
		else 
			edone(040);
		return;
	}
	evlist[evi++] = ev;
e1:
	if (ev == nxev)
		return;
	if ((np = findev(&nxev, name)) == NULL ||
			(op = findev(&ev, NULL)) == NULL)
		goto cannot;
#ifdef INCORE
	*op = env;
	env = *np;
#else
	if (ev >= 0 && ev < NEV) {
		lseek(ibf, ev * sizeof(env), SEEK_SET);
		write(ibf, (char *) & env, sizeof(env));
	} else
		*op = env;
	if (nxev >= 0 && nxev < NEV) {
		lseek(ibf, nxev * sizeof(env), SEEK_SET);
		read(ibf, (char *) & env, sizeof(env));
	} else
		env = *np;
#endif
	ev = nxev;
}

void
caseevc(void)
{
	char	*name;
	int	nxev;
	struct env	tmpenv, *ep;

	if (getev(&nxev, &name) == 0 || (ep = findev(&nxev, name)) == NULL)
		return;
	tmpenv = env;
#ifndef	INCORE
	if (nxev >= 0 && nxev < NEV) {
		lseek(ibuf, nxev * sizeof(env), SEEK_SET);
		read(ibf, (char *) & env, sizeof(env));
	} else
#endif
	env = *ep;
	env._pendnf = 0;
	env._pendw = 0;
	env._pendt = 0;
	env._wch = 0;
	env._wne = 0;
	env._wdstart = 0;
	env._wdend = 0;
	env._linep = line;
	env._wordp = 0;
	env._spflg = 0;
	env._ce = 0;
	env._nn = 0;
	env._ndf = 0;
	env._nms = 0;
	env._ni = 0;
	env._ul = 0;
	env._cu = 0;
	env._it = 0;
	env._itmac = 0;
	env._pendnf = 0;
}

void
caseel(void)
{
	if (--ifx < 0) {
		ifx = 0;
		if (NIF == 0)
			growiflist();
		iflist[0] = 0;
	}
	caseif(2);
}


void
caseie(void)
{
	if (ifx >= NIF)
		growiflist();
	caseif(1);
	ifx++;
}


void
caseif(int x)
{
	extern int falsef;
	register int notflag, true;
	tchar i;

	if (x == 2) {
		notflag = 0;
		true = iflist ? iflist[ifx] : 0;
		goto i1;
	}
	true = 0;
	skip();
	if ((cbits(i = getch())) == '!') {
		notflag = 1;
	} else {
		notflag = 0;
		ch = i;
	}
	i = atoi();
	if (!nonumb) {
		if (i > 0)
			true++;
		goto i1;
	}
	i = getch();
	switch (cbits(i)) {
	case 'e':
		if (!(numtab[PN].val & 01))
			true++;
		break;
	case 'o':
		if (numtab[PN].val & 01)
			true++;
		break;
#ifdef NROFF
	case 'n':
		true++;
	case 't':
#endif
#ifndef NROFF
	case 't':
		true++;
	case 'n':
#endif
	case ' ':
		break;
	default:
		true = cmpstr(i);
	}
i1:
	true ^= notflag;
	if (x == 1) {
		if (ifx >= NIF)
			growiflist();
		iflist[ifx] = !true;
	}
	if (true) {
i2:
		while ((cbits(i = getch())) == ' ')
			;
		if (cbits(i) == LEFT)
			goto i2;
		ch = i;
		nflush++;
	} else {
		copyf++;
		falsef++;
		eatblk(0);
		copyf--;
		falsef--;
	}
}

void
casereturn(void)
{
	popi();
}

void
eatblk(int inblk)
{	register int cnt, i;

	cnt = 0;
	do {
		if (ch)	{
			i = cbits(ch);
			ch = 0;
		} else
			i = cbits(getch0());
		if (i == ESC)
			cnt++;
		else {
			if (cnt == 1)
				switch (i) {
				case '{':  i = LEFT; break;
				case '}':  i = RIGHT; break;
				case '\n': i = 'x'; break;
				}
			cnt = 0;
		}
		if (i == LEFT) eatblk(1);
	} while ((!inblk && (i != '\n')) || (inblk && (i != RIGHT)));
	if (i == '\n')
		nlflg++;
}


int 
cmpstr(tchar c)
{
	register int j, delim;
	register tchar i;
	register int val;
	int savapts, savapts1, savfont, savfont1, savpts, savpts1;
	tchar string[1280];
	register tchar *sp;

	if (ismot(c))
		return(0);
	delim = cbits(c);
	savapts = apts;
	savapts1 = apts1;
	savfont = font;
	savfont1 = font1;
	savpts = pts;
	savpts1 = pts1;
	sp = string;
	while ((j = cbits(i = getch()))!=delim && j!='\n' && sp<&string[1280-1])
		*sp++ = i;
	if (sp >= string + 1280) {
		errprint("too-long string compare.");
		edone(0100);
	}
	if (nlflg) {
		val = sp==string;
		goto rtn;
	}
	*sp++ = 0;
	apts = savapts;
	apts1 = savapts1;
	font = savfont;
	font1 = savfont1;
	pts = savpts;
	pts1 = savpts1;
	mchbits();
	val = 1;
	sp = string;
	while ((j = cbits(i = getch())) != delim && j != '\n') {
		if (*sp != i) {
			eat(delim);
			val = 0;
			goto rtn;
		}
		sp++;
	}
	if (*sp)
		val = 0;
rtn:
	apts = savapts;
	apts1 = savapts1;
	font = savfont;
	font1 = savfont1;
	pts = savpts;
	pts1 = savpts1;
	mchbits();
	return(val);
}


void
caserd(void)
{

	lgf++;
	skip();
	getname();
	if (!iflg) {
		if (quiet) {
#ifdef	NROFF
			echo_off();
			flusho();
#endif	/* NROFF */
			fdprintf(stderr, "\007"); /*bell*/
		} else {
			if (nextf[0]) {
				fdprintf(stderr, "%s:", nextf);
			} else {
				fdprintf(stderr, "\007"); /*bell*/
			}
		}
	}
	collect();
	tty++;
	pushi(XBLIST*BLK, PAIR('r','d'));
}


int 
rdtty(void)
{
	char	onechar;
#ifdef EUC
#ifdef NROFF
	int	i, n, col_index;
#endif /* NROFF */
#endif /* EUC */

	onechar = 0;
	if (read(0, &onechar, 1) == 1) {
		if (onechar == '\n')
			tty++;
		else 
			tty = 1;
#ifndef EUC
		if (tty != 3)
			return(onechar);
#else
#ifndef NROFF
		if (tty != 3)
			return(onechar);
#else
		if (tty != 3) {
			if (!multi_locale)
				return(onechar);
			i = onechar & 0377;
			*mbbuf1p++ = i;
			*mbbuf1p = 0;
			if ((*mbbuf1&~(wchar_t)0177) == 0) {
				twc = *mbbuf1;
				i |= (BYTE_CHR);
				setcsbits(i, 0);
				twc = 0;
				mbbuf1p = mbbuf1;
			}
			else if ((n = mbtowc(&twc, mbbuf1, mb_cur_max)) <= 0) {
				if (mbbuf1p >= mbbuf1 + mb_cur_max) {
					i &= ~(MBMASK | CSMASK);
					twc = 0;
					mbbuf1p = mbbuf1;
					*mbbuf1p = 0;
				} else {
					i |= (MIDDLEOFMB);
				}
			} else {
				if (n > 1)
					i |= (LASTOFMB);
				else
					i |= (BYTE_CHR);
				if ((twc & ~(wchar_t)0177) == 0) {
					col_index = 0;
				} else {
					if ((col_index = wcwidth(twc)) < 0)
						col_index = 0;
				}
				setcsbits(i, col_index);
				twc = 0;
				mbbuf1p = mbbuf1;
			}
			return(i);
		}
#endif /* NROFF */
#endif /* EUC */
	}
	popi();
	tty = 0;
#ifdef	NROFF
	if (quiet)
		echo_on();
#endif	/* NROFF */
	return(0);
}


void
caseec(void)
{
	eschar = chget('\\');
}


void
caseeo(void)
{
	eschar = 0;
}


void
caseta(void)
{
	register int i;

	tabtab[0] = nonumb = 0;
	for (i = 0; ((i < (NTAB - 1)) && !nonumb); i++) {
		if (skip())
			break;
		tabtab[i] = max(hnumb(&tabtab[max(i-1,0)]), 0) & TABMASK;
		if (!nonumb) 
			switch (cbits(ch)) {
			case 'C':
				tabtab[i] |= CTAB;
				break;
			case 'R':
				tabtab[i] |= RTAB;
				break;
			default: /*includes L*/
				break;
			}
		nonumb = ch = 0;
	}
	tabtab[i] = 0;
}


void
casene(void)
{
	register int i, j;

	skip();
	i = vnumb((int *)0);
	if (nonumb)
		i = lss;
	if (i > (j = findt1())) {
		i = lss;
		lss = j;
		dip->nls = 0;
		newline(0);
		lss = i;
	}
}


void
casetr(void)
{
	register int i, j;
	tchar k;

	lgf++;
	skip();
	while ((i = cbits(k=getch())) != '\n') {
		if (ismot(k))
			return;
		if (ismot(k = getch()))
			return;
		if ((j = cbits(k)) == '\n')
			j = ' ';
		trtab[i] = j;
	}
}


void
casecu(void)
{
	cu++;
	caseul();
}


void
caseul(void)
{
	register int i;

	noscale++;
	if (skip())
		i = 1;
	else 
		i = atoi();
	if (ul && (i == 0)) {
		font = sfont;
		ul = cu = 0;
	}
	if (i) {
		if (!ul) {
			sfont = font;
			font = ulfont;
		}
		ul = i;
	}
	noscale = 0;
	mchbits();
}


void
caseuf(void)
{
	register int i, j;
	extern int findft(int);

	if (skip() || !(i = getrq()) || i == 'S' ||  (j = findft(i))  == -1)
		ulfont = ULFONT; /*default underline position*/
	else 
		ulfont = j;
#ifdef NROFF
	if (ulfont == FT)
		ulfont = ULFONT;
#endif
}


void
caseit(void)
{
	register int i;

	lgf++;
	it = itmac = 0;
	noscale++;
	skip();
	i = atoi();
	skip();
	if (!nonumb && (itmac = getrq())) {
		if (itmac >= 256)
			itmac = maybemore(itmac, 1);
		it = i;
	}
	noscale = 0;
}


void
casemc(void)
{
	register int i;

	if (icf > 1)
		ic = 0;
	icf = 0;
	if (skip())
		return;
	ic = getch();
	icf = 1;
	skip();
	i = max(hnumb((int *)0), 0);
	if (!nonumb)
		ics = i;
}


void
casemk(void)
{
	register int i, j;

	if (dip != d)
		j = dip->dnl; 
	else 
		j = numtab[NL].val;
	if (skip()) {
		dip->mkline = j;
		return;
	}
	if ((i = getrq()) == 0)
		return;
	numtab[findr(i)].val = j;
}


void
casesv(void)
{
	register int i;

	skip();
	if ((i = vnumb((int *)0)) < 0)
		return;
	if (nonumb)
		i = 1;
	sv += i;
	caseos();
}


void
caseos(void)
{
	register int savlss;

	if (sv <= findt1()) {
		savlss = lss;
		lss = sv;
		newline(0);
		lss = savlss;
		sv = 0;
	}
}


void
casenm(void)
{
	register int i;

	lnmod = nn = 0;
	if (skip())
		return;
	lnmod++;
	noscale++;
	i = inumb(&numtab[LN].val);
	if (!nonumb)
		numtab[LN].val = max(i, 0);
	getnm(&ndf, 1);
	getnm(&nms, 0);
	getnm(&ni, 0);
	noscale = 0;
	nmbits = chbits;
}


void
getnm(int *p, int min)
{
	register int i;

	eat(' ');
	if (skip())
		return;
	i = atoi();
	if (nonumb)
		return;
	*p = max(i, min);
}


void
casenn(void)
{
	noscale++;
	skip();
	nn = max(atoi(), 1);
	noscale = 0;
}


void
caseab(void)
{
	casetm(1);
	done3(0);
}


#ifdef	NROFF
/*
 * The following routines are concerned with setting terminal options.
 *	The manner of doing this differs between research/Berkeley systems
 *	and UNIX System V systems (i.e. DOCUMENTER'S WORKBENCH)
 *	The distinction is controlled by the #define'd variable USG,
 *	which must be set by System V users.
 */


#ifdef	USG
#include <termios.h>
#define	ECHO_USG (ECHO | ECHOE | ECHOK | ECHONL)
struct termios	ttys;
#else
#include <sgtty.h>
struct	sgttyb	ttys[2];
#endif	/* USG */

int	ttysave[2] = {-1, -1};

void
save_tty(void)			/*save any tty settings that may be changed*/
{

#ifdef	USG
	if (tcgetattr(0, &ttys) >= 0)
		ttysave[0] = ttys.c_lflag;
#else
	if (gtty(0, &ttys[0]) >= 0)
		ttysave[0] = ttys[0].sg_flags;
	if (gtty(1, &ttys[1]) >= 0)
		ttysave[1] = ttys[1].sg_flags;
#endif	/* USG */

}


void 
restore_tty (void)			/*restore tty settings from beginning*/
{

	if (ttysave[0] != -1) {
#ifdef	USG
		ttys.c_lflag = ttysave[0];
		tcsetattr(0, TCSADRAIN, &ttys);
#else
		ttys[0].sg_flags = ttysave[0];
		stty(0, &ttys[0]);
	}
	if (ttysave[1] != -1) {
		ttys[1].sg_flags = ttysave[1];
		stty(1, &ttys[1]);
#endif	/* USG */
	}
}


void 
set_tty (void)			/*this replaces the use of bset and breset*/
{

#ifndef	USG			/*for research/BSD only, reset CRMOD*/
	if (ttysave[1] == -1)
		save_tty();
	if (ttysave[1] != -1) {
		ttys[1].sg_flags &= ~CRMOD;
		stty(1, &ttys[1]);
	}
#endif	/* USG */

}


void 
echo_off (void)			/*turn off ECHO for .rd in "-q" mode*/
{
	if (ttysave[0] == -1)
		return;

#ifdef	USG
	ttys.c_lflag &= ~ECHO_USG;
	tcsetattr(0, TCSADRAIN, &ttys);
#else
	ttys[0].sg_flags &= ~ECHO;
	stty(0, &ttys[0]);
#endif	/* USG */

}


void 
echo_on (void)			/*restore ECHO after .rd in "-q" mode*/
{
	if (ttysave[0] == -1)
		return;

#ifdef	USG
	ttys.c_lflag |= ECHO_USG;
	tcsetattr(0, TCSADRAIN, &ttys);
#else
	ttys[0].sg_flags |= ECHO;
	stty(0, &ttys[0]);
#endif	/* USG */

}
#endif	/* NROFF */
