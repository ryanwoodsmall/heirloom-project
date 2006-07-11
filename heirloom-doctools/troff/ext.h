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
 * Copyright 2002 Sun Microsystems, Inc.  All rights reserved.
 * Use is subject to license terms.
 */

/*	Copyright (c) 1984, 1986, 1987, 1988, 1989 AT&T	*/
/*	  All Rights Reserved  	*/


/*	from OpenSolaris "ext.h	1.10	05/06/08 SMI"	*/

/*
 * Portions Copyright (c) 2005 Gunnar Ritter, Freiburg i. Br., Germany
 *
 * Sccsid @(#)ext.h	1.53 (gritter) 7/11/06
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

#include <sys/types.h>

extern	char	**argp;
extern	char	*chname;
extern	char	*eibuf;
extern	char	*ibufp;
extern	char	*obufp;
extern	char	*xbufp;
extern	char	*xeibuf;
extern	char	*cfname[NSO+1];
extern	char	devname[];
extern	char	ibuf[IBUFSZ];
extern	char	**mfiles;
extern	char	*nextf;
extern	char	obuf[],	*obufp;
extern	char	*termtab,	*fontfile;
extern	char	xbuf[IBUFSZ];
extern	filep	apptr;
extern	filep	ip;
extern	filep	nextb;
extern	filep	offset;
extern	filep	roff;
extern	filep	woff;
extern	short	*chtab;
extern	int	*pnp;
extern	int	*pstab;
extern	int	app;
extern	int	ascii;
extern	int	bd;
extern	int	*bdtab;
extern	int	blmac;
extern	int	ccs;
extern	int	copyf;
extern	int	cs;
extern	int	defaultpl;
extern	int	dfact;
extern	int	dfactd;
extern	int	diflg;
extern	int	dilev;
extern	int	donef;
extern	int	dotT;
extern	int	dpn;
extern	int	ds;
extern	int	ejf;
extern	int	em;
extern	int	eqflg;
extern	int	error;
extern	int	esc;
extern	int	eschar;
extern	int	ev;
extern	int	fc;
extern	int	flss;
extern	int	*fontlab;
extern	int	gflag;
extern	int	hflg;
extern	int	ifi;
extern	int	ifile;
extern	int	ifl[NSO];
extern	int	iflg;
extern	int	init;
extern	int	lastkern;
extern	int	lasttrack;
extern	int	lead;
extern	int	lg;
extern	int	lgf;
extern	int	macerr;
extern	int	mb_cur_max;
extern	int	mflg;
extern	int	mfont;
extern	int	minflg;
extern	int	minspc;
extern	int	mlist[NTRAP];
extern	int	mpts;
extern	int	ndone;
extern	int	newmn;
extern	int	nflush;
extern	int	nfo;
extern	int	nfonts;
extern	int	nform;
extern	int	nhyp;
extern	int	nlflg;
extern	int	nlist[NTRAP];
extern	int	nmfi;
extern	int	no_out;
extern	int	nofeed;
extern	int	nonumb;
extern	int	noscale;
extern	int	npn;
extern	int	npnflg;
extern	int	nx;
extern	int	oldbits;
extern	int	oldmn;
extern	int	over;
extern	int	padc;
extern	int	pfont;
extern	int	pfrom;
extern	pid_t	pipeflg;
extern	int	pl;
extern	int	pnlist[];
extern	int	po1;
extern	int	po;
extern	int	ppts;
extern	int	print;
extern	int	ptid;
extern	int	pto;
extern	int	quiet;
extern	int	ralss;
extern	int	rargc;
extern	int	raw;
extern	int	rawwidth;
extern	long	realpage;
extern	int	res;
extern	int	setwdf;
extern	int	sfont;
extern	int	smnt;
extern	int	stdi;
extern	int	stop;
extern	int	sv;
extern	int	tabch,	ldrch;
extern	int	tailflg;
extern	int	tflg;
extern	int	totout;
extern	int	trap;
extern	int	*trtab;
extern	int	tryglf;
extern	int	tty;
extern	int	ttyod;
extern	int	ulfont;
extern	int	vflag;
extern	int	vpt;
extern	int	wbfi;
extern	int	widthp;
extern	int	xflag;
extern	int	xfont;
extern	int	xpts;
extern	int	no_out;
extern	int	ejl;
extern	struct	s	*frame,	*stk,	*nxf;
extern	tchar	**hyp;
extern	tchar	*olinep;
extern	tchar	*pbbuf;
extern	int	pbsize;
extern	int	pbp;
extern	int	lastpbp;
extern	tchar	ch;
extern	tchar	nrbits;
extern	tchar	oline[];
extern	struct widcache {	/* width cache, indexed by character */
	int	fontpts;
	int	width;
} *widcache;
extern	char *gchtab;
extern	struct	d	*d;
extern	struct	d	*dip;

#ifdef	EUC
#include <stddef.h>
extern	int	multi_locale;
extern  int	csi_width[];
extern	char	mbbuf1[];
extern	char	*mbbuf1p;
extern	wchar_t	twc;
extern	int	(*wdbdg)(wchar_t, wchar_t, int);
extern	wchar_t	*(*wddlm)(wchar_t, wchar_t, int);
#endif	/* EUC */
extern	int	**lhangtab;
extern	int	**rhangtab;
extern	int	**kernafter;
extern	int	**kernbefore;
extern	int	**ftrtab;
extern	char	*lgmark;
extern	struct lgtab	**lgtab;
extern	int	***lgrevtab;
extern	int	spreadwarn;
extern	int	spreadlimit;
extern	int	lastrq;

/* n1.c */
extern int	tryfile(char *, char *, int);
extern void	catch(int);
extern void	kcatch(int);
extern void	init0(void);
extern void	init1(char);
extern void	init2(void);
extern void	cvtime(void);
extern int	ctoi(register char *);
extern void	mesg(int);
extern void	errprint(const char *, ...);
#define	fdprintf	xxfdprintf
extern void	fdprintf(int, char *, ...);
extern char	*roff_sprintf(char *, char *, ...);
extern int	control(register int, register int);
extern int	getrq2(void);
extern int	getrq(int);
extern tchar	getch(void);
extern void	setxon(void);
extern tchar	getch0(void);
extern void	pushback(register tchar *);
extern void	cpushback(register char *);
extern tchar	*growpbbuf(void);
extern int	nextfile(void);
extern int	popf(void);
extern void	flushi(void);
extern int	getach(void);
extern void	casenx(void);
extern int	getname(void);
extern void	caseso(void);
extern void	casepso(void);
extern void	caself(void);
extern void	casecf(void);
extern void	casesy(void);
extern void	getpn(register char *);
extern void	setrpt(void);
extern void	casedb(void);
extern void	casexflag(void);
extern void	caserecursionlimit(void);
/* n2.c */
extern int	pchar(register tchar);
extern void	pchar1(register tchar);
extern void	outascii(tchar);
extern void	oputs(register char *);
extern void	flusho(void);
extern void	caseoutput(void);
extern void	done(int);
extern void	done1(int);
extern void	done2(int);
extern void	done3(int);
extern void	edone(int);
extern void	casepi(void);
/* n3.c */
extern void	*growcontab(void);
extern void	*growblist(void);
extern void	caseig(void);
extern void	casern(void);
extern void	maddhash(register struct contab *);
extern void	munhash(register struct contab *);
extern void	mrehash(void);
extern void	caserm(void);
extern void	caseas(void);
extern void	caseds(void);
extern void	caseam(void);
extern void	casede(void);
extern int	findmn(register int);
extern void	clrmn(register int);
extern filep	finds(register int);
extern int	skip(int);
extern int	copyb(void);
extern void	copys(void);
extern filep	alloc(void);
extern void	ffree(filep);
extern void	wbt(tchar);
extern void	wbf(register tchar);
extern void	wbfl(void);
extern tchar	rbf(void);
extern tchar	rbf0(register filep);
extern filep	incoff(register filep);
extern tchar	popi(void);
extern int	pushi(filep, int);
extern char	*setbrk(int);
extern int	getsn(void);
extern int	setstr(void);
extern void	collect(void);
extern void	seta(void);
extern void	caseda(void);
extern void	casedi(void);
extern void	casedt(void);
extern void	casetl(void);
extern void	casepc(void);
extern void	casechop(void);
extern void	casepm(void);
extern void	stackdump(void);
extern char	*macname(int);
extern int	maybemore(int, int);
extern tchar	setuc(void);
extern int	makerq(const char *);
/* n4.c */
extern void	*grownumtab(void);
extern void	setn(void);
extern int	wrc(tchar);
extern void	setn1(int, int, tchar);
extern void	nrehash(void);
extern void	nunhash(register struct numtab *);
extern int	findr(register int);
extern int	usedr(register int);
extern int	fnumb(register int, register int (*)(tchar));
extern int	decml(register int, register int (*)(tchar));
extern int	roman(int, int (*)(tchar));
extern int	roman0(int, int (*)(tchar), char *, char *);
extern int	abc(int, int (*)(tchar));
extern int	abc0(int, int (*)(tchar));
#define	atoi()	xxatoi()
extern int	atoi();
extern long	long atoi0(void);
extern long	long ckph(void);
extern long	long atoi1(register tchar);
extern void	setnr(const char *, int, int);
extern void	caserr(void);
extern void	casenr(void);
extern void	setr(void);
extern void	caseaf(void);
extern void	setaf(void);
extern int	vnumb(int *);
extern int	hnumb(int *);
extern int	inumb(int *);
extern int	quant(int, int);
/* n5.c */
extern void	save_tty(void);
extern void	casead(void);
extern void	casena(void);
extern void	casefi(void);
extern void	casenf(void);
extern void	casers(void);
extern void	casens(void);
extern void	casespreadwarn(void);
extern int	chget(int);
extern void	casecc(void);
extern void	casec2(void);
extern void	casehc(void);
extern void	casetc(void);
extern void	caselc(void);
extern void	casehy(void);
extern void	casenh(void);
extern void	casehlm(void);
extern int	max(int, int);
extern int	min(int, int);
extern void	casece(void);
extern void	casein(void);
extern void	casell(void);
extern void	caselt(void);
extern void	caseti(void);
extern void	casels(void);
extern void	casepo(void);
extern void	casepl(void);
extern void	casewh(void);
extern void	casech(void);
extern void	casevpt(void);
extern int	findn(int);
extern void	casepn(void);
extern void	casebp(void);
extern void	casetm(int);
extern void	casetmc(void);
extern void	caseopen(void);
extern void	caseopena(void);
extern void	casewrite(void);
extern void	casewritec(void);
extern void	caseclose(void);
extern void	casesp(int);
extern void	casebrp(void);
extern void	caseblm(void);
extern void	casert(void);
extern void	caseem(void);
extern void	casefl(void);
extern void	caseev(void);
extern void	caseevc(void);
extern void	caseel(void);
extern void	caseie(void);
extern void	caseif(int);
extern void	casenop(void);
extern void	casereturn(void);
extern void	eatblk(int);
extern int	cmpstr(tchar);
extern void	caserd(void);
extern int	rdtty(void);
extern void	caseec(void);
extern void	caseeo(void);
extern void	caseta(void);
extern void	casene(void);
extern void	casetr(void);
extern void	casecu(void);
extern void	caseul(void);
extern void	caseuf(void);
extern void	caseit(int);
extern void	caseitc(void);
extern void	casemc(void);
extern void	casemk(void);
extern void	casesv(void);
extern void	caseos(void);
extern void	casenm(void);
extern void	getnm(int *, int);
extern void	casenn(void);
extern void	caseab(void);
extern void	restore_tty(void);
extern void	set_tty(void);
extern void	echo_off(void);
extern void	echo_on(void);
/* n7.c */
extern int	collectmb(tchar);
extern void	tbreak(void);
extern void	donum(void);
extern void	text(void);
extern void	nofill(void);
extern void	callsp(void);
extern void	ckul(void);
extern void	storeline(register tchar, int);
extern void	newline(int);
extern int	findn1(int);
extern void	chkpn(void);
extern int	findt(int);
extern int	findt1(void);
extern void	eject(struct s *);
extern int	movword(void);
extern void	horiz(int);
extern void	setnel(void);
extern int	getword(int);
extern void	storeword(register tchar, register int);
/* n8.c */
extern void	hyphen(tchar *);
extern int	punct(tchar);
extern int	alph(tchar);
extern void	caseht(void);
extern void	casehw(void);
extern int	exword(void);
extern int	suffix(void);
extern int	maplow(register int, int);
extern int	vowel(int);
extern tchar	*chkvow(tchar *);
extern void	digram(void);
extern int	dilook(int, int, const char [26][13]);
extern void	casehylang(void);
/* n9.c */
extern tchar	setz(void);
extern void	setline(void);
extern int	eat(register int);
extern void	setov(void);
extern void	setbra(void);
extern void	setvline(void);
extern void	setdraw(void);
extern void	casefc(void);
extern tchar	setfield(int);
extern tchar	mkxfunc(int, int);
extern void	localize(void);
extern void	caselc_ctype(void);
extern void	casepsbb(void);
extern void	casewarn(void);
extern void	nosuch(int);
extern void	illseq(int, const char *, int);
extern void	missing(void);
extern void	nodelim(int);
extern void	morechars(int);
