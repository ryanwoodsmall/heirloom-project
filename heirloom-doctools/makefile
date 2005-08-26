SUBDIRS = eqn/eqn.d \
	eqn/neqn.d \
	eqn/checkeq.d \
	eqn/eqnchar.d \
	tbl \
	troff/libhnj \
	troff/libhnj/hyphen.d \
	troff/troff.d/font \
	troff/troff.d/dpost.d \
	troff/troff.d \
	troff/troff.d/devaps \
	troff/troff.d/tmac.d \
	troff/troff.d/postscript \
	troff/nroff.d \
	troff/nroff.d/terms.d \
	picpack \
	soelim

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
