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
 * Sccsid @(#)afm.c	1.25 (gritter) 9/16/05
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

#ifdef	KERN
static	void	addkernpair(struct afmtab *, char *_line);
#endif

/*
 * This table maps troff special characters to PostScript names.
 */
const struct names {
	char	*trname;
	char	*psname;
} names[] = {
	{ "hy",	"hyphen" },
	{ "ct",	"cent" },
	{ "fi",	"fi" },
	{ "fl",	"fl" },
	{ "ff", "ff" },
	{ "Fi", "ffi" },
	{ "Fl", "ffl" },
	{ "dg",	"dagger" },
	{ "dd",	"daggerdbl" },
	{ "bu",	"bullet" },
	{ "de",	"ring" },
	{ "em",	"emdash" },
	{ "en",	"endash" },
	{ "sc",	"section" },
	{ "aa","acute" },
	{ "ga","grave" },
	{ "``",	"quotedblleft" },
	{ "''",	"quotedblright" },
	{ 0,	0 }
};

/*
 * Names for the Symbol font only.
 */
const struct names Snames[] = {
	{ "!=",	"notequal" },
	{ "**",	"asteriskmath" },
	{ "*A",	"Alpha" },
	{ "*B",	"Beta" },
	{ "*C",	"Xi" },
	{ "*D",	"Delta" },
	{ "*E",	"Epsilon" },
	{ "*F",	"Phi" },
	{ "*G",	"Gamma" },
	{ "*H",	"Theta" },
	{ "*I",	"Iota" },
	{ "*K",	"Kappa" },
	{ "*L",	"Lambda" },
	{ "*M",	"Mu" },
	{ "*N",	"Nu" },
	{ "*O",	"Omicron" },
	{ "*P",	"Pi" },
	{ "*Q",	"Psi" },
	{ "*R",	"Rho" },
	{ "*S",	"Sigma" },
	{ "*T",	"Tau" },
	{ "*U",	"Upsilon" },
	{ "*W",	"Omega" },
	{ "*X",	"Chi" },
	{ "*Y",	"Eta" },
	{ "*Z",	"Zeta" },
	{ "*a",	"alpha" },
	{ "*b",	"beta" },
	{ "*c",	"xi" },
	{ "*d",	"delta" },
	{ "*e",	"epsilon" },
	{ "*f",	"phi" },
	{ "*g",	"gamma" },
	{ "*h",	"theta" },
	{ "*i",	"iota" },
	{ "*k",	"kappa" },
	{ "*l",	"lambda" },
	{ "*m",	"mu" },
	{ "*n",	"nu" },
	{ "*o",	"omicron" },
	{ "*p",	"pi" },
	{ "*q",	"psi" },
	{ "*r",	"rho" },
	{ "*s",	"sigma" },
	{ "*t",	"tau" },
	{ "*u",	"upsilon" },
	{ "*w",	"omega" },
	{ "*x",	"chi" },
	{ "*y",	"eta" },
	{ "*z",	"zeta" },
	{ "+-",	"plusminus" },
	{ "->",	"arrowright" },
	{ "<",	"less" },
	{ "<-",	"arrowleft" },
	{ "<=",	"lessequal" },
	{ "==",	"equivalence" },
	{ ">",	"greater" },
	{ ">=",	"greaterequal" },
	{ "O+",	"circleplus" },
	{ "Ox",	"circlemultiply" },
	{ "^",	"logicaland" },
	{ "al",	"aleph" },
	{ "ap",	"similar" },
	{ "br",	"parenleftex" },
	{ "bu",	"bullet" },
	{ "bv",	"braceex" },
	{ "ca",	"intersection" },
	{ "co",	"copyrightserif" },
	{ "cu",	"union" },
	{ "da",	"arrowdown" },
	{ "de",	"degree" },
	{ "di",	"divide" },
	{ "eq",	"equal" },
	{ "es",	"emptyset" },
	{ "fa",	"universal" },
	{ "fm",	"minute" },
	{ "gr",	"gradient" },
	{ "ib",	"reflexsubset" },
	{ "if",	"infinity" },
	{ "ip",	"reflexsuperset" },
	{ "is",	"integral" },
	{ "lb",	"braceleftbt" },
	{ "lc",	"bracketlefttp" },
	{ "lf",	"bracketleftbt" },
	{ "lk",	"braceleftmid" },
	{ "lt",	"bracelefttp" },
	{ "mi",	"minus" },
	{ "mo",	"element" },
	{ "mu",	"multiply" },
	{ "no",	"logicalnot" },
	{ "or",	"bar" },
	{ "or",	"logicalor" },
	{ "pd",	"partialdiff" },
	{ "pl",	"plus" },
	{ "pt",	"proportional" },
	{ "rb",	"bracerightbt" },
	{ "rc",	"bracketrighttp" },
	{ "rf",	"bracketrightbt" },
	{ "rg",	"registerserif" },
	{ "rk",	"bracerightmid" },
	{ "rn",	"radicalex" },
	{ "rt",	"bracerighttp" },
	{ "sb",	"propersubset" },
	{ "sl",	"fraction" },
	{ "sp",	"propersuperset" },
	{ "sr",	"radical" },
	{ "te",	"existential" },
	{ "tm",	"trademarkserif" },
	{ "ts",	"sigma1" },
	{ "ua",	"arrowup" },
	{ "ul",	"underscore" },
	{ "vr",	"bracketleftex" },
	{ "~~",	"approxequal" },
	{ 0,	0 },
};

/*
 * These names are only used with the S1 font. They are characters from
 * the Times that serve as fallbacks and should not be superseded by
 * characters from other fonts.
 */
const struct names S1names[] = {
	{ "or",	"bar" },
	{ "14",	"onequarter" },
	{ "34",	"threequarters" },
	{ "12",	"onehalf" },
	{ "\\-","endash" },
	{ "\\'","acute" },
	{ "\\`","grave" },
	{ "ru",	"underscore" },
	{ 0,	0 }
};

/*
 * Figures from charlib.
 */
#define	NCHARLIB	16
static const struct charlib {
	const char	*name;
	int		width;
	int		kern;
	int		code;
	int		symbol;
} charlib[] = {
	{ "bx",	 500,	2,	1,	0 },
	{ "ci",	 750,	0,	1,	0 },
	{ "sq",	 500,	2,	1,	0 },
	{ "ff",	 600,	2,	1,	0 },	/* widths of the ligatures */
	{ "Fi",	 840,	2,	1,	0 },	/* are likely wrong, but */
	{ "Fl",	 840,	2,	1,	0 },	/* they are normally not used */
	{ "~=",	 550,	0,	1,	1 },
	{ "L1", 1100,	1,	2,	1 },
	{ "LA",	1100,	1,	2,	1 },
	{ "LV",	1100,	3,	2,	1 },
	{ "LH",	2100,	1,	2,	1 },
	{ "Lb",	2100,	1,	2,	1 },
	{ "lh",	1000,	0,	2,	1 },
	{ "rh",	1000,	0,	2,	1 },
	{ "Sl",	 500,	2,	1,	1 },
	{ "ob",	 380,	0,	1,	1 },
	{    0,	   0,	0,	0,	0 }
};

/*
 * The values in this table determine if a character that is found on an
 * ASCII position in a PostScript font actually is that ASCII character.
 * If not, the position in fitab remains empty, and the fallback sequence
 * is used to find it in another font.
 */
static const struct asciimap {
	int		code;
	const char	*psc;
} asciimap[] = {
	{ 0x0020,	"space" },
	{ 0x0021,	"exclam" },
	{ 0x0022,	"quotedbl" },
	{ 0x0023,	"numbersign" },
	{ 0x0024,	"dollar" },
	{ 0x0024,	"dollaralt" },		/* FournierMT-RegularAlt */
	{ 0x0025,	"percent" },
	{ 0x0026,	"ampersand" },
	{ 0x0026,	"ampersandalt" },	/* AGaramondAlt-Italic */
	{ 0x0027,	"quoteright" },
	{ 0x0028,	"parenleft" },
	{ 0x0029,	"parenright" },
	{ 0x002A,	"asterisk" },
	{ 0x002B,	"plus" },
	{ 0x002C,	"comma" },
	{ 0x002D,	"hyphen" },
	{ 0x002D,	"minus" },		/* Symbol */
	{ 0x002E,	"period" },
	{ 0x002F,	"slash" },
	{ 0x0030,	"zero" },
	{ 0x0030,	"zerooldstyle" },
	{ 0x0030,	"zeroalt" },		/* BulmerMT-RegularAlt */
	{ 0x0031,	"one" },
	{ 0x0031,	"oneoldstyle" },
	{ 0x0031,	"onefitted" },
	{ 0x0031,	"onealtfitted" },	/* BulmerMT-ItalicAlt */
	{ 0x0032,	"two" },
	{ 0x0032,	"twooldstyle" },
	{ 0x0032,	"twoalt" },		/* BulmerMT-RegularAlt */
	{ 0x0033,	"three" },
	{ 0x0033,	"threeoldstyle" },
	{ 0x0033,	"threealt" },		/* BulmerMT-RegularAlt */
	{ 0x0034,	"four" },
	{ 0x0034,	"fouroldstyle" },
	{ 0x0034,	"fouralt" },		/* BulmerMT-RegularAlt */
	{ 0x0035,	"five" },
	{ 0x0035,	"fiveoldstyle" },
	{ 0x0035,	"fivealt" },		/* BulmerMT-RegularAlt */
	{ 0x0036,	"six" },
	{ 0x0036,	"sixoldstyle" },
	{ 0x0036,	"sixalt" },		/* BulmerMT-RegularAlt */
	{ 0x0037,	"seven" },
	{ 0x0037,	"sevenoldstyle" },
	{ 0x0037,	"sevenalt" },		/* BulmerMT-RegularAlt */
	{ 0x0038,	"eight" },
	{ 0x0038,	"eightoldstyle" },
	{ 0x0038,	"eightalt" },		/* BulmerMT-RegularAlt */
	{ 0x0039,	"nine" },
	{ 0x0039,	"nineoldstyle" },
	{ 0x0039,	"ninealt" },		/* BulmerMT-RegularAlt */
	{ 0x003A,	"colon" },
	{ 0x003B,	"semicolon" },
	{ 0x003C,	"less" },
	{ 0x003D,	"equal" },
	{ 0x003E,	"greater" },
	{ 0x003F,	"question" },
	{ 0x0040,	"at" },
	{ 0x0041,	"A" },
	{ 0x0041,	"Aswash" },		/* AGaramondAlt-Italic */
	{ 0x0042,	"B" },
	{ 0x0042,	"Bswash" },		/* AGaramondAlt-Italic */
	{ 0x0043,	"C" },
	{ 0x0043,	"Cswash" },		/* AGaramondAlt-Italic */
	{ 0x0044,	"D" },
	{ 0x0044,	"Dswash" },		/* AGaramondAlt-Italic */
	{ 0x0045,	"E" },
	{ 0x0045,	"Eswash" },		/* AGaramondAlt-Italic */
	{ 0x0046,	"F" },
	{ 0x0046,	"Fswash" },		/* AGaramondAlt-Italic */
	{ 0x0047,	"G" },
	{ 0x0047,	"Gswash" },		/* AGaramondAlt-Italic */
	{ 0x0048,	"H" },
	{ 0x0048,	"Hswash" },		/* AGaramondAlt-Italic */
	{ 0x0049,	"I" },
	{ 0x0049,	"Iswash" },		/* AGaramondAlt-Italic */
	{ 0x004A,	"J" },
	{ 0x004A,	"Jalt" },		/* FournierMT-RegularAlt */
	{ 0x004A,	"Jalttwo" },		/* BulmerMT-ItalicAlt */
	{ 0x004A,	"Jswash" },		/* AGaramondAlt-Italic */
	{ 0x004A,	"JTallCapalt" },	/* FournierMT-RegularAlt */
	{ 0x004B,	"K" },
	{ 0x004B,	"Kalt" },		/* BulmerMT-ItalicAlt */
	{ 0x004B,	"Kswash" },		/* AGaramondAlt-Italic */
	{ 0x004C,	"L" },
	{ 0x004C,	"Lswash" },		/* AGaramondAlt-Italic */
	{ 0x004D,	"M" },
	{ 0x004D,	"Mswash" },		/* AGaramondAlt-Italic */
	{ 0x004E,	"N" },
	{ 0x004E,	"Nalt" },		/* BulmerMT-ItalicAlt */
	{ 0x004E,	"Nswash" },		/* AGaramondAlt-Italic */
	{ 0x004F,	"O" },
	{ 0x004F,	"Oalt" },		/* BulmerMT-ItalicAlt */
	{ 0x004F,	"Oswash" },		/* AGaramondAlt-Italic */
	{ 0x0050,	"P" },
	{ 0x0050,	"Pswash" },		/* AGaramondAlt-Italic */
	{ 0x0051,	"Q" },
	{ 0x0051,	"Qalt" },		/* FournierMT-RegularAlt */
	{ 0x0051,	"Qalttitling" },	/* AGaramondAlt-Regular */
	{ 0x0051,	"Qswash" },		/* AGaramondAlt-Italic */
	{ 0x0051,	"QTallCapalt" },	/* FournierMT-RegularAlt */
	{ 0x0052,	"R" },
	{ 0x0052,	"Ralternate" },		/* Bembo-Alt */
	{ 0x0052,	"Rswash" },		/* AGaramondAlt-Italic */
	{ 0x0053,	"S" },
	{ 0x0053,	"Sswash" },		/* AGaramondAlt-Italic */
	{ 0x0054,	"T" },
	{ 0x0054,	"Talt" },		/* BulmerMT-ItalicAlt */
	{ 0x0054,	"Tswash" },		/* AGaramondAlt-Italic */
	{ 0x0055,	"U" },
	{ 0x0055,	"Uswash" },		/* AGaramondAlt-Italic */
	{ 0x0056,	"V" },
	{ 0x0056,	"Vswash" },		/* AGaramondAlt-Italic */
	{ 0x0057,	"W" },
	{ 0x0057,	"Wswash" },		/* AGaramondAlt-Italic */
	{ 0x0058,	"X" },
	{ 0x0058,	"Xswash" },		/* AGaramondAlt-Italic */
	{ 0x0059,	"Y" },
	{ 0x0059,	"Yalt" },		/* BulmerMT-ItalicAlt */
	{ 0x0059,	"Yswash" },		/* AGaramondAlt-Italic */
	{ 0x005A,	"Z" },
	{ 0x005A,	"Zswash" },		/* AGaramondAlt-Italic */
	{ 0x005B,	"bracketleft" },
	{ 0x005C,	"backslash" },
	{ 0x005D,	"bracketright" },
	{ 0x005E,	"asciicircum" },
/*	{ 0x005E,	"circumflex" },	*/
	{ 0x005F,	"underscore" },
	{ 0x0060,	"quoteleft" },
	{ 0x0060,	"quotealtleft" },	/* BulmerMT-RegularAlt */
/*	{ 0x0060,	"grave" },	*/
	{ 0x0061,	"a" },
	{ 0x0061,	"Asmall" },
	{ 0x0061,	"aswash" },		/* AGaramondAlt-Regular */
	{ 0x0062,	"b" },
	{ 0x0062,	"Bsmall" },
	{ 0x0063,	"c" },
	{ 0x0063,	"Csmall" },
	{ 0x0064,	"d" },
	{ 0x0064,	"Dsmall" },
	{ 0x0065,	"e" },
	{ 0x0065,	"Esmall" },
	{ 0x0065,	"eswash" },		/* AGaramondAlt-Regular */
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
	{ 0x006A,	"Jsmallalt" },		/* FournierMT-RegularAlt */
	{ 0x006B,	"k" },
	{ 0x006B,	"Ksmall" },
	{ 0x006C,	"l" },
	{ 0x006C,	"Lsmall" },
	{ 0x006D,	"m" },
	{ 0x006D,	"Msmall" },
	{ 0x006E,	"n" },
	{ 0x006E,	"Nsmall" },
	{ 0x006E,	"nswash" },		/* AGaramondAlt-Regular */
	{ 0x006F,	"o" },
	{ 0x006F,	"Osmall" },
	{ 0x0070,	"p" },
	{ 0x0070,	"Psmall" },
	{ 0x0071,	"q" },
	{ 0x0071,	"Qsmall" },
	{ 0x0072,	"r" },
	{ 0x0072,	"Rsmall" },
	{ 0x0072,	"rswash" },		/* AGaramondAlt-Regular */
	{ 0x0073,	"s" },
	{ 0x0073,	"Ssmall" },
	{ 0x0074,	"t" },
	{ 0x0074,	"Tsmall" },
	{ 0x0074,	"tswash" },		/* AGaramondAlt-Regular */
	{ 0x0074,	"tswashalt" },		/* AGaramondAlt-Regular */
	{ 0x0075,	"u" },
	{ 0x0075,	"Usmall" },
	{ 0x0076,	"v" },
	{ 0x0076,	"Vsmall" },
	{ 0x0076,	"vswash" },		/* AGaramondAlt-Italic */
	{ 0x0077,	"w" },
	{ 0x0077,	"Wsmall" },
	{ 0x0077,	"walt" },		/* FournierMT-RegularAlt */
	{ 0x0078,	"x" },
	{ 0x0078,	"Xsmall" },
	{ 0x0079,	"y" },
	{ 0x0079,	"Ysmall" },
	{ 0x007A,	"z" },
	{ 0x007A,	"Zsmall" },
	{ 0x007A,	"zalt" },		/* FournierMT-ItalicAlt */
	{ 0x007A,	"zswash" },		/* AGaramondAlt-Regular */
	{ 0x007B,	"braceleft" },
	{ 0x007C,	"bar" },
	{ 0x007D,	"braceright" },
	{ 0x007E,	"asciitilde" },
	{ 0x007E,	"similar" },		/* Symbol */
	{ 0,		0 }
};

static int
nextprime(int n)
{
	const int	primes[] = {
		509, 1021, 2039, 4093, 8191, 16381, 32749, 65521
	};
	int	mprime = 7;
	int	i;

	for (i = 0; i < sizeof primes / sizeof *primes; i++)
		if ((mprime = primes[i]) >= (n < 65536 ? n*4 :
					n < 262144 ? n*2 : n))
			break;
	if (i == sizeof primes / sizeof *primes)
		mprime = n;     /* not so prime, but better than failure */
	return mprime;
}

unsigned
pjw(const char *cp)
{
	unsigned	h = 0, g;

	cp--;
	while (*++cp) {
		h = (h << 4 & 0xffffffff) + (*cp&0377);
		if ((g = h & 0xf0000000) != 0) {
			h = h ^ g >> 24;
			h = h ^ g;
		}
	}
	return h;
}

struct namecache *
afmnamelook(struct afmtab *a, const char *name)
{
	struct namecache	*np;
	unsigned	h, c, n = 0;

	h = pjw(name) % a->nameprime;
	np = &a->namecache[c = h];
	while (np->afpos != 0) {
		if (a->nametab[np->afpos] == 0 ||
				strcmp(a->nametab[np->afpos], name) == 0)
			break;
		c += n&1 ? -((n+1)/2) * ((n+1)/2) : ((n+1)/2) * ((n+1)/2);
		n++;
		while (c >= a->nameprime)
			c -= a->nameprime;
		np = &a->namecache[c];
	}
	return np;
}

static int
mapname(const char *psname, int isS, int isS1)
{
	int	i, j;

	if (isS) {
		for (i = 0; Snames[i].trname; i++)
			if (strcmp(Snames[i].psname, psname) == 0)
				break;
		if (Snames[i].trname)
			for (j = 0; j < nchtab; j++)
				if (strcmp(Snames[i].trname,
						&chname[chtab[j]]) == 0)
					return j + 128;
	}
	if (isS1) {
		for (i = 0; S1names[i].trname; i++)
			if (strcmp(S1names[i].psname, psname) == 0)
				break;
		if (S1names[i].trname)
			for (j = 0; j < nchtab; j++)
				if (strcmp(S1names[i].trname,
						&chname[chtab[j]]) == 0)
					return j + 128;
	}
	for (i = 0; names[i].trname; i++)
		if (strcmp(names[i].psname, psname) == 0)
			break;
	if (names[i].trname) {
		for (j = 0; j < nchtab; j++)
			if (strcmp(names[i].trname, &chname[chtab[j]]) == 0)
				return j + 128;
	}
	return 0;
}

/*
 * After all characters have been read, construct a font-specific
 * encoding for the rest. Also move the name table to permanent space.
 */
static void
remap(struct afmtab *a)
{
	int	i, j = 128 - 32 + nchtab;
	char	*space, *tp;
	struct namecache	*np;

	for (i = 1; i < a->nchars; i++) {
		if (a->codetab[i] == -1 && a->nametab[i] != NULL) {
			while (a->fitab[j] != 0)
				j++;
			a->fitab[j] = i;
			np = afmnamelook(a, a->nametab[i]);
			np->afpos = i;
			np->fival[0] = j;
			if (strcmp(a->nametab[i], "space") == 0)
				np->fival[1] = 0;
		}
	}
	space = malloc(a->nspace);
	for (i = 0; i < a->nchars; i++) {
		if (a->nametab[i]) {
			tp = a->nametab[i];
			a->nametab[i] = space;
			while (*space++ = *tp++);
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

int
unitconv(int i)
{
	float	d;
	int	Units, Point, uw;

	Units = 1000;
	Point = dev.res / 72;
	uw = Units / Point;
	if (uw > dev.unitwidth) {
		d = uw / dev.unitwidth;
		return i / d + (i % (int)d >= d/2);
	} else if (uw < dev.unitwidth)
		return i * dev.unitwidth / uw;
	else
		return i;
}

static void
addchar(struct afmtab *a, int C, int tp, int cl, int WX, int B[4], char *N)
{
	struct namecache	*np = NULL;
	int	ae;

	if (N != NULL) {
		np = afmnamelook(a, N);
		np->afpos = a->nchars;
	}
	a->fontab[a->nchars] = unitconv(WX);
	if (B[1] <= -10)
		a->kerntab[a->nchars] |= 1;
	if (B[3] > a->capheight)
		a->kerntab[a->nchars] |= 2;
	/*
	 * Only map a character directly if it maps to an ASCII
	 * equivalent or to a troff special character.
	 */
	ae = asciiequiv(C, N);
	if (cl)
		a->codetab[a->nchars] = cl;
	else if (tp)
		a->codetab[a->nchars] = tp;
	else if (C > 32 && C < 127 && ae)
		a->codetab[a->nchars] = C;
	else
		a->codetab[a->nchars] = -1;
	if (C > 32 && C < 127 && ae) {
		a->fitab[C - 32] = a->nchars;
		if (np)
			np->fival[0] = C - 32;
	} else if (C == 32 && np)
		np->fival[0] = 0;
	if (tp) {
		a->fitab[tp - 32] = a->nchars;
		if (np)
			np->fival[1] = tp - 32;
	}
	a->nametab[a->nchars] = N;
	a->nchars++;
}

/*
 * Add charlib figues to the "S" font.
 */
static void
addcharlib(struct afmtab *a, int symbol)
{
	int	i, j;
	int	B[4] = { 0, 0, 0, 0 };

	for (j = 0; j < nchtab; j++)
		for (i = 0; charlib[i].name; i++) {
			if (charlib[i].symbol && !symbol)
				continue;
			if (strcmp(charlib[i].name, &chname[chtab[j]]) == 0) {
				B[1] = charlib[i].kern & 1 ? -11 : 0;
				B[3] = charlib[i].kern & 2 ?
					a->capheight + 1 : 0;
				addchar(a, -1, j+128, charlib[i].code,
						charlib[i].width, B, NULL);
			}
		}
}

static void
addmetrics(struct afmtab *a, char *_line, int isSymbol)
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
			a->nspace += lp - N;
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
	tp = mapname(N, isSymbol,
			a->base[0]=='S' && a->base[1]=='1' && a->base[2]==0);
	addchar(a, C, tp, 0, WX, B, N);
}

int
afmget(struct afmtab *a, char *contents, size_t size)
{
	enum {
		NONE,
		FONTMETRICS,
		CHARMETRICS,
		KERNDATA,
		KERNPAIRS
	} state = NONE;
	char	*cp, *th, *tp;
	int	n = 0, i;
	int	isSymbol = 0;

	if ((cp = strrchr(a->file, '/')) == NULL)
		cp = a->file;
	else
		cp++;
	a->base = malloc(strlen(cp) + 1);
	strcpy(a->base, cp);
	if ((cp = strrchr(a->base, '.')) != NULL)
		*cp = '\0';
	a->lineno = 1;
	for (cp = contents; cp < &contents[size]; a->lineno++, cp++) {
		while (*cp == ' ' || *cp == '\t' || *cp == '\r')
			cp++;
		if (*cp == '\n')
			continue;
		if (thisword(cp, "Comment"))
			/*EMPTY*/;
		else if (state == NONE && thisword(cp, "StartFontMetrics"))
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
			isSymbol = strcmp(a->fontname, "Symbol") == 0;
		} else if (state == FONTMETRICS &&
				(th = thisword(cp, "CapHeight")) != NULL) {
			a->capheight = strtol(th, NULL, 10);
		} else if (state == FONTMETRICS &&
				(th = thisword(cp, "StartCharMetrics")) != 0) {
			n = strtol(th, NULL, 10);
			state = CHARMETRICS;
			a->fitab = calloc(n+NCHARLIB+1 + 128 - 32 + nchtab,
					sizeof *a->fitab);
			a->fontab = malloc((n+NCHARLIB+1)*sizeof *a->fontab);
			a->fontab[0] = dev.res * dev.unitwidth / 72 / 3;
			a->kerntab = malloc((n+NCHARLIB+1)*sizeof *a->kerntab);
			a->kerntab[0] = 0;
			a->codetab = malloc((n+NCHARLIB+1)*sizeof *a->codetab);
			a->codetab[0] = 0;
			for (i = 1; i < n+NCHARLIB+1; i++)
				a->codetab[i] = -1;
			a->nametab = malloc((n+NCHARLIB+1)*sizeof *a->nametab);
			a->nametab[0] = 0;
			a->nchars = 1;
			addcharlib(a, a->base[0]=='S' && a->base[1]==0);
			a->nameprime = nextprime(n+NCHARLIB+1);
			a->namecache = calloc(a->nameprime,
					sizeof *a->namecache);
			for (i = 0; i < a->nameprime; i++) {
				a->namecache[i].fival[0] = -1;
				a->namecache[i].fival[1] = -1;
			}
		} else if (state == CHARMETRICS &&
				thisword(cp, "EndCharMetrics")) {
			state = FONTMETRICS;
			remap(a);
		} else if (state == CHARMETRICS && n-- > 0) {
			addmetrics(a, cp, isSymbol);
#ifdef	KERN
		} else if (state == FONTMETRICS &&
				thisword(cp, "StartKernData") != 0) {
			state = KERNDATA;
		} else if (state == KERNDATA &&
				(th = thisword(cp, "StartKernPairs")) != 0) {
			a->nkernpairs = n = strtol(th, NULL, 10);
			a->kernprime = nextprime(4 * a->nkernpairs);
			state = KERNPAIRS;
			a->kernpairs = calloc(a->kernprime,
					sizeof *a->kernpairs);
		} else if (state == KERNPAIRS &&
				thisword(cp, "EndKernPairs")) {
			state = KERNDATA;
		} else if (state == KERNPAIRS && n-- > 0) {
			addkernpair(a, cp);
		} else if (state == KERNDATA &&
				thisword(cp, "EndKernData")) {
			state = FONTMETRICS;
#endif	/* KERN */
		}
		while (cp < &contents[size] && *cp != '\n')
			cp++;
	}
	if (a->fontname == NULL) {
		errprint("Missing \"FontName\" in %s", a->path);
		return -1;
	}
	a->Font.nwfont = a->nchars > 255 ? 255 : a->nchars;
	return 0;
}

/*
 * This is for legacy font support. It exists at this place because both
 * troff and dpost need it in combination with AFM support.
 */
void
makefont(int nf, char *devfontab, char *devkerntab, char *devcodetab,
		char *devfitab, int nw)
{
	int	i;

	free(fontab[nf]);
	free(kerntab[nf]);
	free(codetab[nf]);
	free(fitab[nf]);
	fontab[nf] = calloc(nw, sizeof *fontab);
	kerntab[nf] = calloc(nw, sizeof *kerntab);
	codetab[nf] = calloc(nw, sizeof *codetab);
	fitab[nf] = calloc(128 - 32 + nchtab, sizeof *fitab);
	if (devfontab) for (i = 0; i < nw; i++)
		fontab[nf][i] = devfontab[i]&0377;
	if (devkerntab) for (i = 0; i < nw; i++)
		kerntab[nf][i] = devkerntab[i]&0377;
	if (devcodetab) for (i = 0; i < nw; i++)
		codetab[nf][i] = devcodetab[i]&0377;
	if (devfitab) for (i = 0; i < 128 - 32 + nchtab; i++)
		fitab[nf][i] = devfitab[i]&0377;
}

#ifdef	KERN
#define	hash(c, prime)	((2654435769U * (c) >> 16) % prime)

static struct kernpair *
kernlook(struct afmtab *a, int ch1, int ch2)
{
	struct kernpair	*kp;
	unsigned	h, c, n = 0;

	h = hash((unsigned)ch1<<16 | (unsigned)ch2, a->kernprime);
	kp = &a->kernpairs[c = h];
	while (kp->ch1 != 0 || kp->ch2 != 0) {
		if (kp->ch1 == ch1 && kp->ch2 == ch2)
			break;
		c += n&1 ? -((n+1)/2) * ((n+1)/2) : ((n+1)/2) * ((n+1)/2);
		n++;
		while (c >= a->kernprime)
			c -= a->kernprime;
		kp = &a->kernpairs[c];
	}
	return kp;
}

static void
addkernpair(struct afmtab *a, char *_line)
{
	struct kernpair	*kp;
	struct namecache	*np1, *np2;
	char	*lp = _line, c, *cp;
	int	n, i, j;

	if (lp[0] == 'K' && lp[1] == 'P') {
		lp += 2;
		if (*lp == 'X')
			lp++;
		while (*lp && *lp == ' ' || *lp == '\t')
			lp++;
		cp = lp;
		while (*lp && *lp != '\n' && *lp != '\r' &&
					*lp != ' ' && *lp != '\t')
			lp++;
		if ((c = *lp) == 0)
			return;
		*lp = 0;
		np1 = afmnamelook(a, cp);
		*lp = c;
		while (*lp && *lp == ' ' || *lp == '\t')
			lp++;
		cp = lp;
		while (*lp && *lp != '\n' && *lp != '\r' &&
				*lp != ' ' && *lp != '\t')
			lp++;
		if ((c = *lp) == 0)
			return;
		*lp = 0;
		np2 = afmnamelook(a, cp);
		*lp = c;
		n = unitconv(strtol(&lp[1], NULL, 10));
		for (i = 0; i < 2; i++)
			if (np1->fival[i] >= 0)
				for (j = 0; j < 2; j++)
					if (np2->fival[j] >= 0) {
						kp = kernlook(a, np1->fival[i],
								np2->fival[j]);
						kp->ch1 = np1->fival[i];
						kp->ch2 = np2->fival[j];
						kp->k = n;
					}
	}
}

int
afmgetkern(struct afmtab *a, int ch1, int ch2)
{
	struct kernpair	*kp;

	if (a->kernpairs) {
		kp = kernlook(a, ch1, ch2);
		return kp->k;
	} else
		return 0;
}
#endif
