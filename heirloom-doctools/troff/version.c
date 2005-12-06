#if __GNUC__ >= 3 && __GNUC_MINOR__ >= 4 || __GNUC__ >= 4
#define	USED	__attribute__ ((used))
#elif defined __GNUC__
#define	USED	__attribute__ ((unused))
#else
#define	USED
#endif
#if defined (DUMP)
static const char sccsid[] USED =
	"@(#)/usr/ucb/otfdump.sl	5.25 (gritter) 12/6/05";
#elif defined (DPOST)
static const char sccsid[] USED = "@(#)dpost.sl  5.25 (gritter) 12/6/05";
const char creator[] = "Heirloom Documentation Tools - dpost 5.25 (gritter) 12/6/05";
#elif defined (NROFF)
static const char sccsid[] USED = "@(#)/usr/ucb/nroff.sl	5.25 (gritter) 12/6/05";
#else	/* !DUMP, !NROFF */
static const char sccsid[] USED = "@(#)/usr/ucb/troff.sl	5.25 (gritter) 12/6/05";
#endif	/* !DUMP, !NROFF */
/* SLIST */
/*
calloc.c: * Sccsid @(#)calloc.c	1.3 (gritter) 8/26/05
ext.h: * Sccsid @(#)ext.h	1.32 (gritter) 12/6/05
hytab.c: * Sccsid @(#)hytab.c	1.4 (gritter) 8/16/05
mallint.h: * Sccsid @(#)mallint.h	1.4 (gritter) 8/27/05
malloc.c: * Sccsid @(#)malloc.c	1.3 (gritter) 8/26/05
n1.c: * Sccsid @(#)n1.c	1.51 (gritter) 12/6/05
n2.c: * Sccsid @(#)n2.c	1.12 (gritter) 12/5/05
n3.c: * Sccsid @(#)n3.c	1.59 (gritter) 12/6/05
n4.c: * Sccsid @(#)n4.c	1.14 (gritter) 12/4/05
n5.c: * Sccsid @(#)n5.c	1.25 (gritter) 12/6/05
n7.c: * Sccsid @(#)n7.c	1.39 (gritter) 12/6/05
n8.c: * Sccsid @(#)n8.c	1.17 (gritter) 10/14/05
n9.c: * Sccsid @(#)n9.c	1.24 (gritter) 11/30/05
ni.c: * Sccsid @(#)ni.c	1.17 (gritter) 12/4/05
nii.c: * Sccsid @(#)nii.c	1.20 (gritter) 12/6/05
suftab.c: * Sccsid @(#)suftab.c	1.4 (gritter) 8/16/05
tdef.h: * Sccsid @(#)tdef.h	1.62 (gritter) 12/6/05
nroff.d/n10.c: * Sccsid @(#)n10.c	1.21 (gritter) 12/5/05
nroff.d/n6.c: * Sccsid @(#)n6.c	1.22 (gritter) 12/4/05
nroff.d/proto.h: * Sccsid @(#)proto.h	1.23 (gritter) 9/30/05
nroff.d/tw.h: * Sccsid @(#)tw.h	1.5 (gritter) 9/8/05
troff.d/afm.c: * Sccsid @(#)afm.c	1.38 (gritter) 10/4/05
troff.d/afm.h: * Sccsid @(#)afm.h	1.16 (gritter) 10/2/05
troff.d/dev.h: * Sccsid @(#)dev.h	1.8 (gritter) 9/30/05
troff.d/draw.c: * Sccsid @(#)draw.c	1.3 (gritter) 8/8/05
troff.d/makedev.c: * Sccsid @(#)makedev.c	1.10 (gritter) 9/8/05
troff.d/otf.c: * Sccsid @(#)otf.c	1.32 (gritter) 10/14/05
troff.d/otfdump.c: * Sccsid @(#)otfdump.c	1.10 (gritter) 10/13/05
troff.d/proto.h: * Sccsid @(#)proto.h	1.30 (gritter) 9/30/05
troff.d/t10.c: * Sccsid @(#)t10.c	1.51 (gritter) 12/3/05
troff.d/t6.c: * Sccsid @(#)t6.c	1.100 (gritter) 11/30/05
troff.d/troff.h: * Sccsid @(#)troff.h	1.13 (gritter) 9/11/05
troff.d/unimap.c: * Sccsid @(#)unimap.c	1.10 (gritter) 12/3/05
troff.d/unimap.h: * Sccsid @(#)unimap.h	1.1 (gritter) 8/17/05
libhnj/hnjalloc.c: * Sccsid @(#)hnjalloc.c	1.3 (gritter) 8/26/05
libhnj/hnjalloc.h: * Sccsid @(#)hnjalloc.h	1.2 (gritter) 8/25/05
libhnj/hyphen.c: * Sccsid @(#)hyphen.c	1.4 (gritter) 9/15/05
libhnj/hyphen.h: * Sccsid @(#)hyphen.h	1.3 (gritter) 8/25/05
troff.d/dpost.d/asciitype.c:	Sccsid @(#)asciitype.c	1.4 (gritter) 4/17/03	
troff.d/dpost.d/asciitype.h:	Sccsid @(#)asciitype.h	1.6 (gritter) 9/9/05	
troff.d/dpost.d/color.c: * Sccsid @(#)color.c	1.5 (gritter) 11/29/05
troff.d/dpost.d/comments.h: * Sccsid @(#)comments.h	1.5 (gritter) 8/23/05
troff.d/dpost.d/dpost.c: * Sccsid @(#)dpost.c	1.109 (gritter) 11/30/05
troff.d/dpost.d/dpost.h: * Sccsid @(#)dpost.h	1.6 (gritter) 10/1/05
troff.d/dpost.d/dpost_afm.c: * Sccsid @(#)dpost_afm.c	1.2 (gritter) 9/29/05
troff.d/dpost.d/dpost_draw.c: * Sccsid @(#)dpost_draw.c	1.5 (gritter) 11/28/05
troff.d/dpost.d/dpost_makedev.c: * Sccsid @(#)dpost_makedev.c	1.1 (gritter) 9/4/05
troff.d/dpost.d/dpost_otf.c: * Sccsid @(#)dpost_otf.c	1.3 (gritter) 10/13/05
troff.d/dpost.d/ext.h: * Sccsid @(#)ext.h	1.4 (gritter) 11/29/05
troff.d/dpost.d/gen.h: * Sccsid @(#)gen.h	1.13 (gritter) 11/29/05
troff.d/dpost.d/getopt.c: * Sccsid @(#)getopt.c	1.8 (gritter) 8/2/05
troff.d/dpost.d/glob.c: * Sccsid @(#)glob.c	1.3 (gritter) 8/9/05
troff.d/dpost.d/misc.c: * Sccsid @(#)misc.c	1.9 (gritter) 11/29/05
troff.d/dpost.d/path.h: * Sccsid @(#)path.h	1.4 (gritter) 11/29/05
troff.d/dpost.d/pictures.c: * Sccsid @(#)pictures.c	1.6 (gritter) 11/29/05
troff.d/dpost.d/ps_include.c: * Sccsid @(#)ps_include.c	1.8 (gritter) 11/29/05
troff.d/dpost.d/request.c: * Sccsid @(#)request.c	1.3 (gritter) 8/9/05
troff.d/dpost.d/request.h: * Sccsid @(#)request.h	1.3 (gritter) 8/9/05
*/
