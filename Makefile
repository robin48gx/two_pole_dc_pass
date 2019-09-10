

two_pole:
	gcc -o tp two_pole_7_8.c -lm
	./tp > tp.dat
	vi tp.dat
	gnuplot < tp.gpt
