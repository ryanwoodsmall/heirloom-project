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


/*	from OpenSolaris "t6.c	1.9	05/06/08 SMI"	*/

/*
 * Portions Copyright (c) 2005 Gunnar Ritter, Freiburg i. Br., Germany
 *
 * Sccsid @(#)t6.c	1.25 (gritter) 8/19/05
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
 * t6.c
 * 
 * width functions, sizes and fonts
 */

#include <stdlib.h>
#include "tdef.h"
#include "dev.h"
#include <ctype.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include "ext.h"
#include "afm.h"
#include "proto.h"
#include "troff.h"

/* fitab[f][c] is 0 if c is not on font f */
	/* if it's non-zero, c is in fontab[f] at position
	 * fitab[f][c].
	 */
int	*fontlab;
short	*pstab;
int	*cstab;
int	*ccstab;
int	**fallbacktab;
int	*bdtab;
struct tkftab	*tkftab;
int	sbold = 0;

int
width(register tchar j)
{
	register int i, k;

	if (j & (ZBIT|MOT)) {
		if (iszbit(j))
			return(0);
		if (isvmot(j))
			return(0);
		k = absmot(j);
		if (isnmot(j))
			k = -k;
		return(k);
	}
	i = cbits(j);
	if (i < ' ') {
		if (i == '\b')
			return(-widthp);
		if (i == PRESC)
			i = eschar;
		else if (iscontrol(i))
			return(0);
	}
	if (i==ohc)
		return(0);
	i = trtab[i];
	if (i < 32)
		return(0);
	if (sfbits(j) == oldbits) {
		xfont = pfont;
		xpts = ppts;
	} else 
		xbits(j, 0);
	if (widcache[i-32].fontpts == (xfont<<8) + xpts && !setwdf)
		k = widcache[i-32].width;
	else {
		k = getcw(i-32);
		if (bd)
			k += (bd - 1) * HOR;
		if (cs)
			k = cs;
	}
	widthp = k;
	return(k);
}

/*
 * clear width cache-- s means just space
 */
void
zapwcache(int s)
{
	register int i;

	if (s) {
		widcache[0].fontpts = 0;
		return;
	}
	for (i=0; i<NWIDCACHE; i++)
		widcache[i].fontpts = 0;
}

int
getcw(register int i)
{
	register int	k;
	register char	*p;
	register int	x, j;
	int nocache = 0;
	int	ofont = xfont;
	int	s;

	bd = 0;
	if (i >= nchtab + 128-32) {
		j = abscw(i + 32 - (nchtab+128));
		goto g0;
	}
	if (i == 0) {	/* a blank */
		k = (fontab[xfont][0] * spacesz + 6) / 12;
		/* this nonsense because .ss cmd uses 1/36 em as its units */
		/* and default is 12 */
		goto g1;
	}
	if ((j = fitab[xfont][i] & BYTEMASK) == 0) {	/* it's not on current font */
		int ii, jj;
		/* search through search list of xfont
		 * to see what font it ought to be on.
		 * first searches explicit fallbacks, then
		 * searches S, then remaining fonts in wraparound order.
		 */
		nocache = 1;
		if (fallbacktab[xfont]) {
			for (jj = 0; fallbacktab[xfont][jj] != 0; jj++) {
				if ((ii = findft(fallbacktab[xfont][jj])) < 0)
					continue;
				j = fitab[ii][i] & BYTEMASK;
				if (j != 0) {
					xfont = ii;
					goto found;
				}
			}
		}
		if (smnt) {
			ii = smnt;
			j = fitab[ii][i] & BYTEMASK;
			if (j != 0)
				goto found;
			for (ii=0; ii <= nfonts; ii++) {
				if (ii == smnt || fitab[ii] == NULL)
					continue;
				j = fitab[ii][i] & BYTEMASK;
				if (j != 0) {
				found:	p = fontab[ii];
					k = *(p + j);
					if (xfont == sbold)
						bd = bdtab[ii];
					if (setwdf)
						numtab[CT].val |= kerntab[ii][j];
					goto g1;
				}
			}
		}
		k = fontab[xfont][0];	/* leave a space-size space */
		goto g1;
	}
 g0:
	p = fontab[xfont];
	if (setwdf)
		numtab[CT].val |= kerntab[ofont][j];
	k = *(p + j);
 g1:
	if (!bd)
		bd = bdtab[ofont];
	if (cs = cstab[ofont]) {
		nocache = 1;
		if (ccs = ccstab[ofont])
			x = ccs; 
		else 
			x = xpts;
		cs = (cs * EMPTS(x)) / 36;
	}
	k = ((k&BYTEMASK) * xpts + (Unitwidth / 2)) / Unitwidth;
	s = xpts*Unitwidth;
	if (s <= tkftab[ofont].s1 && tkftab[ofont].n1) {
		nocache = 1;
		k += tkftab[ofont].n1;
	} else if (s >= tkftab[ofont].s2 && tkftab[ofont].n2) {
		nocache = 1;
		k += tkftab[ofont].n2;
	} else if (s > tkftab[ofont].s1 && s < tkftab[ofont].s2) {
		int	r;
		r = (s * tkftab[ofont].n2 - s * tkftab[ofont].n1
				+ tkftab[ofont].s2 * tkftab[ofont].n1
				- tkftab[ofont].s1 * tkftab[ofont].n2)
			/ (tkftab[ofont].s2 - tkftab[ofont].s1);
		if (r != 0) {
			nocache = 1;
			k += r;
		}
	}
	if (nocache|bd)
		widcache[i].fontpts = 0;
	else {
		widcache[i].fontpts = (xfont<<8) + xpts;
		widcache[i].width = k;
	}
	return(k);
	/* Unitwidth is Units/Point, where
	 * Units is the fundamental digitization
	 * of the character set widths, and
	 * Point is the number of goobies in a point
	 * e.g., for cat, Units=36, Point=6, so Unitwidth=36/6=6
	 * In effect, it's the size at which the widths
	 * translate directly into units.
	 */
}

int
abscw(int n)	/* return index of abs char n in fontab[], etc. */
{	register int i, ncf;

	if (afmtab && (i = (fontbase[xfont]->spare1&BYTEMASK) - 1) >= 0)
		return afmtab[i]->fitab[n-32]&BYTEMASK;
	ncf = fontbase[xfont]->nwfont & BYTEMASK;
	for (i = 0; i < ncf; i++)
		if (codetab[xfont][i] == n)
			return i;
	return 0;
}

void
xbits(register tchar i, int bitf)
{
	register int k;

	xfont = fbits(i);
	k = sbits(i);
	if (k) {
		xpts = pstab[--k];
		oldbits = sfbits(i);
		pfont = xfont;
		ppts = xpts;
		return;
	}
	switch (bitf) {
	case 0:
		xfont = font;
		xpts = pts;
		break;
	case 1:
		xfont = pfont;
		xpts = ppts;
		break;
	case 2:
		xfont = mfont;
		xpts = mpts;
	}
}

static tchar
postchar1(const char *temp, int f)
{
	struct afmtab	*a;
	int	i, j;

	if (afmtab && (i = (fontbase[f]->spare1&BYTEMASK) - 1) >= 0) {
		a = afmtab[i];
		for (j = 1; j < a->nchars; j++)
			if (a->nametab[j] != NULL &&
					strcmp(a->nametab[j], temp) == 0)
				for (i = 0; i < a->nchars + 128 - 32 + nchtab;
						i++)
					if ((a->fitab[i]&BYTEMASK) == j)
						return i + 32 + nchtab + 128;
	}
	return(0);
}

static tchar
postchar(const char *temp, int *fp)
{
	int	i, j;
	tchar	c;

	*fp = xfont;
	if ((c = postchar1(temp, *fp)) != 0)
		return c;
	if (fallbacktab[xfont]) {
		for (j = 0; fallbacktab[xfont][j] != 0; j++) {
			if ((i = findft(fallbacktab[xfont][j])) < 0)
				continue;
			if ((c = postchar1(temp, i)) != 0) {
				*fp = i;
				return c;
			}
		}
	}
	return 0;
}

tchar setch(int delim)
{
	register int j;
	char	*temp = NULL;
	int	c, f, n, sz = 0;

	n = 0;
	for (;;) {
		c = getach();
		if (c == 0 && n < 2) {
			free(temp);
			return(0);
		}
		if (n >= sz)
			temp = realloc(temp, sz += 10);
		if (delim == '[' ? c == ']' : n == 2) {
			temp[n] = 0;
			break;
		}
		temp[n++] = c;
	}
	c = 0;
	if (delim == '[') {
		if ((c = postchar(temp, &f)) != 0) {
			c |= chbits & ~FMASK;
			setfbits(c, f);
		}
	}
	if (c == 0)
		for (j = 0; j < nchtab; j++)
			if (strcmp(&chname[chtab[j]], temp) == 0) {
				c = j + 128 | chbits;
				break;
			}
	free(temp);
	return c;
}

tchar setabs(void)		/* set absolute char from \C'...' */
{
	int n;

	getch();
	n = 0;
	n = inumb(&n);
	getch();
	if (nonumb)
		return 0;
	return n + nchtab + 128;
}



int
findft(register int i)
{
	register int k;

	if ((k = i - '0') >= 0 && k <= nfonts && k < smnt)
		return(k);
	for (k = 0; fontlab[k] != i; k++)
		if (k > nfonts)
			return(-1);
	return(k);
}

void
caseps(void)
{
	register int i;

	if (skip())
		i = apts1;
	else {
		noscale++;
		i = inumb(&apts);	/* this is a disaster for fractional point sizes */
		noscale = 0;
		if (nonumb)
			return;
	}
	casps1(i);
}

void
casps1(register int i)
{

/*
 * in olden times, it used to ignore changes to 0 or negative.
 * this is meant to allow the requested size to be anything,
 * in particular so eqn can generate lots of \s-3's and still
 * get back by matching \s+3's.

	if (i <= 0)
		return;
*/
	apts1 = apts;
	apts = i;
	pts1 = pts;
	pts = findps(i);
	mchbits();
}

int
findps(register int i)
{
	register int j, k;

	for (j=k=0 ; pstab[j] != 0 ; j++)
		if (abs(pstab[j]-i) < abs(pstab[k]-i))
			k = j;

	return(pstab[k]);
}

void
mchbits(void)
{
	register int i, j, k;

	i = pts;
	for (j = 0; i > (k = pstab[j]); j++)
		if (!k) {
			k = pstab[--j];
			break;
		}
	chbits = 0;
	setsbits(chbits, ++j);
	setfbits(chbits, font);
	sps = width(' ' | chbits);
	zapwcache(1);
}

void
setps(void)
{
	register int i, j = 0;

	i = cbits(getch());
	if (ischar(i) && isdigit(i)) {		/* \sd or \sdd */
		i -= '0';
		if (i == 0)		/* \s0 */
			j = apts1;
		else if (i <= 3 && ischar(j = cbits(ch = getch())) &&
		    isdigit(j)) {	/* \sdd */
			j = 10 * i + j - '0';
			ch = 0;
		} else		/* \sd */
			j = i;
	} else if (i == '(') {		/* \s(dd */
		j = cbits(getch()) - '0';
		j = 10 * j + cbits(getch()) - '0';
		if (j == 0)		/* \s(00 */
			j = apts1;
	} else if (i == '+' || i == '-') {	/* \s+, \s- */
		j = cbits(getch());
		if (ischar(j) && isdigit(j)) {		/* \s+d, \s-d */
			j -= '0';
		} else if (j == '(') {		/* \s+(dd, \s-(dd */
			j = cbits(getch()) - '0';
			j = 10 * j + cbits(getch()) - '0';
		}
		if (i == '-')
			j = -j;
		j += apts;
	}
	casps1(j);
}


tchar setht(void)		/* set character height from \H'...' */
{
	int n;
	tchar c;

	getch();
	n = inumb(&apts);
	getch();
	if (n == 0 || nonumb)
		n = apts;	/* does this work? */
	c = CHARHT;
	c |= ZBIT;
	setsbits(c, n);
	return(c);
}

tchar setslant(void)		/* set slant from \S'...' */
{
	int n;
	tchar c;

	getch();
	n = 0;
	n = inumb(&n);
	getch();
	if (nonumb)
		n = 0;
	c = SLANT;
	c |= ZBIT;
	setsfbits(c, n+180);
	return(c);
}

void
caseft(void)
{
	skip();
	setfont(1);
}

void
setfont(int a)
{
	register int i, j;

	if (a)
		i = getrq();
	else 
		i = getsn();
	if (!i || i == 'P') {
		j = font1;
		goto s0;
	}
	if (i == 'S' || i == '0')
		return;
	if ((j = findft(i)) == -1)
		if ((j = setfp(0, i, 0)) == -1)	/* try to put it in position 0 */
			return;
s0:
	font1 = font;
	font = j;
	mchbits();
}

void
setwd(void)
{
	register int base, wid;
	register tchar i;
	int	delim, emsz, k;
	int	savhp, savapts, savapts1, savfont, savfont1, savpts, savpts1;

	base = numtab[ST].val = numtab[ST].val = wid = numtab[CT].val = 0;
	if (ismot(i = getch()))
		return;
	delim = cbits(i);
	savhp = numtab[HP].val;
	numtab[HP].val = 0;
	savapts = apts;
	savapts1 = apts1;
	savfont = font;
	savfont1 = font1;
	savpts = pts;
	savpts1 = pts1;
	setwdf++;
	while (cbits(i = getch()) != delim && !nlflg) {
		k = width(i);
		wid += k;
		numtab[HP].val += k;
		if (!ismot(i)) {
			emsz = POINT * xpts;
		} else if (isvmot(i)) {
			k = absmot(i);
			if (isnmot(i))
				k = -k;
			base -= k;
			emsz = 0;
		} else 
			continue;
		if (base < numtab[SB].val)
			numtab[SB].val = base;
		if ((k = base + emsz) > numtab[ST].val)
			numtab[ST].val = k;
	}
	setn1(wid, 0, (tchar) 0);
	numtab[HP].val = savhp;
	apts = savapts;
	apts1 = savapts1;
	font = savfont;
	font1 = savfont1;
	pts = savpts;
	pts1 = savpts1;
	mchbits();
	setwdf = 0;
}


tchar vmot(void)
{
	dfact = lss;
	vflag++;
	return(mot());
}


tchar hmot(void)
{
	dfact = EM;
	return(mot());
}


tchar mot(void)
{
	register int j, n;
	register tchar i;

	j = HOR;
	getch(); /*eat delim*/
	if (n = atoi()) {
		if (vflag)
			j = VERT;
		i = makem(quant(n, j));
	} else
		i = 0;
	getch();
	vflag = 0;
	dfact = 1;
	return(i);
}


tchar sethl(int k)
{
	register int j;
	tchar i;

	j = EM / 2;
	if (k == 'u')
		j = -j;
	else if (k == 'r')
		j = -2 * j;
	vflag++;
	i = makem(j);
	vflag = 0;
	return(i);
}


tchar makem(register int i)
{
	register tchar j;

	if ((j = i) < 0)
		j = -j;
	j |= MOT;
	if (i < 0)
		j |= NMOT;
	if (vflag)
		j |= VMOT;
	return(j);
}


tchar getlg(tchar i)
{
	tchar j, k;
	register int lf;

	if ((lf = fontbase[fbits(i)]->ligfont) == 0) /* font lacks ligatures */
		return(i);
	j = getch0();
	if (cbits(j) == 'i' && (lf & LFI))
		j = LIG_FI;
	else if (cbits(j) == 'l' && (lf & LFL))
		j = LIG_FL;
	else if (cbits(j) == 'f' && (lf & LFF)) {
		if ((lf & (LFFI|LFFL)) && lg != 2) {
			k = getch0();
			if (cbits(k)=='i' && (lf&LFFI))
				j = LIG_FFI;
			else if (cbits(k)=='l' && (lf&LFFL))
				j = LIG_FFL;
			else {
				*pbp++ = k;
				j = LIG_FF;
			}
		} else 
			j = LIG_FF;
	} else {
		*pbp++ = j;
		j = i;
	}
	return(i & SFMASK | j);
}

void
caselg(void)
{

	lg = 1;
	if (skip())
		return;
	lg = atoi();
}

void
casefp(void)
{
	register int i, j;

	skip();
	if ((i = cbits(getch()) - '0') <= 0 || i > nfonts)
		errprint("fp: bad font position %d", i);
	else if (skip() || !(j = getrq()))
		errprint("fp: no font name");
	else if (skip() || !getname())
		setfp(i, j, 0);
	else		/* 3rd argument = filename */
		setfp(i, j, nextf);
}

int
setfp(int pos, int f, char *truename)	/* mount font f at position pos[0...nfonts] */
{
	extern int sprintf(char *, const char *, ...);
	register int k;
	int n;
	char longname[NS], shortname[20];

	zapwcache(0);
	if (truename)
		strcpy(shortname, truename);
	else {
		shortname[0] = f & BYTEMASK;
		shortname[1] = f >> BYTE;
		shortname[2] = '\0';
	}
	sprintf(longname, "%s/dev%s/%s.out", fontfile, devname, shortname);
	if ((k = open(longname, O_RDONLY)) < 0) {
		errprint("Can't open %s", longname);
		return(-1);
	}
	n = fontbase[pos]->nwfont & BYTEMASK;
	read(k, (char *) fontbase[pos], 3*n + nchtab + 128 - 32 + sizeof(struct Font));
	kerntab[pos] = (char *) fontab[pos] + (fontbase[pos]->nwfont & BYTEMASK);
	/* have to reset the fitab pointer because the width may be different */
	fitab[pos] = (char *) fontab[pos] + 3 * (fontbase[pos]->nwfont & BYTEMASK);
	if ((fontbase[pos]->nwfont & BYTEMASK) > n) {
		errprint("Font %s too big for position %d", shortname,
			pos);
		return(-1);
	}
	fontbase[pos]->nwfont = n;	/* so can load a larger one again later */
	close(k);
	if (pos == smnt) {
		smnt = 0; 
		sbold = 0; 
	}
	if ((fontlab[pos] = f) == 'S')
		smnt = pos;
	bdtab[pos] = cstab[pos] = ccstab[pos] = 0;
	fallbacktab[pos] = NULL;
	memset(&tkftab[pos], 0, sizeof tkftab[pos]);
		/* if there is a directory, no place to store its name. */
		/* if position isn't zero, no place to store its value. */
		/* only time a FONTPOS is pushed back is if it's a */
		/* standard font on position 0 (i.e., mounted implicitly. */
		/* there's a bug here:  if there are several input lines */
		/* that look like .ft XX in short successtion, the output */
		/* will all be in the last one because the "x font ..." */
		/* comes out too soon.  pushing back FONTPOS doesn't work */
		/* with .ft commands because input is flushed after .xx cmds */
	ptfpcmd(pos, shortname);
	if (pos == 0)
		ch = (tchar) FONTPOS | (tchar) f << 16;
	return(pos);
}

void
casecs(void)
{
	register int i, j;

	noscale++;
	skip();
	if (!(i = getrq()) || (i = findft(i)) < 0)
		goto rtn;
	skip();
	cstab[i] = atoi();
	skip();
	j = atoi();
	if (nonumb)
		ccstab[i] = 0;
	else
		ccstab[i] = findps(j);
rtn:
	zapwcache(0);
	noscale = 0;
}

void
casebd(void)
{
	register int i, j = 0, k;

	zapwcache(0);
	k = 0;
bd0:
	if (skip() || !(i = getrq()) || (j = findft(i)) == -1) {
		if (k)
			goto bd1;
		else 
			return;
	}
	if (j == smnt) {
		k = smnt;
		goto bd0;
	}
	if (k) {
		sbold = j;
		j = k;
	}
bd1:
	skip();
	noscale++;
	bdtab[j] = atoi();
	noscale = 0;
}

void
casevs(void)
{
	register int i;

	skip();
	vflag++;
	dfact = INCH; /* default scaling is points! */
	dfactd = 72;
	res = VERT;
	i = inumb(&lss);
	if (nonumb)
		i = lss1;
	if (i < VERT) 
		i = VERT;
	lss1 = lss;
	lss = i;
}

void
casess(void)
{
	register int i, j;

	noscale++;
	skip();
	if (i = atoi()) {
		spacesz = i & 0177;
		zapwcache(0);
		sps = width(' ' | chbits);
		if (xflag) {
			skip();
			j = atoi();
			if (!nonumb)
				ses = j;
		}
	}
	noscale = 0;
}


tchar xlss(void)
{
	/* stores \x'...' into
	 * two successive tchars.
	 * the first contains HX, the second the value,
	 * encoded as a vertical motion.
	 * decoding is done in n2.c by pchar().
	 */
	int	i;

	getch();
	dfact = lss;
	i = quant(atoi(), VERT);
	dfact = 1;
	getch();
	if (i >= 0)
		*pbp++ = MOT | VMOT | i;
	else
		*pbp++ = MOT | VMOT | NMOT | -i;
	return(HX);
}

struct afmtab **afmtab;
int nafm;

void
caseafm(void)
{
	extern int sprintf(char *, const char *, ...);
	struct stat	st;
	int	c, i = 0, j, rq, fd;
	char	*file = NULL, *path, *contents;
	size_t	sz = 0;
	struct afmtab	*a;
	int	nf = nfonts + 1;

	skip();
	if ((rq = getrq()) == 0)
		return;
	skip();
	do {
		c = getach() & 0377;
		if (i >= sz)
			file = realloc(file, (sz += 8) * sizeof *file);
		file[i++] = c;
	} while (c && c != ' ' && c != '\n');
	file[i-1] = 0;
	skip();
	if ((j = atoi()) > 0 && j <= nfonts)
		nf = j;
	path = malloc(strlen(fontfile) + strlen(devname) + strlen(file) + 14);
	sprintf(path, "%s/dev%s/afm/%s.afm", fontfile, devname, file);
	if ((fd = open(path, O_RDONLY)) < 0) {
		errprint("Can't open %s", path);
		free(file);
		free(path);
		return;
	}
	if (fstat(fd, &st) < 0) {
		errprint("Can't stat %s", path);
		free(file);
		free(path);
		return;
	}
	contents = malloc(st.st_size + 1);
	if (read(fd, contents, st.st_size) != st.st_size) {
		errprint("Can't read %s", path);
		free(file);
		free(path);
		free(contents);
		return;
	}
	contents[st.st_size] = 0;
	close(fd);
	a = calloc(1, sizeof *a);
	a->path = path;
	a->file = file;
	a->rq = rq;
	a->Font.namefont[0] = rq&0377;
	a->Font.namefont[1] = (rq>>8)&0377;
	sprintf(a->Font.intname, "%d", nf);
	if (afmget(a, contents, st.st_size) < 0) {
		free(file);
		free(path);
		free(contents);
		return;
	}
	free(contents);
	afmtab = realloc(afmtab, (nafm+1) * sizeof *afmtab);
	afmtab[nafm] = a;
	if (nf >= Nfont)
		growfonts(nf+1);
	a->Font.spare1 = nafm+1;
	if (nf <= NFONT)
		*fontbase[nf] = afmtab[nafm]->Font;
	else
		fontbase[nf] = &afmtab[nafm]->Font;
	fontlab[nf] = rq;
	fontab[nf] = afmtab[nafm]->fontab;
	kerntab[nf] = afmtab[nafm]->kerntab;
	codetab[nf] = afmtab[nafm]->codetab;
	fitab[nf] = afmtab[nafm]->fitab;
	bdtab[nf] = cstab[nf] = ccstab[nf] = 0;
	fallbacktab[nf] = NULL;
	memset(&tkftab[nf], 0, sizeof tkftab[nf]);
	nafm++;
	if (nf > nfonts)
		nfonts = nf;
	if (realpage)
		ptfpcmd(nf, a->file);
}

void
casesupply(void)
{
	int	c, i = 0, sz = 0;
	char	*file = NULL;

	skip();
	do {
		c = getach() & 0377;
		if (i >= sz)
			file = realloc(file, (sz += 8) * sizeof *file);
		file[i++] = c;
	} while (c && c != ' ' && c != '\n');
	file[i-1] = 0;
	ptsupply(file);
	free(file);
}

int
tkfnum(void)
{
	skip();
	dfact = INCH;
	dfactd = 72;
	res = VERT;
	return inumb(NULL);
}

void
casetkf(void)
{
	int	i, j, s1, n1, s2, n2;

	skip();
	i = getrq();
	if ((j = findft(i)) < 0)
		return;
	s1 = tkfnum();
	if (!nonumb) {
		n1 = tkfnum();
		if (!nonumb) {
			s2 = tkfnum();
			if (!nonumb) {
				n2 = tkfnum();
				if (!nonumb) {
					tkftab[j].s1 = s1;
					tkftab[j].n1 = n1;
					tkftab[j].s2 = s2;
					tkftab[j].n2 = n2;
					zapwcache(0);
				}
			}
		}
	}
}

void
casefallback(void)
{
	int	*fb = NULL;
	int	i, j, n = 0;

	skip();
	i = getrq();
	if ((j = findft(i)) < 0)
		return;
	do {
		skip();
		i = getrq();
		fb = realloc(fb, (n+2) * sizeof *fb);
		fb[n++] = i;
	} while (i);
	fallbacktab[j] = fb;
}

#include "unimap.h"

int
mapwc(int c, int *fp)
{
	int	i, j;

	for (i = 0; unimap[i].psc; i++)
		if (unimap[i].code == c) {
			if ((j = postchar(unimap[i].psc, fp)) != 0)
				return j;
			else
				break;
		}
	return 0;
}
