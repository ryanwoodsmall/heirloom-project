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
 * Sccsid @(#)afm.c	1.5 (gritter) 8/17/05
 */

#include <stdlib.h>
#include <string.h>
#include "dev.h"
#include "afm.h"

extern	struct dev	dev;
extern	char		*chname;
extern	short		*chtab;
extern	int		nchtab;

extern	void	errprint(char *, ...);

const struct names {
	char	*trname;
	char	*psname;
} names[] = {
	{ "hy",	"hyphen" },
	{ "ct",	"cent" },
	{ "fi",	"fi" },
	{ "fl",	"fl" },
	{ "dg",	"dagger" },
	{ "bu",	"bullet" },
	{ "de",	"ring" },
	{ "em",	"emdash" },
	{ "en",	"endash" },
	{ "sc",	"paragraph" },
	{ "``",	"quotedblleft" },
	{ "''",	"quotedblright" },
	{ 0,	0 }
};

static int
mapname(const char *psname)
{
	int	i, j;

	for (i = 0; names[i].trname; i++)
		if (strcmp(names[i].psname, psname) == 0)
			break;
	if (names[i].trname)
		for (j = 0; j < nchtab; j++)
			if (strcmp(names[i].trname, &chname[chtab[j]]) == 0)
				return j + 128;
	return 0;
}

/*
 * After all characters have been read, construct a font-specific
 * encoding for the rest.
 */
static void
remap(struct afmtab *a)
{
	int	i, j = 128 - 32;

	for (i = 1; i < a->nchars; i++) {
		if (a->codetab[i] == -1 && a->nametab[i] != NULL) {
			while (a->fitab[j] != 0)
				j++;
			a->fitab[j] = i;
		}
	}
}

static char *
thisword(const char *text, const char *wrd)
{
	while (*text == *wrd)
		text++, wrd++;
	if (*wrd != 0)
		return 0;
	if (*text == 0 || *text == ' ' || *text == '\t' || *text == '\n' ||
			*text == '\r') {
		while (*text != 0 && (*text == ' ' || *text == '\t'))
			text++;
		return (char *)text;
	}
	return NULL;
}

static void
addchar(struct afmtab *a, int C, int tp, int WX, int B[4], char *N)
{
	a->fontab = realloc(a->fontab, (a->nchars+1) * sizeof *a->fontab);
	a->kerntab = realloc(a->kerntab, (a->nchars+1) * sizeof *a->kerntab);
	a->codetab = realloc(a->codetab, (a->nchars+1) * sizeof *a->codetab);
	a->nametab = realloc(a->nametab, (a->nchars+1) * sizeof *a->nametab);
	a->fontab[a->nchars] = WX / dev.unitwidth +
		(WX % dev.unitwidth >= dev.unitwidth / 2);
	if (B[1] < -dev.unitwidth)
		a->kerntab[a->nchars] |= 1;
	if (B[3] > a->capheight)
		a->kerntab[a->nchars] |= 2;
	a->codetab[a->nchars] = C;
	if (tp >= 32)
		a->fitab[tp - 32] = a->nchars;
	if (N) {
		a->nametab[a->nchars] = malloc(strlen(N) + 1);
		strcpy(a->nametab[a->nchars], N);
	} else
		a->nametab[a->nchars] = 0;
	a->nchars++;
}

static void
addmetrics(struct afmtab *a, char *_line)
{
	char	*lp = _line, c, *xp;
	int	C = -1, WX = 0, tp;
	char	*N = NULL;
	int	B[4] = { -1, -1, -1, -1 };

	while (*lp && *lp != '\n' && *lp != '\r') {
		switch (*lp) {
		case 'C':
			C = strtol(&lp[1], NULL, 10);
			break;
		case 'W':
			if (lp[1] == 'X')
				WX = strtol(&lp[2], NULL, 10);
			break;
		case 'N':
			for (N = &lp[1]; *N == ' ' || *N == '\t'; N++);
			for (lp = N; *lp && *lp != '\n' && *lp != '\r' &&
					*lp != ' ' && *lp != '\t' &&
					*lp != ';'; lp++);
			c = *lp;
			*lp++ = 0;
			if (c == ';')
				continue;
			break;
		case 'B':
			xp = &lp[1];
			B[0] = strtol(xp, &xp, 10);
			B[1] = strtol(xp, &xp, 10);
			B[2] = strtol(xp, &xp, 10);
			B[3] = strtol(xp, &xp, 10);
			break;
		case 'L':
			if (C == 'f') {
				xp = &lp[1];
				while (*xp == ' ' || *xp == '\t')
					xp++;
				switch (*xp) {
				case 'i':
					a->Font.ligfont |= LFI;
					break;
				case 'l':
					a->Font.ligfont |= LFL;
					break;
				}
			}
			break;
		default:
			lp++;
		}
		while (*lp && *lp != '\n' && *lp != '\r' && *lp != ';')
			lp++;
		if (*lp == ';') {
			while (*lp && *lp != '\n' && *lp != '\r' &&
					(*lp == ' ' || *lp == '\t' ||
					 *lp == ';'))
				lp++;
		}
	}
	if (N == NULL)
		return;
	if (C < 0 || C > 32)
		addchar(a, C, C, WX, B, N);
	if ((tp = mapname(N)) != 0)
		addchar(a, C, tp, WX, B, NULL);
}

int
afmget(struct afmtab *a, char *contents, size_t size)
{
	enum {
		NONE,
		FONTMETRICS,
		CHARMETRICS
	} state = NONE;
	char	*cp, *th, *tp;
	int	n = 0;

	a->lineno = 1;
	for (cp = contents; cp < &contents[size]; a->lineno++, cp++) {
		while (*cp == ' ' || *cp == '\t' || *cp == '\r')
			cp++;
		if (*cp == '\n')
			continue;
		if (state == NONE && thisword(cp, "StartFontMetrics"))
			state = FONTMETRICS;
		else if (state == FONTMETRICS && thisword(cp, "EndFontMetrics"))
			state = NONE;
		else if (state == FONTMETRICS &&
				(th = thisword(cp, "FontName")) != NULL) {
			for (tp = th; *tp && *tp != ' ' && *tp != '\t' &&
					*tp != '\n' && *tp != '\r'; tp++);
			a->fontname = malloc(tp - th + 1);
			memcpy(a->fontname, th, tp - th);
			a->fontname[tp - th] = 0;
		} else if (state == FONTMETRICS &&
				(th = thisword(cp, "CapHeight")) != NULL) {
			a->capheight = strtol(th, NULL, 10);
		} else if (state == FONTMETRICS &&
				(th = thisword(cp, "StartCharMetrics")) != 0) {
			n = strtol(th, NULL, 10);
			state = CHARMETRICS;
			a->fitab = calloc(n + 128 - 32 + nchtab,
					sizeof *a->fitab);
			a->fontab = malloc(sizeof *a->fontab);
			a->fontab[0] = dev.res * dev.unitwidth / 72 / 3;
			a->kerntab = malloc(sizeof *a->kerntab);
			a->kerntab[0] = 0;
			a->codetab = malloc(sizeof *a->codetab);
			a->codetab[0] = 0;
			a->nametab = malloc(sizeof *a->nametab);
			a->nametab[0] = 0;
			a->nchars = 1;
		} else if (state == CHARMETRICS && n-- > 0) {
			addmetrics(a, cp);
		} else if (state == CHARMETRICS &&
				thisword(cp, "EndCharMetrics"))
			state = FONTMETRICS;
		while (cp < &contents[size] && *cp != '\n')
			cp++;
	}
	if (a->fontname == NULL) {
		errprint("Missing \"FontName\" in %s", a->path);
		return -1;
	}
	remap(a);
	a->Font.nwfont = a->nchars > 255 ? 255 : a->nchars;
	return 0;
}
