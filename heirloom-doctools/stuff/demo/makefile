.SUFFIXES: .tr .ps .pdf

# You need at least Ghostscript 8.53 to create PDF/X documents.
DISTFLAGS = -q -dBATCH -dNOPAUSE -dNOOUTERSAVE -sDEVICE=pdfwrite \
	-sProcessColorModel=DeviceCMYK -dPDFX

.tr.ps:
	troff $(TFLAGS) $< | dpost >$@

.ps.pdf:
	gs $(DISTFLAGS) -sOutputFile=$@ PDFX_def.ps $<

all: troffdemo.ps troffdemo_otf.ps troffdemo.pdf troffdemo_otf.pdf

troffdemo_otf.ps: troffdemo.tr troffdemo_otf.tr

clean:
	rm -f troffdemo.pdf troffdemo.ps troffdemo_otf.ps \
		troffdemo_otf.pdf core log *~
