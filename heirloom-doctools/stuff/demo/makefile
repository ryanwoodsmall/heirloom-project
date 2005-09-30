.SUFFIXES: .tr .ps .pdf

.tr.ps:
	troff $(TFLAGS) $< | dpost >$@

.ps.pdf:
	ps2pdf -sPAPERSIZE=a4 $< $@

all: troffdemo.ps troffdemo_otf.ps troffdemo.pdf troffdemo_otf.pdf

clean:
	rm -f troffdemo.pdf troffdemo.ps troffdemo_otf.ps \
		troffdemo_otf.pdf core log *~
