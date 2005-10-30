#if __GNUC__ >= 3 && __GNUC_MINOR__ >= 4 || __GNUC__ >= 4
#define	USED	__attribute__ ((used))
#elif defined __GNUC__
#define	USED	__attribute__ ((unused))
#else
#define	USED
#endif
static const char sccsid[] USED = "@(#)/usr/ucblib/pm.sl	1.2 (gritter) 10/30/05";
/* SLIST */
/*
misc.h:	Sccsid @(#)misc.h	1.3 (gritter) 10/30/05	
page.h:	Sccsid @(#)page.h	1.3 (gritter) 10/30/05	
range.h:	Sccsid @(#)range.h	1.3 (gritter) 10/30/05	
slug.h:	Sccsid @(#)slug.h	1.3 (gritter) 10/30/05	
misc.cc:	Sccsid @(#)misc.cc	1.3 (gritter) 10/30/05	
page.cc:	Sccsid @(#)page.cc	1.4 (gritter) 10/30/05	
queue.cc:	Sccsid @(#)queue.cc	1.3 (gritter) 10/30/05	
range.cc:	Sccsid @(#)range.cc	1.3 (gritter) 10/30/05	
slug.cc:	Sccsid @(#)slug.cc	1.4 (gritter) 10/30/05	
*/
