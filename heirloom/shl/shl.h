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

/*	Sccsid @(#)shl.h	1.4 (gritter) 10/1/03	*/

#include	<sys/types.h>
#include	<termios.h>

/*
 * Structure for a single shell layer.
 */
struct layer {
	struct layer	*l_prev;	/* previous node in layer list */
	struct layer	*l_next;	/* next node in layer list */
	struct termios	l_tio;		/* termios struct of layer */
	char		l_name[9];	/* name of layer */
	char		l_line[64];	/* device name for tty */
	pid_t		l_pid;		/* pid/pgid of layer */
	int		l_pty;		/* pty master */
	int		l_blk;		/* output is blocked */
};

extern void	*smalloc(size_t);
extern void	*srealloc(void *, size_t);
extern int	pslist(struct layer *, void (*)(const char *, ...));

extern char	*shl_version;
extern char	*shl_date;
extern uid_t	myuid;
extern int	sysv3;
