CC=gcc
CFLAGS=-Wall -std=c99 -pedantic

a.out: file_organizer.o
	$(CC) file_organizer.o
file_organizer.o: file_organizer.c file_organizer.h
	$(CC) $(CFLAGS) -c file_organizer.c file_organizer.h

clean:
	rm *.o *.h.gch a.out

