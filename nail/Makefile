#
# Makefile for nail
#

#
# See the file INSTALL if you need help.
#

PREFIX		= /usr/local
BINDIR		= $(PREFIX)/bin
MANDIR		= $(PREFIX)/share/man
SYSCONFDIR	= /etc

MAILRC		= $(SYSCONFDIR)/nail.rc
MAILSPOOL	= /var/mail
SENDMAIL	= /usr/lib/sendmail

DESTDIR		=

UCBINSTALL	= /usr/ucb/install

#CFLAGS		=
#CPPFLAGS	=
#LDFLAGS		=
WARN		= -Wall -Wno-parentheses -Werror

# Some RedHat versions need INCLUDES = -I/usr/kerberos/include to compile
# with OpenSSL, or to compile with GSSAPI authentication included. In the
# latter case, they also need LDFLAGS = -L/usr/kerberos/lib.
#INCLUDES	= -I/usr/kerberos/include
#LDFLAGS	= -L/usr/kerberos/lib

SHELL		= /sbin/sh

# If you know that the IPv6 functions work on your machine, you can enable
# them here.
#IPv6		= -DHAVE_IPv6_FUNCS

###########################################################################
###########################################################################
# You should really know what you do if you change anything below this line
###########################################################################
###########################################################################

FEATURES	= -DMAILRC='"$(MAILRC)"' -DMAILSPOOL='"$(MAILSPOOL)"' \
			-DSENDMAIL='"$(SENDMAIL)"' $(IPv6)

OBJ = aux.o base64.o cache.o cmd1.o cmd2.o cmd3.o cmdtab.o collect.o \
	dotlock.o edit.o fio.o getname.o getopt.o head.o hmac.o \
	imap.o imap_search.o lex.o list.o lzw.o \
	macro.o main.o md5.o mime.o names.o pop3.o popen.o quit.o send.o \
	sendout.o smtp.o ssl.o strings.o temp.o thread.o tty.o \
	v7.local.o vars.o \
	version.o

.SUFFIXES: .o .c .x
.c.o:
	$(CC) $(CFLAGS) $(CPPFLAGS) $(FEATURES) $(INCLUDES) $(WARN) -c $<

.c.x:
	$(CC) $(CFLAGS) $(CPPFLAGS) $(FEATURES) $(INCLUDES) $(WARN) -E $< >$@

.c:
	$(CC) $(CFLAGS) $(CPPFLAGS) $(FEATURES) $(INCLUDES) $(WARN) \
		$(LDFLAGS) $< `grep '^[^#]' LIBS` $(LIBS) -o $@

all: nail

nail: $(OBJ) LIBS
	$(CC) $(LDFLAGS) $(OBJ) `grep '^[^#]' LIBS` $(LIBS) -o nail

$(OBJ): config.h def.h extern.h glob.h rcv.h
imap.o: imap_gssapi.c
md5.o imap.o hmac.o smtp.o aux.o pop3.o: md5.h

config.h LIBS:
	$(SHELL) ./makeconfig

install: all
	test -d $(DESTDIR)$(BINDIR) || mkdir -p $(DESTDIR)$(BINDIR)
	$(UCBINSTALL) -c -s nail $(DESTDIR)$(BINDIR)/nail
	test -d $(DESTDIR)$(MANDIR)/man1 || mkdir -p $(DESTDIR)$(MANDIR)/man1
	$(UCBINSTALL) -c -m 644 nail.1 $(DESTDIR)$(MANDIR)/man1/nail.1
	test -d $(DESTDIR)$(SYSCONFDIR) || mkdir -p $(DESTDIR)$(SYSCONFDIR)
	test -f $(DESTDIR)$(MAILRC) || \
		$(UCBINSTALL) -c -m 644 nail.rc $(DESTDIR)$(MAILRC)

clean:
	rm -f $(OBJ) nail *~ core log

mrproper: clean
	rm -f config.h config.log LIBS
