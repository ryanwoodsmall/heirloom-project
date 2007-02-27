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

links: fonts
	cd devpost && \
	for i in G HM HK HL; \
	do \
		rm -f $$i.out; ln -s H.out $$i.out || exit; \
	done && \
	rm -f GI.out; ln -s HI.out GI.out

install: all
	test -d $(ROOT)$(FNTDIR) || mkdir -p $(ROOT)$(FNTDIR)
	cp -R devpost $(ROOT)$(FNTDIR)

clean:
	rm -f makedev makedev.o devpost/*.out core log *~

mrproper: clean

makedev.o: makedev.c ../dev.h
