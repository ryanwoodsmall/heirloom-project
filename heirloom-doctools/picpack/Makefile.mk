OBJ = picpack.o getopt.o

FLAGS = -I../troff/troff.d/dpost.d

.c.o:
	$(CC) $(CFLAGS) $(WARN) $(CPPFLAGS) $(FLAGS) -c $<

all: picpack

picpack: $(OBJ)
	$(CC) $(LDFLAGS) $(OBJ) $(LIBS) -o picpack

install:
	$(INSTALL) -c picpack $(ROOT)$(BINDIR)/picpack
	$(STRIP) $(ROOT)$(BINDIR)/picpack
	$(INSTALL) -c -m 644 picpack.1b $(ROOT)$(MANDIR)/man1b/picpack.1b

clean:
	rm -f $(OBJ) picpack core log *~

mrproper: clean
