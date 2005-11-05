.SUFFIXES: .tr .ps .pdf

.tr.ps:
	troff $(TFLAGS) $< | dpost >$@

.ps.pdf:
	ps2pdf13 -sPAPERSIZE=a4 $< $@

all: troffdemo.ps troffdemo_otf.ps troffdemo.pdf troffdemo_otf.pdf

troffdemo_otf.ps: troffdemo.tr troffdemo_otf.tr

clean:
	rm -f troffdemo.pdf troffdemo.ps troffdemo_otf.ps \
		troffdemo_otf.pdf core log *~
