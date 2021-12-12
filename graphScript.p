# setup the output
set term png
set output "Performance.png"

# add axis labels
set xlabel "Number of Pixels on Side"
set ylabel "Time(ms)"

# plot the data
plot "performance.txt" using 1:3 with lines title "Edmonds Karp",\
     "performance.txt" using 1:(0.02*$1*$1*$2) with lines title "O(V*E)"