#if __GNUC__ >= 3 && __GNUC_MINOR__ >= 4
#define	USED	__attribute__ ((used))
#elif defined __GNUC__
#define	USED	__attribute__ ((unused))
#else
#define	USED
#endif
#if defined (SUS)
static const char sccsid[] USED = "@(#)sed_sus.sl	2.25 (gritter) 10/13/04";
#elif defined (S42)
static const char sccsid[] USED = "@(#)sed_s42.sl	2.25 (gritter) 10/13/04";
#else	/* !SUS, !S42 */
static const char sccsid[] USED = "@(#)sed.sl	2.25 (gritter) 10/13/04";
#endif	/* !SUS, !S42 */

/*
sed.h:
	sed.h	1.30 (gritter) 7/24/04
sed0.c:
	sed0.c	1.59 (gritter) 7/24/04
sed1.c:
	sed1.c	1.40 (gritter) 10/13/04
*/