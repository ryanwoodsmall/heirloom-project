all: csplit csplit_sus

csplit: csplit.o
	$(LD) $(LDFLAGS) csplit.o $(LCOMMON) $(LWCHAR) $(LIBS) -lm -o csplit

csplit.o: csplit.c
	$(CC) $(CFLAGS) $(CPPFLAGS) $(XO6FL) $(LARGEF) $(IWCHAR) $(ICOMMON) -c csplit.c

csplit_sus: csplit_sus.o
	$(LD) $(LDFLAGS) csplit_sus.o $(LUXRE) $(LCOMMON) $(LWCHAR) $(LIBS) -lm -o csplit_sus

csplit_sus.o: csplit.c
	$(CC) $(CFLAGS) $(CPPFLAGS) $(XO6FL) $(LARGEF) $(IUXRE) $(IWCHAR) $(ICOMMON) -DSUS -c csplit.c -o csplit_sus.o

install: all
	$(UCBINST) -c csplit $(ROOT)$(SV3BIN)/csplit
	$(STRIP) $(ROOT)$(SV3BIN)/csplit
	$(UCBINST) -c csplit_sus $(ROOT)$(SUSBIN)/csplit
	$(STRIP) $(ROOT)$(SUSBIN)/csplit
	$(MANINST) -c -m 644 csplit.1 $(ROOT)$(MANDIR)/man1/csplit.1

clean:
	rm -f csplit csplit.o csplit_sus csplit_sus.o core log *~
