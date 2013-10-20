demo: jacobi.o openmp.o
	gcc -Wall -o seq jacobi.o
	gcc -Wall -fopenmp -o openmp openmp.o

jacobi.o: jacobi.c
	gcc -c -Wall jacobi.c
openmp.o: openmp.c
	gcc -c -Wall -pedantic -std=c99 -fopenmp openmp.c

clean:
	rm -f jacobi.o seq 
	rm -f openmp.o openmp

