PRESENTATION=presentation
SPEECH=speech

all: dia ${PRESENTATION}.pdf ${SPEECH}.pdf

dia:
	for file in dia/*.dia; do ./dia2pdf.sh $$file; done

${PRESENTATION}.pdf: ${PRESENTATION}.tex
	pdflatex $< && pdflatex $<

${SPEECH}.pdf: ${SPEECH}.tex
	pdflatex $<

clean:
	rm -f *.{log,out,aux}
	rm -f ${PRESENTATION}.pdf ${SPEECH}.pdf
	rm -f dia/*.{eps,pdf}

.PHONY: all clean dia
