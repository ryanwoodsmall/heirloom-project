#if __GNUC__ >= 3 && __GNUC_MINOR__ >= 4 || __GNUC__ >= 4
#define	USED	__attribute__ ((used))
#elif defined __GNUC__
#define	USED	__attribute__ ((unused))
#else
#define	USED
#endif
#ifdef	NROFF
static const char sccsid[] USED = "@(#)/usr/ucb/nroff.sl	1.72 (gritter) 8/20/05";
#else	/* !NROFF */
static const char sccsid[] USED = "@(#)/usr/ucb/troff.sl	1.72 (gritter) 8/20/05";
#endif	/* !NROFF */
/* SLIST */
/*
ext.h: * Sccsid @(#)ext.h	1.10 (gritter) 8/17/05
hytab.c: * Sccsid @(#)hytab.c	1.4 (gritter) 8/16/05
mapmalloc.c: *	Sccsid @(#)mapmalloc.c	2.1 (gritter) 8/18/05
n1.c: * Sccsid @(#)n1.c	1.20 (gritter) 8/20/05
n2.c: * Sccsid @(#)n2.c	1.7 (gritter) 8/18/05
n3.c: * Sccsid @(#)n3.c	1.22 (gritter) 8/20/05
n4.c: * Sccsid @(#)n4.c	1.7 (gritter) 8/15/05
n5.c: * Sccsid @(#)n5.c	1.10 (gritter) 8/15/05
n7.c: * Sccsid @(#)n7.c	1.9 (gritter) 8/16/05
n8.c: * Sccsid @(#)n8.c	1.5 (gritter) 8/16/05
n9.c: * Sccsid @(#)n9.c	1.10 (gritter) 8/18/05
ni.c: * Sccsid @(#)ni.c	1.11 (gritter) 8/16/05
nii.c: * Sccsid @(#)nii.c	1.6 (gritter) 8/15/05
suftab.c: * Sccsid @(#)suftab.c	1.4 (gritter) 8/16/05
tdef.h: * Sccsid @(#)tdef.h	1.21 (gritter) 8/20/05
nroff.d/n10.c: * Sccsid @(#)n10.c	1.17 (gritter) 8/16/05
nroff.d/n6.c: * Sccsid @(#)n6.c	1.10 (gritter) 8/20/05
nroff.d/proto.h: * Sccsid @(#)proto.h	1.7 (gritter) 8/20/05
nroff.d/tw.h: * Sccsid @(#)tw.h	1.4 (gritter) 8/8/05
troff.d/afm.c: * Sccsid @(#)afm.c	1.9 (gritter) 8/19/05
troff.d/afm.h: * Sccsid @(#)afm.h	1.4 (gritter) 8/17/05
troff.d/dev.h: * Sccsid @(#)dev.h	1.3 (gritter) 8/8/05
troff.d/draw.c: * Sccsid @(#)draw.c	1.3 (gritter) 8/8/05
troff.d/makedev.c: * Sccsid @(#)makedev.c	1.3 (gritter) 8/8/05
troff.d/proto.h: * Sccsid @(#)proto.h	1.11 (gritter) 8/20/05
troff.d/t10.c: * Sccsid @(#)t10.c	1.17 (gritter) 8/20/05
troff.d/t6.c: * Sccsid @(#)t6.c	1.31 (gritter) 8/20/05
troff.d/troff.h: * Sccsid @(#)troff.h	1.2 (gritter) 8/18/05
troff.d/unimap.c: * Sccsid @(#)unimap.c	1.6 (gritter) 8/19/05
troff.d/unimap.h: * Sccsid @(#)unimap.h	1.1 (gritter) 8/17/05
*/
