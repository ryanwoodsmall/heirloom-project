all: ed ed_s42 ed_sus

ed: ed.o
	$(LD) $(LDFLAGS) ed.o $(LCOMMON) $(LWCHAR) $(LIBS) -o ed

ed.o: ed.c
	$(CC) $(CFLAGS) $(CPPFLAGS) $(XO5FL) $(IWCHAR) $(ICOMMON) -DSHELL='"$(SHELL)"' -c ed.c

ed_s42: ed_s42.o
	$(LD) $(LDFLAGS) ed_s42.o $(LCOMMON) $(LUXRE) $(LWCHAR) $(LIBS) -o ed_s42

ed_s42.o: ed.c
	$(CC) $(CFLAGS) $(CPPFLAGS) $(XO5FL) $(IWCHAR) $(ICOMMON) $(IUXRE) -DS42 -DSHELL='"$(SHELL)"' -c ed.c -o ed_s42.o

ed_sus: ed_sus.o
	$(LD) $(LDFLAGS) ed_sus.o $(LCOMMON) $(LUXRE) $(LWCHAR) $(LIBS) -o ed_sus

ed_sus.o: ed.c
	$(CC) $(CFLAGS) $(CPPFLAGS) $(XO5FL) $(IWCHAR) $(ICOMMON) $(IUXRE) -DSUS -DSHELL='"$(SHELL)"' -c ed.c -o ed_sus.o

install: all
	$(UCBINST) -c ed $(ROOT)$(SV3BIN)/ed
	$(STRIP) $(ROOT)$(SV3BIN)/ed
	$(UCBINST) -c ed_s42 $(ROOT)$(S42BIN)/ed
	$(STRIP) $(ROOT)$(S42BIN)/ed
	$(UCBINST) -c ed_sus $(ROOT)$(SUSBIN)/ed
	$(STRIP) $(ROOT)$(SUSBIN)/ed
	$(MANINST) -c -m 644 ed.1 $(ROOT)$(MANDIR)/man1/ed.1

clean:
	rm -f ed ed.o ed_s42 ed_s42.o ed_sus ed_sus.o core log *~
