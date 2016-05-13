$DATA << EOD
#Task start      end
0-1   01.02.2016 02.02.2016
1-2   02.02.2016 08.02.2016
2-3   08.02.2016 11.02.2016
3-4   11.02.2016 18.02.2016
4-5   18.02.2016 24.02.2016
4-6   18.02.2016 25.02.2016
6-7   25.02.2016 01.03.2016
7-8   01.03.2016 10.03.2016
8-9   10.03.2016 17.03.2016
8-10  10.03.2016 21.03.2016
10-11 21.03.2016 29.03.2016
11-12 29.03.2016 05.04.2016
12-13 05.04.2016 28.04.2016
13-14 28.04.2016 04.05.2016
EOD

set xdata time
set grid xtics
unset key

T(N) = timecolumn(N,timeformat)
timeformat = "%d.%m.%Y"

set style line 1 lt 1 lw 8
set style arrow 1 nohead size screen 0.008,90 ls 1

set terminal postscript color
set output "| echo > /dev/null"
plot $DATA using (T(2)) : ($0) : (T(3)-T(2)) : (0.0) : yticlabel(1) with vectors arrowstyle 1

set terminal postscript
set output "| ps2pdf - gantt.pdf"

set yr [GPVAL_DATA_Y_MIN-1:GPVAL_DATA_Y_MAX+1]
replot
