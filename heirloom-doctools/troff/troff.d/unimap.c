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
 * Sccsid @(#)unimap.c	1.2 (gritter) 8/17/05
 */

#include "unimap.h"

const struct unimap	unimap[] = {
/*	{ 0x005E,	"circumflex" },	*/
/*	{ 0x0060,	"grave" },	*/
	{ 0x00A1,	"exclamdown" },
	{ 0x00A2,	"cent" },
	{ 0x00A3,	"sterling" },
	{ 0x00A4,	"currency" },
	{ 0x00A5,	"yen" },
	{ 0x00A6,	"brokenbar" },
	{ 0x00A7,	"section" },
	{ 0x00A8,	"dieresis" },
	{ 0x00A9,	"copyright" },
	{ 0x00AA,	"ordfeminine" },
	{ 0x00AB,	"guillemotleft" },
	{ 0x00AC,	"logicalnot" },
	{ 0x00AD,	"hyphen" },
	{ 0x00AE,	"registered" },
	{ 0x00AF,	"macron" },
	{ 0x00B0,	"degree" },
	{ 0x00B1,	"plusminus" },
	{ 0x00B2,	"twosuperior" },
	{ 0x00B3,	"threesuperior" },
	{ 0x00B4,	"acute" },
	{ 0x00B5,	"mu" },
	{ 0x00B6,	"paragraph" },
	{ 0x00B7,	"periodcentered" },
	{ 0x00B8,	"cedilla" },
	{ 0x00B9,	"onesuperior" },
	{ 0x00BA,	"ordmasculine" },
	{ 0x00BB,	"guillemotright" },
	{ 0x00BC,	"onequarter" },
	{ 0x00BD,	"onehalf" },
	{ 0x00BE,	"threequarters" },
	{ 0x00BF,	"questiondown" },
	{ 0x00C0,	"Agrave" },
	{ 0x00C1,	"Aacute" },
	{ 0x00C2,	"Acircumflex" },
	{ 0x00C3,	"Atilde" },
	{ 0x00C4,	"Adieresis" },
	{ 0x00C5,	"Aring" },
	{ 0x00C6,	"AE" },
	{ 0x00C7,	"Ccedilla" },
	{ 0x00C8,	"Egrave" },
	{ 0x00C9,	"Eacute" },
	{ 0x00CA,	"Ecircumflex" },
	{ 0x00CB,	"Edieresis" },
	{ 0x00CC,	"Igrave" },
	{ 0x00CD,	"Iacute" },
	{ 0x00CE,	"Icircumflex" },
	{ 0x00CF,	"Idieresis" },
	{ 0x00D0,	"Eth" },
	{ 0x00D1,	"Ntilde" },
	{ 0x00D2,	"Ograve" },
	{ 0x00D3,	"Oacute" },
	{ 0x00D4,	"Ocircumflex" },
	{ 0x00D5,	"Otilde" },
	{ 0x00D6,	"Odieresis" },
	{ 0x00D7,	"multiply" },
	{ 0x00D8,	"Oslash" },
	{ 0x00D9,	"Ugrave" },
	{ 0x00DA,	"Uacute" },
	{ 0x00DB,	"Ucircumflex" },
	{ 0x00DC,	"Udieresis" },
	{ 0x00DD,	"Yacute" },
	{ 0x00DE,	"Thorn" },
	{ 0x00DF,	"germandbls" },
	{ 0x00E0,	"agrave" },
	{ 0x00E1,	"aacute" },
	{ 0x00E2,	"acircumflex" },
	{ 0x00E3,	"atilde" },
	{ 0x00E4,	"adieresis" },
	{ 0x00E5,	"aring" },
	{ 0x00E6,	"ae" },
	{ 0x00E7,	"ccedilla" },
	{ 0x00E8,	"egrave" },
	{ 0x00E9,	"eacute" },
	{ 0x00EA,	"ecircumflex" },
	{ 0x00EB,	"edieresis" },
	{ 0x00EC,	"igrave" },
	{ 0x00ED,	"iacute" },
	{ 0x00EE,	"icircumflex" },
	{ 0x00EF,	"idieresis" },
	{ 0x00F0,	"eth" },
	{ 0x00F1,	"ntilde" },
	{ 0x00F2,	"ograve" },
	{ 0x00F3,	"oacute" },
	{ 0x00F4,	"ocircumflex" },
	{ 0x00F5,	"otilde" },
	{ 0x00F6,	"odieresis" },
	{ 0x00F7,	"divide" },
	{ 0x00F8,	"oslash" },
	{ 0x00F9,	"ugrave" },
	{ 0x00FA,	"uacute" },
	{ 0x00FB,	"ucircumflex" },
	{ 0x00FC,	"udieresis" },
	{ 0x00FD,	"yacute" },
	{ 0x00FE,	"thorn" },
	{ 0x00FF,	"ydieresis" },
	{ 0x0131,	"dotlessi" },
	{ 0x0141,	"Lslash" },
	{ 0x0141,	"lslash" },
	{ 0x0152,	"OE" },
	{ 0x0153,	"oe" },
	{ 0x0160,	"Scaron" },
	{ 0x0161,	"scaron" },
	{ 0x0178,	"Ydieresis" },
	{ 0x017D,	"Zcaron" },
	{ 0x017E,	"zcaron" },
	{ 0x0192,	"florin" },
	{ 0x02C7,	"caron" },
	{ 0x02D8,	"breve" },
	{ 0x02D9,	"dotaccent" },
	{ 0x02DA,	"ring" },
	{ 0x02DB,	"ogonek" },
	{ 0x02DC,	"tilde" },
	{ 0x2013,	"endash" },
	{ 0x2014,	"emdash" },
	{ 0x201A,	"quotesinglbase" },
	{ 0x201B,	"quotesingle" },
	{ 0x201C,	"quotedblleft" },
	{ 0x201D,	"quotedblright" },
	{ 0x201E,	"quotedblbase" },
	{ 0x2018,	"quoteleft" },
	{ 0x2019,	"quoteright" },
	{ 0x2020,	"dagger" },
	{ 0x2021,	"daggerdbl" },
	{ 0x2022,	"bullet" },
	{ 0x2026,	"ellipsis" },
	{ 0x2030,	"perthousand" },
	{ 0x2039,	"guilsinglleft" },
	{ 0x203A,	"guilsinglright" },
	{ 0x2044,	"fraction" },
	{ 0x20AC,	"Euro" },
	{ 0x2122,	"trademark" },
	{ 0x2212,	"minus" },
	{ 0xFB01,	"fi" },
	{ 0xFB02,	"fl" },
/*	{ 0x0000,	"hungarumlaut" },	??? */
	{ -1,		0 }
};
