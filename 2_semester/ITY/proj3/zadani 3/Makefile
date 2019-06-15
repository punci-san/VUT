PROJ=proj3
PACK=xskuta04-fit

.PHONY: clean pack pdflatex

$(PROJ).pdf: $(PROJ).ps
	ps2pdf -sPAPERSIZE=a4 $^

$(PROJ).ps: $(PROJ).dvi
	dvips $^

$(PROJ).dvi: $(PROJ).tex
	latex $^
	latex $^

$(PACK).tar.gz: $(PROJ).tex etiopan.eps oniisan.eps oniisan2.eps algorithm2e.sty
	tar -czf $@ $^ Makefile

pdflatex: $(PROJ).tex
	pdflatex $^

pack: clean $(PACK).tar.gz

clean:
	rm -f $(PROJ).aux $(PROJ).dvi $(PROJ).log $(PROJ).ps $(PROJ).synctex.gz $(PROJ).fls $(PROJ).fdb_latexmk $(wildcard $(IMAGES_DIRECTORY)/*.pdf)
