#if __GNUC__ >= 3 && __GNUC_MINOR__ >= 4 || __GNUC__ >= 4
#define	USED	__attribute__ ((used))
#elif defined __GNUC__
#define	USED	__attribute__ ((unused))
#else
#define	USED
#endif
static const char sccsid[] USED = "@(#)diff.sl	1.48 (gritter) 6/26/05";
/* SLIST */
/*
diff.c:	Sccsid @(#)diff.c	1.24 (gritter) 3/27/05>	
diff.h:	Sccsid @(#)diff.h	1.15 (gritter) 3/26/05>	
diffdir.c:	Sccsid @(#)diffdir.c	1.28 (gritter) 3/27/05>	
diffreg.c:	Sccsid @(#)diffreg.c	1.29 (gritter) 3/27/05>	
*/
