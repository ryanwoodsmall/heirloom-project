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
static char sccsid[] = "@(#)junk.c	1.16 (gritter) 9/30/04";
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

#define	BOT	.01
#define	TOP	.99
#define	DFL	.40
#define	THR	.9
#define	MID	.5

static struct node {
	char	hash[4];
	char	next[4];
	char	good[3];
	char	bad[3];
	char	prob[3];
} *nodes;

static struct super {
	char	size[4];
	char	used[4];
	char	ngood[4];
	char	nbad[4];
	char	shuffle[4];
	char	bucket[65536][4];
} *super;

#define	get(e)	\
	((unsigned)(((char *)(e))[0]&0377) + \
	 ((unsigned)(((char *)(e))[1]&0377) << 8) + \
	 ((unsigned)(((char *)(e))[2]&0377) << 16))

#define	put(e, n) \
	(((char *)(e))[0] = (n) & 0x0000ff, \
	 ((char *)(e))[1] = ((n) & 0x00ff00) >> 8, \
	 ((char *)(e))[2] = ((n) & 0xff0000) >> 16)

#define	smin(a, b)	((a) < (b) ? (a) : (b))
#define	smax(a, b)	((a) < (b) ? (b) : (a))

#define	f2s(d)	(smin(((unsigned)((d) * 0xffffffU)), 0xffffffU))

#define	s2f(s)	((float)(s) / 0xffffffU)

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

enum db {
	SUPER = 0,
	NODES = 1
};

enum entry {
	GOOD = 0,
	BAD  = 1
};

static int	verbose;

static int	insert __P((int *, enum entry));
static enum okay	getdb __P((void));
static void	putdb __P((void));
static FILE	*dbfp __P((enum db, int));
static enum okay	scan __P((struct message *, enum entry,
	void (*) __P((const char *, enum entry))));
static void	add __P((const char *, enum entry));
static char	*nextword __P((char **, size_t *, size_t *,
			FILE *, struct lexstat *));
static void	recompute __P((void));
static void	clsf __P((struct message *));
static void	rate __P((const char *, enum entry));
static unsigned	long	dbhash __P((const char *));
static struct node	*lookup __P((unsigned long, int));
static void	mkshuffle __P((void));

static enum okay
getdb()
{
	FILE	*sfp, *nfp;
	void	*zp = NULL;
	long	n;

	if ((sfp = dbfp(SUPER, 0)) == (FILE *)-1)
		return STOP;
	super = smalloc(sizeof *super);
	if (sfp) {
		zp = zalloc(sfp);
		if (zread(zp, (char *)super, sizeof *super)
				!= sizeof *super ||
				ferror(sfp)) {
			fprintf(stderr, "Error reading junk mail database.\n");
			memset(super, 0, sizeof *super);
			mkshuffle();
			zfree(zp);
			Fclose(sfp);
			sfp = NULL;
		} else
			zfree(zp);
	} else {
		memset(super, 0, sizeof *super);
		mkshuffle();
	}
	if ((n = getn(super->size)) == 0) {
		n = 1;
		putn(super->size, 1);
	}
	nodes = smalloc(n * sizeof *nodes);
	if (sfp && (nfp = dbfp(NODES, 0)) != NULL) {
		if (nfp == (FILE *)-1) {
			Fclose(sfp);
			free(super);
			free(nodes);
			return STOP;
		}
		zp = zalloc(nfp);
		if (zread(zp, (char *)nodes, n * sizeof *nodes)
				!= n * sizeof *nodes ||
				ferror(nfp)) {
			fprintf(stderr, "Error reading junk mail database.\n");
			memset(nodes, 0, n * sizeof *nodes);
			memset(super, 0, sizeof *super);
			mkshuffle();
			putn(super->size, n);
		}
		zfree(zp);
		Fclose(nfp);
	} else
		memset(nodes, 0, n * sizeof *nodes);
	if (sfp)
		Fclose(sfp);
	return OKAY;
}

static void
putdb()
{
	FILE	*sfp, *nfp;
	sighandler_type	saveint;
	void	*zp;

	if ((sfp = dbfp(SUPER, 1)) == NULL || sfp == (FILE *)-1)
		return;
	if ((nfp = dbfp(NODES, 1)) == NULL || nfp == (FILE *)-1)
		return;
	saveint = safe_signal(SIGINT, SIG_IGN);
	zp = zalloc(sfp);
	zwrite(zp, (char *)super, sizeof *super);
	zfree(zp);
	zp = zalloc(nfp);
	zwrite(zp, (char *)nodes, getn(super->size) * sizeof *nodes);
	zfree(zp);
	safe_signal(SIGINT, saveint);
	Fclose(sfp);
	Fclose(nfp);
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
		"super.Z", "nodes.Z"
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

static struct node *
lookup(hash, create)
	unsigned long	hash;
	int	create;
{
	struct node	*n;
	unsigned long	c, used, size, incr;

	used = getn(super->used);
	size = getn(super->size);
	c = ~getn(super->bucket[hash & 0xffff]);
	n = &nodes[c];
	while (c < used) {
		if (getn(n->hash) == hash)
			return n;
		c = ~getn(n->next);
		n = &nodes[c];
	}
	if (create) {
		if (used >= size) {
			incr = size > 0xffff ? 0xffff : size;
			nodes = srealloc(nodes, (size+incr) * sizeof *nodes);
			memset(&nodes[size], 0, incr * sizeof *nodes);
			size += incr;
			putn(super->size, size);
		}
		n = &nodes[used];
		putn(n->hash, hash);
		c = ~getn(super->bucket[hash & 0xffff]);
		putn(n->next, ~c);
		putn(super->bucket[hash & 0xffff], ~used);
		used++;
		putn(super->used, used);
		return n;
	} else
		return NULL;
}

static char *
nextword(buf, bufsize, count, fp, sp)
	char	**buf;
	size_t	*bufsize, *count;
	FILE	*fp;
	struct lexstat	*sp;
{
	int	c, i, j, k;
	char	*pfx;

loop:	i = 0;
	j = 0;
	pfx = NULL;
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
		c = 0;
		for (k = 0; k < i; k++)
			if (digitchar((*buf)[k+j]&0377))
				c++;
			else if (!alphachar((*buf)[k+j]&0377) &&
					(*buf)[k+j] != '$') {
				c = 0;
				break;
			}
		if (c == i)
			goto loop;
		if (sp->inbody == 0 &&
				(asccasecmp(sp->field, "message-id*") == 0 ||
				 asccasecmp(sp->field, "references*") == 0 ||
				 asccasecmp(sp->field, "received*") == 0 &&
				 	2*c > i))
			goto loop;
		return *buf;
	}
	return NULL;
}

static void
add(word, entry)
	const char	*word;
	enum entry	entry;
{
	unsigned	c;
	unsigned long	h;
	struct node	*n;

	h = dbhash(word);
	if ((n = lookup(h, 1)) != NULL) {
		switch (entry) {
		case GOOD:
			c = get(n->good);
			if (c < 0xffffff) {
				c++;
				put(n->good, c);
			}
			break;
		case BAD:
			c = get(n->bad);
			if (c < 0xffffff) {
				c++;
				put(n->bad, c);
			}
			break;
		}
	}
}

static enum okay
scan(m, entry, func)
	struct message	*m;
	enum entry	entry;
	void	(*func) __P((const char *, enum entry));
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
		(*func)(buf, entry);
	free(buf);
	free(sp);
	Fclose(fp);
	return OKAY;
}

static void
recompute()
{
	unsigned long	used, i;
	unsigned long	ngood, nbad;
	unsigned	g, b, p;
	struct node	*n;
	float	d;

	used = getn(super->used);
	ngood = getn(super->ngood);
	nbad = getn(super->nbad);
	for (i = 0; i < used; i++) {
		n = &nodes[i];
		g = get(n->good) * 2;
		b = get(n->bad);
		if (g + b >= 5) {
			d = smin(1.0, nbad ? (float)b/nbad : 0.0) /
				(smin(1.0, ngood ? (float)g/ngood : 0.0) +
				 smin(1.0, nbad ? (float)b/nbad : 0.0));
			d = smin(TOP, d);
			d = smax(BOT, d);
			p = f2s(d);
			/*fprintf(stderr,
				"i=%u g=%u b=%u d=%g p=%u re=%g "
				"ngood=%lu nbad=%lu\n",
				i, g, b, d, p, s2f(p), ngood, nbad);*/
		} else if (g == 0 && b == 0)
			p = f2s(DFL);
		else
			p = 0;
		put(n->prob, p);
	}
}

static int
insert(msgvec, entry)
	int	*msgvec;
	enum entry	entry;
{
	int	*ip;
	unsigned long	u = 0;

	verbose = value("verbose") != NULL;
	if (getdb() != OKAY)
		return 1;
	switch (entry) {
	case GOOD:
		u = getn(super->ngood);
		break;
	case BAD:
		u = getn(super->nbad);
		break;
	}
	for (ip = msgvec; *ip; ip++) {
		if (entry == GOOD)
			message[*ip-1].m_flag &= ~MJUNK;
		else
			message[*ip-1].m_flag |= MJUNK;
		scan(&message[*ip-1], entry, add);
		u++;
	}
	switch (entry) {
	case GOOD:
		putn(super->ngood, u);
		break;
	case BAD:
		putn(super->nbad, u);
		break;
	}
	recompute();
	putdb();
	free(super);
	free(nodes);
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

	verbose = value("verbose") != NULL;
	if (getdb() != OKAY)
		return 1;
	for (ip = msgvec; *ip; ip++)
		clsf(&message[*ip-1]);
	free(super);
	free(nodes);
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
clsf(m)
	struct message	*m;
{
	int	i;
	float	a = 1, b = 1, r;

	if (verbose)
		fprintf(stderr, "Examining message %d\n", m - &message[0] + 1);
	for (i = 0; i < BEST; i++) {
		best[i].dist = 0;
		best[i].prob = -1;
	}
	if (scan(m, -1, rate) != OKAY)
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

/*ARGSUSED2*/
static void
rate(word, entry)
	const char	*word;
	enum entry	entry;
{
	struct node	*n;
	unsigned long	h;
	unsigned	s;
	float	p, d;
	int	i, j;

	h = dbhash(word);
	if ((n = lookup(h, 0)) != NULL) {
		s = get(n->prob);
		p = s2f(s);
	} else
		p = DFL;
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

static unsigned long
dbhash(word)
	const char	*word;
{
	unsigned char	digest[16];
	unsigned	h;
	MD5_CTX	ctx;

	MD5Init(&ctx);
	MD5Update(&ctx, (unsigned char *)word, strlen(word));
	MD5Final(digest, &ctx);
	h = getn(digest) ^ getn(super->shuffle);
	return h;
}

static void
mkshuffle()
{
	union {
		time_t	t;
		char	c[16];
	} u;
	unsigned long	s;
	unsigned char	digest[16];
	MD5_CTX	ctx;

	memset(&u, 0, sizeof u);
	time(&u.t);
	MD5Init(&ctx);
	MD5Update(&ctx, (unsigned char *)u.c, sizeof u.c);
	MD5Final(digest, &ctx);
	s = getn(digest);
	putn(super->shuffle, s);
}
