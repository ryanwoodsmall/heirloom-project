/*	Derived from 4.3BSD mail.c	4.25 (Berkeley) 5/1/85	*/
/*
 * Copyright (c) 1980, 1993
 * 	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 * 	This product includes software developed by the University of
 * 	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *
 * Copyright(C) Caldera International Inc. 2001-2002. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *   Redistributions of source code and documentation must retain the
 *    above copyright notice, this list of conditions and the following
 *    disclaimer.
 *   Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *   All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *      This product includes software developed or owned by Caldera
 *      International, Inc.
 *   Neither the name of Caldera International, Inc. nor the names of
 *    other contributors may be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * USE OF THE SOFTWARE PROVIDED FOR UNDER THIS LICENSE BY CALDERA
 * INTERNATIONAL, INC. AND CONTRIBUTORS ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL CALDERA INTERNATIONAL, INC. BE
 * LIABLE FOR ANY DIRECT, INDIRECT INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#if __GNUC__ >= 3 && __GNUC_MINOR__ >= 4
#define	USED	__attribute__ ((used))
#elif defined __GNUC__
#define	USED	__attribute__ ((unused))
#else
#define	USED
#endif
static const char sccsid [] USED = "@(#)mail.sl       1.23 (gritter) 11/7/04";

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <ctype.h>
#include <stdio.h>
#include <pwd.h>
#include <utmp.h>
#include <signal.h>
#include "sigset.h"
#include <setjmp.h>
#include <string.h>
#include <time.h>
#include <strings.h>
#include <unistd.h>
#include <stdlib.h>
#include <libgen.h>
#include <limits.h>
#include <stdarg.h>

#if !defined (__FreeBSD__) && !defined (__NetBSD__) && !defined (__OpenBSD__)
#define SENDMAIL	"/usr/lib/sendmail"
#else	/* __FreeBSD__, __NetBSD__, __OpenBSD__ */
#include <paths.h>
#define	SENDMAIL	_PATH_SENDMAIL
#endif	/* __FreeBSD__, __NetBSD__, __OpenBSD__ */

#define	CLOBBGRD(a)	(void)(&(a))

	/* copylet flags */
#define REMOTE		1		/* remote mail, add rmtmsg */
#define ORDINARY	2
#define ZAP		3		/* zap header and trailing empty line */
#define	FORWARD		4

#define	LSIZE		128		/* initial line size */
#define	MAILMODE	0600		/* mode of created mail */

#ifdef	__GLIBC__
#ifdef	_IO_getc_unlocked
#undef	getc
#define	getc(c)		_IO_getc_unlocked(c)
#endif	/* _IO_getc_unlocked */
#ifdef	_IO_putc_unlocked
#undef	putc
#define	putc(c, f)	_IO_putc_unlocked(c, f)
#endif	/* _IO_putc_unlocked */
#endif	/* __GLIBC__ */

static char	*line;
static size_t	lsize;
static char	*resp;
static size_t	rsize;
struct let {
	long	adr;
	char	change;
} *let;
static int	nlet	= 0;
static char	*lfil;
static size_t	lfilsz;
static time_t	iop;
static char	lettmp[] = "/tmp/maXXXXXX";
static const char	maildir[] = "/var/mail/";
static char	*mailfile;
static char	*locktfile;
static const char	dead[] = "dead.letter";
static const char	forwmsg[] = " forwarded\n";
static FILE	*tmpf;
static FILE	*malf;
static char	*my_name;
static char	*my_home;
static int	error;
static int	changed;
static int	forward;
static const char	from[] = "From ";
static int	flge;
int	flgf;
static int	flgp;
static int	delflg = 1;
static jmp_buf	sjbuf;
static int	rmail;
static int	composing;
static char	*progname;

extern int	locked;
extern char	locktmp[];
extern int	lock(const char *, const char *);
extern void	unlock(void);

static void setsig(int, void (*)(int));
static int any(register int, register const char *);
static void printmail(int, char **);
static void copyback(void);
static void copymt(FILE *, FILE *);
static void copylet(int, FILE *, int);
static int isfrom(register const char *);
static void bulkmail(int, char **);
static void savedead(void);
static int sendrmt(int, const char *);
static void usage(void);
static void notifybiff(const char *);
static int sendmail(int, const char *);
static void delex(int);
static void done(void);
static void cat(char *, const char *, const char *);
static char *getarg(char **, size_t *, register char *);
static int safefile(const char *);
static void panic(const char *, ...);
static void *srealloc(void *, size_t);
static void *smalloc(size_t);
static char *fgetline(char **, size_t *, size_t *, FILE *);
static void setnlet(int);

int
main(int argc, char **argv)
{
	register int i;
	struct passwd *pwent;

	progname = basename(argv[0]);
	my_name = getlogin();
	if (my_name == NULL || *my_name == '\0') {
		pwent = getpwuid(getuid());
		if (pwent==NULL)
			my_name = "???";
		else {
			my_name = strdup(pwent->pw_name);
			my_home = strdup(pwent->pw_dir);
		}
	}
	else {
		pwent = getpwnam(my_name);
		if ( getuid() != pwent->pw_uid) {
			pwent = getpwuid(getuid());
			my_name = strdup(pwent->pw_name);
		}
		if (pwent != NULL)
			my_home = strdup(pwent->pw_dir);
	}
	if (setjmp(sjbuf))
		done();
	setsig(SIGHUP, delex);
	setsig(SIGINT, delex);
	setsig(SIGQUIT, delex);
	setsig(SIGPIPE, delex);
	setsig(SIGALRM, delex);
	setsig(SIGTERM, delex);
	setsig(SIGXCPU, delex);
	setsig(SIGXFSZ, delex);
	i = mkstemp(lettmp);
	unlink(lettmp);
	tmpf = fdopen(i, "r+w");
	if (i < 0 || tmpf == NULL)
		panic("mail: %s: cannot open for writing", lettmp);
	/*
	 * This protects against others reading mail from temp file and
	 * if we exit, the file will be deleted already.
	 */
	if (argv[0][0] == 'r')
		rmail++;
	if (argv[0][0] != 'r' &&	/* no favors for rmail*/
	   (argc == 1 || argv[1][0] == '-' && !any(argv[1][1], "hdt-")))
		printmail(argc, argv);
	else
		bulkmail(argc, argv);
	done();
	/*NOTREACHED*/
	return 0;
}

static void
setsig(int i, void (*f)(int))
{
	if (sigset(i, SIG_IGN) != SIG_IGN)
		sigset(i, f);
}

static int
any(register int c, register const char *str)
{

	while (*str)
		if (c == *str++)
			return(1);
	return(0);
}

static void
printmail(int argc, char **argv)
{
	int flg, i, j, print, sig;
	char *p;
	struct stat statb;

	CLOBBGRD(i);
	CLOBBGRD(j);
	CLOBBGRD(print);
	setuid(getuid());
	if ((p = getenv("MAIL")) != NULL) {
		mailfile = smalloc(strlen(p) + 1);
		strcpy(mailfile, p);
	} else {
		mailfile = smalloc(strlen(maildir) + strlen(my_name) + 1);
		cat(mailfile, maildir, my_name);
	}
#ifdef notdef
	if (stat(mailfile, &statb) >= 0
	    && (statb.st_mode & S_IFMT) == S_IFDIR) {
		mailfile = srealloc(strlen(mailfile) + strlen(my_name) + 2);
		strcat(mailfile, "/");
		strcat(mailfile, my_name);
	}
#endif
	while ((i = getopt(argc, argv, "ref:pqr")) != EOF) {
		switch (i) {

		case 'e':
			flge++;
			break;

		case 'p':
			flgp++;
			/*FALLTHRU*/
		case 'q':
			delflg = 0;
			break;

		case 'f':
			mailfile = srealloc(mailfile, strlen(optarg) + 1);
			strcpy(mailfile, optarg);
			break;

		case 'r':
			forward = 1;
			break;

		default:
			usage();
		}
	}
	if (stat(mailfile, &statb) < 0 || statb.st_size == 0 ||
			(malf = fopen(mailfile, "r")) == NULL) {
		if (flge)
			exit(1);
		printf("No mail.\n");
		return;
	}
	if (flge)
		exit(0);
	locktfile = srealloc(locktfile, strlen(mailfile) + 10);
	strcpy(locktfile, mailfile);
	strcat(locktfile, ".tmXXXXXX");
	lock(mailfile, locktfile);
	lockf(fileno(malf), F_TEST, 0);
	copymt(malf, tmpf);
	fclose(malf);			/* implicit unlock */
	unlock();
	fseek(tmpf, 0, SEEK_SET);

	changed = 0;
	print = 1;
	for (i = 0; i < nlet; ) {
		j = forward ? i : nlet - i - 1;
		if ((sig = setjmp(sjbuf)) != 0) {
			sigrelse(sig);
			print = 0;
		} else {
			if (print)
				copylet(j, stdout, ORDINARY);
			print = 1;
		}
		if (flgp) {
			i++;
			continue;
		}
		if ((sig = setjmp(sjbuf)) != 0)
			sigrelse(sig);
		fputs("? ", stdout);
		fflush(stdout);
		if (fgetline(&resp, &rsize, NULL, stdin) == NULL)
			break;
		switch (resp[0]) {

		default:
			printf("usage\n");
			/*FALLTHRU*/
		case '?':
			print = 0;
			printf("q\tquit\n");
			printf("x\texit without changing mail\n");
			printf("p\tprint\n");
			printf("s[file]\tsave (default mbox)\n");
			printf("w[file]\tsame without header\n");
			printf("-\tprint previous\n");
			printf("d\tdelete\n");
			printf("+\tnext (no delete)\n");
			printf("m user\tmail to user\n");
			printf("! cmd\texecute cmd\n");
			break;

		case '+':
		case 'n':
		case '\n':
			i++;
			break;
		case 'x':
			changed = 0;
			/*FALLTHRU*/
		case 'q':
			goto donep;
		case 'p':
			break;
		case '^':
		case '-':
			if (--i < 0)
				i = 0;
			break;
		case 'y':
		case 'w':
		case 's':
			flg = 0;
			if (resp[1] != '\n' && resp[1] != ' ') {
				printf("illegal\n");
				flg++;
				print = 0;
				continue;
			}
			if (resp[1] == '\n' || resp[1] == '\0') {
				size_t sz;

				p = getenv("HOME");
				sz = strlen(p) + 5;
				if (rsize < sz)
					resp = srealloc(resp, rsize += sz);
				if (p != 0)
					cat(resp+1, p, "/mbox");
				else
					cat(resp+1, "", "mbox");
			}
			for (p = resp+1; (p = getarg(&lfil, &lfilsz, p))
					!= NULL; ) {
				malf = fopen(lfil, "a");
				if (malf == NULL) {
					printf("mail: %s: cannot append\n",
					    lfil);
					flg++;
					continue;
				}
				copylet(j, malf, resp[0]=='w'? ZAP: ORDINARY);
				fclose(malf);
			}
			if (flg)
				print = 0;
			else {
				let[j].change = 'd';
				changed++;
				i++;
			}
			break;
		case 'm':
			flg = 0;
			if (resp[1] == '\n' || resp[1] == '\0') {
				i++;
				continue;
			}
			if (resp[1] != ' ') {
				printf("invalid command\n");
				flg++;
				print = 0;
				continue;
			}
			for (p = resp+1; (p = getarg(&lfil, &lfilsz, p))
					!= NULL;)
				if (!sendmail(j, lfil))
					flg++;
			if (flg)
				print = 0;
			else {
				let[j].change = 'd';
				changed++;
				i++;
			}
			break;
		case '!':
			system(resp+1);
			printf("!\n");
			print = 0;
			break;
		case 'd':
			let[j].change = 'd';
			changed++;
			i++;
			if (resp[1] == 'q')
				goto donep;
			break;
		}
	}
   donep:
	if (changed)
		copyback();
}

/* copy temp or whatever back to /var/mail */
static void
copyback(void)
{
	register int i, c;
	int fd, new = 0;
	struct stat stbuf;

	sighold(SIGINT);
	sighold(SIGHUP);
	sighold(SIGQUIT);
	locktfile = srealloc(locktfile, strlen(mailfile) + 10);
	strcpy(locktfile, mailfile);
	strcat(locktfile, ".tmXXXXXX");
	lock(mailfile, locktfile);
	fd = open(mailfile, O_RDWR | O_CREAT, MAILMODE);
	if (fd >= 0) {
		lockf(fd, F_LOCK, 0);
		malf = fdopen(fd, "r+w");
	}
	if (fd < 0 || malf == NULL)
		panic("can't rewrite %s", lfil);
	fstat(fd, &stbuf);
	if (stbuf.st_size != let[nlet].adr) {	/* new mail has arrived */
		fseek(malf, let[nlet].adr, SEEK_SET);
		fseek(tmpf, let[nlet].adr, SEEK_SET);
		while ((c = getc(malf)) != EOF)
			putc(c, tmpf);
		setnlet(nlet + 1);
		let[nlet].adr = stbuf.st_size;
		new = 1;
		fseek(malf, 0, SEEK_SET);
	}
	ftruncate(fd, 0);
	for (i = 0; i < nlet; i++)
		if (let[i].change != 'd')
			copylet(i, malf, ORDINARY);
	fclose(malf);		/* implict unlock */
	if (new)
		printf("New mail has arrived.\n");
	unlock();
	sigrelse(SIGINT);
	sigrelse(SIGHUP);
	sigrelse(SIGQUIT);
}

/* copy mail (f1) to temp (f2) */
static void
copymt(FILE *f1, FILE *f2)
{
	long nextadr;
	size_t llen;

	setnlet(nextadr = 0);
	let[0].adr = 0;
	while (fgetline(&line, &lsize, &llen, f1) != NULL) {
		if (isfrom(line)) {
			let[nlet].adr = nextadr;
			setnlet(nlet + 1);
		}
		nextadr += llen;
		fwrite(line, sizeof *line, llen, f2);
	}
	let[nlet].adr = nextadr;	/* last plus 1 */
}

static void
copylet(int n, FILE *f, int type)
{
	int ch = 0, nl, arpaheader;
	long k, fpos;
	char hostname[257];

	fseek(tmpf, let[n].adr, SEEK_SET);
	k = let[n+1].adr - let[n].adr;
	while (k-- > 1 && (ch = getc(tmpf)) != '\n')
		if (type != ZAP)
			putc(ch, f);
	switch (type) {

	case REMOTE:
		gethostname(hostname, sizeof (hostname));
		fprintf(f, " remote from %s\n", hostname);
		break;

	case FORWARD:
		fprintf(f, forwmsg);
		break;

	case ORDINARY:
		putc(ch, f);
		break;

	case ZAP:
		break;

	default:
		panic("Bad letter type %d to copylet.", type);
	}
	fpos = ftell(f);
	arpaheader = nl = 0;
	while (k-- > 1) {
		ch = getc(tmpf);
		putc(ch, f);
		if (type == ZAP && arpaheader >= 0) {
			if (ch == '\n') {
				if (++nl == 1) {
					if (arpaheader == 0)
						arpaheader = -1;
					else
						arpaheader = 0;
				} else if (nl == 2) {
					fflush(f);
					fseek(f, 0, fpos);
					ftruncate(fileno(f), fpos);
					arpaheader = -1;
				}
			} else {
				if (ch == ':')
					arpaheader = 1;
				else if (ch < 040 || ch >= 0177)
					arpaheader = -1;
				nl = 0;
			}
		}
	}
	if (type != ZAP || ch != '\n')
		putc(getc(tmpf), f);
}

static int
isfrom(register const char *lp)
{
	register const char *p;

	for (p = from; *p; )
		if (*lp++ != *p++)
			return(0);
	return(1);
}

static void
bulkmail(int argc, char **argv)
{
	char *truename;
	int i, j, first;
	int gaver = 0;
	char **newargv;
	register char **ap;
	int dflag = 0;
	int tflag = 0;
	size_t llen;

	CLOBBGRD(dflag);
	CLOBBGRD(argc);
	CLOBBGRD(argv);
	if (argc < 1) {
		fprintf(stderr, "puke\n");
		return;
	}

	truename = malloc(1);
	truename[0] = 0;
	line = smalloc(lsize = LSIZE);
	line[0] = '\0';
	llen = 0;

	/*
	 * When we fall out of this, argv[1] should be first name,
	 * argc should be number of names + 1.
	 */

	while ((i = getopt(argc, argv, "dr:t")) != EOF) {
		switch (i) {
		case 'd':
			dflag = 1;
			break;
		case 'r':
			gaver++;
			truename = srealloc(truename, strlen(optarg) + 1);
			strcpy(truename, optarg);
			fgetline(&line, &lsize, &llen, stdin);
			if (strncasecmp("From", line, 4) == 0) {
				line[0] = '\0';
				llen = 0;
			}
			break;

		case 't':
			tflag = 1;
			break;
		
		default:
			usage();
		}
	}
	argc -= optind - 1, argv += optind - 1;
	if (argc <= 1)
		usage();
	if (gaver == 0) {
		truename = srealloc(truename, strlen(my_name) + 1);
		strcpy(truename, my_name);
	}
	time(&iop);
	if (dflag)
		fprintf(tmpf, "%s%s %s", from, truename, ctime(&iop));
	else {
		if (tflag) {
			fputs("To:", tmpf);
			i = 3;
			for (ap = &argv[1]; *ap; ap++) {
				if ((i += (j = strlen(*ap) + 2)) > 76) {
					putc('\n', tmpf);
					i = j;
				}
				fprintf(tmpf, " %s", *ap);
				if (*(ap + 1))
					putc(',', tmpf);
			}
			putc('\n', tmpf);
		}
		putc('\n', tmpf);
		composing = 1;
		setvbuf(stdin, NULL, _IOLBF, 0);
		if ((i = setjmp(sjbuf)) != 0) {
			sigrelse(i);
			fflush(tmpf);
			setnlet(1);
			let[0].adr = 0;
			let[1].adr = ftell(tmpf);
			savedead();
			done();
		}
	}
	iop = ftell(tmpf);
	flgf = first = 1;
	for (;;) {
		if (first) {
			first = 0;
			if (*line == '\0' &&
					fgetline(&line, &lsize, &llen, stdin)
					== NULL)
				break;
		} else {
			if (fgetline(&line, &lsize, &llen, stdin) == NULL)
				break;
		}
		if (*line == '.' && line[1] == '\n' && isatty(fileno(stdin)))
			break;
		if (dflag && isfrom(line))
			putc('>', tmpf);
		fwrite(line, sizeof *line, llen, tmpf);
		flgf = 0;
	}
	putc('\n', tmpf);
	setnlet(1);
	let[0].adr = 0;
	let[1].adr = ftell(tmpf);
	if (flgf)
		return;
	if (dflag == 0) {
		/* give it to sendmail, rah rah! */
		rewind(tmpf);
		fflush(tmpf);
		dup2(fileno(tmpf), 0);
		close(fileno(tmpf));
		ap = newargv = smalloc((argc + 3) * sizeof *newargv);
		*ap++ = "-sendmail";
		if (rmail)
			*ap++ = "-s";
		argv++;
		while ((*ap++ = *argv++) != NULL);
		setuid(getuid());
		execv(SENDMAIL, newargv);
		perror(SENDMAIL);
		exit(0177);
	}
	while (--argc > 0)
		if (!sendmail(0, *++argv))
			error++;
	if (error)
		savedead();
	fclose(tmpf);
}

static void
savedead(void)
{
	const char *fn = dead, *cp;
	char *cq;

	if (!safefile(dead)) {
tryhome:
		if (my_home != NULL) {
			fn = cq = smalloc(strlen(my_home) + strlen(dead) + 2);
			for (cp = my_home; *cp; cp++)
				*cq++ = *cp;
			*cq++ = '/';
			cp = dead;
			while ((*cq++ = *cp++) != '\0');
			if (!safefile(fn))
				goto cantopen;
		} else
			goto cantopen;
	}
	setuid(getuid());
	malf = fopen(fn, "w");
	if (malf == NULL) {
		if (fn == dead)
			goto tryhome;
cantopen:
		printf("mail: cannot open %s\n", fn);
		fclose(tmpf);
		return;
	}
	copylet(0, malf, ZAP);
	fclose(malf);
	printf("Mail saved in %s\n", fn);
}

static int
sendrmt(int n, const char *name)
{
	FILE *rmf;
	register char *p;
	char *rsys, cmd[256];
	register int pid;
	int sts;

#ifdef notdef
	if (any('^', name)) {
		while (p = strchr(name, '^'))
			*p = '!';
		if (strncmp(name, "researc", 7)) {
			strcpy(rsys, "research");
			if (*name != '!')
				--name;
			goto skip;
		}
	}
#endif
	rsys = smalloc(strlen(name) + 1);
	for (p=rsys; *name!='!'; *p++ = *name++) {
		if (*name=='\0') {
			free(rsys);
			return(0);	/* local address, no '!' */
		}
	}
	*p = '\0';
	if (name[1]=='\0') {
		printf("null name\n");
		free(rsys);
		return(0);
	}
/*skip:*/
	if ((pid = fork()) == -1) {
		fprintf(stderr, "mail: can't create proc for remote\n");
		free(rsys);
		return(0);
	}
	if (pid) {
		free(rsys);
		while (wait(&sts) != pid) {
			if (wait(&sts)==-1)
				return(0);
		}
		return(!sts);
	}
	setuid(getuid());
	if (any('!', name+1))
		snprintf(cmd, sizeof cmd, "uux - %s!rmail \\(%s\\)",
				rsys, name+1);
	else
		snprintf(cmd, sizeof cmd, "uux - %s!rmail %s", rsys, name+1);
	if ((rmf=popen(cmd, "w")) == NULL)
		exit(1);
	copylet(n, rmf, REMOTE);
	exit(pclose(rmf) != 0);
	/*NOTREACHED*/
}

static void
usage(void)
{

	fprintf(stderr, "Usage: %s [-erpqt] [-f file] [persons]\n", progname);
	error = 2;
	done();
}

#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

static void
notifybiff(const char *msg)
{
	static struct sockaddr_in addr;
	static int f = -1;

	if (addr.sin_family == 0) {
		struct hostent *hp = gethostbyname("localhost");
		struct servent *sp = getservbyname("biff", "udp");

		if (hp && sp) {
			addr.sin_family = hp->h_addrtype;
			memcpy(&addr.sin_addr, hp->h_addr, hp->h_length);
			addr.sin_port = sp->s_port;
		}
	}
	if (addr.sin_family) {
		if (f < 0)
			f = socket(AF_INET, SOCK_DGRAM, 0);
		sendto(f, msg, strlen(msg)+1, 0,
				(const struct sockaddr *)&addr, sizeof (addr));
	}
}

static int
sendmail(int n, const char *name)
{
	char *file;
	int /*mask,*/ fd;
	struct passwd *pw;
#ifdef notdef
	struct stat statb;
#endif
	char *buf;
	size_t bufsz;

	if (*name=='!')
		name++;
	if (any('!', name))
		return (sendrmt(n, name));
	if ((pw = getpwnam(name)) == NULL) {
		printf("mail: can't send to %s\n", name);
		return(0);
	}
	file = smalloc(strlen(maildir) + strlen(name) + 1);
	cat(file, maildir, name);
#ifdef notdef
	if (stat(file, &statb) >= 0 && (statb.st_mode & S_IFMT) == S_IFDIR) {
		file = srealloc(file, strlen(file) + strlen(name) + 2);
		strcat(file, "/");
		strcat(file, name);
	}
#endif
	if (!safefile(file)) {
		free(file);
		return(0);
	}
	locktfile = srealloc(locktfile, strlen(file) + 10);
	strcpy(locktfile, file);
	strcat(locktfile, ".tmXXXXXX");
	lock(file, locktfile);
	fd = open(file, O_WRONLY | O_CREAT | O_APPEND, MAILMODE);
	if (fd >= 0) {
		lockf(fd, F_LOCK, 0);
		malf = fdopen(fd, "a");
	}
	if (fd < 0 || malf == NULL) {
		close(fd);
		unlock();
		printf("mail: %s: cannot append\n", file);
		free(file);
		return(0);
	}
	fchown(fd, pw->pw_uid, pw->pw_gid);
	buf = smalloc(bufsz = strlen(name) + 22);
	snprintf(buf, bufsz, "%s@%ld\n", name, (long)ftell(malf)); 
	copylet(n, malf, ORDINARY);
	fclose(malf);
	unlock();
	notifybiff(buf);
	free(file);
	free(buf);
	return(1);
}

static void
delex(int i)
{
	if (!composing)
		putc('\n', stderr);
	if (delflg)
		longjmp(sjbuf, i);
	done();
}

void
done(void)
{
	if(locked)
		unlock();
	unlink(locktmp);
	exit(error);
}

void
cat(char *to, const char *from1, const char *from2)
{
	register char *cp;
	register const char *dp;

	cp = to;
	for (dp = from1; *cp = *dp++; cp++)
		;
	for (dp = from2; *cp++ = *dp++; )
		;
}

/* copy p... into s, update p */
static char *
getarg(char **s, size_t *sz, register char *p)
{
	size_t len;
	register char *sp;

	while (*p == ' ' || *p == '\t')
		p++;
	if (*p == '\n' || *p == '\0')
		return(NULL);
	len = strlen(p);
	if (*sz < len + 1)
		*s = srealloc(*s, len + 1);
	sp = *s;
	while (*p != ' ' && *p != '\t' && *p != '\n' && *p != '\0')
		*sp++ = *p++;
	*sp = '\0';
	return(p);
}

static int
safefile(const char *f)
{
	struct stat statb;

	if (lstat(f, &statb) < 0)
		return (1);
	if (statb.st_nlink != 1 || (statb.st_mode & S_IFMT) == S_IFLNK) {
		fprintf(stderr,
		    "mail: %s has more than one link or is a symbolic link\n",
		    f);
		return (0);
	}
	return (1);
}

static void
panic(const char *msg, ...)
{
	va_list ap;

	fprintf(stderr, "mail: ");
	va_start(ap, msg);
	vfprintf(stderr, msg, ap);
	va_end(ap);
	fprintf(stderr, "\n");
	done();
}

static void *
srealloc(void *vp, size_t nbytes)
{
	void *p;

	if ((p = (void *)realloc(vp, nbytes)) == NULL) {
		write(2, "no memory\n", 10);
		exit(077);
	}
	return p;
}

static void *
smalloc(size_t nbytes)
{
	return srealloc(NULL, nbytes);
}

static char *
fgetline(char **line, size_t *linesize, size_t *llen, FILE *fp)
{
	int c;
	size_t n = 0;

	if (*line == NULL || *linesize < LSIZE + n + 1)
		*line = srealloc(*line, *linesize = LSIZE + n + 1);
	for (;;) {
		if (n >= *linesize - LSIZE / 2)
			*line = srealloc(*line, *linesize += LSIZE);
		c = getc(fp);
		if (c != EOF) {
			(*line)[n++] = c;
			(*line)[n] = '\0';
			if (c == '\n')
				break;
		} else {
			if (n > 0)
				break;
			else
				return NULL;
		}
	}
	if (llen)
		*llen = n;
	return *line;
}

static void
setnlet(int n)
{
	static int allocd;

	nlet = n;
	if (n + 2 > allocd)
		let = srealloc(let, (allocd = n + 32) * sizeof *let);
}
