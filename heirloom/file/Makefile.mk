all: file

file: file.o
	$(LD) $(LDFLAGS) file.o $(LCOMMON) $(LWCHAR) $(LIBS) -o file

file.o: file.c
	$(CC) $(CFLAGS) $(CPPFLAGS) $(GNUFL) $(LARGEF) $(IWCHAR) $(ICOMMON) -DMAGIC='"$(MAGIC)"' -c file.c

install: all
	$(UCBINST) -c file $(ROOT)$(DEFBIN)/file
	$(STRIP) $(ROOT)$(DEFBIN)/file
	$(UCBINST) -c -m 644 magic $(ROOT)$(MAGIC)
	$(MANINST) -c -m 644 file.1 $(ROOT)$(MANDIR)/man1/file.1

clean:
	rm -f file file.o core log *~
