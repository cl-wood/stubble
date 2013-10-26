CFLAGS=-Wall -pedantic -std=c99 -O3

#demo: jacobi.c openmp.c pthreads.c multiproc.c
demo: jacobi.c openmp.c pthreads.c
	gcc $(CFLAGS) -o seq jacobi.c
	gcc $(CFLAGS) -fopenmp -o openmp openmp.c
	gcc $(CFLAGS) -pthread -o pthreads pthreads.c
	#gcc $(CFLAGS) -o multiproc multiproc.c

clean:
	rm -f jacobi.o seq 
	rm -f openmp.o openmp.output openmp
	rm -f pthreads.o pthreads.output pthreads
	rm -f multiproc.o multiproc.output multiproc

