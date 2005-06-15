/*
 * Sccsid @(#)wchar.h	1.1 (gritter) 6/15/05
 */

typedef	int	wint_t;

#ifndef	WEOF
#define	WEOF	((wint_t)-1)
#endif

int
mbtowc(wchar_t *pwc, const char *s, size_t n)
{
	if (s != 0) {
		if (n < 1)
			return -1;
		if (pwc != 0)
			*pwc = *s & 0377;
		return *s != '\0';
	} else
		return 0;
}

int
wctomb(char *s, wchar_t wchar)
{
	if (s != 0) {
		*s = wchar;
		return 1;
	} else
		return 0;
}

#ifdef	__dietlibc__
char *
setlocale(int category, const char *locale)
{
	return locale ? (char *)locale : "C";
}
#endif	/* __dietlibc__ */
