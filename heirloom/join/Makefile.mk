all: join join_sus

join: join.o
	$(LD) $(LDFLAGS) join.o $(LCOMMON) $(LWCHAR) $(LIBS) -o join

join.o: join.c
	$(CC) $(CFLAGS) $(CPPFLAGS) $(XO6FL) $(LARGEF) $(IWCHAR) $(ICOMMON) -c join.c

join_sus: join_sus.o
	$(LD) $(LDFLAGS) join_sus.o $(LCOMMON) $(LWCHAR) $(LIBS) -o join_sus

join_sus.o: join.c
	$(CC) $(CFLAGS) $(CPPFLAGS) $(XO6FL) $(LARGEF) $(IWCHAR) $(ICOMMON) -DSUS -c join.c -o join_sus.o

install: all
	$(UCBINST) -c join $(ROOT)$(SV3BIN)/join
	$(STRIP) $(ROOT)$(SV3BIN)/join
	$(UCBINST) -c join_sus $(ROOT)$(SUSBIN)/join
	$(STRIP) $(ROOT)$(SUSBIN)/join
	$(MANINST) -c -m 644 join.1 $(ROOT)$(MANDIR)/man1/join.1

clean:
	rm -f join join.o join_sus join_sus.o core log *~
