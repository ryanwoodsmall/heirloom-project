/*
 * Nail - a mail user agent derived from Berkeley Mail.
 *
 * Copyright (c) 2000-2002 Gunnar Ritter, Freiburg i. Br., Germany.
 */
/*
 * Copyright (c) 1980, 1993
 *	The Regents of the University of California.  All rights reserved.
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
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
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
 */

#ifndef lint
#ifdef	DOSCCS
static char sccsid[] = "@(#)sendout.c	2.42 (gritter) 9/10/04";
#endif
#endif /* not lint */

#include "rcv.h"
#include "extern.h"
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>

/*
 * Mail -- a mail program
 *
 * Mail to others.
 */

static const char	randfile[] = "/dev/urandom";
static char	*send_boundary;

static char	*makeboundary __P((void));
static char	*getencoding __P((int));
static struct name	*fixhead __P((struct header *, struct name *,
				enum gfield));
static int	put_signature __P((FILE *, int));
static int	attach_file __P((struct attachment *, FILE *));
static int	make_multipart __P((struct header *, int, FILE *, FILE *,
			const char *, const char *));
static FILE	*infix __P((struct header *, FILE *));
static int	savemail __P((char [], FILE *));
static int	sendmail_internal __P((void *, int));
static enum okay	start_mta __P((struct name *, struct name *, FILE *));
static void	message_id __P((FILE *));
static void	date_field __P((FILE *));
static int	fmt __P((char *, struct name *, FILE *, int, int, int));
static int	infix_fw __P((FILE *, FILE *, struct message *,
			struct name *, int));

#define	BOUNDARRAY	8

/*
 * Generate a boundary for MIME multipart messages.
 */
static char *
makeboundary()
{
	int i, bd;
	static unsigned msgc;
	static pid_t pid;
	static char bound[73];
	time_t t;
	unsigned long r[BOUNDARRAY];
	char b[BOUNDARRAY][sizeof r[0] * 2 + 1];

	if (pid == 0) {
		pid = getpid();
		msgc = (unsigned)pid;
	}
	msgc *= 2053 * (unsigned)time(&t);
	if ((bd = open(randfile, O_RDONLY)) != -1) {
		for (i = 0; i < BOUNDARRAY; i++) {
			if (read(bd, &r[i], sizeof r[0]) != sizeof r[0]) {
				r[0] = 0L;
				break;
			}
		}
		close(bd);
	} else
		r[0] = 0L;
	if (r[0] == 0L) {
		srand((unsigned)msgc);
		for (i = 0; i < BOUNDARRAY; i++) {
			r[i] = 1L;
			while (r[i] < 60466176L)
				r[i] *= (unsigned long)rand();
		}
	}
	snprintf(bound, 73,
			"%.5s%.5s-=-%.5s%.5s-CUT-HERE-%.5s%.5s-=-%.5s%.5s",
			itostr(36, (unsigned)r[0], b[0]),
			itostr(36, (unsigned)r[1], b[1]),
			itostr(36, (unsigned)r[2], b[2]),
			itostr(36, (unsigned)r[3], b[3]),
			itostr(36, (unsigned)r[4], b[4]),
			itostr(36, (unsigned)r[5], b[5]),
			itostr(36, (unsigned)r[6], b[6]),
			itostr(36, (unsigned)r[7], b[7]));
	send_boundary = bound;
	return send_boundary;
}

/*
 * Get an encoding flag based on the given string.
 */
static char *
getencoding(convert)
	int convert;
{
	switch (convert) {
	case CONV_7BIT:
		return "7bit";
	case CONV_NONE:
		return "8bit";
	case CONV_TOQP:
		return "quoted-printable";
	case CONV_TOB64:
		return "base64";
	}
	/*NOTREACHED*/
	return NULL;
}

/*
 * Fix the header by glopping all of the expanded names from
 * the distribution list into the appropriate fields.
 */
static struct name *
fixhead(hp, tolist, addauto)
	struct header *hp;
	struct name *tolist;
	enum gfield addauto;
{
	struct name *np;
	char	*cp;

	hp->h_to = NULL;
	hp->h_cc = NULL;
	hp->h_bcc = NULL;
	hp->h_replyto = NULL;
	for (np = tolist; np != NULL; np = np->n_flink)
		if ((np->n_type & GMASK) == GTO)
			hp->h_to =
				cat(hp->h_to, nalloc(np->n_fullname,
							np->n_type|GFULL));
		else if ((np->n_type & GMASK) == GCC)
			hp->h_cc =
				cat(hp->h_cc, nalloc(np->n_fullname,
							np->n_type|GFULL));
		else if ((np->n_type & GMASK) == GBCC)
			hp->h_bcc =
				cat(hp->h_bcc, nalloc(np->n_fullname,
							np->n_type|GFULL));
		else if ((np->n_type & GMASK) == GREPLYTO)
			hp->h_replyto =
				cat(hp->h_replyto, nalloc(np->n_fullname,
							np->n_type|GFULL));
	if (addauto&GCC && (cp = value("autocc")) != NULL && *cp) {
		np = checkaddrs(sextract(cp, GCC|GFULL));
		hp->h_cc = cat(hp->h_cc, np);
		tolist = cat(tolist, np);
	}
	if (addauto&GBCC && (cp = value("autobcc")) != NULL && *cp) {
		np = checkaddrs(sextract(cp, GBCC|GFULL));
		hp->h_bcc = cat(hp->h_bcc, np);
		tolist = cat(tolist, np);
	}
	if (addauto&GREPLYTO && (cp = value("replyto")) != NULL && *cp) {
		np = checkaddrs(sextract(cp, GREPLYTO|GFULL));
		hp->h_replyto = cat(hp->h_replyto, np);
	}
	return tolist;
}


/*
 * Do not change, you get incorrect base64 encodings else!
 */
#define	INFIX_BUF	972

/*
 * Put the signature file at fo.
 */
static int
put_signature(fo, convert)
FILE *fo;
int convert;
{
	char *sig, buf[INFIX_BUF], c = '\n';
	FILE *fsig;
	size_t sz;

	sig = value("signature");
	if (sig == NULL || *sig == '\0')
		return 0;
	else
		sig = expand(sig);
	if ((fsig = Fopen(sig, "r")) == NULL) {
		perror(sig);
		return -1;
	}
	while ((sz = fread(buf, sizeof *buf, INFIX_BUF, fsig)) != 0) {
		c = buf[sz - 1];
		if (mime_write(buf, sizeof *buf, sz, fo, convert, TD_NONE,
					NULL, (size_t)0)
				== 0) {
			perror(sig);
			Fclose(fsig);
			return -1;
		}
	}
	if (ferror(fsig)) {
		perror(sig);
		Fclose(fsig);
		return -1;
	}
	Fclose(fsig);
	if (c != '\n')
		putc('\n', fo);
	return 0;
}

/*
 * Write an attachment to the file buffer, converting to MIME.
 */
static int
attach_file(ap, fo)
struct attachment *ap;
FILE *fo;
{
	FILE *fi;
	char *charset = NULL, *contenttype = NULL, *basename;
	int convert = CONV_TOB64;
	int err = 0;
	enum mimeclean isclean;
	size_t sz;
	char *buf;
	size_t bufsize, count;

	if ((fi = Fopen(ap->a_name, "r")) == NULL) {
		perror(ap->a_name);
		return -1;
	}
	if ((basename = strrchr(ap->a_name, '/')) == NULL)
		basename = ap->a_name;
	else
		basename++;
	if (ap->a_content_type)
		contenttype = ap->a_content_type;
	else
		contenttype = mime_filecontent(basename);
	convert = get_mime_convert(fi, &contenttype, &charset, &isclean);
	fprintf(fo,
		"\n--%s\n"
		"Content-Type: %s",
		send_boundary, contenttype);
	if (charset == NULL)
		putc('\n', fo);
	else
		fprintf(fo, ";\n charset=%s\n", charset);
	if (ap->a_content_disposition == NULL)
		ap->a_content_disposition = "attachment";
	fprintf(fo, "Content-Transfer-Encoding: %s\n"
		"Content-Disposition: %s;\n"
		" filename=\"",
		getencoding(convert),
		ap->a_content_disposition);
	mime_write(basename, sizeof *basename, strlen(basename), fo,
			CONV_TOHDR, TD_NONE, NULL, (size_t)0);
	fwrite("\"\n", sizeof (char), 2, fo);
	if (ap->a_content_id)
		fprintf(fo, "Content-ID: %s\n", ap->a_content_id);
	if (ap->a_content_description)
		fprintf(fo, "Content-Description: %s\n",
				ap->a_content_description);
	putc('\n', fo);
	buf = smalloc(bufsize = INFIX_BUF);
	if (convert == CONV_TOQP) {
		fflush(fi);
		count = fsize(fi);
	}
	for (;;) {
		if (convert == CONV_TOQP) {
			if (fgetline(&buf, &bufsize, &count, &sz, fi, 0)
					== NULL)
				break;
		} else {
			if ((sz = fread(buf, sizeof *buf, bufsize, fi)) == 0)
				break;
		}
		if (mime_write(buf, sizeof *buf, sz, fo, convert, TD_NONE,
					NULL, (size_t)0) == 0)
			err = -1;
	}
	if (ferror(fi))
		err = -1;
	Fclose(fi);
	free(buf);
	return err;
}

/*
 * Generate the body of a MIME multipart message.
 */
static int
make_multipart(hp, convert, fi, fo, contenttype, charset)
struct header *hp;
FILE *fi, *fo;
const char *contenttype;
const char *charset;
int convert;
{
	struct attachment *att;

	fputs("This is a multi-part message in MIME format.\n", fo);
	if (fsize(fi) != 0) {
		char *buf, c = '\n';
		size_t sz, bufsize, count;

		fprintf(fo, "\n--%s\n", send_boundary);
		fprintf(fo, "Content-Type: %s", contenttype);
		if (charset)
			fprintf(fo, "; charset=%s", charset);
		fprintf(fo, "\nContent-Transfer-Encoding: %s\n"
				"Content-Disposition: inline\n\n",
				getencoding(convert));
		buf = smalloc(bufsize = INFIX_BUF);
		if (convert == CONV_TOQP) {
			fflush(fi);
			count = fsize(fi);
		}
		for (;;) {
			if (convert == CONV_TOQP) {
				if (fgetline(&buf, &bufsize, &count, &sz, fi, 0)
						== NULL)
					break;
			} else {
				sz = fread(buf, sizeof *buf, bufsize, fi);
				if (sz == 0)
					break;
			}
			c = buf[sz - 1];
			if (mime_write(buf, sizeof *buf, sz, fo, convert,
					TD_ICONV, NULL, (size_t)0) == 0) {
				free(buf);
				return -1;
			}
		}
		free(buf);
		if (ferror(fi))
			return -1;
		if (c != '\n')
			putc('\n', fo);
		if (charset != NULL)
			put_signature(fo, convert);
	}
	for (att = hp->h_attach; att != NULL; att = att->a_flink)
		if (attach_file(att, fo) != 0)
			return -1;
	/* the final boundary with two attached dashes */
	fprintf(fo, "\n--%s--\n", send_boundary);
	return 0;
}

/*
 * Prepend a header in front of the collected stuff
 * and return the new file.
 */
static FILE *
infix(hp, fi)
	struct header *hp;
	FILE *fi;
{
	FILE *nfo, *nfi;
	char *tempMail;
#ifdef	HAVE_ICONV
	char *tcs, *convhdr = NULL;
#endif
	enum mimeclean isclean;
	int convert;
	char *charset = NULL, *contenttype = NULL;

	if ((nfo = Ftemp(&tempMail, "Rs", "w", 0600, 1)) == NULL) {
		perror(catgets(catd, CATSET, 178, "temporary mail file"));
		return(NULL);
	}
	if ((nfi = Fopen(tempMail, "r")) == NULL) {
		perror(tempMail);
		Fclose(nfo);
		return(NULL);
	}
	rm(tempMail);
	Ftfree(&tempMail);
	convert = get_mime_convert(fi, &contenttype, &charset, &isclean);
#ifdef	HAVE_ICONV
	tcs = gettcharset();
	if (((isclean & MIME_HASNUL) == 0 && (isclean & MIME_HIGHBIT) &&
				strcmp(charset, tcs)) ||
			(convhdr = need_hdrconv(hp,
				GTO|GSUBJECT|GCC|GBCC|GREPLYTO|GIDENT)) != 0) {
		if (convhdr)
			charset = convhdr;
		if (iconvd != (iconv_t)-1)
			iconv_close(iconvd);
		if ((iconvd = iconv_open_ft(charset, tcs)) == (iconv_t)-1
				&& errno != 0) {
			if (errno == EINVAL)
				fprintf(stderr, catgets(catd, CATSET, 179,
			"Cannot convert from %s to %s\n"), tcs, charset);
			else
				perror("iconv_open");
			Fclose(nfo);
			return NULL;
		}
	}
#endif	/* HAVE_ICONV */
	if (puthead(hp, nfo,
		   GTO|GSUBJECT|GCC|GBCC|GNL|GCOMMA|GUA|GMIME
		   |GMSGID|GIDENT|GREPLYTO|GREF|GDATE,
		   convert, contenttype, charset)) {
		Fclose(nfo);
		Fclose(nfi);
#ifdef	HAVE_ICONV
		if (iconvd != (iconv_t)-1) {
			iconv_close(iconvd);
			iconvd = (iconv_t)-1;
		}
#endif
		return NULL;
	}
#ifdef	HAVE_ICONV
	if (convhdr && iconvd != (iconv_t)-1) {
		iconv_close(iconvd);
		iconvd = (iconv_t)-1;
	}
#endif
	if (hp->h_attach != NULL) {
		if (make_multipart(hp, convert, fi, nfo,
					contenttype, charset) != 0) {
			Fclose(nfo);
			Fclose(nfi);
#ifdef	HAVE_ICONV
			if (iconvd != (iconv_t)-1) {
				iconv_close(iconvd);
				iconvd = (iconv_t)-1;
			}
#endif
			return NULL;
		}
	} else {
		size_t sz, bufsize, count;
		char *buf;

		if (convert == CONV_TOQP) {
			fflush(fi);
			count = fsize(fi);
		}
		buf = smalloc(bufsize = INFIX_BUF);
		for (;;) {
			if (convert == CONV_TOQP) {
				if (fgetline(&buf, &bufsize, &count, &sz, fi, 0)
						== NULL)
					break;
			} else {
				sz = fread(buf, sizeof *buf, bufsize, fi);
				if (sz == 0)
					break;
			}
			if (mime_write(buf, sizeof *buf, sz, nfo, convert,
					TD_ICONV, NULL, (size_t)0) == 0) {
				Fclose(nfo);
				Fclose(nfi);
				perror("read");
#ifdef	HAVE_ICONV
				if (iconvd != (iconv_t)-1) {
					iconv_close(iconvd);
					iconvd = (iconv_t)-1;
				}
#endif
				free(buf);
				return NULL;
			}
		}
		free(buf);
		if (ferror(fi)) {
			Fclose(nfo);
			Fclose(nfi);
			perror("read");
#ifdef	HAVE_ICONV
			if (iconvd != (iconv_t)-1) {
				iconv_close(iconvd);
				iconvd = (iconv_t)-1;
			}
#endif
			return NULL;
		}
		if (charset != NULL)
			put_signature(nfo, convert);
	}
#ifdef	HAVE_ICONV
	if (iconvd != (iconv_t)-1) {
		iconv_close(iconvd);
		iconvd = (iconv_t)-1;
	}
#endif
	fflush(nfo);
	if (ferror(nfo)) {
		perror(catgets(catd, CATSET, 180, "temporary mail file"));
		Fclose(nfo);
		Fclose(nfi);
		return NULL;
	}
	Fclose(nfo);
	Fclose(fi);
	fflush(nfi);
	rewind(nfi);
	return(nfi);
}

/*
 * Save the outgoing mail on the passed file.
 */

/*ARGSUSED*/
static int
savemail(name, fi)
	char name[];
	FILE *fi;
{
	FILE *fo;
	char *buf;
	size_t bufsize, buflen, count;
	char *p;
	time_t now;
	int prependnl = 0;
	int error = 0;

	buf = smalloc(bufsize = LINESIZE);
	time(&now);
	if ((fo = Zopen(name, "a+", NULL)) == NULL) {
		if ((fo = Zopen(name, "wx", NULL)) == NULL) {
			perror(name);
			free(buf);
			return (-1);
		}
	} else {
		if (fseek(fo, -2L, SEEK_END) == 0) {
			switch (fread(buf, sizeof *buf, 2, fo)) {
			case 2:
				if (buf[1] != '\n') {
					prependnl = 1;
					break;
				}
				/*FALLTHRU*/
			case 1:
				if (buf[0] != '\n')
					prependnl = 1;
				break;
			default:
				if (ferror(fo)) {
					perror(name);
					free(buf);
					return -1;
				}
			}
			fflush(fo);
			if (prependnl) {
				putc('\n', fo);
				fflush(fo);
			}
		}
	}
	fprintf(fo, "From %s %s", myname, ctime(&now));
	buflen = 0;
	fflush(fi);
	rewind(fi);
	count = fsize(fi);
	while (fgetline(&buf, &bufsize, &count, &buflen, fi, 0) != NULL) {
		if (*buf == '>') {
			p = buf + 1;
			while (*p == '>')
				p++;
			if (strncmp(p, "From ", 5) == 0)
				/* we got a masked From line */
				putc('>', fo);
		} else if (strncmp(buf, "From ", 5) == 0)
			putc('>', fo);
		fwrite(buf, sizeof *buf, buflen, fo);
	}
	if (buflen && *(buf + buflen - 1) != '\n')
		putc('\n', fo);
	putc('\n', fo);
	fflush(fo);
	if (ferror(fo)) {
		perror(name);
		error = -1;
	}
	if (Fclose(fo) != 0)
		error = -1;
	fflush(fi);
	rewind(fi);
	/*
	 * OpenBSD 3.2 and NetBSD 1.5.2 were reported not to 
	 * reset the kernel file offset after the calls above,
	 * a clear violation of IEEE Std 1003.1, 1996, 8.2.3.7.
	 * So do it 'manually'.
	 */
	lseek(fileno(fi), 0, SEEK_SET);
	free(buf);
	return error;
}

/*
 * Interface between the argument list and the mail1 routine
 * which does all the dirty work.
 */
int
mail(to, cc, bcc, smopts, subject, attach, quotefile, recipient_record, tflag)
	struct name *to, *cc, *bcc, *smopts;
	struct attachment *attach;
	char *subject, *quotefile;
	int recipient_record, tflag;
{
	struct header head;
	struct str in, out;

	memset(&head, 0, sizeof head);
	/* The given subject may be in RFC1522 format. */
	if (subject != NULL) {
		in.s = subject;
		in.l = strlen(subject);
		mime_fromhdr(&in, &out, TD_ISPR | TD_ICONV);
		head.h_subject = out.s;
	}
	if (tflag == 0) {
		head.h_to = to;
		head.h_cc = cc;
		head.h_bcc = bcc;
	}
	head.h_attach = attach;
	head.h_smopts = smopts;
	mail1(&head, 0, NULL, quotefile, recipient_record, tflag);
	if (subject != NULL)
		free(out.s);
	return(0);
}

/*
 * Send mail to a bunch of user names.  The interface is through
 * the mail routine below.
 */
static int
sendmail_internal(v, recipient_record)
	void *v;
	int recipient_record;
{
	char *str = v;
	struct header head;

	memset(&head, 0, sizeof head);
	head.h_to = extract(str, GTO|GFULL);
	mail1(&head, 0, NULL, NULL, recipient_record, 0);
	return(0);
}

int
sendmail(v)
	void *v;
{
	return sendmail_internal(v, 0);
}

int
Sendmail(v)
	void *v;
{
	return sendmail_internal(v, 1);
}

/*
 * Start the Mail Transfer Agent
 * mailing to namelist and stdin redirected to input.
 */
static enum okay
start_mta(to, mailargs, input)
struct name *to, *mailargs;
FILE* input;
{
	char **args = NULL, **t;
	pid_t pid;
	sigset_t nset;
	char *cp, *smtp;
	enum okay	ok = STOP;

	if ((smtp = value("smtp")) == NULL) {
		args = unpack(cat(mailargs, to));
		if (debug || value("debug")) {
			printf(catgets(catd, CATSET, 181,
					"Sendmail arguments:"));
			for (t = args; *t != NULL; t++)
				printf(" \"%s\"", *t);
			printf("\n");
			return OKAY;
		}
	} else if (debug || value("debug"))
		return OKAY;
	/*
	 * Fork, set up the temporary mail file as standard
	 * input for "mail", and exec with the user list we generated
	 * far above.
	 */
	if ((pid = fork()) == -1) {
		perror("fork");
		savedeadletter(input);
		senderr++;
		return STOP;
	}
	if (pid == 0) {
		sigemptyset(&nset);
		sigaddset(&nset, SIGHUP);
		sigaddset(&nset, SIGINT);
		sigaddset(&nset, SIGQUIT);
		sigaddset(&nset, SIGTSTP);
		sigaddset(&nset, SIGTTIN);
		sigaddset(&nset, SIGTTOU);
		freopen("/dev/null", "r", stdin);
		if (smtp != NULL) {
			prepare_child(&nset, 0, 1);
			if (smtp_mta(smtp, to, input) == 0)
				_exit(0);
		} else {
			prepare_child(&nset, fileno(input), -1);
			if ((cp = value("sendmail")) != NULL)
				cp = expand(cp);
			else
				cp = SENDMAIL;
			execv(cp, args);
			perror(cp);
		}
		savedeadletter(input);
		fputs(catgets(catd, CATSET, 182,
				". . . message not sent.\n"), stderr);
		_exit(1);
	}
	if (value("verbose") != NULL || value("sendwait")) {
		if (wait_child(pid) == 0)
			ok = OKAY;
		else
			senderr++;
	} else {
		ok = OKAY;
		free_child(pid);
	}
	return ok;
}

/*
 * Mail a message on standard input to the people indicated
 * in the passed header.  (Internal interface).
 */
enum okay
mail1(hp, printheaders, quote, quotefile, recipient_record, tflag)
	struct header *hp;
	int printheaders;
	struct message *quote;
	char *quotefile;
	int recipient_record, tflag;
{
	char *cp, *cq, *ep;
	struct name *to;
	FILE *mtf, *nmtf;
	enum okay	ok = STOP;

#ifdef	notdef
	if ((hp->h_to = checkaddrs(hp->h_to)) == NULL) {
		senderr++;
		return STOP;
	}
#endif
	/*
	 * Collect user's mail from standard input.
	 * Get the result as mtf.
	 */
	if ((mtf = collect(hp, printheaders, quote, quotefile, tflag)) == NULL)
		return STOP;
	if (value("interactive") != NULL) {
		if (((value("bsdcompat") || value("askatend"))
					&& (value("askcc") != NULL ||
					value("askbcc") != NULL)) ||
				value("askattach") != NULL) {
			if (value("askcc") != NULL)
				grabh(hp, GCC, 1);
			if (value("askbcc") != NULL)
				grabh(hp, GBCC, 1);
			if (value("askattach") != NULL)
				hp->h_attach = edit_attachments(hp->h_attach);
		} else {
			printf(catgets(catd, CATSET, 183, "EOT\n"));
			fflush(stdout);
		}
	}
	if (fsize(mtf) == 0) {
		if (hp->h_subject == NULL)
			printf(catgets(catd, CATSET, 184,
				"No message, no subject; hope that's ok\n"));
		else if (value("bsdcompat") || value("bsdmsgs"))
			printf(catgets(catd, CATSET, 185,
				"Null message body; hope that's ok\n"));
	}
	/*
	 * Now, take the user names from the combined
	 * to and cc lists and do all the alias
	 * processing.
	 */
	senderr = 0;
	if ((to = usermap(cat(hp->h_bcc, cat(hp->h_to, hp->h_cc)))) == NULL) {
		printf(catgets(catd, CATSET, 186, "No recipients specified\n"));
		senderr++;
	}
	to = fixhead(hp, to, GCC|GBCC|GREPLYTO);
	if ((nmtf = infix(hp, mtf)) == NULL) {
		/* fprintf(stderr, ". . . message lost, sorry.\n"); */
		perror("");
		senderr++;
		rewind(mtf);
		savedeadletter(mtf);
		fputs(catgets(catd, CATSET, 187,
				". . . message not sent.\n"), stderr);
		return STOP;
	}
	mtf = nmtf;
	/*
	 * Look through the recipient list for names with /'s
	 * in them which we write to as files directly.
	 */
	to = outof(to, mtf, hp);
	if (senderr)
		savedeadletter(mtf);
	to = elide(to);
	if (count(to) == 0) {
		ok = OKAY;
		goto out;
	}
	if (recipient_record) {
		cq = skin(to->n_name);
		cp = salloc(strlen(cq) + 1);
		strcpy(cp, cq);
		for (cq = cp; *cq && *cq != '@'; cq++);
		*cq = '\0';
	} else
		cp = value("record");
	if (cp != NULL) {
		ep = expand(cp);
		if (value("outfolder") && *ep != '/' && *ep != '+' &&
				which_protocol(ep) == PROTO_FILE) {
			cq = salloc(strlen(cp) + 2);
			cq[0] = '+';
			strcpy(&cq[1], cp);
			cp = cq;
			ep = expand(cp);
		}
		if (savemail(ep, mtf) != 0) {
			fprintf(stderr,
				"Error while saving message to %s - "
				"message not sent\n", ep);
			rewind(mtf);
			exit_status |= 1;
			savedeadletter(mtf);
			goto out;
		}
	}
	ok = start_mta(to, hp->h_smopts, mtf);
out:
	Fclose(mtf);
	return ok;
}

/*
 * Create a Message-Id: header field.
 * Use either the host name or the host part of the from address.
 */
static void
message_id(fo)
FILE *fo;
{
	static unsigned long msgc;
	time_t t;
	char *domainpart;
	int rd;
	long randbuf;
	char datestr[sizeof (time_t) * 2 + 1];
	char pidstr[sizeof (pid_t) * 2+ 1];
	char countstr[sizeof msgc * 2 + 1];
	char randstr[sizeof randbuf * 2+ 1];


	msgc++;
	time(&t);
	rd = open(randfile, O_RDONLY);
	if (rd != -1) {
		if (read(rd, &randbuf, sizeof randbuf) != sizeof randbuf) {
			srand((unsigned)(msgc * t));
			randbuf = (long)rand();
		}
		close(rd);
	} else {
		srand((unsigned)(msgc * t));
		randbuf = (long)rand();
	}
	itostr(16, (unsigned)t, datestr);
	itostr(36, (unsigned)getpid(), pidstr);
	itostr(36, (unsigned)msgc, countstr);
	itostr(36, (unsigned)randbuf, randstr);
	if ((domainpart = skin(value("from"))) != NULL
			&& (domainpart = last_at_before_slash(domainpart))
			!= NULL)
		domainpart++;
	else
		domainpart = nodename(1);
	fprintf(fo, "Message-ID: <%s.%s%.5s%s%.5s@%s>\n",
			datestr, progname, pidstr, countstr, randstr,
			domainpart);
}

static const char *weekday_names[] = {
	"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
};

const char *month_names[] = {
	"Jan", "Feb", "Mar", "Apr", "May", "Jun",
	"Jul", "Aug", "Sep", "Oct", "Nov", "Dec", NULL
};

/*
 * Create a Date: header field.
 * We compare the localtime() and gmtime() results to get the timezone,
 * because numeric timezones are easier to read and because $TZ is
 * not set on most GNU systems.
 */
static void
date_field(fo)
FILE *fo;
{
	time_t t;
	struct tm *tmptr;
	int tzdiff, tzdiff_hour, tzdiff_min;

	time(&t);
	tzdiff = t - mktime(gmtime(&t));
	tzdiff_hour = (int)(tzdiff / 60);
	tzdiff_min = tzdiff_hour % 60;
	tzdiff_hour /= 60;
	tmptr = localtime(&t);
	if (tmptr->tm_isdst > 0)
		tzdiff_hour++;
	fprintf(fo, "Date: %s, %02d %s %04d %02d:%02d:%02d %+03d%02d\n",
			weekday_names[tmptr->tm_wday],
			tmptr->tm_mday, month_names[tmptr->tm_mon],
			tmptr->tm_year + 1900, tmptr->tm_hour,
			tmptr->tm_min, tmptr->tm_sec,
			tzdiff_hour, tzdiff_min);
}

#define	FMT_CC_AND_BCC	{ \
				if (hp->h_cc != NULL && w & GCC) { \
					if (fmt("Cc:", hp->h_cc, fo, \
							w&GCOMMA, 0, \
							convert!=CONV_TODISP)) \
						return 1; \
					gotcha++; \
				} \
				if (hp->h_bcc != NULL && w & GBCC) { \
					if (fmt("Bcc:", hp->h_bcc, fo, \
							w&GCOMMA, 0, \
							convert!=CONV_TODISP)) \
						return 1; \
					gotcha++; \
				} \
			}
/*
 * Dump the to, subject, cc header on the
 * passed file buffer.
 */
int
puthead(hp, fo, w, convert, contenttype, charset)
	struct header *hp;
	FILE *fo;
	enum gfield w;
	char *contenttype, *charset;
	int convert;
{
	int gotcha;
	char *addr/*, *cp*/;
	int stealthmua;
	struct name *np;


	if (value("stealthmua"))
		stealthmua = 1;
	else
		stealthmua = 0;
	gotcha = 0;
	if (w & GDATE) {
		date_field(fo), gotcha++;
	}
	if (w & GIDENT) {
		addr = myaddr();
		if (addr != NULL) {
			if (mime_name_invalid(addr, 1))
				return 1;
			/*
			if ((cp = value("smtp")) == NULL
					|| strcmp(cp, "localhost") == 0)
				fprintf(fo, "Sender: %s\n", username());
			*/
			fwrite("From: ", sizeof (char), 6, fo);
			if (mime_write(addr, sizeof *addr, strlen(addr), fo,
					convert == CONV_TODISP ?
					CONV_NONE:CONV_TOHDR_A,
					convert == CONV_TODISP ?
					TD_ISPR|TD_ICONV:TD_ICONV,
					NULL, (size_t)0) == 0)
				return 1;
			gotcha++;
			putc('\n', fo);
		}
		addr = value("ORGANIZATION");
		if (addr != NULL) {
			fwrite("Organization: ", sizeof (char), 14, fo);
			if (mime_write(addr, sizeof *addr, strlen(addr), fo,
					convert == CONV_TODISP ?
					CONV_NONE:CONV_TOHDR,
					convert == CONV_TODISP ?
					TD_ISPR|TD_ICONV:TD_ICONV,
					NULL, (size_t)0) == 0)
				return 1;
			gotcha++;
			putc('\n', fo);
		}
	}
	if (hp->h_replyto != NULL && w & GREPLYTO) {
		if (fmt("Reply-To:", hp->h_replyto, fo, w&GCOMMA, 0,
					convert != CONV_TODISP))
			return 1;
		gotcha++;
	}
	if (hp->h_to != NULL && w & GTO) {
		if (fmt("To:", hp->h_to, fo, w&GCOMMA, 0,convert!=CONV_TODISP))
			return 1;
		gotcha++;
	}
	if (value("bsdcompat") == NULL && value("bsdorder") == NULL)
		FMT_CC_AND_BCC
	if (hp->h_subject != NULL && w & GSUBJECT) {
		fwrite("Subject: ", sizeof (char), 9, fo);
		if (ascncasecmp(hp->h_subject, "re: ", 4) == 0) {
			fwrite("Re: ", sizeof (char), 4, fo);
			if (mime_write(hp->h_subject + 4, sizeof *hp->h_subject,
					strlen(hp->h_subject + 4),
					fo, convert == CONV_TODISP ?
					CONV_NONE:CONV_TOHDR,
					convert == CONV_TODISP ?
					TD_ISPR|TD_ICONV:TD_ICONV,
					NULL, (size_t)0) == 0)
				return 1;
		} else {
			if (mime_write(hp->h_subject, sizeof *hp->h_subject,
					strlen(hp->h_subject),
					fo, convert == CONV_TODISP ?
					CONV_NONE:CONV_TOHDR,
					convert == CONV_TODISP ?
					TD_ISPR|TD_ICONV:TD_ICONV,
					NULL, (size_t)0) == 0)
				return 1;
		}
		gotcha++;
		fwrite("\n", sizeof (char), 1, fo);
	}
	if (value("bsdcompat") || value("bsdorder"))
		FMT_CC_AND_BCC
	if (w & GMSGID && stealthmua == 0)
		message_id(fo), gotcha++;
	if (hp->h_ref != NULL && w & GREF) {
		fmt("References:", hp->h_ref, fo, 0, 1, 0);
		if ((np = hp->h_ref) != NULL && np->n_name) {
			while (np->n_flink)
				np = np->n_flink;
			if (mime_name_invalid(np->n_name, 0) == 0) {
				fprintf(fo, "In-Reply-To: %s\n", np->n_name);
				gotcha++;
			}
		}
	}
	if (w & GUA && stealthmua == 0)
		fprintf(fo, "User-Agent: %s\n", version), gotcha++;
	if (w & GMIME) {
		fputs("MIME-Version: 1.0\n", fo), gotcha++;
		if (hp->h_attach != NULL) {
			makeboundary();
			fprintf(fo, "Content-Type: multipart/mixed;\n"
				" boundary=\"%s\"\n", send_boundary);
		} else {
			fprintf(fo, "Content-Type: %s", contenttype);
			if (charset)
				fprintf(fo, "; charset=%s", charset);
			fprintf(fo, "\nContent-Transfer-Encoding: %s\n",
					getencoding(convert));
		}
	}
	if (gotcha && w & GNL)
		putc('\n', fo);
	return(0);
}

/*
 * Format the given header line to not exceed 72 characters.
 */
static int
fmt(str, np, fo, comma, dropinvalid, domime)
	char *str;
	struct name *np;
	FILE *fo;
	int comma, dropinvalid, domime;
{
	int col, len, count = 0;
	int is_to = 0;

	comma = comma ? 1 : 0;
	col = strlen(str);
	if (col) {
		fwrite(str, sizeof *str, strlen(str), fo);
		if ((col == 3 && asccasecmp(str, "to:") == 0) ||
			(col == 10 && asccasecmp(str, "Resent-To:") == 0))
			is_to = 1;
	}
	for (; np != NULL; np = np->n_flink) {
		if (is_to && is_fileaddr(np->n_name))
			continue;
		if (np->n_flink == NULL)
			comma = 0;
		if (mime_name_invalid(np->n_name, !dropinvalid)) {
			if (dropinvalid)
				continue;
			else
				return 1;
		}
		len = strlen(np->n_fullname);
		col++;		/* for the space */
		if (count && col + len + comma > 72 && col > 1) {
			fputs("\n ", fo);
			col = 1;
		} else
			putc(' ', fo);
		len = mime_write(np->n_fullname, sizeof *np->n_fullname,
				len, fo,
				domime?CONV_TOHDR_A:CONV_NONE,
				TD_ICONV, NULL, (size_t)0);
		if (comma && !(is_to && is_fileaddr(np->n_flink->n_name)))
			putc(',', fo);
		col += len + comma;
		count++;
	}
	putc('\n', fo);
	return 0;
}

/*
 * Rewrite a message for forwarding, adding the Resent-Headers.
 */
static int
infix_fw(fi, fo, mp, to, add_resent)
FILE *fi, *fo;
struct message *mp;
struct name *to;
int add_resent;
{
	size_t count;
	char *buf = NULL, *cp/*, *cp2*/;
	size_t c, bufsize = 0;

	count = mp->m_size;
	/*
	 * Write the original headers first.
	 */
	while (count > 0) {
		if ((cp = foldergets(&buf, &bufsize, &count, &c, fi)) == NULL)
			break;
		if (count > 0 && *buf == '\n')
			break;
		if (ascncasecmp("status: ", buf, 8) != 0
				&& strncmp("From ", buf, 5) != 0) {
			fwrite(buf, sizeof *buf, c, fo);
		}
	}
	/*
	 * Write the Resent-Headers, but only if the message
	 * has headers at all.
	 */
	if (count > 0) {
		if (add_resent) {
				fputs("Resent-", fo);
			date_field(fo);
			cp = myaddr();
			if (cp != NULL) {
				if (mime_name_invalid(cp, 1)) {
					if (buf)
						free(buf);
					return 1;
				}
				fwrite("Resent-From: ", sizeof (char), 13, fo);
				mime_write(cp, sizeof *cp, strlen(cp), fo,
						CONV_TOHDR_A, TD_ICONV,
						NULL, (size_t)0);
				putc('\n', fo);
				/*
				if ((cp2 = value("smtp")) == NULL
					|| strcmp(cp2, "localhost") == 0)
					fprintf(fo, "Resent-Sender: %s\n",
							username());
				*/
			}
#ifdef	notdef
			/*
			 * RFC 2822 disallows generation of this field.
			 */
			cp = value("replyto");
			if (cp != NULL) {
				if (mime_name_invalid(cp, 1)) {
					if (buf)
						free(buf);
					return 1;
				}
				fwrite("Resent-Reply-To: ", sizeof (char),
						17, fo);
				mime_write(cp, sizeof *cp, strlen(cp), fo,
						CONV_TOHDR_A, TD_ICONV,
						NULL, (size_t)0);
				putc('\n', fo);
			}
#endif	/* notdef */
			if (fmt("Resent-To:", to, fo, 1, 1, 0)) {
				if (buf)
					free(buf);
				return 1;
			}
			if (value("stealthmua") == NULL) {
				fputs("Resent-", fo);
				message_id(fo);
			}
		}
		putc('\n', fo);
		/*
		 * Write the message body.
		 */
		while (count > 0) {
			if (foldergets(&buf, &bufsize, &count, &c, fi) == NULL)
				break;
			fwrite(buf, sizeof *buf, c, fo);
		}
	}
	if (buf)
		free(buf);
	if (ferror(fo)) {
		perror(catgets(catd, CATSET, 188, "temporary mail file"));
		return 1;
	}
	return 0;
}

int
forward_msg(mp, to, add_resent)
struct message *mp;
struct name *to;
int add_resent;
{
	FILE *ibuf, *nfo, *nfi;
	char *tempMail;
	struct header head;
	enum okay	ok;

	memset(&head, 0, sizeof head);
	if ((to = checkaddrs(to)) == NULL) {
		senderr++;
		return 1;
	}
	if ((nfo = Ftemp(&tempMail, "Rs", "w", 0600, 1)) == NULL) {
		senderr++;
		perror(catgets(catd, CATSET, 189, "temporary mail file"));
		return 1;
	}
	if ((nfi = Fopen(tempMail, "r")) == NULL) {
		senderr++;
		perror(tempMail);
		return 1;
	}
	rm(tempMail);
	Ftfree(&tempMail);
	if ((ibuf = setinput(&mb, mp, NEED_BODY)) == NULL)
		return 1;
	head.h_to = to;
	to = fixhead(&head, to, 0);
	if (infix_fw(ibuf, nfo, mp, head.h_to, add_resent) != 0) {
		senderr++;
		rewind(nfo);
		savedeadletter(nfi);
		fputs(catgets(catd, CATSET, 190,
				". . . message not sent.\n"), stderr);
		Fclose(nfo);
		Fclose(nfi);
		return 1;
	}
	fflush(nfo);
	rewind(nfo);
	Fclose(nfo);
	to = outof(to, nfi, &head);
	if (senderr)
		savedeadletter(nfi);
	to = elide(to);
	if (count(to) != 0)
		ok = start_mta(to, head.h_smopts, nfi);
	else
		ok = OKAY;
	Fclose(nfi);
	return ok;
}
