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


/*	from OpenSolaris "n4.c	1.8	05/06/08 SMI"	*/

/*
 * Portions Copyright (c) 2005 Gunnar Ritter, Freiburg i. Br., Germany
 *
 * Sccsid @(#)n4.c	1.71 (gritter) 8/31/06
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

#include	<stdlib.h>
#include	<string.h>
#include	<ctype.h>
#include	<locale.h>
#include	<limits.h>
#include	<float.h>
#include "tdef.h"
#ifdef NROFF
#include "tw.h"
#endif
#include "pt.h"
#include "ext.h"
/*
 * troff4.c
 * 
 * number registers, conversion, arithmetic
 */


int	regcnt = NNAMES;
int	falsef	= 0;	/* on if inside false branch of if */
#define	NHASH(i)	((i>>6)^i)&0177
struct	numtab	*nhash[128];	/* 128 == the 0177 on line above */

static int	_findr(register int i, int, int);
static struct acc	_atoi(int);
static struct acc	_atoi0(int);
static struct acc	ckph(int);
static struct acc	atoi1(tchar, int);
static struct acc	_inumb(int *, float *, int);

void *
grownumtab(void)
{
	int	i, inc = 20;
	struct numtab	*onc;

	onc = numtab;
	if ((numtab = realloc(numtab, (NN+inc) * sizeof *numtab)) == NULL)
		return NULL;
	memset(&numtab[NN], 0, inc * sizeof *numtab);
	if (NN == 0) {
		for (i = 0; initnumtab[i].r; i++)
			numtab[i] = initnumtab[i];
	} else {
		for (i = 0; i < sizeof nhash / sizeof *nhash; i++)
			if (nhash[i])
				nhash[i] += numtab - onc;
		for (i = 0; i < NN; i++)
			if (numtab[i].link)
				numtab[i].link += numtab - onc;
	}
	NN += inc;
	return numtab;
}

#define	TMYES	if (tm) return(1)
#define	TMNO	if (tm) return(0)

static int
_setn(int tm)	/* tm: test for presence of readonly register */
{
	extern const char	revision[];
	char	tb[30], *cp;
	const char	*name;
	struct s	*s;
	register int i, j;
	register tchar ii;
	int	f;
	float	fl;

	f = nform = 0;
	if (tm) {
		i = tm;
		goto sl;
	}
	if ((i = cbits(ii = getach())) == '+')
		f = 1;
	else if (i == '-')
		f = -1;
	else 
		ch = ii;
	if (falsef)
		f = 0;
	if ((i = getsn(1)) == 0)
		return(0);
sl:
	name = macname(i);
	if (i < 65536 && (i & 0177) == '.')
		switch (i >> BYTE) {
		case 's': 
			i = fl = u2pts(pts);
			if (i != fl)
				goto flt;
			break;
		case 'v': 
			i = lss;		
			break;
		case 'f': 
			i = font;	
			break;
		case 'p': 
			i = pl;		
			break;
		case 't':  
			i = findt1();	
			break;
		case 'o': 
			i = po;		
			break;
		case 'l': 
			i = ll;		
			break;
		case 'i': 
			i = in;		
			break;
		case '$': 
			for (s = frame; s->loopf && s != stk; s = s->pframe);
			i = s->nargs;
			break;
		case 'A': 
			i = ascii;		
			break;
		case 'c': 
			i = numtab[CD].val;		
			break;
		case 'n': 
			i = lastl;		
			break;
		case 'a': 
			i = ralss;		
			break;
		case 'h': 
			i = dip->hnl;	
			break;
		case 'd':
			if (dip != d)
				i = dip->dnl; 
			else 
				i = numtab[NL].val;
			break;
		case 'u': 
			i = fi;		
			break;
		case 'j': 
			i = ad + 2 * admod;	
			break;
		case 'w': 
			i = widthp;
			break;
		case 'x': 
			if (gflag)
				goto s0;
			i = nel - adspc;
			break;
		case 'y': 
			if (gflag)
				goto s0;
			i = un;		
			break;
		case 'T': 
			i = dotT;		
			break; /*-Tterm used in nroff*/
		case 'V': 
			i = VERT;		
			break;
		case 'H': 
			i = HOR;		
			break;
		case 'k': 
			i = ne + adspc;		
			if (gflag) {
				if (ce || rj || !fi ? pendnf : pendw != NULL)
					i += wne - wsp;
				else if (nwd) {
					i += sps;
					if (seflg || spflg)
						i += ses;
				}
			}
			break;
		case 'P': 
			i = print;		
			break;
		case 'L': 
			i = ls;		
			break;
		case 'R': 
			i = NN - regcnt;	
			break;
		case 'z': 
			TMYES;
			cpushback(macname(dip->curd));
			return(0);
		case 'b': 
			i = bdtab[font];
			break;
		case 'F':
			TMYES;
			cpushback(cfname[ifi] ? cfname[ifi] : "");
			return(0);
		case 'X':
			if (xflag) {
				i = xflag;
				break;
			}
			/*FALLTHRU*/
		case 'Y':
			if (xflag) {
				TMYES;
				cpushback((char *)revision);
				return(0);
			}
			/*FALLTHRU*/

		default:
			goto s0;
		}
	else if (name[0] == '.') {
		if (strcmp(&name[1], "warn") == 0)
			i = warn;
		else if (strcmp(&name[1], "vpt") == 0)
			i = vpt;
		else if (strcmp(&name[1], "ascender") == 0)
			i = getascender();
		else if (strcmp(&name[1], "descender") == 0)
			i = getdescender();
		else if (strcmp(&name[1], "fp") == 0)
			i = nextfp();
		else if (strcmp(&name[1], "ss") == 0)
			i = spacesz;
		else if (strcmp(&name[1], "sss") == 0)
			i = sesspsz;
		else if (strcmp(&name[1], "minss") == 0)
			i = minspsz ? minspsz : spacesz;
		else if (strcmp(&name[1], "lshmin") == 0) {
			i = fl = 100 - lshmin / (LAFACT/100.0);
			if (i != fl)
				goto flt;
		} else if (strcmp(&name[1], "lshmax") == 0) {
			i = fl = 100 + lshmax / (LAFACT/100.0);
			if (i != fl)
				goto flt;
		} else if (strcmp(&name[1], "lspmin") == 0) {
			i = fl = 100 - lspmin / (LAFACT/100.0);
			if (i != fl)
				goto flt;
		} else if (strcmp(&name[1], "lspmax") == 0) {
			i = fl = 100 + lspmax / (LAFACT/100.0);
			if (i != fl)
				goto flt;
		} else if (strcmp(&name[1], "letss") == 0)
			i = letspsz;
		else if (strcmp(&name[1], "hlm") == 0)
			i = hlm;
		else if (strcmp(&name[1], "hlc") == 0)
			i = hlc;
		else if (strcmp(&name[1], "lc_ctype") == 0) {
			if ((cp = setlocale(LC_CTYPE, NULL)) == NULL)
				cp = "C";
			TMYES;
			cpushback(cp);
			return(0);
		} else if (strcmp(&name[1], "hylang") == 0) {
			TMYES;
			if (hylang)
				cpushback(hylang);
			return(0);
		} else if (strcmp(&name[1], "fzoom") == 0) {
			i = fl = getfzoom();
			if (i != fl)
				goto flt;
		} else if (strcmp(&name[1], "sentchar") == 0) {
			TMYES;
			if (sentch[0] == IMP)
				/*EMPTY*/;
			else if (sentch[0] == 0)
				cpushback(".?!:");
			else {
				tchar	tc[NSENT+1];
				for (i = 0; sentch[i] && i < NSENT; i++)
					tc[i] = sentch[i];
				tc[i] = 0;
				pushback(tc);
			}
			return(0);
		} else if (strcmp(&name[1], "transchar") == 0) {
			tchar	tc[NSENT+1];
			TMYES;
			if (transch[0] == IMP)
				/*EMPTY*/;
			else if (transch[0] == 0) {
				cpushback("\"')]*");
				tc[0] = DAGGER;
				tc[1] = 0;
				pushback(tc);
			} else {
				for (i = 0; transch[i] && i < NSENT; i++)
					tc[i] = transch[i];
				tc[i] = 0;
				pushback(tc);
			}
			return(0);
		} else if (strcmp(&name[1], "breakchar") == 0) {
			tchar	tc[NSENT+1];
			TMYES;
			if (breakch[0] == IMP)
				/*EMPTY*/;
			else if (breakch[0] == 0) {
				tc[0] = EMDASH;
				tc[1] = '-';
				tc[2] = 0;
				pushback(tc);
			} else {
				for (i = 0; breakch[i] && i < NSENT; i++)
					tc[i] = breakch[i];
				tc[i] = 0;
				pushback(tc);
			}
			return(0);
		} else if (strcmp(&name[1], "nhychar") == 0) {
			tchar	tc[NSENT+1];
			TMYES;
			if (nhych[0] == IMP)
				/*EMPTY*/;
			else if (nhych[0] == 0) {
				if (!hyext) {
					tc[0] = EMDASH;
					tc[1] = '-';
					tc[2] = 0;
					pushback(tc);
				}
			} else {
				for (i = 0; nhych[i] && i < NSENT; i++)
					tc[i] = nhych[i];
				tc[i] = 0;
				pushback(tc);
			}
			return(0);
		} else if (strcmp(&name[1], "shc") == 0) {
			tchar	tc[2];
			TMYES;
			tc[0] = shc ? shc : HYPHEN;
			tc[1] = 0;
			pushback(tc);
			return(0);
		} else if (strcmp(&name[1], "ev") == 0) {
			TMYES;
			cpushback(evname ? evname : "0");
			return(0);
		} else if (strcmp(&name[1], "ps") == 0) {
			i = pts;
#ifdef	NROFF
			i *= INCH / 72;
#endif	/* NROFF */
		} else if (strcmp(&name[1], "tabs") == 0) {
			TMYES;
			for (i = 0; tabtab[i]; i++);
			while (--i >= 0) {
				cp = tb;
				if (i)
					*cp++ = ' ';
				cp = roff_sprintf(cp, "%d", tabtab[i]&TABMASK);
				*cp++ = 'u';
				switch (tabtab[i] & ~TABMASK) {
				case RTAB:
					*cp++ = 'R';
					break;
				case CTAB:
					*cp++ = 'C';
					break;
				}
				*cp = 0;
				cpushback(tb);
			}
			return(0);
		} else if (strcmp(&name[1], "lpfx") == 0) {
			TMYES;
			if (lpfx)
				pushback(lpfx);
			return(0);
		} else if (strcmp(&name[1], "ce") == 0)
			i = ce;
		else if (strcmp(&name[1], "rj") == 0)
			i = rj;
		else if (strcmp(&name[1], "brnl") == 0)
			i = brnl;
		else if (strcmp(&name[1], "brpnl") == 0)
			i = brpnl;
		else if (strcmp(&name[1], "cht") == 0)
			i = cht;
		else if (strcmp(&name[1], "cdp") == 0)
			i = cdp;
		else if (strcmp(&name[1], "in") == 0)
			i = un;
		else if (strcmp(&name[1], "hy") == 0)
			i = hyf;
		else if (strcmp(&name[1], "int") == 0)
			i = ce || rj || !fi ? pendnf : pendw != NULL;
		else if (strcmp(&name[1], "lt") == 0)
			i = lt;
		else if (strcmp(&name[1], "pn") == 0)
			i = npnflg ? npn : numtab[PN].val + 1;
		else if (strcmp(&name[1], "psr") == 0) {
			i = apts;
#ifdef	NROFF
			i *= INCH / 72;
#endif	/* NROFF */
		} else if (strcmp(&name[1], "sr") == 0) {
			i = fl = u2pts(apts);
			if (i != fl)
				goto flt;
		} else if (strcmp(&name[1], "kc") == 0)
			i = wne - wsp;
		else
			goto s0;
	} else {
s0:
		TMNO;
		if ((j = _findr(i, 1, 2)) == -1)
			i = 0;
		else if (j < 0) {
			i = -j;
			goto sl;
		} else if (numtab[j].fmt == -1) {
			fl = numtab[j].fval = (numtab[j].fval+numtab[j].finc*f);
			if (numtab[j].finc)
				prwatchn(j);
			goto flt;
		} else {
			i = numtab[j].val = (numtab[j].val+numtab[j].inc*f);
			if (numtab[j].inc)
				prwatchn(j);
			nform = numtab[j].fmt;
		}
	}
	TMYES;
	setn1(i, nform, (tchar) 0);
	return(0);
flt:
	TMYES;
	roff_sprintf(tb, "%g", fl);
	cpushback(tb);
	return(0);
}

void
setn(void)
{
	_setn(0);
}

tchar	numbuf[17];
tchar	*numbufp;

int 
wrc(tchar i)
{
	if (numbufp >= &numbuf[16])
		return(0);
	*numbufp++ = i;
	return(1);
}



/* insert into input number i, in format form, with size-font bits bits */
void
setn1(int i, int form, tchar bits)
{
	numbufp = numbuf;
	nrbits = bits;
	nform = form;
	fnumb(i, wrc);
	*numbufp = 0;
	pushback(numbuf);
}


void
nrehash(void)
{
	register struct numtab *p;
	register int i;

	for (i=0; i<128; i++)
		nhash[i] = 0;
	for (p=numtab; p < &numtab[NN]; p++)
		p->link = 0;
	for (p=numtab; p < &numtab[NN]; p++) {
		if (p->r == 0)
			continue;
		i = NHASH(p->r);
		p->link = nhash[i];
		nhash[i] = p;
	}
}

void
nunhash(register struct numtab *rp)
{	
	register struct numtab *p;
	register struct numtab **lp;

	if (rp->r == 0)
		return;
	lp = &nhash[NHASH(rp->r)];
	p = *lp;
	while (p) {
		if (p == rp) {
			*lp = p->link;
			p->link = 0;
			return;
		}
		lp = &p->link;
		p = p->link;
	}
}

int
findr(int i)
{
	return _findr(i, 0, 1);
}

static int 
_findr(register int i, int rd, int aln)
{
	register struct numtab *p;
	register int h = NHASH(i);

	if (i == 0 || i == -2)
		return(-1);
	for (p = nhash[h]; p; p = p->link)
		if (i == p->r) {
			if (p->aln < 0) {
				if (aln > 1)
					return(p->aln);
				else if (aln)
					continue;
			}
			if (aln && p->aln > 0)
				return(p->aln - 1);
			return(p - numtab);
		}
	if (rd && warn & WARN_REG)
		errprint("no such register %s", macname(i));
	do {
		for (p = numtab; p < &numtab[NN]; p++) {
			if (p->r == 0) {
				p->r = i;
				p->link = nhash[h];
				nhash[h] = p;
				regcnt++;
				return(p - numtab);
			}
		}
	} while (p == &numtab[NN] && grownumtab() != NULL);
	errprint("too many number registers (%d).", NN);
	done2(04); 
	/* NOTREACHED */
	return 0;
}

static int 
_usedr (	/* returns -1 if nr i has never been used */
    register int i, int aln
)
{
	register struct numtab *p;

	if (i == 0 || i == -2)
		return(-1);
	for (p = nhash[NHASH(i)]; p; p = p->link)
		if (i == p->r) {
			if (p->aln < 0) {
				if (aln > 1)
					return(p->aln);
				else if (aln)
					continue;
			}
			if (aln && p->aln > 0)
				return(p->aln - 1);
			return(p - numtab);
		}
	return -1;
}


int
usedr(int i)
{
	return _usedr(i, 1);
}


int 
fnumb(register int i, register int (*f)(tchar))
{
	register int j;

	j = 0;
	if (i < 0) {
		j = (*f)('-' | nrbits);
		i = -i;
	}
	switch (nform) {
	default:
	case '1':
	case 0: 
		return decml(i, f) + j;
		break;
	case 'i':
	case 'I': 
		return roman(i, f) + j;
		break;
	case 'a':
	case 'A': 
		return abc(i, f) + j;
		break;
	}
}


int 
decml(register int i, register int (*f)(tchar))
{
	register int j, k;

	k = 0;
	nform--;
	if ((j = i / 10) || (nform > 0))
		k = decml(j, f);
	return(k + (*f)((i % 10 + '0') | nrbits));
}


int 
roman(int i, int (*f)(tchar))
{

	if (!i)
		return((*f)('0' | nrbits));
	if (nform == 'i')
		return(roman0(i, f, "ixcmz", "vldw"));
	else 
		return(roman0(i, f, "IXCMZ", "VLDW"));
}


int 
roman0(int i, int (*f)(tchar), char *onesp, char *fivesp)
{
	register int q, rem, k;

	k = 0;
	if (!i)
		return(0);
	k = roman0(i / 10, f, onesp + 1, fivesp + 1);
	q = (i = i % 10) / 5;
	rem = i % 5;
	if (rem == 4) {
		k += (*f)(*onesp | nrbits);
		if (q)
			i = *(onesp + 1);
		else 
			i = *fivesp;
		return(k += (*f)(i | nrbits));
	}
	if (q)
		k += (*f)(*fivesp | nrbits);
	while (--rem >= 0)
		k += (*f)(*onesp | nrbits);
	return(k);
}


int 
abc(int i, int (*f)(tchar))
{
	if (!i)
		return((*f)('0' | nrbits));
	else 
		return(abc0(i - 1, f));
}


int 
abc0(int i, int (*f)(tchar))
{
	register int j, k;

	k = 0;
	if (j = i / 26)
		k = abc0(j - 1, f);
	return(k + (*f)((i % 26 + nform) | nrbits));
}

static int	illscale;
static int	parlevel;
static int	whitexpr;
static int	empty;

static tchar
agetch(void)
{
	tchar	c;

	for (;;) {
		c = getch();
		if (xflag == 0 || parlevel == 0 || cbits(c) != ' ')
			break;
		whitexpr++;
	}
	return c;
}

int
atoi()
{
	struct acc	a;

	lgf++;
	a = _atoi(0);
	lgf--;
	return a.n;
}

float
atof()
{
	struct acc	a;

	lgf++;
	a = _atoi(1);
	lgf--;
	return a.f;
}

static struct acc
_atoi(int flt)
{
	struct acc	n;
	int	c;

	illscale = 0;
	whitexpr = parlevel = 0;
	empty = 1;
	n = _atoi0(flt);
	c = cbits(ch);
	if (c == RIGHT) {
		if (!empty && (nonumb || parlevel) && warn & WARN_RIGHT_BRACE)
			errprint("\\} terminates numerical expression");
	} else if (nonumb && c && c != ' ' && c != '\n' &&
			warn & WARN_NUMBER && illscale == 0) {
		if (c == 'T' && Tflg)
			/*EMPTY*/;
		else if ((c & ~0177) == 0 && isprint(c))
			errprint("illegal number, char %c", c);
		else
			errprint("illegal number");
	} else if (warn & WARN_SYNTAX) {
		if (parlevel > 0)
			errprint("missing ')'");
		if (parlevel < 0)
			errprint("excess ')'");
		if (xflag && whitexpr && parlevel)
			nonumb = 1;
	}
	if (flt) {
		if (!nonumb && (n.f<0 && n.f<-FLT_MAX || n.f>0 && n.f>FLT_MAX)) {
			if (warn & WARN_NUMBER)
				errprint("floating-point arithmetic overflow");
			if (xflag)
				nonumb = 1;
		}
	} else {
		if (!nonumb && (n.n<0 && n.n <INT_MIN || n.n>0 && n.n>INT_MAX)) {
			if (warn & WARN_NUMBER)
				errprint("arithmetic overflow");
			if (xflag)
				nonumb = 1;
		}
	}
	return n;
}

long long
atoi0(void)
{
	struct acc	a;

	whitexpr = parlevel = 0;
	lgf++;
	a = _atoi0(0);
	lgf--;
	return a.n;
}

double
atof0(void)
{
	struct acc	a;

	whitexpr = parlevel = 0;
	lgf++;
	a = _atoi0(0);
	lgf--;
	return a.f;
}

static struct acc
_atoi0(int flt)
{
	register int c, k, cnt;
	register tchar ii;
	struct acc	i, acc;

	i.f = i.n = 0; 
	acc.f = acc.n = 0;
	nonumb = 0;
	cnt = -1;
a0:
	cnt++;
	ii = agetch();
	c = cbits(ii);
	switch (c) {
	default:
		ch = ii;
		if (cnt)
			break;
	case '+':
		i = ckph(flt);
		if (nonumb)
			break;
		acc.n += i.n;
		if (flt) acc.f += i.f;
		goto a0;
	case '-':
		i = ckph(flt);
		if (nonumb)
			break;
		acc.n -= i.n;
		if (flt) acc.f -= i.f;
		goto a0;
	case '*':
		i = ckph(flt);
		if (nonumb)
			break;
		if (!flt) acc.n *= i.n;
		if (flt) acc.f *= i.f;
		goto a0;
	case '/':
		i = ckph(flt);
		if (nonumb)
			break;
		if (!flt && i.n == 0 || flt && i.f == 0) {
			flusho();
			errprint("divide by zero.");
			acc.n = 0;
			if (flt) acc.f = 0;
		} else  {
			if (!flt) acc.n /= i.n;
			if (flt) acc.f /= i.f;
		}
		goto a0;
	case '%':
		i = ckph(flt);
		if (nonumb)
			break;
		if (flt) acc.n = acc.f, i.n = i.f;
		acc.n %= i.n;
		if (flt) acc.f = acc.n;
		goto a0;
	case '&':	/*and*/
		i = ckph(flt);
		if (nonumb)
			break;
		if ((acc.n > 0) && (i.n > 0))
			acc.n = 1; 
		else 
			acc.n = 0;
		if (flt) acc.f = acc.n;
		goto a0;
	case ':':	/*or*/
		i = ckph(flt);
		if (nonumb)
			break;
		if ((acc.n > 0) || (i.n > 0))
			acc.n = 1; 
		else 
			acc.n = 0;
		if (flt) acc.f = acc.n;
		goto a0;
	case '=':
		if (cbits(ii = getch()) != '=')
			ch = ii;
		i = ckph(flt);
		if (nonumb) {
			acc.n = 0; 
			if (flt) acc.f = 0;
			break;
		}
		acc.n = i.n == acc.n;
		if (flt) acc.f = i.f == acc.f;
		goto a0;
	case '>':
		k = 0;
		if (cbits(ii = getch()) == '=')
			k++; 
		else if (xflag && cbits(ii) == '?')
			goto maximum;
		else 
			ch = ii;
		i = ckph(flt);
		if (nonumb) {
			acc.n = 0; 
			if (flt) acc.f = 0;
			break;
		}
		if (!flt) {
			if (acc.n > (i.n - k))
				acc.n = 1; 
			else 
				acc.n = 0;
		} else
			acc.f = k ? acc.f >= i.f : acc.f > i.f;
		goto a0;
	maximum:
		i = ckph(flt);
		if (nonumb) {
			acc.n = 0; 
			if (flt) acc.f = 0;
			break;
		}
		if (i.n > acc.n)
			acc.n = i.n;
		if (flt && i.f > acc.f)
			acc.f = i.f;
		goto a0;
	case '<':
		k = 0;
		if (cbits(ii = getch()) == '=')
			k++; 
		else if (xflag && cbits(ii) == '?')
			goto minimum;
		else if (xflag && cbits(ii) == '>')
			goto notequal;
		else 
			ch = ii;
		i = ckph(flt);
		if (nonumb) {
			acc.n = 0; 
			if (flt) acc.f = 0;
			break;
		}
		if (!flt) {
			if (acc.n < (i.n + k))
				acc.n = 1; 
			else 
				acc.n = 0;
		} else
			acc.f = k ? acc.f <= i.f : acc.f < i.f;
		goto a0;
	minimum:
		i = ckph(flt);
		if (nonumb) {
			acc.n = 0; 
			if (flt) acc.f = 0;
			break;
		}
		if (i.n < acc.n)
			acc.n = i.n;
		if (flt && i.f < acc.f)
			acc.f = i.f;
		goto a0;
	notequal:
		i = ckph(flt);
		if (nonumb) {
			acc.n = 0; 
			if (flt) acc.f = 0;
			break;
		}
		acc.n = i.n != acc.n;
		if (flt) acc.f = i.f != acc.f;
		goto a0;
	case ')': 
		parlevel--;
		break;
	case '(':
		parlevel++;
		acc = _atoi0(flt);
		goto a0;
	}
	return(acc);
}


static struct acc
ckph(int flt)
{
	register tchar i;
	struct acc	j;

	if (cbits(i = agetch()) == '(') {
		parlevel++;
		j = _atoi0(flt);
	} else {
		j = atoi1(i, flt);
	}
	return(j);
}


static struct acc
atoi1(register tchar ii, int flt)
{
	register int i, j, digits;
	struct acc	acc;
	int	neg, abs, field;
	int	_noscale = 0, scale;
	double	e, f;

	neg = abs = field = digits = 0;
	acc.f = acc.n = 0;
	for (;;) {
		i = cbits(ii);
		switch (i) {
		default:
			break;
		case '+':
			ii = agetch();
			continue;
		case '-':
			neg = 1;
			ii = agetch();
			continue;
		case '|':
			abs = 1 + neg;
			neg = 0;
			ii = agetch();
			continue;
		}
		break;
	}
	if (xflag && i == '(') {
		parlevel++;
		acc = _atoi0(flt);
		e = 1;
		i = j = 1;
		field = -1;
		goto aa;
	}
a1:
	while (i >= '0' && i <= '9') {
		field++;
		digits++;
		if (!flt)
			acc.n = 10 * acc.n + i - '0';
		else if (field == digits)
			acc.f = 10 * acc.f + i - '0';
		else {
			f = i - '0';
			for (j = 0; j < digits; j++)
				f /= 10;
			acc.f += f;
		}
		ii = getch();
		i = cbits(ii);
	}
	if (i == '.') {
		field++;
		digits = 0;
		ii = getch();
		i = cbits(ii);
		goto a1;
	}
	e = 1;
	if (xflag && digits && (i == 'e' || i == 'E')) {
		if ((i = cbits(ii = getch())) == '+')
			j = 1;
		else if (i == '-')
			j = -1;
		else if (i >= '0' && i <= '9') {
			j = 1;
			ch = ii;
		} else {
			ch = ii;
			field = 0;
			goto a2;
		}
		f = 0;
		while ((i = cbits(ii = getch())) >= '0' && i <= '9')
			f = f * 10 + i - '0';
		while (f-- > 0)
			e *= 10;
		if (j < 0)
			e = 1/e;
	}
	if ((!xflag || !parlevel) && !field) {
		ch = ii;
		goto a2;
	}
	switch (scale = i) {
	case 's':
		if (!xflag)
			goto dfl;
		/*FALLTHRU*/
	case 'u':
		i = j = 1;	/* should this be related to HOR?? */
		break;
	case 'v':	/*VSs - vert spacing*/
		j = lss;
		i = 1;
		break;
	case 'm':	/*Ems*/
		j = EM;
		i = 1;
		break;
	case 'n':	/*Ens*/
		j = EM;
#ifndef NROFF
		i = 2;
#endif
#ifdef NROFF
		i = 1;	/*Same as Ems in NROFF*/
#endif
		break;
	case 'z':
		if (!xflag)
			goto dfl;
		/*FALLTHRU*/
	case 'p':	/*Points*/
		j = INCH;
		i = 72;
		break;
	case 'i':	/*Inches*/
		j = INCH;
		i = 1;
		break;
	case 'c':	/*Centimeters*/
		/* if INCH is too big, this will overflow */
		j = INCH * 50;
		i = 127;
		break;
	case 'P':	/*Picas*/
		j = INCH;
		i = 6;
		break;
	case 'M':	/*Ems/100*/
		if (!xflag)
			goto dfl;
		j = EM;
		i = 100;
		break;
	case ';':
		if (!xflag)
			goto dfl;
		i = j = 1;
		_noscale = 1;
		goto newscale;
	default:
	dfl:	if (!field) {
			ch = ii;
			goto a2;
		}
		if ((i >= 'a' && i <= 'z' || i >= 'A' && i <= 'Z') &&
				warn & WARN_SCALE) {
			errprint("undefined scale indicator %c", i);
			illscale = 1;
		} else
			scale = 0;
		j = dfact;
		ch = ii;
		i = dfactd;
	}
	if (!field) {
		tchar	t = getch(), tp[2];
		int	f, d, n;
		if (cbits(t) != ';') {
			tp[0] = t;
			tp[1] = 0;
			pushback(tp);
			ch = ii;
			goto a2;
		}
	newscale:
		/* (c;e) */
		f = dfact;
		d = dfactd;
		n = noscale;
		dfact = j;
		dfactd = i;
		noscale = _noscale;
		acc = _atoi0(flt);
		dfact = f;
		dfactd = d;
		noscale = n;
		return(acc);
	}
	if (noscale && scale && warn & WARN_SYNTAX)
		errprint("ignoring scale indicator %c", scale);
aa:
	if (neg) {
		acc.n = -acc.n;
		if (flt) acc.f = -acc.f;
	}
	if (!noscale) {
		if (!flt) acc.n = (acc.n * j) / i;
		if (flt) acc.f = (acc.f * j) / i;
	}
	if (!flt && (field != digits) && (digits > 0))
		while (digits--)
			acc.n /= 10;
	if (e != 1) {
		if (!flt) acc.n = (int)(e * acc.n);
		if (flt) acc.f *= e;
	}
	if (abs) {
		if (dip != d)
			j = dip->dnl; 
		else 
			j = numtab[NL].val;
		if (!vflag) {
			j = numtab[HP].val;
		}
		if (abs == 2)
			j = -j;
		acc.n -= j;
		if (flt) acc.f -= j;
	}
a2:
	nonumb = !field;
	if (empty)
		empty = !field;
	return(acc);
}


void
setnr(const char *name, int val, int inc)
{
	int	i, j;

	if ((j = makerq(name)) < 0)
		return;
	if ((i = findr(j)) < 0)
		return;
	numtab[i].val = val;
	numtab[i].inc = inc;
	if (numtab[i].fmt == -1)
		numtab[i].fmt = 0;
	prwatchn(i);
}

void
setnrf(const char *name, float val, float inc)
{
	int	i, j;

	if ((j = makerq(name)) < 0)
		return;
	if ((i = findr(j)) < 0)
		return;
	numtab[i].val = numtab[i].fval = val;
	numtab[i].inc = numtab[i].finc = inc;
	numtab[i].fmt = -1;
	prwatchn(i);
}


static void
clrnr(int k)
{
	struct numtab	*p;

	if (k >= 0) {
		p = &numtab[k];
		memset(p, 0, sizeof *p);
		regcnt--;
	}
}

void
caserr(void)
{
	register int i, j, k;
	register struct numtab *p;
	int cnt = 0;

	lgf++;
	while (!skip(!cnt++) && (i = getrq(2)) ) {
		j = _usedr(i, 0);
		if (j < 0) {
			if (warn & WARN_REG)
				errprint("no such register %s", macname(i));
			continue;
		}
		p = &numtab[j];
		nunhash(p);
		if (p->aln && (k = _usedr(i, 1)) >= 0) {
			if (--numtab[k].nlink <= 0)
				clrnr(k);
		}
		if (p->flags & FLAG_WATCH)
			errprint("%s: removing register %s", macname(lastrq),
					macname(i));
		if (p->nlink > 0)
			p->nlink--;
		if (p->nlink == 0)
			clrnr(j);
		else
			p->r = -1;
	}
}


void
casernn(void)
{
	int	i, j, k, n;

	lgf++;
	skip(1);
	if ((i = getrq(0)) == 0)
		return;
	if ((k = _usedr(i, 0)) < 0) {
		if (warn & WARN_REG)
			errprint("no such register %s", macname(i));
		return;
	}
	skip(1);
	j = getrq(1);
	n = _findr(j, 0, 0);
	if (n >= 0) {
		if (numtab[n].nlink) {
			numtab[n].nlink--;
			numtab[n].r = -1;
		}
		n = _findr(j, 0, 0);
	}
	if (n >= 0) {
		numtab[n] = numtab[k];
		numtab[n].r = j;
	}
	clrnr(k);
	if (numtab[n].flags & FLAG_WATCH)
		errprint("%s: renaming register %s to %s", macname(lastrq),
				macname(i), macname(j));
}


void
setr(void)
{
	int	termc, i, j;

	lgf++;
	termc = getach();
	j = getrq(3);
	lgf--;
	if ((i = findr(j)) == -1 || skip(1))
		return;
	j = inumb(&numtab[i].val);
	if (nonumb)
		return;
	if (getach() != termc) {
		nodelim(termc);
		return;
	}
	numtab[i].val = j;
	if (numtab[i].fmt == -1)
		numtab[i].fmt = 0;
	prwatchn(i);
}

void
casenr(int flt)
{
	register int i, j;
	struct acc	a;

	lgf++;
	skip(1);
	j = getrq(3);
	if ((i = findr(j)) == -1)
		goto rtn;
	skip(1);
	a = _inumb(&numtab[i].val, flt ? &numtab[i].fval : NULL, flt);
	if (nonumb)
		goto rtn;
	numtab[i].val = a.n;
	if (flt) {
		numtab[i].fval = a.f;
		numtab[i].fmt = -1;
	} else if (numtab[i].fmt == -1)
		numtab[i].fmt = 0;
	/*
	 * It is common use in pre-processors and macro packages
	 * to append a unit definition to a user-supplied number
	 * in order to achieve a default scale. Catch this case
	 * now to avoid a warning because of an illegal number.
	 */
	j = cbits(ch);
	if ((j >= 'a' && j <= 'z' || j >= 'A' && j <= 'Z') &&
			warn & WARN_SCALE)
		goto rtns;
	skip(0);
	a = _atoi(flt);
	if (nonumb)
		goto rtns;
	numtab[i].inc = a.n;
	if (flt)
		numtab[i].finc = a.f;
rtns:
	prwatchn(i);
rtn:
	return;
}

void
casenrf(void)
{
	casenr(1);
}


void
caseaf(void)
{
	register int i, k, n;
	register tchar j, jj;

	lgf++;
	if (skip(1) || !(i = getrq(3)))
		return;
	if (skip(1))
		return;
	k = 0;
	j = getch();
	if (!ischar(jj = cbits(j)) || !isalpha(jj)) {
		ch = j;
		while ((j = cbits(getch())) >= '0' &&  j <= '9')
			k++;
	}
	if (!k)
		k = j;
	n = findr(i);
	if (numtab[n].fmt == -1) {
		if (warn & WARN_RANGE)
			errprint("cannot change format of floating-point register");
		return;
	}
	numtab[n].fmt = k & BYTEMASK;
	if (numtab[n].flags & FLAG_WATCH) {
		char	b[40];
		int	x;
		if (k & BYTEMASK > ' ') {
			b[0] = k & BYTEMASK;
			b[1] = 0;
		} else {
			for (x = 0; x < k; x++)
				b[x] = '0';
			b[x] = 0;
		}
		errprint("%s: format of register %s set to %s", macname(lastrq),
				macname(i), b);
	}
}

void
setaf (void)	/* return format of number register */
{
	register int i, j;

	i = usedr(getsn(0));
	if (i == -1)
		return;
	if (numtab[i].fmt > 20)	/* it was probably a, A, i or I */
		pbbuf[pbp++] = numtab[i].fmt;
	else if (numtab[i].fmt == -1)
		pbbuf[pbp++] = '0';
	else {
		for (j = (numtab[i].fmt ? numtab[i].fmt : 1); j; j--) {
			if (pbp >= pbsize-3)
				if (growpbbuf() == NULL) {
					errprint("no space for .af");
					done(2);
				}
			pbbuf[pbp++] = '0';
		}
	}
}


void
casealn(void)
{
	int	i, j, r, o;
	int	flags;

	if (skip(1))
		return;
	i = getrq(1);
	if (skip(1))
		return;
	j = getrq(1);
	if (((o = _usedr(j, 2)) < 0)) {
		if (o < -1)
			/*EMPTY*/;
		else if (_setn(j))
			o = -j;
		else {
			if (warn & WARN_REG)
				errprint("no such register %s", macname(j));
			return;
		}
	}
	if ((r = _findr(i, 0, 0)) < 0)
		return;
	if (o >= 0) {
		numtab[r].aln = o + 1;
		if (numtab[o].nlink == 0)
			numtab[o].nlink = 1;
		numtab[o].nlink++;
	} else
		numtab[r].aln = o;
	flags = numtab[r].flags;
	if (o >= 0)
		flags |= numtab[o].flags;
	if (flags & FLAG_WATCH)
		errprint("%s: creating alias %s to register %s",
				macname(lastrq), macname(i), macname(j));
}


void
casewatchn(int unwatch)
{
	int	i, j;

	lgf++;
	if (skip(1))
		return;
	do {
		if (!(j = getrq(1)) || (i = findr(j)) == -1)
			break;
		if (unwatch)
			numtab[i].flags &= ~FLAG_WATCH;
		else
			numtab[i].flags |= FLAG_WATCH;
	} while (!skip(0));
}


void
caseunwatchn(void)
{
	casewatchn(1);
}


void
prwatchn(int i)
{
	if (i < 0 || i >= NN)
		return;
	if (numtab[i].flags & FLAG_WATCH) {
		if (numtab[i].fmt == -1)
			errprint("%s: floating-point register %s set to %g, increment %g",
					macname(lastrq), macname(numtab[i].r),
					numtab[i].fval, numtab[i].finc);
		else
			errprint("%s: register %s set to %d, increment %d",
					macname(lastrq), macname(numtab[i].r),
					numtab[i].val, numtab[i].inc);
	}
}


int 
vnumb(int *i)
{
	vflag++;
	dfact = lss;
	res = VERT;
	return(inumb(i));
}


int 
hnumb(int *i)
{
	dfact = EM;
	res = HOR;
	return(inumb(i));
}


int 
inumb(int *n)
{
	struct acc	a;

	a = _inumb(n, NULL, 0);
	return a.n;
}

static struct acc
_inumb(int *n, float *fp, int flt)
{
	struct acc	i;
	register int j, f;
	register tchar ii;

	f = 0;
	lgf++;
	if (n) {
		if ((j = cbits(ii = getch())) == '+')
			f = 1;
		else if (j == '-')
			f = -1;
		else 
			ch = ii;
	}
	i = _atoi(flt);
	lgf--;
	if (n && f && !flt)
		i.n = *n + f * i.n;
	if (fp && f && flt)
		i.f = *fp + f * i.f;
	if (!flt) i.n = quant(i.n, res);
	vflag = 0;
	res = dfactd = dfact = 1;
	if (nonumb) {
		i.n = 0;
		if (flt) i.f = 0;
	}
	return(i);
}


int 
quant(int n, int m)
{
	register int i, neg;

	neg = 0;
	if (n < 0) {
		neg++;
		n = -n;
	}
	/* better as i = ((n + (m/2))/m)*m */
	i = n / m;
	if ((n - m * i) > (m / 2))
		i += 1;
	i *= m;
	if (neg)
		i = -i;
	return(i);
}


tchar
moflo(int n)
{
	if (warn & WARN_RANGE)
		errprint("value too large for motion");
	return sabsmot(MAXMOT);
}
