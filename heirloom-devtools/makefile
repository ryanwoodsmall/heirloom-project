SUBDIRS = yacc lex m4 \
	sccs/cassi sccs/comobj sccs/mpwlib sccs/src sccs/help.d sccs/man \
	make/bsd make/makestate make/mksdmsi18n make/mksh make/vroot make/src

MAKEFILES = $(SUBDIRS:=/Makefile)

.SUFFIXES: .mk
.mk:
	cat mk.config $< >$@

dummy: $(MAKEFILES) all

makefiles: $(MAKEFILES)

.DEFAULT:
	+ for i in $(SUBDIRS); \
	do \
		(cd "$$i" && $(MAKE) $@) || exit; \
	done

mrproper: clean
	+ for i in $(SUBDIRS); \
	do \
		(cd "$$i" && $(MAKE) $@) || exit; \
	done
	rm -f $(MAKEFILES)

sun:
	/usr/xpg4/bin/make CXX=CC CFLAGS=-O CXXFLAGS=-O WARN=
