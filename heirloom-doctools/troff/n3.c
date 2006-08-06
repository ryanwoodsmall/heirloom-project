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
 * Sccsid @(#)n3.c	1.125 (gritter) 8/6/06
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
#include "pt.h"
#include "ext.h"
#include <unistd.h>

#define	MHASH(x)	((x>>6)^x)&0177
struct	contab *mhash[128];	/* 128 == the 0177 on line above */
#define	blisti(i)	(((i)-ENV_BLK*BLK) / BLK)
filep	*blist;
int	nblist;
int	pagech = '%';
int	strflg;

tchar *wbuf;
tchar *corebuf;

static void	_collect(int);
static int	_findmn(int, int);
static void	caseshift(void);
static void	casesubstring(void);
static void	caselength(void);
static void	caseindex(void);
static void	caseasciify(void);
static void	caseunformat(int);
static int	getls(int, int *);
static void	addcon(int, char *, void(*)(int));

static const struct {
	char	*n;
	void	(*f)(int);
} longrequests[] = {
	{ "aln",		(void(*)(int))casealn },
	{ "als",		(void(*)(int))caseals },
	{ "asciify",		(void(*)(int))caseasciify },
	{ "bleedat",		(void(*)(int))casebleedat },
	{ "blm",		(void(*)(int))caseblm },
	{ "break",		(void(*)(int))casebreak},
	{ "breakchar",		(void(*)(int))casebreakchar },
	{ "brp",		(void(*)(int))casebrp },
	{ "char",		(void(*)(int))casechar },
	{ "chop",		(void(*)(int))casechop },
	{ "close",		(void(*)(int))caseclose },
	{ "continue",		(void(*)(int))casecontinue },
	{ "cropat",		(void(*)(int))casecropat },
	{ "ecs",		(void(*)(int))caseecs },
	{ "ecr",		(void(*)(int))caseecr },
	{ "evc",		(void(*)(int))caseevc },
	{ "fallback",		(void(*)(int))casefallback },
	{ "fchar",		(void(*)(int))casefchar },
	{ "fdeferlig",		(void(*)(int))casefdeferlig },
	{ "feature",		(void(*)(int))casefeature },
	{ "fkern",		(void(*)(int))casefkern },
	{ "flig",		(void(*)(int))caseflig },
	{ "fps",		(void(*)(int))casefps },
	{ "fspacewidth",	(void(*)(int))casefspacewidth },
	{ "ftr",		(void(*)(int))caseftr },
	{ "fzoom",		(void(*)(int))casefzoom },
	{ "hcode",		(void(*)(int))casehcode },
	{ "hidechar",		(void(*)(int))casehidechar },
	{ "hlm",		(void(*)(int))casehlm },
	{ "hylang",		(void(*)(int))casehylang },
	{ "index",		(void(*)(int))caseindex },
	{ "itc",		(void(*)(int))caseitc },
	{ "kern",		(void(*)(int))casekern },
	{ "kernafter",		(void(*)(int))casekernafter },
	{ "kernbefore",		(void(*)(int))casekernbefore },
	{ "kernpair",		(void(*)(int))casekernpair },
	{ "lc_ctype",		(void(*)(int))caselc_ctype },
	{ "length",		(void(*)(int))caselength },
	{ "letadj",		(void(*)(int))caseletadj },
	{ "lhang",		(void(*)(int))caselhang },
	{ "mediasize",		(void(*)(int))casemediasize },
	{ "minss",		(void(*)(int))caseminss },
	{ "nhychar",		(void(*)(int))casenhychar },
	{ "nop",		(void(*)(int))casenop },
	{ "nrf",		(void(*)(int))casenrf },
	{ "open",		(void(*)(int))caseopen },
	{ "opena",		(void(*)(int))caseopena },
	{ "output",		(void(*)(int))caseoutput },
	{ "papersize",		(void(*)(int))casepapersize },
	{ "psbb",		(void(*)(int))casepsbb },
	{ "pso",		(void(*)(int))casepso },
	{ "rchar",		(void(*)(int))caserchar },
	{ "recursionlimit",	(void(*)(int))caserecursionlimit },
	{ "return",		(void(*)(int))casereturn },
	{ "rhang",		(void(*)(int))caserhang },
	{ "rnn",		(void(*)(int))casernn },
	{ "sentchar",		(void(*)(int))casesentchar },
	{ "shc",		(void(*)(int))caseshc },
	{ "shift",		(void(*)(int))caseshift },
	{ "spreadwarn",		(void(*)(int))casespreadwarn },
	{ "substring",		(void(*)(int))casesubstring },
	{ "tmc",		(void(*)(int))casetmc },
	{ "track",		(void(*)(int))casetrack },
	{ "transchar",		(void(*)(int))casetranschar },
	{ "trimat",		(void(*)(int))casetrimat },
	{ "unformat",		(void(*)(int))caseunformat },
	{ "vpt",		(void(*)(int))casevpt },
	{ "warn",		(void(*)(int))casewarn },
	{ "while",		(void(*)(int))casewhile },
	{ "write",		(void(*)(int))casewrite },
	{ "writec",		(void(*)(int))casewritec },
	{ "xflag",		(void(*)(int))casexflag },
	{ NULL,			NULL }
};

void *
growcontab(void)
{
	int	i, j, inc = 256;
	struct contab	*onc;

	onc = contab;
	if ((contab = realloc(contab, (NM+inc) * sizeof *contab)) == NULL)
		return NULL;
	memset(&contab[NM], 0, inc * sizeof *contab);
	if (NM == 0) {
		for (i = 0; initcontab[i].f; i++)
			contab[i] = initcontab[i];
		for (j = 0; longrequests[j].f; j++)
			addcon(i++, longrequests[j].n, longrequests[j].f);
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
	tchar	*ocb;

	if (nblist+inc > XBLIST)
		return NULL;
	if ((blist = realloc(blist, (nblist+inc) * sizeof *blist)) == NULL)
		return NULL;
	memset(&blist[nblist], 0, inc * sizeof *blist);
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
	skip(1);
	if ((i = getrq(0)) == 0)
		return;
	if ((oldmn = _findmn(i, 0)) < 0) {
		nosuch(i);
		return;
	}
	skip(1);
	j = getrq(1);
	clrmn(_findmn(j, 0));
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
	int i, j, k, cnt = 0;

	lgf++;
	while (!skip(!cnt++) && (j = getrq(0)) != 0) {
		k = _findmn(j, 0);
		if (k >= 0 && contab[k].als) {
			i = _findmn(j, 1);
			if (--contab[i].nlink <= 0)
				clrmn(i);
		}
		if (k >= 0 && contab[k].nlink > 0)
			contab[k].nlink--;
		clrmn(k);
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
	int	k, nlink;

	if (dip != d)
		wbfl();
	req = '.';
	lgf++;
	skip(1);
	if ((i = getrq(1)) == 0)
		goto de1;
	if ((offset = finds(i)) == 0)
		goto de1;
	if (ds)
		copys();
	else 
		req = copyb();
	wbfl();
	if (oldmn >= 0 && (nlink = contab[oldmn].nlink) > 0)
		k = contab[oldmn].rq;
	else {
		k = i;
		nlink = 0;
	}
	clrmn(oldmn);
	if (newmn) {
		if (contab[newmn].rq)
			munhash(&contab[newmn]);
		contab[newmn].rq = k;
		contab[newmn].nlink = nlink;
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


static int 
_findmn(register int i, int als)
{
	register struct contab *p;

	for (p = mhash[MHASH(i)]; p; p = p->link)
		if (i == p->rq) {
			if (als && p->als)
				return(_findmn(p->als, als));
			return(p - contab);
		}
	return(-1);
}


int
findmn(int i)
{
	return _findmn(i, 1);
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
		contab[i].als = 0;
		contab[i].nlink = 0;
	}
}


static filep 
_finds(register int mn, int als)
{
	register tchar i;
	register filep savip;

	oldmn = _findmn(mn, als);
	newmn = 0;
	apptr = (filep)0;
	if (app && oldmn >= 0 && contab[oldmn].mx) {
		savip = ip;
		ip = (filep)contab[oldmn].mx;
		oldmn = -1;
		while ((i = rbf()) != 0) {
			if (!diflg && istail(i))
				corebuf[ip - 1] &= ~(tchar)TAILBIT;
		}
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
		nextb = 0;
		if (i == NM && growcontab() == NULL ||
				als && (nextb = alloc()) == 0) {
			app = 0;
			if (macerr++ > 1)
				done2(02);
			errprint("Too many (%d) string/macro names", NM);
			edone(04);
			return(als ? offset = 0 : 0);
		}
		contab[i].mx = (unsigned) nextb;
		newmn = i;
		if (!diflg) {
			if (oldmn == -1)
				contab[i].rq = -1;
		} else {
			contab[i].rq = mn;
			maddhash(&contab[i]);
		}
	}
	app = 0;
	return(als ? offset = nextb : 1);
}


filep
finds(int mn)
{
	return _finds(mn, 1);
}


int 
skip (int required)		/*skip over blanks; return nlflg*/
{
	register tchar i;

	while (cbits(i = getch()) == ' ')
		;
	ch = i;
	if (nlflg && required)
		missing();
	return(nlflg);
}


int 
copyb(void)
{
	register int i, j, state;
	register tchar ii;
	int	req;
	filep savoff = 0, tailoff = 0;
	tchar	tailc = 0;
	char	*cp, *mn;

	if (skip(0) || !(j = getrq(1)))
		j = '.';
	req = j;
	cp = macname(req);
	mn = malloc(strlen(cp) + 1);
	strcpy(mn, cp);
	copyf++;
	flushi();
	nlflg = 0;
	state = 1;

/* state 0	eat up
 * state 1	look for .
 * state 2	look for chars of end macro
 */

	while (1) {
		i = cbits(ii = getch());
		if (state == 2 && mn[j] == 0) {
			ch = ii;
			i = getach();
			ch = ii;
			if (!i)
				break;
			state = 0;
			goto c0;
		}
		if (i == '\n') {
			state = 1;
			nlflg = 0;
			tailoff = offset;
			tailc = ii;
			ii &= ~(tchar)TAILBIT;
			goto c0;
		}
		if (state == 1 && i == '.') {
			state++;
			savoff = offset;
			j = 0;
			goto c0;
		}
		if ((state == 2) && (i == mn[j])) {
			j++;
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
		if (tailoff) {
			offset = tailoff;
			wbt(tailc | TAILBIT);
		}
	}
	copyf--;
	free(mn);
	return(req);
}


void
copys(void)
{
	register tchar i;

	copyf++;
	if (skip(0))
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
		fdprintf(stderr, "newmn %d; nextb was %lx, will be %lx\n",
			newmn, (long)nextb, (long)j);
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
		wbuf = &corebuf[woff];
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
	i = corebuf[ip];
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
	return(corebuf[p]);
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
	tchar	c, d;

	if (frame == stk)
		return(0);
	if (strflg)
		strflg--;
	p = frame;
	if (p->nargs > 0) {
		free(p->argt);
		free(p->argsp);
	}
	frame = p->pframe;
	ip = p->pip;
	pendt = p->ppendt;
	lastpbp = p->lastpbp;
	c = p->pch;
	if (p->loopf & LOOP_NEXT) {
		d = ch;
		ch = c;
		pushi(p->newip, p->mname);
		c = 0;
		ch = d;
	} else
		if (p->loopf & LOOP_FREE)
			ffree(p->newip);
	free(p);
	return(c);
}


int 
pushi(filep newip, int mname)
{
	register struct s *p;

	p = nxf;
	p->pframe = frame;
	p->pip = ip;
	p->ppendt = pendt;
	p->pch = ch;
	p->lastpbp = lastpbp;
	p->mname = mname;
	if (mname != LOOP) {
		p->frame_cnt = frame->frame_cnt + 1;
		p->tail_cnt = frame->tail_cnt + 1;
	} else {
		p->frame_cnt = frame->frame_cnt;
		p->tail_cnt = frame->tail_cnt;
		p->loopf = LOOP_EVAL;
	}
	p->newip = newip;
	lastpbp = pbp;
	pendt = ch = 0;
	frame = nxf;
	nxf = calloc(1, sizeof *nxf);
	return(ip = newip);
}


char *
setbrk(int x)
{
	return(calloc(x, 1));
}


static int
_getsn(int *strp)
{
	register int i;

	if ((i = getach()) == 0)
		return(0);
	if (i == '(')
		return(getrq2());
	else if (i == '[' && xflag != 0)
		return(getls(']', strp));
	else 
		return(i);
}

int
getsn(void)
{
	return _getsn(0);
}


int 
setstr(void)
{
	register int i, j;
	int	space = 0;

	lgf++;
	if ((i = _getsn(&space)) == 0 || (j = findmn(i)) == -1 ||
			!contab[j].mx) {
		if (space)
			nodelim(']');
		nosuch(i);
		lgf--;
		return(0);
	} else {
		if (space)
			_collect(']');
		else
			nxf->nargs = 0;
		strflg++;
		lgf--;
		return pushi((filep)contab[j].mx, i);
	}
}

void
collect(void)
{
	return _collect(0);
}

static void
_collect(int termc)
{
	register tchar i = 0;
	int	at = 0, asp = 0;
	int	nt = 0, nsp = 0;
	int	quote;
	struct s *savnxf;

	copyf++;
	nxf->nargs = 0;
	nxf->argt = NULL;
	nxf->argsp = NULL;
	savnxf = nxf;
	nxf = calloc(1, sizeof *nxf);
	if (skip(0))
		goto rtn;

	strflg = 0;
	while (!skip(0)) {
		if (nt >= at)
			savnxf->argt = realloc(savnxf->argt,
				(at += 10) * sizeof *savnxf->argt);
		savnxf->argt[nt++] = nsp;
		quote = 0;
		if (cbits(i = getch()) == '"')
			quote++;
		else 
			ch = i;
		while (1) {
			i = getch();
			if (termc && i == termc)
				goto rtn;
			if (nlflg || (!quote && cbits(i) == ' '))
				break;
			if (   quote
			    && (cbits(i) == '"')
			    && (cbits(i = getch()) != '"')) {
				ch = i;
				break;
			}
			if (nsp >= asp)
				savnxf->argsp = realloc(savnxf->argsp,
					(asp += 200) * sizeof *savnxf->argsp);
			savnxf->argsp[nsp++] = i;
		}
		if (nsp >= asp)
			savnxf->argsp = realloc(savnxf->argsp,
				++asp * sizeof *savnxf->argsp);
		savnxf->argsp[nsp++] = 0;
	}
rtn:
	if (termc && i != termc)
		nodelim(termc);
	free(nxf);
	nxf = savnxf;
	nxf->nargs = nt;
	copyf--;
}


void
seta(void)
{
	register int c, i;
	char q[] = { 0, 0 };
	struct s	*s;

	for (s = frame; s->loopf && s != stk; s = s->pframe);
	switch (c = cbits(getch())) {
	case '@':
		q[0] = '"';
		/*FALLTHRU*/
	case '*':
		if (xflag == 0)
			goto dfl;
		for (i = s->nargs; i >= 1; i--) {
			if (q[0])
				cpushback(q);
			pushback(&s->argsp[s->argt[i - 1]]);
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
	assign:	if (i > 0 && i <= s->nargs)
			pushback(&s->argsp[s->argt[i - 1]]);
		else if (i == 0)
			cpushback(macname(s->mname));
	}
}

static void
caseshift(void)
{
	int	i, j;
	struct s	*s;

	for (s = frame; s->loopf && s != stk; s = s->pframe);
	if (skip(0))
		i = 1;
	else {
		noscale++;
		i = atoi();
		noscale--;
	}
	if (nonumb)
		return;
	if (i > 0 && i <= s->nargs) {
		s->nargs -= i;
		for (j = 1; j <= s->nargs; j++)
			s->argt[j - 1] = s->argt[j + i - 1];
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
	int	nlink;

	lgf++;
	if (skip(0) || (i = getrq(1)) == 0) {
		if (dip != d)
			wbt((tchar)0);
		if (dilev > 0) {
			numtab[DN].val = dip->dnl;
			numtab[DL].val = dip->maxl;
			dip = &d[--dilev];
			offset = dip->op;
		} else if (warn & WARN_DI)
			errprint(".di outside active diversion");
		goto rtn;
	}
	if (++dilev == NDI) {
		struct d	*nd;
		const int	inc = 5;
		if ((nd = realloc(d, (NDI+inc) * sizeof *d)) == NULL) {
			--dilev;
			errprint("Diversions nested too deep");
			edone(02);
		}
		d = nd;
		memset(&d[NDI], 0, inc * sizeof *d);
		NDI += inc;
	}
	if (dip != d)
		wbt((tchar)0);
	diflg++;
	dip = &d[dilev];
	dip->op = finds(i);
	dip->curd = i;
	if (newmn && oldmn >= 0 && (nlink = contab[oldmn].nlink) > 0) {
		munhash(&contab[newmn]);
		j = contab[oldmn].rq;
	} else {
		j = i;
		nlink = 0;
	}
	clrmn(oldmn);
	if (newmn) {
		contab[newmn].rq = j;
		contab[newmn].nlink = nlink;
		if (i != j)
			maddhash(&contab[newmn]);
	}
	k = (int *) & dip->dnl;
	dip->flss = 0;
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
	skip(0);
	dip->ditrap = vnumb((int *)0);
	if (nonumb)
		return;
	skip(0);
	dip->dimac = getrq(1);
}


void
caseals(void)
{
	int	i, j, k, t;

	if (skip(1))
		return;
	i = getrq(1);
	if (skip(1))
		return;
	j = getrq(1);
	if ((k = findmn(j)) < 0) {
		nosuch(j);
		return;
	}
	if (contab[k].nlink == 0) {
		munhash(&contab[k]);
		t = makerq(NULL);
		contab[k].rq = t;
		maddhash(&contab[k]);
		if (_finds(j, 0) != 0 && newmn) {
			contab[newmn].als = t;
			contab[newmn].rq = j;
			maddhash(&contab[newmn]);
			contab[k].nlink = 1;
		}
	} else
		t = j;
	if (_finds(i, 0) != 0) {
		clrmn(oldmn);
		if (newmn) {
			if (contab[newmn].rq)
				munhash(&contab[newmn]);
			contab[newmn].als = t;
			contab[newmn].rq = i;
			maddhash(&contab[newmn]);
			contab[k].nlink++;
		}
	}
}


void
casetl(void)
{
	register int j;
	int w[3];
	tchar *buf = NULL;
	int	bufsz = 0;
	register tchar *tp;
	tchar i, delim, nexti;
	int oev;

	dip->nls = 0;
	skip(1);
	if (ismot(delim = getch())) {
		ch = delim;
		delim = '\'';
	} else 
		delim = cbits(delim);
	bufsz = LNSIZE;
	buf = malloc(bufsz * sizeof *buf);
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
				      sfmask(i));
				nexti = getch();
				continue;
			}
			numtab[HP].val += width(i);
			oev = ev;
			nexti = getch();
			if (ev == oev)
				numtab[HP].val += kernadjust(i, nexti);
			if (tp >= &buf[bufsz-10]) {
				tchar	*k;
				bufsz += 100;
				k = realloc(buf, bufsz * sizeof *buf);
				tp += k - buf;
				buf = k;
			}
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
	free(buf);
}

void
casepc(void)
{
	pagech = chget(IMP);
}

void
casechop(void)
{
	int	i, j;
	filep	savip;

	if (dip != d)
		wbfl();
	lgf++;
	skip(1);
	if ((i = getrq(0)) == 0)
		return;
	if ((j = findmn(i)) < 0 || !contab[j].mx) {
		nosuch(i);
		return;
	}
	savip = ip;
	ip = (filep)contab[j].mx;
	app = 1;
	while (rbf() != 0);
	app = 0;
	if (ip > (filep)contab[j].mx) {
		offset = ip - 1;
		wbt(0);
	}
	ip = savip;
	offset = dip->op;
}

void
casesubstring(void)
{
	int	i, j, k, sz = 0, st;
	int	n1, n2 = -1, nlink;
	tchar	*tp = NULL, c;
	filep	savip;

	if (dip != d)
		wbfl();
	lgf++;
	skip(1);
	if ((i = getrq(0)) == 0)
		return;
	if ((j = findmn(i)) < 0 || !contab[j].mx) {
		nosuch(i);
		return;
	}
	if (skip(1))
		return;
	noscale++;
	n1 = atoi();
	if (skip(0) == 0)
		n2 = atoi();
	noscale--;
	savip = ip;
	ip = (filep)contab[j].mx;
	k = 0;
	app = 1;
	while ((c = rbf()) != 0) {
		if (k >= sz) {
			sz += 512;
			tp = realloc(tp, sz * sizeof *tp);
		}
		tp[k++] = c;
	}
	app = 0;
	ip = savip;
	if ((offset = finds(i)) != 0) {
		st = 0;
		if (n1 < 0)
			n1 = k + n1;
		if (n2 < 0)
			n2 = k + n2;
		if (n1 >= 0 || n2 >= 0) {
			if (n2 < n1) {
				j = n1;
				n1 = n2;
				n2 = j;
			}
			for (j = 0; j <= k; j++) {
				if (st == 0) {
					if (j >= n1)
						st = 1;
				}
				if (st == 1) {
					wbf(tp[j]);
					if (j >= n2)
						break;
				}
			}
		}
		wbt(0);
		if (oldmn >= 0 && (nlink = contab[oldmn].nlink) > 0)
			k = contab[oldmn].rq;
		else {
			k = i;
			nlink = 0;
		}
		clrmn(oldmn);
		if (newmn) {
			if (contab[newmn].rq)
				munhash(&contab[newmn]);
			contab[newmn].rq = k;
			contab[newmn].nlink = nlink;
			maddhash(&contab[newmn]);
		}
	}
	free(tp);
	offset = dip->op;
	ip = savip;
}

void
caselength(void)
{
	int	i, j;

	lgf++;
	skip(1);
	if ((i = getrq(1)) == 0)
		return;
	j = 0;
	lgf--;
	copyf++;
	if (skip(1) == 0) {
		while(cbits(getch()) != '\n')
			j++;
	}
	copyf--;
	numtab[findr(i)].val = j;
}

void
caseindex(void)
{
	int	i, j, n, N, M;
	int	*sp = NULL, as = 0, ns = 0, *np;
	tchar	c;
	filep	savip;

	lgf++;
	skip(1);
	if ((N = getrq(1)) == 0)
		return;
	skip(1);
	if ((i = getrq(1)) == 0)
		return;
	if ((M = findmn(i)) < 0 || !contab[M].mx) {
		nosuch(i);
		return;
	}
	copyf++;
	if (!skip(0)) {
		while ((c = getch()) != 0 && !ismot(c) &&
				(i = cbits(c)) != '\n') {
			if (ns >= as)
				sp = realloc(sp, (as += 10) * sizeof *sp);
			sp[ns++] = i;
		}
		np = malloc((ns + 1) * sizeof *np);
		i = 0;
		j = -1;
		for (;;) {
			np[i++] = j++;
			if (i >= ns)
				break;
			while (j >= 0 && sp[i] != sp[j])
				j = np[j];
		}
		savip = ip;
		ip = (filep)contab[M].mx;
		app = 1;
		j = 0;
		n = 0;
		while ((c = rbf()) != 0 && j < ns) {
			while (j >= 0 && cbits(c) != sp[j])
				j = np[j];
			j++;
			n++;
		}
		n = j == ns ? n - ns : -1;
		app = 0;
		ip = savip;
		free(sp);
		free(np);
	} else
		n = -1;
	copyf--;
	numtab[findr(N)].val = n;
}

static void
caseasciify(void)
{
	caseunformat(1);
}

static void
caseunformat(int flag)
{
	int	i, j, k, nlink;
	int	ns = 0, as = 0;
	tchar	*tp = NULL, c;
	filep	savip;
	int	noout = 0;

	if (dip != d)
		wbfl();
	lgf++;
	skip(1);
	if ((i = getrq(0)) == 0)
		return;
	if ((j = findmn(i)) < 0 || !contab[j].mx) {
		nosuch(i);
		return;
	}
	savip = ip;
	ip = (filep)contab[j].mx;
	ns = 0;
	app = 1;
	while ((c = rbf()) != 0) {
		if (ns >= as) {
			as += 512;
			tp = realloc(tp, as * sizeof *tp);
		}
		tp[ns++] = c;
	}
	app = 0;
	ip = savip;
	if ((offset = finds(i)) != 0) {
		for (j = 0; j < ns; j++) {
			if (!ismot(c) && cbits(c) == '\n')
				noout = 0;
			else if (j+1 < ns && cbits(tp[j+1]) == XFUNC &&
					fbits(tp[j+1]) == HYPHED)
				noout = 1;
			if (isadjspc(c = tp[j])) {
				if (cbits(c) == WORDSP)
					setcbits(c, ' ');
				c &= ~ADJBIT;
			} else if (c == WORDSP) {
				j++;
				continue;
			} else if (c == FLSS) {
				j++;
				continue;
			} else if (cbits(c) == XFUNC) {
				switch (fbits(c)) {
				case FLDMARK:
					if ((c = sbits(c)) == 0)
						continue;
					break;
				case LETSP:
				case NLETSP:
				case LETSH:
				case NLETSH:
					continue;
				}
			} else if (isadjmot(c))
				continue;
			if (flag & 1 && !ismot(c) && cbits(c) != SLANT) {
#ifndef	NROFF
				int	m = cbits(c);
				int	f = fbits(c);
				int	k;
				if (islig(c) && lgrevtab && lgrevtab[f] &&
						lgrevtab[f][m]) {
					for (k = 0; lgrevtab[f][m][k]; k++)
						if (!noout)
							wbf(lgrevtab[f][m][k]);
					continue;
				} else
#endif
					c = cbits(c);
			}
			if (!noout)
				wbf(c);
		}
		wbt(0);
		if (oldmn >= 0 && (nlink = contab[oldmn].nlink) > 0)
			k = contab[oldmn].rq;
		else {
			k = i;
			nlink = 0;
		}
		clrmn(oldmn);
		if (newmn) {
			if (contab[newmn].rq)
				munhash(&contab[newmn]);
			contab[newmn].rq = k;
			contab[newmn].nlink = nlink;
			maddhash(&contab[newmn]);
		}
	}
	free(tp);
	offset = dip->op;
}


/*
 * Tables for names with more than two characters. Any number in
 * contab.rq or numtab.rq that is greater or equal to MAXRQ2 refers
 * to a long name.
 */
#define	MAXRQ2	0200000

static struct map {
	struct map	*link;
	int	n;
} *map[128];
static char	**had;
static int	hadn;
static int	alcd;

#define	maphash(cp)	(_pjw(cp) & 0177)

static unsigned
_pjw(const char *cp)
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

static int
mapget(const char *cp)
{
	int	h = maphash(cp);
	struct map	*mp;

	for (mp = map[h]; mp; mp = mp->link)
		if (strcmp(had[mp->n], cp) == 0)
			return mp->n;
	return hadn;
}

static void
mapadd(const char *cp, int n)
{
	int	h = maphash(cp);
	struct map	*mp;

	mp = calloc(1, sizeof *mp);
	mp->n = n;
	mp->link = map[h];
	map[h] = mp;
}

void
casepm(void)
{
	register int i, k;
	int	xx, cnt, tcnt, kk, tot;
	filep j;

	kk = cnt = tcnt = 0;
	tot = !skip(0);
	for (i = 0; i < NM; i++) {
		if ((xx = contab[i].rq) == 0 || contab[i].mx == 0) {
			if (contab[i].als && (k = findmn(xx)) >= 0) {
				if (contab[k].rq == 0 || contab[k].mx == 0)
					continue;
			} else
				continue;
		}
		tcnt++;
		if (contab[i].als == 0 && (j = (filep) contab[i].mx) != 0) {
			k = 1;
			while ((j = blist[blisti(j)]) != (unsigned) ~0) {
				k++; 
			}
			cnt++;
		} else
			k = 0;
		kk += k;
		if (!tot && contab[i].nlink == 0)
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
			if (p->mname != LOOP)
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

static tchar
mgetach(void)
{
	static const char nmctab[] = {
		000,000,000,000,000,000,000,000,
		000,000,000,000,000,000,000,000,
		001,001,001,000,001,001,000,001,
		000,000,000,000,000,000,000,000,
		000
	};
	tchar	i;
	int	j;

	lgf++;
	j = cbits(i = getch());
	if (ismot(i) || j == ' ' || j == '\n' || j >= 0200 ||
			j < sizeof nmctab && nmctab[j]) {
		if (j >= 0200)
			illseq(j, NULL, -3);
		ch = i;
		j = 0;
	}
	lgf--;
	return j & 0177;
}

/*
 * To handle requests with more than two characters, an additional
 * table is maintained. On places where more than two characters are
 * allowed, the characters collected are passed in "sofar", and "flags"
 * specifies whether the request is a new one. The routine returns an
 * integer which is above the regular PAIR() values.
 */
int
maybemore(int sofar, int flags)
{
	char	c, buf[NC+1], pb[] = { '\n', 0 };
	int	i = 2, n, _raw = raw, _init = init;

	if (xflag < 2)
		return sofar;
	if (xflag == 2)
		raw = 1;
	else
		init++;
	buf[0] = sofar&BYTEMASK;
	buf[1] = (sofar>>BYTE)&BYTEMASK;
	do {
		c = xflag < 3 ? getch0() : mgetach();
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
	if ((n = mapget(buf)) >= hadn) {
		if ((flags & 1) == 0) {
			strcpy(laststr, buf);
		retn:	buf[i-1] = c;
			if (xflag < 3)
				cpushback(&buf[2]);
			raw = _raw;
			init = _init;
			if (flags & 2) {
				if (i > 3 && xflag >= 3)
					sofar = -2;
			} else if (i > 3 && xflag >= 3) {
				buf[i-1] = 0;
				if (warn & WARN_MAC)
					errprint("%s: no such request", buf);
				sofar = 0;
			} else if (warn & WARN_SPACE && i > 3 &&
					_findmn(sofar, 0) >= 0) {
				buf[i-1] = 0;
				errprint("%s: missing space", macname(sofar));
			}
			return sofar;
		}
		if (n >= alcd)
			had = realloc(had, (alcd += 20) * sizeof *had);
		had[n] = malloc(strlen(buf) + 1);
		strcpy(had[n], buf);
		hadn = n+1;
		mapadd(buf, n);
	}
	pb[0] = c;
	if (xflag < 3)
		cpushback(pb);
	raw = _raw;
	init = _init;
	return MAXRQ2 + n;
}

static int
getls(int termc, int *strp)
{
	char	c, buf[NC+1];
	int	i = 0, j = -1, n = -1;

	do {
		c = xflag < 3 ? getach() : mgetach();
		if (i >= sizeof buf)
			return -1;
		buf[i++] = c;
	} while (c && c != termc);
	if (strp)
		*strp = 0;
	if (c != termc) {
		if (strp && !nlflg)
			*strp = 1;
		else
			nodelim(termc);
	}
	buf[--i] = 0;
	if (i == 0 || c != termc && (!strp || nlflg))
		j = 0;
	else if (i <= 2) {
		j = PAIR(buf[0], buf[1]);
	} else {
		if ((n = mapget(buf)) >= hadn) {
			n = -1;
			strcpy(laststr, buf);
		}
	}
	return n >= 0 ? MAXRQ2 + n : j;
}

int
makerq(const char *name)
{
	static int	t;
	char	_name[20];
	int	n;

	if (name == NULL) {
		roff_sprintf(_name, "\13%d", ++t);
		name = _name;
	}
	if (name[0] == 0 || name[1] == 0 || name[2] == 0)
		return PAIR(name[0], name[1]);
	if ((n = mapget(name)) < hadn)
		return MAXRQ2 + n;
	if (hadn++ >= alcd)
		had = realloc(had, (alcd += 20) * sizeof *had);
	had[n] = malloc(strlen(name) + 1);
	strcpy(had[n], name);
	hadn = n + 1;
	mapadd(name, n);
	return MAXRQ2 + n;
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
	mapadd(rs, n);
}
