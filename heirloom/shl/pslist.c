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

/*	Sccsid @(#)pslist.c	1.13 (gritter) 11/7/04	*/

/*
 * System-dependent ps-like listing format for "layers" command.
 */

#if !defined (__FreeBSD__) && !defined (__hpux) && !defined (_AIX) && \
	!defined (__NetBSD__) && !defined (__OpenBSD__)

#include	"shl.h"
#include	<sys/stat.h>
#include	<unistd.h>
#include	<stdio.h>
#include	<string.h>
#include	<limits.h>
#include	<stdlib.h>
#include	<time.h>
#include	<dirent.h>
#include	<ctype.h>
#include	<pwd.h>
#include	<langinfo.h>
#ifdef	__linux__
#include	<alloca.h>
#endif

#ifndef	__linux__
#ifdef	sun
#define	_STRUCTURED_PROC	1
#endif	/* sun */
#include	<sys/procfs.h>
#include	<utmpx.h>
#endif	/* !__linux__ */

static int	hz;		/* clock ticks per second */
static time_t	now;		/* current time */

struct colontm {
	unsigned	c_sec;		/* seconds */
	unsigned	c_min;		/* minutes */
	unsigned	c_hour;		/* hours */
};

struct pslot {
	struct pslot	*p_prev;	/* previous slot in process table */
	struct pslot	*p_next;	/* next slot in process table */
	long		p_start;	/* start time in jiffies after boot */
	long		p_utime;	/* jiffies in user mode */
	long		p_stime;	/* jiffies in kernel mode */
	char		p_STIME[32];	/* STIME string */
	char		p_TIME[32];		/* TIME string */
	uid_t		p_uid;		/* real uid owning the process */
	pid_t		p_pid;		/* process id */
	pid_t		p_ppid;		/* parent's process id */
	char		p_cmdline[80];	/* command line */
	char		p_cpu;		/* cpu usage for scheduling */
	int		p_zombie;	/* true if zombie */
};

#ifdef	__linux__
enum	valtype {
	VT_CHAR,
	VT_INT,
	VT_UINT,
	VT_LONG,
	VT_ULONG
};

union	value {
	char			v_char;
	int			v_int;
	unsigned int		v_uint;
	long			v_long;
	unsigned long		v_ulong;
};

union value *
getval(char **listp, enum valtype type, int separator)
{
	char	*buf;
	static union value	v;
	const char	*cp, *op;
	char	*cq, *x;

	if (**listp == '\0')
		return NULL;
	op = *listp;
	while (**listp != '\0') {
		if (separator == ' ' ? isspace(**listp) : **listp == separator)
			break;
		(*listp)++;
	}
	buf = alloca(*listp - op + 1);
	for (cp = op, cq = buf; cp < *listp; cp++, cq++)
		*cq = *cp;
	*cq = '\0';
	if (**listp) {
		while (separator == ' ' ?
				isspace(**listp) : **listp == separator)
			(*listp)++;
	}
	switch (type) {
	case VT_CHAR:
		if (buf[1] != '\0')
			return NULL;
		v.v_char = buf[0];
		break;
	case VT_INT:
		v.v_int = strtol(buf, &x, 10);
		if (*x != '\0')
			return NULL;
		break;
	case VT_UINT:
		v.v_uint = strtoul(buf, &x, 10);
		if (*x != '\0')
			return NULL;
		break;
	case VT_LONG:
		v.v_long = strtol(buf, &x, 10);
		if (*x != '\0')
			return NULL;
		break;
	case VT_ULONG:
		v.v_ulong = strtoul(buf, &x, 10);
		if (*x != '\0')
			return NULL;
		break;
	}
	return &v;
}
#endif	/* __linux__ */

/*
 * Free an entire process slot chain.
 */
static void
freeprocs(struct pslot *p)
{
	while (p->p_next) {
		if (p->p_prev)
			free(p->p_prev);
		p = p->p_next;
	}
	free(p);
}

/*
 * Fill a user name into the passed buffer, or the user number.
 */
static void
getname(uid_t uid, char *str, size_t sz)
{
	struct passwd *pwd;

	if ((pwd = getpwuid(uid)) != NULL) {
		strncpy(str, pwd->pw_name, sz);
		str[sz - 1] = '\0';
	} else
		snprintf(str, sz, "%lu", (long)uid);
}

/*
 * List a process slot chain.
 */
static void
listprocs(struct pslot *p, char *line, void (*out)(const char *, ...))
{
	char myname[16], othername[16], *name;

	out("     UID   PID  PPID  C    STIME TTY      TIME %s",
			sysv3 ? "COMMAND" : "COMD");
	getname(myuid, myname, sizeof myname);
	while (p->p_next)
		p = p->p_next;
	while (p) {
		if (p->p_uid != myuid) {
			getname(p->p_uid, othername, sizeof othername);
			name = othername;
		} else
			name = myname;
		out("%8s %5lu %5lu %2d %8s %-7s %5s %s",
				name,
				(long)p->p_pid,
				(long)p->p_ppid,
				p->p_cpu,
				p->p_zombie ? "" : p->p_STIME,
				p->p_zombie ? "" : line + 5,
				p->p_TIME,
				p->p_zombie ? "<defunct>" : p->p_cmdline);
		p = p->p_prev;
	}
}

/*
 * Find the device id for a tty line.
 */
static dev_t
lineno(char *line)
{
	struct stat st;

	if (stat(line, &st) < 0)
		return 0;
	return st.st_rdev;
}

/*
 * Time of system boot since the epoch.
 */
#ifdef	__linux__
static time_t
sysup(void)
{
	FILE *fp;
	char	buf[32];
	char	*cp;
	union value	*v;
	time_t s = 0;

	if ((fp = fopen("/proc/uptime", "r")) == NULL)
		return 0;
	if (fread(buf, 1, sizeof buf, fp) > 0) {
		cp = buf;
		if ((v = getval(&cp, VT_ULONG, '.')) != NULL) {
			s = now - v->v_ulong;
		}
	}
	fclose(fp);
	return s;
}
#endif	/* __linux__ */

/*
 * Convert jiffies to time_t.
 */
static time_t
jifftime(int jiffies)
{
	return jiffies / hz;
}

/*
 * Generate p_STIME.
 */
static void
makeSTIME(struct pslot *p)
{
	struct tm *tm;
	time_t t;

#ifdef	__linux__
	t = sysup() + jifftime(p->p_start);
#else
	t = p->p_start;
#endif
	tm = localtime(&t);
	if (now - t > 86400) {
		nl_item	val;

		switch (tm->tm_mon) {
		case 0:		val = ABMON_1;	break;
		case 1:		val = ABMON_2;	break;
		case 2:		val = ABMON_3;	break;
		case 3:		val = ABMON_4;	break;
		case 4:		val = ABMON_5;	break;
		case 5:		val = ABMON_6;	break;
		case 6:		val = ABMON_7;	break;
		case 7:		val = ABMON_8;	break;
		case 8:		val = ABMON_9;	break;
		case 9:		val = ABMON_10;	break;
		case 10:	val = ABMON_11;	break;
		case 11:	val = ABMON_12;	break;
		default:	val = ABMON_12;	/* won't happen anyway */
		}
		snprintf(p->p_STIME, sizeof p->p_STIME, "  %s %02u",
				nl_langinfo(val), tm->tm_mday);
	} else
		snprintf(p->p_STIME, sizeof p->p_STIME, "%02u:%02u:%02u",
				tm->tm_hour, tm->tm_min, tm->tm_sec);
}

/*
 * Generate p_TIME.
 */
static void
makeTIME(struct pslot *p)
{
	time_t t;

	t = jifftime(p->p_utime) + jifftime(p->p_stime);
	snprintf(p->p_TIME, sizeof p->p_TIME, "%2lu:%02lu",
			(long)t / 60, (long)t % 60);
}

/*
 * Read an entry from /proc into a process slot if it has the right tty.
 */

#define	GETVAL(a)		if ((v = getval(&cp, (a), ' ')) == NULL) \
					return NULL
static struct pslot *
readproc(char *pdir, dev_t dev)
#ifdef	__linux__
{
	static char	*buf;
	static size_t	buflen;
	char	state;
	char fn[256];
	char	line[LINE_MAX];
	FILE *fp;
	union value	*v;
	struct pslot p, *pp;
	char *cp, *cq, *ce, *comm;
	size_t	sz, sc;
	int c;

	memset(&p, 0, sizeof p);
	snprintf(fn, sizeof fn, "%s/stat", pdir);
	if ((fp = fopen(fn, "r")) == NULL)
		return NULL;
	for (cp = buf; ;) {
		const unsigned	chunk = 32;

		if (buflen < (sz = cp - buf + chunk)) {
			sc = cp - buf;
			buf = srealloc(buf, buflen = sz);
			cp = &buf[sc];
		}
		if ((sz = fread(cp, 1, chunk, fp)) < chunk) {
			ce = &cp[sz - 1];
			break;
		}
		cp += chunk;
	}
	fclose(fp);
	if (*ce != '\n')
		return NULL;
	*ce-- = '\0';
	cp = buf;
	GETVAL(VT_INT);
	p.p_pid = v->v_int;
	if (*cp++ != '(')
		return NULL;
	comm = cp;
	for (cq = ce; cq >= cp && *cq != ')'; cq--);
	if (cq < cp)
		return NULL;
	*cq = '\0';
	cp = &cq[1];
	while (isspace(*cp))
		cp++;
	GETVAL(VT_CHAR);
	if ((state = v->v_char) == 'Z')
		p.p_zombie = 1;
	GETVAL(VT_INT);
	p.p_ppid = v->v_int;
	GETVAL(VT_INT);
	/* pgrp unused */
	GETVAL(VT_INT);
	/* session unused */
	GETVAL(VT_INT);
	if (dev != v->v_int)
		return NULL;
	GETVAL(VT_INT);
	/* tty_pgrp unused */
	GETVAL(VT_ULONG);
	/* flags unused */
	GETVAL(VT_ULONG);
	/* min_flt unused */
	GETVAL(VT_ULONG);
	/* cmin_flt unused */
	GETVAL(VT_ULONG);
	/* maj_flt unused */
	GETVAL(VT_ULONG);
	/* cmaj_flut unused */
	GETVAL(VT_ULONG);
	p.p_utime = v->v_ulong;
	GETVAL(VT_ULONG);
	p.p_stime = v->v_ulong;
	GETVAL(VT_ULONG);
	/* cutime unused */
	GETVAL(VT_ULONG);
	/* cstime unused */
	GETVAL(VT_LONG);
	/* priority unused */
	GETVAL(VT_LONG);
	/* nice unused */
	GETVAL(VT_LONG);
	/* timeout unused */
	GETVAL(VT_LONG);
	/* it_real_value unused */
	GETVAL(VT_ULONG);
	p.p_start = v->v_ulong;
	snprintf(fn, sizeof fn, "%s/cmdline", pdir);
	if ((fp = fopen(fn, "r")) != NULL) {
		int	hadzero = 0;

		cp = p.p_cmdline;
		ce = cp + sizeof p.p_cmdline - 1;
		while (cp < ce && (c = getc(fp)) != EOF) {
			if (c != '\0') {
				if (hadzero) {
					*cp++ = ' ';
					if (cp == ce)
						break;
					hadzero = 0;
				}
				*cp++ = c;
			} else
				hadzero = 1;
		}
		*cp = '\0';
		fclose(fp);
	}
	if (*p.p_cmdline == '\0')
		snprintf(p.p_cmdline, sizeof p.p_cmdline, "[ %.8s ]", comm);
	snprintf(fn, sizeof fn, "%s/status", pdir);
	if ((fp = fopen(fn, "r")) == NULL)
		return NULL;
	c = 0;
	while (fgets(line, sizeof line, fp) != NULL) {
		if (strncmp(line, "Uid:", 4) == 0) {
			cp = &line[4];
			while (isspace(*cp))
				cp++;
			if ((v = getval(&cp, VT_INT, ' ')) == NULL) {
				fclose(fp);
				return NULL;
			}
			p.p_uid = v->v_int;
			c++;
		}
	}
	fclose(fp);
	if (c != 1)
		return NULL;
	pp = smalloc(sizeof *pp);
	memcpy(pp, &p, sizeof *pp);
	makeSTIME(pp);
	makeTIME(pp);
	return pp;
}
#else	/* !__linux__ */
{
	char fn[256];
	FILE *fp;
	struct pslot *p;
	struct psinfo pi;
	struct pstatus ps;

	snprintf(fn, sizeof fn, "%s/psinfo", pdir);
	if ((fp = fopen(fn, "r")) == NULL)
		return NULL;
	if ((fread(&pi, 1, sizeof pi, fp)) != sizeof(pi)
			|| pi.pr_ttydev != dev) {
		fclose(fp);
		return NULL;
	}
	p = (struct pslot *)smalloc(sizeof *p);
	if (pi.pr_nlwp == 0)
		p->p_zombie = 1;
	p->p_start = pi.pr_start.tv_sec;
	p->p_uid = pi.pr_uid;
	p->p_pid = pi.pr_pid;
	p->p_ppid = pi.pr_ppid;
	strncpy(p->p_cmdline, pi.pr_psargs, sizeof p->p_cmdline);
	p->p_cmdline[sizeof p->p_cmdline - 1] = '\0';
	fclose(fp);
	snprintf(fn, sizeof fn, "%s/status", pdir);
	if ((fp = fopen(fn, "r")) == NULL) {
		free(p);
		return NULL;
	}
	if ((fread(&ps, 1, sizeof ps, fp)) != sizeof(ps)) {
		free(p);
		fclose(fp);
		return NULL;
	}
	p->p_utime = ps.pr_utime.tv_sec;
	p->p_stime = ps.pr_stime.tv_sec;
	fclose(fp);
	makeSTIME(p);
	makeTIME(p);
	return p;
}
#endif	/* !__linux__ */

/*
 * Take an entry in /proc and return a process slot if advisable.
 */
static struct pslot *
getproc(char *pname, dev_t dev)
{
	struct stat st;
	char *ep;
	char fn[_POSIX_PATH_MAX];

	snprintf(fn, sizeof fn, "/proc/%s", pname);
	if (lstat(fn, &st) < 0)
		return NULL;
	if (!S_ISDIR(st.st_mode))
		return NULL;
	strtol(pname, &ep, 10);
	if (*ep != '\0')
		return NULL;
	return readproc(fn, dev);
}

/*
 * Find all processes belonging to device dev.
 */
static struct pslot *
findprocs(dev_t dev)
{
	DIR *dir;
	struct dirent *dent;
	struct pslot *p, *p0 = NULL;

	if ((dir = opendir("/proc")) == NULL)
		return NULL;
	while ((dent = readdir(dir)) != NULL) {
		if ((p = getproc(dent->d_name, dev)) != NULL) {
			if (p0) {
				p0->p_prev = p;
				p->p_next = p0;
			} else
				p->p_next = NULL;
			p->p_prev = NULL;
			p0 = p;
		}
	}
	closedir(dir);
	return p0;
}

/*
 * External interface. Use out() for all processes in layer.
 */
int
pslist(struct layer *l, void (*out)(const char *, ...))
{
	struct pslot *p;

	if (hz == 0)
		hz = sysconf(_SC_CLK_TCK);
	time(&now);
	if ((p = findprocs(lineno(l->l_line))) == NULL)
		return 1;
	listprocs(p, l->l_line, out);
	freeprocs(p);
	return 0;
}

#endif	/* !__FreeBSD__, !__hpux, !_AIX, !__NetBSD__, !__OpenBSD__ */
