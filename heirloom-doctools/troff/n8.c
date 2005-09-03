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


/*	from OpenSolaris "n8.c	1.8	05/06/08 SMI"	*/

/*
 * Portions Copyright (c) 2005 Gunnar Ritter, Freiburg i. Br., Germany
 *
 * Sccsid @(#)n8.c	1.13 (gritter) 9/3/05
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

#ifdef	EUC
#include	<wctype.h>
#endif
#include	<ctype.h>
#include	<stdlib.h>
#include	<string.h>
#include	"tdef.h"
#include "ext.h"
#include "proto.h"
#include "libhnj/hyphen.h"
#define	HY_BIT	0200	/* generic stuff in here only works for ascii */
#define	HY_BIT2	0x80000000

/*
 * troff8.c
 * 
 * hyphenation
 */

int	*hbuf;
int	NHEX;
int	*nexth;
tchar	*hyend;
#define THRESH 160 /*digram goodness threshold*/
int	thresh = THRESH;

static	HyphenDict	*dicthnj;

static	void		hyphenhnj(void);

static int *
growhbuf(int **pp)
{
	int	*nhbuf;
	int	inc = 4;

	if ((nhbuf = realloc(hbuf, (NHEX+inc) * sizeof *hbuf)) == NULL)
		return NULL;
	NHEX += inc;
	nexth += nhbuf - hbuf;
	if (pp)
		*pp += nhbuf - hbuf;
	return hbuf = nhbuf;
}

void
hyphen(tchar *wp)
{
	register int j;
	register tchar *i;

	i = wp;
	while (punct(*i++))
		;
	if (!alph(*--i))
		return;
	wdstart = i++;
	while (alph(*i++))
		;
	hyend = wdend = --i - 1;
	while (punct(*i++))
		;
	if (*--i)
		return;
	if ((wdend - wdstart - 4) < 0)
		return;
	hyp = hyptr;
	*hyp = 0;
	hyoff = 2;
	if (dicthnj) {
		if (!exword())
			hyphenhnj();
	} else if (!exword() && !suffix())
		digram();
	*hyp++ = 0;
	if (*hyptr) 
		for (j = 1; j; ) {
			j = 0;
			for (hyp = hyptr + 1; *hyp != 0; hyp++) {
				if (*(hyp - 1) > *hyp) {
					j++;
					i = *hyp;
					*hyp = *(hyp - 1);
					*(hyp - 1) = i;
				}
			}
		}
}


int 
punct(int i)
{
	if (!cbits(i) || alph(i))
		return(0);
	else
		return(1);
}


int 
alph(int j)
{
	int i = cbits(j);
#ifndef	NROFF
#ifdef	EUC
	if (!ismot(j) && i & ~0177) {
		int	u = tr2un(i, fbits(j));
		return iswalpha(u);
	} else
#endif	/* EUC */
#endif	/* !NROFF */
	if (!ismot(j) && i >= 'a' && i <= 'z' || i >= 'A' && i <= 'Z')
		return(1);
	else
		return(0);
}


void
caseht(void)
{
	thresh = THRESH;
	if (skip())
		return;
	noscale++;
	thresh = atoi();
	noscale = 0;
}


void
casehw(void)
{
	register int i, k;
	int	*j;
	tchar t;

	if (nexth == NULL)
		growhbuf(NULL);
	k = 0;
	while (!skip()) {
		if ((j = nexth) >= (hbuf + NHEX - 2) && growhbuf(&j) == NULL)
			goto full;
		for (; ; ) {
			if (ismot(t = getch()))
				continue;
			i = cbits(t);
			if (i == ' ' || i == '\n') {
				*j++ = 0;
				nexth = j;
				*j = 0;
				if (i == ' ')
					break;
				else
					return;
			}
			if (i == '-') {
				k = HY_BIT2;
				continue;
			}
			*j++ = maplow(i, xfont) | k;
			k = 0;
			if (j >= (hbuf + NHEX - 2) && growhbuf(&j) == NULL)
				goto full;
		}
	}
	return;
full:
	errprint("exception word list full.");
	*nexth = 0;
}


int 
exword(void)
{
	register tchar *w;
	register int	*e;
	int	*save;

	e = hbuf;
	while (1) {
		save = e;
		if (e == NULL || *e == 0)
			return(0);
		w = wdstart;
		while (*e && w <= hyend &&
				(*e&~HY_BIT2) == maplow(cbits(*w), fbits(*w))) {
			e++; 
			w++;
		};
		if (!*e) {
			if (w-1 == hyend || (w == wdend &&
						maplow(cbits(*w), fbits(*w))
						== 's')) {
				w = wdstart;
				for (e = save; *e; e++) {
					if (*e & HY_BIT2)
						*hyp++ = w;
					if (hyp > (hyptr + NHYP - 1))
						hyp = hyptr + NHYP - 1;
					w++;
				}
				return(1);
			} else {
				e++; 
				continue;
			}
		} else 
			while (*e++)
				;
	}
}


int 
suffix(void)
{
	register tchar *w;
	register const char	*s, *s0;
	tchar i;
	extern const char	*suftab[];

again:
	i = cbits(*hyend);
	if (i >= 128 || !alph(*hyend))
		return(0);
	if (i < 'a')
		i -= 'A' - 'a';
	if ((s0 = suftab[i-'a']) == 0)
		return(0);
	for (; ; ) {
		if ((i = *s0 & 017) == 0)
			return(0);
		s = s0 + i - 1;
		w = hyend - 1;
		while (s > s0 && w >= wdstart &&
				(*s & 0177) == maplow(cbits(*w), fbits(*w))) {
			s--;
			w--;
		}
		if (s == s0)
			break;
		s0 += i;
	}
	s = s0 + i - 1;
	w = hyend;
	if (*s0 & HY_BIT) 
		goto mark;
	while (s > s0) {
		w--;
		if (*s-- & HY_BIT) {
mark:
			hyend = w - 1;
			if (*s0 & 0100)
				continue;
			if (!chkvow(w))
				return(0);
			*hyp++ = w;
		}
	}
	if (*s0 & 040)
		return(0);
	if (exword())
		return(1);
	goto again;
}


int 
maplow(register int i, int f)
{
#ifndef	NROFF
#ifdef	EUC
	if (!ismot(i) && i & ~0177) {
		i = tr2un(i, f);
		if (iswupper(i))
			i = towlower(i);
	} else
#endif	/* EUC */
#endif	/* !NROFF */
	if (ischar(i) && isupper(i)) 
		i = tolower(i);
	return(i);
}


int 
vowel(int i)
{
	switch (maplow(i, xfont)) {
	case 'a':
	case 'e':
	case 'i':
	case 'o':
	case 'u':
	case 'y':
		return(1);
	default:
		return(0);
	}
}


tchar *
chkvow(tchar *w)
{
	while (--w >= wdstart)
		if (vowel(cbits(*w)))
			return(w);
	return(0);
}


void
digram(void) 
{
	register tchar *w;
	register int val;
	tchar * nhyend, *maxw = 0;
	int	maxval;
	extern const char	bxh[26][13], bxxh[26][13], xxh[26][13], xhx[26][13], hxx[26][13];

	for (w = wdstart; w <= wdend; w++)
		if (cbits(*w) & ~0177)
			return;

again:
	if (!(w = chkvow(hyend + 1)))
		return;
	hyend = w;
	if (!(w = chkvow(hyend)))
		return;
	nhyend = w;
	maxval = 0;
	w--;
	while ((++w < hyend) && (w < (wdend - 1))) {
		val = 1;
		if (w == wdstart)
			val *= dilook('a', cbits(*w), bxh);
		else if (w == wdstart + 1)
			val *= dilook(cbits(*(w-1)), cbits(*w), bxxh);
		else 
			val *= dilook(cbits(*(w-1)), cbits(*w), xxh);
		val *= dilook(cbits(*w), cbits(*(w+1)), xhx);
		val *= dilook(cbits(*(w+1)), cbits(*(w+2)), hxx);
		if (val > maxval) {
			maxval = val;
			maxw = w + 1;
		}
	}
	hyend = nhyend;
	if (maxval > thresh)
		*hyp++ = maxw;
	goto again;
}


int 
dilook(int a, int b, const char t[26][13])
{
	register int i, j;

	i = t[maplow(a, xfont)-'a'][(j = maplow(b, xfont)-'a')/2];
	if (!(j & 01))
		i >>= 4;
	return(i & 017);
}

void
casehylang(void)
{
	extern int sprintf(char *, const char *, ...);
	int	c, i = 0, sz = 0;
	char	*file = NULL, *path = NULL;

	if (dicthnj)
		hnj_hyphen_free(dicthnj);
	dicthnj = NULL;
	skip();
	do {
		c = getach();
		if (i >= sz)
			file = realloc(file, (sz += 8) * sizeof *file);
		file[i++] = c;
	} while (c);
	if (i == 1) {
		free(file);
		return;
	}
	if (strchr(file, '/') == NULL) {
		path = malloc(strlen(file) + strlen(HYPDIR) + 12);
		sprintf(path, "%s/hyph_%s.dic", HYPDIR, file);
	} else {
		path = malloc(strlen(file) + 1);
		strcpy(path, file);
	}
	if ((dicthnj = hnj_hyphen_load(path)) == NULL) {
		errprint("Can't load %s", path);
		free(file);
		free(path);
		return;
	}
	free(file);
	free(path);
}

static void
hyphenhnj(void)
{
	tchar	*wp;
	char	cb[3*WDSIZE+1], *cp, hb[3*WDSIZE+1];
	int	wpos[3*WDSIZE+1], *wpp;
	int	i, j, m;

	cp = cb;
	wpp = wpos;
	for (wp = wdstart; wp <= wdend; wp++)
		if (cp < &cb[sizeof cb - 1]) {
			m = cbits(*wp);
			if (m == LIG_FI) {
				*cp++ = 'f';
				*wpp++ = wp - wdstart;
				*cp++ = 'i';
				*wpp++ = -1;
			} else if (m == LIG_FL) {
				*cp++ = 'f';
				*wpp++ = wp - wdstart;
				*cp++ = 'l';
				*wpp++ = -1;
			} else if (m == LIG_FF) {
				*cp++ = 'f';
				*wpp++ = wp - wdstart;
				*cp++ = 'f';
				*wpp++ = -1;
			} else if (m == LIG_FFI) {
				*cp++ = 'f';
				*wpp++ = wp - wdstart;
				*cp++ = 'f';
				*wpp++ = -1;
				*cp++ = 'i';
				*wpp++ = -1;
			} else if (m == LIG_FFL) {
				*cp++ = 'f';
				*wpp++ = wp - wdstart;
				*cp++ = 'f';
				*wpp++ = -1;
				*cp++ = 'l';
				*wpp++ = -1;
			} else {
#ifdef	NROFF
				if (m & ~0177)
					return;
#else
				m = tr2un(m, fbits(*wp));
				if (m < 0 || m & ~0377)
					/* only supporting ISO-8859-1 so far */
					return;
#endif
				*cp++ = m;
				*wpp++ = wp - wdstart;
			}
		}
	*cp = '\0';
	j = cp - cb;
	hnj_hyphen_hyphenate(dicthnj, cb, j, hb);
	for (i = 0; i < j; i++)
		if (hb[i] - '0' & 1 && wpos[i+1] >= 0) {
			*hyp++ = &wdstart[wpos[i+1]];
			if (hyp > (hyptr + NHYP - 1))
				hyp = hyptr + NHYP - 1;
		}
}
