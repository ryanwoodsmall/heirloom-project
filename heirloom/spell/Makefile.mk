#
# from v7 /usr/src/cmd/spell/makefile
#
all:	spell spellprog spellin spellout hlist hlista hlistb hstop

spell: spell.sh
	echo '#!$(SHELL)' | cat - spell.sh | sed ' \
		s,@DEFBIN@,$(DEFBIN),g; \
		s,@SV3BIN@,$(SV3BIN),g; \
		s,@DEFLIB@,$(DEFLIB),g; \
		s,@SPELLHIST@,$(SPELLHIST),g' >spell
	chmod 755 spell

spellprog: spellprog.o
	$(LD) $(LDFLAGS) spellprog.o $(LCOMMON) $(LIBS) -o spellprog

spellin: spellin.o
	$(LD) $(LDFLAGS) spellin.o $(LCOMMON) $(LIBS) -o spellin

host_spellin: spellin.c
	$(HOSTCC) spellin.c -o host_spellin

spellout: spellout.o
	$(LD) $(LDFLAGS) spellout.o $(LCOMMON) $(LIBS) -o spellout

spellin.o: spellin.c
	$(CC) $(CFLAGS) $(CPPFLAGS) $(XO5FL) $(LARGEF) -c spellin.c

spellout.o: spellout.c
	$(CC) $(CFLAGS) $(CPPFLAGS) $(XO5FL) $(LARGEF) -c spellout.c

spellprog.o: spellprog.c
	$(CC) $(CFLAGS) $(CPPFLAGS) $(XO5FL) $(LARGEF) -c spellprog.c


install:	all
	$(UCBINST) -c spell $(ROOT)$(DEFBIN)/spell
	test -d $(ROOT)$(DEFLIB)/spell || mkdir -p $(ROOT)$(DEFLIB)/spell
	$(UCBINST) -c spellprog $(ROOT)$(DEFLIB)/spell/spellprog
	$(STRIP) $(ROOT)$(DEFLIB)/spell/spellprog
	$(UCBINST) -c spellin $(ROOT)$(DEFLIB)/spell/spellin
	$(STRIP) $(ROOT)$(DEFLIB)/spell/spellin
	$(UCBINST) -c spellout $(ROOT)$(DEFLIB)/spell/spellout
	$(STRIP) $(ROOT)$(DEFLIB)/spell/spellout
	$(UCBINST) -c -m 644 hlista $(ROOT)$(DEFLIB)/spell/hlista
	$(UCBINST) -c -m 644 hlistb $(ROOT)$(DEFLIB)/spell/hlistb
	$(UCBINST) -c -m 644 hstop $(ROOT)$(DEFLIB)/spell/hstop
	$(MANINST) -c -m 644 spell.1 $(ROOT)$(MANDIR)/man1/spell.1
	test -d `dirname $(ROOT)$(SPELLHIST)` || \
		mkdir -p `dirname $(ROOT)$(SPELLHIST)`
	-test -r $(ROOT)$(SPELLHIST) || { \
		touch $(ROOT)$(SPELLHIST); \
		chmod 666 $(ROOT)$(SPELLHIST); \
	}

hlist: words host_spellin
	./host_spellin <words >hlist
hlista: american local hlist host_spellin
	(cat american local)|./host_spellin hlist >hlista
hlistb: british local hlist host_spellin
	(cat british local)|./host_spellin hlist >hlistb
hstop: stop spellin
	./host_spellin <stop >hstop

clean:
	rm -f spellprog hlista hlistb hstop hlist spellin spellout \
		spell spellprog.o spellin.o spellout.o host_spellin core log *~


spellin.o: spell.h
spellout.o: spell.h
spellprog.o: spell.h
host_spellin: spell.h
