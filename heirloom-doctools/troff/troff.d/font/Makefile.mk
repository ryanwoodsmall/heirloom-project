VPATH=..

FONTS = AB AI AR AX BI CB CI CO CW CX GR HB HI HX Hb Hi Hr Hx \
	KB KI KR KX NB NI NR NX PA PB PI PX S1 VB VI VR VX ZD ZI B H I R S

FLAGS = -I. -I.. -DFNTDIR='"$(FNTDIR)"'

.c.o:
	$(CC) $(CFLAGS) $(WARN) $(CPPFLAGS) $(FLAGS) -c $<

all:

install: all
	test -d $(ROOT)$(FNTDIR) || mkdir -p $(ROOT)$(FNTDIR)
	cp -R devpost $(ROOT)$(FNTDIR)
	cd $(ROOT)$(FNTDIR)/devpost && \
		for i in G HM HK HL; \
		do \
			rm -f $$i; ln -s H $$i || exit; \
		done && \
		rm -f GI; ln -s HI GI
	cp -R dev7200 $(ROOT)$(FNTDIR)
	test -d $(ROOT)$(FNTDIR)/dev7200/pfb || \
		mkdir -p $(ROOT)$(FNTDIR)/dev7200/pfb
	rm -f $(ROOT)$(FNTDIR)/dev7200/charlib
	ln -s ../devpost/charlib $(ROOT)$(FNTDIR)/dev7200/charlib
	rm -f $(ROOT)$(FNTDIR)/dev7200/postscript
	ln -s ../devpost/postscript $(ROOT)$(FNTDIR)/dev7200/postscript

clean:
	rm -f core log *~

mrproper: clean
