#include "awk.h"
#if defined (SU3)
const char version[] = "@(#)awk_su3.sl  1.46 (gritter) 6/19/05";
int	posix = 1;
#elif defined (SUS)
const char version[] = "@(#)awk_sus.sl  1.46 (gritter) 6/19/05";
int	posix = 1;
#else
const char version[] = "@(#)nawk.sl  1.46 (gritter) 6/19/05";
int	posix = 0;
#endif
/*
awk.g.y:
	awk.g.y	1.8 (gritter) 12/4/04
awk.h:
	awk.h	1.23 (gritter) 12/25/04
awk.lx.l:
	awk.lx.l	1.12 (gritter) 6/18/05
b.c:
	b.c	1.6 (gritter) 5/15/04
lib.c:
	lib.c	1.25 (gritter) 6/19/05
main.c:
	main.c	1.14 (gritter) 12/19/04
maketab.c:
	maketab.c	1.11 (gritter) 12/4/04
nawk.1:
	nawk.1	1.19 (gritter) 5/15/04
parse.c:
	parse.c	1.7 (gritter) 12/4/04
run.c:
	run.c	1.32 (gritter) 6/19/05
tran.c:
	tran.c	1.16 (gritter) 2/4/05
*/
