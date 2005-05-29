#if __GNUC__ >= 3 && __GNUC_MINOR__ >= 4 || __GNUC__ >= 4
#define	USED	__attribute__ ((used))
#elif defined __GNUC__
#define	USED	__attribute__ ((unused))
#else
#define	USED
#endif
static const char sccsid[] USED = "@(#)oawk.sl  2.26 (gritter) 5/29/05";
/*
awk.def:
	awk.def	1.18 (gritter) 10/5/04
awk.g.y:
	awk.g.y	1.5 (gritter) 7/24/03
awk.lx.l:
	awk.lx.l	1.11 (gritter) 7/24/03
b.c:
	b.c	1.10 (gritter) 3/31/03
freeze.c:
	freeze.c	1.3 (gritter) 3/31/03
lib.c:
	lib.c	1.15 (gritter) 4/8/03
main.c:
	main.c	1.11 (gritter) 12/12/04
oawk.1:
	oawk.1	1.12 (gritter) 4/30/03
parse.c:
	parse.c	1.5 (gritter) 4/21/04
proc.c:
	proc.c	1.5 (gritter) 4/4/04
run.c:
	run.c	1.19 (gritter) 10/13/04
tran.c:
	tran.c	1.7 (gritter) 3/31/03
*/
