/*
 * This code contains changes by
 *      Gunnar Ritter, Freiburg i. Br., Germany, 2002. All rights reserved.
 *
 * Conditions 1, 2, and 4 and the no-warranty notice below apply
 * to these changes.
 *
 *
 * Copyright (c) 1980, 1993
 * 	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 * 	This product includes software developed by the University of
 * 	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *
 * Copyright(C) Caldera International Inc. 2001-2002. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *   Redistributions of source code and documentation must retain the
 *    above copyright notice, this list of conditions and the following
 *    disclaimer.
 *   Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *   All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *      This product includes software developed or owned by Caldera
 *      International, Inc.
 *   Neither the name of Caldera International, Inc. nor the names of
 *    other contributors may be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * USE OF THE SOFTWARE PROVIDED FOR UNDER THIS LICENSE BY CALDERA
 * INTERNATIONAL, INC. AND CONTRIBUTORS ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL CALDERA INTERNATIONAL, INC. BE
 * LIABLE FOR ANY DIRECT, INDIRECT INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef	lint
#ifdef	DOSCCS
static char sccsid[] = "@(#)ex_re.c	1.45 (gritter) 2/13/05";
#endif
#endif

/* from ex_re.c	7.5 (Berkeley) 6/7/85 */

#include "ex.h"
#include "ex_re.h"

/*
 * Global, substitute and regular expressions.
 * Very similar to ed, with some re extensions and
 * confirmed substitute.
 */
void 
global(int k)
{
	register char *gp;
	register int c, i;
	register line *a1;
	char	mb[MB_LEN_MAX+1];
	char globuf[GBSIZE], *Cwas;
	int lines = lineDOL();
	int oinglobal = inglobal;
	char *oglobp = globp;

	Cwas = Command;
	/*
	 * States of inglobal:
	 *  0: ordinary - not in a global command.
	 *  1: text coming from some buffer, not tty.
	 *  2: like 1, but the source of the buffer is a global command.
	 * Hence you're only in a global command if inglobal==2. This
	 * strange sounding convention is historically derived from
	 * everybody simulating a global command.
	 */
	if (inglobal==2)
		error(catgets(catd, 1, 121,
				"Global within global@not allowed"));
	markDOT();
	setall();
	nonzero();
	if (skipend())
		error(catgets(catd, 1, 122,
		"Global needs re|Missing regular expression for global"));
	c = GETWC(mb);
	ignore(compile(c, 1));
	savere(&scanre);
	gp = globuf;
	while ((c = GETWC(mb)) != '\n') {
		switch (c) {

		case EOF:
			c = '\n';
			goto brkwh;

		case '\\':
			c = GETWC(mb);
			switch (c) {

			case '\\':
				ungetchar(c);
				break;

			case '\n':
				break;

			default:
				*gp++ = '\\';
				break;
			}
			break;
		}
		for (i = 0; mb[i]; i++) {
			*gp++ = mb[i];
			if (gp >= &globuf[GBSIZE - 2])
				error(catgets(catd, 1, 123,
						"Global command too long"));
		}
	}
brkwh:
	ungetchar(c);
/* out: */
	newline();
	*gp++ = c;
	*gp++ = 0;
	saveall();
	inglobal = 2;
	for (a1 = one; a1 <= dol; a1++) {
		*a1 &= ~01;
		if (a1 >= addr1 && a1 <= addr2 && execute(0, a1) == k)
			*a1 |= 01;
	}
#ifdef notdef
/*
 * This code is commented out for now.  The problem is that we don't
 * fix up the undo area the way we should.  Basically, I think what has
 * to be done is to copy the undo area down (since we shrunk everything)
 * and move the various pointers into it down too.  I will do this later
 * when I have time. (Mark, 10-20-80)
 */
	/*
	 * Special case: g/.../d (avoid n^2 algorithm)
	 */
	if (globuf[0]=='d' && globuf[1]=='\n' && globuf[2]=='\0') {
		gdelete();
		return;
	}
#endif
	if (inopen)
		inopen = -1;
	/*
	 * Now for each marked line, set dot there and do the commands.
	 * Note the n^2 behavior here for lots of lines matching.
	 * This is really needed: in some cases you could delete lines,
	 * causing a marked line to be moved before a1 and missed if
	 * we didn't restart at zero each time.
	 */
	for (a1 = one; a1 <= dol; a1++) {
		if (*a1 & 01) {
			*a1 &= ~01;
			dot = a1;
			globp = globuf;
			commands(1, 1);
			a1 = zero;
		}
	}
	globp = oglobp;
	inglobal = oinglobal;
	endline = 1;
	Command = Cwas;
	netchHAD(lines);
	setlastchar(EOF);
	if (inopen) {
		ungetchar(EOF);
		inopen = 1;
	}
}

/*
 * gdelete: delete inside a global command. Handles the
 * special case g/r.e./d. All lines to be deleted have
 * already been marked. Squeeze the remaining lines together.
 * Note that other cases such as g/r.e./p, g/r.e./s/r.e.2/rhs/,
 * and g/r.e./.,/r.e.2/d are not treated specially.  There is no
 * good reason for this except the question: where to you draw the line?
 */
void 
gdelete(void)
{
	register line *a1, *a2, *a3;

	a3 = dol;
	/* find first marked line. can skip all before it */
	for (a1=zero; (*a1&01)==0; a1++)
		if (a1>=a3)
			return;
	/* copy down unmarked lines, compacting as we go. */
	for (a2=a1+1; a2<=a3;) {
		if (*a2&01) {
			a2++;		/* line is marked, skip it */
			dot = a1;	/* dot left after line deletion */
		} else
			*a1++ = *a2++;	/* unmarked, copy it */
	}
	dol = a1-1;
	if (dot>dol)
		dot = dol;
	change();
}

bool	cflag;
int	scount, slines, stotal;

int 
substitute(int c)
{
	register line *addr;
	register int n;
	int gsubf, hopcount;

	gsubf = compsub(c);
	if(FIXUNDO)
		save12(), undkind = UNDCHANGE;
	stotal = 0;
	slines = 0;
	for (addr = addr1; addr <= addr2; addr++) {
		scount = hopcount = 0;
		if (dosubcon(0, addr) == 0)
			continue;
		if (gsubf) {
			/*
			 * The loop can happen from s/\</&/g
			 * but we don't want to break other, reasonable cases.
			 */
			while (*loc2) {
				if (++hopcount > sizeof linebuf)
					error(catgets(catd, 1, 124,
							"substitution loop"));
				if (dosubcon(1, addr) == 0)
					break;
			}
		}
		if (scount) {
			stotal += scount;
			slines++;
			putmark(addr);
			n = append(getsub, addr);
			addr += n;
			addr2 += n;
		}
	}
	if (stotal == 0 && !inglobal && !cflag)
		error(catgets(catd, 1, 125,
				"Fail|Substitute pattern match failed"));
	snote(stotal, slines);
	return (stotal);
}

int 
compsub(int ch)
{
	register int seof, c, uselastre;
	char	mb[MB_LEN_MAX+1];
	static int gsubf;

	if (!value(EDCOMPATIBLE))
		gsubf = cflag = 0;
	uselastre = 0;
	switch (ch) {

	case 's':
		ignore(skipwh());
		seof = GETWC(mb);
		if (endcmd(seof) || any(seof, "gcr")) {
			ungetchar(seof);
			goto redo;
		}
		if (xisalnum(seof))
			error(catgets(catd, 1, 126,
	"Substitute needs re|Missing regular expression for substitute"));
		seof = compile(seof, 1);
		uselastre = 1;
		comprhs(seof);
		gsubf = 0;
		cflag = 0;
		break;

	case '~':
		uselastre = 1;
		/* fall into ... */
	case '&':
	redo:
		if (re.Expbuf[0] == 0)
			error(catgets(catd, 1, 127,
			"No previous re|No previous regular expression"));
		if (subre.Expbuf[0] == 0)
			error(catgets(catd, 1, 128,
	"No previous substitute re|No previous substitute to repeat"));
		break;
	}
	for (;;) {
		c = getchar();
		switch (c) {

		case 'g':
			gsubf = !gsubf;
			continue;

		case 'c':
			cflag = !cflag;
			continue;

		case 'r':
			uselastre = 1;
			continue;

		default:
			ungetchar(c);
			setcount();
			newline();
			if (uselastre)
				savere(&subre);
			else
				resre(&subre);
			return (gsubf);
		}
	}
}

void
comprhs(int seof)
{
	register char *rp, *orp;
	char	mb[MB_LEN_MAX+1];
#ifdef	BIT8
	char *qp, *oqp;
#endif
	register int c, i;
#ifdef	BIT8
	int q;
#endif
	char orhsbuf[RHSSIZE];
#ifdef	BIT8
	char orhsquo[RHSSIZE];
#endif
	int	hashflag = 0;

	rp = rhsbuf;
#ifdef	BIT8
	qp = rhsquo;
#endif
	CP(orhsbuf, rp);
#ifdef	BIT8
	copy(orhsquo, qp, (size_t) strlen(rp));
#endif
	for (;;) {
		c = GETWC(mb);
#ifdef	BIT8
		q = 0;
#endif
		if (c == seof)
			break;
		switch (c) {

		case '%':
			if (rp == rhsbuf)
				hashflag = 1;
			break;

		case '\\':
			c = GETWC(mb);
			if (c == EOF) {
				ungetchar(c);
				break;
			}
			if (value(MAGIC)) {
				/*
				 * When "magic", \& turns into a plain &,
				 * and all other chars work fine quoted.
				 */
				if (c != '&')
#ifndef	BIT8
					c |= QUOTE;
#else
					q = 1;
#endif
				break;
			}
magic:
			if (c == '~') {
hash:
#ifndef	BIT8
				for (orp = orhsbuf; *orp; *rp++ = *orp++) {
#else
				for (orp = orhsbuf, oqp = orhsquo;
						*orp; *rp++ = *orp++) {
					*qp++ = *oqp++;
#endif
					if (rp >= &rhsbuf[RHSSIZE - 1])
						goto toobig;
				}
				if (hashflag & 2)
					goto endrhs;
				continue;
			}
#ifndef	BIT8
			c |= QUOTE;
#else
			q = 1;
#endif
			break;

		case '\n':
		case EOF:
			if (!(globp && globp[0])) {
				ungetchar(c);
				goto endrhs;
			}

		case '~':
		case '&':
			if (value(MAGIC))
				goto magic;
			break;
		}
		if (rp >= &rhsbuf[RHSSIZE - 1]) {
toobig:
			*rp = 0;
			error(catgets(catd, 1, 129,
		"Replacement pattern too long@- limit 256 characters"));
		}
		for (i = 0; mb[i]; i++) {
			*rp++ = mb[i];
#ifdef	BIT8
			*qp++ = q;
#endif
		}
	}
endrhs:
	if (hashflag == 1 && rhsbuf[0] == '%' && rp == &rhsbuf[1]) {
		rp = rhsbuf;
		hashflag |= 2;
		goto hash;
	}
	*rp++ = 0;
}

int
getsub(void)
{
	register char *p;

	if ((p = linebp) == 0)
		return (EOF);
	strcLIN(p);
	linebp = 0;
	return (0);
}

int
dosubcon(bool f, line *a)
{

	if (execute(f, a) == 0)
		return (0);
	if (confirmed(a)) {
		dosub();
		scount++;
	}
	return (1);
}

int
confirmed(line *a)
{
	register int c;
	char *yesstr = catgets(catd, 1, 249, "y");
	int okay = -1;

	if (cflag == 0)
		return (1);
	pofix();
	pline(lineno(a));
	if (inopen)
		putchar('\n' | QUOTE);
	c = column(loc1 - 1);
	ugo(c - 1 + (inopen ? 1 : 0), ' ');
	ugo(column(loc2 - 1) - c, '^');
	flush();
	c = getkey();
again:
	if (c == '\r')
		c = '\n';
	if (inopen)
		putchar(c), flush();
	if (c != '\n' && c != EOF) {
		if (okay && *yesstr) {
			if (c == (*yesstr++ & 0377))
				okay = 1;
			else
				okay = 0;
		}
		c = getkey();
		goto again;
	}
	noteinp();
	return (okay > 0);
}

#ifdef	notdef
int
ex_getch(void)
{
	char c;

	if (read(2, &c, 1) != 1)
		return (EOF);
#ifndef	BIT8
	return (c & TRIM);
#else
	return c;
#endif
}
#endif	/* notdef */

void
ugo(int cnt, int with)
{

	if (cnt > 0)
		do
			putchar(with);
		while (--cnt > 0);
}

int	casecnt;
bool	destuc;

void
dosub(void)
{
	register char *lp, *sp, *rp;
	int c, n;
#ifdef	BIT8
	register char *qp;
	int q;
#endif

	lp = linebuf;
	sp = genbuf;
	rp = rhsbuf;
#ifdef	BIT8
	qp = rhsquo;
#endif
	while (lp < loc1)
		*sp++ = *lp++;
	casecnt = 0;
	while (*rp) {
		nextc(c, rp, n);
		rp += n;
#ifdef	BIT8
		c &= TRIM;
		q = *qp;
		qp += n;
#endif
		/* ^V <return> from vi to split lines */
		if (c == '\r')
			c = '\n';

#ifndef	BIT8
		if (c & QUOTE)
			switch (c & TRIM) {
#else
		if (q)
			switch (c) {
#endif

			case '&':
				sp = place(sp, loc1, loc2);
				if (sp == 0)
					goto ovflo;
				continue;

			case 'l':
				casecnt = 1;
				destuc = 0;
				continue;

			case 'L':
				casecnt = LBSIZE;
				destuc = 0;
				continue;

			case 'u':
				casecnt = 1;
				destuc = 1;
				continue;

			case 'U':
				casecnt = LBSIZE;
				destuc = 1;
				continue;

			case 'E':
			case 'e':
				casecnt = 0;
				continue;
			}
#ifndef	BIT8
		if (c < 0 && (c &= TRIM) >= '1' && c < nbra + '1') {
#else
		if (q && c >= '1' && c < nbra + '1') {
#endif
			sp = place(sp, braslist[c - '1'], braelist[c - '1']);
			if (sp == 0)
				goto ovflo;
			continue;
		}
#ifdef	MB
		if (mb_cur_max > 1) {
			char	mb[MB_CUR_MAX+1];
			int	i, m;
			if (casecnt)
				c = fixcase(c & TRIM);
			if (c & INVBIT || (m = wctomb(mb, c)) <= 0) {
				*mb = rp[-n];
				m = 1;
			}
			for (i = 0; i < m; i++) {
				*sp++ = mb[i];
				if (sp >= &genbuf[LBSIZE])
					goto ovflo;
			}
		} else
#endif	/* MB */
		{
			if (casecnt)
				*sp++ = fixcase(c & TRIM);
			else
				*sp++ = c & TRIM;
		}
		if (sp >= &genbuf[LBSIZE])
ovflo:
			error(catgets(catd, 1, 130,
					"Line overflow@in substitute"));
	}
	lp = loc2;
	loc2 = sp + (linebuf - genbuf);
#ifdef	UXRE
	if (loc1 == lp) {
		nextc(c, loc2, n);
		loc2 += n;
	}
#endif	/* UXRE */
	while (*sp++ = *lp++)
		if (sp >= &genbuf[LBSIZE])
			goto ovflo;
	strcLIN(genbuf);
}

int
fixcase(register int c)
{

	if (casecnt == 0)
		return (c);
	casecnt--;
#ifdef	MB
	if (c & INVBIT)
		return (c);
	if (mb_cur_max > 1) {
		if (destuc) {
			if (iswlower(c))
				c = towupper(c);
		} else
			if (iswupper(c))
				c = towlower(c);
	} else
#endif	/* MB */
	{
		if (destuc) {
			if (islower(c))
				c = toupper(c);
		} else
			if (isupper(c))
				c = tolower(c);
	}
	return (c);
}

char *
place(register char *sp, register char *l1, register char *l2)
{
	while (l1 < l2) {
#ifdef	MB
		if (mb_cur_max > 1) {
			char	mb[MB_LEN_MAX+1];
			int	c, i, m, n;

			nextc(c, l1, m);
			if (c & INVBIT) {
				m = n = 1;
				*mb = *l1;
			} else {
				c = fixcase(c);
				if ((n = wctomb(mb, c)) <= 0) {
					n = 1;
					*mb = *l1;
				}
			}
			l1 += m;
			for (i = 0; i < n; i++) {
				*sp++ = mb[i];
				if (sp >= &genbuf[LBSIZE])
					return (0);
			}
		} else
#endif	/* MB */
		{
			*sp++ = fixcase(*l1++);
			if (sp >= &genbuf[LBSIZE])
				return (0);
		}
	}
	return (sp);
}

void
snote(register int total, register int lines)
{

	if (!notable(total))
		return;
	printf(mesg(catgets(catd, 1, 131, "%d subs|%d substitutions")), total);
	if (lines != 1 && lines != total)
		printf(catgets(catd, 1, 132, " on %d lines"), lines);
	noonl();
	flush();
}

void
cerror(char *s)
{
	expbuf[0] = '\0';
	error(s);
}

#ifdef	UXRE
void
refree(struct regexp *rp)
{
	struct regexp *r1 = NULL, *r2 = NULL;
	
	if (rp->Re_used == 0)
		return;
	if (rp == &re) {
		r1 = &scanre;
		r2 = &subre;
	} else if (rp == &scanre) {
		r1 = &re;
		r2 = &subre;
	} else if (rp == &subre) {
		r1 = &re;
		r2 = &scanre;
	}
	if ((r1->Re_used == 0 || rp->Re_ident != r1->Re_ident) &&
			(r2->Re_used == 0 || rp->Re_ident != r2->Re_ident))
		regfree(&rp->Re);
	rp->Re_used = 0;
}
#endif

struct regexp *
savere(struct regexp *store)
{
#ifdef	UXRE
	refree(store);
#endif
	copy(store, &re, sizeof re);
	return store;
}

struct regexp *
resre(struct regexp *store)
{
#ifdef	UXRE
	refree(&re);
#endif
	copy(&re, store, sizeof re);
	return store;
}

#ifdef	UXRE
int
compile(int eof, int oknl)
{
	int c, d, i, n;
	char	mb[MB_LEN_MAX+1];
	char *p = re.Expbuf, *end = re.Expbuf + sizeof re.Expbuf;
	int err = 0, nomagic = value(MAGIC) ? 0 : 1, esc;
	char *rhsp;
#ifdef	BIT8
	char *rhsq;
#endif

	if (isalpha(eof) || isdigit(eof))
		error(catgets(catd, 1, 133,
	"Regular expressions cannot be delimited by letters or digits"));
	c = GETWC(mb);
	if (eof == '\\') {
		switch (c) {
		case '/':
		case '?':
			if (scanre.Expbuf[0] == 0)
				error(catgets(catd, 1, 134,
	"No previous scan re|No previous scanning regular expression"));
			resre(&scanre);
			return c;
		case '&':
			if (subre.Expbuf[0] == 0)
				error(catgets(catd, 1, 135,
	"No previous substitute re|No previous substitute regular expression"));
			resre(&subre);
			return c;
		default:
			error(catgets(catd, 1, 136,
	"Badly formed re|Regular expression \\ must be followed by / or ?"));
		}
	}
	if (c == eof || c == '\n' || c == EOF) {
		if (c == '\n' && oknl == 0)
			error(catgets(catd, 1, 138,
			"Missing closing delimiter@for regular expression"));
		if (c != eof)
			ungetchar(c);
		if (re.Re_used == 0)
			error(catgets(catd, 1, 137,
			"No previous re|No previous regular expression"));
		return eof;
	}
	nbra = circfl = 0;
	if (c == '^')
		circfl++;
	esc = 0;
	goto havec;
	/*
	 * Fetch the search pattern. This is quite a mess since we have
	 * to handle nomagic and ~.
	 */
	for (;;) {
		esc = 0;
		c = GETWC(mb);
	havec:	if (c == eof || c == EOF) {
			if (c == EOF)
				ungetchar(c);
			break;
		} else if (c == '\n') {
			if (!oknl)
				cerror(catgets(catd, 1, 157,
	"Badly formed re|Missing closing delimiter for regular expression"));
			ungetchar(c);
			break;
		} else if (nomagic) {
			switch (c) {
			case '.':
			case '*':
			case '[':
			case '~':
				*p++ = '\\';
				esc = 1;
				break;
			case '\\':
				c = GETWC(mb);
				if (c != '.' && c != '*' && c != '[' &&
						c != '~') {
					*p++ = '\\';
					esc = 1;
				}
			}
		} else if (c == '\\') {
			c = GETWC(mb);
			if (c != '~')
				*p++ = '\\';
			esc = 1;
		}
		if (c == EOF) {
			ungetchar(c);
			break;
		}
		if (!esc && c == '~') {
			rhsp = rhsbuf;
#ifdef	BIT8
			rhsq = rhsquo;
#endif
			while (*rhsp) {
#ifndef	BIT8
				if (*rhsp & QUOTE) {
					nextc(c, rhsp, n);
					c &= TRIM;
#else	/* BIT8 */
				if (*rhsq) {
					nextc(c, rhsp, n);
#endif	/* BIT8 */
					if (c == '&')
						error(catgets(catd, 1, 149,
			"Replacement pattern contains &@- cannot use in re"));
					if (c >= '1' && c <= '9')
						error(catgets(catd, 1, 150,
			"Replacement pattern contains \\d@- cannot use in re"));
				}
				if (p >= end - 3)
					goto complex;
				if (*rhsp == '\\' || *rhsp == '[' ||
						*rhsp == '.' ||
						*rhsp == '^' ||
						*rhsp == '*' ||
						*rhsp == '$')
					*p++ = '\\';
#ifdef	BIT8
				nextc(c, rhsp, n);
				for (i = 0; i < n; i++) {
					*p++ = *rhsp++;
					rhsq++;
				}
#else
				*p++ = *rhsp++ & TRIM;
#endif
			}
		} else if (!esc && c == '[') {
			/*
			 * Search for the end of the bracket expression
			 * since '~' may not be recognized inside.
			 */
			*p++ = (char)c;
			if (p >= end)
				goto complex;
			d = EOF;
			do {
				c = GETWC(mb);
				if (c == '\n' || c == EOF)
					goto miss;
				for (i = 0; mb[i]; i++) {
					*p++ = mb[i];
					if (p >= end)
						goto complex;
				}
				if (d == '[' && (c == ':' || c == '.' ||
							c == '=')) {
					d = c;
					do {
						c = GETWC(mb);
						if (c == '\n' || c == EOF)
							goto miss;
						for (i = 0; mb[i]; i++) {
							*p++ = mb[i];
							if (p >= end)
								goto complex;
						}
					} while (c != d || peekchar() != ']');
					c = GETWC(mb);
					for (i = 0; mb[i]; i++) {
						*p++ = mb[i];
						if (p >= end)
							goto complex;
					}
					c = EOF; /* -> reset d and continue */
				}
				d = c;
			} while (c != ']');
		} else if (esc && c == '{') {
			/*
			 * Search for the end of the interval expression
			 * since '~' may not be recognized inside.
			 */
			for (i = 0; mb[i]; i++) {
				*p++ = mb[i];
				if (p >= end)
					goto complex;
			}
			do {
				c = GETWC(mb);
				if (c == '\n' || c == EOF)
					cerror(catgets(catd, 1, 143,
			"Bad number|Bad number in regular expression"));
				for (i = 0; mb[i]; i++) {
					*p++ = mb[i];
					if (p >= end)
						goto complex;
				}
			} while (c != '\\');
			c = GETWC(mb);
			if (c != '}')
				cerror(catgets(catd, 1, 146,
					"} expected after \\"));
			*p++ = (char)c;
		} else {
			for (i = 0; mb[i]; i++) {
				*p++ = mb[i];
				if (p >= end)
					goto complex;
			}
		}
		if (p >= end)
complex:		cerror(catgets(catd, 1, 139,
			"Re too complex|Regular expression too complicated"));
	}
	if (p == expbuf)
		*p++ = '.';	/* approximate historical behavior */
	*p = '\0';
	refree(&re);
	c = REG_ANGLES | REG_BADRANGE;
#ifndef	NO_BE_BACKSLASH
	c |= REG_BKTESCAPE;
#endif	/* !NO_BE_BACKSLASH */
	if (value(IGNORECASE))
		c |= REG_ICASE;
	if ((err = regcomp(&re.Re, re.Expbuf, c)) != 0) {
		switch (err) {
		case REG_EBRACK:
		miss:	cerror(catgets(catd, 1, 154, "Missing ]"));
		default:
			regerror(err, &re.Re, &re.Expbuf[1],
					sizeof re.Expbuf - 1);
			cerror(&re.Expbuf[1]);
		}
	}
	re.Re_used = 1;
	re.Re_ident++;
	if ((nbra = re.Re.re_nsub) > NBRA)
		nbra = NBRA;
	return eof;
}

int
execute(int gf, line *addr)
{
	char *p;
	int c;
	int eflags = 0, nsub;
	regmatch_t bralist[NBRA + 1];

	if (gf) {
		if (circfl)
			return 0;
		eflags |= REG_NOTBOL;
		p = loc2;
	} else {
		if (addr == zero)
			return 0;
		p = linebuf;
		getline(*addr);
	}
	/*
	 * Need subexpression matches only for substitute command,
	 * so don't fetch them otherwise (enables use of DFA).
	 */
	nsub = (re.Re_ident == subre.Re_ident ? NBRA : 0);
	switch (regexec(&re.Re, p, nsub + 1, bralist, eflags)) {
	case 0:
		break;
	case REG_NOMATCH:
		return 0;
	default:
		cerror(catgets(catd, 1, 139,
			"Re too complex|Regular expression too complicated"));
	}
	loc1 = p + bralist[0].rm_so;
	loc2 = p + bralist[0].rm_eo;
	for (c = 0; c < nsub; c++) {
		if (bralist[c + 1].rm_so != -1) {
			braslist[c] = p + bralist[c + 1].rm_so;
			braelist[c] = p + bralist[c + 1].rm_eo;
		} else
			braslist[c] = braelist[c] = NULL;
	}
	return 1;
}
#else	/* !UXRE */
#define	INSCHAR(c)	{ \
				if ((c) == '\n' || (c) == EOF) \
					cerror(catgets(catd, 1, 154, \
						"Missing ]")); \
				*ep++ = (c); \
				cclcnt++; \
				if (ep >= &expbuf[ESIZE]) \
					goto complex; \
			}

int
compile(int eof, int oknl)
{
	register int c;
	register char *ep;
#ifdef	BIT8
#ifndef	NO_BE_BACKSLASH
	bool haddash;
#endif	/* !NO_BE_BACKSLASH */
#endif	/* BIT8 */
	char *lastep = NULL;
	char bracket[NBRA], *bracketp, *rhsp;
#ifdef	BIT8
	char *rhsq;
#endif
	int cclcnt;
	int i, cflg, closed;

	if (isalpha(eof) || isdigit(eof))
		error(catgets(catd, 1, 133,
	"Regular expressions cannot be delimited by letters or digits"));
	ep = expbuf;
	c = getchar();
	if (eof == '\\')
		switch (c) {

		case '/':
		case '?':
			if (scanre.Expbuf[0] == 0)
				error(catgets(catd, 1, 134,
	"No previous scan re|No previous scanning regular expression"));
			resre(&scanre);
			return (c);

		case '&':
			if (subre.Expbuf[0] == 0)
				error(catgets(catd, 1, 135,
	"No previous substitute re|No previous substitute regular expression"));
			resre(&subre);
			return (c);

		default:
			error(catgets(catd, 1, 136,
	"Badly formed re|Regular expression \\ must be followed by / or ?"));
		}
	if (c == eof || c == '\n' || c == EOF) {
		if (*ep == 0)
			error(catgets(catd, 1, 137,
			"No previous re|No previous regular expression"));
		if (c == '\n' && oknl == 0)
			error(catgets(catd, 1, 138,
			"Missing closing delimiter@for regular expression"));
		if (c != eof)
			ungetchar(c);
		return (eof);
	}
	bracketp = bracket;
	nbra = 0;
	circfl = 0;
	closed = 0;
	if (c == '^') {
		c = getchar();
		circfl++;
	}
	ungetchar(c);
	for (;;) {
		if (ep >= &expbuf[ESIZE - 2])
complex:
			cerror(catgets(catd, 1, 139,
			"Re too complex|Regular expression too complicated"));
		c = getchar();
		if (c == eof || c == EOF) {
			if (bracketp != bracket)
				cerror(catgets(catd, 1, 140,
		"Unmatched \\(|More \\('s than \\)'s in regular expression"));
			*ep++ = CEOFC;
			if (c == EOF)
				ungetchar(c);
			return (eof);
		}
		if (value(MAGIC)) {
			if (c != '*' && (c != '\\' || peekchar() != '{') ||
					ep == expbuf) {
				lastep = ep;
			}
		} else
			if (c != '\\' || peekchar() != '*' || ep == expbuf) {
				lastep = ep;
			}
		switch (c) {

		case '\\':
			c = getchar();
			switch (c) {

			case '(':
				if (nbra >= NBRA)
					cerror(catgets(catd, 1, 141,
"Awash in \\('s!|Too many \\('d subexressions in a regular expression"));
				*bracketp++ = nbra;
				*ep++ = CBRA;
				*ep++ = nbra++;
				continue;

			case ')':
				if (bracketp <= bracket)
					cerror(catgets(catd, 1, 142,
		"Extra \\)|More \\)'s than \\('s in regular expression"));
				*ep++ = CKET;
				*ep++ = *--bracketp;
				closed++;
				continue;

			case '<':
				*ep++ = CBRC;
				continue;

			case '>':
				*ep++ = CLET;
				continue;
			case '{':
				if (lastep == (char *)0)
					goto defchar;
				*lastep |= RNGE;
				cflg = 0;
nlim:
				c = getchar();
				i = 0;
				do {
					if ('0' <= c && c <= '9')
						i = 10 * i + c - '0';
					else
						cerror(catgets(catd, 1, 143,
			"Bad number|Bad number in regular expression"));
				} while ((c = getchar()) != '\\' && c != ',');
				if (i > 255)
					cerror(catgets(catd, 1, 144,
"Range endpoint too large|Range endpoint too large in regular expression"));
				*ep++ = i;
				if (c == ',') {
					if (cflg++)
						cerror(catgets(catd, 1, 145,
				"More than 2 numbers given in \\{~\\}"));
					if ((c = getchar()) == '\\') {
						*ep++ = 255;
					} else {
						ungetchar(c);
						goto nlim;
					}
				}
				if (getchar() != '}')
					cerror(catgets(catd, 1, 146,
						"} expected after \\"));
				if (!cflg) {
					*ep++ = i;
				}
				else if ((ep[-1] & 0377) < (ep[-2] & 0377))
					cerror(catgets(catd, 1, 147,
				"First number exceeds second in \\{~\\}"));
				continue;
			default:
				if (c >= '1' && c <= '9') {
					if ((c -= '1') >= closed)
						cerror(catgets(catd, 1, 148,
						"\"\\digit\" out of range"));
					*ep++ = CBACK;
					*ep++ = c;
					continue;
				}
			}
			if (value(MAGIC) == 0)
magic:
			switch (c) {

			case '.':
				*ep++ = CDOT;
				continue;

			case '~':
				rhsp = rhsbuf;
#ifdef	BIT8
				rhsq = rhsquo;
#endif
				while (*rhsp) {
#ifndef	BIT8
					if (*rhsp & QUOTE) {
						c = *rhsp & TRIM;
#else
					if (*rhsq) {
						c = *rhsp;
#endif
						if (c == '&')
							error(catgets(catd, 1,
		149, "Replacement pattern contains &@- cannot use in re"));
						if (c >= '1' && c <= '9')
							error(catgets(catd, 1,
		150, "Replacement pattern contains \\d@- cannot use in re"));
					}
					if (ep >= &expbuf[ESIZE-2])
						goto complex;
					*ep++ = CCHR;
#ifndef	BIT8
					*ep++ = *rhsp++ & TRIM;
#else
					*ep++ = *rhsp++;
					rhsq++;
#endif
				}
				continue;

			case '*':
				if (ep == expbuf)
					break;
				if (*lastep == CBRA || *lastep == CKET)
					cerror(catgets(catd, 1, 151,
		"Illegal *|Can't * a \\( ... \\) in regular expression"));
#ifndef	BIT8
				if (*lastep == CCHR && (lastep[1] & QUOTE))
					cerror(catgets(catd, 1, 152,
			"Illegal *|Can't * a \\n in regular expression"));
#endif
				*lastep |= STAR;
				continue;

			case '[':
				*ep++ = CCL;
				*ep++ = 0;
#ifdef	BIT8
#ifndef	NO_BE_BACKSLASH
				haddash = 0;
#endif	/* !NO_BE_BACKSLASH */
#endif	/* BIT8 */
				cclcnt = 1;
				c = getchar();
				if (c == '^') {
					c = getchar();
					ep[-2] = NCCL;
				}
#ifndef	NO_BE_BACKSLASH
				if (c == ']')
					cerror(catgets(catd, 1, 153,
"Bad character class|Empty character class '[]' or '[^]' cannot match"));
				while (c != ']') {
					if (c == '\\' && any(peekchar(), "]-^\\")) {
#ifndef	BIT8
						c = getchar() | QUOTE;
#else	/* BIT8 */
						if ((c = getchar()) == '-') {
							haddash = 1;
							c = getchar();
						}
#endif	/* BIT8 */
					}
					INSCHAR(c)
					c = getchar();
				}
#ifdef	BIT8
				if (haddash)
					INSCHAR('-')
#endif	/* BIT8 */
#else	/* NO_BE_BACKSLASH */
				/*
				 * There is no escape character inside a
				 * bracket expression. Characters lose their
				 * special meaning by position only.
				 */
				do
					INSCHAR(c)
				while ((c = getchar()) != ']');
#endif	/* NO_BE_BACKSLASH */
				lastep[1] = cclcnt;
				continue;
			}
			if (c == EOF) {
				ungetchar(EOF);
				c = '\\';
				goto defchar;
			}
			*ep++ = CCHR;
			if (c == '\n')
				cerror(catgets(catd, 1, 155,
	"No newlines in re's|Can't escape newlines into regular expressions"));
/*
			if (c < '1' || c > NBRA + '1') {
*/
				*ep++ = c;
				continue;
/*
			}
			c -= '1';
			if (c >= nbra)
				cerror(catgets(catd, 1, 156,
"Bad \\n|\\n in regular expression with n greater than the number of \\('s"));
			*ep++ = c | QUOTE;
			continue;
*/

		case '\n':
			if (oknl) {
				ungetchar(c);
				*ep++ = CEOFC;
				return (eof);
			}
			cerror(catgets(catd, 1, 157,
	"Badly formed re|Missing closing delimiter for regular expression"));

		case '$':
			if (peekchar() == eof || peekchar() == EOF || oknl && peekchar() == '\n') {
				*ep++ = CDOL;
				continue;
			}
			goto defchar;

		case '.':
		case '~':
		case '*':
		case '[':
			if (value(MAGIC))
				goto magic;
defchar:
		default:
			*ep++ = CCHR;
			*ep++ = c;
			continue;
		}
	}
}

int
same(register int a, register int b)
{

	return (a == b || value(IGNORECASE) &&
	   ((islower(a) && toupper(a) == b) || (islower(b) && toupper(b) == a)));
}

int
ecmp(register char *a, register char *b, register int count)
{
	while (count--)
		if (!same(*a++, *b++))
			return (0);
	return (1);
}

char	*locs;

int
execute(int gf, line *addr)
{
	register char *p1, *p2;
	register int c;

	if (gf) {
		if (circfl)
			return (0);
		locs = p1 = loc2;
	} else {
		if (addr == zero)
			return (0);
		p1 = linebuf;
		getline(*addr);
		locs = 0;
	}
	p2 = expbuf;
	if (circfl) {
		loc1 = p1;
		return (advance(p1, p2));
	}
	/* fast check for first character */
	if (*p2 == CCHR) {
		c = p2[1];
		do {
			if (c != *p1 && (!value(IGNORECASE) ||
			   !((islower(c) && toupper(c) == *p1) ||
			   (islower(*p1&0377) && toupper(*p1&0377) == c))))
				continue;
			if (advance(p1, p2)) {
				loc1 = p1;
				return (1);
			}
		} while (*p1++);
		return (0);
	}
	/* regular algorithm */
	do {
		if (advance(p1, p2)) {
			loc1 = p1;
			return (1);
		}
	} while (*p1++);
	return (0);
}

void
getrnge(register char *str)
{
	low = *str++ & 0377;
	siz = (*str & 0377) == 255 ? 20000 : (*str & 0377) - low;
}

#define	uletter(c)	(isalpha(c) || c == '_')

int
advance(register char *lp, register char *ep)
{
	register char *curlp;
	/* char *sp, *sp1; */
	int c, ct;
	char *bbeg;

	for (;;) switch (*ep++) {

	case CCHR:
/* useless
		if (*ep & QUOTE) {
			c = *ep++ & TRIM;
			sp = braslist[c];
			sp1 = braelist[c];
			while (sp < sp1) {
				if (!same(*sp, *lp))
					return (0);
				sp++, lp++;
			}
			continue;
		}
*/
		if (!same(*ep, *lp))
			return (0);
		ep++, lp++;
		continue;

	case CDOT:
		if (*lp++)
			continue;
		return (0);

	case CDOL:
		if (*lp == 0)
			continue;
		return (0);

	case CEOFC:
		loc2 = lp;
		return (1);

	case CCL:
		if (cclass(ep, *lp++, 1)) {
			ep += *ep;
			continue;
		}
		return (0);

	case NCCL:
		if (cclass(ep, *lp++, 0)) {
			ep += *ep;
			continue;
		}
		return (0);

	case CBRA:
		braslist[(int)*ep++] = lp;
		continue;

	case CKET:
		braelist[(int)*ep++] = lp;
		continue;

	case CCHR|RNGE:
		c = *ep++;
		getrnge(ep);
		while (low--)
			if (!same(*lp++, c))
				return (0);
		curlp = lp;
		while (siz--)
			if (!same(*lp++, c))
				break;
		if (siz < 0)
			lp++;
		ep += 2;
		goto star;

	case CDOT|RNGE:
		getrnge(ep);
		while (low--)
			if (*lp++ == '\0')
				return (0);
		curlp = lp;
		while (siz--)
			if (*lp++ == '\0')
				break;
		if (siz < 0)
			lp++;
		ep += 2;
		goto star;

	case CCL|RNGE:
	case NCCL|RNGE:
		getrnge(ep + *ep);
		while (low--) {
			if (!cclass(ep, *lp++, ep[-1] == (CCL|RNGE)))
				return (0);
		}
		curlp = lp;
		while (siz--) {
			if (!cclass(ep, *lp++, ep[-1] == (CCL|RNGE)))
				break;
		}
		if (siz < 0)
			lp++;
		ep += *ep + 2;
		goto star;

	case CBACK:
		bbeg = braslist[*ep & 0377];
		ct = braelist[*ep++ & 0377] - bbeg;
		if (ecmp(bbeg, lp, ct)) {
			lp += ct;
			continue;
		}
		return (0);

	case CBACK|STAR:
		bbeg = braslist[*ep & 0377];
		ct = braelist[*ep++ & 0377] - bbeg;
		curlp = lp;
		while (ecmp(bbeg, lp, ct))
			lp += ct;
		while (lp >= curlp) {
			if (advance(lp, ep))
				return (1);
			lp -= ct;
		}
		return (0);

	case CDOT|STAR:
		curlp = lp;
		while (*lp++)
			continue;
		goto star;

	case CCHR|STAR:
		curlp = lp;
		while (same(*lp, *ep))
			lp++;
		lp++;
		ep++;
		goto star;

	case CCL|STAR:
	case NCCL|STAR:
		curlp = lp;
		while (cclass(ep, *lp++, ep[-1] == (CCL|STAR)))
			continue;
		ep += *ep;
		goto star;
star:
		do {
			lp--;
			if (lp == locs)
				break;
			if (advance(lp, ep))
				return (1);
		} while (lp > curlp);
		return (0);

	case CBRC:
		if (lp == linebuf)
			continue;
		if ((isdigit(*lp&0377) || uletter(*lp&0377))
			&& !uletter(lp[-1]&0377) && !isdigit(lp[-1]&0377))
			continue;
		return (0);

	case CLET:
		if (!uletter(*lp&0377) && !isdigit(*lp&0377))
			continue;
		return (0);

	default:
		error(catgets(catd, 1, 158, "Re internal error"));
	}
}

int
cclass(register char *set, register int c, int af)
{
	register int n;

	if (c == 0)
		return (0);
	if (value(IGNORECASE) && isupper(c))
		c = tolower(c);
	n = *set++;
	while (--n)
		if (n > 2 && set[1] == '-') {
			if (c >= (set[0] & TRIM) && c <= (set[2] & TRIM))
				return (af);
			set += 3;
			n -= 2;
		} else
			if ((*set++ & TRIM) == c)
				return (af);
	return (!af);
}
#endif	/* !UXRE */
