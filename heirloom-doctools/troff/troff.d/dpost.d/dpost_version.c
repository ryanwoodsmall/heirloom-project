#if __GNUC__ >= 3 && __GNUC_MINOR__ >= 4 || __GNUC__ >= 4
#define	USED	__attribute__ ((used))
#elif defined __GNUC__
#define	USED	__attribute__ ((unused))
#else
#define	USED
#endif
static const char sccsid[] USED = "@(#)dpost.sl	1.69 (gritter) 9/7/05";
const char creator[] = "Heirloom Documentation Tools - dpost 1.69 (gritter) 9/7/05";
/* SLIST */
/*
color.c: * Sccsid @(#)color.c	1.3 (gritter) 8/9/05
comments.h: * Sccsid @(#)comments.h	1.5 (gritter) 8/23/05
dpost.c: * Sccsid @(#)dpost.c	1.57 (gritter) 9/7/05
dpost.h: * Sccsid @(#)dpost.h	1.3 (gritter) 8/9/05
dpost_afm.c: * Sccsid @(#)dpost_afm.c	1.1 (gritter) 8/20/05
dpost_draw.c: * Sccsid @(#)draw.c	1.3 (gritter) 8/9/05
dpost_makedev.c: * Sccsid @(#)dpost_makedev.c	1.1 (gritter) 9/4/05
ext.h: * Sccsid @(#)ext.h	1.3 (gritter) 8/9/05
gen.h: * Sccsid @(#)gen.h	1.8 (gritter) 8/23/05
getopt.c: * Sccsid @(#)getopt.c	1.8 (gritter) 8/2/05
glob.c: * Sccsid @(#)glob.c	1.3 (gritter) 8/9/05
misc.c: * Sccsid @(#)misc.c	1.5 (gritter) 8/23/05
path.h: * Sccsid @(#)path.h	1.3 (gritter) 8/9/05
pictures.c: * Sccsid @(#)pictures.c	1.3 (gritter) 8/9/05
ps_include.c: * Sccsid @(#)ps_include.c	1.5 (gritter) 8/13/05
request.c: * Sccsid @(#)request.c	1.3 (gritter) 8/9/05
request.h: * Sccsid @(#)request.h	1.3 (gritter) 8/9/05
../afm.c: * Sccsid @(#)afm.c	1.21 (gritter) 8/29/05
../afm.h: * Sccsid @(#)afm.h	1.9 (gritter) 8/26/05
../makedev.c: * Sccsid @(#)makedev.c	1.9 (gritter) 9/5/05
../dev.h: * Sccsid @(#)dev.h	1.6 (gritter) 9/4/05
*/
