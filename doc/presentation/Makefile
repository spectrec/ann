PRESENTATION=presentation
SPEECH=speech

all: ${PRESENTATION}.pdf ${SPEECH}.pdf

${PRESENTATION}.pdf: ${PRESENTATION}.tex
	pdflatex $< && pdflatex $<

${SPEECH}.pdf: ${SPEECH}.tex
	pdflatex $<

clean:
	rm -f *.{log,out,aux}
	rm -f ${PRESENTATION}.pdf ${SPEECH}.pdf

.PHONY: all clean
