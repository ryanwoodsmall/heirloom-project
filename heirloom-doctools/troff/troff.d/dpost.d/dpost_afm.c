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
 * Sccsid @(#)afm.c	1.6 (gritter) 8/18/05
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

static const struct asciimap {
	int		code;
	const char	*psc;
} asciimap[] = {
	{ 0x0020,	"space" },
	{ 0x0021,	"exclam" },
	{ 0x0022,	"quotedbl" },
	{ 0x0023,	"numbersign" },
	{ 0x0024,	"dollar" },
	{ 0x0025,	"percent" },
	{ 0x0026,	"ampersand" },
	{ 0x0027,	"quoteright" },
	{ 0x0028,	"parenleft" },
	{ 0x0029,	"parenright" },
	{ 0x002A,	"asterisk" },
	{ 0x002B,	"plus" },
	{ 0x002C,	"comma" },
	{ 0x002D,	"hyphen" },
	{ 0x002E,	"period" },
	{ 0x002F,	"slash" },
	{ 0x0030,	"zero" },
	{ 0x0030,	"zerooldstyle" },
	{ 0x0031,	"one" },
	{ 0x0031,	"oneoldstyle" },
	{ 0x0031,	"onefitted" },
	{ 0x0032,	"two" },
	{ 0x0032,	"twooldstyle" },
	{ 0x0033,	"three" },
	{ 0x0033,	"threeoldstyle" },
	{ 0x0034,	"four" },
	{ 0x0034,	"fouroldstyle" },
	{ 0x0035,	"five" },
	{ 0x0035,	"fiveoldstyle" },
	{ 0x0036,	"six" },
	{ 0x0036,	"sixoldstyle" },
	{ 0x0037,	"seven" },
	{ 0x0037,	"sevenoldstyle" },
	{ 0x0038,	"eight" },
	{ 0x0038,	"eightoldstyle" },
	{ 0x0039,	"nine" },
	{ 0x0039,	"nineoldstyle" },
	{ 0x003A,	"colon" },
	{ 0x003B,	"semicolon" },
	{ 0x003C,	"less" },
	{ 0x003D,	"equal" },
	{ 0x003E,	"greater" },
	{ 0x003F,	"question" },
	{ 0x0040,	"at" },
	{ 0x0041,	"A" },
	{ 0x0042,	"B" },
	{ 0x0043,	"C" },
	{ 0x0044,	"D" },
	{ 0x0045,	"E" },
	{ 0x0046,	"F" },
	{ 0x0047,	"G" },
	{ 0x0048,	"H" },
	{ 0x0049,	"I" },
	{ 0x004A,	"J" },
	{ 0x004B,	"K" },
	{ 0x004C,	"L" },
	{ 0x004D,	"M" },
	{ 0x004E,	"N" },
	{ 0x004F,	"O" },
	{ 0x0050,	"P" },
	{ 0x0051,	"Q" },
	{ 0x0052,	"R" },
	{ 0x0053,	"S" },
	{ 0x0054,	"T" },
	{ 0x0055,	"U" },
	{ 0x0056,	"V" },
	{ 0x0057,	"W" },
	{ 0x0058,	"X" },
	{ 0x0059,	"Y" },
	{ 0x005A,	"Z" },
	{ 0x005B,	"bracketleft" },
	{ 0x005C,	"backslash" },
	{ 0x005D,	"bracketright" },
	{ 0x005E,	"asciicircum" },
/*	{ 0x005E,	"circumflex" },	*/
	{ 0x005F,	"underscore" },
	{ 0x0060,	"quoteleft" },
/*	{ 0x0060,	"grave" },	*/
	{ 0x0061,	"a" },
	{ 0x0061,	"Asmall" },
	{ 0x0062,	"b" },
	{ 0x0062,	"Bsmall" },
	{ 0x0063,	"c" },
	{ 0x0063,	"Csmall" },
	{ 0x0064,	"d" },
	{ 0x0064,	"Dsmall" },
	{ 0x0065,	"e" },
	{ 0x0065,	"Esmall" },
	{ 0x0066,	"f" },
	{ 0x0066,	"Fsmall" },
	{ 0x0067,	"g" },
	{ 0x0067,	"Gsmall" },
	{ 0x0068,	"h" },
	{ 0x0068,	"Hsmall" },
	{ 0x0069,	"i" },
	{ 0x0069,	"Ismall" },
	{ 0x006A,	"j" },
	{ 0x006A,	"Jsmall" },
	{ 0x006B,	"k" },
	{ 0x006B,	"Ksmall" },
	{ 0x006C,	"l" },
	{ 0x006C,	"Lsmall" },
	{ 0x006D,	"m" },
	{ 0x006D,	"Msmall" },
	{ 0x006E,	"n" },
	{ 0x006E,	"Nsmall" },
	{ 0x006F,	"o" },
	{ 0x006F,	"Osmall" },
	{ 0x0070,	"p" },
	{ 0x0070,	"Psmall" },
	{ 0x0071,	"q" },
	{ 0x0071,	"Qsmall" },
	{ 0x0072,	"r" },
	{ 0x0072,	"Rsmall" },
	{ 0x0073,	"s" },
	{ 0x0073,	"Ssmall" },
	{ 0x0074,	"t" },
	{ 0x0074,	"Tsmall" },
	{ 0x0075,	"u" },
	{ 0x0075,	"Usmall" },
	{ 0x0076,	"v" },
	{ 0x0076,	"Vsmall" },
	{ 0x0077,	"w" },
	{ 0x0077,	"Wsmall" },
	{ 0x0078,	"x" },
	{ 0x0078,	"Xsmall" },
	{ 0x0079,	"y" },
	{ 0x0079,	"Ysmall" },
	{ 0x007A,	"z" },
	{ 0x007A,	"Zsmall" },
	{ 0x007B,	"braceleft" },
	{ 0x007C,	"bar" },
	{ 0x007D,	"braceright" },
	{ 0x007E,	"asciitilde" },
	{ 0,		0 }
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

static int
asciiequiv(int code, const char *psc)
{
	int	i;

	if (psc != NULL)
		for (i = 0; asciimap[i].psc; i++)
			if (asciimap[i].code == code &&
					strcmp(asciimap[i].psc, psc) == 0)
				return 1;
	return 0;
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
	/*
	 * Only map a character directly if it maps to an ASCII
	 * equivalent or if it is above.
	 */
	if (C < 0 || C > 127 || asciiequiv(C, N)) {
		a->codetab[a->nchars] = C;
		if (tp >= 32)
			a->fitab[tp - 32] = a->nchars;
	} else
		a->codetab[a->nchars] = -1;
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
