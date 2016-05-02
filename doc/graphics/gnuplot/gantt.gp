$DATA << EOD
#Task start      end
0-1   01.02.2016 02.02.2016
1-2   02.02.2016 08.02.2016
2-3   08.02.2016 10.02.2016
3-4   10.02.2016 17.02.2016
4-5   17.02.2016 19.02.2016
4-6   17.02.2016 22.02.2016
6-7   22.02.2016 26.02.2016
7-8   26.02.2016 07.03.2016
8-9   07.03.2016 11.03.2016
8-10  07.03.2016 15.03.2016
10-11 16.03.2016 24.03.2016
11-12 25.03.2016 31.03.2016
12-13 01.04.2016 25.04.2016
13-14 25.04.2016 28.04.2016
EOD

set xdata time
set grid xtics
unset key

T(N) = timecolumn(N,timeformat)
timeformat = "%d.%m.%Y"

set terminal postscript color
set output "| echo > /dev/null"
plot $DATA using (T(2)) : ($0) : (T(3)-T(2)) : (0.0) : yticlabel(1) with vectors

set terminal postscript
set output "| ps2pdf - gantt.pdf"

set yr [GPVAL_DATA_Y_MIN-1:GPVAL_DATA_Y_MAX+1]
replot
