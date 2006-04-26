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
 * Sccsid @(#)ext.h	1.43 (gritter) 4/26/06
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
