#if __GNUC__ >= 3 && __GNUC_MINOR__ >= 4 || __GNUC__ >= 4
#define	USED	__attribute__ ((used))
#elif defined __GNUC__
#define	USED	__attribute__ ((unused))
#else
#define	USED
#endif
static const char sccsid[] USED = "@(#)/usr/ucb/pic.sl	1.8 (gritter) 10/25/05";
const char version[] = "version July 5, 1993	1.8 (gritter) 10/25/05";
/* SLIST */
/*
arcgen.c:	Sccsid @(#)arcgen.c	1.3 (gritter) 10/18/05	
blockgen.c:	Sccsid @(#)blockgen.c	1.3 (gritter) 10/18/05	
boxgen.c:	Sccsid @(#)boxgen.c	1.2 (gritter) 10/18/05	
circgen.c:	Sccsid @(#)circgen.c	1.3 (gritter) 10/18/05	
for.c:	Sccsid @(#)for.c	1.3 (gritter) 10/18/05	
input.c:	Sccsid @(#)input.c	1.5 (gritter) 10/18/05	
linegen.c:	Sccsid @(#)linegen.c	1.3 (gritter) 10/25/05	
main.c:	Sccsid @(#)main.c	1.5 (gritter) 10/25/05	
misc.c:	Sccsid @(#)misc.c	1.3 (gritter) 10/18/05	
movegen.c:	Sccsid @(#)movegen.c	1.2 (gritter) 10/18/05	
pic.h:	Sccsid @(#)pic.h	1.5 (gritter) 10/25/05	
picl.c:	Sccsid @(#)picl.l	1.5 (gritter) 10/25/05	
picl.c: * Sccsid @(#)ncform	1.3 (gritter) 6/18/05
picl.l:	Sccsid @(#)picl.l	1.5 (gritter) 10/25/05	
picy.c:	Sccsid @(#)picy.y	1.3 (gritter) 10/18/05	
picy.c: * Sccsid @(#)yaccpar	1.4 (gritter) 7/30/05
picy.y:	Sccsid @(#)picy.y	1.3 (gritter) 10/18/05	
pltroff.c:	Sccsid @(#)pltroff.c	1.2 (gritter) 10/18/05	
print.c:	Sccsid @(#)print.c	1.3 (gritter) 10/18/05	
symtab.c:	Sccsid @(#)symtab.c	1.3 (gritter) 10/18/05	
textgen.c:	Sccsid @(#)textgen.c	1.2 (gritter) 10/18/05	
*/
