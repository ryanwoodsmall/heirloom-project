#if __GNUC__ >= 3 && __GNUC_MINOR__ >= 4 || __GNUC__ >= 4
#define	USED	__attribute__ ((used))
#elif defined __GNUC__
#define	USED	__attribute__ ((unused))
#else
#define	USED
#endif
#ifdef	XPG4
static const char sccsid[] USED = "@(#)m4_xpg4.sl	2.2 (gritter) 11/27/05";
#else
static const char sccsid[] USED = "@(#)m4.sl	2.2 (gritter) 11/27/05";
#endif
/* SLIST */
/*
m4.c: * Sccsid @(#)m4.c	1.3 (gritter) 10/29/05
m4.h: * Sccsid @(#)m4.h	1.3 (gritter) 10/29/05
m4ext.c: * Sccsid @(#)m4ext.c	1.3 (gritter) 10/29/05
m4macs.c: * Sccsid @(#)m4macs.c	1.4 (gritter) 11/27/05
m4y.c: * Sccsid @(#)m4y.y	1.3 (gritter) 10/29/05
m4y.c: * Sccsid @(#)yaccpar	1.5 (gritter) 11/26/05
m4y.y: * Sccsid @(#)m4y.y	1.3 (gritter) 10/29/05
m4y_xpg4.c: * Sccsid @(#)m4y_xpg4.y	1.3 (gritter) 10/29/05
m4y_xpg4.c: * Sccsid @(#)yaccpar	1.5 (gritter) 11/26/05
m4y_xpg4.y: * Sccsid @(#)m4y_xpg4.y	1.3 (gritter) 10/29/05
*/
