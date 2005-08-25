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
 * Sccsid @(#)n8.c	1.6 (gritter) 8/25/05
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

#include	<ctype.h>
#include	<stdlib.h>
#include	"tdef.h"
#include "ext.h"
#define	HY_BIT	0200	/* stuff in here only works for ascii */

/*
 * troff8.c
 * 
 * hyphenation
 */

char	*hbuf;
int	NHEX;
char	*nexth;
tchar	*hyend;
#define THRESH 160 /*digram goodness threshold*/
int	thresh = THRESH;

static char *
growhbuf(char **pp)
{
	char	*nhbuf;
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
	while (punct(cbits(*i++)))
		;
	if (!alph(cbits(*--i)))
		return;
	wdstart = i++;
	while (alph(cbits(*i++)))
		;
	hyend = wdend = --i - 1;
	while (punct(cbits(*i++)))
		;
	if (*--i)
		return;
	if ((wdend - wdstart - 4) < 0)
		return;
	hyp = hyptr;
	*hyp = 0;
	hyoff = 2;
	if (!exword() && !suffix())
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
	if (!i || alph(i))
		return(0);
	else
		return(1);
}


int 
alph(int i)
{
	if (i >= 'a' && i <= 'z' || i >= 'A' && i <= 'Z')
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
	char	*j;
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
				k = HY_BIT;
				continue;
			}
			*j++ = maplow(i) | k;
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
	register char	*e;
	char	*save;

	e = hbuf;
	while (1) {
		save = e;
		if (e == NULL || *e == 0)
			return(0);
		w = wdstart;
		while (*e && w <= hyend && (*e & 0177) == maplow(cbits(*w))) {
			e++; 
			w++;
		};
		if (!*e) {
			if (w-1 == hyend || (w == wdend && maplow(cbits(*w)) == 's')) {
				w = wdstart;
				for (e = save; *e; e++) {
					if (*e & HY_BIT)
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
	if (!alph(cbits(i = cbits(*hyend))))
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
		while (s > s0 && w >= wdstart && (*s & 0177) == maplow(cbits(*w))) {
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
maplow(register int i)
{
	if (ischar(i) && isupper(i)) 
		i = tolower(i);
	return(i);
}


int 
vowel(int i)
{
	switch (maplow(i)) {
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

	i = t[maplow(a)-'a'][(j = maplow(b)-'a')/2];
	if (!(j & 01))
		i >>= 4;
	return(i & 017);
}


