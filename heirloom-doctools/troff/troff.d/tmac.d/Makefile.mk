MACS = acm.me bib chars.me deltext.me e eqn.me \
	float.me footnote.me index.me local.me m mmn mmt ms.acc \
	ms.cov ms.eqn ms.ref ms.tbl ms.ths ms.toc null.me refer.me \
	s sh.me tbl.me thesis.me tz.map v vgrind \
	an andoc doc doc-common doc-ditroff doc-nroff doc-syms

.SUFFIXES: .in
.in:
	sed 's,@MACDIR@,$(MACDIR),' $< >$@

all: $(MACS)

install: all
	test -d $(ROOT)$(MACDIR) || mkdir -p $(ROOT)$(MACDIR)
	for i in $(MACS); \
	do \
		$(INSTALL) -c -m 644 $$i $(ROOT)$(MACDIR)/$$i || exit; \
	done

clean:
	rm -f andoc bib doc e m s

mrproper: clean
