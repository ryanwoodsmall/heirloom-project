all: find find_sus

find: find.o
	$(LD) $(LDFLAGS) find.o $(LCOMMON) $(LWCHAR) $(LIBS) -o find

find.o: find.c
	$(CC) $(CFLAGS) $(CPPFLAGS) $(LARGEF) $(IWCHAR) $(ICOMMON) $(GNUFL) -DGETDIR -c find.c

find_sus: find_sus.o
	$(LD) $(LDFLAGS) find_sus.o $(LCOMMON) $(LWCHAR) $(LIBS) -o find_sus

find_sus.o: find.c
	$(CC) $(CFLAGS) $(CPPFLAGS) $(LARGEF) $(IWCHAR) $(ICOMMON) $(GNUFL) -DGETDIR -DSUS -c find.c -o find_sus.o

install: all
	$(UCBINST) -c find $(ROOT)$(SV3BIN)/find
	$(STRIP) $(ROOT)$(SV3BIN)/find
	$(UCBINST) -c find_sus $(ROOT)$(SUSBIN)/find
	$(STRIP) $(ROOT)$(SUSBIN)/find
	$(MANINST) -c -m 644 find.1 $(ROOT)$(MANDIR)/man1/find.1

clean:
	rm -f find find.o find_sus find_sus.o core log *~
