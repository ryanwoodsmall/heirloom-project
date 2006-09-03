MACS = acm.me bib chars.me deltext.me e eqn.me \
	float.me footnote.me index.me local.me m mmn mmt ms.acc \
	ms.cov ms.eqn ms.ref ms.tbl ms.ths ms.toc null.me refer.me \
	s sh.me tbl.me thesis.me v vgrind \
	an andoc doc doc-common doc-ditroff doc-nroff doc-syms \
	pictures color pm srefs ptx safe g

.SUFFIXES: .in
.in:
	sed 's:@MACDIR@:$(MACDIR):; s:@LIBDIR@:$(LIBDIR):' $< >$@

all: $(MACS)

install: all
	test -d $(ROOT)$(MACDIR) || mkdir -p $(ROOT)$(MACDIR)
	for i in $(MACS); \
	do \
		$(INSTALL) -c -m 644 $$i $(ROOT)$(MACDIR)/$$i || exit; \
	done
	test -d $(ROOT)$(MANDIR)/man7b || mkdir -p $(ROOT)$(MANDIR)/man7b
	$(INSTALL) -c -m 644 mcolor.7b $(ROOT)$(MANDIR)/man7b/mcolor.7b
	$(INSTALL) -c -m 644 mpictures.7b $(ROOT)$(MANDIR)/man7b/mpictures.7b

clean:
	rm -f andoc bib doc e m s pm

mrproper: clean
