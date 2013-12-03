SHELL=/bin/bash

SRC = thesis.tex \
	math.tex \
	citations.tex \
	figures.tex \
	tables.tex \
	appendix.tex \
        history.tex \
	tests.tex

all: thesis.pdf

# The first run...
thesis.aux: $(SRC) fsuthesis.cls
	-@rm thesis.{aux,toc,dvi,lof,lot,log,lom,bbl,blg,out}
	latex thesis
	-@rm thesis.{toc,dvi,lof,lot,log,lom,bbl,blg,out}

thesis.bbl: $(SRC) myrefs.bib thesis.aux
	bibtex thesis

# The final run...
thesis.pdf: $(SRC) thesis.bbl
	-@rm thesis.pdf
	-@pdflatex -interaction=nonstopmode thesis
	pdflatex thesis
	pdflatex thesis
	-@touch thesis.aux thesis.bbl
	-@if [ -f thesis.pdf ]; then touch thesis.pdf; fi

thesis.dvi: $(SRC) thesis.bbl
	-@rm thesis.dvi
	-@latex -interaction=nonstopmode thesis
	latex thesis
	latex thesis
	-@touch thesis.aux thesis.bbl
	-@if [ -f thesis.dvi ]; then touch thesis.dvi; fi

thesis.ps: thesis.dvi
	dvips -o thesis.ps thesis.dvi

clean:
	-@rm thesis.{aux,toc,dvi,lof,lot,log,lom,bbl,blg,out}

veryclean: clean
	-@rm thesis.{ps,pdf} *~

distclean: clean
	-@rm thesis.ps *~
