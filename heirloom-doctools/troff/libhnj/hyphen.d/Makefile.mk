all:

install: all
	test -d $(ROOT)$(HYPDIR) || mkdir -p $(ROOT)$(HYPDIR)
	for i in *.dic; \
	do \
		$(INSTALL) -c -m 644 $$i $(ROOT)$(HYPDIR)/$$i || exit; \
	done
	rm -f $(ROOT)$(HYPDIR)/hyph_de_CH.dic
	ln -s hyph_de_DE.dic $(ROOT)$(HYPDIR)/hyph_de_CH.dic
	rm -f $(ROOT)$(HYPDIR)/hyph_de_CH@traditional.dic
	ln -s hyph_de_DE@traditional.dic $(ROOT)$(HYPDIR)/hyph_de_CH@traditional.dic
	rm -f $(ROOT)$(HYPDIR)/hyph_en_CA.dic
	ln -s hyph_en_US.dic $(ROOT)$(HYPDIR)/hyph_en_CA.dic
	rm -f $(ROOT)$(HYPDIR)/hyph_en_GB.dic
	ln -s hyph_en_US.dic $(ROOT)$(HYPDIR)/hyph_en_GB.dic
	rm -f $(ROOT)$(HYPDIR)/hyph_en_AU.dic
	ln -s hyph_en_US.dic $(ROOT)$(HYPDIR)/hyph_en_AU.dic
	rm -f $(ROOT)$(HYPDIR)/hyph_fr_CA.dic
	ln -s hyph_fr_FR.dic $(ROOT)$(HYPDIR)/hyph_fr_CA.dic

clean:

mrproper: clean
