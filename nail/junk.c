/*
 * Nail - a mail user agent derived from Berkeley Mail.
 *
 * Copyright (c) 2000-2002 Gunnar Ritter, Freiburg i. Br., Germany.
 */
/*
 * Copyright (c) 2004
 *	Gunnar Ritter.  All rights reserved.
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
 *	This product includes software developed by Gunnar Ritter
 *	and his contributors.
 * 4. Neither the name of Gunnar Ritter nor the names of his contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY GUNNAR RITTER AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL GUNNAR RITTER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef lint
#ifdef	DOSCCS
static char sccsid[] = "@(#)junk.c	1.12 (gritter) 9/27/04";
#endif
#endif /* not lint */

#include "config.h"

#include "rcv.h"
#include "extern.h"
#include <sys/stat.h>
#include <fcntl.h>
#include <limits.h>
#include <time.h>
#include "md5.h"

/*
 * Mail -- a mail program
 *
 * Junk classification, mostly according to Paul Graham's "A Plan for Spam",
 * August 2002, <http://www.paulgraham.com/spam.html>, and his "Better
 * Bayesian Filtering", January 2003, <http://www.paulgraham.com/better.html>.
 */

#define	SHIFT	12
#define	MASK	((1<<(31-SHIFT))-1)
#define	SIZE	(MASK+1)
#define	STATS	4

#define	BOT	.01
#define	TOP	.99
#define	DFL	.40
#define	THR	.9
#define	MID	.5

struct element {
	char	c[2];
};

#define	get(e)	\
	((unsigned)((e)->c[0]&0377) + ((unsigned)((e)->c[1]&0377) << 8))

#define	put(e, n) \
	((e)->c[0] = (n) & 0x00ff, (e)->c[1] = ((n) & 0xff00) >> 8)

#define	smin(a, b)	((a) < (b) ? (a) : (b))
#define	smax(a, b)	((a) < (b) ? (b) : (a))

#define	f2s(d)	(smin(((unsigned)((d) * 0xffffU)), 0xffffU))

#define	s2f(s)	((float)(s) / 0xffffU)

#define	getn(p)	\
	((unsigned long)(((char *)(p))[0]&0377) + \
	 ((unsigned long)(((char *)(p))[1]&0377) << 8) + \
	 ((unsigned long)(((char *)(p))[2]&0377) << 16) + \
	 ((unsigned long)(((char *)(p))[3]&0377) << 24))

#define	putn(p, n) \
	(((char *)(p))[0] = (n) & 0x000000ffUL, \
	 ((char *)(p))[1] = ((n) & 0x0000ff00UL) >> 8, \
	 ((char *)(p))[2] = ((n) & 0x00ff0000UL) >> 16, \
	 ((char *)(p))[3] = ((n) & 0xff000000UL) >> 24)

struct lexstat {
	int	price;
	int	inbody;
	int	lastc;
	char	field[LINESIZE];
};

#define	constituent(c, b, i, price) \
	((c) & 0200 || alnumchar(c) || (c) == '\'' || (c) == '"' || \
		(c) == '$' || (c) == '!' || \
		((c) == '-' && !(price)) || \
		(((c) == '.' || (c) == ',') && \
		 (i) > 0 && digitchar((b)[(i)-1]&0377)))

#define	DBHEAD	4096

enum db {
	GOOD = 0,
	BAD  = 1,
	PROB = 2
};

static int	verbose;

static int	insert __P((int *, enum db));
static struct element	*getdb __P((enum db));
static void	putdb __P((enum db, struct element *));
static FILE	*dbfp __P((enum db, int));
static enum okay	scan __P((struct message *, struct element *,
	void (*) __P((const char *, struct element *))));
static void	add __P((const char *, struct element *));
static char	*nextword __P((char **, size_t *, size_t *,
			FILE *, struct lexstat *));
static void	recompute __P((struct element *, struct element *));
static void	clsf __P((struct message *, struct element *));
static void	rate __P((const char *, struct element *));
static unsigned	dbhash __P((const char *));

static struct element *
getdb(db)
	enum db	db;
{
	FILE	*fp;
	struct element	*table;
	void	*zp = NULL;

	if ((fp = dbfp(db, 0)) == (FILE *)-1)
		return NULL;
	if (fp)
		zp = zalloc(fp);
	table = smalloc(SIZE * sizeof *table + STATS);
	if (fp == NULL || zread(zp, (char *)table,
				SIZE * sizeof *table + STATS) !=
			SIZE * sizeof *table + STATS ||
			ferror(fp))
		memset(table, 0, sizeof *table * SIZE + STATS);
	if (zp)
		zfree(zp);
	if (fp)
		Fclose(fp);
	return table;
}

static void
putdb(db, table)
	enum db	db;
	struct element	*table;
{
	FILE	*fp;
	sighandler_type	saveint;
	void	*zp;

	if ((fp = dbfp(db, 1)) == NULL || fp == (FILE *)-1)
		return;
	zp = zalloc(fp);
	saveint = safe_signal(SIGINT, SIG_IGN);
	zwrite(zp, (char *)table, SIZE * sizeof *table + STATS);
	safe_signal(SIGINT, saveint);
	zfree(zp);
	Fclose(fp);
}

static FILE *
dbfp(db, rw)
	enum db	db;
	int	rw;
{
	FILE	*fp;
	char	*dir, *fn;
	struct flock	flp;
	const char *const	sf[] = {
		"good.Z", "bad.Z", "prob.Z"
	};
	int	n;

	if ((dir = value("junkdb")) == NULL) {
		fprintf(stderr, "No junk mail database specified. "
				"Set the junkdb variable.\n");
		return (FILE *)-1;
	}
	dir = expand(dir);
	if (makedir(dir) == STOP) {
		fprintf(stderr, "Cannot create directory \"%s\"\n.", dir);
		return (FILE *)-1;
	}
	fn = ac_alloc((n = strlen(dir)) + strlen(sf[db]) + 2);
	strcpy(fn, dir);
	fn[n] = '/';
	strcpy(&fn[n+1], sf[db]);
	if ((fp = Fopen(fn, rw ? "r+" : "r")) == NULL &&
			rw ? (fp = Fopen(fn, "w+")) == NULL : 0) {
		fprintf(stderr, "Cannot open junk mail database \"%s\".\n", fn);
		ac_free(fn);
		return NULL;
	}
	ac_free(fn);
	if (fp) {
		flp.l_type = rw ? F_WRLCK : F_RDLCK;
		flp.l_start = 0;
		flp.l_len = 0;
		flp.l_whence = SEEK_SET;
		fcntl(fileno(fp), F_SETLKW, &flp);
	}
	return fp;
}

static char *
nextword(buf, bufsize, count, fp, sp)
	char	**buf;
	size_t	*bufsize, *count;
	FILE	*fp;
	struct lexstat	*sp;
{
	int	c, i = 0, j = 0, k;
	char	*pfx = NULL;

	if (sp->inbody == 0 && sp->field[0]) {
	field:	pfx = sp->field;
		do {
			c = *pfx&0377;
			goto put;
		pfx:	pfx++;
		} while (*pfx);
		pfx = NULL;
		j = i;
		i = 0;
	}
	if (sp->price) {
		sp->price = 0;
		c = '$';
		goto put;
	}
	while (*count > 0 && (c = getc(fp)) != EOF) {
		(*count)--;
		if (c == '$' && i == 0)
			sp->price = 1;
		if (sp->inbody == 0 && sp->lastc == '\n') {
			if (!spacechar(c)) {
				k = 0;
				while (k < sizeof sp->field - 3) {
					sp->field[k++] = c;
					sp->lastc = c;
					if (*count <= 0 ||
							(c = getc(fp)) == EOF)
						break;
					if (spacechar(c) || c == ':') {
						ungetc(c, fp);
						break;
					}
					(*count)--;
				}
				sp->field[k++] = '*';
				sp->field[k] = '\0';
				j = 0;
				goto field;
			} else if (c == '\n') {
				j = 0;
				sp->inbody = 1;
			}
		}
		sp->lastc = c;
		if (constituent(c, *buf, i+j, sp->price)) {
		put:	if (i+j >= (long)*bufsize-4)
				*buf = srealloc(*buf, *bufsize += 32);
			(*buf)[j+i++] = c;
			if (pfx)
				goto pfx;
		} else if (i > 0 && ((*buf)[i+j-1] == ',' ||
				 (*buf)[i+j-1] == '.') && !digitchar(c)) {
			i--;
			ungetc(c, fp);
			(*count)++;
			break;
		} else if (i > 0) {
			if (i < 2)
				i = 0;
			else {
				break;
			}
		}
	}
	if (i > 0) {
		(*buf)[i+j] = '\0';
		return *buf;
	}
	return NULL;
}

static void
add(word, table)
	const char	*word;
	struct element	*table;
{
	unsigned	h, n;

	h = dbhash(word);
	n = get(&table[h]);
	if (n < 0xffff)
		n++;
	put(&table[h], n);
}

static enum okay
scan(m, table, func)
	struct message	*m;
	struct element	*table;
	void	(*func) __P((const char *, struct element *));
{
	FILE	*fp;
	char	*buf = NULL, *cp;
	size_t	bufsize = 0, count;
	struct lexstat	*sp;

	if ((fp = Ftemp(&cp, "Ra", "w+", 0600, 1)) == NULL) {
		perror("tempfile");
		return STOP;
	}
	rm(cp);
	Ftfree(&cp);
	if (send_message(m, fp, NULL, NULL, CONV_TOFLTR, NULL) < 0) {
		Fclose(fp);
		return STOP;
	}
	fflush(fp);
	rewind(fp);
	sp = scalloc(1, sizeof *sp);
	count = fsize(fp);
	while (nextword(&buf, &bufsize, &count, fp, sp) != NULL)
		(*func)(buf, table);
	free(buf);
	free(sp);
	Fclose(fp);
	return OKAY;
}

static void
recompute(good, bad)
	struct element	*good, *bad;
{
	struct element	*prob;
	unsigned long	ngood, nbad;
	unsigned	g, b, n, p;
	float	d;

	if (good == NULL && (good = getdb(GOOD)) == NULL ||
			bad == NULL && (bad = getdb(BAD)) == NULL)
		return;
	ngood = getn(&good[SIZE]);
	nbad = getn(&bad[SIZE]);
	prob = scalloc(sizeof *prob, SIZE + STATS);
	for (n = 0; n < SIZE; n++) {
		g = get(&good[n]) * 2;
		b = get(&bad[n]);
		if (g + b >= 5) {
			d = smin(1.0, nbad ? (float)b/nbad : 0.0) /
				(smin(1.0, ngood ? (float)g/ngood : 0.0) +
				 smin(1.0, nbad ? (float)b/nbad : 0.0));
			d = smin(TOP, d);
			d = smax(BOT, d);
			p = f2s(d);
			/*fprintf(stderr,
				"n=%u g=%u b=%u d=%g p=%u re=%g "
				"ngood=%lu nbad=%lu\n",
				n, g, b, d, p, s2f(p), ngood, nbad);*/
		} else if (g == 0 && b == 0)
			p = f2s(DFL);
		else
			p = 0;
		put(&prob[n], p);
	}
	putdb(PROB, prob);
	free(good);
	free(bad);
	free(prob);
}

static int
insert(msgvec, db)
	int	*msgvec;
	enum db	db;
{
	struct element	*table;
	int	*ip;
	unsigned long	n;

	verbose = value("verbose") != NULL;
	if ((table = getdb(db)) == NULL)
		return 1;
	n = getn(&table[SIZE]);
	for (ip = msgvec; *ip; ip++) {
		if (db == GOOD)
			message[*ip-1].m_flag &= ~MJUNK;
		else
			message[*ip-1].m_flag |= MJUNK;
		if (scan(&message[*ip-1], table, add) == OKAY)
			if (n < 0xffffffffUL)
				n++;
	}
	putn(&table[SIZE], n);
	putdb(db, table);
	recompute(db == GOOD ? table : NULL, db == BAD ? table : NULL);
	return 0;
}

int
cgood(v)
	void	*v;
{
	return insert(v, GOOD);
}

int
cjunk(v)
	void	*v;
{
	return insert(v, BAD);
}

int
cclassify(v)
	void	*v;
{
	int	*msgvec = v, *ip;
	struct element	*table;

	verbose = value("verbose") != NULL;
	if ((table = getdb(PROB)) == NULL)
		return 1;
	for (ip = msgvec; *ip; ip++)
		clsf(&message[*ip-1], table);
	free(table);
	return 0;
}

#define	BEST	15
static struct {
	float	dist;
	float	prob;
	char	*word;
	unsigned	hash;
} best[BEST];

static void
clsf(m, table)
	struct message	*m;
	struct element	*table;
{
	int	i;
	float	a = 1, b = 1, r;

	if (verbose)
		fprintf(stderr, "Examining message %d\n", m - &message[0] + 1);
	for (i = 0; i < BEST; i++) {
		best[i].dist = 0;
		best[i].prob = -1;
	}
	if (scan(m, table, rate) != OKAY)
		return;
	if (best[0].prob == -1) {
		if (verbose)
			fprintf(stderr, "No information found.\n");
		m->m_flag &= ~MJUNK;
		return;
	}
	for (i = 0; i < BEST; i++) {
		if (best[i].prob == -1)
			break;
		if (verbose)
			fprintf(stderr, "Probe %2d: \"%s\", hash=%u "
				"prob=%g dist=%g\n",
				i+1,
				best[i].word, best[i].hash,
				best[i].prob, best[i].dist);
		a *= best[i].prob;
		b *= 1 - best[i].prob;
	}
	r = a+b > 0 ? a / (a+b) : 0;
	if (verbose)
		fprintf(stderr, "Junk probability of message %d: %g\n",
				m - &message[0] + 1, r);
	if (r > THR)
		m->m_flag |= MJUNK;
	else
		m->m_flag &= ~MJUNK;
}

static void
rate(word, table)
	const char	*word;
	struct element	*table;
{
	unsigned	h, s;
	float	p, d;
	int	i, j;

	h = dbhash(word);
	s = get(&table[h]);
	p = s2f(s);
	if (p == 0)
		return;
	d = p >= MID ? p - MID : MID - p;
	if (d > best[BEST-1].dist)
		for (i = 0; i < BEST; i++) {
			if (h == best[i].hash)
				break;
			if (d > best[i].dist) {
				for (j = BEST-2; j >= i; j--)
					best[j+1] = best[j];
				best[i].dist = d;
				best[i].prob = p;
				best[i].word = savestr(word);
				best[i].hash = h;
				break;
			}
		}
}

static unsigned
dbhash(word)
	const char	*word;
{
	unsigned char	digest[16];
	unsigned	h;
	MD5_CTX	ctx;

	MD5Init(&ctx);
	MD5Update(&ctx, (unsigned char *)word, strlen(word));
	MD5Final(digest, &ctx);
	h = getn(digest) & MASK;
	return h;
}
