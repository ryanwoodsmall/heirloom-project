VPATH=..
OBJ = dpost.o dpost_draw.o color.o pictures.o ps_include.o dpost_afm.o \
	dpost_makedev.o glob.o misc.o request.o dpost_version.o getopt.o

FLAGS = -I. -I.. -DFNTDIR='"$(FNTDIR)"' -DPSTDIR='"$(PSTDIR)"'

.c.o:
	$(CC) $(CFLAGS) $(WARN) $(CPPFLAGS) $(FLAGS) -c $<

all: dpost

dpost: $(OBJ)
	$(CC) $(LDFLAGS) $(OBJ) $(LIBS) -o dpost

ps_include.h: ps_include.ps ps_include.awk
	rm -f $@; awk -f ps_include.awk ps_include.ps >$@

install:
	$(INSTALL) -c dpost $(ROOT)$(BINDIR)/dpost
	$(STRIP) $(ROOT)$(BINDIR)/dpost

clean:
	rm -f $(OBJ) dpost core ps_include.h log *~

mrproper: clean

color.o: color.c gen.h ext.h
dpost.o: dpost.c comments.h gen.h path.h ext.h ../dev.h dpost.h afm.h
dpost_draw.o: dpost_draw.c gen.h ext.h
glob.o: glob.c gen.h
misc.o: misc.c gen.h ext.h path.h
pictures.o: pictures.c comments.h gen.h path.h
ps_include.o: ps_include.c ps_include.h gen.h
request.o: request.c gen.h request.h path.h
dpost_afm.o: ../dev.h afm.h ../afm.c
dpost_makedev.o: ../dev.h ../makedev.c
