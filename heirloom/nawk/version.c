#include "awk.h"
#ifndef	SUS
const char version[] = "@(#)nawk.sl  1.40 (gritter) 12/25/04";
int	posix = 0;
#else
const char version[] = "@(#)awk_sus.sl  1.40 (gritter) 12/25/04";
int	posix = 1;
#endif
/*
awk.g.y:
	awk.g.y	1.8 (gritter) 12/4/04
awk.h:
	awk.h	1.23 (gritter) 12/25/04
awk.lx.l:
	awk.lx.l	1.10 (gritter) 12/4/04
b.c:
	b.c	1.6 (gritter) 5/15/04
lib.c:
	lib.c	1.24 (gritter) 12/25/04
main.c:
	main.c	1.14 (gritter) 12/19/04
maketab.c:
	maketab.c	1.11 (gritter) 12/4/04
nawk.1:
	nawk.1	1.19 (gritter) 5/15/04
parse.c:
	parse.c	1.7 (gritter) 12/4/04
run.c:
	run.c	1.28 (gritter) 12/25/04
tran.c:
	tran.c	1.15 (gritter) 12/25/04
*/
