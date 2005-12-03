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
 * Sccsid @(#)unimap.c	1.10 (gritter) 12/3/05
 */

#include "tdef.h"
#include "unimap.h"

const struct unimap	unimap[] = {
	{ 0x00A1,	"exclamdown" },
	{ 0x00A2,	"cent" },
	{ 0x00A2,	"centoldstyle" },
	{ 0x00A3,	"sterling" },
	{ 0x00A3,	"sterlingalt" },	/* FournierMT-RegularAlt */
	{ 0x00A4,	"currency" },
	{ 0x00A5,	"yen" },
	{ 0x00A6,	"brokenbar" },
	{ 0x00A7,	"section" },
	{ 0x00A8,	"Dieresissmall" },
	{ 0x00A8,	"dieresis" },
	{ 0x00A9,	"copyright" },
	{ 0x00A9,	"copyrightsans" },
	{ 0x00A9,	"copyrightserif" },
	{ 0x00AA,	"ordfeminine" },
	{ 0x00AB,	"guillemotleft" },
	{ 0x00AC,	"logicalnot" },
	{ 0x00AE,	"registered" },
	{ 0x00AE,	"registersans" },
	{ 0x00AE,	"registerserif" },
	{ 0x00AF,	"Macronsmall" },
	{ 0x00AF,	"macron" },
	{ 0x00B0,	"degree" },
	{ 0x00B1,	"plusminus" },
	{ 0x00B2,	"twosuperior" },
	{ 0x00B3,	"threesuperior" },
	{ 0x00B4,	"acute" },
	{ 0x00B5,	"mu" },
	{ 0x00B6,	"paragraph" },
	{ 0x00B7,	"periodcentered" },
	{ 0x00B8,	"Cedillasmall" },
	{ 0x00B8,	"cedilla" },
	{ 0x00B9,	"onesuperior" },
	{ 0x00BA,	"ordmasculine" },
	{ 0x00BB,	"guillemotright" },
	{ 0x00BC,	"onequarter" },
	{ 0x00BD,	"onehalf" },
	{ 0x00BE,	"threequarters" },
	{ 0x00BF,	"questiondown" },
	{ 0x00C0,	"Agrave" },
	{ 0x00C0,	"Agravesmall" },
	{ 0x00C1,	"Aacute" },
	{ 0x00C1,	"Aacutesmall" },
	{ 0x00C2,	"Acircumflex" },
	{ 0x00C2,	"Acircumflexsmall" },
	{ 0x00C3,	"Atilde" },
	{ 0x00C3,	"Atildesmall" },
	{ 0x00C4,	"Adieresis" },
	{ 0x00C4,	"Adieresissmall" },
	{ 0x00C5,	"Aring" },
	{ 0x00C5,	"Aringsmall" },
	{ 0x00C6,	"AE" },
	{ 0x00C6,	"AEsmall" },
	{ 0x00C7,	"Ccedilla" },
	{ 0x00C7,	"Ccedillasmall" },
	{ 0x00C8,	"Egrave" },
	{ 0x00C8,	"Egravesmall" },
	{ 0x00C9,	"Eacute" },
	{ 0x00C9,	"Eacutesmall" },
	{ 0x00CA,	"Ecircumflex" },
	{ 0x00CA,	"Ecircumflexsmall" },
	{ 0x00CB,	"Edieresis" },
	{ 0x00CB,	"Edieresissmall" },
	{ 0x00CC,	"Igrave" },
	{ 0x00CC,	"Igravesmall" },
	{ 0x00CD,	"Iacute" },
	{ 0x00CD,	"Iacutesmall" },
	{ 0x00CE,	"Icircumflex" },
	{ 0x00CE,	"Icircumflexsmall" },
	{ 0x00CF,	"Idieresis" },
	{ 0x00CF,	"Idieresissmall" },
	{ 0x00D0,	"Eth" },
	{ 0x00D0,	"Ethsmall" },
	{ 0x00D1,	"Ntilde" },
	{ 0x00D1,	"Ntildesmall" },
	{ 0x00D1,	"Ntildealt" },		/* BulmerMT-ItalicAlt */
	{ 0x00D2,	"Ograve" },
	{ 0x00D2,	"Ogravesmall" },
	{ 0x00D2,	"Ogravealt" },		/* BulmerMT-ItalicAlt */
	{ 0x00D3,	"Oacute" },
	{ 0x00D3,	"Oacutesmall" },
	{ 0x00D3,	"Oacutealt" },		/* BulmerMT-ItalicAlt */
	{ 0x00D4,	"Ocircumflex" },
	{ 0x00D4,	"Ocircumflexsmall" },
	{ 0x00D4,	"Ocircumflexalt" },	/* BulmerMT-ItalicAlt */
	{ 0x00D5,	"Otilde" },
	{ 0x00D5,	"Otildesmall" },
	{ 0x00D5,	"Otildealt" },		/* BulmerMT-ItalicAlt */
	{ 0x00D6,	"Odieresis" },
	{ 0x00D6,	"Odieresissmall" },
	{ 0x00D6,	"Odieresisalt" },	/* BulmerMT-ItalicAlt */
	{ 0x00D7,	"multiply" },
	{ 0x00D8,	"Oslash" },
	{ 0x00D8,	"Oslashsmall" },
	{ 0x00D8,	"Oslashalt" },		/* BulmerMT-ItalicAlt */
	{ 0x00D9,	"Ugrave" },
	{ 0x00D9,	"Ugravesmall" },
	{ 0x00DA,	"Uacute" },
	{ 0x00DA,	"Uacutesmall" },
	{ 0x00DB,	"Ucircumflex" },
	{ 0x00DB,	"Ucircumflexsmall" },
	{ 0x00DC,	"Udieresis" },
	{ 0x00DC,	"Udieresissmall" },
	{ 0x00DD,	"Yacute" },
	{ 0x00DD,	"Yacutesmall" },
	{ 0x00DD,	"Yacutealt" },		/* BulmerMT-ItalicAlt */
	{ 0x00DE,	"Thorn" },
	{ 0x00DE,	"Thornsmall" },
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
	{ 0x0141,	"Lslashsmall" },
	{ 0x0141,	"lslash" },
	{ 0x0152,	"OE" },
	{ 0x0152,	"OEsmall" },
	{ 0x0153,	"oe" },
	{ 0x0160,	"Scaron" },
	{ 0x0160,	"Scaronsmall" },
	{ 0x0161,	"scaron" },
	{ 0x0178,	"Ydieresis" },
	{ 0x0178,	"Ydieresissmall" },
	{ 0x0178,	"Ydieresisalt" },	/* BulmerMT-ItalicAlt */
	{ 0x017D,	"Zcaron" },
	{ 0x017D,	"Zcaronsmall" },
	{ 0x017E,	"zcaron" },
	{ 0x017F,	"longs" },
	{ 0x0192,	"florin" },
	{ 0x02C7,	"Caronsmall" },
	{ 0x02C7,	"caron" },
	{ 0x02D8,	"Brevesmall" },
	{ 0x02D8,	"breve" },
	{ 0x02D9,	"Dotaccentsmall" },
	{ 0x02D9,	"dotaccent" },
	{ 0x02DA,	"Ringsmall" },
	{ 0x02DA,	"ring" },
	{ 0x02DB,	"Ogoneksmall" },
	{ 0x02DB,	"ogonek" },
	{ 0x02DC,	"tilde" },
	{ 0x0391,	"Alpha" },
	{ 0x0392,	"Beta" },
	{ 0x0393,	"Gamma" },
	{ 0x0394,	"Delta" },
	{ 0x0395,	"Epsilon" },
	{ 0x0396,	"Zeta" },
	{ 0x0397,	"Eta" },
	{ 0x0398,	"Theta" },
	{ 0x0399,	"Iota" },
	{ 0x039A,	"Kappa" },
	{ 0x039B,	"Lambda" },
	{ 0x039C,	"Mu" },
	{ 0x039D,	"Nu" },
	{ 0x039E,	"Xi" },
	{ 0x039F,	"Omicron" },
	{ 0x03A0,	"Pi" },
	{ 0x03A1,	"Rho" },
	{ 0x03A3,	"Sigma" },
	{ 0x03A4,	"Tau" },
	{ 0x03A5,	"Upsilon" },
	{ 0x03A6,	"Phi" },
	{ 0x03A7,	"Chi" },
	{ 0x03A8,	"Psi" },
	{ 0x03A9,	"Omega" },
	{ 0x03B1,	"alpha" },
	{ 0x03B2,	"beta" },
	{ 0x03B3,	"gamma" },
	{ 0x03B4,	"delta" },
	{ 0x03B5,	"epsilon" },
	{ 0x03B6,	"zeta" },
	{ 0x03B7,	"eta" },
	{ 0x03B8,	"theta" },
	{ 0x03B9,	"iota" },
	{ 0x03BA,	"kappa" },
	{ 0x03BB,	"lambda" },
	{ 0x03BC,	"mu" },
	{ 0x03BD,	"nu" },
	{ 0x03BE,	"xi" },
	{ 0x03BF,	"omicron" },
	{ 0x03C0,	"pi" },
	{ 0x03C1,	"rho" },
	{ 0x03C2,	"sigma1" },
	{ 0x03C3,	"sigma" },
	{ 0x03C4,	"tau" },
	{ 0x03C5,	"upsilon" },
	{ 0x03C6,	"phi" },
	{ 0x03C7,	"chi" },
	{ 0x03C8,	"psi" },
	{ 0x03C9,	"omega" },
	{ 0x03D1,	"theta1" },
	{ 0x03D2,	"Upsilon1" },
	{ 0x03D5,	"phi1" },
	{ 0x03D6,	"omega1" },
	{ 0x2011,	"hyphen" },
	{ 0x2012,	"figuredash" },
	{ 0x2013,	"endash" },
	{ 0x2014,	"emdash" },
	{ 0x2018,	"quoteleft" },
	{ 0x2019,	"quoteright" },
	{ 0x201A,	"quotesinglbase" },
	{ 0x201B,	"quotesingle" },
	{ 0x201C,	"quotedblleft" },
	{ 0x201C,	"quotealtdblleft" },	/* BulmerMT-RegularAlt */
	{ 0x201D,	"quotedblright" },
	{ 0x201E,	"quotedblbase" },
	{ 0x2020,	"dagger" },
	{ 0x2021,	"daggerdbl" },
	{ 0x2022,	"bullet" },
	{ 0x2026,	"ellipsis" },
	{ 0x2030,	"perthousand" },
	{ 0x2032,	"minute" },
	{ 0x2033,	"second" },
	{ 0x2039,	"guilsinglleft" },
	{ 0x203A,	"guilsinglright" },
	{ 0x203E,	"radicalex" },
	{ 0x2044,	"fraction" },
	{ 0x2070,	"zerosuperior" },
	{ 0x2074,	"foursuperior" },
	{ 0x2075,	"fivesuperior" },
	{ 0x2076,	"sixsuperior" },
	{ 0x2077,	"sevensuperior" },
	{ 0x2078,	"eightsuperior" },
	{ 0x2079,	"ninesuperior" },
	{ 0x207D,	"parenleftsuperior" },
	{ 0x207E,	"parenrightsuperior" },
	{ 0x207F,	"nsuperior" },
	{ 0x2080,	"zeroinferior" },
	{ 0x2081,	"oneinferior" },
	{ 0x2082,	"twoinferior" },
	{ 0x2083,	"threeinferior" },
	{ 0x2084,	"fourinferior" },
	{ 0x2085,	"fiveinferior" },
	{ 0x2086,	"sixinferior" },
	{ 0x2087,	"seveninferior" },
	{ 0x2088,	"eightinferior" },
	{ 0x2089,	"nineinferior" },
	{ 0x208D,	"parenleftinferior" },
	{ 0x208E,	"parenrightinferior" },
	{ 0x20A8,	"rupiah" },
	{ 0x20AC,	"Euro" },
	{ 0x2111,	"Ifraktur" },
	{ 0x2118,	"weierstrass" },
	{ 0x211C,	"Rfraktur" },
	{ 0x2122,	"trademark" },
	{ 0x2122,	"trademarksans" },
	{ 0x2122,	"trademarkserif" },
	{ 0x2126,	"Omega" },
	{ 0x2135,	"aleph" },
	{ 0x2153,	"onethird" },
	{ 0x2154,	"twothirds" },
	{ 0x215B,	"oneeighth" },
	{ 0x215C,	"threeeighths" },
	{ 0x215D,	"fiveeighths" },
	{ 0x215E,	"seveneighths" },
	{ 0x2190,	"arrowleft" },
	{ 0x2191,	"arrowup" },
	{ 0x2192,	"arrowright" },
	{ 0x2193,	"arrowdown" },
	{ 0x2194,	"arrowboth" },
	{ 0x21B5,	"carriagereturn" },
	{ 0x21D0,	"arrowdblleft" },
	{ 0x21D1,	"arrowdblup" },
	{ 0x21D2,	"arrowdblright" },
	{ 0x21D3,	"arrowdbldown" },
	{ 0x21D4,	"arrowdblboth" },
	{ 0x2200,	"universal" },
	{ 0x2202,	"partialdiff" },
	{ 0x2203,	"existential" },
	{ 0x2205,	"emptyset" },
	{ 0x2206,	"Delta" },
	{ 0x2207,	"gradient" },
	{ 0x2208,	"element" },
	{ 0x2209,	"notelement" },
	{ 0x220B,	"suchthat" },
	{ 0x220F,	"product" },
	{ 0x2211,	"summation" },
	{ 0x2212,	"minus" },
	{ 0x2215,	"fraction" },
	{ 0x2217,	"asteriskmath" },
	{ 0x221A,	"radical" },
	{ 0x221D,	"proportional" },
	{ 0x221E,	"infinity" },
	{ 0x2220,	"angle" },
	{ 0x2227,	"logicaland" },
	{ 0x2228,	"logicalor" },
	{ 0x2229,	"intersection" },
	{ 0x222A,	"union" },
	{ 0x222B,	"integral" },
	{ 0x2234,	"therefore" },
	{ 0x223C,	"similar" },
	{ 0x2245,	"congruent" },
	{ 0x2248,	"approxequal" },
	{ 0x2260,	"notequal" },
	{ 0x2261,	"equivalence" },
	{ 0x2264,	"lessequal" },
	{ 0x2265,	"greaterequal" },
	{ 0x2282,	"propersubset" },
	{ 0x2283,	"propersuperset" },
	{ 0x2284,	"notsubset" },
	{ 0x2286,	"reflexsubset" },
	{ 0x2287,	"reflexsuperset" },
	{ 0x2295,	"circleplus" },
	{ 0x2297,	"circlemultiply" },
	{ 0x22A5,	"perpendicular" },
	{ 0x22C5,	"dotmath" },
	{ 0x2320,	"integraltp" },
	{ 0x2321,	"integralbt" },
	{ 0x2329,	"angleleft" },
	{ 0x232A,	"angleright" },
	{ 0x25CA,	"lozenge" },
	{ 0x2660,	"spade" },
	{ 0x2663,	"club" },
	{ 0x2665,	"heart" },
	{ 0x2666,	"diamond" },
	{ 0xF8FF,	"apple" },
	{ 0xFB00,	"ff" },
	{ 0xFB00,	"f_f" },
	{ 0xFB01,	"fi" },
	{ 0xFB01,	"f_i" },
	{ 0xFB02,	"fl" },
	{ 0xFB02,	"f_l" },
	{ 0xFB03,	"ffi" },
	{ 0xFB03,	"f_f_i" },
	{ 0xFB04,	"ffl" },
	{ 0xFB04,	"f_f_l" },
/*	{ 0x0000,	"hungarumlaut" },	??? */
	{ -1,		0 }
};
