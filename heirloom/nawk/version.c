#include "awk.h"
#ifndef	SUS
const char version[] = "@(#)nawk.sl  1.37 (gritter) 11/21/04";
int	posix = 0;
#include "pfmt.h"
#include <ctype.h>
int
vpfmt(FILE *stream, long flags, const char *fmt, va_list ap)
{
	extern char	*pfmt_label__;
	int	n = 0;

	if ((flags & MM_NOGET) == 0) {
		if (*fmt == ':') {
			do
				fmt++;
			while (*fmt != ':');
			fmt++;
		}
	}
	if ((flags & MM_NOSTD) == 0)
		n += fprintf(stream, "%s: ", pfmt_label__);
	if ((flags & MM_ACTION) == 0 && isupper(*fmt&0377))
		n += fprintf(stream, "%c", tolower(*fmt++&0377));
	n += vfprintf(stream, fmt, ap);
	return n;
}
#else
const char version[] = "@(#)awk_sus.sl  1.37 (gritter) 11/21/04";
int	posix = 1;
#endif
/*
awk.g.y:
	awk.g.y	1.7 (gritter) 4/16/04
awk.h:
	awk.h	1.21 (gritter) 11/21/04
awk.lx.l:
	awk.lx.l	1.9 (gritter) 7/13/04
b.c:
	b.c	1.6 (gritter) 5/15/04
lib.c:
	lib.c	1.21 (gritter) 10/13/04
main.c:
	main.c	1.12 (gritter) 5/15/04
maketab.c:
	maketab.c	1.10 (gritter) 4/26/04
parse.c:
	parse.c	1.6 (gritter) 4/8/03
run.c:
	run.c	1.25 (gritter) 7/16/04
tran.c:
	tran.c	1.13 (gritter) 7/13/04
*/
