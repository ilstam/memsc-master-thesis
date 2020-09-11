TEX = pdflatex
BIB = bibtex

file = main

all:
	$(TEX) $(file)
	$(BIB) $(file)
	$(TEX) $(file)
	$(TEX) $(file)

clean:
	rm -f $(file).aux $(file).bbl $(file).blg $(file)-blx.bib $(file).lof $(file).log $(file).lot $(file).run.xml $(file).toc $(file).out $(file).pdf
