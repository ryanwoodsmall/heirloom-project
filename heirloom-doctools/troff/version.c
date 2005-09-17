#if __GNUC__ >= 3 && __GNUC_MINOR__ >= 4 || __GNUC__ >= 4
#define	USED	__attribute__ ((used))
#elif defined __GNUC__
#define	USED	__attribute__ ((unused))
#else
#define	USED
#endif
#ifdef	NROFF
static const char sccsid[] USED = "@(#)/usr/ucb/nroff.sl	4.8 (gritter) 9/18/05";
#else	/* !NROFF */
static const char sccsid[] USED = "@(#)/usr/ucb/troff.sl	4.8 (gritter) 9/18/05";
#endif	/* !NROFF */
/* SLIST */
/*
calloc.c: * Sccsid @(#)calloc.c	1.3 (gritter) 8/26/05
ext.h: * Sccsid @(#)ext.h	1.26 (gritter) 9/13/05
hytab.c: * Sccsid @(#)hytab.c	1.4 (gritter) 8/16/05
mallint.h: * Sccsid @(#)mallint.h	1.4 (gritter) 8/27/05
malloc.c: * Sccsid @(#)malloc.c	1.3 (gritter) 8/26/05
n1.c: * Sccsid @(#)n1.c	1.40 (gritter) 9/18/05
n2.c: * Sccsid @(#)n2.c	1.10 (gritter) 9/11/05
n3.c: * Sccsid @(#)n3.c	1.48 (gritter) 9/13/05
n4.c: * Sccsid @(#)n4.c	1.12 (gritter) 9/5/05
n5.c: * Sccsid @(#)n5.c	1.21 (gritter) 9/2/05
n7.c: * Sccsid @(#)n7.c	1.35 (gritter) 9/13/05
n8.c: * Sccsid @(#)n8.c	1.16 (gritter) 9/5/05
n9.c: * Sccsid @(#)n9.c	1.21 (gritter) 9/13/05
ni.c: * Sccsid @(#)ni.c	1.15 (gritter) 9/6/05
nii.c: * Sccsid @(#)nii.c	1.16 (gritter) 9/11/05
suftab.c: * Sccsid @(#)suftab.c	1.4 (gritter) 8/16/05
tdef.h: * Sccsid @(#)tdef.h	1.46 (gritter) 9/17/05
nroff.d/n10.c: * Sccsid @(#)n10.c	1.19 (gritter) 9/11/05
nroff.d/n6.c: * Sccsid @(#)n6.c	1.20 (gritter) 9/11/05
nroff.d/proto.h: * Sccsid @(#)proto.h	1.20 (gritter) 9/13/05
nroff.d/tw.h: * Sccsid @(#)tw.h	1.5 (gritter) 9/8/05
troff.d/afm.c: * Sccsid @(#)afm.c	1.25 (gritter) 9/16/05
troff.d/afm.h: * Sccsid @(#)afm.h	1.10 (gritter) 9/11/05
troff.d/dev.h: * Sccsid @(#)dev.h	1.7 (gritter) 9/8/05
troff.d/draw.c: * Sccsid @(#)draw.c	1.3 (gritter) 8/8/05
troff.d/makedev.c: * Sccsid @(#)makedev.c	1.10 (gritter) 9/8/05
troff.d/proto.h: * Sccsid @(#)proto.h	1.27 (gritter) 9/13/05
troff.d/t10.c: * Sccsid @(#)t10.c	1.47 (gritter) 9/13/05
troff.d/t6.c: * Sccsid @(#)t6.c	1.85 (gritter) 9/18/05
troff.d/troff.h: * Sccsid @(#)troff.h	1.13 (gritter) 9/11/05
troff.d/unimap.c: * Sccsid @(#)unimap.c	1.8 (gritter) 9/18/05
troff.d/unimap.h: * Sccsid @(#)unimap.h	1.1 (gritter) 8/17/05
libhnj/hnjalloc.c: * Sccsid @(#)hnjalloc.c	1.3 (gritter) 8/26/05
libhnj/hnjalloc.h: * Sccsid @(#)hnjalloc.h	1.2 (gritter) 8/25/05
libhnj/hyphen.c: * Sccsid @(#)hyphen.c	1.4 (gritter) 9/15/05
libhnj/hyphen.h: * Sccsid @(#)hyphen.h	1.3 (gritter) 8/25/05
*/
