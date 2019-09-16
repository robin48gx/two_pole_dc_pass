

two_pole:
	gcc -o tp two_pole_7_8.c -lm
	./tp > tp.dat
	vi tp.dat
	gnuplot < tp.gpt


fa: freq_analysis.c 
	gcc freq_analysis.c -o fa -lm
	./fa > fa.dat
	vi fa.dat
	gnuplot < fa.gpt
