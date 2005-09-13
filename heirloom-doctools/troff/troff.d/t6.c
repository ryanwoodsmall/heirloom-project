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
 * Sccsid @(#)t6.c	1.83 (gritter) 9/13/05
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

#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <limits.h>
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
int	*pstab;
int	*cstab;
int	*ccstab;
int	**fallbacktab;
float	*zoomtab;
int	*bdtab;
int	**lhangtab;
int	**rhangtab;
int	**kernafter;
int	**kernbefore;
struct tracktab	*tracktab;
int	sbold = 0;
int	kern = 0;

int
width(register tchar j)
{
	register int i, k;

	lasttrack = 0;
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
	if (widcache[i-32].fontpts == xfont + (xpts<<8) && !setwdf)
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
	register int	*p;
	register int	x, j;
	int nocache = 0;
	int	ofont = xfont;
	int	s;
	float	z;

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
	if ((j = fitab[xfont][i]) == 0) {	/* it's not on current font */
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
				j = fitab[ii][i];
				if (j != 0) {
					xfont = ii;
					goto found;
				}
			}
		}
		if (smnt) {
			for (ii=smnt, jj=0; jj < nfonts; jj++, ii=ii % nfonts + 1) {
				j = fitab[ii][i];
				if (j != 0) {
					/*
					 * troff traditionally relies on the
					 * device postprocessor to find the
					 * appropriate character since it
					 * searches the fonts in the same
					 * order. This does not work with the
					 * new requests anymore, so change
					 * the font explicitly.
					 */
					if (xflag)
						xfont = ii;
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
	if (dev.anysize == 0 || xflag == 0 || (z = zoomtab[xfont]) == 0)
		z = 1;
	k *= z;
 g1:
	if (!bd)
		bd = bdtab[ofont];
	if (cs = cstab[ofont]) {
		nocache = 1;
		if (ccs = ccstab[ofont])
			x = pts2u(ccs); 
		else 
			x = xpts;
		cs = (cs * EMPTS(x)) / 36;
	}
	k = (k * u2pts(xpts) + (Unitwidth / 2)) / Unitwidth;
	s = xpts;
	lasttrack = 0;
	if (s <= tracktab[ofont].s1 && tracktab[ofont].n1) {
		nocache = 1;
		lasttrack = tracktab[ofont].n1;
	} else if (s >= tracktab[ofont].s2 && tracktab[ofont].n2) {
		nocache = 1;
		lasttrack = tracktab[ofont].n2;
	} else if (s > tracktab[ofont].s1 && s < tracktab[ofont].s2) {
		int	r;
		r = (s * tracktab[ofont].n2 - s * tracktab[ofont].n1
				+ tracktab[ofont].s2 * tracktab[ofont].n1
				- tracktab[ofont].s1 * tracktab[ofont].n2)
			/ (tracktab[ofont].s2 - tracktab[ofont].s1);
		if (r != 0) {
			nocache = 1;
			lasttrack = r;
		}
	}
	k += lasttrack;
	if (nocache|bd)
		widcache[i].fontpts = 0;
	else {
		widcache[i].fontpts = xfont + (xpts<<8);
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

	if (afmtab && (i = (fontbase[xfont]->afmpos) - 1) >= 0)
		return afmtab[i]->fitab[n-32];
	ncf = fontbase[xfont]->nwfont & BYTEMASK;
	for (i = 0; i < ncf; i++)
		if (codetab[xfont][i] == n)
			return i;
	return 0;
}

int
kernadjust(tchar c, tchar d)
{
	lastkern = 0;
	if (!kern || ismot(c) || ismot(d))
		return 0;
	c = trtab[cbits(c)] | c & SFMASK;
	d = trtab[cbits(d)] | d & SFMASK;
	return getkw(c, d);
}

#define	kprime		1021
#define	khash(c, d)	(((2654435769U * (c) * (d) >> 16)&0x7fffffff) % kprime)

static struct knode {
	struct knode	*next;
	tchar	c;
	tchar	d;
	int	n;
} **ktable;

static void
kadd(tchar c, tchar d, int n)
{
	struct knode	*kn;
	int	h;

	if (ktable == NULL)
		ktable = calloc(kprime, sizeof *ktable);
	h = khash(c, d);
	kn = calloc(1, sizeof *kn);
	kn->c = c;
	kn->d = d;
	kn->n = n;
	kn->next = ktable[h];
	ktable[h] = kn;
}

static void
kzap(int f)
{
	struct knode	*kp;
	int	i;

	if (ktable == NULL)
		return;
	for (i = 0; i < kprime; i++)
		for (kp = ktable[i]; kp; kp = kp->next)
			if (fbits(kp->c) == f || fbits(kp->d) == f)
				kp->n = INT_MAX;
}

static tchar
findchar(tchar c)
{
	int	f, i;

	f = fbits(c);
	c = cbits(c);
	i = c - 32;
	if (c != ' ' && i < nchtab + 128 - 32 && fitab[f][i] == 0) {
		int	ii, jj;
		if (fallbacktab[f]) {
			for (jj = 0; fallbacktab[f][jj] != 0; jj++) {
				if ((ii = findft(fallbacktab[f][jj])) < 0)
					continue;
				if (fitab[ii][i] != 0) {
					f = ii;
					goto found;
				}
			}
		}
		if (smnt) {
			for (ii=smnt, jj=0; jj < nfonts; jj++, ii=ii % nfonts + 1) {
				if (fitab[ii][i] != 0) {
					f = ii;
					goto found;
				}
			}
		}
		return 0;
	}
found:
	setfbits(c, f);
	return c;
}

static struct knode *
klook(tchar c, tchar d)
{
	struct knode	*kp;
	int	h;

	c = findchar(c);
	d = findchar(d);
	h = khash(c, d);
	for (kp = ktable[h]; kp; kp = kp->next)
		if (kp->c == c && kp->d == d)
			break;
	return kp && kp->n != INT_MAX ? kp : NULL;
}

int
getkw(tchar c, tchar d)
{
	struct knode	*kp;
	struct afmtab	*a;
	int	f, i, j, k, n, s;
	float	z;

	lastkern = 0;
	if (!kern || iszbit(c) || iszbit(d) || ismot(c) || ismot(d))
		return 0;
	if (sbits(c) != sbits(d))
		return 0;
	if ((f = fbits(c)) == 0)
		f = xfont;
	if (cstab[f])
		return 0;
	if ((s = sbits(c)) == 0)
		s = xpts;
	i = cbits(c);
	j = cbits(d);
	k = 0;
	if (i >= 32 && j >= 32) {
		if (ktable != NULL && (kp = klook(c, d)) != NULL)
			k = kp->n;
		else if (fbits(c) == fbits(d) && afmtab &&
				(n = (fontbase[f]->afmpos)-1) >= 0) {
			a = afmtab[n];
			k = afmgetkern(a, i - 32, j - 32);
		}
		if (j>32 && kernafter != NULL && kernafter[fbits(c)] != NULL)
			k += kernafter[fbits(c)][i];
		if (i>32 && kernbefore != NULL && kernbefore[fbits(d)] != NULL)
			k += kernbefore[fbits(d)][j];
	}
	if (k != 0) {
		k = (k * u2pts(s) + (Unitwidth / 2)) / Unitwidth;
		if (dev.anysize && xflag && (z = zoomtab[f]) != 0)
			k *= z;
	}
	lastkern = k;
	return k;
}

void
xbits(register tchar i, int bitf)
{
	register int k;

	xfont = fbits(i);
	k = sbits(i);
	if (k) {
		xpts = dev.anysize && xflag ? k : pstab[--k];
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
	struct namecache	*np;
	struct afmtab	*a;
	int	i;

	if (afmtab && (i = (fontbase[f]->afmpos) - 1) >= 0) {
		a = afmtab[i];
		np = afmnamelook(a, temp);
		if (np->afpos != 0) {
			if (np->fival[0] >= 0)
				return np->fival[0] + 32 + nchtab + 128;
			else if (np->fival[1] >= 0)
				return np->fival[1] + 32 + nchtab + 128;
			else
				return 0;
		}
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
	if (smnt) {
		for (i=smnt, j=0; j < nfonts; j++, i=i % nfonts + 1) {
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
	char	temp[NC];
	tchar	c;
	int	f, n;

	n = 0;
	do {
		c = getach();
		if (c == 0 && n < 2)
			return(0);
		if (n >= sizeof temp) {
			temp[n-1] = 0;
			break;
		}
		if (delim == '[' && c == ']') {
			temp[n] = 0;
			break;
		}
		temp[n++] = c;
		if (delim != '[' && n == 2) {
			temp[n] = 0;
			break;
		}
	} while (c);
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
		dfact = INCH;
		dfactd = 72;
		res = VERT;
		i = inumb(&apts);
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
	if (xflag == 0)
		i = pts2u(u2pts(i));
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

	if (dev.anysize && xflag) {
		if (i <= 0)
			i = pstab[0];
		return i;
	}
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
	if (dev.anysize && xflag)
		j = i - 1;
	else for (j = 0; i > (k = pstab[j]); j++)
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
			j = pts2u(j);
		} else		/* \sd */
			j = pts2u(i);
	} else if (i == '(') {		/* \s(dd */
		j = cbits(getch()) - '0';
		j = 10 * j + cbits(getch()) - '0';
		if (j == 0)		/* \s(00 */
			j = apts1;
		else
			j = pts2u(j);
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
		j = pts2u(j);
		j += apts;
	} else if (i == '\'' && xflag) {
		dfact = INCH;
		dfactd = 72;
		res = VERT;
		j = inumb(&apts);
		if (nonumb)
			return;
		getch();
	}
	casps1(j);
}


tchar setht(void)		/* set character height from \H'...' */
{
	int n;
	tchar c;

	getch();
	dfact = INCH;
	dfactd = 72;
	res = VERT;
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

	if (a) {
		if ((i = getrq()) >= 256)
			i = maybemore(i, 1);
	} else 
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
			emsz = POINT * u2pts(xpts);
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
	j = sabsmot(j) | MOT;
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
	int f;

	f = fbits(i);
	if ((lf = fontbase[f]->ligfont) == 0) /* font lacks ligatures */
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
				pbbuf[pbp++] = k;
				j = LIG_FF;
			}
		} else 
			j = LIG_FF;
	} else {
		pbbuf[pbp++] = j;
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
caseflig(void)
{
	int	i, j;

	skip();
	i = getrq();
	if ((j = findft(i)) < 0 || skip())
		return;
	fontbase[j]->ligfont = atoi() & 037;
	/*
	 * If the font still contains the charlib substitutes for ff,
	 * Fi, and Fl, hide them. The ".flig" request is intended for
	 * use in combination with expert fonts only.
	 */
	if (fontbase[j]->ligfont & LFF)
		if (codetab[j][fitab[j][LIG_FF-32]] < 32)
			fitab[j][LIG_FF-32] = 0;
	if (fontbase[j]->ligfont & LFFI)
		if (codetab[j][fitab[j][LIG_FFI-32]] < 32)
			fitab[j][LIG_FFI-32] = 0;
	if (fontbase[j]->ligfont & LFFL)
		if (codetab[j][fitab[j][LIG_FFL-32]] < 32)
			fitab[j][LIG_FFL-32] = 0;
}

void
casefp(void)
{
	register int i, j;
	char *file, *supply;

	skip();
	if ((i = xflag ? atoi() : cbits(getch()) - '0') < 0 || i > nfonts)
	bad:	errprint("fp: bad font position %d", i);
	else if (skip() || !(j = getrq()))
		errprint("fp: no font name");
	else {
		if (j >= 256)
			j = maybemore(j, 1);
		if (skip() || !getname()) {
			if (i == 0)
				goto bad;
			setfp(i, j, 0);
		} else {		/* 3rd argument = filename */
			file = malloc(strlen(nextf) + 1);
			strcpy(file, nextf);
			if (!skip() && getname()) {
				supply = malloc(strlen(nextf) + 1);
				strcpy(supply, nextf);
			} else
				supply = NULL;
			if (loadafm(i?i:-1, j, file, supply, 0) == 0) {
				if (i == 0)
					goto bad;
				setfp(i, j, file);
			}
			free(file);
			free(supply);
		}
	}
}

int
setfp(int pos, int f, char *truename)	/* mount font f at position pos[0...nfonts] */
{
	extern int snprintf(char *, size_t, const char *, ...);
	char longname[4096], *shortname, *ap;
	char *fpout;
	int nw;

	zapwcache(0);
	if (truename)
		shortname = truename;
	else
		shortname = macname(f);
	snprintf(longname, sizeof longname, "%s/dev%s/%s",
			fontfile, devname, shortname);
	if ((fpout = readfont(longname, &dev)) == NULL)
		return(-1);
	fontbase[pos] = (struct Font *)fpout;
	if ((ap = strstr(fontbase[pos]->namefont, ".afm")) != NULL) {
		*ap = 0;
		if (ap == &fontbase[pos]->namefont[1])
			f &= BYTEMASK;
		loadafm(pos, f, fontbase[pos]->namefont, NULL, 1);
		free(fpout);
	} else {
		nw = fontbase[pos]->nwfont & BYTEMASK;
		makefont(pos, &((char *)fontbase[pos])[sizeof(struct Font)],
			&((char *)fontbase[pos])[sizeof(struct Font) + nw],
			&((char *)fontbase[pos])[sizeof(struct Font) + 2*nw],
			&((char *)fontbase[pos])[sizeof(struct Font) + 3*nw],
			nw);
	}
	if (pos == smnt) {
		smnt = 0; 
		sbold = 0; 
	}
	if ((fontlab[pos] = f) == 'S')
		smnt = pos;
	bdtab[pos] = cstab[pos] = ccstab[pos] = 0;
	zoomtab[pos] = 0;
	fallbacktab[pos] = NULL;
	free(lhangtab[pos]);
	lhangtab[pos] = NULL;
	free(rhangtab[pos]);
	rhangtab[pos] = NULL;
	memset(&tracktab[pos], 0, sizeof tracktab[pos]);
	kzap(pos);
		/* if there is a directory, no place to store its name. */
		/* if position isn't zero, no place to store its value. */
		/* only time a FONTPOS is pushed back is if it's a */
		/* standard font on position 0 (i.e., mounted implicitly. */
		/* there's a bug here:  if there are several input lines */
		/* that look like .ft XX in short successtion, the output */
		/* will all be in the last one because the "x font ..." */
		/* comes out too soon.  pushing back FONTPOS doesn't work */
		/* with .ft commands because input is flushed after .xx cmds */
	if (realpage && ap == NULL)
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
		pbbuf[pbp++] = MOT | VMOT | sabsmot(i);
	else
		pbbuf[pbp++] = MOT | VMOT | NMOT | sabsmot(-i);
	return(HX);
}

struct afmtab **afmtab;
int nafm;

char *
onefont(char *prefix, char *file, char *type)
{
	char	*path, *fp, *tp;

	path = malloc(strlen(prefix) + strlen(file) + 2);
	strcpy(path, prefix);
	strcat(path, "/");
	strcat(path, file);
	if (type) {
		for (fp = file; *fp; fp++);
		for (tp = type; *tp; tp++);
		while (tp >= type && fp >= file && *fp-- == *tp--);
		if (tp >= type) {
			tp = malloc(strlen(path) + strlen(type) + 2);
			strcpy(tp, path);
			strcat(tp, ".");
			strcat(tp, type);
			free(path);
			path = tp;
		}
	}
	return path;
}

static char *
getfontpath(char *file, char *type)
{
	extern int sprintf(char *, const char *, ...);
	char	*path, *troffonts, *tp, *tq, c;

	if ((troffonts = getenv("TROFFONTS")) != NULL) {
		tp = malloc(strlen(troffonts) + 1);
		strcpy(tp, troffonts);
		troffonts = tp;
		do {
			for (tq = tp; *tq && *tq != ':'; tq++);
			c = *tq;
			*tq = 0;
			path = onefont(tp, file, type);
			if (access(path, 0) == 0) {
				free(troffonts);
				return path;
			}
			free(path);
			tp = &tq[1];
		} while (c);
		free(troffonts);
	}
	tp = malloc(strlen(fontfile) + strlen(devname) + 10);
	sprintf(tp, "%s/dev%s", fontfile, devname);
	path = onefont(tp, file, type);
	free(tp);
	return path;
}

int
loadafm(int nf, int rq, char *file, char *supply, int required)
{
	extern int sprintf(char *, const char *, ...);
	struct stat	st;
	int	fd;
	char	*path, *contents;
	struct afmtab	*a;
	int	i, have = 0;

	if (nf < 0 || nf > nfonts)
		nf = nfonts + 1;
	path = getfontpath(file, "afm");
	a = calloc(1, sizeof *a);
	for (i = 0; i < nafm; i++)
		if (strcmp(afmtab[i]->path, path) == 0) {
			*a = *afmtab[i];
			have = 1;
			break;
		}
	a->path = path;
	a->file = malloc(strlen(file) + 1);
	strcpy(a->file, file);
	a->rq = rq;
	a->Font.namefont[0] = rq&0377;
	a->Font.namefont[1] = (rq>>8)&0377;
	sprintf(a->Font.intname, "%d", nf);
	if (have)
		goto done;
	if ((fd = open(path, O_RDONLY)) < 0) {
		if (required)
			errprint("Can't open %s", path);
		free(a->file);
		free(a);
		free(path);
		return 0;
	}
	if (fstat(fd, &st) < 0) {
		errprint("Can't stat %s", path);
		free(a->file);
		free(a);
		free(path);
		return -1;
	}
	contents = malloc(st.st_size + 1);
	if (read(fd, contents, st.st_size) != st.st_size) {
		errprint("Can't read %s", path);
		free(a->file);
		free(a);
		free(path);
		free(contents);
		return -1;
	}
	contents[st.st_size] = 0;
	close(fd);
	if (afmget(a, contents, st.st_size) < 0) {
		free(path);
		free(contents);
		return -1;
	}
	free(contents);
	morechars(a->nchars+32+1+128-32+nchtab+32+nchtab+128);
done:	afmtab = realloc(afmtab, (nafm+1) * sizeof *afmtab);
	afmtab[nafm] = a;
	if (nf >= Nfont)
		growfonts(nf+1);
	a->Font.afmpos = nafm+1;
	fontbase[nf] = &afmtab[nafm]->Font;
	fontlab[nf] = rq;
	free(fontab[nf]);
	free(kerntab[nf]);
	free(codetab[nf]);
	free(fitab[nf]);
	fontab[nf] = malloc(a->nchars * sizeof *fontab[nf]);
	kerntab[nf] = malloc(a->nchars * sizeof *kerntab[nf]);
	codetab[nf] = malloc(a->nchars * sizeof *codetab[nf]);
	fitab[nf] = malloc((a->nchars+128-32+nchtab) * sizeof *fitab[nf]);
	memcpy(fontab[nf], a->fontab, a->nchars * sizeof *fontab[nf]);
	memcpy(kerntab[nf], a->kerntab, a->nchars * sizeof *kerntab[nf]);
	memcpy(codetab[nf], a->codetab, a->nchars * sizeof *codetab[nf]);
	memcpy(fitab[nf], a->fitab, (a->nchars+128-32+nchtab) *
			sizeof *fitab[nf]);
	bdtab[nf] = cstab[nf] = ccstab[nf] = 0;
	zoomtab[nf] = 0;
	fallbacktab[nf] = NULL;
	free(lhangtab[nf]);
	lhangtab[nf] = NULL;
	free(rhangtab[nf]);
	rhangtab[nf] = NULL;
	memset(&tracktab[nf], 0, sizeof tracktab[nf]);
	kzap(nf);
	nafm++;
	if (nf > nfonts)
		nfonts = nf;
	if (supply) {
		char	*data;
		if (strcmp(supply, "pfb") == 0 || strcmp(supply, "pfa") == 0 ||
				strcmp(supply, "t42") == 0)
			data = getfontpath(file, supply);
		else
			data = getfontpath(supply, NULL);
		ptsupplyfont(a->fontname, data);
		free(data);
	}
	if (realpage)
		ptfpcmd(nf, a->path);
	return 1;
}

int
tracknum(void)
{
	skip();
	dfact = INCH;
	dfactd = 72;
	res = VERT;
	return inumb(NULL);
}

void
casetrack(void)
{
	int	i, j, s1, n1, s2, n2;

	skip();
	i = getrq();
	if ((j = findft(i)) < 0)
		return;
	s1 = tracknum();
	if (!nonumb) {
		n1 = tracknum();
		if (!nonumb) {
			s2 = tracknum();
			if (!nonumb) {
				n2 = tracknum();
				if (!nonumb) {
					tracktab[j].s1 = s1;
					tracktab[j].n1 = n1;
					tracktab[j].s2 = s2;
					tracktab[j].n2 = n2;
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

void
casehidechar(void)
{
	int	i, j, n, m;
	tchar	k;

	skip();
	i = getrq();
	if ((j = findft(i)) < 0)
		return;
	while ((i = cbits(k = getch())) != '\n') {
		if (fbits(k) != xfont || ismot(k) || i == ' ')
			continue;
		n = 128 - 32 + nchtab;
		if (afmtab && (m=(fontbase[xfont]->afmpos)-1) >= 0)
			n += afmtab[m]->nchars;
		if (i < n)
			fitab[xfont][i - 32] = 0;
	}
	zapwcache(0);
}

void
casefzoom(void)
{
	char	*buf = NULL, *bp;
	int	c, i, j;
	int	n = 0, sz = 0;
	float	f;

	skip();
	i = getrq();
	if ((j = findft(i)) < 0)
		return;
	skip();
	do {
		c = getach();
		if (n >= sz)
			buf = realloc(buf, (sz += 8) * sizeof *buf);
		buf[n++] = c;
	} while (c);
	f = strtod(buf, &bp);
	if (*bp == '\0' && f >= 0) {
		zoomtab[j] = f;
		zapwcache(0);
		if (realpage && j == xfont)
			ptps();
	}
	free(buf);
}

void
casekern(void)
{
	kern = skip() || atoi() ? 1 : 0;
}

void
casepapersize(void)
{
	const struct {
		char	*name;
		int	width;
		int	heigth;
	} papersizes[] = {
		{ "executive",	 518,	 756 },
		{ "letter",	 612,	 792 },
		{ "legal",	 612,	 992 },
		{ "ledger",	1224,	 792 },
		{ "tabloid",	 792,	1224 },
		{ "a0",		2384,	3370 },
		{ "a1",		1684,	2384 },
		{ "a2",		1191,	1684 },
		{ "a3",		 842,	1191 },
		{ "a4",		 595,	 842 },
		{ "a5",		 420,	 595 },
		{ "a6",		 298,	 420 },
		{ "a7",		 210,	 298 },
		{ "a8",		 147,	 210 },
		{ "a9",		 105,	 147 },
		{ "a10",	  74,	 105 },
		{ "b0",		2835,	4008 },
		{ "b1",		2004,	2835 },
		{ "b2",		1417,	2004 },
		{ "b3",		1000,	1417 },
		{ "b4",		 709,	1000 },
		{ "b5",		 499,	 709 },
		{ "b6",		 354,	 499 },
		{ "b7",		 249,	 354 },
		{ "b8",		 176,	 249 },
		{ "b9",		 125,	 176 },
		{ "b10",	  87,	 125 },
		{ "c0",		2599,	3677 },
		{ "c1",		1837,	2599 },
		{ "c2",		1298,	1837 },
		{ "c3",		 918,	1298 },
		{ "c4",		 649,	 918 },
		{ "c5",		 459,	 649 },
		{ "c6",		 323,	 459 },
		{ "c7",		 230,	 323 },
		{ "c8",		 162,	 230 },
		{ NULL,		   0,	   0 }
	};
	char	c;
	int	x = 0, y = 0, n;
	char	buf[NC];

	if (skip())
		return;
	c = cbits(ch);
	if (isdigit(c) || c == '(') {
		x = atoi();
		if (!nonumb) {
			skip();
			y = atoi();
		}
		if (nonumb || x == 0 || y == 0)
			return;
	} else {
		n = 0;
		do {
			c = getach();
			if (n+1 < sizeof buf)
				buf[n++] = c;
		} while (c);
		buf[n] = 0;
		for (n = 0; papersizes[n].name != NULL; n++)
			if (strcmp(buf, papersizes[n].name) == 0) {
				x = papersizes[n].width * INCH / 72;
				y = papersizes[n].heigth * INCH / 72;
				break;
			}
		if (x == 0 || y == 0) {
			errprint("Unknown paper size %s", buf);
			return;
		}
	}
	pl = defaultpl = y;
	if (numtab[NL].val > pl)
		numtab[NL].val = pl;
	po = x > 6 * PO ? PO : x / 8;
	ll = ll1 = lt = lt1 = x - 2 * po;
	setnel();
	ptpapersize(x, y);
}

static void
hang(int **tp)
{
	int	i, j, n;
	int	savfont = font, savfont1 = font1;
	tchar	k;

	skip();
	i = getrq();
	if ((j = findft(i)) < 0)
		return;
	font = font1 = j;
	mchbits();
	while (!skip() && (i = cbits(k = getch())) != '\n' && !skip()) {
		noscale++;
		n = atoi();
		noscale--;
		if (fbits(k) == j && !ismot(k)) {
			n = unitconv(n);
			if (tp[j] == NULL)
				tp[j] = calloc(NCHARS, sizeof *tp);
			tp[j][i] = n;
		}
	}
	font = savfont;
	font1 = savfont1;
	mchbits();
}

void
caselhang(void)
{
	hang(lhangtab);
}

void
caserhang(void)
{
	hang(rhangtab);
}

void
casekernpair(void)
{
	int	savfont = font, savfont1 = font1;
	int	f, g, i, n;
	tchar	c, d;

	skip();
	i = getrq();
	if ((f = findft(i)) < 0)
		return;
	font = font1 = f;
	mchbits();
	if (skip())
		goto done;
	c = getch();
	if (fbits(c) != f || skip())
		goto done;
	i = getrq();
	if ((g = findft(i)) < 0)
		goto done;
	font = font1 = g;
	mchbits();
	if (skip())
		goto done;
	d = getch();
	if (fbits(d) != g || skip())
		goto done;
	noscale++;
	n = atoi();
	noscale--;
	if ((c = cbits(c)) == 0)
		goto done;
	if (c == UNPAD)
		c = ' ';
	setfbits(c, f);
	if ((d = cbits(d)) == 0)
		goto done;
	if (d == UNPAD)
		d = ' ';
	setfbits(d, g);
	n = unitconv(n);
	kadd(c, d, n);
done:
	font = savfont;
	font1 = savfont1;
	mchbits();
}

static void
kernsingle(int **tp)
{
	int     savfont = font, savfont1 = font1;
	int	f, i, n;
	tchar	c;

	skip();
	i = getrq();
	if ((f = findft(i)) < 0)
		return;
	font = font1 = f;
	mchbits();
	while (!skip()) {
		c = getch();
		if (skip())
			break;
		noscale++;
		n = atoi();
		noscale--;
		if (fbits(c) != f || (c = cbits(c)) <= 32)
			continue;
		if (tp[f] == NULL)
			tp[f] = calloc(NCHARS, sizeof *tp);
		tp[f][c] = unitconv(n);
	}
	font = savfont;
	font1 = savfont1;
	mchbits();
}

void
casekernafter(void)
{
	kernsingle(kernafter);
}

void
casekernbefore(void)
{
	kernsingle(kernbefore);
}

#include "unimap.h"

int
un2tr(int c, int *fp)
{
	int	i, j;

	for (i = 0; unimap[i].psc; i++)
		if (unimap[i].code == c)
			if ((j = postchar(unimap[i].psc, fp)) != 0)
				return j;
	return 0;
}

int
tr2un(tchar i, int f)
{
	struct afmtab	*a;
	int	c, n;

	if (i < 32)
		return -1;
	else if (i < 128)
		return i;
	if ((n = (fontbase[f]->afmpos) - 1) >= 0) {
		a = afmtab[n];
		if (i - 32 >= nchtab + 128)
			i -= nchtab + 128;
		if ((n = a->fitab[i - 32]) < a->nchars &&
				a->nametab[n] != NULL)
			for (c = 0; unimap[c].psc; c++)
				if (strcmp(unimap[c].psc, a->nametab[n]) == 0)
					return unimap[c].code;
	}
	return -1;
}

int
pts2u(int p)
{
	return p * INCH / 72;
}

double
u2pts(int u)
{
	return u * 72.0 / INCH;
}
