all: cpio pax

cpio: cpio.o unshrink.o explode.o expand.o inflate.o crc32.o blast.o flags.o nonpax.o
	$(LD) $(LDFLAGS) cpio.o unshrink.o explode.o expand.o inflate.o crc32.o blast.o flags.o nonpax.o $(LIBZ) $(LIBBZ2) $(LCOMMON) $(LWCHAR) $(LIBS) -o cpio

pax: cpio pax.o
	$(LD) $(LDFLAGS) cpio.o unshrink.o explode.o expand.o inflate.o crc32.o blast.o pax.o $(LIBZ) $(LIBBZ2) $(LCOMMON) $(LUXRE) $(LWCHAR) $(LIBS) -o pax

cpio.o: cpio.c
	$(CC) $(CFLAGS) $(CPPFLAGS) $(GNUFL) $(LARGEF) $(IWCHAR) $(ICOMMON) -DUSE_ZLIB=$(USE_ZLIB) -DUSE_BZLIB=$(USE_BZLIB) -c cpio.c

flags.o: flags.c
	$(CC) $(CFLAGS) $(CPPFLAGS) $(GNUFL) $(LARGEF) $(IWCHAR) $(ICOMMON) -c flags.c

nonpax.o: nonpax.c
	$(CC) $(CFLAGS) $(CPPFLAGS) $(GNUFL) $(LARGEF) $(IWCHAR) $(ICOMMON) -c nonpax.c

pax.o: pax.c
	$(CC) $(CFLAGS) $(CPPFLAGS) $(GNUFL) $(LARGEF) $(IWCHAR) $(ICOMMON) $(IUXRE) -c pax.c

unshrink.o: unshrink.c
	$(CC) $(CFLAGS2) $(CPPFLAGS) $(GNUFL) $(LARGEF) $(IWCHAR) $(ICOMMON) -c unshrink.c

explode.o: explode.c
	$(CC) $(CFLAGS2) $(CPPFLAGS) $(GNUFL) $(LARGEF) $(IWCHAR) $(ICOMMON) -c explode.c

expand.o: expand.c
	$(CC) $(CFLAGS2) $(CPPFLAGS) $(GNUFL) $(LARGEF) $(IWCHAR) $(ICOMMON) -c expand.c

inflate.o: inflate.c
	$(CC) $(CFLAGS2) $(CPPFLAGS) $(GNUFL) $(LARGEF) $(IWCHAR) $(ICOMMON) -c inflate.c

crc32.o: crc32.c
	$(CC) $(CFLAGS2) $(CPPFLAGS) $(GNUFL) $(LARGEF) $(IWCHAR) $(ICOMMON) -c crc32.c

blast.o: blast.c
	$(CC) $(CFLAGS2) $(CPPFLAGS) $(GNUFL) $(LARGEF) $(IWCHAR) $(ICOMMON) -c blast.c

install: all
	$(UCBINST) -c cpio $(ROOT)$(DEFBIN)/cpio
	$(STRIP) $(ROOT)$(DEFBIN)/cpio
	$(UCBINST) -c pax $(ROOT)$(DEFBIN)/pax
	$(STRIP) $(ROOT)$(DEFBIN)/pax
	$(MANINST) -c -m 644 cpio.1 $(ROOT)$(MANDIR)/man1/cpio.1
	$(MANINST) -c -m 644 pax.1 $(ROOT)$(MANDIR)/man1/pax.1

clean:
	rm -f cpio cpio.o unshrink.o explode.o expand.o inflate.o crc32.o blast.o flags.o nonpax.o pax pax.o core log *~

cpio.o: cpio.h blast.h
unshrink.o: cpio.h unzip.h
explode.o: cpio.h unzip.h
expand.o: cpio.h
inflate.o: cpio.h unzip.h
crc32.o: cpio.h
blast.o: blast.h
flags.o: cpio.h
pax.o: cpio.h
nonpax.o: cpio.h
