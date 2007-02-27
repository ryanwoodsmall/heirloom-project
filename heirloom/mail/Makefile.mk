all: mail

mail: mail.o lock.o
	$(LD) $(LDFLAGS) mail.o lock.o $(LCOMMON) $(LWCHAR) $(LIBS) $(LSOCKET) -o mail

mail.o: mail.c
	$(CC) $(CFLAGS) $(CPPFLAGS) $(GNUFL) $(IWCHAR) $(ICOMMON) -c mail.c

lock.o: lock.c
	$(CC) $(CFLAGS) $(CPPFLAGS) $(XO6FL) $(IWCHAR) $(ICOMMON) -c lock.c

install: all
	$(UCBINST) -c mail $(ROOT)$(DEFBIN)/mail
	$(STRIP) $(ROOT)$(DEFBIN)/mail
	$(MANINST) -c -m 644 mail.1 $(ROOT)$(MANDIR)/man1/mail.1

clean:
	rm -f mail mail.o lock.o core log *~
