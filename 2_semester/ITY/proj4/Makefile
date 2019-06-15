PROJ=proj4
PACK=xskuta04-fit

.PHONY: clean pack bibtex pdflatex

$(PROJ).pdf: $(PROJ).ps
	ps2pdf -sPAPERSIZE=a4 $^

$(PROJ).ps: $(PROJ).dvi
	dvips $^

$(PROJ).dvi: $(PROJ).tex
	latex $^
	make bibtex
	latex $^
	latex $^

bibtex: $(PROJ).bib
	bibtex $(PROJ)

pdflatex: $(PROJ).tex
	pdflatex $^
	make bibtex
	pdflatex $^
	pdflatex $^

pack: $(PACK).tar.gz

$(PACK).tar.gz: $(PROJ).tex $(PROJ).bib czechiso.bst
	tar -czf $@ $^ Makefile

clean:
	rm -f $(PROJ).aux $(PROJ).dvi $(PROJ).log $(PROJ).ps $(PROJ).synctex.gz $(PROJ).fls $(PROJ).fdb_latexmk $(PROJ).bbl $(PROJ).blg

