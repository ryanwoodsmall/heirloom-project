all: getconf getconf_su3

PATHS = -DSUSBIN='"$(SUSBIN)"' -DSU3BIN='"$(SU3BIN)"' -DDEFBIN='"$(DEFBIN)"'

getconf: getconf.o
	$(LD) $(LDFLAGS) getconf.o $(LCOMMON) $(LWCHAR) $(LIBS) -o getconf

getconf_su3: getconf_su3.o
	$(LD) $(LDFLAGS) getconf_su3.o $(LCOMMON) $(LWCHAR) $(LIBS) -o getconf_su3

getconf.o: getconf.c
	$(CC) $(CFLAGS) $(CPPFLAGS) $(GNUFL) $(LARGEF) $(IWCHAR) $(ICOMMON) $(PATHS) -c getconf.c

getconf_su3.o: getconf.c
	$(CC) $(CFLAGS) $(CPPFLAGS) $(GNUFL) $(LARGEF) $(IWCHAR) $(ICOMMON) $(PATHS) -DSU3 -c getconf.c -o getconf_su3.o

install: all
	$(UCBINST) -c getconf $(ROOT)$(SUSBIN)/getconf
	$(STRIP) $(ROOT)$(SUSBIN)/getconf
	$(UCBINST) -c getconf_su3 $(ROOT)$(SU3BIN)/getconf
	$(STRIP) $(ROOT)$(SU3BIN)/getconf
	$(MANINST) -c -m 644 getconf.1 $(ROOT)$(MANDIR)/man1/getconf.1

clean:
	rm -f getconf getconf.o getconf_su3 getconf_su3.o core log *~
