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
 * Copyright 2004 Sun Microsystems, Inc.  All rights reserved.
 * Use is subject to license terms.
 */

/*	Copyright (c) 1984, 1986, 1987, 1988, 1989 AT&T	*/
/*	  All Rights Reserved  	*/


/*	from OpenSolaris "n3.c	1.11	05/06/08 SMI"	*/

/*
 * Portions Copyright (c) 2005 Gunnar Ritter, Freiburg i. Br., Germany
 *
 * Sccsid @(#)n3.c	1.41 (gritter) 9/4/05
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
 * troff3.c
 * 
 * macro and string routines, storage allocation
 */


#include <stdlib.h>
#include <string.h>
#include "tdef.h"
#ifdef NROFF
#include "tw.h"
#endif
#include "proto.h"
#include "ext.h"
#include <unistd.h>

#define	MHASH(x)	((x>>6)^x)&0177
struct	contab *mhash[128];	/* 128 == the 0177 on line above */
#define	blisti(i)	(((i)-ENV_BLK*BLK) / BLK)
filep	*blist;
int	nblist;
tchar	*argtop;
int	pagech = '%';
int	strflg;

#ifdef	INCORE
	tchar *wbuf;
	tchar *corebuf;
#else
	tchar wbuf[BLK];
	tchar rbuf[BLK];
#endif

static void	caseshift(void);
static int	getls(int);
static void	addcon(int, char *, void(*)(int));

void *
growcontab(void)
{
	int	i, inc = 256;
	struct contab	*onc;

	onc = contab;
	if ((contab = realloc(contab, (NM+inc) * sizeof *contab)) == NULL)
		return NULL;
	memset(&contab[NM], 0, inc * sizeof *contab);
	if (NM == 0) {
		for (i = 0; initcontab[i].f; i++)
			contab[i] = initcontab[i];
		addcon(i++, "fpost", (void(*)(int))casefpost);
		addcon(i++, "track", (void(*)(int))casetrack);
		addcon(i++, "lc_ctype", (void(*)(int))caselc_ctype);
		addcon(i++, "fallback", (void(*)(int))casefallback);
		addcon(i++, "hidechar", (void(*)(int))casehidechar);
		addcon(i++, "evc", (void(*)(int))caseevc);
		addcon(i++, "return", (void(*)(int))casereturn);
		addcon(i++, "chop", (void(*)(int))casechop);
		addcon(i++, "fzoom", (void(*)(int))casefzoom);
		addcon(i++, "kern", (void(*)(int))casekern);
		addcon(i++, "hylang", (void(*)(int))casehylang);
		addcon(i++, "flig", (void(*)(int))caseflig);
		addcon(i++, "papersize", (void(*)(int))casepapersize);
		addcon(i++, "shift", (void(*)(int))caseshift);
	} else {
		for (i = 0; i < sizeof mhash / sizeof *mhash; i++)
			if (mhash[i])
				mhash[i] += contab - onc;
		for (i = 0; i < NM; i++)
			if (contab[i].link)
				contab[i].link += contab - onc;
	}
	NM += inc;
	return contab;
}

void *
growblist(void)
{
	int	inc = 512;
#ifdef	INCORE
	tchar	*ocb;
#endif	/* INCORE */

	if (nblist+inc > XBLIST)
		return NULL;
	if ((blist = realloc(blist, (nblist+inc) * sizeof *blist)) == NULL)
		return NULL;
	memset(&blist[nblist], 0, inc * sizeof *blist);
#ifdef	INCORE
	ocb = corebuf;
	if ((corebuf = realloc(corebuf, (ENV_BLK+nblist+inc+1)
					* BLK * sizeof *corebuf)) == NULL)
		return NULL;
	if (ocb == NULL)
		memset(corebuf, 0, (ENV_BLK+1) * BLK * sizeof *corebuf);
	memset(&corebuf[(ENV_BLK+nblist+1) * BLK], 0,
			inc * BLK * sizeof *corebuf);
	if (wbuf)
		wbuf += corebuf - ocb;
#endif	/* INCORE */
	nblist += inc;
	return blist;
}

void
caseig(void)
{
	register int i;
	register filep oldoff;

	oldoff = offset;
	offset = 0;
	i = copyb();
	offset = oldoff;
	if (i != '.')
		control(i, 1);
}


void
casern(void)
{
	register int i, j;

	lgf++;
	skip();
	if ((i = getrq()) == 0)
		return;
	if (i >= 256)
		i = maybemore(i, 0);
	if ((oldmn = findmn(i)) < 0)
		return;
	skip();
	j = getrq();
	if (j >= 256)
		j = maybemore(j, 1);
	clrmn(findmn(j));
	if (j) {
		munhash(&contab[oldmn]);
		contab[oldmn].rq = j;
		maddhash(&contab[oldmn]);
	}
}

void
maddhash(register struct contab *rp)
{
	register struct contab **hp;

	if (rp->rq == 0)
		return;
	hp = &mhash[MHASH(rp->rq)];
	rp->link = *hp;
	*hp = rp;
}

void
munhash(register struct contab *mp)
{	
	register struct contab *p;
	register struct contab **lp;

	if (mp->rq == 0)
		return;
	lp = &mhash[MHASH(mp->rq)];
	p = *lp;
	while (p) {
		if (p == mp) {
			*lp = p->link;
			p->link = 0;
			return;
		}
		lp = &p->link;
		p = p->link;
	}
}

void
mrehash(void)
{
	register struct contab *p;
	register int i;

	for (i=0; i<128; i++)
		mhash[i] = 0;
	for (p=contab; p < &contab[NM]; p++)
		p->link = 0;
	for (p=contab; p < &contab[NM]; p++) {
		if (p->rq == 0)
			continue;
		i = MHASH(p->rq);
		p->link = mhash[i];
		mhash[i] = p;
	}
}

void
caserm(void)
{
	int j;

	lgf++;
	while (!skip() && (j = getrq()) != 0) {
		if (j >= 256)
			j = maybemore(j, 0);
		clrmn(findmn(j));
	}
	lgf--;
}


void
caseas(void)
{
	app++;
	caseds();
}


void
caseds(void)
{
	ds++;
	casede();
}


void
caseam(void)
{
	app++;
	casede();
}


void
casede(void)
{
	register int i, req;
	register filep savoff;

	if (dip != d)
		wbfl();
	req = '.';
	lgf++;
	skip();
	if ((i = getrq()) == 0)
		goto de1;
	if (i >= 256)
		i = maybemore(i, 1);
	if ((offset = finds(i)) == 0)
		goto de1;
	if (ds)
		copys();
	else 
		req = copyb();
	wbfl();
	clrmn(oldmn);
	if (newmn) {
		if (contab[newmn].rq)
			munhash(&contab[newmn]);
		contab[newmn].rq = i;
		maddhash(&contab[newmn]);
	}
	if (apptr) {
		savoff = offset;
		offset = apptr;
		wbt((tchar) IMP);
		offset = savoff;
	}
	offset = dip->op;
	if (req != '.')
		control(req, 1);
de1:
	ds = app = 0;
	return;
}


int 
findmn(register int i)
{
	register struct contab *p;

	for (p = mhash[MHASH(i)]; p; p = p->link)
		if (i == p->rq)
			return(p - contab);
	return(-1);
}


void
clrmn(register int i)
{
	if (i >= 0) {
		if (contab[i].mx)
			ffree((filep)contab[i].mx);
		munhash(&contab[i]);
		contab[i].rq = 0;
		contab[i].mx = 0;
		contab[i].f = 0;
	}
}


filep 
finds(register int mn)
{
	register int i;
	register filep savip;

	oldmn = findmn(mn);
	newmn = 0;
	apptr = (filep)0;
	if (app && oldmn >= 0 && contab[oldmn].mx) {
		savip = ip;
		ip = (filep)contab[oldmn].mx;
		oldmn = -1;
		while ((i = rbf()) != 0)
			;
		apptr = ip;
		if (!diflg)
			ip = incoff(ip);
		nextb = ip;
		ip = savip;
	} else {
		for (i = 0; i < NM; i++) {
			if (contab[i].rq == 0)
				break;
		}
		if (i == NM && growcontab() == NULL || (nextb = alloc()) == 0) {
			app = 0;
			if (macerr++ > 1)
				done2(02);
			errprint("Too many (%d) string/macro names", NM);
			edone(04);
			return(offset = 0);
		}
		contab[i].mx = (unsigned) nextb;
		if (!diflg) {
			newmn = i;
			if (oldmn == -1)
				contab[i].rq = -1;
		} else {
			contab[i].rq = mn;
			maddhash(&contab[i]);
		}
	}
	app = 0;
	return(offset = nextb);
}


int 
skip (void)			/*skip over blanks; return nlflg*/
{
	register tchar i;

	while (cbits(i = getch()) == ' ')
		;
	ch = i;
	return(nlflg);
}


int 
copyb(void)
{
	register int i, j, state;
	register tchar ii;
	int	req, k;
	filep savoff = 0;

	if (skip() || !(j = getrq()))
		j = '.';
	if (j >= 256)
		maybemore(j, 1);
	req = j;
	k = j >> BYTE;
	j &= BYTEMASK;
	copyf++;
	flushi();
	nlflg = 0;
	state = 1;

/* state 0	eat up
 * state 1	look for .
 * state 2	look for first char of end macro
 * state 3	look for second char of end macro
 */

	while (1) {
		i = cbits(ii = getch());
		if (state == 3) {
			if (i == k)
				break;
			if (!k) {
				ch = ii;
				i = getach();
				ch = ii;
				if (!i)
					break;
			}
			state = 0;
			goto c0;
		}
		if (i == '\n') {
			state = 1;
			nlflg = 0;
			goto c0;
		}
		if (state == 1 && i == '.') {
			state++;
			savoff = offset;
			goto c0;
		}
		if ((state == 2) && (i == j)) {
			state++;
			goto c0;
		}
		state = 0;
c0:
		if (offset)
			wbf(ii);
	}
	if (offset) {
		wbfl();
		offset = savoff;
		wbt((tchar)0);
	}
	copyf--;
	return(req);
}


void
copys(void)
{
	register tchar i;

	copyf++;
	if (skip())
		goto c0;
	if (cbits(i = getch()) != '"')
		wbf(i);
	while (cbits(i = getch()) != '\n')
		wbf(i);
c0:
	wbt((tchar)0);
	copyf--;
}


filep 
alloc (void)		/*return free blist[] block in nextb*/
{
	register int i;
	register filep j;

	do {
		for (i = 0; i < nblist; i++) {
			if (blist[i] == 0)
				break;
		}
	} while (i == nblist && growblist() != NULL);
	if (i == nblist) {
		j = 0;
	} else {
		blist[i] = -1;
		j = (filep)i * BLK + ENV_BLK * BLK;
	}
#ifdef	DEBUG
	if (debug & DB_ALLC) {
		char cc1, cc2;
		fdprintf(stderr, "alloc: ");
		if (oldmn >= 0 && oldmn < NM) {
			cc1 = contab[oldmn].rq & 0177;
			if ((cc2 = (contab[oldmn].rq >> BYTE) & 0177) == 0)
				cc2 = ' ';
			fdprintf(stderr, "oldmn %d %c%c, ", oldmn, cc1, cc2);
		}
		fdprintf(stderr, "newmn %d; nextb was %x, will be %x\n",
			newmn, nextb, j);
	}
#endif	/* DEBUG */
	return(nextb = j);
}


void
ffree (		/*free blist[i] and blocks pointed to*/
    filep i
)
{
	register int j;

	while (blist[j = blisti(i)] != (unsigned) ~0) {
		i = (filep) blist[j];
		blist[j] = 0;
	}
	blist[j] = 0;
}

void
wbt(tchar i)
{
	wbf(i);
	wbfl();
}


void
wbf (			/*store i into blist[offset] (?) */
    register tchar i
)
{
	register int j;

	if (!offset)
		return;
	if (!woff) {
		woff = offset;
#ifdef INCORE
		wbuf = &corebuf[woff];	/* INCORE only */
#endif
		wbfi = 0;
	}
	wbuf[wbfi++] = i;
	if (!((++offset) & (BLK - 1))) {
		wbfl();
		j = blisti(--offset);
		if (j < 0 || j >= nblist && growblist() == NULL) {
			errprint("Out of temp file space");
			done2(01);
		}
		if (blist[j] == (unsigned) ~0) {
			if (alloc() == 0) {
				errprint("Out of temp file space");
				done2(01);
			}
			blist[j] = (unsigned)(nextb);
		}
		offset = ((filep)blist[j]);
	}
	if (wbfi >= BLK)
		wbfl();
}


void
wbfl (void)			/*flush current blist[] block*/
{
	if (woff == 0)
		return;
#ifndef INCORE
	lseek(ibf, woff * sizeof(tchar), 0);
	write(ibf, (char *)wbuf, wbfi * sizeof(tchar));
#endif
	if ((woff & (~(BLK - 1))) == (roff & (~(BLK - 1))))
		roff = -1;
	woff = 0;
}


tchar 
rbf (void)		/*return next char from blist[] block*/
{
	register tchar i;
	register filep j, p;

	if (ip == XBLIST*BLK) {		/* for rdtty */
		if (j = rdtty())
			return(j);
		else
			return(popi());
	}
	/* this is an inline expansion of rbf0: dirty! */
#ifndef INCORE
	j = ip & ~(BLK - 1);
	if (j != roff) {
		roff = j;
		lseek(ibf, j * sizeof(tchar), 0);
		if (read(ibf, (char *)rbuf, BLK * sizeof(tchar)) <= 0)
			i = 0;
		else
			i = rbuf[ip & (BLK-1)];
	} else
		i = rbuf[ip & (BLK-1)];
#else
	i = corebuf[ip];
#endif
	/* end of rbf0 */
	if (i == 0) {
		if (!app)
			i = popi();
		return(i);
	}
	/* this is an inline expansion of incoff: also dirty */
	p = ++ip;
	if ((p & (BLK - 1)) == 0) {
		if ((ip = blist[blisti(p-1)]) == (unsigned) ~0) {
			errprint("Bad storage allocation");
			ip = 0;
			done2(-5);
		}
		/* this was meant to protect against people removing
		 * the macro they were standing on, but it's too
		 * sensitive to block boundaries.
		 * if (ip == 0) {
		 *	errprint("Block removed while in use");
		 *	done2(-6);
		 * }
		 */
	}
	return(i);
}


tchar 
rbf0(register filep p)
{
#ifndef INCORE
	register filep i;

	if ((i = p & ~(BLK - 1)) != roff) {
		roff = i;
		lseek(ibf, roff * sizeof(tchar), 0);
		if (read(ibf, (char *)rbuf, BLK * sizeof(tchar)) == 0)
			return(0);
	}
	return(rbuf[p & (BLK-1)]);
#else
	return(corebuf[p]);
#endif
}


filep 
incoff (		/*get next blist[] block*/
    register filep p
)
{
	p++;
	if ((p & (BLK - 1)) == 0) {
		if ((p = blist[blisti(p-1)]) == (unsigned) ~0) {
			errprint("Bad storage allocation");
			done2(-5);
		}
	}
	return(p);
}


tchar 
popi(void)
{
	register struct s *p;

	if (frame == stk)
		return(0);
	if (strflg)
		strflg--;
	p = nxf = frame;
	p->nargs = 0;
	frame = p->pframe;
	ip = p->pip;
	pendt = p->ppendt;
	lastpbp = p->lastpbp;
	return(p->pch);
}

/*
 *	test that the end of the allocation is above a certain location
 *	in memory
 */
#define SPACETEST(base, size) while ((enda - (size)) <= (char *)(base)){setbrk(DELTA);}

int 
pushi(filep newip, int mname)
{
	register struct s *p;

	SPACETEST(nxf, sizeof(struct s));
	p = nxf;
	p->pframe = frame;
	p->pip = ip;
	p->ppendt = pendt;
	p->pch = ch;
	p->lastpbp = lastpbp;
	p->mname = mname;
	lastpbp = pbp;
	pendt = ch = 0;
	frame = nxf;
	if (nxf->nargs == 0) 
		nxf += 1;
	else 
		nxf = (struct s *)argtop;
	return(ip = newip);
}


char *
setbrk(int x)
{
	register char	*i, *k;
	register int j;

	if ((i = sbrk(x)) == (char *) -1) {
		errprint("Core limit reached");
		edone(0100);
	}
	if (j = (unsigned)i % sizeof(int)) {	/*check alignment for 3B*/
		j = sizeof(int) - j;		/*only init calls should need this*/
		if ((k = sbrk(j)) == (char *) -1) {
			errprint("Core limit reached");
			edone(0100);
		}
		if (k != i + x) {	/*there must have been an intervening sbrk*/
			errprint ("internal error in setbrk: i=%x, j=%d, k=%x",
				i, j, k);
			edone(0100);
		}
		i += j;
	}
	enda = i + x;
	return(i);
}


int 
getsn(void)
{
	register int i;

	if ((i = getach()) == 0)
		return(0);
	if (i == '(')
		return(getrq());
	else if (i == '[' && xflag != 0)
		return(getls(']'));
	else 
		return(i);
}


int 
setstr(void)
{
	register int i, j;

	lgf++;
	if ((i = getsn()) == 0 || (j = findmn(i)) == -1 || !contab[j].mx) {
		lgf--;
		return(0);
	} else {
		SPACETEST(nxf, sizeof(struct s));
		nxf->nargs = 0;
		strflg++;
		lgf--;
		return pushi((filep)contab[j].mx, i);
	}
}

static int	APERMAC = 9;

void
collect(void)
{
	register int j;
	register tchar i;
	register tchar *strp;
	tchar * lim;
	tchar * *argpp, **argppend;
	int	quote;
	struct s *savnxf;

	copyf++;
	nxf->nargs = 0;
	savnxf = nxf;
	if (skip())
		goto rtn;

	{
		char *memp;
		memp = (char *)savnxf;
		/*
		 *	1 s structure for the macro descriptor
		 *	APERMAC tchar *'s for pointers into the strings
		 *	space for the tchar's themselves
		 */
		memp += sizeof(struct s);
		/*
		 *	CPERMAC (the total # of characters for ALL arguments)
		 *	to a macro
		 */
#define	CPERMAC	200
		if (xflag)
			APERMAC = 200;
		memp += APERMAC * sizeof(tchar *);
		memp += CPERMAC * sizeof(tchar);
		nxf = (struct s*)memp;
	}
	lim = (tchar *)nxf;
	argpp = (tchar **)(savnxf + 1);
	argppend = &argpp[APERMAC];
	SPACETEST(argppend, sizeof(tchar *));
	strp = (tchar *)argppend;
	/*
	 *	Zero out all the string pointers before filling them in.
	 */
	for (j = 0; j < APERMAC; j++){
		argpp[j] = (tchar *)0;
	}
#if 0
	errprint("savnxf=0x%x,nxf=0x%x,argpp=0x%x,strp=argppend=0x%x,lim=0x%x,enda=0x%x",
		savnxf, nxf, argpp, strp, lim, enda);
#endif
	strflg = 0;
	while ((argpp != argppend) && (!skip())) {
		*argpp++ = strp;
		quote = 0;
		if (cbits(i = getch()) == '"')
			quote++;
		else 
			ch = i;
		while (1) {
			i = getch();
			if (nlflg || (!quote && cbits(i) == ' '))
				break;
			if (   quote
			    && (cbits(i) == '"')
			    && (cbits(i = getch()) != '"')) {
				ch = i;
				break;
			}
			*strp++ = i;
			if (strflg && strp >= lim) {
#if 0
				errprint("strp=0x%x, lim = 0x%x",
					strp, lim);
#endif
				errprint("Macro argument too long");
				copyf--;
				edone(004);
			}
			SPACETEST(strp, 3 * sizeof(tchar));
		}
		*strp++ = 0;
	}
	nxf = savnxf;
	nxf->nargs = argpp - (tchar **)(savnxf + 1);
	argtop = strp;
rtn:
	copyf--;
}


void
seta(void)
{
	register int c, i;
	char q[] = { 0, 0 };

	switch (c = cbits(getch())) {
	case '@':
		q[0] = '"';
		/*FALLTHRU*/
	case '*':
		if (xflag == 0)
			goto dfl;
		for (i = min(APERMAC, frame->nargs); i >= 1; i--) {
			if (q[0])
				cpushback(q);
			pushback(*(((tchar **)(frame + 1)) + i - 1));
			if (q[0])
				cpushback(q);
			if (i > 1)
				cpushback(" ");
		}
		break;
	case '(':
		if (xflag == 0)
			goto dfl;
		c = cbits(getch());
		i = 10 * (c - '0');
		c = cbits(getch());
		i += c - '0';
		goto assign;
	case '[':
		if (xflag == 0)
			goto dfl;
		i = 0;
		while ((c = cbits(getch())) != ']' && c != '\n' && c != 0)
			i = 10 * i + (c - '0');
		goto assign;
	default:
	dfl:	i = c - '0';
	assign:	if (i > 0 && i <= APERMAC && i <= frame->nargs)
			pushback(*(((tchar **)(frame + 1)) + i - 1));
	}
}

void
caseshift(void)
{
	int	i, j;

	if (skip())
		i = 1;
	else
		i = atoi();
	if (nonumb)
		return;
	if (i > 0 && i <= APERMAC && i <= frame->nargs) {
		frame->nargs -= i;
		for (j = 1; j <= frame->nargs; j++)
			*(((tchar **)(frame + 1)) + j - 1) =
				*(((tchar **)(frame + 1)) + j + i - 1);
	}
}

void
caseda(void)
{
	app++;
	casedi();
}


void
casedi(void)
{
	register int i, j;
	register int *k;

	lgf++;
	if (skip() || (i = getrq()) == 0) {
		if (dip != d)
			wbt((tchar)0);
		if (dilev > 0) {
			numtab[DN].val = dip->dnl;
			numtab[DL].val = dip->maxl;
			dip = &d[--dilev];
			offset = dip->op;
		}
		goto rtn;
	}
	if (++dilev == NDI) {
		--dilev;
		errprint("Diversions nested too deep");
		edone(02);
	}
	if (dip != d)
		wbt((tchar)0);
	diflg++;
	dip = &d[dilev];
	if (i >= 256)
		i = maybemore(i, 1);
	dip->op = finds(i);
	dip->curd = i;
	clrmn(oldmn);
	k = (int *) & dip->dnl;
	for (j = 0; j < 10; j++)
		k[j] = 0;	/*not op and curd*/
rtn:
	app = 0;
	diflg = 0;
}


void
casedt(void)
{
	lgf++;
	dip->dimac = dip->ditrap = dip->ditf = 0;
	skip();
	dip->ditrap = vnumb((int *)0);
	if (nonumb)
		return;
	skip();
	dip->dimac = getrq();
	if (dip->dimac >= 256)
		dip->dimac = maybemore(dip->dimac, 1);
}


void
casetl(void)
{
	register int j;
	int w[3];
	tchar buf[LNSIZE];
	register tchar *tp;
	tchar i, delim, nexti;
	int oev;

	dip->nls = 0;
	skip();
	if (ismot(delim = getch())) {
		ch = delim;
		delim = '\'';
	} else 
		delim = cbits(delim);
	tp = buf;
	numtab[HP].val = 0;
	w[0] = w[1] = w[2] = 0;
	j = 0;
	nexti = getch();
	while (cbits(i = nexti) != '\n') {
		if (cbits(i) == cbits(delim)) {
			if (j < 3)
				w[j] = numtab[HP].val;
			numtab[HP].val = 0;
			j++;
			*tp++ = 0;
			nexti = getch();
		} else {
			if (cbits(i) == pagech) {
				setn1(numtab[PN].val, numtab[findr('%')].fmt,
				      i&SFMASK);
				nexti = getch();
				continue;
			}
			numtab[HP].val += width(i);
			oev = ev;
			nexti = getch();
			if (ev == oev)
				numtab[HP].val += kernadjust(i, nexti);
			if (tp < &buf[LNSIZE-10])
				*tp++ = i;
		}
	}
	if (j<3)
		w[j] = numtab[HP].val;
	*tp++ = 0;
	*tp++ = 0;
	*tp++ = 0;
	tp = buf;
#ifdef NROFF
	horiz(po);
#endif
	while (i = *tp++)
		pchar(i);
	if (w[1] || w[2])
		horiz(j = quant((lt - w[1]) / 2 - w[0], HOR));
	while (i = *tp++)
		pchar(i);
	if (w[2]) {
		horiz(lt - w[0] - w[1] - w[2] - j);
		while (i = *tp++)
			pchar(i);
	}
	newline(0);
	if (dip != d) {
		if (dip->dnl > dip->hnl)
			dip->hnl = dip->dnl;
	} else {
		if (numtab[NL].val > dip->hnl)
			dip->hnl = numtab[NL].val;
	}
}

void
casepc(void)
{
	pagech = chget(IMP);
}

void
casechop(void)
{
	int	a = app;
	int	i, j;
	filep	savip, savoffset;

	skip();
	if ((i = getrq()) == 0)
		return;
	if (i >= 256)
		i = maybemore(i, 0);
	if ((j = findmn(i)) < 0)
		return;
	savip = ip;
	ip = (filep)contab[j].mx;
	app = 1;
	while ((i = rbf()) != 0)
		i = 1;
	app = a;
	savoffset = offset;
	if (ip > (filep)contab[j].mx) {
		offset = ip - 1;
		wbf(0);
	}
	ip = savip;
	offset = savoffset;
}


/*
 * Tables for names with more than two characters. Any number in
 * contab.rq or numtab.rq that is greater or equal to MAXRQ2 refers
 * to a long name.
 */
#define	MAXRQ2	0200000

static char	**had;
static int	hadn;
static int	alcd;

void
casepm(void)
{
	register int i, k;
	int	xx, cnt, tcnt, kk, tot;
	filep j;

	kk = cnt = tcnt = 0;
	tot = !skip();
	for (i = 0; i < NM; i++) {
		if ((xx = contab[i].rq) == 0 || contab[i].mx == 0)
			continue;
		tcnt++;
		j = (filep) contab[i].mx;
		k = 1;
		while ((j = blist[blisti(j)]) != (unsigned) ~0) {
			k++; 
		}
		cnt++;
		kk += k;
		if (!tot)
			fdprintf(stderr, "%s %d\n", macname(xx), k);
	}
	fdprintf(stderr, "pm: total %d, macros %d, space %d\n", tcnt, cnt, kk);
}

void
stackdump (void)	/* dumps stack of macros in process */
{
	struct s *p;

	if (frame != stk) {
		for (p = frame; p != stk; p = p->pframe)
			fdprintf(stderr, "%s ", macname(p->mname));
		fdprintf(stderr, "\n");
	}
}

static char	laststr[NC+1];

char *
macname(int rq)
{
	static char	buf[3];
	if (rq < 0) {
		return laststr;
	} else if (rq < MAXRQ2) {
		buf[0] = rq&0177;
		buf[1] = (rq>>BYTE)&0177;
		buf[2] = 0;
		return buf;
	} else if (rq - MAXRQ2 < hadn)
		return had[rq - MAXRQ2];
	else
		return "???";
}

/*
 * To handle requests with more than two characters, an additional
 * table is maintained. On places where more than two characters are
 * allowed, the characters collected are passed in "sofar", and "create"
 * specifies whether the request is a new one. The routine returns an
 * integer which is above the regular PAIR() values.
 */
int
maybemore(int sofar, int create)
{
	char	c, buf[NC+1], pb[] = { '\n', 0 };
	int	i = 2, n, r = raw;

	if (xflag < 2)
		return sofar;
	raw = 1;
	buf[0] = sofar&BYTEMASK;
	buf[1] = (sofar>>BYTE)&BYTEMASK;
	do {
		c = getch0();
		if (i+1 >= sizeof buf) {
			buf[i] = 0;
			goto retn;
		}
		buf[i++] = c;
	} while (c && c != ' ' && c != '\t' && c != '\n');
	buf[i-1] = 0;
	buf[i] = 0;
	if (i == 3)
		goto retn;
	for (n = 0; n < hadn; n++)
		if (strcmp(had[n], buf) == 0)
			break;
	if (n == hadn) {
		if (create == 0) {
		retn:	buf[i-1] = c;
			cpushback(&buf[2]);
			raw = r;
			return sofar;
		}
		if (n >= alcd)
			had = realloc(had, (alcd += 20) * sizeof *had);
		had[n] = malloc(strlen(buf) + 1);
		strcpy(had[n], buf);
		hadn = n+1;
	}
	pb[0] = c;
	cpushback(pb);
	raw = r;
	return MAXRQ2 + n;
}

static int
getls(int termc)
{
	char	c;
	int	i = 0, j = -1, n = -1;

	do {
		c = getach();
		if (i >= sizeof laststr)
			return -1;
		laststr[i++] = c;
	} while (c && c != termc);
	laststr[--i] = 0;
	if (i == 0 || c != termc)
		j = 0;
	else if (i <= 2) {
		j = PAIR(laststr[0], laststr[1]);
	} else {
		for (n = 0; n < hadn; n++)
			if (strcmp(had[n], laststr) == 0)
				break;
		if (n == hadn)
			n = -1;
	}
	return n >= 0 ? MAXRQ2 + n : j;
}

static void
addcon(int t, char *rs, void(*f)(int))
{
	int	n = hadn;

	if (hadn++ >= alcd)
		had = realloc(had, (alcd += 20) * sizeof *had);
	had[n] = rs;
	contab[t].rq = MAXRQ2 + n;
	contab[t].f = f;
}
