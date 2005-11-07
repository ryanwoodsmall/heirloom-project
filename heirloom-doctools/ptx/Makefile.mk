OBJ = ptx.o

FLAGS = -DLIBDIR='"$(LIBDIR)"' $(EUC)

.c.o:
	$(CC) $(CFLAGS) $(WARN) $(CPPFLAGS) $(FLAGS) -c $<

all: ptx

ptx: $(OBJ)
	$(CC) $(LDFLAGS) $(OBJ) $(LIBS) -o ptx

install:
	$(INSTALL) -c ptx $(ROOT)$(BINDIR)/ptx
	$(STRIP) $(ROOT)$(BINDIR)/ptx
	$(INSTALL) -c -m 644 ptx.1b $(ROOT)$(MANDIR)/man1b/ptx.1b
	test -d $(LIBDIR) || mkdir -p $(LIBDIR)
	$(INSTALL) -c -m 644 eign $(LIBDIR)/eign

clean:
	rm -f $(OBJ) ptx core log *~

mrproper: clean
