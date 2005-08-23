VPATH=..

FONTS = AB AI AR AX BI CB CI CO CW CX GR HB HI HX Hb Hi Hr Hx \
	KB KI KR KX NB NI NR NX PA PB PI PX S1 VB VI VR VX ZD ZI B H I R S

FLAGS = -I. -I.. -DFNTDIR='"$(FNTDIR)"'

.c.o:
	$(CC) $(CFLAGS) $(WARN) $(CPPFLAGS) $(FLAGS) -c $<

all: makedev fonts links

makedev: makedev.o
	$(CC) $(LDFLAGS) makedev.o $(LIBS) -o makedev

fonts: makedev
	cd devpost && \
	for i in DESC $(FONTS); \
	do \
		../makedev $$i || exit; \
	done
	cd dev7200 && \
	for i in DESC $(FONTS); \
	do \
		test -f $$i || continue; \
		../makedev $$i || exit; \
	done

links: fonts
	cd devpost && \
	for i in G HM HK HL; \
	do \
		rm -f $$i.out; ln -s H.out $$i.out || exit; \
	done && \
	rm -f GI.out; ln -s HI.out GI.out

install: all
	$(INSTALL) -c makedev $(ROOT)$(BINDIR)/makedev
	$(STRIP) $(ROOT)$(BINDIR)/makedev
	test -d $(ROOT)$(FNTDIR) || mkdir -p $(ROOT)$(FNTDIR)
	cp -R devpost $(ROOT)$(FNTDIR)
	cp -R dev7200 $(ROOT)$(FNTDIR)
	test -d $(ROOT)$(FNTDIR)/dev7200/pfb || \
		mkdir -p $(ROOT)$(FNTDIR)/dev7200/pfb
	rm -f $(ROOT)$(FNTDIR)/dev7200/charlib
	ln -s ../devpost/charlib $(ROOT)$(FNTDIR)/dev7200/charlib
	rm -f $(ROOT)$(FNTDIR)/dev7200/postscript
	ln -s ../devpost/postscript $(ROOT)$(FNTDIR)/dev7200/postscript

clean:
	rm -f makedev makedev.o devpost/*.out dev7200/*.out core log *~

mrproper: clean

makedev.o: makedev.c ../dev.h
