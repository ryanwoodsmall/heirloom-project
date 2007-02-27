all: shl

shl: shl.o pslist.o shlid.o
	$(LD) $(LDFLAGS) shl.o pslist.o shlid.o $(LCOMMON) $(LWCHAR) $(LIBS) -o shl

shl.o: shl.c
	$(CC) $(CFLAGS) $(CPPFLAGS) $(GNUFL) $(IWCHAR) $(ICOMMON) -c shl.c

pslist.o: pslist.c
	$(CC) $(CFLAGS) $(CPPFLAGS) $(XO5FL) $(IWCHAR) $(ICOMMON) -c pslist.c

shlid.o: shlid.c
	$(CC) $(CFLAGS) $(CPPFLAGS) $(XO5FL) $(IWCHAR) $(ICOMMON) -c shlid.c

install: all
	u=`uname`; \
	if test "$$u" != FreeBSD && test "$$u" != HP-UX && \
		test "$$u" != AIX && test "$$u" != NetBSD && \
		test "$$u" != OpenBSD ; \
	then \
		$(UCBINST) -c $(TTYGRP) -m 2755 shl $(ROOT)$(DEFBIN)/shl &&\
		$(STRIP) $(ROOT)$(DEFBIN)/shl &&\
		$(MANINST) -c -m 644 shl.1 $(ROOT)$(MANDIR)/man1/shl.1; \
	else \
		exit 0; \
	fi

clean:
	rm -f shl shl.o pslist.o shlid.o core log *~

shl.o: shl.h
pslist.o: shl.h
shlid.o: shl.h
