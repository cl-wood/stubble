CC=gcc
CFLAGS=-Wall -std=c99 -pedantic

a.out: clark_makefile.o
	$(CC) clark_makefile.o
clark_makefile.o: clark_makefile.c clark_makefile.h
	$(CC) $(CFLAGS) -c clark_makefile.c clark_makefile.h

#demo:

clean:
	rm *.o *.h.gch a.out

