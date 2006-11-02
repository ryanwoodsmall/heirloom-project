#if __GNUC__ >= 3 && __GNUC_MINOR__ >= 4 || __GNUC__ >= 4
#define	USED	__attribute__ ((used))
#elif defined __GNUC__
#define	USED	__attribute__ ((unused))
#else
#define	USED
#endif
#if defined (DUMP)
static const char sccsid[] USED =
	"@(#)/usr/ucb/otfdump.sl	7.173 (gritter) 11/2/06";
#elif defined (DPOST)
static const char sccsid[] USED = "@(#)dpost.sl  7.173 (gritter) 11/2/06";
const char creator[] = "Heirloom Documentation Tools - dpost 7.173 (gritter) 11/2/06";
#elif defined (NROFF)
static const char sccsid[] USED = "@(#)/usr/ucb/nroff.sl	7.173 (gritter) 11/2/06";
#else	/* !DUMP, !NROFF */
static const char sccsid[] USED = "@(#)/usr/ucb/troff.sl	7.173 (gritter) 11/2/06";
#endif	/* !DUMP, !NROFF */
const char revision[] = "7.173";
/* SLIST */
/*
ext.h: * Sccsid @(#)ext.h	1.101 (gritter) 11/2/06
hytab.c: * Sccsid @(#)hytab.c	1.4 (gritter) 8/16/05
n1.c: * Sccsid @(#)n1.c	1.131 (gritter) 11/2/06
n2.c: * Sccsid @(#)n2.c	1.38 (gritter) 11/2/06
n3.c: * Sccsid @(#)n3.c	1.174 (gritter) 11/1/06
n4.c: * Sccsid @(#)n4.c	1.91 (gritter) 11/2/06
n5.c: * Sccsid @(#)n5.c	1.115 (gritter) 11/1/06
n7.c: * Sccsid @(#)n7.c	1.148 (gritter) 11/2/06
n8.c: * Sccsid @(#)n8.c	1.37 (gritter) 10/29/06
n9.c: * Sccsid @(#)n9.c	1.65 (gritter) 10/15/06
ni.c: * Sccsid @(#)ni.c	1.45 (gritter) 10/29/06
nii.c: * Sccsid @(#)nii.c	1.47 (gritter) 11/2/06
suftab.c: * Sccsid @(#)suftab.c	1.4 (gritter) 8/16/05
tdef.h: * Sccsid @(#)tdef.h	1.152 (gritter) 11/2/06
nroff.d/n10.c: * Sccsid @(#)n10.c	1.30 (gritter) 9/5/06
nroff.d/n6.c: * Sccsid @(#)n6.c	1.48 (gritter) 9/8/06
nroff.d/pt.h: *	Sccsid @(#)pt.h	1.41 (gritter) 8/12/06
nroff.d/tw.h: * Sccsid @(#)tw.h	1.6 (gritter) 4/25/06
troff.d/afm.c: * Sccsid @(#)afm.c	1.62 (gritter) 10/30/06
troff.d/afm.h: * Sccsid @(#)afm.h	1.35 (gritter) 10/5/06
troff.d/dev.h: * Sccsid @(#)dev.h	1.15 (gritter) 9/24/06
troff.d/draw.c: * Sccsid @(#)draw.c	1.3 (gritter) 8/8/05
troff.d/makedev.c: * Sccsid @(#)makedev.c	1.16 (gritter) 9/22/06
troff.d/otf.c: * Sccsid @(#)otf.c	1.59 (gritter) 10/5/06
troff.d/otfdump.c: * Sccsid @(#)otfdump.c	1.15 (gritter) 3/16/06
troff.d/pt.h: * Sccsid @(#)pt.h	1.53 (gritter) 9/11/06
troff.d/t10.c: * Sccsid @(#)t10.c	1.96 (gritter) 10/24/06
troff.d/t6.c: * Sccsid @(#)t6.c	1.188 (gritter) 11/2/06
troff.d/troff.h: * Sccsid @(#)troff.h	1.26 (gritter) 8/12/06
troff.d/unimap.c: * Sccsid @(#)unimap.c	1.14 (gritter) 11/2/06
troff.d/unimap.h: * Sccsid @(#)unimap.h	1.3 (gritter) 11/2/06
libhnj/hnjalloc.c: * Sccsid @(#)hnjalloc.c	1.3 (gritter) 8/26/05
libhnj/hnjalloc.h: * Sccsid @(#)hnjalloc.h	1.2 (gritter) 8/25/05
libhnj/hyphen.c: * Sccsid @(#)hyphen.c	1.5 (gritter) 4/19/06
libhnj/hyphen.h: * Sccsid @(#)hyphen.h	1.3 (gritter) 8/25/05
troff.d/dpost.d/afm.c: * Sccsid @(#)afm.c	1.3 (gritter) 5/1/06
troff.d/dpost.d/asciitype.c:	Sccsid @(#)asciitype.c	1.4 (gritter) 4/17/03	
troff.d/dpost.d/asciitype.h:	Sccsid @(#)asciitype.h	1.6 (gritter) 9/9/05	
troff.d/dpost.d/color.c: * Sccsid @(#)color.c	1.5 (gritter) 11/29/05
troff.d/dpost.d/comments.h: * Sccsid @(#)comments.h	1.5 (gritter) 8/23/05
troff.d/dpost.d/dpost.c: * Sccsid @(#)dpost.c	1.169 (gritter) 10/16/06
troff.d/dpost.d/dpost.h: * Sccsid @(#)dpost.h	1.11 (gritter) 9/22/06
troff.d/dpost.d/draw.c: * Sccsid @(#)draw.c	1.5 (gritter) 11/28/05
troff.d/dpost.d/ext.h: * Sccsid @(#)ext.h	1.5 (gritter) 1/6/06
troff.d/dpost.d/gen.h: * Sccsid @(#)gen.h	1.16 (gritter) 10/15/06
troff.d/dpost.d/getopt.c: * Sccsid @(#)getopt.c	1.8 (gritter) 8/2/05
troff.d/dpost.d/glob.c: * Sccsid @(#)glob.c	1.3 (gritter) 8/9/05
troff.d/dpost.d/makedev.c: * Sccsid @(#)makedev.c	1.1 (gritter) 9/4/05
troff.d/dpost.d/misc.c: * Sccsid @(#)misc.c	1.13 (gritter) 10/16/06
troff.d/dpost.d/otf.c: * Sccsid @(#)otf.c	1.3 (gritter) 10/13/05
troff.d/dpost.d/path.h: * Sccsid @(#)path.h	1.5 (gritter) 12/18/05
troff.d/dpost.d/pictures.c: * Sccsid @(#)pictures.c	1.6 (gritter) 11/29/05
troff.d/dpost.d/ps_include.c: * Sccsid @(#)ps_include.c	1.10 (gritter) 10/15/06
troff.d/dpost.d/request.c: * Sccsid @(#)request.c	1.3 (gritter) 8/9/05
troff.d/dpost.d/request.h: * Sccsid @(#)request.h	1.3 (gritter) 8/9/05
*/
