#! /usr/bin/gnuplot -persist

set terminal pdfcairo enhanced
set output "cost_pie_chart.pdf"
set encoding utf8

$DATA << EOD
"Организация рабочих мест" 19500
"Закупка и аренда оборудования" 38470
"Заработная плата исполнителям" 562386.01
"Накладные расходы" 216302.31
EOD

stats $DATA using 2 noout       # get STATS_sum (sum of column 2)
ang(x)=x*360.0/STATS_sum        # get angle (grades)
perc(x)=x*100.0/STATS_sum       # get percentage

set size square                 # square canvas
set xrange [-1:2.6]
set yrange [-1.25:1.25]
set style fill solid 1

unset border
unset tics
unset key

Ai = 0.0; Bi = 0.0;             # init angle
mid = 0.0;                      # mid angle
i = 0; j = 0;                   # color
yi  = 0.0; yi2 = 0.0;           # label position

plot $DATA using (0):(0):(1):(Ai):(Ai=Ai+ang($2)):(i=i+1) with circle linecolor var, \
     $DATA using (2.6):(yi=yi+0.5/STATS_records):($1) with labels, \
     $DATA using (1.3):(yi2=yi2+0.5/STATS_records):(j=j+1) with p pt 5 ps 2 linecolor var
