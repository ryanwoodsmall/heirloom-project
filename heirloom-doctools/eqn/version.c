#if __GNUC__ >= 3 && __GNUC_MINOR__ >= 4 || __GNUC__ >= 4
#define	USED	__attribute__ ((used))
#elif defined __GNUC__
#define	USED	__attribute__ ((unused))
#else
#define	USED
#endif
#ifdef	NEQN
static const char sccsid[] USED = "@(#)/usr/ucb/neqn.sl	1.9 (gritter) 9/6/05";
#else
static const char sccsid[] USED = "@(#)/usr/ucb/eqn.sl	1.9 (gritter) 9/6/05";
#endif
/* SLIST */
/*
diacrit.c: * Sccsid @(#)diacrit.c	1.3 (gritter) 8/12/05
e.h: * Sccsid @(#)e.h	1.5 (gritter) 8/13/05
e.y: * Sccsid @(#)e.y	1.5 (gritter) 8/13/05
eqnbox.c: * Sccsid @(#)eqnbox.c	1.3 (gritter) 8/12/05
font.c: * Sccsid @(#)font.c	1.3 (gritter) 8/12/05
fromto.c: * Sccsid @(#)fromto.c	1.3 (gritter) 8/12/05
funny.c: * Sccsid @(#)funny.c	1.4 (gritter) 8/13/05
glob.c: * Sccsid @(#)glob.c	1.6 (gritter) 9/6/05
integral.c: * Sccsid @(#)integral.c	1.3 (gritter) 8/12/05
io.c: * Sccsid @(#)io.c	1.8 (gritter) 8/30/05
lex.c: * Sccsid @(#)lex.c	1.4 (gritter) 8/13/05
lookup.c: * Sccsid @(#)lookup.c	1.4 (gritter) 8/13/05
mark.c: * Sccsid @(#)mark.c	1.3 (gritter) 8/12/05
matrix.c: * Sccsid @(#)matrix.c	1.3 (gritter) 8/12/05
move.c: * Sccsid @(#)move.c	1.3 (gritter) 8/12/05
over.c: * Sccsid @(#)over.c	1.3 (gritter) 8/12/05
paren.c: * Sccsid @(#)paren.c	1.3 (gritter) 8/12/05
pile.c: * Sccsid @(#)pile.c	1.3 (gritter) 8/12/05
shift.c: * Sccsid @(#)shift.c	1.3 (gritter) 8/12/05
size.c: * Sccsid @(#)size.c	1.3 (gritter) 8/12/05
sqrt.c: * Sccsid @(#)sqrt.c	1.3 (gritter) 8/12/05
text.c: * Sccsid @(#)text.c	1.5 (gritter) 9/2/05
*/
