
ifndef  YARP_ROOT
YARP_ROOT = ../..
endif


############################################################################
# slightly more user-friendly interface to a crufty latex makefile

# give the name (without extension) of the main tex file

MAIN = main

NAME = yarp-protocol-2-0

# give the name (without extension) of the main bibliography file

BMAIN = $(MAIN)

default: $(NAME).pdf

$(NAME).pdf: $(MAIN).pdf
	make clean
	make $(MAIN).pdf
	make clean
	make $(MAIN).pdf
	make clean
	make $(MAIN).pdf

# generate and view dvi file
dvi:
	make figs f dd


# generate and view ps file
ps:
	make figs f gv



# generate and view pdf file
pdf:
	make figs f $(MAIN).pdf && xpdf $(MAIN).pdf


# repeat latex/bibtex a few times to make sure everything is happy
happy:
	make figs
	make bounce

html:
	latex2html -split 0 main.tex

############################################################################
############################################################################
############################################################################
############################################################################
# now we delve into cruftiness

ifeq ($(shell uname),Linux)
GEOMETRY = -geometry +0+0
else
GEOMETRY =
endif

SUBMAIN = $(wildcard *.tex) $(wildcard *.sty)
JPEGS = $(wildcard *.jpg)
BMPS = $(wildcard *.bmp)
GIFS = $(wildcard *.gif)
PNGS = $(wildcard *.png)
EPSSRCS = $(wildcard *.epssrc)
PRNSRCS = $(wildcard *.prn)
PSSRCIS = $(wildcard *.pssrci)
PSSRCS = $(wildcard *.pssrc)
EPSPREV = $(wildcard *.eps)
EPSJPEG = $(patsubst %.jpg, %.eps, $(JPEGS))
EPSBMP = $(patsubst %.bmp, %.eps, $(BMPS))
EPSGIF = $(patsubst %.gif, %.eps, $(GIFS))
EPSPNG = $(patsubst %.png, %.eps, $(PNGS))
EPSSRC = $(patsubst %.epssrc, %.eps, $(EPSSRCS))
PSSRC = $(patsubst %.pssrc, %.eps, $(PSSRCS))  $(patsubst %.prn, %.eps, $(PRNSRCS))
PSSRCI = $(patsubst %.pssrci, %.eps, $(PSSRCIS))
EPS = $(EPSPREV) $(EPSJPEG) $(EPSGIF) $(EPSPNG) $(EPSSRC) $(PSSRC) $(PSSRCI) $(EPSBMP)


PDFFIGS = $(patsubst %.eps, %.pdf, $(EPS))

all: $(MAIN).dvi


%.pdf: %.eps
	epstopdf $^ --outfile=$@

%.pssrc: %.prn
	cp $< $@

%.eps: %.jpg
	convert $^ eps2:$@.tmp
	sed "s/statusdict/%%%statusdict/g" < $@.tmp > $@.tmp2
	sed "s/EndData/EndBinary/g" < $@.tmp2 > $@
	rm -f $@.tmp $@.tmp2

%.eps: %.gif
	convert $^ eps:$@.tmp
	sed "s/statusdict/%%%statusdict/g" < $@.tmp > $@.tmp2
	sed "s/EndData/EndBinary/g" < $@.tmp2 > $@
	rm -f $@.tmp $@.tmp2

%.eps: %.bmp
	convert $^ eps:$@.tmp
	sed "s/statusdict/%%%statusdict/g" < $@.tmp > $@.tmp2
	sed "s/EndData/EndBinary/g" < $@.tmp2 > $@
	rm -f $@.tmp $@.tmp2

%.eps: %.png
	convert $^ eps2:$@.tmp
	sed "s/statusdict/%%%statusdict/g" < $@.tmp > $@.tmp2
	sed "s/EndData/EndBinary/g" < $@.tmp2 > $@
	rm -f $@.tmp $@.tmp2

%.eps: %.epssrc
	cp $^ $@

%.eps: %.pssrc
ifeq ($(shell uname),Linux)
	@echo "On linux, so using straightforward eps generation method"
	ps2eps -l -f -C -s a3 -n -c $<
#	ps2eps -l -f -F -s a3 --rotate=+ $<
	mv $<.eps $@
else
	@echo "***"
	@echo "*** using devious eps generation method, be careful"
	@echo "***"
	grep -q "Title: Microsoft PowerPoint" $< && ps2eps $< || ps2eps -f -C  -n -c $<
	( sed "s/statusdict/%%%statusdict/g" | sed "s/^(%%\[[^ ]/%%(%%\[/" | grep -v "1 dict dup" | grep -v "ImagingBBox null" | sed "s/: N 111 /: %%N 111 /"  | sed "s/: N 803 /: %%N 803 /" ) < $<.eps > $@ && rm $<.eps
endif

#%.eps: %.pssrc
#	ps2eps $<
#	( sed "s/statusdict/%%%statusdict/g" | sed "s/^(%%\[[^ ]/%%(%%\[/" | sed "s/: N 111 /: %%N 111 /" ) < $<.eps > $@
#	rm -f $^.eps

%.eps: %.pssrci
	ps2epsi $<
	sed "s/statusdict/%%%statusdict/g" < $<.epsi > $@
	rm -f $<.epsi
#	mv $^.epsi $@

%.bbl: %.bib
#	latex $*
	bibtex $*

$(MAIN).pdf: $(PDFFIGS)
	pdflatex $(MAIN) && cp $(MAIN).pdf $(NAME).pdf


#$(MAIN).bbl: $(BMAIN).bib
#	latex $(MAIN)
#	cp $(MAIN).aux $(BMAIN).aux
#	bibtex $(BMAIN)

%.pdf: %.ps
	ps2pdf -dCompatibilityLevel=1.3 -dMaxSubsetPct=100 \
	-dSubsetFonts=true -dEmbedAllFonts=true \
	-dAutoFilterColorImages=false -dAutoFilterGrayImages=false \
	-dColorImageFilter=/FlateEncode -dGrayImageFilter=/FlateEncode \
	-dMonoImageFilter=/FlateEncode $< $@
#	ps2pdf $< $@
#	distill -v -compatlevel 2.1 -colordownsample on -graydownsample on -monodownsample on $^
#	distill -compatlevel 2.1 -colordownsample off -graydownsample off -monodownsample off $^

dd: $(MAIN).dvi
	xdvi $(GEOMETRY) $(MAIN).dvi

# need appropriate focus rules on win manager for this to work
#rep:
#	sh -c "while true do\
#	 ( make vision.dvi; cp vision.dvi no-focus.dvi; ( xdvi no-focus.dvi & ); echo Hi | tee rep; while -e rep do; ( make | grep latex && rm rep ); sleep 1; done; )  done"

figs: $(EPS)

d: $(MAIN).dvi
	xdvi -noghostscript $(GEOMETRY) $(MAIN).dvi
#	xdvi $(MAIN).dvi

gv: $(MAIN).ps
	gv $(GEOMETRY) $(MAIN).ps

p: $(MAIN).ps

$(MAIN).half.ps: $(MAIN).ps
	mpage -2 < $< > $@

p2: $(MAIN).half.ps

pdf2: $(MAIN).half.pdf

b: $(MAIN).bbl

fb:
	rm -f $(MAIN).bbl
	make $(MAIN).bbl

f:
	rm -f $(MAIN).pdf
	rm -f $(MAIN).dvi

wc:
	cat *.tex | wc

join:
	cat *.tex | egrep -v "^\%"

miss:
	@egrep "(Warning: ((Reference)|(Citation)))|(\.tex)" $(MAIN).log

bounce:
	make fb ; make force ; make fb ; make force ; make fb ; make force

$(MAIN).dvi: $(MAIN).tex $(EPS) $(MAIN).bbl $(SUBMAIN)
	echo -n > /tmp/view.update
	latex $(MAIN).tex

force:
	latex $(MAIN).tex

$(MAIN).ps: $(MAIN).dvi
#	dvips -t a4 -f $< > $@
#	dvips -t letter -f $< > $@
	dvips -Ppdf -t a4 -G0 -f $< > $@
#	dvips -Ppdf -t letter -G0 -f $< > $@

short:
	cp defs-iros.tex defs.tex
	make bounce

long:
	cp defs-epi.tex defs.tex
	make bounce

commit:
	cvs commit -m "Makefile-generated commit"

c: commit

update:
	cvs update

u: update

# only works on a sun, with distill installed
#pdf: $(MAIN).pdf

clean:
	rm -f $(MAIN).dvi $(MAIN).ps $(MAIN).pdf $(NAME).pdf


include $(YARP_ROOT)/conf/Makefile.dummy.template




