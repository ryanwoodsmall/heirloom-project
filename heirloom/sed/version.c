#if __GNUC__ >= 3 && __GNUC_MINOR__ >= 4
#define	USED	__attribute__ ((used))
#elif defined __GNUC__
#define	USED	__attribute__ ((unused))
#else
#define	USED
#endif
#if defined (SUS)
static const char sccsid[] USED = "@(#)sed_sus.sl	2.29 (gritter) 2/2/05";
#elif defined (S42)
static const char sccsid[] USED = "@(#)sed_s42.sl	2.29 (gritter) 2/2/05";
#else	/* !SUS, !S42 */
static const char sccsid[] USED = "@(#)sed.sl	2.29 (gritter) 2/2/05";
#endif	/* !SUS, !S42 */

/*
sed.h:
	sed.h	1.31 (gritter) 2/1/05
sed0.c:
	sed0.c	1.62 (gritter) 2/1/05
sed1.c:
	sed1.c	1.41 (gritter) 2/2/05
*/
