XOBJ = main.o sub1.o sub2.o sub3.o header.o wcio.o parser.o getopt.o lsearch.o

LOBJ = allprint.o libmain.o reject.o yyless.o yywrap.o

.c.o: ; $(CC) -c $(CFLAGS) $(CPPFLAGS) $(WARN) -DFORMPATH='"$(LIBDIR)"' $<

all: lex libl.a

lex: $(XOBJ)
	$(CC) $(LDFLAGS) $(XOBJ) $(LIBS) -o lex

libl.a: $(LOBJ)
	$(AR) -rv libl.a $(LOBJ)

install: all
	test -d $(ROOT)$(BINDIR) || mkdir -p $(ROOT)$(BINDIR)
	test -d $(ROOT)$(LIBDIR) || mkdir -p $(ROOT)$(LIBDIR)
	$(INSTALL) -c -m 755 lex $(ROOT)$(BINDIR)/lex
	$(STRIP) $(ROOT)$(BINDIR)/lex
	$(INSTALL) -c -m 644 ncform $(ROOT)$(LIBDIR)/ncform
	$(INSTALL) -c -m 644 nceucform $(ROOT)$(LIBDIR)/nceucform
	$(INSTALL) -c -m 644 libl.a $(ROOT)$(LIBDIR)/libl.a
	test -d $(ROOT)$(MANDIR)/man1 || mkdir -p $(ROOT)$(MANDIR)/man1
	$(INSTALL) -c -m 644 lex.1 $(ROOT)$(MANDIR)/man1/lex.1

clean:
	rm -f lex libl.a $(XOBJ) $(LOBJ) parser.c core log *~

mrproper: clean

allprint.o: allprint.c
header.o: header.c ldefs.c
ldefs.o: ldefs.c
libmain.o: libmain.c
main.o: main.c once.h ldefs.c sgs.h
reject.o: reject.c
sub1.o: sub1.c ldefs.c
sub2.o: sub2.c ldefs.c
sub3.o: sub3.c ldefs.c search.h
yyless.o: yyless.c
yywrap.o: yywrap.c
lsearch.o: search.h
