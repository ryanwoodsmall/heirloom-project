/*
 * shl - shell layer manager
 *
 * Gunnar Ritter, Freiburg i. Br., Germany, April 2001.
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

/*
 * Identification strings.
 */

#include	"shl.h"

#if __GNUC__ >= 3 && __GNUC_MINOR__ >= 4
#define	USED	__attribute__ ((used))
#elif defined __GNUC__
#define	USED	__attribute__ ((unused))
#else
#define	USED
#endif
static const char sccsid[] USED = "@(#)shl.sl	1.20 (gritter) 11/21/04";

char	*shl_version = "1.20";
char	*shl_date = "11/21/04";

/*
shl.h:
	shl.h	1.4 (gritter) 10/1/03
shl.c:
	shl.c	1.22 (gritter) 11/21/04
pslist.c:
	pslist.c	1.13 (gritter) 11/7/04
*/
