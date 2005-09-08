.SUFFIXES: .tr .ps .pdf

.tr.ps:
	troff $(TFLAGS) $< | dpost >$@

.ps.pdf:
	ps2pdf -sPAPERSIZE=a4 $< $@

all: troffdemo.ps troffdemo.pdf

clean:
	rm -f troffdemo.pdf troffdemo.ps core log *~
