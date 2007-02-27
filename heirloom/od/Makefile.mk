all: od

od: od.o
	$(LD) $(LDFLAGS) od.o $(LCOMMON) $(LWCHAR) $(LIBS) -o od

od.o: od.c
	$(CC) $(CFLAGS) $(CPPFLAGS) $(XO6FL) $(LARGEF) $(IWCHAR) $(ICOMMON) -c od.c

install: all
	$(UCBINST) -c od $(ROOT)$(DEFBIN)/od
	$(STRIP) $(ROOT)$(DEFBIN)/od
	$(MANINST) -c -m 644 od.1 $(ROOT)$(MANDIR)/man1/od.1

clean:
	rm -f od od.o core log *~
