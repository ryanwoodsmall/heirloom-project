#if __GNUC__ >= 3 && __GNUC_MINOR__ >= 4 || __GNUC__ >= 4
#define	USED	__attribute__ ((used))
#elif defined __GNUC__
#define	USED	__attribute__ ((unused))
#else
#define	USED
#endif
static const char sccsid[] USED = "@(#)/usr/ucb/grap.sl	1.2 (gritter) 10/18/05";
const char version[] = "version Dec 30, 1995	1.2 (gritter) 10/18/05";
/* SLIST */
/*
coord.c:	Sccsid @(#)coord.c	1.3 (gritter) 10/18/05	
for.c:	Sccsid @(#)for.c	1.3 (gritter) 10/18/05	
frame.c:	Sccsid @(#)frame.c	1.3 (gritter) 10/18/05	
grap.c:	Sccsid @(#)version.c	1.2 (gritter) 10/18/05	
grap.c: * Sccsid @(#)yaccpar	1.4 (gritter) 7/30/05
grap.h:	Sccsid @(#)grap.h	1.3 (gritter) 10/18/05	
grap.y:	Sccsid @(#)grap.y	1.3 (gritter) 10/18/05	
grapl.c:	Sccsid @(#)version.c	1.2 (gritter) 10/18/05	
grapl.c: * Sccsid @(#)ncform	1.3 (gritter) 6/18/05
grapl.l:	Sccsid @(#)grapl.l	1.3 (gritter) 10/18/05	
input.c:	Sccsid @(#)input.c	1.3 (gritter) 10/18/05	
label.c:	Sccsid @(#)label.c	1.2 (gritter) 10/18/05	
main.c:	Sccsid @(#)main.c	1.3 (gritter) 10/18/05	
misc.c:	Sccsid @(#)misc.c	1.3 (gritter) 10/18/05	
plot.c:	Sccsid @(#)plot.c	1.3 (gritter) 10/18/05	
print.c:	Sccsid @(#)print.c	1.3 (gritter) 10/18/05	
ticks.c:	Sccsid @(#)ticks.c	1.3 (gritter) 10/18/05	
grap.defines:#	Sccsid @(#)grap.defines	1.2 (gritter) 10/18/05
*/
