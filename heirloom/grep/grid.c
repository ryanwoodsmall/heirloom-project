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
static const char sccsid[] USED = "@(#)grep.sl	2.47 (gritter) 2/6/05";
/*
ac.c:
	fgrep.sl	2.9 (gritter) 12/19/04
alloc.c:
	alloc.c	1.3 (gritter) 4/17/03
alloc.h:
	alloc.h	1.3 (gritter) 4/17/03
egrep.y:
	egrep.sl	2.21 (gritter) 12/17/04
fgrep.c:
	fgrep.c	1.12 (gritter) 12/17/04
ggrep.c:
	ggrep.c	1.25 (gritter) 12/17/04
grep.c:
	grep.c	1.52 (gritter) 2/5/05
grep.h:
	grep.h	1.22 (gritter) 12/8/04
plist.c:
	plist.c	1.22 (gritter) 12/8/04
rcomp.c:
	rcomp.c	1.27 (gritter) 2/6/05
sus.c:
	sus.c	1.23 (gritter) 2/6/05
svid3.c:
	svid3.c	1.7 (gritter) 4/17/03
*/
