OUTPUT=acaros
HTML_AUTHOR="Marko Mikulicic"
LATEX_HTML_OPTIONS=-html_version 4.0,frame,table -white -antialias -antialias_text -local_icons \
	 -show_section_numbers -scalable_fonts -address $(HTML_AUTHOR) -transparent \
	 -accent_images textrm

#
FILES := $(wildcard *.tex)
FIGURES := $(patsubst %.fig,%.pdf,$(wildcard *.fig)) $(patsubst %.eps,%.pdf,$(wildcard *.eps))

default: $(OUTPUT).pdf

all: $(OUTPUT).pdf $(OUTPUT).ps html api

$(OUTPUT).dvi: $(FILES) $(FIGURES)
	@latex $(OUTPUT)
	@makeindex $(OUTPUT)

$(OUTPUT).pdf: $(FILES) $(FIGURES)
	@pdflatex $(OUTPUT)
	@makeindex $(OUTPUT)

$(OUTPUT).ps: $(OUTPUT).dvi
	@dvips $(OUTPUT).dvi -o $(OUTPUT).ps

html:	$(OUTPUT).pdf
	@latex2html $(LATEX_HTML_OPTIONS) $(OUTPUT)

.PHONY: api

api:
	$(MAKE) -C api

clean:
	rm *~ *.pdf *.aux *.log *.toc *.ps *.dvi *.out *.lof *.lot *.idx *.ilg *.ind *.bak $(OUTPUT)/ -fr 

.PRECIOUS: %.eps # avoid auto deletion

%.eps: %.fig
	fig2dev -L eps $< >$@

%.pdf: %.eps
	epstopdf $<
