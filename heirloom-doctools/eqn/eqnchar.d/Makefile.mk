FILES = ascii eqnchar greek iso

all:

install:
	test -d $(ROOT)$(PUBDIR) || mkdir -p $(ROOT)$(PUBDIR)
	for i in $(FILES); \
	do \
		$(INSTALL) -c -m 644 $$i $(ROOT)$(PUBDIR)/$$i || exit; \
	done

clean:

mrproper: clean
