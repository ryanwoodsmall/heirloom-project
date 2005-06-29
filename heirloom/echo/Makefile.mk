all: echo

echo: echo.o main.o version.o
	$(LD) $(LDFLAGS) echo.o main.o version.o $(LCOMMON) $(LIBS) -o echo

echo.o: echo.c
	$(CC) $(CFLAGS) $(CPPFLAGS) $(XO5FL) $(LARGEF) $(ICOMMON) -c echo.c

main.o: main.c
	$(CC) $(CFLAGS) $(CPPFLAGS) $(XO5FL) $(LARGEF) $(ICOMMON) -Dfunc='echo' -c main.c

version.o: version.c
	$(CC) $(CFLAGS) $(CPPFLAGS) $(XO5FL) $(LARGEF) $(ICOMMON) -c version.c

install: all
	$(UCBINST) -c echo $(ROOT)$(DEFBIN)/echo
	$(STRIP) $(ROOT)$(DEFBIN)/echo
	$(MANINST) -c -m 644 echo.1 $(ROOT)$(MANDIR)/man1/echo.1

clean:
	rm -f echo echo.o main.o version.o core log *~

main.o: main.c defs.h
echo.o: echo.c defs.h
