/*
 * CDDL HEADER START
 *
 * The contents of this file are subject to the terms of the
 * Common Development and Distribution License, Version 1.0 only
 * (the "License").  You may not use this file except in compliance
 * with the License.
 *
 * You can obtain a copy of the license at usr/src/OPENSOLARIS.LICENSE
 * or http://www.opensolaris.org/os/licensing.
 * See the License for the specific language governing permissions
 * and limitations under the License.
 *
 * When distributing Covered Code, include this CDDL HEADER in each
 * file and include the License file at usr/src/OPENSOLARIS.LICENSE.
 * If applicable, add the following below this CDDL HEADER, with the
 * fields enclosed by brackets "[]" replaced with your own identifying
 * information: Portions Copyright [yyyy] [name of copyright owner]
 *
 * CDDL HEADER END
 */
/*
 * Copyright (c) 2005 Gunnar Ritter, Freiburg i. Br., Germany
 *
 * Sccsid @(#)main.c	1.2 (gritter) 6/14/05
 */
/*
 * Sccsid @(#)strsig.c	1.5 (gritter) 6/16/05
 */

#include <signal.h>
#include <stdlib.h>
#include "defs.h"

#define	MAXSIGVAL	32

static const struct sig_strlist {
	const int	sig_num;
	const char	*sig_str;
} sig_strs[] = {
	{ 0,		"EXIT"	},
	{ SIGHUP,	"HUP"	},
	{ SIGINT,	"INT"	},
	{ SIGQUIT,	"QUIT"	},
	{ SIGILL,	"ILL"	},
	{ SIGTRAP,	"TRAP"	},
	{ SIGABRT,	"ABRT"	},
#ifdef	SIGIOT
	{ SIGIOT,	"IOT"	},
#endif
#ifdef	SIGEMT
	{ SIGEMT,	"EMT"	},
#endif
#ifdef	SIGFPE
	{ SIGFPE,	"FPE"	},
#endif
#ifdef	SIGKILL
	{ SIGKILL,	"KILL"	},
#endif
#ifdef	SIGBUS
	{ SIGBUS,	"BUS"	},
#endif
#ifdef	SIGSEGV
	{ SIGSEGV,	"SEGV"	},
#endif
#ifdef	SIGSYS
	{ SIGSYS,	"SYS"	},
#endif
#ifdef	SIGPIPE
	{ SIGPIPE,	"PIPE"	},
#endif
#ifdef	SIGALRM
	{ SIGALRM,	"ALRM"	},
#endif
#ifdef	SIGTERM
	{ SIGTERM,	"TERM"	},
#endif
#ifdef	SIGUSR1
	{ SIGUSR1,	"USR1"	},
#endif
#ifdef	SIGUSR2
	{ SIGUSR2,	"USR2"	},
#endif
#ifdef	SIGCLD
	{ SIGCLD,	"CLD"	},
#endif
#ifdef	SIGCHLD
	{ SIGCHLD,	"CHLD"	},
#endif
#ifdef	SIGPWR
	{ SIGPWR,	"PWR"	},
#endif
#ifdef	SIGWINCH
	{ SIGWINCH,	"WINCH"	},
#endif
#ifdef	SIGURG
	{ SIGURG,	"URG"	},
#endif
#ifdef	SIGPOLL
	{ SIGPOLL,	"POLL"	},
#endif
#ifdef	SIGIO
	{ SIGIO,	"IO"	},
#endif
#ifdef	SIGSTOP
	{ SIGSTOP,	"STOP"	},
#endif
#ifdef	SIGTSTP
	{ SIGTSTP,	"TSTP"	},
#endif
#ifdef	SIGCONT
	{ SIGCONT,	"CONT"	},
#endif
#ifdef	SIGTTIN
	{ SIGTTIN,	"TTIN"	},
#endif
#ifdef	SIGTTOU
	{ SIGTTOU,	"TTOU"	},
#endif
#ifdef	SIGVTALRM
	{ SIGVTALRM,	"VTALRM"	},
#endif
#ifdef	SIGPROF
	{ SIGPROF,	"PROF"	},
#endif
#ifdef	SIGXCPU
	{ SIGXCPU,	"XCPU"	},
#endif
#ifdef	SIGXFSZ
	{ SIGXFSZ,	"XFSZ"	},
#endif
#ifdef	SIGWAITING
	{ SIGWAITING,	"WAITING"	},
#endif
#ifdef	SIGLWP
	{ SIGLWP,	"LWP"	},
#endif
#ifdef	SIGFREEZE
	{ SIGFREEZE,	"FREEZE"	},
#endif
#ifdef	SIGTHAW
	{ SIGTHAW,	"THAW"	},
#endif
#ifdef	SIGCANCEL
	{ SIGCANCEL,	"CANCEL"	},
#endif
#ifdef	SIGLOST
	{ SIGLOST,	"LOST"	},
#endif
#ifdef	SIGSTKFLT
	{ SIGSTKFLT,	"STKFLT"	},
#endif
#ifdef	SIGINFO
	{ SIGINFO,	"INFO"	},
#endif
#ifdef	SIG_2_STR_WITH_RT_SIGNALS
	{ SIGRTMIN,	"RTMIN"	},
	{ SIGRTMIN+1,	"RTMIN+1"	},
	{ SIGRTMIN+2,	"RTMIN+2"	},
	{ SIGRTMIN+3,	"RTMIN+3"	},
	{ SIGRTMAX-3,	"RTMAX-3"	},
	{ SIGRTMAX-2,	"RTMAX-2"	},
	{ SIGRTMAX-1,	"RTMAX-1"	},
	{ SIGRTMAX,	"RTMAX"	},
#endif	/* SIG_2_STR_WITH_RT_SIGNALS */
	{ -1,		NULL	}
};

int 
str_2_sig(const char *str, int *signum)
{
	register int	i;
	long	n;
	char	*x;

	for (i = 0; sig_strs[i].sig_str; i++)
		if (eq(str, sig_strs[i].sig_str))
			break;
	if (sig_strs[i].sig_str == NULL) {
		n = strtol(str, &x, 10);
		if (*x != '\0' || n < 0 || n >= i)
			return -1;
		*signum = n;
	} else
		*signum = sig_strs[i].sig_num;
	return 0;
}

int 
sig_2_str(int signum, char *str)
{
	register int	i;

	for (i = 0; sig_strs[i].sig_str; i++)
		if (sig_strs[i].sig_num == signum)
			break;
	if (sig_strs[i].sig_str == NULL)
		return -1;
	movstr(sig_strs[i].sig_str, str);
	return 0;
}

void 
init_sigval(void)
{
	extern void	(*(sigval[]))();

#ifdef	SIGHUP
	if (SIGHUP < MAXSIGVAL)
		sigval[SIGHUP] = done;
#endif
#ifdef	SIGINT
	if (SIGINT < MAXSIGVAL)
		sigval[SIGINT] = fault;
#endif
#ifdef	SIGQUIT
	if (SIGQUIT < MAXSIGVAL)
		sigval[SIGQUIT] = fault;
#endif
#ifdef	SIGILL
	if (SIGILL < MAXSIGVAL)
		sigval[SIGILL] = done;
#endif
#ifdef	SIGTRAP
	if (SIGTRAP < MAXSIGVAL)
		sigval[SIGTRAP] = done;
#endif
#ifdef	SIGIOT
	if (SIGIOT < MAXSIGVAL)
		sigval[SIGIOT] = done;
#endif
#ifdef	SIGBUS
	if (SIGBUS < MAXSIGVAL)
		sigval[SIGBUS] = done;
#endif
#ifdef	SIGFPE
	if (SIGFPE < MAXSIGVAL)
		sigval[SIGFPE] = done;
#endif
#ifdef	SIGKILL
	if (SIGKILL < MAXSIGVAL)
		sigval[SIGKILL] = 0;
#endif
#ifdef	SIGUSR1
	if (SIGUSR1 < MAXSIGVAL)
		sigval[SIGUSR1] = done;
#endif
#ifdef	SIGSEGV
	if (SIGSEGV < MAXSIGVAL)
		sigval[SIGSEGV] = done;
#endif
#ifdef	SIGEMT
	if (SIGEMT < MAXSIGVAL)
		sigval[SIGEMT] = done;
#endif
#ifdef	SIGUSR2
	if (SIGUSR2 < MAXSIGVAL)
		sigval[SIGUSR2] = done;
#endif
#ifdef	SIGPIPE
	if (SIGPIPE < MAXSIGVAL)
		sigval[SIGPIPE] = done;
#endif
#ifdef	SIGALRM
	if (SIGALRM < MAXSIGVAL)
		sigval[SIGALRM] = fault;
#endif
#ifdef	SIGTERM
	if (SIGTERM < MAXSIGVAL)
		sigval[SIGTERM] = fault;
#endif
#ifdef	SIGSTKFLT
	if (SIGSTKFLT < MAXSIGVAL)
		sigval[SIGSTKFLT] = done;
#endif
#ifdef	SIGCHLD
	if (SIGCHLD < MAXSIGVAL)
		sigval[SIGCHLD] = 0;
#endif
#ifdef	SIGCONT
	if (SIGCONT < MAXSIGVAL)
		sigval[SIGCONT] = 0;
#endif
#ifdef	SIGSTOP
	if (SIGSTOP < MAXSIGVAL)
		sigval[SIGSTOP] = 0;
#endif
#ifdef	SIGTSTP
	if (SIGTSTP < MAXSIGVAL)
		sigval[SIGTSTP] = 0;
#endif
#ifdef	SIGTTIN
	if (SIGTTIN < MAXSIGVAL)
		sigval[SIGTTIN] = 0;
#endif
#ifdef	SIGTTOU
	if (SIGTTOU < MAXSIGVAL)
		sigval[SIGTTOU] = 0;
#endif
#ifdef	SIGURG
	if (SIGURG < MAXSIGVAL)
		sigval[SIGURG] = 0;
#endif
#ifdef	SIGXCPU
	if (SIGXCPU < MAXSIGVAL)
		sigval[SIGXCPU] = done;
#endif
#ifdef	SIGXFSZ
	if (SIGXFSZ < MAXSIGVAL)
		sigval[SIGXFSZ] = done;
#endif
#ifdef	SIGVTALRM
	if (SIGVTALRM < MAXSIGVAL)
		sigval[SIGVTALRM] = done;
#endif
#ifdef	SIGPROF
	if (SIGPROF < MAXSIGVAL)
		sigval[SIGPROF] = done;
#endif
#ifdef	SIGWINCH
	if (SIGWINCH < MAXSIGVAL)
		sigval[SIGWINCH] = 0;
#endif
#ifdef	SIGPOLL
	if (SIGPOLL < MAXSIGVAL)
		sigval[SIGPOLL] = done;
#endif
#ifdef	SIGPWR
	if (SIGPWR < MAXSIGVAL)
		sigval[SIGPWR] = done;
#endif
#ifdef	SIGSYS
	if (SIGSYS < MAXSIGVAL)
		sigval[SIGSYS] = done;
#endif
}
