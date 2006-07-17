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


/*	from OpenSolaris "n1.c	1.25	05/06/08 SMI"	*/

/*
 * Portions Copyright (c) 2005 Gunnar Ritter, Freiburg i. Br., Germany
 *
 * Sccsid @(#)n1.c	1.84 (gritter) 7/18/06
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

char *xxxvers = "@(#)roff:n1.c	2.13";
/*
 * n1.c
 *
 *	consume options, initialization, main loop,
 *	input routines, escape function calling
 */

#include <stdio.h>
#include <ctype.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <setjmp.h>
#include <time.h>
#include <stdarg.h>
#include <locale.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#ifdef 	EUC
#include <stddef.h>
#include <limits.h>
#include <wchar.h>
#include <wctype.h>
#endif	/* EUC */

#include "tdef.h"
#include "ext.h"

#ifdef NROFF
#include "tw.h"
#endif
#include "pt.h"

#define	MAX_RECURSION_DEPTH	512
static int	max_recursion_depth = MAX_RECURSION_DEPTH;
static int	max_tail_depth;

jmp_buf sjbuf;
filep	ipl[NSO];
long	offl[NSO];
long	ioff;
char	*ttyp;
char	*cfname[NSO+1];		/*file name stack*/
int	cfline[NSO];		/*input line count stack*/
static int	cfpid[NSO+1];	/* .pso process IDs */
char	*progname;	/* program name (troff) */
#ifdef	EUC
char	mbbuf1[MB_LEN_MAX + 1];
char	*mbbuf1p = mbbuf1;
wchar_t	twc = 0;
#endif	/* EUC */

static void initg(void);
static void printlong(long, int);
static void printn(long, long);
static char *sprintlong(char *s, long, int);
static char *sprintn(char *s, long n, int b);
#define	vfdprintf	xxvfdprintf
static void vfdprintf(int fd, const char *fmt, va_list ap);
static void _setenv(void);

#ifdef	DEBUG
int	debug = 0;	/*debug flag*/
#endif	/* DEBUG */

static int	_xflag;

int
main(int argc, char **argv)
{
	register char	*p;
	register int j;
	register tchar i;
	int eileenct;		/*count to test for "Eileen's loop"*/
	char	**oargv;

	setlocale(LC_CTYPE, "");
	mb_cur_max = MB_CUR_MAX;
	progname = argv[0];
	nextf = calloc(1, NS = 1);
	d = calloc(NDI = 5, sizeof *d);
	growblist();
	growcontab();
	grownumtab();
	growpbbuf();
	morechars(1);
	initg();
	for (j = 0; j <= NSO; j++)
		cfpid[j] = -1;
	if (signal(SIGHUP, SIG_IGN) != SIG_IGN)
		signal(SIGHUP, catch);
	if (signal(SIGINT, catch) == SIG_IGN) {
		signal(SIGHUP, SIG_IGN);
		signal(SIGINT, SIG_IGN);
		signal(SIGQUIT, SIG_IGN);
	}
	signal(SIGPIPE, catch);
	signal(SIGTERM, kcatch);
	oargv = argv;
	cfname[0] = malloc(17);
	strcpy(cfname[0], "<standard input>");
	mrehash();
	nrehash();
	init0();
#ifdef EUC
	localize();
#endif /* EUC */
	if ((p = getenv("TYPESETTER")) != 0)
		strcpy(devname, p);
	while (--argc > 0 && (++argv)[0][0] == '-')
		switch (argv[0][1]) {

		case 'F':	/* switch font tables from default */
			if (argv[0][2] != '\0') {
				termtab = &argv[0][2];
				fontfile = &argv[0][2];
			} else {
				argv++; argc--;
				if (argv[0] != '\0') {
					termtab = argv[0];
					fontfile = argv[0];
				} else
					errprint("missing the font directory");
			}
			continue;
		case 0:
			goto start;
		case 'i':
			stdi++;
			continue;
		case 'q':
#ifdef	NROFF
			quiet++;
			save_tty();
#else
			errprint("-q option ignored in troff");
#endif	/* NROFF */
			continue;
		case 'n':
			npn = ctoi(&argv[0][2]);
			continue;
		case 'u':	/* set emboldening amount */
			bdtab[3] = ctoi(&argv[0][2]);
			if (bdtab[3] < 0 || bdtab[3] > 50)
				bdtab[3] = 0;
			continue;
		case 's':
			if (!(stop = ctoi(&argv[0][2])))
				stop++;
			continue;
		case 't':
			ptid = 1;
			continue;
		case 'r':
		case 'd':
			if (&argv[0][2] != '\0' && strlen(&argv[0][2]) >= 2 && &argv[0][3] != '\0')
			eibuf = roff_sprintf(ibuf+strlen(ibuf), ".%s %c %s%s\n",
				argv[0][1] == 'd' ? "ds" : "nr",
				argv[0][2],
				argv[0][1] == 'd' ? "\"" : "",
				&argv[0][3]); 
			else 
				errprint("wrong options");
			continue;
		case 'c':
		case 'm':
			if (mflg++ >= NMF && (mfiles = realloc(mfiles,
						++NMF * sizeof *mfiles)) == 0) {
				errprint("Too many macro packages: %s",
					 argv[0]);
				continue;
			}
		        if (argv[0][2] == '\0') {
				errprint("No library provided with -m");
				done(02);
			}
			if (getenv("TROFFMACS") != '\0') {
			     if (tryfile(getenv("TROFFMACS"), &argv[0][2], nmfi))
			       nmfi++;
			} else
			  if (tryfile(MACDIR "/", &argv[0][2], nmfi)
			  || tryfile(MACDIR "/tmac.", &argv[0][2], nmfi))
				nmfi++;
			  else {
				errprint("Cannot find library %s\n", argv[0]);
				done(02);
			  } 
			continue;
		case 'o':
			getpn(&argv[0][2]);
			continue;
		case 'T':
			strcpy(devname, &argv[0][2]);
			dotT++;
			continue;
		case 'x':
			if (argv[0][2])
				xflag = strtol(&argv[0][2], NULL, 10);
			else
				xflag = 2;
			continue;
		case 'X':
			xflag = 0;
			continue;
#ifdef NROFF
		case 'h':
			hflg++;
			continue;
		case 'z':
			no_out++;
			continue;
		case 'e':
			eqflg++;
			continue;
#endif
#ifndef NROFF
		case 'z':
			no_out++;
		case 'a':
			ascii = 1;
			nofeed++;
			continue;
		case 'f':
			nofeed++;
			continue;
#endif
		case '#':
#ifdef	DEBUG
			debug = ctoi(&argv[0][2]);
#else
			errprint("DEBUG not enabled");
#endif	/* DEBUG */
			continue;
		default:
			errprint("unknown option %s", argv[0]);
			done(02);
		}

start:
	init1(oargv[0][0]);
	argp = argv;
	rargc = argc;
	nmfi = 0;
	init2();
	_xflag = xflag;
	setjmp(sjbuf);
	eileenct = 0;		/*reset count for "Eileen's loop"*/
loop:
	xflag = _xflag;
	copyf = lgf = nb = nflush = nlflg = 0;
	if (ip && rbf0(ip) == 0 && dip == d && ejf &&
			frame->pframe->tail_cnt <= ejl) {
		nflush++;
		trap = 0;
		eject((struct s *)0);
#ifdef	DEBUG
	if (debug & DB_LOOP)
		fdprintf(stderr, "loop: NL=%d, ejf=%d, lss=%d, eileenct=%d\n",
			numtab[NL].val, ejf, lss, eileenct);
#endif	/* DEBUG */
		if (eileenct > 20) {
			errprint("job looping; check abuse of macros");
			ejf = 0;	/*try to break Eileen's loop*/
			eileenct = 0;
		} else
			eileenct++;
		goto loop;
	}
	eileenct = 0;		/*reset count for "Eileen's loop"*/
	i = getch();
	if (pendt)
		goto Lt;
	if ((j = cbits(i)) == XPAR) {
		copyf++;
		tflg++;
		while (cbits(i) != '\n')
			pchar(i = getch());
		tflg = 0;
		copyf--;
		goto loop;
	}
	if (j == cc || j == c2) {
		if (j == c2)
			nb++;
		copyf++;
		while ((j = cbits(i = getch())) == ' ' || j == '\t')
			;
		ch = i;
		copyf--;
		j = getrq(0);
		if (xflag != 0 && j == PAIR('d', 'o')) {
			xflag = 3;
			skip(1);
			j = getrq(0);
		}
		control(j, 1);
		flushi();
		goto loop;
	}
Lt:
	ch = i;
	text();
	if (nlflg)
		numtab[HP].val = 0;
	goto loop;
}


int
tryfile(register char *pat, register char *fn, int idx)
{
	mfiles[idx] = malloc(strlen(pat) + strlen(fn) + 1);
	strcpy(mfiles[idx], pat);
	strcat(mfiles[idx], fn);
	if (access(mfiles[idx], 4) == -1)
		return(0);
	else return(1);
}	

void catch(int unused)
{
	done3(01);
}


void kcatch(int unused)
{
	signal(SIGTERM, SIG_IGN);
	done3(01);
}


void
init0(void)
{
	eibuf = ibufp = ibuf;
	ibuf[0] = 0;
	numtab[NL].val = -1;
}


void
init1(char a)
{
	register int i;

	for (i = NTRTAB; --i; )
		trtab[i] = i;
	trtab[UNPAD] = ' ';
	trtab[STRETCH] = ' ';
}


void
init2(void)
{
	register int i, j;

	ttyod = 2;
	if ((ttyp=ttyname(j=0)) != 0 || (ttyp=ttyname(j=1)) != 0 || (ttyp=ttyname(j=2)) != 0)
		;
	else 
		ttyp = "notty";
	iflg = j;
	if (ascii)
		mesg(0);
	obufp = obuf;
	ptinit();
	mchbits();
	cvtime();
	numtab[PID].val = getpid();
	spreadlimit = 3*EM;
	olinep = oline;
	ioff = 0;
	numtab[HP].val = init = 0;
	numtab[NL].val = -1;
	nfo = 0;
	ifile = 0;
	copyf = raw = 0;
	eibuf = roff_sprintf(ibuf+strlen(ibuf), ".ds .T %s\n", devname);
	numtab[CD].val = -1;	/* compensation */
	cpushback(ibuf);
	ibufp = ibuf;
	nx = mflg;
	frame = stk = calloc(1, sizeof *stk);
	stk->frame_cnt = 0;
	dip = &d[0];
	nxf = calloc(1, sizeof *nxf);
	initenv = env;
	for (i = 0; i < NEV; i++) {
		extern tchar *corebuf;
		((struct env *)corebuf)[i] = env;
	}
}


void
cvtime(void)
{
	time_t	tt;
	register struct tm *tm;

	tt = time((time_t *) 0);
	tm = localtime(&tt);
	numtab[DY].val = tm->tm_mday;
	numtab[DW].val = tm->tm_wday + 1;
	numtab[YR].val = tm->tm_year;
	numtab[MO].val = tm->tm_mon + 1;
	setnr("hours", tm->tm_hour, 0);
	setnr("minutes", tm->tm_min, 0);
	setnr("seconds", tm->tm_sec, 0);
	setnr("year", tm->tm_year + 1900, 0);

}


int
ctoi(register char *s)
{
	register int n;

	while (*s == ' ')
		s++;
	n = 0;
	while (isdigit((unsigned char)*s))
		n = 10 * n + *s++ - '0';
	return n;
}


void
mesg(int f)
{
	static int	mode;
	struct stat stbuf;

	if (!f) {
		stat(ttyp, &stbuf);
		mode = stbuf.st_mode;
		chmod(ttyp, mode & ~0122);	/* turn off writing for others */
	} else {
		if (ttyp && *ttyp && mode)
			chmod(ttyp, mode);
	}
}

void
verrprint(const char *s, va_list ap)
{
	fdprintf(stderr, "%s: ", progname);
	vfdprintf(stderr, s, ap);
	if (numtab[CD].val > 0)
		fdprintf(stderr, "; line %d, file %s",
			 numtab[CD].val + (nlflg == 0 && frame == stk),
			 cfname[ifi] ? cfname[ifi] : "");
	if (xflag && realpage)
		fdprintf(stderr, "; page %ld", realpage);
	fdprintf(stderr, "\n");
	stackdump();
#ifdef	DEBUG
	if (debug)
		abort();
#endif	/* DEBUG */
}

void
errprint(const char *s, ...)	/* error message printer */
{
	va_list	ap;

	va_start(ap, s);
	verrprint(s, ap);
	va_end(ap);
}


/*
 * Scaled down version of C Library printf.
 * Only %s %u %d (==%u) %o %c %x %D are recognized.
 */
#undef putchar
#define	putchar(n)	(*pfbp++ = (n))	/* NO CHECKING! */

static char	pfbuf[NTM];
static char	*pfbp = pfbuf;
int	stderr	 = 2;	/* NOT stdio value */

void
fdprintf(int fd, char *fmt, ...)
{
	va_list	ap;

	va_start(ap, fmt);
	vfdprintf(fd, fmt, ap);
	va_end(ap);
}

static void
vfdprintf(int fd, const char *fmt, va_list ap)
{
	register int c;
	char	*s;
	register int i;

	pfbp = pfbuf;
loop:
	while ((c = *fmt++) != '%') {
		if (c == '\0') {
			if (fd == stderr)
				write(stderr, pfbuf, pfbp - pfbuf);
			else {
				*pfbp = 0;
				pfbp = pfbuf;
				while (*pfbp) {
					*obufp++ = *pfbp++;
					if (obufp >= &obuf[OBUFSZ])
						flusho();
				}
			}
			return;
		}
		putchar(c);
	}
	c = *fmt++;
	if (c == 'd' || c == 'u' || c == 'o' || c == 'x') {
		i = va_arg(ap, int);
		printlong(i, c);
	} else if (c == 'c') {
		if (c > 0177 || c < 040)
			putchar('\\');
		putchar(va_arg(ap, int) & 0177);
	} else if (c == 's') {
		s = va_arg(ap, char *);
		while (c = *s++)
			putchar(c);
	} else if (c == 'D') {
		printn(va_arg(ap, long), 10);
	} else if (c == 'O') {
		printn(va_arg(ap, long), 8);
	} else if (c == 'e' || c == 'E' ||
			c == 'f' || c == 'F' ||
			c == 'g' || c == 'G') {
		char	tmp[40];
		char	fmt[] = "%%";
		fmt[1] = c;
		sprintf(s = tmp, fmt, va_arg(ap, double));
		while (c = *s++)
			putchar(c);
	} else if (c == 'p') {
		i = (intptr_t)va_arg(ap, void *);
		putchar('0');
		putchar('x');
		printlong(i, 'x');
	} else if (c == 'l') {
		c = *fmt++;
		if (c == 'd' || c == 'u' || c == 'o' || c == 'x') {
			i = va_arg(ap, long);
			printlong(i, c);
		} else if (c == 'c') {
			i = va_arg(ap, int);
			if (c & ~0177) {
#ifdef	EUC
				char	mb[MB_LEN_MAX];
				int	j, n;
				n = wctomb(mb, i);
				for (j = 0; j < n; j++)
					putchar(mb[j]&0377);
#endif	/* EUC */
			} else
				putchar(i);
		}
	} else if (c == 'C') {
		extern int	nchtab;
		tchar	t = va_arg(ap, tchar);
		if ((i = cbits(t)) < 0177) {
			putchar(i);
		} else if (i < 128 + nchtab) {
			putchar('\\');
			putchar('(');
			putchar(chname[chtab[i-128]]);
			putchar(chname[chtab[i-128]+1]);
		}
#ifndef	NROFF
		else if ((i = tr2un(i, fbits(t))) != -1)
			goto U;
#endif	/* !NROFF */
	} else if (c == 'U') {
		i = va_arg(ap, int);
#ifndef	NROFF
	U:
#endif	/* !NROFF */
		putchar('U');
		putchar('+');
		if (i < 0x1000)
			putchar('0');
		if (i < 0x100)
			putchar('0');
		if (i < 0x10)
			putchar('0');
		printn((long)i, 16);
#ifdef	EUC
		if (iswprint(i)) {
			char	mb[MB_LEN_MAX];
			int	j, n;
			n = wctomb(mb, i);
			putchar(' ');
			putchar('(');
			for (j = 0; j < n; j++)
				putchar(mb[j]&0377);
			putchar(')');
		}
#endif	/* EUC */
	}
	goto loop;
}


static void
printlong(long i, int fmt)
{
	switch (fmt) {
	case 'd':
		if (i < 0) {
			putchar('-');
			i = -i;
		}
		/*FALLTHRU*/
	case 'u':
		printn(i, 10);
		break;
	case 'o':
		printn(i, 8);
		break;
	case 'x':
		printn(i, 16);
		break;
	}
}

/*
 * Print an unsigned integer in base b.
 */
static void printn(register long n, register long b)
{
	register long	a;

	if (n < 0) {	/* shouldn't happen */
		putchar('-');
		n = -n;
	}
	if (a = n / b)
		printn(a, b);
	putchar("0123456789ABCDEF"[(int)(n%b)]);
}

/* scaled down version of library roff_sprintf */
/* same limits as fdprintf */
/* returns pointer to \0 that ends the string */

/* VARARGS2 */
char *roff_sprintf(char *str, char *fmt, ...)
{
	register int c;
	char	*s;
	register int i;
	va_list ap;

	va_start(ap, fmt);
loop:
	while ((c = *fmt++) != '%') {
		if (c == '\0') {
			*str = 0;
			va_end(ap);
			return str;
		}
		*str++ = c;
	}
	c = *fmt++;
	if (c == 'd' || c == 'u' || c == 'o' || c == 'x') {
		i = va_arg(ap, int);
		str = sprintlong(str, i, c);
	} else if (c == 'c') {
		if (c > 0177 || c < 040)
			*str++ = '\\';
		*str++ = va_arg(ap, int) & 0177;
	} else if (c == 's') {
		s = va_arg(ap, char *);
		while (c = *s++)
			*str++ = c;
	} else if (c == 'D') {
		str = sprintn(str, va_arg(ap, long), 10);
	} else if (c == 'O') {
		str = sprintn(str, va_arg(ap, unsigned) , 8);
	} else if (c == 'e' || c == 'E' ||
			c == 'f' || c == 'F' ||
			c == 'g' || c == 'G') {
		char	fmt[] = "%%";
		fmt[1] = c;
		str += sprintf(str, fmt, va_arg(ap, double));
	} else if (c == 'p') {
		i = (intptr_t)va_arg(ap, void *);
		*str++ = '0';
		*str++ = 'x';
		str = sprintlong(str, i, 'x');
	} else if (c == 'l') {
		c = *fmt++;
		if (c == 'd' || c == 'u' || c == 'o' || c == 'x') {
			i = va_arg(ap, long);
			printlong(i, c);
		} else if (c == 'c') {
			i = va_arg(ap, int);
			if (i & ~0177) {
#ifdef	EUC
				int	n;
				n = wctomb(str, i);
				if (n > 0)
					str += n;
#endif	/* EUC */
			} else
				*str++ = i;
		}
	}
	goto loop;
}

static char *
sprintlong(char *s, long i, int fmt)
{
	switch (fmt) {
	case 'd':
		if (i < 0) {
			*s++ = '-';
			i = -i;
		}
		/*FALLTHRU*/
	case 'u':
		s = sprintn(s, i, 10);
		break;
	case 'o':
		s = sprintn(s, i, 8);
		break;
	case 'x':
		s = sprintn(s, i, 16);
		break;
	}
	return s;
}

/*
 * Print an unsigned integer in base b.
 */
static char *sprintn(register char *s, register long n, int b)
{
	register long	a;

	if (n < 0) {	/* shouldn't happen */
		*s++ = '-';
		n = -n;
	}
	if (a = n / b)
		s = sprintn(s, a, b);
	*s++ = "0123456789ABCDEF"[(int)(n%b)];
	return s;
}


int
control(register int a, register int b)
{
	register int	j;
	int	newip;
	struct s	*p;

	if (a == 0 || (j = findmn(a)) == -1) {
		nosuch(a);
		return(0);
	}

	/*
	 * Attempt to find endless recursion at runtime. Arbitrary
	 * recursion limit of MAX_RECURSION_DEPTH was chosen as
	 * it is extremely unlikely that a correct nroff/troff
	 * invocation would exceed this value.
	 *
	 * The depth of tail-recursive macro calls is not limited
	 * by default.
	 */

	if (max_recursion_depth > 0 && frame->frame_cnt > max_recursion_depth) {
		errprint(
		    "Exceeded maximum stack size (%d) when "
		    "executing macro %s. Stack dump follows",
		    max_recursion_depth, macname(frame->mname));
		edone(02);
	}
	if (max_tail_depth > 0 && frame->tail_cnt > max_tail_depth) {
		errprint(
		    "Exceeded maximum recursion depth (%d) when "
		    "executing macro %s. Stack dump follows",
		    max_tail_depth, macname(frame->mname));
		edone(02);
	}

	lastrq = a;

#ifdef	DEBUG
	if (debug & DB_MAC)
		fdprintf(stderr, "control: macro %c%c, contab[%d]\n",
			a&0177, (a>>BYTE)&0177 ? (a>>BYTE)&0177 : ' ', j);
#endif	/* DEBUG */
	if (contab[j].f == 0) {
		nxf->nargs = 0;
		tailflg = 0;
		if (b)
			collect();
		flushi();
		newip = pushi((filep)contab[j].mx, a);
		p = frame->pframe;
		if (tailflg && b && p != stk &&
				p->ppendt == 0 &&
				p->pch == 0 &&
				p->pip == frame->pip &&
				p->lastpbp == frame->lastpbp) {
			frame->pframe = p->pframe;
			frame->frame_cnt--;
			if (p->nargs > 0) {
				free(p->argt);
				free(p->argsp);
			}
			*p = *frame;
			free(frame);
			frame = p;
		}
		tailflg = 0;
		return newip;
	} else if (b) {
		(*contab[j].f)(0);
		return 0;
	} else
		return(0);
}


int
getrq2(void)
{
	register int i, j;

	if (((i = getach()) == 0) || ((j = getach()) == 0))
		goto rtn;
	i = PAIR(i, j);
rtn:
	return(i);
}

int
getrq(int flags)
{
	int	i;

	if ((i = getrq2()) >= 256)
		i = maybemore(i, flags);
	return(i);
}

/*
 * table encodes some special characters, to speed up tests
 * in getchar, viz FLSS, RPT, f, \b, \n, fc, tabch, ldrch
 */

static char
_gchtab[] = {
	000,004,000,000,010,000,000,000, /* fc, ldr */
	001,002,001,000,001,000,000,000, /* \b, tab, nl, RPT */
	000,000,000,000,000,000,000,000,
	000,001,000,000,000,000,000,000, /* FLSS */
	000,000,000,000,000,000,000,000,
	000,000,000,000,000,000,000,000,
	000,000,000,000,000,000,000,000,
	000,000,000,000,000,000,000,000,
	000,000,000,000,000,000,000,000,
	000,000,000,000,000,000,000,000,
	000,000,000,000,000,000,000,000,
	000,000,000,000,000,000,000,000,
	000,000,000,000,000,000,001,000, /* f */
	000,000,000,000,000,000,000,000,
	000,000,000,000,000,000,000,000,
	000,000,000,000,000,000,000,000,
};

static void
initg(void)
{
	memcpy(gchtab, _gchtab, sizeof _gchtab);
}

tchar
getch(void)
{
	register int	k;
	register tchar i, j;

g0:
	if (i = ch) {
#ifdef	DEBUG
		if (debug & DB_GETC)
			fdprintf(stderr, "getch: ch is %x (%c)\n",
				ch, (ch&0177) < 040 ? 0177 : ch&0177);
#endif	/* DEBUG */
		if (cbits(i) == '\n') {
			nlflg++;
			tailflg = istail(i);
		}
		ch = 0;
		return(i);
	}

#ifdef	DEBUG
	if (nlflg)
		if (debug & DB_GETC)
			fdprintf(stderr,"getch: nlflg is %x\n", nlflg);
#endif	/* DEBUG */
	if (nlflg)
		return('\n');
	i = getch0();
#ifdef	DEBUG
	if (debug & DB_GETC)
		fdprintf(stderr, "getch: getch0 returns %x (%c)\n",
			i, (i&0177) < 040 ? 0177 : i&0177);
#endif	/* DEBUG */
	if (ismot(i))
		return(i);
	k = cbits(i);
	if (k != ESC) {
		if (i & MBMASK || gchtab[k]==0)
			return(i);
		if (k == '\n') {
			if (cbits(i) == '\n') {
				nlflg++;
				tailflg = istail(i);
			}
			return(k);
		}
		if (k == FLSS) {
			copyf++; 
			raw++;
			i = getch0();
			if (!fi)
				flss = i;
			copyf--; 
			raw--;
			goto g0;
		}
		if (k == RPT) {
			setrpt();
			goto g0;
		}
		if (!copyf) {
			if (gchtab[k]&LGBIT && !isdi(i) && lg && !lgf) {
				i = getlg(i);
				return(i);
			}
			if (k == fc || k == tabch || k == ldrch) {
				if ((i = setfield(k)) == 0)
					goto g0; 
				else 
					return(i);
			}
			if (k == '\b') {
				i = makem(-width(' ' | chbits));
				return(i);
			}
		}
		return(i);
	}
	k = cbits(j = getch0());
	if (ismot(j))
		return(j);
	switch (k) {

	case 'X':	/* \X'...' for copy through */
		setxon();
		goto g0;
	case '\n':	/* concealed newline */
		goto g0;
	case 'n':	/* number register */
		setn();
		goto g0;
	case '*':	/* string indicator */
		setstr();
		goto g0;
	case '$':	/* argument indicator */
		seta();
		goto g0;
	case '{':	/* LEFT */
		i = LEFT;
		goto gx;
	case '}':	/* RIGHT */
		i = RIGHT;
		goto gx;
	case '#':	/* comment including newline */
		if (xflag == 0)
			break;
		/*FALLTHRU*/
	case '"':	/* comment */
		while (cbits(i = getch0()) != '\n')
			;
		if (k == '#')
			goto g0;
		nlflg++;
		tailflg = istail(i);
		return(i);
	case ESC:	/* double backslash */
		i = eschar;
		goto gx;
	case 'e':	/* printable version of current eschar */
		i = PRESC;
		goto gx;
	case ' ':	/* unpaddable space */
		i = UNPAD;
		goto gx;
	case '~':	/* stretchable but unbreakable space */
		if (xflag == 0)
			break;
		i = STRETCH;
		goto gx;
	case '\'':	/* \(aa */
		i = ACUTE;
		goto gx;
	case '`':	/* \(ga */
		i = GRAVE;
		goto gx;
	case '_':	/* \(ul */
		i = UNDERLINE;
		goto gx;
	case '-':	/* current font minus */
		i = MINUS;
		goto gx;
	case '&':	/* filler */
		i = FILLER;
		goto gx;
	case 'c':	/* to be continued */
		i = CONT;
		goto gx;
	case '!':	/* transparent indicator */
		i = XPAR;
		goto gx;
	case 't':	/* tab */
		i = '\t';
		return(i);
	case 'a':	/* leader (SOH) */
		i = LEADER;
		return(i);
	case '%':	/* ohc */
		i = OHC;
		return(i);
	case ':':	/* optional line break but no hyphenation */
		if (xflag == 0)
			break;
		i = OHC | BLBIT;
		return(i);
	case 'g':	/* return format of a number register */
		setaf();
		goto g0;
	case 'N':	/* absolute character number */
		i = setabs();
		goto gx;
	case 'V':	/* environment variable */
		if (xflag == 0)
			break;
		_setenv();
		goto g0;
	case '.':	/* . */
		i = '.';
gx:
		setsfbits(i, sfbits(j));
		return(i);
	}
	if (copyf) {
		pbbuf[pbp++] = j;
		return(eschar);
	}
	switch (k) {

	case '[':
		if (xflag == 0)
			goto dfl;
		/*FALLTHRU*/
	case 'C':
	case '(':	/* special char name */
		if ((i = setch(k)) == 0 && !tryglf)
			goto g0;
		return(i);
	case 'U':	/* Unicode character */
		if (xflag == 0)
			goto dfl;
		if ((i = setuc()) == 0 && !tryglf)
			goto g0;
		return(i);
	case 'E':	/* printable version of current eschar */
		if (xflag == 0)
			goto dfl;
		i = PRESC;
		goto gx;
	}
	if (tryglf) {
		pbbuf[pbp++] = j;
		return(eschar);
	}
	switch (k) {

	case 'p':	/* spread */
		spread++;
		goto g0;
	case 's':	/* size indicator */
		setps();
		goto g0;
	case 'H':	/* character height */
		return(setht());
	case 'S':	/* slant */
		return(setslant());
	case 'f':	/* font indicator */
		setfont(0);
		goto g0;
	case 'w':	/* width function */
		setwd();
		goto g0;
	case 'v':	/* vert mot */
		if (i = vmot())
			return(i);
		goto g0;
	case 'h': 	/* horiz mot */
		if (i = hmot())
			return(i);
		goto g0;
	case 'z':	/* zero with char */
		return(setz());
	case 'l':	/* hor line */
		setline();
		goto g0;
	case 'L':	/* vert line */
		setvline();
		goto g0;
	case 'D':	/* drawing function */
		setdraw();
		goto g0;
	case 'b':	/* bracket */
		setbra();
		goto g0;
	case 'o':	/* overstrike */
		setov();
		goto g0;
	case 'k':	/* mark hor place */
		if ((k = findr(getsn())) != -1) {
			numtab[k].val = numtab[HP].val;
		}
		goto g0;
	case '0':	/* number space */
		return(makem(width('0' | chbits)));
#ifdef NROFF
	case '|':
	case '^':
		goto g0;
#else
	case '|':	/* narrow space */
		return(makem((int)(EM)/6));
	case '^':	/* half narrow space */
		return(makem((int)(EM)/12));
#endif
	case 'x':	/* extra line space */
		if (i = xlss())
			return(i);
		goto g0;
	case 'u':	/* half em up */
	case 'r':	/* full em up */
	case 'd':	/* half em down */
		return(sethl(k));
	case 'A':	/* set anchor */
		if (xflag == 0 || (j = setanchor()) == 0)
			goto g0;
		return(j);
	case 'T':
		if (xflag == 0 || (j = setlink()) == 0)
			goto g0;
		return(j);
	case 'R':
		if (xflag)
			setr();
		goto g0;
	case ';':	/* ligature suppressor (only) */
		if (xflag)
			goto g0;
		/*FALLTHRU*/
	default:
		if (warn & WARN_ESCAPE)
			errprint("undefined escape sequence \\%c", k);
	dfl:	return(j);
	}
	/* NOTREACHED */
}

void
setxon(void)	/* \X'...' for copy through */
{
	tchar xbuf[NC];
	register tchar *i;
	tchar c;
	int delim, k;

	if (ismot(c = getch()))
		return;
	delim = cbits(c);
	i = xbuf;
	*i++ = XON;
	while ((k = cbits(c = getch())) != delim && k != '\n' && i < xbuf+NC-1) {
		if (k == ' ')
			setcbits(c, UNPAD);
		*i++ = c | ZBIT;
	}
	*i++ = XOFF;
	*i = 0;
	pushback(xbuf);
}


char	ifilt[32] = {
	0, 001, 002, 003, 0, 005, 006, 007, 010, 011, 012};

tchar getch0(void)
{
	register int	j;
	register tchar i;
#ifdef	EUC
	register int	n;
#ifdef	NROFF
	int col_index;
#endif	/* NROFF */
#endif	/* EUC */

again:
	if (pbp > lastpbp)
		i = pbbuf[--pbp];
	else if (ip) {
		extern tchar *corebuf;
		i = corebuf[ip];
		if (i == 0)
			i = rbf();
		else {
			if ((++ip & (BLK - 1)) == 0) {
				--ip;
				(void)rbf();
			}
		}
	} else {
		if (donef || ndone)
			done(0);
		if (nx || ibufp >= eibuf) {
			if (nfo==0) {
g0:
				if (nextfile()) {
					if (ip)
						goto again;
					if (ibufp < eibuf)
						goto g2;
				}
			}
			nx = 0;
			if ((j = read(ifile, ibuf, IBUFSZ)) <= 0)
				goto g0;
			ibufp = ibuf;
			eibuf = ibuf + j;
			if (ip)
				goto again;
		}
g2:
#ifndef	EUC
		i = *ibufp++ & 0177;
		ioff++;
		if (i >= 040 && i < 0177)
#else	/* EUC */
#ifndef	NROFF
		i = *ibufp++ & 0377;
		ioff++;
		*mbbuf1p++ = i;
		*mbbuf1p = 0;
		if (multi_locale && (*mbbuf1&~(wchar_t)0177)) {
			mbstate_t	state;
			memset(&state, 0, sizeof state);
			if ((n = mbrtowc(&twc, mbbuf1, mbbuf1p-mbbuf1, &state))
					==(size_t)-1 ||
					twc & ~(wchar_t)0x1FFFFF) {
				illseq(-1, mbbuf1, mbbuf1p-mbbuf1);
				mbbuf1p = mbbuf1;
				*mbbuf1p = 0;
				i &= 0177;
			} else if (n == (size_t)-2)
				goto again;
			else {
				mbbuf1p = mbbuf1;
				*mbbuf1p = 0;
				i = twc | COPYBIT;
				goto g4;
			}
		} else {
			mbbuf1p = mbbuf1;
			*mbbuf1p = 0;
			if (!raw)
				i &= 0177;
		}
		if (i >= 040 && i < 0177)
#else	/* NROFF */
		i = *ibufp++ & 0377;
		*mbbuf1p++ = i;
		*mbbuf1p = 0;
		if (!multi_locale) {
			twc = 0;
			mbbuf1p = mbbuf1;
		} else if ((*mbbuf1&~(wchar_t)0177) == 0) {
			twc = *mbbuf1;
			i |= (BYTE_CHR);
			setcsbits(i, 0);
			twc = 0;
			mbbuf1p = mbbuf1;
		} else if ((n = mbtowc(&twc, mbbuf1, mb_cur_max)) <= 0) {
			if (mbbuf1p >= mbbuf1 + mb_cur_max) {
				illseq(-1, mbbuf1, mb_cur_max);
				i &= ~(MBMASK | CSMASK);
				twc = 0;
				mbbuf1p = mbbuf1;
				*mbbuf1p = 0;
			} else {
				i |= (MIDDLEOFMB);
			}
		} else {
			if (n > 1)
				i |= (LASTOFMB);
			else
				i |= (BYTE_CHR);
			if (isascii(twc)) {
				col_index = 0;
			} else {
				if ((col_index = wcwidth(twc)) < 0)
					col_index = 0;
			}
			setcsbits(i, col_index);
			twc = 0;
			mbbuf1p = mbbuf1;
		}
		ioff++;
		if (i >= 040 && i != 0177)
#endif	/* NROFF */
#endif	/* EUC */
			goto g4;
		if (i != 0177) {
			if (i != ifilt[i])
				illseq(i, NULL, 0);
			i = ifilt[i];
		} else
			illseq(i, NULL, 0);
		if (i == '\n')
			numtab[CD].val++; /* line number */
	}
	if (cbits(i) == IMP && !raw)
		goto again;
	if ((i == 0 || i == 0177) && !init && !raw) {
		goto again;
	}
g4:
	if (!copyf && iscopy(i))
		i = setuc0(cbits(i));
#if !defined (EUC) || !defined (NROFF)
	if (copyf == 0 && (i & ~BYTEMASK) == 0)
#else	/* EUC && NROFF */
	if (copyf == 0 && (i & ~CHMASK) == 0)
#endif	/* EUC && NROFF */
		i |= chbits;
#if defined (EUC) && defined (NROFF)
	if (multi_locale)
		if (i & MBMASK1)
			i |= ZBIT;
#endif /* EUC && NROFF */
	if (cbits(i) == eschar && !raw)
		setcbits(i, ESC);
	return(i);
}

void
pushback(register tchar *b)
{
	register tchar *ob = b;

	while (*b++)
		;
	b--;
	while (b > ob) {
		if (pbp >= pbsize-3)
			if (growpbbuf() == NULL) {
				errprint("pushback overflow");
				done(2);
			}
		pbbuf[pbp++] = *--b;
	}
}

void
cpushback(register char *b)
{
	register char *ob = b;

	while (*b++)
		;
	b--;
	while (b > ob) {
		if (pbp >= pbsize-3)
			if (growpbbuf() == NULL) {
				errprint("cpushback overflow");
				done(2);
			}
		pbbuf[pbp++] = *--b;
	}
}

tchar *
growpbbuf(void)
{
	tchar	*npb;
	int	inc = NC;

	if ((npb = realloc(pbbuf, (pbsize + inc) * sizeof *pbbuf)) == NULL)
		return NULL;
	pbsize += inc;
	return pbbuf = npb;
}

int
nextfile(void)
{
	register char	*p;

n0:
	if (ifile)
		close(ifile);
	if (nx  ||  nmfi < mflg) {
		p = mfiles[nmfi++];
		if (*p != 0)
			goto n1;
	}
	if (ifi > 0) {
		if (popf())
			goto n0; /* popf error */
		return(1); /* popf ok */
	}
	if (rargc-- <= 0) {
		if ((nfo -= mflg) && !stdi)
			done(0);
		nfo++;
		numtab[CD].val = ifile = stdi = mflg = 0;
		free(cfname[ifi]);
		cfname[ifi] = malloc(17);
		strcpy(cfname[ifi], "<standard input>");
		ioff = 0;
		return(0);
	}
	p = (argp++)[0];
n1:
	numtab[CD].val = 0;
	if (p[0] == '-' && p[1] == 0) {
		ifile = 0;
		free(cfname[ifi]);
		cfname[ifi] = malloc(17);
		strcpy(cfname[ifi], "<standard input>");
	} else if ((ifile = open(p, O_RDONLY)) < 0) {
		errprint("cannot open file %s", p);
		nfo -= mflg;
		done(02);
	} else {
		free(cfname[ifi]);
		cfname[ifi] = malloc(strlen(p) + 1);
		strcpy(cfname[ifi],p);
	}
	nfo++;
	ioff = 0;
	return(0);
}


int
popf(void)
{
	register int i;
	register char	*p, *q;

	if (cfpid[ifi] != -1) {
		while (waitpid(cfpid[ifi], NULL, 0) != cfpid[ifi]);
		cfpid[ifi] = -1;
	}
	ioff = offl[--ifi];
	numtab[CD].val = cfline[ifi];		/*restore line counter*/
	ip = ipl[ifi];
	if ((ifile = ifl[ifi]) == 0) {
		p = xbuf;
		q = ibuf;
		ibufp = xbufp;
		eibuf = xeibuf;
		while (q < eibuf)
			*q++ = *p++;
		return(0);
	}
	if (lseek(ifile, ioff & ~(IBUFSZ-1), SEEK_SET) == -1
	   || (i = read(ifile, ibuf, IBUFSZ)) < 0)
		return(1);
	eibuf = ibuf + i;
	ibufp = ibuf;
	if (ttyname(ifile) == 0)
		/* was >= ... */
		if ((ibufp = ibuf + (int)(ioff & (IBUFSZ - 1))) > eibuf)
			return(1);
	return(0);
}


void
flushi(void)
{
	if (nflush)
		return;
	ch = 0;
	copyf++;
	while (!nlflg) {
		if (donef && (frame == stk))
			break;
		getch();
	}
	copyf--;
}


int
getach(void)
{
	register tchar i;
	register int j;

	lgf++;
	j = cbits(i = getch());
#if !defined (EUC) || !defined (NROFF)
	if (ismot(i) || j == ' ' || j == '\n' || j & 0200) {
		if (j >= 0200)
#else	/* EUC && NROFF */
	if (ismot(i) || j == ' ' || j == '\n' || j > 0200) {
		if (j > 0200)
#endif	/* EUC && NROFF */
			illseq(j, NULL, -3);

		ch = i;
		j = 0;
	}
	lgf--;
	return(j & 0177);
}


void
casenx(void)
{
	struct s *pp;
	lgf++;
	skip(0);
	getname();
	nx++;
	if (nmfi > 0)
		nmfi--;
	if (mfiles == NULL)
		mfiles = calloc(1, sizeof *mfiles);
	free(mfiles[nmfi]);
	mfiles[nmfi] = malloc(NS);
	strcpy(mfiles[nmfi], nextf);
	nextfile();
	nlflg++;
	tailflg = 0;
	ip = 0;
	pendt = 0;
	while (frame != stk) {
		pp = frame;
		frame = frame->pframe;
		if (pp->nargs > 0) {
			free(pp->argt);
			free(pp->argsp);
		}
		free(pp);
	}
	nxf = calloc(1, sizeof *nxf);
}


int
getname(void)
{
	register int	j, k;
	tchar i;

	lgf++;
	for (k = 0; ; k++) {
#if !defined (EUC) || !defined (NROFF)
		if (((j = cbits(i = getch())) <= ' ') || (j > 0176))
#else	/* EUC && NROFF */
		if (((j = cbits(i = getch())) <= ' ') || (j == 0177))
#endif /* EUC && NROFF */
			break;
		if (k + 1 >= NS)
			nextf = realloc(nextf, NS += 14);
		nextf[k] = j & BYTEMASK;
	}
	nextf[k] = 0;
	ch = i;
	lgf--;
	return(nextf[0]);
}

tchar
setuc(void)
{
	char	c, d, b[NC], *bp;
	int	i = 0, n;

	d = getach();
	do {
		c = getach();
		if (i >= sizeof b)
			return 0;
		b[i++] = c;
	} while (c && c != d);
	b[--i] = 0;
	if (i == 0 || c != d)
		return 0;
	n = strtol(b, &bp, 16);
	if (n == 0 || *bp != '\0')
		return 0;
	return setuc0(n);
}


static void
_setenv(void)
{
	int	a = 0, i = 0, c, delim;
	char	*np = NULL, *vp;

	if ((delim = getach()) == 0)
		return;
	switch (delim) {
	case '[':
		for (;;) {
			if (i + 1 >= a)
				np = realloc(np, a += 32);
			if ((c = getach()) == 0) {
				nodelim(']');
				break;
			}
			if (c == ']')
				break;
			np[i++] = c;
		}
		np[i] = 0;
		break;
	case '(':
		np = malloc(a = 3);
		np[0] = delim;
		np[1] = getach();
		np[2] = 0;
		break;
	default:
		np = malloc(a = 2);
		np[0] = delim;
		np[1] = 0;
	}
	if ((vp = getenv(np)) != NULL)
		cpushback(vp);
	free(np);
}


static void
sopso(int i, pid_t pid)
{
	register char	*p, *q;

	free(cfname[ifi+1]);
	cfname[ifi+1] = malloc(NS);
	strcpy(cfname[ifi+1], nextf);
	cfline[ifi] = numtab[CD].val;		/*hold line counter*/
	numtab[CD].val = 0;
	flushi();
	cfpid[ifi+1] = pid;
	ifl[ifi] = ifile;
	ifile = i;
	offl[ifi] = ioff;
	ioff = 0;
	ipl[ifi] = ip;
	ip = 0;
	nx++;
	nflush++;
	if (!ifl[ifi++]) {
		p = ibuf;
		q = xbuf;
		xbufp = ibufp;
		xeibuf = eibuf;
		while (p < eibuf)
			*q++ = *p++;
	}
}

void
caseso(void)
{
	register int i = 0;

	lgf++;
	nextf[0] = 0;
	if (skip(1))
		done(02);
	if (!getname() || ((i = open(nextf, O_RDONLY)) < 0) ||
			(ifi >= NSO)) {
		errprint("can't open file %s", nextf);
		done(02);
	}
	sopso(i, -1);
}

void
casepso(void)
{
	int	pd[2];
	int	c, i, k;
	pid_t	pid;

	lgf++;
	nextf[0] = 0;
	if (skip(1))
		done(02);
	if (ifi >= NSO || pipe(pd) < 0) {
		errprint("can't .pso");
		done(02);
	}
	for (k = 0; ; k++) {
		if ((c = cbits(i = getch())) == '\n' || c == 0)
			break;
		if (k + 1 >= NS)
			nextf = realloc(nextf, NS += 14);
		nextf[k] = c & BYTEMASK;
	}
	nextf[k] = 0;
	switch (pid = fork()) {
	case 0:
		close(pd[0]);
		close(1);
		dup(pd[1]);
		close(pd[1]);
		execl(SHELL, "sh", "-c", nextf, NULL);
		_exit(0177);
		/*NOTREACHED*/
	case -1:
		errprint("can't fork");
		done(02);
		/*NOTREACHED*/
	}
	close(pd[1]);
	sopso(pd[0], pid);
}

void
caself(void)	/* set line number and file */
{
	int n;

	if (skip(1))
		return;
	n = atoi();
	cfline[ifi] = numtab[CD].val = n - 2;
	if (skip(0))
		return;
	if (getname()) {
		free(cfname[ifi]);
		cfname[ifi] = malloc(NS);
		strcpy(cfname[ifi], nextf);
	}
}


void
casecf(void)
{	/* copy file without change */
#ifndef NROFF
	int	fd = -1, n;
	char	buf[512];
	extern int hpos, esc, po;
	nextf[0] = 0;
	if (skip(1))
		return;
	if (!getname() || (fd = open(nextf, O_RDONLY)) < 0) {
		errprint("can't open file %s", nextf);
		done(02);
	}
	tbreak();
	/* make it into a clean state, be sure that everything is out */
	hpos = po;
	esc = un;
	ptesc();
	ptlead();
	ptps();
	ptfont();
	flusho();
	while ((n = read(fd, buf, sizeof buf)) > 0)
		write(ptid, buf, n);
	close(fd);
#endif
}

void
casesy(void)	/* call system */
{
	char	sybuf[NTM];
	int	i;

	lgf++;
	copyf++;
	skip(1);
	for (i = 0; i < NTM - 2; i++)
		if ((sybuf[i] = getch()) == '\n')
			break;
	sybuf[i] = 0;
	system(sybuf);
	copyf--;
	lgf--;
}


void
getpn(register char *a)
{
	register int n, neg;

	if (*a == 0)
		return;
	neg = 0;
	for ( ; *a; a++)
		switch (*a) {
		case '+':
		case ',':
			continue;
		case '-':
			neg = 1;
			continue;
		default:
			n = 0;
			if (isdigit((unsigned char)*a)) {
				do
					n = 10 * n + *a++ - '0';
				while (isdigit((unsigned char)*a));
				a--;
			} else
				n = 9999;
			*pnp++ = neg ? -n : n;
			neg = 0;
			if (pnp >= &pnlist[NPN-2]) {
				errprint("too many page numbers");
				done3(-3);
			}
		}
	if (neg)
		*pnp++ = -9999;
	*pnp = -32767;
	print = 0;
	pnp = pnlist;
	if (*pnp != -32767)
		chkpn();
}


void
setrpt(void)
{
	tchar i, j;

	copyf++;
	raw++;
	i = getch0();
	copyf--;
	raw--;
	if (i < 0 || cbits(j = getch0()) == RPT)
		return;
	i &= BYTEMASK;
	while (i>0) {
		if (pbp >= pbsize-3)
			if (growpbbuf() == NULL)
				break;
		i--;
		pbbuf[pbp++] = j;
	}
}


void
casedb(void)
{
#ifdef	DEBUG
	debug = 0;
	if (skip(1))
		return;
	noscale++;
	debug = max(atoi(), 0);
	noscale = 0;
#endif	/* DEBUG */
}

void
casexflag(void)
{
	int	i;

	skip(1);
	i = atoi();
	if (!nonumb)
		_xflag = xflag = i & 3;
}

void
caserecursionlimit(void)
{
	skip(1);
	noscale++;
	max_recursion_depth = atoi();
	skip(0);
	max_tail_depth = atoi();
	noscale--;
}
