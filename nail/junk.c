/*
 * Nail - a mail user agent derived from Berkeley Mail.
 *
 * Copyright (c) 2000-2004 Gunnar Ritter, Freiburg i. Br., Germany.
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
static char sccsid[] = "@(#)junk.c	1.35 (gritter) 10/3/04";
#endif
#endif /* not lint */

#include "config.h"

#include "rcv.h"
#include "extern.h"
#include <sys/stat.h>
#include <fcntl.h>
#include <limits.h>
#include <time.h>
#include <unistd.h>
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

#define	MAX2	0x0000ffff
#define	MAX3	0x00ffffffUL
#define	MAX4	0xffffffffUL

/*
 * The dictionary consists of two files forming a hash table. The hash
 * consists of the first 32 bits of the result of applying MD5 to the
 * input word. This scheme ensures that collisions are unlikely enough
 * to make junk detection work, since the number of actually important
 * words is relatively small. (Experiments indicated that it still
 * worked acceptably even if only 19 bits were used.)
 *
 * On the other hand, using 32 bits makes it nearly impossible to derive
 * reasonably well chosen secret tokens like passwords from the database,
 * since e.g. all six-character ASCII alphanumeric words already map to
 * each possible hash thirteen times. Testing for occurences of randomly
 * chosen tokens will therefore likely not give useful results.
 *
 * To make the chain structure independent from input, the MD5 output is
 * xor'ed with a random number. This makes it impossible that someone uses
 * a carefully crafted message for a denial-of-service attack against the
 * database.
 */
static struct node {
	char	hash[4];	/* mangled first 32 bits of MD5 of word */
	char	next[4];	/* bit-negated table index of next node */
	char	good[3];	/* number of times this appeared in good msgs */
	char	bad[3];		/* number of times this appeared in bad msgs */
	char	prob[3];	/* transformed floating-point probability */
} *nodes;

static struct super {
	char	size[4];	/* allocated nodes in the chain file */
	char	used[4];	/* used nodes in the chain file */
	char	ngood[4];	/* number of good messages scanned so far */
	char	nbad[4];	/* number of bad messages scanned so far */
	char	mangle[4];	/* used to mangle the MD5 hash */
	char	bucket[65536][4];	/* bit-negated node indices */
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

#define	f2s(d)	(smin(((unsigned)((d) * MAX3)), MAX3))

#define	s2f(s)	((float)(s) / MAX3)

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
	char	*save;
	int	price;
	int	url;
	int	lastc;
	enum loc {
		FROM_LINE	= 0,
		HEADER		= 1,
		BODY		= 2
	} loc;
	char	field[LINESIZE];
};

#define	constituent(c, b, i, price) \
	((c) & 0200 || alnumchar(c) || (c) == '\'' || (c) == '"' || \
		(c) == '$' || (c) == '!' || \
		((c) == '-' && !(price)) || \
		(((c) == '.' || (c) == ',') && \
		 (i) > 0 && digitchar((b)[(i)-1]&0377)))

#define	url_xchar(c) \
	(((c)&0200) == 0 && ((c)&037) != (c) && (c) != 0177 && \
	 !spacechar(c) && (c) != '{' && (c) != '}' && (c) != '|' && \
	 (c) != '\\' && (c) != '^' && (c) != '~' && (c) != '[' && \
	 (c) != ']' && (c) != '`' && (c) != '<' && (c) != '>' && \
	 (c) != '#' && (c) != '"')

enum db {
	SUPER = 0,
	NODES = 1
};

enum entry {
	GOOD = 0,
	BAD  = 1
};

static const char	README1[] = "\
This is a junk mail database maintained by nail(1). It does not contain any\n\
of the actual words found in your messages. Instead, parts of mangled MD5\n\
hashes are used for lookup. It is thus possible to tell if some given word\n\
was likely contained in your mail from examining this data, at best.\n";
static const char	README2[] = "\n\
The database files are stored in compress(1) format by default. This saves\n\
some space, but leads to higher processor usage when the database is read\n\
or updated. You can use uncompress(1) on these files if you prefer to store\n\
them in flat form.\n";

static int	verbose;

static enum okay getdb(void);
static void putdb(void);
static FILE *dbfp(enum db db, int rw, int *compressed);
static struct node *lookup(unsigned long hash, int create);
static char *nextword(char **buf, size_t *bufsize, size_t *count, FILE *fp,
		struct lexstat *sp);
static void add(const char *word, enum entry entry, struct lexstat *sp);
static enum okay scan(struct message *m, enum entry entry,
		void (*func)(const char *, enum entry, struct lexstat *));
static void recompute(void);
static int insert(int *msgvec, enum entry entry);
static void clsf(struct message *m);
static void rate(const char *word, enum entry entry, struct lexstat *sp);
static unsigned long dbhash(const char *word);
static void mkmangle(void);

static enum okay 
getdb(void)
{
	FILE	*sfp, *nfp;
	void	*zp = NULL;
	long	n;
	int	compressed;

	if ((sfp = dbfp(SUPER, 0, &compressed)) == (FILE *)-1)
		return STOP;
	super = smalloc(sizeof *super);
	if (sfp) {
		if (compressed)
			zp = zalloc(sfp);
		if ((compressed ? zread(zp, (char *)super, sizeof *super)
					!= sizeof *super :
				fread(super, sizeof *super, 1, sfp) != 1) ||
				ferror(sfp)) {
			fprintf(stderr, "Error reading junk mail database.\n");
			memset(super, 0, sizeof *super);
			mkmangle();
			if (compressed)
				zfree(zp);
			Fclose(sfp);
			sfp = NULL;
		} else if (compressed)
			zfree(zp);
	} else {
		memset(super, 0, sizeof *super);
		mkmangle();
	}
	if ((n = getn(super->size)) == 0) {
		n = 1;
		putn(super->size, 1);
	}
	nodes = smalloc(n * sizeof *nodes);
	if (sfp && (nfp = dbfp(NODES, 0, &compressed)) != NULL) {
		if (nfp == (FILE *)-1) {
			Fclose(sfp);
			free(super);
			free(nodes);
			return STOP;
		}
		if (compressed)
			zp = zalloc(nfp);
		if ((compressed ? zread(zp, (char *)nodes, n * sizeof *nodes)
				!= n * sizeof *nodes :
				fread(nodes, sizeof *nodes, n, nfp) != n) ||
				ferror(nfp)) {
			fprintf(stderr, "Error reading junk mail database.\n");
			memset(nodes, 0, n * sizeof *nodes);
			memset(super, 0, sizeof *super);
			mkmangle();
			putn(super->size, n);
		}
		if (compressed)
			zfree(zp);
		Fclose(nfp);
	} else
		memset(nodes, 0, n * sizeof *nodes);
	if (sfp)
		Fclose(sfp);
	return OKAY;
}

static void 
putdb(void)
{
	FILE	*sfp, *nfp;
	sighandler_type	saveint;
	void	*zp;
	int	scomp, ncomp;

	if ((sfp = dbfp(SUPER, 1, &scomp)) == NULL || sfp == (FILE *)-1)
		return;
	if ((nfp = dbfp(NODES, 1, &ncomp)) == NULL || nfp == (FILE *)-1)
		return;
	saveint = safe_signal(SIGINT, SIG_IGN);
	if (scomp) {
		zp = zalloc(sfp);
		zwrite(zp, (char *)super, sizeof *super);
		zfree(zp);
	} else
		fwrite(super, 1, sizeof *super, sfp);
	if (ncomp) {
		zp = zalloc(nfp);
		zwrite(zp, (char *)nodes, getn(super->size) * sizeof *nodes);
		zfree(zp);
	} else
		fwrite(nodes, sizeof *nodes, getn(super->size), nfp);
	safe_signal(SIGINT, saveint);
	Fclose(sfp);
	Fclose(nfp);
}

static FILE *
dbfp(enum db db, int rw, int *compressed)
{
	FILE	*fp, *rp;
	char	*dir, *fn;
	struct flock	flp;
	const char *const	sf[] = {
		"super", "nodes"
	};
	const char *const	zf[] = {
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
	fn = ac_alloc((n = strlen(dir)) + 40);
	strcpy(fn, dir);
	fn[n] = '/';
	*compressed = 0;
	strcpy(&fn[n+1], sf[db]);
	if ((fp = Fopen(fn, rw ? "r+" : "r")) != NULL)
		goto okay;
	*compressed = 1;
	strcpy(&fn[n+1], zf[db]);
	if ((fp = Fopen(fn, rw ? "r+" : "r")) == NULL &&
			rw ? (fp = Fopen(fn, "w+")) == NULL : 0) {
		fprintf(stderr, "Cannot open junk mail database \"%s\".\n", fn);
		ac_free(fn);
		return NULL;
	}
	if (rw) {
		strcpy(&fn[n+1], "README");
		if (access(fn, F_OK) < 0 && (rp = Fopen(fn, "w")) != NULL) {
			fputs(README1, rp);
			fputs(README2, rp);
			Fclose(rp);
		}
	}
okay:	ac_free(fn);
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
lookup(unsigned long hash, int create)
{
	struct node	*n, *lastn = NULL;
	unsigned long	c, lastc = MAX4, used, size, incr;

	used = getn(super->used);
	size = getn(super->size);
	c = ~getn(super->bucket[hash & MAX2]);
	n = &nodes[c];
	while (c < used) {
		if (getn(n->hash) == hash)
			return n;
		lastc = c;
		lastn = n;
		c = ~getn(n->next);
		n = &nodes[c];
	}
	if (create) {
		if (used >= size) {
			incr = size > MAX2 ? MAX2 : size;
			if (size + incr > MAX4-MAX2) {
				fprintf(stderr,
					"Junk mail database overflow.\n");
				return NULL;
			}
			nodes = srealloc(nodes, (size+incr) * sizeof *nodes);
			memset(&nodes[size], 0, incr * sizeof *nodes);
			size += incr;
			putn(super->size, size);
		}
		n = &nodes[used];
		putn(n->hash, hash);
		if (lastc < used)
			putn(lastn->next, ~used);
		else
			putn(super->bucket[hash & MAX2], ~used);
		used++;
		putn(super->used, used);
		return n;
	} else
		return NULL;
}

#define	SAVE(c)	{ \
	if (i+j >= (long)*bufsize-4) \
		*buf = srealloc(*buf, *bufsize += 32); \
	(*buf)[j+i] = (c); \
	i += (*buf)[j+i] != '\0'; \
}

static char *
nextword(char **buf, size_t *bufsize, size_t *count, FILE *fp,
		struct lexstat *sp)
{
	int	c, i, j, k;
	char	*cp, *cq;

	if (sp->save) {
		i = j = 0;
		for (cp = sp->save; *cp; cp++) {
			SAVE(*cp&0377)
		}
		SAVE('\0')
		free(sp->save);
		sp->save = NULL;
		return *buf;
	}
	if (sp->loc == FROM_LINE)
		while (*count > 0 && (c = getc(fp)) != EOF) {
			sp->lastc = c;
			if (c == '\n') {
				sp->loc = HEADER;
				break;
			}
		}
loop:	i = 0;
	j = 0;
	if (sp->loc == HEADER && sp->field[0]) {
	field:	cp = sp->field;
		do {
			c = *cp&0377;
			SAVE(c)
			cp++;
		} while (*cp);
		j = i;
		i = 0;
	}
	if (sp->price) {
		sp->price = 0;
		SAVE('$')
	}
	while (*count > 0 && (c = getc(fp)) != EOF) {
		(*count)--;
		if (c == '$' && i == 0)
			sp->price = 1;
		if (sp->loc == HEADER && sp->lastc == '\n') {
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
				sp->loc = BODY;
			}
		}
		sp->lastc = c;
		if (sp->url) {
			if (!url_xchar(c)) {
				sp->url = 0;
				cp = sp->save = smalloc(i+6);
				for (cq = "HOST*"; *cq; cq++)
					*cp++ = *cq;
				for (cq = &(*buf)[j]; *cq != ':'; cq++);
				cq += 3;	/* skip "://" */
				while (cq < &(*buf)[i+j] &&
						(alnumchar(*cq&0377) ||
						 *cq == '.' || *cq == '-'))
					*cp++ = *cq++;
				*cp = '\0';
				break;
			}
			SAVE(c)
		} else if (constituent(c, *buf, i+j, sp->price) ||
				sp->loc == HEADER && c == '.' &&
				asccasecmp(sp->field, "subject*")) {
			SAVE(c)
		} else if (i > 0 && c == ':' && *count > 2) {
			if ((c = getc(fp)) != '/') {
				ungetc(c, fp);
				break;
			}
			(*count)--;
			if ((c = getc(fp)) != '/') {
				ungetc(c, fp);
				break;
			}
			(*count)--;
			sp->url = 1;
			SAVE('\0')
			cp = savestr(*buf);
			j = i = 0;
			for (cq = "URL*"; *cq; cq++) {
				SAVE(*cq&0377)
			}
			j = i;
			i = 0;
			do {
				if (alnumchar(*cp&0377)) {
					SAVE(*cp&0377)
				} else
					i = 0;
			} while (*++cp);
			for (cq = "://"; *cq; cq++) {
				SAVE(*cq&0377)
			}
		} else if (i > 0 && ((*buf)[i+j-1] == ',' ||
				 (*buf)[i+j-1] == '.') && !digitchar(c)) {
			i--;
			ungetc(c, fp);
			(*count)++;
			break;
		} else if (i > 0)
			break;
	}
	if (i > 0) {
		SAVE('\0')
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
		/*
		 * Including the results of other filtering software (the
		 * 'X-Spam' fields) might seem tempting, but will also rate
		 * their false negatives good with this filter. Therefore
		 * these fields are ignored.
		 *
		 * Handling 'Received' fields is difficult since they include
		 * lots of both useless and interesting words for our purposes.
		 */
		if (sp->loc == HEADER &&
				(asccasecmp(sp->field, "message-id*") == 0 ||
				 asccasecmp(sp->field, "references*") == 0 ||
				 asccasecmp(sp->field, "in-reply-to*") == 0 ||
				 asccasecmp(sp->field, "status*") == 0 ||
				 asccasecmp(sp->field, "x-status*") == 0 ||
				 asccasecmp(sp->field, "date*") == 0 ||
				 asccasecmp(sp->field, "delivery-date*") == 0 ||
				 ascncasecmp(sp->field, "x-spam", 6) == 0 ||
				 ascncasecmp(sp->field, "x-scanned", 9) == 0 ||
				 asccasecmp(sp->field, "received*") == 0 &&
				 	((2*c > i) || i < 4 ||
					asccasestr(*buf, "localhost") != NULL)))
			goto loop;
		return *buf;
	}
	return NULL;
}

/*ARGSUSED3*/
static void
add(const char *word, enum entry entry, struct lexstat *sp)
{
	unsigned	c;
	unsigned long	h;
	struct node	*n;

	h = dbhash(word);
	if ((n = lookup(h, 1)) != NULL) {
		switch (entry) {
		case GOOD:
			c = get(n->good);
			if (c < MAX3) {
				c++;
				put(n->good, c);
			}
			break;
		case BAD:
			c = get(n->bad);
			if (c < MAX3) {
				c++;
				put(n->bad, c);
			}
			break;
		}
	}
}

static enum okay 
scan(struct message *m, enum entry entry,
		void (*func)(const char *, enum entry, struct lexstat *))
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
		(*func)(buf, entry, sp);
	free(buf);
	free(sp);
	Fclose(fp);
	return OKAY;
}

static void 
recompute(void)
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
insert(int *msgvec, enum entry entry)
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
		if (u == MAX4) {
			fprintf(stderr, "Junk mail database overflow.\n");
			break;
		}
		u++;
		if (entry == GOOD)
			message[*ip-1].m_flag &= ~MJUNK;
		else
			message[*ip-1].m_flag |= MJUNK;
		scan(&message[*ip-1], entry, add);
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
cgood(void *v)
{
	return insert(v, GOOD);
}

int 
cjunk(void *v)
{
	return insert(v, BAD);
}

int 
cclassify(void *v)
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
	unsigned long	hash;
	enum loc	loc;
} best[BEST];

static void 
clsf(struct message *m)
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
			fprintf(stderr, "Probe %2d: \"%s\", hash=%lu "
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
rate(const char *word, enum entry entry, struct lexstat *sp)
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
	if (d >= best[BEST-1].dist)
		for (i = 0; i < BEST; i++) {
			if (h == best[i].hash)
				break;
			/*
			 * This selection prefers words from the end of the
			 * header and from the start of the body. It does
			 * probably not matter much at all, but gives at
			 * least the most interesting verbose output.
			 */
			if (d > best[i].dist || best[i].loc == HEADER &&
					d >= best[i].dist) {
				for (j = BEST-2; j >= i; j--)
					best[j+1] = best[j];
				best[i].dist = d;
				best[i].prob = p;
				best[i].word = savestr(word);
				best[i].hash = h;
				best[i].loc = sp->loc;
				break;
			}
		}
}

static unsigned long
dbhash(const char *word)
{
	unsigned char	digest[16];
	unsigned	h;
	MD5_CTX	ctx;

	MD5Init(&ctx);
	MD5Update(&ctx, (unsigned char *)word, strlen(word));
	MD5Final(digest, &ctx);
	h = getn(digest) ^ getn(super->mangle);
	return h;
}

/*
 * The selection of the value for mangling is not critical. It is practically
 * impossible for any person to determine the exact time when the database
 * was created first (without looking at the database, which would reveal the
 * value anyway), so we just use this. The MD5 hash here ensures that each
 * single second gives a completely different mangling value.
 */
static void 
mkmangle(void)
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
	putn(super->mangle, s);
}
