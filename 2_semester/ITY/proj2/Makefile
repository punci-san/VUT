PROJ=proj2
PACK=xskuta04-fit

.PHONY: clean pack

$(PROJ).dvi: $(PROJ).tex
	pdflatex $(PROJ).tex
	pdflatex $(PROJ).tex

$(PACK).tar.gz: $(PROJ).tex
	tar -czf $(PACK).tar.gz Makefile $(PROJ).tex

pack: $(PACK).tar.gz

clean:
	rm -f $(PROJ).aux $(PROJ).log

