/*
 * grep - search a file for a pattern
 *
 * Gunnar Ritter, Freiburg i. Br., Germany, April 2001.
 */
/*
 * Copyright (c) 2003 Gunnar Ritter
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute
 * it freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 *
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source distribution.
 */

#if __GNUC__ >= 3 && __GNUC_MINOR__ >= 4
#define	USED	__attribute__ ((used))
#elif defined __GNUC__
#define	USED	__attribute__ ((unused))
#else
#define	USED
#endif
static const char sccsid[] USED = "@(#)grep.sl	2.34 (gritter) 12/1/04";
/*
alloc.h:
	alloc.h	1.3 (gritter) 4/17/03
grep.h:
	grep.h	1.20 (gritter) 8/11/03
ac.c:
	fgrep.sl	2.8 (gritter) 7/16/04
alloc.c:
	alloc.c	1.3 (gritter) 4/17/03
fgrep.c:
	fgrep.c	1.10 (gritter) 11/21/04
ggrep.c:
	ggrep.c	1.22 (gritter) 11/21/04
grep.c:
	grep.c	1.43 (gritter) 12/1/04
plist.c:
	plist.c	1.21 (gritter) 12/1/04
rcomp.c:
	rcomp.c	1.24 (gritter) 1/10/04
sus.c:
	sus.c	1.19 (gritter) 11/21/04
svid3.c:
	svid3.c	1.7 (gritter) 4/17/03
egrep.y:
	egrep.sl	2.19 (gritter) 12/1/04
*/