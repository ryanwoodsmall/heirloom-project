#if __GNUC__ >= 3 && __GNUC_MINOR__ >= 4
#define	USED	__attribute__ ((used))
#elif defined __GNUC__
#define	USED	__attribute__ ((unused))
#else
#define	USED
#endif
static const char sccsid[] USED = "@(#)diff.sl	1.38 (gritter) 12/5/04";

/*
diff.h:
	diff.h	1.14 (gritter) 7/11/04
diff.c:
	diff.c	1.21 (gritter) 12/5/04
diffdir.c:
	diffdir.c	1.27 (gritter) 11/7/04
diffreg.c:
	diffreg.c	1.25 (gritter) 12/1/04
*/
