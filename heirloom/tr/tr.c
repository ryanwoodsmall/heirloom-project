/*
 * tr - translate characters
 *
 * Gunnar Ritter, Freiburg i. Br., Germany, December 2002.
 */
/*
 * Copyright (c) 2003 Gunnar Ritter
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute
 * it freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 *
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source distribution.
 */

#if __GNUC__ >= 3 && __GNUC_MINOR__ >= 4
#define	USED	__attribute__ ((used))
#elif defined __GNUC__
#define	USED	__attribute__ ((unused))
#else
#define	USED
#endif
#if defined (SUS)
static const char sccsid[] USED = "@(#)tr_sus.sl	1.28 (gritter) 1/7/05";
#elif defined (UCB)
static const char sccsid[] USED = "@(#)/usr/ucb/tr.sl	1.28 (gritter) 1/7/05";
#else
static const char sccsid[] USED = "@(#)tr.sl	1.28 (gritter) 1/7/05";
#endif

#include	<unistd.h>
#include	<stdio.h>
#include	<string.h>
#include	<stdlib.h>
#include	<errno.h>
#include	<libgen.h>
#include	<locale.h>
#include	<wchar.h>
#include	<wctype.h>
#include	<ctype.h>
#include	<limits.h>
#include	<inttypes.h>

#include	"iblok.h"
#include	"asciitype.h"

#ifdef	UCB
#define	AUTOREP	1
#else
#define	AUTOREP	0
#endif

#if defined (__GLIBC__) && defined (_IO_putc_unlocked)
#undef	putchar
#define	putchar(c)	_IO_putc_unlocked(c, stdout)
#endif

#define	BORDER0	0xFFL		/* single-byte limit */
#define	BORDER1	0xFFFFL		/* Unicode BMP limit */
#define	BORDER2	0x10FFFFL	/* Unicode limit */

#define	HBITS0	8		/* singlebyte hash table bits */
#define	HBITS1	14		/* multibyte hash table bits */

#define	OOM	((struct item *)-1)	/* out of memory indicator */

struct	elem {			/* parsed elements and :class: lists */
	struct elem	*e_nxt;
	enum {
		E_CHAR,		/* character in e_chr */
		E_UPPER,	/* :upper: in e_class */
		E_LOWER,	/* :lower: in e_class */
		E_CLASS,	/* other character class in e_class */
	}		e_type;
	union {
		wctype_t	e_class;
		wint_t		e_chr;
	}		e_data;
};

struct	item {			/* hash table item */
	struct item	*i_nxt;
	wint_t		i_in;	/* untranslated character */
	wint_t		i_out;	/* translated character (if any) */
};

#define	HBITS		14

struct	store {			/* check for presence of characters */
	struct item	**s_htab;	/* hash table with wide characters */
	char		s_ctab[256];	/* singlebyte characters */
	struct elem	*s_chain;	/* list of :class: elements */
	int		s_compl;	/* complement values */
};

struct	trans {			/* translate characters */
	struct item	**t_htab;	/* hash table with wide characters */
	char		t_ctab[256];	/* singlebyte characters */
	struct elem	*t_chain;	/* list of :class: elements */
};

struct	arg {			/* holds argument while parsing */
	const char	*a_arg;		/* argument string */
	struct store	*a_htab;	/* complement tab */
	long		a_len;		/* length of a_arg */
	unsigned long	a_rep;		/* repetition count */
	wctype_t	a_class;	/* char class (singlebyte mode) */
	int		a_needrep;	/* need repetition counts */
	wint_t		a_max;		/* range maximum */
	wint_t		a_char;		/* multipurpose char */
	wint_t		a_lastc;	/* last character returned */
};

static unsigned		errcnt;		/* count of errors */
static int		cflag;		/* complement flag */
static int		dflag;		/* delete flag */
static int		sflag;		/* squeeze flag */
static char		*progname;	/* argv[0] to main() */
static struct trans	*transtab;	/* translation table */
static struct store	*deltab;	/* deletion table */
static struct store	*squeeztab;	/* squeez table */
static int		multibyte;	/* multibyte LC_CTYPE */
static unsigned long	borderc;	/* highest fully supported character */
static int		hbits;		/* size of hash tables is 1<<hbits */
static struct iblok	*input;		/* input buffer structure */
static void		*reserve;	/* reserver for OOM condition */

#define	peek(wc, s)	(multibyte ? mbtowc(&(wc), s, MB_LEN_MAX) :\
				((wc) = *(s) & 0377, (wc) != 0))

void			 *scalloc(size_t nmemb, size_t size);
void			 *srealloc(void *vp, size_t nbytes);
void			 *smalloc(size_t nbytes);

static struct item	*lookup(wint_t wc, struct item **it, int make);
static void		 bad(void);
static wint_t		 dotrans(wint_t wc, struct trans *tp);
static struct		 trans *mktrans(const char *s1, const char *s2,
				int compl);
static int		 have(wint_t wc, struct store *sp);
static struct store	 *mkstore(const char *s, int compl);
static char		 *strip(const char *s, long *sz);
static struct arg	 *mkarg(const char *str, int compl, int needrep);
static struct elem	 *next(struct arg *ap, struct elem **resp, int autorep);
static int		 tr_sb(void);
static int		 tr_mb(void);

void *
scalloc(size_t nmemb, size_t size)
{
	void	*p;

	if ((p = calloc(nmemb, size)) == NULL) {
		write(2, "no memory\n", 10);
		exit(077);
	}
	return p;
}

void *
srealloc(void *vp, size_t nbytes)
{
	void	*p;

	if ((p = (void *)realloc(vp, nbytes)) == NULL) {
		write(2, "no memory\n", 10);
		exit(077);
	}
	return p;
}

void *
smalloc(size_t nbytes)
{
	return srealloc(NULL, nbytes);
}

#define	hash(c)	((uint32_t)(2654435769U * (uint32_t)(c) >> (32-hbits)))

static struct item *
lookup(wint_t wc, struct item **it, int make)
{
	struct item	*ip;
	int	h;

	ip = it[h = hash(wc)];
	while (ip != NULL) {
		if (ip->i_in == wc)
			break;
		ip = ip->i_nxt;
	}
	if (make && ip == NULL) {
		if ((ip = calloc(1, sizeof *ip)) == NULL) {
			if (borderc == BORDER2 && reserve) {
				/*
				 * Not enough memory to cover all of Unicode.
				 * Fall back to BMP limit. Reserve memory has
				 * been allocated previously and must now be
				 * free'd; this is necessary because ib_getw()
				 * will need some memory left.
				 */
				free(reserve);
				borderc = BORDER1;
				return OOM;
			}
			write(2, "no memory\n", 10);
			exit(077);
		}
		ip->i_in = wc;
		ip->i_nxt = it[h];
		it[h] = ip;
	}
	return ip;
}

#ifdef	notdef
static void
usage(int c)
{
	if (c)
		fprintf(stderr, "%s: illegal option -- %c\n", progname, c);
	fprintf(stderr, "\
Usage:\n\
\t%s [-cs] string1 string2\n\
\t%s -s[-c] string1\n\
\t%s -d[-c] string1\n\
\t%s -ds[-c] string1 string2\n",
		progname, progname, progname, progname);
	exit(2);
}
#endif	/* notdef */
#define	usage(c)

static void
bad(void)
{
	fprintf(stderr, "Bad string\n");
	exit(2);
}

static wint_t
dotrans(wint_t wc, struct trans *tp)
{
	struct item	*ip;
	struct elem	*ep;

	if ((ip = lookup(wc, tp->t_htab, 0)) != NULL)
		return ip->i_out;
	if (ip == OOM)
		return wc;
	for (ep = tp->t_chain; ep; ep = ep->e_nxt) {
		switch (ep->e_type) {
		case E_UPPER:
			if (wc & ~(wint_t)0177) {
				if (iswlower(wc))
					return towupper(wc);
			} else {
				if (islower(wc))
					return toupper(wc);
			}
			break;
		case E_LOWER:
			if (wc & ~(wint_t)0177) {
				if (iswupper(wc))
					return towlower(wc);
			} else {
				if (isupper(wc))
					return tolower(wc);
			}
			break;
		case E_CHAR:
		case E_CLASS:
			/* handled in hash table */
			break;
		}
	}
	return wc;
}

static struct trans *
mktrans(const char *s1, const char *s2, int compl)
{
	struct trans	*tp;
	struct item	*ip;
	struct arg	*a1, *a2;
	struct elem	*e1, *e2;
	struct elem	*r1 = NULL, *r2 = NULL;

	if (s1 == NULL || s2 == NULL)
		return NULL;
	a1 = mkarg(s1, compl, 1);
	a2 = mkarg(s2, 0, 2);
	tp = scalloc(1, sizeof *tp);
	tp->t_htab = scalloc(1<<hbits, sizeof *tp->t_htab);
	if (!multibyte) {
		int	c;

		for (c = 0; c <= 255; c++)
			tp->t_ctab[c] = (char)c;
	}
	while ((e1 = next(a1, &r1,0 )) != NULL &&
			(e2 = next(a2, &r2, AUTOREP)) != NULL) {
	retry:	if (e1->e_type == E_CHAR && e2->e_type == E_CHAR) {
			if (!multibyte)
				tp->t_ctab[e1->e_data.e_chr] =
					(char)e2->e_data.e_chr;
			ip = lookup(e1->e_data.e_chr, tp->t_htab, 1);
			if (ip != OOM)
				ip->i_out = e2->e_data.e_chr;
			free(e1);
			free(e2);
		} else if ((e1->e_type == E_UPPER && e2->e_type == E_LOWER) ||
		    (e1->e_type == E_LOWER && e2->e_type == E_UPPER)) {
			if (!multibyte) {
				int	c, d;

				for (c = 0; c <= 255; c++) {
					d = c;
					switch (e2->e_type) {
					case E_LOWER:
						if (isupper(c))
							d = tolower(c);
						break;
					case E_UPPER:
						if (islower(c))
							d = toupper(c);
						break;
					default:
						/* make compiler happy */
						break;
					}
					tp->t_ctab[c] = (char)d;
				}
			}
			e2->e_nxt = tp->t_chain;
			tp->t_chain = e2;
			free(e1);
		} else {	/* have to get every single member of :class: */
			if (e1->e_type != E_CHAR) {
				r1 = e1;
				if ((e1 = next(a1, &r1, 0)) == NULL)
					break;
			}
			if (e2->e_type != E_CHAR) {
				r2 = e2;
				if ((e2 = next(a2, &r2, 1)) == NULL)
					break;
			}
			goto retry;
		}
	}
	return tp;
}

static int
have(wint_t wc, struct store *sp)
{
	int	yes;
	struct elem	*ep;
	struct item	*ip;

	if ((yes = ((ip = lookup(wc, sp->s_htab, 0)) != NULL &&
			ip != OOM)) == 0) {
		for (ep = sp->s_chain; ep && yes == 0; ep = ep->e_nxt) {
			switch (ep->e_type) {
			default:
				yes = iswctype(wc, ep->e_data.e_class) != 0;
				break;
			case E_CHAR:
				/* handled in hash table */
				break;
			}
		}
	}
	return yes ^ sp->s_compl;
}

static struct store *
mkstore(const char *s, int compl)
{
	struct store	*sp;
	struct arg	*ap;
	struct elem	*ep;

	if (s == NULL)
		return NULL;
	sp = scalloc(1, sizeof *sp);
	sp->s_htab = scalloc(1<<hbits, sizeof *sp->s_htab);
	sp->s_compl = compl;
	if (compl && !multibyte) {
		int	c;

		for (c = 0; c <= 255; c++)
			sp->s_ctab[c] = 1;
	}
	ap = mkarg(s, 0, 0);
	while ((ep = next(ap, NULL, 0)) != NULL) {
		switch (ep->e_type) {
		case E_CHAR:
			if (!multibyte)
				sp->s_ctab[ep->e_data.e_chr] = compl == 0;
			lookup(ep->e_data.e_chr, sp->s_htab, 1);
			free(ep);
			break;
		default:
			if (!multibyte) {
				int	c;
				wint_t	wc;

				for (c = 1; c <= 255; c++) {
					if ((wc = btowc(c)) != WEOF &&
					    iswctype(wc, ep->e_data.e_class))
						sp->s_ctab[c] = compl == 0;
				}
			}
			ep->e_nxt = sp->s_chain;
			sp->s_chain = ep;
		}
	}
	return sp;
}

static char *
strip(const char *s, long *sz)
{
	char	*p, *cp;
	int	n;
	wchar_t	wc;

	cp = p = smalloc(strlen(s) + 1);
	while ((n = peek(wc, s)) > 0) {
		if (wc == '\\') {
			s += n;
			if ((n = peek(wc, s)) <= 0)
				break;
			if (digitchar(*s & 0377)) {
				char	seq[4];
				int	cnt = 0;
				long	val;

				do
					seq[cnt++] = *s++;
				while (cnt < 3 && octalchar(*s & 0377));
				seq[cnt] = '\0';
				val = strtol(seq, NULL, 8);
#ifdef	SUS
				if (val < 0 || val > 0377)
					bad();
#endif
				/*
				 * Only quote the result if it is ASCII,
				 * as multiple octal sequences may refer
				 * to one multibyte character.
				 */
				if ((((char)val)&0200) == 0)
					*p++ = '\\';
				*p++ = (char)val;
			} else {
				*p++ = '\\';
				switch (wc) {
				case 'a':
					*p++ = '\a';
					break;
				case 'b':
					*p++ = '\b';
					break;
				case 'f':
					*p++ = '\f';
					break;
				case 'n':
					*p++ = '\n';
					break;
				case 'r':
					*p++ = '\r';
					break;
				case 't':
					*p++ = '\t';
					break;
				case 'v':
					*p++ = '\v';
					break;
				default:
					goto norm;
				}
				s += n;
			}
		} else {
		norm:	while (n--)
				*p++ = *s++;
		}
	}
	if (n < 0)
		bad();
	*p = '\0';
	*sz = p - cp;
	return cp;
}

static struct arg *
mkarg(const char *str, int compl, int needrep)
{
	struct arg	*ap;

	ap = scalloc(1, sizeof *ap);
	ap->a_arg = strip(str, &ap->a_len);
	ap->a_char = 0;
	ap->a_lastc = WEOF;
	ap->a_max = WEOF;
	ap->a_needrep = needrep;
	if (compl)
		ap->a_htab = mkstore(str, 0);
	else
		ap->a_htab = NULL;
	return ap;
}

#if defined (SUS) || defined (UCB)
#define	invbracket()	goto rest
#else
#define	invbracket()	bad()
#endif

#define	fetch(wc, ap, n)	if (((n) = peek((wc), (ap)->a_arg)) < 0) \
					return NULL; \
				if ((n) == 0) (n) = 1; \
				(ap)->a_arg += (n), (ap)->a_len -= (n)

static struct elem *
next(struct arg *ap, struct elem **resp, int autorep)
{
	struct elem	*ep;

	ep = scalloc(1, sizeof *ep);
	if (resp && *resp) {
		/*
		 * Resolve a character class expression.
		 */
		if (multibyte) {
			char	mb[MB_LEN_MAX];

			do
				ap->a_char++;
			while (ap->a_char && ap->a_char <= borderc &&
				(wctomb(mb, ap->a_char) == -1 ||
				!iswctype(ap->a_char,(*resp)->e_data.e_class)));
			if (ap->a_char == 0 || ap->a_char > borderc) {
				*resp = NULL;
				goto cont;
			}
		} else {
			wint_t	wc;

			do
				ap->a_char++;
			while (ap->a_char && ap->a_char <= 255 &&
				((wc = btowc(ap->a_char)) == WEOF ||
				 !iswctype(wc, (*resp)->e_data.e_class)));
			if (ap->a_char == 0 || ap->a_char > 255) {
				*resp = NULL;
				goto cont;
			}
		}
		ep->e_type = E_CHAR;
		ep->e_data.e_chr = ap->a_char;
		ap->a_lastc = ap->a_char;
		return ep;
	}
cont:	if (ap->a_htab) {
		char	mb[MB_LEN_MAX];

		do
			ap->a_char++;
		while (ap->a_char && ap->a_char <= borderc &&
			((multibyte ? wctomb(mb,ap->a_char) == -1 : 0) ||
				have(ap->a_char, ap->a_htab) != 0));
		if (ap->a_char == 0 || ap->a_char > borderc)
			return NULL;
		ep->e_type = E_CHAR;
		ep->e_data.e_chr = ap->a_char;
		ap->a_lastc = ap->a_char;
	} else {
		int	n;
		int	base;
		char	*cp;
		wchar_t	wc, wd;

		if (ap->a_max != WEOF) {
		rnge:	if (ap->a_char <= ap->a_max) {
				ap->a_lastc = ap->a_char;
				ep->e_type = E_CHAR;
				ep->e_data.e_chr = ap->a_char++;
				return ep;
			}
			ap->a_max = WEOF;
		}
		if (ap->a_rep > 0) {
		rep:	ap->a_rep--;
			if (ap->a_needrep == 0)
				ap->a_rep = 0;
			ep->e_type = E_CHAR;
			ep->e_data.e_chr = ap->a_char;
			return ep;
		}
		if (ap->a_len <= 0) {
#ifdef	UCB
			if (autorep && ap->a_lastc != WEOF) {
				ap->a_char = ap->a_lastc;
				ap->a_lastc = WEOF;
				ap->a_rep = ap->a_needrep > 1 ?
					ULONG_MAX : 1000;
				goto rep;
			}
#endif	/* UCB */
			return NULL;
		}
		fetch(wc, ap, n);
		if (wc == '[') {
#if defined (SUS) || defined (UCB)
			const char	*oarg = ap->a_arg;
			long	olen = ap->a_len;
#endif
			fetch(wc, ap, n);
			if (ap->a_len <= 0)
				invbracket();
			if (wc == '=') {
				if ((n = peek(wd, ap->a_arg)) < 0)
					bad();
				if (n == 0)
					n = 1;
				if (wd == '\\') {
					int	m;

					if ((m = peek(wd, &ap->a_arg[n])) < 0)
						bad();
					if (m == 0)
						m = 1;
					n += m;
				}
				/*
				 * Since there is no way to determine
				 * whether something is a collating symbol
				 * in the current locale, we accept exactly
				 * one character that matches only itself.
				 */
				if (ap->a_arg[n] != '=' ||
						ap->a_arg[n+1] != ']')
					goto other;
				n += 2;
				ap->a_arg += n, ap->a_len -= n;
				ep->e_type = E_CHAR;
				ep->e_data.e_chr = wd;
				ap->a_lastc = wd;
				return ep;
			} else if (wc == ':') {
				long	sz;
				char	*class;

				sz = ap->a_len;
				cp = (char *)ap->a_arg;
				if (!alphachar(*cp & 0377))
					goto other;
				while (sz--) {
					if (*cp == ':')
						break;
					if (!alnumchar(*cp & 0377))
						goto other;
					cp++;
				}
				if (sz == 0 || cp[1] != ']')
					goto other;
				ap->a_len -= cp - ap->a_arg + 2;
				cp = class = smalloc(cp - ap->a_arg + 1);
				while (*ap->a_arg != ':')
					*cp++ = *ap->a_arg++;
				*cp = '\0';
				ap->a_arg += 2;
				if (strcmp(class, "upper") == 0)
					ep->e_type = E_UPPER;
				else if (strcmp(class, "lower") == 0)
					ep->e_type = E_LOWER;
				else
					ep->e_type = E_CLASS;
				if ((ep->e_data.e_class = wctype(class)) == 0)
					bad();
				free(class);
				ap->a_char = 0;
				return ep;
			} else if (wc == '\\') {
				fetch(wc, ap, n);
				if (ap->a_len <= 0)
					invbracket();
			}
		other:	fetch(wd, ap, n);
			if (ap->a_len <= 0)
				invbracket();
			switch (wd) {
#if !defined (SUS) && !defined (UCB)
			default:
				bad();
			case '-':
				ap->a_char = wc;
				fetch(wc, ap, n);
				if (ap->a_len <= 0)
					bad();
				if (wc == '\\') {
					fetch(wc, ap, n);
					if (ap->a_len <= 0)
						bad();
				}
				fetch(wd, ap, n);
				if (wd != ']')
					bad();
				ap->a_max = wc;
				if (ap->a_max < ap->a_char)
					bad();
				goto rnge;
#else	/* SUS || UCB */
			default:
			rest:	ap->a_arg = oarg;
				ap->a_len = olen;
				wc = '[';
				break;
#endif	/* SUS || UCB */
			case '*':
				ap->a_char = wc;
				ap->a_lastc = wc;
				if ((n = peek(wc, ap->a_arg)) <= 0)
					bad();
				if (n == 0) n = 1;
				if (wc == '*') {
					ap->a_arg += n, ap->a_len -= n;
					ap->a_rep = ap->a_needrep > 1 ?
						ULONG_MAX : 1000;
					goto rep;
				} else if (wc == '0') {
					ap->a_arg += n, ap->a_len -= n;
					if (ap->a_len <= 0)
						bad();
					base = 8;
				} else
					base = 10;
				if (*ap->a_arg == '+' || *ap->a_arg == '-')
					bad();
				ap->a_rep = strtoul(ap->a_arg,&cp,base);
				if (ap->a_rep == 0)
					ap->a_rep = ap->a_needrep > 1 ?
						ULONG_MAX : 1000;
				if ((ap->a_len -= cp - ap->a_arg) <= 0)
					bad();
				ap->a_arg = cp;
				fetch(wd, ap, n);
				if (wd != ']')
					bad();
				goto rep;
			}
		} else if (wc == '\\') {
			fetch(wc, ap, n);
		}
#if defined (SUS) || defined (UCB)
		if ((n = peek(wd, ap->a_arg)) < 0)
			bad();
		if (n == 0) n = 1;
		if (wd == '-' && ap->a_len > n) {
			ap->a_arg += n, ap->a_len -= n;
			ap->a_char = wc;
			fetch(wc, ap, n);
			if (wc == '\\' && ap->a_len > 0) {
				fetch(wc, ap, n);
			}
			ap->a_max = wc;
			if (ap->a_max < ap->a_char)
				bad();
			goto rnge;
		}
#endif	/* SUS || UCB */
		ep->e_type = E_CHAR;
		ep->e_data.e_chr = wc;
		ap->a_lastc = wc;
	}
	return ep;
}

static int
tr_sb(void)
{
	int	c, lastc = EOF;

	while ((c = ib_get(input)) != EOF) {
#ifndef	SUS
		if (c == '\0')
			continue;
#endif	/* !SUS */
		if (transtab) {
			c = transtab->t_ctab[c];
#ifndef	SUS
			if (c == '\0')
				continue;
#endif	/* !SUS */
		} else if (deltab && deltab->s_ctab[c])
			continue;
		if (squeeztab && c == lastc && squeeztab->s_ctab[c])
			continue;
		lastc = c;
		putchar(c);
	}
	return 0;
}

static int
tr_mb(void)
{
	char	*cp, *op;
	wint_t	wc, wd, lastwc = WEOF;
	char	mb[MB_LEN_MAX];
	int	n, i;

	while ((cp = ib_getw(input, &wc, &n)) != NULL) {
		if (wc != WEOF) {
#ifndef	SUS
			if (wc == '\0')
				continue;
#endif	/* !SUS */
			if (transtab) {
				wc = dotrans(wd = wc, transtab);
				if (wc == '\0') {
#ifdef	SUS
					op = "";
					i = 1;
#else	/* !SUS */
					continue;
#endif	/* !SUS */
				} else if (wc != wd && (i = wctomb(mb, wc)) > 0)
					op = mb;
				else {
					op = cp;
					i = n;
				}
			} else if (deltab && have(wc, deltab))
				continue;
			else {
				i = n;
				op = cp;
			}
			if (squeeztab && wc == lastwc && have(wc, squeeztab))
				continue;
			lastwc = wc;
		} else {	/* invalid character */
			if (cflag && dflag)
				continue;
			op = cp;
			i = n;
		}
		while (i--) {
			putchar(*op);
			op++;
		}
	}
	return 0;
}

int
main(int argc, char **argv)
{
	const char	optstring[] = ":cds";
	char	*string1, *string2;
	int	i;

#ifdef	__GLIBC__
	putenv("POSIXLY_CORRECT=1");
#endif
	progname = basename(argv[0]);
	/*setlocale(LC_COLLATE, "");*/
	setlocale(LC_CTYPE, "");
	multibyte = MB_CUR_MAX > 1;
	borderc = multibyte ? BORDER2 : BORDER0;
	hbits = multibyte ? HBITS1 : HBITS0;
	if (multibyte)
		reserve = smalloc(0xFFFF);
	while ((i = getopt(argc, argv, optstring)) != EOF) {
		switch (i) {
		case 'c':
			cflag = 1;
			break;
		case 'd':
			dflag = 1;
			break;
		case 's':
			sflag = 1;
			break;
		default:
			usage(optopt);
		}
	}
	if (argv[optind]) {
		string1 = argv[optind];
		if (argv[optind + 1]) {
			string2 = argv[optind + 1];
			if (argv[optind + 2])
				usage(0);
		} else
			string2 = NULL;
	} else {
		usage(0);
		string1 = string2 = NULL;
	}
	if (dflag) {
		if (string1 == NULL)
			usage(0);
		deltab = mkstore(string1, cflag);
		if (sflag) {
			if (string2 == NULL)
				usage(0);
			squeeztab = mkstore(string2, 0);
		} else {
			if (string2 != NULL)
				usage(0);
			(void)mkstore(string2, 0);
		}
	} else {
		if (string2) {
			if (string1 == NULL)
				usage(0);
			transtab = mktrans(string1, string2, cflag);
			if (sflag)
				squeeztab = mkstore(string2, 0);
		} else if (sflag) {
			if (string1 == NULL)
				usage(0);
			squeeztab = mkstore(string1, cflag);
		} else
			(void)mkstore(string1, cflag);
	}
	input = ib_alloc(0, 0);
	errcnt |= multibyte ? tr_mb() : tr_sb();
	return errcnt;
}
