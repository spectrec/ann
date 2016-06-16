#!/bin/sh

dir=`dirname $1`
file=`basename -s .dia $1`

dia="$dir/$file.dia"
eps="$dir/$file.eps"
pdf="$dir/$file.pdf"

dia -e $eps -t eps-pango $dia
epstopdf --outfile $pdf $eps
