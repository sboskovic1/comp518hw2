CC=gcc

plots: main.c
	$(CC) -O0 main.c -o main.o -lm
	./main.o
	Rscript draw-plots.r
