CC=gcc
CFLAGS=-Wall -std=c99 -pedantic

clark_make: clark_make.o
	$(CC) -o clark_make clark_make.o
clark_make.o: clark_make.c clark_make.h
	$(CC) $(CFLAGS) -c clark_make.c clark_make.h

demo: clark_make
	./clark_make

clean:
	rm *.o *.h.gch clark_make

