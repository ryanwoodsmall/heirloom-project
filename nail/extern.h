/*
 * Nail - a mail user agent derived from Berkeley Mail.
 *
 * Copyright (c) 2000-2002 Gunnar Ritter, Freiburg i. Br., Germany.
 */
/*-
 * Copyright (c) 1992, 1993
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
 *
 *	Sccsid @(#)extern.h	2.101 (gritter) 9/9/04
 */

struct name *cat __P((struct name *, struct name *));
struct name *elide __P((struct name *));
struct name *extract __P((char [], int));
struct name *sextract __P((char [], int));
struct name *nalloc __P((char [], int));
struct name *outof __P((struct name *, FILE *, struct header *));
struct name *usermap __P((struct name *));
FILE	*safe_fopen __P((char *, char *, int *));
FILE	*Fdopen __P((int, char *));
FILE	*Fopen __P((char *, char *));
FILE	*Popen __P((char *, char *, char *, int));
FILE	*collect __P((struct header *, int, struct message*, char *, int));
char	*detract __P((struct name *, enum gfield));
char	*expand __P((char *));
char	*getdeadletter __P((void));
char	*getname __P((int));
char	*hfield_mult __P((char [], struct message *, int));
#define	hfield(a, b)	hfield_mult(a, b, 1)
char	*nameof __P((struct message *, int));
char	*name1 __P((struct message *, int));
FILE	*run_editor __P((FILE *, off_t, int, int, char *, struct header *));
void	*salloc __P((size_t));
void	*csalloc __P((size_t, size_t));
char	*savestr __P((const char *));
char	*save2str __P((const char *, const char *));
FILE	*setinput __P((struct mailbox *, struct message *, enum needspec));
char	*routeaddr __P((const char *));
char	*skip_comment __P((const char *));
char	*skin __P((char *));
char	*realname __P((char *));
char	*username __P((void));
char	*value __P((char []));
char	*vcopy __P((char []));
int	 Fclose __P((FILE *));
int	 More __P((void *));
int	 Pclose __P((FILE *));
int	 Respond __P((void *));
int	 Followup __P((void *));
int	 Type __P((void *));
int	 Pipecmd __P((void *));
int	 pipecmd __P((void *));
int	 Forwardcmd __P((void *));
int	 forwardcmd __P((void *));
void	 alter __P((char *));
int	 alternates __P((void *));
void	 announce __P((int));
int	 anyof __P((char *, char *));
int	 argcount __P((char **));
void	 assign __P((char [], char []));
int	 blankline __P((char []));
int	 clobber __P((void *));
void	 close_all_files __P((void));
void	 commands __P((void));
int	 copycmd __P((void *));
int	 Copycmd __P((void *));
int	 core __P((void *));
int	 count __P((struct name *));
int	 delete __P((void *));
int	 deltype __P((void *));
void	 demail __P((void));
int	 dosh __P((void *));
int	 dot_lock __P((const char *, int, int, FILE *, const char *));
void	 dot_unlock __P((const char *));
int	 echo __P((void *));
int	 editor __P((void *));
int	 elsecmd __P((void *));
int	 endifcmd __P((void *));
int	 execute __P((char [], int, size_t));
int	 fcntl_lock __P((int, int));
int	 file __P((void *));
struct grouphead	*findgroup __P((char []));
void	remove_group __P((const char *));
void	 findmail __P((char *, int, char *, int));
int	 first __P((int, int));
int	 folders __P((void *));
void	 free_child __P((int));
int	 from __P((void *));
off_t	 fsize __P((FILE *));
int	 getfold __P((char *, int));
int	 getmsglist __P((char *, int *, int));
int	 getrawlist __P((char [], size_t, char **, int, int));
int	 getuserid __P((char []));
int	 grabh __P((struct header *, enum gfield, int));
int	 group __P((void *));
int	 ungroup __P((void *));
int	 hash __P((const char *));
int	 headers __P((void *));
int	 help __P((void *));
void	 holdsigs __P((void));
int	 ifcmd __P((void *));
int	 igfield __P((void *));
int	unignore __P((void *));
int	unretain __P((void *));
int	unsaveignore __P((void *));
int	unsaveretain __P((void *));
int	 is_dir __P((char []));
int	 is_fileaddr __P((char *));
int	 is_head __P((char *, size_t));
int	 is_ign __P((char *, size_t, struct ignoretab []));
void	 i_strcpy __P((char *, char *, int));
void	 load __P((char *));
int	 mail __P((struct name *,
	    struct name *, struct name *, struct name *,
	   	 char *, struct attachment *, char *, int, int));
enum okay	 mail1 __P((struct header *, int, struct message *,
			char *, int, int));
int	 mboxit __P((void *));
int	 member __P((char *, struct ignoretab *));
int	 messize __P((void *));
int	 more __P((void *));
int	 newfileinfo __P((void));
int	 newmailinfo __P((int));
int	 next __P((void *));
void	 panic __P((const char *, ...))
#ifdef	__GNUC__
    __attribute__((__format__(__printf__,1,2),__noreturn__));
#else
    ;
#endif
#ifndef	HAVE_SNPRINTF
int	 snprintf __P((char *, size_t, const char *, ...))
#ifdef	__GNUC__
    __attribute__((__format__(__printf__,1,2)));
#else
    ;
#endif
#endif
void	 parse __P((char *, size_t, struct headline *, char *));
int	 pcmdlist __P((void *));
int	 pdot __P((void *));
void	 prepare_child __P((sigset_t *, int, int));
int	 preserve __P((void *));
void	 prettyprint __P((struct name *));
void	 printgroup __P((char []));
void	 printhead __P((int, FILE *, int));
int	 puthead __P((struct header *, FILE *, enum gfield, int, char *,
			char *));
int	 putline __P((FILE *, char *, size_t));
int	 pversion __P((void *));
void	 quit __P((void));
int	 quitcmd __P((void *));
char 	*readtty __P((char *, char *));
void	 relsesigs __P((void));
int	 respond __P((void *));
int	 respondall __P((void *));
int	 respondsender __P((void *));
int	 followup __P((void *));
int	 followupall __P((void *));
int	 followupsender __P((void *));
int	 retfield __P((void *));
int	 rexit __P((void *));
int	 rm __P((char *));
int	 run_command __P((char *, sigset_t *, int, int, char *, char *, char *));
int	 save __P((void *));
int	 Save __P((void *));
void	 savedeadletter __P((FILE *));
int	 saveigfield __P((void *));
int	 saveretfield __P((void *));
int	 schdir __P((void *));
int	 screensize __P((void));
int	 scroll __P((void *));
int	 send_message __P((struct message *, FILE *,
			struct ignoretab *, char *, enum conversion, off_t *));
int	 sendmail __P((void *));
int	 Sendmail __P((void *));
int	 set __P((void *));
int	 setfile __P((char *, int));
void	 setptr __P((FILE *, off_t));
int	 shell __P((void *));
void	 sigchild __P((int));
int	 source __P((void *));
void	 spreserve __P((void));
void	 sreset __P((void));
int	 start_command __P((char *, sigset_t *, int, int, char *, char *, char *));
int	 stouch __P((void *));
int	 cwrite __P((void *));
void	 tinit __P((void));
int	 top __P((void *));
void	 touch __P((struct message *));
int	 type __P((void *));
int	 undeletecmd __P((void *));
char	**unpack __P((struct name *));
int	 unread __P((void *));
int	 unset __P((void *));
int	 unstack __P((void));
int	 visual __P((void *));
int	 wait_child __P((int));
void	*smalloc __P((size_t));
void	*srealloc __P((void *, size_t));
void	*scalloc __P((size_t, size_t));
char	*sstrdup __P((const char *));
char	*sstpcpy __P((char *, const char *));
char	*itostr __P((unsigned, unsigned, char *));
size_t	mime_write_tob64 __P((struct str*, FILE*, int));
void	mime_fromb64 __P((struct str*, struct str*, int));
void	mime_fromb64_b __P((struct str*, struct str*, int, FILE*));
enum mimeenc	mime_getenc __P((char*));
int	mime_getcontent __P((char*));
char	*mime_filecontent __P((char*));
char	*mime_getparam __P((char*,char*));
char	*mime_getboundary __P((char*));
void	mime_fromhdr __P((struct str*, struct str*, enum tdflags));
char	*mime_fromaddr __P((char *));
size_t	mime_write __P((void*, size_t, size_t, FILE*, enum conversion,
			enum tdflags, char *, size_t));
sighandler_type safe_signal __P((int, sighandler_type));
char	*laststring __P((char *, int *, int));
int	forward_msg __P((struct message *, struct name *, int));
int	smtp_mta __P((char *, struct name *, FILE *));
char	*nodename __P((int));
int	mime_name_invalid __P((char *, int));
struct name	*checkaddrs __P((struct name *));
char	*myaddr __P((void));
char	*gettcharset __P((void));
size_t	makeprint __P((char *, size_t));
char	*makeprint0 __P((char *));
#ifdef	HAVE_ICONV
iconv_t	iconv_open_ft __P((const char *, const char *));
#endif
size_t	prefixwrite __P((void *, size_t, size_t, FILE *, char *, size_t));
FILE	*Ftemp __P((char **, char *, char *, int, int));
void	Ftfree __P((char **));
struct attachment	*edit_attachments __P((struct attachment *));
struct attachment	*add_attachment __P((struct attachment *, const char *));
struct name	*delete_alternates __P((struct name *));
int	is_myname __P((char *));
int	unset_internal __P((char *));
char	*get_pager __P((void));
struct message	*setdot __P((struct message *));
char	*fgetline __P((char **, size_t *, size_t *, size_t *, FILE *, int));
char	*foldergets __P((char **, size_t *, size_t *, size_t *, FILE *));
int	readline_restart __P((FILE *, char **, size_t *, size_t));
#define	readline(a, b, c)	readline_restart(a, b, c, 0)
int	asccasecmp __P((const char *, const char *));
int	ascncasecmp __P((const char *, const char *, size_t));
char	*asccasestr __P((const char *, const char *));
int	get_mime_convert __P((FILE *, char **, char **, enum mimeclean *));
void	newline_appended __P((void));
int	newmail __P((void *));
int	pop3_setfile __P((const char *, int, int));
enum okay	pop3_header __P((struct message *));
enum okay	pop3_body __P((struct message *));
void	pop3_quit __P((void));
int	imap_setfile __P((const char *, int, int));
enum okay	imap_select __P((struct mailbox *, off_t *, int *,
			const char *));
enum okay	imap_header __P((struct message *));
enum okay	imap_body __P((struct message *));
void	imap_getheaders __P((int, int));
void	imap_quit __P((void));
int	imap_newmail __P((int));
enum okay	imap_undelete __P((struct message *, int));
enum okay	imap_unread __P((struct message *, int));
enum okay	imap_copy __P((struct message *, int, const char *));
enum okay	imap_search __P((const char *, int));
enum okay	imap_search1 __P((const char *, int));
int	imap_thisaccount __P((const char *));
int	imap_imap __P((void *));
enum okay	imap_append __P((const char *, FILE *));
void	imap_folders __P((const char *, int));
enum okay	imap_dequeue __P((struct mailbox *, FILE *));
time_t	imap_read_date_time __P((const char *));
time_t	imap_read_date __P((const char *));
const char	*imap_make_date_time __P((time_t));
char	*imap_quotestr __P((const char *));
char	*imap_unquotestr __P((const char *));
enum protocol	which_protocol __P((const char *));
void	initbox __P((const char *));
void	setmsize __P((int));
char	*fakefrom __P((struct message *));
char	*fakedate __P((time_t));
int	holdbits __P((void));
enum okay	makembox();
char	*nexttoken __P((char *));
enum okay	get_body __P((struct message *));
int	shortcut __P((void *));
int	unshortcut __P((void *));
struct shortcut	*get_shortcut __P((const char *));
int	account __P((void *));
void	out_of_memory __P((void));
time_t	rfctime __P((char *));
time_t	unixtime __P((char *));
void	extract_header __P((FILE *, struct header *));
char	*need_hdrconv __P((struct header *, enum gfield));
char	*savecat __P((const char *, const char *));
FILE	*Zopen __P((char *, char *, int *));
char	*strtob64 __P((const char *));
void	*memtob64 __P((const void *, size_t));
#ifdef	USE_SSL
enum okay ssl_open __P((const char *, struct sock *, const char *));
void ssl_gen_err __P((const char *));
#endif	/* USE_SSL */
time_t	combinetime __P((int, int, int, int, int, int));
char	*protbase __P((const char *));
const char	*protfile __P((const char *));
#ifdef	HAVE_SOCKETS
int	sclose __P((struct sock *));
enum okay	swrite __P((struct sock *, const char *));
enum okay	swrite1 __P((struct sock *, const char *, int, int));
int	sgetline __P((char **, size_t *, size_t *, struct sock *));
void	sunget __P((struct sock *, const char *));
enum okay	sopen __P((const char *, struct sock *, int,
			const char *, const char *, int));
#endif	/* HAVE_SOCKETS */
char	*strenc __P((const char *));
char	*strdec __P((const char *));
enum okay	getcache __P((struct mailbox *, struct message *,
			enum needspec));
enum okay	getcache1 __P((struct mailbox *, struct message *,
			enum needspec, int));
void	putcache __P((struct mailbox *, struct message *));
void	initcache __P((struct mailbox *));
void	purgecache __P((struct mailbox *, struct message *, long));
void	delcache __P((struct mailbox *, struct message *));
enum okay	cache_setptr __P((int));
enum okay	cache_list __P((struct mailbox *, const char *, int, FILE *));
unsigned long	cached_uidvalidity __P((struct mailbox *));
FILE	*cache_queue __P((struct mailbox *));
enum okay	cache_dequeue __P((struct mailbox *));
void	*zalloc __P((FILE *));
int	zread __P((void *, char *, int));
int	zwrite __P((void *, const char *, int));
int	zfree __P((void *));
int	disconnected __P((const char *));
int	getmdot __P((int));
int	thread __P((void *));
int	unthread __P((void *));
int	msgidcmp __P((const char *, const char *));
struct message	*next_in_thread __P((struct message *));
struct message	*prev_in_thread __P((struct message *));
struct message	*this_in_thread __P((struct message *, long));
char	*md5tohex __P((const void *));
char	*cram_md5_string __P((const char *, const char *, const char *));
char	*getuser __P((void));
char	*getpassword __P((struct termios *, int *));
int	cconnect __P((void *));
int	cdisconnect __P((void *));
void	transflags __P((struct message *, long, int));
int	sort __P((void *));
int	is_prefix __P((const char *, const char *));
void	makelow __P((char *));
int	ccache __P((void *));
void	mark __P((int, int));
int	substr __P((const char *, const char *));
char	*last_at_before_slash __P((const char *));
int	cflag __P((void *));
int	cunflag __P((void *));
int	canswered __P((void *));
int	cunanswered __P((void *));
int	cdraft __P((void *));
int	ckill __P((void *));
int	cunkill __P((void *));
int	cundraft __P((void *));
int	cscore __P((void *));
int	cdefine __P((void *));
int	cdefines __P((void *));
int	define1 __P((const char *, int));
int	cundef __P((void *));
void	delaccount __P((const char *));
int	ccall __P((void *));
int	callaccount __P((const char *));
int	listaccounts __P((FILE *));
int	callhook __P((const char *, int));
int	cmove __P((void *));
int	cMove __P((void *));
char	*colalign __P((const char *, int, int));
int	cnoop __P((void *));
enum okay	pop3_noop __P((void));
enum okay	imap_noop __P((void));
void	try_pager __P((FILE *));
enum okay	cwget __P((struct cw *));
enum okay	cwret __P((struct cw *));
void	cwrelse __P((struct cw *));
enum okay	makedir __P((const char *));
int	maildir_setfile __P((const char *, int, int));
void	maildir_quit __P((void));
enum okay	maildir_append __P((const char *, FILE *));
unsigned	pjw __P((const char *));
long	nextprime __P((long));
extern void	substdate __P((struct message *));
int	ccollapse __P((void *));
int	cuncollapse __P((void *));
void	uncollapse1 __P((struct message *, int));
