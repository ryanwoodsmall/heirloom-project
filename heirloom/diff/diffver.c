#if __GNUC__ >= 3 && __GNUC_MINOR__ >= 4
#define	USED	__attribute__ ((used))
#elif defined __GNUC__
#define	USED	__attribute__ ((unused))
#else
#define	USED
#endif
static const char sccsid[] USED = "@(#)diff.sl	1.43 (gritter) 3/27/05";

/*
diff.h:
	diff.h	1.15 (gritter) 3/26/05
diff.c:
	diff.c	1.22 (gritter) 3/26/05
diffdir.c:
	diffdir.c	1.28 (gritter) 3/27/05
diffreg.c:
	diffreg.c	1.29 (gritter) 3/27/05
*/
