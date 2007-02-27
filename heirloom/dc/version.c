#if __GNUC__ >= 3 && __GNUC_MINOR__ >= 4
#define	USED	__attribute__ ((used))
#elif defined __GNUC__
#define	USED	__attribute__ ((unused))
#else
#define	USED
#endif
static const char sccsid[] USED = "@(#)dc.sl	2.7 (gritter) 11/21/04";

/*
dc.c:
	dc.c	1.18 (gritter) 11/21/04
dc.h:
	dc.h	1.8 (gritter) 5/1/04
*/
