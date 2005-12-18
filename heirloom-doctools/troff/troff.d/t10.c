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
 * Copyright 1989 Sun Microsystems, Inc.  All rights reserved.
 * Use is subject to license terms.
 */

/*	Copyright (c) 1984, 1986, 1987, 1988, 1989 AT&T	*/
/*	  All Rights Reserved  	*/


/*	from OpenSolaris "t10.c	1.11	05/06/08 SMI"	*/

/*
 * Portions Copyright (c) 2005 Gunnar Ritter, Freiburg i. Br., Germany
 *
 * Sccsid @(#)t10.c	1.56 (gritter) 12/18/05
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

#include <stdlib.h>
#include "tdef.h"
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "ext.h"
#include "dev.h"
#include "afm.h"
#include "proto.h"
#include "troff.h"
/*
 * troff10.c
 * 
 * typesetter interface
 */

int	vpos	 = 0;	/* absolute vertical position on page */
int	hpos	 = 0;	/* ditto horizontal */

short	*chtab;
char	*chname;
int	**fontab;
char	**kerntab;
short	**fitab;
short	**codetab;

int	Inch;
int	Hor;
int	Vert;
int	Unitwidth;
int	nfonts;
int	nsizes;
int	nchtab;

long	realpage;

static float	mzoom;

/* these characters are used as various signals or values
 * in miscellaneous places.
 * values are set in specnames in t10.c
 */

int	c_hyphen;
int	c_emdash;
int	c_rule;
int	c_minus;
int	c_fi;
int	c_fl;
int	c_ff;
int	c_ffi;
int	c_ffl;
int	c_acute;
int	c_grave;
int	c_under;
int	c_rooten;
int	c_boxrule;
int	c_lefthand;
int	c_dagger;

struct dev dev;
struct Font **fontbase;

int Nfont;

void
growfonts(int n)
{
	int	i, j;

	fontbase = realloc(fontbase, n * sizeof *fontbase);
	memset(&fontbase[Nfont], 0, (n - Nfont) * sizeof *fontbase);
	fontab = realloc(fontab, n * sizeof *fontab);
	memset(&fontab[Nfont], 0, (n - Nfont) * sizeof *fontab);
	kerntab = realloc(kerntab, n * sizeof *kerntab);
	memset(&kerntab[Nfont], 0, (n - Nfont) * sizeof *kerntab);
	codetab = realloc(codetab, n * sizeof *codetab);
	memset(&codetab[Nfont], 0, (n - Nfont) * sizeof *codetab);
	fitab = realloc(fitab, n * sizeof *fitab);
	memset(&fitab[Nfont], 0, (n - Nfont) * sizeof *fitab);
	fontlab = realloc(fontlab, n * sizeof *fontlab);
	memset(&fontlab[Nfont], 0, (n - Nfont) * sizeof *fontlab);
	cstab = realloc(cstab, n * sizeof *cstab);
	memset(&cstab[Nfont], 0, (n - Nfont) * sizeof *cstab);
	ccstab = realloc(ccstab, n * sizeof *ccstab);
	memset(&ccstab[Nfont], 0, (n - Nfont) * sizeof *ccstab);
	bdtab = realloc(bdtab, n * sizeof *bdtab);
	memset(&bdtab[Nfont], 0, (n - Nfont) * sizeof *bdtab);
	tracktab = realloc(tracktab, n * sizeof *tracktab);
	memset(&tracktab[Nfont], 0, (n - Nfont) * sizeof *tracktab);
	fallbacktab = realloc(fallbacktab, n * sizeof *fallbacktab);
	memset(&fallbacktab[Nfont], 0, (n - Nfont) * sizeof *fallbacktab);
	zoomtab = realloc(zoomtab, n * sizeof *zoomtab);
	memset(&zoomtab[Nfont], 0, (n - Nfont) * sizeof *zoomtab);
	lhangtab = realloc(lhangtab, n * sizeof *lhangtab);
	memset(&lhangtab[Nfont], 0, (n - Nfont) * sizeof *lhangtab);
	rhangtab = realloc(rhangtab, n * sizeof *rhangtab);
	memset(&rhangtab[Nfont], 0, (n - Nfont) * sizeof *rhangtab);
	kernafter = realloc(kernafter, n * sizeof *kernafter);
	memset(&kernafter[Nfont], 0, (n - Nfont) * sizeof *kernafter);
	kernbefore = realloc(kernbefore, n * sizeof *kernbefore);
	memset(&kernbefore[Nfont], 0, (n - Nfont) * sizeof *kernbefore);
	ftrtab = realloc(ftrtab, n * sizeof *ftrtab);
	for (i = Nfont; i < n; i++) {
		ftrtab[i] = malloc(NCHARS * sizeof **ftrtab);
		for (j = 0; j < NCHARS; j++)
			ftrtab[i][j] = j;
	}
	Nfont = n;
}

void
ptinit(void)
{
	int	i, nw;
	char	*filebase, *p, *ap, *descp;

	growfonts(NFONT+1);
	/* open table for device,
	 * read in resolution, size info, font info, etc.
	 * and set params
	 */
	p = malloc(strlen(termtab) + strlen(devname) + 10);
	termtab = strcpy(p, termtab);
	strcat(termtab, "/dev");
	strcat(termtab, devname);
	strcat(termtab, "/DESC");	/* makes "..../devXXX/DESC" */
	if ((descp = readdesc(termtab)) == NULL)
		done3(1);
	memcpy(&dev, descp, sizeof dev);
	Inch = dev.res;
	Hor = dev.hor;
	Vert = dev.vert;
	Unitwidth = dev.unitwidth;
	nfonts = dev.nfonts;
	nsizes = dev.nsizes;
	nchtab = dev.nchtab;
	if (nchtab >= NCHARS - 128) {
		errprint("too many special characters in file %s",
			termtab);
		done3(1);
	}
	filebase = setbrk(dev.filesize + 2*EXTRAFONT);	/* enough room for whole file */
	memcpy(filebase, &descp[sizeof dev], dev.filesize); /* all at once */
	free(descp);
	pstab = (int *) filebase;
	for (i = 0; pstab[i]; i++)
		pstab[i] = pts2u(pstab[i]);
	chtab = (short *)(pstab + nsizes + 1);
	chname = (char *) (chtab + dev.nchtab);
	p = chname + dev.lchname;
	for (i = 1; i <= nfonts; i++) {
		fontbase[i] = (struct Font *) p;
		nw = *p & BYTEMASK;	/* 1st thing is width count */
		fontlab[i] = PAIR(fontbase[i]->namefont[0], fontbase[i]->namefont[1]);
		/* for now, still 2 char names */
		if (smnt == 0 && fontbase[i]->specfont == 1)
			smnt = i;	/* first special font */
		p += sizeof(struct Font);	/* that's what's on the beginning */
		if ((ap = strstr(fontbase[i]->namefont, ".afm")) != NULL) {
			*ap = 0;
			if (ap == &fontbase[i]->namefont[1])
				fontlab[i] &= BYTEMASK;
			loadafm(i, fontlab[i], fontbase[i]->namefont, NULL, 1);
		} else
			makefont(i, p, p + nw, p + 2 * nw, p + 3 * nw, nw);
		p += 3 * nw + dev.nchtab + 128 - 32;
	}
	fontbase[0] = (struct Font *) p;	/* the last shall be first */
	memset(fontbase[0], 0, sizeof *fontbase[0]);
	nw = EXTRAFONT - dev.nchtab - (128-32) - sizeof (struct Font);
	fontbase[0]->nwfont = nw;
	makefont(0, p, p + nw, p + 2 * nw, p + 3 * nw, nw);
	/* there are a lot of things that used to be constant
	 * that now require code to be executed.
	 */
	sps = SPS;
	ses = SES;
	for (i = 0; i < 16; i++)
		tabtab[i] = DTAB * (i + 1);
	pl = 11 * INCH;
	po = PO;
	spacesz = SS;
	lss = lss1 = VS;
	ll = ll1 = lt = lt1 = LL;
	apts = pts2u(apts);
	apts1 = pts2u(apts1);
	pts = pts2u(pts);
	pts1 = pts2u(pts1);
	ics = ICS;
	specnames();	/* install names like "hyphen", etc. */
	kern = xflag;
	if (ascii)
		return;
	fdprintf(ptid, "x T %s\n", devname);
	fdprintf(ptid, "x res %d %d %d\n", Inch, Hor, Vert);
	fdprintf(ptid, "x init\n");	/* do initialization for particular device */
  /*
	for (i = 1; i <= nfonts; i++)
		fdprintf(ptid, "x font %d %s\n", i, fontbase[i]->namefont);
	fdprintf(ptid, "x xxx fonts=%d sizes=%d unit=%d\n", nfonts, nsizes, Unitwidth);
	fdprintf(ptid, "x xxx nchtab=%d lchname=%d nfitab=%d\n",
		dev.nchtab, dev.lchname, dev.nchtab+128-32);
	fdprintf(ptid, "x xxx sizes:\nx xxx ");
	for (i = 0; i < nsizes; i++)
		fdprintf(ptid, " %d", pstab[i]);
	fdprintf(ptid, "\nx xxx chars:\nx xxx ");
	for (i = 0; i < dev.nchtab; i++)
		fdprintf(ptid, " %s", &chname[chtab[i]]);
	fdprintf(ptid, "\nx xxx\n");
  */
}

void
specnames(void)
{
	static struct {
		int	*n;
		char	*v;
	} spnames[] = {
		&c_hyphen, "hy",
		&c_emdash, "em",
		&c_rule, "ru",
		&c_minus, "\\-",
		&c_fi, "fi",
		&c_fl, "fl",
		&c_ff, "ff",
		&c_ffi, "Fi",
		&c_ffl, "Fl",
		&c_acute, "aa",
		&c_grave, "ga",
		&c_under, "ul",
		&c_rooten, "rn",
		&c_boxrule, "br",
		&c_lefthand, "lh",
		&c_dagger, "dg",
		0, 0
	};
	int	i;

	for (i = 0; spnames[i].n; i++)
		*spnames[i].n = findch(spnames[i].v);
}

int
findch(register char *s)	/* find char s in chname */
{
	register int	i;

	for (i = 0; i < nchtab; i++)
		if (strcmp(s, &chname[chtab[i]]) == 0)
			return(i + 128);
	return(0);
}

void
ptout(register tchar i)
{
	register int dv;
	register tchar	*k;
	int temp, a, b;

	if (cbits(i) != '\n') {
		*olinep++ = i;
		return;
	}
	if (olinep == oline) {
		lead += lss;
		return;
	}

	hpos = po;	/* ??? */
	esc = 0;	/* ??? */
	ptesc();	/* the problem is to get back to the left end of the line */
	dv = 0;
	for (k = oline; k < olinep; k++) {
		if (ismot(*k) && isvmot(*k)) {
			temp = absmot(*k);
			if (isnmot(*k))
				temp = -temp;
			dv += temp;
		}
	}
	if (dv) {
		vflag++;
		*olinep++ = makem(-dv);
		vflag = 0;
	}

	b = dip->blss + lss;
	lead += dip->blss + lss;
	dip->blss = 0;
	for (k = oline; k < olinep; )
		k = ptout0(k, olinep);	/* now passing a pointer! */
	olinep = oline;
	lead += dip->alss;
	a = dip->alss;
	dip->alss = 0;
	/*
	fdprintf(ptid, "x xxx end of line: hpos=%d, vpos=%d\n", hpos, vpos);
*/
	fdprintf(ptid, "n%d %d\n", b, a);	/* be nice to chuck */
}

tchar *
ptout0(tchar *pi, tchar *pend)
{
	register int j;
	register int k, w = 0;
	int	z, dx, dy, dx2, dy2, n;
	register tchar	i;
	int outsize;	/* size of object being printed */
	double	f;
	int tfont;

	outsize = 1;	/* default */
	i = *pi;
	k = cbits(i);
	if (k == FILLER)
		return(pi+outsize);
	if (ismot(i)) {
		j = absmot(i);
		if (isnmot(i))
			j = -j;
		if (isvmot(i))
			lead += j;
		else 
			esc += j;
		return(pi+outsize);
	}
	if (k == XON) {
		if (xfont != mfont)
			ptfont();
		if (xpts != mpts || zoomtab[xfont] != mzoom)
			ptps();
		if (lead)
			ptlead();
		fdprintf(ptid, "x X ");
		/* 
	     * not guaranteed of finding a XOFF if a word overflow
		 * error occured, so also bound this loop by olinep
		 */
		pi++;
		while( cbits(*pi) != XOFF && pi < olinep )
			outascii(*pi++);
		oput('\n');
		if ( cbits(*pi) == XOFF )
			pi++;
		return pi;
	}
			;
	if (k == CHARHT) {
		if (xpts != mpts || zoomtab[xfont] != mzoom)
			ptps();
		j = f = u2pts(sbits(i));
		if (j != f && xflag && dev.anysize)
			fdprintf(ptid, "x H -23 %g\n", f);
		else
			fdprintf(ptid, "x H %d\n", j);
		return(pi+outsize);
	}
	if (k == SLANT) {
		fdprintf(ptid, "x S %d\n", (int)sfbits(i)-180);
		return(pi+outsize);
	}
	if (k == WORDSP) {
		oput('w');
		return(pi+outsize);
	}
	if (k == FONTPOS) {
		char temp[3];
		n = i >> 22;
		temp[0] = n & BYTEMASK;
		temp[1] = n >> BYTE;
		temp[2] = 0;
		ptfpcmd(0, temp);
		return(pi+outsize);
	}
	if (sfbits(i) == oldbits) {
		xfont = pfont;
		xpts = ppts;
	} else 
		xbits(i, 2);
	if (k < 040 && k != DRAWFCN)
		return(pi+outsize);
	if (k >= 32) {
		if (widcache[k-32].fontpts == xfont + (xpts<<8)  && !setwdf &&
				kern == 0) {
			w = widcache[k-32].width;
			bd = 0;
			cs = 0;
		} else {
			tfont = xfont;
			w = getcw(k-32);
			if (tfont != xfont)
				k = ftrans(xfont, k);
		}
	}
	if (xfont != mfont)
		ptfont();
	if (xpts != mpts || zoomtab[xfont] != mzoom)
		ptps();
	if (lead)
		ptlead();
	if (&pi[outsize] < pend)
		w += getkw(pi[0], pi[outsize]);
	j = z = 0;
	if (k != DRAWFCN) {
		if (cs) {
			if (bd)
				w += (bd - 1) * HOR;
			j = (cs - w) / 2;
			w = cs - j;
			if (bd)
				w -= (bd - 1) * HOR;
		}
		if (iszbit(i)) {
			if (cs)
				w = -j; 
			else 
				w = 0;
			z = 1;
		}
	}
	esc += j;
	/* put out the real character here */
	if (k == DRAWFCN) {
		if (esc)
			ptesc();
		dx = absmot(pi[3]);
		if (isnmot(pi[3]))
			dx = -dx;
		dy = absmot(pi[4]);
		if (isnmot(pi[4]))
			dy = -dy;
		switch (cbits(pi[1])) {
		case DRAWCIRCLE:	/* circle */
			fdprintf(ptid, "D%c %d\n", DRAWCIRCLE, dx);	/* dx is diameter */
			w = 0;
			hpos += dx;
			break;
		case DRAWELLIPSE:
			fdprintf(ptid, "D%c %d %d\n", DRAWELLIPSE, dx, dy);
			w = 0;
			hpos += dx;
			break;
		case DRAWLINE:	/* line */
			k = cbits(pi[2]);
			fdprintf(ptid, "D%c %d %d ", DRAWLINE, dx, dy);
			if (k < 128)
				fdprintf(ptid, "%c\n", k);
			else
				fdprintf(ptid, "%s\n", &chname[chtab[k - 128]]);
			w = 0;
			hpos += dx;
			vpos += dy;
			break;
		case DRAWARC:	/* arc */
			dx2 = absmot(pi[5]);
			if (isnmot(pi[5]))
				dx2 = -dx2;
			dy2 = absmot(pi[6]);
			if (isnmot(pi[6]))
				dy2 = -dy2;
			fdprintf(ptid, "D%c %d %d %d %d\n", DRAWARC,
				dx, dy, dx2, dy2);
			w = 0;
			hpos += dx + dx2;
			vpos += dy + dy2;
			break;
		case DRAWSPLINE:	/* spline */
		default:	/* something else; copy it like spline */
			fdprintf(ptid, "D%c %d %d", (int)cbits(pi[1]), dx, dy);
			w = 0;
			hpos += dx;
			vpos += dy;
			if (cbits(pi[3]) == DRAWFCN || cbits(pi[4]) == DRAWFCN) {
				/* it was somehow defective */
				fdprintf(ptid, "\n");
				break;
			}
			for (n = 5; cbits(pi[n]) != DRAWFCN; n += 2) {
				dx = absmot(pi[n]);
				if (isnmot(pi[n]))
					dx = -dx;
				dy = absmot(pi[n+1]);
				if (isnmot(pi[n+1]))
					dy = -dy;
				fdprintf(ptid, " %d %d", dx, dy);
				hpos += dx;
				vpos += dy;
			}
			fdprintf(ptid, "\n");
			break;
		}
		for (n = 3; cbits(pi[n]) != DRAWFCN; n++)
			;
		outsize = n + 1;
	} else if (k < 128) {
		/* try to go faster and compress output */
		/* by printing nnc for small positive motion followed by c */
		/* kludgery; have to make sure set all the vars too */
		if (esc > 0 && esc < 100) {
			oput(esc / 10 + '0');
			oput(esc % 10 + '0');
			oput(k);
			hpos += esc;
			esc = 0;
		} else {
			if (esc)
				ptesc();
			oput('c');
			oput(k);
			oput('\n');
		}
	} else {
		if (esc)
			ptesc();
		if (k >= nchtab + 128)
			fdprintf(ptid, "N%d\n", k - (nchtab+128));
		else
			fdprintf(ptid, "C%s\n", &chname[chtab[k - 128]]);
	}
	if (bd) {
		bd -= HOR;
		if (esc += bd)
			ptesc();
		if (k < 128) {
			fdprintf(ptid, "c%c\n", k);
		} else if (k >= nchtab + 128) {
			fdprintf(ptid, "N%d\n", k - (nchtab+128));
		} else
			fdprintf(ptid, "C%s\n", &chname[chtab[k - 128]]);
		if (z)
			esc -= bd;
	}
	esc += w;
	return(pi+outsize);
}

void
ptps(void)
{
	register int i, j, k;
	double	s, z;
	int	found;

	i = xpts;
	for (j = 0; i > (k = pstab[j]); j++)
		if (!k) {
			k = pstab[--j];
			break;
		}
	found = k == i;
	if (dev.anysize && xflag)
		k = i;
	s = u2pts(k);
	if ((z = zoomtab[xfont]) != 0 && dev.anysize && xflag)
		s *= z;
	if (dev.anysize && xflag && (!found || z != 0 && z != 1))
		fdprintf(ptid, "s-23 %g\n", s);
	else
		fdprintf(ptid, "s%d\n", (int)s);	/* really should put out string rep of size */
	mpts = i;
	mzoom = z;
}

void
ptfont(void)
{
	mfont = xfont;
	fdprintf(ptid, "f%d\n", xfont);
	if (xflag && lasttrack)
		fdprintf(ptid, "x X Track %d\n", lasttrack);
}

void
ptfpcmd(int f, char *s)
{
	if (ascii)
		return;
	fdprintf(ptid, "x font %d %s\n", f, s);
	ptfont();	/* make sure that it gets noticed */
}

void
ptlead(void)
{
	vpos += lead;
	if (!ascii)
		fdprintf(ptid, "V%d\n", vpos);
	lead = 0;
}

void
ptesc(void)
{
	hpos += esc;
	if (esc > 0) {
		oput('h');
		if (esc>=10 && esc<100) {
			oput(esc/10 + '0');
			oput(esc%10 + '0');
		} else
			fdprintf(ptid, "%d", esc);
	} else
		fdprintf(ptid, "H%d\n", hpos);
	esc = 0;
}

void
ptsupplyfont(char *fontname, char *file)
{
	if (ascii)
		return;
	fdprintf(ptid, "x X SupplyFont %s %s\n", fontname, file);
}

void
ptpapersize(void)
{
	if (ascii || mediasize.flag == 0)
		return;
	fdprintf(ptid, "x X PaperSize %d %d %d\n",
			mediasize.val[2], mediasize.val[3],
			mediasize.flag&2?1:0);
}

static void
cut1(const char *name, struct box *bp)
{
	if (bp->flag)
		fdprintf(ptid, "x X %s %d %d %d %d\n", name,
			bp->val[0], bp->val[1], bp->val[2], bp->val[3]);
}

void
ptcut(void)
{
	if (ascii)
		return;
	cut1("TrimAt", &trimat);
	cut1("BleedAt", &bleedat);
	cut1("CropAt", &cropat);
}

void
newpage(int n)	/* called at end of each output page (we hope) */
{
	int i;

	realpage++;
	ptlead();
	vpos = 0;
	if (ascii)
		return;
	fdprintf(ptid, "p%d\n", n);	/* new page */
	for (i = 0; i <= nfonts; i++)
		if (afmtab && fontbase[i]->afmpos)
			fdprintf(ptid, "x font %d %s\n", i,
				afmtab[(fontbase[i]->afmpos)-1]->path);
		else if (fontbase[i]->namefont && fontbase[i]->namefont[0])
			fdprintf(ptid, "x font %d %s\n", i, fontbase[i]->namefont);
	ptps();
	ptfont();
	ptpapersize();
	ptcut();
}

void
pttrailer(void)
{
	fdprintf(ptid, "x trailer\n");
}

void
ptstop(void)
{
	fdprintf(ptid, "x stop\n");
}

void
dostop(void)
{
	if (ascii)
		return;
	ptlead();
	vpos = 0;
	/* fdprintf(ptid, "x xxx end of page\n");*/
	if (!nofeed)
		pttrailer();
	ptlead();
	fdprintf(ptid, "x pause\n");
	flusho();
	mpts = mfont = 0;
	ptesc();
	esc = po;
	hpos = vpos = 0;	/* probably in wrong place */
}
