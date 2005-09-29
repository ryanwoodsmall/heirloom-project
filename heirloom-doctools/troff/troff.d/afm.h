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
 * Copyright (c) 2005 Gunnar Ritter, Freiburg i. Br., Germany
 *
 * Sccsid @(#)afm.h	1.13 (gritter) 9/29/05
 */

struct kernpair {
	unsigned short	ch1;
	unsigned short	ch2;
	short	k;
};

struct namecache {
	short	afpos;
	short	fival[2];
};

extern struct afmtab {
	struct Font	Font;
	char	*path;
	char	*file;
	char	*base;
	char	*fontname;
	int	*fontab;
	char	*kerntab;
	short	*codetab;
	short	*fitab;
	char	**nametab;
	struct namecache	*namecache;
	int	nameprime;
	struct kernpair	*kernpairs;
	int	nkernpairs;
	int	kernprime;
	int	nspace;
	int	rq;
	int	lineno;
	int	nchars;
	int	capheight;
	int	xheight;
	enum {
		TYPE_AFM,
		TYPE_OTF
	}	type;
} **afmtab;
extern int nafm;

extern	short		**fitab;
extern	int		**fontab;
extern	char		**kerntab;
extern	short		**codetab;

extern unsigned short	unitsPerEm;

extern	int	afmget(struct afmtab *, char *, size_t);
extern	int	otfget(struct afmtab *, char *, size_t);
extern	int	otfcff(const char *, char *, size_t, size_t *, size_t *);
extern	struct namecache	*afmnamelook(struct afmtab *, const char *);
extern	int	afmgetkern(struct afmtab *, int, int);
extern	void	makefont(int, char *, char *, char *, char *, int);
extern	int	unitconv(int);
extern	void	afmalloc(struct afmtab *, int);
extern	void	afmremap(struct afmtab *);
extern	int	afmmapname(const char *, int, int);
extern	void	afmaddchar(struct afmtab *, int, int, int, int, int[],
			char *, int, int);
extern	struct kernpair	*afmkernlook(struct afmtab *, int, int);
extern	int	nextprime(int n);
